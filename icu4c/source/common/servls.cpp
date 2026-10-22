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

#تعریف_کړه UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کړه AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کړه PERIOD_CHAR     ((char16_t)46)

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
ICULocaleService::get(ثابت Locale& locale, UError_کوډ& حالت) ثابت
{
    أرجع get(locale, Locale_کلی::ډول_ANY, تش_اشاره, حالت);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t ډول, UError_کوډ& حالت) ثابت
{
    أرجع get(locale, ډول, تش_اشاره, حالت);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, Locale* actualReturn, UError_کوډ& حالت) ثابت
{
    أرجع get(locale, Locale_کلی::ډول_ANY, actualReturn, حالت);
}

UObject*
ICULocaleService::get(ثابت Locale& locale, int32_t ډول, Locale* actualReturn, UError_کوډ& حالت) ثابت
{
    UObject* پایله = تش_اشاره;
    إذا (U_FAILURE(حالت)) {
        أرجع پایله;
    }

    UnicodeString locName(locale.getName(), -1, US_INV);
    إذا (locName.isBogus()) {
        حالت = U_حافظه_ویش_ERROR;
    } وإلا {
        ICUService_کلی* کلی = create_کلی(&locName, ډول, حالت);
        إذا (کلی) {
            إذا (actualReturn == تش_اشاره) {
                پایله = get_کلی(*کلی, حالت);
            } وإلا {
                UnicodeString لنډمهاله;
                پایله = get_کلی(*کلی, &لنډمهاله, حالت);

                إذا (پایله != تش_اشاره) {
                    کلی->parseSuffix(لنډمهاله);
                    LocaleUtility::initLocaleFromName(لنډمهاله, *actualReturn);
                }
            }
            ړنګول کلی;
        }
    }
    أرجع پایله;
}


URegistry_کلی
ICULocaleService::ثبتوونکی_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& locale, 
    UBool visible, UError_کوډ& حالت)
{
    Locale loc;
    LocaleUtility::initLocaleFromName(locale, loc);
    أرجع ثبتوونکی_Instance(obj_منزل_Adopt, loc, Locale_کلی::ډول_ANY, 
        visible ? Locale_کلی_Factory::VISIBLE : Locale_کلی_Factory::INVISIBLE, حالت);
}

URegistry_کلی
ICULocaleService::ثبتوونکی_Instance(UObject* obj_منزل_Adopt, ثابت Locale& locale, UError_کوډ& حالت)
{
    أرجع ثبتوونکی_Instance(obj_منزل_Adopt, locale, Locale_کلی::ډول_ANY, Locale_کلی_Factory::VISIBLE, حالت);
}

URegistry_کلی
ICULocaleService::ثبتوونکی_Instance(UObject* obj_منزل_Adopt, ثابت Locale& locale, int32_t ډول, UError_کوډ& حالت)
{
    أرجع ثبتوونکی_Instance(obj_منزل_Adopt, locale, ډول, Locale_کلی_Factory::VISIBLE, حالت);
}

URegistry_کلی
ICULocaleService::ثبتوونکی_Instance(UObject* obj_منزل_Adopt, ثابت Locale& locale, int32_t ډول, int32_t coverage, UError_کوډ& حالت)
{
    ICUServiceFactory * factory = جديد Simple_Locale_کلی_Factory(obj_منزل_Adopt, locale, ډول, coverage);
    إذا (factory != تش_اشاره) {
        أرجع ثبتوونکی_Factory(factory, حالت);
    }
    ړنګول obj_منزل_Adopt;
    أرجع تش_اشاره;
}

#إذا 0
URegistry_کلی
ICULocaleService::ثبتوونکی_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& locale, UError_کوډ& حالت)
{
    أرجع ثبتوونکی_Instance(obj_منزل_Adopt, locale, Locale_کلی::ډول_ANY, Locale_کلی_Factory::VISIBLE, حالت);
}

URegistry_کلی
ICULocaleService::ثبتوونکی_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& locale, UBool visible, UError_کوډ& حالت)
{
    أرجع ثبتوونکی_Instance(obj_منزل_Adopt, locale, Locale_کلی::ډول_ANY,
                            visible ? Locale_کلی_Factory::VISIBLE : Locale_کلی_Factory::INVISIBLE,
                            حالت);
}

