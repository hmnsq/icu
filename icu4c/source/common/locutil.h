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

#شامل "unicode/utypes.h"
#شامل "hash.h"

#اگر !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
عمومی:
  ایستا UnicodeString& canonicalLocaleString(ثابت UnicodeString* id, UnicodeString& result);
  ایستا Locale& initLocaleFromName(ثابت UnicodeString& id, Locale& result);
  ایستا UnicodeString& initNameFromLocale(ثابت Locale& locale, UnicodeString& result);
  ایستا ثابت Hashtable* getAvailableLocaleNames(ثابت UnicodeString& bundleID);
  ایستا bool isFallbackOf(ثابت UnicodeString& ریشه, ثابت UnicodeString& child);
};

U_NAMESPACE_END


#endif

#endif
