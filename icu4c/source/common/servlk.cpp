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

#ಯದಿ !UCONFIG_NO_SERVICE

#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "servloc.h"
#ಅನ್ತರ್ಭಾವ "ustrfmt.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_ಕೀಲಿ*
Locale_ಕೀಲಿ::createWithCanonicalFallback(ಧ್ರುವ UnicodeString* primaryID,
                                       ಧ್ರುವ UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    ಪ್ರತ್ಯಾಗಮ Locale_ಕೀಲಿ::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

Locale_ಕೀಲಿ*
Locale_ಕೀಲಿ::createWithCanonicalFallback(ಧ್ರುವ UnicodeString* primaryID,
                                       ಧ್ರುವ UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    ಯದಿ (primaryID == nullptr || U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    ಪ್ರತ್ಯಾಗಮ ನೂತನ Locale_ಕೀಲಿ(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

Locale_ಕೀಲಿ::Locale_ಕೀಲಿ(ಧ್ರುವ UnicodeString& primaryID,
                     ಧ್ರುವ UnicodeString& canonicalPrimaryID,
                     ಧ್ರುವ UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUService_ಕೀಲಿ(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    ಯದಿ (_primaryID.length() != 0) {
        ಯದಿ (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_ಕೀಲಿ::~Locale_ಕೀಲಿ() {}

UnicodeString&
Locale_ಕೀಲಿ::prefix(UnicodeString& result) ಧ್ರುವ {
    ಯದಿ (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

int32_t
Locale_ಕೀಲಿ::kind() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ _kind;
}

UnicodeString&
Locale_ಕೀಲಿ::canonicalID(UnicodeString& result) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ result.append(_primaryID);
}

UnicodeString&
Locale_ಕೀಲಿ::currentID(UnicodeString& result) ಧ್ರುವ {
    ಯದಿ (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString&
Locale_ಕೀಲಿ::currentDescriptor(UnicodeString& result) ಧ್ರುವ {
    ಯದಿ (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } ಅನ್ಯಥಾ {
        result.setToBogus();
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

Locale&
Locale_ಕೀಲಿ::canonicalLocale(Locale& result) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
Locale_ಕೀಲಿ::currentLocale(Locale& result) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
Locale_ಕೀಲಿ::fallback() {
    ಯದಿ (!_currentID.isBogus()) {
        ಪೂರ್ಣಾಙ್ಕ x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        ಯದಿ (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }

        ಯದಿ (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }

        ಯದಿ (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }

        _currentID.setToBogus();
    }

    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

UBool
Locale_ಕೀಲಿ::isFallbackOf(ಧ್ರುವ UnicodeString& id) ಧ್ರುವ {
    UnicodeString temp(id);
    parseSuffix(temp);
    ಪ್ರತ್ಯಾಗಮ temp.indexOf(_primaryID) == 0 &&
        (temp.length() == _primaryID.length() ||
        temp.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_ಕೀಲಿ::debug(UnicodeString& result) ಧ್ರುವ
{
    ICUService_ಕೀಲಿ::debug(result);
    result.append((UnicodeString)" kind: ");
    result.append(_kind);
    result.append((UnicodeString)" primaryID: ");
    result.append(_primaryID);
    result.append((UnicodeString)" fallbackID: ");
    result.append(_fallbackID);
    result.append((UnicodeString)" currentID: ");
    result.append(_currentID);
    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString&
Locale_ಕೀಲಿ::debugClass(UnicodeString& result) ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_ಕೀಲಿ)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


