// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestream.h"
#ضمن "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

حرف* ByteSink::GetAppendBuffer(int32_t min_capacity,
                                int32_t /*desired_capacity_hint*/,
                                حرف* scratch U_LIFETIME_BOUND,
                                int32_t scratch_capacity,
                                int32_t* result_capacity) U_LIFETIME_BOUND {
  𐡄𐡍 (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    أرجع nullptr;
  }
  *result_capacity = scratch_capacity;
  أرجع scratch;
}

فراغ ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(حرف* outbuf, int32_t capacity)
    : outbuf_(outbuf), capacity_(capacity < 0 ? 0 : capacity),
      size_(0), appended_(0), overflowed_(خطأ) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  size_ = appended_ = 0;
  overflowed_ = خطأ;
  أرجع *هذا;
}

فراغ CheckedArrayByteSink::Append(ثابت حرف* bytes, int32_t n) {
  𐡄𐡍 (n <= 0) {
    أرجع;
  }
  𐡄𐡍 (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = 𐡒𐡔𐡈;
    أرجع;
  }
  appended_ += n;
  int32_t available = capacity_ - size_;
  𐡄𐡍 (n > available) {
    n = available;
    overflowed_ = 𐡒𐡔𐡈;
  }
  𐡄𐡍 (n > 0 && bytes != (outbuf_ + size_)) {
    uprv_memcpy(outbuf_ + size_, bytes, n);
  }
  size_ += n;
}

حرف* CheckedArrayByteSink::GetAppendBuffer(int32_t min_capacity,
                                            int32_t /*desired_capacity_hint*/,
                                            حرف* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_capacity,
                                            int32_t* result_capacity) U_LIFETIME_BOUND {
  𐡄𐡍 (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    أرجع nullptr;
  }
  int32_t available = capacity_ - size_;
  𐡄𐡍 (available >= min_capacity) {
    *result_capacity = available;
    أرجع outbuf_ + size_;
  } وإلا {
    *result_capacity = scratch_capacity;
    أرجع scratch;
  }
}

U_NAMESPACE_END
