// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1999-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  utf8.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999sep13
*   created by: Markus W. Scherer
*/

/**
 * \file
 * \brief C API: 8-bit Unicode handling macros
 * 
 * This file defines macros to deal with 8-bit Unicode (UTF-8) code units (bytes) and strings.
 *
 * For more information see utf.h and the ICU User Guide Strings chapter
 * (https://unicode-org.github.io/icu/userguide/strings).
 *
 * <em>Usage:</em>
 * ICU coding guidelines for if() statements should be followed when using these macros.
 * Compound statements (curly braces {}) must be used  for if-else-while... 
 * bodies and all macro statements should be terminated with semicolon.
 */

#جيڪڏهن_وصف_نه_ٿيل __UTF8_H__
#وصف_ڪريو __UTF8_H__

#ضمن <stdbool.h>
#ضمن "unicode/umachine.h"
#جيڪڏهن_وصف_نه_ٿيل __UTF_H__
#   include "unicode/utf.h"
#جيڪڏهن_پورو

/* internal definitions ----------------------------------------------------- */

/**
 * Counts the trail bytes for a UTF-8 lead byte.
 * Returns 0 for 0..0xc1 as well as for 0xf5..0xff.
 * leadByte might be evaluated multiple times.
 *
 * This is internal since it is not meant to be called directly by external clients;
 * however it is called by public macros in this file and thus must remain stable.
 *
 * @param leadByte The first byte of a UTF-8 sequence. Must be 0..0xff.
 * @internal
 */
#وصف_ڪريو U8_ڳڻپ_TRAIL_بائيٽون(leadByte) \
    (U8_IS_LEAD(leadByte) ? \
        ((uint8_t)(leadByte)>=0xe0)+((uint8_t)(leadByte)>=0xf0)+1 : 0)

/**
 * Counts the trail bytes for a UTF-8 lead byte of a valid UTF-8 sequence.
 * Returns 0 for 0..0xc1. Undefined for 0xf5..0xff.
 * leadByte might be evaluated multiple times.
 *
 * This is internal since it is not meant to be called directly by external clients;
 * however it is called by public macros in this file and thus must remain stable.
 *
 * @param leadByte The first byte of a UTF-8 sequence. Must be 0..0xff.
 * @internal
 */
#وصف_ڪريو U8_ڳڻپ_TRAIL_بائيٽون_UNSAFE(leadByte) \
    (((uint8_t)(leadByte)>=0xc2)+((uint8_t)(leadByte)>=0xe0)+((uint8_t)(leadByte)>=0xf0))

/**
 * Mask a UTF-8 lead byte, leave only the lower bits that form part of the code point value.
 *
 * This is internal since it is not meant to be called directly by external clients;
 * however it is called by public macros in this file and thus must remain stable.
 * @internal
 */
#وصف_ڪريو U8_نقاب_LEAD_BYTE(leadByte, ڳڻپ_Trail_بائيٽون) ((leadByte)&=(1<<(6-(ڳڻپ_Trail_بائيٽون)))-1)

/**
 * Internal bit vector for 3-byte UTF-8 validity check, for use in U8_IS_VALID_LEAD3_AND_T1.
 * Each bit indicates whether one lead byte + first trail byte pair starts a valid sequence.
 * Lead byte E0..EF bits 3..0 are used as byte index,
 * first trail byte bits 7..5 are used as bit index into that byte.
 * @see U8_IS_VALID_LEAD3_AND_T1
 * @internal
 */
#وصف_ڪريو U8_LEAD3_T1_بٽ "\x20\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x10\x30\x30"

/**
 * Internal 3-byte UTF-8 validity check.
 * Non-zero if lead byte E0..EF and first trail byte 00..FF start a valid sequence.
 * @internal
 */
#وصف_ڪريو U8_IS_VALID_LEAD3_AND_T1(lead, t1) (U8_LEAD3_T1_بٽ[(lead)&0xf]&(1<<((uint8_t)(t1)>>5)))

/**
 * Internal bit vector for 4-byte UTF-8 validity check, for use in U8_IS_VALID_LEAD4_AND_T1.
 * Each bit indicates whether one lead byte + first trail byte pair starts a valid sequence.
 * First trail byte bits 7..4 are used as byte index,
 * lead byte F0..F4 bits 2..0 are used as bit index into that byte.
 * @see U8_IS_VALID_LEAD4_AND_T1
 * @internal
 */
#وصف_ڪريو U8_LEAD4_T1_بٽ "\x00\x00\x00\x00\x00\x00\x00\x00\x1E\x0F\x0F\x0F\x00\x00\x00\x00"

