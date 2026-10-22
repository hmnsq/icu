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

#تعریف_کرو UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کرو AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کرو PERIOD_CHAR     ((char16_t)46)


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
Locale_کلید_Factory::create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UError_کوڈ& حالت) ثابت {
    إذا (handles_کلید(کلید, حالت)) {
        ثابت Locale_کلید& lkey = static_cast<ثابت Locale_کلید&>(کلید);
        int32_t قسم = lkey.قسم();
        Locale loc;
        lkey.currentLocale(loc);

        أرجع handleCreate(loc, قسم, service, حالت);
    }
    أرجع خالی_سوچک;
}

UBool
Locale_کلید_Factory::handles_کلید(ثابت ICUService_کلید& کلید, UError_کوڈ& حالت) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(حالت);
    إذا (supported) {
        UnicodeString id;
        کلید.currentID(id);
        أرجع supported->get(id) != خالی_سوچک;
    }
    أرجع خطأ;
}

فراغ
Locale_کلید_Factory::updateVisibleIDs(Hashtable& نتیجہ, UError_کوڈ& حالت) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(حالت);
    إذا (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ثابت UHashElement* elem = خالی_سوچک;
        int32_t pos = UHASH_پہلا;
        طالما ((elem = supported->اگلا_Element(pos)) != خالی_سوچک) {
            ثابت UnicodeString& id = *static_cast<ثابت UnicodeString*>(elem->کلید.اشارہ_گر);
            إذا (!visible) {
                نتیجہ.remove(id);
            } وإلا {
                نتیجہ.put(id, (فراغ*)هذا, حالت); // this is dummy non-void marker used for set semantics
                إذا (U_FAILURE(حالت)) {
                    اكسر;
                }
            }
        }
    }
}

UnicodeString&
Locale_کلید_Factory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& locale, UnicodeString& نتیجہ) ثابت {
    إذا ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            أرجع loc.getDisplayName(locale, نتیجہ);
        // }
    }
    نتیجہ.سیٹ_منزل_Bogus();
    أرجع نتیجہ;
}

UObject*
Locale_کلید_Factory::handleCreate(ثابت Locale& /* loc */, 
                   int32_t /*قسم*/, 
                   ثابت ICUService* /* service */, 
                   UError_کوڈ& /* status */) ثابت {
    أرجع خالی_سوچک;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ثابت Hashtable*
Locale_کلید_Factory::getSupportedIDs(UError_کوڈ& /* status */) ثابت {
    أرجع خالی_سوچک;
}

#جے_تعریف_شدہ SERVICE_DEBUG
UnicodeString&
Locale_کلید_Factory::debug(UnicodeString& نتیجہ) ثابت
{
    debugClass(نتیجہ);
    نتیجہ.append((UnicodeString)", name: ");
    نتیجہ.append(_name);
    نتیجہ.append((UnicodeString)", coverage: ");
    نتیجہ.append(_coverage);
    أرجع نتیجہ;
}

UnicodeString&
Locale_کلید_Factory::debugClass(UnicodeString& نتیجہ) ثابت
{
  أرجع نتیجہ.append((UnicodeString)"LocaleKeyFactory");
}
#جے_انت

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_کلید_Factory)

U_NAMESPACE_سرا

/* !UCONFIG_NO_SERVICE */
#جے_انت


