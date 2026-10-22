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
#અન્તર્ભાવ "locbased.h"
#અન્તર્ભાવ "uresimp.h"

U_NAMESPACE_BEGIN

અચલ Locale& LocaleBased::getLocale(અચલ Locale& valid U_LIFETIME_BOUND,
                                     અચલ Locale& actual U_LIFETIME_BOUND,
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    જો (U_FAILURE(status)) {
        પાછા Locale::get_મૂળ();
    }

    પસંદગી(type) {
    સ્થિતિ ULOC_VALID_LOCALE:
        પાછા valid;
    સ્થિતિ ULOC_ACTUAL_LOCALE:
        પાછા actual;
    મૂળભૂત:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા Locale::get_મૂળ();
    }
}

અચલ અક્ષર* LocaleBased::getLocaleID(અચલ Locale& valid, અચલ Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    અચલ Locale& locale = getLocale(valid, actual, type, status);

    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }

    પાછા locale == Locale::get_મૂળ() ? k_મૂળ_Locale_Name : locale.getName();
}

U_NAMESPACE_END
