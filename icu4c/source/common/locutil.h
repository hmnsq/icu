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
#جيڪڏهن_وصف_نه_ٿيل LOCUTIL_H
#وصف_ڪريو LOCUTIL_H

#ضمن "unicode/utypes.h"
#ضمن "hash.h"

#إذا !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
عام:
  ساكن UnicodeString& canonicalLocaleString(ثابت UnicodeString* id, UnicodeString& نتيجو);
  ساكن Locale& initLocaleFromName(ثابت UnicodeString& id, Locale& نتيجو);
  ساكن UnicodeString& initNameFromLocale(ثابت Locale& locale, UnicodeString& نتيجو);
  ساكن ثابت Hashtable* get_موجود_Locale_نالا(ثابت UnicodeString& bundleID);
  ساكن bool is_Fallback_جو(ثابت UnicodeString& پاڙ, ثابت UnicodeString& ٻار);
};

U_NAMESPACE_پڇاڙي


#جيڪڏهن_پورو

#جيڪڏهن_پورو
