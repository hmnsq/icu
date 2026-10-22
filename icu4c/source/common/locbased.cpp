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
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    إذا (U_FAILURE(status)) {
        أرجع Locale::get_اکر();
    }

    بدل(type) {
    حالة ULOC_VALID_LOCALE:
        أرجع valid;
    حالة ULOC_ACTUAL_LOCALE:
        أرجع actual;
    افتراضي:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع Locale::get_اکر();
    }
}

ثابت حرف* LocaleBased::getLocaleID(ثابت Locale& valid, ثابت Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    ثابت Locale& locale = getLocale(valid, actual, type, status);

    إذا (U_FAILURE(status)) {
        أرجع nullptr;
    }

    أرجع locale == Locale::get_اکر() ? k_اکر_Locale_Name : locale.getName();
}

U_NAMESPACE_END
