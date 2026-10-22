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
#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_SERVICE

#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "servloc.h"
#अन्तर्भाव "ustrfmt.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(स्थिरांक UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(स्थिरांक Locale& locale, UErrorCode& status) स्थिरांक
{
    परत get(locale, Locale_किल्ली::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(स्थिरांक Locale& locale, int32_t kind, UErrorCode& status) स्थिरांक
{
    परत get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(स्थिरांक Locale& locale, Locale* actualReturn, UErrorCode& status) स्थिरांक
{
    परत get(locale, Locale_किल्ली::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(स्थिरांक Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) स्थिरांक
{
    UObject* result = nullptr;
    जर (U_FAILURE(status)) {
        परत result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    जर (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा {
        ICUService_किल्ली* किल्ली = create_किल्ली(&locName, kind, status);
        जर (किल्ली) {
            जर (actualReturn == nullptr) {
                result = get_किल्ली(*किल्ली, status);
            } अन्यथा {
                UnicodeString temp;
                result = get_किल्ली(*किल्ली, &temp, status);

                जर (result != nullptr) {
                    किल्ली->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete किल्ली;
        }
    }
    परत result;
}


URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिरांक UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    परत registerInstance(objToAdopt, loc, Locale_किल्ली::KIND_ANY, 
        visible ? Locale_किल्ली_Factory::VISIBLE : Locale_किल्ली_Factory::INVISIBLE, status);
}

URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिरांक Locale& locale, UErrorCode& status)
{
    परत registerInstance(objToAdopt, locale, Locale_किल्ली::KIND_ANY, Locale_किल्ली_Factory::VISIBLE, status);
}

URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिरांक Locale& locale, int32_t kind, UErrorCode& status)
{
    परत registerInstance(objToAdopt, locale, kind, Locale_किल्ली_Factory::VISIBLE, status);
}

URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिरांक Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = नवीन Simple_Locale_किल्ली_Factory(objToAdopt, locale, kind, coverage);
    जर (factory != nullptr) {
        परत registerFactory(factory, status);
    }
    delete objToAdopt;
    परत nullptr;
}

#जर 0
URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिरांक UnicodeString& locale, UErrorCode& status)
{
    परत registerInstance(objToAdopt, locale, Locale_किल्ली::KIND_ANY, Locale_किल्ली_Factory::VISIBLE, status);
}

URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिरांक UnicodeString& locale, UBool visible, UErrorCode& status)
{
    परत registerInstance(objToAdopt, locale, Locale_किल्ली::KIND_ANY,
                            visible ? Locale_किल्ली_Factory::VISIBLE : Locale_किल्ली_Factory::INVISIBLE,
                            status);
}

URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिरांक UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = नवीन Simple_Locale_किल्ली_Factory(objToAdopt, locale, kind, coverage);
    जर (factory != nullptr) {
        परत registerFactory(factory, status);
    }
    delete objToAdopt;
    परत nullptr;
}
#endif

वर्ग ServiceEnumeration : सार्वजनिक StringEnumeration {
खाजगी:
    स्थिरांक ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

खाजगी:
    ServiceEnumeration(स्थिरांक ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(स्थिरांक ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        जर(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            साठी(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            जर(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

सार्वजनिक:
    स्थिर ServiceEnumeration* create(स्थिरांक ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = नवीन ServiceEnumeration(service, status);
        जर (U_SUCCESS(status)) {
            परत result;
        }
        delete result;
        परत nullptr;
    }

    आभासी ~ServiceEnumeration();

    आभासी StringEnumeration *clone() स्थिरांक override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = नवीन ServiceEnumeration(*हे, status);
        जर(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        परत cl;
    }

    UBool upToDate(UErrorCode& status) स्थिरांक {
        जर (U_SUCCESS(status)) {
            जर (_timestamp == _service->getTimestamp()) {
                परत सत्य;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        परत असत्य;
    }

    आभासी int32_t count(UErrorCode& status) स्थिरांक override {
        परत upToDate(status) ? _ids.size() : 0;
    }

    आभासी स्थिरांक UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        जर (upToDate(status) && (_pos < _ids.size())) {
            परत static_cast<स्थिरांक UnicodeString*>(_ids[_pos++]);
        }
        परत nullptr;
    }

    आभासी शून्य reset(UErrorCode& status) override {
        जर (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        जर (U_SUCCESS(status)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, status);
        }
    }

सार्वजनिक:
    स्थिर UClassID U_EXPORT2 getStaticClassID();
    आभासी UClassID getDynamicClassID() स्थिरांक override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() स्थिरांक U_LIFETIME_BOUND
{
    परत ServiceEnumeration::create(हे);
}

स्थिरांक UnicodeString&
ICULocaleService::validateFallbackLocale() स्थिरांक
{
    स्थिरांक Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(हे);
    स्थिर UMutex llock;
    {
        Mutex mutex(&llock);
        जर (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    परत fallbackLocaleName;
}

ICUService_किल्ली*
ICULocaleService::create_किल्ली(स्थिरांक UnicodeString* id, UErrorCode& status) स्थिरांक
{
    परत Locale_किल्ली::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_किल्ली*
ICULocaleService::create_किल्ली(स्थिरांक UnicodeString* id, int32_t kind, UErrorCode& status) स्थिरांक
{
    परत Locale_किल्ली::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


