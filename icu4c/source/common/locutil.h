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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "hash.h"

#എങ്കിൽ !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
പൊതു:
  സ്ഥിതം UnicodeString& canonicalLocaleString(സ്ഥിരം UnicodeString* id, UnicodeString& result);
  സ്ഥിതം Locale& initLocaleFromName(സ്ഥിരം UnicodeString& id, Locale& result);
  സ്ഥിതം UnicodeString& initNameFromLocale(സ്ഥിരം Locale& locale, UnicodeString& result);
  സ്ഥിതം സ്ഥിരം Hashtable* getAvailableLocaleNames(സ്ഥിരം UnicodeString& bundleID);
  സ്ഥിതം bool isFallbackOf(സ്ഥിരം UnicodeString& വേര്, സ്ഥിരം UnicodeString& child);
};

U_NAMESPACE_END


#endif

#endif
