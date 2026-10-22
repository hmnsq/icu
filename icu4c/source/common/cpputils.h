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

#ifndef CPPUTILS_H
#define CPPUTILS_H

#شامل "unicode/utypes.h"
#شامل "unicode/unistr.h"
#شامل "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

ایستا
درونخطی تهی uprv_arrayCopy(ثابت دوگانه* src, دوگانه* dst, int32_t شمار)
{ uprv_memcpy(dst, src, (size_t)شمار * اندازه(*src)); }

ایستا
درونخطی تهی uprv_arrayCopy(ثابت دوگانه* src, int32_t srcStart,
              دوگانه* dst, int32_t dstStart, int32_t شمار)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)شمار * اندازه(*src)); }

ایستا
درونخطی تهی uprv_arrayCopy(ثابت int8_t* src, int8_t* dst, int32_t شمار)
    { uprv_memcpy(dst, src, (size_t)شمار * اندازه(*src)); }

ایستا
درونخطی تهی uprv_arrayCopy(ثابت int8_t* src, int32_t srcStart,
              int8_t* dst, int32_t dstStart, int32_t شمار)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)شمار * اندازه(*src)); }

ایستا
درونخطی تهی uprv_arrayCopy(ثابت int16_t* src, int16_t* dst, int32_t شمار)
{ uprv_memcpy(dst, src, (size_t)شمار * اندازه(*src)); }

ایستا
درونخطی تهی uprv_arrayCopy(ثابت int16_t* src, int32_t srcStart,
              int16_t* dst, int32_t dstStart, int32_t شمار)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)شمار * اندازه(*src)); }

ایستا
درونخطی تهی uprv_arrayCopy(ثابت int32_t* src, int32_t* dst, int32_t شمار)
{ uprv_memcpy(dst, src, (size_t)شمار * اندازه(*src)); }

ایستا
درونخطی تهی uprv_arrayCopy(ثابت int32_t* src, int32_t srcStart,
              int32_t* dst, int32_t dstStart, int32_t شمار)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)شمار * اندازه(*src)); }

ایستا
درونخطی تهی
uprv_arrayCopy(ثابت char16_t *src, int32_t srcStart,
        char16_t *dst, int32_t dstStart, int32_t شمار)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)شمار * اندازه(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ایستا درونخطی تهی
uprv_arrayCopy(ثابت icu::UnicodeString *src, icu::UnicodeString *dst, int32_t شمار)
{ وقتی(شمار-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ایستا درونخطی تهی
uprv_arrayCopy(ثابت icu::UnicodeString *src, int32_t srcStart,
               icu::UnicodeString *dst, int32_t dstStart, int32_t شمار)
{ uprv_arrayCopy(src+srcStart, dst+dstStart, شمار); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
درونخطی تهی
uprv_checkCanGetBuffer(ثابت icu::UnicodeString &s, UErrorCode &errorCode) {
    اگر(U_SUCCESS(errorCode) && s.isBogus()) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

#endif /* _CPPUTILS */
