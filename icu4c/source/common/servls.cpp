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
#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_SERVICE

#ضمن "unicode/resbund.h"
#ضمن "uresimp.h"
#ضمن "cmemory.h"
#ضمن "servloc.h"
#ضمن "ustrfmt.h"
#ضمن "charstr.h"
#ضمن "uassert.h"

#وصف_ڪريو UNDERSCORE_CHAR ((char16_t)0x005f)
#وصف_ڪريو AT_SIGN_CHAR    ((char16_t)64)
#وصف_ڪريو PERIOD_CHAR     ((char16_t)46)

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
ICULocaleService::get(ثابت Locale& locale, UError_ڪوڊ& حالت) ثابت
{
    أرجع get(locale, Locale_ڪنجي::قسم_ANY, خالي_اشارو, حالت);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t قسم, UError_ڪوڊ& حالت) ثابت
{
    أرجع get(locale, قسم, خالي_اشارو, حالت);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, Locale* actualReturn, UError_ڪوڊ& حالت) ثابت
{
    أرجع get(locale, Locale_ڪنجي::قسم_ANY, actualReturn, حالت);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t قسم, Locale* actualReturn, UError_ڪوڊ& حالت) ثابت
{
    UObject* نتيجو = خالي_اشارو;
    إذا (U_FAILURE(حالت)) {
        أرجع نتيجو;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    إذا (locName.isBogus()) {
        حالت = U_يادگيري_ورهاست_ERROR;
    } وإلا {
        ICUService_ڪنجي* ڪنجي = create_ڪنجي(&locName, قسم, حالت);
        إذا (ڪنجي) {
            إذا (actualReturn == خالي_اشارو) {
                نتيجو = get_ڪنجي(*ڪنجي, حالت);
            } وإلا {
                UnicodeString عارضي;
                نتيجو = get_ڪنجي(*ڪنجي, &عارضي, حالت);

                إذا (نتيجو != خالي_اشارو) {
                    ڪنجي->parseSuffix(عارضي);
                    LocaleUtility::initLocaleFromName(عارضي, *actualReturn);
                }
            }
            ميٽيو ڪنجي;
        }
    }
    أرجع نتيجو;
}


URegistry_ڪنجي
ICULocaleService::رجسٽر_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& locale, 
    UBool visible, UError_ڪوڊ& حالت)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    أرجع رجسٽر_Instance(obj_منزل_Adopt, loc, Locale_ڪنجي::قسم_ANY, 
        visible ? Locale_ڪنجي_Factory::VISIBLE : Locale_ڪنجي_Factory::INVISIBLE, حالت);
}

URegistry_ڪنجي
ICULocaleService::رجسٽر_Instance(UObject* obj_منزل_Adopt, ثابت Locale& locale, UError_ڪوڊ& حالت)
{
    أرجع رجسٽر_Instance(obj_منزل_Adopt, locale, Locale_ڪنجي::قسم_ANY, Locale_ڪنجي_Factory::VISIBLE, حالت);
}

URegistry_ڪنجي
ICULocaleService::رجسٽر_Instance(UObject* obj_منزل_Adopt, ثابت Locale& locale, int32_t قسم, UError_ڪوڊ& حالت)
{
    أرجع رجسٽر_Instance(obj_منزل_Adopt, locale, قسم, Locale_ڪنجي_Factory::VISIBLE, حالت);
}

URegistry_ڪنجي
ICULocaleService::رجسٽر_Instance(UObject* obj_منزل_Adopt, ثابت Locale& locale, int32_t قسم, int32_t coverage, UError_ڪوڊ& حالت)
{
    ICUServiceFactory * factory = جديد Simple_Locale_ڪنجي_Factory(obj_منزل_Adopt, locale, قسم, coverage);
    إذا (factory != خالي_اشارو) {
        أرجع رجسٽر_Factory(factory, حالت);
    }
    ميٽيو obj_منزل_Adopt;
    أرجع خالي_اشارو;
}

#إذا 0
URegistry_ڪنجي
ICULocaleService::رجسٽر_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& locale, UError_ڪوڊ& حالت)
{
    أرجع رجسٽر_Instance(obj_منزل_Adopt, locale, Locale_ڪنجي::قسم_ANY, Locale_ڪنجي_Factory::VISIBLE, حالت);
}

URegistry_ڪنجي
ICULocaleService::رجسٽر_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& locale, UBool visible, UError_ڪوڊ& حالت)
{
    أرجع رجسٽر_Instance(obj_منزل_Adopt, locale, Locale_ڪنجي::قسم_ANY,
                            visible ? Locale_ڪنجي_Factory::VISIBLE : Locale_ڪنجي_Factory::INVISIBLE,
                            حالت);
}

