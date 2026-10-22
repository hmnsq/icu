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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "unicode/brkiter.h"
#समावेश "unicode/ubrk.h"
#समावेश "unicode/casemap.h"
#समावेश "unicode/ucasemap.h"
#समावेश "cmemory.h"
#समावेश "ucase.h"
#समावेश "ucasemap_imp.h"

U_NAMESPACE_BEGIN

व्योम CaseMap::utf8ToTitle(
        स्थिर अक्षर *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) {
        वापस;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.तथ्य(), src.length(), &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    अगर (iter == nullptr) {
        utext_close(&utext);
        वापस;
    }
    iter->setText(&utext, errorCode);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.तथ्य(), src.length(),
        ucasemap_internalUTF8ToTitle, sink, edits, errorCode);
    utext_close(&utext);
}

int32_t CaseMap::utf8ToTitle(
        स्थिर अक्षर *locale, uint32_t options, BreakIterator *iter,
        स्थिर अक्षर *src, int32_t src_लंबाई,
        अक्षर *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) {
        वापस 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_लंबाई, &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    अगर(iter==nullptr) {
        utext_close(&utext);
        वापस 0;
    }
    iter->setText(&utext, errorCode);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, src_लंबाई,
        ucasemap_internalUTF8ToTitle, edits, errorCode);
    utext_close(&utext);
    वापस length;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI स्थिर UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(स्थिर UCaseMap* csm U_LIFETIME_BOUND) {
    स्वतः *impl = reinterpret_cast<स्थिर UCaseMapImpl *>(csm);
    वापस reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI व्योम U_EXPORT2
ucasemap_setBreakIterator(UCaseMap *csm, UBreakIterator *iterToAdopt, UErrorCode *pErrorCode) {
    अगर(U_FAILURE(*pErrorCode)) {
        वापस;
    }
    स्वतः *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    मिटाओ impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iterToAdopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToTitle(UCaseMap *csm,
                     अक्षर *dest, int32_t destCapacity,
                     स्थिर अक्षर *src, int32_t src_लंबाई,
                     UErrorCode *pErrorCode) {
    अगर (U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_लंबाई, pErrorCode);
    अगर (U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    स्वतः *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    अगर(impl->iter==nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        अगर (iter == nullptr) {
            utext_close(&utext);
            वापस 0;
        }
        impl->iter = ownedIter.orphan();
    }
    impl->iter->setText(&utext, *pErrorCode);
    int32_t length=ucasemap_mapUTF8(
            impl->caseLocale, impl->options, impl->iter,
            dest, destCapacity,
            src, src_लंबाई,
            ucasemap_internalUTF8ToTitle, nullptr, *pErrorCode);
    utext_close(&utext);
    वापस length;
}

#पूर्ण_अगर  // !UCONFIG_NO_BREAK_ITERATION
