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

#شامل "unicode/utypes.h"
#شامل "unicode/uchar.h"
#شامل "unicode/unistr.h"
#شامل "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  اگر(isBogus()) {
    بازگشت *این;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t oldLength = این->length();
  int32_t i = oldLength, length;

  // first cut off trailing white space
  برای(;;) {
    length = i;
    اگر(i <= 0) {
      بشکن;
    }
    U16_PREV(array, 0, i, c);
    اگر(!(c == 0x20 || u_isWhitespace(c))) {
      بشکن;
    }
  }
  اگر(length < oldLength) {
    setLength(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  برای(;;) {
    start = i;
    اگر(i >= length) {
      بشکن;
    }
    U16_NEXT(array, i, length, c);
    اگر(!(c == 0x20 || u_isWhitespace(c))) {
      بشکن;
    }
  }

  // move string forward over leading white space
  اگر(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  بازگشت *این;
}

U_NAMESPACE_END
