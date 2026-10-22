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
#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_SERVICE

#অন্তর্ভাব "unicode/resbund.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "servloc.h"
#অন্তর্ভাব "ustrfmt.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_চাবি*
Locale_চাবি::createWithCanonicalFallback(ধ্রুব UnicodeString* primaryID,
                                       ধ্রুব UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    প্রত্যাগম Locale_চাবি::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

Locale_চাবি*
Locale_চাবি::createWithCanonicalFallback(ধ্রুব UnicodeString* primaryID,
                                       ধ্রুব UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    যদি (primaryID == nullptr || U_FAILURE(status)) {
        প্রত্যাগম nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    প্রত্যাগম নূতন Locale_চাবি(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

Locale_চাবি::Locale_চাবি(ধ্রুব UnicodeString& primaryID,
                     ধ্রুব UnicodeString& canonicalPrimaryID,
                     ধ্রুব UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUService_চাবি(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    যদি (_primaryID.length() != 0) {
        যদি (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_চাবি::~Locale_চাবি() {}

UnicodeString&
Locale_চাবি::prefix(UnicodeString& result) ধ্রুব {
    যদি (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    প্রত্যাগম result;
}

int32_t
Locale_চাবি::kind() ধ্রুব {
    প্রত্যাগম _kind;
}

UnicodeString&
Locale_চাবি::canonicalID(UnicodeString& result) ধ্রুব {
    প্রত্যাগম result.append(_primaryID);
}

UnicodeString&
Locale_চাবি::currentID(UnicodeString& result) ধ্রুব {
    যদি (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    প্রত্যাগম result;
}

UnicodeString&
Locale_চাবি::currentDescriptor(UnicodeString& result) ধ্রুব {
    যদি (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } অন্যথা {
        result.setToBogus();
    }
    প্রত্যাগম result;
}

Locale&
Locale_চাবি::canonicalLocale(Locale& result) ধ্রুব {
    প্রত্যাগম LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
Locale_চাবি::currentLocale(Locale& result) ধ্রুব {
    প্রত্যাগম LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
Locale_চাবি::fallback() {
    যদি (!_currentID.isBogus()) {
        পূর্ণাঙ্ক x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        যদি (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            প্রত্যাগম সত্য;
        }

        যদি (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            প্রত্যাগম সত্য;
        }

        যদি (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            প্রত্যাগম সত্য;
        }

        _currentID.setToBogus();
    }

    প্রত্যাগম অসত্য;
}

UBool
Locale_চাবি::isFallbackOf(ধ্রুব UnicodeString& id) ধ্রুব {
    UnicodeString temp(id);
    parseSuffix(temp);
    প্রত্যাগম temp.indexOf(_primaryID) == 0 &&
        (temp.length() == _primaryID.length() ||
        temp.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_চাবি::debug(UnicodeString& result) ধ্রুব
{
    ICUService_চাবি::debug(result);
    result.append((UnicodeString)" kind: ");
    result.append(_kind);
    result.append((UnicodeString)" primaryID: ");
    result.append(_primaryID);
    result.append((UnicodeString)" fallbackID: ");
    result.append(_fallbackID);
    result.append((UnicodeString)" currentID: ");
    result.append(_currentID);
    প্রত্যাগম result;
}

UnicodeString&
Locale_চাবি::debugClass(UnicodeString& result) ধ্রুব
{
    প্রত্যাগম result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_চাবি)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


