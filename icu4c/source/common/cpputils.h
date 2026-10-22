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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

स्थिर
ओळीत शून्य uprv_arrayCopy(स्थिरांक द्विगुण* src, द्विगुण* dst, int32_t मोजणी)
{ uprv_memcpy(dst, src, (माप_प्रकार)मोजणी * आकार(*src)); }

स्थिर
ओळीत शून्य uprv_arrayCopy(स्थिरांक द्विगुण* src, int32_t srcStart,
              द्विगुण* dst, int32_t dstStart, int32_t मोजणी)
{ uprv_memcpy(dst+dstStart, src+srcStart, (माप_प्रकार)मोजणी * आकार(*src)); }

स्थिर
ओळीत शून्य uprv_arrayCopy(स्थिरांक int8_t* src, int8_t* dst, int32_t मोजणी)
    { uprv_memcpy(dst, src, (माप_प्रकार)मोजणी * आकार(*src)); }

स्थिर
ओळीत शून्य uprv_arrayCopy(स्थिरांक int8_t* src, int32_t srcStart,
              int8_t* dst, int32_t dstStart, int32_t मोजणी)
{ uprv_memcpy(dst+dstStart, src+srcStart, (माप_प्रकार)मोजणी * आकार(*src)); }

स्थिर
ओळीत शून्य uprv_arrayCopy(स्थिरांक int16_t* src, int16_t* dst, int32_t मोजणी)
{ uprv_memcpy(dst, src, (माप_प्रकार)मोजणी * आकार(*src)); }

स्थिर
ओळीत शून्य uprv_arrayCopy(स्थिरांक int16_t* src, int32_t srcStart,
              int16_t* dst, int32_t dstStart, int32_t मोजणी)
{ uprv_memcpy(dst+dstStart, src+srcStart, (माप_प्रकार)मोजणी * आकार(*src)); }

स्थिर
ओळीत शून्य uprv_arrayCopy(स्थिरांक int32_t* src, int32_t* dst, int32_t मोजणी)
{ uprv_memcpy(dst, src, (माप_प्रकार)मोजणी * आकार(*src)); }

स्थिर
ओळीत शून्य uprv_arrayCopy(स्थिरांक int32_t* src, int32_t srcStart,
              int32_t* dst, int32_t dstStart, int32_t मोजणी)
{ uprv_memcpy(dst+dstStart, src+srcStart, (माप_प्रकार)मोजणी * आकार(*src)); }

स्थिर
ओळीत शून्य
uprv_arrayCopy(स्थिरांक char16_t *src, int32_t srcStart,
        char16_t *dst, int32_t dstStart, int32_t मोजणी)
{ uprv_memcpy(dst+dstStart, src+srcStart, (माप_प्रकार)मोजणी * आकार(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
स्थिर ओळीत शून्य
uprv_arrayCopy(स्थिरांक icu::UnicodeString *src, icu::UnicodeString *dst, int32_t मोजणी)
{ पर्यंत(मोजणी-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
स्थिर ओळीत शून्य
uprv_arrayCopy(स्थिरांक icu::UnicodeString *src, int32_t srcStart,
               icu::UnicodeString *dst, int32_t dstStart, int32_t मोजणी)
{ uprv_arrayCopy(src+srcStart, dst+dstStart, मोजणी); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
ओळीत शून्य
uprv_checkCanGetBuffer(स्थिरांक icu::UnicodeString &s, UErrorCode &errorCode) {
    जर(U_SUCCESS(errorCode) && s.isBogus()) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

#endif /* _CPPUTILS */
