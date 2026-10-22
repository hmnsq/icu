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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucasemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::toLower() U_LIFETIME_BOUND {
  ಪ್ರತ್ಯಾಗಮ caseMap(ustrcase_getCaseLocale(nullptr), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalToLower);
}

UnicodeString&
UnicodeString::toLower(ಧ್ರುವ Locale& locale) U_LIFETIME_BOUND {
  ಪ್ರತ್ಯಾಗಮ caseMap(ustrcase_getCaseLocale(locale.getBaseName()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalToLower);
}

UnicodeString&
UnicodeString::toUpper() U_LIFETIME_BOUND {
  ಪ್ರತ್ಯಾಗಮ caseMap(ustrcase_getCaseLocale(nullptr), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalToUpper);
}

UnicodeString&
UnicodeString::toUpper(ಧ್ರುವ Locale& locale) U_LIFETIME_BOUND {
  ಪ್ರತ್ಯಾಗಮ caseMap(ustrcase_getCaseLocale(locale.getBaseName()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_internalToUpper);
}

U_NAMESPACE_END
