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

فراغ CaseMap::utf8_مقصد_Title(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) {
        أرجع;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.داده(), src.length(), &error_کد);
    Local_اشاره_گر<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(اشاره_تهی, locale, options, iter, ownedIter, error_کد);
    إذا (iter == اشاره_تهی) {
        utext_close(&utext);
        أرجع;
    }
    iter->مجموعه_متن(&utext, error_کد);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.داده(), src.length(),
        ucasemap_داخلی_UTF8_مقصد_Title, sink, edits, error_کد);
    utext_close(&utext);
}

int32_t CaseMap::utf8_مقصد_Title(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        ثابت حرف *src, int32_t src_درازی,
        حرف *dest, int32_t dest_ظرفیت, Edits *edits,
        UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) {
        أرجع 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_درازی, &error_کد);
    Local_اشاره_گر<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(اشاره_تهی, locale, options, iter, ownedIter, error_کد);
    إذا(iter==اشاره_تهی) {
        utext_close(&utext);
        أرجع 0;
    }
    iter->مجموعه_متن(&utext, error_کد);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_ظرفیت,
        src, src_درازی,
        ucasemap_داخلی_UTF8_مقصد_Title, edits, error_کد);
    utext_close(&utext);
    أرجع length;
}

U_NAMESPACE_انتها

U_NAMESPACE_USE

U_CAPI ثابت UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(ثابت UCaseMap* csm U_LIFETIME_BOUND) {
    خودکار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI فراغ U_EXPORT2
ucasemap_مجموعه_Break_Iterator(UCaseMap *csm, UBreakIterator *iter_مقصد_Adopt, UError_کد *p_Error_کد) {
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    خودکار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    حذف impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iter_مقصد_Adopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_مقصد_Title(UCaseMap *csm,
                     حرف *dest, int32_t dest_ظرفیت,
                     ثابت حرف *src, int32_t src_درازی,
                     UError_کد *p_Error_کد) {
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_درازی, p_Error_کد);
    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    خودکار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    إذا(impl->iter==اشاره_تهی) {
        Local_اشاره_گر<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            اشاره_تهی, impl->locale, impl->options, اشاره_تهی, ownedIter, *p_Error_کد);
        إذا (iter == اشاره_تهی) {
            utext_close(&utext);
            أرجع 0;
        }
        impl->iter = ownedIter.orphan();
    }
    impl->iter->مجموعه_متن(&utext, *p_Error_کد);
    int32_t length=ucasemap_mapUTF8(
            impl->caseLocale, impl->options, impl->iter,
            dest, dest_ظرفیت,
            src, src_درازی,
            ucasemap_داخلی_UTF8_مقصد_Title, اشاره_تهی, *p_Error_کد);
    utext_close(&utext);
    أرجع length;
}

#پایان_اگر  // !UCONFIG_NO_BREAK_ITERATION
