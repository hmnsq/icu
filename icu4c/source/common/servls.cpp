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
#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_SERVICE

#অন্তর্ভাৱ "unicode/resbund.h"
#অন্তর্ভাৱ "uresimp.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "servloc.h"
#অন্তর্ভাৱ "ustrfmt.h"
#অন্তর্ভাৱ "charstr.h"
#অন্তর্ভাৱ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(ধ্রুৱ UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(ধ্রুৱ Locale& locale, UErrorCode& status) ধ্রুৱ
{
    ঘূৰাই get(locale, Locale_চাবি::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(ধ্রুৱ Locale& locale, int32_t kind, UErrorCode& status) ধ্রুৱ
{
    ঘূৰাই get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(ধ্রুৱ Locale& locale, Locale* actualReturn, UErrorCode& status) ধ্রুৱ
{
    ঘূৰাই get(locale, Locale_চাবি::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(ধ্রুৱ Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) ধ্রুৱ
{
    UObject* result = nullptr;
    যদি (U_FAILURE(status)) {
        ঘূৰাই result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    যদি (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } অন্যথা {
        ICUService_চাবি* চাবি = create_চাবি(&locName, kind, status);
        যদি (চাবি) {
            যদি (actualReturn == nullptr) {
                result = get_চাবি(*চাবি, status);
            } অন্যথা {
                UnicodeString temp;
                result = get_চাবি(*চাবি, &temp, status);

                যদি (result != nullptr) {
                    চাবি->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete চাবি;
        }
    }
    ঘূৰাই result;
}


URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুৱ UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    ঘূৰাই registerInstance(objToAdopt, loc, Locale_চাবি::KIND_ANY, 
        visible ? Locale_চাবি_Factory::VISIBLE : Locale_চাবি_Factory::INVISIBLE, status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুৱ Locale& locale, UErrorCode& status)
{
    ঘূৰাই registerInstance(objToAdopt, locale, Locale_চাবি::KIND_ANY, Locale_চাবি_Factory::VISIBLE, status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুৱ Locale& locale, int32_t kind, UErrorCode& status)
{
    ঘূৰাই registerInstance(objToAdopt, locale, kind, Locale_চাবি_Factory::VISIBLE, status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুৱ Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = নূতন Simple_Locale_চাবি_Factory(objToAdopt, locale, kind, coverage);
    যদি (factory != nullptr) {
        ঘূৰাই registerFactory(factory, status);
    }
    delete objToAdopt;
    ঘূৰাই nullptr;
}

#যদি 0
URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুৱ UnicodeString& locale, UErrorCode& status)
{
    ঘূৰাই registerInstance(objToAdopt, locale, Locale_চাবি::KIND_ANY, Locale_চাবি_Factory::VISIBLE, status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুৱ UnicodeString& locale, UBool visible, UErrorCode& status)
{
    ঘূৰাই registerInstance(objToAdopt, locale, Locale_চাবি::KIND_ANY,
                            visible ? Locale_চাবি_Factory::VISIBLE : Locale_চাবি_Factory::INVISIBLE,
                            status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুৱ UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = নূতন Simple_Locale_চাবি_Factory(objToAdopt, locale, kind, coverage);
    যদি (factory != nullptr) {
        ঘূৰাই registerFactory(factory, status);
    }
    delete objToAdopt;
    ঘূৰাই nullptr;
}
#endif

শ্ৰেণী ServiceEnumeration : সার্ৱজনিক StringEnumeration {
গুপ্ত:
    ধ্রুৱ ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

গুপ্ত:
    ServiceEnumeration(ধ্রুৱ ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(ধ্রুৱ ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        যদি(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            বাবে(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            যদি(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

সার্ৱজনিক:
    স্থির ServiceEnumeration* create(ধ্রুৱ ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = নূতন ServiceEnumeration(service, status);
        যদি (U_SUCCESS(status)) {
            ঘূৰাই result;
        }
        delete result;
        ঘূৰাই nullptr;
    }

    আভাস ~ServiceEnumeration();

    আভাস StringEnumeration *clone() ধ্রুৱ override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = নূতন ServiceEnumeration(*এতত্, status);
        যদি(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        ঘূৰাই cl;
    }

    UBool upToDate(UErrorCode& status) ধ্রুৱ {
        যদি (U_SUCCESS(status)) {
            যদি (_timestamp == _service->getTimestamp()) {
                ঘূৰাই সঁচা;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        ঘূৰাই মিছা;
    }

    আভাস int32_t count(UErrorCode& status) ধ্রুৱ override {
        ঘূৰাই upToDate(status) ? _ids.size() : 0;
    }

    আভাস ধ্রুৱ UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        যদি (upToDate(status) && (_pos < _ids.size())) {
            ঘূৰাই static_cast<ধ্রুৱ UnicodeString*>(_ids[_pos++]);
        }
        ঘূৰাই nullptr;
    }

    আভাস শূন্য reset(UErrorCode& status) override {
        যদি (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        যদি (U_SUCCESS(status)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, status);
        }
    }

সার্ৱজনিক:
    স্থির UClassID U_EXPORT2 getStaticClassID();
    আভাস UClassID getDynamicClassID() ধ্রুৱ override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() ধ্রুৱ U_LIFETIME_BOUND
{
    ঘূৰাই ServiceEnumeration::create(এতত্);
}

ধ্রুৱ UnicodeString&
ICULocaleService::validateFallbackLocale() ধ্রুৱ
{
    ধ্রুৱ Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(এতত্);
    স্থির UMutex llock;
    {
        Mutex mutex(&llock);
        যদি (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    ঘূৰাই fallbackLocaleName;
}

ICUService_চাবি*
ICULocaleService::create_চাবি(ধ্রুৱ UnicodeString* id, UErrorCode& status) ধ্রুৱ
{
    ঘূৰাই Locale_চাবি::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_চাবি*
ICULocaleService::create_চাবি(ধ্রুৱ UnicodeString* id, int32_t kind, UErrorCode& status) ধ্রুৱ
{
    ঘূৰাই Locale_চাবি::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


