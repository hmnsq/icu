// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(స్థిరరాశి అక్షరం* str U_LIFETIME_BOUND)
    : ptr_(str), నిడివి_2((str == శూన్యసూచిక) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(స్థిరరాశి StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  ఒకవేళ (pos < 0) {
    pos = 0;
  } లేకపోతే ఒకవేళ (pos > x.నిడివి_2) {
    pos = x.నిడివి_2;
  }
  ptr_ = x.ptr_ + pos;
  నిడివి_2 = x.నిడివి_2 - pos;
}

StringPiece::StringPiece(స్థిరరాశి StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  ఒకవేళ (pos < 0) {
    pos = 0;
  } లేకపోతే ఒకవేళ (pos > x.నిడివి_2) {
    pos = x.నిడివి_2;
  }
  ఒకవేళ (len < 0) {
    len = 0;
  } లేకపోతే ఒకవేళ (len > x.నిడివి_2 - pos) {
    len = x.నిడివి_2 - pos;
  }
  ptr_ = x.ptr_ + pos;
  నిడివి_2 = len;
}

శూన్య StringPiece::సమితి(స్థిరరాశి అక్షరం* str) {
  ptr_ = str;
  ఒకవేళ (str != శూన్యసూచిక)
    నిడివి_2 = static_cast<int32_t>(uprv_strlen(str));
  లేకపోతే
    నిడివి_2 = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t స్థానభ్రంశం) {
  ఒకవేళ (length() == 0 && needle.length() == 0) {
    తిరిగి 0;
  }
  // TODO: Improve to be better than O(N^2)?
  కొరకు (int32_t i = స్థానభ్రంశం; i < length(); i++) {
    int32_t j = 0;
    కొరకు (; j < needle.length(); i++, j++) {
      ఒకవేళ (దత్తాంశం()[i] != needle.దత్తాంశం()[j]) {
        i -= j;
        వెళ్ళు outer_చివర_2;
      }
    }
    తిరిగి i - j;
    outer_చివర_2: శూన్య();
  }
  తిరిగి -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  కొరకు (; i < length(); i++) {
    ఒకవేళ (i == other.length()) {
      // this is longer
      తిరిగి 1;
    }
    అక్షరం a = దత్తాంశం()[i];
    అక్షరం b = other.దత్తాంశం()[i];
    ఒకవేళ (a < b) {
      తిరిగి -1;
    } లేకపోతే ఒకవేళ (a > b) {
      తిరిగి 1;
    }
  }
  ఒకవేళ (i < other.length()) {
    // other is longer
    తిరిగి -1;
  }
  తిరిగి 0;
}

U_EXPORT UBool U_EXPORT2
క్రియాకారకం==(స్థిరరాశి StringPiece& x, స్థిరరాశి StringPiece& y) {
  int32_t len = x.size();
  ఒకవేళ (len != y.size()) {
    తిరిగి అబద్ధం;
  }
  ఒకవేళ (len == 0) {
    తిరిగి నిజం;
  }
  స్థిరరాశి అక్షరం* p = x.దత్తాంశం();
  స్థిరరాశి అక్షరం* p2 = y.దత్తాంశం();
  // Test last byte in case strings share large common prefix
  --len;
  ఒకవేళ (p[len] != p2[len]) తిరిగి అబద్ధం;
  // At this point we can, but don't have to, ignore the last byte.
  తిరిగి uprv_memcmp(p, p2, len) == 0;
}


స్థిరరాశి int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_చివర
