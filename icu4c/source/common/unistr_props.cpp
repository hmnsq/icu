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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/uchar.h"
#অন্তর্ভাৱ "unicode/unistr.h"
#অন্তর্ভাৱ "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  যদি(isBogus()) {
    ঘূৰাই *এতত্;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t oldLength = এতত্->length();
  int32_t i = oldLength, length;

  // first cut off trailing white space
  বাবে(;;) {
    length = i;
    যদি(i <= 0) {
      ভঙ্গ;
    }
    U16_PREV(array, 0, i, c);
    যদি(!(c == 0x20 || u_isWhitespace(c))) {
      ভঙ্গ;
    }
  }
  যদি(length < oldLength) {
    setLength(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  বাবে(;;) {
    start = i;
    যদি(i >= length) {
      ভঙ্গ;
    }
    U16_NEXT(array, i, length, c);
    যদি(!(c == 0x20 || u_isWhitespace(c))) {
      ভঙ্গ;
    }
  }

  // move string forward over leading white space
  যদি(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  ঘূৰাই *এতত্;
}

U_NAMESPACE_END
