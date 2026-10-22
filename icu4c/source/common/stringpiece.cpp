// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringpiece.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(ଅଚଳ ଅକ୍ଷର* str U_LIFETIME_BOUND)
    : ptr_(str), length_((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(ଅଚଳ StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  ଯଦି (pos < 0) {
    pos = 0;
  } ଅନ୍ୟଥା ଯଦି (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(ଅଚଳ StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  ଯଦି (pos < 0) {
    pos = 0;
  } ଅନ୍ୟଥା ଯଦି (pos > x.length_) {
    pos = x.length_;
  }
  ଯଦି (len < 0) {
    len = 0;
  } ଅନ୍ୟଥା ଯଦି (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

ଶୂନ୍ୟ StringPiece::set(ଅଚଳ ଅକ୍ଷର* str) {
  ptr_ = str;
  ଯଦି (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  ଅନ୍ୟଥା
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  ଯଦି (length() == 0 && needle.length() == 0) {
    ଫେରନ୍ତୁ 0;
  }
  // TODO: Improve to be better than O(N^2)?
  ପାଇଁ (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    ପାଇଁ (; j < needle.length(); i++, j++) {
      ଯଦି (data()[i] != needle.data()[j]) {
        i -= j;
        ଯାଅ outer_end;
      }
    }
    ଫେରନ୍ତୁ i - j;
    outer_end: ଶୂନ୍ୟ();
  }
  ଫେରନ୍ତୁ -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  ପାଇଁ (; i < length(); i++) {
    ଯଦି (i == other.length()) {
      // this is longer
      ଫେରନ୍ତୁ 1;
    }
    ଅକ୍ଷର a = data()[i];
    ଅକ୍ଷର b = other.data()[i];
    ଯଦି (a < b) {
      ଫେରନ୍ତୁ -1;
    } ଅନ୍ୟଥା ଯଦି (a > b) {
      ଫେରନ୍ତୁ 1;
    }
  }
  ଯଦି (i < other.length()) {
    // other is longer
    ଫେରନ୍ତୁ -1;
  }
  ଫେରନ୍ତୁ 0;
}

U_EXPORT UBool U_EXPORT2
operator==(ଅଚଳ StringPiece& x, ଅଚଳ StringPiece& y) {
  int32_t len = x.size();
  ଯଦି (len != y.size()) {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
  }
  ଯଦି (len == 0) {
    ଫେରନ୍ତୁ ସତ୍ୟ;
  }
  ଅଚଳ ଅକ୍ଷର* p = x.data();
  ଅଚଳ ଅକ୍ଷର* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  ଯଦି (p[len] != p2[len]) ଫେରନ୍ତୁ ମିଥ୍ୟା;
  // At this point we can, but don't have to, ignore the last byte.
  ଫେରନ୍ତୁ uprv_memcmp(p, p2, len) == 0;
}


ଅଚଳ int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
