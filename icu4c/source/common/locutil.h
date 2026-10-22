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
#جے_بنا_تعریف LOCUTIL_H
#تعریف_کرو LOCUTIL_H

#ضمن "unicode/utypes.h"
#ضمن "hash.h"

#إذا !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
عام:
  ساكن UnicodeString& canonicalLocaleString(ثابت UnicodeString* id, UnicodeString& نتیجہ);
  ساكن Locale& initLocaleFromName(ثابت UnicodeString& id, Locale& نتیجہ);
  ساكن UnicodeString& initNameFromLocale(ثابت Locale& locale, UnicodeString& نتیجہ);
  ساكن ثابت Hashtable* get_دستیاب_Locale_ناں(ثابت UnicodeString& bundleID);
  ساكن bool is_Fallback_دا(ثابت UnicodeString& جڑ, ثابت UnicodeString& بچہ);
};

U_NAMESPACE_سرا


#جے_انت

#جے_انت
