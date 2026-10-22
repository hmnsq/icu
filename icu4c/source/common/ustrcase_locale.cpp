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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/casemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucasemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "ucase.h"
#ಅನ್ತರ್ಭಾವ "ucasemap_imp.h"

U_CFUNC int32_t
ustrcase_getCaseLocale(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale) {
    ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) {
        locale = uloc_getDefault();
    }
    ಆದರೆ (*locale == 0) {
        ಹಿಂತಿರುಗಿ UCASE_LOC_ಬೇರು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ucase_getCaseLocale(locale);
    }
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_str_ಗಮ್ಯ_ಅಧೋ(char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
             ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_ಅಧೋ, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
u_str_ಗಮ್ಯ_ಊರ್ಧ್ವ(char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
             ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale,
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_ಊರ್ಧ್ವ, *p_Error_ಸಂಕೇತ);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::ಗಮ್ಯ_ಅಧೋ(
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale, uint32_t options,
        ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
        char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, Edits *edits,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_ಅಧೋ, edits, error_ಸಂಕೇತ);
}

int32_t CaseMap::ಗಮ್ಯ_ಊರ್ಧ್ವ(
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale, uint32_t options,
        ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
        char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, Edits *edits,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_ಊರ್ಧ್ವ, edits, error_ಸಂಕೇತ);
}

U_NAMESPACE_ಕೊನೆ
