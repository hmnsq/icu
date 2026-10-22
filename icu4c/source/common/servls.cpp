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
#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_SERVICE

#অন্তর্ভাব "unicode/resbund.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "servloc.h"
#অন্তর্ভাব "ustrfmt.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(ধ্রুবক UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(ধ্রুবক Locale& locale, UErrorCode& status) ধ্রুবক
{
    ফেরত get(locale, Locale_চাবি::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(ধ্রুবক Locale& locale, int32_t kind, UErrorCode& status) ধ্রুবক
{
    ফেরত get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(ধ্রুবক Locale& locale, Locale* actualReturn, UErrorCode& status) ধ্রুবক
{
    ফেরত get(locale, Locale_চাবি::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(ধ্রুবক Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) ধ্রুবক
{
    UObject* ফলাফল = nullptr;
    যদি (U_FAILURE(status)) {
        ফেরত ফলাফল;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    যদি (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } অন্যথা {
        ICUService_চাবি* চাবি = create_চাবি(&locName, kind, status);
        যদি (চাবি) {
            যদি (actualReturn == nullptr) {
                ফলাফল = get_চাবি(*চাবি, status);
            } অন্যথা {
                UnicodeString অস্থায়ী;
                ফলাফল = get_চাবি(*চাবি, &অস্থায়ী, status);

                যদি (ফলাফল != nullptr) {
                    চাবি->parseSuffix(অস্থায়ী);
                    LocaleUtility::initLocaleFromName(অস্থায়ী, *actualReturn);
                }
            }
            delete চাবি;
        }
    }
    ফেরত ফলাফল;
}


URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুবক UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    ফেরত registerInstance(objToAdopt, loc, Locale_চাবি::KIND_ANY, 
        visible ? Locale_চাবি_Factory::VISIBLE : Locale_চাবি_Factory::INVISIBLE, status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুবক Locale& locale, UErrorCode& status)
{
    ফেরত registerInstance(objToAdopt, locale, Locale_চাবি::KIND_ANY, Locale_চাবি_Factory::VISIBLE, status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুবক Locale& locale, int32_t kind, UErrorCode& status)
{
    ফেরত registerInstance(objToAdopt, locale, kind, Locale_চাবি_Factory::VISIBLE, status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুবক Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = নতুন Simple_Locale_চাবি_Factory(objToAdopt, locale, kind, coverage);
    যদি (factory != nullptr) {
        ফেরত registerFactory(factory, status);
    }
    delete objToAdopt;
    ফেরত nullptr;
}

#যদি 0
URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুবক UnicodeString& locale, UErrorCode& status)
{
    ফেরত registerInstance(objToAdopt, locale, Locale_চাবি::KIND_ANY, Locale_চাবি_Factory::VISIBLE, status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুবক UnicodeString& locale, UBool visible, UErrorCode& status)
{
    ফেরত registerInstance(objToAdopt, locale, Locale_চাবি::KIND_ANY,
                            visible ? Locale_চাবি_Factory::VISIBLE : Locale_চাবি_Factory::INVISIBLE,
                            status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুবক UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = নতুন Simple_Locale_চাবি_Factory(objToAdopt, locale, kind, coverage);
    যদি (factory != nullptr) {
        ফেরত registerFactory(factory, status);
    }
    delete objToAdopt;
    ফেরত nullptr;
}
#endif

শ্রেণী ServiceEnumeration : সর্বজনীন StringEnumeration {
ব্যক্তিগত:
    ধ্রুবক ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

ব্যক্তিগত:
    ServiceEnumeration(ধ্রুবক ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(ধ্রুবক ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        যদি(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            জন্য(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            যদি(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

সর্বজনীন:
    স্থির ServiceEnumeration* create(ধ্রুবক ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* ফলাফল = নতুন ServiceEnumeration(service, status);
        যদি (U_SUCCESS(status)) {
            ফেরত ফলাফল;
        }
        delete ফলাফল;
        ফেরত nullptr;
    }

    আভাসী ~ServiceEnumeration();

    আভাসী StringEnumeration *clone() ধ্রুবক override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = নতুন ServiceEnumeration(*এই, status);
        যদি(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        ফেরত cl;
    }

    UBool upToDate(UErrorCode& status) ধ্রুবক {
        যদি (U_SUCCESS(status)) {
            যদি (_timestamp == _service->getTimestamp()) {
                ফেরত সত্য;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        ফেরত মিথ্যা;
    }

    আভাসী int32_t count(UErrorCode& status) ধ্রুবক override {
        ফেরত upToDate(status) ? _ids.size() : 0;
    }

    আভাসী ধ্রুবক UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        যদি (upToDate(status) && (_pos < _ids.size())) {
            ফেরত static_cast<ধ্রুবক UnicodeString*>(_ids[_pos++]);
        }
        ফেরত nullptr;
    }

    আভাসী শূন্য reset(UErrorCode& status) override {
        যদি (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        যদি (U_SUCCESS(status)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, status);
        }
    }

সর্বজনীন:
    স্থির UClassID U_EXPORT2 getStaticClassID();
    আভাসী UClassID getDynamicClassID() ধ্রুবক override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() ধ্রুবক U_LIFETIME_BOUND
{
    ফেরত ServiceEnumeration::create(এই);
}

ধ্রুবক UnicodeString&
ICULocaleService::validateFallbackLocale() ধ্রুবক
{
    ধ্রুবক Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(এই);
    স্থির UMutex llock;
    {
        Mutex mutex(&llock);
        যদি (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    ফেরত fallbackLocaleName;
}

ICUService_চাবি*
ICULocaleService::create_চাবি(ধ্রুবক UnicodeString* id, UErrorCode& status) ধ্রুবক
{
    ফেরত Locale_চাবি::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_চাবি*
ICULocaleService::create_চাবি(ধ্রুবক UnicodeString* id, int32_t kind, UErrorCode& status) ধ্রুবক
{
    ফেরত Locale_চাবি::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


