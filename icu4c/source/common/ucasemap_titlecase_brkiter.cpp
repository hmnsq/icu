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

शून्य CaseMap::utf8_गन्तव्यम्_Title(
        ध्रुव वर्ण *locale, uint32_t options, BreakIterator *iter,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    UText utext = UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src.दत्त(), src.length(), &error_संहिता);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(शून्यसूचकः, locale, options, iter, ownedIter, error_संहिता);
    यदि (iter == शून्यसूचकः) {
        utext_close(&utext);
        प्रत्यागम;
    }
    iter->समुच्चयः_Text(&utext, error_संहिता);
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        src.दत्त(), src.length(),
        ucasemap_आन्तरिकम्_UTF8_गन्तव्यम्_Title, sink, edits, error_संहिता);
    utext_close(&utext);
}

int32_t CaseMap::utf8_गन्तव्यम्_Title(
        ध्रुव वर्ण *locale, uint32_t options, BreakIterator *iter,
        ध्रुव वर्ण *src, int32_t src_दीर्घता,
        वर्ण *dest, int32_t dest_क्षमता, Edits *edits,
        UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_दीर्घता, &error_संहिता);
    LocalPointer<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(शून्यसूचकः, locale, options, iter, ownedIter, error_संहिता);
    यदि(iter==शून्यसूचकः) {
        utext_close(&utext);
        प्रत्यागम 0;
    }
    iter->समुच्चयः_Text(&utext, error_संहिता);
    int32_t length=ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ucasemap_आन्तरिकम्_UTF8_गन्तव्यम्_Title, edits, error_संहिता);
    utext_close(&utext);
    प्रत्यागम length;
}

U_NAMESPACE_अन्तः

U_NAMESPACE_USE

U_CAPI ध्रुव UBreakIterator* U_EXPORT2
ucasemap_getBreakIterator(ध्रुव UCaseMap* csm U_LIFETIME_BOUND) {
    स्वचालित *impl = reinterpret_cast<ध्रुव UCaseMapImpl *>(csm);
    प्रत्यागम reinterpret_cast<UBreakIterator *>(impl->iter);
}

U_CAPI शून्य U_EXPORT2
ucasemap_समुच्चयः_Break_Iterator(UCaseMap *csm, UBreakIterator *iter_गन्तव्यम्_Adopt, UError_संहिता *p_Error_संहिता) {
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    स्वचालित *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    विलोपय impl->iter;
    impl->iter=reinterpret_cast<BreakIterator *>(iter_गन्तव्यम्_Adopt);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_गन्तव्यम्_Title(UCaseMap *csm,
                     वर्ण *dest, int32_t dest_क्षमता,
                     ध्रुव वर्ण *src, int32_t src_दीर्घता,
                     UError_संहिता *p_Error_संहिता) {
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    UText utext=UTEXT_INITIALIZER;
    utext_openUTF8(&utext, src, src_दीर्घता, p_Error_संहिता);
    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    स्वचालित *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    यदि(impl->iter==शून्यसूचकः) {
        LocalPointer<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            शून्यसूचकः, impl->locale, impl->options, शून्यसूचकः, ownedIter, *p_Error_संहिता);
        यदि (iter == शून्यसूचकः) {
            utext_close(&utext);
            प्रत्यागम 0;
        }
        impl->iter = ownedIter.orphan();
    }
    impl->iter->समुच्चयः_Text(&utext, *p_Error_संहिता);
    int32_t length=ucasemap_mapUTF8(
            impl->caseLocale, impl->options, impl->iter,
            dest, dest_क्षमता,
            src, src_दीर्घता,
            ucasemap_आन्तरिकम्_UTF8_गन्तव्यम्_Title, शून्यसूचकः, *p_Error_संहिता);
    utext_close(&utext);
    प्रत्यागम length;
}

#यदि_अन्तः  // !UCONFIG_NO_BREAK_ITERATION
