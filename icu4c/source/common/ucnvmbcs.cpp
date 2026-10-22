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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#شامل "unicode/ucnv.h"
#شامل "unicode/ucnv_cb.h"
#شامل "unicode/udata.h"
#شامل "unicode/uset.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "ucnv_bld.h"
#شامل "ucnvmbcs.h"
#شامل "ucnv_ext.h"
#شامل "ucnv_cnv.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "umutex.h"
#شامل "ustr_imp.h"

/* control optimizations according to the platform */
#تعریف_کرو MBCS_UNROLL_SINGLE_مقصد_BMP 1
#تعریف_کرو MBCS_UNROLL_SINGLE_FROM_BMP 0

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
قسمتعریف UBool U_CALLCONV
UConverter_Enum_مقصد_UCallback(ثابت خالی *context, uint32_t value, UChar32 رمز_Points[32]);

ساکن خالی U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_ڈیٹا *shared_ڈیٹا,
          UConverter_بھرو_دلائل *p_دلائل,
          ثابت uint8_t *raw,
          UError_رمز *p_Error_رمز);

ساکن خالی U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_ڈیٹا *shared_ڈیٹا);

ساکن خالی U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_بھرو_دلائل *p_دلائل,
              UError_رمز *p_Error_رمز);

ساکن UChar32 U_CALLCONV
ucnv_MBCSGet_اگلا_UChar(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                  UError_رمز *p_Error_رمز);

ساکن خالی U_CALLCONV
ucnv_MBCSGetStarters(ثابت UConverter* cnv,
                 UBool starters[256],
                 UError_رمز *p_Error_رمز);

U_CDECL_BEGIN
ساکن ثابت حرف* U_CALLCONV
ucnv_MBCSGetName(ثابت UConverter *cnv);
U_CDECL_انتہا

ساکن خالی U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_دلائل *p_دلائل,
              int32_t انحراف_اشاریہ,
              UError_رمز *p_Error_رمز);

ساکن UChar32 U_CALLCONV
ucnv_MBCSGet_اگلا_UChar(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                  UError_رمز *p_Error_رمز);

ساکن خالی U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_دلائل *pFromUArgs,
                  UConverter_مقصد_Unicode_دلائل *p_مقصد_UArgs,
                  UError_رمز *p_Error_رمز);

ساکن خالی U_CALLCONV
ucnv_MBCSGet_Unicode_سیٹ(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverter_Unicode_سیٹ which,
                   UError_رمز *p_Error_رمز);

ساکن خالی U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_دلائل *pFromUArgs,
                  UConverter_مقصد_Unicode_دلائل *p_مقصد_UArgs,
                  UError_رمز *p_Error_رمز);

ساکن ثابت UConverterImpl _SBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    خالی_اشاریہ,
    خالی_اشاریہ,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_اگلا_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    خالی_اشاریہ,
    ucnv_MBCSGet_Unicode_سیٹ,

    خالی_اشاریہ,
    ucnv_SBCSFromUTF8
};

ساکن ثابت UConverterImpl _DBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    خالی_اشاریہ,
    خالی_اشاریہ,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_اگلا_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    خالی_اشاریہ,
    ucnv_MBCSGet_Unicode_سیٹ,

    خالی_اشاریہ,
    ucnv_DBCSFromUTF8
};

ساکن ثابت UConverterImpl _MBCSImpl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    خالی_اشاریہ,
    خالی_اشاریہ,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_اگلا_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    خالی_اشاریہ,
    ucnv_MBCSGet_Unicode_سیٹ,
    خالی_اشاریہ,
    خالی_اشاریہ
};

/* Static data is in tools/makeconv/ucnvstat.c for data-based
 * converters. Be sure to update it as well.
 */

ثابت UConverter_Shared_ڈیٹا _MBCSData={
    حجم(UConverter_Shared_ڈیٹا), 1,
    خالی_اشاریہ, خالی_اشاریہ, جھوٹ, سچ, &_MBCSImpl,
    0, UCNV_MBCS_جدول_INITIALIZER
};


/* GB 18030 data ------------------------------------------------------------ */

/* helper macros for linear values for GB 18030 four-byte sequences */
#تعریف_کرو خطی_18030(a, b, c, d) ((((a)*10+(b))*126L+(c))*10L+(d))

#تعریف_کرو خطی_18030_بنیاد خطی_18030(0x81, 0x30, 0x81, 0x30)

#تعریف_کرو خطی(x) خطی_18030(x>>24, (x>>16)&0xff, (x>>8)&0xff, x&0xff)

/*
 * Some ranges of GB 18030 where both the Unicode code points and the
 * GB four-byte sequences are contiguous and are handled algorithmically by
 * the special callback functions below.
 * The values are start & end of Unicode & GB codes.
 *
 * Note that single surrogates are not mapped by GB 18030
 * as of the re-released mapping tables from 2000-nov-30.
 */
ساکن ثابت uint32_t
gb18030Ranges[14][4]={
    {0x10000, 0x10FFFF, خطی(0x90308130), خطی(0xE3329A35)},
    {0x9FA6, 0xD7FF, خطی(0x82358F33), خطی(0x8336C738)},
    {0x0452, 0x1E3E, خطی(0x8130D330), خطی(0x8135F436)},
    {0x1E40, 0x200F, خطی(0x8135F438), خطی(0x8136A531)},
    {0xE865, 0xF92B, خطی(0x8336D030), خطی(0x84308534)},
    {0x2643, 0x2E80, خطی(0x8137A839), خطی(0x8138FD38)},
    {0xFA2A, 0xFE2F, خطی(0x84309C38), خطی(0x84318537)},
    {0x3CE1, 0x4055, خطی(0x8231D438), خطی(0x8232AF32)},
    {0x361B, 0x3917, خطی(0x8230A633), خطی(0x8230F237)},
    {0x49B8, 0x4C76, خطی(0x8234A131), خطی(0x8234E733)},
    {0x4160, 0x4336, خطی(0x8232C937), خطی(0x8232F837)},
    {0x478E, 0x4946, خطی(0x8233E838), خطی(0x82349638)},
    {0x44D7, 0x464B, خطی(0x8233A339), خطی(0x8233C931)},
    {0xFFE6, 0xFFFF, خطی(0x8431A234), خطی(0x8431A439)}
};

/* bit flag for UConverter.options indicating GB 18030 special handling */
#تعریف_کرو _MBCS_OPTION_GB18030 0x8000

/* bit flag for UConverter.options indicating KEIS,JEF,JIF special handling */
#تعریف_کرو _MBCS_OPTION_KEIS 0x01000
#تعریف_کرو _MBCS_OPTION_JEF  0x02000
#تعریف_کرو _MBCS_OPTION_JIPS 0x04000

#تعریف_کرو KEIS_SO_CHAR_1 0x0A
#تعریف_کرو KEIS_SO_CHAR_2 0x42
#تعریف_کرو KEIS_SI_CHAR_1 0x0A
#تعریف_کرو KEIS_SI_CHAR_2 0x41

#تعریف_کرو JEF_SO_CHAR 0x28
#تعریف_کرو JEF_SI_CHAR 0x29

#تعریف_کرو JIPS_SO_CHAR_1 0x1A
#تعریف_کرو JIPS_SO_CHAR_2 0x70
#تعریف_کرو JIPS_SI_CHAR_1 0x1A
#تعریف_کرو JIPS_SI_CHAR_2 0x71

شمار SISO_Option {
    SI,
    SO
};
قسمتعریف شمار SISO_Option SISO_Option;

ساکن int32_t getSISOBytes(SISO_Option option, uint32_t cnvOption, uint8_t *value) {
    int32_t SISOLength = 0;

    انتخاب (option) {
        حالت SI:
            اگر ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SI_CHAR_1;
                value[1] = KEIS_SI_CHAR_2;
                SISOLength = 2;
            } ورنہ اگر ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SI_CHAR;
                SISOLength = 1;
            } ورنہ اگر ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SI_CHAR_1;
                value[1] = JIPS_SI_CHAR_2;
                SISOLength = 2;
            } ورنہ {
                value[0] = UCNV_SI;
                SISOLength = 1;
            }
            توڑو;
        حالت SO:
            اگر ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SO_CHAR_1;
                value[1] = KEIS_SO_CHAR_2;
                SISOLength = 2;
            } ورنہ اگر ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SO_CHAR;
                SISOLength = 1;
            } ورنہ اگر ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SO_CHAR_1;
                value[1] = JIPS_SO_CHAR_2;
                SISOLength = 2;
            } ورنہ {
                value[0] = UCNV_SO;
                SISOLength = 1;
            }
            توڑو;
        طےشدہ:
            /* Should never happen. */
            توڑو;
    }

    واپس SISOLength;
}

/* Miscellaneous ------------------------------------------------------------ */

/* similar to ucnv_MBCSGetNextUChar() but recursive */
ساکن UBool
enum_مقصد_U(UConverterMBCSTable *mbcs_جدول, int8_t stateProps[],
        int32_t state, uint32_t انحراف,
        uint32_t value,
        UConverter_Enum_مقصد_UCallback *callback, ثابت خالی *context,
        UError_رمز *p_Error_رمز) {
    UChar32 رمز_Points[32];
    ثابت int32_t *صف;
    ثابت uint16_t *unicode_رمز_Units;
    UChar32 any_رمز_Points;
    int32_t b, حد_بندی;

    صف=mbcs_جدول->state_جدول[state];
    unicode_رمز_Units=mbcs_جدول->unicode_رمز_Units;

    value<<=8;
    any_رمز_Points=-1;  /* becomes non-negative if there is a mapping */

    b=(stateProps[state]&0x38)<<2;
    اگر(b==0 && stateProps[state]>=0x40) {
        /* skip byte sequences with leading zeros because they are not stored in the fromUnicode table */
        رمز_Points[0]=U_SENTINEL;
        b=1;
    }
    حد_بندی=((stateProps[state]&7)+1)<<5;
    جب(b<حد_بندی) {
        int32_t اندراج_2=صف[b];
        اگر(MBCS_اندراج_IS_TRANSITION(اندراج_2)) {
            int32_t اگلا_State=MBCS_اندراج_TRANSITION_STATE(اندراج_2);
            اگر(stateProps[اگلا_State]>=0) {
                /* recurse to a state with non-ignorable actions */
                اگر(!enum_مقصد_U(
                        mbcs_جدول, stateProps, اگلا_State,
                        انحراف+MBCS_اندراج_TRANSITION_انحراف(اندراج_2),
                        value | static_cast<uint32_t>(b),
                        callback, context,
                        p_Error_رمز)) {
                    واپس جھوٹ;
                }
            }
            رمز_Points[b&0x1f]=U_SENTINEL;
        } ورنہ {
            UChar32 c;
            int32_t action;

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=MBCS_اندراج_FINAL_ACTION(اندراج_2);
            اگر(action==MBCS_STATE_VALID_براہ_راست_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            } ورنہ اگر(action==MBCS_STATE_VALID_16) {
                int32_t final_انحراف=انحراف+MBCS_اندراج_FINAL_قدر_16(اندراج_2);
                c=unicode_رمز_Units[final_انحراف];
                اگر(c<0xfffe) {
                    /* output BMP code point */
                } ورنہ {
                    c=U_SENTINEL;
                }
            } ورنہ اگر(action==MBCS_STATE_VALID_16_PAIR) {
                int32_t final_انحراف=انحراف+MBCS_اندراج_FINAL_قدر_16(اندراج_2);
                c=unicode_رمز_Units[final_انحراف++];
                اگر(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } ورنہ اگر(c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_رمز_Units[final_انحراف]+(0x10000-0xdc00);
                } ورنہ اگر(c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_رمز_Units[final_انحراف];
                } ورنہ {
                    c=U_SENTINEL;
                }
            } ورنہ اگر(action==MBCS_STATE_VALID_براہ_راست_20) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_اندراج_FINAL_قدر(اندراج_2) + 0x10000);
            } ورنہ {
                c=U_SENTINEL;
            }

            رمز_Points[b&0x1f]=c;
            any_رمز_Points&=c;
        }
        اگر(((++b)&0x1f)==0) {
            اگر(any_رمز_Points>=0) {
                اگر (!callback(context, value | static_cast<uint32_t>(b - 0x20), رمز_Points)) {
                    واپس جھوٹ;
                }
                any_رمز_Points=-1;
            }
        }
    }
    واپس سچ;
}

/*
 * Only called if stateProps[state]==-1.
 * A recursive call may do stateProps[state]|=0x40 if this state is the target of an
 * MBCS_STATE_CHANGE_ONLY.
 */
ساکن int8_t
getStateProp(ثابت int32_t (*state_جدول)[256], int8_t stateProps[], صحیح state) {
    ثابت int32_t *صف;
    int32_t min, max, اندراج_2, اگلا_State;

    صف=state_جدول[state];
    stateProps[state]=0;

    /* find first non-ignorable state */
    برائے(min=0;; ++min) {
        اندراج_2=صف[min];
        اگلا_State=MBCS_اندراج_STATE(اندراج_2);
        اگر(stateProps[اگلا_State]==-1) {
            getStateProp(state_جدول, stateProps, اگلا_State);
        }
        اگر(MBCS_اندراج_IS_TRANSITION(اندراج_2)) {
            اگر(stateProps[اگلا_State]>=0) {
                توڑو;
            }
        } ورنہ اگر(MBCS_اندراج_FINAL_ACTION(اندراج_2)<MBCS_STATE_UNASSIGNED) {
            توڑو;
        }
        اگر(min==0xff) {
            stateProps[state]=-0x40;  /* (int8_t)0xc0 */
            واپس stateProps[state];
        }
    }
    stateProps[state] |= static_cast<int8_t>((min >> 5) << 3);

    /* find last non-ignorable state */
    برائے(max=0xff; min<max; --max) {
        اندراج_2=صف[max];
        اگلا_State=MBCS_اندراج_STATE(اندراج_2);
        اگر(stateProps[اگلا_State]==-1) {
            getStateProp(state_جدول, stateProps, اگلا_State);
        }
        اگر(MBCS_اندراج_IS_TRANSITION(اندراج_2)) {
            اگر(stateProps[اگلا_State]>=0) {
                توڑو;
            }
        } ورنہ اگر(MBCS_اندراج_FINAL_ACTION(اندراج_2)<MBCS_STATE_UNASSIGNED) {
            توڑو;
        }
    }
    stateProps[state] |= static_cast<int8_t>(max >> 5);

    /* recurse further and collect direct-state information */
    جب(min<=max) {
        اندراج_2=صف[min];
        اگلا_State=MBCS_اندراج_STATE(اندراج_2);
        اگر(stateProps[اگلا_State]==-1) {
            getStateProp(state_جدول, stateProps, اگلا_State);
        }
        اگر(MBCS_اندراج_IS_FINAL(اندراج_2)) {
            stateProps[اگلا_State]|=0x40;
            اگر(MBCS_اندراج_FINAL_ACTION(اندراج_2)<=MBCS_STATE_FALLBACK_براہ_راست_20) {
                stateProps[state]|=0x40;
            }
        }
        ++min;
    }
    واپس stateProps[state];
}

/*
 * Internal function enumerating the toUnicode data of an MBCS converter.
 * Currently only used for reconstituting data for a MBCS_OPT_NO_FROM_U
 * table, but could also be used for a future ucnv_getUnicodeSet() option
 * that includes reverse fallbacks (after updating this function's implementation).
 * Currently only handles roundtrip mappings.
 * Does not currently handle extensions.
 */
ساکن خالی
ucnv_MBCSEnum_مقصد_Unicode(UConverterMBCSTable *mbcs_جدول,
                       UConverter_Enum_مقصد_UCallback *callback, ثابت خالی *context,
                       UError_رمز *p_Error_رمز) {
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
    int8_t stateProps[MBCS_MAX_STATE_گنتی];
    int32_t state;

    uprv_memset(stateProps, -1, حجم(stateProps));

    /* recurse from state 0 and set all stateProps */
    getStateProp(mbcs_جدول->state_جدول, stateProps, 0);

    برائے(state=0; state<mbcs_جدول->گنتی_حالتیں; ++state) {
        /*if(stateProps[state]==-1) {
            printf("unused/unreachable <icu:state> %d\n", state);
        }*/
        اگر(stateProps[state]>=0x40) {
            /* start from each direct state */
            enum_مقصد_U(
                mbcs_جدول, stateProps, state, 0, 0,
                callback, context,
                p_Error_رمز);
        }
    }
}

