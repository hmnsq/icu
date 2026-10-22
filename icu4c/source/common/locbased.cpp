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
                                     ULoc_दत्त_Locale_Type type,
                                     UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम Locale::get_मूलम्();
    }

    विकल्प(type) {
    पक्ष ULOC_VALID_LOCALE:
        प्रत्यागम valid;
    पक्ष ULOC_ACTUAL_LOCALE:
        प्रत्यागम actual;
    स्वभाव:
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम Locale::get_मूलम्();
    }
}

ध्रुव वर्ण* LocaleBased::getLocaleID(ध्रुव Locale& valid, ध्रुव Locale& actual,
                                     ULoc_दत्त_Locale_Type type, UError_संहिता& स्थितिः) {
    ध्रुव Locale& locale = getLocale(valid, actual, type, स्थितिः);

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    प्रत्यागम locale == Locale::get_मूलम्() ? k_मूलम्_Locale_Name : locale.getName();
}

U_NAMESPACE_अन्तः
