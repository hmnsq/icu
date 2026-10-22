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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  যদি(isBogus()) {
    ফেরত *এই;
  }

  char16_t *array = getArrayStart();
  UChar32 c;
  int32_t old_দৈর্ঘ্য = এই->length();
  int32_t i = old_দৈর্ঘ্য, length;

  // first cut off trailing white space
  জন্য(;;) {
    length = i;
    যদি(i <= 0) {
      বিরতি;
    }
    U16_পূর্ব(array, 0, i, c);
    যদি(!(c == 0x20 || u_isWhitespace(c))) {
      বিরতি;
    }
  }
  যদি(length < old_দৈর্ঘ্য) {
    set_দৈর্ঘ্য(length);
  }

  // find leading white space
  int32_t start;
  i = 0;
  জন্য(;;) {
    start = i;
    যদি(i >= length) {
      বিরতি;
    }
    U16_পরবর্তী(array, i, length, c);
    যদি(!(c == 0x20 || u_isWhitespace(c))) {
      বিরতি;
    }
  }

  // move string forward over leading white space
  যদি(start > 0) {
    doReplace(0, start, nullptr, 0, 0);
  }

  ফেরত *এই;
}

U_NAMESPACE_END
