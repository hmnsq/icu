// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#شامل "unicode/utypes.h"
#شامل "unicode/bytestream.h"
#شامل "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

حرف* ByteSink::Get_Append_بفر(int32_t min_استعداد_2,
                                int32_t /*desired_capacity_hint*/,
                                حرف* scratch U_LIFETIME_BOUND,
                                int32_t scratch_استعداد_2,
                                int32_t* نتیجہ_استعداد_2) U_LIFETIME_BOUND {
  اگر (min_استعداد_2 < 1 || scratch_استعداد_2 < min_استعداد_2) {
    *نتیجہ_استعداد_2 = 0;
    واپس خالی_اشاریہ;
  }
  *نتیجہ_استعداد_2 = scratch_استعداد_2;
  واپس scratch;
}

خالی ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(حرف* outbuf, int32_t capacity)
    : outbuf_(outbuf), استعداد_2(capacity < 0 ? 0 : capacity),
      سائز(0), appended_(0), overflowed_(جھوٹ) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  سائز = appended_ = 0;
  overflowed_ = جھوٹ;
  واپس *یہ;
}

خالی CheckedArrayByteSink::Append(ثابت حرف* بائٹیں, int32_t n) {
  اگر (n <= 0) {
    واپس;
  }
  اگر (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = سچ;
    واپس;
  }
  appended_ += n;
  int32_t دستیاب = استعداد_2 - سائز;
  اگر (n > دستیاب) {
    n = دستیاب;
    overflowed_ = سچ;
  }
  اگر (n > 0 && بائٹیں != (outbuf_ + سائز)) {
    uprv_memcpy(outbuf_ + سائز, بائٹیں, n);
  }
  سائز += n;
}

حرف* CheckedArrayByteSink::Get_Append_بفر(int32_t min_استعداد_2,
                                            int32_t /*desired_capacity_hint*/,
                                            حرف* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_استعداد_2,
                                            int32_t* نتیجہ_استعداد_2) U_LIFETIME_BOUND {
  اگر (min_استعداد_2 < 1 || scratch_استعداد_2 < min_استعداد_2) {
    *نتیجہ_استعداد_2 = 0;
    واپس خالی_اشاریہ;
  }
  int32_t دستیاب = استعداد_2 - سائز;
  اگر (دستیاب >= min_استعداد_2) {
    *نتیجہ_استعداد_2 = دستیاب;
    واپس outbuf_ + سائز;
  } ورنہ {
    *نتیجہ_استعداد_2 = scratch_استعداد_2;
    واپس scratch;
  }
}

U_NAMESPACE_انتہا
