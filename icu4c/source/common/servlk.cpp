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

#यदि !UCONFIG_NO_SERVICE

#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "servloc.h"
#अन्तर्भाव "ustrfmt.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "uassert.h"

#परिभाषय UNDERSCORE_CHAR ((char16_t)0x005f)
#परिभाषय AT_SIGN_CHAR    ((char16_t)64)
#परिभाषय PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_कुञ्चिका*
Locale_कुञ्चिका::createWithCanonicalFallback(ध्रुव UnicodeString* primaryID,
                                       ध्रुव UnicodeString* canonicalFallbackID,
                                       UError_संहिता& स्थितिः)
{
    प्रत्यागम Locale_कुञ्चिका::createWithCanonicalFallback(primaryID, canonicalFallbackID, विधा_ANY, स्थितिः);
}

Locale_कुञ्चिका*
Locale_कुञ्चिका::createWithCanonicalFallback(ध्रुव UnicodeString* primaryID,
                                       ध्रुव UnicodeString* canonicalFallbackID,
                                       int32_t विधा,
                                       UError_संहिता& स्थितिः)
{
    यदि (primaryID == शून्यसूचकः || U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    प्रत्यागम नूतन Locale_कुञ्चिका(*primaryID, canonicalPrimaryID, canonicalFallbackID, विधा);
}

Locale_कुञ्चिका::Locale_कुञ्चिका(ध्रुव UnicodeString& primaryID,
                     ध्रुव UnicodeString& canonicalPrimaryID,
                     ध्रुव UnicodeString* canonicalFallbackID,
                     int32_t विधा)
  : ICUService_कुञ्चिका(primaryID)
  , विधा_2(विधा)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.समुच्चयः_गन्तव्यम्_Bogus();
    यदि (_primaryID.length() != 0) {
        यदि (canonicalFallbackID != शून्यसूचकः && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_कुञ्चिका::~Locale_कुञ्चिका() {}

UnicodeString&
Locale_कुञ्चिका::उपसर्गः(UnicodeString& परिणाम) ध्रुव {
    यदि (विधा_2 != विधा_ANY) {
        char16_t बफर[64];
        uprv_itou(बफर, 64, विधा_2, 10, 0);
        UnicodeString अस्थायी(बफर);
        परिणाम.append(अस्थायी);
    }
    प्रत्यागम परिणाम;
}

int32_t
Locale_कुञ्चिका::विधा() ध्रुव {
    प्रत्यागम विधा_2;
}

UnicodeString&
Locale_कुञ्चिका::canonicalID(UnicodeString& परिणाम) ध्रुव {
    प्रत्यागम परिणाम.append(_primaryID);
}

UnicodeString&
Locale_कुञ्चिका::currentID(UnicodeString& परिणाम) ध्रुव {
    यदि (!_currentID.isBogus()) {
        परिणाम.append(_currentID);
    }
    प्रत्यागम परिणाम;
}

UnicodeString&
Locale_कुञ्चिका::currentDescriptor(UnicodeString& परिणाम) ध्रुव {
    यदि (!_currentID.isBogus()) {
        उपसर्गः(परिणाम).append(उपसर्गः_DELIMITER).append(_currentID);
    } अन्यथा {
        परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
    }
    प्रत्यागम परिणाम;
}

Locale&
Locale_कुञ्चिका::canonicalLocale(Locale& परिणाम) ध्रुव {
    प्रत्यागम LocaleUtility::initLocaleFromName(_primaryID, परिणाम);
}

Locale&
Locale_कुञ्चिका::currentLocale(Locale& परिणाम) ध्रुव {
    प्रत्यागम LocaleUtility::initLocaleFromName(_currentID, परिणाम);
}

UBool
Locale_कुञ्चिका::fallback() {
    यदि (!_currentID.isBogus()) {
        पूर्णाङ्क x = _currentID.चरमः_सूचकांक_सम्बन्धि(UNDERSCORE_CHAR);
        यदि (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            प्रत्यागम सत्य;
        }

        यदि (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.समुच्चयः_गन्तव्यम्_Bogus();
            प्रत्यागम सत्य;
        }

        यदि (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            प्रत्यागम सत्य;
        }

        _currentID.समुच्चयः_गन्तव्यम्_Bogus();
    }

    प्रत्यागम असत्य;
}

UBool
Locale_कुञ्चिका::is_Fallback_सम्बन्धि(ध्रुव UnicodeString& id) ध्रुव {
    UnicodeString अस्थायी(id);
    parseSuffix(अस्थायी);
    प्रत्यागम अस्थायी.सूचकांक_सम्बन्धि(_primaryID) == 0 &&
        (अस्थायी.length() == _primaryID.length() ||
        अस्थायी.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#यदि_परिभाषितम् SERVICE_DEBUG
UnicodeString&
Locale_कुञ्चिका::debug(UnicodeString& परिणाम) ध्रुव
{
    ICUService_कुञ्चिका::debug(परिणाम);
    परिणाम.append((UnicodeString)" kind: ");
    परिणाम.append(विधा_2);
    परिणाम.append((UnicodeString)" primaryID: ");
    परिणाम.append(_primaryID);
    परिणाम.append((UnicodeString)" fallbackID: ");
    परिणाम.append(_fallbackID);
    परिणाम.append((UnicodeString)" currentID: ");
    परिणाम.append(_currentID);
    प्रत्यागम परिणाम;
}

UnicodeString&
Locale_कुञ्चिका::debugClass(UnicodeString& परिणाम) ध्रुव
{
    प्रत्यागम परिणाम.append((UnicodeString)"LocaleKey ");
}
#यदि_अन्तः

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_कुञ्चिका)

U_NAMESPACE_अन्तः

/* !UCONFIG_NO_SERVICE */
#यदि_अन्तः


