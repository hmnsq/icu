// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/stringpiece.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(സ്ഥിരം അക്ഷരം* str U_LIFETIME_BOUND)
    : ptr_(str), length_((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(സ്ഥിരം StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  എങ്കിൽ (pos < 0) {
    pos = 0;
  } അല്ലെങ്കിൽ എങ്കിൽ (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(സ്ഥിരം StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  എങ്കിൽ (pos < 0) {
    pos = 0;
  } അല്ലെങ്കിൽ എങ്കിൽ (pos > x.length_) {
    pos = x.length_;
  }
  എങ്കിൽ (len < 0) {
    len = 0;
  } അല്ലെങ്കിൽ എങ്കിൽ (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

ശൂന്യം StringPiece::set(സ്ഥിരം അക്ഷരം* str) {
  ptr_ = str;
  എങ്കിൽ (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  അല്ലെങ്കിൽ
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  എങ്കിൽ (length() == 0 && needle.length() == 0) {
    തിരികെ 0;
  }
  // TODO: Improve to be better than O(N^2)?
  വേണ്ടി (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    വേണ്ടി (; j < needle.length(); i++, j++) {
      എങ്കിൽ (data()[i] != needle.data()[j]) {
        i -= j;
        പോകുക outer_end;
      }
    }
    തിരികെ i - j;
    outer_end: ശൂന്യം();
  }
  തിരികെ -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  വേണ്ടി (; i < length(); i++) {
    എങ്കിൽ (i == other.length()) {
      // this is longer
      തിരികെ 1;
    }
    അക്ഷരം a = data()[i];
    അക്ഷരം b = other.data()[i];
    എങ്കിൽ (a < b) {
      തിരികെ -1;
    } അല്ലെങ്കിൽ എങ്കിൽ (a > b) {
      തിരികെ 1;
    }
  }
  എങ്കിൽ (i < other.length()) {
    // other is longer
    തിരികെ -1;
  }
  തിരികെ 0;
}

U_EXPORT UBool U_EXPORT2
operator==(സ്ഥിരം StringPiece& x, സ്ഥിരം StringPiece& y) {
  int32_t len = x.size();
  എങ്കിൽ (len != y.size()) {
    തിരികെ അസത്യം;
  }
  എങ്കിൽ (len == 0) {
    തിരികെ സത്യം;
  }
  സ്ഥിരം അക്ഷരം* p = x.data();
  സ്ഥിരം അക്ഷരം* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  എങ്കിൽ (p[len] != p2[len]) തിരികെ അസത്യം;
  // At this point we can, but don't have to, ignore the last byte.
  തിരികെ uprv_memcmp(p, p2, len) == 0;
}


സ്ഥിരം int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
