// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#include "unicode/utypes.h"
#include "unicode/bytestream.h"
#include "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

char* ByteSink::GetAppendBuffer(int32_t min_capacity,
                                int32_t /*desired_capacity_hint*/,
                                char* scratch U_LIFETIME_BOUND,
                                int32_t scratch_capacity,
                                int32_t* result_capacity) U_LIFETIME_BOUND {
  만약 (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    반환 nullptr;
  }
  *result_capacity = scratch_capacity;
  반환 scratch;
}

void ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(char* outbuf, int32_t capacity)
    : outbuf_(outbuf), capacity_(capacity < 0 ? 0 : capacity),
      size_(0), appended_(0), overflowed_(false) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  size_ = appended_ = 0;
  overflowed_ = false;
  반환 *this;
}

void CheckedArrayByteSink::Append(const char* bytes, int32_t n) {
  만약 (n <= 0) {
    반환;
  }
  만약 (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = true;
    반환;
  }
  appended_ += n;
  int32_t available = capacity_ - size_;
  만약 (n > available) {
    n = available;
    overflowed_ = true;
  }
  만약 (n > 0 && bytes != (outbuf_ + size_)) {
    uprv_memcpy(outbuf_ + size_, bytes, n);
  }
  size_ += n;
}

char* CheckedArrayByteSink::GetAppendBuffer(int32_t min_capacity,
                                            int32_t /*desired_capacity_hint*/,
                                            char* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_capacity,
                                            int32_t* result_capacity) U_LIFETIME_BOUND {
  만약 (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    반환 nullptr;
  }
  int32_t available = capacity_ - size_;
  만약 (available >= min_capacity) {
    *result_capacity = available;
    반환 outbuf_ + size_;
  } 아니면 {
    *result_capacity = scratch_capacity;
    반환 scratch;
  }
}

U_NAMESPACE_END
