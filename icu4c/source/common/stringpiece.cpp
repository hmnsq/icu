// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/stringpiece.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(மாறிலி எழுத்து* str U_LIFETIME_BOUND)
    : ptr_(str), length_((str == nullptr) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(மாறிலி StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  எனில் (pos < 0) {
    pos = 0;
  } இல்லையெனில் எனில் (pos > x.length_) {
    pos = x.length_;
  }
  ptr_ = x.ptr_ + pos;
  length_ = x.length_ - pos;
}

StringPiece::StringPiece(மாறிலி StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  எனில் (pos < 0) {
    pos = 0;
  } இல்லையெனில் எனில் (pos > x.length_) {
    pos = x.length_;
  }
  எனில் (len < 0) {
    len = 0;
  } இல்லையெனில் எனில் (len > x.length_ - pos) {
    len = x.length_ - pos;
  }
  ptr_ = x.ptr_ + pos;
  length_ = len;
}

வெறுமை StringPiece::set(மாறிலி எழுத்து* str) {
  ptr_ = str;
  எனில் (str != nullptr)
    length_ = static_cast<int32_t>(uprv_strlen(str));
  இல்லையெனில்
    length_ = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t offset) {
  எனில் (length() == 0 && needle.length() == 0) {
    திரும்பு 0;
  }
  // TODO: Improve to be better than O(N^2)?
  க்கு (int32_t i = offset; i < length(); i++) {
    int32_t j = 0;
    க்கு (; j < needle.length(); i++, j++) {
      எனில் (data()[i] != needle.data()[j]) {
        i -= j;
        செல் outer_end;
      }
    }
    திரும்பு i - j;
    outer_end: வெறுமை();
  }
  திரும்பு -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  க்கு (; i < length(); i++) {
    எனில் (i == other.length()) {
      // this is longer
      திரும்பு 1;
    }
    எழுத்து a = data()[i];
    எழுத்து b = other.data()[i];
    எனில் (a < b) {
      திரும்பு -1;
    } இல்லையெனில் எனில் (a > b) {
      திரும்பு 1;
    }
  }
  எனில் (i < other.length()) {
    // other is longer
    திரும்பு -1;
  }
  திரும்பு 0;
}

U_EXPORT UBool U_EXPORT2
operator==(மாறிலி StringPiece& x, மாறிலி StringPiece& y) {
  int32_t len = x.size();
  எனில் (len != y.size()) {
    திரும்பு பொய்;
  }
  எனில் (len == 0) {
    திரும்பு உண்மை;
  }
  மாறிலி எழுத்து* p = x.data();
  மாறிலி எழுத்து* p2 = y.data();
  // Test last byte in case strings share large common prefix
  --len;
  எனில் (p[len] != p2[len]) திரும்பு பொய்;
  // At this point we can, but don't have to, ignore the last byte.
  திரும்பு uprv_memcmp(p, p2, len) == 0;
}


மாறிலி int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_END
