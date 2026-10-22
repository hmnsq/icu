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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ "unicode/brkiter.h"
#அந்தர்பாவ "unicode/ubrk.h"
#அந்தர்பாவ "unicode/casemap.h"
#அந்தர்பாவ "unicode/ucasemap.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "ucase.h"
#அந்தர்பாவ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

வெறுமை CaseMap::utf8ToTitle(
        மாறிலி எழுத்து *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.data(), src.length(), &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    எனில் (iter == nullptr) {
        utext_close(&utext);
        திரும்பு;
    }
    iter->setText(&utext, errorCode);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.data(), src.length(),
        ucasemap_internalUTF8ToTitle, sink, edits, errorCode);
    utext_close(&utext);
}

int32_t CaseMap::utf8ToTitle(
        மாறிலி எழுத்து *locale, uint32_t options, BreakIterator *iter,
        மாறிலி எழுத்து *src, int32_t srcLength,
        எழுத்து *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    எனில்(iter==nullptr) {
        utext_close(&utext);
        திரும்பு 0;
    }
    iter->setText(&utext, errorCode);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToTitle, edits, errorCode);
    utext_close(&utext);
    திரும்பு length;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI மாறிலி UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(மாறிலி UCaseMap* csm U_LIFETIME_BOUND) {
    auto *impl = reinterpret_cast<மாறிலி UCaseMapImpl *>(csm);
    திரும்பு reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI வெறுமை U_EXPORT2
ucasemap_setBreakIterator(UCaseMap *csm, UBreakIterator *iterToAdopt, UErrorCode *pErrorCode) {
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    delete impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iterToAdopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToTitle(UCaseMap *csm,
                     எழுத்து *dest, int32_t destCapacity,
                     மாறிலி எழுத்து *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, pErrorCode);
    எனில் (U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    எனில்(impl->iter==nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        எனில் (iter == nullptr) {
            utext_close(&utext);
            திரும்பு 0;
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
    திரும்பு length;
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
