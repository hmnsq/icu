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
    إذا (locale == تش_اشاره) {
        locale = uloc_getDefault();
    }
    إذا (*locale == 0) {
        أرجع UCASE_LOC_ریښه;
    } وإلا {
        أرجع ucase_getCaseLocale(locale);
    }
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_str_منزل_ښکتنی(char16_t *dest, int32_t dest_ظرفیت,
             ثابت char16_t *src, int32_t src_اوږدتیا,
             ثابت حرف *locale,
             UError_کوډ *p_Error_کوډ) {
    أرجع ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ustrcase_دننی_منزل_ښکتنی, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
u_str_منزل_پاسنی(char16_t *dest, int32_t dest_ظرفیت,
             ثابت char16_t *src, int32_t src_اوږدتیا,
             ثابت حرف *locale,
             UError_کوډ *p_Error_کوډ) {
    أرجع ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ustrcase_دننی_منزل_پاسنی, *p_Error_کوډ);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::منزل_ښکتنی(
        ثابت حرف *locale, uint32_t options,
        ثابت char16_t *src, int32_t src_اوږدتیا,
        char16_t *dest, int32_t dest_ظرفیت, Edits *edits,
        UError_کوډ &error_کوډ) {
    أرجع ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ustrcase_دننی_منزل_ښکتنی, edits, error_کوډ);
}

int32_t CaseMap::منزل_پاسنی(
        ثابت حرف *locale, uint32_t options,
        ثابت char16_t *src, int32_t src_اوږدتیا,
        char16_t *dest, int32_t dest_ظرفیت, Edits *edits,
        UError_کوډ &error_کوډ) {
    أرجع ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ustrcase_دننی_منزل_پاسنی, edits, error_کوډ);
}

U_NAMESPACE_پای_ټکی
