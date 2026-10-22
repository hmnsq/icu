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
#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_SERVICE

#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "servloc.h"
#अन्तर्भाव "ustrfmt.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "uassert.h"

#परिभाषय UNDERSCORE_CHAR ((char16_t)0x005f)
#परिभाषय AT_SIGN_CHAR    ((char16_t)64)
#परिभाषय PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

ICULocaleService::ICULocaleService()
  : fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::ICULocaleService(ध्रुव UnicodeString& dname)
  : ICUService(dname)
  , fallbackLocale(Locale::getDefault())
{
}

ICULocaleService::~ICULocaleService()
{
}

UObject*
ICULocaleService::get(ध्रुव Locale& locale, UError_संहिता& स्थितिः) ध्रुव
{
    प्रत्यागम get(locale, Locale_कुञ्चिका::विधा_ANY, शून्यसूचकः, स्थितिः);
}

UObject*
ICULocaleService::get(ध्रुव Locale& locale, int32_t विधा, UError_संहिता& स्थितिः) ध्रुव
{
    प्रत्यागम get(locale, विधा, शून्यसूचकः, स्थितिः);
}

UObject*
ICULocaleService::get(ध्रुव Locale& locale, Locale* actualReturn, UError_संहिता& स्थितिः) ध्रुव
{
    प्रत्यागम get(locale, Locale_कुञ्चिका::विधा_ANY, actualReturn, स्थितिः);
}

UObject*
ICULocaleService::get(ध्रुव Locale& locale, int32_t विधा, Locale* actualReturn, UError_संहिता& स्थितिः) ध्रुव
{
    UObject* परिणाम = शून्यसूचकः;
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम परिणाम;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    यदि (locName.isBogus()) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    } अन्यथा {
        ICUService_कुञ्चिका* कुञ्चिका = create_कुञ्चिका(&locName, विधा, स्थितिः);
        यदि (कुञ्चिका) {
            यदि (actualReturn == शून्यसूचकः) {
                परिणाम = get_कुञ्चिका(*कुञ्चिका, स्थितिः);
            } अन्यथा {
                UnicodeString अस्थायी;
                परिणाम = get_कुञ्चिका(*कुञ्चिका, &अस्थायी, स्थितिः);

                यदि (परिणाम != शून्यसूचकः) {
                    कुञ्चिका->parseSuffix(अस्थायी);
                    LocaleUtility::initLocaleFromName(अस्थायी, *actualReturn);
                }
            }
            विलोपय कुञ्चिका;
        }
    }
    प्रत्यागम परिणाम;
}


URegistry_कुञ्चिका
ICULocaleService::पञ्जिकाकोशः_Instance(UObject* obj_गन्तव्यम्_Adopt, ध्रुव UnicodeString& locale, 
    UBool visible, UError_संहिता& स्थितिः)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    प्रत्यागम पञ्जिकाकोशः_Instance(obj_गन्तव्यम्_Adopt, loc, Locale_कुञ्चिका::विधा_ANY, 
        visible ? Locale_कुञ्चिका_Factory::VISIBLE : Locale_कुञ्चिका_Factory::INVISIBLE, स्थितिः);
}

URegistry_कुञ्चिका
ICULocaleService::पञ्जिकाकोशः_Instance(UObject* obj_गन्तव्यम्_Adopt, ध्रुव Locale& locale, UError_संहिता& स्थितिः)
{
    प्रत्यागम पञ्जिकाकोशः_Instance(obj_गन्तव्यम्_Adopt, locale, Locale_कुञ्चिका::विधा_ANY, Locale_कुञ्चिका_Factory::VISIBLE, स्थितिः);
}

URegistry_कुञ्चिका
ICULocaleService::पञ्जिकाकोशः_Instance(UObject* obj_गन्तव्यम्_Adopt, ध्रुव Locale& locale, int32_t विधा, UError_संहिता& स्थितिः)
{
    प्रत्यागम पञ्जिकाकोशः_Instance(obj_गन्तव्यम्_Adopt, locale, विधा, Locale_कुञ्चिका_Factory::VISIBLE, स्थितिः);
}

URegistry_कुञ्चिका
ICULocaleService::पञ्जिकाकोशः_Instance(UObject* obj_गन्तव्यम्_Adopt, ध्रुव Locale& locale, int32_t विधा, int32_t coverage, UError_संहिता& स्थितिः)
{
    ICUServiceFactory * factory = नूतन Simple_Locale_कुञ्चिका_Factory(obj_गन्तव्यम्_Adopt, locale, विधा, coverage);
    यदि (factory != शून्यसूचकः) {
        प्रत्यागम पञ्जिकाकोशः_Factory(factory, स्थितिः);
    }
    विलोपय obj_गन्तव्यम्_Adopt;
    प्रत्यागम शून्यसूचकः;
}

#यदि 0
URegistry_कुञ्चिका
ICULocaleService::पञ्जिकाकोशः_Instance(UObject* obj_गन्तव्यम्_Adopt, ध्रुव UnicodeString& locale, UError_संहिता& स्थितिः)
{
    प्रत्यागम पञ्जिकाकोशः_Instance(obj_गन्तव्यम्_Adopt, locale, Locale_कुञ्चिका::विधा_ANY, Locale_कुञ्चिका_Factory::VISIBLE, स्थितिः);
}

URegistry_कुञ्चिका
ICULocaleService::पञ्जिकाकोशः_Instance(UObject* obj_गन्तव्यम्_Adopt, ध्रुव UnicodeString& locale, UBool visible, UError_संहिता& स्थितिः)
{
    प्रत्यागम पञ्जिकाकोशः_Instance(obj_गन्तव्यम्_Adopt, locale, Locale_कुञ्चिका::विधा_ANY,
                            visible ? Locale_कुञ्चिका_Factory::VISIBLE : Locale_कुञ्चिका_Factory::INVISIBLE,
                            स्थितिः);
}

