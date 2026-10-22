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

#اگر_غیر_تعریف_شدہ __UTF8_H__
#تعریف_کرو __UTF8_H__

#شامل <stdbool.h>
#شامل "unicode/umachine.h"
#اگر_غیر_تعریف_شدہ __UTF_H__
#   include "unicode/utf.h"
#اگر_ختم

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
#تعریف_کرو U8_گنتی_TRAIL_بائٹیں(leadByte) \
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
#تعریف_کرو U8_گنتی_TRAIL_بائٹیں_UNSAFE(leadByte) \
    (((uint8_t)(leadByte)>=0xc2)+((uint8_t)(leadByte)>=0xe0)+((uint8_t)(leadByte)>=0xf0))

/**
 * Mask a UTF-8 lead byte, leave only the lower bits that form part of the code point value.
 *
 * This is internal since it is not meant to be called directly by external clients;
 * however it is called by public macros in this file and thus must remain stable.
 * @internal
 */
#تعریف_کرو U8_نقاب_LEAD_BYTE(leadByte, گنتی_Trail_بائٹیں) ((leadByte)&=(1<<(6-(گنتی_Trail_بائٹیں)))-1)

/**
 * Internal bit vector for 3-byte UTF-8 validity check, for use in U8_IS_VALID_LEAD3_AND_T1.
 * Each bit indicates whether one lead byte + first trail byte pair starts a valid sequence.
 * Lead byte E0..EF bits 3..0 are used as byte index,
 * first trail byte bits 7..5 are used as bit index into that byte.
 * @see U8_IS_VALID_LEAD3_AND_T1
 * @internal
 */
#تعریف_کرو U8_LEAD3_T1_بٹ "\x20\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x10\x30\x30"

/**
 * Internal 3-byte UTF-8 validity check.
 * Non-zero if lead byte E0..EF and first trail byte 00..FF start a valid sequence.
 * @internal
 */
#تعریف_کرو U8_IS_VALID_LEAD3_AND_T1(lead, t1) (U8_LEAD3_T1_بٹ[(lead)&0xf]&(1<<((uint8_t)(t1)>>5)))

/**
 * Internal bit vector for 4-byte UTF-8 validity check, for use in U8_IS_VALID_LEAD4_AND_T1.
 * Each bit indicates whether one lead byte + first trail byte pair starts a valid sequence.
 * First trail byte bits 7..4 are used as byte index,
 * lead byte F0..F4 bits 2..0 are used as bit index into that byte.
 * @see U8_IS_VALID_LEAD4_AND_T1
 * @internal
 */
#تعریف_کرو U8_LEAD4_T1_بٹ "\x00\x00\x00\x00\x00\x00\x00\x00\x1E\x0F\x0F\x0F\x00\x00\x00\x00"

/**
 * Internal 4-byte UTF-8 validity check.
 * Non-zero if lead byte F0..F4 and first trail byte 00..FF start a valid sequence.
 * @internal
 */
#تعریف_کرو U8_IS_VALID_LEAD4_AND_T1(lead, t1) (U8_LEAD4_T1_بٹ[(uint8_t)(t1)>>4]&(1<<((lead)&7)))

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
utf8_اگلا_Char_محفوظ_Body(ثابت uint8_t *s, int32_t *pi, int32_t لمبائی, UChar32 c, int8_t strict);

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
utf8_append_Char_محفوظ_Body(uint8_t *s, int32_t i, int32_t لمبائی, UChar32 c, UBool *pIsError);

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
utf8_پچھلا_Char_محفوظ_Body(ثابت uint8_t *s, int32_t آغاز, int32_t *pi, UChar32 c, int8_t strict);

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
utf8_back1_محفوظ_Body(ثابت uint8_t *s, int32_t آغاز, int32_t i);

/* single-code point definitions -------------------------------------------- */

/**
 * Does this code unit (byte) encode a code point by itself (US-ASCII 0..0x7f)?
 * @param c 8-bit code unit (byte)
 * @return true or false
 * @stable ICU 2.4
 */
