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
  यदि(isBogus()) {
    फर्काऊ *एतत्;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t oldLength = एतत्->length();
  int32_t i = oldLength, length;

  // first cut off trailing white space
  लागि(;;) {
    length = i;
    यदि(i <= 0) {
      रोक;
    }
    U16_PREV(array, 0, i, c);
    यदि(!(c == 0x20 || u_isWhitespace(c))) {
      रोक;
    }
  }
  यदि(length < oldLength) {
    setLength(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  लागि(;;) {
    start = i;
    यदि(i >= length) {
      रोक;
    }
    U16_NEXT(array, i, length, c);
    यदि(!(c == 0x20 || u_isWhitespace(c))) {
      रोक;
    }
  }

  // move string forward over leading white space
  यदि(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  फर्काऊ *एतत्;
}

U_NAMESPACE_END
