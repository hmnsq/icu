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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

സ്ഥിതം
വരിയിൽ ശൂന്യം uprv_arrayCopy(സ്ഥിരം ഇരട്ട* src, ഇരട്ട* dst, int32_t എണ്ണം)
{ uprv_memcpy(dst, src, (size_t)എണ്ണം * വലുപ്പം(*src)); }

സ്ഥിതം
വരിയിൽ ശൂന്യം uprv_arrayCopy(സ്ഥിരം ഇരട്ട* src, int32_t srcStart,
              ഇരട്ട* dst, int32_t dstStart, int32_t എണ്ണം)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)എണ്ണം * വലുപ്പം(*src)); }

സ്ഥിതം
വരിയിൽ ശൂന്യം uprv_arrayCopy(സ്ഥിരം int8_t* src, int8_t* dst, int32_t എണ്ണം)
    { uprv_memcpy(dst, src, (size_t)എണ്ണം * വലുപ്പം(*src)); }

സ്ഥിതം
വരിയിൽ ശൂന്യം uprv_arrayCopy(സ്ഥിരം int8_t* src, int32_t srcStart,
              int8_t* dst, int32_t dstStart, int32_t എണ്ണം)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)എണ്ണം * വലുപ്പം(*src)); }

സ്ഥിതം
വരിയിൽ ശൂന്യം uprv_arrayCopy(സ്ഥിരം int16_t* src, int16_t* dst, int32_t എണ്ണം)
{ uprv_memcpy(dst, src, (size_t)എണ്ണം * വലുപ്പം(*src)); }

സ്ഥിതം
വരിയിൽ ശൂന്യം uprv_arrayCopy(സ്ഥിരം int16_t* src, int32_t srcStart,
              int16_t* dst, int32_t dstStart, int32_t എണ്ണം)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)എണ്ണം * വലുപ്പം(*src)); }

സ്ഥിതം
വരിയിൽ ശൂന്യം uprv_arrayCopy(സ്ഥിരം int32_t* src, int32_t* dst, int32_t എണ്ണം)
{ uprv_memcpy(dst, src, (size_t)എണ്ണം * വലുപ്പം(*src)); }

സ്ഥിതം
വരിയിൽ ശൂന്യം uprv_arrayCopy(സ്ഥിരം int32_t* src, int32_t srcStart,
              int32_t* dst, int32_t dstStart, int32_t എണ്ണം)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)എണ്ണം * വലുപ്പം(*src)); }

സ്ഥിതം
വരിയിൽ ശൂന്യം
uprv_arrayCopy(സ്ഥിരം char16_t *src, int32_t srcStart,
        char16_t *dst, int32_t dstStart, int32_t എണ്ണം)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)എണ്ണം * വലുപ്പം(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
സ്ഥിതം വരിയിൽ ശൂന്യം
uprv_arrayCopy(സ്ഥിരം icu::UnicodeString *src, icu::UnicodeString *dst, int32_t എണ്ണം)
{ ആകുമ്പോൾ(എണ്ണം-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
സ്ഥിതം വരിയിൽ ശൂന്യം
uprv_arrayCopy(സ്ഥിരം icu::UnicodeString *src, int32_t srcStart,
               icu::UnicodeString *dst, int32_t dstStart, int32_t എണ്ണം)
{ uprv_arrayCopy(src+srcStart, dst+dstStart, എണ്ണം); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
വരിയിൽ ശൂന്യം
uprv_checkCanGetBuffer(സ്ഥിരം icu::UnicodeString &s, UErrorCode &errorCode) {
    എങ്കിൽ(U_SUCCESS(errorCode) && s.isBogus()) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

#endif /* _CPPUTILS */
