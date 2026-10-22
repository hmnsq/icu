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

#ضمن "unicode/utypes.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  إذا(isBogus()) {
    أرجع *هذا;
  }

  char16_t *array = get_Array_شروع();
  UChar32 c;
  int32_t old_درازی = هذا->length();
  int32_t i = old_درازی, length;

  // first cut off trailing white space
  لكل(;;) {
    length = i;
    إذا(i <= 0) {
      اكسر;
    }
    U16_PREV(array, 0, i, c);
    إذا(!(c == 0x20 || u_isWhitespace(c))) {
      اكسر;
    }
  }
  إذا(length < old_درازی) {
    مجموعه_درازی(length);
  }

  // find leading white space
  int32_t شروع;
  i = 0;
  لكل(;;) {
    شروع = i;
    إذا(i >= length) {
      اكسر;
    }
    U16_بعدی(array, i, length, c);
    إذا(!(c == 0x20 || u_isWhitespace(c))) {
      اكسر;
    }
  }

  // move string forward over leading white space
  إذا(شروع > 0) {
    doReplace(0, شروع, اشاره_تهی, 0, 0);
  }

  أرجع *هذا;
}

U_NAMESPACE_انتها