/**
 * Internal 4-byte UTF-8 validity check.
 * Non-zero if lead byte F0..F4 and first trail byte 00..FF start a valid sequence.
 * @internal
 */
#وصف_ڪريو U8_IS_VALID_LEAD4_AND_T1(lead, t1) (U8_LEAD4_T1_بٽ[(uint8_t)(t1)>>4]&(1<<((lead)&7)))

/**
 * Function for handling "next code point" with error-checking.
 *
 * This is internal since it is not meant to be called directly by external clients;
 * however it is called by public macros in this
 * file and thus must remain stable, and should not be hidden when other internal
 * functions are hidden (otherwise public macros would fail to compile).
 * @internal
 */
U_CAPI UChar32 U_EXPORT2
utf8_ايندڙ_Char_محفوظ_Body(ثابت uint8_t *s, int32_t *pi, int32_t ڊيگهائي, UChar32 c, int8_t strict);

/**
 * Function for handling "append code point" with error-checking.
 *
 * This is internal since it is not meant to be called directly by external clients;
 * however it is called by public macros in this
 * file and thus must remain stable, and should not be hidden when other internal
 * functions are hidden (otherwise public macros would fail to compile).
 * @internal
 */
U_CAPI int32_t U_EXPORT2
utf8_append_Char_محفوظ_Body(uint8_t *s, int32_t i, int32_t ڊيگهائي, UChar32 c, UBool *pIsError);

/**
 * Function for handling "previous code point" with error-checking.
 *
 * This is internal since it is not meant to be called directly by external clients;
 * however it is called by public macros in this
 * file and thus must remain stable, and should not be hidden when other internal
 * functions are hidden (otherwise public macros would fail to compile).
 * @internal
 */
U_CAPI UChar32 U_EXPORT2
utf8_prev_Char_محفوظ_Body(ثابت uint8_t *s, int32_t شروعات, int32_t *pi, UChar32 c, int8_t strict);

/**
 * Function for handling "skip backward one code point" with error-checking.
 *
 * This is internal since it is not meant to be called directly by external clients;
 * however it is called by public macros in this
 * file and thus must remain stable, and should not be hidden when other internal
 * functions are hidden (otherwise public macros would fail to compile).
 * @internal
 */
U_CAPI int32_t U_EXPORT2
utf8_back1_محفوظ_Body(ثابت uint8_t *s, int32_t شروعات, int32_t i);

/* single-code point definitions -------------------------------------------- */

/**
 * Does this code unit (byte) encode a code point by itself (US-ASCII 0..0x7f)?
 * @param c 8-bit code unit (byte)
 * @return true or false
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_IS_SINGLE(c) ((int8_t)(c)>=0)

/**
 * Is this code unit (byte) a UTF-8 lead byte? (0xC2..0xF4)
 * @param c 8-bit code unit (byte)
 * @return true or false
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_IS_LEAD(c) ((uint8_t)((c)-0xc2)<=0x32)
// 0x32=0xf4-0xc2

/**
 * Is this code unit (byte) a UTF-8 trail byte? (0x80..0xBF)
 * @param c 8-bit code unit (byte)
 * @return true or false
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_IS_TRAIL(c) ((int8_t)(c)<-0x40)

/**
 * How many code units (bytes) are used for the UTF-8 encoding
 * of this Unicode code point?
 * @param c 32-bit code point
 * @return 1..4, or 0 if c is a surrogate or not a Unicode code point
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_ڊيگهائي(c) \
    ((uint32_t)(c)<=0x7f ? 1 : \
        ((uint32_t)(c)<=0x7ff ? 2 : \
            ((uint32_t)(c)<=0xd7ff ? 3 : \
                ((uint32_t)(c)<=0xdfff || (uint32_t)(c)>0x10ffff ? 0 : \
                    ((uint32_t)(c)<=0xffff ? 3 : 4)\
                ) \
            ) \
        ) \
    )

/**
 * The maximum number of UTF-8 code units (bytes) per Unicode code point (U+0000..U+10ffff).
 * @return 4
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_MAX_ڊيگهائي 4

#جيڪڏهن_وصف_نه_ٿيل U_HIDE_DRAFT_API

/**
 * Returns the length of a well-formed UTF-8 byte sequence according to its lead byte.
 * Returns 1 for 0..0xc1 as well as for 0xf5..0xff.
 * leadByte might be evaluated multiple times.
 *
 * @param leadByte The first byte of a UTF-8 sequence. Must be 0..0xff.
 * @return 1..4
 * @draft ICU 78
 */
