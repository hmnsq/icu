// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2003-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  ucnv_ext.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003jun13
*   created by: Markus W. Scherer
*
*   Conversion extensions
*/

#اگر_تعریف_نشده __UCNV_EXT_H__
#تعریف_کن __UCNV_EXT_H__

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "ucnv_cnv.h"

/*
 * See icuhtml/design/conversion/conversion_extensions.html
 *
 * Conversion extensions serve three purposes:
 * 1. They support m:n mappings.
 * 2. They support extension-only conversion files that are used together
 *    with the regular conversion data in base files.
 * 3. They support mappings with more complicated meta data,
 *    for example "good one-way" mappings (|4).
 *
 * A base file may contain an extension table (explicitly requested or
 * implicitly generated for m:n mappings), but its extension table is not
 * used when an extension-only file is used.
 *
 * It is an error if a base file contains any regular (not extension) mapping
 * from the same sequence as a mapping in the extension file
 * because the base mapping would hide the extension mapping.
 *
 *
 * Data for conversion extensions:
 *
 * One set of data structures per conversion direction (to/from Unicode).
 * The data structures are sorted by input units to allow for binary search.
 * Input sequences of more than one unit are handled like contraction tables
 * in collation:
 * The lookup value of a unit points to another table that is to be searched
 * for the next unit, recursively.
 *
 * For conversion from Unicode, the initial code point is looked up in
 * a 3-stage trie for speed,
 * with an additional table of unique results to save space.
 *
 * Long output strings are stored in separate arrays, with length and index
 * in the lookup tables.
 * Output results also include a flag distinguishing roundtrip from
 * (reverse) fallback mappings.
 *
 * Input Unicode strings must not begin or end with unpaired surrogates
 * to avoid problems with matches on parts of surrogate pairs.
 *
 * Mappings from multiple characters (code points or codepage state
 * table sequences) must be searched preferring the longest match.
 * For this to work and be efficient, the variable-width table must contain
 * all mappings that contain prefixes of the multiple characters.
 * If an extension table is built on top of a base table in another file
 * and a base table entry is a prefix of a multi-character mapping, then
 * this is an error.
 *
 *
 * Implementation note:
 *
 * Currently, the parser and several checks in the code limit the number
 * of UChars or bytes in a mapping to
 * UCNV_EXT_MAX_UCHARS and UCNV_EXT_MAX_BYTES, respectively,
 * which are output value limits in the data structure.
 *
 * For input, this is not strictly necessary - it is a hard limit only for the
 * buffers in UConverter that are used to store partial matches.
 *
 * Input sequences could otherwise be arbitrarily long if partial matches
 * need not be stored (i.e., if a sequence does not span several buffers with too
 * many units before the last buffer), although then results would differ
 * depending on whether partial matches exceed the limits or not,
 * which depends on the pattern of buffer sizes.
 *
 *
 * Data structure:
 *
 * int32_t indexes[>=32];
 *
 *   Array of indexes and lengths etc. The length of the array is at least 32.
 *   The actual length is stored in indexes[0] to be forward compatible.
 *
 *   Each index to another array is the number of bytes from indexes[].
 *   Each length of an array is the number of array base units in that array.
 *
 *   Some of the structures may not be present, in which case their indexes
 *   and lengths are 0.
 *
 *   Usage of indexes[i]:
 *   [0]  length of indexes[]
 *
 *   // to Unicode table
 *   [1]  index of toUTable[] (array of uint32_t)
 *   [2]  length of toUTable[]
 *   [3]  index of toUUChars[] (array of UChar)
 *   [4]  length of toUUChars[]
 *
 *   // from Unicode table, not for the initial code point
 *   [5]  index of fromUTableUChars[] (array of UChar)
 *   [6]  index of fromUTableValues[] (array of uint32_t)
 *   [7]  length of fromUTableUChars[] and fromUTableValues[]
 *   [8]  index of fromUBytes[] (array of char)
 *   [9]  length of fromUBytes[]
 *
 *   // from Unicode trie for initial-code point lookup
 *   [10] index of fromUStage12[] (combined array of uint16_t for stages 1 & 2)
 *   [11] length of stage 1 portion of fromUStage12[]
 *   [12] length of fromUStage12[]
 *   [13] index of fromUStage3[] (array of uint16_t indexes into fromUStage3b[])
 *   [14] length of fromUStage3[]
 *   [15] index of fromUStage3b[] (array of uint32_t like fromUTableValues[])
 *   [16] length of fromUStage3b[]
 *
 *   [17] Bit field containing numbers of bytes:
 *        31..24 reserved, 0
 *        23..16 maximum input bytes
 *        15.. 8 maximum output bytes
 *         7.. 0 maximum bytes per UChar
 *
 *   [18] Bit field containing numbers of UChars:
 *        31..24 reserved, 0
 *        23..16 maximum input UChars
 *        15.. 8 maximum output UChars
 *         7.. 0 maximum UChars per byte
 *
 *   [19] Bit field containing flags:
 *               (extension table unicodeMask)
 *         1     UCNV_HAS_SURROGATES flag for the extension table
 *         0     UCNV_HAS_SUPPLEMENTARY flag for the extension table
 *
 *   [20]..[30] reserved, 0
 *   [31] number of bytes for the entire extension structure
 *   [>31] reserved; there are indexes[0] indexes
 *
 *
 * uint32_t toUTable[];
 *
 *   Array of byte/value pairs for lookups for toUnicode conversion.
 *   The array is partitioned into sections like collation contraction tables.
 *   Each section contains one word with the number of following words and
 *   a default value for when the lookup in this section yields no match.
 *
 *   A section is sorted in ascending order of input bytes,
 *   allowing for fast linear or binary searches.
 *   The builder may store entries for a contiguous range of byte values
 *   (compare difference between the first and last one with count),
 *   which then allows for direct array access.
 *   The builder should always do this for the initial table section.
 *
 *   Entries may have 0 values, see below.
 *   No two entries in a section have the same byte values.
 *
 *   Each uint32_t contains an input byte value in bits 31..24 and the
 *   corresponding lookup value in bits 23..0.
 *   Interpret the value as follows:
 *     if(value==0) {
 *       no match, see below
 *     } else if(value<0x1f0000) {
 *       partial match - use value as index to the next toUTable section
 *       and match the next unit; (value indexes toUTable[value])
 *     } else {
 *       if(bit 23 set) {
 *         roundtrip;
 *       } else {
 *         fallback;
 *       }
 *       unset value bit 23;
 *       if(value<=0x2fffff) {
 *         (value-0x1f0000) is a code point; (BMP: value<=0x1fffff)
 *       } else {
 *         bits 17..0 (value&0x3ffff) is an index to
 *           the result UChars in toUUChars[]; (0 indexes toUUChars[0])
 *         length of the result=((value>>18)-12); (length=0..19)
 *       }
 *     }
 *
 *   The first word in a section contains the number of following words in the
 *   input byte position (bits 31..24, number=1..0xff).
 *   The value of the initial word is used when the current byte is not found
 *   in this section.
 *   If the value is not 0, then it represents a result as above.
 *   If the value is 0, then the search has to return a shorter match with an
 *   earlier default value as the result, or result in "unmappable" even for the
 *   initial bytes.
 *   If the value is 0 for the initial toUTable entry, then the initial byte
 *   does not start any mapping input.
 *
 *
 * UChar toUUChars[];
 *
 *   Contains toUnicode mapping results, stored as sequences of UChars.
 *   Indexes and lengths stored in the toUTable[].
 *
 *
 * UChar fromUTableUChars[];
 * uint32_t fromUTableValues[];
 *
 *   The fromUTable is split into two arrays, but works otherwise much like
 *   the toUTable. The array is partitioned into sections like collation
 *   contraction tables and toUTable.
 *   A row in the table consists of same-index entries in fromUTableUChars[]
 *   and fromUTableValues[].
 *
 *   Interpret a value as follows:
 *     if(value==0) {
 *       no match, see below
 *     } else if(value<=0xffffff) { (bits 31..24 are 0)
 *       partial match - use value as index to the next fromUTable section
 *       and match the next unit; (value indexes fromUTable[value])
 *     } else {
 *       if(value==0x80000001) {
 *         return no mapping, but request for <subchar1>;
 *       }
 *       if(bit 31 set) {
 *         roundtrip (|0);
 *       } else if(bit 30 set) {
 *         "good one-way" mapping (|4); -- new in ICU4C 51, _MBCSHeader.version 5.4/4.4
 *       } else {
 *         normal fallback (|1);
 *       }
 *       // bit 29 reserved, 0
 *       length=(value>>24)&0x1f; (bits 28..24)
 *       if(length==1..3) {
 *         bits 23..0 contain 1..3 bytes, padded with 00s on the left;
 *       } else {
 *         bits 23..0 (value&0xffffff) is an index to
 *           the result bytes in fromUBytes[]; (0 indexes fromUBytes[0])
 *       }
 *     }
 *       
 *   The first pair in a section contains the number of following pairs in the
 *   UChar position (16 bits, number=1..0xffff).
 *   The value of the initial pair is used when the current UChar is not found
 *   in this section.
 *   If the value is not 0, then it represents a result as above.
 *   If the value is 0, then the search has to return a shorter match with an
 *   earlier default value as the result, or result in "unmappable" even for the
 *   initial UChars.
 *
 *   If the from Unicode trie is present, then the from Unicode search tables
 *   are not used for initial code points.
 *   In this case, the first entries (index 0) in the tables are not used
 *   (reserved, set to 0) because a value of 0 is used in trie results
 *   to indicate no mapping.
 *
 *
 * uint16_t fromUStage12[];
 *
 *   Stages 1 & 2 of a trie that maps an initial code point.
 *   Indexes in stage 1 are all offset by the length of stage 1 so that the
 *   same array pointer can be used for both stages.
 *   If (c>>10)>=(length of stage 1) then c does not start any mapping.
 *   Same bit distribution as for regular conversion tries.
 *
 *
 * uint16_t fromUStage3[];
 * uint32_t fromUStage3b[];
 *
 *   Stage 3 of the trie. The first array simply contains indexes to the second,
 *   which contains words in the same format as fromUTableValues[].
 *   Use a stage 3 granularity of 4, which allows for 256k stage 3 entries,
 *   and 16-bit entries in stage 3 allow for 64k stage 3b entries.
 *   The stage 3 granularity means that the stage 2 entry needs to be left-shifted.
 *
 *   Two arrays are used because it is expected that more than half of the stage 3
 *   entries will be zero. The 16-bit index stage 3 array saves space even
 *   considering storing a total of 6 bytes per non-zero entry in both arrays
 *   together.
 *   Using a stage 3 granularity of >1 diminishes the compactability in that stage
 *   but provides a larger effective addressing space in stage 2.
 *   All but the final result stage use 16-bit entries to save space.
 *
 *   fromUStage3b[] contains a zero for "no mapping" at its index 0,
 *   and may contain UCNV_EXT_FROM_U_SUBCHAR1 at index 1 for "<subchar1> SUB mapping"
 *   (i.e., "no mapping" with preference for <subchar1> rather than <subchar>),
 *   and all other items are unique non-zero results.
 *
 *   The default value of a fromUTableValues[] section that is referenced
 *   _directly_ from a fromUStage3b[] item may also be UCNV_EXT_FROM_U_SUBCHAR1,
 *   but this value must not occur anywhere else in fromUTableValues[]
 *   because "no mapping" is always a property of a single code point,
 *   never of multiple.
 *
 *
 * char fromUBytes[];
 *
 *   Contains fromUnicode mapping results, stored as sequences of chars.
 *   Indexes and lengths stored in the fromUTableValues[].
 */
