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

#تعریف_کرو UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کرو AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کرو PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_کلید*
Locale_کلید::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       UError_کوڈ& حالت)
{
    أرجع Locale_کلید::createWithCanonicalFallback(primaryID, canonicalFallbackID, قسم_ANY, حالت);
}

Locale_کلید*
Locale_کلید::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       int32_t قسم,
                                       UError_کوڈ& حالت)
{
    إذا (primaryID == خالی_سوچک || U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    أرجع جديد Locale_کلید(*primaryID, canonicalPrimaryID, canonicalFallbackID, قسم);
}

Locale_کلید::Locale_کلید(ثابت UnicodeString& primaryID,
                     ثابت UnicodeString& canonicalPrimaryID,
                     ثابت UnicodeString* canonicalFallbackID,
                     int32_t قسم)
  : ICUService_کلید(primaryID)
  , قسم_2(قسم)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.سیٹ_منزل_Bogus();
    إذا (_primaryID.length() != 0) {
        إذا (canonicalFallbackID != خالی_سوچک && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_کلید::~Locale_کلید() {}

UnicodeString&
Locale_کلید::اگیتر(UnicodeString& نتیجہ) ثابت {
    إذا (قسم_2 != قسم_ANY) {
        char16_t بفر[64];
        uprv_itou(بفر, 64, قسم_2, 10, 0);
        UnicodeString عارضی(بفر);
        نتیجہ.append(عارضی);
    }
    أرجع نتیجہ;
}

int32_t
Locale_کلید::قسم() ثابت {
    أرجع قسم_2;
}

UnicodeString&
Locale_کلید::canonicalID(UnicodeString& نتیجہ) ثابت {
    أرجع نتیجہ.append(_primaryID);
}

UnicodeString&
Locale_کلید::currentID(UnicodeString& نتیجہ) ثابت {
    إذا (!_currentID.isBogus()) {
        نتیجہ.append(_currentID);
    }
    أرجع نتیجہ;
}

UnicodeString&
Locale_کلید::currentDescriptor(UnicodeString& نتیجہ) ثابت {
    إذا (!_currentID.isBogus()) {
        اگیتر(نتیجہ).append(اگیتر_DELIMITER).append(_currentID);
    } وإلا {
        نتیجہ.سیٹ_منزل_Bogus();
    }
    أرجع نتیجہ;
}

Locale&
Locale_کلید::canonicalLocale(Locale& نتیجہ) ثابت {
    أرجع LocaleUtility::initLocaleFromName(_primaryID, نتیجہ);
}

Locale&
Locale_کلید::currentLocale(Locale& نتیجہ) ثابت {
    أرجع LocaleUtility::initLocaleFromName(_currentID, نتیجہ);
}

UBool
Locale_کلید::fallback() {
    إذا (!_currentID.isBogus()) {
        صحيح x = _currentID.آخری_اشاریہ_دا(UNDERSCORE_CHAR);
        إذا (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            أرجع سچ;
        }

        إذا (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.سیٹ_منزل_Bogus();
            أرجع سچ;
        }

        إذا (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            أرجع سچ;
        }

        _currentID.سیٹ_منزل_Bogus();
    }

    أرجع خطأ;
}

UBool
Locale_کلید::is_Fallback_دا(ثابت UnicodeString& id) ثابت {
    UnicodeString عارضی(id);
    parseSuffix(عارضی);
    أرجع عارضی.اشاریہ_دا(_primaryID) == 0 &&
        (عارضی.length() == _primaryID.length() ||
        عارضی.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#جے_تعریف_شدہ SERVICE_DEBUG
UnicodeString&
Locale_کلید::debug(UnicodeString& نتیجہ) ثابت
{
    ICUService_کلید::debug(نتیجہ);
    نتیجہ.append((UnicodeString)" kind: ");
    نتیجہ.append(قسم_2);
    نتیجہ.append((UnicodeString)" primaryID: ");
    نتیجہ.append(_primaryID);
    نتیجہ.append((UnicodeString)" fallbackID: ");
    نتیجہ.append(_fallbackID);
    نتیجہ.append((UnicodeString)" currentID: ");
    نتیجہ.append(_currentID);
    أرجع نتیجہ;
}

UnicodeString&
Locale_کلید::debugClass(UnicodeString& نتیجہ) ثابت
{
    أرجع نتیجہ.append((UnicodeString)"LocaleKey ");
}
#جے_انت

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_کلید)

U_NAMESPACE_سرا

/* !UCONFIG_NO_SERVICE */
#جے_انت