#وصف_ڪريو U8_ڊيگهائي_FROM_LEAD_BYTE(leadByte) (U8_ڳڻپ_TRAIL_بائيٽون(leadByte) + 1)

/**
 * Returns the length of a well-formed UTF-8 byte sequence according to its lead byte.
 * Returns 1 for 0..0xc1. Undefined for 0xf5..0xff.
 * leadByte might be evaluated multiple times.
 *
 * @param leadByte The first byte of a UTF-8 sequence. Must be 0..0xff.
 * @return 1..4
 * @draft ICU 78
 */
#وصف_ڪريو U8_ڊيگهائي_FROM_LEAD_BYTE_UNSAFE(leadByte) (U8_ڳڻپ_TRAIL_بائيٽون_UNSAFE(leadByte) + 1)

#جيڪڏهن_پورو  // U_HIDE_DRAFT_API

/**
 * Get a code point from a string at a random-access offset,
 * without changing the offset.
 * The offset may point to either the lead byte or one of the trail bytes
 * for a code point, in which case the macro will read all of the bytes
 * for the code point.
 * The result is undefined if the offset points to an illegal UTF-8
 * byte sequence.
 * Iteration through a string is more efficient with U8_NEXT_UNSAFE or U8_NEXT.
 *
 * @param s const uint8_t * string
 * @param i string offset
 * @param c output UChar32 variable
 * @see U8_GET
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_GET_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    int32_t u8_get_unsafe_اشاريو=(int32_t)(i); \
    U8_سيٽ_CP_شروعات_UNSAFE(s, u8_get_unsafe_اشاريو); \
    U8_ايندڙ_UNSAFE(s, u8_get_unsafe_اشاريو, c); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Get a code point from a string at a random-access offset,
 * without changing the offset.
 * The offset may point to either the lead byte or one of the trail bytes
 * for a code point, in which case the macro will read all of the bytes
 * for the code point.
 *
 * The length can be negative for a NUL-terminated string.
 *
 * If the offset points to an illegal UTF-8 byte sequence, then
 * c is set to a negative value.
 * Iteration through a string is more efficient with U8_NEXT_UNSAFE or U8_NEXT.
 *
 * @param s const uint8_t * string
 * @param start int32_t starting string offset
 * @param i int32_t string offset, must be start<=i<length
 * @param length int32_t string length
 * @param c output UChar32 variable, set to <0 in case of an error
 * @see U8_GET_UNSAFE
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_GET(s, شروعات, i, ڊيگهائي, c) UPRV_بلاڪ_MACRO_BEGIN { \
    int32_t u8_get_اشاريو=(i); \
    U8_سيٽ_CP_شروعات(s, شروعات, u8_get_اشاريو); \
    U8_ايندڙ(s, u8_get_اشاريو, ڊيگهائي, c); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Get a code point from a string at a random-access offset,
 * without changing the offset.
 * The offset may point to either the lead byte or one of the trail bytes
 * for a code point, in which case the macro will read all of the bytes
 * for the code point.
 *
 * The length can be negative for a NUL-terminated string.
 *
 * If the offset points to an illegal UTF-8 byte sequence, then
 * c is set to U+FFFD.
 * Iteration through a string is more efficient with U8_NEXT_UNSAFE or U8_NEXT_OR_FFFD.
 *
 * This macro does not distinguish between a real U+FFFD in the text
 * and U+FFFD returned for an ill-formed sequence.
 * Use U8_GET() if that distinction is important.
 *
 * @param s const uint8_t * string
 * @param start int32_t starting string offset
 * @param i int32_t string offset, must be start<=i<length
 * @param length int32_t string length
 * @param c output UChar32 variable, set to U+FFFD in case of an error
 * @see U8_GET
 * @stable ICU 51
 */
#وصف_ڪريو U8_GET_OR_FFFD(s, شروعات, i, ڊيگهائي, c) UPRV_بلاڪ_MACRO_BEGIN { \
    int32_t u8_get_اشاريو=(i); \
    U8_سيٽ_CP_شروعات(s, شروعات, u8_get_اشاريو); \
    U8_ايندڙ_OR_FFFD(s, u8_get_اشاريو, ڊيگهائي, c); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/* definitions with forward iteration --------------------------------------- */

