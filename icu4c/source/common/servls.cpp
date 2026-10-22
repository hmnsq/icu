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

#सचे !UCONFIG_NO_SERVICE

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

ICULocaleService::ICULocaleService(ध्रुव UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(ध्रुव Locale& locale, UErrorCode& status) ध्रुव
{
    पटिनिवत्त get(locale, Locale_कुञ्चिका::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(ध्रुव Locale& locale, int32_t kind, UErrorCode& status) ध्रुव
{
    पटिनिवत्त get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(ध्रुव Locale& locale, Locale* actualReturn, UErrorCode& status) ध्रुव
{
    पटिनिवत्त get(locale, Locale_कुञ्चिका::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(ध्रुव Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) ध्रुव
{
    UObject* result = nullptr;
    सचे (U_FAILURE(status)) {
        पटिनिवत्त result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    सचे (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अञ्ञथा {
        ICUService_कुञ्चिका* कुञ्चिका = create_कुञ्चिका(&locName, kind, status);
        सचे (कुञ्चिका) {
            सचे (actualReturn == nullptr) {
                result = get_कुञ्चिका(*कुञ्चिका, status);
            } अञ्ञथा {
                UnicodeString temp;
                result = get_कुञ्चिका(*कुञ्चिका, &temp, status);

                सचे (result != nullptr) {
                    कुञ्चिका->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete कुञ्चिका;
        }
    }
    पटिनिवत्त result;
}


URegistry_कुञ्चिका
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    पटिनिवत्त registerInstance(objToAdopt, loc, Locale_कुञ्चिका::KIND_ANY, 
        visible ? Locale_कुञ्चिका_Factory::VISIBLE : Locale_कुञ्चिका_Factory::INVISIBLE, status);
}

URegistry_कुञ्चिका
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव Locale& locale, UErrorCode& status)
{
    पटिनिवत्त registerInstance(objToAdopt, locale, Locale_कुञ्चिका::KIND_ANY, Locale_कुञ्चिका_Factory::VISIBLE, status);
}

URegistry_कुञ्चिका
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव Locale& locale, int32_t kind, UErrorCode& status)
{
    पटिनिवत्त registerInstance(objToAdopt, locale, kind, Locale_कुञ्चिका_Factory::VISIBLE, status);
}

URegistry_कुञ्चिका
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = नूतन Simple_Locale_कुञ्चिका_Factory(objToAdopt, locale, kind, coverage);
    सचे (factory != nullptr) {
        पटिनिवत्त registerFactory(factory, status);
    }
    delete objToAdopt;
    पटिनिवत्त nullptr;
}

#सचे 0
URegistry_कुञ्चिका
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& locale, UErrorCode& status)
{
    पटिनिवत्त registerInstance(objToAdopt, locale, Locale_कुञ्चिका::KIND_ANY, Locale_कुञ्चिका_Factory::VISIBLE, status);
}

URegistry_कुञ्चिका
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& locale, UBool visible, UErrorCode& status)
{
    पटिनिवत्त registerInstance(objToAdopt, locale, Locale_कुञ्चिका::KIND_ANY,
                            visible ? Locale_कुञ्चिका_Factory::VISIBLE : Locale_कुञ्चिका_Factory::INVISIBLE,
                            status);
}

URegistry_कुञ्चिका
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = नूतन Simple_Locale_कुञ्चिका_Factory(objToAdopt, locale, kind, coverage);
    सचे (factory != nullptr) {
        पटिनिवत्त registerFactory(factory, status);
    }
    delete objToAdopt;
    पटिनिवत्त nullptr;
}
#endif

वग्ग ServiceEnumeration : सार्वजनिक StringEnumeration {
गुप्त:
    ध्रुव ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

गुप्त:
    ServiceEnumeration(ध्रुव ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(ध्रुव ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        सचे(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            अत्थाय(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            सचे(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

सार्वजनिक:
    स्थिर ServiceEnumeration* create(ध्रुव ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = नूतन ServiceEnumeration(service, status);
        सचे (U_SUCCESS(status)) {
            पटिनिवत्त result;
        }
        delete result;
        पटिनिवत्त nullptr;
    }

    आभास ~ServiceEnumeration();

    आभास StringEnumeration *clone() ध्रुव override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = नूतन ServiceEnumeration(*एतत्, status);
        सचे(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        पटिनिवत्त cl;
    }

    UBool upToDate(UErrorCode& status) ध्रुव {
        सचे (U_SUCCESS(status)) {
            सचे (_timestamp == _service->getTimestamp()) {
                पटिनिवत्त सच्च;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        पटिनिवत्त मिच्छा;
    }

    आभास int32_t count(UErrorCode& status) ध्रुव override {
        पटिनिवत्त upToDate(status) ? _ids.size() : 0;
    }

    आभास ध्रुव UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        सचे (upToDate(status) && (_pos < _ids.size())) {
            पटिनिवत्त static_cast<ध्रुव UnicodeString*>(_ids[_pos++]);
        }
        पटिनिवत्त nullptr;
    }

    आभास शून्य reset(UErrorCode& status) override {
        सचे (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        सचे (U_SUCCESS(status)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, status);
        }
    }

सार्वजनिक:
    स्थिर UClassID U_EXPORT2 getStaticClassID();
    आभास UClassID getDynamicClassID() ध्रुव override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() ध्रुव U_LIFETIME_BOUND
{
    पटिनिवत्त ServiceEnumeration::create(एतत्);
}

ध्रुव UnicodeString&
ICULocaleService::validateFallbackLocale() ध्रुव
{
    ध्रुव Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(एतत्);
    स्थिर UMutex llock;
    {
        Mutex mutex(&llock);
        सचे (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    पटिनिवत्त fallbackLocaleName;
}

ICUService_कुञ्चिका*
ICULocaleService::create_कुञ्चिका(ध्रुव UnicodeString* id, UErrorCode& status) ध्रुव
{
    पटिनिवत्त Locale_कुञ्चिका::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_कुञ्चिका*
ICULocaleService::create_कुञ्चिका(ध्रुव UnicodeString* id, int32_t kind, UErrorCode& status) ध्रुव
{
    पटिनिवत्त Locale_कुञ्चिका::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


