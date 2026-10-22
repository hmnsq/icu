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

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/ubrk.h"
#अन्तर्भाव "unicode/casemap.h"
#अन्तर्भाव "unicode/ucasemap.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ucase.h"
#अन्तर्भाव "ucasemap_imp.h"

U_NAMESPACE_BEGIN

शून्य CaseMap::utf8ToTitle(
        ध्रुव वर्ण *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.data(), src.length(), &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    यदि (iter == nullptr) {
        utext_close(&utext);
        प्रत्यागम;
    }
    iter->setText(&utext, errorCode);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.data(), src.length(),
        ucasemap_internalUTF8ToTitle, sink, edits, errorCode);
    utext_close(&utext);
}

int32_t CaseMap::utf8ToTitle(
        ध्रुव वर्ण *locale, uint32_t options, BreakIterator *iter,
        ध्रुव वर्ण *src, int32_t srcLength,
        वर्ण *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    यदि(iter==nullptr) {
        utext_close(&utext);
        प्रत्यागम 0;
    }
    iter->setText(&utext, errorCode);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToTitle, edits, errorCode);
    utext_close(&utext);
    प्रत्यागम length;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI ध्रुव UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(ध्रुव UCaseMap* csm U_LIFETIME_BOUND) {
    auto *impl = reinterpret_cast<ध्रुव UCaseMapImpl *>(csm);
    प्रत्यागम reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI शून्य U_EXPORT2
ucasemap_setBreakIterator(UCaseMap *csm, UBreakIterator *iterToAdopt, UErrorCode *pErrorCode) {
    यदि(U_FAILURE(*pErrorCode)) {
        प्रत्यागम;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    delete impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iterToAdopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToTitle(UCaseMap *csm,
                     वर्ण *dest, int32_t destCapacity,
                     ध्रुव वर्ण *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    यदि (U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, pErrorCode);
    यदि (U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    यदि(impl->iter==nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        यदि (iter == nullptr) {
            utext_close(&utext);
            प्रत्यागम 0;
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
    प्रत्यागम length;
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
