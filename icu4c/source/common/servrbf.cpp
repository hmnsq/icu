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

#إذا !UCONFIG_NO_SERVICE

#ضمن "unicode/resbund.h"
#ضمن "uresimp.h"
#ضمن "cmemory.h"
#ضمن "servloc.h"
#ضمن "ustrfmt.h"
#ضمن "uhash.h"
#ضمن "charstr.h"
#ضمن "ucln_cmn.h"
#ضمن "uassert.h"

#تعریف_کرو UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کرو AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کرو PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_کلید_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(ثابت UnicodeString& bundleName)
  : Locale_کلید_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

ثابت Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UError_کوڈ& حالت) ثابت
{
    إذا (U_SUCCESS(حالت)) {
        أرجع LocaleUtility::get_دستیاب_Locale_ناں(_bundleName);
    }
    أرجع خالی_سوچک;
}

UObject*
ICUResourceBundleFactory::handleCreate(ثابت Locale& loc, int32_t /*قسم*/, ثابت ICUService* /* service */, UError_کوڈ& حالت) ثابت
{
    إذا (U_SUCCESS(حالت)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        حرف pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(حجم(pkg)), US_INV);
        إذا (length >= static_cast<int32_t>(حجم(pkg))) {
            أرجع خالی_سوچک;
        }
        أرجع جديد ResourceBundle(pkg, loc, حالت);
    }
    أرجع خالی_سوچک;
}

#جے_تعریف_شدہ SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& نتیجہ) ثابت
{
    Locale_کلید_Factory::debug(نتیجہ);
    نتیجہ.append((UnicodeString)", bundle: ");
    أرجع نتیجہ.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& نتیجہ) ثابت
{
    أرجع نتیجہ.append((UnicodeString)"ICUResourceBundleFactory");
}
#جے_انت

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_سرا

/* !UCONFIG_NO_SERVICE */
#جے_انت


