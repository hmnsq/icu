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
  නම් (pos < 0) {
    pos = 0;
  } නැතහොත් නම් (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(const StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  නම් (pos < 0) {
    pos = 0;
  } නැතහොත් නම් (pos > x.length_) {
    pos = x.length_;
  }
  නම් (len < 0) {
    len = 0;
  } නැතහොත් නම් (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

void StringPiece::set(const var_na* str) {
  ptr_ = str;
  නම් (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  නැතහොත්
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  නම් (length() == 0 && needle.length() == 0) {
    ආපසු 0;
  }
  // TODO: Improve to be better than O(N^2)?
  සඳහා (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    සඳහා (; j < needle.length(); i++, j++) {
      නම් (data()[i] != needle.data()[j]) {
        i -= j;
        goto outer_end;
      }
    }
    ආපසු i - j;
    outer_end: void();
  }
  ආපසු -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  සඳහා (; i < length(); i++) {
    නම් (i == other.length()) {
      // this is longer
      ආපසු 1;
    }
    var_na a = data()[i];
    var_na b = other.data()[i];
    නම් (a < b) {
      ආපසු -1;
    } නැතහොත් නම් (a > b) {
      ආපසු 1;
    }
  }
  නම් (i < other.length()) {
    // other is longer
    ආපසු -1;
  }
  ආපසු 0;
}

U_EXPORT UBool U_EXPORT2
operator==(const StringPiece& x, const StringPiece& y) {
  int32_t len = x.size();
  නම් (len != y.size()) {
    ආපසු false;
  }
  නම් (len == 0) {
    ආපසු true;
  }
  const var_na* p = x.data();
  const var_na* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  නම් (p[len] != p2[len]) ආපසු false;
  // At this point we can, but don't have to, ignore the last byte.
  ආපසු uprv_memcmp(p, p2, len) == 0;
}


const int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
