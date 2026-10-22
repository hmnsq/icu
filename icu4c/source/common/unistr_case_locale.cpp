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
UnicodeString::ಗಮ್ಯ_ಅಧೋ() U_LIFETIME_BOUND {
  ಹಿಂತಿರುಗಿ caseMap(ustrcase_getCaseLocale(ಶೂನ್ಯಸೂಚಕ), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_ಅಧೋ);
}

UnicodeString&
UnicodeString::ಗಮ್ಯ_ಅಧೋ(ಸ್ಥಿರಾಂಕ Locale& locale) U_LIFETIME_BOUND {
  ಹಿಂತಿರುಗಿ caseMap(ustrcase_getCaseLocale(locale.get_ತಳ_Name()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_ಅಧೋ);
}

UnicodeString&
UnicodeString::ಗಮ್ಯ_ಊರ್ಧ್ವ() U_LIFETIME_BOUND {
  ಹಿಂತಿರುಗಿ caseMap(ustrcase_getCaseLocale(ಶೂನ್ಯಸೂಚಕ), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_ಊರ್ಧ್ವ);
}

UnicodeString&
UnicodeString::ಗಮ್ಯ_ಊರ್ಧ್ವ(ಸ್ಥಿರಾಂಕ Locale& locale) U_LIFETIME_BOUND {
  ಹಿಂತಿರುಗಿ caseMap(ustrcase_getCaseLocale(locale.get_ತಳ_Name()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_ಊರ್ಧ್ವ);
}

U_NAMESPACE_ಕೊನೆ
