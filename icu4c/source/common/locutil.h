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
#که_نه_تعریف_شوی LOCUTIL_H
#تعریف_کړه LOCUTIL_H

#ضمن "unicode/utypes.h"
#ضمن "hash.h"

#إذا !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
عام:
  ساكن UnicodeString& canonicalLocaleString(ثابت UnicodeString* id, UnicodeString& پایله);
  ساكن Locale& initLocaleFromName(ثابت UnicodeString& id, Locale& پایله);
  ساكن UnicodeString& initNameFromLocale(ثابت Locale& locale, UnicodeString& پایله);
  ساكن ثابت Hashtable* get_شته_Locale_نومونه(ثابت UnicodeString& bundleID);
  ساكن bool is_Fallback_د(ثابت UnicodeString& ریښه, ثابت UnicodeString& ماشوم);
};

U_NAMESPACE_پای_ټکی


#د_که_پای

#د_که_پای
