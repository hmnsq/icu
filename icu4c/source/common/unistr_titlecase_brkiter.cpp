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
UnicodeString::مقصد_Title(BreakIterator* iter) U_LIFETIME_BOUND {
    أرجع مقصد_Title(iter, Locale::getDefault(), 0);
}

UnicodeString&
UnicodeString::مقصد_Title(BreakIterator* iter, ثابت Locale& locale) U_LIFETIME_BOUND {
    أرجع مقصد_Title(iter, locale, 0);
}

UnicodeString&
UnicodeString::مقصد_Title(BreakIterator* iter,
                       ثابت Locale& locale,
                       uint32_t options) U_LIFETIME_BOUND {
    Local_اشاره_گر<BreakIterator> ownedIter;
    UError_کد error_کد = U_ZERO_ERROR;
    iter = ustrcase_getTitleBreakIterator(&locale, "", options, iter, ownedIter, error_کد);
    إذا (iter == اشاره_تهی) {
        مجموعه_مقصد_Bogus();
        أرجع *هذا;
    }
    caseMap(ustrcase_getCaseLocale(locale.get_پایه_Name()), options, iter, ustrcase_داخلی_مقصد_Title);
    أرجع *هذا;
}

U_NAMESPACE_انتها

#پایان_اگر  // !UCONFIG_NO_BREAK_ITERATION
