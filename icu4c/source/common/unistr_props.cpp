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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/utf16.h"

U_NAMESPACE_BEGIN

UnicodeString& 
UnicodeString::trim() U_LIFETIME_BOUND
{
  ఒకవేళ(isBogus()) {
    తిరిగి *ఇది;
  }

  char16_t *array = get_Array_ప్రారంభం();
  UChar32 c;
  int32_t old_నిడివి = ఇది->length();
  int32_t i = old_నిడివి, length;

  // first cut off trailing white space
  కొరకు(;;) {
    length = i;
    ఒకవేళ(i <= 0) {
      ఆపు;
    }
    U16_PREV(array, 0, i, c);
    ఒకవేళ(!(c == 0x20 || u_isWhitespace(c))) {
      ఆపు;
    }
  }
  ఒకవేళ(length < old_నిడివి) {
    సమితి_నిడివి(length);
  }

  // find leading white space
  int32_t ప్రారంభం;
  i = 0;
  కొరకు(;;) {
    ప్రారంభం = i;
    ఒకవేళ(i >= length) {
      ఆపు;
    }
    U16_తదుపరి(array, i, length, c);
    ఒకవేళ(!(c == 0x20 || u_isWhitespace(c))) {
      ఆపు;
    }
  }

  // move string forward over leading white space
  ఒకవేళ(ప్రారంభం > 0) {
    doReplace(0, ప్రారంభం, శూన్యసూచిక, 0, 0);
  }

  తిరిగి *ఇది;
}

U_NAMESPACE_చివర
