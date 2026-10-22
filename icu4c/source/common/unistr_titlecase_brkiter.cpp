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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/ucasemap.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "ucasemap_imp.h"

U_NAMESPACE_BEGIN

UnicodeString&
UnicodeString::गन्तव्यम्_Title(BreakIterator* iter) U_LIFETIME_BOUND {
    प्रत्यागम गन्तव्यम्_Title(iter, Locale::getDefault(), 0);
}

UnicodeString&
UnicodeString::गन्तव्यम्_Title(BreakIterator* iter, ध्रुव Locale& locale) U_LIFETIME_BOUND {
    प्रत्यागम गन्तव्यम्_Title(iter, locale, 0);
}

UnicodeString&
UnicodeString::गन्तव्यम्_Title(BreakIterator* iter,
                       ध्रुव Locale& locale,
                       uint32_t options) U_LIFETIME_BOUND {
    LocalPointer<BreakIterator> ownedIter;
    UError_संहिता error_संहिता = U_ZERO_ERROR;
    iter = ustrcase_getTitleBreakIterator(&locale, "", options, iter, ownedIter, error_संहिता);
    यदि (iter == शून्यसूचकः) {
        समुच्चयः_गन्तव्यम्_Bogus();
        प्रत्यागम *एतत्;
    }
    caseMap(ustrcase_getCaseLocale(locale.get_आधारः_Name()), options, iter, ustrcase_आन्तरिकम्_गन्तव्यम्_Title);
    प्रत्यागम *एतत्;
}

U_NAMESPACE_अन्तः

#यदि_अन्तः  // !UCONFIG_NO_BREAK_ITERATION
