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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#समावेश "unicode/ucnv.h"
#समावेश "unicode/ucnv_cb.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/uset.h"
#समावेश "unicode/utf8.h"
#समावेश "unicode/utf16.h"
#समावेश "ucnv_bld.h"
#समावेश "ucnvmbcs.h"
#समावेश "ucnv_ext.h"
#समावेश "ucnv_cnv.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "umutex.h"
#समावेश "ustr_imp.h"

/* control optimizations according to the platform */
#घोषणा MBCS_UNROLL_SINGLE_TO_BMP 1
#घोषणा MBCS_UNROLL_SINGLE_FROM_BMP 0

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
प्रकार UBool U_CALLCONV
UConverterEnumToUCallback(स्थिर व्योम *context, uint32_t value, UChar32 codePoints[32]);

अटल व्योम U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_तथ्य *shared_तथ्य,
          UConverterLoadArgs *pArgs,
          स्थिर uint8_t *raw,
          UErrorCode *pErrorCode);

अटल व्योम U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_तथ्य *shared_तथ्य);

अटल व्योम U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverterLoadArgs *pArgs,
              UErrorCode *pErrorCode);

अटल UChar32 U_CALLCONV
ucnv_MBCSGet_अग्र_UChar(UConverterToUnicodeArgs *pArgs,
                  UErrorCode *pErrorCode);

अटल व्योम U_CALLCONV
ucnv_MBCSGetStarters(स्थिर UConverter* cnv,
                 UBool starters[256],
                 UErrorCode *pErrorCode);

U_CDECL_BEGIN
अटल स्थिर अक्षर* U_CALLCONV
ucnv_MBCSGetName(स्थिर UConverter *cnv);
U_CDECL_END

अटल व्योम U_CALLCONV
ucnv_MBCSWriteSub(UConverterFromUnicodeArgs *pArgs,
              int32_t offset_सूचकांक,
              UErrorCode *pErrorCode);

अटल UChar32 U_CALLCONV
ucnv_MBCSGet_अग्र_UChar(UConverterToUnicodeArgs *pArgs,
                  UErrorCode *pErrorCode);

अटल व्योम U_CALLCONV
ucnv_SBCSFromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                  UConverterToUnicodeArgs *pToUArgs,
                  UErrorCode *pErrorCode);

अटल व्योम U_CALLCONV
ucnv_MBCSGetUnicodeSet(स्थिर UConverter *cnv,
                   स्थिर USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode);

अटल व्योम U_CALLCONV
ucnv_DBCSFromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                  UConverterToUnicodeArgs *pToUArgs,
                  UErrorCode *pErrorCode);

अटल स्थिर UConverterImpl _SBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    nullptr,
    nullptr,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_अग्र_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    nullptr,
    ucnv_MBCSGetUnicodeSet,

    nullptr,
    ucnv_SBCSFromUTF8
};

अटल स्थिर UConverterImpl _DBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    nullptr,
    nullptr,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_अग्र_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    nullptr,
    ucnv_MBCSGetUnicodeSet,

    nullptr,
    ucnv_DBCSFromUTF8
};

अटल स्थिर UConverterImpl _MBCSImpl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    nullptr,
    nullptr,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_अग्र_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    nullptr,
    ucnv_MBCSGetUnicodeSet,
    nullptr,
    nullptr
};

/* Static data is in tools/makeconv/ucnvstat.c for data-based
 * converters. Be sure to update it as well.
 */

स्थिर UConverter_Shared_तथ्य _MBCSData={
    माप(UConverter_Shared_तथ्य), 1,
    nullptr, nullptr, false, true, &_MBCSImpl,
    0, UCNV_MBCS_TABLE_INITIALIZER
};


/* GB 18030 data ------------------------------------------------------------ */

/* helper macros for linear values for GB 18030 four-byte sequences */
#घोषणा LINEAR_18030(a, b, c, d) ((((a)*10+(b))*126L+(c))*10L+(d))

#घोषणा LINEAR_18030_BASE LINEAR_18030(0x81, 0x30, 0x81, 0x30)

#घोषणा LINEAR(x) LINEAR_18030(x>>24, (x>>16)&0xff, (x>>8)&0xff, x&0xff)

/*
 * Some ranges of GB 18030 where both the Unicode code points and the
 * GB four-byte sequences are contiguous and are handled algorithmically by
 * the special callback functions below.
 * The values are start & end of Unicode & GB codes.
 *
 * Note that single surrogates are not mapped by GB 18030
 * as of the re-released mapping tables from 2000-nov-30.
 */
अटल स्थिर uint32_t
gb18030Ranges[14][4]={
    {0x10000, 0x10FFFF, LINEAR(0x90308130), LINEAR(0xE3329A35)},
    {0x9FA6, 0xD7FF, LINEAR(0x82358F33), LINEAR(0x8336C738)},
    {0x0452, 0x1E3E, LINEAR(0x8130D330), LINEAR(0x8135F436)},
    {0x1E40, 0x200F, LINEAR(0x8135F438), LINEAR(0x8136A531)},
    {0xE865, 0xF92B, LINEAR(0x8336D030), LINEAR(0x84308534)},
    {0x2643, 0x2E80, LINEAR(0x8137A839), LINEAR(0x8138FD38)},
    {0xFA2A, 0xFE2F, LINEAR(0x84309C38), LINEAR(0x84318537)},
    {0x3CE1, 0x4055, LINEAR(0x8231D438), LINEAR(0x8232AF32)},
    {0x361B, 0x3917, LINEAR(0x8230A633), LINEAR(0x8230F237)},
    {0x49B8, 0x4C76, LINEAR(0x8234A131), LINEAR(0x8234E733)},
    {0x4160, 0x4336, LINEAR(0x8232C937), LINEAR(0x8232F837)},
    {0x478E, 0x4946, LINEAR(0x8233E838), LINEAR(0x82349638)},
    {0x44D7, 0x464B, LINEAR(0x8233A339), LINEAR(0x8233C931)},
    {0xFFE6, 0xFFFF, LINEAR(0x8431A234), LINEAR(0x8431A439)}
};

/* bit flag for UConverter.options indicating GB 18030 special handling */
#घोषणा _MBCS_OPTION_GB18030 0x8000

/* bit flag for UConverter.options indicating KEIS,JEF,JIF special handling */
#घोषणा _MBCS_OPTION_KEIS 0x01000
#घोषणा _MBCS_OPTION_JEF  0x02000
#घोषणा _MBCS_OPTION_JIPS 0x04000

#घोषणा KEIS_SO_CHAR_1 0x0A
#घोषणा KEIS_SO_CHAR_2 0x42
#घोषणा KEIS_SI_CHAR_1 0x0A
#घोषणा KEIS_SI_CHAR_2 0x41

#घोषणा JEF_SO_CHAR 0x28
#घोषणा JEF_SI_CHAR 0x29

#घोषणा JIPS_SO_CHAR_1 0x1A
#घोषणा JIPS_SO_CHAR_2 0x70
#घोषणा JIPS_SI_CHAR_1 0x1A
#घोषणा JIPS_SI_CHAR_2 0x71

क्रमागत SISO_Option {
    SI,
    SO
};
प्रकार क्रमागत SISO_Option SISO_Option;

अटल int32_t getSISOBytes(SISO_Option option, uint32_t cnvOption, uint8_t *value) {
    int32_t SISOLength = 0;

    चयन (option) {
        हाल SI:
            अगर ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SI_CHAR_1;
                value[1] = KEIS_SI_CHAR_2;
                SISOLength = 2;
            } अन्यथा अगर ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SI_CHAR;
                SISOLength = 1;
            } अन्यथा अगर ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SI_CHAR_1;
                value[1] = JIPS_SI_CHAR_2;
                SISOLength = 2;
            } अन्यथा {
                value[0] = UCNV_SI;
                SISOLength = 1;
            }
            अवरोध;
        हाल SO:
            अगर ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SO_CHAR_1;
                value[1] = KEIS_SO_CHAR_2;
                SISOLength = 2;
            } अन्यथा अगर ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SO_CHAR;
                SISOLength = 1;
            } अन्यथा अगर ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SO_CHAR_1;
                value[1] = JIPS_SO_CHAR_2;
                SISOLength = 2;
            } अन्यथा {
                value[0] = UCNV_SO;
                SISOLength = 1;
            }
            अवरोध;
        शेष:
            /* Should never happen. */
            अवरोध;
    }

    वापस SISOLength;
}

/* Miscellaneous ------------------------------------------------------------ */

/* similar to ucnv_MBCSGetNextUChar() but recursive */
अटल UBool
enumToU(UConverterMBCSTable *mbcsTable, int8_t stateProps[],
        int32_t state, uint32_t offset,
        uint32_t value,
        UConverterEnumToUCallback *callback, स्थिर व्योम *context,
        UErrorCode *pErrorCode) {
    UChar32 codePoints[32];
    स्थिर int32_t *row;
    स्थिर uint16_t *unicodeCodeUnits;
    UChar32 anyCodePoints;
    int32_t b, limit;

    row=mbcsTable->stateTable[state];
    unicodeCodeUnits=mbcsTable->unicodeCodeUnits;

    value<<=8;
    anyCodePoints=-1;  /* becomes non-negative if there is a mapping */

    b=(stateProps[state]&0x38)<<2;
    अगर(b==0 && stateProps[state]>=0x40) {
        /* skip byte sequences with leading zeros because they are not stored in the fromUnicode table */
        codePoints[0]=U_SENTINEL;
        b=1;
    }
    limit=((stateProps[state]&7)+1)<<5;
    जबतक(b<limit) {
        int32_t entry=row[b];
        अगर(MBCS_ENTRY_IS_TRANSITION(entry)) {
            int32_t अग्र_State=MBCS_ENTRY_TRANSITION_STATE(entry);
            अगर(stateProps[अग्र_State]>=0) {
                /* recurse to a state with non-ignorable actions */
                अगर(!enumToU(
                        mbcsTable, stateProps, अग्र_State,
                        offset+MBCS_ENTRY_TRANSITION_OFFSET(entry),
                        value | static_cast<uint32_t>(b),
                        callback, context,
                        pErrorCode)) {
                    वापस false;
                }
            }
            codePoints[b&0x1f]=U_SENTINEL;
        } अन्यथा {
            UChar32 c;
            int32_t action;

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=MBCS_ENTRY_FINAL_ACTION(entry);
            अगर(action==MBCS_STATE_VALID_DIRECT_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            } अन्यथा अगर(action==MBCS_STATE_VALID_16) {
                int32_t finalOffset=offset+MBCS_ENTRY_FINAL_मूल्य_16(entry);
                c=unicodeCodeUnits[finalOffset];
                अगर(c<0xfffe) {
                    /* output BMP code point */
                } अन्यथा {
                    c=U_SENTINEL;
                }
            } अन्यथा अगर(action==MBCS_STATE_VALID_16_PAIR) {
                int32_t finalOffset=offset+MBCS_ENTRY_FINAL_मूल्य_16(entry);
                c=unicodeCodeUnits[finalOffset++];
                अगर(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } अन्यथा अगर(c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicodeCodeUnits[finalOffset]+(0x10000-0xdc00);
                } अन्यथा अगर(c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicodeCodeUnits[finalOffset];
                } अन्यथा {
                    c=U_SENTINEL;
                }
            } अन्यथा अगर(action==MBCS_STATE_VALID_DIRECT_20) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_ENTRY_FINAL_मूल्य(entry) + 0x10000);
            } अन्यथा {
                c=U_SENTINEL;
            }

            codePoints[b&0x1f]=c;
            anyCodePoints&=c;
        }
        अगर(((++b)&0x1f)==0) {
            अगर(anyCodePoints>=0) {
                अगर (!callback(context, value | static_cast<uint32_t>(b - 0x20), codePoints)) {
                    वापस false;
                }
                anyCodePoints=-1;
            }
        }
    }
    वापस true;
}

/*
 * Only called if stateProps[state]==-1.
 * A recursive call may do stateProps[state]|=0x40 if this state is the target of an
 * MBCS_STATE_CHANGE_ONLY.
 */
अटल int8_t
getStateProp(स्थिर int32_t (*stateTable)[256], int8_t stateProps[], पूर्णांक state) {
    स्थिर int32_t *row;
    int32_t min, max, entry, अग्र_State;

    row=stateTable[state];
    stateProps[state]=0;

    /* find first non-ignorable state */
    क्रम(min=0;; ++min) {
        entry=row[min];
        अग्र_State=MBCS_ENTRY_STATE(entry);
        अगर(stateProps[अग्र_State]==-1) {
            getStateProp(stateTable, stateProps, अग्र_State);
        }
        अगर(MBCS_ENTRY_IS_TRANSITION(entry)) {
            अगर(stateProps[अग्र_State]>=0) {
                अवरोध;
            }
        } अन्यथा अगर(MBCS_ENTRY_FINAL_ACTION(entry)<MBCS_STATE_UNASSIGNED) {
            अवरोध;
        }
        अगर(min==0xff) {
            stateProps[state]=-0x40;  /* (int8_t)0xc0 */
            वापस stateProps[state];
        }
    }
    stateProps[state] |= static_cast<int8_t>((min >> 5) << 3);

    /* find last non-ignorable state */
    क्रम(max=0xff; min<max; --max) {
        entry=row[max];
        अग्र_State=MBCS_ENTRY_STATE(entry);
        अगर(stateProps[अग्र_State]==-1) {
            getStateProp(stateTable, stateProps, अग्र_State);
        }
        अगर(MBCS_ENTRY_IS_TRANSITION(entry)) {
            अगर(stateProps[अग्र_State]>=0) {
                अवरोध;
            }
        } अन्यथा अगर(MBCS_ENTRY_FINAL_ACTION(entry)<MBCS_STATE_UNASSIGNED) {
            अवरोध;
        }
    }
    stateProps[state] |= static_cast<int8_t>(max >> 5);

    /* recurse further and collect direct-state information */
    जबतक(min<=max) {
        entry=row[min];
        अग्र_State=MBCS_ENTRY_STATE(entry);
        अगर(stateProps[अग्र_State]==-1) {
            getStateProp(stateTable, stateProps, अग्र_State);
        }
        अगर(MBCS_ENTRY_IS_FINAL(entry)) {
            stateProps[अग्र_State]|=0x40;
            अगर(MBCS_ENTRY_FINAL_ACTION(entry)<=MBCS_STATE_FALLBACK_DIRECT_20) {
                stateProps[state]|=0x40;
            }
        }
        ++min;
    }
    वापस stateProps[state];
}

/*
 * Internal function enumerating the toUnicode data of an MBCS converter.
 * Currently only used for reconstituting data for a MBCS_OPT_NO_FROM_U
 * table, but could also be used for a future ucnv_getUnicodeSet() option
 * that includes reverse fallbacks (after updating this function's implementation).
 * Currently only handles roundtrip mappings.
 * Does not currently handle extensions.
 */
अटल व्योम
ucnv_MBCSEnumToUnicode(UConverterMBCSTable *mbcsTable,
                       UConverterEnumToUCallback *callback, स्थिर व्योम *context,
                       UErrorCode *pErrorCode) {
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
    int8_t stateProps[MBCS_MAX_STATE_गिनती];
    int32_t state;

    uprv_memset(stateProps, -1, माप(stateProps));

    /* recurse from state 0 and set all stateProps */
    getStateProp(mbcsTable->stateTable, stateProps, 0);

    क्रम(state=0; state<mbcsTable->गिनती_States; ++state) {
        /*if(stateProps[state]==-1) {
            printf("unused/unreachable <icu:state> %d\n", state);
        }*/
        अगर(stateProps[state]>=0x40) {
            /* start from each direct state */
            enumToU(
                mbcsTable, stateProps, state, 0, 0,
                callback, context,
                pErrorCode);
        }
    }
}

