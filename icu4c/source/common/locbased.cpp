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
                                     ULoc_معلومات_Locale_Type type,
                                     UError_ڪوڊ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع Locale::get_پاڙ();
    }

    بدل(type) {
    حالة ULOC_VALID_LOCALE:
        أرجع valid;
    حالة ULOC_ACTUAL_LOCALE:
        أرجع actual;
    افتراضي:
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع Locale::get_پاڙ();
    }
}

ثابت حرف* LocaleBased::getLocaleID(ثابت Locale& valid, ثابت Locale& actual,
                                     ULoc_معلومات_Locale_Type type, UError_ڪوڊ& حالت) {
    ثابت Locale& locale = getLocale(valid, actual, type, حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }

    أرجع locale == Locale::get_پاڙ() ? k_پاڙ_Locale_Name : locale.getName();
}

U_NAMESPACE_پڇاڙي
