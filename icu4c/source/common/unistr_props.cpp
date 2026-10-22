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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  ਜੇ(isBogus()) {
    ਵਾਪਸ *ਇਹ;
  }

  char16_t *array = get_Array_ਸ਼ੁਰੂ();
  UChar32 c;
  int32_t old_ਲੰਮਾਈ = ਇਹ->length();
  int32_t i = old_ਲੰਮਾਈ, length;

  // first cut off trailing white space
  ਲਈ(;;) {
    length = i;
    ਜੇ(i <= 0) {
      ਰੋਕੋ;
    }
    U16_PREV(array, 0, i, c);
    ਜੇ(!(c == 0x20 || u_isWhitespace(c))) {
      ਰੋਕੋ;
    }
  }
  ਜੇ(length < old_ਲੰਮਾਈ) {
    ਸੈੱਟ_ਲੰਮਾਈ(length);
  }

  // find leading white space
  int32_t ਸ਼ੁਰੂ;
  i = 0;
  ਲਈ(;;) {
    ਸ਼ੁਰੂ = i;
    ਜੇ(i >= length) {
      ਰੋਕੋ;
    }
    U16_ਅਗਲਾ(array, i, length, c);
    ਜੇ(!(c == 0x20 || u_isWhitespace(c))) {
      ਰੋਕੋ;
    }
  }

  // move string forward over leading white space
  ਜੇ(ਸ਼ੁਰੂ > 0) {
    doReplace(0, ਸ਼ੁਰੂ, ਖਾਲੀ_ਸੂਚਕ, 0, 0);
  }

  ਵਾਪਸ *ਇਹ;
}

U_NAMESPACE_ਸਿਰਾ
