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
#اگر_تعریف_نشده LOCUTIL_H
#تعریف_کن LOCUTIL_H

#ضمن "unicode/utypes.h"
#ضمن "hash.h"

#إذا !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
عام:
  ساكن UnicodeString& canonicalLocaleString(ثابت UnicodeString* id, UnicodeString& نتیجه);
  ساكن Locale& initLocaleFromName(ثابت UnicodeString& id, Locale& نتیجه);
  ساكن UnicodeString& initNameFromLocale(ثابت Locale& locale, UnicodeString& نتیجه);
  ساكن ثابت Hashtable* get_موجود_Locale_نامها(ثابت UnicodeString& bundleID);
  ساكن bool is_Fallback_از(ثابت UnicodeString& ریشه, ثابت UnicodeString& فرزند);
};

U_NAMESPACE_انتها


#پایان_اگر

#پایان_اگر
