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
#অন্তর্ভাব "locbased.h"
#অন্তর্ভাব "uresimp.h"

U_NAMESPACE_BEGIN

ধ্রুব Locale& LocaleBased::getLocale(ধ্রুব Locale& valid U_LIFETIME_BOUND,
                                     ধ্রুব Locale& actual U_LIFETIME_BOUND,
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        প্রত্যাগম Locale::get_মূল();
    }

    বিকল্প(type) {
    পক্ষ ULOC_VALID_LOCALE:
        প্রত্যাগম valid;
    পক্ষ ULOC_ACTUAL_LOCALE:
        প্রত্যাগম actual;
    স্বভাব:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম Locale::get_মূল();
    }
}

ধ্রুব বর্ণ* LocaleBased::getLocaleID(ধ্রুব Locale& valid, ধ্রুব Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    ধ্রুব Locale& locale = getLocale(valid, actual, type, status);

    যদি (U_FAILURE(status)) {
        প্রত্যাগম nullptr;
    }

    প্রত্যাগম locale == Locale::get_মূল() ? k_মূল_Locale_Name : locale.getName();
}

U_NAMESPACE_END
