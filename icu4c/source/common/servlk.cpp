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

#وصف_ڪريو UNDERSCORE_CHAR ((char16_t)0x005f)
#وصف_ڪريو AT_SIGN_CHAR    ((char16_t)64)
#وصف_ڪريو PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_ڪنجي*
Locale_ڪنجي::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       UError_ڪوڊ& حالت)
{
    أرجع Locale_ڪنجي::createWithCanonicalFallback(primaryID, canonicalFallbackID, قسم_ANY, حالت);
}

Locale_ڪنجي*
Locale_ڪنجي::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       int32_t قسم,
                                       UError_ڪوڊ& حالت)
{
    إذا (primaryID == خالي_اشارو || U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    أرجع جديد Locale_ڪنجي(*primaryID, canonicalPrimaryID, canonicalFallbackID, قسم);
}

Locale_ڪنجي::Locale_ڪنجي(ثابت UnicodeString& primaryID,
                     ثابت UnicodeString& canonicalPrimaryID,
                     ثابت UnicodeString* canonicalFallbackID,
                     int32_t قسم)
  : ICUService_ڪنجي(primaryID)
  , قسم_2(قسم)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.سيٽ_منزل_Bogus();
    إذا (_primaryID.length() != 0) {
        إذا (canonicalFallbackID != خالي_اشارو && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_ڪنجي::~Locale_ڪنجي() {}

UnicodeString&
Locale_ڪنجي::اڳياڙي(UnicodeString& نتيجو) ثابت {
    إذا (قسم_2 != قسم_ANY) {
        char16_t بفر[64];
        uprv_itou(بفر, 64, قسم_2, 10, 0);
        UnicodeString عارضي(بفر);
        نتيجو.append(عارضي);
    }
    أرجع نتيجو;
}

int32_t
Locale_ڪنجي::قسم() ثابت {
    أرجع قسم_2;
}

UnicodeString&
Locale_ڪنجي::canonicalID(UnicodeString& نتيجو) ثابت {
    أرجع نتيجو.append(_primaryID);
}

UnicodeString&
Locale_ڪنجي::currentID(UnicodeString& نتيجو) ثابت {
    إذا (!_currentID.isBogus()) {
        نتيجو.append(_currentID);
    }
    أرجع نتيجو;
}

UnicodeString&
Locale_ڪنجي::currentDescriptor(UnicodeString& نتيجو) ثابت {
    إذا (!_currentID.isBogus()) {
        اڳياڙي(نتيجو).append(اڳياڙي_DELIMITER).append(_currentID);
    } وإلا {
        نتيجو.سيٽ_منزل_Bogus();
    }
    أرجع نتيجو;
}

Locale&
Locale_ڪنجي::canonicalLocale(Locale& نتيجو) ثابت {
    أرجع LocaleUtility::initLocaleFromName(_primaryID, نتيجو);
}

Locale&
Locale_ڪنجي::currentLocale(Locale& نتيجو) ثابت {
    أرجع LocaleUtility::initLocaleFromName(_currentID, نتيجو);
}

UBool
Locale_ڪنجي::fallback() {
    إذا (!_currentID.isBogus()) {
        صحيح x = _currentID.آخري_اشاريو_جو(UNDERSCORE_CHAR);
        إذا (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            أرجع سچ;
        }

        إذا (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.سيٽ_منزل_Bogus();
            أرجع سچ;
        }

        إذا (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            أرجع سچ;
        }

        _currentID.سيٽ_منزل_Bogus();
    }

    أرجع خطأ;
}

UBool
Locale_ڪنجي::is_Fallback_جو(ثابت UnicodeString& id) ثابت {
    UnicodeString عارضي(id);
    parseSuffix(عارضي);
    أرجع عارضي.اشاريو_جو(_primaryID) == 0 &&
        (عارضي.length() == _primaryID.length() ||
        عارضي.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#جيڪڏهن_وصف_ٿيل SERVICE_DEBUG
UnicodeString&
Locale_ڪنجي::debug(UnicodeString& نتيجو) ثابت
{
    ICUService_ڪنجي::debug(نتيجو);
    نتيجو.append((UnicodeString)" kind: ");
    نتيجو.append(قسم_2);
    نتيجو.append((UnicodeString)" primaryID: ");
    نتيجو.append(_primaryID);
    نتيجو.append((UnicodeString)" fallbackID: ");
    نتيجو.append(_fallbackID);
    نتيجو.append((UnicodeString)" currentID: ");
    نتيجو.append(_currentID);
    أرجع نتيجو;
}

UnicodeString&
Locale_ڪنجي::debugClass(UnicodeString& نتيجو) ثابت
{
    أرجع نتيجو.append((UnicodeString)"LocaleKey ");
}
#جيڪڏهن_پورو

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_ڪنجي)

U_NAMESPACE_پڇاڙي

/* !UCONFIG_NO_SERVICE */
#جيڪڏهن_پورو


