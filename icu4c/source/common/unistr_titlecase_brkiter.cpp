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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucasemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

UnicodeString&
UnicodeString::ಗಮ್ಯ_Title(BreakIterator* iter) U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ ಗಮ್ಯ_Title(iter, Locale::getDefault(), 0);
}

UnicodeString&
UnicodeString::ಗಮ್ಯ_Title(BreakIterator* iter, ಸ್ಥಿರಾಂಕ Locale& locale) U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ ಗಮ್ಯ_Title(iter, locale, 0);
}

UnicodeString&
UnicodeString::ಗಮ್ಯ_Title(BreakIterator* iter,
                       ಸ್ಥಿರಾಂಕ Locale& locale,
                       uint32_t options) U_LIFETIME_BOUND {
    Local_ಸೂಚಕ<BreakIterator> ownedIter;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
    iter = ustrcase_getTitleBreakIterator(&locale, "", options, iter, ownedIter, error_ಸಂಕೇತ);
    ಆದರೆ (iter == ಶೂನ್ಯಸೂಚಕ) {
        ಗಣ_ಗಮ್ಯ_Bogus();
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    caseMap(ustrcase_getCaseLocale(locale.get_ತಳ_Name()), options, iter, ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_Title);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // !UCONFIG_NO_BREAK_ITERATION
