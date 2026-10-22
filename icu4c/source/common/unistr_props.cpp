// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  unistr_props.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:2
*
*   created on: 2004aug25
*   created by: Markus W. Scherer
*
*   Character property dependent functions moved here from unistr.cpp
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  ಆದರೆ(isBogus()) {
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  char16_t *array = get_Array_ಆರಂಭ();
  UChar32 c;
  int32_t old_ಉದ್ದಳತೆ = ಇದು->length();
  int32_t i = old_ಉದ್ದಳತೆ, length;

  // first cut off trailing white space
  ಗಾಗಿ(;;) {
    length = i;
    ಆದರೆ(i <= 0) {
      ನಿಲ್ಲಿಸು;
    }
    U16_PREV(array, 0, i, c);
    ಆದರೆ(!(c == 0x20 || u_isWhitespace(c))) {
      ನಿಲ್ಲಿಸು;
    }
  }
  ಆದರೆ(length < old_ಉದ್ದಳತೆ) {
    ಗಣ_ಉದ್ದಳತೆ(length);
  }

  // find leading white space
  int32_t ಆರಂಭ;
  i = 0;
  ಗಾಗಿ(;;) {
    ಆರಂಭ = i;
    ಆದರೆ(i >= length) {
      ನಿಲ್ಲಿಸು;
    }
    U16_ಮುಂದಿನ(array, i, length, c);
    ಆದರೆ(!(c == 0x20 || u_isWhitespace(c))) {
      ನಿಲ್ಲಿಸು;
    }
  }

  // move string forward over leading white space
  ಆದರೆ(ಆರಂಭ > 0) {
    doReplace(0, ಆರಂಭ, ಶೂನ್ಯಸೂಚಕ, 0, 0);
  }

  ಹಿಂತಿರುಗಿ *ಇದು;
}

U_NAMESPACE_ಕೊನೆ
