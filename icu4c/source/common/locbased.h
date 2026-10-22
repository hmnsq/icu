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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ LOCBASED_H
#ವ್ಯಾಖ್ಯಾನಿಸು LOCBASED_H

#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

U_NAMESPACE_BEGIN

/**
 * A utility class that unifies the implementation of getLocale() by
 * various ICU services.  This class is likely to be removed in the
 * ICU 3.0 time frame in favor of an integrated approach with the
 * services framework.
 * @since ICU 2.8
 */
class U_COMMON_API LocaleBased : ಸಾರ್ವಜನಿಕ UMemory {

 ಸಾರ್ವಜನಿಕ:

    /**
     * Return locale meta-data for the service object wrapped by this
     * object.  Either the valid or the actual locale may be
     * retrieved.
     * @param valid The valid locale.
     * @param actual The actual locale.
     * @param type either ULOC_VALID_LOCALE or ULOC_ACTUAL_LOCALE
     * @param status input-output error code
     * @return the indicated locale
     */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Locale& getLocale(
        ಸ್ಥಿರಾಂಕ Locale& valid U_LIFETIME_BOUND,
        ಸ್ಥಿರಾಂಕ Locale& actual U_LIFETIME_BOUND,
        ULoc_ದತ್ತಾಂಶ_Locale_Type type,
        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    /**
     * Return the locale ID for the service object wrapped by this
     * object.  Either the valid or the actual locale may be
     * retrieved.
     * @param valid The valid locale.
     * @param actual The actual locale.
     * @param type either ULOC_VALID_LOCALE or ULOC_ACTUAL_LOCALE
     * @param status input-output error code
     * @return the indicated locale ID
     */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* getLocaleID(
        ಸ್ಥಿರಾಂಕ Locale& valid, ಸ್ಥಿರಾಂಕ Locale& actual,
        ULoc_ದತ್ತಾಂಶ_Locale_Type type, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
