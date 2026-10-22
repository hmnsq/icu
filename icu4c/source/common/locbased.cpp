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
#ಅನ್ತರ್ಭಾವ "locbased.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"

U_NAMESPACE_BEGIN

ಧ್ರುವ Locale& LocaleBased::getLocale(ಧ್ರುವ Locale& valid U_LIFETIME_BOUND,
                                     ಧ್ರುವ Locale& actual U_LIFETIME_BOUND,
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ Locale::get_ಬೇರು();
    }

    ವಿಕಲ್ಪ(type) {
    ಪಕ್ಷ ULOC_VALID_LOCALE:
        ಪ್ರತ್ಯಾಗಮ valid;
    ಪಕ್ಷ ULOC_ACTUAL_LOCALE:
        ಪ್ರತ್ಯಾಗಮ actual;
    ಸ್ವಭಾವ:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ Locale::get_ಬೇರು();
    }
}

ಧ್ರುವ ವರ್ಣ* LocaleBased::getLocaleID(ಧ್ರುವ Locale& valid, ಧ್ರುವ Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    ಧ್ರುವ Locale& locale = getLocale(valid, actual, type, status);

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಪ್ರತ್ಯಾಗಮ locale == Locale::get_ಬೇರು() ? k_ಬೇರು_Locale_Name : locale.getName();
}

U_NAMESPACE_END
