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
#अन्तर्भाव "unicode/utypes.h"

#सचे !UCONFIG_NO_SERVICE

#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "servloc.h"
#अन्तर्भाव "ustrfmt.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_कुञ्चिका*
Locale_कुञ्चिका::createWithCanonicalFallback(ध्रुव UnicodeString* primaryID,
                                       ध्रुव UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    पटिनिवत्त Locale_कुञ्चिका::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

Locale_कुञ्चिका*
Locale_कुञ्चिका::createWithCanonicalFallback(ध्रुव UnicodeString* primaryID,
                                       ध्रुव UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    सचे (primaryID == nullptr || U_FAILURE(status)) {
        पटिनिवत्त nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    पटिनिवत्त नूतन Locale_कुञ्चिका(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

Locale_कुञ्चिका::Locale_कुञ्चिका(ध्रुव UnicodeString& primaryID,
                     ध्रुव UnicodeString& canonicalPrimaryID,
                     ध्रुव UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUService_कुञ्चिका(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    सचे (_primaryID.length() != 0) {
        सचे (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_कुञ्चिका::~Locale_कुञ्चिका() {}

UnicodeString&
Locale_कुञ्चिका::prefix(UnicodeString& result) ध्रुव {
    सचे (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    पटिनिवत्त result;
}

int32_t
Locale_कुञ्चिका::kind() ध्रुव {
    पटिनिवत्त _kind;
}

UnicodeString&
Locale_कुञ्चिका::canonicalID(UnicodeString& result) ध्रुव {
    पटिनिवत्त result.append(_primaryID);
}

UnicodeString&
Locale_कुञ्चिका::currentID(UnicodeString& result) ध्रुव {
    सचे (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    पटिनिवत्त result;
}

UnicodeString&
Locale_कुञ्चिका::currentDescriptor(UnicodeString& result) ध्रुव {
    सचे (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } अञ्ञथा {
        result.setToBogus();
    }
    पटिनिवत्त result;
}

Locale&
Locale_कुञ्चिका::canonicalLocale(Locale& result) ध्रुव {
    पटिनिवत्त LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
Locale_कुञ्चिका::currentLocale(Locale& result) ध्रुव {
    पटिनिवत्त LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
Locale_कुञ्चिका::fallback() {
    सचे (!_currentID.isBogus()) {
        पूर्णाङ्क x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        सचे (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            पटिनिवत्त सच्च;
        }

        सचे (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            पटिनिवत्त सच्च;
        }

        सचे (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            पटिनिवत्त सच्च;
        }

        _currentID.setToBogus();
    }

    पटिनिवत्त मिच्छा;
}

UBool
Locale_कुञ्चिका::isFallbackOf(ध्रुव UnicodeString& id) ध्रुव {
    UnicodeString temp(id);
    parseSuffix(temp);
    पटिनिवत्त temp.indexOf(_primaryID) == 0 &&
        (temp.length() == _primaryID.length() ||
        temp.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_कुञ्चिका::debug(UnicodeString& result) ध्रुव
{
    ICUService_कुञ्चिका::debug(result);
    result.append((UnicodeString)" kind: ");
    result.append(_kind);
    result.append((UnicodeString)" primaryID: ");
    result.append(_primaryID);
    result.append((UnicodeString)" fallbackID: ");
    result.append(_fallbackID);
    result.append((UnicodeString)" currentID: ");
    result.append(_currentID);
    पटिनिवत्त result;
}

UnicodeString&
Locale_कुञ्चिका::debugClass(UnicodeString& result) ध्रुव
{
    पटिनिवत्त result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_कुञ्चिका)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


