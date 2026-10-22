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
#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_SERVICE

#అన్తర్భావ "unicode/resbund.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "servloc.h"
#అన్తర్భావ "ustrfmt.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "uassert.h"

#నిర్వచించు UNDERSCORE_CHAR ((char16_t)0x005f)
#నిర్వచించు AT_SIGN_CHAR    ((char16_t)64)
#నిర్వచించు PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(స్థిరరాశి UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(స్థిరరాశి Locale& locale, UError_సంకేతం& స్థితి_2) స్థిరరాశి
{
    తిరిగి get(locale, Locale_కీలకం::రకం_ANY, శూన్యసూచిక, స్థితి_2);
}

UObject*
ICULocaleService::get(స్థిరరాశి Locale& locale, int32_t రకం, UError_సంకేతం& స్థితి_2) స్థిరరాశి
{
    తిరిగి get(locale, రకం, శూన్యసూచిక, స్థితి_2);
}

UObject*
ICULocaleService::get(స్థిరరాశి Locale& locale, Locale* actualReturn, UError_సంకేతం& స్థితి_2) స్థిరరాశి
{
    తిరిగి get(locale, Locale_కీలకం::రకం_ANY, actualReturn, స్థితి_2);
}

UObject*
ICULocaleService::get(స్థిరరాశి Locale& locale, int32_t రకం, Locale* actualReturn, UError_సంకేతం& స్థితి_2) స్థిరరాశి
{
    UObject* ఫలితం = శూన్యసూచిక;
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి ఫలితం;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    ఒకవేళ (locName.isBogus()) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    } లేకపోతే {
        ICUService_కీలకం* కీలకం = create_కీలకం(&locName, రకం, స్థితి_2);
        ఒకవేళ (కీలకం) {
            ఒకవేళ (actualReturn == శూన్యసూచిక) {
                ఫలితం = get_కీలకం(*కీలకం, స్థితి_2);
            } లేకపోతే {
                UnicodeString తాత్కాలిక;
                ఫలితం = get_కీలకం(*కీలకం, &తాత్కాలిక, స్థితి_2);

                ఒకవేళ (ఫలితం != శూన్యసూచిక) {
                    కీలకం->parseSuffix(తాత్కాలిక);
                    LocaleUtility::initLocaleFromName(తాత్కాలిక, *actualReturn);
                }
            }
            తొలగించు కీలకం;
        }
    }
    తిరిగి ఫలితం;
}


URegistry_కీలకం
ICULocaleService::రిజిస్టరు_Instance(UObject* obj_గమ్యం_Adopt, స్థిరరాశి UnicodeString& locale, 
    UBool visible, UError_సంకేతం& స్థితి_2)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    తిరిగి రిజిస్టరు_Instance(obj_గమ్యం_Adopt, loc, Locale_కీలకం::రకం_ANY, 
        visible ? Locale_కీలకం_Factory::VISIBLE : Locale_కీలకం_Factory::INVISIBLE, స్థితి_2);
}

URegistry_కీలకం
ICULocaleService::రిజిస్టరు_Instance(UObject* obj_గమ్యం_Adopt, స్థిరరాశి Locale& locale, UError_సంకేతం& స్థితి_2)
{
    తిరిగి రిజిస్టరు_Instance(obj_గమ్యం_Adopt, locale, Locale_కీలకం::రకం_ANY, Locale_కీలకం_Factory::VISIBLE, స్థితి_2);
}

URegistry_కీలకం
ICULocaleService::రిజిస్టరు_Instance(UObject* obj_గమ్యం_Adopt, స్థిరరాశి Locale& locale, int32_t రకం, UError_సంకేతం& స్థితి_2)
{
    తిరిగి రిజిస్టరు_Instance(obj_గమ్యం_Adopt, locale, రకం, Locale_కీలకం_Factory::VISIBLE, స్థితి_2);
}

URegistry_కీలకం
ICULocaleService::రిజిస్టరు_Instance(UObject* obj_గమ్యం_Adopt, స్థిరరాశి Locale& locale, int32_t రకం, int32_t coverage, UError_సంకేతం& స్థితి_2)
{
    ICUServiceFactory * factory = కొత్త Simple_Locale_కీలకం_Factory(obj_గమ్యం_Adopt, locale, రకం, coverage);
    ఒకవేళ (factory != శూన్యసూచిక) {
        తిరిగి రిజిస్టరు_Factory(factory, స్థితి_2);
    }
    తొలగించు obj_గమ్యం_Adopt;
    తిరిగి శూన్యసూచిక;
}

#ఒకవేళ 0
URegistry_కీలకం
ICULocaleService::రిజిస్టరు_Instance(UObject* obj_గమ్యం_Adopt, స్థిరరాశి UnicodeString& locale, UError_సంకేతం& స్థితి_2)
{
    తిరిగి రిజిస్టరు_Instance(obj_గమ్యం_Adopt, locale, Locale_కీలకం::రకం_ANY, Locale_కీలకం_Factory::VISIBLE, స్థితి_2);
}

URegistry_కీలకం
ICULocaleService::రిజిస్టరు_Instance(UObject* obj_గమ్యం_Adopt, స్థిరరాశి UnicodeString& locale, UBool visible, UError_సంకేతం& స్థితి_2)
{
    తిరిగి రిజిస్టరు_Instance(obj_గమ్యం_Adopt, locale, Locale_కీలకం::రకం_ANY,
                            visible ? Locale_కీలకం_Factory::VISIBLE : Locale_కీలకం_Factory::INVISIBLE,
                            స్థితి_2);
}

