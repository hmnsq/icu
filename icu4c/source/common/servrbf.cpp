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
#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_SERVICE

#شامل "unicode/resbund.h"
#شامل "uresimp.h"
#شامل "cmemory.h"
#شامل "servloc.h"
#شامل "ustrfmt.h"
#شامل "uhash.h"
#شامل "charstr.h"
#شامل "ucln_cmn.h"
#شامل "uassert.h"

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
ICUResourceBundleFactory::getSupportedIDs(UError_رمز& حالت_2) ثابت
{
    اگر (U_SUCCESS(حالت_2)) {
        واپس LocaleUtility::get_دستیاب_Locale_نام(_bundleName);
    }
    واپس خالی_اشاریہ;
}

UObject*
ICUResourceBundleFactory::handleCreate(ثابت Locale& loc, int32_t /*قسم*/, ثابت ICUService* /* service */, UError_رمز& حالت_2) ثابت
{
    اگر (U_SUCCESS(حالت_2)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        حرف pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(حجم(pkg)), US_INV);
        اگر (length >= static_cast<int32_t>(حجم(pkg))) {
            واپس خالی_اشاریہ;
        }
        واپس نیا ResourceBundle(pkg, loc, حالت_2);
    }
    واپس خالی_اشاریہ;
}

#اگر_تعریف_شدہ SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& نتیجہ) ثابت
{
    Locale_کلید_Factory::debug(نتیجہ);
    نتیجہ.append((UnicodeString)", bundle: ");
    واپس نتیجہ.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& نتیجہ) ثابت
{
    واپس نتیجہ.append((UnicodeString)"ICUResourceBundleFactory");
}
#اگر_ختم

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_انتہا

/* !UCONFIG_NO_SERVICE */
#اگر_ختم


