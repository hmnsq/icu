// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#समावेश "unicode/utypes.h"
#समावेश "unicode/stringpiece.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(स्थिर अक्षर* str U_LIFETIME_BOUND)
    : ptr_(str), लंबाई_2((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(स्थिर StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  अगर (pos < 0) {
    pos = 0;
  } अन्यथा अगर (pos > x.लंबाई_2) {
    pos = x.लंबाई_2;
  }
  ptr_ = x.ptr_ + pos;
  लंबाई_2 = x.लंबाई_2 - pos;
}

StringPiece::StringPiece(स्थिर StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  अगर (pos < 0) {
    pos = 0;
  } अन्यथा अगर (pos > x.लंबाई_2) {
    pos = x.लंबाई_2;
  }
  अगर (len < 0) {
    len = 0;
  } अन्यथा अगर (len > x.लंबाई_2 - pos) {
    len = x.लंबाई_2 - pos;
  }
  ptr_ = x.ptr_ + pos;
  लंबाई_2 = len;
}

व्योम StringPiece::set(स्थिर अक्षर* str) {
  ptr_ = str;
  अगर (str != nullptr)
    लंबाई_2 = static_cast<int32_t>(uprv_strlen(str));
  अन्यथा
    लंबाई_2 = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  अगर (length() == 0 && needle.length() == 0) {
    वापस 0;
  }
  // TODO: Improve to be better than O(N^2)?
  क्रम (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    क्रम (; j < needle.length(); i++, j++) {
      अगर (तथ्य()[i] != needle.तथ्य()[j]) {
        i -= j;
        जाओ outer_end;
      }
    }
    वापस i - j;
    outer_end: व्योम();
  }
  वापस -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  क्रम (; i < length(); i++) {
    अगर (i == other.length()) {
      // this is longer
      वापस 1;
    }
    अक्षर a = तथ्य()[i];
    अक्षर b = other.तथ्य()[i];
    अगर (a < b) {
      वापस -1;
    } अन्यथा अगर (a > b) {
      वापस 1;
    }
  }
  अगर (i < other.length()) {
    // other is longer
    वापस -1;
  }
  वापस 0;
}

U_EXPORT UBool U_EXPORT2
चालक==(स्थिर StringPiece& x, स्थिर StringPiece& y) {
  int32_t len = x.size();
  अगर (len != y.size()) {
    वापस false;
  }
  अगर (len == 0) {
    वापस true;
  }
  स्थिर अक्षर* p = x.तथ्य();
  स्थिर अक्षर* p2 = y.तथ्य();
  // Test last byte in case strings share large common prefix
  --len;
  अगर (p[len] != p2[len]) वापस false;
  // At this point we can, but don't have to, ignore the last byte.
  वापस uprv_memcmp(p, p2, len) == 0;
}


स्थिर int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
