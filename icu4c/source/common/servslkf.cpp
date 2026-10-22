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
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_SERVICE

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/resbund.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "servloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustrfmt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNDERSCORE_CHAR ((char16_t)0x005f)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AT_SIGN_CHAR    ((char16_t)64)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

/*
 ******************************************************************
 */

Simple_Locale_ਕੁੰਜੀ_Factory::Simple_Locale_ਕੁੰਜੀ_Factory(UObject* obj_ਮੰਜ਼ਿਲ_Adopt,
                                               ਅਚਲ UnicodeString& locale,
                                               int32_t ਕਿਸਮ,
                                               int32_t coverage)
  : Locale_ਕੁੰਜੀ_Factory(coverage)
  , _obj(obj_ਮੰਜ਼ਿਲ_Adopt)
  , _id(locale)
  , ਕਿਸਮ_2(ਕਿਸਮ)
{
}

Simple_Locale_ਕੁੰਜੀ_Factory::Simple_Locale_ਕੁੰਜੀ_Factory(UObject* obj_ਮੰਜ਼ਿਲ_Adopt,
                                               ਅਚਲ Locale& locale,
                                               int32_t ਕਿਸਮ,
                                               int32_t coverage)
  : Locale_ਕੁੰਜੀ_Factory(coverage)
  , _obj(obj_ਮੰਜ਼ਿਲ_Adopt)
  , _id()
  , ਕਿਸਮ_2(ਕਿਸਮ)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_ਕੁੰਜੀ_Factory::~Simple_Locale_ਕੁੰਜੀ_Factory()
{
  ਮਿਟਾਓ _obj;
  _obj = ਖਾਲੀ_ਸੂਚਕ;
}

UObject*
Simple_Locale_ਕੁੰਜੀ_Factory::create(ਅਚਲ ICUService_ਕੁੰਜੀ& ਕੁੰਜੀ, ਅਚਲ ICUService* service, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ
{
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ਅਚਲ Locale_ਕੁੰਜੀ& lkey = static_cast<ਅਚਲ Locale_ਕੁੰਜੀ&>(ਕੁੰਜੀ);
        ਜੇ (ਕਿਸਮ_2 == Locale_ਕੁੰਜੀ::ਕਿਸਮ_ANY || ਕਿਸਮ_2 == lkey.ਕਿਸਮ()) {
            UnicodeString ਕੁੰਜੀ_ID;
            lkey.currentID(ਕੁੰਜੀ_ID);
            ਜੇ (_id == ਕੁੰਜੀ_ID) {
                ਵਾਪਸ service->cloneInstance(_obj);
            }
        }
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

ਖਾਲੀ
Simple_Locale_ਕੁੰਜੀ_Factory::updateVisibleIDs(Hashtable& ਨਤੀਜਾ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ
{
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        ਜੇ (_coverage & 0x1) {
            ਨਤੀਜਾ.remove(_id);
        } ਨਹੀਂਤਾਂ {
            ਨਤੀਜਾ.put(_id, (ਖਾਲੀ*)ਇਹ, ਸਥਿਤੀ);
        }
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SERVICE_DEBUG
UnicodeString&
Simple_Locale_ਕੁੰਜੀ_Factory::debug(UnicodeString& ਨਤੀਜਾ) ਅਚਲ
{
    Locale_ਕੁੰਜੀ_Factory::debug(ਨਤੀਜਾ);
    ਨਤੀਜਾ.append((UnicodeString)", id: ");
    ਨਤੀਜਾ.append(_id);
    ਨਤੀਜਾ.append((UnicodeString)", kind: ");
    ਨਤੀਜਾ.append(ਕਿਸਮ_2);
    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString&
Simple_Locale_ਕੁੰਜੀ_Factory::debugClass(UnicodeString& ਨਤੀਜਾ) ਅਚਲ
{
    ਵਾਪਸ ਨਤੀਜਾ.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#ਜੇ_ਅੰਤ

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_ਕੁੰਜੀ_Factory)

U_NAMESPACE_ਸਿਰਾ

/* !UCONFIG_NO_SERVICE */
#ਜੇ_ਅੰਤ


