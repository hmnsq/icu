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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

ಸ್ಥಿರ
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ uprv_arrayCopy(ಧ್ರುವ ದ್ವಿಗುಣ* src, ದ್ವಿಗುಣ* dst, int32_t ಎಣಿಕೆ)
{ uprv_memcpy(dst, src, (size_t)ಎಣಿಕೆ * ಪರಿಮಾಣ(*src)); }

ಸ್ಥಿರ
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ uprv_arrayCopy(ಧ್ರುವ ದ್ವಿಗುಣ* src, int32_t srcStart,
              ದ್ವಿಗುಣ* dst, int32_t dstStart, int32_t ಎಣಿಕೆ)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ಎಣಿಕೆ * ಪರಿಮಾಣ(*src)); }

ಸ್ಥಿರ
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ uprv_arrayCopy(ಧ್ರುವ int8_t* src, int8_t* dst, int32_t ಎಣಿಕೆ)
    { uprv_memcpy(dst, src, (size_t)ಎಣಿಕೆ * ಪರಿಮಾಣ(*src)); }

ಸ್ಥಿರ
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ uprv_arrayCopy(ಧ್ರುವ int8_t* src, int32_t srcStart,
              int8_t* dst, int32_t dstStart, int32_t ಎಣಿಕೆ)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ಎಣಿಕೆ * ಪರಿಮಾಣ(*src)); }

ಸ್ಥಿರ
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ uprv_arrayCopy(ಧ್ರುವ int16_t* src, int16_t* dst, int32_t ಎಣಿಕೆ)
{ uprv_memcpy(dst, src, (size_t)ಎಣಿಕೆ * ಪರಿಮಾಣ(*src)); }

ಸ್ಥಿರ
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ uprv_arrayCopy(ಧ್ರುವ int16_t* src, int32_t srcStart,
              int16_t* dst, int32_t dstStart, int32_t ಎಣಿಕೆ)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ಎಣಿಕೆ * ಪರಿಮಾಣ(*src)); }

ಸ್ಥಿರ
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ uprv_arrayCopy(ಧ್ರುವ int32_t* src, int32_t* dst, int32_t ಎಣಿಕೆ)
{ uprv_memcpy(dst, src, (size_t)ಎಣಿಕೆ * ಪರಿಮಾಣ(*src)); }

ಸ್ಥಿರ
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ uprv_arrayCopy(ಧ್ರುವ int32_t* src, int32_t srcStart,
              int32_t* dst, int32_t dstStart, int32_t ಎಣಿಕೆ)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ಎಣಿಕೆ * ಪರಿಮಾಣ(*src)); }

ಸ್ಥಿರ
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ
uprv_arrayCopy(ಧ್ರುವ char16_t *src, int32_t srcStart,
        char16_t *dst, int32_t dstStart, int32_t ಎಣಿಕೆ)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ಎಣಿಕೆ * ಪರಿಮಾಣ(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ
uprv_arrayCopy(ಧ್ರುವ icu::UnicodeString *src, icu::UnicodeString *dst, int32_t ಎಣಿಕೆ)
{ ಯಾವತ್(ಎಣಿಕೆ-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ
uprv_arrayCopy(ಧ್ರುವ icu::UnicodeString *src, int32_t srcStart,
               icu::UnicodeString *dst, int32_t dstStart, int32_t ಎಣಿಕೆ)
{ uprv_arrayCopy(src+srcStart, dst+dstStart, ಎಣಿಕೆ); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ
uprv_checkCanGetBuffer(ಧ್ರುವ icu::UnicodeString &s, UErrorCode &errorCode) {
    ಯದಿ(U_SUCCESS(errorCode) && s.isBogus()) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

#endif /* _CPPUTILS */
