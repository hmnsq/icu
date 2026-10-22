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
ICULocaleService::get(ثابت Locale& locale, UError_رمز& حالت_2) ثابت
{
    واپس get(locale, Locale_کلید::قسم_ANY, خالی_اشاریہ, حالت_2);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t قسم, UError_رمز& حالت_2) ثابت
{
    واپس get(locale, قسم, خالی_اشاریہ, حالت_2);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, Locale* actualReturn, UError_رمز& حالت_2) ثابت
{
    واپس get(locale, Locale_کلید::قسم_ANY, actualReturn, حالت_2);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t قسم, Locale* actualReturn, UError_رمز& حالت_2) ثابت
{
    UObject* نتیجہ = خالی_اشاریہ;
    اگر (U_FAILURE(حالت_2)) {
        واپس نتیجہ;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    اگر (locName.isBogus()) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
    } ورنہ {
        ICUService_کلید* کلید = create_کلید(&locName, قسم, حالت_2);
        اگر (کلید) {
            اگر (actualReturn == خالی_اشاریہ) {
                نتیجہ = get_کلید(*کلید, حالت_2);
            } ورنہ {
                UnicodeString عارضی;
                نتیجہ = get_کلید(*کلید, &عارضی, حالت_2);

                اگر (نتیجہ != خالی_اشاریہ) {
                    کلید->parseSuffix(عارضی);
                    LocaleUtility::initLocaleFromName(عارضی, *actualReturn);
                }
            }
            حذف کلید;
        }
    }
    واپس نتیجہ;
}


URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_مقصد_Adopt, ثابت UnicodeString& locale, 
    UBool visible, UError_رمز& حالت_2)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    واپس رجسٹر_Instance(obj_مقصد_Adopt, loc, Locale_کلید::قسم_ANY, 
        visible ? Locale_کلید_Factory::VISIBLE : Locale_کلید_Factory::INVISIBLE, حالت_2);
}

URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_مقصد_Adopt, ثابت Locale& locale, UError_رمز& حالت_2)
{
    واپس رجسٹر_Instance(obj_مقصد_Adopt, locale, Locale_کلید::قسم_ANY, Locale_کلید_Factory::VISIBLE, حالت_2);
}

URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_مقصد_Adopt, ثابت Locale& locale, int32_t قسم, UError_رمز& حالت_2)
{
    واپس رجسٹر_Instance(obj_مقصد_Adopt, locale, قسم, Locale_کلید_Factory::VISIBLE, حالت_2);
}

URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_مقصد_Adopt, ثابت Locale& locale, int32_t قسم, int32_t coverage, UError_رمز& حالت_2)
{
    ICUServiceFactory * factory = نیا Simple_Locale_کلید_Factory(obj_مقصد_Adopt, locale, قسم, coverage);
    اگر (factory != خالی_اشاریہ) {
        واپس رجسٹر_Factory(factory, حالت_2);
    }
    حذف obj_مقصد_Adopt;
    واپس خالی_اشاریہ;
}

#اگر 0
URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_مقصد_Adopt, ثابت UnicodeString& locale, UError_رمز& حالت_2)
{
    واپس رجسٹر_Instance(obj_مقصد_Adopt, locale, Locale_کلید::قسم_ANY, Locale_کلید_Factory::VISIBLE, حالت_2);
}

URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_مقصد_Adopt, ثابت UnicodeString& locale, UBool visible, UError_رمز& حالت_2)
{
    واپس رجسٹر_Instance(obj_مقصد_Adopt, locale, Locale_کلید::قسم_ANY,
                            visible ? Locale_کلید_Factory::VISIBLE : Locale_کلید_Factory::INVISIBLE,
                            حالت_2);
}

URegistry_کلید
ICULocaleService::رجسٹر_Instance(UObject* obj_مقصد_Adopt, ثابت UnicodeString& locale, int32_t قسم, int32_t coverage, UError_رمز& حالت_2)
{
    ICUServiceFactory * factory = نیا Simple_Locale_کلید_Factory(obj_مقصد_Adopt, locale, قسم, coverage);
    اگر (factory != خالی_اشاریہ) {
        واپس رجسٹر_Factory(factory, حالت_2);
    }
    حذف obj_مقصد_Adopt;
    واپس خالی_اشاریہ;
}
#اگر_ختم

