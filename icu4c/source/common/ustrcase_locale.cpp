// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ustrcase_locale.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011may31
*   created by: Markus W. Scherer
*
*   Locale-sensitive case mapping functions (ones that call uloc_getDefault())
*   were moved here to break dependency cycles among parts of the common library.
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/casemap.h"
#అన్తర్భావ "unicode/ucasemap.h"
#అన్తర్భావ "unicode/uloc.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "ucase.h"
#అన్తర్భావ "ucasemap_imp.h"

U_CFUNC int32_t
ustrcase_getCaseLocale(స్థిరరాశి అక్షరం *locale) {
    ఒకవేళ (locale == శూన్యసూచిక) {
        locale = uloc_getDefault();
    }
    ఒకవేళ (*locale == 0) {
        తిరిగి UCASE_LOC_మూలం;
    } లేకపోతే {
        తిరిగి ucase_getCaseLocale(locale);
    }
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_str_గమ్యం_క్రింది(char16_t *dest, int32_t dest_సామర్థ్యం,
             స్థిరరాశి char16_t *src, int32_t src_నిడివి,
             స్థిరరాశి అక్షరం *locale,
             UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ustrcase_అంతర్గత_గమ్యం_క్రింది, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
u_str_గమ్యం_పై(char16_t *dest, int32_t dest_సామర్థ్యం,
             స్థిరరాశి char16_t *src, int32_t src_నిడివి,
             స్థిరరాశి అక్షరం *locale,
             UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి ustrcase_mapWithOverlap(
        ustrcase_getCaseLocale(locale), 0, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ustrcase_అంతర్గత_గమ్యం_పై, *p_Error_సంకేతం);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::గమ్యం_క్రింది(
        స్థిరరాశి అక్షరం *locale, uint32_t options,
        స్థిరరాశి char16_t *src, int32_t src_నిడివి,
        char16_t *dest, int32_t dest_సామర్థ్యం, Edits *edits,
        UError_సంకేతం &error_సంకేతం) {
    తిరిగి ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ustrcase_అంతర్గత_గమ్యం_క్రింది, edits, error_సంకేతం);
}

int32_t CaseMap::గమ్యం_పై(
        స్థిరరాశి అక్షరం *locale, uint32_t options,
        స్థిరరాశి char16_t *src, int32_t src_నిడివి,
        char16_t *dest, int32_t dest_సామర్థ్యం, Edits *edits,
        UError_సంకేతం &error_సంకేతం) {
    తిరిగి ustrcase_map(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ustrcase_అంతర్గత_గమ్యం_పై, edits, error_సంకేతం);
}

U_NAMESPACE_చివర
