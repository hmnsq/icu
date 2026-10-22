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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "unicode/ucnv_cb.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/uset.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnvmbcs.h"
#ضمن "ucnv_ext.h"
#ضمن "ucnv_cnv.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "umutex.h"
#ضمن "ustr_imp.h"

/* control optimizations according to the platform */
#وصف_ڪريو MBCS_UNROLL_SINGLE_منزل_BMP 1
#وصف_ڪريو MBCS_UNROLL_SINGLE_FROM_BMP 0

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
تعريفنوع UBool U_CALLCONV
UConverter_Enum_منزل_UCallback(ثابت فراغ *context, uint32_t value, UChar32 ڪوڊ_Points[32]);

ساكن فراغ U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_معلومات *shared_معلومات,
          UConverter_ڀريو_دليلون *p_دليلون,
          ثابت uint8_t *raw,
          UError_ڪوڊ *p_Error_ڪوڊ);

ساكن فراغ U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_معلومات *shared_معلومات);

ساكن فراغ U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_ڀريو_دليلون *p_دليلون,
              UError_ڪوڊ *p_Error_ڪوڊ);

ساكن UChar32 U_CALLCONV
ucnv_MBCSGet_ايندڙ_UChar(UConverter_منزل_Unicode_دليلون *p_دليلون,
                  UError_ڪوڊ *p_Error_ڪوڊ);

ساكن فراغ U_CALLCONV
ucnv_MBCSGetStarters(ثابت UConverter* cnv,
                 UBool starters[256],
                 UError_ڪوڊ *p_Error_ڪوڊ);

U_CDECL_BEGIN
ساكن ثابت حرف* U_CALLCONV
ucnv_MBCSGetName(ثابت UConverter *cnv);
U_CDECL_پڇاڙي

ساكن فراغ U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_دليلون *p_دليلون,
              int32_t وٿي_اشاريو,
              UError_ڪوڊ *p_Error_ڪوڊ);

ساكن UChar32 U_CALLCONV
ucnv_MBCSGet_ايندڙ_UChar(UConverter_منزل_Unicode_دليلون *p_دليلون,
                  UError_ڪوڊ *p_Error_ڪوڊ);

ساكن فراغ U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_دليلون *pFromUArgs,
                  UConverter_منزل_Unicode_دليلون *p_منزل_UArgs,
                  UError_ڪوڊ *p_Error_ڪوڊ);

ساكن فراغ U_CALLCONV
ucnv_MBCSGet_Unicode_سيٽ(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverter_Unicode_سيٽ which,
                   UError_ڪوڊ *p_Error_ڪوڊ);

ساكن فراغ U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_دليلون *pFromUArgs,
                  UConverter_منزل_Unicode_دليلون *p_منزل_UArgs,
                  UError_ڪوڊ *p_Error_ڪوڊ);

ساكن ثابت UConverterImpl _SBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    خالي_اشارو,
    خالي_اشارو,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_ايندڙ_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    خالي_اشارو,
    ucnv_MBCSGet_Unicode_سيٽ,

    خالي_اشارو,
    ucnv_SBCSFromUTF8
};

ساكن ثابت UConverterImpl _DBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    خالي_اشارو,
    خالي_اشارو,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_ايندڙ_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    خالي_اشارو,
    ucnv_MBCSGet_Unicode_سيٽ,

    خالي_اشارو,
    ucnv_DBCSFromUTF8
};

ساكن ثابت UConverterImpl _MBCSImpl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    خالي_اشارو,
    خالي_اشارو,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_ايندڙ_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    خالي_اشارو,
    ucnv_MBCSGet_Unicode_سيٽ,
    خالي_اشارو,
    خالي_اشارو
};

/* Static data is in tools/makeconv/ucnvstat.c for data-based
 * converters. Be sure to update it as well.
 */

ثابت UConverter_Shared_معلومات _MBCSData={
    حجم(UConverter_Shared_معلومات), 1,
    خالي_اشارو, خالي_اشارو, خطأ, سچ, &_MBCSImpl,
    0, UCNV_MBCS_جدول_INITIALIZER
};


/* GB 18030 data ------------------------------------------------------------ */

/* helper macros for linear values for GB 18030 four-byte sequences */
#وصف_ڪريو لڪيري_18030(a, b, c, d) ((((a)*10+(b))*126L+(c))*10L+(d))

#وصف_ڪريو لڪيري_18030_بنياد لڪيري_18030(0x81, 0x30, 0x81, 0x30)

#وصف_ڪريو لڪيري(x) لڪيري_18030(x>>24, (x>>16)&0xff, (x>>8)&0xff, x&0xff)

/*
 * Some ranges of GB 18030 where both the Unicode code points and the
 * GB four-byte sequences are contiguous and are handled algorithmically by
 * the special callback functions below.
 * The values are start & end of Unicode & GB codes.
 *
 * Note that single surrogates are not mapped by GB 18030
 * as of the re-released mapping tables from 2000-nov-30.
 */
ساكن ثابت uint32_t
gb18030Ranges[14][4]={
    {0x10000, 0x10FFFF, لڪيري(0x90308130), لڪيري(0xE3329A35)},
    {0x9FA6, 0xD7FF, لڪيري(0x82358F33), لڪيري(0x8336C738)},
    {0x0452, 0x1E3E, لڪيري(0x8130D330), لڪيري(0x8135F436)},
    {0x1E40, 0x200F, لڪيري(0x8135F438), لڪيري(0x8136A531)},
    {0xE865, 0xF92B, لڪيري(0x8336D030), لڪيري(0x84308534)},
    {0x2643, 0x2E80, لڪيري(0x8137A839), لڪيري(0x8138FD38)},
    {0xFA2A, 0xFE2F, لڪيري(0x84309C38), لڪيري(0x84318537)},
    {0x3CE1, 0x4055, لڪيري(0x8231D438), لڪيري(0x8232AF32)},
    {0x361B, 0x3917, لڪيري(0x8230A633), لڪيري(0x8230F237)},
    {0x49B8, 0x4C76, لڪيري(0x8234A131), لڪيري(0x8234E733)},
    {0x4160, 0x4336, لڪيري(0x8232C937), لڪيري(0x8232F837)},
    {0x478E, 0x4946, لڪيري(0x8233E838), لڪيري(0x82349638)},
    {0x44D7, 0x464B, لڪيري(0x8233A339), لڪيري(0x8233C931)},
    {0xFFE6, 0xFFFF, لڪيري(0x8431A234), لڪيري(0x8431A439)}
};

/* bit flag for UConverter.options indicating GB 18030 special handling */
#وصف_ڪريو _MBCS_OPTION_GB18030 0x8000

/* bit flag for UConverter.options indicating KEIS,JEF,JIF special handling */
#وصف_ڪريو _MBCS_OPTION_KEIS 0x01000
#وصف_ڪريو _MBCS_OPTION_JEF  0x02000
#وصف_ڪريو _MBCS_OPTION_JIPS 0x04000

#وصف_ڪريو KEIS_SO_CHAR_1 0x0A
#وصف_ڪريو KEIS_SO_CHAR_2 0x42
#وصف_ڪريو KEIS_SI_CHAR_1 0x0A
#وصف_ڪريو KEIS_SI_CHAR_2 0x41

#وصف_ڪريو JEF_SO_CHAR 0x28
#وصف_ڪريو JEF_SI_CHAR 0x29

#وصف_ڪريو JIPS_SO_CHAR_1 0x1A
#وصف_ڪريو JIPS_SO_CHAR_2 0x70
#وصف_ڪريو JIPS_SI_CHAR_1 0x1A
#وصف_ڪريو JIPS_SI_CHAR_2 0x71

تعداد SISO_Option {
    SI,
    SO
};
تعريفنوع تعداد SISO_Option SISO_Option;

ساكن int32_t getSISOBytes(SISO_Option option, uint32_t cnvOption, uint8_t *value) {
    int32_t SISOLength = 0;

    بدل (option) {
        حالة SI:
            إذا ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SI_CHAR_1;
                value[1] = KEIS_SI_CHAR_2;
                SISOLength = 2;
            } وإلا إذا ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SI_CHAR;
                SISOLength = 1;
            } وإلا إذا ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SI_CHAR_1;
                value[1] = JIPS_SI_CHAR_2;
                SISOLength = 2;
            } وإلا {
                value[0] = UCNV_SI;
                SISOLength = 1;
            }
            اكسر;
        حالة SO:
            إذا ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SO_CHAR_1;
                value[1] = KEIS_SO_CHAR_2;
                SISOLength = 2;
            } وإلا إذا ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SO_CHAR;
                SISOLength = 1;
            } وإلا إذا ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SO_CHAR_1;
                value[1] = JIPS_SO_CHAR_2;
                SISOLength = 2;
            } وإلا {
                value[0] = UCNV_SO;
                SISOLength = 1;
            }
            اكسر;
        افتراضي:
            /* Should never happen. */
            اكسر;
    }

    أرجع SISOLength;
}

/* Miscellaneous ------------------------------------------------------------ */

/* similar to ucnv_MBCSGetNextUChar() but recursive */
ساكن UBool
enum_منزل_U(UConverterMBCSTable *mbcs_جدول, int8_t stateProps[],
        int32_t state, uint32_t وٿي,
        uint32_t value,
        UConverter_Enum_منزل_UCallback *callback, ثابت فراغ *context,
        UError_ڪوڊ *p_Error_ڪوڊ) {
    UChar32 ڪوڊ_Points[32];
    ثابت int32_t *صف;
    ثابت uint16_t *unicode_ڪوڊ_Units;
    UChar32 any_ڪوڊ_Points;
    int32_t b, حد_بندي;

    صف=mbcs_جدول->state_جدول[state];
    unicode_ڪوڊ_Units=mbcs_جدول->unicode_ڪوڊ_Units;

    value<<=8;
    any_ڪوڊ_Points=-1;  /* becomes non-negative if there is a mapping */

    b=(stateProps[state]&0x38)<<2;
    إذا(b==0 && stateProps[state]>=0x40) {
        /* skip byte sequences with leading zeros because they are not stored in the fromUnicode table */
        ڪوڊ_Points[0]=U_SENTINEL;
        b=1;
    }
    حد_بندي=((stateProps[state]&7)+1)<<5;
    طالما(b<حد_بندي) {
        int32_t داخلا_2=صف[b];
        إذا(MBCS_داخلا_IS_TRANSITION(داخلا_2)) {
            int32_t ايندڙ_State=MBCS_داخلا_TRANSITION_STATE(داخلا_2);
            إذا(stateProps[ايندڙ_State]>=0) {
                /* recurse to a state with non-ignorable actions */
                إذا(!enum_منزل_U(
                        mbcs_جدول, stateProps, ايندڙ_State,
                        وٿي+MBCS_داخلا_TRANSITION_وٿي(داخلا_2),
                        value | static_cast<uint32_t>(b),
                        callback, context,
                        p_Error_ڪوڊ)) {
                    أرجع خطأ;
                }
            }
            ڪوڊ_Points[b&0x1f]=U_SENTINEL;
        } وإلا {
            UChar32 c;
            int32_t action;

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=MBCS_داخلا_FINAL_ACTION(داخلا_2);
            إذا(action==MBCS_STATE_VALID_سڌو_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            } وإلا إذا(action==MBCS_STATE_VALID_16) {
                int32_t final_وٿي=وٿي+MBCS_داخلا_FINAL_قدر_16(داخلا_2);
                c=unicode_ڪوڊ_Units[final_وٿي];
                إذا(c<0xfffe) {
                    /* output BMP code point */
                } وإلا {
                    c=U_SENTINEL;
                }
            } وإلا إذا(action==MBCS_STATE_VALID_16_PAIR) {
                int32_t final_وٿي=وٿي+MBCS_داخلا_FINAL_قدر_16(داخلا_2);
                c=unicode_ڪوڊ_Units[final_وٿي++];
                إذا(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } وإلا إذا(c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_ڪوڊ_Units[final_وٿي]+(0x10000-0xdc00);
                } وإلا إذا(c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_ڪوڊ_Units[final_وٿي];
                } وإلا {
                    c=U_SENTINEL;
                }
            } وإلا إذا(action==MBCS_STATE_VALID_سڌو_20) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_داخلا_FINAL_قدر(داخلا_2) + 0x10000);
            } وإلا {
                c=U_SENTINEL;
            }

            ڪوڊ_Points[b&0x1f]=c;
            any_ڪوڊ_Points&=c;
        }
        إذا(((++b)&0x1f)==0) {
            إذا(any_ڪوڊ_Points>=0) {
                إذا (!callback(context, value | static_cast<uint32_t>(b - 0x20), ڪوڊ_Points)) {
                    أرجع خطأ;
                }
                any_ڪوڊ_Points=-1;
            }
        }
    }
    أرجع سچ;
}

/*
 * Only called if stateProps[state]==-1.
 * A recursive call may do stateProps[state]|=0x40 if this state is the target of an
 * MBCS_STATE_CHANGE_ONLY.
 */
ساكن int8_t
getStateProp(ثابت int32_t (*state_جدول)[256], int8_t stateProps[], صحيح state) {
    ثابت int32_t *صف;
    int32_t min, max, داخلا_2, ايندڙ_State;

    صف=state_جدول[state];
    stateProps[state]=0;

    /* find first non-ignorable state */
    لكل(min=0;; ++min) {
        داخلا_2=صف[min];
        ايندڙ_State=MBCS_داخلا_STATE(داخلا_2);
        إذا(stateProps[ايندڙ_State]==-1) {
            getStateProp(state_جدول, stateProps, ايندڙ_State);
        }
        إذا(MBCS_داخلا_IS_TRANSITION(داخلا_2)) {
            إذا(stateProps[ايندڙ_State]>=0) {
                اكسر;
            }
        } وإلا إذا(MBCS_داخلا_FINAL_ACTION(داخلا_2)<MBCS_STATE_UNASSIGNED) {
            اكسر;
        }
        إذا(min==0xff) {
            stateProps[state]=-0x40;  /* (int8_t)0xc0 */
            أرجع stateProps[state];
        }
    }
    stateProps[state] |= static_cast<int8_t>((min >> 5) << 3);

    /* find last non-ignorable state */
    لكل(max=0xff; min<max; --max) {
        داخلا_2=صف[max];
        ايندڙ_State=MBCS_داخلا_STATE(داخلا_2);
        إذا(stateProps[ايندڙ_State]==-1) {
            getStateProp(state_جدول, stateProps, ايندڙ_State);
        }
        إذا(MBCS_داخلا_IS_TRANSITION(داخلا_2)) {
            إذا(stateProps[ايندڙ_State]>=0) {
                اكسر;
            }
        } وإلا إذا(MBCS_داخلا_FINAL_ACTION(داخلا_2)<MBCS_STATE_UNASSIGNED) {
            اكسر;
        }
    }
    stateProps[state] |= static_cast<int8_t>(max >> 5);

    /* recurse further and collect direct-state information */
    طالما(min<=max) {
        داخلا_2=صف[min];
        ايندڙ_State=MBCS_داخلا_STATE(داخلا_2);
        إذا(stateProps[ايندڙ_State]==-1) {
            getStateProp(state_جدول, stateProps, ايندڙ_State);
        }
        إذا(MBCS_داخلا_IS_FINAL(داخلا_2)) {
            stateProps[ايندڙ_State]|=0x40;
            إذا(MBCS_داخلا_FINAL_ACTION(داخلا_2)<=MBCS_STATE_FALLBACK_سڌو_20) {
                stateProps[state]|=0x40;
            }
        }
        ++min;
    }
    أرجع stateProps[state];
}

/*
 * Internal function enumerating the toUnicode data of an MBCS converter.
 * Currently only used for reconstituting data for a MBCS_OPT_NO_FROM_U
 * table, but could also be used for a future ucnv_getUnicodeSet() option
 * that includes reverse fallbacks (after updating this function's implementation).
 * Currently only handles roundtrip mappings.
 * Does not currently handle extensions.
 */
ساكن فراغ
ucnv_MBCSEnum_منزل_Unicode(UConverterMBCSTable *mbcs_جدول,
                       UConverter_Enum_منزل_UCallback *callback, ثابت فراغ *context,
                       UError_ڪوڊ *p_Error_ڪوڊ) {
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
    int8_t stateProps[MBCS_MAX_STATE_ڳڻپ];
    int32_t state;

    uprv_memset(stateProps, -1, حجم(stateProps));

    /* recurse from state 0 and set all stateProps */
    getStateProp(mbcs_جدول->state_جدول, stateProps, 0);

    لكل(state=0; state<mbcs_جدول->ڳڻپ_حالتون; ++state) {
        /*if(stateProps[state]==-1) {
            printf("unused/unreachable <icu:state> %d\n", state);
        }*/
        إذا(stateProps[state]>=0x40) {
            /* start from each direct state */
            enum_منزل_U(
                mbcs_جدول, stateProps, state, 0, 0,
                callback, context,
                p_Error_ڪوڊ);
        }
    }
}

