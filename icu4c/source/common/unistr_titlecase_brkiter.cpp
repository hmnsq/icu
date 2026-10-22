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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucasemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

UnicodeString&
UnicodeString::ਮੰਜ਼ਿਲ_Title(BreakIterator* iter) U_LIFETIME_BOUND {
    ਵਾਪਸ ਮੰਜ਼ਿਲ_Title(iter, Locale::getDefault(), 0);
}

UnicodeString&
UnicodeString::ਮੰਜ਼ਿਲ_Title(BreakIterator* iter, ਅਚਲ Locale& locale) U_LIFETIME_BOUND {
    ਵਾਪਸ ਮੰਜ਼ਿਲ_Title(iter, locale, 0);
}

UnicodeString&
UnicodeString::ਮੰਜ਼ਿਲ_Title(BreakIterator* iter,
                       ਅਚਲ Locale& locale,
                       uint32_t options) U_LIFETIME_BOUND {
    Local_ਸੂਚਕ<BreakIterator> ownedIter;
    UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
    iter = ustrcase_getTitleBreakIterator(&locale, "", options, iter, ownedIter, error_ਕੋਡ);
    ਜੇ (iter == ਖਾਲੀ_ਸੂਚਕ) {
        ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਵਾਪਸ *ਇਹ;
    }
    caseMap(ustrcase_getCaseLocale(locale.get_ਅਧਾਰ_Name()), options, iter, ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_Title);
    ਵਾਪਸ *ਇਹ;
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // !UCONFIG_NO_BREAK_ITERATION
