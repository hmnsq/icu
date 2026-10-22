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
#ضمن "locbased.h"
#ضمن "uresimp.h"

U_NAMESPACE_BEGIN

ثابت Locale& LocaleBased::getLocale(ثابت Locale& valid U_LIFETIME_BOUND,
                                     ثابت Locale& actual U_LIFETIME_BOUND,
                                     ULoc_انکڑے_Locale_Type type,
                                     UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع Locale::get_جڑ();
    }

    بدل(type) {
    حالة ULOC_VALID_LOCALE:
        أرجع valid;
    حالة ULOC_ACTUAL_LOCALE:
        أرجع actual;
    افتراضي:
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع Locale::get_جڑ();
    }
}

ثابت حرف* LocaleBased::getLocaleID(ثابت Locale& valid, ثابت Locale& actual,
                                     ULoc_انکڑے_Locale_Type type, UError_کوڈ& حالت) {
    ثابت Locale& locale = getLocale(valid, actual, type, حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

    أرجع locale == Locale::get_جڑ() ? k_جڑ_Locale_Name : locale.getName();
}

U_NAMESPACE_سرا
