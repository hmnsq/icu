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
#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_SERVICE

#समावेश "unicode/resbund.h"
#समावेश "uresimp.h"
#समावेश "cmemory.h"
#समावेश "servloc.h"
#समावेश "ustrfmt.h"
#समावेश "uhash.h"
#समावेश "charstr.h"
#समावेश "uassert.h"

#घोषणा UNDERSCORE_CHAR ((char16_t)0x005f)
#घोषणा AT_SIGN_CHAR    ((char16_t)64)
#घोषणा PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_कुंजी_Factory::Simple_Locale_कुंजी_Factory(UObject* objToAdopt,
                                               स्थिर UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_कुंजी_Factory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

Simple_Locale_कुंजी_Factory::Simple_Locale_कुंजी_Factory(UObject* objToAdopt,
                                               स्थिर Locale& locale,
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
  मिटाओ _obj;
  _obj = nullptr;
}

UObject*
Simple_Locale_कुंजी_Factory::create(स्थिर ICUService_कुंजी& कुंजी, स्थिर ICUService* service, UErrorCode& status) स्थिर
{
    अगर (U_SUCCESS(status)) {
        स्थिर Locale_कुंजी& lkey = static_cast<स्थिर Locale_कुंजी&>(कुंजी);
        अगर (_kind == Locale_कुंजी::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString कुंजी_ID;
            lkey.currentID(कुंजी_ID);
            अगर (_id == कुंजी_ID) {
                वापस service->cloneInstance(_obj);
            }
        }
    }
    वापस nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

व्योम
Simple_Locale_कुंजी_Factory::updateVisibleIDs(Hashtable& परिणाम, UErrorCode& status) स्थिर
{
    अगर (U_SUCCESS(status)) {
        अगर (_coverage & 0x1) {
            परिणाम.हटाओ(_id);
        } अन्यथा {
            परिणाम.दो(_id, (व्योम*)यह, status);
        }
    }
}

#अगर_घोषित SERVICE_DEBUG
UnicodeString&
Simple_Locale_कुंजी_Factory::debug(UnicodeString& परिणाम) स्थिर
{
    Locale_कुंजी_Factory::debug(परिणाम);
    परिणाम.append((UnicodeString)", id: ");
    परिणाम.append(_id);
    परिणाम.append((UnicodeString)", kind: ");
    परिणाम.append(_kind);
    वापस परिणाम;
}

UnicodeString&
Simple_Locale_कुंजी_Factory::debugClass(UnicodeString& परिणाम) स्थिर
{
    वापस परिणाम.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#पूर्ण_अगर

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_कुंजी_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#पूर्ण_अगर


