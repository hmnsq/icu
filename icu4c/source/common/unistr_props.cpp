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

#ضمن "unicode/utypes.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  إذا(isBogus()) {
    أرجع *هذا;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t oldLength = هذا->length();
  int32_t i = oldLength, length;

  // first cut off trailing white space
  لكل(;;) {
    length = i;
    إذا(i <= 0) {
      اكسر;
    }
    U16_PREV(array, 0, i, c);
    إذا(!(c == 0x20 || u_isWhitespace(c))) {
      اكسر;
    }
  }
  إذا(length < oldLength) {
    setLength(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  لكل(;;) {
    start = i;
    إذا(i >= length) {
      اكسر;
    }
    U16_NEXT(array, i, length, c);
    إذا(!(c == 0x20 || u_isWhitespace(c))) {
      اكسر;
    }
  }

  // move string forward over leading white space
  إذا(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  أرجع *هذا;
}

U_NAMESPACE_END
