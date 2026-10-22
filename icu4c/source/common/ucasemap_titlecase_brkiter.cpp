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

#ضمن "unicode/utypes.h"

#ܐܢ !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/brkiter.h"
#ضمن "unicode/ubrk.h"
#ضمن "unicode/casemap.h"
#ضمن "unicode/ucasemap.h"
#ضمن "cmemory.h"
#ضمن "ucase.h"
#ضمن "ucasemap_imp.h"

U_NAMESPACE_BEGIN

فراغ CaseMap::utf8ToTitle(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ܐܢ (U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.data(), src.length(), &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    ܐܢ (iter == nullptr) {
        utext_close(&utext);
        ܦܢܐ;
    }
    iter->setText(&utext, errorCode);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.data(), src.length(),
        ucasemap_internalUTF8ToTitle, sink, edits, errorCode);
    utext_close(&utext);
}

int32_t CaseMap::utf8ToTitle(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        ثابت حرف *src, int32_t srcLength,
        حرف *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ܐܢ (U_FAILURE(errorCode)) {
        ܦܢܐ 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    ܐܢ(iter==nullptr) {
        utext_close(&utext);
        ܦܢܐ 0;
    }
    iter->setText(&utext, errorCode);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToTitle, edits, errorCode);
    utext_close(&utext);
    ܦܢܐ length;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI ثابت UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(ثابت UCaseMap* csm U_LIFETIME_BOUND) {
    auto *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    ܦܢܐ reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI فراغ U_EXPORT2
ucasemap_setBreakIterator(UCaseMap *csm, UBreakIterator *iterToAdopt, UErrorCode *pErrorCode) {
    ܐܢ(U_FAILURE(*pErrorCode)) {
        ܦܢܐ;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    delete impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iterToAdopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToTitle(UCaseMap *csm,
                     حرف *dest, int32_t destCapacity,
                     ثابت حرف *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    ܐܢ (U_FAILURE(*pErrorCode)) {
        ܦܢܐ 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, pErrorCode);
    ܐܢ (U_FAILURE(*pErrorCode)) {
        ܦܢܐ 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ܐܢ(impl->iter==nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        ܐܢ (iter == nullptr) {
            utext_close(&utext);
            ܦܢܐ 0;
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
    ܦܢܐ length;
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
