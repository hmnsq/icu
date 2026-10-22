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
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNDERSCORE_CHAR ((char16_t)0x005f)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AT_SIGN_CHAR    ((char16_t)64)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(ਅਚਲ UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(ਅਚਲ Locale& locale, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ
{
    ਵਾਪਸ get(locale, Locale_ਕੁੰਜੀ::ਕਿਸਮ_ANY, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
}

UObject*
ICULocaleService::get(ਅਚਲ Locale& locale, int32_t ਕਿਸਮ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ
{
    ਵਾਪਸ get(locale, ਕਿਸਮ, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ);
}

UObject*
ICULocaleService::get(ਅਚਲ Locale& locale, Locale* actualReturn, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ
{
    ਵਾਪਸ get(locale, Locale_ਕੁੰਜੀ::ਕਿਸਮ_ANY, actualReturn, ਸਥਿਤੀ);
}

UObject*
ICULocaleService::get(ਅਚਲ Locale& locale, int32_t ਕਿਸਮ, Locale* actualReturn, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ
{
    UObject* ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    ਜੇ (locName.isBogus()) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    } ਨਹੀਂਤਾਂ {
        ICUService_ਕੁੰਜੀ* ਕੁੰਜੀ = create_ਕੁੰਜੀ(&locName, ਕਿਸਮ, ਸਥਿਤੀ);
        ਜੇ (ਕੁੰਜੀ) {
            ਜੇ (actualReturn == ਖਾਲੀ_ਸੂਚਕ) {
                ਨਤੀਜਾ = get_ਕੁੰਜੀ(*ਕੁੰਜੀ, ਸਥਿਤੀ);
            } ਨਹੀਂਤਾਂ {
                UnicodeString ਆਰਜ਼ੀ;
                ਨਤੀਜਾ = get_ਕੁੰਜੀ(*ਕੁੰਜੀ, &ਆਰਜ਼ੀ, ਸਥਿਤੀ);

                ਜੇ (ਨਤੀਜਾ != ਖਾਲੀ_ਸੂਚਕ) {
                    ਕੁੰਜੀ->parseSuffix(ਆਰਜ਼ੀ);
                    LocaleUtility::initLocaleFromName(ਆਰਜ਼ੀ, *actualReturn);
                }
            }
            ਮਿਟਾਓ ਕੁੰਜੀ;
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}


URegistry_ਕੁੰਜੀ
ICULocaleService::ਰਜਿਸਟਰ_Instance(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ UnicodeString& locale, 
    UBool visible, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    ਵਾਪਸ ਰਜਿਸਟਰ_Instance(obj_ਮੰਜ਼ਿਲ_Adopt, loc, Locale_ਕੁੰਜੀ::ਕਿਸਮ_ANY, 
        visible ? Locale_ਕੁੰਜੀ_Factory::VISIBLE : Locale_ਕੁੰਜੀ_Factory::INVISIBLE, ਸਥਿਤੀ);
}

URegistry_ਕੁੰਜੀ
ICULocaleService::ਰਜਿਸਟਰ_Instance(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ Locale& locale, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਵਾਪਸ ਰਜਿਸਟਰ_Instance(obj_ਮੰਜ਼ਿਲ_Adopt, locale, Locale_ਕੁੰਜੀ::ਕਿਸਮ_ANY, Locale_ਕੁੰਜੀ_Factory::VISIBLE, ਸਥਿਤੀ);
}

URegistry_ਕੁੰਜੀ
ICULocaleService::ਰਜਿਸਟਰ_Instance(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ Locale& locale, int32_t ਕਿਸਮ, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਵਾਪਸ ਰਜਿਸਟਰ_Instance(obj_ਮੰਜ਼ਿਲ_Adopt, locale, ਕਿਸਮ, Locale_ਕੁੰਜੀ_Factory::VISIBLE, ਸਥਿਤੀ);
}

URegistry_ਕੁੰਜੀ
ICULocaleService::ਰਜਿਸਟਰ_Instance(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ Locale& locale, int32_t ਕਿਸਮ, int32_t coverage, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ICUServiceFactory * factory = ਨਵਾਂ Simple_Locale_ਕੁੰਜੀ_Factory(obj_ਮੰਜ਼ਿਲ_Adopt, locale, ਕਿਸਮ, coverage);
    ਜੇ (factory != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਰਜਿਸਟਰ_Factory(factory, ਸਥਿਤੀ);
    }
    ਮਿਟਾਓ obj_ਮੰਜ਼ਿਲ_Adopt;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

#ਜੇ 0
URegistry_ਕੁੰਜੀ
ICULocaleService::ਰਜਿਸਟਰ_Instance(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ UnicodeString& locale, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਵਾਪਸ ਰਜਿਸਟਰ_Instance(obj_ਮੰਜ਼ਿਲ_Adopt, locale, Locale_ਕੁੰਜੀ::ਕਿਸਮ_ANY, Locale_ਕੁੰਜੀ_Factory::VISIBLE, ਸਥਿਤੀ);
}

URegistry_ਕੁੰਜੀ
ICULocaleService::ਰਜਿਸਟਰ_Instance(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ UnicodeString& locale, UBool visible, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਵਾਪਸ ਰਜਿਸਟਰ_Instance(obj_ਮੰਜ਼ਿਲ_Adopt, locale, Locale_ਕੁੰਜੀ::ਕਿਸਮ_ANY,
                            visible ? Locale_ਕੁੰਜੀ_Factory::VISIBLE : Locale_ਕੁੰਜੀ_Factory::INVISIBLE,
                            ਸਥਿਤੀ);
}

URegistry_ਕੁੰਜੀ
ICULocaleService::ਰਜਿਸਟਰ_Instance(UObject* obj_ਮੰਜ਼ਿਲ_Adopt, ਅਚਲ UnicodeString& locale, int32_t ਕਿਸਮ, int32_t coverage, UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ICUServiceFactory * factory = ਨਵਾਂ Simple_Locale_ਕੁੰਜੀ_Factory(obj_ਮੰਜ਼ਿਲ_Adopt, locale, ਕਿਸਮ, coverage);
    ਜੇ (factory != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਰਜਿਸਟਰ_Factory(factory, ਸਥਿਤੀ);
    }
    ਮਿਟਾਓ obj_ਮੰਜ਼ਿਲ_Adopt;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}
#ਜੇ_ਅੰਤ

ਵਰਗ ServiceEnumeration : ਜਨਤਕ StringEnumeration {
ਨਿਜੀ:
    ਅਚਲ ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

ਨਿਜੀ:
    ServiceEnumeration(ਅਚਲ ICULocaleService* service U_LIFETIME_BOUND, UError_ਕੋਡ& ਸਥਿਤੀ)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, ਸਥਿਤੀ);
    }

    ServiceEnumeration(ਅਚਲ ServiceEnumeration &other, UError_ਕੋਡ &ਸਥਿਤੀ)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, ਖਾਲੀ_ਸੂਚਕ, ਸਥਿਤੀ)
        , _pos(0)
    {
        ਜੇ(U_SUCCESS(ਸਥਿਤੀ)) {
            int32_t i, length;

            length = other._ids.size();
            ਲਈ(i = 0; i < length; ++i) {
                Local_ਸੂਚਕ<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), ਸਥਿਤੀ);
                _ids.adoptElement(clonedId.orphan(), ਸਥਿਤੀ);
            }

            ਜੇ(U_SUCCESS(ਸਥਿਤੀ)) {
                _pos = other._pos;
            }
        }
    }

ਜਨਤਕ:
    ਸਥਿਰ ServiceEnumeration* create(ਅਚਲ ICULocaleService* service U_LIFETIME_BOUND) {
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        ServiceEnumeration* ਨਤੀਜਾ = ਨਵਾਂ ServiceEnumeration(service, ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            ਵਾਪਸ ਨਤੀਜਾ;
        }
        ਮਿਟਾਓ ਨਤੀਜਾ;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਆਭਾਸੀ ~ServiceEnumeration();

    ਆਭਾਸੀ StringEnumeration *clone() ਅਚਲ override {
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        ServiceEnumeration *cl = ਨਵਾਂ ServiceEnumeration(*ਇਹ, ਸਥਿਤੀ);
        ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
            ਮਿਟਾਓ cl;
            cl = ਖਾਲੀ_ਸੂਚਕ;
        }
        ਵਾਪਸ cl;
    }

    UBool ਉੱਪਰ_ਮੰਜ਼ਿਲ_Date(UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            ਜੇ (_timestamp == _service->getTimestamp()) {
                ਵਾਪਸ ਸੱਚ;
            }
            ਸਥਿਤੀ = U_ENUM_ਬਾਹਰਲਾ_ਦਾ_SYNC_ERROR;
        }
        ਵਾਪਸ ਝੂਠ;
    }

    ਆਭਾਸੀ int32_t count(UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override {
        ਵਾਪਸ ਉੱਪਰ_ਮੰਜ਼ਿਲ_Date(ਸਥਿਤੀ) ? _ids.size() : 0;
    }

    ਆਭਾਸੀ ਅਚਲ UnicodeString* snext(UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND override {
        ਜੇ (ਉੱਪਰ_ਮੰਜ਼ਿਲ_Date(ਸਥਿਤੀ) && (_pos < _ids.size())) {
            ਵਾਪਸ static_cast<ਅਚਲ UnicodeString*>(_ids[_pos++]);
        }
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਆਭਾਸੀ ਖਾਲੀ reset(UError_ਕੋਡ& ਸਥਿਤੀ) override {
        ਜੇ (ਸਥਿਤੀ == U_ENUM_ਬਾਹਰਲਾ_ਦਾ_SYNC_ERROR) {
            ਸਥਿਤੀ = U_ZERO_ERROR;
        }
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, ਸਥਿਤੀ);
        }
    }

ਜਨਤਕ:
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID();
    ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::get_ਉਪਲਬਧ_Locales() ਅਚਲ U_LIFETIME_BOUND
{
    ਵਾਪਸ ServiceEnumeration::create(ਇਹ);
}

ਅਚਲ UnicodeString&
ICULocaleService::validateFallbackLocale() ਅਚਲ
{
    ਅਚਲ Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(ਇਹ);
    ਸਥਿਰ UMutex llock;
    {
        Mutex mutex(&llock);
        ਜੇ (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    ਵਾਪਸ fallbackLocaleName;
}

ICUService_ਕੁੰਜੀ*
ICULocaleService::create_ਕੁੰਜੀ(ਅਚਲ UnicodeString* id, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ
{
    ਵਾਪਸ Locale_ਕੁੰਜੀ::createWithCanonicalFallback(id, &validateFallbackLocale(), ਸਥਿਤੀ);
}

ICUService_ਕੁੰਜੀ*
ICULocaleService::create_ਕੁੰਜੀ(ਅਚਲ UnicodeString* id, int32_t ਕਿਸਮ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ
{
    ਵਾਪਸ Locale_ਕੁੰਜੀ::createWithCanonicalFallback(id, &validateFallbackLocale(), ਕਿਸਮ, ਸਥਿਤੀ);
}

U_NAMESPACE_ਸਿਰਾ

/* !UCONFIG_NO_SERVICE */
#ਜੇ_ਅੰਤ