URegistry_کلی
ICULocaleService::ثبتوونکی_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& locale, int32_t ډول, int32_t coverage, UError_کوډ& حالت)
{
    ICUServiceFactory * factory = جديد Simple_Locale_کلی_Factory(obj_منزل_Adopt, locale, ډول, coverage);
    إذا (factory != تش_اشاره) {
        أرجع ثبتوونکی_Factory(factory, حالت);
    }
    ړنګول obj_منزل_Adopt;
    أرجع تش_اشاره;
}
#د_که_پای

صنف ServiceEnumeration : عام StringEnumeration {
خاص:
    ثابت ICULocaleService* _service;
    int32_t _timestamp;
    UVector _ids;
    int32_t _pos;

خاص:
    ServiceEnumeration(ثابت ICULocaleService* service U_LIFETIME_BOUND, UError_کوډ& حالت)
        : _service(service)
        , _timestamp(service->getTimestamp())
        , _ids(uprv_deleteUObject, تش_اشاره, حالت)
        , _pos(0)
    {
        _service->getVisibleIDs(_ids, حالت);
    }

    ServiceEnumeration(ثابت ServiceEnumeration &other, UError_کوډ &حالت)
        : _service(other._service)
        , _timestamp(other._timestamp)
        , _ids(uprv_deleteUObject, تش_اشاره, حالت)
        , _pos(0)
    {
        إذا(U_SUCCESS(حالت)) {
            int32_t i, length;

            length = other._ids.size();
            لكل(i = 0; i < length; ++i) {
                Local_اشاره_ګر<UnicodeString> clonedId(static_cast<UnicodeString*>(other._ids.elementAt(i))->clone(), حالت);
                _ids.adoptElement(clonedId.orphan(), حالت);
            }

            إذا(U_SUCCESS(حالت)) {
                _pos = other._pos;
            }
        }
    }

عام:
    ساكن ServiceEnumeration* create(ثابت ICULocaleService* service U_LIFETIME_BOUND) {
        UError_کوډ حالت = U_ZERO_ERROR;
        ServiceEnumeration* پایله = جديد ServiceEnumeration(service, حالت);
        إذا (U_SUCCESS(حالت)) {
            أرجع پایله;
        }
        ړنګول پایله;
        أرجع تش_اشاره;
    }

    مجازي ~ServiceEnumeration();

    مجازي StringEnumeration *clone() ثابت override {
        UError_کوډ حالت = U_ZERO_ERROR;
        ServiceEnumeration *cl = جديد ServiceEnumeration(*هذا, حالت);
        إذا(U_FAILURE(حالت)) {
            ړنګول cl;
            cl = تش_اشاره;
        }
        أرجع cl;
    }

    UBool پورته_منزل_Date(UError_کوډ& حالت) ثابت {
        إذا (U_SUCCESS(حالت)) {
            إذا (_timestamp == _service->getTimestamp()) {
                أرجع ریښتیا;
            }
            حالت = U_ENUM_بهر_د_SYNC_ERROR;
        }
        أرجع خطأ;
    }

    مجازي int32_t count(UError_کوډ& حالت) ثابت override {
        أرجع پورته_منزل_Date(حالت) ? _ids.size() : 0;
    }

    مجازي ثابت UnicodeString* snext(UError_کوډ& حالت) U_LIFETIME_BOUND override {
        إذا (پورته_منزل_Date(حالت) && (_pos < _ids.size())) {
            أرجع static_cast<ثابت UnicodeString*>(_ids[_pos++]);
        }
        أرجع تش_اشاره;
    }

    مجازي فراغ reset(UError_کوډ& حالت) override {
        إذا (حالت == U_ENUM_بهر_د_SYNC_ERROR) {
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
ICULocaleService::get_شته_Locales() ثابت U_LIFETIME_BOUND
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

ICUService_کلی*
ICULocaleService::create_کلی(ثابت UnicodeString* id, UError_کوډ& حالت) ثابت
{
    أرجع Locale_کلی::createWithCanonicalFallback(id, &validateFallbackLocale(), حالت);
}

ICUService_کلی*
ICULocaleService::create_کلی(ثابت UnicodeString* id, int32_t ډول, UError_کوډ& حالت) ثابت
{
    أرجع Locale_کلی::createWithCanonicalFallback(id, &validateFallbackLocale(), ډول, حالت);
}

U_NAMESPACE_پای_ټکی

/* !UCONFIG_NO_SERVICE */
#د_که_پای


