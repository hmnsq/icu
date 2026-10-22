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

#تعریف_کړه UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کړه AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کړه PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_کلی_Factory::Simple_Locale_کلی_Factory(UObject* obj_منزل_Adopt,
                                               ثابت UnicodeString& locale,
                                               int32_t ډول,
                                               int32_t coverage)
  : Locale_کلی_Factory(coverage)
  , _obj(obj_منزل_Adopt)
  , _id(locale)
  , ډول_2(ډول)
{
}

Simple_Locale_کلی_Factory::Simple_Locale_کلی_Factory(UObject* obj_منزل_Adopt,
                                               ثابت Locale& locale,
                                               int32_t ډول,
                                               int32_t coverage)
  : Locale_کلی_Factory(coverage)
  , _obj(obj_منزل_Adopt)
  , _id()
  , ډول_2(ډول)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_کلی_Factory::~Simple_Locale_کلی_Factory()
{
  ړنګول _obj;
  _obj = تش_اشاره;
}

UObject*
Simple_Locale_کلی_Factory::create(ثابت ICUService_کلی& کلی, ثابت ICUService* service, UError_کوډ& حالت) ثابت
{
    إذا (U_SUCCESS(حالت)) {
        ثابت Locale_کلی& lkey = static_cast<ثابت Locale_کلی&>(کلی);
        إذا (ډول_2 == Locale_کلی::ډول_ANY || ډول_2 == lkey.ډول()) {
            UnicodeString کلی_ID;
            lkey.currentID(کلی_ID);
            إذا (_id == کلی_ID) {
                أرجع service->cloneInstance(_obj);
            }
        }
    }
    أرجع تش_اشاره;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

فراغ
Simple_Locale_کلی_Factory::updateVisibleIDs(Hashtable& پایله, UError_کوډ& حالت) ثابت
{
    إذا (U_SUCCESS(حالت)) {
        إذا (_coverage & 0x1) {
            پایله.remove(_id);
        } وإلا {
            پایله.put(_id, (فراغ*)هذا, حالت);
        }
    }
}

#که_تعریف_شوی SERVICE_DEBUG
UnicodeString&
Simple_Locale_کلی_Factory::debug(UnicodeString& پایله) ثابت
{
    Locale_کلی_Factory::debug(پایله);
    پایله.append((UnicodeString)", id: ");
    پایله.append(_id);
    پایله.append((UnicodeString)", kind: ");
    پایله.append(ډول_2);
    أرجع پایله;
}

UnicodeString&
Simple_Locale_کلی_Factory::debugClass(UnicodeString& پایله) ثابت
{
    أرجع پایله.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#د_که_پای

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_کلی_Factory)

U_NAMESPACE_پای_ټکی

/* !UCONFIG_NO_SERVICE */
#د_که_پای


