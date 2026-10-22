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
#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_SERVICE

#অন্তর্ভাব "unicode/resbund.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "servloc.h"
#অন্তর্ভাব "ustrfmt.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_চাবি_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(ধ্রুবক UnicodeString& bundleName)
  : Locale_চাবি_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

ধ্রুবক Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UErrorCode& status) ধ্রুবক
{
    যদি (U_SUCCESS(status)) {
        ফেরত LocaleUtility::getAvailableLocaleNames(_bundleName);
    }
    ফেরত nullptr;
}

UObject*
ICUResourceBundleFactory::handleCreate(ধ্রুবক Locale& loc, int32_t /* kind */, ধ্রুবক ICUService* /* service */, UErrorCode& status) ধ্রুবক
{
    যদি (U_SUCCESS(status)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        অক্ষর pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(আকার(pkg)), US_INV);
        যদি (length >= static_cast<int32_t>(আকার(pkg))) {
            ফেরত nullptr;
        }
        ফেরত নতুন ResourceBundle(pkg, loc, status);
    }
    ফেরত nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& ফলাফল) ধ্রুবক
{
    Locale_চাবি_Factory::debug(ফলাফল);
    ফলাফল.append((UnicodeString)", bundle: ");
    ফেরত ফলাফল.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& ফলাফল) ধ্রুবক
{
    ফেরত ফলাফল.append((UnicodeString)"ICUResourceBundleFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


