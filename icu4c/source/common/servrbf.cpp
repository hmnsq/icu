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

#ಆದರೆ !UCONFIG_NO_SERVICE

#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "servloc.h"
#ಅನ್ತರ್ಭಾವ "ustrfmt.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UNDERSCORE_CHAR ((char16_t)0x005f)
#ವ್ಯಾಖ್ಯಾನಿಸು AT_SIGN_CHAR    ((char16_t)64)
#ವ್ಯಾಖ್ಯಾನಿಸು PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_ಕೀಲಿ_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(ಸ್ಥಿರಾಂಕ UnicodeString& bundleName)
  : Locale_ಕೀಲಿ_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

ಸ್ಥಿರಾಂಕ Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ LocaleUtility::get_ಲಭ್ಯ_Locale_ಹೆಸರುಗಳು(_bundleName);
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

UObject*
ICUResourceBundleFactory::handleCreate(ಸ್ಥಿರಾಂಕ Locale& loc, int32_t /*ಬಗೆ*/, ಸ್ಥಿರಾಂಕ ICUService* /* service */, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        ಅಕ್ಷರ pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(ಗಾತ್ರ(pkg)), US_INV);
        ಆದರೆ (length >= static_cast<int32_t>(ಗಾತ್ರ(pkg))) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        ಹಿಂತಿರುಗಿ ಹೊಸ ResourceBundle(pkg, loc, ಸ್ಥಿತಿ_2);
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ
{
    Locale_ಕೀಲಿ_Factory::debug(ಫಲಿತಾಂಶ);
    ಫಲಿತಾಂಶ.append((UnicodeString)", bundle: ");
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append((UnicodeString)"ICUResourceBundleFactory");
}
#ಆದರೆ_ಅಂತ್ಯ

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_ಕೊನೆ

/* !UCONFIG_NO_SERVICE */
#ಆದರೆ_ಅಂತ್ಯ


