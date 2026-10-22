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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ LOCBASED_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOCBASED_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"

U_NAMESPACE_BEGIN

/**
 * A utility class that unifies the implementation of getLocale() by
 * various ICU services.  This class is likely to be removed in the
 * ICU 3.0 time frame in favor of an integrated approach with the
 * services framework.
 * @since ICU 2.8
 */
class U_COMMON_API LocaleBased : ਜਨਤਕ UMemory {

 ਜਨਤਕ:

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
    ਸਥਿਰ ਅਚਲ Locale& getLocale(
        ਅਚਲ Locale& valid U_LIFETIME_BOUND,
        ਅਚਲ Locale& actual U_LIFETIME_BOUND,
        ULoc_ਅੰਕੜੇ_Locale_Type type,
        UError_ਕੋਡ& ਸਥਿਤੀ);

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
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ* getLocaleID(
        ਅਚਲ Locale& valid, ਅਚਲ Locale& actual,
        ULoc_ਅੰਕੜੇ_Locale_Type type, UError_ਕੋਡ& ਸਥਿਤੀ);

};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
