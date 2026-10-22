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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/brkiter.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/ucasemap.h"
#ضمن "unicode/unistr.h"
#ضمن "ucasemap_imp.h"

U_NAMESPACE_BEGIN

UnicodeString&
UnicodeString::منزل_Title(BreakIterator* iter) U_LIFETIME_BOUND {
    أرجع منزل_Title(iter, Locale::getDefault(), 0);
}

UnicodeString&
UnicodeString::منزل_Title(BreakIterator* iter, ثابت Locale& locale) U_LIFETIME_BOUND {
    أرجع منزل_Title(iter, locale, 0);
}

UnicodeString&
UnicodeString::منزل_Title(BreakIterator* iter,
                       ثابت Locale& locale,
                       uint32_t options) U_LIFETIME_BOUND {
    Local_اشارہ_گر<BreakIterator> ownedIter;
    UError_کوڈ error_کوڈ = U_ZERO_ERROR;
    iter = ustrcase_getTitleBreakIterator(&locale, "", options, iter, ownedIter, error_کوڈ);
    إذا (iter == خالی_سوچک) {
        سیٹ_منزل_Bogus();
        أرجع *هذا;
    }
    caseMap(ustrcase_getCaseLocale(locale.get_ادھار_Name()), options, iter, ustrcase_اندرونی_منزل_Title);
    أرجع *هذا;
}

U_NAMESPACE_سرا

#جے_انت  // !UCONFIG_NO_BREAK_ITERATION
