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

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubrk.h"
#ಅನ್ತರ್ಭಾವ "unicode/casemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucasemap.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ucase.h"
#ಅನ್ತರ್ಭಾವ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

ಶೂನ್ಯ CaseMap::utf8ToTitle(
        ಧ್ರುವ ವರ್ಣ *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.data(), src.length(), &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    ಯದಿ (iter == nullptr) {
        utext_close(&utext);
        ಪ್ರತ್ಯಾಗಮ;
    }
    iter->setText(&utext, errorCode);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.data(), src.length(),
        ucasemap_internalUTF8ToTitle, sink, edits, errorCode);
    utext_close(&utext);
}

int32_t CaseMap::utf8ToTitle(
        ಧ್ರುವ ವರ್ಣ *locale, uint32_t options, BreakIterator *iter,
        ಧ್ರುವ ವರ್ಣ *src, int32_t srcLength,
        ವರ್ಣ *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    ಯದಿ(iter==nullptr) {
        utext_close(&utext);
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    iter->setText(&utext, errorCode);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToTitle, edits, errorCode);
    utext_close(&utext);
    ಪ್ರತ್ಯಾಗಮ length;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI ಧ್ರುವ UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(ಧ್ರುವ UCaseMap* csm U_LIFETIME_BOUND) {
    auto *impl = reinterpret_cast<ಧ್ರುವ UCaseMapImpl *>(csm);
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucasemap_setBreakIterator(UCaseMap *csm, UBreakIterator *iterToAdopt, UErrorCode *pErrorCode) {
    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    delete impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iterToAdopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToTitle(UCaseMap *csm,
                     ವರ್ಣ *dest, int32_t destCapacity,
                     ಧ್ರುವ ವರ್ಣ *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, pErrorCode);
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ಯದಿ(impl->iter==nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        ಯದಿ (iter == nullptr) {
            utext_close(&utext);
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        impl->iter = ownedIter.orphan();
    }
    impl->iter->setText(&utext, *pErrorCode);
    int32_t length=ucasemap_mapUTF8(
            impl->caseLocale, impl->options, impl->iter,
            dest, destCapacity,
            src, srcLength,
            ucasemap_internalUTF8ToTitle, nullptr, *pErrorCode);
    utext_close(&utext);
    ಪ್ರತ್ಯಾಗಮ length;
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
