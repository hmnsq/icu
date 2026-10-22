// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(स्थिरांक अक्षर* str U_LIFETIME_BOUND)
    : ptr_(str), length_((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(स्थिरांक StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  जर (pos < 0) {
    pos = 0;
  } अन्यथा जर (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(स्थिरांक StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  जर (pos < 0) {
    pos = 0;
  } अन्यथा जर (pos > x.length_) {
    pos = x.length_;
  }
  जर (len < 0) {
    len = 0;
  } अन्यथा जर (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

शून्य StringPiece::set(स्थिरांक अक्षर* str) {
  ptr_ = str;
  जर (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  अन्यथा
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  जर (length() == 0 && needle.length() == 0) {
    परत 0;
  }
  // TODO: Improve to be better than O(N^2)?
  साठी (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    साठी (; j < needle.length(); i++, j++) {
      जर (data()[i] != needle.data()[j]) {
        i -= j;
        जा outer_end;
      }
    }
    परत i - j;
    outer_end: शून्य();
  }
  परत -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  साठी (; i < length(); i++) {
    जर (i == other.length()) {
      // this is longer
      परत 1;
    }
    अक्षर a = data()[i];
    अक्षर b = other.data()[i];
    जर (a < b) {
      परत -1;
    } अन्यथा जर (a > b) {
      परत 1;
    }
  }
  जर (i < other.length()) {
    // other is longer
    परत -1;
  }
  परत 0;
}

U_EXPORT UBool U_EXPORT2
चालक==(स्थिरांक StringPiece& x, स्थिरांक StringPiece& y) {
  int32_t len = x.size();
  जर (len != y.size()) {
    परत असत्य;
  }
  जर (len == 0) {
    परत सत्य;
  }
  स्थिरांक अक्षर* p = x.data();
  स्थिरांक अक्षर* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  जर (p[len] != p2[len]) परत असत्य;
  // At this point we can, but don't have to, ignore the last byte.
  परत uprv_memcmp(p, p2, len) == 0;
}


स्थिरांक int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