#تعریف_کرو U8_IS_SINGLE(c) ((int8_t)(c)>=0)

/**
 * Is this code unit (byte) a UTF-8 lead byte? (0xC2..0xF4)
 * @param c 8-bit code unit (byte)
 * @return true or false
 * @stable ICU 2.4
 */
#تعریف_کرو U8_IS_LEAD(c) ((uint8_t)((c)-0xc2)<=0x32)
// 0x32=0xf4-0xc2

/**
 * Is this code unit (byte) a UTF-8 trail byte? (0x80..0xBF)
 * @param c 8-bit code unit (byte)
 * @return true or false
 * @stable ICU 2.4
 */
#تعریف_کرو U8_IS_TRAIL(c) ((int8_t)(c)<-0x40)

/**
 * How many code units (bytes) are used for the UTF-8 encoding
 * of this Unicode code point?
 * @param c 32-bit code point
 * @return 1..4, or 0 if c is a surrogate or not a Unicode code point
 * @stable ICU 2.4
 */
#تعریف_کرو U8_لمبائی(c) \
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
#تعریف_کرو U8_MAX_لمبائی 4

#اگر_غیر_تعریف_شدہ U_HIDE_DRAFT_API

/**
 * Returns the length of a well-formed UTF-8 byte sequence according to its lead byte.
 * Returns 1 for 0..0xc1 as well as for 0xf5..0xff.
 * leadByte might be evaluated multiple times.
 *
 * @param leadByte The first byte of a UTF-8 sequence. Must be 0..0xff.
 * @return 1..4
 * @draft ICU 78
 */
#تعریف_کرو U8_لمبائی_FROM_LEAD_BYTE(leadByte) (U8_گنتی_TRAIL_بائٹیں(leadByte) + 1)

/**
 * Returns the length of a well-formed UTF-8 byte sequence according to its lead byte.
 * Returns 1 for 0..0xc1. Undefined for 0xf5..0xff.
 * leadByte might be evaluated multiple times.
 *
 * @param leadByte The first byte of a UTF-8 sequence. Must be 0..0xff.
 * @return 1..4
 * @draft ICU 78
 */
#تعریف_کرو U8_لمبائی_FROM_LEAD_BYTE_UNSAFE(leadByte) (U8_گنتی_TRAIL_بائٹیں_UNSAFE(leadByte) + 1)

