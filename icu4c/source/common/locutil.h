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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "hash.h"

#যদি !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
সার্বজনিক:
  স্থির UnicodeString& canonicalLocaleString(ধ্রুব UnicodeString* id, UnicodeString& result);
  স্থির Locale& initLocaleFromName(ধ্রুব UnicodeString& id, Locale& result);
  স্থির UnicodeString& initNameFromLocale(ধ্রুব Locale& locale, UnicodeString& result);
  স্থির ধ্রুব Hashtable* getAvailableLocaleNames(ধ্রুব UnicodeString& bundleID);
  স্থির bool isFallbackOf(ধ্রুব UnicodeString& মূল, ধ্রুব UnicodeString& child);
};

U_NAMESPACE_END


#endif

#endif
