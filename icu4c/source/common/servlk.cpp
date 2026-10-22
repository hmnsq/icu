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
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_SERVICE

#ଅନ୍ତର୍ଭାଵ "unicode/resbund.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "servloc.h"
#ଅନ୍ତର୍ଭାଵ "ustrfmt.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

#define UNDERSCORE_CHAR ((char16_t)0x005f)
#define AT_SIGN_CHAR    ((char16_t)64)
#define PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_ଚାବି*
Locale_ଚାବି::createWithCanonicalFallback(ଅଚଳ UnicodeString* primaryID,
                                       ଅଚଳ UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    ଫେରନ୍ତୁ Locale_ଚାବି::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

Locale_ଚାବି*
Locale_ଚାବି::createWithCanonicalFallback(ଅଚଳ UnicodeString* primaryID,
                                       ଅଚଳ UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    ଯଦି (primaryID == nullptr || U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    ଫେରନ୍ତୁ ନୂତନ Locale_ଚାବି(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

Locale_ଚାବି::Locale_ଚାବି(ଅଚଳ UnicodeString& primaryID,
                     ଅଚଳ UnicodeString& canonicalPrimaryID,
                     ଅଚଳ UnicodeString* canonicalFallbackID,
                     int32_t kind)
  : ICUService_ଚାବି(primaryID)
  , _kind(kind)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.setToBogus();
    ଯଦି (_primaryID.length() != 0) {
        ଯଦି (canonicalFallbackID != nullptr && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_ଚାବି::~Locale_ଚାବି() {}

UnicodeString&
Locale_ଚାବି::prefix(UnicodeString& result) ଅଚଳ {
    ଯଦି (_kind != KIND_ANY) {
        char16_t buffer[64];
        uprv_itou(buffer, 64, _kind, 10, 0);
        UnicodeString temp(buffer);
        result.append(temp);
    }
    ଫେରନ୍ତୁ result;
}

int32_t
Locale_ଚାବି::kind() ଅଚଳ {
    ଫେରନ୍ତୁ _kind;
}

UnicodeString&
Locale_ଚାବି::canonicalID(UnicodeString& result) ଅଚଳ {
    ଫେରନ୍ତୁ result.append(_primaryID);
}

UnicodeString&
Locale_ଚାବି::currentID(UnicodeString& result) ଅଚଳ {
    ଯଦି (!_currentID.isBogus()) {
        result.append(_currentID);
    }
    ଫେରନ୍ତୁ result;
}

UnicodeString&
Locale_ଚାବି::currentDescriptor(UnicodeString& result) ଅଚଳ {
    ଯଦି (!_currentID.isBogus()) {
        prefix(result).append(PREFIX_DELIMITER).append(_currentID);
    } ଅନ୍ୟଥା {
        result.setToBogus();
    }
    ଫେରନ୍ତୁ result;
}

Locale&
Locale_ଚାବି::canonicalLocale(Locale& result) ଅଚଳ {
    ଫେରନ୍ତୁ LocaleUtility::initLocaleFromName(_primaryID, result);
}

Locale&
Locale_ଚାବି::currentLocale(Locale& result) ଅଚଳ {
    ଫେରନ୍ତୁ LocaleUtility::initLocaleFromName(_currentID, result);
}

UBool
Locale_ଚାବି::fallback() {
    ଯଦି (!_currentID.isBogus()) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା x = _currentID.lastIndexOf(UNDERSCORE_CHAR);
        ଯଦି (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }

        ଯଦି (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }

        ଯଦି (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }

        _currentID.setToBogus();
    }

    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

UBool
Locale_ଚାବି::isFallbackOf(ଅଚଳ UnicodeString& id) ଅଚଳ {
    UnicodeString temp(id);
    parseSuffix(temp);
    ଫେରନ୍ତୁ temp.indexOf(_primaryID) == 0 &&
        (temp.length() == _primaryID.length() ||
        temp.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_ଚାବି::debug(UnicodeString& result) ଅଚଳ
{
    ICUService_ଚାବି::debug(result);
    result.append((UnicodeString)" kind: ");
    result.append(_kind);
    result.append((UnicodeString)" primaryID: ");
    result.append(_primaryID);
    result.append((UnicodeString)" fallbackID: ");
    result.append(_fallbackID);
    result.append((UnicodeString)" currentID: ");
    result.append(_currentID);
    ଫେରନ୍ତୁ result;
}

UnicodeString&
Locale_ଚାବି::debugClass(UnicodeString& result) ଅଚଳ
{
    ଫେରନ୍ତୁ result.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_ଚାବି)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


