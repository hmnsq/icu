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

StringPiece::StringPiece(const char* str U_LIFETIME_BOUND)
    : ptr_(str), length_((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(const StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  如果 (pos < 0) {
    pos = 0;
  } 否则 如果 (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(const StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  如果 (pos < 0) {
    pos = 0;
  } 否则 如果 (pos > x.length_) {
    pos = x.length_;
  }
  如果 (len < 0) {
    len = 0;
  } 否则 如果 (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

void StringPiece::set(const char* str) {
  ptr_ = str;
  如果 (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  否则
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  如果 (length() == 0 && needle.length() == 0) {
    返回 0;
  }
  // TODO: Improve to be better than O(N^2)?
  为 (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    为 (; j < needle.length(); i++, j++) {
      如果 (data()[i] != needle.data()[j]) {
        i -= j;
        goto outer_end;
      }
    }
    返回 i - j;
    outer_end: void();
  }
  返回 -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  为 (; i < length(); i++) {
    如果 (i == other.length()) {
      // this is longer
      返回 1;
    }
    char a = data()[i];
    char b = other.data()[i];
    如果 (a < b) {
      返回 -1;
    } 否则 如果 (a > b) {
      返回 1;
    }
  }
  如果 (i < other.length()) {
    // other is longer
    返回 -1;
  }
  返回 0;
}

U_EXPORT UBool U_EXPORT2
operator==(const StringPiece& x, const StringPiece& y) {
  int32_t len = x.size();
  如果 (len != y.size()) {
    返回 false;
  }
  如果 (len == 0) {
    返回 true;
  }
  const char* p = x.data();
  const char* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  如果 (p[len] != p2[len]) 返回 false;
  // At this point we can, but don't have to, ignore the last byte.
  返回 uprv_memcmp(p, p2, len) == 0;
}


const int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
