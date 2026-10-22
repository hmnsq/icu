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
#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_SERVICE

#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "servloc.h"
#अन्तर्भाव "ustrfmt.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_कुंजी_Factory::Simple_Locale_कुंजी_Factory(UObject* objToAdopt,
                                               ध्रुव UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_कुंजी_Factory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

Simple_Locale_कुंजी_Factory::Simple_Locale_कुंजी_Factory(UObject* objToAdopt,
                                               ध्रुव Locale& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_कुंजी_Factory(coverage)
  , _obj(objToAdopt)
  , _id()
  , _kind(kind)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_कुंजी_Factory::~Simple_Locale_कुंजी_Factory()
{
  delete _obj;
  _obj = nullptr;
}

UObject*
Simple_Locale_कुंजी_Factory::create(ध्रुव ICUService_कुंजी& कुंजी, ध्रुव ICUService* service, UErrorCode& status) ध्रुव
{
    यदि (U_SUCCESS(status)) {
        ध्रुव Locale_कुंजी& lkey = static_cast<ध्रुव Locale_कुंजी&>(कुंजी);
        यदि (_kind == Locale_कुंजी::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString कुंजी_ID;
            lkey.currentID(कुंजी_ID);
            यदि (_id == कुंजी_ID) {
                प्रत्यागम service->cloneInstance(_obj);
            }
        }
    }
    प्रत्यागम nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

शून्य
Simple_Locale_कुंजी_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ध्रुव
{
    यदि (U_SUCCESS(status)) {
        यदि (_coverage & 0x1) {
            result.remove(_id);
        } अन्यथा {
            result.put(_id, (शून्य*)एतत्, status);
        }
    }
}

#ifdef SERVICE_DEBUG
UnicodeString&
Simple_Locale_कुंजी_Factory::debug(UnicodeString& result) ध्रुव
{
    Locale_कुंजी_Factory::debug(result);
    result.append((UnicodeString)", id: ");
    result.append(_id);
    result.append((UnicodeString)", kind: ");
    result.append(_kind);
    प्रत्यागम result;
}

UnicodeString&
Simple_Locale_कुंजी_Factory::debugClass(UnicodeString& result) ध्रुव
{
    प्रत्यागम result.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_कुंजी_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


