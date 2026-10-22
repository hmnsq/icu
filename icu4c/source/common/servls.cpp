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
#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_SERVICE

#شامل "unicode/resbund.h"
#شامل "uresimp.h"
#شامل "cmemory.h"
#شامل "servloc.h"
#شامل "ustrfmt.h"
#شامل "charstr.h"
#شامل "uassert.h"

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
    بازگشت get(locale, Locale_کلید::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t kind, UErrorCode& status) ثابت
{
    بازگشت get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, Locale* actualReturn, UErrorCode& status) ثابت
{
    بازگشت get(locale, Locale_کلید::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) ثابت
{
    UObject* result = nullptr;
    اگر (U_FAILURE(status)) {
        بازگشت result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    اگر (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } وگرنه {
        ICUService_کلید* کلید = create_کلید(&locName, kind, status);
        اگر (کلید) {
            اگر (actualReturn == nullptr) {
                result = get_کلید(*کلید, status);
            } وگرنه {
                UnicodeString temp;
                result = get_کلید(*کلید, &temp, status);

                اگر (result != nullptr) {
                    کلید->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete کلید;
        }
    }
    بازگشت result;
}


URegistry_کلید
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    بازگشت registerInstance(objToAdopt, loc, Locale_کلید::KIND_ANY, 
        visible ? Locale_کلید_Factory::VISIBLE : Locale_کلید_Factory::INVISIBLE, status);
}

URegistry_کلید
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت Locale& locale, UErrorCode& status)
{
    بازگشت registerInstance(objToAdopt, locale, Locale_کلید::KIND_ANY, Locale_کلید_Factory::VISIBLE, status);
}

URegistry_کلید
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت Locale& locale, int32_t kind, UErrorCode& status)
{
    بازگشت registerInstance(objToAdopt, locale, kind, Locale_کلید_Factory::VISIBLE, status);
}

URegistry_کلید
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = جدید Simple_Locale_کلید_Factory(objToAdopt, locale, kind, coverage);
    اگر (factory != nullptr) {
        بازگشت registerFactory(factory, status);
    }
    delete objToAdopt;
    بازگشت nullptr;
}

#اگر 0
URegistry_کلید
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& locale, UErrorCode& status)
{
    بازگشت registerInstance(objToAdopt, locale, Locale_کلید::KIND_ANY, Locale_کلید_Factory::VISIBLE, status);
}

URegistry_کلید
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& locale, UBool visible, UErrorCode& status)
{
    بازگشت registerInstance(objToAdopt, locale, Locale_کلید::KIND_ANY,
                            visible ? Locale_کلید_Factory::VISIBLE : Locale_کلید_Factory::INVISIBLE,
                            status);
}

URegistry_کلید
ICULocaleService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = جدید Simple_Locale_کلید_Factory(objToAdopt, locale, kind, coverage);
    اگر (factory != nullptr) {
        بازگشت registerFactory(factory, status);
    }
    delete objToAdopt;
    بازگشت nullptr;
}
#endif

رده ServiceEnumeration : عمومی StringEnumeration {
خصوصی:
    ثابت ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

خصوصی:
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
        اگر(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            برای(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            اگر(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

عمومی:
    ایستا ServiceEnumeration* create(ثابت ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = جدید ServiceEnumeration(service, status);
        اگر (U_SUCCESS(status)) {
            بازگشت result;
        }
        delete result;
        بازگشت nullptr;
    }

    مجازی ~ServiceEnumeration();

    مجازی StringEnumeration *clone() ثابت override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = جدید ServiceEnumeration(*این, status);
        اگر(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        بازگشت cl;
    }

    UBool upToDate(UErrorCode& status) ثابت {
        اگر (U_SUCCESS(status)) {
            اگر (_timestamp == _service->getTimestamp()) {
                بازگشت درست;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        بازگشت نادرست;
    }

    مجازی int32_t count(UErrorCode& status) ثابت override {
        بازگشت upToDate(status) ? _ids.size() : 0;
    }

    مجازی ثابت UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        اگر (upToDate(status) && (_pos < _ids.size())) {
            بازگشت static_cast<ثابت UnicodeString*>(_ids[_pos++]);
        }
        بازگشت nullptr;
    }

    مجازی تهی reset(UErrorCode& status) override {
        اگر (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        اگر (U_SUCCESS(status)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, status);
        }
    }

عمومی:
    ایستا UClassID U_EXPORT2 getStaticClassID();
    مجازی UClassID getDynamicClassID() ثابت override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() ثابت U_LIFETIME_BOUND
{
    بازگشت ServiceEnumeration::create(این);
}

ثابت UnicodeString&
ICULocaleService::validateFallbackLocale() ثابت
{
    ثابت Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(این);
    ایستا UMutex llock;
    {
        Mutex mutex(&llock);
        اگر (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    بازگشت fallbackLocaleName;
}

ICUService_کلید*
ICULocaleService::create_کلید(ثابت UnicodeString* id, UErrorCode& status) ثابت
{
    بازگشت Locale_کلید::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_کلید*
ICULocaleService::create_کلید(ثابت UnicodeString* id, int32_t kind, UErrorCode& status) ثابت
{
    بازگشت Locale_کلید::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


