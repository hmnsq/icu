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
#ضمن "uassert.h"

#وصف_ڪريو UNDERSCORE_CHAR ((char16_t)0x005f)
#وصف_ڪريو AT_SIGN_CHAR    ((char16_t)64)
#وصف_ڪريو PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_ڪنجي_Factory::Simple_Locale_ڪنجي_Factory(UObject* obj_منزل_Adopt,
                                               ثابت UnicodeString& locale,
                                               int32_t قسم,
                                               int32_t coverage)
  : Locale_ڪنجي_Factory(coverage)
  , _obj(obj_منزل_Adopt)
  , _id(locale)
  , قسم_2(قسم)
{
}

Simple_Locale_ڪنجي_Factory::Simple_Locale_ڪنجي_Factory(UObject* obj_منزل_Adopt,
                                               ثابت Locale& locale,
                                               int32_t قسم,
                                               int32_t coverage)
  : Locale_ڪنجي_Factory(coverage)
  , _obj(obj_منزل_Adopt)
  , _id()
  , قسم_2(قسم)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_ڪنجي_Factory::~Simple_Locale_ڪنجي_Factory()
{
  ميٽيو _obj;
  _obj = خالي_اشارو;
}

UObject*
Simple_Locale_ڪنجي_Factory::create(ثابت ICUService_ڪنجي& ڪنجي, ثابت ICUService* service, UError_ڪوڊ& حالت) ثابت
{
    إذا (U_SUCCESS(حالت)) {
        ثابت Locale_ڪنجي& lkey = static_cast<ثابت Locale_ڪنجي&>(ڪنجي);
        إذا (قسم_2 == Locale_ڪنجي::قسم_ANY || قسم_2 == lkey.قسم()) {
            UnicodeString ڪنجي_ID;
            lkey.currentID(ڪنجي_ID);
            إذا (_id == ڪنجي_ID) {
                أرجع service->cloneInstance(_obj);
            }
        }
    }
    أرجع خالي_اشارو;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

فراغ
Simple_Locale_ڪنجي_Factory::updateVisibleIDs(Hashtable& نتيجو, UError_ڪوڊ& حالت) ثابت
{
    إذا (U_SUCCESS(حالت)) {
        إذا (_coverage & 0x1) {
            نتيجو.remove(_id);
        } وإلا {
            نتيجو.put(_id, (فراغ*)هذا, حالت);
        }
    }
}

#جيڪڏهن_وصف_ٿيل SERVICE_DEBUG
UnicodeString&
Simple_Locale_ڪنجي_Factory::debug(UnicodeString& نتيجو) ثابت
{
    Locale_ڪنجي_Factory::debug(نتيجو);
    نتيجو.append((UnicodeString)", id: ");
    نتيجو.append(_id);
    نتيجو.append((UnicodeString)", kind: ");
    نتيجو.append(قسم_2);
    أرجع نتيجو;
}

UnicodeString&
Simple_Locale_ڪنجي_Factory::debugClass(UnicodeString& نتيجو) ثابت
{
    أرجع نتيجو.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#جيڪڏهن_پورو

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_ڪنجي_Factory)

U_NAMESPACE_پڇاڙي

/* !UCONFIG_NO_SERVICE */
#جيڪڏهن_پورو


