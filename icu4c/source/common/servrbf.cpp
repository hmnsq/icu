// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 *
 *******************************************************************************
 */
#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_SERVICE

#అన్తర్భావ "unicode/resbund.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "servloc.h"
#అన్తర్భావ "ustrfmt.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "uassert.h"

#నిర్వచించు UNDERSCORE_CHAR ((char16_t)0x005f)
#నిర్వచించు AT_SIGN_CHAR    ((char16_t)64)
#నిర్వచించు PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_కీలకం_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(స్థిరరాశి UnicodeString& bundleName)
  : Locale_కీలకం_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

స్థిరరాశి Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UError_సంకేతం& స్థితి_2) స్థిరరాశి
{
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        తిరిగి LocaleUtility::get_అందుబాటు_Locale_పేర్లు(_bundleName);
    }
    తిరిగి శూన్యసూచిక;
}

UObject*
ICUResourceBundleFactory::handleCreate(స్థిరరాశి Locale& loc, int32_t /*రకం*/, స్థిరరాశి ICUService* /* service */, UError_సంకేతం& స్థితి_2) స్థిరరాశి
{
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        అక్షరం pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(పరిమాణం(pkg)), US_INV);
        ఒకవేళ (length >= static_cast<int32_t>(పరిమాణం(pkg))) {
            తిరిగి శూన్యసూచిక;
        }
        తిరిగి కొత్త ResourceBundle(pkg, loc, స్థితి_2);
    }
    తిరిగి శూన్యసూచిక;
}

#నిర్వచితమైతే SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& ఫలితం) స్థిరరాశి
{
    Locale_కీలకం_Factory::debug(ఫలితం);
    ఫలితం.append((UnicodeString)", bundle: ");
    తిరిగి ఫలితం.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& ఫలితం) స్థిరరాశి
{
    తిరిగి ఫలితం.append((UnicodeString)"ICUResourceBundleFactory");
}
#అయితే_ముగింపు

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_చివర

/* !UCONFIG_NO_SERVICE */
#అయితే_ముగింపు


