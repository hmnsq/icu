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
#ifndef LOCBASED_H
#define LOCBASED_H

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
class U_COMMON_API LocaleBased : عمومی UMemory {

 عمومی:

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
    ایستا ثابت Locale& getLocale(
        ثابت Locale& valid U_LIFETIME_BOUND,
        ثابت Locale& actual U_LIFETIME_BOUND,
        ULocDataLocaleType type,
        UErrorCode& status);

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
    ایستا ثابت نویسه* getLocaleID(
        ثابت Locale& valid, ثابت Locale& actual,
        ULocDataLocaleType type, UErrorCode& status);

};

U_NAMESPACE_END

#endif
