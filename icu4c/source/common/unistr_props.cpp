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

#समावेश "unicode/utypes.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  अगर(isBogus()) {
    वापस *यह;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t old_लंबाई = यह->length();
  int32_t i = old_लंबाई, length;

  // first cut off trailing white space
  क्रम(;;) {
    length = i;
    अगर(i <= 0) {
      अवरोध;
    }
    U16_पूर्व(array, 0, i, c);
    अगर(!(c == 0x20 || u_isWhitespace(c))) {
      अवरोध;
    }
  }
  अगर(length < old_लंबाई) {
    set_लंबाई(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  क्रम(;;) {
    start = i;
    अगर(i >= length) {
      अवरोध;
    }
    U16_अग्र(array, i, length, c);
    अगर(!(c == 0x20 || u_isWhitespace(c))) {
      अवरोध;
    }
  }

  // move string forward over leading white space
  अगर(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  वापस *यह;
}

U_NAMESPACE_END
