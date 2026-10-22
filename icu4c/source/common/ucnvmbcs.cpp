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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_cb.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnvmbcs.h"
#ಅನ್ತರ್ಭಾವ "ucnv_ext.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

/* control optimizations according to the platform */
#ವ್ಯಾಖ್ಯಾನಿಸು MBCS_UNROLL_SINGLE_ಗಮ್ಯ_BMP 1
#ವ್ಯಾಖ್ಯಾನಿಸು MBCS_UNROLL_SINGLE_FROM_BMP 0

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ UBool U_CALLCONV
UConverter_Enum_ಗಮ್ಯ_UCallback(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, uint32_t value, UChar32 ಸಂಕೇತ_Points[32]);

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ,
          UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
          ಸ್ಥಿರಾಂಕ uint8_t *raw,
          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ);

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

ಸ್ಥಿರ UChar32 U_CALLCONV
ucnv_MBCSGet_ಮುಂದಿನ_UChar(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_MBCSGetStarters(ಸ್ಥಿರಾಂಕ UConverter* cnv,
                 UBool starters[256],
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

U_CDECL_BEGIN
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_CALLCONV
ucnv_MBCSGetName(ಸ್ಥಿರಾಂಕ UConverter *cnv);
U_CDECL_ಕೊನೆ

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
              int32_t ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

ಸ್ಥಿರ UChar32 U_CALLCONV
ucnv_MBCSGet_ಮುಂದಿನ_UChar(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_ವಾದಗಳು *pFromUArgs,
                  UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ಗಮ್ಯ_UArgs,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_MBCSGet_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                   ಸ್ಥಿರಾಂಕ USetAdder *sa,
                   UConverter_Unicode_ಗಣ which,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_ವಾದಗಳು *pFromUArgs,
                  UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ಗಮ್ಯ_UArgs,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _SBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_ಮುಂದಿನ_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_MBCSGet_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ucnv_SBCSFromUTF8
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _DBCSUTF8Impl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_ಮುಂದಿನ_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_MBCSGet_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ucnv_DBCSFromUTF8
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _MBCSImpl={
    UCNV_MBCS,

    ucnv_MBCSLoad,
    ucnv_MBCSUnload,

    ucnv_MBCSOpen,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSToUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSFromUnicodeWithOffsets,
    ucnv_MBCSGet_ಮುಂದಿನ_UChar,

    ucnv_MBCSGetStarters,
    ucnv_MBCSGetName,
    ucnv_MBCSWriteSub,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_MBCSGet_Unicode_ಗಣ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

/* Static data is in tools/makeconv/ucnvstat.c for data-based
 * converters. Be sure to update it as well.
 */

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _MBCSData={
    ಗಾತ್ರ(UConverter_Shared_ದತ್ತಾಂಶ), 1,
    ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಸುಳ್ಳು, ನಿಜ, &_MBCSImpl,
    0, UCNV_MBCS_ಕೋಷ್ಟಕ_INITIALIZER
};


/* GB 18030 data ------------------------------------------------------------ */

/* helper macros for linear values for GB 18030 four-byte sequences */
#ವ್ಯಾಖ್ಯಾನಿಸು ರೇಖೀಯ_18030(a, b, c, d) ((((a)*10+(b))*126L+(c))*10L+(d))

#ವ್ಯಾಖ್ಯಾನಿಸು ರೇಖೀಯ_18030_ತಳ ರೇಖೀಯ_18030(0x81, 0x30, 0x81, 0x30)

#ವ್ಯಾಖ್ಯಾನಿಸು ರೇಖೀಯ(x) ರೇಖೀಯ_18030(x>>24, (x>>16)&0xff, (x>>8)&0xff, x&0xff)

/*
 * Some ranges of GB 18030 where both the Unicode code points and the
 * GB four-byte sequences are contiguous and are handled algorithmically by
 * the special callback functions below.
 * The values are start & end of Unicode & GB codes.
 *
 * Note that single surrogates are not mapped by GB 18030
 * as of the re-released mapping tables from 2000-nov-30.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint32_t
gb18030Ranges[14][4]={
    {0x10000, 0x10FFFF, ರೇಖೀಯ(0x90308130), ರೇಖೀಯ(0xE3329A35)},
    {0x9FA6, 0xD7FF, ರೇಖೀಯ(0x82358F33), ರೇಖೀಯ(0x8336C738)},
    {0x0452, 0x1E3E, ರೇಖೀಯ(0x8130D330), ರೇಖೀಯ(0x8135F436)},
    {0x1E40, 0x200F, ರೇಖೀಯ(0x8135F438), ರೇಖೀಯ(0x8136A531)},
    {0xE865, 0xF92B, ರೇಖೀಯ(0x8336D030), ರೇಖೀಯ(0x84308534)},
    {0x2643, 0x2E80, ರೇಖೀಯ(0x8137A839), ರೇಖೀಯ(0x8138FD38)},
    {0xFA2A, 0xFE2F, ರೇಖೀಯ(0x84309C38), ರೇಖೀಯ(0x84318537)},
    {0x3CE1, 0x4055, ರೇಖೀಯ(0x8231D438), ರೇಖೀಯ(0x8232AF32)},
    {0x361B, 0x3917, ರೇಖೀಯ(0x8230A633), ರೇಖೀಯ(0x8230F237)},
    {0x49B8, 0x4C76, ರೇಖೀಯ(0x8234A131), ರೇಖೀಯ(0x8234E733)},
    {0x4160, 0x4336, ರೇಖೀಯ(0x8232C937), ರೇಖೀಯ(0x8232F837)},
    {0x478E, 0x4946, ರೇಖೀಯ(0x8233E838), ರೇಖೀಯ(0x82349638)},
    {0x44D7, 0x464B, ರೇಖೀಯ(0x8233A339), ರೇಖೀಯ(0x8233C931)},
    {0xFFE6, 0xFFFF, ರೇಖೀಯ(0x8431A234), ರೇಖೀಯ(0x8431A439)}
};

/* bit flag for UConverter.options indicating GB 18030 special handling */
#ವ್ಯಾಖ್ಯಾನಿಸು _MBCS_OPTION_GB18030 0x8000

/* bit flag for UConverter.options indicating KEIS,JEF,JIF special handling */
#ವ್ಯಾಖ್ಯಾನಿಸು _MBCS_OPTION_KEIS 0x01000
#ವ್ಯಾಖ್ಯಾನಿಸು _MBCS_OPTION_JEF  0x02000
#ವ್ಯಾಖ್ಯಾನಿಸು _MBCS_OPTION_JIPS 0x04000

#ವ್ಯಾಖ್ಯಾನಿಸು KEIS_SO_CHAR_1 0x0A
#ವ್ಯಾಖ್ಯಾನಿಸು KEIS_SO_CHAR_2 0x42
#ವ್ಯಾಖ್ಯಾನಿಸು KEIS_SI_CHAR_1 0x0A
#ವ್ಯಾಖ್ಯಾನಿಸು KEIS_SI_CHAR_2 0x41

#ವ್ಯಾಖ್ಯಾನಿಸು JEF_SO_CHAR 0x28
#ವ್ಯಾಖ್ಯಾನಿಸು JEF_SI_CHAR 0x29

#ವ್ಯಾಖ್ಯಾನಿಸು JIPS_SO_CHAR_1 0x1A
#ವ್ಯಾಖ್ಯಾನಿಸು JIPS_SO_CHAR_2 0x70
#ವ್ಯಾಖ್ಯಾನಿಸು JIPS_SI_CHAR_1 0x1A
#ವ್ಯಾಖ್ಯಾನಿಸು JIPS_SI_CHAR_2 0x71

ಎಣಿಕೆ SISO_Option {
    SI,
    SO
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ SISO_Option SISO_Option;

ಸ್ಥಿರ int32_t getSISOBytes(SISO_Option option, uint32_t cnvOption, uint8_t *value) {
    int32_t SISOLength = 0;

    ಆಯ್ಕೆ (option) {
        ಸ್ಥಿತಿ SI:
            ಆದರೆ ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SI_CHAR_1;
                value[1] = KEIS_SI_CHAR_2;
                SISOLength = 2;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SI_CHAR;
                SISOLength = 1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SI_CHAR_1;
                value[1] = JIPS_SI_CHAR_2;
                SISOLength = 2;
            } ಇಲ್ಲದಿದ್ದರೆ {
                value[0] = UCNV_SI;
                SISOLength = 1;
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ SO:
            ಆದರೆ ((cnvOption&_MBCS_OPTION_KEIS)!=0) {
                value[0] = KEIS_SO_CHAR_1;
                value[1] = KEIS_SO_CHAR_2;
                SISOLength = 2;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cnvOption&_MBCS_OPTION_JEF)!=0) {
                value[0] = JEF_SO_CHAR;
                SISOLength = 1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cnvOption&_MBCS_OPTION_JIPS)!=0) {
                value[0] = JIPS_SO_CHAR_1;
                value[1] = JIPS_SO_CHAR_2;
                SISOLength = 2;
            } ಇಲ್ಲದಿದ್ದರೆ {
                value[0] = UCNV_SO;
                SISOLength = 1;
            }
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            /* Should never happen. */
            ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ SISOLength;
}

/* Miscellaneous ------------------------------------------------------------ */

/* similar to ucnv_MBCSGetNextUChar() but recursive */
ಸ್ಥಿರ UBool
enum_ಗಮ್ಯ_U(UConverterMBCSTable *mbcs_ಕೋಷ್ಟಕ, int8_t stateProps[],
        int32_t state, uint32_t ಸ್ಥಾನಾಂತರ,
        uint32_t value,
        UConverter_Enum_ಗಮ್ಯ_UCallback *callback, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
        UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UChar32 ಸಂಕೇತ_Points[32];
    ಸ್ಥಿರಾಂಕ int32_t *ಅಡ್ಡಸಾಲು;
    ಸ್ಥಿರಾಂಕ uint16_t *unicode_ಸಂಕೇತ_Units;
    UChar32 any_ಸಂಕೇತ_Points;
    int32_t b, ಎಲ್ಲೆ;

    ಅಡ್ಡಸಾಲು=mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ[state];
    unicode_ಸಂಕೇತ_Units=mbcs_ಕೋಷ್ಟಕ->unicode_ಸಂಕೇತ_Units;

    value<<=8;
    any_ಸಂಕೇತ_Points=-1;  /* becomes non-negative if there is a mapping */

    b=(stateProps[state]&0x38)<<2;
    ಆದರೆ(b==0 && stateProps[state]>=0x40) {
        /* skip byte sequences with leading zeros because they are not stored in the fromUnicode table */
        ಸಂಕೇತ_Points[0]=U_SENTINEL;
        b=1;
    }
    ಎಲ್ಲೆ=((stateProps[state]&7)+1)<<5;
    ಆಗ(b<ಎಲ್ಲೆ) {
        int32_t ನಮೂದು=ಅಡ್ಡಸಾಲು[b];
        ಆದರೆ(MBCS_ನಮೂದು_IS_TRANSITION(ನಮೂದು)) {
            int32_t ಮುಂದಿನ_State=MBCS_ನಮೂದು_TRANSITION_STATE(ನಮೂದು);
            ಆದರೆ(stateProps[ಮುಂದಿನ_State]>=0) {
                /* recurse to a state with non-ignorable actions */
                ಆದರೆ(!enum_ಗಮ್ಯ_U(
                        mbcs_ಕೋಷ್ಟಕ, stateProps, ಮುಂದಿನ_State,
                        ಸ್ಥಾನಾಂತರ+MBCS_ನಮೂದು_TRANSITION_ಸ್ಥಾನಾಂತರ(ನಮೂದು),
                        value | static_cast<uint32_t>(b),
                        callback, context,
                        p_Error_ಸಂಕೇತ)) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            }
            ಸಂಕೇತ_Points[b&0x1f]=U_SENTINEL;
        } ಇಲ್ಲದಿದ್ದರೆ {
            UChar32 c;
            int32_t action;

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು);
            ಆದರೆ(action==MBCS_STATE_VALID_ನೇರ_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_VALID_16) {
                int32_t final_ಸ್ಥಾನಾಂತರ=ಸ್ಥಾನಾಂತರ+MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
                c=unicode_ಸಂಕೇತ_Units[final_ಸ್ಥಾನಾಂತರ];
                ಆದರೆ(c<0xfffe) {
                    /* output BMP code point */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    c=U_SENTINEL;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_VALID_16_PAIR) {
                int32_t final_ಸ್ಥಾನಾಂತರ=ಸ್ಥಾನಾಂತರ+MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
                c=unicode_ಸಂಕೇತ_Units[final_ಸ್ಥಾನಾಂತರ++];
                ಆದರೆ(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_ಸಂಕೇತ_Units[final_ಸ್ಥಾನಾಂತರ]+(0x10000-0xdc00);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_ಸಂಕೇತ_Units[final_ಸ್ಥಾನಾಂತರ];
                } ಇಲ್ಲದಿದ್ದರೆ {
                    c=U_SENTINEL;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_VALID_ನೇರ_20) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ(ನಮೂದು) + 0x10000);
            } ಇಲ್ಲದಿದ್ದರೆ {
                c=U_SENTINEL;
            }

            ಸಂಕೇತ_Points[b&0x1f]=c;
            any_ಸಂಕೇತ_Points&=c;
        }
        ಆದರೆ(((++b)&0x1f)==0) {
            ಆದರೆ(any_ಸಂಕೇತ_Points>=0) {
                ಆದರೆ (!callback(context, value | static_cast<uint32_t>(b - 0x20), ಸಂಕೇತ_Points)) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
                any_ಸಂಕೇತ_Points=-1;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * Only called if stateProps[state]==-1.
 * A recursive call may do stateProps[state]|=0x40 if this state is the target of an
 * MBCS_STATE_CHANGE_ONLY.
 */
ಸ್ಥಿರ int8_t
getStateProp(ಸ್ಥಿರಾಂಕ int32_t (*state_ಕೋಷ್ಟಕ)[256], int8_t stateProps[], ಪೂರ್ಣಾಂಕ state) {
    ಸ್ಥಿರಾಂಕ int32_t *ಅಡ್ಡಸಾಲು;
    int32_t min, max, ನಮೂದು, ಮುಂದಿನ_State;

    ಅಡ್ಡಸಾಲು=state_ಕೋಷ್ಟಕ[state];
    stateProps[state]=0;

    /* find first non-ignorable state */
    ಗಾಗಿ(min=0;; ++min) {
        ನಮೂದು=ಅಡ್ಡಸಾಲು[min];
        ಮುಂದಿನ_State=MBCS_ನಮೂದು_STATE(ನಮೂದು);
        ಆದರೆ(stateProps[ಮುಂದಿನ_State]==-1) {
            getStateProp(state_ಕೋಷ್ಟಕ, stateProps, ಮುಂದಿನ_State);
        }
        ಆದರೆ(MBCS_ನಮೂದು_IS_TRANSITION(ನಮೂದು)) {
            ಆದರೆ(stateProps[ಮುಂದಿನ_State]>=0) {
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು)<MBCS_STATE_UNASSIGNED) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ(min==0xff) {
            stateProps[state]=-0x40;  /* (int8_t)0xc0 */
            ಹಿಂತಿರುಗಿ stateProps[state];
        }
    }
    stateProps[state] |= static_cast<int8_t>((min >> 5) << 3);

    /* find last non-ignorable state */
    ಗಾಗಿ(max=0xff; min<max; --max) {
        ನಮೂದು=ಅಡ್ಡಸಾಲು[max];
        ಮುಂದಿನ_State=MBCS_ನಮೂದು_STATE(ನಮೂದು);
        ಆದರೆ(stateProps[ಮುಂದಿನ_State]==-1) {
            getStateProp(state_ಕೋಷ್ಟಕ, stateProps, ಮುಂದಿನ_State);
        }
        ಆದರೆ(MBCS_ನಮೂದು_IS_TRANSITION(ನಮೂದು)) {
            ಆದರೆ(stateProps[ಮುಂದಿನ_State]>=0) {
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು)<MBCS_STATE_UNASSIGNED) {
            ನಿಲ್ಲಿಸು;
        }
    }
    stateProps[state] |= static_cast<int8_t>(max >> 5);

    /* recurse further and collect direct-state information */
    ಆಗ(min<=max) {
        ನಮೂದು=ಅಡ್ಡಸಾಲು[min];
        ಮುಂದಿನ_State=MBCS_ನಮೂದು_STATE(ನಮೂದು);
        ಆದರೆ(stateProps[ಮುಂದಿನ_State]==-1) {
            getStateProp(state_ಕೋಷ್ಟಕ, stateProps, ಮುಂದಿನ_State);
        }
        ಆದರೆ(MBCS_ನಮೂದು_IS_FINAL(ನಮೂದು)) {
            stateProps[ಮುಂದಿನ_State]|=0x40;
            ಆದರೆ(MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು)<=MBCS_STATE_FALLBACK_ನೇರ_20) {
                stateProps[state]|=0x40;
            }
        }
        ++min;
    }
    ಹಿಂತಿರುಗಿ stateProps[state];
}

/*
 * Internal function enumerating the toUnicode data of an MBCS converter.
 * Currently only used for reconstituting data for a MBCS_OPT_NO_FROM_U
 * table, but could also be used for a future ucnv_getUnicodeSet() option
 * that includes reverse fallbacks (after updating this function's implementation).
 * Currently only handles roundtrip mappings.
 * Does not currently handle extensions.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ucnv_MBCSEnum_ಗಮ್ಯ_Unicode(UConverterMBCSTable *mbcs_ಕೋಷ್ಟಕ,
                       UConverter_Enum_ಗಮ್ಯ_UCallback *callback, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
                       UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
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
    int8_t stateProps[MBCS_MAX_STATE_ಎಣಿಕೆ];
    int32_t state;

    uprv_memset(stateProps, -1, ಗಾತ್ರ(stateProps));

    /* recurse from state 0 and set all stateProps */
    getStateProp(mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ, stateProps, 0);

    ಗಾಗಿ(state=0; state<mbcs_ಕೋಷ್ಟಕ->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು; ++state) {
        /*if(stateProps[state]==-1) {
            printf("unused/unreachable <icu:state> %d\n", state);
        }*/
        ಆದರೆ(stateProps[state]>=0x40) {
            /* start from each direct state */
            enum_ಗಮ್ಯ_U(
                mbcs_ಕೋಷ್ಟಕ, stateProps, state, 0, 0,
                callback, context,
                p_Error_ಸಂಕೇತ);
        }
    }
}

U_CFUNC ಶೂನ್ಯ 
ucnv_MBCSGet_Filtered_Unicode_ಗಣ_For_Unicode(ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ,
                                         ಸ್ಥಿರಾಂಕ USetAdder *sa,
                                         UConverter_Unicode_ಗಣ which,
                                         UConverter_ಗಣ_Filter filter,
                                         UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UConverterMBCSTable *mbcs_ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ;

    uint32_t st3;
    uint16_t st1, maxStage1, st2;

    UChar32 c;

    /* enumerate the from-Unicode trie table */
    mbcs_ಕೋಷ್ಟಕ=&shared_ದತ್ತಾಂಶ->mbcs;
    ಕೋಷ್ಟಕ=mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಕೋಷ್ಟಕ;
    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->unicode_ಮುಖವಾಡ&UCNV_HAS_SUPPLEMENTARY) {
        maxStage1=0x440;
    } ಇಲ್ಲದಿದ್ದರೆ {
        maxStage1=0x40;
    }

    c=0; /* keep track of the current code point while enumerating */

    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->outputType==MBCS_OUTPUT_1) {
        ಸ್ಥಿರಾಂಕ uint16_t *stage2, *stage3, *results;
        uint16_t min_ಮೌಲ್ಯ;

        results=(ಸ್ಥಿರಾಂಕ uint16_t *)mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಬೈಟುಗಳು;

        /*
         * Set a threshold variable for selecting which mappings to use.
         * See ucnv_MBCSSingleFromBMPWithOffsets() and
         * MBCS_SINGLE_RESULT_FROM_U() for details.
         */
        ಆದರೆ(which==UCNV_ROUNDTRIP_ಗಣ) {
            /* use only roundtrips */
            min_ಮೌಲ್ಯ=0xf00;
        } ಇಲ್ಲದಿದ್ದರೆ /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
            /* use all roundtrip and fallback results */
            min_ಮೌಲ್ಯ=0x800;
        }

        ಗಾಗಿ(st1=0; st1<maxStage1; ++st1) {
            st2=ಕೋಷ್ಟಕ[st1];
            ಆದರೆ(st2>maxStage1) {
                stage2=ಕೋಷ್ಟಕ+st2;
                ಗಾಗಿ(st2=0; st2<64; ++st2) {
                    ಆದರೆ((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=results+st3;

                        ಮಾಡು {
                            ಆದರೆ(*stage3++>=min_ಮೌಲ್ಯ) {
                                sa->ಕೂಡಿಸು(sa->ಗಣ, c);
                            }
                        } ಆಗ((++c&0xf)!=0);
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        c+=16; /* empty stage 3 block */
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                c+=1024; /* empty stage 2 block */
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಿರಾಂಕ uint32_t *stage2;
        ಸ್ಥಿರಾಂಕ uint8_t *stage3, *ಬೈಟುಗಳು;
        uint32_t st3Multiplier;
        uint32_t value;
        UBool useFallback;

        ಬೈಟುಗಳು=mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಬೈಟುಗಳು;

        useFallback = which == UCNV_ROUNDTRIP_AND_FALLBACK_ಗಣ;

        ಆಯ್ಕೆ(mbcs_ಕೋಷ್ಟಕ->outputType) {
        ಸ್ಥಿತಿ MBCS_OUTPUT_3:
        ಸ್ಥಿತಿ MBCS_OUTPUT_4_EUC:
            st3Multiplier=3;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ MBCS_OUTPUT_4:
            st3Multiplier=4;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            st3Multiplier=2;
            ನಿಲ್ಲಿಸು;
        }

        ಗಾಗಿ(st1=0; st1<maxStage1; ++st1) {
            st2=ಕೋಷ್ಟಕ[st1];
            ಆದರೆ(st2>(maxStage1>>1)) {
                stage2=(ಸ್ಥಿರಾಂಕ uint32_t *)ಕೋಷ್ಟಕ+st2;
                ಗಾಗಿ(st2=0; st2<64; ++st2) {
                    ಆದರೆ((st3=stage2[st2])!=0) {
                        /* read the stage 3 block */
                        stage3=ಬೈಟುಗಳು+st3Multiplier*16*(uint32_t)(uint16_t)st3;

                        /* get the roundtrip flags for the stage 3 block */
                        st3>>=16;

                        /*
                         * Add code points for which the roundtrip flag is set,
                         * or which map to non-zero bytes if we use fallbacks.
                         * See ucnv_MBCSFromUnicodeWithOffsets() for details.
                         */
                        ಆಯ್ಕೆ(filter) {
                        ಸ್ಥಿತಿ UCNV_ಗಣ_FILTER_NONE:
                            ಮಾಡು {
                                ಆದರೆ(st3&1) {
                                    sa->ಕೂಡಿಸು(sa->ಗಣ, c);
                                    stage3+=st3Multiplier;
                                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(useFallback) {
                                    uint8_t b=0;
                                    ಆಯ್ಕೆ(st3Multiplier) {
                                    ಸ್ಥಿತಿ 4:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    ಸ್ಥಿತಿ 3:
                                        b|=*stage3++;
                                        U_FALLTHROUGH;
                                    ಸ್ಥಿತಿ 2:
                                        b|=stage3[0]|stage3[1];
                                        stage3+=2;
                                        U_FALLTHROUGH;
                                    ಪೂರ್ವನಿಯೋಜಿತ:
                                        ನಿಲ್ಲಿಸು;
                                    }
                                    ಆದರೆ(b!=0) {
                                        sa->ಕೂಡಿಸು(sa->ಗಣ, c);
                                    }
                                }
                                st3>>=1;
                            } ಆಗ((++c&0xf)!=0);
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ UCNV_ಗಣ_FILTER_DBCS_ONLY:
                             /* Ignore single-byte results (<0x100). */
                            ಮಾಡು {
                                ಆದರೆ(((st3&1)!=0 || useFallback) && *((ಸ್ಥಿರಾಂಕ uint16_t *)stage3)>=0x100) {
                                    sa->ಕೂಡಿಸು(sa->ಗಣ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } ಆಗ((++c&0xf)!=0);
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ UCNV_ಗಣ_FILTER_2022_CN:
                             /* Only add code points that map to CNS 11643 planes 1 & 2 for non-EXT ISO-2022-CN. */
                            ಮಾಡು {
                                ಆದರೆ(((st3&1)!=0 || useFallback) && ((value=*stage3)==0x81 || value==0x82)) {
                                    sa->ಕೂಡಿಸು(sa->ಗಣ, c);
                                }
                                st3>>=1;
                                stage3+=3;  /* +=st3Multiplier */
                            } ಆಗ((++c&0xf)!=0);
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ UCNV_ಗಣ_FILTER_SJIS:
                             /* Only add code points that map to Shift-JIS codes corresponding to JIS X 0208. */
                            ಮಾಡು {
                                ಆದರೆ(((st3&1)!=0 || useFallback) && (value=*((ಸ್ಥಿರಾಂಕ uint16_t *)stage3))>=0x8140 && value<=0xeffc) {
                                    sa->ಕೂಡಿಸು(sa->ಗಣ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } ಆಗ((++c&0xf)!=0);
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ UCNV_ಗಣ_FILTER_GR94_DBCS:
                            /* Only add code points that map to ISO 2022 GR 94 DBCS codes (each byte A1..FE). */
                            ಮಾಡು {
                                ಆದರೆ( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((ಸ್ಥಿರಾಂಕ uint16_t *)stage3)) - 0xa1a1)<=(0xfefe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->ಕೂಡಿಸು(sa->ಗಣ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } ಆಗ((++c&0xf)!=0);
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ UCNV_ಗಣ_FILTER_HZ:
                            /* Only add code points that are suitable for HZ DBCS (lead byte A1..FD). */
                            ಮಾಡು {
                                ಆದರೆ( ((st3&1)!=0 || useFallback) &&
                                    (uint16_t)((value=*((ಸ್ಥಿರಾಂಕ uint16_t *)stage3))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                    (uint8_t)(value-0xa1)<=(0xfe - 0xa1)
                                ) {
                                    sa->ಕೂಡಿಸು(sa->ಗಣ, c);
                                }
                                st3>>=1;
                                stage3+=2;  /* +=st3Multiplier */
                            } ಆಗ((++c&0xf)!=0);
                            ನಿಲ್ಲಿಸು;
                        ಪೂರ್ವನಿಯೋಜಿತ:
                            *p_Error_ಸಂಕೇತ=U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
                            ಹಿಂತಿರುಗಿ;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        c+=16; /* empty stage 3 block */
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                c+=1024; /* empty stage 2 block */
            }
        }
    }

    ucnv_ext_Get_Unicode_ಗಣ(shared_ದತ್ತಾಂಶ, sa, which, filter, p_Error_ಸಂಕೇತ);
}

U_CFUNC ಶೂನ್ಯ
ucnv_MBCSGet_Unicode_ಗಣ_For_Unicode(ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ USetAdder *sa,
                                 UConverter_Unicode_ಗಣ which,
                                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ucnv_MBCSGet_Filtered_Unicode_ಗಣ_For_Unicode(
        shared_ದತ್ತಾಂಶ, sa, which,
        shared_ದತ್ತಾಂಶ->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ?
            UCNV_ಗಣ_FILTER_DBCS_ONLY :
            UCNV_ಗಣ_FILTER_NONE,
        p_Error_ಸಂಕೇತ);
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_MBCSGet_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                   ಸ್ಥಿರಾಂಕ USetAdder *sa,
                   UConverter_Unicode_ಗಣ which,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(cnv->options&_MBCS_OPTION_GB18030) {
        sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0, 0xd7ff);
        sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0xe000, 0x10ffff);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ucnv_MBCSGet_Unicode_ಗಣ_For_Unicode(cnv->shared_ದತ್ತಾಂಶ, sa, which, p_Error_ಸಂಕೇತ);
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
ಸ್ಥಿರ UChar32
_extFromU(UConverter *cnv, ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ,
          UChar32 cp,
          ಸ್ಥಿರಾಂಕ char16_t **ಮೂಲ, ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ_ಎಲ್ಲೆ,
          uint8_t **ಗುರಿ, ಸ್ಥಿರಾಂಕ uint8_t *ಗುರಿ_ಎಲ್ಲೆ,
          int32_t **offsets, int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ,
          UBool flush,
          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ int32_t *cx;

    cnv->useSubChar1=ಸುಳ್ಳು;

    ಆದರೆ( (cx=shared_ದತ್ತಾಂಶ->mbcs.extIndexes)!=ಶೂನ್ಯಸೂಚಕ &&
        ucnv_ext_Initial_ಹೊಂದಿಕೆ_From_U(
            cnv, cx,
            cp, ಮೂಲ, ಮೂಲ_ಎಲ್ಲೆ,
            reinterpret_cast<ಅಕ್ಷರ**>(ಗುರಿ), reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಗುರಿ_ಎಲ್ಲೆ),
            offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
            flush,
            p_Error_ಸಂಕೇತ)
    ) {
        ಹಿಂತಿರುಗಿ 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    ಆದರೆ((cnv->options&_MBCS_OPTION_GB18030)!=0) {
        ಸ್ಥಿರಾಂಕ uint32_t *range;
        int32_t i;

        range=gb18030Ranges[0];
        ಗಾಗಿ(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            ಆದರೆ (range[0] <= static_cast<uint32_t>(cp) && static_cast<uint32_t>(cp) <= range[1]) {
                /* found the Unicode code point, output the four-byte sequence for it */
                uint32_t ರೇಖೀಯ_2;
                ಅಕ್ಷರ ಬೈಟುಗಳು[4];

                /* get the linear value of the first GB 18030 code in this range */
                ರೇಖೀಯ_2=range[2]-ರೇಖೀಯ_18030_ತಳ;

                /* add the offset from the beginning of the range */
                ರೇಖೀಯ_2 += (static_cast<uint32_t>(cp) - range[0]);

                /* turn this into a four-byte sequence */
                ಬೈಟುಗಳು[3] = static_cast<ಅಕ್ಷರ>(0x30 + ರೇಖೀಯ_2 % 10); ರೇಖೀಯ_2 /= 10;
                ಬೈಟುಗಳು[2] = static_cast<ಅಕ್ಷರ>(0x81 + ರೇಖೀಯ_2 % 126); ರೇಖೀಯ_2 /= 126;
                ಬೈಟುಗಳು[1] = static_cast<ಅಕ್ಷರ>(0x30 + ರೇಖೀಯ_2 % 10); ರೇಖೀಯ_2 /= 10;
                ಬೈಟುಗಳು[0] = static_cast<ಅಕ್ಷರ>(0x81 + ರೇಖೀಯ_2);

                /* output this sequence */
                ucnv_from_UWrite_ಬೈಟುಗಳು(cnv,
                                     ಬೈಟುಗಳು, 4, reinterpret_cast<ಅಕ್ಷರ**>(ಗುರಿ), reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಗುರಿ_ಎಲ್ಲೆ),
                                     offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ, p_Error_ಸಂಕೇತ);
                ಹಿಂತಿರುಗಿ 0;
            }
        }
    }

    /* no mapping */
    *p_Error_ಸಂಕೇತ=U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
    ಹಿಂತಿರುಗಿ cp;
}

/*
 * Input sequence: cnv->toUBytes[0..length[
 * @return if(U_FAILURE) return the length (toULength, byteIndex) for the input
 *         else return 0 after output has been written to the target
 */
ಸ್ಥಿರ int8_t
ext_ಗಮ್ಯ_U(UConverter *cnv, ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ,
        int8_t length,
        ಸ್ಥಿರಾಂಕ uint8_t **ಮೂಲ, ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ_ಎಲ್ಲೆ,
        char16_t **ಗುರಿ, ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ,
        int32_t **offsets, int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ,
        UBool flush,
        UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ int32_t *cx;

    ಆದರೆ( (cx=shared_ದತ್ತಾಂಶ->mbcs.extIndexes)!=ಶೂನ್ಯಸೂಚಕ &&
        ucnv_ext_Initial_ಹೊಂದಿಕೆ_ಗಮ್ಯ_U(
            cnv, cx,
            length, reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ**>(ಮೂಲ), reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಮೂಲ_ಎಲ್ಲೆ),
            ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ,
            offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
            flush,
            p_Error_ಸಂಕೇತ)
    ) {
        ಹಿಂತಿರುಗಿ 0; /* an extension mapping handled the input */
    }

    /* GB 18030 */
    ಆದರೆ(length==4 && (cnv->options&_MBCS_OPTION_GB18030)!=0) {
        ಸ್ಥಿರಾಂಕ uint32_t *range;
        uint32_t ರೇಖೀಯ_2;
        int32_t i;

        ರೇಖೀಯ_2=ರೇಖೀಯ_18030(cnv->ಗಮ್ಯ_UBytes[0], cnv->ಗಮ್ಯ_UBytes[1], cnv->ಗಮ್ಯ_UBytes[2], cnv->ಗಮ್ಯ_UBytes[3]);
        range=gb18030Ranges[0];
        ಗಾಗಿ(i=0; i<UPRV_LENGTHOF(gb18030Ranges); range+=4, ++i) {
            ಆದರೆ(range[2]<=ರೇಖೀಯ_2 && ರೇಖೀಯ_2<=range[3]) {
                /* found the sequence, output the Unicode code point for it */
                *p_Error_ಸಂಕೇತ=U_ZERO_ERROR;

                /* add the linear difference between the input and start sequences to the start code point */
                ರೇಖೀಯ_2=range[0]+(ರೇಖೀಯ_2-range[2]);

                /* output this code point */
                ucnv_ಗಮ್ಯ_UWrite_ಸಂಕೇತ_ಬಿಂದು(cnv, ರೇಖೀಯ_2, ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ, offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ, p_Error_ಸಂಕೇತ);

                ಹಿಂತಿರುಗಿ 0;
            }
        }
    }

    /* no mapping */
    *p_Error_ಸಂಕೇತ=U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
    ಹಿಂತಿರುಗಿ length;
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
#ವ್ಯಾಖ್ಯಾನಿಸು EBCDIC_LF 0x25
#ವ್ಯಾಖ್ಯಾನಿಸು EBCDIC_NL 0x15

/* standard EBCDIC codes with roundtrip flag as stored in Unicode-to-single-byte tables */
#ವ್ಯಾಖ್ಯಾನಿಸು EBCDIC_RT_LF 0xf25
#ವ್ಯಾಖ್ಯಾನಿಸು EBCDIC_RT_NL 0xf15

/* Unicode code points */
#ವ್ಯಾಖ್ಯಾನಿಸು U_LF 0x0a
#ವ್ಯಾಖ್ಯಾನಿಸು U_NL 0x85

ಸ್ಥಿರ UBool
_EBCDICSwapLFNL(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverterMBCSTable *mbcs_ಕೋಷ್ಟಕ;

    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ, *results;
    ಸ್ಥಿರಾಂಕ uint8_t *ಬೈಟುಗಳು;

    int32_t (*new_State_ಕೋಷ್ಟಕ)[256];
    uint16_t *newResults;
    uint8_t *p;
    ಅಕ್ಷರ *name;

    uint32_t stage2_ನಮೂದು;
    uint32_t size, sizeofFromUBytes;

    mbcs_ಕೋಷ್ಟಕ=&shared_ದತ್ತಾಂಶ->mbcs;

    ಕೋಷ್ಟಕ=mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಕೋಷ್ಟಕ;
    ಬೈಟುಗಳು=mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಬೈಟುಗಳು;
    results = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(ಬೈಟುಗಳು);

    /*
     * Check that this is an EBCDIC table with SBCS portion -
     * SBCS or EBCDIC_STATEFUL with standard EBCDIC LF and NL mappings.
     *
     * If not, ignore the option. Options are always ignored if they do not apply.
     */
    ಆದರೆ(!(
         (mbcs_ಕೋಷ್ಟಕ->outputType==MBCS_OUTPUT_1 || mbcs_ಕೋಷ್ಟಕ->outputType==MBCS_OUTPUT_2_SISO) &&
         mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ[0][EBCDIC_LF]==MBCS_ನಮೂದು_FINAL(0, MBCS_STATE_VALID_ನೇರ_16, U_LF) &&
         mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ[0][EBCDIC_NL]==MBCS_ನಮೂದು_FINAL(0, MBCS_STATE_VALID_ನೇರ_16, U_NL)
    )) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->outputType==MBCS_OUTPUT_1) {
        ಆದರೆ(!(
             EBCDIC_RT_LF==MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, results, U_LF) &&
             EBCDIC_RT_NL==MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, results, U_NL)
        )) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    } ಇಲ್ಲದಿದ್ದರೆ /* MBCS_OUTPUT_2_SISO */ {
        stage2_ನಮೂದು=MBCS_STAGE_2_FROM_U(ಕೋಷ್ಟಕ, U_LF);
        ಆದರೆ(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_ನಮೂದು, U_LF)!=0 &&
             EBCDIC_LF==MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(ಬೈಟುಗಳು, stage2_ನಮೂದು, U_LF)
        )) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        stage2_ನಮೂದು=MBCS_STAGE_2_FROM_U(ಕೋಷ್ಟಕ, U_NL);
        ಆದರೆ(!(
             MBCS_FROM_U_IS_ROUNDTRIP(stage2_ನಮೂದು, U_NL)!=0 &&
             EBCDIC_NL==MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(ಬೈಟುಗಳು, stage2_ನಮೂದು, U_NL)
        )) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }

    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->from_UBytes_ಉದ್ದಳತೆ>0) {
        /*
         * We _know_ the number of bytes in the fromUnicodeBytes array
         * starting with header.version 4.1.
         */
        sizeofFromUBytes=mbcs_ಕೋಷ್ಟಕ->from_UBytes_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
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
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    /*
     * The table has an appropriate format.
     * Allocate and build
     * - a modified to-Unicode state table
     * - a modified from-Unicode output array
     * - a converter name string with the swap option appended
     */
    size=
        mbcs_ಕೋಷ್ಟಕ->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು*1024+
        sizeofFromUBytes+
        UCNV_MAX_CONVERTER_NAME_ಉದ್ದಳತೆ+20;
    p = static_cast<uint8_t*>(uprv_malloc(size));
    ಆದರೆ(p==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    /* copy and modify the to-Unicode state table */
    new_State_ಕೋಷ್ಟಕ = reinterpret_cast<int32_t(*)[256]>(p);
    uprv_memcpy(new_State_ಕೋಷ್ಟಕ, mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ, mbcs_ಕೋಷ್ಟಕ->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು*1024);

    new_State_ಕೋಷ್ಟಕ[0][EBCDIC_LF]=MBCS_ನಮೂದು_FINAL(0, MBCS_STATE_VALID_ನೇರ_16, U_NL);
    new_State_ಕೋಷ್ಟಕ[0][EBCDIC_NL]=MBCS_ನಮೂದು_FINAL(0, MBCS_STATE_VALID_ನೇರ_16, U_LF);

    /* copy and modify the from-Unicode result table */
    newResults = reinterpret_cast<uint16_t*>(new_State_ಕೋಷ್ಟಕ[mbcs_ಕೋಷ್ಟಕ->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು]);
    uprv_memcpy(newResults, ಬೈಟುಗಳು, sizeofFromUBytes);

    /* conveniently, the table access macros work on the left side of expressions */
    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->outputType==MBCS_OUTPUT_1) {
        MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, newResults, U_LF)=EBCDIC_RT_NL;
        MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, newResults, U_NL)=EBCDIC_RT_LF;
    } ಇಲ್ಲದಿದ್ದರೆ /* MBCS_OUTPUT_2_SISO */ {
        stage2_ನಮೂದು=MBCS_STAGE_2_FROM_U(ಕೋಷ್ಟಕ, U_LF);
        MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(newResults, stage2_ನಮೂದು, U_LF)=EBCDIC_NL;

        stage2_ನಮೂದು=MBCS_STAGE_2_FROM_U(ಕೋಷ್ಟಕ, U_NL);
        MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(newResults, stage2_ನಮೂದು, U_NL)=EBCDIC_LF;
    }

    /* set the canonical converter name */
    name = reinterpret_cast<ಅಕ್ಷರ*>(newResults) + sizeofFromUBytes;
    uprv_strcpy(name, shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->name);
    uprv_strcat(name, UCNV_ಅದಲುಬದಲು_LFNL_OPTION_STRING);

    /* set the pointers */
    icu::umtx_ಬೀಗ(ಶೂನ್ಯಸೂಚಕ);
    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->ಅದಲುಬದಲು_LFNLState_ಕೋಷ್ಟಕ==ಶೂನ್ಯಸೂಚಕ) {
        mbcs_ಕೋಷ್ಟಕ->ಅದಲುಬದಲು_LFNLState_ಕೋಷ್ಟಕ=new_State_ಕೋಷ್ಟಕ;
        mbcs_ಕೋಷ್ಟಕ->ಅದಲುಬದಲು_LFNLFrom_Unicode_ಬೈಟುಗಳು = reinterpret_cast<uint8_t*>(newResults);
        mbcs_ಕೋಷ್ಟಕ->ಅದಲುಬದಲು_LFNLName=name;

        new_State_ಕೋಷ್ಟಕ=ಶೂನ್ಯಸೂಚಕ;
    }
    icu::umtx_unlock(ಶೂನ್ಯಸೂಚಕ);

    /* release the allocated memory if another thread beat us to it */
    ಆದರೆ(new_State_ಕೋಷ್ಟಕ!=ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(new_State_ಕೋಷ್ಟಕ);
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* reconstitute omitted fromUnicode data ------------------------------------ */

/* for details, compare with genmbcs.c MBCSAddFromUnicode() and transformEUC() */
ಸ್ಥಿರ UBool U_CALLCONV
writeStage3Roundtrip(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, uint32_t value, UChar32 ಸಂಕೇತ_Points[32]) {
    UConverterMBCSTable *mbcs_ಕೋಷ್ಟಕ=(UConverterMBCSTable *)context;
    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ;
    uint32_t *stage2;
    uint8_t *ಬೈಟುಗಳು, *p;
    UChar32 c;
    int32_t i, st3;

    ಕೋಷ್ಟಕ=mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಕೋಷ್ಟಕ;
    ಬೈಟುಗಳು = const_cast<uint8_t*>(mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಬೈಟುಗಳು);

    /* for EUC outputTypes, modify the value like genmbcs.c's transformEUC() */
    ಆಯ್ಕೆ(mbcs_ಕೋಷ್ಟಕ->outputType) {
    ಸ್ಥಿತಿ MBCS_OUTPUT_3_EUC:
        ಆದರೆ(value<=0xffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0x8effff) {
            /* code set 2 */
            value&=0x7fff;
        } ಇಲ್ಲದಿದ್ದರೆ /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7f;
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MBCS_OUTPUT_4_EUC:
        ಆದರೆ(value<=0xffffff) {
            /* short sequences are stored directly */
            /* code set 0 or 1 */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0x8effffff) {
            /* code set 2 */
            value&=0x7fffff;
        } ಇಲ್ಲದಿದ್ದರೆ /* first byte is 0x8f */ {
            /* code set 3 */
            value&=0xff7fff;
        }
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }

    ಗಾಗಿ(i=0; i<=0x1f; ++value, ++i) {
        c=ಸಂಕೇತ_Points[i];
        ಆದರೆ(c<0) {
            ಮುಂದುವರಿ;
        }

        /* locate the stage 2 & 3 data */
        stage2=((uint32_t *)ಕೋಷ್ಟಕ)+ಕೋಷ್ಟಕ[c>>10]+((c>>4)&0x3f);
        p=ಬೈಟುಗಳು;
        st3 = static_cast<int32_t>(static_cast<uint16_t>(*stage2)) * 16 + (c & 0xf);

        /* write the codepage bytes into stage 3 */
        ಆಯ್ಕೆ(mbcs_ಕೋಷ್ಟಕ->outputType) {
        ಸ್ಥಿತಿ MBCS_OUTPUT_3:
        ಸ್ಥಿತಿ MBCS_OUTPUT_4_EUC:
            p+=st3*3;
            p[0] = static_cast<uint8_t>(value >> 16);
            p[1] = static_cast<uint8_t>(value >> 8);
            p[2] = static_cast<uint8_t>(value);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ MBCS_OUTPUT_4:
            reinterpret_cast<uint32_t*>(p)[st3] = value;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            /* 2 bytes per character */
            reinterpret_cast<uint16_t*>(p)[st3] = static_cast<uint16_t>(value);
            ನಿಲ್ಲಿಸು;
        }

        /* set the roundtrip flag */
        *stage2|=(1UL<<(16+(c&0xf)));
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
 }

ಸ್ಥಿರ ಶೂನ್ಯ
reconstitute_ದತ್ತಾಂಶ(UConverterMBCSTable *mbcs_ಕೋಷ್ಟಕ,
                 uint32_t stage1_ಉದ್ದಳತೆ, uint32_t stage2_ಉದ್ದಳತೆ,
                 uint32_t ತುಂಬಿದ_Stage2_ಉದ್ದಳತೆ,  /* lengths are numbers of units, not bytes */
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    uint16_t *stage1;
    uint32_t *stage2;
    uint32_t ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=stage1_ಉದ್ದಳತೆ*2+ತುಂಬಿದ_Stage2_ಉದ್ದಳತೆ*4+mbcs_ಕೋಷ್ಟಕ->from_UBytes_ಉದ್ದಳತೆ;
    mbcs_ಕೋಷ್ಟಕ->reconstituted_ದತ್ತಾಂಶ = static_cast<uint8_t*>(uprv_malloc(ದತ್ತಾಂಶ_ಉದ್ದಳತೆ));
    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->reconstituted_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    uprv_memset(mbcs_ಕೋಷ್ಟಕ->reconstituted_ದತ್ತಾಂಶ, 0, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);

    /* copy existing data and reroute the pointers */
    stage1 = reinterpret_cast<uint16_t*>(mbcs_ಕೋಷ್ಟಕ->reconstituted_ದತ್ತಾಂಶ);
    uprv_memcpy(stage1, mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಕೋಷ್ಟಕ, stage1_ಉದ್ದಳತೆ*2);

    stage2 = reinterpret_cast<uint32_t*>(stage1 + stage1_ಉದ್ದಳತೆ);
    uprv_memcpy(stage2+(ತುಂಬಿದ_Stage2_ಉದ್ದಳತೆ-stage2_ಉದ್ದಳತೆ),
                mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಕೋಷ್ಟಕ+stage1_ಉದ್ದಳತೆ,
                stage2_ಉದ್ದಳತೆ*4);

    mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಕೋಷ್ಟಕ=stage1;
    mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಬೈಟುಗಳು = reinterpret_cast<uint8_t*>(stage2 + ತುಂಬಿದ_Stage2_ಉದ್ದಳತೆ);

    /* indexes into stage 2 count from the bottom of the fromUnicodeTable */
    stage2 = reinterpret_cast<uint32_t*>(stage1);

    /* reconstitute the initial part of stage 2 from the mbcsIndex */
    {
        int32_t stage_UTF8_ಉದ್ದಳತೆ = (static_cast<int32_t>(mbcs_ಕೋಷ್ಟಕ->maxFastUChar) + 1) >> 6;
        int32_t stage_UTF8_ಸೂಚ್ಯಂಕ=0;
        int32_t st1, st2, st3, i;

        ಗಾಗಿ(st1=0; stage_UTF8_ಸೂಚ್ಯಂಕ<stage_UTF8_ಉದ್ದಳತೆ; ++st1) {
            st2=stage1[st1];
            ಆದರೆ (st2 != static_cast<int32_t>(stage1_ಉದ್ದಳತೆ) / 2) {
                /* each stage 2 block has 64 entries corresponding to 16 entries in the mbcsIndex */
                ಗಾಗಿ(i=0; i<16; ++i) {
                    st3=mbcs_ಕೋಷ್ಟಕ->mbcs_ಸೂಚ್ಯಂಕ[stage_UTF8_ಸೂಚ್ಯಂಕ++];
                    ಆದರೆ(st3!=0) {
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
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* no stage 3 block, skip */
                        st2+=4;
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* no stage 2 block, skip */
                stage_UTF8_ಸೂಚ್ಯಂಕ+=16;
            }
        }
    }

    /* reconstitute fromUnicodeBytes with roundtrips from toUnicode data */
    ucnv_MBCSEnum_ಗಮ್ಯ_Unicode(mbcs_ಕೋಷ್ಟಕ, writeStage3Roundtrip, mbcs_ಕೋಷ್ಟಕ, p_Error_ಸಂಕೇತ);
}

/* MBCS setup functions ----------------------------------------------------- */

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_MBCSLoad(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ,
          UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
          ಸ್ಥಿರಾಂಕ uint8_t *raw,
          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UDataInfo info;
    UConverterMBCSTable *mbcs_ಕೋಷ್ಟಕ=&shared_ದತ್ತಾಂಶ->mbcs;
    _MBCSHeader *ಶೀರ್ಷಿಕೆ=(_MBCSHeader *)raw;
    uint32_t ಸ್ಥಾನಾಂತರ;
    uint32_t ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ;
    UBool noFromU=ಸುಳ್ಳು;

    ಆದರೆ(ಶೀರ್ಷಿಕೆ->ಆವೃತ್ತಿ[0]==4) {
        ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ=MBCS_ಶೀರ್ಷಿಕೆ_V4_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಶೀರ್ಷಿಕೆ->ಆವೃತ್ತಿ[0]==5 && ಶೀರ್ಷಿಕೆ->ಆವೃತ್ತಿ[1]>=3 &&
              (ಶೀರ್ಷಿಕೆ->options&MBCS_OPT_UNKNOWN_INCOMPATIBLE_ಮುಖವಾಡ)==0) {
        ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ=ಶೀರ್ಷಿಕೆ->options&MBCS_OPT_ಉದ್ದಳತೆ_ಮುಖವಾಡ;
        noFromU = static_cast<UBool>((ಶೀರ್ಷಿಕೆ->options & MBCS_OPT_NO_FROM_U) != 0);
    } ಇಲ್ಲದಿದ್ದರೆ {
        *p_Error_ಸಂಕೇತ=U_INVALID_ಕೋಷ್ಟಕ_FORMAT;
        ಹಿಂತಿರುಗಿ;
    }

    mbcs_ಕೋಷ್ಟಕ->outputType = static_cast<uint8_t>(ಶೀರ್ಷಿಕೆ->flags);
    ಆದರೆ(noFromU && mbcs_ಕೋಷ್ಟಕ->outputType==MBCS_OUTPUT_1) {
        *p_Error_ಸಂಕೇತ=U_INVALID_ಕೋಷ್ಟಕ_FORMAT;
        ಹಿಂತಿರುಗಿ;
    }

    /* extension data, header version 4.2 and higher */
    ಸ್ಥಾನಾಂತರ=ಶೀರ್ಷಿಕೆ->flags>>8;
    ಆದರೆ(ಸ್ಥಾನಾಂತರ!=0) {
        mbcs_ಕೋಷ್ಟಕ->extIndexes = reinterpret_cast<ಸ್ಥಿರಾಂಕ int32_t*>(raw + ಸ್ಥಾನಾಂತರ);
    }

    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->outputType==MBCS_OUTPUT_EXT_ONLY) {
        UConverter_ತುಂಬು_ವಾದಗಳು ವಾದಗಳು_2=UCNV_ತುಂಬು_ವಾದಗಳು_INITIALIZER;
        UConverter_Shared_ದತ್ತಾಂಶ *ತಳ_Shared_ದತ್ತಾಂಶ;
        ಸ್ಥಿರಾಂಕ int32_t *extIndexes;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ತಳ_Name;

        /* extension-only file, load the base table and set values appropriately */
        ಆದರೆ((extIndexes=mbcs_ಕೋಷ್ಟಕ->extIndexes)==ಶೂನ್ಯಸೂಚಕ) {
            /* extension-only file without extension */
            *p_Error_ಸಂಕೇತ=U_INVALID_ಕೋಷ್ಟಕ_FORMAT;
            ಹಿಂತಿರುಗಿ;
        }

        ಆದರೆ(p_ವಾದಗಳು->nestedLoads!=1) {
            /* an extension table must not be loaded as a base table */
            *p_Error_ಸಂಕೇತ=U_INVALID_ಕೋಷ್ಟಕ_FILE;
            ಹಿಂತಿರುಗಿ;
        }

        /* load the base table */
        ತಳ_Name = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಶೀರ್ಷಿಕೆ) + ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ * 4;
        ಆದರೆ(0==uprv_strcmp(ತಳ_Name, shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->name)) {
            /* forbid loading this same extension-only file */
            *p_Error_ಸಂಕೇತ=U_INVALID_ಕೋಷ್ಟಕ_FORMAT;
            ಹಿಂತಿರುಗಿ;
        }

        /* TODO parse package name out of the prefix of the base name in the extension .cnv file? */
        ವಾದಗಳು_2.size=ಗಾತ್ರ(UConverter_ತುಂಬು_ವಾದಗಳು);
        ವಾದಗಳು_2.nestedLoads=2;
        ವಾದಗಳು_2.onlyTestIsLoadable=p_ವಾದಗಳು->onlyTestIsLoadable;
        ವಾದಗಳು_2.reserved=p_ವಾದಗಳು->reserved;
        ವಾದಗಳು_2.options=p_ವಾದಗಳು->options;
        ವಾದಗಳು_2.pkg=p_ವಾದಗಳು->pkg;
        ವಾದಗಳು_2.name=ತಳ_Name;
        ತಳ_Shared_ದತ್ತಾಂಶ=ucnv_ತುಂಬು(&ವಾದಗಳು_2, p_Error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ( ತಳ_Shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->conversionType!=UCNV_MBCS ||
            ತಳ_Shared_ದತ್ತಾಂಶ->mbcs.ತಳ_Shared_ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ
        ) {
            ucnv_unload(ತಳ_Shared_ದತ್ತಾಂಶ);
            *p_Error_ಸಂಕೇತ=U_INVALID_ಕೋಷ್ಟಕ_FORMAT;
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ(p_ವಾದಗಳು->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            ucnv_unload(ತಳ_Shared_ದತ್ತಾಂಶ);
            ಹಿಂತಿರುಗಿ;
        }

        /* copy the base table data */
        uprv_memcpy(mbcs_ಕೋಷ್ಟಕ, &ತಳ_Shared_ದತ್ತಾಂಶ->mbcs, ಗಾತ್ರ(UConverterMBCSTable));

        /* overwrite values with relevant ones for the extension converter */
        mbcs_ಕೋಷ್ಟಕ->ತಳ_Shared_ದತ್ತಾಂಶ=ತಳ_Shared_ದತ್ತಾಂಶ;
        mbcs_ಕೋಷ್ಟಕ->extIndexes=extIndexes;

        /*
         * It would be possible to share the swapLFNL data with a base converter,
         * but the generated name would have to be different, and the memory
         * would have to be free'd only once.
         * It is easier to just create the data for the extension converter
         * separately when it is requested.
         */
        mbcs_ಕೋಷ್ಟಕ->ಅದಲುಬದಲು_LFNLState_ಕೋಷ್ಟಕ=ಶೂನ್ಯಸೂಚಕ;
        mbcs_ಕೋಷ್ಟಕ->ಅದಲುಬದಲು_LFNLFrom_Unicode_ಬೈಟುಗಳು=ಶೂನ್ಯಸೂಚಕ;
        mbcs_ಕೋಷ್ಟಕ->ಅದಲುಬದಲು_LFNLName=ಶೂನ್ಯಸೂಚಕ;

        /*
         * The reconstitutedData must be deleted only when the base converter
         * is unloaded.
         */
        mbcs_ಕೋಷ್ಟಕ->reconstituted_ದತ್ತಾಂಶ=ಶೂನ್ಯಸೂಚಕ;

        /*
         * Set a special, runtime-only outputType if the extension converter
         * is a DBCS version of a base converter that also maps single bytes.
         */
        ಆದರೆ( shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->conversionType==UCNV_DBCS ||
                (shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->conversionType==UCNV_MBCS &&
                 shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->min_ಬೈಟುಗಳು_ಪ್ರತಿ_Char>=2)
        ) {
            ಆದರೆ(ತಳ_Shared_ದತ್ತಾಂಶ->mbcs.outputType==MBCS_OUTPUT_2_SISO) {
                /* the base converter is SI/SO-stateful */
                int32_t ನಮೂದು;

                /* get the dbcs state from the state table entry for SO=0x0e */
                ನಮೂದು=mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ[0][0xe];
                ಆದರೆ( MBCS_ನಮೂದು_IS_FINAL(ನಮೂದು) &&
                    MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು)==MBCS_STATE_CHANGE_ONLY &&
                    MBCS_ನಮೂದು_FINAL_STATE(ನಮೂದು)!=0
                ) {
                    mbcs_ಕೋಷ್ಟಕ->dbcsOnlyState = static_cast<uint8_t>(MBCS_ನಮೂದು_FINAL_STATE(ನಮೂದು));

                    mbcs_ಕೋಷ್ಟಕ->outputType=MBCS_OUTPUT_DBCS_ONLY;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(
                ತಳ_Shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->conversionType==UCNV_MBCS &&
                ತಳ_Shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->min_ಬೈಟುಗಳು_ಪ್ರತಿ_Char==1 &&
                ತಳ_Shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->max_ಬೈಟುಗಳು_ಪ್ರತಿ_Char==2 &&
                mbcs_ಕೋಷ್ಟಕ->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು<=127
            ) {
                /* non-stateful base converter, need to modify the state table */
                int32_t (*new_State_ಕೋಷ್ಟಕ)[256];
                int32_t *state;
                int32_t i, count;

                /* allocate a new state table and copy the base state table contents */
                count=mbcs_ಕೋಷ್ಟಕ->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು;
                new_State_ಕೋಷ್ಟಕ = static_cast<int32_t(*)[256]>(uprv_malloc((count + 1) * 1024));
                ಆದರೆ(new_State_ಕೋಷ್ಟಕ==ಶೂನ್ಯಸೂಚಕ) {
                    ucnv_unload(ತಳ_Shared_ದತ್ತಾಂಶ);
                    *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    ಹಿಂತಿರುಗಿ;
                }

                uprv_memcpy(new_State_ಕೋಷ್ಟಕ, mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ, count*1024);

                /* change all final single-byte entries to go to a new all-illegal state */
                state=new_State_ಕೋಷ್ಟಕ[0];
                ಗಾಗಿ(i=0; i<256; ++i) {
                    ಆದರೆ(MBCS_ನಮೂದು_IS_FINAL(state[i])) {
                        state[i]=MBCS_ನಮೂದು_TRANSITION(count, 0);
                    }
                }

                /* build the new all-illegal state */
                state=new_State_ಕೋಷ್ಟಕ[count];
                ಗಾಗಿ(i=0; i<256; ++i) {
                    state[i]=MBCS_ನಮೂದು_FINAL(0, MBCS_STATE_ILLEGAL, 0);
                }
                mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ=(ಸ್ಥಿರಾಂಕ int32_t (*)[256])new_State_ಕೋಷ್ಟಕ;
                mbcs_ಕೋಷ್ಟಕ->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು = static_cast<uint8_t>(count + 1);
                mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ_Owned=ನಿಜ;

                mbcs_ಕೋಷ್ಟಕ->outputType=MBCS_OUTPUT_DBCS_ONLY;
            }
        }

        /*
         * unlike below for files with base tables, do not get the unicodeMask
         * from the sharedData; instead, use the base table's unicodeMask,
         * which we copied in the memcpy above;
         * this is necessary because the static data unicodeMask, especially
         * the UCNV_HAS_SUPPLEMENTARY flag, is part of the base table data
         */
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* conversion file with a base table; an additional extension table is optional */
        /* make sure that the output type is known */
        ಆಯ್ಕೆ(mbcs_ಕೋಷ್ಟಕ->outputType) {
        ಸ್ಥಿತಿ MBCS_OUTPUT_1:
        ಸ್ಥಿತಿ MBCS_OUTPUT_2:
        ಸ್ಥಿತಿ MBCS_OUTPUT_3:
        ಸ್ಥಿತಿ MBCS_OUTPUT_4:
        ಸ್ಥಿತಿ MBCS_OUTPUT_3_EUC:
        ಸ್ಥಿತಿ MBCS_OUTPUT_4_EUC:
        ಸ್ಥಿತಿ MBCS_OUTPUT_2_SISO:
            /* OK */
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            *p_Error_ಸಂಕೇತ=U_INVALID_ಕೋಷ್ಟಕ_FORMAT;
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ(p_ವಾದಗಳು->onlyTestIsLoadable) {
            /*
             * Exit as soon as we know that we can load the converter
             * and the format is valid and supported.
             * The worst that can happen in the following code is a memory
             * allocation error.
             */
            ಹಿಂತಿರುಗಿ;
        }

        mbcs_ಕೋಷ್ಟಕ->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು = static_cast<uint8_t>(ಶೀರ್ಷಿಕೆ->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು);
        mbcs_ಕೋಷ್ಟಕ->ಎಣಿಕೆ_ಗಮ್ಯ_UFallbacks=ಶೀರ್ಷಿಕೆ->ಎಣಿಕೆ_ಗಮ್ಯ_UFallbacks;
        mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ = reinterpret_cast<ಸ್ಥಿರಾಂಕ int32_t(*)[256]>(raw + ಶೀರ್ಷಿಕೆ_ಉದ್ದಳತೆ * 4);
        mbcs_ಕೋಷ್ಟಕ->ಗಮ್ಯ_UFallbacks = reinterpret_cast<ಸ್ಥಿರಾಂಕ _MBCSToUFallback*>(mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ + ಶೀರ್ಷಿಕೆ->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು);
        mbcs_ಕೋಷ್ಟಕ->unicode_ಸಂಕೇತ_Units = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(raw + ಶೀರ್ಷಿಕೆ->ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_UCode_Units);

        mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಕೋಷ್ಟಕ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(raw + ಶೀರ್ಷಿಕೆ->ಸ್ಥಾನಾಂತರ_From_UTable);
        mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಬೈಟುಗಳು = raw + ಶೀರ್ಷಿಕೆ->ಸ್ಥಾನಾಂತರ_From_UBytes;
        mbcs_ಕೋಷ್ಟಕ->from_UBytes_ಉದ್ದಳತೆ=ಶೀರ್ಷಿಕೆ->from_UBytes_ಉದ್ದಳತೆ;

        /*
         * converter versions 6.1 and up contain a unicodeMask that is
         * used here to select the most efficient function implementations
         */
        info.size=ಗಾತ್ರ(UDataInfo);
        udata_getInfo((UData_ಸ್ಮರಣೆ *)shared_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ_ಸ್ಮರಣೆ, &info);
        ಆದರೆ(info.format_ಆವೃತ್ತಿ[0]>6 || (info.format_ಆವೃತ್ತಿ[0]==6 && info.format_ಆವೃತ್ತಿ[1]>=1)) {
            /* mask off possible future extensions to be safe */
            mbcs_ಕೋಷ್ಟಕ->unicode_ಮುಖವಾಡ = static_cast<uint8_t>(shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->unicode_ಮುಖವಾಡ & 3);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* for older versions, assume worst case: contains anything possible (prevent over-optimizations) */
            mbcs_ಕೋಷ್ಟಕ->unicode_ಮುಖವಾಡ=UCNV_HAS_SUPPLEMENTARY|UCNV_HAS_SURROGATES;
        }

        /*
         * _MBCSHeader.version 4.3 adds utf8Friendly data structures.
         * Check for the header version, SBCS vs. MBCS, and for whether the
         * data structures are optimized for code points as high as what the
         * runtime code is designed for.
         * The implementation does not handle mapping tables with entries for
         * unpaired surrogates.
         */
        ಆದರೆ( ಶೀರ್ಷಿಕೆ->ಆವೃತ್ತಿ[1]>=3 &&
            (mbcs_ಕೋಷ್ಟಕ->unicode_ಮುಖವಾಡ&UCNV_HAS_SURROGATES)==0 &&
            (mbcs_ಕೋಷ್ಟಕ->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು==1 ?
                (ಶೀರ್ಷಿಕೆ->ಆವೃತ್ತಿ[2]>=(SBCS_FAST_MAX>>8)) :
                (ಶೀರ್ಷಿಕೆ->ಆವೃತ್ತಿ[2]>=(MBCS_FAST_MAX>>8))
            )
        ) {
            mbcs_ಕೋಷ್ಟಕ->utf8Friendly=ನಿಜ;

            ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು==1) {
                /*
                 * SBCS: Stage 3 is allocated in 64-entry blocks for U+0000..SBCS_FAST_MAX or higher.
                 * Build a table with indexes to each block, to be used instead of
                 * the regular stage 1/2 table.
                 */
                int32_t i;
                ಗಾಗಿ(i=0; i<(SBCS_FAST_ಎಲ್ಲೆ>>6); ++i) {
                    mbcs_ಕೋಷ್ಟಕ->sbcs_ಸೂಚ್ಯಂಕ[i]=mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಕೋಷ್ಟಕ[mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಕೋಷ್ಟಕ[i>>4]+((i<<2)&0x3c)];
                }
                /* set SBCS_FAST_MAX to reflect the reach of sbcsIndex[] even if header->version[2]>(SBCS_FAST_MAX>>8) */
                mbcs_ಕೋಷ್ಟಕ->maxFastUChar=SBCS_FAST_MAX;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /*
                 * MBCS: Stage 3 is allocated in 64-entry blocks for U+0000..MBCS_FAST_MAX or higher.
                 * The .cnv file is prebuilt with an additional stage table with indexes
                 * to each block.
                 */
                mbcs_ಕೋಷ್ಟಕ->mbcs_ಸೂಚ್ಯಂಕ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(
                    mbcs_ಕೋಷ್ಟಕ->from_Unicode_ಬೈಟುಗಳು +
                     (noFromU ? 0 : mbcs_ಕೋಷ್ಟಕ->from_UBytes_ಉದ್ದಳತೆ));
                mbcs_ಕೋಷ್ಟಕ->maxFastUChar = (static_cast<char16_t>(ಶೀರ್ಷಿಕೆ->ಆವೃತ್ತಿ[2]) << 8) | 0xff;
            }
        }

        /* calculate a bit set of 4 ASCII characters per bit that round-trip to ASCII bytes */
        {
            uint32_t asciiRoundtrips=0xffffffff;
            int32_t i;

            ಗಾಗಿ(i=0; i<0x80; ++i) {
                ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ[0][i]!=MBCS_ನಮೂದು_FINAL(0, MBCS_STATE_VALID_ನೇರ_16, i)) {
                    asciiRoundtrips &= ~(static_cast<uint32_t>(1) << (i >> 2));
                }
            }
            mbcs_ಕೋಷ್ಟಕ->asciiRoundtrips=asciiRoundtrips;
        }

        ಆದರೆ(noFromU) {
            uint32_t stage1_ಉದ್ದಳತೆ=
                mbcs_ಕೋಷ್ಟಕ->unicode_ಮುಖವಾಡ&UCNV_HAS_SUPPLEMENTARY ?
                    0x440 : 0x40;
            uint32_t stage2_ಉದ್ದಳತೆ=
                (ಶೀರ್ಷಿಕೆ->ಸ್ಥಾನಾಂತರ_From_UBytes-ಶೀರ್ಷಿಕೆ->ಸ್ಥಾನಾಂತರ_From_UTable)/4-
                stage1_ಉದ್ದಳತೆ/2;
            reconstitute_ದತ್ತಾಂಶ(mbcs_ಕೋಷ್ಟಕ, stage1_ಉದ್ದಳತೆ, stage2_ಉದ್ದಳತೆ, ಶೀರ್ಷಿಕೆ->ತುಂಬಿದ_Stage2_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
        }
    }

    /* Set the impl pointer here so that it is set for both extension-only and base tables. */
    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->utf8Friendly) {
        ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು==1) {
            shared_ದತ್ತಾಂಶ->impl=&_SBCSUTF8Impl;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->outputType==MBCS_OUTPUT_2) {
                shared_ದತ್ತಾಂಶ->impl=&_DBCSUTF8Impl;
            }
        }
    }

    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->outputType==MBCS_OUTPUT_DBCS_ONLY || mbcs_ಕೋಷ್ಟಕ->outputType==MBCS_OUTPUT_2_SISO) {
        /*
         * MBCS_OUTPUT_DBCS_ONLY: No SBCS mappings, therefore ASCII does not roundtrip.
         * MBCS_OUTPUT_2_SISO: Bypass the ASCII fastpath to handle prevLength correctly.
         */
        mbcs_ಕೋಷ್ಟಕ->asciiRoundtrips=0;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_MBCSUnload(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ) {
    UConverterMBCSTable *mbcs_ಕೋಷ್ಟಕ=&shared_ದತ್ತಾಂಶ->mbcs;

    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->ಅದಲುಬದಲು_LFNLState_ಕೋಷ್ಟಕ!=ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(mbcs_ಕೋಷ್ಟಕ->ಅದಲುಬದಲು_LFNLState_ಕೋಷ್ಟಕ);
    }
    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ_Owned) {
        uprv_free((ಶೂನ್ಯ *)mbcs_ಕೋಷ್ಟಕ->state_ಕೋಷ್ಟಕ);
    }
    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->ತಳ_Shared_ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ) {
        ucnv_unload(mbcs_ಕೋಷ್ಟಕ->ತಳ_Shared_ದತ್ತಾಂಶ);
    }
    ಆದರೆ(mbcs_ಕೋಷ್ಟಕ->reconstituted_ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(mbcs_ಕೋಷ್ಟಕ->reconstituted_ದತ್ತಾಂಶ);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_MBCSOpen(UConverter *cnv,
              UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverterMBCSTable *mbcs_ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ int32_t *extIndexes;
    uint8_t outputType;
    int8_t max_ಬೈಟುಗಳು_ಪ್ರತಿ_UChar;

    ಆದರೆ(p_ವಾದಗಳು->onlyTestIsLoadable) {
        ಹಿಂತಿರುಗಿ;
    }

    mbcs_ಕೋಷ್ಟಕ=&cnv->shared_ದತ್ತಾಂಶ->mbcs;
    outputType=mbcs_ಕೋಷ್ಟಕ->outputType;

    ಆದರೆ(outputType==MBCS_OUTPUT_DBCS_ONLY) {
        /* the swaplfnl option does not apply, remove it */
        cnv->options=p_ವಾದಗಳು->options&=~UCNV_OPTION_ಅದಲುಬದಲು_LFNL;
    }

    ಆದರೆ((p_ವಾದಗಳು->options&UCNV_OPTION_ಅದಲುಬದಲು_LFNL)!=0) {
        /* do this because double-checked locking is broken */
        UBool isCached;

        icu::umtx_ಬೀಗ(ಶೂನ್ಯಸೂಚಕ);
        isCached=mbcs_ಕೋಷ್ಟಕ->ಅದಲುಬದಲು_LFNLState_ಕೋಷ್ಟಕ!=ಶೂನ್ಯಸೂಚಕ;
        icu::umtx_unlock(ಶೂನ್ಯಸೂಚಕ);

        ಆದರೆ(!isCached) {
            ಆದರೆ(!_EBCDICSwapLFNL(cnv->shared_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ)) {
                ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                    ಹಿಂತಿರುಗಿ; /* something went wrong */
                }

                /* the option does not apply, remove it */
                cnv->options=p_ವಾದಗಳು->options&=~UCNV_OPTION_ಅದಲುಬದಲು_LFNL;
            }
        }
    }

    ಆದರೆ(uprv_strstr(p_ವಾದಗಳು->name, "18030")!=ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(uprv_strstr(p_ವಾದಗಳು->name, "gb18030")!=ಶೂನ್ಯಸೂಚಕ || uprv_strstr(p_ವಾದಗಳು->name, "GB18030")!=ಶೂನ್ಯಸೂಚಕ) {
            /* set a flag for GB 18030 mode, which changes the callback behavior */
            cnv->options|=_MBCS_OPTION_GB18030;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uprv_strstr(p_ವಾದಗಳು->name, "KEIS")!=ಶೂನ್ಯಸೂಚಕ) || (uprv_strstr(p_ವಾದಗಳು->name, "keis")!=ಶೂನ್ಯಸೂಚಕ)) {
        /* set a flag for KEIS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_KEIS;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uprv_strstr(p_ವಾದಗಳು->name, "JEF")!=ಶೂನ್ಯಸೂಚಕ) || (uprv_strstr(p_ವಾದಗಳು->name, "jef")!=ಶೂನ್ಯಸೂಚಕ)) {
        /* set a flag for JEF converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JEF;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uprv_strstr(p_ವಾದಗಳು->name, "JIPS")!=ಶೂನ್ಯಸೂಚಕ) || (uprv_strstr(p_ವಾದಗಳು->name, "jips")!=ಶೂನ್ಯಸೂಚಕ)) {
        /* set a flag for JIPS converter, which changes the SI/SO character sequence */
        cnv->options|=_MBCS_OPTION_JIPS;
    }

    /* fix maxBytesPerUChar depending on outputType and options etc. */
    ಆದರೆ(outputType==MBCS_OUTPUT_2_SISO) {
        cnv->max_ಬೈಟುಗಳು_ಪ್ರತಿ_UChar=3; /* SO+DBCS */
    }

    extIndexes=mbcs_ಕೋಷ್ಟಕ->extIndexes;
    ಆದರೆ(extIndexes!=ಶೂನ್ಯಸೂಚಕ) {
        max_ಬೈಟುಗಳು_ಪ್ರತಿ_UChar = static_cast<int8_t>(UCNV_GET_MAX_ಬೈಟುಗಳು_ಪ್ರತಿ_UCHAR(extIndexes));
        ಆದರೆ(outputType==MBCS_OUTPUT_2_SISO) {
            ++max_ಬೈಟುಗಳು_ಪ್ರತಿ_UChar; /* SO + multiple DBCS */
        }

        ಆದರೆ(max_ಬೈಟುಗಳು_ಪ್ರತಿ_UChar>cnv->max_ಬೈಟುಗಳು_ಪ್ರತಿ_UChar) {
            cnv->max_ಬೈಟುಗಳು_ಪ್ರತಿ_UChar=max_ಬೈಟುಗಳು_ಪ್ರತಿ_UChar;
        }
    }

#ಆದರೆ 0
    /*
     * documentation of UConverter fields used for status
     * all of these fields are (re)set to 0 by ucnv_bld.c and ucnv_reset()
     */

    /* toUnicode */
    cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=0;     /*ಸ್ಥಾನಾಂತರ*/
    cnv->ಬಹುಲಕ=0;                /* state */
    cnv->ಗಮ್ಯ_ULength=0;           /* byteIndex */

    /* fromUnicode */
    cnv->fromUChar32=0;
    cnv->from_Unicode_ಸ್ಥಿತಿ=1;   /* prevLength */
#ಆದರೆ_ಅಂತ್ಯ
}

U_CDECL_BEGIN

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_CALLCONV
ucnv_MBCSGetName(ಸ್ಥಿರಾಂಕ UConverter *cnv) {
    ಆದರೆ((cnv->options&UCNV_OPTION_ಅದಲುಬದಲು_LFNL)!=0 && cnv->shared_ದತ್ತಾಂಶ->mbcs.ಅದಲುಬದಲು_LFNLName!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ cnv->shared_ದತ್ತಾಂಶ->mbcs.ಅದಲುಬದಲು_LFNLName;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ cnv->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->name;
    }
}
U_CDECL_ಕೊನೆ


/* MBCS-to-Unicode conversion functions ------------------------------------- */

ಸ್ಥಿರ UChar32 U_CALLCONV
ucnv_MBCSGetFallback(UConverterMBCSTable *mbcs_ಕೋಷ್ಟಕ, uint32_t ಸ್ಥಾನಾಂತರ) {
    ಸ್ಥಿರಾಂಕ _MBCSToUFallback *ಗಮ್ಯ_UFallbacks;
    uint32_t i, ಆರಂಭ, ಎಲ್ಲೆ;

    ಎಲ್ಲೆ=mbcs_ಕೋಷ್ಟಕ->ಎಣಿಕೆ_ಗಮ್ಯ_UFallbacks;
    ಆದರೆ(ಎಲ್ಲೆ>0) {
        /* do a binary search for the fallback mapping */
        ಗಮ್ಯ_UFallbacks=mbcs_ಕೋಷ್ಟಕ->ಗಮ್ಯ_UFallbacks;
        ಆರಂಭ=0;
        ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ-1) {
            i=(ಆರಂಭ+ಎಲ್ಲೆ)/2;
            ಆದರೆ(ಸ್ಥಾನಾಂತರ<ಗಮ್ಯ_UFallbacks[i].ಸ್ಥಾನಾಂತರ) {
                ಎಲ್ಲೆ=i;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆರಂಭ=i;
            }
        }

        /* did we really find it? */
        ಆದರೆ(ಸ್ಥಾನಾಂತರ==ಗಮ್ಯ_UFallbacks[ಆರಂಭ].ಸ್ಥಾನಾಂತರ) {
            ಹಿಂತಿರುಗಿ ಗಮ್ಯ_UFallbacks[ಆರಂಭ].ಸಂಕೇತ_ಬಿಂದು;
        }
    }

    ಹಿಂತಿರುಗಿ 0xfffe;
}

/* This version of ucnv_MBCSToUnicodeWithOffsets() is optimized for single-byte, single-state codepages. */
ಸ್ಥಿರ ಶೂನ್ಯ
ucnv_MBCSSingle_ಗಮ್ಯ_Unicode_With_Offsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                                UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    char16_t *ಗುರಿ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ;
    int32_t *offsets;

    ಸ್ಥಿರಾಂಕ int32_t (*state_ಕೋಷ್ಟಕ)[256];

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ;

    int32_t ನಮೂದು;
    char16_t c;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಮೂಲ);
    ಮೂಲ_ಎಲ್ಲೆ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ);
    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ;
    offsets=p_ವಾದಗಳು->offsets;

    ಆದರೆ((cnv->options&UCNV_OPTION_ಅದಲುಬದಲು_LFNL)!=0) {
        state_ಕೋಷ್ಟಕ=(ಸ್ಥಿರಾಂಕ int32_t (*)[256])cnv->shared_ದತ್ತಾಂಶ->mbcs.ಅದಲುಬದಲು_LFNLState_ಕೋಷ್ಟಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        state_ಕೋಷ್ಟಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.state_ಕೋಷ್ಟಕ;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    /* conversion loop */
    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ಆದರೆ(ಗುರಿ>=ಗುರಿ_ಎಲ್ಲೆ) {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }

        ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        ಆದರೆ(MBCS_ನಮೂದು_FINAL_IS_VALID_ನೇರ_16(ನಮೂದು)) {
            /* output BMP code point */
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }

            /* normal end of action codes: prepare for a new character */
            ++ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಮುಂದುವರಿ;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು));
        ಆದರೆ(action==MBCS_STATE_VALID_ನೇರ_20 ||
           (action==MBCS_STATE_FALLBACK_ನೇರ_20 && UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv))
        ) {
            ನಮೂದು=MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ(ನಮೂದು);
            /* output surrogate pair */
            *ಗುರಿ++ = static_cast<char16_t>(0xd800 | static_cast<char16_t>(ನಮೂದು >> 10));
            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }
            c = static_cast<char16_t>(0xdc00 | static_cast<char16_t>(ನಮೂದು & 0x3ff));
            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                *ಗುರಿ++=c;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* target overflow */
                cnv->UChar_Error_ಬಫರ್[0]=c;
                cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }

            ++ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_FALLBACK_ನೇರ_16) {
            ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }

                ++ಮೂಲ_ಸೂಚ್ಯಂಕ;
                ಮುಂದುವರಿ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* reserved, must never occur */
            ++ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಮುಂದುವರಿ;
        }

        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            /* callback(illegal) */
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_ವಾದಗಳು->ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಮೂಲ);
            cnv->ಗಮ್ಯ_UBytes[0]=*(ಮೂಲ-1);
            cnv->ಗಮ್ಯ_ULength=ext_ಗಮ್ಯ_U(cnv, cnv->shared_ದತ್ತಾಂಶ,
                                    1, &ಮೂಲ, ಮೂಲ_ಎಲ್ಲೆ,
                                    &ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ,
                                    &offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
                                    p_ವಾದಗಳು->flush,
                                    p_Error_ಸಂಕೇತ);
            ಮೂಲ_ಸೂಚ್ಯಂಕ += 1 + static_cast<int32_t>(ಮೂಲ - reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಮೂಲ));

            ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                /* not mappable or buffer overflow */
                ನಿಲ್ಲಿಸು;
            }
        }
    }

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಮೂಲ);
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

