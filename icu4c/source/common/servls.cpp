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
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(ಧ್ರುವ UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(ಧ್ರುವ Locale& locale, UErrorCode& status) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ get(locale, Locale_ಕೀಲಿ::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(ಧ್ರುವ Locale& locale, int32_t kind, UErrorCode& status) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(ಧ್ರುವ Locale& locale, Locale* actualReturn, UErrorCode& status) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ get(locale, Locale_ಕೀಲಿ::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(ಧ್ರುವ Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) ಧ್ರುವ
{
    UObject* result = nullptr;
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    ಯದಿ (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } ಅನ್ಯಥಾ {
        ICUService_ಕೀಲಿ* ಕೀಲಿ = create_ಕೀಲಿ(&locName, kind, status);
        ಯದಿ (ಕೀಲಿ) {
            ಯದಿ (actualReturn == nullptr) {
                result = get_ಕೀಲಿ(*ಕೀಲಿ, status);
            } ಅನ್ಯಥಾ {
                UnicodeString temp;
                result = get_ಕೀಲಿ(*ಕೀಲಿ, &temp, status);

                ಯದಿ (result != nullptr) {
                    ಕೀಲಿ->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete ಕೀಲಿ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ result;
}


URegistry_ಕೀಲಿ
ICULocaleService::registerInstance(UObject* objToAdopt, ಧ್ರುವ UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    ಪ್ರತ್ಯಾಗಮ registerInstance(objToAdopt, loc, Locale_ಕೀಲಿ::KIND_ANY, 
        visible ? Locale_ಕೀಲಿ_Factory::VISIBLE : Locale_ಕೀಲಿ_Factory::INVISIBLE, status);
}

URegistry_ಕೀಲಿ
ICULocaleService::registerInstance(UObject* objToAdopt, ಧ್ರುವ Locale& locale, UErrorCode& status)
{
    ಪ್ರತ್ಯಾಗಮ registerInstance(objToAdopt, locale, Locale_ಕೀಲಿ::KIND_ANY, Locale_ಕೀಲಿ_Factory::VISIBLE, status);
}

URegistry_ಕೀಲಿ
ICULocaleService::registerInstance(UObject* objToAdopt, ಧ್ರುವ Locale& locale, int32_t kind, UErrorCode& status)
{
    ಪ್ರತ್ಯಾಗಮ registerInstance(objToAdopt, locale, kind, Locale_ಕೀಲಿ_Factory::VISIBLE, status);
}

URegistry_ಕೀಲಿ
ICULocaleService::registerInstance(UObject* objToAdopt, ಧ್ರುವ Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = ನೂತನ Simple_Locale_ಕೀಲಿ_Factory(objToAdopt, locale, kind, coverage);
    ಯದಿ (factory != nullptr) {
        ಪ್ರತ್ಯಾಗಮ registerFactory(factory, status);
    }
    delete objToAdopt;
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

#ಯದಿ 0
URegistry_ಕೀಲಿ
ICULocaleService::registerInstance(UObject* objToAdopt, ಧ್ರುವ UnicodeString& locale, UErrorCode& status)
{
    ಪ್ರತ್ಯಾಗಮ registerInstance(objToAdopt, locale, Locale_ಕೀಲಿ::KIND_ANY, Locale_ಕೀಲಿ_Factory::VISIBLE, status);
}

URegistry_ಕೀಲಿ
ICULocaleService::registerInstance(UObject* objToAdopt, ಧ್ರುವ UnicodeString& locale, UBool visible, UErrorCode& status)
{
    ಪ್ರತ್ಯಾಗಮ registerInstance(objToAdopt, locale, Locale_ಕೀಲಿ::KIND_ANY,
                            visible ? Locale_ಕೀಲಿ_Factory::VISIBLE : Locale_ಕೀಲಿ_Factory::INVISIBLE,
                            status);
}

URegistry_ಕೀಲಿ
ICULocaleService::registerInstance(UObject* objToAdopt, ಧ್ರುವ UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = ನೂತನ Simple_Locale_ಕೀಲಿ_Factory(objToAdopt, locale, kind, coverage);
    ಯದಿ (factory != nullptr) {
        ಪ್ರತ್ಯಾಗಮ registerFactory(factory, status);
    }
    delete objToAdopt;
    ಪ್ರತ್ಯಾಗಮ nullptr;
}
#endif

ವರ್ಗ ServiceEnumeration : ಸಾರ್ವಜನಿಕ StringEnumeration {
ಗುಪ್ತ:
    ಧ್ರುವ ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

ಗುಪ್ತ:
    ServiceEnumeration(ಧ್ರುವ ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(ಧ್ರುವ ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        ಯದಿ(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            ಪ್ರತಿ(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            ಯದಿ(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ ServiceEnumeration* create(ಧ್ರುವ ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = ನೂತನ ServiceEnumeration(service, status);
        ಯದಿ (U_SUCCESS(status)) {
            ಪ್ರತ್ಯಾಗಮ result;
        }
        delete result;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಆಭಾಸ ~ServiceEnumeration();

    ಆಭಾಸ StringEnumeration *clone() ಧ್ರುವ override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = ನೂತನ ServiceEnumeration(*ಏತತ್, status);
        ಯದಿ(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        ಪ್ರತ್ಯಾಗಮ cl;
    }

    UBool upToDate(UErrorCode& status) ಧ್ರುವ {
        ಯದಿ (U_SUCCESS(status)) {
            ಯದಿ (_timestamp == _service->getTimestamp()) {
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    ಆಭಾಸ int32_t count(UErrorCode& status) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ upToDate(status) ? _ids.size() : 0;
    }

    ಆಭಾಸ ಧ್ರುವ UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        ಯದಿ (upToDate(status) && (_pos < _ids.size())) {
            ಪ್ರತ್ಯಾಗಮ static_cast<ಧ್ರುವ UnicodeString*>(_ids[_pos++]);
        }
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಆಭಾಸ ಶೂನ್ಯ reset(UErrorCode& status) override {
        ಯದಿ (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        ಯದಿ (U_SUCCESS(status)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, status);
        }
    }

ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();
    ಆಭಾಸ UClassID getDynamicClassID() ಧ್ರುವ override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() ಧ್ರುವ U_LIFETIME_BOUND
{
    ಪ್ರತ್ಯಾಗಮ ServiceEnumeration::create(ಏತತ್);
}

ಧ್ರುವ UnicodeString&
ICULocaleService::validateFallbackLocale() ಧ್ರುವ
{
    ಧ್ರುವ Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(ಏತತ್);
    ಸ್ಥಿರ UMutex llock;
    {
        Mutex mutex(&llock);
        ಯದಿ (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    ಪ್ರತ್ಯಾಗಮ fallbackLocaleName;
}

ICUService_ಕೀಲಿ*
ICULocaleService::create_ಕೀಲಿ(ಧ್ರುವ UnicodeString* id, UErrorCode& status) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ Locale_ಕೀಲಿ::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_ಕೀಲಿ*
ICULocaleService::create_ಕೀಲಿ(ಧ್ರುವ UnicodeString* id, int32_t kind, UErrorCode& status) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ Locale_ಕೀಲಿ::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


