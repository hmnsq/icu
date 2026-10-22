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
#ifndef LOCUTIL_H
#define LOCUTIL_H

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "hash.h"

#જો !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
જાહેર:
  સ્થિર UnicodeString& canonicalLocaleString(અચલ UnicodeString* id, UnicodeString& result);
  સ્થિર Locale& initLocaleFromName(અચલ UnicodeString& id, Locale& result);
  સ્થિર UnicodeString& initNameFromLocale(અચલ Locale& locale, UnicodeString& result);
  સ્થિર અચલ Hashtable* getAvailableLocaleNames(અચલ UnicodeString& bundleID);
  સ્થિર bool isFallbackOf(અચલ UnicodeString& મૂળ, અચલ UnicodeString& child);
};

U_NAMESPACE_END


#endif

#endif
