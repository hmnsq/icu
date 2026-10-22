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

#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/ubrk.h"
#अन्तर्भाव "unicode/casemap.h"
#अन्तर्भाव "unicode/ucasemap.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ucase.h"
#अन्तर्भाव "ucasemap_imp.h"

U_NAMESPACE_BEGIN

शून्य CaseMap::utf8ToTitle(
        स्थिरांक अक्षर *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) {
        परत;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.data(), src.length(), &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    जर (iter == nullptr) {
        utext_close(&utext);
        परत;
    }
    iter->setText(&utext, errorCode);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.data(), src.length(),
        ucasemap_internalUTF8ToTitle, sink, edits, errorCode);
    utext_close(&utext);
}

int32_t CaseMap::utf8ToTitle(
        स्थिरांक अक्षर *locale, uint32_t options, BreakIterator *iter,
        स्थिरांक अक्षर *src, int32_t srcLength,
        अक्षर *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) {
        परत 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    जर(iter==nullptr) {
        utext_close(&utext);
        परत 0;
    }
    iter->setText(&utext, errorCode);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToTitle, edits, errorCode);
    utext_close(&utext);
    परत length;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI स्थिरांक UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(स्थिरांक UCaseMap* csm U_LIFETIME_BOUND) {
    स्वतः *impl = reinterpret_cast<स्थिरांक UCaseMapImpl *>(csm);
    परत reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI शून्य U_EXPORT2
ucasemap_setBreakIterator(UCaseMap *csm, UBreakIterator *iterToAdopt, UErrorCode *pErrorCode) {
    जर(U_FAILURE(*pErrorCode)) {
        परत;
    }
    स्वतः *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    delete impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iterToAdopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToTitle(UCaseMap *csm,
                     अक्षर *dest, int32_t destCapacity,
                     स्थिरांक अक्षर *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    जर (U_FAILURE(*pErrorCode)) {
        परत 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, pErrorCode);
    जर (U_FAILURE(*pErrorCode)) {
        परत 0;
    }
    स्वतः *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    जर(impl->iter==nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        जर (iter == nullptr) {
            utext_close(&utext);
            परत 0;
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
    परत length;
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
