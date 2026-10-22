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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  જો(isBogus()) {
    પાછા *આ;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t oldLength = આ->length();
  int32_t i = oldLength, length;

  // first cut off trailing white space
  માટે(;;) {
    length = i;
    જો(i <= 0) {
      તોડો;
    }
    U16_PREV(array, 0, i, c);
    જો(!(c == 0x20 || u_isWhitespace(c))) {
      તોડો;
    }
  }
  જો(length < oldLength) {
    setLength(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  માટે(;;) {
    start = i;
    જો(i >= length) {
      તોડો;
    }
    U16_NEXT(array, i, length, c);
    જો(!(c == 0x20 || u_isWhitespace(c))) {
      તોડો;
    }
  }

  // move string forward over leading white space
  જો(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  પાછા *આ;
}

U_NAMESPACE_END
