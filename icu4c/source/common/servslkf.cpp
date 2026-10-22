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

/*
 ******************************************************************
 */

Simple_Locale_కీలకం_Factory::Simple_Locale_కీలకం_Factory(UObject* obj_గమ్యం_Adopt,
                                               స్థిరరాశి UnicodeString& locale,
                                               int32_t రకం,
                                               int32_t coverage)
  : Locale_కీలకం_Factory(coverage)
  , _obj(obj_గమ్యం_Adopt)
  , _id(locale)
  , రకం_2(రకం)
{
}

Simple_Locale_కీలకం_Factory::Simple_Locale_కీలకం_Factory(UObject* obj_గమ్యం_Adopt,
                                               స్థిరరాశి Locale& locale,
                                               int32_t రకం,
                                               int32_t coverage)
  : Locale_కీలకం_Factory(coverage)
  , _obj(obj_గమ్యం_Adopt)
  , _id()
  , రకం_2(రకం)
{
    LocaleUtility::initNameFromLocale(locale, _id);
}

Simple_Locale_కీలకం_Factory::~Simple_Locale_కీలకం_Factory()
{
  తొలగించు _obj;
  _obj = శూన్యసూచిక;
}

UObject*
Simple_Locale_కీలకం_Factory::create(స్థిరరాశి ICUService_కీలకం& కీలకం, స్థిరరాశి ICUService* service, UError_సంకేతం& స్థితి_2) స్థిరరాశి
{
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        స్థిరరాశి Locale_కీలకం& lkey = static_cast<స్థిరరాశి Locale_కీలకం&>(కీలకం);
        ఒకవేళ (రకం_2 == Locale_కీలకం::రకం_ANY || రకం_2 == lkey.రకం()) {
            UnicodeString కీలకం_ID;
            lkey.currentID(కీలకం_ID);
            ఒకవేళ (_id == కీలకం_ID) {
                తిరిగి service->cloneInstance(_obj);
            }
        }
    }
    తిరిగి శూన్యసూచిక;
}

//UBool
//SimpleLocaleKeyFactory::isSupportedID(const UnicodeString& id, UErrorCode& /* status */) const
//{
//    return id == _id;
//}

శూన్య
Simple_Locale_కీలకం_Factory::updateVisibleIDs(Hashtable& ఫలితం, UError_సంకేతం& స్థితి_2) స్థిరరాశి
{
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        ఒకవేళ (_coverage & 0x1) {
            ఫలితం.remove(_id);
        } లేకపోతే {
            ఫలితం.put(_id, (శూన్య*)ఇది, స్థితి_2);
        }
    }
}

#నిర్వచితమైతే SERVICE_DEBUG
UnicodeString&
Simple_Locale_కీలకం_Factory::debug(UnicodeString& ఫలితం) స్థిరరాశి
{
    Locale_కీలకం_Factory::debug(ఫలితం);
    ఫలితం.append((UnicodeString)", id: ");
    ఫలితం.append(_id);
    ఫలితం.append((UnicodeString)", kind: ");
    ఫలితం.append(రకం_2);
    తిరిగి ఫలితం;
}

UnicodeString&
Simple_Locale_కీలకం_Factory::debugClass(UnicodeString& ఫలితం) స్థిరరాశి
{
    తిరిగి ఫలితం.append((UnicodeString)"SimpleLocaleKeyFactory");
}
#అయితే_ముగింపు

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Simple_Locale_కీలకం_Factory)

U_NAMESPACE_చివర

/* !UCONFIG_NO_SERVICE */
#అయితే_ముగింపు


