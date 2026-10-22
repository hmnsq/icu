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

حرف* ByteSink::Get_Append_بفر(int32_t min_ظرفیت_2,
                                int32_t /*desired_capacity_hint*/,
                                حرف* scratch U_LIFETIME_BOUND,
                                int32_t scratch_ظرفیت_2,
                                int32_t* پایله_ظرفیت_2) U_LIFETIME_BOUND {
  إذا (min_ظرفیت_2 < 1 || scratch_ظرفیت_2 < min_ظرفیت_2) {
    *پایله_ظرفیت_2 = 0;
    أرجع تش_اشاره;
  }
  *پایله_ظرفیت_2 = scratch_ظرفیت_2;
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

فراغ CheckedArrayByteSink::Append(ثابت حرف* بایټونه, int32_t n) {
  إذا (n <= 0) {
    أرجع;
  }
  إذا (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = ریښتیا;
    أرجع;
  }
  appended_ += n;
  int32_t شته = ظرفیت_2 - اندازه;
  إذا (n > شته) {
    n = شته;
    overflowed_ = ریښتیا;
  }
  إذا (n > 0 && بایټونه != (outbuf_ + اندازه)) {
    uprv_memcpy(outbuf_ + اندازه, بایټونه, n);
  }
  اندازه += n;
}

حرف* CheckedArrayByteSink::Get_Append_بفر(int32_t min_ظرفیت_2,
                                            int32_t /*desired_capacity_hint*/,
                                            حرف* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_ظرفیت_2,
                                            int32_t* پایله_ظرفیت_2) U_LIFETIME_BOUND {
  إذا (min_ظرفیت_2 < 1 || scratch_ظرفیت_2 < min_ظرفیت_2) {
    *پایله_ظرفیت_2 = 0;
    أرجع تش_اشاره;
  }
  int32_t شته = ظرفیت_2 - اندازه;
  إذا (شته >= min_ظرفیت_2) {
    *پایله_ظرفیت_2 = شته;
    أرجع outbuf_ + اندازه;
  } وإلا {
    *پایله_ظرفیت_2 = scratch_ظرفیت_2;
    أرجع scratch;
  }
}

U_NAMESPACE_پای_ټکی
