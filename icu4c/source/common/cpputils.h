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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

ସ୍ଥିର
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ uprv_arrayCopy(ଅଚଳ ଦ୍ୱିଗୁଣ* src, ଦ୍ୱିଗୁଣ* dst, int32_t ଗଣନା_2)
{ uprv_memcpy(dst, src, (size_t)ଗଣନା_2 * ଆକାର(*src)); }

ସ୍ଥିର
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ uprv_arrayCopy(ଅଚଳ ଦ୍ୱିଗୁଣ* src, int32_t srcStart,
              ଦ୍ୱିଗୁଣ* dst, int32_t dstStart, int32_t ଗଣନା_2)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ଗଣନା_2 * ଆକାର(*src)); }

ସ୍ଥିର
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ uprv_arrayCopy(ଅଚଳ int8_t* src, int8_t* dst, int32_t ଗଣନା_2)
    { uprv_memcpy(dst, src, (size_t)ଗଣନା_2 * ଆକାର(*src)); }

ସ୍ଥିର
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ uprv_arrayCopy(ଅଚଳ int8_t* src, int32_t srcStart,
              int8_t* dst, int32_t dstStart, int32_t ଗଣନା_2)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ଗଣନା_2 * ଆକାର(*src)); }

ସ୍ଥିର
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ uprv_arrayCopy(ଅଚଳ int16_t* src, int16_t* dst, int32_t ଗଣନା_2)
{ uprv_memcpy(dst, src, (size_t)ଗଣନା_2 * ଆକାର(*src)); }

ସ୍ଥିର
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ uprv_arrayCopy(ଅଚଳ int16_t* src, int32_t srcStart,
              int16_t* dst, int32_t dstStart, int32_t ଗଣନା_2)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ଗଣନା_2 * ଆକାର(*src)); }

ସ୍ଥିର
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ uprv_arrayCopy(ଅଚଳ int32_t* src, int32_t* dst, int32_t ଗଣନା_2)
{ uprv_memcpy(dst, src, (size_t)ଗଣନା_2 * ଆକାର(*src)); }

ସ୍ଥିର
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ uprv_arrayCopy(ଅଚଳ int32_t* src, int32_t srcStart,
              int32_t* dst, int32_t dstStart, int32_t ଗଣନା_2)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ଗଣନା_2 * ଆକାର(*src)); }

ସ୍ଥିର
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
uprv_arrayCopy(ଅଚଳ char16_t *src, int32_t srcStart,
        char16_t *dst, int32_t dstStart, int32_t ଗଣନା_2)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ଗଣନା_2 * ଆକାର(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
uprv_arrayCopy(ଅଚଳ icu::UnicodeString *src, icu::UnicodeString *dst, int32_t ଗଣନା_2)
{ ଯେତେବେଳେ(ଗଣନା_2-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
uprv_arrayCopy(ଅଚଳ icu::UnicodeString *src, int32_t srcStart,
               icu::UnicodeString *dst, int32_t dstStart, int32_t ଗଣନା_2)
{ uprv_arrayCopy(src+srcStart, dst+dstStart, ଗଣନା_2); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
uprv_checkCanGetBuffer(ଅଚଳ icu::UnicodeString &s, UErrorCode &errorCode) {
    ଯଦି(U_SUCCESS(errorCode) && s.isBogus()) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

#endif /* _CPPUTILS */
