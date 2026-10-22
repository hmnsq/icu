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

#تعریف_کن UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کن AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کن PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_کلید_Factory::Simple_Locale_کلید_Factory(UObject* obj_مقصد_Adopt,
                                               ثابت UnicodeString& locale,
                                               int32_t نوع,
                                               int32_t coverage)
  : Locale_کلید_Factory(coverage)
  , _obj(obj_مقصد_Adopt)
  , _id(locale)
  , نوع_2(نوع)
{
}

Simple_Locale_کلید_Factory::Simple_Locale_کلید_Factory(UObject* obj_مقصد_Adopt,
                                               ثابت Locale& locale,
                                               int32_t نوع,
                                               int32_t coverage)
  : Locale_کلید_Factory(coverage)
  , _obj(obj_مقصد_Adopt)
  , _id()
  , نوع_2(نوع)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_کلید_Factory::~Simple_Locale_کلید_Factory()
{
  حذف _obj;
  _obj = اشاره_تهی;
}

UObject*
Simple_Locale_کلید_Factory::create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UError_کد& وضعیت) ثابت
{
    إذا (U_SUCCESS(وضعیت)) {
        ثابت Locale_کلید& lkey = static_cast<ثابت Locale_کلید&>(کلید);
        إذا (نوع_2 == Locale_کلید::نوع_ANY || نوع_2 == lkey.نوع()) {
            UnicodeString کلید_ID;
            lkey.currentID(کلید_ID);
            إذا (_id == کلید_ID) {
                أرجع service->cloneInstance(_obj);
            }
        }
    }
    أرجع اشاره_تهی;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

فراغ
Simple_Locale_کلید_Factory::updateVisibleIDs(Hashtable& نتیجه, UError_کد& وضعیت) ثابت
{
    إذا (U_SUCCESS(وضعیت)) {
        إذا (_coverage & 0x1) {
            نتیجه.remove(_id);
        } وإلا {
            نتیجه.put(_id, (فراغ*)هذا, وضعیت);
        }
    }
}

#اگر_تعریف_شده SERVICE_DEBUG
UnicodeString&
Simple_Locale_کلید_Factory::debug(UnicodeString& نتیجه) ثابت
{
    Locale_کلید_Factory::debug(نتیجه);
    نتیجه.append((UnicodeString)", id: ");
    نتیجه.append(_id);
    نتیجه.append((UnicodeString)", kind: ");
    نتیجه.append(نوع_2);
    أرجع نتیجه;
}

UnicodeString&
Simple_Locale_کلید_Factory::debugClass(UnicodeString& نتیجه) ثابت
{
    أرجع نتیجه.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#پایان_اگر

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_کلید_Factory)

U_NAMESPACE_انتها

/* !UCONFIG_NO_SERVICE */
#پایان_اگر


