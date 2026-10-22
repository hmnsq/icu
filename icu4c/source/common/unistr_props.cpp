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
  אם(isBogus()) {
    החזר *هذا;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t oldLength = هذا->length();
  int32_t i = oldLength, length;

  // first cut off trailing white space
  עבור(;;) {
    length = i;
    אם(i <= 0) {
      اكسر;
    }
    U16_PREV(array, 0, i, c);
    אם(!(c == 0x20 || u_isWhitespace(c))) {
      اكسر;
    }
  }
  אם(length < oldLength) {
    setLength(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  עבור(;;) {
    start = i;
    אם(i >= length) {
      اكسر;
    }
    U16_NEXT(array, i, length, c);
    אם(!(c == 0x20 || u_isWhitespace(c))) {
      اكسر;
    }
  }

  // move string forward over leading white space
  אם(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  החזר *هذا;
}

U_NAMESPACE_END
