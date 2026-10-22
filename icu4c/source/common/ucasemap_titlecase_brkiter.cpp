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
        UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.معلومات(), src.length(), &error_کوډ);
    Local_اشاره_ګر<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(تش_اشاره, locale, options, iter, ownedIter, error_کوډ);
    إذا (iter == تش_اشاره) {
        utext_close(&utext);
        أرجع;
    }
    iter->سېټ_متن(&utext, error_کوډ);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.معلومات(), src.length(),
        ucasemap_دننی_UTF8_منزل_Title, sink, edits, error_کوډ);
    utext_close(&utext);
}

int32_t CaseMap::utf8_منزل_Title(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        ثابت حرف *src, int32_t src_اوږدتیا,
        حرف *dest, int32_t dest_ظرفیت, Edits *edits,
        UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_اوږدتیا, &error_کوډ);
    Local_اشاره_ګر<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(تش_اشاره, locale, options, iter, ownedIter, error_کوډ);
    إذا(iter==تش_اشاره) {
        utext_close(&utext);
        أرجع 0;
    }
    iter->سېټ_متن(&utext, error_کوډ);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ucasemap_دننی_UTF8_منزل_Title, edits, error_کوډ);
    utext_close(&utext);
    أرجع length;
}

U_NAMESPACE_پای_ټکی

U_NAMESPACE_USE

U_CAPI ثابت UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(ثابت UCaseMap* csm U_LIFETIME_BOUND) {
    خپلکاری *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI فراغ U_EXPORT2
ucasemap_سېټ_Break_Iterator(UCaseMap *csm, UBreakIterator *iter_منزل_Adopt, UError_کوډ *p_Error_کوډ) {
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }
    خپلکاری *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ړنګول impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iter_منزل_Adopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_منزل_Title(UCaseMap *csm,
                     حرف *dest, int32_t dest_ظرفیت,
                     ثابت حرف *src, int32_t src_اوږدتیا,
                     UError_کوډ *p_Error_کوډ) {
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_اوږدتیا, p_Error_کوډ);
    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    خپلکاری *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    إذا(impl->iter==تش_اشاره) {
        Local_اشاره_ګر<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            تش_اشاره, impl->locale, impl->options, تش_اشاره, ownedIter, *p_Error_کوډ);
        إذا (iter == تش_اشاره) {
            utext_close(&utext);
            أرجع 0;
        }
        impl->iter = ownedIter.orphan();
    }
    impl->iter->سېټ_متن(&utext, *p_Error_کوډ);
    int32_t length=ucasemap_mapUTF8(
            impl->caseLocale, impl->options, impl->iter,
            dest, dest_ظرفیت,
            src, src_اوږدتیا,
            ucasemap_دننی_UTF8_منزل_Title, تش_اشاره, *p_Error_کوډ);
    utext_close(&utext);
    أرجع length;
}

#د_که_پای  // !UCONFIG_NO_BREAK_ITERATION
