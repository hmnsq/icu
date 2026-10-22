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
#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_SERVICE

#समावेश "unicode/resbund.h"
#समावेश "uresimp.h"
#समावेश "cmemory.h"
#समावेश "servloc.h"
#समावेश "ustrfmt.h"
#समावेश "charstr.h"
#समावेश "uassert.h"

#घोषणा UNDERSCORE_CHAR ((char16_t)0x005f)
#घोषणा AT_SIGN_CHAR    ((char16_t)64)
#घोषणा PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(स्थिर UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::लो(स्थिर Locale& locale, UErrorCode& status) स्थिर
{
    वापस लो(locale, Locale_कुंजी::KIND_ANY, nullptr, status);
}

UObject*
ICULocaleService::लो(स्थिर Locale& locale, int32_t kind, UErrorCode& status) स्थिर
{
    वापस लो(locale, kind, nullptr, status);
}

UObject*
ICULocaleService::लो(स्थिर Locale& locale, Locale* actualReturn, UErrorCode& status) स्थिर
{
    वापस लो(locale, Locale_कुंजी::KIND_ANY, actualReturn, status);
}

UObject*
ICULocaleService::लो(स्थिर Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) स्थिर
{
    UObject* परिणाम = nullptr;
    अगर (U_FAILURE(status)) {
        वापस परिणाम;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    अगर (locName.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा {
        ICUService_कुंजी* कुंजी = create_कुंजी(&locName, kind, status);
        अगर (कुंजी) {
            अगर (actualReturn == nullptr) {
                परिणाम = get_कुंजी(*कुंजी, status);
            } अन्यथा {
                UnicodeString अस्थायी;
                परिणाम = get_कुंजी(*कुंजी, &अस्थायी, status);

                अगर (परिणाम != nullptr) {
                    कुंजी->parseSuffix(अस्थायी);
                    LocaleUtility::initLocaleFromName(अस्थायी, *actualReturn);
                }
            }
            मिटाओ कुंजी;
        }
    }
    वापस परिणाम;
}


URegistry_कुंजी
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिर UnicodeString& locale, 
    UBool visible, UErrorCode& status)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    वापस registerInstance(objToAdopt, loc, Locale_कुंजी::KIND_ANY, 
        visible ? Locale_कुंजी_Factory::VISIBLE : Locale_कुंजी_Factory::INVISIBLE, status);
}

URegistry_कुंजी
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिर Locale& locale, UErrorCode& status)
{
    वापस registerInstance(objToAdopt, locale, Locale_कुंजी::KIND_ANY, Locale_कुंजी_Factory::VISIBLE, status);
}

URegistry_कुंजी
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिर Locale& locale, int32_t kind, UErrorCode& status)
{
    वापस registerInstance(objToAdopt, locale, kind, Locale_कुंजी_Factory::VISIBLE, status);
}

URegistry_कुंजी
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिर Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = नया Simple_Locale_कुंजी_Factory(objToAdopt, locale, kind, coverage);
    अगर (factory != nullptr) {
        वापस registerFactory(factory, status);
    }
    मिटाओ objToAdopt;
    वापस nullptr;
}

#अगर 0
URegistry_कुंजी
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिर UnicodeString& locale, UErrorCode& status)
{
    वापस registerInstance(objToAdopt, locale, Locale_कुंजी::KIND_ANY, Locale_कुंजी_Factory::VISIBLE, status);
}

URegistry_कुंजी
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिर UnicodeString& locale, UBool visible, UErrorCode& status)
{
    वापस registerInstance(objToAdopt, locale, Locale_कुंजी::KIND_ANY,
                            visible ? Locale_कुंजी_Factory::VISIBLE : Locale_कुंजी_Factory::INVISIBLE,
                            status);
}

