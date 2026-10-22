// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ustr_titlecase_brkiter.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011may30
*   created by: Markus W. Scherer
*
*   Titlecasing functions that are based on BreakIterator
*   were moved here to break dependency cycles among parts of the common library.
*/

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/casemap.h"
#అన్తర్భావ "unicode/chariter.h"
#అన్తర్భావ "unicode/localpointer.h"
#అన్తర్భావ "unicode/ubrk.h"
#అన్తర్భావ "unicode/ucasemap.h"
#అన్తర్భావ "unicode/utext.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucase.h"
#అన్తర్భావ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

/**
 * Whole-string BreakIterator.
 * Titlecasing only calls setText(), first(), and next().
 * We implement the rest only to satisfy the abstract interface.
 */
తరగతి సంపూర్ణ_String_Break_Iterator : బహిరంగ BreakIterator {
బహిరంగ:
    సంపూర్ణ_String_Break_Iterator() : BreakIterator(), length(0) {}
    ~సంపూర్ణ_String_Break_Iterator() override;
    తార్కిక క్రియాకారకం==(స్థిరరాశి BreakIterator&) స్థిరరాశి override;
    సంపూర్ణ_String_Break_Iterator *clone() స్థిరరాశి override;
    స్థిర UClassID U_EXPORT2 getStaticClassID();
    UClassID getDynamicClassID() స్థిరరాశి override;
    CharacterIterator& get_పాఠ్యం() స్థిరరాశి U_LIFETIME_BOUND override;
    UText* getUText(UText* fill_లోపల U_LIFETIME_BOUND, UError_సంకేతం& error_సంకేతం) స్థిరరాశి override;
    శూన్య  సమితి_పాఠ్యం(స్థిరరాశి UnicodeString &పాఠ్యం) override;
    శూన్య  సమితి_పాఠ్యం(UText *పాఠ్యం, UError_సంకేతం &error_సంకేతం) override;
    శూన్య  adopt_పాఠ్యం(CharacterIterator* it) override;
    int32_t first() override;
    int32_t చివరి() override;
    int32_t మునుపటి() override;
    int32_t next() override;
    int32_t current() స్థిరరాశి override;
    int32_t following(int32_t స్థానభ్రంశం) override;
    int32_t preceding(int32_t స్థానభ్రంశం) override;
    UBool isBoundary(int32_t స్థానభ్రంశం) override;
    int32_t next(int32_t n) override;
    సంపూర్ణ_String_Break_Iterator *create_బఫరు_Clone(శూన్య *దొంతర_బఫరు, int32_t &బఫరు_కొలత_2,
                                                UError_సంకేతం &error_సంకేతం) override;
    సంపూర్ణ_String_Break_Iterator& refresh_ఆదానం_పాఠ్యం(UText* input,
                                               UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND override;

అంతరంగ:
    int32_t length;
};

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(సంపూర్ణ_String_Break_Iterator)

సంపూర్ణ_String_Break_Iterator::~సంపూర్ణ_String_Break_Iterator() {}
తార్కిక సంపూర్ణ_String_Break_Iterator::క్రియాకారకం==(స్థిరరాశి BreakIterator&) స్థిరరాశి { తిరిగి అబద్ధం; }
సంపూర్ణ_String_Break_Iterator *సంపూర్ణ_String_Break_Iterator::clone() స్థిరరాశి { తిరిగి శూన్యసూచిక; }

CharacterIterator& సంపూర్ణ_String_Break_Iterator::get_పాఠ్యం() స్థిరరాశి U_LIFETIME_BOUND {
    UPRV_UNREACHABLE_EXIT;  // really should not be called
}
UText *సంపూర్ణ_String_Break_Iterator::getUText(UText * /*fillIn*/, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
        error_సంకేతం = U_UNSUPPORTED_ERROR;
    }
    తిరిగి శూన్యసూచిక;
}

శూన్య  సంపూర్ణ_String_Break_Iterator::సమితి_పాఠ్యం(స్థిరరాశి UnicodeString &పాఠ్యం) {
    length = పాఠ్యం.length();
}
శూన్య  సంపూర్ణ_String_Break_Iterator::సమితి_పాఠ్యం(UText *పాఠ్యం, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
        int64_t length64 = utext_native_నిడివి(పాఠ్యం);
        ఒకవేళ (length64 <= INT32_MAX) {
            length = static_cast<int32_t>(length64);
        } లేకపోతే {
            error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
        }
    }
}
శూన్య  సంపూర్ణ_String_Break_Iterator::adopt_పాఠ్యం(CharacterIterator*) {
    UPRV_UNREACHABLE_EXIT;  // should not be called
}

int32_t సంపూర్ణ_String_Break_Iterator::first() { తిరిగి 0; }
int32_t సంపూర్ణ_String_Break_Iterator::చివరి() { తిరిగి length; }
int32_t సంపూర్ణ_String_Break_Iterator::మునుపటి() { తిరిగి 0; }
int32_t సంపూర్ణ_String_Break_Iterator::next() { తిరిగి length; }
int32_t సంపూర్ణ_String_Break_Iterator::current() స్థిరరాశి { తిరిగి 0; }
int32_t సంపూర్ణ_String_Break_Iterator::following(int32_t /*offset*/) { తిరిగి length; }
int32_t సంపూర్ణ_String_Break_Iterator::preceding(int32_t /*offset*/) { తిరిగి 0; }
UBool సంపూర్ణ_String_Break_Iterator::isBoundary(int32_t /*offset*/) { తిరిగి అబద్ధం; }
int32_t సంపూర్ణ_String_Break_Iterator::next(int32_t /*n*/) { తిరిగి length; }

