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
#include "locbased.h"
#include "uresimp.h"

U_NAMESPACE_BEGIN

const Locale& LocaleBased::getLocale(const Locale& valid U_LIFETIME_BOUND,
                                     const Locale& actual U_LIFETIME_BOUND,
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    if (U_FAILURE(status)) {
        return Locale::get_उगात();
    }

    switch(type) {
    case ULOC_VALID_LOCALE:
        return valid;
    case ULOC_ACTUAL_LOCALE:
        return actual;
    default:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        return Locale::get_उगात();
    }
}

const char* LocaleBased::getLocaleID(const Locale& valid, const Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    const Locale& locale = getLocale(valid, actual, type, status);

    if (U_FAILURE(status)) {
        return nullptr;
    }

    return locale == Locale::get_उगात() ? k_उगात_Locale_Name : locale.getName();
}

U_NAMESPACE_END
