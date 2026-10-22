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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/casemap.h"
#അന്തര്ഭാവ "unicode/ucasemap.h"
#അന്തര്ഭാവ "unicode/uloc.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "ucase.h"
#അന്തര്ഭാവ "ucasemap_imp.h"

U_CFUNC int32_t
ustrcase_getCaseLocale(സ്ഥിരം അക്ഷരം *locale) {
    എങ്കിൽ (locale == nullptr) {
        locale = uloc_getDefault();
    }
    എങ്കിൽ (*locale == 0) {
        തിരികെ UCASE_LOC_വേര്;
    } അല്ലെങ്കിൽ {
        തിരികെ ucase_getCaseLocale(locale);
    }
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strToLower(char16_t *dest, int32_t destCapacity,
             സ്ഥിരം char16_t *src, int32_t srcLength,
             സ്ഥിരം അക്ഷരം *locale,
             UErrorCode *pErrorCode) {
    തിരികെ ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToLower, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_strToUpper(char16_t *dest, int32_t destCapacity,
             സ്ഥിരം char16_t *src, int32_t srcLength,
             സ്ഥിരം അക്ഷരം *locale,
             UErrorCode *pErrorCode) {
    തിരികെ ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToUpper, *pErrorCode);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::toLower(
        സ്ഥിരം അക്ഷരം *locale, uint32_t options,
        സ്ഥിരം char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    തിരികെ ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToLower, edits, errorCode);
}

int32_t CaseMap::toUpper(
        സ്ഥിരം അക്ഷരം *locale, uint32_t options,
        സ്ഥിരം char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    തിരികെ ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalToUpper, edits, errorCode);
}

U_NAMESPACE_END
