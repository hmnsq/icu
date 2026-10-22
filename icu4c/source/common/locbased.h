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
#నిర్వచితం_కాకపోతే LOCBASED_H
#నిర్వచించు LOCBASED_H

#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/uobject.h"

U_NAMESPACE_BEGIN

/**
 * A utility class that unifies the implementation of getLocale() by
 * various ICU services.  This class is likely to be removed in the
 * ICU 3.0 time frame in favor of an integrated approach with the
 * services framework.
 * @since ICU 2.8
 */
class U_COMMON_API LocaleBased : బహిరంగ UMemory {

 బహిరంగ:

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
    స్థిర స్థిరరాశి Locale& getLocale(
        స్థిరరాశి Locale& valid U_LIFETIME_BOUND,
        స్థిరరాశి Locale& actual U_LIFETIME_BOUND,
        ULoc_దత్తాంశం_Locale_Type type,
        UError_సంకేతం& స్థితి_2);

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
    స్థిర స్థిరరాశి అక్షరం* getLocaleID(
        స్థిరరాశి Locale& valid, స్థిరరాశి Locale& actual,
        ULoc_దత్తాంశం_Locale_Type type, UError_సంకేతం& స్థితి_2);

};

U_NAMESPACE_చివర

#అయితే_ముగింపు
