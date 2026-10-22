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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/ucasemap.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "ucasemap_imp.h"

U_NAMESPACE_BEGIN

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::गन्तव्यम्_अधस्तनम्() U_LIFETIME_BOUND {
  प्रत्यागम caseMap(ustrcase_getCaseLocale(शून्यसूचकः), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_आन्तरिकम्_गन्तव्यम्_अधस्तनम्);
}

UnicodeString&
UnicodeString::गन्तव्यम्_अधस्तनम्(ध्रुव Locale& locale) U_LIFETIME_BOUND {
  प्रत्यागम caseMap(ustrcase_getCaseLocale(locale.get_आधारः_Name()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_आन्तरिकम्_गन्तव्यम्_अधस्तनम्);
}

UnicodeString&
UnicodeString::गन्तव्यम्_ऊर्ध्वस्थम्() U_LIFETIME_BOUND {
  प्रत्यागम caseMap(ustrcase_getCaseLocale(शून्यसूचकः), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_आन्तरिकम्_गन्तव्यम्_ऊर्ध्वस्थम्);
}

UnicodeString&
UnicodeString::गन्तव्यम्_ऊर्ध्वस्थम्(ध्रुव Locale& locale) U_LIFETIME_BOUND {
  प्रत्यागम caseMap(ustrcase_getCaseLocale(locale.get_आधारः_Name()), 0,
                 UCASEMAP_BREAK_ITERATOR_NULL ustrcase_आन्तरिकम्_गन्तव्यम्_ऊर्ध्वस्थम्);
}

U_NAMESPACE_अन्तः
