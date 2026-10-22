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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/ucnv_cb.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnvmbcs.h"
#అన్తర్భావ "ucnv_ext.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "ustr_imp.h"

/* control optimizations according to the platform */
#నిర్వచించు MBCS_UNROLL_SINGLE_గమ్యం_BMP 1
#నిర్వచించు MBCS_UNROLL_SINGLE_FROM_BMP 0

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
రకనిర్వచనం UBool U_CALLCONV
UConverter_Enum_గమ్యం_UCallback(స్థిరరాశి శూన్య *context, uint32_t value, UChar32 సంకేతం_Points[32]);

స్థిర శూన్య U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం,
          UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
          స్థిరరాశి uint8_t *raw,
          UError_సంకేతం *p_Error_సంకేతం);

స్థిర శూన్య U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం);

స్థిర శూన్య U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
              UError_సంకేతం *p_Error_సంకేతం);

స్థిర UChar32 U_CALLCONV
ucnv_MBCSGet_తదుపరి_UChar(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                  UError_సంకేతం *p_Error_సంకేతం);

స్థిర శూన్య U_CALLCONV
ucnv_MBCSGetStarters(స్థిరరాశి UConverter* cnv,
                 UBool starters[256],
                 UError_సంకేతం *p_Error_సంకేతం);

U_CDECL_BEGIN
స్థిర స్థిరరాశి అక్షరం* U_CALLCONV
ucnv_MBCSGetName(స్థిరరాశి UConverter *cnv);
U_CDECL_చివర

స్థిర శూన్య U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
              int32_t స్థానభ్రంశం_సూచీ,
              UError_సంకేతం *p_Error_సంకేతం);

స్థిర UChar32 U_CALLCONV
ucnv_MBCSGet_తదుపరి_UChar(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                  UError_సంకేతం *p_Error_సంకేతం);

స్థిర శూన్య U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_వాదనలు *pFromUArgs,
                  UConverter_గమ్యం_Unicode_వాదనలు *p_గమ్యం_UArgs,
                  UError_సంకేతం *p_Error_సంకేతం);

స్థిర శూన్య U_CALLCONV
ucnv_MBCSGet_Unicode_సమితి(స్థిరరాశి UConverter *cnv,
                   స్థిరరాశి USetAdder *sa,
                   UConverter_Unicode_సమితి which,
                   UError_సంకేతం *p_Error_సంకేతం);

స్థిర శూన్య U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_వాదనలు *pFromUArgs,
                  UConverter_గమ్యం_Unicode_వాదనలు *p_గమ్యం_UArgs,
                  UError_సంకేతం *p_Error_సంకేతం);

స్థిర స్థిరరాశి UConverterImpl _SBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    శూన్యసూచిక,
    శూన్యసూచిక,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_తదుపరి_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    శూన్యసూచిక,
    ucnv_MBCSGet_Unicode_సమితి,

    శూన్యసూచిక,
    ucnv_SBCSFromUTF8
};

స్థిర స్థిరరాశి UConverterImpl _DBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    శూన్యసూచిక,
    శూన్యసూచిక,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_తదుపరి_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    శూన్యసూచిక,
    ucnv_MBCSGet_Unicode_సమితి,

    శూన్యసూచిక,
    ucnv_DBCSFromUTF8
};

స్థిర స్థిరరాశి UConverterImpl _MBCSImpl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    శూన్యసూచిక,
    శూన్యసూచిక,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_తదుపరి_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    శూన్యసూచిక,
    ucnv_MBCSGet_Unicode_సమితి,
    శూన్యసూచిక,
    శూన్యసూచిక
};

/* Static data is in tools/makeconv/ucnvstat.c for data-based
 * converters. Be sure to update it as well.
 */

స్థిరరాశి UConverter_Shared_దత్తాంశం _MBCSData={
    పరిమాణం(UConverter_Shared_దత్తాంశం), 1,
    శూన్యసూచిక, శూన్యసూచిక, అబద్ధం, నిజం, &_MBCSImpl,
    0, UCNV_MBCS_పట్టిక_INITIALIZER
};


/* GB 18030 data ------------------------------------------------------------ */

/* helper macros for linear values for GB 18030 four-byte sequences */
#నిర్వచించు రేఖీయ_18030(a, b, c, d) ((((a)*10+(b))*126L+(c))*10L+(d))

#నిర్వచించు రేఖీయ_18030_పునాది రేఖీయ_18030(0x81, 0x30, 0x81, 0x30)

#నిర్వచించు రేఖీయ(x) రేఖీయ_18030(x>>24, (x>>16)&0xff, (x>>8)&0xff, x&0xff)

/*
 * Some ranges of GB 18030 where both the Unicode code points and the
 * GB four-byte sequences are contiguous and are handled algorithmically by
 * the special callback functions below.
 * The values are start & end of Unicode & GB codes.
 *
 * Note that single surrogates are not mapped by GB 18030
 * as of the re-released mapping tables from 2000-nov-30.
 */
స్థిర స్థిరరాశి uint32_t
gb18030Ranges[14][4]={
    {0x10000, 0x10FFFF, రేఖీయ(0x90308130), రేఖీయ(0xE3329A35)},
    {0x9FA6, 0xD7FF, రేఖీయ(0x82358F33), రేఖీయ(0x8336C738)},
    {0x0452, 0x1E3E, రేఖీయ(0x8130D330), రేఖీయ(0x8135F436)},
    {0x1E40, 0x200F, రేఖీయ(0x8135F438), రేఖీయ(0x8136A531)},
    {0xE865, 0xF92B, రేఖీయ(0x8336D030), రేఖీయ(0x84308534)},
    {0x2643, 0x2E80, రేఖీయ(0x8137A839), రేఖీయ(0x8138FD38)},
    {0xFA2A, 0xFE2F, రేఖీయ(0x84309C38), రేఖీయ(0x84318537)},
    {0x3CE1, 0x4055, రేఖీయ(0x8231D438), రేఖీయ(0x8232AF32)},
    {0x361B, 0x3917, రేఖీయ(0x8230A633), రేఖీయ(0x8230F237)},
    {0x49B8, 0x4C76, రేఖీయ(0x8234A131), రేఖీయ(0x8234E733)},
    {0x4160, 0x4336, రేఖీయ(0x8232C937), రేఖీయ(0x8232F837)},
    {0x478E, 0x4946, రేఖీయ(0x8233E838), రేఖీయ(0x82349638)},
    {0x44D7, 0x464B, రేఖీయ(0x8233A339), రేఖీయ(0x8233C931)},
    {0xFFE6, 0xFFFF, రేఖీయ(0x8431A234), రేఖీయ(0x8431A439)}
};

/* bit flag for UConverter.options indicating GB 18030 special handling */
#నిర్వచించు _MBCS_OPTION_GB18030 0x8000

/* bit flag for UConverter.options indicating KEIS,JEF,JIF special handling */
#నిర్వచించు _MBCS_OPTION_KEIS 0x01000
#నిర్వచించు _MBCS_OPTION_JEF  0x02000
#నిర్వచించు _MBCS_OPTION_JIPS 0x04000

#నిర్వచించు KEIS_SO_CHAR_1 0x0A
#నిర్వచించు KEIS_SO_CHAR_2 0x42
#నిర్వచించు KEIS_SI_CHAR_1 0x0A
#నిర్వచించు KEIS_SI_CHAR_2 0x41

#నిర్వచించు JEF_SO_CHAR 0x28
#నిర్వచించు JEF_SI_CHAR 0x29

#నిర్వచించు JIPS_SO_CHAR_1 0x1A
#నిర్వచించు JIPS_SO_CHAR_2 0x70
#నిర్వచించు JIPS_SI_CHAR_1 0x1A
#నిర్వచించు JIPS_SI_CHAR_2 0x71

గణన SISO_Option {
    SI,
    SO
};
రకనిర్వచనం గణన SISO_Option SISO_Option;

స్థిర int32_t getSISOBytes(SISO_Option option, uint32_t cnvOption, uint8_t *value) {
    int32_t SISOLength = 0;

    ఎంపిక (option) {
        స్థితి SI:
            ఒకవేళ ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SI_CHAR_1;
                value[1] = KEIS_SI_CHAR_2;
                SISOLength = 2;
            } లేకపోతే ఒకవేళ ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SI_CHAR;
                SISOLength = 1;
            } లేకపోతే ఒకవేళ ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SI_CHAR_1;
                value[1] = JIPS_SI_CHAR_2;
                SISOLength = 2;
            } లేకపోతే {
                value[0] = UCNV_SI;
                SISOLength = 1;
            }
            ఆపు;
        స్థితి SO:
            ఒకవేళ ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SO_CHAR_1;
                value[1] = KEIS_SO_CHAR_2;
                SISOLength = 2;
            } లేకపోతే ఒకవేళ ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SO_CHAR;
                SISOLength = 1;
            } లేకపోతే ఒకవేళ ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SO_CHAR_1;
                value[1] = JIPS_SO_CHAR_2;
                SISOLength = 2;
            } లేకపోతే {
                value[0] = UCNV_SO;
                SISOLength = 1;
            }
            ఆపు;
        అప్రమేయ:
            /* Should never happen. */
            ఆపు;
    }

    తిరిగి SISOLength;
}

/* Miscellaneous ------------------------------------------------------------ */

/* similar to ucnv_MBCSGetNextUChar() but recursive */
స్థిర UBool
enum_గమ్యం_U(UConverterMBCSTable *mbcs_పట్టిక, int8_t stateProps[],
        int32_t state, uint32_t స్థానభ్రంశం,
        uint32_t value,
        UConverter_Enum_గమ్యం_UCallback *callback, స్థిరరాశి శూన్య *context,
        UError_సంకేతం *p_Error_సంకేతం) {
    UChar32 సంకేతం_Points[32];
    స్థిరరాశి int32_t *అడ్డువరుస;
    స్థిరరాశి uint16_t *unicode_సంకేతం_Units;
    UChar32 any_సంకేతం_Points;
    int32_t b, హద్దు;

    అడ్డువరుస=mbcs_పట్టిక->state_పట్టిక[state];
    unicode_సంకేతం_Units=mbcs_పట్టిక->unicode_సంకేతం_Units;

    value<<=8;
    any_సంకేతం_Points=-1;  /* becomes non-negative if there is a mapping */

    b=(stateProps[state]&0x38)<<2;
    ఒకవేళ(b==0 && stateProps[state]>=0x40) {
        /* skip byte sequences with leading zeros because they are not stored in the fromUnicode table */
        సంకేతం_Points[0]=U_SENTINEL;
        b=1;
    }
    హద్దు=((stateProps[state]&7)+1)<<5;
    అయితే(b<హద్దు) {
        int32_t నమోదు_2=అడ్డువరుస[b];
        ఒకవేళ(MBCS_నమోదు_IS_TRANSITION(నమోదు_2)) {
            int32_t తదుపరి_State=MBCS_నమోదు_TRANSITION_STATE(నమోదు_2);
            ఒకవేళ(stateProps[తదుపరి_State]>=0) {
                /* recurse to a state with non-ignorable actions */
                ఒకవేళ(!enum_గమ్యం_U(
                        mbcs_పట్టిక, stateProps, తదుపరి_State,
                        స్థానభ్రంశం+MBCS_నమోదు_TRANSITION_స్థానభ్రంశం(నమోదు_2),
                        value | static_cast<uint32_t>(b),
                        callback, context,
                        p_Error_సంకేతం)) {
                    తిరిగి అబద్ధం;
                }
            }
            సంకేతం_Points[b&0x1f]=U_SENTINEL;
        } లేకపోతే {
            UChar32 c;
            int32_t action;

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=MBCS_నమోదు_FINAL_ACTION(నమోదు_2);
            ఒకవేళ(action==MBCS_STATE_VALID_ప్రత్యక్ష_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_VALID_16) {
                int32_t final_స్థానభ్రంశం=స్థానభ్రంశం+MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
                c=unicode_సంకేతం_Units[final_స్థానభ్రంశం];
                ఒకవేళ(c<0xfffe) {
                    /* output BMP code point */
                } లేకపోతే {
                    c=U_SENTINEL;
                }
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_VALID_16_PAIR) {
                int32_t final_స్థానభ్రంశం=స్థానభ్రంశం+MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
                c=unicode_సంకేతం_Units[final_స్థానభ్రంశం++];
                ఒకవేళ(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } లేకపోతే ఒకవేళ(c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_సంకేతం_Units[final_స్థానభ్రంశం]+(0x10000-0xdc00);
                } లేకపోతే ఒకవేళ(c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_సంకేతం_Units[final_స్థానభ్రంశం];
                } లేకపోతే {
                    c=U_SENTINEL;
                }
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_VALID_ప్రత్యక్ష_20) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_నమోదు_FINAL_విలువ(నమోదు_2) + 0x10000);
            } లేకపోతే {
                c=U_SENTINEL;
            }

            సంకేతం_Points[b&0x1f]=c;
            any_సంకేతం_Points&=c;
        }
        ఒకవేళ(((++b)&0x1f)==0) {
            ఒకవేళ(any_సంకేతం_Points>=0) {
                ఒకవేళ (!callback(context, value | static_cast<uint32_t>(b - 0x20), సంకేతం_Points)) {
                    తిరిగి అబద్ధం;
                }
                any_సంకేతం_Points=-1;
            }
        }
    }
    తిరిగి నిజం;
}

/*
 * Only called if stateProps[state]==-1.
 * A recursive call may do stateProps[state]|=0x40 if this state is the target of an
 * MBCS_STATE_CHANGE_ONLY.
 */
స్థిర int8_t
getStateProp(స్థిరరాశి int32_t (*state_పట్టిక)[256], int8_t stateProps[], పూర్ణాంకం state) {
    స్థిరరాశి int32_t *అడ్డువరుస;
    int32_t min, max, నమోదు_2, తదుపరి_State;

    అడ్డువరుస=state_పట్టిక[state];
    stateProps[state]=0;

    /* find first non-ignorable state */
    కొరకు(min=0;; ++min) {
        నమోదు_2=అడ్డువరుస[min];
        తదుపరి_State=MBCS_నమోదు_STATE(నమోదు_2);
        ఒకవేళ(stateProps[తదుపరి_State]==-1) {
            getStateProp(state_పట్టిక, stateProps, తదుపరి_State);
        }
        ఒకవేళ(MBCS_నమోదు_IS_TRANSITION(నమోదు_2)) {
            ఒకవేళ(stateProps[తదుపరి_State]>=0) {
                ఆపు;
            }
        } లేకపోతే ఒకవేళ(MBCS_నమోదు_FINAL_ACTION(నమోదు_2)<MBCS_STATE_UNASSIGNED) {
            ఆపు;
        }
        ఒకవేళ(min==0xff) {
            stateProps[state]=-0x40;  /* (int8_t)0xc0 */
            తిరిగి stateProps[state];
        }
    }
    stateProps[state] |= static_cast<int8_t>((min >> 5) << 3);

    /* find last non-ignorable state */
    కొరకు(max=0xff; min<max; --max) {
        నమోదు_2=అడ్డువరుస[max];
        తదుపరి_State=MBCS_నమోదు_STATE(నమోదు_2);
        ఒకవేళ(stateProps[తదుపరి_State]==-1) {
            getStateProp(state_పట్టిక, stateProps, తదుపరి_State);
        }
        ఒకవేళ(MBCS_నమోదు_IS_TRANSITION(నమోదు_2)) {
            ఒకవేళ(stateProps[తదుపరి_State]>=0) {
                ఆపు;
            }
        } లేకపోతే ఒకవేళ(MBCS_నమోదు_FINAL_ACTION(నమోదు_2)<MBCS_STATE_UNASSIGNED) {
            ఆపు;
        }
    }
    stateProps[state] |= static_cast<int8_t>(max >> 5);

    /* recurse further and collect direct-state information */
    అయితే(min<=max) {
        నమోదు_2=అడ్డువరుస[min];
        తదుపరి_State=MBCS_నమోదు_STATE(నమోదు_2);
        ఒకవేళ(stateProps[తదుపరి_State]==-1) {
            getStateProp(state_పట్టిక, stateProps, తదుపరి_State);
        }
        ఒకవేళ(MBCS_నమోదు_IS_FINAL(నమోదు_2)) {
            stateProps[తదుపరి_State]|=0x40;
            ఒకవేళ(MBCS_నమోదు_FINAL_ACTION(నమోదు_2)<=MBCS_STATE_FALLBACK_ప్రత్యక్ష_20) {
                stateProps[state]|=0x40;
            }
        }
        ++min;
    }
    తిరిగి stateProps[state];
}

/*
 * Internal function enumerating the toUnicode data of an MBCS converter.
 * Currently only used for reconstituting data for a MBCS_OPT_NO_FROM_U
 * table, but could also be used for a future ucnv_getUnicodeSet() option
 * that includes reverse fallbacks (after updating this function's implementation).
 * Currently only handles roundtrip mappings.
 * Does not currently handle extensions.
 */
స్థిర శూన్య
ucnv_MBCSEnum_గమ్యం_Unicode(UConverterMBCSTable *mbcs_పట్టిక,
                       UConverter_Enum_గమ్యం_UCallback *callback, స్థిరరాశి శూన్య *context,
                       UError_సంకేతం *p_Error_సంకేతం) {
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
    int8_t stateProps[MBCS_MAX_STATE_లెక్క];
    int32_t state;

    uprv_memset(stateProps, -1, పరిమాణం(stateProps));

    /* recurse from state 0 and set all stateProps */
    getStateProp(mbcs_పట్టిక->state_పట్టిక, stateProps, 0);

    కొరకు(state=0; state<mbcs_పట్టిక->లెక్క_స్థితులు; ++state) {
        /*if(stateProps[state]==-1) {
            printf("unused/unreachable <icu:state> %d\n", state);
        }*/
        ఒకవేళ(stateProps[state]>=0x40) {
            /* start from each direct state */
            enum_గమ్యం_U(
                mbcs_పట్టిక, stateProps, state, 0, 0,
                callback, context,
                p_Error_సంకేతం);
        }
    }
}

