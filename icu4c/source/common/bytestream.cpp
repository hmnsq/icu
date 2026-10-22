// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/bytestream.h"
#அந்தர்பாவ "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

எழுத்து* ByteSink::GetAppendBuffer(int32_t min_capacity,
                                int32_t /*desired_capacity_hint*/,
                                எழுத்து* scratch U_LIFETIME_BOUND,
                                int32_t scratch_capacity,
                                int32_t* result_capacity) U_LIFETIME_BOUND {
  எனில் (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    திரும்பு nullptr;
  }
  *result_capacity = scratch_capacity;
  திரும்பு scratch;
}

வெறுமை ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(எழுத்து* outbuf, int32_t capacity)
    : outbuf_(outbuf), capacity_(capacity < 0 ? 0 : capacity),
      size_(0), appended_(0), overflowed_(பொய்) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  size_ = appended_ = 0;
  overflowed_ = பொய்;
  திரும்பு *இது;
}

வெறுமை CheckedArrayByteSink::Append(மாறிலி எழுத்து* bytes, int32_t n) {
  எனில் (n <= 0) {
    திரும்பு;
  }
  எனில் (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = உண்மை;
    திரும்பு;
  }
  appended_ += n;
  int32_t available = capacity_ - size_;
  எனில் (n > available) {
    n = available;
    overflowed_ = உண்மை;
  }
  எனில் (n > 0 && bytes != (outbuf_ + size_)) {
    uprv_memcpy(outbuf_ + size_, bytes, n);
  }
  size_ += n;
}

எழுத்து* CheckedArrayByteSink::GetAppendBuffer(int32_t min_capacity,
                                            int32_t /*desired_capacity_hint*/,
                                            எழுத்து* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_capacity,
                                            int32_t* result_capacity) U_LIFETIME_BOUND {
  எனில் (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    திரும்பு nullptr;
  }
  int32_t available = capacity_ - size_;
  எனில் (available >= min_capacity) {
    *result_capacity = available;
    திரும்பு outbuf_ + size_;
  } இல்லையெனில் {
    *result_capacity = scratch_capacity;
    திரும்பு scratch;
  }
}

U_NAMESPACE_END
