// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1997-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  cpputils.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*/

#اگر_غیر_تعریف_شدہ CPPUTILS_H
#تعریف_کرو CPPUTILS_H

#شامل "unicode/utypes.h"
#شامل "unicode/unistr.h"
#شامل "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

ساکن
اندرونی خالی uprv_arrayCopy(ثابت دوہرا* src, دوہرا* dst, int32_t گنتی)
{ uprv_memcpy(dst, src, (size_t)گنتی * حجم(*src)); }

ساکن
اندرونی خالی uprv_arrayCopy(ثابت دوہرا* src, int32_t src_آغاز,
              دوہرا* dst, int32_t dst_آغاز, int32_t گنتی)
{ uprv_memcpy(dst+dst_آغاز, src+src_آغاز, (size_t)گنتی * حجم(*src)); }

ساکن
اندرونی خالی uprv_arrayCopy(ثابت int8_t* src, int8_t* dst, int32_t گنتی)
    { uprv_memcpy(dst, src, (size_t)گنتی * حجم(*src)); }

ساکن
اندرونی خالی uprv_arrayCopy(ثابت int8_t* src, int32_t src_آغاز,
              int8_t* dst, int32_t dst_آغاز, int32_t گنتی)
{ uprv_memcpy(dst+dst_آغاز, src+src_آغاز, (size_t)گنتی * حجم(*src)); }

ساکن
اندرونی خالی uprv_arrayCopy(ثابت int16_t* src, int16_t* dst, int32_t گنتی)
{ uprv_memcpy(dst, src, (size_t)گنتی * حجم(*src)); }

ساکن
اندرونی خالی uprv_arrayCopy(ثابت int16_t* src, int32_t src_آغاز,
              int16_t* dst, int32_t dst_آغاز, int32_t گنتی)
{ uprv_memcpy(dst+dst_آغاز, src+src_آغاز, (size_t)گنتی * حجم(*src)); }

ساکن
اندرونی خالی uprv_arrayCopy(ثابت int32_t* src, int32_t* dst, int32_t گنتی)
{ uprv_memcpy(dst, src, (size_t)گنتی * حجم(*src)); }

ساکن
اندرونی خالی uprv_arrayCopy(ثابت int32_t* src, int32_t src_آغاز,
              int32_t* dst, int32_t dst_آغاز, int32_t گنتی)
{ uprv_memcpy(dst+dst_آغاز, src+src_آغاز, (size_t)گنتی * حجم(*src)); }

ساکن
اندرونی خالی
uprv_arrayCopy(ثابت char16_t *src, int32_t src_آغاز,
        char16_t *dst, int32_t dst_آغاز, int32_t گنتی)
{ uprv_memcpy(dst+dst_آغاز, src+src_آغاز, (size_t)گنتی * حجم(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ساکن اندرونی خالی
uprv_arrayCopy(ثابت icu::UnicodeString *src, icu::UnicodeString *dst, int32_t گنتی)
{ جب(گنتی-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ساکن اندرونی خالی
uprv_arrayCopy(ثابت icu::UnicodeString *src, int32_t src_آغاز,
               icu::UnicodeString *dst, int32_t dst_آغاز, int32_t گنتی)
{ uprv_arrayCopy(src+src_آغاز, dst+dst_آغاز, گنتی); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
اندرونی خالی
uprv_check_Can_Get_بفر(ثابت icu::UnicodeString &s, UError_رمز &error_رمز) {
    اگر(U_SUCCESS(error_رمز) && s.isBogus()) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
    }
}

#اگر_ختم /* _CPPUTILS */
