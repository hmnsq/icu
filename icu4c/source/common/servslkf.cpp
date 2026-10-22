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
#شامل "uassert.h"

#تعریف_کرو UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کرو AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کرو PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_کلید_Factory::Simple_Locale_کلید_Factory(UObject* obj_مقصد_Adopt,
                                               ثابت UnicodeString& locale,
                                               int32_t قسم,
                                               int32_t coverage)
  : Locale_کلید_Factory(coverage)
  , _obj(obj_مقصد_Adopt)
  , _id(locale)
  , قسم_2(قسم)
{
}

Simple_Locale_کلید_Factory::Simple_Locale_کلید_Factory(UObject* obj_مقصد_Adopt,
                                               ثابت Locale& locale,
                                               int32_t قسم,
                                               int32_t coverage)
  : Locale_کلید_Factory(coverage)
  , _obj(obj_مقصد_Adopt)
  , _id()
  , قسم_2(قسم)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_کلید_Factory::~Simple_Locale_کلید_Factory()
{
  حذف _obj;
  _obj = خالی_اشاریہ;
}

UObject*
Simple_Locale_کلید_Factory::create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UError_رمز& حالت_2) ثابت
{
    اگر (U_SUCCESS(حالت_2)) {
        ثابت Locale_کلید& lkey = static_cast<ثابت Locale_کلید&>(کلید);
        اگر (قسم_2 == Locale_کلید::قسم_ANY || قسم_2 == lkey.قسم()) {
            UnicodeString کلید_ID;
            lkey.currentID(کلید_ID);
            اگر (_id == کلید_ID) {
                واپس service->cloneInstance(_obj);
            }
        }
    }
    واپس خالی_اشاریہ;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

خالی
Simple_Locale_کلید_Factory::updateVisibleIDs(Hashtable& نتیجہ, UError_رمز& حالت_2) ثابت
{
    اگر (U_SUCCESS(حالت_2)) {
        اگر (_coverage & 0x1) {
            نتیجہ.remove(_id);
        } ورنہ {
            نتیجہ.put(_id, (خالی*)یہ, حالت_2);
        }
    }
}

#اگر_تعریف_شدہ SERVICE_DEBUG
UnicodeString&
Simple_Locale_کلید_Factory::debug(UnicodeString& نتیجہ) ثابت
{
    Locale_کلید_Factory::debug(نتیجہ);
    نتیجہ.append((UnicodeString)", id: ");
    نتیجہ.append(_id);
    نتیجہ.append((UnicodeString)", kind: ");
    نتیجہ.append(قسم_2);
    واپس نتیجہ;
}

UnicodeString&
Simple_Locale_کلید_Factory::debugClass(UnicodeString& نتیجہ) ثابت
{
    واپس نتیجہ.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#اگر_ختم

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_کلید_Factory)

U_NAMESPACE_انتہا

/* !UCONFIG_NO_SERVICE */
#اگر_ختم


