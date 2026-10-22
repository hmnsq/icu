// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2011, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2007 Google Inc. All Rights Reserved.
// Author: sanjay@google.com (Sanjay Ghemawat)

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_NAMESPACE_BEGIN

ByteSink::~ByteSink() {}

ಅಕ್ಷರ* ByteSink::Get_Append_ಬಫರ್(int32_t min_ಸಾಮರ್ಥ್ಯ_2,
                                int32_t /*desired_capacity_hint*/,
                                ಅಕ್ಷರ* scratch U_LIFETIME_BOUND,
                                int32_t scratch_ಸಾಮರ್ಥ್ಯ_2,
                                int32_t* ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ_2) U_LIFETIME_BOUND {
  ಆದರೆ (min_ಸಾಮರ್ಥ್ಯ_2 < 1 || scratch_ಸಾಮರ್ಥ್ಯ_2 < min_ಸಾಮರ್ಥ್ಯ_2) {
    *ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ_2 = 0;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  *ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ_2 = scratch_ಸಾಮರ್ಥ್ಯ_2;
  ಹಿಂತಿರುಗಿ scratch;
}

ಶೂನ್ಯ ByteSink::Flush() {}

CheckedArrayByteSink::CheckedArrayByteSink(ಅಕ್ಷರ* outbuf, int32_t capacity)
    : outbuf_(outbuf), ಸಾಮರ್ಥ್ಯ_2(capacity < 0 ? 0 : capacity),
      ಗಾತ್ರ_2(0), appended_(0), overflowed_(ಸುಳ್ಳು) {
}

CheckedArrayByteSink::~CheckedArrayByteSink() {}

CheckedArrayByteSink& CheckedArrayByteSink::Reset() U_LIFETIME_BOUND {
  ಗಾತ್ರ_2 = appended_ = 0;
  overflowed_ = ಸುಳ್ಳು;
  ಹಿಂತಿರುಗಿ *ಇದು;
}

ಶೂನ್ಯ CheckedArrayByteSink::Append(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಬೈಟುಗಳು, int32_t n) {
  ಆದರೆ (n <= 0) {
    ಹಿಂತಿರುಗಿ;
  }
  ಆದರೆ (n > (INT32_MAX - appended_)) {
    // TODO: Report as integer overflow, not merely buffer overflow.
    appended_ = INT32_MAX;
    overflowed_ = ನಿಜ;
    ಹಿಂತಿರುಗಿ;
  }
  appended_ += n;
  int32_t ಲಭ್ಯ = ಸಾಮರ್ಥ್ಯ_2 - ಗಾತ್ರ_2;
  ಆದರೆ (n > ಲಭ್ಯ) {
    n = ಲಭ್ಯ;
    overflowed_ = ನಿಜ;
  }
  ಆದರೆ (n > 0 && ಬೈಟುಗಳು != (outbuf_ + ಗಾತ್ರ_2)) {
    uprv_memcpy(outbuf_ + ಗಾತ್ರ_2, ಬೈಟುಗಳು, n);
  }
  ಗಾತ್ರ_2 += n;
}

ಅಕ್ಷರ* CheckedArrayByteSink::Get_Append_ಬಫರ್(int32_t min_ಸಾಮರ್ಥ್ಯ_2,
                                            int32_t /*desired_capacity_hint*/,
                                            ಅಕ್ಷರ* scratch U_LIFETIME_BOUND,
                                            int32_t scratch_ಸಾಮರ್ಥ್ಯ_2,
                                            int32_t* ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ_2) U_LIFETIME_BOUND {
  ಆದರೆ (min_ಸಾಮರ್ಥ್ಯ_2 < 1 || scratch_ಸಾಮರ್ಥ್ಯ_2 < min_ಸಾಮರ್ಥ್ಯ_2) {
    *ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ_2 = 0;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  int32_t ಲಭ್ಯ = ಸಾಮರ್ಥ್ಯ_2 - ಗಾತ್ರ_2;
  ಆದರೆ (ಲಭ್ಯ >= min_ಸಾಮರ್ಥ್ಯ_2) {
    *ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ_2 = ಲಭ್ಯ;
    ಹಿಂತಿರುಗಿ outbuf_ + ಗಾತ್ರ_2;
  } ಇಲ್ಲದಿದ್ದರೆ {
    *ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ_2 = scratch_ಸಾಮರ್ಥ್ಯ_2;
    ಹಿಂತಿರುಗಿ scratch;
  }
}

U_NAMESPACE_ಕೊನೆ