تعداد {
    UCNV_EXT_INDEXES_درازی,            /* 0 */

    UCNV_EXT_مقصد_U_شاخص,                /* 1 */
    UCNV_EXT_مقصد_U_درازی,
    UCNV_EXT_مقصد_U_UCHARS_شاخص,
    UCNV_EXT_مقصد_U_UCHARS_درازی,

    UCNV_EXT_FROM_U_UCHARS_شاخص,       /* 5 */
    UCNV_EXT_FROM_U_مقادیر_شاخص,
    UCNV_EXT_FROM_U_درازی,
    UCNV_EXT_FROM_U_بایتها_شاخص,
    UCNV_EXT_FROM_U_بایتها_درازی,

    UCNV_EXT_FROM_U_STAGE_12_شاخص,     /* 10 */
    UCNV_EXT_FROM_U_STAGE_1_درازی,
    UCNV_EXT_FROM_U_STAGE_12_درازی,
    UCNV_EXT_FROM_U_STAGE_3_شاخص,
    UCNV_EXT_FROM_U_STAGE_3_درازی,
    UCNV_EXT_FROM_U_STAGE_3_B_شاخص,
    UCNV_EXT_FROM_U_STAGE_3_B_درازی,

    UCNV_EXT_شمار_بایتها,               /* 17 */
    UCNV_EXT_شمار_UCHARS,
    UCNV_EXT_FLAGS,

    UCNV_EXT_RESERVED_شاخص,            /* 20, moves with additional indexes */

    UCNV_EXT_اندازه=31,
    UCNV_EXT_INDEXES_MIN_درازی=32
};

