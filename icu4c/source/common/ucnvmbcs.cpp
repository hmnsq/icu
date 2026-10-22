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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_cb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnvmbcs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_ext.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

/* control optimizations according to the platform */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MBCS_UNROLL_SINGLE_ਮੰਜ਼ਿਲ_BMP 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MBCS_UNROLL_SINGLE_FROM_BMP 0

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ UBool U_CALLCONV
UConverter_Enum_ਮੰਜ਼ਿਲ_UCallback(ਅਚਲ ਖਾਲੀ *context, uint32_t value, UChar32 ਕੋਡ_Points[32]);

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ,
          UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
          ਅਚਲ uint8_t *raw,
          UError_ਕੋਡ *p_Error_ਕੋਡ);

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ);

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
              UError_ਕੋਡ *p_Error_ਕੋਡ);

ਸਥਿਰ UChar32 U_CALLCONV
ucnv_MBCSGet_ਅਗਲਾ_UChar(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                  UError_ਕੋਡ *p_Error_ਕੋਡ);

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_MBCSGetStarters(ਅਚਲ UConverter* cnv,
                 UBool starters[256],
                 UError_ਕੋਡ *p_Error_ਕੋਡ);

U_CDECL_BEGIN
ਸਥਿਰ ਅਚਲ ਅੱਖਰ* U_CALLCONV
ucnv_MBCSGetName(ਅਚਲ UConverter *cnv);
U_CDECL_ਸਿਰਾ

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
              int32_t ਵਿੱਥ_ਸੂਚਕਾਂਕ,
              UError_ਕੋਡ *p_Error_ਕੋਡ);

ਸਥਿਰ UChar32 U_CALLCONV
ucnv_MBCSGet_ਅਗਲਾ_UChar(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                  UError_ਕੋਡ *p_Error_ਕੋਡ);

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_ਦਲੀਲਾਂ *pFromUArgs,
                  UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਮੰਜ਼ਿਲ_UArgs,
                  UError_ਕੋਡ *p_Error_ਕੋਡ);

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_MBCSGet_Unicode_ਸੈੱਟ(ਅਚਲ UConverter *cnv,
                   ਅਚਲ USetAdder *sa,
                   UConverter_Unicode_ਸੈੱਟ which,
                   UError_ਕੋਡ *p_Error_ਕੋਡ);

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_ਦਲੀਲਾਂ *pFromUArgs,
                  UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਮੰਜ਼ਿਲ_UArgs,
                  UError_ਕੋਡ *p_Error_ਕੋਡ);

ਸਥਿਰ ਅਚਲ UConverterImpl _SBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_ਅਗਲਾ_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_MBCSGet_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_SBCSFromUTF8
};

ਸਥਿਰ ਅਚਲ UConverterImpl _DBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_ਅਗਲਾ_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_MBCSGet_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_DBCSFromUTF8
};

ਸਥਿਰ ਅਚਲ UConverterImpl _MBCSImpl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_ਅਗਲਾ_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_MBCSGet_Unicode_ਸੈੱਟ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

/* Static data is in tools/makeconv/ucnvstat.c for data-based
 * converters. Be sure to update it as well.
 */

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _MBCSData={
    ਆਕਾਰ(UConverter_Shared_ਅੰਕੜੇ), 1,
    ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਝੂਠ, ਸੱਚ, &_MBCSImpl,
    0, UCNV_MBCS_ਸਾਰਣੀ_INITIALIZER
};


/* GB 18030 data ------------------------------------------------------------ */

/* helper macros for linear values for GB 18030 four-byte sequences */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰੇਖਿਕ_18030(a, b, c, d) ((((a)*10+(b))*126L+(c))*10L+(d))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰੇਖਿਕ_18030_ਅਧਾਰ ਰੇਖਿਕ_18030(0x81, 0x30, 0x81, 0x30)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰੇਖਿਕ(x) ਰੇਖਿਕ_18030(x>>24, (x>>16)&0xff, (x>>8)&0xff, x&0xff)

/*
 * Some ranges of GB 18030 where both the Unicode code points and the
 * GB four-byte sequences are contiguous and are handled algorithmically by
 * the special callback functions below.
 * The values are start & end of Unicode & GB codes.
 *
 * Note that single surrogates are not mapped by GB 18030
 * as of the re-released mapping tables from 2000-nov-30.
 */
ਸਥਿਰ ਅਚਲ uint32_t
gb18030Ranges[14][4]={
    {0x10000, 0x10FFFF, ਰੇਖਿਕ(0x90308130), ਰੇਖਿਕ(0xE3329A35)},
    {0x9FA6, 0xD7FF, ਰੇਖਿਕ(0x82358F33), ਰੇਖਿਕ(0x8336C738)},
    {0x0452, 0x1E3E, ਰੇਖਿਕ(0x8130D330), ਰੇਖਿਕ(0x8135F436)},
    {0x1E40, 0x200F, ਰੇਖਿਕ(0x8135F438), ਰੇਖਿਕ(0x8136A531)},
    {0xE865, 0xF92B, ਰੇਖਿਕ(0x8336D030), ਰੇਖਿਕ(0x84308534)},
    {0x2643, 0x2E80, ਰੇਖਿਕ(0x8137A839), ਰੇਖਿਕ(0x8138FD38)},
    {0xFA2A, 0xFE2F, ਰੇਖਿਕ(0x84309C38), ਰੇਖਿਕ(0x84318537)},
    {0x3CE1, 0x4055, ਰੇਖਿਕ(0x8231D438), ਰੇਖਿਕ(0x8232AF32)},
    {0x361B, 0x3917, ਰੇਖਿਕ(0x8230A633), ਰੇਖਿਕ(0x8230F237)},
    {0x49B8, 0x4C76, ਰੇਖਿਕ(0x8234A131), ਰੇਖਿਕ(0x8234E733)},
    {0x4160, 0x4336, ਰੇਖਿਕ(0x8232C937), ਰੇਖਿਕ(0x8232F837)},
    {0x478E, 0x4946, ਰੇਖਿਕ(0x8233E838), ਰੇਖਿਕ(0x82349638)},
    {0x44D7, 0x464B, ਰੇਖਿਕ(0x8233A339), ਰੇਖਿਕ(0x8233C931)},
    {0xFFE6, 0xFFFF, ਰੇਖਿਕ(0x8431A234), ਰੇਖਿਕ(0x8431A439)}
};

/* bit flag for UConverter.options indicating GB 18030 special handling */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _MBCS_OPTION_GB18030 0x8000

/* bit flag for UConverter.options indicating KEIS,JEF,JIF special handling */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _MBCS_OPTION_KEIS 0x01000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _MBCS_OPTION_JEF  0x02000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _MBCS_OPTION_JIPS 0x04000

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEIS_SO_CHAR_1 0x0A
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEIS_SO_CHAR_2 0x42
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEIS_SI_CHAR_1 0x0A
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEIS_SI_CHAR_2 0x41

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JEF_SO_CHAR 0x28
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JEF_SI_CHAR 0x29

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JIPS_SO_CHAR_1 0x1A
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JIPS_SO_CHAR_2 0x70
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JIPS_SI_CHAR_1 0x1A
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JIPS_SI_CHAR_2 0x71

ਗਿਣਤੀ SISO_Option {
    SI,
    SO
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ SISO_Option SISO_Option;

ਸਥਿਰ int32_t getSISOBytes(SISO_Option option, uint32_t cnvOption, uint8_t *value) {
    int32_t SISOLength = 0;

    ਚੋਣ (option) {
        ਹਾਲਤ SI:
            ਜੇ ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SI_CHAR_1;
                value[1] = KEIS_SI_CHAR_2;
                SISOLength = 2;
            } ਨਹੀਂਤਾਂ ਜੇ ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SI_CHAR;
                SISOLength = 1;
            } ਨਹੀਂਤਾਂ ਜੇ ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SI_CHAR_1;
                value[1] = JIPS_SI_CHAR_2;
                SISOLength = 2;
            } ਨਹੀਂਤਾਂ {
                value[0] = UCNV_SI;
                SISOLength = 1;
            }
            ਰੋਕੋ;
        ਹਾਲਤ SO:
            ਜੇ ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SO_CHAR_1;
                value[1] = KEIS_SO_CHAR_2;
                SISOLength = 2;
            } ਨਹੀਂਤਾਂ ਜੇ ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SO_CHAR;
                SISOLength = 1;
            } ਨਹੀਂਤਾਂ ਜੇ ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SO_CHAR_1;
                value[1] = JIPS_SO_CHAR_2;
                SISOLength = 2;
            } ਨਹੀਂਤਾਂ {
                value[0] = UCNV_SO;
                SISOLength = 1;
            }
            ਰੋਕੋ;
        ਮੂਲ:
            /* Should never happen. */
            ਰੋਕੋ;
    }

    ਵਾਪਸ SISOLength;
}

/* Miscellaneous ------------------------------------------------------------ */

