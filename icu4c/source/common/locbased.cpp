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
#अन्तर्भाव "locbased.h"
#अन्तर्भाव "uresimp.h"

U_NAMESPACE_BEGIN

स्थिरांक Locale& LocaleBased::getLocale(स्थिरांक Locale& valid U_LIFETIME_BOUND,
                                     स्थिरांक Locale& actual U_LIFETIME_BOUND,
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    जर (U_FAILURE(status)) {
        परत Locale::get_मूळ();
    }

    निवड(type) {
    स्थिती ULOC_VALID_LOCALE:
        परत valid;
    स्थिती ULOC_ACTUAL_LOCALE:
        परत actual;
    पूर्वनिर्धारित:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        परत Locale::get_मूळ();
    }
}

स्थिरांक अक्षर* LocaleBased::getLocaleID(स्थिरांक Locale& valid, स्थिरांक Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    स्थिरांक Locale& locale = getLocale(valid, actual, type, status);

    जर (U_FAILURE(status)) {
        परत nullptr;
    }

    परत locale == Locale::get_मूळ() ? k_मूळ_Locale_Name : locale.getName();
}

U_NAMESPACE_END
