// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(ধ্রুব বর্ণ* str U_LIFETIME_BOUND)
    : ptr_(str), length_((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(ধ্রুব StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  যদি (pos < 0) {
    pos = 0;
  } অন্যথা যদি (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(ধ্রুব StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
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

শূন্য StringPiece::set(ধ্রুব বর্ণ* str) {
  ptr_ = str;
  যদি (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  অন্যথা
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  যদি (length() == 0 && needle.length() == 0) {
    প্রত্যাগম 0;
  }
  // TODO: Improve to be better than O(N^2)?
  প্রতি (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    প্রতি (; j < needle.length(); i++, j++) {
      যদি (data()[i] != needle.data()[j]) {
        i -= j;
        গচ্ছ outer_end;
      }
    }
    প্রত্যাগম i - j;
    outer_end: শূন্য();
  }
  প্রত্যাগম -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  প্রতি (; i < length(); i++) {
    যদি (i == other.length()) {
      // this is longer
      প্রত্যাগম 1;
    }
    বর্ণ a = data()[i];
    বর্ণ b = other.data()[i];
    যদি (a < b) {
      প্রত্যাগম -1;
    } অন্যথা যদি (a > b) {
      প্রত্যাগম 1;
    }
  }
  যদি (i < other.length()) {
    // other is longer
    প্রত্যাগম -1;
  }
  প্রত্যাগম 0;
}

U_EXPORT UBool U_EXPORT2
operator==(ধ্রুব StringPiece& x, ধ্রুব StringPiece& y) {
  int32_t len = x.size();
  যদি (len != y.size()) {
    প্রত্যাগম অসত্য;
  }
  যদি (len == 0) {
    প্রত্যাগম সত্য;
  }
  ধ্রুব বর্ণ* p = x.data();
  ধ্রুব বর্ণ* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  যদি (p[len] != p2[len]) প্রত্যাগম অসত্য;
  // At this point we can, but don't have to, ignore the last byte.
  প্রত্যাগম uprv_memcmp(p, p2, len) == 0;
}


ধ্রুব int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