U_CFUNC శూన్య 
ucnv_MBCSGet_Filtered_Unicode_సమితి_For_Unicode(స్థిరరాశి UConverter_Shared_దత్తాంశం *shared_దత్తాంశం,
                                         స్థిరరాశి USetAdder *sa,
                                         UConverter_Unicode_సమితి which,
                                         UConverter_సమితి_Filter filter,
                                         UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UConverterMBCSTable *mbcs_పట్టిక;
    స్థిరరాశి uint16_t *పట్టిక;

    uint32_t st3;
    uint16_t st1, maxStage1, st2;

    UChar32 c;

    /* enumerate the from-Unicode trie table */
    mbcs_పట్టిక=&shared_దత్తాంశం->mbcs;
    పట్టిక=mbcs_పట్టిక->from_Unicode_పట్టిక;
    ఒకవేళ(mbcs_పట్టిక->unicode_ముసుగు&UCNV_HAS_SUPPLEMENTARY) {
        maxStage1=0x440;
    } లేకపోతే {
        maxStage1=0x40;
    }

    c=0; /* keep track of the current code point while enumerating */

    ఒకవేళ(mbcs_పట్టిక->outputType==MBCS_OUTPUT_1) {
        స్థిరరాశి uint16_t *stage2, *stage3, *results;
        uint16_t min_విలువ;

        results=(స్థిరరాశి uint16_t *)mbcs_పట్టిక->from_Unicode_బైట్లు;

        /*
         * Set a threshold variable for selecting which mappings to use.
         * See ucnv_MBCSSingleFromBMPWithOffsets() and
         * MBCS_SINGLE_RESULT_FROM_U() for details.
         */
        ఒకవేళ(which==UCNV_ROUNDTRIP_సమితి) {
            /* use only roundtrips */
            min_విలువ=0xf00;
        } లేకపోతే /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
            /* use all roundtrip and fallback results */
            min_విలువ=0x800;
        }

        కొరకు(st1=0; st1<maxStage1; ++st1) {
            st2=పట్టిక[st1];
            ఒకవేళ(st2>maxStage1) {
                stage2=పట్టిక+st2;
                కొరకు(st2=0; st2<64; ++st2) {
                    ఒకవేళ((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=results+st3;

                        చేయి {
                            ఒకవేళ(*stage3++>=min_విలువ) {
                                sa->కూడు(sa->సమితి, c);
                            }
                        } అయితే((++c&0xf)!=0);
                    } లేకపోతే {
                        c+=16; /* empty stage 3 block */
                    }
                }
            } లేకపోతే {
                c+=1024; /* empty stage 2 block */
            }
        }
    } లేకపోతే {
        స్థిరరాశి uint32_t *stage2;
        స్థిరరాశి uint8_t *stage3, *బైట్లు;
        uint32_t st3Multiplier;
        uint32_t value;
        UBool useFallback;

        బైట్లు=mbcs_పట్టిక->from_Unicode_బైట్లు;

        useFallback = which == UCNV_ROUNDTRIP_AND_FALLBACK_సమితి;

        ఎంపిక(mbcs_పట్టిక->outputType) {
        స్థితి MBCS_OUTPUT_3:
        స్థితి MBCS_OUTPUT_4_EUC:
            st3Multiplier=3;
            ఆపు;
        స్థితి MBCS_OUTPUT_4:
            st3Multiplier=4;
            ఆపు;
        అప్రమేయ:
            st3Multiplier=2;
            ఆపు;
        }

        కొరకు(st1=0; st1<maxStage1; ++st1) {
            st2=పట్టిక[st1];
            ఒకవేళ(st2>(maxStage1>>1)) {
                stage2=(స్థిరరాశి uint32_t *)పట్టిక+st2;
                కొరకు(st2=0; st2<64; ++st2) {
                    ఒకవేళ((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=బైట్లు+st3Multiplier*16*(uint32_t)(uint16_t)st3;

                        /* get the roundtrip flags for the stage 3 block */
                        st3>>=16;

                        /*
                         * Add code points for which the roundtrip flag is set,
                         * or which map to non-zero bytes if we use fallbacks.
                         * See ucnv_MBCSFromUnicodeWithOffsets() for details.
                         */
                        ఎంపిక(filter) {
                        స్థితి UCNV_సమితి_FILTER_NONE:
                            చేయి {
                                ఒకవేళ(st3&1) {
                                    sa->కూడు(sa->సమితి, c);
                                    stage3+=st3Multiplier;
                                } లేకపోతే ఒకవేళ(useFallback) {
                                    uint8_t b=0;
                                    ఎంపిక(st3Multiplier) {
                                    స్థితి 4:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    స్థితి 3:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    స్థితి 2:
                                        b|=stage3[0]|stage3[1];
                                        stage3+=2;
                                        U_FALLTHROUGH;
                                    అప్రమేయ:
                                        ఆపు;
                                    }
                                    ఒకవేళ(b!=0) {
                                        sa->కూడు(sa->సమితి, c);
                                    }
                                }
                                st3>>=1;
                            } అయితే((++c&0xf)!=0);
                            ఆపు;
                        స్థితి UCNV_సమితి_FILTER_DBCS_ONLY:
                             /* Ignore single-byte results (<0x100). */
                            చేయి {
                                ఒకవేళ(((st3&1)!=0 || useFallback) && *((స్థిరరాశి uint16_t *)stage3)>=0x100) {
                                    sa->కూడు(sa->సమితి, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } అయితే((++c&0xf)!=0);
                            ఆపు;
                        స్థితి UCNV_సమితి_FILTER_2022_CN:
                             /* Only add code points that map to CNS 11643 planes 1 & 2 for non-EXT ISO-2022-CN. */
                            చేయి {
                                ఒకవేళ(((st3&1)!=0 || useFallback) && ((value=*stage3)==0x81 || value==0x82)) {
                                    sa->కూడు(sa->సమితి, c);
                                }
                                st3>>=1;
                                stage3+=3;  /* +=st3Multiplier */
                            } అయితే((++c&0xf)!=0);
                            ఆపు;
                        స్థితి UCNV_సమితి_FILTER_SJIS:
                             /* Only add code points that map to Shift-JIS codes corresponding to JIS X 0208. */
                            చేయి {
                                ఒకవేళ(((st3&1)!=0 || useFallback) && (value=*((స్థిరరాశి uint16_t *)stage3))>=0x8140 && value<=0xeffc) {
                                    sa->కూడు(sa->సమితి, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } అయితే((++c&0xf)!=0);
                            ఆపు;
                        స్థితి UCNV_సమితి_FILTER_GR94_DBCS:
                            /* Only add code points that map to ISO 2022 GR 94 DBCS codes (each byte A1..FE). */
                            చేయి {
                                ఒకవేళ( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((స్థిరరాశి uint16_t *)stage3)) - 0xa1a1)<=(0xfefe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->కూడు(sa->సమితి, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } అయితే((++c&0xf)!=0);
                            ఆపు;
                        స్థితి UCNV_సమితి_FILTER_HZ:
                            /* Only add code points that are suitable for HZ DBCS (lead byte A1..FD). */
                            చేయి {
                                ఒకవేళ( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((స్థిరరాశి uint16_t *)stage3))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->కూడు(sa->సమితి, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } అయితే((++c&0xf)!=0);
                            ఆపు;
                        అప్రమేయ:
                            *p_Error_సంకేతం=U_అంతర్గత_కార్యక్రమం_ERROR;
                            తిరిగి;
                        }
                    } లేకపోతే {
                        c+=16; /* empty stage 3 block */
                    }
                }
            } లేకపోతే {
                c+=1024; /* empty stage 2 block */
            }
        }
    }

    ucnv_ext_Get_Unicode_సమితి(shared_దత్తాంశం, sa, which, filter, p_Error_సంకేతం);
}

U_CFUNC శూన్య
ucnv_MBCSGet_Unicode_సమితి_For_Unicode(స్థిరరాశి UConverter_Shared_దత్తాంశం *shared_దత్తాంశం,
                                 స్థిరరాశి USetAdder *sa,
                                 UConverter_Unicode_సమితి which,
                                 UError_సంకేతం *p_Error_సంకేతం) {
    ucnv_MBCSGet_Filtered_Unicode_సమితి_For_Unicode(
        shared_దత్తాంశం, sa, which,
        shared_దత్తాంశం->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ?
            UCNV_సమితి_FILTER_DBCS_ONLY :
            UCNV_సమితి_FILTER_NONE,
        p_Error_సంకేతం);
}

స్థిర శూన్య U_CALLCONV
ucnv_MBCSGet_Unicode_సమితి(స్థిరరాశి UConverter *cnv,
                   స్థిరరాశి USetAdder *sa,
                   UConverter_Unicode_సమితి which,
                   UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(cnv->options&_MBCS_OPTION_GB18030) {
        sa->కూడు_Range(sa->సమితి, 0, 0xd7ff);
        sa->కూడు_Range(sa->సమితి, 0xe000, 0x10ffff);
    } లేకపోతే {
        ucnv_MBCSGet_Unicode_సమితి_For_Unicode(cnv->shared_దత్తాంశం, sa, which, p_Error_సంకేతం);
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
స్థిర UChar32
_extFromU(UConverter *cnv, స్థిరరాశి UConverter_Shared_దత్తాంశం *shared_దత్తాంశం,
          UChar32 cp,
          స్థిరరాశి char16_t **ఆధారం, స్థిరరాశి char16_t *ఆధారం_హద్దు,
          uint8_t **లక్ష్యం, స్థిరరాశి uint8_t *లక్ష్యం_హద్దు,
          int32_t **offsets, int32_t ఆధారం_సూచీ,
          UBool flush,
          UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి int32_t *cx;

    cnv->useSubChar1=అబద్ధం;

    ఒకవేళ( (cx=shared_దత్తాంశం->mbcs.extIndexes)!=శూన్యసూచిక &&
        ucnv_ext_Initial_సరిపోలిక_From_U(
            cnv, cx,
            cp, ఆధారం, ఆధారం_హద్దు,
            reinterpret_cast<అక్షరం**>(లక్ష్యం), reinterpret_cast<స్థిరరాశి అక్షరం*>(లక్ష్యం_హద్దు),
            offsets, ఆధారం_సూచీ,
            flush,
            p_Error_సంకేతం)
    ) {
        తిరిగి 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    ఒకవేళ((cnv->options&_MBCS_OPTION_GB18030)!=0) {
        స్థిరరాశి uint32_t *range;
        int32_t i;

        range=gb18030Ranges[0];
        కొరకు(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            ఒకవేళ (range[0] <= static_cast<uint32_t>(cp) && static_cast<uint32_t>(cp) <= range[1]) {
                /* found the Unicode code point, output the four-byte sequence for it */
                uint32_t రేఖీయ_2;
                అక్షరం బైట్లు[4];

                /* get the linear value of the first GB 18030 code in this range */
                రేఖీయ_2=range[2]-రేఖీయ_18030_పునాది;

                /* add the offset from the beginning of the range */
                రేఖీయ_2 += (static_cast<uint32_t>(cp) - range[0]);

                /* turn this into a four-byte sequence */
                బైట్లు[3] = static_cast<అక్షరం>(0x30 + రేఖీయ_2 % 10); రేఖీయ_2 /= 10;
                బైట్లు[2] = static_cast<అక్షరం>(0x81 + రేఖీయ_2 % 126); రేఖీయ_2 /= 126;
                బైట్లు[1] = static_cast<అక్షరం>(0x30 + రేఖీయ_2 % 10); రేఖీయ_2 /= 10;
                బైట్లు[0] = static_cast<అక్షరం>(0x81 + రేఖీయ_2);

                /* output this sequence */
                ucnv_from_UWrite_బైట్లు(cnv,
                                     బైట్లు, 4, reinterpret_cast<అక్షరం**>(లక్ష్యం), reinterpret_cast<స్థిరరాశి అక్షరం*>(లక్ష్యం_హద్దు),
                                     offsets, ఆధారం_సూచీ, p_Error_సంకేతం);
                తిరిగి 0;
            }
        }
    }

    /* no mapping */
    *p_Error_సంకేతం=U_INVALID_CHAR_దొరికింది;
    తిరిగి cp;
}

/*
 * Input sequence: cnv->toUBytes[0..length[
 * @return if(U_FAILURE) return the length (toULength, byteIndex) for the input
 *         else return 0 after output has been written to the target
 */
స్థిర int8_t
ext_గమ్యం_U(UConverter *cnv, స్థిరరాశి UConverter_Shared_దత్తాంశం *shared_దత్తాంశం,
        int8_t length,
        స్థిరరాశి uint8_t **ఆధారం, స్థిరరాశి uint8_t *ఆధారం_హద్దు,
        char16_t **లక్ష్యం, స్థిరరాశి char16_t *లక్ష్యం_హద్దు,
        int32_t **offsets, int32_t ఆధారం_సూచీ,
        UBool flush,
        UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి int32_t *cx;

    ఒకవేళ( (cx=shared_దత్తాంశం->mbcs.extIndexes)!=శూన్యసూచిక &&
        ucnv_ext_Initial_సరిపోలిక_గమ్యం_U(
            cnv, cx,
            length, reinterpret_cast<స్థిరరాశి అక్షరం**>(ఆధారం), reinterpret_cast<స్థిరరాశి అక్షరం*>(ఆధారం_హద్దు),
            లక్ష్యం, లక్ష్యం_హద్దు,
            offsets, ఆధారం_సూచీ,
            flush,
            p_Error_సంకేతం)
    ) {
        తిరిగి 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    ఒకవేళ(length==4 && (cnv->options&_MBCS_OPTION_GB18030)!=0) {
        స్థిరరాశి uint32_t *range;
        uint32_t రేఖీయ_2;
        int32_t i;

        రేఖీయ_2=రేఖీయ_18030(cnv->గమ్యం_UBytes[0], cnv->గమ్యం_UBytes[1], cnv->గమ్యం_UBytes[2], cnv->గమ్యం_UBytes[3]);
        range=gb18030Ranges[0];
        కొరకు(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            ఒకవేళ(range[2]<=రేఖీయ_2 && రేఖీయ_2<=range[3]) {
                /* found the sequence, output the Unicode code point for it */
                *p_Error_సంకేతం=U_ZERO_ERROR;

                /* add the linear difference between the input and start sequences to the start code point */
                రేఖీయ_2=range[0]+(రేఖీయ_2-range[2]);

                /* output this code point */
                ucnv_గమ్యం_UWrite_సంకేతం_బిందువు(cnv, రేఖీయ_2, లక్ష్యం, లక్ష్యం_హద్దు, offsets, ఆధారం_సూచీ, p_Error_సంకేతం);

                తిరిగి 0;
            }
        }
    }

    /* no mapping */
    *p_Error_సంకేతం=U_INVALID_CHAR_దొరికింది;
    తిరిగి length;
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
#నిర్వచించు EBCDIC_LF 0x25
#నిర్వచించు EBCDIC_NL 0x15

/* standard EBCDIC codes with roundtrip flag as stored in Unicode-to-single-byte tables */
#నిర్వచించు EBCDIC_RT_LF 0xf25
#నిర్వచించు EBCDIC_RT_NL 0xf15

/* Unicode code points */
#నిర్వచించు U_LF 0x0a
#నిర్వచించు U_NL 0x85

స్థిర UBool
_EBCDICSwapLFNL(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం, UError_సంకేతం *p_Error_సంకేతం) {
    UConverterMBCSTable *mbcs_పట్టిక;

    స్థిరరాశి uint16_t *పట్టిక, *results;
    స్థిరరాశి uint8_t *బైట్లు;

    int32_t (*new_State_పట్టిక)[256];
    uint16_t *newResults;
    uint8_t *p;
    అక్షరం *name;

    uint32_t stage2_నమోదు;
    uint32_t size, sizeofFromUBytes;

    mbcs_పట్టిక=&shared_దత్తాంశం->mbcs;

    పట్టిక=mbcs_పట్టిక->from_Unicode_పట్టిక;
    బైట్లు=mbcs_పట్టిక->from_Unicode_బైట్లు;
    results = reinterpret_cast<స్థిరరాశి uint16_t*>(బైట్లు);

    /*
     * Check that this is an EBCDIC table with SBCS portion -
     * SBCS or EBCDIC_STATEFUL with standard EBCDIC LF and NL mappings.
     *
     * If not, ignore the option. Options are always ignored if they do not apply.
     */
    ఒకవేళ(!(
         (mbcs_పట్టిక->outputType==MBCS_OUTPUT_1 || mbcs_పట్టిక->outputType==MBCS_OUTPUT_2_SISO) &&
         mbcs_పట్టిక->state_పట్టిక[0][EBCDIC_LF]==MBCS_నమోదు_FINAL(0, MBCS_STATE_VALID_ప్రత్యక్ష_16, U_LF) &&
         mbcs_పట్టిక->state_పట్టిక[0][EBCDIC_NL]==MBCS_నమోదు_FINAL(0, MBCS_STATE_VALID_ప్రత్యక్ష_16, U_NL)
    )) {
        తిరిగి అబద్ధం;
    }

    ఒకవేళ(mbcs_పట్టిక->outputType==MBCS_OUTPUT_1) {
        ఒకవేళ(!(
             EBCDIC_RT_LF==MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, results, U_LF) &&
             EBCDIC_RT_NL==MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, results, U_NL)
        )) {
            తిరిగి అబద్ధం;
        }
    } లేకపోతే /* MBCS_OUTPUT_2_SISO */ {
        stage2_నమోదు=MBCS_STAGE_2_FROM_U(పట్టిక, U_LF);
        ఒకవేళ(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_నమోదు, U_LF)!=0 &&
             EBCDIC_LF==MBCS_విలువ_2_FROM_STAGE_2(బైట్లు, stage2_నమోదు, U_LF)
        )) {
            తిరిగి అబద్ధం;
        }

        stage2_నమోదు=MBCS_STAGE_2_FROM_U(పట్టిక, U_NL);
        ఒకవేళ(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_నమోదు, U_NL)!=0 &&
             EBCDIC_NL==MBCS_విలువ_2_FROM_STAGE_2(బైట్లు, stage2_నమోదు, U_NL)
        )) {
            తిరిగి అబద్ధం;
        }
    }

    ఒకవేళ(mbcs_పట్టిక->from_UBytes_నిడివి>0) {
        /*
         * We _know_ the number of bytes in the fromUnicodeBytes array
         * starting with header.version 4.1.
         */
        sizeofFromUBytes=mbcs_పట్టిక->from_UBytes_నిడివి;
    } లేకపోతే {
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
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి అబద్ధం;
    }

    /*
     * The table has an appropriate format.
     * Allocate and build
     * - a modified to-Unicode state table
     * - a modified from-Unicode output array
     * - a converter name string with the swap option appended
     */
    size=
        mbcs_పట్టిక->లెక్క_స్థితులు*1024+
        sizeofFromUBytes+
        UCNV_MAX_CONVERTER_NAME_నిడివి+20;
    p = static_cast<uint8_t*>(uprv_malloc(size));
    ఒకవేళ(p==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి అబద్ధం;
    }

    /* copy and modify the to-Unicode state table */
    new_State_పట్టిక = reinterpret_cast<int32_t(*)[256]>(p);
    uprv_memcpy(new_State_పట్టిక, mbcs_పట్టిక->state_పట్టిక, mbcs_పట్టిక->లెక్క_స్థితులు*1024);

    new_State_పట్టిక[0][EBCDIC_LF]=MBCS_నమోదు_FINAL(0, MBCS_STATE_VALID_ప్రత్యక్ష_16, U_NL);
    new_State_పట్టిక[0][EBCDIC_NL]=MBCS_నమోదు_FINAL(0, MBCS_STATE_VALID_ప్రత్యక్ష_16, U_LF);

    /* copy and modify the from-Unicode result table */
    newResults = reinterpret_cast<uint16_t*>(new_State_పట్టిక[mbcs_పట్టిక->లెక్క_స్థితులు]);
    uprv_memcpy(newResults, బైట్లు, sizeofFromUBytes);

    /* conveniently, the table access macros work on the left side of expressions */
    ఒకవేళ(mbcs_పట్టిక->outputType==MBCS_OUTPUT_1) {
        MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, newResults, U_LF)=EBCDIC_RT_NL;
        MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, newResults, U_NL)=EBCDIC_RT_LF;
    } లేకపోతే /* MBCS_OUTPUT_2_SISO */ {
        stage2_నమోదు=MBCS_STAGE_2_FROM_U(పట్టిక, U_LF);
        MBCS_విలువ_2_FROM_STAGE_2(newResults, stage2_నమోదు, U_LF)=EBCDIC_NL;

        stage2_నమోదు=MBCS_STAGE_2_FROM_U(పట్టిక, U_NL);
        MBCS_విలువ_2_FROM_STAGE_2(newResults, stage2_నమోదు, U_NL)=EBCDIC_LF;
    }

    /* set the canonical converter name */
    name = reinterpret_cast<అక్షరం*>(newResults) + sizeofFromUBytes;
    uprv_strcpy(name, shared_దత్తాంశం->static_దత్తాంశం->name);
    uprv_strcat(name, UCNV_మార్పిడి_LFNL_OPTION_STRING);

    /* set the pointers */
    icu::umtx_తాళం(శూన్యసూచిక);
    ఒకవేళ(mbcs_పట్టిక->మార్పిడి_LFNLState_పట్టిక==శూన్యసూచిక) {
        mbcs_పట్టిక->మార్పిడి_LFNLState_పట్టిక=new_State_పట్టిక;
        mbcs_పట్టిక->మార్పిడి_LFNLFrom_Unicode_బైట్లు = reinterpret_cast<uint8_t*>(newResults);
        mbcs_పట్టిక->మార్పిడి_LFNLName=name;

        new_State_పట్టిక=శూన్యసూచిక;
    }
    icu::umtx_unlock(శూన్యసూచిక);

    /* release the allocated memory if another thread beat us to it */
    ఒకవేళ(new_State_పట్టిక!=శూన్యసూచిక) {
        uprv_free(new_State_పట్టిక);
    }
    తిరిగి నిజం;
}

/* reconstitute omitted fromUnicode data ------------------------------------ */

/* for details, compare with genmbcs.c MBCSAddFromUnicode() and transformEUC() */
స్థిర UBool U_CALLCONV
writeStage3Roundtrip(స్థిరరాశి శూన్య *context, uint32_t value, UChar32 సంకేతం_Points[32]) {
    UConverterMBCSTable *mbcs_పట్టిక=(UConverterMBCSTable *)context;
    స్థిరరాశి uint16_t *పట్టిక;
    uint32_t *stage2;
    uint8_t *బైట్లు, *p;
    UChar32 c;
    int32_t i, st3;

    పట్టిక=mbcs_పట్టిక->from_Unicode_పట్టిక;
    బైట్లు = const_cast<uint8_t*>(mbcs_పట్టిక->from_Unicode_బైట్లు);

    /* for EUC outputTypes, modify the value like genmbcs.c's transformEUC() */
    ఎంపిక(mbcs_పట్టిక->outputType) {
    స్థితి MBCS_OUTPUT_3_EUC:
        ఒకవేళ(value<=0xffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } లేకపోతే ఒకవేళ(value<=0x8effff) {
            /* code set 2 */
            value&=0x7fff;
        } లేకపోతే /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7f;
        }
        ఆపు;
    స్థితి MBCS_OUTPUT_4_EUC:
        ఒకవేళ(value<=0xffffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } లేకపోతే ఒకవేళ(value<=0x8effffff) {
            /* code set 2 */
            value&=0x7fffff;
        } లేకపోతే /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7fff;
        }
        ఆపు;
    అప్రమేయ:
        ఆపు;
    }

    కొరకు(i=0; i<=0x1f; ++value, ++i) {
        c=సంకేతం_Points[i];
        ఒకవేళ(c<0) {
            కొనసాగు;
        }

        /* locate the stage 2 & 3 data */
        stage2=((uint32_t *)పట్టిక)+పట్టిక[c>>10]+((c>>4)&0x3f);
        p=బైట్లు;
        st3 = static_cast<int32_t>(static_cast<uint16_t>(*stage2)) * 16 + (c & 0xf);

        /* write the codepage bytes into stage 3 */
        ఎంపిక(mbcs_పట్టిక->outputType) {
        స్థితి MBCS_OUTPUT_3:
        స్థితి MBCS_OUTPUT_4_EUC:
            p+=st3*3;
            p[0] = static_cast<uint8_t>(value >> 16);
            p[1] = static_cast<uint8_t>(value >> 8);
            p[2] = static_cast<uint8_t>(value);
            ఆపు;
        స్థితి MBCS_OUTPUT_4:
            reinterpret_cast<uint32_t*>(p)[st3] = value;
            ఆపు;
        అప్రమేయ:
            /* 2 bytes per character */
            reinterpret_cast<uint16_t*>(p)[st3] = static_cast<uint16_t>(value);
            ఆపు;
        }

        /* set the roundtrip flag */
        *stage2|=(1UL<<(16+(c&0xf)));
    }
    తిరిగి నిజం;
 }

స్థిర శూన్య
reconstitute_దత్తాంశం(UConverterMBCSTable *mbcs_పట్టిక,
                 uint32_t stage1_నిడివి, uint32_t stage2_నిడివి,
                 uint32_t నిండిన_Stage2_నిడివి,  /* lengths are numbers of units, not bytes */
                 UError_సంకేతం *p_Error_సంకేతం) {
    uint16_t *stage1;
    uint32_t *stage2;
    uint32_t దత్తాంశం_నిడివి=stage1_నిడివి*2+నిండిన_Stage2_నిడివి*4+mbcs_పట్టిక->from_UBytes_నిడివి;
    mbcs_పట్టిక->reconstituted_దత్తాంశం = static_cast<uint8_t*>(uprv_malloc(దత్తాంశం_నిడివి));
    ఒకవేళ(mbcs_పట్టిక->reconstituted_దత్తాంశం==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    uprv_memset(mbcs_పట్టిక->reconstituted_దత్తాంశం, 0, దత్తాంశం_నిడివి);

    /* copy existing data and reroute the pointers */
    stage1 = reinterpret_cast<uint16_t*>(mbcs_పట్టిక->reconstituted_దత్తాంశం);
    uprv_memcpy(stage1, mbcs_పట్టిక->from_Unicode_పట్టిక, stage1_నిడివి*2);

    stage2 = reinterpret_cast<uint32_t*>(stage1 + stage1_నిడివి);
    uprv_memcpy(stage2+(నిండిన_Stage2_నిడివి-stage2_నిడివి),
                mbcs_పట్టిక->from_Unicode_పట్టిక+stage1_నిడివి,
                stage2_నిడివి*4);

    mbcs_పట్టిక->from_Unicode_పట్టిక=stage1;
    mbcs_పట్టిక->from_Unicode_బైట్లు = reinterpret_cast<uint8_t*>(stage2 + నిండిన_Stage2_నిడివి);

    /* indexes into stage 2 count from the bottom of the fromUnicodeTable */
    stage2 = reinterpret_cast<uint32_t*>(stage1);

    /* reconstitute the initial part of stage 2 from the mbcsIndex */
    {
        int32_t stage_UTF8_నిడివి = (static_cast<int32_t>(mbcs_పట్టిక->maxFastUChar) + 1) >> 6;
        int32_t stage_UTF8_సూచీ=0;
        int32_t st1, st2, st3, i;

        కొరకు(st1=0; stage_UTF8_సూచీ<stage_UTF8_నిడివి; ++st1) {
            st2=stage1[st1];
            ఒకవేళ (st2 != static_cast<int32_t>(stage1_నిడివి) / 2) {
                /* each stage 2 block has 64 entries corresponding to 16 entries in the mbcsIndex */
                కొరకు(i=0; i<16; ++i) {
                    st3=mbcs_పట్టిక->mbcs_సూచీ[stage_UTF8_సూచీ++];
                    ఒకవేళ(st3!=0) {
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
                    } లేకపోతే {
                        /* no stage 3 block, skip */
                        st2+=4;
                    }
                }
            } లేకపోతే {
                /* no stage 2 block, skip */
                stage_UTF8_సూచీ+=16;
            }
        }
    }

    /* reconstitute fromUnicodeBytes with roundtrips from toUnicode data */
    ucnv_MBCSEnum_గమ్యం_Unicode(mbcs_పట్టిక, writeStage3Roundtrip, mbcs_పట్టిక, p_Error_సంకేతం);
}

/* MBCS setup functions ----------------------------------------------------- */

స్థిర శూన్య U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం,
          UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
          స్థిరరాశి uint8_t *raw,
          UError_సంకేతం *p_Error_సంకేతం) {
    UDataInfo info;
    UConverterMBCSTable *mbcs_పట్టిక=&shared_దత్తాంశం->mbcs;
    _MBCSHeader *శీర్షిక=(_MBCSHeader *)raw;
    uint32_t స్థానభ్రంశం;
    uint32_t శీర్షిక_నిడివి;
    UBool noFromU=అబద్ధం;

    ఒకవేళ(శీర్షిక->సంస్కరణ[0]==4) {
        శీర్షిక_నిడివి=MBCS_శీర్షిక_V4_నిడివి;
    } లేకపోతే ఒకవేళ(శీర్షిక->సంస్కరణ[0]==5 && శీర్షిక->సంస్కరణ[1]>=3 &&
              (శీర్షిక->options&MBCS_OPT_UNKNOWN_INCOMPATIBLE_ముసుగు)==0) {
        శీర్షిక_నిడివి=శీర్షిక->options&MBCS_OPT_నిడివి_ముసుగు;
        noFromU = static_cast<UBool>((శీర్షిక->options & MBCS_OPT_NO_FROM_U) != 0);
    } లేకపోతే {
        *p_Error_సంకేతం=U_INVALID_పట్టిక_FORMAT;
        తిరిగి;
    }

    mbcs_పట్టిక->outputType = static_cast<uint8_t>(శీర్షిక->flags);
    ఒకవేళ(noFromU && mbcs_పట్టిక->outputType==MBCS_OUTPUT_1) {
        *p_Error_సంకేతం=U_INVALID_పట్టిక_FORMAT;
        తిరిగి;
    }

    /* extension data, header version 4.2 and higher */
    స్థానభ్రంశం=శీర్షిక->flags>>8;
    ఒకవేళ(స్థానభ్రంశం!=0) {
        mbcs_పట్టిక->extIndexes = reinterpret_cast<స్థిరరాశి int32_t*>(raw + స్థానభ్రంశం);
    }

    ఒకవేళ(mbcs_పట్టిక->outputType==MBCS_OUTPUT_EXT_ONLY) {
        UConverter_ఎక్కించు_వాదనలు వాదనలు_2=UCNV_ఎక్కించు_వాదనలు_INITIALIZER;
        UConverter_Shared_దత్తాంశం *పునాది_Shared_దత్తాంశం;
        స్థిరరాశి int32_t *extIndexes;
        స్థిరరాశి అక్షరం *పునాది_Name;

        /* extension-only file, load the base table and set values appropriately */
        ఒకవేళ((extIndexes=mbcs_పట్టిక->extIndexes)==శూన్యసూచిక) {
            /* extension-only file without extension */
            *p_Error_సంకేతం=U_INVALID_పట్టిక_FORMAT;
            తిరిగి;
        }

        ఒకవేళ(p_వాదనలు->nestedLoads!=1) {
            /* an extension table must not be loaded as a base table */
            *p_Error_సంకేతం=U_INVALID_పట్టిక_FILE;
            తిరిగి;
        }

        /* load the base table */
        పునాది_Name = reinterpret_cast<స్థిరరాశి అక్షరం*>(శీర్షిక) + శీర్షిక_నిడివి * 4;
        ఒకవేళ(0==uprv_strcmp(పునాది_Name, shared_దత్తాంశం->static_దత్తాంశం->name)) {
            /* forbid loading this same extension-only file */
            *p_Error_సంకేతం=U_INVALID_పట్టిక_FORMAT;
            తిరిగి;
        }

        /* TODO parse package name out of the prefix of the base name in the extension .cnv file? */
        వాదనలు_2.size=పరిమాణం(UConverter_ఎక్కించు_వాదనలు);
        వాదనలు_2.nestedLoads=2;
        వాదనలు_2.onlyTestIsLoadable=p_వాదనలు->onlyTestIsLoadable;
        వాదనలు_2.reserved=p_వాదనలు->reserved;
        వాదనలు_2.options=p_వాదనలు->options;
        వాదనలు_2.pkg=p_వాదనలు->pkg;
        వాదనలు_2.name=పునాది_Name;
        పునాది_Shared_దత్తాంశం=ucnv_ఎక్కించు(&వాదనలు_2, p_Error_సంకేతం);
        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            తిరిగి;
        }
        ఒకవేళ( పునాది_Shared_దత్తాంశం->static_దత్తాంశం->conversionType!=UCNV_MBCS ||
            పునాది_Shared_దత్తాంశం->mbcs.పునాది_Shared_దత్తాంశం!=శూన్యసూచిక
        ) {
            ucnv_unload(పునాది_Shared_దత్తాంశం);
            *p_Error_సంకేతం=U_INVALID_పట్టిక_FORMAT;
            తిరిగి;
        }
        ఒకవేళ(p_వాదనలు->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            ucnv_unload(పునాది_Shared_దత్తాంశం);
            తిరిగి;
        }

        /* copy the base table data */
        uprv_memcpy(mbcs_పట్టిక, &పునాది_Shared_దత్తాంశం->mbcs, పరిమాణం(UConverterMBCSTable));

        /* overwrite values with relevant ones for the extension converter */
        mbcs_పట్టిక->పునాది_Shared_దత్తాంశం=పునాది_Shared_దత్తాంశం;
        mbcs_పట్టిక->extIndexes=extIndexes;

        /*
         * It would be possible to share the swapLFNL data with a base converter,
         * but the generated name would have to be different, and the memory
         * would have to be free'd only once.
         * It is easier to just create the data for the extension converter
         * separately when it is requested.
         */
        mbcs_పట్టిక->మార్పిడి_LFNLState_పట్టిక=శూన్యసూచిక;
        mbcs_పట్టిక->మార్పిడి_LFNLFrom_Unicode_బైట్లు=శూన్యసూచిక;
        mbcs_పట్టిక->మార్పిడి_LFNLName=శూన్యసూచిక;

        /*
         * The reconstitutedData must be deleted only when the base converter
         * is unloaded.
         */
        mbcs_పట్టిక->reconstituted_దత్తాంశం=శూన్యసూచిక;

        /*
         * Set a special, runtime-only outputType if the extension converter
         * is a DBCS version of a base converter that also maps single bytes.
         */
        ఒకవేళ( shared_దత్తాంశం->static_దత్తాంశం->conversionType==UCNV_DBCS ||
                (shared_దత్తాంశం->static_దత్తాంశం->conversionType==UCNV_MBCS &&
                 shared_దత్తాంశం->static_దత్తాంశం->min_బైట్లు_చొప్పున_Char>=2)
        ) {
            ఒకవేళ(పునాది_Shared_దత్తాంశం->mbcs.outputType==MBCS_OUTPUT_2_SISO) {
                /* the base converter is SI/SO-stateful */
                int32_t నమోదు_2;

                /* get the dbcs state from the state table entry for SO=0x0e */
                నమోదు_2=mbcs_పట్టిక->state_పట్టిక[0][0xe];
                ఒకవేళ( MBCS_నమోదు_IS_FINAL(నమోదు_2) &&
                    MBCS_నమోదు_FINAL_ACTION(నమోదు_2)==MBCS_STATE_CHANGE_ONLY &&
                    MBCS_నమోదు_FINAL_STATE(నమోదు_2)!=0
                ) {
                    mbcs_పట్టిక->dbcsOnlyState = static_cast<uint8_t>(MBCS_నమోదు_FINAL_STATE(నమోదు_2));

                    mbcs_పట్టిక->outputType=MBCS_OUTPUT_DBCS_ONLY;
                }
            } లేకపోతే ఒకవేళ(
                పునాది_Shared_దత్తాంశం->static_దత్తాంశం->conversionType==UCNV_MBCS &&
                పునాది_Shared_దత్తాంశం->static_దత్తాంశం->min_బైట్లు_చొప్పున_Char==1 &&
                పునాది_Shared_దత్తాంశం->static_దత్తాంశం->max_బైట్లు_చొప్పున_Char==2 &&
                mbcs_పట్టిక->లెక్క_స్థితులు<=127
            ) {
                /* non-stateful base converter, need to modify the state table */
                int32_t (*new_State_పట్టిక)[256];
                int32_t *state;
                int32_t i, count;

                /* allocate a new state table and copy the base state table contents */
                count=mbcs_పట్టిక->లెక్క_స్థితులు;
                new_State_పట్టిక = static_cast<int32_t(*)[256]>(uprv_malloc((count + 1) * 1024));
                ఒకవేళ(new_State_పట్టిక==శూన్యసూచిక) {
                    ucnv_unload(పునాది_Shared_దత్తాంశం);
                    *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    తిరిగి;
                }

                uprv_memcpy(new_State_పట్టిక, mbcs_పట్టిక->state_పట్టిక, count*1024);

                /* change all final single-byte entries to go to a new all-illegal state */
                state=new_State_పట్టిక[0];
                కొరకు(i=0; i<256; ++i) {
                    ఒకవేళ(MBCS_నమోదు_IS_FINAL(state[i])) {
                        state[i]=MBCS_నమోదు_TRANSITION(count, 0);
                    }
                }

                /* build the new all-illegal state */
                state=new_State_పట్టిక[count];
                కొరకు(i=0; i<256; ++i) {
                    state[i]=MBCS_నమోదు_FINAL(0, MBCS_STATE_ILLEGAL, 0);
                }
                mbcs_పట్టిక->state_పట్టిక=(స్థిరరాశి int32_t (*)[256])new_State_పట్టిక;
                mbcs_పట్టిక->లెక్క_స్థితులు = static_cast<uint8_t>(count + 1);
                mbcs_పట్టిక->state_పట్టిక_Owned=నిజం;

                mbcs_పట్టిక->outputType=MBCS_OUTPUT_DBCS_ONLY;
            }
        }

        /*
         * unlike below for files with base tables, do not get the unicodeMask
         * from the sharedData; instead, use the base table's unicodeMask,
         * which we copied in the memcpy above;
         * this is necessary because the static data unicodeMask, especially
         * the UCNV_HAS_SUPPLEMENTARY flag, is part of the base table data
         */
    } లేకపోతే {
        /* conversion file with a base table; an additional extension table is optional */
        /* make sure that the output type is known */
        ఎంపిక(mbcs_పట్టిక->outputType) {
        స్థితి MBCS_OUTPUT_1:
        స్థితి MBCS_OUTPUT_2:
        స్థితి MBCS_OUTPUT_3:
        స్థితి MBCS_OUTPUT_4:
        స్థితి MBCS_OUTPUT_3_EUC:
        స్థితి MBCS_OUTPUT_4_EUC:
        స్థితి MBCS_OUTPUT_2_SISO:
            /* OK */
            ఆపు;
        అప్రమేయ:
            *p_Error_సంకేతం=U_INVALID_పట్టిక_FORMAT;
            తిరిగి;
        }
        ఒకవేళ(p_వాదనలు->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            తిరిగి;
        }

        mbcs_పట్టిక->లెక్క_స్థితులు = static_cast<uint8_t>(శీర్షిక->లెక్క_స్థితులు);
        mbcs_పట్టిక->లెక్క_గమ్యం_UFallbacks=శీర్షిక->లెక్క_గమ్యం_UFallbacks;
        mbcs_పట్టిక->state_పట్టిక = reinterpret_cast<స్థిరరాశి int32_t(*)[256]>(raw + శీర్షిక_నిడివి * 4);
        mbcs_పట్టిక->గమ్యం_UFallbacks = reinterpret_cast<స్థిరరాశి _MBCSToUFallback*>(mbcs_పట్టిక->state_పట్టిక + శీర్షిక->లెక్క_స్థితులు);
        mbcs_పట్టిక->unicode_సంకేతం_Units = reinterpret_cast<స్థిరరాశి uint16_t*>(raw + శీర్షిక->స్థానభ్రంశం_గమ్యం_UCode_Units);

        mbcs_పట్టిక->from_Unicode_పట్టిక = reinterpret_cast<స్థిరరాశి uint16_t*>(raw + శీర్షిక->స్థానభ్రంశం_From_UTable);
        mbcs_పట్టిక->from_Unicode_బైట్లు = raw + శీర్షిక->స్థానభ్రంశం_From_UBytes;
        mbcs_పట్టిక->from_UBytes_నిడివి=శీర్షిక->from_UBytes_నిడివి;

        /*
         * converter versions 6.1 and up contain a unicodeMask that is
         * used here to select the most efficient function implementations
         */
        info.size=పరిమాణం(UDataInfo);
        udata_getInfo((UData_జ్ఞాపకం *)shared_దత్తాంశం->దత్తాంశం_జ్ఞాపకం, &info);
        ఒకవేళ(info.format_సంస్కరణ[0]>6 || (info.format_సంస్కరణ[0]==6 && info.format_సంస్కరణ[1]>=1)) {
            /* mask off possible future extensions to be safe */
            mbcs_పట్టిక->unicode_ముసుగు = static_cast<uint8_t>(shared_దత్తాంశం->static_దత్తాంశం->unicode_ముసుగు & 3);
        } లేకపోతే {
            /* for older versions, assume worst case: contains anything possible (prevent over-optimizations) */
            mbcs_పట్టిక->unicode_ముసుగు=UCNV_HAS_SUPPLEMENTARY|UCNV_HAS_SURROGATES;
        }

        /*
         * _MBCSHeader.version 4.3 adds utf8Friendly data structures.
         * Check for the header version, SBCS vs. MBCS, and for whether the
         * data structures are optimized for code points as high as what the
         * runtime code is designed for.
         * The implementation does not handle mapping tables with entries for
         * unpaired surrogates.
         */
        ఒకవేళ( శీర్షిక->సంస్కరణ[1]>=3 &&
            (mbcs_పట్టిక->unicode_ముసుగు&UCNV_HAS_SURROGATES)==0 &&
            (mbcs_పట్టిక->లెక్క_స్థితులు==1 ?
                (శీర్షిక->సంస్కరణ[2]>=(SBCS_FAST_MAX>>8)) :
                (శీర్షిక->సంస్కరణ[2]>=(MBCS_FAST_MAX>>8))
            )
        ) {
            mbcs_పట్టిక->utf8Friendly=నిజం;

            ఒకవేళ(mbcs_పట్టిక->లెక్క_స్థితులు==1) {
                /*
                 * SBCS: Stage 3 is allocated in 64-entry blocks for U+0000..SBCS_FAST_MAX or higher.
                 * Build a table with indexes to each block, to be used instead of
                 * the regular stage 1/2 table.
                 */
                int32_t i;
                కొరకు(i=0; i<(SBCS_FAST_హద్దు>>6); ++i) {
                    mbcs_పట్టిక->sbcs_సూచీ[i]=mbcs_పట్టిక->from_Unicode_పట్టిక[mbcs_పట్టిక->from_Unicode_పట్టిక[i>>4]+((i<<2)&0x3c)];
                }
                /* set SBCS_FAST_MAX to reflect the reach of sbcsIndex[] even if header->version[2]>(SBCS_FAST_MAX>>8) */
                mbcs_పట్టిక->maxFastUChar=SBCS_FAST_MAX;
            } లేకపోతే {
                /*
                 * MBCS: Stage 3 is allocated in 64-entry blocks for U+0000..MBCS_FAST_MAX or higher.
                 * The .cnv file is prebuilt with an additional stage table with indexes
                 * to each block.
                 */
                mbcs_పట్టిక->mbcs_సూచీ = reinterpret_cast<స్థిరరాశి uint16_t*>(
                    mbcs_పట్టిక->from_Unicode_బైట్లు +
                     (noFromU ? 0 : mbcs_పట్టిక->from_UBytes_నిడివి));
                mbcs_పట్టిక->maxFastUChar = (static_cast<char16_t>(శీర్షిక->సంస్కరణ[2]) << 8) | 0xff;
            }
        }

        /* calculate a bit set of 4 ASCII characters per bit that round-trip to ASCII bytes */
        {
            uint32_t asciiRoundtrips=0xffffffff;
            int32_t i;

            కొరకు(i=0; i<0x80; ++i) {
                ఒకవేళ(mbcs_పట్టిక->state_పట్టిక[0][i]!=MBCS_నమోదు_FINAL(0, MBCS_STATE_VALID_ప్రత్యక్ష_16, i)) {
                    asciiRoundtrips &= ~(static_cast<uint32_t>(1) << (i >> 2));
                }
            }
            mbcs_పట్టిక->asciiRoundtrips=asciiRoundtrips;
        }

        ఒకవేళ(noFromU) {
            uint32_t stage1_నిడివి=
                mbcs_పట్టిక->unicode_ముసుగు&UCNV_HAS_SUPPLEMENTARY ?
                    0x440 : 0x40;
            uint32_t stage2_నిడివి=
                (శీర్షిక->స్థానభ్రంశం_From_UBytes-శీర్షిక->స్థానభ్రంశం_From_UTable)/4-
                stage1_నిడివి/2;
            reconstitute_దత్తాంశం(mbcs_పట్టిక, stage1_నిడివి, stage2_నిడివి, శీర్షిక->నిండిన_Stage2_నిడివి, p_Error_సంకేతం);
        }
    }

    /* Set the impl pointer here so that it is set for both extension-only and base tables. */
    ఒకవేళ(mbcs_పట్టిక->utf8Friendly) {
        ఒకవేళ(mbcs_పట్టిక->లెక్క_స్థితులు==1) {
            shared_దత్తాంశం->impl=&_SBCSUTF8Impl;
        } లేకపోతే {
            ఒకవేళ(mbcs_పట్టిక->outputType==MBCS_OUTPUT_2) {
                shared_దత్తాంశం->impl=&_DBCSUTF8Impl;
            }
        }
    }

    ఒకవేళ(mbcs_పట్టిక->outputType==MBCS_OUTPUT_DBCS_ONLY || mbcs_పట్టిక->outputType==MBCS_OUTPUT_2_SISO) {
        /*
         * MBCS_OUTPUT_DBCS_ONLY: No SBCS mappings, therefore ASCII does not roundtrip.
         * MBCS_OUTPUT_2_SISO: Bypass the ASCII fastpath to handle prevLength correctly.
         */
        mbcs_పట్టిక->asciiRoundtrips=0;
    }
}

స్థిర శూన్య U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం) {
    UConverterMBCSTable *mbcs_పట్టిక=&shared_దత్తాంశం->mbcs;

    ఒకవేళ(mbcs_పట్టిక->మార్పిడి_LFNLState_పట్టిక!=శూన్యసూచిక) {
        uprv_free(mbcs_పట్టిక->మార్పిడి_LFNLState_పట్టిక);
    }
    ఒకవేళ(mbcs_పట్టిక->state_పట్టిక_Owned) {
        uprv_free((శూన్య *)mbcs_పట్టిక->state_పట్టిక);
    }
    ఒకవేళ(mbcs_పట్టిక->పునాది_Shared_దత్తాంశం!=శూన్యసూచిక) {
        ucnv_unload(mbcs_పట్టిక->పునాది_Shared_దత్తాంశం);
    }
    ఒకవేళ(mbcs_పట్టిక->reconstituted_దత్తాంశం!=శూన్యసూచిక) {
        uprv_free(mbcs_పట్టిక->reconstituted_దత్తాంశం);
    }
}

