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

#ຖ້າ !UCONFIG_NO_SERVICE

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
    ສົ່ງຄືນ LocaleKey::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

LocaleKey*
LocaleKey::createWithCanonicalFallback(const UnicodeString* primaryID,
                                       const UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    ຖ້າ (primaryID == nullptr || U_FAILURE(status)) {
        ສົ່ງຄືນ nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    ສົ່ງຄືນ new LocaleKey(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
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
    ຖ້າ (_primaryID.length() != 0) {
        ຖ້າ (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

LocaleKey::~LocaleKey() {}

UnicodeString&
LocaleKey::prefix(UnicodeString& result) const {
    ຖ້າ (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    ສົ່ງຄືນ result;
}

int32_t
LocaleKey::kind() const {
    ສົ່ງຄືນ _kind;
}

UnicodeString&
LocaleKey::canonicalID(UnicodeString& result) const {
    ສົ່ງຄືນ result.append(_primaryID);
}

UnicodeString&
LocaleKey::currentID(UnicodeString& result) const {
    ຖ້າ (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    ສົ່ງຄືນ result;
}

UnicodeString&
LocaleKey::currentDescriptor(UnicodeString& result) const {
    ຖ້າ (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } ບໍ່ດັ່ງນັ້ນ {
        result.setToBogus();
    }
    ສົ່ງຄືນ result;
}

Locale&
LocaleKey::canonicalLocale(Locale& result) const {
    ສົ່ງຄືນ LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
LocaleKey::currentLocale(Locale& result) const {
    ສົ່ງຄືນ LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
LocaleKey::fallback() {
    ຖ້າ (!_currentID.isBogus()) {
        ຈຳນວນເຕັມ x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        ຖ້າ (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            ສົ່ງຄືນ true;
        }

        ຖ້າ (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            ສົ່ງຄືນ true;
        }

        ຖ້າ (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            ສົ່ງຄືນ true;
        }

        _currentID.setToBogus();
    }

    ສົ່ງຄືນ false;
}

UBool
LocaleKey::isFallbackOf(const UnicodeString& id) const {
    UnicodeString temp(id);
    parseSuffix(temp);
    ສົ່ງຄືນ temp.indexOf(_primaryID) == 0 &&
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
    ສົ່ງຄືນ result;
}

UnicodeString&
LocaleKey::debugClass(UnicodeString& result) const
{
    ສົ່ງຄືນ result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(LocaleKey)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