/* similar to ucnv_MBCSGetNextUChar() but recursive */
ਸਥਿਰ UBool
enum_ਮੰਜ਼ਿਲ_U(UConverterMBCSTable *mbcs_ਸਾਰਣੀ, int8_t stateProps[],
        int32_t state, uint32_t ਵਿੱਥ,
        uint32_t value,
        UConverter_Enum_ਮੰਜ਼ਿਲ_UCallback *callback, ਅਚਲ ਖਾਲੀ *context,
        UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UChar32 ਕੋਡ_Points[32];
    ਅਚਲ int32_t *ਪੰਕਤੀ;
    ਅਚਲ uint16_t *unicode_ਕੋਡ_Units;
    UChar32 any_ਕੋਡ_Points;
    int32_t b, ਹੱਦਬੰਦੀ;

    ਪੰਕਤੀ=mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ[state];
    unicode_ਕੋਡ_Units=mbcs_ਸਾਰਣੀ->unicode_ਕੋਡ_Units;

    value<<=8;
    any_ਕੋਡ_Points=-1;  /* becomes non-negative if there is a mapping */

    b=(stateProps[state]&0x38)<<2;
    ਜੇ(b==0 && stateProps[state]>=0x40) {
        /* skip byte sequences with leading zeros because they are not stored in the fromUnicode table */
        ਕੋਡ_Points[0]=U_SENTINEL;
        b=1;
    }
    ਹੱਦਬੰਦੀ=((stateProps[state]&7)+1)<<5;
    ਜਦੋਂ(b<ਹੱਦਬੰਦੀ) {
        int32_t ਇੰਦਰਾਜ=ਪੰਕਤੀ[b];
        ਜੇ(MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(ਇੰਦਰਾਜ)) {
            int32_t ਅਗਲਾ_State=MBCS_ਇੰਦਰਾਜ_TRANSITION_STATE(ਇੰਦਰਾਜ);
            ਜੇ(stateProps[ਅਗਲਾ_State]>=0) {
                /* recurse to a state with non-ignorable actions */
                ਜੇ(!enum_ਮੰਜ਼ਿਲ_U(
                        mbcs_ਸਾਰਣੀ, stateProps, ਅਗਲਾ_State,
                        ਵਿੱਥ+MBCS_ਇੰਦਰਾਜ_TRANSITION_ਵਿੱਥ(ਇੰਦਰਾਜ),
                        value | static_cast<uint32_t>(b),
                        callback, context,
                        p_Error_ਕੋਡ)) {
                    ਵਾਪਸ ਝੂਠ;
                }
            }
            ਕੋਡ_Points[b&0x1f]=U_SENTINEL;
        } ਨਹੀਂਤਾਂ {
            UChar32 c;
            int32_t action;

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ);
            ਜੇ(action==MBCS_STATE_VALID_ਸਿੱਧਾ_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_VALID_16) {
                int32_t final_ਵਿੱਥ=ਵਿੱਥ+MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
                c=unicode_ਕੋਡ_Units[final_ਵਿੱਥ];
                ਜੇ(c<0xfffe) {
                    /* output BMP code point */
                } ਨਹੀਂਤਾਂ {
                    c=U_SENTINEL;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_VALID_16_PAIR) {
                int32_t final_ਵਿੱਥ=ਵਿੱਥ+MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
                c=unicode_ਕੋਡ_Units[final_ਵਿੱਥ++];
                ਜੇ(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } ਨਹੀਂਤਾਂ ਜੇ(c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_ਕੋਡ_Units[final_ਵਿੱਥ]+(0x10000-0xdc00);
                } ਨਹੀਂਤਾਂ ਜੇ(c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_ਕੋਡ_Units[final_ਵਿੱਥ];
                } ਨਹੀਂਤਾਂ {
                    c=U_SENTINEL;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_VALID_ਸਿੱਧਾ_20) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ(ਇੰਦਰਾਜ) + 0x10000);
            } ਨਹੀਂਤਾਂ {
                c=U_SENTINEL;
            }

            ਕੋਡ_Points[b&0x1f]=c;
            any_ਕੋਡ_Points&=c;
        }
        ਜੇ(((++b)&0x1f)==0) {
            ਜੇ(any_ਕੋਡ_Points>=0) {
                ਜੇ (!callback(context, value | static_cast<uint32_t>(b - 0x20), ਕੋਡ_Points)) {
                    ਵਾਪਸ ਝੂਠ;
                }
                any_ਕੋਡ_Points=-1;
            }
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

/*
 * Only called if stateProps[state]==-1.
 * A recursive call may do stateProps[state]|=0x40 if this state is the target of an
 * MBCS_STATE_CHANGE_ONLY.
 */
ਸਥਿਰ int8_t
getStateProp(ਅਚਲ int32_t (*state_ਸਾਰਣੀ)[256], int8_t stateProps[], ਪੂਰਨ state) {
    ਅਚਲ int32_t *ਪੰਕਤੀ;
    int32_t min, max, ਇੰਦਰਾਜ, ਅਗਲਾ_State;

    ਪੰਕਤੀ=state_ਸਾਰਣੀ[state];
    stateProps[state]=0;

    /* find first non-ignorable state */
    ਲਈ(min=0;; ++min) {
        ਇੰਦਰਾਜ=ਪੰਕਤੀ[min];
        ਅਗਲਾ_State=MBCS_ਇੰਦਰਾਜ_STATE(ਇੰਦਰਾਜ);
        ਜੇ(stateProps[ਅਗਲਾ_State]==-1) {
            getStateProp(state_ਸਾਰਣੀ, stateProps, ਅਗਲਾ_State);
        }
        ਜੇ(MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(ਇੰਦਰਾਜ)) {
            ਜੇ(stateProps[ਅਗਲਾ_State]>=0) {
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ)<MBCS_STATE_UNASSIGNED) {
            ਰੋਕੋ;
        }
        ਜੇ(min==0xff) {
            stateProps[state]=-0x40;  /* (int8_t)0xc0 */
            ਵਾਪਸ stateProps[state];
        }
    }
    stateProps[state] |= static_cast<int8_t>((min >> 5) << 3);

    /* find last non-ignorable state */
    ਲਈ(max=0xff; min<max; --max) {
        ਇੰਦਰਾਜ=ਪੰਕਤੀ[max];
        ਅਗਲਾ_State=MBCS_ਇੰਦਰਾਜ_STATE(ਇੰਦਰਾਜ);
        ਜੇ(stateProps[ਅਗਲਾ_State]==-1) {
            getStateProp(state_ਸਾਰਣੀ, stateProps, ਅਗਲਾ_State);
        }
        ਜੇ(MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(ਇੰਦਰਾਜ)) {
            ਜੇ(stateProps[ਅਗਲਾ_State]>=0) {
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ)<MBCS_STATE_UNASSIGNED) {
            ਰੋਕੋ;
        }
    }
    stateProps[state] |= static_cast<int8_t>(max >> 5);

    /* recurse further and collect direct-state information */
    ਜਦੋਂ(min<=max) {
        ਇੰਦਰਾਜ=ਪੰਕਤੀ[min];
        ਅਗਲਾ_State=MBCS_ਇੰਦਰਾਜ_STATE(ਇੰਦਰਾਜ);
        ਜੇ(stateProps[ਅਗਲਾ_State]==-1) {
            getStateProp(state_ਸਾਰਣੀ, stateProps, ਅਗਲਾ_State);
        }
        ਜੇ(MBCS_ਇੰਦਰਾਜ_IS_FINAL(ਇੰਦਰਾਜ)) {
            stateProps[ਅਗਲਾ_State]|=0x40;
            ਜੇ(MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ)<=MBCS_STATE_FALLBACK_ਸਿੱਧਾ_20) {
                stateProps[state]|=0x40;
            }
        }
        ++min;
    }
    ਵਾਪਸ stateProps[state];
}

/*
 * Internal function enumerating the toUnicode data of an MBCS converter.
 * Currently only used for reconstituting data for a MBCS_OPT_NO_FROM_U
 * table, but could also be used for a future ucnv_getUnicodeSet() option
 * that includes reverse fallbacks (after updating this function's implementation).
 * Currently only handles roundtrip mappings.
 * Does not currently handle extensions.
 */
ਸਥਿਰ ਖਾਲੀ
ucnv_MBCSEnum_ਮੰਜ਼ਿਲ_Unicode(UConverterMBCSTable *mbcs_ਸਾਰਣੀ,
                       UConverter_Enum_ਮੰਜ਼ਿਲ_UCallback *callback, ਅਚਲ ਖਾਲੀ *context,
                       UError_ਕੋਡ *p_Error_ਕੋਡ) {
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
    int8_t stateProps[MBCS_MAX_STATE_ਗਿਣਤੀ];
    int32_t state;

    uprv_memset(stateProps, -1, ਆਕਾਰ(stateProps));

    /* recurse from state 0 and set all stateProps */
    getStateProp(mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ, stateProps, 0);

    ਲਈ(state=0; state<mbcs_ਸਾਰਣੀ->ਗਿਣਤੀ_ਹਾਲਤਾਂ; ++state) {
        /*if(stateProps[state]==-1) {
            printf("unused/unreachable <icu:state> %d\n", state);
        }*/
        ਜੇ(stateProps[state]>=0x40) {
            /* start from each direct state */
            enum_ਮੰਜ਼ਿਲ_U(
                mbcs_ਸਾਰਣੀ, stateProps, state, 0, 0,
                callback, context,
                p_Error_ਕੋਡ);
        }
    }
}

U_CFUNC ਖਾਲੀ 
ucnv_MBCSGet_Filtered_Unicode_ਸੈੱਟ_For_Unicode(ਅਚਲ UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ,
                                         ਅਚਲ USetAdder *sa,
                                         UConverter_Unicode_ਸੈੱਟ which,
                                         UConverter_ਸੈੱਟ_Filter filter,
                                         UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UConverterMBCSTable *mbcs_ਸਾਰਣੀ;
    ਅਚਲ uint16_t *ਸਾਰਣੀ;

    uint32_t st3;
    uint16_t st1, maxStage1, st2;

    UChar32 c;

    /* enumerate the from-Unicode trie table */
    mbcs_ਸਾਰਣੀ=&shared_ਅੰਕੜੇ->mbcs;
    ਸਾਰਣੀ=mbcs_ਸਾਰਣੀ->from_Unicode_ਸਾਰਣੀ;
    ਜੇ(mbcs_ਸਾਰਣੀ->unicode_ਨਕਾਬ&UCNV_HAS_SUPPLEMENTARY) {
        maxStage1=0x440;
    } ਨਹੀਂਤਾਂ {
        maxStage1=0x40;
    }

    c=0; /* keep track of the current code point while enumerating */

    ਜੇ(mbcs_ਸਾਰਣੀ->outputType==MBCS_OUTPUT_1) {
        ਅਚਲ uint16_t *stage2, *stage3, *results;
        uint16_t min_ਮੁੱਲ;

        results=(ਅਚਲ uint16_t *)mbcs_ਸਾਰਣੀ->from_Unicode_ਬਾਈਟਾਂ;

        /*
         * Set a threshold variable for selecting which mappings to use.
         * See ucnv_MBCSSingleFromBMPWithOffsets() and
         * MBCS_SINGLE_RESULT_FROM_U() for details.
         */
        ਜੇ(which==UCNV_ROUNDTRIP_ਸੈੱਟ) {
            /* use only roundtrips */
            min_ਮੁੱਲ=0xf00;
        } ਨਹੀਂਤਾਂ /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
            /* use all roundtrip and fallback results */
            min_ਮੁੱਲ=0x800;
        }

        ਲਈ(st1=0; st1<maxStage1; ++st1) {
            st2=ਸਾਰਣੀ[st1];
            ਜੇ(st2>maxStage1) {
                stage2=ਸਾਰਣੀ+st2;
                ਲਈ(st2=0; st2<64; ++st2) {
                    ਜੇ((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=results+st3;

                        ਕਰੋ {
                            ਜੇ(*stage3++>=min_ਮੁੱਲ) {
                                sa->ਜੋੜੋ(sa->ਸੈੱਟ, c);
                            }
                        } ਜਦੋਂ((++c&0xf)!=0);
                    } ਨਹੀਂਤਾਂ {
                        c+=16; /* empty stage 3 block */
                    }
                }
            } ਨਹੀਂਤਾਂ {
                c+=1024; /* empty stage 2 block */
            }
        }
    } ਨਹੀਂਤਾਂ {
        ਅਚਲ uint32_t *stage2;
        ਅਚਲ uint8_t *stage3, *ਬਾਈਟਾਂ;
        uint32_t st3Multiplier;
        uint32_t value;
        UBool useFallback;

        ਬਾਈਟਾਂ=mbcs_ਸਾਰਣੀ->from_Unicode_ਬਾਈਟਾਂ;

        useFallback = which == UCNV_ROUNDTRIP_AND_FALLBACK_ਸੈੱਟ;

        ਚੋਣ(mbcs_ਸਾਰਣੀ->outputType) {
        ਹਾਲਤ MBCS_OUTPUT_3:
        ਹਾਲਤ MBCS_OUTPUT_4_EUC:
            st3Multiplier=3;
            ਰੋਕੋ;
        ਹਾਲਤ MBCS_OUTPUT_4:
            st3Multiplier=4;
            ਰੋਕੋ;
        ਮੂਲ:
            st3Multiplier=2;
            ਰੋਕੋ;
        }

        ਲਈ(st1=0; st1<maxStage1; ++st1) {
            st2=ਸਾਰਣੀ[st1];
            ਜੇ(st2>(maxStage1>>1)) {
                stage2=(ਅਚਲ uint32_t *)ਸਾਰਣੀ+st2;
                ਲਈ(st2=0; st2<64; ++st2) {
                    ਜੇ((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=ਬਾਈਟਾਂ+st3Multiplier*16*(uint32_t)(uint16_t)st3;

                        /* get the roundtrip flags for the stage 3 block */
                        st3>>=16;

                        /*
                         * Add code points for which the roundtrip flag is set,
                         * or which map to non-zero bytes if we use fallbacks.
                         * See ucnv_MBCSFromUnicodeWithOffsets() for details.
                         */
                        ਚੋਣ(filter) {
                        ਹਾਲਤ UCNV_ਸੈੱਟ_FILTER_NONE:
                            ਕਰੋ {
                                ਜੇ(st3&1) {
                                    sa->ਜੋੜੋ(sa->ਸੈੱਟ, c);
                                    stage3+=st3Multiplier;
                                } ਨਹੀਂਤਾਂ ਜੇ(useFallback) {
                                    uint8_t b=0;
                                    ਚੋਣ(st3Multiplier) {
                                    ਹਾਲਤ 4:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    ਹਾਲਤ 3:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    ਹਾਲਤ 2:
                                        b|=stage3[0]|stage3[1];
                                        stage3+=2;
                                        U_FALLTHROUGH;
                                    ਮੂਲ:
                                        ਰੋਕੋ;
                                    }
                                    ਜੇ(b!=0) {
                                        sa->ਜੋੜੋ(sa->ਸੈੱਟ, c);
                                    }
                                }
                                st3>>=1;
                            } ਜਦੋਂ((++c&0xf)!=0);
                            ਰੋਕੋ;
                        ਹਾਲਤ UCNV_ਸੈੱਟ_FILTER_DBCS_ONLY:
                             /* Ignore single-byte results (<0x100). */
                            ਕਰੋ {
                                ਜੇ(((st3&1)!=0 || useFallback) && *((ਅਚਲ uint16_t *)stage3)>=0x100) {
                                    sa->ਜੋੜੋ(sa->ਸੈੱਟ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } ਜਦੋਂ((++c&0xf)!=0);
                            ਰੋਕੋ;
                        ਹਾਲਤ UCNV_ਸੈੱਟ_FILTER_2022_CN:
                             /* Only add code points that map to CNS 11643 planes 1 & 2 for non-EXT ISO-2022-CN. */
                            ਕਰੋ {
                                ਜੇ(((st3&1)!=0 || useFallback) && ((value=*stage3)==0x81 || value==0x82)) {
                                    sa->ਜੋੜੋ(sa->ਸੈੱਟ, c);
                                }
                                st3>>=1;
                                stage3+=3;  /* +=st3Multiplier */
                            } ਜਦੋਂ((++c&0xf)!=0);
                            ਰੋਕੋ;
                        ਹਾਲਤ UCNV_ਸੈੱਟ_FILTER_SJIS:
                             /* Only add code points that map to Shift-JIS codes corresponding to JIS X 0208. */
                            ਕਰੋ {
                                ਜੇ(((st3&1)!=0 || useFallback) && (value=*((ਅਚਲ uint16_t *)stage3))>=0x8140 && value<=0xeffc) {
                                    sa->ਜੋੜੋ(sa->ਸੈੱਟ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } ਜਦੋਂ((++c&0xf)!=0);
                            ਰੋਕੋ;
                        ਹਾਲਤ UCNV_ਸੈੱਟ_FILTER_GR94_DBCS:
                            /* Only add code points that map to ISO 2022 GR 94 DBCS codes (each byte A1..FE). */
                            ਕਰੋ {
                                ਜੇ( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((ਅਚਲ uint16_t *)stage3)) - 0xa1a1)<=(0xfefe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->ਜੋੜੋ(sa->ਸੈੱਟ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } ਜਦੋਂ((++c&0xf)!=0);
                            ਰੋਕੋ;
                        ਹਾਲਤ UCNV_ਸੈੱਟ_FILTER_HZ:
                            /* Only add code points that are suitable for HZ DBCS (lead byte A1..FD). */
                            ਕਰੋ {
                                ਜੇ( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((ਅਚਲ uint16_t *)stage3))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->ਜੋੜੋ(sa->ਸੈੱਟ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } ਜਦੋਂ((++c&0xf)!=0);
                            ਰੋਕੋ;
                        ਮੂਲ:
                            *p_Error_ਕੋਡ=U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
                            ਵਾਪਸ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        c+=16; /* empty stage 3 block */
                    }
                }
            } ਨਹੀਂਤਾਂ {
                c+=1024; /* empty stage 2 block */
            }
        }
    }

    ucnv_ext_Get_Unicode_ਸੈੱਟ(shared_ਅੰਕੜੇ, sa, which, filter, p_Error_ਕੋਡ);
}

U_CFUNC ਖਾਲੀ
ucnv_MBCSGet_Unicode_ਸੈੱਟ_For_Unicode(ਅਚਲ UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ,
                                 ਅਚਲ USetAdder *sa,
                                 UConverter_Unicode_ਸੈੱਟ which,
                                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ucnv_MBCSGet_Filtered_Unicode_ਸੈੱਟ_For_Unicode(
        shared_ਅੰਕੜੇ, sa, which,
        shared_ਅੰਕੜੇ->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ?
            UCNV_ਸੈੱਟ_FILTER_DBCS_ONLY :
            UCNV_ਸੈੱਟ_FILTER_NONE,
        p_Error_ਕੋਡ);
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_MBCSGet_Unicode_ਸੈੱਟ(ਅਚਲ UConverter *cnv,
                   ਅਚਲ USetAdder *sa,
                   UConverter_Unicode_ਸੈੱਟ which,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(cnv->options&_MBCS_OPTION_GB18030) {
        sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0, 0xd7ff);
        sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0xe000, 0x10ffff);
    } ਨਹੀਂਤਾਂ {
        ucnv_MBCSGet_Unicode_ਸੈੱਟ_For_Unicode(cnv->shared_ਅੰਕੜੇ, sa, which, p_Error_ਕੋਡ);
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
ਸਥਿਰ UChar32
_extFromU(UConverter *cnv, ਅਚਲ UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ,
          UChar32 cp,
          ਅਚਲ char16_t **ਸਰੋਤ, ਅਚਲ char16_t *ਸਰੋਤ_ਹੱਦਬੰਦੀ,
          uint8_t **ਨਿਸ਼ਾਨਾ, ਅਚਲ uint8_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
          int32_t **offsets, int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ,
          UBool flush,
          UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ int32_t *cx;

    cnv->useSubChar1=ਝੂਠ;

    ਜੇ( (cx=shared_ਅੰਕੜੇ->mbcs.extIndexes)!=ਖਾਲੀ_ਸੂਚਕ &&
        ucnv_ext_Initial_ਮੇਲ_From_U(
            cnv, cx,
            cp, ਸਰੋਤ, ਸਰੋਤ_ਹੱਦਬੰਦੀ,
            reinterpret_cast<ਅੱਖਰ**>(ਨਿਸ਼ਾਨਾ), reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ),
            offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
            flush,
            p_Error_ਕੋਡ)
    ) {
        ਵਾਪਸ 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    ਜੇ((cnv->options&_MBCS_OPTION_GB18030)!=0) {
        ਅਚਲ uint32_t *range;
        int32_t i;

        range=gb18030Ranges[0];
        ਲਈ(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            ਜੇ (range[0] <= static_cast<uint32_t>(cp) && static_cast<uint32_t>(cp) <= range[1]) {
                /* found the Unicode code point, output the four-byte sequence for it */
                uint32_t ਰੇਖਿਕ_2;
                ਅੱਖਰ ਬਾਈਟਾਂ[4];

                /* get the linear value of the first GB 18030 code in this range */
                ਰੇਖਿਕ_2=range[2]-ਰੇਖਿਕ_18030_ਅਧਾਰ;

                /* add the offset from the beginning of the range */
                ਰੇਖਿਕ_2 += (static_cast<uint32_t>(cp) - range[0]);

                /* turn this into a four-byte sequence */
                ਬਾਈਟਾਂ[3] = static_cast<ਅੱਖਰ>(0x30 + ਰੇਖਿਕ_2 % 10); ਰੇਖਿਕ_2 /= 10;
                ਬਾਈਟਾਂ[2] = static_cast<ਅੱਖਰ>(0x81 + ਰੇਖਿਕ_2 % 126); ਰੇਖਿਕ_2 /= 126;
                ਬਾਈਟਾਂ[1] = static_cast<ਅੱਖਰ>(0x30 + ਰੇਖਿਕ_2 % 10); ਰੇਖਿਕ_2 /= 10;
                ਬਾਈਟਾਂ[0] = static_cast<ਅੱਖਰ>(0x81 + ਰੇਖਿਕ_2);

                /* output this sequence */
                ucnv_from_UWrite_ਬਾਈਟਾਂ(cnv,
                                     ਬਾਈਟਾਂ, 4, reinterpret_cast<ਅੱਖਰ**>(ਨਿਸ਼ਾਨਾ), reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ),
                                     offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ, p_Error_ਕੋਡ);
                ਵਾਪਸ 0;
            }
        }
    }

    /* no mapping */
    *p_Error_ਕੋਡ=U_INVALID_CHAR_ਮਿਲਿਆ;
    ਵਾਪਸ cp;
}

/*
 * Input sequence: cnv->toUBytes[0..length[
 * @return if(U_FAILURE) return the length (toULength, byteIndex) for the input
 *         else return 0 after output has been written to the target
 */
ਸਥਿਰ int8_t
ext_ਮੰਜ਼ਿਲ_U(UConverter *cnv, ਅਚਲ UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ,
        int8_t length,
        ਅਚਲ uint8_t **ਸਰੋਤ, ਅਚਲ uint8_t *ਸਰੋਤ_ਹੱਦਬੰਦੀ,
        char16_t **ਨਿਸ਼ਾਨਾ, ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
        int32_t **offsets, int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ,
        UBool flush,
        UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ int32_t *cx;

    ਜੇ( (cx=shared_ਅੰਕੜੇ->mbcs.extIndexes)!=ਖਾਲੀ_ਸੂਚਕ &&
        ucnv_ext_Initial_ਮੇਲ_ਮੰਜ਼ਿਲ_U(
            cnv, cx,
            length, reinterpret_cast<ਅਚਲ ਅੱਖਰ**>(ਸਰੋਤ), reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਸਰੋਤ_ਹੱਦਬੰਦੀ),
            ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
            offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
            flush,
            p_Error_ਕੋਡ)
    ) {
        ਵਾਪਸ 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    ਜੇ(length==4 && (cnv->options&_MBCS_OPTION_GB18030)!=0) {
        ਅਚਲ uint32_t *range;
        uint32_t ਰੇਖਿਕ_2;
        int32_t i;

        ਰੇਖਿਕ_2=ਰੇਖਿਕ_18030(cnv->ਮੰਜ਼ਿਲ_UBytes[0], cnv->ਮੰਜ਼ਿਲ_UBytes[1], cnv->ਮੰਜ਼ਿਲ_UBytes[2], cnv->ਮੰਜ਼ਿਲ_UBytes[3]);
        range=gb18030Ranges[0];
        ਲਈ(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            ਜੇ(range[2]<=ਰੇਖਿਕ_2 && ਰੇਖਿਕ_2<=range[3]) {
                /* found the sequence, output the Unicode code point for it */
                *p_Error_ਕੋਡ=U_ZERO_ERROR;

                /* add the linear difference between the input and start sequences to the start code point */
                ਰੇਖਿਕ_2=range[0]+(ਰੇਖਿਕ_2-range[2]);

                /* output this code point */
                ucnv_ਮੰਜ਼ਿਲ_UWrite_ਕੋਡ_ਬਿੰਦੂ(cnv, ਰੇਖਿਕ_2, ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ, offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ, p_Error_ਕੋਡ);

                ਵਾਪਸ 0;
            }
        }
    }

    /* no mapping */
    *p_Error_ਕੋਡ=U_INVALID_CHAR_ਮਿਲਿਆ;
    ਵਾਪਸ length;
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EBCDIC_LF 0x25
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EBCDIC_NL 0x15

/* standard EBCDIC codes with roundtrip flag as stored in Unicode-to-single-byte tables */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EBCDIC_RT_LF 0xf25
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EBCDIC_RT_NL 0xf15

/* Unicode code points */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_LF 0x0a
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_NL 0x85

ਸਥਿਰ UBool
_EBCDICSwapLFNL(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverterMBCSTable *mbcs_ਸਾਰਣੀ;

    ਅਚਲ uint16_t *ਸਾਰਣੀ, *results;
    ਅਚਲ uint8_t *ਬਾਈਟਾਂ;

    int32_t (*new_State_ਸਾਰਣੀ)[256];
    uint16_t *newResults;
    uint8_t *p;
    ਅੱਖਰ *name;

    uint32_t stage2_ਇੰਦਰਾਜ;
    uint32_t size, sizeofFromUBytes;

    mbcs_ਸਾਰਣੀ=&shared_ਅੰਕੜੇ->mbcs;

    ਸਾਰਣੀ=mbcs_ਸਾਰਣੀ->from_Unicode_ਸਾਰਣੀ;
    ਬਾਈਟਾਂ=mbcs_ਸਾਰਣੀ->from_Unicode_ਬਾਈਟਾਂ;
    results = reinterpret_cast<ਅਚਲ uint16_t*>(ਬਾਈਟਾਂ);

    /*
     * Check that this is an EBCDIC table with SBCS portion -
     * SBCS or EBCDIC_STATEFUL with standard EBCDIC LF and NL mappings.
     *
     * If not, ignore the option. Options are always ignored if they do not apply.
     */
    ਜੇ(!(
         (mbcs_ਸਾਰਣੀ->outputType==MBCS_OUTPUT_1 || mbcs_ਸਾਰਣੀ->outputType==MBCS_OUTPUT_2_SISO) &&
         mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ[0][EBCDIC_LF]==MBCS_ਇੰਦਰਾਜ_FINAL(0, MBCS_STATE_VALID_ਸਿੱਧਾ_16, U_LF) &&
         mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ[0][EBCDIC_NL]==MBCS_ਇੰਦਰਾਜ_FINAL(0, MBCS_STATE_VALID_ਸਿੱਧਾ_16, U_NL)
    )) {
        ਵਾਪਸ ਝੂਠ;
    }

    ਜੇ(mbcs_ਸਾਰਣੀ->outputType==MBCS_OUTPUT_1) {
        ਜੇ(!(
             EBCDIC_RT_LF==MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, results, U_LF) &&
             EBCDIC_RT_NL==MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, results, U_NL)
        )) {
            ਵਾਪਸ ਝੂਠ;
        }
    } ਨਹੀਂਤਾਂ /* MBCS_OUTPUT_2_SISO */ {
        stage2_ਇੰਦਰਾਜ=MBCS_STAGE_2_FROM_U(ਸਾਰਣੀ, U_LF);
        ਜੇ(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_ਇੰਦਰਾਜ, U_LF)!=0 &&
             EBCDIC_LF==MBCS_ਮੁੱਲ_2_FROM_STAGE_2(ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, U_LF)
        )) {
            ਵਾਪਸ ਝੂਠ;
        }

        stage2_ਇੰਦਰਾਜ=MBCS_STAGE_2_FROM_U(ਸਾਰਣੀ, U_NL);
        ਜੇ(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_ਇੰਦਰਾਜ, U_NL)!=0 &&
             EBCDIC_NL==MBCS_ਮੁੱਲ_2_FROM_STAGE_2(ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, U_NL)
        )) {
            ਵਾਪਸ ਝੂਠ;
        }
    }

    ਜੇ(mbcs_ਸਾਰਣੀ->from_UBytes_ਲੰਮਾਈ>0) {
        /*
         * We _know_ the number of bytes in the fromUnicodeBytes array
         * starting with header.version 4.1.
         */
        sizeofFromUBytes=mbcs_ਸਾਰਣੀ->from_UBytes_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
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
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }

    /*
     * The table has an appropriate format.
     * Allocate and build
     * - a modified to-Unicode state table
     * - a modified from-Unicode output array
     * - a converter name string with the swap option appended
     */
    size=
        mbcs_ਸਾਰਣੀ->ਗਿਣਤੀ_ਹਾਲਤਾਂ*1024+
        sizeofFromUBytes+
        UCNV_MAX_CONVERTER_NAME_ਲੰਮਾਈ+20;
    p = static_cast<uint8_t*>(uprv_malloc(size));
    ਜੇ(p==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }

    /* copy and modify the to-Unicode state table */
    new_State_ਸਾਰਣੀ = reinterpret_cast<int32_t(*)[256]>(p);
    uprv_memcpy(new_State_ਸਾਰਣੀ, mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ, mbcs_ਸਾਰਣੀ->ਗਿਣਤੀ_ਹਾਲਤਾਂ*1024);

    new_State_ਸਾਰਣੀ[0][EBCDIC_LF]=MBCS_ਇੰਦਰਾਜ_FINAL(0, MBCS_STATE_VALID_ਸਿੱਧਾ_16, U_NL);
    new_State_ਸਾਰਣੀ[0][EBCDIC_NL]=MBCS_ਇੰਦਰਾਜ_FINAL(0, MBCS_STATE_VALID_ਸਿੱਧਾ_16, U_LF);

    /* copy and modify the from-Unicode result table */
    newResults = reinterpret_cast<uint16_t*>(new_State_ਸਾਰਣੀ[mbcs_ਸਾਰਣੀ->ਗਿਣਤੀ_ਹਾਲਤਾਂ]);
    uprv_memcpy(newResults, ਬਾਈਟਾਂ, sizeofFromUBytes);

    /* conveniently, the table access macros work on the left side of expressions */
    ਜੇ(mbcs_ਸਾਰਣੀ->outputType==MBCS_OUTPUT_1) {
        MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, newResults, U_LF)=EBCDIC_RT_NL;
        MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, newResults, U_NL)=EBCDIC_RT_LF;
    } ਨਹੀਂਤਾਂ /* MBCS_OUTPUT_2_SISO */ {
        stage2_ਇੰਦਰਾਜ=MBCS_STAGE_2_FROM_U(ਸਾਰਣੀ, U_LF);
        MBCS_ਮੁੱਲ_2_FROM_STAGE_2(newResults, stage2_ਇੰਦਰਾਜ, U_LF)=EBCDIC_NL;

        stage2_ਇੰਦਰਾਜ=MBCS_STAGE_2_FROM_U(ਸਾਰਣੀ, U_NL);
        MBCS_ਮੁੱਲ_2_FROM_STAGE_2(newResults, stage2_ਇੰਦਰਾਜ, U_NL)=EBCDIC_LF;
    }

    /* set the canonical converter name */
    name = reinterpret_cast<ਅੱਖਰ*>(newResults) + sizeofFromUBytes;
    uprv_strcpy(name, shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->name);
    uprv_strcat(name, UCNV_ਅਦਲਾ_ਬਦਲੀ_LFNL_OPTION_STRING);

    /* set the pointers */
    icu::umtx_ਤਾਲਾ(ਖਾਲੀ_ਸੂਚਕ);
    ਜੇ(mbcs_ਸਾਰਣੀ->ਅਦਲਾ_ਬਦਲੀ_LFNLState_ਸਾਰਣੀ==ਖਾਲੀ_ਸੂਚਕ) {
        mbcs_ਸਾਰਣੀ->ਅਦਲਾ_ਬਦਲੀ_LFNLState_ਸਾਰਣੀ=new_State_ਸਾਰਣੀ;
        mbcs_ਸਾਰਣੀ->ਅਦਲਾ_ਬਦਲੀ_LFNLFrom_Unicode_ਬਾਈਟਾਂ = reinterpret_cast<uint8_t*>(newResults);
        mbcs_ਸਾਰਣੀ->ਅਦਲਾ_ਬਦਲੀ_LFNLName=name;

        new_State_ਸਾਰਣੀ=ਖਾਲੀ_ਸੂਚਕ;
    }
    icu::umtx_unlock(ਖਾਲੀ_ਸੂਚਕ);

    /* release the allocated memory if another thread beat us to it */
    ਜੇ(new_State_ਸਾਰਣੀ!=ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(new_State_ਸਾਰਣੀ);
    }
    ਵਾਪਸ ਸੱਚ;
}

/* reconstitute omitted fromUnicode data ------------------------------------ */

/* for details, compare with genmbcs.c MBCSAddFromUnicode() and transformEUC() */
ਸਥਿਰ UBool U_CALLCONV
writeStage3Roundtrip(ਅਚਲ ਖਾਲੀ *context, uint32_t value, UChar32 ਕੋਡ_Points[32]) {
    UConverterMBCSTable *mbcs_ਸਾਰਣੀ=(UConverterMBCSTable *)context;
    ਅਚਲ uint16_t *ਸਾਰਣੀ;
    uint32_t *stage2;
    uint8_t *ਬਾਈਟਾਂ, *p;
    UChar32 c;
    int32_t i, st3;

    ਸਾਰਣੀ=mbcs_ਸਾਰਣੀ->from_Unicode_ਸਾਰਣੀ;
    ਬਾਈਟਾਂ = const_cast<uint8_t*>(mbcs_ਸਾਰਣੀ->from_Unicode_ਬਾਈਟਾਂ);

    /* for EUC outputTypes, modify the value like genmbcs.c's transformEUC() */
    ਚੋਣ(mbcs_ਸਾਰਣੀ->outputType) {
    ਹਾਲਤ MBCS_OUTPUT_3_EUC:
        ਜੇ(value<=0xffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } ਨਹੀਂਤਾਂ ਜੇ(value<=0x8effff) {
            /* code set 2 */
            value&=0x7fff;
        } ਨਹੀਂਤਾਂ /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7f;
        }
        ਰੋਕੋ;
    ਹਾਲਤ MBCS_OUTPUT_4_EUC:
        ਜੇ(value<=0xffffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } ਨਹੀਂਤਾਂ ਜੇ(value<=0x8effffff) {
            /* code set 2 */
            value&=0x7fffff;
        } ਨਹੀਂਤਾਂ /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7fff;
        }
        ਰੋਕੋ;
    ਮੂਲ:
        ਰੋਕੋ;
    }

    ਲਈ(i=0; i<=0x1f; ++value, ++i) {
        c=ਕੋਡ_Points[i];
        ਜੇ(c<0) {
            ਜਾਰੀ;
        }

        /* locate the stage 2 & 3 data */
        stage2=((uint32_t *)ਸਾਰਣੀ)+ਸਾਰਣੀ[c>>10]+((c>>4)&0x3f);
        p=ਬਾਈਟਾਂ;
        st3 = static_cast<int32_t>(static_cast<uint16_t>(*stage2)) * 16 + (c & 0xf);

        /* write the codepage bytes into stage 3 */
        ਚੋਣ(mbcs_ਸਾਰਣੀ->outputType) {
        ਹਾਲਤ MBCS_OUTPUT_3:
        ਹਾਲਤ MBCS_OUTPUT_4_EUC:
            p+=st3*3;
            p[0] = static_cast<uint8_t>(value >> 16);
            p[1] = static_cast<uint8_t>(value >> 8);
            p[2] = static_cast<uint8_t>(value);
            ਰੋਕੋ;
        ਹਾਲਤ MBCS_OUTPUT_4:
            reinterpret_cast<uint32_t*>(p)[st3] = value;
            ਰੋਕੋ;
        ਮੂਲ:
            /* 2 bytes per character */
            reinterpret_cast<uint16_t*>(p)[st3] = static_cast<uint16_t>(value);
            ਰੋਕੋ;
        }

        /* set the roundtrip flag */
        *stage2|=(1UL<<(16+(c&0xf)));
    }
    ਵਾਪਸ ਸੱਚ;
 }

