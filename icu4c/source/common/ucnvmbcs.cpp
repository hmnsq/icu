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
#تعریف_کن MBCS_UNROLL_SINGLE_مقصد_BMP 1
#تعریف_کن MBCS_UNROLL_SINGLE_FROM_BMP 0

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
UConverter_Enum_مقصد_UCallback(ثابت فراغ *context, uint32_t value, UChar32 کد_Points[32]);

ساكن فراغ U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_داده *shared_داده,
          UConverter_بارگیری_آرگومانها *p_آرگومانها,
          ثابت uint8_t *raw,
          UError_کد *p_Error_کد);

ساكن فراغ U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_داده *shared_داده);

ساكن فراغ U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_بارگیری_آرگومانها *p_آرگومانها,
              UError_کد *p_Error_کد);

ساكن UChar32 U_CALLCONV
ucnv_MBCSGet_بعدی_UChar(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                  UError_کد *p_Error_کد);

ساكن فراغ U_CALLCONV
ucnv_MBCSGetStarters(ثابت UConverter* cnv,
                 UBool starters[256],
                 UError_کد *p_Error_کد);

U_CDECL_BEGIN
ساكن ثابت حرف* U_CALLCONV
ucnv_MBCSGetName(ثابت UConverter *cnv);
U_CDECL_انتها

ساكن فراغ U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_آرگومانها *p_آرگومانها,
              int32_t جابه_جایی_شاخص,
              UError_کد *p_Error_کد);

ساكن UChar32 U_CALLCONV
ucnv_MBCSGet_بعدی_UChar(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                  UError_کد *p_Error_کد);

ساكن فراغ U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_آرگومانها *pFromUArgs,
                  UConverter_مقصد_Unicode_آرگومانها *p_مقصد_UArgs,
                  UError_کد *p_Error_کد);

ساكن فراغ U_CALLCONV
ucnv_MBCSGet_Unicode_مجموعه(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverter_Unicode_مجموعه which,
                   UError_کد *p_Error_کد);

ساكن فراغ U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_آرگومانها *pFromUArgs,
                  UConverter_مقصد_Unicode_آرگومانها *p_مقصد_UArgs,
                  UError_کد *p_Error_کد);

ساكن ثابت UConverterImpl _SBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    اشاره_تهی,
    اشاره_تهی,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_بعدی_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    اشاره_تهی,
    ucnv_MBCSGet_Unicode_مجموعه,

    اشاره_تهی,
    ucnv_SBCSFromUTF8
};

ساكن ثابت UConverterImpl _DBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    اشاره_تهی,
    اشاره_تهی,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_بعدی_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    اشاره_تهی,
    ucnv_MBCSGet_Unicode_مجموعه,

    اشاره_تهی,
    ucnv_DBCSFromUTF8
};

ساكن ثابت UConverterImpl _MBCSImpl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    اشاره_تهی,
    اشاره_تهی,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_بعدی_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    اشاره_تهی,
    ucnv_MBCSGet_Unicode_مجموعه,
    اشاره_تهی,
    اشاره_تهی
};

/* Static data is in tools/makeconv/ucnvstat.c for data-based
 * converters. Be sure to update it as well.
 */

ثابت UConverter_Shared_داده _MBCSData={
    حجم(UConverter_Shared_داده), 1,
    اشاره_تهی, اشاره_تهی, خطأ, درست, &_MBCSImpl,
    0, UCNV_MBCS_جدول_INITIALIZER
};


/* GB 18030 data ------------------------------------------------------------ */

/* helper macros for linear values for GB 18030 four-byte sequences */
#تعریف_کن خطی_18030(a, b, c, d) ((((a)*10+(b))*126L+(c))*10L+(d))

#تعریف_کن خطی_18030_پایه خطی_18030(0x81, 0x30, 0x81, 0x30)

#تعریف_کن خطی(x) خطی_18030(x>>24, (x>>16)&0xff, (x>>8)&0xff, x&0xff)

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
#تعریف_کن _MBCS_OPTION_GB18030 0x8000

/* bit flag for UConverter.options indicating KEIS,JEF,JIF special handling */
#تعریف_کن _MBCS_OPTION_KEIS 0x01000
#تعریف_کن _MBCS_OPTION_JEF  0x02000
#تعریف_کن _MBCS_OPTION_JIPS 0x04000

#تعریف_کن KEIS_SO_CHAR_1 0x0A
#تعریف_کن KEIS_SO_CHAR_2 0x42
#تعریف_کن KEIS_SI_CHAR_1 0x0A
#تعریف_کن KEIS_SI_CHAR_2 0x41

#تعریف_کن JEF_SO_CHAR 0x28
#تعریف_کن JEF_SI_CHAR 0x29

#تعریف_کن JIPS_SO_CHAR_1 0x1A
#تعریف_کن JIPS_SO_CHAR_2 0x70
#تعریف_کن JIPS_SI_CHAR_1 0x1A
#تعریف_کن JIPS_SI_CHAR_2 0x71

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
enum_مقصد_U(UConverterMBCSTable *mbcs_جدول, int8_t stateProps[],
        int32_t state, uint32_t جابه_جایی,
        uint32_t value,
        UConverter_Enum_مقصد_UCallback *callback, ثابت فراغ *context,
        UError_کد *p_Error_کد) {
    UChar32 کد_Points[32];
    ثابت int32_t *ردیف;
    ثابت uint16_t *unicode_کد_Units;
    UChar32 any_کد_Points;
    int32_t b, حد;

    ردیف=mbcs_جدول->state_جدول[state];
    unicode_کد_Units=mbcs_جدول->unicode_کد_Units;

    value<<=8;
    any_کد_Points=-1;  /* becomes non-negative if there is a mapping */

    b=(stateProps[state]&0x38)<<2;
    إذا(b==0 && stateProps[state]>=0x40) {
        /* skip byte sequences with leading zeros because they are not stored in the fromUnicode table */
        کد_Points[0]=U_SENTINEL;
        b=1;
    }
    حد=((stateProps[state]&7)+1)<<5;
    طالما(b<حد) {
        int32_t مدخل=ردیف[b];
        إذا(MBCS_مدخل_IS_TRANSITION(مدخل)) {
            int32_t بعدی_State=MBCS_مدخل_TRANSITION_STATE(مدخل);
            إذا(stateProps[بعدی_State]>=0) {
                /* recurse to a state with non-ignorable actions */
                إذا(!enum_مقصد_U(
                        mbcs_جدول, stateProps, بعدی_State,
                        جابه_جایی+MBCS_مدخل_TRANSITION_جابه_جایی(مدخل),
                        value | static_cast<uint32_t>(b),
                        callback, context,
                        p_Error_کد)) {
                    أرجع خطأ;
                }
            }
            کد_Points[b&0x1f]=U_SENTINEL;
        } وإلا {
            UChar32 c;
            int32_t action;

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=MBCS_مدخل_FINAL_ACTION(مدخل);
            إذا(action==MBCS_STATE_VALID_مستقیم_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            } وإلا إذا(action==MBCS_STATE_VALID_16) {
                int32_t final_جابه_جایی=جابه_جایی+MBCS_مدخل_FINAL_مقدار_16(مدخل);
                c=unicode_کد_Units[final_جابه_جایی];
                إذا(c<0xfffe) {
                    /* output BMP code point */
                } وإلا {
                    c=U_SENTINEL;
                }
            } وإلا إذا(action==MBCS_STATE_VALID_16_PAIR) {
                int32_t final_جابه_جایی=جابه_جایی+MBCS_مدخل_FINAL_مقدار_16(مدخل);
                c=unicode_کد_Units[final_جابه_جایی++];
                إذا(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } وإلا إذا(c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_کد_Units[final_جابه_جایی]+(0x10000-0xdc00);
                } وإلا إذا(c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_کد_Units[final_جابه_جایی];
                } وإلا {
                    c=U_SENTINEL;
                }
            } وإلا إذا(action==MBCS_STATE_VALID_مستقیم_20) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_مدخل_FINAL_مقدار(مدخل) + 0x10000);
            } وإلا {
                c=U_SENTINEL;
            }

            کد_Points[b&0x1f]=c;
            any_کد_Points&=c;
        }
        إذا(((++b)&0x1f)==0) {
            إذا(any_کد_Points>=0) {
                إذا (!callback(context, value | static_cast<uint32_t>(b - 0x20), کد_Points)) {
                    أرجع خطأ;
                }
                any_کد_Points=-1;
            }
        }
    }
    أرجع درست;
}

/*
 * Only called if stateProps[state]==-1.
 * A recursive call may do stateProps[state]|=0x40 if this state is the target of an
 * MBCS_STATE_CHANGE_ONLY.
 */
