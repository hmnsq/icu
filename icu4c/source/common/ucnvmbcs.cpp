// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2000-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  ucnvmbcs.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2000jul03
*   created by: Markus W. Scherer
*
*   The current code in this file replaces the previous implementation
*   of conversion code from multi-byte codepages to Unicode and back.
*   This implementation supports the following:
*   - legacy variable-length codepages with up to 4 bytes per character
*   - all Unicode code points (up to 0x10ffff)
*   - efficient distinction of unassigned vs. illegal byte sequences
*   - it is possible in fromUnicode() to directly deal with simple
*     stateful encodings (used for EBCDIC_STATEFUL)
*   - it is possible to convert Unicode code points
*     to a single zero byte (but not as a fallback except for SBCS)
*
*   Remaining limitations in fromUnicode:
*   - byte sequences must not have leading zero bytes
*   - except for SBCS codepages: no fallback mapping from Unicode to a zero byte
*   - limitation to up to 4 bytes per character
*
*   ICU 2.8 (late 2003) adds a secondary data structure which lifts some of these
*   limitations and adds m:n character mappings and other features.
*   See ucnv_ext.h for details.
*
*   Change history: 
*
*    5/6/2001       Ram       Moved  MBCS_SINGLE_RESULT_FROM_U,MBCS_STAGE_2_FROM_U,
*                             MBCS_VALUE_2_FROM_STAGE_2, MBCS_VALUE_4_FROM_STAGE_2
*                             macros to ucnvmbcs.h file
*/

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/ucnv_cb.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnvmbcs.h"
#अन्तर्भाव "ucnv_ext.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "ustr_imp.h"

/* control optimizations according to the platform */
#परिभाषय MBCS_UNROLL_SINGLE_गन्तव्यम्_BMP 1
#परिभाषय MBCS_UNROLL_SINGLE_FROM_BMP 0

/*
 * _MBCSHeader versions 5.3 & 4.3
 * (Note that the _MBCSHeader version is in addition to the converter formatVersion.)
 *
 * This version is optional. Version 5 is used for incompatible data format changes.
 * makeconv will continue to generate version 4 files if possible.
 *
 * Changes from version 4:
 *
 * The main difference is an additional _MBCSHeader field with
 * - the length (number of uint32_t) of the _MBCSHeader
 * - flags for further incompatible data format changes
 * - flags for further, backward compatible data format changes
 *
 * The MBCS_OPT_FROM_U flag indicates that most of the fromUnicode data is omitted from
 * the file and needs to be reconstituted at load time.
 * This requires a utf8Friendly format with an additional mbcsIndex table for fast
 * (and UTF-8-friendly) fromUnicode conversion for Unicode code points up to maxFastUChar.
 * (For details about these structures see below, and see ucnvmbcs.h.)
 *
 *   utf8Friendly also implies that the fromUnicode mappings are stored in ascending order
 *   of the Unicode code points. (This requires that the .ucm file has the |0 etc.
 *   precision markers for all mappings.)
 *
 *   All fallbacks have been moved to the extension table, leaving only roundtrips in the
 *   omitted data that can be reconstituted from the toUnicode data.
 *
 *   Of the stage 2 table, the part corresponding to maxFastUChar and below is omitted.
 *   With only roundtrip mappings in the base fromUnicode data, this part is fully
 *   redundant with the mbcsIndex and will be reconstituted from that (also using the
 *   stage 1 table which contains the information about how stage 2 was compacted).
 *
 *   The rest of the stage 2 table, the part for code points above maxFastUChar,
 *   is stored in the file and will be appended to the reconstituted part.
 *
 *   The entire fromUBytes array is omitted from the file and will be reconstitued.
 *   This is done by enumerating all toUnicode roundtrip mappings, performing
 *   each mapping (using the stage 1 and reconstituted stage 2 tables) and
 *   writing instead of reading the byte values.
 *
 * _MBCSHeader version 4.3
 *
 * Change from version 4.2:
 * - Optional utf8Friendly data structures, with 64-entry stage 3 block
 *   allocation for parts of the BMP, and an additional mbcsIndex in non-SBCS
 *   files which can be used instead of stages 1 & 2.
 *   Faster lookups for roundtrips from most commonly used characters,
 *   and lookups from UTF-8 byte sequences with a natural bit distribution.
 *   See ucnvmbcs.h for more details.
 *
 * Change from version 4.1:
 * - Added an optional extension table structure at the end of the .cnv file.
 *   It is present if the upper bits of the header flags field contains a non-zero
 *   byte offset to it.
 *   Files that contain only a conversion table and no base table
 *   use the special outputType MBCS_OUTPUT_EXT_ONLY.
 *   These contain the base table name between the MBCS header and the extension
 *   data.
 *
 * Change from version 4.0:
 * - Replace header.reserved with header.fromUBytesLength so that all
 *   fields in the data have length.
 *
 * Changes from version 3 (for performance improvements):
 * - new bit distribution for state table entries
 * - reordered action codes
 * - new data structure for single-byte fromUnicode
 *   + stage 2 only contains indexes
 *   + stage 3 stores 16 bits per character with classification bits 15..8
 * - no multiplier for stage 1 entries
 * - stage 2 for non-single-byte codepages contains the index and the flags in
 *   one 32-bit value
 * - 2-byte and 4-byte fromUnicode results are stored directly as 16/32-bit integers
 *
 * For more details about old versions of the MBCS data structure, see
 * the corresponding versions of this file.
 *
 * Converting stateless codepage data ---------------------------------------***
 * (or codepage data with simple states) to Unicode.
 *
 * Data structure and algorithm for converting from complex legacy codepages
 * to Unicode. (Designed before 2000-may-22.)
 *
 * The basic idea is that the structure of legacy codepages can be described
 * with state tables.
 * When reading a byte stream, each input byte causes a state transition.
 * Some transitions result in the output of a code point, some result in
 * "unassigned" or "illegal" output.
 * This is used here for character conversion.
 *
 * The data structure begins with a state table consisting of a row
 * per state, with 256 entries (columns) per row for each possible input
 * byte value.
 * Each entry is 32 bits wide, with two formats distinguished by
 * the sign bit (bit 31):
 *
 * One format for transitional entries (bit 31 not set) for non-final bytes, and
 * one format for final entries (bit 31 set).
 * Both formats contain the number of the next state in the same bit
 * positions.
 * State 0 is the initial state.
 *
 * Most of the time, the offset values of subsequent states are added
 * up to a scalar value. This value will eventually be the index of
 * the Unicode code point in a table that follows the state table.
 * The effect is that the code points for final state table rows
 * are contiguous. The code points of final state rows follow each other
 * in the order of the references to those final states by previous
 * states, etc.
 *
 * For some terminal states, the offset is itself the output Unicode
 * code point (16 bits for a BMP code point or 20 bits for a supplementary
 * code point (stored as code point minus 0x10000 so that 20 bits are enough).
 * For others, the code point in the Unicode table is stored with either
 * one or two code units: one for BMP code points, two for a pair of
 * surrogates.
 * All code points for a final state entry take up the same number of code
 * units, regardless of whether they all actually _use_ the same number
 * of code units. This is necessary for simple array access.
 *
 * An additional feature comes in with what in ICU is called "fallback"
 * mappings:
 *
 * In addition to round-trippable, precise, 1:1 mappings, there are often
 * mappings defined between similar, though not the same, characters.
 * Typically, such mappings occur only in fromUnicode mapping tables because
 * Unicode has a superset repertoire of most other codepages. However, it
 * is possible to provide such mappings in the toUnicode tables, too.
 * In this case, the fallback mappings are partly integrated into the
 * general state tables because the structure of the encoding includes their
 * byte sequences.
 * For final entries in an initial state, fallback mappings are stored in
 * the entry itself like with roundtrip mappings.
 * For other final entries, they are stored in the code units table if
 * the entry is for a pair of code units.
 * For single-unit results in the code units table, there is no space to
 * alternatively hold a fallback mapping; in this case, the code unit
 * is stored as U+fffe (unassigned), and the fallback mapping needs to
 * be looked up by the scalar offset value in a separate table.
 *
 * "Unassigned" state entries really mean "structurally unassigned",
 * i.e., such a byte sequence will never have a mapping result.
 *
 * The interpretation of the bits in each entry is as follows:
 *
 * Bit 31 not set, not a terminal entry ("transitional"):
 * 30..24 next state
 * 23..0  offset delta, to be added up
 *
 * Bit 31 set, terminal ("final") entry:
 * 30..24 next state (regardless of action code)
 * 23..20 action code:
 *        action codes 0 and 1 result in precise-mapping Unicode code points
 *        0  valid byte sequence
 *           19..16 not used, 0
 *           15..0  16-bit Unicode BMP code point
 *                  never U+fffe or U+ffff
 *        1  valid byte sequence
 *           19..0  20-bit Unicode supplementary code point
 *                  never U+fffe or U+ffff
 *
 *        action codes 2 and 3 result in fallback (unidirectional-mapping) Unicode code points
 *        2  valid byte sequence (fallback)
 *           19..16 not used, 0
 *           15..0  16-bit Unicode BMP code point as fallback result
 *        3  valid byte sequence (fallback)
 *           19..0  20-bit Unicode supplementary code point as fallback result
 *
 *        action codes 4 and 5 may result in roundtrip/fallback/unassigned/illegal results
 *        depending on the code units they result in
 *        4  valid byte sequence
 *           19..9  not used, 0
 *            8..0  final offset delta
 *                  pointing to one 16-bit code unit which may be
 *                  fffe  unassigned -- look for a fallback for this offset
 *                  ffff  illegal
 *        5  valid byte sequence
 *           19..9  not used, 0
 *            8..0  final offset delta
 *                  pointing to two 16-bit code units
 *                  (typically UTF-16 surrogates)
 *                  the result depends on the first code unit as follows:
 *                  0000..d7ff  roundtrip BMP code point (1st alone)
 *                  d800..dbff  roundtrip surrogate pair (1st, 2nd)
 *                  dc00..dfff  fallback surrogate pair (1st-400, 2nd)
 *                  e000        roundtrip BMP code point (2nd alone)
 *                  e001        fallback BMP code point (2nd alone)
 *                  fffe        unassigned
 *                  ffff        illegal
 *           (the final offset deltas are at most 255 * 2,
 *            times 2 because of storing code unit pairs)
 *
 *        6  unassigned byte sequence
 *           19..16 not used, 0
 *           15..0  16-bit Unicode BMP code point U+fffe (new with version 2)
 *                  this does not contain a final offset delta because the main
 *                  purpose of this action code is to save scalar offset values;
 *                  therefore, fallback values cannot be assigned to byte
 *                  sequences that result in this action code
 *        7  illegal byte sequence
 *           19..16 not used, 0
 *           15..0  16-bit Unicode BMP code point U+ffff (new with version 2)
 *        8  state change only
 *           19..0  not used, 0
 *           useful for state changes in simple stateful encodings,
 *           at Shift-In/Shift-Out codes
 *
 *
 *        9..15 reserved for future use
 *           current implementations will only perform a state change
 *           and ignore bits 19..0
 *
 * An encoding with contiguous ranges of unassigned byte sequences, like
 * Shift-JIS and especially EUC-TW, can be stored efficiently by having
 * at least two states for the trail bytes:
 * One trail byte state that results in code points, and one that only
 * has "unassigned" and "illegal" terminal states.
 *
 * Note: partly by accident, this data structure supports simple stateful
 * encodings without any additional logic.
 * Currently, only simple Shift-In/Shift-Out schemes are handled with
 * appropriate state tables (especially EBCDIC_STATEFUL!).
 *
 * MBCS version 2 added:
 * unassigned and illegal action codes have U+fffe and U+ffff
 * instead of unused bits; this is useful for _MBCS_SINGLE_SIMPLE_GET_NEXT_BMP()
 *
 * Converting from Unicode to codepage bytes --------------------------------***
 *
 * The conversion data structure for fromUnicode is designed for the known
 * structure of Unicode. It maps from 21-bit code points (0..0x10ffff) to
 * a sequence of 1..4 bytes, in addition to a flag that indicates if there is
 * a roundtrip mapping.
 *
 * The lookup is done with a 3-stage trie, using 11/6/4 bits for stage 1/2/3
 * like in the character properties table.
 * The beginning of the trie is at offsetFromUTable, the beginning of stage 3
 * with the resulting bytes is at offsetFromUBytes.
 *
 * Beginning with version 4, single-byte codepages have a significantly different
 * trie compared to other codepages.
 * In all cases, the entry in stage 1 is directly the index of the block of
 * 64 entries in stage 2.
 *
 * Single-byte lookup:
 *
 * Stage 2 only contains 16-bit indexes directly to the 16-blocks in stage 3.
 * Stage 3 contains one 16-bit word per result:
 * Bits 15..8 indicate the kind of result:
 *    f  roundtrip result
 *    c  fallback result from private-use code point
 *    8  fallback result from other code points
 *    0  unassigned
 * Bits 7..0 contain the codepage byte. A zero byte is always possible.
 *
 * In version 4.3, the runtime code can build an sbcsIndex for a utf8Friendly
 * file. For 2-byte UTF-8 byte sequences and some 3-byte sequences the lookup
 * becomes a 2-stage (single-index) trie lookup with 6 bits for stage 3.
 * ASCII code points can be looked up with a linear array access into stage 3.
 * See maxFastUChar and other details in ucnvmbcs.h.
 *
 * Multi-byte lookup:
 *
 * Stage 2 contains a 32-bit word for each 16-block in stage 3:
 * Bits 31..16 contain flags for which stage 3 entries contain roundtrip results
 *             test: MBCS_FROM_U_IS_ROUNDTRIP(stage2Entry, c)
 *             If this test is false, then a non-zero result will be interpreted as
 *             a fallback mapping.
 * Bits 15..0  contain the index to stage 3, which must be multiplied by 16*(bytes per char)
 *
 * Stage 3 contains 2, 3, or 4 bytes per result.
 * 2 or 4 bytes are stored as uint16_t/uint32_t in platform endianness,
 * while 3 bytes are stored as bytes in big-endian order.
 * Leading zero bytes are ignored, and the number of bytes is counted.
 * A zero byte mapping result is possible as a roundtrip result.
 * For some output types, the actual result is processed from this;
 * see ucnv_MBCSFromUnicodeWithOffsets().
 *
 * Note that stage 1 always contains 0x440=1088 entries (0x440==0x110000>>10),
 * or (version 3 and up) for BMP-only codepages, it contains 64 entries.
 *
 * In version 4.3, a utf8Friendly file contains an mbcsIndex table.
 * For 2-byte UTF-8 byte sequences and most 3-byte sequences the lookup
 * becomes a 2-stage (single-index) trie lookup with 6 bits for stage 3.
 * ASCII code points can be looked up with a linear array access into stage 3.
 * See maxFastUChar, mbcsIndex and other details in ucnvmbcs.h.
 *
 * In version 3, stage 2 blocks may overlap by multiples of the multiplier
 * for compaction.
 * In version 4, stage 2 blocks (and for single-byte codepages, stage 3 blocks)
 * may overlap by any number of entries.
 *
 * MBCS version 2 added:
 * the converter checks for known output types, which allows
 * adding new ones without crashing an unaware converter
 */

/**
 * Callback from ucnv_MBCSEnumToUnicode(), takes 32 mappings from
 * consecutive sequences of bytes, starting from the one encoded in value,
 * to Unicode code points. (Multiple mappings to reduce per-function call overhead.)
 * Does not currently support m:n mappings or reverse fallbacks.
 * This function will not be called for sequences of bytes with leading zeros.
 *
 * @param context an opaque pointer, as passed into ucnv_MBCSEnumToUnicode()
 * @param value contains 1..4 bytes of the first byte sequence, right-aligned
 * @param codePoints resulting Unicode code points, or negative if a byte sequence does
 *        not map to anything
 * @return true to continue enumeration, false to stop
 */
प्रकारनिर्देश UBool U_CALLCONV
UConverter_Enum_गन्तव्यम्_UCallback(ध्रुव शून्य *context, uint32_t value, UChar32 संहिता_Points[32]);

स्थिर शून्य U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_दत्त *shared_दत्त,
          UConverter_आरोपय_तर्काः *p_तर्काः,
          ध्रुव uint8_t *raw,
          UError_संहिता *p_Error_संहिता);

स्थिर शून्य U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_दत्त *shared_दत्त);

स्थिर शून्य U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_आरोपय_तर्काः *p_तर्काः,
              UError_संहिता *p_Error_संहिता);

स्थिर UChar32 U_CALLCONV
ucnv_MBCSGet_अग्र_UChar(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                  UError_संहिता *p_Error_संहिता);

स्थिर शून्य U_CALLCONV
ucnv_MBCSGetStarters(ध्रुव UConverter* cnv,
                 UBool starters[256],
                 UError_संहिता *p_Error_संहिता);

U_CDECL_BEGIN
स्थिर ध्रुव वर्ण* U_CALLCONV
ucnv_MBCSGetName(ध्रुव UConverter *cnv);
U_CDECL_अन्तः

स्थिर शून्य U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_तर्काः *p_तर्काः,
              int32_t विस्थापनम्_सूचकांक,
              UError_संहिता *p_Error_संहिता);

स्थिर UChar32 U_CALLCONV
ucnv_MBCSGet_अग्र_UChar(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                  UError_संहिता *p_Error_संहिता);

स्थिर शून्य U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_तर्काः *pFromUArgs,
                  UConverter_गन्तव्यम्_Unicode_तर्काः *p_गन्तव्यम्_UArgs,
                  UError_संहिता *p_Error_संहिता);

स्थिर शून्य U_CALLCONV
ucnv_MBCSGet_Unicode_समुच्चयः(ध्रुव UConverter *cnv,
                   ध्रुव USetAdder *sa,
                   UConverter_Unicode_समुच्चयः which,
                   UError_संहिता *p_Error_संहिता);

स्थिर शून्य U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_तर्काः *pFromUArgs,
                  UConverter_गन्तव्यम्_Unicode_तर्काः *p_गन्तव्यम्_UArgs,
                  UError_संहिता *p_Error_संहिता);

स्थिर ध्रुव UConverterImpl _SBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    शून्यसूचकः,
    शून्यसूचकः,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_अग्र_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    शून्यसूचकः,
    ucnv_MBCSGet_Unicode_समुच्चयः,

    शून्यसूचकः,
    ucnv_SBCSFromUTF8
};

स्थिर ध्रुव UConverterImpl _DBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    शून्यसूचकः,
    शून्यसूचकः,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_अग्र_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    शून्यसूचकः,
    ucnv_MBCSGet_Unicode_समुच्चयः,

    शून्यसूचकः,
    ucnv_DBCSFromUTF8
};

स्थिर ध्रुव UConverterImpl _MBCSImpl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    शून्यसूचकः,
    शून्यसूचकः,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_अग्र_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    शून्यसूचकः,
    ucnv_MBCSGet_Unicode_समुच्चयः,
    शून्यसूचकः,
    शून्यसूचकः
};

/* Static data is in tools/makeconv/ucnvstat.c for data-based
 * converters. Be sure to update it as well.
 */

ध्रुव UConverter_Shared_दत्त _MBCSData={
    परिमाण(UConverter_Shared_दत्त), 1,
    शून्यसूचकः, शून्यसूचकः, असत्य, सत्य, &_MBCSImpl,
    0, UCNV_MBCS_सारणी_INITIALIZER
};


/* GB 18030 data ------------------------------------------------------------ */

/* helper macros for linear values for GB 18030 four-byte sequences */
#परिभाषय रैखिकम्_18030(a, b, c, d) ((((a)*10+(b))*126L+(c))*10L+(d))

#परिभाषय रैखिकम्_18030_आधारः रैखिकम्_18030(0x81, 0x30, 0x81, 0x30)

#परिभाषय रैखिकम्(x) रैखिकम्_18030(x>>24, (x>>16)&0xff, (x>>8)&0xff, x&0xff)

/*
 * Some ranges of GB 18030 where both the Unicode code points and the
 * GB four-byte sequences are contiguous and are handled algorithmically by
 * the special callback functions below.
 * The values are start & end of Unicode & GB codes.
 *
 * Note that single surrogates are not mapped by GB 18030
 * as of the re-released mapping tables from 2000-nov-30.
 */
स्थिर ध्रुव uint32_t
gb18030Ranges[14][4]={
    {0x10000, 0x10FFFF, रैखिकम्(0x90308130), रैखिकम्(0xE3329A35)},
    {0x9FA6, 0xD7FF, रैखिकम्(0x82358F33), रैखिकम्(0x8336C738)},
    {0x0452, 0x1E3E, रैखिकम्(0x8130D330), रैखिकम्(0x8135F436)},
    {0x1E40, 0x200F, रैखिकम्(0x8135F438), रैखिकम्(0x8136A531)},
    {0xE865, 0xF92B, रैखिकम्(0x8336D030), रैखिकम्(0x84308534)},
    {0x2643, 0x2E80, रैखिकम्(0x8137A839), रैखिकम्(0x8138FD38)},
    {0xFA2A, 0xFE2F, रैखिकम्(0x84309C38), रैखिकम्(0x84318537)},
    {0x3CE1, 0x4055, रैखिकम्(0x8231D438), रैखिकम्(0x8232AF32)},
    {0x361B, 0x3917, रैखिकम्(0x8230A633), रैखिकम्(0x8230F237)},
    {0x49B8, 0x4C76, रैखिकम्(0x8234A131), रैखिकम्(0x8234E733)},
    {0x4160, 0x4336, रैखिकम्(0x8232C937), रैखिकम्(0x8232F837)},
    {0x478E, 0x4946, रैखिकम्(0x8233E838), रैखिकम्(0x82349638)},
    {0x44D7, 0x464B, रैखिकम्(0x8233A339), रैखिकम्(0x8233C931)},
    {0xFFE6, 0xFFFF, रैखिकम्(0x8431A234), रैखिकम्(0x8431A439)}
};

/* bit flag for UConverter.options indicating GB 18030 special handling */
#परिभाषय _MBCS_OPTION_GB18030 0x8000

/* bit flag for UConverter.options indicating KEIS,JEF,JIF special handling */
#परिभाषय _MBCS_OPTION_KEIS 0x01000
#परिभाषय _MBCS_OPTION_JEF  0x02000
#परिभाषय _MBCS_OPTION_JIPS 0x04000

#परिभाषय KEIS_SO_CHAR_1 0x0A
#परिभाषय KEIS_SO_CHAR_2 0x42
#परिभाषय KEIS_SI_CHAR_1 0x0A
#परिभाषय KEIS_SI_CHAR_2 0x41

#परिभाषय JEF_SO_CHAR 0x28
#परिभाषय JEF_SI_CHAR 0x29

#परिभाषय JIPS_SO_CHAR_1 0x1A
#परिभाषय JIPS_SO_CHAR_2 0x70
#परिभाषय JIPS_SI_CHAR_1 0x1A
#परिभाषय JIPS_SI_CHAR_2 0x71

गणना SISO_Option {
    SI,
    SO
};
प्रकारनिर्देश गणना SISO_Option SISO_Option;

स्थिर int32_t getSISOBytes(SISO_Option option, uint32_t cnvOption, uint8_t *value) {
    int32_t SISOLength = 0;

    विकल्प (option) {
        पक्ष SI:
            यदि ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SI_CHAR_1;
                value[1] = KEIS_SI_CHAR_2;
                SISOLength = 2;
            } अन्यथा यदि ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SI_CHAR;
                SISOLength = 1;
            } अन्यथा यदि ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SI_CHAR_1;
                value[1] = JIPS_SI_CHAR_2;
                SISOLength = 2;
            } अन्यथा {
                value[0] = UCNV_SI;
                SISOLength = 1;
            }
            भङ्ग;
        पक्ष SO:
            यदि ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SO_CHAR_1;
                value[1] = KEIS_SO_CHAR_2;
                SISOLength = 2;
            } अन्यथा यदि ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SO_CHAR;
                SISOLength = 1;
            } अन्यथा यदि ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SO_CHAR_1;
                value[1] = JIPS_SO_CHAR_2;
                SISOLength = 2;
            } अन्यथा {
                value[0] = UCNV_SO;
                SISOLength = 1;
            }
            भङ्ग;
        स्वभाव:
            /* Should never happen. */
            भङ्ग;
    }

    प्रत्यागम SISOLength;
}

/* Miscellaneous ------------------------------------------------------------ */

