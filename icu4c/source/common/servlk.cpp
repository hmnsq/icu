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
#ضمن "unicode/utypes.h"

#𐤀𐤌 !UCONFIG_NO_SERVICE

#ضمن "unicode/resbund.h"
#ضمن "uresimp.h"
#ضمن "cmemory.h"
#ضمن "servloc.h"
#ضمن "ustrfmt.h"
#ضمن "uhash.h"
#ضمن "charstr.h"
#ضمن "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

LocaleKey*
LocaleKey::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    أرجع LocaleKey::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

LocaleKey*
LocaleKey::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    𐤀𐤌 (primaryID == nullptr || U_FAILURE(status)) {
        أرجع nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    أرجع جديد LocaleKey(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

LocaleKey::LocaleKey(ثابت UnicodeString& primaryID,
                     ثابت UnicodeString& canonicalPrimaryID,
                     ثابت UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUServiceKey(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    𐤀𐤌 (_primaryID.length() != 0) {
        𐤀𐤌 (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

LocaleKey::~LocaleKey() {}

UnicodeString&
LocaleKey::prefix(UnicodeString& result) ثابت {
    𐤀𐤌 (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    أرجع result;
}

int32_t
LocaleKey::kind() ثابت {
    أرجع _kind;
}

UnicodeString&
LocaleKey::canonicalID(UnicodeString& result) ثابت {
    أرجع result.append(_primaryID);
}

UnicodeString&
LocaleKey::currentID(UnicodeString& result) ثابت {
    𐤀𐤌 (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    أرجع result;
}

UnicodeString&
LocaleKey::currentDescriptor(UnicodeString& result) ثابت {
    𐤀𐤌 (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } وإلا {
        result.setToBogus();
    }
    أرجع result;
}

Locale&
LocaleKey::canonicalLocale(Locale& result) ثابت {
    أرجع LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
LocaleKey::currentLocale(Locale& result) ثابت {
    أرجع LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
LocaleKey::fallback() {
    𐤀𐤌 (!_currentID.isBogus()) {
        صحيح x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        𐤀𐤌 (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            أرجع true;
        }

        𐤀𐤌 (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            أرجع true;
        }

        𐤀𐤌 (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            أرجع true;
        }

        _currentID.setToBogus();
    }

    أرجع خطأ;
}

UBool
LocaleKey::isFallbackOf(ثابت UnicodeString& id) ثابت {
    UnicodeString temp(id);
    parseSuffix(temp);
    أرجع temp.indexOf(_primaryID) == 0 &&
        (temp.length() == _primaryID.length() ||
        temp.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
LocaleKey::debug(UnicodeString& result) ثابت
{
    ICUServiceKey::debug(result);
    result.append((UnicodeString)" kind: ");
    result.append(_kind);
    result.append((UnicodeString)" primaryID: ");
    result.append(_primaryID);
    result.append((UnicodeString)" fallbackID: ");
    result.append(_fallbackID);
    result.append((UnicodeString)" currentID: ");
    result.append(_currentID);
    أرجع result;
}

UnicodeString&
LocaleKey::debugClass(UnicodeString& result) ثابت
{
    أرجع result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(LocaleKey)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


