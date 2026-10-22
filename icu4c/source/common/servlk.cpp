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
#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_SERVICE

#അന്തര്ഭാവ "unicode/resbund.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "servloc.h"
#അന്തര്ഭാവ "ustrfmt.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_താക്കോൽ*
Locale_താക്കോൽ::createWithCanonicalFallback(സ്ഥിരം UnicodeString* primaryID,
                                       സ്ഥിരം UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    തിരികെ Locale_താക്കോൽ::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

Locale_താക്കോൽ*
Locale_താക്കോൽ::createWithCanonicalFallback(സ്ഥിരം UnicodeString* primaryID,
                                       സ്ഥിരം UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    എങ്കിൽ (primaryID == nullptr || U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    തിരികെ പുതിയ Locale_താക്കോൽ(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

Locale_താക്കോൽ::Locale_താക്കോൽ(സ്ഥിരം UnicodeString& primaryID,
                     സ്ഥിരം UnicodeString& canonicalPrimaryID,
                     സ്ഥിരം UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUService_താക്കോൽ(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    എങ്കിൽ (_primaryID.length() != 0) {
        എങ്കിൽ (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_താക്കോൽ::~Locale_താക്കോൽ() {}

UnicodeString&
Locale_താക്കോൽ::prefix(UnicodeString& result) സ്ഥിരം {
    എങ്കിൽ (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    തിരികെ result;
}

int32_t
Locale_താക്കോൽ::kind() സ്ഥിരം {
    തിരികെ _kind;
}

UnicodeString&
Locale_താക്കോൽ::canonicalID(UnicodeString& result) സ്ഥിരം {
    തിരികെ result.append(_primaryID);
}

UnicodeString&
Locale_താക്കോൽ::currentID(UnicodeString& result) സ്ഥിരം {
    എങ്കിൽ (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    തിരികെ result;
}

UnicodeString&
Locale_താക്കോൽ::currentDescriptor(UnicodeString& result) സ്ഥിരം {
    എങ്കിൽ (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } അല്ലെങ്കിൽ {
        result.setToBogus();
    }
    തിരികെ result;
}

Locale&
Locale_താക്കോൽ::canonicalLocale(Locale& result) സ്ഥിരം {
    തിരികെ LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
Locale_താക്കോൽ::currentLocale(Locale& result) സ്ഥിരം {
    തിരികെ LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
Locale_താക്കോൽ::fallback() {
    എങ്കിൽ (!_currentID.isBogus()) {
        പൂർണ്ണസംഖ്യ x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        എങ്കിൽ (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            തിരികെ സത്യം;
        }

        എങ്കിൽ (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            തിരികെ സത്യം;
        }

        എങ്കിൽ (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            തിരികെ സത്യം;
        }

        _currentID.setToBogus();
    }

    തിരികെ അസത്യം;
}

UBool
Locale_താക്കോൽ::isFallbackOf(സ്ഥിരം UnicodeString& id) സ്ഥിരം {
    UnicodeString temp(id);
    parseSuffix(temp);
    തിരികെ temp.indexOf(_primaryID) == 0 &&
        (temp.length() == _primaryID.length() ||
        temp.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_താക്കോൽ::debug(UnicodeString& result) സ്ഥിരം
{
    ICUService_താക്കോൽ::debug(result);
    result.append((UnicodeString)" kind: ");
    result.append(_kind);
    result.append((UnicodeString)" primaryID: ");
    result.append(_primaryID);
    result.append((UnicodeString)" fallbackID: ");
    result.append(_fallbackID);
    result.append((UnicodeString)" currentID: ");
    result.append(_currentID);
    തിരികെ result;
}

UnicodeString&
Locale_താക്കോൽ::debugClass(UnicodeString& result) സ്ഥിരം
{
    തിരികെ result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_താക്കോൽ)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


