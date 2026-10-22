// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/bytestream.h"
#অন্তর্ভাৱ "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

ৱর্ণ* ByteSink::GetAppendBuffer(int32_t min_capacity,
                                int32_t /*desired_capacity_hint*/,
                                ৱর্ণ* scratch U_LIFETIME_BOUND,
                                int32_t scratch_capacity,
                                int32_t* result_capacity) U_LIFETIME_BOUND {
  যদি (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    ঘূৰাই nullptr;
  }
  *result_capacity = scratch_capacity;
  ঘূৰাই scratch;
}

শূন্য ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(ৱর্ণ* outbuf, int32_t capacity)
    : outbuf_(outbuf), capacity_(capacity < 0 ? 0 : capacity),
      size_(0), appended_(0), overflowed_(মিছা) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  size_ = appended_ = 0;
  overflowed_ = মিছা;
  ঘূৰাই *এতত্;
}

শূন্য CheckedArrayByteSink::Append(ধ্রুৱ ৱর্ণ* bytes, int32_t n) {
  যদি (n <= 0) {
    ঘূৰাই;
  }
  যদি (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = সঁচা;
    ঘূৰাই;
  }
  appended_ += n;
  int32_t available = capacity_ - size_;
  যদি (n > available) {
    n = available;
    overflowed_ = সঁচা;
  }
  যদি (n > 0 && bytes != (outbuf_ + size_)) {
    uprv_memcpy(outbuf_ + size_, bytes, n);
  }
  size_ += n;
}

ৱর্ণ* CheckedArrayByteSink::GetAppendBuffer(int32_t min_capacity,
                                            int32_t /*desired_capacity_hint*/,
                                            ৱর্ণ* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_capacity,
                                            int32_t* result_capacity) U_LIFETIME_BOUND {
  যদি (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    ঘূৰাই nullptr;
  }
  int32_t available = capacity_ - size_;
  যদি (available >= min_capacity) {
    *result_capacity = available;
    ঘূৰাই outbuf_ + size_;
  } অন্যথা {
    *result_capacity = scratch_capacity;
    ঘূৰাই scratch;
  }
}

U_NAMESPACE_END
