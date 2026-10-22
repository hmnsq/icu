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

ICUResourceBundleFactory::ICUResourceBundleFactory(ধ্রুব UnicodeString& bundleName)
  : Locale_চাবি_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

ধ্রুব Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UErrorCode& status) ধ্রুব
{
    যদি (U_SUCCESS(status)) {
        প্রত্যাগম LocaleUtility::getAvailableLocaleNames(_bundleName);
    }
    প্রত্যাগম nullptr;
}

UObject*
ICUResourceBundleFactory::handleCreate(ধ্রুব Locale& loc, int32_t /* kind */, ধ্রুব ICUService* /* service */, UErrorCode& status) ধ্রুব
{
    যদি (U_SUCCESS(status)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        বর্ণ pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(পরিমাণ(pkg)), US_INV);
        যদি (length >= static_cast<int32_t>(পরিমাণ(pkg))) {
            প্রত্যাগম nullptr;
        }
        প্রত্যাগম নূতন ResourceBundle(pkg, loc, status);
    }
    প্রত্যাগম nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& result) ধ্রুব
{
    Locale_চাবি_Factory::debug(result);
    result.append((UnicodeString)", bundle: ");
    প্রত্যাগম result.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& result) ধ্রুব
{
    প্রত্যাগম result.append((UnicodeString)"ICUResourceBundleFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


