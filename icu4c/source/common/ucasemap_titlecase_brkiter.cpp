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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubrk.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/casemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucasemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

ਖਾਲੀ CaseMap::utf8_ਮੰਜ਼ਿਲ_Title(
        ਅਚਲ ਅੱਖਰ *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.ਅੰਕੜੇ(), src.length(), &error_ਕੋਡ);
    Local_ਸੂਚਕ<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(ਖਾਲੀ_ਸੂਚਕ, locale, options, iter, ownedIter, error_ਕੋਡ);
    ਜੇ (iter == ਖਾਲੀ_ਸੂਚਕ) {
        utext_close(&utext);
        ਵਾਪਸ;
    }
    iter->ਸੈੱਟ_ਪਾਠ(&utext, error_ਕੋਡ);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.ਅੰਕੜੇ(), src.length(),
        ucasemap_ਅੰਦਰੂਨੀ_UTF8_ਮੰਜ਼ਿਲ_Title, sink, edits, error_ਕੋਡ);
    utext_close(&utext);
}

int32_t CaseMap::utf8_ਮੰਜ਼ਿਲ_Title(
        ਅਚਲ ਅੱਖਰ *locale, uint32_t options, BreakIterator *iter,
        ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
        ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_ਲੰਮਾਈ, &error_ਕੋਡ);
    Local_ਸੂਚਕ<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(ਖਾਲੀ_ਸੂਚਕ, locale, options, iter, ownedIter, error_ਕੋਡ);
    ਜੇ(iter==ਖਾਲੀ_ਸੂਚਕ) {
        utext_close(&utext);
        ਵਾਪਸ 0;
    }
    iter->ਸੈੱਟ_ਪਾਠ(&utext, error_ਕੋਡ);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ucasemap_ਅੰਦਰੂਨੀ_UTF8_ਮੰਜ਼ਿਲ_Title, edits, error_ਕੋਡ);
    utext_close(&utext);
    ਵਾਪਸ length;
}

U_NAMESPACE_ਸਿਰਾ

U_NAMESPACE_USE

U_CAPI ਅਚਲ UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(ਅਚਲ UCaseMap* csm U_LIFETIME_BOUND) {
    ਸਵੈਚਾਲਿਤ *impl = reinterpret_cast<ਅਚਲ UCaseMapImpl *>(csm);
    ਵਾਪਸ reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI ਖਾਲੀ U_EXPORT2
ucasemap_ਸੈੱਟ_Break_Iterator(UCaseMap *csm, UBreakIterator *iter_ਮੰਜ਼ਿਲ_Adopt, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਸਵੈਚਾਲਿਤ *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ਮਿਟਾਓ impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iter_ਮੰਜ਼ਿਲ_Adopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_ਮੰਜ਼ਿਲ_Title(UCaseMap *csm,
                     ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ,
                     ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
                     UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_ਲੰਮਾਈ, p_Error_ਕੋਡ);
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਸਵੈਚਾਲਿਤ *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ਜੇ(impl->iter==ਖਾਲੀ_ਸੂਚਕ) {
        Local_ਸੂਚਕ<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            ਖਾਲੀ_ਸੂਚਕ, impl->locale, impl->options, ਖਾਲੀ_ਸੂਚਕ, ownedIter, *p_Error_ਕੋਡ);
        ਜੇ (iter == ਖਾਲੀ_ਸੂਚਕ) {
            utext_close(&utext);
            ਵਾਪਸ 0;
        }
        impl->iter = ownedIter.orphan();
    }
    impl->iter->ਸੈੱਟ_ਪਾਠ(&utext, *p_Error_ਕੋਡ);
    int32_t length=ucasemap_mapUTF8(
            impl->caseLocale, impl->options, impl->iter,
            dest, dest_ਸਮਰੱਥਾ,
            src, src_ਲੰਮਾਈ,
            ucasemap_ਅੰਦਰੂਨੀ_UTF8_ਮੰਜ਼ਿਲ_Title, ਖਾਲੀ_ਸੂਚਕ, *p_Error_ਕੋਡ);
    utext_close(&utext);
    ਵਾਪਸ length;
}

#ਜੇ_ਅੰਤ  // !UCONFIG_NO_BREAK_ITERATION