U_CFUNC فراغ 
ucnv_MBCSGet_Filtered_Unicode_سيٽ_For_Unicode(ثابت UConverter_Shared_معلومات *shared_معلومات,
                                         ثابت USetAdder *sa,
                                         UConverter_Unicode_سيٽ which,
                                         UConverter_سيٽ_Filter filter,
                                         UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت UConverterMBCSTable *mbcs_جدول;
    ثابت uint16_t *جدول;

    uint32_t st3;
    uint16_t st1, maxStage1, st2;

    UChar32 c;

    /* enumerate the from-Unicode trie table */
    mbcs_جدول=&shared_معلومات->mbcs;
    جدول=mbcs_جدول->from_Unicode_جدول;
    إذا(mbcs_جدول->unicode_نقاب&UCNV_HAS_SUPPLEMENTARY) {
        maxStage1=0x440;
    } وإلا {
        maxStage1=0x40;
    }

    c=0; /* keep track of the current code point while enumerating */

    إذا(mbcs_جدول->outputType==MBCS_OUTPUT_1) {
        ثابت uint16_t *stage2, *stage3, *results;
        uint16_t min_قدر;

        results=(ثابت uint16_t *)mbcs_جدول->from_Unicode_بائيٽون;

        /*
         * Set a threshold variable for selecting which mappings to use.
         * See ucnv_MBCSSingleFromBMPWithOffsets() and
         * MBCS_SINGLE_RESULT_FROM_U() for details.
         */
        إذا(which==UCNV_ROUNDTRIP_سيٽ) {
            /* use only roundtrips */
            min_قدر=0xf00;
        } وإلا /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
            /* use all roundtrip and fallback results */
            min_قدر=0x800;
        }

        لكل(st1=0; st1<maxStage1; ++st1) {
            st2=جدول[st1];
            إذا(st2>maxStage1) {
                stage2=جدول+st2;
                لكل(st2=0; st2<64; ++st2) {
                    إذا((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=results+st3;

                        افعل {
                            إذا(*stage3++>=min_قدر) {
                                sa->جوڙيو(sa->سيٽ, c);
                            }
                        } طالما((++c&0xf)!=0);
                    } وإلا {
                        c+=16; /* empty stage 3 block */
                    }
                }
            } وإلا {
                c+=1024; /* empty stage 2 block */
            }
        }
    } وإلا {
        ثابت uint32_t *stage2;
        ثابت uint8_t *stage3, *بائيٽون;
        uint32_t st3Multiplier;
        uint32_t value;
        UBool useFallback;

        بائيٽون=mbcs_جدول->from_Unicode_بائيٽون;

        useFallback = which == UCNV_ROUNDTRIP_AND_FALLBACK_سيٽ;

        بدل(mbcs_جدول->outputType) {
        حالة MBCS_OUTPUT_3:
        حالة MBCS_OUTPUT_4_EUC:
            st3Multiplier=3;
            اكسر;
        حالة MBCS_OUTPUT_4:
            st3Multiplier=4;
            اكسر;
        افتراضي:
            st3Multiplier=2;
            اكسر;
        }

        لكل(st1=0; st1<maxStage1; ++st1) {
            st2=جدول[st1];
            إذا(st2>(maxStage1>>1)) {
                stage2=(ثابت uint32_t *)جدول+st2;
                لكل(st2=0; st2<64; ++st2) {
                    إذا((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=بائيٽون+st3Multiplier*16*(uint32_t)(uint16_t)st3;

                        /* get the roundtrip flags for the stage 3 block */
                        st3>>=16;

                        /*
                         * Add code points for which the roundtrip flag is set,
                         * or which map to non-zero bytes if we use fallbacks.
                         * See ucnv_MBCSFromUnicodeWithOffsets() for details.
                         */
                        بدل(filter) {
                        حالة UCNV_سيٽ_FILTER_NONE:
                            افعل {
                                إذا(st3&1) {
                                    sa->جوڙيو(sa->سيٽ, c);
                                    stage3+=st3Multiplier;
                                } وإلا إذا(useFallback) {
                                    uint8_t b=0;
                                    بدل(st3Multiplier) {
                                    حالة 4:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    حالة 3:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    حالة 2:
                                        b|=stage3[0]|stage3[1];
                                        stage3+=2;
                                        U_FALLTHROUGH;
                                    افتراضي:
                                        اكسر;
                                    }
                                    إذا(b!=0) {
                                        sa->جوڙيو(sa->سيٽ, c);
                                    }
                                }
                                st3>>=1;
                            } طالما((++c&0xf)!=0);
                            اكسر;
                        حالة UCNV_سيٽ_FILTER_DBCS_ONLY:
                             /* Ignore single-byte results (<0x100). */
                            افعل {
                                إذا(((st3&1)!=0 || useFallback) && *((ثابت uint16_t *)stage3)>=0x100) {
                                    sa->جوڙيو(sa->سيٽ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } طالما((++c&0xf)!=0);
                            اكسر;
                        حالة UCNV_سيٽ_FILTER_2022_CN:
                             /* Only add code points that map to CNS 11643 planes 1 & 2 for non-EXT ISO-2022-CN. */
                            افعل {
                                إذا(((st3&1)!=0 || useFallback) && ((value=*stage3)==0x81 || value==0x82)) {
                                    sa->جوڙيو(sa->سيٽ, c);
                                }
                                st3>>=1;
                                stage3+=3;  /* +=st3Multiplier */
                            } طالما((++c&0xf)!=0);
                            اكسر;
                        حالة UCNV_سيٽ_FILTER_SJIS:
                             /* Only add code points that map to Shift-JIS codes corresponding to JIS X 0208. */
                            افعل {
                                إذا(((st3&1)!=0 || useFallback) && (value=*((ثابت uint16_t *)stage3))>=0x8140 && value<=0xeffc) {
                                    sa->جوڙيو(sa->سيٽ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } طالما((++c&0xf)!=0);
                            اكسر;
                        حالة UCNV_سيٽ_FILTER_GR94_DBCS:
                            /* Only add code points that map to ISO 2022 GR 94 DBCS codes (each byte A1..FE). */
                            افعل {
                                إذا( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((ثابت uint16_t *)stage3)) - 0xa1a1)<=(0xfefe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->جوڙيو(sa->سيٽ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } طالما((++c&0xf)!=0);
                            اكسر;
                        حالة UCNV_سيٽ_FILTER_HZ:
                            /* Only add code points that are suitable for HZ DBCS (lead byte A1..FD). */
                            افعل {
                                إذا( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((ثابت uint16_t *)stage3))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->جوڙيو(sa->سيٽ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } طالما((++c&0xf)!=0);
                            اكسر;
                        افتراضي:
                            *p_Error_ڪوڊ=U_اندروني_پروگرام_ERROR;
                            أرجع;
                        }
                    } وإلا {
                        c+=16; /* empty stage 3 block */
                    }
                }
            } وإلا {
                c+=1024; /* empty stage 2 block */
            }
        }
    }

    ucnv_ext_Get_Unicode_سيٽ(shared_معلومات, sa, which, filter, p_Error_ڪوڊ);
}

U_CFUNC فراغ
ucnv_MBCSGet_Unicode_سيٽ_For_Unicode(ثابت UConverter_Shared_معلومات *shared_معلومات,
                                 ثابت USetAdder *sa,
                                 UConverter_Unicode_سيٽ which,
                                 UError_ڪوڊ *p_Error_ڪوڊ) {
    ucnv_MBCSGet_Filtered_Unicode_سيٽ_For_Unicode(
        shared_معلومات, sa, which,
        shared_معلومات->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ?
            UCNV_سيٽ_FILTER_DBCS_ONLY :
            UCNV_سيٽ_FILTER_NONE,
        p_Error_ڪوڊ);
}

ساكن فراغ U_CALLCONV
ucnv_MBCSGet_Unicode_سيٽ(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverter_Unicode_سيٽ which,
                   UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(cnv->options&_MBCS_OPTION_GB18030) {
        sa->جوڙيو_Range(sa->سيٽ, 0, 0xd7ff);
        sa->جوڙيو_Range(sa->سيٽ, 0xe000, 0x10ffff);
    } وإلا {
        ucnv_MBCSGet_Unicode_سيٽ_For_Unicode(cnv->shared_معلومات, sa, which, p_Error_ڪوڊ);
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
ساكن UChar32
_extFromU(UConverter *cnv, ثابت UConverter_Shared_معلومات *shared_معلومات,
          UChar32 cp,
          ثابت char16_t **ذريعو, ثابت char16_t *ذريعو_حد_بندي,
          uint8_t **هدف, ثابت uint8_t *هدف_حد_بندي,
          int32_t **offsets, int32_t ذريعو_اشاريو,
          UBool flush,
          UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت int32_t *cx;

    cnv->useSubChar1=خطأ;

    إذا( (cx=shared_معلومات->mbcs.extIndexes)!=خالي_اشارو &&
        ucnv_ext_Initial_ميلاپ_From_U(
            cnv, cx,
            cp, ذريعو, ذريعو_حد_بندي,
            reinterpret_cast<حرف**>(هدف), reinterpret_cast<ثابت حرف*>(هدف_حد_بندي),
            offsets, ذريعو_اشاريو,
            flush,
            p_Error_ڪوڊ)
    ) {
        أرجع 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    إذا((cnv->options&_MBCS_OPTION_GB18030)!=0) {
        ثابت uint32_t *range;
        int32_t i;

        range=gb18030Ranges[0];
        لكل(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            إذا (range[0] <= static_cast<uint32_t>(cp) && static_cast<uint32_t>(cp) <= range[1]) {
                /* found the Unicode code point, output the four-byte sequence for it */
                uint32_t لڪيري_2;
                حرف بائيٽون[4];

                /* get the linear value of the first GB 18030 code in this range */
                لڪيري_2=range[2]-لڪيري_18030_بنياد;

                /* add the offset from the beginning of the range */
                لڪيري_2 += (static_cast<uint32_t>(cp) - range[0]);

                /* turn this into a four-byte sequence */
                بائيٽون[3] = static_cast<حرف>(0x30 + لڪيري_2 % 10); لڪيري_2 /= 10;
                بائيٽون[2] = static_cast<حرف>(0x81 + لڪيري_2 % 126); لڪيري_2 /= 126;
                بائيٽون[1] = static_cast<حرف>(0x30 + لڪيري_2 % 10); لڪيري_2 /= 10;
                بائيٽون[0] = static_cast<حرف>(0x81 + لڪيري_2);

                /* output this sequence */
                ucnv_from_UWrite_بائيٽون(cnv,
                                     بائيٽون, 4, reinterpret_cast<حرف**>(هدف), reinterpret_cast<ثابت حرف*>(هدف_حد_بندي),
                                     offsets, ذريعو_اشاريو, p_Error_ڪوڊ);
                أرجع 0;
            }
        }
    }

    /* no mapping */
    *p_Error_ڪوڊ=U_INVALID_CHAR_مليو;
    أرجع cp;
}

/*
 * Input sequence: cnv->toUBytes[0..length[
 * @return if(U_FAILURE) return the length (toULength, byteIndex) for the input
 *         else return 0 after output has been written to the target
 */
ساكن int8_t
ext_منزل_U(UConverter *cnv, ثابت UConverter_Shared_معلومات *shared_معلومات,
        int8_t length,
        ثابت uint8_t **ذريعو, ثابت uint8_t *ذريعو_حد_بندي,
        char16_t **هدف, ثابت char16_t *هدف_حد_بندي,
        int32_t **offsets, int32_t ذريعو_اشاريو,
        UBool flush,
        UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت int32_t *cx;

    إذا( (cx=shared_معلومات->mbcs.extIndexes)!=خالي_اشارو &&
        ucnv_ext_Initial_ميلاپ_منزل_U(
            cnv, cx,
            length, reinterpret_cast<ثابت حرف**>(ذريعو), reinterpret_cast<ثابت حرف*>(ذريعو_حد_بندي),
            هدف, هدف_حد_بندي,
            offsets, ذريعو_اشاريو,
            flush,
            p_Error_ڪوڊ)
    ) {
        أرجع 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    إذا(length==4 && (cnv->options&_MBCS_OPTION_GB18030)!=0) {
        ثابت uint32_t *range;
        uint32_t لڪيري_2;
        int32_t i;

        لڪيري_2=لڪيري_18030(cnv->منزل_UBytes[0], cnv->منزل_UBytes[1], cnv->منزل_UBytes[2], cnv->منزل_UBytes[3]);
        range=gb18030Ranges[0];
        لكل(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            إذا(range[2]<=لڪيري_2 && لڪيري_2<=range[3]) {
                /* found the sequence, output the Unicode code point for it */
                *p_Error_ڪوڊ=U_ZERO_ERROR;

                /* add the linear difference between the input and start sequences to the start code point */
                لڪيري_2=range[0]+(لڪيري_2-range[2]);

                /* output this code point */
                ucnv_منزل_UWrite_ڪوڊ_نقطو(cnv, لڪيري_2, هدف, هدف_حد_بندي, offsets, ذريعو_اشاريو, p_Error_ڪوڊ);

                أرجع 0;
            }
        }
    }

    /* no mapping */
    *p_Error_ڪوڊ=U_INVALID_CHAR_مليو;
    أرجع length;
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
#وصف_ڪريو EBCDIC_LF 0x25
#وصف_ڪريو EBCDIC_NL 0x15

/* standard EBCDIC codes with roundtrip flag as stored in Unicode-to-single-byte tables */
#وصف_ڪريو EBCDIC_RT_LF 0xf25
#وصف_ڪريو EBCDIC_RT_NL 0xf15

/* Unicode code points */
#وصف_ڪريو U_LF 0x0a
#وصف_ڪريو U_NL 0x85

ساكن UBool
_EBCDICSwapLFNL(UConverter_Shared_معلومات *shared_معلومات, UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverterMBCSTable *mbcs_جدول;

    ثابت uint16_t *جدول, *results;
    ثابت uint8_t *بائيٽون;

    int32_t (*new_State_جدول)[256];
    uint16_t *newResults;
    uint8_t *p;
    حرف *name;

    uint32_t stage2_داخلا;
    uint32_t size, sizeofFromUBytes;

    mbcs_جدول=&shared_معلومات->mbcs;

    جدول=mbcs_جدول->from_Unicode_جدول;
    بائيٽون=mbcs_جدول->from_Unicode_بائيٽون;
    results = reinterpret_cast<ثابت uint16_t*>(بائيٽون);

    /*
     * Check that this is an EBCDIC table with SBCS portion -
     * SBCS or EBCDIC_STATEFUL with standard EBCDIC LF and NL mappings.
     *
     * If not, ignore the option. Options are always ignored if they do not apply.
     */
    إذا(!(
         (mbcs_جدول->outputType==MBCS_OUTPUT_1 || mbcs_جدول->outputType==MBCS_OUTPUT_2_SISO) &&
         mbcs_جدول->state_جدول[0][EBCDIC_LF]==MBCS_داخلا_FINAL(0, MBCS_STATE_VALID_سڌو_16, U_LF) &&
         mbcs_جدول->state_جدول[0][EBCDIC_NL]==MBCS_داخلا_FINAL(0, MBCS_STATE_VALID_سڌو_16, U_NL)
    )) {
        أرجع خطأ;
    }

    إذا(mbcs_جدول->outputType==MBCS_OUTPUT_1) {
        إذا(!(
             EBCDIC_RT_LF==MBCS_SINGLE_نتيجو_FROM_U(جدول, results, U_LF) &&
             EBCDIC_RT_NL==MBCS_SINGLE_نتيجو_FROM_U(جدول, results, U_NL)
        )) {
            أرجع خطأ;
        }
    } وإلا /* MBCS_OUTPUT_2_SISO */ {
        stage2_داخلا=MBCS_STAGE_2_FROM_U(جدول, U_LF);
        إذا(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_داخلا, U_LF)!=0 &&
             EBCDIC_LF==MBCS_قدر_2_FROM_STAGE_2(بائيٽون, stage2_داخلا, U_LF)
        )) {
            أرجع خطأ;
        }

        stage2_داخلا=MBCS_STAGE_2_FROM_U(جدول, U_NL);
        إذا(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_داخلا, U_NL)!=0 &&
             EBCDIC_NL==MBCS_قدر_2_FROM_STAGE_2(بائيٽون, stage2_داخلا, U_NL)
        )) {
            أرجع خطأ;
        }
    }

    إذا(mbcs_جدول->from_UBytes_ڊيگهائي>0) {
        /*
         * We _know_ the number of bytes in the fromUnicodeBytes array
         * starting with header.version 4.1.
         */
        sizeofFromUBytes=mbcs_جدول->from_UBytes_ڊيگهائي;
    } وإلا {
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
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع خطأ;
    }

    /*
     * The table has an appropriate format.
     * Allocate and build
     * - a modified to-Unicode state table
     * - a modified from-Unicode output array
     * - a converter name string with the swap option appended
     */
    size=
        mbcs_جدول->ڳڻپ_حالتون*1024+
        sizeofFromUBytes+
        UCNV_MAX_CONVERTER_NAME_ڊيگهائي+20;
    p = static_cast<uint8_t*>(uprv_malloc(size));
    إذا(p==خالي_اشارو) {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خطأ;
    }

    /* copy and modify the to-Unicode state table */
    new_State_جدول = reinterpret_cast<int32_t(*)[256]>(p);
    uprv_memcpy(new_State_جدول, mbcs_جدول->state_جدول, mbcs_جدول->ڳڻپ_حالتون*1024);

    new_State_جدول[0][EBCDIC_LF]=MBCS_داخلا_FINAL(0, MBCS_STATE_VALID_سڌو_16, U_NL);
    new_State_جدول[0][EBCDIC_NL]=MBCS_داخلا_FINAL(0, MBCS_STATE_VALID_سڌو_16, U_LF);

    /* copy and modify the from-Unicode result table */
    newResults = reinterpret_cast<uint16_t*>(new_State_جدول[mbcs_جدول->ڳڻپ_حالتون]);
    uprv_memcpy(newResults, بائيٽون, sizeofFromUBytes);

    /* conveniently, the table access macros work on the left side of expressions */
    إذا(mbcs_جدول->outputType==MBCS_OUTPUT_1) {
        MBCS_SINGLE_نتيجو_FROM_U(جدول, newResults, U_LF)=EBCDIC_RT_NL;
        MBCS_SINGLE_نتيجو_FROM_U(جدول, newResults, U_NL)=EBCDIC_RT_LF;
    } وإلا /* MBCS_OUTPUT_2_SISO */ {
        stage2_داخلا=MBCS_STAGE_2_FROM_U(جدول, U_LF);
        MBCS_قدر_2_FROM_STAGE_2(newResults, stage2_داخلا, U_LF)=EBCDIC_NL;

        stage2_داخلا=MBCS_STAGE_2_FROM_U(جدول, U_NL);
        MBCS_قدر_2_FROM_STAGE_2(newResults, stage2_داخلا, U_NL)=EBCDIC_LF;
    }

    /* set the canonical converter name */
    name = reinterpret_cast<حرف*>(newResults) + sizeofFromUBytes;
    uprv_strcpy(name, shared_معلومات->static_معلومات->name);
    uprv_strcat(name, UCNV_مٽا_سٽا_LFNL_OPTION_STRING);

    /* set the pointers */
    icu::umtx_تالو(خالي_اشارو);
    إذا(mbcs_جدول->مٽا_سٽا_LFNLState_جدول==خالي_اشارو) {
        mbcs_جدول->مٽا_سٽا_LFNLState_جدول=new_State_جدول;
        mbcs_جدول->مٽا_سٽا_LFNLFrom_Unicode_بائيٽون = reinterpret_cast<uint8_t*>(newResults);
        mbcs_جدول->مٽا_سٽا_LFNLName=name;

        new_State_جدول=خالي_اشارو;
    }
    icu::umtx_unlock(خالي_اشارو);

    /* release the allocated memory if another thread beat us to it */
    إذا(new_State_جدول!=خالي_اشارو) {
        uprv_free(new_State_جدول);
    }
    أرجع سچ;
}

/* reconstitute omitted fromUnicode data ------------------------------------ */

/* for details, compare with genmbcs.c MBCSAddFromUnicode() and transformEUC() */
ساكن UBool U_CALLCONV
writeStage3Roundtrip(ثابت فراغ *context, uint32_t value, UChar32 ڪوڊ_Points[32]) {
    UConverterMBCSTable *mbcs_جدول=(UConverterMBCSTable *)context;
    ثابت uint16_t *جدول;
    uint32_t *stage2;
    uint8_t *بائيٽون, *p;
    UChar32 c;
    int32_t i, st3;

    جدول=mbcs_جدول->from_Unicode_جدول;
    بائيٽون = const_cast<uint8_t*>(mbcs_جدول->from_Unicode_بائيٽون);

    /* for EUC outputTypes, modify the value like genmbcs.c's transformEUC() */
    بدل(mbcs_جدول->outputType) {
    حالة MBCS_OUTPUT_3_EUC:
        إذا(value<=0xffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } وإلا إذا(value<=0x8effff) {
            /* code set 2 */
            value&=0x7fff;
        } وإلا /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7f;
        }
        اكسر;
    حالة MBCS_OUTPUT_4_EUC:
        إذا(value<=0xffffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } وإلا إذا(value<=0x8effffff) {
            /* code set 2 */
            value&=0x7fffff;
        } وإلا /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7fff;
        }
        اكسر;
    افتراضي:
        اكسر;
    }

    لكل(i=0; i<=0x1f; ++value, ++i) {
        c=ڪوڊ_Points[i];
        إذا(c<0) {
            استمر;
        }

        /* locate the stage 2 & 3 data */
        stage2=((uint32_t *)جدول)+جدول[c>>10]+((c>>4)&0x3f);
        p=بائيٽون;
        st3 = static_cast<int32_t>(static_cast<uint16_t>(*stage2)) * 16 + (c & 0xf);

        /* write the codepage bytes into stage 3 */
        بدل(mbcs_جدول->outputType) {
        حالة MBCS_OUTPUT_3:
        حالة MBCS_OUTPUT_4_EUC:
            p+=st3*3;
            p[0] = static_cast<uint8_t>(value >> 16);
            p[1] = static_cast<uint8_t>(value >> 8);
            p[2] = static_cast<uint8_t>(value);
            اكسر;
        حالة MBCS_OUTPUT_4:
            reinterpret_cast<uint32_t*>(p)[st3] = value;
            اكسر;
        افتراضي:
            /* 2 bytes per character */
            reinterpret_cast<uint16_t*>(p)[st3] = static_cast<uint16_t>(value);
            اكسر;
        }

        /* set the roundtrip flag */
        *stage2|=(1UL<<(16+(c&0xf)));
    }
    أرجع سچ;
 }

ساكن فراغ
reconstitute_معلومات(UConverterMBCSTable *mbcs_جدول,
                 uint32_t stage1_ڊيگهائي, uint32_t stage2_ڊيگهائي,
                 uint32_t ڀريل_Stage2_ڊيگهائي,  /* lengths are numbers of units, not bytes */
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    uint16_t *stage1;
    uint32_t *stage2;
    uint32_t معلومات_ڊيگهائي=stage1_ڊيگهائي*2+ڀريل_Stage2_ڊيگهائي*4+mbcs_جدول->from_UBytes_ڊيگهائي;
    mbcs_جدول->reconstituted_معلومات = static_cast<uint8_t*>(uprv_malloc(معلومات_ڊيگهائي));
    إذا(mbcs_جدول->reconstituted_معلومات==خالي_اشارو) {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    uprv_memset(mbcs_جدول->reconstituted_معلومات, 0, معلومات_ڊيگهائي);

    /* copy existing data and reroute the pointers */
    stage1 = reinterpret_cast<uint16_t*>(mbcs_جدول->reconstituted_معلومات);
    uprv_memcpy(stage1, mbcs_جدول->from_Unicode_جدول, stage1_ڊيگهائي*2);

    stage2 = reinterpret_cast<uint32_t*>(stage1 + stage1_ڊيگهائي);
    uprv_memcpy(stage2+(ڀريل_Stage2_ڊيگهائي-stage2_ڊيگهائي),
                mbcs_جدول->from_Unicode_جدول+stage1_ڊيگهائي,
                stage2_ڊيگهائي*4);

    mbcs_جدول->from_Unicode_جدول=stage1;
    mbcs_جدول->from_Unicode_بائيٽون = reinterpret_cast<uint8_t*>(stage2 + ڀريل_Stage2_ڊيگهائي);

    /* indexes into stage 2 count from the bottom of the fromUnicodeTable */
    stage2 = reinterpret_cast<uint32_t*>(stage1);

    /* reconstitute the initial part of stage 2 from the mbcsIndex */
    {
        int32_t stage_UTF8_ڊيگهائي = (static_cast<int32_t>(mbcs_جدول->maxFastUChar) + 1) >> 6;
        int32_t stage_UTF8_اشاريو=0;
        int32_t st1, st2, st3, i;

        لكل(st1=0; stage_UTF8_اشاريو<stage_UTF8_ڊيگهائي; ++st1) {
            st2=stage1[st1];
            إذا (st2 != static_cast<int32_t>(stage1_ڊيگهائي) / 2) {
                /* each stage 2 block has 64 entries corresponding to 16 entries in the mbcsIndex */
                لكل(i=0; i<16; ++i) {
                    st3=mbcs_جدول->mbcs_اشاريو[stage_UTF8_اشاريو++];
                    إذا(st3!=0) {
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
                    } وإلا {
                        /* no stage 3 block, skip */
                        st2+=4;
                    }
                }
            } وإلا {
                /* no stage 2 block, skip */
                stage_UTF8_اشاريو+=16;
            }
        }
    }

    /* reconstitute fromUnicodeBytes with roundtrips from toUnicode data */
    ucnv_MBCSEnum_منزل_Unicode(mbcs_جدول, writeStage3Roundtrip, mbcs_جدول, p_Error_ڪوڊ);
}

/* MBCS setup functions ----------------------------------------------------- */

ساكن فراغ U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_معلومات *shared_معلومات,
          UConverter_ڀريو_دليلون *p_دليلون,
          ثابت uint8_t *raw,
          UError_ڪوڊ *p_Error_ڪوڊ) {
    UDataInfo info;
    UConverterMBCSTable *mbcs_جدول=&shared_معلومات->mbcs;
    _MBCSHeader *سرو=(_MBCSHeader *)raw;
    uint32_t وٿي;
    uint32_t سرو_ڊيگهائي;
    UBool noFromU=خطأ;

    إذا(سرو->نسخو[0]==4) {
        سرو_ڊيگهائي=MBCS_سرو_V4_ڊيگهائي;
    } وإلا إذا(سرو->نسخو[0]==5 && سرو->نسخو[1]>=3 &&
              (سرو->options&MBCS_OPT_UNKNOWN_INCOMPATIBLE_نقاب)==0) {
        سرو_ڊيگهائي=سرو->options&MBCS_OPT_ڊيگهائي_نقاب;
        noFromU = static_cast<UBool>((سرو->options & MBCS_OPT_NO_FROM_U) != 0);
    } وإلا {
        *p_Error_ڪوڊ=U_INVALID_جدول_FORMAT;
        أرجع;
    }

    mbcs_جدول->outputType = static_cast<uint8_t>(سرو->flags);
    إذا(noFromU && mbcs_جدول->outputType==MBCS_OUTPUT_1) {
        *p_Error_ڪوڊ=U_INVALID_جدول_FORMAT;
        أرجع;
    }

    /* extension data, header version 4.2 and higher */
    وٿي=سرو->flags>>8;
    إذا(وٿي!=0) {
        mbcs_جدول->extIndexes = reinterpret_cast<ثابت int32_t*>(raw + وٿي);
    }

    إذا(mbcs_جدول->outputType==MBCS_OUTPUT_EXT_ONLY) {
        UConverter_ڀريو_دليلون دليلون_2=UCNV_ڀريو_دليلون_INITIALIZER;
        UConverter_Shared_معلومات *بنياد_Shared_معلومات;
        ثابت int32_t *extIndexes;
        ثابت حرف *بنياد_Name;

        /* extension-only file, load the base table and set values appropriately */
        إذا((extIndexes=mbcs_جدول->extIndexes)==خالي_اشارو) {
            /* extension-only file without extension */
            *p_Error_ڪوڊ=U_INVALID_جدول_FORMAT;
            أرجع;
        }

        إذا(p_دليلون->nestedLoads!=1) {
            /* an extension table must not be loaded as a base table */
            *p_Error_ڪوڊ=U_INVALID_جدول_FILE;
            أرجع;
        }

        /* load the base table */
        بنياد_Name = reinterpret_cast<ثابت حرف*>(سرو) + سرو_ڊيگهائي * 4;
        إذا(0==uprv_strcmp(بنياد_Name, shared_معلومات->static_معلومات->name)) {
            /* forbid loading this same extension-only file */
            *p_Error_ڪوڊ=U_INVALID_جدول_FORMAT;
            أرجع;
        }

        /* TODO parse package name out of the prefix of the base name in the extension .cnv file? */
        دليلون_2.size=حجم(UConverter_ڀريو_دليلون);
        دليلون_2.nestedLoads=2;
        دليلون_2.onlyTestIsLoadable=p_دليلون->onlyTestIsLoadable;
        دليلون_2.reserved=p_دليلون->reserved;
        دليلون_2.options=p_دليلون->options;
        دليلون_2.pkg=p_دليلون->pkg;
        دليلون_2.name=بنياد_Name;
        بنياد_Shared_معلومات=ucnv_ڀريو(&دليلون_2, p_Error_ڪوڊ);
        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            أرجع;
        }
        إذا( بنياد_Shared_معلومات->static_معلومات->conversionType!=UCNV_MBCS ||
            بنياد_Shared_معلومات->mbcs.بنياد_Shared_معلومات!=خالي_اشارو
        ) {
            ucnv_unload(بنياد_Shared_معلومات);
            *p_Error_ڪوڊ=U_INVALID_جدول_FORMAT;
            أرجع;
        }
        إذا(p_دليلون->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            ucnv_unload(بنياد_Shared_معلومات);
            أرجع;
        }

        /* copy the base table data */
        uprv_memcpy(mbcs_جدول, &بنياد_Shared_معلومات->mbcs, حجم(UConverterMBCSTable));

        /* overwrite values with relevant ones for the extension converter */
        mbcs_جدول->بنياد_Shared_معلومات=بنياد_Shared_معلومات;
        mbcs_جدول->extIndexes=extIndexes;

        /*
         * It would be possible to share the swapLFNL data with a base converter,
         * but the generated name would have to be different, and the memory
         * would have to be free'd only once.
         * It is easier to just create the data for the extension converter
         * separately when it is requested.
         */
        mbcs_جدول->مٽا_سٽا_LFNLState_جدول=خالي_اشارو;
        mbcs_جدول->مٽا_سٽا_LFNLFrom_Unicode_بائيٽون=خالي_اشارو;
        mbcs_جدول->مٽا_سٽا_LFNLName=خالي_اشارو;

        /*
         * The reconstitutedData must be deleted only when the base converter
         * is unloaded.
         */
        mbcs_جدول->reconstituted_معلومات=خالي_اشارو;

        /*
         * Set a special, runtime-only outputType if the extension converter
         * is a DBCS version of a base converter that also maps single bytes.
         */
        إذا( shared_معلومات->static_معلومات->conversionType==UCNV_DBCS ||
                (shared_معلومات->static_معلومات->conversionType==UCNV_MBCS &&
                 shared_معلومات->static_معلومات->min_بائيٽون_في_Char>=2)
        ) {
            إذا(بنياد_Shared_معلومات->mbcs.outputType==MBCS_OUTPUT_2_SISO) {
                /* the base converter is SI/SO-stateful */
                int32_t داخلا_2;

                /* get the dbcs state from the state table entry for SO=0x0e */
                داخلا_2=mbcs_جدول->state_جدول[0][0xe];
                إذا( MBCS_داخلا_IS_FINAL(داخلا_2) &&
                    MBCS_داخلا_FINAL_ACTION(داخلا_2)==MBCS_STATE_CHANGE_ONLY &&
                    MBCS_داخلا_FINAL_STATE(داخلا_2)!=0
                ) {
                    mbcs_جدول->dbcsOnlyState = static_cast<uint8_t>(MBCS_داخلا_FINAL_STATE(داخلا_2));

                    mbcs_جدول->outputType=MBCS_OUTPUT_DBCS_ONLY;
                }
            } وإلا إذا(
                بنياد_Shared_معلومات->static_معلومات->conversionType==UCNV_MBCS &&
                بنياد_Shared_معلومات->static_معلومات->min_بائيٽون_في_Char==1 &&
                بنياد_Shared_معلومات->static_معلومات->max_بائيٽون_في_Char==2 &&
                mbcs_جدول->ڳڻپ_حالتون<=127
            ) {
                /* non-stateful base converter, need to modify the state table */
                int32_t (*new_State_جدول)[256];
                int32_t *state;
                int32_t i, count;

                /* allocate a new state table and copy the base state table contents */
                count=mbcs_جدول->ڳڻپ_حالتون;
                new_State_جدول = static_cast<int32_t(*)[256]>(uprv_malloc((count + 1) * 1024));
                إذا(new_State_جدول==خالي_اشارو) {
                    ucnv_unload(بنياد_Shared_معلومات);
                    *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
                    أرجع;
                }

                uprv_memcpy(new_State_جدول, mbcs_جدول->state_جدول, count*1024);

                /* change all final single-byte entries to go to a new all-illegal state */
                state=new_State_جدول[0];
                لكل(i=0; i<256; ++i) {
                    إذا(MBCS_داخلا_IS_FINAL(state[i])) {
                        state[i]=MBCS_داخلا_TRANSITION(count, 0);
                    }
                }

                /* build the new all-illegal state */
                state=new_State_جدول[count];
                لكل(i=0; i<256; ++i) {
                    state[i]=MBCS_داخلا_FINAL(0, MBCS_STATE_ILLEGAL, 0);
                }
                mbcs_جدول->state_جدول=(ثابت int32_t (*)[256])new_State_جدول;
                mbcs_جدول->ڳڻپ_حالتون = static_cast<uint8_t>(count + 1);
                mbcs_جدول->state_جدول_Owned=سچ;

                mbcs_جدول->outputType=MBCS_OUTPUT_DBCS_ONLY;
            }
        }

        /*
         * unlike below for files with base tables, do not get the unicodeMask
         * from the sharedData; instead, use the base table's unicodeMask,
         * which we copied in the memcpy above;
         * this is necessary because the static data unicodeMask, especially
         * the UCNV_HAS_SUPPLEMENTARY flag, is part of the base table data
         */
    } وإلا {
        /* conversion file with a base table; an additional extension table is optional */
        /* make sure that the output type is known */
        بدل(mbcs_جدول->outputType) {
        حالة MBCS_OUTPUT_1:
        حالة MBCS_OUTPUT_2:
        حالة MBCS_OUTPUT_3:
        حالة MBCS_OUTPUT_4:
        حالة MBCS_OUTPUT_3_EUC:
        حالة MBCS_OUTPUT_4_EUC:
        حالة MBCS_OUTPUT_2_SISO:
            /* OK */
            اكسر;
        افتراضي:
            *p_Error_ڪوڊ=U_INVALID_جدول_FORMAT;
            أرجع;
        }
        إذا(p_دليلون->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            أرجع;
        }

        mbcs_جدول->ڳڻپ_حالتون = static_cast<uint8_t>(سرو->ڳڻپ_حالتون);
        mbcs_جدول->ڳڻپ_منزل_UFallbacks=سرو->ڳڻپ_منزل_UFallbacks;
        mbcs_جدول->state_جدول = reinterpret_cast<ثابت int32_t(*)[256]>(raw + سرو_ڊيگهائي * 4);
        mbcs_جدول->منزل_UFallbacks = reinterpret_cast<ثابت _MBCSToUFallback*>(mbcs_جدول->state_جدول + سرو->ڳڻپ_حالتون);
        mbcs_جدول->unicode_ڪوڊ_Units = reinterpret_cast<ثابت uint16_t*>(raw + سرو->وٿي_منزل_UCode_Units);

        mbcs_جدول->from_Unicode_جدول = reinterpret_cast<ثابت uint16_t*>(raw + سرو->وٿي_From_UTable);
        mbcs_جدول->from_Unicode_بائيٽون = raw + سرو->وٿي_From_UBytes;
        mbcs_جدول->from_UBytes_ڊيگهائي=سرو->from_UBytes_ڊيگهائي;

        /*
         * converter versions 6.1 and up contain a unicodeMask that is
         * used here to select the most efficient function implementations
         */
        info.size=حجم(UDataInfo);
        udata_getInfo((UData_يادگيري *)shared_معلومات->معلومات_يادگيري, &info);
        إذا(info.format_نسخو[0]>6 || (info.format_نسخو[0]==6 && info.format_نسخو[1]>=1)) {
            /* mask off possible future extensions to be safe */
            mbcs_جدول->unicode_نقاب = static_cast<uint8_t>(shared_معلومات->static_معلومات->unicode_نقاب & 3);
        } وإلا {
            /* for older versions, assume worst case: contains anything possible (prevent over-optimizations) */
            mbcs_جدول->unicode_نقاب=UCNV_HAS_SUPPLEMENTARY|UCNV_HAS_SURROGATES;
        }

        /*
         * _MBCSHeader.version 4.3 adds utf8Friendly data structures.
         * Check for the header version, SBCS vs. MBCS, and for whether the
         * data structures are optimized for code points as high as what the
         * runtime code is designed for.
         * The implementation does not handle mapping tables with entries for
         * unpaired surrogates.
         */
        إذا( سرو->نسخو[1]>=3 &&
            (mbcs_جدول->unicode_نقاب&UCNV_HAS_SURROGATES)==0 &&
            (mbcs_جدول->ڳڻپ_حالتون==1 ?
                (سرو->نسخو[2]>=(SBCS_FAST_MAX>>8)) :
                (سرو->نسخو[2]>=(MBCS_FAST_MAX>>8))
            )
        ) {
            mbcs_جدول->utf8Friendly=سچ;

            إذا(mbcs_جدول->ڳڻپ_حالتون==1) {
                /*
                 * SBCS: Stage 3 is allocated in 64-entry blocks for U+0000..SBCS_FAST_MAX or higher.
                 * Build a table with indexes to each block, to be used instead of
                 * the regular stage 1/2 table.
                 */
                int32_t i;
                لكل(i=0; i<(SBCS_FAST_حد_بندي>>6); ++i) {
                    mbcs_جدول->sbcs_اشاريو[i]=mbcs_جدول->from_Unicode_جدول[mbcs_جدول->from_Unicode_جدول[i>>4]+((i<<2)&0x3c)];
                }
                /* set SBCS_FAST_MAX to reflect the reach of sbcsIndex[] even if header->version[2]>(SBCS_FAST_MAX>>8) */
                mbcs_جدول->maxFastUChar=SBCS_FAST_MAX;
            } وإلا {
                /*
                 * MBCS: Stage 3 is allocated in 64-entry blocks for U+0000..MBCS_FAST_MAX or higher.
                 * The .cnv file is prebuilt with an additional stage table with indexes
                 * to each block.
                 */
                mbcs_جدول->mbcs_اشاريو = reinterpret_cast<ثابت uint16_t*>(
                    mbcs_جدول->from_Unicode_بائيٽون +
                     (noFromU ? 0 : mbcs_جدول->from_UBytes_ڊيگهائي));
                mbcs_جدول->maxFastUChar = (static_cast<char16_t>(سرو->نسخو[2]) << 8) | 0xff;
            }
        }

        /* calculate a bit set of 4 ASCII characters per bit that round-trip to ASCII bytes */
        {
            uint32_t asciiRoundtrips=0xffffffff;
            int32_t i;

            لكل(i=0; i<0x80; ++i) {
                إذا(mbcs_جدول->state_جدول[0][i]!=MBCS_داخلا_FINAL(0, MBCS_STATE_VALID_سڌو_16, i)) {
                    asciiRoundtrips &= ~(static_cast<uint32_t>(1) << (i >> 2));
                }
            }
            mbcs_جدول->asciiRoundtrips=asciiRoundtrips;
        }

        إذا(noFromU) {
            uint32_t stage1_ڊيگهائي=
                mbcs_جدول->unicode_نقاب&UCNV_HAS_SUPPLEMENTARY ?
                    0x440 : 0x40;
            uint32_t stage2_ڊيگهائي=
                (سرو->وٿي_From_UBytes-سرو->وٿي_From_UTable)/4-
                stage1_ڊيگهائي/2;
            reconstitute_معلومات(mbcs_جدول, stage1_ڊيگهائي, stage2_ڊيگهائي, سرو->ڀريل_Stage2_ڊيگهائي, p_Error_ڪوڊ);
        }
    }

    /* Set the impl pointer here so that it is set for both extension-only and base tables. */
    إذا(mbcs_جدول->utf8Friendly) {
        إذا(mbcs_جدول->ڳڻپ_حالتون==1) {
            shared_معلومات->impl=&_SBCSUTF8Impl;
        } وإلا {
            إذا(mbcs_جدول->outputType==MBCS_OUTPUT_2) {
                shared_معلومات->impl=&_DBCSUTF8Impl;
            }
        }
    }

    إذا(mbcs_جدول->outputType==MBCS_OUTPUT_DBCS_ONLY || mbcs_جدول->outputType==MBCS_OUTPUT_2_SISO) {
        /*
         * MBCS_OUTPUT_DBCS_ONLY: No SBCS mappings, therefore ASCII does not roundtrip.
         * MBCS_OUTPUT_2_SISO: Bypass the ASCII fastpath to handle prevLength correctly.
         */
        mbcs_جدول->asciiRoundtrips=0;
    }
}

ساكن فراغ U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_معلومات *shared_معلومات) {
    UConverterMBCSTable *mbcs_جدول=&shared_معلومات->mbcs;

    إذا(mbcs_جدول->مٽا_سٽا_LFNLState_جدول!=خالي_اشارو) {
        uprv_free(mbcs_جدول->مٽا_سٽا_LFNLState_جدول);
    }
    إذا(mbcs_جدول->state_جدول_Owned) {
        uprv_free((فراغ *)mbcs_جدول->state_جدول);
    }
    إذا(mbcs_جدول->بنياد_Shared_معلومات!=خالي_اشارو) {
        ucnv_unload(mbcs_جدول->بنياد_Shared_معلومات);
    }
    إذا(mbcs_جدول->reconstituted_معلومات!=خالي_اشارو) {
        uprv_free(mbcs_جدول->reconstituted_معلومات);
    }
}

ساكن فراغ U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_ڀريو_دليلون *p_دليلون,
              UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverterMBCSTable *mbcs_جدول;
    ثابت int32_t *extIndexes;
    uint8_t outputType;
    int8_t max_بائيٽون_في_UChar;

    إذا(p_دليلون->onlyTestIsLoadable) {
        أرجع;
    }

    mbcs_جدول=&cnv->shared_معلومات->mbcs;
    outputType=mbcs_جدول->outputType;

    إذا(outputType==MBCS_OUTPUT_DBCS_ONLY) {
        /* the swaplfnl option does not apply, remove it */
        cnv->options=p_دليلون->options&=~UCNV_OPTION_مٽا_سٽا_LFNL;
    }

    إذا((p_دليلون->options&UCNV_OPTION_مٽا_سٽا_LFNL)!=0) {
        /* do this because double-checked locking is broken */
        UBool isCached;

        icu::umtx_تالو(خالي_اشارو);
        isCached=mbcs_جدول->مٽا_سٽا_LFNLState_جدول!=خالي_اشارو;
        icu::umtx_unlock(خالي_اشارو);

        إذا(!isCached) {
            إذا(!_EBCDICSwapLFNL(cnv->shared_معلومات, p_Error_ڪوڊ)) {
                إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                    أرجع; /* something went wrong */
                }

                /* the option does not apply, remove it */
                cnv->options=p_دليلون->options&=~UCNV_OPTION_مٽا_سٽا_LFNL;
            }
        }
    }

    إذا(uprv_strstr(p_دليلون->name, "18030")!=خالي_اشارو) {
        إذا(uprv_strstr(p_دليلون->name, "gb18030")!=خالي_اشارو || uprv_strstr(p_دليلون->name, "GB18030")!=خالي_اشارو) {
            /* set a flag for GB 18030 mode, which changes the callback behavior */
            cnv->options|=_MBCS_OPTION_GB18030;
        }
    } وإلا إذا((uprv_strstr(p_دليلون->name, "KEIS")!=خالي_اشارو) || (uprv_strstr(p_دليلون->name, "keis")!=خالي_اشارو)) {
        /* set a flag for KEIS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_KEIS;
    } وإلا إذا((uprv_strstr(p_دليلون->name, "JEF")!=خالي_اشارو) || (uprv_strstr(p_دليلون->name, "jef")!=خالي_اشارو)) {
        /* set a flag for JEF converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JEF;
    } وإلا إذا((uprv_strstr(p_دليلون->name, "JIPS")!=خالي_اشارو) || (uprv_strstr(p_دليلون->name, "jips")!=خالي_اشارو)) {
        /* set a flag for JIPS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JIPS;
    }

    /* fix maxBytesPerUChar depending on outputType and options etc. */
    إذا(outputType==MBCS_OUTPUT_2_SISO) {
        cnv->max_بائيٽون_في_UChar=3; /* SO+DBCS */
    }

    extIndexes=mbcs_جدول->extIndexes;
    إذا(extIndexes!=خالي_اشارو) {
        max_بائيٽون_في_UChar = static_cast<int8_t>(UCNV_GET_MAX_بائيٽون_في_UCHAR(extIndexes));
        إذا(outputType==MBCS_OUTPUT_2_SISO) {
            ++max_بائيٽون_في_UChar; /* SO + multiple DBCS */
        }

        إذا(max_بائيٽون_في_UChar>cnv->max_بائيٽون_في_UChar) {
            cnv->max_بائيٽون_في_UChar=max_بائيٽون_في_UChar;
        }
    }

#إذا 0
    /*
     * documentation of UConverter fields used for status
     * all of these fields are (re)set to 0 by ucnv_bld.c and ucnv_reset()
     */

    /* toUnicode */
    cnv->منزل_Unicode_حالت=0;     /*وٿي*/
    cnv->گهڻائي=0;                /* state */
    cnv->منزل_ULength=0;           /* byteIndex */

    /* fromUnicode */
    cnv->fromUChar32=0;
    cnv->from_Unicode_حالت=1;   /* prevLength */
#جيڪڏهن_پورو
}

U_CDECL_BEGIN

ساكن ثابت حرف* U_CALLCONV
ucnv_MBCSGetName(ثابت UConverter *cnv) {
    إذا((cnv->options&UCNV_OPTION_مٽا_سٽا_LFNL)!=0 && cnv->shared_معلومات->mbcs.مٽا_سٽا_LFNLName!=خالي_اشارو) {
        أرجع cnv->shared_معلومات->mbcs.مٽا_سٽا_LFNLName;
    } وإلا {
        أرجع cnv->shared_معلومات->static_معلومات->name;
    }
}
U_CDECL_پڇاڙي


/* MBCS-to-Unicode conversion functions ------------------------------------- */

ساكن UChar32 U_CALLCONV
ucnv_MBCSGetFallback(UConverterMBCSTable *mbcs_جدول, uint32_t وٿي) {
    ثابت _MBCSToUFallback *منزل_UFallbacks;
    uint32_t i, شروعات, حد_بندي;

    حد_بندي=mbcs_جدول->ڳڻپ_منزل_UFallbacks;
    إذا(حد_بندي>0) {
        /* do a binary search for the fallback mapping */
        منزل_UFallbacks=mbcs_جدول->منزل_UFallbacks;
        شروعات=0;
        طالما(شروعات<حد_بندي-1) {
            i=(شروعات+حد_بندي)/2;
            إذا(وٿي<منزل_UFallbacks[i].وٿي) {
                حد_بندي=i;
            } وإلا {
                شروعات=i;
            }
        }

        /* did we really find it? */
        إذا(وٿي==منزل_UFallbacks[شروعات].وٿي) {
            أرجع منزل_UFallbacks[شروعات].ڪوڊ_نقطو;
        }
    }

    أرجع 0xfffe;
}

/* This version of ucnv_MBCSToUnicodeWithOffsets() is optimized for single-byte, single-state codepages. */
ساكن فراغ
ucnv_MBCSSingle_منزل_Unicode_With_Offsets(UConverter_منزل_Unicode_دليلون *p_دليلون,
                                UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي;
    char16_t *هدف;
    ثابت char16_t *هدف_حد_بندي;
    int32_t *offsets;

    ثابت int32_t (*state_جدول)[256];

    int32_t ذريعو_اشاريو;

    int32_t داخلا_2;
    char16_t c;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    ذريعو = reinterpret_cast<ثابت uint8_t*>(p_دليلون->ذريعو);
    ذريعو_حد_بندي = reinterpret_cast<ثابت uint8_t*>(p_دليلون->ذريعو_حد_بندي);
    هدف=p_دليلون->هدف;
    هدف_حد_بندي=p_دليلون->هدف_حد_بندي;
    offsets=p_دليلون->offsets;

    إذا((cnv->options&UCNV_OPTION_مٽا_سٽا_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_معلومات->mbcs.مٽا_سٽا_LFNLState_جدول;
    } وإلا {
        state_جدول=cnv->shared_معلومات->mbcs.state_جدول;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ذريعو_اشاريو=0;

    /* conversion loop */
    طالما(ذريعو<ذريعو_حد_بندي) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        إذا(هدف>=هدف_حد_بندي) {
            /* target is full */
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }

        داخلا_2=state_جدول[0][*ذريعو++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        إذا(MBCS_داخلا_FINAL_IS_VALID_سڌو_16(داخلا_2)) {
            /* output BMP code point */
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            إذا(offsets!=خالي_اشارو) {
                *offsets++=ذريعو_اشاريو;
            }

            /* normal end of action codes: prepare for a new character */
            ++ذريعو_اشاريو;
            استمر;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_داخلا_FINAL_ACTION(داخلا_2));
        إذا(action==MBCS_STATE_VALID_سڌو_20 ||
           (action==MBCS_STATE_FALLBACK_سڌو_20 && UCNV_منزل_U_USE_FALLBACK(cnv))
        ) {
            داخلا_2=MBCS_داخلا_FINAL_قدر(داخلا_2);
            /* output surrogate pair */
            *هدف++ = static_cast<char16_t>(0xd800 | static_cast<char16_t>(داخلا_2 >> 10));
            إذا(offsets!=خالي_اشارو) {
                *offsets++=ذريعو_اشاريو;
            }
            c = static_cast<char16_t>(0xdc00 | static_cast<char16_t>(داخلا_2 & 0x3ff));
            إذا(هدف<هدف_حد_بندي) {
                *هدف++=c;
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
            } وإلا {
                /* target overflow */
                cnv->UChar_Error_بفر[0]=c;
                cnv->UChar_Error_بفر_ڊيگهائي=1;
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                اكسر;
            }

            ++ذريعو_اشاريو;
            استمر;
        } وإلا إذا(action==MBCS_STATE_FALLBACK_سڌو_16) {
            إذا(UCNV_منزل_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }

                ++ذريعو_اشاريو;
                استمر;
            }
        } وإلا إذا(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } وإلا إذا(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
        } وإلا {
            /* reserved, must never occur */
            ++ذريعو_اشاريو;
            استمر;
        }

        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            /* callback(illegal) */
            اكسر;
        } وإلا /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_دليلون->ذريعو = reinterpret_cast<ثابت حرف*>(ذريعو);
            cnv->منزل_UBytes[0]=*(ذريعو-1);
            cnv->منزل_ULength=ext_منزل_U(cnv, cnv->shared_معلومات,
                                    1, &ذريعو, ذريعو_حد_بندي,
                                    &هدف, هدف_حد_بندي,
                                    &offsets, ذريعو_اشاريو,
                                    p_دليلون->flush,
                                    p_Error_ڪوڊ);
            ذريعو_اشاريو += 1 + static_cast<int32_t>(ذريعو - reinterpret_cast<ثابت uint8_t*>(p_دليلون->ذريعو));

            إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                /* not mappable or buffer overflow */
                اكسر;
            }
        }
    }

    /* write back the updated pointers */
    p_دليلون->ذريعو = reinterpret_cast<ثابت حرف*>(ذريعو);
    p_دليلون->هدف=هدف;
    p_دليلون->offsets=offsets;
}

/*
 * This version of ucnv_MBCSSingleToUnicodeWithOffsets() is optimized for single-byte, single-state codepages
 * that only map to and from the BMP.
 * In addition to single-byte optimizations, the offset calculations
 * become much easier.
 */
ساكن فراغ
ucnv_MBCSSingle_منزل_BMPWith_Offsets(UConverter_منزل_Unicode_دليلون *p_دليلون,
                            UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي, *آخري_ذريعو;
    char16_t *هدف;
    int32_t هدف_صلاحيت, length;
    int32_t *offsets;

    ثابت int32_t (*state_جدول)[256];

    int32_t ذريعو_اشاريو;

    int32_t داخلا_2;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    ذريعو = reinterpret_cast<ثابت uint8_t*>(p_دليلون->ذريعو);
    ذريعو_حد_بندي = reinterpret_cast<ثابت uint8_t*>(p_دليلون->ذريعو_حد_بندي);
    هدف=p_دليلون->هدف;
    هدف_صلاحيت = static_cast<int32_t>(p_دليلون->هدف_حد_بندي - p_دليلون->هدف);
    offsets=p_دليلون->offsets;

    إذا((cnv->options&UCNV_OPTION_مٽا_سٽا_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_معلومات->mbcs.مٽا_سٽا_LFNLState_جدول;
    } وإلا {
        state_جدول=cnv->shared_معلومات->mbcs.state_جدول;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ذريعو_اشاريو=0;
    آخري_ذريعو=ذريعو;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(ذريعو_حد_بندي - ذريعو);
    إذا(length<هدف_صلاحيت) {
        هدف_صلاحيت=length;
    }

#إذا MBCS_UNROLL_SINGLE_منزل_BMP
    /* unrolling makes it faster on Pentium III/Windows 2000 */
    /* unroll the loop with the most common case */
unrolled:
    إذا(هدف_صلاحيت>=16) {
        int32_t count, loops, oredEntries;

        loops=count=هدف_صلاحيت>>4;
        افعل {
            oredEntries=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            oredEntries|=داخلا_2=state_جدول[0][*ذريعو++];
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));

            /* were all 16 entries really valid? */
            إذا(!MBCS_داخلا_FINAL_IS_VALID_سڌو_16(oredEntries)) {
                /* no, return to the first of these 16 */
                ذريعو-=16;
                هدف-=16;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        هدف_صلاحيت-=16*count;

        إذا(offsets!=خالي_اشارو) {
            آخري_ذريعو+=16*count;
            طالما(count>0) {
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                --count;
            }
        }
    }
#جيڪڏهن_پورو

    /* conversion loop */
    طالما(هدف_صلاحيت > 0 && ذريعو < ذريعو_حد_بندي) {
        داخلا_2=state_جدول[0][*ذريعو++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        إذا(MBCS_داخلا_FINAL_IS_VALID_سڌو_16(داخلا_2)) {
            /* output BMP code point */
            *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            --هدف_صلاحيت;
            استمر;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_داخلا_FINAL_ACTION(داخلا_2));
        إذا(action==MBCS_STATE_FALLBACK_سڌو_16) {
            إذا(UCNV_منزل_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *هدف++ = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
                --هدف_صلاحيت;
                استمر;
            }
        } وإلا إذا(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } وإلا إذا(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
        } وإلا {
            /* reserved, must never occur */
            استمر;
        }

        /* set offsets since the start or the last extension */
        إذا(offsets!=خالي_اشارو) {
            int32_t count = static_cast<int32_t>(ذريعو - آخري_ذريعو);

            /* predecrement: do not set the offset for the callback-causing character */
            طالما(--count>0) {
                *offsets++=ذريعو_اشاريو++;
            }
            /* offset and sourceIndex are now set for the current character */
        }

        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            /* callback(illegal) */
            اكسر;
        } وإلا /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            آخري_ذريعو=ذريعو;
            cnv->منزل_UBytes[0]=*(ذريعو-1);
            cnv->منزل_ULength=ext_منزل_U(cnv, cnv->shared_معلومات,
                                    1, &ذريعو, ذريعو_حد_بندي,
                                    &هدف, p_دليلون->هدف_حد_بندي,
                                    &offsets, ذريعو_اشاريو,
                                    p_دليلون->flush,
                                    p_Error_ڪوڊ);
            ذريعو_اشاريو += 1 + static_cast<int32_t>(ذريعو - آخري_ذريعو);

            إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                /* not mappable or buffer overflow */
                اكسر;
            }

            /* recalculate the targetCapacity after an extension mapping */
            هدف_صلاحيت = static_cast<int32_t>(p_دليلون->هدف_حد_بندي - هدف);
            length = static_cast<int32_t>(ذريعو_حد_بندي - ذريعو);
            إذا(length<هدف_صلاحيت) {
                هدف_صلاحيت=length;
            }
        }

#إذا MBCS_UNROLL_SINGLE_منزل_BMP
        /* unrolling makes it faster on Pentium III/Windows 2000 */
        اذهب unrolled;
#جيڪڏهن_پورو
    }

    إذا(U_SUCCESS(*p_Error_ڪوڊ) && ذريعو<ذريعو_حد_بندي && هدف>=p_دليلون->هدف_حد_بندي) {
        /* target is full */
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
    }

    /* set offsets since the start or the last callback */
    إذا(offsets!=خالي_اشارو) {
        size_t count=ذريعو-آخري_ذريعو;
        طالما(count>0) {
            *offsets++=ذريعو_اشاريو++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_دليلون->ذريعو = reinterpret_cast<ثابت حرف*>(ذريعو);
    p_دليلون->هدف=هدف;
    p_دليلون->offsets=offsets;
}

ساكن UBool
has_Valid_Trail_بائيٽون(ثابت int32_t (*state_جدول)[256], uint8_t state) {
    ثابت int32_t *صف=state_جدول[state];
    int32_t b, داخلا_2;
    /* First test for final entries in this state for some commonly valid byte values. */
    داخلا_2=صف[0xa1];
    إذا( !MBCS_داخلا_IS_TRANSITION(داخلا_2) &&
        MBCS_داخلا_FINAL_ACTION(داخلا_2)!=MBCS_STATE_ILLEGAL
    ) {
        أرجع سچ;
    }
    داخلا_2=صف[0x41];
    إذا( !MBCS_داخلا_IS_TRANSITION(داخلا_2) &&
        MBCS_داخلا_FINAL_ACTION(داخلا_2)!=MBCS_STATE_ILLEGAL
    ) {
        أرجع سچ;
    }
    /* Then test for final entries in this state. */
    لكل(b=0; b<=0xff; ++b) {
        داخلا_2=صف[b];
        إذا( !MBCS_داخلا_IS_TRANSITION(داخلا_2) &&
            MBCS_داخلا_FINAL_ACTION(داخلا_2)!=MBCS_STATE_ILLEGAL
        ) {
            أرجع سچ;
        }
    }
    /* Then recurse for transition entries. */
    لكل(b=0; b<=0xff; ++b) {
        داخلا_2=صف[b];
        إذا( MBCS_داخلا_IS_TRANSITION(داخلا_2) &&
            has_Valid_Trail_بائيٽون(state_جدول, static_cast<uint8_t>(MBCS_داخلا_TRANSITION_STATE(داخلا_2)))
        ) {
            أرجع سچ;
        }
    }
    أرجع خطأ;
}

/*
 * Is byte b a single/lead byte in this state?
 * Recurse for transition states, because here we don't want to say that
 * b is a lead byte if all byte sequences that start with b are illegal.
 */
ساكن UBool
isSingleOrLead(ثابت int32_t (*state_جدول)[256], uint8_t state, UBool isDBCSOnly, uint8_t b) {
    ثابت int32_t *صف=state_جدول[state];
    int32_t داخلا_2=صف[b];
    إذا(MBCS_داخلا_IS_TRANSITION(داخلا_2)) {   /* lead byte */
        أرجع has_Valid_Trail_بائيٽون(state_جدول, static_cast<uint8_t>(MBCS_داخلا_TRANSITION_STATE(داخلا_2)));
    } وإلا {
        uint8_t action = static_cast<uint8_t>(MBCS_داخلا_FINAL_ACTION(داخلا_2));
        إذا(action==MBCS_STATE_CHANGE_ONLY && isDBCSOnly) {
            أرجع خطأ;   /* SI/SO are illegal for DBCS-only conversion */
        } وإلا {
            أرجع action!=MBCS_STATE_ILLEGAL;
        }
    }
}

U_CFUNC فراغ
ucnv_MBCSToUnicodeWithOffsets(UConverter_منزل_Unicode_دليلون *p_دليلون,
                          UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي;
    char16_t *هدف;
    ثابت char16_t *هدف_حد_بندي;
    int32_t *offsets;

    ثابت int32_t (*state_جدول)[256];
    ثابت uint16_t *unicode_ڪوڊ_Units;

    uint32_t وٿي;
    uint8_t state;
    int8_t byte_اشاريو;
    uint8_t *بائيٽون;

    int32_t ذريعو_اشاريو, ايندڙ_ذريعو_اشاريو;

    int32_t داخلا_2;
    char16_t c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_دليلون->converter;

    إذا(cnv->pre_منزل_ULength>0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_ميلاپ_منزل_U(cnv, p_دليلون, -1, p_Error_ڪوڊ);

        إذا(U_FAILURE(*p_Error_ڪوڊ) || cnv->pre_منزل_ULength<0) {
            أرجع;
        }
    }

    إذا(cnv->shared_معلومات->mbcs.ڳڻپ_حالتون==1) {
        إذا(!(cnv->shared_معلومات->mbcs.unicode_نقاب&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingle_منزل_BMPWith_Offsets(p_دليلون, p_Error_ڪوڊ);
        } وإلا {
            ucnv_MBCSSingle_منزل_Unicode_With_Offsets(p_دليلون, p_Error_ڪوڊ);
        }
        أرجع;
    }

    /* set up the local pointers */
    ذريعو=(ثابت uint8_t *)p_دليلون->ذريعو;
    ذريعو_حد_بندي=(ثابت uint8_t *)p_دليلون->ذريعو_حد_بندي;
    هدف=p_دليلون->هدف;
    هدف_حد_بندي=p_دليلون->هدف_حد_بندي;
    offsets=p_دليلون->offsets;

    إذا((cnv->options&UCNV_OPTION_مٽا_سٽا_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_معلومات->mbcs.مٽا_سٽا_LFNLState_جدول;
    } وإلا {
        state_جدول=cnv->shared_معلومات->mbcs.state_جدول;
    }
    unicode_ڪوڊ_Units=cnv->shared_معلومات->mbcs.unicode_ڪوڊ_Units;

    /* get the converter state from UConverter */
    وٿي=cnv->منزل_Unicode_حالت;
    byte_اشاريو=cnv->منزل_ULength;
    بائيٽون=cnv->منزل_UBytes;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    إذا((state=(uint8_t)(cnv->گهڻائي))==0) {
        state=cnv->shared_معلومات->mbcs.dbcsOnlyState;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ذريعو_اشاريو=byte_اشاريو==0 ? 0 : -1;
    ايندڙ_ذريعو_اشاريو=0;

    /* conversion loop */
    طالما(ذريعو<ذريعو_حد_بندي) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        إذا(هدف>=هدف_حد_بندي) {
            /* target is full */
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }

        إذا(byte_اشاريو==0) {
            /* optimized loop for 1/2-byte input and BMP output */
            إذا(offsets==خالي_اشارو) {
                افعل {
                    داخلا_2=state_جدول[state][*ذريعو];
                    إذا(MBCS_داخلا_IS_TRANSITION(داخلا_2)) {
                        state=(uint8_t)MBCS_داخلا_TRANSITION_STATE(داخلا_2);
                        وٿي=MBCS_داخلا_TRANSITION_وٿي(داخلا_2);

                        ++ذريعو;
                        إذا( ذريعو<ذريعو_حد_بندي &&
                            MBCS_داخلا_IS_FINAL(داخلا_2=state_جدول[state][*ذريعو]) &&
                            MBCS_داخلا_FINAL_ACTION(داخلا_2)==MBCS_STATE_VALID_16 &&
                            (c=unicode_ڪوڊ_Units[وٿي+MBCS_داخلا_FINAL_قدر_16(داخلا_2)])<0xfffe
                        ) {
                            ++ذريعو;
                            *هدف++=c;
                            state=(uint8_t)MBCS_داخلا_FINAL_STATE(داخلا_2); /* typically 0 */
                            وٿي=0;
                        } وإلا {
                            /* set the state and leave the optimized loop */
                            بائيٽون[0]=*(ذريعو-1);
                            byte_اشاريو=1;
                            اكسر;
                        }
                    } وإلا {
                        إذا(MBCS_داخلا_FINAL_IS_VALID_سڌو_16(داخلا_2)) {
                            /* output BMP code point */
                            ++ذريعو;
                            *هدف++=(char16_t)MBCS_داخلا_FINAL_قدر_16(داخلا_2);
                            state=(uint8_t)MBCS_داخلا_FINAL_STATE(داخلا_2); /* typically 0 */
                        } وإلا {
                            /* leave the optimized loop */
                            اكسر;
                        }
                    }
                } طالما(ذريعو<ذريعو_حد_بندي && هدف<هدف_حد_بندي);
            } وإلا /* offsets!=nullptr */ {
                افعل {
                    داخلا_2=state_جدول[state][*ذريعو];
                    إذا(MBCS_داخلا_IS_TRANSITION(داخلا_2)) {
                        state=(uint8_t)MBCS_داخلا_TRANSITION_STATE(داخلا_2);
                        وٿي=MBCS_داخلا_TRANSITION_وٿي(داخلا_2);

                        ++ذريعو;
                        إذا( ذريعو<ذريعو_حد_بندي &&
                            MBCS_داخلا_IS_FINAL(داخلا_2=state_جدول[state][*ذريعو]) &&
                            MBCS_داخلا_FINAL_ACTION(داخلا_2)==MBCS_STATE_VALID_16 &&
                            (c=unicode_ڪوڊ_Units[وٿي+MBCS_داخلا_FINAL_قدر_16(داخلا_2)])<0xfffe
                        ) {
                            ++ذريعو;
                            *هدف++=c;
                            إذا(offsets!=خالي_اشارو) {
                                *offsets++=ذريعو_اشاريو;
                                ذريعو_اشاريو=(ايندڙ_ذريعو_اشاريو+=2);
                            }
                            state=(uint8_t)MBCS_داخلا_FINAL_STATE(داخلا_2); /* typically 0 */
                            وٿي=0;
                        } وإلا {
                            /* set the state and leave the optimized loop */
                            ++ايندڙ_ذريعو_اشاريو;
                            بائيٽون[0]=*(ذريعو-1);
                            byte_اشاريو=1;
                            اكسر;
                        }
                    } وإلا {
                        إذا(MBCS_داخلا_FINAL_IS_VALID_سڌو_16(داخلا_2)) {
                            /* output BMP code point */
                            ++ذريعو;
                            *هدف++=(char16_t)MBCS_داخلا_FINAL_قدر_16(داخلا_2);
                            إذا(offsets!=خالي_اشارو) {
                                *offsets++=ذريعو_اشاريو;
                                ذريعو_اشاريو=++ايندڙ_ذريعو_اشاريو;
                            }
                            state=(uint8_t)MBCS_داخلا_FINAL_STATE(داخلا_2); /* typically 0 */
                        } وإلا {
                            /* leave the optimized loop */
                            اكسر;
                        }
                    }
                } طالما(ذريعو<ذريعو_حد_بندي && هدف<هدف_حد_بندي);
            }

            /*
             * these tests and break statements could be put inside the loop
             * if C had "break outerLoop" like Java
             */
            إذا(ذريعو>=ذريعو_حد_بندي) {
                اكسر;
            }
            إذا(هدف>=هدف_حد_بندي) {
                /* target is full */
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                اكسر;
            }

            ++ايندڙ_ذريعو_اشاريو;
            بائيٽون[byte_اشاريو++]=*ذريعو++;
        } وإلا /* byteIndex>0 */ {
            ++ايندڙ_ذريعو_اشاريو;
            داخلا_2=state_جدول[state][بائيٽون[byte_اشاريو++]=*ذريعو++];
        }

        إذا(MBCS_داخلا_IS_TRANSITION(داخلا_2)) {
            state=(uint8_t)MBCS_داخلا_TRANSITION_STATE(داخلا_2);
            وٿي+=MBCS_داخلا_TRANSITION_وٿي(داخلا_2);
            استمر;
        }

        /* save the previous state for proper extension mapping with SI/SO-stateful converters */
        cnv->گهڻائي=state;

        /* set the next state early so that we can reuse the entry variable */
        state=(uint8_t)MBCS_داخلا_FINAL_STATE(داخلا_2); /* typically 0 */

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action=(uint8_t)(MBCS_داخلا_FINAL_ACTION(داخلا_2));
        إذا(action==MBCS_STATE_VALID_16) {
            وٿي+=MBCS_داخلا_FINAL_قدر_16(داخلا_2);
            c=unicode_ڪوڊ_Units[وٿي];
            إذا(c<0xfffe) {
                /* output BMP code point */
                *هدف++=c;
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
                byte_اشاريو=0;
            } وإلا إذا(c==0xfffe) {
                إذا(UCNV_منزل_U_USE_FALLBACK(cnv) && (داخلا_2=(int32_t)ucnv_MBCSGetFallback(&cnv->shared_معلومات->mbcs, وٿي))!=0xfffe) {
                    /* output fallback BMP code point */
                    *هدف++=(char16_t)داخلا_2;
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                    }
                    byte_اشاريو=0;
                }
            } وإلا {
                /* callback(illegal) */
                *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
            }
        } وإلا إذا(action==MBCS_STATE_VALID_سڌو_16) {
            /* output BMP code point */
            *هدف++=(char16_t)MBCS_داخلا_FINAL_قدر_16(داخلا_2);
            إذا(offsets!=خالي_اشارو) {
                *offsets++=ذريعو_اشاريو;
            }
            byte_اشاريو=0;
        } وإلا إذا(action==MBCS_STATE_VALID_16_PAIR) {
            وٿي+=MBCS_داخلا_FINAL_قدر_16(داخلا_2);
            c=unicode_ڪوڊ_Units[وٿي++];
            إذا(c<0xd800) {
                /* output BMP code point below 0xd800 */
                *هدف++=c;
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
                byte_اشاريو=0;
            } وإلا إذا(UCNV_منزل_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                /* output roundtrip or fallback surrogate pair */
                *هدف++=(char16_t)(c&0xdbff);
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
                byte_اشاريو=0;
                إذا(هدف<هدف_حد_بندي) {
                    *هدف++=unicode_ڪوڊ_Units[وٿي];
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                    }
                } وإلا {
                    /* target overflow */
                    cnv->UChar_Error_بفر[0]=unicode_ڪوڊ_Units[وٿي];
                    cnv->UChar_Error_بفر_ڊيگهائي=1;
                    *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;

                    وٿي=0;
                    اكسر;
                }
            } وإلا إذا(UCNV_منزل_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                *هدف++=unicode_ڪوڊ_Units[وٿي];
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
                byte_اشاريو=0;
            } وإلا إذا(c==0xffff) {
                /* callback(illegal) */
                *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
            }
        } وإلا إذا(action==MBCS_STATE_VALID_سڌو_20 ||
                  (action==MBCS_STATE_FALLBACK_سڌو_20 && UCNV_منزل_U_USE_FALLBACK(cnv))
        ) {
            داخلا_2=MBCS_داخلا_FINAL_قدر(داخلا_2);
            /* output surrogate pair */
            *هدف++=(char16_t)(0xd800|(char16_t)(داخلا_2>>10));
            إذا(offsets!=خالي_اشارو) {
                *offsets++=ذريعو_اشاريو;
            }
            byte_اشاريو=0;
            c=(char16_t)(0xdc00|(char16_t)(داخلا_2&0x3ff));
            إذا(هدف<هدف_حد_بندي) {
                *هدف++=c;
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
            } وإلا {
                /* target overflow */
                cnv->UChar_Error_بفر[0]=c;
                cnv->UChar_Error_بفر_ڊيگهائي=1;
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;

                وٿي=0;
                اكسر;
            }
        } وإلا إذا(action==MBCS_STATE_CHANGE_ONLY) {
            /*
             * This serves as a state change without any output.
             * It is useful for reading simple stateful encodings,
             * for example using just Shift-In/Shift-Out codes.
             * The 21 unused bits may later be used for more sophisticated
             * state transitions.
             */
            إذا(cnv->shared_معلومات->mbcs.dbcsOnlyState==0) {
                byte_اشاريو=0;
            } وإلا {
                /* SI/SO are illegal for DBCS-only conversion */
                state=(uint8_t)(cnv->گهڻائي); /* restore the previous state */

                /* callback(illegal) */
                *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
            }
        } وإلا إذا(action==MBCS_STATE_FALLBACK_سڌو_16) {
            إذا(UCNV_منزل_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *هدف++=(char16_t)MBCS_داخلا_FINAL_قدر_16(داخلا_2);
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
                byte_اشاريو=0;
            }
        } وإلا إذا(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } وإلا إذا(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
        } وإلا {
            /* reserved, must never occur */
            byte_اشاريو=0;
        }

        /* end of action codes: prepare for a new character */
        وٿي=0;

        إذا(byte_اشاريو==0) {
            ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
        } وإلا إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            /* callback(illegal) */
            إذا(byte_اشاريو>1) {
                /*
                 * Ticket 5691: consistent illegal sequences:
                 * - We include at least the first byte in the illegal sequence.
                 * - If any of the non-initial bytes could be the start of a character,
                 *   we stop the illegal sequence before the first one of those.
                 */
                UBool isDBCSOnly = cnv->shared_معلومات->mbcs.dbcsOnlyState != 0;
                int8_t i;
                لكل(i=1;
                    i<byte_اشاريو && !isSingleOrLead(state_جدول, state, isDBCSOnly, بائيٽون[i]);
                    ++i) {}
                إذا(i<byte_اشاريو) {
                    /* Back out some bytes. */
                    int8_t پٺ_ٻاهريون_مفاصلو=byte_اشاريو-i;
                    int32_t بائيٽون_From_This_بفر=(int32_t)(ذريعو-(ثابت uint8_t *)p_دليلون->ذريعو);
                    byte_اشاريو=i;  /* length of reported illegal byte sequence */
                    إذا(پٺ_ٻاهريون_مفاصلو<=بائيٽون_From_This_بفر) {
                        ذريعو-=پٺ_ٻاهريون_مفاصلو;
                    } وإلا {
                        /* Back out bytes from the previous buffer: Need to replay them. */
                        cnv->pre_منزل_ULength=(int8_t)(بائيٽون_From_This_بفر-پٺ_ٻاهريون_مفاصلو);
                        /* preToULength is negative! */
                        uprv_memcpy(cnv->pre_منزل_U, بائيٽون+i, -cnv->pre_منزل_ULength);
                        ذريعو=(ثابت uint8_t *)p_دليلون->ذريعو;
                    }
                }
            }
            اكسر;
        } وإلا /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_دليلون->ذريعو=(ثابت حرف *)ذريعو;
            byte_اشاريو=ext_منزل_U(cnv, cnv->shared_معلومات,
                              byte_اشاريو, &ذريعو, ذريعو_حد_بندي,
                              &هدف, هدف_حد_بندي,
                              &offsets, ذريعو_اشاريو,
                              p_دليلون->flush,
                              p_Error_ڪوڊ);
            ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو+=(int32_t)(ذريعو-(ثابت uint8_t *)p_دليلون->ذريعو);

            إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                /* not mappable or buffer overflow */
                اكسر;
            }
        }
    }

    /* set the converter state back into UConverter */
    cnv->منزل_Unicode_حالت=وٿي;
    cnv->گهڻائي=state;
    cnv->منزل_ULength=byte_اشاريو;

    /* write back the updated pointers */
    p_دليلون->ذريعو=(ثابت حرف *)ذريعو;
    p_دليلون->هدف=هدف;
    p_دليلون->offsets=offsets;
}

