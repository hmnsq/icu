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
  якщо (pos < 0) {
    pos = 0;
  } інакше якщо (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(const StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  якщо (pos < 0) {
    pos = 0;
  } інакше якщо (pos > x.length_) {
    pos = x.length_;
  }
  якщо (len < 0) {
    len = 0;
  } інакше якщо (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

void StringPiece::set(const char* str) {
  ptr_ = str;
  якщо (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  інакше
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  якщо (length() == 0 && needle.length() == 0) {
    повернути 0;
  }
  // TODO: Improve to be better than O(N^2)?
  для (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    для (; j < needle.length(); i++, j++) {
      якщо (data()[i] != needle.data()[j]) {
        i -= j;
        goto outer_end;
      }
    }
    повернути i - j;
    outer_end: void();
  }
  повернути -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  для (; i < length(); i++) {
    якщо (i == other.length()) {
      // this is longer
      повернути 1;
    }
    char a = data()[i];
    char b = other.data()[i];
    якщо (a < b) {
      повернути -1;
    } інакше якщо (a > b) {
      повернути 1;
    }
  }
  якщо (i < other.length()) {
    // other is longer
    повернути -1;
  }
  повернути 0;
}

U_EXPORT UBool U_EXPORT2
operator==(const StringPiece& x, const StringPiece& y) {
  int32_t len = x.size();
  якщо (len != y.size()) {
    повернути false;
  }
  якщо (len == 0) {
    повернути true;
  }
  const char* p = x.data();
  const char* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  якщо (p[len] != p2[len]) повернути false;
  // At this point we can, but don't have to, ignore the last byte.
  повернути uprv_memcmp(p, p2, len) == 0;
}


const int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
