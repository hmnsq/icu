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
#అన్తర్భావ "locbased.h"
#అన్తర్భావ "uresimp.h"

U_NAMESPACE_BEGIN

స్థిరరాశి Locale& LocaleBased::getLocale(స్థిరరాశి Locale& valid U_LIFETIME_BOUND,
                                     స్థిరరాశి Locale& actual U_LIFETIME_BOUND,
                                     ULoc_దత్తాంశం_Locale_Type type,
                                     UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి Locale::get_మూలం();
    }

    ఎంపిక(type) {
    స్థితి ULOC_VALID_LOCALE:
        తిరిగి valid;
    స్థితి ULOC_ACTUAL_LOCALE:
        తిరిగి actual;
    అప్రమేయ:
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి Locale::get_మూలం();
    }
}

స్థిరరాశి అక్షరం* LocaleBased::getLocaleID(స్థిరరాశి Locale& valid, స్థిరరాశి Locale& actual,
                                     ULoc_దత్తాంశం_Locale_Type type, UError_సంకేతం& స్థితి_2) {
    స్థిరరాశి Locale& locale = getLocale(valid, actual, type, స్థితి_2);

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    తిరిగి locale == Locale::get_మూలం() ? k_మూలం_Locale_Name : locale.getName();
}

U_NAMESPACE_చివర
