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

#تعریف_کړه UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کړه AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کړه PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_کلی*
Locale_کلی::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       UError_کوډ& حالت)
{
    أرجع Locale_کلی::createWithCanonicalFallback(primaryID, canonicalFallbackID, ډول_ANY, حالت);
}

Locale_کلی*
Locale_کلی::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       int32_t ډول,
                                       UError_کوډ& حالت)
{
    إذا (primaryID == تش_اشاره || U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    أرجع جديد Locale_کلی(*primaryID, canonicalPrimaryID, canonicalFallbackID, ډول);
}

Locale_کلی::Locale_کلی(ثابت UnicodeString& primaryID,
                     ثابت UnicodeString& canonicalPrimaryID,
                     ثابت UnicodeString* canonicalFallbackID,
                     int32_t ډول)
  : ICUService_کلی(primaryID)
  , ډول_2(ډول)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.سېټ_منزل_Bogus();
    إذا (_primaryID.length() != 0) {
        إذا (canonicalFallbackID != تش_اشاره && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_کلی::~Locale_کلی() {}

UnicodeString&
Locale_کلی::مختاړی(UnicodeString& پایله) ثابت {
    إذا (ډول_2 != ډول_ANY) {
        char16_t بفر[64];
        uprv_itou(بفر, 64, ډول_2, 10, 0);
        UnicodeString لنډمهاله(بفر);
        پایله.append(لنډمهاله);
    }
    أرجع پایله;
}

int32_t
Locale_کلی::ډول() ثابت {
    أرجع ډول_2;
}

UnicodeString&
Locale_کلی::canonicalID(UnicodeString& پایله) ثابت {
    أرجع پایله.append(_primaryID);
}

UnicodeString&
Locale_کلی::currentID(UnicodeString& پایله) ثابت {
    إذا (!_currentID.isBogus()) {
        پایله.append(_currentID);
    }
    أرجع پایله;
}

UnicodeString&
Locale_کلی::currentDescriptor(UnicodeString& پایله) ثابت {
    إذا (!_currentID.isBogus()) {
        مختاړی(پایله).append(مختاړی_DELIMITER).append(_currentID);
    } وإلا {
        پایله.سېټ_منزل_Bogus();
    }
    أرجع پایله;
}

Locale&
Locale_کلی::canonicalLocale(Locale& پایله) ثابت {
    أرجع LocaleUtility::initLocaleFromName(_primaryID, پایله);
}

Locale&
Locale_کلی::currentLocale(Locale& پایله) ثابت {
    أرجع LocaleUtility::initLocaleFromName(_currentID, پایله);
}

UBool
Locale_کلی::fallback() {
    إذا (!_currentID.isBogus()) {
        صحيح x = _currentID.وروستی_شاخص_د(UNDERSCORE_CHAR);
        إذا (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            أرجع ریښتیا;
        }

        إذا (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.سېټ_منزل_Bogus();
            أرجع ریښتیا;
        }

        إذا (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            أرجع ریښتیا;
        }

        _currentID.سېټ_منزل_Bogus();
    }

    أرجع خطأ;
}

UBool
Locale_کلی::is_Fallback_د(ثابت UnicodeString& id) ثابت {
    UnicodeString لنډمهاله(id);
    parseSuffix(لنډمهاله);
    أرجع لنډمهاله.شاخص_د(_primaryID) == 0 &&
        (لنډمهاله.length() == _primaryID.length() ||
        لنډمهاله.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#که_تعریف_شوی SERVICE_DEBUG
UnicodeString&
Locale_کلی::debug(UnicodeString& پایله) ثابت
{
    ICUService_کلی::debug(پایله);
    پایله.append((UnicodeString)" kind: ");
    پایله.append(ډول_2);
    پایله.append((UnicodeString)" primaryID: ");
    پایله.append(_primaryID);
    پایله.append((UnicodeString)" fallbackID: ");
    پایله.append(_fallbackID);
    پایله.append((UnicodeString)" currentID: ");
    پایله.append(_currentID);
    أرجع پایله;
}

UnicodeString&
Locale_کلی::debugClass(UnicodeString& پایله) ثابت
{
    أرجع پایله.append((UnicodeString)"LocaleKey ");
}
#د_که_پای

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_کلی)

U_NAMESPACE_پای_ټکی

/* !UCONFIG_NO_SERVICE */
#د_که_پای