U_CFUNC व्योम 
ucnv_MBCSGetFilteredUnicodeSetForUnicode(स्थिर UConverter_Shared_तथ्य *shared_तथ्य,
                                         स्थिर USetAdder *sa,
                                         UConverterUnicodeSet which,
                                         UConverterSetFilter filter,
                                         UErrorCode *pErrorCode) {
    स्थिर UConverterMBCSTable *mbcsTable;
    स्थिर uint16_t *table;

    uint32_t st3;
    uint16_t st1, maxStage1, st2;

    UChar32 c;

    /* enumerate the from-Unicode trie table */
    mbcsTable=&shared_तथ्य->mbcs;
    table=mbcsTable->fromUnicodeTable;
    अगर(mbcsTable->unicodeMask&UCNV_HAS_SUPPLEMENTARY) {
        maxStage1=0x440;
    } अन्यथा {
        maxStage1=0x40;
    }

    c=0; /* keep track of the current code point while enumerating */

    अगर(mbcsTable->outputType==MBCS_OUTPUT_1) {
        स्थिर uint16_t *stage2, *stage3, *results;
        uint16_t min_मूल्य;

        results=(स्थिर uint16_t *)mbcsTable->fromUnicodeBytes;

        /*
         * Set a threshold variable for selecting which mappings to use.
         * See ucnv_MBCSSingleFromBMPWithOffsets() and
         * MBCS_SINGLE_RESULT_FROM_U() for details.
         */
        अगर(which==UCNV_ROUNDTRIP_SET) {
            /* use only roundtrips */
            min_मूल्य=0xf00;
        } अन्यथा /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
            /* use all roundtrip and fallback results */
            min_मूल्य=0x800;
        }

        क्रम(st1=0; st1<maxStage1; ++st1) {
            st2=table[st1];
            अगर(st2>maxStage1) {
                stage2=table+st2;
                क्रम(st2=0; st2<64; ++st2) {
                    अगर((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=results+st3;

                        करो {
                            अगर(*stage3++>=min_मूल्य) {
                                sa->add(sa->set, c);
                            }
                        } जबतक((++c&0xf)!=0);
                    } अन्यथा {
                        c+=16; /* empty stage 3 block */
                    }
                }
            } अन्यथा {
                c+=1024; /* empty stage 2 block */
            }
        }
    } अन्यथा {
        स्थिर uint32_t *stage2;
        स्थिर uint8_t *stage3, *bytes;
        uint32_t st3Multiplier;
        uint32_t value;
        UBool useFallback;

        bytes=mbcsTable->fromUnicodeBytes;

        useFallback = which == UCNV_ROUNDTRIP_AND_FALLBACK_SET;

        चयन(mbcsTable->outputType) {
        हाल MBCS_OUTPUT_3:
        हाल MBCS_OUTPUT_4_EUC:
            st3Multiplier=3;
            अवरोध;
        हाल MBCS_OUTPUT_4:
            st3Multiplier=4;
            अवरोध;
        शेष:
            st3Multiplier=2;
            अवरोध;
        }

        क्रम(st1=0; st1<maxStage1; ++st1) {
            st2=table[st1];
            अगर(st2>(maxStage1>>1)) {
                stage2=(स्थिर uint32_t *)table+st2;
                क्रम(st2=0; st2<64; ++st2) {
                    अगर((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=bytes+st3Multiplier*16*(uint32_t)(uint16_t)st3;

                        /* get the roundtrip flags for the stage 3 block */
                        st3>>=16;

                        /*
                         * Add code points for which the roundtrip flag is set,
                         * or which map to non-zero bytes if we use fallbacks.
                         * See ucnv_MBCSFromUnicodeWithOffsets() for details.
                         */
                        चयन(filter) {
                        हाल UCNV_SET_FILTER_NONE:
                            करो {
                                अगर(st3&1) {
                                    sa->add(sa->set, c);
                                    stage3+=st3Multiplier;
                                } अन्यथा अगर(useFallback) {
                                    uint8_t b=0;
                                    चयन(st3Multiplier) {
                                    हाल 4:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    हाल 3:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    हाल 2:
                                        b|=stage3[0]|stage3[1];
                                        stage3+=2;
                                        U_FALLTHROUGH;
                                    शेष:
                                        अवरोध;
                                    }
                                    अगर(b!=0) {
                                        sa->add(sa->set, c);
                                    }
                                }
                                st3>>=1;
                            } जबतक((++c&0xf)!=0);
                            अवरोध;
                        हाल UCNV_SET_FILTER_DBCS_ONLY:
                             /* Ignore single-byte results (<0x100). */
                            करो {
                                अगर(((st3&1)!=0 || useFallback) && *((स्थिर uint16_t *)stage3)>=0x100) {
                                    sa->add(sa->set, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } जबतक((++c&0xf)!=0);
                            अवरोध;
                        हाल UCNV_SET_FILTER_2022_CN:
                             /* Only add code points that map to CNS 11643 planes 1 & 2 for non-EXT ISO-2022-CN. */
                            करो {
                                अगर(((st3&1)!=0 || useFallback) && ((value=*stage3)==0x81 || value==0x82)) {
                                    sa->add(sa->set, c);
                                }
                                st3>>=1;
                                stage3+=3;  /* +=st3Multiplier */
                            } जबतक((++c&0xf)!=0);
                            अवरोध;
                        हाल UCNV_SET_FILTER_SJIS:
                             /* Only add code points that map to Shift-JIS codes corresponding to JIS X 0208. */
                            करो {
                                अगर(((st3&1)!=0 || useFallback) && (value=*((स्थिर uint16_t *)stage3))>=0x8140 && value<=0xeffc) {
                                    sa->add(sa->set, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } जबतक((++c&0xf)!=0);
                            अवरोध;
                        हाल UCNV_SET_FILTER_GR94DBCS:
                            /* Only add code points that map to ISO 2022 GR 94 DBCS codes (each byte A1..FE). */
                            करो {
                                अगर( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((स्थिर uint16_t *)stage3)) - 0xa1a1)<=(0xfefe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->add(sa->set, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } जबतक((++c&0xf)!=0);
                            अवरोध;
                        हाल UCNV_SET_FILTER_HZ:
                            /* Only add code points that are suitable for HZ DBCS (lead byte A1..FD). */
                            करो {
                                अगर( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((स्थिर uint16_t *)stage3))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->add(sa->set, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } जबतक((++c&0xf)!=0);
                            अवरोध;
                        शेष:
                            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
                            वापस;
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

    ucnv_extGetUnicodeSet(shared_तथ्य, sa, which, filter, pErrorCode);
}

U_CFUNC व्योम
ucnv_MBCSGetUnicodeSetForUnicode(स्थिर UConverter_Shared_तथ्य *shared_तथ्य,
                                 स्थिर USetAdder *sa,
                                 UConverterUnicodeSet which,
                                 UErrorCode *pErrorCode) {
    ucnv_MBCSGetFilteredUnicodeSetForUnicode(
        shared_तथ्य, sa, which,
        shared_तथ्य->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ?
            UCNV_SET_FILTER_DBCS_ONLY :
            UCNV_SET_FILTER_NONE,
        pErrorCode);
}

अटल व्योम U_CALLCONV
ucnv_MBCSGetUnicodeSet(स्थिर UConverter *cnv,
                   स्थिर USetAdder *sa,
                   UConverterUnicodeSet which,
                   UErrorCode *pErrorCode) {
    अगर(cnv->options&_MBCS_OPTION_GB18030) {
        sa->addRange(sa->set, 0, 0xd7ff);
        sa->addRange(sa->set, 0xe000, 0x10ffff);
    } अन्यथा {
        ucnv_MBCSGetUnicodeSetForUnicode(cnv->shared_तथ्य, sa, which, pErrorCode);
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
अटल UChar32
_extFromU(UConverter *cnv, स्थिर UConverter_Shared_तथ्य *shared_तथ्य,
          UChar32 cp,
          स्थिर char16_t **source, स्थिर char16_t *sourceLimit,
          uint8_t **target, स्थिर uint8_t *targetLimit,
          int32_t **offsets, int32_t source_सूचकांक,
          UBool सफाई,
          UErrorCode *pErrorCode) {
    स्थिर int32_t *cx;

    cnv->useSubChar1=false;

    अगर( (cx=shared_तथ्य->mbcs.extIndexes)!=nullptr &&
        ucnv_extInitialMatchFromU(
            cnv, cx,
            cp, source, sourceLimit,
            reinterpret_cast<अक्षर**>(target), reinterpret_cast<स्थिर अक्षर*>(targetLimit),
            offsets, source_सूचकांक,
            सफाई,
            pErrorCode)
    ) {
        वापस 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    अगर((cnv->options&_MBCS_OPTION_GB18030)!=0) {
        स्थिर uint32_t *range;
        int32_t i;

        range=gb18030Ranges[0];
        क्रम(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            अगर (range[0] <= static_cast<uint32_t>(cp) && static_cast<uint32_t>(cp) <= range[1]) {
                /* found the Unicode code point, output the four-byte sequence for it */
                uint32_t linear;
                अक्षर bytes[4];

                /* get the linear value of the first GB 18030 code in this range */
                linear=range[2]-LINEAR_18030_BASE;

                /* add the offset from the beginning of the range */
                linear += (static_cast<uint32_t>(cp) - range[0]);

                /* turn this into a four-byte sequence */
                bytes[3] = static_cast<अक्षर>(0x30 + linear % 10); linear /= 10;
                bytes[2] = static_cast<अक्षर>(0x81 + linear % 126); linear /= 126;
                bytes[1] = static_cast<अक्षर>(0x30 + linear % 10); linear /= 10;
                bytes[0] = static_cast<अक्षर>(0x81 + linear);

                /* output this sequence */
                ucnv_fromUWriteBytes(cnv,
                                     bytes, 4, reinterpret_cast<अक्षर**>(target), reinterpret_cast<स्थिर अक्षर*>(targetLimit),
                                     offsets, source_सूचकांक, pErrorCode);
                वापस 0;
            }
        }
    }

    /* no mapping */
    *pErrorCode=U_INVALID_CHAR_FOUND;
    वापस cp;
}

/*
 * Input sequence: cnv->toUBytes[0..length[
 * @return if(U_FAILURE) return the length (toULength, byteIndex) for the input
 *         else return 0 after output has been written to the target
 */
अटल int8_t
_extToU(UConverter *cnv, स्थिर UConverter_Shared_तथ्य *shared_तथ्य,
        int8_t length,
        स्थिर uint8_t **source, स्थिर uint8_t *sourceLimit,
        char16_t **target, स्थिर char16_t *targetLimit,
        int32_t **offsets, int32_t source_सूचकांक,
        UBool सफाई,
        UErrorCode *pErrorCode) {
    स्थिर int32_t *cx;

    अगर( (cx=shared_तथ्य->mbcs.extIndexes)!=nullptr &&
        ucnv_extInitialMatchToU(
            cnv, cx,
            length, reinterpret_cast<स्थिर अक्षर**>(source), reinterpret_cast<स्थिर अक्षर*>(sourceLimit),
            target, targetLimit,
            offsets, source_सूचकांक,
            सफाई,
            pErrorCode)
    ) {
        वापस 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    अगर(length==4 && (cnv->options&_MBCS_OPTION_GB18030)!=0) {
        स्थिर uint32_t *range;
        uint32_t linear;
        int32_t i;

        linear=LINEAR_18030(cnv->toUBytes[0], cnv->toUBytes[1], cnv->toUBytes[2], cnv->toUBytes[3]);
        range=gb18030Ranges[0];
        क्रम(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            अगर(range[2]<=linear && linear<=range[3]) {
                /* found the sequence, output the Unicode code point for it */
                *pErrorCode=U_ZERO_ERROR;

                /* add the linear difference between the input and start sequences to the start code point */
                linear=range[0]+(linear-range[2]);

                /* output this code point */
                ucnv_toUWriteCodePoint(cnv, linear, target, targetLimit, offsets, source_सूचकांक, pErrorCode);

                वापस 0;
            }
        }
    }

    /* no mapping */
    *pErrorCode=U_INVALID_CHAR_FOUND;
    वापस length;
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
#घोषणा EBCDIC_LF 0x25
#घोषणा EBCDIC_NL 0x15

/* standard EBCDIC codes with roundtrip flag as stored in Unicode-to-single-byte tables */
#घोषणा EBCDIC_RT_LF 0xf25
#घोषणा EBCDIC_RT_NL 0xf15

/* Unicode code points */
#घोषणा U_LF 0x0a
#घोषणा U_NL 0x85

अटल UBool
_EBCDICSwapLFNL(UConverter_Shared_तथ्य *shared_तथ्य, UErrorCode *pErrorCode) {
    UConverterMBCSTable *mbcsTable;

    स्थिर uint16_t *table, *results;
    स्थिर uint8_t *bytes;

    int32_t (*newStateTable)[256];
    uint16_t *newResults;
    uint8_t *p;
    अक्षर *name;

    uint32_t stage2Entry;
    uint32_t size, sizeofFromUBytes;

    mbcsTable=&shared_तथ्य->mbcs;

    table=mbcsTable->fromUnicodeTable;
    bytes=mbcsTable->fromUnicodeBytes;
    results = reinterpret_cast<स्थिर uint16_t*>(bytes);

    /*
     * Check that this is an EBCDIC table with SBCS portion -
     * SBCS or EBCDIC_STATEFUL with standard EBCDIC LF and NL mappings.
     *
     * If not, ignore the option. Options are always ignored if they do not apply.
     */
    अगर(!(
         (mbcsTable->outputType==MBCS_OUTPUT_1 || mbcsTable->outputType==MBCS_OUTPUT_2_SISO) &&
         mbcsTable->stateTable[0][EBCDIC_LF]==MBCS_ENTRY_FINAL(0, MBCS_STATE_VALID_DIRECT_16, U_LF) &&
         mbcsTable->stateTable[0][EBCDIC_NL]==MBCS_ENTRY_FINAL(0, MBCS_STATE_VALID_DIRECT_16, U_NL)
    )) {
        वापस false;
    }

    अगर(mbcsTable->outputType==MBCS_OUTPUT_1) {
        अगर(!(
             EBCDIC_RT_LF==MBCS_SINGLE_परिणाम_FROM_U(table, results, U_LF) &&
             EBCDIC_RT_NL==MBCS_SINGLE_परिणाम_FROM_U(table, results, U_NL)
        )) {
            वापस false;
        }
    } अन्यथा /* MBCS_OUTPUT_2_SISO */ {
        stage2Entry=MBCS_STAGE_2_FROM_U(table, U_LF);
        अगर(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2Entry, U_LF)!=0 &&
             EBCDIC_LF==MBCS_मूल्य_2_FROM_STAGE_2(bytes, stage2Entry, U_LF)
        )) {
            वापस false;
        }

        stage2Entry=MBCS_STAGE_2_FROM_U(table, U_NL);
        अगर(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2Entry, U_NL)!=0 &&
             EBCDIC_NL==MBCS_मूल्य_2_FROM_STAGE_2(bytes, stage2Entry, U_NL)
        )) {
            वापस false;
        }
    }

    अगर(mbcsTable->from_UBytes_लंबाई>0) {
        /*
         * We _know_ the number of bytes in the fromUnicodeBytes array
         * starting with header.version 4.1.
         */
        sizeofFromUBytes=mbcsTable->from_UBytes_लंबाई;
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
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस false;
    }

    /*
     * The table has an appropriate format.
     * Allocate and build
     * - a modified to-Unicode state table
     * - a modified from-Unicode output array
     * - a converter name string with the swap option appended
     */
    size=
        mbcsTable->गिनती_States*1024+
        sizeofFromUBytes+
        UCNV_MAX_CONVERTER_NAME_लंबाई+20;
    p = static_cast<uint8_t*>(uprv_malloc(size));
    अगर(p==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस false;
    }

    /* copy and modify the to-Unicode state table */
    newStateTable = reinterpret_cast<int32_t(*)[256]>(p);
    uprv_memcpy(newStateTable, mbcsTable->stateTable, mbcsTable->गिनती_States*1024);

    newStateTable[0][EBCDIC_LF]=MBCS_ENTRY_FINAL(0, MBCS_STATE_VALID_DIRECT_16, U_NL);
    newStateTable[0][EBCDIC_NL]=MBCS_ENTRY_FINAL(0, MBCS_STATE_VALID_DIRECT_16, U_LF);

    /* copy and modify the from-Unicode result table */
    newResults = reinterpret_cast<uint16_t*>(newStateTable[mbcsTable->गिनती_States]);
    uprv_memcpy(newResults, bytes, sizeofFromUBytes);

    /* conveniently, the table access macros work on the left side of expressions */
    अगर(mbcsTable->outputType==MBCS_OUTPUT_1) {
        MBCS_SINGLE_परिणाम_FROM_U(table, newResults, U_LF)=EBCDIC_RT_NL;
        MBCS_SINGLE_परिणाम_FROM_U(table, newResults, U_NL)=EBCDIC_RT_LF;
    } अन्यथा /* MBCS_OUTPUT_2_SISO */ {
        stage2Entry=MBCS_STAGE_2_FROM_U(table, U_LF);
        MBCS_मूल्य_2_FROM_STAGE_2(newResults, stage2Entry, U_LF)=EBCDIC_NL;

        stage2Entry=MBCS_STAGE_2_FROM_U(table, U_NL);
        MBCS_मूल्य_2_FROM_STAGE_2(newResults, stage2Entry, U_NL)=EBCDIC_LF;
    }

    /* set the canonical converter name */
    name = reinterpret_cast<अक्षर*>(newResults) + sizeofFromUBytes;
    uprv_strcpy(name, shared_तथ्य->static_तथ्य->name);
    uprv_strcat(name, UCNV_SWAP_LFNL_OPTION_STRING);

    /* set the pointers */
    icu::umtx_lock(nullptr);
    अगर(mbcsTable->swapLFNLStateTable==nullptr) {
        mbcsTable->swapLFNLStateTable=newStateTable;
        mbcsTable->swapLFNLFromUnicodeBytes = reinterpret_cast<uint8_t*>(newResults);
        mbcsTable->swapLFNLName=name;

        newStateTable=nullptr;
    }
    icu::umtx_unlock(nullptr);

    /* release the allocated memory if another thread beat us to it */
    अगर(newStateTable!=nullptr) {
        uprv_free(newStateTable);
    }
    वापस true;
}

/* reconstitute omitted fromUnicode data ------------------------------------ */

/* for details, compare with genmbcs.c MBCSAddFromUnicode() and transformEUC() */
अटल UBool U_CALLCONV
writeStage3Roundtrip(स्थिर व्योम *context, uint32_t value, UChar32 codePoints[32]) {
    UConverterMBCSTable *mbcsTable=(UConverterMBCSTable *)context;
    स्थिर uint16_t *table;
    uint32_t *stage2;
    uint8_t *bytes, *p;
    UChar32 c;
    int32_t i, st3;

    table=mbcsTable->fromUnicodeTable;
    bytes = const_cast<uint8_t*>(mbcsTable->fromUnicodeBytes);

    /* for EUC outputTypes, modify the value like genmbcs.c's transformEUC() */
    चयन(mbcsTable->outputType) {
    हाल MBCS_OUTPUT_3_EUC:
        अगर(value<=0xffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } अन्यथा अगर(value<=0x8effff) {
            /* code set 2 */
            value&=0x7fff;
        } अन्यथा /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7f;
        }
        अवरोध;
    हाल MBCS_OUTPUT_4_EUC:
        अगर(value<=0xffffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } अन्यथा अगर(value<=0x8effffff) {
            /* code set 2 */
            value&=0x7fffff;
        } अन्यथा /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7fff;
        }
        अवरोध;
    शेष:
        अवरोध;
    }

    क्रम(i=0; i<=0x1f; ++value, ++i) {
        c=codePoints[i];
        अगर(c<0) {
            जारी;
        }

        /* locate the stage 2 & 3 data */
        stage2=((uint32_t *)table)+table[c>>10]+((c>>4)&0x3f);
        p=bytes;
        st3 = static_cast<int32_t>(static_cast<uint16_t>(*stage2)) * 16 + (c & 0xf);

        /* write the codepage bytes into stage 3 */
        चयन(mbcsTable->outputType) {
        हाल MBCS_OUTPUT_3:
        हाल MBCS_OUTPUT_4_EUC:
            p+=st3*3;
            p[0] = static_cast<uint8_t>(value >> 16);
            p[1] = static_cast<uint8_t>(value >> 8);
            p[2] = static_cast<uint8_t>(value);
            अवरोध;
        हाल MBCS_OUTPUT_4:
            reinterpret_cast<uint32_t*>(p)[st3] = value;
            अवरोध;
        शेष:
            /* 2 bytes per character */
            reinterpret_cast<uint16_t*>(p)[st3] = static_cast<uint16_t>(value);
            अवरोध;
        }

        /* set the roundtrip flag */
        *stage2|=(1UL<<(16+(c&0xf)));
    }
    वापस true;
 }

अटल व्योम
reconstitute_तथ्य(UConverterMBCSTable *mbcsTable,
                 uint32_t stage1_लंबाई, uint32_t stage2_लंबाई,
                 uint32_t full_Stage2_लंबाई,  /* lengths are numbers of units, not bytes */
                 UErrorCode *pErrorCode) {
    uint16_t *stage1;
    uint32_t *stage2;
    uint32_t तथ्य_लंबाई=stage1_लंबाई*2+full_Stage2_लंबाई*4+mbcsTable->from_UBytes_लंबाई;
    mbcsTable->reconstituted_तथ्य = static_cast<uint8_t*>(uprv_malloc(तथ्य_लंबाई));
    अगर(mbcsTable->reconstituted_तथ्य==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    uprv_memset(mbcsTable->reconstituted_तथ्य, 0, तथ्य_लंबाई);

    /* copy existing data and reroute the pointers */
    stage1 = reinterpret_cast<uint16_t*>(mbcsTable->reconstituted_तथ्य);
    uprv_memcpy(stage1, mbcsTable->fromUnicodeTable, stage1_लंबाई*2);

    stage2 = reinterpret_cast<uint32_t*>(stage1 + stage1_लंबाई);
    uprv_memcpy(stage2+(full_Stage2_लंबाई-stage2_लंबाई),
                mbcsTable->fromUnicodeTable+stage1_लंबाई,
                stage2_लंबाई*4);

    mbcsTable->fromUnicodeTable=stage1;
    mbcsTable->fromUnicodeBytes = reinterpret_cast<uint8_t*>(stage2 + full_Stage2_लंबाई);

    /* indexes into stage 2 count from the bottom of the fromUnicodeTable */
    stage2 = reinterpret_cast<uint32_t*>(stage1);

    /* reconstitute the initial part of stage 2 from the mbcsIndex */
    {
        int32_t stage_UTF8_लंबाई = (static_cast<int32_t>(mbcsTable->maxFastUChar) + 1) >> 6;
        int32_t stage_UTF8_सूचकांक=0;
        int32_t st1, st2, st3, i;

        क्रम(st1=0; stage_UTF8_सूचकांक<stage_UTF8_लंबाई; ++st1) {
            st2=stage1[st1];
            अगर (st2 != static_cast<int32_t>(stage1_लंबाई) / 2) {
                /* each stage 2 block has 64 entries corresponding to 16 entries in the mbcsIndex */
                क्रम(i=0; i<16; ++i) {
                    st3=mbcsTable->mbcs_सूचकांक[stage_UTF8_सूचकांक++];
                    अगर(st3!=0) {
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
    ucnv_MBCSEnumToUnicode(mbcsTable, writeStage3Roundtrip, mbcsTable, pErrorCode);
}

/* MBCS setup functions ----------------------------------------------------- */

अटल व्योम U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_तथ्य *shared_तथ्य,
          UConverterLoadArgs *pArgs,
          स्थिर uint8_t *raw,
          UErrorCode *pErrorCode) {
    UDataInfo info;
    UConverterMBCSTable *mbcsTable=&shared_तथ्य->mbcs;
    _MBCSHeader *header=(_MBCSHeader *)raw;
    uint32_t offset;
    uint32_t header_लंबाई;
    UBool noFromU=false;

    अगर(header->version[0]==4) {
        header_लंबाई=MBCS_HEADER_V4_लंबाई;
    } अन्यथा अगर(header->version[0]==5 && header->version[1]>=3 &&
              (header->options&MBCS_OPT_UNKNOWN_INCOMPATIBLE_MASK)==0) {
        header_लंबाई=header->options&MBCS_OPT_लंबाई_MASK;
        noFromU = static_cast<UBool>((header->options & MBCS_OPT_NO_FROM_U) != 0);
    } अन्यथा {
        *pErrorCode=U_INVALID_TABLE_FORMAT;
        वापस;
    }

    mbcsTable->outputType = static_cast<uint8_t>(header->flags);
    अगर(noFromU && mbcsTable->outputType==MBCS_OUTPUT_1) {
        *pErrorCode=U_INVALID_TABLE_FORMAT;
        वापस;
    }

    /* extension data, header version 4.2 and higher */
    offset=header->flags>>8;
    अगर(offset!=0) {
        mbcsTable->extIndexes = reinterpret_cast<स्थिर int32_t*>(raw + offset);
    }

    अगर(mbcsTable->outputType==MBCS_OUTPUT_EXT_ONLY) {
        UConverterLoadArgs args=UCNV_LOAD_ARGS_INITIALIZER;
        UConverter_Shared_तथ्य *base_Shared_तथ्य;
        स्थिर int32_t *extIndexes;
        स्थिर अक्षर *baseName;

        /* extension-only file, load the base table and set values appropriately */
        अगर((extIndexes=mbcsTable->extIndexes)==nullptr) {
            /* extension-only file without extension */
            *pErrorCode=U_INVALID_TABLE_FORMAT;
            वापस;
        }

        अगर(pArgs->nestedLoads!=1) {
            /* an extension table must not be loaded as a base table */
            *pErrorCode=U_INVALID_TABLE_FILE;
            वापस;
        }

        /* load the base table */
        baseName = reinterpret_cast<स्थिर अक्षर*>(header) + header_लंबाई * 4;
        अगर(0==uprv_strcmp(baseName, shared_तथ्य->static_तथ्य->name)) {
            /* forbid loading this same extension-only file */
            *pErrorCode=U_INVALID_TABLE_FORMAT;
            वापस;
        }

        /* TODO parse package name out of the prefix of the base name in the extension .cnv file? */
        args.size=माप(UConverterLoadArgs);
        args.nestedLoads=2;
        args.onlyTestIsLoadable=pArgs->onlyTestIsLoadable;
        args.reserved=pArgs->reserved;
        args.options=pArgs->options;
        args.pkg=pArgs->pkg;
        args.name=baseName;
        base_Shared_तथ्य=ucnv_load(&args, pErrorCode);
        अगर(U_FAILURE(*pErrorCode)) {
            वापस;
        }
        अगर( base_Shared_तथ्य->static_तथ्य->conversionType!=UCNV_MBCS ||
            base_Shared_तथ्य->mbcs.base_Shared_तथ्य!=nullptr
        ) {
            ucnv_unload(base_Shared_तथ्य);
            *pErrorCode=U_INVALID_TABLE_FORMAT;
            वापस;
        }
        अगर(pArgs->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            ucnv_unload(base_Shared_तथ्य);
            वापस;
        }

        /* copy the base table data */
        uprv_memcpy(mbcsTable, &base_Shared_तथ्य->mbcs, माप(UConverterMBCSTable));

        /* overwrite values with relevant ones for the extension converter */
        mbcsTable->base_Shared_तथ्य=base_Shared_तथ्य;
        mbcsTable->extIndexes=extIndexes;

        /*
         * It would be possible to share the swapLFNL data with a base converter,
         * but the generated name would have to be different, and the memory
         * would have to be free'd only once.
         * It is easier to just create the data for the extension converter
         * separately when it is requested.
         */
        mbcsTable->swapLFNLStateTable=nullptr;
        mbcsTable->swapLFNLFromUnicodeBytes=nullptr;
        mbcsTable->swapLFNLName=nullptr;

        /*
         * The reconstitutedData must be deleted only when the base converter
         * is unloaded.
         */
        mbcsTable->reconstituted_तथ्य=nullptr;

        /*
         * Set a special, runtime-only outputType if the extension converter
         * is a DBCS version of a base converter that also maps single bytes.
         */
        अगर( shared_तथ्य->static_तथ्य->conversionType==UCNV_DBCS ||
                (shared_तथ्य->static_तथ्य->conversionType==UCNV_MBCS &&
                 shared_तथ्य->static_तथ्य->minBytesPerChar>=2)
        ) {
            अगर(base_Shared_तथ्य->mbcs.outputType==MBCS_OUTPUT_2_SISO) {
                /* the base converter is SI/SO-stateful */
                int32_t entry;

                /* get the dbcs state from the state table entry for SO=0x0e */
                entry=mbcsTable->stateTable[0][0xe];
                अगर( MBCS_ENTRY_IS_FINAL(entry) &&
                    MBCS_ENTRY_FINAL_ACTION(entry)==MBCS_STATE_CHANGE_ONLY &&
                    MBCS_ENTRY_FINAL_STATE(entry)!=0
                ) {
                    mbcsTable->dbcsOnlyState = static_cast<uint8_t>(MBCS_ENTRY_FINAL_STATE(entry));

                    mbcsTable->outputType=MBCS_OUTPUT_DBCS_ONLY;
                }
            } अन्यथा अगर(
                base_Shared_तथ्य->static_तथ्य->conversionType==UCNV_MBCS &&
                base_Shared_तथ्य->static_तथ्य->minBytesPerChar==1 &&
                base_Shared_तथ्य->static_तथ्य->maxBytesPerChar==2 &&
                mbcsTable->गिनती_States<=127
            ) {
                /* non-stateful base converter, need to modify the state table */
                int32_t (*newStateTable)[256];
                int32_t *state;
                int32_t i, count;

                /* allocate a new state table and copy the base state table contents */
                count=mbcsTable->गिनती_States;
                newStateTable = static_cast<int32_t(*)[256]>(uprv_malloc((count + 1) * 1024));
                अगर(newStateTable==nullptr) {
                    ucnv_unload(base_Shared_तथ्य);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    वापस;
                }

                uprv_memcpy(newStateTable, mbcsTable->stateTable, count*1024);

                /* change all final single-byte entries to go to a new all-illegal state */
                state=newStateTable[0];
                क्रम(i=0; i<256; ++i) {
                    अगर(MBCS_ENTRY_IS_FINAL(state[i])) {
                        state[i]=MBCS_ENTRY_TRANSITION(count, 0);
                    }
                }

                /* build the new all-illegal state */
                state=newStateTable[count];
                क्रम(i=0; i<256; ++i) {
                    state[i]=MBCS_ENTRY_FINAL(0, MBCS_STATE_ILLEGAL, 0);
                }
                mbcsTable->stateTable=(स्थिर int32_t (*)[256])newStateTable;
                mbcsTable->गिनती_States = static_cast<uint8_t>(count + 1);
                mbcsTable->stateTableOwned=true;

                mbcsTable->outputType=MBCS_OUTPUT_DBCS_ONLY;
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
        चयन(mbcsTable->outputType) {
        हाल MBCS_OUTPUT_1:
        हाल MBCS_OUTPUT_2:
        हाल MBCS_OUTPUT_3:
        हाल MBCS_OUTPUT_4:
        हाल MBCS_OUTPUT_3_EUC:
        हाल MBCS_OUTPUT_4_EUC:
        हाल MBCS_OUTPUT_2_SISO:
            /* OK */
            अवरोध;
        शेष:
            *pErrorCode=U_INVALID_TABLE_FORMAT;
            वापस;
        }
        अगर(pArgs->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            वापस;
        }

        mbcsTable->गिनती_States = static_cast<uint8_t>(header->गिनती_States);
        mbcsTable->गिनती_To_UFallbacks=header->गिनती_To_UFallbacks;
        mbcsTable->stateTable = reinterpret_cast<स्थिर int32_t(*)[256]>(raw + header_लंबाई * 4);
        mbcsTable->toUFallbacks = reinterpret_cast<स्थिर _MBCSToUFallback*>(mbcsTable->stateTable + header->गिनती_States);
        mbcsTable->unicodeCodeUnits = reinterpret_cast<स्थिर uint16_t*>(raw + header->offsetToUCodeUnits);

        mbcsTable->fromUnicodeTable = reinterpret_cast<स्थिर uint16_t*>(raw + header->offsetFromUTable);
        mbcsTable->fromUnicodeBytes = raw + header->offsetFromUBytes;
        mbcsTable->from_UBytes_लंबाई=header->from_UBytes_लंबाई;

        /*
         * converter versions 6.1 and up contain a unicodeMask that is
         * used here to select the most efficient function implementations
         */
        info.size=माप(UDataInfo);
        udata_getInfo((UDataMemory *)shared_तथ्य->तथ्य_Memory, &info);
        अगर(info.formatVersion[0]>6 || (info.formatVersion[0]==6 && info.formatVersion[1]>=1)) {
            /* mask off possible future extensions to be safe */
            mbcsTable->unicodeMask = static_cast<uint8_t>(shared_तथ्य->static_तथ्य->unicodeMask & 3);
        } अन्यथा {
            /* for older versions, assume worst case: contains anything possible (prevent over-optimizations) */
            mbcsTable->unicodeMask=UCNV_HAS_SUPPLEMENTARY|UCNV_HAS_SURROGATES;
        }

        /*
         * _MBCSHeader.version 4.3 adds utf8Friendly data structures.
         * Check for the header version, SBCS vs. MBCS, and for whether the
         * data structures are optimized for code points as high as what the
         * runtime code is designed for.
         * The implementation does not handle mapping tables with entries for
         * unpaired surrogates.
         */
        अगर( header->version[1]>=3 &&
            (mbcsTable->unicodeMask&UCNV_HAS_SURROGATES)==0 &&
            (mbcsTable->गिनती_States==1 ?
                (header->version[2]>=(SBCS_FAST_MAX>>8)) :
                (header->version[2]>=(MBCS_FAST_MAX>>8))
            )
        ) {
            mbcsTable->utf8Friendly=true;

            अगर(mbcsTable->गिनती_States==1) {
                /*
                 * SBCS: Stage 3 is allocated in 64-entry blocks for U+0000..SBCS_FAST_MAX or higher.
                 * Build a table with indexes to each block, to be used instead of
                 * the regular stage 1/2 table.
                 */
                int32_t i;
                क्रम(i=0; i<(SBCS_FAST_LIMIT>>6); ++i) {
                    mbcsTable->sbcs_सूचकांक[i]=mbcsTable->fromUnicodeTable[mbcsTable->fromUnicodeTable[i>>4]+((i<<2)&0x3c)];
                }
                /* set SBCS_FAST_MAX to reflect the reach of sbcsIndex[] even if header->version[2]>(SBCS_FAST_MAX>>8) */
                mbcsTable->maxFastUChar=SBCS_FAST_MAX;
            } अन्यथा {
                /*
                 * MBCS: Stage 3 is allocated in 64-entry blocks for U+0000..MBCS_FAST_MAX or higher.
                 * The .cnv file is prebuilt with an additional stage table with indexes
                 * to each block.
                 */
                mbcsTable->mbcs_सूचकांक = reinterpret_cast<स्थिर uint16_t*>(
                    mbcsTable->fromUnicodeBytes +
                     (noFromU ? 0 : mbcsTable->from_UBytes_लंबाई));
                mbcsTable->maxFastUChar = (static_cast<char16_t>(header->version[2]) << 8) | 0xff;
            }
        }

        /* calculate a bit set of 4 ASCII characters per bit that round-trip to ASCII bytes */
        {
            uint32_t asciiRoundtrips=0xffffffff;
            int32_t i;

            क्रम(i=0; i<0x80; ++i) {
                अगर(mbcsTable->stateTable[0][i]!=MBCS_ENTRY_FINAL(0, MBCS_STATE_VALID_DIRECT_16, i)) {
                    asciiRoundtrips &= ~(static_cast<uint32_t>(1) << (i >> 2));
                }
            }
            mbcsTable->asciiRoundtrips=asciiRoundtrips;
        }

        अगर(noFromU) {
            uint32_t stage1_लंबाई=
                mbcsTable->unicodeMask&UCNV_HAS_SUPPLEMENTARY ?
                    0x440 : 0x40;
            uint32_t stage2_लंबाई=
                (header->offsetFromUBytes-header->offsetFromUTable)/4-
                stage1_लंबाई/2;
            reconstitute_तथ्य(mbcsTable, stage1_लंबाई, stage2_लंबाई, header->full_Stage2_लंबाई, pErrorCode);
        }
    }

    /* Set the impl pointer here so that it is set for both extension-only and base tables. */
    अगर(mbcsTable->utf8Friendly) {
        अगर(mbcsTable->गिनती_States==1) {
            shared_तथ्य->impl=&_SBCSUTF8Impl;
        } अन्यथा {
            अगर(mbcsTable->outputType==MBCS_OUTPUT_2) {
                shared_तथ्य->impl=&_DBCSUTF8Impl;
            }
        }
    }

    अगर(mbcsTable->outputType==MBCS_OUTPUT_DBCS_ONLY || mbcsTable->outputType==MBCS_OUTPUT_2_SISO) {
        /*
         * MBCS_OUTPUT_DBCS_ONLY: No SBCS mappings, therefore ASCII does not roundtrip.
         * MBCS_OUTPUT_2_SISO: Bypass the ASCII fastpath to handle prevLength correctly.
         */
        mbcsTable->asciiRoundtrips=0;
    }
}

अटल व्योम U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_तथ्य *shared_तथ्य) {
    UConverterMBCSTable *mbcsTable=&shared_तथ्य->mbcs;

    अगर(mbcsTable->swapLFNLStateTable!=nullptr) {
        uprv_free(mbcsTable->swapLFNLStateTable);
    }
    अगर(mbcsTable->stateTableOwned) {
        uprv_free((व्योम *)mbcsTable->stateTable);
    }
    अगर(mbcsTable->base_Shared_तथ्य!=nullptr) {
        ucnv_unload(mbcsTable->base_Shared_तथ्य);
    }
    अगर(mbcsTable->reconstituted_तथ्य!=nullptr) {
        uprv_free(mbcsTable->reconstituted_तथ्य);
    }
}

अटल व्योम U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverterLoadArgs *pArgs,
              UErrorCode *pErrorCode) {
    UConverterMBCSTable *mbcsTable;
    स्थिर int32_t *extIndexes;
    uint8_t outputType;
    int8_t maxBytesPerUChar;

    अगर(pArgs->onlyTestIsLoadable) {
        वापस;
    }

    mbcsTable=&cnv->shared_तथ्य->mbcs;
    outputType=mbcsTable->outputType;

    अगर(outputType==MBCS_OUTPUT_DBCS_ONLY) {
        /* the swaplfnl option does not apply, remove it */
        cnv->options=pArgs->options&=~UCNV_OPTION_SWAP_LFNL;
    }

    अगर((pArgs->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        /* do this because double-checked locking is broken */
        UBool isCached;

        icu::umtx_lock(nullptr);
        isCached=mbcsTable->swapLFNLStateTable!=nullptr;
        icu::umtx_unlock(nullptr);

        अगर(!isCached) {
            अगर(!_EBCDICSwapLFNL(cnv->shared_तथ्य, pErrorCode)) {
                अगर(U_FAILURE(*pErrorCode)) {
                    वापस; /* something went wrong */
                }

                /* the option does not apply, remove it */
                cnv->options=pArgs->options&=~UCNV_OPTION_SWAP_LFNL;
            }
        }
    }

    अगर(uprv_strstr(pArgs->name, "18030")!=nullptr) {
        अगर(uprv_strstr(pArgs->name, "gb18030")!=nullptr || uprv_strstr(pArgs->name, "GB18030")!=nullptr) {
            /* set a flag for GB 18030 mode, which changes the callback behavior */
            cnv->options|=_MBCS_OPTION_GB18030;
        }
    } अन्यथा अगर((uprv_strstr(pArgs->name, "KEIS")!=nullptr) || (uprv_strstr(pArgs->name, "keis")!=nullptr)) {
        /* set a flag for KEIS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_KEIS;
    } अन्यथा अगर((uprv_strstr(pArgs->name, "JEF")!=nullptr) || (uprv_strstr(pArgs->name, "jef")!=nullptr)) {
        /* set a flag for JEF converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JEF;
    } अन्यथा अगर((uprv_strstr(pArgs->name, "JIPS")!=nullptr) || (uprv_strstr(pArgs->name, "jips")!=nullptr)) {
        /* set a flag for JIPS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JIPS;
    }

    /* fix maxBytesPerUChar depending on outputType and options etc. */
    अगर(outputType==MBCS_OUTPUT_2_SISO) {
        cnv->maxBytesPerUChar=3; /* SO+DBCS */
    }

    extIndexes=mbcsTable->extIndexes;
    अगर(extIndexes!=nullptr) {
        maxBytesPerUChar = static_cast<int8_t>(UCNV_GET_MAX_BYTES_PER_UCHAR(extIndexes));
        अगर(outputType==MBCS_OUTPUT_2_SISO) {
            ++maxBytesPerUChar; /* SO + multiple DBCS */
        }

        अगर(maxBytesPerUChar>cnv->maxBytesPerUChar) {
            cnv->maxBytesPerUChar=maxBytesPerUChar;
        }
    }

#अगर 0
    /*
     * documentation of UConverter fields used for status
     * all of these fields are (re)set to 0 by ucnv_bld.c and ucnv_reset()
     */

    /* toUnicode */
    cnv->toUnicodeStatus=0;     /* offset */
    cnv->mode=0;                /* state */
    cnv->toULength=0;           /* byteIndex */

    /* fromUnicode */
    cnv->fromUChar32=0;
    cnv->fromUnicodeStatus=1;   /* prevLength */
#पूर्ण_अगर
}

U_CDECL_BEGIN

अटल स्थिर अक्षर* U_CALLCONV
ucnv_MBCSGetName(स्थिर UConverter *cnv) {
    अगर((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0 && cnv->shared_तथ्य->mbcs.swapLFNLName!=nullptr) {
        वापस cnv->shared_तथ्य->mbcs.swapLFNLName;
    } अन्यथा {
        वापस cnv->shared_तथ्य->static_तथ्य->name;
    }
}
U_CDECL_END


/* MBCS-to-Unicode conversion functions ------------------------------------- */

अटल UChar32 U_CALLCONV
ucnv_MBCSGetFallback(UConverterMBCSTable *mbcsTable, uint32_t offset) {
    स्थिर _MBCSToUFallback *toUFallbacks;
    uint32_t i, start, limit;

    limit=mbcsTable->गिनती_To_UFallbacks;
    अगर(limit>0) {
        /* do a binary search for the fallback mapping */
        toUFallbacks=mbcsTable->toUFallbacks;
        start=0;
        जबतक(start<limit-1) {
            i=(start+limit)/2;
            अगर(offset<toUFallbacks[i].offset) {
                limit=i;
            } अन्यथा {
                start=i;
            }
        }

        /* did we really find it? */
        अगर(offset==toUFallbacks[start].offset) {
            वापस toUFallbacks[start].codePoint;
        }
    }

    वापस 0xfffe;
}

/* This version of ucnv_MBCSToUnicodeWithOffsets() is optimized for single-byte, single-state codepages. */
अटल व्योम
ucnv_MBCSSingleToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                                UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर uint8_t *source, *sourceLimit;
    char16_t *target;
    स्थिर char16_t *targetLimit;
    int32_t *offsets;

    स्थिर int32_t (*stateTable)[256];

    int32_t source_सूचकांक;

    int32_t entry;
    char16_t c;
    uint8_t action;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source = reinterpret_cast<स्थिर uint8_t*>(pArgs->source);
    sourceLimit = reinterpret_cast<स्थिर uint8_t*>(pArgs->sourceLimit);
    target=pArgs->target;
    targetLimit=pArgs->targetLimit;
    offsets=pArgs->offsets;

    अगर((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        stateTable=(स्थिर int32_t (*)[256])cnv->shared_तथ्य->mbcs.swapLFNLStateTable;
    } अन्यथा {
        stateTable=cnv->shared_तथ्य->mbcs.stateTable;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक=0;

    /* conversion loop */
    जबतक(source<sourceLimit) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        अगर(target>=targetLimit) {
            /* target is full */
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }

        entry=stateTable[0][*source++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        अगर(MBCS_ENTRY_FINAL_IS_VALID_DIRECT_16(entry)) {
            /* output BMP code point */
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            अगर(offsets!=nullptr) {
                *offsets++=source_सूचकांक;
            }

            /* normal end of action codes: prepare for a new character */
            ++source_सूचकांक;
            जारी;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_ENTRY_FINAL_ACTION(entry));
        अगर(action==MBCS_STATE_VALID_DIRECT_20 ||
           (action==MBCS_STATE_FALLBACK_DIRECT_20 && UCNV_TO_U_USE_FALLBACK(cnv))
        ) {
            entry=MBCS_ENTRY_FINAL_मूल्य(entry);
            /* output surrogate pair */
            *target++ = static_cast<char16_t>(0xd800 | static_cast<char16_t>(entry >> 10));
            अगर(offsets!=nullptr) {
                *offsets++=source_सूचकांक;
            }
            c = static_cast<char16_t>(0xdc00 | static_cast<char16_t>(entry & 0x3ff));
            अगर(target<targetLimit) {
                *target++=c;
                अगर(offsets!=nullptr) {
                    *offsets++=source_सूचकांक;
                }
            } अन्यथा {
                /* target overflow */
                cnv->UChar_Error_बफर[0]=c;
                cnv->UChar_Error_बफर_लंबाई=1;
                *pErrorCode=U_बफर_OVERFLOW_ERROR;
                अवरोध;
            }

            ++source_सूचकांक;
            जारी;
        } अन्यथा अगर(action==MBCS_STATE_FALLBACK_DIRECT_16) {
            अगर(UCNV_TO_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
                अगर(offsets!=nullptr) {
                    *offsets++=source_सूचकांक;
                }

                ++source_सूचकांक;
                जारी;
            }
        } अन्यथा अगर(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } अन्यथा अगर(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        } अन्यथा {
            /* reserved, must never occur */
            ++source_सूचकांक;
            जारी;
        }

        अगर(U_FAILURE(*pErrorCode)) {
            /* callback(illegal) */
            अवरोध;
        } अन्यथा /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            pArgs->source = reinterpret_cast<स्थिर अक्षर*>(source);
            cnv->toUBytes[0]=*(source-1);
            cnv->toULength=_extToU(cnv, cnv->shared_तथ्य,
                                    1, &source, sourceLimit,
                                    &target, targetLimit,
                                    &offsets, source_सूचकांक,
                                    pArgs->सफाई,
                                    pErrorCode);
            source_सूचकांक += 1 + static_cast<int32_t>(source - reinterpret_cast<स्थिर uint8_t*>(pArgs->source));

            अगर(U_FAILURE(*pErrorCode)) {
                /* not mappable or buffer overflow */
                अवरोध;
            }
        }
    }

    /* write back the updated pointers */
    pArgs->source = reinterpret_cast<स्थिर अक्षर*>(source);
    pArgs->target=target;
    pArgs->offsets=offsets;
}

/*
 * This version of ucnv_MBCSSingleToUnicodeWithOffsets() is optimized for single-byte, single-state codepages
 * that only map to and from the BMP.
 * In addition to single-byte optimizations, the offset calculations
 * become much easier.
 */
अटल व्योम
ucnv_MBCSSingleToBMPWithOffsets(UConverterToUnicodeArgs *pArgs,
                            UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर uint8_t *source, *sourceLimit, *lastSource;
    char16_t *target;
    int32_t targetCapacity, length;
    int32_t *offsets;

    स्थिर int32_t (*stateTable)[256];

    int32_t source_सूचकांक;

    int32_t entry;
    uint8_t action;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source = reinterpret_cast<स्थिर uint8_t*>(pArgs->source);
    sourceLimit = reinterpret_cast<स्थिर uint8_t*>(pArgs->sourceLimit);
    target=pArgs->target;
    targetCapacity = static_cast<int32_t>(pArgs->targetLimit - pArgs->target);
    offsets=pArgs->offsets;

    अगर((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        stateTable=(स्थिर int32_t (*)[256])cnv->shared_तथ्य->mbcs.swapLFNLStateTable;
    } अन्यथा {
        stateTable=cnv->shared_तथ्य->mbcs.stateTable;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक=0;
    lastSource=source;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(sourceLimit - source);
    अगर(length<targetCapacity) {
        targetCapacity=length;
    }

#अगर MBCS_UNROLL_SINGLE_TO_BMP
    /* unrolling makes it faster on Pentium III/Windows 2000 */
    /* unroll the loop with the most common case */
unrolled:
    अगर(targetCapacity>=16) {
        int32_t count, loops, oredEntries;

        loops=count=targetCapacity>>4;
        करो {
            oredEntries=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            oredEntries|=entry=stateTable[0][*source++];
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));

            /* were all 16 entries really valid? */
            अगर(!MBCS_ENTRY_FINAL_IS_VALID_DIRECT_16(oredEntries)) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                अवरोध;
            }
        } जबतक(--count>0);
        count=loops-count;
        targetCapacity-=16*count;

        अगर(offsets!=nullptr) {
            lastSource+=16*count;
            जबतक(count>0) {
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
#पूर्ण_अगर

    /* conversion loop */
    जबतक(targetCapacity > 0 && source < sourceLimit) {
        entry=stateTable[0][*source++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        अगर(MBCS_ENTRY_FINAL_IS_VALID_DIRECT_16(entry)) {
            /* output BMP code point */
            *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            --targetCapacity;
            जारी;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_ENTRY_FINAL_ACTION(entry));
        अगर(action==MBCS_STATE_FALLBACK_DIRECT_16) {
            अगर(UCNV_TO_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *target++ = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
                --targetCapacity;
                जारी;
            }
        } अन्यथा अगर(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } अन्यथा अगर(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        } अन्यथा {
            /* reserved, must never occur */
            जारी;
        }

        /* set offsets since the start or the last extension */
        अगर(offsets!=nullptr) {
            int32_t count = static_cast<int32_t>(source - lastSource);

            /* predecrement: do not set the offset for the callback-causing character */
            जबतक(--count>0) {
                *offsets++=source_सूचकांक++;
            }
            /* offset and sourceIndex are now set for the current character */
        }

        अगर(U_FAILURE(*pErrorCode)) {
            /* callback(illegal) */
            अवरोध;
        } अन्यथा /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            lastSource=source;
            cnv->toUBytes[0]=*(source-1);
            cnv->toULength=_extToU(cnv, cnv->shared_तथ्य,
                                    1, &source, sourceLimit,
                                    &target, pArgs->targetLimit,
                                    &offsets, source_सूचकांक,
                                    pArgs->सफाई,
                                    pErrorCode);
            source_सूचकांक += 1 + static_cast<int32_t>(source - lastSource);

            अगर(U_FAILURE(*pErrorCode)) {
                /* not mappable or buffer overflow */
                अवरोध;
            }

            /* recalculate the targetCapacity after an extension mapping */
            targetCapacity = static_cast<int32_t>(pArgs->targetLimit - target);
            length = static_cast<int32_t>(sourceLimit - source);
            अगर(length<targetCapacity) {
                targetCapacity=length;
            }
        }

#अगर MBCS_UNROLL_SINGLE_TO_BMP
        /* unrolling makes it faster on Pentium III/Windows 2000 */
        जाओ unrolled;
#पूर्ण_अगर
    }

    अगर(U_SUCCESS(*pErrorCode) && source<sourceLimit && target>=pArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
    }

    /* set offsets since the start or the last callback */
    अगर(offsets!=nullptr) {
        माप_प्रकार count=source-lastSource;
        जबतक(count>0) {
            *offsets++=source_सूचकांक++;
            --count;
        }
    }

    /* write back the updated pointers */
    pArgs->source = reinterpret_cast<स्थिर अक्षर*>(source);
    pArgs->target=target;
    pArgs->offsets=offsets;
}

अटल UBool
hasValidTrailBytes(स्थिर int32_t (*stateTable)[256], uint8_t state) {
    स्थिर int32_t *row=stateTable[state];
    int32_t b, entry;
    /* First test for final entries in this state for some commonly valid byte values. */
    entry=row[0xa1];
    अगर( !MBCS_ENTRY_IS_TRANSITION(entry) &&
        MBCS_ENTRY_FINAL_ACTION(entry)!=MBCS_STATE_ILLEGAL
    ) {
        वापस true;
    }
    entry=row[0x41];
    अगर( !MBCS_ENTRY_IS_TRANSITION(entry) &&
        MBCS_ENTRY_FINAL_ACTION(entry)!=MBCS_STATE_ILLEGAL
    ) {
        वापस true;
    }
    /* Then test for final entries in this state. */
    क्रम(b=0; b<=0xff; ++b) {
        entry=row[b];
        अगर( !MBCS_ENTRY_IS_TRANSITION(entry) &&
            MBCS_ENTRY_FINAL_ACTION(entry)!=MBCS_STATE_ILLEGAL
        ) {
            वापस true;
        }
    }
    /* Then recurse for transition entries. */
    क्रम(b=0; b<=0xff; ++b) {
        entry=row[b];
        अगर( MBCS_ENTRY_IS_TRANSITION(entry) &&
            hasValidTrailBytes(stateTable, static_cast<uint8_t>(MBCS_ENTRY_TRANSITION_STATE(entry)))
        ) {
            वापस true;
        }
    }
    वापस false;
}

/*
 * Is byte b a single/lead byte in this state?
 * Recurse for transition states, because here we don't want to say that
 * b is a lead byte if all byte sequences that start with b are illegal.
 */
अटल UBool
isSingleOrLead(स्थिर int32_t (*stateTable)[256], uint8_t state, UBool isDBCSOnly, uint8_t b) {
    स्थिर int32_t *row=stateTable[state];
    int32_t entry=row[b];
    अगर(MBCS_ENTRY_IS_TRANSITION(entry)) {   /* lead byte */
        वापस hasValidTrailBytes(stateTable, static_cast<uint8_t>(MBCS_ENTRY_TRANSITION_STATE(entry)));
    } अन्यथा {
        uint8_t action = static_cast<uint8_t>(MBCS_ENTRY_FINAL_ACTION(entry));
        अगर(action==MBCS_STATE_CHANGE_ONLY && isDBCSOnly) {
            वापस false;   /* SI/SO are illegal for DBCS-only conversion */
        } अन्यथा {
            वापस action!=MBCS_STATE_ILLEGAL;
        }
    }
}

U_CFUNC व्योम
ucnv_MBCSToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                          UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर uint8_t *source, *sourceLimit;
    char16_t *target;
    स्थिर char16_t *targetLimit;
    int32_t *offsets;

    स्थिर int32_t (*stateTable)[256];
    स्थिर uint16_t *unicodeCodeUnits;

    uint32_t offset;
    uint8_t state;
    int8_t byte_सूचकांक;
    uint8_t *bytes;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    int32_t entry;
    char16_t c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=pArgs->converter;

    अगर(cnv->preToULength>0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_extContinueMatchToU(cnv, pArgs, -1, pErrorCode);

        अगर(U_FAILURE(*pErrorCode) || cnv->preToULength<0) {
            वापस;
        }
    }

    अगर(cnv->shared_तथ्य->mbcs.गिनती_States==1) {
        अगर(!(cnv->shared_तथ्य->mbcs.unicodeMask&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingleToBMPWithOffsets(pArgs, pErrorCode);
        } अन्यथा {
            ucnv_MBCSSingleToUnicodeWithOffsets(pArgs, pErrorCode);
        }
        वापस;
    }

    /* set up the local pointers */
    source=(स्थिर uint8_t *)pArgs->source;
    sourceLimit=(स्थिर uint8_t *)pArgs->sourceLimit;
    target=pArgs->target;
    targetLimit=pArgs->targetLimit;
    offsets=pArgs->offsets;

    अगर((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        stateTable=(स्थिर int32_t (*)[256])cnv->shared_तथ्य->mbcs.swapLFNLStateTable;
    } अन्यथा {
        stateTable=cnv->shared_तथ्य->mbcs.stateTable;
    }
    unicodeCodeUnits=cnv->shared_तथ्य->mbcs.unicodeCodeUnits;

    /* get the converter state from UConverter */
    offset=cnv->toUnicodeStatus;
    byte_सूचकांक=cnv->toULength;
    bytes=cnv->toUBytes;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    अगर((state=(uint8_t)(cnv->mode))==0) {
        state=cnv->shared_तथ्य->mbcs.dbcsOnlyState;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक=byte_सूचकांक==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    /* conversion loop */
    जबतक(source<sourceLimit) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        अगर(target>=targetLimit) {
            /* target is full */
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }

        अगर(byte_सूचकांक==0) {
            /* optimized loop for 1/2-byte input and BMP output */
            अगर(offsets==nullptr) {
                करो {
                    entry=stateTable[state][*source];
                    अगर(MBCS_ENTRY_IS_TRANSITION(entry)) {
                        state=(uint8_t)MBCS_ENTRY_TRANSITION_STATE(entry);
                        offset=MBCS_ENTRY_TRANSITION_OFFSET(entry);

                        ++source;
                        अगर( source<sourceLimit &&
                            MBCS_ENTRY_IS_FINAL(entry=stateTable[state][*source]) &&
                            MBCS_ENTRY_FINAL_ACTION(entry)==MBCS_STATE_VALID_16 &&
                            (c=unicodeCodeUnits[offset+MBCS_ENTRY_FINAL_मूल्य_16(entry)])<0xfffe
                        ) {
                            ++source;
                            *target++=c;
                            state=(uint8_t)MBCS_ENTRY_FINAL_STATE(entry); /* typically 0 */
                            offset=0;
                        } अन्यथा {
                            /* set the state and leave the optimized loop */
                            bytes[0]=*(source-1);
                            byte_सूचकांक=1;
                            अवरोध;
                        }
                    } अन्यथा {
                        अगर(MBCS_ENTRY_FINAL_IS_VALID_DIRECT_16(entry)) {
                            /* output BMP code point */
                            ++source;
                            *target++=(char16_t)MBCS_ENTRY_FINAL_मूल्य_16(entry);
                            state=(uint8_t)MBCS_ENTRY_FINAL_STATE(entry); /* typically 0 */
                        } अन्यथा {
                            /* leave the optimized loop */
                            अवरोध;
                        }
                    }
                } जबतक(source<sourceLimit && target<targetLimit);
            } अन्यथा /* offsets!=nullptr */ {
                करो {
                    entry=stateTable[state][*source];
                    अगर(MBCS_ENTRY_IS_TRANSITION(entry)) {
                        state=(uint8_t)MBCS_ENTRY_TRANSITION_STATE(entry);
                        offset=MBCS_ENTRY_TRANSITION_OFFSET(entry);

                        ++source;
                        अगर( source<sourceLimit &&
                            MBCS_ENTRY_IS_FINAL(entry=stateTable[state][*source]) &&
                            MBCS_ENTRY_FINAL_ACTION(entry)==MBCS_STATE_VALID_16 &&
                            (c=unicodeCodeUnits[offset+MBCS_ENTRY_FINAL_मूल्य_16(entry)])<0xfffe
                        ) {
                            ++source;
                            *target++=c;
                            अगर(offsets!=nullptr) {
                                *offsets++=source_सूचकांक;
                                source_सूचकांक=(अग्र_Source_सूचकांक+=2);
                            }
                            state=(uint8_t)MBCS_ENTRY_FINAL_STATE(entry); /* typically 0 */
                            offset=0;
                        } अन्यथा {
                            /* set the state and leave the optimized loop */
                            ++अग्र_Source_सूचकांक;
                            bytes[0]=*(source-1);
                            byte_सूचकांक=1;
                            अवरोध;
                        }
                    } अन्यथा {
                        अगर(MBCS_ENTRY_FINAL_IS_VALID_DIRECT_16(entry)) {
                            /* output BMP code point */
                            ++source;
                            *target++=(char16_t)MBCS_ENTRY_FINAL_मूल्य_16(entry);
                            अगर(offsets!=nullptr) {
                                *offsets++=source_सूचकांक;
                                source_सूचकांक=++अग्र_Source_सूचकांक;
                            }
                            state=(uint8_t)MBCS_ENTRY_FINAL_STATE(entry); /* typically 0 */
                        } अन्यथा {
                            /* leave the optimized loop */
                            अवरोध;
                        }
                    }
                } जबतक(source<sourceLimit && target<targetLimit);
            }

            /*
             * these tests and break statements could be put inside the loop
             * if C had "break outerLoop" like Java
             */
            अगर(source>=sourceLimit) {
                अवरोध;
            }
            अगर(target>=targetLimit) {
                /* target is full */
                *pErrorCode=U_बफर_OVERFLOW_ERROR;
                अवरोध;
            }

            ++अग्र_Source_सूचकांक;
            bytes[byte_सूचकांक++]=*source++;
        } अन्यथा /* byteIndex>0 */ {
            ++अग्र_Source_सूचकांक;
            entry=stateTable[state][bytes[byte_सूचकांक++]=*source++];
        }

        अगर(MBCS_ENTRY_IS_TRANSITION(entry)) {
            state=(uint8_t)MBCS_ENTRY_TRANSITION_STATE(entry);
            offset+=MBCS_ENTRY_TRANSITION_OFFSET(entry);
            जारी;
        }

        /* save the previous state for proper extension mapping with SI/SO-stateful converters */
        cnv->mode=state;

        /* set the next state early so that we can reuse the entry variable */
        state=(uint8_t)MBCS_ENTRY_FINAL_STATE(entry); /* typically 0 */

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action=(uint8_t)(MBCS_ENTRY_FINAL_ACTION(entry));
        अगर(action==MBCS_STATE_VALID_16) {
            offset+=MBCS_ENTRY_FINAL_मूल्य_16(entry);
            c=unicodeCodeUnits[offset];
            अगर(c<0xfffe) {
                /* output BMP code point */
                *target++=c;
                अगर(offsets!=nullptr) {
                    *offsets++=source_सूचकांक;
                }
                byte_सूचकांक=0;
            } अन्यथा अगर(c==0xfffe) {
                अगर(UCNV_TO_U_USE_FALLBACK(cnv) && (entry=(int32_t)ucnv_MBCSGetFallback(&cnv->shared_तथ्य->mbcs, offset))!=0xfffe) {
                    /* output fallback BMP code point */
                    *target++=(char16_t)entry;
                    अगर(offsets!=nullptr) {
                        *offsets++=source_सूचकांक;
                    }
                    byte_सूचकांक=0;
                }
            } अन्यथा {
                /* callback(illegal) */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            }
        } अन्यथा अगर(action==MBCS_STATE_VALID_DIRECT_16) {
            /* output BMP code point */
            *target++=(char16_t)MBCS_ENTRY_FINAL_मूल्य_16(entry);
            अगर(offsets!=nullptr) {
                *offsets++=source_सूचकांक;
            }
            byte_सूचकांक=0;
        } अन्यथा अगर(action==MBCS_STATE_VALID_16_PAIR) {
            offset+=MBCS_ENTRY_FINAL_मूल्य_16(entry);
            c=unicodeCodeUnits[offset++];
            अगर(c<0xd800) {
                /* output BMP code point below 0xd800 */
                *target++=c;
                अगर(offsets!=nullptr) {
                    *offsets++=source_सूचकांक;
                }
                byte_सूचकांक=0;
            } अन्यथा अगर(UCNV_TO_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                /* output roundtrip or fallback surrogate pair */
                *target++=(char16_t)(c&0xdbff);
                अगर(offsets!=nullptr) {
                    *offsets++=source_सूचकांक;
                }
                byte_सूचकांक=0;
                अगर(target<targetLimit) {
                    *target++=unicodeCodeUnits[offset];
                    अगर(offsets!=nullptr) {
                        *offsets++=source_सूचकांक;
                    }
                } अन्यथा {
                    /* target overflow */
                    cnv->UChar_Error_बफर[0]=unicodeCodeUnits[offset];
                    cnv->UChar_Error_बफर_लंबाई=1;
                    *pErrorCode=U_बफर_OVERFLOW_ERROR;

                    offset=0;
                    अवरोध;
                }
            } अन्यथा अगर(UCNV_TO_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                *target++=unicodeCodeUnits[offset];
                अगर(offsets!=nullptr) {
                    *offsets++=source_सूचकांक;
                }
                byte_सूचकांक=0;
            } अन्यथा अगर(c==0xffff) {
                /* callback(illegal) */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            }
        } अन्यथा अगर(action==MBCS_STATE_VALID_DIRECT_20 ||
                  (action==MBCS_STATE_FALLBACK_DIRECT_20 && UCNV_TO_U_USE_FALLBACK(cnv))
        ) {
            entry=MBCS_ENTRY_FINAL_मूल्य(entry);
            /* output surrogate pair */
            *target++=(char16_t)(0xd800|(char16_t)(entry>>10));
            अगर(offsets!=nullptr) {
                *offsets++=source_सूचकांक;
            }
            byte_सूचकांक=0;
            c=(char16_t)(0xdc00|(char16_t)(entry&0x3ff));
            अगर(target<targetLimit) {
                *target++=c;
                अगर(offsets!=nullptr) {
                    *offsets++=source_सूचकांक;
                }
            } अन्यथा {
                /* target overflow */
                cnv->UChar_Error_बफर[0]=c;
                cnv->UChar_Error_बफर_लंबाई=1;
                *pErrorCode=U_बफर_OVERFLOW_ERROR;

                offset=0;
                अवरोध;
            }
        } अन्यथा अगर(action==MBCS_STATE_CHANGE_ONLY) {
            /*
             * This serves as a state change without any output.
             * It is useful for reading simple stateful encodings,
             * for example using just Shift-In/Shift-Out codes.
             * The 21 unused bits may later be used for more sophisticated
             * state transitions.
             */
            अगर(cnv->shared_तथ्य->mbcs.dbcsOnlyState==0) {
                byte_सूचकांक=0;
            } अन्यथा {
                /* SI/SO are illegal for DBCS-only conversion */
                state=(uint8_t)(cnv->mode); /* restore the previous state */

                /* callback(illegal) */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            }
        } अन्यथा अगर(action==MBCS_STATE_FALLBACK_DIRECT_16) {
            अगर(UCNV_TO_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *target++=(char16_t)MBCS_ENTRY_FINAL_मूल्य_16(entry);
                अगर(offsets!=nullptr) {
                    *offsets++=source_सूचकांक;
                }
                byte_सूचकांक=0;
            }
        } अन्यथा अगर(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } अन्यथा अगर(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        } अन्यथा {
            /* reserved, must never occur */
            byte_सूचकांक=0;
        }

        /* end of action codes: prepare for a new character */
        offset=0;

        अगर(byte_सूचकांक==0) {
            source_सूचकांक=अग्र_Source_सूचकांक;
        } अन्यथा अगर(U_FAILURE(*pErrorCode)) {
            /* callback(illegal) */
            अगर(byte_सूचकांक>1) {
                /*
                 * Ticket 5691: consistent illegal sequences:
                 * - We include at least the first byte in the illegal sequence.
                 * - If any of the non-initial bytes could be the start of a character,
                 *   we stop the illegal sequence before the first one of those.
                 */
                UBool isDBCSOnly = cnv->shared_तथ्य->mbcs.dbcsOnlyState != 0;
                int8_t i;
                क्रम(i=1;
                    i<byte_सूचकांक && !isSingleOrLead(stateTable, state, isDBCSOnly, bytes[i]);
                    ++i) {}
                अगर(i<byte_सूचकांक) {
                    /* Back out some bytes. */
                    int8_t backOutDistance=byte_सूचकांक-i;
                    int32_t bytes_From_This_बफर=(int32_t)(source-(स्थिर uint8_t *)pArgs->source);
                    byte_सूचकांक=i;  /* length of reported illegal byte sequence */
                    अगर(backOutDistance<=bytes_From_This_बफर) {
                        source-=backOutDistance;
                    } अन्यथा {
                        /* Back out bytes from the previous buffer: Need to replay them. */
                        cnv->preToULength=(int8_t)(bytes_From_This_बफर-backOutDistance);
                        /* preToULength is negative! */
                        uprv_memcpy(cnv->preToU, bytes+i, -cnv->preToULength);
                        source=(स्थिर uint8_t *)pArgs->source;
                    }
                }
            }
            अवरोध;
        } अन्यथा /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            pArgs->source=(स्थिर अक्षर *)source;
            byte_सूचकांक=_extToU(cnv, cnv->shared_तथ्य,
                              byte_सूचकांक, &source, sourceLimit,
                              &target, targetLimit,
                              &offsets, source_सूचकांक,
                              pArgs->सफाई,
                              pErrorCode);
            source_सूचकांक=अग्र_Source_सूचकांक+=(int32_t)(source-(स्थिर uint8_t *)pArgs->source);

            अगर(U_FAILURE(*pErrorCode)) {
                /* not mappable or buffer overflow */
                अवरोध;
            }
        }
    }

    /* set the converter state back into UConverter */
    cnv->toUnicodeStatus=offset;
    cnv->mode=state;
    cnv->toULength=byte_सूचकांक;

    /* write back the updated pointers */
    pArgs->source=(स्थिर अक्षर *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

/*
 * This version of ucnv_MBCSGetNextUChar() is optimized for single-byte, single-state codepages.
 * We still need a conversion loop in case we find reserved action codes, which are to be ignored.
 */
अटल UChar32
ucnv_MBCSSingle_Get_अग्र_UChar(UConverterToUnicodeArgs *pArgs,
                        UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर int32_t (*stateTable)[256];
    स्थिर uint8_t *source, *sourceLimit;

    int32_t entry;
    uint8_t action;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source = reinterpret_cast<स्थिर uint8_t*>(pArgs->source);
    sourceLimit = reinterpret_cast<स्थिर uint8_t*>(pArgs->sourceLimit);
    अगर((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        stateTable=(स्थिर int32_t (*)[256])cnv->shared_तथ्य->mbcs.swapLFNLStateTable;
    } अन्यथा {
        stateTable=cnv->shared_तथ्य->mbcs.stateTable;
    }

    /* conversion loop */
    जबतक(source<sourceLimit) {
        entry=stateTable[0][*source++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* write back the updated pointer early so that we can return directly */
        pArgs->source = reinterpret_cast<स्थिर अक्षर*>(source);

        अगर(MBCS_ENTRY_FINAL_IS_VALID_DIRECT_16(entry)) {
            /* output BMP code point */
            वापस static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_ENTRY_FINAL_ACTION(entry));
        अगर( action==MBCS_STATE_VALID_DIRECT_20 ||
            (action==MBCS_STATE_FALLBACK_DIRECT_20 && UCNV_TO_U_USE_FALLBACK(cnv))
        ) {
            /* output supplementary code point */
            वापस static_cast<UChar32>(MBCS_ENTRY_FINAL_मूल्य(entry) + 0x10000);
        } अन्यथा अगर(action==MBCS_STATE_FALLBACK_DIRECT_16) {
            अगर(UCNV_TO_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                वापस static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
            }
        } अन्यथा अगर(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } अन्यथा अगर(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        } अन्यथा {
            /* reserved, must never occur */
            जारी;
        }

        अगर(U_FAILURE(*pErrorCode)) {
            /* callback(illegal) */
            अवरोध;
        } अन्यथा /* unassigned sequence */ {
            /* defer to the generic implementation */
            pArgs->source = reinterpret_cast<स्थिर अक्षर*>(source) - 1;
            वापस UCNV_GET_अग्र_UCHAR_USE_TO_U;
        }
    }

    /* no output because of empty input or only state changes */
    *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
    वापस 0xffff;
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
अटल UChar32 U_CALLCONV
ucnv_MBCSGet_अग्र_UChar(UConverterToUnicodeArgs *pArgs,
                  UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर uint8_t *source, *sourceLimit, *lastSource;

    स्थिर int32_t (*stateTable)[256];
    स्थिर uint16_t *unicodeCodeUnits;

    uint32_t offset;
    uint8_t state;

    int32_t entry;
    UChar32 c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=pArgs->converter;

    अगर(cnv->preToULength>0) {
        /* use the generic code in ucnv_getNextUChar() to continue with a partial match */
        वापस UCNV_GET_अग्र_UCHAR_USE_TO_U;
    }

    अगर(cnv->shared_तथ्य->mbcs.unicodeMask&UCNV_HAS_SURROGATES) {
        /*
         * Using the generic ucnv_getNextUChar() code lets us deal correctly
         * with the rare case of a codepage that maps single surrogates
         * without adding the complexity to this already complicated function here.
         */
        वापस UCNV_GET_अग्र_UCHAR_USE_TO_U;
    } अन्यथा अगर(cnv->shared_तथ्य->mbcs.गिनती_States==1) {
        वापस ucnv_MBCSSingle_Get_अग्र_UChar(pArgs, pErrorCode);
    }

    /* set up the local pointers */
    source = lastSource = reinterpret_cast<स्थिर uint8_t*>(pArgs->source);
    sourceLimit = reinterpret_cast<स्थिर uint8_t*>(pArgs->sourceLimit);

    अगर((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        stateTable=(स्थिर int32_t (*)[256])cnv->shared_तथ्य->mbcs.swapLFNLStateTable;
    } अन्यथा {
        stateTable=cnv->shared_तथ्य->mbcs.stateTable;
    }
    unicodeCodeUnits=cnv->shared_तथ्य->mbcs.unicodeCodeUnits;

    /* get the converter state from UConverter */
    offset=cnv->toUnicodeStatus;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    अगर ((state = static_cast<uint8_t>(cnv->mode)) == 0) {
        state=cnv->shared_तथ्य->mbcs.dbcsOnlyState;
    }

    /* conversion loop */
    c=U_SENTINEL;
    जबतक(source<sourceLimit) {
        entry=stateTable[state][*source++];
        अगर(MBCS_ENTRY_IS_TRANSITION(entry)) {
            state = static_cast<uint8_t>(MBCS_ENTRY_TRANSITION_STATE(entry));
            offset+=MBCS_ENTRY_TRANSITION_OFFSET(entry);

            /* optimization for 1/2-byte input and BMP output */
            अगर( source<sourceLimit &&
                MBCS_ENTRY_IS_FINAL(entry=stateTable[state][*source]) &&
                MBCS_ENTRY_FINAL_ACTION(entry)==MBCS_STATE_VALID_16 &&
                (c=unicodeCodeUnits[offset+MBCS_ENTRY_FINAL_मूल्य_16(entry)])<0xfffe
            ) {
                ++source;
                state = static_cast<uint8_t>(MBCS_ENTRY_FINAL_STATE(entry)); /* typically 0 */
                /* output BMP code point */
                अवरोध;
            }
        } अन्यथा {
            /* save the previous state for proper extension mapping with SI/SO-stateful converters */
            cnv->mode=state;

            /* set the next state early so that we can reuse the entry variable */
            state = static_cast<uint8_t>(MBCS_ENTRY_FINAL_STATE(entry)); /* typically 0 */

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action = static_cast<uint8_t>(MBCS_ENTRY_FINAL_ACTION(entry));
            अगर(action==MBCS_STATE_VALID_DIRECT_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
                अवरोध;
            } अन्यथा अगर(action==MBCS_STATE_VALID_16) {
                offset+=MBCS_ENTRY_FINAL_मूल्य_16(entry);
                c=unicodeCodeUnits[offset];
                अगर(c<0xfffe) {
                    /* output BMP code point */
                    अवरोध;
                } अन्यथा अगर(c==0xfffe) {
                    अगर(UCNV_TO_U_USE_FALLBACK(cnv) && (c=ucnv_MBCSGetFallback(&cnv->shared_तथ्य->mbcs, offset))!=0xfffe) {
                        अवरोध;
                    }
                } अन्यथा {
                    /* callback(illegal) */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } अन्यथा अगर(action==MBCS_STATE_VALID_16_PAIR) {
                offset+=MBCS_ENTRY_FINAL_मूल्य_16(entry);
                c=unicodeCodeUnits[offset++];
                अगर(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                    अवरोध;
                } अन्यथा अगर(UCNV_TO_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicodeCodeUnits[offset]+(0x10000-0xdc00);
                    अवरोध;
                } अन्यथा अगर(UCNV_TO_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicodeCodeUnits[offset];
                    अवरोध;
                } अन्यथा अगर(c==0xffff) {
                    /* callback(illegal) */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } अन्यथा अगर(action==MBCS_STATE_VALID_DIRECT_20 ||
                      (action==MBCS_STATE_FALLBACK_DIRECT_20 && UCNV_TO_U_USE_FALLBACK(cnv))
            ) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_ENTRY_FINAL_मूल्य(entry) + 0x10000);
                अवरोध;
            } अन्यथा अगर(action==MBCS_STATE_CHANGE_ONLY) {
                /*
                 * This serves as a state change without any output.
                 * It is useful for reading simple stateful encodings,
                 * for example using just Shift-In/Shift-Out codes.
                 * The 21 unused bits may later be used for more sophisticated
                 * state transitions.
                 */
                अगर(cnv->shared_तथ्य->mbcs.dbcsOnlyState!=0) {
                    /* SI/SO are illegal for DBCS-only conversion */
                    state = static_cast<uint8_t>(cnv->mode); /* restore the previous state */

                    /* callback(illegal) */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } अन्यथा अगर(action==MBCS_STATE_FALLBACK_DIRECT_16) {
                अगर(UCNV_TO_U_USE_FALLBACK(cnv)) {
                    /* output BMP code point */
                    c = static_cast<char16_t>(MBCS_ENTRY_FINAL_मूल्य_16(entry));
                    अवरोध;
                }
            } अन्यथा अगर(action==MBCS_STATE_UNASSIGNED) {
                /* just fall through */
            } अन्यथा अगर(action==MBCS_STATE_ILLEGAL) {
                /* callback(illegal) */
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            } अन्यथा {
                /* reserved (must never occur), or only state change */
                offset=0;
                lastSource=source;
                जारी;
            }

            /* end of action codes: prepare for a new character */
            offset=0;

            अगर(U_FAILURE(*pErrorCode)) {
                /* callback(illegal) */
                अवरोध;
            } अन्यथा /* unassigned sequence */ {
                /* defer to the generic implementation */
                cnv->toUnicodeStatus=0;
                cnv->mode=state;
                pArgs->source = reinterpret_cast<स्थिर अक्षर*>(lastSource);
                वापस UCNV_GET_अग्र_UCHAR_USE_TO_U;
            }
        }
    }

    अगर(c<0) {
        अगर(U_SUCCESS(*pErrorCode) && source==sourceLimit && lastSource<source) {
            /* incomplete character byte sequence */
            cnv->toULength = static_cast<int8_t>(source - lastSource);
            uprv_memcpy(cnv->toUBytes, lastSource, cnv->toULength);
            *pErrorCode=U_TRUNCATED_CHAR_FOUND;
        } अन्यथा अगर(U_FAILURE(*pErrorCode)) {
            /* callback(illegal) */
            /*
             * Ticket 5691: consistent illegal sequences:
             * - We include at least the first byte in the illegal sequence.
             * - If any of the non-initial bytes could be the start of a character,
             *   we stop the illegal sequence before the first one of those.
             */
            UBool isDBCSOnly = static_cast<UBool>(cnv->shared_तथ्य->mbcs.dbcsOnlyState != 0);
            uint8_t *bytes=cnv->toUBytes;
            *bytes++=*lastSource++;     /* first byte */
            अगर(lastSource==source) {
                cnv->toULength=1;
            } अन्यथा /* lastSource<source: multi-byte character */ {
                int8_t i;
                क्रम(i=1;
                    lastSource<source && !isSingleOrLead(stateTable, state, isDBCSOnly, *lastSource);
                    ++i
                ) {
                    *bytes++=*lastSource++;
                }
                cnv->toULength=i;
                source=lastSource;
            }
        } अन्यथा {
            /* no output because of empty input or only state changes */
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        }
        c=0xffff;
    }

    /* set the converter state back into UConverter, ready for a new character */
    cnv->toUnicodeStatus=0;
    cnv->mode=state;

    /* write back the updated pointer */
    pArgs->source = reinterpret_cast<स्थिर अक्षर*>(source);
    वापस c;
}

#अगर 0
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
ucnv_MBCSSingle_Simple_Get_अग्र_UChar(UConverter_Shared_तथ्य *shared_तथ्य,
                              uint8_t b, UBool useFallback) {
    int32_t entry;
    uint8_t action;

    entry=shared_तथ्य->mbcs.stateTable[0][b];
    /* MBCS_ENTRY_IS_FINAL(entry) */

    अगर(MBCS_ENTRY_FINAL_IS_VALID_DIRECT_16(entry)) {
        /* output BMP code point */
        वापस (char16_t)MBCS_ENTRY_FINAL_मूल्य_16(entry);
    }

    /*
     * An if-else-if chain provides more reliable performance for
     * the most common cases compared to a switch.
     */
    action=(uint8_t)(MBCS_ENTRY_FINAL_ACTION(entry));
    अगर(action==MBCS_STATE_VALID_DIRECT_20) {
        /* output supplementary code point */
        वापस 0x10000+MBCS_ENTRY_FINAL_मूल्य(entry);
    } अन्यथा अगर(action==MBCS_STATE_FALLBACK_DIRECT_16) {
        अगर(!TO_U_USE_FALLBACK(useFallback)) {
            वापस 0xfffe;
        }
        /* output BMP code point */
        वापस (char16_t)MBCS_ENTRY_FINAL_मूल्य_16(entry);
    } अन्यथा अगर(action==MBCS_STATE_FALLBACK_DIRECT_20) {
        अगर(!TO_U_USE_FALLBACK(useFallback)) {
            वापस 0xfffe;
        }
        /* output supplementary code point */
        वापस 0x10000+MBCS_ENTRY_FINAL_मूल्य(entry);
    } अन्यथा अगर(action==MBCS_STATE_UNASSIGNED) {
        वापस 0xfffe;
    } अन्यथा अगर(action==MBCS_STATE_ILLEGAL) {
        वापस 0xffff;
    } अन्यथा {
        /* reserved, must never occur */
        वापस 0xffff;
    }
}
#पूर्ण_अगर

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
ucnv_MBCSSimple_Get_अग्र_UChar(UConverter_Shared_तथ्य *shared_तथ्य,
                        स्थिर अक्षर *source, int32_t length,
                        UBool useFallback) {
    स्थिर int32_t (*stateTable)[256];
    स्थिर uint16_t *unicodeCodeUnits;

    uint32_t offset;
    uint8_t state, action;

    UChar32 c;
    int32_t i, entry;

    अगर(length<=0) {
        /* no input at all: "illegal" */
        वापस 0xffff;
    }

#अगर 0
/*
 * Code disabled 2002dec09 (ICU 2.4) because it is not currently used in ICU. markus
 * TODO In future releases, verify that this function is never called for SBCS
 * conversions, i.e., that sharedData->mbcs.countStates==1 is still true.
 * Removal improves code coverage.
 */
    /* use optimized function if possible */
    अगर(shared_तथ्य->mbcs.गिनती_States==1) {
        अगर(length==1) {
            वापस ucnv_MBCSSingle_Simple_Get_अग्र_UChar(shared_तथ्य, (uint8_t)*source, useFallback);
        } अन्यथा {
            वापस 0xffff; /* illegal: more than a single byte for an SBCS converter */
        }
    }
#पूर्ण_अगर

    /* set up the local pointers */
    stateTable=shared_तथ्य->mbcs.stateTable;
    unicodeCodeUnits=shared_तथ्य->mbcs.unicodeCodeUnits;

    /* converter state */
    offset=0;
    state=shared_तथ्य->mbcs.dbcsOnlyState;

    /* conversion loop */
    क्रम(i=0;;) {
        entry=stateTable[state][(uint8_t)source[i++]];
        अगर(MBCS_ENTRY_IS_TRANSITION(entry)) {
            state=(uint8_t)MBCS_ENTRY_TRANSITION_STATE(entry);
            offset+=MBCS_ENTRY_TRANSITION_OFFSET(entry);

            अगर(i==length) {
                वापस 0xffff; /* truncated character */
            }
        } अन्यथा {
            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=(uint8_t)(MBCS_ENTRY_FINAL_ACTION(entry));
            अगर(action==MBCS_STATE_VALID_16) {
                offset+=MBCS_ENTRY_FINAL_मूल्य_16(entry);
                c=unicodeCodeUnits[offset];
                अगर(c!=0xfffe) {
                    /* done */
                } अन्यथा अगर(UCNV_TO_U_USE_FALLBACK(cnv)) {
                    c=ucnv_MBCSGetFallback(&shared_तथ्य->mbcs, offset);
                /* else done with 0xfffe */
                }
                अवरोध;
            } अन्यथा अगर(action==MBCS_STATE_VALID_DIRECT_16) {
                /* output BMP code point */
                c=(char16_t)MBCS_ENTRY_FINAL_मूल्य_16(entry);
                अवरोध;
            } अन्यथा अगर(action==MBCS_STATE_VALID_16_PAIR) {
                offset+=MBCS_ENTRY_FINAL_मूल्य_16(entry);
                c=unicodeCodeUnits[offset++];
                अगर(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } अन्यथा अगर(UCNV_TO_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=(UChar32)(((c&0x3ff)<<10)+unicodeCodeUnits[offset]+(0x10000-0xdc00));
                } अन्यथा अगर(UCNV_TO_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicodeCodeUnits[offset];
                } अन्यथा अगर(c==0xffff) {
                    वापस 0xffff;
                } अन्यथा {
                    c=0xfffe;
                }
                अवरोध;
            } अन्यथा अगर(action==MBCS_STATE_VALID_DIRECT_20) {
                /* output supplementary code point */
                c=0x10000+MBCS_ENTRY_FINAL_मूल्य(entry);
                अवरोध;
            } अन्यथा अगर(action==MBCS_STATE_FALLBACK_DIRECT_16) {
                अगर(!TO_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    अवरोध;
                }
                /* output BMP code point */
                c=(char16_t)MBCS_ENTRY_FINAL_मूल्य_16(entry);
                अवरोध;
            } अन्यथा अगर(action==MBCS_STATE_FALLBACK_DIRECT_20) {
                अगर(!TO_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    अवरोध;
                }
                /* output supplementary code point */
                c=0x10000+MBCS_ENTRY_FINAL_मूल्य(entry);
                अवरोध;
            } अन्यथा अगर(action==MBCS_STATE_UNASSIGNED) {
                c=0xfffe;
                अवरोध;
            }

            /*
             * forbid MBCS_STATE_CHANGE_ONLY for this function,
             * and MBCS_STATE_ILLEGAL and reserved action codes
             */
            वापस 0xffff;
        }
    }

    अगर(i!=length) {
        /* illegal for this function: not all input consumed */
        वापस 0xffff;
    }

    अगर(c==0xfffe) {
        /* try an extension mapping */
        स्थिर int32_t *cx=shared_तथ्य->mbcs.extIndexes;
        अगर(cx!=nullptr) {
            वापस ucnv_extSimpleMatchToU(cx, source, length, useFallback);
        }
    }

    वापस c;
}

/* MBCS-from-Unicode conversion functions ----------------------------------- */

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for double-byte codepages. */
अटल व्योम
ucnv_MBCSDoubleFromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                                  UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर char16_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity;
    int32_t *offsets;

    स्थिर uint16_t *table;
    स्थिर uint16_t *mbcs_सूचकांक;
    स्थिर uint8_t *bytes;

    UChar32 c;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    uint32_t stage2Entry;
    uint32_t asciiRoundtrips;
    uint32_t value;
    uint8_t unicodeMask;

    /* use optimized function if possible */
    cnv=pArgs->converter;
    unicodeMask=cnv->shared_तथ्य->mbcs.unicodeMask;

    /* set up the local pointers */
    source=pArgs->source;
    sourceLimit=pArgs->sourceLimit;
    target = reinterpret_cast<uint8_t*>(pArgs->target);
    targetCapacity = static_cast<int32_t>(pArgs->targetLimit - pArgs->target);
    offsets=pArgs->offsets;

    table=cnv->shared_तथ्य->mbcs.fromUnicodeTable;
    mbcs_सूचकांक=cnv->shared_तथ्य->mbcs.mbcs_सूचकांक;
    अगर((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        bytes=cnv->shared_तथ्य->mbcs.swapLFNLFromUnicodeBytes;
    } अन्यथा {
        bytes=cnv->shared_तथ्य->mbcs.fromUnicodeBytes;
    }
    asciiRoundtrips=cnv->shared_तथ्य->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक= c==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    /* conversion loop */
    अगर(c!=0 && targetCapacity>0) {
        जाओ getTrail;
    }

    जबतक(source<sourceLimit) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        अगर(targetCapacity>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*source++;
            ++अग्र_Source_सूचकांक;
            अगर(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *target++ = static_cast<uint8_t>(c);
                अगर(offsets!=nullptr) {
                    *offsets++=source_सूचकांक;
                    source_सूचकांक=अग्र_Source_सूचकांक;
                }
                --targetCapacity;
                c=0;
                जारी;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            अगर(c<=0xd7ff) {
                value=DBCS_परिणाम_FROM_MOST_BMP(mbcs_सूचकांक, (स्थिर uint16_t *)bytes, c);
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                अगर(value==0) {
                    जाओ unassigned;
                }
                /* output the value */
            } अन्यथा {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                अगर(U16_IS_SURROGATE(c) && !(unicodeMask&UCNV_HAS_SURROGATES)) {
                    अगर(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        अगर(source<sourceLimit) {
                            /* test the following code unit */
                            char16_t trail=*source;
                            अगर(U16_IS_TRAIL(trail)) {
                                ++source;
                                ++अग्र_Source_सूचकांक;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                अगर(!(unicodeMask&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    /* callback(unassigned) */
                                    जाओ unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } अन्यथा {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                                अवरोध;
                            }
                        } अन्यथा {
                            /* no more input */
                            अवरोध;
                        }
                    } अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        अवरोध;
                    }
                }

                /* convert the Unicode code point in c into codepage bytes */
                stage2Entry=MBCS_STAGE_2_FROM_U(table, c);

                /* get the bytes and the length for the output */
                /* MBCS_OUTPUT_2 */
                value=MBCS_मूल्य_2_FROM_STAGE_2(bytes, stage2Entry, c);

                /* is this code point assigned, or do we use fallbacks? */
                अगर(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2Entry, c) ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    pArgs->source=source;
                    c=_extFromU(cnv, cnv->shared_तथ्य,
                                c, &source, sourceLimit,
                                &target, target+targetCapacity,
                                &offsets, source_सूचकांक,
                                pArgs->सफाई,
                                pErrorCode);
                    अग्र_Source_सूचकांक += static_cast<int32_t>(source - pArgs->source);

                    अगर(U_FAILURE(*pErrorCode)) {
                        /* not mappable or buffer overflow */
                        अवरोध;
                    } अन्यथा {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        targetCapacity = static_cast<int32_t>(pArgs->targetLimit - reinterpret_cast<अक्षर*>(target));

                        /* normal end of conversion: prepare for a new character */
                        source_सूचकांक=अग्र_Source_सूचकांक;
                        जारी;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            अगर(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *target++ = static_cast<uint8_t>(value);
                अगर(offsets!=nullptr) {
                    *offsets++=source_सूचकांक;
                }
                --targetCapacity;
            } अन्यथा /* length==2 */ {
                *target++ = static_cast<uint8_t>(value >> 8);
                अगर(2<=targetCapacity) {
                    *target++ = static_cast<uint8_t>(value);
                    अगर(offsets!=nullptr) {
                        *offsets++=source_सूचकांक;
                        *offsets++=source_सूचकांक;
                    }
                    targetCapacity-=2;
                } अन्यथा {
                    अगर(offsets!=nullptr) {
                        *offsets++=source_सूचकांक;
                    }
                    cnv->char_Error_बफर[0] = static_cast<अक्षर>(value);
                    cnv->char_Error_बफर_लंबाई=1;

                    /* target overflow */
                    targetCapacity=0;
                    *pErrorCode=U_बफर_OVERFLOW_ERROR;
                    c=0;
                    अवरोध;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            source_सूचकांक=अग्र_Source_सूचकांक;
            जारी;
        } अन्यथा {
            /* target is full */
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target = reinterpret_cast<अक्षर*>(target);
    pArgs->offsets=offsets;
}

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for single-byte codepages. */
अटल व्योम
ucnv_MBCSSingleFromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                                  UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर char16_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity;
    int32_t *offsets;

    स्थिर uint16_t *table;
    स्थिर uint16_t *results;

    UChar32 c;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    uint16_t value, min_मूल्य;
    UBool hasSupplementary;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source=pArgs->source;
    sourceLimit=pArgs->sourceLimit;
    target = reinterpret_cast<uint8_t*>(pArgs->target);
    targetCapacity = static_cast<int32_t>(pArgs->targetLimit - pArgs->target);
    offsets=pArgs->offsets;

    table=cnv->shared_तथ्य->mbcs.fromUnicodeTable;
    अगर((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_तथ्य->mbcs.swapLFNLFromUnicodeBytes);
    } अन्यथा {
        results=(uint16_t *)cnv->shared_तथ्य->mbcs.fromUnicodeBytes;
    }

    अगर(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_मूल्य=0x800;
    } अन्यथा {
        /* use only roundtrips and fallbacks from private-use characters */
        min_मूल्य=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_तथ्य->mbcs.unicodeMask & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक= c==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    /* conversion loop */
    अगर(c!=0 && targetCapacity>0) {
        जाओ getTrail;
    }

    जबतक(source<sourceLimit) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        अगर(targetCapacity>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*source++;
            ++अग्र_Source_सूचकांक;
            अगर(U16_IS_SURROGATE(c)) {
                अगर(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                    अगर(source<sourceLimit) {
                        /* test the following code unit */
                        char16_t trail=*source;
                        अगर(U16_IS_TRAIL(trail)) {
                            ++source;
                            ++अग्र_Source_सूचकांक;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            अगर(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                /* callback(unassigned) */
                                जाओ unassigned;
                            }
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } अन्यथा {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                            अवरोध;
                        }
                    } अन्यथा {
                        /* no more input */
                        अवरोध;
                    }
                } अन्यथा {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                    अवरोध;
                }
            }

            /* convert the Unicode code point in c into codepage bytes */
            value=MBCS_SINGLE_परिणाम_FROM_U(table, results, c);

            /* is this code point assigned, or do we use fallbacks? */
            अगर(value>=min_मूल्य) {
                /* assigned, write the output character bytes from value and length */
                /* length==1 */
                /* this is easy because we know that there is enough space */
                *target++ = static_cast<uint8_t>(value);
                अगर(offsets!=nullptr) {
                    *offsets++=source_सूचकांक;
                }
                --targetCapacity;

                /* normal end of conversion: prepare for a new character */
                c=0;
                source_सूचकांक=अग्र_Source_सूचकांक;
            } अन्यथा { /* unassigned */
unassigned:
                /* try an extension mapping */
                pArgs->source=source;
                c=_extFromU(cnv, cnv->shared_तथ्य,
                            c, &source, sourceLimit,
                            &target, target+targetCapacity,
                            &offsets, source_सूचकांक,
                            pArgs->सफाई,
                            pErrorCode);
                अग्र_Source_सूचकांक += static_cast<int32_t>(source - pArgs->source);

                अगर(U_FAILURE(*pErrorCode)) {
                    /* not mappable or buffer overflow */
                    अवरोध;
                } अन्यथा {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    targetCapacity = static_cast<int32_t>(pArgs->targetLimit - reinterpret_cast<अक्षर*>(target));

                    /* normal end of conversion: prepare for a new character */
                    source_सूचकांक=अग्र_Source_सूचकांक;
                }
            }
        } अन्यथा {
            /* target is full */
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target = reinterpret_cast<अक्षर*>(target);
    pArgs->offsets=offsets;
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
अटल व्योम
ucnv_MBCSSingleFromBMPWithOffsets(UConverterFromUnicodeArgs *pArgs,
                              UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर char16_t *source, *sourceLimit, *lastSource;
    uint8_t *target;
    int32_t targetCapacity, length;
    int32_t *offsets;

    स्थिर uint16_t *table;
    स्थिर uint16_t *results;

    UChar32 c;

    int32_t source_सूचकांक;

    uint32_t asciiRoundtrips;
    uint16_t value, min_मूल्य;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source=pArgs->source;
    sourceLimit=pArgs->sourceLimit;
    target = reinterpret_cast<uint8_t*>(pArgs->target);
    targetCapacity = static_cast<int32_t>(pArgs->targetLimit - pArgs->target);
    offsets=pArgs->offsets;

    table=cnv->shared_तथ्य->mbcs.fromUnicodeTable;
    अगर((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_तथ्य->mbcs.swapLFNLFromUnicodeBytes);
    } अन्यथा {
        results=(uint16_t *)cnv->shared_तथ्य->mbcs.fromUnicodeBytes;
    }
    asciiRoundtrips=cnv->shared_तथ्य->mbcs.asciiRoundtrips;

    अगर(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_मूल्य=0x800;
    } अन्यथा {
        /* use only roundtrips and fallbacks from private-use characters */
        min_मूल्य=0xc00;
    }

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक= c==0 ? 0 : -1;
    lastSource=source;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(sourceLimit - source);
    अगर(length<targetCapacity) {
        targetCapacity=length;
    }

    /* conversion loop */
    अगर(c!=0 && targetCapacity>0) {
        जाओ getTrail;
    }

#अगर MBCS_UNROLL_SINGLE_FROM_BMP
    /* unrolling makes it slower on Pentium III/Windows 2000?! */
    /* unroll the loop with the most common case */
unrolled:
    अगर(targetCapacity>=4) {
        int32_t count, loops;
        uint16_t anded_मूल्यावली;

        loops=count=targetCapacity>>2;
        करो {
            c=*source++;
            anded_मूल्यावली=value=MBCS_SINGLE_परिणाम_FROM_U(table, results, c);
            *target++=(uint8_t)value;
            c=*source++;
            anded_मूल्यावली&=value=MBCS_SINGLE_परिणाम_FROM_U(table, results, c);
            *target++=(uint8_t)value;
            c=*source++;
            anded_मूल्यावली&=value=MBCS_SINGLE_परिणाम_FROM_U(table, results, c);
            *target++=(uint8_t)value;
            c=*source++;
            anded_मूल्यावली&=value=MBCS_SINGLE_परिणाम_FROM_U(table, results, c);
            *target++=(uint8_t)value;

            /* were all 4 entries really valid? */
            अगर(anded_मूल्यावली<min_मूल्य) {
                /* no, return to the first of these 4 */
                source-=4;
                target-=4;
                अवरोध;
            }
        } जबतक(--count>0);
        count=loops-count;
        targetCapacity-=4*count;

        अगर(offsets!=nullptr) {
            lastSource+=4*count;
            जबतक(count>0) {
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                --count;
            }
        }

        c=0;
    }
#पूर्ण_अगर

    जबतक(targetCapacity>0) {
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
        अगर(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
            *target++ = static_cast<uint8_t>(c);
            --targetCapacity;
            c=0;
            जारी;
        }
        value=MBCS_SINGLE_परिणाम_FROM_U(table, results, c);
        /* is this code point assigned, or do we use fallbacks? */
        अगर(value>=min_मूल्य) {
            /* assigned, write the output character bytes from value and length */
            /* length==1 */
            /* this is easy because we know that there is enough space */
            *target++ = static_cast<uint8_t>(value);
            --targetCapacity;

            /* normal end of conversion: prepare for a new character */
            c=0;
            जारी;
        } अन्यथा अगर(!U16_IS_SURROGATE(c)) {
            /* normal, unassigned BMP character */
        } अन्यथा अगर(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
            अगर(source<sourceLimit) {
                /* test the following code unit */
                char16_t trail=*source;
                अगर(U16_IS_TRAIL(trail)) {
                    ++source;
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } अन्यथा {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                    अवरोध;
                }
            } अन्यथा {
                /* no more input */
                अगर (pArgs->सफाई) {
                    *pErrorCode=U_TRUNCATED_CHAR_FOUND;
                }
                अवरोध;
            }
        } अन्यथा {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            अवरोध;
        }

        /* c does not have a mapping */

        /* get the number of code units for c to correctly advance sourceIndex */
        length=U16_लंबाई(c);

        /* set offsets since the start or the last extension */
        अगर(offsets!=nullptr) {
            int32_t count = static_cast<int32_t>(source - lastSource);

            /* do not set the offset for this character */
            count-=length;

            जबतक(count>0) {
                *offsets++=source_सूचकांक++;
                --count;
            }
            /* offsets and sourceIndex are now set for the current character */
        }

        /* try an extension mapping */
        lastSource=source;
        c=_extFromU(cnv, cnv->shared_तथ्य,
                    c, &source, sourceLimit,
                    &target, reinterpret_cast<स्थिर uint8_t*>(pArgs->targetLimit),
                    &offsets, source_सूचकांक,
                    pArgs->सफाई,
                    pErrorCode);
        source_सूचकांक += length + static_cast<int32_t>(source - lastSource);
        lastSource=source;

        अगर(U_FAILURE(*pErrorCode)) {
            /* not mappable or buffer overflow */
            अवरोध;
        } अन्यथा {
            /* a mapping was written to the target, continue */

            /* recalculate the targetCapacity after an extension mapping */
            targetCapacity = static_cast<int32_t>(pArgs->targetLimit - reinterpret_cast<अक्षर*>(target));
            length = static_cast<int32_t>(sourceLimit - source);
            अगर(length<targetCapacity) {
                targetCapacity=length;
            }
        }

#अगर MBCS_UNROLL_SINGLE_FROM_BMP
        /* unrolling makes it slower on Pentium III/Windows 2000?! */
        जाओ unrolled;
#पूर्ण_अगर
    }

    अगर(U_SUCCESS(*pErrorCode) && source<sourceLimit && target>=(uint8_t *)pArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
    }

    /* set offsets since the start or the last callback */
    अगर(offsets!=nullptr) {
        माप_प्रकार count=source-lastSource;
        अगर (count > 0 && *pErrorCode == U_TRUNCATED_CHAR_FOUND) {
            /*
            Caller gave us a partial supplementary character,
            which this function couldn't convert in any case.
            The callback will handle the offset.
            */
            count--;
        }
        जबतक(count>0) {
            *offsets++=source_सूचकांक++;
            --count;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target = reinterpret_cast<अक्षर*>(target);
    pArgs->offsets=offsets;
}

U_CFUNC व्योम
ucnv_MBCSFromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                            UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर char16_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity;
    int32_t *offsets;

    स्थिर uint16_t *table;
    स्थिर uint16_t *mbcs_सूचकांक;
    स्थिर uint8_t *p, *bytes;
    uint8_t outputType;

    UChar32 c;

    int32_t पूर्व_Source_सूचकांक, source_सूचकांक, अग्र_Source_सूचकांक;

    uint32_t stage2Entry;
    uint32_t asciiRoundtrips;
    uint32_t value;
    /* Shift-In and Shift-Out byte sequences differ by encoding scheme. */
    uint8_t siBytes[2] = {0, 0};
    uint8_t soBytes[2] = {0, 0};
    uint8_t si_लंबाई, so_लंबाई;
    int32_t length = 0, पूर्व_लंबाई;
    uint8_t unicodeMask;

    cnv=pArgs->converter;

    अगर(cnv->preFromUFirstCP>=0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_extContinueMatchFromU(cnv, pArgs, -1, pErrorCode);

        अगर(U_FAILURE(*pErrorCode) || cnv->preFromULength<0) {
            वापस;
        }
    }

    /* use optimized function if possible */
    outputType=cnv->shared_तथ्य->mbcs.outputType;
    unicodeMask=cnv->shared_तथ्य->mbcs.unicodeMask;
    अगर(outputType==MBCS_OUTPUT_1 && !(unicodeMask&UCNV_HAS_SURROGATES)) {
        अगर(!(unicodeMask&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingleFromBMPWithOffsets(pArgs, pErrorCode);
        } अन्यथा {
            ucnv_MBCSSingleFromUnicodeWithOffsets(pArgs, pErrorCode);
        }
        वापस;
    } अन्यथा अगर(outputType==MBCS_OUTPUT_2 && cnv->shared_तथ्य->mbcs.utf8Friendly) {
        ucnv_MBCSDoubleFromUnicodeWithOffsets(pArgs, pErrorCode);
        वापस;
    }

    /* set up the local pointers */
    source=pArgs->source;
    sourceLimit=pArgs->sourceLimit;
    target=(uint8_t *)pArgs->target;
    targetCapacity=(int32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;

    table=cnv->shared_तथ्य->mbcs.fromUnicodeTable;
    अगर(cnv->shared_तथ्य->mbcs.utf8Friendly) {
        mbcs_सूचकांक=cnv->shared_तथ्य->mbcs.mbcs_सूचकांक;
    } अन्यथा {
        mbcs_सूचकांक=nullptr;
    }
    अगर((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        bytes=cnv->shared_तथ्य->mbcs.swapLFNLFromUnicodeBytes;
    } अन्यथा {
        bytes=cnv->shared_तथ्य->mbcs.fromUnicodeBytes;
    }
    asciiRoundtrips=cnv->shared_तथ्य->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    अगर(outputType==MBCS_OUTPUT_2_SISO) {
        पूर्व_लंबाई=cnv->fromUnicodeStatus;
        अगर(पूर्व_लंबाई==0) {
            /* set the real value */
            पूर्व_लंबाई=1;
        }
    } अन्यथा {
        /* prevent fromUnicodeStatus from being set to something non-0 */
        पूर्व_लंबाई=0;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    पूर्व_Source_सूचकांक=-1;
    source_सूचकांक= c==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    /* Get the SI/SO character for the converter */
    si_लंबाई = static_cast<uint8_t>(getSISOBytes(SI, cnv->options, siBytes));
    so_लंबाई = static_cast<uint8_t>(getSISOBytes(SO, cnv->options, soBytes));

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
    अगर(c!=0 && targetCapacity>0) {
        जाओ getTrail;
    }

    जबतक(source<sourceLimit) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        अगर(targetCapacity>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*source++;
            ++अग्र_Source_सूचकांक;
            अगर(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *target++=(uint8_t)c;
                अगर(offsets!=nullptr) {
                    *offsets++=source_सूचकांक;
                    पूर्व_Source_सूचकांक=source_सूचकांक;
                    source_सूचकांक=अग्र_Source_सूचकांक;
                }
                --targetCapacity;
                c=0;
                जारी;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            अगर(c<=0xd7ff && mbcs_सूचकांक!=nullptr) {
                value=mbcs_सूचकांक[c>>6];

                /* get the bytes and the length for the output (copied from below and adapted for utf8Friendly data) */
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                चयन(outputType) {
                हाल MBCS_OUTPUT_2:
                    value=((स्थिर uint16_t *)bytes)[value +(c&0x3f)];
                    अगर(value<=0xff) {
                        अगर(value==0) {
                            जाओ unassigned;
                        } अन्यथा {
                            length=1;
                        }
                    } अन्यथा {
                        length=2;
                    }
                    अवरोध;
                हाल MBCS_OUTPUT_2_SISO:
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
                    cnv->fromUnicodeStatus=पूर्व_लंबाई; /* save the old state */
                    value=((स्थिर uint16_t *)bytes)[value +(c&0x3f)];
                    अगर(value<=0xff) {
                        अगर(value==0) {
                            जाओ unassigned;
                        } अन्यथा अगर(पूर्व_लंबाई<=1) {
                            length=1;
                        } अन्यथा {
                            /* change from double-byte mode to single-byte */
                            अगर (si_लंबाई == 1) {
                                value|=(uint32_t)siBytes[0]<<8;
                                length = 2;
                            } अन्यथा अगर (si_लंबाई == 2) {
                                value|=(uint32_t)siBytes[1]<<8;
                                value|=(uint32_t)siBytes[0]<<16;
                                length = 3;
                            }
                            पूर्व_लंबाई=1;
                        }
                    } अन्यथा {
                        अगर(पूर्व_लंबाई==2) {
                            length=2;
                        } अन्यथा {
                            /* change from single-byte mode to double-byte */
                            अगर (so_लंबाई == 1) {
                                value|=(uint32_t)soBytes[0]<<16;
                                length = 3;
                            } अन्यथा अगर (so_लंबाई == 2) {
                                value|=(uint32_t)soBytes[1]<<16;
                                value|=(uint32_t)soBytes[0]<<24;
                                length = 4;
                            }
                            पूर्व_लंबाई=2;
                        }
                    }
                    अवरोध;
                हाल MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=((स्थिर uint16_t *)bytes)[value +(c&0x3f)];
                    अगर(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        जाओ unassigned;
                    } अन्यथा {
                        length=2;
                    }
                    अवरोध;
                हाल MBCS_OUTPUT_3:
                    p=bytes+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    अगर(value<=0xff) {
                        अगर(value==0) {
                            जाओ unassigned;
                        } अन्यथा {
                            length=1;
                        }
                    } अन्यथा अगर(value<=0xffff) {
                        length=2;
                    } अन्यथा {
                        length=3;
                    }
                    अवरोध;
                हाल MBCS_OUTPUT_4:
                    value=((स्थिर uint32_t *)bytes)[value +(c&0x3f)];
                    अगर(value<=0xff) {
                        अगर(value==0) {
                            जाओ unassigned;
                        } अन्यथा {
                            length=1;
                        }
                    } अन्यथा अगर(value<=0xffff) {
                        length=2;
                    } अन्यथा अगर(value<=0xffffff) {
                        length=3;
                    } अन्यथा {
                        length=4;
                    }
                    अवरोध;
                हाल MBCS_OUTPUT_3_EUC:
                    value=((स्थिर uint16_t *)bytes)[value +(c&0x3f)];
                    /* EUC 16-bit fixed-length representation */
                    अगर(value<=0xff) {
                        अगर(value==0) {
                            जाओ unassigned;
                        } अन्यथा {
                            length=1;
                        }
                    } अन्यथा अगर((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } अन्यथा अगर((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } अन्यथा {
                        length=2;
                    }
                    अवरोध;
                हाल MBCS_OUTPUT_4_EUC:
                    p=bytes+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    अगर(value<=0xff) {
                        अगर(value==0) {
                            जाओ unassigned;
                        } अन्यथा {
                            length=1;
                        }
                    } अन्यथा अगर(value<=0xffff) {
                        length=2;
                    } अन्यथा अगर((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } अन्यथा अगर((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } अन्यथा {
                        length=3;
                    }
                    अवरोध;
                शेष:
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
                    अवरोध;
                }
                /* output the value */
            } अन्यथा {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                अगर(U16_IS_SURROGATE(c) && !(unicodeMask&UCNV_HAS_SURROGATES)) {
                    अगर(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        अगर(source<sourceLimit) {
                            /* test the following code unit */
                            char16_t trail=*source;
                            अगर(U16_IS_TRAIL(trail)) {
                                ++source;
                                ++अग्र_Source_सूचकांक;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                अगर(!(unicodeMask&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    cnv->fromUnicodeStatus=पूर्व_लंबाई; /* save the old state */
                                    /* callback(unassigned) */
                                    जाओ unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } अन्यथा {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                                अवरोध;
                            }
                        } अन्यथा {
                            /* no more input */
                            अवरोध;
                        }
                    } अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        अवरोध;
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
                stage2Entry=MBCS_STAGE_2_FROM_U(table, c);

                /* get the bytes and the length for the output */
                चयन(outputType) {
                हाल MBCS_OUTPUT_2:
                    value=MBCS_मूल्य_2_FROM_STAGE_2(bytes, stage2Entry, c);
                    अगर(value<=0xff) {
                        length=1;
                    } अन्यथा {
                        length=2;
                    }
                    अवरोध;
                हाल MBCS_OUTPUT_2_SISO:
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
                    cnv->fromUnicodeStatus=पूर्व_लंबाई; /* save the old state */
                    value=MBCS_मूल्य_2_FROM_STAGE_2(bytes, stage2Entry, c);
                    अगर(value<=0xff) {
                        अगर(value==0 && MBCS_FROM_U_IS_ROUNDTRIP(stage2Entry, c)==0) {
                            /* no mapping, leave value==0 */
                            length=0;
                        } अन्यथा अगर(पूर्व_लंबाई<=1) {
                            length=1;
                        } अन्यथा {
                            /* change from double-byte mode to single-byte */
                            अगर (si_लंबाई == 1) {
                                value|=(uint32_t)siBytes[0]<<8;
                                length = 2;
                            } अन्यथा अगर (si_लंबाई == 2) {
                                value|=(uint32_t)siBytes[1]<<8;
                                value|=(uint32_t)siBytes[0]<<16;
                                length = 3;
                            }
                            पूर्व_लंबाई=1;
                        }
                    } अन्यथा {
                        अगर(पूर्व_लंबाई==2) {
                            length=2;
                        } अन्यथा {
                            /* change from single-byte mode to double-byte */
                            अगर (so_लंबाई == 1) {
                                value|=(uint32_t)soBytes[0]<<16;
                                length = 3;
                            } अन्यथा अगर (so_लंबाई == 2) {
                                value|=(uint32_t)soBytes[1]<<16;
                                value|=(uint32_t)soBytes[0]<<24;
                                length = 4;
                            }
                            पूर्व_लंबाई=2;
                        }
                    }
                    अवरोध;
                हाल MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=MBCS_मूल्य_2_FROM_STAGE_2(bytes, stage2Entry, c);
                    अगर(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        value=stage2Entry=0; /* stage2Entry=0 to reset roundtrip flags */
                        length=0;
                    } अन्यथा {
                        length=2;
                    }
                    अवरोध;
                हाल MBCS_OUTPUT_3:
                    p=MBCS_POINTER_3_FROM_STAGE_2(bytes, stage2Entry, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    अगर(value<=0xff) {
                        length=1;
                    } अन्यथा अगर(value<=0xffff) {
                        length=2;
                    } अन्यथा {
                        length=3;
                    }
                    अवरोध;
                हाल MBCS_OUTPUT_4:
                    value=MBCS_मूल्य_4_FROM_STAGE_2(bytes, stage2Entry, c);
                    अगर(value<=0xff) {
                        length=1;
                    } अन्यथा अगर(value<=0xffff) {
                        length=2;
                    } अन्यथा अगर(value<=0xffffff) {
                        length=3;
                    } अन्यथा {
                        length=4;
                    }
                    अवरोध;
                हाल MBCS_OUTPUT_3_EUC:
                    value=MBCS_मूल्य_2_FROM_STAGE_2(bytes, stage2Entry, c);
                    /* EUC 16-bit fixed-length representation */
                    अगर(value<=0xff) {
                        length=1;
                    } अन्यथा अगर((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } अन्यथा अगर((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } अन्यथा {
                        length=2;
                    }
                    अवरोध;
                हाल MBCS_OUTPUT_4_EUC:
                    p=MBCS_POINTER_3_FROM_STAGE_2(bytes, stage2Entry, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    अगर(value<=0xff) {
                        length=1;
                    } अन्यथा अगर(value<=0xffff) {
                        length=2;
                    } अन्यथा अगर((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } अन्यथा अगर((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } अन्यथा {
                        length=3;
                    }
                    अवरोध;
                शेष:
                    /* must not occur */
                    /*
                     * To avoid compiler warnings that value & length may be
                     * used without having been initialized, we set them here.
                     * In reality, this is unreachable code.
                     * Not having a default branch also causes warnings with
                     * some compilers.
                     */
                    value=stage2Entry=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                    अवरोध;
                }

                /* is this code point assigned, or do we use fallbacks? */
                अगर(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2Entry, c)!=0 ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    pArgs->source=source;
                    c=_extFromU(cnv, cnv->shared_तथ्य,
                                c, &source, sourceLimit,
                                &target, target+targetCapacity,
                                &offsets, source_सूचकांक,
                                pArgs->सफाई,
                                pErrorCode);
                    अग्र_Source_सूचकांक+=(int32_t)(source-pArgs->source);
                    पूर्व_लंबाई=cnv->fromUnicodeStatus; /* restore SISO state */

                    अगर(U_FAILURE(*pErrorCode)) {
                        /* not mappable or buffer overflow */
                        अवरोध;
                    } अन्यथा {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        targetCapacity=(int32_t)(pArgs->targetLimit-(अक्षर *)target);

                        /* normal end of conversion: prepare for a new character */
                        अगर(offsets!=nullptr) {
                            पूर्व_Source_सूचकांक=source_सूचकांक;
                            source_सूचकांक=अग्र_Source_सूचकांक;
                        }
                        जारी;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            अगर(length<=targetCapacity) {
                अगर(offsets==nullptr) {
                    चयन(length) {
                        /* each branch falls through to the next one */
                    हाल 4:
                        *target++=(uint8_t)(value>>24);
                        U_FALLTHROUGH;
                    हाल 3:
                        *target++=(uint8_t)(value>>16);
                        U_FALLTHROUGH;
                    हाल 2:
                        *target++=(uint8_t)(value>>8);
                        U_FALLTHROUGH;
                    हाल 1:
                        *target++=(uint8_t)value;
                        U_FALLTHROUGH;
                    शेष:
                        /* will never occur */
                        अवरोध;
                    }
                } अन्यथा {
                    चयन(length) {
                        /* each branch falls through to the next one */
                    हाल 4:
                        *target++=(uint8_t)(value>>24);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    हाल 3:
                        *target++=(uint8_t)(value>>16);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    हाल 2:
                        *target++=(uint8_t)(value>>8);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    हाल 1:
                        *target++=(uint8_t)value;
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    शेष:
                        /* will never occur */
                        अवरोध;
                    }
                }
                targetCapacity-=length;
            } अन्यथा {
                uint8_t *char_Error_बफर;

                /*
                 * We actually do this backwards here:
                 * In order to save an intermediate variable, we output
                 * first to the overflow buffer what does not fit into the
                 * regular target.
                 */
                /* we know that 1<=targetCapacity<length<=4 */
                length-=targetCapacity;
                char_Error_बफर=(uint8_t *)cnv->char_Error_बफर;
                चयन(length) {
                    /* each branch falls through to the next one */
                हाल 3:
                    *char_Error_बफर++=(uint8_t)(value>>16);
                    U_FALLTHROUGH;
                हाल 2:
                    *char_Error_बफर++=(uint8_t)(value>>8);
                    U_FALLTHROUGH;
                हाल 1:
                    *char_Error_बफर=(uint8_t)value;
                    U_FALLTHROUGH;
                शेष:
                    /* will never occur */
                    अवरोध;
                }
                cnv->char_Error_बफर_लंबाई=(int8_t)length;

                /* now output what fits into the regular target */
                value>>=8*length; /* length was reduced by targetCapacity */
                चयन(targetCapacity) {
                    /* each branch falls through to the next one */
                हाल 3:
                    *target++=(uint8_t)(value>>16);
                    अगर(offsets!=nullptr) {
                        *offsets++=source_सूचकांक;
                    }
                    U_FALLTHROUGH;
                हाल 2:
                    *target++=(uint8_t)(value>>8);
                    अगर(offsets!=nullptr) {
                        *offsets++=source_सूचकांक;
                    }
                    U_FALLTHROUGH;
                हाल 1:
                    *target++=(uint8_t)value;
                    अगर(offsets!=nullptr) {
                        *offsets++=source_सूचकांक;
                    }
                    U_FALLTHROUGH;
                शेष:
                    /* will never occur */
                    अवरोध;
                }

                /* target overflow */
                targetCapacity=0;
                *pErrorCode=U_बफर_OVERFLOW_ERROR;
                c=0;
                अवरोध;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            अगर(offsets!=nullptr) {
                पूर्व_Source_सूचकांक=source_सूचकांक;
                source_सूचकांक=अग्र_Source_सूचकांक;
            }
            जारी;
        } अन्यथा {
            /* target is full */
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            अवरोध;
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
    अगर( U_SUCCESS(*pErrorCode) &&
        outputType==MBCS_OUTPUT_2_SISO && पूर्व_लंबाई==2 &&
        pArgs->सफाई && source>=sourceLimit && c==0
    ) {
        /* EBCDIC_STATEFUL ending with DBCS: emit an SI to return the output stream to SBCS */
        अगर(targetCapacity>0) {
            *target++ = siBytes[0];
            अगर (si_लंबाई == 2) {
                अगर (targetCapacity<2) {
                    cnv->char_Error_बफर[0] = siBytes[1];
                    cnv->char_Error_बफर_लंबाई=1;
                    *pErrorCode=U_बफर_OVERFLOW_ERROR;
                } अन्यथा {
                    *target++ = siBytes[1];
                }
            }
            अगर(offsets!=nullptr) {
                /* set the last source character's index (sourceIndex points at sourceLimit now) */
                *offsets++=पूर्व_Source_सूचकांक;
            }
        } अन्यथा {
            /* target is full */
            cnv->char_Error_बफर[0] = siBytes[0];
            अगर (si_लंबाई == 2) {
                cnv->char_Error_बफर[1] = siBytes[1];
            }
            cnv->char_Error_बफर_लंबाई=si_लंबाई;
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
        }
        पूर्व_लंबाई=1; /* we switched into SBCS */
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;
    cnv->fromUnicodeStatus=पूर्व_लंबाई;

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target=(अक्षर *)target;
    pArgs->offsets=offsets;
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
ucnv_MBCSFromUChar32(UConverter_Shared_तथ्य *shared_तथ्य,
                 UChar32 c, uint32_t *p_मूल्य,
                 UBool useFallback) {
    स्थिर int32_t *cx;
    स्थिर uint16_t *table;
#अगर 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
    स्थिर uint8_t *p;
#पूर्ण_अगर
    uint32_t stage2Entry;
    uint32_t value;
    int32_t length;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    अगर(c<=0xffff || (shared_तथ्य->mbcs.unicodeMask&UCNV_HAS_SUPPLEMENTARY)) {
        table=shared_तथ्य->mbcs.fromUnicodeTable;

        /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
        अगर(shared_तथ्य->mbcs.outputType==MBCS_OUTPUT_1) {
            value=MBCS_SINGLE_परिणाम_FROM_U(table, (uint16_t *)shared_तथ्य->mbcs.fromUnicodeBytes, c);
            /* is this code point assigned, or do we use fallbacks? */
            अगर(useFallback ? value>=0x800 : value>=0xc00) {
                *p_मूल्य=value&0xff;
                वापस 1;
            }
        } अन्यथा /* outputType!=MBCS_OUTPUT_1 */ {
            stage2Entry=MBCS_STAGE_2_FROM_U(table, c);

            /* get the bytes and the length for the output */
            चयन(shared_तथ्य->mbcs.outputType) {
            हाल MBCS_OUTPUT_2:
                value=MBCS_मूल्य_2_FROM_STAGE_2(shared_तथ्य->mbcs.fromUnicodeBytes, stage2Entry, c);
                अगर(value<=0xff) {
                    length=1;
                } अन्यथा {
                    length=2;
                }
                अवरोध;
#अगर 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
            हाल MBCS_OUTPUT_DBCS_ONLY:
                /* table with single-byte results, but only DBCS mappings used */
                value=MBCS_मूल्य_2_FROM_STAGE_2(shared_तथ्य->mbcs.fromUnicodeBytes, stage2Entry, c);
                अगर(value<=0xff) {
                    /* no mapping or SBCS result, not taken for DBCS-only */
                    value=stage2Entry=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                } अन्यथा {
                    length=2;
                }
                अवरोध;
            हाल MBCS_OUTPUT_3:
                p=MBCS_POINTER_3_FROM_STAGE_2(shared_तथ्य->mbcs.fromUnicodeBytes, stage2Entry, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                अगर(value<=0xff) {
                    length=1;
                } अन्यथा अगर(value<=0xffff) {
                    length=2;
                } अन्यथा {
                    length=3;
                }
                अवरोध;
            हाल MBCS_OUTPUT_4:
                value=MBCS_मूल्य_4_FROM_STAGE_2(shared_तथ्य->mbcs.fromUnicodeBytes, stage2Entry, c);
                अगर(value<=0xff) {
                    length=1;
                } अन्यथा अगर(value<=0xffff) {
                    length=2;
                } अन्यथा अगर(value<=0xffffff) {
                    length=3;
                } अन्यथा {
                    length=4;
                }
                अवरोध;
            हाल MBCS_OUTPUT_3_EUC:
                value=MBCS_मूल्य_2_FROM_STAGE_2(shared_तथ्य->mbcs.fromUnicodeBytes, stage2Entry, c);
                /* EUC 16-bit fixed-length representation */
                अगर(value<=0xff) {
                    length=1;
                } अन्यथा अगर((value&0x8000)==0) {
                    value|=0x8e8000;
                    length=3;
                } अन्यथा अगर((value&0x80)==0) {
                    value|=0x8f0080;
                    length=3;
                } अन्यथा {
                    length=2;
                }
                अवरोध;
            हाल MBCS_OUTPUT_4_EUC:
                p=MBCS_POINTER_3_FROM_STAGE_2(shared_तथ्य->mbcs.fromUnicodeBytes, stage2Entry, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                /* EUC 16-bit fixed-length representation applied to the first two bytes */
                अगर(value<=0xff) {
                    length=1;
                } अन्यथा अगर(value<=0xffff) {
                    length=2;
                } अन्यथा अगर((value&0x800000)==0) {
                    value|=0x8e800000;
                    length=4;
                } अन्यथा अगर((value&0x8000)==0) {
                    value|=0x8f008000;
                    length=4;
                } अन्यथा {
                    length=3;
                }
                अवरोध;
#पूर्ण_अगर
            शेष:
                /* must not occur */
                वापस -1;
            }

            /* is this code point assigned, or do we use fallbacks? */
            अगर( MBCS_FROM_U_IS_ROUNDTRIP(stage2Entry, c) ||
                (FROM_U_USE_FALLBACK(useFallback, c) && value!=0)
            ) {
                /*
                 * We allow a 0 byte output if the "assigned" bit is set for this entry.
                 * There is no way with this data structure for fallback output
                 * to be a zero byte.
                 */
                /* assigned */
                *p_मूल्य=value;
                वापस length;
            }
        }
    }

    cx=shared_तथ्य->mbcs.extIndexes;
    अगर(cx!=nullptr) {
        length=ucnv_extSimpleMatchFromU(cx, c, p_मूल्य, useFallback);
        वापस length>=0 ? length : -length;  /* return abs(length); */
    }

    /* unassigned */
    वापस 0;
}


#अगर 0
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
ucnv_MBCSSingleFromUChar32(UConverter_Shared_तथ्य *shared_तथ्य,
                       UChar32 c,
                       UBool useFallback) {
    स्थिर uint16_t *table;
    int32_t value;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    अगर(c>=0x10000 && !(shared_तथ्य->mbcs.unicodeMask&UCNV_HAS_SUPPLEMENTARY)) {
        वापस -1;
    }

    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    table=shared_तथ्य->mbcs.fromUnicodeTable;

    /* get the byte for the output */
    value=MBCS_SINGLE_परिणाम_FROM_U(table, (uint16_t *)shared_तथ्य->mbcs.fromUnicodeBytes, c);
    /* is this code point assigned, or do we use fallbacks? */
    अगर(useFallback ? value>=0x800 : value>=0xc00) {
        वापस value&0xff;
    } अन्यथा {
        वापस -1;
    }
}
#पूर्ण_अगर

/* MBCS-from-UTF-8 conversion functions ------------------------------------- */

/* offsets for n-byte UTF-8 sequences that were calculated with ((lead<<6)+trail)<<6+trail... */
अटल स्थिर UChar32
utf8_offsets[5]={ 0, 0, 0x3080, 0xE2080, 0x3C82080 };

अटल व्योम U_CALLCONV
ucnv_SBCSFromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                  UConverterToUnicodeArgs *pToUArgs,
                  UErrorCode *pErrorCode) {
    UConverter *utf8, *cnv;
    स्थिर uint8_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity;

    स्थिर uint16_t *table, *sbcs_सूचकांक;
    स्थिर uint16_t *results;

    int8_t oldToULength, toULength, toULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t asciiRoundtrips;
    uint16_t value, min_मूल्य = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=pToUArgs->converter;
    cnv=pFromUArgs->converter;
    source=(uint8_t *)pToUArgs->source;
    sourceLimit=(uint8_t *)pToUArgs->sourceLimit;
    target = reinterpret_cast<uint8_t*>(pFromUArgs->target);
    targetCapacity = static_cast<int32_t>(pFromUArgs->targetLimit - pFromUArgs->target);

    table=cnv->shared_तथ्य->mbcs.fromUnicodeTable;
    sbcs_सूचकांक=cnv->shared_तथ्य->mbcs.sbcs_सूचकांक;
    अगर((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_तथ्य->mbcs.swapLFNLFromUnicodeBytes);
    } अन्यथा {
        results=(uint16_t *)cnv->shared_तथ्य->mbcs.fromUnicodeBytes;
    }
    asciiRoundtrips=cnv->shared_तथ्य->mbcs.asciiRoundtrips;

    अगर(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_मूल्य=0x800;
    } अन्यथा {
        /* use only roundtrips and fallbacks from private-use characters */
        min_मूल्य=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_तथ्य->mbcs.unicodeMask & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    अगर(utf8->toULength > 0) {
        toULength=oldToULength=utf8->toULength;
        toULimit = static_cast<int8_t>(utf8->mode);
        c = static_cast<UChar32>(utf8->toUnicodeStatus);
    } अन्यथा {
        toULength=oldToULength=toULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(sourceLimit - source) - (toULimit - oldToULength);
        अगर(length>0) {
            uint8_t b1=*(sourceLimit-1);
            अगर(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } अन्यथा अगर(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(sourceLimit-2);
                अगर(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    sourceLimit-=2;
                }
            } अन्यथा अगर(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --sourceLimit;
            }
        }
    }

    अगर(c!=0 && targetCapacity>0) {
        utf8->toUnicodeStatus=0;
        utf8->toULength=0;
        जाओ moreBytes;
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
    जबतक(source<sourceLimit) {
        अगर(targetCapacity>0) {
            b=*source++;
            अगर(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                अगर(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *target++ = b;
                    --targetCapacity;
                    जारी;
                } अन्यथा {
                    c=b;
                    value=SBCS_परिणाम_FROM_UTF8(sbcs_सूचकांक, results, 0, c);
                }
            } अन्यथा {
                अगर(b<0xe0) {
                    अगर( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*source - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++source;
                        value=SBCS_परिणाम_FROM_UTF8(sbcs_सूचकांक, results, c, t1);
                        अगर(value>=min_मूल्य) {
                            *target++ = static_cast<uint8_t>(value);
                            --targetCapacity;
                            जारी;
                        } अन्यथा {
                            c=(c<<6)|t1;
                        }
                    } अन्यथा {
                        c=-1;
                    }
                } अन्यथा अगर(b==0xe0) {
                    अगर( /* handle U+0800..U+0FFF inline */
                        (t1 = static_cast<uint8_t>(source[0] - 0x80)) <= 0x3f && t1 >= 0x20 &&
                        (t2 = static_cast<uint8_t>(source[1] - 0x80)) <= 0x3f
                    ) {
                        c=t1;
                        source+=2;
                        value=SBCS_परिणाम_FROM_UTF8(sbcs_सूचकांक, results, c, t2);
                        अगर(value>=min_मूल्य) {
                            *target++ = static_cast<uint8_t>(value);
                            --targetCapacity;
                            जारी;
                        } अन्यथा {
                            c=(c<<6)|t2;
                        }
                    } अन्यथा {
                        c=-1;
                    }
                } अन्यथा {
                    c=-1;
                }

                अगर(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    oldToULength=0;
                    toULength=1;
                    toULimit=U8_गिनती_BYTES_NON_ASCII(b);
                    c=b;
moreBytes:
                    जबतक(toULength<toULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        अगर(source<(uint8_t *)pToUArgs->sourceLimit) {
                            b=*source;
                            अगर(icu::UTF8::isValidTrail(c, b, toULength, toULimit)) {
                                ++source;
                                ++toULength;
                                c=(c<<6)+b;
                            } अन्यथा {
                                अवरोध; /* sequence too short, stop with toULength<toULimit */
                            }
                        } अन्यथा {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            source-=(toULength-oldToULength);
                            जबतक(oldToULength<toULength) {
                                utf8->toUBytes[oldToULength++]=*source++;
                            }
                            utf8->toUnicodeStatus=c;
                            utf8->toULength=toULength;
                            utf8->mode=toULimit;
                            pToUArgs->source=(अक्षर *)source;
                            pFromUArgs->target = reinterpret_cast<अक्षर*>(target);
                            वापस;
                        }
                    }

                    अगर(toULength==toULimit) {
                        c-=utf8_offsets[toULength];
                        अगर(toULength<=3) {  /* BMP */
                            value=MBCS_SINGLE_परिणाम_FROM_U(table, results, c);
                        } अन्यथा {
                            /* supplementary code point */
                            अगर(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                value=0;
                            } अन्यथा {
                                value=MBCS_SINGLE_परिणाम_FROM_U(table, results, c);
                            }
                        }
                    } अन्यथा {
                        /* error handling: illegal UTF-8 byte sequence */
                        source-=(toULength-oldToULength);
                        जबतक(oldToULength<toULength) {
                            utf8->toUBytes[oldToULength++]=*source++;
                        }
                        utf8->toULength=toULength;
                        pToUArgs->source=(अक्षर *)source;
                        pFromUArgs->target = reinterpret_cast<अक्षर*>(target);
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        वापस;
                    }
                }
            }

            अगर(value>=min_मूल्य) {
                /* output the mapping for c */
                *target++ = static_cast<uint8_t>(value);
                --targetCapacity;
            } अन्यथा {
                /* value<minValue means c is unassigned (unmappable) */
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                अटल स्थिर char16_t nul=0;
                स्थिर char16_t *noSource=&nul;
                c=_extFromU(cnv, cnv->shared_तथ्य,
                            c, &noSource, noSource,
                            &target, target+targetCapacity,
                            nullptr, -1,
                            pFromUArgs->सफाई,
                            pErrorCode);

                अगर(U_FAILURE(*pErrorCode)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    अवरोध;
                } अन्यथा अगर(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *pErrorCode=U_USING_DEFAULT_WARNING;
                    अवरोध;
                } अन्यथा {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    targetCapacity = static_cast<int32_t>(pFromUArgs->targetLimit - reinterpret_cast<अक्षर*>(target));
                }
            }
        } अन्यथा {
            /* target is full */
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    अगर(U_SUCCESS(*pErrorCode) &&
            cnv->preFromUFirstCP<0 &&
            source<(sourceLimit=(uint8_t *)pToUArgs->sourceLimit)) {
        c=utf8->toUBytes[0]=b=*source++;
        toULength=1;
        toULimit=U8_गिनती_BYTES(b);
        जबतक(source<sourceLimit) {
            utf8->toUBytes[toULength++]=b=*source++;
            c=(c<<6)+b;
        }
        utf8->toUnicodeStatus=c;
        utf8->toULength=toULength;
        utf8->mode=toULimit;
    }

    /* write back the updated pointers */
    pToUArgs->source=(अक्षर *)source;
    pFromUArgs->target = reinterpret_cast<अक्षर*>(target);
}

अटल व्योम U_CALLCONV
ucnv_DBCSFromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                  UConverterToUnicodeArgs *pToUArgs,
                  UErrorCode *pErrorCode) {
    UConverter *utf8, *cnv;
    स्थिर uint8_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity;

    स्थिर uint16_t *table, *mbcs_सूचकांक;
    स्थिर uint16_t *results;

    int8_t oldToULength, toULength, toULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t stage2Entry;
    uint32_t asciiRoundtrips;
    uint16_t value = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=pToUArgs->converter;
    cnv=pFromUArgs->converter;
    source=(uint8_t *)pToUArgs->source;
    sourceLimit=(uint8_t *)pToUArgs->sourceLimit;
    target = reinterpret_cast<uint8_t*>(pFromUArgs->target);
    targetCapacity = static_cast<int32_t>(pFromUArgs->targetLimit - pFromUArgs->target);

    table=cnv->shared_तथ्य->mbcs.fromUnicodeTable;
    mbcs_सूचकांक=cnv->shared_तथ्य->mbcs.mbcs_सूचकांक;
    अगर((cnv->options&UCNV_OPTION_SWAP_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_तथ्य->mbcs.swapLFNLFromUnicodeBytes);
    } अन्यथा {
        results=(uint16_t *)cnv->shared_तथ्य->mbcs.fromUnicodeBytes;
    }
    asciiRoundtrips=cnv->shared_तथ्य->mbcs.asciiRoundtrips;

    hasSupplementary = static_cast<UBool>(cnv->shared_तथ्य->mbcs.unicodeMask & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    अगर(utf8->toULength > 0) {
        toULength=oldToULength=utf8->toULength;
        toULimit = static_cast<int8_t>(utf8->mode);
        c = static_cast<UChar32>(utf8->toUnicodeStatus);
    } अन्यथा {
        toULength=oldToULength=toULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(sourceLimit - source) - (toULimit - oldToULength);
        अगर(length>0) {
            uint8_t b1=*(sourceLimit-1);
            अगर(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } अन्यथा अगर(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(sourceLimit-2);
                अगर(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    sourceLimit-=2;
                }
            } अन्यथा अगर(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --sourceLimit;
            }
        }
    }

    अगर(c!=0 && targetCapacity>0) {
        utf8->toUnicodeStatus=0;
        utf8->toULength=0;
        जाओ moreBytes;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    जबतक(source<sourceLimit) {
        अगर(targetCapacity>0) {
            b=*source++;
            अगर(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                अगर(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *target++=b;
                    --targetCapacity;
                    जारी;
                } अन्यथा {
                    value=DBCS_परिणाम_FROM_UTF8(mbcs_सूचकांक, results, 0, b);
                    अगर(value==0) {
                        c=b;
                        जाओ unassigned;
                    }
                }
            } अन्यथा {
                अगर(b>=0xe0) {
                    अगर( /* handle U+0800..U+D7FF inline */
                        b<=0xed &&  // do not assume maxFastUChar>0xd7ff
                        U8_IS_VALID_LEAD3_AND_T1(b, t1=source[0]) &&
                        (t2 = static_cast<uint8_t>(source[1] - 0x80)) <= 0x3f
                    ) {
                        c=((b&0xf)<<6)|(t1&0x3f);
                        source+=2;
                        value=DBCS_परिणाम_FROM_UTF8(mbcs_सूचकांक, results, c, t2);
                        अगर(value==0) {
                            c=(c<<6)|t2;
                            जाओ unassigned;
                        }
                    } अन्यथा {
                        c=-1;
                    }
                } अन्यथा {
                    अगर( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*source - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++source;
                        value=DBCS_परिणाम_FROM_UTF8(mbcs_सूचकांक, results, c, t1);
                        अगर(value==0) {
                            c=(c<<6)|t1;
                            जाओ unassigned;
                        }
                    } अन्यथा {
                        c=-1;
                    }
                }

                अगर(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    oldToULength=0;
                    toULength=1;
                    toULimit=U8_गिनती_BYTES_NON_ASCII(b);
                    c=b;
moreBytes:
                    जबतक(toULength<toULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        अगर(source<(uint8_t *)pToUArgs->sourceLimit) {
                            b=*source;
                            अगर(icu::UTF8::isValidTrail(c, b, toULength, toULimit)) {
                                ++source;
                                ++toULength;
                                c=(c<<6)+b;
                            } अन्यथा {
                                अवरोध; /* sequence too short, stop with toULength<toULimit */
                            }
                        } अन्यथा {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            source-=(toULength-oldToULength);
                            जबतक(oldToULength<toULength) {
                                utf8->toUBytes[oldToULength++]=*source++;
                            }
                            utf8->toUnicodeStatus=c;
                            utf8->toULength=toULength;
                            utf8->mode=toULimit;
                            pToUArgs->source=(अक्षर *)source;
                            pFromUArgs->target = reinterpret_cast<अक्षर*>(target);
                            वापस;
                        }
                    }

                    अगर(toULength==toULimit) {
                        c-=utf8_offsets[toULength];
                        अगर(toULength<=3) {  /* BMP */
                            stage2Entry=MBCS_STAGE_2_FROM_U(table, c);
                        } अन्यथा {
                            /* supplementary code point */
                            अगर(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                stage2Entry=0;
                            } अन्यथा {
                                stage2Entry=MBCS_STAGE_2_FROM_U(table, c);
                            }
                        }
                    } अन्यथा {
                        /* error handling: illegal UTF-8 byte sequence */
                        source-=(toULength-oldToULength);
                        जबतक(oldToULength<toULength) {
                            utf8->toUBytes[oldToULength++]=*source++;
                        }
                        utf8->toULength=toULength;
                        pToUArgs->source=(अक्षर *)source;
                        pFromUArgs->target = reinterpret_cast<अक्षर*>(target);
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        वापस;
                    }

                    /* get the bytes and the length for the output */
                    /* MBCS_OUTPUT_2 */
                    value=MBCS_मूल्य_2_FROM_STAGE_2(results, stage2Entry, c);

                    /* is this code point assigned, or do we use fallbacks? */
                    अगर(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2Entry, c) ||
                         (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                    ) {
                        जाओ unassigned;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            अगर(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *target++ = static_cast<uint8_t>(value);
                --targetCapacity;
            } अन्यथा /* length==2 */ {
                *target++ = static_cast<uint8_t>(value >> 8);
                अगर(2<=targetCapacity) {
                    *target++ = static_cast<uint8_t>(value);
                    targetCapacity-=2;
                } अन्यथा {
                    cnv->char_Error_बफर[0] = static_cast<अक्षर>(value);
                    cnv->char_Error_बफर_लंबाई=1;

                    /* target overflow */
                    *pErrorCode=U_बफर_OVERFLOW_ERROR;
                    अवरोध;
                }
            }
            जारी;

unassigned:
            {
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                अटल स्थिर char16_t nul=0;
                स्थिर char16_t *noSource=&nul;
                c=_extFromU(cnv, cnv->shared_तथ्य,
                            c, &noSource, noSource,
                            &target, target+targetCapacity,
                            nullptr, -1,
                            pFromUArgs->सफाई,
                            pErrorCode);

                अगर(U_FAILURE(*pErrorCode)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    अवरोध;
                } अन्यथा अगर(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *pErrorCode=U_USING_DEFAULT_WARNING;
                    अवरोध;
                } अन्यथा {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    targetCapacity = static_cast<int32_t>(pFromUArgs->targetLimit - reinterpret_cast<अक्षर*>(target));
                    जारी;
                }
            }
        } अन्यथा {
            /* target is full */
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    अगर(U_SUCCESS(*pErrorCode) &&
            cnv->preFromUFirstCP<0 &&
            source<(sourceLimit=(uint8_t *)pToUArgs->sourceLimit)) {
        c=utf8->toUBytes[0]=b=*source++;
        toULength=1;
        toULimit=U8_गिनती_BYTES(b);
        जबतक(source<sourceLimit) {
            utf8->toUBytes[toULength++]=b=*source++;
            c=(c<<6)+b;
        }
        utf8->toUnicodeStatus=c;
        utf8->toULength=toULength;
        utf8->mode=toULimit;
    }

    /* write back the updated pointers */
    pToUArgs->source=(अक्षर *)source;
    pFromUArgs->target = reinterpret_cast<अक्षर*>(target);
}

/* miscellaneous ------------------------------------------------------------ */

अटल व्योम U_CALLCONV
ucnv_MBCSGetStarters(स्थिर UConverter* cnv,
                 UBool starters[256],
                 UErrorCode *) {
    स्थिर int32_t *state0;
    पूर्णांक i;

    state0=cnv->shared_तथ्य->mbcs.stateTable[cnv->shared_तथ्य->mbcs.dbcsOnlyState];
    क्रम(i=0; i<256; ++i) {
        /* all bytes that cause a state transition from state 0 are lead bytes */
        starters[i] = static_cast<UBool>(MBCS_ENTRY_IS_TRANSITION(state0[i]));
    }
}

/*
 * This is an internal function that allows other converter implementations
 * to check whether a byte is a lead byte.
 */
U_CFUNC UBool
ucnv_MBCSIsLeadByte(UConverter_Shared_तथ्य *shared_तथ्य, अक्षर byte) {
    वापस MBCS_ENTRY_IS_TRANSITION(shared_तथ्य->mbcs.stateTable[0][(uint8_t)byte]);
}

अटल व्योम U_CALLCONV
ucnv_MBCSWriteSub(UConverterFromUnicodeArgs *pArgs,
              int32_t offset_सूचकांक,
              UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    अक्षर *p, *subchar;
    अक्षर बफर[4];
    int32_t length;

    /* first, select between subChar and subChar1 */
    अगर( cnv->subChar1!=0 &&
        (cnv->shared_तथ्य->mbcs.extIndexes!=nullptr ?
            cnv->useSubChar1 :
            (cnv->invalid_UChar_बफर[0]<=0xff))
    ) {
        /* select subChar1 if it is set (not 0) and the unmappable Unicode code point is up to U+00ff (IBM MBCS behavior) */
        subchar = reinterpret_cast<अक्षर*>(&cnv->subChar1);
        length=1;
    } अन्यथा {
        /* select subChar in all other cases */
        subchar = reinterpret_cast<अक्षर*>(cnv->subChars);
        length=cnv->subCharLen;
    }

    /* reset the selector for the next code point */
    cnv->useSubChar1=false;

    अगर (cnv->shared_तथ्य->mbcs.outputType == MBCS_OUTPUT_2_SISO) {
        p=बफर;

        /* fromUnicodeStatus contains prevLength */
        चयन(length) {
        हाल 1:
            अगर(cnv->fromUnicodeStatus==2) {
                /* DBCS mode and SBCS sub char: change to SBCS */
                cnv->fromUnicodeStatus=1;
                *p++=UCNV_SI;
            }
            *p++=subchar[0];
            अवरोध;
        हाल 2:
            अगर(cnv->fromUnicodeStatus<=1) {
                /* SBCS mode and DBCS sub char: change to DBCS */
                cnv->fromUnicodeStatus=2;
                *p++=UCNV_SO;
            }
            *p++=subchar[0];
            *p++=subchar[1];
            अवरोध;
        शेष:
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
            वापस;
        }
        subchar=बफर;
        length = static_cast<int32_t>(p - बफर);
    }

    ucnv_cbFromUWriteBytes(pArgs, subchar, length, offset_सूचकांक, pErrorCode);
}

U_CFUNC UConverterType
ucnv_MBCSGetType(स्थिर UConverter* converter) {
    /* SBCS, DBCS, and EBCDIC_STATEFUL are replaced by MBCS, but here we cheat a little */
    अगर(converter->shared_तथ्य->mbcs.गिनती_States==1) {
        वापस (UConverterType)UCNV_SBCS;
    } अन्यथा अगर((converter->shared_तथ्य->mbcs.outputType&0xff)==MBCS_OUTPUT_2_SISO) {
        वापस (UConverterType)UCNV_EBCDIC_STATEFUL;
    } अन्यथा अगर(converter->shared_तथ्य->static_तथ्य->minBytesPerChar==2 && converter->shared_तथ्य->static_तथ्य->maxBytesPerChar==2) {
        वापस (UConverterType)UCNV_DBCS;
    }
    वापस (UConverterType)UCNV_MBCS;
}

#पूर्ण_अगर /* #if !UCONFIG_NO_LEGACY_CONVERSION */
