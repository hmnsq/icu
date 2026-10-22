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

#nếu !UCONFIG_NO_SERVICE

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
    trả_về get(locale, LocaleKey::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(const Locale& locale, int32_t kind, UErrorCode& status) const
{
    trả_về get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(const Locale& locale, Locale* actualReturn, UErrorCode& status) const
{
    trả_về get(locale, LocaleKey::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(const Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) const
{
    UObject* result = nullptr;
    nếu (U_FAILURE(status)) {
        trả_về result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    nếu (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } không_thì {
        ICUServiceKey* key = createKey(&locName, kind, status);
        nếu (key) {
            nếu (actualReturn == nullptr) {
                result = getKey(*key, status);
            } không_thì {
                UnicodeString temp;
                result = getKey(*key, &temp, status);

                nếu (result != nullptr) {
                    key->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete key;
        }
    }
    trả_về result;
}


URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    trả_về registerInstance(objToAdopt, loc, LocaleKey::KIND_ANY, 
        visible ? LocaleKeyFactory::VISIBLE : LocaleKeyFactory::INVISIBLE, status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const Locale& locale, UErrorCode& status)
{
    trả_về registerInstance(objToAdopt, locale, LocaleKey::KIND_ANY, LocaleKeyFactory::VISIBLE, status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const Locale& locale, int32_t kind, UErrorCode& status)
{
    trả_về registerInstance(objToAdopt, locale, kind, LocaleKeyFactory::VISIBLE, status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = new SimpleLocaleKeyFactory(objToAdopt, locale, kind, coverage);
    nếu (factory != nullptr) {
        trả_về registerFactory(factory, status);
    }
    delete objToAdopt;
    trả_về nullptr;
}

#nếu 0
URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const UnicodeString& locale, UErrorCode& status)
{
    trả_về registerInstance(objToAdopt, locale, LocaleKey::KIND_ANY, LocaleKeyFactory::VISIBLE, status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const UnicodeString& locale, UBool visible, UErrorCode& status)
{
    trả_về registerInstance(objToAdopt, locale, LocaleKey::KIND_ANY,
                            visible ? LocaleKeyFactory::VISIBLE : LocaleKeyFactory::INVISIBLE,
                            status);
}

URegistryKey
ICULocaleService::registerInstance(UObject* objToAdopt, const UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = new SimpleLocaleKeyFactory(objToAdopt, locale, kind, coverage);
    nếu (factory != nullptr) {
        trả_về registerFactory(factory, status);
    }
    delete objToAdopt;
    trả_về nullptr;
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
        nếu(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            cho(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            nếu(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

public:
    static ServiceEnumeration* create(const ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = new ServiceEnumeration(service, status);
        nếu (U_SUCCESS(status)) {
            trả_về result;
        }
        delete result;
        trả_về nullptr;
    }

    virtual ~ServiceEnumeration();

    virtual StringEnumeration *clone() const override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = new ServiceEnumeration(*this, status);
        nếu(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        trả_về cl;
    }

    UBool upToDate(UErrorCode& status) const {
        nếu (U_SUCCESS(status)) {
            nếu (_timestamp == _service->getTimestamp()) {
                trả_về true;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        trả_về false;
    }

    virtual int32_t count(UErrorCode& status) const override {
        trả_về upToDate(status) ? _ids.size() : 0;
    }

    virtual const UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        nếu (upToDate(status) && (_pos < _ids.size())) {
            trả_về static_cast<const UnicodeString*>(_ids[_pos++]);
        }
        trả_về nullptr;
    }

    virtual void reset(UErrorCode& status) override {
        nếu (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        nếu (U_SUCCESS(status)) {
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
    trả_về ServiceEnumeration::create(this);
}

const UnicodeString&
ICULocaleService::validateFallbackLocale() const
{
    const Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(this);
    static UMutex llock;
    {
        Mutex mutex(&llock);
        nếu (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    trả_về fallbackLocaleName;
}

ICUServiceKey*
ICULocaleService::createKey(const UnicodeString* id, UErrorCode& status) const
{
    trả_về LocaleKey::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUServiceKey*
ICULocaleService::createKey(const UnicodeString* id, int32_t kind, UErrorCode& status) const
{
    trả_về LocaleKey::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


