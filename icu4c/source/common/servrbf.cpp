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
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_SERVICE

#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "servloc.h"
#ಅನ್ತರ್ಭಾವ "ustrfmt.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_ಕೀಲಿ_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(ಧ್ರುವ UnicodeString& bundleName)
  : Locale_ಕೀಲಿ_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

ಧ್ರುವ Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UErrorCode& status) ಧ್ರುವ
{
    ಯದಿ (U_SUCCESS(status)) {
        ಪ್ರತ್ಯಾಗಮ LocaleUtility::getAvailableLocaleNames(_bundleName);
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

UObject*
ICUResourceBundleFactory::handleCreate(ಧ್ರುವ Locale& loc, int32_t /* kind */, ಧ್ರುವ ICUService* /* service */, UErrorCode& status) ಧ್ರುವ
{
    ಯದಿ (U_SUCCESS(status)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        ವರ್ಣ pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(ಪರಿಮಾಣ(pkg)), US_INV);
        ಯದಿ (length >= static_cast<int32_t>(ಪರಿಮಾಣ(pkg))) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        ಪ್ರತ್ಯಾಗಮ ನೂತನ ResourceBundle(pkg, loc, status);
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& result) ಧ್ರುವ
{
    Locale_ಕೀಲಿ_Factory::debug(result);
    result.append((UnicodeString)", bundle: ");
    ಪ್ರತ್ಯಾಗಮ result.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& result) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ result.append((UnicodeString)"ICUResourceBundleFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


