// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#समावेश "unicode/utypes.h"
#समावेश "unicode/bytestream.h"
#समावेश "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

अक्षर* ByteSink::Get_Append_बफर(int32_t min_capacity,
                                int32_t /*desired_capacity_hint*/,
                                अक्षर* scratch U_LIFETIME_BOUND,
                                int32_t scratch_capacity,
                                int32_t* परिणाम_capacity) U_LIFETIME_BOUND {
  अगर (min_capacity < 1 || scratch_capacity < min_capacity) {
    *परिणाम_capacity = 0;
    वापस nullptr;
  }
  *परिणाम_capacity = scratch_capacity;
  वापस scratch;
}

व्योम ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(अक्षर* outbuf, int32_t capacity)
    : outbuf_(outbuf), capacity_(capacity < 0 ? 0 : capacity),
      आकार(0), appended_(0), overflowed_(false) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  आकार = appended_ = 0;
  overflowed_ = false;
  वापस *यह;
}

व्योम CheckedArrayByteSink::Append(स्थिर अक्षर* bytes, int32_t n) {
  अगर (n <= 0) {
    वापस;
  }
  अगर (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = true;
    वापस;
  }
  appended_ += n;
  int32_t available = capacity_ - आकार;
  अगर (n > available) {
    n = available;
    overflowed_ = true;
  }
  अगर (n > 0 && bytes != (outbuf_ + आकार)) {
    uprv_memcpy(outbuf_ + आकार, bytes, n);
  }
  आकार += n;
}

अक्षर* CheckedArrayByteSink::Get_Append_बफर(int32_t min_capacity,
                                            int32_t /*desired_capacity_hint*/,
                                            अक्षर* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_capacity,
                                            int32_t* परिणाम_capacity) U_LIFETIME_BOUND {
  अगर (min_capacity < 1 || scratch_capacity < min_capacity) {
    *परिणाम_capacity = 0;
    वापस nullptr;
  }
  int32_t available = capacity_ - आकार;
  अगर (available >= min_capacity) {
    *परिणाम_capacity = available;
    वापस outbuf_ + आकार;
  } अन्यथा {
    *परिणाम_capacity = scratch_capacity;
    वापस scratch;
  }
}

U_NAMESPACE_END