ਸਥਿਰ ਖਾਲੀ
reconstitute_ਅੰਕੜੇ(UConverterMBCSTable *mbcs_ਸਾਰਣੀ,
                 uint32_t stage1_ਲੰਮਾਈ, uint32_t stage2_ਲੰਮਾਈ,
                 uint32_t ਭਰਿਆ_Stage2_ਲੰਮਾਈ,  /* lengths are numbers of units, not bytes */
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    uint16_t *stage1;
    uint32_t *stage2;
    uint32_t ਅੰਕੜੇ_ਲੰਮਾਈ=stage1_ਲੰਮਾਈ*2+ਭਰਿਆ_Stage2_ਲੰਮਾਈ*4+mbcs_ਸਾਰਣੀ->from_UBytes_ਲੰਮਾਈ;
    mbcs_ਸਾਰਣੀ->reconstituted_ਅੰਕੜੇ = static_cast<uint8_t*>(uprv_malloc(ਅੰਕੜੇ_ਲੰਮਾਈ));
    ਜੇ(mbcs_ਸਾਰਣੀ->reconstituted_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    uprv_memset(mbcs_ਸਾਰਣੀ->reconstituted_ਅੰਕੜੇ, 0, ਅੰਕੜੇ_ਲੰਮਾਈ);

    /* copy existing data and reroute the pointers */
    stage1 = reinterpret_cast<uint16_t*>(mbcs_ਸਾਰਣੀ->reconstituted_ਅੰਕੜੇ);
    uprv_memcpy(stage1, mbcs_ਸਾਰਣੀ->from_Unicode_ਸਾਰਣੀ, stage1_ਲੰਮਾਈ*2);

    stage2 = reinterpret_cast<uint32_t*>(stage1 + stage1_ਲੰਮਾਈ);
    uprv_memcpy(stage2+(ਭਰਿਆ_Stage2_ਲੰਮਾਈ-stage2_ਲੰਮਾਈ),
                mbcs_ਸਾਰਣੀ->from_Unicode_ਸਾਰਣੀ+stage1_ਲੰਮਾਈ,
                stage2_ਲੰਮਾਈ*4);

    mbcs_ਸਾਰਣੀ->from_Unicode_ਸਾਰਣੀ=stage1;
    mbcs_ਸਾਰਣੀ->from_Unicode_ਬਾਈਟਾਂ = reinterpret_cast<uint8_t*>(stage2 + ਭਰਿਆ_Stage2_ਲੰਮਾਈ);

    /* indexes into stage 2 count from the bottom of the fromUnicodeTable */
    stage2 = reinterpret_cast<uint32_t*>(stage1);

    /* reconstitute the initial part of stage 2 from the mbcsIndex */
    {
        int32_t stage_UTF8_ਲੰਮਾਈ = (static_cast<int32_t>(mbcs_ਸਾਰਣੀ->maxFastUChar) + 1) >> 6;
        int32_t stage_UTF8_ਸੂਚਕਾਂਕ=0;
        int32_t st1, st2, st3, i;

        ਲਈ(st1=0; stage_UTF8_ਸੂਚਕਾਂਕ<stage_UTF8_ਲੰਮਾਈ; ++st1) {
            st2=stage1[st1];
            ਜੇ (st2 != static_cast<int32_t>(stage1_ਲੰਮਾਈ) / 2) {
                /* each stage 2 block has 64 entries corresponding to 16 entries in the mbcsIndex */
                ਲਈ(i=0; i<16; ++i) {
                    st3=mbcs_ਸਾਰਣੀ->mbcs_ਸੂਚਕਾਂਕ[stage_UTF8_ਸੂਚਕਾਂਕ++];
                    ਜੇ(st3!=0) {
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
                    } ਨਹੀਂਤਾਂ {
                        /* no stage 3 block, skip */
                        st2+=4;
                    }
                }
            } ਨਹੀਂਤਾਂ {
                /* no stage 2 block, skip */
                stage_UTF8_ਸੂਚਕਾਂਕ+=16;
            }
        }
    }

    /* reconstitute fromUnicodeBytes with roundtrips from toUnicode data */
    ucnv_MBCSEnum_ਮੰਜ਼ਿਲ_Unicode(mbcs_ਸਾਰਣੀ, writeStage3Roundtrip, mbcs_ਸਾਰਣੀ, p_Error_ਕੋਡ);
}

/* MBCS setup functions ----------------------------------------------------- */

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ,
          UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
          ਅਚਲ uint8_t *raw,
          UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UDataInfo info;
    UConverterMBCSTable *mbcs_ਸਾਰਣੀ=&shared_ਅੰਕੜੇ->mbcs;
    _MBCSHeader *ਸਿਰਲੇਖ=(_MBCSHeader *)raw;
    uint32_t ਵਿੱਥ;
    uint32_t ਸਿਰਲੇਖ_ਲੰਮਾਈ;
    UBool noFromU=ਝੂਠ;

    ਜੇ(ਸਿਰਲੇਖ->ਸੰਸਕਰਣ[0]==4) {
        ਸਿਰਲੇਖ_ਲੰਮਾਈ=MBCS_ਸਿਰਲੇਖ_V4_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ ਜੇ(ਸਿਰਲੇਖ->ਸੰਸਕਰਣ[0]==5 && ਸਿਰਲੇਖ->ਸੰਸਕਰਣ[1]>=3 &&
              (ਸਿਰਲੇਖ->options&MBCS_OPT_UNKNOWN_INCOMPATIBLE_ਨਕਾਬ)==0) {
        ਸਿਰਲੇਖ_ਲੰਮਾਈ=ਸਿਰਲੇਖ->options&MBCS_OPT_ਲੰਮਾਈ_ਨਕਾਬ;
        noFromU = static_cast<UBool>((ਸਿਰਲੇਖ->options & MBCS_OPT_NO_FROM_U) != 0);
    } ਨਹੀਂਤਾਂ {
        *p_Error_ਕੋਡ=U_INVALID_ਸਾਰਣੀ_FORMAT;
        ਵਾਪਸ;
    }

    mbcs_ਸਾਰਣੀ->outputType = static_cast<uint8_t>(ਸਿਰਲੇਖ->flags);
    ਜੇ(noFromU && mbcs_ਸਾਰਣੀ->outputType==MBCS_OUTPUT_1) {
        *p_Error_ਕੋਡ=U_INVALID_ਸਾਰਣੀ_FORMAT;
        ਵਾਪਸ;
    }

    /* extension data, header version 4.2 and higher */
    ਵਿੱਥ=ਸਿਰਲੇਖ->flags>>8;
    ਜੇ(ਵਿੱਥ!=0) {
        mbcs_ਸਾਰਣੀ->extIndexes = reinterpret_cast<ਅਚਲ int32_t*>(raw + ਵਿੱਥ);
    }

    ਜੇ(mbcs_ਸਾਰਣੀ->outputType==MBCS_OUTPUT_EXT_ONLY) {
        UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ ਦਲੀਲਾਂ_2=UCNV_ਲੱਦੋ_ਦਲੀਲਾਂ_INITIALIZER;
        UConverter_Shared_ਅੰਕੜੇ *ਅਧਾਰ_Shared_ਅੰਕੜੇ;
        ਅਚਲ int32_t *extIndexes;
        ਅਚਲ ਅੱਖਰ *ਅਧਾਰ_Name;

        /* extension-only file, load the base table and set values appropriately */
        ਜੇ((extIndexes=mbcs_ਸਾਰਣੀ->extIndexes)==ਖਾਲੀ_ਸੂਚਕ) {
            /* extension-only file without extension */
            *p_Error_ਕੋਡ=U_INVALID_ਸਾਰਣੀ_FORMAT;
            ਵਾਪਸ;
        }

        ਜੇ(p_ਦਲੀਲਾਂ->nestedLoads!=1) {
            /* an extension table must not be loaded as a base table */
            *p_Error_ਕੋਡ=U_INVALID_ਸਾਰਣੀ_FILE;
            ਵਾਪਸ;
        }

        /* load the base table */
        ਅਧਾਰ_Name = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਸਿਰਲੇਖ) + ਸਿਰਲੇਖ_ਲੰਮਾਈ * 4;
        ਜੇ(0==uprv_strcmp(ਅਧਾਰ_Name, shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->name)) {
            /* forbid loading this same extension-only file */
            *p_Error_ਕੋਡ=U_INVALID_ਸਾਰਣੀ_FORMAT;
            ਵਾਪਸ;
        }

        /* TODO parse package name out of the prefix of the base name in the extension .cnv file? */
        ਦਲੀਲਾਂ_2.size=ਆਕਾਰ(UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ);
        ਦਲੀਲਾਂ_2.nestedLoads=2;
        ਦਲੀਲਾਂ_2.onlyTestIsLoadable=p_ਦਲੀਲਾਂ->onlyTestIsLoadable;
        ਦਲੀਲਾਂ_2.reserved=p_ਦਲੀਲਾਂ->reserved;
        ਦਲੀਲਾਂ_2.options=p_ਦਲੀਲਾਂ->options;
        ਦਲੀਲਾਂ_2.pkg=p_ਦਲੀਲਾਂ->pkg;
        ਦਲੀਲਾਂ_2.name=ਅਧਾਰ_Name;
        ਅਧਾਰ_Shared_ਅੰਕੜੇ=ucnv_ਲੱਦੋ(&ਦਲੀਲਾਂ_2, p_Error_ਕੋਡ);
        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            ਵਾਪਸ;
        }
        ਜੇ( ਅਧਾਰ_Shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->conversionType!=UCNV_MBCS ||
            ਅਧਾਰ_Shared_ਅੰਕੜੇ->mbcs.ਅਧਾਰ_Shared_ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ
        ) {
            ucnv_unload(ਅਧਾਰ_Shared_ਅੰਕੜੇ);
            *p_Error_ਕੋਡ=U_INVALID_ਸਾਰਣੀ_FORMAT;
            ਵਾਪਸ;
        }
        ਜੇ(p_ਦਲੀਲਾਂ->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            ucnv_unload(ਅਧਾਰ_Shared_ਅੰਕੜੇ);
            ਵਾਪਸ;
        }

        /* copy the base table data */
        uprv_memcpy(mbcs_ਸਾਰਣੀ, &ਅਧਾਰ_Shared_ਅੰਕੜੇ->mbcs, ਆਕਾਰ(UConverterMBCSTable));

        /* overwrite values with relevant ones for the extension converter */
        mbcs_ਸਾਰਣੀ->ਅਧਾਰ_Shared_ਅੰਕੜੇ=ਅਧਾਰ_Shared_ਅੰਕੜੇ;
        mbcs_ਸਾਰਣੀ->extIndexes=extIndexes;

        /*
         * It would be possible to share the swapLFNL data with a base converter,
         * but the generated name would have to be different, and the memory
         * would have to be free'd only once.
         * It is easier to just create the data for the extension converter
         * separately when it is requested.
         */
        mbcs_ਸਾਰਣੀ->ਅਦਲਾ_ਬਦਲੀ_LFNLState_ਸਾਰਣੀ=ਖਾਲੀ_ਸੂਚਕ;
        mbcs_ਸਾਰਣੀ->ਅਦਲਾ_ਬਦਲੀ_LFNLFrom_Unicode_ਬਾਈਟਾਂ=ਖਾਲੀ_ਸੂਚਕ;
        mbcs_ਸਾਰਣੀ->ਅਦਲਾ_ਬਦਲੀ_LFNLName=ਖਾਲੀ_ਸੂਚਕ;

        /*
         * The reconstitutedData must be deleted only when the base converter
         * is unloaded.
         */
        mbcs_ਸਾਰਣੀ->reconstituted_ਅੰਕੜੇ=ਖਾਲੀ_ਸੂਚਕ;

        /*
         * Set a special, runtime-only outputType if the extension converter
         * is a DBCS version of a base converter that also maps single bytes.
         */
        ਜੇ( shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->conversionType==UCNV_DBCS ||
                (shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->conversionType==UCNV_MBCS &&
                 shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->min_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_Char>=2)
        ) {
            ਜੇ(ਅਧਾਰ_Shared_ਅੰਕੜੇ->mbcs.outputType==MBCS_OUTPUT_2_SISO) {
                /* the base converter is SI/SO-stateful */
                int32_t ਇੰਦਰਾਜ;

                /* get the dbcs state from the state table entry for SO=0x0e */
                ਇੰਦਰਾਜ=mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ[0][0xe];
                ਜੇ( MBCS_ਇੰਦਰਾਜ_IS_FINAL(ਇੰਦਰਾਜ) &&
                    MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ)==MBCS_STATE_CHANGE_ONLY &&
                    MBCS_ਇੰਦਰਾਜ_FINAL_STATE(ਇੰਦਰਾਜ)!=0
                ) {
                    mbcs_ਸਾਰਣੀ->dbcsOnlyState = static_cast<uint8_t>(MBCS_ਇੰਦਰਾਜ_FINAL_STATE(ਇੰਦਰਾਜ));

                    mbcs_ਸਾਰਣੀ->outputType=MBCS_OUTPUT_DBCS_ONLY;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(
                ਅਧਾਰ_Shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->conversionType==UCNV_MBCS &&
                ਅਧਾਰ_Shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->min_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_Char==1 &&
                ਅਧਾਰ_Shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_Char==2 &&
                mbcs_ਸਾਰਣੀ->ਗਿਣਤੀ_ਹਾਲਤਾਂ<=127
            ) {
                /* non-stateful base converter, need to modify the state table */
                int32_t (*new_State_ਸਾਰਣੀ)[256];
                int32_t *state;
                int32_t i, count;

                /* allocate a new state table and copy the base state table contents */
                count=mbcs_ਸਾਰਣੀ->ਗਿਣਤੀ_ਹਾਲਤਾਂ;
                new_State_ਸਾਰਣੀ = static_cast<int32_t(*)[256]>(uprv_malloc((count + 1) * 1024));
                ਜੇ(new_State_ਸਾਰਣੀ==ਖਾਲੀ_ਸੂਚਕ) {
                    ucnv_unload(ਅਧਾਰ_Shared_ਅੰਕੜੇ);
                    *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    ਵਾਪਸ;
                }

                uprv_memcpy(new_State_ਸਾਰਣੀ, mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ, count*1024);

                /* change all final single-byte entries to go to a new all-illegal state */
                state=new_State_ਸਾਰਣੀ[0];
                ਲਈ(i=0; i<256; ++i) {
                    ਜੇ(MBCS_ਇੰਦਰਾਜ_IS_FINAL(state[i])) {
                        state[i]=MBCS_ਇੰਦਰਾਜ_TRANSITION(count, 0);
                    }
                }

                /* build the new all-illegal state */
                state=new_State_ਸਾਰਣੀ[count];
                ਲਈ(i=0; i<256; ++i) {
                    state[i]=MBCS_ਇੰਦਰਾਜ_FINAL(0, MBCS_STATE_ILLEGAL, 0);
                }
                mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ=(ਅਚਲ int32_t (*)[256])new_State_ਸਾਰਣੀ;
                mbcs_ਸਾਰਣੀ->ਗਿਣਤੀ_ਹਾਲਤਾਂ = static_cast<uint8_t>(count + 1);
                mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ_Owned=ਸੱਚ;

                mbcs_ਸਾਰਣੀ->outputType=MBCS_OUTPUT_DBCS_ONLY;
            }
        }

        /*
         * unlike below for files with base tables, do not get the unicodeMask
         * from the sharedData; instead, use the base table's unicodeMask,
         * which we copied in the memcpy above;
         * this is necessary because the static data unicodeMask, especially
         * the UCNV_HAS_SUPPLEMENTARY flag, is part of the base table data
         */
    } ਨਹੀਂਤਾਂ {
        /* conversion file with a base table; an additional extension table is optional */
        /* make sure that the output type is known */
        ਚੋਣ(mbcs_ਸਾਰਣੀ->outputType) {
        ਹਾਲਤ MBCS_OUTPUT_1:
        ਹਾਲਤ MBCS_OUTPUT_2:
        ਹਾਲਤ MBCS_OUTPUT_3:
        ਹਾਲਤ MBCS_OUTPUT_4:
        ਹਾਲਤ MBCS_OUTPUT_3_EUC:
        ਹਾਲਤ MBCS_OUTPUT_4_EUC:
        ਹਾਲਤ MBCS_OUTPUT_2_SISO:
            /* OK */
            ਰੋਕੋ;
        ਮੂਲ:
            *p_Error_ਕੋਡ=U_INVALID_ਸਾਰਣੀ_FORMAT;
            ਵਾਪਸ;
        }
        ਜੇ(p_ਦਲੀਲਾਂ->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            ਵਾਪਸ;
        }

        mbcs_ਸਾਰਣੀ->ਗਿਣਤੀ_ਹਾਲਤਾਂ = static_cast<uint8_t>(ਸਿਰਲੇਖ->ਗਿਣਤੀ_ਹਾਲਤਾਂ);
        mbcs_ਸਾਰਣੀ->ਗਿਣਤੀ_ਮੰਜ਼ਿਲ_UFallbacks=ਸਿਰਲੇਖ->ਗਿਣਤੀ_ਮੰਜ਼ਿਲ_UFallbacks;
        mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ = reinterpret_cast<ਅਚਲ int32_t(*)[256]>(raw + ਸਿਰਲੇਖ_ਲੰਮਾਈ * 4);
        mbcs_ਸਾਰਣੀ->ਮੰਜ਼ਿਲ_UFallbacks = reinterpret_cast<ਅਚਲ _MBCSToUFallback*>(mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ + ਸਿਰਲੇਖ->ਗਿਣਤੀ_ਹਾਲਤਾਂ);
        mbcs_ਸਾਰਣੀ->unicode_ਕੋਡ_Units = reinterpret_cast<ਅਚਲ uint16_t*>(raw + ਸਿਰਲੇਖ->ਵਿੱਥ_ਮੰਜ਼ਿਲ_UCode_Units);

        mbcs_ਸਾਰਣੀ->from_Unicode_ਸਾਰਣੀ = reinterpret_cast<ਅਚਲ uint16_t*>(raw + ਸਿਰਲੇਖ->ਵਿੱਥ_From_UTable);
        mbcs_ਸਾਰਣੀ->from_Unicode_ਬਾਈਟਾਂ = raw + ਸਿਰਲੇਖ->ਵਿੱਥ_From_UBytes;
        mbcs_ਸਾਰਣੀ->from_UBytes_ਲੰਮਾਈ=ਸਿਰਲੇਖ->from_UBytes_ਲੰਮਾਈ;

        /*
         * converter versions 6.1 and up contain a unicodeMask that is
         * used here to select the most efficient function implementations
         */
        info.size=ਆਕਾਰ(UDataInfo);
        udata_getInfo((UData_ਸਮ੍ਰਿਤੀ *)shared_ਅੰਕੜੇ->ਅੰਕੜੇ_ਸਮ੍ਰਿਤੀ, &info);
        ਜੇ(info.format_ਸੰਸਕਰਣ[0]>6 || (info.format_ਸੰਸਕਰਣ[0]==6 && info.format_ਸੰਸਕਰਣ[1]>=1)) {
            /* mask off possible future extensions to be safe */
            mbcs_ਸਾਰਣੀ->unicode_ਨਕਾਬ = static_cast<uint8_t>(shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->unicode_ਨਕਾਬ & 3);
        } ਨਹੀਂਤਾਂ {
            /* for older versions, assume worst case: contains anything possible (prevent over-optimizations) */
            mbcs_ਸਾਰਣੀ->unicode_ਨਕਾਬ=UCNV_HAS_SUPPLEMENTARY|UCNV_HAS_SURROGATES;
        }

        /*
         * _MBCSHeader.version 4.3 adds utf8Friendly data structures.
         * Check for the header version, SBCS vs. MBCS, and for whether the
         * data structures are optimized for code points as high as what the
         * runtime code is designed for.
         * The implementation does not handle mapping tables with entries for
         * unpaired surrogates.
         */
        ਜੇ( ਸਿਰਲੇਖ->ਸੰਸਕਰਣ[1]>=3 &&
            (mbcs_ਸਾਰਣੀ->unicode_ਨਕਾਬ&UCNV_HAS_SURROGATES)==0 &&
            (mbcs_ਸਾਰਣੀ->ਗਿਣਤੀ_ਹਾਲਤਾਂ==1 ?
                (ਸਿਰਲੇਖ->ਸੰਸਕਰਣ[2]>=(SBCS_FAST_MAX>>8)) :
                (ਸਿਰਲੇਖ->ਸੰਸਕਰਣ[2]>=(MBCS_FAST_MAX>>8))
            )
        ) {
            mbcs_ਸਾਰਣੀ->utf8Friendly=ਸੱਚ;

            ਜੇ(mbcs_ਸਾਰਣੀ->ਗਿਣਤੀ_ਹਾਲਤਾਂ==1) {
                /*
                 * SBCS: Stage 3 is allocated in 64-entry blocks for U+0000..SBCS_FAST_MAX or higher.
                 * Build a table with indexes to each block, to be used instead of
                 * the regular stage 1/2 table.
                 */
                int32_t i;
                ਲਈ(i=0; i<(SBCS_FAST_ਹੱਦਬੰਦੀ>>6); ++i) {
                    mbcs_ਸਾਰਣੀ->sbcs_ਸੂਚਕਾਂਕ[i]=mbcs_ਸਾਰਣੀ->from_Unicode_ਸਾਰਣੀ[mbcs_ਸਾਰਣੀ->from_Unicode_ਸਾਰਣੀ[i>>4]+((i<<2)&0x3c)];
                }
                /* set SBCS_FAST_MAX to reflect the reach of sbcsIndex[] even if header->version[2]>(SBCS_FAST_MAX>>8) */
                mbcs_ਸਾਰਣੀ->maxFastUChar=SBCS_FAST_MAX;
            } ਨਹੀਂਤਾਂ {
                /*
                 * MBCS: Stage 3 is allocated in 64-entry blocks for U+0000..MBCS_FAST_MAX or higher.
                 * The .cnv file is prebuilt with an additional stage table with indexes
                 * to each block.
                 */
                mbcs_ਸਾਰਣੀ->mbcs_ਸੂਚਕਾਂਕ = reinterpret_cast<ਅਚਲ uint16_t*>(
                    mbcs_ਸਾਰਣੀ->from_Unicode_ਬਾਈਟਾਂ +
                     (noFromU ? 0 : mbcs_ਸਾਰਣੀ->from_UBytes_ਲੰਮਾਈ));
                mbcs_ਸਾਰਣੀ->maxFastUChar = (static_cast<char16_t>(ਸਿਰਲੇਖ->ਸੰਸਕਰਣ[2]) << 8) | 0xff;
            }
        }

        /* calculate a bit set of 4 ASCII characters per bit that round-trip to ASCII bytes */
        {
            uint32_t asciiRoundtrips=0xffffffff;
            int32_t i;

            ਲਈ(i=0; i<0x80; ++i) {
                ਜੇ(mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ[0][i]!=MBCS_ਇੰਦਰਾਜ_FINAL(0, MBCS_STATE_VALID_ਸਿੱਧਾ_16, i)) {
                    asciiRoundtrips &= ~(static_cast<uint32_t>(1) << (i >> 2));
                }
            }
            mbcs_ਸਾਰਣੀ->asciiRoundtrips=asciiRoundtrips;
        }

        ਜੇ(noFromU) {
            uint32_t stage1_ਲੰਮਾਈ=
                mbcs_ਸਾਰਣੀ->unicode_ਨਕਾਬ&UCNV_HAS_SUPPLEMENTARY ?
                    0x440 : 0x40;
            uint32_t stage2_ਲੰਮਾਈ=
                (ਸਿਰਲੇਖ->ਵਿੱਥ_From_UBytes-ਸਿਰਲੇਖ->ਵਿੱਥ_From_UTable)/4-
                stage1_ਲੰਮਾਈ/2;
            reconstitute_ਅੰਕੜੇ(mbcs_ਸਾਰਣੀ, stage1_ਲੰਮਾਈ, stage2_ਲੰਮਾਈ, ਸਿਰਲੇਖ->ਭਰਿਆ_Stage2_ਲੰਮਾਈ, p_Error_ਕੋਡ);
        }
    }

    /* Set the impl pointer here so that it is set for both extension-only and base tables. */
    ਜੇ(mbcs_ਸਾਰਣੀ->utf8Friendly) {
        ਜੇ(mbcs_ਸਾਰਣੀ->ਗਿਣਤੀ_ਹਾਲਤਾਂ==1) {
            shared_ਅੰਕੜੇ->impl=&_SBCSUTF8Impl;
        } ਨਹੀਂਤਾਂ {
            ਜੇ(mbcs_ਸਾਰਣੀ->outputType==MBCS_OUTPUT_2) {
                shared_ਅੰਕੜੇ->impl=&_DBCSUTF8Impl;
            }
        }
    }

    ਜੇ(mbcs_ਸਾਰਣੀ->outputType==MBCS_OUTPUT_DBCS_ONLY || mbcs_ਸਾਰਣੀ->outputType==MBCS_OUTPUT_2_SISO) {
        /*
         * MBCS_OUTPUT_DBCS_ONLY: No SBCS mappings, therefore ASCII does not roundtrip.
         * MBCS_OUTPUT_2_SISO: Bypass the ASCII fastpath to handle prevLength correctly.
         */
        mbcs_ਸਾਰਣੀ->asciiRoundtrips=0;
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ) {
    UConverterMBCSTable *mbcs_ਸਾਰਣੀ=&shared_ਅੰਕੜੇ->mbcs;

    ਜੇ(mbcs_ਸਾਰਣੀ->ਅਦਲਾ_ਬਦਲੀ_LFNLState_ਸਾਰਣੀ!=ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(mbcs_ਸਾਰਣੀ->ਅਦਲਾ_ਬਦਲੀ_LFNLState_ਸਾਰਣੀ);
    }
    ਜੇ(mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ_Owned) {
        uprv_free((ਖਾਲੀ *)mbcs_ਸਾਰਣੀ->state_ਸਾਰਣੀ);
    }
    ਜੇ(mbcs_ਸਾਰਣੀ->ਅਧਾਰ_Shared_ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ) {
        ucnv_unload(mbcs_ਸਾਰਣੀ->ਅਧਾਰ_Shared_ਅੰਕੜੇ);
    }
    ਜੇ(mbcs_ਸਾਰਣੀ->reconstituted_ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(mbcs_ਸਾਰਣੀ->reconstituted_ਅੰਕੜੇ);
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
              UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverterMBCSTable *mbcs_ਸਾਰਣੀ;
    ਅਚਲ int32_t *extIndexes;
    uint8_t outputType;
    int8_t max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_UChar;

    ਜੇ(p_ਦਲੀਲਾਂ->onlyTestIsLoadable) {
        ਵਾਪਸ;
    }

    mbcs_ਸਾਰਣੀ=&cnv->shared_ਅੰਕੜੇ->mbcs;
    outputType=mbcs_ਸਾਰਣੀ->outputType;

    ਜੇ(outputType==MBCS_OUTPUT_DBCS_ONLY) {
        /* the swaplfnl option does not apply, remove it */
        cnv->options=p_ਦਲੀਲਾਂ->options&=~UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL;
    }

    ਜੇ((p_ਦਲੀਲਾਂ->options&UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL)!=0) {
        /* do this because double-checked locking is broken */
        UBool isCached;

        icu::umtx_ਤਾਲਾ(ਖਾਲੀ_ਸੂਚਕ);
        isCached=mbcs_ਸਾਰਣੀ->ਅਦਲਾ_ਬਦਲੀ_LFNLState_ਸਾਰਣੀ!=ਖਾਲੀ_ਸੂਚਕ;
        icu::umtx_unlock(ਖਾਲੀ_ਸੂਚਕ);

        ਜੇ(!isCached) {
            ਜੇ(!_EBCDICSwapLFNL(cnv->shared_ਅੰਕੜੇ, p_Error_ਕੋਡ)) {
                ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                    ਵਾਪਸ; /* something went wrong */
                }

                /* the option does not apply, remove it */
                cnv->options=p_ਦਲੀਲਾਂ->options&=~UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL;
            }
        }
    }

    ਜੇ(uprv_strstr(p_ਦਲੀਲਾਂ->name, "18030")!=ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(uprv_strstr(p_ਦਲੀਲਾਂ->name, "gb18030")!=ਖਾਲੀ_ਸੂਚਕ || uprv_strstr(p_ਦਲੀਲਾਂ->name, "GB18030")!=ਖਾਲੀ_ਸੂਚਕ) {
            /* set a flag for GB 18030 mode, which changes the callback behavior */
            cnv->options|=_MBCS_OPTION_GB18030;
        }
    } ਨਹੀਂਤਾਂ ਜੇ((uprv_strstr(p_ਦਲੀਲਾਂ->name, "KEIS")!=ਖਾਲੀ_ਸੂਚਕ) || (uprv_strstr(p_ਦਲੀਲਾਂ->name, "keis")!=ਖਾਲੀ_ਸੂਚਕ)) {
        /* set a flag for KEIS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_KEIS;
    } ਨਹੀਂਤਾਂ ਜੇ((uprv_strstr(p_ਦਲੀਲਾਂ->name, "JEF")!=ਖਾਲੀ_ਸੂਚਕ) || (uprv_strstr(p_ਦਲੀਲਾਂ->name, "jef")!=ਖਾਲੀ_ਸੂਚਕ)) {
        /* set a flag for JEF converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JEF;
    } ਨਹੀਂਤਾਂ ਜੇ((uprv_strstr(p_ਦਲੀਲਾਂ->name, "JIPS")!=ਖਾਲੀ_ਸੂਚਕ) || (uprv_strstr(p_ਦਲੀਲਾਂ->name, "jips")!=ਖਾਲੀ_ਸੂਚਕ)) {
        /* set a flag for JIPS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JIPS;
    }

    /* fix maxBytesPerUChar depending on outputType and options etc. */
    ਜੇ(outputType==MBCS_OUTPUT_2_SISO) {
        cnv->max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_UChar=3; /* SO+DBCS */
    }

    extIndexes=mbcs_ਸਾਰਣੀ->extIndexes;
    ਜੇ(extIndexes!=ਖਾਲੀ_ਸੂਚਕ) {
        max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_UChar = static_cast<int8_t>(UCNV_GET_MAX_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_UCHAR(extIndexes));
        ਜੇ(outputType==MBCS_OUTPUT_2_SISO) {
            ++max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_UChar; /* SO + multiple DBCS */
        }

        ਜੇ(max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_UChar>cnv->max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_UChar) {
            cnv->max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_UChar=max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_UChar;
        }
    }

#ਜੇ 0
    /*
     * documentation of UConverter fields used for status
     * all of these fields are (re)set to 0 by ucnv_bld.c and ucnv_reset()
     */

    /* toUnicode */
    cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=0;     /*ਵਿੱਥ*/
    cnv->ਬਹੁਲਕ=0;                /* state */
    cnv->ਮੰਜ਼ਿਲ_ULength=0;           /* byteIndex */

    /* fromUnicode */
    cnv->fromUChar32=0;
    cnv->from_Unicode_ਸਥਿਤੀ=1;   /* prevLength */
#ਜੇ_ਅੰਤ
}

U_CDECL_BEGIN

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* U_CALLCONV
ucnv_MBCSGetName(ਅਚਲ UConverter *cnv) {
    ਜੇ((cnv->options&UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL)!=0 && cnv->shared_ਅੰਕੜੇ->mbcs.ਅਦਲਾ_ਬਦਲੀ_LFNLName!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ cnv->shared_ਅੰਕੜੇ->mbcs.ਅਦਲਾ_ਬਦਲੀ_LFNLName;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ cnv->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->name;
    }
}
U_CDECL_ਸਿਰਾ


/* MBCS-to-Unicode conversion functions ------------------------------------- */

ਸਥਿਰ UChar32 U_CALLCONV
ucnv_MBCSGetFallback(UConverterMBCSTable *mbcs_ਸਾਰਣੀ, uint32_t ਵਿੱਥ) {
    ਅਚਲ _MBCSToUFallback *ਮੰਜ਼ਿਲ_UFallbacks;
    uint32_t i, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ;

    ਹੱਦਬੰਦੀ=mbcs_ਸਾਰਣੀ->ਗਿਣਤੀ_ਮੰਜ਼ਿਲ_UFallbacks;
    ਜੇ(ਹੱਦਬੰਦੀ>0) {
        /* do a binary search for the fallback mapping */
        ਮੰਜ਼ਿਲ_UFallbacks=mbcs_ਸਾਰਣੀ->ਮੰਜ਼ਿਲ_UFallbacks;
        ਸ਼ੁਰੂ=0;
        ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ-1) {
            i=(ਸ਼ੁਰੂ+ਹੱਦਬੰਦੀ)/2;
            ਜੇ(ਵਿੱਥ<ਮੰਜ਼ਿਲ_UFallbacks[i].ਵਿੱਥ) {
                ਹੱਦਬੰਦੀ=i;
            } ਨਹੀਂਤਾਂ {
                ਸ਼ੁਰੂ=i;
            }
        }

        /* did we really find it? */
        ਜੇ(ਵਿੱਥ==ਮੰਜ਼ਿਲ_UFallbacks[ਸ਼ੁਰੂ].ਵਿੱਥ) {
            ਵਾਪਸ ਮੰਜ਼ਿਲ_UFallbacks[ਸ਼ੁਰੂ].ਕੋਡ_ਬਿੰਦੂ;
        }
    }

    ਵਾਪਸ 0xfffe;
}

/* This version of ucnv_MBCSToUnicodeWithOffsets() is optimized for single-byte, single-state codepages. */
ਸਥਿਰ ਖਾਲੀ
ucnv_MBCSSingle_ਮੰਜ਼ਿਲ_Unicode_With_Offsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                                UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    char16_t *ਨਿਸ਼ਾਨਾ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    int32_t *offsets;

    ਅਚਲ int32_t (*state_ਸਾਰਣੀ)[256];

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    int32_t ਇੰਦਰਾਜ;
    char16_t c;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    ਸਰੋਤ = reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਸਰੋਤ);
    ਸਰੋਤ_ਹੱਦਬੰਦੀ = reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ);
    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    offsets=p_ਦਲੀਲਾਂ->offsets;

    ਜੇ((cnv->options&UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL)!=0) {
        state_ਸਾਰਣੀ=(ਅਚਲ int32_t (*)[256])cnv->shared_ਅੰਕੜੇ->mbcs.ਅਦਲਾ_ਬਦਲੀ_LFNLState_ਸਾਰਣੀ;
    } ਨਹੀਂਤਾਂ {
        state_ਸਾਰਣੀ=cnv->shared_ਅੰਕੜੇ->mbcs.state_ਸਾਰਣੀ;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    /* conversion loop */
    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ਜੇ(ਨਿਸ਼ਾਨਾ>=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }

        ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        ਜੇ(MBCS_ਇੰਦਰਾਜ_FINAL_IS_VALID_ਸਿੱਧਾ_16(ਇੰਦਰਾਜ)) {
            /* output BMP code point */
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }

            /* normal end of action codes: prepare for a new character */
            ++ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਜਾਰੀ;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ));
        ਜੇ(action==MBCS_STATE_VALID_ਸਿੱਧਾ_20 ||
           (action==MBCS_STATE_FALLBACK_ਸਿੱਧਾ_20 && UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv))
        ) {
            ਇੰਦਰਾਜ=MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ(ਇੰਦਰਾਜ);
            /* output surrogate pair */
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(0xd800 | static_cast<char16_t>(ਇੰਦਰਾਜ >> 10));
            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }
            c = static_cast<char16_t>(0xdc00 | static_cast<char16_t>(ਇੰਦਰਾਜ & 0x3ff));
            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                *ਨਿਸ਼ਾਨਾ++=c;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
            } ਨਹੀਂਤਾਂ {
                /* target overflow */
                cnv->UChar_Error_ਬਫਰ[0]=c;
                cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }

            ++ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_FALLBACK_ਸਿੱਧਾ_16) {
            ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }

                ++ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                ਜਾਰੀ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
        } ਨਹੀਂਤਾਂ {
            /* reserved, must never occur */
            ++ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਜਾਰੀ;
        }

        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            /* callback(illegal) */
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_ਦਲੀਲਾਂ->ਸਰੋਤ = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਸਰੋਤ);
            cnv->ਮੰਜ਼ਿਲ_UBytes[0]=*(ਸਰੋਤ-1);
            cnv->ਮੰਜ਼ਿਲ_ULength=ext_ਮੰਜ਼ਿਲ_U(cnv, cnv->shared_ਅੰਕੜੇ,
                                    1, &ਸਰੋਤ, ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                                    &ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                                    &offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                                    p_ਦਲੀਲਾਂ->flush,
                                    p_Error_ਕੋਡ);
            ਸਰੋਤ_ਸੂਚਕਾਂਕ += 1 + static_cast<int32_t>(ਸਰੋਤ - reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਸਰੋਤ));

            ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                /* not mappable or buffer overflow */
                ਰੋਕੋ;
            }
        }
    }

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਸਰੋਤ);
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

