// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#شامل "unicode/utypes.h"
#شامل "unicode/bytestream.h"
#شامل "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

نویسه* ByteSink::GetAppendBuffer(int32_t min_capacity,
                                int32_t /*desired_capacity_hint*/,
                                نویسه* scratch U_LIFETIME_BOUND,
                                int32_t scratch_capacity,
                                int32_t* result_capacity) U_LIFETIME_BOUND {
  اگر (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    بازگشت nullptr;
  }
  *result_capacity = scratch_capacity;
  بازگشت scratch;
}

تهی ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(نویسه* outbuf, int32_t capacity)
    : outbuf_(outbuf), capacity_(capacity < 0 ? 0 : capacity),
      size_(0), appended_(0), overflowed_(نادرست) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  size_ = appended_ = 0;
  overflowed_ = نادرست;
  بازگشت *این;
}

تهی CheckedArrayByteSink::Append(ثابت نویسه* bytes, int32_t n) {
  اگر (n <= 0) {
    بازگشت;
  }
  اگر (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = درست;
    بازگشت;
  }
  appended_ += n;
  int32_t available = capacity_ - size_;
  اگر (n > available) {
    n = available;
    overflowed_ = درست;
  }
  اگر (n > 0 && bytes != (outbuf_ + size_)) {
    uprv_memcpy(outbuf_ + size_, bytes, n);
  }
  size_ += n;
}

نویسه* CheckedArrayByteSink::GetAppendBuffer(int32_t min_capacity,
                                            int32_t /*desired_capacity_hint*/,
                                            نویسه* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_capacity,
                                            int32_t* result_capacity) U_LIFETIME_BOUND {
  اگر (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    بازگشت nullptr;
  }
  int32_t available = capacity_ - size_;
  اگر (available >= min_capacity) {
    *result_capacity = available;
    بازگشت outbuf_ + size_;
  } وگرنه {
    *result_capacity = scratch_capacity;
    بازگشت scratch;
  }
}

U_NAMESPACE_END