/* similar to ucnv_MBCSGetNextUChar() but recursive */
स्थिर UBool
enum_गन्तव्यम्_U(UConverterMBCSTable *mbcs_सारणी, int8_t stateProps[],
        int32_t state, uint32_t विस्थापनम्,
        uint32_t value,
        UConverter_Enum_गन्तव्यम्_UCallback *callback, ध्रुव शून्य *context,
        UError_संहिता *p_Error_संहिता) {
    UChar32 संहिता_Points[32];
    ध्रुव int32_t *row;
    ध्रुव uint16_t *unicode_संहिता_Units;
    UChar32 any_संहिता_Points;
    int32_t b, सीमा;

    row=mbcs_सारणी->state_सारणी[state];
    unicode_संहिता_Units=mbcs_सारणी->unicode_संहिता_Units;

    value<<=8;
    any_संहिता_Points=-1;  /* becomes non-negative if there is a mapping */

    b=(stateProps[state]&0x38)<<2;
    यदि(b==0 && stateProps[state]>=0x40) {
        /* skip byte sequences with leading zeros because they are not stored in the fromUnicode table */
        संहिता_Points[0]=U_SENTINEL;
        b=1;
    }
    सीमा=((stateProps[state]&7)+1)<<5;
    यावत्(b<सीमा) {
        int32_t प्रविष्टिः=row[b];
        यदि(MBCS_प्रविष्टिः_IS_TRANSITION(प्रविष्टिः)) {
            int32_t अग्र_State=MBCS_प्रविष्टिः_TRANSITION_STATE(प्रविष्टिः);
            यदि(stateProps[अग्र_State]>=0) {
                /* recurse to a state with non-ignorable actions */
                यदि(!enum_गन्तव्यम्_U(
                        mbcs_सारणी, stateProps, अग्र_State,
                        विस्थापनम्+MBCS_प्रविष्टिः_TRANSITION_विस्थापनम्(प्रविष्टिः),
                        value | static_cast<uint32_t>(b),
                        callback, context,
                        p_Error_संहिता)) {
                    प्रत्यागम असत्य;
                }
            }
            संहिता_Points[b&0x1f]=U_SENTINEL;
        } अन्यथा {
            UChar32 c;
            int32_t action;

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः);
            यदि(action==MBCS_STATE_VALID_प्रत्यक्षम्_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            } अन्यथा यदि(action==MBCS_STATE_VALID_16) {
                int32_t final_विस्थापनम्=विस्थापनम्+MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
                c=unicode_संहिता_Units[final_विस्थापनम्];
                यदि(c<0xfffe) {
                    /* output BMP code point */
                } अन्यथा {
                    c=U_SENTINEL;
                }
            } अन्यथा यदि(action==MBCS_STATE_VALID_16_PAIR) {
                int32_t final_विस्थापनम्=विस्थापनम्+MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
                c=unicode_संहिता_Units[final_विस्थापनम्++];
                यदि(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } अन्यथा यदि(c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_संहिता_Units[final_विस्थापनम्]+(0x10000-0xdc00);
                } अन्यथा यदि(c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_संहिता_Units[final_विस्थापनम्];
                } अन्यथा {
                    c=U_SENTINEL;
                }
            } अन्यथा यदि(action==MBCS_STATE_VALID_प्रत्यक्षम्_20) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_प्रविष्टिः_FINAL_मानम्(प्रविष्टिः) + 0x10000);
            } अन्यथा {
                c=U_SENTINEL;
            }

            संहिता_Points[b&0x1f]=c;
            any_संहिता_Points&=c;
        }
        यदि(((++b)&0x1f)==0) {
            यदि(any_संहिता_Points>=0) {
                यदि (!callback(context, value | static_cast<uint32_t>(b - 0x20), संहिता_Points)) {
                    प्रत्यागम असत्य;
                }
                any_संहिता_Points=-1;
            }
        }
    }
    प्रत्यागम सत्य;
}

/*
 * Only called if stateProps[state]==-1.
 * A recursive call may do stateProps[state]|=0x40 if this state is the target of an
 * MBCS_STATE_CHANGE_ONLY.
 */
स्थिर int8_t
getStateProp(ध्रुव int32_t (*state_सारणी)[256], int8_t stateProps[], पूर्णाङ्क state) {
    ध्रुव int32_t *row;
    int32_t min, max, प्रविष्टिः, अग्र_State;

    row=state_सारणी[state];
    stateProps[state]=0;

    /* find first non-ignorable state */
    प्रति(min=0;; ++min) {
        प्रविष्टिः=row[min];
        अग्र_State=MBCS_प्रविष्टिः_STATE(प्रविष्टिः);
        यदि(stateProps[अग्र_State]==-1) {
            getStateProp(state_सारणी, stateProps, अग्र_State);
        }
        यदि(MBCS_प्रविष्टिः_IS_TRANSITION(प्रविष्टिः)) {
            यदि(stateProps[अग्र_State]>=0) {
                भङ्ग;
            }
        } अन्यथा यदि(MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः)<MBCS_STATE_UNASSIGNED) {
            भङ्ग;
        }
        यदि(min==0xff) {
            stateProps[state]=-0x40;  /* (int8_t)0xc0 */
            प्रत्यागम stateProps[state];
        }
    }
    stateProps[state] |= static_cast<int8_t>((min >> 5) << 3);

    /* find last non-ignorable state */
    प्रति(max=0xff; min<max; --max) {
        प्रविष्टिः=row[max];
        अग्र_State=MBCS_प्रविष्टिः_STATE(प्रविष्टिः);
        यदि(stateProps[अग्र_State]==-1) {
            getStateProp(state_सारणी, stateProps, अग्र_State);
        }
        यदि(MBCS_प्रविष्टिः_IS_TRANSITION(प्रविष्टिः)) {
            यदि(stateProps[अग्र_State]>=0) {
                भङ्ग;
            }
        } अन्यथा यदि(MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः)<MBCS_STATE_UNASSIGNED) {
            भङ्ग;
        }
    }
    stateProps[state] |= static_cast<int8_t>(max >> 5);

    /* recurse further and collect direct-state information */
    यावत्(min<=max) {
        प्रविष्टिः=row[min];
        अग्र_State=MBCS_प्रविष्टिः_STATE(प्रविष्टिः);
        यदि(stateProps[अग्र_State]==-1) {
            getStateProp(state_सारणी, stateProps, अग्र_State);
        }
        यदि(MBCS_प्रविष्टिः_IS_FINAL(प्रविष्टिः)) {
            stateProps[अग्र_State]|=0x40;
            यदि(MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः)<=MBCS_STATE_FALLBACK_प्रत्यक्षम्_20) {
                stateProps[state]|=0x40;
            }
        }
        ++min;
    }
    प्रत्यागम stateProps[state];
}

/*
 * Internal function enumerating the toUnicode data of an MBCS converter.
 * Currently only used for reconstituting data for a MBCS_OPT_NO_FROM_U
 * table, but could also be used for a future ucnv_getUnicodeSet() option
 * that includes reverse fallbacks (after updating this function's implementation).
 * Currently only handles roundtrip mappings.
 * Does not currently handle extensions.
 */
स्थिर शून्य
ucnv_MBCSEnum_गन्तव्यम्_Unicode(UConverterMBCSTable *mbcs_सारणी,
                       UConverter_Enum_गन्तव्यम्_UCallback *callback, ध्रुव शून्य *context,
                       UError_संहिता *p_Error_संहिता) {
    /*
     * Properties for each state, to speed up the enumeration.
     * Ignorable actions are unassigned/illegal/state-change-only:
     * They do not lead to mappings.
     *
     * Bits 7..6:
     * 1 direct/initial state (stateful converters have multiple)
     * 0 non-initial state with transitions or with non-ignorable result actions
     * -1 final state with only ignorable actions
     *
     * Bits 5..3:
     * The lowest byte value with non-ignorable actions is
     * value<<5 (rounded down).
     *
     * Bits 2..0:
     * The highest byte value with non-ignorable actions is
     * (value<<5)&0x1f (rounded up).
     */
    int8_t stateProps[MBCS_MAX_STATE_गणनम्];
    int32_t state;

    uprv_memset(stateProps, -1, परिमाण(stateProps));

    /* recurse from state 0 and set all stateProps */
    getStateProp(mbcs_सारणी->state_सारणी, stateProps, 0);

    प्रति(state=0; state<mbcs_सारणी->गणनम्_अवस्थाः; ++state) {
        /*if(stateProps[state]==-1) {
            printf("unused/unreachable <icu:state> %d\n", state);
        }*/
        यदि(stateProps[state]>=0x40) {
            /* start from each direct state */
            enum_गन्तव्यम्_U(
                mbcs_सारणी, stateProps, state, 0, 0,
                callback, context,
                p_Error_संहिता);
        }
    }
}

