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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  എങ്കിൽ(isBogus()) {
    തിരികെ *ഇത്;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t oldLength = ഇത്->length();
  int32_t i = oldLength, length;

  // first cut off trailing white space
  വേണ്ടി(;;) {
    length = i;
    എങ്കിൽ(i <= 0) {
      നിർത്തുക;
    }
    U16_PREV(array, 0, i, c);
    എങ്കിൽ(!(c == 0x20 || u_isWhitespace(c))) {
      നിർത്തുക;
    }
  }
  എങ്കിൽ(length < oldLength) {
    setLength(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  വേണ്ടി(;;) {
    start = i;
    എങ്കിൽ(i >= length) {
      നിർത്തുക;
    }
    U16_NEXT(array, i, length, c);
    എങ്കിൽ(!(c == 0x20 || u_isWhitespace(c))) {
      നിർത്തുക;
    }
  }

  // move string forward over leading white space
  എങ്കിൽ(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  തിരികെ *ഇത്;
}

U_NAMESPACE_END
