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
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNDERSCORE_CHAR ((char16_t)0x005f)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AT_SIGN_CHAR    ((char16_t)64)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_ਕੁੰਜੀ*
Locale_ਕੁੰਜੀ::createWithCanonicalFallback(ਅਚਲ UnicodeString* primaryID,
                                       ਅਚਲ UnicodeString* canonicalFallbackID,
                                       UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਵਾਪਸ Locale_ਕੁੰਜੀ::createWithCanonicalFallback(primaryID, canonicalFallbackID, ਕਿਸਮ_ANY, ਸਥਿਤੀ);
}

Locale_ਕੁੰਜੀ*
Locale_ਕੁੰਜੀ::createWithCanonicalFallback(ਅਚਲ UnicodeString* primaryID,
                                       ਅਚਲ UnicodeString* canonicalFallbackID,
                                       int32_t ਕਿਸਮ,
                                       UError_ਕੋਡ& ਸਥਿਤੀ)
{
    ਜੇ (primaryID == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    ਵਾਪਸ ਨਵਾਂ Locale_ਕੁੰਜੀ(*primaryID, canonicalPrimaryID, canonicalFallbackID, ਕਿਸਮ);
}

Locale_ਕੁੰਜੀ::Locale_ਕੁੰਜੀ(ਅਚਲ UnicodeString& primaryID,
                     ਅਚਲ UnicodeString& canonicalPrimaryID,
                     ਅਚਲ UnicodeString* canonicalFallbackID,
                     int32_t ਕਿਸਮ)
  : ICUService_ਕੁੰਜੀ(primaryID)
  , ਕਿਸਮ_2(ਕਿਸਮ)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਜੇ (_primaryID.length() != 0) {
        ਜੇ (canonicalFallbackID != ਖਾਲੀ_ਸੂਚਕ && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_ਕੁੰਜੀ::~Locale_ਕੁੰਜੀ() {}

UnicodeString&
Locale_ਕੁੰਜੀ::ਅਗੇਤਰ(UnicodeString& ਨਤੀਜਾ) ਅਚਲ {
    ਜੇ (ਕਿਸਮ_2 != ਕਿਸਮ_ANY) {
        char16_t ਬਫਰ[64];
        uprv_itou(ਬਫਰ, 64, ਕਿਸਮ_2, 10, 0);
        UnicodeString ਆਰਜ਼ੀ(ਬਫਰ);
        ਨਤੀਜਾ.append(ਆਰਜ਼ੀ);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

int32_t
Locale_ਕੁੰਜੀ::ਕਿਸਮ() ਅਚਲ {
    ਵਾਪਸ ਕਿਸਮ_2;
}

UnicodeString&
Locale_ਕੁੰਜੀ::canonicalID(UnicodeString& ਨਤੀਜਾ) ਅਚਲ {
    ਵਾਪਸ ਨਤੀਜਾ.append(_primaryID);
}

UnicodeString&
Locale_ਕੁੰਜੀ::currentID(UnicodeString& ਨਤੀਜਾ) ਅਚਲ {
    ਜੇ (!_currentID.isBogus()) {
        ਨਤੀਜਾ.append(_currentID);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString&
Locale_ਕੁੰਜੀ::currentDescriptor(UnicodeString& ਨਤੀਜਾ) ਅਚਲ {
    ਜੇ (!_currentID.isBogus()) {
        ਅਗੇਤਰ(ਨਤੀਜਾ).append(ਅਗੇਤਰ_DELIMITER).append(_currentID);
    } ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

Locale&
Locale_ਕੁੰਜੀ::canonicalLocale(Locale& ਨਤੀਜਾ) ਅਚਲ {
    ਵਾਪਸ LocaleUtility::initLocaleFromName(_primaryID, ਨਤੀਜਾ);
}

Locale&
Locale_ਕੁੰਜੀ::currentLocale(Locale& ਨਤੀਜਾ) ਅਚਲ {
    ਵਾਪਸ LocaleUtility::initLocaleFromName(_currentID, ਨਤੀਜਾ);
}

UBool
Locale_ਕੁੰਜੀ::fallback() {
    ਜੇ (!_currentID.isBogus()) {
        ਪੂਰਨ x = _currentID.ਆਖਰੀ_ਸੂਚਕਾਂਕ_ਦਾ(UNDERSCORE_CHAR);
        ਜੇ (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            ਵਾਪਸ ਸੱਚ;
        }

        ਜੇ (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
            ਵਾਪਸ ਸੱਚ;
        }

        ਜੇ (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            ਵਾਪਸ ਸੱਚ;
        }

        _currentID.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    }

    ਵਾਪਸ ਝੂਠ;
}

UBool
Locale_ਕੁੰਜੀ::is_Fallback_ਦਾ(ਅਚਲ UnicodeString& id) ਅਚਲ {
    UnicodeString ਆਰਜ਼ੀ(id);
    parseSuffix(ਆਰਜ਼ੀ);
    ਵਾਪਸ ਆਰਜ਼ੀ.ਸੂਚਕਾਂਕ_ਦਾ(_primaryID) == 0 &&
        (ਆਰਜ਼ੀ.length() == _primaryID.length() ||
        ਆਰਜ਼ੀ.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SERVICE_DEBUG
UnicodeString&
Locale_ਕੁੰਜੀ::debug(UnicodeString& ਨਤੀਜਾ) ਅਚਲ
{
    ICUService_ਕੁੰਜੀ::debug(ਨਤੀਜਾ);
    ਨਤੀਜਾ.append((UnicodeString)" kind: ");
    ਨਤੀਜਾ.append(ਕਿਸਮ_2);
    ਨਤੀਜਾ.append((UnicodeString)" primaryID: ");
    ਨਤੀਜਾ.append(_primaryID);
    ਨਤੀਜਾ.append((UnicodeString)" fallbackID: ");
    ਨਤੀਜਾ.append(_fallbackID);
    ਨਤੀਜਾ.append((UnicodeString)" currentID: ");
    ਨਤੀਜਾ.append(_currentID);
    ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString&
Locale_ਕੁੰਜੀ::debugClass(UnicodeString& ਨਤੀਜਾ) ਅਚਲ
{
    ਵਾਪਸ ਨਤੀਜਾ.append((UnicodeString)"LocaleKey ");
}
#ਜੇ_ਅੰਤ

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_ਕੁੰਜੀ)

U_NAMESPACE_ਸਿਰਾ

/* !UCONFIG_NO_SERVICE */
#ਜੇ_ਅੰਤ


