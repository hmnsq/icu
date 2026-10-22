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
#include "unicode/utypes.h"

#만약 !UCONFIG_NO_SERVICE

#include "unicode/resbund.h"
#include "uresimp.h"
#include "cmemory.h"
#include "servloc.h"
#include "ustrfmt.h"
#include "uhash.h"
#include "charstr.h"
#include "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

LocaleKey*
LocaleKey::createWithCanonicalFallback(const UnicodeString* primaryID,
                                       const UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    반환 LocaleKey::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

LocaleKey*
LocaleKey::createWithCanonicalFallback(const UnicodeString* primaryID,
                                       const UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    만약 (primaryID == nullptr || U_FAILURE(status)) {
        반환 nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    반환 new LocaleKey(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

LocaleKey::LocaleKey(const UnicodeString& primaryID,
                     const UnicodeString& canonicalPrimaryID,
                     const UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUServiceKey(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    만약 (_primaryID.length() != 0) {
        만약 (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

LocaleKey::~LocaleKey() {}

UnicodeString&
LocaleKey::prefix(UnicodeString& result) const {
    만약 (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    반환 result;
}

int32_t
LocaleKey::kind() const {
    반환 _kind;
}

UnicodeString&
LocaleKey::canonicalID(UnicodeString& result) const {
    반환 result.append(_primaryID);
}

UnicodeString&
LocaleKey::currentID(UnicodeString& result) const {
    만약 (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    반환 result;
}

UnicodeString&
LocaleKey::currentDescriptor(UnicodeString& result) const {
    만약 (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } 아니면 {
        result.setToBogus();
    }
    반환 result;
}

Locale&
LocaleKey::canonicalLocale(Locale& result) const {
    반환 LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
LocaleKey::currentLocale(Locale& result) const {
    반환 LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
LocaleKey::fallback() {
    만약 (!_currentID.isBogus()) {
        정수 x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        만약 (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            반환 true;
        }

        만약 (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            반환 true;
        }

        만약 (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            반환 true;
        }

        _currentID.setToBogus();
    }

    반환 false;
}

UBool
LocaleKey::isFallbackOf(const UnicodeString& id) const {
    UnicodeString temp(id);
    parseSuffix(temp);
    반환 temp.indexOf(_primaryID) == 0 &&
        (temp.length() == _primaryID.length() ||
        temp.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
LocaleKey::debug(UnicodeString& result) const
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
    반환 result;
}

UnicodeString&
LocaleKey::debugClass(UnicodeString& result) const
{
    반환 result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(LocaleKey)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


