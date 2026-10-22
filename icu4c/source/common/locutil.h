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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "hash.h"

#যদি !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
সার্ৱজনিক:
  স্থির UnicodeString& canonicalLocaleString(ধ্রুৱ UnicodeString* id, UnicodeString& result);
  স্থির Locale& initLocaleFromName(ধ্রুৱ UnicodeString& id, Locale& result);
  স্থির UnicodeString& initNameFromLocale(ধ্রুৱ Locale& locale, UnicodeString& result);
  স্থির ধ্রুৱ Hashtable* getAvailableLocaleNames(ধ্রুৱ UnicodeString& bundleID);
  স্থির bool isFallbackOf(ধ্রুৱ UnicodeString& মূল, ধ্রুৱ UnicodeString& child);
};

U_NAMESPACE_END


#endif

#endif