U_CFUNC शून्य 
ucnv_MBCSGet_Filtered_Unicode_समुच्चयः_For_Unicode(ध्रुव UConverter_Shared_दत्त *shared_दत्त,
                                         ध्रुव USetAdder *sa,
                                         UConverter_Unicode_समुच्चयः which,
                                         UConverter_समुच्चयः_Filter filter,
                                         UError_संहिता *p_Error_संहिता) {
    ध्रुव UConverterMBCSTable *mbcs_सारणी;
    ध्रुव uint16_t *सारणी;

    uint32_t st3;
    uint16_t st1, maxStage1, st2;

    UChar32 c;

    /* enumerate the from-Unicode trie table */
    mbcs_सारणी=&shared_दत्त->mbcs;
    सारणी=mbcs_सारणी->from_Unicode_सारणी;
    यदि(mbcs_सारणी->unicode_आवरणम्&UCNV_HAS_SUPPLEMENTARY) {
        maxStage1=0x440;
    } अन्यथा {
        maxStage1=0x40;
    }

    c=0; /* keep track of the current code point while enumerating */

    यदि(mbcs_सारणी->outputType==MBCS_OUTPUT_1) {
        ध्रुव uint16_t *stage2, *stage3, *results;
        uint16_t min_मानम्;

        results=(ध्रुव uint16_t *)mbcs_सारणी->from_Unicode_अष्टकानि;

        /*
         * Set a threshold variable for selecting which mappings to use.
         * See ucnv_MBCSSingleFromBMPWithOffsets() and
         * MBCS_SINGLE_RESULT_FROM_U() for details.
         */
        यदि(which==UCNV_ROUNDTRIP_समुच्चयः) {
            /* use only roundtrips */
            min_मानम्=0xf00;
        } अन्यथा /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
            /* use all roundtrip and fallback results */
            min_मानम्=0x800;
        }

        प्रति(st1=0; st1<maxStage1; ++st1) {
            st2=सारणी[st1];
            यदि(st2>maxStage1) {
                stage2=सारणी+st2;
                प्रति(st2=0; st2<64; ++st2) {
                    यदि((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=results+st3;

                        कुरु {
                            यदि(*stage3++>=min_मानम्) {
                                sa->सङ्कलय(sa->समुच्चयः, c);
                            }
                        } यावत्((++c&0xf)!=0);
                    } अन्यथा {
                        c+=16; /* empty stage 3 block */
                    }
                }
            } अन्यथा {
                c+=1024; /* empty stage 2 block */
            }
        }
    } अन्यथा {
        ध्रुव uint32_t *stage2;
        ध्रुव uint8_t *stage3, *अष्टकानि;
        uint32_t st3Multiplier;
        uint32_t value;
        UBool useFallback;

        अष्टकानि=mbcs_सारणी->from_Unicode_अष्टकानि;

        useFallback = which == UCNV_ROUNDTRIP_AND_FALLBACK_समुच्चयः;

        विकल्प(mbcs_सारणी->outputType) {
        पक्ष MBCS_OUTPUT_3:
        पक्ष MBCS_OUTPUT_4_EUC:
            st3Multiplier=3;
            भङ्ग;
        पक्ष MBCS_OUTPUT_4:
            st3Multiplier=4;
            भङ्ग;
        स्वभाव:
            st3Multiplier=2;
            भङ्ग;
        }

        प्रति(st1=0; st1<maxStage1; ++st1) {
            st2=सारणी[st1];
            यदि(st2>(maxStage1>>1)) {
                stage2=(ध्रुव uint32_t *)सारणी+st2;
                प्रति(st2=0; st2<64; ++st2) {
                    यदि((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=अष्टकानि+st3Multiplier*16*(uint32_t)(uint16_t)st3;

                        /* get the roundtrip flags for the stage 3 block */
                        st3>>=16;

                        /*
                         * Add code points for which the roundtrip flag is set,
                         * or which map to non-zero bytes if we use fallbacks.
                         * See ucnv_MBCSFromUnicodeWithOffsets() for details.
                         */
                        विकल्प(filter) {
                        पक्ष UCNV_समुच्चयः_FILTER_NONE:
                            कुरु {
                                यदि(st3&1) {
                                    sa->सङ्कलय(sa->समुच्चयः, c);
                                    stage3+=st3Multiplier;
                                } अन्यथा यदि(useFallback) {
                                    uint8_t b=0;
                                    विकल्प(st3Multiplier) {
                                    पक्ष 4:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    पक्ष 3:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    पक्ष 2:
                                        b|=stage3[0]|stage3[1];
                                        stage3+=2;
                                        U_FALLTHROUGH;
                                    स्वभाव:
                                        भङ्ग;
                                    }
                                    यदि(b!=0) {
                                        sa->सङ्कलय(sa->समुच्चयः, c);
                                    }
                                }
                                st3>>=1;
                            } यावत्((++c&0xf)!=0);
                            भङ्ग;
                        पक्ष UCNV_समुच्चयः_FILTER_DBCS_ONLY:
                             /* Ignore single-byte results (<0x100). */
                            कुरु {
                                यदि(((st3&1)!=0 || useFallback) && *((ध्रुव uint16_t *)stage3)>=0x100) {
                                    sa->सङ्कलय(sa->समुच्चयः, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } यावत्((++c&0xf)!=0);
                            भङ्ग;
                        पक्ष UCNV_समुच्चयः_FILTER_2022_CN:
                             /* Only add code points that map to CNS 11643 planes 1 & 2 for non-EXT ISO-2022-CN. */
                            कुरु {
                                यदि(((st3&1)!=0 || useFallback) && ((value=*stage3)==0x81 || value==0x82)) {
                                    sa->सङ्कलय(sa->समुच्चयः, c);
                                }
                                st3>>=1;
                                stage3+=3;  /* +=st3Multiplier */
                            } यावत्((++c&0xf)!=0);
                            भङ्ग;
                        पक्ष UCNV_समुच्चयः_FILTER_SJIS:
                             /* Only add code points that map to Shift-JIS codes corresponding to JIS X 0208. */
                            कुरु {
                                यदि(((st3&1)!=0 || useFallback) && (value=*((ध्रुव uint16_t *)stage3))>=0x8140 && value<=0xeffc) {
                                    sa->सङ्कलय(sa->समुच्चयः, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } यावत्((++c&0xf)!=0);
                            भङ्ग;
                        पक्ष UCNV_समुच्चयः_FILTER_GR94_DBCS:
                            /* Only add code points that map to ISO 2022 GR 94 DBCS codes (each byte A1..FE). */
                            कुरु {
                                यदि( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((ध्रुव uint16_t *)stage3)) - 0xa1a1)<=(0xfefe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->सङ्कलय(sa->समुच्चयः, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } यावत्((++c&0xf)!=0);
                            भङ्ग;
                        पक्ष UCNV_समुच्चयः_FILTER_HZ:
                            /* Only add code points that are suitable for HZ DBCS (lead byte A1..FD). */
                            कुरु {
                                यदि( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((ध्रुव uint16_t *)stage3))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->सङ्कलय(sa->समुच्चयः, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } यावत्((++c&0xf)!=0);
                            भङ्ग;
                        स्वभाव:
                            *p_Error_संहिता=U_आन्तरिकम्_क्रमादेशः_ERROR;
                            प्रत्यागम;
                        }
                    } अन्यथा {
                        c+=16; /* empty stage 3 block */
                    }
                }
            } अन्यथा {
                c+=1024; /* empty stage 2 block */
            }
        }
    }

    ucnv_ext_Get_Unicode_समुच्चयः(shared_दत्त, sa, which, filter, p_Error_संहिता);
}

U_CFUNC शून्य
ucnv_MBCSGet_Unicode_समुच्चयः_For_Unicode(ध्रुव UConverter_Shared_दत्त *shared_दत्त,
                                 ध्रुव USetAdder *sa,
                                 UConverter_Unicode_समुच्चयः which,
                                 UError_संहिता *p_Error_संहिता) {
    ucnv_MBCSGet_Filtered_Unicode_समुच्चयः_For_Unicode(
        shared_दत्त, sa, which,
        shared_दत्त->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ?
            UCNV_समुच्चयः_FILTER_DBCS_ONLY :
            UCNV_समुच्चयः_FILTER_NONE,
        p_Error_संहिता);
}

स्थिर शून्य U_CALLCONV
ucnv_MBCSGet_Unicode_समुच्चयः(ध्रुव UConverter *cnv,
                   ध्रुव USetAdder *sa,
                   UConverter_Unicode_समुच्चयः which,
                   UError_संहिता *p_Error_संहिता) {
    यदि(cnv->options&_MBCS_OPTION_GB18030) {
        sa->सङ्कलय_Range(sa->समुच्चयः, 0, 0xd7ff);
        sa->सङ्कलय_Range(sa->समुच्चयः, 0xe000, 0x10ffff);
    } अन्यथा {
        ucnv_MBCSGet_Unicode_समुच्चयः_For_Unicode(cnv->shared_दत्त, sa, which, p_Error_संहिता);
    }
}

/* conversion extensions for input not in the main table -------------------- */

/*
 * Hardcoded extension handling for GB 18030.
 * Definition of LINEAR macros and gb18030Ranges see near the beginning of the file.
 *
 * In the future, conversion extensions may handle m:n mappings and delta tables,
 * see https://htmlpreview.github.io/?https://github.com/unicode-org/icu-docs/blob/main/design/conversion/conversion_extensions.html
 *
 * If an input character cannot be mapped, then these functions set an error
 * code. The framework will then call the callback function.
 */

/*
 * @return if(U_FAILURE) return the code point for cnv->fromUChar32
 *         else return 0 after output has been written to the target
 */
स्थिर UChar32
_extFromU(UConverter *cnv, ध्रुव UConverter_Shared_दत्त *shared_दत्त,
          UChar32 cp,
          ध्रुव char16_t **source, ध्रुव char16_t *source_सीमा,
          uint8_t **target, ध्रुव uint8_t *target_सीमा,
          int32_t **offsets, int32_t source_सूचकांक,
          UBool flush,
          UError_संहिता *p_Error_संहिता) {
    ध्रुव int32_t *cx;

    cnv->useSubChar1=असत्य;

    यदि( (cx=shared_दत्त->mbcs.extIndexes)!=शून्यसूचकः &&
        ucnv_ext_Initial_मेलनम्_From_U(
            cnv, cx,
            cp, source, source_सीमा,
            reinterpret_cast<वर्ण**>(target), reinterpret_cast<ध्रुव वर्ण*>(target_सीमा),
            offsets, source_सूचकांक,
            flush,
            p_Error_संहिता)
    ) {
        प्रत्यागम 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    यदि((cnv->options&_MBCS_OPTION_GB18030)!=0) {
        ध्रुव uint32_t *range;
        int32_t i;

        range=gb18030Ranges[0];
        प्रति(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            यदि (range[0] <= static_cast<uint32_t>(cp) && static_cast<uint32_t>(cp) <= range[1]) {
                /* found the Unicode code point, output the four-byte sequence for it */
                uint32_t रैखिकम्_2;
                वर्ण अष्टकानि[4];

                /* get the linear value of the first GB 18030 code in this range */
                रैखिकम्_2=range[2]-रैखिकम्_18030_आधारः;

                /* add the offset from the beginning of the range */
                रैखिकम्_2 += (static_cast<uint32_t>(cp) - range[0]);

                /* turn this into a four-byte sequence */
                अष्टकानि[3] = static_cast<वर्ण>(0x30 + रैखिकम्_2 % 10); रैखिकम्_2 /= 10;
                अष्टकानि[2] = static_cast<वर्ण>(0x81 + रैखिकम्_2 % 126); रैखिकम्_2 /= 126;
                अष्टकानि[1] = static_cast<वर्ण>(0x30 + रैखिकम्_2 % 10); रैखिकम्_2 /= 10;
                अष्टकानि[0] = static_cast<वर्ण>(0x81 + रैखिकम्_2);

                /* output this sequence */
                ucnv_from_UWrite_अष्टकानि(cnv,
                                     अष्टकानि, 4, reinterpret_cast<वर्ण**>(target), reinterpret_cast<ध्रुव वर्ण*>(target_सीमा),
                                     offsets, source_सूचकांक, p_Error_संहिता);
                प्रत्यागम 0;
            }
        }
    }

    /* no mapping */
    *p_Error_संहिता=U_INVALID_CHAR_प्राप्तम्;
    प्रत्यागम cp;
}

/*
 * Input sequence: cnv->toUBytes[0..length[
 * @return if(U_FAILURE) return the length (toULength, byteIndex) for the input
 *         else return 0 after output has been written to the target
 */
स्थिर int8_t
ext_गन्तव्यम्_U(UConverter *cnv, ध्रुव UConverter_Shared_दत्त *shared_दत्त,
        int8_t length,
        ध्रुव uint8_t **source, ध्रुव uint8_t *source_सीमा,
        char16_t **target, ध्रुव char16_t *target_सीमा,
        int32_t **offsets, int32_t source_सूचकांक,
        UBool flush,
        UError_संहिता *p_Error_संहिता) {
    ध्रुव int32_t *cx;

    यदि( (cx=shared_दत्त->mbcs.extIndexes)!=शून्यसूचकः &&
        ucnv_ext_Initial_मेलनम्_गन्तव्यम्_U(
            cnv, cx,
            length, reinterpret_cast<ध्रुव वर्ण**>(source), reinterpret_cast<ध्रुव वर्ण*>(source_सीमा),
            target, target_सीमा,
            offsets, source_सूचकांक,
            flush,
            p_Error_संहिता)
    ) {
        प्रत्यागम 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    यदि(length==4 && (cnv->options&_MBCS_OPTION_GB18030)!=0) {
        ध्रुव uint32_t *range;
        uint32_t रैखिकम्_2;
        int32_t i;

        रैखिकम्_2=रैखिकम्_18030(cnv->गन्तव्यम्_UBytes[0], cnv->गन्तव्यम्_UBytes[1], cnv->गन्तव्यम्_UBytes[2], cnv->गन्तव्यम्_UBytes[3]);
        range=gb18030Ranges[0];
        प्रति(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            यदि(range[2]<=रैखिकम्_2 && रैखिकम्_2<=range[3]) {
                /* found the sequence, output the Unicode code point for it */
                *p_Error_संहिता=U_ZERO_ERROR;

                /* add the linear difference between the input and start sequences to the start code point */
                रैखिकम्_2=range[0]+(रैखिकम्_2-range[2]);

                /* output this code point */
                ucnv_गन्तव्यम्_UWrite_संहिता_Point(cnv, रैखिकम्_2, target, target_सीमा, offsets, source_सूचकांक, p_Error_संहिता);

                प्रत्यागम 0;
            }
        }
    }

    /* no mapping */
    *p_Error_संहिता=U_INVALID_CHAR_प्राप्तम्;
    प्रत्यागम length;
}

/* EBCDIC swap LF<->NL ------------------------------------------------------ */

/*
 * This code modifies a standard EBCDIC<->Unicode mapping table for
 * OS/390 (z/OS) Unix System Services (Open Edition).
 * The difference is in the mapping of Line Feed and New Line control codes:
 * Standard EBCDIC maps
 *
 *   <U000A> \x25 |0
 *   <U0085> \x15 |0
 *
 * but OS/390 USS EBCDIC swaps the control codes for LF and NL,
 * mapping
 *
 *   <U000A> \x15 |0
 *   <U0085> \x25 |0
 *
 * This code modifies a loaded standard EBCDIC<->Unicode mapping table
 * by copying it into allocated memory and swapping the LF and NL values.
 * It allows to support the same EBCDIC charset in both versions without
 * duplicating the entire installed table.
 */

/* standard EBCDIC codes */
#परिभाषय EBCDIC_LF 0x25
#परिभाषय EBCDIC_NL 0x15

/* standard EBCDIC codes with roundtrip flag as stored in Unicode-to-single-byte tables */
#परिभाषय EBCDIC_RT_LF 0xf25
#परिभाषय EBCDIC_RT_NL 0xf15

/* Unicode code points */
#परिभाषय U_LF 0x0a
#परिभाषय U_NL 0x85

स्थिर UBool
_EBCDICSwapLFNL(UConverter_Shared_दत्त *shared_दत्त, UError_संहिता *p_Error_संहिता) {
    UConverterMBCSTable *mbcs_सारणी;

    ध्रुव uint16_t *सारणी, *results;
    ध्रुव uint8_t *अष्टकानि;

    int32_t (*new_State_सारणी)[256];
    uint16_t *newResults;
    uint8_t *p;
    वर्ण *name;

    uint32_t stage2_प्रविष्टिः;
    uint32_t size, sizeofFromUBytes;

    mbcs_सारणी=&shared_दत्त->mbcs;

    सारणी=mbcs_सारणी->from_Unicode_सारणी;
    अष्टकानि=mbcs_सारणी->from_Unicode_अष्टकानि;
    results = reinterpret_cast<ध्रुव uint16_t*>(अष्टकानि);

    /*
     * Check that this is an EBCDIC table with SBCS portion -
     * SBCS or EBCDIC_STATEFUL with standard EBCDIC LF and NL mappings.
     *
     * If not, ignore the option. Options are always ignored if they do not apply.
     */
    यदि(!(
         (mbcs_सारणी->outputType==MBCS_OUTPUT_1 || mbcs_सारणी->outputType==MBCS_OUTPUT_2_SISO) &&
         mbcs_सारणी->state_सारणी[0][EBCDIC_LF]==MBCS_प्रविष्टिः_FINAL(0, MBCS_STATE_VALID_प्रत्यक्षम्_16, U_LF) &&
         mbcs_सारणी->state_सारणी[0][EBCDIC_NL]==MBCS_प्रविष्टिः_FINAL(0, MBCS_STATE_VALID_प्रत्यक्षम्_16, U_NL)
    )) {
        प्रत्यागम असत्य;
    }

    यदि(mbcs_सारणी->outputType==MBCS_OUTPUT_1) {
        यदि(!(
             EBCDIC_RT_LF==MBCS_SINGLE_परिणाम_FROM_U(सारणी, results, U_LF) &&
             EBCDIC_RT_NL==MBCS_SINGLE_परिणाम_FROM_U(सारणी, results, U_NL)
        )) {
            प्रत्यागम असत्य;
        }
    } अन्यथा /* MBCS_OUTPUT_2_SISO */ {
        stage2_प्रविष्टिः=MBCS_STAGE_2_FROM_U(सारणी, U_LF);
        यदि(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_प्रविष्टिः, U_LF)!=0 &&
             EBCDIC_LF==MBCS_मानम्_2_FROM_STAGE_2(अष्टकानि, stage2_प्रविष्टिः, U_LF)
        )) {
            प्रत्यागम असत्य;
        }

        stage2_प्रविष्टिः=MBCS_STAGE_2_FROM_U(सारणी, U_NL);
        यदि(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_प्रविष्टिः, U_NL)!=0 &&
             EBCDIC_NL==MBCS_मानम्_2_FROM_STAGE_2(अष्टकानि, stage2_प्रविष्टिः, U_NL)
        )) {
            प्रत्यागम असत्य;
        }
    }

    यदि(mbcs_सारणी->from_UBytes_दीर्घता>0) {
        /*
         * We _know_ the number of bytes in the fromUnicodeBytes array
         * starting with header.version 4.1.
         */
        sizeofFromUBytes=mbcs_सारणी->from_UBytes_दीर्घता;
    } अन्यथा {
        /*
         * Otherwise:
         * There used to be code to enumerate the fromUnicode
         * trie and find the highest entry, but it was removed in ICU 3.2
         * because it was not tested and caused a low code coverage number.
         * See Jitterbug 3674.
         * This affects only some .cnv file formats with a header.version
         * below 4.1, and only when swaplfnl is requested.
         *
         * ucnvmbcs.c revision 1.99 is the last one with the
         * ucnv_MBCSSizeofFromUBytes() function.
         */
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम असत्य;
    }

    /*
     * The table has an appropriate format.
     * Allocate and build
     * - a modified to-Unicode state table
     * - a modified from-Unicode output array
     * - a converter name string with the swap option appended
     */
    size=
        mbcs_सारणी->गणनम्_अवस्थाः*1024+
        sizeofFromUBytes+
        UCNV_MAX_CONVERTER_NAME_दीर्घता+20;
    p = static_cast<uint8_t*>(uprv_malloc(size));
    यदि(p==शून्यसूचकः) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम असत्य;
    }

    /* copy and modify the to-Unicode state table */
    new_State_सारणी = reinterpret_cast<int32_t(*)[256]>(p);
    uprv_memcpy(new_State_सारणी, mbcs_सारणी->state_सारणी, mbcs_सारणी->गणनम्_अवस्थाः*1024);

    new_State_सारणी[0][EBCDIC_LF]=MBCS_प्रविष्टिः_FINAL(0, MBCS_STATE_VALID_प्रत्यक्षम्_16, U_NL);
    new_State_सारणी[0][EBCDIC_NL]=MBCS_प्रविष्टिः_FINAL(0, MBCS_STATE_VALID_प्रत्यक्षम्_16, U_LF);

    /* copy and modify the from-Unicode result table */
    newResults = reinterpret_cast<uint16_t*>(new_State_सारणी[mbcs_सारणी->गणनम्_अवस्थाः]);
    uprv_memcpy(newResults, अष्टकानि, sizeofFromUBytes);

    /* conveniently, the table access macros work on the left side of expressions */
    यदि(mbcs_सारणी->outputType==MBCS_OUTPUT_1) {
        MBCS_SINGLE_परिणाम_FROM_U(सारणी, newResults, U_LF)=EBCDIC_RT_NL;
        MBCS_SINGLE_परिणाम_FROM_U(सारणी, newResults, U_NL)=EBCDIC_RT_LF;
    } अन्यथा /* MBCS_OUTPUT_2_SISO */ {
        stage2_प्रविष्टिः=MBCS_STAGE_2_FROM_U(सारणी, U_LF);
        MBCS_मानम्_2_FROM_STAGE_2(newResults, stage2_प्रविष्टिः, U_LF)=EBCDIC_NL;

        stage2_प्रविष्टिः=MBCS_STAGE_2_FROM_U(सारणी, U_NL);
        MBCS_मानम्_2_FROM_STAGE_2(newResults, stage2_प्रविष्टिः, U_NL)=EBCDIC_LF;
    }

    /* set the canonical converter name */
    name = reinterpret_cast<वर्ण*>(newResults) + sizeofFromUBytes;
    uprv_strcpy(name, shared_दत्त->static_दत्त->name);
    uprv_strcat(name, UCNV_SWAP_LFNL_OPTION_STRING);

    /* set the pointers */
    icu::umtx_तालकम्(शून्यसूचकः);
    यदि(mbcs_सारणी->swap_LFNLState_सारणी==शून्यसूचकः) {
        mbcs_सारणी->swap_LFNLState_सारणी=new_State_सारणी;
        mbcs_सारणी->swap_LFNLFrom_Unicode_अष्टकानि = reinterpret_cast<uint8_t*>(newResults);
        mbcs_सारणी->swapLFNLName=name;

        new_State_सारणी=शून्यसूचकः;
    }
    icu::umtx_unlock(शून्यसूचकः);

    /* release the allocated memory if another thread beat us to it */
    यदि(new_State_सारणी!=शून्यसूचकः) {
        uprv_free(new_State_सारणी);
    }
    प्रत्यागम सत्य;
}

/* reconstitute omitted fromUnicode data ------------------------------------ */

/* for details, compare with genmbcs.c MBCSAddFromUnicode() and transformEUC() */
स्थिर UBool U_CALLCONV
writeStage3Roundtrip(ध्रुव शून्य *context, uint32_t value, UChar32 संहिता_Points[32]) {
    UConverterMBCSTable *mbcs_सारणी=(UConverterMBCSTable *)context;
    ध्रुव uint16_t *सारणी;
    uint32_t *stage2;
    uint8_t *अष्टकानि, *p;
    UChar32 c;
    int32_t i, st3;

    सारणी=mbcs_सारणी->from_Unicode_सारणी;
    अष्टकानि = const_cast<uint8_t*>(mbcs_सारणी->from_Unicode_अष्टकानि);

    /* for EUC outputTypes, modify the value like genmbcs.c's transformEUC() */
    विकल्प(mbcs_सारणी->outputType) {
    पक्ष MBCS_OUTPUT_3_EUC:
        यदि(value<=0xffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } अन्यथा यदि(value<=0x8effff) {
            /* code set 2 */
            value&=0x7fff;
        } अन्यथा /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7f;
        }
        भङ्ग;
    पक्ष MBCS_OUTPUT_4_EUC:
        यदि(value<=0xffffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } अन्यथा यदि(value<=0x8effffff) {
            /* code set 2 */
            value&=0x7fffff;
        } अन्यथा /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7fff;
        }
        भङ्ग;
    स्वभाव:
        भङ्ग;
    }

    प्रति(i=0; i<=0x1f; ++value, ++i) {
        c=संहिता_Points[i];
        यदि(c<0) {
            अनुवर्त;
        }

        /* locate the stage 2 & 3 data */
        stage2=((uint32_t *)सारणी)+सारणी[c>>10]+((c>>4)&0x3f);
        p=अष्टकानि;
        st3 = static_cast<int32_t>(static_cast<uint16_t>(*stage2)) * 16 + (c & 0xf);

        /* write the codepage bytes into stage 3 */
        विकल्प(mbcs_सारणी->outputType) {
        पक्ष MBCS_OUTPUT_3:
        पक्ष MBCS_OUTPUT_4_EUC:
            p+=st3*3;
            p[0] = static_cast<uint8_t>(value >> 16);
            p[1] = static_cast<uint8_t>(value >> 8);
            p[2] = static_cast<uint8_t>(value);
            भङ्ग;
        पक्ष MBCS_OUTPUT_4:
            reinterpret_cast<uint32_t*>(p)[st3] = value;
            भङ्ग;
        स्वभाव:
            /* 2 bytes per character */
            reinterpret_cast<uint16_t*>(p)[st3] = static_cast<uint16_t>(value);
            भङ्ग;
        }

        /* set the roundtrip flag */
        *stage2|=(1UL<<(16+(c&0xf)));
    }
    प्रत्यागम सत्य;
 }

स्थिर शून्य
reconstitute_दत्त(UConverterMBCSTable *mbcs_सारणी,
                 uint32_t stage1_दीर्घता, uint32_t stage2_दीर्घता,
                 uint32_t पूर्णम्_Stage2_दीर्घता,  /* lengths are numbers of units, not bytes */
                 UError_संहिता *p_Error_संहिता) {
    uint16_t *stage1;
    uint32_t *stage2;
    uint32_t दत्त_दीर्घता=stage1_दीर्घता*2+पूर्णम्_Stage2_दीर्घता*4+mbcs_सारणी->from_UBytes_दीर्घता;
    mbcs_सारणी->reconstituted_दत्त = static_cast<uint8_t*>(uprv_malloc(दत्त_दीर्घता));
    यदि(mbcs_सारणी->reconstituted_दत्त==शून्यसूचकः) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    uprv_memset(mbcs_सारणी->reconstituted_दत्त, 0, दत्त_दीर्घता);

    /* copy existing data and reroute the pointers */
    stage1 = reinterpret_cast<uint16_t*>(mbcs_सारणी->reconstituted_दत्त);
    uprv_memcpy(stage1, mbcs_सारणी->from_Unicode_सारणी, stage1_दीर्घता*2);

    stage2 = reinterpret_cast<uint32_t*>(stage1 + stage1_दीर्घता);
    uprv_memcpy(stage2+(पूर्णम्_Stage2_दीर्घता-stage2_दीर्घता),
                mbcs_सारणी->from_Unicode_सारणी+stage1_दीर्घता,
                stage2_दीर्घता*4);

    mbcs_सारणी->from_Unicode_सारणी=stage1;
    mbcs_सारणी->from_Unicode_अष्टकानि = reinterpret_cast<uint8_t*>(stage2 + पूर्णम्_Stage2_दीर्घता);

    /* indexes into stage 2 count from the bottom of the fromUnicodeTable */
    stage2 = reinterpret_cast<uint32_t*>(stage1);

    /* reconstitute the initial part of stage 2 from the mbcsIndex */
    {
        int32_t stage_UTF8_दीर्घता = (static_cast<int32_t>(mbcs_सारणी->maxFastUChar) + 1) >> 6;
        int32_t stage_UTF8_सूचकांक=0;
        int32_t st1, st2, st3, i;

        प्रति(st1=0; stage_UTF8_सूचकांक<stage_UTF8_दीर्घता; ++st1) {
            st2=stage1[st1];
            यदि (st2 != static_cast<int32_t>(stage1_दीर्घता) / 2) {
                /* each stage 2 block has 64 entries corresponding to 16 entries in the mbcsIndex */
                प्रति(i=0; i<16; ++i) {
                    st3=mbcs_सारणी->mbcs_सूचकांक[stage_UTF8_सूचकांक++];
                    यदि(st3!=0) {
                        /* an stage 2 entry's index is per stage 3 16-block, not per stage 3 entry */
                        st3>>=4;
                        /*
                         * 4 stage 2 entries point to 4 consecutive stage 3 16-blocks which are
                         * allocated together as a single 64-block for access from the mbcsIndex
                         */
                        stage2[st2++]=st3++;
                        stage2[st2++]=st3++;
                        stage2[st2++]=st3++;
                        stage2[st2++]=st3;
                    } अन्यथा {
                        /* no stage 3 block, skip */
                        st2+=4;
                    }
                }
            } अन्यथा {
                /* no stage 2 block, skip */
                stage_UTF8_सूचकांक+=16;
            }
        }
    }

    /* reconstitute fromUnicodeBytes with roundtrips from toUnicode data */
    ucnv_MBCSEnum_गन्तव्यम्_Unicode(mbcs_सारणी, writeStage3Roundtrip, mbcs_सारणी, p_Error_संहिता);
}

/* MBCS setup functions ----------------------------------------------------- */

स्थिर शून्य U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_दत्त *shared_दत्त,
          UConverter_आरोपय_तर्काः *p_तर्काः,
          ध्रुव uint8_t *raw,
          UError_संहिता *p_Error_संहिता) {
    UDataInfo info;
    UConverterMBCSTable *mbcs_सारणी=&shared_दत्त->mbcs;
    _MBCSHeader *शीर्षकम्=(_MBCSHeader *)raw;
    uint32_t विस्थापनम्;
    uint32_t शीर्षकम्_दीर्घता;
    UBool noFromU=असत्य;

    यदि(शीर्षकम्->संस्करणम्[0]==4) {
        शीर्षकम्_दीर्घता=MBCS_शीर्षकम्_V4_दीर्घता;
    } अन्यथा यदि(शीर्षकम्->संस्करणम्[0]==5 && शीर्षकम्->संस्करणम्[1]>=3 &&
              (शीर्षकम्->options&MBCS_OPT_UNKNOWN_INCOMPATIBLE_आवरणम्)==0) {
        शीर्षकम्_दीर्घता=शीर्षकम्->options&MBCS_OPT_दीर्घता_आवरणम्;
        noFromU = static_cast<UBool>((शीर्षकम्->options & MBCS_OPT_NO_FROM_U) != 0);
    } अन्यथा {
        *p_Error_संहिता=U_INVALID_सारणी_FORMAT;
        प्रत्यागम;
    }

    mbcs_सारणी->outputType = static_cast<uint8_t>(शीर्षकम्->flags);
    यदि(noFromU && mbcs_सारणी->outputType==MBCS_OUTPUT_1) {
        *p_Error_संहिता=U_INVALID_सारणी_FORMAT;
        प्रत्यागम;
    }

    /* extension data, header version 4.2 and higher */
    विस्थापनम्=शीर्षकम्->flags>>8;
    यदि(विस्थापनम्!=0) {
        mbcs_सारणी->extIndexes = reinterpret_cast<ध्रुव int32_t*>(raw + विस्थापनम्);
    }

    यदि(mbcs_सारणी->outputType==MBCS_OUTPUT_EXT_ONLY) {
        UConverter_आरोपय_तर्काः तर्काः_2=UCNV_आरोपय_तर्काः_INITIALIZER;
        UConverter_Shared_दत्त *आधारः_Shared_दत्त;
        ध्रुव int32_t *extIndexes;
        ध्रुव वर्ण *आधारः_Name;

        /* extension-only file, load the base table and set values appropriately */
        यदि((extIndexes=mbcs_सारणी->extIndexes)==शून्यसूचकः) {
            /* extension-only file without extension */
            *p_Error_संहिता=U_INVALID_सारणी_FORMAT;
            प्रत्यागम;
        }

        यदि(p_तर्काः->nestedLoads!=1) {
            /* an extension table must not be loaded as a base table */
            *p_Error_संहिता=U_INVALID_सारणी_FILE;
            प्रत्यागम;
        }

        /* load the base table */
        आधारः_Name = reinterpret_cast<ध्रुव वर्ण*>(शीर्षकम्) + शीर्षकम्_दीर्घता * 4;
        यदि(0==uprv_strcmp(आधारः_Name, shared_दत्त->static_दत्त->name)) {
            /* forbid loading this same extension-only file */
            *p_Error_संहिता=U_INVALID_सारणी_FORMAT;
            प्रत्यागम;
        }

        /* TODO parse package name out of the prefix of the base name in the extension .cnv file? */
        तर्काः_2.size=परिमाण(UConverter_आरोपय_तर्काः);
        तर्काः_2.nestedLoads=2;
        तर्काः_2.onlyTestIsLoadable=p_तर्काः->onlyTestIsLoadable;
        तर्काः_2.reserved=p_तर्काः->reserved;
        तर्काः_2.options=p_तर्काः->options;
        तर्काः_2.pkg=p_तर्काः->pkg;
        तर्काः_2.name=आधारः_Name;
        आधारः_Shared_दत्त=ucnv_आरोपय(&तर्काः_2, p_Error_संहिता);
        यदि(U_FAILURE(*p_Error_संहिता)) {
            प्रत्यागम;
        }
        यदि( आधारः_Shared_दत्त->static_दत्त->conversionType!=UCNV_MBCS ||
            आधारः_Shared_दत्त->mbcs.आधारः_Shared_दत्त!=शून्यसूचकः
        ) {
            ucnv_unload(आधारः_Shared_दत्त);
            *p_Error_संहिता=U_INVALID_सारणी_FORMAT;
            प्रत्यागम;
        }
        यदि(p_तर्काः->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            ucnv_unload(आधारः_Shared_दत्त);
            प्रत्यागम;
        }

        /* copy the base table data */
        uprv_memcpy(mbcs_सारणी, &आधारः_Shared_दत्त->mbcs, परिमाण(UConverterMBCSTable));

        /* overwrite values with relevant ones for the extension converter */
        mbcs_सारणी->आधारः_Shared_दत्त=आधारः_Shared_दत्त;
        mbcs_सारणी->extIndexes=extIndexes;

        /*
         * It would be possible to share the swapLFNL data with a base converter,
         * but the generated name would have to be different, and the memory
         * would have to be free'd only once.
         * It is easier to just create the data for the extension converter
         * separately when it is requested.
         */
        mbcs_सारणी->swap_LFNLState_सारणी=शून्यसूचकः;
        mbcs_सारणी->swap_LFNLFrom_Unicode_अष्टकानि=शून्यसूचकः;
        mbcs_सारणी->swapLFNLName=शून्यसूचकः;

        /*
         * The reconstitutedData must be deleted only when the base converter
         * is unloaded.
         */
        mbcs_सारणी->reconstituted_दत्त=शून्यसूचकः;

        /*
         * Set a special, runtime-only outputType if the extension converter
         * is a DBCS version of a base converter that also maps single bytes.
         */
        यदि( shared_दत्त->static_दत्त->conversionType==UCNV_DBCS ||
                (shared_दत्त->static_दत्त->conversionType==UCNV_MBCS &&
                 shared_दत्त->static_दत्त->min_अष्टकानि_प्रत्येकम्_Char>=2)
        ) {
            यदि(आधारः_Shared_दत्त->mbcs.outputType==MBCS_OUTPUT_2_SISO) {
                /* the base converter is SI/SO-stateful */
                int32_t प्रविष्टिः;

                /* get the dbcs state from the state table entry for SO=0x0e */
                प्रविष्टिः=mbcs_सारणी->state_सारणी[0][0xe];
                यदि( MBCS_प्रविष्टिः_IS_FINAL(प्रविष्टिः) &&
                    MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः)==MBCS_STATE_CHANGE_ONLY &&
                    MBCS_प्रविष्टिः_FINAL_STATE(प्रविष्टिः)!=0
                ) {
                    mbcs_सारणी->dbcsOnlyState = static_cast<uint8_t>(MBCS_प्रविष्टिः_FINAL_STATE(प्रविष्टिः));

                    mbcs_सारणी->outputType=MBCS_OUTPUT_DBCS_ONLY;
                }
            } अन्यथा यदि(
                आधारः_Shared_दत्त->static_दत्त->conversionType==UCNV_MBCS &&
                आधारः_Shared_दत्त->static_दत्त->min_अष्टकानि_प्रत्येकम्_Char==1 &&
                आधारः_Shared_दत्त->static_दत्त->max_अष्टकानि_प्रत्येकम्_Char==2 &&
                mbcs_सारणी->गणनम्_अवस्थाः<=127
            ) {
                /* non-stateful base converter, need to modify the state table */
                int32_t (*new_State_सारणी)[256];
                int32_t *state;
                int32_t i, count;

                /* allocate a new state table and copy the base state table contents */
                count=mbcs_सारणी->गणनम्_अवस्थाः;
                new_State_सारणी = static_cast<int32_t(*)[256]>(uprv_malloc((count + 1) * 1024));
                यदि(new_State_सारणी==शून्यसूचकः) {
                    ucnv_unload(आधारः_Shared_दत्त);
                    *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                    प्रत्यागम;
                }

                uprv_memcpy(new_State_सारणी, mbcs_सारणी->state_सारणी, count*1024);

                /* change all final single-byte entries to go to a new all-illegal state */
                state=new_State_सारणी[0];
                प्रति(i=0; i<256; ++i) {
                    यदि(MBCS_प्रविष्टिः_IS_FINAL(state[i])) {
                        state[i]=MBCS_प्रविष्टिः_TRANSITION(count, 0);
                    }
                }

                /* build the new all-illegal state */
                state=new_State_सारणी[count];
                प्रति(i=0; i<256; ++i) {
                    state[i]=MBCS_प्रविष्टिः_FINAL(0, MBCS_STATE_ILLEGAL, 0);
                }
                mbcs_सारणी->state_सारणी=(ध्रुव int32_t (*)[256])new_State_सारणी;
                mbcs_सारणी->गणनम्_अवस्थाः = static_cast<uint8_t>(count + 1);
                mbcs_सारणी->state_सारणी_Owned=सत्य;

                mbcs_सारणी->outputType=MBCS_OUTPUT_DBCS_ONLY;
            }
        }

        /*
         * unlike below for files with base tables, do not get the unicodeMask
         * from the sharedData; instead, use the base table's unicodeMask,
         * which we copied in the memcpy above;
         * this is necessary because the static data unicodeMask, especially
         * the UCNV_HAS_SUPPLEMENTARY flag, is part of the base table data
         */
    } अन्यथा {
        /* conversion file with a base table; an additional extension table is optional */
        /* make sure that the output type is known */
        विकल्प(mbcs_सारणी->outputType) {
        पक्ष MBCS_OUTPUT_1:
        पक्ष MBCS_OUTPUT_2:
        पक्ष MBCS_OUTPUT_3:
        पक्ष MBCS_OUTPUT_4:
        पक्ष MBCS_OUTPUT_3_EUC:
        पक्ष MBCS_OUTPUT_4_EUC:
        पक्ष MBCS_OUTPUT_2_SISO:
            /* OK */
            भङ्ग;
        स्वभाव:
            *p_Error_संहिता=U_INVALID_सारणी_FORMAT;
            प्रत्यागम;
        }
        यदि(p_तर्काः->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            प्रत्यागम;
        }

        mbcs_सारणी->गणनम्_अवस्थाः = static_cast<uint8_t>(शीर्षकम्->गणनम्_अवस्थाः);
        mbcs_सारणी->गणनम्_गन्तव्यम्_UFallbacks=शीर्षकम्->गणनम्_गन्तव्यम्_UFallbacks;
        mbcs_सारणी->state_सारणी = reinterpret_cast<ध्रुव int32_t(*)[256]>(raw + शीर्षकम्_दीर्घता * 4);
        mbcs_सारणी->गन्तव्यम्_UFallbacks = reinterpret_cast<ध्रुव _MBCSToUFallback*>(mbcs_सारणी->state_सारणी + शीर्षकम्->गणनम्_अवस्थाः);
        mbcs_सारणी->unicode_संहिता_Units = reinterpret_cast<ध्रुव uint16_t*>(raw + शीर्षकम्->विस्थापनम्_गन्तव्यम्_UCode_Units);

        mbcs_सारणी->from_Unicode_सारणी = reinterpret_cast<ध्रुव uint16_t*>(raw + शीर्षकम्->विस्थापनम्_From_UTable);
        mbcs_सारणी->from_Unicode_अष्टकानि = raw + शीर्षकम्->विस्थापनम्_From_UBytes;
        mbcs_सारणी->from_UBytes_दीर्घता=शीर्षकम्->from_UBytes_दीर्घता;

        /*
         * converter versions 6.1 and up contain a unicodeMask that is
         * used here to select the most efficient function implementations
         */
        info.size=परिमाण(UDataInfo);
        udata_getInfo((UData_स्मृतिः *)shared_दत्त->दत्त_स्मृतिः, &info);
        यदि(info.format_संस्करणम्[0]>6 || (info.format_संस्करणम्[0]==6 && info.format_संस्करणम्[1]>=1)) {
            /* mask off possible future extensions to be safe */
            mbcs_सारणी->unicode_आवरणम् = static_cast<uint8_t>(shared_दत्त->static_दत्त->unicode_आवरणम् & 3);
        } अन्यथा {
            /* for older versions, assume worst case: contains anything possible (prevent over-optimizations) */
            mbcs_सारणी->unicode_आवरणम्=UCNV_HAS_SUPPLEMENTARY|UCNV_HAS_SURROGATES;
        }

        /*
         * _MBCSHeader.version 4.3 adds utf8Friendly data structures.
         * Check for the header version, SBCS vs. MBCS, and for whether the
         * data structures are optimized for code points as high as what the
         * runtime code is designed for.
         * The implementation does not handle mapping tables with entries for
         * unpaired surrogates.
         */
        यदि( शीर्षकम्->संस्करणम्[1]>=3 &&
            (mbcs_सारणी->unicode_आवरणम्&UCNV_HAS_SURROGATES)==0 &&
            (mbcs_सारणी->गणनम्_अवस्थाः==1 ?
                (शीर्षकम्->संस्करणम्[2]>=(SBCS_FAST_MAX>>8)) :
                (शीर्षकम्->संस्करणम्[2]>=(MBCS_FAST_MAX>>8))
            )
        ) {
            mbcs_सारणी->utf8Friendly=सत्य;

            यदि(mbcs_सारणी->गणनम्_अवस्थाः==1) {
                /*
                 * SBCS: Stage 3 is allocated in 64-entry blocks for U+0000..SBCS_FAST_MAX or higher.
                 * Build a table with indexes to each block, to be used instead of
                 * the regular stage 1/2 table.
                 */
                int32_t i;
                प्रति(i=0; i<(SBCS_FAST_सीमा>>6); ++i) {
                    mbcs_सारणी->sbcs_सूचकांक[i]=mbcs_सारणी->from_Unicode_सारणी[mbcs_सारणी->from_Unicode_सारणी[i>>4]+((i<<2)&0x3c)];
                }
                /* set SBCS_FAST_MAX to reflect the reach of sbcsIndex[] even if header->version[2]>(SBCS_FAST_MAX>>8) */
                mbcs_सारणी->maxFastUChar=SBCS_FAST_MAX;
            } अन्यथा {
                /*
                 * MBCS: Stage 3 is allocated in 64-entry blocks for U+0000..MBCS_FAST_MAX or higher.
                 * The .cnv file is prebuilt with an additional stage table with indexes
                 * to each block.
                 */
                mbcs_सारणी->mbcs_सूचकांक = reinterpret_cast<ध्रुव uint16_t*>(
                    mbcs_सारणी->from_Unicode_अष्टकानि +
                     (noFromU ? 0 : mbcs_सारणी->from_UBytes_दीर्घता));
                mbcs_सारणी->maxFastUChar = (static_cast<char16_t>(शीर्षकम्->संस्करणम्[2]) << 8) | 0xff;
            }
        }

        /* calculate a bit set of 4 ASCII characters per bit that round-trip to ASCII bytes */
        {
            uint32_t asciiRoundtrips=0xffffffff;
            int32_t i;

            प्रति(i=0; i<0x80; ++i) {
                यदि(mbcs_सारणी->state_सारणी[0][i]!=MBCS_प्रविष्टिः_FINAL(0, MBCS_STATE_VALID_प्रत्यक्षम्_16, i)) {
                    asciiRoundtrips &= ~(static_cast<uint32_t>(1) << (i >> 2));
                }
            }
            mbcs_सारणी->asciiRoundtrips=asciiRoundtrips;
        }

        यदि(noFromU) {
            uint32_t stage1_दीर्घता=
                mbcs_सारणी->unicode_आवरणम्&UCNV_HAS_SUPPLEMENTARY ?
                    0x440 : 0x40;
            uint32_t stage2_दीर्घता=
                (शीर्षकम्->विस्थापनम्_From_UBytes-शीर्षकम्->विस्थापनम्_From_UTable)/4-
                stage1_दीर्घता/2;
            reconstitute_दत्त(mbcs_सारणी, stage1_दीर्घता, stage2_दीर्घता, शीर्षकम्->पूर्णम्_Stage2_दीर्घता, p_Error_संहिता);
        }
    }

    /* Set the impl pointer here so that it is set for both extension-only and base tables. */
    यदि(mbcs_सारणी->utf8Friendly) {
        यदि(mbcs_सारणी->गणनम्_अवस्थाः==1) {
            shared_दत्त->impl=&_SBCSUTF8Impl;
        } अन्यथा {
            यदि(mbcs_सारणी->outputType==MBCS_OUTPUT_2) {
                shared_दत्त->impl=&_DBCSUTF8Impl;
            }
        }
    }

    यदि(mbcs_सारणी->outputType==MBCS_OUTPUT_DBCS_ONLY || mbcs_सारणी->outputType==MBCS_OUTPUT_2_SISO) {
        /*
         * MBCS_OUTPUT_DBCS_ONLY: No SBCS mappings, therefore ASCII does not roundtrip.
         * MBCS_OUTPUT_2_SISO: Bypass the ASCII fastpath to handle prevLength correctly.
         */
        mbcs_सारणी->asciiRoundtrips=0;
    }
}

