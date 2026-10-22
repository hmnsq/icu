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
    ᨀᨘ (U_FAILURE(status)) {
        ᨀᨘᨒ Locale::getRoot();
    }

    switch(type) {
    pak_sa ULOC_VALID_LOCALE:
        ᨀᨘᨒ valid;
    pak_sa ULOC_ACTUAL_LOCALE:
        ᨀᨘᨒ actual;
    default:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ᨀᨘᨒ Locale::getRoot();
    }
}

const var_na* LocaleBased::getLocaleID(const Locale& valid, const Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    const Locale& locale = getLocale(valid, actual, type, status);

    ᨀᨘ (U_FAILURE(status)) {
        ᨀᨘᨒ nullptr;
    }

    ᨀᨘᨒ locale == Locale::getRoot() ? kRootLocaleName : locale.getName();
}

U_NAMESPACE_END
