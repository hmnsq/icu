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

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/brkiter.h"
#ضمن "unicode/ubrk.h"
#ضمن "unicode/casemap.h"
#ضمن "unicode/ucasemap.h"
#ضمن "cmemory.h"
#ضمن "ucase.h"
#ضمن "ucasemap_imp.h"

U_NAMESPACE_BEGIN

فراغ CaseMap::utf8_منزل_Title(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.معلومات(), src.length(), &error_ڪوڊ);
    Local_اشارو<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(خالي_اشارو, locale, options, iter, ownedIter, error_ڪوڊ);
    إذا (iter == خالي_اشارو) {
        utext_close(&utext);
        أرجع;
    }
    iter->سيٽ_متن(&utext, error_ڪوڊ);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.معلومات(), src.length(),
        ucasemap_اندروني_UTF8_منزل_Title, sink, edits, error_ڪوڊ);
    utext_close(&utext);
}

int32_t CaseMap::utf8_منزل_Title(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        ثابت حرف *src, int32_t src_ڊيگهائي,
        حرف *dest, int32_t dest_صلاحيت, Edits *edits,
        UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_ڊيگهائي, &error_ڪوڊ);
    Local_اشارو<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(خالي_اشارو, locale, options, iter, ownedIter, error_ڪوڊ);
    إذا(iter==خالي_اشارو) {
        utext_close(&utext);
        أرجع 0;
    }
    iter->سيٽ_متن(&utext, error_ڪوڊ);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_صلاحيت,
        src, src_ڊيگهائي,
        ucasemap_اندروني_UTF8_منزل_Title, edits, error_ڪوڊ);
    utext_close(&utext);
    أرجع length;
}

U_NAMESPACE_پڇاڙي

U_NAMESPACE_USE

U_CAPI ثابت UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(ثابت UCaseMap* csm U_LIFETIME_BOUND) {
    خودڪار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI فراغ U_EXPORT2
ucasemap_سيٽ_Break_Iterator(UCaseMap *csm, UBreakIterator *iter_منزل_Adopt, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }
    خودڪار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ميٽيو impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iter_منزل_Adopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_منزل_Title(UCaseMap *csm,
                     حرف *dest, int32_t dest_صلاحيت,
                     ثابت حرف *src, int32_t src_ڊيگهائي,
                     UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_ڊيگهائي, p_Error_ڪوڊ);
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    خودڪار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    إذا(impl->iter==خالي_اشارو) {
        Local_اشارو<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            خالي_اشارو, impl->locale, impl->options, خالي_اشارو, ownedIter, *p_Error_ڪوڊ);
        إذا (iter == خالي_اشارو) {
            utext_close(&utext);
            أرجع 0;
        }
        impl->iter = ownedIter.orphan();
    }
    impl->iter->سيٽ_متن(&utext, *p_Error_ڪوڊ);
    int32_t length=ucasemap_mapUTF8(
            impl->caseLocale, impl->options, impl->iter,
            dest, dest_صلاحيت,
            src, src_ڊيگهائي,
            ucasemap_اندروني_UTF8_منزل_Title, خالي_اشارو, *p_Error_ڪوڊ);
    utext_close(&utext);
    أرجع length;
}

#جيڪڏهن_پورو  // !UCONFIG_NO_BREAK_ITERATION