ساكن int8_t
getStateProp(ثابت int32_t (*state_جدول)[256], int8_t stateProps[], صحيح state) {
    ثابت int32_t *ردیف;
    int32_t min, max, مدخل, بعدی_State;

    ردیف=state_جدول[state];
    stateProps[state]=0;

    /* find first non-ignorable state */
    لكل(min=0;; ++min) {
        مدخل=ردیف[min];
        بعدی_State=MBCS_مدخل_STATE(مدخل);
        إذا(stateProps[بعدی_State]==-1) {
            getStateProp(state_جدول, stateProps, بعدی_State);
        }
        إذا(MBCS_مدخل_IS_TRANSITION(مدخل)) {
            إذا(stateProps[بعدی_State]>=0) {
                اكسر;
            }
        } وإلا إذا(MBCS_مدخل_FINAL_ACTION(مدخل)<MBCS_STATE_UNASSIGNED) {
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
        مدخل=ردیف[max];
        بعدی_State=MBCS_مدخل_STATE(مدخل);
        إذا(stateProps[بعدی_State]==-1) {
            getStateProp(state_جدول, stateProps, بعدی_State);
        }
        إذا(MBCS_مدخل_IS_TRANSITION(مدخل)) {
            إذا(stateProps[بعدی_State]>=0) {
                اكسر;
            }
        } وإلا إذا(MBCS_مدخل_FINAL_ACTION(مدخل)<MBCS_STATE_UNASSIGNED) {
            اكسر;
        }
    }
    stateProps[state] |= static_cast<int8_t>(max >> 5);

    /* recurse further and collect direct-state information */
    طالما(min<=max) {
        مدخل=ردیف[min];
        بعدی_State=MBCS_مدخل_STATE(مدخل);
        إذا(stateProps[بعدی_State]==-1) {
            getStateProp(state_جدول, stateProps, بعدی_State);
        }
        إذا(MBCS_مدخل_IS_FINAL(مدخل)) {
            stateProps[بعدی_State]|=0x40;
            إذا(MBCS_مدخل_FINAL_ACTION(مدخل)<=MBCS_STATE_FALLBACK_مستقیم_20) {
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
ucnv_MBCSEnum_مقصد_Unicode(UConverterMBCSTable *mbcs_جدول,
                       UConverter_Enum_مقصد_UCallback *callback, ثابت فراغ *context,
                       UError_کد *p_Error_کد) {
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
    int8_t stateProps[MBCS_MAX_STATE_شمار];
    int32_t state;

    uprv_memset(stateProps, -1, حجم(stateProps));

    /* recurse from state 0 and set all stateProps */
    getStateProp(mbcs_جدول->state_جدول, stateProps, 0);

    لكل(state=0; state<mbcs_جدول->شمار_حالتها; ++state) {
        /*if(stateProps[state]==-1) {
            printf("unused/unreachable <icu:state> %d\n", state);
        }*/
        إذا(stateProps[state]>=0x40) {
            /* start from each direct state */
            enum_مقصد_U(
                mbcs_جدول, stateProps, state, 0, 0,
                callback, context,
                p_Error_کد);
        }
    }
}

U_CFUNC فراغ 
ucnv_MBCSGet_Filtered_Unicode_مجموعه_For_Unicode(ثابت UConverter_Shared_داده *shared_داده,
                                         ثابت USetAdder *sa,
                                         UConverter_Unicode_مجموعه which,
                                         UConverter_مجموعه_Filter filter,
                                         UError_کد *p_Error_کد) {
    ثابت UConverterMBCSTable *mbcs_جدول;
    ثابت uint16_t *جدول;

    uint32_t st3;
    uint16_t st1, maxStage1, st2;

    UChar32 c;

    /* enumerate the from-Unicode trie table */
    mbcs_جدول=&shared_داده->mbcs;
    جدول=mbcs_جدول->from_Unicode_جدول;
    إذا(mbcs_جدول->unicode_ماسک&UCNV_HAS_SUPPLEMENTARY) {
        maxStage1=0x440;
    } وإلا {
        maxStage1=0x40;
    }

    c=0; /* keep track of the current code point while enumerating */

    إذا(mbcs_جدول->outputType==MBCS_OUTPUT_1) {
        ثابت uint16_t *stage2, *stage3, *results;
        uint16_t min_مقدار;

        results=(ثابت uint16_t *)mbcs_جدول->from_Unicode_بایتها;

        /*
         * Set a threshold variable for selecting which mappings to use.
         * See ucnv_MBCSSingleFromBMPWithOffsets() and
         * MBCS_SINGLE_RESULT_FROM_U() for details.
         */
        إذا(which==UCNV_ROUNDTRIP_مجموعه) {
            /* use only roundtrips */
            min_مقدار=0xf00;
        } وإلا /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
            /* use all roundtrip and fallback results */
            min_مقدار=0x800;
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
                            إذا(*stage3++>=min_مقدار) {
                                sa->جمع_کن(sa->مجموعه, c);
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
        ثابت uint8_t *stage3, *بایتها;
        uint32_t st3Multiplier;
        uint32_t value;
        UBool useFallback;

        بایتها=mbcs_جدول->from_Unicode_بایتها;

        useFallback = which == UCNV_ROUNDTRIP_AND_FALLBACK_مجموعه;

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
                        stage3=بایتها+st3Multiplier*16*(uint32_t)(uint16_t)st3;

                        /* get the roundtrip flags for the stage 3 block */
                        st3>>=16;

                        /*
                         * Add code points for which the roundtrip flag is set,
                         * or which map to non-zero bytes if we use fallbacks.
                         * See ucnv_MBCSFromUnicodeWithOffsets() for details.
                         */
                        بدل(filter) {
                        حالة UCNV_مجموعه_FILTER_NONE:
                            افعل {
                                إذا(st3&1) {
                                    sa->جمع_کن(sa->مجموعه, c);
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
                                        sa->جمع_کن(sa->مجموعه, c);
                                    }
                                }
                                st3>>=1;
                            } طالما((++c&0xf)!=0);
                            اكسر;
                        حالة UCNV_مجموعه_FILTER_DBCS_ONLY:
                             /* Ignore single-byte results (<0x100). */
                            افعل {
                                إذا(((st3&1)!=0 || useFallback) && *((ثابت uint16_t *)stage3)>=0x100) {
                                    sa->جمع_کن(sa->مجموعه, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } طالما((++c&0xf)!=0);
                            اكسر;
                        حالة UCNV_مجموعه_FILTER_2022_CN:
                             /* Only add code points that map to CNS 11643 planes 1 & 2 for non-EXT ISO-2022-CN. */
                            افعل {
                                إذا(((st3&1)!=0 || useFallback) && ((value=*stage3)==0x81 || value==0x82)) {
                                    sa->جمع_کن(sa->مجموعه, c);
                                }
                                st3>>=1;
                                stage3+=3;  /* +=st3Multiplier */
                            } طالما((++c&0xf)!=0);
                            اكسر;
                        حالة UCNV_مجموعه_FILTER_SJIS:
                             /* Only add code points that map to Shift-JIS codes corresponding to JIS X 0208. */
                            افعل {
                                إذا(((st3&1)!=0 || useFallback) && (value=*((ثابت uint16_t *)stage3))>=0x8140 && value<=0xeffc) {
                                    sa->جمع_کن(sa->مجموعه, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } طالما((++c&0xf)!=0);
                            اكسر;
                        حالة UCNV_مجموعه_FILTER_GR94_DBCS:
                            /* Only add code points that map to ISO 2022 GR 94 DBCS codes (each byte A1..FE). */
                            افعل {
                                إذا( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((ثابت uint16_t *)stage3)) - 0xa1a1)<=(0xfefe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->جمع_کن(sa->مجموعه, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } طالما((++c&0xf)!=0);
                            اكسر;
                        حالة UCNV_مجموعه_FILTER_HZ:
                            /* Only add code points that are suitable for HZ DBCS (lead byte A1..FD). */
                            افعل {
                                إذا( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((ثابت uint16_t *)stage3))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->جمع_کن(sa->مجموعه, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } طالما((++c&0xf)!=0);
                            اكسر;
                        افتراضي:
                            *p_Error_کد=U_داخلی_برنامه_ERROR;
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

    ucnv_ext_Get_Unicode_مجموعه(shared_داده, sa, which, filter, p_Error_کد);
}

U_CFUNC فراغ
ucnv_MBCSGet_Unicode_مجموعه_For_Unicode(ثابت UConverter_Shared_داده *shared_داده,
                                 ثابت USetAdder *sa,
                                 UConverter_Unicode_مجموعه which,
                                 UError_کد *p_Error_کد) {
    ucnv_MBCSGet_Filtered_Unicode_مجموعه_For_Unicode(
        shared_داده, sa, which,
        shared_داده->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ?
            UCNV_مجموعه_FILTER_DBCS_ONLY :
            UCNV_مجموعه_FILTER_NONE,
        p_Error_کد);
}

ساكن فراغ U_CALLCONV
ucnv_MBCSGet_Unicode_مجموعه(ثابت UConverter *cnv,
                   ثابت USetAdder *sa,
                   UConverter_Unicode_مجموعه which,
                   UError_کد *p_Error_کد) {
    إذا(cnv->options&_MBCS_OPTION_GB18030) {
        sa->جمع_کن_Range(sa->مجموعه, 0, 0xd7ff);
        sa->جمع_کن_Range(sa->مجموعه, 0xe000, 0x10ffff);
    } وإلا {
        ucnv_MBCSGet_Unicode_مجموعه_For_Unicode(cnv->shared_داده, sa, which, p_Error_کد);
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
_extFromU(UConverter *cnv, ثابت UConverter_Shared_داده *shared_داده,
          UChar32 cp,
          ثابت char16_t **منبع, ثابت char16_t *منبع_حد,
          uint8_t **هدف, ثابت uint8_t *هدف_حد,
          int32_t **offsets, int32_t منبع_شاخص,
          UBool flush,
          UError_کد *p_Error_کد) {
    ثابت int32_t *cx;

    cnv->useSubChar1=خطأ;

    إذا( (cx=shared_داده->mbcs.extIndexes)!=اشاره_تهی &&
        ucnv_ext_Initial_تطابق_From_U(
            cnv, cx,
            cp, منبع, منبع_حد,
            reinterpret_cast<حرف**>(هدف), reinterpret_cast<ثابت حرف*>(هدف_حد),
            offsets, منبع_شاخص,
            flush,
            p_Error_کد)
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
                uint32_t خطی_2;
                حرف بایتها[4];

                /* get the linear value of the first GB 18030 code in this range */
                خطی_2=range[2]-خطی_18030_پایه;

                /* add the offset from the beginning of the range */
                خطی_2 += (static_cast<uint32_t>(cp) - range[0]);

                /* turn this into a four-byte sequence */
                بایتها[3] = static_cast<حرف>(0x30 + خطی_2 % 10); خطی_2 /= 10;
                بایتها[2] = static_cast<حرف>(0x81 + خطی_2 % 126); خطی_2 /= 126;
                بایتها[1] = static_cast<حرف>(0x30 + خطی_2 % 10); خطی_2 /= 10;
                بایتها[0] = static_cast<حرف>(0x81 + خطی_2);

                /* output this sequence */
                ucnv_from_UWrite_بایتها(cnv,
                                     بایتها, 4, reinterpret_cast<حرف**>(هدف), reinterpret_cast<ثابت حرف*>(هدف_حد),
                                     offsets, منبع_شاخص, p_Error_کد);
                أرجع 0;
            }
        }
    }

    /* no mapping */
    *p_Error_کد=U_INVALID_CHAR_یافت;
    أرجع cp;
}

/*
 * Input sequence: cnv->toUBytes[0..length[
 * @return if(U_FAILURE) return the length (toULength, byteIndex) for the input
 *         else return 0 after output has been written to the target
 */
ساكن int8_t
ext_مقصد_U(UConverter *cnv, ثابت UConverter_Shared_داده *shared_داده,
        int8_t length,
        ثابت uint8_t **منبع, ثابت uint8_t *منبع_حد,
        char16_t **هدف, ثابت char16_t *هدف_حد,
        int32_t **offsets, int32_t منبع_شاخص,
        UBool flush,
        UError_کد *p_Error_کد) {
    ثابت int32_t *cx;

    إذا( (cx=shared_داده->mbcs.extIndexes)!=اشاره_تهی &&
        ucnv_ext_Initial_تطابق_مقصد_U(
            cnv, cx,
            length, reinterpret_cast<ثابت حرف**>(منبع), reinterpret_cast<ثابت حرف*>(منبع_حد),
            هدف, هدف_حد,
            offsets, منبع_شاخص,
            flush,
            p_Error_کد)
    ) {
        أرجع 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    إذا(length==4 && (cnv->options&_MBCS_OPTION_GB18030)!=0) {
        ثابت uint32_t *range;
        uint32_t خطی_2;
        int32_t i;

        خطی_2=خطی_18030(cnv->مقصد_UBytes[0], cnv->مقصد_UBytes[1], cnv->مقصد_UBytes[2], cnv->مقصد_UBytes[3]);
        range=gb18030Ranges[0];
        لكل(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            إذا(range[2]<=خطی_2 && خطی_2<=range[3]) {
                /* found the sequence, output the Unicode code point for it */
                *p_Error_کد=U_ZERO_ERROR;

                /* add the linear difference between the input and start sequences to the start code point */
                خطی_2=range[0]+(خطی_2-range[2]);

                /* output this code point */
                ucnv_مقصد_UWrite_کد_نقطه(cnv, خطی_2, هدف, هدف_حد, offsets, منبع_شاخص, p_Error_کد);

                أرجع 0;
            }
        }
    }

    /* no mapping */
    *p_Error_کد=U_INVALID_CHAR_یافت;
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
#تعریف_کن EBCDIC_LF 0x25
#تعریف_کن EBCDIC_NL 0x15

/* standard EBCDIC codes with roundtrip flag as stored in Unicode-to-single-byte tables */
#تعریف_کن EBCDIC_RT_LF 0xf25
#تعریف_کن EBCDIC_RT_NL 0xf15

/* Unicode code points */
#تعریف_کن U_LF 0x0a
#تعریف_کن U_NL 0x85

ساكن UBool
_EBCDICSwapLFNL(UConverter_Shared_داده *shared_داده, UError_کد *p_Error_کد) {
    UConverterMBCSTable *mbcs_جدول;

    ثابت uint16_t *جدول, *results;
    ثابت uint8_t *بایتها;

    int32_t (*new_State_جدول)[256];
    uint16_t *newResults;
    uint8_t *p;
    حرف *name;

    uint32_t stage2_مدخل;
    uint32_t size, sizeofFromUBytes;

    mbcs_جدول=&shared_داده->mbcs;

    جدول=mbcs_جدول->from_Unicode_جدول;
    بایتها=mbcs_جدول->from_Unicode_بایتها;
    results = reinterpret_cast<ثابت uint16_t*>(بایتها);

    /*
     * Check that this is an EBCDIC table with SBCS portion -
     * SBCS or EBCDIC_STATEFUL with standard EBCDIC LF and NL mappings.
     *
     * If not, ignore the option. Options are always ignored if they do not apply.
     */
    إذا(!(
         (mbcs_جدول->outputType==MBCS_OUTPUT_1 || mbcs_جدول->outputType==MBCS_OUTPUT_2_SISO) &&
         mbcs_جدول->state_جدول[0][EBCDIC_LF]==MBCS_مدخل_FINAL(0, MBCS_STATE_VALID_مستقیم_16, U_LF) &&
         mbcs_جدول->state_جدول[0][EBCDIC_NL]==MBCS_مدخل_FINAL(0, MBCS_STATE_VALID_مستقیم_16, U_NL)
    )) {
        أرجع خطأ;
    }

    إذا(mbcs_جدول->outputType==MBCS_OUTPUT_1) {
        إذا(!(
             EBCDIC_RT_LF==MBCS_SINGLE_نتیجه_FROM_U(جدول, results, U_LF) &&
             EBCDIC_RT_NL==MBCS_SINGLE_نتیجه_FROM_U(جدول, results, U_NL)
        )) {
            أرجع خطأ;
        }
    } وإلا /* MBCS_OUTPUT_2_SISO */ {
        stage2_مدخل=MBCS_STAGE_2_FROM_U(جدول, U_LF);
        إذا(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_مدخل, U_LF)!=0 &&
             EBCDIC_LF==MBCS_مقدار_2_FROM_STAGE_2(بایتها, stage2_مدخل, U_LF)
        )) {
            أرجع خطأ;
        }

        stage2_مدخل=MBCS_STAGE_2_FROM_U(جدول, U_NL);
        إذا(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_مدخل, U_NL)!=0 &&
             EBCDIC_NL==MBCS_مقدار_2_FROM_STAGE_2(بایتها, stage2_مدخل, U_NL)
        )) {
            أرجع خطأ;
        }
    }

    إذا(mbcs_جدول->from_UBytes_درازی>0) {
        /*
         * We _know_ the number of bytes in the fromUnicodeBytes array
         * starting with header.version 4.1.
         */
        sizeofFromUBytes=mbcs_جدول->from_UBytes_درازی;
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
        *p_Error_کد=U_INVALID_FORMAT_ERROR;
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
        mbcs_جدول->شمار_حالتها*1024+
        sizeofFromUBytes+
        UCNV_MAX_CONVERTER_NAME_درازی+20;
    p = static_cast<uint8_t*>(uprv_malloc(size));
    إذا(p==اشاره_تهی) {
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        أرجع خطأ;
    }

    /* copy and modify the to-Unicode state table */
    new_State_جدول = reinterpret_cast<int32_t(*)[256]>(p);
    uprv_memcpy(new_State_جدول, mbcs_جدول->state_جدول, mbcs_جدول->شمار_حالتها*1024);

    new_State_جدول[0][EBCDIC_LF]=MBCS_مدخل_FINAL(0, MBCS_STATE_VALID_مستقیم_16, U_NL);
    new_State_جدول[0][EBCDIC_NL]=MBCS_مدخل_FINAL(0, MBCS_STATE_VALID_مستقیم_16, U_LF);

    /* copy and modify the from-Unicode result table */
    newResults = reinterpret_cast<uint16_t*>(new_State_جدول[mbcs_جدول->شمار_حالتها]);
    uprv_memcpy(newResults, بایتها, sizeofFromUBytes);

    /* conveniently, the table access macros work on the left side of expressions */
    إذا(mbcs_جدول->outputType==MBCS_OUTPUT_1) {
        MBCS_SINGLE_نتیجه_FROM_U(جدول, newResults, U_LF)=EBCDIC_RT_NL;
        MBCS_SINGLE_نتیجه_FROM_U(جدول, newResults, U_NL)=EBCDIC_RT_LF;
    } وإلا /* MBCS_OUTPUT_2_SISO */ {
        stage2_مدخل=MBCS_STAGE_2_FROM_U(جدول, U_LF);
        MBCS_مقدار_2_FROM_STAGE_2(newResults, stage2_مدخل, U_LF)=EBCDIC_NL;

        stage2_مدخل=MBCS_STAGE_2_FROM_U(جدول, U_NL);
        MBCS_مقدار_2_FROM_STAGE_2(newResults, stage2_مدخل, U_NL)=EBCDIC_LF;
    }

    /* set the canonical converter name */
    name = reinterpret_cast<حرف*>(newResults) + sizeofFromUBytes;
    uprv_strcpy(name, shared_داده->static_داده->name);
    uprv_strcat(name, UCNV_جابجا_LFNL_OPTION_STRING);

    /* set the pointers */
    icu::umtx_قفل(اشاره_تهی);
    إذا(mbcs_جدول->جابجا_LFNLState_جدول==اشاره_تهی) {
        mbcs_جدول->جابجا_LFNLState_جدول=new_State_جدول;
        mbcs_جدول->جابجا_LFNLFrom_Unicode_بایتها = reinterpret_cast<uint8_t*>(newResults);
        mbcs_جدول->جابجا_LFNLName=name;

        new_State_جدول=اشاره_تهی;
    }
    icu::umtx_unlock(اشاره_تهی);

    /* release the allocated memory if another thread beat us to it */
    إذا(new_State_جدول!=اشاره_تهی) {
        uprv_free(new_State_جدول);
    }
    أرجع درست;
}

/* reconstitute omitted fromUnicode data ------------------------------------ */

/* for details, compare with genmbcs.c MBCSAddFromUnicode() and transformEUC() */
ساكن UBool U_CALLCONV
writeStage3Roundtrip(ثابت فراغ *context, uint32_t value, UChar32 کد_Points[32]) {
    UConverterMBCSTable *mbcs_جدول=(UConverterMBCSTable *)context;
    ثابت uint16_t *جدول;
    uint32_t *stage2;
    uint8_t *بایتها, *p;
    UChar32 c;
    int32_t i, st3;

    جدول=mbcs_جدول->from_Unicode_جدول;
    بایتها = const_cast<uint8_t*>(mbcs_جدول->from_Unicode_بایتها);

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
        c=کد_Points[i];
        إذا(c<0) {
            استمر;
        }

        /* locate the stage 2 & 3 data */
        stage2=((uint32_t *)جدول)+جدول[c>>10]+((c>>4)&0x3f);
        p=بایتها;
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
    أرجع درست;
 }

ساكن فراغ
reconstitute_داده(UConverterMBCSTable *mbcs_جدول,
                 uint32_t stage1_درازی, uint32_t stage2_درازی,
                 uint32_t پر_Stage2_درازی,  /* lengths are numbers of units, not bytes */
                 UError_کد *p_Error_کد) {
    uint16_t *stage1;
    uint32_t *stage2;
    uint32_t داده_درازی=stage1_درازی*2+پر_Stage2_درازی*4+mbcs_جدول->from_UBytes_درازی;
    mbcs_جدول->reconstituted_داده = static_cast<uint8_t*>(uprv_malloc(داده_درازی));
    إذا(mbcs_جدول->reconstituted_داده==اشاره_تهی) {
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    uprv_memset(mbcs_جدول->reconstituted_داده, 0, داده_درازی);

    /* copy existing data and reroute the pointers */
    stage1 = reinterpret_cast<uint16_t*>(mbcs_جدول->reconstituted_داده);
    uprv_memcpy(stage1, mbcs_جدول->from_Unicode_جدول, stage1_درازی*2);

    stage2 = reinterpret_cast<uint32_t*>(stage1 + stage1_درازی);
    uprv_memcpy(stage2+(پر_Stage2_درازی-stage2_درازی),
                mbcs_جدول->from_Unicode_جدول+stage1_درازی,
                stage2_درازی*4);

    mbcs_جدول->from_Unicode_جدول=stage1;
    mbcs_جدول->from_Unicode_بایتها = reinterpret_cast<uint8_t*>(stage2 + پر_Stage2_درازی);

    /* indexes into stage 2 count from the bottom of the fromUnicodeTable */
    stage2 = reinterpret_cast<uint32_t*>(stage1);

    /* reconstitute the initial part of stage 2 from the mbcsIndex */
    {
        int32_t stage_UTF8_درازی = (static_cast<int32_t>(mbcs_جدول->maxFastUChar) + 1) >> 6;
        int32_t stage_UTF8_شاخص=0;
        int32_t st1, st2, st3, i;

        لكل(st1=0; stage_UTF8_شاخص<stage_UTF8_درازی; ++st1) {
            st2=stage1[st1];
            إذا (st2 != static_cast<int32_t>(stage1_درازی) / 2) {
                /* each stage 2 block has 64 entries corresponding to 16 entries in the mbcsIndex */
                لكل(i=0; i<16; ++i) {
                    st3=mbcs_جدول->mbcs_شاخص[stage_UTF8_شاخص++];
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
                stage_UTF8_شاخص+=16;
            }
        }
    }

    /* reconstitute fromUnicodeBytes with roundtrips from toUnicode data */
    ucnv_MBCSEnum_مقصد_Unicode(mbcs_جدول, writeStage3Roundtrip, mbcs_جدول, p_Error_کد);
}

/* MBCS setup functions ----------------------------------------------------- */

ساكن فراغ U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_داده *shared_داده,
          UConverter_بارگیری_آرگومانها *p_آرگومانها,
          ثابت uint8_t *raw,
          UError_کد *p_Error_کد) {
    UDataInfo info;
    UConverterMBCSTable *mbcs_جدول=&shared_داده->mbcs;
    _MBCSHeader *سرآیند=(_MBCSHeader *)raw;
    uint32_t جابه_جایی;
    uint32_t سرآیند_درازی;
    UBool noFromU=خطأ;

    إذا(سرآیند->نسخه[0]==4) {
        سرآیند_درازی=MBCS_سرآیند_V4_درازی;
    } وإلا إذا(سرآیند->نسخه[0]==5 && سرآیند->نسخه[1]>=3 &&
              (سرآیند->options&MBCS_OPT_UNKNOWN_INCOMPATIBLE_ماسک)==0) {
        سرآیند_درازی=سرآیند->options&MBCS_OPT_درازی_ماسک;
        noFromU = static_cast<UBool>((سرآیند->options & MBCS_OPT_NO_FROM_U) != 0);
    } وإلا {
        *p_Error_کد=U_INVALID_جدول_FORMAT;
        أرجع;
    }

    mbcs_جدول->outputType = static_cast<uint8_t>(سرآیند->flags);
    إذا(noFromU && mbcs_جدول->outputType==MBCS_OUTPUT_1) {
        *p_Error_کد=U_INVALID_جدول_FORMAT;
        أرجع;
    }

    /* extension data, header version 4.2 and higher */
    جابه_جایی=سرآیند->flags>>8;
    إذا(جابه_جایی!=0) {
        mbcs_جدول->extIndexes = reinterpret_cast<ثابت int32_t*>(raw + جابه_جایی);
    }

    إذا(mbcs_جدول->outputType==MBCS_OUTPUT_EXT_ONLY) {
        UConverter_بارگیری_آرگومانها آرگومانها_2=UCNV_بارگیری_آرگومانها_INITIALIZER;
        UConverter_Shared_داده *پایه_Shared_داده;
        ثابت int32_t *extIndexes;
        ثابت حرف *پایه_Name;

        /* extension-only file, load the base table and set values appropriately */
        إذا((extIndexes=mbcs_جدول->extIndexes)==اشاره_تهی) {
            /* extension-only file without extension */
            *p_Error_کد=U_INVALID_جدول_FORMAT;
            أرجع;
        }

        إذا(p_آرگومانها->nestedLoads!=1) {
            /* an extension table must not be loaded as a base table */
            *p_Error_کد=U_INVALID_جدول_FILE;
            أرجع;
        }

        /* load the base table */
        پایه_Name = reinterpret_cast<ثابت حرف*>(سرآیند) + سرآیند_درازی * 4;
        إذا(0==uprv_strcmp(پایه_Name, shared_داده->static_داده->name)) {
            /* forbid loading this same extension-only file */
            *p_Error_کد=U_INVALID_جدول_FORMAT;
            أرجع;
        }

        /* TODO parse package name out of the prefix of the base name in the extension .cnv file? */
        آرگومانها_2.size=حجم(UConverter_بارگیری_آرگومانها);
        آرگومانها_2.nestedLoads=2;
        آرگومانها_2.onlyTestIsLoadable=p_آرگومانها->onlyTestIsLoadable;
        آرگومانها_2.reserved=p_آرگومانها->reserved;
        آرگومانها_2.options=p_آرگومانها->options;
        آرگومانها_2.pkg=p_آرگومانها->pkg;
        آرگومانها_2.name=پایه_Name;
        پایه_Shared_داده=ucnv_بارگیری(&آرگومانها_2, p_Error_کد);
        إذا(U_FAILURE(*p_Error_کد)) {
            أرجع;
        }
        إذا( پایه_Shared_داده->static_داده->conversionType!=UCNV_MBCS ||
            پایه_Shared_داده->mbcs.پایه_Shared_داده!=اشاره_تهی
        ) {
            ucnv_unload(پایه_Shared_داده);
            *p_Error_کد=U_INVALID_جدول_FORMAT;
            أرجع;
        }
        إذا(p_آرگومانها->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            ucnv_unload(پایه_Shared_داده);
            أرجع;
        }

        /* copy the base table data */
        uprv_memcpy(mbcs_جدول, &پایه_Shared_داده->mbcs, حجم(UConverterMBCSTable));

        /* overwrite values with relevant ones for the extension converter */
        mbcs_جدول->پایه_Shared_داده=پایه_Shared_داده;
        mbcs_جدول->extIndexes=extIndexes;

        /*
         * It would be possible to share the swapLFNL data with a base converter,
         * but the generated name would have to be different, and the memory
         * would have to be free'd only once.
         * It is easier to just create the data for the extension converter
         * separately when it is requested.
         */
        mbcs_جدول->جابجا_LFNLState_جدول=اشاره_تهی;
        mbcs_جدول->جابجا_LFNLFrom_Unicode_بایتها=اشاره_تهی;
        mbcs_جدول->جابجا_LFNLName=اشاره_تهی;

        /*
         * The reconstitutedData must be deleted only when the base converter
         * is unloaded.
         */
        mbcs_جدول->reconstituted_داده=اشاره_تهی;

        /*
         * Set a special, runtime-only outputType if the extension converter
         * is a DBCS version of a base converter that also maps single bytes.
         */
        إذا( shared_داده->static_داده->conversionType==UCNV_DBCS ||
                (shared_داده->static_داده->conversionType==UCNV_MBCS &&
                 shared_داده->static_داده->min_بایتها_در_هر_Char>=2)
        ) {
            إذا(پایه_Shared_داده->mbcs.outputType==MBCS_OUTPUT_2_SISO) {
                /* the base converter is SI/SO-stateful */
                int32_t مدخل;

                /* get the dbcs state from the state table entry for SO=0x0e */
                مدخل=mbcs_جدول->state_جدول[0][0xe];
                إذا( MBCS_مدخل_IS_FINAL(مدخل) &&
                    MBCS_مدخل_FINAL_ACTION(مدخل)==MBCS_STATE_CHANGE_ONLY &&
                    MBCS_مدخل_FINAL_STATE(مدخل)!=0
                ) {
                    mbcs_جدول->dbcsOnlyState = static_cast<uint8_t>(MBCS_مدخل_FINAL_STATE(مدخل));

                    mbcs_جدول->outputType=MBCS_OUTPUT_DBCS_ONLY;
                }
            } وإلا إذا(
                پایه_Shared_داده->static_داده->conversionType==UCNV_MBCS &&
                پایه_Shared_داده->static_داده->min_بایتها_در_هر_Char==1 &&
                پایه_Shared_داده->static_داده->max_بایتها_در_هر_Char==2 &&
                mbcs_جدول->شمار_حالتها<=127
            ) {
                /* non-stateful base converter, need to modify the state table */
                int32_t (*new_State_جدول)[256];
                int32_t *state;
                int32_t i, count;

                /* allocate a new state table and copy the base state table contents */
                count=mbcs_جدول->شمار_حالتها;
                new_State_جدول = static_cast<int32_t(*)[256]>(uprv_malloc((count + 1) * 1024));
                إذا(new_State_جدول==اشاره_تهی) {
                    ucnv_unload(پایه_Shared_داده);
                    *p_Error_کد=U_حافظه_تخصیص_ERROR;
                    أرجع;
                }

                uprv_memcpy(new_State_جدول, mbcs_جدول->state_جدول, count*1024);

                /* change all final single-byte entries to go to a new all-illegal state */
                state=new_State_جدول[0];
                لكل(i=0; i<256; ++i) {
                    إذا(MBCS_مدخل_IS_FINAL(state[i])) {
                        state[i]=MBCS_مدخل_TRANSITION(count, 0);
                    }
                }

                /* build the new all-illegal state */
                state=new_State_جدول[count];
                لكل(i=0; i<256; ++i) {
                    state[i]=MBCS_مدخل_FINAL(0, MBCS_STATE_ILLEGAL, 0);
                }
                mbcs_جدول->state_جدول=(ثابت int32_t (*)[256])new_State_جدول;
                mbcs_جدول->شمار_حالتها = static_cast<uint8_t>(count + 1);
                mbcs_جدول->state_جدول_Owned=درست;

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
            *p_Error_کد=U_INVALID_جدول_FORMAT;
            أرجع;
        }
        إذا(p_آرگومانها->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            أرجع;
        }

        mbcs_جدول->شمار_حالتها = static_cast<uint8_t>(سرآیند->شمار_حالتها);
        mbcs_جدول->شمار_مقصد_UFallbacks=سرآیند->شمار_مقصد_UFallbacks;
        mbcs_جدول->state_جدول = reinterpret_cast<ثابت int32_t(*)[256]>(raw + سرآیند_درازی * 4);
        mbcs_جدول->مقصد_UFallbacks = reinterpret_cast<ثابت _MBCSToUFallback*>(mbcs_جدول->state_جدول + سرآیند->شمار_حالتها);
        mbcs_جدول->unicode_کد_Units = reinterpret_cast<ثابت uint16_t*>(raw + سرآیند->جابه_جایی_مقصد_UCode_Units);

        mbcs_جدول->from_Unicode_جدول = reinterpret_cast<ثابت uint16_t*>(raw + سرآیند->جابه_جایی_From_UTable);
        mbcs_جدول->from_Unicode_بایتها = raw + سرآیند->جابه_جایی_From_UBytes;
        mbcs_جدول->from_UBytes_درازی=سرآیند->from_UBytes_درازی;

        /*
         * converter versions 6.1 and up contain a unicodeMask that is
         * used here to select the most efficient function implementations
         */
        info.size=حجم(UDataInfo);
        udata_getInfo((UData_حافظه *)shared_داده->داده_حافظه, &info);
        إذا(info.format_نسخه[0]>6 || (info.format_نسخه[0]==6 && info.format_نسخه[1]>=1)) {
            /* mask off possible future extensions to be safe */
            mbcs_جدول->unicode_ماسک = static_cast<uint8_t>(shared_داده->static_داده->unicode_ماسک & 3);
        } وإلا {
            /* for older versions, assume worst case: contains anything possible (prevent over-optimizations) */
            mbcs_جدول->unicode_ماسک=UCNV_HAS_SUPPLEMENTARY|UCNV_HAS_SURROGATES;
        }

        /*
         * _MBCSHeader.version 4.3 adds utf8Friendly data structures.
         * Check for the header version, SBCS vs. MBCS, and for whether the
         * data structures are optimized for code points as high as what the
         * runtime code is designed for.
         * The implementation does not handle mapping tables with entries for
         * unpaired surrogates.
         */
        إذا( سرآیند->نسخه[1]>=3 &&
            (mbcs_جدول->unicode_ماسک&UCNV_HAS_SURROGATES)==0 &&
            (mbcs_جدول->شمار_حالتها==1 ?
                (سرآیند->نسخه[2]>=(SBCS_FAST_MAX>>8)) :
                (سرآیند->نسخه[2]>=(MBCS_FAST_MAX>>8))
            )
        ) {
            mbcs_جدول->utf8Friendly=درست;

            إذا(mbcs_جدول->شمار_حالتها==1) {
                /*
                 * SBCS: Stage 3 is allocated in 64-entry blocks for U+0000..SBCS_FAST_MAX or higher.
                 * Build a table with indexes to each block, to be used instead of
                 * the regular stage 1/2 table.
                 */
                int32_t i;
                لكل(i=0; i<(SBCS_FAST_حد>>6); ++i) {
                    mbcs_جدول->sbcs_شاخص[i]=mbcs_جدول->from_Unicode_جدول[mbcs_جدول->from_Unicode_جدول[i>>4]+((i<<2)&0x3c)];
                }
                /* set SBCS_FAST_MAX to reflect the reach of sbcsIndex[] even if header->version[2]>(SBCS_FAST_MAX>>8) */
                mbcs_جدول->maxFastUChar=SBCS_FAST_MAX;
            } وإلا {
                /*
                 * MBCS: Stage 3 is allocated in 64-entry blocks for U+0000..MBCS_FAST_MAX or higher.
                 * The .cnv file is prebuilt with an additional stage table with indexes
                 * to each block.
                 */
                mbcs_جدول->mbcs_شاخص = reinterpret_cast<ثابت uint16_t*>(
                    mbcs_جدول->from_Unicode_بایتها +
                     (noFromU ? 0 : mbcs_جدول->from_UBytes_درازی));
                mbcs_جدول->maxFastUChar = (static_cast<char16_t>(سرآیند->نسخه[2]) << 8) | 0xff;
            }
        }

        /* calculate a bit set of 4 ASCII characters per bit that round-trip to ASCII bytes */
        {
            uint32_t asciiRoundtrips=0xffffffff;
            int32_t i;

            لكل(i=0; i<0x80; ++i) {
                إذا(mbcs_جدول->state_جدول[0][i]!=MBCS_مدخل_FINAL(0, MBCS_STATE_VALID_مستقیم_16, i)) {
                    asciiRoundtrips &= ~(static_cast<uint32_t>(1) << (i >> 2));
                }
            }
            mbcs_جدول->asciiRoundtrips=asciiRoundtrips;
        }

        إذا(noFromU) {
            uint32_t stage1_درازی=
                mbcs_جدول->unicode_ماسک&UCNV_HAS_SUPPLEMENTARY ?
                    0x440 : 0x40;
            uint32_t stage2_درازی=
                (سرآیند->جابه_جایی_From_UBytes-سرآیند->جابه_جایی_From_UTable)/4-
                stage1_درازی/2;
            reconstitute_داده(mbcs_جدول, stage1_درازی, stage2_درازی, سرآیند->پر_Stage2_درازی, p_Error_کد);
        }
    }

    /* Set the impl pointer here so that it is set for both extension-only and base tables. */
    إذا(mbcs_جدول->utf8Friendly) {
        إذا(mbcs_جدول->شمار_حالتها==1) {
            shared_داده->impl=&_SBCSUTF8Impl;
        } وإلا {
            إذا(mbcs_جدول->outputType==MBCS_OUTPUT_2) {
                shared_داده->impl=&_DBCSUTF8Impl;
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
ucnv_MBCSUnload(UConverter_Shared_داده *shared_داده) {
    UConverterMBCSTable *mbcs_جدول=&shared_داده->mbcs;

    إذا(mbcs_جدول->جابجا_LFNLState_جدول!=اشاره_تهی) {
        uprv_free(mbcs_جدول->جابجا_LFNLState_جدول);
    }
    إذا(mbcs_جدول->state_جدول_Owned) {
        uprv_free((فراغ *)mbcs_جدول->state_جدول);
    }
    إذا(mbcs_جدول->پایه_Shared_داده!=اشاره_تهی) {
        ucnv_unload(mbcs_جدول->پایه_Shared_داده);
    }
    إذا(mbcs_جدول->reconstituted_داده!=اشاره_تهی) {
        uprv_free(mbcs_جدول->reconstituted_داده);
    }
}

ساكن فراغ U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_بارگیری_آرگومانها *p_آرگومانها,
              UError_کد *p_Error_کد) {
    UConverterMBCSTable *mbcs_جدول;
    ثابت int32_t *extIndexes;
    uint8_t outputType;
    int8_t max_بایتها_در_هر_UChar;

    إذا(p_آرگومانها->onlyTestIsLoadable) {
        أرجع;
    }

    mbcs_جدول=&cnv->shared_داده->mbcs;
    outputType=mbcs_جدول->outputType;

    إذا(outputType==MBCS_OUTPUT_DBCS_ONLY) {
        /* the swaplfnl option does not apply, remove it */
        cnv->options=p_آرگومانها->options&=~UCNV_OPTION_جابجا_LFNL;
    }

    إذا((p_آرگومانها->options&UCNV_OPTION_جابجا_LFNL)!=0) {
        /* do this because double-checked locking is broken */
        UBool isCached;

        icu::umtx_قفل(اشاره_تهی);
        isCached=mbcs_جدول->جابجا_LFNLState_جدول!=اشاره_تهی;
        icu::umtx_unlock(اشاره_تهی);

        إذا(!isCached) {
            إذا(!_EBCDICSwapLFNL(cnv->shared_داده, p_Error_کد)) {
                إذا(U_FAILURE(*p_Error_کد)) {
                    أرجع; /* something went wrong */
                }

                /* the option does not apply, remove it */
                cnv->options=p_آرگومانها->options&=~UCNV_OPTION_جابجا_LFNL;
            }
        }
    }

    إذا(uprv_strstr(p_آرگومانها->name, "18030")!=اشاره_تهی) {
        إذا(uprv_strstr(p_آرگومانها->name, "gb18030")!=اشاره_تهی || uprv_strstr(p_آرگومانها->name, "GB18030")!=اشاره_تهی) {
            /* set a flag for GB 18030 mode, which changes the callback behavior */
            cnv->options|=_MBCS_OPTION_GB18030;
        }
    } وإلا إذا((uprv_strstr(p_آرگومانها->name, "KEIS")!=اشاره_تهی) || (uprv_strstr(p_آرگومانها->name, "keis")!=اشاره_تهی)) {
        /* set a flag for KEIS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_KEIS;
    } وإلا إذا((uprv_strstr(p_آرگومانها->name, "JEF")!=اشاره_تهی) || (uprv_strstr(p_آرگومانها->name, "jef")!=اشاره_تهی)) {
        /* set a flag for JEF converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JEF;
    } وإلا إذا((uprv_strstr(p_آرگومانها->name, "JIPS")!=اشاره_تهی) || (uprv_strstr(p_آرگومانها->name, "jips")!=اشاره_تهی)) {
        /* set a flag for JIPS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JIPS;
    }

    /* fix maxBytesPerUChar depending on outputType and options etc. */
    إذا(outputType==MBCS_OUTPUT_2_SISO) {
        cnv->max_بایتها_در_هر_UChar=3; /* SO+DBCS */
    }

    extIndexes=mbcs_جدول->extIndexes;
    إذا(extIndexes!=اشاره_تهی) {
        max_بایتها_در_هر_UChar = static_cast<int8_t>(UCNV_GET_MAX_بایتها_در_هر_UCHAR(extIndexes));
        إذا(outputType==MBCS_OUTPUT_2_SISO) {
            ++max_بایتها_در_هر_UChar; /* SO + multiple DBCS */
        }

        إذا(max_بایتها_در_هر_UChar>cnv->max_بایتها_در_هر_UChar) {
            cnv->max_بایتها_در_هر_UChar=max_بایتها_در_هر_UChar;
        }
    }

#إذا 0
    /*
     * documentation of UConverter fields used for status
     * all of these fields are (re)set to 0 by ucnv_bld.c and ucnv_reset()
     */

    /* toUnicode */
    cnv->مقصد_Unicode_وضعیت=0;     /*جابه_جایی*/
    cnv->نما=0;                /* state */
    cnv->مقصد_ULength=0;           /* byteIndex */

    /* fromUnicode */
    cnv->fromUChar32=0;
    cnv->from_Unicode_وضعیت=1;   /* prevLength */
#پایان_اگر
}

U_CDECL_BEGIN

ساكن ثابت حرف* U_CALLCONV
ucnv_MBCSGetName(ثابت UConverter *cnv) {
    إذا((cnv->options&UCNV_OPTION_جابجا_LFNL)!=0 && cnv->shared_داده->mbcs.جابجا_LFNLName!=اشاره_تهی) {
        أرجع cnv->shared_داده->mbcs.جابجا_LFNLName;
    } وإلا {
        أرجع cnv->shared_داده->static_داده->name;
    }
}
U_CDECL_انتها


/* MBCS-to-Unicode conversion functions ------------------------------------- */

ساكن UChar32 U_CALLCONV
ucnv_MBCSGetFallback(UConverterMBCSTable *mbcs_جدول, uint32_t جابه_جایی) {
    ثابت _MBCSToUFallback *مقصد_UFallbacks;
    uint32_t i, شروع, حد;

    حد=mbcs_جدول->شمار_مقصد_UFallbacks;
    إذا(حد>0) {
        /* do a binary search for the fallback mapping */
        مقصد_UFallbacks=mbcs_جدول->مقصد_UFallbacks;
        شروع=0;
        طالما(شروع<حد-1) {
            i=(شروع+حد)/2;
            إذا(جابه_جایی<مقصد_UFallbacks[i].جابه_جایی) {
                حد=i;
            } وإلا {
                شروع=i;
            }
        }

        /* did we really find it? */
        إذا(جابه_جایی==مقصد_UFallbacks[شروع].جابه_جایی) {
            أرجع مقصد_UFallbacks[شروع].کد_نقطه;
        }
    }

    أرجع 0xfffe;
}

/* This version of ucnv_MBCSToUnicodeWithOffsets() is optimized for single-byte, single-state codepages. */
ساكن فراغ
ucnv_MBCSSingle_مقصد_Unicode_With_Offsets(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                                UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت uint8_t *منبع, *منبع_حد;
    char16_t *هدف;
    ثابت char16_t *هدف_حد;
    int32_t *offsets;

    ثابت int32_t (*state_جدول)[256];

    int32_t منبع_شاخص;

    int32_t مدخل;
    char16_t c;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_آرگومانها->converter;
    منبع = reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->منبع);
    منبع_حد = reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->منبع_حد);
    هدف=p_آرگومانها->هدف;
    هدف_حد=p_آرگومانها->هدف_حد;
    offsets=p_آرگومانها->offsets;

    إذا((cnv->options&UCNV_OPTION_جابجا_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_داده->mbcs.جابجا_LFNLState_جدول;
    } وإلا {
        state_جدول=cnv->shared_داده->mbcs.state_جدول;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    منبع_شاخص=0;

    /* conversion loop */
    طالما(منبع<منبع_حد) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        إذا(هدف>=هدف_حد) {
            /* target is full */
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
            اكسر;
        }

        مدخل=state_جدول[0][*منبع++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        إذا(MBCS_مدخل_FINAL_IS_VALID_مستقیم_16(مدخل)) {
            /* output BMP code point */
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            إذا(offsets!=اشاره_تهی) {
                *offsets++=منبع_شاخص;
            }

            /* normal end of action codes: prepare for a new character */
            ++منبع_شاخص;
            استمر;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_مدخل_FINAL_ACTION(مدخل));
        إذا(action==MBCS_STATE_VALID_مستقیم_20 ||
           (action==MBCS_STATE_FALLBACK_مستقیم_20 && UCNV_مقصد_U_USE_FALLBACK(cnv))
        ) {
            مدخل=MBCS_مدخل_FINAL_مقدار(مدخل);
            /* output surrogate pair */
            *هدف++ = static_cast<char16_t>(0xd800 | static_cast<char16_t>(مدخل >> 10));
            إذا(offsets!=اشاره_تهی) {
                *offsets++=منبع_شاخص;
            }
            c = static_cast<char16_t>(0xdc00 | static_cast<char16_t>(مدخل & 0x3ff));
            إذا(هدف<هدف_حد) {
                *هدف++=c;
                إذا(offsets!=اشاره_تهی) {
                    *offsets++=منبع_شاخص;
                }
            } وإلا {
                /* target overflow */
                cnv->UChar_Error_میانگیر[0]=c;
                cnv->UChar_Error_میانگیر_درازی=1;
                *p_Error_کد=U_میانگیر_سرریز_ERROR;
                اكسر;
            }

            ++منبع_شاخص;
            استمر;
        } وإلا إذا(action==MBCS_STATE_FALLBACK_مستقیم_16) {
            إذا(UCNV_مقصد_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
                إذا(offsets!=اشاره_تهی) {
                    *offsets++=منبع_شاخص;
                }

                ++منبع_شاخص;
                استمر;
            }
        } وإلا إذا(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } وإلا إذا(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_کد=U_ILLEGAL_CHAR_یافت;
        } وإلا {
            /* reserved, must never occur */
            ++منبع_شاخص;
            استمر;
        }

        إذا(U_FAILURE(*p_Error_کد)) {
            /* callback(illegal) */
            اكسر;
        } وإلا /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_آرگومانها->منبع = reinterpret_cast<ثابت حرف*>(منبع);
            cnv->مقصد_UBytes[0]=*(منبع-1);
            cnv->مقصد_ULength=ext_مقصد_U(cnv, cnv->shared_داده,
                                    1, &منبع, منبع_حد,
                                    &هدف, هدف_حد,
                                    &offsets, منبع_شاخص,
                                    p_آرگومانها->flush,
                                    p_Error_کد);
            منبع_شاخص += 1 + static_cast<int32_t>(منبع - reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->منبع));

            إذا(U_FAILURE(*p_Error_کد)) {
                /* not mappable or buffer overflow */
                اكسر;
            }
        }
    }

    /* write back the updated pointers */
    p_آرگومانها->منبع = reinterpret_cast<ثابت حرف*>(منبع);
    p_آرگومانها->هدف=هدف;
    p_آرگومانها->offsets=offsets;
}

/*
 * This version of ucnv_MBCSSingleToUnicodeWithOffsets() is optimized for single-byte, single-state codepages
 * that only map to and from the BMP.
 * In addition to single-byte optimizations, the offset calculations
 * become much easier.
 */
ساكن فراغ
ucnv_MBCSSingle_مقصد_BMPWith_Offsets(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                            UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت uint8_t *منبع, *منبع_حد, *آخرین_منبع;
    char16_t *هدف;
    int32_t هدف_ظرفیت, length;
    int32_t *offsets;

    ثابت int32_t (*state_جدول)[256];

    int32_t منبع_شاخص;

    int32_t مدخل;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_آرگومانها->converter;
    منبع = reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->منبع);
    منبع_حد = reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->منبع_حد);
    هدف=p_آرگومانها->هدف;
    هدف_ظرفیت = static_cast<int32_t>(p_آرگومانها->هدف_حد - p_آرگومانها->هدف);
    offsets=p_آرگومانها->offsets;

    إذا((cnv->options&UCNV_OPTION_جابجا_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_داده->mbcs.جابجا_LFNLState_جدول;
    } وإلا {
        state_جدول=cnv->shared_داده->mbcs.state_جدول;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    منبع_شاخص=0;
    آخرین_منبع=منبع;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(منبع_حد - منبع);
    إذا(length<هدف_ظرفیت) {
        هدف_ظرفیت=length;
    }

#إذا MBCS_UNROLL_SINGLE_مقصد_BMP
    /* unrolling makes it faster on Pentium III/Windows 2000 */
    /* unroll the loop with the most common case */
unrolled:
    إذا(هدف_ظرفیت>=16) {
        int32_t count, loops, oredEntries;

        loops=count=هدف_ظرفیت>>4;
        افعل {
            oredEntries=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            oredEntries|=مدخل=state_جدول[0][*منبع++];
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));

            /* were all 16 entries really valid? */
            إذا(!MBCS_مدخل_FINAL_IS_VALID_مستقیم_16(oredEntries)) {
                /* no, return to the first of these 16 */
                منبع-=16;
                هدف-=16;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        هدف_ظرفیت-=16*count;

        إذا(offsets!=اشاره_تهی) {
            آخرین_منبع+=16*count;
            طالما(count>0) {
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                --count;
            }
        }
    }
#پایان_اگر

    /* conversion loop */
    طالما(هدف_ظرفیت > 0 && منبع < منبع_حد) {
        مدخل=state_جدول[0][*منبع++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        إذا(MBCS_مدخل_FINAL_IS_VALID_مستقیم_16(مدخل)) {
            /* output BMP code point */
            *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            --هدف_ظرفیت;
            استمر;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_مدخل_FINAL_ACTION(مدخل));
        إذا(action==MBCS_STATE_FALLBACK_مستقیم_16) {
            إذا(UCNV_مقصد_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *هدف++ = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
                --هدف_ظرفیت;
                استمر;
            }
        } وإلا إذا(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } وإلا إذا(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_کد=U_ILLEGAL_CHAR_یافت;
        } وإلا {
            /* reserved, must never occur */
            استمر;
        }

        /* set offsets since the start or the last extension */
        إذا(offsets!=اشاره_تهی) {
            int32_t count = static_cast<int32_t>(منبع - آخرین_منبع);

            /* predecrement: do not set the offset for the callback-causing character */
            طالما(--count>0) {
                *offsets++=منبع_شاخص++;
            }
            /* offset and sourceIndex are now set for the current character */
        }

        إذا(U_FAILURE(*p_Error_کد)) {
            /* callback(illegal) */
            اكسر;
        } وإلا /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            آخرین_منبع=منبع;
            cnv->مقصد_UBytes[0]=*(منبع-1);
            cnv->مقصد_ULength=ext_مقصد_U(cnv, cnv->shared_داده,
                                    1, &منبع, منبع_حد,
                                    &هدف, p_آرگومانها->هدف_حد,
                                    &offsets, منبع_شاخص,
                                    p_آرگومانها->flush,
                                    p_Error_کد);
            منبع_شاخص += 1 + static_cast<int32_t>(منبع - آخرین_منبع);

            إذا(U_FAILURE(*p_Error_کد)) {
                /* not mappable or buffer overflow */
                اكسر;
            }

            /* recalculate the targetCapacity after an extension mapping */
            هدف_ظرفیت = static_cast<int32_t>(p_آرگومانها->هدف_حد - هدف);
            length = static_cast<int32_t>(منبع_حد - منبع);
            إذا(length<هدف_ظرفیت) {
                هدف_ظرفیت=length;
            }
        }

#إذا MBCS_UNROLL_SINGLE_مقصد_BMP
        /* unrolling makes it faster on Pentium III/Windows 2000 */
        اذهب unrolled;
#پایان_اگر
    }

    إذا(U_SUCCESS(*p_Error_کد) && منبع<منبع_حد && هدف>=p_آرگومانها->هدف_حد) {
        /* target is full */
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
    }

    /* set offsets since the start or the last callback */
    إذا(offsets!=اشاره_تهی) {
        size_t count=منبع-آخرین_منبع;
        طالما(count>0) {
            *offsets++=منبع_شاخص++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_آرگومانها->منبع = reinterpret_cast<ثابت حرف*>(منبع);
    p_آرگومانها->هدف=هدف;
    p_آرگومانها->offsets=offsets;
}

ساكن UBool
has_Valid_Trail_بایتها(ثابت int32_t (*state_جدول)[256], uint8_t state) {
    ثابت int32_t *ردیف=state_جدول[state];
    int32_t b, مدخل;
    /* First test for final entries in this state for some commonly valid byte values. */
    مدخل=ردیف[0xa1];
    إذا( !MBCS_مدخل_IS_TRANSITION(مدخل) &&
        MBCS_مدخل_FINAL_ACTION(مدخل)!=MBCS_STATE_ILLEGAL
    ) {
        أرجع درست;
    }
    مدخل=ردیف[0x41];
    إذا( !MBCS_مدخل_IS_TRANSITION(مدخل) &&
        MBCS_مدخل_FINAL_ACTION(مدخل)!=MBCS_STATE_ILLEGAL
    ) {
        أرجع درست;
    }
    /* Then test for final entries in this state. */
    لكل(b=0; b<=0xff; ++b) {
        مدخل=ردیف[b];
        إذا( !MBCS_مدخل_IS_TRANSITION(مدخل) &&
            MBCS_مدخل_FINAL_ACTION(مدخل)!=MBCS_STATE_ILLEGAL
        ) {
            أرجع درست;
        }
    }
    /* Then recurse for transition entries. */
    لكل(b=0; b<=0xff; ++b) {
        مدخل=ردیف[b];
        إذا( MBCS_مدخل_IS_TRANSITION(مدخل) &&
            has_Valid_Trail_بایتها(state_جدول, static_cast<uint8_t>(MBCS_مدخل_TRANSITION_STATE(مدخل)))
        ) {
            أرجع درست;
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
    ثابت int32_t *ردیف=state_جدول[state];
    int32_t مدخل=ردیف[b];
    إذا(MBCS_مدخل_IS_TRANSITION(مدخل)) {   /* lead byte */
        أرجع has_Valid_Trail_بایتها(state_جدول, static_cast<uint8_t>(MBCS_مدخل_TRANSITION_STATE(مدخل)));
    } وإلا {
        uint8_t action = static_cast<uint8_t>(MBCS_مدخل_FINAL_ACTION(مدخل));
        إذا(action==MBCS_STATE_CHANGE_ONLY && isDBCSOnly) {
            أرجع خطأ;   /* SI/SO are illegal for DBCS-only conversion */
        } وإلا {
            أرجع action!=MBCS_STATE_ILLEGAL;
        }
    }
}

U_CFUNC فراغ
ucnv_MBCSToUnicodeWithOffsets(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                          UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت uint8_t *منبع, *منبع_حد;
    char16_t *هدف;
    ثابت char16_t *هدف_حد;
    int32_t *offsets;

    ثابت int32_t (*state_جدول)[256];
    ثابت uint16_t *unicode_کد_Units;

    uint32_t جابه_جایی;
    uint8_t state;
    int8_t byte_شاخص;
    uint8_t *بایتها;

    int32_t منبع_شاخص, بعدی_منبع_شاخص;

    int32_t مدخل;
    char16_t c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_آرگومانها->converter;

    إذا(cnv->pre_مقصد_ULength>0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_تطابق_مقصد_U(cnv, p_آرگومانها, -1, p_Error_کد);

        إذا(U_FAILURE(*p_Error_کد) || cnv->pre_مقصد_ULength<0) {
            أرجع;
        }
    }

    إذا(cnv->shared_داده->mbcs.شمار_حالتها==1) {
        إذا(!(cnv->shared_داده->mbcs.unicode_ماسک&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingle_مقصد_BMPWith_Offsets(p_آرگومانها, p_Error_کد);
        } وإلا {
            ucnv_MBCSSingle_مقصد_Unicode_With_Offsets(p_آرگومانها, p_Error_کد);
        }
        أرجع;
    }

    /* set up the local pointers */
    منبع=(ثابت uint8_t *)p_آرگومانها->منبع;
    منبع_حد=(ثابت uint8_t *)p_آرگومانها->منبع_حد;
    هدف=p_آرگومانها->هدف;
    هدف_حد=p_آرگومانها->هدف_حد;
    offsets=p_آرگومانها->offsets;

    إذا((cnv->options&UCNV_OPTION_جابجا_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_داده->mbcs.جابجا_LFNLState_جدول;
    } وإلا {
        state_جدول=cnv->shared_داده->mbcs.state_جدول;
    }
    unicode_کد_Units=cnv->shared_داده->mbcs.unicode_کد_Units;

    /* get the converter state from UConverter */
    جابه_جایی=cnv->مقصد_Unicode_وضعیت;
    byte_شاخص=cnv->مقصد_ULength;
    بایتها=cnv->مقصد_UBytes;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    إذا((state=(uint8_t)(cnv->نما))==0) {
        state=cnv->shared_داده->mbcs.dbcsOnlyState;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    منبع_شاخص=byte_شاخص==0 ? 0 : -1;
    بعدی_منبع_شاخص=0;

    /* conversion loop */
    طالما(منبع<منبع_حد) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        إذا(هدف>=هدف_حد) {
            /* target is full */
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
            اكسر;
        }

        إذا(byte_شاخص==0) {
            /* optimized loop for 1/2-byte input and BMP output */
            إذا(offsets==اشاره_تهی) {
                افعل {
                    مدخل=state_جدول[state][*منبع];
                    إذا(MBCS_مدخل_IS_TRANSITION(مدخل)) {
                        state=(uint8_t)MBCS_مدخل_TRANSITION_STATE(مدخل);
                        جابه_جایی=MBCS_مدخل_TRANSITION_جابه_جایی(مدخل);

                        ++منبع;
                        إذا( منبع<منبع_حد &&
                            MBCS_مدخل_IS_FINAL(مدخل=state_جدول[state][*منبع]) &&
                            MBCS_مدخل_FINAL_ACTION(مدخل)==MBCS_STATE_VALID_16 &&
                            (c=unicode_کد_Units[جابه_جایی+MBCS_مدخل_FINAL_مقدار_16(مدخل)])<0xfffe
                        ) {
                            ++منبع;
                            *هدف++=c;
                            state=(uint8_t)MBCS_مدخل_FINAL_STATE(مدخل); /* typically 0 */
                            جابه_جایی=0;
                        } وإلا {
                            /* set the state and leave the optimized loop */
                            بایتها[0]=*(منبع-1);
                            byte_شاخص=1;
                            اكسر;
                        }
                    } وإلا {
                        إذا(MBCS_مدخل_FINAL_IS_VALID_مستقیم_16(مدخل)) {
                            /* output BMP code point */
                            ++منبع;
                            *هدف++=(char16_t)MBCS_مدخل_FINAL_مقدار_16(مدخل);
                            state=(uint8_t)MBCS_مدخل_FINAL_STATE(مدخل); /* typically 0 */
                        } وإلا {
                            /* leave the optimized loop */
                            اكسر;
                        }
                    }
                } طالما(منبع<منبع_حد && هدف<هدف_حد);
            } وإلا /* offsets!=nullptr */ {
                افعل {
                    مدخل=state_جدول[state][*منبع];
                    إذا(MBCS_مدخل_IS_TRANSITION(مدخل)) {
                        state=(uint8_t)MBCS_مدخل_TRANSITION_STATE(مدخل);
                        جابه_جایی=MBCS_مدخل_TRANSITION_جابه_جایی(مدخل);

                        ++منبع;
                        إذا( منبع<منبع_حد &&
                            MBCS_مدخل_IS_FINAL(مدخل=state_جدول[state][*منبع]) &&
                            MBCS_مدخل_FINAL_ACTION(مدخل)==MBCS_STATE_VALID_16 &&
                            (c=unicode_کد_Units[جابه_جایی+MBCS_مدخل_FINAL_مقدار_16(مدخل)])<0xfffe
                        ) {
                            ++منبع;
                            *هدف++=c;
                            إذا(offsets!=اشاره_تهی) {
                                *offsets++=منبع_شاخص;
                                منبع_شاخص=(بعدی_منبع_شاخص+=2);
                            }
                            state=(uint8_t)MBCS_مدخل_FINAL_STATE(مدخل); /* typically 0 */
                            جابه_جایی=0;
                        } وإلا {
                            /* set the state and leave the optimized loop */
                            ++بعدی_منبع_شاخص;
                            بایتها[0]=*(منبع-1);
                            byte_شاخص=1;
                            اكسر;
                        }
                    } وإلا {
                        إذا(MBCS_مدخل_FINAL_IS_VALID_مستقیم_16(مدخل)) {
                            /* output BMP code point */
                            ++منبع;
                            *هدف++=(char16_t)MBCS_مدخل_FINAL_مقدار_16(مدخل);
                            إذا(offsets!=اشاره_تهی) {
                                *offsets++=منبع_شاخص;
                                منبع_شاخص=++بعدی_منبع_شاخص;
                            }
                            state=(uint8_t)MBCS_مدخل_FINAL_STATE(مدخل); /* typically 0 */
                        } وإلا {
                            /* leave the optimized loop */
                            اكسر;
                        }
                    }
                } طالما(منبع<منبع_حد && هدف<هدف_حد);
            }

            /*
             * these tests and break statements could be put inside the loop
             * if C had "break outerLoop" like Java
             */
            إذا(منبع>=منبع_حد) {
                اكسر;
            }
            إذا(هدف>=هدف_حد) {
                /* target is full */
                *p_Error_کد=U_میانگیر_سرریز_ERROR;
                اكسر;
            }

            ++بعدی_منبع_شاخص;
            بایتها[byte_شاخص++]=*منبع++;
        } وإلا /* byteIndex>0 */ {
            ++بعدی_منبع_شاخص;
            مدخل=state_جدول[state][بایتها[byte_شاخص++]=*منبع++];
        }

        إذا(MBCS_مدخل_IS_TRANSITION(مدخل)) {
            state=(uint8_t)MBCS_مدخل_TRANSITION_STATE(مدخل);
            جابه_جایی+=MBCS_مدخل_TRANSITION_جابه_جایی(مدخل);
            استمر;
        }

        /* save the previous state for proper extension mapping with SI/SO-stateful converters */
        cnv->نما=state;

        /* set the next state early so that we can reuse the entry variable */
        state=(uint8_t)MBCS_مدخل_FINAL_STATE(مدخل); /* typically 0 */

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action=(uint8_t)(MBCS_مدخل_FINAL_ACTION(مدخل));
        إذا(action==MBCS_STATE_VALID_16) {
            جابه_جایی+=MBCS_مدخل_FINAL_مقدار_16(مدخل);
            c=unicode_کد_Units[جابه_جایی];
            إذا(c<0xfffe) {
                /* output BMP code point */
                *هدف++=c;
                إذا(offsets!=اشاره_تهی) {
                    *offsets++=منبع_شاخص;
                }
                byte_شاخص=0;
            } وإلا إذا(c==0xfffe) {
                إذا(UCNV_مقصد_U_USE_FALLBACK(cnv) && (مدخل=(int32_t)ucnv_MBCSGetFallback(&cnv->shared_داده->mbcs, جابه_جایی))!=0xfffe) {
                    /* output fallback BMP code point */
                    *هدف++=(char16_t)مدخل;
                    إذا(offsets!=اشاره_تهی) {
                        *offsets++=منبع_شاخص;
                    }
                    byte_شاخص=0;
                }
            } وإلا {
                /* callback(illegal) */
                *p_Error_کد=U_ILLEGAL_CHAR_یافت;
            }
        } وإلا إذا(action==MBCS_STATE_VALID_مستقیم_16) {
            /* output BMP code point */
            *هدف++=(char16_t)MBCS_مدخل_FINAL_مقدار_16(مدخل);
            إذا(offsets!=اشاره_تهی) {
                *offsets++=منبع_شاخص;
            }
            byte_شاخص=0;
        } وإلا إذا(action==MBCS_STATE_VALID_16_PAIR) {
            جابه_جایی+=MBCS_مدخل_FINAL_مقدار_16(مدخل);
            c=unicode_کد_Units[جابه_جایی++];
            إذا(c<0xd800) {
                /* output BMP code point below 0xd800 */
                *هدف++=c;
                إذا(offsets!=اشاره_تهی) {
                    *offsets++=منبع_شاخص;
                }
                byte_شاخص=0;
            } وإلا إذا(UCNV_مقصد_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                /* output roundtrip or fallback surrogate pair */
                *هدف++=(char16_t)(c&0xdbff);
                إذا(offsets!=اشاره_تهی) {
                    *offsets++=منبع_شاخص;
                }
                byte_شاخص=0;
                إذا(هدف<هدف_حد) {
                    *هدف++=unicode_کد_Units[جابه_جایی];
                    إذا(offsets!=اشاره_تهی) {
                        *offsets++=منبع_شاخص;
                    }
                } وإلا {
                    /* target overflow */
                    cnv->UChar_Error_میانگیر[0]=unicode_کد_Units[جابه_جایی];
                    cnv->UChar_Error_میانگیر_درازی=1;
                    *p_Error_کد=U_میانگیر_سرریز_ERROR;

                    جابه_جایی=0;
                    اكسر;
                }
            } وإلا إذا(UCNV_مقصد_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                *هدف++=unicode_کد_Units[جابه_جایی];
                إذا(offsets!=اشاره_تهی) {
                    *offsets++=منبع_شاخص;
                }
                byte_شاخص=0;
            } وإلا إذا(c==0xffff) {
                /* callback(illegal) */
                *p_Error_کد=U_ILLEGAL_CHAR_یافت;
            }
        } وإلا إذا(action==MBCS_STATE_VALID_مستقیم_20 ||
                  (action==MBCS_STATE_FALLBACK_مستقیم_20 && UCNV_مقصد_U_USE_FALLBACK(cnv))
        ) {
            مدخل=MBCS_مدخل_FINAL_مقدار(مدخل);
            /* output surrogate pair */
            *هدف++=(char16_t)(0xd800|(char16_t)(مدخل>>10));
            إذا(offsets!=اشاره_تهی) {
                *offsets++=منبع_شاخص;
            }
            byte_شاخص=0;
            c=(char16_t)(0xdc00|(char16_t)(مدخل&0x3ff));
            إذا(هدف<هدف_حد) {
                *هدف++=c;
                إذا(offsets!=اشاره_تهی) {
                    *offsets++=منبع_شاخص;
                }
            } وإلا {
                /* target overflow */
                cnv->UChar_Error_میانگیر[0]=c;
                cnv->UChar_Error_میانگیر_درازی=1;
                *p_Error_کد=U_میانگیر_سرریز_ERROR;

                جابه_جایی=0;
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
            إذا(cnv->shared_داده->mbcs.dbcsOnlyState==0) {
                byte_شاخص=0;
            } وإلا {
                /* SI/SO are illegal for DBCS-only conversion */
                state=(uint8_t)(cnv->نما); /* restore the previous state */

                /* callback(illegal) */
                *p_Error_کد=U_ILLEGAL_CHAR_یافت;
            }
        } وإلا إذا(action==MBCS_STATE_FALLBACK_مستقیم_16) {
            إذا(UCNV_مقصد_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *هدف++=(char16_t)MBCS_مدخل_FINAL_مقدار_16(مدخل);
                إذا(offsets!=اشاره_تهی) {
                    *offsets++=منبع_شاخص;
                }
                byte_شاخص=0;
            }
        } وإلا إذا(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } وإلا إذا(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_کد=U_ILLEGAL_CHAR_یافت;
        } وإلا {
            /* reserved, must never occur */
            byte_شاخص=0;
        }

        /* end of action codes: prepare for a new character */
        جابه_جایی=0;

        إذا(byte_شاخص==0) {
            منبع_شاخص=بعدی_منبع_شاخص;
        } وإلا إذا(U_FAILURE(*p_Error_کد)) {
            /* callback(illegal) */
            إذا(byte_شاخص>1) {
                /*
                 * Ticket 5691: consistent illegal sequences:
                 * - We include at least the first byte in the illegal sequence.
                 * - If any of the non-initial bytes could be the start of a character,
                 *   we stop the illegal sequence before the first one of those.
                 */
                UBool isDBCSOnly = cnv->shared_داده->mbcs.dbcsOnlyState != 0;
                int8_t i;
                لكل(i=1;
                    i<byte_شاخص && !isSingleOrLead(state_جدول, state, isDBCSOnly, بایتها[i]);
                    ++i) {}
                إذا(i<byte_شاخص) {
                    /* Back out some bytes. */
                    int8_t پشت_بیرون_فاصله=byte_شاخص-i;
                    int32_t بایتها_From_This_میانگیر=(int32_t)(منبع-(ثابت uint8_t *)p_آرگومانها->منبع);
                    byte_شاخص=i;  /* length of reported illegal byte sequence */
                    إذا(پشت_بیرون_فاصله<=بایتها_From_This_میانگیر) {
                        منبع-=پشت_بیرون_فاصله;
                    } وإلا {
                        /* Back out bytes from the previous buffer: Need to replay them. */
                        cnv->pre_مقصد_ULength=(int8_t)(بایتها_From_This_میانگیر-پشت_بیرون_فاصله);
                        /* preToULength is negative! */
                        uprv_memcpy(cnv->pre_مقصد_U, بایتها+i, -cnv->pre_مقصد_ULength);
                        منبع=(ثابت uint8_t *)p_آرگومانها->منبع;
                    }
                }
            }
            اكسر;
        } وإلا /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_آرگومانها->منبع=(ثابت حرف *)منبع;
            byte_شاخص=ext_مقصد_U(cnv, cnv->shared_داده,
                              byte_شاخص, &منبع, منبع_حد,
                              &هدف, هدف_حد,
                              &offsets, منبع_شاخص,
                              p_آرگومانها->flush,
                              p_Error_کد);
            منبع_شاخص=بعدی_منبع_شاخص+=(int32_t)(منبع-(ثابت uint8_t *)p_آرگومانها->منبع);

            إذا(U_FAILURE(*p_Error_کد)) {
                /* not mappable or buffer overflow */
                اكسر;
            }
        }
    }

    /* set the converter state back into UConverter */
    cnv->مقصد_Unicode_وضعیت=جابه_جایی;
    cnv->نما=state;
    cnv->مقصد_ULength=byte_شاخص;

    /* write back the updated pointers */
    p_آرگومانها->منبع=(ثابت حرف *)منبع;
    p_آرگومانها->هدف=هدف;
    p_آرگومانها->offsets=offsets;
}

/*
 * This version of ucnv_MBCSGetNextUChar() is optimized for single-byte, single-state codepages.
 * We still need a conversion loop in case we find reserved action codes, which are to be ignored.
 */
ساكن UChar32
ucnv_MBCSSingle_Get_بعدی_UChar(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                        UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت int32_t (*state_جدول)[256];
    ثابت uint8_t *منبع, *منبع_حد;

    int32_t مدخل;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_آرگومانها->converter;
    منبع = reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->منبع);
    منبع_حد = reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->منبع_حد);
    إذا((cnv->options&UCNV_OPTION_جابجا_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_داده->mbcs.جابجا_LFNLState_جدول;
    } وإلا {
        state_جدول=cnv->shared_داده->mbcs.state_جدول;
    }

    /* conversion loop */
    طالما(منبع<منبع_حد) {
        مدخل=state_جدول[0][*منبع++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* write back the updated pointer early so that we can return directly */
        p_آرگومانها->منبع = reinterpret_cast<ثابت حرف*>(منبع);

        إذا(MBCS_مدخل_FINAL_IS_VALID_مستقیم_16(مدخل)) {
            /* output BMP code point */
            أرجع static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_مدخل_FINAL_ACTION(مدخل));
        إذا( action==MBCS_STATE_VALID_مستقیم_20 ||
            (action==MBCS_STATE_FALLBACK_مستقیم_20 && UCNV_مقصد_U_USE_FALLBACK(cnv))
        ) {
            /* output supplementary code point */
            أرجع static_cast<UChar32>(MBCS_مدخل_FINAL_مقدار(مدخل) + 0x10000);
        } وإلا إذا(action==MBCS_STATE_FALLBACK_مستقیم_16) {
            إذا(UCNV_مقصد_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                أرجع static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
            }
        } وإلا إذا(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } وإلا إذا(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_کد=U_ILLEGAL_CHAR_یافت;
        } وإلا {
            /* reserved, must never occur */
            استمر;
        }

        إذا(U_FAILURE(*p_Error_کد)) {
            /* callback(illegal) */
            اكسر;
        } وإلا /* unassigned sequence */ {
            /* defer to the generic implementation */
            p_آرگومانها->منبع = reinterpret_cast<ثابت حرف*>(منبع) - 1;
            أرجع UCNV_GET_بعدی_UCHAR_USE_مقصد_U;
        }
    }

    /* no output because of empty input or only state changes */
    *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
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
ucnv_MBCSGet_بعدی_UChar(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                  UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت uint8_t *منبع, *منبع_حد, *آخرین_منبع;

    ثابت int32_t (*state_جدول)[256];
    ثابت uint16_t *unicode_کد_Units;

    uint32_t جابه_جایی;
    uint8_t state;

    int32_t مدخل;
    UChar32 c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_آرگومانها->converter;

    إذا(cnv->pre_مقصد_ULength>0) {
        /* use the generic code in ucnv_getNextUChar() to continue with a partial match */
        أرجع UCNV_GET_بعدی_UCHAR_USE_مقصد_U;
    }

    إذا(cnv->shared_داده->mbcs.unicode_ماسک&UCNV_HAS_SURROGATES) {
        /*
         * Using the generic ucnv_getNextUChar() code lets us deal correctly
         * with the rare case of a codepage that maps single surrogates
         * without adding the complexity to this already complicated function here.
         */
        أرجع UCNV_GET_بعدی_UCHAR_USE_مقصد_U;
    } وإلا إذا(cnv->shared_داده->mbcs.شمار_حالتها==1) {
        أرجع ucnv_MBCSSingle_Get_بعدی_UChar(p_آرگومانها, p_Error_کد);
    }

    /* set up the local pointers */
    منبع = آخرین_منبع = reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->منبع);
    منبع_حد = reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->منبع_حد);

    إذا((cnv->options&UCNV_OPTION_جابجا_LFNL)!=0) {
        state_جدول=(ثابت int32_t (*)[256])cnv->shared_داده->mbcs.جابجا_LFNLState_جدول;
    } وإلا {
        state_جدول=cnv->shared_داده->mbcs.state_جدول;
    }
    unicode_کد_Units=cnv->shared_داده->mbcs.unicode_کد_Units;

    /* get the converter state from UConverter */
    جابه_جایی=cnv->مقصد_Unicode_وضعیت;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    إذا ((state = static_cast<uint8_t>(cnv->نما)) == 0) {
        state=cnv->shared_داده->mbcs.dbcsOnlyState;
    }

    /* conversion loop */
    c=U_SENTINEL;
    طالما(منبع<منبع_حد) {
        مدخل=state_جدول[state][*منبع++];
        إذا(MBCS_مدخل_IS_TRANSITION(مدخل)) {
            state = static_cast<uint8_t>(MBCS_مدخل_TRANSITION_STATE(مدخل));
            جابه_جایی+=MBCS_مدخل_TRANSITION_جابه_جایی(مدخل);

            /* optimization for 1/2-byte input and BMP output */
            إذا( منبع<منبع_حد &&
                MBCS_مدخل_IS_FINAL(مدخل=state_جدول[state][*منبع]) &&
                MBCS_مدخل_FINAL_ACTION(مدخل)==MBCS_STATE_VALID_16 &&
                (c=unicode_کد_Units[جابه_جایی+MBCS_مدخل_FINAL_مقدار_16(مدخل)])<0xfffe
            ) {
                ++منبع;
                state = static_cast<uint8_t>(MBCS_مدخل_FINAL_STATE(مدخل)); /* typically 0 */
                /* output BMP code point */
                اكسر;
            }
        } وإلا {
            /* save the previous state for proper extension mapping with SI/SO-stateful converters */
            cnv->نما=state;

            /* set the next state early so that we can reuse the entry variable */
            state = static_cast<uint8_t>(MBCS_مدخل_FINAL_STATE(مدخل)); /* typically 0 */

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action = static_cast<uint8_t>(MBCS_مدخل_FINAL_ACTION(مدخل));
            إذا(action==MBCS_STATE_VALID_مستقیم_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
                اكسر;
            } وإلا إذا(action==MBCS_STATE_VALID_16) {
                جابه_جایی+=MBCS_مدخل_FINAL_مقدار_16(مدخل);
                c=unicode_کد_Units[جابه_جایی];
                إذا(c<0xfffe) {
                    /* output BMP code point */
                    اكسر;
                } وإلا إذا(c==0xfffe) {
                    إذا(UCNV_مقصد_U_USE_FALLBACK(cnv) && (c=ucnv_MBCSGetFallback(&cnv->shared_داده->mbcs, جابه_جایی))!=0xfffe) {
                        اكسر;
                    }
                } وإلا {
                    /* callback(illegal) */
                    *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                }
            } وإلا إذا(action==MBCS_STATE_VALID_16_PAIR) {
                جابه_جایی+=MBCS_مدخل_FINAL_مقدار_16(مدخل);
                c=unicode_کد_Units[جابه_جایی++];
                إذا(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                    اكسر;
                } وإلا إذا(UCNV_مقصد_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_کد_Units[جابه_جایی]+(0x10000-0xdc00);
                    اكسر;
                } وإلا إذا(UCNV_مقصد_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_کد_Units[جابه_جایی];
                    اكسر;
                } وإلا إذا(c==0xffff) {
                    /* callback(illegal) */
                    *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                }
            } وإلا إذا(action==MBCS_STATE_VALID_مستقیم_20 ||
                      (action==MBCS_STATE_FALLBACK_مستقیم_20 && UCNV_مقصد_U_USE_FALLBACK(cnv))
            ) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_مدخل_FINAL_مقدار(مدخل) + 0x10000);
                اكسر;
            } وإلا إذا(action==MBCS_STATE_CHANGE_ONLY) {
                /*
                 * This serves as a state change without any output.
                 * It is useful for reading simple stateful encodings,
                 * for example using just Shift-In/Shift-Out codes.
                 * The 21 unused bits may later be used for more sophisticated
                 * state transitions.
                 */
                إذا(cnv->shared_داده->mbcs.dbcsOnlyState!=0) {
                    /* SI/SO are illegal for DBCS-only conversion */
                    state = static_cast<uint8_t>(cnv->نما); /* restore the previous state */

                    /* callback(illegal) */
                    *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                }
            } وإلا إذا(action==MBCS_STATE_FALLBACK_مستقیم_16) {
                إذا(UCNV_مقصد_U_USE_FALLBACK(cnv)) {
                    /* output BMP code point */
                    c = static_cast<char16_t>(MBCS_مدخل_FINAL_مقدار_16(مدخل));
                    اكسر;
                }
            } وإلا إذا(action==MBCS_STATE_UNASSIGNED) {
                /* just fall through */
            } وإلا إذا(action==MBCS_STATE_ILLEGAL) {
                /* callback(illegal) */
                *p_Error_کد=U_ILLEGAL_CHAR_یافت;
            } وإلا {
                /* reserved (must never occur), or only state change */
                جابه_جایی=0;
                آخرین_منبع=منبع;
                استمر;
            }

            /* end of action codes: prepare for a new character */
            جابه_جایی=0;

            إذا(U_FAILURE(*p_Error_کد)) {
                /* callback(illegal) */
                اكسر;
            } وإلا /* unassigned sequence */ {
                /* defer to the generic implementation */
                cnv->مقصد_Unicode_وضعیت=0;
                cnv->نما=state;
                p_آرگومانها->منبع = reinterpret_cast<ثابت حرف*>(آخرین_منبع);
                أرجع UCNV_GET_بعدی_UCHAR_USE_مقصد_U;
            }
        }
    }

    إذا(c<0) {
        إذا(U_SUCCESS(*p_Error_کد) && منبع==منبع_حد && آخرین_منبع<منبع) {
            /* incomplete character byte sequence */
            cnv->مقصد_ULength = static_cast<int8_t>(منبع - آخرین_منبع);
            uprv_memcpy(cnv->مقصد_UBytes, آخرین_منبع, cnv->مقصد_ULength);
            *p_Error_کد=U_TRUNCATED_CHAR_یافت;
        } وإلا إذا(U_FAILURE(*p_Error_کد)) {
            /* callback(illegal) */
            /*
             * Ticket 5691: consistent illegal sequences:
             * - We include at least the first byte in the illegal sequence.
             * - If any of the non-initial bytes could be the start of a character,
             *   we stop the illegal sequence before the first one of those.
             */
            UBool isDBCSOnly = static_cast<UBool>(cnv->shared_داده->mbcs.dbcsOnlyState != 0);
            uint8_t *بایتها=cnv->مقصد_UBytes;
            *بایتها++=*آخرین_منبع++;     /* first byte */
            إذا(آخرین_منبع==منبع) {
                cnv->مقصد_ULength=1;
            } وإلا /* lastSource<source: multi-byte character */ {
                int8_t i;
                لكل(i=1;
                    آخرین_منبع<منبع && !isSingleOrLead(state_جدول, state, isDBCSOnly, *آخرین_منبع);
                    ++i
                ) {
                    *بایتها++=*آخرین_منبع++;
                }
                cnv->مقصد_ULength=i;
                منبع=آخرین_منبع;
            }
        } وإلا {
            /* no output because of empty input or only state changes */
            *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        }
        c=0xffff;
    }

    /* set the converter state back into UConverter, ready for a new character */
    cnv->مقصد_Unicode_وضعیت=0;
    cnv->نما=state;

    /* write back the updated pointer */
    p_آرگومانها->منبع = reinterpret_cast<ثابت حرف*>(منبع);
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
ucnv_MBCSSingle_Simple_Get_بعدی_UChar(UConverter_Shared_داده *shared_داده,
                              uint8_t b, UBool useFallback) {
    int32_t مدخل;
    uint8_t action;

    مدخل=shared_داده->mbcs.state_جدول[0][b];
    /* MBCS_ENTRY_IS_FINAL(entry) */

    إذا(MBCS_مدخل_FINAL_IS_VALID_مستقیم_16(مدخل)) {
        /* output BMP code point */
        أرجع (char16_t)MBCS_مدخل_FINAL_مقدار_16(مدخل);
    }

    /*
     * An if-else-if chain provides more reliable performance for
     * the most common cases compared to a switch.
     */
    action=(uint8_t)(MBCS_مدخل_FINAL_ACTION(مدخل));
    إذا(action==MBCS_STATE_VALID_مستقیم_20) {
        /* output supplementary code point */
        أرجع 0x10000+MBCS_مدخل_FINAL_مقدار(مدخل);
    } وإلا إذا(action==MBCS_STATE_FALLBACK_مستقیم_16) {
        إذا(!مقصد_U_USE_FALLBACK(useFallback)) {
            أرجع 0xfffe;
        }
        /* output BMP code point */
        أرجع (char16_t)MBCS_مدخل_FINAL_مقدار_16(مدخل);
    } وإلا إذا(action==MBCS_STATE_FALLBACK_مستقیم_20) {
        إذا(!مقصد_U_USE_FALLBACK(useFallback)) {
            أرجع 0xfffe;
        }
        /* output supplementary code point */
        أرجع 0x10000+MBCS_مدخل_FINAL_مقدار(مدخل);
    } وإلا إذا(action==MBCS_STATE_UNASSIGNED) {
        أرجع 0xfffe;
    } وإلا إذا(action==MBCS_STATE_ILLEGAL) {
        أرجع 0xffff;
    } وإلا {
        /* reserved, must never occur */
        أرجع 0xffff;
    }
}
#پایان_اگر

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
ucnv_MBCSSimple_Get_بعدی_UChar(UConverter_Shared_داده *shared_داده,
                        ثابت حرف *منبع, int32_t length,
                        UBool useFallback) {
    ثابت int32_t (*state_جدول)[256];
    ثابت uint16_t *unicode_کد_Units;

    uint32_t جابه_جایی;
    uint8_t state, action;

    UChar32 c;
    int32_t i, مدخل;

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
    إذا(shared_داده->mbcs.شمار_حالتها==1) {
        إذا(length==1) {
            أرجع ucnv_MBCSSingle_Simple_Get_بعدی_UChar(shared_داده, (uint8_t)*منبع, useFallback);
        } وإلا {
            أرجع 0xffff; /* illegal: more than a single byte for an SBCS converter */
        }
    }
#پایان_اگر

    /* set up the local pointers */
    state_جدول=shared_داده->mbcs.state_جدول;
    unicode_کد_Units=shared_داده->mbcs.unicode_کد_Units;

    /* converter state */
    جابه_جایی=0;
    state=shared_داده->mbcs.dbcsOnlyState;

    /* conversion loop */
    لكل(i=0;;) {
        مدخل=state_جدول[state][(uint8_t)منبع[i++]];
        إذا(MBCS_مدخل_IS_TRANSITION(مدخل)) {
            state=(uint8_t)MBCS_مدخل_TRANSITION_STATE(مدخل);
            جابه_جایی+=MBCS_مدخل_TRANSITION_جابه_جایی(مدخل);

            إذا(i==length) {
                أرجع 0xffff; /* truncated character */
            }
        } وإلا {
            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=(uint8_t)(MBCS_مدخل_FINAL_ACTION(مدخل));
            إذا(action==MBCS_STATE_VALID_16) {
                جابه_جایی+=MBCS_مدخل_FINAL_مقدار_16(مدخل);
                c=unicode_کد_Units[جابه_جایی];
                إذا(c!=0xfffe) {
                    /*انجامشده*/
                } وإلا إذا(UCNV_مقصد_U_USE_FALLBACK(cnv)) {
                    c=ucnv_MBCSGetFallback(&shared_داده->mbcs, جابه_جایی);
                /* else done with 0xfffe */
                }
                اكسر;
            } وإلا إذا(action==MBCS_STATE_VALID_مستقیم_16) {
                /* output BMP code point */
                c=(char16_t)MBCS_مدخل_FINAL_مقدار_16(مدخل);
                اكسر;
            } وإلا إذا(action==MBCS_STATE_VALID_16_PAIR) {
                جابه_جایی+=MBCS_مدخل_FINAL_مقدار_16(مدخل);
                c=unicode_کد_Units[جابه_جایی++];
                إذا(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } وإلا إذا(UCNV_مقصد_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=(UChar32)(((c&0x3ff)<<10)+unicode_کد_Units[جابه_جایی]+(0x10000-0xdc00));
                } وإلا إذا(UCNV_مقصد_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_کد_Units[جابه_جایی];
                } وإلا إذا(c==0xffff) {
                    أرجع 0xffff;
                } وإلا {
                    c=0xfffe;
                }
                اكسر;
            } وإلا إذا(action==MBCS_STATE_VALID_مستقیم_20) {
                /* output supplementary code point */
                c=0x10000+MBCS_مدخل_FINAL_مقدار(مدخل);
                اكسر;
            } وإلا إذا(action==MBCS_STATE_FALLBACK_مستقیم_16) {
                إذا(!مقصد_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    اكسر;
                }
                /* output BMP code point */
                c=(char16_t)MBCS_مدخل_FINAL_مقدار_16(مدخل);
                اكسر;
            } وإلا إذا(action==MBCS_STATE_FALLBACK_مستقیم_20) {
                إذا(!مقصد_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    اكسر;
                }
                /* output supplementary code point */
                c=0x10000+MBCS_مدخل_FINAL_مقدار(مدخل);
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
        ثابت int32_t *cx=shared_داده->mbcs.extIndexes;
        إذا(cx!=اشاره_تهی) {
            أرجع ucnv_ext_Simple_تطابق_مقصد_U(cx, منبع, length, useFallback);
        }
    }

    أرجع c;
}

/* MBCS-from-Unicode conversion functions ----------------------------------- */

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for double-byte codepages. */
ساكن فراغ
ucnv_MBCSDoubleFromUnicodeWithOffsets(UConverter_From_Unicode_آرگومانها *p_آرگومانها,
                                  UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت char16_t *منبع, *منبع_حد;
    uint8_t *هدف;
    int32_t هدف_ظرفیت;
    int32_t *offsets;

    ثابت uint16_t *جدول;
    ثابت uint16_t *mbcs_شاخص;
    ثابت uint8_t *بایتها;

    UChar32 c;

    int32_t منبع_شاخص, بعدی_منبع_شاخص;

    uint32_t stage2_مدخل;
    uint32_t asciiRoundtrips;
    uint32_t value;
    uint8_t unicode_ماسک;

    /* use optimized function if possible */
    cnv=p_آرگومانها->converter;
    unicode_ماسک=cnv->shared_داده->mbcs.unicode_ماسک;

    /* set up the local pointers */
    منبع=p_آرگومانها->منبع;
    منبع_حد=p_آرگومانها->منبع_حد;
    هدف = reinterpret_cast<uint8_t*>(p_آرگومانها->هدف);
    هدف_ظرفیت = static_cast<int32_t>(p_آرگومانها->هدف_حد - p_آرگومانها->هدف);
    offsets=p_آرگومانها->offsets;

    جدول=cnv->shared_داده->mbcs.from_Unicode_جدول;
    mbcs_شاخص=cnv->shared_داده->mbcs.mbcs_شاخص;
    إذا((cnv->options&UCNV_OPTION_جابجا_LFNL)!=0) {
        بایتها=cnv->shared_داده->mbcs.جابجا_LFNLFrom_Unicode_بایتها;
    } وإلا {
        بایتها=cnv->shared_داده->mbcs.from_Unicode_بایتها;
    }
    asciiRoundtrips=cnv->shared_داده->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    منبع_شاخص= c==0 ? 0 : -1;
    بعدی_منبع_شاخص=0;

    /* conversion loop */
    إذا(c!=0 && هدف_ظرفیت>0) {
        اذهب getTrail;
    }

    طالما(منبع<منبع_حد) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        إذا(هدف_ظرفیت>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*منبع++;
            ++بعدی_منبع_شاخص;
            إذا(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *هدف++ = static_cast<uint8_t>(c);
                إذا(offsets!=اشاره_تهی) {
                    *offsets++=منبع_شاخص;
                    منبع_شاخص=بعدی_منبع_شاخص;
                }
                --هدف_ظرفیت;
                c=0;
                استمر;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            إذا(c<=0xd7ff) {
                value=DBCS_نتیجه_FROM_MOST_BMP(mbcs_شاخص, (ثابت uint16_t *)بایتها, c);
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
                إذا(U16_IS_SURROGATE(c) && !(unicode_ماسک&UCNV_HAS_SURROGATES)) {
                    إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        إذا(منبع<منبع_حد) {
                            /* test the following code unit */
                            char16_t trail=*منبع;
                            إذا(U16_IS_TRAIL(trail)) {
                                ++منبع;
                                ++بعدی_منبع_شاخص;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                إذا(!(unicode_ماسک&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    /* callback(unassigned) */
                                    اذهب unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } وإلا {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                                اكسر;
                            }
                        } وإلا {
                            /* no more input */
                            اكسر;
                        }
                    } وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                        اكسر;
                    }
                }

                /* convert the Unicode code point in c into codepage bytes */
                stage2_مدخل=MBCS_STAGE_2_FROM_U(جدول, c);

                /* get the bytes and the length for the output */
                /* MBCS_OUTPUT_2 */
                value=MBCS_مقدار_2_FROM_STAGE_2(بایتها, stage2_مدخل, c);

                /* is this code point assigned, or do we use fallbacks? */
                إذا(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_مدخل, c) ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_آرگومانها->منبع=منبع;
                    c=_extFromU(cnv, cnv->shared_داده,
                                c, &منبع, منبع_حد,
                                &هدف, هدف+هدف_ظرفیت,
                                &offsets, منبع_شاخص,
                                p_آرگومانها->flush,
                                p_Error_کد);
                    بعدی_منبع_شاخص += static_cast<int32_t>(منبع - p_آرگومانها->منبع);

                    إذا(U_FAILURE(*p_Error_کد)) {
                        /* not mappable or buffer overflow */
                        اكسر;
                    } وإلا {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        هدف_ظرفیت = static_cast<int32_t>(p_آرگومانها->هدف_حد - reinterpret_cast<حرف*>(هدف));

                        /* normal end of conversion: prepare for a new character */
                        منبع_شاخص=بعدی_منبع_شاخص;
                        استمر;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            إذا(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *هدف++ = static_cast<uint8_t>(value);
                إذا(offsets!=اشاره_تهی) {
                    *offsets++=منبع_شاخص;
                }
                --هدف_ظرفیت;
            } وإلا /* length==2 */ {
                *هدف++ = static_cast<uint8_t>(value >> 8);
                إذا(2<=هدف_ظرفیت) {
                    *هدف++ = static_cast<uint8_t>(value);
                    إذا(offsets!=اشاره_تهی) {
                        *offsets++=منبع_شاخص;
                        *offsets++=منبع_شاخص;
                    }
                    هدف_ظرفیت-=2;
                } وإلا {
                    إذا(offsets!=اشاره_تهی) {
                        *offsets++=منبع_شاخص;
                    }
                    cnv->char_Error_میانگیر[0] = static_cast<حرف>(value);
                    cnv->char_Error_میانگیر_درازی=1;

                    /* target overflow */
                    هدف_ظرفیت=0;
                    *p_Error_کد=U_میانگیر_سرریز_ERROR;
                    c=0;
                    اكسر;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            منبع_شاخص=بعدی_منبع_شاخص;
            استمر;
        } وإلا {
            /* target is full */
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
            اكسر;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_آرگومانها->منبع=منبع;
    p_آرگومانها->هدف = reinterpret_cast<حرف*>(هدف);
    p_آرگومانها->offsets=offsets;
}

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for single-byte codepages. */
ساكن فراغ
ucnv_MBCSSingleFromUnicodeWithOffsets(UConverter_From_Unicode_آرگومانها *p_آرگومانها,
                                  UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت char16_t *منبع, *منبع_حد;
    uint8_t *هدف;
    int32_t هدف_ظرفیت;
    int32_t *offsets;

    ثابت uint16_t *جدول;
    ثابت uint16_t *results;

    UChar32 c;

    int32_t منبع_شاخص, بعدی_منبع_شاخص;

    uint16_t value, min_مقدار;
    UBool hasSupplementary;

    /* set up the local pointers */
    cnv=p_آرگومانها->converter;
    منبع=p_آرگومانها->منبع;
    منبع_حد=p_آرگومانها->منبع_حد;
    هدف = reinterpret_cast<uint8_t*>(p_آرگومانها->هدف);
    هدف_ظرفیت = static_cast<int32_t>(p_آرگومانها->هدف_حد - p_آرگومانها->هدف);
    offsets=p_آرگومانها->offsets;

    جدول=cnv->shared_داده->mbcs.from_Unicode_جدول;
    إذا((cnv->options&UCNV_OPTION_جابجا_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_داده->mbcs.جابجا_LFNLFrom_Unicode_بایتها);
    } وإلا {
        results=(uint16_t *)cnv->shared_داده->mbcs.from_Unicode_بایتها;
    }

    إذا(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_مقدار=0x800;
    } وإلا {
        /* use only roundtrips and fallbacks from private-use characters */
        min_مقدار=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_داده->mbcs.unicode_ماسک & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    منبع_شاخص= c==0 ? 0 : -1;
    بعدی_منبع_شاخص=0;

    /* conversion loop */
    إذا(c!=0 && هدف_ظرفیت>0) {
        اذهب getTrail;
    }

    طالما(منبع<منبع_حد) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        إذا(هدف_ظرفیت>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*منبع++;
            ++بعدی_منبع_شاخص;
            إذا(U16_IS_SURROGATE(c)) {
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                    إذا(منبع<منبع_حد) {
                        /* test the following code unit */
                        char16_t trail=*منبع;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++منبع;
                            ++بعدی_منبع_شاخص;
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
                            *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                            اكسر;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                    اكسر;
                }
            }

            /* convert the Unicode code point in c into codepage bytes */
            value=MBCS_SINGLE_نتیجه_FROM_U(جدول, results, c);

            /* is this code point assigned, or do we use fallbacks? */
            إذا(value>=min_مقدار) {
                /* assigned, write the output character bytes from value and length */
                /* length==1 */
                /* this is easy because we know that there is enough space */
                *هدف++ = static_cast<uint8_t>(value);
                إذا(offsets!=اشاره_تهی) {
                    *offsets++=منبع_شاخص;
                }
                --هدف_ظرفیت;

                /* normal end of conversion: prepare for a new character */
                c=0;
                منبع_شاخص=بعدی_منبع_شاخص;
            } وإلا { /* unassigned */
unassigned:
                /* try an extension mapping */
                p_آرگومانها->منبع=منبع;
                c=_extFromU(cnv, cnv->shared_داده,
                            c, &منبع, منبع_حد,
                            &هدف, هدف+هدف_ظرفیت,
                            &offsets, منبع_شاخص,
                            p_آرگومانها->flush,
                            p_Error_کد);
                بعدی_منبع_شاخص += static_cast<int32_t>(منبع - p_آرگومانها->منبع);

                إذا(U_FAILURE(*p_Error_کد)) {
                    /* not mappable or buffer overflow */
                    اكسر;
                } وإلا {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    هدف_ظرفیت = static_cast<int32_t>(p_آرگومانها->هدف_حد - reinterpret_cast<حرف*>(هدف));

                    /* normal end of conversion: prepare for a new character */
                    منبع_شاخص=بعدی_منبع_شاخص;
                }
            }
        } وإلا {
            /* target is full */
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
            اكسر;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_آرگومانها->منبع=منبع;
    p_آرگومانها->هدف = reinterpret_cast<حرف*>(هدف);
    p_آرگومانها->offsets=offsets;
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
ucnv_MBCSSingleFromBMPWithOffsets(UConverter_From_Unicode_آرگومانها *p_آرگومانها,
                              UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت char16_t *منبع, *منبع_حد, *آخرین_منبع;
    uint8_t *هدف;
    int32_t هدف_ظرفیت, length;
    int32_t *offsets;

    ثابت uint16_t *جدول;
    ثابت uint16_t *results;

    UChar32 c;

    int32_t منبع_شاخص;

    uint32_t asciiRoundtrips;
    uint16_t value, min_مقدار;

    /* set up the local pointers */
    cnv=p_آرگومانها->converter;
    منبع=p_آرگومانها->منبع;
    منبع_حد=p_آرگومانها->منبع_حد;
    هدف = reinterpret_cast<uint8_t*>(p_آرگومانها->هدف);
    هدف_ظرفیت = static_cast<int32_t>(p_آرگومانها->هدف_حد - p_آرگومانها->هدف);
    offsets=p_آرگومانها->offsets;

    جدول=cnv->shared_داده->mbcs.from_Unicode_جدول;
    إذا((cnv->options&UCNV_OPTION_جابجا_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_داده->mbcs.جابجا_LFNLFrom_Unicode_بایتها);
    } وإلا {
        results=(uint16_t *)cnv->shared_داده->mbcs.from_Unicode_بایتها;
    }
    asciiRoundtrips=cnv->shared_داده->mbcs.asciiRoundtrips;

    إذا(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_مقدار=0x800;
    } وإلا {
        /* use only roundtrips and fallbacks from private-use characters */
        min_مقدار=0xc00;
    }

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    منبع_شاخص= c==0 ? 0 : -1;
    آخرین_منبع=منبع;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(منبع_حد - منبع);
    إذا(length<هدف_ظرفیت) {
        هدف_ظرفیت=length;
    }

    /* conversion loop */
    إذا(c!=0 && هدف_ظرفیت>0) {
        اذهب getTrail;
    }

#إذا MBCS_UNROLL_SINGLE_FROM_BMP
    /* unrolling makes it slower on Pentium III/Windows 2000?! */
    /* unroll the loop with the most common case */
unrolled:
    إذا(هدف_ظرفیت>=4) {
        int32_t count, loops;
        uint16_t anded_مقادیر;

        loops=count=هدف_ظرفیت>>2;
        افعل {
            c=*منبع++;
            anded_مقادیر=value=MBCS_SINGLE_نتیجه_FROM_U(جدول, results, c);
            *هدف++=(uint8_t)value;
            c=*منبع++;
            anded_مقادیر&=value=MBCS_SINGLE_نتیجه_FROM_U(جدول, results, c);
            *هدف++=(uint8_t)value;
            c=*منبع++;
            anded_مقادیر&=value=MBCS_SINGLE_نتیجه_FROM_U(جدول, results, c);
            *هدف++=(uint8_t)value;
            c=*منبع++;
            anded_مقادیر&=value=MBCS_SINGLE_نتیجه_FROM_U(جدول, results, c);
            *هدف++=(uint8_t)value;

            /* were all 4 entries really valid? */
            إذا(anded_مقادیر<min_مقدار) {
                /* no, return to the first of these 4 */
                منبع-=4;
                هدف-=4;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        هدف_ظرفیت-=4*count;

        إذا(offsets!=اشاره_تهی) {
            آخرین_منبع+=4*count;
            طالما(count>0) {
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                --count;
            }
        }

        c=0;
    }
#پایان_اگر

    طالما(هدف_ظرفیت>0) {
        /*
         * Get a correct Unicode code point:
         * a single char16_t for a BMP code point or
         * a matched surrogate pair for a "supplementary code point".
         */
        c=*منبع++;
        /*
         * Do not immediately check for single surrogates:
         * Assume that they are unassigned and check for them in that case.
         * This speeds up the conversion of assigned characters.
         */
        /* convert the Unicode code point in c into codepage bytes */
        إذا(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
            *هدف++ = static_cast<uint8_t>(c);
            --هدف_ظرفیت;
            c=0;
            استمر;
        }
        value=MBCS_SINGLE_نتیجه_FROM_U(جدول, results, c);
        /* is this code point assigned, or do we use fallbacks? */
        إذا(value>=min_مقدار) {
            /* assigned, write the output character bytes from value and length */
            /* length==1 */
            /* this is easy because we know that there is enough space */
            *هدف++ = static_cast<uint8_t>(value);
            --هدف_ظرفیت;

            /* normal end of conversion: prepare for a new character */
            c=0;
            استمر;
        } وإلا إذا(!U16_IS_SURROGATE(c)) {
            /* normal, unassigned BMP character */
        } وإلا إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
            إذا(منبع<منبع_حد) {
                /* test the following code unit */
                char16_t trail=*منبع;
                إذا(U16_IS_TRAIL(trail)) {
                    ++منبع;
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } وإلا {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                    *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                    اكسر;
                }
            } وإلا {
                /* no more input */
                إذا (p_آرگومانها->flush) {
                    *p_Error_کد=U_TRUNCATED_CHAR_یافت;
                }
                اكسر;
            }
        } وإلا {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
            *p_Error_کد=U_ILLEGAL_CHAR_یافت;
            اكسر;
        }

        /* c does not have a mapping */

        /* get the number of code units for c to correctly advance sourceIndex */
        length=U16_درازی(c);

        /* set offsets since the start or the last extension */
        إذا(offsets!=اشاره_تهی) {
            int32_t count = static_cast<int32_t>(منبع - آخرین_منبع);

            /* do not set the offset for this character */
            count-=length;

            طالما(count>0) {
                *offsets++=منبع_شاخص++;
                --count;
            }
            /* offsets and sourceIndex are now set for the current character */
        }

        /* try an extension mapping */
        آخرین_منبع=منبع;
        c=_extFromU(cnv, cnv->shared_داده,
                    c, &منبع, منبع_حد,
                    &هدف, reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->هدف_حد),
                    &offsets, منبع_شاخص,
                    p_آرگومانها->flush,
                    p_Error_کد);
        منبع_شاخص += length + static_cast<int32_t>(منبع - آخرین_منبع);
        آخرین_منبع=منبع;

        إذا(U_FAILURE(*p_Error_کد)) {
            /* not mappable or buffer overflow */
            اكسر;
        } وإلا {
            /* a mapping was written to the target, continue */

            /* recalculate the targetCapacity after an extension mapping */
            هدف_ظرفیت = static_cast<int32_t>(p_آرگومانها->هدف_حد - reinterpret_cast<حرف*>(هدف));
            length = static_cast<int32_t>(منبع_حد - منبع);
            إذا(length<هدف_ظرفیت) {
                هدف_ظرفیت=length;
            }
        }

#إذا MBCS_UNROLL_SINGLE_FROM_BMP
        /* unrolling makes it slower on Pentium III/Windows 2000?! */
        اذهب unrolled;
#پایان_اگر
    }

    إذا(U_SUCCESS(*p_Error_کد) && منبع<منبع_حد && هدف>=(uint8_t *)p_آرگومانها->هدف_حد) {
        /* target is full */
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
    }

    /* set offsets since the start or the last callback */
    إذا(offsets!=اشاره_تهی) {
        size_t count=منبع-آخرین_منبع;
        إذا (count > 0 && *p_Error_کد == U_TRUNCATED_CHAR_یافت) {
            /*
            Caller gave us a partial supplementary character,
            which this function couldn't convert in any case.
            The callback will handle the offset.
            */
            count--;
        }
        طالما(count>0) {
            *offsets++=منبع_شاخص++;
            --count;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_آرگومانها->منبع=منبع;
    p_آرگومانها->هدف = reinterpret_cast<حرف*>(هدف);
    p_آرگومانها->offsets=offsets;
}

U_CFUNC فراغ
ucnv_MBCSFromUnicodeWithOffsets(UConverter_From_Unicode_آرگومانها *p_آرگومانها,
                            UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت char16_t *منبع, *منبع_حد;
    uint8_t *هدف;
    int32_t هدف_ظرفیت;
    int32_t *offsets;

    ثابت uint16_t *جدول;
    ثابت uint16_t *mbcs_شاخص;
    ثابت uint8_t *p, *بایتها;
    uint8_t outputType;

    UChar32 c;

    int32_t prev_منبع_شاخص, منبع_شاخص, بعدی_منبع_شاخص;

    uint32_t stage2_مدخل;
    uint32_t asciiRoundtrips;
    uint32_t value;
    /* Shift-In and Shift-Out byte sequences differ by encoding scheme. */
    uint8_t si_بایتها[2] = {0, 0};
    uint8_t so_بایتها[2] = {0, 0};
    uint8_t si_درازی, so_درازی;
    int32_t length = 0, prev_درازی;
    uint8_t unicode_ماسک;

    cnv=p_آرگومانها->converter;

    إذا(cnv->preFromUFirstCP>=0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_تطابق_From_U(cnv, p_آرگومانها, -1, p_Error_کد);

        إذا(U_FAILURE(*p_Error_کد) || cnv->preFromULength<0) {
            أرجع;
        }
    }

    /* use optimized function if possible */
    outputType=cnv->shared_داده->mbcs.outputType;
    unicode_ماسک=cnv->shared_داده->mbcs.unicode_ماسک;
    إذا(outputType==MBCS_OUTPUT_1 && !(unicode_ماسک&UCNV_HAS_SURROGATES)) {
        إذا(!(unicode_ماسک&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingleFromBMPWithOffsets(p_آرگومانها, p_Error_کد);
        } وإلا {
            ucnv_MBCSSingleFromUnicodeWithOffsets(p_آرگومانها, p_Error_کد);
        }
        أرجع;
    } وإلا إذا(outputType==MBCS_OUTPUT_2 && cnv->shared_داده->mbcs.utf8Friendly) {
        ucnv_MBCSDoubleFromUnicodeWithOffsets(p_آرگومانها, p_Error_کد);
        أرجع;
    }

    /* set up the local pointers */
    منبع=p_آرگومانها->منبع;
    منبع_حد=p_آرگومانها->منبع_حد;
    هدف=(uint8_t *)p_آرگومانها->هدف;
    هدف_ظرفیت=(int32_t)(p_آرگومانها->هدف_حد-p_آرگومانها->هدف);
    offsets=p_آرگومانها->offsets;

    جدول=cnv->shared_داده->mbcs.from_Unicode_جدول;
    إذا(cnv->shared_داده->mbcs.utf8Friendly) {
        mbcs_شاخص=cnv->shared_داده->mbcs.mbcs_شاخص;
    } وإلا {
        mbcs_شاخص=اشاره_تهی;
    }
    إذا((cnv->options&UCNV_OPTION_جابجا_LFNL)!=0) {
        بایتها=cnv->shared_داده->mbcs.جابجا_LFNLFrom_Unicode_بایتها;
    } وإلا {
        بایتها=cnv->shared_داده->mbcs.from_Unicode_بایتها;
    }
    asciiRoundtrips=cnv->shared_داده->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    إذا(outputType==MBCS_OUTPUT_2_SISO) {
        prev_درازی=cnv->from_Unicode_وضعیت;
        إذا(prev_درازی==0) {
            /* set the real value */
            prev_درازی=1;
        }
    } وإلا {
        /* prevent fromUnicodeStatus from being set to something non-0 */
        prev_درازی=0;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    prev_منبع_شاخص=-1;
    منبع_شاخص= c==0 ? 0 : -1;
    بعدی_منبع_شاخص=0;

    /* Get the SI/SO character for the converter */
    si_درازی = static_cast<uint8_t>(getSISOBytes(SI, cnv->options, si_بایتها));
    so_درازی = static_cast<uint8_t>(getSISOBytes(SO, cnv->options, so_بایتها));

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
    إذا(c!=0 && هدف_ظرفیت>0) {
        اذهب getTrail;
    }

    طالما(منبع<منبع_حد) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        إذا(هدف_ظرفیت>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*منبع++;
            ++بعدی_منبع_شاخص;
            إذا(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *هدف++=(uint8_t)c;
                إذا(offsets!=اشاره_تهی) {
                    *offsets++=منبع_شاخص;
                    prev_منبع_شاخص=منبع_شاخص;
                    منبع_شاخص=بعدی_منبع_شاخص;
                }
                --هدف_ظرفیت;
                c=0;
                استمر;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            إذا(c<=0xd7ff && mbcs_شاخص!=اشاره_تهی) {
                value=mbcs_شاخص[c>>6];

                /* get the bytes and the length for the output (copied from below and adapted for utf8Friendly data) */
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                بدل(outputType) {
                حالة MBCS_OUTPUT_2:
                    value=((ثابت uint16_t *)بایتها)[value +(c&0x3f)];
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
                    cnv->from_Unicode_وضعیت=prev_درازی; /* save the old state */
                    value=((ثابت uint16_t *)بایتها)[value +(c&0x3f)];
                    إذا(value<=0xff) {
                        إذا(value==0) {
                            اذهب unassigned;
                        } وإلا إذا(prev_درازی<=1) {
                            length=1;
                        } وإلا {
                            /* change from double-byte mode to single-byte */
                            إذا (si_درازی == 1) {
                                value|=(uint32_t)si_بایتها[0]<<8;
                                length = 2;
                            } وإلا إذا (si_درازی == 2) {
                                value|=(uint32_t)si_بایتها[1]<<8;
                                value|=(uint32_t)si_بایتها[0]<<16;
                                length = 3;
                            }
                            prev_درازی=1;
                        }
                    } وإلا {
                        إذا(prev_درازی==2) {
                            length=2;
                        } وإلا {
                            /* change from single-byte mode to double-byte */
                            إذا (so_درازی == 1) {
                                value|=(uint32_t)so_بایتها[0]<<16;
                                length = 3;
                            } وإلا إذا (so_درازی == 2) {
                                value|=(uint32_t)so_بایتها[1]<<16;
                                value|=(uint32_t)so_بایتها[0]<<24;
                                length = 4;
                            }
                            prev_درازی=2;
                        }
                    }
                    اكسر;
                حالة MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=((ثابت uint16_t *)بایتها)[value +(c&0x3f)];
                    إذا(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        اذهب unassigned;
                    } وإلا {
                        length=2;
                    }
                    اكسر;
                حالة MBCS_OUTPUT_3:
                    p=بایتها+(value+(c&0x3f))*3;
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
                    value=((ثابت uint32_t *)بایتها)[value +(c&0x3f)];
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
                    value=((ثابت uint16_t *)بایتها)[value +(c&0x3f)];
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
                    p=بایتها+(value+(c&0x3f))*3;
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
                إذا(U16_IS_SURROGATE(c) && !(unicode_ماسک&UCNV_HAS_SURROGATES)) {
                    إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        إذا(منبع<منبع_حد) {
                            /* test the following code unit */
                            char16_t trail=*منبع;
                            إذا(U16_IS_TRAIL(trail)) {
                                ++منبع;
                                ++بعدی_منبع_شاخص;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                إذا(!(unicode_ماسک&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    cnv->from_Unicode_وضعیت=prev_درازی; /* save the old state */
                                    /* callback(unassigned) */
                                    اذهب unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } وإلا {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                                اكسر;
                            }
                        } وإلا {
                            /* no more input */
                            اكسر;
                        }
                    } وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_کد=U_ILLEGAL_CHAR_یافت;
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
                stage2_مدخل=MBCS_STAGE_2_FROM_U(جدول, c);

                /* get the bytes and the length for the output */
                بدل(outputType) {
                حالة MBCS_OUTPUT_2:
                    value=MBCS_مقدار_2_FROM_STAGE_2(بایتها, stage2_مدخل, c);
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
                    cnv->from_Unicode_وضعیت=prev_درازی; /* save the old state */
                    value=MBCS_مقدار_2_FROM_STAGE_2(بایتها, stage2_مدخل, c);
                    إذا(value<=0xff) {
                        إذا(value==0 && MBCS_FROM_U_IS_ROUNDTRIP(stage2_مدخل, c)==0) {
                            /* no mapping, leave value==0 */
                            length=0;
                        } وإلا إذا(prev_درازی<=1) {
                            length=1;
                        } وإلا {
                            /* change from double-byte mode to single-byte */
                            إذا (si_درازی == 1) {
                                value|=(uint32_t)si_بایتها[0]<<8;
                                length = 2;
                            } وإلا إذا (si_درازی == 2) {
                                value|=(uint32_t)si_بایتها[1]<<8;
                                value|=(uint32_t)si_بایتها[0]<<16;
                                length = 3;
                            }
                            prev_درازی=1;
                        }
                    } وإلا {
                        إذا(prev_درازی==2) {
                            length=2;
                        } وإلا {
                            /* change from single-byte mode to double-byte */
                            إذا (so_درازی == 1) {
                                value|=(uint32_t)so_بایتها[0]<<16;
                                length = 3;
                            } وإلا إذا (so_درازی == 2) {
                                value|=(uint32_t)so_بایتها[1]<<16;
                                value|=(uint32_t)so_بایتها[0]<<24;
                                length = 4;
                            }
                            prev_درازی=2;
                        }
                    }
                    اكسر;
                حالة MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=MBCS_مقدار_2_FROM_STAGE_2(بایتها, stage2_مدخل, c);
                    إذا(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        value=stage2_مدخل=0; /* stage2Entry=0 to reset roundtrip flags */
                        length=0;
                    } وإلا {
                        length=2;
                    }
                    اكسر;
                حالة MBCS_OUTPUT_3:
                    p=MBCS_اشاره_گر_3_FROM_STAGE_2(بایتها, stage2_مدخل, c);
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
                    value=MBCS_مقدار_4_FROM_STAGE_2(بایتها, stage2_مدخل, c);
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
                    value=MBCS_مقدار_2_FROM_STAGE_2(بایتها, stage2_مدخل, c);
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
                    p=MBCS_اشاره_گر_3_FROM_STAGE_2(بایتها, stage2_مدخل, c);
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
                    value=stage2_مدخل=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                    اكسر;
                }

                /* is this code point assigned, or do we use fallbacks? */
                إذا(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_مدخل, c)!=0 ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_آرگومانها->منبع=منبع;
                    c=_extFromU(cnv, cnv->shared_داده,
                                c, &منبع, منبع_حد,
                                &هدف, هدف+هدف_ظرفیت,
                                &offsets, منبع_شاخص,
                                p_آرگومانها->flush,
                                p_Error_کد);
                    بعدی_منبع_شاخص+=(int32_t)(منبع-p_آرگومانها->منبع);
                    prev_درازی=cnv->from_Unicode_وضعیت; /* restore SISO state */

                    إذا(U_FAILURE(*p_Error_کد)) {
                        /* not mappable or buffer overflow */
                        اكسر;
                    } وإلا {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        هدف_ظرفیت=(int32_t)(p_آرگومانها->هدف_حد-(حرف *)هدف);

                        /* normal end of conversion: prepare for a new character */
                        إذا(offsets!=اشاره_تهی) {
                            prev_منبع_شاخص=منبع_شاخص;
                            منبع_شاخص=بعدی_منبع_شاخص;
                        }
                        استمر;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            إذا(length<=هدف_ظرفیت) {
                إذا(offsets==اشاره_تهی) {
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
                        *offsets++=منبع_شاخص;
                        U_FALLTHROUGH;
                    حالة 3:
                        *هدف++=(uint8_t)(value>>16);
                        *offsets++=منبع_شاخص;
                        U_FALLTHROUGH;
                    حالة 2:
                        *هدف++=(uint8_t)(value>>8);
                        *offsets++=منبع_شاخص;
                        U_FALLTHROUGH;
                    حالة 1:
                        *هدف++=(uint8_t)value;
                        *offsets++=منبع_شاخص;
                        U_FALLTHROUGH;
                    افتراضي:
                        /* will never occur */
                        اكسر;
                    }
                }
                هدف_ظرفیت-=length;
            } وإلا {
                uint8_t *char_Error_میانگیر;

                /*
                 * We actually do this backwards here:
                 * In order to save an intermediate variable, we output
                 * first to the overflow buffer what does not fit into the
                 * regular target.
                 */
                /* we know that 1<=targetCapacity<length<=4 */
                length-=هدف_ظرفیت;
                char_Error_میانگیر=(uint8_t *)cnv->char_Error_میانگیر;
                بدل(length) {
                    /* each branch falls through to the next one */
                حالة 3:
                    *char_Error_میانگیر++=(uint8_t)(value>>16);
                    U_FALLTHROUGH;
                حالة 2:
                    *char_Error_میانگیر++=(uint8_t)(value>>8);
                    U_FALLTHROUGH;
                حالة 1:
                    *char_Error_میانگیر=(uint8_t)value;
                    U_FALLTHROUGH;
                افتراضي:
                    /* will never occur */
                    اكسر;
                }
                cnv->char_Error_میانگیر_درازی=(int8_t)length;

                /* now output what fits into the regular target */
                value>>=8*length; /* length was reduced by targetCapacity */
                بدل(هدف_ظرفیت) {
                    /* each branch falls through to the next one */
                حالة 3:
                    *هدف++=(uint8_t)(value>>16);
                    إذا(offsets!=اشاره_تهی) {
                        *offsets++=منبع_شاخص;
                    }
                    U_FALLTHROUGH;
                حالة 2:
                    *هدف++=(uint8_t)(value>>8);
                    إذا(offsets!=اشاره_تهی) {
                        *offsets++=منبع_شاخص;
                    }
                    U_FALLTHROUGH;
                حالة 1:
                    *هدف++=(uint8_t)value;
                    إذا(offsets!=اشاره_تهی) {
                        *offsets++=منبع_شاخص;
                    }
                    U_FALLTHROUGH;
                افتراضي:
                    /* will never occur */
                    اكسر;
                }

                /* target overflow */
                هدف_ظرفیت=0;
                *p_Error_کد=U_میانگیر_سرریز_ERROR;
                c=0;
                اكسر;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            إذا(offsets!=اشاره_تهی) {
                prev_منبع_شاخص=منبع_شاخص;
                منبع_شاخص=بعدی_منبع_شاخص;
            }
            استمر;
        } وإلا {
            /* target is full */
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
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
    إذا( U_SUCCESS(*p_Error_کد) &&
        outputType==MBCS_OUTPUT_2_SISO && prev_درازی==2 &&
        p_آرگومانها->flush && منبع>=منبع_حد && c==0
    ) {
        /* EBCDIC_STATEFUL ending with DBCS: emit an SI to return the output stream to SBCS */
        إذا(هدف_ظرفیت>0) {
            *هدف++ = si_بایتها[0];
            إذا (si_درازی == 2) {
                إذا (هدف_ظرفیت<2) {
                    cnv->char_Error_میانگیر[0] = si_بایتها[1];
                    cnv->char_Error_میانگیر_درازی=1;
                    *p_Error_کد=U_میانگیر_سرریز_ERROR;
                } وإلا {
                    *هدف++ = si_بایتها[1];
                }
            }
            إذا(offsets!=اشاره_تهی) {
                /* set the last source character's index (sourceIndex points at sourceLimit now) */
                *offsets++=prev_منبع_شاخص;
            }
        } وإلا {
            /* target is full */
            cnv->char_Error_میانگیر[0] = si_بایتها[0];
            إذا (si_درازی == 2) {
                cnv->char_Error_میانگیر[1] = si_بایتها[1];
            }
            cnv->char_Error_میانگیر_درازی=si_درازی;
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
        }
        prev_درازی=1; /* we switched into SBCS */
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;
    cnv->from_Unicode_وضعیت=prev_درازی;

    /* write back the updated pointers */
    p_آرگومانها->منبع=منبع;
    p_آرگومانها->هدف=(حرف *)هدف;
    p_آرگومانها->offsets=offsets;
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
ucnv_MBCSFromUChar32(UConverter_Shared_داده *shared_داده,
                 UChar32 c, uint32_t *p_مقدار,
                 UBool useFallback) {
    ثابت int32_t *cx;
    ثابت uint16_t *جدول;
#إذا 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
    ثابت uint8_t *p;
#پایان_اگر
    uint32_t stage2_مدخل;
    uint32_t value;
    int32_t length;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    إذا(c<=0xffff || (shared_داده->mbcs.unicode_ماسک&UCNV_HAS_SUPPLEMENTARY)) {
        جدول=shared_داده->mbcs.from_Unicode_جدول;

        /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
        إذا(shared_داده->mbcs.outputType==MBCS_OUTPUT_1) {
            value=MBCS_SINGLE_نتیجه_FROM_U(جدول, (uint16_t *)shared_داده->mbcs.from_Unicode_بایتها, c);
            /* is this code point assigned, or do we use fallbacks? */
            إذا(useFallback ? value>=0x800 : value>=0xc00) {
                *p_مقدار=value&0xff;
                أرجع 1;
            }
        } وإلا /* outputType!=MBCS_OUTPUT_1 */ {
            stage2_مدخل=MBCS_STAGE_2_FROM_U(جدول, c);

            /* get the bytes and the length for the output */
            بدل(shared_داده->mbcs.outputType) {
            حالة MBCS_OUTPUT_2:
                value=MBCS_مقدار_2_FROM_STAGE_2(shared_داده->mbcs.from_Unicode_بایتها, stage2_مدخل, c);
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
                value=MBCS_مقدار_2_FROM_STAGE_2(shared_داده->mbcs.from_Unicode_بایتها, stage2_مدخل, c);
                إذا(value<=0xff) {
                    /* no mapping or SBCS result, not taken for DBCS-only */
                    value=stage2_مدخل=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                } وإلا {
                    length=2;
                }
                اكسر;
            حالة MBCS_OUTPUT_3:
                p=MBCS_اشاره_گر_3_FROM_STAGE_2(shared_داده->mbcs.from_Unicode_بایتها, stage2_مدخل, c);
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
                value=MBCS_مقدار_4_FROM_STAGE_2(shared_داده->mbcs.from_Unicode_بایتها, stage2_مدخل, c);
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
                value=MBCS_مقدار_2_FROM_STAGE_2(shared_داده->mbcs.from_Unicode_بایتها, stage2_مدخل, c);
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
                p=MBCS_اشاره_گر_3_FROM_STAGE_2(shared_داده->mbcs.from_Unicode_بایتها, stage2_مدخل, c);
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
#پایان_اگر
            افتراضي:
                /* must not occur */
                أرجع -1;
            }

            /* is this code point assigned, or do we use fallbacks? */
            إذا( MBCS_FROM_U_IS_ROUNDTRIP(stage2_مدخل, c) ||
                (FROM_U_USE_FALLBACK(useFallback, c) && value!=0)
            ) {
                /*
                 * We allow a 0 byte output if the "assigned" bit is set for this entry.
                 * There is no way with this data structure for fallback output
                 * to be a zero byte.
                 */
                /* assigned */
                *p_مقدار=value;
                أرجع length;
            }
        }
    }

    cx=shared_داده->mbcs.extIndexes;
    إذا(cx!=اشاره_تهی) {
        length=ucnv_ext_Simple_تطابق_From_U(cx, c, p_مقدار, useFallback);
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
ucnv_MBCSSingleFromUChar32(UConverter_Shared_داده *shared_داده,
                       UChar32 c,
                       UBool useFallback) {
    ثابت uint16_t *جدول;
    int32_t value;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    إذا(c>=0x10000 && !(shared_داده->mbcs.unicode_ماسک&UCNV_HAS_SUPPLEMENTARY)) {
        أرجع -1;
    }

    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    جدول=shared_داده->mbcs.from_Unicode_جدول;

    /* get the byte for the output */
    value=MBCS_SINGLE_نتیجه_FROM_U(جدول, (uint16_t *)shared_داده->mbcs.from_Unicode_بایتها, c);
    /* is this code point assigned, or do we use fallbacks? */
    إذا(useFallback ? value>=0x800 : value>=0xc00) {
        أرجع value&0xff;
    } وإلا {
        أرجع -1;
    }
}
#پایان_اگر

/* MBCS-from-UTF-8 conversion functions ------------------------------------- */

/* offsets for n-byte UTF-8 sequences that were calculated with ((lead<<6)+trail)<<6+trail... */
ساكن ثابت UChar32
utf8_offsets[5]={ 0, 0, 0x3080, 0xE2080, 0x3C82080 };

ساكن فراغ U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_آرگومانها *pFromUArgs,
                  UConverter_مقصد_Unicode_آرگومانها *p_مقصد_UArgs,
                  UError_کد *p_Error_کد) {
    UConverter *utf8, *cnv;
    ثابت uint8_t *منبع, *منبع_حد;
    uint8_t *هدف;
    int32_t هدف_ظرفیت;

    ثابت uint16_t *جدول, *sbcs_شاخص;
    ثابت uint16_t *results;

    int8_t old_مقصد_ULength, مقصد_ULength, مقصد_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t asciiRoundtrips;
    uint16_t value, min_مقدار = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_مقصد_UArgs->converter;
    cnv=pFromUArgs->converter;
    منبع=(uint8_t *)p_مقصد_UArgs->منبع;
    منبع_حد=(uint8_t *)p_مقصد_UArgs->منبع_حد;
    هدف = reinterpret_cast<uint8_t*>(pFromUArgs->هدف);
    هدف_ظرفیت = static_cast<int32_t>(pFromUArgs->هدف_حد - pFromUArgs->هدف);

    جدول=cnv->shared_داده->mbcs.from_Unicode_جدول;
    sbcs_شاخص=cnv->shared_داده->mbcs.sbcs_شاخص;
    إذا((cnv->options&UCNV_OPTION_جابجا_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_داده->mbcs.جابجا_LFNLFrom_Unicode_بایتها);
    } وإلا {
        results=(uint16_t *)cnv->shared_داده->mbcs.from_Unicode_بایتها;
    }
    asciiRoundtrips=cnv->shared_داده->mbcs.asciiRoundtrips;

    إذا(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_مقدار=0x800;
    } وإلا {
        /* use only roundtrips and fallbacks from private-use characters */
        min_مقدار=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_داده->mbcs.unicode_ماسک & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    إذا(utf8->مقصد_ULength > 0) {
        مقصد_ULength=old_مقصد_ULength=utf8->مقصد_ULength;
        مقصد_ULimit = static_cast<int8_t>(utf8->نما);
        c = static_cast<UChar32>(utf8->مقصد_Unicode_وضعیت);
    } وإلا {
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
        int32_t length = static_cast<int32_t>(منبع_حد - منبع) - (مقصد_ULimit - old_مقصد_ULength);
        إذا(length>0) {
            uint8_t b1=*(منبع_حد-1);
            إذا(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } وإلا إذا(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(منبع_حد-2);
                إذا(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    منبع_حد-=2;
                }
            } وإلا إذا(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --منبع_حد;
            }
        }
    }

    إذا(c!=0 && هدف_ظرفیت>0) {
        utf8->مقصد_Unicode_وضعیت=0;
        utf8->مقصد_ULength=0;
        اذهب بیشتر_بایتها;
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
    طالما(منبع<منبع_حد) {
        إذا(هدف_ظرفیت>0) {
            b=*منبع++;
            إذا(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                إذا(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *هدف++ = b;
                    --هدف_ظرفیت;
                    استمر;
                } وإلا {
                    c=b;
                    value=SBCS_نتیجه_FROM_UTF8(sbcs_شاخص, results, 0, c);
                }
            } وإلا {
                إذا(b<0xe0) {
                    إذا( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*منبع - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++منبع;
                        value=SBCS_نتیجه_FROM_UTF8(sbcs_شاخص, results, c, t1);
                        إذا(value>=min_مقدار) {
                            *هدف++ = static_cast<uint8_t>(value);
                            --هدف_ظرفیت;
                            استمر;
                        } وإلا {
                            c=(c<<6)|t1;
                        }
                    } وإلا {
                        c=-1;
                    }
                } وإلا إذا(b==0xe0) {
                    إذا( /* handle U+0800..U+0FFF inline */
                        (t1 = static_cast<uint8_t>(منبع[0] - 0x80)) <= 0x3f && t1 >= 0x20 &&
                        (t2 = static_cast<uint8_t>(منبع[1] - 0x80)) <= 0x3f
                    ) {
                        c=t1;
                        منبع+=2;
                        value=SBCS_نتیجه_FROM_UTF8(sbcs_شاخص, results, c, t2);
                        إذا(value>=min_مقدار) {
                            *هدف++ = static_cast<uint8_t>(value);
                            --هدف_ظرفیت;
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
                    old_مقصد_ULength=0;
                    مقصد_ULength=1;
                    مقصد_ULimit=U8_شمار_بایتها_NON_ASCII(b);
                    c=b;
بیشتر_بایتها:
                    طالما(مقصد_ULength<مقصد_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        إذا(منبع<(uint8_t *)p_مقصد_UArgs->منبع_حد) {
                            b=*منبع;
                            إذا(icu::UTF8::isValidTrail(c, b, مقصد_ULength, مقصد_ULimit)) {
                                ++منبع;
                                ++مقصد_ULength;
                                c=(c<<6)+b;
                            } وإلا {
                                اكسر; /* sequence too short, stop with toULength<toULimit */
                            }
                        } وإلا {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            منبع-=(مقصد_ULength-old_مقصد_ULength);
                            طالما(old_مقصد_ULength<مقصد_ULength) {
                                utf8->مقصد_UBytes[old_مقصد_ULength++]=*منبع++;
                            }
                            utf8->مقصد_Unicode_وضعیت=c;
                            utf8->مقصد_ULength=مقصد_ULength;
                            utf8->نما=مقصد_ULimit;
                            p_مقصد_UArgs->منبع=(حرف *)منبع;
                            pFromUArgs->هدف = reinterpret_cast<حرف*>(هدف);
                            أرجع;
                        }
                    }

                    إذا(مقصد_ULength==مقصد_ULimit) {
                        c-=utf8_offsets[مقصد_ULength];
                        إذا(مقصد_ULength<=3) {  /* BMP */
                            value=MBCS_SINGLE_نتیجه_FROM_U(جدول, results, c);
                        } وإلا {
                            /* supplementary code point */
                            إذا(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                value=0;
                            } وإلا {
                                value=MBCS_SINGLE_نتیجه_FROM_U(جدول, results, c);
                            }
                        }
                    } وإلا {
                        /* error handling: illegal UTF-8 byte sequence */
                        منبع-=(مقصد_ULength-old_مقصد_ULength);
                        طالما(old_مقصد_ULength<مقصد_ULength) {
                            utf8->مقصد_UBytes[old_مقصد_ULength++]=*منبع++;
                        }
                        utf8->مقصد_ULength=مقصد_ULength;
                        p_مقصد_UArgs->منبع=(حرف *)منبع;
                        pFromUArgs->هدف = reinterpret_cast<حرف*>(هدف);
                        *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                        أرجع;
                    }
                }
            }

            إذا(value>=min_مقدار) {
                /* output the mapping for c */
                *هدف++ = static_cast<uint8_t>(value);
                --هدف_ظرفیت;
            } وإلا {
                /* value<minValue means c is unassigned (unmappable) */
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                ساكن ثابت char16_t nul=0;
                ثابت char16_t *no_منبع=&nul;
                c=_extFromU(cnv, cnv->shared_داده,
                            c, &no_منبع, no_منبع,
                            &هدف, هدف+هدف_ظرفیت,
                            اشاره_تهی, -1,
                            pFromUArgs->flush,
                            p_Error_کد);

                إذا(U_FAILURE(*p_Error_کد)) {
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
                    *p_Error_کد=U_USING_DEFAULT_WARNING;
                    اكسر;
                } وإلا {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    هدف_ظرفیت = static_cast<int32_t>(pFromUArgs->هدف_حد - reinterpret_cast<حرف*>(هدف));
                }
            }
        } وإلا {
            /* target is full */
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
            اكسر;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    إذا(U_SUCCESS(*p_Error_کد) &&
            cnv->preFromUFirstCP<0 &&
            منبع<(منبع_حد=(uint8_t *)p_مقصد_UArgs->منبع_حد)) {
        c=utf8->مقصد_UBytes[0]=b=*منبع++;
        مقصد_ULength=1;
        مقصد_ULimit=U8_شمار_بایتها(b);
        طالما(منبع<منبع_حد) {
            utf8->مقصد_UBytes[مقصد_ULength++]=b=*منبع++;
            c=(c<<6)+b;
        }
        utf8->مقصد_Unicode_وضعیت=c;
        utf8->مقصد_ULength=مقصد_ULength;
        utf8->نما=مقصد_ULimit;
    }

    /* write back the updated pointers */
    p_مقصد_UArgs->منبع=(حرف *)منبع;
    pFromUArgs->هدف = reinterpret_cast<حرف*>(هدف);
}

ساكن فراغ U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_آرگومانها *pFromUArgs,
                  UConverter_مقصد_Unicode_آرگومانها *p_مقصد_UArgs,
                  UError_کد *p_Error_کد) {
    UConverter *utf8, *cnv;
    ثابت uint8_t *منبع, *منبع_حد;
    uint8_t *هدف;
    int32_t هدف_ظرفیت;

    ثابت uint16_t *جدول, *mbcs_شاخص;
    ثابت uint16_t *results;

    int8_t old_مقصد_ULength, مقصد_ULength, مقصد_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t stage2_مدخل;
    uint32_t asciiRoundtrips;
    uint16_t value = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_مقصد_UArgs->converter;
    cnv=pFromUArgs->converter;
    منبع=(uint8_t *)p_مقصد_UArgs->منبع;
    منبع_حد=(uint8_t *)p_مقصد_UArgs->منبع_حد;
    هدف = reinterpret_cast<uint8_t*>(pFromUArgs->هدف);
    هدف_ظرفیت = static_cast<int32_t>(pFromUArgs->هدف_حد - pFromUArgs->هدف);

    جدول=cnv->shared_داده->mbcs.from_Unicode_جدول;
    mbcs_شاخص=cnv->shared_داده->mbcs.mbcs_شاخص;
    إذا((cnv->options&UCNV_OPTION_جابجا_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_داده->mbcs.جابجا_LFNLFrom_Unicode_بایتها);
    } وإلا {
        results=(uint16_t *)cnv->shared_داده->mbcs.from_Unicode_بایتها;
    }
    asciiRoundtrips=cnv->shared_داده->mbcs.asciiRoundtrips;

    hasSupplementary = static_cast<UBool>(cnv->shared_داده->mbcs.unicode_ماسک & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    إذا(utf8->مقصد_ULength > 0) {
        مقصد_ULength=old_مقصد_ULength=utf8->مقصد_ULength;
        مقصد_ULimit = static_cast<int8_t>(utf8->نما);
        c = static_cast<UChar32>(utf8->مقصد_Unicode_وضعیت);
    } وإلا {
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
        int32_t length = static_cast<int32_t>(منبع_حد - منبع) - (مقصد_ULimit - old_مقصد_ULength);
        إذا(length>0) {
            uint8_t b1=*(منبع_حد-1);
            إذا(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } وإلا إذا(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(منبع_حد-2);
                إذا(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    منبع_حد-=2;
                }
            } وإلا إذا(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --منبع_حد;
            }
        }
    }

    إذا(c!=0 && هدف_ظرفیت>0) {
        utf8->مقصد_Unicode_وضعیت=0;
        utf8->مقصد_ULength=0;
        اذهب بیشتر_بایتها;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    طالما(منبع<منبع_حد) {
        إذا(هدف_ظرفیت>0) {
            b=*منبع++;
            إذا(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                إذا(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *هدف++=b;
                    --هدف_ظرفیت;
                    استمر;
                } وإلا {
                    value=DBCS_نتیجه_FROM_UTF8(mbcs_شاخص, results, 0, b);
                    إذا(value==0) {
                        c=b;
                        اذهب unassigned;
                    }
                }
            } وإلا {
                إذا(b>=0xe0) {
                    إذا( /* handle U+0800..U+D7FF inline */
                        b<=0xed &&  // do not assume maxFastUChar>0xd7ff
                        U8_IS_VALID_LEAD3_AND_T1(b, t1=منبع[0]) &&
                        (t2 = static_cast<uint8_t>(منبع[1] - 0x80)) <= 0x3f
                    ) {
                        c=((b&0xf)<<6)|(t1&0x3f);
                        منبع+=2;
                        value=DBCS_نتیجه_FROM_UTF8(mbcs_شاخص, results, c, t2);
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
                        (t1 = static_cast<uint8_t>(*منبع - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++منبع;
                        value=DBCS_نتیجه_FROM_UTF8(mbcs_شاخص, results, c, t1);
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
                    old_مقصد_ULength=0;
                    مقصد_ULength=1;
                    مقصد_ULimit=U8_شمار_بایتها_NON_ASCII(b);
                    c=b;
بیشتر_بایتها:
                    طالما(مقصد_ULength<مقصد_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        إذا(منبع<(uint8_t *)p_مقصد_UArgs->منبع_حد) {
                            b=*منبع;
                            إذا(icu::UTF8::isValidTrail(c, b, مقصد_ULength, مقصد_ULimit)) {
                                ++منبع;
                                ++مقصد_ULength;
                                c=(c<<6)+b;
                            } وإلا {
                                اكسر; /* sequence too short, stop with toULength<toULimit */
                            }
                        } وإلا {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            منبع-=(مقصد_ULength-old_مقصد_ULength);
                            طالما(old_مقصد_ULength<مقصد_ULength) {
                                utf8->مقصد_UBytes[old_مقصد_ULength++]=*منبع++;
                            }
                            utf8->مقصد_Unicode_وضعیت=c;
                            utf8->مقصد_ULength=مقصد_ULength;
                            utf8->نما=مقصد_ULimit;
                            p_مقصد_UArgs->منبع=(حرف *)منبع;
                            pFromUArgs->هدف = reinterpret_cast<حرف*>(هدف);
                            أرجع;
                        }
                    }

                    إذا(مقصد_ULength==مقصد_ULimit) {
                        c-=utf8_offsets[مقصد_ULength];
                        إذا(مقصد_ULength<=3) {  /* BMP */
                            stage2_مدخل=MBCS_STAGE_2_FROM_U(جدول, c);
                        } وإلا {
                            /* supplementary code point */
                            إذا(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                stage2_مدخل=0;
                            } وإلا {
                                stage2_مدخل=MBCS_STAGE_2_FROM_U(جدول, c);
                            }
                        }
                    } وإلا {
                        /* error handling: illegal UTF-8 byte sequence */
                        منبع-=(مقصد_ULength-old_مقصد_ULength);
                        طالما(old_مقصد_ULength<مقصد_ULength) {
                            utf8->مقصد_UBytes[old_مقصد_ULength++]=*منبع++;
                        }
                        utf8->مقصد_ULength=مقصد_ULength;
                        p_مقصد_UArgs->منبع=(حرف *)منبع;
                        pFromUArgs->هدف = reinterpret_cast<حرف*>(هدف);
                        *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                        أرجع;
                    }

                    /* get the bytes and the length for the output */
                    /* MBCS_OUTPUT_2 */
                    value=MBCS_مقدار_2_FROM_STAGE_2(results, stage2_مدخل, c);

                    /* is this code point assigned, or do we use fallbacks? */
                    إذا(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_مدخل, c) ||
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
                --هدف_ظرفیت;
            } وإلا /* length==2 */ {
                *هدف++ = static_cast<uint8_t>(value >> 8);
                إذا(2<=هدف_ظرفیت) {
                    *هدف++ = static_cast<uint8_t>(value);
                    هدف_ظرفیت-=2;
                } وإلا {
                    cnv->char_Error_میانگیر[0] = static_cast<حرف>(value);
                    cnv->char_Error_میانگیر_درازی=1;

                    /* target overflow */
                    *p_Error_کد=U_میانگیر_سرریز_ERROR;
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
                ثابت char16_t *no_منبع=&nul;
                c=_extFromU(cnv, cnv->shared_داده,
                            c, &no_منبع, no_منبع,
                            &هدف, هدف+هدف_ظرفیت,
                            اشاره_تهی, -1,
                            pFromUArgs->flush,
                            p_Error_کد);

                إذا(U_FAILURE(*p_Error_کد)) {
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
                    *p_Error_کد=U_USING_DEFAULT_WARNING;
                    اكسر;
                } وإلا {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    هدف_ظرفیت = static_cast<int32_t>(pFromUArgs->هدف_حد - reinterpret_cast<حرف*>(هدف));
                    استمر;
                }
            }
        } وإلا {
            /* target is full */
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
            اكسر;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    إذا(U_SUCCESS(*p_Error_کد) &&
            cnv->preFromUFirstCP<0 &&
            منبع<(منبع_حد=(uint8_t *)p_مقصد_UArgs->منبع_حد)) {
        c=utf8->مقصد_UBytes[0]=b=*منبع++;
        مقصد_ULength=1;
        مقصد_ULimit=U8_شمار_بایتها(b);
        طالما(منبع<منبع_حد) {
            utf8->مقصد_UBytes[مقصد_ULength++]=b=*منبع++;
            c=(c<<6)+b;
        }
        utf8->مقصد_Unicode_وضعیت=c;
        utf8->مقصد_ULength=مقصد_ULength;
        utf8->نما=مقصد_ULimit;
    }

    /* write back the updated pointers */
    p_مقصد_UArgs->منبع=(حرف *)منبع;
    pFromUArgs->هدف = reinterpret_cast<حرف*>(هدف);
}

/* miscellaneous ------------------------------------------------------------ */

ساكن فراغ U_CALLCONV
ucnv_MBCSGetStarters(ثابت UConverter* cnv,
                 UBool starters[256],
                 UError_کد *) {
    ثابت int32_t *state0;
    صحيح i;

    state0=cnv->shared_داده->mbcs.state_جدول[cnv->shared_داده->mbcs.dbcsOnlyState];
    لكل(i=0; i<256; ++i) {
        /* all bytes that cause a state transition from state 0 are lead bytes */
        starters[i] = static_cast<UBool>(MBCS_مدخل_IS_TRANSITION(state0[i]));
    }
}

/*
 * This is an internal function that allows other converter implementations
 * to check whether a byte is a lead byte.
 */
U_CFUNC UBool
ucnv_MBCSIsLeadByte(UConverter_Shared_داده *shared_داده, حرف byte) {
    أرجع MBCS_مدخل_IS_TRANSITION(shared_داده->mbcs.state_جدول[0][(uint8_t)byte]);
}

ساكن فراغ U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_آرگومانها *p_آرگومانها,
              int32_t جابه_جایی_شاخص,
              UError_کد *p_Error_کد) {
    UConverter *cnv=p_آرگومانها->converter;
    حرف *p, *subchar;
    حرف میانگیر[4];
    int32_t length;

    /* first, select between subChar and subChar1 */
    إذا( cnv->subChar1!=0 &&
        (cnv->shared_داده->mbcs.extIndexes!=اشاره_تهی ?
            cnv->useSubChar1 :
            (cnv->invalid_UChar_میانگیر[0]<=0xff))
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

    إذا (cnv->shared_داده->mbcs.outputType == MBCS_OUTPUT_2_SISO) {
        p=میانگیر;

        /* fromUnicodeStatus contains prevLength */
        بدل(length) {
        حالة 1:
            إذا(cnv->from_Unicode_وضعیت==2) {
                /* DBCS mode and SBCS sub char: change to SBCS */
                cnv->from_Unicode_وضعیت=1;
                *p++=UCNV_SI;
            }
            *p++=subchar[0];
            اكسر;
        حالة 2:
            إذا(cnv->from_Unicode_وضعیت<=1) {
                /* SBCS mode and DBCS sub char: change to DBCS */
                cnv->from_Unicode_وضعیت=2;
                *p++=UCNV_SO;
            }
            *p++=subchar[0];
            *p++=subchar[1];
            اكسر;
        افتراضي:
            *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
            أرجع;
        }
        subchar=میانگیر;
        length = static_cast<int32_t>(p - میانگیر);
    }

    ucnv_cb_From_UWrite_بایتها(p_آرگومانها, subchar, length, جابه_جایی_شاخص, p_Error_کد);
}

U_CFUNC UConverterType
ucnv_MBCSGetType(ثابت UConverter* converter) {
    /* SBCS, DBCS, and EBCDIC_STATEFUL are replaced by MBCS, but here we cheat a little */
    إذا(converter->shared_داده->mbcs.شمار_حالتها==1) {
        أرجع (UConverterType)UCNV_SBCS;
    } وإلا إذا((converter->shared_داده->mbcs.outputType&0xff)==MBCS_OUTPUT_2_SISO) {
        أرجع (UConverterType)UCNV_EBCDIC_STATEFUL;
    } وإلا إذا(converter->shared_داده->static_داده->min_بایتها_در_هر_Char==2 && converter->shared_داده->static_داده->max_بایتها_در_هر_Char==2) {
        أرجع (UConverterType)UCNV_DBCS;
    }
    أرجع (UConverterType)UCNV_MBCS;
}

#پایان_اگر /* #if !UCONFIG_NO_LEGACY_CONVERSION */
