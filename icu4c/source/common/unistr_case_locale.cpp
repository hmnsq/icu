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

#ضمن "unicode/utypes.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/ucasemap.h"
#ضمن "unicode/unistr.h"
#ضمن "ucasemap_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::منزل_هيٺاهون() U_LIFETIME_BOUND {
  أرجع caseMap(ustrcase_getCaseLocale(خالي_اشارو), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_اندروني_منزل_هيٺاهون);
}

UnicodeString&
UnicodeString::منزل_هيٺاهون(ثابت Locale& locale) U_LIFETIME_BOUND {
  أرجع caseMap(ustrcase_getCaseLocale(locale.get_بنياد_Name()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_اندروني_منزل_هيٺاهون);
}

UnicodeString&
UnicodeString::منزل_مٿانهون() U_LIFETIME_BOUND {
  أرجع caseMap(ustrcase_getCaseLocale(خالي_اشارو), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_اندروني_منزل_مٿانهون);
}

UnicodeString&
UnicodeString::منزل_مٿانهون(ثابت Locale& locale) U_LIFETIME_BOUND {
  أرجع caseMap(ustrcase_getCaseLocale(locale.get_بنياد_Name()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_اندروني_منزل_مٿانهون);
}

U_NAMESPACE_پڇاڙي
