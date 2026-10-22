// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/bytestream.h"
#অন্তর্ভাব "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

অক্ষর* ByteSink::Get_Append_বাফার(int32_t min_capacity,
                                int32_t /*desired_capacity_hint*/,
                                অক্ষর* scratch U_LIFETIME_BOUND,
                                int32_t scratch_capacity,
                                int32_t* ফলাফল_capacity) U_LIFETIME_BOUND {
  যদি (min_capacity < 1 || scratch_capacity < min_capacity) {
    *ফলাফল_capacity = 0;
    ফেরত nullptr;
  }
  *ফলাফল_capacity = scratch_capacity;
  ফেরত scratch;
}

শূন্য ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(অক্ষর* outbuf, int32_t capacity)
    : outbuf_(outbuf), capacity_(capacity < 0 ? 0 : capacity),
      আকার_2(0), appended_(0), overflowed_(মিথ্যা) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  আকার_2 = appended_ = 0;
  overflowed_ = মিথ্যা;
  ফেরত *এই;
}

শূন্য CheckedArrayByteSink::Append(ধ্রুবক অক্ষর* bytes, int32_t n) {
  যদি (n <= 0) {
    ফেরত;
  }
  যদি (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = সত্য;
    ফেরত;
  }
  appended_ += n;
  int32_t available = capacity_ - আকার_2;
  যদি (n > available) {
    n = available;
    overflowed_ = সত্য;
  }
  যদি (n > 0 && bytes != (outbuf_ + আকার_2)) {
    uprv_memcpy(outbuf_ + আকার_2, bytes, n);
  }
  আকার_2 += n;
}

অক্ষর* CheckedArrayByteSink::Get_Append_বাফার(int32_t min_capacity,
                                            int32_t /*desired_capacity_hint*/,
                                            অক্ষর* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_capacity,
                                            int32_t* ফলাফল_capacity) U_LIFETIME_BOUND {
  যদি (min_capacity < 1 || scratch_capacity < min_capacity) {
    *ফলাফল_capacity = 0;
    ফেরত nullptr;
  }
  int32_t available = capacity_ - আকার_2;
  যদি (available >= min_capacity) {
    *ফলাফল_capacity = available;
    ফেরত outbuf_ + আকার_2;
  } অন্যথা {
    *ফলাফল_capacity = scratch_capacity;
    ফেরত scratch;
  }
}

U_NAMESPACE_END