/**
 * Get a code point from a string at a code point boundary offset,
 * and advance the offset to the next code point boundary.
 * (Post-incrementing forward iteration.)
 * "Unsafe" macro, assumes well-formed UTF-8.
 *
 * The offset may point to the lead byte of a multi-byte sequence,
 * in which case the macro will read the whole sequence.
 * The result is undefined if the offset points to a trail byte
 * or an illegal UTF-8 sequence.
 *
 * @param s const uint8_t * string
 * @param i string offset
 * @param c output UChar32 variable
 * @see U8_NEXT
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_ايندڙ_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(uint8_t)(s)[(i)++]; \
    إذا(!U8_IS_SINGLE(c)) { \
        إذا((c)<0xe0) { \
            (c)=(((c)&0x1f)<<6)|((s)[(i)++]&0x3f); \
        } وإلا إذا((c)<0xf0) { \
            /* no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (UChar) */ \
            (c)=(UChar)(((c)<<12)|(((s)[i]&0x3f)<<6)|((s)[(i)+1]&0x3f)); \
            (i)+=2; \
        } وإلا { \
            (c)=(((c)&7)<<18)|(((s)[i]&0x3f)<<12)|(((s)[(i)+1]&0x3f)<<6)|((s)[(i)+2]&0x3f); \
            (i)+=3; \
        } \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Get a code point from a string at a code point boundary offset,
 * and advance the offset to the next code point boundary.
 * (Post-incrementing forward iteration.)
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 *
 * The length can be negative for a NUL-terminated string.
 *
 * The offset may point to the lead byte of a multi-byte sequence,
 * in which case the macro will read the whole sequence.
 * If the offset points to a trail byte or an illegal UTF-8 sequence, then
 * c is set to a negative value.
 *
 * @param s const uint8_t * string
 * @param i int32_t string offset, must be i<length
 * @param length int32_t string length
 * @param c output UChar32 variable, set to <0 in case of an error
 * @see U8_NEXT_UNSAFE
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_ايندڙ(s, i, ڊيگهائي, c) U8_اندروني_ايندڙ_OR_SUB(s, i, ڊيگهائي, c, U_SENTINEL)

/**
 * Get a code point from a string at a code point boundary offset,
 * and advance the offset to the next code point boundary.
 * (Post-incrementing forward iteration.)
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 *
 * The length can be negative for a NUL-terminated string.
 *
 * The offset may point to the lead byte of a multi-byte sequence,
 * in which case the macro will read the whole sequence.
 * If the offset points to a trail byte or an illegal UTF-8 sequence, then
 * c is set to U+FFFD.
 *
 * This macro does not distinguish between a real U+FFFD in the text
 * and U+FFFD returned for an ill-formed sequence.
 * Use U8_NEXT() if that distinction is important.
 *
 * @param s const uint8_t * string
 * @param i int32_t string offset, must be i<length
 * @param length int32_t string length
 * @param c output UChar32 variable, set to U+FFFD in case of an error
 * @see U8_NEXT
 * @stable ICU 51
 */
#وصف_ڪريو U8_ايندڙ_OR_FFFD(s, i, ڊيگهائي, c) U8_اندروني_ايندڙ_OR_SUB(s, i, ڊيگهائي, c, 0xfffd)