/* get the pointer to an extension array from indexes[index] */
#تعریف_کن UCNV_EXT_ARRAY(indexes, شاخص, قلم_Type) \
    ((ثابت قلم_Type *)((ثابت حرف *)(indexes)+(indexes)[شاخص]))

#تعریف_کن UCNV_GET_MAX_بایتها_در_هر_UCHAR(indexes) \
    ((indexes)[UCNV_EXT_شمار_بایتها]&0xff)

/* internal API ------------------------------------------------------------- */

U_CFUNC UBool
ucnv_ext_Initial_تطابق_مقصد_U(UConverter *cnv, ثابت int32_t *cx,
                        int32_t اولی_درازی,
                        ثابت حرف **src, ثابت حرف *src_حد,
                        UChar **هدف, ثابت UChar *هدف_حد,
                        int32_t **offsets, int32_t src_شاخص,
                        UBool flush,
                        UError_کد *p_Error_کد);

U_CFUNC UChar32
ucnv_ext_Simple_تطابق_مقصد_U(ثابت int32_t *cx,
                       ثابت حرف *منبع, int32_t درازی,
                       UBool useFallback);

U_CFUNC فراغ
ucnv_ext_Continue_تطابق_مقصد_U(UConverter *cnv,
                         UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها, int32_t src_شاخص,
                         UError_کد *p_Error_کد);


