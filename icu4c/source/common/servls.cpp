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

ICULocaleService::ICULocaleService(ধ্রুব UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(ধ্রুব Locale& locale, UErrorCode& status) ধ্রুব
{
    প্রত্যাগম get(locale, Locale_চাবি::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(ধ্রুব Locale& locale, int32_t kind, UErrorCode& status) ধ্রুব
{
    প্রত্যাগম get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(ধ্রুব Locale& locale, Locale* actualReturn, UErrorCode& status) ধ্রুব
{
    প্রত্যাগম get(locale, Locale_চাবি::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(ধ্রুব Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) ধ্রুব
{
    UObject* result = nullptr;
    যদি (U_FAILURE(status)) {
        প্রত্যাগম result;
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
    প্রত্যাগম result;
}


URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুব UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    প্রত্যাগম registerInstance(objToAdopt, loc, Locale_চাবি::KIND_ANY, 
        visible ? Locale_চাবি_Factory::VISIBLE : Locale_চাবি_Factory::INVISIBLE, status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুব Locale& locale, UErrorCode& status)
{
    প্রত্যাগম registerInstance(objToAdopt, locale, Locale_চাবি::KIND_ANY, Locale_চাবি_Factory::VISIBLE, status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুব Locale& locale, int32_t kind, UErrorCode& status)
{
    প্রত্যাগম registerInstance(objToAdopt, locale, kind, Locale_চাবি_Factory::VISIBLE, status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুব Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = নূতন Simple_Locale_চাবি_Factory(objToAdopt, locale, kind, coverage);
    যদি (factory != nullptr) {
        প্রত্যাগম registerFactory(factory, status);
    }
    delete objToAdopt;
    প্রত্যাগম nullptr;
}

#যদি 0
URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুব UnicodeString& locale, UErrorCode& status)
{
    প্রত্যাগম registerInstance(objToAdopt, locale, Locale_চাবি::KIND_ANY, Locale_চাবি_Factory::VISIBLE, status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুব UnicodeString& locale, UBool visible, UErrorCode& status)
{
    প্রত্যাগম registerInstance(objToAdopt, locale, Locale_চাবি::KIND_ANY,
                            visible ? Locale_চাবি_Factory::VISIBLE : Locale_চাবি_Factory::INVISIBLE,
                            status);
}

URegistry_চাবি
ICULocaleService::registerInstance(UObject* objToAdopt, ধ্রুব UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = নূতন Simple_Locale_চাবি_Factory(objToAdopt, locale, kind, coverage);
    যদি (factory != nullptr) {
        প্রত্যাগম registerFactory(factory, status);
    }
    delete objToAdopt;
    প্রত্যাগম nullptr;
}
#endif

বর্গ ServiceEnumeration : সার্বজনিক StringEnumeration {
গুপ্ত:
    ধ্রুব ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

গুপ্ত:
    ServiceEnumeration(ধ্রুব ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(ধ্রুব ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        যদি(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            প্রতি(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            যদি(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

সার্বজনিক:
    স্থির ServiceEnumeration* create(ধ্রুব ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = নূতন ServiceEnumeration(service, status);
        যদি (U_SUCCESS(status)) {
            প্রত্যাগম result;
        }
        delete result;
        প্রত্যাগম nullptr;
    }

    আভাস ~ServiceEnumeration();

    আভাস StringEnumeration *clone() ধ্রুব override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = নূতন ServiceEnumeration(*এতত্, status);
        যদি(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        প্রত্যাগম cl;
    }

    UBool upToDate(UErrorCode& status) ধ্রুব {
        যদি (U_SUCCESS(status)) {
            যদি (_timestamp == _service->getTimestamp()) {
                প্রত্যাগম সত্য;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        প্রত্যাগম অসত্য;
    }

    আভাস int32_t count(UErrorCode& status) ধ্রুব override {
        প্রত্যাগম upToDate(status) ? _ids.size() : 0;
    }

    আভাস ধ্রুব UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        যদি (upToDate(status) && (_pos < _ids.size())) {
            প্রত্যাগম static_cast<ধ্রুব UnicodeString*>(_ids[_pos++]);
        }
        প্রত্যাগম nullptr;
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

সার্বজনিক:
    স্থির UClassID U_EXPORT2 getStaticClassID();
    আভাস UClassID getDynamicClassID() ধ্রুব override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() ধ্রুব U_LIFETIME_BOUND
{
    প্রত্যাগম ServiceEnumeration::create(এতত্);
}

ধ্রুব UnicodeString&
ICULocaleService::validateFallbackLocale() ধ্রুব
{
    ধ্রুব Locale&     loc    = Locale::getDefault();
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
    প্রত্যাগম fallbackLocaleName;
}

ICUService_চাবি*
ICULocaleService::create_চাবি(ধ্রুব UnicodeString* id, UErrorCode& status) ধ্রুব
{
    প্রত্যাগম Locale_চাবি::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_চাবি*
ICULocaleService::create_চাবি(ধ্রুব UnicodeString* id, int32_t kind, UErrorCode& status) ধ্রুব
{
    প্রত্যাগম Locale_চাবি::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


