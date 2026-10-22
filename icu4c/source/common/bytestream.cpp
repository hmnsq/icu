// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

वर्ण* ByteSink::GetAppendBuffer(int32_t min_capacity,
                                int32_t /*desired_capacity_hint*/,
                                वर्ण* scratch U_LIFETIME_BOUND,
                                int32_t scratch_capacity,
                                int32_t* result_capacity) U_LIFETIME_BOUND {
  यदि (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    प्रत्यागम nullptr;
  }
  *result_capacity = scratch_capacity;
  प्रत्यागम scratch;
}

शून्य ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(वर्ण* outbuf, int32_t capacity)
    : outbuf_(outbuf), capacity_(capacity < 0 ? 0 : capacity),
      size_(0), appended_(0), overflowed_(असत्य) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  size_ = appended_ = 0;
  overflowed_ = असत्य;
  प्रत्यागम *एतत्;
}

शून्य CheckedArrayByteSink::Append(ध्रुव वर्ण* bytes, int32_t n) {
  यदि (n <= 0) {
    प्रत्यागम;
  }
  यदि (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = सत्य;
    प्रत्यागम;
  }
  appended_ += n;
  int32_t available = capacity_ - size_;
  यदि (n > available) {
    n = available;
    overflowed_ = सत्य;
  }
  यदि (n > 0 && bytes != (outbuf_ + size_)) {
    uprv_memcpy(outbuf_ + size_, bytes, n);
  }
  size_ += n;
}

वर्ण* CheckedArrayByteSink::GetAppendBuffer(int32_t min_capacity,
                                            int32_t /*desired_capacity_hint*/,
                                            वर्ण* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_capacity,
                                            int32_t* result_capacity) U_LIFETIME_BOUND {
  यदि (min_capacity < 1 || scratch_capacity < min_capacity) {
    *result_capacity = 0;
    प्रत्यागम nullptr;
  }
  int32_t available = capacity_ - size_;
  यदि (available >= min_capacity) {
    *result_capacity = available;
    प्रत्यागम outbuf_ + size_;
  } अन्यथा {
    *result_capacity = scratch_capacity;
    प्रत्यागम scratch;
  }
}

U_NAMESPACE_END
