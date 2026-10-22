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

#सचे !UCONFIG_NO_SERVICE

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
  : Locale_कुञ्चिका_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(ध्रुव UnicodeString& bundleName)
  : Locale_कुञ्चिका_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

ध्रुव Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UErrorCode& status) ध्रुव
{
    सचे (U_SUCCESS(status)) {
        पटिनिवत्त LocaleUtility::getAvailableLocaleNames(_bundleName);
    }
    पटिनिवत्त nullptr;
}

UObject*
ICUResourceBundleFactory::handleCreate(ध्रुव Locale& loc, int32_t /* kind */, ध्रुव ICUService* /* service */, UErrorCode& status) ध्रुव
{
    सचे (U_SUCCESS(status)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        वर्ण pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(परिमाण(pkg)), US_INV);
        सचे (length >= static_cast<int32_t>(परिमाण(pkg))) {
            पटिनिवत्त nullptr;
        }
        पटिनिवत्त नूतन ResourceBundle(pkg, loc, status);
    }
    पटिनिवत्त nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& result) ध्रुव
{
    Locale_कुञ्चिका_Factory::debug(result);
    result.append((UnicodeString)", bundle: ");
    पटिनिवत्त result.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& result) ध्रुव
{
    पटिनिवत्त result.append((UnicodeString)"ICUResourceBundleFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


