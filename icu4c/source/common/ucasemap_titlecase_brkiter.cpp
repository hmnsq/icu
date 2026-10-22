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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/ubrk.h"
#അന്തര്ഭാവ "unicode/casemap.h"
#അന്തര്ഭാവ "unicode/ucasemap.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "ucase.h"
#അന്തര്ഭാവ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

ശൂന്യം CaseMap::utf8ToTitle(
        സ്ഥിരം അക്ഷരം *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.data(), src.length(), &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    എങ്കിൽ (iter == nullptr) {
        utext_close(&utext);
        തിരികെ;
    }
    iter->setText(&utext, errorCode);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.data(), src.length(),
        ucasemap_internalUTF8ToTitle, sink, edits, errorCode);
    utext_close(&utext);
}

int32_t CaseMap::utf8ToTitle(
        സ്ഥിരം അക്ഷരം *locale, uint32_t options, BreakIterator *iter,
        സ്ഥിരം അക്ഷരം *src, int32_t srcLength,
        അക്ഷരം *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, &errorCode);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(nullptr, locale, options, iter, ownedIter, errorCode);
    എങ്കിൽ(iter==nullptr) {
        utext_close(&utext);
        തിരികെ 0;
    }
    iter->setText(&utext, errorCode);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToTitle, edits, errorCode);
    utext_close(&utext);
    തിരികെ length;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI സ്ഥിരം UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(സ്ഥിരം UCaseMap* csm U_LIFETIME_BOUND) {
    auto *impl = reinterpret_cast<സ്ഥിരം UCaseMapImpl *>(csm);
    തിരികെ reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI ശൂന്യം U_EXPORT2
ucasemap_setBreakIterator(UCaseMap *csm, UBreakIterator *iterToAdopt, UErrorCode *pErrorCode) {
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    delete impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iterToAdopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToTitle(UCaseMap *csm,
                     അക്ഷരം *dest, int32_t destCapacity,
                     സ്ഥിരം അക്ഷരം *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    എങ്കിൽ (U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, srcLength, pErrorCode);
    എങ്കിൽ (U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    എങ്കിൽ(impl->iter==nullptr) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            nullptr, impl->locale, impl->options, nullptr, ownedIter, *pErrorCode);
        എങ്കിൽ (iter == nullptr) {
            utext_close(&utext);
            തിരികെ 0;
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
    തിരികെ length;
}

#endif  // !UCONFIG_NO_BREAK_ITERATION
