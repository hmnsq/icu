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
#ਅਨ੍ਤਰ੍ਭਾਵ "locbased.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"

U_NAMESPACE_BEGIN

ਅਚਲ Locale& LocaleBased::getLocale(ਅਚਲ Locale& valid U_LIFETIME_BOUND,
                                     ਅਚਲ Locale& actual U_LIFETIME_BOUND,
                                     ULoc_ਅੰਕੜੇ_Locale_Type type,
                                     UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ Locale::get_ਜੜ੍ਹ();
    }

    ਚੋਣ(type) {
    ਹਾਲਤ ULOC_VALID_LOCALE:
        ਵਾਪਸ valid;
    ਹਾਲਤ ULOC_ACTUAL_LOCALE:
        ਵਾਪਸ actual;
    ਮੂਲ:
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ Locale::get_ਜੜ੍ਹ();
    }
}

ਅਚਲ ਅੱਖਰ* LocaleBased::getLocaleID(ਅਚਲ Locale& valid, ਅਚਲ Locale& actual,
                                     ULoc_ਅੰਕੜੇ_Locale_Type type, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਅਚਲ Locale& locale = getLocale(valid, actual, type, ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਵਾਪਸ locale == Locale::get_ਜੜ੍ਹ() ? k_ਜੜ੍ਹ_Locale_Name : locale.getName();
}

U_NAMESPACE_ਸਿਰਾ
