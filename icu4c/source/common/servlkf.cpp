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
#ضمن "uhash.h"
#ضمن "charstr.h"
#ضمن "ucln_cmn.h"
#ضمن "uassert.h"

#تعریف_کن UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کن AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کن PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_کلید_Factory::Locale_کلید_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_کلید_Factory::Locale_کلید_Factory(int32_t coverage, ثابت UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_کلید_Factory::~Locale_کلید_Factory() {
}

UObject*
Locale_کلید_Factory::create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UError_کد& وضعیت) ثابت {
    إذا (handles_کلید(کلید, وضعیت)) {
        ثابت Locale_کلید& lkey = static_cast<ثابت Locale_کلید&>(کلید);
        int32_t نوع = lkey.نوع();
        Locale loc;
        lkey.currentLocale(loc);

        أرجع handleCreate(loc, نوع, service, وضعیت);
    }
    أرجع اشاره_تهی;
}

UBool
Locale_کلید_Factory::handles_کلید(ثابت ICUService_کلید& کلید, UError_کد& وضعیت) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(وضعیت);
    إذا (supported) {
        UnicodeString id;
        کلید.currentID(id);
        أرجع supported->get(id) != اشاره_تهی;
    }
    أرجع خطأ;
}

فراغ
Locale_کلید_Factory::updateVisibleIDs(Hashtable& نتیجه, UError_کد& وضعیت) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(وضعیت);
    إذا (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ثابت UHashElement* elem = اشاره_تهی;
        int32_t pos = UHASH_اولی;
        طالما ((elem = supported->بعدی_Element(pos)) != اشاره_تهی) {
            ثابت UnicodeString& id = *static_cast<ثابت UnicodeString*>(elem->کلید.اشاره_گر);
            إذا (!visible) {
                نتیجه.remove(id);
            } وإلا {
                نتیجه.put(id, (فراغ*)هذا, وضعیت); // this is dummy non-void marker used for set semantics
                إذا (U_FAILURE(وضعیت)) {
                    اكسر;
                }
            }
        }
    }
}

UnicodeString&
Locale_کلید_Factory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& locale, UnicodeString& نتیجه) ثابت {
    إذا ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            أرجع loc.getDisplayName(locale, نتیجه);
        // }
    }
    نتیجه.مجموعه_مقصد_Bogus();
    أرجع نتیجه;
}

UObject*
Locale_کلید_Factory::handleCreate(ثابت Locale& /* loc */, 
                   int32_t /*نوع*/, 
                   ثابت ICUService* /* service */, 
                   UError_کد& /* status */) ثابت {
    أرجع اشاره_تهی;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ثابت Hashtable*
Locale_کلید_Factory::getSupportedIDs(UError_کد& /* status */) ثابت {
    أرجع اشاره_تهی;
}

#اگر_تعریف_شده SERVICE_DEBUG
UnicodeString&
Locale_کلید_Factory::debug(UnicodeString& نتیجه) ثابت
{
    debugClass(نتیجه);
    نتیجه.append((UnicodeString)", name: ");
    نتیجه.append(_name);
    نتیجه.append((UnicodeString)", coverage: ");
    نتیجه.append(_coverage);
    أرجع نتیجه;
}

UnicodeString&
Locale_کلید_Factory::debugClass(UnicodeString& نتیجه) ثابت
{
  أرجع نتیجه.append((UnicodeString)"LocaleKeyFactory");
}
#پایان_اگر

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_کلید_Factory)

U_NAMESPACE_انتها

/* !UCONFIG_NO_SERVICE */
#پایان_اگر