/*
 * This version of ucnv_MBCSSingleToUnicodeWithOffsets() is optimized for single-byte, single-state codepages
 * that only map to and from the BMP.
 * In addition to single-byte optimizations, the offset calculations
 * become much easier.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ucnv_MBCSSingle_ಗಮ್ಯ_BMPWith_Offsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ, *ಕೊನೆಯ_ಮೂಲ;
    char16_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ, length;
    int32_t *offsets;

    ಸ್ಥಿರಾಂಕ int32_t (*state_ಕೋಷ್ಟಕ)[256];

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ;

    int32_t ನಮೂದು;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಮೂಲ);
    ಮೂಲ_ಎಲ್ಲೆ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ);
    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - p_ವಾದಗಳು->ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;

    ಆದರೆ((cnv->options&UCNV_OPTION_ಅದಲುಬದಲು_LFNL)!=0) {
        state_ಕೋಷ್ಟಕ=(ಸ್ಥಿರಾಂಕ int32_t (*)[256])cnv->shared_ದತ್ತಾಂಶ->mbcs.ಅದಲುಬದಲು_LFNLState_ಕೋಷ್ಟಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        state_ಕೋಷ್ಟಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.state_ಕೋಷ್ಟಕ;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ=0;
    ಕೊನೆಯ_ಮೂಲ=ಮೂಲ;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(ಮೂಲ_ಎಲ್ಲೆ - ಮೂಲ);
    ಆದರೆ(length<ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=length;
    }

#ಆದರೆ MBCS_UNROLL_SINGLE_ಗಮ್ಯ_BMP
    /* unrolling makes it faster on Pentium III/Windows 2000 */
    /* unroll the loop with the most common case */
