// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  unistr_titlecase_brkiter.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:2
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
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/ucasemap.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

UnicodeString&
UnicodeString::గమ్యం_Title(BreakIterator* iter) U_LIFETIME_BOUND {
    తిరిగి గమ్యం_Title(iter, Locale::getDefault(), 0);
}

UnicodeString&
UnicodeString::గమ్యం_Title(BreakIterator* iter, స్థిరరాశి Locale& locale) U_LIFETIME_BOUND {
    తిరిగి గమ్యం_Title(iter, locale, 0);
}

UnicodeString&
UnicodeString::గమ్యం_Title(BreakIterator* iter,
                       స్థిరరాశి Locale& locale,
                       uint32_t options) U_LIFETIME_BOUND {
    Local_సూచిక<BreakIterator> ownedIter;
    UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
    iter = ustrcase_getTitleBreakIterator(&locale, "", options, iter, ownedIter, error_సంకేతం);
    ఒకవేళ (iter == శూన్యసూచిక) {
        సమితి_గమ్యం_Bogus();
        తిరిగి *ఇది;
    }
    caseMap(ustrcase_getCaseLocale(locale.get_పునాది_Name()), options, iter, ustrcase_అంతర్గత_గమ్యం_Title);
    తిరిగి *ఇది;
}

U_NAMESPACE_చివర

#అయితే_ముగింపు  // !UCONFIG_NO_BREAK_ITERATION
