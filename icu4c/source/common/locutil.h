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
সর্বজনীন:
  স্থির UnicodeString& canonicalLocaleString(ধ্রুবক UnicodeString* id, UnicodeString& ফলাফল);
  স্থির Locale& initLocaleFromName(ধ্রুবক UnicodeString& id, Locale& ফলাফল);
  স্থির UnicodeString& initNameFromLocale(ধ্রুবক Locale& locale, UnicodeString& ফলাফল);
  স্থির ধ্রুবক Hashtable* getAvailableLocaleNames(ধ্রুবক UnicodeString& bundleID);
  স্থির bool isFallbackOf(ধ্রুবক UnicodeString& মূল, ধ্রুবক UnicodeString& child);
};

U_NAMESPACE_END


#endif

#endif