unrolled:
    ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>=16) {
        int32_t count, loops, oredEntries;

        loops=count=ಗುರಿ_ಸಾಮರ್ಥ್ಯ>>4;
        ಮಾಡು {
            oredEntries=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            oredEntries|=ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));

            /* were all 16 entries really valid? */
            ಆದರೆ(!MBCS_ನಮೂದು_FINAL_IS_VALID_ನೇರ_16(oredEntries)) {
                /* no, return to the first of these 16 */
                ಮೂಲ-=16;
                ಗುರಿ-=16;
                ನಿಲ್ಲಿಸು;
            }
        } ಆಗ(--count>0);
        count=loops-count;
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=16*count;

        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
            ಕೊನೆಯ_ಮೂಲ+=16*count;
            ಆಗ(count>0) {
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                --count;
            }
        }
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* conversion loop */
    ಆಗ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ > 0 && ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
        ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* test the most common case first */
        ಆದರೆ(MBCS_ನಮೂದು_FINAL_IS_VALID_ನೇರ_16(ನಮೂದು)) {
            /* output BMP code point */
            *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
            ಮುಂದುವರಿ;
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು));
        ಆದರೆ(action==MBCS_STATE_FALLBACK_ನೇರ_16) {
            ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *ಗುರಿ++ = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                ಮುಂದುವರಿ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* reserved, must never occur */
            ಮುಂದುವರಿ;
        }

        /* set offsets since the start or the last extension */
        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
            int32_t count = static_cast<int32_t>(ಮೂಲ - ಕೊನೆಯ_ಮೂಲ);

            /* predecrement: do not set the offset for the callback-causing character */
            ಆಗ(--count>0) {
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
            }
            /* offset and sourceIndex are now set for the current character */
        }

        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            /* callback(illegal) */
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            ಕೊನೆಯ_ಮೂಲ=ಮೂಲ;
            cnv->ಗಮ್ಯ_UBytes[0]=*(ಮೂಲ-1);
            cnv->ಗಮ್ಯ_ULength=ext_ಗಮ್ಯ_U(cnv, cnv->shared_ದತ್ತಾಂಶ,
                                    1, &ಮೂಲ, ಮೂಲ_ಎಲ್ಲೆ,
                                    &ಗುರಿ, p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ,
                                    &offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
                                    p_ವಾದಗಳು->flush,
                                    p_Error_ಸಂಕೇತ);
            ಮೂಲ_ಸೂಚ್ಯಂಕ += 1 + static_cast<int32_t>(ಮೂಲ - ಕೊನೆಯ_ಮೂಲ);

            ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                /* not mappable or buffer overflow */
                ನಿಲ್ಲಿಸು;
            }

            /* recalculate the targetCapacity after an extension mapping */
            ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - ಗುರಿ);
            length = static_cast<int32_t>(ಮೂಲ_ಎಲ್ಲೆ - ಮೂಲ);
            ಆದರೆ(length<ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
                ಗುರಿ_ಸಾಮರ್ಥ್ಯ=length;
            }
        }

