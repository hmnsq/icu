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

حرف* ByteSink::Get_Append_میانگیر(int32_t min_ظرفیت_2,
                                int32_t /*desired_capacity_hint*/,
                                حرف* scratch U_LIFETIME_BOUND,
                                int32_t scratch_ظرفیت_2,
                                int32_t* نتیجه_ظرفیت_2) U_LIFETIME_BOUND {
  إذا (min_ظرفیت_2 < 1 || scratch_ظرفیت_2 < min_ظرفیت_2) {
    *نتیجه_ظرفیت_2 = 0;
    أرجع اشاره_تهی;
  }
  *نتیجه_ظرفیت_2 = scratch_ظرفیت_2;
  أرجع scratch;
}

فراغ ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(حرف* outbuf, int32_t capacity)
    : outbuf_(outbuf), ظرفیت_2(capacity < 0 ? 0 : capacity),
      اندازه(0), appended_(0), overflowed_(خطأ) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  اندازه = appended_ = 0;
  overflowed_ = خطأ;
  أرجع *هذا;
}

فراغ CheckedArrayByteSink::Append(ثابت حرف* بایتها, int32_t n) {
  إذا (n <= 0) {
    أرجع;
  }
  إذا (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = درست;
    أرجع;
  }
  appended_ += n;
  int32_t موجود = ظرفیت_2 - اندازه;
  إذا (n > موجود) {
    n = موجود;
    overflowed_ = درست;
  }
  إذا (n > 0 && بایتها != (outbuf_ + اندازه)) {
    uprv_memcpy(outbuf_ + اندازه, بایتها, n);
  }
  اندازه += n;
}

حرف* CheckedArrayByteSink::Get_Append_میانگیر(int32_t min_ظرفیت_2,
                                            int32_t /*desired_capacity_hint*/,
                                            حرف* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_ظرفیت_2,
                                            int32_t* نتیجه_ظرفیت_2) U_LIFETIME_BOUND {
  إذا (min_ظرفیت_2 < 1 || scratch_ظرفیت_2 < min_ظرفیت_2) {
    *نتیجه_ظرفیت_2 = 0;
    أرجع اشاره_تهی;
  }
  int32_t موجود = ظرفیت_2 - اندازه;
  إذا (موجود >= min_ظرفیت_2) {
    *نتیجه_ظرفیت_2 = موجود;
    أرجع outbuf_ + اندازه;
  } وإلا {
    *نتیجه_ظرفیت_2 = scratch_ظرفیت_2;
    أرجع scratch;
  }
}

U_NAMESPACE_انتها
