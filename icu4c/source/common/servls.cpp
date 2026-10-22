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
#ضمن "unicode/utypes.h"

#ܐܢ !UCONFIG_NO_SERVICE

#ضمن "unicode/resbund.h"
#ضمن "uresimp.h"
#ضمن "cmemory.h"
#ضمن "servloc.h"
#ضمن "ustrfmt.h"
#ضمن "charstr.h"
#ضمن "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(ثابت UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(ثابت Locale& locale, UErrorCode& status) ثابت
{
    ܦܢܐ get(locale, LocaleKey::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t kind, UErrorCode& status) ثابت
{
    ܦܢܐ get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, Locale* actualReturn, UErrorCode& status) ثابت
{
    ܦܢܐ get(locale, LocaleKey::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) ثابت
{
    UObject* result = nullptr;
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    ܐܢ (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } ܐܚܪܢܐ {
        ICUServiceKey* key = createKey(&locName, kind, status);
        ܐܢ (key) {
            ܐܢ (actualReturn == nullptr) {
                result = getKey(*key, status);
            } ܐܚܪܢܐ {
                UnicodeString temp;
                result = getKey(*key, &temp, status);

                ܐܢ (result != nullptr) {
                    key->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete key;
        }
    }
    ܦܢܐ result;
}


URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    ܦܢܐ registerInstance(objToAdopt, loc, LocaleKey::KIND_ANY, 
        visible ? LocaleKeyFactory::VISIBLE : LocaleKeyFactory::INVISIBLE, status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت Locale& locale, UErrorCode& status)
{
    ܦܢܐ registerInstance(objToAdopt, locale, LocaleKey::KIND_ANY, LocaleKeyFactory::VISIBLE, status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت Locale& locale, int32_t kind, UErrorCode& status)
{
    ܦܢܐ registerInstance(objToAdopt, locale, kind, LocaleKeyFactory::VISIBLE, status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = جديد SimpleLocaleKeyFactory(objToAdopt, locale, kind, coverage);
    ܐܢ (factory != nullptr) {
        ܦܢܐ registerFactory(factory, status);
    }
    delete objToAdopt;
    ܦܢܐ nullptr;
}

#ܐܢ 0
URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& locale, UErrorCode& status)
{
    ܦܢܐ registerInstance(objToAdopt, locale, LocaleKey::KIND_ANY, LocaleKeyFactory::VISIBLE, status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& locale, UBool visible, UErrorCode& status)
{
    ܦܢܐ registerInstance(objToAdopt, locale, LocaleKey::KIND_ANY,
                            visible ? LocaleKeyFactory::VISIBLE : LocaleKeyFactory::INVISIBLE,
                            status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = جديد SimpleLocaleKeyFactory(objToAdopt, locale, kind, coverage);
    ܐܢ (factory != nullptr) {
        ܦܢܐ registerFactory(factory, status);
    }
    delete objToAdopt;
    ܦܢܐ nullptr;
}
#endif

صنف ServiceEnumeration : عام StringEnumeration {
خاص:
    ثابت ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

خاص:
    ServiceEnumeration(ثابت ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(ثابت ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        ܐܢ(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            لكل(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            ܐܢ(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

عام:
    ساكن ServiceEnumeration* create(ثابت ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = جديد ServiceEnumeration(service, status);
        ܐܢ (U_SUCCESS(status)) {
            ܦܢܐ result;
        }
        delete result;
        ܦܢܐ nullptr;
    }

    virtual ~ServiceEnumeration();

    virtual StringEnumeration *clone() ثابت override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = جديد ServiceEnumeration(*هذا, status);
        ܐܢ(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        ܦܢܐ cl;
    }

    UBool upToDate(UErrorCode& status) ثابت {
        ܐܢ (U_SUCCESS(status)) {
            ܐܢ (_timestamp == _service->getTimestamp()) {
                ܦܢܐ ܫܪܪܐ;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        ܦܢܐ ܕܓܠܐ;
    }

    virtual int32_t count(UErrorCode& status) ثابت override {
        ܦܢܐ upToDate(status) ? _ids.size() : 0;
    }

    virtual ثابت UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        ܐܢ (upToDate(status) && (_pos < _ids.size())) {
            ܦܢܐ static_cast<ثابت UnicodeString*>(_ids[_pos++]);
        }
        ܦܢܐ nullptr;
    }

    virtual فراغ reset(UErrorCode& status) override {
        ܐܢ (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        ܐܢ (U_SUCCESS(status)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, status);
        }
    }

عام:
    ساكن UClassID U_EXPORT2 getStaticClassID();
    virtual UClassID getDynamicClassID() ثابت override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() ثابت U_LIFETIME_BOUND
{
    ܦܢܐ ServiceEnumeration::create(هذا);
}

ثابت UnicodeString&
ICULocaleService::validateFallbackLocale() ثابت
{
    ثابت Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(هذا);
    ساكن UMutex llock;
    {
        Mutex mutex(&llock);
        ܐܢ (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    ܦܢܐ fallbackLocaleName;
}

ICUServiceKey*
ICULocaleService::createKey(ثابت UnicodeString* id, UErrorCode& status) ثابت
{
    ܦܢܐ LocaleKey::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUServiceKey*
ICULocaleService::createKey(ثابت UnicodeString* id, int32_t kind, UErrorCode& status) ثابت
{
    ܦܢܐ LocaleKey::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


