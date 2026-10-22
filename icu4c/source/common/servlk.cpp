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

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_साँचो*
Locale_साँचो::createWithCanonicalFallback(अचर UnicodeString* primaryID,
                                       अचर UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    फर्काऊ Locale_साँचो::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

Locale_साँचो*
Locale_साँचो::createWithCanonicalFallback(अचर UnicodeString* primaryID,
                                       अचर UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    यदि (primaryID == nullptr || U_FAILURE(status)) {
        फर्काऊ nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    फर्काऊ नूतन Locale_साँचो(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

Locale_साँचो::Locale_साँचो(अचर UnicodeString& primaryID,
                     अचर UnicodeString& canonicalPrimaryID,
                     अचर UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUService_साँचो(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    यदि (_primaryID.length() != 0) {
        यदि (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_साँचो::~Locale_साँचो() {}

UnicodeString&
Locale_साँचो::prefix(UnicodeString& result) अचर {
    यदि (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    फर्काऊ result;
}

int32_t
Locale_साँचो::kind() अचर {
    फर्काऊ _kind;
}

UnicodeString&
Locale_साँचो::canonicalID(UnicodeString& result) अचर {
    फर्काऊ result.append(_primaryID);
}

UnicodeString&
Locale_साँचो::currentID(UnicodeString& result) अचर {
    यदि (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    फर्काऊ result;
}

UnicodeString&
Locale_साँचो::currentDescriptor(UnicodeString& result) अचर {
    यदि (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } अन्यथा {
        result.setToBogus();
    }
    फर्काऊ result;
}

Locale&
Locale_साँचो::canonicalLocale(Locale& result) अचर {
    फर्काऊ LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
Locale_साँचो::currentLocale(Locale& result) अचर {
    फर्काऊ LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
Locale_साँचो::fallback() {
    यदि (!_currentID.isBogus()) {
        पूर्णाङ्क x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        यदि (x != -1) {
            _currentID.हटाओ(x); // truncate current or fallback, whichever we're pointing to
            फर्काऊ सत्य;
        }

        यदि (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            फर्काऊ सत्य;
        }

        यदि (_currentID.length() > 0) {
            _currentID.हटाओ(0); // completely truncate
            फर्काऊ सत्य;
        }

        _currentID.setToBogus();
    }

    फर्काऊ असत्य;
}

UBool
Locale_साँचो::isFallbackOf(अचर UnicodeString& id) अचर {
    UnicodeString temp(id);
    parseSuffix(temp);
    फर्काऊ temp.indexOf(_primaryID) == 0 &&
        (temp.length() == _primaryID.length() ||
        temp.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_साँचो::debug(UnicodeString& result) अचर
{
    ICUService_साँचो::debug(result);
    result.append((UnicodeString)" kind: ");
    result.append(_kind);
    result.append((UnicodeString)" primaryID: ");
    result.append(_primaryID);
    result.append((UnicodeString)" fallbackID: ");
    result.append(_fallbackID);
    result.append((UnicodeString)" currentID: ");
    result.append(_currentID);
    फर्काऊ result;
}

UnicodeString&
Locale_साँचो::debugClass(UnicodeString& result) अचर
{
    फर्काऊ result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_साँचो)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