/*
 * This version of ucnv_MBCSGetNextUChar() is optimized for single-byte, single-state codepages.
 * We still need a conversion loop in case we find reserved action codes, which are to be ignored.
 */
ساكن UChar32
ucnv_MBCSSingle_Get_ايندڙ_UChar(UConverter_منزل_Unicode_دليلون *p_دليلون,
                        UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت int32_t (*state_جدول)[256];
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي;

    int32_t داخلا_2;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    ذريعو = reinterpret_cast<ثابت uint8_t*>(p_دليلون->ذريعو);
    ذريعو_حد_بندي = reinterpret_cast<ثابت uint8_t*>(p_دليلون->ذريعو_حد_بندي);
    إذا((cnv->options&UCNV_OPTION_مٽا_سٽا_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_معلومات->mbcs.مٽا_سٽا_LFNLState_جدول;
    } وإلا {
        state_جدول=cnv->shared_معلومات->mbcs.state_جدول;
    }

    /* conversion loop */
    طالما(ذريعو<ذريعو_حد_بندي) {
        داخلا_2=state_جدول[0][*ذريعو++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* write back the updated pointer early so that we can return directly */
        p_دليلون->ذريعو = reinterpret_cast<ثابت حرف*>(ذريعو);

        إذا(MBCS_داخلا_FINAL_IS_VALID_سڌو_16(داخلا_2)) {
            /* output BMP code point */
            أرجع static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_داخلا_FINAL_ACTION(داخلا_2));
        إذا( action==MBCS_STATE_VALID_سڌو_20 ||
            (action==MBCS_STATE_FALLBACK_سڌو_20 && UCNV_منزل_U_USE_FALLBACK(cnv))
        ) {
            /* output supplementary code point */
            أرجع static_cast<UChar32>(MBCS_داخلا_FINAL_قدر(داخلا_2) + 0x10000);
        } وإلا إذا(action==MBCS_STATE_FALLBACK_سڌو_16) {
            إذا(UCNV_منزل_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                أرجع static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
            }
        } وإلا إذا(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } وإلا إذا(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
        } وإلا {
            /* reserved, must never occur */
            استمر;
        }

        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            /* callback(illegal) */
            اكسر;
        } وإلا /* unassigned sequence */ {
            /* defer to the generic implementation */
            p_دليلون->ذريعو = reinterpret_cast<ثابت حرف*>(ذريعو) - 1;
            أرجع UCNV_GET_ايندڙ_UCHAR_USE_منزل_U;
        }
    }

    /* no output because of empty input or only state changes */
    *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
    أرجع 0xffff;
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
ساكن UChar32 U_CALLCONV
ucnv_MBCSGet_ايندڙ_UChar(UConverter_منزل_Unicode_دليلون *p_دليلون,
                  UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي, *آخري_ذريعو;

    ثابت int32_t (*state_جدول)[256];
    ثابت uint16_t *unicode_ڪوڊ_Units;

    uint32_t وٿي;
    uint8_t state;

    int32_t داخلا_2;
    UChar32 c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_دليلون->converter;

    إذا(cnv->pre_منزل_ULength>0) {
        /* use the generic code in ucnv_getNextUChar() to continue with a partial match */
        أرجع UCNV_GET_ايندڙ_UCHAR_USE_منزل_U;
    }

    إذا(cnv->shared_معلومات->mbcs.unicode_نقاب&UCNV_HAS_SURROGATES) {
        /*
         * Using the generic ucnv_getNextUChar() code lets us deal correctly
         * with the rare case of a codepage that maps single surrogates
         * without adding the complexity to this already complicated function here.
         */
        أرجع UCNV_GET_ايندڙ_UCHAR_USE_منزل_U;
    } وإلا إذا(cnv->shared_معلومات->mbcs.ڳڻپ_حالتون==1) {
        أرجع ucnv_MBCSSingle_Get_ايندڙ_UChar(p_دليلون, p_Error_ڪوڊ);
    }

    /* set up the local pointers */
    ذريعو = آخري_ذريعو = reinterpret_cast<ثابت uint8_t*>(p_دليلون->ذريعو);
    ذريعو_حد_بندي = reinterpret_cast<ثابت uint8_t*>(p_دليلون->ذريعو_حد_بندي);

    إذا((cnv->options&UCNV_OPTION_مٽا_سٽا_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_معلومات->mbcs.مٽا_سٽا_LFNLState_جدول;
    } وإلا {
        state_جدول=cnv->shared_معلومات->mbcs.state_جدول;
    }
    unicode_ڪوڊ_Units=cnv->shared_معلومات->mbcs.unicode_ڪوڊ_Units;

    /* get the converter state from UConverter */
    وٿي=cnv->منزل_Unicode_حالت;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    إذا ((state = static_cast<uint8_t>(cnv->گهڻائي)) == 0) {
        state=cnv->shared_معلومات->mbcs.dbcsOnlyState;
    }

    /* conversion loop */
    c=U_SENTINEL;
    طالما(ذريعو<ذريعو_حد_بندي) {
        داخلا_2=state_جدول[state][*ذريعو++];
        إذا(MBCS_داخلا_IS_TRANSITION(داخلا_2)) {
            state = static_cast<uint8_t>(MBCS_داخلا_TRANSITION_STATE(داخلا_2));
            وٿي+=MBCS_داخلا_TRANSITION_وٿي(داخلا_2);

            /* optimization for 1/2-byte input and BMP output */
            إذا( ذريعو<ذريعو_حد_بندي &&
                MBCS_داخلا_IS_FINAL(داخلا_2=state_جدول[state][*ذريعو]) &&
                MBCS_داخلا_FINAL_ACTION(داخلا_2)==MBCS_STATE_VALID_16 &&
                (c=unicode_ڪوڊ_Units[وٿي+MBCS_داخلا_FINAL_قدر_16(داخلا_2)])<0xfffe
            ) {
                ++ذريعو;
                state = static_cast<uint8_t>(MBCS_داخلا_FINAL_STATE(داخلا_2)); /* typically 0 */
                /* output BMP code point */
                اكسر;
            }
        } وإلا {
            /* save the previous state for proper extension mapping with SI/SO-stateful converters */
            cnv->گهڻائي=state;

            /* set the next state early so that we can reuse the entry variable */
            state = static_cast<uint8_t>(MBCS_داخلا_FINAL_STATE(داخلا_2)); /* typically 0 */

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action = static_cast<uint8_t>(MBCS_داخلا_FINAL_ACTION(داخلا_2));
            إذا(action==MBCS_STATE_VALID_سڌو_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
                اكسر;
            } وإلا إذا(action==MBCS_STATE_VALID_16) {
                وٿي+=MBCS_داخلا_FINAL_قدر_16(داخلا_2);
                c=unicode_ڪوڊ_Units[وٿي];
                إذا(c<0xfffe) {
                    /* output BMP code point */
                    اكسر;
                } وإلا إذا(c==0xfffe) {
                    إذا(UCNV_منزل_U_USE_FALLBACK(cnv) && (c=ucnv_MBCSGetFallback(&cnv->shared_معلومات->mbcs, وٿي))!=0xfffe) {
                        اكسر;
                    }
                } وإلا {
                    /* callback(illegal) */
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                }
            } وإلا إذا(action==MBCS_STATE_VALID_16_PAIR) {
                وٿي+=MBCS_داخلا_FINAL_قدر_16(داخلا_2);
                c=unicode_ڪوڊ_Units[وٿي++];
                إذا(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                    اكسر;
                } وإلا إذا(UCNV_منزل_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_ڪوڊ_Units[وٿي]+(0x10000-0xdc00);
                    اكسر;
                } وإلا إذا(UCNV_منزل_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_ڪوڊ_Units[وٿي];
                    اكسر;
                } وإلا إذا(c==0xffff) {
                    /* callback(illegal) */
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                }
            } وإلا إذا(action==MBCS_STATE_VALID_سڌو_20 ||
                      (action==MBCS_STATE_FALLBACK_سڌو_20 && UCNV_منزل_U_USE_FALLBACK(cnv))
            ) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_داخلا_FINAL_قدر(داخلا_2) + 0x10000);
                اكسر;
            } وإلا إذا(action==MBCS_STATE_CHANGE_ONLY) {
                /*
                 * This serves as a state change without any output.
                 * It is useful for reading simple stateful encodings,
                 * for example using just Shift-In/Shift-Out codes.
                 * The 21 unused bits may later be used for more sophisticated
                 * state transitions.
                 */
                إذا(cnv->shared_معلومات->mbcs.dbcsOnlyState!=0) {
                    /* SI/SO are illegal for DBCS-only conversion */
                    state = static_cast<uint8_t>(cnv->گهڻائي); /* restore the previous state */

                    /* callback(illegal) */
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                }
            } وإلا إذا(action==MBCS_STATE_FALLBACK_سڌو_16) {
                إذا(UCNV_منزل_U_USE_FALLBACK(cnv)) {
                    /* output BMP code point */
                    c = static_cast<char16_t>(MBCS_داخلا_FINAL_قدر_16(داخلا_2));
                    اكسر;
                }
            } وإلا إذا(action==MBCS_STATE_UNASSIGNED) {
                /* just fall through */
            } وإلا إذا(action==MBCS_STATE_ILLEGAL) {
                /* callback(illegal) */
                *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
            } وإلا {
                /* reserved (must never occur), or only state change */
                وٿي=0;
                آخري_ذريعو=ذريعو;
                استمر;
            }

            /* end of action codes: prepare for a new character */
            وٿي=0;

            إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                /* callback(illegal) */
                اكسر;
            } وإلا /* unassigned sequence */ {
                /* defer to the generic implementation */
                cnv->منزل_Unicode_حالت=0;
                cnv->گهڻائي=state;
                p_دليلون->ذريعو = reinterpret_cast<ثابت حرف*>(آخري_ذريعو);
                أرجع UCNV_GET_ايندڙ_UCHAR_USE_منزل_U;
            }
        }
    }

    إذا(c<0) {
        إذا(U_SUCCESS(*p_Error_ڪوڊ) && ذريعو==ذريعو_حد_بندي && آخري_ذريعو<ذريعو) {
            /* incomplete character byte sequence */
            cnv->منزل_ULength = static_cast<int8_t>(ذريعو - آخري_ذريعو);
            uprv_memcpy(cnv->منزل_UBytes, آخري_ذريعو, cnv->منزل_ULength);
            *p_Error_ڪوڊ=U_TRUNCATED_CHAR_مليو;
        } وإلا إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            /* callback(illegal) */
            /*
             * Ticket 5691: consistent illegal sequences:
             * - We include at least the first byte in the illegal sequence.
             * - If any of the non-initial bytes could be the start of a character,
             *   we stop the illegal sequence before the first one of those.
             */
            UBool isDBCSOnly = static_cast<UBool>(cnv->shared_معلومات->mbcs.dbcsOnlyState != 0);
            uint8_t *بائيٽون=cnv->منزل_UBytes;
            *بائيٽون++=*آخري_ذريعو++;     /* first byte */
            إذا(آخري_ذريعو==ذريعو) {
                cnv->منزل_ULength=1;
            } وإلا /* lastSource<source: multi-byte character */ {
                int8_t i;
                لكل(i=1;
                    آخري_ذريعو<ذريعو && !isSingleOrLead(state_جدول, state, isDBCSOnly, *آخري_ذريعو);
                    ++i
                ) {
                    *بائيٽون++=*آخري_ذريعو++;
                }
                cnv->منزل_ULength=i;
                ذريعو=آخري_ذريعو;
            }
        } وإلا {
            /* no output because of empty input or only state changes */
            *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        }
        c=0xffff;
    }

    /* set the converter state back into UConverter, ready for a new character */
    cnv->منزل_Unicode_حالت=0;
    cnv->گهڻائي=state;

    /* write back the updated pointer */
    p_دليلون->ذريعو = reinterpret_cast<ثابت حرف*>(ذريعو);
    أرجع c;
}

