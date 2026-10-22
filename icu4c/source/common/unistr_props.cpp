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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  எனில்(isBogus()) {
    திரும்பு *இது;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t oldLength = இது->length();
  int32_t i = oldLength, length;

  // first cut off trailing white space
  க்கு(;;) {
    length = i;
    எனில்(i <= 0) {
      நிறுத்து;
    }
    U16_PREV(array, 0, i, c);
    எனில்(!(c == 0x20 || u_isWhitespace(c))) {
      நிறுத்து;
    }
  }
  எனில்(length < oldLength) {
    setLength(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  க்கு(;;) {
    start = i;
    எனில்(i >= length) {
      நிறுத்து;
    }
    U16_NEXT(array, i, length, c);
    எனில்(!(c == 0x20 || u_isWhitespace(c))) {
      நிறுத்து;
    }
  }

  // move string forward over leading white space
  எனில்(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  திரும்பு *இது;
}

U_NAMESPACE_END
