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

#إذا !UCONFIG_NO_SERVICE

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
    إرجاع get(locale, Locale_مفتاح::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t kind, UErrorCode& status) ثابت
{
    إرجاع get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, Locale* actualReturn, UErrorCode& status) ثابت
{
    إرجاع get(locale, Locale_مفتاح::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) ثابت
{
    UObject* result = nullptr;
    إذا (U_FAILURE(status)) {
        إرجاع result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    إذا (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } وإلا {
        ICUService_مفتاح* مفتاح = create_مفتاح(&locName, kind, status);
        إذا (مفتاح) {
            إذا (actualReturn == nullptr) {
                result = get_مفتاح(*مفتاح, status);
            } وإلا {
                UnicodeString temp;
                result = get_مفتاح(*مفتاح, &temp, status);

                إذا (result != nullptr) {
                    مفتاح->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete مفتاح;
        }
    }
    إرجاع result;
}


URegistry_مفتاح
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    إرجاع registerInstance(objToAdopt, loc, Locale_مفتاح::KIND_ANY, 
        visible ? Locale_مفتاح_Factory::VISIBLE : Locale_مفتاح_Factory::INVISIBLE, status);
}

URegistry_مفتاح
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت Locale& locale, UErrorCode& status)
{
    إرجاع registerInstance(objToAdopt, locale, Locale_مفتاح::KIND_ANY, Locale_مفتاح_Factory::VISIBLE, status);
}

URegistry_مفتاح
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت Locale& locale, int32_t kind, UErrorCode& status)
{
    إرجاع registerInstance(objToAdopt, locale, kind, Locale_مفتاح_Factory::VISIBLE, status);
}

URegistry_مفتاح
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = جديد Simple_Locale_مفتاح_Factory(objToAdopt, locale, kind, coverage);
    إذا (factory != nullptr) {
        إرجاع registerFactory(factory, status);
    }
    delete objToAdopt;
    إرجاع nullptr;
}

#إذا 0
URegistry_مفتاح
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& locale, UErrorCode& status)
{
    إرجاع registerInstance(objToAdopt, locale, Locale_مفتاح::KIND_ANY, Locale_مفتاح_Factory::VISIBLE, status);
}

URegistry_مفتاح
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& locale, UBool visible, UErrorCode& status)
{
    إرجاع registerInstance(objToAdopt, locale, Locale_مفتاح::KIND_ANY,
                            visible ? Locale_مفتاح_Factory::VISIBLE : Locale_مفتاح_Factory::INVISIBLE,
                            status);
}

URegistry_مفتاح
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = جديد Simple_Locale_مفتاح_Factory(objToAdopt, locale, kind, coverage);
    إذا (factory != nullptr) {
        إرجاع registerFactory(factory, status);
    }
    delete objToAdopt;
    إرجاع nullptr;
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
        إذا(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            لكل(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            إذا(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

عام:
    ساكن ServiceEnumeration* create(ثابت ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = جديد ServiceEnumeration(service, status);
        إذا (U_SUCCESS(status)) {
            إرجاع result;
        }
        delete result;
        إرجاع nullptr;
    }

    virtual ~ServiceEnumeration();

    virtual StringEnumeration *clone() ثابت override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = جديد ServiceEnumeration(*هذا, status);
        إذا(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        إرجاع cl;
    }

    UBool upToDate(UErrorCode& status) ثابت {
        إذا (U_SUCCESS(status)) {
            إذا (_timestamp == _service->getTimestamp()) {
                إرجاع true;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        إرجاع خطأ;
    }

    virtual int32_t count(UErrorCode& status) ثابت override {
        إرجاع upToDate(status) ? _ids.size() : 0;
    }

    virtual ثابت UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        إذا (upToDate(status) && (_pos < _ids.size())) {
            إرجاع static_cast<ثابت UnicodeString*>(_ids[_pos++]);
        }
        إرجاع nullptr;
    }

    virtual فراغ reset(UErrorCode& status) override {
        إذا (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        إذا (U_SUCCESS(status)) {
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
    إرجاع ServiceEnumeration::create(هذا);
}

ثابت UnicodeString&
ICULocaleService::validateFallbackLocale() ثابت
{
    ثابت Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(هذا);
    ساكن UMutex llock;
    {
        Mutex mutex(&llock);
        إذا (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    إرجاع fallbackLocaleName;
}

ICUService_مفتاح*
ICULocaleService::create_مفتاح(ثابت UnicodeString* id, UErrorCode& status) ثابت
{
    إرجاع Locale_مفتاح::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_مفتاح*
ICULocaleService::create_مفتاح(ثابت UnicodeString* id, int32_t kind, UErrorCode& status) ثابت
{
    إرجاع Locale_مفتاح::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


