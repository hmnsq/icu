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
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_SERVICE

#ଅନ୍ତର୍ଭାଵ "unicode/resbund.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "servloc.h"
#ଅନ୍ତର୍ଭାଵ "ustrfmt.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(ଅଚଳ UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(ଅଚଳ Locale& locale, UErrorCode& status) ଅଚଳ
{
    ଫେରନ୍ତୁ get(locale, Locale_ଚାବି::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(ଅଚଳ Locale& locale, int32_t kind, UErrorCode& status) ଅଚଳ
{
    ଫେରନ୍ତୁ get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(ଅଚଳ Locale& locale, Locale* actualReturn, UErrorCode& status) ଅଚଳ
{
    ଫେରନ୍ତୁ get(locale, Locale_ଚାବି::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(ଅଚଳ Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) ଅଚଳ
{
    UObject* result = nullptr;
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    ଯଦି (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } ଅନ୍ୟଥା {
        ICUService_ଚାବି* ଚାବି = create_ଚାବି(&locName, kind, status);
        ଯଦି (ଚାବି) {
            ଯଦି (actualReturn == nullptr) {
                result = get_ଚାବି(*ଚାବି, status);
            } ଅନ୍ୟଥା {
                UnicodeString temp;
                result = get_ଚାବି(*ଚାବି, &temp, status);

                ଯଦି (result != nullptr) {
                    ଚାବି->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete ଚାବି;
        }
    }
    ଫେରନ୍ତୁ result;
}


URegistry_ଚାବି
ICULocaleService::registerInstance(UObject* objToAdopt, ଅଚଳ UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    ଫେରନ୍ତୁ registerInstance(objToAdopt, loc, Locale_ଚାବି::KIND_ANY, 
        visible ? Locale_ଚାବି_Factory::VISIBLE : Locale_ଚାବି_Factory::INVISIBLE, status);
}

URegistry_ଚାବି
ICULocaleService::registerInstance(UObject* objToAdopt, ଅଚଳ Locale& locale, UErrorCode& status)
{
    ଫେରନ୍ତୁ registerInstance(objToAdopt, locale, Locale_ଚାବି::KIND_ANY, Locale_ଚାବି_Factory::VISIBLE, status);
}

URegistry_ଚାବି
ICULocaleService::registerInstance(UObject* objToAdopt, ଅଚଳ Locale& locale, int32_t kind, UErrorCode& status)
{
    ଫେରନ୍ତୁ registerInstance(objToAdopt, locale, kind, Locale_ଚାବି_Factory::VISIBLE, status);
}

URegistry_ଚାବି
ICULocaleService::registerInstance(UObject* objToAdopt, ଅଚଳ Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = ନୂତନ Simple_Locale_ଚାବି_Factory(objToAdopt, locale, kind, coverage);
    ଯଦି (factory != nullptr) {
        ଫେରନ୍ତୁ registerFactory(factory, status);
    }
    delete objToAdopt;
    ଫେରନ୍ତୁ nullptr;
}

#ଯଦି 0
URegistry_ଚାବି
ICULocaleService::registerInstance(UObject* objToAdopt, ଅଚଳ UnicodeString& locale, UErrorCode& status)
{
    ଫେରନ୍ତୁ registerInstance(objToAdopt, locale, Locale_ଚାବି::KIND_ANY, Locale_ଚାବି_Factory::VISIBLE, status);
}

URegistry_ଚାବି
ICULocaleService::registerInstance(UObject* objToAdopt, ଅଚଳ UnicodeString& locale, UBool visible, UErrorCode& status)
{
    ଫେରନ୍ତୁ registerInstance(objToAdopt, locale, Locale_ଚାବି::KIND_ANY,
                            visible ? Locale_ଚାବି_Factory::VISIBLE : Locale_ଚାବି_Factory::INVISIBLE,
                            status);
}

URegistry_ଚାବି
ICULocaleService::registerInstance(UObject* objToAdopt, ଅଚଳ UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = ନୂତନ Simple_Locale_ଚାବି_Factory(objToAdopt, locale, kind, coverage);
    ଯଦି (factory != nullptr) {
        ଫେରନ୍ତୁ registerFactory(factory, status);
    }
    delete objToAdopt;
    ଫେରନ୍ତୁ nullptr;
}
#endif

ଶ୍ରେଣୀ ServiceEnumeration : ସର୍ବସାଧାରଣ StringEnumeration {
ବ୍ୟକ୍ତିଗତ:
    ଅଚଳ ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

ବ୍ୟକ୍ତିଗତ:
    ServiceEnumeration(ଅଚଳ ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(ଅଚଳ ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        ଯଦି(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            ପାଇଁ(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            ଯଦି(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

ସର୍ବସାଧାରଣ:
    ସ୍ଥିର ServiceEnumeration* create(ଅଚଳ ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = ନୂତନ ServiceEnumeration(service, status);
        ଯଦି (U_SUCCESS(status)) {
            ଫେରନ୍ତୁ result;
        }
        delete result;
        ଫେରନ୍ତୁ nullptr;
    }

    ଆଭାସୀ ~ServiceEnumeration();

    ଆଭାସୀ StringEnumeration *clone() ଅଚଳ override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = ନୂତନ ServiceEnumeration(*ଏହି, status);
        ଯଦି(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        ଫେରନ୍ତୁ cl;
    }

    UBool upToDate(UErrorCode& status) ଅଚଳ {
        ଯଦି (U_SUCCESS(status)) {
            ଯଦି (_timestamp == _service->getTimestamp()) {
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    ଆଭାସୀ int32_t count(UErrorCode& status) ଅଚଳ override {
        ଫେରନ୍ତୁ upToDate(status) ? _ids.size() : 0;
    }

    ଆଭାସୀ ଅଚଳ UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        ଯଦି (upToDate(status) && (_pos < _ids.size())) {
            ଫେରନ୍ତୁ static_cast<ଅଚଳ UnicodeString*>(_ids[_pos++]);
        }
        ଫେରନ୍ତୁ nullptr;
    }

    ଆଭାସୀ ଶୂନ୍ୟ reset(UErrorCode& status) override {
        ଯଦି (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        ଯଦି (U_SUCCESS(status)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, status);
        }
    }

ସର୍ବସାଧାରଣ:
    ସ୍ଥିର UClassID U_EXPORT2 getStaticClassID();
    ଆଭାସୀ UClassID getDynamicClassID() ଅଚଳ override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() ଅଚଳ U_LIFETIME_BOUND
{
    ଫେରନ୍ତୁ ServiceEnumeration::create(ଏହି);
}

ଅଚଳ UnicodeString&
ICULocaleService::validateFallbackLocale() ଅଚଳ
{
    ଅଚଳ Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(ଏହି);
    ସ୍ଥିର UMutex llock;
    {
        Mutex mutex(&llock);
        ଯଦି (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    ଫେରନ୍ତୁ fallbackLocaleName;
}

ICUService_ଚାବି*
ICULocaleService::create_ଚାବି(ଅଚଳ UnicodeString* id, UErrorCode& status) ଅଚଳ
{
    ଫେରନ୍ତୁ Locale_ଚାବି::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_ଚାବି*
ICULocaleService::create_ଚାବି(ଅଚଳ UnicodeString* id, int32_t kind, UErrorCode& status) ଅଚଳ
{
    ଫେରନ୍ତୁ Locale_ଚାବି::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


