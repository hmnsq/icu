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
#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_SERVICE

#અન્તર્ભાવ "unicode/resbund.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "servloc.h"
#અન્તર્ભાવ "ustrfmt.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_ચાવી_Factory::Simple_Locale_ચાવી_Factory(UObject* objToAdopt,
                                               અચલ UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_ચાવી_Factory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

Simple_Locale_ચાવી_Factory::Simple_Locale_ચાવી_Factory(UObject* objToAdopt,
                                               અચલ Locale& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_ચાવી_Factory(coverage)
  , _obj(objToAdopt)
  , _id()
  , _kind(kind)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_ચાવી_Factory::~Simple_Locale_ચાવી_Factory()
{
  delete _obj;
  _obj = nullptr;
}

UObject*
Simple_Locale_ચાવી_Factory::create(અચલ ICUService_ચાવી& ચાવી, અચલ ICUService* service, UErrorCode& status) અચલ
{
    જો (U_SUCCESS(status)) {
        અચલ Locale_ચાવી& lkey = static_cast<અચલ Locale_ચાવી&>(ચાવી);
        જો (_kind == Locale_ચાવી::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString ચાવી_ID;
            lkey.currentID(ચાવી_ID);
            જો (_id == ચાવી_ID) {
                પાછા service->cloneInstance(_obj);
            }
        }
    }
    પાછા nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

શૂન્ય
Simple_Locale_ચાવી_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) અચલ
{
    જો (U_SUCCESS(status)) {
        જો (_coverage & 0x1) {
            result.remove(_id);
        } નહીંતર {
            result.put(_id, (શૂન્ય*)આ, status);
        }
    }
}

#ifdef SERVICE_DEBUG
UnicodeString&
Simple_Locale_ચાવી_Factory::debug(UnicodeString& result) અચલ
{
    Locale_ચાવી_Factory::debug(result);
    result.append((UnicodeString)", id: ");
    result.append(_id);
    result.append((UnicodeString)", kind: ");
    result.append(_kind);
    પાછા result;
}

UnicodeString&
Simple_Locale_ચાવી_Factory::debugClass(UnicodeString& result) અચલ
{
    પાછા result.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_ચાવી_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


