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

#إذا !UCONFIG_NO_SERVICE

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

Locale_مفتاح*
Locale_مفتاح::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    أرجع Locale_مفتاح::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

Locale_مفتاح*
Locale_مفتاح::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    إذا (primaryID == nullptr || U_FAILURE(status)) {
        أرجع nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    أرجع جديد Locale_مفتاح(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

Locale_مفتاح::Locale_مفتاح(ثابت UnicodeString& primaryID,
                     ثابت UnicodeString& canonicalPrimaryID,
                     ثابت UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUService_مفتاح(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    إذا (_primaryID.length() != 0) {
        إذا (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_مفتاح::~Locale_مفتاح() {}

UnicodeString&
Locale_مفتاح::prefix(UnicodeString& result) ثابت {
    إذا (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    أرجع result;
}

int32_t
Locale_مفتاح::kind() ثابت {
    أرجع _kind;
}

UnicodeString&
Locale_مفتاح::canonicalID(UnicodeString& result) ثابت {
    أرجع result.append(_primaryID);
}

UnicodeString&
Locale_مفتاح::currentID(UnicodeString& result) ثابت {
    إذا (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    أرجع result;
}

UnicodeString&
Locale_مفتاح::currentDescriptor(UnicodeString& result) ثابت {
    إذا (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } وإلا {
        result.setToBogus();
    }
    أرجع result;
}

Locale&
Locale_مفتاح::canonicalLocale(Locale& result) ثابت {
    أرجع LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
Locale_مفتاح::currentLocale(Locale& result) ثابت {
    أرجع LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
Locale_مفتاح::fallback() {
    إذا (!_currentID.isBogus()) {
        صحيح x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        إذا (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            أرجع true;
        }

        إذا (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            أرجع true;
        }

        إذا (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            أرجع true;
        }

        _currentID.setToBogus();
    }

    أرجع خطأ;
}

UBool
Locale_مفتاح::isFallbackOf(ثابت UnicodeString& id) ثابت {
    UnicodeString temp(id);
    parseSuffix(temp);
    أرجع temp.indexOf(_primaryID) == 0 &&
        (temp.length() == _primaryID.length() ||
        temp.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_مفتاح::debug(UnicodeString& result) ثابت
{
    ICUService_مفتاح::debug(result);
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
Locale_مفتاح::debugClass(UnicodeString& result) ثابت
{
    أرجع result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_مفتاح)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


