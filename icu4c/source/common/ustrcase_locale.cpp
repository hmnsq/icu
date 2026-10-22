// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ustrcase_locale.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011may31
*   created by: Markus W. Scherer
*
*   Locale-sensitive case mapping functions (ones that call uloc_getDefault())
*   were moved here to break dependency cycles among parts of the common library.
*/

#समावेश "unicode/utypes.h"
#समावेश "uassert.h"
#समावेश "unicode/brkiter.h"
#समावेश "unicode/casemap.h"
#समावेश "unicode/ucasemap.h"
#समावेश "unicode/uloc.h"
#समावेश "unicode/ustring.h"
#समावेश "ucase.h"
#समावेश "ucasemap_imp.h"

U_CFUNC int32_t
ustrcase_getCaseLocale(स्थिर अक्षर *locale) {
    अगर (locale == nullptr) {
        locale = uloc_getDefault();
    }
    अगर (*locale == 0) {
        वापस UCASE_LOC_मूल;
    } अन्यथा {
        वापस ucase_getCaseLocale(locale);
    }
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strToLower(char16_t *dest, int32_t destCapacity,
             स्थिर char16_t *src, int32_t src_लंबाई,
             स्थिर अक्षर *locale,
             UErrorCode *pErrorCode) {
    वापस ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_लंबाई,
        ustrcase_internalToLower, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_strToUpper(char16_t *dest, int32_t destCapacity,
             स्थिर char16_t *src, int32_t src_लंबाई,
             स्थिर अक्षर *locale,
             UErrorCode *pErrorCode) {
    वापस ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_लंबाई,
        ustrcase_internalToUpper, *pErrorCode);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::toLower(
        स्थिर अक्षर *locale, uint32_t options,
        स्थिर char16_t *src, int32_t src_लंबाई,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    वापस ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_लंबाई,
        ustrcase_internalToLower, edits, errorCode);
}

int32_t CaseMap::toUpper(
        स्थिर अक्षर *locale, uint32_t options,
        स्थिर char16_t *src, int32_t src_लंबाई,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    वापस ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_लंबाई,
        ustrcase_internalToUpper, edits, errorCode);
}

U_NAMESPACE_END
