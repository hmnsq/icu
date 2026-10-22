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

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_کلید_Factory::Simple_Locale_کلید_Factory(UObject* objToAdopt,
                                               ثابت UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_کلید_Factory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

Simple_Locale_کلید_Factory::Simple_Locale_کلید_Factory(UObject* objToAdopt,
                                               ثابت Locale& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_کلید_Factory(coverage)
  , _obj(objToAdopt)
  , _id()
  , _kind(kind)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_کلید_Factory::~Simple_Locale_کلید_Factory()
{
  delete _obj;
  _obj = nullptr;
}

UObject*
Simple_Locale_کلید_Factory::create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UErrorCode& status) ثابت
{
    اگر (U_SUCCESS(status)) {
        ثابت Locale_کلید& lkey = static_cast<ثابت Locale_کلید&>(کلید);
        اگر (_kind == Locale_کلید::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString کلید_ID;
            lkey.currentID(کلید_ID);
            اگر (_id == کلید_ID) {
                بازگشت service->cloneInstance(_obj);
            }
        }
    }
    بازگشت nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

تهی
Simple_Locale_کلید_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ثابت
{
    اگر (U_SUCCESS(status)) {
        اگر (_coverage & 0x1) {
            result.remove(_id);
        } وگرنه {
            result.put(_id, (تهی*)این, status);
        }
    }
}

#ifdef SERVICE_DEBUG
UnicodeString&
Simple_Locale_کلید_Factory::debug(UnicodeString& result) ثابت
{
    Locale_کلید_Factory::debug(result);
    result.append((UnicodeString)", id: ");
    result.append(_id);
    result.append((UnicodeString)", kind: ");
    result.append(_kind);
    بازگشت result;
}

UnicodeString&
Simple_Locale_کلید_Factory::debugClass(UnicodeString& result) ثابت
{
    بازگشت result.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_کلید_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