#ಆದರೆ MBCS_UNROLL_SINGLE_ಗಮ್ಯ_BMP
        /* unrolling makes it faster on Pentium III/Windows 2000 */
        ಹೋಗು unrolled;
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ) && ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ>=p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ) {
        /* target is full */
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    /* set offsets since the start or the last callback */
    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
        size_t count=ಮೂಲ-ಕೊನೆಯ_ಮೂಲ;
        ಆಗ(count>0) {
            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಮೂಲ);
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

ಸ್ಥಿರ UBool
has_Valid_Trail_ಬೈಟುಗಳು(ಸ್ಥಿರಾಂಕ int32_t (*state_ಕೋಷ್ಟಕ)[256], uint8_t state) {
    ಸ್ಥಿರಾಂಕ int32_t *ಅಡ್ಡಸಾಲು=state_ಕೋಷ್ಟಕ[state];
    int32_t b, ನಮೂದು;
    /* First test for final entries in this state for some commonly valid byte values. */
    ನಮೂದು=ಅಡ್ಡಸಾಲು[0xa1];
    ಆದರೆ( !MBCS_ನಮೂದು_IS_TRANSITION(ನಮೂದು) &&
        MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು)!=MBCS_STATE_ILLEGAL
    ) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ನಮೂದು=ಅಡ್ಡಸಾಲು[0x41];
    ಆದರೆ( !MBCS_ನಮೂದು_IS_TRANSITION(ನಮೂದು) &&
        MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು)!=MBCS_STATE_ILLEGAL
    ) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    /* Then test for final entries in this state. */
    ಗಾಗಿ(b=0; b<=0xff; ++b) {
        ನಮೂದು=ಅಡ್ಡಸಾಲು[b];
        ಆದರೆ( !MBCS_ನಮೂದು_IS_TRANSITION(ನಮೂದು) &&
            MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು)!=MBCS_STATE_ILLEGAL
        ) {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }
    /* Then recurse for transition entries. */
    ಗಾಗಿ(b=0; b<=0xff; ++b) {
        ನಮೂದು=ಅಡ್ಡಸಾಲು[b];
        ಆದರೆ( MBCS_ನಮೂದು_IS_TRANSITION(ನಮೂದು) &&
            has_Valid_Trail_ಬೈಟುಗಳು(state_ಕೋಷ್ಟಕ, static_cast<uint8_t>(MBCS_ನಮೂದು_TRANSITION_STATE(ನಮೂದು)))
        ) {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

/*
 * Is byte b a single/lead byte in this state?
 * Recurse for transition states, because here we don't want to say that
 * b is a lead byte if all byte sequences that start with b are illegal.
 */
ಸ್ಥಿರ UBool
isSingleOrLead(ಸ್ಥಿರಾಂಕ int32_t (*state_ಕೋಷ್ಟಕ)[256], uint8_t state, UBool isDBCSOnly, uint8_t b) {
    ಸ್ಥಿರಾಂಕ int32_t *ಅಡ್ಡಸಾಲು=state_ಕೋಷ್ಟಕ[state];
    int32_t ನಮೂದು=ಅಡ್ಡಸಾಲು[b];
    ಆದರೆ(MBCS_ನಮೂದು_IS_TRANSITION(ನಮೂದು)) {   /* lead byte */
        ಹಿಂತಿರುಗಿ has_Valid_Trail_ಬೈಟುಗಳು(state_ಕೋಷ್ಟಕ, static_cast<uint8_t>(MBCS_ನಮೂದು_TRANSITION_STATE(ನಮೂದು)));
    } ಇಲ್ಲದಿದ್ದರೆ {
        uint8_t action = static_cast<uint8_t>(MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು));
        ಆದರೆ(action==MBCS_STATE_CHANGE_ONLY && isDBCSOnly) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;   /* SI/SO are illegal for DBCS-only conversion */
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ action!=MBCS_STATE_ILLEGAL;
        }
    }
}

