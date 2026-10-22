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
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_SERVICE

#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "servloc.h"
#ಅನ್ತರ್ಭಾವ "ustrfmt.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UNDERSCORE_CHAR ((char16_t)0x005f)
#ವ್ಯಾಖ್ಯಾನಿಸು AT_SIGN_CHAR    ((char16_t)64)
#ವ್ಯಾಖ್ಯಾನಿಸು PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_ಕೀಲಿ*
Locale_ಕೀಲಿ::createWithCanonicalFallback(ಸ್ಥಿರಾಂಕ UnicodeString* primaryID,
                                       ಸ್ಥಿರಾಂಕ UnicodeString* canonicalFallbackID,
                                       UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಹಿಂತಿರುಗಿ Locale_ಕೀಲಿ::createWithCanonicalFallback(primaryID, canonicalFallbackID, ಬಗೆ_ANY, ಸ್ಥಿತಿ_2);
}

Locale_ಕೀಲಿ*
Locale_ಕೀಲಿ::createWithCanonicalFallback(ಸ್ಥಿರಾಂಕ UnicodeString* primaryID,
                                       ಸ್ಥಿರಾಂಕ UnicodeString* canonicalFallbackID,
                                       int32_t ಬಗೆ,
                                       UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (primaryID == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    ಹಿಂತಿರುಗಿ ಹೊಸ Locale_ಕೀಲಿ(*primaryID, canonicalPrimaryID, canonicalFallbackID, ಬಗೆ);
}

Locale_ಕೀಲಿ::Locale_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UnicodeString& primaryID,
                     ಸ್ಥಿರಾಂಕ UnicodeString& canonicalPrimaryID,
                     ಸ್ಥಿರಾಂಕ UnicodeString* canonicalFallbackID,
                     int32_t ಬಗೆ)
  : ICUService_ಕೀಲಿ(primaryID)
  , ಬಗೆ_2(ಬಗೆ)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.ಗಣ_ಗಮ್ಯ_Bogus();
    ಆದರೆ (_primaryID.length() != 0) {
        ಆದರೆ (canonicalFallbackID != ಶೂನ್ಯಸೂಚಕ && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_ಕೀಲಿ::~Locale_ಕೀಲಿ() {}

UnicodeString&
Locale_ಕೀಲಿ::ಪೂರ್ವಪ್ರತ್ಯಯ(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (ಬಗೆ_2 != ಬಗೆ_ANY) {
        char16_t ಬಫರ್[64];
        uprv_itou(ಬಫರ್, 64, ಬಗೆ_2, 10, 0);
        UnicodeString ತಾತ್ಕಾಲಿಕ(ಬಫರ್);
        ಫಲಿತಾಂಶ.append(ತಾತ್ಕಾಲಿಕ);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

int32_t
Locale_ಕೀಲಿ::ಬಗೆ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಬಗೆ_2;
}

UnicodeString&
Locale_ಕೀಲಿ::canonicalID(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append(_primaryID);
}

UnicodeString&
Locale_ಕೀಲಿ::currentID(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (!_currentID.isBogus()) {
        ಫಲಿತಾಂಶ.append(_currentID);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString&
Locale_ಕೀಲಿ::currentDescriptor(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (!_currentID.isBogus()) {
        ಪೂರ್ವಪ್ರತ್ಯಯ(ಫಲಿತಾಂಶ).append(ಪೂರ್ವಪ್ರತ್ಯಯ_DELIMITER).append(_currentID);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

Locale&
Locale_ಕೀಲಿ::canonicalLocale(Locale& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ LocaleUtility::initLocaleFromName(_primaryID, ಫಲಿತಾಂಶ);
}

Locale&
Locale_ಕೀಲಿ::currentLocale(Locale& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ LocaleUtility::initLocaleFromName(_currentID, ಫಲಿತಾಂಶ);
}

UBool
Locale_ಕೀಲಿ::fallback() {
    ಆದರೆ (!_currentID.isBogus()) {
        ಪೂರ್ಣಾಂಕ x = _currentID.ಕೊನೆಯ_ಸೂಚ್ಯಂಕ_ದ(UNDERSCORE_CHAR);
        ಆದರೆ (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }

        ಆದರೆ (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.ಗಣ_ಗಮ್ಯ_Bogus();
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }

        ಆದರೆ (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }

        _currentID.ಗಣ_ಗಮ್ಯ_Bogus();
    }

    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

UBool
Locale_ಕೀಲಿ::is_Fallback_ದ(ಸ್ಥಿರಾಂಕ UnicodeString& id) ಸ್ಥಿರಾಂಕ {
    UnicodeString ತಾತ್ಕಾಲಿಕ(id);
    parseSuffix(ತಾತ್ಕಾಲಿಕ);
    ಹಿಂತಿರುಗಿ ತಾತ್ಕಾಲಿಕ.ಸೂಚ್ಯಂಕ_ದ(_primaryID) == 0 &&
        (ತಾತ್ಕಾಲಿಕ.length() == _primaryID.length() ||
        ತಾತ್ಕಾಲಿಕ.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SERVICE_DEBUG
UnicodeString&
Locale_ಕೀಲಿ::debug(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ
{
    ICUService_ಕೀಲಿ::debug(ಫಲಿತಾಂಶ);
    ಫಲಿತಾಂಶ.append((UnicodeString)" kind: ");
    ಫಲಿತಾಂಶ.append(ಬಗೆ_2);
    ಫಲಿತಾಂಶ.append((UnicodeString)" primaryID: ");
    ಫಲಿತಾಂಶ.append(_primaryID);
    ಫಲಿತಾಂಶ.append((UnicodeString)" fallbackID: ");
    ಫಲಿತಾಂಶ.append(_fallbackID);
    ಫಲಿತಾಂಶ.append((UnicodeString)" currentID: ");
    ಫಲಿತಾಂಶ.append(_currentID);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString&
Locale_ಕೀಲಿ::debugClass(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append((UnicodeString)"LocaleKey ");
}
#ಆದರೆ_ಅಂತ್ಯ

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_ಕೀಲಿ)

U_NAMESPACE_ಕೊನೆ

/* !UCONFIG_NO_SERVICE */
#ಆದರೆ_ಅಂತ್ಯ


