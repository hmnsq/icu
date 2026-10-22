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
#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_SERVICE

#அந்தர்பாவ "unicode/resbund.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "servloc.h"
#அந்தர்பாவ "ustrfmt.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_சாவி_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(மாறிலி UnicodeString& bundleName)
  : Locale_சாவி_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

மாறிலி Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UErrorCode& status) மாறிலி
{
    எனில் (U_SUCCESS(status)) {
        திரும்பு LocaleUtility::getAvailableLocaleNames(_bundleName);
    }
    திரும்பு nullptr;
}

UObject*
ICUResourceBundleFactory::handleCreate(மாறிலி Locale& loc, int32_t /* kind */, மாறிலி ICUService* /* service */, UErrorCode& status) மாறிலி
{
    எனில் (U_SUCCESS(status)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        எழுத்து pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(அளவு(pkg)), US_INV);
        எனில் (length >= static_cast<int32_t>(அளவு(pkg))) {
            திரும்பு nullptr;
        }
        திரும்பு புதிய ResourceBundle(pkg, loc, status);
    }
    திரும்பு nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& result) மாறிலி
{
    Locale_சாவி_Factory::debug(result);
    result.append((UnicodeString)", bundle: ");
    திரும்பு result.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& result) மாறிலி
{
    திரும்பு result.append((UnicodeString)"ICUResourceBundleFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