#إذا 0
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
ucnv_MBCSSingle_Simple_Get_ايندڙ_UChar(UConverter_Shared_معلومات *shared_معلومات,
                              uint8_t b, UBool useFallback) {
    int32_t داخلا_2;
    uint8_t action;

    داخلا_2=shared_معلومات->mbcs.state_جدول[0][b];
    /* MBCS_ENTRY_IS_FINAL(entry) */

    إذا(MBCS_داخلا_FINAL_IS_VALID_سڌو_16(داخلا_2)) {
        /* output BMP code point */
        أرجع (char16_t)MBCS_داخلا_FINAL_قدر_16(داخلا_2);
    }

    /*
     * An if-else-if chain provides more reliable performance for
     * the most common cases compared to a switch.
     */
    action=(uint8_t)(MBCS_داخلا_FINAL_ACTION(داخلا_2));
    إذا(action==MBCS_STATE_VALID_سڌو_20) {
        /* output supplementary code point */
        أرجع 0x10000+MBCS_داخلا_FINAL_قدر(داخلا_2);
    } وإلا إذا(action==MBCS_STATE_FALLBACK_سڌو_16) {
        إذا(!منزل_U_USE_FALLBACK(useFallback)) {
            أرجع 0xfffe;
        }
        /* output BMP code point */
        أرجع (char16_t)MBCS_داخلا_FINAL_قدر_16(داخلا_2);
    } وإلا إذا(action==MBCS_STATE_FALLBACK_سڌو_20) {
        إذا(!منزل_U_USE_FALLBACK(useFallback)) {
            أرجع 0xfffe;
        }
        /* output supplementary code point */
        أرجع 0x10000+MBCS_داخلا_FINAL_قدر(داخلا_2);
    } وإلا إذا(action==MBCS_STATE_UNASSIGNED) {
        أرجع 0xfffe;
    } وإلا إذا(action==MBCS_STATE_ILLEGAL) {
        أرجع 0xffff;
    } وإلا {
        /* reserved, must never occur */
        أرجع 0xffff;
    }
}
#جيڪڏهن_پورو

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
ucnv_MBCSSimple_Get_ايندڙ_UChar(UConverter_Shared_معلومات *shared_معلومات,
                        ثابت حرف *ذريعو, int32_t length,
                        UBool useFallback) {
    ثابت int32_t (*state_جدول)[256];
    ثابت uint16_t *unicode_ڪوڊ_Units;

    uint32_t وٿي;
    uint8_t state, action;

    UChar32 c;
    int32_t i, داخلا_2;

    إذا(length<=0) {
        /* no input at all: "illegal" */
        أرجع 0xffff;
    }

#إذا 0
/*
 * Code disabled 2002dec09 (ICU 2.4) because it is not currently used in ICU. markus
 * TODO In future releases, verify that this function is never called for SBCS
 * conversions, i.e., that sharedData->mbcs.countStates==1 is still true.
 * Removal improves code coverage.
 */
    /* use optimized function if possible */
    إذا(shared_معلومات->mbcs.ڳڻپ_حالتون==1) {
        إذا(length==1) {
            أرجع ucnv_MBCSSingle_Simple_Get_ايندڙ_UChar(shared_معلومات, (uint8_t)*ذريعو, useFallback);
        } وإلا {
            أرجع 0xffff; /* illegal: more than a single byte for an SBCS converter */
        }
    }
#جيڪڏهن_پورو

    /* set up the local pointers */
    state_جدول=shared_معلومات->mbcs.state_جدول;
    unicode_ڪوڊ_Units=shared_معلومات->mbcs.unicode_ڪوڊ_Units;

    /* converter state */
    وٿي=0;
    state=shared_معلومات->mbcs.dbcsOnlyState;

    /* conversion loop */
    لكل(i=0;;) {
        داخلا_2=state_جدول[state][(uint8_t)ذريعو[i++]];
        إذا(MBCS_داخلا_IS_TRANSITION(داخلا_2)) {
            state=(uint8_t)MBCS_داخلا_TRANSITION_STATE(داخلا_2);
            وٿي+=MBCS_داخلا_TRANSITION_وٿي(داخلا_2);

            إذا(i==length) {
                أرجع 0xffff; /* truncated character */
            }
        } وإلا {
            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=(uint8_t)(MBCS_داخلا_FINAL_ACTION(داخلا_2));
            إذا(action==MBCS_STATE_VALID_16) {
                وٿي+=MBCS_داخلا_FINAL_قدر_16(داخلا_2);
                c=unicode_ڪوڊ_Units[وٿي];
                إذا(c!=0xfffe) {
                    /*پورو*/
                } وإلا إذا(UCNV_منزل_U_USE_FALLBACK(cnv)) {
                    c=ucnv_MBCSGetFallback(&shared_معلومات->mbcs, وٿي);
                /* else done with 0xfffe */
                }
                اكسر;
            } وإلا إذا(action==MBCS_STATE_VALID_سڌو_16) {
                /* output BMP code point */
                c=(char16_t)MBCS_داخلا_FINAL_قدر_16(داخلا_2);
                اكسر;
            } وإلا إذا(action==MBCS_STATE_VALID_16_PAIR) {
                وٿي+=MBCS_داخلا_FINAL_قدر_16(داخلا_2);
                c=unicode_ڪوڊ_Units[وٿي++];
                إذا(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } وإلا إذا(UCNV_منزل_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=(UChar32)(((c&0x3ff)<<10)+unicode_ڪوڊ_Units[وٿي]+(0x10000-0xdc00));
                } وإلا إذا(UCNV_منزل_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_ڪوڊ_Units[وٿي];
                } وإلا إذا(c==0xffff) {
                    أرجع 0xffff;
                } وإلا {
                    c=0xfffe;
                }
                اكسر;
            } وإلا إذا(action==MBCS_STATE_VALID_سڌو_20) {
                /* output supplementary code point */
                c=0x10000+MBCS_داخلا_FINAL_قدر(داخلا_2);
                اكسر;
            } وإلا إذا(action==MBCS_STATE_FALLBACK_سڌو_16) {
                إذا(!منزل_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    اكسر;
                }
                /* output BMP code point */
                c=(char16_t)MBCS_داخلا_FINAL_قدر_16(داخلا_2);
                اكسر;
            } وإلا إذا(action==MBCS_STATE_FALLBACK_سڌو_20) {
                إذا(!منزل_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    اكسر;
                }
                /* output supplementary code point */
                c=0x10000+MBCS_داخلا_FINAL_قدر(داخلا_2);
                اكسر;
            } وإلا إذا(action==MBCS_STATE_UNASSIGNED) {
                c=0xfffe;
                اكسر;
            }

            /*
             * forbid MBCS_STATE_CHANGE_ONLY for this function,
             * and MBCS_STATE_ILLEGAL and reserved action codes
             */
            أرجع 0xffff;
        }
    }

    إذا(i!=length) {
        /* illegal for this function: not all input consumed */
        أرجع 0xffff;
    }

    إذا(c==0xfffe) {
        /* try an extension mapping */
        ثابت int32_t *cx=shared_معلومات->mbcs.extIndexes;
        إذا(cx!=خالي_اشارو) {
            أرجع ucnv_ext_Simple_ميلاپ_منزل_U(cx, ذريعو, length, useFallback);
        }
    }

    أرجع c;
}

