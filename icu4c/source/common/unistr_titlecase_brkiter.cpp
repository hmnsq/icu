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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/ucasemap.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "ucasemap_imp.h"

U_NAMESPACE_BEGIN

UnicodeString&
UnicodeString::toTitle(BreakIterator* iter) U_LIFETIME_BOUND {
    തിരികെ toTitle(iter, Locale::getDefault(), 0);
}

UnicodeString&
UnicodeString::toTitle(BreakIterator* iter, സ്ഥിരം Locale& locale) U_LIFETIME_BOUND {
    തിരികെ toTitle(iter, locale, 0);
}

UnicodeString&
UnicodeString::toTitle(BreakIterator* iter,
                       സ്ഥിരം Locale& locale,
                       uint32_t options) U_LIFETIME_BOUND {
    LocalPointer<BreakIterator> ownedIter;
    UErrorCode errorCode = U_ZERO_ERROR;
    iter = ustrcase_getTitleBreakIterator(&locale, "", options, iter, ownedIter, errorCode);
    എങ്കിൽ (iter == nullptr) {
        setToBogus();
        തിരികെ *ഇത്;
    }
    caseMap(ustrcase_getCaseLocale(locale.getBaseName()), options, iter, ustrcase_internalToTitle);
    തിരികെ *ഇത്;
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_BREAK_ITERATION
