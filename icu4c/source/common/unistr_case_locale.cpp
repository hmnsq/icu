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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucasemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::ਮੰਜ਼ਿਲ_ਹੇਠਲਾ() U_LIFETIME_BOUND {
  ਵਾਪਸ caseMap(ustrcase_getCaseLocale(ਖਾਲੀ_ਸੂਚਕ), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ);
}

UnicodeString&
UnicodeString::ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(ਅਚਲ Locale& locale) U_LIFETIME_BOUND {
  ਵਾਪਸ caseMap(ustrcase_getCaseLocale(locale.get_ਅਧਾਰ_Name()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ);
}

UnicodeString&
UnicodeString::ਮੰਜ਼ਿਲ_ਉੱਤਲਾ() U_LIFETIME_BOUND {
  ਵਾਪਸ caseMap(ustrcase_getCaseLocale(ਖਾਲੀ_ਸੂਚਕ), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ);
}

UnicodeString&
UnicodeString::ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(ਅਚਲ Locale& locale) U_LIFETIME_BOUND {
  ਵਾਪਸ caseMap(ustrcase_getCaseLocale(locale.get_ਅਧਾਰ_Name()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ);
}

U_NAMESPACE_ਸਿਰਾ