جماعت ServiceEnumeration : عوامی StringEnumeration {
نجی:
    ثابت ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

نجی:
    ServiceEnumeration(ثابت ICULocaleService* service U_LIFETIME_BOUND, UError_رمز& حالت_2)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, خالی_اشاریہ, حالت_2)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, حالت_2);
    }

    ServiceEnumeration(ثابت ServiceEnumeration &other, UError_رمز &حالت_2)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, خالی_اشاریہ, حالت_2)
        , _pos(0)
    {
        اگر(U_SUCCESS(حالت_2)) {
            int32_t i, length;

            length = other._ids.size();
            برائے(i = 0; i < length; ++i) {
                Local_اشارہ_گر<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), حالت_2);
                _ids.adoptElement(clonedId.orphan(), حالت_2);
            }

            اگر(U_SUCCESS(حالت_2)) {
                _pos = other._pos;
            }
        }
    }

عوامی:
    ساکن ServiceEnumeration* create(ثابت ICULocaleService* service U_LIFETIME_BOUND) {
        UError_رمز حالت_2 = U_ZERO_ERROR;
        ServiceEnumeration* نتیجہ = نیا ServiceEnumeration(service, حالت_2);
        اگر (U_SUCCESS(حالت_2)) {
            واپس نتیجہ;
        }
        حذف نتیجہ;
        واپس خالی_اشاریہ;
    }

    مجازی ~ServiceEnumeration();

    مجازی StringEnumeration *clone() ثابت override {
        UError_رمز حالت_2 = U_ZERO_ERROR;
        ServiceEnumeration *cl = نیا ServiceEnumeration(*یہ, حالت_2);
        اگر(U_FAILURE(حالت_2)) {
            حذف cl;
            cl = خالی_اشاریہ;
        }
        واپس cl;
    }

    UBool اوپر_مقصد_Date(UError_رمز& حالت_2) ثابت {
        اگر (U_SUCCESS(حالت_2)) {
            اگر (_timestamp == _service->getTimestamp()) {
                واپس سچ;
            }
            حالت_2 = U_ENUM_برآمد_کا_SYNC_ERROR;
        }
        واپس جھوٹ;
    }

    مجازی int32_t count(UError_رمز& حالت_2) ثابت override {
        واپس اوپر_مقصد_Date(حالت_2) ? _ids.size() : 0;
    }

    مجازی ثابت UnicodeString* snext(UError_رمز& حالت_2) U_LIFETIME_BOUND override {
        اگر (اوپر_مقصد_Date(حالت_2) && (_pos < _ids.size())) {
            واپس static_cast<ثابت UnicodeString*>(_ids[_pos++]);
        }
        واپس خالی_اشاریہ;
    }

    مجازی خالی reset(UError_رمز& حالت_2) override {
        اگر (حالت_2 == U_ENUM_برآمد_کا_SYNC_ERROR) {
            حالت_2 = U_ZERO_ERROR;
        }
        اگر (U_SUCCESS(حالت_2)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, حالت_2);
        }
    }

عوامی:
    ساکن UClassID U_EXPORT2 getStaticClassID();
    مجازی UClassID getDynamicClassID() ثابت override;
};

ServiceEnumeration::~ServiceEnumeration() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceEnumeration)

StringEnumeration*
ICULocaleService::get_دستیاب_Locales() ثابت U_LIFETIME_BOUND
{
    واپس ServiceEnumeration::create(یہ);
}

ثابت UnicodeString&
ICULocaleService::validateFallbackLocale() ثابت
{
    ثابت Locale&     loc    = Locale::getDefault();
    ICULocaleService* ncThis = const_cast<ICULocaleService*>(یہ);
    ساکن UMutex llock;
    {
        Mutex mutex(&llock);
        اگر (loc != fallbackLocale) {
            ncThis->fallbackLocale = loc;
            LocaleUtility::initNameFromLocale(loc, ncThis->fallbackLocaleName);
            ncThis->clearServiceCache();
        }
    }
    واپس fallbackLocaleName;
}

ICUService_کلید*
ICULocaleService::create_کلید(ثابت UnicodeString* id, UError_رمز& حالت_2) ثابت
{
    واپس Locale_کلید::createWithCanonicalFallback(id, &validateFallbackLocale(), حالت_2);
}

ICUService_کلید*
ICULocaleService::create_کلید(ثابت UnicodeString* id, int32_t قسم, UError_رمز& حالت_2) ثابت
{
    واپس Locale_کلید::createWithCanonicalFallback(id, &validateFallbackLocale(), قسم, حالت_2);
}

U_NAMESPACE_انتہا

/* !UCONFIG_NO_SERVICE */
#اگر_ختم


