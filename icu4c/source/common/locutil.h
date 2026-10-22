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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "hash.h"

#ಯದಿ !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
ಸಾರ್ವಜನಿಕ:
  ಸ್ಥಿರ UnicodeString& canonicalLocaleString(ಧ್ರುವ UnicodeString* id, UnicodeString& result);
  ಸ್ಥಿರ Locale& initLocaleFromName(ಧ್ರುವ UnicodeString& id, Locale& result);
  ಸ್ಥಿರ UnicodeString& initNameFromLocale(ಧ್ರುವ Locale& locale, UnicodeString& result);
  ಸ್ಥಿರ ಧ್ರುವ Hashtable* getAvailableLocaleNames(ಧ್ರುವ UnicodeString& bundleID);
  ಸ್ಥಿರ bool isFallbackOf(ಧ್ರುವ UnicodeString& ಬೇರು, ಧ್ರುವ UnicodeString& child);
};

U_NAMESPACE_END


#endif

#endif
