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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/brkiter.h"
#شامل "unicode/locid.h"
#شامل "unicode/ucasemap.h"
#شامل "unicode/unistr.h"
#شامل "ucasemap_imp.h"

U_NAMESPACE_BEGIN

UnicodeString&
UnicodeString::toTitle(BreakIterator* iter) U_LIFETIME_BOUND {
    بازگشت toTitle(iter, Locale::getDefault(), 0);
}

UnicodeString&
UnicodeString::toTitle(BreakIterator* iter, ثابت Locale& locale) U_LIFETIME_BOUND {
    بازگشت toTitle(iter, locale, 0);
}

UnicodeString&
UnicodeString::toTitle(BreakIterator* iter,
                       ثابت Locale& locale,
                       uint32_t options) U_LIFETIME_BOUND {
    LocalPointer<BreakIterator> ownedIter;
    UErrorCode errorCode = U_ZERO_ERROR;
    iter = ustrcase_getTitleBreakIterator(&locale, "", options, iter, ownedIter, errorCode);
    اگر (iter == nullptr) {
        setToBogus();
        بازگشت *این;
    }
    caseMap(ustrcase_getCaseLocale(locale.getBaseName()), options, iter, ustrcase_internalToTitle);
    بازگشت *این;
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_BREAK_ITERATION
