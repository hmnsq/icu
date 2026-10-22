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

ICULocaleService::ICULocaleService(अचर UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(अचर Locale& locale, UErrorCode& status) अचर
{
    फर्काऊ get(locale, Locale_साँचो::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(अचर Locale& locale, int32_t kind, UErrorCode& status) अचर
{
    फर्काऊ get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(अचर Locale& locale, Locale* actualReturn, UErrorCode& status) अचर
{
    फर्काऊ get(locale, Locale_साँचो::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(अचर Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) अचर
{
    UObject* result = nullptr;
    यदि (U_FAILURE(status)) {
        फर्काऊ result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    यदि (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा {
        ICUService_साँचो* साँचो = create_साँचो(&locName, kind, status);
        यदि (साँचो) {
            यदि (actualReturn == nullptr) {
                result = get_साँचो(*साँचो, status);
            } अन्यथा {
                UnicodeString temp;
                result = get_साँचो(*साँचो, &temp, status);

                यदि (result != nullptr) {
                    साँचो->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete साँचो;
        }
    }
    फर्काऊ result;
}


URegistry_साँचो
ICULocaleService::registerInstance(UObject* objToAdopt, अचर UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    फर्काऊ registerInstance(objToAdopt, loc, Locale_साँचो::KIND_ANY, 
        visible ? Locale_साँचो_Factory::VISIBLE : Locale_साँचो_Factory::INVISIBLE, status);
}

URegistry_साँचो
ICULocaleService::registerInstance(UObject* objToAdopt, अचर Locale& locale, UErrorCode& status)
{
    फर्काऊ registerInstance(objToAdopt, locale, Locale_साँचो::KIND_ANY, Locale_साँचो_Factory::VISIBLE, status);
}

URegistry_साँचो
ICULocaleService::registerInstance(UObject* objToAdopt, अचर Locale& locale, int32_t kind, UErrorCode& status)
{
    फर्काऊ registerInstance(objToAdopt, locale, kind, Locale_साँचो_Factory::VISIBLE, status);
}

URegistry_साँचो
ICULocaleService::registerInstance(UObject* objToAdopt, अचर Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = नूतन Simple_Locale_साँचो_Factory(objToAdopt, locale, kind, coverage);
    यदि (factory != nullptr) {
        फर्काऊ registerFactory(factory, status);
    }
    delete objToAdopt;
    फर्काऊ nullptr;
}

#यदि 0
URegistry_साँचो
ICULocaleService::registerInstance(UObject* objToAdopt, अचर UnicodeString& locale, UErrorCode& status)
{
    फर्काऊ registerInstance(objToAdopt, locale, Locale_साँचो::KIND_ANY, Locale_साँचो_Factory::VISIBLE, status);
}

URegistry_साँचो
ICULocaleService::registerInstance(UObject* objToAdopt, अचर UnicodeString& locale, UBool visible, UErrorCode& status)
{
    फर्काऊ registerInstance(objToAdopt, locale, Locale_साँचो::KIND_ANY,
                            visible ? Locale_साँचो_Factory::VISIBLE : Locale_साँचो_Factory::INVISIBLE,
                            status);
}

URegistry_साँचो
ICULocaleService::registerInstance(UObject* objToAdopt, अचर UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = नूतन Simple_Locale_साँचो_Factory(objToAdopt, locale, kind, coverage);
    यदि (factory != nullptr) {
        फर्काऊ registerFactory(factory, status);
    }
    delete objToAdopt;
    फर्काऊ nullptr;
}
#endif

वर्ग ServiceEnumeration : सार्वजनिक StringEnumeration {
गुप्त:
    अचर ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

गुप्त:
    ServiceEnumeration(अचर ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(अचर ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        यदि(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            लागि(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            यदि(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

सार्वजनिक:
    स्थिर ServiceEnumeration* create(अचर ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = नूतन ServiceEnumeration(service, status);
        यदि (U_SUCCESS(status)) {
            फर्काऊ result;
        }
        delete result;
        फर्काऊ nullptr;
    }

    आभास ~ServiceEnumeration();

    आभास StringEnumeration *clone() अचर override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = नूतन ServiceEnumeration(*एतत्, status);
        यदि(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        फर्काऊ cl;
    }

    UBool upToDate(UErrorCode& status) अचर {
        यदि (U_SUCCESS(status)) {
            यदि (_timestamp == _service->getTimestamp()) {
                फर्काऊ सत्य;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        फर्काऊ असत्य;
    }

    आभास int32_t count(UErrorCode& status) अचर override {
        फर्काऊ upToDate(status) ? _ids.size() : 0;
    }

    आभास अचर UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        यदि (upToDate(status) && (_pos < _ids.size())) {
            फर्काऊ static_cast<अचर UnicodeString*>(_ids[_pos++]);
        }
        फर्काऊ nullptr;
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
    आभास UClassID getDynamicClassID() अचर override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() अचर U_LIFETIME_BOUND
{
    फर्काऊ ServiceEnumeration::create(एतत्);
}

अचर UnicodeString&
ICULocaleService::validateFallbackLocale() अचर
{
    अचर Locale&     loc    = Locale::getDefault();
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
    फर्काऊ fallbackLocaleName;
}

ICUService_साँचो*
ICULocaleService::create_साँचो(अचर UnicodeString* id, UErrorCode& status) अचर
{
    फर्काऊ Locale_साँचो::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_साँचो*
ICULocaleService::create_साँचो(अचर UnicodeString* id, int32_t kind, UErrorCode& status) अचर
{
    फर्काऊ Locale_साँचो::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


