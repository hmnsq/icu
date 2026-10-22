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
#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_SERVICE

#அந்தர்பாவ "unicode/resbund.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "servloc.h"
#அந்தர்பாவ "ustrfmt.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(மாறிலி UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(மாறிலி Locale& locale, UErrorCode& status) மாறிலி
{
    திரும்பு get(locale, Locale_சாவி::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(மாறிலி Locale& locale, int32_t kind, UErrorCode& status) மாறிலி
{
    திரும்பு get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(மாறிலி Locale& locale, Locale* actualReturn, UErrorCode& status) மாறிலி
{
    திரும்பு get(locale, Locale_சாவி::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(மாறிலி Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) மாறிலி
{
    UObject* result = nullptr;
    எனில் (U_FAILURE(status)) {
        திரும்பு result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    எனில் (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } இல்லையெனில் {
        ICUService_சாவி* சாவி = create_சாவி(&locName, kind, status);
        எனில் (சாவி) {
            எனில் (actualReturn == nullptr) {
                result = get_சாவி(*சாவி, status);
            } இல்லையெனில் {
                UnicodeString temp;
                result = get_சாவி(*சாவி, &temp, status);

                எனில் (result != nullptr) {
                    சாவி->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete சாவி;
        }
    }
    திரும்பு result;
}


URegistry_சாவி
ICULocaleService::registerInstance(UObject* objToAdopt, மாறிலி UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    திரும்பு registerInstance(objToAdopt, loc, Locale_சாவி::KIND_ANY, 
        visible ? Locale_சாவி_Factory::VISIBLE : Locale_சாவி_Factory::INVISIBLE, status);
}

URegistry_சாவி
ICULocaleService::registerInstance(UObject* objToAdopt, மாறிலி Locale& locale, UErrorCode& status)
{
    திரும்பு registerInstance(objToAdopt, locale, Locale_சாவி::KIND_ANY, Locale_சாவி_Factory::VISIBLE, status);
}

URegistry_சாவி
ICULocaleService::registerInstance(UObject* objToAdopt, மாறிலி Locale& locale, int32_t kind, UErrorCode& status)
{
    திரும்பு registerInstance(objToAdopt, locale, kind, Locale_சாவி_Factory::VISIBLE, status);
}

URegistry_சாவி
ICULocaleService::registerInstance(UObject* objToAdopt, மாறிலி Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = புதிய Simple_Locale_சாவி_Factory(objToAdopt, locale, kind, coverage);
    எனில் (factory != nullptr) {
        திரும்பு registerFactory(factory, status);
    }
    delete objToAdopt;
    திரும்பு nullptr;
}

#எனில் 0
URegistry_சாவி
ICULocaleService::registerInstance(UObject* objToAdopt, மாறிலி UnicodeString& locale, UErrorCode& status)
{
    திரும்பு registerInstance(objToAdopt, locale, Locale_சாவி::KIND_ANY, Locale_சாவி_Factory::VISIBLE, status);
}

URegistry_சாவி
ICULocaleService::registerInstance(UObject* objToAdopt, மாறிலி UnicodeString& locale, UBool visible, UErrorCode& status)
{
    திரும்பு registerInstance(objToAdopt, locale, Locale_சாவி::KIND_ANY,
                            visible ? Locale_சாவி_Factory::VISIBLE : Locale_சாவி_Factory::INVISIBLE,
                            status);
}

URegistry_சாவி
ICULocaleService::registerInstance(UObject* objToAdopt, மாறிலி UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = புதிய Simple_Locale_சாவி_Factory(objToAdopt, locale, kind, coverage);
    எனில் (factory != nullptr) {
        திரும்பு registerFactory(factory, status);
    }
    delete objToAdopt;
    திரும்பு nullptr;
}
#endif

வகுப்பு ServiceEnumeration : பொது StringEnumeration {
தனிப்பட்ட:
    மாறிலி ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

தனிப்பட்ட:
    ServiceEnumeration(மாறிலி ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(மாறிலி ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        எனில்(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            க்கு(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            எனில்(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

பொது:
    நிலையான ServiceEnumeration* create(மாறிலி ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = புதிய ServiceEnumeration(service, status);
        எனில் (U_SUCCESS(status)) {
            திரும்பு result;
        }
        delete result;
        திரும்பு nullptr;
    }

    மெய்நிகர் ~ServiceEnumeration();

    மெய்நிகர் StringEnumeration *clone() மாறிலி override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = புதிய ServiceEnumeration(*இது, status);
        எனில்(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        திரும்பு cl;
    }

    UBool upToDate(UErrorCode& status) மாறிலி {
        எனில் (U_SUCCESS(status)) {
            எனில் (_timestamp == _service->getTimestamp()) {
                திரும்பு உண்மை;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        திரும்பு பொய்;
    }

    மெய்நிகர் int32_t count(UErrorCode& status) மாறிலி override {
        திரும்பு upToDate(status) ? _ids.size() : 0;
    }

    மெய்நிகர் மாறிலி UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        எனில் (upToDate(status) && (_pos < _ids.size())) {
            திரும்பு static_cast<மாறிலி UnicodeString*>(_ids[_pos++]);
        }
        திரும்பு nullptr;
    }

    மெய்நிகர் வெறுமை reset(UErrorCode& status) override {
        எனில் (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        எனில் (U_SUCCESS(status)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, status);
        }
    }

பொது:
    நிலையான UClassID U_EXPORT2 getStaticClassID();
    மெய்நிகர் UClassID getDynamicClassID() மாறிலி override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() மாறிலி U_LIFETIME_BOUND
{
    திரும்பு ServiceEnumeration::create(இது);
}

மாறிலி UnicodeString&
ICULocaleService::validateFallbackLocale() மாறிலி
{
    மாறிலி Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(இது);
    நிலையான UMutex llock;
    {
        Mutex mutex(&llock);
        எனில் (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    திரும்பு fallbackLocaleName;
}

ICUService_சாவி*
ICULocaleService::create_சாவி(மாறிலி UnicodeString* id, UErrorCode& status) மாறிலி
{
    திரும்பு Locale_சாவி::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_சாவி*
ICULocaleService::create_சாவி(மாறிலி UnicodeString* id, int32_t kind, UErrorCode& status) மாறிலி
{
    திரும்பு Locale_சாவி::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


