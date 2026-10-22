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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/brkiter.h"
#شامل "unicode/locid.h"
#شامل "unicode/ucasemap.h"
#شامل "unicode/unistr.h"
#شامل "ucasemap_imp.h"

U_NAMESPACE_BEGIN

UnicodeString&
UnicodeString::مقصد_Title(BreakIterator* iter) U_LIFETIME_BOUND {
    واپس مقصد_Title(iter, Locale::getDefault(), 0);
}

UnicodeString&
UnicodeString::مقصد_Title(BreakIterator* iter, ثابت Locale& locale) U_LIFETIME_BOUND {
    واپس مقصد_Title(iter, locale, 0);
}

UnicodeString&
UnicodeString::مقصد_Title(BreakIterator* iter,
                       ثابت Locale& locale,
                       uint32_t options) U_LIFETIME_BOUND {
    Local_اشارہ_گر<BreakIterator> ownedIter;
    UError_رمز error_رمز = U_ZERO_ERROR;
    iter = ustrcase_getTitleBreakIterator(&locale, "", options, iter, ownedIter, error_رمز);
    اگر (iter == خالی_اشاریہ) {
        سیٹ_مقصد_Bogus();
        واپس *یہ;
    }
    caseMap(ustrcase_getCaseLocale(locale.get_بنیاد_Name()), options, iter, ustrcase_اندرونی_مقصد_Title);
    واپس *یہ;
}

U_NAMESPACE_انتہا

#اگر_ختم  // !UCONFIG_NO_BREAK_ITERATION
