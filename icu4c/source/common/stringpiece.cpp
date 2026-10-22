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

StringPiece::StringPiece(ثابت حرف* str U_LIFETIME_BOUND)
    : ptr_(str), لمبائی_2((str == خالی_اشاریہ) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(ثابت StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  اگر (pos < 0) {
    pos = 0;
  } ورنہ اگر (pos > x.لمبائی_2) {
    pos = x.لمبائی_2;
  }
  ptr_ = x.ptr_ + pos;
  لمبائی_2 = x.لمبائی_2 - pos;
}

StringPiece::StringPiece(ثابت StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  اگر (pos < 0) {
    pos = 0;
  } ورنہ اگر (pos > x.لمبائی_2) {
    pos = x.لمبائی_2;
  }
  اگر (len < 0) {
    len = 0;
  } ورنہ اگر (len > x.لمبائی_2 - pos) {
    len = x.لمبائی_2 - pos;
  }
  ptr_ = x.ptr_ + pos;
  لمبائی_2 = len;
}

خالی StringPiece::سیٹ(ثابت حرف* str) {
  ptr_ = str;
  اگر (str != خالی_اشاریہ)
    لمبائی_2 = static_cast<int32_t>(uprv_strlen(str));
  ورنہ
    لمبائی_2 = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t انحراف) {
  اگر (length() == 0 && needle.length() == 0) {
    واپس 0;
  }
  // TODO: Improve to be better than O(N^2)?
  برائے (int32_t i = انحراف; i < length(); i++) {
    int32_t j = 0;
    برائے (; j < needle.length(); i++, j++) {
      اگر (ڈیٹا()[i] != needle.ڈیٹا()[j]) {
        i -= j;
        جاؤ outer_انتہا_2;
      }
    }
    واپس i - j;
    outer_انتہا_2: خالی();
  }
  واپس -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  برائے (; i < length(); i++) {
    اگر (i == other.length()) {
      // this is longer
      واپس 1;
    }
    حرف a = ڈیٹا()[i];
    حرف b = other.ڈیٹا()[i];
    اگر (a < b) {
      واپس -1;
    } ورنہ اگر (a > b) {
      واپس 1;
    }
  }
  اگر (i < other.length()) {
    // other is longer
    واپس -1;
  }
  واپس 0;
}

U_EXPORT UBool U_EXPORT2
عامل==(ثابت StringPiece& x, ثابت StringPiece& y) {
  int32_t len = x.size();
  اگر (len != y.size()) {
    واپس جھوٹ;
  }
  اگر (len == 0) {
    واپس سچ;
  }
  ثابت حرف* p = x.ڈیٹا();
  ثابت حرف* p2 = y.ڈیٹا();
  // Test last byte in case strings share large common prefix
  --len;
  اگر (p[len] != p2[len]) واپس جھوٹ;
  // At this point we can, but don't have to, ignore the last byte.
  واپس uprv_memcmp(p, p2, len) == 0;
}


ثابت int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_انتہا
