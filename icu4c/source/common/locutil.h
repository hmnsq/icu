// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2002-2005, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 *
 *******************************************************************************
 */
#నిర్వచితం_కాకపోతే LOCUTIL_H
#నిర్వచించు LOCUTIL_H

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "hash.h"

#ఒకవేళ !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
బహిరంగ:
  స్థిర UnicodeString& canonicalLocaleString(స్థిరరాశి UnicodeString* id, UnicodeString& ఫలితం);
  స్థిర Locale& initLocaleFromName(స్థిరరాశి UnicodeString& id, Locale& ఫలితం);
  స్థిర UnicodeString& initNameFromLocale(స్థిరరాశి Locale& locale, UnicodeString& ఫలితం);
  స్థిర స్థిరరాశి Hashtable* get_అందుబాటు_Locale_పేర్లు(స్థిరరాశి UnicodeString& bundleID);
  స్థిర bool is_Fallback_యొక్క(స్థిరరాశి UnicodeString& మూలం, స్థిరరాశి UnicodeString& బిడ్డ);
};

U_NAMESPACE_చివర


#అయితే_ముగింపు

#అయితే_ముగింపు
