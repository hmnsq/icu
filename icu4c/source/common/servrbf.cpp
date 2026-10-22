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
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_SERVICE

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/resbund.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "servloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustrfmt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNDERSCORE_CHAR ((char16_t)0x005f)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AT_SIGN_CHAR    ((char16_t)64)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_ਕੁੰਜੀ_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(ਅਚਲ UnicodeString& bundleName)
  : Locale_ਕੁੰਜੀ_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

ਅਚਲ Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ
{
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ਵਾਪਸ LocaleUtility::get_ਉਪਲਬਧ_Locale_ਨਾਂ(_bundleName);
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

UObject*
ICUResourceBundleFactory::handleCreate(ਅਚਲ Locale& loc, int32_t /*ਕਿਸਮ*/, ਅਚਲ ICUService* /* service */, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ
{
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        ਅੱਖਰ pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(ਆਕਾਰ(pkg)), US_INV);
        ਜੇ (length >= static_cast<int32_t>(ਆਕਾਰ(pkg))) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        ਵਾਪਸ ਨਵਾਂ ResourceBundle(pkg, loc, ਸਥਿਤੀ);
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& ਨਤੀਜਾ) ਅਚਲ
{
    Locale_ਕੁੰਜੀ_Factory::debug(ਨਤੀਜਾ);
    ਨਤੀਜਾ.append((UnicodeString)", bundle: ");
    ਵਾਪਸ ਨਤੀਜਾ.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& ਨਤੀਜਾ) ਅਚਲ
{
    ਵਾਪਸ ਨਤੀਜਾ.append((UnicodeString)"ICUResourceBundleFactory");
}
#ਜੇ_ਅੰਤ

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_ਸਿਰਾ

/* !UCONFIG_NO_SERVICE */
#ਜੇ_ਅੰਤ


