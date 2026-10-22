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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

સ્થિર
અંતઃરેખ શૂન્ય uprv_arrayCopy(અચલ દ્વિગુણ* src, દ્વિગુણ* dst, int32_t ગણતરી)
{ uprv_memcpy(dst, src, (size_t)ગણતરી * કદ(*src)); }

સ્થિર
અંતઃરેખ શૂન્ય uprv_arrayCopy(અચલ દ્વિગુણ* src, int32_t srcStart,
              દ્વિગુણ* dst, int32_t dstStart, int32_t ગણતરી)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ગણતરી * કદ(*src)); }

સ્થિર
અંતઃરેખ શૂન્ય uprv_arrayCopy(અચલ int8_t* src, int8_t* dst, int32_t ગણતરી)
    { uprv_memcpy(dst, src, (size_t)ગણતરી * કદ(*src)); }

સ્થિર
અંતઃરેખ શૂન્ય uprv_arrayCopy(અચલ int8_t* src, int32_t srcStart,
              int8_t* dst, int32_t dstStart, int32_t ગણતરી)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ગણતરી * કદ(*src)); }

સ્થિર
અંતઃરેખ શૂન્ય uprv_arrayCopy(અચલ int16_t* src, int16_t* dst, int32_t ગણતરી)
{ uprv_memcpy(dst, src, (size_t)ગણતરી * કદ(*src)); }

સ્થિર
અંતઃરેખ શૂન્ય uprv_arrayCopy(અચલ int16_t* src, int32_t srcStart,
              int16_t* dst, int32_t dstStart, int32_t ગણતરી)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ગણતરી * કદ(*src)); }

સ્થિર
અંતઃરેખ શૂન્ય uprv_arrayCopy(અચલ int32_t* src, int32_t* dst, int32_t ગણતરી)
{ uprv_memcpy(dst, src, (size_t)ગણતરી * કદ(*src)); }

સ્થિર
અંતઃરેખ શૂન્ય uprv_arrayCopy(અચલ int32_t* src, int32_t srcStart,
              int32_t* dst, int32_t dstStart, int32_t ગણતરી)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ગણતરી * કદ(*src)); }

સ્થિર
અંતઃરેખ શૂન્ય
uprv_arrayCopy(અચલ char16_t *src, int32_t srcStart,
        char16_t *dst, int32_t dstStart, int32_t ગણતરી)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)ગણતરી * કદ(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
સ્થિર અંતઃરેખ શૂન્ય
uprv_arrayCopy(અચલ icu::UnicodeString *src, icu::UnicodeString *dst, int32_t ગણતરી)
{ જ્યારે(ગણતરી-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
સ્થિર અંતઃરેખ શૂન્ય
uprv_arrayCopy(અચલ icu::UnicodeString *src, int32_t srcStart,
               icu::UnicodeString *dst, int32_t dstStart, int32_t ગણતરી)
{ uprv_arrayCopy(src+srcStart, dst+dstStart, ગણતરી); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
અંતઃરેખ શૂન્ય
uprv_checkCanGetBuffer(અચલ icu::UnicodeString &s, UErrorCode &errorCode) {
    જો(U_SUCCESS(errorCode) && s.isBogus()) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

#endif /* _CPPUTILS */
