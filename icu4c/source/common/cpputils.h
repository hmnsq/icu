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

#include "unicode/utypes.h"
#include "unicode/unistr.h"
#include "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

static
anta_hstha void uprv_arrayCopy(const dviguu_na* src, dviguu_na* dst, int32_t count)
{ uprv_memcpy(dst, src, (size_t)count * parimaa_na(*src)); }

static
anta_hstha void uprv_arrayCopy(const dviguu_na* src, int32_t srcStart,
              dviguu_na* dst, int32_t dstStart, int32_t count)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)count * parimaa_na(*src)); }

static
anta_hstha void uprv_arrayCopy(const int8_t* src, int8_t* dst, int32_t count)
    { uprv_memcpy(dst, src, (size_t)count * parimaa_na(*src)); }

static
anta_hstha void uprv_arrayCopy(const int8_t* src, int32_t srcStart,
              int8_t* dst, int32_t dstStart, int32_t count)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)count * parimaa_na(*src)); }

static
anta_hstha void uprv_arrayCopy(const int16_t* src, int16_t* dst, int32_t count)
{ uprv_memcpy(dst, src, (size_t)count * parimaa_na(*src)); }

static
anta_hstha void uprv_arrayCopy(const int16_t* src, int32_t srcStart,
              int16_t* dst, int32_t dstStart, int32_t count)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)count * parimaa_na(*src)); }

static
anta_hstha void uprv_arrayCopy(const int32_t* src, int32_t* dst, int32_t count)
{ uprv_memcpy(dst, src, (size_t)count * parimaa_na(*src)); }

static
anta_hstha void uprv_arrayCopy(const int32_t* src, int32_t srcStart,
              int32_t* dst, int32_t dstStart, int32_t count)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)count * parimaa_na(*src)); }

static
anta_hstha void
uprv_arrayCopy(const char16_t *src, int32_t srcStart,
        char16_t *dst, int32_t dstStart, int32_t count)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)count * parimaa_na(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
static anta_hstha void
uprv_arrayCopy(const icu::UnicodeString *src, icu::UnicodeString *dst, int32_t count)
{ ᬲᭂᬤᭂᬮ(count-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
static anta_hstha void
uprv_arrayCopy(const icu::UnicodeString *src, int32_t srcStart,
               icu::UnicodeString *dst, int32_t dstStart, int32_t count)
{ uprv_arrayCopy(src+srcStart, dst+dstStart, count); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
anta_hstha void
uprv_checkCanGetBuffer(const icu::UnicodeString &s, UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_SUCCESS(errorCode) && s.isBogus()) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

#endif /* _CPPUTILS */
