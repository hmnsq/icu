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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/ubrk.h"
#అన్తర్భావ "unicode/casemap.h"
#అన్తర్భావ "unicode/ucasemap.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ucase.h"
#అన్తర్భావ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

శూన్య CaseMap::utf8_గమ్యం_Title(
        స్థిరరాశి అక్షరం *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.దత్తాంశం(), src.length(), &error_సంకేతం);
    Local_సూచిక<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(శూన్యసూచిక, locale, options, iter, ownedIter, error_సంకేతం);
    ఒకవేళ (iter == శూన్యసూచిక) {
        utext_close(&utext);
        తిరిగి;
    }
    iter->సమితి_పాఠ్యం(&utext, error_సంకేతం);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.దత్తాంశం(), src.length(),
        ucasemap_అంతర్గత_UTF8_గమ్యం_Title, sink, edits, error_సంకేతం);
    utext_close(&utext);
}

int32_t CaseMap::utf8_గమ్యం_Title(
        స్థిరరాశి అక్షరం *locale, uint32_t options, BreakIterator *iter,
        స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
        అక్షరం *dest, int32_t dest_సామర్థ్యం, Edits *edits,
        UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_నిడివి, &error_సంకేతం);
    Local_సూచిక<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(శూన్యసూచిక, locale, options, iter, ownedIter, error_సంకేతం);
    ఒకవేళ(iter==శూన్యసూచిక) {
        utext_close(&utext);
        తిరిగి 0;
    }
    iter->సమితి_పాఠ్యం(&utext, error_సంకేతం);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ucasemap_అంతర్గత_UTF8_గమ్యం_Title, edits, error_సంకేతం);
    utext_close(&utext);
    తిరిగి length;
}

U_NAMESPACE_చివర

U_NAMESPACE_USE

U_CAPI స్థిరరాశి UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(స్థిరరాశి UCaseMap* csm U_LIFETIME_BOUND) {
    స్వయంచాలక *impl = reinterpret_cast<స్థిరరాశి UCaseMapImpl *>(csm);
    తిరిగి reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI శూన్య U_EXPORT2
ucasemap_సమితి_Break_Iterator(UCaseMap *csm, UBreakIterator *iter_గమ్యం_Adopt, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    స్వయంచాలక *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    తొలగించు impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iter_గమ్యం_Adopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_గమ్యం_Title(UCaseMap *csm,
                     అక్షరం *dest, int32_t dest_సామర్థ్యం,
                     స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
                     UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_నిడివి, p_Error_సంకేతం);
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    స్వయంచాలక *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ఒకవేళ(impl->iter==శూన్యసూచిక) {
        Local_సూచిక<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            శూన్యసూచిక, impl->locale, impl->options, శూన్యసూచిక, ownedIter, *p_Error_సంకేతం);
        ఒకవేళ (iter == శూన్యసూచిక) {
            utext_close(&utext);
            తిరిగి 0;
        }
        impl->iter = ownedIter.orphan();
    }
    impl->iter->సమితి_పాఠ్యం(&utext, *p_Error_సంకేతం);
    int32_t length=ucasemap_mapUTF8(
            impl->caseLocale, impl->options, impl->iter,
            dest, dest_సామర్థ్యం,
            src, src_నిడివి,
            ucasemap_అంతర్గత_UTF8_గమ్యం_Title, శూన్యసూచిక, *p_Error_సంకేతం);
    utext_close(&utext);
    తిరిగి length;
}

#అయితే_ముగింపు  // !UCONFIG_NO_BREAK_ITERATION
