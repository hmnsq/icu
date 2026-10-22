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

#यदि_अपरिभाषितम् CPPUTILS_H
#परिभाषय CPPUTILS_H

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

स्थिर
अन्तःस्थ शून्य uprv_arrayCopy(ध्रुव द्विगुण* src, द्विगुण* dst, int32_t गणनम्)
{ uprv_memcpy(dst, src, (size_t)गणनम् * परिमाण(*src)); }

स्थिर
अन्तःस्थ शून्य uprv_arrayCopy(ध्रुव द्विगुण* src, int32_t srcStart,
              द्विगुण* dst, int32_t dstStart, int32_t गणनम्)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)गणनम् * परिमाण(*src)); }

स्थिर
अन्तःस्थ शून्य uprv_arrayCopy(ध्रुव int8_t* src, int8_t* dst, int32_t गणनम्)
    { uprv_memcpy(dst, src, (size_t)गणनम् * परिमाण(*src)); }

स्थिर
अन्तःस्थ शून्य uprv_arrayCopy(ध्रुव int8_t* src, int32_t srcStart,
              int8_t* dst, int32_t dstStart, int32_t गणनम्)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)गणनम् * परिमाण(*src)); }

स्थिर
अन्तःस्थ शून्य uprv_arrayCopy(ध्रुव int16_t* src, int16_t* dst, int32_t गणनम्)
{ uprv_memcpy(dst, src, (size_t)गणनम् * परिमाण(*src)); }

स्थिर
अन्तःस्थ शून्य uprv_arrayCopy(ध्रुव int16_t* src, int32_t srcStart,
              int16_t* dst, int32_t dstStart, int32_t गणनम्)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)गणनम् * परिमाण(*src)); }

स्थिर
अन्तःस्थ शून्य uprv_arrayCopy(ध्रुव int32_t* src, int32_t* dst, int32_t गणनम्)
{ uprv_memcpy(dst, src, (size_t)गणनम् * परिमाण(*src)); }

स्थिर
अन्तःस्थ शून्य uprv_arrayCopy(ध्रुव int32_t* src, int32_t srcStart,
              int32_t* dst, int32_t dstStart, int32_t गणनम्)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)गणनम् * परिमाण(*src)); }

स्थिर
अन्तःस्थ शून्य
uprv_arrayCopy(ध्रुव char16_t *src, int32_t srcStart,
        char16_t *dst, int32_t dstStart, int32_t गणनम्)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)गणनम् * परिमाण(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
स्थिर अन्तःस्थ शून्य
uprv_arrayCopy(ध्रुव icu::UnicodeString *src, icu::UnicodeString *dst, int32_t गणनम्)
{ यावत्(गणनम्-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
स्थिर अन्तःस्थ शून्य
uprv_arrayCopy(ध्रुव icu::UnicodeString *src, int32_t srcStart,
               icu::UnicodeString *dst, int32_t dstStart, int32_t गणनम्)
{ uprv_arrayCopy(src+srcStart, dst+dstStart, गणनम्); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
अन्तःस्थ शून्य
uprv_check_Can_Get_बफर(ध्रुव icu::UnicodeString &s, UError_संहिता &error_संहिता) {
    यदि(U_SUCCESS(error_संहिता) && s.isBogus()) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
    }
}

#यदि_अन्तः /* _CPPUTILS */
