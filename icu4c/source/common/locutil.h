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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ LOCUTIL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOCUTIL_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hash.h"

#ਜੇ !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
ਜਨਤਕ:
  ਸਥਿਰ UnicodeString& canonicalLocaleString(ਅਚਲ UnicodeString* id, UnicodeString& ਨਤੀਜਾ);
  ਸਥਿਰ Locale& initLocaleFromName(ਅਚਲ UnicodeString& id, Locale& ਨਤੀਜਾ);
  ਸਥਿਰ UnicodeString& initNameFromLocale(ਅਚਲ Locale& locale, UnicodeString& ਨਤੀਜਾ);
  ਸਥਿਰ ਅਚਲ Hashtable* get_ਉਪਲਬਧ_Locale_ਨਾਂ(ਅਚਲ UnicodeString& bundleID);
  ਸਥਿਰ bool is_Fallback_ਦਾ(ਅਚਲ UnicodeString& ਜੜ੍ਹ, ਅਚਲ UnicodeString& ਬੱਚਾ);
};

U_NAMESPACE_ਸਿਰਾ


#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
