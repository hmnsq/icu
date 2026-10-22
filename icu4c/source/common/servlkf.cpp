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
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "uassert.h"

#నిర్వచించు UNDERSCORE_CHAR ((char16_t)0x005f)
#నిర్వచించు AT_SIGN_CHAR    ((char16_t)64)
#నిర్వచించు PERIOD_CHAR     ((char16_t)46)


U_NAMESPACE_BEGIN

Locale_కీలకం_Factory::Locale_కీలకం_Factory(int32_t coverage)
  : _name()
  , _coverage(coverage)
{
}

Locale_కీలకం_Factory::Locale_కీలకం_Factory(int32_t coverage, స్థిరరాశి UnicodeString& name)
  : _name(name)
  , _coverage(coverage)
{
}

Locale_కీలకం_Factory::~Locale_కీలకం_Factory() {
}

UObject*
Locale_కీలకం_Factory::create(స్థిరరాశి ICUService_కీలకం& కీలకం, స్థిరరాశి ICUService* service, UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    ఒకవేళ (handles_కీలకం(కీలకం, స్థితి_2)) {
        స్థిరరాశి Locale_కీలకం& lkey = static_cast<స్థిరరాశి Locale_కీలకం&>(కీలకం);
        int32_t రకం = lkey.రకం();
        Locale loc;
        lkey.currentLocale(loc);

        తిరిగి handleCreate(loc, రకం, service, స్థితి_2);
    }
    తిరిగి శూన్యసూచిక;
}

UBool
Locale_కీలకం_Factory::handles_కీలకం(స్థిరరాశి ICUService_కీలకం& కీలకం, UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    స్థిరరాశి Hashtable* supported = getSupportedIDs(స్థితి_2);
    ఒకవేళ (supported) {
        UnicodeString id;
        కీలకం.currentID(id);
        తిరిగి supported->get(id) != శూన్యసూచిక;
    }
    తిరిగి అబద్ధం;
}

శూన్య
Locale_కీలకం_Factory::updateVisibleIDs(Hashtable& ఫలితం, UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    స్థిరరాశి Hashtable* supported = getSupportedIDs(స్థితి_2);
    ఒకవేళ (supported) {
        UBool visible = (_coverage & 0x1) == 0;
        స్థిరరాశి UHashElement* elem = శూన్యసూచిక;
        int32_t pos = UHASH_మొదటి;
        అయితే ((elem = supported->తదుపరి_Element(pos)) != శూన్యసూచిక) {
            స్థిరరాశి UnicodeString& id = *static_cast<స్థిరరాశి UnicodeString*>(elem->కీలకం.సూచిక);
            ఒకవేళ (!visible) {
                ఫలితం.remove(id);
            } లేకపోతే {
                ఫలితం.put(id, (శూన్య*)ఇది, స్థితి_2); // this is dummy non-void marker used for set semantics
                ఒకవేళ (U_FAILURE(స్థితి_2)) {
                    ఆపు;
                }
            }
        }
    }
}

UnicodeString&
Locale_కీలకం_Factory::getDisplayName(స్థిరరాశి UnicodeString& id, స్థిరరాశి Locale& locale, UnicodeString& ఫలితం) స్థిరరాశి {
    ఒకవేళ ((_coverage & 0x1) == 0) {
        //UErrorCode status = U_ZERO_ERROR;
        // assume if this is called on us, we support some fallback of this id
        // if (isSupportedID(id, status)) {
            Locale loc;
            LocaleUtility::initLocaleFromName(id, loc);
            తిరిగి loc.getDisplayName(locale, ఫలితం);
        // }
    }
    ఫలితం.సమితి_గమ్యం_Bogus();
    తిరిగి ఫలితం;
}

UObject*
Locale_కీలకం_Factory::handleCreate(స్థిరరాశి Locale& /* loc */, 
                   int32_t /*రకం*/, 
                   స్థిరరాశి ICUService* /* service */, 
                   UError_సంకేతం& /* status */) స్థిరరాశి {
    తిరిగి శూన్యసూచిక;
}

//UBool
//LocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& status) const {
//    const Hashtable* ids = getSupportedIDs(status);
//    return ids && ids->get(id);
//}

స్థిరరాశి Hashtable*
Locale_కీలకం_Factory::getSupportedIDs(UError_సంకేతం& /* status */) స్థిరరాశి {
    తిరిగి శూన్యసూచిక;
}

#నిర్వచితమైతే SERVICE_DEBUG
UnicodeString&
Locale_కీలకం_Factory::debug(UnicodeString& ఫలితం) స్థిరరాశి
{
    debugClass(ఫలితం);
    ఫలితం.append((UnicodeString)", name: ");
    ఫలితం.append(_name);
    ఫలితం.append((UnicodeString)", coverage: ");
    ఫలితం.append(_coverage);
    తిరిగి ఫలితం;
}

UnicodeString&
Locale_కీలకం_Factory::debugClass(UnicodeString& ఫలితం) స్థిరరాశి
{
  తిరిగి ఫలితం.append((UnicodeString)"LocaleKeyFactory");
}
#అయితే_ముగింపు

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Locale_కీలకం_Factory)

U_NAMESPACE_చివర

/* !UCONFIG_NO_SERVICE */
#అయితే_ముగింపు


