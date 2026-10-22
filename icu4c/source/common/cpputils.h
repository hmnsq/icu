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

#اگر_تعریف_نشده CPPUTILS_H
#تعریف_کن CPPUTILS_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/unistr.h"
#ضمن "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

ساكن
مضمن فراغ uprv_arrayCopy(ثابت مزدوج* src, مزدوج* dst, int32_t شمار)
{ uprv_memcpy(dst, src, (size_t)شمار * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت مزدوج* src, int32_t src_شروع,
              مزدوج* dst, int32_t dst_شروع, int32_t شمار)
{ uprv_memcpy(dst+dst_شروع, src+src_شروع, (size_t)شمار * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int8_t* src, int8_t* dst, int32_t شمار)
    { uprv_memcpy(dst, src, (size_t)شمار * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int8_t* src, int32_t src_شروع,
              int8_t* dst, int32_t dst_شروع, int32_t شمار)
{ uprv_memcpy(dst+dst_شروع, src+src_شروع, (size_t)شمار * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int16_t* src, int16_t* dst, int32_t شمار)
{ uprv_memcpy(dst, src, (size_t)شمار * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int16_t* src, int32_t src_شروع,
              int16_t* dst, int32_t dst_شروع, int32_t شمار)
{ uprv_memcpy(dst+dst_شروع, src+src_شروع, (size_t)شمار * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int32_t* src, int32_t* dst, int32_t شمار)
{ uprv_memcpy(dst, src, (size_t)شمار * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int32_t* src, int32_t src_شروع,
              int32_t* dst, int32_t dst_شروع, int32_t شمار)
{ uprv_memcpy(dst+dst_شروع, src+src_شروع, (size_t)شمار * حجم(*src)); }

ساكن
مضمن فراغ
uprv_arrayCopy(ثابت char16_t *src, int32_t src_شروع,
        char16_t *dst, int32_t dst_شروع, int32_t شمار)
{ uprv_memcpy(dst+dst_شروع, src+src_شروع, (size_t)شمار * حجم(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ساكن مضمن فراغ
uprv_arrayCopy(ثابت icu::UnicodeString *src, icu::UnicodeString *dst, int32_t شمار)
{ طالما(شمار-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ساكن مضمن فراغ
uprv_arrayCopy(ثابت icu::UnicodeString *src, int32_t src_شروع,
               icu::UnicodeString *dst, int32_t dst_شروع, int32_t شمار)
{ uprv_arrayCopy(src+src_شروع, dst+dst_شروع, شمار); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
مضمن فراغ
uprv_check_Can_Get_میانگیر(ثابت icu::UnicodeString &s, UError_کد &error_کد) {
    إذا(U_SUCCESS(error_کد) && s.isBogus()) {
        error_کد=U_ILLEGAL_آرگومان_ERROR;
    }
}

#پایان_اگر /* _CPPUTILS */
