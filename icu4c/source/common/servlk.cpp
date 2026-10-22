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
#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_SERVICE

#అన్తర్భావ "unicode/resbund.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "servloc.h"
#అన్తర్భావ "ustrfmt.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "uassert.h"

#నిర్వచించు UNDERSCORE_CHAR ((char16_t)0x005f)
#నిర్వచించు AT_SIGN_CHAR    ((char16_t)64)
#నిర్వచించు PERIOD_CHAR     ((char16_t)46)

U_NAMESPACE_BEGIN

Locale_కీలకం*
Locale_కీలకం::createWithCanonicalFallback(స్థిరరాశి UnicodeString* primaryID,
                                       స్థిరరాశి UnicodeString* canonicalFallbackID,
                                       UError_సంకేతం& స్థితి_2)
{
    తిరిగి Locale_కీలకం::createWithCanonicalFallback(primaryID, canonicalFallbackID, రకం_ANY, స్థితి_2);
}

Locale_కీలకం*
Locale_కీలకం::createWithCanonicalFallback(స్థిరరాశి UnicodeString* primaryID,
                                       స్థిరరాశి UnicodeString* canonicalFallbackID,
                                       int32_t రకం,
                                       UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (primaryID == శూన్యసూచిక || U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    UnicodeString canonicalPrimaryID;
    LocaleUtility::canonicalLocaleString(primaryID, canonicalPrimaryID);
    తిరిగి కొత్త Locale_కీలకం(*primaryID, canonicalPrimaryID, canonicalFallbackID, రకం);
}

Locale_కీలకం::Locale_కీలకం(స్థిరరాశి UnicodeString& primaryID,
                     స్థిరరాశి UnicodeString& canonicalPrimaryID,
                     స్థిరరాశి UnicodeString* canonicalFallbackID,
                     int32_t రకం)
  : ICUService_కీలకం(primaryID)
  , రకం_2(రకం)
  , _primaryID(canonicalPrimaryID)
  , _fallbackID()
  , _currentID()
{
    _fallbackID.సమితి_గమ్యం_Bogus();
    ఒకవేళ (_primaryID.length() != 0) {
        ఒకవేళ (canonicalFallbackID != శూన్యసూచిక && _primaryID != *canonicalFallbackID) {
            _fallbackID = *canonicalFallbackID;
        }
    }

    _currentID = _primaryID;
}

Locale_కీలకం::~Locale_కీలకం() {}

UnicodeString&
Locale_కీలకం::ఉపసర్గ(UnicodeString& ఫలితం) స్థిరరాశి {
    ఒకవేళ (రకం_2 != రకం_ANY) {
        char16_t బఫరు[64];
        uprv_itou(బఫరు, 64, రకం_2, 10, 0);
        UnicodeString తాత్కాలిక(బఫరు);
        ఫలితం.append(తాత్కాలిక);
    }
    తిరిగి ఫలితం;
}

int32_t
Locale_కీలకం::రకం() స్థిరరాశి {
    తిరిగి రకం_2;
}

UnicodeString&
Locale_కీలకం::canonicalID(UnicodeString& ఫలితం) స్థిరరాశి {
    తిరిగి ఫలితం.append(_primaryID);
}

UnicodeString&
Locale_కీలకం::currentID(UnicodeString& ఫలితం) స్థిరరాశి {
    ఒకవేళ (!_currentID.isBogus()) {
        ఫలితం.append(_currentID);
    }
    తిరిగి ఫలితం;
}

UnicodeString&
Locale_కీలకం::currentDescriptor(UnicodeString& ఫలితం) స్థిరరాశి {
    ఒకవేళ (!_currentID.isBogus()) {
        ఉపసర్గ(ఫలితం).append(ఉపసర్గ_DELIMITER).append(_currentID);
    } లేకపోతే {
        ఫలితం.సమితి_గమ్యం_Bogus();
    }
    తిరిగి ఫలితం;
}

Locale&
Locale_కీలకం::canonicalLocale(Locale& ఫలితం) స్థిరరాశి {
    తిరిగి LocaleUtility::initLocaleFromName(_primaryID, ఫలితం);
}

Locale&
Locale_కీలకం::currentLocale(Locale& ఫలితం) స్థిరరాశి {
    తిరిగి LocaleUtility::initLocaleFromName(_currentID, ఫలితం);
}

UBool
Locale_కీలకం::fallback() {
    ఒకవేళ (!_currentID.isBogus()) {
        పూర్ణాంకం x = _currentID.చివరి_సూచీ_యొక్క(UNDERSCORE_CHAR);
        ఒకవేళ (x != -1) {
            _currentID.remove(x); // truncate current or fallback, whichever we're pointing to
            తిరిగి నిజం;
        }

        ఒకవేళ (!_fallbackID.isBogus()) {
            _currentID = _fallbackID;
            _fallbackID.సమితి_గమ్యం_Bogus();
            తిరిగి నిజం;
        }

        ఒకవేళ (_currentID.length() > 0) {
            _currentID.remove(0); // completely truncate
            తిరిగి నిజం;
        }

        _currentID.సమితి_గమ్యం_Bogus();
    }

    తిరిగి అబద్ధం;
}

UBool
Locale_కీలకం::is_Fallback_యొక్క(స్థిరరాశి UnicodeString& id) స్థిరరాశి {
    UnicodeString తాత్కాలిక(id);
    parseSuffix(తాత్కాలిక);
    తిరిగి తాత్కాలిక.సూచీ_యొక్క(_primaryID) == 0 &&
        (తాత్కాలిక.length() == _primaryID.length() ||
        తాత్కాలిక.charAt(_primaryID.length()) == UNDERSCORE_CHAR);
}

#నిర్వచితమైతే SERVICE_DEBUG
UnicodeString&
Locale_కీలకం::debug(UnicodeString& ఫలితం) స్థిరరాశి
{
    ICUService_కీలకం::debug(ఫలితం);
    ఫలితం.append((UnicodeString)" kind: ");
    ఫలితం.append(రకం_2);
    ఫలితం.append((UnicodeString)" primaryID: ");
    ఫలితం.append(_primaryID);
    ఫలితం.append((UnicodeString)" fallbackID: ");
    ఫలితం.append(_fallbackID);
    ఫలితం.append((UnicodeString)" currentID: ");
    ఫలితం.append(_currentID);
    తిరిగి ఫలితం;
}

UnicodeString&
Locale_కీలకం::debugClass(UnicodeString& ఫలితం) స్థిరరాశి
{
    తిరిగి ఫలితం.append((UnicodeString)"LocaleKey ");
}
#అయితే_ముగింపు

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_కీలకం)

U_NAMESPACE_చివర

/* !UCONFIG_NO_SERVICE */
#అయితే_ముగింపు


