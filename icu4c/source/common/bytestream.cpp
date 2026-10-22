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

حرف* ByteSink::Get_Append_بفر(int32_t min_صلاحيت_2,
                                int32_t /*desired_capacity_hint*/,
                                حرف* scratch U_LIFETIME_BOUND,
                                int32_t scratch_صلاحيت_2,
                                int32_t* نتيجو_صلاحيت_2) U_LIFETIME_BOUND {
  إذا (min_صلاحيت_2 < 1 || scratch_صلاحيت_2 < min_صلاحيت_2) {
    *نتيجو_صلاحيت_2 = 0;
    أرجع خالي_اشارو;
  }
  *نتيجو_صلاحيت_2 = scratch_صلاحيت_2;
  أرجع scratch;
}

فراغ ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(حرف* outbuf, int32_t capacity)
    : outbuf_(outbuf), صلاحيت_2(capacity < 0 ? 0 : capacity),
      سائيز(0), appended_(0), overflowed_(خطأ) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  سائيز = appended_ = 0;
  overflowed_ = خطأ;
  أرجع *هذا;
}

فراغ CheckedArrayByteSink::Append(ثابت حرف* بائيٽون, int32_t n) {
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
  int32_t موجود = صلاحيت_2 - سائيز;
  إذا (n > موجود) {
    n = موجود;
    overflowed_ = سچ;
  }
  إذا (n > 0 && بائيٽون != (outbuf_ + سائيز)) {
    uprv_memcpy(outbuf_ + سائيز, بائيٽون, n);
  }
  سائيز += n;
}

حرف* CheckedArrayByteSink::Get_Append_بفر(int32_t min_صلاحيت_2,
                                            int32_t /*desired_capacity_hint*/,
                                            حرف* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_صلاحيت_2,
                                            int32_t* نتيجو_صلاحيت_2) U_LIFETIME_BOUND {
  إذا (min_صلاحيت_2 < 1 || scratch_صلاحيت_2 < min_صلاحيت_2) {
    *نتيجو_صلاحيت_2 = 0;
    أرجع خالي_اشارو;
  }
  int32_t موجود = صلاحيت_2 - سائيز;
  إذا (موجود >= min_صلاحيت_2) {
    *نتيجو_صلاحيت_2 = موجود;
    أرجع outbuf_ + سائيز;
  } وإلا {
    *نتيجو_صلاحيت_2 = scratch_صلاحيت_2;
    أرجع scratch;
  }
}

U_NAMESPACE_پڇاڙي