U_CFUNC ಶೂನ್ಯ
ucnv_MBCSToUnicodeWithOffsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    char16_t *ಗುರಿ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ;
    int32_t *offsets;

    ಸ್ಥಿರಾಂಕ int32_t (*state_ಕೋಷ್ಟಕ)[256];
    ಸ್ಥಿರಾಂಕ uint16_t *unicode_ಸಂಕೇತ_Units;

    uint32_t ಸ್ಥಾನಾಂತರ;
    uint8_t state;
    int8_t byte_ಸೂಚ್ಯಂಕ;
    uint8_t *ಬೈಟುಗಳು;

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ, ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;

    int32_t ನಮೂದು;
    char16_t c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_ವಾದಗಳು->converter;

    ಆದರೆ(cnv->pre_ಗಮ್ಯ_ULength>0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_ಹೊಂದಿಕೆ_ಗಮ್ಯ_U(cnv, p_ವಾದಗಳು, -1, p_Error_ಸಂಕೇತ);

        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ) || cnv->pre_ಗಮ್ಯ_ULength<0) {
            ಹಿಂತಿರುಗಿ;
        }
    }

    ಆದರೆ(cnv->shared_ದತ್ತಾಂಶ->mbcs.ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು==1) {
        ಆದರೆ(!(cnv->shared_ದತ್ತಾಂಶ->mbcs.unicode_ಮುಖವಾಡ&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingle_ಗಮ್ಯ_BMPWith_Offsets(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ucnv_MBCSSingle_ಗಮ್ಯ_Unicode_With_Offsets(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
        }
        ಹಿಂತಿರುಗಿ;
    }

    /* set up the local pointers */
    ಮೂಲ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ;
    offsets=p_ವಾದಗಳು->offsets;

    ಆದರೆ((cnv->options&UCNV_OPTION_ಅದಲುಬದಲು_LFNL)!=0) {
        state_ಕೋಷ್ಟಕ=(ಸ್ಥಿರಾಂಕ int32_t (*)[256])cnv->shared_ದತ್ತಾಂಶ->mbcs.ಅದಲುಬದಲು_LFNLState_ಕೋಷ್ಟಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        state_ಕೋಷ್ಟಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.state_ಕೋಷ್ಟಕ;
    }
    unicode_ಸಂಕೇತ_Units=cnv->shared_ದತ್ತಾಂಶ->mbcs.unicode_ಸಂಕೇತ_Units;

    /* get the converter state from UConverter */
    ಸ್ಥಾನಾಂತರ=cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;
    byte_ಸೂಚ್ಯಂಕ=cnv->ಗಮ್ಯ_ULength;
    ಬೈಟುಗಳು=cnv->ಗಮ್ಯ_UBytes;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    ಆದರೆ((state=(uint8_t)(cnv->ಬಹುಲಕ))==0) {
        state=cnv->shared_ದತ್ತಾಂಶ->mbcs.dbcsOnlyState;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ=byte_ಸೂಚ್ಯಂಕ==0 ? 0 : -1;
    ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    /* conversion loop */
    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one code unit that
         * overflows as a result of a surrogate pair or callback output
         * from the last source byte.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ಆದರೆ(ಗುರಿ>=ಗುರಿ_ಎಲ್ಲೆ) {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }

        ಆದರೆ(byte_ಸೂಚ್ಯಂಕ==0) {
            /* optimized loop for 1/2-byte input and BMP output */
            ಆದರೆ(offsets==ಶೂನ್ಯಸೂಚಕ) {
                ಮಾಡು {
                    ನಮೂದು=state_ಕೋಷ್ಟಕ[state][*ಮೂಲ];
                    ಆದರೆ(MBCS_ನಮೂದು_IS_TRANSITION(ನಮೂದು)) {
                        state=(uint8_t)MBCS_ನಮೂದು_TRANSITION_STATE(ನಮೂದು);
                        ಸ್ಥಾನಾಂತರ=MBCS_ನಮೂದು_TRANSITION_ಸ್ಥಾನಾಂತರ(ನಮೂದು);

                        ++ಮೂಲ;
                        ಆದರೆ( ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ &&
                            MBCS_ನಮೂದು_IS_FINAL(ನಮೂದು=state_ಕೋಷ್ಟಕ[state][*ಮೂಲ]) &&
                            MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು)==MBCS_STATE_VALID_16 &&
                            (c=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ+MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು)])<0xfffe
                        ) {
                            ++ಮೂಲ;
                            *ಗುರಿ++=c;
                            state=(uint8_t)MBCS_ನಮೂದು_FINAL_STATE(ನಮೂದು); /* typically 0 */
                            ಸ್ಥಾನಾಂತರ=0;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* set the state and leave the optimized loop */
                            ಬೈಟುಗಳು[0]=*(ಮೂಲ-1);
                            byte_ಸೂಚ್ಯಂಕ=1;
                            ನಿಲ್ಲಿಸು;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ(MBCS_ನಮೂದು_FINAL_IS_VALID_ನೇರ_16(ನಮೂದು)) {
                            /* output BMP code point */
                            ++ಮೂಲ;
                            *ಗುರಿ++=(char16_t)MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
                            state=(uint8_t)MBCS_ನಮೂದು_FINAL_STATE(ನಮೂದು); /* typically 0 */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* leave the optimized loop */
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                } ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ);
            } ಇಲ್ಲದಿದ್ದರೆ /* offsets!=nullptr */ {
                ಮಾಡು {
                    ನಮೂದು=state_ಕೋಷ್ಟಕ[state][*ಮೂಲ];
                    ಆದರೆ(MBCS_ನಮೂದು_IS_TRANSITION(ನಮೂದು)) {
                        state=(uint8_t)MBCS_ನಮೂದು_TRANSITION_STATE(ನಮೂದು);
                        ಸ್ಥಾನಾಂತರ=MBCS_ನಮೂದು_TRANSITION_ಸ್ಥಾನಾಂತರ(ನಮೂದು);

                        ++ಮೂಲ;
                        ಆದರೆ( ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ &&
                            MBCS_ನಮೂದು_IS_FINAL(ನಮೂದು=state_ಕೋಷ್ಟಕ[state][*ಮೂಲ]) &&
                            MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು)==MBCS_STATE_VALID_16 &&
                            (c=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ+MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು)])<0xfffe
                        ) {
                            ++ಮೂಲ;
                            *ಗುರಿ++=c;
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                ಮೂಲ_ಸೂಚ್ಯಂಕ=(ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ+=2);
                            }
                            state=(uint8_t)MBCS_ನಮೂದು_FINAL_STATE(ನಮೂದು); /* typically 0 */
                            ಸ್ಥಾನಾಂತರ=0;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* set the state and leave the optimized loop */
                            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            ಬೈಟುಗಳು[0]=*(ಮೂಲ-1);
                            byte_ಸೂಚ್ಯಂಕ=1;
                            ನಿಲ್ಲಿಸು;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ(MBCS_ನಮೂದು_FINAL_IS_VALID_ನೇರ_16(ನಮೂದು)) {
                            /* output BMP code point */
                            ++ಮೂಲ;
                            *ಗುರಿ++=(char16_t)MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                ಮೂಲ_ಸೂಚ್ಯಂಕ=++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            }
                            state=(uint8_t)MBCS_ನಮೂದು_FINAL_STATE(ನಮೂದು); /* typically 0 */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* leave the optimized loop */
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                } ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ);
            }

            /*
             * these tests and break statements could be put inside the loop
             * if C had "break outerLoop" like Java
             */
            ಆದರೆ(ಮೂಲ>=ಮೂಲ_ಎಲ್ಲೆ) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(ಗುರಿ>=ಗುರಿ_ಎಲ್ಲೆ) {
                /* target is full */
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }

            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಬೈಟುಗಳು[byte_ಸೂಚ್ಯಂಕ++]=*ಮೂಲ++;
        } ಇಲ್ಲದಿದ್ದರೆ /* byteIndex>0 */ {
            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ನಮೂದು=state_ಕೋಷ್ಟಕ[state][ಬೈಟುಗಳು[byte_ಸೂಚ್ಯಂಕ++]=*ಮೂಲ++];
        }

        ಆದರೆ(MBCS_ನಮೂದು_IS_TRANSITION(ನಮೂದು)) {
            state=(uint8_t)MBCS_ನಮೂದು_TRANSITION_STATE(ನಮೂದು);
            ಸ್ಥಾನಾಂತರ+=MBCS_ನಮೂದು_TRANSITION_ಸ್ಥಾನಾಂತರ(ನಮೂದು);
            ಮುಂದುವರಿ;
        }

        /* save the previous state for proper extension mapping with SI/SO-stateful converters */
        cnv->ಬಹುಲಕ=state;

        /* set the next state early so that we can reuse the entry variable */
        state=(uint8_t)MBCS_ನಮೂದು_FINAL_STATE(ನಮೂದು); /* typically 0 */

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action=(uint8_t)(MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು));
        ಆದರೆ(action==MBCS_STATE_VALID_16) {
            ಸ್ಥಾನಾಂತರ+=MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
            c=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ];
            ಆದರೆ(c<0xfffe) {
                /* output BMP code point */
                *ಗುರಿ++=c;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                byte_ಸೂಚ್ಯಂಕ=0;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0xfffe) {
                ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv) && (ನಮೂದು=(int32_t)ucnv_MBCSGetFallback(&cnv->shared_ದತ್ತಾಂಶ->mbcs, ಸ್ಥಾನಾಂತರ))!=0xfffe) {
                    /* output fallback BMP code point */
                    *ಗುರಿ++=(char16_t)ನಮೂದು;
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                    byte_ಸೂಚ್ಯಂಕ=0;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* callback(illegal) */
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_VALID_ನೇರ_16) {
            /* output BMP code point */
            *ಗುರಿ++=(char16_t)MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }
            byte_ಸೂಚ್ಯಂಕ=0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_VALID_16_PAIR) {
            ಸ್ಥಾನಾಂತರ+=MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
            c=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ++];
            ಆದರೆ(c<0xd800) {
                /* output BMP code point below 0xd800 */
                *ಗುರಿ++=c;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                byte_ಸೂಚ್ಯಂಕ=0;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                /* output roundtrip or fallback surrogate pair */
                *ಗುರಿ++=(char16_t)(c&0xdbff);
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                byte_ಸೂಚ್ಯಂಕ=0;
                ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                    *ಗುರಿ++=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ];
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* target overflow */
                    cnv->UChar_Error_ಬಫರ್[0]=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ];
                    cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                    *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;

                    ಸ್ಥಾನಾಂತರ=0;
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                *ಗುರಿ++=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ];
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                byte_ಸೂಚ್ಯಂಕ=0;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0xffff) {
                /* callback(illegal) */
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_VALID_ನೇರ_20 ||
                  (action==MBCS_STATE_FALLBACK_ನೇರ_20 && UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv))
        ) {
            ನಮೂದು=MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ(ನಮೂದು);
            /* output surrogate pair */
            *ಗುರಿ++=(char16_t)(0xd800|(char16_t)(ನಮೂದು>>10));
            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }
            byte_ಸೂಚ್ಯಂಕ=0;
            c=(char16_t)(0xdc00|(char16_t)(ನಮೂದು&0x3ff));
            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                *ಗುರಿ++=c;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* target overflow */
                cnv->UChar_Error_ಬಫರ್[0]=c;
                cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;

                ಸ್ಥಾನಾಂತರ=0;
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_CHANGE_ONLY) {
            /*
             * This serves as a state change without any output.
             * It is useful for reading simple stateful encodings,
             * for example using just Shift-In/Shift-Out codes.
             * The 21 unused bits may later be used for more sophisticated
             * state transitions.
             */
            ಆದರೆ(cnv->shared_ದತ್ತಾಂಶ->mbcs.dbcsOnlyState==0) {
                byte_ಸೂಚ್ಯಂಕ=0;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* SI/SO are illegal for DBCS-only conversion */
                state=(uint8_t)(cnv->ಬಹುಲಕ); /* restore the previous state */

                /* callback(illegal) */
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_FALLBACK_ನೇರ_16) {
            ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                *ಗುರಿ++=(char16_t)MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                byte_ಸೂಚ್ಯಂಕ=0;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* reserved, must never occur */
            byte_ಸೂಚ್ಯಂಕ=0;
        }

        /* end of action codes: prepare for a new character */
        ಸ್ಥಾನಾಂತರ=0;

        ಆದರೆ(byte_ಸೂಚ್ಯಂಕ==0) {
            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            /* callback(illegal) */
            ಆದರೆ(byte_ಸೂಚ್ಯಂಕ>1) {
                /*
                 * Ticket 5691: consistent illegal sequences:
                 * - We include at least the first byte in the illegal sequence.
                 * - If any of the non-initial bytes could be the start of a character,
                 *   we stop the illegal sequence before the first one of those.
                 */
                UBool isDBCSOnly = cnv->shared_ದತ್ತಾಂಶ->mbcs.dbcsOnlyState != 0;
                int8_t i;
                ಗಾಗಿ(i=1;
                    i<byte_ಸೂಚ್ಯಂಕ && !isSingleOrLead(state_ಕೋಷ್ಟಕ, state, isDBCSOnly, ಬೈಟುಗಳು[i]);
                    ++i) {}
                ಆದರೆ(i<byte_ಸೂಚ್ಯಂಕ) {
                    /* Back out some bytes. */
                    int8_t ಹಿಂಭಾಗ_ಹೊರಹೋಗು_ದೂರ=byte_ಸೂಚ್ಯಂಕ-i;
                    int32_t ಬೈಟುಗಳು_From_This_ಬಫರ್=(int32_t)(ಮೂಲ-(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ);
                    byte_ಸೂಚ್ಯಂಕ=i;  /* length of reported illegal byte sequence */
                    ಆದರೆ(ಹಿಂಭಾಗ_ಹೊರಹೋಗು_ದೂರ<=ಬೈಟುಗಳು_From_This_ಬಫರ್) {
                        ಮೂಲ-=ಹಿಂಭಾಗ_ಹೊರಹೋಗು_ದೂರ;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* Back out bytes from the previous buffer: Need to replay them. */
                        cnv->pre_ಗಮ್ಯ_ULength=(int8_t)(ಬೈಟುಗಳು_From_This_ಬಫರ್-ಹಿಂಭಾಗ_ಹೊರಹೋಗು_ದೂರ);
                        /* preToULength is negative! */
                        uprv_memcpy(cnv->pre_ಗಮ್ಯ_U, ಬೈಟುಗಳು+i, -cnv->pre_ಗಮ್ಯ_ULength);
                        ಮೂಲ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
                    }
                }
            }
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ /* unassigned sequences indicated with byteIndex>0 */ {
            /* try an extension mapping */
            p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
            byte_ಸೂಚ್ಯಂಕ=ext_ಗಮ್ಯ_U(cnv, cnv->shared_ದತ್ತಾಂಶ,
                              byte_ಸೂಚ್ಯಂಕ, &ಮೂಲ, ಮೂಲ_ಎಲ್ಲೆ,
                              &ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ,
                              &offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
                              p_ವಾದಗಳು->flush,
                              p_Error_ಸಂಕೇತ);
            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ+=(int32_t)(ಮೂಲ-(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ);

            ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                /* not mappable or buffer overflow */
                ನಿಲ್ಲಿಸು;
            }
        }
    }

    /* set the converter state back into UConverter */
    cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=ಸ್ಥಾನಾಂತರ;
    cnv->ಬಹುಲಕ=state;
    cnv->ಗಮ್ಯ_ULength=byte_ಸೂಚ್ಯಂಕ;

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

/*
 * This version of ucnv_MBCSGetNextUChar() is optimized for single-byte, single-state codepages.
 * We still need a conversion loop in case we find reserved action codes, which are to be ignored.
 */
ಸ್ಥಿರ UChar32
ucnv_MBCSSingle_Get_ಮುಂದಿನ_UChar(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                        UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ int32_t (*state_ಕೋಷ್ಟಕ)[256];
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;

    int32_t ನಮೂದು;
    uint8_t action;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಮೂಲ);
    ಮೂಲ_ಎಲ್ಲೆ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ);
    ಆದರೆ((cnv->options&UCNV_OPTION_ಅದಲುಬದಲು_LFNL)!=0) {
        state_ಕೋಷ್ಟಕ=(ಸ್ಥಿರಾಂಕ int32_t (*)[256])cnv->shared_ದತ್ತಾಂಶ->mbcs.ಅದಲುಬದಲು_LFNLState_ಕೋಷ್ಟಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        state_ಕೋಷ್ಟಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.state_ಕೋಷ್ಟಕ;
    }

    /* conversion loop */
    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        ನಮೂದು=state_ಕೋಷ್ಟಕ[0][*ಮೂಲ++];
        /* MBCS_ENTRY_IS_FINAL(entry) */

        /* write back the updated pointer early so that we can return directly */
        p_ವಾದಗಳು->ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಮೂಲ);

        ಆದರೆ(MBCS_ನಮೂದು_FINAL_IS_VALID_ನೇರ_16(ನಮೂದು)) {
            /* output BMP code point */
            ಹಿಂತಿರುಗಿ static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
        }

        /*
         * An if-else-if chain provides more reliable performance for
         * the most common cases compared to a switch.
         */
        action = static_cast<uint8_t>(MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು));
        ಆದರೆ( action==MBCS_STATE_VALID_ನೇರ_20 ||
            (action==MBCS_STATE_FALLBACK_ನೇರ_20 && UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv))
        ) {
            /* output supplementary code point */
            ಹಿಂತಿರುಗಿ static_cast<UChar32>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ(ನಮೂದು) + 0x10000);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_FALLBACK_ನೇರ_16) {
            ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv)) {
                /* output BMP code point */
                ಹಿಂತಿರುಗಿ static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_UNASSIGNED) {
            /* just fall through */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_ILLEGAL) {
            /* callback(illegal) */
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* reserved, must never occur */
            ಮುಂದುವರಿ;
        }

        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            /* callback(illegal) */
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ /* unassigned sequence */ {
            /* defer to the generic implementation */
            p_ವಾದಗಳು->ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಮೂಲ) - 1;
            ಹಿಂತಿರುಗಿ UCNV_GET_ಮುಂದಿನ_UCHAR_USE_ಗಮ್ಯ_U;
        }
    }

    /* no output because of empty input or only state changes */
    *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
    ಹಿಂತಿರುಗಿ 0xffff;
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
ಸ್ಥಿರ UChar32 U_CALLCONV
ucnv_MBCSGet_ಮುಂದಿನ_UChar(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ, *ಕೊನೆಯ_ಮೂಲ;

    ಸ್ಥಿರಾಂಕ int32_t (*state_ಕೋಷ್ಟಕ)[256];
    ಸ್ಥಿರಾಂಕ uint16_t *unicode_ಸಂಕೇತ_Units;

    uint32_t ಸ್ಥಾನಾಂತರ;
    uint8_t state;

    int32_t ನಮೂದು;
    UChar32 c;
    uint8_t action;

    /* use optimized function if possible */
    cnv=p_ವಾದಗಳು->converter;

    ಆದರೆ(cnv->pre_ಗಮ್ಯ_ULength>0) {
        /* use the generic code in ucnv_getNextUChar() to continue with a partial match */
        ಹಿಂತಿರುಗಿ UCNV_GET_ಮುಂದಿನ_UCHAR_USE_ಗಮ್ಯ_U;
    }

    ಆದರೆ(cnv->shared_ದತ್ತಾಂಶ->mbcs.unicode_ಮುಖವಾಡ&UCNV_HAS_SURROGATES) {
        /*
         * Using the generic ucnv_getNextUChar() code lets us deal correctly
         * with the rare case of a codepage that maps single surrogates
         * without adding the complexity to this already complicated function here.
         */
        ಹಿಂತಿರುಗಿ UCNV_GET_ಮುಂದಿನ_UCHAR_USE_ಗಮ್ಯ_U;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cnv->shared_ದತ್ತಾಂಶ->mbcs.ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು==1) {
        ಹಿಂತಿರುಗಿ ucnv_MBCSSingle_Get_ಮುಂದಿನ_UChar(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
    }

    /* set up the local pointers */
    ಮೂಲ = ಕೊನೆಯ_ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಮೂಲ);
    ಮೂಲ_ಎಲ್ಲೆ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ);

    ಆದರೆ((cnv->options&UCNV_OPTION_ಅದಲುಬದಲು_LFNL)!=0) {
        state_ಕೋಷ್ಟಕ=(ಸ್ಥಿರಾಂಕ int32_t (*)[256])cnv->shared_ದತ್ತಾಂಶ->mbcs.ಅದಲುಬದಲು_LFNLState_ಕೋಷ್ಟಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        state_ಕೋಷ್ಟಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.state_ಕೋಷ್ಟಕ;
    }
    unicode_ಸಂಕೇತ_Units=cnv->shared_ದತ್ತಾಂಶ->mbcs.unicode_ಸಂಕೇತ_Units;

    /* get the converter state from UConverter */
    ಸ್ಥಾನಾಂತರ=cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;

    /*
     * if we are in the SBCS state for a DBCS-only converter,
     * then load the DBCS state from the MBCS data
     * (dbcsOnlyState==0 if it is not a DBCS-only converter)
     */
    ಆದರೆ ((state = static_cast<uint8_t>(cnv->ಬಹುಲಕ)) == 0) {
        state=cnv->shared_ದತ್ತಾಂಶ->mbcs.dbcsOnlyState;
    }

    /* conversion loop */
    c=U_SENTINEL;
    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        ನಮೂದು=state_ಕೋಷ್ಟಕ[state][*ಮೂಲ++];
        ಆದರೆ(MBCS_ನಮೂದು_IS_TRANSITION(ನಮೂದು)) {
            state = static_cast<uint8_t>(MBCS_ನಮೂದು_TRANSITION_STATE(ನಮೂದು));
            ಸ್ಥಾನಾಂತರ+=MBCS_ನಮೂದು_TRANSITION_ಸ್ಥಾನಾಂತರ(ನಮೂದು);

            /* optimization for 1/2-byte input and BMP output */
            ಆದರೆ( ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ &&
                MBCS_ನಮೂದು_IS_FINAL(ನಮೂದು=state_ಕೋಷ್ಟಕ[state][*ಮೂಲ]) &&
                MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು)==MBCS_STATE_VALID_16 &&
                (c=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ+MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು)])<0xfffe
            ) {
                ++ಮೂಲ;
                state = static_cast<uint8_t>(MBCS_ನಮೂದು_FINAL_STATE(ನಮೂದು)); /* typically 0 */
                /* output BMP code point */
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* save the previous state for proper extension mapping with SI/SO-stateful converters */
            cnv->ಬಹುಲಕ=state;

            /* set the next state early so that we can reuse the entry variable */
            state = static_cast<uint8_t>(MBCS_ನಮೂದು_FINAL_STATE(ನಮೂದು)); /* typically 0 */

            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action = static_cast<uint8_t>(MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು));
            ಆದರೆ(action==MBCS_STATE_VALID_ನೇರ_16) {
                /* output BMP code point */
                c = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_VALID_16) {
                ಸ್ಥಾನಾಂತರ+=MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
                c=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ];
                ಆದರೆ(c<0xfffe) {
                    /* output BMP code point */
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0xfffe) {
                    ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv) && (c=ucnv_MBCSGetFallback(&cnv->shared_ದತ್ತಾಂಶ->mbcs, ಸ್ಥಾನಾಂತರ))!=0xfffe) {
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* callback(illegal) */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_VALID_16_PAIR) {
                ಸ್ಥಾನಾಂತರ+=MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
                c=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ++];
                ಆದರೆ(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=((c&0x3ff)<<10)+unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ]+(0x10000-0xdc00);
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ];
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0xffff) {
                    /* callback(illegal) */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_VALID_ನೇರ_20 ||
                      (action==MBCS_STATE_FALLBACK_ನೇರ_20 && UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv))
            ) {
                /* output supplementary code point */
                c = static_cast<UChar32>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ(ನಮೂದು) + 0x10000);
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_CHANGE_ONLY) {
                /*
                 * This serves as a state change without any output.
                 * It is useful for reading simple stateful encodings,
                 * for example using just Shift-In/Shift-Out codes.
                 * The 21 unused bits may later be used for more sophisticated
                 * state transitions.
                 */
                ಆದರೆ(cnv->shared_ದತ್ತಾಂಶ->mbcs.dbcsOnlyState!=0) {
                    /* SI/SO are illegal for DBCS-only conversion */
                    state = static_cast<uint8_t>(cnv->ಬಹುಲಕ); /* restore the previous state */

                    /* callback(illegal) */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_FALLBACK_ನೇರ_16) {
                ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv)) {
                    /* output BMP code point */
                    c = static_cast<char16_t>(MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು));
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_UNASSIGNED) {
                /* just fall through */
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_ILLEGAL) {
                /* callback(illegal) */
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* reserved (must never occur), or only state change */
                ಸ್ಥಾನಾಂತರ=0;
                ಕೊನೆಯ_ಮೂಲ=ಮೂಲ;
                ಮುಂದುವರಿ;
            }

            /* end of action codes: prepare for a new character */
            ಸ್ಥಾನಾಂತರ=0;

            ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                /* callback(illegal) */
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ /* unassigned sequence */ {
                /* defer to the generic implementation */
                cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=0;
                cnv->ಬಹುಲಕ=state;
                p_ವಾದಗಳು->ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಕೊನೆಯ_ಮೂಲ);
                ಹಿಂತಿರುಗಿ UCNV_GET_ಮುಂದಿನ_UCHAR_USE_ಗಮ್ಯ_U;
            }
        }
    }

    ಆದರೆ(c<0) {
        ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ) && ಮೂಲ==ಮೂಲ_ಎಲ್ಲೆ && ಕೊನೆಯ_ಮೂಲ<ಮೂಲ) {
            /* incomplete character byte sequence */
            cnv->ಗಮ್ಯ_ULength = static_cast<int8_t>(ಮೂಲ - ಕೊನೆಯ_ಮೂಲ);
            uprv_memcpy(cnv->ಗಮ್ಯ_UBytes, ಕೊನೆಯ_ಮೂಲ, cnv->ಗಮ್ಯ_ULength);
            *p_Error_ಸಂಕೇತ=U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            /* callback(illegal) */
            /*
             * Ticket 5691: consistent illegal sequences:
             * - We include at least the first byte in the illegal sequence.
             * - If any of the non-initial bytes could be the start of a character,
             *   we stop the illegal sequence before the first one of those.
             */
            UBool isDBCSOnly = static_cast<UBool>(cnv->shared_ದತ್ತಾಂಶ->mbcs.dbcsOnlyState != 0);
            uint8_t *ಬೈಟುಗಳು=cnv->ಗಮ್ಯ_UBytes;
            *ಬೈಟುಗಳು++=*ಕೊನೆಯ_ಮೂಲ++;     /* first byte */
            ಆದರೆ(ಕೊನೆಯ_ಮೂಲ==ಮೂಲ) {
                cnv->ಗಮ್ಯ_ULength=1;
            } ಇಲ್ಲದಿದ್ದರೆ /* lastSource<source: multi-byte character */ {
                int8_t i;
                ಗಾಗಿ(i=1;
                    ಕೊನೆಯ_ಮೂಲ<ಮೂಲ && !isSingleOrLead(state_ಕೋಷ್ಟಕ, state, isDBCSOnly, *ಕೊನೆಯ_ಮೂಲ);
                    ++i
                ) {
                    *ಬೈಟುಗಳು++=*ಕೊನೆಯ_ಮೂಲ++;
                }
                cnv->ಗಮ್ಯ_ULength=i;
                ಮೂಲ=ಕೊನೆಯ_ಮೂಲ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* no output because of empty input or only state changes */
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        }
        c=0xffff;
    }

    /* set the converter state back into UConverter, ready for a new character */
    cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=0;
    cnv->ಬಹುಲಕ=state;

    /* write back the updated pointer */
    p_ವಾದಗಳು->ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಮೂಲ);
    ಹಿಂತಿರುಗಿ c;
}

#ಆದರೆ 0
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
ucnv_MBCSSingle_Simple_Get_ಮುಂದಿನ_UChar(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ,
                              uint8_t b, UBool useFallback) {
    int32_t ನಮೂದು;
    uint8_t action;

    ನಮೂದು=shared_ದತ್ತಾಂಶ->mbcs.state_ಕೋಷ್ಟಕ[0][b];
    /* MBCS_ENTRY_IS_FINAL(entry) */

    ಆದರೆ(MBCS_ನಮೂದು_FINAL_IS_VALID_ನೇರ_16(ನಮೂದು)) {
        /* output BMP code point */
        ಹಿಂತಿರುಗಿ (char16_t)MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
    }

    /*
     * An if-else-if chain provides more reliable performance for
     * the most common cases compared to a switch.
     */
    action=(uint8_t)(MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು));
    ಆದರೆ(action==MBCS_STATE_VALID_ನೇರ_20) {
        /* output supplementary code point */
        ಹಿಂತಿರುಗಿ 0x10000+MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ(ನಮೂದು);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_FALLBACK_ನೇರ_16) {
        ಆದರೆ(!ಗಮ್ಯ_U_USE_FALLBACK(useFallback)) {
            ಹಿಂತಿರುಗಿ 0xfffe;
        }
        /* output BMP code point */
        ಹಿಂತಿರುಗಿ (char16_t)MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_FALLBACK_ನೇರ_20) {
        ಆದರೆ(!ಗಮ್ಯ_U_USE_FALLBACK(useFallback)) {
            ಹಿಂತಿರುಗಿ 0xfffe;
        }
        /* output supplementary code point */
        ಹಿಂತಿರುಗಿ 0x10000+MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ(ನಮೂದು);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_UNASSIGNED) {
        ಹಿಂತಿರುಗಿ 0xfffe;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_ILLEGAL) {
        ಹಿಂತಿರುಗಿ 0xffff;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* reserved, must never occur */
        ಹಿಂತಿರುಗಿ 0xffff;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

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
ucnv_MBCSSimple_Get_ಮುಂದಿನ_UChar(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ, int32_t length,
                        UBool useFallback) {
    ಸ್ಥಿರಾಂಕ int32_t (*state_ಕೋಷ್ಟಕ)[256];
    ಸ್ಥಿರಾಂಕ uint16_t *unicode_ಸಂಕೇತ_Units;

    uint32_t ಸ್ಥಾನಾಂತರ;
    uint8_t state, action;

    UChar32 c;
    int32_t i, ನಮೂದು;

    ಆದರೆ(length<=0) {
        /* no input at all: "illegal" */
        ಹಿಂತಿರುಗಿ 0xffff;
    }

#ಆದರೆ 0
/*
 * Code disabled 2002dec09 (ICU 2.4) because it is not currently used in ICU. markus
 * TODO In future releases, verify that this function is never called for SBCS
 * conversions, i.e., that sharedData->mbcs.countStates==1 is still true.
 * Removal improves code coverage.
 */
    /* use optimized function if possible */
    ಆದರೆ(shared_ದತ್ತಾಂಶ->mbcs.ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು==1) {
        ಆದರೆ(length==1) {
            ಹಿಂತಿರುಗಿ ucnv_MBCSSingle_Simple_Get_ಮುಂದಿನ_UChar(shared_ದತ್ತಾಂಶ, (uint8_t)*ಮೂಲ, useFallback);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ 0xffff; /* illegal: more than a single byte for an SBCS converter */
        }
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* set up the local pointers */
    state_ಕೋಷ್ಟಕ=shared_ದತ್ತಾಂಶ->mbcs.state_ಕೋಷ್ಟಕ;
    unicode_ಸಂಕೇತ_Units=shared_ದತ್ತಾಂಶ->mbcs.unicode_ಸಂಕೇತ_Units;

    /* converter state */
    ಸ್ಥಾನಾಂತರ=0;
    state=shared_ದತ್ತಾಂಶ->mbcs.dbcsOnlyState;

    /* conversion loop */
    ಗಾಗಿ(i=0;;) {
        ನಮೂದು=state_ಕೋಷ್ಟಕ[state][(uint8_t)ಮೂಲ[i++]];
        ಆದರೆ(MBCS_ನಮೂದು_IS_TRANSITION(ನಮೂದು)) {
            state=(uint8_t)MBCS_ನಮೂದು_TRANSITION_STATE(ನಮೂದು);
            ಸ್ಥಾನಾಂತರ+=MBCS_ನಮೂದು_TRANSITION_ಸ್ಥಾನಾಂತರ(ನಮೂದು);

            ಆದರೆ(i==length) {
                ಹಿಂತಿರುಗಿ 0xffff; /* truncated character */
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /*
             * An if-else-if chain provides more reliable performance for
             * the most common cases compared to a switch.
             */
            action=(uint8_t)(MBCS_ನಮೂದು_FINAL_ACTION(ನಮೂದು));
            ಆದರೆ(action==MBCS_STATE_VALID_16) {
                ಸ್ಥಾನಾಂತರ+=MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
                c=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ];
                ಆದರೆ(c!=0xfffe) {
                    /*ಮುಗಿದ*/
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv)) {
                    c=ucnv_MBCSGetFallback(&shared_ದತ್ತಾಂಶ->mbcs, ಸ್ಥಾನಾಂತರ);
                /* else done with 0xfffe */
                }
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_VALID_ನೇರ_16) {
                /* output BMP code point */
                c=(char16_t)MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_VALID_16_PAIR) {
                ಸ್ಥಾನಾಂತರ+=MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
                c=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ++];
                ಆದರೆ(c<0xd800) {
                    /* output BMP code point below 0xd800 */
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv) ? c<=0xdfff : c<=0xdbff) {
                    /* output roundtrip or fallback supplementary code point */
                    c=(UChar32)(((c&0x3ff)<<10)+unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ]+(0x10000-0xdc00));
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UCNV_ಗಮ್ಯ_U_USE_FALLBACK(cnv) ? (c&0xfffe)==0xe000 : c==0xe000) {
                    /* output roundtrip BMP code point above 0xd800 or fallback BMP code point */
                    c=unicode_ಸಂಕೇತ_Units[ಸ್ಥಾನಾಂತರ];
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0xffff) {
                    ಹಿಂತಿರುಗಿ 0xffff;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    c=0xfffe;
                }
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_VALID_ನೇರ_20) {
                /* output supplementary code point */
                c=0x10000+MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ(ನಮೂದು);
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_FALLBACK_ನೇರ_16) {
                ಆದರೆ(!ಗಮ್ಯ_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    ನಿಲ್ಲಿಸು;
                }
                /* output BMP code point */
                c=(char16_t)MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ_16(ನಮೂದು);
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_FALLBACK_ನೇರ_20) {
                ಆದರೆ(!ಗಮ್ಯ_U_USE_FALLBACK(useFallback)) {
                    c=0xfffe;
                    ನಿಲ್ಲಿಸು;
                }
                /* output supplementary code point */
                c=0x10000+MBCS_ನಮೂದು_FINAL_ಮೌಲ್ಯ(ನಮೂದು);
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(action==MBCS_STATE_UNASSIGNED) {
                c=0xfffe;
                ನಿಲ್ಲಿಸು;
            }

            /*
             * forbid MBCS_STATE_CHANGE_ONLY for this function,
             * and MBCS_STATE_ILLEGAL and reserved action codes
             */
            ಹಿಂತಿರುಗಿ 0xffff;
        }
    }

    ಆದರೆ(i!=length) {
        /* illegal for this function: not all input consumed */
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    ಆದರೆ(c==0xfffe) {
        /* try an extension mapping */
        ಸ್ಥಿರಾಂಕ int32_t *cx=shared_ದತ್ತಾಂಶ->mbcs.extIndexes;
        ಆದರೆ(cx!=ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ ucnv_ext_Simple_ಹೊಂದಿಕೆ_ಗಮ್ಯ_U(cx, ಮೂಲ, length, useFallback);
        }
    }

    ಹಿಂತಿರುಗಿ c;
}