स्थिर शून्य U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_दत्त *shared_दत्त) {
    UConverterMBCSTable *mbcs_सारणी=&shared_दत्त->mbcs;

    यदि(mbcs_सारणी->swap_LFNLState_सारणी!=शून्यसूचकः) {
        uprv_free(mbcs_सारणी->swap_LFNLState_सारणी);
    }
    यदि(mbcs_सारणी->state_सारणी_Owned) {
        uprv_free((शून्य *)mbcs_सारणी->state_सारणी);
    }
    यदि(mbcs_सारणी->आधारः_Shared_दत्त!=शून्यसूचकः) {
        ucnv_unload(mbcs_सारणी->आधारः_Shared_दत्त);
    }
    यदि(mbcs_सारणी->reconstituted_दत्त!=शून्यसूचकः) {
        uprv_free(mbcs_सारणी->reconstituted_दत्त);
    }
}

स्थिर शून्य U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_आरोपय_तर्काः *p_तर्काः,
              UError_संहिता *p_Error_संहिता) {
    UConverterMBCSTable *mbcs_सारणी;
    ध्रुव int32_t *extIndexes;
    uint8_t outputType;
    int8_t max_अष्टकानि_प्रत्येकम्_UChar;

    यदि(p_तर्काः->onlyTestIsLoadable) {
        प्रत्यागम;
    }

    mbcs_सारणी=&cnv->shared_दत्त->mbcs;
    outputType=mbcs_सारणी->outputType;

    यदि(outputType==MBCS_OUTPUT_DBCS_ONLY) {
        /* the swaplfnl option does not apply, remove it */
        cnv->options=p_तर्काः->options&=~UCNV_OPTION_SWAP_LFNL;
    }

    यदि((p_तर्काः->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        /* do this because double-checked locking is broken */
        UBool isCached;

        icu::umtx_तालकम्(शून्यसूचकः);
        isCached=mbcs_सारणी->swap_LFNLState_सारणी!=शून्यसूचकः;
        icu::umtx_unlock(शून्यसूचकः);

        यदि(!isCached) {
            यदि(!_EBCDICSwapLFNL(cnv->shared_दत्त, p_Error_संहिता)) {
                यदि(U_FAILURE(*p_Error_संहिता)) {
                    प्रत्यागम; /* something went wrong */
                }

                /* the option does not apply, remove it */
                cnv->options=p_तर्काः->options&=~UCNV_OPTION_SWAP_LFNL;
            }
        }
    }

    यदि(uprv_strstr(p_तर्काः->name, "18030")!=शून्यसूचकः) {
        यदि(uprv_strstr(p_तर्काः->name, "gb18030")!=शून्यसूचकः || uprv_strstr(p_तर्काः->name, "GB18030")!=शून्यसूचकः) {
            /* set a flag for GB 18030 mode, which changes the callback behavior */
            cnv->options|=_MBCS_OPTION_GB18030;
        }
    } अन्यथा यदि((uprv_strstr(p_तर्काः->name, "KEIS")!=शून्यसूचकः) || (uprv_strstr(p_तर्काः->name, "keis")!=शून्यसूचकः)) {
        /* set a flag for KEIS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_KEIS;
    } अन्यथा यदि((uprv_strstr(p_तर्काः->name, "JEF")!=शून्यसूचकः) || (uprv_strstr(p_तर्काः->name, "jef")!=शून्यसूचकः)) {
        /* set a flag for JEF converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JEF;
    } अन्यथा यदि((uprv_strstr(p_तर्काः->name, "JIPS")!=शून्यसूचकः) || (uprv_strstr(p_तर्काः->name, "jips")!=शून्यसूचकः)) {
        /* set a flag for JIPS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JIPS;
    }

    /* fix maxBytesPerUChar depending on outputType and options etc. */
    यदि(outputType==MBCS_OUTPUT_2_SISO) {
        cnv->max_अष्टकानि_प्रत्येकम्_UChar=3; /* SO+DBCS */
    }

    extIndexes=mbcs_सारणी->extIndexes;
    यदि(extIndexes!=शून्यसूचकः) {
        max_अष्टकानि_प्रत्येकम्_UChar = static_cast<int8_t>(UCNV_GET_MAX_अष्टकानि_प्रत्येकम्_UCHAR(extIndexes));
        यदि(outputType==MBCS_OUTPUT_2_SISO) {
            ++max_अष्टकानि_प्रत्येकम्_UChar; /* SO + multiple DBCS */
        }

        यदि(max_अष्टकानि_प्रत्येकम्_UChar>cnv->max_अष्टकानि_प्रत्येकम्_UChar) {
            cnv->max_अष्टकानि_प्रत्येकम्_UChar=max_अष्टकानि_प्रत्येकम्_UChar;
        }
    }

#यदि 0
    /*
     * documentation of UConverter fields used for status
     * all of these fields are (re)set to 0 by ucnv_bld.c and ucnv_reset()
     */

    /* toUnicode */
    cnv->गन्तव्यम्_Unicode_स्थितिः=0;     /*विस्थापनम्*/
    cnv->बहुलकम्=0;                /* state */
    cnv->गन्तव्यम्_ULength=0;           /* byteIndex */

    /* fromUnicode */
    cnv->fromUChar32=0;
    cnv->from_Unicode_स्थितिः=1;   /* prevLength */
#यदि_अन्तः
}

U_CDECL_BEGIN

स्थिर ध्रुव वर्ण* U_CALLCONV
ucnv_MBCSGetName(ध्रुव UConverter *cnv) {
    यदि((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0 && cnv->shared_दत्त->mbcs.swapLFNLName!=शून्यसूचकः) {
        प्रत्यागम cnv->shared_दत्त->mbcs.swapLFNLName;
    } अन्यथा {
        प्रत्यागम cnv->shared_दत्त->static_दत्त->name;
    }
}
U_CDECL_अन्तः


/* MBCS-to-Unicode conversion functions ------------------------------------- */

स्थिर UChar32 U_CALLCONV
ucnv_MBCSGetFallback(UConverterMBCSTable *mbcs_सारणी, uint32_t विस्थापनम्) {
    ध्रुव _MBCSToUFallback *गन्तव्यम्_UFallbacks;
    uint32_t i, start, सीमा;

    सीमा=mbcs_सारणी->गणनम्_गन्तव्यम्_UFallbacks;
    यदि(सीमा>0) {
        /* do a binary search for the fallback mapping */
        गन्तव्यम्_UFallbacks=mbcs_सारणी->गन्तव्यम्_UFallbacks;
        start=0;
        यावत्(start<सीमा-1) {
            i=(start+सीमा)/2;
            यदि(विस्थापनम्<गन्तव्यम्_UFallbacks[i].विस्थापनम्) {
                सीमा=i;
            } अन्यथा {
                start=i;
            }
        }

        /* did we really find it? */
        यदि(विस्थापनम्==गन्तव्यम्_UFallbacks[start].विस्थापनम्) {
            प्रत्यागम गन्तव्यम्_UFallbacks[start].संहिता_Point;
        }
    }

    प्रत्यागम 0xfffe;
}

/* This version of ucnv_MBCSToUnicodeWithOffsets() is optimized for single-byte, single-state codepages. */
स्थिर शून्य
ucnv_MBCSSingle_गन्तव्यम्_Unicode_With_Offsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                                UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव uint8_t *source, *source_सीमा;
    char16_t *target;
    ध्रुव char16_t *target_सीमा;
    int32_t *offsets;

    ध्रुव int32_t (*state_सारणी)[256];

    int32_t source_सूचकांक;

    int32_t प्रविष्टिः;
    char16_t c;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    source = reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->source);
    source_सीमा = reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->source_सीमा);
    target=p_तर्काः->target;
    target_सीमा=p_तर्काः->target_सीमा;
    offsets=p_तर्काः->offsets;

    यदि((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        state_सारणी=(ध्रुव int32_t (*)[256])cnv->shared_दत्त->mbcs.swap_LFNLState_सारणी;
    } अन्यथा {
        state_सारणी=cnv->shared_दत्त->mbcs.state_सारणी;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक=0;

    /* conversion loop */
    यावत्(source<source_सीमा) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        यदि(target>=target_सीमा) {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }

        प्रविष्टिः=state_सारणी[0][*source++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        यदि(MBCS_प्रविष्टिः_FINAL_IS_VALID_प्रत्यक्षम्_16(प्रविष्टिः)) {
            /* output BMP code point */
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            यदि(offsets!=शून्यसूचकः) {
                *offsets++=source_सूचकांक;
            }

            /* normal end of action codes: prepare for a new character */
            ++source_सूचकांक;
            अनुवर्त;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः));
        यदि(action==MBCS_STATE_VALID_प्रत्यक्षम्_20 ||
           (action==MBCS_STATE_FALLBACK_प्रत्यक्षम्_20 && UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv))
        ) {
            प्रविष्टिः=MBCS_प्रविष्टिः_FINAL_मानम्(प्रविष्टिः);
            /* output surrogate pair */
            *target++ = static_cast<char16_t>(0xd800 | static_cast<char16_t>(प्रविष्टिः >> 10));
            यदि(offsets!=शून्यसूचकः) {
                *offsets++=source_सूचकांक;
            }
            c = static_cast<char16_t>(0xdc00 | static_cast<char16_t>(प्रविष्टिः & 0x3ff));
            यदि(target<target_सीमा) {
                *target++=c;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
            } अन्यथा {
                /* target overflow */
                cnv->UChar_Error_बफर[0]=c;
                cnv->UChar_Error_बफर_दीर्घता=1;
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }

            ++source_सूचकांक;
            अनुवर्त;
        } अन्यथा यदि(action==MBCS_STATE_FALLBACK_प्रत्यक्षम्_16) {
            यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }

                ++source_सूचकांक;
                अनुवर्त;
            }
        } अन्यथा यदि(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } अन्यथा यदि(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
        } अन्यथा {
            /* reserved, must never occur */
            ++source_सूचकांक;
            अनुवर्त;
        }

        यदि(U_FAILURE(*p_Error_संहिता)) {
            /* callback(illegal) */
            भङ्ग;
        } अन्यथा /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_तर्काः->source = reinterpret_cast<ध्रुव वर्ण*>(source);
            cnv->गन्तव्यम्_UBytes[0]=*(source-1);
            cnv->गन्तव्यम्_ULength=ext_गन्तव्यम्_U(cnv, cnv->shared_दत्त,
                                    1, &source, source_सीमा,
                                    &target, target_सीमा,
                                    &offsets, source_सूचकांक,
                                    p_तर्काः->flush,
                                    p_Error_संहिता);
            source_सूचकांक += 1 + static_cast<int32_t>(source - reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->source));

            यदि(U_FAILURE(*p_Error_संहिता)) {
                /* not mappable or buffer overflow */
                भङ्ग;
            }
        }
    }

    /* write back the updated pointers */
    p_तर्काः->source = reinterpret_cast<ध्रुव वर्ण*>(source);
    p_तर्काः->target=target;
    p_तर्काः->offsets=offsets;
}

/*
 * This version of ucnv_MBCSSingleToUnicodeWithOffsets() is optimized for single-byte, single-state codepages
 * that only map to and from the BMP.
 * In addition to single-byte optimizations, the offset calculations
 * become much easier.
 */
स्थिर शून्य
ucnv_MBCSSingle_गन्तव्यम्_BMPWith_Offsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                            UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव uint8_t *source, *source_सीमा, *चरमः_Source;
    char16_t *target;
    int32_t target_क्षमता, length;
    int32_t *offsets;

    ध्रुव int32_t (*state_सारणी)[256];

    int32_t source_सूचकांक;

    int32_t प्रविष्टिः;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    source = reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->source);
    source_सीमा = reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->source_सीमा);
    target=p_तर्काः->target;
    target_क्षमता = static_cast<int32_t>(p_तर्काः->target_सीमा - p_तर्काः->target);
    offsets=p_तर्काः->offsets;

    यदि((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        state_सारणी=(ध्रुव int32_t (*)[256])cnv->shared_दत्त->mbcs.swap_LFNLState_सारणी;
    } अन्यथा {
        state_सारणी=cnv->shared_दत्त->mbcs.state_सारणी;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक=0;
    चरमः_Source=source;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(source_सीमा - source);
    यदि(length<target_क्षमता) {
        target_क्षमता=length;
    }

#यदि MBCS_UNROLL_SINGLE_गन्तव्यम्_BMP
    /* unrolling makes it faster on Pentium III/Windows 2000 */
    /* unroll the loop with the most common case */
unrolled:
    यदि(target_क्षमता>=16) {
        int32_t count, loops, oredEntries;

        loops=count=target_क्षमता>>4;
        कुरु {
            oredEntries=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            oredEntries|=प्रविष्टिः=state_सारणी[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));

            /* were all 16 entries really valid? */
            यदि(!MBCS_प्रविष्टिः_FINAL_IS_VALID_प्रत्यक्षम्_16(oredEntries)) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                भङ्ग;
            }
        } यावत्(--count>0);
        count=loops-count;
        target_क्षमता-=16*count;

        यदि(offsets!=शून्यसूचकः) {
            चरमः_Source+=16*count;
            यावत्(count>0) {
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                --count;
            }
        }
    }
#यदि_अन्तः

    /* conversion loop */
    यावत्(target_क्षमता > 0 && source < source_सीमा) {
        प्रविष्टिः=state_सारणी[0][*source++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        यदि(MBCS_प्रविष्टिः_FINAL_IS_VALID_प्रत्यक्षम्_16(प्रविष्टिः)) {
            /* output BMP code point */
            *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            --target_क्षमता;
            अनुवर्त;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः));
        यदि(action==MBCS_STATE_FALLBACK_प्रत्यक्षम्_16) {
            यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *target++ = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
                --target_क्षमता;
                अनुवर्त;
            }
        } अन्यथा यदि(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } अन्यथा यदि(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
        } अन्यथा {
            /* reserved, must never occur */
            अनुवर्त;
        }

        /* set offsets since the start or the last extension */
        यदि(offsets!=शून्यसूचकः) {
            int32_t count = static_cast<int32_t>(source - चरमः_Source);

            /* predecrement: do not set the offset for the callback-causing character */
            यावत्(--count>0) {
                *offsets++=source_सूचकांक++;
            }
            /* offset and sourceIndex are now set for the current character */
        }

        यदि(U_FAILURE(*p_Error_संहिता)) {
            /* callback(illegal) */
            भङ्ग;
        } अन्यथा /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            चरमः_Source=source;
            cnv->गन्तव्यम्_UBytes[0]=*(source-1);
            cnv->गन्तव्यम्_ULength=ext_गन्तव्यम्_U(cnv, cnv->shared_दत्त,
                                    1, &source, source_सीमा,
                                    &target, p_तर्काः->target_सीमा,
                                    &offsets, source_सूचकांक,
                                    p_तर्काः->flush,
                                    p_Error_संहिता);
            source_सूचकांक += 1 + static_cast<int32_t>(source - चरमः_Source);

            यदि(U_FAILURE(*p_Error_संहिता)) {
                /* not mappable or buffer overflow */
                भङ्ग;
            }

            /* recalculate the targetCapacity after an extension mapping */
            target_क्षमता = static_cast<int32_t>(p_तर्काः->target_सीमा - target);
            length = static_cast<int32_t>(source_सीमा - source);
            यदि(length<target_क्षमता) {
                target_क्षमता=length;
            }
        }

#यदि MBCS_UNROLL_SINGLE_गन्तव्यम्_BMP
        /* unrolling makes it faster on Pentium III/Windows 2000 */
        गच्छ unrolled;
#यदि_अन्तः
    }

    यदि(U_SUCCESS(*p_Error_संहिता) && source<source_सीमा && target>=p_तर्काः->target_सीमा) {
        /* target is full */
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
    }

    /* set offsets since the start or the last callback */
    यदि(offsets!=शून्यसूचकः) {
        size_t count=source-चरमः_Source;
        यावत्(count>0) {
            *offsets++=source_सूचकांक++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_तर्काः->source = reinterpret_cast<ध्रुव वर्ण*>(source);
    p_तर्काः->target=target;
    p_तर्काः->offsets=offsets;
}

स्थिर UBool
has_Valid_Trail_अष्टकानि(ध्रुव int32_t (*state_सारणी)[256], uint8_t state) {
    ध्रुव int32_t *row=state_सारणी[state];
    int32_t b, प्रविष्टिः;
    /* First test for final entries in this state for some commonly valid byte values. */
    प्रविष्टिः=row[0xa1];
    यदि( !MBCS_प्रविष्टिः_IS_TRANSITION(प्रविष्टिः) &&
        MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः)!=MBCS_STATE_ILLEGAL
    ) {
        प्रत्यागम सत्य;
    }
    प्रविष्टिः=row[0x41];
    यदि( !MBCS_प्रविष्टिः_IS_TRANSITION(प्रविष्टिः) &&
        MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः)!=MBCS_STATE_ILLEGAL
    ) {
        प्रत्यागम सत्य;
    }
    /* Then test for final entries in this state. */
    प्रति(b=0; b<=0xff; ++b) {
        प्रविष्टिः=row[b];
        यदि( !MBCS_प्रविष्टिः_IS_TRANSITION(प्रविष्टिः) &&
            MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः)!=MBCS_STATE_ILLEGAL
        ) {
            प्रत्यागम सत्य;
        }
    }
    /* Then recurse for transition entries. */
    प्रति(b=0; b<=0xff; ++b) {
        प्रविष्टिः=row[b];
        यदि( MBCS_प्रविष्टिः_IS_TRANSITION(प्रविष्टिः) &&
            has_Valid_Trail_अष्टकानि(state_सारणी, static_cast<uint8_t>(MBCS_प्रविष्टिः_TRANSITION_STATE(प्रविष्टिः)))
        ) {
            प्रत्यागम सत्य;
        }
    }
    प्रत्यागम असत्य;
}

/*
 * Is byte b a single/lead byte in this state?
 * Recurse for transition states, because here we don't want to say that
 * b is a lead byte if all byte sequences that start with b are illegal.
 */
स्थिर UBool
isSingleOrLead(ध्रुव int32_t (*state_सारणी)[256], uint8_t state, UBool isDBCSOnly, uint8_t b) {
    ध्रुव int32_t *row=state_सारणी[state];
    int32_t प्रविष्टिः=row[b];
    यदि(MBCS_प्रविष्टिः_IS_TRANSITION(प्रविष्टिः)) {   /* lead byte */
        प्रत्यागम has_Valid_Trail_अष्टकानि(state_सारणी, static_cast<uint8_t>(MBCS_प्रविष्टिः_TRANSITION_STATE(प्रविष्टिः)));
    } अन्यथा {
        uint8_t action = static_cast<uint8_t>(MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः));
        यदि(action==MBCS_STATE_CHANGE_ONLY && isDBCSOnly) {
            प्रत्यागम असत्य;   /* SI/SO are illegal for DBCS-only conversion */
        } अन्यथा {
            प्रत्यागम action!=MBCS_STATE_ILLEGAL;
        }
    }
}

