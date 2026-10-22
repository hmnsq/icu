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
#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_SERVICE

#অন্তর্ভাব "unicode/resbund.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "servloc.h"
#অন্তর্ভাব "ustrfmt.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_চাবি_Factory::Simple_Locale_চাবি_Factory(UObject* objToAdopt,
                                               ধ্রুবক UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_চাবি_Factory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

Simple_Locale_চাবি_Factory::Simple_Locale_চাবি_Factory(UObject* objToAdopt,
                                               ধ্রুবক Locale& locale,
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
Simple_Locale_চাবি_Factory::create(ধ্রুবক ICUService_চাবি& চাবি, ধ্রুবক ICUService* service, UErrorCode& status) ধ্রুবক
{
    যদি (U_SUCCESS(status)) {
        ধ্রুবক Locale_চাবি& lkey = static_cast<ধ্রুবক Locale_চাবি&>(চাবি);
        যদি (_kind == Locale_চাবি::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString চাবি_ID;
            lkey.currentID(চাবি_ID);
            যদি (_id == চাবি_ID) {
                ফেরত service->cloneInstance(_obj);
            }
        }
    }
    ফেরত nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

শূন্য
Simple_Locale_চাবি_Factory::updateVisibleIDs(Hashtable& ফলাফল, UErrorCode& status) ধ্রুবক
{
    যদি (U_SUCCESS(status)) {
        যদি (_coverage & 0x1) {
            ফলাফল.remove(_id);
        } অন্যথা {
            ফলাফল.put(_id, (শূন্য*)এই, status);
        }
    }
}

#ifdef SERVICE_DEBUG
UnicodeString&
Simple_Locale_চাবি_Factory::debug(UnicodeString& ফলাফল) ধ্রুবক
{
    Locale_চাবি_Factory::debug(ফলাফল);
    ফলাফল.append((UnicodeString)", id: ");
    ফলাফল.append(_id);
    ফলাফল.append((UnicodeString)", kind: ");
    ফলাফল.append(_kind);
    ফেরত ফলাফল;
}

UnicodeString&
Simple_Locale_চাবি_Factory::debugClass(UnicodeString& ফলাফল) ধ্রুবক
{
    ফেরত ফলাফল.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_চাবি_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


