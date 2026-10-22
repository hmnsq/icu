// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*  
**********************************************************************
*   Copyright (C) 1999-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ustr_imp.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2001jan30
*   created by: Markus W. Scherer
*/

#اگر_غیر_تعریف_شدہ __USTR_IMP_H__
#تعریف_کرو __USTR_IMP_H__

#شامل "unicode/utypes.h"
#شامل "unicode/utf8.h"

/**
 * Internal option for unorm_cmpEquivFold() for strncmp style.
 * If set, checks for both string length and terminating NUL.
 */
#تعریف_کرو _STRNCMP_STYLE 0x1000

/**
 * Compare two strings in code point order or code unit order.
 * Works in strcmp style (both lengths -1),
 * strncmp style (lengths equal and >=0, flag true),
 * and memcmp/UnicodeString style (at least one length >=0).
 */
U_CFUNC int32_t U_EXPORT2
uprv_strCompare(ثابت UChar *s1, int32_t length1,
                ثابت UChar *s2, int32_t length2,
                UBool strncmpStyle, UBool رمز_نقطہ_ترتیب);

U_CAPI int32_t U_EXPORT2 
ustr_hashUCharsN(ثابت UChar *str, int32_t لمبائی);

U_CAPI int32_t U_EXPORT2 
ustr_hashCharsN(ثابت حرف *str, int32_t لمبائی);

U_CAPI int32_t U_EXPORT2
ustr_hashICharsN(ثابت حرف *str, int32_t لمبائی);

/**
 * Convert an ASCII-range lowercase character to uppercase.
 * 
 * @param c A UChar.
 * @return If UChar is a lowercase ASCII character, returns the uppercase version.
 *         Otherwise, returns the input character.
 */
U_CAPI UChar U_EXPORT2
u_ascii_مقصد_بالائی(UChar c);

// TODO: Add u_asciiToLower if/when there is a need for it.

/**
 * NUL-terminate a UChar * string if possible.
 * If length  < destCapacity then NUL-terminate.
 * If length == destCapacity then do not terminate but set U_STRING_NOT_TERMINATED_WARNING.
 * If length  > destCapacity then do not terminate but set U_BUFFER_OVERFLOW_ERROR.
 *
 * @param dest Destination buffer, can be NULL if destCapacity==0.
 * @param destCapacity Number of UChars available at dest.
 * @param length Number of UChars that were (to be) written to dest.
 * @param pErrorCode ICU error code.
 * @return length
 */
U_CAPI int32_t U_EXPORT2
u_terminateUChars(UChar *dest, int32_t dest_استعداد, int32_t لمبائی, UError_رمز *p_Error_رمز);

/**
 * NUL-terminate a char * string if possible.
 * Same as u_terminateUChars() but for a different string type.
 */
U_CAPI int32_t U_EXPORT2
u_terminateChars(حرف *dest, int32_t dest_استعداد, int32_t لمبائی, UError_رمز *p_Error_رمز);

/**
 * NUL-terminate a UChar32 * string if possible.
 * Same as u_terminateUChars() but for a different string type.
 */
U_CAPI int32_t U_EXPORT2
u_terminateUChar32s(UChar32 *dest, int32_t dest_استعداد, int32_t لمبائی, UError_رمز *p_Error_رمز);

/**
 * NUL-terminate a wchar_t * string if possible.
 * Same as u_terminateUChars() but for a different string type.
 */
U_CAPI int32_t U_EXPORT2
u_terminateWChars(wchar_t *dest, int32_t dest_استعداد, int32_t لمبائی, UError_رمز *p_Error_رمز);

/**
 * Counts the bytes of any whole valid sequence for a UTF-8 lead byte.
 * Returns 1 for ASCII 0..0x7f.
 * Returns 0 for 0x80..0xc1 as well as for 0xf5..0xff.
 * leadByte might be evaluated multiple times.
 *
 * @param leadByte The first byte of a UTF-8 sequence. Must be 0..0xff.
 * @return 0..4
 */
#تعریف_کرو U8_گنتی_بائٹیں(leadByte) \
    (U8_IS_SINGLE(leadByte) ? 1 : U8_گنتی_بائٹیں_NON_ASCII(leadByte))

/**
 * Counts the bytes of any whole valid sequence for a UTF-8 lead byte.
 * Returns 0 for 0x00..0xc1 as well as for 0xf5..0xff.
 * leadByte might be evaluated multiple times.
 *
 * @param leadByte The first byte of a UTF-8 sequence. Must be 0..0xff.
 * @return 0 or 2..4
 */
#تعریف_کرو U8_گنتی_بائٹیں_NON_ASCII(leadByte) \
    (U8_IS_LEAD(leadByte) ? ((uint8_t)(leadByte)>=0xe0)+((uint8_t)(leadByte)>=0xf0)+2 : 0)

#اگر_تعریف_شدہ __cplusplus

U_NAMESPACE_BEGIN

class UTF8 {
عوامی:
    UTF8() = delete;  // all static

    /**
     * Is t a valid UTF-8 trail byte?
     *
     * @param prev Must be the preceding lead byte if i==1 and length>=3;
     *             otherwise ignored.
     * @param t The i-th byte following the lead byte.
     * @param i The index (1..3) of byte t in the byte sequence. 0<i<length
     * @param length The length (2..4) of the byte sequence according to the lead byte.
     * @return true if t is a valid trail byte in this context.
     */
    ساکن اندرونی UBool isValidTrail(int32_t پچھلا, uint8_t t, int32_t i, int32_t لمبائی) {
        // The first trail byte after a 3- or 4-byte lead byte
        // needs to be validated together with its lead byte.
        اگر (لمبائی <= 2 || i > 1) {
            واپس U8_IS_TRAIL(t);
        } ورنہ اگر (لمبائی == 3) {
            واپس U8_IS_VALID_LEAD3_AND_T1(پچھلا, t);
        } ورنہ {  // length == 4
            واپس U8_IS_VALID_LEAD4_AND_T1(پچھلا, t);
        }
    }
};

U_NAMESPACE_انتہا

#اگر_ختم  // __cplusplus

#اگر_ختم
