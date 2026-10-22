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

अचर Locale& LocaleBased::getLocale(अचर Locale& valid U_LIFETIME_BOUND,
                                     अचर Locale& actual U_LIFETIME_BOUND,
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ Locale::get_जरा();
    }

    विकल्प(type) {
    पक्ष ULOC_VALID_LOCALE:
        फर्काऊ valid;
    पक्ष ULOC_ACTUAL_LOCALE:
        फर्काऊ actual;
    स्वभाव:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ Locale::get_जरा();
    }
}

अचर वर्ण* LocaleBased::getLocaleID(अचर Locale& valid, अचर Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    अचर Locale& locale = getLocale(valid, actual, type, status);

    यदि (U_FAILURE(status)) {
        फर्काऊ nullptr;
    }

    फर्काऊ locale == Locale::get_जरा() ? k_जरा_Locale_Name : locale.getName();
}

U_NAMESPACE_END
