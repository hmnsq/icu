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

#जर !UCONFIG_NO_SERVICE

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

Locale_किल्ली*
Locale_किल्ली::createWithCanonicalFallback(स्थिरांक UnicodeString* primaryID,
                                       स्थिरांक UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    परत Locale_किल्ली::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

Locale_किल्ली*
Locale_किल्ली::createWithCanonicalFallback(स्थिरांक UnicodeString* primaryID,
                                       स्थिरांक UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    जर (primaryID == nullptr || U_FAILURE(status)) {
        परत nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    परत नवीन Locale_किल्ली(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

Locale_किल्ली::Locale_किल्ली(स्थिरांक UnicodeString& primaryID,
                     स्थिरांक UnicodeString& canonicalPrimaryID,
                     स्थिरांक UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUService_किल्ली(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    जर (_primaryID.length() != 0) {
        जर (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_किल्ली::~Locale_किल्ली() {}

UnicodeString&
Locale_किल्ली::prefix(UnicodeString& result) स्थिरांक {
    जर (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    परत result;
}

int32_t
Locale_किल्ली::kind() स्थिरांक {
    परत _kind;
}

UnicodeString&
Locale_किल्ली::canonicalID(UnicodeString& result) स्थिरांक {
    परत result.append(_primaryID);
}

UnicodeString&
Locale_किल्ली::currentID(UnicodeString& result) स्थिरांक {
    जर (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    परत result;
}

UnicodeString&
Locale_किल्ली::currentDescriptor(UnicodeString& result) स्थिरांक {
    जर (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } अन्यथा {
        result.setToBogus();
    }
    परत result;
}

Locale&
Locale_किल्ली::canonicalLocale(Locale& result) स्थिरांक {
    परत LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
Locale_किल्ली::currentLocale(Locale& result) स्थिरांक {
    परत LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
Locale_किल्ली::fallback() {
    जर (!_currentID.isBogus()) {
        पूर्णांक x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        जर (x != -1) {
            _currentID.हटाओ(x); // truncate current or fallback, whichever we're pointing to
            परत सत्य;
        }

        जर (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            परत सत्य;
        }

        जर (_currentID.length() > 0) {
            _currentID.हटाओ(0); // completely truncate
            परत सत्य;
        }

        _currentID.setToBogus();
    }

    परत असत्य;
}

UBool
Locale_किल्ली::isFallbackOf(स्थिरांक UnicodeString& id) स्थिरांक {
    UnicodeString temp(id);
    parseSuffix(temp);
    परत temp.indexOf(_primaryID) == 0 &&
        (temp.length() == _primaryID.length() ||
        temp.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_किल्ली::debug(UnicodeString& result) स्थिरांक
{
    ICUService_किल्ली::debug(result);
    result.append((UnicodeString)" kind: ");
    result.append(_kind);
    result.append((UnicodeString)" primaryID: ");
    result.append(_primaryID);
    result.append((UnicodeString)" fallbackID: ");
    result.append(_fallbackID);
    result.append((UnicodeString)" currentID: ");
    result.append(_currentID);
    परत result;
}

UnicodeString&
Locale_किल्ली::debugClass(UnicodeString& result) स्थिरांक
{
    परत result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_किल्ली)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


