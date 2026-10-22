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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

நிலையான
உள்வரி வெறுமை uprv_arrayCopy(மாறிலி இரட்டை* src, இரட்டை* dst, int32_t மொத்தம்)
{ uprv_memcpy(dst, src, (size_t)மொத்தம் * அளவு(*src)); }

நிலையான
உள்வரி வெறுமை uprv_arrayCopy(மாறிலி இரட்டை* src, int32_t srcStart,
              இரட்டை* dst, int32_t dstStart, int32_t மொத்தம்)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)மொத்தம் * அளவு(*src)); }

நிலையான
உள்வரி வெறுமை uprv_arrayCopy(மாறிலி int8_t* src, int8_t* dst, int32_t மொத்தம்)
    { uprv_memcpy(dst, src, (size_t)மொத்தம் * அளவு(*src)); }

நிலையான
உள்வரி வெறுமை uprv_arrayCopy(மாறிலி int8_t* src, int32_t srcStart,
              int8_t* dst, int32_t dstStart, int32_t மொத்தம்)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)மொத்தம் * அளவு(*src)); }

நிலையான
உள்வரி வெறுமை uprv_arrayCopy(மாறிலி int16_t* src, int16_t* dst, int32_t மொத்தம்)
{ uprv_memcpy(dst, src, (size_t)மொத்தம் * அளவு(*src)); }

நிலையான
உள்வரி வெறுமை uprv_arrayCopy(மாறிலி int16_t* src, int32_t srcStart,
              int16_t* dst, int32_t dstStart, int32_t மொத்தம்)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)மொத்தம் * அளவு(*src)); }

நிலையான
உள்வரி வெறுமை uprv_arrayCopy(மாறிலி int32_t* src, int32_t* dst, int32_t மொத்தம்)
{ uprv_memcpy(dst, src, (size_t)மொத்தம் * அளவு(*src)); }

நிலையான
உள்வரி வெறுமை uprv_arrayCopy(மாறிலி int32_t* src, int32_t srcStart,
              int32_t* dst, int32_t dstStart, int32_t மொத்தம்)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)மொத்தம் * அளவு(*src)); }

நிலையான
உள்வரி வெறுமை
uprv_arrayCopy(மாறிலி char16_t *src, int32_t srcStart,
        char16_t *dst, int32_t dstStart, int32_t மொத்தம்)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)மொத்தம் * அளவு(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
நிலையான உள்வரி வெறுமை
uprv_arrayCopy(மாறிலி icu::UnicodeString *src, icu::UnicodeString *dst, int32_t மொத்தம்)
{ போது(மொத்தம்-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
நிலையான உள்வரி வெறுமை
uprv_arrayCopy(மாறிலி icu::UnicodeString *src, int32_t srcStart,
               icu::UnicodeString *dst, int32_t dstStart, int32_t மொத்தம்)
{ uprv_arrayCopy(src+srcStart, dst+dstStart, மொத்தம்); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
உள்வரி வெறுமை
uprv_checkCanGetBuffer(மாறிலி icu::UnicodeString &s, UErrorCode &errorCode) {
    எனில்(U_SUCCESS(errorCode) && s.isBogus()) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

#endif /* _CPPUTILS */