URegistry_कुंजी
ICULocaleService::registerInstance(UObject* objToAdopt, स्थिर UnicodeString& locale, int32_t kind, int32_t coverage, UErrorCode& status)
{
    ICUServiceFactory * factory = नया Simple_Locale_कुंजी_Factory(objToAdopt, locale, kind, coverage);
    अगर (factory != nullptr) {
        वापस registerFactory(factory, status);
    }
    मिटाओ objToAdopt;
    वापस nullptr;
}
#पूर्ण_अगर

श्रेणी ServiceEnumeration : खुला StringEnumeration {
निजी:
    स्थिर ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

निजी:
    ServiceEnumeration(स्थिर ICULocaleService* service U_LIFETIME_BOUND, UErrorCode& status)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, status);
    }

    ServiceEnumeration(स्थिर ServiceEnumeration &other, UErrorCode &status)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, nullptr, status)
        , _pos(0)
    {
        अगर(U_SUCCESS(status)) {
            int32_t i, length;

            length = other._ids.size();
            क्रम(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), status);
                _ids.adoptElement(clonedId.orphan(), status);
            }

            अगर(U_SUCCESS(status)) {
                _pos = other._pos;
            }
        }
    }

खुला:
    अटल ServiceEnumeration* create(स्थिर ICULocaleService* service U_LIFETIME_BOUND) {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration* परिणाम = नया ServiceEnumeration(service, status);
        अगर (U_SUCCESS(status)) {
            वापस परिणाम;
        }
        मिटाओ परिणाम;
        वापस nullptr;
    }

    भव ~ServiceEnumeration();

    भव StringEnumeration *clone() स्थिर override {
        UErrorCode status = U_ZERO_ERROR;
        ServiceEnumeration *cl = नया ServiceEnumeration(*यह, status);
        अगर(U_FAILURE(status)) {
            मिटाओ cl;
            cl = nullptr;
        }
        वापस cl;
    }

    UBool upToDate(UErrorCode& status) स्थिर {
        अगर (U_SUCCESS(status)) {
            अगर (_timestamp == _service->getTimestamp()) {
                वापस true;
            }
            status = U_ENUM_OUT_OF_SYNC_ERROR;
        }
        वापस false;
    }

    भव int32_t count(UErrorCode& status) स्थिर override {
        वापस upToDate(status) ? _ids.size() : 0;
    }

    भव स्थिर UnicodeString* snext(UErrorCode& status) U_LIFETIME_BOUND override {
        अगर (upToDate(status) && (_pos < _ids.size())) {
            वापस static_cast<स्थिर UnicodeString*>(_ids[_pos++]);
        }
        वापस nullptr;
    }

    भव व्योम reset(UErrorCode& status) override {
        अगर (status == U_ENUM_OUT_OF_SYNC_ERROR) {
            status = U_ZERO_ERROR;
        }
        अगर (U_SUCCESS(status)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, status);
        }
    }

खुला:
    अटल UClassID U_EXPORT2 getStaticClassID();
    भव UClassID getDynamicClassID() स्थिर override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::getAvailableLocales() स्थिर U_LIFETIME_BOUND
{
    वापस ServiceEnumeration::create(यह);
}

स्थिर UnicodeString&
ICULocaleService::validateFallbackLocale() स्थिर
{
    स्थिर Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(यह);
    अटल UMutex llock;
    {
        Mutex mutex(&llock);
        अगर (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    वापस fallbackLocaleName;
}

ICUService_कुंजी*
ICULocaleService::create_कुंजी(स्थिर UnicodeString* id, UErrorCode& status) स्थिर
{
    वापस Locale_कुंजी::createWithCanonicalFallback(id, &validateFallbackLocale(), status);
}

ICUService_कुंजी*
ICULocaleService::create_कुंजी(स्थिर UnicodeString* id, int32_t kind, UErrorCode& status) स्थिर
{
    वापस Locale_कुंजी::createWithCanonicalFallback(id, &validateFallbackLocale(), kind, status);
}

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#पूर्ण_अगर


