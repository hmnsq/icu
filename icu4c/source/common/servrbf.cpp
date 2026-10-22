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
#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_SERVICE

#समावेश "unicode/resbund.h"
#समावेश "uresimp.h"
#समावेश "cmemory.h"
#समावेश "servloc.h"
#समावेश "ustrfmt.h"
#समावेश "uhash.h"
#समावेश "charstr.h"
#समावेश "ucln_cmn.h"
#समावेश "uassert.h"

#घोषणा UNDERSCORE_CHAR ((char16_t)0x005f)
#घोषणा AT_SIGN_CHAR    ((char16_t)64)
#घोषणा PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_कुंजी_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(स्थिर UnicodeString& bundleName)
  : Locale_कुंजी_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

स्थिर Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UErrorCode& status) स्थिर
{
    अगर (U_SUCCESS(status)) {
        वापस LocaleUtility::getAvailableLocaleNames(_bundleName);
    }
    वापस nullptr;
}

UObject*
ICUResourceBundleFactory::handleCreate(स्थिर Locale& loc, int32_t /* kind */, स्थिर ICUService* /* service */, UErrorCode& status) स्थिर
{
    अगर (U_SUCCESS(status)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        अक्षर pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(माप(pkg)), US_INV);
        अगर (length >= static_cast<int32_t>(माप(pkg))) {
            वापस nullptr;
        }
        वापस नया ResourceBundle(pkg, loc, status);
    }
    वापस nullptr;
}

#अगर_घोषित SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& परिणाम) स्थिर
{
    Locale_कुंजी_Factory::debug(परिणाम);
    परिणाम.append((UnicodeString)", bundle: ");
    वापस परिणाम.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& परिणाम) स्थिर
{
    वापस परिणाम.append((UnicodeString)"ICUResourceBundleFactory");
}
#पूर्ण_अगर

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#पूर्ण_अगर


