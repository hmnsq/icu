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
#ضمن "locbased.h"
#ضمن "uresimp.h"

U_NAMESPACE_BEGIN

ثابت Locale& LocaleBased::getLocale(ثابت Locale& valid U_LIFETIME_BOUND,
                                     ثابت Locale& actual U_LIFETIME_BOUND,
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ Locale::getRoot();
    }

    بدل(type) {
    حالة ULOC_VALID_LOCALE:
        ܦܢܐ valid;
    حالة ULOC_ACTUAL_LOCALE:
        ܦܢܐ actual;
    افتراضي:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ Locale::getRoot();
    }
}

ثابت حرف* LocaleBased::getLocaleID(ثابت Locale& valid, ثابت Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    ثابت Locale& locale = getLocale(valid, actual, type, status);

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ nullptr;
    }

    ܦܢܐ locale == Locale::getRoot() ? kRootLocaleName : locale.getName();
}

U_NAMESPACE_END