#اگر_ختم  // U_HIDE_DRAFT_API

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
#تعریف_کرو U8_GET_UNSAFE(s, i, c) UPRV_بلاک_MACRO_BEGIN { \
    int32_t u8_get_unsafe_اشاریہ=(int32_t)(i); \
    U8_سیٹ_CP_آغاز_UNSAFE(s, u8_get_unsafe_اشاریہ); \
    U8_اگلا_UNSAFE(s, u8_get_unsafe_اشاریہ, c); \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_GET(s, آغاز, i, لمبائی, c) UPRV_بلاک_MACRO_BEGIN { \
    int32_t u8_get_اشاریہ=(i); \
    U8_سیٹ_CP_آغاز(s, آغاز, u8_get_اشاریہ); \
    U8_اگلا(s, u8_get_اشاریہ, لمبائی, c); \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_GET_OR_FFFD(s, آغاز, i, لمبائی, c) UPRV_بلاک_MACRO_BEGIN { \
    int32_t u8_get_اشاریہ=(i); \
    U8_سیٹ_CP_آغاز(s, آغاز, u8_get_اشاریہ); \
    U8_اگلا_OR_FFFD(s, u8_get_اشاریہ, لمبائی, c); \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_اگلا_UNSAFE(s, i, c) UPRV_بلاک_MACRO_BEGIN { \
    (c)=(uint8_t)(s)[(i)++]; \
    اگر(!U8_IS_SINGLE(c)) { \
        اگر((c)<0xe0) { \
            (c)=(((c)&0x1f)<<6)|((s)[(i)++]&0x3f); \
        } ورنہ اگر((c)<0xf0) { \
            /* no need for (c&0xf) because the upper bits are truncated after <<12 in the cast to (UChar) */ \
            (c)=(UChar)(((c)<<12)|(((s)[i]&0x3f)<<6)|((s)[(i)+1]&0x3f)); \
            (i)+=2; \
        } ورنہ { \
            (c)=(((c)&7)<<18)|(((s)[i]&0x3f)<<12)|(((s)[(i)+1]&0x3f)<<6)|((s)[(i)+2]&0x3f); \
            (i)+=3; \
        } \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_اگلا(s, i, لمبائی, c) U8_اندرونی_اگلا_OR_SUB(s, i, لمبائی, c, U_SENTINEL)

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
#تعریف_کرو U8_اگلا_OR_FFFD(s, i, لمبائی, c) U8_اندرونی_اگلا_OR_SUB(s, i, لمبائی, c, 0xfffd)

/** @internal */
#تعریف_کرو U8_اندرونی_اگلا_OR_SUB(s, i, لمبائی, c, sub) UPRV_بلاک_MACRO_BEGIN { \
    (c)=(uint8_t)(s)[(i)++]; \
    اگر(!U8_IS_SINGLE(c)) { \
        uint8_t __t = 0; \
        اگر((i)!=(لمبائی) && \
            /* fetch/validate/assemble all but last trail byte */ \
            ((c)>=0xe0 ? \
                ((c)<0xf0 ?  /* U+0800..U+FFFF except surrogates */ \
                    U8_LEAD3_T1_بٹ[(c)&=0xf]&(1<<((__t=(s)[i])>>5)) && \
                    (__t&=0x3f, 1) \
                :  /* U+10000..U+10FFFF */ \
                    ((c)-=0xf0)<=4 && \
                    U8_LEAD4_T1_بٹ[(__t=(s)[i])>>4]&(1<<(c)) && \
                    ((c)=((c)<<6)|(__t&0x3f), ++(i)!=(لمبائی)) && \
                    (__t=(s)[i]-0x80)<=0x3f) && \
                /* valid second-to-last trail byte */ \
                ((c)=((c)<<6)|__t, ++(i)!=(لمبائی)) \
            :  /* U+0080..U+07FF */ \
                (c)>=0xc2 && ((c)&=0x1f, 1)) && \
            /* last trail byte */ \
            (__t=(s)[i]-0x80)<=0x3f && \
            ((c)=((c)<<6)|__t, ++(i), 1)) { \
        } ورنہ { \
            (c)=(sub);  /* ill-formed*/ \
        } \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_APPEND_UNSAFE(s, i, c) UPRV_بلاک_MACRO_BEGIN { \
    uint32_t __uc=(c); \
    اگر(__uc<=0x7f) { \
        (s)[(i)++]=(uint8_t)__uc; \
    } ورنہ { \
        اگر(__uc<=0x7ff) { \
            (s)[(i)++]=(uint8_t)((__uc>>6)|0xc0); \
        } ورنہ { \
            اگر(__uc<=0xffff) { \
                (s)[(i)++]=(uint8_t)((__uc>>12)|0xe0); \
            } ورنہ { \
                (s)[(i)++]=(uint8_t)((__uc>>18)|0xf0); \
                (s)[(i)++]=(uint8_t)(((__uc>>12)&0x3f)|0x80); \
            } \
            (s)[(i)++]=(uint8_t)(((__uc>>6)&0x3f)|0x80); \
        } \
        (s)[(i)++]=(uint8_t)((__uc&0x3f)|0x80); \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_APPEND(s, i, استعداد, c, isError) UPRV_بلاک_MACRO_BEGIN { \
    uint32_t __uc=(c); \
    اگر(__uc<=0x7f) { \
        (s)[(i)++]=(uint8_t)__uc; \
    } ورنہ اگر(__uc<=0x7ff && (i)+1<(استعداد)) { \
        (s)[(i)++]=(uint8_t)((__uc>>6)|0xc0); \
        (s)[(i)++]=(uint8_t)((__uc&0x3f)|0x80); \
    } ورنہ اگر((__uc<=0xd7ff || (0xe000<=__uc && __uc<=0xffff)) && (i)+2<(استعداد)) { \
        (s)[(i)++]=(uint8_t)((__uc>>12)|0xe0); \
        (s)[(i)++]=(uint8_t)(((__uc>>6)&0x3f)|0x80); \
        (s)[(i)++]=(uint8_t)((__uc&0x3f)|0x80); \
    } ورنہ اگر(0xffff<__uc && __uc<=0x10ffff && (i)+3<(استعداد)) { \
        (s)[(i)++]=(uint8_t)((__uc>>18)|0xf0); \
        (s)[(i)++]=(uint8_t)(((__uc>>12)&0x3f)|0x80); \
        (s)[(i)++]=(uint8_t)(((__uc>>6)&0x3f)|0x80); \
        (s)[(i)++]=(uint8_t)((__uc&0x3f)|0x80); \
    } ورنہ { \
        (isError)=true; \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_FWD_1_UNSAFE(s, i) UPRV_بلاک_MACRO_BEGIN { \
    (i)+=1+U8_گنتی_TRAIL_بائٹیں_UNSAFE((s)[i]); \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_FWD_1(s, i, لمبائی) UPRV_بلاک_MACRO_BEGIN { \
    uint8_t __b=(s)[(i)++]; \
    اگر(U8_IS_LEAD(__b) && (i)!=(لمبائی)) { \
        uint8_t __t1=(s)[i]; \
        اگر((0xe0<=__b && __b<0xf0)) { \
            اگر(U8_IS_VALID_LEAD3_AND_T1(__b, __t1) && \
                    ++(i)!=(لمبائی) && U8_IS_TRAIL((s)[i])) { \
                ++(i); \
            } \
        } ورنہ اگر(__b<0xe0) { \
            اگر(U8_IS_TRAIL(__t1)) { \
                ++(i); \
            } \
        } ورنہ /* b>=0xf0 */ { \
            اگر(U8_IS_VALID_LEAD4_AND_T1(__b, __t1) && \
                    ++(i)!=(لمبائی) && U8_IS_TRAIL((s)[i]) && \
                    ++(i)!=(لمبائی) && U8_IS_TRAIL((s)[i])) { \
                ++(i); \
            } \
        } \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_FWD_N_UNSAFE(s, i, n) UPRV_بلاک_MACRO_BEGIN { \
    int32_t __N=(n); \
    جب(__N>0) { \
        U8_FWD_1_UNSAFE(s, i); \
        --__N; \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_FWD_N(s, i, لمبائی, n) UPRV_بلاک_MACRO_BEGIN { \
    int32_t __N=(n); \
    جب(__N>0 && ((i)<(لمبائی) || ((لمبائی)<0 && (s)[i]!=0))) { \
        U8_FWD_1(s, i, لمبائی); \
        --__N; \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_سیٹ_CP_آغاز_UNSAFE(s, i) UPRV_بلاک_MACRO_BEGIN { \
    جب(U8_IS_TRAIL((s)[i])) { --(i); } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_سیٹ_CP_آغاز(s, آغاز, i) UPRV_بلاک_MACRO_BEGIN { \
    اگر(U8_IS_TRAIL((s)[(i)])) { \
        (i)=utf8_back1_محفوظ_Body(s, آغاز, (i)); \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_TRUNCATE_IF_INCOMPLETE(s, آغاز, لمبائی) UPRV_بلاک_MACRO_BEGIN { \
    اگر((لمبائی)>(آغاز)) { \
        uint8_t __b1=s[(لمبائی)-1]; \
        اگر(U8_IS_SINGLE(__b1)) { \
            /* common ASCII character */ \
        } ورنہ اگر(U8_IS_LEAD(__b1)) { \
            --(لمبائی); \
        } ورنہ اگر(U8_IS_TRAIL(__b1) && ((لمبائی)-2)>=(آغاز)) { \
            uint8_t __b2=s[(لمبائی)-2]; \
            اگر(0xe0<=__b2 && __b2<=0xf4) { \
                اگر(__b2<0xf0 ? U8_IS_VALID_LEAD3_AND_T1(__b2, __b1) : \
                        U8_IS_VALID_LEAD4_AND_T1(__b2, __b1)) { \
                    (لمبائی)-=2; \
                } \
            } ورنہ اگر(U8_IS_TRAIL(__b2) && ((لمبائی)-3)>=(آغاز)) { \
                uint8_t __b3=s[(لمبائی)-3]; \
                اگر(0xf0<=__b3 && __b3<=0xf4 && U8_IS_VALID_LEAD4_AND_T1(__b3, __b2)) { \
                    (لمبائی)-=3; \
                } \
            } \
        } \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_پچھلا_UNSAFE(s, i, c) UPRV_بلاک_MACRO_BEGIN { \
    (c)=(uint8_t)(s)[--(i)]; \
    اگر(!U8_IS_SINGLE(c)) { \
        uint8_t __b, __count=1, __shift=6; \
\
        /* c is a trail byte */ \
        (c)&=0x3f; \
        برائے(;;) { \
            __b=(s)[--(i)]; \
            اگر(__b>=0xc0) { \
                U8_نقاب_LEAD_BYTE(__b, __count); \
                (c)|=(UChar32)__b<<__shift; \
                توڑو; \
            } ورنہ { \
                (c)|=(UChar32)(__b&0x3f)<<__shift; \
                ++__count; \
                __shift+=6; \
            } \
        } \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_پچھلا(s, آغاز, i, c) UPRV_بلاک_MACRO_BEGIN { \
    (c)=(uint8_t)(s)[--(i)]; \
    اگر(!U8_IS_SINGLE(c)) { \
        (c)=utf8_پچھلا_Char_محفوظ_Body((ثابت uint8_t *)s, آغاز, &(i), c, -1); \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_پچھلا_OR_FFFD(s, آغاز, i, c) UPRV_بلاک_MACRO_BEGIN { \
    (c)=(uint8_t)(s)[--(i)]; \
    اگر(!U8_IS_SINGLE(c)) { \
        (c)=utf8_پچھلا_Char_محفوظ_Body((ثابت uint8_t *)s, آغاز, &(i), c, -3); \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_عقب_1_UNSAFE(s, i) UPRV_بلاک_MACRO_BEGIN { \
    جب(U8_IS_TRAIL((s)[--(i)])) {} \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_عقب_1(s, آغاز, i) UPRV_بلاک_MACRO_BEGIN { \
    اگر(U8_IS_TRAIL((s)[--(i)])) { \
        (i)=utf8_back1_محفوظ_Body(s, آغاز, (i)); \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_عقب_N_UNSAFE(s, i, n) UPRV_بلاک_MACRO_BEGIN { \
    int32_t __N=(n); \
    جب(__N>0) { \
        U8_عقب_1_UNSAFE(s, i); \
        --__N; \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_عقب_N(s, آغاز, i, n) UPRV_بلاک_MACRO_BEGIN { \
    int32_t __N=(n); \
    جب(__N>0 && (i)>(آغاز)) { \
        U8_عقب_1(s, آغاز, i); \
        --__N; \
    } \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_سیٹ_CP_حد_بندی_UNSAFE(s, i) UPRV_بلاک_MACRO_BEGIN { \
    U8_عقب_1_UNSAFE(s, i); \
    U8_FWD_1_UNSAFE(s, i); \
} UPRV_بلاک_MACRO_انتہا

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
#تعریف_کرو U8_سیٹ_CP_حد_بندی(s, آغاز, i, لمبائی) UPRV_بلاک_MACRO_BEGIN { \
    اگر((آغاز)<(i) && ((i)<(لمبائی) || (لمبائی)<0)) { \
        U8_عقب_1(s, آغاز, i); \
        U8_FWD_1(s, i, لمبائی); \
    } \
} UPRV_بلاک_MACRO_انتہا

#اگر_ختم
