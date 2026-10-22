// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucasemap_titlecase_brkiter.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011jun02
*   created by: Markus W. Scherer
*
*   Titlecasing functions that are based on BreakIterator
*   were moved here to break dependency cycles among parts of the common library.
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubrk.h"
#ಅನ್ತರ್ಭಾವ "unicode/casemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucasemap.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ucase.h"
#ಅನ್ತರ್ಭಾವ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

ಶೂನ್ಯ CaseMap::utf8_ಗಮ್ಯ_Title(
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.ದತ್ತಾಂಶ(), src.length(), &error_ಸಂಕೇತ);
    Local_ಸೂಚಕ<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(ಶೂನ್ಯಸೂಚಕ, locale, options, iter, ownedIter, error_ಸಂಕೇತ);
    ಆದರೆ (iter == ಶೂನ್ಯಸೂಚಕ) {
        utext_close(&utext);
        ಹಿಂತಿರುಗಿ;
    }
    iter->ಗಣ_ಪಠ್ಯ(&utext, error_ಸಂಕೇತ);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.ದತ್ತಾಂಶ(), src.length(),
        ucasemap_ಆಂತರಿಕ_UTF8_ಗಮ್ಯ_Title, sink, edits, error_ಸಂಕೇತ);
    utext_close(&utext);
}

int32_t CaseMap::utf8_ಗಮ್ಯ_Title(
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale, uint32_t options, BreakIterator *iter,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t src_ಉದ್ದಳತೆ,
        ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, Edits *edits,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_ಉದ್ದಳತೆ, &error_ಸಂಕೇತ);
    Local_ಸೂಚಕ<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(ಶೂನ್ಯಸೂಚಕ, locale, options, iter, ownedIter, error_ಸಂಕೇತ);
    ಆದರೆ(iter==ಶೂನ್ಯಸೂಚಕ) {
        utext_close(&utext);
        ಹಿಂತಿರುಗಿ 0;
    }
    iter->ಗಣ_ಪಠ್ಯ(&utext, error_ಸಂಕೇತ);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ucasemap_ಆಂತರಿಕ_UTF8_ಗಮ್ಯ_Title, edits, error_ಸಂಕೇತ);
    utext_close(&utext);
    ಹಿಂತಿರುಗಿ length;
}

U_NAMESPACE_ಕೊನೆ

U_NAMESPACE_USE

U_CAPI ಸ್ಥಿರಾಂಕ UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(ಸ್ಥಿರಾಂಕ UCaseMap* csm U_LIFETIME_BOUND) {
    ಸ್ವಯಂಚಾಲಿತ *impl = reinterpret_cast<ಸ್ಥಿರಾಂಕ UCaseMapImpl *>(csm);
    ಹಿಂತಿರುಗಿ reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucasemap_ಗಣ_Break_Iterator(UCaseMap *csm, UBreakIterator *iter_ಗಮ್ಯ_Adopt, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ವಯಂಚಾಲಿತ *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ಅಳಿಸು impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iter_ಗಮ್ಯ_Adopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_ಗಮ್ಯ_Title(UCaseMap *csm,
                     ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t src_ಉದ್ದಳತೆ,
                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಸ್ವಯಂಚಾಲಿತ *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ಆದರೆ(impl->iter==ಶೂನ್ಯಸೂಚಕ) {
        Local_ಸೂಚಕ<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            ಶೂನ್ಯಸೂಚಕ, impl->locale, impl->options, ಶೂನ್ಯಸೂಚಕ, ownedIter, *p_Error_ಸಂಕೇತ);
        ಆದರೆ (iter == ಶೂನ್ಯಸೂಚಕ) {
            utext_close(&utext);
            ಹಿಂತಿರುಗಿ 0;
        }
        impl->iter = ownedIter.orphan();
    }
    impl->iter->ಗಣ_ಪಠ್ಯ(&utext, *p_Error_ಸಂಕೇತ);
    int32_t length=ucasemap_mapUTF8(
            impl->caseLocale, impl->options, impl->iter,
            dest, dest_ಸಾಮರ್ಥ್ಯ,
            src, src_ಉದ್ದಳತೆ,
            ucasemap_ಆಂತರಿಕ_UTF8_ಗಮ್ಯ_Title, ಶೂನ್ಯಸೂಚಕ, *p_Error_ಸಂಕೇತ);
    utext_close(&utext);
    ಹಿಂತಿರುಗಿ length;
}

#ಆದರೆ_ಅಂತ್ಯ  // !UCONFIG_NO_BREAK_ITERATION
