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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "unicode/brkiter.h"
#அந்தர்பாவ "unicode/casemap.h"
#அந்தர்பாவ "unicode/ucasemap.h"
#அந்தர்பாவ "unicode/uloc.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "ucase.h"
#அந்தர்பாவ "ucasemap_imp.h"

U_CFUNC int32_t
ustrcase_getCaseLocale(மாறிலி எழுத்து *locale) {
    எனில் (locale == nullptr) {
        locale = uloc_getDefault();
    }
    எனில் (*locale == 0) {
        திரும்பு UCASE_LOC_வேர்;
    } இல்லையெனில் {
        திரும்பு ucase_getCaseLocale(locale);
    }
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strToLower(char16_t *dest, int32_t destCapacity,
             மாறிலி char16_t *src, int32_t srcLength,
             மாறிலி எழுத்து *locale,
             UErrorCode *pErrorCode) {
    திரும்பு ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToLower, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_strToUpper(char16_t *dest, int32_t destCapacity,
             மாறிலி char16_t *src, int32_t srcLength,
             மாறிலி எழுத்து *locale,
             UErrorCode *pErrorCode) {
    திரும்பு ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToUpper, *pErrorCode);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::toLower(
        மாறிலி எழுத்து *locale, uint32_t options,
        மாறிலி char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    திரும்பு ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToLower, edits, errorCode);
}

int32_t CaseMap::toUpper(
        மாறிலி எழுத்து *locale, uint32_t options,
        மாறிலி char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    திரும்பு ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToUpper, edits, errorCode);
}

U_NAMESPACE_END