స్థిర శూన్య U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
              UError_సంకేతం *p_Error_సంకేతం) {
    UConverterMBCSTable *mbcs_పట్టిక;
    స్థిరరాశి int32_t *extIndexes;
    uint8_t outputType;
    int8_t max_బైట్లు_చొప్పున_UChar;

    ఒకవేళ(p_వాదనలు->onlyTestIsLoadable) {
        తిరిగి;
    }

    mbcs_పట్టిక=&cnv->shared_దత్తాంశం->mbcs;
    outputType=mbcs_పట్టిక->outputType;

    ఒకవేళ(outputType==MBCS_OUTPUT_DBCS_ONLY) {
        /* the swaplfnl option does not apply, remove it */
        cnv->options=p_వాదనలు->options&=~UCNV_OPTION_మార్పిడి_LFNL;
    }

    ఒకవేళ((p_వాదనలు->options&UCNV_OPTION_మార్పిడి_LFNL)!=0) {
        /* do this because double-checked locking is broken */
        UBool isCached;

        icu::umtx_తాళం(శూన్యసూచిక);
        isCached=mbcs_పట్టిక->మార్పిడి_LFNLState_పట్టిక!=శూన్యసూచిక;
        icu::umtx_unlock(శూన్యసూచిక);

        ఒకవేళ(!isCached) {
            ఒకవేళ(!_EBCDICSwapLFNL(cnv->shared_దత్తాంశం, p_Error_సంకేతం)) {
                ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                    తిరిగి; /* something went wrong */
                }

                /* the option does not apply, remove it */
                cnv->options=p_వాదనలు->options&=~UCNV_OPTION_మార్పిడి_LFNL;
            }
        }
    }

    ఒకవేళ(uprv_strstr(p_వాదనలు->name, "18030")!=శూన్యసూచిక) {
        ఒకవేళ(uprv_strstr(p_వాదనలు->name, "gb18030")!=శూన్యసూచిక || uprv_strstr(p_వాదనలు->name, "GB18030")!=శూన్యసూచిక) {
            /* set a flag for GB 18030 mode, which changes the callback behavior */
            cnv->options|=_MBCS_OPTION_GB18030;
        }
    } లేకపోతే ఒకవేళ((uprv_strstr(p_వాదనలు->name, "KEIS")!=శూన్యసూచిక) || (uprv_strstr(p_వాదనలు->name, "keis")!=శూన్యసూచిక)) {
        /* set a flag for KEIS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_KEIS;
    } లేకపోతే ఒకవేళ((uprv_strstr(p_వాదనలు->name, "JEF")!=శూన్యసూచిక) || (uprv_strstr(p_వాదనలు->name, "jef")!=శూన్యసూచిక)) {
        /* set a flag for JEF converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JEF;
    } లేకపోతే ఒకవేళ((uprv_strstr(p_వాదనలు->name, "JIPS")!=శూన్యసూచిక) || (uprv_strstr(p_వాదనలు->name, "jips")!=శూన్యసూచిక)) {
        /* set a flag for JIPS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JIPS;
    }

    /* fix maxBytesPerUChar depending on outputType and options etc. */
    ఒకవేళ(outputType==MBCS_OUTPUT_2_SISO) {
        cnv->max_బైట్లు_చొప్పున_UChar=3; /* SO+DBCS */
    }

    extIndexes=mbcs_పట్టిక->extIndexes;
    ఒకవేళ(extIndexes!=శూన్యసూచిక) {
        max_బైట్లు_చొప్పున_UChar = static_cast<int8_t>(UCNV_GET_MAX_బైట్లు_చొప్పున_UCHAR(extIndexes));
        ఒకవేళ(outputType==MBCS_OUTPUT_2_SISO) {
            ++max_బైట్లు_చొప్పున_UChar; /* SO + multiple DBCS */
        }

        ఒకవేళ(max_బైట్లు_చొప్పున_UChar>cnv->max_బైట్లు_చొప్పున_UChar) {
            cnv->max_బైట్లు_చొప్పున_UChar=max_బైట్లు_చొప్పున_UChar;
        }
    }

#ఒకవేళ 0
    /*
     * documentation of UConverter fields used for status
     * all of these fields are (re)set to 0 by ucnv_bld.c and ucnv_reset()
     */

    /* toUnicode */
    cnv->గమ్యం_Unicode_స్థితి=0;     /*స్థానభ్రంశం*/
    cnv->బహుళకం=0;                /* state */
    cnv->గమ్యం_ULength=0;           /* byteIndex */

    /* fromUnicode */
    cnv->fromUChar32=0;
    cnv->from_Unicode_స్థితి=1;   /* prevLength */
#అయితే_ముగింపు
}

U_CDECL_BEGIN

స్థిర స్థిరరాశి అక్షరం* U_CALLCONV
ucnv_MBCSGetName(స్థిరరాశి UConverter *cnv) {
    ఒకవేళ((cnv->options&UCNV_OPTION_మార్పిడి_LFNL)!=0 && cnv->shared_దత్తాంశం->mbcs.మార్పిడి_LFNLName!=శూన్యసూచిక) {
        తిరిగి cnv->shared_దత్తాంశం->mbcs.మార్పిడి_LFNLName;
    } లేకపోతే {
        తిరిగి cnv->shared_దత్తాంశం->static_దత్తాంశం->name;
    }
}
U_CDECL_చివర


/* MBCS-to-Unicode conversion functions ------------------------------------- */

స్థిర UChar32 U_CALLCONV
ucnv_MBCSGetFallback(UConverterMBCSTable *mbcs_పట్టిక, uint32_t స్థానభ్రంశం) {
    స్థిరరాశి _MBCSToUFallback *గమ్యం_UFallbacks;
    uint32_t i, ప్రారంభం, హద్దు;

    హద్దు=mbcs_పట్టిక->లెక్క_గమ్యం_UFallbacks;
    ఒకవేళ(హద్దు>0) {
        /* do a binary search for the fallback mapping */
        గమ్యం_UFallbacks=mbcs_పట్టిక->గమ్యం_UFallbacks;
        ప్రారంభం=0;
        అయితే(ప్రారంభం<హద్దు-1) {
            i=(ప్రారంభం+హద్దు)/2;
            ఒకవేళ(స్థానభ్రంశం<గమ్యం_UFallbacks[i].స్థానభ్రంశం) {
                హద్దు=i;
            } లేకపోతే {
                ప్రారంభం=i;
            }
        }

        /* did we really find it? */
        ఒకవేళ(స్థానభ్రంశం==గమ్యం_UFallbacks[ప్రారంభం].స్థానభ్రంశం) {
            తిరిగి గమ్యం_UFallbacks[ప్రారంభం].సంకేతం_బిందువు;
        }
    }

    తిరిగి 0xfffe;
}

