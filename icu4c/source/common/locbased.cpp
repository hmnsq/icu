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
#समावेश "locbased.h"
#समावेश "uresimp.h"

U_NAMESPACE_BEGIN

स्थिर Locale& LocaleBased::getLocale(स्थिर Locale& valid U_LIFETIME_BOUND,
                                     स्थिर Locale& actual U_LIFETIME_BOUND,
                                     ULoc_तथ्य_Locale_Type type,
                                     UErrorCode& status) {
    अगर (U_FAILURE(status)) {
        वापस Locale::get_मूल();
    }

    चयन(type) {
    हाल ULOC_VALID_LOCALE:
        वापस valid;
    हाल ULOC_ACTUAL_LOCALE:
        वापस actual;
    शेष:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस Locale::get_मूल();
    }
}

स्थिर अक्षर* LocaleBased::getLocaleID(स्थिर Locale& valid, स्थिर Locale& actual,
                                     ULoc_तथ्य_Locale_Type type, UErrorCode& status) {
    स्थिर Locale& locale = getLocale(valid, actual, type, status);

    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

    वापस locale == Locale::get_मूल() ? k_मूल_Locale_Name : locale.getName();
}

U_NAMESPACE_END
