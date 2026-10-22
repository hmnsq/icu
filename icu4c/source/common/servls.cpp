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

#تعریف_کن UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کن AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کن PERIOD_CHAR     ((char16_t)46)

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
ICULocaleService::get(ثابت Locale& locale, UError_کد& وضعیت) ثابت
{
    أرجع get(locale, Locale_کلید::نوع_ANY, اشاره_تهی, وضعیت);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t نوع, UError_کد& وضعیت) ثابت
{
    أرجع get(locale, نوع, اشاره_تهی, وضعیت);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, Locale* actualReturn, UError_کد& وضعیت) ثابت
{
    أرجع get(locale, Locale_کلید::نوع_ANY, actualReturn, وضعیت);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t نوع, Locale* actualReturn, UError_کد& وضعیت) ثابت
{
    UObject* نتیجه = اشاره_تهی;
    إذا (U_FAILURE(وضعیت)) {
        أرجع نتیجه;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    إذا (locName.isBogus()) {
        وضعیت = U_حافظه_تخصیص_ERROR;
    } وإلا {
        ICUService_کلید* کلید = create_کلید(&locName, نوع, وضعیت);
        إذا (کلید) {
            إذا (actualReturn == اشاره_تهی) {
                نتیجه = get_کلید(*کلید, وضعیت);
            } وإلا {
                UnicodeString موقت;
                نتیجه = get_کلید(*کلید, &موقت, وضعیت);

                إذا (نتیجه != اشاره_تهی) {
                    کلید->parseSuffix(موقت);
                    LocaleUtility::initLocaleFromName(موقت, *actualReturn);
                }
            }
            حذف کلید;
        }
    }
    أرجع نتیجه;
}


URegistry_کلید
ICULocaleService::ثبات_Instance(UObject* obj_مقصد_Adopt, ثابت UnicodeString& locale, 
    UBool visible, UError_کد& وضعیت)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    أرجع ثبات_Instance(obj_مقصد_Adopt, loc, Locale_کلید::نوع_ANY, 
        visible ? Locale_کلید_Factory::VISIBLE : Locale_کلید_Factory::INVISIBLE, وضعیت);
}

URegistry_کلید
ICULocaleService::ثبات_Instance(UObject* obj_مقصد_Adopt, ثابت Locale& locale, UError_کد& وضعیت)
{
    أرجع ثبات_Instance(obj_مقصد_Adopt, locale, Locale_کلید::نوع_ANY, Locale_کلید_Factory::VISIBLE, وضعیت);
}

URegistry_کلید
ICULocaleService::ثبات_Instance(UObject* obj_مقصد_Adopt, ثابت Locale& locale, int32_t نوع, UError_کد& وضعیت)
{
    أرجع ثبات_Instance(obj_مقصد_Adopt, locale, نوع, Locale_کلید_Factory::VISIBLE, وضعیت);
}

URegistry_کلید
ICULocaleService::ثبات_Instance(UObject* obj_مقصد_Adopt, ثابت Locale& locale, int32_t نوع, int32_t coverage, UError_کد& وضعیت)
{
    ICUServiceFactory * factory = جديد Simple_Locale_کلید_Factory(obj_مقصد_Adopt, locale, نوع, coverage);
    إذا (factory != اشاره_تهی) {
        أرجع ثبات_Factory(factory, وضعیت);
    }
    حذف obj_مقصد_Adopt;
    أرجع اشاره_تهی;
}

#إذا 0
URegistry_کلید
ICULocaleService::ثبات_Instance(UObject* obj_مقصد_Adopt, ثابت UnicodeString& locale, UError_کد& وضعیت)
{
    أرجع ثبات_Instance(obj_مقصد_Adopt, locale, Locale_کلید::نوع_ANY, Locale_کلید_Factory::VISIBLE, وضعیت);
}

URegistry_کلید
ICULocaleService::ثبات_Instance(UObject* obj_مقصد_Adopt, ثابت UnicodeString& locale, UBool visible, UError_کد& وضعیت)
{
    أرجع ثبات_Instance(obj_مقصد_Adopt, locale, Locale_کلید::نوع_ANY,
                            visible ? Locale_کلید_Factory::VISIBLE : Locale_کلید_Factory::INVISIBLE,
                            وضعیت);
}

