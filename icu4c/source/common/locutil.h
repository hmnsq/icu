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

#जर !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
सार्वजनिक:
  स्थिर UnicodeString& canonicalLocaleString(स्थिरांक UnicodeString* id, UnicodeString& result);
  स्थिर Locale& initLocaleFromName(स्थिरांक UnicodeString& id, Locale& result);
  स्थिर UnicodeString& initNameFromLocale(स्थिरांक Locale& locale, UnicodeString& result);
  स्थिर स्थिरांक Hashtable* getAvailableLocaleNames(स्थिरांक UnicodeString& bundleID);
  स्थिर bool isFallbackOf(स्थिरांक UnicodeString& मूळ, स्थिरांक UnicodeString& child);
};

U_NAMESPACE_END


#endif

#endif
