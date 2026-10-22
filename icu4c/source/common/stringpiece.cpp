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
  ᰀᰧ (pos < 0) {
    pos = 0;
  } ᰀᰧᰀ ᰀᰧ (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(const StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  ᰀᰧ (pos < 0) {
    pos = 0;
  } ᰀᰧᰀ ᰀᰧ (pos > x.length_) {
    pos = x.length_;
  }
  ᰀᰧ (len < 0) {
    len = 0;
  } ᰀᰧᰀ ᰀᰧ (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

void StringPiece::set(const var_na* str) {
  ptr_ = str;
  ᰀᰧ (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  ᰀᰧᰀ
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  ᰀᰧ (length() == 0 && needle.length() == 0) {
    ᰀᰧᰌ 0;
  }
  // TODO: Improve to be better than O(N^2)?
  for (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    for (; j < needle.length(); i++, j++) {
      ᰀᰧ (data()[i] != needle.data()[j]) {
        i -= j;
        goto outer_end;
      }
    }
    ᰀᰧᰌ i - j;
    outer_end: void();
  }
  ᰀᰧᰌ -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  for (; i < length(); i++) {
    ᰀᰧ (i == other.length()) {
      // this is longer
      ᰀᰧᰌ 1;
    }
    var_na a = data()[i];
    var_na b = other.data()[i];
    ᰀᰧ (a < b) {
      ᰀᰧᰌ -1;
    } ᰀᰧᰀ ᰀᰧ (a > b) {
      ᰀᰧᰌ 1;
    }
  }
  ᰀᰧ (i < other.length()) {
    // other is longer
    ᰀᰧᰌ -1;
  }
  ᰀᰧᰌ 0;
}

U_EXPORT UBool U_EXPORT2
operator==(const StringPiece& x, const StringPiece& y) {
  int32_t len = x.size();
  ᰀᰧ (len != y.size()) {
    ᰀᰧᰌ false;
  }
  ᰀᰧ (len == 0) {
    ᰀᰧᰌ true;
  }
  const var_na* p = x.data();
  const var_na* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  ᰀᰧ (p[len] != p2[len]) ᰀᰧᰌ false;
  // At this point we can, but don't have to, ignore the last byte.
  ᰀᰧᰌ uprv_memcmp(p, p2, len) == 0;
}


const int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
