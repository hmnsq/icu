// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/bytestream.h"
#అన్తర్భావ "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

అక్షరం* ByteSink::Get_Append_బఫరు(int32_t min_సామర్థ్యం_2,
                                int32_t /*desired_capacity_hint*/,
                                అక్షరం* scratch U_LIFETIME_BOUND,
                                int32_t scratch_సామర్థ్యం_2,
                                int32_t* ఫలితం_సామర్థ్యం_2) U_LIFETIME_BOUND {
  ఒకవేళ (min_సామర్థ్యం_2 < 1 || scratch_సామర్థ్యం_2 < min_సామర్థ్యం_2) {
    *ఫలితం_సామర్థ్యం_2 = 0;
    తిరిగి శూన్యసూచిక;
  }
  *ఫలితం_సామర్థ్యం_2 = scratch_సామర్థ్యం_2;
  తిరిగి scratch;
}

శూన్య ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(అక్షరం* outbuf, int32_t capacity)
    : outbuf_(outbuf), సామర్థ్యం_2(capacity < 0 ? 0 : capacity),
      కొలత(0), appended_(0), overflowed_(అబద్ధం) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  కొలత = appended_ = 0;
  overflowed_ = అబద్ధం;
  తిరిగి *ఇది;
}

శూన్య CheckedArrayByteSink::Append(స్థిరరాశి అక్షరం* బైట్లు, int32_t n) {
  ఒకవేళ (n <= 0) {
    తిరిగి;
  }
  ఒకవేళ (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = నిజం;
    తిరిగి;
  }
  appended_ += n;
  int32_t అందుబాటు = సామర్థ్యం_2 - కొలత;
  ఒకవేళ (n > అందుబాటు) {
    n = అందుబాటు;
    overflowed_ = నిజం;
  }
  ఒకవేళ (n > 0 && బైట్లు != (outbuf_ + కొలత)) {
    uprv_memcpy(outbuf_ + కొలత, బైట్లు, n);
  }
  కొలత += n;
}

అక్షరం* CheckedArrayByteSink::Get_Append_బఫరు(int32_t min_సామర్థ్యం_2,
                                            int32_t /*desired_capacity_hint*/,
                                            అక్షరం* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_సామర్థ్యం_2,
                                            int32_t* ఫలితం_సామర్థ్యం_2) U_LIFETIME_BOUND {
  ఒకవేళ (min_సామర్థ్యం_2 < 1 || scratch_సామర్థ్యం_2 < min_సామర్థ్యం_2) {
    *ఫలితం_సామర్థ్యం_2 = 0;
    తిరిగి శూన్యసూచిక;
  }
  int32_t అందుబాటు = సామర్థ్యం_2 - కొలత;
  ఒకవేళ (అందుబాటు >= min_సామర్థ్యం_2) {
    *ఫలితం_సామర్థ్యం_2 = అందుబాటు;
    తిరిగి outbuf_ + కొలత;
  } లేకపోతే {
    *ఫలితం_సామర్థ్యం_2 = scratch_సామర్థ్యం_2;
    తిరిగి scratch;
  }
}

U_NAMESPACE_చివర
