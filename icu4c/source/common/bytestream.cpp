// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/bytestream.h"
#അന്തര്ഭാവ "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

അക്ഷരം* ByteSink::GetAppendBuffer(int32_t min_capacity,
                                int32_t /*desired_capacity_hint*/,
                                അക്ഷരം* scratch U_LIFETIME_BOUND,
                                int32_t scratch_capacity,
                                int32_t* result_capacity) U_LIFETIME_BOUND {
  എങ്കിൽ (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    തിരികെ nullptr;
  }
  *result_capacity = scratch_capacity;
  തിരികെ scratch;
}

ശൂന്യം ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(അക്ഷരം* outbuf, int32_t capacity)
    : outbuf_(outbuf), capacity_(capacity < 0 ? 0 : capacity),
      size_(0), appended_(0), overflowed_(അസത്യം) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  size_ = appended_ = 0;
  overflowed_ = അസത്യം;
  തിരികെ *ഇത്;
}

ശൂന്യം CheckedArrayByteSink::Append(സ്ഥിരം അക്ഷരം* bytes, int32_t n) {
  എങ്കിൽ (n <= 0) {
    തിരികെ;
  }
  എങ്കിൽ (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = സത്യം;
    തിരികെ;
  }
  appended_ += n;
  int32_t available = capacity_ - size_;
  എങ്കിൽ (n > available) {
    n = available;
    overflowed_ = സത്യം;
  }
  എങ്കിൽ (n > 0 && bytes != (outbuf_ + size_)) {
    uprv_memcpy(outbuf_ + size_, bytes, n);
  }
  size_ += n;
}

അക്ഷരം* CheckedArrayByteSink::GetAppendBuffer(int32_t min_capacity,
                                            int32_t /*desired_capacity_hint*/,
                                            അക്ഷരം* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_capacity,
                                            int32_t* result_capacity) U_LIFETIME_BOUND {
  എങ്കിൽ (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    തിരികെ nullptr;
  }
  int32_t available = capacity_ - size_;
  എങ്കിൽ (available >= min_capacity) {
    *result_capacity = available;
    തിരികെ outbuf_ + size_;
  } അല്ലെങ്കിൽ {
    *result_capacity = scratch_capacity;
    തിരികെ scratch;
  }
}

U_NAMESPACE_END
