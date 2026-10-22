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

#ಆದರೆ !UCONFIG_NO_SERVICE

#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "servloc.h"
#ಅನ್ತರ್ಭಾವ "ustrfmt.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UNDERSCORE_CHAR ((char16_t)0x005f)
#ವ್ಯಾಖ್ಯಾನಿಸು AT_SIGN_CHAR    ((char16_t)64)
#ವ್ಯಾಖ್ಯಾನಿಸು PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(ಸ್ಥಿರಾಂಕ UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(ಸ್ಥಿರಾಂಕ Locale& locale, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ get(locale, Locale_ಕೀಲಿ::ಬಗೆ_ANY, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
}

UObject*
ICULocaleService::get(ಸ್ಥಿರಾಂಕ Locale& locale, int32_t ಬಗೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ get(locale, ಬಗೆ, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
}

UObject*
ICULocaleService::get(ಸ್ಥಿರಾಂಕ Locale& locale, Locale* actualReturn, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ get(locale, Locale_ಕೀಲಿ::ಬಗೆ_ANY, actualReturn, ಸ್ಥಿತಿ_2);
}

UObject*
ICULocaleService::get(ಸ್ಥಿರಾಂಕ Locale& locale, int32_t ಬಗೆ, Locale* actualReturn, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    UObject* ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    ಆದರೆ (locName.isBogus()) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ICUService_ಕೀಲಿ* ಕೀಲಿ = create_ಕೀಲಿ(&locName, ಬಗೆ, ಸ್ಥಿತಿ_2);
        ಆದರೆ (ಕೀಲಿ) {
            ಆದರೆ (actualReturn == ಶೂನ್ಯಸೂಚಕ) {
                ಫಲಿತಾಂಶ = get_ಕೀಲಿ(*ಕೀಲಿ, ಸ್ಥಿತಿ_2);
            } ಇಲ್ಲದಿದ್ದರೆ {
                UnicodeString ತಾತ್ಕಾಲಿಕ;
                ಫಲಿತಾಂಶ = get_ಕೀಲಿ(*ಕೀಲಿ, &ತಾತ್ಕಾಲಿಕ, ಸ್ಥಿತಿ_2);

                ಆದರೆ (ಫಲಿತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
                    ಕೀಲಿ->parseSuffix(ತಾತ್ಕಾಲಿಕ);
                    LocaleUtility::initLocaleFromName(ತಾತ್ಕಾಲಿಕ, *actualReturn);
                }
            }
            ಅಳಿಸು ಕೀಲಿ;
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}


URegistry_ಕೀಲಿ
ICULocaleService::ರಿಜಿಸ್ಟರ್_Instance(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ UnicodeString& locale, 
    UBool visible, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    ಹಿಂತಿರುಗಿ ರಿಜಿಸ್ಟರ್_Instance(obj_ಗಮ್ಯ_Adopt, loc, Locale_ಕೀಲಿ::ಬಗೆ_ANY, 
        visible ? Locale_ಕೀಲಿ_Factory::VISIBLE : Locale_ಕೀಲಿ_Factory::INVISIBLE, ಸ್ಥಿತಿ_2);
}

URegistry_ಕೀಲಿ
ICULocaleService::ರಿಜಿಸ್ಟರ್_Instance(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ Locale& locale, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ ರಿಜಿಸ್ಟರ್_Instance(obj_ಗಮ್ಯ_Adopt, locale, Locale_ಕೀಲಿ::ಬಗೆ_ANY, Locale_ಕೀಲಿ_Factory::VISIBLE, ಸ್ಥಿತಿ_2);
}

URegistry_ಕೀಲಿ
ICULocaleService::ರಿಜಿಸ್ಟರ್_Instance(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ Locale& locale, int32_t ಬಗೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ ರಿಜಿಸ್ಟರ್_Instance(obj_ಗಮ್ಯ_Adopt, locale, ಬಗೆ, Locale_ಕೀಲಿ_Factory::VISIBLE, ಸ್ಥಿತಿ_2);
}

URegistry_ಕೀಲಿ
ICULocaleService::ರಿಜಿಸ್ಟರ್_Instance(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ Locale& locale, int32_t ಬಗೆ, int32_t coverage, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ICUServiceFactory * factory = ಹೊಸ Simple_Locale_ಕೀಲಿ_Factory(obj_ಗಮ್ಯ_Adopt, locale, ಬಗೆ, coverage);
    ಆದರೆ (factory != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ರಿಜಿಸ್ಟರ್_Factory(factory, ಸ್ಥಿತಿ_2);
    }
    ಅಳಿಸು obj_ಗಮ್ಯ_Adopt;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

#ಆದರೆ 0
URegistry_ಕೀಲಿ
ICULocaleService::ರಿಜಿಸ್ಟರ್_Instance(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ UnicodeString& locale, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ ರಿಜಿಸ್ಟರ್_Instance(obj_ಗಮ್ಯ_Adopt, locale, Locale_ಕೀಲಿ::ಬಗೆ_ANY, Locale_ಕೀಲಿ_Factory::VISIBLE, ಸ್ಥಿತಿ_2);
}

URegistry_ಕೀಲಿ
ICULocaleService::ರಿಜಿಸ್ಟರ್_Instance(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ UnicodeString& locale, UBool visible, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ ರಿಜಿಸ್ಟರ್_Instance(obj_ಗಮ್ಯ_Adopt, locale, Locale_ಕೀಲಿ::ಬಗೆ_ANY,
                            visible ? Locale_ಕೀಲಿ_Factory::VISIBLE : Locale_ಕೀಲಿ_Factory::INVISIBLE,
                            ಸ್ಥಿತಿ_2);
}

URegistry_ಕೀಲಿ
ICULocaleService::ರಿಜಿಸ್ಟರ್_Instance(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ UnicodeString& locale, int32_t ಬಗೆ, int32_t coverage, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ICUServiceFactory * factory = ಹೊಸ Simple_Locale_ಕೀಲಿ_Factory(obj_ಗಮ್ಯ_Adopt, locale, ಬಗೆ, coverage);
    ಆದರೆ (factory != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ರಿಜಿಸ್ಟರ್_Factory(factory, ಸ್ಥಿತಿ_2);
    }
    ಅಳಿಸು obj_ಗಮ್ಯ_Adopt;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}
#ಆದರೆ_ಅಂತ್ಯ

ವರ್ಗ ServiceEnumeration : ಸಾರ್ವಜನಿಕ StringEnumeration {
ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

ಖಾಸಗಿ:
    ServiceEnumeration(ಸ್ಥಿರಾಂಕ ICULocaleService* service U_LIFETIME_BOUND, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, ಸ್ಥಿತಿ_2);
    }

    ServiceEnumeration(ಸ್ಥಿರಾಂಕ ServiceEnumeration &other, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2)
        , _pos(0)
    {
        ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            int32_t i, length;

            length = other._ids.size();
            ಗಾಗಿ(i = 0; i < length; ++i) {
                Local_ಸೂಚಕ<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), ಸ್ಥಿತಿ_2);
                _ids.adoptElement(clonedId.orphan(), ಸ್ಥಿತಿ_2);
            }

            ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2)) {
                _pos = other._pos;
            }
        }
    }

ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ ServiceEnumeration* create(ಸ್ಥಿರಾಂಕ ICULocaleService* service U_LIFETIME_BOUND) {
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        ServiceEnumeration* ಫಲಿತಾಂಶ = ಹೊಸ ServiceEnumeration(service, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        ಅಳಿಸು ಫಲಿತಾಂಶ;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ವಾಸ್ತವಿಕ ~ServiceEnumeration();

    ವಾಸ್ತವಿಕ StringEnumeration *clone() ಸ್ಥಿರಾಂಕ override {
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        ServiceEnumeration *cl = ಹೊಸ ServiceEnumeration(*ಇದು, ಸ್ಥಿತಿ_2);
        ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಅಳಿಸು cl;
            cl = ಶೂನ್ಯಸೂಚಕ;
        }
        ಹಿಂತಿರುಗಿ cl;
    }

    UBool ಮೇಲೆ_ಗಮ್ಯ_Date(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಆದರೆ (_timestamp == _service->getTimestamp()) {
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
            ಸ್ಥಿತಿ_2 = U_ENUM_ಹೊರಹೋಗು_ದ_SYNC_ERROR;
        }
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ವಾಸ್ತವಿಕ int32_t count(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ ಮೇಲೆ_ಗಮ್ಯ_Date(ಸ್ಥಿತಿ_2) ? _ids.size() : 0;
    }

    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ UnicodeString* snext(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND override {
        ಆದರೆ (ಮೇಲೆ_ಗಮ್ಯ_Date(ಸ್ಥಿತಿ_2) && (_pos < _ids.size())) {
            ಹಿಂತಿರುಗಿ static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(_ids[_pos++]);
        }
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ವಾಸ್ತವಿಕ ಶೂನ್ಯ reset(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) override {
        ಆದರೆ (ಸ್ಥಿತಿ_2 == U_ENUM_ಹೊರಹೋಗು_ದ_SYNC_ERROR) {
            ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        }
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, ಸ್ಥಿತಿ_2);
        }
    }

ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();
    ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::get_ಲಭ್ಯ_Locales() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND
{
    ಹಿಂತಿರುಗಿ ServiceEnumeration::create(ಇದು);
}

ಸ್ಥಿರಾಂಕ UnicodeString&
ICULocaleService::validateFallbackLocale() ಸ್ಥಿರಾಂಕ
{
    ಸ್ಥಿರಾಂಕ Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(ಇದು);
    ಸ್ಥಿರ UMutex llock;
    {
        Mutex mutex(&llock);
        ಆದರೆ (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    ಹಿಂತಿರುಗಿ fallbackLocaleName;
}

ICUService_ಕೀಲಿ*
ICULocaleService::create_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UnicodeString* id, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ Locale_ಕೀಲಿ::createWithCanonicalFallback(id, &validateFallbackLocale(), ಸ್ಥಿತಿ_2);
}

ICUService_ಕೀಲಿ*
ICULocaleService::create_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UnicodeString* id, int32_t ಬಗೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ Locale_ಕೀಲಿ::createWithCanonicalFallback(id, &validateFallbackLocale(), ಬಗೆ, ಸ್ಥಿತಿ_2);
}

U_NAMESPACE_ಕೊನೆ

/* !UCONFIG_NO_SERVICE */
#ಆದರೆ_ಅಂತ್ಯ


