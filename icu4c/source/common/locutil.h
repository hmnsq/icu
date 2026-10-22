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
#اگر_غیر_تعریف_شدہ LOCUTIL_H
#تعریف_کرو LOCUTIL_H

#شامل "unicode/utypes.h"
#شامل "hash.h"

#اگر !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
عوامی:
  ساکن UnicodeString& canonicalLocaleString(ثابت UnicodeString* id, UnicodeString& نتیجہ);
  ساکن Locale& initLocaleFromName(ثابت UnicodeString& id, Locale& نتیجہ);
  ساکن UnicodeString& initNameFromLocale(ثابت Locale& locale, UnicodeString& نتیجہ);
  ساکن ثابت Hashtable* get_دستیاب_Locale_نام(ثابت UnicodeString& bundleID);
  ساکن bool is_Fallback_کا(ثابت UnicodeString& جڑ, ثابت UnicodeString& بچہ);
};

U_NAMESPACE_انتہا


#اگر_ختم

#اگر_ختم