సంపూర్ణ_String_Break_Iterator *సంపూర్ణ_String_Break_Iterator::create_బఫరు_Clone(
        శూన్య * /*stackBuffer*/, int32_t & /*BufferSize*/, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
        error_సంకేతం = U_UNSUPPORTED_ERROR;
    }
    తిరిగి శూన్యసూచిక;
}
సంపూర్ణ_String_Break_Iterator&
సంపూర్ణ_String_Break_Iterator::refresh_ఆదానం_పాఠ్యం(UText* /*ఆదానం*/,
                                           UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
        error_సంకేతం = U_UNSUPPORTED_ERROR;
    }
    తిరిగి *ఇది;
}

U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(స్థిరరాశి Locale* locale,
                                              స్థిరరాశి అక్షరం* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              Local_సూచిక<BreakIterator>& ownedIter,
                                              UError_సంకేతం& error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    options &= U_TITLECASE_ITERATOR_ముసుగు;
    ఒకవేళ (options != 0 && iter != శూన్యసూచిక) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (iter == శూన్యసూచిక) {
        ఎంపిక (options) {
        స్థితి 0:
            iter = BreakIterator::create_మాట_Instance(
                locale != శూన్యసూచిక ? *locale : Locale(locID), error_సంకేతం);
            ఆపు;
        స్థితి U_TITLECASE_సంపూర్ణ_STRING:
            iter = కొత్త సంపూర్ణ_String_Break_Iterator();
            ఒకవేళ (iter == శూన్యసూచిక) {
                error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            }
            ఆపు;
        స్థితి U_TITLECASE_SENTENCES:
            iter = BreakIterator::createSentenceInstance(
                locale != శూన్యసూచిక ? *locale : Locale(locID), error_సంకేతం);
            ఆపు;
        అప్రమేయ:
            error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
            ఆపు;
        }
        ownedIter.adoptInstead(iter);
    }
    తిరిగి iter;
}

int32_t CaseMap::గమ్యం_Title(
        స్థిరరాశి అక్షరం *locale, uint32_t options, BreakIterator *iter,
        స్థిరరాశి char16_t *src, int32_t src_నిడివి,
        char16_t *dest, int32_t dest_సామర్థ్యం, Edits *edits,
        UError_సంకేతం &error_సంకేతం) {
    Local_సూచిక<BreakIterator> ownedIter;
    iter = ustrcase_getTitleBreakIterator(శూన్యసూచిక, locale, options, iter, ownedIter, error_సంకేతం);
    ఒకవేళ(iter==శూన్యసూచిక) {
        తిరిగి 0;
    }
    UnicodeString s(src_నిడివి<0, src, src_నిడివి);
    iter->సమితి_పాఠ్యం(s);
    తిరిగి ustrcase_map(
        ustrcase_getCaseLocale(locale), options, iter,
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ustrcase_అంతర్గత_గమ్యం_Title, edits, error_సంకేతం);
}

U_NAMESPACE_చివర

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_str_గమ్యం_Title(char16_t *dest, int32_t dest_సామర్థ్యం,
             స్థిరరాశి char16_t *src, int32_t src_నిడివి,
             UBreakIterator *titleIter,
             స్థిరరాశి అక్షరం *locale,
             UError_సంకేతం *p_Error_సంకేతం) {
    Local_సూచిక<BreakIterator> ownedIter;
    BreakIterator *iter = ustrcase_getTitleBreakIterator(
        శూన్యసూచిక, locale, 0, reinterpret_cast<BreakIterator *>(titleIter),
        ownedIter, *p_Error_సంకేతం);
    ఒకవేళ (iter == శూన్యసూచిక) {
        తిరిగి 0;
    }
    UnicodeString s(src_నిడివి<0, src, src_నిడివి);
    iter->సమితి_పాఠ్యం(s);
    తిరిగి ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, iter,
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ustrcase_అంతర్గత_గమ్యం_Title, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
ucasemap_గమ్యం_Title(UCaseMap *csm,
                 char16_t *dest, int32_t dest_సామర్థ్యం,
                 స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                 UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    స్వయంచాలక *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ఒకవేళ (impl->iter == శూన్యసూచిక) {
        Local_సూచిక<BreakIterator> ownedIter;
        BreakIterator *iter = ustrcase_getTitleBreakIterator(
            శూన్యసూచిక, impl->locale, impl->options, శూన్యసూచిక, ownedIter, *p_Error_సంకేతం);
        ఒకవేళ (iter == శూన్యసూచిక) {
            తిరిగి 0;
        }
        impl->iter = ownedIter.orphan();
    }
    UnicodeString s(src_నిడివి<0, src, src_నిడివి);
    impl->iter->సమితి_పాఠ్యం(s);
    తిరిగి ustrcase_map(
        impl->caseLocale, impl->options, impl->iter,
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ustrcase_అంతర్గత_గమ్యం_Title, శూన్యసూచిక, *p_Error_సంకేతం);
}

#అయితే_ముగింపు  // !UCONFIG_NO_BREAK_ITERATION
