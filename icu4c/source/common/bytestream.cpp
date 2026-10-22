// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestream.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

ਅੱਖਰ* ByteSink::Get_Append_ਬਫਰ(int32_t min_ਸਮਰੱਥਾ_2,
                                int32_t /*desired_capacity_hint*/,
                                ਅੱਖਰ* scratch U_LIFETIME_BOUND,
                                int32_t scratch_ਸਮਰੱਥਾ_2,
                                int32_t* ਨਤੀਜਾ_ਸਮਰੱਥਾ_2) U_LIFETIME_BOUND {
  ਜੇ (min_ਸਮਰੱਥਾ_2 < 1 || scratch_ਸਮਰੱਥਾ_2 < min_ਸਮਰੱਥਾ_2) {
    *ਨਤੀਜਾ_ਸਮਰੱਥਾ_2 = 0;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  *ਨਤੀਜਾ_ਸਮਰੱਥਾ_2 = scratch_ਸਮਰੱਥਾ_2;
  ਵਾਪਸ scratch;
}

ਖਾਲੀ ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(ਅੱਖਰ* outbuf, int32_t capacity)
    : outbuf_(outbuf), ਸਮਰੱਥਾ_2(capacity < 0 ? 0 : capacity),
      ਮਾਪ(0), appended_(0), overflowed_(ਝੂਠ) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  ਮਾਪ = appended_ = 0;
  overflowed_ = ਝੂਠ;
  ਵਾਪਸ *ਇਹ;
}

ਖਾਲੀ CheckedArrayByteSink::Append(ਅਚਲ ਅੱਖਰ* ਬਾਈਟਾਂ, int32_t n) {
  ਜੇ (n <= 0) {
    ਵਾਪਸ;
  }
  ਜੇ (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = ਸੱਚ;
    ਵਾਪਸ;
  }
  appended_ += n;
  int32_t ਉਪਲਬਧ = ਸਮਰੱਥਾ_2 - ਮਾਪ;
  ਜੇ (n > ਉਪਲਬਧ) {
    n = ਉਪਲਬਧ;
    overflowed_ = ਸੱਚ;
  }
  ਜੇ (n > 0 && ਬਾਈਟਾਂ != (outbuf_ + ਮਾਪ)) {
    uprv_memcpy(outbuf_ + ਮਾਪ, ਬਾਈਟਾਂ, n);
  }
  ਮਾਪ += n;
}

ਅੱਖਰ* CheckedArrayByteSink::Get_Append_ਬਫਰ(int32_t min_ਸਮਰੱਥਾ_2,
                                            int32_t /*desired_capacity_hint*/,
                                            ਅੱਖਰ* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_ਸਮਰੱਥਾ_2,
                                            int32_t* ਨਤੀਜਾ_ਸਮਰੱਥਾ_2) U_LIFETIME_BOUND {
  ਜੇ (min_ਸਮਰੱਥਾ_2 < 1 || scratch_ਸਮਰੱਥਾ_2 < min_ਸਮਰੱਥਾ_2) {
    *ਨਤੀਜਾ_ਸਮਰੱਥਾ_2 = 0;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  int32_t ਉਪਲਬਧ = ਸਮਰੱਥਾ_2 - ਮਾਪ;
  ਜੇ (ਉਪਲਬਧ >= min_ਸਮਰੱਥਾ_2) {
    *ਨਤੀਜਾ_ਸਮਰੱਥਾ_2 = ਉਪਲਬਧ;
    ਵਾਪਸ outbuf_ + ਮਾਪ;
  } ਨਹੀਂਤਾਂ {
    *ਨਤੀਜਾ_ਸਮਰੱਥਾ_2 = scratch_ਸਮਰੱਥਾ_2;
    ਵਾਪਸ scratch;
  }
}

U_NAMESPACE_ਸਿਰਾ
