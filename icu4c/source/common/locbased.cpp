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
#شامل "locbased.h"
#شامل "uresimp.h"

U_NAMESPACE_BEGIN

ثابت Locale& LocaleBased::getLocale(ثابت Locale& valid U_LIFETIME_BOUND,
                                     ثابت Locale& actual U_LIFETIME_BOUND,
                                     ULoc_ڈیٹا_Locale_Type type,
                                     UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس Locale::get_جڑ();
    }

    انتخاب(type) {
    حالت ULOC_VALID_LOCALE:
        واپس valid;
    حالت ULOC_ACTUAL_LOCALE:
        واپس actual;
    طےشدہ:
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس Locale::get_جڑ();
    }
}

ثابت حرف* LocaleBased::getLocaleID(ثابت Locale& valid, ثابت Locale& actual,
                                     ULoc_ڈیٹا_Locale_Type type, UError_رمز& حالت_2) {
    ثابت Locale& locale = getLocale(valid, actual, type, حالت_2);

    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

    واپس locale == Locale::get_جڑ() ? k_جڑ_Locale_Name : locale.getName();
}

U_NAMESPACE_انتہا
