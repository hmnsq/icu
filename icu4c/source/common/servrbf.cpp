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

#وصف_ڪريو UNDERSCORE_CHAR ((char16_t)0x005f)
#وصف_ڪريو AT_SIGN_CHAR    ((char16_t)64)
#وصف_ڪريو PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_ڪنجي_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(ثابت UnicodeString& bundleName)
  : Locale_ڪنجي_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

ثابت Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UError_ڪوڊ& حالت) ثابت
{
    إذا (U_SUCCESS(حالت)) {
        أرجع LocaleUtility::get_موجود_Locale_نالا(_bundleName);
    }
    أرجع خالي_اشارو;
}

UObject*
ICUResourceBundleFactory::handleCreate(ثابت Locale& loc, int32_t /*قسم*/, ثابت ICUService* /* service */, UError_ڪوڊ& حالت) ثابت
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
            أرجع خالي_اشارو;
        }
        أرجع جديد ResourceBundle(pkg, loc, حالت);
    }
    أرجع خالي_اشارو;
}

#جيڪڏهن_وصف_ٿيل SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& نتيجو) ثابت
{
    Locale_ڪنجي_Factory::debug(نتيجو);
    نتيجو.append((UnicodeString)", bundle: ");
    أرجع نتيجو.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& نتيجو) ثابت
{
    أرجع نتيجو.append((UnicodeString)"ICUResourceBundleFactory");
}
#جيڪڏهن_پورو

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_پڇاڙي

/* !UCONFIG_NO_SERVICE */
#جيڪڏهن_پورو


