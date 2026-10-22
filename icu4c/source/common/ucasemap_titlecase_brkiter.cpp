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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাৱ "unicode/brkiter.h"
#অন্তর্ভাৱ "unicode/ubrk.h"
#অন্তর্ভাৱ "unicode/casemap.h"
#অন্তর্ভাৱ "unicode/ucasemap.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "ucase.h"
#অন্তর্ভাৱ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

শূন্য CaseMap::utf8ToTitle(
        ধ্রুৱ ৱর্ণ *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) {
        ঘূৰাই;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.data(), src.length(), &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    যদি (iter == nullptr) {
        utext_close(&utext);
        ঘূৰাই;
    }
    iter->setText(&utext, errorCode);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.data(), src.length(),
        ucasemap_internalUTF8ToTitle, sink, edits, errorCode);
    utext_close(&utext);
}

int32_t CaseMap::utf8ToTitle(
        ধ্রুৱ ৱর্ণ *locale, uint32_t options, BreakIterator *iter,
        ধ্রুৱ ৱর্ণ *src, int32_t srcLength,
        ৱর্ণ *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) {
        ঘূৰাই 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    যদি(iter==nullptr) {
        utext_close(&utext);
        ঘূৰাই 0;
    }
    iter->setText(&utext, errorCode);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToTitle, edits, errorCode);
    utext_close(&utext);
    ঘূৰাই length;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI ধ্রুৱ UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(ধ্রুৱ UCaseMap* csm U_LIFETIME_BOUND) {
    auto *impl = reinterpret_cast<ধ্রুৱ UCaseMapImpl *>(csm);
    ঘূৰাই reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI শূন্য U_EXPORT2
ucasemap_setBreakIterator(UCaseMap *csm, UBreakIterator *iterToAdopt, UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        ঘূৰাই;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    delete impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iterToAdopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToTitle(UCaseMap *csm,
                     ৱর্ণ *dest, int32_t destCapacity,
                     ধ্রুৱ ৱর্ণ *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    যদি (U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, pErrorCode);
    যদি (U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    যদি(impl->iter==nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        যদি (iter == nullptr) {
            utext_close(&utext);
            ঘূৰাই 0;
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
    ঘূৰাই length;
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
