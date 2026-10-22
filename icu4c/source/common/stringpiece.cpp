// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(ಧ್ರುವ ವರ್ಣ* str U_LIFETIME_BOUND)
    : ptr_(str), length_((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(ಧ್ರುವ StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  ಯದಿ (pos < 0) {
    pos = 0;
  } ಅನ್ಯಥಾ ಯದಿ (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(ಧ್ರುವ StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  ಯದಿ (pos < 0) {
    pos = 0;
  } ಅನ್ಯಥಾ ಯದಿ (pos > x.length_) {
    pos = x.length_;
  }
  ಯದಿ (len < 0) {
    len = 0;
  } ಅನ್ಯಥಾ ಯದಿ (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

ಶೂನ್ಯ StringPiece::set(ಧ್ರುವ ವರ್ಣ* str) {
  ptr_ = str;
  ಯದಿ (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  ಅನ್ಯಥಾ
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  ಯದಿ (length() == 0 && needle.length() == 0) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  // TODO: Improve to be better than O(N^2)?
  ಪ್ರತಿ (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    ಪ್ರತಿ (; j < needle.length(); i++, j++) {
      ಯದಿ (data()[i] != needle.data()[j]) {
        i -= j;
        ಗಚ್ಛ outer_end;
      }
    }
    ಪ್ರತ್ಯಾಗಮ i - j;
    outer_end: ಶೂನ್ಯ();
  }
  ಪ್ರತ್ಯಾಗಮ -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  ಪ್ರತಿ (; i < length(); i++) {
    ಯದಿ (i == other.length()) {
      // this is longer
      ಪ್ರತ್ಯಾಗಮ 1;
    }
    ವರ್ಣ a = data()[i];
    ವರ್ಣ b = other.data()[i];
    ಯದಿ (a < b) {
      ಪ್ರತ್ಯಾಗಮ -1;
    } ಅನ್ಯಥಾ ಯದಿ (a > b) {
      ಪ್ರತ್ಯಾಗಮ 1;
    }
  }
  ಯದಿ (i < other.length()) {
    // other is longer
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

U_EXPORT UBool U_EXPORT2
operator==(ಧ್ರುವ StringPiece& x, ಧ್ರುವ StringPiece& y) {
  int32_t len = x.size();
  ಯದಿ (len != y.size()) {
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
  }
  ಯದಿ (len == 0) {
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
  }
  ಧ್ರುವ ವರ್ಣ* p = x.data();
  ಧ್ರುವ ವರ್ಣ* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  ಯದಿ (p[len] != p2[len]) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
  // At this point we can, but don't have to, ignore the last byte.
  ಪ್ರತ್ಯಾಗಮ uprv_memcmp(p, p2, len) == 0;
}


ಧ್ರುವ int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
