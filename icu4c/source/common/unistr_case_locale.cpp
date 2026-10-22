// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  unistr_case_locale.cpp
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
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/ucasemap.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::గమ్యం_క్రింది() U_LIFETIME_BOUND {
  తిరిగి caseMap(ustrcase_getCaseLocale(శూన్యసూచిక), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_అంతర్గత_గమ్యం_క్రింది);
}

UnicodeString&
UnicodeString::గమ్యం_క్రింది(స్థిరరాశి Locale& locale) U_LIFETIME_BOUND {
  తిరిగి caseMap(ustrcase_getCaseLocale(locale.get_పునాది_Name()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_అంతర్గత_గమ్యం_క్రింది);
}

UnicodeString&
UnicodeString::గమ్యం_పై() U_LIFETIME_BOUND {
  తిరిగి caseMap(ustrcase_getCaseLocale(శూన్యసూచిక), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_అంతర్గత_గమ్యం_పై);
}

UnicodeString&
UnicodeString::గమ్యం_పై(స్థిరరాశి Locale& locale) U_LIFETIME_BOUND {
  తిరిగి caseMap(ustrcase_getCaseLocale(locale.get_పునాది_Name()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_అంతర్గత_గమ్యం_పై);
}

U_NAMESPACE_చివర