/* This version of ucnv_MBCSToUnicodeWithOffsets() is optimized for single-byte, single-state codepages. */
స్థిర శూన్య
ucnv_MBCSSingle_గమ్యం_Unicode_With_Offsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                                UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    char16_t *లక్ష్యం;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు;
    int32_t *offsets;

    స్థిరరాశి int32_t (*state_పట్టిక)[256];

    int32_t ఆధారం_సూచీ;

    int32_t నమోదు_2;
    char16_t c;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    ఆధారం = reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->ఆధారం);
    ఆధారం_హద్దు = reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->ఆధారం_హద్దు);
    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    లక్ష్యం_హద్దు=p_వాదనలు->లక్ష్యం_హద్దు;
    offsets=p_వాదనలు->offsets;

    ఒకవేళ((cnv->options&UCNV_OPTION_మార్పిడి_LFNL)!=0) {
        state_పట్టిక=(స్థిరరాశి int32_t (*)[256])cnv->shared_దత్తాంశం->mbcs.మార్పిడి_LFNLState_పట్టిక;
    } లేకపోతే {
        state_పట్టిక=cnv->shared_దత్తాంశం->mbcs.state_పట్టిక;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ=0;

    /* conversion loop */
    అయితే(ఆధారం<ఆధారం_హద్దు) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ఒకవేళ(లక్ష్యం>=లక్ష్యం_హద్దు) {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }

        నమోదు_2=state_పట్టిక[0][*ఆధారం++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        ఒకవేళ(MBCS_నమోదు_FINAL_IS_VALID_ప్రత్యక్ష_16(నమోదు_2)) {
            /* output BMP code point */
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            ఒకవేళ(offsets!=శూన్యసూచిక) {
                *offsets++=ఆధారం_సూచీ;
            }

            /* normal end of action codes: prepare for a new character */
            ++ఆధారం_సూచీ;
            కొనసాగు;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_నమోదు_FINAL_ACTION(నమోదు_2));
        ఒకవేళ(action==MBCS_STATE_VALID_ప్రత్యక్ష_20 ||
           (action==MBCS_STATE_FALLBACK_ప్రత్యక్ష_20 && UCNV_గమ్యం_U_USE_FALLBACK(cnv))
        ) {
            నమోదు_2=MBCS_నమోదు_FINAL_విలువ(నమోదు_2);
            /* output surrogate pair */
            *లక్ష్యం++ = static_cast<char16_t>(0xd800 | static_cast<char16_t>(నమోదు_2 >> 10));
            ఒకవేళ(offsets!=శూన్యసూచిక) {
                *offsets++=ఆధారం_సూచీ;
            }
            c = static_cast<char16_t>(0xdc00 | static_cast<char16_t>(నమోదు_2 & 0x3ff));
            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                *లక్ష్యం++=c;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
            } లేకపోతే {
                /* target overflow */
                cnv->UChar_Error_బఫరు[0]=c;
                cnv->UChar_Error_బఫరు_నిడివి=1;
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }

            ++ఆధారం_సూచీ;
            కొనసాగు;
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_FALLBACK_ప్రత్యక్ష_16) {
            ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }

                ++ఆధారం_సూచీ;
                కొనసాగు;
            }
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
        } లేకపోతే {
            /* reserved, must never occur */
            ++ఆధారం_సూచీ;
            కొనసాగు;
        }

        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            /* callback(illegal) */
            ఆపు;
        } లేకపోతే /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_వాదనలు->ఆధారం = reinterpret_cast<స్థిరరాశి అక్షరం*>(ఆధారం);
            cnv->గమ్యం_UBytes[0]=*(ఆధారం-1);
            cnv->గమ్యం_ULength=ext_గమ్యం_U(cnv, cnv->shared_దత్తాంశం,
                                    1, &ఆధారం, ఆధారం_హద్దు,
                                    &లక్ష్యం, లక్ష్యం_హద్దు,
                                    &offsets, ఆధారం_సూచీ,
                                    p_వాదనలు->flush,
                                    p_Error_సంకేతం);
            ఆధారం_సూచీ += 1 + static_cast<int32_t>(ఆధారం - reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->ఆధారం));

            ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                /* not mappable or buffer overflow */
                ఆపు;
            }
        }
    }

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం = reinterpret_cast<స్థిరరాశి అక్షరం*>(ఆధారం);
    p_వాదనలు->లక్ష్యం=లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

/*
 * This version of ucnv_MBCSSingleToUnicodeWithOffsets() is optimized for single-byte, single-state codepages
 * that only map to and from the BMP.
 * In addition to single-byte optimizations, the offset calculations
 * become much easier.
 */
స్థిర శూన్య
ucnv_MBCSSingle_గమ్యం_BMPWith_Offsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                            UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు, *చివరి_ఆధారం;
    char16_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం, length;
    int32_t *offsets;

    స్థిరరాశి int32_t (*state_పట్టిక)[256];

    int32_t ఆధారం_సూచీ;

    int32_t నమోదు_2;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    ఆధారం = reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->ఆధారం);
    ఆధారం_హద్దు = reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->ఆధారం_హద్దు);
    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(p_వాదనలు->లక్ష్యం_హద్దు - p_వాదనలు->లక్ష్యం);
    offsets=p_వాదనలు->offsets;

    ఒకవేళ((cnv->options&UCNV_OPTION_మార్పిడి_LFNL)!=0) {
        state_పట్టిక=(స్థిరరాశి int32_t (*)[256])cnv->shared_దత్తాంశం->mbcs.మార్పిడి_LFNLState_పట్టిక;
    } లేకపోతే {
        state_పట్టిక=cnv->shared_దత్తాంశం->mbcs.state_పట్టిక;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ=0;
    చివరి_ఆధారం=ఆధారం;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(ఆధారం_హద్దు - ఆధారం);
    ఒకవేళ(length<లక్ష్యం_సామర్థ్యం) {
        లక్ష్యం_సామర్థ్యం=length;
    }

#ఒకవేళ MBCS_UNROLL_SINGLE_గమ్యం_BMP
    /* unrolling makes it faster on Pentium III/Windows 2000 */
    /* unroll the loop with the most common case */
unrolled:
    ఒకవేళ(లక్ష్యం_సామర్థ్యం>=16) {
        int32_t count, loops, oredEntries;

        loops=count=లక్ష్యం_సామర్థ్యం>>4;
        చేయి {
            oredEntries=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            oredEntries|=నమోదు_2=state_పట్టిక[0][*ఆధారం++];
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));

            /* were all 16 entries really valid? */
            ఒకవేళ(!MBCS_నమోదు_FINAL_IS_VALID_ప్రత్యక్ష_16(oredEntries)) {
                /* no, return to the first of these 16 */
                ఆధారం-=16;
                లక్ష్యం-=16;
                ఆపు;
            }
        } అయితే(--count>0);
        count=loops-count;
        లక్ష్యం_సామర్థ్యం-=16*count;

        ఒకవేళ(offsets!=శూన్యసూచిక) {
            చివరి_ఆధారం+=16*count;
            అయితే(count>0) {
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                --count;
            }
        }
    }
#అయితే_ముగింపు

    /* conversion loop */
    అయితే(లక్ష్యం_సామర్థ్యం > 0 && ఆధారం < ఆధారం_హద్దు) {
        నమోదు_2=state_పట్టిక[0][*ఆధారం++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        ఒకవేళ(MBCS_నమోదు_FINAL_IS_VALID_ప్రత్యక్ష_16(నమోదు_2)) {
            /* output BMP code point */
            *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            --లక్ష్యం_సామర్థ్యం;
            కొనసాగు;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_నమోదు_FINAL_ACTION(నమోదు_2));
        ఒకవేళ(action==MBCS_STATE_FALLBACK_ప్రత్యక్ష_16) {
            ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *లక్ష్యం++ = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
                --లక్ష్యం_సామర్థ్యం;
                కొనసాగు;
            }
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
        } లేకపోతే {
            /* reserved, must never occur */
            కొనసాగు;
        }

        /* set offsets since the start or the last extension */
        ఒకవేళ(offsets!=శూన్యసూచిక) {
            int32_t count = static_cast<int32_t>(ఆధారం - చివరి_ఆధారం);

            /* predecrement: do not set the offset for the callback-causing character */
            అయితే(--count>0) {
                *offsets++=ఆధారం_సూచీ++;
            }
            /* offset and sourceIndex are now set for the current character */
        }

        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            /* callback(illegal) */
            ఆపు;
        } లేకపోతే /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            చివరి_ఆధారం=ఆధారం;
            cnv->గమ్యం_UBytes[0]=*(ఆధారం-1);
            cnv->గమ్యం_ULength=ext_గమ్యం_U(cnv, cnv->shared_దత్తాంశం,
                                    1, &ఆధారం, ఆధారం_హద్దు,
                                    &లక్ష్యం, p_వాదనలు->లక్ష్యం_హద్దు,
                                    &offsets, ఆధారం_సూచీ,
                                    p_వాదనలు->flush,
                                    p_Error_సంకేతం);
            ఆధారం_సూచీ += 1 + static_cast<int32_t>(ఆధారం - చివరి_ఆధారం);

            ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                /* not mappable or buffer overflow */
                ఆపు;
            }

            /* recalculate the targetCapacity after an extension mapping */
            లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(p_వాదనలు->లక్ష్యం_హద్దు - లక్ష్యం);
            length = static_cast<int32_t>(ఆధారం_హద్దు - ఆధారం);
            ఒకవేళ(length<లక్ష్యం_సామర్థ్యం) {
                లక్ష్యం_సామర్థ్యం=length;
            }
        }

#ఒకవేళ MBCS_UNROLL_SINGLE_గమ్యం_BMP
        /* unrolling makes it faster on Pentium III/Windows 2000 */
        వెళ్ళు unrolled;
#అయితే_ముగింపు
    }

    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం) && ఆధారం<ఆధారం_హద్దు && లక్ష్యం>=p_వాదనలు->లక్ష్యం_హద్దు) {
        /* target is full */
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
    }

    /* set offsets since the start or the last callback */
    ఒకవేళ(offsets!=శూన్యసూచిక) {
        size_t count=ఆధారం-చివరి_ఆధారం;
        అయితే(count>0) {
            *offsets++=ఆధారం_సూచీ++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం = reinterpret_cast<స్థిరరాశి అక్షరం*>(ఆధారం);
    p_వాదనలు->లక్ష్యం=లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

స్థిర UBool
has_Valid_Trail_బైట్లు(స్థిరరాశి int32_t (*state_పట్టిక)[256], uint8_t state) {
    స్థిరరాశి int32_t *అడ్డువరుస=state_పట్టిక[state];
    int32_t b, నమోదు_2;
    /* First test for final entries in this state for some commonly valid byte values. */
    నమోదు_2=అడ్డువరుస[0xa1];
    ఒకవేళ( !MBCS_నమోదు_IS_TRANSITION(నమోదు_2) &&
        MBCS_నమోదు_FINAL_ACTION(నమోదు_2)!=MBCS_STATE_ILLEGAL
    ) {
        తిరిగి నిజం;
    }
    నమోదు_2=అడ్డువరుస[0x41];
    ఒకవేళ( !MBCS_నమోదు_IS_TRANSITION(నమోదు_2) &&
        MBCS_నమోదు_FINAL_ACTION(నమోదు_2)!=MBCS_STATE_ILLEGAL
    ) {
        తిరిగి నిజం;
    }
    /* Then test for final entries in this state. */
    కొరకు(b=0; b<=0xff; ++b) {
        నమోదు_2=అడ్డువరుస[b];
        ఒకవేళ( !MBCS_నమోదు_IS_TRANSITION(నమోదు_2) &&
            MBCS_నమోదు_FINAL_ACTION(నమోదు_2)!=MBCS_STATE_ILLEGAL
        ) {
            తిరిగి నిజం;
        }
    }
    /* Then recurse for transition entries. */
    కొరకు(b=0; b<=0xff; ++b) {
        నమోదు_2=అడ్డువరుస[b];
        ఒకవేళ( MBCS_నమోదు_IS_TRANSITION(నమోదు_2) &&
            has_Valid_Trail_బైట్లు(state_పట్టిక, static_cast<uint8_t>(MBCS_నమోదు_TRANSITION_STATE(నమోదు_2)))
        ) {
            తిరిగి నిజం;
        }
    }
    తిరిగి అబద్ధం;
}

/*
 * Is byte b a single/lead byte in this state?
 * Recurse for transition states, because here we don't want to say that
 * b is a lead byte if all byte sequences that start with b are illegal.
 */
స్థిర UBool
isSingleOrLead(స్థిరరాశి int32_t (*state_పట్టిక)[256], uint8_t state, UBool isDBCSOnly, uint8_t b) {
    స్థిరరాశి int32_t *అడ్డువరుస=state_పట్టిక[state];
    int32_t నమోదు_2=అడ్డువరుస[b];
    ఒకవేళ(MBCS_నమోదు_IS_TRANSITION(నమోదు_2)) {   /* lead byte */
        తిరిగి has_Valid_Trail_బైట్లు(state_పట్టిక, static_cast<uint8_t>(MBCS_నమోదు_TRANSITION_STATE(నమోదు_2)));
    } లేకపోతే {
        uint8_t action = static_cast<uint8_t>(MBCS_నమోదు_FINAL_ACTION(నమోదు_2));
        ఒకవేళ(action==MBCS_STATE_CHANGE_ONLY && isDBCSOnly) {
            తిరిగి అబద్ధం;   /* SI/SO are illegal for DBCS-only conversion */
        } లేకపోతే {
            తిరిగి action!=MBCS_STATE_ILLEGAL;
        }
    }
}

U_CFUNC శూన్య
ucnv_MBCSToUnicodeWithOffsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                          UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    char16_t *లక్ష్యం;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు;
    int32_t *offsets;

    స్థిరరాశి int32_t (*state_పట్టిక)[256];
    స్థిరరాశి uint16_t *unicode_సంకేతం_Units;

    uint32_t స్థానభ్రంశం;
    uint8_t state;
    int8_t byte_సూచీ;
    uint8_t *బైట్లు;

    int32_t ఆధారం_సూచీ, తదుపరి_ఆధారం_సూచీ;

    int32_t నమోదు_2;
    char16_t c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_వాదనలు->converter;

    ఒకవేళ(cnv->pre_గమ్యం_ULength>0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_సరిపోలిక_గమ్యం_U(cnv, p_వాదనలు, -1, p_Error_సంకేతం);

        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం) || cnv->pre_గమ్యం_ULength<0) {
            తిరిగి;
        }
    }

    ఒకవేళ(cnv->shared_దత్తాంశం->mbcs.లెక్క_స్థితులు==1) {
        ఒకవేళ(!(cnv->shared_దత్తాంశం->mbcs.unicode_ముసుగు&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingle_గమ్యం_BMPWith_Offsets(p_వాదనలు, p_Error_సంకేతం);
        } లేకపోతే {
            ucnv_MBCSSingle_గమ్యం_Unicode_With_Offsets(p_వాదనలు, p_Error_సంకేతం);
        }
        తిరిగి;
    }

    /* set up the local pointers */
    ఆధారం=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    లక్ష్యం_హద్దు=p_వాదనలు->లక్ష్యం_హద్దు;
    offsets=p_వాదనలు->offsets;

    ఒకవేళ((cnv->options&UCNV_OPTION_మార్పిడి_LFNL)!=0) {
        state_పట్టిక=(స్థిరరాశి int32_t (*)[256])cnv->shared_దత్తాంశం->mbcs.మార్పిడి_LFNLState_పట్టిక;
    } లేకపోతే {
        state_పట్టిక=cnv->shared_దత్తాంశం->mbcs.state_పట్టిక;
    }
    unicode_సంకేతం_Units=cnv->shared_దత్తాంశం->mbcs.unicode_సంకేతం_Units;

    /* get the converter state from UConverter */
    స్థానభ్రంశం=cnv->గమ్యం_Unicode_స్థితి;
    byte_సూచీ=cnv->గమ్యం_ULength;
    బైట్లు=cnv->గమ్యం_UBytes;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    ఒకవేళ((state=(uint8_t)(cnv->బహుళకం))==0) {
        state=cnv->shared_దత్తాంశం->mbcs.dbcsOnlyState;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ=byte_సూచీ==0 ? 0 : -1;
    తదుపరి_ఆధారం_సూచీ=0;

    /* conversion loop */
    అయితే(ఆధారం<ఆధారం_హద్దు) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ఒకవేళ(లక్ష్యం>=లక్ష్యం_హద్దు) {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }

        ఒకవేళ(byte_సూచీ==0) {
            /* optimized loop for 1/2-byte input and BMP output */
            ఒకవేళ(offsets==శూన్యసూచిక) {
                చేయి {
                    నమోదు_2=state_పట్టిక[state][*ఆధారం];
                    ఒకవేళ(MBCS_నమోదు_IS_TRANSITION(నమోదు_2)) {
                        state=(uint8_t)MBCS_నమోదు_TRANSITION_STATE(నమోదు_2);
                        స్థానభ్రంశం=MBCS_నమోదు_TRANSITION_స్థానభ్రంశం(నమోదు_2);

                        ++ఆధారం;
                        ఒకవేళ( ఆధారం<ఆధారం_హద్దు &&
                            MBCS_నమోదు_IS_FINAL(నమోదు_2=state_పట్టిక[state][*ఆధారం]) &&
                            MBCS_నమోదు_FINAL_ACTION(నమోదు_2)==MBCS_STATE_VALID_16 &&
                            (c=unicode_సంకేతం_Units[స్థానభ్రంశం+MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2)])<0xfffe
                        ) {
                            ++ఆధారం;
                            *లక్ష్యం++=c;
                            state=(uint8_t)MBCS_నమోదు_FINAL_STATE(నమోదు_2); /* typically 0 */
                            స్థానభ్రంశం=0;
                        } లేకపోతే {
                            /* set the state and leave the optimized loop */
                            బైట్లు[0]=*(ఆధారం-1);
                            byte_సూచీ=1;
                            ఆపు;
                        }
                    } లేకపోతే {
                        ఒకవేళ(MBCS_నమోదు_FINAL_IS_VALID_ప్రత్యక్ష_16(నమోదు_2)) {
                            /* output BMP code point */
                            ++ఆధారం;
                            *లక్ష్యం++=(char16_t)MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
                            state=(uint8_t)MBCS_నమోదు_FINAL_STATE(నమోదు_2); /* typically 0 */
                        } లేకపోతే {
                            /* leave the optimized loop */
                            ఆపు;
                        }
                    }
                } అయితే(ఆధారం<ఆధారం_హద్దు && లక్ష్యం<లక్ష్యం_హద్దు);
            } లేకపోతే /* offsets!=nullptr */ {
                చేయి {
                    నమోదు_2=state_పట్టిక[state][*ఆధారం];
                    ఒకవేళ(MBCS_నమోదు_IS_TRANSITION(నమోదు_2)) {
                        state=(uint8_t)MBCS_నమోదు_TRANSITION_STATE(నమోదు_2);
                        స్థానభ్రంశం=MBCS_నమోదు_TRANSITION_స్థానభ్రంశం(నమోదు_2);

                        ++ఆధారం;
                        ఒకవేళ( ఆధారం<ఆధారం_హద్దు &&
                            MBCS_నమోదు_IS_FINAL(నమోదు_2=state_పట్టిక[state][*ఆధారం]) &&
                            MBCS_నమోదు_FINAL_ACTION(నమోదు_2)==MBCS_STATE_VALID_16 &&
                            (c=unicode_సంకేతం_Units[స్థానభ్రంశం+MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2)])<0xfffe
                        ) {
                            ++ఆధారం;
                            *లక్ష్యం++=c;
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ;
                                ఆధారం_సూచీ=(తదుపరి_ఆధారం_సూచీ+=2);
                            }
                            state=(uint8_t)MBCS_నమోదు_FINAL_STATE(నమోదు_2); /* typically 0 */
                            స్థానభ్రంశం=0;
                        } లేకపోతే {
                            /* set the state and leave the optimized loop */
                            ++తదుపరి_ఆధారం_సూచీ;
                            బైట్లు[0]=*(ఆధారం-1);
                            byte_సూచీ=1;
                            ఆపు;
                        }
                    } లేకపోతే {
                        ఒకవేళ(MBCS_నమోదు_FINAL_IS_VALID_ప్రత్యక్ష_16(నమోదు_2)) {
                            /* output BMP code point */
                            ++ఆధారం;
                            *లక్ష్యం++=(char16_t)MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ;
                                ఆధారం_సూచీ=++తదుపరి_ఆధారం_సూచీ;
                            }
                            state=(uint8_t)MBCS_నమోదు_FINAL_STATE(నమోదు_2); /* typically 0 */
                        } లేకపోతే {
                            /* leave the optimized loop */
                            ఆపు;
                        }
                    }
                } అయితే(ఆధారం<ఆధారం_హద్దు && లక్ష్యం<లక్ష్యం_హద్దు);
            }

            /*
             * these tests and break statements could be put inside the loop
             * if C had "break outerLoop" like Java
             */
            ఒకవేళ(ఆధారం>=ఆధారం_హద్దు) {
                ఆపు;
            }
            ఒకవేళ(లక్ష్యం>=లక్ష్యం_హద్దు) {
                /* target is full */
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }

            ++తదుపరి_ఆధారం_సూచీ;
            బైట్లు[byte_సూచీ++]=*ఆధారం++;
        } లేకపోతే /* byteIndex>0 */ {
            ++తదుపరి_ఆధారం_సూచీ;
            నమోదు_2=state_పట్టిక[state][బైట్లు[byte_సూచీ++]=*ఆధారం++];
        }

        ఒకవేళ(MBCS_నమోదు_IS_TRANSITION(నమోదు_2)) {
            state=(uint8_t)MBCS_నమోదు_TRANSITION_STATE(నమోదు_2);
            స్థానభ్రంశం+=MBCS_నమోదు_TRANSITION_స్థానభ్రంశం(నమోదు_2);
            కొనసాగు;
        }

        /* save the previous state for proper extension mapping with SI/SO-stateful converters */
        cnv->బహుళకం=state;

        /* set the next state early so that we can reuse the entry variable */
        state=(uint8_t)MBCS_నమోదు_FINAL_STATE(నమోదు_2); /* typically 0 */

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action=(uint8_t)(MBCS_నమోదు_FINAL_ACTION(నమోదు_2));
        ఒకవేళ(action==MBCS_STATE_VALID_16) {
            స్థానభ్రంశం+=MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
            c=unicode_సంకేతం_Units[స్థానభ్రంశం];
            ఒకవేళ(c<0xfffe) {
                /* output BMP code point */
                *లక్ష్యం++=c;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                byte_సూచీ=0;
            } లేకపోతే ఒకవేళ(c==0xfffe) {
                ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv) && (నమోదు_2=(int32_t)ucnv_MBCSGetFallback(&cnv->shared_దత్తాంశం->mbcs, స్థానభ్రంశం))!=0xfffe) {
                    /* output fallback BMP code point */
                    *లక్ష్యం++=(char16_t)నమోదు_2;
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                    byte_సూచీ=0;
                }
            } లేకపోతే {
                /* callback(illegal) */
                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
            }
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_VALID_ప్రత్యక్ష_16) {
            /* output BMP code point */
            *లక్ష్యం++=(char16_t)MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
            ఒకవేళ(offsets!=శూన్యసూచిక) {
                *offsets++=ఆధారం_సూచీ;
            }
            byte_సూచీ=0;
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_VALID_16_PAIR) {
            స్థానభ్రంశం+=MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
            c=unicode_సంకేతం_Units[స్థానభ్రంశం++];
            ఒకవేళ(c<0xd800) {
                /* output BMP code point below 0xd800 */
                *లక్ష్యం++=c;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                byte_సూచీ=0;
            } లేకపోతే ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                /* output roundtrip or fallback surrogate pair */
                *లక్ష్యం++=(char16_t)(c&0xdbff);
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                byte_సూచీ=0;
                ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                    *లక్ష్యం++=unicode_సంకేతం_Units[స్థానభ్రంశం];
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                } లేకపోతే {
                    /* target overflow */
                    cnv->UChar_Error_బఫరు[0]=unicode_సంకేతం_Units[స్థానభ్రంశం];
                    cnv->UChar_Error_బఫరు_నిడివి=1;
                    *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;

                    స్థానభ్రంశం=0;
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                *లక్ష్యం++=unicode_సంకేతం_Units[స్థానభ్రంశం];
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                byte_సూచీ=0;
            } లేకపోతే ఒకవేళ(c==0xffff) {
                /* callback(illegal) */
                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
            }
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_VALID_ప్రత్యక్ష_20 ||
                  (action==MBCS_STATE_FALLBACK_ప్రత్యక్ష_20 && UCNV_గమ్యం_U_USE_FALLBACK(cnv))
        ) {
            నమోదు_2=MBCS_నమోదు_FINAL_విలువ(నమోదు_2);
            /* output surrogate pair */
            *లక్ష్యం++=(char16_t)(0xd800|(char16_t)(నమోదు_2>>10));
            ఒకవేళ(offsets!=శూన్యసూచిక) {
                *offsets++=ఆధారం_సూచీ;
            }
            byte_సూచీ=0;
            c=(char16_t)(0xdc00|(char16_t)(నమోదు_2&0x3ff));
            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                *లక్ష్యం++=c;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
            } లేకపోతే {
                /* target overflow */
                cnv->UChar_Error_బఫరు[0]=c;
                cnv->UChar_Error_బఫరు_నిడివి=1;
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;

                స్థానభ్రంశం=0;
                ఆపు;
            }
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_CHANGE_ONLY) {
            /*
             * This serves as a state change without any output.
             * It is useful for reading simple stateful encodings,
             * for example using just Shift-In/Shift-Out codes.
             * The 21 unused bits may later be used for more sophisticated
             * state transitions.
             */
            ఒకవేళ(cnv->shared_దత్తాంశం->mbcs.dbcsOnlyState==0) {
                byte_సూచీ=0;
            } లేకపోతే {
                /* SI/SO are illegal for DBCS-only conversion */
                state=(uint8_t)(cnv->బహుళకం); /* restore the previous state */

                /* callback(illegal) */
                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
            }
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_FALLBACK_ప్రత్యక్ష_16) {
            ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *లక్ష్యం++=(char16_t)MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                byte_సూచీ=0;
            }
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
        } లేకపోతే {
            /* reserved, must never occur */
            byte_సూచీ=0;
        }

        /* end of action codes: prepare for a new character */
        స్థానభ్రంశం=0;

        ఒకవేళ(byte_సూచీ==0) {
            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
        } లేకపోతే ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            /* callback(illegal) */
            ఒకవేళ(byte_సూచీ>1) {
                /*
                 * Ticket 5691: consistent illegal sequences:
                 * - We include at least the first byte in the illegal sequence.
                 * - If any of the non-initial bytes could be the start of a character,
                 *   we stop the illegal sequence before the first one of those.
                 */
                UBool isDBCSOnly = cnv->shared_దత్తాంశం->mbcs.dbcsOnlyState != 0;
                int8_t i;
                కొరకు(i=1;
                    i<byte_సూచీ && !isSingleOrLead(state_పట్టిక, state, isDBCSOnly, బైట్లు[i]);
                    ++i) {}
                ఒకవేళ(i<byte_సూచీ) {
                    /* Back out some bytes. */
                    int8_t వెనుక_బహిర్గతం_దూరం=byte_సూచీ-i;
                    int32_t బైట్లు_From_This_బఫరు=(int32_t)(ఆధారం-(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం);
                    byte_సూచీ=i;  /* length of reported illegal byte sequence */
                    ఒకవేళ(వెనుక_బహిర్గతం_దూరం<=బైట్లు_From_This_బఫరు) {
                        ఆధారం-=వెనుక_బహిర్గతం_దూరం;
                    } లేకపోతే {
                        /* Back out bytes from the previous buffer: Need to replay them. */
                        cnv->pre_గమ్యం_ULength=(int8_t)(బైట్లు_From_This_బఫరు-వెనుక_బహిర్గతం_దూరం);
                        /* preToULength is negative! */
                        uprv_memcpy(cnv->pre_గమ్యం_U, బైట్లు+i, -cnv->pre_గమ్యం_ULength);
                        ఆధారం=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
                    }
                }
            }
            ఆపు;
        } లేకపోతే /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
            byte_సూచీ=ext_గమ్యం_U(cnv, cnv->shared_దత్తాంశం,
                              byte_సూచీ, &ఆధారం, ఆధారం_హద్దు,
                              &లక్ష్యం, లక్ష్యం_హద్దు,
                              &offsets, ఆధారం_సూచీ,
                              p_వాదనలు->flush,
                              p_Error_సంకేతం);
            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ+=(int32_t)(ఆధారం-(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం);

            ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                /* not mappable or buffer overflow */
                ఆపు;
            }
        }
    }

    /* set the converter state back into UConverter */
    cnv->గమ్యం_Unicode_స్థితి=స్థానభ్రంశం;
    cnv->బహుళకం=state;
    cnv->గమ్యం_ULength=byte_సూచీ;

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
    p_వాదనలు->లక్ష్యం=లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

