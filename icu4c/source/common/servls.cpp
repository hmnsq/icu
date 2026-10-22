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
#include "unicode/utypes.h"

#ប្រសិនបើ !UCONFIG_NO_SERVICE

#include "unicode/resbund.h"
#include "uresimp.h"
#include "cmemory.h"
#include "servloc.h"
#include "ustrfmt.h"
#include "charstr.h"
#include "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(const UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(const Locale& locale, UErrorCode& status) const
{
    ត្រឡប់ get(locale, LocaleKey::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(const Locale& locale, int32_t kind, UErrorCode& status) const
{
    ត្រឡប់ get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(const Locale& locale, Locale* actualReturn, UErrorCode& status) const
{
    ត្រឡប់ get(locale, LocaleKey::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(const Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) const
{
    UObject* result = nullptr;
    ប្រសិនបើ (U_FAILURE(status)) {
        ត្រឡប់ result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    ប្រសិនបើ (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } បើមិនដូច្នេះ {
        ICUServiceKey* key = createKey(&locName, kind, status);
        ប្រសិនបើ (key) {
            ប្រសិនបើ (actualReturn == nullptr) {
                result = getKey(*key, status);
            } បើមិនដូច្នេះ {
                UnicodeString temp;
                result = getKey(*key, &temp, status);

                ប្រសិនបើ (result != nullptr) {
                    key->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete key;
        }
    }
    ត្រឡប់ result;
}


URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    ត្រឡប់ registerInstance(objToAdopt, loc, LocaleKey::KIND_ANY, 
        visible ? LocaleKeyFactory::VISIBLE : LocaleKeyFactory::INVISIBLE, status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const Locale& locale, UErrorCode& status)
{
    ត្រឡប់ registerInstance(objToAdopt, locale, LocaleKey::KIND_ANY, LocaleKeyFactory::VISIBLE, status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const Locale& locale, int32_t kind, UErrorCode& status)
{
    ត្រឡប់ registerInstance(objToAdopt, locale, kind, LocaleKeyFactory::VISIBLE, status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = new SimpleLocaleKeyFactory(objToAdopt, locale, kind, coverage);
    ប្រសិនបើ (factory != nullptr) {
        ត្រឡប់ registerFactory(factory, status);
    }
    delete objToAdopt;
    ត្រឡប់ nullptr;
}

#ប្រសិនបើ 0
URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const UnicodeString& locale, UErrorCode& status)
{
    ត្រឡប់ registerInstance(objToAdopt, locale, LocaleKey::KIND_ANY, LocaleKeyFactory::VISIBLE, status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const UnicodeString& locale, UBool visible, UErrorCode& status)
{
    ត្រឡប់ registerInstance(objToAdopt, locale, LocaleKey::KIND_ANY,
                            visible ? LocaleKeyFactory::VISIBLE : LocaleKeyFactory::INVISIBLE,
                            status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = new SimpleLocaleKeyFactory(objToAdopt, locale, kind, coverage);
    ប្រសិនបើ (factory != nullptr) {
        ត្រឡប់ registerFactory(factory, status);
    }
    delete objToAdopt;
    ត្រឡប់ nullptr;
}
#endif

class ServiceEnumeration : public StringEnumeration {
private:
    const ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

private:
    ServiceEnumeration(const ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(const ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        ប្រសិនបើ(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            សម្រាប់(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            ប្រសិនបើ(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

public:
    static ServiceEnumeration* create(const ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = new ServiceEnumeration(service, status);
        ប្រសិនបើ (U_SUCCESS(status)) {
            ត្រឡប់ result;
        }
        delete result;
        ត្រឡប់ nullptr;
    }

    virtual ~ServiceEnumeration();

    virtual StringEnumeration *clone() const override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = new ServiceEnumeration(*this, status);
        ប្រសិនបើ(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        ត្រឡប់ cl;
    }

    UBool upToDate(UErrorCode& status) const {
        ប្រសិនបើ (U_SUCCESS(status)) {
            ប្រសិនបើ (_timestamp == _service->getTimestamp()) {
                ត្រឡប់ true;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        ត្រឡប់ false;
    }

    virtual int32_t count(UErrorCode& status) const override {
        ត្រឡប់ upToDate(status) ? _ids.size() : 0;
    }

    virtual const UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        ប្រសិនបើ (upToDate(status) && (_pos < _ids.size())) {
            ត្រឡប់ static_cast<const UnicodeString*>(_ids[_pos++]);
        }
        ត្រឡប់ nullptr;
    }

    virtual void reset(UErrorCode& status) override {
        ប្រសិនបើ (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        ប្រសិនបើ (U_SUCCESS(status)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, status);
        }
    }

public:
    static UClassID U_EXPORT2 getStaticClassID();
    virtual UClassID getDynamicClassID() const override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() const U_LIFETIME_BOUND
{
    ត្រឡប់ ServiceEnumeration::create(this);
}

const UnicodeString&
ICULocaleService::validateFallbackLocale() const
{
    const Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(this);
    static UMutex llock;
    {
        Mutex mutex(&llock);
        ប្រសិនបើ (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    ត្រឡប់ fallbackLocaleName;
}

ICUServiceKey*
ICULocaleService::createKey(const UnicodeString* id, UErrorCode& status) const
{
    ត្រឡប់ LocaleKey::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUServiceKey*
ICULocaleService::createKey(const UnicodeString* id, int32_t kind, UErrorCode& status) const
{
    ត្រឡប់ LocaleKey::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


