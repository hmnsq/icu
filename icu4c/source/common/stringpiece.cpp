// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2004 and onwards Google Inc.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_NAMESPACE_BEGIN

StringPiece::StringPiece(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* str U_LIFETIME_BOUND)
    : ptr_(str), ಉದ್ದಳತೆ_2((str == ಶೂನ್ಯಸೂಚಕ) ? 0 : static_cast<int32_t>(uprv_strlen(str))) { }

StringPiece::StringPiece(ಸ್ಥಿರಾಂಕ StringPiece& x U_LIFETIME_BOUND, int32_t pos) {
  ಆದರೆ (pos < 0) {
    pos = 0;
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pos > x.ಉದ್ದಳತೆ_2) {
    pos = x.ಉದ್ದಳತೆ_2;
  }
  ptr_ = x.ptr_ + pos;
  ಉದ್ದಳತೆ_2 = x.ಉದ್ದಳತೆ_2 - pos;
}

StringPiece::StringPiece(ಸ್ಥಿರಾಂಕ StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len) {
  ಆದರೆ (pos < 0) {
    pos = 0;
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pos > x.ಉದ್ದಳತೆ_2) {
    pos = x.ಉದ್ದಳತೆ_2;
  }
  ಆದರೆ (len < 0) {
    len = 0;
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len > x.ಉದ್ದಳತೆ_2 - pos) {
    len = x.ಉದ್ದಳತೆ_2 - pos;
  }
  ptr_ = x.ptr_ + pos;
  ಉದ್ದಳತೆ_2 = len;
}

ಶೂನ್ಯ StringPiece::ಗಣ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* str) {
  ptr_ = str;
  ಆದರೆ (str != ಶೂನ್ಯಸೂಚಕ)
    ಉದ್ದಳತೆ_2 = static_cast<int32_t>(uprv_strlen(str));
  ಇಲ್ಲದಿದ್ದರೆ
    ಉದ್ದಳತೆ_2 = 0;
}

int32_t StringPiece::find(StringPiece needle, int32_t ಸ್ಥಾನಾಂತರ) {
  ಆದರೆ (length() == 0 && needle.length() == 0) {
    ಹಿಂತಿರುಗಿ 0;
  }
  // TODO: Improve to be better than O(N^2)?
  ಗಾಗಿ (int32_t i = ಸ್ಥಾನಾಂತರ; i < length(); i++) {
    int32_t j = 0;
    ಗಾಗಿ (; j < needle.length(); i++, j++) {
      ಆದರೆ (ದತ್ತಾಂಶ()[i] != needle.ದತ್ತಾಂಶ()[j]) {
        i -= j;
        ಹೋಗು outer_ಕೊನೆ_2;
      }
    }
    ಹಿಂತಿರುಗಿ i - j;
    outer_ಕೊನೆ_2: ಶೂನ್ಯ();
  }
  ಹಿಂತಿರುಗಿ -1;
}

int32_t StringPiece::compare(StringPiece other) {
  int32_t i = 0;
  ಗಾಗಿ (; i < length(); i++) {
    ಆದರೆ (i == other.length()) {
      // this is longer
      ಹಿಂತಿರುಗಿ 1;
    }
    ಅಕ್ಷರ a = ದತ್ತಾಂಶ()[i];
    ಅಕ್ಷರ b = other.ದತ್ತಾಂಶ()[i];
    ಆದರೆ (a < b) {
      ಹಿಂತಿರುಗಿ -1;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a > b) {
      ಹಿಂತಿರುಗಿ 1;
    }
  }
  ಆದರೆ (i < other.length()) {
    // other is longer
    ಹಿಂತಿರುಗಿ -1;
  }
  ಹಿಂತಿರುಗಿ 0;
}

U_EXPORT UBool U_EXPORT2
ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ StringPiece& x, ಸ್ಥಿರಾಂಕ StringPiece& y) {
  int32_t len = x.size();
  ಆದರೆ (len != y.size()) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
  }
  ಆದರೆ (len == 0) {
    ಹಿಂತಿರುಗಿ ನಿಜ;
  }
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* p = x.ದತ್ತಾಂಶ();
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* p2 = y.ದತ್ತಾಂಶ();
  // Test last byte in case strings share large common prefix
  --len;
  ಆದರೆ (p[len] != p2[len]) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
  // At this point we can, but don't have to, ignore the last byte.
  ಹಿಂತಿರುಗಿ uprv_memcmp(p, p2, len) == 0;
}


ಸ್ಥಿರಾಂಕ int32_t StringPiece::npos = 0x7fffffff;

U_NAMESPACE_ಕೊನೆ
