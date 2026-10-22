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

#ضمن "unicode/utypes.h"
#ضمن "uassert.h"
#ضمن "unicode/brkiter.h"
#ضمن "unicode/casemap.h"
#ضمن "unicode/ucasemap.h"
#ضمن "unicode/uloc.h"
#ضمن "unicode/ustring.h"
#ضمن "ucase.h"
#ضمن "ucasemap_imp.h"

U_CFUNC int32_t
ustrcase_getCaseLocale(ثابت حرف *locale) {
    إذا (locale == خالی_سوچک) {
        locale = uloc_getDefault();
    }
    إذا (*locale == 0) {
        أرجع UCASE_LOC_جڑ;
    } وإلا {
        أرجع ucase_getCaseLocale(locale);
    }
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_str_منزل_ہیٹھلا(char16_t *dest, int32_t dest_سمرتھا,
             ثابت char16_t *src, int32_t src_لمائی,
             ثابت حرف *locale,
             UError_کوڈ *p_Error_کوڈ) {
    أرجع ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_سمرتھا,
        src, src_لمائی,
        ustrcase_اندرونی_منزل_ہیٹھلا, *p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
u_str_منزل_اتلا(char16_t *dest, int32_t dest_سمرتھا,
             ثابت char16_t *src, int32_t src_لمائی,
             ثابت حرف *locale,
             UError_کوڈ *p_Error_کوڈ) {
    أرجع ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_سمرتھا,
        src, src_لمائی,
        ustrcase_اندرونی_منزل_اتلا, *p_Error_کوڈ);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::منزل_ہیٹھلا(
        ثابت حرف *locale, uint32_t options,
        ثابت char16_t *src, int32_t src_لمائی,
        char16_t *dest, int32_t dest_سمرتھا, Edits *edits,
        UError_کوڈ &error_کوڈ) {
    أرجع ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_سمرتھا,
        src, src_لمائی,
        ustrcase_اندرونی_منزل_ہیٹھلا, edits, error_کوڈ);
}

int32_t CaseMap::منزل_اتلا(
        ثابت حرف *locale, uint32_t options,
        ثابت char16_t *src, int32_t src_لمائی,
        char16_t *dest, int32_t dest_سمرتھا, Edits *edits,
        UError_کوڈ &error_کوڈ) {
    أرجع ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_سمرتھا,
        src, src_لمائی,
        ustrcase_اندرونی_منزل_اتلا, edits, error_کوڈ);
}

U_NAMESPACE_سرا
