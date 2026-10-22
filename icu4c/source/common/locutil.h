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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "hash.h"

#यदि !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
सार्वजनिक:
  स्थिर UnicodeString& canonicalLocaleString(अचर UnicodeString* id, UnicodeString& result);
  स्थिर Locale& initLocaleFromName(अचर UnicodeString& id, Locale& result);
  स्थिर UnicodeString& initNameFromLocale(अचर Locale& locale, UnicodeString& result);
  स्थिर अचर Hashtable* getAvailableLocaleNames(अचर UnicodeString& bundleID);
  स्थिर bool isFallbackOf(अचर UnicodeString& जरा, अचर UnicodeString& child);
};

U_NAMESPACE_END


#endif

#endif