U_CFUNC UBool
ucnv_ext_Initial_تطابق_From_U(UConverter *cnv, ثابت int32_t *cx,
                          UChar32 cp,
                          ثابت UChar **src, ثابت UChar *src_حد,
                          حرف **هدف, ثابت حرف *هدف_حد,
                          int32_t **offsets, int32_t src_شاخص,
                          UBool flush,
                          UError_کد *p_Error_کد);

U_CFUNC int32_t
ucnv_ext_Simple_تطابق_From_U(ثابت int32_t *cx,
                         UChar32 cp, uint32_t *p_مقدار,
                         UBool useFallback);

U_CFUNC فراغ
ucnv_ext_Continue_تطابق_From_U(UConverter *cnv,
                           UConverter_From_Unicode_آرگومانها *p_آرگومانها, int32_t src_شاخص,
                           UError_کد *p_Error_کد);

/*
 * Add code points and strings to the set according to the extension mappings.
 * Limitation on the UConverterSetFilter:
 * The filters currently assume that they are used with 1:1 mappings.
 * They only apply to single input code points, and then they pass through
 * only mappings with single-charset-code results.
 * For example, the Shift-JIS filter only works for 2-byte results and tests
 * that those 2 bytes are in the JIS X 0208 range of Shift-JIS.
 */
U_CFUNC فراغ
ucnv_ext_Get_Unicode_مجموعه(ثابت UConverter_Shared_داده *shared_داده,
                      ثابت USetAdder *sa,
                      UConverter_Unicode_مجموعه which,
                      UConverter_مجموعه_Filter filter,
                      UError_کد *p_Error_کد);

/* toUnicode helpers -------------------------------------------------------- */

#تعریف_کن UCNV_EXT_مقصد_U_BYTE_لغزش 24
#تعریف_کن UCNV_EXT_مقصد_U_مقدار_ماسک 0xffffff
#تعریف_کن UCNV_EXT_مقصد_U_MIN_کد_نقطه 0x1f0000
#تعریف_کن UCNV_EXT_مقصد_U_MAX_کد_نقطه 0x2fffff
#تعریف_کن UCNV_EXT_مقصد_U_ROUNDTRIP_FLAG ((uint32_t)1<<23)
#تعریف_کن UCNV_EXT_مقصد_U_شاخص_ماسک 0x3ffff
#تعریف_کن UCNV_EXT_مقصد_U_درازی_لغزش 18
#تعریف_کن UCNV_EXT_مقصد_U_درازی_جابه_جایی 12

/* maximum number of indexed UChars */
#تعریف_کن UCNV_EXT_MAX_UCHARS 19

#تعریف_کن UCNV_EXT_مقصد_U_MAKE_کلمه(byte, مقدار_2) (((uint32_t)(byte)<<UCNV_EXT_مقصد_U_BYTE_لغزش)|(مقدار_2))

#تعریف_کن UCNV_EXT_مقصد_U_GET_BYTE(کلمه) ((کلمه)>>UCNV_EXT_مقصد_U_BYTE_لغزش)
#تعریف_کن UCNV_EXT_مقصد_U_GET_مقدار(کلمه) ((کلمه)&UCNV_EXT_مقصد_U_مقدار_ماسک)

#تعریف_کن UCNV_EXT_مقصد_U_IS_PARTIAL(مقدار_2) ((مقدار_2)<UCNV_EXT_مقصد_U_MIN_کد_نقطه)
#تعریف_کن UCNV_EXT_مقصد_U_GET_PARTIAL_شاخص(مقدار_2) (مقدار_2)

