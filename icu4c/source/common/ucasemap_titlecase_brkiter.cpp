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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/brkiter.h"
#شامل "unicode/ubrk.h"
#شامل "unicode/casemap.h"
#شامل "unicode/ucasemap.h"
#شامل "cmemory.h"
#شامل "ucase.h"
#شامل "ucasemap_imp.h"

U_NAMESPACE_BEGIN

خالی CaseMap::utf8_مقصد_Title(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) {
        واپس;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.ڈیٹا(), src.length(), &error_رمز);
    Local_اشارہ_گر<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(خالی_اشاریہ, locale, options, iter, ownedIter, error_رمز);
    اگر (iter == خالی_اشاریہ) {
        utext_close(&utext);
        واپس;
    }
    iter->سیٹ_متن(&utext, error_رمز);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.ڈیٹا(), src.length(),
        ucasemap_اندرونی_UTF8_مقصد_Title, sink, edits, error_رمز);
    utext_close(&utext);
}

int32_t CaseMap::utf8_مقصد_Title(
        ثابت حرف *locale, uint32_t options, BreakIterator *iter,
        ثابت حرف *src, int32_t src_لمبائی,
        حرف *dest, int32_t dest_استعداد, Edits *edits,
        UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) {
        واپس 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_لمبائی, &error_رمز);
    Local_اشارہ_گر<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(خالی_اشاریہ, locale, options, iter, ownedIter, error_رمز);
    اگر(iter==خالی_اشاریہ) {
        utext_close(&utext);
        واپس 0;
    }
    iter->سیٹ_متن(&utext, error_رمز);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_استعداد,
        src, src_لمبائی,
        ucasemap_اندرونی_UTF8_مقصد_Title, edits, error_رمز);
    utext_close(&utext);
    واپس length;
}

U_NAMESPACE_انتہا

U_NAMESPACE_USE

U_CAPI ثابت UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(ثابت UCaseMap* csm U_LIFETIME_BOUND) {
    خودکار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    واپس reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI خالی U_EXPORT2
ucasemap_سیٹ_Break_Iterator(UCaseMap *csm, UBreakIterator *iter_مقصد_Adopt, UError_رمز *p_Error_رمز) {
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس;
    }
    خودکار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    حذف impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iter_مقصد_Adopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_مقصد_Title(UCaseMap *csm,
                     حرف *dest, int32_t dest_استعداد,
                     ثابت حرف *src, int32_t src_لمبائی,
                     UError_رمز *p_Error_رمز) {
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_لمبائی, p_Error_رمز);
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    خودکار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    اگر(impl->iter==خالی_اشاریہ) {
        Local_اشارہ_گر<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            خالی_اشاریہ, impl->locale, impl->options, خالی_اشاریہ, ownedIter, *p_Error_رمز);
        اگر (iter == خالی_اشاریہ) {
            utext_close(&utext);
            واپس 0;
        }
        impl->iter = ownedIter.orphan();
    }
    impl->iter->سیٹ_متن(&utext, *p_Error_رمز);
    int32_t length=ucasemap_mapUTF8(
            impl->caseLocale, impl->options, impl->iter,
            dest, dest_استعداد,
            src, src_لمبائی,
            ucasemap_اندرونی_UTF8_مقصد_Title, خالی_اشاریہ, *p_Error_رمز);
    utext_close(&utext);
    واپس length;
}

#اگر_ختم  // !UCONFIG_NO_BREAK_ITERATION
