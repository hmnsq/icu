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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "unicode/brkiter.h"
#অন্তর্ভাৱ "unicode/casemap.h"
#অন্তর্ভাৱ "unicode/ucasemap.h"
#অন্তর্ভাৱ "unicode/uloc.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "ucase.h"
#অন্তর্ভাৱ "ucasemap_imp.h"

U_CFUNC int32_t
ustrcase_getCaseLocale(ধ্রুৱ ৱর্ণ *locale) {
    যদি (locale == nullptr) {
        locale = uloc_getDefault();
    }
    যদি (*locale == 0) {
        ঘূৰাই UCASE_LOC_মূল;
    } অন্যথা {
        ঘূৰাই ucase_getCaseLocale(locale);
    }
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strToLower(char16_t *dest, int32_t destCapacity,
             ধ্রুৱ char16_t *src, int32_t srcLength,
             ধ্রুৱ ৱর্ণ *locale,
             UErrorCode *pErrorCode) {
    ঘূৰাই ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToLower, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_strToUpper(char16_t *dest, int32_t destCapacity,
             ধ্রুৱ char16_t *src, int32_t srcLength,
             ধ্রুৱ ৱর্ণ *locale,
             UErrorCode *pErrorCode) {
    ঘূৰাই ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToUpper, *pErrorCode);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::toLower(
        ধ্রুৱ ৱর্ণ *locale, uint32_t options,
        ধ্রুৱ char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ঘূৰাই ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToLower, edits, errorCode);
}

int32_t CaseMap::toUpper(
        ধ্রুৱ ৱর্ণ *locale, uint32_t options,
        ধ্রুৱ char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ঘূৰাই ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToUpper, edits, errorCode);
}

U_NAMESPACE_END
