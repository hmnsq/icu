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

#تعریف_کرو UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کرو AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کرو PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_کلید*
Locale_کلید::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       UError_رمز& حالت_2)
{
    واپس Locale_کلید::createWithCanonicalFallback(primaryID, canonicalFallbackID, قسم_ANY, حالت_2);
}

Locale_کلید*
Locale_کلید::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       int32_t قسم,
                                       UError_رمز& حالت_2)
{
    اگر (primaryID == خالی_اشاریہ || U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    واپس نیا Locale_کلید(*primaryID, canonicalPrimaryID, canonicalFallbackID, قسم);
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
    _fallbackID.سیٹ_مقصد_Bogus();
    اگر (_primaryID.length() != 0) {
        اگر (canonicalFallbackID != خالی_اشاریہ && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_کلید::~Locale_کلید() {}

UnicodeString&
Locale_کلید::سابقہ(UnicodeString& نتیجہ) ثابت {
    اگر (قسم_2 != قسم_ANY) {
        char16_t بفر[64];
        uprv_itou(بفر, 64, قسم_2, 10, 0);
        UnicodeString عارضی(بفر);
        نتیجہ.append(عارضی);
    }
    واپس نتیجہ;
}

int32_t
Locale_کلید::قسم() ثابت {
    واپس قسم_2;
}

UnicodeString&
Locale_کلید::canonicalID(UnicodeString& نتیجہ) ثابت {
    واپس نتیجہ.append(_primaryID);
}

UnicodeString&
Locale_کلید::currentID(UnicodeString& نتیجہ) ثابت {
    اگر (!_currentID.isBogus()) {
        نتیجہ.append(_currentID);
    }
    واپس نتیجہ;
}

UnicodeString&
Locale_کلید::currentDescriptor(UnicodeString& نتیجہ) ثابت {
    اگر (!_currentID.isBogus()) {
        سابقہ(نتیجہ).append(سابقہ_DELIMITER).append(_currentID);
    } ورنہ {
        نتیجہ.سیٹ_مقصد_Bogus();
    }
    واپس نتیجہ;
}

Locale&
Locale_کلید::canonicalLocale(Locale& نتیجہ) ثابت {
    واپس LocaleUtility::initLocaleFromName(_primaryID, نتیجہ);
}

Locale&
Locale_کلید::currentLocale(Locale& نتیجہ) ثابت {
    واپس LocaleUtility::initLocaleFromName(_currentID, نتیجہ);
}

UBool
Locale_کلید::fallback() {
    اگر (!_currentID.isBogus()) {
        صحیح x = _currentID.آخری_اشاریہ_کا(UNDERSCORE_CHAR);
        اگر (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            واپس سچ;
        }

        اگر (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.سیٹ_مقصد_Bogus();
            واپس سچ;
        }

        اگر (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            واپس سچ;
        }

        _currentID.سیٹ_مقصد_Bogus();
    }

    واپس جھوٹ;
}

UBool
Locale_کلید::is_Fallback_کا(ثابت UnicodeString& id) ثابت {
    UnicodeString عارضی(id);
    parseSuffix(عارضی);
    واپس عارضی.اشاریہ_کا(_primaryID) == 0 &&
        (عارضی.length() == _primaryID.length() ||
        عارضی.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#اگر_تعریف_شدہ SERVICE_DEBUG
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
    واپس نتیجہ;
}

UnicodeString&
Locale_کلید::debugClass(UnicodeString& نتیجہ) ثابت
{
    واپس نتیجہ.append((UnicodeString)"LocaleKey ");
}
#اگر_ختم

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_کلید)

U_NAMESPACE_انتہا

/* !UCONFIG_NO_SERVICE */
#اگر_ختم


