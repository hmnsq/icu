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

حرف* ByteSink::Get_Append_بفر(int32_t min_سمرتھا_2,
                                int32_t /*desired_capacity_hint*/,
                                حرف* scratch U_LIFETIME_BOUND,
                                int32_t scratch_سمرتھا_2,
                                int32_t* نتیجہ_سمرتھا_2) U_LIFETIME_BOUND {
  إذا (min_سمرتھا_2 < 1 || scratch_سمرتھا_2 < min_سمرتھا_2) {
    *نتیجہ_سمرتھا_2 = 0;
    أرجع خالی_سوچک;
  }
  *نتیجہ_سمرتھا_2 = scratch_سمرتھا_2;
  أرجع scratch;
}

فراغ ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(حرف* outbuf, int32_t capacity)
    : outbuf_(outbuf), سمرتھا_2(capacity < 0 ? 0 : capacity),
      ناپ(0), appended_(0), overflowed_(خطأ) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  ناپ = appended_ = 0;
  overflowed_ = خطأ;
  أرجع *هذا;
}

فراغ CheckedArrayByteSink::Append(ثابت حرف* بائٹاں, int32_t n) {
  إذا (n <= 0) {
    أرجع;
  }
  إذا (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = سچ;
    أرجع;
  }
  appended_ += n;
  int32_t دستیاب = سمرتھا_2 - ناپ;
  إذا (n > دستیاب) {
    n = دستیاب;
    overflowed_ = سچ;
  }
  إذا (n > 0 && بائٹاں != (outbuf_ + ناپ)) {
    uprv_memcpy(outbuf_ + ناپ, بائٹاں, n);
  }
  ناپ += n;
}

حرف* CheckedArrayByteSink::Get_Append_بفر(int32_t min_سمرتھا_2,
                                            int32_t /*desired_capacity_hint*/,
                                            حرف* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_سمرتھا_2,
                                            int32_t* نتیجہ_سمرتھا_2) U_LIFETIME_BOUND {
  إذا (min_سمرتھا_2 < 1 || scratch_سمرتھا_2 < min_سمرتھا_2) {
    *نتیجہ_سمرتھا_2 = 0;
    أرجع خالی_سوچک;
  }
  int32_t دستیاب = سمرتھا_2 - ناپ;
  إذا (دستیاب >= min_سمرتھا_2) {
    *نتیجہ_سمرتھا_2 = دستیاب;
    أرجع outbuf_ + ناپ;
  } وإلا {
    *نتیجہ_سمرتھا_2 = scratch_سمرتھا_2;
    أرجع scratch;
  }
}

U_NAMESPACE_سرا
