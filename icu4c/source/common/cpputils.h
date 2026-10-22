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

#جيڪڏهن_وصف_نه_ٿيل CPPUTILS_H
#وصف_ڪريو CPPUTILS_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/unistr.h"
#ضمن "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

ساكن
مضمن فراغ uprv_arrayCopy(ثابت مزدوج* src, مزدوج* dst, int32_t ڳڻپ)
{ uprv_memcpy(dst, src, (size_t)ڳڻپ * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت مزدوج* src, int32_t src_شروعات,
              مزدوج* dst, int32_t dst_شروعات, int32_t ڳڻپ)
{ uprv_memcpy(dst+dst_شروعات, src+src_شروعات, (size_t)ڳڻپ * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int8_t* src, int8_t* dst, int32_t ڳڻپ)
    { uprv_memcpy(dst, src, (size_t)ڳڻپ * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int8_t* src, int32_t src_شروعات,
              int8_t* dst, int32_t dst_شروعات, int32_t ڳڻپ)
{ uprv_memcpy(dst+dst_شروعات, src+src_شروعات, (size_t)ڳڻپ * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int16_t* src, int16_t* dst, int32_t ڳڻپ)
{ uprv_memcpy(dst, src, (size_t)ڳڻپ * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int16_t* src, int32_t src_شروعات,
              int16_t* dst, int32_t dst_شروعات, int32_t ڳڻپ)
{ uprv_memcpy(dst+dst_شروعات, src+src_شروعات, (size_t)ڳڻپ * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int32_t* src, int32_t* dst, int32_t ڳڻپ)
{ uprv_memcpy(dst, src, (size_t)ڳڻپ * حجم(*src)); }

ساكن
مضمن فراغ uprv_arrayCopy(ثابت int32_t* src, int32_t src_شروعات,
              int32_t* dst, int32_t dst_شروعات, int32_t ڳڻپ)
{ uprv_memcpy(dst+dst_شروعات, src+src_شروعات, (size_t)ڳڻپ * حجم(*src)); }

ساكن
مضمن فراغ
uprv_arrayCopy(ثابت char16_t *src, int32_t src_شروعات,
        char16_t *dst, int32_t dst_شروعات, int32_t ڳڻپ)
{ uprv_memcpy(dst+dst_شروعات, src+src_شروعات, (size_t)ڳڻپ * حجم(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ساكن مضمن فراغ
uprv_arrayCopy(ثابت icu::UnicodeString *src, icu::UnicodeString *dst, int32_t ڳڻپ)
{ طالما(ڳڻپ-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ساكن مضمن فراغ
uprv_arrayCopy(ثابت icu::UnicodeString *src, int32_t src_شروعات,
               icu::UnicodeString *dst, int32_t dst_شروعات, int32_t ڳڻپ)
{ uprv_arrayCopy(src+src_شروعات, dst+dst_شروعات, ڳڻپ); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
مضمن فراغ
uprv_check_Can_Get_بفر(ثابت icu::UnicodeString &s, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_SUCCESS(error_ڪوڊ) && s.isBogus()) {
        error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
    }
}

#جيڪڏهن_پورو /* _CPPUTILS */