#تعریف_کن UCNV_EXT_مقصد_U_IS_ROUNDTRIP(مقدار_2) (((مقدار_2)&UCNV_EXT_مقصد_U_ROUNDTRIP_FLAG)!=0)
#تعریف_کن UCNV_EXT_مقصد_U_ماسک_ROUNDTRIP(مقدار_2) ((مقدار_2)&~UCNV_EXT_مقصد_U_ROUNDTRIP_FLAG)

/* use after masking off the roundtrip flag */
#تعریف_کن UCNV_EXT_مقصد_U_IS_کد_نقطه(مقدار_2) ((مقدار_2)<=UCNV_EXT_مقصد_U_MAX_کد_نقطه)
#تعریف_کن UCNV_EXT_مقصد_U_GET_کد_نقطه(مقدار_2) ((مقدار_2)-UCNV_EXT_مقصد_U_MIN_کد_نقطه)

#تعریف_کن UCNV_EXT_مقصد_U_GET_شاخص(مقدار_2) ((مقدار_2)&UCNV_EXT_مقصد_U_شاخص_ماسک)
#تعریف_کن UCNV_EXT_مقصد_U_GET_درازی(مقدار_2) (((مقدار_2)>>UCNV_EXT_مقصد_U_درازی_لغزش)-UCNV_EXT_مقصد_U_درازی_جابه_جایی)

/* fromUnicode helpers ------------------------------------------------------ */

/* most trie constants are shared with ucnvmbcs.h */

/* see similar utrie.h UTRIE_INDEX_SHIFT and UTRIE_DATA_GRANULARITY */
#تعریف_کن UCNV_EXT_STAGE_2_چپ_لغزش 2
#تعریف_کن UCNV_EXT_STAGE_3_GRANULARITY 4

/* trie access, returns the stage 3 value=index to stage 3b; s1Index=c>>10 */
#تعریف_کن UCNV_EXT_FROM_U(stage12, stage3, s1_شاخص, c) \
    (stage3)[ ((int32_t)(stage12)[ (stage12)[s1_شاخص] +(((c)>>4)&0x3f) ]<<UCNV_EXT_STAGE_2_چپ_لغزش) +((c)&0xf) ]

#تعریف_کن UCNV_EXT_FROM_U_درازی_لغزش 24
#تعریف_کن UCNV_EXT_FROM_U_ROUNDTRIP_FLAG ((uint32_t)1<<31)
#تعریف_کن UCNV_EXT_FROM_U_GOOD_ONE_WAY_FLAG 0x40000000
#تعریف_کن UCNV_EXT_FROM_U_وضعیت_ماسک 0xc0000000
#تعریف_کن UCNV_EXT_FROM_U_RESERVED_ماسک 0x20000000
#تعریف_کن UCNV_EXT_FROM_U_داده_ماسک 0xffffff

/* special value for "no mapping" to <subchar1> (impossible roundtrip to 0 bytes, value 01) */
#تعریف_کن UCNV_EXT_FROM_U_SUBCHAR1 0x80000001

/* at most 3 bytes in the lower part of the value */
#تعریف_کن UCNV_EXT_FROM_U_MAX_مستقیم_درازی 3

/* maximum number of indexed bytes */
#تعریف_کن UCNV_EXT_MAX_بایتها 0x1f

#تعریف_کن UCNV_EXT_FROM_U_IS_PARTIAL(مقدار_2) (((مقدار_2)>>UCNV_EXT_FROM_U_درازی_لغزش)==0)
#تعریف_کن UCNV_EXT_FROM_U_GET_PARTIAL_شاخص(مقدار_2) (مقدار_2)

#تعریف_کن UCNV_EXT_FROM_U_IS_ROUNDTRIP(مقدار_2) (((مقدار_2)&UCNV_EXT_FROM_U_ROUNDTRIP_FLAG)!=0)
#تعریف_کن UCNV_EXT_FROM_U_ماسک_ROUNDTRIP(مقدار_2) ((مقدار_2)&~UCNV_EXT_FROM_U_ROUNDTRIP_FLAG)

/* get length; masks away all other bits */
#تعریف_کن UCNV_EXT_FROM_U_GET_درازی(مقدار_2) (int32_t)(((مقدار_2)>>UCNV_EXT_FROM_U_درازی_لغزش)&UCNV_EXT_MAX_بایتها)

/* get bytes or bytes index */
#تعریف_کن UCNV_EXT_FROM_U_GET_داده(مقدار_2) ((مقدار_2)&UCNV_EXT_FROM_U_داده_ماسک)

#پایان_اگر

#پایان_اگر
