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
    如果 (U_FAILURE(status)) {
        返回 Locale::getRoot();
    }

    switch(type) {
    case ULOC_VALID_LOCALE:
        返回 valid;
    case ULOC_ACTUAL_LOCALE:
        返回 actual;
    default:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        返回 Locale::getRoot();
    }
}

const char* LocaleBased::getLocaleID(const Locale& valid, const Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    const Locale& locale = getLocale(valid, actual, type, status);

    如果 (U_FAILURE(status)) {
        返回 nullptr;
    }

    返回 locale == Locale::getRoot() ? kRootLocaleName : locale.getName();
}

U_NAMESPACE_END
