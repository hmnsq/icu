// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#ضمن "unicode/utypes.h"
#ضمن "unicode/stringpiece.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(ثابت حرف* str U_LIFETIME_BOUND)
    : ptr_(str), length_((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(ثابت StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  إذا (pos < 0) {
    pos = 0;
  } وإلا إذا (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(ثابت StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  إذا (pos < 0) {
    pos = 0;
  } وإلا إذا (pos > x.length_) {
    pos = x.length_;
  }
  إذا (len < 0) {
    len = 0;
  } وإلا إذا (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

فراغ StringPiece::set(ثابت حرف* str) {
  ptr_ = str;
  إذا (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  وإلا
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  إذا (length() == 0 && needle.length() == 0) {
    إرجاع 0;
  }
  // TODO: Improve to be better than O(N^2)?
  لكل (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    لكل (; j < needle.length(); i++, j++) {
      إذا (data()[i] != needle.data()[j]) {
        i -= j;
        اذهب outer_end;
      }
    }
    إرجاع i - j;
    outer_end: فراغ();
  }
  إرجاع -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  لكل (; i < length(); i++) {
    إذا (i == other.length()) {
      // this is longer
      إرجاع 1;
    }
    حرف a = data()[i];
    حرف b = other.data()[i];
    إذا (a < b) {
      إرجاع -1;
    } وإلا إذا (a > b) {
      إرجاع 1;
    }
  }
  إذا (i < other.length()) {
    // other is longer
    إرجاع -1;
  }
  إرجاع 0;
}

U_EXPORT UBool U_EXPORT2
operator==(ثابت StringPiece& x, ثابت StringPiece& y) {
  int32_t len = x.size();
  إذا (len != y.size()) {
    إرجاع خطأ;
  }
  إذا (len == 0) {
    إرجاع true;
  }
  ثابت حرف* p = x.data();
  ثابت حرف* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  إذا (p[len] != p2[len]) إرجاع خطأ;
  // At this point we can, but don't have to, ignore the last byte.
  إرجاع uprv_memcmp(p, p2, len) == 0;
}


ثابت int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