/* MBCS-from-Unicode conversion functions ----------------------------------- */

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for double-byte codepages. */
ساكن فراغ
ucnv_MBCSDoubleFromUnicodeWithOffsets(UConverter_From_Unicode_دليلون *p_دليلون,
                                  UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت char16_t *ذريعو, *ذريعو_حد_بندي;
    uint8_t *هدف;
    int32_t هدف_صلاحيت;
    int32_t *offsets;

    ثابت uint16_t *جدول;
    ثابت uint16_t *mbcs_اشاريو;
    ثابت uint8_t *بائيٽون;

    UChar32 c;

    int32_t ذريعو_اشاريو, ايندڙ_ذريعو_اشاريو;

    uint32_t stage2_داخلا;
    uint32_t asciiRoundtrips;
    uint32_t value;
    uint8_t unicode_نقاب;

    /* use optimized function if possible */
    cnv=p_دليلون->converter;
    unicode_نقاب=cnv->shared_معلومات->mbcs.unicode_نقاب;

    /* set up the local pointers */
    ذريعو=p_دليلون->ذريعو;
    ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
    هدف = reinterpret_cast<uint8_t*>(p_دليلون->هدف);
    هدف_صلاحيت = static_cast<int32_t>(p_دليلون->هدف_حد_بندي - p_دليلون->هدف);
    offsets=p_دليلون->offsets;

    جدول=cnv->shared_معلومات->mbcs.from_Unicode_جدول;
    mbcs_اشاريو=cnv->shared_معلومات->mbcs.mbcs_اشاريو;
    إذا((cnv->options&UCNV_OPTION_مٽا_سٽا_LFNL)!=0) {
        بائيٽون=cnv->shared_معلومات->mbcs.مٽا_سٽا_LFNLFrom_Unicode_بائيٽون;
    } وإلا {
        بائيٽون=cnv->shared_معلومات->mbcs.from_Unicode_بائيٽون;
    }
    asciiRoundtrips=cnv->shared_معلومات->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ذريعو_اشاريو= c==0 ? 0 : -1;
    ايندڙ_ذريعو_اشاريو=0;

    /* conversion loop */
    إذا(c!=0 && هدف_صلاحيت>0) {
        اذهب getTrail;
    }

    طالما(ذريعو<ذريعو_حد_بندي) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        إذا(هدف_صلاحيت>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ذريعو++;
            ++ايندڙ_ذريعو_اشاريو;
            إذا(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *هدف++ = static_cast<uint8_t>(c);
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                    ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                }
                --هدف_صلاحيت;
                c=0;
                استمر;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            إذا(c<=0xd7ff) {
                value=DBCS_نتيجو_FROM_MOST_BMP(mbcs_اشاريو, (ثابت uint16_t *)بائيٽون, c);
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                إذا(value==0) {
                    اذهب unassigned;
                }
                /* output the value */
            } وإلا {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                إذا(U16_IS_SURROGATE(c) && !(unicode_نقاب&UCNV_HAS_SURROGATES)) {
                    إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        إذا(ذريعو<ذريعو_حد_بندي) {
                            /* test the following code unit */
                            char16_t trail=*ذريعو;
                            إذا(U16_IS_TRAIL(trail)) {
                                ++ذريعو;
                                ++ايندڙ_ذريعو_اشاريو;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                إذا(!(unicode_نقاب&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    /* callback(unassigned) */
                                    اذهب unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } وإلا {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                                اكسر;
                            }
                        } وإلا {
                            /* no more input */
                            اكسر;
                        }
                    } وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                        اكسر;
                    }
                }

                /* convert the Unicode code point in c into codepage bytes */
                stage2_داخلا=MBCS_STAGE_2_FROM_U(جدول, c);

                /* get the bytes and the length for the output */
                /* MBCS_OUTPUT_2 */
                value=MBCS_قدر_2_FROM_STAGE_2(بائيٽون, stage2_داخلا, c);

                /* is this code point assigned, or do we use fallbacks? */
                إذا(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_داخلا, c) ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_دليلون->ذريعو=ذريعو;
                    c=_extFromU(cnv, cnv->shared_معلومات,
                                c, &ذريعو, ذريعو_حد_بندي,
                                &هدف, هدف+هدف_صلاحيت,
                                &offsets, ذريعو_اشاريو,
                                p_دليلون->flush,
                                p_Error_ڪوڊ);
                    ايندڙ_ذريعو_اشاريو += static_cast<int32_t>(ذريعو - p_دليلون->ذريعو);

                    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                        /* not mappable or buffer overflow */
                        اكسر;
                    } وإلا {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        هدف_صلاحيت = static_cast<int32_t>(p_دليلون->هدف_حد_بندي - reinterpret_cast<حرف*>(هدف));

                        /* normal end of conversion: prepare for a new character */
                        ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                        استمر;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            إذا(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *هدف++ = static_cast<uint8_t>(value);
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
                --هدف_صلاحيت;
            } وإلا /* length==2 */ {
                *هدف++ = static_cast<uint8_t>(value >> 8);
                إذا(2<=هدف_صلاحيت) {
                    *هدف++ = static_cast<uint8_t>(value);
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                        *offsets++=ذريعو_اشاريو;
                    }
                    هدف_صلاحيت-=2;
                } وإلا {
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                    }
                    cnv->char_Error_بفر[0] = static_cast<حرف>(value);
                    cnv->char_Error_بفر_ڊيگهائي=1;

                    /* target overflow */
                    هدف_صلاحيت=0;
                    *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                    c=0;
                    اكسر;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
            استمر;
        } وإلا {
            /* target is full */
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_دليلون->ذريعو=ذريعو;
    p_دليلون->هدف = reinterpret_cast<حرف*>(هدف);
    p_دليلون->offsets=offsets;
}

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for single-byte codepages. */
ساكن فراغ
ucnv_MBCSSingleFromUnicodeWithOffsets(UConverter_From_Unicode_دليلون *p_دليلون,
                                  UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت char16_t *ذريعو, *ذريعو_حد_بندي;
    uint8_t *هدف;
    int32_t هدف_صلاحيت;
    int32_t *offsets;

    ثابت uint16_t *جدول;
    ثابت uint16_t *results;

    UChar32 c;

    int32_t ذريعو_اشاريو, ايندڙ_ذريعو_اشاريو;

    uint16_t value, min_قدر;
    UBool hasSupplementary;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    ذريعو=p_دليلون->ذريعو;
    ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
    هدف = reinterpret_cast<uint8_t*>(p_دليلون->هدف);
    هدف_صلاحيت = static_cast<int32_t>(p_دليلون->هدف_حد_بندي - p_دليلون->هدف);
    offsets=p_دليلون->offsets;

    جدول=cnv->shared_معلومات->mbcs.from_Unicode_جدول;
    إذا((cnv->options&UCNV_OPTION_مٽا_سٽا_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_معلومات->mbcs.مٽا_سٽا_LFNLFrom_Unicode_بائيٽون);
    } وإلا {
        results=(uint16_t *)cnv->shared_معلومات->mbcs.from_Unicode_بائيٽون;
    }

    إذا(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_قدر=0x800;
    } وإلا {
        /* use only roundtrips and fallbacks from private-use characters */
        min_قدر=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_معلومات->mbcs.unicode_نقاب & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ذريعو_اشاريو= c==0 ? 0 : -1;
    ايندڙ_ذريعو_اشاريو=0;

    /* conversion loop */
    إذا(c!=0 && هدف_صلاحيت>0) {
        اذهب getTrail;
    }

    طالما(ذريعو<ذريعو_حد_بندي) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        إذا(هدف_صلاحيت>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ذريعو++;
            ++ايندڙ_ذريعو_اشاريو;
            إذا(U16_IS_SURROGATE(c)) {
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                    إذا(ذريعو<ذريعو_حد_بندي) {
                        /* test the following code unit */
                        char16_t trail=*ذريعو;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++ذريعو;
                            ++ايندڙ_ذريعو_اشاريو;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            إذا(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                /* callback(unassigned) */
                                اذهب unassigned;
                            }
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                            اكسر;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                    اكسر;
                }
            }

            /* convert the Unicode code point in c into codepage bytes */
            value=MBCS_SINGLE_نتيجو_FROM_U(جدول, results, c);

            /* is this code point assigned, or do we use fallbacks? */
            إذا(value>=min_قدر) {
                /* assigned, write the output character bytes from value and length */
                /* length==1 */
                /* this is easy because we know that there is enough space */
                *هدف++ = static_cast<uint8_t>(value);
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
                --هدف_صلاحيت;

                /* normal end of conversion: prepare for a new character */
                c=0;
                ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
            } وإلا { /* unassigned */
unassigned:
                /* try an extension mapping */
                p_دليلون->ذريعو=ذريعو;
                c=_extFromU(cnv, cnv->shared_معلومات,
                            c, &ذريعو, ذريعو_حد_بندي,
                            &هدف, هدف+هدف_صلاحيت,
                            &offsets, ذريعو_اشاريو,
                            p_دليلون->flush,
                            p_Error_ڪوڊ);
                ايندڙ_ذريعو_اشاريو += static_cast<int32_t>(ذريعو - p_دليلون->ذريعو);

                إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                    /* not mappable or buffer overflow */
                    اكسر;
                } وإلا {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    هدف_صلاحيت = static_cast<int32_t>(p_دليلون->هدف_حد_بندي - reinterpret_cast<حرف*>(هدف));

                    /* normal end of conversion: prepare for a new character */
                    ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                }
            }
        } وإلا {
            /* target is full */
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_دليلون->ذريعو=ذريعو;
    p_دليلون->هدف = reinterpret_cast<حرف*>(هدف);
    p_دليلون->offsets=offsets;
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
ساكن فراغ
ucnv_MBCSSingleFromBMPWithOffsets(UConverter_From_Unicode_دليلون *p_دليلون,
                              UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت char16_t *ذريعو, *ذريعو_حد_بندي, *آخري_ذريعو;
    uint8_t *هدف;
    int32_t هدف_صلاحيت, length;
    int32_t *offsets;

    ثابت uint16_t *جدول;
    ثابت uint16_t *results;

    UChar32 c;

    int32_t ذريعو_اشاريو;

    uint32_t asciiRoundtrips;
    uint16_t value, min_قدر;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    ذريعو=p_دليلون->ذريعو;
    ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
    هدف = reinterpret_cast<uint8_t*>(p_دليلون->هدف);
    هدف_صلاحيت = static_cast<int32_t>(p_دليلون->هدف_حد_بندي - p_دليلون->هدف);
    offsets=p_دليلون->offsets;

    جدول=cnv->shared_معلومات->mbcs.from_Unicode_جدول;
    إذا((cnv->options&UCNV_OPTION_مٽا_سٽا_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_معلومات->mbcs.مٽا_سٽا_LFNLFrom_Unicode_بائيٽون);
    } وإلا {
        results=(uint16_t *)cnv->shared_معلومات->mbcs.from_Unicode_بائيٽون;
    }
    asciiRoundtrips=cnv->shared_معلومات->mbcs.asciiRoundtrips;

    إذا(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_قدر=0x800;
    } وإلا {
        /* use only roundtrips and fallbacks from private-use characters */
        min_قدر=0xc00;
    }

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ذريعو_اشاريو= c==0 ? 0 : -1;
    آخري_ذريعو=ذريعو;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(ذريعو_حد_بندي - ذريعو);
    إذا(length<هدف_صلاحيت) {
        هدف_صلاحيت=length;
    }

    /* conversion loop */
    إذا(c!=0 && هدف_صلاحيت>0) {
        اذهب getTrail;
    }

#إذا MBCS_UNROLL_SINGLE_FROM_BMP
    /* unrolling makes it slower on Pentium III/Windows 2000?! */
    /* unroll the loop with the most common case */
unrolled:
    إذا(هدف_صلاحيت>=4) {
        int32_t count, loops;
        uint16_t anded_قدرون;

        loops=count=هدف_صلاحيت>>2;
        افعل {
            c=*ذريعو++;
            anded_قدرون=value=MBCS_SINGLE_نتيجو_FROM_U(جدول, results, c);
            *هدف++=(uint8_t)value;
            c=*ذريعو++;
            anded_قدرون&=value=MBCS_SINGLE_نتيجو_FROM_U(جدول, results, c);
            *هدف++=(uint8_t)value;
            c=*ذريعو++;
            anded_قدرون&=value=MBCS_SINGLE_نتيجو_FROM_U(جدول, results, c);
            *هدف++=(uint8_t)value;
            c=*ذريعو++;
            anded_قدرون&=value=MBCS_SINGLE_نتيجو_FROM_U(جدول, results, c);
            *هدف++=(uint8_t)value;

            /* were all 4 entries really valid? */
            إذا(anded_قدرون<min_قدر) {
                /* no, return to the first of these 4 */
                ذريعو-=4;
                هدف-=4;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        هدف_صلاحيت-=4*count;

        إذا(offsets!=خالي_اشارو) {
            آخري_ذريعو+=4*count;
            طالما(count>0) {
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                --count;
            }
        }

        c=0;
    }
#جيڪڏهن_پورو

    طالما(هدف_صلاحيت>0) {
        /*
         * Get a correct Unicode code point:
         * a single char16_t for a BMP code point or
         * a matched surrogate pair for a "supplementary code point".
         */
        c=*ذريعو++;
        /*
         * Do not immediately check for single surrogates:
         * Assume that they are unassigned and check for them in that case.
         * This speeds up the conversion of assigned characters.
         */
        /* convert the Unicode code point in c into codepage bytes */
        إذا(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
            *هدف++ = static_cast<uint8_t>(c);
            --هدف_صلاحيت;
            c=0;
            استمر;
        }
        value=MBCS_SINGLE_نتيجو_FROM_U(جدول, results, c);
        /* is this code point assigned, or do we use fallbacks? */
        إذا(value>=min_قدر) {
            /* assigned, write the output character bytes from value and length */
            /* length==1 */
            /* this is easy because we know that there is enough space */
            *هدف++ = static_cast<uint8_t>(value);
            --هدف_صلاحيت;

            /* normal end of conversion: prepare for a new character */
            c=0;
            استمر;
        } وإلا إذا(!U16_IS_SURROGATE(c)) {
            /* normal, unassigned BMP character */
        } وإلا إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
            إذا(ذريعو<ذريعو_حد_بندي) {
                /* test the following code unit */
                char16_t trail=*ذريعو;
                إذا(U16_IS_TRAIL(trail)) {
                    ++ذريعو;
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } وإلا {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                    اكسر;
                }
            } وإلا {
                /* no more input */
                إذا (p_دليلون->flush) {
                    *p_Error_ڪوڊ=U_TRUNCATED_CHAR_مليو;
                }
                اكسر;
            }
        } وإلا {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
            *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
            اكسر;
        }

        /* c does not have a mapping */

        /* get the number of code units for c to correctly advance sourceIndex */
        length=U16_ڊيگهائي(c);

        /* set offsets since the start or the last extension */
        إذا(offsets!=خالي_اشارو) {
            int32_t count = static_cast<int32_t>(ذريعو - آخري_ذريعو);

            /* do not set the offset for this character */
            count-=length;

            طالما(count>0) {
                *offsets++=ذريعو_اشاريو++;
                --count;
            }
            /* offsets and sourceIndex are now set for the current character */
        }

        /* try an extension mapping */
        آخري_ذريعو=ذريعو;
        c=_extFromU(cnv, cnv->shared_معلومات,
                    c, &ذريعو, ذريعو_حد_بندي,
                    &هدف, reinterpret_cast<ثابت uint8_t*>(p_دليلون->هدف_حد_بندي),
                    &offsets, ذريعو_اشاريو,
                    p_دليلون->flush,
                    p_Error_ڪوڊ);
        ذريعو_اشاريو += length + static_cast<int32_t>(ذريعو - آخري_ذريعو);
        آخري_ذريعو=ذريعو;

        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            /* not mappable or buffer overflow */
            اكسر;
        } وإلا {
            /* a mapping was written to the target, continue */

            /* recalculate the targetCapacity after an extension mapping */
            هدف_صلاحيت = static_cast<int32_t>(p_دليلون->هدف_حد_بندي - reinterpret_cast<حرف*>(هدف));
            length = static_cast<int32_t>(ذريعو_حد_بندي - ذريعو);
            إذا(length<هدف_صلاحيت) {
                هدف_صلاحيت=length;
            }
        }

#إذا MBCS_UNROLL_SINGLE_FROM_BMP
        /* unrolling makes it slower on Pentium III/Windows 2000?! */
        اذهب unrolled;
#جيڪڏهن_پورو
    }

    إذا(U_SUCCESS(*p_Error_ڪوڊ) && ذريعو<ذريعو_حد_بندي && هدف>=(uint8_t *)p_دليلون->هدف_حد_بندي) {
        /* target is full */
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
    }

    /* set offsets since the start or the last callback */
    إذا(offsets!=خالي_اشارو) {
        size_t count=ذريعو-آخري_ذريعو;
        إذا (count > 0 && *p_Error_ڪوڊ == U_TRUNCATED_CHAR_مليو) {
            /*
            Caller gave us a partial supplementary character,
            which this function couldn't convert in any case.
            The callback will handle the offset.
            */
            count--;
        }
        طالما(count>0) {
            *offsets++=ذريعو_اشاريو++;
            --count;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_دليلون->ذريعو=ذريعو;
    p_دليلون->هدف = reinterpret_cast<حرف*>(هدف);
    p_دليلون->offsets=offsets;
}

U_CFUNC فراغ
ucnv_MBCSFromUnicodeWithOffsets(UConverter_From_Unicode_دليلون *p_دليلون,
                            UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت char16_t *ذريعو, *ذريعو_حد_بندي;
    uint8_t *هدف;
    int32_t هدف_صلاحيت;
    int32_t *offsets;

    ثابت uint16_t *جدول;
    ثابت uint16_t *mbcs_اشاريو;
    ثابت uint8_t *p, *بائيٽون;
    uint8_t outputType;

    UChar32 c;

    int32_t prev_ذريعو_اشاريو, ذريعو_اشاريو, ايندڙ_ذريعو_اشاريو;

    uint32_t stage2_داخلا;
    uint32_t asciiRoundtrips;
    uint32_t value;
    /* Shift-In and Shift-Out byte sequences differ by encoding scheme. */
    uint8_t si_بائيٽون[2] = {0, 0};
    uint8_t so_بائيٽون[2] = {0, 0};
    uint8_t si_ڊيگهائي, so_ڊيگهائي;
    int32_t length = 0, prev_ڊيگهائي;
    uint8_t unicode_نقاب;

    cnv=p_دليلون->converter;

    إذا(cnv->preFromUFirstCP>=0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_ميلاپ_From_U(cnv, p_دليلون, -1, p_Error_ڪوڊ);

        إذا(U_FAILURE(*p_Error_ڪوڊ) || cnv->preFromULength<0) {
            أرجع;
        }
    }

    /* use optimized function if possible */
    outputType=cnv->shared_معلومات->mbcs.outputType;
    unicode_نقاب=cnv->shared_معلومات->mbcs.unicode_نقاب;
    إذا(outputType==MBCS_OUTPUT_1 && !(unicode_نقاب&UCNV_HAS_SURROGATES)) {
        إذا(!(unicode_نقاب&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingleFromBMPWithOffsets(p_دليلون, p_Error_ڪوڊ);
        } وإلا {
            ucnv_MBCSSingleFromUnicodeWithOffsets(p_دليلون, p_Error_ڪوڊ);
        }
        أرجع;
    } وإلا إذا(outputType==MBCS_OUTPUT_2 && cnv->shared_معلومات->mbcs.utf8Friendly) {
        ucnv_MBCSDoubleFromUnicodeWithOffsets(p_دليلون, p_Error_ڪوڊ);
        أرجع;
    }

    /* set up the local pointers */
    ذريعو=p_دليلون->ذريعو;
    ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
    هدف=(uint8_t *)p_دليلون->هدف;
    هدف_صلاحيت=(int32_t)(p_دليلون->هدف_حد_بندي-p_دليلون->هدف);
    offsets=p_دليلون->offsets;

    جدول=cnv->shared_معلومات->mbcs.from_Unicode_جدول;
    إذا(cnv->shared_معلومات->mbcs.utf8Friendly) {
        mbcs_اشاريو=cnv->shared_معلومات->mbcs.mbcs_اشاريو;
    } وإلا {
        mbcs_اشاريو=خالي_اشارو;
    }
    إذا((cnv->options&UCNV_OPTION_مٽا_سٽا_LFNL)!=0) {
        بائيٽون=cnv->shared_معلومات->mbcs.مٽا_سٽا_LFNLFrom_Unicode_بائيٽون;
    } وإلا {
        بائيٽون=cnv->shared_معلومات->mbcs.from_Unicode_بائيٽون;
    }
    asciiRoundtrips=cnv->shared_معلومات->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    إذا(outputType==MBCS_OUTPUT_2_SISO) {
        prev_ڊيگهائي=cnv->from_Unicode_حالت;
        إذا(prev_ڊيگهائي==0) {
            /* set the real value */
            prev_ڊيگهائي=1;
        }
    } وإلا {
        /* prevent fromUnicodeStatus from being set to something non-0 */
        prev_ڊيگهائي=0;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    prev_ذريعو_اشاريو=-1;
    ذريعو_اشاريو= c==0 ? 0 : -1;
    ايندڙ_ذريعو_اشاريو=0;

    /* Get the SI/SO character for the converter */
    si_ڊيگهائي = static_cast<uint8_t>(getSISOBytes(SI, cnv->options, si_بائيٽون));
    so_ڊيگهائي = static_cast<uint8_t>(getSISOBytes(SO, cnv->options, so_بائيٽون));

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
    إذا(c!=0 && هدف_صلاحيت>0) {
        اذهب getTrail;
    }

    طالما(ذريعو<ذريعو_حد_بندي) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        إذا(هدف_صلاحيت>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ذريعو++;
            ++ايندڙ_ذريعو_اشاريو;
            إذا(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *هدف++=(uint8_t)c;
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                    prev_ذريعو_اشاريو=ذريعو_اشاريو;
                    ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                }
                --هدف_صلاحيت;
                c=0;
                استمر;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            إذا(c<=0xd7ff && mbcs_اشاريو!=خالي_اشارو) {
                value=mbcs_اشاريو[c>>6];

                /* get the bytes and the length for the output (copied from below and adapted for utf8Friendly data) */
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                بدل(outputType) {
                حالة MBCS_OUTPUT_2:
                    value=((ثابت uint16_t *)بائيٽون)[value +(c&0x3f)];
                    إذا(value<=0xff) {
                        إذا(value==0) {
                            اذهب unassigned;
                        } وإلا {
                            length=1;
                        }
                    } وإلا {
                        length=2;
                    }
                    اكسر;
                حالة MBCS_OUTPUT_2_SISO:
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
                    cnv->from_Unicode_حالت=prev_ڊيگهائي; /* save the old state */
                    value=((ثابت uint16_t *)بائيٽون)[value +(c&0x3f)];
                    إذا(value<=0xff) {
                        إذا(value==0) {
                            اذهب unassigned;
                        } وإلا إذا(prev_ڊيگهائي<=1) {
                            length=1;
                        } وإلا {
                            /* change from double-byte mode to single-byte */
                            إذا (si_ڊيگهائي == 1) {
                                value|=(uint32_t)si_بائيٽون[0]<<8;
                                length = 2;
                            } وإلا إذا (si_ڊيگهائي == 2) {
                                value|=(uint32_t)si_بائيٽون[1]<<8;
                                value|=(uint32_t)si_بائيٽون[0]<<16;
                                length = 3;
                            }
                            prev_ڊيگهائي=1;
                        }
                    } وإلا {
                        إذا(prev_ڊيگهائي==2) {
                            length=2;
                        } وإلا {
                            /* change from single-byte mode to double-byte */
                            إذا (so_ڊيگهائي == 1) {
                                value|=(uint32_t)so_بائيٽون[0]<<16;
                                length = 3;
                            } وإلا إذا (so_ڊيگهائي == 2) {
                                value|=(uint32_t)so_بائيٽون[1]<<16;
                                value|=(uint32_t)so_بائيٽون[0]<<24;
                                length = 4;
                            }
                            prev_ڊيگهائي=2;
                        }
                    }
                    اكسر;
                حالة MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=((ثابت uint16_t *)بائيٽون)[value +(c&0x3f)];
                    إذا(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        اذهب unassigned;
                    } وإلا {
                        length=2;
                    }
                    اكسر;
                حالة MBCS_OUTPUT_3:
                    p=بائيٽون+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    إذا(value<=0xff) {
                        إذا(value==0) {
                            اذهب unassigned;
                        } وإلا {
                            length=1;
                        }
                    } وإلا إذا(value<=0xffff) {
                        length=2;
                    } وإلا {
                        length=3;
                    }
                    اكسر;
                حالة MBCS_OUTPUT_4:
                    value=((ثابت uint32_t *)بائيٽون)[value +(c&0x3f)];
                    إذا(value<=0xff) {
                        إذا(value==0) {
                            اذهب unassigned;
                        } وإلا {
                            length=1;
                        }
                    } وإلا إذا(value<=0xffff) {
                        length=2;
                    } وإلا إذا(value<=0xffffff) {
                        length=3;
                    } وإلا {
                        length=4;
                    }
                    اكسر;
                حالة MBCS_OUTPUT_3_EUC:
                    value=((ثابت uint16_t *)بائيٽون)[value +(c&0x3f)];
                    /* EUC 16-bit fixed-length representation */
                    إذا(value<=0xff) {
                        إذا(value==0) {
                            اذهب unassigned;
                        } وإلا {
                            length=1;
                        }
                    } وإلا إذا((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } وإلا إذا((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } وإلا {
                        length=2;
                    }
                    اكسر;
                حالة MBCS_OUTPUT_4_EUC:
                    p=بائيٽون+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    إذا(value<=0xff) {
                        إذا(value==0) {
                            اذهب unassigned;
                        } وإلا {
                            length=1;
                        }
                    } وإلا إذا(value<=0xffff) {
                        length=2;
                    } وإلا إذا((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } وإلا إذا((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } وإلا {
                        length=3;
                    }
                    اكسر;
                افتراضي:
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
                    اكسر;
                }
                /* output the value */
            } وإلا {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                إذا(U16_IS_SURROGATE(c) && !(unicode_نقاب&UCNV_HAS_SURROGATES)) {
                    إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        إذا(ذريعو<ذريعو_حد_بندي) {
                            /* test the following code unit */
                            char16_t trail=*ذريعو;
                            إذا(U16_IS_TRAIL(trail)) {
                                ++ذريعو;
                                ++ايندڙ_ذريعو_اشاريو;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                إذا(!(unicode_نقاب&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    cnv->from_Unicode_حالت=prev_ڊيگهائي; /* save the old state */
                                    /* callback(unassigned) */
                                    اذهب unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } وإلا {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                                اكسر;
                            }
                        } وإلا {
                            /* no more input */
                            اكسر;
                        }
                    } وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                        اكسر;
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
                stage2_داخلا=MBCS_STAGE_2_FROM_U(جدول, c);

                /* get the bytes and the length for the output */
                بدل(outputType) {
                حالة MBCS_OUTPUT_2:
                    value=MBCS_قدر_2_FROM_STAGE_2(بائيٽون, stage2_داخلا, c);
                    إذا(value<=0xff) {
                        length=1;
                    } وإلا {
                        length=2;
                    }
                    اكسر;
                حالة MBCS_OUTPUT_2_SISO:
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
                    cnv->from_Unicode_حالت=prev_ڊيگهائي; /* save the old state */
                    value=MBCS_قدر_2_FROM_STAGE_2(بائيٽون, stage2_داخلا, c);
                    إذا(value<=0xff) {
                        إذا(value==0 && MBCS_FROM_U_IS_ROUNDTRIP(stage2_داخلا, c)==0) {
                            /* no mapping, leave value==0 */
                            length=0;
                        } وإلا إذا(prev_ڊيگهائي<=1) {
                            length=1;
                        } وإلا {
                            /* change from double-byte mode to single-byte */
                            إذا (si_ڊيگهائي == 1) {
                                value|=(uint32_t)si_بائيٽون[0]<<8;
                                length = 2;
                            } وإلا إذا (si_ڊيگهائي == 2) {
                                value|=(uint32_t)si_بائيٽون[1]<<8;
                                value|=(uint32_t)si_بائيٽون[0]<<16;
                                length = 3;
                            }
                            prev_ڊيگهائي=1;
                        }
                    } وإلا {
                        إذا(prev_ڊيگهائي==2) {
                            length=2;
                        } وإلا {
                            /* change from single-byte mode to double-byte */
                            إذا (so_ڊيگهائي == 1) {
                                value|=(uint32_t)so_بائيٽون[0]<<16;
                                length = 3;
                            } وإلا إذا (so_ڊيگهائي == 2) {
                                value|=(uint32_t)so_بائيٽون[1]<<16;
                                value|=(uint32_t)so_بائيٽون[0]<<24;
                                length = 4;
                            }
                            prev_ڊيگهائي=2;
                        }
                    }
                    اكسر;
                حالة MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=MBCS_قدر_2_FROM_STAGE_2(بائيٽون, stage2_داخلا, c);
                    إذا(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        value=stage2_داخلا=0; /* stage2Entry=0 to reset roundtrip flags */
                        length=0;
                    } وإلا {
                        length=2;
                    }
                    اكسر;
                حالة MBCS_OUTPUT_3:
                    p=MBCS_اشارو_3_FROM_STAGE_2(بائيٽون, stage2_داخلا, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    إذا(value<=0xff) {
                        length=1;
                    } وإلا إذا(value<=0xffff) {
                        length=2;
                    } وإلا {
                        length=3;
                    }
                    اكسر;
                حالة MBCS_OUTPUT_4:
                    value=MBCS_قدر_4_FROM_STAGE_2(بائيٽون, stage2_داخلا, c);
                    إذا(value<=0xff) {
                        length=1;
                    } وإلا إذا(value<=0xffff) {
                        length=2;
                    } وإلا إذا(value<=0xffffff) {
                        length=3;
                    } وإلا {
                        length=4;
                    }
                    اكسر;
                حالة MBCS_OUTPUT_3_EUC:
                    value=MBCS_قدر_2_FROM_STAGE_2(بائيٽون, stage2_داخلا, c);
                    /* EUC 16-bit fixed-length representation */
                    إذا(value<=0xff) {
                        length=1;
                    } وإلا إذا((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } وإلا إذا((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } وإلا {
                        length=2;
                    }
                    اكسر;
                حالة MBCS_OUTPUT_4_EUC:
                    p=MBCS_اشارو_3_FROM_STAGE_2(بائيٽون, stage2_داخلا, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    إذا(value<=0xff) {
                        length=1;
                    } وإلا إذا(value<=0xffff) {
                        length=2;
                    } وإلا إذا((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } وإلا إذا((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } وإلا {
                        length=3;
                    }
                    اكسر;
                افتراضي:
                    /* must not occur */
                    /*
                     * To avoid compiler warnings that value & length may be
                     * used without having been initialized, we set them here.
                     * In reality, this is unreachable code.
                     * Not having a default branch also causes warnings with
                     * some compilers.
                     */
                    value=stage2_داخلا=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                    اكسر;
                }

                /* is this code point assigned, or do we use fallbacks? */
                إذا(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_داخلا, c)!=0 ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_دليلون->ذريعو=ذريعو;
                    c=_extFromU(cnv, cnv->shared_معلومات,
                                c, &ذريعو, ذريعو_حد_بندي,
                                &هدف, هدف+هدف_صلاحيت,
                                &offsets, ذريعو_اشاريو,
                                p_دليلون->flush,
                                p_Error_ڪوڊ);
                    ايندڙ_ذريعو_اشاريو+=(int32_t)(ذريعو-p_دليلون->ذريعو);
                    prev_ڊيگهائي=cnv->from_Unicode_حالت; /* restore SISO state */

                    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                        /* not mappable or buffer overflow */
                        اكسر;
                    } وإلا {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        هدف_صلاحيت=(int32_t)(p_دليلون->هدف_حد_بندي-(حرف *)هدف);

                        /* normal end of conversion: prepare for a new character */
                        إذا(offsets!=خالي_اشارو) {
                            prev_ذريعو_اشاريو=ذريعو_اشاريو;
                            ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                        }
                        استمر;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            إذا(length<=هدف_صلاحيت) {
                إذا(offsets==خالي_اشارو) {
                    بدل(length) {
                        /* each branch falls through to the next one */
                    حالة 4:
                        *هدف++=(uint8_t)(value>>24);
                        U_FALLTHROUGH;
                    حالة 3:
                        *هدف++=(uint8_t)(value>>16);
                        U_FALLTHROUGH;
                    حالة 2:
                        *هدف++=(uint8_t)(value>>8);
                        U_FALLTHROUGH;
                    حالة 1:
                        *هدف++=(uint8_t)value;
                        U_FALLTHROUGH;
                    افتراضي:
                        /* will never occur */
                        اكسر;
                    }
                } وإلا {
                    بدل(length) {
                        /* each branch falls through to the next one */
                    حالة 4:
                        *هدف++=(uint8_t)(value>>24);
                        *offsets++=ذريعو_اشاريو;
                        U_FALLTHROUGH;
                    حالة 3:
                        *هدف++=(uint8_t)(value>>16);
                        *offsets++=ذريعو_اشاريو;
                        U_FALLTHROUGH;
                    حالة 2:
                        *هدف++=(uint8_t)(value>>8);
                        *offsets++=ذريعو_اشاريو;
                        U_FALLTHROUGH;
                    حالة 1:
                        *هدف++=(uint8_t)value;
                        *offsets++=ذريعو_اشاريو;
                        U_FALLTHROUGH;
                    افتراضي:
                        /* will never occur */
                        اكسر;
                    }
                }
                هدف_صلاحيت-=length;
            } وإلا {
                uint8_t *char_Error_بفر;

                /*
                 * We actually do this backwards here:
                 * In order to save an intermediate variable, we output
                 * first to the overflow buffer what does not fit into the
                 * regular target.
                 */
                /* we know that 1<=targetCapacity<length<=4 */
                length-=هدف_صلاحيت;
                char_Error_بفر=(uint8_t *)cnv->char_Error_بفر;
                بدل(length) {
                    /* each branch falls through to the next one */
                حالة 3:
                    *char_Error_بفر++=(uint8_t)(value>>16);
                    U_FALLTHROUGH;
                حالة 2:
                    *char_Error_بفر++=(uint8_t)(value>>8);
                    U_FALLTHROUGH;
                حالة 1:
                    *char_Error_بفر=(uint8_t)value;
                    U_FALLTHROUGH;
                افتراضي:
                    /* will never occur */
                    اكسر;
                }
                cnv->char_Error_بفر_ڊيگهائي=(int8_t)length;

                /* now output what fits into the regular target */
                value>>=8*length; /* length was reduced by targetCapacity */
                بدل(هدف_صلاحيت) {
                    /* each branch falls through to the next one */
                حالة 3:
                    *هدف++=(uint8_t)(value>>16);
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                    }
                    U_FALLTHROUGH;
                حالة 2:
                    *هدف++=(uint8_t)(value>>8);
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                    }
                    U_FALLTHROUGH;
                حالة 1:
                    *هدف++=(uint8_t)value;
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                    }
                    U_FALLTHROUGH;
                افتراضي:
                    /* will never occur */
                    اكسر;
                }

                /* target overflow */
                هدف_صلاحيت=0;
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                c=0;
                اكسر;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            إذا(offsets!=خالي_اشارو) {
                prev_ذريعو_اشاريو=ذريعو_اشاريو;
                ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
            }
            استمر;
        } وإلا {
            /* target is full */
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
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
    إذا( U_SUCCESS(*p_Error_ڪوڊ) &&
        outputType==MBCS_OUTPUT_2_SISO && prev_ڊيگهائي==2 &&
        p_دليلون->flush && ذريعو>=ذريعو_حد_بندي && c==0
    ) {
        /* EBCDIC_STATEFUL ending with DBCS: emit an SI to return the output stream to SBCS */
        إذا(هدف_صلاحيت>0) {
            *هدف++ = si_بائيٽون[0];
            إذا (si_ڊيگهائي == 2) {
                إذا (هدف_صلاحيت<2) {
                    cnv->char_Error_بفر[0] = si_بائيٽون[1];
                    cnv->char_Error_بفر_ڊيگهائي=1;
                    *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                } وإلا {
                    *هدف++ = si_بائيٽون[1];
                }
            }
            إذا(offsets!=خالي_اشارو) {
                /* set the last source character's index (sourceIndex points at sourceLimit now) */
                *offsets++=prev_ذريعو_اشاريو;
            }
        } وإلا {
            /* target is full */
            cnv->char_Error_بفر[0] = si_بائيٽون[0];
            إذا (si_ڊيگهائي == 2) {
                cnv->char_Error_بفر[1] = si_بائيٽون[1];
            }
            cnv->char_Error_بفر_ڊيگهائي=si_ڊيگهائي;
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
        }
        prev_ڊيگهائي=1; /* we switched into SBCS */
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;
    cnv->from_Unicode_حالت=prev_ڊيگهائي;

    /* write back the updated pointers */
    p_دليلون->ذريعو=ذريعو;
    p_دليلون->هدف=(حرف *)هدف;
    p_دليلون->offsets=offsets;
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
ucnv_MBCSFromUChar32(UConverter_Shared_معلومات *shared_معلومات,
                 UChar32 c, uint32_t *p_قدر,
                 UBool useFallback) {
    ثابت int32_t *cx;
    ثابت uint16_t *جدول;
#إذا 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
    ثابت uint8_t *p;
#جيڪڏهن_پورو
    uint32_t stage2_داخلا;
    uint32_t value;
    int32_t length;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    إذا(c<=0xffff || (shared_معلومات->mbcs.unicode_نقاب&UCNV_HAS_SUPPLEMENTARY)) {
        جدول=shared_معلومات->mbcs.from_Unicode_جدول;

        /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
        إذا(shared_معلومات->mbcs.outputType==MBCS_OUTPUT_1) {
            value=MBCS_SINGLE_نتيجو_FROM_U(جدول, (uint16_t *)shared_معلومات->mbcs.from_Unicode_بائيٽون, c);
            /* is this code point assigned, or do we use fallbacks? */
            إذا(useFallback ? value>=0x800 : value>=0xc00) {
                *p_قدر=value&0xff;
                أرجع 1;
            }
        } وإلا /* outputType!=MBCS_OUTPUT_1 */ {
            stage2_داخلا=MBCS_STAGE_2_FROM_U(جدول, c);

            /* get the bytes and the length for the output */
            بدل(shared_معلومات->mbcs.outputType) {
            حالة MBCS_OUTPUT_2:
                value=MBCS_قدر_2_FROM_STAGE_2(shared_معلومات->mbcs.from_Unicode_بائيٽون, stage2_داخلا, c);
                إذا(value<=0xff) {
                    length=1;
                } وإلا {
                    length=2;
                }
                اكسر;
#إذا 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
            حالة MBCS_OUTPUT_DBCS_ONLY:
                /* table with single-byte results, but only DBCS mappings used */
                value=MBCS_قدر_2_FROM_STAGE_2(shared_معلومات->mbcs.from_Unicode_بائيٽون, stage2_داخلا, c);
                إذا(value<=0xff) {
                    /* no mapping or SBCS result, not taken for DBCS-only */
                    value=stage2_داخلا=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                } وإلا {
                    length=2;
                }
                اكسر;
            حالة MBCS_OUTPUT_3:
                p=MBCS_اشارو_3_FROM_STAGE_2(shared_معلومات->mbcs.from_Unicode_بائيٽون, stage2_داخلا, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                إذا(value<=0xff) {
                    length=1;
                } وإلا إذا(value<=0xffff) {
                    length=2;
                } وإلا {
                    length=3;
                }
                اكسر;
            حالة MBCS_OUTPUT_4:
                value=MBCS_قدر_4_FROM_STAGE_2(shared_معلومات->mbcs.from_Unicode_بائيٽون, stage2_داخلا, c);
                إذا(value<=0xff) {
                    length=1;
                } وإلا إذا(value<=0xffff) {
                    length=2;
                } وإلا إذا(value<=0xffffff) {
                    length=3;
                } وإلا {
                    length=4;
                }
                اكسر;
            حالة MBCS_OUTPUT_3_EUC:
                value=MBCS_قدر_2_FROM_STAGE_2(shared_معلومات->mbcs.from_Unicode_بائيٽون, stage2_داخلا, c);
                /* EUC 16-bit fixed-length representation */
                إذا(value<=0xff) {
                    length=1;
                } وإلا إذا((value&0x8000)==0) {
                    value|=0x8e8000;
                    length=3;
                } وإلا إذا((value&0x80)==0) {
                    value|=0x8f0080;
                    length=3;
                } وإلا {
                    length=2;
                }
                اكسر;
            حالة MBCS_OUTPUT_4_EUC:
                p=MBCS_اشارو_3_FROM_STAGE_2(shared_معلومات->mbcs.from_Unicode_بائيٽون, stage2_داخلا, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                /* EUC 16-bit fixed-length representation applied to the first two bytes */
                إذا(value<=0xff) {
                    length=1;
                } وإلا إذا(value<=0xffff) {
                    length=2;
                } وإلا إذا((value&0x800000)==0) {
                    value|=0x8e800000;
                    length=4;
                } وإلا إذا((value&0x8000)==0) {
                    value|=0x8f008000;
                    length=4;
                } وإلا {
                    length=3;
                }
                اكسر;
#جيڪڏهن_پورو
            افتراضي:
                /* must not occur */
                أرجع -1;
            }

            /* is this code point assigned, or do we use fallbacks? */
            إذا( MBCS_FROM_U_IS_ROUNDTRIP(stage2_داخلا, c) ||
                (FROM_U_USE_FALLBACK(useFallback, c) && value!=0)
            ) {
                /*
                 * We allow a 0 byte output if the "assigned" bit is set for this entry.
                 * There is no way with this data structure for fallback output
                 * to be a zero byte.
                 */
                /* assigned */
                *p_قدر=value;
                أرجع length;
            }
        }
    }

    cx=shared_معلومات->mbcs.extIndexes;
    إذا(cx!=خالي_اشارو) {
        length=ucnv_ext_Simple_ميلاپ_From_U(cx, c, p_قدر, useFallback);
        أرجع length>=0 ? length : -length;  /* return abs(length); */
    }

    /* unassigned */
    أرجع 0;
}


#إذا 0
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
ucnv_MBCSSingleFromUChar32(UConverter_Shared_معلومات *shared_معلومات,
                       UChar32 c,
                       UBool useFallback) {
    ثابت uint16_t *جدول;
    int32_t value;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    إذا(c>=0x10000 && !(shared_معلومات->mbcs.unicode_نقاب&UCNV_HAS_SUPPLEMENTARY)) {
        أرجع -1;
    }

    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    جدول=shared_معلومات->mbcs.from_Unicode_جدول;

    /* get the byte for the output */
    value=MBCS_SINGLE_نتيجو_FROM_U(جدول, (uint16_t *)shared_معلومات->mbcs.from_Unicode_بائيٽون, c);
    /* is this code point assigned, or do we use fallbacks? */
    إذا(useFallback ? value>=0x800 : value>=0xc00) {
        أرجع value&0xff;
    } وإلا {
        أرجع -1;
    }
}
#جيڪڏهن_پورو

/* MBCS-from-UTF-8 conversion functions ------------------------------------- */

/* offsets for n-byte UTF-8 sequences that were calculated with ((lead<<6)+trail)<<6+trail... */
ساكن ثابت UChar32
utf8_offsets[5]={ 0, 0, 0x3080, 0xE2080, 0x3C82080 };

ساكن فراغ U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_دليلون *pFromUArgs,
                  UConverter_منزل_Unicode_دليلون *p_منزل_UArgs,
                  UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *utf8, *cnv;
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي;
    uint8_t *هدف;
    int32_t هدف_صلاحيت;

    ثابت uint16_t *جدول, *sbcs_اشاريو;
    ثابت uint16_t *results;

    int8_t old_منزل_ULength, منزل_ULength, منزل_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t asciiRoundtrips;
    uint16_t value, min_قدر = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_منزل_UArgs->converter;
    cnv=pFromUArgs->converter;
    ذريعو=(uint8_t *)p_منزل_UArgs->ذريعو;
    ذريعو_حد_بندي=(uint8_t *)p_منزل_UArgs->ذريعو_حد_بندي;
    هدف = reinterpret_cast<uint8_t*>(pFromUArgs->هدف);
    هدف_صلاحيت = static_cast<int32_t>(pFromUArgs->هدف_حد_بندي - pFromUArgs->هدف);

    جدول=cnv->shared_معلومات->mbcs.from_Unicode_جدول;
    sbcs_اشاريو=cnv->shared_معلومات->mbcs.sbcs_اشاريو;
    إذا((cnv->options&UCNV_OPTION_مٽا_سٽا_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_معلومات->mbcs.مٽا_سٽا_LFNLFrom_Unicode_بائيٽون);
    } وإلا {
        results=(uint16_t *)cnv->shared_معلومات->mbcs.from_Unicode_بائيٽون;
    }
    asciiRoundtrips=cnv->shared_معلومات->mbcs.asciiRoundtrips;

    إذا(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_قدر=0x800;
    } وإلا {
        /* use only roundtrips and fallbacks from private-use characters */
        min_قدر=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_معلومات->mbcs.unicode_نقاب & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    إذا(utf8->منزل_ULength > 0) {
        منزل_ULength=old_منزل_ULength=utf8->منزل_ULength;
        منزل_ULimit = static_cast<int8_t>(utf8->گهڻائي);
        c = static_cast<UChar32>(utf8->منزل_Unicode_حالت);
    } وإلا {
        منزل_ULength=old_منزل_ULength=منزل_ULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(ذريعو_حد_بندي - ذريعو) - (منزل_ULimit - old_منزل_ULength);
        إذا(length>0) {
            uint8_t b1=*(ذريعو_حد_بندي-1);
            إذا(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } وإلا إذا(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(ذريعو_حد_بندي-2);
                إذا(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    ذريعو_حد_بندي-=2;
                }
            } وإلا إذا(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --ذريعو_حد_بندي;
            }
        }
    }

    إذا(c!=0 && هدف_صلاحيت>0) {
        utf8->منزل_Unicode_حالت=0;
        utf8->منزل_ULength=0;
        اذهب وڌيڪ_بائيٽون;
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
    طالما(ذريعو<ذريعو_حد_بندي) {
        إذا(هدف_صلاحيت>0) {
            b=*ذريعو++;
            إذا(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                إذا(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *هدف++ = b;
                    --هدف_صلاحيت;
                    استمر;
                } وإلا {
                    c=b;
                    value=SBCS_نتيجو_FROM_UTF8(sbcs_اشاريو, results, 0, c);
                }
            } وإلا {
                إذا(b<0xe0) {
                    إذا( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*ذريعو - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++ذريعو;
                        value=SBCS_نتيجو_FROM_UTF8(sbcs_اشاريو, results, c, t1);
                        إذا(value>=min_قدر) {
                            *هدف++ = static_cast<uint8_t>(value);
                            --هدف_صلاحيت;
                            استمر;
                        } وإلا {
                            c=(c<<6)|t1;
                        }
                    } وإلا {
                        c=-1;
                    }
                } وإلا إذا(b==0xe0) {
                    إذا( /* handle U+0800..U+0FFF inline */
                        (t1 = static_cast<uint8_t>(ذريعو[0] - 0x80)) <= 0x3f && t1 >= 0x20 &&
                        (t2 = static_cast<uint8_t>(ذريعو[1] - 0x80)) <= 0x3f
                    ) {
                        c=t1;
                        ذريعو+=2;
                        value=SBCS_نتيجو_FROM_UTF8(sbcs_اشاريو, results, c, t2);
                        إذا(value>=min_قدر) {
                            *هدف++ = static_cast<uint8_t>(value);
                            --هدف_صلاحيت;
                            استمر;
                        } وإلا {
                            c=(c<<6)|t2;
                        }
                    } وإلا {
                        c=-1;
                    }
                } وإلا {
                    c=-1;
                }

                إذا(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    old_منزل_ULength=0;
                    منزل_ULength=1;
                    منزل_ULimit=U8_ڳڻپ_بائيٽون_NON_ASCII(b);
                    c=b;
وڌيڪ_بائيٽون:
                    طالما(منزل_ULength<منزل_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        إذا(ذريعو<(uint8_t *)p_منزل_UArgs->ذريعو_حد_بندي) {
                            b=*ذريعو;
                            إذا(icu::UTF8::isValidTrail(c, b, منزل_ULength, منزل_ULimit)) {
                                ++ذريعو;
                                ++منزل_ULength;
                                c=(c<<6)+b;
                            } وإلا {
                                اكسر; /* sequence too short, stop with toULength<toULimit */
                            }
                        } وإلا {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            ذريعو-=(منزل_ULength-old_منزل_ULength);
                            طالما(old_منزل_ULength<منزل_ULength) {
                                utf8->منزل_UBytes[old_منزل_ULength++]=*ذريعو++;
                            }
                            utf8->منزل_Unicode_حالت=c;
                            utf8->منزل_ULength=منزل_ULength;
                            utf8->گهڻائي=منزل_ULimit;
                            p_منزل_UArgs->ذريعو=(حرف *)ذريعو;
                            pFromUArgs->هدف = reinterpret_cast<حرف*>(هدف);
                            أرجع;
                        }
                    }

                    إذا(منزل_ULength==منزل_ULimit) {
                        c-=utf8_offsets[منزل_ULength];
                        إذا(منزل_ULength<=3) {  /* BMP */
                            value=MBCS_SINGLE_نتيجو_FROM_U(جدول, results, c);
                        } وإلا {
                            /* supplementary code point */
                            إذا(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                value=0;
                            } وإلا {
                                value=MBCS_SINGLE_نتيجو_FROM_U(جدول, results, c);
                            }
                        }
                    } وإلا {
                        /* error handling: illegal UTF-8 byte sequence */
                        ذريعو-=(منزل_ULength-old_منزل_ULength);
                        طالما(old_منزل_ULength<منزل_ULength) {
                            utf8->منزل_UBytes[old_منزل_ULength++]=*ذريعو++;
                        }
                        utf8->منزل_ULength=منزل_ULength;
                        p_منزل_UArgs->ذريعو=(حرف *)ذريعو;
                        pFromUArgs->هدف = reinterpret_cast<حرف*>(هدف);
                        *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                        أرجع;
                    }
                }
            }

            إذا(value>=min_قدر) {
                /* output the mapping for c */
                *هدف++ = static_cast<uint8_t>(value);
                --هدف_صلاحيت;
            } وإلا {
                /* value<minValue means c is unassigned (unmappable) */
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                ساكن ثابت char16_t nul=0;
                ثابت char16_t *no_ذريعو=&nul;
                c=_extFromU(cnv, cnv->shared_معلومات,
                            c, &no_ذريعو, no_ذريعو,
                            &هدف, هدف+هدف_صلاحيت,
                            خالي_اشارو, -1,
                            pFromUArgs->flush,
                            p_Error_ڪوڊ);

                إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    اكسر;
                } وإلا إذا(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *p_Error_ڪوڊ=U_USING_DEFAULT_WARNING;
                    اكسر;
                } وإلا {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    هدف_صلاحيت = static_cast<int32_t>(pFromUArgs->هدف_حد_بندي - reinterpret_cast<حرف*>(هدف));
                }
            }
        } وإلا {
            /* target is full */
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    إذا(U_SUCCESS(*p_Error_ڪوڊ) &&
            cnv->preFromUFirstCP<0 &&
            ذريعو<(ذريعو_حد_بندي=(uint8_t *)p_منزل_UArgs->ذريعو_حد_بندي)) {
        c=utf8->منزل_UBytes[0]=b=*ذريعو++;
        منزل_ULength=1;
        منزل_ULimit=U8_ڳڻپ_بائيٽون(b);
        طالما(ذريعو<ذريعو_حد_بندي) {
            utf8->منزل_UBytes[منزل_ULength++]=b=*ذريعو++;
            c=(c<<6)+b;
        }
        utf8->منزل_Unicode_حالت=c;
        utf8->منزل_ULength=منزل_ULength;
        utf8->گهڻائي=منزل_ULimit;
    }

    /* write back the updated pointers */
    p_منزل_UArgs->ذريعو=(حرف *)ذريعو;
    pFromUArgs->هدف = reinterpret_cast<حرف*>(هدف);
}

ساكن فراغ U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_دليلون *pFromUArgs,
                  UConverter_منزل_Unicode_دليلون *p_منزل_UArgs,
                  UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *utf8, *cnv;
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي;
    uint8_t *هدف;
    int32_t هدف_صلاحيت;

    ثابت uint16_t *جدول, *mbcs_اشاريو;
    ثابت uint16_t *results;

    int8_t old_منزل_ULength, منزل_ULength, منزل_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t stage2_داخلا;
    uint32_t asciiRoundtrips;
    uint16_t value = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_منزل_UArgs->converter;
    cnv=pFromUArgs->converter;
    ذريعو=(uint8_t *)p_منزل_UArgs->ذريعو;
    ذريعو_حد_بندي=(uint8_t *)p_منزل_UArgs->ذريعو_حد_بندي;
    هدف = reinterpret_cast<uint8_t*>(pFromUArgs->هدف);
    هدف_صلاحيت = static_cast<int32_t>(pFromUArgs->هدف_حد_بندي - pFromUArgs->هدف);

    جدول=cnv->shared_معلومات->mbcs.from_Unicode_جدول;
    mbcs_اشاريو=cnv->shared_معلومات->mbcs.mbcs_اشاريو;
    إذا((cnv->options&UCNV_OPTION_مٽا_سٽا_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_معلومات->mbcs.مٽا_سٽا_LFNLFrom_Unicode_بائيٽون);
    } وإلا {
        results=(uint16_t *)cnv->shared_معلومات->mbcs.from_Unicode_بائيٽون;
    }
    asciiRoundtrips=cnv->shared_معلومات->mbcs.asciiRoundtrips;

    hasSupplementary = static_cast<UBool>(cnv->shared_معلومات->mbcs.unicode_نقاب & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    إذا(utf8->منزل_ULength > 0) {
        منزل_ULength=old_منزل_ULength=utf8->منزل_ULength;
        منزل_ULimit = static_cast<int8_t>(utf8->گهڻائي);
        c = static_cast<UChar32>(utf8->منزل_Unicode_حالت);
    } وإلا {
        منزل_ULength=old_منزل_ULength=منزل_ULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(ذريعو_حد_بندي - ذريعو) - (منزل_ULimit - old_منزل_ULength);
        إذا(length>0) {
            uint8_t b1=*(ذريعو_حد_بندي-1);
            إذا(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } وإلا إذا(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(ذريعو_حد_بندي-2);
                إذا(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    ذريعو_حد_بندي-=2;
                }
            } وإلا إذا(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --ذريعو_حد_بندي;
            }
        }
    }

    إذا(c!=0 && هدف_صلاحيت>0) {
        utf8->منزل_Unicode_حالت=0;
        utf8->منزل_ULength=0;
        اذهب وڌيڪ_بائيٽون;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    طالما(ذريعو<ذريعو_حد_بندي) {
        إذا(هدف_صلاحيت>0) {
            b=*ذريعو++;
            إذا(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                إذا(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *هدف++=b;
                    --هدف_صلاحيت;
                    استمر;
                } وإلا {
                    value=DBCS_نتيجو_FROM_UTF8(mbcs_اشاريو, results, 0, b);
                    إذا(value==0) {
                        c=b;
                        اذهب unassigned;
                    }
                }
            } وإلا {
                إذا(b>=0xe0) {
                    إذا( /* handle U+0800..U+D7FF inline */
                        b<=0xed &&  // do not assume maxFastUChar>0xd7ff
                        U8_IS_VALID_LEAD3_AND_T1(b, t1=ذريعو[0]) &&
                        (t2 = static_cast<uint8_t>(ذريعو[1] - 0x80)) <= 0x3f
                    ) {
                        c=((b&0xf)<<6)|(t1&0x3f);
                        ذريعو+=2;
                        value=DBCS_نتيجو_FROM_UTF8(mbcs_اشاريو, results, c, t2);
                        إذا(value==0) {
                            c=(c<<6)|t2;
                            اذهب unassigned;
                        }
                    } وإلا {
                        c=-1;
                    }
                } وإلا {
                    إذا( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*ذريعو - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++ذريعو;
                        value=DBCS_نتيجو_FROM_UTF8(mbcs_اشاريو, results, c, t1);
                        إذا(value==0) {
                            c=(c<<6)|t1;
                            اذهب unassigned;
                        }
                    } وإلا {
                        c=-1;
                    }
                }

                إذا(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    old_منزل_ULength=0;
                    منزل_ULength=1;
                    منزل_ULimit=U8_ڳڻپ_بائيٽون_NON_ASCII(b);
                    c=b;
وڌيڪ_بائيٽون:
                    طالما(منزل_ULength<منزل_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        إذا(ذريعو<(uint8_t *)p_منزل_UArgs->ذريعو_حد_بندي) {
                            b=*ذريعو;
                            إذا(icu::UTF8::isValidTrail(c, b, منزل_ULength, منزل_ULimit)) {
                                ++ذريعو;
                                ++منزل_ULength;
                                c=(c<<6)+b;
                            } وإلا {
                                اكسر; /* sequence too short, stop with toULength<toULimit */
                            }
                        } وإلا {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            ذريعو-=(منزل_ULength-old_منزل_ULength);
                            طالما(old_منزل_ULength<منزل_ULength) {
                                utf8->منزل_UBytes[old_منزل_ULength++]=*ذريعو++;
                            }
                            utf8->منزل_Unicode_حالت=c;
                            utf8->منزل_ULength=منزل_ULength;
                            utf8->گهڻائي=منزل_ULimit;
                            p_منزل_UArgs->ذريعو=(حرف *)ذريعو;
                            pFromUArgs->هدف = reinterpret_cast<حرف*>(هدف);
                            أرجع;
                        }
                    }

                    إذا(منزل_ULength==منزل_ULimit) {
                        c-=utf8_offsets[منزل_ULength];
                        إذا(منزل_ULength<=3) {  /* BMP */
                            stage2_داخلا=MBCS_STAGE_2_FROM_U(جدول, c);
                        } وإلا {
                            /* supplementary code point */
                            إذا(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                stage2_داخلا=0;
                            } وإلا {
                                stage2_داخلا=MBCS_STAGE_2_FROM_U(جدول, c);
                            }
                        }
                    } وإلا {
                        /* error handling: illegal UTF-8 byte sequence */
                        ذريعو-=(منزل_ULength-old_منزل_ULength);
                        طالما(old_منزل_ULength<منزل_ULength) {
                            utf8->منزل_UBytes[old_منزل_ULength++]=*ذريعو++;
                        }
                        utf8->منزل_ULength=منزل_ULength;
                        p_منزل_UArgs->ذريعو=(حرف *)ذريعو;
                        pFromUArgs->هدف = reinterpret_cast<حرف*>(هدف);
                        *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                        أرجع;
                    }

                    /* get the bytes and the length for the output */
                    /* MBCS_OUTPUT_2 */
                    value=MBCS_قدر_2_FROM_STAGE_2(results, stage2_داخلا, c);

                    /* is this code point assigned, or do we use fallbacks? */
                    إذا(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_داخلا, c) ||
                         (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                    ) {
                        اذهب unassigned;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            إذا(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *هدف++ = static_cast<uint8_t>(value);
                --هدف_صلاحيت;
            } وإلا /* length==2 */ {
                *هدف++ = static_cast<uint8_t>(value >> 8);
                إذا(2<=هدف_صلاحيت) {
                    *هدف++ = static_cast<uint8_t>(value);
                    هدف_صلاحيت-=2;
                } وإلا {
                    cnv->char_Error_بفر[0] = static_cast<حرف>(value);
                    cnv->char_Error_بفر_ڊيگهائي=1;

                    /* target overflow */
                    *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                    اكسر;
                }
            }
            استمر;

unassigned:
            {
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                ساكن ثابت char16_t nul=0;
                ثابت char16_t *no_ذريعو=&nul;
                c=_extFromU(cnv, cnv->shared_معلومات,
                            c, &no_ذريعو, no_ذريعو,
                            &هدف, هدف+هدف_صلاحيت,
                            خالي_اشارو, -1,
                            pFromUArgs->flush,
                            p_Error_ڪوڊ);

                إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    اكسر;
                } وإلا إذا(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *p_Error_ڪوڊ=U_USING_DEFAULT_WARNING;
                    اكسر;
                } وإلا {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    هدف_صلاحيت = static_cast<int32_t>(pFromUArgs->هدف_حد_بندي - reinterpret_cast<حرف*>(هدف));
                    استمر;
                }
            }
        } وإلا {
            /* target is full */
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    إذا(U_SUCCESS(*p_Error_ڪوڊ) &&
            cnv->preFromUFirstCP<0 &&
            ذريعو<(ذريعو_حد_بندي=(uint8_t *)p_منزل_UArgs->ذريعو_حد_بندي)) {
        c=utf8->منزل_UBytes[0]=b=*ذريعو++;
        منزل_ULength=1;
        منزل_ULimit=U8_ڳڻپ_بائيٽون(b);
        طالما(ذريعو<ذريعو_حد_بندي) {
            utf8->منزل_UBytes[منزل_ULength++]=b=*ذريعو++;
            c=(c<<6)+b;
        }
        utf8->منزل_Unicode_حالت=c;
        utf8->منزل_ULength=منزل_ULength;
        utf8->گهڻائي=منزل_ULimit;
    }

    /* write back the updated pointers */
    p_منزل_UArgs->ذريعو=(حرف *)ذريعو;
    pFromUArgs->هدف = reinterpret_cast<حرف*>(هدف);
}

/* miscellaneous ------------------------------------------------------------ */

ساكن فراغ U_CALLCONV
ucnv_MBCSGetStarters(ثابت UConverter* cnv,
                 UBool starters[256],
                 UError_ڪوڊ *) {
    ثابت int32_t *state0;
    صحيح i;

    state0=cnv->shared_معلومات->mbcs.state_جدول[cnv->shared_معلومات->mbcs.dbcsOnlyState];
    لكل(i=0; i<256; ++i) {
        /* all bytes that cause a state transition from state 0 are lead bytes */
        starters[i] = static_cast<UBool>(MBCS_داخلا_IS_TRANSITION(state0[i]));
    }
}

/*
 * This is an internal function that allows other converter implementations
 * to check whether a byte is a lead byte.
 */
U_CFUNC UBool
ucnv_MBCSIsLeadByte(UConverter_Shared_معلومات *shared_معلومات, حرف byte) {
    أرجع MBCS_داخلا_IS_TRANSITION(shared_معلومات->mbcs.state_جدول[0][(uint8_t)byte]);
}

ساكن فراغ U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_دليلون *p_دليلون,
              int32_t وٿي_اشاريو,
              UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv=p_دليلون->converter;
    حرف *p, *subchar;
    حرف بفر[4];
    int32_t length;

    /* first, select between subChar and subChar1 */
    إذا( cnv->subChar1!=0 &&
        (cnv->shared_معلومات->mbcs.extIndexes!=خالي_اشارو ?
            cnv->useSubChar1 :
            (cnv->invalid_UChar_بفر[0]<=0xff))
    ) {
        /* select subChar1 if it is set (not 0) and the unmappable Unicode code point is up to U+00ff (IBM MBCS behavior) */
        subchar = reinterpret_cast<حرف*>(&cnv->subChar1);
        length=1;
    } وإلا {
        /* select subChar in all other cases */
        subchar = reinterpret_cast<حرف*>(cnv->subChars);
        length=cnv->subCharLen;
    }

    /* reset the selector for the next code point */
    cnv->useSubChar1=خطأ;

    إذا (cnv->shared_معلومات->mbcs.outputType == MBCS_OUTPUT_2_SISO) {
        p=بفر;

        /* fromUnicodeStatus contains prevLength */
        بدل(length) {
        حالة 1:
            إذا(cnv->from_Unicode_حالت==2) {
                /* DBCS mode and SBCS sub char: change to SBCS */
                cnv->from_Unicode_حالت=1;
                *p++=UCNV_SI;
            }
            *p++=subchar[0];
            اكسر;
        حالة 2:
            إذا(cnv->from_Unicode_حالت<=1) {
                /* SBCS mode and DBCS sub char: change to DBCS */
                cnv->from_Unicode_حالت=2;
                *p++=UCNV_SO;
            }
            *p++=subchar[0];
            *p++=subchar[1];
            اكسر;
        افتراضي:
            *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
            أرجع;
        }
        subchar=بفر;
        length = static_cast<int32_t>(p - بفر);
    }

    ucnv_cb_From_UWrite_بائيٽون(p_دليلون, subchar, length, وٿي_اشاريو, p_Error_ڪوڊ);
}

U_CFUNC UConverterType
ucnv_MBCSGetType(ثابت UConverter* converter) {
    /* SBCS, DBCS, and EBCDIC_STATEFUL are replaced by MBCS, but here we cheat a little */
    إذا(converter->shared_معلومات->mbcs.ڳڻپ_حالتون==1) {
        أرجع (UConverterType)UCNV_SBCS;
    } وإلا إذا((converter->shared_معلومات->mbcs.outputType&0xff)==MBCS_OUTPUT_2_SISO) {
        أرجع (UConverterType)UCNV_EBCDIC_STATEFUL;
    } وإلا إذا(converter->shared_معلومات->static_معلومات->min_بائيٽون_في_Char==2 && converter->shared_معلومات->static_معلومات->max_بائيٽون_في_Char==2) {
        أرجع (UConverterType)UCNV_DBCS;
    }
    أرجع (UConverterType)UCNV_MBCS;
}

#جيڪڏهن_پورو /* #if !UCONFIG_NO_LEGACY_CONVERSION */
