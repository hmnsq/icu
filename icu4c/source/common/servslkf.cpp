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
#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_SERVICE

#അന്തര്ഭാവ "unicode/resbund.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "servloc.h"
#അന്തര്ഭാവ "ustrfmt.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_താക്കോൽ_Factory::Simple_Locale_താക്കോൽ_Factory(UObject* objToAdopt,
                                               സ്ഥിരം UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_താക്കോൽ_Factory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

Simple_Locale_താക്കോൽ_Factory::Simple_Locale_താക്കോൽ_Factory(UObject* objToAdopt,
                                               സ്ഥിരം Locale& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_താക്കോൽ_Factory(coverage)
  , _obj(objToAdopt)
  , _id()
  , _kind(kind)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_താക്കോൽ_Factory::~Simple_Locale_താക്കോൽ_Factory()
{
  delete _obj;
  _obj = nullptr;
}

UObject*
Simple_Locale_താക്കോൽ_Factory::create(സ്ഥിരം ICUService_താക്കോൽ& താക്കോൽ, സ്ഥിരം ICUService* service, UErrorCode& status) സ്ഥിരം
{
    എങ്കിൽ (U_SUCCESS(status)) {
        സ്ഥിരം Locale_താക്കോൽ& lkey = static_cast<സ്ഥിരം Locale_താക്കോൽ&>(താക്കോൽ);
        എങ്കിൽ (_kind == Locale_താക്കോൽ::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString താക്കോൽ_ID;
            lkey.currentID(താക്കോൽ_ID);
            എങ്കിൽ (_id == താക്കോൽ_ID) {
                തിരികെ service->cloneInstance(_obj);
            }
        }
    }
    തിരികെ nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

ശൂന്യം
Simple_Locale_താക്കോൽ_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) സ്ഥിരം
{
    എങ്കിൽ (U_SUCCESS(status)) {
        എങ്കിൽ (_coverage & 0x1) {
            result.remove(_id);
        } അല്ലെങ്കിൽ {
            result.put(_id, (ശൂന്യം*)ഇത്, status);
        }
    }
}

#ifdef SERVICE_DEBUG
UnicodeString&
Simple_Locale_താക്കോൽ_Factory::debug(UnicodeString& result) സ്ഥിരം
{
    Locale_താക്കോൽ_Factory::debug(result);
    result.append((UnicodeString)", id: ");
    result.append(_id);
    result.append((UnicodeString)", kind: ");
    result.append(_kind);
    തിരികെ result;
}

UnicodeString&
Simple_Locale_താക്കോൽ_Factory::debugClass(UnicodeString& result) സ്ഥിരം
{
    തിരികെ result.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_താക്കോൽ_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


