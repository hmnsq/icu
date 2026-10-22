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
  ຖ້າ (pos < 0) {
    pos = 0;
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(const StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  ຖ້າ (pos < 0) {
    pos = 0;
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (pos > x.length_) {
    pos = x.length_;
  }
  ຖ້າ (len < 0) {
    len = 0;
  } ບໍ່ດັ່ງນັ້ນ ຖ້າ (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

void StringPiece::set(const var_na* str) {
  ptr_ = str;
  ຖ້າ (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  ບໍ່ດັ່ງນັ້ນ
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  ຖ້າ (length() == 0 && needle.length() == 0) {
    ສົ່ງຄືນ 0;
  }
  // TODO: Improve to be better than O(N^2)?
  ສຳລັບ (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    ສຳລັບ (; j < needle.length(); i++, j++) {
      ຖ້າ (data()[i] != needle.data()[j]) {
        i -= j;
        goto outer_end;
      }
    }
    ສົ່ງຄືນ i - j;
    outer_end: void();
  }
  ສົ່ງຄືນ -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  ສຳລັບ (; i < length(); i++) {
    ຖ້າ (i == other.length()) {
      // this is longer
      ສົ່ງຄືນ 1;
    }
    var_na a = data()[i];
    var_na b = other.data()[i];
    ຖ້າ (a < b) {
      ສົ່ງຄືນ -1;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (a > b) {
      ສົ່ງຄືນ 1;
    }
  }
  ຖ້າ (i < other.length()) {
    // other is longer
    ສົ່ງຄືນ -1;
  }
  ສົ່ງຄືນ 0;
}

U_EXPORT UBool U_EXPORT2
operator==(const StringPiece& x, const StringPiece& y) {
  int32_t len = x.size();
  ຖ້າ (len != y.size()) {
    ສົ່ງຄືນ false;
  }
  ຖ້າ (len == 0) {
    ສົ່ງຄືນ true;
  }
  const var_na* p = x.data();
  const var_na* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  ຖ້າ (p[len] != p2[len]) ສົ່ງຄືນ false;
  // At this point we can, but don't have to, ignore the last byte.
  ສົ່ງຄືນ uprv_memcmp(p, p2, len) == 0;
}


const int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
