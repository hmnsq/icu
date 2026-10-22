// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestream.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

ଅକ୍ଷର* ByteSink::GetAppendBuffer(int32_t min_capacity,
                                int32_t /*desired_capacity_hint*/,
                                ଅକ୍ଷର* scratch U_LIFETIME_BOUND,
                                int32_t scratch_capacity,
                                int32_t* result_capacity) U_LIFETIME_BOUND {
  ଯଦି (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    ଫେରନ୍ତୁ nullptr;
  }
  *result_capacity = scratch_capacity;
  ଫେରନ୍ତୁ scratch;
}

ଶୂନ୍ୟ ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(ଅକ୍ଷର* outbuf, int32_t capacity)
    : outbuf_(outbuf), capacity_(capacity < 0 ? 0 : capacity),
      size_(0), appended_(0), overflowed_(ମିଥ୍ୟା) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  size_ = appended_ = 0;
  overflowed_ = ମିଥ୍ୟା;
  ଫେରନ୍ତୁ *ଏହି;
}

ଶୂନ୍ୟ CheckedArrayByteSink::Append(ଅଚଳ ଅକ୍ଷର* bytes, int32_t n) {
  ଯଦି (n <= 0) {
    ଫେରନ୍ତୁ;
  }
  ଯଦି (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = ସତ୍ୟ;
    ଫେରନ୍ତୁ;
  }
  appended_ += n;
  int32_t available = capacity_ - size_;
  ଯଦି (n > available) {
    n = available;
    overflowed_ = ସତ୍ୟ;
  }
  ଯଦି (n > 0 && bytes != (outbuf_ + size_)) {
    uprv_memcpy(outbuf_ + size_, bytes, n);
  }
  size_ += n;
}

ଅକ୍ଷର* CheckedArrayByteSink::GetAppendBuffer(int32_t min_capacity,
                                            int32_t /*desired_capacity_hint*/,
                                            ଅକ୍ଷର* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_capacity,
                                            int32_t* result_capacity) U_LIFETIME_BOUND {
  ଯଦି (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    ଫେରନ୍ତୁ nullptr;
  }
  int32_t available = capacity_ - size_;
  ଯଦି (available >= min_capacity) {
    *result_capacity = available;
    ଫେରନ୍ତୁ outbuf_ + size_;
  } ଅନ୍ୟଥା {
    *result_capacity = scratch_capacity;
    ଫେରନ୍ତୁ scratch;
  }
}

U_NAMESPACE_END
