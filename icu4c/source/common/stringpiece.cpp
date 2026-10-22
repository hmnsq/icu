// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(ध्रुव वर्ण* str U_LIFETIME_BOUND)
    : ptr_(str), length_((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(ध्रुव StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  जइ (pos < 0) {
    pos = 0;
  } अण्णहा जइ (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(ध्रुव StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  जइ (pos < 0) {
    pos = 0;
  } अण्णहा जइ (pos > x.length_) {
    pos = x.length_;
  }
  जइ (len < 0) {
    len = 0;
  } अण्णहा जइ (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

शून्य StringPiece::set(ध्रुव वर्ण* str) {
  ptr_ = str;
  जइ (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  अण्णहा
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  जइ (length() == 0 && needle.length() == 0) {
    प्रत्यागम 0;
  }
  // TODO: Improve to be better than O(N^2)?
  प्रति (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    प्रति (; j < needle.length(); i++, j++) {
      जइ (data()[i] != needle.data()[j]) {
        i -= j;
        गच्छ outer_end;
      }
    }
    प्रत्यागम i - j;
    outer_end: शून्य();
  }
  प्रत्यागम -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  प्रति (; i < length(); i++) {
    जइ (i == other.length()) {
      // this is longer
      प्रत्यागम 1;
    }
    वर्ण a = data()[i];
    वर्ण b = other.data()[i];
    जइ (a < b) {
      प्रत्यागम -1;
    } अण्णहा जइ (a > b) {
      प्रत्यागम 1;
    }
  }
  जइ (i < other.length()) {
    // other is longer
    प्रत्यागम -1;
  }
  प्रत्यागम 0;
}

U_EXPORT UBool U_EXPORT2
operator==(ध्रुव StringPiece& x, ध्रुव StringPiece& y) {
  int32_t len = x.size();
  जइ (len != y.size()) {
    प्रत्यागम मिच्छ;
  }
  जइ (len == 0) {
    प्रत्यागम सच्च;
  }
  ध्रुव वर्ण* p = x.data();
  ध्रुव वर्ण* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  जइ (p[len] != p2[len]) प्रत्यागम मिच्छ;
  // At this point we can, but don't have to, ignore the last byte.
  प्रत्यागम uprv_memcmp(p, p2, len) == 0;
}


ध्रुव int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
