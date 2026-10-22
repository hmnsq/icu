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
#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_SERVICE

#અન્તર્ભાવ "unicode/resbund.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "servloc.h"
#અન્તર્ભાવ "ustrfmt.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(અચલ UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(અચલ Locale& locale, UErrorCode& status) અચલ
{
    પાછા get(locale, Locale_ચાવી::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(અચલ Locale& locale, int32_t kind, UErrorCode& status) અચલ
{
    પાછા get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(અચલ Locale& locale, Locale* actualReturn, UErrorCode& status) અચલ
{
    પાછા get(locale, Locale_ચાવી::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(અચલ Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) અચલ
{
    UObject* result = nullptr;
    જો (U_FAILURE(status)) {
        પાછા result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    જો (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } નહીંતર {
        ICUService_ચાવી* ચાવી = create_ચાવી(&locName, kind, status);
        જો (ચાવી) {
            જો (actualReturn == nullptr) {
                result = get_ચાવી(*ચાવી, status);
            } નહીંતર {
                UnicodeString temp;
                result = get_ચાવી(*ચાવી, &temp, status);

                જો (result != nullptr) {
                    ચાવી->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete ચાવી;
        }
    }
    પાછા result;
}


URegistry_ચાવી
ICULocaleService::registerInstance(UObject* objToAdopt, અચલ UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    પાછા registerInstance(objToAdopt, loc, Locale_ચાવી::KIND_ANY, 
        visible ? Locale_ચાવી_Factory::VISIBLE : Locale_ચાવી_Factory::INVISIBLE, status);
}

URegistry_ચાવી
ICULocaleService::registerInstance(UObject* objToAdopt, અચલ Locale& locale, UErrorCode& status)
{
    પાછા registerInstance(objToAdopt, locale, Locale_ચાવી::KIND_ANY, Locale_ચાવી_Factory::VISIBLE, status);
}

URegistry_ચાવી
ICULocaleService::registerInstance(UObject* objToAdopt, અચલ Locale& locale, int32_t kind, UErrorCode& status)
{
    પાછા registerInstance(objToAdopt, locale, kind, Locale_ચાવી_Factory::VISIBLE, status);
}

URegistry_ચાવી
ICULocaleService::registerInstance(UObject* objToAdopt, અચલ Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = નવું Simple_Locale_ચાવી_Factory(objToAdopt, locale, kind, coverage);
    જો (factory != nullptr) {
        પાછા registerFactory(factory, status);
    }
    delete objToAdopt;
    પાછા nullptr;
}

#જો 0
URegistry_ચાવી
ICULocaleService::registerInstance(UObject* objToAdopt, અચલ UnicodeString& locale, UErrorCode& status)
{
    પાછા registerInstance(objToAdopt, locale, Locale_ચાવી::KIND_ANY, Locale_ચાવી_Factory::VISIBLE, status);
}

URegistry_ચાવી
ICULocaleService::registerInstance(UObject* objToAdopt, અચલ UnicodeString& locale, UBool visible, UErrorCode& status)
{
    પાછા registerInstance(objToAdopt, locale, Locale_ચાવી::KIND_ANY,
                            visible ? Locale_ચાવી_Factory::VISIBLE : Locale_ચાવી_Factory::INVISIBLE,
                            status);
}

URegistry_ચાવી
ICULocaleService::registerInstance(UObject* objToAdopt, અચલ UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = નવું Simple_Locale_ચાવી_Factory(objToAdopt, locale, kind, coverage);
    જો (factory != nullptr) {
        પાછા registerFactory(factory, status);
    }
    delete objToAdopt;
    પાછા nullptr;
}
#endif

વર્ગ ServiceEnumeration : જાહેર StringEnumeration {
ખાનગી:
    અચલ ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

ખાનગી:
    ServiceEnumeration(અચલ ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(અચલ ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        જો(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            માટે(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            જો(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

જાહેર:
    સ્થિર ServiceEnumeration* create(અચલ ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = નવું ServiceEnumeration(service, status);
        જો (U_SUCCESS(status)) {
            પાછા result;
        }
        delete result;
        પાછા nullptr;
    }

    આભાસી ~ServiceEnumeration();

    આભાસી StringEnumeration *clone() અચલ override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = નવું ServiceEnumeration(*આ, status);
        જો(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        પાછા cl;
    }

    UBool upToDate(UErrorCode& status) અચલ {
        જો (U_SUCCESS(status)) {
            જો (_timestamp == _service->getTimestamp()) {
                પાછા સાચું;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        પાછા ખોટું;
    }

    આભાસી int32_t count(UErrorCode& status) અચલ override {
        પાછા upToDate(status) ? _ids.size() : 0;
    }

    આભાસી અચલ UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        જો (upToDate(status) && (_pos < _ids.size())) {
            પાછા static_cast<અચલ UnicodeString*>(_ids[_pos++]);
        }
        પાછા nullptr;
    }

    આભાસી શૂન્ય reset(UErrorCode& status) override {
        જો (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        જો (U_SUCCESS(status)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, status);
        }
    }

જાહેર:
    સ્થિર UClassID U_EXPORT2 getStaticClassID();
    આભાસી UClassID getDynamicClassID() અચલ override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() અચલ U_LIFETIME_BOUND
{
    પાછા ServiceEnumeration::create(આ);
}

અચલ UnicodeString&
ICULocaleService::validateFallbackLocale() અચલ
{
    અચલ Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(આ);
    સ્થિર UMutex llock;
    {
        Mutex mutex(&llock);
        જો (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    પાછા fallbackLocaleName;
}

ICUService_ચાવી*
ICULocaleService::create_ચાવી(અચલ UnicodeString* id, UErrorCode& status) અચલ
{
    પાછા Locale_ચાવી::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_ચાવી*
ICULocaleService::create_ચાવી(અચલ UnicodeString* id, int32_t kind, UErrorCode& status) અચલ
{
    પાછા Locale_ચાવી::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