U_CFUNC शून्य
ucnv_MBCSToUnicodeWithOffsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                          UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव uint8_t *source, *source_सीमा;
    char16_t *target;
    ध्रुव char16_t *target_सीमा;
    int32_t *offsets;

    ध्रुव int32_t (*state_सारणी)[256];
    ध्रुव uint16_t *unicode_संहिता_Units;

    uint32_t विस्थापनम्;
    uint8_t state;
    int8_t byte_सूचकांक;
    uint8_t *अष्टकानि;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    int32_t प्रविष्टिः;
    char16_t c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_तर्काः->converter;

    यदि(cnv->pre_गन्तव्यम्_ULength>0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_मेलनम्_गन्तव्यम्_U(cnv, p_तर्काः, -1, p_Error_संहिता);

        यदि(U_FAILURE(*p_Error_संहिता) || cnv->pre_गन्तव्यम्_ULength<0) {
            प्रत्यागम;
        }
    }

    यदि(cnv->shared_दत्त->mbcs.गणनम्_अवस्थाः==1) {
        यदि(!(cnv->shared_दत्त->mbcs.unicode_आवरणम्&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingle_गन्तव्यम्_BMPWith_Offsets(p_तर्काः, p_Error_संहिता);
        } अन्यथा {
            ucnv_MBCSSingle_गन्तव्यम्_Unicode_With_Offsets(p_तर्काः, p_Error_संहिता);
        }
        प्रत्यागम;
    }

    /* set up the local pointers */
    source=(ध्रुव uint8_t *)p_तर्काः->source;
    source_सीमा=(ध्रुव uint8_t *)p_तर्काः->source_सीमा;
    target=p_तर्काः->target;
    target_सीमा=p_तर्काः->target_सीमा;
    offsets=p_तर्काः->offsets;

    यदि((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        state_सारणी=(ध्रुव int32_t (*)[256])cnv->shared_दत्त->mbcs.swap_LFNLState_सारणी;
    } अन्यथा {
        state_सारणी=cnv->shared_दत्त->mbcs.state_सारणी;
    }
    unicode_संहिता_Units=cnv->shared_दत्त->mbcs.unicode_संहिता_Units;

    /* get the converter state from UConverter */
    विस्थापनम्=cnv->गन्तव्यम्_Unicode_स्थितिः;
    byte_सूचकांक=cnv->गन्तव्यम्_ULength;
    अष्टकानि=cnv->गन्तव्यम्_UBytes;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    यदि((state=(uint8_t)(cnv->बहुलकम्))==0) {
        state=cnv->shared_दत्त->mbcs.dbcsOnlyState;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक=byte_सूचकांक==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    /* conversion loop */
    यावत्(source<source_सीमा) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        यदि(target>=target_सीमा) {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }

        यदि(byte_सूचकांक==0) {
            /* optimized loop for 1/2-byte input and BMP output */
            यदि(offsets==शून्यसूचकः) {
                कुरु {
                    प्रविष्टिः=state_सारणी[state][*source];
                    यदि(MBCS_प्रविष्टिः_IS_TRANSITION(प्रविष्टिः)) {
                        state=(uint8_t)MBCS_प्रविष्टिः_TRANSITION_STATE(प्रविष्टिः);
                        विस्थापनम्=MBCS_प्रविष्टिः_TRANSITION_विस्थापनम्(प्रविष्टिः);

                        ++source;
                        यदि( source<source_सीमा &&
                            MBCS_प्रविष्टिः_IS_FINAL(प्रविष्टिः=state_सारणी[state][*source]) &&
                            MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः)==MBCS_STATE_VALID_16 &&
                            (c=unicode_संहिता_Units[विस्थापनम्+MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः)])<0xfffe
                        ) {
                            ++source;
                            *target++=c;
                            state=(uint8_t)MBCS_प्रविष्टिः_FINAL_STATE(प्रविष्टिः); /* typically 0 */
                            विस्थापनम्=0;
                        } अन्यथा {
                            /* set the state and leave the optimized loop */
                            अष्टकानि[0]=*(source-1);
                            byte_सूचकांक=1;
                            भङ्ग;
                        }
                    } अन्यथा {
                        यदि(MBCS_प्रविष्टिः_FINAL_IS_VALID_प्रत्यक्षम्_16(प्रविष्टिः)) {
                            /* output BMP code point */
                            ++source;
                            *target++=(char16_t)MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
                            state=(uint8_t)MBCS_प्रविष्टिः_FINAL_STATE(प्रविष्टिः); /* typically 0 */
                        } अन्यथा {
                            /* leave the optimized loop */
                            भङ्ग;
                        }
                    }
                } यावत्(source<source_सीमा && target<target_सीमा);
            } अन्यथा /* offsets!=nullptr */ {
                कुरु {
                    प्रविष्टिः=state_सारणी[state][*source];
                    यदि(MBCS_प्रविष्टिः_IS_TRANSITION(प्रविष्टिः)) {
                        state=(uint8_t)MBCS_प्रविष्टिः_TRANSITION_STATE(प्रविष्टिः);
                        विस्थापनम्=MBCS_प्रविष्टिः_TRANSITION_विस्थापनम्(प्रविष्टिः);

                        ++source;
                        यदि( source<source_सीमा &&
                            MBCS_प्रविष्टिः_IS_FINAL(प्रविष्टिः=state_सारणी[state][*source]) &&
                            MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः)==MBCS_STATE_VALID_16 &&
                            (c=unicode_संहिता_Units[विस्थापनम्+MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः)])<0xfffe
                        ) {
                            ++source;
                            *target++=c;
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक;
                                source_सूचकांक=(अग्र_Source_सूचकांक+=2);
                            }
                            state=(uint8_t)MBCS_प्रविष्टिः_FINAL_STATE(प्रविष्टिः); /* typically 0 */
                            विस्थापनम्=0;
                        } अन्यथा {
                            /* set the state and leave the optimized loop */
                            ++अग्र_Source_सूचकांक;
                            अष्टकानि[0]=*(source-1);
                            byte_सूचकांक=1;
                            भङ्ग;
                        }
                    } अन्यथा {
                        यदि(MBCS_प्रविष्टिः_FINAL_IS_VALID_प्रत्यक्षम्_16(प्रविष्टिः)) {
                            /* output BMP code point */
                            ++source;
                            *target++=(char16_t)MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक;
                                source_सूचकांक=++अग्र_Source_सूचकांक;
                            }
                            state=(uint8_t)MBCS_प्रविष्टिः_FINAL_STATE(प्रविष्टिः); /* typically 0 */
                        } अन्यथा {
                            /* leave the optimized loop */
                            भङ्ग;
                        }
                    }
                } यावत्(source<source_सीमा && target<target_सीमा);
            }

            /*
             * these tests and break statements could be put inside the loop
             * if C had "break outerLoop" like Java
             */
            यदि(source>=source_सीमा) {
                भङ्ग;
            }
            यदि(target>=target_सीमा) {
                /* target is full */
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }

            ++अग्र_Source_सूचकांक;
            अष्टकानि[byte_सूचकांक++]=*source++;
        } अन्यथा /* byteIndex>0 */ {
            ++अग्र_Source_सूचकांक;
            प्रविष्टिः=state_सारणी[state][अष्टकानि[byte_सूचकांक++]=*source++];
        }

        यदि(MBCS_प्रविष्टिः_IS_TRANSITION(प्रविष्टिः)) {
            state=(uint8_t)MBCS_प्रविष्टिः_TRANSITION_STATE(प्रविष्टिः);
            विस्थापनम्+=MBCS_प्रविष्टिः_TRANSITION_विस्थापनम्(प्रविष्टिः);
            अनुवर्त;
        }

        /* save the previous state for proper extension mapping with SI/SO-stateful converters */
        cnv->बहुलकम्=state;

        /* set the next state early so that we can reuse the entry variable */
        state=(uint8_t)MBCS_प्रविष्टिः_FINAL_STATE(प्रविष्टिः); /* typically 0 */

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action=(uint8_t)(MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः));
        यदि(action==MBCS_STATE_VALID_16) {
            विस्थापनम्+=MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
            c=unicode_संहिता_Units[विस्थापनम्];
            यदि(c<0xfffe) {
                /* output BMP code point */
                *target++=c;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                byte_सूचकांक=0;
            } अन्यथा यदि(c==0xfffe) {
                यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv) && (प्रविष्टिः=(int32_t)ucnv_MBCSGetFallback(&cnv->shared_दत्त->mbcs, विस्थापनम्))!=0xfffe) {
                    /* output fallback BMP code point */
                    *target++=(char16_t)प्रविष्टिः;
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                    byte_सूचकांक=0;
                }
            } अन्यथा {
                /* callback(illegal) */
                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
            }
        } अन्यथा यदि(action==MBCS_STATE_VALID_प्रत्यक्षम्_16) {
            /* output BMP code point */
            *target++=(char16_t)MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
            यदि(offsets!=शून्यसूचकः) {
                *offsets++=source_सूचकांक;
            }
            byte_सूचकांक=0;
        } अन्यथा यदि(action==MBCS_STATE_VALID_16_PAIR) {
            विस्थापनम्+=MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
            c=unicode_संहिता_Units[विस्थापनम्++];
            यदि(c<0xd800) {
                /* output BMP code point below 0xd800 */
                *target++=c;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                byte_सूचकांक=0;
            } अन्यथा यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                /* output roundtrip or fallback surrogate pair */
                *target++=(char16_t)(c&0xdbff);
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                byte_सूचकांक=0;
                यदि(target<target_सीमा) {
                    *target++=unicode_संहिता_Units[विस्थापनम्];
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                } अन्यथा {
                    /* target overflow */
                    cnv->UChar_Error_बफर[0]=unicode_संहिता_Units[विस्थापनम्];
                    cnv->UChar_Error_बफर_दीर्घता=1;
                    *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;

                    विस्थापनम्=0;
                    भङ्ग;
                }
            } अन्यथा यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                *target++=unicode_संहिता_Units[विस्थापनम्];
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                byte_सूचकांक=0;
            } अन्यथा यदि(c==0xffff) {
                /* callback(illegal) */
                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
            }
        } अन्यथा यदि(action==MBCS_STATE_VALID_प्रत्यक्षम्_20 ||
                  (action==MBCS_STATE_FALLBACK_प्रत्यक्षम्_20 && UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv))
        ) {
            प्रविष्टिः=MBCS_प्रविष्टिः_FINAL_मानम्(प्रविष्टिः);
            /* output surrogate pair */
            *target++=(char16_t)(0xd800|(char16_t)(प्रविष्टिः>>10));
            यदि(offsets!=शून्यसूचकः) {
                *offsets++=source_सूचकांक;
            }
            byte_सूचकांक=0;
            c=(char16_t)(0xdc00|(char16_t)(प्रविष्टिः&0x3ff));
            यदि(target<target_सीमा) {
                *target++=c;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
            } अन्यथा {
                /* target overflow */
                cnv->UChar_Error_बफर[0]=c;
                cnv->UChar_Error_बफर_दीर्घता=1;
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;

                विस्थापनम्=0;
                भङ्ग;
            }
        } अन्यथा यदि(action==MBCS_STATE_CHANGE_ONLY) {
            /*
             * This serves as a state change without any output.
             * It is useful for reading simple stateful encodings,
             * for example using just Shift-In/Shift-Out codes.
             * The 21 unused bits may later be used for more sophisticated
             * state transitions.
             */
            यदि(cnv->shared_दत्त->mbcs.dbcsOnlyState==0) {
                byte_सूचकांक=0;
            } अन्यथा {
                /* SI/SO are illegal for DBCS-only conversion */
                state=(uint8_t)(cnv->बहुलकम्); /* restore the previous state */

                /* callback(illegal) */
                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
            }
        } अन्यथा यदि(action==MBCS_STATE_FALLBACK_प्रत्यक्षम्_16) {
            यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *target++=(char16_t)MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                byte_सूचकांक=0;
            }
        } अन्यथा यदि(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } अन्यथा यदि(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
        } अन्यथा {
            /* reserved, must never occur */
            byte_सूचकांक=0;
        }

        /* end of action codes: prepare for a new character */
        विस्थापनम्=0;

        यदि(byte_सूचकांक==0) {
            source_सूचकांक=अग्र_Source_सूचकांक;
        } अन्यथा यदि(U_FAILURE(*p_Error_संहिता)) {
            /* callback(illegal) */
            यदि(byte_सूचकांक>1) {
                /*
                 * Ticket 5691: consistent illegal sequences:
                 * - We include at least the first byte in the illegal sequence.
                 * - If any of the non-initial bytes could be the start of a character,
                 *   we stop the illegal sequence before the first one of those.
                 */
                UBool isDBCSOnly = cnv->shared_दत्त->mbcs.dbcsOnlyState != 0;
                int8_t i;
                प्रति(i=1;
                    i<byte_सूचकांक && !isSingleOrLead(state_सारणी, state, isDBCSOnly, अष्टकानि[i]);
                    ++i) {}
                यदि(i<byte_सूचकांक) {
                    /* Back out some bytes. */
                    int8_t पृष्ठभागः_बहिर्गमनम्_Distance=byte_सूचकांक-i;
                    int32_t अष्टकानि_From_This_बफर=(int32_t)(source-(ध्रुव uint8_t *)p_तर्काः->source);
                    byte_सूचकांक=i;  /* length of reported illegal byte sequence */
                    यदि(पृष्ठभागः_बहिर्गमनम्_Distance<=अष्टकानि_From_This_बफर) {
                        source-=पृष्ठभागः_बहिर्गमनम्_Distance;
                    } अन्यथा {
                        /* Back out bytes from the previous buffer: Need to replay them. */
                        cnv->pre_गन्तव्यम्_ULength=(int8_t)(अष्टकानि_From_This_बफर-पृष्ठभागः_बहिर्गमनम्_Distance);
                        /* preToULength is negative! */
                        uprv_memcpy(cnv->pre_गन्तव्यम्_U, अष्टकानि+i, -cnv->pre_गन्तव्यम्_ULength);
                        source=(ध्रुव uint8_t *)p_तर्काः->source;
                    }
                }
            }
            भङ्ग;
        } अन्यथा /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_तर्काः->source=(ध्रुव वर्ण *)source;
            byte_सूचकांक=ext_गन्तव्यम्_U(cnv, cnv->shared_दत्त,
                              byte_सूचकांक, &source, source_सीमा,
                              &target, target_सीमा,
                              &offsets, source_सूचकांक,
                              p_तर्काः->flush,
                              p_Error_संहिता);
            source_सूचकांक=अग्र_Source_सूचकांक+=(int32_t)(source-(ध्रुव uint8_t *)p_तर्काः->source);

            यदि(U_FAILURE(*p_Error_संहिता)) {
                /* not mappable or buffer overflow */
                भङ्ग;
            }
        }
    }

    /* set the converter state back into UConverter */
    cnv->गन्तव्यम्_Unicode_स्थितिः=विस्थापनम्;
    cnv->बहुलकम्=state;
    cnv->गन्तव्यम्_ULength=byte_सूचकांक;

    /* write back the updated pointers */
    p_तर्काः->source=(ध्रुव वर्ण *)source;
    p_तर्काः->target=target;
    p_तर्काः->offsets=offsets;
}

/*
 * This version of ucnv_MBCSGetNextUChar() is optimized for single-byte, single-state codepages.
 * We still need a conversion loop in case we find reserved action codes, which are to be ignored.
 */
स्थिर UChar32
ucnv_MBCSSingle_Get_अग्र_UChar(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                        UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव int32_t (*state_सारणी)[256];
    ध्रुव uint8_t *source, *source_सीमा;

    int32_t प्रविष्टिः;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    source = reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->source);
    source_सीमा = reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->source_सीमा);
    यदि((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        state_सारणी=(ध्रुव int32_t (*)[256])cnv->shared_दत्त->mbcs.swap_LFNLState_सारणी;
    } अन्यथा {
        state_सारणी=cnv->shared_दत्त->mbcs.state_सारणी;
    }

    /* conversion loop */
    यावत्(source<source_सीमा) {
        प्रविष्टिः=state_सारणी[0][*source++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* write back the updated pointer early so that we can return directly */
        p_तर्काः->source = reinterpret_cast<ध्रुव वर्ण*>(source);

        यदि(MBCS_प्रविष्टिः_FINAL_IS_VALID_प्रत्यक्षम्_16(प्रविष्टिः)) {
            /* output BMP code point */
            प्रत्यागम static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः));
        यदि( action==MBCS_STATE_VALID_प्रत्यक्षम्_20 ||
            (action==MBCS_STATE_FALLBACK_प्रत्यक्षम्_20 && UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv))
        ) {
            /* output supplementary code point */
            प्रत्यागम static_cast<UChar32>(MBCS_प्रविष्टिः_FINAL_मानम्(प्रविष्टिः) + 0x10000);
        } अन्यथा यदि(action==MBCS_STATE_FALLBACK_प्रत्यक्षम्_16) {
            यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                प्रत्यागम static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
            }
        } अन्यथा यदि(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } अन्यथा यदि(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
        } अन्यथा {
            /* reserved, must never occur */
            अनुवर्त;
        }

        यदि(U_FAILURE(*p_Error_संहिता)) {
            /* callback(illegal) */
            भङ्ग;
        } अन्यथा /* unassigned sequence */ {
            /* defer to the generic implementation */
            p_तर्काः->source = reinterpret_cast<ध्रुव वर्ण*>(source) - 1;
            प्रत्यागम UCNV_GET_अग्र_UCHAR_USE_गन्तव्यम्_U;
        }
    }

    /* no output because of empty input or only state changes */
    *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
    प्रत्यागम 0xffff;
}

/*
 * Version of _MBCSToUnicodeWithOffsets() optimized for single-character
 * conversion without offset handling.
 *
 * When a character does not have a mapping to Unicode, then we return to the
 * generic ucnv_getNextUChar() code for extension/GB 18030 and error/callback
 * handling.
 * We also defer to the generic code in other complicated cases and have them
 * ultimately handled by _MBCSToUnicodeWithOffsets() itself.
 *
 * All normal mappings and errors are handled here.
 */
स्थिर UChar32 U_CALLCONV
ucnv_MBCSGet_अग्र_UChar(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                  UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव uint8_t *source, *source_सीमा, *चरमः_Source;

    ध्रुव int32_t (*state_सारणी)[256];
    ध्रुव uint16_t *unicode_संहिता_Units;

    uint32_t विस्थापनम्;
    uint8_t state;

    int32_t प्रविष्टिः;
    UChar32 c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_तर्काः->converter;

    यदि(cnv->pre_गन्तव्यम्_ULength>0) {
        /* use the generic code in ucnv_getNextUChar() to continue with a partial match */
        प्रत्यागम UCNV_GET_अग्र_UCHAR_USE_गन्तव्यम्_U;
    }

    यदि(cnv->shared_दत्त->mbcs.unicode_आवरणम्&UCNV_HAS_SURROGATES) {
        /*
         * Using the generic ucnv_getNextUChar() code lets us deal correctly
         * with the rare case of a codepage that maps single surrogates
         * without adding the complexity to this already complicated function here.
         */
        प्रत्यागम UCNV_GET_अग्र_UCHAR_USE_गन्तव्यम्_U;
    } अन्यथा यदि(cnv->shared_दत्त->mbcs.गणनम्_अवस्थाः==1) {
        प्रत्यागम ucnv_MBCSSingle_Get_अग्र_UChar(p_तर्काः, p_Error_संहिता);
    }

    /* set up the local pointers */
    source = चरमः_Source = reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->source);
    source_सीमा = reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->source_सीमा);

    यदि((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        state_सारणी=(ध्रुव int32_t (*)[256])cnv->shared_दत्त->mbcs.swap_LFNLState_सारणी;
    } अन्यथा {
        state_सारणी=cnv->shared_दत्त->mbcs.state_सारणी;
    }
    unicode_संहिता_Units=cnv->shared_दत्त->mbcs.unicode_संहिता_Units;

    /* get the converter state from UConverter */
    विस्थापनम्=cnv->गन्तव्यम्_Unicode_स्थितिः;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    यदि ((state = static_cast<uint8_t>(cnv->बहुलकम्)) == 0) {
        state=cnv->shared_दत्त->mbcs.dbcsOnlyState;
    }

    /* conversion loop */
    c=U_SENTINEL;
    यावत्(source<source_सीमा) {
        प्रविष्टिः=state_सारणी[state][*source++];
        यदि(MBCS_प्रविष्टिः_IS_TRANSITION(प्रविष्टिः)) {
            state = static_cast<uint8_t>(MBCS_प्रविष्टिः_TRANSITION_STATE(प्रविष्टिः));
            विस्थापनम्+=MBCS_प्रविष्टिः_TRANSITION_विस्थापनम्(प्रविष्टिः);

            /* optimization for 1/2-byte input and BMP output */
            यदि( source<source_सीमा &&
                MBCS_प्रविष्टिः_IS_FINAL(प्रविष्टिः=state_सारणी[state][*source]) &&
                MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः)==MBCS_STATE_VALID_16 &&
                (c=unicode_संहिता_Units[विस्थापनम्+MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः)])<0xfffe
            ) {
                ++source;
                state = static_cast<uint8_t>(MBCS_प्रविष्टिः_FINAL_STATE(प्रविष्टिः)); /* typically 0 */
                /* output BMP code point */
                भङ्ग;
            }
        } अन्यथा {
            /* save the previous state for proper extension mapping with SI/SO-stateful converters */
            cnv->बहुलकम्=state;

            /* set the next state early so that we can reuse the entry variable */
            state = static_cast<uint8_t>(MBCS_प्रविष्टिः_FINAL_STATE(प्रविष्टिः)); /* typically 0 */

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action = static_cast<uint8_t>(MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः));
            यदि(action==MBCS_STATE_VALID_प्रत्यक्षम्_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
                भङ्ग;
            } अन्यथा यदि(action==MBCS_STATE_VALID_16) {
                विस्थापनम्+=MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
                c=unicode_संहिता_Units[विस्थापनम्];
                यदि(c<0xfffe) {
                    /* output BMP code point */
                    भङ्ग;
                } अन्यथा यदि(c==0xfffe) {
                    यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv) && (c=ucnv_MBCSGetFallback(&cnv->shared_दत्त->mbcs, विस्थापनम्))!=0xfffe) {
                        भङ्ग;
                    }
                } अन्यथा {
                    /* callback(illegal) */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                }
            } अन्यथा यदि(action==MBCS_STATE_VALID_16_PAIR) {
                विस्थापनम्+=MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
                c=unicode_संहिता_Units[विस्थापनम्++];
                यदि(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                    भङ्ग;
                } अन्यथा यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_संहिता_Units[विस्थापनम्]+(0x10000-0xdc00);
                    भङ्ग;
                } अन्यथा यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_संहिता_Units[विस्थापनम्];
                    भङ्ग;
                } अन्यथा यदि(c==0xffff) {
                    /* callback(illegal) */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                }
            } अन्यथा यदि(action==MBCS_STATE_VALID_प्रत्यक्षम्_20 ||
                      (action==MBCS_STATE_FALLBACK_प्रत्यक्षम्_20 && UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv))
            ) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_प्रविष्टिः_FINAL_मानम्(प्रविष्टिः) + 0x10000);
                भङ्ग;
            } अन्यथा यदि(action==MBCS_STATE_CHANGE_ONLY) {
                /*
                 * This serves as a state change without any output.
                 * It is useful for reading simple stateful encodings,
                 * for example using just Shift-In/Shift-Out codes.
                 * The 21 unused bits may later be used for more sophisticated
                 * state transitions.
                 */
                यदि(cnv->shared_दत्त->mbcs.dbcsOnlyState!=0) {
                    /* SI/SO are illegal for DBCS-only conversion */
                    state = static_cast<uint8_t>(cnv->बहुलकम्); /* restore the previous state */

                    /* callback(illegal) */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                }
            } अन्यथा यदि(action==MBCS_STATE_FALLBACK_प्रत्यक्षम्_16) {
                यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv)) {
                    /* output BMP code point */
                    c = static_cast<char16_t>(MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः));
                    भङ्ग;
                }
            } अन्यथा यदि(action==MBCS_STATE_UNASSIGNED) {
                /* just fall through */
            } अन्यथा यदि(action==MBCS_STATE_ILLEGAL) {
                /* callback(illegal) */
                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
            } अन्यथा {
                /* reserved (must never occur), or only state change */
                विस्थापनम्=0;
                चरमः_Source=source;
                अनुवर्त;
            }

            /* end of action codes: prepare for a new character */
            विस्थापनम्=0;

            यदि(U_FAILURE(*p_Error_संहिता)) {
                /* callback(illegal) */
                भङ्ग;
            } अन्यथा /* unassigned sequence */ {
                /* defer to the generic implementation */
                cnv->गन्तव्यम्_Unicode_स्थितिः=0;
                cnv->बहुलकम्=state;
                p_तर्काः->source = reinterpret_cast<ध्रुव वर्ण*>(चरमः_Source);
                प्रत्यागम UCNV_GET_अग्र_UCHAR_USE_गन्तव्यम्_U;
            }
        }
    }

    यदि(c<0) {
        यदि(U_SUCCESS(*p_Error_संहिता) && source==source_सीमा && चरमः_Source<source) {
            /* incomplete character byte sequence */
            cnv->गन्तव्यम्_ULength = static_cast<int8_t>(source - चरमः_Source);
            uprv_memcpy(cnv->गन्तव्यम्_UBytes, चरमः_Source, cnv->गन्तव्यम्_ULength);
            *p_Error_संहिता=U_TRUNCATED_CHAR_प्राप्तम्;
        } अन्यथा यदि(U_FAILURE(*p_Error_संहिता)) {
            /* callback(illegal) */
            /*
             * Ticket 5691: consistent illegal sequences:
             * - We include at least the first byte in the illegal sequence.
             * - If any of the non-initial bytes could be the start of a character,
             *   we stop the illegal sequence before the first one of those.
             */
            UBool isDBCSOnly = static_cast<UBool>(cnv->shared_दत्त->mbcs.dbcsOnlyState != 0);
            uint8_t *अष्टकानि=cnv->गन्तव्यम्_UBytes;
            *अष्टकानि++=*चरमः_Source++;     /* first byte */
            यदि(चरमः_Source==source) {
                cnv->गन्तव्यम्_ULength=1;
            } अन्यथा /* lastSource<source: multi-byte character */ {
                int8_t i;
                प्रति(i=1;
                    चरमः_Source<source && !isSingleOrLead(state_सारणी, state, isDBCSOnly, *चरमः_Source);
                    ++i
                ) {
                    *अष्टकानि++=*चरमः_Source++;
                }
                cnv->गन्तव्यम्_ULength=i;
                source=चरमः_Source;
            }
        } अन्यथा {
            /* no output because of empty input or only state changes */
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        }
        c=0xffff;
    }

    /* set the converter state back into UConverter, ready for a new character */
    cnv->गन्तव्यम्_Unicode_स्थितिः=0;
    cnv->बहुलकम्=state;

    /* write back the updated pointer */
    p_तर्काः->source = reinterpret_cast<ध्रुव वर्ण*>(source);
    प्रत्यागम c;
}

#यदि 0
/*
 * Code disabled 2002dec09 (ICU 2.4) because it is not currently used in ICU. markus
 * Removal improves code coverage.
 */
/**
 * This version of ucnv_MBCSSimpleGetNextUChar() is optimized for single-byte, single-state codepages.
 * It does not handle the EBCDIC swaplfnl option (set in UConverter).
 * It does not handle conversion extensions (_extToU()).
 */
U_CFUNC UChar32
ucnv_MBCSSingle_Simple_Get_अग्र_UChar(UConverter_Shared_दत्त *shared_दत्त,
                              uint8_t b, UBool useFallback) {
    int32_t प्रविष्टिः;
    uint8_t action;

    प्रविष्टिः=shared_दत्त->mbcs.state_सारणी[0][b];
    /* MBCS_ENTRY_IS_FINAL(entry) */

    यदि(MBCS_प्रविष्टिः_FINAL_IS_VALID_प्रत्यक्षम्_16(प्रविष्टिः)) {
        /* output BMP code point */
        प्रत्यागम (char16_t)MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
    }

    /*
     * An if-else-if chain provides more reliable performance for
     * the most common cases compared to a switch.
     */
    action=(uint8_t)(MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः));
    यदि(action==MBCS_STATE_VALID_प्रत्यक्षम्_20) {
        /* output supplementary code point */
        प्रत्यागम 0x10000+MBCS_प्रविष्टिः_FINAL_मानम्(प्रविष्टिः);
    } अन्यथा यदि(action==MBCS_STATE_FALLBACK_प्रत्यक्षम्_16) {
        यदि(!गन्तव्यम्_U_USE_FALLBACK(useFallback)) {
            प्रत्यागम 0xfffe;
        }
        /* output BMP code point */
        प्रत्यागम (char16_t)MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
    } अन्यथा यदि(action==MBCS_STATE_FALLBACK_प्रत्यक्षम्_20) {
        यदि(!गन्तव्यम्_U_USE_FALLBACK(useFallback)) {
            प्रत्यागम 0xfffe;
        }
        /* output supplementary code point */
        प्रत्यागम 0x10000+MBCS_प्रविष्टिः_FINAL_मानम्(प्रविष्टिः);
    } अन्यथा यदि(action==MBCS_STATE_UNASSIGNED) {
        प्रत्यागम 0xfffe;
    } अन्यथा यदि(action==MBCS_STATE_ILLEGAL) {
        प्रत्यागम 0xffff;
    } अन्यथा {
        /* reserved, must never occur */
        प्रत्यागम 0xffff;
    }
}
#यदि_अन्तः

/*
 * This is a simple version of _MBCSGetNextUChar() that is used
 * by other converter implementations.
 * It only returns an "assigned" result if it consumes the entire input.
 * It does not use state from the converter, nor error codes.
 * It does not handle the EBCDIC swaplfnl option (set in UConverter).
 * It handles conversion extensions but not GB 18030.
 *
 * Return value:
 * U+fffe   unassigned
 * U+ffff   illegal
 * otherwise the Unicode code point
 */
