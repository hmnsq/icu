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
#অন্তর্ভাৱ "locbased.h"
#অন্তর্ভাৱ "uresimp.h"

U_NAMESPACE_BEGIN

ধ্রুৱ Locale& LocaleBased::getLocale(ধ্রুৱ Locale& valid U_LIFETIME_BOUND,
                                     ধ্রুৱ Locale& actual U_LIFETIME_BOUND,
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই Locale::get_মূল();
    }

    ৱিকল্প(type) {
    পক্ষ ULOC_VALID_LOCALE:
        ঘূৰাই valid;
    পক্ষ ULOC_ACTUAL_LOCALE:
        ঘূৰাই actual;
    স্ৱভাৱ:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই Locale::get_মূল();
    }
}

ধ্রুৱ ৱর্ণ* LocaleBased::getLocaleID(ধ্রুৱ Locale& valid, ধ্রুৱ Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    ধ্রুৱ Locale& locale = getLocale(valid, actual, type, status);

    যদি (U_FAILURE(status)) {
        ঘূৰাই nullptr;
    }

    ঘূৰাই locale == Locale::get_মূল() ? k_মূল_Locale_Name : locale.getName();
}

U_NAMESPACE_END
