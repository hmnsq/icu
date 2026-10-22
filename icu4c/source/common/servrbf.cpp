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
#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_SERVICE

#شامل "unicode/resbund.h"
#شامل "uresimp.h"
#شامل "cmemory.h"
#شامل "servloc.h"
#شامل "ustrfmt.h"
#شامل "uhash.h"
#شامل "charstr.h"
#شامل "ucln_cmn.h"
#شامل "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_کلید_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(ثابت UnicodeString& bundleName)
  : Locale_کلید_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

ثابت Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UErrorCode& status) ثابت
{
    اگر (U_SUCCESS(status)) {
        بازگشت LocaleUtility::getAvailableLocaleNames(_bundleName);
    }
    بازگشت nullptr;
}

UObject*
ICUResourceBundleFactory::handleCreate(ثابت Locale& loc, int32_t /* kind */, ثابت ICUService* /* service */, UErrorCode& status) ثابت
{
    اگر (U_SUCCESS(status)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        نویسه pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(اندازه(pkg)), US_INV);
        اگر (length >= static_cast<int32_t>(اندازه(pkg))) {
            بازگشت nullptr;
        }
        بازگشت جدید ResourceBundle(pkg, loc, status);
    }
    بازگشت nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& result) ثابت
{
    Locale_کلید_Factory::debug(result);
    result.append((UnicodeString)", bundle: ");
    بازگشت result.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& result) ثابت
{
    بازگشت result.append((UnicodeString)"ICUResourceBundleFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