URegistry_کلید
ICULocaleService::ثبات_Instance(UObject* obj_مقصد_Adopt, ثابت UnicodeString& locale, int32_t نوع, int32_t coverage, UError_کد& وضعیت)
{
    ICUServiceFactory * factory = جديد Simple_Locale_کلید_Factory(obj_مقصد_Adopt, locale, نوع, coverage);
    إذا (factory != اشاره_تهی) {
        أرجع ثبات_Factory(factory, وضعیت);
    }
    حذف obj_مقصد_Adopt;
    أرجع اشاره_تهی;
}
#پایان_اگر

صنف ServiceEnumeration : عام StringEnumeration {
خاص:
    ثابت ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

خاص:
    ServiceEnumeration(ثابت ICULocaleService* service U_LIFETIME_BOUND, UError_کد& وضعیت)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, اشاره_تهی, وضعیت)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, وضعیت);
    }

    ServiceEnumeration(ثابت ServiceEnumeration &other, UError_کد &وضعیت)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, اشاره_تهی, وضعیت)
        , _pos(0)
    {
        إذا(U_SUCCESS(وضعیت)) {
            int32_t i, length;

            length = other._ids.size();
            لكل(i = 0; i < length; ++i) {
                Local_اشاره_گر<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), وضعیت);
                _ids.adoptElement(clonedId.orphan(), وضعیت);
            }

            إذا(U_SUCCESS(وضعیت)) {
                _pos = other._pos;
            }
        }
    }

عام:
    ساكن ServiceEnumeration* create(ثابت ICULocaleService* service U_LIFETIME_BOUND) {
        UError_کد وضعیت = U_ZERO_ERROR;
        ServiceEnumeration* نتیجه = جديد ServiceEnumeration(service, وضعیت);
        إذا (U_SUCCESS(وضعیت)) {
            أرجع نتیجه;
        }
        حذف نتیجه;
        أرجع اشاره_تهی;
    }

    مجازی ~ServiceEnumeration();

    مجازی StringEnumeration *clone() ثابت override {
        UError_کد وضعیت = U_ZERO_ERROR;
        ServiceEnumeration *cl = جديد ServiceEnumeration(*هذا, وضعیت);
        إذا(U_FAILURE(وضعیت)) {
            حذف cl;
            cl = اشاره_تهی;
        }
        أرجع cl;
    }

    UBool به_بالا_مقصد_Date(UError_کد& وضعیت) ثابت {
        إذا (U_SUCCESS(وضعیت)) {
            إذا (_timestamp == _service->getTimestamp()) {
                أرجع درست;
            }
            وضعیت = U_ENUM_بیرون_از_SYNC_ERROR;
        }
        أرجع خطأ;
    }

    مجازی int32_t count(UError_کد& وضعیت) ثابت override {
        أرجع به_بالا_مقصد_Date(وضعیت) ? _ids.size() : 0;
    }

    مجازی ثابت UnicodeString* snext(UError_کد& وضعیت) U_LIFETIME_BOUND override {
        إذا (به_بالا_مقصد_Date(وضعیت) && (_pos < _ids.size())) {
            أرجع static_cast<ثابت UnicodeString*>(_ids[_pos++]);
        }
        أرجع اشاره_تهی;
    }

    مجازی فراغ reset(UError_کد& وضعیت) override {
        إذا (وضعیت == U_ENUM_بیرون_از_SYNC_ERROR) {
            وضعیت = U_ZERO_ERROR;
        }
        إذا (U_SUCCESS(وضعیت)) {
            _timestamp = _service->getTimestamp();
            _pos = 0;
            _service->getVisibleIDs(_ids, وضعیت);
        }
    }

عام:
    ساكن UClassID U_EXPORT2 getStaticClassID();
    مجازی UClassID getDynamicClassID() ثابت override;
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

ICUService_کلید*
ICULocaleService::create_کلید(ثابت UnicodeString* id, UError_کد& وضعیت) ثابت
{
    أرجع Locale_کلید::createWithCanonicalFallback(id, &validateFallbackLocale(), وضعیت);
}

ICUService_کلید*
ICULocaleService::create_کلید(ثابت UnicodeString* id, int32_t نوع, UError_کد& وضعیت) ثابت
{
    أرجع Locale_کلید::createWithCanonicalFallback(id, &validateFallbackLocale(), نوع, وضعیت);
}

U_NAMESPACE_انتها

/* !UCONFIG_NO_SERVICE */
#پایان_اگر