/* MBCS-from-Unicode conversion functions ----------------------------------- */

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for double-byte codepages. */
ಸ್ಥಿರ ಶೂನ್ಯ
ucnv_MBCSDoubleFromUnicodeWithOffsets(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    int32_t *offsets;

    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ uint16_t *mbcs_ಸೂಚ್ಯಂಕ;
    ಸ್ಥಿರಾಂಕ uint8_t *ಬೈಟುಗಳು;

    UChar32 c;

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ, ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;

    uint32_t stage2_ನಮೂದು;
    uint32_t asciiRoundtrips;
    uint32_t value;
    uint8_t unicode_ಮುಖವಾಡ;

    /* use optimized function if possible */
    cnv=p_ವಾದಗಳು->converter;
    unicode_ಮುಖವಾಡ=cnv->shared_ದತ್ತಾಂಶ->mbcs.unicode_ಮುಖವಾಡ;

    /* set up the local pointers */
    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ = reinterpret_cast<uint8_t*>(p_ವಾದಗಳು->ಗುರಿ);
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - p_ವಾದಗಳು->ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;

    ಕೋಷ್ಟಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಕೋಷ್ಟಕ;
    mbcs_ಸೂಚ್ಯಂಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.mbcs_ಸೂಚ್ಯಂಕ;
    ಆದರೆ((cnv->options&UCNV_OPTION_ಅದಲುಬದಲು_LFNL)!=0) {
        ಬೈಟುಗಳು=cnv->shared_ದತ್ತಾಂಶ->mbcs.ಅದಲುಬದಲು_LFNLFrom_Unicode_ಬೈಟುಗಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬೈಟುಗಳು=cnv->shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು;
    }
    asciiRoundtrips=cnv->shared_ದತ್ತಾಂಶ->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ= c==0 ? 0 : -1;
    ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    /* conversion loop */
    ಆದರೆ(c!=0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
        ಹೋಗು getTrail;
    }

    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ಮೂಲ++;
            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಆದರೆ(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *ಗುರಿ++ = static_cast<uint8_t>(c);
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                c=0;
                ಮುಂದುವರಿ;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            ಆದರೆ(c<=0xd7ff) {
                value=DBCS_ಫಲಿತಾಂಶ_FROM_MOST_BMP(mbcs_ಸೂಚ್ಯಂಕ, (ಸ್ಥಿರಾಂಕ uint16_t *)ಬೈಟುಗಳು, c);
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                ಆದರೆ(value==0) {
                    ಹೋಗು unassigned;
                }
                /* output the value */
            } ಇಲ್ಲದಿದ್ದರೆ {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                ಆದರೆ(U16_IS_SURROGATE(c) && !(unicode_ಮುಖವಾಡ&UCNV_HAS_SURROGATES)) {
                    ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
                            /* test the following code unit */
                            char16_t trail=*ಮೂಲ;
                            ಆದರೆ(U16_IS_TRAIL(trail)) {
                                ++ಮೂಲ;
                                ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                ಆದರೆ(!(unicode_ಮುಖವಾಡ&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    /* callback(unassigned) */
                                    ಹೋಗು unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                                ನಿಲ್ಲಿಸು;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* no more input */
                            ನಿಲ್ಲಿಸು;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ನಿಲ್ಲಿಸು;
                    }
                }

                /* convert the Unicode code point in c into codepage bytes */
                stage2_ನಮೂದು=MBCS_STAGE_2_FROM_U(ಕೋಷ್ಟಕ, c);

                /* get the bytes and the length for the output */
                /* MBCS_OUTPUT_2 */
                value=MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(ಬೈಟುಗಳು, stage2_ನಮೂದು, c);

                /* is this code point assigned, or do we use fallbacks? */
                ಆದರೆ(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_ನಮೂದು, c) ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
                    c=_extFromU(cnv, cnv->shared_ದತ್ತಾಂಶ,
                                c, &ಮೂಲ, ಮೂಲ_ಎಲ್ಲೆ,
                                &ಗುರಿ, ಗುರಿ+ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                                &offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
                                p_ವಾದಗಳು->flush,
                                p_Error_ಸಂಕೇತ);
                    ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ += static_cast<int32_t>(ಮೂಲ - p_ವಾದಗಳು->ಮೂಲ);

                    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                        /* not mappable or buffer overflow */
                        ನಿಲ್ಲಿಸು;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ));

                        /* normal end of conversion: prepare for a new character */
                        ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        ಮುಂದುವರಿ;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            ಆದರೆ(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *ಗುರಿ++ = static_cast<uint8_t>(value);
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ /* length==2 */ {
                *ಗುರಿ++ = static_cast<uint8_t>(value >> 8);
                ಆದರೆ(2<=ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
                    *ಗುರಿ++ = static_cast<uint8_t>(value);
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                    ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=2;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                    cnv->char_Error_ಬಫರ್[0] = static_cast<ಅಕ್ಷರ>(value);
                    cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;

                    /* target overflow */
                    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=0;
                    *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    c=0;
                    ನಿಲ್ಲಿಸು;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
    p_ವಾದಗಳು->offsets=offsets;
}

/* This version of ucnv_MBCSFromUnicodeWithOffsets() is optimized for single-byte codepages. */
ಸ್ಥಿರ ಶೂನ್ಯ
ucnv_MBCSSingleFromUnicodeWithOffsets(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    int32_t *offsets;

    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ uint16_t *results;

    UChar32 c;

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ, ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;

    uint16_t value, min_ಮೌಲ್ಯ;
    UBool hasSupplementary;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ = reinterpret_cast<uint8_t*>(p_ವಾದಗಳು->ಗುರಿ);
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - p_ವಾದಗಳು->ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;

    ಕೋಷ್ಟಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಕೋಷ್ಟಕ;
    ಆದರೆ((cnv->options&UCNV_OPTION_ಅದಲುಬದಲು_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_ದತ್ತಾಂಶ->mbcs.ಅದಲುಬದಲು_LFNLFrom_Unicode_ಬೈಟುಗಳು);
    } ಇಲ್ಲದಿದ್ದರೆ {
        results=(uint16_t *)cnv->shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು;
    }

    ಆದರೆ(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_ಮೌಲ್ಯ=0x800;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* use only roundtrips and fallbacks from private-use characters */
        min_ಮೌಲ್ಯ=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_ದತ್ತಾಂಶ->mbcs.unicode_ಮುಖವಾಡ & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ= c==0 ? 0 : -1;
    ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    /* conversion loop */
    ಆದರೆ(c!=0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
        ಹೋಗು getTrail;
    }

    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ಮೂಲ++;
            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಆದರೆ(U16_IS_SURROGATE(c)) {
                ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                    ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
                        /* test the following code unit */
                        char16_t trail=*ಮೂಲ;
                        ಆದರೆ(U16_IS_TRAIL(trail)) {
                            ++ಮೂಲ;
                            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            ಆದರೆ(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                /* callback(unassigned) */
                                ಹೋಗು unassigned;
                            }
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            ನಿಲ್ಲಿಸು;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* no more input */
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    ನಿಲ್ಲಿಸು;
                }
            }

            /* convert the Unicode code point in c into codepage bytes */
            value=MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, results, c);

            /* is this code point assigned, or do we use fallbacks? */
            ಆದರೆ(value>=min_ಮೌಲ್ಯ) {
                /* assigned, write the output character bytes from value and length */
                /* length==1 */
                /* this is easy because we know that there is enough space */
                *ಗುರಿ++ = static_cast<uint8_t>(value);
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;

                /* normal end of conversion: prepare for a new character */
                c=0;
                ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            } ಇಲ್ಲದಿದ್ದರೆ { /* unassigned */
unassigned:
                /* try an extension mapping */
                p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
                c=_extFromU(cnv, cnv->shared_ದತ್ತಾಂಶ,
                            c, &ಮೂಲ, ಮೂಲ_ಎಲ್ಲೆ,
                            &ಗುರಿ, ಗುರಿ+ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                            &offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
                            p_ವಾದಗಳು->flush,
                            p_Error_ಸಂಕೇತ);
                ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ += static_cast<int32_t>(ಮೂಲ - p_ವಾದಗಳು->ಮೂಲ);

                ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                    /* not mappable or buffer overflow */
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ));

                    /* normal end of conversion: prepare for a new character */
                    ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
    p_ವಾದಗಳು->offsets=offsets;
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
ಸ್ಥಿರ ಶೂನ್ಯ
ucnv_MBCSSingleFromBMPWithOffsets(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ, *ಕೊನೆಯ_ಮೂಲ;
    uint8_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ, length;
    int32_t *offsets;

    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ uint16_t *results;

    UChar32 c;

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ;

    uint32_t asciiRoundtrips;
    uint16_t value, min_ಮೌಲ್ಯ;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ = reinterpret_cast<uint8_t*>(p_ವಾದಗಳು->ಗುರಿ);
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - p_ವಾದಗಳು->ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;

    ಕೋಷ್ಟಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಕೋಷ್ಟಕ;
    ಆದರೆ((cnv->options&UCNV_OPTION_ಅದಲುಬದಲು_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_ದತ್ತಾಂಶ->mbcs.ಅದಲುಬದಲು_LFNLFrom_Unicode_ಬೈಟುಗಳು);
    } ಇಲ್ಲದಿದ್ದರೆ {
        results=(uint16_t *)cnv->shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು;
    }
    asciiRoundtrips=cnv->shared_ದತ್ತಾಂಶ->mbcs.asciiRoundtrips;

    ಆದರೆ(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_ಮೌಲ್ಯ=0x800;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* use only roundtrips and fallbacks from private-use characters */
        min_ಮೌಲ್ಯ=0xc00;
    }

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ= c==0 ? 0 : -1;
    ಕೊನೆಯ_ಮೂಲ=ಮೂಲ;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length = static_cast<int32_t>(ಮೂಲ_ಎಲ್ಲೆ - ಮೂಲ);
    ಆದರೆ(length<ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=length;
    }

    /* conversion loop */
    ಆದರೆ(c!=0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
        ಹೋಗು getTrail;
    }

#ಆದರೆ MBCS_UNROLL_SINGLE_FROM_BMP
    /* unrolling makes it slower on Pentium III/Windows 2000?! */
    /* unroll the loop with the most common case */
unrolled:
    ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>=4) {
        int32_t count, loops;
        uint16_t anded_ಮೌಲ್ಯಗಳು;

        loops=count=ಗುರಿ_ಸಾಮರ್ಥ್ಯ>>2;
        ಮಾಡು {
            c=*ಮೂಲ++;
            anded_ಮೌಲ್ಯಗಳು=value=MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, results, c);
            *ಗುರಿ++=(uint8_t)value;
            c=*ಮೂಲ++;
            anded_ಮೌಲ್ಯಗಳು&=value=MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, results, c);
            *ಗುರಿ++=(uint8_t)value;
            c=*ಮೂಲ++;
            anded_ಮೌಲ್ಯಗಳು&=value=MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, results, c);
            *ಗುರಿ++=(uint8_t)value;
            c=*ಮೂಲ++;
            anded_ಮೌಲ್ಯಗಳು&=value=MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, results, c);
            *ಗುರಿ++=(uint8_t)value;

            /* were all 4 entries really valid? */
            ಆದರೆ(anded_ಮೌಲ್ಯಗಳು<min_ಮೌಲ್ಯ) {
                /* no, return to the first of these 4 */
                ಮೂಲ-=4;
                ಗುರಿ-=4;
                ನಿಲ್ಲಿಸು;
            }
        } ಆಗ(--count>0);
        count=loops-count;
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=4*count;

        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
            ಕೊನೆಯ_ಮೂಲ+=4*count;
            ಆಗ(count>0) {
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                --count;
            }
        }

        c=0;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆಗ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
        /*
         * Get a correct Unicode code point:
         * a single char16_t for a BMP code point or
         * a matched surrogate pair for a "supplementary code point".
         */
        c=*ಮೂಲ++;
        /*
         * Do not immediately check for single surrogates:
         * Assume that they are unassigned and check for them in that case.
         * This speeds up the conversion of assigned characters.
         */
        /* convert the Unicode code point in c into codepage bytes */
        ಆದರೆ(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
            *ಗುರಿ++ = static_cast<uint8_t>(c);
            --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
            c=0;
            ಮುಂದುವರಿ;
        }
        value=MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, results, c);
        /* is this code point assigned, or do we use fallbacks? */
        ಆದರೆ(value>=min_ಮೌಲ್ಯ) {
            /* assigned, write the output character bytes from value and length */
            /* length==1 */
            /* this is easy because we know that there is enough space */
            *ಗುರಿ++ = static_cast<uint8_t>(value);
            --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;

            /* normal end of conversion: prepare for a new character */
            c=0;
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!U16_IS_SURROGATE(c)) {
            /* normal, unassigned BMP character */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
            ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
                /* test the following code unit */
                char16_t trail=*ಮೂಲ;
                ಆದರೆ(U16_IS_TRAIL(trail)) {
                    ++ಮೂಲ;
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* no more input */
                ಆದರೆ (p_ವಾದಗಳು->flush) {
                    *p_Error_ಸಂಕೇತ=U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
                }
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
            ನಿಲ್ಲಿಸು;
        }

        /* c does not have a mapping */

        /* get the number of code units for c to correctly advance sourceIndex */
        length=U16_ಉದ್ದಳತೆ(c);

        /* set offsets since the start or the last extension */
        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
            int32_t count = static_cast<int32_t>(ಮೂಲ - ಕೊನೆಯ_ಮೂಲ);

            /* do not set the offset for this character */
            count-=length;

            ಆಗ(count>0) {
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                --count;
            }
            /* offsets and sourceIndex are now set for the current character */
        }

        /* try an extension mapping */
        ಕೊನೆಯ_ಮೂಲ=ಮೂಲ;
        c=_extFromU(cnv, cnv->shared_ದತ್ತಾಂಶ,
                    c, &ಮೂಲ, ಮೂಲ_ಎಲ್ಲೆ,
                    &ಗುರಿ, reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ),
                    &offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
                    p_ವಾದಗಳು->flush,
                    p_Error_ಸಂಕೇತ);
        ಮೂಲ_ಸೂಚ್ಯಂಕ += length + static_cast<int32_t>(ಮೂಲ - ಕೊನೆಯ_ಮೂಲ);
        ಕೊನೆಯ_ಮೂಲ=ಮೂಲ;

        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            /* not mappable or buffer overflow */
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* a mapping was written to the target, continue */

            /* recalculate the targetCapacity after an extension mapping */
            ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ));
            length = static_cast<int32_t>(ಮೂಲ_ಎಲ್ಲೆ - ಮೂಲ);
            ಆದರೆ(length<ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
                ಗುರಿ_ಸಾಮರ್ಥ್ಯ=length;
            }
        }