URegistry_ڪنجي
ICULocaleService::رجسٽر_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& locale, int32_t قسم, int32_t coverage, UError_ڪوڊ& حالت)
{
    ICUServiceFactory * factory = جديد Simple_Locale_ڪنجي_Factory(obj_منزل_Adopt, locale, قسم, coverage);
    إذا (factory != خالي_اشارو) {
        أرجع رجسٽر_Factory(factory, حالت);
    }
    ميٽيو obj_منزل_Adopt;
    أرجع خالي_اشارو;
}
#جيڪڏهن_پورو

صنف ServiceEnumeration : عام StringEnumeration {
خاص:
    ثابت ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

خاص:
    ServiceEnumeration(ثابت ICULocaleService* service U_LIFETIME_BOUND, UError_ڪوڊ& حالت)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, خالي_اشارو, حالت)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, حالت);
    }

    ServiceEnumeration(ثابت ServiceEnumeration &other, UError_ڪوڊ &حالت)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, خالي_اشارو, حالت)
        , _pos(0)
    {
        إذا(U_SUCCESS(حالت)) {
            int32_t i, length;

            length = other._ids.size();
            لكل(i = 0; i < length; ++i) {
                Local_اشارو<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), حالت);
                _ids.adoptElement(clonedId.orphan(), حالت);
            }

            إذا(U_SUCCESS(حالت)) {
                _pos = other._pos;
            }
        }
    }

عام:
    ساكن ServiceEnumeration* create(ثابت ICULocaleService* service U_LIFETIME_BOUND) {
        UError_ڪوڊ حالت = U_ZERO_ERROR;
        ServiceEnumeration* نتيجو = جديد ServiceEnumeration(service, حالت);
        إذا (U_SUCCESS(حالت)) {
            أرجع نتيجو;
        }
        ميٽيو نتيجو;
        أرجع خالي_اشارو;
    }

    مجازي ~ServiceEnumeration();

    مجازي StringEnumeration *clone() ثابت override {
        UError_ڪوڊ حالت = U_ZERO_ERROR;
        ServiceEnumeration *cl = جديد ServiceEnumeration(*هذا, حالت);
        إذا(U_FAILURE(حالت)) {
            ميٽيو cl;
            cl = خالي_اشارو;
        }
        أرجع cl;
    }

    UBool مٿي_منزل_Date(UError_ڪوڊ& حالت) ثابت {
        إذا (U_SUCCESS(حالت)) {
            إذا (_timestamp == _service->getTimestamp()) {
                أرجع سچ;
            }
            حالت = U_ENUM_ٻاهريون_جو_SYNC_ERROR;
        }
        أرجع خطأ;
    }

    مجازي int32_t count(UError_ڪوڊ& حالت) ثابت override {
        أرجع مٿي_منزل_Date(حالت) ? _ids.size() : 0;
    }

    مجازي ثابت UnicodeString* snext(UError_ڪوڊ& حالت) U_LIFETIME_BOUND override {
        إذا (مٿي_منزل_Date(حالت) && (_pos < _ids.size())) {
            أرجع static_cast<ثابت UnicodeString*>(_ids[_pos++]);
        }
        أرجع خالي_اشارو;
    }

    مجازي فراغ reset(UError_ڪوڊ& حالت) override {
        إذا (حالت == U_ENUM_ٻاهريون_جو_SYNC_ERROR) {
            حالت = U_ZERO_ERROR;
        }
        إذا (U_SUCCESS(حالت)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, حالت);
        }
    }

عام:
    ساكن UClassID U_EXPORT2 getStaticClassID();
    مجازي UClassID getDynamicClassID() ثابت override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::get_موجود_Locales() ثابت U_LIFETIME_BOUND
{
    أرجع ServiceEnumeration::create(هذا);
}

ثابت UnicodeString&
ICULocaleService::validateFallbackLocale() ثابت
{
    ثابت Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(هذا);
    ساكن UMutex llock;
    {
        Mutex mutex(&llock);
        إذا (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    أرجع fallbackLocaleName;
}

ICUService_ڪنجي*
ICULocaleService::create_ڪنجي(ثابت UnicodeString* id, UError_ڪوڊ& حالت) ثابت
{
    أرجع Locale_ڪنجي::createWithCanonicalFallback(id, &validateFallbackLocale(), حالت);
}

ICUService_ڪنجي*
ICULocaleService::create_ڪنجي(ثابت UnicodeString* id, int32_t قسم, UError_ڪوڊ& حالت) ثابت
{
    أرجع Locale_ڪنجي::createWithCanonicalFallback(id, &validateFallbackLocale(), قسم, حالت);
}

U_NAMESPACE_پڇاڙي

/* !UCONFIG_NO_SERVICE */
#جيڪڏهن_پورو


