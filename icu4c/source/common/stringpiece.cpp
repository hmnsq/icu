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

StringPiece::StringPiece(ধ্রুবক অক্ষর* str U_LIFETIME_BOUND)
    : ptr_(str), দৈর্ঘ্য_2((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(ধ্রুবক StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  যদি (pos < 0) {
    pos = 0;
  } অন্যথা যদি (pos > x.দৈর্ঘ্য_2) {
    pos = x.দৈর্ঘ্য_2;
  }
  ptr_ = x.ptr_ + pos;
  দৈর্ঘ্য_2 = x.দৈর্ঘ্য_2 - pos;
}

StringPiece::StringPiece(ধ্রুবক StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  যদি (pos < 0) {
    pos = 0;
  } অন্যথা যদি (pos > x.দৈর্ঘ্য_2) {
    pos = x.দৈর্ঘ্য_2;
  }
  যদি (len < 0) {
    len = 0;
  } অন্যথা যদি (len > x.দৈর্ঘ্য_2 - pos) {
    len = x.দৈর্ঘ্য_2 - pos;
  }
  ptr_ = x.ptr_ + pos;
  দৈর্ঘ্য_2 = len;
}

শূন্য StringPiece::set(ধ্রুবক অক্ষর* str) {
  ptr_ = str;
  যদি (str != nullptr)
    দৈর্ঘ্য_2 = static_cast<int32_t>(uprv_strlen(str));
  অন্যথা
    দৈর্ঘ্য_2 = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  যদি (length() == 0 && needle.length() == 0) {
    ফেরত 0;
  }
  // TODO: Improve to be better than O(N^2)?
  জন্য (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    জন্য (; j < needle.length(); i++, j++) {
      যদি (তথ্য()[i] != needle.তথ্য()[j]) {
        i -= j;
        যাও outer_end;
      }
    }
    ফেরত i - j;
    outer_end: শূন্য();
  }
  ফেরত -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  জন্য (; i < length(); i++) {
    যদি (i == other.length()) {
      // this is longer
      ফেরত 1;
    }
    অক্ষর a = তথ্য()[i];
    অক্ষর b = other.তথ্য()[i];
    যদি (a < b) {
      ফেরত -1;
    } অন্যথা যদি (a > b) {
      ফেরত 1;
    }
  }
  যদি (i < other.length()) {
    // other is longer
    ফেরত -1;
  }
  ফেরত 0;
}

U_EXPORT UBool U_EXPORT2
operator==(ধ্রুবক StringPiece& x, ধ্রুবক StringPiece& y) {
  int32_t len = x.size();
  যদি (len != y.size()) {
    ফেরত মিথ্যা;
  }
  যদি (len == 0) {
    ফেরত সত্য;
  }
  ধ্রুবক অক্ষর* p = x.তথ্য();
  ধ্রুবক অক্ষর* p2 = y.তথ্য();
  // Test last byte in case strings share large common prefix
  --len;
  যদি (p[len] != p2[len]) ফেরত মিথ্যা;
  // At this point we can, but don't have to, ignore the last byte.
  ফেরত uprv_memcmp(p, p2, len) == 0;
}


ধ্রুবক int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
