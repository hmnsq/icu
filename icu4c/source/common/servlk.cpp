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
#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_SERVICE

#समावेश "unicode/resbund.h"
#समावेश "uresimp.h"
#समावेश "cmemory.h"
#समावेश "servloc.h"
#समावेश "ustrfmt.h"
#समावेश "uhash.h"
#समावेश "charstr.h"
#समावेश "uassert.h"

#घोषणा UNDERSCORE_CHAR ((char16_t)0x005f)
#घोषणा AT_SIGN_CHAR    ((char16_t)64)
#घोषणा PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_कुंजी*
Locale_कुंजी::createWithCanonicalFallback(स्थिर UnicodeString* primaryID,
                                       स्थिर UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    वापस Locale_कुंजी::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

Locale_कुंजी*
Locale_कुंजी::createWithCanonicalFallback(स्थिर UnicodeString* primaryID,
                                       स्थिर UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    अगर (primaryID == nullptr || U_FAILURE(status)) {
        वापस nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    वापस नया Locale_कुंजी(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

Locale_कुंजी::Locale_कुंजी(स्थिर UnicodeString& primaryID,
                     स्थिर UnicodeString& canonicalPrimaryID,
                     स्थिर UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUService_कुंजी(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    अगर (_primaryID.length() != 0) {
        अगर (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_कुंजी::~Locale_कुंजी() {}

UnicodeString&
Locale_कुंजी::prefix(UnicodeString& परिणाम) स्थिर {
    अगर (_kind != KIND_ANY) {
        char16_t बफर[64];
        uprv_itou(बफर, 64, _kind, 10, 0);
        UnicodeString अस्थायी(बफर);
        परिणाम.append(अस्थायी);
    }
    वापस परिणाम;
}

int32_t
Locale_कुंजी::kind() स्थिर {
    वापस _kind;
}

UnicodeString&
Locale_कुंजी::canonicalID(UnicodeString& परिणाम) स्थिर {
    वापस परिणाम.append(_primaryID);
}

UnicodeString&
Locale_कुंजी::currentID(UnicodeString& परिणाम) स्थिर {
    अगर (!_currentID.isBogus()) {
        परिणाम.append(_currentID);
    }
    वापस परिणाम;
}

UnicodeString&
Locale_कुंजी::currentDescriptor(UnicodeString& परिणाम) स्थिर {
    अगर (!_currentID.isBogus()) {
        prefix(परिणाम).append(PREFIX_DELIMITER).append(_currentID);
    } अन्यथा {
        परिणाम.setToBogus();
    }
    वापस परिणाम;
}

Locale&
Locale_कुंजी::canonicalLocale(Locale& परिणाम) स्थिर {
    वापस LocaleUtility::initLocaleFromName(_primaryID, परिणाम);
}

Locale&
Locale_कुंजी::currentLocale(Locale& परिणाम) स्थिर {
    वापस LocaleUtility::initLocaleFromName(_currentID, परिणाम);
}

UBool
Locale_कुंजी::fallback() {
    अगर (!_currentID.isBogus()) {
        पूर्णांक x = _currentID.last_सूचकांक_Of(UNDERSCORE_CHAR);
        अगर (x != -1) {
            _currentID.हटाओ(x); // truncate current or fallback, whichever we're pointing to
            वापस true;
        }

        अगर (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            वापस true;
        }

        अगर (_currentID.length() > 0) {
            _currentID.हटाओ(0); // completely truncate
            वापस true;
        }

        _currentID.setToBogus();
    }

    वापस false;
}

UBool
Locale_कुंजी::isFallbackOf(स्थिर UnicodeString& id) स्थिर {
    UnicodeString अस्थायी(id);
    parseSuffix(अस्थायी);
    वापस अस्थायी.सूचकांक_Of(_primaryID) == 0 &&
        (अस्थायी.length() == _primaryID.length() ||
        अस्थायी.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#अगर_घोषित SERVICE_DEBUG
UnicodeString&
Locale_कुंजी::debug(UnicodeString& परिणाम) स्थिर
{
    ICUService_कुंजी::debug(परिणाम);
    परिणाम.append((UnicodeString)" kind: ");
    परिणाम.append(_kind);
    परिणाम.append((UnicodeString)" primaryID: ");
    परिणाम.append(_primaryID);
    परिणाम.append((UnicodeString)" fallbackID: ");
    परिणाम.append(_fallbackID);
    परिणाम.append((UnicodeString)" currentID: ");
    परिणाम.append(_currentID);
    वापस परिणाम;
}

UnicodeString&
Locale_कुंजी::debugClass(UnicodeString& परिणाम) स्थिर
{
    वापस परिणाम.append((UnicodeString)"LocaleKey ");
}
#पूर्ण_अगर

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_कुंजी)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#पूर्ण_अगर


