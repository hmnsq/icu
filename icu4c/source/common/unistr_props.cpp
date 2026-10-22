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

#include "unicode/utypes.h"
#include "unicode/uchar.h"
#include "unicode/unistr.h"
#include "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  якщо(isBogus()) {
    повернути *this;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t oldLength = this->length();
  int32_t i = oldLength, length;

  // first cut off trailing white space
  для(;;) {
    length = i;
    якщо(i <= 0) {
      break;
    }
    U16_PREV(array, 0, i, c);
    якщо(!(c == 0x20 || u_isWhitespace(c))) {
      break;
    }
  }
  якщо(length < oldLength) {
    setLength(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  для(;;) {
    start = i;
    якщо(i >= length) {
      break;
    }
    U16_NEXT(array, i, length, c);
    якщо(!(c == 0x20 || u_isWhitespace(c))) {
      break;
    }
  }

  // move string forward over leading white space
  якщо(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  повернути *this;
}

U_NAMESPACE_END
