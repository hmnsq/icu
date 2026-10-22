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
#ଅନ୍ତର୍ଭାଵ "locbased.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"

U_NAMESPACE_BEGIN

ଅଚଳ Locale& LocaleBased::getLocale(ଅଚଳ Locale& valid U_LIFETIME_BOUND,
                                     ଅଚଳ Locale& actual U_LIFETIME_BOUND,
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ Locale::get_ମୂଳ();
    }

    ବାଛ(type) {
    ସ୍ଥିତି ULOC_VALID_LOCALE:
        ଫେରନ୍ତୁ valid;
    ସ୍ଥିତି ULOC_ACTUAL_LOCALE:
        ଫେରନ୍ତୁ actual;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ Locale::get_ମୂଳ();
    }
}

ଅଚଳ ଅକ୍ଷର* LocaleBased::getLocaleID(ଅଚଳ Locale& valid, ଅଚଳ Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    ଅଚଳ Locale& locale = getLocale(valid, actual, type, status);

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ଫେରନ୍ତୁ locale == Locale::get_ମୂଳ() ? k_ମୂଳ_Locale_Name : locale.getName();
}

U_NAMESPACE_END
