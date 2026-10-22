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

वर्ण* ByteSink::Get_Append_बफर(int32_t min_क्षमता_2,
                                int32_t /*desired_capacity_hint*/,
                                वर्ण* scratch U_LIFETIME_BOUND,
                                int32_t scratch_क्षमता_2,
                                int32_t* परिणाम_क्षमता_2) U_LIFETIME_BOUND {
  यदि (min_क्षमता_2 < 1 || scratch_क्षमता_2 < min_क्षमता_2) {
    *परिणाम_क्षमता_2 = 0;
    प्रत्यागम शून्यसूचकः;
  }
  *परिणाम_क्षमता_2 = scratch_क्षमता_2;
  प्रत्यागम scratch;
}

शून्य ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(वर्ण* outbuf, int32_t capacity)
    : outbuf_(outbuf), क्षमता_2(capacity < 0 ? 0 : capacity),
      आकार(0), appended_(0), overflowed_(असत्य) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  आकार = appended_ = 0;
  overflowed_ = असत्य;
  प्रत्यागम *एतत्;
}

शून्य CheckedArrayByteSink::Append(ध्रुव वर्ण* अष्टकानि, int32_t n) {
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
  int32_t उपलब्धम् = क्षमता_2 - आकार;
  यदि (n > उपलब्धम्) {
    n = उपलब्धम्;
    overflowed_ = सत्य;
  }
  यदि (n > 0 && अष्टकानि != (outbuf_ + आकार)) {
    uprv_memcpy(outbuf_ + आकार, अष्टकानि, n);
  }
  आकार += n;
}

वर्ण* CheckedArrayByteSink::Get_Append_बफर(int32_t min_क्षमता_2,
                                            int32_t /*desired_capacity_hint*/,
                                            वर्ण* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_क्षमता_2,
                                            int32_t* परिणाम_क्षमता_2) U_LIFETIME_BOUND {
  यदि (min_क्षमता_2 < 1 || scratch_क्षमता_2 < min_क्षमता_2) {
    *परिणाम_क्षमता_2 = 0;
    प्रत्यागम शून्यसूचकः;
  }
  int32_t उपलब्धम् = क्षमता_2 - आकार;
  यदि (उपलब्धम् >= min_क्षमता_2) {
    *परिणाम_क्षमता_2 = उपलब्धम्;
    प्रत्यागम outbuf_ + आकार;
  } अन्यथा {
    *परिणाम_क्षमता_2 = scratch_क्षमता_2;
    प्रत्यागम scratch;
  }
}

U_NAMESPACE_अन्तः
