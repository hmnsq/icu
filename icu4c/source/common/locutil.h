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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "hash.h"

#ଯଦି !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
ସର୍ବସାଧାରଣ:
  ସ୍ଥିର UnicodeString& canonicalLocaleString(ଅଚଳ UnicodeString* id, UnicodeString& result);
  ସ୍ଥିର Locale& initLocaleFromName(ଅଚଳ UnicodeString& id, Locale& result);
  ସ୍ଥିର UnicodeString& initNameFromLocale(ଅଚଳ Locale& locale, UnicodeString& result);
  ସ୍ଥିର ଅଚଳ Hashtable* getAvailableLocaleNames(ଅଚଳ UnicodeString& bundleID);
  ସ୍ଥିର bool isFallbackOf(ଅଚଳ UnicodeString& ମୂଳ, ଅଚଳ UnicodeString& child);
};

U_NAMESPACE_END


#endif

#endif
