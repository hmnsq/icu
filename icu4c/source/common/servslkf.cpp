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
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_SERVICE

#ଅନ୍ତର୍ଭାଵ "unicode/resbund.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "servloc.h"
#ଅନ୍ତର୍ଭାଵ "ustrfmt.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_ଚାବି_Factory::Simple_Locale_ଚାବି_Factory(UObject* objToAdopt,
                                               ଅଚଳ UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_ଚାବି_Factory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

Simple_Locale_ଚାବି_Factory::Simple_Locale_ଚାବି_Factory(UObject* objToAdopt,
                                               ଅଚଳ Locale& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_ଚାବି_Factory(coverage)
  , _obj(objToAdopt)
  , _id()
  , _kind(kind)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_ଚାବି_Factory::~Simple_Locale_ଚାବି_Factory()
{
  delete _obj;
  _obj = nullptr;
}

UObject*
Simple_Locale_ଚାବି_Factory::create(ଅଚଳ ICUService_ଚାବି& ଚାବି, ଅଚଳ ICUService* service, UErrorCode& status) ଅଚଳ
{
    ଯଦି (U_SUCCESS(status)) {
        ଅଚଳ Locale_ଚାବି& lkey = static_cast<ଅଚଳ Locale_ଚାବି&>(ଚାବି);
        ଯଦି (_kind == Locale_ଚାବି::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString ଚାବି_ID;
            lkey.currentID(ଚାବି_ID);
            ଯଦି (_id == ଚାବି_ID) {
                ଫେରନ୍ତୁ service->cloneInstance(_obj);
            }
        }
    }
    ଫେରନ୍ତୁ nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

ଶୂନ୍ୟ
Simple_Locale_ଚାବି_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ଅଚଳ
{
    ଯଦି (U_SUCCESS(status)) {
        ଯଦି (_coverage & 0x1) {
            result.remove(_id);
        } ଅନ୍ୟଥା {
            result.put(_id, (ଶୂନ୍ୟ*)ଏହି, status);
        }
    }
}

#ifdef SERVICE_DEBUG
UnicodeString&
Simple_Locale_ଚାବି_Factory::debug(UnicodeString& result) ଅଚଳ
{
    Locale_ଚାବି_Factory::debug(result);
    result.append((UnicodeString)", id: ");
    result.append(_id);
    result.append((UnicodeString)", kind: ");
    result.append(_kind);
    ଫେରନ୍ତୁ result;
}

UnicodeString&
Simple_Locale_ଚାବି_Factory::debugClass(UnicodeString& result) ଅଚଳ
{
    ଫେରନ୍ତୁ result.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_ଚାବି_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


