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
#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_SERVICE

#شامل "unicode/resbund.h"
#شامل "uresimp.h"
#شامل "cmemory.h"
#شامل "servloc.h"
#شامل "ustrfmt.h"
#شامل "uhash.h"
#شامل "charstr.h"
#شامل "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_کلید*
Locale_کلید::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    بازگشت Locale_کلید::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

Locale_کلید*
Locale_کلید::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    اگر (primaryID == nullptr || U_FAILURE(status)) {
        بازگشت nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    بازگشت جدید Locale_کلید(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

Locale_کلید::Locale_کلید(ثابت UnicodeString& primaryID,
                     ثابت UnicodeString& canonicalPrimaryID,
                     ثابت UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUService_کلید(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    اگر (_primaryID.length() != 0) {
        اگر (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_کلید::~Locale_کلید() {}

UnicodeString&
Locale_کلید::prefix(UnicodeString& result) ثابت {
    اگر (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    بازگشت result;
}

int32_t
Locale_کلید::kind() ثابت {
    بازگشت _kind;
}

UnicodeString&
Locale_کلید::canonicalID(UnicodeString& result) ثابت {
    بازگشت result.append(_primaryID);
}

UnicodeString&
Locale_کلید::currentID(UnicodeString& result) ثابت {
    اگر (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    بازگشت result;
}

UnicodeString&
Locale_کلید::currentDescriptor(UnicodeString& result) ثابت {
    اگر (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } وگرنه {
        result.setToBogus();
    }
    بازگشت result;
}

Locale&
Locale_کلید::canonicalLocale(Locale& result) ثابت {
    بازگشت LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
Locale_کلید::currentLocale(Locale& result) ثابت {
    بازگشت LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
Locale_کلید::fallback() {
    اگر (!_currentID.isBogus()) {
        صحیح x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        اگر (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            بازگشت درست;
        }

        اگر (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            بازگشت درست;
        }

        اگر (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            بازگشت درست;
        }

        _currentID.setToBogus();
    }

    بازگشت نادرست;
}

UBool
Locale_کلید::isFallbackOf(ثابت UnicodeString& id) ثابت {
    UnicodeString temp(id);
    parseSuffix(temp);
    بازگشت temp.indexOf(_primaryID) == 0 &&
        (temp.length() == _primaryID.length() ||
        temp.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_کلید::debug(UnicodeString& result) ثابت
{
    ICUService_کلید::debug(result);
    result.append((UnicodeString)" kind: ");
    result.append(_kind);
    result.append((UnicodeString)" primaryID: ");
    result.append(_primaryID);
    result.append((UnicodeString)" fallbackID: ");
    result.append(_fallbackID);
    result.append((UnicodeString)" currentID: ");
    result.append(_currentID);
    بازگشت result;
}

UnicodeString&
Locale_کلید::debugClass(UnicodeString& result) ثابت
{
    بازگشت result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_کلید)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


