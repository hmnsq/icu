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
#اگر_غیر_تعریف_شدہ LOCBASED_H
#تعریف_کرو LOCBASED_H

#شامل "unicode/locid.h"
#شامل "unicode/uobject.h"

U_NAMESPACE_BEGIN

/**
 * A utility class that unifies the implementation of getLocale() by
 * various ICU services.  This class is likely to be removed in the
 * ICU 3.0 time frame in favor of an integrated approach with the
 * services framework.
 * @since ICU 2.8
 */
class U_COMMON_API LocaleBased : عوامی UMemory {

 عوامی:

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
    ساکن ثابت Locale& getLocale(
        ثابت Locale& valid U_LIFETIME_BOUND,
        ثابت Locale& actual U_LIFETIME_BOUND,
        ULoc_ڈیٹا_Locale_Type type,
        UError_رمز& حالت_2);

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
    ساکن ثابت حرف* getLocaleID(
        ثابت Locale& valid, ثابت Locale& actual,
        ULoc_ڈیٹا_Locale_Type type, UError_رمز& حالت_2);

};

U_NAMESPACE_انتہا

#اگر_ختم
