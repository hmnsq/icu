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
  אם (pos < 0) {
    pos = 0;
  } אחרת אם (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(ثابت StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  אם (pos < 0) {
    pos = 0;
  } אחרת אם (pos > x.length_) {
    pos = x.length_;
  }
  אם (len < 0) {
    len = 0;
  } אחרת אם (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

فراغ StringPiece::set(ثابت حرف* str) {
  ptr_ = str;
  אם (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  אחרת
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  אם (length() == 0 && needle.length() == 0) {
    החזר 0;
  }
  // TODO: Improve to be better than O(N^2)?
  עבור (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    עבור (; j < needle.length(); i++, j++) {
      אם (data()[i] != needle.data()[j]) {
        i -= j;
        اذهب outer_end;
      }
    }
    החזר i - j;
    outer_end: فراغ();
  }
  החזר -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  עבור (; i < length(); i++) {
    אם (i == other.length()) {
      // this is longer
      החזר 1;
    }
    حرف a = data()[i];
    حرف b = other.data()[i];
    אם (a < b) {
      החזר -1;
    } אחרת אם (a > b) {
      החזר 1;
    }
  }
  אם (i < other.length()) {
    // other is longer
    החזר -1;
  }
  החזר 0;
}

U_EXPORT UBool U_EXPORT2
operator==(ثابت StringPiece& x, ثابت StringPiece& y) {
  int32_t len = x.size();
  אם (len != y.size()) {
    החזר שקר;
  }
  אם (len == 0) {
    החזר אמת;
  }
  ثابت حرف* p = x.data();
  ثابت حرف* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  אם (p[len] != p2[len]) החזר שקר;
  // At this point we can, but don't have to, ignore the last byte.
  החזר uprv_memcmp(p, p2, len) == 0;
}


ثابت int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
