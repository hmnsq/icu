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

#अगर_अघोषित CPPUTILS_H
#घोषणा CPPUTILS_H

#समावेश "unicode/utypes.h"
#समावेश "unicode/unistr.h"
#समावेश "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

अटल
अंतरभूत व्योम uprv_arrayCopy(स्थिर द्विगुन* src, द्विगुन* dst, int32_t गिनती)
{ uprv_memcpy(dst, src, (माप_प्रकार)गिनती * माप(*src)); }

अटल
अंतरभूत व्योम uprv_arrayCopy(स्थिर द्विगुन* src, int32_t srcStart,
              द्विगुन* dst, int32_t dstStart, int32_t गिनती)
{ uprv_memcpy(dst+dstStart, src+srcStart, (माप_प्रकार)गिनती * माप(*src)); }

अटल
अंतरभूत व्योम uprv_arrayCopy(स्थिर int8_t* src, int8_t* dst, int32_t गिनती)
    { uprv_memcpy(dst, src, (माप_प्रकार)गिनती * माप(*src)); }

अटल
अंतरभूत व्योम uprv_arrayCopy(स्थिर int8_t* src, int32_t srcStart,
              int8_t* dst, int32_t dstStart, int32_t गिनती)
{ uprv_memcpy(dst+dstStart, src+srcStart, (माप_प्रकार)गिनती * माप(*src)); }

अटल
अंतरभूत व्योम uprv_arrayCopy(स्थिर int16_t* src, int16_t* dst, int32_t गिनती)
{ uprv_memcpy(dst, src, (माप_प्रकार)गिनती * माप(*src)); }

अटल
अंतरभूत व्योम uprv_arrayCopy(स्थिर int16_t* src, int32_t srcStart,
              int16_t* dst, int32_t dstStart, int32_t गिनती)
{ uprv_memcpy(dst+dstStart, src+srcStart, (माप_प्रकार)गिनती * माप(*src)); }

अटल
अंतरभूत व्योम uprv_arrayCopy(स्थिर int32_t* src, int32_t* dst, int32_t गिनती)
{ uprv_memcpy(dst, src, (माप_प्रकार)गिनती * माप(*src)); }

अटल
अंतरभूत व्योम uprv_arrayCopy(स्थिर int32_t* src, int32_t srcStart,
              int32_t* dst, int32_t dstStart, int32_t गिनती)
{ uprv_memcpy(dst+dstStart, src+srcStart, (माप_प्रकार)गिनती * माप(*src)); }

अटल
अंतरभूत व्योम
uprv_arrayCopy(स्थिर char16_t *src, int32_t srcStart,
        char16_t *dst, int32_t dstStart, int32_t गिनती)
{ uprv_memcpy(dst+dstStart, src+srcStart, (माप_प्रकार)गिनती * माप(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
अटल अंतरभूत व्योम
uprv_arrayCopy(स्थिर icu::UnicodeString *src, icu::UnicodeString *dst, int32_t गिनती)
{ जबतक(गिनती-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
अटल अंतरभूत व्योम
uprv_arrayCopy(स्थिर icu::UnicodeString *src, int32_t srcStart,
               icu::UnicodeString *dst, int32_t dstStart, int32_t गिनती)
{ uprv_arrayCopy(src+srcStart, dst+dstStart, गिनती); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
अंतरभूत व्योम
uprv_check_Can_Get_बफर(स्थिर icu::UnicodeString &s, UErrorCode &errorCode) {
    अगर(U_SUCCESS(errorCode) && s.isBogus()) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

#पूर्ण_अगर /* _CPPUTILS */
