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
#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_SERVICE

#அந்தர்பாவ "unicode/resbund.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "servloc.h"
#அந்தர்பாவ "ustrfmt.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_சாவி*
Locale_சாவி::createWithCanonicalFallback(மாறிலி UnicodeString* primaryID,
                                       மாறிலி UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    திரும்பு Locale_சாவி::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

Locale_சாவி*
Locale_சாவி::createWithCanonicalFallback(மாறிலி UnicodeString* primaryID,
                                       மாறிலி UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    எனில் (primaryID == nullptr || U_FAILURE(status)) {
        திரும்பு nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    திரும்பு புதிய Locale_சாவி(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

Locale_சாவி::Locale_சாவி(மாறிலி UnicodeString& primaryID,
                     மாறிலி UnicodeString& canonicalPrimaryID,
                     மாறிலி UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUService_சாவி(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    எனில் (_primaryID.length() != 0) {
        எனில் (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_சாவி::~Locale_சாவி() {}

UnicodeString&
Locale_சாவி::prefix(UnicodeString& result) மாறிலி {
    எனில் (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    திரும்பு result;
}

int32_t
Locale_சாவி::kind() மாறிலி {
    திரும்பு _kind;
}

UnicodeString&
Locale_சாவி::canonicalID(UnicodeString& result) மாறிலி {
    திரும்பு result.append(_primaryID);
}

UnicodeString&
Locale_சாவி::currentID(UnicodeString& result) மாறிலி {
    எனில் (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    திரும்பு result;
}

UnicodeString&
Locale_சாவி::currentDescriptor(UnicodeString& result) மாறிலி {
    எனில் (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } இல்லையெனில் {
        result.setToBogus();
    }
    திரும்பு result;
}

Locale&
Locale_சாவி::canonicalLocale(Locale& result) மாறிலி {
    திரும்பு LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
Locale_சாவி::currentLocale(Locale& result) மாறிலி {
    திரும்பு LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
Locale_சாவி::fallback() {
    எனில் (!_currentID.isBogus()) {
        முழு x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        எனில் (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            திரும்பு உண்மை;
        }

        எனில் (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            திரும்பு உண்மை;
        }

        எனில் (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            திரும்பு உண்மை;
        }

        _currentID.setToBogus();
    }

    திரும்பு பொய்;
}

UBool
Locale_சாவி::isFallbackOf(மாறிலி UnicodeString& id) மாறிலி {
    UnicodeString temp(id);
    parseSuffix(temp);
    திரும்பு temp.indexOf(_primaryID) == 0 &&
        (temp.length() == _primaryID.length() ||
        temp.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_சாவி::debug(UnicodeString& result) மாறிலி
{
    ICUService_சாவி::debug(result);
    result.append((UnicodeString)" kind: ");
    result.append(_kind);
    result.append((UnicodeString)" primaryID: ");
    result.append(_primaryID);
    result.append((UnicodeString)" fallbackID: ");
    result.append(_fallbackID);
    result.append((UnicodeString)" currentID: ");
    result.append(_currentID);
    திரும்பு result;
}

UnicodeString&
Locale_சாவி::debugClass(UnicodeString& result) மாறிலி
{
    திரும்பு result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_சாவி)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