/** @internal */
#وصف_ڪريو U8_اندروني_ايندڙ_OR_SUB(s, i, ڊيگهائي, c, sub) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(uint8_t)(s)[(i)++]; \
    إذا(!U8_IS_SINGLE(c)) { \
        uint8_t __t = 0; \
        إذا((i)!=(ڊيگهائي) && \
            /* fetch/validate/assemble all but last trail byte */ \
            ((c)>=0xe0 ? \
                ((c)<0xf0 ?  /* U+0800..U+FFFF except surrogates */ \
                    U8_LEAD3_T1_بٽ[(c)&=0xf]&(1<<((__t=(s)[i])>>5)) && \
                    (__t&=0x3f, 1) \
                :  /* U+10000..U+10FFFF */ \
                    ((c)-=0xf0)<=4 && \
                    U8_LEAD4_T1_بٽ[(__t=(s)[i])>>4]&(1<<(c)) && \
                    ((c)=((c)<<6)|(__t&0x3f), ++(i)!=(ڊيگهائي)) && \
                    (__t=(s)[i]-0x80)<=0x3f) && \
                /* valid second-to-last trail byte */ \
                ((c)=((c)<<6)|__t, ++(i)!=(ڊيگهائي)) \
            :  /* U+0080..U+07FF */ \
                (c)>=0xc2 && ((c)&=0x1f, 1)) && \
            /* last trail byte */ \
            (__t=(s)[i]-0x80)<=0x3f && \
            ((c)=((c)<<6)|__t, ++(i), 1)) { \
        } وإلا { \
            (c)=(sub);  /* ill-formed*/ \
        } \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Append a code point to a string, overwriting 1 to 4 bytes.
 * The offset points to the current end of the string contents
 * and is advanced (post-increment).
 * "Unsafe" macro, assumes a valid code point and sufficient space in the string.
 * Otherwise, the result is undefined.
 *
 * @param s const uint8_t * string buffer
 * @param i string offset
 * @param c code point to append
 * @see U8_APPEND
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_APPEND_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    uint32_t __uc=(c); \
    إذا(__uc<=0x7f) { \
        (s)[(i)++]=(uint8_t)__uc; \
    } وإلا { \
        إذا(__uc<=0x7ff) { \
            (s)[(i)++]=(uint8_t)((__uc>>6)|0xc0); \
        } وإلا { \
            إذا(__uc<=0xffff) { \
                (s)[(i)++]=(uint8_t)((__uc>>12)|0xe0); \
            } وإلا { \
                (s)[(i)++]=(uint8_t)((__uc>>18)|0xf0); \
                (s)[(i)++]=(uint8_t)(((__uc>>12)&0x3f)|0x80); \
            } \
            (s)[(i)++]=(uint8_t)(((__uc>>6)&0x3f)|0x80); \
        } \
        (s)[(i)++]=(uint8_t)((__uc&0x3f)|0x80); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Append a code point to a string, overwriting 1 to 4 bytes.
 * The offset points to the current end of the string contents
 * and is advanced (post-increment).
 * "Safe" macro, checks for a valid code point.
 * If a non-ASCII code point is written, checks for sufficient space in the string.
 * If the code point is not valid or trail bytes do not fit,
 * then isError is set to true.
 *
 * @param s const uint8_t * string buffer
 * @param i int32_t string offset, must be i<capacity
 * @param capacity int32_t size of the string buffer
 * @param c UChar32 code point to append
 * @param isError output UBool set to true if an error occurs, otherwise not modified
 * @see U8_APPEND_UNSAFE
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_APPEND(s, i, صلاحيت, c, isError) UPRV_بلاڪ_MACRO_BEGIN { \
    uint32_t __uc=(c); \
    إذا(__uc<=0x7f) { \
        (s)[(i)++]=(uint8_t)__uc; \
    } وإلا إذا(__uc<=0x7ff && (i)+1<(صلاحيت)) { \
        (s)[(i)++]=(uint8_t)((__uc>>6)|0xc0); \
        (s)[(i)++]=(uint8_t)((__uc&0x3f)|0x80); \
    } وإلا إذا((__uc<=0xd7ff || (0xe000<=__uc && __uc<=0xffff)) && (i)+2<(صلاحيت)) { \
        (s)[(i)++]=(uint8_t)((__uc>>12)|0xe0); \
        (s)[(i)++]=(uint8_t)(((__uc>>6)&0x3f)|0x80); \
        (s)[(i)++]=(uint8_t)((__uc&0x3f)|0x80); \
    } وإلا إذا(0xffff<__uc && __uc<=0x10ffff && (i)+3<(صلاحيت)) { \
        (s)[(i)++]=(uint8_t)((__uc>>18)|0xf0); \
        (s)[(i)++]=(uint8_t)(((__uc>>12)&0x3f)|0x80); \
        (s)[(i)++]=(uint8_t)(((__uc>>6)&0x3f)|0x80); \
        (s)[(i)++]=(uint8_t)((__uc&0x3f)|0x80); \
    } وإلا { \
        (isError)=true; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Advance the string offset from one code point boundary to the next.
 * (Post-incrementing iteration.)
 * "Unsafe" macro, assumes well-formed UTF-8.
 *
 * @param s const uint8_t * string
 * @param i string offset
 * @see U8_FWD_1
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_FWD_1_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    (i)+=1+U8_ڳڻپ_TRAIL_بائيٽون_UNSAFE((s)[i]); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Advance the string offset from one code point boundary to the next.
 * (Post-incrementing iteration.)
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 *
 * The length can be negative for a NUL-terminated string.
 *
 * @param s const uint8_t * string
 * @param i int32_t string offset, must be i<length
 * @param length int32_t string length
 * @see U8_FWD_1_UNSAFE
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_FWD_1(s, i, ڊيگهائي) UPRV_بلاڪ_MACRO_BEGIN { \
    uint8_t __b=(s)[(i)++]; \
    إذا(U8_IS_LEAD(__b) && (i)!=(ڊيگهائي)) { \
        uint8_t __t1=(s)[i]; \
        إذا((0xe0<=__b && __b<0xf0)) { \
            إذا(U8_IS_VALID_LEAD3_AND_T1(__b, __t1) && \
                    ++(i)!=(ڊيگهائي) && U8_IS_TRAIL((s)[i])) { \
                ++(i); \
            } \
        } وإلا إذا(__b<0xe0) { \
            إذا(U8_IS_TRAIL(__t1)) { \
                ++(i); \
            } \
        } وإلا /* b>=0xf0 */ { \
            إذا(U8_IS_VALID_LEAD4_AND_T1(__b, __t1) && \
                    ++(i)!=(ڊيگهائي) && U8_IS_TRAIL((s)[i]) && \
                    ++(i)!=(ڊيگهائي) && U8_IS_TRAIL((s)[i])) { \
                ++(i); \
            } \
        } \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Advance the string offset from one code point boundary to the n-th next one,
 * i.e., move forward by n code points.
 * (Post-incrementing iteration.)
 * "Unsafe" macro, assumes well-formed UTF-8.
 *
 * @param s const uint8_t * string
 * @param i string offset
 * @param n number of code points to skip
 * @see U8_FWD_N
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_FWD_N_UNSAFE(s, i, n) UPRV_بلاڪ_MACRO_BEGIN { \
    int32_t __N=(n); \
    طالما(__N>0) { \
        U8_FWD_1_UNSAFE(s, i); \
        --__N; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Advance the string offset from one code point boundary to the n-th next one,
 * i.e., move forward by n code points.
 * (Post-incrementing iteration.)
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 *
 * The length can be negative for a NUL-terminated string.
 *
 * @param s const uint8_t * string
 * @param i int32_t string offset, must be i<length
 * @param length int32_t string length
 * @param n number of code points to skip
 * @see U8_FWD_N_UNSAFE
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_FWD_N(s, i, ڊيگهائي, n) UPRV_بلاڪ_MACRO_BEGIN { \
    int32_t __N=(n); \
    طالما(__N>0 && ((i)<(ڊيگهائي) || ((ڊيگهائي)<0 && (s)[i]!=0))) { \
        U8_FWD_1(s, i, ڊيگهائي); \
        --__N; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Adjust a random-access offset to a code point boundary
 * at the start of a code point.
 * If the offset points to a UTF-8 trail byte,
 * then the offset is moved backward to the corresponding lead byte.
 * Otherwise, it is not modified.
 * "Unsafe" macro, assumes well-formed UTF-8.
 *
 * @param s const uint8_t * string
 * @param i string offset
 * @see U8_SET_CP_START
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_سيٽ_CP_شروعات_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    طالما(U8_IS_TRAIL((s)[i])) { --(i); } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Adjust a random-access offset to a code point boundary
 * at the start of a code point.
 * If the offset points to a UTF-8 trail byte,
 * then the offset is moved backward to the corresponding lead byte.
 * Otherwise, it is not modified.
 *
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 * Unlike U8_TRUNCATE_IF_INCOMPLETE(), this macro always reads s[i].
 *
 * @param s const uint8_t * string
 * @param start int32_t starting string offset (usually 0)
 * @param i int32_t string offset, must be start<=i
 * @see U8_SET_CP_START_UNSAFE
 * @see U8_TRUNCATE_IF_INCOMPLETE
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_سيٽ_CP_شروعات(s, شروعات, i) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(U8_IS_TRAIL((s)[(i)])) { \
        (i)=utf8_back1_محفوظ_Body(s, شروعات, (i)); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * If the string ends with a UTF-8 byte sequence that is valid so far
 * but incomplete, then reduce the length of the string to end before
 * the lead byte of that incomplete sequence.
 * For example, if the string ends with E1 80, the length is reduced by 2.
 *
 * In all other cases (the string ends with a complete sequence, or it is not
 * possible for any further trail byte to extend the trailing sequence)
 * the length remains unchanged.
 *
 * Useful for processing text split across multiple buffers
 * (save the incomplete sequence for later)
 * and for optimizing iteration
 * (check for string length only once per character).
 *
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 * Unlike U8_SET_CP_START(), this macro never reads s[length].
 *
 * (In UTF-16, simply check for U16_IS_LEAD(last code unit).)
 *
 * @param s const uint8_t * string
 * @param start int32_t starting string offset (usually 0)
 * @param length int32_t string length (usually start<=length)
 * @see U8_SET_CP_START
 * @stable ICU 61
 */
#وصف_ڪريو U8_TRUNCATE_IF_INCOMPLETE(s, شروعات, ڊيگهائي) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا((ڊيگهائي)>(شروعات)) { \
        uint8_t __b1=s[(ڊيگهائي)-1]; \
        إذا(U8_IS_SINGLE(__b1)) { \
            /* common ASCII character */ \
        } وإلا إذا(U8_IS_LEAD(__b1)) { \
            --(ڊيگهائي); \
        } وإلا إذا(U8_IS_TRAIL(__b1) && ((ڊيگهائي)-2)>=(شروعات)) { \
            uint8_t __b2=s[(ڊيگهائي)-2]; \
            إذا(0xe0<=__b2 && __b2<=0xf4) { \
                إذا(__b2<0xf0 ? U8_IS_VALID_LEAD3_AND_T1(__b2, __b1) : \
                        U8_IS_VALID_LEAD4_AND_T1(__b2, __b1)) { \
                    (ڊيگهائي)-=2; \
                } \
            } وإلا إذا(U8_IS_TRAIL(__b2) && ((ڊيگهائي)-3)>=(شروعات)) { \
                uint8_t __b3=s[(ڊيگهائي)-3]; \
                إذا(0xf0<=__b3 && __b3<=0xf4 && U8_IS_VALID_LEAD4_AND_T1(__b3, __b2)) { \
                    (ڊيگهائي)-=3; \
                } \
            } \
        } \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/* definitions with backward iteration -------------------------------------- */

/**
 * Move the string offset from one code point boundary to the previous one
 * and get the code point between them.
 * (Pre-decrementing backward iteration.)
 * "Unsafe" macro, assumes well-formed UTF-8.
 *
 * The input offset may be the same as the string length.
 * If the offset is behind a multi-byte sequence, then the macro will read
 * the whole sequence.
 * If the offset is behind a lead byte, then that itself
 * will be returned as the code point.
 * The result is undefined if the offset is behind an illegal UTF-8 sequence.
 *
 * @param s const uint8_t * string
 * @param i string offset
 * @param c output UChar32 variable
 * @see U8_PREV
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_PREV_UNSAFE(s, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(uint8_t)(s)[--(i)]; \
    إذا(!U8_IS_SINGLE(c)) { \
        uint8_t __b, __count=1, __shift=6; \
\
        /* c is a trail byte */ \
        (c)&=0x3f; \
        لكل(;;) { \
            __b=(s)[--(i)]; \
            إذا(__b>=0xc0) { \
                U8_نقاب_LEAD_BYTE(__b, __count); \
                (c)|=(UChar32)__b<<__shift; \
                اكسر; \
            } وإلا { \
                (c)|=(UChar32)(__b&0x3f)<<__shift; \
                ++__count; \
                __shift+=6; \
            } \
        } \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Move the string offset from one code point boundary to the previous one
 * and get the code point between them.
 * (Pre-decrementing backward iteration.)
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 *
 * The input offset may be the same as the string length.
 * If the offset is behind a multi-byte sequence, then the macro will read
 * the whole sequence.
 * If the offset is behind a lead byte, then that itself
 * will be returned as the code point.
 * If the offset is behind an illegal UTF-8 sequence, then c is set to a negative value.
 *
 * @param s const uint8_t * string
 * @param start int32_t starting string offset (usually 0)
 * @param i int32_t string offset, must be start<i
 * @param c output UChar32 variable, set to <0 in case of an error
 * @see U8_PREV_UNSAFE
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_PREV(s, شروعات, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(uint8_t)(s)[--(i)]; \
    إذا(!U8_IS_SINGLE(c)) { \
        (c)=utf8_prev_Char_محفوظ_Body((ثابت uint8_t *)s, شروعات, &(i), c, -1); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Move the string offset from one code point boundary to the previous one
 * and get the code point between them.
 * (Pre-decrementing backward iteration.)
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 *
 * The input offset may be the same as the string length.
 * If the offset is behind a multi-byte sequence, then the macro will read
 * the whole sequence.
 * If the offset is behind a lead byte, then that itself
 * will be returned as the code point.
 * If the offset is behind an illegal UTF-8 sequence, then c is set to U+FFFD.
 *
 * This macro does not distinguish between a real U+FFFD in the text
 * and U+FFFD returned for an ill-formed sequence.
 * Use U8_PREV() if that distinction is important.
 *
 * @param s const uint8_t * string
 * @param start int32_t starting string offset (usually 0)
 * @param i int32_t string offset, must be start<i
 * @param c output UChar32 variable, set to U+FFFD in case of an error
 * @see U8_PREV
 * @stable ICU 51
 */
#وصف_ڪريو U8_PREV_OR_FFFD(s, شروعات, i, c) UPRV_بلاڪ_MACRO_BEGIN { \
    (c)=(uint8_t)(s)[--(i)]; \
    إذا(!U8_IS_SINGLE(c)) { \
        (c)=utf8_prev_Char_محفوظ_Body((ثابت uint8_t *)s, شروعات, &(i), c, -3); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Move the string offset from one code point boundary to the previous one.
 * (Pre-decrementing backward iteration.)
 * The input offset may be the same as the string length.
 * "Unsafe" macro, assumes well-formed UTF-8.
 *
 * @param s const uint8_t * string
 * @param i string offset
 * @see U8_BACK_1
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_پٺ_1_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    طالما(U8_IS_TRAIL((s)[--(i)])) {} \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Move the string offset from one code point boundary to the previous one.
 * (Pre-decrementing backward iteration.)
 * The input offset may be the same as the string length.
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 *
 * @param s const uint8_t * string
 * @param start int32_t starting string offset (usually 0)
 * @param i int32_t string offset, must be start<i
 * @see U8_BACK_1_UNSAFE
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_پٺ_1(s, شروعات, i) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(U8_IS_TRAIL((s)[--(i)])) { \
        (i)=utf8_back1_محفوظ_Body(s, شروعات, (i)); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Move the string offset from one code point boundary to the n-th one before it,
 * i.e., move backward by n code points.
 * (Pre-decrementing backward iteration.)
 * The input offset may be the same as the string length.
 * "Unsafe" macro, assumes well-formed UTF-8.
 *
 * @param s const uint8_t * string
 * @param i string offset
 * @param n number of code points to skip
 * @see U8_BACK_N
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_پٺ_N_UNSAFE(s, i, n) UPRV_بلاڪ_MACRO_BEGIN { \
    int32_t __N=(n); \
    طالما(__N>0) { \
        U8_پٺ_1_UNSAFE(s, i); \
        --__N; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Move the string offset from one code point boundary to the n-th one before it,
 * i.e., move backward by n code points.
 * (Pre-decrementing backward iteration.)
 * The input offset may be the same as the string length.
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 *
 * @param s const uint8_t * string
 * @param start int32_t index of the start of the string
 * @param i int32_t string offset, must be start<i
 * @param n number of code points to skip
 * @see U8_BACK_N_UNSAFE
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_پٺ_N(s, شروعات, i, n) UPRV_بلاڪ_MACRO_BEGIN { \
    int32_t __N=(n); \
    طالما(__N>0 && (i)>(شروعات)) { \
        U8_پٺ_1(s, شروعات, i); \
        --__N; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Adjust a random-access offset to a code point boundary after a code point.
 * If the offset is behind a partial multi-byte sequence,
 * then the offset is incremented to behind the whole sequence.
 * Otherwise, it is not modified.
 * The input offset may be the same as the string length.
 * "Unsafe" macro, assumes well-formed UTF-8.
 *
 * @param s const uint8_t * string
 * @param i string offset
 * @see U8_SET_CP_LIMIT
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_سيٽ_CP_حد_بندي_UNSAFE(s, i) UPRV_بلاڪ_MACRO_BEGIN { \
    U8_پٺ_1_UNSAFE(s, i); \
    U8_FWD_1_UNSAFE(s, i); \
} UPRV_بلاڪ_MACRO_پڇاڙي

/**
 * Adjust a random-access offset to a code point boundary after a code point.
 * If the offset is behind a partial multi-byte sequence,
 * then the offset is incremented to behind the whole sequence.
 * Otherwise, it is not modified.
 * The input offset may be the same as the string length.
 * "Safe" macro, checks for illegal sequences and for string boundaries.
 *
 * The length can be negative for a NUL-terminated string.
 *
 * @param s const uint8_t * string
 * @param start int32_t starting string offset (usually 0)
 * @param i int32_t string offset, must be start<=i<=length
 * @param length int32_t string length
 * @see U8_SET_CP_LIMIT_UNSAFE
 * @stable ICU 2.4
 */
#وصف_ڪريو U8_سيٽ_CP_حد_بندي(s, شروعات, i, ڊيگهائي) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا((شروعات)<(i) && ((i)<(ڊيگهائي) || (ڊيگهائي)<0)) { \
        U8_پٺ_1(s, شروعات, i); \
        U8_FWD_1(s, i, ڊيگهائي); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

#جيڪڏهن_پورو
