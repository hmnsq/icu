// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/bytestream.h"
#અન્તર્ભાવ "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

અક્ષર* ByteSink::GetAppendBuffer(int32_t min_capacity,
                                int32_t /*desired_capacity_hint*/,
                                અક્ષર* scratch U_LIFETIME_BOUND,
                                int32_t scratch_capacity,
                                int32_t* result_capacity) U_LIFETIME_BOUND {
  જો (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    પાછા nullptr;
  }
  *result_capacity = scratch_capacity;
  પાછા scratch;
}

શૂન્ય ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(અક્ષર* outbuf, int32_t capacity)
    : outbuf_(outbuf), capacity_(capacity < 0 ? 0 : capacity),
      size_(0), appended_(0), overflowed_(ખોટું) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  size_ = appended_ = 0;
  overflowed_ = ખોટું;
  પાછા *આ;
}

શૂન્ય CheckedArrayByteSink::Append(અચલ અક્ષર* bytes, int32_t n) {
  જો (n <= 0) {
    પાછા;
  }
  જો (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = સાચું;
    પાછા;
  }
  appended_ += n;
  int32_t available = capacity_ - size_;
  જો (n > available) {
    n = available;
    overflowed_ = સાચું;
  }
  જો (n > 0 && bytes != (outbuf_ + size_)) {
    uprv_memcpy(outbuf_ + size_, bytes, n);
  }
  size_ += n;
}

અક્ષર* CheckedArrayByteSink::GetAppendBuffer(int32_t min_capacity,
                                            int32_t /*desired_capacity_hint*/,
                                            અક્ષર* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_capacity,
                                            int32_t* result_capacity) U_LIFETIME_BOUND {
  જો (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    પાછા nullptr;
  }
  int32_t available = capacity_ - size_;
  જો (available >= min_capacity) {
    *result_capacity = available;
    પાછા outbuf_ + size_;
  } નહીંતર {
    *result_capacity = scratch_capacity;
    પાછા scratch;
  }
}

U_NAMESPACE_END
