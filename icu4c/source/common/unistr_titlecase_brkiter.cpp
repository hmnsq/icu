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

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucasemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

UnicodeString&
UnicodeString::toTitle(BreakIterator* iter) U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ toTitle(iter, Locale::getDefault(), 0);
}

UnicodeString&
UnicodeString::toTitle(BreakIterator* iter, ಧ್ರುವ Locale& locale) U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ toTitle(iter, locale, 0);
}

UnicodeString&
UnicodeString::toTitle(BreakIterator* iter,
                       ಧ್ರುವ Locale& locale,
                       uint32_t options) U_LIFETIME_BOUND {
    LocalPointer<BreakIterator> ownedIter;
    UErrorCode errorCode = U_ZERO_ERROR;
    iter = ustrcase_getTitleBreakIterator(&locale, "", options, iter, ownedIter, errorCode);
    ಯದಿ (iter == nullptr) {
        setToBogus();
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    caseMap(ustrcase_getCaseLocale(locale.getBaseName()), options, iter, ustrcase_internalToTitle);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_BREAK_ITERATION