/*
 * This version of ucnv_MBCSGetNextUChar() is optimized for single-byte, single-state codepages.
 * We still need a conversion loop in case we find reserved action codes, which are to be ignored.
 */
స్థిర UChar32
ucnv_MBCSSingle_Get_తదుపరి_UChar(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                        UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి int32_t (*state_పట్టిక)[256];
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;

    int32_t నమోదు_2;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    ఆధారం = reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->ఆధారం);
    ఆధారం_హద్దు = reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->ఆధారం_హద్దు);
    ఒకవేళ((cnv->options&UCNV_OPTION_మార్పిడి_LFNL)!=0) {
        state_పట్టిక=(స్థిరరాశి int32_t (*)[256])cnv->shared_దత్తాంశం->mbcs.మార్పిడి_LFNLState_పట్టిక;
    } లేకపోతే {
        state_పట్టిక=cnv->shared_దత్తాంశం->mbcs.state_పట్టిక;
    }

    /* conversion loop */
    అయితే(ఆధారం<ఆధారం_హద్దు) {
        నమోదు_2=state_పట్టిక[0][*ఆధారం++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* write back the updated pointer early so that we can return directly */
        p_వాదనలు->ఆధారం = reinterpret_cast<స్థిరరాశి అక్షరం*>(ఆధారం);

        ఒకవేళ(MBCS_నమోదు_FINAL_IS_VALID_ప్రత్యక్ష_16(నమోదు_2)) {
            /* output BMP code point */
            తిరిగి static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_నమోదు_FINAL_ACTION(నమోదు_2));
        ఒకవేళ( action==MBCS_STATE_VALID_ప్రత్యక్ష_20 ||
            (action==MBCS_STATE_FALLBACK_ప్రత్యక్ష_20 && UCNV_గమ్యం_U_USE_FALLBACK(cnv))
        ) {
            /* output supplementary code point */
            తిరిగి static_cast<UChar32>(MBCS_నమోదు_FINAL_విలువ(నమోదు_2) + 0x10000);
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_FALLBACK_ప్రత్యక్ష_16) {
            ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                తిరిగి static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
            }
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } లేకపోతే ఒకవేళ(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
        } లేకపోతే {
            /* reserved, must never occur */
            కొనసాగు;
        }

        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            /* callback(illegal) */
            ఆపు;
        } లేకపోతే /* unassigned sequence */ {
            /* defer to the generic implementation */
            p_వాదనలు->ఆధారం = reinterpret_cast<స్థిరరాశి అక్షరం*>(ఆధారం) - 1;
            తిరిగి UCNV_GET_తదుపరి_UCHAR_USE_గమ్యం_U;
        }
    }

    /* no output because of empty input or only state changes */
    *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
    తిరిగి 0xffff;
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
స్థిర UChar32 U_CALLCONV
ucnv_MBCSGet_తదుపరి_UChar(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                  UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు, *చివరి_ఆధారం;

    స్థిరరాశి int32_t (*state_పట్టిక)[256];
    స్థిరరాశి uint16_t *unicode_సంకేతం_Units;

    uint32_t స్థానభ్రంశం;
    uint8_t state;

    int32_t నమోదు_2;
    UChar32 c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_వాదనలు->converter;

    ఒకవేళ(cnv->pre_గమ్యం_ULength>0) {
        /* use the generic code in ucnv_getNextUChar() to continue with a partial match */
        తిరిగి UCNV_GET_తదుపరి_UCHAR_USE_గమ్యం_U;
    }

    ఒకవేళ(cnv->shared_దత్తాంశం->mbcs.unicode_ముసుగు&UCNV_HAS_SURROGATES) {
        /*
         * Using the generic ucnv_getNextUChar() code lets us deal correctly
         * with the rare case of a codepage that maps single surrogates
         * without adding the complexity to this already complicated function here.
         */
        తిరిగి UCNV_GET_తదుపరి_UCHAR_USE_గమ్యం_U;
    } లేకపోతే ఒకవేళ(cnv->shared_దత్తాంశం->mbcs.లెక్క_స్థితులు==1) {
        తిరిగి ucnv_MBCSSingle_Get_తదుపరి_UChar(p_వాదనలు, p_Error_సంకేతం);
    }

    /* set up the local pointers */
    ఆధారం = చివరి_ఆధారం = reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->ఆధారం);
    ఆధారం_హద్దు = reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->ఆధారం_హద్దు);

    ఒకవేళ((cnv->options&UCNV_OPTION_మార్పిడి_LFNL)!=0) {
        state_పట్టిక=(స్థిరరాశి int32_t (*)[256])cnv->shared_దత్తాంశం->mbcs.మార్పిడి_LFNLState_పట్టిక;
    } లేకపోతే {
        state_పట్టిక=cnv->shared_దత్తాంశం->mbcs.state_పట్టిక;
    }
    unicode_సంకేతం_Units=cnv->shared_దత్తాంశం->mbcs.unicode_సంకేతం_Units;

    /* get the converter state from UConverter */
    స్థానభ్రంశం=cnv->గమ్యం_Unicode_స్థితి;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    ఒకవేళ ((state = static_cast<uint8_t>(cnv->బహుళకం)) == 0) {
        state=cnv->shared_దత్తాంశం->mbcs.dbcsOnlyState;
    }

    /* conversion loop */
    c=U_SENTINEL;
    అయితే(ఆధారం<ఆధారం_హద్దు) {
        నమోదు_2=state_పట్టిక[state][*ఆధారం++];
        ఒకవేళ(MBCS_నమోదు_IS_TRANSITION(నమోదు_2)) {
            state = static_cast<uint8_t>(MBCS_నమోదు_TRANSITION_STATE(నమోదు_2));
            స్థానభ్రంశం+=MBCS_నమోదు_TRANSITION_స్థానభ్రంశం(నమోదు_2);

            /* optimization for 1/2-byte input and BMP output */
            ఒకవేళ( ఆధారం<ఆధారం_హద్దు &&
                MBCS_నమోదు_IS_FINAL(నమోదు_2=state_పట్టిక[state][*ఆధారం]) &&
                MBCS_నమోదు_FINAL_ACTION(నమోదు_2)==MBCS_STATE_VALID_16 &&
                (c=unicode_సంకేతం_Units[స్థానభ్రంశం+MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2)])<0xfffe
            ) {
                ++ఆధారం;
                state = static_cast<uint8_t>(MBCS_నమోదు_FINAL_STATE(నమోదు_2)); /* typically 0 */
                /* output BMP code point */
                ఆపు;
            }
        } లేకపోతే {
            /* save the previous state for proper extension mapping with SI/SO-stateful converters */
            cnv->బహుళకం=state;

            /* set the next state early so that we can reuse the entry variable */
            state = static_cast<uint8_t>(MBCS_నమోదు_FINAL_STATE(నమోదు_2)); /* typically 0 */

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action = static_cast<uint8_t>(MBCS_నమోదు_FINAL_ACTION(నమోదు_2));
            ఒకవేళ(action==MBCS_STATE_VALID_ప్రత్యక్ష_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
                ఆపు;
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_VALID_16) {
                స్థానభ్రంశం+=MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
                c=unicode_సంకేతం_Units[స్థానభ్రంశం];
                ఒకవేళ(c<0xfffe) {
                    /* output BMP code point */
                    ఆపు;
                } లేకపోతే ఒకవేళ(c==0xfffe) {
                    ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv) && (c=ucnv_MBCSGetFallback(&cnv->shared_దత్తాంశం->mbcs, స్థానభ్రంశం))!=0xfffe) {
                        ఆపు;
                    }
                } లేకపోతే {
                    /* callback(illegal) */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                }
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_VALID_16_PAIR) {
                స్థానభ్రంశం+=MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
                c=unicode_సంకేతం_Units[స్థానభ్రంశం++];
                ఒకవేళ(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                    ఆపు;
                } లేకపోతే ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_సంకేతం_Units[స్థానభ్రంశం]+(0x10000-0xdc00);
                    ఆపు;
                } లేకపోతే ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_సంకేతం_Units[స్థానభ్రంశం];
                    ఆపు;
                } లేకపోతే ఒకవేళ(c==0xffff) {
                    /* callback(illegal) */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                }
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_VALID_ప్రత్యక్ష_20 ||
                      (action==MBCS_STATE_FALLBACK_ప్రత్యక్ష_20 && UCNV_గమ్యం_U_USE_FALLBACK(cnv))
            ) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_నమోదు_FINAL_విలువ(నమోదు_2) + 0x10000);
                ఆపు;
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_CHANGE_ONLY) {
                /*
                 * This serves as a state change without any output.
                 * It is useful for reading simple stateful encodings,
                 * for example using just Shift-In/Shift-Out codes.
                 * The 21 unused bits may later be used for more sophisticated
                 * state transitions.
                 */
                ఒకవేళ(cnv->shared_దత్తాంశం->mbcs.dbcsOnlyState!=0) {
                    /* SI/SO are illegal for DBCS-only conversion */
                    state = static_cast<uint8_t>(cnv->బహుళకం); /* restore the previous state */

                    /* callback(illegal) */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                }
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_FALLBACK_ప్రత్యక్ష_16) {
                ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv)) {
                    /* output BMP code point */
                    c = static_cast<char16_t>(MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2));
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_UNASSIGNED) {
                /* just fall through */
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_ILLEGAL) {
                /* callback(illegal) */
                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
            } లేకపోతే {
                /* reserved (must never occur), or only state change */
                స్థానభ్రంశం=0;
                చివరి_ఆధారం=ఆధారం;
                కొనసాగు;
            }

            /* end of action codes: prepare for a new character */
            స్థానభ్రంశం=0;

            ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                /* callback(illegal) */
                ఆపు;
            } లేకపోతే /* unassigned sequence */ {
                /* defer to the generic implementation */
                cnv->గమ్యం_Unicode_స్థితి=0;
                cnv->బహుళకం=state;
                p_వాదనలు->ఆధారం = reinterpret_cast<స్థిరరాశి అక్షరం*>(చివరి_ఆధారం);
                తిరిగి UCNV_GET_తదుపరి_UCHAR_USE_గమ్యం_U;
            }
        }
    }

    ఒకవేళ(c<0) {
        ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం) && ఆధారం==ఆధారం_హద్దు && చివరి_ఆధారం<ఆధారం) {
            /* incomplete character byte sequence */
            cnv->గమ్యం_ULength = static_cast<int8_t>(ఆధారం - చివరి_ఆధారం);
            uprv_memcpy(cnv->గమ్యం_UBytes, చివరి_ఆధారం, cnv->గమ్యం_ULength);
            *p_Error_సంకేతం=U_TRUNCATED_CHAR_దొరికింది;
        } లేకపోతే ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            /* callback(illegal) */
            /*
             * Ticket 5691: consistent illegal sequences:
             * - We include at least the first byte in the illegal sequence.
             * - If any of the non-initial bytes could be the start of a character,
             *   we stop the illegal sequence before the first one of those.
             */
            UBool isDBCSOnly = static_cast<UBool>(cnv->shared_దత్తాంశం->mbcs.dbcsOnlyState != 0);
            uint8_t *బైట్లు=cnv->గమ్యం_UBytes;
            *బైట్లు++=*చివరి_ఆధారం++;     /* first byte */
            ఒకవేళ(చివరి_ఆధారం==ఆధారం) {
                cnv->గమ్యం_ULength=1;
            } లేకపోతే /* lastSource<source: multi-byte character */ {
                int8_t i;
                కొరకు(i=1;
                    చివరి_ఆధారం<ఆధారం && !isSingleOrLead(state_పట్టిక, state, isDBCSOnly, *చివరి_ఆధారం);
                    ++i
                ) {
                    *బైట్లు++=*చివరి_ఆధారం++;
                }
                cnv->గమ్యం_ULength=i;
                ఆధారం=చివరి_ఆధారం;
            }
        } లేకపోతే {
            /* no output because of empty input or only state changes */
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        }
        c=0xffff;
    }

    /* set the converter state back into UConverter, ready for a new character */
    cnv->గమ్యం_Unicode_స్థితి=0;
    cnv->బహుళకం=state;

    /* write back the updated pointer */
    p_వాదనలు->ఆధారం = reinterpret_cast<స్థిరరాశి అక్షరం*>(ఆధారం);
    తిరిగి c;
}

#ఒకవేళ 0
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
ucnv_MBCSSingle_Simple_Get_తదుపరి_UChar(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం,
                              uint8_t b, UBool useFallback) {
    int32_t నమోదు_2;
    uint8_t action;

    నమోదు_2=shared_దత్తాంశం->mbcs.state_పట్టిక[0][b];
    /* MBCS_ENTRY_IS_FINAL(entry) */

    ఒకవేళ(MBCS_నమోదు_FINAL_IS_VALID_ప్రత్యక్ష_16(నమోదు_2)) {
        /* output BMP code point */
        తిరిగి (char16_t)MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
    }

    /*
     * An if-else-if chain provides more reliable performance for
     * the most common cases compared to a switch.
     */
    action=(uint8_t)(MBCS_నమోదు_FINAL_ACTION(నమోదు_2));
    ఒకవేళ(action==MBCS_STATE_VALID_ప్రత్యక్ష_20) {
        /* output supplementary code point */
        తిరిగి 0x10000+MBCS_నమోదు_FINAL_విలువ(నమోదు_2);
    } లేకపోతే ఒకవేళ(action==MBCS_STATE_FALLBACK_ప్రత్యక్ష_16) {
        ఒకవేళ(!గమ్యం_U_USE_FALLBACK(useFallback)) {
            తిరిగి 0xfffe;
        }
        /* output BMP code point */
        తిరిగి (char16_t)MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
    } లేకపోతే ఒకవేళ(action==MBCS_STATE_FALLBACK_ప్రత్యక్ష_20) {
        ఒకవేళ(!గమ్యం_U_USE_FALLBACK(useFallback)) {
            తిరిగి 0xfffe;
        }
        /* output supplementary code point */
        తిరిగి 0x10000+MBCS_నమోదు_FINAL_విలువ(నమోదు_2);
    } లేకపోతే ఒకవేళ(action==MBCS_STATE_UNASSIGNED) {
        తిరిగి 0xfffe;
    } లేకపోతే ఒకవేళ(action==MBCS_STATE_ILLEGAL) {
        తిరిగి 0xffff;
    } లేకపోతే {
        /* reserved, must never occur */
        తిరిగి 0xffff;
    }
}
#అయితే_ముగింపు

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
ucnv_MBCSSimple_Get_తదుపరి_UChar(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం,
                        స్థిరరాశి అక్షరం *ఆధారం, int32_t length,
                        UBool useFallback) {
    స్థిరరాశి int32_t (*state_పట్టిక)[256];
    స్థిరరాశి uint16_t *unicode_సంకేతం_Units;

    uint32_t స్థానభ్రంశం;
    uint8_t state, action;

    UChar32 c;
    int32_t i, నమోదు_2;

    ఒకవేళ(length<=0) {
        /* no input at all: "illegal" */
        తిరిగి 0xffff;
    }

#ఒకవేళ 0
/*
 * Code disabled 2002dec09 (ICU 2.4) because it is not currently used in ICU. markus
 * TODO In future releases, verify that this function is never called for SBCS
 * conversions, i.e., that sharedData->mbcs.countStates==1 is still true.
 * Removal improves code coverage.
 */
    /* use optimized function if possible */
    ఒకవేళ(shared_దత్తాంశం->mbcs.లెక్క_స్థితులు==1) {
        ఒకవేళ(length==1) {
            తిరిగి ucnv_MBCSSingle_Simple_Get_తదుపరి_UChar(shared_దత్తాంశం, (uint8_t)*ఆధారం, useFallback);
        } లేకపోతే {
            తిరిగి 0xffff; /* illegal: more than a single byte for an SBCS converter */
        }
    }
#అయితే_ముగింపు

    /* set up the local pointers */
    state_పట్టిక=shared_దత్తాంశం->mbcs.state_పట్టిక;
    unicode_సంకేతం_Units=shared_దత్తాంశం->mbcs.unicode_సంకేతం_Units;

    /* converter state */
    స్థానభ్రంశం=0;
    state=shared_దత్తాంశం->mbcs.dbcsOnlyState;

    /* conversion loop */
    కొరకు(i=0;;) {
        నమోదు_2=state_పట్టిక[state][(uint8_t)ఆధారం[i++]];
        ఒకవేళ(MBCS_నమోదు_IS_TRANSITION(నమోదు_2)) {
            state=(uint8_t)MBCS_నమోదు_TRANSITION_STATE(నమోదు_2);
            స్థానభ్రంశం+=MBCS_నమోదు_TRANSITION_స్థానభ్రంశం(నమోదు_2);

            ఒకవేళ(i==length) {
                తిరిగి 0xffff; /* truncated character */
            }
        } లేకపోతే {
            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=(uint8_t)(MBCS_నమోదు_FINAL_ACTION(నమోదు_2));
            ఒకవేళ(action==MBCS_STATE_VALID_16) {
                స్థానభ్రంశం+=MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
                c=unicode_సంకేతం_Units[స్థానభ్రంశం];
                ఒకవేళ(c!=0xfffe) {
                    /*పూర్తి*/
                } లేకపోతే ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv)) {
                    c=ucnv_MBCSGetFallback(&shared_దత్తాంశం->mbcs, స్థానభ్రంశం);
                /* else done with 0xfffe */
                }
                ఆపు;
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_VALID_ప్రత్యక్ష_16) {
                /* output BMP code point */
                c=(char16_t)MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
                ఆపు;
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_VALID_16_PAIR) {
                స్థానభ్రంశం+=MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
                c=unicode_సంకేతం_Units[స్థానభ్రంశం++];
                ఒకవేళ(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } లేకపోతే ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=(UChar32)(((c&0x3ff)<<10)+unicode_సంకేతం_Units[స్థానభ్రంశం]+(0x10000-0xdc00));
                } లేకపోతే ఒకవేళ(UCNV_గమ్యం_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_సంకేతం_Units[స్థానభ్రంశం];
                } లేకపోతే ఒకవేళ(c==0xffff) {
                    తిరిగి 0xffff;
                } లేకపోతే {
                    c=0xfffe;
                }
                ఆపు;
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_VALID_ప్రత్యక్ష_20) {
                /* output supplementary code point */
                c=0x10000+MBCS_నమోదు_FINAL_విలువ(నమోదు_2);
                ఆపు;
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_FALLBACK_ప్రత్యక్ష_16) {
                ఒకవేళ(!గమ్యం_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    ఆపు;
                }
                /* output BMP code point */
                c=(char16_t)MBCS_నమోదు_FINAL_విలువ_16(నమోదు_2);
                ఆపు;
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_FALLBACK_ప్రత్యక్ష_20) {
                ఒకవేళ(!గమ్యం_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    ఆపు;
                }
                /* output supplementary code point */
                c=0x10000+MBCS_నమోదు_FINAL_విలువ(నమోదు_2);
                ఆపు;
            } లేకపోతే ఒకవేళ(action==MBCS_STATE_UNASSIGNED) {
                c=0xfffe;
                ఆపు;
            }

            /*
             * forbid MBCS_STATE_CHANGE_ONLY for this function,
             * and MBCS_STATE_ILLEGAL and reserved action codes
             */
            తిరిగి 0xffff;
        }
    }

    ఒకవేళ(i!=length) {
        /* illegal for this function: not all input consumed */
        తిరిగి 0xffff;
    }

    ఒకవేళ(c==0xfffe) {
        /* try an extension mapping */
        స్థిరరాశి int32_t *cx=shared_దత్తాంశం->mbcs.extIndexes;
        ఒకవేళ(cx!=శూన్యసూచిక) {
            తిరిగి ucnv_ext_Simple_సరిపోలిక_గమ్యం_U(cx, ఆధారం, length, useFallback);
        }
    }

    తిరిగి c;
}

