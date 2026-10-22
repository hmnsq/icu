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
#که_نه_تعریف_شوی LOCBASED_H
#تعریف_کړه LOCBASED_H

#ضمن "unicode/locid.h"
#ضمن "unicode/uobject.h"

U_NAMESPACE_BEGIN

/**
 * A utility class that unifies the implementation of getLocale() by
 * various ICU services.  This class is likely to be removed in the
 * ICU 3.0 time frame in favor of an integrated approach with the
 * services framework.
 * @since ICU 2.8
 */
class U_COMMON_API LocaleBased : عام UMemory {

 عام:

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
    ساكن ثابت Locale& getLocale(
        ثابت Locale& valid U_LIFETIME_BOUND,
        ثابت Locale& actual U_LIFETIME_BOUND,
        ULoc_معلومات_Locale_Type type,
        UError_کوډ& حالت);

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
    ساكن ثابت حرف* getLocaleID(
        ثابت Locale& valid, ثابت Locale& actual,
        ULoc_معلومات_Locale_Type type, UError_کوډ& حالت);

};

U_NAMESPACE_پای_ټکی

#د_که_پای
