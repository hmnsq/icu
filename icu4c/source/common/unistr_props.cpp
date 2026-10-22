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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  ಯದಿ(isBogus()) {
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t oldLength = ಏತತ್->length();
  int32_t i = oldLength, length;

  // first cut off trailing white space
  ಪ್ರತಿ(;;) {
    length = i;
    ಯದಿ(i <= 0) {
      ಭಙ್ಗ;
    }
    U16_PREV(array, 0, i, c);
    ಯದಿ(!(c == 0x20 || u_isWhitespace(c))) {
      ಭಙ್ಗ;
    }
  }
  ಯದಿ(length < oldLength) {
    setLength(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  ಪ್ರತಿ(;;) {
    start = i;
    ಯದಿ(i >= length) {
      ಭಙ್ಗ;
    }
    U16_NEXT(array, i, length, c);
    ಯದಿ(!(c == 0x20 || u_isWhitespace(c))) {
      ಭಙ್ಗ;
    }
  }

  // move string forward over leading white space
  ಯದಿ(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

U_NAMESPACE_END
