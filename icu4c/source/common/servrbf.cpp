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
#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_SERVICE

#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "servloc.h"
#अन्तर्भाव "ustrfmt.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_किल्ली_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(स्थिरांक UnicodeString& bundleName)
  : Locale_किल्ली_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

स्थिरांक Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UErrorCode& status) स्थिरांक
{
    जर (U_SUCCESS(status)) {
        परत LocaleUtility::getAvailableLocaleNames(_bundleName);
    }
    परत nullptr;
}

UObject*
ICUResourceBundleFactory::handleCreate(स्थिरांक Locale& loc, int32_t /* kind */, स्थिरांक ICUService* /* service */, UErrorCode& status) स्थिरांक
{
    जर (U_SUCCESS(status)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        अक्षर pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(आकार(pkg)), US_INV);
        जर (length >= static_cast<int32_t>(आकार(pkg))) {
            परत nullptr;
        }
        परत नवीन ResourceBundle(pkg, loc, status);
    }
    परत nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& result) स्थिरांक
{
    Locale_किल्ली_Factory::debug(result);
    result.append((UnicodeString)", bundle: ");
    परत result.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& result) स्थिरांक
{
    परत result.append((UnicodeString)"ICUResourceBundleFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


