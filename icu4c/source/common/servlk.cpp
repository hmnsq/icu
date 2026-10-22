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
#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_SERVICE

#અન્તર્ભાવ "unicode/resbund.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "servloc.h"
#અન્તર્ભાવ "ustrfmt.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_ચાવી*
Locale_ચાવી::createWithCanonicalFallback(અચલ UnicodeString* primaryID,
                                       અચલ UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    પાછા Locale_ચાવી::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

Locale_ચાવી*
Locale_ચાવી::createWithCanonicalFallback(અચલ UnicodeString* primaryID,
                                       અચલ UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    જો (primaryID == nullptr || U_FAILURE(status)) {
        પાછા nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    પાછા નવું Locale_ચાવી(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

Locale_ચાવી::Locale_ચાવી(અચલ UnicodeString& primaryID,
                     અચલ UnicodeString& canonicalPrimaryID,
                     અચલ UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUService_ચાવી(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    જો (_primaryID.length() != 0) {
        જો (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_ચાવી::~Locale_ચાવી() {}

UnicodeString&
Locale_ચાવી::prefix(UnicodeString& result) અચલ {
    જો (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    પાછા result;
}

int32_t
Locale_ચાવી::kind() અચલ {
    પાછા _kind;
}

UnicodeString&
Locale_ચાવી::canonicalID(UnicodeString& result) અચલ {
    પાછા result.append(_primaryID);
}

UnicodeString&
Locale_ચાવી::currentID(UnicodeString& result) અચલ {
    જો (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    પાછા result;
}

UnicodeString&
Locale_ચાવી::currentDescriptor(UnicodeString& result) અચલ {
    જો (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } નહીંતર {
        result.setToBogus();
    }
    પાછા result;
}

Locale&
Locale_ચાવી::canonicalLocale(Locale& result) અચલ {
    પાછા LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
Locale_ચાવી::currentLocale(Locale& result) અચલ {
    પાછા LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
Locale_ચાવી::fallback() {
    જો (!_currentID.isBogus()) {
        પૂર્ણાંક x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        જો (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            પાછા સાચું;
        }

        જો (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            પાછા સાચું;
        }

        જો (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            પાછા સાચું;
        }

        _currentID.setToBogus();
    }

    પાછા ખોટું;
}

UBool
Locale_ચાવી::isFallbackOf(અચલ UnicodeString& id) અચલ {
    UnicodeString temp(id);
    parseSuffix(temp);
    પાછા temp.indexOf(_primaryID) == 0 &&
        (temp.length() == _primaryID.length() ||
        temp.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_ચાવી::debug(UnicodeString& result) અચલ
{
    ICUService_ચાવી::debug(result);
    result.append((UnicodeString)" kind: ");
    result.append(_kind);
    result.append((UnicodeString)" primaryID: ");
    result.append(_primaryID);
    result.append((UnicodeString)" fallbackID: ");
    result.append(_fallbackID);
    result.append((UnicodeString)" currentID: ");
    result.append(_currentID);
    પાછા result;
}

UnicodeString&
Locale_ચાવી::debugClass(UnicodeString& result) અચલ
{
    પાછા result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_ચાવી)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


