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
  nếu (pos < 0) {
    pos = 0;
  } không_thì nếu (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(const StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  nếu (pos < 0) {
    pos = 0;
  } không_thì nếu (pos > x.length_) {
    pos = x.length_;
  }
  nếu (len < 0) {
    len = 0;
  } không_thì nếu (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

void StringPiece::set(const char* str) {
  ptr_ = str;
  nếu (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  không_thì
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  nếu (length() == 0 && needle.length() == 0) {
    trả_về 0;
  }
  // TODO: Improve to be better than O(N^2)?
  cho (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    cho (; j < needle.length(); i++, j++) {
      nếu (data()[i] != needle.data()[j]) {
        i -= j;
        goto outer_end;
      }
    }
    trả_về i - j;
    outer_end: void();
  }
  trả_về -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  cho (; i < length(); i++) {
    nếu (i == other.length()) {
      // this is longer
      trả_về 1;
    }
    char a = data()[i];
    char b = other.data()[i];
    nếu (a < b) {
      trả_về -1;
    } không_thì nếu (a > b) {
      trả_về 1;
    }
  }
  nếu (i < other.length()) {
    // other is longer
    trả_về -1;
  }
  trả_về 0;
}

U_EXPORT UBool U_EXPORT2
operator==(const StringPiece& x, const StringPiece& y) {
  int32_t len = x.size();
  nếu (len != y.size()) {
    trả_về false;
  }
  nếu (len == 0) {
    trả_về true;
  }
  const char* p = x.data();
  const char* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  nếu (p[len] != p2[len]) trả_về false;
  // At this point we can, but don't have to, ignore the last byte.
  trả_về uprv_memcmp(p, p2, len) == 0;
}


const int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
