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

#यदि !UCONFIG_NO_SERVICE

#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "servloc.h"
#अन्तर्भाव "ustrfmt.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uassert.h"

#परिभाषय UNDERSCORE_CHAR ((char16_t)0x005f)
#परिभाषय AT_SIGN_CHAR    ((char16_t)64)
#परिभाषय PERIOD_CHAR     ((char16_t)46)

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
ICUResourceBundleFactory::getSupportedIDs(UError_संहिता& स्थितिः) ध्रुव
{
    यदि (U_SUCCESS(स्थितिः)) {
        प्रत्यागम LocaleUtility::get_उपलब्धम्_Locale_नामानि(_bundleName);
    }
    प्रत्यागम शून्यसूचकः;
}

UObject*
ICUResourceBundleFactory::handleCreate(ध्रुव Locale& loc, int32_t /*विधा*/, ध्रुव ICUService* /* service */, UError_संहिता& स्थितिः) ध्रुव
{
    यदि (U_SUCCESS(स्थितिः)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        वर्ण pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(परिमाण(pkg)), US_INV);
        यदि (length >= static_cast<int32_t>(परिमाण(pkg))) {
            प्रत्यागम शून्यसूचकः;
        }
        प्रत्यागम नूतन ResourceBundle(pkg, loc, स्थितिः);
    }
    प्रत्यागम शून्यसूचकः;
}

#यदि_परिभाषितम् SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& परिणाम) ध्रुव
{
    Locale_कुञ्चिका_Factory::debug(परिणाम);
    परिणाम.append((UnicodeString)", bundle: ");
    प्रत्यागम परिणाम.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& परिणाम) ध्रुव
{
    प्रत्यागम परिणाम.append((UnicodeString)"ICUResourceBundleFactory");
}
#यदि_अन्तः

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_अन्तः

/* !UCONFIG_NO_SERVICE */
#यदि_अन्तः