#ಆದರೆ MBCS_UNROLL_SINGLE_FROM_BMP
        /* unrolling makes it slower on Pentium III/Windows 2000?! */
        ಹೋಗು unrolled;
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ) && ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ>=(uint8_t *)p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ) {
        /* target is full */
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    /* set offsets since the start or the last callback */
    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
        size_t count=ಮೂಲ-ಕೊನೆಯ_ಮೂಲ;
        ಆದರೆ (count > 0 && *p_Error_ಸಂಕೇತ == U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು) {
            /*
            Caller gave us a partial supplementary character,
            which this function couldn't convert in any case.
            The callback will handle the offset.
            */
            count--;
        }
        ಆಗ(count>0) {
            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
            --count;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
    p_ವಾದಗಳು->offsets=offsets;
}

U_CFUNC ಶೂನ್ಯ
ucnv_MBCSFromUnicodeWithOffsets(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    int32_t *offsets;

    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ uint16_t *mbcs_ಸೂಚ್ಯಂಕ;
    ಸ್ಥಿರಾಂಕ uint8_t *p, *ಬೈಟುಗಳು;
    uint8_t outputType;

    UChar32 c;

    int32_t prev_ಮೂಲ_ಸೂಚ್ಯಂಕ, ಮೂಲ_ಸೂಚ್ಯಂಕ, ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;

    uint32_t stage2_ನಮೂದು;
    uint32_t asciiRoundtrips;
    uint32_t value;
    /* Shift-In and Shift-Out byte sequences differ by encoding scheme. */
    uint8_t si_ಬೈಟುಗಳು[2] = {0, 0};
    uint8_t so_ಬೈಟುಗಳು[2] = {0, 0};
    uint8_t si_ಉದ್ದಳತೆ, so_ಉದ್ದಳತೆ;
    int32_t length = 0, prev_ಉದ್ದಳತೆ;
    uint8_t unicode_ಮುಖವಾಡ;

    cnv=p_ವಾದಗಳು->converter;

    ಆದರೆ(cnv->preFromUFirstCP>=0) {
        /*
         * pass sourceIndex=-1 because we continue from an earlier buffer
         * in the future, this may change with continuous offsets
         */
        ucnv_ext_Continue_ಹೊಂದಿಕೆ_From_U(cnv, p_ವಾದಗಳು, -1, p_Error_ಸಂಕೇತ);

        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ) || cnv->preFromULength<0) {
            ಹಿಂತಿರುಗಿ;
        }
    }

    /* use optimized function if possible */
    outputType=cnv->shared_ದತ್ತಾಂಶ->mbcs.outputType;
    unicode_ಮುಖವಾಡ=cnv->shared_ದತ್ತಾಂಶ->mbcs.unicode_ಮುಖವಾಡ;
    ಆದರೆ(outputType==MBCS_OUTPUT_1 && !(unicode_ಮುಖವಾಡ&UCNV_HAS_SURROGATES)) {
        ಆದರೆ(!(unicode_ಮುಖವಾಡ&UCNV_HAS_SUPPLEMENTARY)) {
            ucnv_MBCSSingleFromBMPWithOffsets(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ucnv_MBCSSingleFromUnicodeWithOffsets(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
        }
        ಹಿಂತಿರುಗಿ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(outputType==MBCS_OUTPUT_2 && cnv->shared_ದತ್ತಾಂಶ->mbcs.utf8Friendly) {
        ucnv_MBCSDoubleFromUnicodeWithOffsets(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ;
    }

    /* set up the local pointers */
    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=(uint8_t *)p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ-p_ವಾದಗಳು->ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;

    ಕೋಷ್ಟಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಕೋಷ್ಟಕ;
    ಆದರೆ(cnv->shared_ದತ್ತಾಂಶ->mbcs.utf8Friendly) {
        mbcs_ಸೂಚ್ಯಂಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.mbcs_ಸೂಚ್ಯಂಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        mbcs_ಸೂಚ್ಯಂಕ=ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ((cnv->options&UCNV_OPTION_ಅದಲುಬದಲು_LFNL)!=0) {
        ಬೈಟುಗಳು=cnv->shared_ದತ್ತಾಂಶ->mbcs.ಅದಲುಬದಲು_LFNLFrom_Unicode_ಬೈಟುಗಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬೈಟುಗಳು=cnv->shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು;
    }
    asciiRoundtrips=cnv->shared_ದತ್ತಾಂಶ->mbcs.asciiRoundtrips;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;

    ಆದರೆ(outputType==MBCS_OUTPUT_2_SISO) {
        prev_ಉದ್ದಳತೆ=cnv->from_Unicode_ಸ್ಥಿತಿ;
        ಆದರೆ(prev_ಉದ್ದಳತೆ==0) {
            /* set the real value */
            prev_ಉದ್ದಳತೆ=1;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* prevent fromUnicodeStatus from being set to something non-0 */
        prev_ಉದ್ದಳತೆ=0;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    prev_ಮೂಲ_ಸೂಚ್ಯಂಕ=-1;
    ಮೂಲ_ಸೂಚ್ಯಂಕ= c==0 ? 0 : -1;
    ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    /* Get the SI/SO character for the converter */
    si_ಉದ್ದಳತೆ = static_cast<uint8_t>(getSISOBytes(SI, cnv->options, si_ಬೈಟುಗಳು));
    so_ಉದ್ದಳತೆ = static_cast<uint8_t>(getSISOBytes(SO, cnv->options, so_ಬೈಟುಗಳು));

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
    ಆದರೆ(c!=0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
        ಹೋಗು getTrail;
    }

    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        /*
         * This following test is to see if available input would overflow the output.
         * It does not catch output of more than one byte that
         * overflows as a result of a multi-byte character or callback output
         * from the last source character.
         * Therefore, those situations also test for overflows and will
         * then break the loop, too.
         */
        ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
            /*
             * Get a correct Unicode code point:
             * a single char16_t for a BMP code point or
             * a matched surrogate pair for a "supplementary code point".
             */
            c=*ಮೂಲ++;
            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಆದರೆ(c<=0x7f && IS_ASCII_ROUNDTRIP(c, asciiRoundtrips)) {
                *ಗುರಿ++=(uint8_t)c;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    prev_ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                c=0;
                ಮುಂದುವರಿ;
            }
            /*
             * utf8Friendly table: Test for <=0xd7ff rather than <=MBCS_FAST_MAX
             * to avoid dealing with surrogates.
             * MBCS_FAST_MAX must be >=0xd7ff.
             */
            ಆದರೆ(c<=0xd7ff && mbcs_ಸೂಚ್ಯಂಕ!=ಶೂನ್ಯಸೂಚಕ) {
                value=mbcs_ಸೂಚ್ಯಂಕ[c>>6];

                /* get the bytes and the length for the output (copied from below and adapted for utf8Friendly data) */
                /* There are only roundtrips (!=0) and no-mapping (==0) entries. */
                ಆಯ್ಕೆ(outputType) {
                ಸ್ಥಿತಿ MBCS_OUTPUT_2:
                    value=((ಸ್ಥಿರಾಂಕ uint16_t *)ಬೈಟುಗಳು)[value +(c&0x3f)];
                    ಆದರೆ(value<=0xff) {
                        ಆದರೆ(value==0) {
                            ಹೋಗು unassigned;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            length=1;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        length=2;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ MBCS_OUTPUT_2_SISO:
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
                    cnv->from_Unicode_ಸ್ಥಿತಿ=prev_ಉದ್ದಳತೆ; /* save the old state */
                    value=((ಸ್ಥಿರಾಂಕ uint16_t *)ಬೈಟುಗಳು)[value +(c&0x3f)];
                    ಆದರೆ(value<=0xff) {
                        ಆದರೆ(value==0) {
                            ಹೋಗು unassigned;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(prev_ಉದ್ದಳತೆ<=1) {
                            length=1;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* change from double-byte mode to single-byte */
                            ಆದರೆ (si_ಉದ್ದಳತೆ == 1) {
                                value|=(uint32_t)si_ಬೈಟುಗಳು[0]<<8;
                                length = 2;
                            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (si_ಉದ್ದಳತೆ == 2) {
                                value|=(uint32_t)si_ಬೈಟುಗಳು[1]<<8;
                                value|=(uint32_t)si_ಬೈಟುಗಳು[0]<<16;
                                length = 3;
                            }
                            prev_ಉದ್ದಳತೆ=1;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ(prev_ಉದ್ದಳತೆ==2) {
                            length=2;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* change from single-byte mode to double-byte */
                            ಆದರೆ (so_ಉದ್ದಳತೆ == 1) {
                                value|=(uint32_t)so_ಬೈಟುಗಳು[0]<<16;
                                length = 3;
                            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (so_ಉದ್ದಳತೆ == 2) {
                                value|=(uint32_t)so_ಬೈಟುಗಳು[1]<<16;
                                value|=(uint32_t)so_ಬೈಟುಗಳು[0]<<24;
                                length = 4;
                            }
                            prev_ಉದ್ದಳತೆ=2;
                        }
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=((ಸ್ಥಿರಾಂಕ uint16_t *)ಬೈಟುಗಳು)[value +(c&0x3f)];
                    ಆದರೆ(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        ಹೋಗು unassigned;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        length=2;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ MBCS_OUTPUT_3:
                    p=ಬೈಟುಗಳು+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    ಆದರೆ(value<=0xff) {
                        ಆದರೆ(value==0) {
                            ಹೋಗು unassigned;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            length=1;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0xffff) {
                        length=2;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        length=3;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ MBCS_OUTPUT_4:
                    value=((ಸ್ಥಿರಾಂಕ uint32_t *)ಬೈಟುಗಳು)[value +(c&0x3f)];
                    ಆದರೆ(value<=0xff) {
                        ಆದರೆ(value==0) {
                            ಹೋಗು unassigned;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            length=1;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0xffff) {
                        length=2;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0xffffff) {
                        length=3;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        length=4;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ MBCS_OUTPUT_3_EUC:
                    value=((ಸ್ಥಿರಾಂಕ uint16_t *)ಬೈಟುಗಳು)[value +(c&0x3f)];
                    /* EUC 16-bit fixed-length representation */
                    ಆದರೆ(value<=0xff) {
                        ಆದರೆ(value==0) {
                            ಹೋಗು unassigned;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            length=1;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        length=2;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ MBCS_OUTPUT_4_EUC:
                    p=ಬೈಟುಗಳು+(value+(c&0x3f))*3;
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    ಆದರೆ(value<=0xff) {
                        ಆದರೆ(value==0) {
                            ಹೋಗು unassigned;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            length=1;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0xffff) {
                        length=2;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        length=3;
                    }
                    ನಿಲ್ಲಿಸು;
                ಪೂರ್ವನಿಯೋಜಿತ:
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
                    ನಿಲ್ಲಿಸು;
                }
                /* output the value */
            } ಇಲ್ಲದಿದ್ದರೆ {
                /*
                 * This also tests if the codepage maps single surrogates.
                 * If it does, then surrogates are not paired but mapped separately.
                 * Note that in this case unmatched surrogates are not detected.
                 */
                ಆದರೆ(U16_IS_SURROGATE(c) && !(unicode_ಮುಖವಾಡ&UCNV_HAS_SURROGATES)) {
                    ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
getTrail:
                        ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
                            /* test the following code unit */
                            char16_t trail=*ಮೂಲ;
                            ಆದರೆ(U16_IS_TRAIL(trail)) {
                                ++ಮೂಲ;
                                ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                c=U16_GET_SUPPLEMENTARY(c, trail);
                                ಆದರೆ(!(unicode_ಮುಖವಾಡ&UCNV_HAS_SUPPLEMENTARY)) {
                                    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                    cnv->from_Unicode_ಸ್ಥಿತಿ=prev_ಉದ್ದಳತೆ; /* save the old state */
                                    /* callback(unassigned) */
                                    ಹೋಗು unassigned;
                                }
                                /* convert this supplementary code point */
                                /* exit this condition tree */
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                                ನಿಲ್ಲಿಸು;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* no more input */
                            ನಿಲ್ಲಿಸು;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ನಿಲ್ಲಿಸು;
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
                stage2_ನಮೂದು=MBCS_STAGE_2_FROM_U(ಕೋಷ್ಟಕ, c);

                /* get the bytes and the length for the output */
                ಆಯ್ಕೆ(outputType) {
                ಸ್ಥಿತಿ MBCS_OUTPUT_2:
                    value=MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
                    ಆದರೆ(value<=0xff) {
                        length=1;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        length=2;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ MBCS_OUTPUT_2_SISO:
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
                    cnv->from_Unicode_ಸ್ಥಿತಿ=prev_ಉದ್ದಳತೆ; /* save the old state */
                    value=MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
                    ಆದರೆ(value<=0xff) {
                        ಆದರೆ(value==0 && MBCS_FROM_U_IS_ROUNDTRIP(stage2_ನಮೂದು, c)==0) {
                            /* no mapping, leave value==0 */
                            length=0;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(prev_ಉದ್ದಳತೆ<=1) {
                            length=1;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* change from double-byte mode to single-byte */
                            ಆದರೆ (si_ಉದ್ದಳತೆ == 1) {
                                value|=(uint32_t)si_ಬೈಟುಗಳು[0]<<8;
                                length = 2;
                            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (si_ಉದ್ದಳತೆ == 2) {
                                value|=(uint32_t)si_ಬೈಟುಗಳು[1]<<8;
                                value|=(uint32_t)si_ಬೈಟುಗಳು[0]<<16;
                                length = 3;
                            }
                            prev_ಉದ್ದಳತೆ=1;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ(prev_ಉದ್ದಳತೆ==2) {
                            length=2;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* change from single-byte mode to double-byte */
                            ಆದರೆ (so_ಉದ್ದಳತೆ == 1) {
                                value|=(uint32_t)so_ಬೈಟುಗಳು[0]<<16;
                                length = 3;
                            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (so_ಉದ್ದಳತೆ == 2) {
                                value|=(uint32_t)so_ಬೈಟುಗಳು[1]<<16;
                                value|=(uint32_t)so_ಬೈಟುಗಳು[0]<<24;
                                length = 4;
                            }
                            prev_ಉದ್ದಳತೆ=2;
                        }
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ MBCS_OUTPUT_DBCS_ONLY:
                    /* table with single-byte results, but only DBCS mappings used */
                    value=MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
                    ಆದರೆ(value<=0xff) {
                        /* no mapping or SBCS result, not taken for DBCS-only */
                        value=stage2_ನಮೂದು=0; /* stage2Entry=0 to reset roundtrip flags */
                        length=0;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        length=2;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ MBCS_OUTPUT_3:
                    p=MBCS_ಸೂಚಕ_3_FROM_STAGE_2(ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    ಆದರೆ(value<=0xff) {
                        length=1;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0xffff) {
                        length=2;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        length=3;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ MBCS_OUTPUT_4:
                    value=MBCS_ಮೌಲ್ಯ_4_FROM_STAGE_2(ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
                    ಆದರೆ(value<=0xff) {
                        length=1;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0xffff) {
                        length=2;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0xffffff) {
                        length=3;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        length=4;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ MBCS_OUTPUT_3_EUC:
                    value=MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
                    /* EUC 16-bit fixed-length representation */
                    ಆದರೆ(value<=0xff) {
                        length=1;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((value&0x8000)==0) {
                        value|=0x8e8000;
                        length=3;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((value&0x80)==0) {
                        value|=0x8f0080;
                        length=3;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        length=2;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ MBCS_OUTPUT_4_EUC:
                    p=MBCS_ಸೂಚಕ_3_FROM_STAGE_2(ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
                    value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                    /* EUC 16-bit fixed-length representation applied to the first two bytes */
                    ಆದರೆ(value<=0xff) {
                        length=1;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0xffff) {
                        length=2;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((value&0x800000)==0) {
                        value|=0x8e800000;
                        length=4;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((value&0x8000)==0) {
                        value|=0x8f008000;
                        length=4;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        length=3;
                    }
                    ನಿಲ್ಲಿಸು;
                ಪೂರ್ವನಿಯೋಜಿತ:
                    /* must not occur */
                    /*
                     * To avoid compiler warnings that value & length may be
                     * used without having been initialized, we set them here.
                     * In reality, this is unreachable code.
                     * Not having a default branch also causes warnings with
                     * some compilers.
                     */
                    value=stage2_ನಮೂದು=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                    ನಿಲ್ಲಿಸು;
                }

                /* is this code point assigned, or do we use fallbacks? */
                ಆದರೆ(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_ನಮೂದು, c)!=0 ||
                     (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                ) {
                    /*
                     * We allow a 0 byte output if the "assigned" bit is set for this entry.
                     * There is no way with this data structure for fallback output
                     * to be a zero byte.
                     */

unassigned:
                    /* try an extension mapping */
                    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
                    c=_extFromU(cnv, cnv->shared_ದತ್ತಾಂಶ,
                                c, &ಮೂಲ, ಮೂಲ_ಎಲ್ಲೆ,
                                &ಗುರಿ, ಗುರಿ+ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                                &offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
                                p_ವಾದಗಳು->flush,
                                p_Error_ಸಂಕೇತ);
                    ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ+=(int32_t)(ಮೂಲ-p_ವಾದಗಳು->ಮೂಲ);
                    prev_ಉದ್ದಳತೆ=cnv->from_Unicode_ಸ್ಥಿತಿ; /* restore SISO state */

                    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                        /* not mappable or buffer overflow */
                        ನಿಲ್ಲಿಸು;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* a mapping was written to the target, continue */

                        /* recalculate the targetCapacity after an extension mapping */
                        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ-(ಅಕ್ಷರ *)ಗುರಿ);

                        /* normal end of conversion: prepare for a new character */
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            prev_ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        }
                        ಮುಂದುವರಿ;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            ಆದರೆ(length<=ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
                ಆದರೆ(offsets==ಶೂನ್ಯಸೂಚಕ) {
                    ಆಯ್ಕೆ(length) {
                        /* each branch falls through to the next one */
                    ಸ್ಥಿತಿ 4:
                        *ಗುರಿ++=(uint8_t)(value>>24);
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 3:
                        *ಗುರಿ++=(uint8_t)(value>>16);
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 2:
                        *ಗುರಿ++=(uint8_t)(value>>8);
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 1:
                        *ಗುರಿ++=(uint8_t)value;
                        U_FALLTHROUGH;
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        /* will never occur */
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆಯ್ಕೆ(length) {
                        /* each branch falls through to the next one */
                    ಸ್ಥಿತಿ 4:
                        *ಗುರಿ++=(uint8_t)(value>>24);
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 3:
                        *ಗುರಿ++=(uint8_t)(value>>16);
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 2:
                        *ಗುರಿ++=(uint8_t)(value>>8);
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 1:
                        *ಗುರಿ++=(uint8_t)value;
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        U_FALLTHROUGH;
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        /* will never occur */
                        ನಿಲ್ಲಿಸು;
                    }
                }
                ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=length;
            } ಇಲ್ಲದಿದ್ದರೆ {
                uint8_t *char_Error_ಬಫರ್;

                /*
                 * We actually do this backwards here:
                 * In order to save an intermediate variable, we output
                 * first to the overflow buffer what does not fit into the
                 * regular target.
                 */
                /* we know that 1<=targetCapacity<length<=4 */
                length-=ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                char_Error_ಬಫರ್=(uint8_t *)cnv->char_Error_ಬಫರ್;
                ಆಯ್ಕೆ(length) {
                    /* each branch falls through to the next one */
                ಸ್ಥಿತಿ 3:
                    *char_Error_ಬಫರ್++=(uint8_t)(value>>16);
                    U_FALLTHROUGH;
                ಸ್ಥಿತಿ 2:
                    *char_Error_ಬಫರ್++=(uint8_t)(value>>8);
                    U_FALLTHROUGH;
                ಸ್ಥಿತಿ 1:
                    *char_Error_ಬಫರ್=(uint8_t)value;
                    U_FALLTHROUGH;
                ಪೂರ್ವನಿಯೋಜಿತ:
                    /* will never occur */
                    ನಿಲ್ಲಿಸು;
                }
                cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=(int8_t)length;

                /* now output what fits into the regular target */
                value>>=8*length; /* length was reduced by targetCapacity */
                ಆಯ್ಕೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
                    /* each branch falls through to the next one */
                ಸ್ಥಿತಿ 3:
                    *ಗುರಿ++=(uint8_t)(value>>16);
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                    U_FALLTHROUGH;
                ಸ್ಥಿತಿ 2:
                    *ಗುರಿ++=(uint8_t)(value>>8);
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                    U_FALLTHROUGH;
                ಸ್ಥಿತಿ 1:
                    *ಗುರಿ++=(uint8_t)value;
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                    U_FALLTHROUGH;
                ಪೂರ್ವನಿಯೋಜಿತ:
                    /* will never occur */
                    ನಿಲ್ಲಿಸು;
                }

                /* target overflow */
                ಗುರಿ_ಸಾಮರ್ಥ್ಯ=0;
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                c=0;
                ನಿಲ್ಲಿಸು;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                prev_ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
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
    ಆದರೆ( U_SUCCESS(*p_Error_ಸಂಕೇತ) &&
        outputType==MBCS_OUTPUT_2_SISO && prev_ಉದ್ದಳತೆ==2 &&
        p_ವಾದಗಳು->flush && ಮೂಲ>=ಮೂಲ_ಎಲ್ಲೆ && c==0
    ) {
        /* EBCDIC_STATEFUL ending with DBCS: emit an SI to return the output stream to SBCS */
        ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
            *ಗುರಿ++ = si_ಬೈಟುಗಳು[0];
            ಆದರೆ (si_ಉದ್ದಳತೆ == 2) {
                ಆದರೆ (ಗುರಿ_ಸಾಮರ್ಥ್ಯ<2) {
                    cnv->char_Error_ಬಫರ್[0] = si_ಬೈಟುಗಳು[1];
                    cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                    *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    *ಗುರಿ++ = si_ಬೈಟುಗಳು[1];
                }
            }
            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                /* set the last source character's index (sourceIndex points at sourceLimit now) */
                *offsets++=prev_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* target is full */
            cnv->char_Error_ಬಫರ್[0] = si_ಬೈಟುಗಳು[0];
            ಆದರೆ (si_ಉದ್ದಳತೆ == 2) {
                cnv->char_Error_ಬಫರ್[1] = si_ಬೈಟುಗಳು[1];
            }
            cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=si_ಉದ್ದಳತೆ;
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        }
        prev_ಉದ್ದಳತೆ=1; /* we switched into SBCS */
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32=c;
    cnv->from_Unicode_ಸ್ಥಿತಿ=prev_ಉದ್ದಳತೆ;

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=(ಅಕ್ಷರ *)ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
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
ucnv_MBCSFromUChar32(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ,
                 UChar32 c, uint32_t *p_ಮೌಲ್ಯ,
                 UBool useFallback) {
    ಸ್ಥಿರಾಂಕ int32_t *cx;
    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ;
#ಆದರೆ 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
    ಸ್ಥಿರಾಂಕ uint8_t *p;
#ಆದರೆ_ಅಂತ್ಯ
    uint32_t stage2_ನಮೂದು;
    uint32_t value;
    int32_t length;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    ಆದರೆ(c<=0xffff || (shared_ದತ್ತಾಂಶ->mbcs.unicode_ಮುಖವಾಡ&UCNV_HAS_SUPPLEMENTARY)) {
        ಕೋಷ್ಟಕ=shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಕೋಷ್ಟಕ;

        /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
        ಆದರೆ(shared_ದತ್ತಾಂಶ->mbcs.outputType==MBCS_OUTPUT_1) {
            value=MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, (uint16_t *)shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು, c);
            /* is this code point assigned, or do we use fallbacks? */
            ಆದರೆ(useFallback ? value>=0x800 : value>=0xc00) {
                *p_ಮೌಲ್ಯ=value&0xff;
                ಹಿಂತಿರುಗಿ 1;
            }
        } ಇಲ್ಲದಿದ್ದರೆ /* outputType!=MBCS_OUTPUT_1 */ {
            stage2_ನಮೂದು=MBCS_STAGE_2_FROM_U(ಕೋಷ್ಟಕ, c);

            /* get the bytes and the length for the output */
            ಆಯ್ಕೆ(shared_ದತ್ತಾಂಶ->mbcs.outputType) {
            ಸ್ಥಿತಿ MBCS_OUTPUT_2:
                value=MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
                ಆದರೆ(value<=0xff) {
                    length=1;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    length=2;
                }
                ನಿಲ್ಲಿಸು;
#ಆದರೆ 0
/* #if 0 because this is not currently used in ICU - reduce code, increase code coverage */
            ಸ್ಥಿತಿ MBCS_OUTPUT_DBCS_ONLY:
                /* table with single-byte results, but only DBCS mappings used */
                value=MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
                ಆದರೆ(value<=0xff) {
                    /* no mapping or SBCS result, not taken for DBCS-only */
                    value=stage2_ನಮೂದು=0; /* stage2Entry=0 to reset roundtrip flags */
                    length=0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    length=2;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ MBCS_OUTPUT_3:
                p=MBCS_ಸೂಚಕ_3_FROM_STAGE_2(shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                ಆದರೆ(value<=0xff) {
                    length=1;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0xffff) {
                    length=2;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    length=3;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ MBCS_OUTPUT_4:
                value=MBCS_ಮೌಲ್ಯ_4_FROM_STAGE_2(shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
                ಆದರೆ(value<=0xff) {
                    length=1;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0xffff) {
                    length=2;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0xffffff) {
                    length=3;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    length=4;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ MBCS_OUTPUT_3_EUC:
                value=MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
                /* EUC 16-bit fixed-length representation */
                ಆದರೆ(value<=0xff) {
                    length=1;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((value&0x8000)==0) {
                    value|=0x8e8000;
                    length=3;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((value&0x80)==0) {
                    value|=0x8f0080;
                    length=3;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    length=2;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ MBCS_OUTPUT_4_EUC:
                p=MBCS_ಸೂಚಕ_3_FROM_STAGE_2(shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
                value=((uint32_t)*p<<16)|((uint32_t)p[1]<<8)|p[2];
                /* EUC 16-bit fixed-length representation applied to the first two bytes */
                ಆದರೆ(value<=0xff) {
                    length=1;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value<=0xffff) {
                    length=2;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((value&0x800000)==0) {
                    value|=0x8e800000;
                    length=4;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((value&0x8000)==0) {
                    value|=0x8f008000;
                    length=4;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    length=3;
                }
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
            ಪೂರ್ವನಿಯೋಜಿತ:
                /* must not occur */
                ಹಿಂತಿರುಗಿ -1;
            }

            /* is this code point assigned, or do we use fallbacks? */
            ಆದರೆ( MBCS_FROM_U_IS_ROUNDTRIP(stage2_ನಮೂದು, c) ||
                (FROM_U_USE_FALLBACK(useFallback, c) && value!=0)
            ) {
                /*
                 * We allow a 0 byte output if the "assigned" bit is set for this entry.
                 * There is no way with this data structure for fallback output
                 * to be a zero byte.
                 */
                /* assigned */
                *p_ಮೌಲ್ಯ=value;
                ಹಿಂತಿರುಗಿ length;
            }
        }
    }

    cx=shared_ದತ್ತಾಂಶ->mbcs.extIndexes;
    ಆದರೆ(cx!=ಶೂನ್ಯಸೂಚಕ) {
        length=ucnv_ext_Simple_ಹೊಂದಿಕೆ_From_U(cx, c, p_ಮೌಲ್ಯ, useFallback);
        ಹಿಂತಿರುಗಿ length>=0 ? length : -length;  /* return abs(length); */
    }

    /* unassigned */
    ಹಿಂತಿರುಗಿ 0;
}


#ಆದರೆ 0
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
ucnv_MBCSSingleFromUChar32(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ,
                       UChar32 c,
                       UBool useFallback) {
    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ;
    int32_t value;

    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    ಆದರೆ(c>=0x10000 && !(shared_ದತ್ತಾಂಶ->mbcs.unicode_ಮುಖವಾಡ&UCNV_HAS_SUPPLEMENTARY)) {
        ಹಿಂತಿರುಗಿ -1;
    }

    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    ಕೋಷ್ಟಕ=shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಕೋಷ್ಟಕ;

    /* get the byte for the output */
    value=MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, (uint16_t *)shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು, c);
    /* is this code point assigned, or do we use fallbacks? */
    ಆದರೆ(useFallback ? value>=0x800 : value>=0xc00) {
        ಹಿಂತಿರುಗಿ value&0xff;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ -1;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/* MBCS-from-UTF-8 conversion functions ------------------------------------- */

/* offsets for n-byte UTF-8 sequences that were calculated with ((lead<<6)+trail)<<6+trail... */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UChar32
utf8_offsets[5]={ 0, 0, 0x3080, 0xE2080, 0x3C82080 };

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_SBCSFromUTF8(UConverter_From_Unicode_ವಾದಗಳು *pFromUArgs,
                  UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ಗಮ್ಯ_UArgs,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *utf8, *cnv;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ;

    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ, *sbcs_ಸೂಚ್ಯಂಕ;
    ಸ್ಥಿರಾಂಕ uint16_t *results;

    int8_t old_ಗಮ್ಯ_ULength, ಗಮ್ಯ_ULength, ಗಮ್ಯ_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t asciiRoundtrips;
    uint16_t value, min_ಮೌಲ್ಯ = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_ಗಮ್ಯ_UArgs->converter;
    cnv=pFromUArgs->converter;
    ಮೂಲ=(uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=(uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ = reinterpret_cast<uint8_t*>(pFromUArgs->ಗುರಿ);
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(pFromUArgs->ಗುರಿ_ಎಲ್ಲೆ - pFromUArgs->ಗುರಿ);

    ಕೋಷ್ಟಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಕೋಷ್ಟಕ;
    sbcs_ಸೂಚ್ಯಂಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.sbcs_ಸೂಚ್ಯಂಕ;
    ಆದರೆ((cnv->options&UCNV_OPTION_ಅದಲುಬದಲು_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_ದತ್ತಾಂಶ->mbcs.ಅದಲುಬದಲು_LFNLFrom_Unicode_ಬೈಟುಗಳು);
    } ಇಲ್ಲದಿದ್ದರೆ {
        results=(uint16_t *)cnv->shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು;
    }
    asciiRoundtrips=cnv->shared_ದತ್ತಾಂಶ->mbcs.asciiRoundtrips;

    ಆದರೆ(cnv->useFallback) {
        /* use all roundtrip and fallback results */
        min_ಮೌಲ್ಯ=0x800;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* use only roundtrips and fallbacks from private-use characters */
        min_ಮೌಲ್ಯ=0xc00;
    }
    hasSupplementary = static_cast<UBool>(cnv->shared_ದತ್ತಾಂಶ->mbcs.unicode_ಮುಖವಾಡ & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    ಆದರೆ(utf8->ಗಮ್ಯ_ULength > 0) {
        ಗಮ್ಯ_ULength=old_ಗಮ್ಯ_ULength=utf8->ಗಮ್ಯ_ULength;
        ಗಮ್ಯ_ULimit = static_cast<int8_t>(utf8->ಬಹುಲಕ);
        c = static_cast<UChar32>(utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಮ್ಯ_ULength=old_ಗಮ್ಯ_ULength=ಗಮ್ಯ_ULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(ಮೂಲ_ಎಲ್ಲೆ - ಮೂಲ) - (ಗಮ್ಯ_ULimit - old_ಗಮ್ಯ_ULength);
        ಆದರೆ(length>0) {
            uint8_t b1=*(ಮೂಲ_ಎಲ್ಲೆ-1);
            ಆದರೆ(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(ಮೂಲ_ಎಲ್ಲೆ-2);
                ಆದರೆ(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    ಮೂಲ_ಎಲ್ಲೆ-=2;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --ಮೂಲ_ಎಲ್ಲೆ;
            }
        }
    }

    ಆದರೆ(c!=0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
        utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=0;
        utf8->ಗಮ್ಯ_ULength=0;
        ಹೋಗು ಇನ್ನಷ್ಟು_ಬೈಟುಗಳು;
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
    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
            b=*ಮೂಲ++;
            ಆದರೆ(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                ಆದರೆ(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *ಗುರಿ++ = b;
                    --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                    ಮುಂದುವರಿ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    c=b;
                    value=SBCS_ಫಲಿತಾಂಶ_FROM_UTF8(sbcs_ಸೂಚ್ಯಂಕ, results, 0, c);
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ(b<0xe0) {
                    ಆದರೆ( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*ಮೂಲ - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++ಮೂಲ;
                        value=SBCS_ಫಲಿತಾಂಶ_FROM_UTF8(sbcs_ಸೂಚ್ಯಂಕ, results, c, t1);
                        ಆದರೆ(value>=min_ಮೌಲ್ಯ) {
                            *ಗುರಿ++ = static_cast<uint8_t>(value);
                            --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                            ಮುಂದುವರಿ;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            c=(c<<6)|t1;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        c=-1;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b==0xe0) {
                    ಆದರೆ( /* handle U+0800..U+0FFF inline */
                        (t1 = static_cast<uint8_t>(ಮೂಲ[0] - 0x80)) <= 0x3f && t1 >= 0x20 &&
                        (t2 = static_cast<uint8_t>(ಮೂಲ[1] - 0x80)) <= 0x3f
                    ) {
                        c=t1;
                        ಮೂಲ+=2;
                        value=SBCS_ಫಲಿತಾಂಶ_FROM_UTF8(sbcs_ಸೂಚ್ಯಂಕ, results, c, t2);
                        ಆದರೆ(value>=min_ಮೌಲ್ಯ) {
                            *ಗುರಿ++ = static_cast<uint8_t>(value);
                            --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                            ಮುಂದುವರಿ;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            c=(c<<6)|t2;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        c=-1;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    c=-1;
                }

                ಆದರೆ(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    old_ಗಮ್ಯ_ULength=0;
                    ಗಮ್ಯ_ULength=1;
                    ಗಮ್ಯ_ULimit=U8_ಎಣಿಕೆ_ಬೈಟುಗಳು_NON_ASCII(b);
                    c=b;
ಇನ್ನಷ್ಟು_ಬೈಟುಗಳು:
                    ಆಗ(ಗಮ್ಯ_ULength<ಗಮ್ಯ_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        ಆದರೆ(ಮೂಲ<(uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ_ಎಲ್ಲೆ) {
                            b=*ಮೂಲ;
                            ಆದರೆ(icu::UTF8::isValidTrail(c, b, ಗಮ್ಯ_ULength, ಗಮ್ಯ_ULimit)) {
                                ++ಮೂಲ;
                                ++ಗಮ್ಯ_ULength;
                                c=(c<<6)+b;
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                ನಿಲ್ಲಿಸು; /* sequence too short, stop with toULength<toULimit */
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            ಮೂಲ-=(ಗಮ್ಯ_ULength-old_ಗಮ್ಯ_ULength);
                            ಆಗ(old_ಗಮ್ಯ_ULength<ಗಮ್ಯ_ULength) {
                                utf8->ಗಮ್ಯ_UBytes[old_ಗಮ್ಯ_ULength++]=*ಮೂಲ++;
                            }
                            utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=c;
                            utf8->ಗಮ್ಯ_ULength=ಗಮ್ಯ_ULength;
                            utf8->ಬಹುಲಕ=ಗಮ್ಯ_ULimit;
                            p_ಗಮ್ಯ_UArgs->ಮೂಲ=(ಅಕ್ಷರ *)ಮೂಲ;
                            pFromUArgs->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
                            ಹಿಂತಿರುಗಿ;
                        }
                    }

                    ಆದರೆ(ಗಮ್ಯ_ULength==ಗಮ್ಯ_ULimit) {
                        c-=utf8_offsets[ಗಮ್ಯ_ULength];
                        ಆದರೆ(ಗಮ್ಯ_ULength<=3) {  /* BMP */
                            value=MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, results, c);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* supplementary code point */
                            ಆದರೆ(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                value=0;
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                value=MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, results, c);
                            }
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* error handling: illegal UTF-8 byte sequence */
                        ಮೂಲ-=(ಗಮ್ಯ_ULength-old_ಗಮ್ಯ_ULength);
                        ಆಗ(old_ಗಮ್ಯ_ULength<ಗಮ್ಯ_ULength) {
                            utf8->ಗಮ್ಯ_UBytes[old_ಗಮ್ಯ_ULength++]=*ಮೂಲ++;
                        }
                        utf8->ಗಮ್ಯ_ULength=ಗಮ್ಯ_ULength;
                        p_ಗಮ್ಯ_UArgs->ಮೂಲ=(ಅಕ್ಷರ *)ಮೂಲ;
                        pFromUArgs->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
                        *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ಹಿಂತಿರುಗಿ;
                    }
                }
            }

            ಆದರೆ(value>=min_ಮೌಲ್ಯ) {
                /* output the mapping for c */
                *ಗುರಿ++ = static_cast<uint8_t>(value);
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* value<minValue means c is unassigned (unmappable) */
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t nul=0;
                ಸ್ಥಿರಾಂಕ char16_t *no_ಮೂಲ=&nul;
                c=_extFromU(cnv, cnv->shared_ದತ್ತಾಂಶ,
                            c, &no_ಮೂಲ, no_ಮೂಲ,
                            &ಗುರಿ, ಗುರಿ+ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                            ಶೂನ್ಯಸೂಚಕ, -1,
                            pFromUArgs->flush,
                            p_Error_ಸಂಕೇತ);

                ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *p_Error_ಸಂಕೇತ=U_USING_DEFAULT_WARNING;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(pFromUArgs->ಗುರಿ_ಎಲ್ಲೆ - reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ));
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ) &&
            cnv->preFromUFirstCP<0 &&
            ಮೂಲ<(ಮೂಲ_ಎಲ್ಲೆ=(uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ_ಎಲ್ಲೆ)) {
        c=utf8->ಗಮ್ಯ_UBytes[0]=b=*ಮೂಲ++;
        ಗಮ್ಯ_ULength=1;
        ಗಮ್ಯ_ULimit=U8_ಎಣಿಕೆ_ಬೈಟುಗಳು(b);
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            utf8->ಗಮ್ಯ_UBytes[ಗಮ್ಯ_ULength++]=b=*ಮೂಲ++;
            c=(c<<6)+b;
        }
        utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=c;
        utf8->ಗಮ್ಯ_ULength=ಗಮ್ಯ_ULength;
        utf8->ಬಹುಲಕ=ಗಮ್ಯ_ULimit;
    }

    /* write back the updated pointers */
    p_ಗಮ್ಯ_UArgs->ಮೂಲ=(ಅಕ್ಷರ *)ಮೂಲ;
    pFromUArgs->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_DBCSFromUTF8(UConverter_From_Unicode_ವಾದಗಳು *pFromUArgs,
                  UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ಗಮ್ಯ_UArgs,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *utf8, *cnv;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ;

    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ, *mbcs_ಸೂಚ್ಯಂಕ;
    ಸ್ಥಿರಾಂಕ uint16_t *results;

    int8_t old_ಗಮ್ಯ_ULength, ಗಮ್ಯ_ULength, ಗಮ್ಯ_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    uint32_t stage2_ನಮೂದು;
    uint32_t asciiRoundtrips;
    uint16_t value = 0;
    UBool hasSupplementary;

    /* set up the local pointers */
    utf8=p_ಗಮ್ಯ_UArgs->converter;
    cnv=pFromUArgs->converter;
    ಮೂಲ=(uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=(uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ = reinterpret_cast<uint8_t*>(pFromUArgs->ಗುರಿ);
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(pFromUArgs->ಗುರಿ_ಎಲ್ಲೆ - pFromUArgs->ಗುರಿ);

    ಕೋಷ್ಟಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಕೋಷ್ಟಕ;
    mbcs_ಸೂಚ್ಯಂಕ=cnv->shared_ದತ್ತಾಂಶ->mbcs.mbcs_ಸೂಚ್ಯಂಕ;
    ಆದರೆ((cnv->options&UCNV_OPTION_ಅದಲುಬದಲು_LFNL)!=0) {
        results = reinterpret_cast<uint16_t*>(cnv->shared_ದತ್ತಾಂಶ->mbcs.ಅದಲುಬದಲು_LFNLFrom_Unicode_ಬೈಟುಗಳು);
    } ಇಲ್ಲದಿದ್ದರೆ {
        results=(uint16_t *)cnv->shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು;
    }
    asciiRoundtrips=cnv->shared_ದತ್ತಾಂಶ->mbcs.asciiRoundtrips;

    hasSupplementary = static_cast<UBool>(cnv->shared_ದತ್ತಾಂಶ->mbcs.unicode_ಮುಖವಾಡ & UCNV_HAS_SUPPLEMENTARY);

    /* get the converter state from the UTF-8 UConverter */
    ಆದರೆ(utf8->ಗಮ್ಯ_ULength > 0) {
        ಗಮ್ಯ_ULength=old_ಗಮ್ಯ_ULength=utf8->ಗಮ್ಯ_ULength;
        ಗಮ್ಯ_ULimit = static_cast<int8_t>(utf8->ಬಹುಲಕ);
        c = static_cast<UChar32>(utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಮ್ಯ_ULength=old_ಗಮ್ಯ_ULength=ಗಮ್ಯ_ULimit=0;
        c = 0;
    }

    // The conversion loop checks source<sourceLimit only once per 1/2/3-byte character.
    // If the buffer ends with a truncated 2- or 3-byte sequence,
    // then we reduce the sourceLimit to before that,
    // and collect the remaining bytes after the conversion loop.
    {
        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length = static_cast<int32_t>(ಮೂಲ_ಎಲ್ಲೆ - ಮೂಲ) - (ಗಮ್ಯ_ULimit - old_ಗಮ್ಯ_ULength);
        ಆದರೆ(length>0) {
            uint8_t b1=*(ಮೂಲ_ಎಲ್ಲೆ-1);
            ಆದರೆ(U8_IS_SINGLE(b1)) {
                // common ASCII character
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U8_IS_TRAIL(b1) && length>=2) {
                uint8_t b2=*(ಮೂಲ_ಎಲ್ಲೆ-2);
                ಆದರೆ(0xe0<=b2 && b2<0xf0 && U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                    // truncated 3-byte sequence
                    ಮೂಲ_ಎಲ್ಲೆ-=2;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(0xc2<=b1 && b1<0xf0) {
                // truncated 2- or 3-byte sequence
                --ಮೂಲ_ಎಲ್ಲೆ;
            }
        }
    }

    ಆದರೆ(c!=0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
        utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=0;
        utf8->ಗಮ್ಯ_ULength=0;
        ಹೋಗು ಇನ್ನಷ್ಟು_ಬೈಟುಗಳು;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
            b=*ಮೂಲ++;
            ಆದರೆ(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                ಆದರೆ(IS_ASCII_ROUNDTRIP(b, asciiRoundtrips)) {
                    *ಗುರಿ++=b;
                    --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                    ಮುಂದುವರಿ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    value=DBCS_ಫಲಿತಾಂಶ_FROM_UTF8(mbcs_ಸೂಚ್ಯಂಕ, results, 0, b);
                    ಆದರೆ(value==0) {
                        c=b;
                        ಹೋಗು unassigned;
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ(b>=0xe0) {
                    ಆದರೆ( /* handle U+0800..U+D7FF inline */
                        b<=0xed &&  // do not assume maxFastUChar>0xd7ff
                        U8_IS_VALID_LEAD3_AND_T1(b, t1=ಮೂಲ[0]) &&
                        (t2 = static_cast<uint8_t>(ಮೂಲ[1] - 0x80)) <= 0x3f
                    ) {
                        c=((b&0xf)<<6)|(t1&0x3f);
                        ಮೂಲ+=2;
                        value=DBCS_ಫಲಿತಾಂಶ_FROM_UTF8(mbcs_ಸೂಚ್ಯಂಕ, results, c, t2);
                        ಆದರೆ(value==0) {
                            c=(c<<6)|t2;
                            ಹೋಗು unassigned;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        c=-1;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ( /* handle U+0080..U+07FF inline */
                        b>=0xc2 &&
                        (t1 = static_cast<uint8_t>(*ಮೂಲ - 0x80)) <= 0x3f
                    ) {
                        c=b&0x1f;
                        ++ಮೂಲ;
                        value=DBCS_ಫಲಿತಾಂಶ_FROM_UTF8(mbcs_ಸೂಚ್ಯಂಕ, results, c, t1);
                        ಆದರೆ(value==0) {
                            c=(c<<6)|t1;
                            ಹೋಗು unassigned;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        c=-1;
                    }
                }

                ಆದರೆ(c<0) {
                    /* handle "complicated" and error cases, and continuing partial characters */
                    old_ಗಮ್ಯ_ULength=0;
                    ಗಮ್ಯ_ULength=1;
                    ಗಮ್ಯ_ULimit=U8_ಎಣಿಕೆ_ಬೈಟುಗಳು_NON_ASCII(b);
                    c=b;
ಇನ್ನಷ್ಟು_ಬೈಟುಗಳು:
                    ಆಗ(ಗಮ್ಯ_ULength<ಗಮ್ಯ_ULimit) {
                        /*
                         * The sourceLimit may have been adjusted before the conversion loop
                         * to stop before a truncated sequence.
                         * Here we need to use the real limit in case we have two truncated
                         * sequences at the end.
                         * See ticket #7492.
                         */
                        ಆದರೆ(ಮೂಲ<(uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ_ಎಲ್ಲೆ) {
                            b=*ಮೂಲ;
                            ಆದರೆ(icu::UTF8::isValidTrail(c, b, ಗಮ್ಯ_ULength, ಗಮ್ಯ_ULimit)) {
                                ++ಮೂಲ;
                                ++ಗಮ್ಯ_ULength;
                                c=(c<<6)+b;
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                ನಿಲ್ಲಿಸು; /* sequence too short, stop with toULength<toULimit */
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                            ಮೂಲ-=(ಗಮ್ಯ_ULength-old_ಗಮ್ಯ_ULength);
                            ಆಗ(old_ಗಮ್ಯ_ULength<ಗಮ್ಯ_ULength) {
                                utf8->ಗಮ್ಯ_UBytes[old_ಗಮ್ಯ_ULength++]=*ಮೂಲ++;
                            }
                            utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=c;
                            utf8->ಗಮ್ಯ_ULength=ಗಮ್ಯ_ULength;
                            utf8->ಬಹುಲಕ=ಗಮ್ಯ_ULimit;
                            p_ಗಮ್ಯ_UArgs->ಮೂಲ=(ಅಕ್ಷರ *)ಮೂಲ;
                            pFromUArgs->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
                            ಹಿಂತಿರುಗಿ;
                        }
                    }

                    ಆದರೆ(ಗಮ್ಯ_ULength==ಗಮ್ಯ_ULimit) {
                        c-=utf8_offsets[ಗಮ್ಯ_ULength];
                        ಆದರೆ(ಗಮ್ಯ_ULength<=3) {  /* BMP */
                            stage2_ನಮೂದು=MBCS_STAGE_2_FROM_U(ಕೋಷ್ಟಕ, c);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* supplementary code point */
                            ಆದರೆ(!hasSupplementary) {
                                /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
                                stage2_ನಮೂದು=0;
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                stage2_ನಮೂದು=MBCS_STAGE_2_FROM_U(ಕೋಷ್ಟಕ, c);
                            }
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* error handling: illegal UTF-8 byte sequence */
                        ಮೂಲ-=(ಗಮ್ಯ_ULength-old_ಗಮ್ಯ_ULength);
                        ಆಗ(old_ಗಮ್ಯ_ULength<ಗಮ್ಯ_ULength) {
                            utf8->ಗಮ್ಯ_UBytes[old_ಗಮ್ಯ_ULength++]=*ಮೂಲ++;
                        }
                        utf8->ಗಮ್ಯ_ULength=ಗಮ್ಯ_ULength;
                        p_ಗಮ್ಯ_UArgs->ಮೂಲ=(ಅಕ್ಷರ *)ಮೂಲ;
                        pFromUArgs->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
                        *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ಹಿಂತಿರುಗಿ;
                    }

                    /* get the bytes and the length for the output */
                    /* MBCS_OUTPUT_2 */
                    value=MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(results, stage2_ನಮೂದು, c);

                    /* is this code point assigned, or do we use fallbacks? */
                    ಆದರೆ(!(MBCS_FROM_U_IS_ROUNDTRIP(stage2_ನಮೂದು, c) ||
                         (UCNV_FROM_U_USE_FALLBACK(cnv, c) && value!=0))
                    ) {
                        ಹೋಗು unassigned;
                    }
                }
            }

            /* write the output character bytes from value and length */
            /* from the first if in the loop we know that targetCapacity>0 */
            ಆದರೆ(value<=0xff) {
                /* this is easy because we know that there is enough space */
                *ಗುರಿ++ = static_cast<uint8_t>(value);
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ /* length==2 */ {
                *ಗುರಿ++ = static_cast<uint8_t>(value >> 8);
                ಆದರೆ(2<=ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
                    *ಗುರಿ++ = static_cast<uint8_t>(value);
                    ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=2;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    cnv->char_Error_ಬಫರ್[0] = static_cast<ಅಕ್ಷರ>(value);
                    cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;

                    /* target overflow */
                    *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಮುಂದುವರಿ;

unassigned:
            {
                /*
                 * Try an extension mapping.
                 * Pass in no source because we don't have UTF-16 input.
                 * If we have a partial match on c, we will return and revert
                 * to UTF-8->UTF-16->charset conversion.
                 */
                ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t nul=0;
                ಸ್ಥಿರಾಂಕ char16_t *no_ಮೂಲ=&nul;
                c=_extFromU(cnv, cnv->shared_ದತ್ತಾಂಶ,
                            c, &no_ಮೂಲ, no_ಮೂಲ,
                            &ಗುರಿ, ಗುರಿ+ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                            ಶೂನ್ಯಸೂಚಕ, -1,
                            pFromUArgs->flush,
                            p_Error_ಸಂಕೇತ);

                ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                    /* not mappable or buffer overflow */
                    cnv->fromUChar32=c;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cnv->preFromUFirstCP>=0) {
                    /*
                     * Partial match, return and revert to pivoting.
                     * In normal from-UTF-16 conversion, we would just continue
                     * but then exit the loop because the extension match would
                     * have consumed the source.
                     */
                    *p_Error_ಸಂಕೇತ=U_USING_DEFAULT_WARNING;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* a mapping was written to the target, continue */

                    /* recalculate the targetCapacity after an extension mapping */
                    ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(pFromUArgs->ಗುರಿ_ಎಲ್ಲೆ - reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ));
                    ಮುಂದುವರಿ;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     */
    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ) &&
            cnv->preFromUFirstCP<0 &&
            ಮೂಲ<(ಮೂಲ_ಎಲ್ಲೆ=(uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ_ಎಲ್ಲೆ)) {
        c=utf8->ಗಮ್ಯ_UBytes[0]=b=*ಮೂಲ++;
        ಗಮ್ಯ_ULength=1;
        ಗಮ್ಯ_ULimit=U8_ಎಣಿಕೆ_ಬೈಟುಗಳು(b);
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            utf8->ಗಮ್ಯ_UBytes[ಗಮ್ಯ_ULength++]=b=*ಮೂಲ++;
            c=(c<<6)+b;
        }
        utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=c;
        utf8->ಗಮ್ಯ_ULength=ಗಮ್ಯ_ULength;
        utf8->ಬಹುಲಕ=ಗಮ್ಯ_ULimit;
    }

    /* write back the updated pointers */
    p_ಗಮ್ಯ_UArgs->ಮೂಲ=(ಅಕ್ಷರ *)ಮೂಲ;
    pFromUArgs->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
}

/* miscellaneous ------------------------------------------------------------ */

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_MBCSGetStarters(ಸ್ಥಿರಾಂಕ UConverter* cnv,
                 UBool starters[256],
                 UError_ಸಂಕೇತ *) {
    ಸ್ಥಿರಾಂಕ int32_t *state0;
    ಪೂರ್ಣಾಂಕ i;

    state0=cnv->shared_ದತ್ತಾಂಶ->mbcs.state_ಕೋಷ್ಟಕ[cnv->shared_ದತ್ತಾಂಶ->mbcs.dbcsOnlyState];
    ಗಾಗಿ(i=0; i<256; ++i) {
        /* all bytes that cause a state transition from state 0 are lead bytes */
        starters[i] = static_cast<UBool>(MBCS_ನಮೂದು_IS_TRANSITION(state0[i]));
    }
}

/*
 * This is an internal function that allows other converter implementations
 * to check whether a byte is a lead byte.
 */
U_CFUNC UBool
ucnv_MBCSIsLeadByte(UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ, ಅಕ್ಷರ byte) {
    ಹಿಂತಿರುಗಿ MBCS_ನಮೂದು_IS_TRANSITION(shared_ದತ್ತಾಂಶ->mbcs.state_ಕೋಷ್ಟಕ[0][(uint8_t)byte]);
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_MBCSWriteSub(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
              int32_t ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv=p_ವಾದಗಳು->converter;
    ಅಕ್ಷರ *p, *subchar;
    ಅಕ್ಷರ ಬಫರ್[4];
    int32_t length;

    /* first, select between subChar and subChar1 */
    ಆದರೆ( cnv->subChar1!=0 &&
        (cnv->shared_ದತ್ತಾಂಶ->mbcs.extIndexes!=ಶೂನ್ಯಸೂಚಕ ?
            cnv->useSubChar1 :
            (cnv->invalid_UChar_ಬಫರ್[0]<=0xff))
    ) {
        /* select subChar1 if it is set (not 0) and the unmappable Unicode code point is up to U+00ff (IBM MBCS behavior) */
        subchar = reinterpret_cast<ಅಕ್ಷರ*>(&cnv->subChar1);
        length=1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* select subChar in all other cases */
        subchar = reinterpret_cast<ಅಕ್ಷರ*>(cnv->subChars);
        length=cnv->subCharLen;
    }

    /* reset the selector for the next code point */
    cnv->useSubChar1=ಸುಳ್ಳು;

    ಆದರೆ (cnv->shared_ದತ್ತಾಂಶ->mbcs.outputType == MBCS_OUTPUT_2_SISO) {
        p=ಬಫರ್;

        /* fromUnicodeStatus contains prevLength */
        ಆಯ್ಕೆ(length) {
        ಸ್ಥಿತಿ 1:
            ಆದರೆ(cnv->from_Unicode_ಸ್ಥಿತಿ==2) {
                /* DBCS mode and SBCS sub char: change to SBCS */
                cnv->from_Unicode_ಸ್ಥಿತಿ=1;
                *p++=UCNV_SI;
            }
            *p++=subchar[0];
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 2:
            ಆದರೆ(cnv->from_Unicode_ಸ್ಥಿತಿ<=1) {
                /* SBCS mode and DBCS sub char: change to DBCS */
                cnv->from_Unicode_ಸ್ಥಿತಿ=2;
                *p++=UCNV_SO;
            }
            *p++=subchar[0];
            *p++=subchar[1];
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        subchar=ಬಫರ್;
        length = static_cast<int32_t>(p - ಬಫರ್);
    }

    ucnv_cb_From_UWrite_ಬೈಟುಗಳು(p_ವಾದಗಳು, subchar, length, ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ, p_Error_ಸಂಕೇತ);
}

U_CFUNC UConverterType
ucnv_MBCSGetType(ಸ್ಥಿರಾಂಕ UConverter* converter) {
    /* SBCS, DBCS, and EBCDIC_STATEFUL are replaced by MBCS, but here we cheat a little */
    ಆದರೆ(converter->shared_ದತ್ತಾಂಶ->mbcs.ಎಣಿಕೆ_ಸ್ಥಿತಿಗಳು==1) {
        ಹಿಂತಿರುಗಿ (UConverterType)UCNV_SBCS;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((converter->shared_ದತ್ತಾಂಶ->mbcs.outputType&0xff)==MBCS_OUTPUT_2_SISO) {
        ಹಿಂತಿರುಗಿ (UConverterType)UCNV_EBCDIC_STATEFUL;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(converter->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->min_ಬೈಟುಗಳು_ಪ್ರತಿ_Char==2 && converter->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->max_ಬೈಟುಗಳು_ಪ್ರತಿ_Char==2) {
        ಹಿಂತಿರುಗಿ (UConverterType)UCNV_DBCS;
    }
    ಹಿಂತಿರುಗಿ (UConverterType)UCNV_MBCS;
}

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_LEGACY_CONVERSION */
