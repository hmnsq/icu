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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ LOCUTIL_H
#ವ್ಯಾಖ್ಯಾನಿಸು LOCUTIL_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "hash.h"

#ಆದರೆ !UCONFIG_NO_SERVICE || !UCONFIG_NO_TRANSLITERATION


U_NAMESPACE_BEGIN

// temporary utility functions, till I know where to find them
// in header so tests can also access them

class U_COMMON_API LocaleUtility {
ಸಾರ್ವಜನಿಕ:
  ಸ್ಥಿರ UnicodeString& canonicalLocaleString(ಸ್ಥಿರಾಂಕ UnicodeString* id, UnicodeString& ಫಲಿತಾಂಶ);
  ಸ್ಥಿರ Locale& initLocaleFromName(ಸ್ಥಿರಾಂಕ UnicodeString& id, Locale& ಫಲಿತಾಂಶ);
  ಸ್ಥಿರ UnicodeString& initNameFromLocale(ಸ್ಥಿರಾಂಕ Locale& locale, UnicodeString& ಫಲಿತಾಂಶ);
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Hashtable* get_ಲಭ್ಯ_Locale_ಹೆಸರುಗಳು(ಸ್ಥಿರಾಂಕ UnicodeString& bundleID);
  ಸ್ಥಿರ bool is_Fallback_ದ(ಸ್ಥಿರಾಂಕ UnicodeString& ಬೇರು, ಸ್ಥಿರಾಂಕ UnicodeString& ಮಗು);
};

U_NAMESPACE_ಕೊನೆ


#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ
