// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#include "unicode/utypes.h"
#include "unicode/stringpiece.h"
#include "cstring.h"
#include "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(const var_na* str U_LIFETIME_BOUND)
    : ptr_(str), length_((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(const StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  ถ้า (pos < 0) {
    pos = 0;
  } มิฉะนั้น ถ้า (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(const StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  ถ้า (pos < 0) {
    pos = 0;
  } มิฉะนั้น ถ้า (pos > x.length_) {
    pos = x.length_;
  }
  ถ้า (len < 0) {
    len = 0;
  } มิฉะนั้น ถ้า (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

void StringPiece::set(const var_na* str) {
  ptr_ = str;
  ถ้า (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  มิฉะนั้น
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  ถ้า (length() == 0 && needle.length() == 0) {
    คืน 0;
  }
  // TODO: Improve to be better than O(N^2)?
  สำหรับ (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    สำหรับ (; j < needle.length(); i++, j++) {
      ถ้า (data()[i] != needle.data()[j]) {
        i -= j;
        goto outer_end;
      }
    }
    คืน i - j;
    outer_end: void();
  }
  คืน -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  สำหรับ (; i < length(); i++) {
    ถ้า (i == other.length()) {
      // this is longer
      คืน 1;
    }
    var_na a = data()[i];
    var_na b = other.data()[i];
    ถ้า (a < b) {
      คืน -1;
    } มิฉะนั้น ถ้า (a > b) {
      คืน 1;
    }
  }
  ถ้า (i < other.length()) {
    // other is longer
    คืน -1;
  }
  คืน 0;
}

U_EXPORT UBool U_EXPORT2
operator==(const StringPiece& x, const StringPiece& y) {
  int32_t len = x.size();
  ถ้า (len != y.size()) {
    คืน false;
  }
  ถ้า (len == 0) {
    คืน true;
  }
  const var_na* p = x.data();
  const var_na* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  ถ้า (p[len] != p2[len]) คืน false;
  // At this point we can, but don't have to, ignore the last byte.
  คืน uprv_memcmp(p, p2, len) == 0;
}


const int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
