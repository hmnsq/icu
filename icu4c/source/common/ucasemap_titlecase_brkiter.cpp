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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_BREAK_ITERATION

#અન્તર્ભાવ "unicode/brkiter.h"
#અન્તર્ભાવ "unicode/ubrk.h"
#અન્તર્ભાવ "unicode/casemap.h"
#અન્તર્ભાવ "unicode/ucasemap.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "ucase.h"
#અન્તર્ભાવ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

શૂન્ય CaseMap::utf8ToTitle(
        અચલ અક્ષર *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) {
        પાછા;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.data(), src.length(), &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    જો (iter == nullptr) {
        utext_close(&utext);
        પાછા;
    }
    iter->setText(&utext, errorCode);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.data(), src.length(),
        ucasemap_internalUTF8ToTitle, sink, edits, errorCode);
    utext_close(&utext);
}

int32_t CaseMap::utf8ToTitle(
        અચલ અક્ષર *locale, uint32_t options, BreakIterator *iter,
        અચલ અક્ષર *src, int32_t srcLength,
        અક્ષર *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) {
        પાછા 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    જો(iter==nullptr) {
        utext_close(&utext);
        પાછા 0;
    }
    iter->setText(&utext, errorCode);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToTitle, edits, errorCode);
    utext_close(&utext);
    પાછા length;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI અચલ UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(અચલ UCaseMap* csm U_LIFETIME_BOUND) {
    auto *impl = reinterpret_cast<અચલ UCaseMapImpl *>(csm);
    પાછા reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI શૂન્ય U_EXPORT2
ucasemap_setBreakIterator(UCaseMap *csm, UBreakIterator *iterToAdopt, UErrorCode *pErrorCode) {
    જો(U_FAILURE(*pErrorCode)) {
        પાછા;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    delete impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iterToAdopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToTitle(UCaseMap *csm,
                     અક્ષર *dest, int32_t destCapacity,
                     અચલ અક્ષર *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    જો (U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, pErrorCode);
    જો (U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    જો(impl->iter==nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        જો (iter == nullptr) {
            utext_close(&utext);
            પાછા 0;
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
    પાછા length;
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
