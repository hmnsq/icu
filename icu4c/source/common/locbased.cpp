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
                                     ULoc_داده_Locale_Type type,
                                     UError_کد& وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع Locale::get_ریشه();
    }

    بدل(type) {
    حالة ULOC_VALID_LOCALE:
        أرجع valid;
    حالة ULOC_ACTUAL_LOCALE:
        أرجع actual;
    افتراضي:
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع Locale::get_ریشه();
    }
}

ثابت حرف* LocaleBased::getLocaleID(ثابت Locale& valid, ثابت Locale& actual,
                                     ULoc_داده_Locale_Type type, UError_کد& وضعیت) {
    ثابت Locale& locale = getLocale(valid, actual, type, وضعیت);

    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }

    أرجع locale == Locale::get_ریشه() ? k_ریشه_Locale_Name : locale.getName();
}

U_NAMESPACE_انتها
