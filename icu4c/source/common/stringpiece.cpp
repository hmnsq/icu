// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/stringpiece.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(અચલ અક્ષર* str U_LIFETIME_BOUND)
    : ptr_(str), length_((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(અચલ StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  જો (pos < 0) {
    pos = 0;
  } નહીંતર જો (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(અચલ StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  જો (pos < 0) {
    pos = 0;
  } નહીંતર જો (pos > x.length_) {
    pos = x.length_;
  }
  જો (len < 0) {
    len = 0;
  } નહીંતર જો (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

શૂન્ય StringPiece::set(અચલ અક્ષર* str) {
  ptr_ = str;
  જો (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  નહીંતર
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  જો (length() == 0 && needle.length() == 0) {
    પાછા 0;
  }
  // TODO: Improve to be better than O(N^2)?
  માટે (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    માટે (; j < needle.length(); i++, j++) {
      જો (data()[i] != needle.data()[j]) {
        i -= j;
        જાઓ outer_end;
      }
    }
    પાછા i - j;
    outer_end: શૂન્ય();
  }
  પાછા -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  માટે (; i < length(); i++) {
    જો (i == other.length()) {
      // this is longer
      પાછા 1;
    }
    અક્ષર a = data()[i];
    અક્ષર b = other.data()[i];
    જો (a < b) {
      પાછા -1;
    } નહીંતર જો (a > b) {
      પાછા 1;
    }
  }
  જો (i < other.length()) {
    // other is longer
    પાછા -1;
  }
  પાછા 0;
}

U_EXPORT UBool U_EXPORT2
operator==(અચલ StringPiece& x, અચલ StringPiece& y) {
  int32_t len = x.size();
  જો (len != y.size()) {
    પાછા ખોટું;
  }
  જો (len == 0) {
    પાછા સાચું;
  }
  અચલ અક્ષર* p = x.data();
  અચલ અક્ષર* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  જો (p[len] != p2[len]) પાછા ખોટું;
  // At this point we can, but don't have to, ignore the last byte.
  પાછા uprv_memcmp(p, p2, len) == 0;
}


અચલ int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
