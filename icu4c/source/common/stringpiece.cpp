// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(ਅਚਲ ਅੱਖਰ* str U_LIFETIME_BOUND)
    : ptr_(str), ਲੰਮਾਈ_2((str == ਖਾਲੀ_ਸੂਚਕ) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(ਅਚਲ StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  ਜੇ (pos < 0) {
    pos = 0;
  } ਨਹੀਂਤਾਂ ਜੇ (pos > x.ਲੰਮਾਈ_2) {
    pos = x.ਲੰਮਾਈ_2;
  }
  ptr_ = x.ptr_ + pos;
  ਲੰਮਾਈ_2 = x.ਲੰਮਾਈ_2 - pos;
}

StringPiece::StringPiece(ਅਚਲ StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  ਜੇ (pos < 0) {
    pos = 0;
  } ਨਹੀਂਤਾਂ ਜੇ (pos > x.ਲੰਮਾਈ_2) {
    pos = x.ਲੰਮਾਈ_2;
  }
  ਜੇ (len < 0) {
    len = 0;
  } ਨਹੀਂਤਾਂ ਜੇ (len > x.ਲੰਮਾਈ_2 - pos) {
    len = x.ਲੰਮਾਈ_2 - pos;
  }
  ptr_ = x.ptr_ + pos;
  ਲੰਮਾਈ_2 = len;
}

ਖਾਲੀ StringPiece::ਸੈੱਟ(ਅਚਲ ਅੱਖਰ* str) {
  ptr_ = str;
  ਜੇ (str != ਖਾਲੀ_ਸੂਚਕ)
    ਲੰਮਾਈ_2 = static_cast<int32_t>(uprv_strlen(str));
  ਨਹੀਂਤਾਂ
    ਲੰਮਾਈ_2 = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t ਵਿੱਥ) {
  ਜੇ (length() == 0 && needle.length() == 0) {
    ਵਾਪਸ 0;
  }
  // TODO: Improve to be better than O(N^2)?
  ਲਈ (int32_t i = ਵਿੱਥ; i < length(); i++) {
    int32_t j = 0;
    ਲਈ (; j < needle.length(); i++, j++) {
      ਜੇ (ਅੰਕੜੇ()[i] != needle.ਅੰਕੜੇ()[j]) {
        i -= j;
        ਜਾਓ outer_ਸਿਰਾ_2;
      }
    }
    ਵਾਪਸ i - j;
    outer_ਸਿਰਾ_2: ਖਾਲੀ();
  }
  ਵਾਪਸ -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  ਲਈ (; i < length(); i++) {
    ਜੇ (i == other.length()) {
      // this is longer
      ਵਾਪਸ 1;
    }
    ਅੱਖਰ a = ਅੰਕੜੇ()[i];
    ਅੱਖਰ b = other.ਅੰਕੜੇ()[i];
    ਜੇ (a < b) {
      ਵਾਪਸ -1;
    } ਨਹੀਂਤਾਂ ਜੇ (a > b) {
      ਵਾਪਸ 1;
    }
  }
  ਜੇ (i < other.length()) {
    // other is longer
    ਵਾਪਸ -1;
  }
  ਵਾਪਸ 0;
}

U_EXPORT UBool U_EXPORT2
ਸੰਚਾਲਕ==(ਅਚਲ StringPiece& x, ਅਚਲ StringPiece& y) {
  int32_t len = x.size();
  ਜੇ (len != y.size()) {
    ਵਾਪਸ ਝੂਠ;
  }
  ਜੇ (len == 0) {
    ਵਾਪਸ ਸੱਚ;
  }
  ਅਚਲ ਅੱਖਰ* p = x.ਅੰਕੜੇ();
  ਅਚਲ ਅੱਖਰ* p2 = y.ਅੰਕੜੇ();
  // Test last byte in case strings share large common prefix
  --len;
  ਜੇ (p[len] != p2[len]) ਵਾਪਸ ਝੂਠ;
  // At this point we can, but don't have to, ignore the last byte.
  ਵਾਪਸ uprv_memcmp(p, p2, len) == 0;
}


ਅਚਲ int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_ਸਿਰਾ
