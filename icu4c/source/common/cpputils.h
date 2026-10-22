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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "cmemory.h"

/*==========================================================================*/
/* Array copy utility functions */
/*==========================================================================*/

স্থির
অন্তঃস্থ শূন্য uprv_arrayCopy(ধ্রুব দ্বিগুণ* src, দ্বিগুণ* dst, int32_t গুনতি)
{ uprv_memcpy(dst, src, (size_t)গুনতি * পরিমাণ(*src)); }

স্থির
অন্তঃস্থ শূন্য uprv_arrayCopy(ধ্রুব দ্বিগুণ* src, int32_t srcStart,
              দ্বিগুণ* dst, int32_t dstStart, int32_t গুনতি)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)গুনতি * পরিমাণ(*src)); }

স্থির
অন্তঃস্থ শূন্য uprv_arrayCopy(ধ্রুব int8_t* src, int8_t* dst, int32_t গুনতি)
    { uprv_memcpy(dst, src, (size_t)গুনতি * পরিমাণ(*src)); }

স্থির
অন্তঃস্থ শূন্য uprv_arrayCopy(ধ্রুব int8_t* src, int32_t srcStart,
              int8_t* dst, int32_t dstStart, int32_t গুনতি)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)গুনতি * পরিমাণ(*src)); }

স্থির
অন্তঃস্থ শূন্য uprv_arrayCopy(ধ্রুব int16_t* src, int16_t* dst, int32_t গুনতি)
{ uprv_memcpy(dst, src, (size_t)গুনতি * পরিমাণ(*src)); }

স্থির
অন্তঃস্থ শূন্য uprv_arrayCopy(ধ্রুব int16_t* src, int32_t srcStart,
              int16_t* dst, int32_t dstStart, int32_t গুনতি)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)গুনতি * পরিমাণ(*src)); }

স্থির
অন্তঃস্থ শূন্য uprv_arrayCopy(ধ্রুব int32_t* src, int32_t* dst, int32_t গুনতি)
{ uprv_memcpy(dst, src, (size_t)গুনতি * পরিমাণ(*src)); }

স্থির
অন্তঃস্থ শূন্য uprv_arrayCopy(ধ্রুব int32_t* src, int32_t srcStart,
              int32_t* dst, int32_t dstStart, int32_t গুনতি)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)গুনতি * পরিমাণ(*src)); }

স্থির
অন্তঃস্থ শূন্য
uprv_arrayCopy(ধ্রুব char16_t *src, int32_t srcStart,
        char16_t *dst, int32_t dstStart, int32_t গুনতি)
{ uprv_memcpy(dst+dstStart, src+srcStart, (size_t)গুনতি * পরিমাণ(*src)); }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
স্থির অন্তঃস্থ শূন্য
uprv_arrayCopy(ধ্রুব icu::UnicodeString *src, icu::UnicodeString *dst, int32_t গুনতি)
{ যাবত্(গুনতি-- > 0) *dst++ = *src++; }

/**
 * Copy an array of UnicodeString OBJECTS (not pointers).
 * @internal
 */
স্থির অন্তঃস্থ শূন্য
uprv_arrayCopy(ধ্রুব icu::UnicodeString *src, int32_t srcStart,
               icu::UnicodeString *dst, int32_t dstStart, int32_t গুনতি)
{ uprv_arrayCopy(src+srcStart, dst+dstStart, গুনতি); }

/**
 * Checks that the string is readable and writable.
 * Sets U_ILLEGAL_ARGUMENT_ERROR if the string isBogus() or has an open getBuffer().
 */
অন্তঃস্থ শূন্য
uprv_checkCanGetBuffer(ধ্রুব icu::UnicodeString &s, UErrorCode &errorCode) {
    যদি(U_SUCCESS(errorCode) && s.isBogus()) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

#endif /* _CPPUTILS */
