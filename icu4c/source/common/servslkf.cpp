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

#जर !UCONFIG_NO_SERVICE

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

Simple_Locale_किल्ली_Factory::Simple_Locale_किल्ली_Factory(UObject* objToAdopt,
                                               स्थिरांक UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_किल्ली_Factory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

Simple_Locale_किल्ली_Factory::Simple_Locale_किल्ली_Factory(UObject* objToAdopt,
                                               स्थिरांक Locale& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_किल्ली_Factory(coverage)
  , _obj(objToAdopt)
  , _id()
  , _kind(kind)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_किल्ली_Factory::~Simple_Locale_किल्ली_Factory()
{
  delete _obj;
  _obj = nullptr;
}

UObject*
Simple_Locale_किल्ली_Factory::create(स्थिरांक ICUService_किल्ली& किल्ली, स्थिरांक ICUService* service, UErrorCode& status) स्थिरांक
{
    जर (U_SUCCESS(status)) {
        स्थिरांक Locale_किल्ली& lkey = static_cast<स्थिरांक Locale_किल्ली&>(किल्ली);
        जर (_kind == Locale_किल्ली::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString किल्ली_ID;
            lkey.currentID(किल्ली_ID);
            जर (_id == किल्ली_ID) {
                परत service->cloneInstance(_obj);
            }
        }
    }
    परत nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

शून्य
Simple_Locale_किल्ली_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) स्थिरांक
{
    जर (U_SUCCESS(status)) {
        जर (_coverage & 0x1) {
            result.हटाओ(_id);
        } अन्यथा {
            result.put(_id, (शून्य*)हे, status);
        }
    }
}

#ifdef SERVICE_DEBUG
UnicodeString&
Simple_Locale_किल्ली_Factory::debug(UnicodeString& result) स्थिरांक
{
    Locale_किल्ली_Factory::debug(result);
    result.append((UnicodeString)", id: ");
    result.append(_id);
    result.append((UnicodeString)", kind: ");
    result.append(_kind);
    परत result;
}

UnicodeString&
Simple_Locale_किल्ली_Factory::debugClass(UnicodeString& result) स्थिरांक
{
    परत result.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_किल्ली_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