U_CFUNC UChar32
ucnv_MBCSSimple_Get_अग्र_UChar(UConverter_Shared_दत्त *shared_दत्त,
                        ध्रुव वर्ण *source, int32_t length,
                        UBool useFallback) {
    ध्रुव int32_t (*state_सारणी)[256];
    ध्रुव uint16_t *unicode_संहिता_Units;

    uint32_t विस्थापनम्;
    uint8_t state, action;

    UChar32 c;
    int32_t i, प्रविष्टिः;

    यदि(length<=0) {
        /* no input at all: "illegal" */
        प्रत्यागम 0xffff;
    }

#यदि 0
/*
 * Code disabled 2002dec09 (ICU 2.4) because it is not currently used in ICU. markus
 * TODO In future releases, verify that this function is never called for SBCS
 * conversions, i.e., that sharedData->mbcs.countStates==1 is still true.
 * Removal improves code coverage.
 */
    /* use optimized function if possible */
    यदि(shared_दत्त->mbcs.गणनम्_अवस्थाः==1) {
        यदि(length==1) {
            प्रत्यागम ucnv_MBCSSingle_Simple_Get_अग्र_UChar(shared_दत्त, (uint8_t)*source, useFallback);
        } अन्यथा {
            प्रत्यागम 0xffff; /* illegal: more than a single byte for an SBCS converter */
        }
    }
#यदि_अन्तः

    /* set up the local pointers */
    state_सारणी=shared_दत्त->mbcs.state_सारणी;
    unicode_संहिता_Units=shared_दत्त->mbcs.unicode_संहिता_Units;

    /* converter state */
    विस्थापनम्=0;
    state=shared_दत्त->mbcs.dbcsOnlyState;

    /* conversion loop */
    प्रति(i=0;;) {
        प्रविष्टिः=state_सारणी[state][(uint8_t)source[i++]];
        यदि(MBCS_प्रविष्टिः_IS_TRANSITION(प्रविष्टिः)) {
            state=(uint8_t)MBCS_प्रविष्टिः_TRANSITION_STATE(प्रविष्टिः);
            विस्थापनम्+=MBCS_प्रविष्टिः_TRANSITION_विस्थापनम्(प्रविष्टिः);

            यदि(i==length) {
                प्रत्यागम 0xffff; /* truncated character */
            }
        } अन्यथा {
            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=(uint8_t)(MBCS_प्रविष्टिः_FINAL_ACTION(प्रविष्टिः));
            यदि(action==MBCS_STATE_VALID_16) {
                विस्थापनम्+=MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
                c=unicode_संहिता_Units[विस्थापनम्];
                यदि(c!=0xfffe) {
                    /*समाप्तम्*/
                } अन्यथा यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv)) {
                    c=ucnv_MBCSGetFallback(&shared_दत्त->mbcs, विस्थापनम्);
                /* else done with 0xfffe */
                }
                भङ्ग;
            } अन्यथा यदि(action==MBCS_STATE_VALID_प्रत्यक्षम्_16) {
                /* output BMP code point */
                c=(char16_t)MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
                भङ्ग;
            } अन्यथा यदि(action==MBCS_STATE_VALID_16_PAIR) {
                विस्थापनम्+=MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
                c=unicode_संहिता_Units[विस्थापनम्++];
                यदि(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } अन्यथा यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=(UChar32)(((c&0x3ff)<<10)+unicode_संहिता_Units[विस्थापनम्]+(0x10000-0xdc00));
                } अन्यथा यदि(UCNV_गन्तव्यम्_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_संहिता_Units[विस्थापनम्];
                } अन्यथा यदि(c==0xffff) {
                    प्रत्यागम 0xffff;
                } अन्यथा {
                    c=0xfffe;
                }
                भङ्ग;
            } अन्यथा यदि(action==MBCS_STATE_VALID_प्रत्यक्षम्_20) {
                /* output supplementary code point */
                c=0x10000+MBCS_प्रविष्टिः_FINAL_मानम्(प्रविष्टिः);
                भङ्ग;
            } अन्यथा यदि(action==MBCS_STATE_FALLBACK_प्रत्यक्षम्_16) {
                यदि(!गन्तव्यम्_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    भङ्ग;
                }
                /* output BMP code point */
                c=(char16_t)MBCS_प्रविष्टिः_FINAL_मानम्_16(प्रविष्टिः);
                भङ्ग;
            } अन्यथा यदि(action==MBCS_STATE_FALLBACK_प्रत्यक्षम्_20) {
                यदि(!गन्तव्यम्_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    भङ्ग;
                }
                /* output supplementary code point */
                c=0x10000+MBCS_प्रविष्टिः_FINAL_मानम्(प्रविष्टिः);
                भङ्ग;
            } अन्यथा यदि(action==MBCS_STATE_UNASSIGNED) {
                c=0xfffe;
                भङ्ग;
            }

            /*
             * forbid MBCS_STATE_CHANGE_ONLY for this function,
             * and MBCS_STATE_ILLEGAL and reserved action codes
             */
            प्रत्यागम 0xffff;
        }
    }

    यदि(i!=length) {
        /* illegal for this function: not all input consumed */
        प्रत्यागम 0xffff;
    }

    यदि(c==0xfffe) {
        /* try an extension mapping */
        ध्रुव int32_t *cx=shared_दत्त->mbcs.extIndexes;
        यदि(cx!=शून्यसूचकः) {
            प्रत्यागम ucnv_ext_Simple_मेलनम्_गन्तव्यम्_U(cx, source, length, useFallback);
        }
    }

    प्रत्यागम c;
}

