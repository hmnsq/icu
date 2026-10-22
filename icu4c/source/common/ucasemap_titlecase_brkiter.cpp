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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "unicode/brkiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ubrk.h"
#ଅନ୍ତର୍ଭାଵ "unicode/casemap.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucasemap.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "ucase.h"
#ଅନ୍ତର୍ଭାଵ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

ଶୂନ୍ୟ CaseMap::utf8ToTitle(
        ଅଚଳ ଅକ୍ଷର *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.data(), src.length(), &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    ଯଦି (iter == nullptr) {
        utext_close(&utext);
        ଫେରନ୍ତୁ;
    }
    iter->setText(&utext, errorCode);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.data(), src.length(),
        ucasemap_internalUTF8ToTitle, sink, edits, errorCode);
    utext_close(&utext);
}

int32_t CaseMap::utf8ToTitle(
        ଅଚଳ ଅକ୍ଷର *locale, uint32_t options, BreakIterator *iter,
        ଅଚଳ ଅକ୍ଷର *src, int32_t srcLength,
        ଅକ୍ଷର *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    ଯଦି(iter==nullptr) {
        utext_close(&utext);
        ଫେରନ୍ତୁ 0;
    }
    iter->setText(&utext, errorCode);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToTitle, edits, errorCode);
    utext_close(&utext);
    ଫେରନ୍ତୁ length;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI ଅଚଳ UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(ଅଚଳ UCaseMap* csm U_LIFETIME_BOUND) {
    auto *impl = reinterpret_cast<ଅଚଳ UCaseMapImpl *>(csm);
    ଫେରନ୍ତୁ reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
ucasemap_setBreakIterator(UCaseMap *csm, UBreakIterator *iterToAdopt, UErrorCode *pErrorCode) {
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    delete impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iterToAdopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToTitle(UCaseMap *csm,
                     ଅକ୍ଷର *dest, int32_t destCapacity,
                     ଅଚଳ ଅକ୍ଷର *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, pErrorCode);
    ଯଦି (U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ଯଦି(impl->iter==nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        ଯଦି (iter == nullptr) {
            utext_close(&utext);
            ଫେରନ୍ତୁ 0;
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
    ଫେରନ୍ତୁ length;
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
