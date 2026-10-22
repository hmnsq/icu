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

#परिभाषय UNDERSCORE_CHAR ((char16_t)0x005f)
#परिभाषय AT_SIGN_CHAR    ((char16_t)64)
#परिभाषय PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_कुञ्चिका_Factory::Simple_Locale_कुञ्चिका_Factory(UObject* obj_गन्तव्यम्_Adopt,
                                               ध्रुव UnicodeString& locale,
                                               int32_t विधा,
                                               int32_t coverage)
  : Locale_कुञ्चिका_Factory(coverage)
  , _obj(obj_गन्तव्यम्_Adopt)
  , _id(locale)
  , विधा_2(विधा)
{
}

Simple_Locale_कुञ्चिका_Factory::Simple_Locale_कुञ्चिका_Factory(UObject* obj_गन्तव्यम्_Adopt,
                                               ध्रुव Locale& locale,
                                               int32_t विधा,
                                               int32_t coverage)
  : Locale_कुञ्चिका_Factory(coverage)
  , _obj(obj_गन्तव्यम्_Adopt)
  , _id()
  , विधा_2(विधा)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_कुञ्चिका_Factory::~Simple_Locale_कुञ्चिका_Factory()
{
  विलोपय _obj;
  _obj = शून्यसूचकः;
}

UObject*
Simple_Locale_कुञ्चिका_Factory::create(ध्रुव ICUService_कुञ्चिका& कुञ्चिका, ध्रुव ICUService* service, UError_संहिता& स्थितिः) ध्रुव
{
    यदि (U_SUCCESS(स्थितिः)) {
        ध्रुव Locale_कुञ्चिका& lkey = static_cast<ध्रुव Locale_कुञ्चिका&>(कुञ्चिका);
        यदि (विधा_2 == Locale_कुञ्चिका::विधा_ANY || विधा_2 == lkey.विधा()) {
            UnicodeString कुञ्चिका_ID;
            lkey.currentID(कुञ्चिका_ID);
            यदि (_id == कुञ्चिका_ID) {
                प्रत्यागम service->cloneInstance(_obj);
            }
        }
    }
    प्रत्यागम शून्यसूचकः;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

शून्य
Simple_Locale_कुञ्चिका_Factory::updateVisibleIDs(Hashtable& परिणाम, UError_संहिता& स्थितिः) ध्रुव
{
    यदि (U_SUCCESS(स्थितिः)) {
        यदि (_coverage & 0x1) {
            परिणाम.remove(_id);
        } अन्यथा {
            परिणाम.put(_id, (शून्य*)एतत्, स्थितिः);
        }
    }
}

#यदि_परिभाषितम् SERVICE_DEBUG
UnicodeString&
Simple_Locale_कुञ्चिका_Factory::debug(UnicodeString& परिणाम) ध्रुव
{
    Locale_कुञ्चिका_Factory::debug(परिणाम);
    परिणाम.append((UnicodeString)", id: ");
    परिणाम.append(_id);
    परिणाम.append((UnicodeString)", kind: ");
    परिणाम.append(विधा_2);
    प्रत्यागम परिणाम;
}

UnicodeString&
Simple_Locale_कुञ्चिका_Factory::debugClass(UnicodeString& परिणाम) ध्रुव
{
    प्रत्यागम परिणाम.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#यदि_अन्तः

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_कुञ्चिका_Factory)

U_NAMESPACE_अन्तः

/* !UCONFIG_NO_SERVICE */
#यदि_अन्तः