/* MBCS-from-Unicode conversion functions ----------------------------------- */

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for double-byte codepages. */
स्थिर शून्य
ucnv_MBCSDoubleFromUnicodeWithOffsets(UConverter_From_Unicode_तर्काः *p_तर्काः,
                                  UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव char16_t *source, *source_सीमा;
    uint8_t *target;
    int32_t target_क्षमता;
    int32_t *offsets;

    ध्रुव uint16_t *सारणी;
    ध्रुव uint16_t *mbcs_सूचकांक;
    ध्रुव uint8_t *अष्टकानि;

    UChar32 c;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    uint32_t stage2_प्रविष्टिः;
    uint32_t asciiRoundtrips;
    uint32_t value;
    uint8_t unicode_आवरणम्;

    /* use optimized function if possible */
    cnv=p_तर्काः->converter;
    unicode_आवरणम्=cnv->shared_दत्त->mbcs.unicode_आवरणम्;

    /* set up the local pointers */
    source=p_तर्काः->source;
    source_सीमा=p_तर्काः->source_सीमा;
    target = reinterpret_cast<uint8_t*>(p_तर्काः->target);
    target_क्षमता = static_cast<int32_t>(p_तर्काः->target_सीमा - p_तर्काः->target);
    offsets=p_तर्काः->offsets;

    सारणी=cnv->shared_दत्त->mbcs.from_Unicode_सारणी;
    mbcs_सूचकांक=cnv->shared_दत्त->mbcs.mbcs_सूचकांक;
    यदि((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        अष्टकानि=cnv->shared_दत्त->mbcs.swap_LFNLFrom_Unicode_अष्टकानि;
    } अन्यथा {
        अष्टकानि=cnv->shared_दत्त->mbcs.from_Unicode_अष्टकानि;
    }
    asciiRoundtrips=cnv->shared_दत्त->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक= c==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    /* conversion loop */
    यदि(c!=0 && target_क्षमता>0) {
        गच्छ getTrail;
    }

    यावत्(source<source_सीमा) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        यदि(target_क्षमता>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*source++;
            ++अग्र_Source_सूचकांक;
            यदि(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *target++ = static_cast<uint8_t>(c);
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                    source_सूचकांक=अग्र_Source_सूचकांक;
                }
                --target_क्षमता;
                c=0;
                अनुवर्त;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            यदि(c<=0xd7ff) {
                value=DBCS_परिणाम_FROM_MOST_BMP(mbcs_सूचकांक, (ध्रुव uint16_t *)अष्टकानि, c);
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                यदि(value==0) {
                    गच्छ unassigned;
                }
                /* output the value */
            } अन्यथा {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                यदि(U16_IS_SURROGATE(c) && !(unicode_आवरणम्&UCNV_HAS_SURROGATES)) {
                    यदि(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        यदि(source<source_सीमा) {
                            /* test the following code unit */
                            char16_t trail=*source;
                            यदि(U16_IS_TRAIL(trail)) {
                                ++source;
                                ++अग्र_Source_सूचकांक;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                यदि(!(unicode_आवरणम्&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    /* callback(unassigned) */
                                    गच्छ unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } अन्यथा {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                                भङ्ग;
                            }
                        } अन्यथा {
                            /* no more input */
                            भङ्ग;
                        }
                    } अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                        भङ्ग;
                    }
                }

                /* convert the Unicode code point in c into codepage bytes */
                stage2_प्रविष्टिः=MBCS_STAGE_2_FROM_U(सारणी, c);

                /* get the bytes and the length for the output */
                /* MBCS_OUTPUT_2 */
                value=MBCS_मानम्_2_FROM_STAGE_2(अष्टकानि, stage2_प्रविष्टिः, c);

                /* is this code point assigned, or do we use fallbacks? */
                यदि(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_प्रविष्टिः, c) ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_तर्काः->source=source;
                    c=_extFromU(cnv, cnv->shared_दत्त,
                                c, &source, source_सीमा,
                                &target, target+target_क्षमता,
                                &offsets, source_सूचकांक,
                                p_तर्काः->flush,
                                p_Error_संहिता);
                    अग्र_Source_सूचकांक += static_cast<int32_t>(source - p_तर्काः->source);

                    यदि(U_FAILURE(*p_Error_संहिता)) {
                        /* not mappable or buffer overflow */
                        भङ्ग;
                    } अन्यथा {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        target_क्षमता = static_cast<int32_t>(p_तर्काः->target_सीमा - reinterpret_cast<वर्ण*>(target));

                        /* normal end of conversion: prepare for a new character */
                        source_सूचकांक=अग्र_Source_सूचकांक;
                        अनुवर्त;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            यदि(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *target++ = static_cast<uint8_t>(value);
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                --target_क्षमता;
            } अन्यथा /* length==2 */ {
                *target++ = static_cast<uint8_t>(value >> 8);
                यदि(2<=target_क्षमता) {
                    *target++ = static_cast<uint8_t>(value);
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                        *offsets++=source_सूचकांक;
                    }
                    target_क्षमता-=2;
                } अन्यथा {
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                    cnv->char_Error_बफर[0] = static_cast<वर्ण>(value);
                    cnv->char_Error_बफर_दीर्घता=1;

                    /* target overflow */
                    target_क्षमता=0;
                    *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                    c=0;
                    भङ्ग;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            source_सूचकांक=अग्र_Source_सूचकांक;
            अनुवर्त;
        } अन्यथा {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_तर्काः->source=source;
    p_तर्काः->target = reinterpret_cast<वर्ण*>(target);
    p_तर्काः->offsets=offsets;
}

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for single-byte codepages. */
स्थिर शून्य
ucnv_MBCSSingleFromUnicodeWithOffsets(UConverter_From_Unicode_तर्काः *p_तर्काः,
                                  UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव char16_t *source, *source_सीमा;
    uint8_t *target;
    int32_t target_क्षमता;
    int32_t *offsets;

    ध्रुव uint16_t *सारणी;
    ध्रुव uint16_t *results;

    UChar32 c;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    uint16_t value, min_मानम्;
    UBool hasSupplementary;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    source=p_तर्काः->source;
    source_सीमा=p_तर्काः->source_सीमा;
    target = reinterpret_cast<uint8_t*>(p_तर्काः->target);
    target_क्षमता = static_cast<int32_t>(p_तर्काः->target_सीमा - p_तर्काः->target);
    offsets=p_तर्काः->offsets;

    सारणी=cnv->shared_दत्त->mbcs.from_Unicode_सारणी;
    यदि((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_दत्त->mbcs.swap_LFNLFrom_Unicode_अष्टकानि);
    } अन्यथा {
        results=(uint16_t *)cnv->shared_दत्त->mbcs.from_Unicode_अष्टकानि;
    }

    यदि(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_मानम्=0x800;
    } अन्यथा {
        /* use only roundtrips and fallbacks from private-use characters */
        min_मानम्=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_दत्त->mbcs.unicode_आवरणम् & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक= c==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    /* conversion loop */
    यदि(c!=0 && target_क्षमता>0) {
        गच्छ getTrail;
    }

    यावत्(source<source_सीमा) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        यदि(target_क्षमता>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*source++;
            ++अग्र_Source_सूचकांक;
            यदि(U16_IS_SURROGATE(c)) {
                यदि(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                    यदि(source<source_सीमा) {
                        /* test the following code unit */
                        char16_t trail=*source;
                        यदि(U16_IS_TRAIL(trail)) {
                            ++source;
                            ++अग्र_Source_सूचकांक;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            यदि(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                /* callback(unassigned) */
                                गच्छ unassigned;
                            }
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } अन्यथा {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                            भङ्ग;
                        }
                    } अन्यथा {
                        /* no more input */
                        भङ्ग;
                    }
                } अन्यथा {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    भङ्ग;
                }
            }

            /* convert the Unicode code point in c into codepage bytes */
            value=MBCS_SINGLE_परिणाम_FROM_U(सारणी, results, c);

            /* is this code point assigned, or do we use fallbacks? */
            यदि(value>=min_मानम्) {
                /* assigned, write the output character bytes from value and length */
                /* length==1 */
                /* this is easy because we know that there is enough space */
                *target++ = static_cast<uint8_t>(value);
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                --target_क्षमता;

                /* normal end of conversion: prepare for a new character */
                c=0;
                source_सूचकांक=अग्र_Source_सूचकांक;
            } अन्यथा { /* unassigned */
unassigned:
                /* try an extension mapping */
                p_तर्काः->source=source;
                c=_extFromU(cnv, cnv->shared_दत्त,
                            c, &source, source_सीमा,
                            &target, target+target_क्षमता,
                            &offsets, source_सूचकांक,
                            p_तर्काः->flush,
                            p_Error_संहिता);
                अग्र_Source_सूचकांक += static_cast<int32_t>(source - p_तर्काः->source);

                यदि(U_FAILURE(*p_Error_संहिता)) {
                    /* not mappable or buffer overflow */
                    भङ्ग;
                } अन्यथा {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    target_क्षमता = static_cast<int32_t>(p_तर्काः->target_सीमा - reinterpret_cast<वर्ण*>(target));

                    /* normal end of conversion: prepare for a new character */
                    source_सूचकांक=अग्र_Source_सूचकांक;
                }
            }
        } अन्यथा {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_तर्काः->source=source;
    p_तर्काः->target = reinterpret_cast<वर्ण*>(target);
    p_तर्काः->offsets=offsets;
}

/*
 * This version of ucnv_MBCSFromUnicode() is optimized for single-byte codepages
 * that map only to and from the BMP.
 * In addition to single-byte/state optimizations, the offset calculations
 * become much easier.
 * It would be possible to use the sbcsIndex for UTF-8-friendly tables,
 * but measurements have shown that this diminishes performance
 * in more cases than it improves it.
 * See SVN revision 21013 (2007-feb-06) for the last version with #if switches
 * for various MBCS and SBCS optimizations.
 */
स्थिर शून्य
ucnv_MBCSSingleFromBMPWithOffsets(UConverter_From_Unicode_तर्काः *p_तर्काः,
                              UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव char16_t *source, *source_सीमा, *चरमः_Source;
    uint8_t *target;
    int32_t target_क्षमता, length;
    int32_t *offsets;

    ध्रुव uint16_t *सारणी;
    ध्रुव uint16_t *results;

    UChar32 c;

    int32_t source_सूचकांक;

    uint32_t asciiRoundtrips;
    uint16_t value, min_मानम्;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    source=p_तर्काः->source;
    source_सीमा=p_तर्काः->source_सीमा;
    target = reinterpret_cast<uint8_t*>(p_तर्काः->target);
    target_क्षमता = static_cast<int32_t>(p_तर्काः->target_सीमा - p_तर्काः->target);
    offsets=p_तर्काः->offsets;

    सारणी=cnv->shared_दत्त->mbcs.from_Unicode_सारणी;
    यदि((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_दत्त->mbcs.swap_LFNLFrom_Unicode_अष्टकानि);
    } अन्यथा {
        results=(uint16_t *)cnv->shared_दत्त->mbcs.from_Unicode_अष्टकानि;
    }
    asciiRoundtrips=cnv->shared_दत्त->mbcs.asciiRoundtrips;

    यदि(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_मानम्=0x800;
    } अन्यथा {
        /* use only roundtrips and fallbacks from private-use characters */
        min_मानम्=0xc00;
    }

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक= c==0 ? 0 : -1;
    चरमः_Source=source;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(source_सीमा - source);
    यदि(length<target_क्षमता) {
        target_क्षमता=length;
    }

    /* conversion loop */
    यदि(c!=0 && target_क्षमता>0) {
        गच्छ getTrail;
    }

#यदि MBCS_UNROLL_SINGLE_FROM_BMP
    /* unrolling makes it slower on Pentium III/Windows 2000?! */
    /* unroll the loop with the most common case */
unrolled:
    यदि(target_क्षमता>=4) {
        int32_t count, loops;
        uint16_t anded_मानानि;

        loops=count=target_क्षमता>>2;
        कुरु {
            c=*source++;
            anded_मानानि=value=MBCS_SINGLE_परिणाम_FROM_U(सारणी, results, c);
            *target++=(uint8_t)value;
            c=*source++;
            anded_मानानि&=value=MBCS_SINGLE_परिणाम_FROM_U(सारणी, results, c);
            *target++=(uint8_t)value;
            c=*source++;
            anded_मानानि&=value=MBCS_SINGLE_परिणाम_FROM_U(सारणी, results, c);
            *target++=(uint8_t)value;
            c=*source++;
            anded_मानानि&=value=MBCS_SINGLE_परिणाम_FROM_U(सारणी, results, c);
            *target++=(uint8_t)value;

            /* were all 4 entries really valid? */
            यदि(anded_मानानि<min_मानम्) {
                /* no, return to the first of these 4 */
                source-=4;
                target-=4;
                भङ्ग;
            }
        } यावत्(--count>0);
        count=loops-count;
        target_क्षमता-=4*count;

        यदि(offsets!=शून्यसूचकः) {
            चरमः_Source+=4*count;
            यावत्(count>0) {
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                --count;
            }
        }

        c=0;
    }
#यदि_अन्तः

    यावत्(target_क्षमता>0) {
        /*
         * Get a correct Unicode code point:
         * a single char16_t for a BMP code point or
         * a matched surrogate pair for a "supplementary code point".
         */
        c=*source++;
        /*
         * Do not immediately check for single surrogates:
         * Assume that they are unassigned and check for them in that case.
         * This speeds up the conversion of assigned characters.
         */
        /* convert the Unicode code point in c into codepage bytes */
        यदि(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
            *target++ = static_cast<uint8_t>(c);
            --target_क्षमता;
            c=0;
            अनुवर्त;
        }
        value=MBCS_SINGLE_परिणाम_FROM_U(सारणी, results, c);
        /* is this code point assigned, or do we use fallbacks? */
        यदि(value>=min_मानम्) {
            /* assigned, write the output character bytes from value and length */
            /* length==1 */
            /* this is easy because we know that there is enough space */
            *target++ = static_cast<uint8_t>(value);
            --target_क्षमता;

            /* normal end of conversion: prepare for a new character */
            c=0;
            अनुवर्त;
        } अन्यथा यदि(!U16_IS_SURROGATE(c)) {
            /* normal, unassigned BMP character */
        } अन्यथा यदि(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
            यदि(source<source_सीमा) {
                /* test the following code unit */
                char16_t trail=*source;
                यदि(U16_IS_TRAIL(trail)) {
                    ++source;
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } अन्यथा {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    भङ्ग;
                }
            } अन्यथा {
                /* no more input */
                यदि (p_तर्काः->flush) {
                    *p_Error_संहिता=U_TRUNCATED_CHAR_प्राप्तम्;
                }
                भङ्ग;
            }
        } अन्यथा {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
            भङ्ग;
        }

        /* c does not have a mapping */

        /* get the number of code units for c to correctly advance sourceIndex */
        length=U16_दीर्घता(c);

        /* set offsets since the start or the last extension */
        यदि(offsets!=शून्यसूचकः) {
            int32_t count = static_cast<int32_t>(source - चरमः_Source);

            /* do not set the offset for this character */
            count-=length;

            यावत्(count>0) {
                *offsets++=source_सूचकांक++;
                --count;
            }
            /* offsets and sourceIndex are now set for the current character */
        }

        /* try an extension mapping */
        चरमः_Source=source;
        c=_extFromU(cnv, cnv->shared_दत्त,
                    c, &source, source_सीमा,
                    &target, reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->target_सीमा),
                    &offsets, source_सूचकांक,
                    p_तर्काः->flush,
                    p_Error_संहिता);
        source_सूचकांक += length + static_cast<int32_t>(source - चरमः_Source);
        चरमः_Source=source;

        यदि(U_FAILURE(*p_Error_संहिता)) {
            /* not mappable or buffer overflow */
            भङ्ग;
        } अन्यथा {
            /* a mapping was written to the target, continue */

            /* recalculate the targetCapacity after an extension mapping */
            target_क्षमता = static_cast<int32_t>(p_तर्काः->target_सीमा - reinterpret_cast<वर्ण*>(target));
            length = static_cast<int32_t>(source_सीमा - source);
            यदि(length<target_क्षमता) {
                target_क्षमता=length;
            }
        }

#यदि MBCS_UNROLL_SINGLE_FROM_BMP
        /* unrolling makes it slower on Pentium III/Windows 2000?! */
        गच्छ unrolled;
#यदि_अन्तः
    }

    यदि(U_SUCCESS(*p_Error_संहिता) && source<source_सीमा && target>=(uint8_t *)p_तर्काः->target_सीमा) {
        /* target is full */
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
    }

    /* set offsets since the start or the last callback */
    यदि(offsets!=शून्यसूचकः) {
        size_t count=source-चरमः_Source;
        यदि (count > 0 && *p_Error_संहिता == U_TRUNCATED_CHAR_प्राप्तम्) {
            /*
            Caller gave us a partial supplementary character,
            which this function couldn't convert in any case.
            The callback will handle the offset.
            */
            count--;
        }
        यावत्(count>0) {
            *offsets++=source_सूचकांक++;
            --count;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_तर्काः->source=source;
    p_तर्काः->target = reinterpret_cast<वर्ण*>(target);
    p_तर्काः->offsets=offsets;
}

U_CFUNC शून्य
ucnv_MBCSFromUnicodeWithOffsets(UConverter_From_Unicode_तर्काः *p_तर्काः,
                            UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव char16_t *source, *source_सीमा;
    uint8_t *target;
    int32_t target_क्षमता;
    int32_t *offsets;

    ध्रुव uint16_t *सारणी;
    ध्रुव uint16_t *mbcs_सूचकांक;
    ध्रुव uint8_t *p, *अष्टकानि;
    uint8_t outputType;

    UChar32 c;

    int32_t पूर्व_Source_सूचकांक, source_सूचकांक, अग्र_Source_सूचकांक;

    uint32_t stage2_प्रविष्टिः;
    uint32_t asciiRoundtrips;
    uint32_t value;
    /* Shift-In and Shift-Out byte sequences differ by encoding scheme. */
    uint8_t si_अष्टकानि[2] = {0, 0};
    uint8_t so_अष्टकानि[2] = {0, 0};
    uint8_t si_दीर्घता, so_दीर्घता;
    int32_t length = 0, पूर्व_दीर्घता;
    uint8_t unicode_आवरणम्;

    cnv=p_तर्काः->converter;

    यदि(cnv->preFromUFirstCP>=0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_मेलनम्_From_U(cnv, p_तर्काः, -1, p_Error_संहिता);

        यदि(U_FAILURE(*p_Error_संहिता) || cnv->preFromULength<0) {
            प्रत्यागम;
        }
    }

    /* use optimized function if possible */
    outputType=cnv->shared_दत्त->mbcs.outputType;
    unicode_आवरणम्=cnv->shared_दत्त->mbcs.unicode_आवरणम्;
    यदि(outputType==MBCS_OUTPUT_1 && !(unicode_आवरणम्&UCNV_HAS_SURROGATES)) {
        यदि(!(unicode_आवरणम्&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingleFromBMPWithOffsets(p_तर्काः, p_Error_संहिता);
        } अन्यथा {
            ucnv_MBCSSingleFromUnicodeWithOffsets(p_तर्काः, p_Error_संहिता);
        }
        प्रत्यागम;
    } अन्यथा यदि(outputType==MBCS_OUTPUT_2 && cnv->shared_दत्त->mbcs.utf8Friendly) {
        ucnv_MBCSDoubleFromUnicodeWithOffsets(p_तर्काः, p_Error_संहिता);
        प्रत्यागम;
    }

    /* set up the local pointers */
    source=p_तर्काः->source;
    source_सीमा=p_तर्काः->source_सीमा;
    target=(uint8_t *)p_तर्काः->target;
    target_क्षमता=(int32_t)(p_तर्काः->target_सीमा-p_तर्काः->target);
    offsets=p_तर्काः->offsets;

    सारणी=cnv->shared_दत्त->mbcs.from_Unicode_सारणी;
    यदि(cnv->shared_दत्त->mbcs.utf8Friendly) {
        mbcs_सूचकांक=cnv->shared_दत्त->mbcs.mbcs_सूचकांक;
    } अन्यथा {
        mbcs_सूचकांक=शून्यसूचकः;
    }
    यदि((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        अष्टकानि=cnv->shared_दत्त->mbcs.swap_LFNLFrom_Unicode_अष्टकानि;
    } अन्यथा {
        अष्टकानि=cnv->shared_दत्त->mbcs.from_Unicode_अष्टकानि;
    }
    asciiRoundtrips=cnv->shared_दत्त->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    यदि(outputType==MBCS_OUTPUT_2_SISO) {
        पूर्व_दीर्घता=cnv->from_Unicode_स्थितिः;
        यदि(पूर्व_दीर्घता==0) {
            /* set the real value */
            पूर्व_दीर्घता=1;
        }
    } अन्यथा {
        /* prevent fromUnicodeStatus from being set to something non-0 */
        पूर्व_दीर्घता=0;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    पूर्व_Source_सूचकांक=-1;
    source_सूचकांक= c==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    /* Get the SI/SO character for the converter */
    si_दीर्घता = static_cast<uint8_t>(getSISOBytes(SI, cnv->options, si_अष्टकानि));
    so_दीर्घता = static_cast<uint8_t>(getSISOBytes(SO, cnv->options, so_अष्टकानि));

    /* conversion loop */
    /*
     * This is another piece of ugly code:
     * A goto into the loop if the converter state contains a first surrogate
     * from the previous function call.
     * It saves me to check in each loop iteration a check of if(c==0)
     * and duplicating the trail-surrogate-handling code in the else
     * branch of that check.
     * I could not find any other way to get around this other than
     * using a function call for the conversion and callback, which would
     * be even more inefficient.
     *
     * Markus Scherer 2000-jul-19
     */
    यदि(c!=0 && target_क्षमता>0) {
        गच्छ getTrail;
    }

    यावत्(source<source_सीमा) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        यदि(target_क्षमता>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*source++;
            ++अग्र_Source_सूचकांक;
            यदि(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *target++=(uint8_t)c;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                    पूर्व_Source_सूचकांक=source_सूचकांक;
                    source_सूचकांक=अग्र_Source_सूचकांक;
                }
                --target_क्षमता;
                c=0;
                अनुवर्त;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            यदि(c<=0xd7ff && mbcs_सूचकांक!=शून्यसूचकः) {
                value=mbcs_सूचकांक[c>>6];

                /* get the bytes and the length for the output (copied from below and adapted for utf8Friendly data) */
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                विकल्प(outputType) {
                पक्ष MBCS_OUTPUT_2:
                    value=((ध्रुव uint16_t *)अष्टकानि)[value +(c&0x3f)];
                    यदि(value<=0xff) {
                        यदि(value==0) {
                            गच्छ unassigned;
                        } अन्यथा {
                            length=1;
                        }
                    } अन्यथा {
                        length=2;
                    }
                    भङ्ग;
                पक्ष MBCS_OUTPUT_2_SISO:
                    /* 1/2-byte stateful with Shift-In/Shift-Out */
                    /*
                     * Save the old state in the converter object
                     * right here, then change the local prevLength state variable if necessary.
                     * Then, if this character turns out to be unassigned or a fallback that
                     * is not taken, the callback code must not save the new state in the converter
                     * because the new state is for a character that is not output.
                     * However, the callback must still restore the state from the converter
                     * in case the callback function changed it for its output.
                     */
                    cnv->from_Unicode_स्थितिः=पूर्व_दीर्घता; /* save the old state */
                    value=((ध्रुव uint16_t *)अष्टकानि)[value +(c&0x3f)];
                    यदि(value<=0xff) {
                        यदि(value==0) {
                            गच्छ unassigned;
                        } अन्यथा यदि(पूर्व_दीर्घता<=1) {
                            length=1;
                        } अन्यथा {
                            /* change from double-byte mode to single-byte */
                            यदि (si_दीर्घता == 1) {
                                value|=(uint32_t)si_अष्टकानि[0]<<8;
                                length = 2;
                            } अन्यथा यदि (si_दीर्घता == 2) {
                                value|=(uint32_t)si_अष्टकानि[1]<<8;
                                value|=(uint32_t)si_अष्टकानि[0]<<16;
                                length = 3;
                            }
                            पूर्व_दीर्घता=1;
                        }
                    } अन्यथा {
                        यदि(पूर्व_दीर्घता==2) {
                            length=2;
                        } अन्यथा {
                            /* change from single-byte mode to double-byte */
                            यदि (so_दीर्घता == 1) {
                                value|=(uint32_t)so_अष्टकानि[0]<<16;
                                length = 3;
                            } अन्यथा यदि (so_दीर्घता == 2) {
                                value|=(uint32_t)so_अष्टकानि[1]<<16;
                                value|=(uint32_t)so_अष्टकानि[0]<<24;
                                length = 4;
                            }
                            पूर्व_दीर्घता=2;
                        }
                    }
                    भङ्ग;
                पक्ष MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=((ध्रुव uint16_t *)अष्टकानि)[value +(c&0x3f)];
                    यदि(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        गच्छ unassigned;
                    } अन्यथा {
                        length=2;
                    }
                    भङ्ग;
                पक्ष MBCS_OUTPUT_3:
                    p=अष्टकानि+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    यदि(value<=0xff) {
                        यदि(value==0) {
                            गच्छ unassigned;
                        } अन्यथा {
                            length=1;
                        }
                    } अन्यथा यदि(value<=0xffff) {
                        length=2;
                    } अन्यथा {
                        length=3;
                    }
                    भङ्ग;
                पक्ष MBCS_OUTPUT_4:
                    value=((ध्रुव uint32_t *)अष्टकानि)[value +(c&0x3f)];
                    यदि(value<=0xff) {
                        यदि(value==0) {
                            गच्छ unassigned;
                        } अन्यथा {
                            length=1;
                        }
                    } अन्यथा यदि(value<=0xffff) {
                        length=2;
                    } अन्यथा यदि(value<=0xffffff) {
                        length=3;
                    } अन्यथा {
                        length=4;
                    }
                    भङ्ग;
                पक्ष MBCS_OUTPUT_3_EUC:
                    value=((ध्रुव uint16_t *)अष्टकानि)[value +(c&0x3f)];
                    /* EUC 16-bit fixed-length representation */
                    यदि(value<=0xff) {
                        यदि(value==0) {
                            गच्छ unassigned;
                        } अन्यथा {
                            length=1;
                        }
                    } अन्यथा यदि((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } अन्यथा यदि((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } अन्यथा {
                        length=2;
                    }
                    भङ्ग;
                पक्ष MBCS_OUTPUT_4_EUC:
                    p=अष्टकानि+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    यदि(value<=0xff) {
                        यदि(value==0) {
                            गच्छ unassigned;
                        } अन्यथा {
                            length=1;
                        }
                    } अन्यथा यदि(value<=0xffff) {
                        length=2;
                    } अन्यथा यदि((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } अन्यथा यदि((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } अन्यथा {
                        length=3;
                    }
                    भङ्ग;
                स्वभाव:
                    /* must not occur */
                    /*
                     * To avoid compiler warnings that value & length may be
                     * used without having been initialized, we set them here.
                     * In reality, this is unreachable code.
                     * Not having a default branch also causes warnings with
                     * some compilers.
                     */
                    value=0;
                    length=0;
                    भङ्ग;
                }
                /* output the value */
            } अन्यथा {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                यदि(U16_IS_SURROGATE(c) && !(unicode_आवरणम्&UCNV_HAS_SURROGATES)) {
                    यदि(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        यदि(source<source_सीमा) {
                            /* test the following code unit */
                            char16_t trail=*source;
                            यदि(U16_IS_TRAIL(trail)) {
                                ++source;
                                ++अग्र_Source_सूचकांक;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                यदि(!(unicode_आवरणम्&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    cnv->from_Unicode_स्थितिः=पूर्व_दीर्घता; /* save the old state */
                                    /* callback(unassigned) */
                                    गच्छ unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } अन्यथा {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                                भङ्ग;
                            }
                        } अन्यथा {
                            /* no more input */
                            भङ्ग;
                        }
                    } अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                        भङ्ग;
                    }
                }

                /* convert the Unicode code point in c into codepage bytes */

                /*
                 * The basic lookup is a triple-stage compact array (trie) lookup.
                 * For details see the beginning of this file.
                 *
                 * Single-byte codepages are handled with a different data structure
                 * by _MBCSSingle... functions.
                 *
                 * The result consists of a 32-bit value from stage 2 and
                 * a pointer to as many bytes as are stored per character.
                 * The pointer points to the character's bytes in stage 3.
                 * Bits 15..0 of the stage 2 entry contain the stage 3 index
                 * for that pointer, while bits 31..16 are flags for which of
                 * the 16 characters in the block are roundtrip-assigned.
                 *
                 * For 2-byte and 4-byte codepages, the bytes are stored as uint16_t
                 * respectively as uint32_t, in the platform encoding.
                 * For 3-byte codepages, the bytes are always stored in big-endian order.
                 *
                 * For EUC encodings that use only either 0x8e or 0x8f as the first
                 * byte of their longest byte sequences, the first two bytes in
                 * this third stage indicate with their 7th bits whether these bytes
                 * are to be written directly or actually need to be preceded by
                 * one of the two Single-Shift codes. With this, the third stage
                 * stores one byte fewer per character than the actual maximum length of
                 * EUC byte sequences.
                 *
                 * Other than that, leading zero bytes are removed and the other
                 * bytes output. A single zero byte may be output if the "assigned"
                 * bit in stage 2 was on.
                 * The data structure does not support zero byte output as a fallback,
                 * and also does not allow output of leading zeros.
                 */
                stage2_प्रविष्टिः=MBCS_STAGE_2_FROM_U(सारणी, c);

                /* get the bytes and the length for the output */
                विकल्प(outputType) {
                पक्ष MBCS_OUTPUT_2:
                    value=MBCS_मानम्_2_FROM_STAGE_2(अष्टकानि, stage2_प्रविष्टिः, c);
                    यदि(value<=0xff) {
                        length=1;
                    } अन्यथा {
                        length=2;
                    }
                    भङ्ग;
                पक्ष MBCS_OUTPUT_2_SISO:
                    /* 1/2-byte stateful with Shift-In/Shift-Out */
                    /*
                     * Save the old state in the converter object
                     * right here, then change the local prevLength state variable if necessary.
                     * Then, if this character turns out to be unassigned or a fallback that
                     * is not taken, the callback code must not save the new state in the converter
                     * because the new state is for a character that is not output.
                     * However, the callback must still restore the state from the converter
                     * in case the callback function changed it for its output.
                     */
                    cnv->from_Unicode_स्थितिः=पूर्व_दीर्घता; /* save the old state */
                    value=MBCS_मानम्_2_FROM_STAGE_2(अष्टकानि, stage2_प्रविष्टिः, c);
                    यदि(value<=0xff) {
                        यदि(value==0 && MBCS_FROM_U_IS_ROUNDTRIP(stage2_प्रविष्टिः, c)==0) {
                            /* no mapping, leave value==0 */
                            length=0;
                        } अन्यथा यदि(पूर्व_दीर्घता<=1) {
                            length=1;
                        } अन्यथा {
                            /* change from double-byte mode to single-byte */
                            यदि (si_दीर्घता == 1) {
                                value|=(uint32_t)si_अष्टकानि[0]<<8;
                                length = 2;
                            } अन्यथा यदि (si_दीर्घता == 2) {
                                value|=(uint32_t)si_अष्टकानि[1]<<8;
                                value|=(uint32_t)si_अष्टकानि[0]<<16;
                                length = 3;
                            }
                            पूर्व_दीर्घता=1;
                        }
                    } अन्यथा {
                        यदि(पूर्व_दीर्घता==2) {
                            length=2;
                        } अन्यथा {
                            /* change from single-byte mode to double-byte */
                            यदि (so_दीर्घता == 1) {
                                value|=(uint32_t)so_अष्टकानि[0]<<16;
                                length = 3;
                            } अन्यथा यदि (so_दीर्घता == 2) {
                                value|=(uint32_t)so_अष्टकानि[1]<<16;
                                value|=(uint32_t)so_अष्टकानि[0]<<24;
                                length = 4;
                            }
                            पूर्व_दीर्घता=2;
                        }
                    }
                    भङ्ग;
                पक्ष MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=MBCS_मानम्_2_FROM_STAGE_2(अष्टकानि, stage2_प्रविष्टिः, c);
                    यदि(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        value=stage2_प्रविष्टिः=0; /* stage2Entry=0 to reset roundtrip flags */
                        length=0;
                    } अन्यथा {
                        length=2;
                    }
                    भङ्ग;
                पक्ष MBCS_OUTPUT_3:
                    p=MBCS_POINTER_3_FROM_STAGE_2(अष्टकानि, stage2_प्रविष्टिः, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    यदि(value<=0xff) {
                        length=1;
                    } अन्यथा यदि(value<=0xffff) {
                        length=2;
                    } अन्यथा {
                        length=3;
                    }
                    भङ्ग;
                पक्ष MBCS_OUTPUT_4:
                    value=MBCS_मानम्_4_FROM_STAGE_2(अष्टकानि, stage2_प्रविष्टिः, c);
                    यदि(value<=0xff) {
                        length=1;
                    } अन्यथा यदि(value<=0xffff) {
                        length=2;
                    } अन्यथा यदि(value<=0xffffff) {
                        length=3;
                    } अन्यथा {
                        length=4;
                    }
                    भङ्ग;
                पक्ष MBCS_OUTPUT_3_EUC:
                    value=MBCS_मानम्_2_FROM_STAGE_2(अष्टकानि, stage2_प्रविष्टिः, c);
                    /* EUC 16-bit fixed-length representation */
                    यदि(value<=0xff) {
                        length=1;
                    } अन्यथा यदि((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } अन्यथा यदि((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } अन्यथा {
                        length=2;
                    }
                    भङ्ग;
                पक्ष MBCS_OUTPUT_4_EUC:
                    p=MBCS_POINTER_3_FROM_STAGE_2(अष्टकानि, stage2_प्रविष्टिः, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    यदि(value<=0xff) {
                        length=1;
                    } अन्यथा यदि(value<=0xffff) {
                        length=2;
                    } अन्यथा यदि((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } अन्यथा यदि((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } अन्यथा {
                        length=3;
                    }
                    भङ्ग;
                स्वभाव:
                    /* must not occur */
                    /*
                     * To avoid compiler warnings that value & length may be
                     * used without having been initialized, we set them here.
                     * In reality, this is unreachable code.
                     * Not having a default branch also causes warnings with
                     * some compilers.
                     */
                    value=stage2_प्रविष्टिः=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                    भङ्ग;
                }

                /* is this code point assigned, or do we use fallbacks? */
                यदि(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_प्रविष्टिः, c)!=0 ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_तर्काः->source=source;
                    c=_extFromU(cnv, cnv->shared_दत्त,
                                c, &source, source_सीमा,
                                &target, target+target_क्षमता,
                                &offsets, source_सूचकांक,
                                p_तर्काः->flush,
                                p_Error_संहिता);
                    अग्र_Source_सूचकांक+=(int32_t)(source-p_तर्काः->source);
                    पूर्व_दीर्घता=cnv->from_Unicode_स्थितिः; /* restore SISO state */

                    यदि(U_FAILURE(*p_Error_संहिता)) {
                        /* not mappable or buffer overflow */
                        भङ्ग;
                    } अन्यथा {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        target_क्षमता=(int32_t)(p_तर्काः->target_सीमा-(वर्ण *)target);

                        /* normal end of conversion: prepare for a new character */
                        यदि(offsets!=शून्यसूचकः) {
                            पूर्व_Source_सूचकांक=source_सूचकांक;
                            source_सूचकांक=अग्र_Source_सूचकांक;
                        }
                        अनुवर्त;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            यदि(length<=target_क्षमता) {
                यदि(offsets==शून्यसूचकः) {
                    विकल्प(length) {
                        /* each branch falls through to the next one */
                    पक्ष 4:
                        *target++=(uint8_t)(value>>24);
                        U_FALLTHROUGH;
                    पक्ष 3:
                        *target++=(uint8_t)(value>>16);
                        U_FALLTHROUGH;
                    पक्ष 2:
                        *target++=(uint8_t)(value>>8);
                        U_FALLTHROUGH;
                    पक्ष 1:
                        *target++=(uint8_t)value;
                        U_FALLTHROUGH;
                    स्वभाव:
                        /* will never occur */
                        भङ्ग;
                    }
                } अन्यथा {
                    विकल्प(length) {
                        /* each branch falls through to the next one */
                    पक्ष 4:
                        *target++=(uint8_t)(value>>24);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    पक्ष 3:
                        *target++=(uint8_t)(value>>16);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    पक्ष 2:
                        *target++=(uint8_t)(value>>8);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    पक्ष 1:
                        *target++=(uint8_t)value;
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    स्वभाव:
                        /* will never occur */
                        भङ्ग;
                    }
                }
                target_क्षमता-=length;
            } अन्यथा {
                uint8_t *char_Error_बफर;

                /*
                 * We actually do this backwards here:
                 * In order to save an intermediate variable, we output
                 * first to the overflow buffer what does not fit into the
                 * regular target.
                 */
                /* we know that 1<=targetCapacity<length<=4 */
                length-=target_क्षमता;
                char_Error_बफर=(uint8_t *)cnv->char_Error_बफर;
                विकल्प(length) {
                    /* each branch falls through to the next one */
                पक्ष 3:
                    *char_Error_बफर++=(uint8_t)(value>>16);
                    U_FALLTHROUGH;
                पक्ष 2:
                    *char_Error_बफर++=(uint8_t)(value>>8);
                    U_FALLTHROUGH;
                पक्ष 1:
                    *char_Error_बफर=(uint8_t)value;
                    U_FALLTHROUGH;
                स्वभाव:
                    /* will never occur */
                    भङ्ग;
                }
                cnv->char_Error_बफर_दीर्घता=(int8_t)length;

                /* now output what fits into the regular target */
                value>>=8*length; /* length was reduced by targetCapacity */
                विकल्प(target_क्षमता) {
                    /* each branch falls through to the next one */
                पक्ष 3:
                    *target++=(uint8_t)(value>>16);
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                    U_FALLTHROUGH;
                पक्ष 2:
                    *target++=(uint8_t)(value>>8);
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                    U_FALLTHROUGH;
                पक्ष 1:
                    *target++=(uint8_t)value;
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                    U_FALLTHROUGH;
                स्वभाव:
                    /* will never occur */
                    भङ्ग;
                }

                /* target overflow */
                target_क्षमता=0;
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                c=0;
                भङ्ग;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            यदि(offsets!=शून्यसूचकः) {
                पूर्व_Source_सूचकांक=source_सूचकांक;
                source_सूचकांक=अग्र_Source_सूचकांक;
            }
            अनुवर्त;
        } अन्यथा {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }

    /*
     * the end of the input stream and detection of truncated input
     * are handled by the framework, but for EBCDIC_STATEFUL conversion
     * we need to emit an SI at the very end
     *
     * conditions:
     *   successful
     *   EBCDIC_STATEFUL in DBCS mode
     *   end of input and no truncated input
     */
    यदि( U_SUCCESS(*p_Error_संहिता) &&
        outputType==MBCS_OUTPUT_2_SISO && पूर्व_दीर्घता==2 &&
        p_तर्काः->flush && source>=source_सीमा && c==0
    ) {
        /* EBCDIC_STATEFUL ending with DBCS: emit an SI to return the output stream to SBCS */
        यदि(target_क्षमता>0) {
            *target++ = si_अष्टकानि[0];
            यदि (si_दीर्घता == 2) {
                यदि (target_क्षमता<2) {
                    cnv->char_Error_बफर[0] = si_अष्टकानि[1];
                    cnv->char_Error_बफर_दीर्घता=1;
                    *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                } अन्यथा {
                    *target++ = si_अष्टकानि[1];
                }
            }
            यदि(offsets!=शून्यसूचकः) {
                /* set the last source character's index (sourceIndex points at sourceLimit now) */
                *offsets++=पूर्व_Source_सूचकांक;
            }
        } अन्यथा {
            /* target is full */
            cnv->char_Error_बफर[0] = si_अष्टकानि[0];
            यदि (si_दीर्घता == 2) {
                cnv->char_Error_बफर[1] = si_अष्टकानि[1];
            }
            cnv->char_Error_बफर_दीर्घता=si_दीर्घता;
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        }
        पूर्व_दीर्घता=1; /* we switched into SBCS */
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;
    cnv->from_Unicode_स्थितिः=पूर्व_दीर्घता;

    /* write back the updated pointers */
    p_तर्काः->source=source;
    p_तर्काः->target=(वर्ण *)target;
    p_तर्काः->offsets=offsets;
}

/*
 * This is another simple conversion function for internal use by other
 * conversion implementations.
 * It does not use the converter state nor call callbacks.
 * It does not handle the EBCDIC swaplfnl option (set in UConverter).
 * It handles conversion extensions but not GB 18030.
 *
 * It converts one single Unicode code point into codepage bytes, encoded
 * as one 32-bit value. The function returns the number of bytes in *pValue:
 * 1..4 the number of bytes in *pValue
 * 0    unassigned (*pValue undefined)
 * -1   illegal (currently not used, *pValue undefined)
 *
 * *pValue will contain the resulting bytes with the last byte in bits 7..0,
 * the second to last byte in bits 15..8, etc.
 * Currently, the function assumes but does not check that 0<=c<=0x10ffff.
 */
U_CFUNC int32_t
ucnv_MBCSFromUChar32(UConverter_Shared_दत्त *shared_दत्त,
                 UChar32 c, uint32_t *p_मानम्,
                 UBool useFallback) {
    ध्रुव int32_t *cx;
    ध्रुव uint16_t *सारणी;
#यदि 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
    ध्रुव uint8_t *p;
#यदि_अन्तः
    uint32_t stage2_प्रविष्टिः;
    uint32_t value;
    int32_t length;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    यदि(c<=0xffff || (shared_दत्त->mbcs.unicode_आवरणम्&UCNV_HAS_SUPPLEMENTARY)) {
        सारणी=shared_दत्त->mbcs.from_Unicode_सारणी;

        /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
        यदि(shared_दत्त->mbcs.outputType==MBCS_OUTPUT_1) {
            value=MBCS_SINGLE_परिणाम_FROM_U(सारणी, (uint16_t *)shared_दत्त->mbcs.from_Unicode_अष्टकानि, c);
            /* is this code point assigned, or do we use fallbacks? */
            यदि(useFallback ? value>=0x800 : value>=0xc00) {
                *p_मानम्=value&0xff;
                प्रत्यागम 1;
            }
        } अन्यथा /* outputType!=MBCS_OUTPUT_1 */ {
            stage2_प्रविष्टिः=MBCS_STAGE_2_FROM_U(सारणी, c);

            /* get the bytes and the length for the output */
            विकल्प(shared_दत्त->mbcs.outputType) {
            पक्ष MBCS_OUTPUT_2:
                value=MBCS_मानम्_2_FROM_STAGE_2(shared_दत्त->mbcs.from_Unicode_अष्टकानि, stage2_प्रविष्टिः, c);
                यदि(value<=0xff) {
                    length=1;
                } अन्यथा {
                    length=2;
                }
                भङ्ग;
#यदि 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
            पक्ष MBCS_OUTPUT_DBCS_ONLY:
                /* table with single-byte results, but only DBCS mappings used */
                value=MBCS_मानम्_2_FROM_STAGE_2(shared_दत्त->mbcs.from_Unicode_अष्टकानि, stage2_प्रविष्टिः, c);
                यदि(value<=0xff) {
                    /* no mapping or SBCS result, not taken for DBCS-only */
                    value=stage2_प्रविष्टिः=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                } अन्यथा {
                    length=2;
                }
                भङ्ग;
            पक्ष MBCS_OUTPUT_3:
                p=MBCS_POINTER_3_FROM_STAGE_2(shared_दत्त->mbcs.from_Unicode_अष्टकानि, stage2_प्रविष्टिः, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                यदि(value<=0xff) {
                    length=1;
                } अन्यथा यदि(value<=0xffff) {
                    length=2;
                } अन्यथा {
                    length=3;
                }
                भङ्ग;
            पक्ष MBCS_OUTPUT_4:
                value=MBCS_मानम्_4_FROM_STAGE_2(shared_दत्त->mbcs.from_Unicode_अष्टकानि, stage2_प्रविष्टिः, c);
                यदि(value<=0xff) {
                    length=1;
                } अन्यथा यदि(value<=0xffff) {
                    length=2;
                } अन्यथा यदि(value<=0xffffff) {
                    length=3;
                } अन्यथा {
                    length=4;
                }
                भङ्ग;
            पक्ष MBCS_OUTPUT_3_EUC:
                value=MBCS_मानम्_2_FROM_STAGE_2(shared_दत्त->mbcs.from_Unicode_अष्टकानि, stage2_प्रविष्टिः, c);
                /* EUC 16-bit fixed-length representation */
                यदि(value<=0xff) {
                    length=1;
                } अन्यथा यदि((value&0x8000)==0) {
                    value|=0x8e8000;
                    length=3;
                } अन्यथा यदि((value&0x80)==0) {
                    value|=0x8f0080;
                    length=3;
                } अन्यथा {
                    length=2;
                }
                भङ्ग;
            पक्ष MBCS_OUTPUT_4_EUC:
                p=MBCS_POINTER_3_FROM_STAGE_2(shared_दत्त->mbcs.from_Unicode_अष्टकानि, stage2_प्रविष्टिः, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                /* EUC 16-bit fixed-length representation applied to the first two bytes */
                यदि(value<=0xff) {
                    length=1;
                } अन्यथा यदि(value<=0xffff) {
                    length=2;
                } अन्यथा यदि((value&0x800000)==0) {
                    value|=0x8e800000;
                    length=4;
                } अन्यथा यदि((value&0x8000)==0) {
                    value|=0x8f008000;
                    length=4;
                } अन्यथा {
                    length=3;
                }
                भङ्ग;
#यदि_अन्तः
            स्वभाव:
                /* must not occur */
                प्रत्यागम -1;
            }

            /* is this code point assigned, or do we use fallbacks? */
            यदि( MBCS_FROM_U_IS_ROUNDTRIP(stage2_प्रविष्टिः, c) ||
                (FROM_U_USE_FALLBACK(useFallback, c) && value!=0)
            ) {
                /*
                 * We allow a 0 byte output if the "assigned" bit is set for this entry.
                 * There is no way with this data structure for fallback output
                 * to be a zero byte.
                 */
                /* assigned */
                *p_मानम्=value;
                प्रत्यागम length;
            }
        }
    }

    cx=shared_दत्त->mbcs.extIndexes;
    यदि(cx!=शून्यसूचकः) {
        length=ucnv_ext_Simple_मेलनम्_From_U(cx, c, p_मानम्, useFallback);
        प्रत्यागम length>=0 ? length : -length;  /* return abs(length); */
    }

    /* unassigned */
    प्रत्यागम 0;
}


#यदि 0
/*
 * This function has been moved to ucnv2022.c for inlining.
 * This implementation is here only for documentation purposes
 */

/**
 * This version of ucnv_MBCSFromUChar32() is optimized for single-byte codepages.
 * It does not handle the EBCDIC swaplfnl option (set in UConverter).
 * It does not handle conversion extensions (_extFromU()).
 *
 * It returns the codepage byte for the code point, or -1 if it is unassigned.
 */
U_CFUNC int32_t
ucnv_MBCSSingleFromUChar32(UConverter_Shared_दत्त *shared_दत्त,
                       UChar32 c,
                       UBool useFallback) {
    ध्रुव uint16_t *सारणी;
    int32_t value;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    यदि(c>=0x10000 && !(shared_दत्त->mbcs.unicode_आवरणम्&UCNV_HAS_SUPPLEMENTARY)) {
        प्रत्यागम -1;
    }

    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    सारणी=shared_दत्त->mbcs.from_Unicode_सारणी;

    /* get the byte for the output */
    value=MBCS_SINGLE_परिणाम_FROM_U(सारणी, (uint16_t *)shared_दत्त->mbcs.from_Unicode_अष्टकानि, c);
    /* is this code point assigned, or do we use fallbacks? */
    यदि(useFallback ? value>=0x800 : value>=0xc00) {
        प्रत्यागम value&0xff;
    } अन्यथा {
        प्रत्यागम -1;
    }
}
#यदि_अन्तः

/* MBCS-from-UTF-8 conversion functions ------------------------------------- */

/* offsets for n-byte UTF-8 sequences that were calculated with ((lead<<6)+trail)<<6+trail... */
स्थिर ध्रुव UChar32
utf8_offsets[5]={ 0, 0, 0x3080, 0xE2080, 0x3C82080 };

स्थिर शून्य U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_तर्काः *pFromUArgs,
                  UConverter_गन्तव्यम्_Unicode_तर्काः *p_गन्तव्यम्_UArgs,
                  UError_संहिता *p_Error_संहिता) {
    UConverter *utf8, *cnv;
    ध्रुव uint8_t *source, *source_सीमा;
    uint8_t *target;
    int32_t target_क्षमता;

    ध्रुव uint16_t *सारणी, *sbcs_सूचकांक;
    ध्रुव uint16_t *results;

    int8_t old_गन्तव्यम्_ULength, गन्तव्यम्_ULength, गन्तव्यम्_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t asciiRoundtrips;
    uint16_t value, min_मानम् = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_गन्तव्यम्_UArgs->converter;
    cnv=pFromUArgs->converter;
    source=(uint8_t *)p_गन्तव्यम्_UArgs->source;
    source_सीमा=(uint8_t *)p_गन्तव्यम्_UArgs->source_सीमा;
    target = reinterpret_cast<uint8_t*>(pFromUArgs->target);
    target_क्षमता = static_cast<int32_t>(pFromUArgs->target_सीमा - pFromUArgs->target);

    सारणी=cnv->shared_दत्त->mbcs.from_Unicode_सारणी;
    sbcs_सूचकांक=cnv->shared_दत्त->mbcs.sbcs_सूचकांक;
    यदि((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_दत्त->mbcs.swap_LFNLFrom_Unicode_अष्टकानि);
    } अन्यथा {
        results=(uint16_t *)cnv->shared_दत्त->mbcs.from_Unicode_अष्टकानि;
    }
    asciiRoundtrips=cnv->shared_दत्त->mbcs.asciiRoundtrips;

    यदि(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_मानम्=0x800;
    } अन्यथा {
        /* use only roundtrips and fallbacks from private-use characters */
        min_मानम्=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_दत्त->mbcs.unicode_आवरणम् & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    यदि(utf8->गन्तव्यम्_ULength > 0) {
        गन्तव्यम्_ULength=old_गन्तव्यम्_ULength=utf8->गन्तव्यम्_ULength;
        गन्तव्यम्_ULimit = static_cast<int8_t>(utf8->बहुलकम्);
        c = static_cast<UChar32>(utf8->गन्तव्यम्_Unicode_स्थितिः);
    } अन्यथा {
        गन्तव्यम्_ULength=old_गन्तव्यम्_ULength=गन्तव्यम्_ULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(source_सीमा - source) - (गन्तव्यम्_ULimit - old_गन्तव्यम्_ULength);
        यदि(length>0) {
            uint8_t b1=*(source_सीमा-1);
            यदि(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } अन्यथा यदि(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(source_सीमा-2);
                यदि(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    source_सीमा-=2;
                }
            } अन्यथा यदि(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --source_सीमा;
            }
        }
    }

    यदि(c!=0 && target_क्षमता>0) {
        utf8->गन्तव्यम्_Unicode_स्थितिः=0;
        utf8->गन्तव्यम्_ULength=0;
        गच्छ अधिकम्_अष्टकानि;
        /*
         * Note: We could avoid the goto by duplicating some of the moreBytes
         * code, but only up to the point of collecting a complete UTF-8
         * sequence; then recurse for the toUBytes[toULength]
         * and then continue with normal conversion.
         *
         * If so, move this code to just after initializing the minimum
         * set of local variables for reading the UTF-8 input
         * (utf8, source, target, limits but not cnv, table, minValue, etc.).
         *
         * Potential advantages:
         * - avoid the goto
         * - oldToULength could become a local variable in just those code blocks
         *   that deal with buffer boundaries
         * - possibly faster if the goto prevents some compiler optimizations
         *   (this would need measuring to confirm)
         * Disadvantage:
         * - code duplication
         */
    }

    /* conversion loop */
    यावत्(source<source_सीमा) {
        यदि(target_क्षमता>0) {
            b=*source++;
            यदि(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                यदि(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *target++ = b;
                    --target_क्षमता;
                    अनुवर्त;
                } अन्यथा {
                    c=b;
                    value=SBCS_परिणाम_FROM_UTF8(sbcs_सूचकांक, results, 0, c);
                }
            } अन्यथा {
                यदि(b<0xe0) {
                    यदि( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*source - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++source;
                        value=SBCS_परिणाम_FROM_UTF8(sbcs_सूचकांक, results, c, t1);
                        यदि(value>=min_मानम्) {
                            *target++ = static_cast<uint8_t>(value);
                            --target_क्षमता;
                            अनुवर्त;
                        } अन्यथा {
                            c=(c<<6)|t1;
                        }
                    } अन्यथा {
                        c=-1;
                    }
                } अन्यथा यदि(b==0xe0) {
                    यदि( /* handle U+0800..U+0FFF inline */
                        (t1 = static_cast<uint8_t>(source[0] - 0x80)) <= 0x3f && t1 >= 0x20 &&
                        (t2 = static_cast<uint8_t>(source[1] - 0x80)) <= 0x3f
                    ) {
                        c=t1;
                        source+=2;
                        value=SBCS_परिणाम_FROM_UTF8(sbcs_सूचकांक, results, c, t2);
                        यदि(value>=min_मानम्) {
                            *target++ = static_cast<uint8_t>(value);
                            --target_क्षमता;
                            अनुवर्त;
                        } अन्यथा {
                            c=(c<<6)|t2;
                        }
                    } अन्यथा {
                        c=-1;
                    }
                } अन्यथा {
                    c=-1;
                }

                यदि(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    old_गन्तव्यम्_ULength=0;
                    गन्तव्यम्_ULength=1;
                    गन्तव्यम्_ULimit=U8_गणनम्_अष्टकानि_NON_ASCII(b);
                    c=b;
अधिकम्_अष्टकानि:
                    यावत्(गन्तव्यम्_ULength<गन्तव्यम्_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        यदि(source<(uint8_t *)p_गन्तव्यम्_UArgs->source_सीमा) {
                            b=*source;
                            यदि(icu::UTF8::isValidTrail(c, b, गन्तव्यम्_ULength, गन्तव्यम्_ULimit)) {
                                ++source;
                                ++गन्तव्यम्_ULength;
                                c=(c<<6)+b;
                            } अन्यथा {
                                भङ्ग; /* sequence too short, stop with toULength<toULimit */
                            }
                        } अन्यथा {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            source-=(गन्तव्यम्_ULength-old_गन्तव्यम्_ULength);
                            यावत्(old_गन्तव्यम्_ULength<गन्तव्यम्_ULength) {
                                utf8->गन्तव्यम्_UBytes[old_गन्तव्यम्_ULength++]=*source++;
                            }
                            utf8->गन्तव्यम्_Unicode_स्थितिः=c;
                            utf8->गन्तव्यम्_ULength=गन्तव्यम्_ULength;
                            utf8->बहुलकम्=गन्तव्यम्_ULimit;
                            p_गन्तव्यम्_UArgs->source=(वर्ण *)source;
                            pFromUArgs->target = reinterpret_cast<वर्ण*>(target);
                            प्रत्यागम;
                        }
                    }

                    यदि(गन्तव्यम्_ULength==गन्तव्यम्_ULimit) {
                        c-=utf8_offsets[गन्तव्यम्_ULength];
                        यदि(गन्तव्यम्_ULength<=3) {  /* BMP */
                            value=MBCS_SINGLE_परिणाम_FROM_U(सारणी, results, c);
                        } अन्यथा {
                            /* supplementary code point */
                            यदि(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                value=0;
                            } अन्यथा {
                                value=MBCS_SINGLE_परिणाम_FROM_U(सारणी, results, c);
                            }
                        }
                    } अन्यथा {
                        /* error handling: illegal UTF-8 byte sequence */
                        source-=(गन्तव्यम्_ULength-old_गन्तव्यम्_ULength);
                        यावत्(old_गन्तव्यम्_ULength<गन्तव्यम्_ULength) {
                            utf8->गन्तव्यम्_UBytes[old_गन्तव्यम्_ULength++]=*source++;
                        }
                        utf8->गन्तव्यम्_ULength=गन्तव्यम्_ULength;
                        p_गन्तव्यम्_UArgs->source=(वर्ण *)source;
                        pFromUArgs->target = reinterpret_cast<वर्ण*>(target);
                        *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                        प्रत्यागम;
                    }
                }
            }

            यदि(value>=min_मानम्) {
                /* output the mapping for c */
                *target++ = static_cast<uint8_t>(value);
                --target_क्षमता;
            } अन्यथा {
                /* value<minValue means c is unassigned (unmappable) */
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                स्थिर ध्रुव char16_t nul=0;
                ध्रुव char16_t *noSource=&nul;
                c=_extFromU(cnv, cnv->shared_दत्त,
                            c, &noSource, noSource,
                            &target, target+target_क्षमता,
                            शून्यसूचकः, -1,
                            pFromUArgs->flush,
                            p_Error_संहिता);

                यदि(U_FAILURE(*p_Error_संहिता)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    भङ्ग;
                } अन्यथा यदि(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *p_Error_संहिता=U_USING_DEFAULT_WARNING;
                    भङ्ग;
                } अन्यथा {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    target_क्षमता = static_cast<int32_t>(pFromUArgs->target_सीमा - reinterpret_cast<वर्ण*>(target));
                }
            }
        } अन्यथा {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    यदि(U_SUCCESS(*p_Error_संहिता) &&
            cnv->preFromUFirstCP<0 &&
            source<(source_सीमा=(uint8_t *)p_गन्तव्यम्_UArgs->source_सीमा)) {
        c=utf8->गन्तव्यम्_UBytes[0]=b=*source++;
        गन्तव्यम्_ULength=1;
        गन्तव्यम्_ULimit=U8_गणनम्_अष्टकानि(b);
        यावत्(source<source_सीमा) {
            utf8->गन्तव्यम्_UBytes[गन्तव्यम्_ULength++]=b=*source++;
            c=(c<<6)+b;
        }
        utf8->गन्तव्यम्_Unicode_स्थितिः=c;
        utf8->गन्तव्यम्_ULength=गन्तव्यम्_ULength;
        utf8->बहुलकम्=गन्तव्यम्_ULimit;
    }

    /* write back the updated pointers */
    p_गन्तव्यम्_UArgs->source=(वर्ण *)source;
    pFromUArgs->target = reinterpret_cast<वर्ण*>(target);
}

स्थिर शून्य U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_तर्काः *pFromUArgs,
                  UConverter_गन्तव्यम्_Unicode_तर्काः *p_गन्तव्यम्_UArgs,
                  UError_संहिता *p_Error_संहिता) {
    UConverter *utf8, *cnv;
    ध्रुव uint8_t *source, *source_सीमा;
    uint8_t *target;
    int32_t target_क्षमता;

    ध्रुव uint16_t *सारणी, *mbcs_सूचकांक;
    ध्रुव uint16_t *results;

    int8_t old_गन्तव्यम्_ULength, गन्तव्यम्_ULength, गन्तव्यम्_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t stage2_प्रविष्टिः;
    uint32_t asciiRoundtrips;
    uint16_t value = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_गन्तव्यम्_UArgs->converter;
    cnv=pFromUArgs->converter;
    source=(uint8_t *)p_गन्तव्यम्_UArgs->source;
    source_सीमा=(uint8_t *)p_गन्तव्यम्_UArgs->source_सीमा;
    target = reinterpret_cast<uint8_t*>(pFromUArgs->target);
    target_क्षमता = static_cast<int32_t>(pFromUArgs->target_सीमा - pFromUArgs->target);

    सारणी=cnv->shared_दत्त->mbcs.from_Unicode_सारणी;
    mbcs_सूचकांक=cnv->shared_दत्त->mbcs.mbcs_सूचकांक;
    यदि((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_दत्त->mbcs.swap_LFNLFrom_Unicode_अष्टकानि);
    } अन्यथा {
        results=(uint16_t *)cnv->shared_दत्त->mbcs.from_Unicode_अष्टकानि;
    }
    asciiRoundtrips=cnv->shared_दत्त->mbcs.asciiRoundtrips;

    hasSupplementary = static_cast<UBool>(cnv->shared_दत्त->mbcs.unicode_आवरणम् & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    यदि(utf8->गन्तव्यम्_ULength > 0) {
        गन्तव्यम्_ULength=old_गन्तव्यम्_ULength=utf8->गन्तव्यम्_ULength;
        गन्तव्यम्_ULimit = static_cast<int8_t>(utf8->बहुलकम्);
        c = static_cast<UChar32>(utf8->गन्तव्यम्_Unicode_स्थितिः);
    } अन्यथा {
        गन्तव्यम्_ULength=old_गन्तव्यम्_ULength=गन्तव्यम्_ULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(source_सीमा - source) - (गन्तव्यम्_ULimit - old_गन्तव्यम्_ULength);
        यदि(length>0) {
            uint8_t b1=*(source_सीमा-1);
            यदि(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } अन्यथा यदि(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(source_सीमा-2);
                यदि(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    source_सीमा-=2;
                }
            } अन्यथा यदि(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --source_सीमा;
            }
        }
    }

    यदि(c!=0 && target_क्षमता>0) {
        utf8->गन्तव्यम्_Unicode_स्थितिः=0;
        utf8->गन्तव्यम्_ULength=0;
        गच्छ अधिकम्_अष्टकानि;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    यावत्(source<source_सीमा) {
        यदि(target_क्षमता>0) {
            b=*source++;
            यदि(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                यदि(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *target++=b;
                    --target_क्षमता;
                    अनुवर्त;
                } अन्यथा {
                    value=DBCS_परिणाम_FROM_UTF8(mbcs_सूचकांक, results, 0, b);
                    यदि(value==0) {
                        c=b;
                        गच्छ unassigned;
                    }
                }
            } अन्यथा {
                यदि(b>=0xe0) {
                    यदि( /* handle U+0800..U+D7FF inline */
                        b<=0xed &&  // do not assume maxFastUChar>0xd7ff
                        U8_IS_VALID_LEAD3_AND_T1(b, t1=source[0]) &&
                        (t2 = static_cast<uint8_t>(source[1] - 0x80)) <= 0x3f
                    ) {
                        c=((b&0xf)<<6)|(t1&0x3f);
                        source+=2;
                        value=DBCS_परिणाम_FROM_UTF8(mbcs_सूचकांक, results, c, t2);
                        यदि(value==0) {
                            c=(c<<6)|t2;
                            गच्छ unassigned;
                        }
                    } अन्यथा {
                        c=-1;
                    }
                } अन्यथा {
                    यदि( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*source - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++source;
                        value=DBCS_परिणाम_FROM_UTF8(mbcs_सूचकांक, results, c, t1);
                        यदि(value==0) {
                            c=(c<<6)|t1;
                            गच्छ unassigned;
                        }
                    } अन्यथा {
                        c=-1;
                    }
                }

                यदि(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    old_गन्तव्यम्_ULength=0;
                    गन्तव्यम्_ULength=1;
                    गन्तव्यम्_ULimit=U8_गणनम्_अष्टकानि_NON_ASCII(b);
                    c=b;
अधिकम्_अष्टकानि:
                    यावत्(गन्तव्यम्_ULength<गन्तव्यम्_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        यदि(source<(uint8_t *)p_गन्तव्यम्_UArgs->source_सीमा) {
                            b=*source;
                            यदि(icu::UTF8::isValidTrail(c, b, गन्तव्यम्_ULength, गन्तव्यम्_ULimit)) {
                                ++source;
                                ++गन्तव्यम्_ULength;
                                c=(c<<6)+b;
                            } अन्यथा {
                                भङ्ग; /* sequence too short, stop with toULength<toULimit */
                            }
                        } अन्यथा {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            source-=(गन्तव्यम्_ULength-old_गन्तव्यम्_ULength);
                            यावत्(old_गन्तव्यम्_ULength<गन्तव्यम्_ULength) {
                                utf8->गन्तव्यम्_UBytes[old_गन्तव्यम्_ULength++]=*source++;
                            }
                            utf8->गन्तव्यम्_Unicode_स्थितिः=c;
                            utf8->गन्तव्यम्_ULength=गन्तव्यम्_ULength;
                            utf8->बहुलकम्=गन्तव्यम्_ULimit;
                            p_गन्तव्यम्_UArgs->source=(वर्ण *)source;
                            pFromUArgs->target = reinterpret_cast<वर्ण*>(target);
                            प्रत्यागम;
                        }
                    }

                    यदि(गन्तव्यम्_ULength==गन्तव्यम्_ULimit) {
                        c-=utf8_offsets[गन्तव्यम्_ULength];
                        यदि(गन्तव्यम्_ULength<=3) {  /* BMP */
                            stage2_प्रविष्टिः=MBCS_STAGE_2_FROM_U(सारणी, c);
                        } अन्यथा {
                            /* supplementary code point */
                            यदि(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                stage2_प्रविष्टिः=0;
                            } अन्यथा {
                                stage2_प्रविष्टिः=MBCS_STAGE_2_FROM_U(सारणी, c);
                            }
                        }
                    } अन्यथा {
                        /* error handling: illegal UTF-8 byte sequence */
                        source-=(गन्तव्यम्_ULength-old_गन्तव्यम्_ULength);
                        यावत्(old_गन्तव्यम्_ULength<गन्तव्यम्_ULength) {
                            utf8->गन्तव्यम्_UBytes[old_गन्तव्यम्_ULength++]=*source++;
                        }
                        utf8->गन्तव्यम्_ULength=गन्तव्यम्_ULength;
                        p_गन्तव्यम्_UArgs->source=(वर्ण *)source;
                        pFromUArgs->target = reinterpret_cast<वर्ण*>(target);
                        *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                        प्रत्यागम;
                    }

                    /* get the bytes and the length for the output */
                    /* MBCS_OUTPUT_2 */
                    value=MBCS_मानम्_2_FROM_STAGE_2(results, stage2_प्रविष्टिः, c);

                    /* is this code point assigned, or do we use fallbacks? */
                    यदि(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_प्रविष्टिः, c) ||
                         (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                    ) {
                        गच्छ unassigned;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            यदि(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *target++ = static_cast<uint8_t>(value);
                --target_क्षमता;
            } अन्यथा /* length==2 */ {
                *target++ = static_cast<uint8_t>(value >> 8);
                यदि(2<=target_क्षमता) {
                    *target++ = static_cast<uint8_t>(value);
                    target_क्षमता-=2;
                } अन्यथा {
                    cnv->char_Error_बफर[0] = static_cast<वर्ण>(value);
                    cnv->char_Error_बफर_दीर्घता=1;

                    /* target overflow */
                    *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                    भङ्ग;
                }
            }
            अनुवर्त;

unassigned:
            {
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                स्थिर ध्रुव char16_t nul=0;
                ध्रुव char16_t *noSource=&nul;
                c=_extFromU(cnv, cnv->shared_दत्त,
                            c, &noSource, noSource,
                            &target, target+target_क्षमता,
                            शून्यसूचकः, -1,
                            pFromUArgs->flush,
                            p_Error_संहिता);

                यदि(U_FAILURE(*p_Error_संहिता)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    भङ्ग;
                } अन्यथा यदि(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *p_Error_संहिता=U_USING_DEFAULT_WARNING;
                    भङ्ग;
                } अन्यथा {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    target_क्षमता = static_cast<int32_t>(pFromUArgs->target_सीमा - reinterpret_cast<वर्ण*>(target));
                    अनुवर्त;
                }
            }
        } अन्यथा {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    यदि(U_SUCCESS(*p_Error_संहिता) &&
            cnv->preFromUFirstCP<0 &&
            source<(source_सीमा=(uint8_t *)p_गन्तव्यम्_UArgs->source_सीमा)) {
        c=utf8->गन्तव्यम्_UBytes[0]=b=*source++;
        गन्तव्यम्_ULength=1;
        गन्तव्यम्_ULimit=U8_गणनम्_अष्टकानि(b);
        यावत्(source<source_सीमा) {
            utf8->गन्तव्यम्_UBytes[गन्तव्यम्_ULength++]=b=*source++;
            c=(c<<6)+b;
        }
        utf8->गन्तव्यम्_Unicode_स्थितिः=c;
        utf8->गन्तव्यम्_ULength=गन्तव्यम्_ULength;
        utf8->बहुलकम्=गन्तव्यम्_ULimit;
    }

    /* write back the updated pointers */
    p_गन्तव्यम्_UArgs->source=(वर्ण *)source;
    pFromUArgs->target = reinterpret_cast<वर्ण*>(target);
}

/* miscellaneous ------------------------------------------------------------ */

स्थिर शून्य U_CALLCONV
ucnv_MBCSGetStarters(ध्रुव UConverter* cnv,
                 UBool starters[256],
                 UError_संहिता *) {
    ध्रुव int32_t *state0;
    पूर्णाङ्क i;

    state0=cnv->shared_दत्त->mbcs.state_सारणी[cnv->shared_दत्त->mbcs.dbcsOnlyState];
    प्रति(i=0; i<256; ++i) {
        /* all bytes that cause a state transition from state 0 are lead bytes */
        starters[i] = static_cast<UBool>(MBCS_प्रविष्टिः_IS_TRANSITION(state0[i]));
    }
}

/*
 * This is an internal function that allows other converter implementations
 * to check whether a byte is a lead byte.
 */
U_CFUNC UBool
ucnv_MBCSIsLeadByte(UConverter_Shared_दत्त *shared_दत्त, वर्ण byte) {
    प्रत्यागम MBCS_प्रविष्टिः_IS_TRANSITION(shared_दत्त->mbcs.state_सारणी[0][(uint8_t)byte]);
}

स्थिर शून्य U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_तर्काः *p_तर्काः,
              int32_t विस्थापनम्_सूचकांक,
              UError_संहिता *p_Error_संहिता) {
    UConverter *cnv=p_तर्काः->converter;
    वर्ण *p, *subchar;
    वर्ण बफर[4];
    int32_t length;

    /* first, select between subChar and subChar1 */
    यदि( cnv->subChar1!=0 &&
        (cnv->shared_दत्त->mbcs.extIndexes!=शून्यसूचकः ?
            cnv->useSubChar1 :
            (cnv->invalid_UChar_बफर[0]<=0xff))
    ) {
        /* select subChar1 if it is set (not 0) and the unmappable Unicode code point is up to U+00ff (IBM MBCS behavior) */
        subchar = reinterpret_cast<वर्ण*>(&cnv->subChar1);
        length=1;
    } अन्यथा {
        /* select subChar in all other cases */
        subchar = reinterpret_cast<वर्ण*>(cnv->subChars);
        length=cnv->subCharLen;
    }

    /* reset the selector for the next code point */
    cnv->useSubChar1=असत्य;

    यदि (cnv->shared_दत्त->mbcs.outputType == MBCS_OUTPUT_2_SISO) {
        p=बफर;

        /* fromUnicodeStatus contains prevLength */
        विकल्प(length) {
        पक्ष 1:
            यदि(cnv->from_Unicode_स्थितिः==2) {
                /* DBCS mode and SBCS sub char: change to SBCS */
                cnv->from_Unicode_स्थितिः=1;
                *p++=UCNV_SI;
            }
            *p++=subchar[0];
            भङ्ग;
        पक्ष 2:
            यदि(cnv->from_Unicode_स्थितिः<=1) {
                /* SBCS mode and DBCS sub char: change to DBCS */
                cnv->from_Unicode_स्थितिः=2;
                *p++=UCNV_SO;
            }
            *p++=subchar[0];
            *p++=subchar[1];
            भङ्ग;
        स्वभाव:
            *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम;
        }
        subchar=बफर;
        length = static_cast<int32_t>(p - बफर);
    }

    ucnv_cb_From_UWrite_अष्टकानि(p_तर्काः, subchar, length, विस्थापनम्_सूचकांक, p_Error_संहिता);
}

U_CFUNC UConverterType
ucnv_MBCSGetType(ध्रुव UConverter* converter) {
    /* SBCS, DBCS, and EBCDIC_STATEFUL are replaced by MBCS, but here we cheat a little */
    यदि(converter->shared_दत्त->mbcs.गणनम्_अवस्थाः==1) {
        प्रत्यागम (UConverterType)UCNV_SBCS;
    } अन्यथा यदि((converter->shared_दत्त->mbcs.outputType&0xff)==MBCS_OUTPUT_2_SISO) {
        प्रत्यागम (UConverterType)UCNV_EBCDIC_STATEFUL;
    } अन्यथा यदि(converter->shared_दत्त->static_दत्त->min_अष्टकानि_प्रत्येकम्_Char==2 && converter->shared_दत्त->static_दत्त->max_अष्टकानि_प्रत्येकम्_Char==2) {
        प्रत्यागम (UConverterType)UCNV_DBCS;
    }
    प्रत्यागम (UConverterType)UCNV_MBCS;
}

#यदि_अन्तः /* #if !UCONFIG_NO_LEGACY_CONVERSION */
