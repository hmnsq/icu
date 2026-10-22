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

#यदि !UCONFIG_NO_SERVICE

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
    प्रत्यागम get(locale, Locale_किल्ली::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(ध्रुव Locale& locale, int32_t kind, UErrorCode& status) ध्रुव
{
    प्रत्यागम get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(ध्रुव Locale& locale, Locale* actualReturn, UErrorCode& status) ध्रुव
{
    प्रत्यागम get(locale, Locale_किल्ली::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(ध्रुव Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) ध्रुव
{
    UObject* result = nullptr;
    यदि (U_FAILURE(status)) {
        प्रत्यागम result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    यदि (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा {
        ICUService_किल्ली* किल्ली = create_किल्ली(&locName, kind, status);
        यदि (किल्ली) {
            यदि (actualReturn == nullptr) {
                result = get_किल्ली(*किल्ली, status);
            } अन्यथा {
                UnicodeString temp;
                result = get_किल्ली(*किल्ली, &temp, status);

                यदि (result != nullptr) {
                    किल्ली->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete किल्ली;
        }
    }
    प्रत्यागम result;
}


URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    प्रत्यागम registerInstance(objToAdopt, loc, Locale_किल्ली::KIND_ANY, 
        visible ? Locale_किल्ली_Factory::VISIBLE : Locale_किल्ली_Factory::INVISIBLE, status);
}

URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव Locale& locale, UErrorCode& status)
{
    प्रत्यागम registerInstance(objToAdopt, locale, Locale_किल्ली::KIND_ANY, Locale_किल्ली_Factory::VISIBLE, status);
}

URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव Locale& locale, int32_t kind, UErrorCode& status)
{
    प्रत्यागम registerInstance(objToAdopt, locale, kind, Locale_किल्ली_Factory::VISIBLE, status);
}

URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = नूतन Simple_Locale_किल्ली_Factory(objToAdopt, locale, kind, coverage);
    यदि (factory != nullptr) {
        प्रत्यागम registerFactory(factory, status);
    }
    delete objToAdopt;
    प्रत्यागम nullptr;
}

#यदि 0
URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& locale, UErrorCode& status)
{
    प्रत्यागम registerInstance(objToAdopt, locale, Locale_किल्ली::KIND_ANY, Locale_किल्ली_Factory::VISIBLE, status);
}

URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& locale, UBool visible, UErrorCode& status)
{
    प्रत्यागम registerInstance(objToAdopt, locale, Locale_किल्ली::KIND_ANY,
                            visible ? Locale_किल्ली_Factory::VISIBLE : Locale_किल्ली_Factory::INVISIBLE,
                            status);
}

URegistry_किल्ली
ICULocaleService::registerInstance(UObject* objToAdopt, ध्रुव UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = नूतन Simple_Locale_किल्ली_Factory(objToAdopt, locale, kind, coverage);
    यदि (factory != nullptr) {
        प्रत्यागम registerFactory(factory, status);
    }
    delete objToAdopt;
    प्रत्यागम nullptr;
}
#endif

वर्ग ServiceEnumeration : सार्वजनिक StringEnumeration {
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
        यदि(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            प्रति(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            यदि(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

सार्वजनिक:
    स्थिर ServiceEnumeration* create(ध्रुव ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = नूतन ServiceEnumeration(service, status);
        यदि (U_SUCCESS(status)) {
            प्रत्यागम result;
        }
        delete result;
        प्रत्यागम nullptr;
    }

    आभास ~ServiceEnumeration();

    आभास StringEnumeration *clone() ध्रुव override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = नूतन ServiceEnumeration(*एतत्, status);
        यदि(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        प्रत्यागम cl;
    }

    UBool upToDate(UErrorCode& status) ध्रुव {
        यदि (U_SUCCESS(status)) {
            यदि (_timestamp == _service->getTimestamp()) {
                प्रत्यागम सत्य;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        प्रत्यागम असत्य;
    }

    आभास int32_t count(UErrorCode& status) ध्रुव override {
        प्रत्यागम upToDate(status) ? _ids.size() : 0;
    }

    आभास ध्रुव UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        यदि (upToDate(status) && (_pos < _ids.size())) {
            प्रत्यागम static_cast<ध्रुव UnicodeString*>(_ids[_pos++]);
        }
        प्रत्यागम nullptr;
    }

    आभास शून्य reset(UErrorCode& status) override {
        यदि (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        यदि (U_SUCCESS(status)) {
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
    प्रत्यागम ServiceEnumeration::create(एतत्);
}

ध्रुव UnicodeString&
ICULocaleService::validateFallbackLocale() ध्रुव
{
    ध्रुव Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(एतत्);
    स्थिर UMutex llock;
    {
        Mutex mutex(&llock);
        यदि (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    प्रत्यागम fallbackLocaleName;
}

ICUService_किल्ली*
ICULocaleService::create_किल्ली(ध्रुव UnicodeString* id, UErrorCode& status) ध्रुव
{
    प्रत्यागम Locale_किल्ली::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_किल्ली*
ICULocaleService::create_किल्ली(ध्रुव UnicodeString* id, int32_t kind, UErrorCode& status) ध्रुव
{
    प्रत्यागम Locale_किल्ली::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