/*
 * This version of ucnv_MBCSSingleToUnicodeWithOffsets() is optimized for single-byte, single-state codepages
 * that only map to and from the BMP.
 * In addition to single-byte optimizations, the offset calculations
 * become much easier.
 */
ਸਥਿਰ ਖਾਲੀ
ucnv_MBCSSingle_ਮੰਜ਼ਿਲ_BMPWith_Offsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                            UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ, *ਆਖਰੀ_ਸਰੋਤ;
    char16_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, length;
    int32_t *offsets;

    ਅਚਲ int32_t (*state_ਸਾਰਣੀ)[256];

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    int32_t ਇੰਦਰਾਜ;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    ਸਰੋਤ = reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਸਰੋਤ);
    ਸਰੋਤ_ਹੱਦਬੰਦੀ = reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ);
    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;

    ਜੇ((cnv->options&UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL)!=0) {
        state_ਸਾਰਣੀ=(ਅਚਲ int32_t (*)[256])cnv->shared_ਅੰਕੜੇ->mbcs.ਅਦਲਾ_ਬਦਲੀ_LFNLState_ਸਾਰਣੀ;
    } ਨਹੀਂਤਾਂ {
        state_ਸਾਰਣੀ=cnv->shared_ਅੰਕੜੇ->mbcs.state_ਸਾਰਣੀ;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;
    ਆਖਰੀ_ਸਰੋਤ=ਸਰੋਤ;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(ਸਰੋਤ_ਹੱਦਬੰਦੀ - ਸਰੋਤ);
    ਜੇ(length<ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=length;
    }

#ਜੇ MBCS_UNROLL_SINGLE_ਮੰਜ਼ਿਲ_BMP
    /* unrolling makes it faster on Pentium III/Windows 2000 */
    /* unroll the loop with the most common case */
unrolled:
    ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>=16) {
        int32_t count, loops, oredEntries;

        loops=count=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>>4;
        ਕਰੋ {
            oredEntries=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            oredEntries|=ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));

            /* were all 16 entries really valid? */
            ਜੇ(!MBCS_ਇੰਦਰਾਜ_FINAL_IS_VALID_ਸਿੱਧਾ_16(oredEntries)) {
                /* no, return to the first of these 16 */
                ਸਰੋਤ-=16;
                ਨਿਸ਼ਾਨਾ-=16;
                ਰੋਕੋ;
            }
        } ਜਦੋਂ(--count>0);
        count=loops-count;
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=16*count;

        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
            ਆਖਰੀ_ਸਰੋਤ+=16*count;
            ਜਦੋਂ(count>0) {
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                --count;
            }
        }
    }
#ਜੇ_ਅੰਤ

    /* conversion loop */
    ਜਦੋਂ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ > 0 && ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        ਜੇ(MBCS_ਇੰਦਰਾਜ_FINAL_IS_VALID_ਸਿੱਧਾ_16(ਇੰਦਰਾਜ)) {
            /* output BMP code point */
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
            ਜਾਰੀ;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ));
        ਜੇ(action==MBCS_STATE_FALLBACK_ਸਿੱਧਾ_16) {
            ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                ਜਾਰੀ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
        } ਨਹੀਂਤਾਂ {
            /* reserved, must never occur */
            ਜਾਰੀ;
        }

        /* set offsets since the start or the last extension */
        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
            int32_t count = static_cast<int32_t>(ਸਰੋਤ - ਆਖਰੀ_ਸਰੋਤ);

            /* predecrement: do not set the offset for the callback-causing character */
            ਜਦੋਂ(--count>0) {
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
            }
            /* offset and sourceIndex are now set for the current character */
        }

        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            /* callback(illegal) */
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            ਆਖਰੀ_ਸਰੋਤ=ਸਰੋਤ;
            cnv->ਮੰਜ਼ਿਲ_UBytes[0]=*(ਸਰੋਤ-1);
            cnv->ਮੰਜ਼ਿਲ_ULength=ext_ਮੰਜ਼ਿਲ_U(cnv, cnv->shared_ਅੰਕੜੇ,
                                    1, &ਸਰੋਤ, ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                                    &ਨਿਸ਼ਾਨਾ, p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                                    &offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                                    p_ਦਲੀਲਾਂ->flush,
                                    p_Error_ਕੋਡ);
            ਸਰੋਤ_ਸੂਚਕਾਂਕ += 1 + static_cast<int32_t>(ਸਰੋਤ - ਆਖਰੀ_ਸਰੋਤ);

            ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                /* not mappable or buffer overflow */
                ਰੋਕੋ;
            }

            /* recalculate the targetCapacity after an extension mapping */
            ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - ਨਿਸ਼ਾਨਾ);
            length = static_cast<int32_t>(ਸਰੋਤ_ਹੱਦਬੰਦੀ - ਸਰੋਤ);
            ਜੇ(length<ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
                ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=length;
            }
        }

#ਜੇ MBCS_UNROLL_SINGLE_ਮੰਜ਼ਿਲ_BMP
        /* unrolling makes it faster on Pentium III/Windows 2000 */
        ਜਾਓ unrolled;
#ਜੇ_ਅੰਤ
    }

    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ) && ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ>=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        /* target is full */
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    /* set offsets since the start or the last callback */
    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
        size_t count=ਸਰੋਤ-ਆਖਰੀ_ਸਰੋਤ;
        ਜਦੋਂ(count>0) {
            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਸਰੋਤ);
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

ਸਥਿਰ UBool
has_Valid_Trail_ਬਾਈਟਾਂ(ਅਚਲ int32_t (*state_ਸਾਰਣੀ)[256], uint8_t state) {
    ਅਚਲ int32_t *ਪੰਕਤੀ=state_ਸਾਰਣੀ[state];
    int32_t b, ਇੰਦਰਾਜ;
    /* First test for final entries in this state for some commonly valid byte values. */
    ਇੰਦਰਾਜ=ਪੰਕਤੀ[0xa1];
    ਜੇ( !MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(ਇੰਦਰਾਜ) &&
        MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ)!=MBCS_STATE_ILLEGAL
    ) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਇੰਦਰਾਜ=ਪੰਕਤੀ[0x41];
    ਜੇ( !MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(ਇੰਦਰਾਜ) &&
        MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ)!=MBCS_STATE_ILLEGAL
    ) {
        ਵਾਪਸ ਸੱਚ;
    }
    /* Then test for final entries in this state. */
    ਲਈ(b=0; b<=0xff; ++b) {
        ਇੰਦਰਾਜ=ਪੰਕਤੀ[b];
        ਜੇ( !MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(ਇੰਦਰਾਜ) &&
            MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ)!=MBCS_STATE_ILLEGAL
        ) {
            ਵਾਪਸ ਸੱਚ;
        }
    }
    /* Then recurse for transition entries. */
    ਲਈ(b=0; b<=0xff; ++b) {
        ਇੰਦਰਾਜ=ਪੰਕਤੀ[b];
        ਜੇ( MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(ਇੰਦਰਾਜ) &&
            has_Valid_Trail_ਬਾਈਟਾਂ(state_ਸਾਰਣੀ, static_cast<uint8_t>(MBCS_ਇੰਦਰਾਜ_TRANSITION_STATE(ਇੰਦਰਾਜ)))
        ) {
            ਵਾਪਸ ਸੱਚ;
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

/*
 * Is byte b a single/lead byte in this state?
 * Recurse for transition states, because here we don't want to say that
 * b is a lead byte if all byte sequences that start with b are illegal.
 */
ਸਥਿਰ UBool
isSingleOrLead(ਅਚਲ int32_t (*state_ਸਾਰਣੀ)[256], uint8_t state, UBool isDBCSOnly, uint8_t b) {
    ਅਚਲ int32_t *ਪੰਕਤੀ=state_ਸਾਰਣੀ[state];
    int32_t ਇੰਦਰਾਜ=ਪੰਕਤੀ[b];
    ਜੇ(MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(ਇੰਦਰਾਜ)) {   /* lead byte */
        ਵਾਪਸ has_Valid_Trail_ਬਾਈਟਾਂ(state_ਸਾਰਣੀ, static_cast<uint8_t>(MBCS_ਇੰਦਰਾਜ_TRANSITION_STATE(ਇੰਦਰਾਜ)));
    } ਨਹੀਂਤਾਂ {
        uint8_t action = static_cast<uint8_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ));
        ਜੇ(action==MBCS_STATE_CHANGE_ONLY && isDBCSOnly) {
            ਵਾਪਸ ਝੂਠ;   /* SI/SO are illegal for DBCS-only conversion */
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ action!=MBCS_STATE_ILLEGAL;
        }
    }
}

