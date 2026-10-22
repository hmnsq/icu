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
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_SERVICE

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/resbund.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "servloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustrfmt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNDERSCORE_CHAR ((char16_t)0x005f)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AT_SIGN_CHAR    ((char16_t)64)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_ਕੁੰਜੀ_Factory::Locale_ਕੁੰਜੀ_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_ਕੁੰਜੀ_Factory::Locale_ਕੁੰਜੀ_Factory(int32_t coverage, ਅਚਲ UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_ਕੁੰਜੀ_Factory::~Locale_ਕੁੰਜੀ_Factory() {
}

UObject*
Locale_ਕੁੰਜੀ_Factory::create(ਅਚਲ ICUService_ਕੁੰਜੀ& ਕੁੰਜੀ, ਅਚਲ ICUService* service, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (handles_ਕੁੰਜੀ(ਕੁੰਜੀ, ਸਥਿਤੀ)) {
        ਅਚਲ Locale_ਕੁੰਜੀ& lkey = static_cast<ਅਚਲ Locale_ਕੁੰਜੀ&>(ਕੁੰਜੀ);
        int32_t ਕਿਸਮ = lkey.ਕਿਸਮ();
        Locale loc;
        lkey.currentLocale(loc);

        ਵਾਪਸ handleCreate(loc, ਕਿਸਮ, service, ਸਥਿਤੀ);
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

UBool
Locale_ਕੁੰਜੀ_Factory::handles_ਕੁੰਜੀ(ਅਚਲ ICUService_ਕੁੰਜੀ& ਕੁੰਜੀ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਅਚਲ Hashtable* supported = getSupportedIDs(ਸਥਿਤੀ);
    ਜੇ (supported) {
        UnicodeString id;
        ਕੁੰਜੀ.currentID(id);
        ਵਾਪਸ supported->get(id) != ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ
Locale_ਕੁੰਜੀ_Factory::updateVisibleIDs(Hashtable& ਨਤੀਜਾ, UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਅਚਲ Hashtable* supported = getSupportedIDs(ਸਥਿਤੀ);
    ਜੇ (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        ਅਚਲ UHashElement* elem = ਖਾਲੀ_ਸੂਚਕ;
        int32_t pos = UHASH_ਪਹਿਲਾ;
        ਜਦੋਂ ((elem = supported->ਅਗਲਾ_Element(pos)) != ਖਾਲੀ_ਸੂਚਕ) {
            ਅਚਲ UnicodeString& id = *static_cast<ਅਚਲ UnicodeString*>(elem->ਕੁੰਜੀ.ਸੂਚਕ);
            ਜੇ (!visible) {
                ਨਤੀਜਾ.remove(id);
            } ਨਹੀਂਤਾਂ {
                ਨਤੀਜਾ.put(id, (ਖਾਲੀ*)ਇਹ, ਸਥਿਤੀ); // this is dummy non-void marker used for set semantics
                ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                    ਰੋਕੋ;
                }
            }
        }
    }
}

UnicodeString&
Locale_ਕੁੰਜੀ_Factory::getDisplayName(ਅਚਲ UnicodeString& id, ਅਚਲ Locale& locale, UnicodeString& ਨਤੀਜਾ) ਅਚਲ {
    ਜੇ ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            ਵਾਪਸ loc.getDisplayName(locale, ਨਤੀਜਾ);
        // }
    }
    ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਵਾਪਸ ਨਤੀਜਾ;
}

UObject*
Locale_ਕੁੰਜੀ_Factory::handleCreate(ਅਚਲ Locale& /* loc */, 
                   int32_t /*ਕਿਸਮ*/, 
                   ਅਚਲ ICUService* /* service */, 
                   UError_ਕੋਡ& /* status */) ਅਚਲ {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

ਅਚਲ Hashtable*
Locale_ਕੁੰਜੀ_Factory::getSupportedIDs(UError_ਕੋਡ& /* status */) ਅਚਲ {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SERVICE_DEBUG
UnicodeString&
Locale_ਕੁੰਜੀ_Factory::debug(UnicodeString& ਨਤੀਜਾ) ਅਚਲ
{
    debugClass(ਨਤੀਜਾ);
    ਨਤੀਜਾ.append((UnicodeString)", name: ");
    ਨਤੀਜਾ.append(_name);
    ਨਤੀਜਾ.append((UnicodeString)", coverage: ");
    ਨਤੀਜਾ.append(_coverage);
    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString&
Locale_ਕੁੰਜੀ_Factory::debugClass(UnicodeString& ਨਤੀਜਾ) ਅਚਲ
{
  ਵਾਪਸ ਨਤੀਜਾ.append((UnicodeString)"LocaleKeyFactory");
}
#ਜੇ_ਅੰਤ

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_ਕੁੰਜੀ_Factory)

U_NAMESPACE_ਸਿਰਾ

/* !UCONFIG_NO_SERVICE */
#ਜੇ_ਅੰਤ


