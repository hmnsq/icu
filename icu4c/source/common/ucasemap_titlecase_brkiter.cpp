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
        UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.انکڑے(), src.length(), &error_کوڈ);
    Local_اشارہ_گر<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(خالی_سوچک, locale, options, iter, ownedIter, error_کوڈ);
    إذا (iter == خالی_سوچک) {
        utext_close(&utext);
        أرجع;
    }
    iter->سیٹ_متن(&utext, error_کوڈ);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.انکڑے(), src.length(),
        ucasemap_اندرونی_UTF8_منزل_Title, sink, edits, error_کوڈ);
    utext_close(&utext);
}

int32_t CaseMap::utf8_منزل_Title(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        ثابت حرف *src, int32_t src_لمائی,
        حرف *dest, int32_t dest_سمرتھا, Edits *edits,
        UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_لمائی, &error_کوڈ);
    Local_اشارہ_گر<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(خالی_سوچک, locale, options, iter, ownedIter, error_کوڈ);
    إذا(iter==خالی_سوچک) {
        utext_close(&utext);
        أرجع 0;
    }
    iter->سیٹ_متن(&utext, error_کوڈ);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_سمرتھا,
        src, src_لمائی,
        ucasemap_اندرونی_UTF8_منزل_Title, edits, error_کوڈ);
    utext_close(&utext);
    أرجع length;
}

U_NAMESPACE_سرا

U_NAMESPACE_USE

U_CAPI ثابت UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(ثابت UCaseMap* csm U_LIFETIME_BOUND) {
    خودکار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI فراغ U_EXPORT2
ucasemap_سیٹ_Break_Iterator(UCaseMap *csm, UBreakIterator *iter_منزل_Adopt, UError_کوڈ *p_Error_کوڈ) {
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    خودکار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    مٹاؤ impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iter_منزل_Adopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_منزل_Title(UCaseMap *csm,
                     حرف *dest, int32_t dest_سمرتھا,
                     ثابت حرف *src, int32_t src_لمائی,
                     UError_کوڈ *p_Error_کوڈ) {
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_لمائی, p_Error_کوڈ);
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    خودکار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    إذا(impl->iter==خالی_سوچک) {
        Local_اشارہ_گر<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            خالی_سوچک, impl->locale, impl->options, خالی_سوچک, ownedIter, *p_Error_کوڈ);
        إذا (iter == خالی_سوچک) {
            utext_close(&utext);
            أرجع 0;
        }
        impl->iter = ownedIter.orphan();
    }
    impl->iter->سیٹ_متن(&utext, *p_Error_کوڈ);
    int32_t length=ucasemap_mapUTF8(
            impl->caseLocale, impl->options, impl->iter,
            dest, dest_سمرتھا,
            src, src_لمائی,
            ucasemap_اندرونی_UTF8_منزل_Title, خالی_سوچک, *p_Error_کوڈ);
    utext_close(&utext);
    أرجع length;
}

#جے_انت  // !UCONFIG_NO_BREAK_ITERATION