URegistry_కీలకం
ICULocaleService::రిజిస్టరు_Instance(UObject* obj_గమ్యం_Adopt, స్థిరరాశి UnicodeString& locale, int32_t రకం, int32_t coverage, UError_సంకేతం& స్థితి_2)
{
    ICUServiceFactory * factory = కొత్త Simple_Locale_కీలకం_Factory(obj_గమ్యం_Adopt, locale, రకం, coverage);
    ఒకవేళ (factory != శూన్యసూచిక) {
        తిరిగి రిజిస్టరు_Factory(factory, స్థితి_2);
    }
    తొలగించు obj_గమ్యం_Adopt;
    తిరిగి శూన్యసూచిక;
}
#అయితే_ముగింపు

తరగతి ServiceEnumeration : బహిరంగ StringEnumeration {
అంతరంగ:
    స్థిరరాశి ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

అంతరంగ:
    ServiceEnumeration(స్థిరరాశి ICULocaleService* service U_LIFETIME_BOUND, UError_సంకేతం& స్థితి_2)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, శూన్యసూచిక, స్థితి_2)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, స్థితి_2);
    }

    ServiceEnumeration(స్థిరరాశి ServiceEnumeration &other, UError_సంకేతం &స్థితి_2)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, శూన్యసూచిక, స్థితి_2)
        , _pos(0)
    {
        ఒకవేళ(U_SUCCESS(స్థితి_2)) {
            int32_t i, length;

            length = other._ids.size();
            కొరకు(i = 0; i < length; ++i) {
                Local_సూచిక<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), స్థితి_2);
                _ids.adoptElement(clonedId.orphan(), స్థితి_2);
            }

            ఒకవేళ(U_SUCCESS(స్థితి_2)) {
                _pos = other._pos;
            }
        }
    }

బహిరంగ:
    స్థిర ServiceEnumeration* create(స్థిరరాశి ICULocaleService* service U_LIFETIME_BOUND) {
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        ServiceEnumeration* ఫలితం = కొత్త ServiceEnumeration(service, స్థితి_2);
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            తిరిగి ఫలితం;
        }
        తొలగించు ఫలితం;
        తిరిగి శూన్యసూచిక;
    }

    మాయా ~ServiceEnumeration();

    మాయా StringEnumeration *clone() స్థిరరాశి override {
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        ServiceEnumeration *cl = కొత్త ServiceEnumeration(*ఇది, స్థితి_2);
        ఒకవేళ(U_FAILURE(స్థితి_2)) {
            తొలగించు cl;
            cl = శూన్యసూచిక;
        }
        తిరిగి cl;
    }

    UBool పైకి_గమ్యం_Date(UError_సంకేతం& స్థితి_2) స్థిరరాశి {
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            ఒకవేళ (_timestamp == _service->getTimestamp()) {
                తిరిగి నిజం;
            }
            స్థితి_2 = U_ENUM_బహిర్గతం_యొక్క_SYNC_ERROR;
        }
        తిరిగి అబద్ధం;
    }

    మాయా int32_t count(UError_సంకేతం& స్థితి_2) స్థిరరాశి override {
        తిరిగి పైకి_గమ్యం_Date(స్థితి_2) ? _ids.size() : 0;
    }

    మాయా స్థిరరాశి UnicodeString* snext(UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND override {
        ఒకవేళ (పైకి_గమ్యం_Date(స్థితి_2) && (_pos < _ids.size())) {
            తిరిగి static_cast<స్థిరరాశి UnicodeString*>(_ids[_pos++]);
        }
        తిరిగి శూన్యసూచిక;
    }

    మాయా శూన్య reset(UError_సంకేతం& స్థితి_2) override {
        ఒకవేళ (స్థితి_2 == U_ENUM_బహిర్గతం_యొక్క_SYNC_ERROR) {
            స్థితి_2 = U_ZERO_ERROR;
        }
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, స్థితి_2);
        }
    }

బహిరంగ:
    స్థిర UClassID U_EXPORT2 getStaticClassID();
    మాయా UClassID getDynamicClassID() స్థిరరాశి override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::get_అందుబాటు_Locales() స్థిరరాశి U_LIFETIME_BOUND
{
    తిరిగి ServiceEnumeration::create(ఇది);
}

స్థిరరాశి UnicodeString&
ICULocaleService::validateFallbackLocale() స్థిరరాశి
{
    స్థిరరాశి Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(ఇది);
    స్థిర UMutex llock;
    {
        Mutex mutex(&llock);
        ఒకవేళ (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    తిరిగి fallbackLocaleName;
}

ICUService_కీలకం*
ICULocaleService::create_కీలకం(స్థిరరాశి UnicodeString* id, UError_సంకేతం& స్థితి_2) స్థిరరాశి
{
    తిరిగి Locale_కీలకం::createWithCanonicalFallback(id, &validateFallbackLocale(), స్థితి_2);
}

ICUService_కీలకం*
ICULocaleService::create_కీలకం(స్థిరరాశి UnicodeString* id, int32_t రకం, UError_సంకేతం& స్థితి_2) స్థిరరాశి
{
    తిరిగి Locale_కీలకం::createWithCanonicalFallback(id, &validateFallbackLocale(), రకం, స్థితి_2);
}

U_NAMESPACE_చివర

/* !UCONFIG_NO_SERVICE */
#అయితే_ముగింపు


