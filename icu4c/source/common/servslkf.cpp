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
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_SERVICE

#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "servloc.h"
#ಅನ್ತರ್ಭಾವ "ustrfmt.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_ಕೀಲಿ_Factory::Simple_Locale_ಕೀಲಿ_Factory(UObject* objToAdopt,
                                               ಧ್ರುವ UnicodeString& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_ಕೀಲಿ_Factory(coverage)
  , _obj(objToAdopt)
  , _id(locale)
  , _kind(kind)
{
}

Simple_Locale_ಕೀಲಿ_Factory::Simple_Locale_ಕೀಲಿ_Factory(UObject* objToAdopt,
                                               ಧ್ರುವ Locale& locale,
                                               int32_t kind,
                                               int32_t coverage)
  : Locale_ಕೀಲಿ_Factory(coverage)
  , _obj(objToAdopt)
  , _id()
  , _kind(kind)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_ಕೀಲಿ_Factory::~Simple_Locale_ಕೀಲಿ_Factory()
{
  delete _obj;
  _obj = nullptr;
}

UObject*
Simple_Locale_ಕೀಲಿ_Factory::create(ಧ್ರುವ ICUService_ಕೀಲಿ& ಕೀಲಿ, ಧ್ರುವ ICUService* service, UErrorCode& status) ಧ್ರುವ
{
    ಯದಿ (U_SUCCESS(status)) {
        ಧ್ರುವ Locale_ಕೀಲಿ& lkey = static_cast<ಧ್ರುವ Locale_ಕೀಲಿ&>(ಕೀಲಿ);
        ಯದಿ (_kind == Locale_ಕೀಲಿ::KIND_ANY || _kind == lkey.kind()) {
            UnicodeString ಕೀಲಿ_ID;
            lkey.currentID(ಕೀಲಿ_ID);
            ಯದಿ (_id == ಕೀಲಿ_ID) {
                ಪ್ರತ್ಯಾಗಮ service->cloneInstance(_obj);
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

ಶೂನ್ಯ
Simple_Locale_ಕೀಲಿ_Factory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ಧ್ರುವ
{
    ಯದಿ (U_SUCCESS(status)) {
        ಯದಿ (_coverage & 0x1) {
            result.remove(_id);
        } ಅನ್ಯಥಾ {
            result.put(_id, (ಶೂನ್ಯ*)ಏತತ್, status);
        }
    }
}

#ifdef SERVICE_DEBUG
UnicodeString&
Simple_Locale_ಕೀಲಿ_Factory::debug(UnicodeString& result) ಧ್ರುವ
{
    Locale_ಕೀಲಿ_Factory::debug(result);
    result.append((UnicodeString)", id: ");
    result.append(_id);
    result.append((UnicodeString)", kind: ");
    result.append(_kind);
    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString&
Simple_Locale_ಕೀಲಿ_Factory::debugClass(UnicodeString& result) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ result.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_ಕೀಲಿ_Factory)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


