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

ಸ್ಥಿರಾಂಕ Locale& LocaleBased::getLocale(ಸ್ಥಿರಾಂಕ Locale& valid U_LIFETIME_BOUND,
                                     ಸ್ಥಿರಾಂಕ Locale& actual U_LIFETIME_BOUND,
                                     ULoc_ದತ್ತಾಂಶ_Locale_Type type,
                                     UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ Locale::get_ಬೇರು();
    }

    ಆಯ್ಕೆ(type) {
    ಸ್ಥಿತಿ ULOC_VALID_LOCALE:
        ಹಿಂತಿರುಗಿ valid;
    ಸ್ಥಿತಿ ULOC_ACTUAL_LOCALE:
        ಹಿಂತಿರುಗಿ actual;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ Locale::get_ಬೇರು();
    }
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* LocaleBased::getLocaleID(ಸ್ಥಿರಾಂಕ Locale& valid, ಸ್ಥಿರಾಂಕ Locale& actual,
                                     ULoc_ದತ್ತಾಂಶ_Locale_Type type, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಸ್ಥಿರಾಂಕ Locale& locale = getLocale(valid, actual, type, ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಹಿಂತಿರುಗಿ locale == Locale::get_ಬೇರು() ? k_ಬೇರು_Locale_Name : locale.getName();
}

U_NAMESPACE_ಕೊನೆ
