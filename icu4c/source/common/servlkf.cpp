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
#شامل "uhash.h"
#شامل "charstr.h"
#شامل "ucln_cmn.h"
#شامل "uassert.h"

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
Locale_کلید_Factory::create(ثابت ICUService_کلید& کلید, ثابت ICUService* service, UError_رمز& حالت_2) ثابت {
    اگر (handles_کلید(کلید, حالت_2)) {
        ثابت Locale_کلید& lkey = static_cast<ثابت Locale_کلید&>(کلید);
        int32_t قسم = lkey.قسم();
        Locale loc;
        lkey.currentLocale(loc);

        واپس handleCreate(loc, قسم, service, حالت_2);
    }
    واپس خالی_اشاریہ;
}

UBool
Locale_کلید_Factory::handles_کلید(ثابت ICUService_کلید& کلید, UError_رمز& حالت_2) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(حالت_2);
    اگر (supported) {
        UnicodeString id;
        کلید.currentID(id);
        واپس supported->get(id) != خالی_اشاریہ;
    }
    واپس جھوٹ;
}

خالی
Locale_کلید_Factory::updateVisibleIDs(Hashtable& نتیجہ, UError_رمز& حالت_2) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(حالت_2);
    اگر (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ثابت UHashElement* elem = خالی_اشاریہ;
        int32_t pos = UHASH_پہلا;
        جب ((elem = supported->اگلا_Element(pos)) != خالی_اشاریہ) {
            ثابت UnicodeString& id = *static_cast<ثابت UnicodeString*>(elem->کلید.اشارہ_گر);
            اگر (!visible) {
                نتیجہ.remove(id);
            } ورنہ {
                نتیجہ.put(id, (خالی*)یہ, حالت_2); // this is dummy non-void marker used for set semantics
                اگر (U_FAILURE(حالت_2)) {
                    توڑو;
                }
            }
        }
    }
}

UnicodeString&
Locale_کلید_Factory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& locale, UnicodeString& نتیجہ) ثابت {
    اگر ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            واپس loc.getDisplayName(locale, نتیجہ);
        // }
    }
    نتیجہ.سیٹ_مقصد_Bogus();
    واپس نتیجہ;
}

UObject*
Locale_کلید_Factory::handleCreate(ثابت Locale& /* loc */, 
                   int32_t /*قسم*/, 
                   ثابت ICUService* /* service */, 
                   UError_رمز& /* status */) ثابت {
    واپس خالی_اشاریہ;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ثابت Hashtable*
Locale_کلید_Factory::getSupportedIDs(UError_رمز& /* status */) ثابت {
    واپس خالی_اشاریہ;
}

#اگر_تعریف_شدہ SERVICE_DEBUG
UnicodeString&
Locale_کلید_Factory::debug(UnicodeString& نتیجہ) ثابت
{
    debugClass(نتیجہ);
    نتیجہ.append((UnicodeString)", name: ");
    نتیجہ.append(_name);
    نتیجہ.append((UnicodeString)", coverage: ");
    نتیجہ.append(_coverage);
    واپس نتیجہ;
}

UnicodeString&
Locale_کلید_Factory::debugClass(UnicodeString& نتیجہ) ثابت
{
  واپس نتیجہ.append((UnicodeString)"LocaleKeyFactory");
}
#اگر_ختم

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_کلید_Factory)

U_NAMESPACE_انتہا

/* !UCONFIG_NO_SERVICE */
#اگر_ختم


