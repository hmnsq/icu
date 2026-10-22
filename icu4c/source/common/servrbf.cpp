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

#تعریف_کن UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کن AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کن PERIOD_CHAR     ((char16_t)46)

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
ICUResourceBundleFactory::getSupportedIDs(UError_کد& وضعیت) ثابت
{
    إذا (U_SUCCESS(وضعیت)) {
        أرجع LocaleUtility::get_موجود_Locale_نامها(_bundleName);
    }
    أرجع اشاره_تهی;
}

UObject*
ICUResourceBundleFactory::handleCreate(ثابت Locale& loc, int32_t /*نوع*/, ثابت ICUService* /* service */, UError_کد& وضعیت) ثابت
{
    إذا (U_SUCCESS(وضعیت)) {
        // _bundleName is a package name
        // and should only contain invariant characters
                // ??? is it always true that the max length of the bundle name is 19?
                // who made this change? -- dlf
        حرف pkg[20];
        int32_t length;
        length = _bundleName.extract(0, INT32_MAX, pkg, static_cast<int32_t>(حجم(pkg)), US_INV);
        إذا (length >= static_cast<int32_t>(حجم(pkg))) {
            أرجع اشاره_تهی;
        }
        أرجع جديد ResourceBundle(pkg, loc, وضعیت);
    }
    أرجع اشاره_تهی;
}

#اگر_تعریف_شده SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& نتیجه) ثابت
{
    Locale_کلید_Factory::debug(نتیجه);
    نتیجه.append((UnicodeString)", bundle: ");
    أرجع نتیجه.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& نتیجه) ثابت
{
    أرجع نتیجه.append((UnicodeString)"ICUResourceBundleFactory");
}
#پایان_اگر

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_انتها

/* !UCONFIG_NO_SERVICE */
#پایان_اگر


