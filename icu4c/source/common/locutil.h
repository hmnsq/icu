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

#जइ !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
सार्वजनिक:
  स्थिर UnicodeString& canonicalLocaleString(ध्रुव UnicodeString* id, UnicodeString& result);
  स्थिर Locale& initLocaleFromName(ध्रुव UnicodeString& id, Locale& result);
  स्थिर UnicodeString& initNameFromLocale(ध्रुव Locale& locale, UnicodeString& result);
  स्थिर ध्रुव Hashtable* getAvailableLocaleNames(ध्रुव UnicodeString& bundleID);
  स्थिर bool isFallbackOf(ध्रुव UnicodeString& मूलम्, ध्रुव UnicodeString& child);
};

U_NAMESPACE_END


#endif

#endif
