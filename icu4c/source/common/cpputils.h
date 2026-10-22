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

#که_نه_تعریف_شوی CPPUTILS_H
#تعریف_کړه CPPUTILS_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/unistr.h"
#ضمن "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

ساكن
مضمن فراغ uprv_arrayCopy(ثابت مزدوج* src, مزدوج* dst, int32_t شمېر)
{ uprv_memcpy(dst, src, (size_t)شمېر * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت مزدوج* src, int32_t src_پیل,
              مزدوج* dst, int32_t dst_پیل, int32_t شمېر)
{ uprv_memcpy(dst+dst_پیل, src+src_پیل, (size_t)شمېر * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int8_t* src, int8_t* dst, int32_t شمېر)
    { uprv_memcpy(dst, src, (size_t)شمېر * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int8_t* src, int32_t src_پیل,
              int8_t* dst, int32_t dst_پیل, int32_t شمېر)
{ uprv_memcpy(dst+dst_پیل, src+src_پیل, (size_t)شمېر * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int16_t* src, int16_t* dst, int32_t شمېر)
{ uprv_memcpy(dst, src, (size_t)شمېر * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int16_t* src, int32_t src_پیل,
              int16_t* dst, int32_t dst_پیل, int32_t شمېر)
{ uprv_memcpy(dst+dst_پیل, src+src_پیل, (size_t)شمېر * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int32_t* src, int32_t* dst, int32_t شمېر)
{ uprv_memcpy(dst, src, (size_t)شمېر * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int32_t* src, int32_t src_پیل,
              int32_t* dst, int32_t dst_پیل, int32_t شمېر)
{ uprv_memcpy(dst+dst_پیل, src+src_پیل, (size_t)شمېر * حجم(*src)); }

ساكن
مضمن فراغ
uprv_arrayCopy(ثابت char16_t *src, int32_t src_پیل,
        char16_t *dst, int32_t dst_پیل, int32_t شمېر)
{ uprv_memcpy(dst+dst_پیل, src+src_پیل, (size_t)شمېر * حجم(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ساكن مضمن فراغ
uprv_arrayCopy(ثابت icu::UnicodeString *src, icu::UnicodeString *dst, int32_t شمېر)
{ طالما(شمېر-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ساكن مضمن فراغ
uprv_arrayCopy(ثابت icu::UnicodeString *src, int32_t src_پیل,
               icu::UnicodeString *dst, int32_t dst_پیل, int32_t شمېر)
{ uprv_arrayCopy(src+src_پیل, dst+dst_پیل, شمېر); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
مضمن فراغ
uprv_check_Can_Get_بفر(ثابت icu::UnicodeString &s, UError_کوډ &error_کوډ) {
    إذا(U_SUCCESS(error_کوډ) && s.isBogus()) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
    }
}

#د_که_پای /* _CPPUTILS */
