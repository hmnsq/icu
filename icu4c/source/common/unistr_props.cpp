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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  जर(isBogus()) {
    परत *हे;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t oldLength = हे->length();
  int32_t i = oldLength, length;

  // first cut off trailing white space
  साठी(;;) {
    length = i;
    जर(i <= 0) {
      थांबा;
    }
    U16_PREV(array, 0, i, c);
    जर(!(c == 0x20 || u_isWhitespace(c))) {
      थांबा;
    }
  }
  जर(length < oldLength) {
    setLength(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  साठी(;;) {
    start = i;
    जर(i >= length) {
      थांबा;
    }
    U16_NEXT(array, i, length, c);
    जर(!(c == 0x20 || u_isWhitespace(c))) {
      थांबा;
    }
  }

  // move string forward over leading white space
  जर(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  परत *हे;
}

U_NAMESPACE_END
