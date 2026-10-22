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
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_SERVICE

#ଅନ୍ତର୍ଭାଵ "unicode/resbund.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "servloc.h"
#ଅନ୍ତର୍ଭାଵ "ustrfmt.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_ଚାବି_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(ଅଚଳ UnicodeString& bundleName)
  : Locale_ଚାବି_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

ଅଚଳ Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UErrorCode& status) ଅଚଳ
{
    ଯଦି (U_SUCCESS(status)) {
        ଫେରନ୍ତୁ LocaleUtility::getAvailableLocaleNames(_bundleName);
    }
    ଫେରନ୍ତୁ nullptr;
}

UObject*
ICUResourceBundleFactory::handleCreate(ଅଚଳ Locale& loc, int32_t /* kind */, ଅଚଳ ICUService* /* service */, UErrorCode& status) ଅଚଳ
{
    ଯଦି (U_SUCCESS(status)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        ଅକ୍ଷର pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(ଆକାର(pkg)), US_INV);
        ଯଦି (length >= static_cast<int32_t>(ଆକାର(pkg))) {
            ଫେରନ୍ତୁ nullptr;
        }
        ଫେରନ୍ତୁ ନୂତନ ResourceBundle(pkg, loc, status);
    }
    ଫେରନ୍ତୁ nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& result) ଅଚଳ
{
    Locale_ଚାବି_Factory::debug(result);
    result.append((UnicodeString)", bundle: ");
    ଫେରନ୍ତୁ result.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& result) ଅଚଳ
{
    ଫେରନ୍ତୁ result.append((UnicodeString)"ICUResourceBundleFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


