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

#تعریف_کرو UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کرو AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کرو PERIOD_CHAR     ((char16_t)46)

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
ICULocaleService::get(ثابت Locale& locale, UError_کوڈ& حالت) ثابت
{
    أرجع get(locale, Locale_کلید::قسم_ANY, خالی_سوچک, حالت);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t قسم, UError_کوڈ& حالت) ثابت
{
    أرجع get(locale, قسم, خالی_سوچک, حالت);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, Locale* actualReturn, UError_کوڈ& حالت) ثابت
{
    أرجع get(locale, Locale_کلید::قسم_ANY, actualReturn, حالت);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t قسم, Locale* actualReturn, UError_کوڈ& حالت) ثابت
{
    UObject* نتیجہ = خالی_سوچک;
    إذا (U_FAILURE(حالت)) {
        أرجع نتیجہ;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    إذا (locName.isBogus()) {
        حالت = U_حافظہ_ونڈ_ERROR;
    } وإلا {
        ICUService_کلید* کلید = create_کلید(&locName, قسم, حالت);
        إذا (کلید) {
            إذا (actualReturn == خالی_سوچک) {
                نتیجہ = get_کلید(*کلید, حالت);
            } وإلا {
                UnicodeString عارضی;
                نتیجہ = get_کلید(*کلید, &عارضی, حالت);

                إذا (نتیجہ != خالی_سوچک) {
                    کلید->parseSuffix(عارضی);
                    LocaleUtility::initLocaleFromName(عارضی, *actualReturn);
                }
            }
            مٹاؤ کلید;
        }
    }
    أرجع نتیجہ;
}


URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& locale, 
    UBool visible, UError_کوڈ& حالت)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    أرجع رجسٹر_Instance(obj_منزل_Adopt, loc, Locale_کلید::قسم_ANY, 
        visible ? Locale_کلید_Factory::VISIBLE : Locale_کلید_Factory::INVISIBLE, حالت);
}

URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_منزل_Adopt, ثابت Locale& locale, UError_کوڈ& حالت)
{
    أرجع رجسٹر_Instance(obj_منزل_Adopt, locale, Locale_کلید::قسم_ANY, Locale_کلید_Factory::VISIBLE, حالت);
}

URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_منزل_Adopt, ثابت Locale& locale, int32_t قسم, UError_کوڈ& حالت)
{
    أرجع رجسٹر_Instance(obj_منزل_Adopt, locale, قسم, Locale_کلید_Factory::VISIBLE, حالت);
}

URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_منزل_Adopt, ثابت Locale& locale, int32_t قسم, int32_t coverage, UError_کوڈ& حالت)
{
    ICUServiceFactory * factory = جديد Simple_Locale_کلید_Factory(obj_منزل_Adopt, locale, قسم, coverage);
    إذا (factory != خالی_سوچک) {
        أرجع رجسٹر_Factory(factory, حالت);
    }
    مٹاؤ obj_منزل_Adopt;
    أرجع خالی_سوچک;
}

#إذا 0
URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& locale, UError_کوڈ& حالت)
{
    أرجع رجسٹر_Instance(obj_منزل_Adopt, locale, Locale_کلید::قسم_ANY, Locale_کلید_Factory::VISIBLE, حالت);
}

URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& locale, UBool visible, UError_کوڈ& حالت)
{
    أرجع رجسٹر_Instance(obj_منزل_Adopt, locale, Locale_کلید::قسم_ANY,
                            visible ? Locale_کلید_Factory::VISIBLE : Locale_کلید_Factory::INVISIBLE,
                            حالت);
}

URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& locale, int32_t قسم, int32_t coverage, UError_کوڈ& حالت)
{
    ICUServiceFactory * factory = جديد Simple_Locale_کلید_Factory(obj_منزل_Adopt, locale, قسم, coverage);
    إذا (factory != خالی_سوچک) {
        أرجع رجسٹر_Factory(factory, حالت);
    }
    مٹاؤ obj_منزل_Adopt;
    أرجع خالی_سوچک;
}
#جے_انت

صنف ServiceEnumeration : عام StringEnumeration {
خاص:
    ثابت ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

خاص:
    ServiceEnumeration(ثابت ICULocaleService* service U_LIFETIME_BOUND, UError_کوڈ& حالت)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, خالی_سوچک, حالت)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, حالت);
    }

    ServiceEnumeration(ثابت ServiceEnumeration &other, UError_کوڈ &حالت)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, خالی_سوچک, حالت)
        , _pos(0)
    {
        إذا(U_SUCCESS(حالت)) {
            int32_t i, length;

            length = other._ids.size();
            لكل(i = 0; i < length; ++i) {
                Local_اشارہ_گر<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), حالت);
                _ids.adoptElement(clonedId.orphan(), حالت);
            }

            إذا(U_SUCCESS(حالت)) {
                _pos = other._pos;
            }
        }
    }

عام:
    ساكن ServiceEnumeration* create(ثابت ICULocaleService* service U_LIFETIME_BOUND) {
        UError_کوڈ حالت = U_ZERO_ERROR;
        ServiceEnumeration* نتیجہ = جديد ServiceEnumeration(service, حالت);
        إذا (U_SUCCESS(حالت)) {
            أرجع نتیجہ;
        }
        مٹاؤ نتیجہ;
        أرجع خالی_سوچک;
    }

    مجازی ~ServiceEnumeration();

    مجازی StringEnumeration *clone() ثابت override {
        UError_کوڈ حالت = U_ZERO_ERROR;
        ServiceEnumeration *cl = جديد ServiceEnumeration(*هذا, حالت);
        إذا(U_FAILURE(حالت)) {
            مٹاؤ cl;
            cl = خالی_سوچک;
        }
        أرجع cl;
    }

    UBool اتے_منزل_Date(UError_کوڈ& حالت) ثابت {
        إذا (U_SUCCESS(حالت)) {
            إذا (_timestamp == _service->getTimestamp()) {
                أرجع سچ;
            }
            حالت = U_ENUM_باہرلا_دا_SYNC_ERROR;
        }
        أرجع خطأ;
    }

    مجازی int32_t count(UError_کوڈ& حالت) ثابت override {
        أرجع اتے_منزل_Date(حالت) ? _ids.size() : 0;
    }

    مجازی ثابت UnicodeString* snext(UError_کوڈ& حالت) U_LIFETIME_BOUND override {
        إذا (اتے_منزل_Date(حالت) && (_pos < _ids.size())) {
            أرجع static_cast<ثابت UnicodeString*>(_ids[_pos++]);
        }
        أرجع خالی_سوچک;
    }

    مجازی فراغ reset(UError_کوڈ& حالت) override {
        إذا (حالت == U_ENUM_باہرلا_دا_SYNC_ERROR) {
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
    مجازی UClassID getDynamicClassID() ثابت override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::get_دستیاب_Locales() ثابت U_LIFETIME_BOUND
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

ICUService_کلید*
ICULocaleService::create_کلید(ثابت UnicodeString* id, UError_کوڈ& حالت) ثابت
{
    أرجع Locale_کلید::createWithCanonicalFallback(id, &validateFallbackLocale(), حالت);
}

ICUService_کلید*
ICULocaleService::create_کلید(ثابت UnicodeString* id, int32_t قسم, UError_کوڈ& حالت) ثابت
{
    أرجع Locale_کلید::createWithCanonicalFallback(id, &validateFallbackLocale(), قسم, حالت);
}

U_NAMESPACE_سرا

/* !UCONFIG_NO_SERVICE */
#جے_انت


