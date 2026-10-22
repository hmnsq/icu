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
    : ptr_(str), درازی_2((str == اشاره_تهی) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(ثابت StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  إذا (pos < 0) {
    pos = 0;
  } وإلا إذا (pos > x.درازی_2) {
    pos = x.درازی_2;
  }
  ptr_ = x.ptr_ + pos;
  درازی_2 = x.درازی_2 - pos;
}

StringPiece::StringPiece(ثابت StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  إذا (pos < 0) {
    pos = 0;
  } وإلا إذا (pos > x.درازی_2) {
    pos = x.درازی_2;
  }
  إذا (len < 0) {
    len = 0;
  } وإلا إذا (len > x.درازی_2 - pos) {
    len = x.درازی_2 - pos;
  }
  ptr_ = x.ptr_ + pos;
  درازی_2 = len;
}

فراغ StringPiece::مجموعه(ثابت حرف* str) {
  ptr_ = str;
  إذا (str != اشاره_تهی)
    درازی_2 = static_cast<int32_t>(uprv_strlen(str));
  وإلا
    درازی_2 = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t جابه_جایی) {
  إذا (length() == 0 && needle.length() == 0) {
    أرجع 0;
  }
  // TODO: Improve to be better than O(N^2)?
  لكل (int32_t i = جابه_جایی; i < length(); i++) {
    int32_t j = 0;
    لكل (; j < needle.length(); i++, j++) {
      إذا (داده()[i] != needle.داده()[j]) {
        i -= j;
        اذهب outer_انتها_2;
      }
    }
    أرجع i - j;
    outer_انتها_2: فراغ();
  }
  أرجع -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  لكل (; i < length(); i++) {
    إذا (i == other.length()) {
      // this is longer
      أرجع 1;
    }
    حرف a = داده()[i];
    حرف b = other.داده()[i];
    إذا (a < b) {
      أرجع -1;
    } وإلا إذا (a > b) {
      أرجع 1;
    }
  }
  إذا (i < other.length()) {
    // other is longer
    أرجع -1;
  }
  أرجع 0;
}

U_EXPORT UBool U_EXPORT2
عملگر==(ثابت StringPiece& x, ثابت StringPiece& y) {
  int32_t len = x.size();
  إذا (len != y.size()) {
    أرجع خطأ;
  }
  إذا (len == 0) {
    أرجع درست;
  }
  ثابت حرف* p = x.داده();
  ثابت حرف* p2 = y.داده();
  // Test last byte in case strings share large common prefix
  --len;
  إذا (p[len] != p2[len]) أرجع خطأ;
  // At this point we can, but don't have to, ignore the last byte.
  أرجع uprv_memcmp(p, p2, len) == 0;
}


ثابت int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_انتها
