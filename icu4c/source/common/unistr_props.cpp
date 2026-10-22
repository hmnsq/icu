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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  ଯଦି(isBogus()) {
    ଫେରନ୍ତୁ *ଏହି;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t oldLength = ଏହି->length();
  int32_t i = oldLength, length;

  // first cut off trailing white space
  ପାଇଁ(;;) {
    length = i;
    ଯଦି(i <= 0) {
      ଭାଙ୍ଗ;
    }
    U16_PREV(array, 0, i, c);
    ଯଦି(!(c == 0x20 || u_isWhitespace(c))) {
      ଭାଙ୍ଗ;
    }
  }
  ଯଦି(length < oldLength) {
    setLength(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  ପାଇଁ(;;) {
    start = i;
    ଯଦି(i >= length) {
      ଭାଙ୍ଗ;
    }
    U16_NEXT(array, i, length, c);
    ଯଦି(!(c == 0x20 || u_isWhitespace(c))) {
      ଭାଙ୍ଗ;
    }
  }

  // move string forward over leading white space
  ଯଦି(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  ଫେରନ୍ତୁ *ଏହି;
}

U_NAMESPACE_END
