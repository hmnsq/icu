// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2004-2014, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* Author: Alan Liu
* Created: January 16 2004
* Since: ICU 2.8
**********************************************************************
*/
#अन्तर्भाव "locbased.h"
#अन्तर्भाव "uresimp.h"

U_NAMESPACE_BEGIN

ध्रुव Locale& LocaleBased::getLocale(ध्रुव Locale& valid U_LIFETIME_BOUND,
                                     ध्रुव Locale& actual U_LIFETIME_BOUND,
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त Locale::get_मूलम्();
    }

    विकल्प(type) {
    पक्ष ULOC_VALID_LOCALE:
        पटिनिवत्त valid;
    पक्ष ULOC_ACTUAL_LOCALE:
        पटिनिवत्त actual;
    स्वभाव:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त Locale::get_मूलम्();
    }
}

ध्रुव वर्ण* LocaleBased::getLocaleID(ध्रुव Locale& valid, ध्रुव Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    ध्रुव Locale& locale = getLocale(valid, actual, type, status);

    सचे (U_FAILURE(status)) {
        पटिनिवत्त nullptr;
    }

    पटिनिवत्त locale == Locale::get_मूलम्() ? k_मूलम्_Locale_Name : locale.getName();
}

U_NAMESPACE_END
