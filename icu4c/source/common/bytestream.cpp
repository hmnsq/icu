// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

ವರ್ಣ* ByteSink::GetAppendBuffer(int32_t min_capacity,
                                int32_t /*desired_capacity_hint*/,
                                ವರ್ಣ* scratch U_LIFETIME_BOUND,
                                int32_t scratch_capacity,
                                int32_t* result_capacity) U_LIFETIME_BOUND {
  ಯದಿ (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  *result_capacity = scratch_capacity;
  ಪ್ರತ್ಯಾಗಮ scratch;
}

ಶೂನ್ಯ ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(ವರ್ಣ* outbuf, int32_t capacity)
    : outbuf_(outbuf), capacity_(capacity < 0 ? 0 : capacity),
      size_(0), appended_(0), overflowed_(ಅಸತ್ಯ) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  size_ = appended_ = 0;
  overflowed_ = ಅಸತ್ಯ;
  ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ಶೂನ್ಯ CheckedArrayByteSink::Append(ಧ್ರುವ ವರ್ಣ* bytes, int32_t n) {
  ಯದಿ (n <= 0) {
    ಪ್ರತ್ಯಾಗಮ;
  }
  ಯದಿ (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = ಸತ್ಯ;
    ಪ್ರತ್ಯಾಗಮ;
  }
  appended_ += n;
  int32_t available = capacity_ - size_;
  ಯದಿ (n > available) {
    n = available;
    overflowed_ = ಸತ್ಯ;
  }
  ಯದಿ (n > 0 && bytes != (outbuf_ + size_)) {
    uprv_memcpy(outbuf_ + size_, bytes, n);
  }
  size_ += n;
}

ವರ್ಣ* CheckedArrayByteSink::GetAppendBuffer(int32_t min_capacity,
                                            int32_t /*desired_capacity_hint*/,
                                            ವರ್ಣ* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_capacity,
                                            int32_t* result_capacity) U_LIFETIME_BOUND {
  ಯದಿ (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    ಪ್ರತ್ಯಾಗಮ nullptr;
  }
  int32_t available = capacity_ - size_;
  ಯದಿ (available >= min_capacity) {
    *result_capacity = available;
    ಪ್ರತ್ಯಾಗಮ outbuf_ + size_;
  } ಅನ್ಯಥಾ {
    *result_capacity = scratch_capacity;
    ಪ್ರತ್ಯಾಗಮ scratch;
  }
}

U_NAMESPACE_END