U_CFUNC ਖਾਲੀ
ucnv_MBCSToUnicodeWithOffsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                          UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    char16_t *ਨਿਸ਼ਾਨਾ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    int32_t *offsets;

    ਅਚਲ int32_t (*state_ਸਾਰਣੀ)[256];
    ਅਚਲ uint16_t *unicode_ਕੋਡ_Units;

    uint32_t ਵਿੱਥ;
    uint8_t state;
    int8_t byte_ਸੂਚਕਾਂਕ;
    uint8_t *ਬਾਈਟਾਂ;

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ, ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    int32_t ਇੰਦਰਾਜ;
    char16_t c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_ਦਲੀਲਾਂ->converter;

    ਜੇ(cnv->pre_ਮੰਜ਼ਿਲ_ULength>0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_ਮੇਲ_ਮੰਜ਼ਿਲ_U(cnv, p_ਦਲੀਲਾਂ, -1, p_Error_ਕੋਡ);

        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ) || cnv->pre_ਮੰਜ਼ਿਲ_ULength<0) {
            ਵਾਪਸ;
        }
    }

    ਜੇ(cnv->shared_ਅੰਕੜੇ->mbcs.ਗਿਣਤੀ_ਹਾਲਤਾਂ==1) {
        ਜੇ(!(cnv->shared_ਅੰਕੜੇ->mbcs.unicode_ਨਕਾਬ&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingle_ਮੰਜ਼ਿਲ_BMPWith_Offsets(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ {
            ucnv_MBCSSingle_ਮੰਜ਼ਿਲ_Unicode_With_Offsets(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
        }
        ਵਾਪਸ;
    }

    /* set up the local pointers */
    ਸਰੋਤ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    offsets=p_ਦਲੀਲਾਂ->offsets;

    ਜੇ((cnv->options&UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL)!=0) {
        state_ਸਾਰਣੀ=(ਅਚਲ int32_t (*)[256])cnv->shared_ਅੰਕੜੇ->mbcs.ਅਦਲਾ_ਬਦਲੀ_LFNLState_ਸਾਰਣੀ;
    } ਨਹੀਂਤਾਂ {
        state_ਸਾਰਣੀ=cnv->shared_ਅੰਕੜੇ->mbcs.state_ਸਾਰਣੀ;
    }
    unicode_ਕੋਡ_Units=cnv->shared_ਅੰਕੜੇ->mbcs.unicode_ਕੋਡ_Units;

    /* get the converter state from UConverter */
    ਵਿੱਥ=cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;
    byte_ਸੂਚਕਾਂਕ=cnv->ਮੰਜ਼ਿਲ_ULength;
    ਬਾਈਟਾਂ=cnv->ਮੰਜ਼ਿਲ_UBytes;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    ਜੇ((state=(uint8_t)(cnv->ਬਹੁਲਕ))==0) {
        state=cnv->shared_ਅੰਕੜੇ->mbcs.dbcsOnlyState;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=byte_ਸੂਚਕਾਂਕ==0 ? 0 : -1;
    ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    /* conversion loop */
    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ਜੇ(ਨਿਸ਼ਾਨਾ>=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }

        ਜੇ(byte_ਸੂਚਕਾਂਕ==0) {
            /* optimized loop for 1/2-byte input and BMP output */
            ਜੇ(offsets==ਖਾਲੀ_ਸੂਚਕ) {
                ਕਰੋ {
                    ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[state][*ਸਰੋਤ];
                    ਜੇ(MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(ਇੰਦਰਾਜ)) {
                        state=(uint8_t)MBCS_ਇੰਦਰਾਜ_TRANSITION_STATE(ਇੰਦਰਾਜ);
                        ਵਿੱਥ=MBCS_ਇੰਦਰਾਜ_TRANSITION_ਵਿੱਥ(ਇੰਦਰਾਜ);

                        ++ਸਰੋਤ;
                        ਜੇ( ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ &&
                            MBCS_ਇੰਦਰਾਜ_IS_FINAL(ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[state][*ਸਰੋਤ]) &&
                            MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ)==MBCS_STATE_VALID_16 &&
                            (c=unicode_ਕੋਡ_Units[ਵਿੱਥ+MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ)])<0xfffe
                        ) {
                            ++ਸਰੋਤ;
                            *ਨਿਸ਼ਾਨਾ++=c;
                            state=(uint8_t)MBCS_ਇੰਦਰਾਜ_FINAL_STATE(ਇੰਦਰਾਜ); /* typically 0 */
                            ਵਿੱਥ=0;
                        } ਨਹੀਂਤਾਂ {
                            /* set the state and leave the optimized loop */
                            ਬਾਈਟਾਂ[0]=*(ਸਰੋਤ-1);
                            byte_ਸੂਚਕਾਂਕ=1;
                            ਰੋਕੋ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        ਜੇ(MBCS_ਇੰਦਰਾਜ_FINAL_IS_VALID_ਸਿੱਧਾ_16(ਇੰਦਰਾਜ)) {
                            /* output BMP code point */
                            ++ਸਰੋਤ;
                            *ਨਿਸ਼ਾਨਾ++=(char16_t)MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
                            state=(uint8_t)MBCS_ਇੰਦਰਾਜ_FINAL_STATE(ਇੰਦਰਾਜ); /* typically 0 */
                        } ਨਹੀਂਤਾਂ {
                            /* leave the optimized loop */
                            ਰੋਕੋ;
                        }
                    }
                } ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ);
            } ਨਹੀਂਤਾਂ /* offsets!=nullptr */ {
                ਕਰੋ {
                    ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[state][*ਸਰੋਤ];
                    ਜੇ(MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(ਇੰਦਰਾਜ)) {
                        state=(uint8_t)MBCS_ਇੰਦਰਾਜ_TRANSITION_STATE(ਇੰਦਰਾਜ);
                        ਵਿੱਥ=MBCS_ਇੰਦਰਾਜ_TRANSITION_ਵਿੱਥ(ਇੰਦਰਾਜ);

                        ++ਸਰੋਤ;
                        ਜੇ( ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ &&
                            MBCS_ਇੰਦਰਾਜ_IS_FINAL(ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[state][*ਸਰੋਤ]) &&
                            MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ)==MBCS_STATE_VALID_16 &&
                            (c=unicode_ਕੋਡ_Units[ਵਿੱਥ+MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ)])<0xfffe
                        ) {
                            ++ਸਰੋਤ;
                            *ਨਿਸ਼ਾਨਾ++=c;
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                ਸਰੋਤ_ਸੂਚਕਾਂਕ=(ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ+=2);
                            }
                            state=(uint8_t)MBCS_ਇੰਦਰਾਜ_FINAL_STATE(ਇੰਦਰਾਜ); /* typically 0 */
                            ਵਿੱਥ=0;
                        } ਨਹੀਂਤਾਂ {
                            /* set the state and leave the optimized loop */
                            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            ਬਾਈਟਾਂ[0]=*(ਸਰੋਤ-1);
                            byte_ਸੂਚਕਾਂਕ=1;
                            ਰੋਕੋ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        ਜੇ(MBCS_ਇੰਦਰਾਜ_FINAL_IS_VALID_ਸਿੱਧਾ_16(ਇੰਦਰਾਜ)) {
                            /* output BMP code point */
                            ++ਸਰੋਤ;
                            *ਨਿਸ਼ਾਨਾ++=(char16_t)MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                ਸਰੋਤ_ਸੂਚਕਾਂਕ=++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            }
                            state=(uint8_t)MBCS_ਇੰਦਰਾਜ_FINAL_STATE(ਇੰਦਰਾਜ); /* typically 0 */
                        } ਨਹੀਂਤਾਂ {
                            /* leave the optimized loop */
                            ਰੋਕੋ;
                        }
                    }
                } ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ);
            }

            /*
             * these tests and break statements could be put inside the loop
             * if C had "break outerLoop" like Java
             */
            ਜੇ(ਸਰੋਤ>=ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                ਰੋਕੋ;
            }
            ਜੇ(ਨਿਸ਼ਾਨਾ>=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                /* target is full */
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }

            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਬਾਈਟਾਂ[byte_ਸੂਚਕਾਂਕ++]=*ਸਰੋਤ++;
        } ਨਹੀਂਤਾਂ /* byteIndex>0 */ {
            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[state][ਬਾਈਟਾਂ[byte_ਸੂਚਕਾਂਕ++]=*ਸਰੋਤ++];
        }

        ਜੇ(MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(ਇੰਦਰਾਜ)) {
            state=(uint8_t)MBCS_ਇੰਦਰਾਜ_TRANSITION_STATE(ਇੰਦਰਾਜ);
            ਵਿੱਥ+=MBCS_ਇੰਦਰਾਜ_TRANSITION_ਵਿੱਥ(ਇੰਦਰਾਜ);
            ਜਾਰੀ;
        }

        /* save the previous state for proper extension mapping with SI/SO-stateful converters */
        cnv->ਬਹੁਲਕ=state;

        /* set the next state early so that we can reuse the entry variable */
        state=(uint8_t)MBCS_ਇੰਦਰਾਜ_FINAL_STATE(ਇੰਦਰਾਜ); /* typically 0 */

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action=(uint8_t)(MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ));
        ਜੇ(action==MBCS_STATE_VALID_16) {
            ਵਿੱਥ+=MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
            c=unicode_ਕੋਡ_Units[ਵਿੱਥ];
            ਜੇ(c<0xfffe) {
                /* output BMP code point */
                *ਨਿਸ਼ਾਨਾ++=c;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                byte_ਸੂਚਕਾਂਕ=0;
            } ਨਹੀਂਤਾਂ ਜੇ(c==0xfffe) {
                ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv) && (ਇੰਦਰਾਜ=(int32_t)ucnv_MBCSGetFallback(&cnv->shared_ਅੰਕੜੇ->mbcs, ਵਿੱਥ))!=0xfffe) {
                    /* output fallback BMP code point */
                    *ਨਿਸ਼ਾਨਾ++=(char16_t)ਇੰਦਰਾਜ;
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                    byte_ਸੂਚਕਾਂਕ=0;
                }
            } ਨਹੀਂਤਾਂ {
                /* callback(illegal) */
                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_VALID_ਸਿੱਧਾ_16) {
            /* output BMP code point */
            *ਨਿਸ਼ਾਨਾ++=(char16_t)MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }
            byte_ਸੂਚਕਾਂਕ=0;
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_VALID_16_PAIR) {
            ਵਿੱਥ+=MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
            c=unicode_ਕੋਡ_Units[ਵਿੱਥ++];
            ਜੇ(c<0xd800) {
                /* output BMP code point below 0xd800 */
                *ਨਿਸ਼ਾਨਾ++=c;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                byte_ਸੂਚਕਾਂਕ=0;
            } ਨਹੀਂਤਾਂ ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                /* output roundtrip or fallback surrogate pair */
                *ਨਿਸ਼ਾਨਾ++=(char16_t)(c&0xdbff);
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                byte_ਸੂਚਕਾਂਕ=0;
                ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                    *ਨਿਸ਼ਾਨਾ++=unicode_ਕੋਡ_Units[ਵਿੱਥ];
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* target overflow */
                    cnv->UChar_Error_ਬਫਰ[0]=unicode_ਕੋਡ_Units[ਵਿੱਥ];
                    cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                    *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;

                    ਵਿੱਥ=0;
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                *ਨਿਸ਼ਾਨਾ++=unicode_ਕੋਡ_Units[ਵਿੱਥ];
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                byte_ਸੂਚਕਾਂਕ=0;
            } ਨਹੀਂਤਾਂ ਜੇ(c==0xffff) {
                /* callback(illegal) */
                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_VALID_ਸਿੱਧਾ_20 ||
                  (action==MBCS_STATE_FALLBACK_ਸਿੱਧਾ_20 && UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv))
        ) {
            ਇੰਦਰਾਜ=MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ(ਇੰਦਰਾਜ);
            /* output surrogate pair */
            *ਨਿਸ਼ਾਨਾ++=(char16_t)(0xd800|(char16_t)(ਇੰਦਰਾਜ>>10));
            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }
            byte_ਸੂਚਕਾਂਕ=0;
            c=(char16_t)(0xdc00|(char16_t)(ਇੰਦਰਾਜ&0x3ff));
            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                *ਨਿਸ਼ਾਨਾ++=c;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
            } ਨਹੀਂਤਾਂ {
                /* target overflow */
                cnv->UChar_Error_ਬਫਰ[0]=c;
                cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;

                ਵਿੱਥ=0;
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_CHANGE_ONLY) {
            /*
             * This serves as a state change without any output.
             * It is useful for reading simple stateful encodings,
             * for example using just Shift-In/Shift-Out codes.
             * The 21 unused bits may later be used for more sophisticated
             * state transitions.
             */
            ਜੇ(cnv->shared_ਅੰਕੜੇ->mbcs.dbcsOnlyState==0) {
                byte_ਸੂਚਕਾਂਕ=0;
            } ਨਹੀਂਤਾਂ {
                /* SI/SO are illegal for DBCS-only conversion */
                state=(uint8_t)(cnv->ਬਹੁਲਕ); /* restore the previous state */

                /* callback(illegal) */
                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_FALLBACK_ਸਿੱਧਾ_16) {
            ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *ਨਿਸ਼ਾਨਾ++=(char16_t)MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                byte_ਸੂਚਕਾਂਕ=0;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
        } ਨਹੀਂਤਾਂ {
            /* reserved, must never occur */
            byte_ਸੂਚਕਾਂਕ=0;
        }

        /* end of action codes: prepare for a new character */
        ਵਿੱਥ=0;

        ਜੇ(byte_ਸੂਚਕਾਂਕ==0) {
            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
        } ਨਹੀਂਤਾਂ ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            /* callback(illegal) */
            ਜੇ(byte_ਸੂਚਕਾਂਕ>1) {
                /*
                 * Ticket 5691: consistent illegal sequences:
                 * - We include at least the first byte in the illegal sequence.
                 * - If any of the non-initial bytes could be the start of a character,
                 *   we stop the illegal sequence before the first one of those.
                 */
                UBool isDBCSOnly = cnv->shared_ਅੰਕੜੇ->mbcs.dbcsOnlyState != 0;
                int8_t i;
                ਲਈ(i=1;
                    i<byte_ਸੂਚਕਾਂਕ && !isSingleOrLead(state_ਸਾਰਣੀ, state, isDBCSOnly, ਬਾਈਟਾਂ[i]);
                    ++i) {}
                ਜੇ(i<byte_ਸੂਚਕਾਂਕ) {
                    /* Back out some bytes. */
                    int8_t ਪਿੱਠ_ਬਾਹਰਲਾ_ਦੂਰੀ=byte_ਸੂਚਕਾਂਕ-i;
                    int32_t ਬਾਈਟਾਂ_From_This_ਬਫਰ=(int32_t)(ਸਰੋਤ-(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ);
                    byte_ਸੂਚਕਾਂਕ=i;  /* length of reported illegal byte sequence */
                    ਜੇ(ਪਿੱਠ_ਬਾਹਰਲਾ_ਦੂਰੀ<=ਬਾਈਟਾਂ_From_This_ਬਫਰ) {
                        ਸਰੋਤ-=ਪਿੱਠ_ਬਾਹਰਲਾ_ਦੂਰੀ;
                    } ਨਹੀਂਤਾਂ {
                        /* Back out bytes from the previous buffer: Need to replay them. */
                        cnv->pre_ਮੰਜ਼ਿਲ_ULength=(int8_t)(ਬਾਈਟਾਂ_From_This_ਬਫਰ-ਪਿੱਠ_ਬਾਹਰਲਾ_ਦੂਰੀ);
                        /* preToULength is negative! */
                        uprv_memcpy(cnv->pre_ਮੰਜ਼ਿਲ_U, ਬਾਈਟਾਂ+i, -cnv->pre_ਮੰਜ਼ਿਲ_ULength);
                        ਸਰੋਤ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
                    }
                }
            }
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
            byte_ਸੂਚਕਾਂਕ=ext_ਮੰਜ਼ਿਲ_U(cnv, cnv->shared_ਅੰਕੜੇ,
                              byte_ਸੂਚਕਾਂਕ, &ਸਰੋਤ, ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                              &ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                              &offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                              p_ਦਲੀਲਾਂ->flush,
                              p_Error_ਕੋਡ);
            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ+=(int32_t)(ਸਰੋਤ-(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ);

            ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                /* not mappable or buffer overflow */
                ਰੋਕੋ;
            }
        }
    }

    /* set the converter state back into UConverter */
    cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=ਵਿੱਥ;
    cnv->ਬਹੁਲਕ=state;
    cnv->ਮੰਜ਼ਿਲ_ULength=byte_ਸੂਚਕਾਂਕ;

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

/*
 * This version of ucnv_MBCSGetNextUChar() is optimized for single-byte, single-state codepages.
 * We still need a conversion loop in case we find reserved action codes, which are to be ignored.
 */
ਸਥਿਰ UChar32
ucnv_MBCSSingle_Get_ਅਗਲਾ_UChar(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                        UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ int32_t (*state_ਸਾਰਣੀ)[256];
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;

    int32_t ਇੰਦਰਾਜ;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    ਸਰੋਤ = reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਸਰੋਤ);
    ਸਰੋਤ_ਹੱਦਬੰਦੀ = reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ);
    ਜੇ((cnv->options&UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL)!=0) {
        state_ਸਾਰਣੀ=(ਅਚਲ int32_t (*)[256])cnv->shared_ਅੰਕੜੇ->mbcs.ਅਦਲਾ_ਬਦਲੀ_LFNLState_ਸਾਰਣੀ;
    } ਨਹੀਂਤਾਂ {
        state_ਸਾਰਣੀ=cnv->shared_ਅੰਕੜੇ->mbcs.state_ਸਾਰਣੀ;
    }

    /* conversion loop */
    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[0][*ਸਰੋਤ++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* write back the updated pointer early so that we can return directly */
        p_ਦਲੀਲਾਂ->ਸਰੋਤ = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਸਰੋਤ);

        ਜੇ(MBCS_ਇੰਦਰਾਜ_FINAL_IS_VALID_ਸਿੱਧਾ_16(ਇੰਦਰਾਜ)) {
            /* output BMP code point */
            ਵਾਪਸ static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ));
        ਜੇ( action==MBCS_STATE_VALID_ਸਿੱਧਾ_20 ||
            (action==MBCS_STATE_FALLBACK_ਸਿੱਧਾ_20 && UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv))
        ) {
            /* output supplementary code point */
            ਵਾਪਸ static_cast<UChar32>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ(ਇੰਦਰਾਜ) + 0x10000);
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_FALLBACK_ਸਿੱਧਾ_16) {
            ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                ਵਾਪਸ static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
            }
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
        } ਨਹੀਂਤਾਂ {
            /* reserved, must never occur */
            ਜਾਰੀ;
        }

        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            /* callback(illegal) */
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ /* unassigned sequence */ {
            /* defer to the generic implementation */
            p_ਦਲੀਲਾਂ->ਸਰੋਤ = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਸਰੋਤ) - 1;
            ਵਾਪਸ UCNV_GET_ਅਗਲਾ_UCHAR_USE_ਮੰਜ਼ਿਲ_U;
        }
    }

    /* no output because of empty input or only state changes */
    *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
    ਵਾਪਸ 0xffff;
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
ਸਥਿਰ UChar32 U_CALLCONV
ucnv_MBCSGet_ਅਗਲਾ_UChar(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ, *ਆਖਰੀ_ਸਰੋਤ;

    ਅਚਲ int32_t (*state_ਸਾਰਣੀ)[256];
    ਅਚਲ uint16_t *unicode_ਕੋਡ_Units;

    uint32_t ਵਿੱਥ;
    uint8_t state;

    int32_t ਇੰਦਰਾਜ;
    UChar32 c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_ਦਲੀਲਾਂ->converter;

    ਜੇ(cnv->pre_ਮੰਜ਼ਿਲ_ULength>0) {
        /* use the generic code in ucnv_getNextUChar() to continue with a partial match */
        ਵਾਪਸ UCNV_GET_ਅਗਲਾ_UCHAR_USE_ਮੰਜ਼ਿਲ_U;
    }

    ਜੇ(cnv->shared_ਅੰਕੜੇ->mbcs.unicode_ਨਕਾਬ&UCNV_HAS_SURROGATES) {
        /*
         * Using the generic ucnv_getNextUChar() code lets us deal correctly
         * with the rare case of a codepage that maps single surrogates
         * without adding the complexity to this already complicated function here.
         */
        ਵਾਪਸ UCNV_GET_ਅਗਲਾ_UCHAR_USE_ਮੰਜ਼ਿਲ_U;
    } ਨਹੀਂਤਾਂ ਜੇ(cnv->shared_ਅੰਕੜੇ->mbcs.ਗਿਣਤੀ_ਹਾਲਤਾਂ==1) {
        ਵਾਪਸ ucnv_MBCSSingle_Get_ਅਗਲਾ_UChar(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
    }

    /* set up the local pointers */
    ਸਰੋਤ = ਆਖਰੀ_ਸਰੋਤ = reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਸਰੋਤ);
    ਸਰੋਤ_ਹੱਦਬੰਦੀ = reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ);

    ਜੇ((cnv->options&UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL)!=0) {
        state_ਸਾਰਣੀ=(ਅਚਲ int32_t (*)[256])cnv->shared_ਅੰਕੜੇ->mbcs.ਅਦਲਾ_ਬਦਲੀ_LFNLState_ਸਾਰਣੀ;
    } ਨਹੀਂਤਾਂ {
        state_ਸਾਰਣੀ=cnv->shared_ਅੰਕੜੇ->mbcs.state_ਸਾਰਣੀ;
    }
    unicode_ਕੋਡ_Units=cnv->shared_ਅੰਕੜੇ->mbcs.unicode_ਕੋਡ_Units;

    /* get the converter state from UConverter */
    ਵਿੱਥ=cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    ਜੇ ((state = static_cast<uint8_t>(cnv->ਬਹੁਲਕ)) == 0) {
        state=cnv->shared_ਅੰਕੜੇ->mbcs.dbcsOnlyState;
    }

    /* conversion loop */
    c=U_SENTINEL;
    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[state][*ਸਰੋਤ++];
        ਜੇ(MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(ਇੰਦਰਾਜ)) {
            state = static_cast<uint8_t>(MBCS_ਇੰਦਰਾਜ_TRANSITION_STATE(ਇੰਦਰਾਜ));
            ਵਿੱਥ+=MBCS_ਇੰਦਰਾਜ_TRANSITION_ਵਿੱਥ(ਇੰਦਰਾਜ);

            /* optimization for 1/2-byte input and BMP output */
            ਜੇ( ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ &&
                MBCS_ਇੰਦਰਾਜ_IS_FINAL(ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[state][*ਸਰੋਤ]) &&
                MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ)==MBCS_STATE_VALID_16 &&
                (c=unicode_ਕੋਡ_Units[ਵਿੱਥ+MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ)])<0xfffe
            ) {
                ++ਸਰੋਤ;
                state = static_cast<uint8_t>(MBCS_ਇੰਦਰਾਜ_FINAL_STATE(ਇੰਦਰਾਜ)); /* typically 0 */
                /* output BMP code point */
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ {
            /* save the previous state for proper extension mapping with SI/SO-stateful converters */
            cnv->ਬਹੁਲਕ=state;

            /* set the next state early so that we can reuse the entry variable */
            state = static_cast<uint8_t>(MBCS_ਇੰਦਰਾਜ_FINAL_STATE(ਇੰਦਰਾਜ)); /* typically 0 */

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action = static_cast<uint8_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ));
            ਜੇ(action==MBCS_STATE_VALID_ਸਿੱਧਾ_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_VALID_16) {
                ਵਿੱਥ+=MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
                c=unicode_ਕੋਡ_Units[ਵਿੱਥ];
                ਜੇ(c<0xfffe) {
                    /* output BMP code point */
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(c==0xfffe) {
                    ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv) && (c=ucnv_MBCSGetFallback(&cnv->shared_ਅੰਕੜੇ->mbcs, ਵਿੱਥ))!=0xfffe) {
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* callback(illegal) */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_VALID_16_PAIR) {
                ਵਿੱਥ+=MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
                c=unicode_ਕੋਡ_Units[ਵਿੱਥ++];
                ਜੇ(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_ਕੋਡ_Units[ਵਿੱਥ]+(0x10000-0xdc00);
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_ਕੋਡ_Units[ਵਿੱਥ];
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(c==0xffff) {
                    /* callback(illegal) */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_VALID_ਸਿੱਧਾ_20 ||
                      (action==MBCS_STATE_FALLBACK_ਸਿੱਧਾ_20 && UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv))
            ) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ(ਇੰਦਰਾਜ) + 0x10000);
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_CHANGE_ONLY) {
                /*
                 * This serves as a state change without any output.
                 * It is useful for reading simple stateful encodings,
                 * for example using just Shift-In/Shift-Out codes.
                 * The 21 unused bits may later be used for more sophisticated
                 * state transitions.
                 */
                ਜੇ(cnv->shared_ਅੰਕੜੇ->mbcs.dbcsOnlyState!=0) {
                    /* SI/SO are illegal for DBCS-only conversion */
                    state = static_cast<uint8_t>(cnv->ਬਹੁਲਕ); /* restore the previous state */

                    /* callback(illegal) */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_FALLBACK_ਸਿੱਧਾ_16) {
                ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv)) {
                    /* output BMP code point */
                    c = static_cast<char16_t>(MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ));
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_UNASSIGNED) {
                /* just fall through */
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_ILLEGAL) {
                /* callback(illegal) */
                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
            } ਨਹੀਂਤਾਂ {
                /* reserved (must never occur), or only state change */
                ਵਿੱਥ=0;
                ਆਖਰੀ_ਸਰੋਤ=ਸਰੋਤ;
                ਜਾਰੀ;
            }

            /* end of action codes: prepare for a new character */
            ਵਿੱਥ=0;

            ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                /* callback(illegal) */
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ /* unassigned sequence */ {
                /* defer to the generic implementation */
                cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=0;
                cnv->ਬਹੁਲਕ=state;
                p_ਦਲੀਲਾਂ->ਸਰੋਤ = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਆਖਰੀ_ਸਰੋਤ);
                ਵਾਪਸ UCNV_GET_ਅਗਲਾ_UCHAR_USE_ਮੰਜ਼ਿਲ_U;
            }
        }
    }

    ਜੇ(c<0) {
        ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ) && ਸਰੋਤ==ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਆਖਰੀ_ਸਰੋਤ<ਸਰੋਤ) {
            /* incomplete character byte sequence */
            cnv->ਮੰਜ਼ਿਲ_ULength = static_cast<int8_t>(ਸਰੋਤ - ਆਖਰੀ_ਸਰੋਤ);
            uprv_memcpy(cnv->ਮੰਜ਼ਿਲ_UBytes, ਆਖਰੀ_ਸਰੋਤ, cnv->ਮੰਜ਼ਿਲ_ULength);
            *p_Error_ਕੋਡ=U_TRUNCATED_CHAR_ਮਿਲਿਆ;
        } ਨਹੀਂਤਾਂ ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            /* callback(illegal) */
            /*
             * Ticket 5691: consistent illegal sequences:
             * - We include at least the first byte in the illegal sequence.
             * - If any of the non-initial bytes could be the start of a character,
             *   we stop the illegal sequence before the first one of those.
             */
            UBool isDBCSOnly = static_cast<UBool>(cnv->shared_ਅੰਕੜੇ->mbcs.dbcsOnlyState != 0);
            uint8_t *ਬਾਈਟਾਂ=cnv->ਮੰਜ਼ਿਲ_UBytes;
            *ਬਾਈਟਾਂ++=*ਆਖਰੀ_ਸਰੋਤ++;     /* first byte */
            ਜੇ(ਆਖਰੀ_ਸਰੋਤ==ਸਰੋਤ) {
                cnv->ਮੰਜ਼ਿਲ_ULength=1;
            } ਨਹੀਂਤਾਂ /* lastSource<source: multi-byte character */ {
                int8_t i;
                ਲਈ(i=1;
                    ਆਖਰੀ_ਸਰੋਤ<ਸਰੋਤ && !isSingleOrLead(state_ਸਾਰਣੀ, state, isDBCSOnly, *ਆਖਰੀ_ਸਰੋਤ);
                    ++i
                ) {
                    *ਬਾਈਟਾਂ++=*ਆਖਰੀ_ਸਰੋਤ++;
                }
                cnv->ਮੰਜ਼ਿਲ_ULength=i;
                ਸਰੋਤ=ਆਖਰੀ_ਸਰੋਤ;
            }
        } ਨਹੀਂਤਾਂ {
            /* no output because of empty input or only state changes */
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        }
        c=0xffff;
    }

    /* set the converter state back into UConverter, ready for a new character */
    cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=0;
    cnv->ਬਹੁਲਕ=state;

    /* write back the updated pointer */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਸਰੋਤ);
    ਵਾਪਸ c;
}

#ਜੇ 0
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
ucnv_MBCSSingle_Simple_Get_ਅਗਲਾ_UChar(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ,
                              uint8_t b, UBool useFallback) {
    int32_t ਇੰਦਰਾਜ;
    uint8_t action;

    ਇੰਦਰਾਜ=shared_ਅੰਕੜੇ->mbcs.state_ਸਾਰਣੀ[0][b];
    /* MBCS_ENTRY_IS_FINAL(entry) */

    ਜੇ(MBCS_ਇੰਦਰਾਜ_FINAL_IS_VALID_ਸਿੱਧਾ_16(ਇੰਦਰਾਜ)) {
        /* output BMP code point */
        ਵਾਪਸ (char16_t)MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
    }

    /*
     * An if-else-if chain provides more reliable performance for
     * the most common cases compared to a switch.
     */
    action=(uint8_t)(MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ));
    ਜੇ(action==MBCS_STATE_VALID_ਸਿੱਧਾ_20) {
        /* output supplementary code point */
        ਵਾਪਸ 0x10000+MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ(ਇੰਦਰਾਜ);
    } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_FALLBACK_ਸਿੱਧਾ_16) {
        ਜੇ(!ਮੰਜ਼ਿਲ_U_USE_FALLBACK(useFallback)) {
            ਵਾਪਸ 0xfffe;
        }
        /* output BMP code point */
        ਵਾਪਸ (char16_t)MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
    } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_FALLBACK_ਸਿੱਧਾ_20) {
        ਜੇ(!ਮੰਜ਼ਿਲ_U_USE_FALLBACK(useFallback)) {
            ਵਾਪਸ 0xfffe;
        }
        /* output supplementary code point */
        ਵਾਪਸ 0x10000+MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ(ਇੰਦਰਾਜ);
    } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_UNASSIGNED) {
        ਵਾਪਸ 0xfffe;
    } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_ILLEGAL) {
        ਵਾਪਸ 0xffff;
    } ਨਹੀਂਤਾਂ {
        /* reserved, must never occur */
        ਵਾਪਸ 0xffff;
    }
}
#ਜੇ_ਅੰਤ

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
ucnv_MBCSSimple_Get_ਅਗਲਾ_UChar(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ,
                        ਅਚਲ ਅੱਖਰ *ਸਰੋਤ, int32_t length,
                        UBool useFallback) {
    ਅਚਲ int32_t (*state_ਸਾਰਣੀ)[256];
    ਅਚਲ uint16_t *unicode_ਕੋਡ_Units;

    uint32_t ਵਿੱਥ;
    uint8_t state, action;

    UChar32 c;
    int32_t i, ਇੰਦਰਾਜ;

    ਜੇ(length<=0) {
        /* no input at all: "illegal" */
        ਵਾਪਸ 0xffff;
    }

#ਜੇ 0
/*
 * Code disabled 2002dec09 (ICU 2.4) because it is not currently used in ICU. markus
 * TODO In future releases, verify that this function is never called for SBCS
 * conversions, i.e., that sharedData->mbcs.countStates==1 is still true.
 * Removal improves code coverage.
 */
    /* use optimized function if possible */
    ਜੇ(shared_ਅੰਕੜੇ->mbcs.ਗਿਣਤੀ_ਹਾਲਤਾਂ==1) {
        ਜੇ(length==1) {
            ਵਾਪਸ ucnv_MBCSSingle_Simple_Get_ਅਗਲਾ_UChar(shared_ਅੰਕੜੇ, (uint8_t)*ਸਰੋਤ, useFallback);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ 0xffff; /* illegal: more than a single byte for an SBCS converter */
        }
    }
#ਜੇ_ਅੰਤ

    /* set up the local pointers */
    state_ਸਾਰਣੀ=shared_ਅੰਕੜੇ->mbcs.state_ਸਾਰਣੀ;
    unicode_ਕੋਡ_Units=shared_ਅੰਕੜੇ->mbcs.unicode_ਕੋਡ_Units;

    /* converter state */
    ਵਿੱਥ=0;
    state=shared_ਅੰਕੜੇ->mbcs.dbcsOnlyState;

    /* conversion loop */
    ਲਈ(i=0;;) {
        ਇੰਦਰਾਜ=state_ਸਾਰਣੀ[state][(uint8_t)ਸਰੋਤ[i++]];
        ਜੇ(MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(ਇੰਦਰਾਜ)) {
            state=(uint8_t)MBCS_ਇੰਦਰਾਜ_TRANSITION_STATE(ਇੰਦਰਾਜ);
            ਵਿੱਥ+=MBCS_ਇੰਦਰਾਜ_TRANSITION_ਵਿੱਥ(ਇੰਦਰਾਜ);

            ਜੇ(i==length) {
                ਵਾਪਸ 0xffff; /* truncated character */
            }
        } ਨਹੀਂਤਾਂ {
            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=(uint8_t)(MBCS_ਇੰਦਰਾਜ_FINAL_ACTION(ਇੰਦਰਾਜ));
            ਜੇ(action==MBCS_STATE_VALID_16) {
                ਵਿੱਥ+=MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
                c=unicode_ਕੋਡ_Units[ਵਿੱਥ];
                ਜੇ(c!=0xfffe) {
                    /*ਪੂਰਾ*/
                } ਨਹੀਂਤਾਂ ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv)) {
                    c=ucnv_MBCSGetFallback(&shared_ਅੰਕੜੇ->mbcs, ਵਿੱਥ);
                /* else done with 0xfffe */
                }
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_VALID_ਸਿੱਧਾ_16) {
                /* output BMP code point */
                c=(char16_t)MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_VALID_16_PAIR) {
                ਵਿੱਥ+=MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
                c=unicode_ਕੋਡ_Units[ਵਿੱਥ++];
                ਜੇ(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } ਨਹੀਂਤਾਂ ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=(UChar32)(((c&0x3ff)<<10)+unicode_ਕੋਡ_Units[ਵਿੱਥ]+(0x10000-0xdc00));
                } ਨਹੀਂਤਾਂ ਜੇ(UCNV_ਮੰਜ਼ਿਲ_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_ਕੋਡ_Units[ਵਿੱਥ];
                } ਨਹੀਂਤਾਂ ਜੇ(c==0xffff) {
                    ਵਾਪਸ 0xffff;
                } ਨਹੀਂਤਾਂ {
                    c=0xfffe;
                }
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_VALID_ਸਿੱਧਾ_20) {
                /* output supplementary code point */
                c=0x10000+MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ(ਇੰਦਰਾਜ);
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_FALLBACK_ਸਿੱਧਾ_16) {
                ਜੇ(!ਮੰਜ਼ਿਲ_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    ਰੋਕੋ;
                }
                /* output BMP code point */
                c=(char16_t)MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ_16(ਇੰਦਰਾਜ);
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_FALLBACK_ਸਿੱਧਾ_20) {
                ਜੇ(!ਮੰਜ਼ਿਲ_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    ਰੋਕੋ;
                }
                /* output supplementary code point */
                c=0x10000+MBCS_ਇੰਦਰਾਜ_FINAL_ਮੁੱਲ(ਇੰਦਰਾਜ);
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(action==MBCS_STATE_UNASSIGNED) {
                c=0xfffe;
                ਰੋਕੋ;
            }

            /*
             * forbid MBCS_STATE_CHANGE_ONLY for this function,
             * and MBCS_STATE_ILLEGAL and reserved action codes
             */
            ਵਾਪਸ 0xffff;
        }
    }

    ਜੇ(i!=length) {
        /* illegal for this function: not all input consumed */
        ਵਾਪਸ 0xffff;
    }

    ਜੇ(c==0xfffe) {
        /* try an extension mapping */
        ਅਚਲ int32_t *cx=shared_ਅੰਕੜੇ->mbcs.extIndexes;
        ਜੇ(cx!=ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ ucnv_ext_Simple_ਮੇਲ_ਮੰਜ਼ਿਲ_U(cx, ਸਰੋਤ, length, useFallback);
        }
    }

    ਵਾਪਸ c;
}

