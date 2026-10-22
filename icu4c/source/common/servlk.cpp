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

#تعریف_کن UNDERSCORE_CHAR ((char16_t)0x005f)
#تعریف_کن AT_SIGN_CHAR    ((char16_t)64)
#تعریف_کن PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_کلید*
Locale_کلید::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       UError_کد& وضعیت)
{
    أرجع Locale_کلید::createWithCanonicalFallback(primaryID, canonicalFallbackID, نوع_ANY, وضعیت);
}

Locale_کلید*
Locale_کلید::createWithCanonicalFallback(ثابت UnicodeString* primaryID,
                                       ثابت UnicodeString* canonicalFallbackID,
                                       int32_t نوع,
                                       UError_کد& وضعیت)
{
    إذا (primaryID == اشاره_تهی || U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    أرجع جديد Locale_کلید(*primaryID, canonicalPrimaryID, canonicalFallbackID, نوع);
}

Locale_کلید::Locale_کلید(ثابت UnicodeString& primaryID,
                     ثابت UnicodeString& canonicalPrimaryID,
                     ثابت UnicodeString* canonicalFallbackID,
                     int32_t نوع)
  : ICUService_کلید(primaryID)
  , نوع_2(نوع)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.مجموعه_مقصد_Bogus();
    إذا (_primaryID.length() != 0) {
        إذا (canonicalFallbackID != اشاره_تهی && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_کلید::~Locale_کلید() {}

UnicodeString&
Locale_کلید::پیشوند(UnicodeString& نتیجه) ثابت {
    إذا (نوع_2 != نوع_ANY) {
        char16_t میانگیر[64];
        uprv_itou(میانگیر, 64, نوع_2, 10, 0);
        UnicodeString موقت(میانگیر);
        نتیجه.append(موقت);
    }
    أرجع نتیجه;
}

int32_t
Locale_کلید::نوع() ثابت {
    أرجع نوع_2;
}

UnicodeString&
Locale_کلید::canonicalID(UnicodeString& نتیجه) ثابت {
    أرجع نتیجه.append(_primaryID);
}

UnicodeString&
Locale_کلید::currentID(UnicodeString& نتیجه) ثابت {
    إذا (!_currentID.isBogus()) {
        نتیجه.append(_currentID);
    }
    أرجع نتیجه;
}

UnicodeString&
Locale_کلید::currentDescriptor(UnicodeString& نتیجه) ثابت {
    إذا (!_currentID.isBogus()) {
        پیشوند(نتیجه).append(پیشوند_DELIMITER).append(_currentID);
    } وإلا {
        نتیجه.مجموعه_مقصد_Bogus();
    }
    أرجع نتیجه;
}

Locale&
Locale_کلید::canonicalLocale(Locale& نتیجه) ثابت {
    أرجع LocaleUtility::initLocaleFromName(_primaryID, نتیجه);
}

Locale&
Locale_کلید::currentLocale(Locale& نتیجه) ثابت {
    أرجع LocaleUtility::initLocaleFromName(_currentID, نتیجه);
}

UBool
Locale_کلید::fallback() {
    إذا (!_currentID.isBogus()) {
        صحيح x = _currentID.آخرین_شاخص_از(UNDERSCORE_CHAR);
        إذا (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            أرجع درست;
        }

        إذا (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.مجموعه_مقصد_Bogus();
            أرجع درست;
        }

        إذا (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            أرجع درست;
        }

        _currentID.مجموعه_مقصد_Bogus();
    }

    أرجع خطأ;
}

UBool
Locale_کلید::is_Fallback_از(ثابت UnicodeString& id) ثابت {
    UnicodeString موقت(id);
    parseSuffix(موقت);
    أرجع موقت.شاخص_از(_primaryID) == 0 &&
        (موقت.length() == _primaryID.length() ||
        موقت.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#اگر_تعریف_شده SERVICE_DEBUG
UnicodeString&
Locale_کلید::debug(UnicodeString& نتیجه) ثابت
{
    ICUService_کلید::debug(نتیجه);
    نتیجه.append((UnicodeString)" kind: ");
    نتیجه.append(نوع_2);
    نتیجه.append((UnicodeString)" primaryID: ");
    نتیجه.append(_primaryID);
    نتیجه.append((UnicodeString)" fallbackID: ");
    نتیجه.append(_fallbackID);
    نتیجه.append((UnicodeString)" currentID: ");
    نتیجه.append(_currentID);
    أرجع نتیجه;
}

UnicodeString&
Locale_کلید::debugClass(UnicodeString& نتیجه) ثابت
{
    أرجع نتیجه.append((UnicodeString)"LocaleKey ");
}
#پایان_اگر

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_کلید)

U_NAMESPACE_انتها

/* !UCONFIG_NO_SERVICE */
#پایان_اگر


