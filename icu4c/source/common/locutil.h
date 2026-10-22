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
#अगर_अघोषित LOCUTIL_H
#घोषणा LOCUTIL_H

#समावेश "unicode/utypes.h"
#समावेश "hash.h"

#अगर !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
खुला:
  अटल UnicodeString& canonicalLocaleString(स्थिर UnicodeString* id, UnicodeString& परिणाम);
  अटल Locale& initLocaleFromName(स्थिर UnicodeString& id, Locale& परिणाम);
  अटल UnicodeString& initNameFromLocale(स्थिर Locale& locale, UnicodeString& परिणाम);
  अटल स्थिर Hashtable* getAvailableLocaleNames(स्थिर UnicodeString& bundleID);
  अटल bool isFallbackOf(स्थिर UnicodeString& मूल, स्थिर UnicodeString& child);
};

U_NAMESPACE_END


#पूर्ण_अगर

#पूर्ण_अगर
