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

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_مبودي_Factory::Simple_Locale_مبودي_Factory(UObject* objToAdopt,
                                               ثابت UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_مبودي_Factory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

Simple_Locale_مبودي_Factory::Simple_Locale_مبودي_Factory(UObject* objToAdopt,
                                               ثابت Locale& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_مبودي_Factory(coverage)
  , _obj(objToAdopt)
  , _id()
  , _kind(kind)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_مبودي_Factory::~Simple_Locale_مبودي_Factory()
{
  delete _obj;
  _obj = nullptr;
}

UObject*
Simple_Locale_مبودي_Factory::create(ثابت ICUService_مبودي& مبودي, ثابت ICUService* service, UErrorCode& status) ثابت
{
    إذا (U_SUCCESS(status)) {
        ثابت Locale_مبودي& lkey = static_cast<ثابت Locale_مبودي&>(مبودي);
        إذا (_kind == Locale_مبودي::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString مبودي_ID;
            lkey.currentID(مبودي_ID);
            إذا (_id == مبودي_ID) {
                أرجع service->cloneInstance(_obj);
            }
        }
    }
    أرجع nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

فراغ
Simple_Locale_مبودي_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ثابت
{
    إذا (U_SUCCESS(status)) {
        إذا (_coverage & 0x1) {
            result.remove(_id);
        } وإلا {
            result.put(_id, (فراغ*)هذا, status);
        }
    }
}

#ifdef SERVICE_DEBUG
UnicodeString&
Simple_Locale_مبودي_Factory::debug(UnicodeString& result) ثابت
{
    Locale_مبودي_Factory::debug(result);
    result.append((UnicodeString)", id: ");
    result.append(_id);
    result.append((UnicodeString)", kind: ");
    result.append(_kind);
    أرجع result;
}

UnicodeString&
Simple_Locale_مبودي_Factory::debugClass(UnicodeString& result) ثابت
{
    أرجع result.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_مبودي_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