/* MBCS-from-Unicode conversion functions ----------------------------------- */

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for double-byte codepages. */
ਸਥਿਰ ਖਾਲੀ
ucnv_MBCSDoubleFromUnicodeWithOffsets(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ char16_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    int32_t *offsets;

    ਅਚਲ uint16_t *ਸਾਰਣੀ;
    ਅਚਲ uint16_t *mbcs_ਸੂਚਕਾਂਕ;
    ਅਚਲ uint8_t *ਬਾਈਟਾਂ;

    UChar32 c;

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ, ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    uint32_t stage2_ਇੰਦਰਾਜ;
    uint32_t asciiRoundtrips;
    uint32_t value;
    uint8_t unicode_ਨਕਾਬ;

    /* use optimized function if possible */
    cnv=p_ਦਲੀਲਾਂ->converter;
    unicode_ਨਕਾਬ=cnv->shared_ਅੰਕੜੇ->mbcs.unicode_ਨਕਾਬ;

    /* set up the local pointers */
    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ = reinterpret_cast<uint8_t*>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;

    ਸਾਰਣੀ=cnv->shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਸਾਰਣੀ;
    mbcs_ਸੂਚਕਾਂਕ=cnv->shared_ਅੰਕੜੇ->mbcs.mbcs_ਸੂਚਕਾਂਕ;
    ਜੇ((cnv->options&UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL)!=0) {
        ਬਾਈਟਾਂ=cnv->shared_ਅੰਕੜੇ->mbcs.ਅਦਲਾ_ਬਦਲੀ_LFNLFrom_Unicode_ਬਾਈਟਾਂ;
    } ਨਹੀਂਤਾਂ {
        ਬਾਈਟਾਂ=cnv->shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ;
    }
    asciiRoundtrips=cnv->shared_ਅੰਕੜੇ->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ= c==0 ? 0 : -1;
    ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    /* conversion loop */
    ਜੇ(c!=0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
        ਜਾਓ getTrail;
    }

    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ਸਰੋਤ++;
            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਜੇ(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(c);
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                c=0;
                ਜਾਰੀ;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            ਜੇ(c<=0xd7ff) {
                value=DBCS_ਨਤੀਜਾ_FROM_MOST_BMP(mbcs_ਸੂਚਕਾਂਕ, (ਅਚਲ uint16_t *)ਬਾਈਟਾਂ, c);
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                ਜੇ(value==0) {
                    ਜਾਓ unassigned;
                }
                /* output the value */
            } ਨਹੀਂਤਾਂ {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                ਜੇ(U16_IS_SURROGATE(c) && !(unicode_ਨਕਾਬ&UCNV_HAS_SURROGATES)) {
                    ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                            /* test the following code unit */
                            char16_t trail=*ਸਰੋਤ;
                            ਜੇ(U16_IS_TRAIL(trail)) {
                                ++ਸਰੋਤ;
                                ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                ਜੇ(!(unicode_ਨਕਾਬ&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    /* callback(unassigned) */
                                    ਜਾਓ unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } ਨਹੀਂਤਾਂ {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                                ਰੋਕੋ;
                            }
                        } ਨਹੀਂਤਾਂ {
                            /* no more input */
                            ਰੋਕੋ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਰੋਕੋ;
                    }
                }

                /* convert the Unicode code point in c into codepage bytes */
                stage2_ਇੰਦਰਾਜ=MBCS_STAGE_2_FROM_U(ਸਾਰਣੀ, c);

                /* get the bytes and the length for the output */
                /* MBCS_OUTPUT_2 */
                value=MBCS_ਮੁੱਲ_2_FROM_STAGE_2(ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);

                /* is this code point assigned, or do we use fallbacks? */
                ਜੇ(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_ਇੰਦਰਾਜ, c) ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
                    c=_extFromU(cnv, cnv->shared_ਅੰਕੜੇ,
                                c, &ਸਰੋਤ, ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                                &ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ+ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                                &offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                                p_ਦਲੀਲਾਂ->flush,
                                p_Error_ਕੋਡ);
                    ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ += static_cast<int32_t>(ਸਰੋਤ - p_ਦਲੀਲਾਂ->ਸਰੋਤ);

                    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                        /* not mappable or buffer overflow */
                        ਰੋਕੋ;
                    } ਨਹੀਂਤਾਂ {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ));

                        /* normal end of conversion: prepare for a new character */
                        ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        ਜਾਰੀ;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            ਜੇ(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(value);
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
            } ਨਹੀਂਤਾਂ /* length==2 */ {
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(value >> 8);
                ਜੇ(2<=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
                    *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(value);
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=2;
                } ਨਹੀਂਤਾਂ {
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                    cnv->char_Error_ਬਫਰ[0] = static_cast<ਅੱਖਰ>(value);
                    cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=1;

                    /* target overflow */
                    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=0;
                    *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    c=0;
                    ਰੋਕੋ;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for single-byte codepages. */
ਸਥਿਰ ਖਾਲੀ
ucnv_MBCSSingleFromUnicodeWithOffsets(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ char16_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    int32_t *offsets;

    ਅਚਲ uint16_t *ਸਾਰਣੀ;
    ਅਚਲ uint16_t *results;

    UChar32 c;

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ, ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    uint16_t value, min_ਮੁੱਲ;
    UBool hasSupplementary;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ = reinterpret_cast<uint8_t*>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;

    ਸਾਰਣੀ=cnv->shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਸਾਰਣੀ;
    ਜੇ((cnv->options&UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_ਅੰਕੜੇ->mbcs.ਅਦਲਾ_ਬਦਲੀ_LFNLFrom_Unicode_ਬਾਈਟਾਂ);
    } ਨਹੀਂਤਾਂ {
        results=(uint16_t *)cnv->shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ;
    }

    ਜੇ(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_ਮੁੱਲ=0x800;
    } ਨਹੀਂਤਾਂ {
        /* use only roundtrips and fallbacks from private-use characters */
        min_ਮੁੱਲ=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_ਅੰਕੜੇ->mbcs.unicode_ਨਕਾਬ & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ= c==0 ? 0 : -1;
    ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    /* conversion loop */
    ਜੇ(c!=0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
        ਜਾਓ getTrail;
    }

    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ਸਰੋਤ++;
            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਜੇ(U16_IS_SURROGATE(c)) {
                ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                    ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                        /* test the following code unit */
                        char16_t trail=*ਸਰੋਤ;
                        ਜੇ(U16_IS_TRAIL(trail)) {
                            ++ਸਰੋਤ;
                            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            ਜੇ(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                /* callback(unassigned) */
                                ਜਾਓ unassigned;
                            }
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } ਨਹੀਂਤਾਂ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            ਰੋਕੋ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* no more input */
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    ਰੋਕੋ;
                }
            }

            /* convert the Unicode code point in c into codepage bytes */
            value=MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, results, c);

            /* is this code point assigned, or do we use fallbacks? */
            ਜੇ(value>=min_ਮੁੱਲ) {
                /* assigned, write the output character bytes from value and length */
                /* length==1 */
                /* this is easy because we know that there is enough space */
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(value);
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;

                /* normal end of conversion: prepare for a new character */
                c=0;
                ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            } ਨਹੀਂਤਾਂ { /* unassigned */
unassigned:
                /* try an extension mapping */
                p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
                c=_extFromU(cnv, cnv->shared_ਅੰਕੜੇ,
                            c, &ਸਰੋਤ, ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                            &ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ+ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                            &offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                            p_ਦਲੀਲਾਂ->flush,
                            p_Error_ਕੋਡ);
                ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ += static_cast<int32_t>(ਸਰੋਤ - p_ਦਲੀਲਾਂ->ਸਰੋਤ);

                ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                    /* not mappable or buffer overflow */
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ));

                    /* normal end of conversion: prepare for a new character */
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
            }
        } ਨਹੀਂਤਾਂ {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
    p_ਦਲੀਲਾਂ->offsets=offsets;
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
ਸਥਿਰ ਖਾਲੀ
ucnv_MBCSSingleFromBMPWithOffsets(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                              UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ char16_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ, *ਆਖਰੀ_ਸਰੋਤ;
    uint8_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, length;
    int32_t *offsets;

    ਅਚਲ uint16_t *ਸਾਰਣੀ;
    ਅਚਲ uint16_t *results;

    UChar32 c;

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    uint32_t asciiRoundtrips;
    uint16_t value, min_ਮੁੱਲ;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ = reinterpret_cast<uint8_t*>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;

    ਸਾਰਣੀ=cnv->shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਸਾਰਣੀ;
    ਜੇ((cnv->options&UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_ਅੰਕੜੇ->mbcs.ਅਦਲਾ_ਬਦਲੀ_LFNLFrom_Unicode_ਬਾਈਟਾਂ);
    } ਨਹੀਂਤਾਂ {
        results=(uint16_t *)cnv->shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ;
    }
    asciiRoundtrips=cnv->shared_ਅੰਕੜੇ->mbcs.asciiRoundtrips;

    ਜੇ(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_ਮੁੱਲ=0x800;
    } ਨਹੀਂਤਾਂ {
        /* use only roundtrips and fallbacks from private-use characters */
        min_ਮੁੱਲ=0xc00;
    }

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ= c==0 ? 0 : -1;
    ਆਖਰੀ_ਸਰੋਤ=ਸਰੋਤ;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(ਸਰੋਤ_ਹੱਦਬੰਦੀ - ਸਰੋਤ);
    ਜੇ(length<ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=length;
    }

    /* conversion loop */
    ਜੇ(c!=0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
        ਜਾਓ getTrail;
    }

#ਜੇ MBCS_UNROLL_SINGLE_FROM_BMP
    /* unrolling makes it slower on Pentium III/Windows 2000?! */
    /* unroll the loop with the most common case */
unrolled:
    ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>=4) {
        int32_t count, loops;
        uint16_t anded_ਮੁੱਲਾਂ;

        loops=count=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>>2;
        ਕਰੋ {
            c=*ਸਰੋਤ++;
            anded_ਮੁੱਲਾਂ=value=MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, results, c);
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)value;
            c=*ਸਰੋਤ++;
            anded_ਮੁੱਲਾਂ&=value=MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, results, c);
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)value;
            c=*ਸਰੋਤ++;
            anded_ਮੁੱਲਾਂ&=value=MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, results, c);
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)value;
            c=*ਸਰੋਤ++;
            anded_ਮੁੱਲਾਂ&=value=MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, results, c);
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)value;

            /* were all 4 entries really valid? */
            ਜੇ(anded_ਮੁੱਲਾਂ<min_ਮੁੱਲ) {
                /* no, return to the first of these 4 */
                ਸਰੋਤ-=4;
                ਨਿਸ਼ਾਨਾ-=4;
                ਰੋਕੋ;
            }
        } ਜਦੋਂ(--count>0);
        count=loops-count;
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=4*count;

        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
            ਆਖਰੀ_ਸਰੋਤ+=4*count;
            ਜਦੋਂ(count>0) {
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                --count;
            }
        }

        c=0;
    }
#ਜੇ_ਅੰਤ

    ਜਦੋਂ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
        /*
         * Get a correct Unicode code point:
         * a single char16_t for a BMP code point or
         * a matched surrogate pair for a "supplementary code point".
         */
        c=*ਸਰੋਤ++;
        /*
         * Do not immediately check for single surrogates:
         * Assume that they are unassigned and check for them in that case.
         * This speeds up the conversion of assigned characters.
         */
        /* convert the Unicode code point in c into codepage bytes */
        ਜੇ(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
            *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(c);
            --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
            c=0;
            ਜਾਰੀ;
        }
        value=MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, results, c);
        /* is this code point assigned, or do we use fallbacks? */
        ਜੇ(value>=min_ਮੁੱਲ) {
            /* assigned, write the output character bytes from value and length */
            /* length==1 */
            /* this is easy because we know that there is enough space */
            *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(value);
            --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;

            /* normal end of conversion: prepare for a new character */
            c=0;
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ ਜੇ(!U16_IS_SURROGATE(c)) {
            /* normal, unassigned BMP character */
        } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
            ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                /* test the following code unit */
                char16_t trail=*ਸਰੋਤ;
                ਜੇ(U16_IS_TRAIL(trail)) {
                    ++ਸਰੋਤ;
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } ਨਹੀਂਤਾਂ {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ {
                /* no more input */
                ਜੇ (p_ਦਲੀਲਾਂ->flush) {
                    *p_Error_ਕੋਡ=U_TRUNCATED_CHAR_ਮਿਲਿਆ;
                }
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
            ਰੋਕੋ;
        }

        /* c does not have a mapping */

        /* get the number of code units for c to correctly advance sourceIndex */
        length=U16_ਲੰਮਾਈ(c);

        /* set offsets since the start or the last extension */
        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
            int32_t count = static_cast<int32_t>(ਸਰੋਤ - ਆਖਰੀ_ਸਰੋਤ);

            /* do not set the offset for this character */
            count-=length;

            ਜਦੋਂ(count>0) {
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                --count;
            }
            /* offsets and sourceIndex are now set for the current character */
        }

        /* try an extension mapping */
        ਆਖਰੀ_ਸਰੋਤ=ਸਰੋਤ;
        c=_extFromU(cnv, cnv->shared_ਅੰਕੜੇ,
                    c, &ਸਰੋਤ, ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                    &ਨਿਸ਼ਾਨਾ, reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ),
                    &offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                    p_ਦਲੀਲਾਂ->flush,
                    p_Error_ਕੋਡ);
        ਸਰੋਤ_ਸੂਚਕਾਂਕ += length + static_cast<int32_t>(ਸਰੋਤ - ਆਖਰੀ_ਸਰੋਤ);
        ਆਖਰੀ_ਸਰੋਤ=ਸਰੋਤ;

        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            /* not mappable or buffer overflow */
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ {
            /* a mapping was written to the target, continue */

            /* recalculate the targetCapacity after an extension mapping */
            ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ));
            length = static_cast<int32_t>(ਸਰੋਤ_ਹੱਦਬੰਦੀ - ਸਰੋਤ);
            ਜੇ(length<ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
                ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=length;
            }
        }

#ਜੇ MBCS_UNROLL_SINGLE_FROM_BMP
        /* unrolling makes it slower on Pentium III/Windows 2000?! */
        ਜਾਓ unrolled;