/* MBCS-from-Unicode conversion functions ----------------------------------- */

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for double-byte codepages. */
స్థిర శూన్య
ucnv_MBCSDoubleFromUnicodeWithOffsets(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
                                  UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి char16_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం;
    int32_t *offsets;

    స్థిరరాశి uint16_t *పట్టిక;
    స్థిరరాశి uint16_t *mbcs_సూచీ;
    స్థిరరాశి uint8_t *బైట్లు;

    UChar32 c;

    int32_t ఆధారం_సూచీ, తదుపరి_ఆధారం_సూచీ;

    uint32_t stage2_నమోదు;
    uint32_t asciiRoundtrips;
    uint32_t value;
    uint8_t unicode_ముసుగు;

    /* use optimized function if possible */
    cnv=p_వాదనలు->converter;
    unicode_ముసుగు=cnv->shared_దత్తాంశం->mbcs.unicode_ముసుగు;

    /* set up the local pointers */
    ఆధారం=p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం = reinterpret_cast<uint8_t*>(p_వాదనలు->లక్ష్యం);
    లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(p_వాదనలు->లక్ష్యం_హద్దు - p_వాదనలు->లక్ష్యం);
    offsets=p_వాదనలు->offsets;

    పట్టిక=cnv->shared_దత్తాంశం->mbcs.from_Unicode_పట్టిక;
    mbcs_సూచీ=cnv->shared_దత్తాంశం->mbcs.mbcs_సూచీ;
    ఒకవేళ((cnv->options&UCNV_OPTION_మార్పిడి_LFNL)!=0) {
        బైట్లు=cnv->shared_దత్తాంశం->mbcs.మార్పిడి_LFNLFrom_Unicode_బైట్లు;
    } లేకపోతే {
        బైట్లు=cnv->shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు;
    }
    asciiRoundtrips=cnv->shared_దత్తాంశం->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ= c==0 ? 0 : -1;
    తదుపరి_ఆధారం_సూచీ=0;

    /* conversion loop */
    ఒకవేళ(c!=0 && లక్ష్యం_సామర్థ్యం>0) {
        వెళ్ళు getTrail;
    }

    అయితే(ఆధారం<ఆధారం_హద్దు) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ఒకవేళ(లక్ష్యం_సామర్థ్యం>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ఆధారం++;
            ++తదుపరి_ఆధారం_సూచీ;
            ఒకవేళ(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *లక్ష్యం++ = static_cast<uint8_t>(c);
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                    ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                }
                --లక్ష్యం_సామర్థ్యం;
                c=0;
                కొనసాగు;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            ఒకవేళ(c<=0xd7ff) {
                value=DBCS_ఫలితం_FROM_MOST_BMP(mbcs_సూచీ, (స్థిరరాశి uint16_t *)బైట్లు, c);
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                ఒకవేళ(value==0) {
                    వెళ్ళు unassigned;
                }
                /* output the value */
            } లేకపోతే {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                ఒకవేళ(U16_IS_SURROGATE(c) && !(unicode_ముసుగు&UCNV_HAS_SURROGATES)) {
                    ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        ఒకవేళ(ఆధారం<ఆధారం_హద్దు) {
                            /* test the following code unit */
                            char16_t trail=*ఆధారం;
                            ఒకవేళ(U16_IS_TRAIL(trail)) {
                                ++ఆధారం;
                                ++తదుపరి_ఆధారం_సూచీ;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                ఒకవేళ(!(unicode_ముసుగు&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    /* callback(unassigned) */
                                    వెళ్ళు unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } లేకపోతే {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                                ఆపు;
                            }
                        } లేకపోతే {
                            /* no more input */
                            ఆపు;
                        }
                    } లేకపోతే {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                        ఆపు;
                    }
                }

                /* convert the Unicode code point in c into codepage bytes */
                stage2_నమోదు=MBCS_STAGE_2_FROM_U(పట్టిక, c);

                /* get the bytes and the length for the output */
                /* MBCS_OUTPUT_2 */
                value=MBCS_విలువ_2_FROM_STAGE_2(బైట్లు, stage2_నమోదు, c);

                /* is this code point assigned, or do we use fallbacks? */
                ఒకవేళ(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_నమోదు, c) ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_వాదనలు->ఆధారం=ఆధారం;
                    c=_extFromU(cnv, cnv->shared_దత్తాంశం,
                                c, &ఆధారం, ఆధారం_హద్దు,
                                &లక్ష్యం, లక్ష్యం+లక్ష్యం_సామర్థ్యం,
                                &offsets, ఆధారం_సూచీ,
                                p_వాదనలు->flush,
                                p_Error_సంకేతం);
                    తదుపరి_ఆధారం_సూచీ += static_cast<int32_t>(ఆధారం - p_వాదనలు->ఆధారం);

                    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                        /* not mappable or buffer overflow */
                        ఆపు;
                    } లేకపోతే {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(p_వాదనలు->లక్ష్యం_హద్దు - reinterpret_cast<అక్షరం*>(లక్ష్యం));

                        /* normal end of conversion: prepare for a new character */
                        ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                        కొనసాగు;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            ఒకవేళ(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *లక్ష్యం++ = static_cast<uint8_t>(value);
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                --లక్ష్యం_సామర్థ్యం;
            } లేకపోతే /* length==2 */ {
                *లక్ష్యం++ = static_cast<uint8_t>(value >> 8);
                ఒకవేళ(2<=లక్ష్యం_సామర్థ్యం) {
                    *లక్ష్యం++ = static_cast<uint8_t>(value);
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                        *offsets++=ఆధారం_సూచీ;
                    }
                    లక్ష్యం_సామర్థ్యం-=2;
                } లేకపోతే {
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                    cnv->char_Error_బఫరు[0] = static_cast<అక్షరం>(value);
                    cnv->char_Error_బఫరు_నిడివి=1;

                    /* target overflow */
                    లక్ష్యం_సామర్థ్యం=0;
                    *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                    c=0;
                    ఆపు;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
            కొనసాగు;
        } లేకపోతే {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=ఆధారం;
    p_వాదనలు->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
    p_వాదనలు->offsets=offsets;
}

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for single-byte codepages. */
స్థిర శూన్య
ucnv_MBCSSingleFromUnicodeWithOffsets(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
                                  UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి char16_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం;
    int32_t *offsets;

    స్థిరరాశి uint16_t *పట్టిక;
    స్థిరరాశి uint16_t *results;

    UChar32 c;

    int32_t ఆధారం_సూచీ, తదుపరి_ఆధారం_సూచీ;

    uint16_t value, min_విలువ;
    UBool hasSupplementary;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    ఆధారం=p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం = reinterpret_cast<uint8_t*>(p_వాదనలు->లక్ష్యం);
    లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(p_వాదనలు->లక్ష్యం_హద్దు - p_వాదనలు->లక్ష్యం);
    offsets=p_వాదనలు->offsets;

    పట్టిక=cnv->shared_దత్తాంశం->mbcs.from_Unicode_పట్టిక;
    ఒకవేళ((cnv->options&UCNV_OPTION_మార్పిడి_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_దత్తాంశం->mbcs.మార్పిడి_LFNLFrom_Unicode_బైట్లు);
    } లేకపోతే {
        results=(uint16_t *)cnv->shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు;
    }

    ఒకవేళ(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_విలువ=0x800;
    } లేకపోతే {
        /* use only roundtrips and fallbacks from private-use characters */
        min_విలువ=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_దత్తాంశం->mbcs.unicode_ముసుగు & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ= c==0 ? 0 : -1;
    తదుపరి_ఆధారం_సూచీ=0;

    /* conversion loop */
    ఒకవేళ(c!=0 && లక్ష్యం_సామర్థ్యం>0) {
        వెళ్ళు getTrail;
    }

    అయితే(ఆధారం<ఆధారం_హద్దు) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ఒకవేళ(లక్ష్యం_సామర్థ్యం>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ఆధారం++;
            ++తదుపరి_ఆధారం_సూచీ;
            ఒకవేళ(U16_IS_SURROGATE(c)) {
                ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                    ఒకవేళ(ఆధారం<ఆధారం_హద్దు) {
                        /* test the following code unit */
                        char16_t trail=*ఆధారం;
                        ఒకవేళ(U16_IS_TRAIL(trail)) {
                            ++ఆధారం;
                            ++తదుపరి_ఆధారం_సూచీ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            ఒకవేళ(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                /* callback(unassigned) */
                                వెళ్ళు unassigned;
                            }
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } లేకపోతే {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                            ఆపు;
                        }
                    } లేకపోతే {
                        /* no more input */
                        ఆపు;
                    }
                } లేకపోతే {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    ఆపు;
                }
            }

            /* convert the Unicode code point in c into codepage bytes */
            value=MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, results, c);

            /* is this code point assigned, or do we use fallbacks? */
            ఒకవేళ(value>=min_విలువ) {
                /* assigned, write the output character bytes from value and length */
                /* length==1 */
                /* this is easy because we know that there is enough space */
                *లక్ష్యం++ = static_cast<uint8_t>(value);
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                --లక్ష్యం_సామర్థ్యం;

                /* normal end of conversion: prepare for a new character */
                c=0;
                ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
            } లేకపోతే { /* unassigned */
unassigned:
                /* try an extension mapping */
                p_వాదనలు->ఆధారం=ఆధారం;
                c=_extFromU(cnv, cnv->shared_దత్తాంశం,
                            c, &ఆధారం, ఆధారం_హద్దు,
                            &లక్ష్యం, లక్ష్యం+లక్ష్యం_సామర్థ్యం,
                            &offsets, ఆధారం_సూచీ,
                            p_వాదనలు->flush,
                            p_Error_సంకేతం);
                తదుపరి_ఆధారం_సూచీ += static_cast<int32_t>(ఆధారం - p_వాదనలు->ఆధారం);

                ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                    /* not mappable or buffer overflow */
                    ఆపు;
                } లేకపోతే {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(p_వాదనలు->లక్ష్యం_హద్దు - reinterpret_cast<అక్షరం*>(లక్ష్యం));

                    /* normal end of conversion: prepare for a new character */
                    ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                }
            }
        } లేకపోతే {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=ఆధారం;
    p_వాదనలు->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
    p_వాదనలు->offsets=offsets;
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
స్థిర శూన్య
ucnv_MBCSSingleFromBMPWithOffsets(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
                              UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి char16_t *ఆధారం, *ఆధారం_హద్దు, *చివరి_ఆధారం;
    uint8_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం, length;
    int32_t *offsets;

    స్థిరరాశి uint16_t *పట్టిక;
    స్థిరరాశి uint16_t *results;

    UChar32 c;

    int32_t ఆధారం_సూచీ;

    uint32_t asciiRoundtrips;
    uint16_t value, min_విలువ;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    ఆధారం=p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం = reinterpret_cast<uint8_t*>(p_వాదనలు->లక్ష్యం);
    లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(p_వాదనలు->లక్ష్యం_హద్దు - p_వాదనలు->లక్ష్యం);
    offsets=p_వాదనలు->offsets;

    పట్టిక=cnv->shared_దత్తాంశం->mbcs.from_Unicode_పట్టిక;
    ఒకవేళ((cnv->options&UCNV_OPTION_మార్పిడి_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_దత్తాంశం->mbcs.మార్పిడి_LFNLFrom_Unicode_బైట్లు);
    } లేకపోతే {
        results=(uint16_t *)cnv->shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు;
    }
    asciiRoundtrips=cnv->shared_దత్తాంశం->mbcs.asciiRoundtrips;

    ఒకవేళ(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_విలువ=0x800;
    } లేకపోతే {
        /* use only roundtrips and fallbacks from private-use characters */
        min_విలువ=0xc00;
    }

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ= c==0 ? 0 : -1;
    చివరి_ఆధారం=ఆధారం;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(ఆధారం_హద్దు - ఆధారం);
    ఒకవేళ(length<లక్ష్యం_సామర్థ్యం) {
        లక్ష్యం_సామర్థ్యం=length;
    }

    /* conversion loop */
    ఒకవేళ(c!=0 && లక్ష్యం_సామర్థ్యం>0) {
        వెళ్ళు getTrail;
    }

#ఒకవేళ MBCS_UNROLL_SINGLE_FROM_BMP
    /* unrolling makes it slower on Pentium III/Windows 2000?! */
    /* unroll the loop with the most common case */
unrolled:
    ఒకవేళ(లక్ష్యం_సామర్థ్యం>=4) {
        int32_t count, loops;
        uint16_t anded_విలువలు;

        loops=count=లక్ష్యం_సామర్థ్యం>>2;
        చేయి {
            c=*ఆధారం++;
            anded_విలువలు=value=MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, results, c);
            *లక్ష్యం++=(uint8_t)value;
            c=*ఆధారం++;
            anded_విలువలు&=value=MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, results, c);
            *లక్ష్యం++=(uint8_t)value;
            c=*ఆధారం++;
            anded_విలువలు&=value=MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, results, c);
            *లక్ష్యం++=(uint8_t)value;
            c=*ఆధారం++;
            anded_విలువలు&=value=MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, results, c);
            *లక్ష్యం++=(uint8_t)value;

            /* were all 4 entries really valid? */
            ఒకవేళ(anded_విలువలు<min_విలువ) {
                /* no, return to the first of these 4 */
                ఆధారం-=4;
                లక్ష్యం-=4;
                ఆపు;
            }
        } అయితే(--count>0);
        count=loops-count;
        లక్ష్యం_సామర్థ్యం-=4*count;

        ఒకవేళ(offsets!=శూన్యసూచిక) {
            చివరి_ఆధారం+=4*count;
            అయితే(count>0) {
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                --count;
            }
        }

        c=0;
    }
#అయితే_ముగింపు

    అయితే(లక్ష్యం_సామర్థ్యం>0) {
        /*
         * Get a correct Unicode code point:
         * a single char16_t for a BMP code point or
         * a matched surrogate pair for a "supplementary code point".
         */
        c=*ఆధారం++;
        /*
         * Do not immediately check for single surrogates:
         * Assume that they are unassigned and check for them in that case.
         * This speeds up the conversion of assigned characters.
         */
        /* convert the Unicode code point in c into codepage bytes */
        ఒకవేళ(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
            *లక్ష్యం++ = static_cast<uint8_t>(c);
            --లక్ష్యం_సామర్థ్యం;
            c=0;
            కొనసాగు;
        }
        value=MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, results, c);
        /* is this code point assigned, or do we use fallbacks? */
        ఒకవేళ(value>=min_విలువ) {
            /* assigned, write the output character bytes from value and length */
            /* length==1 */
            /* this is easy because we know that there is enough space */
            *లక్ష్యం++ = static_cast<uint8_t>(value);
            --లక్ష్యం_సామర్థ్యం;

            /* normal end of conversion: prepare for a new character */
            c=0;
            కొనసాగు;
        } లేకపోతే ఒకవేళ(!U16_IS_SURROGATE(c)) {
            /* normal, unassigned BMP character */
        } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
            ఒకవేళ(ఆధారం<ఆధారం_హద్దు) {
                /* test the following code unit */
                char16_t trail=*ఆధారం;
                ఒకవేళ(U16_IS_TRAIL(trail)) {
                    ++ఆధారం;
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } లేకపోతే {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    ఆపు;
                }
            } లేకపోతే {
                /* no more input */
                ఒకవేళ (p_వాదనలు->flush) {
                    *p_Error_సంకేతం=U_TRUNCATED_CHAR_దొరికింది;
                }
                ఆపు;
            }
        } లేకపోతే {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
            ఆపు;
        }

        /* c does not have a mapping */

        /* get the number of code units for c to correctly advance sourceIndex */
        length=U16_నిడివి(c);

        /* set offsets since the start or the last extension */
        ఒకవేళ(offsets!=శూన్యసూచిక) {
            int32_t count = static_cast<int32_t>(ఆధారం - చివరి_ఆధారం);

            /* do not set the offset for this character */
            count-=length;

            అయితే(count>0) {
                *offsets++=ఆధారం_సూచీ++;
                --count;
            }
            /* offsets and sourceIndex are now set for the current character */
        }

        /* try an extension mapping */
        చివరి_ఆధారం=ఆధారం;
        c=_extFromU(cnv, cnv->shared_దత్తాంశం,
                    c, &ఆధారం, ఆధారం_హద్దు,
                    &లక్ష్యం, reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->లక్ష్యం_హద్దు),
                    &offsets, ఆధారం_సూచీ,
                    p_వాదనలు->flush,
                    p_Error_సంకేతం);
        ఆధారం_సూచీ += length + static_cast<int32_t>(ఆధారం - చివరి_ఆధారం);
        చివరి_ఆధారం=ఆధారం;

        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            /* not mappable or buffer overflow */
            ఆపు;
        } లేకపోతే {
            /* a mapping was written to the target, continue */

            /* recalculate the targetCapacity after an extension mapping */
            లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(p_వాదనలు->లక్ష్యం_హద్దు - reinterpret_cast<అక్షరం*>(లక్ష్యం));
            length = static_cast<int32_t>(ఆధారం_హద్దు - ఆధారం);
            ఒకవేళ(length<లక్ష్యం_సామర్థ్యం) {
                లక్ష్యం_సామర్థ్యం=length;
            }
        }

#ఒకవేళ MBCS_UNROLL_SINGLE_FROM_BMP
        /* unrolling makes it slower on Pentium III/Windows 2000?! */
        వెళ్ళు unrolled;
