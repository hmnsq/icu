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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/casemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucasemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucasemap_imp.h"

U_CFUNC int32_t
ustrcase_getCaseLocale(ਅਚਲ ਅੱਖਰ *locale) {
    ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) {
        locale = uloc_getDefault();
    }
    ਜੇ (*locale == 0) {
        ਵਾਪਸ UCASE_LOC_ਜੜ੍ਹ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ucase_getCaseLocale(locale);
    }
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_str_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
             ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
             ਅਚਲ ਅੱਖਰ *locale,
             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
u_str_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
             ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
             ਅਚਲ ਅੱਖਰ *locale,
             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ, *p_Error_ਕੋਡ);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(
        ਅਚਲ ਅੱਖਰ *locale, uint32_t options,
        ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
        char16_t *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ਵਾਪਸ ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ, edits, error_ਕੋਡ);
}

int32_t CaseMap::ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(
        ਅਚਲ ਅੱਖਰ *locale, uint32_t options,
        ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
        char16_t *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ਵਾਪਸ ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ, edits, error_ਕੋਡ);
}

U_NAMESPACE_ਸਿਰਾ
