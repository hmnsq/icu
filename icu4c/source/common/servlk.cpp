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
Locale_চাবি::createWithCanonicalFallback(ধ্রুবক UnicodeString* primaryID,
                                       ধ্রুবক UnicodeString* canonicalFallbackID,
                                       UErrorCode& status)
{
    ফেরত Locale_চাবি::createWithCanonicalFallback(primaryID, canonicalFallbackID, KIND_ANY, status);
}

Locale_চাবি*
Locale_চাবি::createWithCanonicalFallback(ধ্রুবক UnicodeString* primaryID,
                                       ধ্রুবক UnicodeString* canonicalFallbackID,
                                       int32_t kind,
                                       UErrorCode& status)
{
    যদি (primaryID == nullptr || U_FAILURE(status)) {
        ফেরত nullptr;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    ফেরত নতুন Locale_চাবি(*primaryID, canonicalPrimaryID, canonicalFallbackID, kind);
}

Locale_চাবি::Locale_চাবি(ধ্রুবক UnicodeString& primaryID,
                     ধ্রুবক UnicodeString& canonicalPrimaryID,
                     ধ্রুবক UnicodeString* canonicalFallbackID,
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
Locale_চাবি::prefix(UnicodeString& ফলাফল) ধ্রুবক {
    যদি (_kind != KIND_ANY) {
        char16_t বাফার[64];
        uprv_itou(বাফার, 64, _kind, 10, 0);
        UnicodeString অস্থায়ী(বাফার);
        ফলাফল.append(অস্থায়ী);
    }
    ফেরত ফলাফল;
}

int32_t
Locale_চাবি::kind() ধ্রুবক {
    ফেরত _kind;
}

UnicodeString&
Locale_চাবি::canonicalID(UnicodeString& ফলাফল) ধ্রুবক {
    ফেরত ফলাফল.append(_primaryID);
}

UnicodeString&
Locale_চাবি::currentID(UnicodeString& ফলাফল) ধ্রুবক {
    যদি (!_currentID.isBogus()) {
        ফলাফল.append(_currentID);
    }
    ফেরত ফলাফল;
}

UnicodeString&
Locale_চাবি::currentDescriptor(UnicodeString& ফলাফল) ধ্রুবক {
    যদি (!_currentID.isBogus()) {
        prefix(ফলাফল).append(PREFIX_DELIMITER).append(_currentID);
    } অন্যথা {
        ফলাফল.setToBogus();
    }
    ফেরত ফলাফল;
}

Locale&
Locale_চাবি::canonicalLocale(Locale& ফলাফল) ধ্রুবক {
    ফেরত LocaleUtility::initLocaleFromName(_primaryID, ফলাফল);
}

Locale&
Locale_চাবি::currentLocale(Locale& ফলাফল) ধ্রুবক {
    ফেরত LocaleUtility::initLocaleFromName(_currentID, ফলাফল);
}

UBool
Locale_চাবি::fallback() {
    যদি (!_currentID.isBogus()) {
        পূর্ণসংখ্যা x = _currentID.last_সূচক_Of(UNDERSCORE_CHAR);
        যদি (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            ফেরত সত্য;
        }

        যদি (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.setToBogus();
            ফেরত সত্য;
        }

        যদি (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            ফেরত সত্য;
        }

        _currentID.setToBogus();
    }

    ফেরত মিথ্যা;
}

UBool
Locale_চাবি::isFallbackOf(ধ্রুবক UnicodeString& id) ধ্রুবক {
    UnicodeString অস্থায়ী(id);
    parseSuffix(অস্থায়ী);
    ফেরত অস্থায়ী.সূচক_Of(_primaryID) == 0 &&
        (অস্থায়ী.length() == _primaryID.length() ||
        অস্থায়ী.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#ifdef SERVICE_DEBUG
UnicodeString&
Locale_চাবি::debug(UnicodeString& ফলাফল) ধ্রুবক
{
    ICUService_চাবি::debug(ফলাফল);
    ফলাফল.append((UnicodeString)" kind: ");
    ফলাফল.append(_kind);
    ফলাফল.append((UnicodeString)" primaryID: ");
    ফলাফল.append(_primaryID);
    ফলাফল.append((UnicodeString)" fallbackID: ");
    ফলাফল.append(_fallbackID);
    ফলাফল.append((UnicodeString)" currentID: ");
    ফলাফল.append(_currentID);
    ফেরত ফলাফল;
}

UnicodeString&
Locale_চাবি::debugClass(UnicodeString& ফলাফল) ধ্রুবক
{
    ফেরত ফলাফল.append((UnicodeString)"LocaleKey ");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_চাবি)

U_NAMESPACE_END

/* !UCONFIG_NO_SERVICE */
#endif


