// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#شامل "unicode/utypes.h"
#شامل "unicode/stringpiece.h"
#شامل "cstring.h"
#شامل "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(ثابت نویسه* str U_LIFETIME_BOUND)
    : ptr_(str), length_((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(ثابت StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  اگر (pos < 0) {
    pos = 0;
  } وگرنه اگر (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(ثابت StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  اگر (pos < 0) {
    pos = 0;
  } وگرنه اگر (pos > x.length_) {
    pos = x.length_;
  }
  اگر (len < 0) {
    len = 0;
  } وگرنه اگر (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

تهی StringPiece::set(ثابت نویسه* str) {
  ptr_ = str;
  اگر (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  وگرنه
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  اگر (length() == 0 && needle.length() == 0) {
    بازگشت 0;
  }
  // TODO: Improve to be better than O(N^2)?
  برای (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    برای (; j < needle.length(); i++, j++) {
      اگر (data()[i] != needle.data()[j]) {
        i -= j;
        برو outer_end;
      }
    }
    بازگشت i - j;
    outer_end: تهی();
  }
  بازگشت -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  برای (; i < length(); i++) {
    اگر (i == other.length()) {
      // this is longer
      بازگشت 1;
    }
    نویسه a = data()[i];
    نویسه b = other.data()[i];
    اگر (a < b) {
      بازگشت -1;
    } وگرنه اگر (a > b) {
      بازگشت 1;
    }
  }
  اگر (i < other.length()) {
    // other is longer
    بازگشت -1;
  }
  بازگشت 0;
}

U_EXPORT UBool U_EXPORT2
operator==(ثابت StringPiece& x, ثابت StringPiece& y) {
  int32_t len = x.size();
  اگر (len != y.size()) {
    بازگشت نادرست;
  }
  اگر (len == 0) {
    بازگشت درست;
  }
  ثابت نویسه* p = x.data();
  ثابت نویسه* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  اگر (p[len] != p2[len]) بازگشت نادرست;
  // At this point we can, but don't have to, ignore the last byte.
  بازگشت uprv_memcmp(p, p2, len) == 0;
}


ثابت int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