URegistry_कुञ्चिका
ICULocaleService::पञ्जिकाकोशः_Instance(UObject* obj_गन्तव्यम्_Adopt, ध्रुव UnicodeString& locale, int32_t विधा, int32_t coverage, UError_संहिता& स्थितिः)
{
    ICUServiceFactory * factory = नूतन Simple_Locale_कुञ्चिका_Factory(obj_गन्तव्यम्_Adopt, locale, विधा, coverage);
    यदि (factory != शून्यसूचकः) {
        प्रत्यागम पञ्जिकाकोशः_Factory(factory, स्थितिः);
    }
    विलोपय obj_गन्तव्यम्_Adopt;
    प्रत्यागम शून्यसूचकः;
}
#यदि_अन्तः

वर्ग ServiceEnumeration : सार्वजनिक StringEnumeration {
गुप्त:
    ध्रुव ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

गुप्त:
    ServiceEnumeration(ध्रुव ICULocaleService* service U_LIFETIME_BOUND, UError_संहिता& स्थितिः)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, शून्यसूचकः, स्थितिः)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, स्थितिः);
    }

    ServiceEnumeration(ध्रुव ServiceEnumeration &other, UError_संहिता &स्थितिः)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, शून्यसूचकः, स्थितिः)
        , _pos(0)
    {
        यदि(U_SUCCESS(स्थितिः)) {
            int32_t i, length;

            length = other._ids.size();
            प्रति(i = 0; i < length; ++i) {
                LocalPointer<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), स्थितिः);
                _ids.adoptElement(clonedId.orphan(), स्थितिः);
            }

            यदि(U_SUCCESS(स्थितिः)) {
                _pos = other._pos;
            }
        }
    }

सार्वजनिक:
    स्थिर ServiceEnumeration* create(ध्रुव ICULocaleService* service U_LIFETIME_BOUND) {
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        ServiceEnumeration* परिणाम = नूतन ServiceEnumeration(service, स्थितिः);
        यदि (U_SUCCESS(स्थितिः)) {
            प्रत्यागम परिणाम;
        }
        विलोपय परिणाम;
        प्रत्यागम शून्यसूचकः;
    }

    आभास ~ServiceEnumeration();

    आभास StringEnumeration *clone() ध्रुव override {
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        ServiceEnumeration *cl = नूतन ServiceEnumeration(*एतत्, स्थितिः);
        यदि(U_FAILURE(स्थितिः)) {
            विलोपय cl;
            cl = शून्यसूचकः;
        }
        प्रत्यागम cl;
    }

    UBool ऊर्ध्वम्_गन्तव्यम्_Date(UError_संहिता& स्थितिः) ध्रुव {
        यदि (U_SUCCESS(स्थितिः)) {
            यदि (_timestamp == _service->getTimestamp()) {
                प्रत्यागम सत्य;
            }
            स्थितिः = U_ENUM_बहिर्गमनम्_सम्बन्धि_SYNC_ERROR;
        }
        प्रत्यागम असत्य;
    }

    आभास int32_t count(UError_संहिता& स्थितिः) ध्रुव override {
        प्रत्यागम ऊर्ध्वम्_गन्तव्यम्_Date(स्थितिः) ? _ids.size() : 0;
    }

    आभास ध्रुव UnicodeString* snext(UError_संहिता& स्थितिः) U_LIFETIME_BOUND override {
        यदि (ऊर्ध्वम्_गन्तव्यम्_Date(स्थितिः) && (_pos < _ids.size())) {
            प्रत्यागम static_cast<ध्रुव UnicodeString*>(_ids[_pos++]);
        }
        प्रत्यागम शून्यसूचकः;
    }

    आभास शून्य reset(UError_संहिता& स्थितिः) override {
        यदि (स्थितिः == U_ENUM_बहिर्गमनम्_सम्बन्धि_SYNC_ERROR) {
            स्थितिः = U_ZERO_ERROR;
        }
        यदि (U_SUCCESS(स्थितिः)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, स्थितिः);
        }
    }

सार्वजनिक:
    स्थिर UClassID U_EXPORT2 getStaticClassID();
    आभास UClassID getDynamicClassID() ध्रुव override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::get_उपलब्धम्_Locales() ध्रुव U_LIFETIME_BOUND
{
    प्रत्यागम ServiceEnumeration::create(एतत्);
}

ध्रुव UnicodeString&
ICULocaleService::validateFallbackLocale() ध्रुव
{
    ध्रुव Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(एतत्);
    स्थिर UMutex llock;
    {
        Mutex mutex(&llock);
        यदि (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    प्रत्यागम fallbackLocaleName;
}

ICUService_कुञ्चिका*
ICULocaleService::create_कुञ्चिका(ध्रुव UnicodeString* id, UError_संहिता& स्थितिः) ध्रुव
{
    प्रत्यागम Locale_कुञ्चिका::createWithCanonicalFallback(id, &validateFallbackLocale(), स्थितिः);
}

ICUService_कुञ्चिका*
ICULocaleService::create_कुञ्चिका(ध्रुव UnicodeString* id, int32_t विधा, UError_संहिता& स्थितिः) ध्रुव
{
    प्रत्यागम Locale_कुञ्चिका::createWithCanonicalFallback(id, &validateFallbackLocale(), विधा, स्थितिः);
}

U_NAMESPACE_अन्तः

/* !UCONFIG_NO_SERVICE */
#यदि_अन्तः


