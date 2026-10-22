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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "unicode/brkiter.h"
#অন্তর্ভাব "unicode/casemap.h"
#অন্তর্ভাব "unicode/ucasemap.h"
#অন্তর্ভাব "unicode/uloc.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "ucase.h"
#অন্তর্ভাব "ucasemap_imp.h"

U_CFUNC int32_t
ustrcase_getCaseLocale(ধ্রুবক অক্ষর *locale) {
    যদি (locale == nullptr) {
        locale = uloc_getDefault();
    }
    যদি (*locale == 0) {
        ফেরত UCASE_LOC_মূল;
    } অন্যথা {
        ফেরত ucase_getCaseLocale(locale);
    }
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strToLower(char16_t *dest, int32_t destCapacity,
             ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
             ধ্রুবক অক্ষর *locale,
             UErrorCode *pErrorCode) {
    ফেরত ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ustrcase_internalToLower, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_strToUpper(char16_t *dest, int32_t destCapacity,
             ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
             ধ্রুবক অক্ষর *locale,
             UErrorCode *pErrorCode) {
    ফেরত ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ustrcase_internalToUpper, *pErrorCode);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::toLower(
        ধ্রুবক অক্ষর *locale, uint32_t options,
        ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ফেরত ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ustrcase_internalToLower, edits, errorCode);
}

int32_t CaseMap::toUpper(
        ধ্রুবক অক্ষর *locale, uint32_t options,
        ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ফেরত ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ustrcase_internalToUpper, edits, errorCode);
}

U_NAMESPACE_END
