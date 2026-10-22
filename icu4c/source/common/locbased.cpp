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
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    اگر (U_FAILURE(status)) {
        بازگشت Locale::get_ریشه();
    }

    انتخاب(type) {
    حالت ULOC_VALID_LOCALE:
        بازگشت valid;
    حالت ULOC_ACTUAL_LOCALE:
        بازگشت actual;
    پیشفرض:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت Locale::get_ریشه();
    }
}

ثابت نویسه* LocaleBased::getLocaleID(ثابت Locale& valid, ثابت Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    ثابت Locale& locale = getLocale(valid, actual, type, status);

    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }

    بازگشت locale == Locale::get_ریشه() ? k_ریشه_Locale_Name : locale.getName();
}

U_NAMESPACE_END