U_CFUNC خالی 
ucnv_MBCSGet_Filtered_Unicode_سیٹ_For_Unicode(ثابت UConverter_Shared_ڈیٹا *shared_ڈیٹا,
                                         ثابت USetAdder *sa,
                                         UConverter_Unicode_سیٹ which,
                                         UConverter_سیٹ_Filter filter,
                                         UError_رمز *p_Error_رمز) {
    ثابت UConverterMBCSTable *mbcs_جدول;
    ثابت uint16_t *جدول;

    uint32_t st3;
    uint16_t st1, maxStage1, st2;

    UChar32 c;

    /* enumerate the from-Unicode trie table */
    mbcs_جدول=&shared_ڈیٹا->mbcs;
    جدول=mbcs_جدول->from_Unicode_جدول;
    اگر(mbcs_جدول->unicode_نقاب&UCNV_HAS_SUPPLEMENTARY) {
        maxStage1=0x440;
    } ورنہ {
        maxStage1=0x40;
    }

    c=0; /* keep track of the current code point while enumerating */

    اگر(mbcs_جدول->outputType==MBCS_OUTPUT_1) {
        ثابت uint16_t *stage2, *stage3, *results;
        uint16_t min_قدر;

        results=(ثابت uint16_t *)mbcs_جدول->from_Unicode_بائٹیں;

        /*
         * Set a threshold variable for selecting which mappings to use.
         * See ucnv_MBCSSingleFromBMPWithOffsets() and
         * MBCS_SINGLE_RESULT_FROM_U() for details.
         */
        اگر(which==UCNV_ROUNDTRIP_سیٹ) {
            /* use only roundtrips */
            min_قدر=0xf00;
        } ورنہ /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
            /* use all roundtrip and fallback results */
            min_قدر=0x800;
        }

        برائے(st1=0; st1<maxStage1; ++st1) {
            st2=جدول[st1];
            اگر(st2>maxStage1) {
                stage2=جدول+st2;
                برائے(st2=0; st2<64; ++st2) {
                    اگر((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=results+st3;

                        کرو {
                            اگر(*stage3++>=min_قدر) {
                                sa->جوڑو(sa->سیٹ, c);
                            }
                        } جب((++c&0xf)!=0);
                    } ورنہ {
                        c+=16; /* empty stage 3 block */
                    }
                }
            } ورنہ {
                c+=1024; /* empty stage 2 block */
            }
        }
    } ورنہ {
        ثابت uint32_t *stage2;
        ثابت uint8_t *stage3, *بائٹیں;
        uint32_t st3Multiplier;
        uint32_t value;
        UBool useFallback;

        بائٹیں=mbcs_جدول->from_Unicode_بائٹیں;

        useFallback = which == UCNV_ROUNDTRIP_AND_FALLBACK_سیٹ;

        انتخاب(mbcs_جدول->outputType) {
        حالت MBCS_OUTPUT_3:
        حالت MBCS_OUTPUT_4_EUC:
            st3Multiplier=3;
            توڑو;
        حالت MBCS_OUTPUT_4:
            st3Multiplier=4;
            توڑو;
        طےشدہ:
            st3Multiplier=2;
            توڑو;
        }

        برائے(st1=0; st1<maxStage1; ++st1) {
            st2=جدول[st1];
            اگر(st2>(maxStage1>>1)) {
                stage2=(ثابت uint32_t *)جدول+st2;
                برائے(st2=0; st2<64; ++st2) {
                    اگر((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=بائٹیں+st3Multiplier*16*(uint32_t)(uint16_t)st3;

                        /* get the roundtrip flags for the stage 3 block */
                        st3>>=16;

                        /*
                         * Add code points for which the roundtrip flag is set,
                         * or which map to non-zero bytes if we use fallbacks.
                         * See ucnv_MBCSFromUnicodeWithOffsets() for details.
                         */
                        انتخاب(filter) {
                        حالت UCNV_سیٹ_FILTER_NONE:
                            کرو {
                                اگر(st3&1) {
                                    sa->جوڑو(sa->سیٹ, c);
                                    stage3+=st3Multiplier;
                                } ورنہ اگر(useFallback) {
                                    uint8_t b=0;
                                    انتخاب(st3Multiplier) {
                                    حالت 4:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    حالت 3:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    حالت 2:
                                        b|=stage3[0]|stage3[1];
                                        stage3+=2;
                                        U_FALLTHROUGH;
                                    طےشدہ:
                                        توڑو;
                                    }
                                    اگر(b!=0) {
                                        sa->جوڑو(sa->سیٹ, c);
                                    }
                                }
                                st3>>=1;
                            } جب((++c&0xf)!=0);
                            توڑو;
                        حالت UCNV_سیٹ_FILTER_DBCS_ONLY:
                             /* Ignore single-byte results (<0x100). */
                            کرو {
                                اگر(((st3&1)!=0 || useFallback) && *((ثابت uint16_t *)stage3)>=0x100) {
                                    sa->جوڑو(sa->سیٹ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } جب((++c&0xf)!=0);
                            توڑو;
                        حالت UCNV_سیٹ_FILTER_2022_CN:
                             /* Only add code points that map to CNS 11643 planes 1 & 2 for non-EXT ISO-2022-CN. */
                            کرو {
                                اگر(((st3&1)!=0 || useFallback) && ((value=*stage3)==0x81 || value==0x82)) {
                                    sa->جوڑو(sa->سیٹ, c);
                                }
                                st3>>=1;
                                stage3+=3;  /* +=st3Multiplier */
                            } جب((++c&0xf)!=0);
                            توڑو;
                        حالت UCNV_سیٹ_FILTER_SJIS:
                             /* Only add code points that map to Shift-JIS codes corresponding to JIS X 0208. */
                            کرو {
                                اگر(((st3&1)!=0 || useFallback) && (value=*((ثابت uint16_t *)stage3))>=0x8140 && value<=0xeffc) {
                                    sa->جوڑو(sa->سیٹ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } جب((++c&0xf)!=0);
                            توڑو;
                        حالت UCNV_سیٹ_FILTER_GR94_DBCS:
                            /* Only add code points that map to ISO 2022 GR 94 DBCS codes (each byte A1..FE). */
                            کرو {
                                اگر( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((ثابت uint16_t *)stage3)) - 0xa1a1)<=(0xfefe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->جوڑو(sa->سیٹ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } جب((++c&0xf)!=0);
                            توڑو;
                        حالت UCNV_سیٹ_FILTER_HZ:
                            /* Only add code points that are suitable for HZ DBCS (lead byte A1..FD). */
                            کرو {
                                اگر( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((ثابت uint16_t *)stage3))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->جوڑو(sa->سیٹ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } جب((++c&0xf)!=0);
                            توڑو;
                        طےشدہ:
                            *p_Error_رمز=U_اندرونی_پروگرام_ERROR;
                            واپس;
                        }
                    } ورنہ {
                        c+=16; /* empty stage 3 block */
                    }
                }
            } ورنہ {
                c+=1024; /* empty stage 2 block */
            }
        }
    }

    ucnv_ext_Get_Unicode_سیٹ(shared_ڈیٹا, sa, which, filter, p_Error_رمز);
}

U_CFUNC خالی
ucnv_MBCSGet_Unicode_سیٹ_For_Unicode(ثابت UConverter_Shared_ڈیٹا *shared_ڈیٹا,
                                 ثابت USetAdder *sa,
                                 UConverter_Unicode_سیٹ which,
                                 UError_رمز *p_Error_رمز) {
    ucnv_MBCSGet_Filtered_Unicode_سیٹ_For_Unicode(
        shared_ڈیٹا, sa, which,
        shared_ڈیٹا->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ?
            UCNV_سیٹ_FILTER_DBCS_ONLY :
            UCNV_سیٹ_FILTER_NONE,
        p_Error_رمز);
}

ساکن خالی U_CALLCONV
ucnv_MBCSGet_Unicode_سیٹ(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverter_Unicode_سیٹ which,
                   UError_رمز *p_Error_رمز) {
    اگر(cnv->options&_MBCS_OPTION_GB18030) {
        sa->جوڑو_Range(sa->سیٹ, 0, 0xd7ff);
        sa->جوڑو_Range(sa->سیٹ, 0xe000, 0x10ffff);
    } ورنہ {
        ucnv_MBCSGet_Unicode_سیٹ_For_Unicode(cnv->shared_ڈیٹا, sa, which, p_Error_رمز);
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
ساکن UChar32
_extFromU(UConverter *cnv, ثابت UConverter_Shared_ڈیٹا *shared_ڈیٹا,
          UChar32 cp,
          ثابت char16_t **ماخذ, ثابت char16_t *ماخذ_حد_بندی,
          uint8_t **ہدف, ثابت uint8_t *ہدف_حد_بندی,
          int32_t **offsets, int32_t ماخذ_اشاریہ,
          UBool flush,
          UError_رمز *p_Error_رمز) {
    ثابت int32_t *cx;

    cnv->useSubChar1=جھوٹ;

    اگر( (cx=shared_ڈیٹا->mbcs.extIndexes)!=خالی_اشاریہ &&
        ucnv_ext_Initial_مطابقت_From_U(
            cnv, cx,
            cp, ماخذ, ماخذ_حد_بندی,
            reinterpret_cast<حرف**>(ہدف), reinterpret_cast<ثابت حرف*>(ہدف_حد_بندی),
            offsets, ماخذ_اشاریہ,
            flush,
            p_Error_رمز)
    ) {
        واپس 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    اگر((cnv->options&_MBCS_OPTION_GB18030)!=0) {
        ثابت uint32_t *range;
        int32_t i;

        range=gb18030Ranges[0];
        برائے(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            اگر (range[0] <= static_cast<uint32_t>(cp) && static_cast<uint32_t>(cp) <= range[1]) {
                /* found the Unicode code point, output the four-byte sequence for it */
                uint32_t خطی_2;
                حرف بائٹیں[4];

                /* get the linear value of the first GB 18030 code in this range */
                خطی_2=range[2]-خطی_18030_بنیاد;

                /* add the offset from the beginning of the range */
                خطی_2 += (static_cast<uint32_t>(cp) - range[0]);

                /* turn this into a four-byte sequence */
                بائٹیں[3] = static_cast<حرف>(0x30 + خطی_2 % 10); خطی_2 /= 10;
                بائٹیں[2] = static_cast<حرف>(0x81 + خطی_2 % 126); خطی_2 /= 126;
                بائٹیں[1] = static_cast<حرف>(0x30 + خطی_2 % 10); خطی_2 /= 10;
                بائٹیں[0] = static_cast<حرف>(0x81 + خطی_2);

                /* output this sequence */
                ucnv_from_UWrite_بائٹیں(cnv,
                                     بائٹیں, 4, reinterpret_cast<حرف**>(ہدف), reinterpret_cast<ثابت حرف*>(ہدف_حد_بندی),
                                     offsets, ماخذ_اشاریہ, p_Error_رمز);
                واپس 0;
            }
        }
    }

    /* no mapping */
    *p_Error_رمز=U_INVALID_CHAR_ملا;
    واپس cp;
}

/*
 * Input sequence: cnv->toUBytes[0..length[
 * @return if(U_FAILURE) return the length (toULength, byteIndex) for the input
 *         else return 0 after output has been written to the target
 */
ساکن int8_t
ext_مقصد_U(UConverter *cnv, ثابت UConverter_Shared_ڈیٹا *shared_ڈیٹا,
        int8_t length,
        ثابت uint8_t **ماخذ, ثابت uint8_t *ماخذ_حد_بندی,
        char16_t **ہدف, ثابت char16_t *ہدف_حد_بندی,
        int32_t **offsets, int32_t ماخذ_اشاریہ,
        UBool flush,
        UError_رمز *p_Error_رمز) {
    ثابت int32_t *cx;

    اگر( (cx=shared_ڈیٹا->mbcs.extIndexes)!=خالی_اشاریہ &&
        ucnv_ext_Initial_مطابقت_مقصد_U(
            cnv, cx,
            length, reinterpret_cast<ثابت حرف**>(ماخذ), reinterpret_cast<ثابت حرف*>(ماخذ_حد_بندی),
            ہدف, ہدف_حد_بندی,
            offsets, ماخذ_اشاریہ,
            flush,
            p_Error_رمز)
    ) {
        واپس 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    اگر(length==4 && (cnv->options&_MBCS_OPTION_GB18030)!=0) {
        ثابت uint32_t *range;
        uint32_t خطی_2;
        int32_t i;

        خطی_2=خطی_18030(cnv->مقصد_UBytes[0], cnv->مقصد_UBytes[1], cnv->مقصد_UBytes[2], cnv->مقصد_UBytes[3]);
        range=gb18030Ranges[0];
        برائے(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            اگر(range[2]<=خطی_2 && خطی_2<=range[3]) {
                /* found the sequence, output the Unicode code point for it */
                *p_Error_رمز=U_ZERO_ERROR;

                /* add the linear difference between the input and start sequences to the start code point */
                خطی_2=range[0]+(خطی_2-range[2]);

                /* output this code point */
                ucnv_مقصد_UWrite_رمز_نقطہ(cnv, خطی_2, ہدف, ہدف_حد_بندی, offsets, ماخذ_اشاریہ, p_Error_رمز);

                واپس 0;
            }
        }
    }

    /* no mapping */
    *p_Error_رمز=U_INVALID_CHAR_ملا;
    واپس length;
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
#تعریف_کرو EBCDIC_LF 0x25
#تعریف_کرو EBCDIC_NL 0x15

/* standard EBCDIC codes with roundtrip flag as stored in Unicode-to-single-byte tables */
#تعریف_کرو EBCDIC_RT_LF 0xf25
#تعریف_کرو EBCDIC_RT_NL 0xf15

/* Unicode code points */
#تعریف_کرو U_LF 0x0a
#تعریف_کرو U_NL 0x85

ساکن UBool
_EBCDICSwapLFNL(UConverter_Shared_ڈیٹا *shared_ڈیٹا, UError_رمز *p_Error_رمز) {
    UConverterMBCSTable *mbcs_جدول;

    ثابت uint16_t *جدول, *results;
    ثابت uint8_t *بائٹیں;

    int32_t (*new_State_جدول)[256];
    uint16_t *newResults;
    uint8_t *p;
    حرف *name;

    uint32_t stage2_اندراج;
    uint32_t size, sizeofFromUBytes;

    mbcs_جدول=&shared_ڈیٹا->mbcs;

    جدول=mbcs_جدول->from_Unicode_جدول;
    بائٹیں=mbcs_جدول->from_Unicode_بائٹیں;
    results = reinterpret_cast<ثابت uint16_t*>(بائٹیں);

    /*
     * Check that this is an EBCDIC table with SBCS portion -
     * SBCS or EBCDIC_STATEFUL with standard EBCDIC LF and NL mappings.
     *
     * If not, ignore the option. Options are always ignored if they do not apply.
     */
    اگر(!(
         (mbcs_جدول->outputType==MBCS_OUTPUT_1 || mbcs_جدول->outputType==MBCS_OUTPUT_2_SISO) &&
         mbcs_جدول->state_جدول[0][EBCDIC_LF]==MBCS_اندراج_FINAL(0, MBCS_STATE_VALID_براہ_راست_16, U_LF) &&
         mbcs_جدول->state_جدول[0][EBCDIC_NL]==MBCS_اندراج_FINAL(0, MBCS_STATE_VALID_براہ_راست_16, U_NL)
    )) {
        واپس جھوٹ;
    }

    اگر(mbcs_جدول->outputType==MBCS_OUTPUT_1) {
        اگر(!(
             EBCDIC_RT_LF==MBCS_SINGLE_نتیجہ_FROM_U(جدول, results, U_LF) &&
             EBCDIC_RT_NL==MBCS_SINGLE_نتیجہ_FROM_U(جدول, results, U_NL)
        )) {
            واپس جھوٹ;
        }
    } ورنہ /* MBCS_OUTPUT_2_SISO */ {
        stage2_اندراج=MBCS_STAGE_2_FROM_U(جدول, U_LF);
        اگر(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_اندراج, U_LF)!=0 &&
             EBCDIC_LF==MBCS_قدر_2_FROM_STAGE_2(بائٹیں, stage2_اندراج, U_LF)
        )) {
            واپس جھوٹ;
        }

        stage2_اندراج=MBCS_STAGE_2_FROM_U(جدول, U_NL);
        اگر(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_اندراج, U_NL)!=0 &&
             EBCDIC_NL==MBCS_قدر_2_FROM_STAGE_2(بائٹیں, stage2_اندراج, U_NL)
        )) {
            واپس جھوٹ;
        }
    }

    اگر(mbcs_جدول->from_UBytes_لمبائی>0) {
        /*
         * We _know_ the number of bytes in the fromUnicodeBytes array
         * starting with header.version 4.1.
         */
        sizeofFromUBytes=mbcs_جدول->from_UBytes_لمبائی;
    } ورنہ {
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
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس جھوٹ;
    }

    /*
     * The table has an appropriate format.
     * Allocate and build
     * - a modified to-Unicode state table
     * - a modified from-Unicode output array
     * - a converter name string with the swap option appended
     */
    size=
        mbcs_جدول->گنتی_حالتیں*1024+
        sizeofFromUBytes+
        UCNV_MAX_CONVERTER_NAME_لمبائی+20;
    p = static_cast<uint8_t*>(uprv_malloc(size));
    اگر(p==خالی_اشاریہ) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس جھوٹ;
    }

    /* copy and modify the to-Unicode state table */
    new_State_جدول = reinterpret_cast<int32_t(*)[256]>(p);
    uprv_memcpy(new_State_جدول, mbcs_جدول->state_جدول, mbcs_جدول->گنتی_حالتیں*1024);

    new_State_جدول[0][EBCDIC_LF]=MBCS_اندراج_FINAL(0, MBCS_STATE_VALID_براہ_راست_16, U_NL);
    new_State_جدول[0][EBCDIC_NL]=MBCS_اندراج_FINAL(0, MBCS_STATE_VALID_براہ_راست_16, U_LF);

    /* copy and modify the from-Unicode result table */
    newResults = reinterpret_cast<uint16_t*>(new_State_جدول[mbcs_جدول->گنتی_حالتیں]);
    uprv_memcpy(newResults, بائٹیں, sizeofFromUBytes);

    /* conveniently, the table access macros work on the left side of expressions */
    اگر(mbcs_جدول->outputType==MBCS_OUTPUT_1) {
        MBCS_SINGLE_نتیجہ_FROM_U(جدول, newResults, U_LF)=EBCDIC_RT_NL;
        MBCS_SINGLE_نتیجہ_FROM_U(جدول, newResults, U_NL)=EBCDIC_RT_LF;
    } ورنہ /* MBCS_OUTPUT_2_SISO */ {
        stage2_اندراج=MBCS_STAGE_2_FROM_U(جدول, U_LF);
        MBCS_قدر_2_FROM_STAGE_2(newResults, stage2_اندراج, U_LF)=EBCDIC_NL;

        stage2_اندراج=MBCS_STAGE_2_FROM_U(جدول, U_NL);
        MBCS_قدر_2_FROM_STAGE_2(newResults, stage2_اندراج, U_NL)=EBCDIC_LF;
    }

    /* set the canonical converter name */
    name = reinterpret_cast<حرف*>(newResults) + sizeofFromUBytes;
    uprv_strcpy(name, shared_ڈیٹا->static_ڈیٹا->name);
    uprv_strcat(name, UCNV_تبادلہ_LFNL_OPTION_STRING);

    /* set the pointers */
    icu::umtx_تالا(خالی_اشاریہ);
    اگر(mbcs_جدول->تبادلہ_LFNLState_جدول==خالی_اشاریہ) {
        mbcs_جدول->تبادلہ_LFNLState_جدول=new_State_جدول;
        mbcs_جدول->تبادلہ_LFNLFrom_Unicode_بائٹیں = reinterpret_cast<uint8_t*>(newResults);
        mbcs_جدول->تبادلہ_LFNLName=name;

        new_State_جدول=خالی_اشاریہ;
    }
    icu::umtx_unlock(خالی_اشاریہ);

    /* release the allocated memory if another thread beat us to it */
    اگر(new_State_جدول!=خالی_اشاریہ) {
        uprv_free(new_State_جدول);
    }
    واپس سچ;
}

/* reconstitute omitted fromUnicode data ------------------------------------ */

/* for details, compare with genmbcs.c MBCSAddFromUnicode() and transformEUC() */
ساکن UBool U_CALLCONV
writeStage3Roundtrip(ثابت خالی *context, uint32_t value, UChar32 رمز_Points[32]) {
    UConverterMBCSTable *mbcs_جدول=(UConverterMBCSTable *)context;
    ثابت uint16_t *جدول;
    uint32_t *stage2;
    uint8_t *بائٹیں, *p;
    UChar32 c;
    int32_t i, st3;

    جدول=mbcs_جدول->from_Unicode_جدول;
    بائٹیں = const_cast<uint8_t*>(mbcs_جدول->from_Unicode_بائٹیں);

    /* for EUC outputTypes, modify the value like genmbcs.c's transformEUC() */
    انتخاب(mbcs_جدول->outputType) {
    حالت MBCS_OUTPUT_3_EUC:
        اگر(value<=0xffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } ورنہ اگر(value<=0x8effff) {
            /* code set 2 */
            value&=0x7fff;
        } ورنہ /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7f;
        }
        توڑو;
    حالت MBCS_OUTPUT_4_EUC:
        اگر(value<=0xffffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } ورنہ اگر(value<=0x8effffff) {
            /* code set 2 */
            value&=0x7fffff;
        } ورنہ /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7fff;
        }
        توڑو;
    طےشدہ:
        توڑو;
    }

    برائے(i=0; i<=0x1f; ++value, ++i) {
        c=رمز_Points[i];
        اگر(c<0) {
            جاری;
        }

        /* locate the stage 2 & 3 data */
        stage2=((uint32_t *)جدول)+جدول[c>>10]+((c>>4)&0x3f);
        p=بائٹیں;
        st3 = static_cast<int32_t>(static_cast<uint16_t>(*stage2)) * 16 + (c & 0xf);

        /* write the codepage bytes into stage 3 */
        انتخاب(mbcs_جدول->outputType) {
        حالت MBCS_OUTPUT_3:
        حالت MBCS_OUTPUT_4_EUC:
            p+=st3*3;
            p[0] = static_cast<uint8_t>(value >> 16);
            p[1] = static_cast<uint8_t>(value >> 8);
            p[2] = static_cast<uint8_t>(value);
            توڑو;
        حالت MBCS_OUTPUT_4:
            reinterpret_cast<uint32_t*>(p)[st3] = value;
            توڑو;
        طےشدہ:
            /* 2 bytes per character */
            reinterpret_cast<uint16_t*>(p)[st3] = static_cast<uint16_t>(value);
            توڑو;
        }

        /* set the roundtrip flag */
        *stage2|=(1UL<<(16+(c&0xf)));
    }
    واپس سچ;
 }

ساکن خالی
reconstitute_ڈیٹا(UConverterMBCSTable *mbcs_جدول,
                 uint32_t stage1_لمبائی, uint32_t stage2_لمبائی,
                 uint32_t بھرا_Stage2_لمبائی,  /* lengths are numbers of units, not bytes */
                 UError_رمز *p_Error_رمز) {
    uint16_t *stage1;
    uint32_t *stage2;
    uint32_t ڈیٹا_لمبائی=stage1_لمبائی*2+بھرا_Stage2_لمبائی*4+mbcs_جدول->from_UBytes_لمبائی;
    mbcs_جدول->reconstituted_ڈیٹا = static_cast<uint8_t*>(uprv_malloc(ڈیٹا_لمبائی));
    اگر(mbcs_جدول->reconstituted_ڈیٹا==خالی_اشاریہ) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    uprv_memset(mbcs_جدول->reconstituted_ڈیٹا, 0, ڈیٹا_لمبائی);

    /* copy existing data and reroute the pointers */
    stage1 = reinterpret_cast<uint16_t*>(mbcs_جدول->reconstituted_ڈیٹا);
    uprv_memcpy(stage1, mbcs_جدول->from_Unicode_جدول, stage1_لمبائی*2);

    stage2 = reinterpret_cast<uint32_t*>(stage1 + stage1_لمبائی);
    uprv_memcpy(stage2+(بھرا_Stage2_لمبائی-stage2_لمبائی),
                mbcs_جدول->from_Unicode_جدول+stage1_لمبائی,
                stage2_لمبائی*4);

    mbcs_جدول->from_Unicode_جدول=stage1;
    mbcs_جدول->from_Unicode_بائٹیں = reinterpret_cast<uint8_t*>(stage2 + بھرا_Stage2_لمبائی);

    /* indexes into stage 2 count from the bottom of the fromUnicodeTable */
    stage2 = reinterpret_cast<uint32_t*>(stage1);

    /* reconstitute the initial part of stage 2 from the mbcsIndex */
    {
        int32_t stage_UTF8_لمبائی = (static_cast<int32_t>(mbcs_جدول->maxFastUChar) + 1) >> 6;
        int32_t stage_UTF8_اشاریہ=0;
        int32_t st1, st2, st3, i;

        برائے(st1=0; stage_UTF8_اشاریہ<stage_UTF8_لمبائی; ++st1) {
            st2=stage1[st1];
            اگر (st2 != static_cast<int32_t>(stage1_لمبائی) / 2) {
                /* each stage 2 block has 64 entries corresponding to 16 entries in the mbcsIndex */
                برائے(i=0; i<16; ++i) {
                    st3=mbcs_جدول->mbcs_اشاریہ[stage_UTF8_اشاریہ++];
                    اگر(st3!=0) {
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
                    } ورنہ {
                        /* no stage 3 block, skip */
                        st2+=4;
                    }
                }
            } ورنہ {
                /* no stage 2 block, skip */
                stage_UTF8_اشاریہ+=16;
            }
        }
    }

    /* reconstitute fromUnicodeBytes with roundtrips from toUnicode data */
    ucnv_MBCSEnum_مقصد_Unicode(mbcs_جدول, writeStage3Roundtrip, mbcs_جدول, p_Error_رمز);
}

/* MBCS setup functions ----------------------------------------------------- */

ساکن خالی U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_ڈیٹا *shared_ڈیٹا,
          UConverter_بھرو_دلائل *p_دلائل,
          ثابت uint8_t *raw,
          UError_رمز *p_Error_رمز) {
    UDataInfo info;
    UConverterMBCSTable *mbcs_جدول=&shared_ڈیٹا->mbcs;
    _MBCSHeader *سرخی=(_MBCSHeader *)raw;
    uint32_t انحراف;
    uint32_t سرخی_لمبائی;
    UBool noFromU=جھوٹ;

    اگر(سرخی->نسخہ[0]==4) {
        سرخی_لمبائی=MBCS_سرخی_V4_لمبائی;
    } ورنہ اگر(سرخی->نسخہ[0]==5 && سرخی->نسخہ[1]>=3 &&
              (سرخی->options&MBCS_OPT_UNKNOWN_INCOMPATIBLE_نقاب)==0) {
        سرخی_لمبائی=سرخی->options&MBCS_OPT_لمبائی_نقاب;
        noFromU = static_cast<UBool>((سرخی->options & MBCS_OPT_NO_FROM_U) != 0);
    } ورنہ {
        *p_Error_رمز=U_INVALID_جدول_FORMAT;
        واپس;
    }

    mbcs_جدول->outputType = static_cast<uint8_t>(سرخی->flags);
    اگر(noFromU && mbcs_جدول->outputType==MBCS_OUTPUT_1) {
        *p_Error_رمز=U_INVALID_جدول_FORMAT;
        واپس;
    }

    /* extension data, header version 4.2 and higher */
    انحراف=سرخی->flags>>8;
    اگر(انحراف!=0) {
        mbcs_جدول->extIndexes = reinterpret_cast<ثابت int32_t*>(raw + انحراف);
    }

    اگر(mbcs_جدول->outputType==MBCS_OUTPUT_EXT_ONLY) {
        UConverter_بھرو_دلائل دلائل_2=UCNV_بھرو_دلائل_INITIALIZER;
        UConverter_Shared_ڈیٹا *بنیاد_Shared_ڈیٹا;
        ثابت int32_t *extIndexes;
        ثابت حرف *بنیاد_Name;

        /* extension-only file, load the base table and set values appropriately */
        اگر((extIndexes=mbcs_جدول->extIndexes)==خالی_اشاریہ) {
            /* extension-only file without extension */
            *p_Error_رمز=U_INVALID_جدول_FORMAT;
            واپس;
        }

        اگر(p_دلائل->nestedLoads!=1) {
            /* an extension table must not be loaded as a base table */
            *p_Error_رمز=U_INVALID_جدول_FILE;
            واپس;
        }

        /* load the base table */
        بنیاد_Name = reinterpret_cast<ثابت حرف*>(سرخی) + سرخی_لمبائی * 4;
        اگر(0==uprv_strcmp(بنیاد_Name, shared_ڈیٹا->static_ڈیٹا->name)) {
            /* forbid loading this same extension-only file */
            *p_Error_رمز=U_INVALID_جدول_FORMAT;
            واپس;
        }

        /* TODO parse package name out of the prefix of the base name in the extension .cnv file? */
        دلائل_2.size=حجم(UConverter_بھرو_دلائل);
        دلائل_2.nestedLoads=2;
        دلائل_2.onlyTestIsLoadable=p_دلائل->onlyTestIsLoadable;
        دلائل_2.reserved=p_دلائل->reserved;
        دلائل_2.options=p_دلائل->options;
        دلائل_2.pkg=p_دلائل->pkg;
        دلائل_2.name=بنیاد_Name;
        بنیاد_Shared_ڈیٹا=ucnv_بھرو(&دلائل_2, p_Error_رمز);
        اگر(U_FAILURE(*p_Error_رمز)) {
            واپس;
        }
        اگر( بنیاد_Shared_ڈیٹا->static_ڈیٹا->conversionType!=UCNV_MBCS ||
            بنیاد_Shared_ڈیٹا->mbcs.بنیاد_Shared_ڈیٹا!=خالی_اشاریہ
        ) {
            ucnv_unload(بنیاد_Shared_ڈیٹا);
            *p_Error_رمز=U_INVALID_جدول_FORMAT;
            واپس;
        }
        اگر(p_دلائل->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            ucnv_unload(بنیاد_Shared_ڈیٹا);
            واپس;
        }

        /* copy the base table data */
        uprv_memcpy(mbcs_جدول, &بنیاد_Shared_ڈیٹا->mbcs, حجم(UConverterMBCSTable));

        /* overwrite values with relevant ones for the extension converter */
        mbcs_جدول->بنیاد_Shared_ڈیٹا=بنیاد_Shared_ڈیٹا;
        mbcs_جدول->extIndexes=extIndexes;

        /*
         * It would be possible to share the swapLFNL data with a base converter,
         * but the generated name would have to be different, and the memory
         * would have to be free'd only once.
         * It is easier to just create the data for the extension converter
         * separately when it is requested.
         */
        mbcs_جدول->تبادلہ_LFNLState_جدول=خالی_اشاریہ;
        mbcs_جدول->تبادلہ_LFNLFrom_Unicode_بائٹیں=خالی_اشاریہ;
        mbcs_جدول->تبادلہ_LFNLName=خالی_اشاریہ;

        /*
         * The reconstitutedData must be deleted only when the base converter
         * is unloaded.
         */
        mbcs_جدول->reconstituted_ڈیٹا=خالی_اشاریہ;

        /*
         * Set a special, runtime-only outputType if the extension converter
         * is a DBCS version of a base converter that also maps single bytes.
         */
        اگر( shared_ڈیٹا->static_ڈیٹا->conversionType==UCNV_DBCS ||
                (shared_ڈیٹا->static_ڈیٹا->conversionType==UCNV_MBCS &&
                 shared_ڈیٹا->static_ڈیٹا->min_بائٹیں_فی_Char>=2)
        ) {
            اگر(بنیاد_Shared_ڈیٹا->mbcs.outputType==MBCS_OUTPUT_2_SISO) {
                /* the base converter is SI/SO-stateful */
                int32_t اندراج_2;

                /* get the dbcs state from the state table entry for SO=0x0e */
                اندراج_2=mbcs_جدول->state_جدول[0][0xe];
                اگر( MBCS_اندراج_IS_FINAL(اندراج_2) &&
                    MBCS_اندراج_FINAL_ACTION(اندراج_2)==MBCS_STATE_CHANGE_ONLY &&
                    MBCS_اندراج_FINAL_STATE(اندراج_2)!=0
                ) {
                    mbcs_جدول->dbcsOnlyState = static_cast<uint8_t>(MBCS_اندراج_FINAL_STATE(اندراج_2));

                    mbcs_جدول->outputType=MBCS_OUTPUT_DBCS_ONLY;
                }
            } ورنہ اگر(
                بنیاد_Shared_ڈیٹا->static_ڈیٹا->conversionType==UCNV_MBCS &&
                بنیاد_Shared_ڈیٹا->static_ڈیٹا->min_بائٹیں_فی_Char==1 &&
                بنیاد_Shared_ڈیٹا->static_ڈیٹا->max_بائٹیں_فی_Char==2 &&
                mbcs_جدول->گنتی_حالتیں<=127
            ) {
                /* non-stateful base converter, need to modify the state table */
                int32_t (*new_State_جدول)[256];
                int32_t *state;
                int32_t i, count;

                /* allocate a new state table and copy the base state table contents */
                count=mbcs_جدول->گنتی_حالتیں;
                new_State_جدول = static_cast<int32_t(*)[256]>(uprv_malloc((count + 1) * 1024));
                اگر(new_State_جدول==خالی_اشاریہ) {
                    ucnv_unload(بنیاد_Shared_ڈیٹا);
                    *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
                    واپس;
                }

                uprv_memcpy(new_State_جدول, mbcs_جدول->state_جدول, count*1024);

                /* change all final single-byte entries to go to a new all-illegal state */
                state=new_State_جدول[0];
                برائے(i=0; i<256; ++i) {
                    اگر(MBCS_اندراج_IS_FINAL(state[i])) {
                        state[i]=MBCS_اندراج_TRANSITION(count, 0);
                    }
                }

                /* build the new all-illegal state */
                state=new_State_جدول[count];
                برائے(i=0; i<256; ++i) {
                    state[i]=MBCS_اندراج_FINAL(0, MBCS_STATE_ILLEGAL, 0);
                }
                mbcs_جدول->state_جدول=(ثابت int32_t (*)[256])new_State_جدول;
                mbcs_جدول->گنتی_حالتیں = static_cast<uint8_t>(count + 1);
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
    } ورنہ {
        /* conversion file with a base table; an additional extension table is optional */
        /* make sure that the output type is known */
        انتخاب(mbcs_جدول->outputType) {
        حالت MBCS_OUTPUT_1:
        حالت MBCS_OUTPUT_2:
        حالت MBCS_OUTPUT_3:
        حالت MBCS_OUTPUT_4:
        حالت MBCS_OUTPUT_3_EUC:
        حالت MBCS_OUTPUT_4_EUC:
        حالت MBCS_OUTPUT_2_SISO:
            /* OK */
            توڑو;
        طےشدہ:
            *p_Error_رمز=U_INVALID_جدول_FORMAT;
            واپس;
        }
        اگر(p_دلائل->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            واپس;
        }

        mbcs_جدول->گنتی_حالتیں = static_cast<uint8_t>(سرخی->گنتی_حالتیں);
        mbcs_جدول->گنتی_مقصد_UFallbacks=سرخی->گنتی_مقصد_UFallbacks;
        mbcs_جدول->state_جدول = reinterpret_cast<ثابت int32_t(*)[256]>(raw + سرخی_لمبائی * 4);
        mbcs_جدول->مقصد_UFallbacks = reinterpret_cast<ثابت _MBCSToUFallback*>(mbcs_جدول->state_جدول + سرخی->گنتی_حالتیں);
        mbcs_جدول->unicode_رمز_Units = reinterpret_cast<ثابت uint16_t*>(raw + سرخی->انحراف_مقصد_UCode_Units);

        mbcs_جدول->from_Unicode_جدول = reinterpret_cast<ثابت uint16_t*>(raw + سرخی->انحراف_From_UTable);
        mbcs_جدول->from_Unicode_بائٹیں = raw + سرخی->انحراف_From_UBytes;
        mbcs_جدول->from_UBytes_لمبائی=سرخی->from_UBytes_لمبائی;

        /*
         * converter versions 6.1 and up contain a unicodeMask that is
         * used here to select the most efficient function implementations
         */
        info.size=حجم(UDataInfo);
        udata_getInfo((UData_حافظہ *)shared_ڈیٹا->ڈیٹا_حافظہ, &info);
        اگر(info.format_نسخہ[0]>6 || (info.format_نسخہ[0]==6 && info.format_نسخہ[1]>=1)) {
            /* mask off possible future extensions to be safe */
            mbcs_جدول->unicode_نقاب = static_cast<uint8_t>(shared_ڈیٹا->static_ڈیٹا->unicode_نقاب & 3);
        } ورنہ {
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
        اگر( سرخی->نسخہ[1]>=3 &&
            (mbcs_جدول->unicode_نقاب&UCNV_HAS_SURROGATES)==0 &&
            (mbcs_جدول->گنتی_حالتیں==1 ?
                (سرخی->نسخہ[2]>=(SBCS_FAST_MAX>>8)) :
                (سرخی->نسخہ[2]>=(MBCS_FAST_MAX>>8))
            )
        ) {
            mbcs_جدول->utf8Friendly=سچ;

            اگر(mbcs_جدول->گنتی_حالتیں==1) {
                /*
                 * SBCS: Stage 3 is allocated in 64-entry blocks for U+0000..SBCS_FAST_MAX or higher.
                 * Build a table with indexes to each block, to be used instead of
                 * the regular stage 1/2 table.
                 */
                int32_t i;
                برائے(i=0; i<(SBCS_FAST_حد_بندی>>6); ++i) {
                    mbcs_جدول->sbcs_اشاریہ[i]=mbcs_جدول->from_Unicode_جدول[mbcs_جدول->from_Unicode_جدول[i>>4]+((i<<2)&0x3c)];
                }
                /* set SBCS_FAST_MAX to reflect the reach of sbcsIndex[] even if header->version[2]>(SBCS_FAST_MAX>>8) */
                mbcs_جدول->maxFastUChar=SBCS_FAST_MAX;
            } ورنہ {
                /*
                 * MBCS: Stage 3 is allocated in 64-entry blocks for U+0000..MBCS_FAST_MAX or higher.
                 * The .cnv file is prebuilt with an additional stage table with indexes
                 * to each block.
                 */
                mbcs_جدول->mbcs_اشاریہ = reinterpret_cast<ثابت uint16_t*>(
                    mbcs_جدول->from_Unicode_بائٹیں +
                     (noFromU ? 0 : mbcs_جدول->from_UBytes_لمبائی));
                mbcs_جدول->maxFastUChar = (static_cast<char16_t>(سرخی->نسخہ[2]) << 8) | 0xff;
            }
        }

        /* calculate a bit set of 4 ASCII characters per bit that round-trip to ASCII bytes */
        {
            uint32_t asciiRoundtrips=0xffffffff;
            int32_t i;

            برائے(i=0; i<0x80; ++i) {
                اگر(mbcs_جدول->state_جدول[0][i]!=MBCS_اندراج_FINAL(0, MBCS_STATE_VALID_براہ_راست_16, i)) {
                    asciiRoundtrips &= ~(static_cast<uint32_t>(1) << (i >> 2));
                }
            }
            mbcs_جدول->asciiRoundtrips=asciiRoundtrips;
        }

        اگر(noFromU) {
            uint32_t stage1_لمبائی=
                mbcs_جدول->unicode_نقاب&UCNV_HAS_SUPPLEMENTARY ?
                    0x440 : 0x40;
            uint32_t stage2_لمبائی=
                (سرخی->انحراف_From_UBytes-سرخی->انحراف_From_UTable)/4-
                stage1_لمبائی/2;
            reconstitute_ڈیٹا(mbcs_جدول, stage1_لمبائی, stage2_لمبائی, سرخی->بھرا_Stage2_لمبائی, p_Error_رمز);
        }
    }

    /* Set the impl pointer here so that it is set for both extension-only and base tables. */
    اگر(mbcs_جدول->utf8Friendly) {
        اگر(mbcs_جدول->گنتی_حالتیں==1) {
            shared_ڈیٹا->impl=&_SBCSUTF8Impl;
        } ورنہ {
            اگر(mbcs_جدول->outputType==MBCS_OUTPUT_2) {
                shared_ڈیٹا->impl=&_DBCSUTF8Impl;
            }
        }
    }

    اگر(mbcs_جدول->outputType==MBCS_OUTPUT_DBCS_ONLY || mbcs_جدول->outputType==MBCS_OUTPUT_2_SISO) {
        /*
         * MBCS_OUTPUT_DBCS_ONLY: No SBCS mappings, therefore ASCII does not roundtrip.
         * MBCS_OUTPUT_2_SISO: Bypass the ASCII fastpath to handle prevLength correctly.
         */
        mbcs_جدول->asciiRoundtrips=0;
    }
}

ساکن خالی U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_ڈیٹا *shared_ڈیٹا) {
    UConverterMBCSTable *mbcs_جدول=&shared_ڈیٹا->mbcs;

    اگر(mbcs_جدول->تبادلہ_LFNLState_جدول!=خالی_اشاریہ) {
        uprv_free(mbcs_جدول->تبادلہ_LFNLState_جدول);
    }
    اگر(mbcs_جدول->state_جدول_Owned) {
        uprv_free((خالی *)mbcs_جدول->state_جدول);
    }
    اگر(mbcs_جدول->بنیاد_Shared_ڈیٹا!=خالی_اشاریہ) {
        ucnv_unload(mbcs_جدول->بنیاد_Shared_ڈیٹا);
    }
    اگر(mbcs_جدول->reconstituted_ڈیٹا!=خالی_اشاریہ) {
        uprv_free(mbcs_جدول->reconstituted_ڈیٹا);
    }
}

ساکن خالی U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_بھرو_دلائل *p_دلائل,
              UError_رمز *p_Error_رمز) {
    UConverterMBCSTable *mbcs_جدول;
    ثابت int32_t *extIndexes;
    uint8_t outputType;
    int8_t max_بائٹیں_فی_UChar;

    اگر(p_دلائل->onlyTestIsLoadable) {
        واپس;
    }

    mbcs_جدول=&cnv->shared_ڈیٹا->mbcs;
    outputType=mbcs_جدول->outputType;

    اگر(outputType==MBCS_OUTPUT_DBCS_ONLY) {
        /* the swaplfnl option does not apply, remove it */
        cnv->options=p_دلائل->options&=~UCNV_OPTION_تبادلہ_LFNL;
    }

    اگر((p_دلائل->options&UCNV_OPTION_تبادلہ_LFNL)!=0) {
        /* do this because double-checked locking is broken */
        UBool isCached;

        icu::umtx_تالا(خالی_اشاریہ);
        isCached=mbcs_جدول->تبادلہ_LFNLState_جدول!=خالی_اشاریہ;
        icu::umtx_unlock(خالی_اشاریہ);

        اگر(!isCached) {
            اگر(!_EBCDICSwapLFNL(cnv->shared_ڈیٹا, p_Error_رمز)) {
                اگر(U_FAILURE(*p_Error_رمز)) {
                    واپس; /* something went wrong */
                }

                /* the option does not apply, remove it */
                cnv->options=p_دلائل->options&=~UCNV_OPTION_تبادلہ_LFNL;
            }
        }
    }

    اگر(uprv_strstr(p_دلائل->name, "18030")!=خالی_اشاریہ) {
        اگر(uprv_strstr(p_دلائل->name, "gb18030")!=خالی_اشاریہ || uprv_strstr(p_دلائل->name, "GB18030")!=خالی_اشاریہ) {
            /* set a flag for GB 18030 mode, which changes the callback behavior */
            cnv->options|=_MBCS_OPTION_GB18030;
        }
    } ورنہ اگر((uprv_strstr(p_دلائل->name, "KEIS")!=خالی_اشاریہ) || (uprv_strstr(p_دلائل->name, "keis")!=خالی_اشاریہ)) {
        /* set a flag for KEIS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_KEIS;
    } ورنہ اگر((uprv_strstr(p_دلائل->name, "JEF")!=خالی_اشاریہ) || (uprv_strstr(p_دلائل->name, "jef")!=خالی_اشاریہ)) {
        /* set a flag for JEF converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JEF;
    } ورنہ اگر((uprv_strstr(p_دلائل->name, "JIPS")!=خالی_اشاریہ) || (uprv_strstr(p_دلائل->name, "jips")!=خالی_اشاریہ)) {
        /* set a flag for JIPS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JIPS;
    }

    /* fix maxBytesPerUChar depending on outputType and options etc. */
    اگر(outputType==MBCS_OUTPUT_2_SISO) {
        cnv->max_بائٹیں_فی_UChar=3; /* SO+DBCS */
    }

    extIndexes=mbcs_جدول->extIndexes;
    اگر(extIndexes!=خالی_اشاریہ) {
        max_بائٹیں_فی_UChar = static_cast<int8_t>(UCNV_GET_MAX_بائٹیں_فی_UCHAR(extIndexes));
        اگر(outputType==MBCS_OUTPUT_2_SISO) {
            ++max_بائٹیں_فی_UChar; /* SO + multiple DBCS */
        }

        اگر(max_بائٹیں_فی_UChar>cnv->max_بائٹیں_فی_UChar) {
            cnv->max_بائٹیں_فی_UChar=max_بائٹیں_فی_UChar;
        }
    }

#اگر 0
    /*
     * documentation of UConverter fields used for status
     * all of these fields are (re)set to 0 by ucnv_bld.c and ucnv_reset()
     */

    /* toUnicode */
    cnv->مقصد_Unicode_حالت=0;     /*انحراف*/
    cnv->منوال=0;                /* state */
    cnv->مقصد_ULength=0;           /* byteIndex */

    /* fromUnicode */
    cnv->fromUChar32=0;
    cnv->from_Unicode_حالت=1;   /* prevLength */
#اگر_ختم
}

U_CDECL_BEGIN

ساکن ثابت حرف* U_CALLCONV
ucnv_MBCSGetName(ثابت UConverter *cnv) {
    اگر((cnv->options&UCNV_OPTION_تبادلہ_LFNL)!=0 && cnv->shared_ڈیٹا->mbcs.تبادلہ_LFNLName!=خالی_اشاریہ) {
        واپس cnv->shared_ڈیٹا->mbcs.تبادلہ_LFNLName;
    } ورنہ {
        واپس cnv->shared_ڈیٹا->static_ڈیٹا->name;
    }
}
U_CDECL_انتہا


/* MBCS-to-Unicode conversion functions ------------------------------------- */

ساکن UChar32 U_CALLCONV
ucnv_MBCSGetFallback(UConverterMBCSTable *mbcs_جدول, uint32_t انحراف) {
    ثابت _MBCSToUFallback *مقصد_UFallbacks;
    uint32_t i, آغاز, حد_بندی;

    حد_بندی=mbcs_جدول->گنتی_مقصد_UFallbacks;
    اگر(حد_بندی>0) {
        /* do a binary search for the fallback mapping */
        مقصد_UFallbacks=mbcs_جدول->مقصد_UFallbacks;
        آغاز=0;
        جب(آغاز<حد_بندی-1) {
            i=(آغاز+حد_بندی)/2;
            اگر(انحراف<مقصد_UFallbacks[i].انحراف) {
                حد_بندی=i;
            } ورنہ {
                آغاز=i;
            }
        }

        /* did we really find it? */
        اگر(انحراف==مقصد_UFallbacks[آغاز].انحراف) {
            واپس مقصد_UFallbacks[آغاز].رمز_نقطہ;
        }
    }

    واپس 0xfffe;
}

/* This version of ucnv_MBCSToUnicodeWithOffsets() is optimized for single-byte, single-state codepages. */
ساکن خالی
ucnv_MBCSSingle_مقصد_Unicode_With_Offsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                                UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    char16_t *ہدف;
    ثابت char16_t *ہدف_حد_بندی;
    int32_t *offsets;

    ثابت int32_t (*state_جدول)[256];

    int32_t ماخذ_اشاریہ;

    int32_t اندراج_2;
    char16_t c;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    ماخذ = reinterpret_cast<ثابت uint8_t*>(p_دلائل->ماخذ);
    ماخذ_حد_بندی = reinterpret_cast<ثابت uint8_t*>(p_دلائل->ماخذ_حد_بندی);
    ہدف=p_دلائل->ہدف;
    ہدف_حد_بندی=p_دلائل->ہدف_حد_بندی;
    offsets=p_دلائل->offsets;

    اگر((cnv->options&UCNV_OPTION_تبادلہ_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_ڈیٹا->mbcs.تبادلہ_LFNLState_جدول;
    } ورنہ {
        state_جدول=cnv->shared_ڈیٹا->mbcs.state_جدول;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ=0;

    /* conversion loop */
    جب(ماخذ<ماخذ_حد_بندی) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        اگر(ہدف>=ہدف_حد_بندی) {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }

        اندراج_2=state_جدول[0][*ماخذ++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        اگر(MBCS_اندراج_FINAL_IS_VALID_براہ_راست_16(اندراج_2)) {
            /* output BMP code point */
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            اگر(offsets!=خالی_اشاریہ) {
                *offsets++=ماخذ_اشاریہ;
            }

            /* normal end of action codes: prepare for a new character */
            ++ماخذ_اشاریہ;
            جاری;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_اندراج_FINAL_ACTION(اندراج_2));
        اگر(action==MBCS_STATE_VALID_براہ_راست_20 ||
           (action==MBCS_STATE_FALLBACK_براہ_راست_20 && UCNV_مقصد_U_USE_FALLBACK(cnv))
        ) {
            اندراج_2=MBCS_اندراج_FINAL_قدر(اندراج_2);
            /* output surrogate pair */
            *ہدف++ = static_cast<char16_t>(0xd800 | static_cast<char16_t>(اندراج_2 >> 10));
            اگر(offsets!=خالی_اشاریہ) {
                *offsets++=ماخذ_اشاریہ;
            }
            c = static_cast<char16_t>(0xdc00 | static_cast<char16_t>(اندراج_2 & 0x3ff));
            اگر(ہدف<ہدف_حد_بندی) {
                *ہدف++=c;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
            } ورنہ {
                /* target overflow */
                cnv->UChar_Error_بفر[0]=c;
                cnv->UChar_Error_بفر_لمبائی=1;
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }

            ++ماخذ_اشاریہ;
            جاری;
        } ورنہ اگر(action==MBCS_STATE_FALLBACK_براہ_راست_16) {
            اگر(UCNV_مقصد_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }

                ++ماخذ_اشاریہ;
                جاری;
            }
        } ورنہ اگر(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } ورنہ اگر(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
        } ورنہ {
            /* reserved, must never occur */
            ++ماخذ_اشاریہ;
            جاری;
        }

        اگر(U_FAILURE(*p_Error_رمز)) {
            /* callback(illegal) */
            توڑو;
        } ورنہ /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_دلائل->ماخذ = reinterpret_cast<ثابت حرف*>(ماخذ);
            cnv->مقصد_UBytes[0]=*(ماخذ-1);
            cnv->مقصد_ULength=ext_مقصد_U(cnv, cnv->shared_ڈیٹا,
                                    1, &ماخذ, ماخذ_حد_بندی,
                                    &ہدف, ہدف_حد_بندی,
                                    &offsets, ماخذ_اشاریہ,
                                    p_دلائل->flush,
                                    p_Error_رمز);
            ماخذ_اشاریہ += 1 + static_cast<int32_t>(ماخذ - reinterpret_cast<ثابت uint8_t*>(p_دلائل->ماخذ));

            اگر(U_FAILURE(*p_Error_رمز)) {
                /* not mappable or buffer overflow */
                توڑو;
            }
        }
    }

    /* write back the updated pointers */
    p_دلائل->ماخذ = reinterpret_cast<ثابت حرف*>(ماخذ);
    p_دلائل->ہدف=ہدف;
    p_دلائل->offsets=offsets;
}

/*
 * This version of ucnv_MBCSSingleToUnicodeWithOffsets() is optimized for single-byte, single-state codepages
 * that only map to and from the BMP.
 * In addition to single-byte optimizations, the offset calculations
 * become much easier.
 */
ساکن خالی
ucnv_MBCSSingle_مقصد_BMPWith_Offsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                            UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی, *آخری_ماخذ;
    char16_t *ہدف;
    int32_t ہدف_استعداد, length;
    int32_t *offsets;

    ثابت int32_t (*state_جدول)[256];

    int32_t ماخذ_اشاریہ;

    int32_t اندراج_2;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    ماخذ = reinterpret_cast<ثابت uint8_t*>(p_دلائل->ماخذ);
    ماخذ_حد_بندی = reinterpret_cast<ثابت uint8_t*>(p_دلائل->ماخذ_حد_بندی);
    ہدف=p_دلائل->ہدف;
    ہدف_استعداد = static_cast<int32_t>(p_دلائل->ہدف_حد_بندی - p_دلائل->ہدف);
    offsets=p_دلائل->offsets;

    اگر((cnv->options&UCNV_OPTION_تبادلہ_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_ڈیٹا->mbcs.تبادلہ_LFNLState_جدول;
    } ورنہ {
        state_جدول=cnv->shared_ڈیٹا->mbcs.state_جدول;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ=0;
    آخری_ماخذ=ماخذ;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(ماخذ_حد_بندی - ماخذ);
    اگر(length<ہدف_استعداد) {
        ہدف_استعداد=length;
    }

#اگر MBCS_UNROLL_SINGLE_مقصد_BMP
    /* unrolling makes it faster on Pentium III/Windows 2000 */
    /* unroll the loop with the most common case */
unrolled:
    اگر(ہدف_استعداد>=16) {
        int32_t count, loops, oredEntries;

        loops=count=ہدف_استعداد>>4;
        کرو {
            oredEntries=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            oredEntries|=اندراج_2=state_جدول[0][*ماخذ++];
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));

            /* were all 16 entries really valid? */
            اگر(!MBCS_اندراج_FINAL_IS_VALID_براہ_راست_16(oredEntries)) {
                /* no, return to the first of these 16 */
                ماخذ-=16;
                ہدف-=16;
                توڑو;
            }
        } جب(--count>0);
        count=loops-count;
        ہدف_استعداد-=16*count;

        اگر(offsets!=خالی_اشاریہ) {
            آخری_ماخذ+=16*count;
            جب(count>0) {
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                --count;
            }
        }
    }
#اگر_ختم

    /* conversion loop */
    جب(ہدف_استعداد > 0 && ماخذ < ماخذ_حد_بندی) {
        اندراج_2=state_جدول[0][*ماخذ++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        اگر(MBCS_اندراج_FINAL_IS_VALID_براہ_راست_16(اندراج_2)) {
            /* output BMP code point */
            *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            --ہدف_استعداد;
            جاری;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_اندراج_FINAL_ACTION(اندراج_2));
        اگر(action==MBCS_STATE_FALLBACK_براہ_راست_16) {
            اگر(UCNV_مقصد_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *ہدف++ = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
                --ہدف_استعداد;
                جاری;
            }
        } ورنہ اگر(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } ورنہ اگر(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
        } ورنہ {
            /* reserved, must never occur */
            جاری;
        }

        /* set offsets since the start or the last extension */
        اگر(offsets!=خالی_اشاریہ) {
            int32_t count = static_cast<int32_t>(ماخذ - آخری_ماخذ);

            /* predecrement: do not set the offset for the callback-causing character */
            جب(--count>0) {
                *offsets++=ماخذ_اشاریہ++;
            }
            /* offset and sourceIndex are now set for the current character */
        }

        اگر(U_FAILURE(*p_Error_رمز)) {
            /* callback(illegal) */
            توڑو;
        } ورنہ /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            آخری_ماخذ=ماخذ;
            cnv->مقصد_UBytes[0]=*(ماخذ-1);
            cnv->مقصد_ULength=ext_مقصد_U(cnv, cnv->shared_ڈیٹا,
                                    1, &ماخذ, ماخذ_حد_بندی,
                                    &ہدف, p_دلائل->ہدف_حد_بندی,
                                    &offsets, ماخذ_اشاریہ,
                                    p_دلائل->flush,
                                    p_Error_رمز);
            ماخذ_اشاریہ += 1 + static_cast<int32_t>(ماخذ - آخری_ماخذ);

            اگر(U_FAILURE(*p_Error_رمز)) {
                /* not mappable or buffer overflow */
                توڑو;
            }

            /* recalculate the targetCapacity after an extension mapping */
            ہدف_استعداد = static_cast<int32_t>(p_دلائل->ہدف_حد_بندی - ہدف);
            length = static_cast<int32_t>(ماخذ_حد_بندی - ماخذ);
            اگر(length<ہدف_استعداد) {
                ہدف_استعداد=length;
            }
        }

#اگر MBCS_UNROLL_SINGLE_مقصد_BMP
        /* unrolling makes it faster on Pentium III/Windows 2000 */
        جاؤ unrolled;
#اگر_ختم
    }

    اگر(U_SUCCESS(*p_Error_رمز) && ماخذ<ماخذ_حد_بندی && ہدف>=p_دلائل->ہدف_حد_بندی) {
        /* target is full */
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
    }

    /* set offsets since the start or the last callback */
    اگر(offsets!=خالی_اشاریہ) {
        size_t count=ماخذ-آخری_ماخذ;
        جب(count>0) {
            *offsets++=ماخذ_اشاریہ++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_دلائل->ماخذ = reinterpret_cast<ثابت حرف*>(ماخذ);
    p_دلائل->ہدف=ہدف;
    p_دلائل->offsets=offsets;
}

ساکن UBool
has_Valid_Trail_بائٹیں(ثابت int32_t (*state_جدول)[256], uint8_t state) {
    ثابت int32_t *صف=state_جدول[state];
    int32_t b, اندراج_2;
    /* First test for final entries in this state for some commonly valid byte values. */
    اندراج_2=صف[0xa1];
    اگر( !MBCS_اندراج_IS_TRANSITION(اندراج_2) &&
        MBCS_اندراج_FINAL_ACTION(اندراج_2)!=MBCS_STATE_ILLEGAL
    ) {
        واپس سچ;
    }
    اندراج_2=صف[0x41];
    اگر( !MBCS_اندراج_IS_TRANSITION(اندراج_2) &&
        MBCS_اندراج_FINAL_ACTION(اندراج_2)!=MBCS_STATE_ILLEGAL
    ) {
        واپس سچ;
    }
    /* Then test for final entries in this state. */
    برائے(b=0; b<=0xff; ++b) {
        اندراج_2=صف[b];
        اگر( !MBCS_اندراج_IS_TRANSITION(اندراج_2) &&
            MBCS_اندراج_FINAL_ACTION(اندراج_2)!=MBCS_STATE_ILLEGAL
        ) {
            واپس سچ;
        }
    }
    /* Then recurse for transition entries. */
    برائے(b=0; b<=0xff; ++b) {
        اندراج_2=صف[b];
        اگر( MBCS_اندراج_IS_TRANSITION(اندراج_2) &&
            has_Valid_Trail_بائٹیں(state_جدول, static_cast<uint8_t>(MBCS_اندراج_TRANSITION_STATE(اندراج_2)))
        ) {
            واپس سچ;
        }
    }
    واپس جھوٹ;
}

/*
 * Is byte b a single/lead byte in this state?
 * Recurse for transition states, because here we don't want to say that
 * b is a lead byte if all byte sequences that start with b are illegal.
 */
ساکن UBool
isSingleOrLead(ثابت int32_t (*state_جدول)[256], uint8_t state, UBool isDBCSOnly, uint8_t b) {
    ثابت int32_t *صف=state_جدول[state];
    int32_t اندراج_2=صف[b];
    اگر(MBCS_اندراج_IS_TRANSITION(اندراج_2)) {   /* lead byte */
        واپس has_Valid_Trail_بائٹیں(state_جدول, static_cast<uint8_t>(MBCS_اندراج_TRANSITION_STATE(اندراج_2)));
    } ورنہ {
        uint8_t action = static_cast<uint8_t>(MBCS_اندراج_FINAL_ACTION(اندراج_2));
        اگر(action==MBCS_STATE_CHANGE_ONLY && isDBCSOnly) {
            واپس جھوٹ;   /* SI/SO are illegal for DBCS-only conversion */
        } ورنہ {
            واپس action!=MBCS_STATE_ILLEGAL;
        }
    }
}

U_CFUNC خالی
ucnv_MBCSToUnicodeWithOffsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                          UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    char16_t *ہدف;
    ثابت char16_t *ہدف_حد_بندی;
    int32_t *offsets;

    ثابت int32_t (*state_جدول)[256];
    ثابت uint16_t *unicode_رمز_Units;

    uint32_t انحراف;
    uint8_t state;
    int8_t byte_اشاریہ;
    uint8_t *بائٹیں;

    int32_t ماخذ_اشاریہ, اگلا_ماخذ_اشاریہ;

    int32_t اندراج_2;
    char16_t c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_دلائل->converter;

    اگر(cnv->pre_مقصد_ULength>0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_مطابقت_مقصد_U(cnv, p_دلائل, -1, p_Error_رمز);

        اگر(U_FAILURE(*p_Error_رمز) || cnv->pre_مقصد_ULength<0) {
            واپس;
        }
    }

    اگر(cnv->shared_ڈیٹا->mbcs.گنتی_حالتیں==1) {
        اگر(!(cnv->shared_ڈیٹا->mbcs.unicode_نقاب&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingle_مقصد_BMPWith_Offsets(p_دلائل, p_Error_رمز);
        } ورنہ {
            ucnv_MBCSSingle_مقصد_Unicode_With_Offsets(p_دلائل, p_Error_رمز);
        }
        واپس;
    }

    /* set up the local pointers */
    ماخذ=(ثابت uint8_t *)p_دلائل->ماخذ;
    ماخذ_حد_بندی=(ثابت uint8_t *)p_دلائل->ماخذ_حد_بندی;
    ہدف=p_دلائل->ہدف;
    ہدف_حد_بندی=p_دلائل->ہدف_حد_بندی;
    offsets=p_دلائل->offsets;

    اگر((cnv->options&UCNV_OPTION_تبادلہ_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_ڈیٹا->mbcs.تبادلہ_LFNLState_جدول;
    } ورنہ {
        state_جدول=cnv->shared_ڈیٹا->mbcs.state_جدول;
    }
    unicode_رمز_Units=cnv->shared_ڈیٹا->mbcs.unicode_رمز_Units;

    /* get the converter state from UConverter */
    انحراف=cnv->مقصد_Unicode_حالت;
    byte_اشاریہ=cnv->مقصد_ULength;
    بائٹیں=cnv->مقصد_UBytes;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    اگر((state=(uint8_t)(cnv->منوال))==0) {
        state=cnv->shared_ڈیٹا->mbcs.dbcsOnlyState;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ=byte_اشاریہ==0 ? 0 : -1;
    اگلا_ماخذ_اشاریہ=0;

    /* conversion loop */
    جب(ماخذ<ماخذ_حد_بندی) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        اگر(ہدف>=ہدف_حد_بندی) {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }

        اگر(byte_اشاریہ==0) {
            /* optimized loop for 1/2-byte input and BMP output */
            اگر(offsets==خالی_اشاریہ) {
                کرو {
                    اندراج_2=state_جدول[state][*ماخذ];
                    اگر(MBCS_اندراج_IS_TRANSITION(اندراج_2)) {
                        state=(uint8_t)MBCS_اندراج_TRANSITION_STATE(اندراج_2);
                        انحراف=MBCS_اندراج_TRANSITION_انحراف(اندراج_2);

                        ++ماخذ;
                        اگر( ماخذ<ماخذ_حد_بندی &&
                            MBCS_اندراج_IS_FINAL(اندراج_2=state_جدول[state][*ماخذ]) &&
                            MBCS_اندراج_FINAL_ACTION(اندراج_2)==MBCS_STATE_VALID_16 &&
                            (c=unicode_رمز_Units[انحراف+MBCS_اندراج_FINAL_قدر_16(اندراج_2)])<0xfffe
                        ) {
                            ++ماخذ;
                            *ہدف++=c;
                            state=(uint8_t)MBCS_اندراج_FINAL_STATE(اندراج_2); /* typically 0 */
                            انحراف=0;
                        } ورنہ {
                            /* set the state and leave the optimized loop */
                            بائٹیں[0]=*(ماخذ-1);
                            byte_اشاریہ=1;
                            توڑو;
                        }
                    } ورنہ {
                        اگر(MBCS_اندراج_FINAL_IS_VALID_براہ_راست_16(اندراج_2)) {
                            /* output BMP code point */
                            ++ماخذ;
                            *ہدف++=(char16_t)MBCS_اندراج_FINAL_قدر_16(اندراج_2);
                            state=(uint8_t)MBCS_اندراج_FINAL_STATE(اندراج_2); /* typically 0 */
                        } ورنہ {
                            /* leave the optimized loop */
                            توڑو;
                        }
                    }
                } جب(ماخذ<ماخذ_حد_بندی && ہدف<ہدف_حد_بندی);
            } ورنہ /* offsets!=nullptr */ {
                کرو {
                    اندراج_2=state_جدول[state][*ماخذ];
                    اگر(MBCS_اندراج_IS_TRANSITION(اندراج_2)) {
                        state=(uint8_t)MBCS_اندراج_TRANSITION_STATE(اندراج_2);
                        انحراف=MBCS_اندراج_TRANSITION_انحراف(اندراج_2);

                        ++ماخذ;
                        اگر( ماخذ<ماخذ_حد_بندی &&
                            MBCS_اندراج_IS_FINAL(اندراج_2=state_جدول[state][*ماخذ]) &&
                            MBCS_اندراج_FINAL_ACTION(اندراج_2)==MBCS_STATE_VALID_16 &&
                            (c=unicode_رمز_Units[انحراف+MBCS_اندراج_FINAL_قدر_16(اندراج_2)])<0xfffe
                        ) {
                            ++ماخذ;
                            *ہدف++=c;
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ;
                                ماخذ_اشاریہ=(اگلا_ماخذ_اشاریہ+=2);
                            }
                            state=(uint8_t)MBCS_اندراج_FINAL_STATE(اندراج_2); /* typically 0 */
                            انحراف=0;
                        } ورنہ {
                            /* set the state and leave the optimized loop */
                            ++اگلا_ماخذ_اشاریہ;
                            بائٹیں[0]=*(ماخذ-1);
                            byte_اشاریہ=1;
                            توڑو;
                        }
                    } ورنہ {
                        اگر(MBCS_اندراج_FINAL_IS_VALID_براہ_راست_16(اندراج_2)) {
                            /* output BMP code point */
                            ++ماخذ;
                            *ہدف++=(char16_t)MBCS_اندراج_FINAL_قدر_16(اندراج_2);
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ;
                                ماخذ_اشاریہ=++اگلا_ماخذ_اشاریہ;
                            }
                            state=(uint8_t)MBCS_اندراج_FINAL_STATE(اندراج_2); /* typically 0 */
                        } ورنہ {
                            /* leave the optimized loop */
                            توڑو;
                        }
                    }
                } جب(ماخذ<ماخذ_حد_بندی && ہدف<ہدف_حد_بندی);
            }

            /*
             * these tests and break statements could be put inside the loop
             * if C had "break outerLoop" like Java
             */
            اگر(ماخذ>=ماخذ_حد_بندی) {
                توڑو;
            }
            اگر(ہدف>=ہدف_حد_بندی) {
                /* target is full */
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }

            ++اگلا_ماخذ_اشاریہ;
            بائٹیں[byte_اشاریہ++]=*ماخذ++;
        } ورنہ /* byteIndex>0 */ {
            ++اگلا_ماخذ_اشاریہ;
            اندراج_2=state_جدول[state][بائٹیں[byte_اشاریہ++]=*ماخذ++];
        }

        اگر(MBCS_اندراج_IS_TRANSITION(اندراج_2)) {
            state=(uint8_t)MBCS_اندراج_TRANSITION_STATE(اندراج_2);
            انحراف+=MBCS_اندراج_TRANSITION_انحراف(اندراج_2);
            جاری;
        }

        /* save the previous state for proper extension mapping with SI/SO-stateful converters */
        cnv->منوال=state;

        /* set the next state early so that we can reuse the entry variable */
        state=(uint8_t)MBCS_اندراج_FINAL_STATE(اندراج_2); /* typically 0 */

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action=(uint8_t)(MBCS_اندراج_FINAL_ACTION(اندراج_2));
        اگر(action==MBCS_STATE_VALID_16) {
            انحراف+=MBCS_اندراج_FINAL_قدر_16(اندراج_2);
            c=unicode_رمز_Units[انحراف];
            اگر(c<0xfffe) {
                /* output BMP code point */
                *ہدف++=c;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                byte_اشاریہ=0;
            } ورنہ اگر(c==0xfffe) {
                اگر(UCNV_مقصد_U_USE_FALLBACK(cnv) && (اندراج_2=(int32_t)ucnv_MBCSGetFallback(&cnv->shared_ڈیٹا->mbcs, انحراف))!=0xfffe) {
                    /* output fallback BMP code point */
                    *ہدف++=(char16_t)اندراج_2;
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                    byte_اشاریہ=0;
                }
            } ورنہ {
                /* callback(illegal) */
                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
            }
        } ورنہ اگر(action==MBCS_STATE_VALID_براہ_راست_16) {
            /* output BMP code point */
            *ہدف++=(char16_t)MBCS_اندراج_FINAL_قدر_16(اندراج_2);
            اگر(offsets!=خالی_اشاریہ) {
                *offsets++=ماخذ_اشاریہ;
            }
            byte_اشاریہ=0;
        } ورنہ اگر(action==MBCS_STATE_VALID_16_PAIR) {
            انحراف+=MBCS_اندراج_FINAL_قدر_16(اندراج_2);
            c=unicode_رمز_Units[انحراف++];
            اگر(c<0xd800) {
                /* output BMP code point below 0xd800 */
                *ہدف++=c;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                byte_اشاریہ=0;
            } ورنہ اگر(UCNV_مقصد_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                /* output roundtrip or fallback surrogate pair */
                *ہدف++=(char16_t)(c&0xdbff);
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                byte_اشاریہ=0;
                اگر(ہدف<ہدف_حد_بندی) {
                    *ہدف++=unicode_رمز_Units[انحراف];
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                } ورنہ {
                    /* target overflow */
                    cnv->UChar_Error_بفر[0]=unicode_رمز_Units[انحراف];
                    cnv->UChar_Error_بفر_لمبائی=1;
                    *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;

                    انحراف=0;
                    توڑو;
                }
            } ورنہ اگر(UCNV_مقصد_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                *ہدف++=unicode_رمز_Units[انحراف];
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                byte_اشاریہ=0;
            } ورنہ اگر(c==0xffff) {
                /* callback(illegal) */
                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
            }
        } ورنہ اگر(action==MBCS_STATE_VALID_براہ_راست_20 ||
                  (action==MBCS_STATE_FALLBACK_براہ_راست_20 && UCNV_مقصد_U_USE_FALLBACK(cnv))
        ) {
            اندراج_2=MBCS_اندراج_FINAL_قدر(اندراج_2);
            /* output surrogate pair */
            *ہدف++=(char16_t)(0xd800|(char16_t)(اندراج_2>>10));
            اگر(offsets!=خالی_اشاریہ) {
                *offsets++=ماخذ_اشاریہ;
            }
            byte_اشاریہ=0;
            c=(char16_t)(0xdc00|(char16_t)(اندراج_2&0x3ff));
            اگر(ہدف<ہدف_حد_بندی) {
                *ہدف++=c;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
            } ورنہ {
                /* target overflow */
                cnv->UChar_Error_بفر[0]=c;
                cnv->UChar_Error_بفر_لمبائی=1;
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;

                انحراف=0;
                توڑو;
            }
        } ورنہ اگر(action==MBCS_STATE_CHANGE_ONLY) {
            /*
             * This serves as a state change without any output.
             * It is useful for reading simple stateful encodings,
             * for example using just Shift-In/Shift-Out codes.
             * The 21 unused bits may later be used for more sophisticated
             * state transitions.
             */
            اگر(cnv->shared_ڈیٹا->mbcs.dbcsOnlyState==0) {
                byte_اشاریہ=0;
            } ورنہ {
                /* SI/SO are illegal for DBCS-only conversion */
                state=(uint8_t)(cnv->منوال); /* restore the previous state */

                /* callback(illegal) */
                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
            }
        } ورنہ اگر(action==MBCS_STATE_FALLBACK_براہ_راست_16) {
            اگر(UCNV_مقصد_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *ہدف++=(char16_t)MBCS_اندراج_FINAL_قدر_16(اندراج_2);
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                byte_اشاریہ=0;
            }
        } ورنہ اگر(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } ورنہ اگر(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
        } ورنہ {
            /* reserved, must never occur */
            byte_اشاریہ=0;
        }

        /* end of action codes: prepare for a new character */
        انحراف=0;

        اگر(byte_اشاریہ==0) {
            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
        } ورنہ اگر(U_FAILURE(*p_Error_رمز)) {
            /* callback(illegal) */
            اگر(byte_اشاریہ>1) {
                /*
                 * Ticket 5691: consistent illegal sequences:
                 * - We include at least the first byte in the illegal sequence.
                 * - If any of the non-initial bytes could be the start of a character,
                 *   we stop the illegal sequence before the first one of those.
                 */
                UBool isDBCSOnly = cnv->shared_ڈیٹا->mbcs.dbcsOnlyState != 0;
                int8_t i;
                برائے(i=1;
                    i<byte_اشاریہ && !isSingleOrLead(state_جدول, state, isDBCSOnly, بائٹیں[i]);
                    ++i) {}
                اگر(i<byte_اشاریہ) {
                    /* Back out some bytes. */
                    int8_t عقب_برآمد_فاصلہ=byte_اشاریہ-i;
                    int32_t بائٹیں_From_This_بفر=(int32_t)(ماخذ-(ثابت uint8_t *)p_دلائل->ماخذ);
                    byte_اشاریہ=i;  /* length of reported illegal byte sequence */
                    اگر(عقب_برآمد_فاصلہ<=بائٹیں_From_This_بفر) {
                        ماخذ-=عقب_برآمد_فاصلہ;
                    } ورنہ {
                        /* Back out bytes from the previous buffer: Need to replay them. */
                        cnv->pre_مقصد_ULength=(int8_t)(بائٹیں_From_This_بفر-عقب_برآمد_فاصلہ);
                        /* preToULength is negative! */
                        uprv_memcpy(cnv->pre_مقصد_U, بائٹیں+i, -cnv->pre_مقصد_ULength);
                        ماخذ=(ثابت uint8_t *)p_دلائل->ماخذ;
                    }
                }
            }
            توڑو;
        } ورنہ /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_دلائل->ماخذ=(ثابت حرف *)ماخذ;
            byte_اشاریہ=ext_مقصد_U(cnv, cnv->shared_ڈیٹا,
                              byte_اشاریہ, &ماخذ, ماخذ_حد_بندی,
                              &ہدف, ہدف_حد_بندی,
                              &offsets, ماخذ_اشاریہ,
                              p_دلائل->flush,
                              p_Error_رمز);
            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ+=(int32_t)(ماخذ-(ثابت uint8_t *)p_دلائل->ماخذ);

            اگر(U_FAILURE(*p_Error_رمز)) {
                /* not mappable or buffer overflow */
                توڑو;
            }
        }
    }

    /* set the converter state back into UConverter */
    cnv->مقصد_Unicode_حالت=انحراف;
    cnv->منوال=state;
    cnv->مقصد_ULength=byte_اشاریہ;

    /* write back the updated pointers */
    p_دلائل->ماخذ=(ثابت حرف *)ماخذ;
    p_دلائل->ہدف=ہدف;
    p_دلائل->offsets=offsets;
}

/*
 * This version of ucnv_MBCSGetNextUChar() is optimized for single-byte, single-state codepages.
 * We still need a conversion loop in case we find reserved action codes, which are to be ignored.
 */
ساکن UChar32
ucnv_MBCSSingle_Get_اگلا_UChar(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                        UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت int32_t (*state_جدول)[256];
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;

    int32_t اندراج_2;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    ماخذ = reinterpret_cast<ثابت uint8_t*>(p_دلائل->ماخذ);
    ماخذ_حد_بندی = reinterpret_cast<ثابت uint8_t*>(p_دلائل->ماخذ_حد_بندی);
    اگر((cnv->options&UCNV_OPTION_تبادلہ_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_ڈیٹا->mbcs.تبادلہ_LFNLState_جدول;
    } ورنہ {
        state_جدول=cnv->shared_ڈیٹا->mbcs.state_جدول;
    }

    /* conversion loop */
    جب(ماخذ<ماخذ_حد_بندی) {
        اندراج_2=state_جدول[0][*ماخذ++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* write back the updated pointer early so that we can return directly */
        p_دلائل->ماخذ = reinterpret_cast<ثابت حرف*>(ماخذ);

        اگر(MBCS_اندراج_FINAL_IS_VALID_براہ_راست_16(اندراج_2)) {
            /* output BMP code point */
            واپس static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_اندراج_FINAL_ACTION(اندراج_2));
        اگر( action==MBCS_STATE_VALID_براہ_راست_20 ||
            (action==MBCS_STATE_FALLBACK_براہ_راست_20 && UCNV_مقصد_U_USE_FALLBACK(cnv))
        ) {
            /* output supplementary code point */
            واپس static_cast<UChar32>(MBCS_اندراج_FINAL_قدر(اندراج_2) + 0x10000);
        } ورنہ اگر(action==MBCS_STATE_FALLBACK_براہ_راست_16) {
            اگر(UCNV_مقصد_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                واپس static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
            }
        } ورنہ اگر(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } ورنہ اگر(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
        } ورنہ {
            /* reserved, must never occur */
            جاری;
        }

        اگر(U_FAILURE(*p_Error_رمز)) {
            /* callback(illegal) */
            توڑو;
        } ورنہ /* unassigned sequence */ {
            /* defer to the generic implementation */
            p_دلائل->ماخذ = reinterpret_cast<ثابت حرف*>(ماخذ) - 1;
            واپس UCNV_GET_اگلا_UCHAR_USE_مقصد_U;
        }
    }

    /* no output because of empty input or only state changes */
    *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
    واپس 0xffff;
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
ساکن UChar32 U_CALLCONV
ucnv_MBCSGet_اگلا_UChar(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                  UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی, *آخری_ماخذ;

    ثابت int32_t (*state_جدول)[256];
    ثابت uint16_t *unicode_رمز_Units;

    uint32_t انحراف;
    uint8_t state;

    int32_t اندراج_2;
    UChar32 c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_دلائل->converter;

    اگر(cnv->pre_مقصد_ULength>0) {
        /* use the generic code in ucnv_getNextUChar() to continue with a partial match */
        واپس UCNV_GET_اگلا_UCHAR_USE_مقصد_U;
    }

    اگر(cnv->shared_ڈیٹا->mbcs.unicode_نقاب&UCNV_HAS_SURROGATES) {
        /*
         * Using the generic ucnv_getNextUChar() code lets us deal correctly
         * with the rare case of a codepage that maps single surrogates
         * without adding the complexity to this already complicated function here.
         */
        واپس UCNV_GET_اگلا_UCHAR_USE_مقصد_U;
    } ورنہ اگر(cnv->shared_ڈیٹا->mbcs.گنتی_حالتیں==1) {
        واپس ucnv_MBCSSingle_Get_اگلا_UChar(p_دلائل, p_Error_رمز);
    }

    /* set up the local pointers */
    ماخذ = آخری_ماخذ = reinterpret_cast<ثابت uint8_t*>(p_دلائل->ماخذ);
    ماخذ_حد_بندی = reinterpret_cast<ثابت uint8_t*>(p_دلائل->ماخذ_حد_بندی);

    اگر((cnv->options&UCNV_OPTION_تبادلہ_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_ڈیٹا->mbcs.تبادلہ_LFNLState_جدول;
    } ورنہ {
        state_جدول=cnv->shared_ڈیٹا->mbcs.state_جدول;
    }
    unicode_رمز_Units=cnv->shared_ڈیٹا->mbcs.unicode_رمز_Units;

    /* get the converter state from UConverter */
    انحراف=cnv->مقصد_Unicode_حالت;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    اگر ((state = static_cast<uint8_t>(cnv->منوال)) == 0) {
        state=cnv->shared_ڈیٹا->mbcs.dbcsOnlyState;
    }

    /* conversion loop */
    c=U_SENTINEL;
    جب(ماخذ<ماخذ_حد_بندی) {
        اندراج_2=state_جدول[state][*ماخذ++];
        اگر(MBCS_اندراج_IS_TRANSITION(اندراج_2)) {
            state = static_cast<uint8_t>(MBCS_اندراج_TRANSITION_STATE(اندراج_2));
            انحراف+=MBCS_اندراج_TRANSITION_انحراف(اندراج_2);

            /* optimization for 1/2-byte input and BMP output */
            اگر( ماخذ<ماخذ_حد_بندی &&
                MBCS_اندراج_IS_FINAL(اندراج_2=state_جدول[state][*ماخذ]) &&
                MBCS_اندراج_FINAL_ACTION(اندراج_2)==MBCS_STATE_VALID_16 &&
                (c=unicode_رمز_Units[انحراف+MBCS_اندراج_FINAL_قدر_16(اندراج_2)])<0xfffe
            ) {
                ++ماخذ;
                state = static_cast<uint8_t>(MBCS_اندراج_FINAL_STATE(اندراج_2)); /* typically 0 */
                /* output BMP code point */
                توڑو;
            }
        } ورنہ {
            /* save the previous state for proper extension mapping with SI/SO-stateful converters */
            cnv->منوال=state;

            /* set the next state early so that we can reuse the entry variable */
            state = static_cast<uint8_t>(MBCS_اندراج_FINAL_STATE(اندراج_2)); /* typically 0 */

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action = static_cast<uint8_t>(MBCS_اندراج_FINAL_ACTION(اندراج_2));
            اگر(action==MBCS_STATE_VALID_براہ_راست_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
                توڑو;
            } ورنہ اگر(action==MBCS_STATE_VALID_16) {
                انحراف+=MBCS_اندراج_FINAL_قدر_16(اندراج_2);
                c=unicode_رمز_Units[انحراف];
                اگر(c<0xfffe) {
                    /* output BMP code point */
                    توڑو;
                } ورنہ اگر(c==0xfffe) {
                    اگر(UCNV_مقصد_U_USE_FALLBACK(cnv) && (c=ucnv_MBCSGetFallback(&cnv->shared_ڈیٹا->mbcs, انحراف))!=0xfffe) {
                        توڑو;
                    }
                } ورنہ {
                    /* callback(illegal) */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                }
            } ورنہ اگر(action==MBCS_STATE_VALID_16_PAIR) {
                انحراف+=MBCS_اندراج_FINAL_قدر_16(اندراج_2);
                c=unicode_رمز_Units[انحراف++];
                اگر(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                    توڑو;
                } ورنہ اگر(UCNV_مقصد_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_رمز_Units[انحراف]+(0x10000-0xdc00);
                    توڑو;
                } ورنہ اگر(UCNV_مقصد_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_رمز_Units[انحراف];
                    توڑو;
                } ورنہ اگر(c==0xffff) {
                    /* callback(illegal) */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                }
            } ورنہ اگر(action==MBCS_STATE_VALID_براہ_راست_20 ||
                      (action==MBCS_STATE_FALLBACK_براہ_راست_20 && UCNV_مقصد_U_USE_FALLBACK(cnv))
            ) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_اندراج_FINAL_قدر(اندراج_2) + 0x10000);
                توڑو;
            } ورنہ اگر(action==MBCS_STATE_CHANGE_ONLY) {
                /*
                 * This serves as a state change without any output.
                 * It is useful for reading simple stateful encodings,
                 * for example using just Shift-In/Shift-Out codes.
                 * The 21 unused bits may later be used for more sophisticated
                 * state transitions.
                 */
                اگر(cnv->shared_ڈیٹا->mbcs.dbcsOnlyState!=0) {
                    /* SI/SO are illegal for DBCS-only conversion */
                    state = static_cast<uint8_t>(cnv->منوال); /* restore the previous state */

                    /* callback(illegal) */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                }
            } ورنہ اگر(action==MBCS_STATE_FALLBACK_براہ_راست_16) {
                اگر(UCNV_مقصد_U_USE_FALLBACK(cnv)) {
                    /* output BMP code point */
                    c = static_cast<char16_t>(MBCS_اندراج_FINAL_قدر_16(اندراج_2));
                    توڑو;
                }
            } ورنہ اگر(action==MBCS_STATE_UNASSIGNED) {
                /* just fall through */
            } ورنہ اگر(action==MBCS_STATE_ILLEGAL) {
                /* callback(illegal) */
                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
            } ورنہ {
                /* reserved (must never occur), or only state change */
                انحراف=0;
                آخری_ماخذ=ماخذ;
                جاری;
            }

            /* end of action codes: prepare for a new character */
            انحراف=0;

            اگر(U_FAILURE(*p_Error_رمز)) {
                /* callback(illegal) */
                توڑو;
            } ورنہ /* unassigned sequence */ {
                /* defer to the generic implementation */
                cnv->مقصد_Unicode_حالت=0;
                cnv->منوال=state;
                p_دلائل->ماخذ = reinterpret_cast<ثابت حرف*>(آخری_ماخذ);
                واپس UCNV_GET_اگلا_UCHAR_USE_مقصد_U;
            }
        }
    }

    اگر(c<0) {
        اگر(U_SUCCESS(*p_Error_رمز) && ماخذ==ماخذ_حد_بندی && آخری_ماخذ<ماخذ) {
            /* incomplete character byte sequence */
            cnv->مقصد_ULength = static_cast<int8_t>(ماخذ - آخری_ماخذ);
            uprv_memcpy(cnv->مقصد_UBytes, آخری_ماخذ, cnv->مقصد_ULength);
            *p_Error_رمز=U_TRUNCATED_CHAR_ملا;
        } ورنہ اگر(U_FAILURE(*p_Error_رمز)) {
            /* callback(illegal) */
            /*
             * Ticket 5691: consistent illegal sequences:
             * - We include at least the first byte in the illegal sequence.
             * - If any of the non-initial bytes could be the start of a character,
             *   we stop the illegal sequence before the first one of those.
             */
            UBool isDBCSOnly = static_cast<UBool>(cnv->shared_ڈیٹا->mbcs.dbcsOnlyState != 0);
            uint8_t *بائٹیں=cnv->مقصد_UBytes;
            *بائٹیں++=*آخری_ماخذ++;     /* first byte */
            اگر(آخری_ماخذ==ماخذ) {
                cnv->مقصد_ULength=1;
            } ورنہ /* lastSource<source: multi-byte character */ {
                int8_t i;
                برائے(i=1;
                    آخری_ماخذ<ماخذ && !isSingleOrLead(state_جدول, state, isDBCSOnly, *آخری_ماخذ);
                    ++i
                ) {
                    *بائٹیں++=*آخری_ماخذ++;
                }
                cnv->مقصد_ULength=i;
                ماخذ=آخری_ماخذ;
            }
        } ورنہ {
            /* no output because of empty input or only state changes */
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        }
        c=0xffff;
    }

    /* set the converter state back into UConverter, ready for a new character */
    cnv->مقصد_Unicode_حالت=0;
    cnv->منوال=state;

    /* write back the updated pointer */
    p_دلائل->ماخذ = reinterpret_cast<ثابت حرف*>(ماخذ);
    واپس c;
}

#اگر 0
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
ucnv_MBCSSingle_Simple_Get_اگلا_UChar(UConverter_Shared_ڈیٹا *shared_ڈیٹا,
                              uint8_t b, UBool useFallback) {
    int32_t اندراج_2;
    uint8_t action;

    اندراج_2=shared_ڈیٹا->mbcs.state_جدول[0][b];
    /* MBCS_ENTRY_IS_FINAL(entry) */

    اگر(MBCS_اندراج_FINAL_IS_VALID_براہ_راست_16(اندراج_2)) {
        /* output BMP code point */
        واپس (char16_t)MBCS_اندراج_FINAL_قدر_16(اندراج_2);
    }

    /*
     * An if-else-if chain provides more reliable performance for
     * the most common cases compared to a switch.
     */
    action=(uint8_t)(MBCS_اندراج_FINAL_ACTION(اندراج_2));
    اگر(action==MBCS_STATE_VALID_براہ_راست_20) {
        /* output supplementary code point */
        واپس 0x10000+MBCS_اندراج_FINAL_قدر(اندراج_2);
    } ورنہ اگر(action==MBCS_STATE_FALLBACK_براہ_راست_16) {
        اگر(!مقصد_U_USE_FALLBACK(useFallback)) {
            واپس 0xfffe;
        }
        /* output BMP code point */
        واپس (char16_t)MBCS_اندراج_FINAL_قدر_16(اندراج_2);
    } ورنہ اگر(action==MBCS_STATE_FALLBACK_براہ_راست_20) {
        اگر(!مقصد_U_USE_FALLBACK(useFallback)) {
            واپس 0xfffe;
        }
        /* output supplementary code point */
        واپس 0x10000+MBCS_اندراج_FINAL_قدر(اندراج_2);
    } ورنہ اگر(action==MBCS_STATE_UNASSIGNED) {
        واپس 0xfffe;
    } ورنہ اگر(action==MBCS_STATE_ILLEGAL) {
        واپس 0xffff;
    } ورنہ {
        /* reserved, must never occur */
        واپس 0xffff;
    }
}
#اگر_ختم

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
ucnv_MBCSSimple_Get_اگلا_UChar(UConverter_Shared_ڈیٹا *shared_ڈیٹا,
                        ثابت حرف *ماخذ, int32_t length,
                        UBool useFallback) {
    ثابت int32_t (*state_جدول)[256];
    ثابت uint16_t *unicode_رمز_Units;

    uint32_t انحراف;
    uint8_t state, action;

    UChar32 c;
    int32_t i, اندراج_2;

    اگر(length<=0) {
        /* no input at all: "illegal" */
        واپس 0xffff;
    }

#اگر 0
/*
 * Code disabled 2002dec09 (ICU 2.4) because it is not currently used in ICU. markus
 * TODO In future releases, verify that this function is never called for SBCS
 * conversions, i.e., that sharedData->mbcs.countStates==1 is still true.
 * Removal improves code coverage.
 */
    /* use optimized function if possible */
    اگر(shared_ڈیٹا->mbcs.گنتی_حالتیں==1) {
        اگر(length==1) {
            واپس ucnv_MBCSSingle_Simple_Get_اگلا_UChar(shared_ڈیٹا, (uint8_t)*ماخذ, useFallback);
        } ورنہ {
            واپس 0xffff; /* illegal: more than a single byte for an SBCS converter */
        }
    }
#اگر_ختم

    /* set up the local pointers */
    state_جدول=shared_ڈیٹا->mbcs.state_جدول;
    unicode_رمز_Units=shared_ڈیٹا->mbcs.unicode_رمز_Units;

    /* converter state */
    انحراف=0;
    state=shared_ڈیٹا->mbcs.dbcsOnlyState;

    /* conversion loop */
    برائے(i=0;;) {
        اندراج_2=state_جدول[state][(uint8_t)ماخذ[i++]];
        اگر(MBCS_اندراج_IS_TRANSITION(اندراج_2)) {
            state=(uint8_t)MBCS_اندراج_TRANSITION_STATE(اندراج_2);
            انحراف+=MBCS_اندراج_TRANSITION_انحراف(اندراج_2);

            اگر(i==length) {
                واپس 0xffff; /* truncated character */
            }
        } ورنہ {
            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=(uint8_t)(MBCS_اندراج_FINAL_ACTION(اندراج_2));
            اگر(action==MBCS_STATE_VALID_16) {
                انحراف+=MBCS_اندراج_FINAL_قدر_16(اندراج_2);
                c=unicode_رمز_Units[انحراف];
                اگر(c!=0xfffe) {
                    /*مکمل*/
                } ورنہ اگر(UCNV_مقصد_U_USE_FALLBACK(cnv)) {
                    c=ucnv_MBCSGetFallback(&shared_ڈیٹا->mbcs, انحراف);
                /* else done with 0xfffe */
                }
                توڑو;
            } ورنہ اگر(action==MBCS_STATE_VALID_براہ_راست_16) {
                /* output BMP code point */
                c=(char16_t)MBCS_اندراج_FINAL_قدر_16(اندراج_2);
                توڑو;
            } ورنہ اگر(action==MBCS_STATE_VALID_16_PAIR) {
                انحراف+=MBCS_اندراج_FINAL_قدر_16(اندراج_2);
                c=unicode_رمز_Units[انحراف++];
                اگر(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } ورنہ اگر(UCNV_مقصد_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=(UChar32)(((c&0x3ff)<<10)+unicode_رمز_Units[انحراف]+(0x10000-0xdc00));
                } ورنہ اگر(UCNV_مقصد_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_رمز_Units[انحراف];
                } ورنہ اگر(c==0xffff) {
                    واپس 0xffff;
                } ورنہ {
                    c=0xfffe;
                }
                توڑو;
            } ورنہ اگر(action==MBCS_STATE_VALID_براہ_راست_20) {
                /* output supplementary code point */
                c=0x10000+MBCS_اندراج_FINAL_قدر(اندراج_2);
                توڑو;
            } ورنہ اگر(action==MBCS_STATE_FALLBACK_براہ_راست_16) {
                اگر(!مقصد_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    توڑو;
                }
                /* output BMP code point */
                c=(char16_t)MBCS_اندراج_FINAL_قدر_16(اندراج_2);
                توڑو;
            } ورنہ اگر(action==MBCS_STATE_FALLBACK_براہ_راست_20) {
                اگر(!مقصد_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    توڑو;
                }
                /* output supplementary code point */
                c=0x10000+MBCS_اندراج_FINAL_قدر(اندراج_2);
                توڑو;
            } ورنہ اگر(action==MBCS_STATE_UNASSIGNED) {
                c=0xfffe;
                توڑو;
            }

            /*
             * forbid MBCS_STATE_CHANGE_ONLY for this function,
             * and MBCS_STATE_ILLEGAL and reserved action codes
             */
            واپس 0xffff;
        }
    }

    اگر(i!=length) {
        /* illegal for this function: not all input consumed */
        واپس 0xffff;
    }

    اگر(c==0xfffe) {
        /* try an extension mapping */
        ثابت int32_t *cx=shared_ڈیٹا->mbcs.extIndexes;
        اگر(cx!=خالی_اشاریہ) {
            واپس ucnv_ext_Simple_مطابقت_مقصد_U(cx, ماخذ, length, useFallback);
        }
    }

    واپس c;
}

/* MBCS-from-Unicode conversion functions ----------------------------------- */

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for double-byte codepages. */
ساکن خالی
ucnv_MBCSDoubleFromUnicodeWithOffsets(UConverter_From_Unicode_دلائل *p_دلائل,
                                  UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت char16_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف;
    int32_t ہدف_استعداد;
    int32_t *offsets;

    ثابت uint16_t *جدول;
    ثابت uint16_t *mbcs_اشاریہ;
    ثابت uint8_t *بائٹیں;

    UChar32 c;

    int32_t ماخذ_اشاریہ, اگلا_ماخذ_اشاریہ;

    uint32_t stage2_اندراج;
    uint32_t asciiRoundtrips;
    uint32_t value;
    uint8_t unicode_نقاب;

    /* use optimized function if possible */
    cnv=p_دلائل->converter;
    unicode_نقاب=cnv->shared_ڈیٹا->mbcs.unicode_نقاب;

    /* set up the local pointers */
    ماخذ=p_دلائل->ماخذ;
    ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
    ہدف = reinterpret_cast<uint8_t*>(p_دلائل->ہدف);
    ہدف_استعداد = static_cast<int32_t>(p_دلائل->ہدف_حد_بندی - p_دلائل->ہدف);
    offsets=p_دلائل->offsets;

    جدول=cnv->shared_ڈیٹا->mbcs.from_Unicode_جدول;
    mbcs_اشاریہ=cnv->shared_ڈیٹا->mbcs.mbcs_اشاریہ;
    اگر((cnv->options&UCNV_OPTION_تبادلہ_LFNL)!=0) {
        بائٹیں=cnv->shared_ڈیٹا->mbcs.تبادلہ_LFNLFrom_Unicode_بائٹیں;
    } ورنہ {
        بائٹیں=cnv->shared_ڈیٹا->mbcs.from_Unicode_بائٹیں;
    }
    asciiRoundtrips=cnv->shared_ڈیٹا->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ= c==0 ? 0 : -1;
    اگلا_ماخذ_اشاریہ=0;

    /* conversion loop */
    اگر(c!=0 && ہدف_استعداد>0) {
        جاؤ getTrail;
    }

    جب(ماخذ<ماخذ_حد_بندی) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        اگر(ہدف_استعداد>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ماخذ++;
            ++اگلا_ماخذ_اشاریہ;
            اگر(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *ہدف++ = static_cast<uint8_t>(c);
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                    ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                }
                --ہدف_استعداد;
                c=0;
                جاری;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            اگر(c<=0xd7ff) {
                value=DBCS_نتیجہ_FROM_MOST_BMP(mbcs_اشاریہ, (ثابت uint16_t *)بائٹیں, c);
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                اگر(value==0) {
                    جاؤ unassigned;
                }
                /* output the value */
            } ورنہ {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                اگر(U16_IS_SURROGATE(c) && !(unicode_نقاب&UCNV_HAS_SURROGATES)) {
                    اگر(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        اگر(ماخذ<ماخذ_حد_بندی) {
                            /* test the following code unit */
                            char16_t trail=*ماخذ;
                            اگر(U16_IS_TRAIL(trail)) {
                                ++ماخذ;
                                ++اگلا_ماخذ_اشاریہ;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                اگر(!(unicode_نقاب&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    /* callback(unassigned) */
                                    جاؤ unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } ورنہ {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                                توڑو;
                            }
                        } ورنہ {
                            /* no more input */
                            توڑو;
                        }
                    } ورنہ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                        توڑو;
                    }
                }

                /* convert the Unicode code point in c into codepage bytes */
                stage2_اندراج=MBCS_STAGE_2_FROM_U(جدول, c);

                /* get the bytes and the length for the output */
                /* MBCS_OUTPUT_2 */
                value=MBCS_قدر_2_FROM_STAGE_2(بائٹیں, stage2_اندراج, c);

                /* is this code point assigned, or do we use fallbacks? */
                اگر(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_اندراج, c) ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_دلائل->ماخذ=ماخذ;
                    c=_extFromU(cnv, cnv->shared_ڈیٹا,
                                c, &ماخذ, ماخذ_حد_بندی,
                                &ہدف, ہدف+ہدف_استعداد,
                                &offsets, ماخذ_اشاریہ,
                                p_دلائل->flush,
                                p_Error_رمز);
                    اگلا_ماخذ_اشاریہ += static_cast<int32_t>(ماخذ - p_دلائل->ماخذ);

                    اگر(U_FAILURE(*p_Error_رمز)) {
                        /* not mappable or buffer overflow */
                        توڑو;
                    } ورنہ {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        ہدف_استعداد = static_cast<int32_t>(p_دلائل->ہدف_حد_بندی - reinterpret_cast<حرف*>(ہدف));

                        /* normal end of conversion: prepare for a new character */
                        ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                        جاری;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            اگر(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *ہدف++ = static_cast<uint8_t>(value);
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                --ہدف_استعداد;
            } ورنہ /* length==2 */ {
                *ہدف++ = static_cast<uint8_t>(value >> 8);
                اگر(2<=ہدف_استعداد) {
                    *ہدف++ = static_cast<uint8_t>(value);
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                        *offsets++=ماخذ_اشاریہ;
                    }
                    ہدف_استعداد-=2;
                } ورنہ {
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                    cnv->char_Error_بفر[0] = static_cast<حرف>(value);
                    cnv->char_Error_بفر_لمبائی=1;

                    /* target overflow */
                    ہدف_استعداد=0;
                    *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                    c=0;
                    توڑو;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
            جاری;
        } ورنہ {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_دلائل->ماخذ=ماخذ;
    p_دلائل->ہدف = reinterpret_cast<حرف*>(ہدف);
    p_دلائل->offsets=offsets;
}

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for single-byte codepages. */
ساکن خالی
ucnv_MBCSSingleFromUnicodeWithOffsets(UConverter_From_Unicode_دلائل *p_دلائل,
                                  UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت char16_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف;
    int32_t ہدف_استعداد;
    int32_t *offsets;

    ثابت uint16_t *جدول;
    ثابت uint16_t *results;

    UChar32 c;

    int32_t ماخذ_اشاریہ, اگلا_ماخذ_اشاریہ;

    uint16_t value, min_قدر;
    UBool hasSupplementary;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    ماخذ=p_دلائل->ماخذ;
    ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
    ہدف = reinterpret_cast<uint8_t*>(p_دلائل->ہدف);
    ہدف_استعداد = static_cast<int32_t>(p_دلائل->ہدف_حد_بندی - p_دلائل->ہدف);
    offsets=p_دلائل->offsets;

    جدول=cnv->shared_ڈیٹا->mbcs.from_Unicode_جدول;
    اگر((cnv->options&UCNV_OPTION_تبادلہ_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_ڈیٹا->mbcs.تبادلہ_LFNLFrom_Unicode_بائٹیں);
    } ورنہ {
        results=(uint16_t *)cnv->shared_ڈیٹا->mbcs.from_Unicode_بائٹیں;
    }

    اگر(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_قدر=0x800;
    } ورنہ {
        /* use only roundtrips and fallbacks from private-use characters */
        min_قدر=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_ڈیٹا->mbcs.unicode_نقاب & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ= c==0 ? 0 : -1;
    اگلا_ماخذ_اشاریہ=0;

    /* conversion loop */
    اگر(c!=0 && ہدف_استعداد>0) {
        جاؤ getTrail;
    }

    جب(ماخذ<ماخذ_حد_بندی) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        اگر(ہدف_استعداد>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ماخذ++;
            ++اگلا_ماخذ_اشاریہ;
            اگر(U16_IS_SURROGATE(c)) {
                اگر(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                    اگر(ماخذ<ماخذ_حد_بندی) {
                        /* test the following code unit */
                        char16_t trail=*ماخذ;
                        اگر(U16_IS_TRAIL(trail)) {
                            ++ماخذ;
                            ++اگلا_ماخذ_اشاریہ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            اگر(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                /* callback(unassigned) */
                                جاؤ unassigned;
                            }
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } ورنہ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                            توڑو;
                        }
                    } ورنہ {
                        /* no more input */
                        توڑو;
                    }
                } ورنہ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    توڑو;
                }
            }

            /* convert the Unicode code point in c into codepage bytes */
            value=MBCS_SINGLE_نتیجہ_FROM_U(جدول, results, c);

            /* is this code point assigned, or do we use fallbacks? */
            اگر(value>=min_قدر) {
                /* assigned, write the output character bytes from value and length */
                /* length==1 */
                /* this is easy because we know that there is enough space */
                *ہدف++ = static_cast<uint8_t>(value);
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                --ہدف_استعداد;

                /* normal end of conversion: prepare for a new character */
                c=0;
                ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
            } ورنہ { /* unassigned */
unassigned:
                /* try an extension mapping */
                p_دلائل->ماخذ=ماخذ;
                c=_extFromU(cnv, cnv->shared_ڈیٹا,
                            c, &ماخذ, ماخذ_حد_بندی,
                            &ہدف, ہدف+ہدف_استعداد,
                            &offsets, ماخذ_اشاریہ,
                            p_دلائل->flush,
                            p_Error_رمز);
                اگلا_ماخذ_اشاریہ += static_cast<int32_t>(ماخذ - p_دلائل->ماخذ);

                اگر(U_FAILURE(*p_Error_رمز)) {
                    /* not mappable or buffer overflow */
                    توڑو;
                } ورنہ {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    ہدف_استعداد = static_cast<int32_t>(p_دلائل->ہدف_حد_بندی - reinterpret_cast<حرف*>(ہدف));

                    /* normal end of conversion: prepare for a new character */
                    ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                }
            }
        } ورنہ {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_دلائل->ماخذ=ماخذ;
    p_دلائل->ہدف = reinterpret_cast<حرف*>(ہدف);
    p_دلائل->offsets=offsets;
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
ساکن خالی
ucnv_MBCSSingleFromBMPWithOffsets(UConverter_From_Unicode_دلائل *p_دلائل,
                              UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت char16_t *ماخذ, *ماخذ_حد_بندی, *آخری_ماخذ;
    uint8_t *ہدف;
    int32_t ہدف_استعداد, length;
    int32_t *offsets;

    ثابت uint16_t *جدول;
    ثابت uint16_t *results;

    UChar32 c;

    int32_t ماخذ_اشاریہ;

    uint32_t asciiRoundtrips;
    uint16_t value, min_قدر;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    ماخذ=p_دلائل->ماخذ;
    ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
    ہدف = reinterpret_cast<uint8_t*>(p_دلائل->ہدف);
    ہدف_استعداد = static_cast<int32_t>(p_دلائل->ہدف_حد_بندی - p_دلائل->ہدف);
    offsets=p_دلائل->offsets;

    جدول=cnv->shared_ڈیٹا->mbcs.from_Unicode_جدول;
    اگر((cnv->options&UCNV_OPTION_تبادلہ_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_ڈیٹا->mbcs.تبادلہ_LFNLFrom_Unicode_بائٹیں);
    } ورنہ {
        results=(uint16_t *)cnv->shared_ڈیٹا->mbcs.from_Unicode_بائٹیں;
    }
    asciiRoundtrips=cnv->shared_ڈیٹا->mbcs.asciiRoundtrips;

    اگر(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_قدر=0x800;
    } ورنہ {
        /* use only roundtrips and fallbacks from private-use characters */
        min_قدر=0xc00;
    }

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ= c==0 ? 0 : -1;
    آخری_ماخذ=ماخذ;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(ماخذ_حد_بندی - ماخذ);
    اگر(length<ہدف_استعداد) {
        ہدف_استعداد=length;
    }

    /* conversion loop */
    اگر(c!=0 && ہدف_استعداد>0) {
        جاؤ getTrail;
    }

#اگر MBCS_UNROLL_SINGLE_FROM_BMP
    /* unrolling makes it slower on Pentium III/Windows 2000?! */
    /* unroll the loop with the most common case */
unrolled:
    اگر(ہدف_استعداد>=4) {
        int32_t count, loops;
        uint16_t anded_اقدار;

        loops=count=ہدف_استعداد>>2;
        کرو {
            c=*ماخذ++;
            anded_اقدار=value=MBCS_SINGLE_نتیجہ_FROM_U(جدول, results, c);
            *ہدف++=(uint8_t)value;
            c=*ماخذ++;
            anded_اقدار&=value=MBCS_SINGLE_نتیجہ_FROM_U(جدول, results, c);
            *ہدف++=(uint8_t)value;
            c=*ماخذ++;
            anded_اقدار&=value=MBCS_SINGLE_نتیجہ_FROM_U(جدول, results, c);
            *ہدف++=(uint8_t)value;
            c=*ماخذ++;
            anded_اقدار&=value=MBCS_SINGLE_نتیجہ_FROM_U(جدول, results, c);
            *ہدف++=(uint8_t)value;

            /* were all 4 entries really valid? */
            اگر(anded_اقدار<min_قدر) {
                /* no, return to the first of these 4 */
                ماخذ-=4;
                ہدف-=4;
                توڑو;
            }
        } جب(--count>0);
        count=loops-count;
        ہدف_استعداد-=4*count;

        اگر(offsets!=خالی_اشاریہ) {
            آخری_ماخذ+=4*count;
            جب(count>0) {
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                --count;
            }
        }

        c=0;
    }
#اگر_ختم

    جب(ہدف_استعداد>0) {
        /*
         * Get a correct Unicode code point:
         * a single char16_t for a BMP code point or
         * a matched surrogate pair for a "supplementary code point".
         */
        c=*ماخذ++;
        /*
         * Do not immediately check for single surrogates:
         * Assume that they are unassigned and check for them in that case.
         * This speeds up the conversion of assigned characters.
         */
        /* convert the Unicode code point in c into codepage bytes */
        اگر(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
            *ہدف++ = static_cast<uint8_t>(c);
            --ہدف_استعداد;
            c=0;
            جاری;
        }
        value=MBCS_SINGLE_نتیجہ_FROM_U(جدول, results, c);
        /* is this code point assigned, or do we use fallbacks? */
        اگر(value>=min_قدر) {
            /* assigned, write the output character bytes from value and length */
            /* length==1 */
            /* this is easy because we know that there is enough space */
            *ہدف++ = static_cast<uint8_t>(value);
            --ہدف_استعداد;

            /* normal end of conversion: prepare for a new character */
            c=0;
            جاری;
        } ورنہ اگر(!U16_IS_SURROGATE(c)) {
            /* normal, unassigned BMP character */
        } ورنہ اگر(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
            اگر(ماخذ<ماخذ_حد_بندی) {
                /* test the following code unit */
                char16_t trail=*ماخذ;
                اگر(U16_IS_TRAIL(trail)) {
                    ++ماخذ;
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } ورنہ {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    توڑو;
                }
            } ورنہ {
                /* no more input */
                اگر (p_دلائل->flush) {
                    *p_Error_رمز=U_TRUNCATED_CHAR_ملا;
                }
                توڑو;
            }
        } ورنہ {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
            توڑو;
        }

        /* c does not have a mapping */

        /* get the number of code units for c to correctly advance sourceIndex */
        length=U16_لمبائی(c);

        /* set offsets since the start or the last extension */
        اگر(offsets!=خالی_اشاریہ) {
            int32_t count = static_cast<int32_t>(ماخذ - آخری_ماخذ);

            /* do not set the offset for this character */
            count-=length;

            جب(count>0) {
                *offsets++=ماخذ_اشاریہ++;
                --count;
            }
            /* offsets and sourceIndex are now set for the current character */
        }

        /* try an extension mapping */
        آخری_ماخذ=ماخذ;
        c=_extFromU(cnv, cnv->shared_ڈیٹا,
                    c, &ماخذ, ماخذ_حد_بندی,
                    &ہدف, reinterpret_cast<ثابت uint8_t*>(p_دلائل->ہدف_حد_بندی),
                    &offsets, ماخذ_اشاریہ,
                    p_دلائل->flush,
                    p_Error_رمز);
        ماخذ_اشاریہ += length + static_cast<int32_t>(ماخذ - آخری_ماخذ);
        آخری_ماخذ=ماخذ;

        اگر(U_FAILURE(*p_Error_رمز)) {
            /* not mappable or buffer overflow */
            توڑو;
        } ورنہ {
            /* a mapping was written to the target, continue */

            /* recalculate the targetCapacity after an extension mapping */
            ہدف_استعداد = static_cast<int32_t>(p_دلائل->ہدف_حد_بندی - reinterpret_cast<حرف*>(ہدف));
            length = static_cast<int32_t>(ماخذ_حد_بندی - ماخذ);
            اگر(length<ہدف_استعداد) {
                ہدف_استعداد=length;
            }
        }

#اگر MBCS_UNROLL_SINGLE_FROM_BMP
        /* unrolling makes it slower on Pentium III/Windows 2000?! */
        جاؤ unrolled;
#اگر_ختم
    }

    اگر(U_SUCCESS(*p_Error_رمز) && ماخذ<ماخذ_حد_بندی && ہدف>=(uint8_t *)p_دلائل->ہدف_حد_بندی) {
        /* target is full */
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
    }

    /* set offsets since the start or the last callback */
    اگر(offsets!=خالی_اشاریہ) {
        size_t count=ماخذ-آخری_ماخذ;
        اگر (count > 0 && *p_Error_رمز == U_TRUNCATED_CHAR_ملا) {
            /*
            Caller gave us a partial supplementary character,
            which this function couldn't convert in any case.
            The callback will handle the offset.
            */
            count--;
        }
        جب(count>0) {
            *offsets++=ماخذ_اشاریہ++;
            --count;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_دلائل->ماخذ=ماخذ;
    p_دلائل->ہدف = reinterpret_cast<حرف*>(ہدف);
    p_دلائل->offsets=offsets;
}

U_CFUNC خالی
ucnv_MBCSFromUnicodeWithOffsets(UConverter_From_Unicode_دلائل *p_دلائل,
                            UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت char16_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف;
    int32_t ہدف_استعداد;
    int32_t *offsets;

    ثابت uint16_t *جدول;
    ثابت uint16_t *mbcs_اشاریہ;
    ثابت uint8_t *p, *بائٹیں;
    uint8_t outputType;

    UChar32 c;

    int32_t پچھلا_ماخذ_اشاریہ, ماخذ_اشاریہ, اگلا_ماخذ_اشاریہ;

    uint32_t stage2_اندراج;
    uint32_t asciiRoundtrips;
    uint32_t value;
    /* Shift-In and Shift-Out byte sequences differ by encoding scheme. */
    uint8_t si_بائٹیں[2] = {0, 0};
    uint8_t so_بائٹیں[2] = {0, 0};
    uint8_t si_لمبائی, so_لمبائی;
    int32_t length = 0, پچھلا_لمبائی;
    uint8_t unicode_نقاب;

    cnv=p_دلائل->converter;

    اگر(cnv->preFromUFirstCP>=0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_مطابقت_From_U(cnv, p_دلائل, -1, p_Error_رمز);

        اگر(U_FAILURE(*p_Error_رمز) || cnv->preFromULength<0) {
            واپس;
        }
    }

    /* use optimized function if possible */
    outputType=cnv->shared_ڈیٹا->mbcs.outputType;
    unicode_نقاب=cnv->shared_ڈیٹا->mbcs.unicode_نقاب;
    اگر(outputType==MBCS_OUTPUT_1 && !(unicode_نقاب&UCNV_HAS_SURROGATES)) {
        اگر(!(unicode_نقاب&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingleFromBMPWithOffsets(p_دلائل, p_Error_رمز);
        } ورنہ {
            ucnv_MBCSSingleFromUnicodeWithOffsets(p_دلائل, p_Error_رمز);
        }
        واپس;
    } ورنہ اگر(outputType==MBCS_OUTPUT_2 && cnv->shared_ڈیٹا->mbcs.utf8Friendly) {
        ucnv_MBCSDoubleFromUnicodeWithOffsets(p_دلائل, p_Error_رمز);
        واپس;
    }

    /* set up the local pointers */
    ماخذ=p_دلائل->ماخذ;
    ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
    ہدف=(uint8_t *)p_دلائل->ہدف;
    ہدف_استعداد=(int32_t)(p_دلائل->ہدف_حد_بندی-p_دلائل->ہدف);
    offsets=p_دلائل->offsets;

    جدول=cnv->shared_ڈیٹا->mbcs.from_Unicode_جدول;
    اگر(cnv->shared_ڈیٹا->mbcs.utf8Friendly) {
        mbcs_اشاریہ=cnv->shared_ڈیٹا->mbcs.mbcs_اشاریہ;
    } ورنہ {
        mbcs_اشاریہ=خالی_اشاریہ;
    }
    اگر((cnv->options&UCNV_OPTION_تبادلہ_LFNL)!=0) {
        بائٹیں=cnv->shared_ڈیٹا->mbcs.تبادلہ_LFNLFrom_Unicode_بائٹیں;
    } ورنہ {
        بائٹیں=cnv->shared_ڈیٹا->mbcs.from_Unicode_بائٹیں;
    }
    asciiRoundtrips=cnv->shared_ڈیٹا->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    اگر(outputType==MBCS_OUTPUT_2_SISO) {
        پچھلا_لمبائی=cnv->from_Unicode_حالت;
        اگر(پچھلا_لمبائی==0) {
            /* set the real value */
            پچھلا_لمبائی=1;
        }
    } ورنہ {
        /* prevent fromUnicodeStatus from being set to something non-0 */
        پچھلا_لمبائی=0;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    پچھلا_ماخذ_اشاریہ=-1;
    ماخذ_اشاریہ= c==0 ? 0 : -1;
    اگلا_ماخذ_اشاریہ=0;

    /* Get the SI/SO character for the converter */
    si_لمبائی = static_cast<uint8_t>(getSISOBytes(SI, cnv->options, si_بائٹیں));
    so_لمبائی = static_cast<uint8_t>(getSISOBytes(SO, cnv->options, so_بائٹیں));

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
    اگر(c!=0 && ہدف_استعداد>0) {
        جاؤ getTrail;
    }

    جب(ماخذ<ماخذ_حد_بندی) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        اگر(ہدف_استعداد>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ماخذ++;
            ++اگلا_ماخذ_اشاریہ;
            اگر(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *ہدف++=(uint8_t)c;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                    پچھلا_ماخذ_اشاریہ=ماخذ_اشاریہ;
                    ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                }
                --ہدف_استعداد;
                c=0;
                جاری;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            اگر(c<=0xd7ff && mbcs_اشاریہ!=خالی_اشاریہ) {
                value=mbcs_اشاریہ[c>>6];

                /* get the bytes and the length for the output (copied from below and adapted for utf8Friendly data) */
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                انتخاب(outputType) {
                حالت MBCS_OUTPUT_2:
                    value=((ثابت uint16_t *)بائٹیں)[value +(c&0x3f)];
                    اگر(value<=0xff) {
                        اگر(value==0) {
                            جاؤ unassigned;
                        } ورنہ {
                            length=1;
                        }
                    } ورنہ {
                        length=2;
                    }
                    توڑو;
                حالت MBCS_OUTPUT_2_SISO:
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
                    cnv->from_Unicode_حالت=پچھلا_لمبائی; /* save the old state */
                    value=((ثابت uint16_t *)بائٹیں)[value +(c&0x3f)];
                    اگر(value<=0xff) {
                        اگر(value==0) {
                            جاؤ unassigned;
                        } ورنہ اگر(پچھلا_لمبائی<=1) {
                            length=1;
                        } ورنہ {
                            /* change from double-byte mode to single-byte */
                            اگر (si_لمبائی == 1) {
                                value|=(uint32_t)si_بائٹیں[0]<<8;
                                length = 2;
                            } ورنہ اگر (si_لمبائی == 2) {
                                value|=(uint32_t)si_بائٹیں[1]<<8;
                                value|=(uint32_t)si_بائٹیں[0]<<16;
                                length = 3;
                            }
                            پچھلا_لمبائی=1;
                        }
                    } ورنہ {
                        اگر(پچھلا_لمبائی==2) {
                            length=2;
                        } ورنہ {
                            /* change from single-byte mode to double-byte */
                            اگر (so_لمبائی == 1) {
                                value|=(uint32_t)so_بائٹیں[0]<<16;
                                length = 3;
                            } ورنہ اگر (so_لمبائی == 2) {
                                value|=(uint32_t)so_بائٹیں[1]<<16;
                                value|=(uint32_t)so_بائٹیں[0]<<24;
                                length = 4;
                            }
                            پچھلا_لمبائی=2;
                        }
                    }
                    توڑو;
                حالت MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=((ثابت uint16_t *)بائٹیں)[value +(c&0x3f)];
                    اگر(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        جاؤ unassigned;
                    } ورنہ {
                        length=2;
                    }
                    توڑو;
                حالت MBCS_OUTPUT_3:
                    p=بائٹیں+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    اگر(value<=0xff) {
                        اگر(value==0) {
                            جاؤ unassigned;
                        } ورنہ {
                            length=1;
                        }
                    } ورنہ اگر(value<=0xffff) {
                        length=2;
                    } ورنہ {
                        length=3;
                    }
                    توڑو;
                حالت MBCS_OUTPUT_4:
                    value=((ثابت uint32_t *)بائٹیں)[value +(c&0x3f)];
                    اگر(value<=0xff) {
                        اگر(value==0) {
                            جاؤ unassigned;
                        } ورنہ {
                            length=1;
                        }
                    } ورنہ اگر(value<=0xffff) {
                        length=2;
                    } ورنہ اگر(value<=0xffffff) {
                        length=3;
                    } ورنہ {
                        length=4;
                    }
                    توڑو;
                حالت MBCS_OUTPUT_3_EUC:
                    value=((ثابت uint16_t *)بائٹیں)[value +(c&0x3f)];
                    /* EUC 16-bit fixed-length representation */
                    اگر(value<=0xff) {
                        اگر(value==0) {
                            جاؤ unassigned;
                        } ورنہ {
                            length=1;
                        }
                    } ورنہ اگر((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } ورنہ اگر((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } ورنہ {
                        length=2;
                    }
                    توڑو;
                حالت MBCS_OUTPUT_4_EUC:
                    p=بائٹیں+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    اگر(value<=0xff) {
                        اگر(value==0) {
                            جاؤ unassigned;
                        } ورنہ {
                            length=1;
                        }
                    } ورنہ اگر(value<=0xffff) {
                        length=2;
                    } ورنہ اگر((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } ورنہ اگر((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } ورنہ {
                        length=3;
                    }
                    توڑو;
                طےشدہ:
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
                    توڑو;
                }
                /* output the value */
            } ورنہ {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                اگر(U16_IS_SURROGATE(c) && !(unicode_نقاب&UCNV_HAS_SURROGATES)) {
                    اگر(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        اگر(ماخذ<ماخذ_حد_بندی) {
                            /* test the following code unit */
                            char16_t trail=*ماخذ;
                            اگر(U16_IS_TRAIL(trail)) {
                                ++ماخذ;
                                ++اگلا_ماخذ_اشاریہ;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                اگر(!(unicode_نقاب&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    cnv->from_Unicode_حالت=پچھلا_لمبائی; /* save the old state */
                                    /* callback(unassigned) */
                                    جاؤ unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } ورنہ {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                                توڑو;
                            }
                        } ورنہ {
                            /* no more input */
                            توڑو;
                        }
                    } ورنہ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                        توڑو;
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
                stage2_اندراج=MBCS_STAGE_2_FROM_U(جدول, c);

                /* get the bytes and the length for the output */
                انتخاب(outputType) {
                حالت MBCS_OUTPUT_2:
                    value=MBCS_قدر_2_FROM_STAGE_2(بائٹیں, stage2_اندراج, c);
                    اگر(value<=0xff) {
                        length=1;
                    } ورنہ {
                        length=2;
                    }
                    توڑو;
                حالت MBCS_OUTPUT_2_SISO:
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
                    cnv->from_Unicode_حالت=پچھلا_لمبائی; /* save the old state */
                    value=MBCS_قدر_2_FROM_STAGE_2(بائٹیں, stage2_اندراج, c);
                    اگر(value<=0xff) {
                        اگر(value==0 && MBCS_FROM_U_IS_ROUNDTRIP(stage2_اندراج, c)==0) {
                            /* no mapping, leave value==0 */
                            length=0;
                        } ورنہ اگر(پچھلا_لمبائی<=1) {
                            length=1;
                        } ورنہ {
                            /* change from double-byte mode to single-byte */
                            اگر (si_لمبائی == 1) {
                                value|=(uint32_t)si_بائٹیں[0]<<8;
                                length = 2;
                            } ورنہ اگر (si_لمبائی == 2) {
                                value|=(uint32_t)si_بائٹیں[1]<<8;
                                value|=(uint32_t)si_بائٹیں[0]<<16;
                                length = 3;
                            }
                            پچھلا_لمبائی=1;
                        }
                    } ورنہ {
                        اگر(پچھلا_لمبائی==2) {
                            length=2;
                        } ورنہ {
                            /* change from single-byte mode to double-byte */
                            اگر (so_لمبائی == 1) {
                                value|=(uint32_t)so_بائٹیں[0]<<16;
                                length = 3;
                            } ورنہ اگر (so_لمبائی == 2) {
                                value|=(uint32_t)so_بائٹیں[1]<<16;
                                value|=(uint32_t)so_بائٹیں[0]<<24;
                                length = 4;
                            }
                            پچھلا_لمبائی=2;
                        }
                    }
                    توڑو;
                حالت MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=MBCS_قدر_2_FROM_STAGE_2(بائٹیں, stage2_اندراج, c);
                    اگر(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        value=stage2_اندراج=0; /* stage2Entry=0 to reset roundtrip flags */
                        length=0;
                    } ورنہ {
                        length=2;
                    }
                    توڑو;
                حالت MBCS_OUTPUT_3:
                    p=MBCS_اشارہ_گر_3_FROM_STAGE_2(بائٹیں, stage2_اندراج, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    اگر(value<=0xff) {
                        length=1;
                    } ورنہ اگر(value<=0xffff) {
                        length=2;
                    } ورنہ {
                        length=3;
                    }
                    توڑو;
                حالت MBCS_OUTPUT_4:
                    value=MBCS_قدر_4_FROM_STAGE_2(بائٹیں, stage2_اندراج, c);
                    اگر(value<=0xff) {
                        length=1;
                    } ورنہ اگر(value<=0xffff) {
                        length=2;
                    } ورنہ اگر(value<=0xffffff) {
                        length=3;
                    } ورنہ {
                        length=4;
                    }
                    توڑو;
                حالت MBCS_OUTPUT_3_EUC:
                    value=MBCS_قدر_2_FROM_STAGE_2(بائٹیں, stage2_اندراج, c);
                    /* EUC 16-bit fixed-length representation */
                    اگر(value<=0xff) {
                        length=1;
                    } ورنہ اگر((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } ورنہ اگر((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } ورنہ {
                        length=2;
                    }
                    توڑو;
                حالت MBCS_OUTPUT_4_EUC:
                    p=MBCS_اشارہ_گر_3_FROM_STAGE_2(بائٹیں, stage2_اندراج, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    اگر(value<=0xff) {
                        length=1;
                    } ورنہ اگر(value<=0xffff) {
                        length=2;
                    } ورنہ اگر((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } ورنہ اگر((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } ورنہ {
                        length=3;
                    }
                    توڑو;
                طےشدہ:
                    /* must not occur */
                    /*
                     * To avoid compiler warnings that value & length may be
                     * used without having been initialized, we set them here.
                     * In reality, this is unreachable code.
                     * Not having a default branch also causes warnings with
                     * some compilers.
                     */
                    value=stage2_اندراج=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                    توڑو;
                }

                /* is this code point assigned, or do we use fallbacks? */
                اگر(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_اندراج, c)!=0 ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_دلائل->ماخذ=ماخذ;
                    c=_extFromU(cnv, cnv->shared_ڈیٹا,
                                c, &ماخذ, ماخذ_حد_بندی,
                                &ہدف, ہدف+ہدف_استعداد,
                                &offsets, ماخذ_اشاریہ,
                                p_دلائل->flush,
                                p_Error_رمز);
                    اگلا_ماخذ_اشاریہ+=(int32_t)(ماخذ-p_دلائل->ماخذ);
                    پچھلا_لمبائی=cnv->from_Unicode_حالت; /* restore SISO state */

                    اگر(U_FAILURE(*p_Error_رمز)) {
                        /* not mappable or buffer overflow */
                        توڑو;
                    } ورنہ {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        ہدف_استعداد=(int32_t)(p_دلائل->ہدف_حد_بندی-(حرف *)ہدف);

                        /* normal end of conversion: prepare for a new character */
                        اگر(offsets!=خالی_اشاریہ) {
                            پچھلا_ماخذ_اشاریہ=ماخذ_اشاریہ;
                            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                        }
                        جاری;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            اگر(length<=ہدف_استعداد) {
                اگر(offsets==خالی_اشاریہ) {
                    انتخاب(length) {
                        /* each branch falls through to the next one */
                    حالت 4:
                        *ہدف++=(uint8_t)(value>>24);
                        U_FALLTHROUGH;
                    حالت 3:
                        *ہدف++=(uint8_t)(value>>16);
                        U_FALLTHROUGH;
                    حالت 2:
                        *ہدف++=(uint8_t)(value>>8);
                        U_FALLTHROUGH;
                    حالت 1:
                        *ہدف++=(uint8_t)value;
                        U_FALLTHROUGH;
                    طےشدہ:
                        /* will never occur */
                        توڑو;
                    }
                } ورنہ {
                    انتخاب(length) {
                        /* each branch falls through to the next one */
                    حالت 4:
                        *ہدف++=(uint8_t)(value>>24);
                        *offsets++=ماخذ_اشاریہ;
                        U_FALLTHROUGH;
                    حالت 3:
                        *ہدف++=(uint8_t)(value>>16);
                        *offsets++=ماخذ_اشاریہ;
                        U_FALLTHROUGH;
                    حالت 2:
                        *ہدف++=(uint8_t)(value>>8);
                        *offsets++=ماخذ_اشاریہ;
                        U_FALLTHROUGH;
                    حالت 1:
                        *ہدف++=(uint8_t)value;
                        *offsets++=ماخذ_اشاریہ;
                        U_FALLTHROUGH;
                    طےشدہ:
                        /* will never occur */
                        توڑو;
                    }
                }
                ہدف_استعداد-=length;
            } ورنہ {
                uint8_t *char_Error_بفر;

                /*
                 * We actually do this backwards here:
                 * In order to save an intermediate variable, we output
                 * first to the overflow buffer what does not fit into the
                 * regular target.
                 */
                /* we know that 1<=targetCapacity<length<=4 */
                length-=ہدف_استعداد;
                char_Error_بفر=(uint8_t *)cnv->char_Error_بفر;
                انتخاب(length) {
                    /* each branch falls through to the next one */
                حالت 3:
                    *char_Error_بفر++=(uint8_t)(value>>16);
                    U_FALLTHROUGH;
                حالت 2:
                    *char_Error_بفر++=(uint8_t)(value>>8);
                    U_FALLTHROUGH;
                حالت 1:
                    *char_Error_بفر=(uint8_t)value;
                    U_FALLTHROUGH;
                طےشدہ:
                    /* will never occur */
                    توڑو;
                }
                cnv->char_Error_بفر_لمبائی=(int8_t)length;

                /* now output what fits into the regular target */
                value>>=8*length; /* length was reduced by targetCapacity */
                انتخاب(ہدف_استعداد) {
                    /* each branch falls through to the next one */
                حالت 3:
                    *ہدف++=(uint8_t)(value>>16);
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                    U_FALLTHROUGH;
                حالت 2:
                    *ہدف++=(uint8_t)(value>>8);
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                    U_FALLTHROUGH;
                حالت 1:
                    *ہدف++=(uint8_t)value;
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                    U_FALLTHROUGH;
                طےشدہ:
                    /* will never occur */
                    توڑو;
                }

                /* target overflow */
                ہدف_استعداد=0;
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                c=0;
                توڑو;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            اگر(offsets!=خالی_اشاریہ) {
                پچھلا_ماخذ_اشاریہ=ماخذ_اشاریہ;
                ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
            }
            جاری;
        } ورنہ {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
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
    اگر( U_SUCCESS(*p_Error_رمز) &&
        outputType==MBCS_OUTPUT_2_SISO && پچھلا_لمبائی==2 &&
        p_دلائل->flush && ماخذ>=ماخذ_حد_بندی && c==0
    ) {
        /* EBCDIC_STATEFUL ending with DBCS: emit an SI to return the output stream to SBCS */
        اگر(ہدف_استعداد>0) {
            *ہدف++ = si_بائٹیں[0];
            اگر (si_لمبائی == 2) {
                اگر (ہدف_استعداد<2) {
                    cnv->char_Error_بفر[0] = si_بائٹیں[1];
                    cnv->char_Error_بفر_لمبائی=1;
                    *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                } ورنہ {
                    *ہدف++ = si_بائٹیں[1];
                }
            }
            اگر(offsets!=خالی_اشاریہ) {
                /* set the last source character's index (sourceIndex points at sourceLimit now) */
                *offsets++=پچھلا_ماخذ_اشاریہ;
            }
        } ورنہ {
            /* target is full */
            cnv->char_Error_بفر[0] = si_بائٹیں[0];
            اگر (si_لمبائی == 2) {
                cnv->char_Error_بفر[1] = si_بائٹیں[1];
            }
            cnv->char_Error_بفر_لمبائی=si_لمبائی;
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        }
        پچھلا_لمبائی=1; /* we switched into SBCS */
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;
    cnv->from_Unicode_حالت=پچھلا_لمبائی;

    /* write back the updated pointers */
    p_دلائل->ماخذ=ماخذ;
    p_دلائل->ہدف=(حرف *)ہدف;
    p_دلائل->offsets=offsets;
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
ucnv_MBCSFromUChar32(UConverter_Shared_ڈیٹا *shared_ڈیٹا,
                 UChar32 c, uint32_t *p_قدر,
                 UBool useFallback) {
    ثابت int32_t *cx;
    ثابت uint16_t *جدول;
#اگر 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
    ثابت uint8_t *p;
#اگر_ختم
    uint32_t stage2_اندراج;
    uint32_t value;
    int32_t length;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    اگر(c<=0xffff || (shared_ڈیٹا->mbcs.unicode_نقاب&UCNV_HAS_SUPPLEMENTARY)) {
        جدول=shared_ڈیٹا->mbcs.from_Unicode_جدول;

        /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
        اگر(shared_ڈیٹا->mbcs.outputType==MBCS_OUTPUT_1) {
            value=MBCS_SINGLE_نتیجہ_FROM_U(جدول, (uint16_t *)shared_ڈیٹا->mbcs.from_Unicode_بائٹیں, c);
            /* is this code point assigned, or do we use fallbacks? */
            اگر(useFallback ? value>=0x800 : value>=0xc00) {
                *p_قدر=value&0xff;
                واپس 1;
            }
        } ورنہ /* outputType!=MBCS_OUTPUT_1 */ {
            stage2_اندراج=MBCS_STAGE_2_FROM_U(جدول, c);

            /* get the bytes and the length for the output */
            انتخاب(shared_ڈیٹا->mbcs.outputType) {
            حالت MBCS_OUTPUT_2:
                value=MBCS_قدر_2_FROM_STAGE_2(shared_ڈیٹا->mbcs.from_Unicode_بائٹیں, stage2_اندراج, c);
                اگر(value<=0xff) {
                    length=1;
                } ورنہ {
                    length=2;
                }
                توڑو;
#اگر 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
            حالت MBCS_OUTPUT_DBCS_ONLY:
                /* table with single-byte results, but only DBCS mappings used */
                value=MBCS_قدر_2_FROM_STAGE_2(shared_ڈیٹا->mbcs.from_Unicode_بائٹیں, stage2_اندراج, c);
                اگر(value<=0xff) {
                    /* no mapping or SBCS result, not taken for DBCS-only */
                    value=stage2_اندراج=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                } ورنہ {
                    length=2;
                }
                توڑو;
            حالت MBCS_OUTPUT_3:
                p=MBCS_اشارہ_گر_3_FROM_STAGE_2(shared_ڈیٹا->mbcs.from_Unicode_بائٹیں, stage2_اندراج, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                اگر(value<=0xff) {
                    length=1;
                } ورنہ اگر(value<=0xffff) {
                    length=2;
                } ورنہ {
                    length=3;
                }
                توڑو;
            حالت MBCS_OUTPUT_4:
                value=MBCS_قدر_4_FROM_STAGE_2(shared_ڈیٹا->mbcs.from_Unicode_بائٹیں, stage2_اندراج, c);
                اگر(value<=0xff) {
                    length=1;
                } ورنہ اگر(value<=0xffff) {
                    length=2;
                } ورنہ اگر(value<=0xffffff) {
                    length=3;
                } ورنہ {
                    length=4;
                }
                توڑو;
            حالت MBCS_OUTPUT_3_EUC:
                value=MBCS_قدر_2_FROM_STAGE_2(shared_ڈیٹا->mbcs.from_Unicode_بائٹیں, stage2_اندراج, c);
                /* EUC 16-bit fixed-length representation */
                اگر(value<=0xff) {
                    length=1;
                } ورنہ اگر((value&0x8000)==0) {
                    value|=0x8e8000;
                    length=3;
                } ورنہ اگر((value&0x80)==0) {
                    value|=0x8f0080;
                    length=3;
                } ورنہ {
                    length=2;
                }
                توڑو;
            حالت MBCS_OUTPUT_4_EUC:
                p=MBCS_اشارہ_گر_3_FROM_STAGE_2(shared_ڈیٹا->mbcs.from_Unicode_بائٹیں, stage2_اندراج, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                /* EUC 16-bit fixed-length representation applied to the first two bytes */
                اگر(value<=0xff) {
                    length=1;
                } ورنہ اگر(value<=0xffff) {
                    length=2;
                } ورنہ اگر((value&0x800000)==0) {
                    value|=0x8e800000;
                    length=4;
                } ورنہ اگر((value&0x8000)==0) {
                    value|=0x8f008000;
                    length=4;
                } ورنہ {
                    length=3;
                }
                توڑو;
#اگر_ختم
            طےشدہ:
                /* must not occur */
                واپس -1;
            }

            /* is this code point assigned, or do we use fallbacks? */
            اگر( MBCS_FROM_U_IS_ROUNDTRIP(stage2_اندراج, c) ||
                (FROM_U_USE_FALLBACK(useFallback, c) && value!=0)
            ) {
                /*
                 * We allow a 0 byte output if the "assigned" bit is set for this entry.
                 * There is no way with this data structure for fallback output
                 * to be a zero byte.
                 */
                /* assigned */
                *p_قدر=value;
                واپس length;
            }
        }
    }

    cx=shared_ڈیٹا->mbcs.extIndexes;
    اگر(cx!=خالی_اشاریہ) {
        length=ucnv_ext_Simple_مطابقت_From_U(cx, c, p_قدر, useFallback);
        واپس length>=0 ? length : -length;  /* return abs(length); */
    }

    /* unassigned */
    واپس 0;
}


#اگر 0
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
ucnv_MBCSSingleFromUChar32(UConverter_Shared_ڈیٹا *shared_ڈیٹا,
                       UChar32 c,
                       UBool useFallback) {
    ثابت uint16_t *جدول;
    int32_t value;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    اگر(c>=0x10000 && !(shared_ڈیٹا->mbcs.unicode_نقاب&UCNV_HAS_SUPPLEMENTARY)) {
        واپس -1;
    }

    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    جدول=shared_ڈیٹا->mbcs.from_Unicode_جدول;

    /* get the byte for the output */
    value=MBCS_SINGLE_نتیجہ_FROM_U(جدول, (uint16_t *)shared_ڈیٹا->mbcs.from_Unicode_بائٹیں, c);
    /* is this code point assigned, or do we use fallbacks? */
    اگر(useFallback ? value>=0x800 : value>=0xc00) {
        واپس value&0xff;
    } ورنہ {
        واپس -1;
    }
}
#اگر_ختم

/* MBCS-from-UTF-8 conversion functions ------------------------------------- */

/* offsets for n-byte UTF-8 sequences that were calculated with ((lead<<6)+trail)<<6+trail... */
ساکن ثابت UChar32
utf8_offsets[5]={ 0, 0, 0x3080, 0xE2080, 0x3C82080 };

ساکن خالی U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_دلائل *pFromUArgs,
                  UConverter_مقصد_Unicode_دلائل *p_مقصد_UArgs,
                  UError_رمز *p_Error_رمز) {
    UConverter *utf8, *cnv;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف;
    int32_t ہدف_استعداد;

    ثابت uint16_t *جدول, *sbcs_اشاریہ;
    ثابت uint16_t *results;

    int8_t old_مقصد_ULength, مقصد_ULength, مقصد_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t asciiRoundtrips;
    uint16_t value, min_قدر = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_مقصد_UArgs->converter;
    cnv=pFromUArgs->converter;
    ماخذ=(uint8_t *)p_مقصد_UArgs->ماخذ;
    ماخذ_حد_بندی=(uint8_t *)p_مقصد_UArgs->ماخذ_حد_بندی;
    ہدف = reinterpret_cast<uint8_t*>(pFromUArgs->ہدف);
    ہدف_استعداد = static_cast<int32_t>(pFromUArgs->ہدف_حد_بندی - pFromUArgs->ہدف);

    جدول=cnv->shared_ڈیٹا->mbcs.from_Unicode_جدول;
    sbcs_اشاریہ=cnv->shared_ڈیٹا->mbcs.sbcs_اشاریہ;
    اگر((cnv->options&UCNV_OPTION_تبادلہ_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_ڈیٹا->mbcs.تبادلہ_LFNLFrom_Unicode_بائٹیں);
    } ورنہ {
        results=(uint16_t *)cnv->shared_ڈیٹا->mbcs.from_Unicode_بائٹیں;
    }
    asciiRoundtrips=cnv->shared_ڈیٹا->mbcs.asciiRoundtrips;

    اگر(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_قدر=0x800;
    } ورنہ {
        /* use only roundtrips and fallbacks from private-use characters */
        min_قدر=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_ڈیٹا->mbcs.unicode_نقاب & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    اگر(utf8->مقصد_ULength > 0) {
        مقصد_ULength=old_مقصد_ULength=utf8->مقصد_ULength;
        مقصد_ULimit = static_cast<int8_t>(utf8->منوال);
        c = static_cast<UChar32>(utf8->مقصد_Unicode_حالت);
    } ورنہ {
        مقصد_ULength=old_مقصد_ULength=مقصد_ULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(ماخذ_حد_بندی - ماخذ) - (مقصد_ULimit - old_مقصد_ULength);
        اگر(length>0) {
            uint8_t b1=*(ماخذ_حد_بندی-1);
            اگر(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } ورنہ اگر(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(ماخذ_حد_بندی-2);
                اگر(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    ماخذ_حد_بندی-=2;
                }
            } ورنہ اگر(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --ماخذ_حد_بندی;
            }
        }
    }

    اگر(c!=0 && ہدف_استعداد>0) {
        utf8->مقصد_Unicode_حالت=0;
        utf8->مقصد_ULength=0;
        جاؤ مزید_بائٹیں;
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
    جب(ماخذ<ماخذ_حد_بندی) {
        اگر(ہدف_استعداد>0) {
            b=*ماخذ++;
            اگر(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                اگر(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *ہدف++ = b;
                    --ہدف_استعداد;
                    جاری;
                } ورنہ {
                    c=b;
                    value=SBCS_نتیجہ_FROM_UTF8(sbcs_اشاریہ, results, 0, c);
                }
            } ورنہ {
                اگر(b<0xe0) {
                    اگر( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*ماخذ - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++ماخذ;
                        value=SBCS_نتیجہ_FROM_UTF8(sbcs_اشاریہ, results, c, t1);
                        اگر(value>=min_قدر) {
                            *ہدف++ = static_cast<uint8_t>(value);
                            --ہدف_استعداد;
                            جاری;
                        } ورنہ {
                            c=(c<<6)|t1;
                        }
                    } ورنہ {
                        c=-1;
                    }
                } ورنہ اگر(b==0xe0) {
                    اگر( /* handle U+0800..U+0FFF inline */
                        (t1 = static_cast<uint8_t>(ماخذ[0] - 0x80)) <= 0x3f && t1 >= 0x20 &&
                        (t2 = static_cast<uint8_t>(ماخذ[1] - 0x80)) <= 0x3f
                    ) {
                        c=t1;
                        ماخذ+=2;
                        value=SBCS_نتیجہ_FROM_UTF8(sbcs_اشاریہ, results, c, t2);
                        اگر(value>=min_قدر) {
                            *ہدف++ = static_cast<uint8_t>(value);
                            --ہدف_استعداد;
                            جاری;
                        } ورنہ {
                            c=(c<<6)|t2;
                        }
                    } ورنہ {
                        c=-1;
                    }
                } ورنہ {
                    c=-1;
                }

                اگر(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    old_مقصد_ULength=0;
                    مقصد_ULength=1;
                    مقصد_ULimit=U8_گنتی_بائٹیں_NON_ASCII(b);
                    c=b;
مزید_بائٹیں:
                    جب(مقصد_ULength<مقصد_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        اگر(ماخذ<(uint8_t *)p_مقصد_UArgs->ماخذ_حد_بندی) {
                            b=*ماخذ;
                            اگر(icu::UTF8::isValidTrail(c, b, مقصد_ULength, مقصد_ULimit)) {
                                ++ماخذ;
                                ++مقصد_ULength;
                                c=(c<<6)+b;
                            } ورنہ {
                                توڑو; /* sequence too short, stop with toULength<toULimit */
                            }
                        } ورنہ {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            ماخذ-=(مقصد_ULength-old_مقصد_ULength);
                            جب(old_مقصد_ULength<مقصد_ULength) {
                                utf8->مقصد_UBytes[old_مقصد_ULength++]=*ماخذ++;
                            }
                            utf8->مقصد_Unicode_حالت=c;
                            utf8->مقصد_ULength=مقصد_ULength;
                            utf8->منوال=مقصد_ULimit;
                            p_مقصد_UArgs->ماخذ=(حرف *)ماخذ;
                            pFromUArgs->ہدف = reinterpret_cast<حرف*>(ہدف);
                            واپس;
                        }
                    }

                    اگر(مقصد_ULength==مقصد_ULimit) {
                        c-=utf8_offsets[مقصد_ULength];
                        اگر(مقصد_ULength<=3) {  /* BMP */
                            value=MBCS_SINGLE_نتیجہ_FROM_U(جدول, results, c);
                        } ورنہ {
                            /* supplementary code point */
                            اگر(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                value=0;
                            } ورنہ {
                                value=MBCS_SINGLE_نتیجہ_FROM_U(جدول, results, c);
                            }
                        }
                    } ورنہ {
                        /* error handling: illegal UTF-8 byte sequence */
                        ماخذ-=(مقصد_ULength-old_مقصد_ULength);
                        جب(old_مقصد_ULength<مقصد_ULength) {
                            utf8->مقصد_UBytes[old_مقصد_ULength++]=*ماخذ++;
                        }
                        utf8->مقصد_ULength=مقصد_ULength;
                        p_مقصد_UArgs->ماخذ=(حرف *)ماخذ;
                        pFromUArgs->ہدف = reinterpret_cast<حرف*>(ہدف);
                        *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                        واپس;
                    }
                }
            }

            اگر(value>=min_قدر) {
                /* output the mapping for c */
                *ہدف++ = static_cast<uint8_t>(value);
                --ہدف_استعداد;
            } ورنہ {
                /* value<minValue means c is unassigned (unmappable) */
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                ساکن ثابت char16_t nul=0;
                ثابت char16_t *no_ماخذ=&nul;
                c=_extFromU(cnv, cnv->shared_ڈیٹا,
                            c, &no_ماخذ, no_ماخذ,
                            &ہدف, ہدف+ہدف_استعداد,
                            خالی_اشاریہ, -1,
                            pFromUArgs->flush,
                            p_Error_رمز);

                اگر(U_FAILURE(*p_Error_رمز)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    توڑو;
                } ورنہ اگر(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *p_Error_رمز=U_USING_DEFAULT_WARNING;
                    توڑو;
                } ورنہ {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    ہدف_استعداد = static_cast<int32_t>(pFromUArgs->ہدف_حد_بندی - reinterpret_cast<حرف*>(ہدف));
                }
            }
        } ورنہ {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    اگر(U_SUCCESS(*p_Error_رمز) &&
            cnv->preFromUFirstCP<0 &&
            ماخذ<(ماخذ_حد_بندی=(uint8_t *)p_مقصد_UArgs->ماخذ_حد_بندی)) {
        c=utf8->مقصد_UBytes[0]=b=*ماخذ++;
        مقصد_ULength=1;
        مقصد_ULimit=U8_گنتی_بائٹیں(b);
        جب(ماخذ<ماخذ_حد_بندی) {
            utf8->مقصد_UBytes[مقصد_ULength++]=b=*ماخذ++;
            c=(c<<6)+b;
        }
        utf8->مقصد_Unicode_حالت=c;
        utf8->مقصد_ULength=مقصد_ULength;
        utf8->منوال=مقصد_ULimit;
    }

    /* write back the updated pointers */
    p_مقصد_UArgs->ماخذ=(حرف *)ماخذ;
    pFromUArgs->ہدف = reinterpret_cast<حرف*>(ہدف);
}

ساکن خالی U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_دلائل *pFromUArgs,
                  UConverter_مقصد_Unicode_دلائل *p_مقصد_UArgs,
                  UError_رمز *p_Error_رمز) {
    UConverter *utf8, *cnv;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف;
    int32_t ہدف_استعداد;

    ثابت uint16_t *جدول, *mbcs_اشاریہ;
    ثابت uint16_t *results;

    int8_t old_مقصد_ULength, مقصد_ULength, مقصد_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t stage2_اندراج;
    uint32_t asciiRoundtrips;
    uint16_t value = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_مقصد_UArgs->converter;
    cnv=pFromUArgs->converter;
    ماخذ=(uint8_t *)p_مقصد_UArgs->ماخذ;
    ماخذ_حد_بندی=(uint8_t *)p_مقصد_UArgs->ماخذ_حد_بندی;
    ہدف = reinterpret_cast<uint8_t*>(pFromUArgs->ہدف);
    ہدف_استعداد = static_cast<int32_t>(pFromUArgs->ہدف_حد_بندی - pFromUArgs->ہدف);

    جدول=cnv->shared_ڈیٹا->mbcs.from_Unicode_جدول;
    mbcs_اشاریہ=cnv->shared_ڈیٹا->mbcs.mbcs_اشاریہ;
    اگر((cnv->options&UCNV_OPTION_تبادلہ_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_ڈیٹا->mbcs.تبادلہ_LFNLFrom_Unicode_بائٹیں);
    } ورنہ {
        results=(uint16_t *)cnv->shared_ڈیٹا->mbcs.from_Unicode_بائٹیں;
    }
    asciiRoundtrips=cnv->shared_ڈیٹا->mbcs.asciiRoundtrips;

    hasSupplementary = static_cast<UBool>(cnv->shared_ڈیٹا->mbcs.unicode_نقاب & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    اگر(utf8->مقصد_ULength > 0) {
        مقصد_ULength=old_مقصد_ULength=utf8->مقصد_ULength;
        مقصد_ULimit = static_cast<int8_t>(utf8->منوال);
        c = static_cast<UChar32>(utf8->مقصد_Unicode_حالت);
    } ورنہ {
        مقصد_ULength=old_مقصد_ULength=مقصد_ULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(ماخذ_حد_بندی - ماخذ) - (مقصد_ULimit - old_مقصد_ULength);
        اگر(length>0) {
            uint8_t b1=*(ماخذ_حد_بندی-1);
            اگر(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } ورنہ اگر(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(ماخذ_حد_بندی-2);
                اگر(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    ماخذ_حد_بندی-=2;
                }
            } ورنہ اگر(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --ماخذ_حد_بندی;
            }
        }
    }

    اگر(c!=0 && ہدف_استعداد>0) {
        utf8->مقصد_Unicode_حالت=0;
        utf8->مقصد_ULength=0;
        جاؤ مزید_بائٹیں;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    جب(ماخذ<ماخذ_حد_بندی) {
        اگر(ہدف_استعداد>0) {
            b=*ماخذ++;
            اگر(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                اگر(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *ہدف++=b;
                    --ہدف_استعداد;
                    جاری;
                } ورنہ {
                    value=DBCS_نتیجہ_FROM_UTF8(mbcs_اشاریہ, results, 0, b);
                    اگر(value==0) {
                        c=b;
                        جاؤ unassigned;
                    }
                }
            } ورنہ {
                اگر(b>=0xe0) {
                    اگر( /* handle U+0800..U+D7FF inline */
                        b<=0xed &&  // do not assume maxFastUChar>0xd7ff
                        U8_IS_VALID_LEAD3_AND_T1(b, t1=ماخذ[0]) &&
                        (t2 = static_cast<uint8_t>(ماخذ[1] - 0x80)) <= 0x3f
                    ) {
                        c=((b&0xf)<<6)|(t1&0x3f);
                        ماخذ+=2;
                        value=DBCS_نتیجہ_FROM_UTF8(mbcs_اشاریہ, results, c, t2);
                        اگر(value==0) {
                            c=(c<<6)|t2;
                            جاؤ unassigned;
                        }
                    } ورنہ {
                        c=-1;
                    }
                } ورنہ {
                    اگر( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*ماخذ - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++ماخذ;
                        value=DBCS_نتیجہ_FROM_UTF8(mbcs_اشاریہ, results, c, t1);
                        اگر(value==0) {
                            c=(c<<6)|t1;
                            جاؤ unassigned;
                        }
                    } ورنہ {
                        c=-1;
                    }
                }

                اگر(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    old_مقصد_ULength=0;
                    مقصد_ULength=1;
                    مقصد_ULimit=U8_گنتی_بائٹیں_NON_ASCII(b);
                    c=b;
مزید_بائٹیں:
                    جب(مقصد_ULength<مقصد_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        اگر(ماخذ<(uint8_t *)p_مقصد_UArgs->ماخذ_حد_بندی) {
                            b=*ماخذ;
                            اگر(icu::UTF8::isValidTrail(c, b, مقصد_ULength, مقصد_ULimit)) {
                                ++ماخذ;
                                ++مقصد_ULength;
                                c=(c<<6)+b;
                            } ورنہ {
                                توڑو; /* sequence too short, stop with toULength<toULimit */
                            }
                        } ورنہ {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            ماخذ-=(مقصد_ULength-old_مقصد_ULength);
                            جب(old_مقصد_ULength<مقصد_ULength) {
                                utf8->مقصد_UBytes[old_مقصد_ULength++]=*ماخذ++;
                            }
                            utf8->مقصد_Unicode_حالت=c;
                            utf8->مقصد_ULength=مقصد_ULength;
                            utf8->منوال=مقصد_ULimit;
                            p_مقصد_UArgs->ماخذ=(حرف *)ماخذ;
                            pFromUArgs->ہدف = reinterpret_cast<حرف*>(ہدف);
                            واپس;
                        }
                    }

                    اگر(مقصد_ULength==مقصد_ULimit) {
                        c-=utf8_offsets[مقصد_ULength];
                        اگر(مقصد_ULength<=3) {  /* BMP */
                            stage2_اندراج=MBCS_STAGE_2_FROM_U(جدول, c);
                        } ورنہ {
                            /* supplementary code point */
                            اگر(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                stage2_اندراج=0;
                            } ورنہ {
                                stage2_اندراج=MBCS_STAGE_2_FROM_U(جدول, c);
                            }
                        }
                    } ورنہ {
                        /* error handling: illegal UTF-8 byte sequence */
                        ماخذ-=(مقصد_ULength-old_مقصد_ULength);
                        جب(old_مقصد_ULength<مقصد_ULength) {
                            utf8->مقصد_UBytes[old_مقصد_ULength++]=*ماخذ++;
                        }
                        utf8->مقصد_ULength=مقصد_ULength;
                        p_مقصد_UArgs->ماخذ=(حرف *)ماخذ;
                        pFromUArgs->ہدف = reinterpret_cast<حرف*>(ہدف);
                        *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                        واپس;
                    }

                    /* get the bytes and the length for the output */
                    /* MBCS_OUTPUT_2 */
                    value=MBCS_قدر_2_FROM_STAGE_2(results, stage2_اندراج, c);

                    /* is this code point assigned, or do we use fallbacks? */
                    اگر(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_اندراج, c) ||
                         (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                    ) {
                        جاؤ unassigned;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            اگر(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *ہدف++ = static_cast<uint8_t>(value);
                --ہدف_استعداد;
            } ورنہ /* length==2 */ {
                *ہدف++ = static_cast<uint8_t>(value >> 8);
                اگر(2<=ہدف_استعداد) {
                    *ہدف++ = static_cast<uint8_t>(value);
                    ہدف_استعداد-=2;
                } ورنہ {
                    cnv->char_Error_بفر[0] = static_cast<حرف>(value);
                    cnv->char_Error_بفر_لمبائی=1;

                    /* target overflow */
                    *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                    توڑو;
                }
            }
            جاری;

unassigned:
            {
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                ساکن ثابت char16_t nul=0;
                ثابت char16_t *no_ماخذ=&nul;
                c=_extFromU(cnv, cnv->shared_ڈیٹا,
                            c, &no_ماخذ, no_ماخذ,
                            &ہدف, ہدف+ہدف_استعداد,
                            خالی_اشاریہ, -1,
                            pFromUArgs->flush,
                            p_Error_رمز);

                اگر(U_FAILURE(*p_Error_رمز)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    توڑو;
                } ورنہ اگر(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *p_Error_رمز=U_USING_DEFAULT_WARNING;
                    توڑو;
                } ورنہ {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    ہدف_استعداد = static_cast<int32_t>(pFromUArgs->ہدف_حد_بندی - reinterpret_cast<حرف*>(ہدف));
                    جاری;
                }
            }
        } ورنہ {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    اگر(U_SUCCESS(*p_Error_رمز) &&
            cnv->preFromUFirstCP<0 &&
            ماخذ<(ماخذ_حد_بندی=(uint8_t *)p_مقصد_UArgs->ماخذ_حد_بندی)) {
        c=utf8->مقصد_UBytes[0]=b=*ماخذ++;
        مقصد_ULength=1;
        مقصد_ULimit=U8_گنتی_بائٹیں(b);
        جب(ماخذ<ماخذ_حد_بندی) {
            utf8->مقصد_UBytes[مقصد_ULength++]=b=*ماخذ++;
            c=(c<<6)+b;
        }
        utf8->مقصد_Unicode_حالت=c;
        utf8->مقصد_ULength=مقصد_ULength;
        utf8->منوال=مقصد_ULimit;
    }

    /* write back the updated pointers */
    p_مقصد_UArgs->ماخذ=(حرف *)ماخذ;
    pFromUArgs->ہدف = reinterpret_cast<حرف*>(ہدف);
}

/* miscellaneous ------------------------------------------------------------ */

ساکن خالی U_CALLCONV
ucnv_MBCSGetStarters(ثابت UConverter* cnv,
                 UBool starters[256],
                 UError_رمز *) {
    ثابت int32_t *state0;
    صحیح i;

    state0=cnv->shared_ڈیٹا->mbcs.state_جدول[cnv->shared_ڈیٹا->mbcs.dbcsOnlyState];
    برائے(i=0; i<256; ++i) {
        /* all bytes that cause a state transition from state 0 are lead bytes */
        starters[i] = static_cast<UBool>(MBCS_اندراج_IS_TRANSITION(state0[i]));
    }
}

/*
 * This is an internal function that allows other converter implementations
 * to check whether a byte is a lead byte.
 */
U_CFUNC UBool
ucnv_MBCSIsLeadByte(UConverter_Shared_ڈیٹا *shared_ڈیٹا, حرف byte) {
    واپس MBCS_اندراج_IS_TRANSITION(shared_ڈیٹا->mbcs.state_جدول[0][(uint8_t)byte]);
}

ساکن خالی U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_دلائل *p_دلائل,
              int32_t انحراف_اشاریہ,
              UError_رمز *p_Error_رمز) {
    UConverter *cnv=p_دلائل->converter;
    حرف *p, *subchar;
    حرف بفر[4];
    int32_t length;

    /* first, select between subChar and subChar1 */
    اگر( cnv->subChar1!=0 &&
        (cnv->shared_ڈیٹا->mbcs.extIndexes!=خالی_اشاریہ ?
            cnv->useSubChar1 :
            (cnv->invalid_UChar_بفر[0]<=0xff))
    ) {
        /* select subChar1 if it is set (not 0) and the unmappable Unicode code point is up to U+00ff (IBM MBCS behavior) */
        subchar = reinterpret_cast<حرف*>(&cnv->subChar1);
        length=1;
    } ورنہ {
        /* select subChar in all other cases */
        subchar = reinterpret_cast<حرف*>(cnv->subChars);
        length=cnv->subCharLen;
    }

    /* reset the selector for the next code point */
    cnv->useSubChar1=جھوٹ;

    اگر (cnv->shared_ڈیٹا->mbcs.outputType == MBCS_OUTPUT_2_SISO) {
        p=بفر;

        /* fromUnicodeStatus contains prevLength */
        انتخاب(length) {
        حالت 1:
            اگر(cnv->from_Unicode_حالت==2) {
                /* DBCS mode and SBCS sub char: change to SBCS */
                cnv->from_Unicode_حالت=1;
                *p++=UCNV_SI;
            }
            *p++=subchar[0];
            توڑو;
        حالت 2:
            اگر(cnv->from_Unicode_حالت<=1) {
                /* SBCS mode and DBCS sub char: change to DBCS */
                cnv->from_Unicode_حالت=2;
                *p++=UCNV_SO;
            }
            *p++=subchar[0];
            *p++=subchar[1];
            توڑو;
        طےشدہ:
            *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
            واپس;
        }
        subchar=بفر;
        length = static_cast<int32_t>(p - بفر);
    }

    ucnv_cb_From_UWrite_بائٹیں(p_دلائل, subchar, length, انحراف_اشاریہ, p_Error_رمز);
}

U_CFUNC UConverterType
ucnv_MBCSGetType(ثابت UConverter* converter) {
    /* SBCS, DBCS, and EBCDIC_STATEFUL are replaced by MBCS, but here we cheat a little */
    اگر(converter->shared_ڈیٹا->mbcs.گنتی_حالتیں==1) {
        واپس (UConverterType)UCNV_SBCS;
    } ورنہ اگر((converter->shared_ڈیٹا->mbcs.outputType&0xff)==MBCS_OUTPUT_2_SISO) {
        واپس (UConverterType)UCNV_EBCDIC_STATEFUL;
    } ورنہ اگر(converter->shared_ڈیٹا->static_ڈیٹا->min_بائٹیں_فی_Char==2 && converter->shared_ڈیٹا->static_ڈیٹا->max_بائٹیں_فی_Char==2) {
        واپس (UConverterType)UCNV_DBCS;
    }
    واپس (UConverterType)UCNV_MBCS;
}

#اگر_ختم /* #if !UCONFIG_NO_LEGACY_CONVERSION */
