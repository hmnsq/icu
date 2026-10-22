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
#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_SERVICE

#அந்தர்பாவ "unicode/resbund.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "servloc.h"
#அந்தர்பாவ "ustrfmt.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_சாவி_Factory::Simple_Locale_சாவி_Factory(UObject* objToAdopt,
                                               மாறிலி UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_சாவி_Factory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

Simple_Locale_சாவி_Factory::Simple_Locale_சாவி_Factory(UObject* objToAdopt,
                                               மாறிலி Locale& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_சாவி_Factory(coverage)
  , _obj(objToAdopt)
  , _id()
  , _kind(kind)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_சாவி_Factory::~Simple_Locale_சாவி_Factory()
{
  delete _obj;
  _obj = nullptr;
}

UObject*
Simple_Locale_சாவி_Factory::create(மாறிலி ICUService_சாவி& சாவி, மாறிலி ICUService* service, UErrorCode& status) மாறிலி
{
    எனில் (U_SUCCESS(status)) {
        மாறிலி Locale_சாவி& lkey = static_cast<மாறிலி Locale_சாவி&>(சாவி);
        எனில் (_kind == Locale_சாவி::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString சாவி_ID;
            lkey.currentID(சாவி_ID);
            எனில் (_id == சாவி_ID) {
                திரும்பு service->cloneInstance(_obj);
            }
        }
    }
    திரும்பு nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

வெறுமை
Simple_Locale_சாவி_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) மாறிலி
{
    எனில் (U_SUCCESS(status)) {
        எனில் (_coverage & 0x1) {
            result.remove(_id);
        } இல்லையெனில் {
            result.put(_id, (வெறுமை*)இது, status);
        }
    }
}

#ifdef SERVICE_DEBUG
UnicodeString&
Simple_Locale_சாவி_Factory::debug(UnicodeString& result) மாறிலி
{
    Locale_சாவி_Factory::debug(result);
    result.append((UnicodeString)", id: ");
    result.append(_id);
    result.append((UnicodeString)", kind: ");
    result.append(_kind);
    திரும்பு result;
}

UnicodeString&
Simple_Locale_சாவி_Factory::debugClass(UnicodeString& result) மாறிலி
{
    திரும்பு result.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_சாவி_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


