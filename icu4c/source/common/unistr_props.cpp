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
    واپس *یہ;
  }

  char16_t *array = get_Array_آغاز();
  UChar32 c;
  int32_t old_لمبائی = یہ->length();
  int32_t i = old_لمبائی, length;

  // first cut off trailing white space
  برائے(;;) {
    length = i;
    اگر(i <= 0) {
      توڑو;
    }
    U16_پچھلا(array, 0, i, c);
    اگر(!(c == 0x20 || u_isWhitespace(c))) {
      توڑو;
    }
  }
  اگر(length < old_لمبائی) {
    سیٹ_لمبائی(length);
  }

  // find leading white space
  int32_t آغاز;
  i = 0;
  برائے(;;) {
    آغاز = i;
    اگر(i >= length) {
      توڑو;
    }
    U16_اگلا(array, i, length, c);
    اگر(!(c == 0x20 || u_isWhitespace(c))) {
      توڑو;
    }
  }

  // move string forward over leading white space
  اگر(آغاز > 0) {
    doReplace(0, آغاز, خالی_اشاریہ, 0, 0);
  }

  واپس *یہ;
}

U_NAMESPACE_انتہا
