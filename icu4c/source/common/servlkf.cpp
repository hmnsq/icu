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

#وصف_ڪريو UNDERSCORE_CHAR ((char16_t)0x005f)
#وصف_ڪريو AT_SIGN_CHAR    ((char16_t)64)
#وصف_ڪريو PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_ڪنجي_Factory::Locale_ڪنجي_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_ڪنجي_Factory::Locale_ڪنجي_Factory(int32_t coverage, ثابت UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_ڪنجي_Factory::~Locale_ڪنجي_Factory() {
}

UObject*
Locale_ڪنجي_Factory::create(ثابت ICUService_ڪنجي& ڪنجي, ثابت ICUService* service, UError_ڪوڊ& حالت) ثابت {
    إذا (handles_ڪنجي(ڪنجي, حالت)) {
        ثابت Locale_ڪنجي& lkey = static_cast<ثابت Locale_ڪنجي&>(ڪنجي);
        int32_t قسم = lkey.قسم();
        Locale loc;
        lkey.currentLocale(loc);

        أرجع handleCreate(loc, قسم, service, حالت);
    }
    أرجع خالي_اشارو;
}

UBool
Locale_ڪنجي_Factory::handles_ڪنجي(ثابت ICUService_ڪنجي& ڪنجي, UError_ڪوڊ& حالت) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(حالت);
    إذا (supported) {
        UnicodeString id;
        ڪنجي.currentID(id);
        أرجع supported->get(id) != خالي_اشارو;
    }
    أرجع خطأ;
}

فراغ
Locale_ڪنجي_Factory::updateVisibleIDs(Hashtable& نتيجو, UError_ڪوڊ& حالت) ثابت {
    ثابت Hashtable* supported = getSupportedIDs(حالت);
    إذا (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ثابت UHashElement* elem = خالي_اشارو;
        int32_t pos = UHASH_پهريون;
        طالما ((elem = supported->ايندڙ_Element(pos)) != خالي_اشارو) {
            ثابت UnicodeString& id = *static_cast<ثابت UnicodeString*>(elem->ڪنجي.اشارو);
            إذا (!visible) {
                نتيجو.remove(id);
            } وإلا {
                نتيجو.put(id, (فراغ*)هذا, حالت); // this is dummy non-void marker used for set semantics
                إذا (U_FAILURE(حالت)) {
                    اكسر;
                }
            }
        }
    }
}

UnicodeString&
Locale_ڪنجي_Factory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& locale, UnicodeString& نتيجو) ثابت {
    إذا ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            أرجع loc.getDisplayName(locale, نتيجو);
        // }
    }
    نتيجو.سيٽ_منزل_Bogus();
    أرجع نتيجو;
}

UObject*
Locale_ڪنجي_Factory::handleCreate(ثابت Locale& /* loc */, 
                   int32_t /*قسم*/, 
                   ثابت ICUService* /* service */, 
                   UError_ڪوڊ& /* status */) ثابت {
    أرجع خالي_اشارو;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ثابت Hashtable*
Locale_ڪنجي_Factory::getSupportedIDs(UError_ڪوڊ& /* status */) ثابت {
    أرجع خالي_اشارو;
}

#جيڪڏهن_وصف_ٿيل SERVICE_DEBUG
UnicodeString&
Locale_ڪنجي_Factory::debug(UnicodeString& نتيجو) ثابت
{
    debugClass(نتيجو);
    نتيجو.append((UnicodeString)", name: ");
    نتيجو.append(_name);
    نتيجو.append((UnicodeString)", coverage: ");
    نتيجو.append(_coverage);
    أرجع نتيجو;
}

UnicodeString&
Locale_ڪنجي_Factory::debugClass(UnicodeString& نتيجو) ثابت
{
  أرجع نتيجو.append((UnicodeString)"LocaleKeyFactory");
}
#جيڪڏهن_پورو

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_ڪنجي_Factory)

U_NAMESPACE_پڇاڙي

/* !UCONFIG_NO_SERVICE */
#جيڪڏهن_پورو


