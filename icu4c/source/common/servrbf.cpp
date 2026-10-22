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
#ضمن "unicode/utypes.h"

#𐤀𐤌 !UCONFIG_NO_SERVICE

#ضمن "unicode/resbund.h"
#ضمن "uresimp.h"
#ضمن "cmemory.h"
#ضمن "servloc.h"
#ضمن "ustrfmt.h"
#ضمن "uhash.h"
#ضمن "charstr.h"
#ضمن "ucln_cmn.h"
#ضمن "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : LocaleKeyFactory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(ثابت UnicodeString& bundleName)
  : LocaleKeyFactory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

ثابت Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UErrorCode& status) ثابت
{
    𐤀𐤌 (U_SUCCESS(status)) {
        أرجع LocaleUtility::getAvailableLocaleNames(_bundleName);
    }
    أرجع nullptr;
}

UObject*
ICUResourceBundleFactory::handleCreate(ثابت Locale& loc, int32_t /* kind */, ثابت ICUService* /* service */, UErrorCode& status) ثابت
{
    𐤀𐤌 (U_SUCCESS(status)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        حرف pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(حجم(pkg)), US_INV);
        𐤀𐤌 (length >= static_cast<int32_t>(حجم(pkg))) {
            أرجع nullptr;
        }
        أرجع جديد ResourceBundle(pkg, loc, status);
    }
    أرجع nullptr;
}

#ifdef SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& result) ثابت
{
    LocaleKeyFactory::debug(result);
    result.append((UnicodeString)", bundle: ");
    أرجع result.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& result) ثابت
{
    أرجع result.append((UnicodeString)"ICUResourceBundleFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


