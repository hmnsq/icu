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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "hash.h"

#எனில் !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
பொது:
  நிலையான UnicodeString& canonicalLocaleString(மாறிலி UnicodeString* id, UnicodeString& result);
  நிலையான Locale& initLocaleFromName(மாறிலி UnicodeString& id, Locale& result);
  நிலையான UnicodeString& initNameFromLocale(மாறிலி Locale& locale, UnicodeString& result);
  நிலையான மாறிலி Hashtable* getAvailableLocaleNames(மாறிலி UnicodeString& bundleID);
  நிலையான bool isFallbackOf(மாறிலி UnicodeString& வேர், மாறிலி UnicodeString& child);
};

U_NAMESPACE_END


#endif

#endif