#ਜੇ_ਅੰਤ
    }

    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ) && ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ>=(uint8_t *)p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        /* target is full */
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    /* set offsets since the start or the last callback */
    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
        size_t count=ਸਰੋਤ-ਆਖਰੀ_ਸਰੋਤ;
        ਜੇ (count > 0 && *p_Error_ਕੋਡ == U_TRUNCATED_CHAR_ਮਿਲਿਆ) {
            /*
            Caller gave us a partial supplementary character,
            which this function couldn't convert in any case.
            The callback will handle the offset.
            */
            count--;
        }
        ਜਦੋਂ(count>0) {
            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
            --count;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

U_CFUNC ਖਾਲੀ
ucnv_MBCSFromUnicodeWithOffsets(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                            UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ char16_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    int32_t *offsets;

    ਅਚਲ uint16_t *ਸਾਰਣੀ;
    ਅਚਲ uint16_t *mbcs_ਸੂਚਕਾਂਕ;
    ਅਚਲ uint8_t *p, *ਬਾਈਟਾਂ;
    uint8_t outputType;

    UChar32 c;

    int32_t prev_ਸਰੋਤ_ਸੂਚਕਾਂਕ, ਸਰੋਤ_ਸੂਚਕਾਂਕ, ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    uint32_t stage2_ਇੰਦਰਾਜ;
    uint32_t asciiRoundtrips;
    uint32_t value;
    /* Shift-In and Shift-Out byte sequences differ by encoding scheme. */
    uint8_t si_ਬਾਈਟਾਂ[2] = {0, 0};
    uint8_t so_ਬਾਈਟਾਂ[2] = {0, 0};
    uint8_t si_ਲੰਮਾਈ, so_ਲੰਮਾਈ;
    int32_t length = 0, prev_ਲੰਮਾਈ;
    uint8_t unicode_ਨਕਾਬ;

    cnv=p_ਦਲੀਲਾਂ->converter;

    ਜੇ(cnv->preFromUFirstCP>=0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_ਮੇਲ_From_U(cnv, p_ਦਲੀਲਾਂ, -1, p_Error_ਕੋਡ);

        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ) || cnv->preFromULength<0) {
            ਵਾਪਸ;
        }
    }

    /* use optimized function if possible */
    outputType=cnv->shared_ਅੰਕੜੇ->mbcs.outputType;
    unicode_ਨਕਾਬ=cnv->shared_ਅੰਕੜੇ->mbcs.unicode_ਨਕਾਬ;
    ਜੇ(outputType==MBCS_OUTPUT_1 && !(unicode_ਨਕਾਬ&UCNV_HAS_SURROGATES)) {
        ਜੇ(!(unicode_ਨਕਾਬ&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingleFromBMPWithOffsets(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ {
            ucnv_MBCSSingleFromUnicodeWithOffsets(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
        }
        ਵਾਪਸ;
    } ਨਹੀਂਤਾਂ ਜੇ(outputType==MBCS_OUTPUT_2 && cnv->shared_ਅੰਕੜੇ->mbcs.utf8Friendly) {
        ucnv_MBCSDoubleFromUnicodeWithOffsets(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
        ਵਾਪਸ;
    }

    /* set up the local pointers */
    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=(uint8_t *)p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;

    ਸਾਰਣੀ=cnv->shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਸਾਰਣੀ;
    ਜੇ(cnv->shared_ਅੰਕੜੇ->mbcs.utf8Friendly) {
        mbcs_ਸੂਚਕਾਂਕ=cnv->shared_ਅੰਕੜੇ->mbcs.mbcs_ਸੂਚਕਾਂਕ;
    } ਨਹੀਂਤਾਂ {
        mbcs_ਸੂਚਕਾਂਕ=ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ((cnv->options&UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL)!=0) {
        ਬਾਈਟਾਂ=cnv->shared_ਅੰਕੜੇ->mbcs.ਅਦਲਾ_ਬਦਲੀ_LFNLFrom_Unicode_ਬਾਈਟਾਂ;
    } ਨਹੀਂਤਾਂ {
        ਬਾਈਟਾਂ=cnv->shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ;
    }
    asciiRoundtrips=cnv->shared_ਅੰਕੜੇ->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    ਜੇ(outputType==MBCS_OUTPUT_2_SISO) {
        prev_ਲੰਮਾਈ=cnv->from_Unicode_ਸਥਿਤੀ;
        ਜੇ(prev_ਲੰਮਾਈ==0) {
            /* set the real value */
            prev_ਲੰਮਾਈ=1;
        }
    } ਨਹੀਂਤਾਂ {
        /* prevent fromUnicodeStatus from being set to something non-0 */
        prev_ਲੰਮਾਈ=0;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    prev_ਸਰੋਤ_ਸੂਚਕਾਂਕ=-1;
    ਸਰੋਤ_ਸੂਚਕਾਂਕ= c==0 ? 0 : -1;
    ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    /* Get the SI/SO character for the converter */
    si_ਲੰਮਾਈ = static_cast<uint8_t>(getSISOBytes(SI, cnv->options, si_ਬਾਈਟਾਂ));
    so_ਲੰਮਾਈ = static_cast<uint8_t>(getSISOBytes(SO, cnv->options, so_ਬਾਈਟਾਂ));

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
    ਜੇ(c!=0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
        ਜਾਓ getTrail;
    }

    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ਸਰੋਤ++;
            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਜੇ(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    prev_ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                c=0;
                ਜਾਰੀ;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            ਜੇ(c<=0xd7ff && mbcs_ਸੂਚਕਾਂਕ!=ਖਾਲੀ_ਸੂਚਕ) {
                value=mbcs_ਸੂਚਕਾਂਕ[c>>6];

                /* get the bytes and the length for the output (copied from below and adapted for utf8Friendly data) */
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                ਚੋਣ(outputType) {
                ਹਾਲਤ MBCS_OUTPUT_2:
                    value=((ਅਚਲ uint16_t *)ਬਾਈਟਾਂ)[value +(c&0x3f)];
                    ਜੇ(value<=0xff) {
                        ਜੇ(value==0) {
                            ਜਾਓ unassigned;
                        } ਨਹੀਂਤਾਂ {
                            length=1;
                        }
                    } ਨਹੀਂਤਾਂ {
                        length=2;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ MBCS_OUTPUT_2_SISO:
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
                    cnv->from_Unicode_ਸਥਿਤੀ=prev_ਲੰਮਾਈ; /* save the old state */
                    value=((ਅਚਲ uint16_t *)ਬਾਈਟਾਂ)[value +(c&0x3f)];
                    ਜੇ(value<=0xff) {
                        ਜੇ(value==0) {
                            ਜਾਓ unassigned;
                        } ਨਹੀਂਤਾਂ ਜੇ(prev_ਲੰਮਾਈ<=1) {
                            length=1;
                        } ਨਹੀਂਤਾਂ {
                            /* change from double-byte mode to single-byte */
                            ਜੇ (si_ਲੰਮਾਈ == 1) {
                                value|=(uint32_t)si_ਬਾਈਟਾਂ[0]<<8;
                                length = 2;
                            } ਨਹੀਂਤਾਂ ਜੇ (si_ਲੰਮਾਈ == 2) {
                                value|=(uint32_t)si_ਬਾਈਟਾਂ[1]<<8;
                                value|=(uint32_t)si_ਬਾਈਟਾਂ[0]<<16;
                                length = 3;
                            }
                            prev_ਲੰਮਾਈ=1;
                        }
                    } ਨਹੀਂਤਾਂ {
                        ਜੇ(prev_ਲੰਮਾਈ==2) {
                            length=2;
                        } ਨਹੀਂਤਾਂ {
                            /* change from single-byte mode to double-byte */
                            ਜੇ (so_ਲੰਮਾਈ == 1) {
                                value|=(uint32_t)so_ਬਾਈਟਾਂ[0]<<16;
                                length = 3;
                            } ਨਹੀਂਤਾਂ ਜੇ (so_ਲੰਮਾਈ == 2) {
                                value|=(uint32_t)so_ਬਾਈਟਾਂ[1]<<16;
                                value|=(uint32_t)so_ਬਾਈਟਾਂ[0]<<24;
                                length = 4;
                            }
                            prev_ਲੰਮਾਈ=2;
                        }
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=((ਅਚਲ uint16_t *)ਬਾਈਟਾਂ)[value +(c&0x3f)];
                    ਜੇ(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        ਜਾਓ unassigned;
                    } ਨਹੀਂਤਾਂ {
                        length=2;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ MBCS_OUTPUT_3:
                    p=ਬਾਈਟਾਂ+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    ਜੇ(value<=0xff) {
                        ਜੇ(value==0) {
                            ਜਾਓ unassigned;
                        } ਨਹੀਂਤਾਂ {
                            length=1;
                        }
                    } ਨਹੀਂਤਾਂ ਜੇ(value<=0xffff) {
                        length=2;
                    } ਨਹੀਂਤਾਂ {
                        length=3;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ MBCS_OUTPUT_4:
                    value=((ਅਚਲ uint32_t *)ਬਾਈਟਾਂ)[value +(c&0x3f)];
                    ਜੇ(value<=0xff) {
                        ਜੇ(value==0) {
                            ਜਾਓ unassigned;
                        } ਨਹੀਂਤਾਂ {
                            length=1;
                        }
                    } ਨਹੀਂਤਾਂ ਜੇ(value<=0xffff) {
                        length=2;
                    } ਨਹੀਂਤਾਂ ਜੇ(value<=0xffffff) {
                        length=3;
                    } ਨਹੀਂਤਾਂ {
                        length=4;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ MBCS_OUTPUT_3_EUC:
                    value=((ਅਚਲ uint16_t *)ਬਾਈਟਾਂ)[value +(c&0x3f)];
                    /* EUC 16-bit fixed-length representation */
                    ਜੇ(value<=0xff) {
                        ਜੇ(value==0) {
                            ਜਾਓ unassigned;
                        } ਨਹੀਂਤਾਂ {
                            length=1;
                        }
                    } ਨਹੀਂਤਾਂ ਜੇ((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } ਨਹੀਂਤਾਂ ਜੇ((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } ਨਹੀਂਤਾਂ {
                        length=2;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ MBCS_OUTPUT_4_EUC:
                    p=ਬਾਈਟਾਂ+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    ਜੇ(value<=0xff) {
                        ਜੇ(value==0) {
                            ਜਾਓ unassigned;
                        } ਨਹੀਂਤਾਂ {
                            length=1;
                        }
                    } ਨਹੀਂਤਾਂ ਜੇ(value<=0xffff) {
                        length=2;
                    } ਨਹੀਂਤਾਂ ਜੇ((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } ਨਹੀਂਤਾਂ ਜੇ((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } ਨਹੀਂਤਾਂ {
                        length=3;
                    }
                    ਰੋਕੋ;
                ਮੂਲ:
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
                    ਰੋਕੋ;
                }
                /* output the value */
            } ਨਹੀਂਤਾਂ {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                ਜੇ(U16_IS_SURROGATE(c) && !(unicode_ਨਕਾਬ&UCNV_HAS_SURROGATES)) {
                    ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                            /* test the following code unit */
                            char16_t trail=*ਸਰੋਤ;
                            ਜੇ(U16_IS_TRAIL(trail)) {
                                ++ਸਰੋਤ;
                                ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                ਜੇ(!(unicode_ਨਕਾਬ&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    cnv->from_Unicode_ਸਥਿਤੀ=prev_ਲੰਮਾਈ; /* save the old state */
                                    /* callback(unassigned) */
                                    ਜਾਓ unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } ਨਹੀਂਤਾਂ {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                                ਰੋਕੋ;
                            }
                        } ਨਹੀਂਤਾਂ {
                            /* no more input */
                            ਰੋਕੋ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਰੋਕੋ;
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
                stage2_ਇੰਦਰਾਜ=MBCS_STAGE_2_FROM_U(ਸਾਰਣੀ, c);

                /* get the bytes and the length for the output */
                ਚੋਣ(outputType) {
                ਹਾਲਤ MBCS_OUTPUT_2:
                    value=MBCS_ਮੁੱਲ_2_FROM_STAGE_2(ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
                    ਜੇ(value<=0xff) {
                        length=1;
                    } ਨਹੀਂਤਾਂ {
                        length=2;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ MBCS_OUTPUT_2_SISO:
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
                    cnv->from_Unicode_ਸਥਿਤੀ=prev_ਲੰਮਾਈ; /* save the old state */
                    value=MBCS_ਮੁੱਲ_2_FROM_STAGE_2(ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
                    ਜੇ(value<=0xff) {
                        ਜੇ(value==0 && MBCS_FROM_U_IS_ROUNDTRIP(stage2_ਇੰਦਰਾਜ, c)==0) {
                            /* no mapping, leave value==0 */
                            length=0;
                        } ਨਹੀਂਤਾਂ ਜੇ(prev_ਲੰਮਾਈ<=1) {
                            length=1;
                        } ਨਹੀਂਤਾਂ {
                            /* change from double-byte mode to single-byte */
                            ਜੇ (si_ਲੰਮਾਈ == 1) {
                                value|=(uint32_t)si_ਬਾਈਟਾਂ[0]<<8;
                                length = 2;
                            } ਨਹੀਂਤਾਂ ਜੇ (si_ਲੰਮਾਈ == 2) {
                                value|=(uint32_t)si_ਬਾਈਟਾਂ[1]<<8;
                                value|=(uint32_t)si_ਬਾਈਟਾਂ[0]<<16;
                                length = 3;
                            }
                            prev_ਲੰਮਾਈ=1;
                        }
                    } ਨਹੀਂਤਾਂ {
                        ਜੇ(prev_ਲੰਮਾਈ==2) {
                            length=2;
                        } ਨਹੀਂਤਾਂ {
                            /* change from single-byte mode to double-byte */
                            ਜੇ (so_ਲੰਮਾਈ == 1) {
                                value|=(uint32_t)so_ਬਾਈਟਾਂ[0]<<16;
                                length = 3;
                            } ਨਹੀਂਤਾਂ ਜੇ (so_ਲੰਮਾਈ == 2) {
                                value|=(uint32_t)so_ਬਾਈਟਾਂ[1]<<16;
                                value|=(uint32_t)so_ਬਾਈਟਾਂ[0]<<24;
                                length = 4;
                            }
                            prev_ਲੰਮਾਈ=2;
                        }
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=MBCS_ਮੁੱਲ_2_FROM_STAGE_2(ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
                    ਜੇ(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        value=stage2_ਇੰਦਰਾਜ=0; /* stage2Entry=0 to reset roundtrip flags */
                        length=0;
                    } ਨਹੀਂਤਾਂ {
                        length=2;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ MBCS_OUTPUT_3:
                    p=MBCS_ਸੂਚਕ_3_FROM_STAGE_2(ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    ਜੇ(value<=0xff) {
                        length=1;
                    } ਨਹੀਂਤਾਂ ਜੇ(value<=0xffff) {
                        length=2;
                    } ਨਹੀਂਤਾਂ {
                        length=3;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ MBCS_OUTPUT_4:
                    value=MBCS_ਮੁੱਲ_4_FROM_STAGE_2(ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
                    ਜੇ(value<=0xff) {
                        length=1;
                    } ਨਹੀਂਤਾਂ ਜੇ(value<=0xffff) {
                        length=2;
                    } ਨਹੀਂਤਾਂ ਜੇ(value<=0xffffff) {
                        length=3;
                    } ਨਹੀਂਤਾਂ {
                        length=4;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ MBCS_OUTPUT_3_EUC:
                    value=MBCS_ਮੁੱਲ_2_FROM_STAGE_2(ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
                    /* EUC 16-bit fixed-length representation */
                    ਜੇ(value<=0xff) {
                        length=1;
                    } ਨਹੀਂਤਾਂ ਜੇ((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } ਨਹੀਂਤਾਂ ਜੇ((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } ਨਹੀਂਤਾਂ {
                        length=2;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ MBCS_OUTPUT_4_EUC:
                    p=MBCS_ਸੂਚਕ_3_FROM_STAGE_2(ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    ਜੇ(value<=0xff) {
                        length=1;
                    } ਨਹੀਂਤਾਂ ਜੇ(value<=0xffff) {
                        length=2;
                    } ਨਹੀਂਤਾਂ ਜੇ((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } ਨਹੀਂਤਾਂ ਜੇ((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } ਨਹੀਂਤਾਂ {
                        length=3;
                    }
                    ਰੋਕੋ;
                ਮੂਲ:
                    /* must not occur */
                    /*
                     * To avoid compiler warnings that value & length may be
                     * used without having been initialized, we set them here.
                     * In reality, this is unreachable code.
                     * Not having a default branch also causes warnings with
                     * some compilers.
                     */
                    value=stage2_ਇੰਦਰਾਜ=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                    ਰੋਕੋ;
                }

                /* is this code point assigned, or do we use fallbacks? */
                ਜੇ(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_ਇੰਦਰਾਜ, c)!=0 ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
                    c=_extFromU(cnv, cnv->shared_ਅੰਕੜੇ,
                                c, &ਸਰੋਤ, ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                                &ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ+ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                                &offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                                p_ਦਲੀਲਾਂ->flush,
                                p_Error_ਕੋਡ);
                    ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ+=(int32_t)(ਸਰੋਤ-p_ਦਲੀਲਾਂ->ਸਰੋਤ);
                    prev_ਲੰਮਾਈ=cnv->from_Unicode_ਸਥਿਤੀ; /* restore SISO state */

                    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                        /* not mappable or buffer overflow */
                        ਰੋਕੋ;
                    } ਨਹੀਂਤਾਂ {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-(ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ);

                        /* normal end of conversion: prepare for a new character */
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            prev_ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        }
                        ਜਾਰੀ;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            ਜੇ(length<=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
                ਜੇ(offsets==ਖਾਲੀ_ਸੂਚਕ) {
                    ਚੋਣ(length) {
                        /* each branch falls through to the next one */
                    ਹਾਲਤ 4:
                        *ਨਿਸ਼ਾਨਾ++=(uint8_t)(value>>24);
                        U_FALLTHROUGH;
                    ਹਾਲਤ 3:
                        *ਨਿਸ਼ਾਨਾ++=(uint8_t)(value>>16);
                        U_FALLTHROUGH;
                    ਹਾਲਤ 2:
                        *ਨਿਸ਼ਾਨਾ++=(uint8_t)(value>>8);
                        U_FALLTHROUGH;
                    ਹਾਲਤ 1:
                        *ਨਿਸ਼ਾਨਾ++=(uint8_t)value;
                        U_FALLTHROUGH;
                    ਮੂਲ:
                        /* will never occur */
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    ਚੋਣ(length) {
                        /* each branch falls through to the next one */
                    ਹਾਲਤ 4:
                        *ਨਿਸ਼ਾਨਾ++=(uint8_t)(value>>24);
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        U_FALLTHROUGH;
                    ਹਾਲਤ 3:
                        *ਨਿਸ਼ਾਨਾ++=(uint8_t)(value>>16);
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        U_FALLTHROUGH;
                    ਹਾਲਤ 2:
                        *ਨਿਸ਼ਾਨਾ++=(uint8_t)(value>>8);
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        U_FALLTHROUGH;
                    ਹਾਲਤ 1:
                        *ਨਿਸ਼ਾਨਾ++=(uint8_t)value;
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        U_FALLTHROUGH;
                    ਮੂਲ:
                        /* will never occur */
                        ਰੋਕੋ;
                    }
                }
                ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=length;
            } ਨਹੀਂਤਾਂ {
                uint8_t *char_Error_ਬਫਰ;

                /*
                 * We actually do this backwards here:
                 * In order to save an intermediate variable, we output
                 * first to the overflow buffer what does not fit into the
                 * regular target.
                 */
                /* we know that 1<=targetCapacity<length<=4 */
                length-=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                char_Error_ਬਫਰ=(uint8_t *)cnv->char_Error_ਬਫਰ;
                ਚੋਣ(length) {
                    /* each branch falls through to the next one */
                ਹਾਲਤ 3:
                    *char_Error_ਬਫਰ++=(uint8_t)(value>>16);
                    U_FALLTHROUGH;
                ਹਾਲਤ 2:
                    *char_Error_ਬਫਰ++=(uint8_t)(value>>8);
                    U_FALLTHROUGH;
                ਹਾਲਤ 1:
                    *char_Error_ਬਫਰ=(uint8_t)value;
                    U_FALLTHROUGH;
                ਮੂਲ:
                    /* will never occur */
                    ਰੋਕੋ;
                }
                cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=(int8_t)length;

                /* now output what fits into the regular target */
                value>>=8*length; /* length was reduced by targetCapacity */
                ਚੋਣ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
                    /* each branch falls through to the next one */
                ਹਾਲਤ 3:
                    *ਨਿਸ਼ਾਨਾ++=(uint8_t)(value>>16);
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                    U_FALLTHROUGH;
                ਹਾਲਤ 2:
                    *ਨਿਸ਼ਾਨਾ++=(uint8_t)(value>>8);
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                    U_FALLTHROUGH;
                ਹਾਲਤ 1:
                    *ਨਿਸ਼ਾਨਾ++=(uint8_t)value;
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                    U_FALLTHROUGH;
                ਮੂਲ:
                    /* will never occur */
                    ਰੋਕੋ;
                }

                /* target overflow */
                ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=0;
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                c=0;
                ਰੋਕੋ;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                prev_ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
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
    ਜੇ( U_SUCCESS(*p_Error_ਕੋਡ) &&
        outputType==MBCS_OUTPUT_2_SISO && prev_ਲੰਮਾਈ==2 &&
        p_ਦਲੀਲਾਂ->flush && ਸਰੋਤ>=ਸਰੋਤ_ਹੱਦਬੰਦੀ && c==0
    ) {
        /* EBCDIC_STATEFUL ending with DBCS: emit an SI to return the output stream to SBCS */
        ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
            *ਨਿਸ਼ਾਨਾ++ = si_ਬਾਈਟਾਂ[0];
            ਜੇ (si_ਲੰਮਾਈ == 2) {
                ਜੇ (ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ<2) {
                    cnv->char_Error_ਬਫਰ[0] = si_ਬਾਈਟਾਂ[1];
                    cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                    *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                } ਨਹੀਂਤਾਂ {
                    *ਨਿਸ਼ਾਨਾ++ = si_ਬਾਈਟਾਂ[1];
                }
            }
            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                /* set the last source character's index (sourceIndex points at sourceLimit now) */
                *offsets++=prev_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }
        } ਨਹੀਂਤਾਂ {
            /* target is full */
            cnv->char_Error_ਬਫਰ[0] = si_ਬਾਈਟਾਂ[0];
            ਜੇ (si_ਲੰਮਾਈ == 2) {
                cnv->char_Error_ਬਫਰ[1] = si_ਬਾਈਟਾਂ[1];
            }
            cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=si_ਲੰਮਾਈ;
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        }
        prev_ਲੰਮਾਈ=1; /* we switched into SBCS */
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;
    cnv->from_Unicode_ਸਥਿਤੀ=prev_ਲੰਮਾਈ;

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=(ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
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
ucnv_MBCSFromUChar32(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ,
                 UChar32 c, uint32_t *p_ਮੁੱਲ,
                 UBool useFallback) {
    ਅਚਲ int32_t *cx;
    ਅਚਲ uint16_t *ਸਾਰਣੀ;
#ਜੇ 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
    ਅਚਲ uint8_t *p;
#ਜੇ_ਅੰਤ
    uint32_t stage2_ਇੰਦਰਾਜ;
    uint32_t value;
    int32_t length;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    ਜੇ(c<=0xffff || (shared_ਅੰਕੜੇ->mbcs.unicode_ਨਕਾਬ&UCNV_HAS_SUPPLEMENTARY)) {
        ਸਾਰਣੀ=shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਸਾਰਣੀ;

        /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
        ਜੇ(shared_ਅੰਕੜੇ->mbcs.outputType==MBCS_OUTPUT_1) {
            value=MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, (uint16_t *)shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ, c);
            /* is this code point assigned, or do we use fallbacks? */
            ਜੇ(useFallback ? value>=0x800 : value>=0xc00) {
                *p_ਮੁੱਲ=value&0xff;
                ਵਾਪਸ 1;
            }
        } ਨਹੀਂਤਾਂ /* outputType!=MBCS_OUTPUT_1 */ {
            stage2_ਇੰਦਰਾਜ=MBCS_STAGE_2_FROM_U(ਸਾਰਣੀ, c);

            /* get the bytes and the length for the output */
            ਚੋਣ(shared_ਅੰਕੜੇ->mbcs.outputType) {
            ਹਾਲਤ MBCS_OUTPUT_2:
                value=MBCS_ਮੁੱਲ_2_FROM_STAGE_2(shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
                ਜੇ(value<=0xff) {
                    length=1;
                } ਨਹੀਂਤਾਂ {
                    length=2;
                }
                ਰੋਕੋ;
#ਜੇ 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
            ਹਾਲਤ MBCS_OUTPUT_DBCS_ONLY:
                /* table with single-byte results, but only DBCS mappings used */
                value=MBCS_ਮੁੱਲ_2_FROM_STAGE_2(shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
                ਜੇ(value<=0xff) {
                    /* no mapping or SBCS result, not taken for DBCS-only */
                    value=stage2_ਇੰਦਰਾਜ=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                } ਨਹੀਂਤਾਂ {
                    length=2;
                }
                ਰੋਕੋ;
            ਹਾਲਤ MBCS_OUTPUT_3:
                p=MBCS_ਸੂਚਕ_3_FROM_STAGE_2(shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                ਜੇ(value<=0xff) {
                    length=1;
                } ਨਹੀਂਤਾਂ ਜੇ(value<=0xffff) {
                    length=2;
                } ਨਹੀਂਤਾਂ {
                    length=3;
                }
                ਰੋਕੋ;
            ਹਾਲਤ MBCS_OUTPUT_4:
                value=MBCS_ਮੁੱਲ_4_FROM_STAGE_2(shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
                ਜੇ(value<=0xff) {
                    length=1;
                } ਨਹੀਂਤਾਂ ਜੇ(value<=0xffff) {
                    length=2;
                } ਨਹੀਂਤਾਂ ਜੇ(value<=0xffffff) {
                    length=3;
                } ਨਹੀਂਤਾਂ {
                    length=4;
                }
                ਰੋਕੋ;
            ਹਾਲਤ MBCS_OUTPUT_3_EUC:
                value=MBCS_ਮੁੱਲ_2_FROM_STAGE_2(shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
                /* EUC 16-bit fixed-length representation */
                ਜੇ(value<=0xff) {
                    length=1;
                } ਨਹੀਂਤਾਂ ਜੇ((value&0x8000)==0) {
                    value|=0x8e8000;
                    length=3;
                } ਨਹੀਂਤਾਂ ਜੇ((value&0x80)==0) {
                    value|=0x8f0080;
                    length=3;
                } ਨਹੀਂਤਾਂ {
                    length=2;
                }
                ਰੋਕੋ;
            ਹਾਲਤ MBCS_OUTPUT_4_EUC:
                p=MBCS_ਸੂਚਕ_3_FROM_STAGE_2(shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                /* EUC 16-bit fixed-length representation applied to the first two bytes */
                ਜੇ(value<=0xff) {
                    length=1;
                } ਨਹੀਂਤਾਂ ਜੇ(value<=0xffff) {
                    length=2;
                } ਨਹੀਂਤਾਂ ਜੇ((value&0x800000)==0) {
                    value|=0x8e800000;
                    length=4;
                } ਨਹੀਂਤਾਂ ਜੇ((value&0x8000)==0) {
                    value|=0x8f008000;
                    length=4;
                } ਨਹੀਂਤਾਂ {
                    length=3;
                }
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ
            ਮੂਲ:
                /* must not occur */
                ਵਾਪਸ -1;
            }

            /* is this code point assigned, or do we use fallbacks? */
            ਜੇ( MBCS_FROM_U_IS_ROUNDTRIP(stage2_ਇੰਦਰਾਜ, c) ||
                (FROM_U_USE_FALLBACK(useFallback, c) && value!=0)
            ) {
                /*
                 * We allow a 0 byte output if the "assigned" bit is set for this entry.
                 * There is no way with this data structure for fallback output
                 * to be a zero byte.
                 */
                /* assigned */
                *p_ਮੁੱਲ=value;
                ਵਾਪਸ length;
            }
        }
    }

    cx=shared_ਅੰਕੜੇ->mbcs.extIndexes;
    ਜੇ(cx!=ਖਾਲੀ_ਸੂਚਕ) {
        length=ucnv_ext_Simple_ਮੇਲ_From_U(cx, c, p_ਮੁੱਲ, useFallback);
        ਵਾਪਸ length>=0 ? length : -length;  /* return abs(length); */
    }

    /* unassigned */
    ਵਾਪਸ 0;
}


#ਜੇ 0
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
ucnv_MBCSSingleFromUChar32(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ,
                       UChar32 c,
                       UBool useFallback) {
    ਅਚਲ uint16_t *ਸਾਰਣੀ;
    int32_t value;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    ਜੇ(c>=0x10000 && !(shared_ਅੰਕੜੇ->mbcs.unicode_ਨਕਾਬ&UCNV_HAS_SUPPLEMENTARY)) {
        ਵਾਪਸ -1;
    }

    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    ਸਾਰਣੀ=shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਸਾਰਣੀ;

    /* get the byte for the output */
    value=MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, (uint16_t *)shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ, c);
    /* is this code point assigned, or do we use fallbacks? */
    ਜੇ(useFallback ? value>=0x800 : value>=0xc00) {
        ਵਾਪਸ value&0xff;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ -1;
    }
}
#ਜੇ_ਅੰਤ

/* MBCS-from-UTF-8 conversion functions ------------------------------------- */

/* offsets for n-byte UTF-8 sequences that were calculated with ((lead<<6)+trail)<<6+trail... */
ਸਥਿਰ ਅਚਲ UChar32
utf8_offsets[5]={ 0, 0, 0x3080, 0xE2080, 0x3C82080 };

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_ਦਲੀਲਾਂ *pFromUArgs,
                  UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਮੰਜ਼ਿਲ_UArgs,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *utf8, *cnv;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;

    ਅਚਲ uint16_t *ਸਾਰਣੀ, *sbcs_ਸੂਚਕਾਂਕ;
    ਅਚਲ uint16_t *results;

    int8_t old_ਮੰਜ਼ਿਲ_ULength, ਮੰਜ਼ਿਲ_ULength, ਮੰਜ਼ਿਲ_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t asciiRoundtrips;
    uint16_t value, min_ਮੁੱਲ = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_ਮੰਜ਼ਿਲ_UArgs->converter;
    cnv=pFromUArgs->converter;
    ਸਰੋਤ=(uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=(uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ = reinterpret_cast<uint8_t*>(pFromUArgs->ਨਿਸ਼ਾਨਾ);
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(pFromUArgs->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - pFromUArgs->ਨਿਸ਼ਾਨਾ);

    ਸਾਰਣੀ=cnv->shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਸਾਰਣੀ;
    sbcs_ਸੂਚਕਾਂਕ=cnv->shared_ਅੰਕੜੇ->mbcs.sbcs_ਸੂਚਕਾਂਕ;
    ਜੇ((cnv->options&UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_ਅੰਕੜੇ->mbcs.ਅਦਲਾ_ਬਦਲੀ_LFNLFrom_Unicode_ਬਾਈਟਾਂ);
    } ਨਹੀਂਤਾਂ {
        results=(uint16_t *)cnv->shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ;
    }
    asciiRoundtrips=cnv->shared_ਅੰਕੜੇ->mbcs.asciiRoundtrips;

    ਜੇ(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_ਮੁੱਲ=0x800;
    } ਨਹੀਂਤਾਂ {
        /* use only roundtrips and fallbacks from private-use characters */
        min_ਮੁੱਲ=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_ਅੰਕੜੇ->mbcs.unicode_ਨਕਾਬ & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    ਜੇ(utf8->ਮੰਜ਼ਿਲ_ULength > 0) {
        ਮੰਜ਼ਿਲ_ULength=old_ਮੰਜ਼ਿਲ_ULength=utf8->ਮੰਜ਼ਿਲ_ULength;
        ਮੰਜ਼ਿਲ_ULimit = static_cast<int8_t>(utf8->ਬਹੁਲਕ);
        c = static_cast<UChar32>(utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ {
        ਮੰਜ਼ਿਲ_ULength=old_ਮੰਜ਼ਿਲ_ULength=ਮੰਜ਼ਿਲ_ULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(ਸਰੋਤ_ਹੱਦਬੰਦੀ - ਸਰੋਤ) - (ਮੰਜ਼ਿਲ_ULimit - old_ਮੰਜ਼ਿਲ_ULength);
        ਜੇ(length>0) {
            uint8_t b1=*(ਸਰੋਤ_ਹੱਦਬੰਦੀ-1);
            ਜੇ(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } ਨਹੀਂਤਾਂ ਜੇ(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(ਸਰੋਤ_ਹੱਦਬੰਦੀ-2);
                ਜੇ(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    ਸਰੋਤ_ਹੱਦਬੰਦੀ-=2;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --ਸਰੋਤ_ਹੱਦਬੰਦੀ;
            }
        }
    }

    ਜੇ(c!=0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
        utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=0;
        utf8->ਮੰਜ਼ਿਲ_ULength=0;
        ਜਾਓ ਹੋਰ_ਬਾਈਟਾਂ;
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
    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
            b=*ਸਰੋਤ++;
            ਜੇ(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                ਜੇ(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *ਨਿਸ਼ਾਨਾ++ = b;
                    --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                    ਜਾਰੀ;
                } ਨਹੀਂਤਾਂ {
                    c=b;
                    value=SBCS_ਨਤੀਜਾ_FROM_UTF8(sbcs_ਸੂਚਕਾਂਕ, results, 0, c);
                }
            } ਨਹੀਂਤਾਂ {
                ਜੇ(b<0xe0) {
                    ਜੇ( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*ਸਰੋਤ - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++ਸਰੋਤ;
                        value=SBCS_ਨਤੀਜਾ_FROM_UTF8(sbcs_ਸੂਚਕਾਂਕ, results, c, t1);
                        ਜੇ(value>=min_ਮੁੱਲ) {
                            *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(value);
                            --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                            ਜਾਰੀ;
                        } ਨਹੀਂਤਾਂ {
                            c=(c<<6)|t1;
                        }
                    } ਨਹੀਂਤਾਂ {
                        c=-1;
                    }
                } ਨਹੀਂਤਾਂ ਜੇ(b==0xe0) {
                    ਜੇ( /* handle U+0800..U+0FFF inline */
                        (t1 = static_cast<uint8_t>(ਸਰੋਤ[0] - 0x80)) <= 0x3f && t1 >= 0x20 &&
                        (t2 = static_cast<uint8_t>(ਸਰੋਤ[1] - 0x80)) <= 0x3f
                    ) {
                        c=t1;
                        ਸਰੋਤ+=2;
                        value=SBCS_ਨਤੀਜਾ_FROM_UTF8(sbcs_ਸੂਚਕਾਂਕ, results, c, t2);
                        ਜੇ(value>=min_ਮੁੱਲ) {
                            *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(value);
                            --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                            ਜਾਰੀ;
                        } ਨਹੀਂਤਾਂ {
                            c=(c<<6)|t2;
                        }
                    } ਨਹੀਂਤਾਂ {
                        c=-1;
                    }
                } ਨਹੀਂਤਾਂ {
                    c=-1;
                }

                ਜੇ(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    old_ਮੰਜ਼ਿਲ_ULength=0;
                    ਮੰਜ਼ਿਲ_ULength=1;
                    ਮੰਜ਼ਿਲ_ULimit=U8_ਗਿਣਤੀ_ਬਾਈਟਾਂ_NON_ASCII(b);
                    c=b;
ਹੋਰ_ਬਾਈਟਾਂ:
                    ਜਦੋਂ(ਮੰਜ਼ਿਲ_ULength<ਮੰਜ਼ਿਲ_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        ਜੇ(ਸਰੋਤ<(uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                            b=*ਸਰੋਤ;
                            ਜੇ(icu::UTF8::isValidTrail(c, b, ਮੰਜ਼ਿਲ_ULength, ਮੰਜ਼ਿਲ_ULimit)) {
                                ++ਸਰੋਤ;
                                ++ਮੰਜ਼ਿਲ_ULength;
                                c=(c<<6)+b;
                            } ਨਹੀਂਤਾਂ {
                                ਰੋਕੋ; /* sequence too short, stop with toULength<toULimit */
                            }
                        } ਨਹੀਂਤਾਂ {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            ਸਰੋਤ-=(ਮੰਜ਼ਿਲ_ULength-old_ਮੰਜ਼ਿਲ_ULength);
                            ਜਦੋਂ(old_ਮੰਜ਼ਿਲ_ULength<ਮੰਜ਼ਿਲ_ULength) {
                                utf8->ਮੰਜ਼ਿਲ_UBytes[old_ਮੰਜ਼ਿਲ_ULength++]=*ਸਰੋਤ++;
                            }
                            utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=c;
                            utf8->ਮੰਜ਼ਿਲ_ULength=ਮੰਜ਼ਿਲ_ULength;
                            utf8->ਬਹੁਲਕ=ਮੰਜ਼ਿਲ_ULimit;
                            p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ=(ਅੱਖਰ *)ਸਰੋਤ;
                            pFromUArgs->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
                            ਵਾਪਸ;
                        }
                    }

                    ਜੇ(ਮੰਜ਼ਿਲ_ULength==ਮੰਜ਼ਿਲ_ULimit) {
                        c-=utf8_offsets[ਮੰਜ਼ਿਲ_ULength];
                        ਜੇ(ਮੰਜ਼ਿਲ_ULength<=3) {  /* BMP */
                            value=MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, results, c);
                        } ਨਹੀਂਤਾਂ {
                            /* supplementary code point */
                            ਜੇ(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                value=0;
                            } ਨਹੀਂਤਾਂ {
                                value=MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, results, c);
                            }
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* error handling: illegal UTF-8 byte sequence */
                        ਸਰੋਤ-=(ਮੰਜ਼ਿਲ_ULength-old_ਮੰਜ਼ਿਲ_ULength);
                        ਜਦੋਂ(old_ਮੰਜ਼ਿਲ_ULength<ਮੰਜ਼ਿਲ_ULength) {
                            utf8->ਮੰਜ਼ਿਲ_UBytes[old_ਮੰਜ਼ਿਲ_ULength++]=*ਸਰੋਤ++;
                        }
                        utf8->ਮੰਜ਼ਿਲ_ULength=ਮੰਜ਼ਿਲ_ULength;
                        p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ=(ਅੱਖਰ *)ਸਰੋਤ;
                        pFromUArgs->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
                        *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਵਾਪਸ;
                    }
                }
            }

            ਜੇ(value>=min_ਮੁੱਲ) {
                /* output the mapping for c */
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(value);
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
            } ਨਹੀਂਤਾਂ {
                /* value<minValue means c is unassigned (unmappable) */
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                ਸਥਿਰ ਅਚਲ char16_t nul=0;
                ਅਚਲ char16_t *no_ਸਰੋਤ=&nul;
                c=_extFromU(cnv, cnv->shared_ਅੰਕੜੇ,
                            c, &no_ਸਰੋਤ, no_ਸਰੋਤ,
                            &ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ+ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                            ਖਾਲੀ_ਸੂਚਕ, -1,
                            pFromUArgs->flush,
                            p_Error_ਕੋਡ);

                ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *p_Error_ਕੋਡ=U_USING_DEFAULT_WARNING;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(pFromUArgs->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ));
                }
            }
        } ਨਹੀਂਤਾਂ {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ) &&
            cnv->preFromUFirstCP<0 &&
            ਸਰੋਤ<(ਸਰੋਤ_ਹੱਦਬੰਦੀ=(uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ_ਹੱਦਬੰਦੀ)) {
        c=utf8->ਮੰਜ਼ਿਲ_UBytes[0]=b=*ਸਰੋਤ++;
        ਮੰਜ਼ਿਲ_ULength=1;
        ਮੰਜ਼ਿਲ_ULimit=U8_ਗਿਣਤੀ_ਬਾਈਟਾਂ(b);
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            utf8->ਮੰਜ਼ਿਲ_UBytes[ਮੰਜ਼ਿਲ_ULength++]=b=*ਸਰੋਤ++;
            c=(c<<6)+b;
        }
        utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=c;
        utf8->ਮੰਜ਼ਿਲ_ULength=ਮੰਜ਼ਿਲ_ULength;
        utf8->ਬਹੁਲਕ=ਮੰਜ਼ਿਲ_ULimit;
    }

    /* write back the updated pointers */
    p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ=(ਅੱਖਰ *)ਸਰੋਤ;
    pFromUArgs->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_ਦਲੀਲਾਂ *pFromUArgs,
                  UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਮੰਜ਼ਿਲ_UArgs,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *utf8, *cnv;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;

    ਅਚਲ uint16_t *ਸਾਰਣੀ, *mbcs_ਸੂਚਕਾਂਕ;
    ਅਚਲ uint16_t *results;

    int8_t old_ਮੰਜ਼ਿਲ_ULength, ਮੰਜ਼ਿਲ_ULength, ਮੰਜ਼ਿਲ_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t stage2_ਇੰਦਰਾਜ;
    uint32_t asciiRoundtrips;
    uint16_t value = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_ਮੰਜ਼ਿਲ_UArgs->converter;
    cnv=pFromUArgs->converter;
    ਸਰੋਤ=(uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=(uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ = reinterpret_cast<uint8_t*>(pFromUArgs->ਨਿਸ਼ਾਨਾ);
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(pFromUArgs->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - pFromUArgs->ਨਿਸ਼ਾਨਾ);

    ਸਾਰਣੀ=cnv->shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਸਾਰਣੀ;
    mbcs_ਸੂਚਕਾਂਕ=cnv->shared_ਅੰਕੜੇ->mbcs.mbcs_ਸੂਚਕਾਂਕ;
    ਜੇ((cnv->options&UCNV_OPTION_ਅਦਲਾ_ਬਦਲੀ_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_ਅੰਕੜੇ->mbcs.ਅਦਲਾ_ਬਦਲੀ_LFNLFrom_Unicode_ਬਾਈਟਾਂ);
    } ਨਹੀਂਤਾਂ {
        results=(uint16_t *)cnv->shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ;
    }
    asciiRoundtrips=cnv->shared_ਅੰਕੜੇ->mbcs.asciiRoundtrips;

    hasSupplementary = static_cast<UBool>(cnv->shared_ਅੰਕੜੇ->mbcs.unicode_ਨਕਾਬ & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    ਜੇ(utf8->ਮੰਜ਼ਿਲ_ULength > 0) {
        ਮੰਜ਼ਿਲ_ULength=old_ਮੰਜ਼ਿਲ_ULength=utf8->ਮੰਜ਼ਿਲ_ULength;
        ਮੰਜ਼ਿਲ_ULimit = static_cast<int8_t>(utf8->ਬਹੁਲਕ);
        c = static_cast<UChar32>(utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ {
        ਮੰਜ਼ਿਲ_ULength=old_ਮੰਜ਼ਿਲ_ULength=ਮੰਜ਼ਿਲ_ULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(ਸਰੋਤ_ਹੱਦਬੰਦੀ - ਸਰੋਤ) - (ਮੰਜ਼ਿਲ_ULimit - old_ਮੰਜ਼ਿਲ_ULength);
        ਜੇ(length>0) {
            uint8_t b1=*(ਸਰੋਤ_ਹੱਦਬੰਦੀ-1);
            ਜੇ(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } ਨਹੀਂਤਾਂ ਜੇ(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(ਸਰੋਤ_ਹੱਦਬੰਦੀ-2);
                ਜੇ(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    ਸਰੋਤ_ਹੱਦਬੰਦੀ-=2;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --ਸਰੋਤ_ਹੱਦਬੰਦੀ;
            }
        }
    }

    ਜੇ(c!=0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
        utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=0;
        utf8->ਮੰਜ਼ਿਲ_ULength=0;
        ਜਾਓ ਹੋਰ_ਬਾਈਟਾਂ;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
            b=*ਸਰੋਤ++;
            ਜੇ(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                ਜੇ(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *ਨਿਸ਼ਾਨਾ++=b;
                    --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                    ਜਾਰੀ;
                } ਨਹੀਂਤਾਂ {
                    value=DBCS_ਨਤੀਜਾ_FROM_UTF8(mbcs_ਸੂਚਕਾਂਕ, results, 0, b);
                    ਜੇ(value==0) {
                        c=b;
                        ਜਾਓ unassigned;
                    }
                }
            } ਨਹੀਂਤਾਂ {
                ਜੇ(b>=0xe0) {
                    ਜੇ( /* handle U+0800..U+D7FF inline */
                        b<=0xed &&  // do not assume maxFastUChar>0xd7ff
                        U8_IS_VALID_LEAD3_AND_T1(b, t1=ਸਰੋਤ[0]) &&
                        (t2 = static_cast<uint8_t>(ਸਰੋਤ[1] - 0x80)) <= 0x3f
                    ) {
                        c=((b&0xf)<<6)|(t1&0x3f);
                        ਸਰੋਤ+=2;
                        value=DBCS_ਨਤੀਜਾ_FROM_UTF8(mbcs_ਸੂਚਕਾਂਕ, results, c, t2);
                        ਜੇ(value==0) {
                            c=(c<<6)|t2;
                            ਜਾਓ unassigned;
                        }
                    } ਨਹੀਂਤਾਂ {
                        c=-1;
                    }
                } ਨਹੀਂਤਾਂ {
                    ਜੇ( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*ਸਰੋਤ - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++ਸਰੋਤ;
                        value=DBCS_ਨਤੀਜਾ_FROM_UTF8(mbcs_ਸੂਚਕਾਂਕ, results, c, t1);
                        ਜੇ(value==0) {
                            c=(c<<6)|t1;
                            ਜਾਓ unassigned;
                        }
                    } ਨਹੀਂਤਾਂ {
                        c=-1;
                    }
                }

                ਜੇ(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    old_ਮੰਜ਼ਿਲ_ULength=0;
                    ਮੰਜ਼ਿਲ_ULength=1;
                    ਮੰਜ਼ਿਲ_ULimit=U8_ਗਿਣਤੀ_ਬਾਈਟਾਂ_NON_ASCII(b);
                    c=b;
ਹੋਰ_ਬਾਈਟਾਂ:
                    ਜਦੋਂ(ਮੰਜ਼ਿਲ_ULength<ਮੰਜ਼ਿਲ_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        ਜੇ(ਸਰੋਤ<(uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                            b=*ਸਰੋਤ;
                            ਜੇ(icu::UTF8::isValidTrail(c, b, ਮੰਜ਼ਿਲ_ULength, ਮੰਜ਼ਿਲ_ULimit)) {
                                ++ਸਰੋਤ;
                                ++ਮੰਜ਼ਿਲ_ULength;
                                c=(c<<6)+b;
                            } ਨਹੀਂਤਾਂ {
                                ਰੋਕੋ; /* sequence too short, stop with toULength<toULimit */
                            }
                        } ਨਹੀਂਤਾਂ {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            ਸਰੋਤ-=(ਮੰਜ਼ਿਲ_ULength-old_ਮੰਜ਼ਿਲ_ULength);
                            ਜਦੋਂ(old_ਮੰਜ਼ਿਲ_ULength<ਮੰਜ਼ਿਲ_ULength) {
                                utf8->ਮੰਜ਼ਿਲ_UBytes[old_ਮੰਜ਼ਿਲ_ULength++]=*ਸਰੋਤ++;
                            }
                            utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=c;
                            utf8->ਮੰਜ਼ਿਲ_ULength=ਮੰਜ਼ਿਲ_ULength;
                            utf8->ਬਹੁਲਕ=ਮੰਜ਼ਿਲ_ULimit;
                            p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ=(ਅੱਖਰ *)ਸਰੋਤ;
                            pFromUArgs->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
                            ਵਾਪਸ;
                        }
                    }

                    ਜੇ(ਮੰਜ਼ਿਲ_ULength==ਮੰਜ਼ਿਲ_ULimit) {
                        c-=utf8_offsets[ਮੰਜ਼ਿਲ_ULength];
                        ਜੇ(ਮੰਜ਼ਿਲ_ULength<=3) {  /* BMP */
                            stage2_ਇੰਦਰਾਜ=MBCS_STAGE_2_FROM_U(ਸਾਰਣੀ, c);
                        } ਨਹੀਂਤਾਂ {
                            /* supplementary code point */
                            ਜੇ(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                stage2_ਇੰਦਰਾਜ=0;
                            } ਨਹੀਂਤਾਂ {
                                stage2_ਇੰਦਰਾਜ=MBCS_STAGE_2_FROM_U(ਸਾਰਣੀ, c);
                            }
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* error handling: illegal UTF-8 byte sequence */
                        ਸਰੋਤ-=(ਮੰਜ਼ਿਲ_ULength-old_ਮੰਜ਼ਿਲ_ULength);
                        ਜਦੋਂ(old_ਮੰਜ਼ਿਲ_ULength<ਮੰਜ਼ਿਲ_ULength) {
                            utf8->ਮੰਜ਼ਿਲ_UBytes[old_ਮੰਜ਼ਿਲ_ULength++]=*ਸਰੋਤ++;
                        }
                        utf8->ਮੰਜ਼ਿਲ_ULength=ਮੰਜ਼ਿਲ_ULength;
                        p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ=(ਅੱਖਰ *)ਸਰੋਤ;
                        pFromUArgs->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
                        *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਵਾਪਸ;
                    }

                    /* get the bytes and the length for the output */
                    /* MBCS_OUTPUT_2 */
                    value=MBCS_ਮੁੱਲ_2_FROM_STAGE_2(results, stage2_ਇੰਦਰਾਜ, c);

                    /* is this code point assigned, or do we use fallbacks? */
                    ਜੇ(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_ਇੰਦਰਾਜ, c) ||
                         (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                    ) {
                        ਜਾਓ unassigned;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            ਜੇ(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(value);
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
            } ਨਹੀਂਤਾਂ /* length==2 */ {
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(value >> 8);
                ਜੇ(2<=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
                    *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(value);
                    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=2;
                } ਨਹੀਂਤਾਂ {
                    cnv->char_Error_ਬਫਰ[0] = static_cast<ਅੱਖਰ>(value);
                    cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=1;

                    /* target overflow */
                    *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    ਰੋਕੋ;
                }
            }
            ਜਾਰੀ;

unassigned:
            {
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                ਸਥਿਰ ਅਚਲ char16_t nul=0;
                ਅਚਲ char16_t *no_ਸਰੋਤ=&nul;
                c=_extFromU(cnv, cnv->shared_ਅੰਕੜੇ,
                            c, &no_ਸਰੋਤ, no_ਸਰੋਤ,
                            &ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ+ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                            ਖਾਲੀ_ਸੂਚਕ, -1,
                            pFromUArgs->flush,
                            p_Error_ਕੋਡ);

                ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *p_Error_ਕੋਡ=U_USING_DEFAULT_WARNING;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(pFromUArgs->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ));
                    ਜਾਰੀ;
                }
            }
        } ਨਹੀਂਤਾਂ {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ) &&
            cnv->preFromUFirstCP<0 &&
            ਸਰੋਤ<(ਸਰੋਤ_ਹੱਦਬੰਦੀ=(uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ_ਹੱਦਬੰਦੀ)) {
        c=utf8->ਮੰਜ਼ਿਲ_UBytes[0]=b=*ਸਰੋਤ++;
        ਮੰਜ਼ਿਲ_ULength=1;
        ਮੰਜ਼ਿਲ_ULimit=U8_ਗਿਣਤੀ_ਬਾਈਟਾਂ(b);
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            utf8->ਮੰਜ਼ਿਲ_UBytes[ਮੰਜ਼ਿਲ_ULength++]=b=*ਸਰੋਤ++;
            c=(c<<6)+b;
        }
        utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=c;
        utf8->ਮੰਜ਼ਿਲ_ULength=ਮੰਜ਼ਿਲ_ULength;
        utf8->ਬਹੁਲਕ=ਮੰਜ਼ਿਲ_ULimit;
    }

    /* write back the updated pointers */
    p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ=(ਅੱਖਰ *)ਸਰੋਤ;
    pFromUArgs->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
}

/* miscellaneous ------------------------------------------------------------ */

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_MBCSGetStarters(ਅਚਲ UConverter* cnv,
                 UBool starters[256],
                 UError_ਕੋਡ *) {
    ਅਚਲ int32_t *state0;
    ਪੂਰਨ i;

    state0=cnv->shared_ਅੰਕੜੇ->mbcs.state_ਸਾਰਣੀ[cnv->shared_ਅੰਕੜੇ->mbcs.dbcsOnlyState];
    ਲਈ(i=0; i<256; ++i) {
        /* all bytes that cause a state transition from state 0 are lead bytes */
        starters[i] = static_cast<UBool>(MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(state0[i]));
    }
}

/*
 * This is an internal function that allows other converter implementations
 * to check whether a byte is a lead byte.
 */
U_CFUNC UBool
ucnv_MBCSIsLeadByte(UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ, ਅੱਖਰ byte) {
    ਵਾਪਸ MBCS_ਇੰਦਰਾਜ_IS_TRANSITION(shared_ਅੰਕੜੇ->mbcs.state_ਸਾਰਣੀ[0][(uint8_t)byte]);
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
              int32_t ਵਿੱਥ_ਸੂਚਕਾਂਕ,
              UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv=p_ਦਲੀਲਾਂ->converter;
    ਅੱਖਰ *p, *subchar;
    ਅੱਖਰ ਬਫਰ[4];
    int32_t length;

    /* first, select between subChar and subChar1 */
    ਜੇ( cnv->subChar1!=0 &&
        (cnv->shared_ਅੰਕੜੇ->mbcs.extIndexes!=ਖਾਲੀ_ਸੂਚਕ ?
            cnv->useSubChar1 :
            (cnv->invalid_UChar_ਬਫਰ[0]<=0xff))
    ) {
        /* select subChar1 if it is set (not 0) and the unmappable Unicode code point is up to U+00ff (IBM MBCS behavior) */
        subchar = reinterpret_cast<ਅੱਖਰ*>(&cnv->subChar1);
        length=1;
    } ਨਹੀਂਤਾਂ {
        /* select subChar in all other cases */
        subchar = reinterpret_cast<ਅੱਖਰ*>(cnv->subChars);
        length=cnv->subCharLen;
    }

    /* reset the selector for the next code point */
    cnv->useSubChar1=ਝੂਠ;

    ਜੇ (cnv->shared_ਅੰਕੜੇ->mbcs.outputType == MBCS_OUTPUT_2_SISO) {
        p=ਬਫਰ;

        /* fromUnicodeStatus contains prevLength */
        ਚੋਣ(length) {
        ਹਾਲਤ 1:
            ਜੇ(cnv->from_Unicode_ਸਥਿਤੀ==2) {
                /* DBCS mode and SBCS sub char: change to SBCS */
                cnv->from_Unicode_ਸਥਿਤੀ=1;
                *p++=UCNV_SI;
            }
            *p++=subchar[0];
            ਰੋਕੋ;
        ਹਾਲਤ 2:
            ਜੇ(cnv->from_Unicode_ਸਥਿਤੀ<=1) {
                /* SBCS mode and DBCS sub char: change to DBCS */
                cnv->from_Unicode_ਸਥਿਤੀ=2;
                *p++=UCNV_SO;
            }
            *p++=subchar[0];
            *p++=subchar[1];
            ਰੋਕੋ;
        ਮੂਲ:
            *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ;
        }
        subchar=ਬਫਰ;
        length = static_cast<int32_t>(p - ਬਫਰ);
    }

    ucnv_cb_From_UWrite_ਬਾਈਟਾਂ(p_ਦਲੀਲਾਂ, subchar, length, ਵਿੱਥ_ਸੂਚਕਾਂਕ, p_Error_ਕੋਡ);
}

U_CFUNC UConverterType
ucnv_MBCSGetType(ਅਚਲ UConverter* converter) {
    /* SBCS, DBCS, and EBCDIC_STATEFUL are replaced by MBCS, but here we cheat a little */
    ਜੇ(converter->shared_ਅੰਕੜੇ->mbcs.ਗਿਣਤੀ_ਹਾਲਤਾਂ==1) {
        ਵਾਪਸ (UConverterType)UCNV_SBCS;
    } ਨਹੀਂਤਾਂ ਜੇ((converter->shared_ਅੰਕੜੇ->mbcs.outputType&0xff)==MBCS_OUTPUT_2_SISO) {
        ਵਾਪਸ (UConverterType)UCNV_EBCDIC_STATEFUL;
    } ਨਹੀਂਤਾਂ ਜੇ(converter->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->min_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_Char==2 && converter->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_Char==2) {
        ਵਾਪਸ (UConverterType)UCNV_DBCS;
    }
    ਵਾਪਸ (UConverterType)UCNV_MBCS;
}

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_LEGACY_CONVERSION */
