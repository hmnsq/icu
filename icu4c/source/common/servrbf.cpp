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
#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_SERVICE

#അന്തര്ഭാവ "unicode/resbund.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "servloc.h"
#അന്തര്ഭാവ "ustrfmt.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_താക്കോൽ_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(സ്ഥിരം UnicodeString& bundleName)
  : Locale_താക്കോൽ_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

സ്ഥിരം Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UErrorCode& status) സ്ഥിരം
{
    എങ്കിൽ (U_SUCCESS(status)) {
        തിരികെ LocaleUtility::getAvailableLocaleNames(_bundleName);
    }
    തിരികെ nullptr;
}

UObject*
ICUResourceBundleFactory::handleCreate(സ്ഥിരം Locale& loc, int32_t /* kind */, സ്ഥിരം ICUService* /* service */, UErrorCode& status) സ്ഥിരം
{
    എങ്കിൽ (U_SUCCESS(status)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        അക്ഷരം pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(വലുപ്പം(pkg)), US_INV);
        എങ്കിൽ (length >= static_cast<int32_t>(വലുപ്പം(pkg))) {
            തിരികെ nullptr;
        }
        തിരികെ പുതിയ ResourceBundle(pkg, loc, status);
    }
    തിരികെ nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& result) സ്ഥിരം
{
    Locale_താക്കോൽ_Factory::debug(result);
    result.append((UnicodeString)", bundle: ");
    തിരികെ result.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& result) സ്ഥിരം
{
    തിരികെ result.append((UnicodeString)"ICUResourceBundleFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


