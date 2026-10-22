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
#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_SERVICE

#അന്തര്ഭാവ "unicode/resbund.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "servloc.h"
#അന്തര്ഭാവ "ustrfmt.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(സ്ഥിരം UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(സ്ഥിരം Locale& locale, UErrorCode& status) സ്ഥിരം
{
    തിരികെ get(locale, Locale_താക്കോൽ::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::get(സ്ഥിരം Locale& locale, int32_t kind, UErrorCode& status) സ്ഥിരം
{
    തിരികെ get(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::get(സ്ഥിരം Locale& locale, Locale* actualReturn, UErrorCode& status) സ്ഥിരം
{
    തിരികെ get(locale, Locale_താക്കോൽ::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::get(സ്ഥിരം Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) സ്ഥിരം
{
    UObject* result = nullptr;
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ result;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    എങ്കിൽ (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } അല്ലെങ്കിൽ {
        ICUService_താക്കോൽ* താക്കോൽ = create_താക്കോൽ(&locName, kind, status);
        എങ്കിൽ (താക്കോൽ) {
            എങ്കിൽ (actualReturn == nullptr) {
                result = get_താക്കോൽ(*താക്കോൽ, status);
            } അല്ലെങ്കിൽ {
                UnicodeString temp;
                result = get_താക്കോൽ(*താക്കോൽ, &temp, status);

                എങ്കിൽ (result != nullptr) {
                    താക്കോൽ->parseSuffix(temp);
                    LocaleUtility::initLocaleFromName(temp, *actualReturn);
                }
            }
            delete താക്കോൽ;
        }
    }
    തിരികെ result;
}


URegistry_താക്കോൽ
ICULocaleService::registerInstance(UObject* objToAdopt, സ്ഥിരം UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    തിരികെ registerInstance(objToAdopt, loc, Locale_താക്കോൽ::KIND_ANY, 
        visible ? Locale_താക്കോൽ_Factory::VISIBLE : Locale_താക്കോൽ_Factory::INVISIBLE, status);
}

URegistry_താക്കോൽ
ICULocaleService::registerInstance(UObject* objToAdopt, സ്ഥിരം Locale& locale, UErrorCode& status)
{
    തിരികെ registerInstance(objToAdopt, locale, Locale_താക്കോൽ::KIND_ANY, Locale_താക്കോൽ_Factory::VISIBLE, status);
}

URegistry_താക്കോൽ
ICULocaleService::registerInstance(UObject* objToAdopt, സ്ഥിരം Locale& locale, int32_t kind, UErrorCode& status)
{
    തിരികെ registerInstance(objToAdopt, locale, kind, Locale_താക്കോൽ_Factory::VISIBLE, status);
}

URegistry_താക്കോൽ
ICULocaleService::registerInstance(UObject* objToAdopt, സ്ഥിരം Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = പുതിയ Simple_Locale_താക്കോൽ_Factory(objToAdopt, locale, kind, coverage);
    എങ്കിൽ (factory != nullptr) {
        തിരികെ registerFactory(factory, status);
    }
    delete objToAdopt;
    തിരികെ nullptr;
}

#എങ്കിൽ 0
URegistry_താക്കോൽ
ICULocaleService::registerInstance(UObject* objToAdopt, സ്ഥിരം UnicodeString& locale, UErrorCode& status)
{
    തിരികെ registerInstance(objToAdopt, locale, Locale_താക്കോൽ::KIND_ANY, Locale_താക്കോൽ_Factory::VISIBLE, status);
}

URegistry_താക്കോൽ
ICULocaleService::registerInstance(UObject* objToAdopt, സ്ഥിരം UnicodeString& locale, UBool visible, UErrorCode& status)
{
    തിരികെ registerInstance(objToAdopt, locale, Locale_താക്കോൽ::KIND_ANY,
                            visible ? Locale_താക്കോൽ_Factory::VISIBLE : Locale_താക്കോൽ_Factory::INVISIBLE,
                            status);
}

URegistry_താക്കോൽ
ICULocaleService::registerInstance(UObject* objToAdopt, സ്ഥിരം UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = പുതിയ Simple_Locale_താക്കോൽ_Factory(objToAdopt, locale, kind, coverage);
    എങ്കിൽ (factory != nullptr) {
        തിരികെ registerFactory(factory, status);
    }
    delete objToAdopt;
    തിരികെ nullptr;
}
#endif

വർഗ്ഗം ServiceEnumeration : പൊതു StringEnumeration {
സ്വകാര്യ:
    സ്ഥിരം ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

സ്വകാര്യ:
    ServiceEnumeration(സ്ഥിരം ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(സ്ഥിരം ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        എങ്കിൽ(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            വേണ്ടി(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            എങ്കിൽ(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

പൊതു:
    സ്ഥിതം ServiceEnumeration* create(സ്ഥിരം ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* result = പുതിയ ServiceEnumeration(service, status);
        എങ്കിൽ (U_SUCCESS(status)) {
            തിരികെ result;
        }
        delete result;
        തിരികെ nullptr;
    }

    മായാ ~ServiceEnumeration();

    മായാ StringEnumeration *clone() സ്ഥിരം override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = പുതിയ ServiceEnumeration(*ഇത്, status);
        എങ്കിൽ(U_FAILURE(status)) {
            delete cl;
            cl = nullptr;
        }
        തിരികെ cl;
    }

    UBool upToDate(UErrorCode& status) സ്ഥിരം {
        എങ്കിൽ (U_SUCCESS(status)) {
            എങ്കിൽ (_timestamp == _service->getTimestamp()) {
                തിരികെ സത്യം;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        തിരികെ അസത്യം;
    }

    മായാ int32_t count(UErrorCode& status) സ്ഥിരം override {
        തിരികെ upToDate(status) ? _ids.size() : 0;
    }

    മായാ സ്ഥിരം UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        എങ്കിൽ (upToDate(status) && (_pos < _ids.size())) {
            തിരികെ static_cast<സ്ഥിരം UnicodeString*>(_ids[_pos++]);
        }
        തിരികെ nullptr;
    }

    മായാ ശൂന്യം reset(UErrorCode& status) override {
        എങ്കിൽ (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        എങ്കിൽ (U_SUCCESS(status)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, status);
        }
    }

പൊതു:
    സ്ഥിതം UClassID U_EXPORT2 getStaticClassID();
    മായാ UClassID getDynamicClassID() സ്ഥിരം override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() സ്ഥിരം U_LIFETIME_BOUND
{
    തിരികെ ServiceEnumeration::create(ഇത്);
}

സ്ഥിരം UnicodeString&
ICULocaleService::validateFallbackLocale() സ്ഥിരം
{
    സ്ഥിരം Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(ഇത്);
    സ്ഥിതം UMutex llock;
    {
        Mutex mutex(&llock);
        എങ്കിൽ (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    തിരികെ fallbackLocaleName;
}

ICUService_താക്കോൽ*
ICULocaleService::create_താക്കോൽ(സ്ഥിരം UnicodeString* id, UErrorCode& status) സ്ഥിരം
{
    തിരികെ Locale_താക്കോൽ::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_താക്കോൽ*
ICULocaleService::create_താക്കോൽ(സ്ഥിരം UnicodeString* id, int32_t kind, UErrorCode& status) സ്ഥിരം
{
    തിരികെ Locale_താക്കോൽ::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


