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
  གལ་སྲིད(isBogus()) {
    ལོག *this;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t oldLength = this->length();
  int32_t i = oldLength, length;

  // first cut off trailing white space
  དོན་ལུ(;;) {
    length = i;
    གལ་སྲིད(i <= 0) {
      bha_nga;
    }
    U16_PREV(array, 0, i, c);
    གལ་སྲིད(!(c == 0x20 || u_isWhitespace(c))) {
      bha_nga;
    }
  }
  གལ་སྲིད(length < oldLength) {
    setLength(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  དོན་ལུ(;;) {
    start = i;
    གལ་སྲིད(i >= length) {
      bha_nga;
    }
    U16_NEXT(array, i, length, c);
    གལ་སྲིད(!(c == 0x20 || u_isWhitespace(c))) {
      bha_nga;
    }
  }

  // move string forward over leading white space
  གལ་སྲིད(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  ལོག *this;
}

U_NAMESPACE_END
