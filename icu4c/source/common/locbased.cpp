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

ধ্রুবক Locale& LocaleBased::getLocale(ধ্রুবক Locale& valid U_LIFETIME_BOUND,
                                     ধ্রুবক Locale& actual U_LIFETIME_BOUND,
                                     ULoc_তথ্য_Locale_Type type,
                                     UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ফেরত Locale::get_মূল();
    }

    নির্বাচন(type) {
    ক্ষেত্র ULOC_VALID_LOCALE:
        ফেরত valid;
    ক্ষেত্র ULOC_ACTUAL_LOCALE:
        ফেরত actual;
    পূর্বনির্ধারিত:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত Locale::get_মূল();
    }
}

ধ্রুবক অক্ষর* LocaleBased::getLocaleID(ধ্রুবক Locale& valid, ধ্রুবক Locale& actual,
                                     ULoc_তথ্য_Locale_Type type, UErrorCode& status) {
    ধ্রুবক Locale& locale = getLocale(valid, actual, type, status);

    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }

    ফেরত locale == Locale::get_মূল() ? k_মূল_Locale_Name : locale.getName();
}

U_NAMESPACE_END