#అయితే_ముగింపు
    }

    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం) && ఆధారం<ఆధారం_హద్దు && లక్ష్యం>=(uint8_t *)p_వాదనలు->లక్ష్యం_హద్దు) {
        /* target is full */
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
    }

    /* set offsets since the start or the last callback */
    ఒకవేళ(offsets!=శూన్యసూచిక) {
        size_t count=ఆధారం-చివరి_ఆధారం;
        ఒకవేళ (count > 0 && *p_Error_సంకేతం == U_TRUNCATED_CHAR_దొరికింది) {
            /*
            Caller gave us a partial supplementary character,
            which this function couldn't convert in any case.
            The callback will handle the offset.
            */
            count--;
        }
        అయితే(count>0) {
            *offsets++=ఆధారం_సూచీ++;
            --count;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=ఆధారం;
    p_వాదనలు->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
    p_వాదనలు->offsets=offsets;
}

U_CFUNC శూన్య
ucnv_MBCSFromUnicodeWithOffsets(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
                            UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి char16_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం;
    int32_t *offsets;

    స్థిరరాశి uint16_t *పట్టిక;
    స్థిరరాశి uint16_t *mbcs_సూచీ;
    స్థిరరాశి uint8_t *p, *బైట్లు;
    uint8_t outputType;

    UChar32 c;

    int32_t prev_ఆధారం_సూచీ, ఆధారం_సూచీ, తదుపరి_ఆధారం_సూచీ;

    uint32_t stage2_నమోదు;
    uint32_t asciiRoundtrips;
    uint32_t value;
    /* Shift-In and Shift-Out byte sequences differ by encoding scheme. */
    uint8_t si_బైట్లు[2] = {0, 0};
    uint8_t so_బైట్లు[2] = {0, 0};
    uint8_t si_నిడివి, so_నిడివి;
    int32_t length = 0, prev_నిడివి;
    uint8_t unicode_ముసుగు;

    cnv=p_వాదనలు->converter;

    ఒకవేళ(cnv->preFromUFirstCP>=0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_సరిపోలిక_From_U(cnv, p_వాదనలు, -1, p_Error_సంకేతం);

        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం) || cnv->preFromULength<0) {
            తిరిగి;
        }
    }

    /* use optimized function if possible */
    outputType=cnv->shared_దత్తాంశం->mbcs.outputType;
    unicode_ముసుగు=cnv->shared_దత్తాంశం->mbcs.unicode_ముసుగు;
    ఒకవేళ(outputType==MBCS_OUTPUT_1 && !(unicode_ముసుగు&UCNV_HAS_SURROGATES)) {
        ఒకవేళ(!(unicode_ముసుగు&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingleFromBMPWithOffsets(p_వాదనలు, p_Error_సంకేతం);
        } లేకపోతే {
            ucnv_MBCSSingleFromUnicodeWithOffsets(p_వాదనలు, p_Error_సంకేతం);
        }
        తిరిగి;
    } లేకపోతే ఒకవేళ(outputType==MBCS_OUTPUT_2 && cnv->shared_దత్తాంశం->mbcs.utf8Friendly) {
        ucnv_MBCSDoubleFromUnicodeWithOffsets(p_వాదనలు, p_Error_సంకేతం);
        తిరిగి;
    }

    /* set up the local pointers */
    ఆధారం=p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం=(uint8_t *)p_వాదనలు->లక్ష్యం;
    లక్ష్యం_సామర్థ్యం=(int32_t)(p_వాదనలు->లక్ష్యం_హద్దు-p_వాదనలు->లక్ష్యం);
    offsets=p_వాదనలు->offsets;

    పట్టిక=cnv->shared_దత్తాంశం->mbcs.from_Unicode_పట్టిక;
    ఒకవేళ(cnv->shared_దత్తాంశం->mbcs.utf8Friendly) {
        mbcs_సూచీ=cnv->shared_దత్తాంశం->mbcs.mbcs_సూచీ;
    } లేకపోతే {
        mbcs_సూచీ=శూన్యసూచిక;
    }
    ఒకవేళ((cnv->options&UCNV_OPTION_మార్పిడి_LFNL)!=0) {
        బైట్లు=cnv->shared_దత్తాంశం->mbcs.మార్పిడి_LFNLFrom_Unicode_బైట్లు;
    } లేకపోతే {
        బైట్లు=cnv->shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు;
    }
    asciiRoundtrips=cnv->shared_దత్తాంశం->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    ఒకవేళ(outputType==MBCS_OUTPUT_2_SISO) {
        prev_నిడివి=cnv->from_Unicode_స్థితి;
        ఒకవేళ(prev_నిడివి==0) {
            /* set the real value */
            prev_నిడివి=1;
        }
    } లేకపోతే {
        /* prevent fromUnicodeStatus from being set to something non-0 */
        prev_నిడివి=0;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    prev_ఆధారం_సూచీ=-1;
    ఆధారం_సూచీ= c==0 ? 0 : -1;
    తదుపరి_ఆధారం_సూచీ=0;

    /* Get the SI/SO character for the converter */
    si_నిడివి = static_cast<uint8_t>(getSISOBytes(SI, cnv->options, si_బైట్లు));
    so_నిడివి = static_cast<uint8_t>(getSISOBytes(SO, cnv->options, so_బైట్లు));

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
    ఒకవేళ(c!=0 && లక్ష్యం_సామర్థ్యం>0) {
        వెళ్ళు getTrail;
    }

    అయితే(ఆధారం<ఆధారం_హద్దు) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ఒకవేళ(లక్ష్యం_సామర్థ్యం>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ఆధారం++;
            ++తదుపరి_ఆధారం_సూచీ;
            ఒకవేళ(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *లక్ష్యం++=(uint8_t)c;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                    prev_ఆధారం_సూచీ=ఆధారం_సూచీ;
                    ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                }
                --లక్ష్యం_సామర్థ్యం;
                c=0;
                కొనసాగు;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            ఒకవేళ(c<=0xd7ff && mbcs_సూచీ!=శూన్యసూచిక) {
                value=mbcs_సూచీ[c>>6];

                /* get the bytes and the length for the output (copied from below and adapted for utf8Friendly data) */
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                ఎంపిక(outputType) {
                స్థితి MBCS_OUTPUT_2:
                    value=((స్థిరరాశి uint16_t *)బైట్లు)[value +(c&0x3f)];
                    ఒకవేళ(value<=0xff) {
                        ఒకవేళ(value==0) {
                            వెళ్ళు unassigned;
                        } లేకపోతే {
                            length=1;
                        }
                    } లేకపోతే {
                        length=2;
                    }
                    ఆపు;
                స్థితి MBCS_OUTPUT_2_SISO:
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
                    cnv->from_Unicode_స్థితి=prev_నిడివి; /* save the old state */
                    value=((స్థిరరాశి uint16_t *)బైట్లు)[value +(c&0x3f)];
                    ఒకవేళ(value<=0xff) {
                        ఒకవేళ(value==0) {
                            వెళ్ళు unassigned;
                        } లేకపోతే ఒకవేళ(prev_నిడివి<=1) {
                            length=1;
                        } లేకపోతే {
                            /* change from double-byte mode to single-byte */
                            ఒకవేళ (si_నిడివి == 1) {
                                value|=(uint32_t)si_బైట్లు[0]<<8;
                                length = 2;
                            } లేకపోతే ఒకవేళ (si_నిడివి == 2) {
                                value|=(uint32_t)si_బైట్లు[1]<<8;
                                value|=(uint32_t)si_బైట్లు[0]<<16;
                                length = 3;
                            }
                            prev_నిడివి=1;
                        }
                    } లేకపోతే {
                        ఒకవేళ(prev_నిడివి==2) {
                            length=2;
                        } లేకపోతే {
                            /* change from single-byte mode to double-byte */
                            ఒకవేళ (so_నిడివి == 1) {
                                value|=(uint32_t)so_బైట్లు[0]<<16;
                                length = 3;
                            } లేకపోతే ఒకవేళ (so_నిడివి == 2) {
                                value|=(uint32_t)so_బైట్లు[1]<<16;
                                value|=(uint32_t)so_బైట్లు[0]<<24;
                                length = 4;
                            }
                            prev_నిడివి=2;
                        }
                    }
                    ఆపు;
                స్థితి MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=((స్థిరరాశి uint16_t *)బైట్లు)[value +(c&0x3f)];
                    ఒకవేళ(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        వెళ్ళు unassigned;
                    } లేకపోతే {
                        length=2;
                    }
                    ఆపు;
                స్థితి MBCS_OUTPUT_3:
                    p=బైట్లు+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    ఒకవేళ(value<=0xff) {
                        ఒకవేళ(value==0) {
                            వెళ్ళు unassigned;
                        } లేకపోతే {
                            length=1;
                        }
                    } లేకపోతే ఒకవేళ(value<=0xffff) {
                        length=2;
                    } లేకపోతే {
                        length=3;
                    }
                    ఆపు;
                స్థితి MBCS_OUTPUT_4:
                    value=((స్థిరరాశి uint32_t *)బైట్లు)[value +(c&0x3f)];
                    ఒకవేళ(value<=0xff) {
                        ఒకవేళ(value==0) {
                            వెళ్ళు unassigned;
                        } లేకపోతే {
                            length=1;
                        }
                    } లేకపోతే ఒకవేళ(value<=0xffff) {
                        length=2;
                    } లేకపోతే ఒకవేళ(value<=0xffffff) {
                        length=3;
                    } లేకపోతే {
                        length=4;
                    }
                    ఆపు;
                స్థితి MBCS_OUTPUT_3_EUC:
                    value=((స్థిరరాశి uint16_t *)బైట్లు)[value +(c&0x3f)];
                    /* EUC 16-bit fixed-length representation */
                    ఒకవేళ(value<=0xff) {
                        ఒకవేళ(value==0) {
                            వెళ్ళు unassigned;
                        } లేకపోతే {
                            length=1;
                        }
                    } లేకపోతే ఒకవేళ((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } లేకపోతే ఒకవేళ((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } లేకపోతే {
                        length=2;
                    }
                    ఆపు;
                స్థితి MBCS_OUTPUT_4_EUC:
                    p=బైట్లు+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    ఒకవేళ(value<=0xff) {
                        ఒకవేళ(value==0) {
                            వెళ్ళు unassigned;
                        } లేకపోతే {
                            length=1;
                        }
                    } లేకపోతే ఒకవేళ(value<=0xffff) {
                        length=2;
                    } లేకపోతే ఒకవేళ((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } లేకపోతే ఒకవేళ((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } లేకపోతే {
                        length=3;
                    }
                    ఆపు;
                అప్రమేయ:
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
                    ఆపు;
                }
                /* output the value */
            } లేకపోతే {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                ఒకవేళ(U16_IS_SURROGATE(c) && !(unicode_ముసుగు&UCNV_HAS_SURROGATES)) {
                    ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        ఒకవేళ(ఆధారం<ఆధారం_హద్దు) {
                            /* test the following code unit */
                            char16_t trail=*ఆధారం;
                            ఒకవేళ(U16_IS_TRAIL(trail)) {
                                ++ఆధారం;
                                ++తదుపరి_ఆధారం_సూచీ;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                ఒకవేళ(!(unicode_ముసుగు&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    cnv->from_Unicode_స్థితి=prev_నిడివి; /* save the old state */
                                    /* callback(unassigned) */
                                    వెళ్ళు unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } లేకపోతే {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                                ఆపు;
                            }
                        } లేకపోతే {
                            /* no more input */
                            ఆపు;
                        }
                    } లేకపోతే {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                        ఆపు;
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
                stage2_నమోదు=MBCS_STAGE_2_FROM_U(పట్టిక, c);

                /* get the bytes and the length for the output */
                ఎంపిక(outputType) {
                స్థితి MBCS_OUTPUT_2:
                    value=MBCS_విలువ_2_FROM_STAGE_2(బైట్లు, stage2_నమోదు, c);
                    ఒకవేళ(value<=0xff) {
                        length=1;
                    } లేకపోతే {
                        length=2;
                    }
                    ఆపు;
                స్థితి MBCS_OUTPUT_2_SISO:
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
                    cnv->from_Unicode_స్థితి=prev_నిడివి; /* save the old state */
                    value=MBCS_విలువ_2_FROM_STAGE_2(బైట్లు, stage2_నమోదు, c);
                    ఒకవేళ(value<=0xff) {
                        ఒకవేళ(value==0 && MBCS_FROM_U_IS_ROUNDTRIP(stage2_నమోదు, c)==0) {
                            /* no mapping, leave value==0 */
                            length=0;
                        } లేకపోతే ఒకవేళ(prev_నిడివి<=1) {
                            length=1;
                        } లేకపోతే {
                            /* change from double-byte mode to single-byte */
                            ఒకవేళ (si_నిడివి == 1) {
                                value|=(uint32_t)si_బైట్లు[0]<<8;
                                length = 2;
                            } లేకపోతే ఒకవేళ (si_నిడివి == 2) {
                                value|=(uint32_t)si_బైట్లు[1]<<8;
                                value|=(uint32_t)si_బైట్లు[0]<<16;
                                length = 3;
                            }
                            prev_నిడివి=1;
                        }
                    } లేకపోతే {
                        ఒకవేళ(prev_నిడివి==2) {
                            length=2;
                        } లేకపోతే {
                            /* change from single-byte mode to double-byte */
                            ఒకవేళ (so_నిడివి == 1) {
                                value|=(uint32_t)so_బైట్లు[0]<<16;
                                length = 3;
                            } లేకపోతే ఒకవేళ (so_నిడివి == 2) {
                                value|=(uint32_t)so_బైట్లు[1]<<16;
                                value|=(uint32_t)so_బైట్లు[0]<<24;
                                length = 4;
                            }
                            prev_నిడివి=2;
                        }
                    }
                    ఆపు;
                స్థితి MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=MBCS_విలువ_2_FROM_STAGE_2(బైట్లు, stage2_నమోదు, c);
                    ఒకవేళ(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        value=stage2_నమోదు=0; /* stage2Entry=0 to reset roundtrip flags */
                        length=0;
                    } లేకపోతే {
                        length=2;
                    }
                    ఆపు;
                స్థితి MBCS_OUTPUT_3:
                    p=MBCS_సూచిక_3_FROM_STAGE_2(బైట్లు, stage2_నమోదు, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    ఒకవేళ(value<=0xff) {
                        length=1;
                    } లేకపోతే ఒకవేళ(value<=0xffff) {
                        length=2;
                    } లేకపోతే {
                        length=3;
                    }
                    ఆపు;
                స్థితి MBCS_OUTPUT_4:
                    value=MBCS_విలువ_4_FROM_STAGE_2(బైట్లు, stage2_నమోదు, c);
                    ఒకవేళ(value<=0xff) {
                        length=1;
                    } లేకపోతే ఒకవేళ(value<=0xffff) {
                        length=2;
                    } లేకపోతే ఒకవేళ(value<=0xffffff) {
                        length=3;
                    } లేకపోతే {
                        length=4;
                    }
                    ఆపు;
                స్థితి MBCS_OUTPUT_3_EUC:
                    value=MBCS_విలువ_2_FROM_STAGE_2(బైట్లు, stage2_నమోదు, c);
                    /* EUC 16-bit fixed-length representation */
                    ఒకవేళ(value<=0xff) {
                        length=1;
                    } లేకపోతే ఒకవేళ((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } లేకపోతే ఒకవేళ((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } లేకపోతే {
                        length=2;
                    }
                    ఆపు;
                స్థితి MBCS_OUTPUT_4_EUC:
                    p=MBCS_సూచిక_3_FROM_STAGE_2(బైట్లు, stage2_నమోదు, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    ఒకవేళ(value<=0xff) {
                        length=1;
                    } లేకపోతే ఒకవేళ(value<=0xffff) {
                        length=2;
                    } లేకపోతే ఒకవేళ((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } లేకపోతే ఒకవేళ((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } లేకపోతే {
                        length=3;
                    }
                    ఆపు;
                అప్రమేయ:
                    /* must not occur */
                    /*
                     * To avoid compiler warnings that value & length may be
                     * used without having been initialized, we set them here.
                     * In reality, this is unreachable code.
                     * Not having a default branch also causes warnings with
                     * some compilers.
                     */
                    value=stage2_నమోదు=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                    ఆపు;
                }

                /* is this code point assigned, or do we use fallbacks? */
                ఒకవేళ(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_నమోదు, c)!=0 ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_వాదనలు->ఆధారం=ఆధారం;
                    c=_extFromU(cnv, cnv->shared_దత్తాంశం,
                                c, &ఆధారం, ఆధారం_హద్దు,
                                &లక్ష్యం, లక్ష్యం+లక్ష్యం_సామర్థ్యం,
                                &offsets, ఆధారం_సూచీ,
                                p_వాదనలు->flush,
                                p_Error_సంకేతం);
                    తదుపరి_ఆధారం_సూచీ+=(int32_t)(ఆధారం-p_వాదనలు->ఆధారం);
                    prev_నిడివి=cnv->from_Unicode_స్థితి; /* restore SISO state */

                    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                        /* not mappable or buffer overflow */
                        ఆపు;
                    } లేకపోతే {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        లక్ష్యం_సామర్థ్యం=(int32_t)(p_వాదనలు->లక్ష్యం_హద్దు-(అక్షరం *)లక్ష్యం);

                        /* normal end of conversion: prepare for a new character */
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            prev_ఆధారం_సూచీ=ఆధారం_సూచీ;
                            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                        }
                        కొనసాగు;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            ఒకవేళ(length<=లక్ష్యం_సామర్థ్యం) {
                ఒకవేళ(offsets==శూన్యసూచిక) {
                    ఎంపిక(length) {
                        /* each branch falls through to the next one */
                    స్థితి 4:
                        *లక్ష్యం++=(uint8_t)(value>>24);
                        U_FALLTHROUGH;
                    స్థితి 3:
                        *లక్ష్యం++=(uint8_t)(value>>16);
                        U_FALLTHROUGH;
                    స్థితి 2:
                        *లక్ష్యం++=(uint8_t)(value>>8);
                        U_FALLTHROUGH;
                    స్థితి 1:
                        *లక్ష్యం++=(uint8_t)value;
                        U_FALLTHROUGH;
                    అప్రమేయ:
                        /* will never occur */
                        ఆపు;
                    }
                } లేకపోతే {
                    ఎంపిక(length) {
                        /* each branch falls through to the next one */
                    స్థితి 4:
                        *లక్ష్యం++=(uint8_t)(value>>24);
                        *offsets++=ఆధారం_సూచీ;
                        U_FALLTHROUGH;
                    స్థితి 3:
                        *లక్ష్యం++=(uint8_t)(value>>16);
                        *offsets++=ఆధారం_సూచీ;
                        U_FALLTHROUGH;
                    స్థితి 2:
                        *లక్ష్యం++=(uint8_t)(value>>8);
                        *offsets++=ఆధారం_సూచీ;
                        U_FALLTHROUGH;
                    స్థితి 1:
                        *లక్ష్యం++=(uint8_t)value;
                        *offsets++=ఆధారం_సూచీ;
                        U_FALLTHROUGH;
                    అప్రమేయ:
                        /* will never occur */
                        ఆపు;
                    }
                }
                లక్ష్యం_సామర్థ్యం-=length;
            } లేకపోతే {
                uint8_t *char_Error_బఫరు;

                /*
                 * We actually do this backwards here:
                 * In order to save an intermediate variable, we output
                 * first to the overflow buffer what does not fit into the
                 * regular target.
                 */
                /* we know that 1<=targetCapacity<length<=4 */
                length-=లక్ష్యం_సామర్థ్యం;
                char_Error_బఫరు=(uint8_t *)cnv->char_Error_బఫరు;
                ఎంపిక(length) {
                    /* each branch falls through to the next one */
                స్థితి 3:
                    *char_Error_బఫరు++=(uint8_t)(value>>16);
                    U_FALLTHROUGH;
                స్థితి 2:
                    *char_Error_బఫరు++=(uint8_t)(value>>8);
                    U_FALLTHROUGH;
                స్థితి 1:
                    *char_Error_బఫరు=(uint8_t)value;
                    U_FALLTHROUGH;
                అప్రమేయ:
                    /* will never occur */
                    ఆపు;
                }
                cnv->char_Error_బఫరు_నిడివి=(int8_t)length;

                /* now output what fits into the regular target */
                value>>=8*length; /* length was reduced by targetCapacity */
                ఎంపిక(లక్ష్యం_సామర్థ్యం) {
                    /* each branch falls through to the next one */
                స్థితి 3:
                    *లక్ష్యం++=(uint8_t)(value>>16);
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                    U_FALLTHROUGH;
                స్థితి 2:
                    *లక్ష్యం++=(uint8_t)(value>>8);
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                    U_FALLTHROUGH;
                స్థితి 1:
                    *లక్ష్యం++=(uint8_t)value;
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                    U_FALLTHROUGH;
                అప్రమేయ:
                    /* will never occur */
                    ఆపు;
                }

                /* target overflow */
                లక్ష్యం_సామర్థ్యం=0;
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                c=0;
                ఆపు;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ఒకవేళ(offsets!=శూన్యసూచిక) {
                prev_ఆధారం_సూచీ=ఆధారం_సూచీ;
                ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
            }
            కొనసాగు;
        } లేకపోతే {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
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
    ఒకవేళ( U_SUCCESS(*p_Error_సంకేతం) &&
        outputType==MBCS_OUTPUT_2_SISO && prev_నిడివి==2 &&
        p_వాదనలు->flush && ఆధారం>=ఆధారం_హద్దు && c==0
    ) {
        /* EBCDIC_STATEFUL ending with DBCS: emit an SI to return the output stream to SBCS */
        ఒకవేళ(లక్ష్యం_సామర్థ్యం>0) {
            *లక్ష్యం++ = si_బైట్లు[0];
            ఒకవేళ (si_నిడివి == 2) {
                ఒకవేళ (లక్ష్యం_సామర్థ్యం<2) {
                    cnv->char_Error_బఫరు[0] = si_బైట్లు[1];
                    cnv->char_Error_బఫరు_నిడివి=1;
                    *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                } లేకపోతే {
                    *లక్ష్యం++ = si_బైట్లు[1];
                }
            }
            ఒకవేళ(offsets!=శూన్యసూచిక) {
                /* set the last source character's index (sourceIndex points at sourceLimit now) */
                *offsets++=prev_ఆధారం_సూచీ;
            }
        } లేకపోతే {
            /* target is full */
            cnv->char_Error_బఫరు[0] = si_బైట్లు[0];
            ఒకవేళ (si_నిడివి == 2) {
                cnv->char_Error_బఫరు[1] = si_బైట్లు[1];
            }
            cnv->char_Error_బఫరు_నిడివి=si_నిడివి;
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        }
        prev_నిడివి=1; /* we switched into SBCS */
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;
    cnv->from_Unicode_స్థితి=prev_నిడివి;

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=ఆధారం;
    p_వాదనలు->లక్ష్యం=(అక్షరం *)లక్ష్యం;
    p_వాదనలు->offsets=offsets;
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
ucnv_MBCSFromUChar32(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం,
                 UChar32 c, uint32_t *p_విలువ,
                 UBool useFallback) {
    స్థిరరాశి int32_t *cx;
    స్థిరరాశి uint16_t *పట్టిక;
#ఒకవేళ 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
    స్థిరరాశి uint8_t *p;
#అయితే_ముగింపు
    uint32_t stage2_నమోదు;
    uint32_t value;
    int32_t length;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    ఒకవేళ(c<=0xffff || (shared_దత్తాంశం->mbcs.unicode_ముసుగు&UCNV_HAS_SUPPLEMENTARY)) {
        పట్టిక=shared_దత్తాంశం->mbcs.from_Unicode_పట్టిక;

        /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
        ఒకవేళ(shared_దత్తాంశం->mbcs.outputType==MBCS_OUTPUT_1) {
            value=MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, (uint16_t *)shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు, c);
            /* is this code point assigned, or do we use fallbacks? */
            ఒకవేళ(useFallback ? value>=0x800 : value>=0xc00) {
                *p_విలువ=value&0xff;
                తిరిగి 1;
            }
        } లేకపోతే /* outputType!=MBCS_OUTPUT_1 */ {
            stage2_నమోదు=MBCS_STAGE_2_FROM_U(పట్టిక, c);

            /* get the bytes and the length for the output */
            ఎంపిక(shared_దత్తాంశం->mbcs.outputType) {
            స్థితి MBCS_OUTPUT_2:
                value=MBCS_విలువ_2_FROM_STAGE_2(shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు, stage2_నమోదు, c);
                ఒకవేళ(value<=0xff) {
                    length=1;
                } లేకపోతే {
                    length=2;
                }
                ఆపు;
#ఒకవేళ 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
            స్థితి MBCS_OUTPUT_DBCS_ONLY:
                /* table with single-byte results, but only DBCS mappings used */
                value=MBCS_విలువ_2_FROM_STAGE_2(shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు, stage2_నమోదు, c);
                ఒకవేళ(value<=0xff) {
                    /* no mapping or SBCS result, not taken for DBCS-only */
                    value=stage2_నమోదు=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                } లేకపోతే {
                    length=2;
                }
                ఆపు;
            స్థితి MBCS_OUTPUT_3:
                p=MBCS_సూచిక_3_FROM_STAGE_2(shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు, stage2_నమోదు, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                ఒకవేళ(value<=0xff) {
                    length=1;
                } లేకపోతే ఒకవేళ(value<=0xffff) {
                    length=2;
                } లేకపోతే {
                    length=3;
                }
                ఆపు;
            స్థితి MBCS_OUTPUT_4:
                value=MBCS_విలువ_4_FROM_STAGE_2(shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు, stage2_నమోదు, c);
                ఒకవేళ(value<=0xff) {
                    length=1;
                } లేకపోతే ఒకవేళ(value<=0xffff) {
                    length=2;
                } లేకపోతే ఒకవేళ(value<=0xffffff) {
                    length=3;
                } లేకపోతే {
                    length=4;
                }
                ఆపు;
            స్థితి MBCS_OUTPUT_3_EUC:
                value=MBCS_విలువ_2_FROM_STAGE_2(shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు, stage2_నమోదు, c);
                /* EUC 16-bit fixed-length representation */
                ఒకవేళ(value<=0xff) {
                    length=1;
                } లేకపోతే ఒకవేళ((value&0x8000)==0) {
                    value|=0x8e8000;
                    length=3;
                } లేకపోతే ఒకవేళ((value&0x80)==0) {
                    value|=0x8f0080;
                    length=3;
                } లేకపోతే {
                    length=2;
                }
                ఆపు;
            స్థితి MBCS_OUTPUT_4_EUC:
                p=MBCS_సూచిక_3_FROM_STAGE_2(shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు, stage2_నమోదు, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                /* EUC 16-bit fixed-length representation applied to the first two bytes */
                ఒకవేళ(value<=0xff) {
                    length=1;
                } లేకపోతే ఒకవేళ(value<=0xffff) {
                    length=2;
                } లేకపోతే ఒకవేళ((value&0x800000)==0) {
                    value|=0x8e800000;
                    length=4;
                } లేకపోతే ఒకవేళ((value&0x8000)==0) {
                    value|=0x8f008000;
                    length=4;
                } లేకపోతే {
                    length=3;
                }
                ఆపు;
#అయితే_ముగింపు
            అప్రమేయ:
                /* must not occur */
                తిరిగి -1;
            }

            /* is this code point assigned, or do we use fallbacks? */
            ఒకవేళ( MBCS_FROM_U_IS_ROUNDTRIP(stage2_నమోదు, c) ||
                (FROM_U_USE_FALLBACK(useFallback, c) && value!=0)
            ) {
                /*
                 * We allow a 0 byte output if the "assigned" bit is set for this entry.
                 * There is no way with this data structure for fallback output
                 * to be a zero byte.
                 */
                /* assigned */
                *p_విలువ=value;
                తిరిగి length;
            }
        }
    }

    cx=shared_దత్తాంశం->mbcs.extIndexes;
    ఒకవేళ(cx!=శూన్యసూచిక) {
        length=ucnv_ext_Simple_సరిపోలిక_From_U(cx, c, p_విలువ, useFallback);
        తిరిగి length>=0 ? length : -length;  /* return abs(length); */
    }

    /* unassigned */
    తిరిగి 0;
}


#ఒకవేళ 0
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
ucnv_MBCSSingleFromUChar32(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం,
                       UChar32 c,
                       UBool useFallback) {
    స్థిరరాశి uint16_t *పట్టిక;
    int32_t value;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    ఒకవేళ(c>=0x10000 && !(shared_దత్తాంశం->mbcs.unicode_ముసుగు&UCNV_HAS_SUPPLEMENTARY)) {
        తిరిగి -1;
    }

    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    పట్టిక=shared_దత్తాంశం->mbcs.from_Unicode_పట్టిక;

    /* get the byte for the output */
    value=MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, (uint16_t *)shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు, c);
    /* is this code point assigned, or do we use fallbacks? */
    ఒకవేళ(useFallback ? value>=0x800 : value>=0xc00) {
        తిరిగి value&0xff;
    } లేకపోతే {
        తిరిగి -1;
    }
}
#అయితే_ముగింపు

/* MBCS-from-UTF-8 conversion functions ------------------------------------- */

/* offsets for n-byte UTF-8 sequences that were calculated with ((lead<<6)+trail)<<6+trail... */
స్థిర స్థిరరాశి UChar32
utf8_offsets[5]={ 0, 0, 0x3080, 0xE2080, 0x3C82080 };

స్థిర శూన్య U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_వాదనలు *pFromUArgs,
                  UConverter_గమ్యం_Unicode_వాదనలు *p_గమ్యం_UArgs,
                  UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *utf8, *cnv;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం;

    స్థిరరాశి uint16_t *పట్టిక, *sbcs_సూచీ;
    స్థిరరాశి uint16_t *results;

    int8_t old_గమ్యం_ULength, గమ్యం_ULength, గమ్యం_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t asciiRoundtrips;
    uint16_t value, min_విలువ = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_గమ్యం_UArgs->converter;
    cnv=pFromUArgs->converter;
    ఆధారం=(uint8_t *)p_గమ్యం_UArgs->ఆధారం;
    ఆధారం_హద్దు=(uint8_t *)p_గమ్యం_UArgs->ఆధారం_హద్దు;
    లక్ష్యం = reinterpret_cast<uint8_t*>(pFromUArgs->లక్ష్యం);
    లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(pFromUArgs->లక్ష్యం_హద్దు - pFromUArgs->లక్ష్యం);

    పట్టిక=cnv->shared_దత్తాంశం->mbcs.from_Unicode_పట్టిక;
    sbcs_సూచీ=cnv->shared_దత్తాంశం->mbcs.sbcs_సూచీ;
    ఒకవేళ((cnv->options&UCNV_OPTION_మార్పిడి_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_దత్తాంశం->mbcs.మార్పిడి_LFNLFrom_Unicode_బైట్లు);
    } లేకపోతే {
        results=(uint16_t *)cnv->shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు;
    }
    asciiRoundtrips=cnv->shared_దత్తాంశం->mbcs.asciiRoundtrips;

    ఒకవేళ(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_విలువ=0x800;
    } లేకపోతే {
        /* use only roundtrips and fallbacks from private-use characters */
        min_విలువ=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_దత్తాంశం->mbcs.unicode_ముసుగు & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    ఒకవేళ(utf8->గమ్యం_ULength > 0) {
        గమ్యం_ULength=old_గమ్యం_ULength=utf8->గమ్యం_ULength;
        గమ్యం_ULimit = static_cast<int8_t>(utf8->బహుళకం);
        c = static_cast<UChar32>(utf8->గమ్యం_Unicode_స్థితి);
    } లేకపోతే {
        గమ్యం_ULength=old_గమ్యం_ULength=గమ్యం_ULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(ఆధారం_హద్దు - ఆధారం) - (గమ్యం_ULimit - old_గమ్యం_ULength);
        ఒకవేళ(length>0) {
            uint8_t b1=*(ఆధారం_హద్దు-1);
            ఒకవేళ(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } లేకపోతే ఒకవేళ(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(ఆధారం_హద్దు-2);
                ఒకవేళ(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    ఆధారం_హద్దు-=2;
                }
            } లేకపోతే ఒకవేళ(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --ఆధారం_హద్దు;
            }
        }
    }

    ఒకవేళ(c!=0 && లక్ష్యం_సామర్థ్యం>0) {
        utf8->గమ్యం_Unicode_స్థితి=0;
        utf8->గమ్యం_ULength=0;
        వెళ్ళు మరిన్ని_బైట్లు;
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
    అయితే(ఆధారం<ఆధారం_హద్దు) {
        ఒకవేళ(లక్ష్యం_సామర్థ్యం>0) {
            b=*ఆధారం++;
            ఒకవేళ(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                ఒకవేళ(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *లక్ష్యం++ = b;
                    --లక్ష్యం_సామర్థ్యం;
                    కొనసాగు;
                } లేకపోతే {
                    c=b;
                    value=SBCS_ఫలితం_FROM_UTF8(sbcs_సూచీ, results, 0, c);
                }
            } లేకపోతే {
                ఒకవేళ(b<0xe0) {
                    ఒకవేళ( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*ఆధారం - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++ఆధారం;
                        value=SBCS_ఫలితం_FROM_UTF8(sbcs_సూచీ, results, c, t1);
                        ఒకవేళ(value>=min_విలువ) {
                            *లక్ష్యం++ = static_cast<uint8_t>(value);
                            --లక్ష్యం_సామర్థ్యం;
                            కొనసాగు;
                        } లేకపోతే {
                            c=(c<<6)|t1;
                        }
                    } లేకపోతే {
                        c=-1;
                    }
                } లేకపోతే ఒకవేళ(b==0xe0) {
                    ఒకవేళ( /* handle U+0800..U+0FFF inline */
                        (t1 = static_cast<uint8_t>(ఆధారం[0] - 0x80)) <= 0x3f && t1 >= 0x20 &&
                        (t2 = static_cast<uint8_t>(ఆధారం[1] - 0x80)) <= 0x3f
                    ) {
                        c=t1;
                        ఆధారం+=2;
                        value=SBCS_ఫలితం_FROM_UTF8(sbcs_సూచీ, results, c, t2);
                        ఒకవేళ(value>=min_విలువ) {
                            *లక్ష్యం++ = static_cast<uint8_t>(value);
                            --లక్ష్యం_సామర్థ్యం;
                            కొనసాగు;
                        } లేకపోతే {
                            c=(c<<6)|t2;
                        }
                    } లేకపోతే {
                        c=-1;
                    }
                } లేకపోతే {
                    c=-1;
                }

                ఒకవేళ(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    old_గమ్యం_ULength=0;
                    గమ్యం_ULength=1;
                    గమ్యం_ULimit=U8_లెక్క_బైట్లు_NON_ASCII(b);
                    c=b;
మరిన్ని_బైట్లు:
                    అయితే(గమ్యం_ULength<గమ్యం_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        ఒకవేళ(ఆధారం<(uint8_t *)p_గమ్యం_UArgs->ఆధారం_హద్దు) {
                            b=*ఆధారం;
                            ఒకవేళ(icu::UTF8::isValidTrail(c, b, గమ్యం_ULength, గమ్యం_ULimit)) {
                                ++ఆధారం;
                                ++గమ్యం_ULength;
                                c=(c<<6)+b;
                            } లేకపోతే {
                                ఆపు; /* sequence too short, stop with toULength<toULimit */
                            }
                        } లేకపోతే {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            ఆధారం-=(గమ్యం_ULength-old_గమ్యం_ULength);
                            అయితే(old_గమ్యం_ULength<గమ్యం_ULength) {
                                utf8->గమ్యం_UBytes[old_గమ్యం_ULength++]=*ఆధారం++;
                            }
                            utf8->గమ్యం_Unicode_స్థితి=c;
                            utf8->గమ్యం_ULength=గమ్యం_ULength;
                            utf8->బహుళకం=గమ్యం_ULimit;
                            p_గమ్యం_UArgs->ఆధారం=(అక్షరం *)ఆధారం;
                            pFromUArgs->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
                            తిరిగి;
                        }
                    }

                    ఒకవేళ(గమ్యం_ULength==గమ్యం_ULimit) {
                        c-=utf8_offsets[గమ్యం_ULength];
                        ఒకవేళ(గమ్యం_ULength<=3) {  /* BMP */
                            value=MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, results, c);
                        } లేకపోతే {
                            /* supplementary code point */
                            ఒకవేళ(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                value=0;
                            } లేకపోతే {
                                value=MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, results, c);
                            }
                        }
                    } లేకపోతే {
                        /* error handling: illegal UTF-8 byte sequence */
                        ఆధారం-=(గమ్యం_ULength-old_గమ్యం_ULength);
                        అయితే(old_గమ్యం_ULength<గమ్యం_ULength) {
                            utf8->గమ్యం_UBytes[old_గమ్యం_ULength++]=*ఆధారం++;
                        }
                        utf8->గమ్యం_ULength=గమ్యం_ULength;
                        p_గమ్యం_UArgs->ఆధారం=(అక్షరం *)ఆధారం;
                        pFromUArgs->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
                        *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                        తిరిగి;
                    }
                }
            }

            ఒకవేళ(value>=min_విలువ) {
                /* output the mapping for c */
                *లక్ష్యం++ = static_cast<uint8_t>(value);
                --లక్ష్యం_సామర్థ్యం;
            } లేకపోతే {
                /* value<minValue means c is unassigned (unmappable) */
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                స్థిర స్థిరరాశి char16_t nul=0;
                స్థిరరాశి char16_t *no_ఆధారం=&nul;
                c=_extFromU(cnv, cnv->shared_దత్తాంశం,
                            c, &no_ఆధారం, no_ఆధారం,
                            &లక్ష్యం, లక్ష్యం+లక్ష్యం_సామర్థ్యం,
                            శూన్యసూచిక, -1,
                            pFromUArgs->flush,
                            p_Error_సంకేతం);

                ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    ఆపు;
                } లేకపోతే ఒకవేళ(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *p_Error_సంకేతం=U_USING_DEFAULT_WARNING;
                    ఆపు;
                } లేకపోతే {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(pFromUArgs->లక్ష్యం_హద్దు - reinterpret_cast<అక్షరం*>(లక్ష్యం));
                }
            }
        } లేకపోతే {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం) &&
            cnv->preFromUFirstCP<0 &&
            ఆధారం<(ఆధారం_హద్దు=(uint8_t *)p_గమ్యం_UArgs->ఆధారం_హద్దు)) {
        c=utf8->గమ్యం_UBytes[0]=b=*ఆధారం++;
        గమ్యం_ULength=1;
        గమ్యం_ULimit=U8_లెక్క_బైట్లు(b);
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            utf8->గమ్యం_UBytes[గమ్యం_ULength++]=b=*ఆధారం++;
            c=(c<<6)+b;
        }
        utf8->గమ్యం_Unicode_స్థితి=c;
        utf8->గమ్యం_ULength=గమ్యం_ULength;
        utf8->బహుళకం=గమ్యం_ULimit;
    }

    /* write back the updated pointers */
    p_గమ్యం_UArgs->ఆధారం=(అక్షరం *)ఆధారం;
    pFromUArgs->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
}

స్థిర శూన్య U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_వాదనలు *pFromUArgs,
                  UConverter_గమ్యం_Unicode_వాదనలు *p_గమ్యం_UArgs,
                  UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *utf8, *cnv;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం;

    స్థిరరాశి uint16_t *పట్టిక, *mbcs_సూచీ;
    స్థిరరాశి uint16_t *results;

    int8_t old_గమ్యం_ULength, గమ్యం_ULength, గమ్యం_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t stage2_నమోదు;
    uint32_t asciiRoundtrips;
    uint16_t value = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_గమ్యం_UArgs->converter;
    cnv=pFromUArgs->converter;
    ఆధారం=(uint8_t *)p_గమ్యం_UArgs->ఆధారం;
    ఆధారం_హద్దు=(uint8_t *)p_గమ్యం_UArgs->ఆధారం_హద్దు;
    లక్ష్యం = reinterpret_cast<uint8_t*>(pFromUArgs->లక్ష్యం);
    లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(pFromUArgs->లక్ష్యం_హద్దు - pFromUArgs->లక్ష్యం);

    పట్టిక=cnv->shared_దత్తాంశం->mbcs.from_Unicode_పట్టిక;
    mbcs_సూచీ=cnv->shared_దత్తాంశం->mbcs.mbcs_సూచీ;
    ఒకవేళ((cnv->options&UCNV_OPTION_మార్పిడి_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_దత్తాంశం->mbcs.మార్పిడి_LFNLFrom_Unicode_బైట్లు);
    } లేకపోతే {
        results=(uint16_t *)cnv->shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు;
    }
    asciiRoundtrips=cnv->shared_దత్తాంశం->mbcs.asciiRoundtrips;

    hasSupplementary = static_cast<UBool>(cnv->shared_దత్తాంశం->mbcs.unicode_ముసుగు & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    ఒకవేళ(utf8->గమ్యం_ULength > 0) {
        గమ్యం_ULength=old_గమ్యం_ULength=utf8->గమ్యం_ULength;
        గమ్యం_ULimit = static_cast<int8_t>(utf8->బహుళకం);
        c = static_cast<UChar32>(utf8->గమ్యం_Unicode_స్థితి);
    } లేకపోతే {
        గమ్యం_ULength=old_గమ్యం_ULength=గమ్యం_ULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(ఆధారం_హద్దు - ఆధారం) - (గమ్యం_ULimit - old_గమ్యం_ULength);
        ఒకవేళ(length>0) {
            uint8_t b1=*(ఆధారం_హద్దు-1);
            ఒకవేళ(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } లేకపోతే ఒకవేళ(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(ఆధారం_హద్దు-2);
                ఒకవేళ(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    ఆధారం_హద్దు-=2;
                }
            } లేకపోతే ఒకవేళ(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --ఆధారం_హద్దు;
            }
        }
    }

    ఒకవేళ(c!=0 && లక్ష్యం_సామర్థ్యం>0) {
        utf8->గమ్యం_Unicode_స్థితి=0;
        utf8->గమ్యం_ULength=0;
        వెళ్ళు మరిన్ని_బైట్లు;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    అయితే(ఆధారం<ఆధారం_హద్దు) {
        ఒకవేళ(లక్ష్యం_సామర్థ్యం>0) {
            b=*ఆధారం++;
            ఒకవేళ(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                ఒకవేళ(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *లక్ష్యం++=b;
                    --లక్ష్యం_సామర్థ్యం;
                    కొనసాగు;
                } లేకపోతే {
                    value=DBCS_ఫలితం_FROM_UTF8(mbcs_సూచీ, results, 0, b);
                    ఒకవేళ(value==0) {
                        c=b;
                        వెళ్ళు unassigned;
                    }
                }
            } లేకపోతే {
                ఒకవేళ(b>=0xe0) {
                    ఒకవేళ( /* handle U+0800..U+D7FF inline */
                        b<=0xed &&  // do not assume maxFastUChar>0xd7ff
                        U8_IS_VALID_LEAD3_AND_T1(b, t1=ఆధారం[0]) &&
                        (t2 = static_cast<uint8_t>(ఆధారం[1] - 0x80)) <= 0x3f
                    ) {
                        c=((b&0xf)<<6)|(t1&0x3f);
                        ఆధారం+=2;
                        value=DBCS_ఫలితం_FROM_UTF8(mbcs_సూచీ, results, c, t2);
                        ఒకవేళ(value==0) {
                            c=(c<<6)|t2;
                            వెళ్ళు unassigned;
                        }
                    } లేకపోతే {
                        c=-1;
                    }
                } లేకపోతే {
                    ఒకవేళ( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*ఆధారం - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++ఆధారం;
                        value=DBCS_ఫలితం_FROM_UTF8(mbcs_సూచీ, results, c, t1);
                        ఒకవేళ(value==0) {
                            c=(c<<6)|t1;
                            వెళ్ళు unassigned;
                        }
                    } లేకపోతే {
                        c=-1;
                    }
                }

                ఒకవేళ(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    old_గమ్యం_ULength=0;
                    గమ్యం_ULength=1;
                    గమ్యం_ULimit=U8_లెక్క_బైట్లు_NON_ASCII(b);
                    c=b;
మరిన్ని_బైట్లు:
                    అయితే(గమ్యం_ULength<గమ్యం_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        ఒకవేళ(ఆధారం<(uint8_t *)p_గమ్యం_UArgs->ఆధారం_హద్దు) {
                            b=*ఆధారం;
                            ఒకవేళ(icu::UTF8::isValidTrail(c, b, గమ్యం_ULength, గమ్యం_ULimit)) {
                                ++ఆధారం;
                                ++గమ్యం_ULength;
                                c=(c<<6)+b;
                            } లేకపోతే {
                                ఆపు; /* sequence too short, stop with toULength<toULimit */
                            }
                        } లేకపోతే {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            ఆధారం-=(గమ్యం_ULength-old_గమ్యం_ULength);
                            అయితే(old_గమ్యం_ULength<గమ్యం_ULength) {
                                utf8->గమ్యం_UBytes[old_గమ్యం_ULength++]=*ఆధారం++;
                            }
                            utf8->గమ్యం_Unicode_స్థితి=c;
                            utf8->గమ్యం_ULength=గమ్యం_ULength;
                            utf8->బహుళకం=గమ్యం_ULimit;
                            p_గమ్యం_UArgs->ఆధారం=(అక్షరం *)ఆధారం;
                            pFromUArgs->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
                            తిరిగి;
                        }
                    }

                    ఒకవేళ(గమ్యం_ULength==గమ్యం_ULimit) {
                        c-=utf8_offsets[గమ్యం_ULength];
                        ఒకవేళ(గమ్యం_ULength<=3) {  /* BMP */
                            stage2_నమోదు=MBCS_STAGE_2_FROM_U(పట్టిక, c);
                        } లేకపోతే {
                            /* supplementary code point */
                            ఒకవేళ(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                stage2_నమోదు=0;
                            } లేకపోతే {
                                stage2_నమోదు=MBCS_STAGE_2_FROM_U(పట్టిక, c);
                            }
                        }
                    } లేకపోతే {
                        /* error handling: illegal UTF-8 byte sequence */
                        ఆధారం-=(గమ్యం_ULength-old_గమ్యం_ULength);
                        అయితే(old_గమ్యం_ULength<గమ్యం_ULength) {
                            utf8->గమ్యం_UBytes[old_గమ్యం_ULength++]=*ఆధారం++;
                        }
                        utf8->గమ్యం_ULength=గమ్యం_ULength;
                        p_గమ్యం_UArgs->ఆధారం=(అక్షరం *)ఆధారం;
                        pFromUArgs->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
                        *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                        తిరిగి;
                    }

                    /* get the bytes and the length for the output */
                    /* MBCS_OUTPUT_2 */
                    value=MBCS_విలువ_2_FROM_STAGE_2(results, stage2_నమోదు, c);

                    /* is this code point assigned, or do we use fallbacks? */
                    ఒకవేళ(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_నమోదు, c) ||
                         (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                    ) {
                        వెళ్ళు unassigned;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            ఒకవేళ(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *లక్ష్యం++ = static_cast<uint8_t>(value);
                --లక్ష్యం_సామర్థ్యం;
            } లేకపోతే /* length==2 */ {
                *లక్ష్యం++ = static_cast<uint8_t>(value >> 8);
                ఒకవేళ(2<=లక్ష్యం_సామర్థ్యం) {
                    *లక్ష్యం++ = static_cast<uint8_t>(value);
                    లక్ష్యం_సామర్థ్యం-=2;
                } లేకపోతే {
                    cnv->char_Error_బఫరు[0] = static_cast<అక్షరం>(value);
                    cnv->char_Error_బఫరు_నిడివి=1;

                    /* target overflow */
                    *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                    ఆపు;
                }
            }
            కొనసాగు;

unassigned:
            {
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                స్థిర స్థిరరాశి char16_t nul=0;
                స్థిరరాశి char16_t *no_ఆధారం=&nul;
                c=_extFromU(cnv, cnv->shared_దత్తాంశం,
                            c, &no_ఆధారం, no_ఆధారం,
                            &లక్ష్యం, లక్ష్యం+లక్ష్యం_సామర్థ్యం,
                            శూన్యసూచిక, -1,
                            pFromUArgs->flush,
                            p_Error_సంకేతం);

                ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    ఆపు;
                } లేకపోతే ఒకవేళ(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *p_Error_సంకేతం=U_USING_DEFAULT_WARNING;
                    ఆపు;
                } లేకపోతే {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(pFromUArgs->లక్ష్యం_హద్దు - reinterpret_cast<అక్షరం*>(లక్ష్యం));
                    కొనసాగు;
                }
            }
        } లేకపోతే {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం) &&
            cnv->preFromUFirstCP<0 &&
            ఆధారం<(ఆధారం_హద్దు=(uint8_t *)p_గమ్యం_UArgs->ఆధారం_హద్దు)) {
        c=utf8->గమ్యం_UBytes[0]=b=*ఆధారం++;
        గమ్యం_ULength=1;
        గమ్యం_ULimit=U8_లెక్క_బైట్లు(b);
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            utf8->గమ్యం_UBytes[గమ్యం_ULength++]=b=*ఆధారం++;
            c=(c<<6)+b;
        }
        utf8->గమ్యం_Unicode_స్థితి=c;
        utf8->గమ్యం_ULength=గమ్యం_ULength;
        utf8->బహుళకం=గమ్యం_ULimit;
    }

    /* write back the updated pointers */
    p_గమ్యం_UArgs->ఆధారం=(అక్షరం *)ఆధారం;
    pFromUArgs->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
}

/* miscellaneous ------------------------------------------------------------ */

స్థిర శూన్య U_CALLCONV
ucnv_MBCSGetStarters(స్థిరరాశి UConverter* cnv,
                 UBool starters[256],
                 UError_సంకేతం *) {
    స్థిరరాశి int32_t *state0;
    పూర్ణాంకం i;

    state0=cnv->shared_దత్తాంశం->mbcs.state_పట్టిక[cnv->shared_దత్తాంశం->mbcs.dbcsOnlyState];
    కొరకు(i=0; i<256; ++i) {
        /* all bytes that cause a state transition from state 0 are lead bytes */
        starters[i] = static_cast<UBool>(MBCS_నమోదు_IS_TRANSITION(state0[i]));
    }
}

/*
 * This is an internal function that allows other converter implementations
 * to check whether a byte is a lead byte.
 */
U_CFUNC UBool
ucnv_MBCSIsLeadByte(UConverter_Shared_దత్తాంశం *shared_దత్తాంశం, అక్షరం byte) {
    తిరిగి MBCS_నమోదు_IS_TRANSITION(shared_దత్తాంశం->mbcs.state_పట్టిక[0][(uint8_t)byte]);
}

స్థిర శూన్య U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
              int32_t స్థానభ్రంశం_సూచీ,
              UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv=p_వాదనలు->converter;
    అక్షరం *p, *subchar;
    అక్షరం బఫరు[4];
    int32_t length;

    /* first, select between subChar and subChar1 */
    ఒకవేళ( cnv->subChar1!=0 &&
        (cnv->shared_దత్తాంశం->mbcs.extIndexes!=శూన్యసూచిక ?
            cnv->useSubChar1 :
            (cnv->invalid_UChar_బఫరు[0]<=0xff))
    ) {
        /* select subChar1 if it is set (not 0) and the unmappable Unicode code point is up to U+00ff (IBM MBCS behavior) */
        subchar = reinterpret_cast<అక్షరం*>(&cnv->subChar1);
        length=1;
    } లేకపోతే {
        /* select subChar in all other cases */
        subchar = reinterpret_cast<అక్షరం*>(cnv->subChars);
        length=cnv->subCharLen;
    }

    /* reset the selector for the next code point */
    cnv->useSubChar1=అబద్ధం;

    ఒకవేళ (cnv->shared_దత్తాంశం->mbcs.outputType == MBCS_OUTPUT_2_SISO) {
        p=బఫరు;

        /* fromUnicodeStatus contains prevLength */
        ఎంపిక(length) {
        స్థితి 1:
            ఒకవేళ(cnv->from_Unicode_స్థితి==2) {
                /* DBCS mode and SBCS sub char: change to SBCS */
                cnv->from_Unicode_స్థితి=1;
                *p++=UCNV_SI;
            }
            *p++=subchar[0];
            ఆపు;
        స్థితి 2:
            ఒకవేళ(cnv->from_Unicode_స్థితి<=1) {
                /* SBCS mode and DBCS sub char: change to DBCS */
                cnv->from_Unicode_స్థితి=2;
                *p++=UCNV_SO;
            }
            *p++=subchar[0];
            *p++=subchar[1];
            ఆపు;
        అప్రమేయ:
            *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
            తిరిగి;
        }
        subchar=బఫరు;
        length = static_cast<int32_t>(p - బఫరు);
    }

    ucnv_cb_From_UWrite_బైట్లు(p_వాదనలు, subchar, length, స్థానభ్రంశం_సూచీ, p_Error_సంకేతం);
}

U_CFUNC UConverterType
ucnv_MBCSGetType(స్థిరరాశి UConverter* converter) {
    /* SBCS, DBCS, and EBCDIC_STATEFUL are replaced by MBCS, but here we cheat a little */
    ఒకవేళ(converter->shared_దత్తాంశం->mbcs.లెక్క_స్థితులు==1) {
        తిరిగి (UConverterType)UCNV_SBCS;
    } లేకపోతే ఒకవేళ((converter->shared_దత్తాంశం->mbcs.outputType&0xff)==MBCS_OUTPUT_2_SISO) {
        తిరిగి (UConverterType)UCNV_EBCDIC_STATEFUL;
    } లేకపోతే ఒకవేళ(converter->shared_దత్తాంశం->static_దత్తాంశం->min_బైట్లు_చొప్పున_Char==2 && converter->shared_దత్తాంశం->static_దత్తాంశం->max_బైట్లు_చొప్పున_Char==2) {
        తిరిగి (UConverterType)UCNV_DBCS;
    }
    తిరిగి (UConverterType)UCNV_MBCS;
}

#అయితే_ముగింపు /* #if !UCONFIG_NO_LEGACY_CONVERSION */
