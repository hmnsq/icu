// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/stringpiece.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(ধ্রুৱ ৱর্ণ* str U_LIFETIME_BOUND)
    : ptr_(str), length_((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(ধ্রুৱ StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  যদি (pos < 0) {
    pos = 0;
  } অন্যথা যদি (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(ধ্রুৱ StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  যদি (pos < 0) {
    pos = 0;
  } অন্যথা যদি (pos > x.length_) {
    pos = x.length_;
  }
  যদি (len < 0) {
    len = 0;
  } অন্যথা যদি (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

শূন্য StringPiece::set(ধ্রুৱ ৱর্ণ* str) {
  ptr_ = str;
  যদি (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  অন্যথা
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  যদি (length() == 0 && needle.length() == 0) {
    ঘূৰাই 0;
  }
  // TODO: Improve to be better than O(N^2)?
  বাবে (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    বাবে (; j < needle.length(); i++, j++) {
      যদি (data()[i] != needle.data()[j]) {
        i -= j;
        গচ্ছ outer_end;
      }
    }
    ঘূৰাই i - j;
    outer_end: শূন্য();
  }
  ঘূৰাই -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  বাবে (; i < length(); i++) {
    যদি (i == other.length()) {
      // this is longer
      ঘূৰাই 1;
    }
    ৱর্ণ a = data()[i];
    ৱর্ণ b = other.data()[i];
    যদি (a < b) {
      ঘূৰাই -1;
    } অন্যথা যদি (a > b) {
      ঘূৰাই 1;
    }
  }
  যদি (i < other.length()) {
    // other is longer
    ঘূৰাই -1;
  }
  ঘূৰাই 0;
}

U_EXPORT UBool U_EXPORT2
operator==(ধ্রুৱ StringPiece& x, ধ্রুৱ StringPiece& y) {
  int32_t len = x.size();
  যদি (len != y.size()) {
    ঘূৰাই মিছা;
  }
  যদি (len == 0) {
    ঘূৰাই সঁচা;
  }
  ধ্রুৱ ৱর্ণ* p = x.data();
  ধ্রুৱ ৱর্ণ* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  যদি (p[len] != p2[len]) ঘূৰাই মিছা;
  // At this point we can, but don't have to, ignore the last byte.
  ঘূৰাই uprv_memcmp(p, p2, len) == 0;
}


ধ্রুৱ int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
