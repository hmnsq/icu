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
#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_SERVICE

#অন্তর্ভাৱ "unicode/resbund.h"
#অন্তর্ভাৱ "uresimp.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "servloc.h"
#অন্তর্ভাৱ "ustrfmt.h"
#অন্তর্ভাৱ "uhash.h"
#অন্তর্ভাৱ "charstr.h"
#অন্তর্ভাৱ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_চাবি_Factory::Simple_Locale_চাবি_Factory(UObject* objToAdopt,
                                               ধ্রুৱ UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_চাবি_Factory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

Simple_Locale_চাবি_Factory::Simple_Locale_চাবি_Factory(UObject* objToAdopt,
                                               ধ্রুৱ Locale& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_চাবি_Factory(coverage)
  , _obj(objToAdopt)
  , _id()
  , _kind(kind)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_চাবি_Factory::~Simple_Locale_চাবি_Factory()
{
  delete _obj;
  _obj = nullptr;
}

UObject*
Simple_Locale_চাবি_Factory::create(ধ্রুৱ ICUService_চাবি& চাবি, ধ্রুৱ ICUService* service, UErrorCode& status) ধ্রুৱ
{
    যদি (U_SUCCESS(status)) {
        ধ্রুৱ Locale_চাবি& lkey = static_cast<ধ্রুৱ Locale_চাবি&>(চাবি);
        যদি (_kind == Locale_চাবি::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString চাবি_ID;
            lkey.currentID(চাবি_ID);
            যদি (_id == চাবি_ID) {
                ঘূৰাই service->cloneInstance(_obj);
            }
        }
    }
    ঘূৰাই nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

শূন্য
Simple_Locale_চাবি_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ধ্রুৱ
{
    যদি (U_SUCCESS(status)) {
        যদি (_coverage & 0x1) {
            result.remove(_id);
        } অন্যথা {
            result.put(_id, (শূন্য*)এতত্, status);
        }
    }
}

#ifdef SERVICE_DEBUG
UnicodeString&
Simple_Locale_চাবি_Factory::debug(UnicodeString& result) ধ্রুৱ
{
    Locale_চাবি_Factory::debug(result);
    result.append((UnicodeString)", id: ");
    result.append(_id);
    result.append((UnicodeString)", kind: ");
    result.append(_kind);
    ঘূৰাই result;
}

UnicodeString&
Simple_Locale_চাবি_Factory::debugClass(UnicodeString& result) ধ্রুৱ
{
    ঘূৰাই result.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_চাবি_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


