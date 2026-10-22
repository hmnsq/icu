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

#تعریف_کړه UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کړه AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کړه PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICUResourceBundleFactory::ICUResourceBundleFactory()
  : Locale_کلی_Factory(VISIBLE)
  , _bundleName()
{
}

ICUResourceBundleFactory::ICUResourceBundleFactory(ثابت UnicodeString& bundleName)
  : Locale_کلی_Factory(VISIBLE)
  , _bundleName(bundleName)
{
}

ICUResourceBundleFactory::~ICUResourceBundleFactory() {}

ثابت Hashtable*
ICUResourceBundleFactory::getSupportedIDs(UError_کوډ& حالت) ثابت
{
    إذا (U_SUCCESS(حالت)) {
        أرجع LocaleUtility::get_شته_Locale_نومونه(_bundleName);
    }
    أرجع تش_اشاره;
}

UObject*
ICUResourceBundleFactory::handleCreate(ثابت Locale& loc, int32_t /*ډول*/, ثابت ICUService* /* service */, UError_کوډ& حالت) ثابت
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
            أرجع تش_اشاره;
        }
        أرجع جديد ResourceBundle(pkg, loc, حالت);
    }
    أرجع تش_اشاره;
}

#که_تعریف_شوی SERVICE_DEBUG
UnicodeString&
ICUResourceBundleFactory::debug(UnicodeString& پایله) ثابت
{
    Locale_کلی_Factory::debug(پایله);
    پایله.append((UnicodeString)", bundle: ");
    أرجع پایله.append(_bundleName);
}

UnicodeString&
ICUResourceBundleFactory::debugClass(UnicodeString& پایله) ثابت
{
    أرجع پایله.append((UnicodeString)"ICUResourceBundleFactory");
}
#د_که_پای

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUResourceBundleFactory)

U_NAMESPACE_پای_ټکی

/* !UCONFIG_NO_SERVICE */
#د_که_پای


