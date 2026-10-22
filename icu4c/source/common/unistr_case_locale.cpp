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

#شامل "unicode/utypes.h"
#شامل "unicode/locid.h"
#شامل "unicode/ucasemap.h"
#شامل "unicode/unistr.h"
#شامل "ucasemap_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::مقصد_زیریں() U_LIFETIME_BOUND {
  واپس caseMap(ustrcase_getCaseLocale(خالی_اشاریہ), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_اندرونی_مقصد_زیریں);
}

UnicodeString&
UnicodeString::مقصد_زیریں(ثابت Locale& locale) U_LIFETIME_BOUND {
  واپس caseMap(ustrcase_getCaseLocale(locale.get_بنیاد_Name()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_اندرونی_مقصد_زیریں);
}

UnicodeString&
UnicodeString::مقصد_بالائی() U_LIFETIME_BOUND {
  واپس caseMap(ustrcase_getCaseLocale(خالی_اشاریہ), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_اندرونی_مقصد_بالائی);
}

UnicodeString&
UnicodeString::مقصد_بالائی(ثابت Locale& locale) U_LIFETIME_BOUND {
  واپس caseMap(ustrcase_getCaseLocale(locale.get_بنیاد_Name()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_اندرونی_مقصد_بالائی);
}

U_NAMESPACE_انتہا
