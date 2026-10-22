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

#تعریف_کرو UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کرو AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کرو PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_کلید_Factory::Simple_Locale_کلید_Factory(UObject* obj_منزل_Adopt,
                                               ثابت UnicodeString& locale,
                                               int32_t قسم,
                                               int32_t coverage)
  : Locale_کلید_Factory(coverage)
  , _obj(obj_منزل_Adopt)
  , _id(locale)
  , قسم_2(قسم)
{
}

Simple_Locale_کلید_Factory::Simple_Locale_کلید_Factory(UObject* obj_منزل_Adopt,
                                               ثابت Locale& locale,
                                               int32_t قسم,
                                               int32_t coverage)
  : Locale_کلید_Factory(coverage)
  , _obj(obj_منزل_Adopt)
  , _id()
  , قسم_2(قسم)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_کلید_Factory::~Simple_Locale_کلید_Factory()
{
  مٹاؤ _obj;
  _obj = خالی_سوچک;
}

UObject*
Simple_Locale_کلید_Factory::create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UError_کوڈ& حالت) ثابت
{
    إذا (U_SUCCESS(حالت)) {
        ثابت Locale_کلید& lkey = static_cast<ثابت Locale_کلید&>(کلید);
        إذا (قسم_2 == Locale_کلید::قسم_ANY || قسم_2 == lkey.قسم()) {
            UnicodeString کلید_ID;
            lkey.currentID(کلید_ID);
            إذا (_id == کلید_ID) {
                أرجع service->cloneInstance(_obj);
            }
        }
    }
    أرجع خالی_سوچک;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

فراغ
Simple_Locale_کلید_Factory::updateVisibleIDs(Hashtable& نتیجہ, UError_کوڈ& حالت) ثابت
{
    إذا (U_SUCCESS(حالت)) {
        إذا (_coverage & 0x1) {
            نتیجہ.remove(_id);
        } وإلا {
            نتیجہ.put(_id, (فراغ*)هذا, حالت);
        }
    }
}

#جے_تعریف_شدہ SERVICE_DEBUG
UnicodeString&
Simple_Locale_کلید_Factory::debug(UnicodeString& نتیجہ) ثابت
{
    Locale_کلید_Factory::debug(نتیجہ);
    نتیجہ.append((UnicodeString)", id: ");
    نتیجہ.append(_id);
    نتیجہ.append((UnicodeString)", kind: ");
    نتیجہ.append(قسم_2);
    أرجع نتیجہ;
}

UnicodeString&
Simple_Locale_کلید_Factory::debugClass(UnicodeString& نتیجہ) ثابت
{
    أرجع نتیجہ.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#جے_انت

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_کلید_Factory)

U_NAMESPACE_سرا

/* !UCONFIG_NO_SERVICE */
#جے_انت


