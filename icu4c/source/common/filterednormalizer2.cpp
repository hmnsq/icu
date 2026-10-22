// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2009-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  filterednormalizer2.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2009dec10
*   created by: Markus W. Scherer
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_NORMALIZATION

#ಅನ್ತರ್ಭಾವ "unicode/edits.h"
#ಅನ್ತರ್ಭಾವ "unicode/normalizer2.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringoptions.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/unorm.h"
#ಅನ್ತರ್ಭಾವ "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(ಸ್ಥಿರಾಂಕ UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    uprv_check_Can_Get_ಬಫರ್(src, error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        dest.ಗಣ_ಗಮ್ಯ_Bogus();
        ಹಿಂತಿರುಗಿ dest;
    }
    ಆದರೆ(&dest==&src) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ dest;
    }
    dest.remove();
    ಹಿಂತಿರುಗಿ normalize(src, dest, USET_ಹರವು_SIMPLE, error_ಸಂಕೇತ);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(ಸ್ಥಿರಾಂಕ UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USet_ಹರವು_Condition ಹರವು_Condition,
                               UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    UnicodeString ತಾತ್ಕಾಲಿಕ_Dest;  // Don't throw away destination buffer between iterations.
    ಗಾಗಿ(int32_t prev_ಹರವು_ಎಲ್ಲೆ=0; prev_ಹರವು_ಎಲ್ಲೆ<src.length();) {
        int32_t ಹರವು_ಎಲ್ಲೆ=ಗಣ.ಹರವು(src, prev_ಹರವು_ಎಲ್ಲೆ, ಹರವು_Condition);
        int32_t ಹರವು_ಉದ್ದಳತೆ=ಹರವು_ಎಲ್ಲೆ-prev_ಹರವು_ಎಲ್ಲೆ;
        ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_NOT_CONTAINED) {
            ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ!=0) {
                dest.append(src, prev_ಹರವು_ಎಲ್ಲೆ, ಹರವು_ಉದ್ದಳತೆ);
            }
            ಹರವು_Condition=USET_ಹರವು_SIMPLE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(ಹರವು_ಉದ್ದಳತೆ!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.ತಾತ್ಕಾಲಿಕ_Sub_String_Between(prev_ಹರವು_ಎಲ್ಲೆ, ಹರವು_ಎಲ್ಲೆ),
                                            ತಾತ್ಕಾಲಿಕ_Dest, error_ಸಂಕೇತ));
                ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಹರವು_Condition=USET_ಹರವು_NOT_CONTAINED;
        }
        prev_ಹರವು_ಎಲ್ಲೆ=ಹರವು_ಎಲ್ಲೆ;
    }
    ಹಿಂತಿರುಗಿ dest;
}

ಶೂನ್ಯ
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.ದತ್ತಾಂಶ(), src.length(), sink, edits, USET_ಹರವು_SIMPLE, error_ಸಂಕೇತ);
}

ಶೂನ್ಯ
FilteredNormalizer2::normalizeUTF8(uint32_t options, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USet_ಹರವು_Condition ಹರವು_Condition,
                                   UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆಗ (length > 0) {
        int32_t ಹರವು_ಉದ್ದಳತೆ = ಗಣ.ಹರವು_UTF8(src, length, ಹರವು_Condition);
        ಆದರೆ (ಹರವು_Condition == USET_ಹರವು_NOT_CONTAINED) {
            ಆದರೆ (ಹರವು_ಉದ್ದಳತೆ != 0) {
                ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
                    edits->ಕೂಡಿಸು_Unchanged(ಹರವು_ಉದ್ದಳತೆ);
                }
                ಆದರೆ ((options & U_OMIT_UNCHANGED_ಪಠ್ಯ) == 0) {
                    sink.Append(src, ಹರವು_ಉದ್ದಳತೆ);
                }
            }
            ಹರವು_Condition = USET_ಹರವು_SIMPLE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (ಹರವು_ಉದ್ದಳತೆ != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, ಹರವು_ಉದ್ದಳತೆ), sink, edits, error_ಸಂಕೇತ);
                ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಹರವು_Condition = USET_ಹರವು_NOT_CONTAINED;
        }
        src += ಹರವು_ಉದ್ದಳತೆ;
        length -= ಹರವು_ಉದ್ದಳತೆ;
    }
}

UnicodeString&
FilteredNormalizer2::normalize_ಎರಡನೇ_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ಸ್ಥಿರಾಂಕ UnicodeString& ಎರಡನೇ,
                                              UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ normalize_ಎರಡನೇ_And_Append(first, ಎರಡನೇ, ನಿಜ, error_ಸಂಕೇತ);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ಸ್ಥಿರಾಂಕ UnicodeString& ಎರಡನೇ,
                            UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ normalize_ಎರಡನೇ_And_Append(first, ಎರಡನೇ, ಸುಳ್ಳು, error_ಸಂಕೇತ);
}

UnicodeString&
FilteredNormalizer2::normalize_ಎರಡನೇ_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ಸ್ಥಿರಾಂಕ UnicodeString& ಎರಡನೇ,
                                              UBool doNormalize,
                                              UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    uprv_check_Can_Get_ಬಫರ್(first, error_ಸಂಕೇತ);
    uprv_check_Can_Get_ಬಫರ್(ಎರಡನೇ, error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ first;
    }
    ಆದರೆ(&first==&ಎರಡನೇ) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ first;
    }
    ಆದರೆ(first.is_ಖಾಲಿಗಳು()) {
        ಆದರೆ(doNormalize) {
            ಹಿಂತಿರುಗಿ normalize(ಎರಡನೇ, first, error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ first=ಎರಡನೇ;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t ಪೂರ್ವಪ್ರತ್ಯಯ_ಎಲ್ಲೆ=ಗಣ.ಹರವು(ಎರಡನೇ, 0, USET_ಹರವು_SIMPLE);
    ಆದರೆ(ಪೂರ್ವಪ್ರತ್ಯಯ_ಎಲ್ಲೆ!=0) {
        UnicodeString ಪೂರ್ವಪ್ರತ್ಯಯ(ಎರಡನೇ.ತಾತ್ಕಾಲಿಕ_Sub_String(0, ಪೂರ್ವಪ್ರತ್ಯಯ_ಎಲ್ಲೆ));
        int32_t suffix_ಆರಂಭ=ಗಣ.ಹರವು_ಹಿಂಭಾಗ(first, INT32_MAX, USET_ಹರವು_SIMPLE);
        ಆದರೆ(suffix_ಆರಂಭ==0) {
            ಆದರೆ(doNormalize) {
                norm2.normalize_ಎರಡನೇ_And_Append(first, ಪೂರ್ವಪ್ರತ್ಯಯ, error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                norm2.append(first, ಪೂರ್ವಪ್ರತ್ಯಯ, error_ಸಂಕೇತ);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            UnicodeString ಮಧ್ಯ(first, suffix_ಆರಂಭ, INT32_MAX);
            ಆದರೆ(doNormalize) {
                norm2.normalize_ಎರಡನೇ_And_Append(ಮಧ್ಯ, ಪೂರ್ವಪ್ರತ್ಯಯ, error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                norm2.append(ಮಧ್ಯ, ಪೂರ್ವಪ್ರತ್ಯಯ, error_ಸಂಕೇತ);
            }
            first.replace(suffix_ಆರಂಭ, INT32_MAX, ಮಧ್ಯ);
        }
    }
    ಆದರೆ(ಪೂರ್ವಪ್ರತ್ಯಯ_ಎಲ್ಲೆ<ಎರಡನೇ.length()) {
        UnicodeString rest(ಎರಡನೇ.ತಾತ್ಕಾಲಿಕ_Sub_String(ಪೂರ್ವಪ್ರತ್ಯಯ_ಎಲ್ಲೆ, INT32_MAX));
        ಆದರೆ(doNormalize) {
            normalize(rest, first, USET_ಹರವು_NOT_CONTAINED, error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            first.append(rest);
        }
    }
    ಹಿಂತಿರುಗಿ first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಗಣ.ಒಳಗೊಂಡಿದೆ(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಗಣ.ಒಳಗೊಂಡಿದೆ(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ (ಗಣ.ಒಳಗೊಂಡಿದೆ(a) && ಗಣ.ಒಳಗೊಂಡಿದೆ(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಗಣ.ಒಳಗೊಂಡಿದೆ(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ಸ್ಥಿರಾಂಕ UnicodeString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    uprv_check_Can_Get_ಬಫರ್(s, error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    USet_ಹರವು_Condition ಹರವು_Condition=USET_ಹರವು_SIMPLE;
    ಗಾಗಿ(int32_t prev_ಹರವು_ಎಲ್ಲೆ=0; prev_ಹರವು_ಎಲ್ಲೆ<s.length();) {
        int32_t ಹರವು_ಎಲ್ಲೆ=ಗಣ.ಹರವು(s, prev_ಹರವು_ಎಲ್ಲೆ, ಹರವು_Condition);
        ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_NOT_CONTAINED) {
            ಹರವು_Condition=USET_ಹರವು_SIMPLE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ( !norm2.isNormalized(s.ತಾತ್ಕಾಲಿಕ_Sub_String_Between(prev_ಹರವು_ಎಲ್ಲೆ, ಹರವು_ಎಲ್ಲೆ), error_ಸಂಕೇತ) ||
                U_FAILURE(error_ಸಂಕೇತ)
            ) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಹರವು_Condition=USET_ಹರವು_NOT_CONTAINED;
        }
        prev_ಹರವು_ಎಲ್ಲೆ=ಹರವು_ಎಲ್ಲೆ;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = sp.ದತ್ತಾಂಶ();
    int32_t length = sp.length();
    USet_ಹರವು_Condition ಹರವು_Condition = USET_ಹರವು_SIMPLE;
    ಆಗ (length > 0) {
        int32_t ಹರವು_ಉದ್ದಳತೆ = ಗಣ.ಹರವು_UTF8(s, length, ಹರವು_Condition);
        ಆದರೆ (ಹರವು_Condition == USET_ಹರವು_NOT_CONTAINED) {
            ಹರವು_Condition = USET_ಹರವು_SIMPLE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (!norm2.isNormalizedUTF8(StringPiece(s, ಹರವು_ಉದ್ದಳತೆ), error_ಸಂಕೇತ) ||
                    U_FAILURE(error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಹರವು_Condition = USET_ಹರವು_NOT_CONTAINED;
        }
        s += ಹರವು_ಉದ್ದಳತೆ;
        length -= ಹರವು_ಉದ್ದಳತೆ;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UNormalization_Check_ಫಲಿತಾಂಶ
FilteredNormalizer2::ಶೀಘ್ರ_Check(ಸ್ಥಿರಾಂಕ UnicodeString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    uprv_check_Can_Get_ಬಫರ್(s, error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ UNORM_MAYBE;
    }
    UNormalization_Check_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ=UNORM_YES;
    USet_ಹರವು_Condition ಹರವು_Condition=USET_ಹರವು_SIMPLE;
    ಗಾಗಿ(int32_t prev_ಹರವು_ಎಲ್ಲೆ=0; prev_ಹರವು_ಎಲ್ಲೆ<s.length();) {
        int32_t ಹರವು_ಎಲ್ಲೆ=ಗಣ.ಹರವು(s, prev_ಹರವು_ಎಲ್ಲೆ, ಹರವು_Condition);
        ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_NOT_CONTAINED) {
            ಹರವು_Condition=USET_ಹರವು_SIMPLE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            UNormalization_Check_ಫಲಿತಾಂಶ qc_ಫಲಿತಾಂಶ=
                norm2.ಶೀಘ್ರ_Check(s.ತಾತ್ಕಾಲಿಕ_Sub_String_Between(prev_ಹರವು_ಎಲ್ಲೆ, ಹರವು_ಎಲ್ಲೆ), error_ಸಂಕೇತ);
            ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ) || qc_ಫಲಿತಾಂಶ==UNORM_NO) {
                ಹಿಂತಿರುಗಿ qc_ಫಲಿತಾಂಶ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(qc_ಫಲಿತಾಂಶ==UNORM_MAYBE) {
                ಫಲಿತಾಂಶ=qc_ಫಲಿತಾಂಶ;
            }
            ಹರವು_Condition=USET_ಹರವು_NOT_CONTAINED;
        }
        prev_ಹರವು_ಎಲ್ಲೆ=ಹರವು_ಎಲ್ಲೆ;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

int32_t
FilteredNormalizer2::ಹರವು_ಶೀಘ್ರ_Check_Yes(ಸ್ಥಿರಾಂಕ UnicodeString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    uprv_check_Can_Get_ಬಫರ್(s, error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    USet_ಹರವು_Condition ಹರವು_Condition=USET_ಹರವು_SIMPLE;
    ಗಾಗಿ(int32_t prev_ಹರವು_ಎಲ್ಲೆ=0; prev_ಹರವು_ಎಲ್ಲೆ<s.length();) {
        int32_t ಹರವು_ಎಲ್ಲೆ=ಗಣ.ಹರವು(s, prev_ಹರವು_ಎಲ್ಲೆ, ಹರವು_Condition);
        ಆದರೆ(ಹರವು_Condition==USET_ಹರವು_NOT_CONTAINED) {
            ಹರವು_Condition=USET_ಹರವು_SIMPLE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            int32_t yes_ಎಲ್ಲೆ=
                prev_ಹರವು_ಎಲ್ಲೆ+
                norm2.ಹರವು_ಶೀಘ್ರ_Check_Yes(
                    s.ತಾತ್ಕಾಲಿಕ_Sub_String_Between(prev_ಹರವು_ಎಲ್ಲೆ, ಹರವು_ಎಲ್ಲೆ), error_ಸಂಕೇತ);
            ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ) || yes_ಎಲ್ಲೆ<ಹರವು_ಎಲ್ಲೆ) {
                ಹಿಂತಿರುಗಿ yes_ಎಲ್ಲೆ;
            }
            ಹರವು_Condition=USET_ಹರವು_NOT_CONTAINED;
        }
        prev_ಹರವು_ಎಲ್ಲೆ=ಹರವು_ಎಲ್ಲೆ;
    }
    ಹಿಂತಿರುಗಿ s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ !ಗಣ.ಒಳಗೊಂಡಿದೆ(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ !ಗಣ.ಒಳಗೊಂಡಿದೆ(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ !ಗಣ.ಒಳಗೊಂಡಿದೆ(c) || norm2.isInert(c);
}

U_NAMESPACE_ಕೊನೆ

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ಸ್ಥಿರಾಂಕ UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ಸ್ಥಿರಾಂಕ USet* filter_ಗಣ U_LIFETIME_BOUND,
                    UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(filter_ಗಣ==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    Normalizer2 *fn2=ಹೊಸ FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *Unicode_ಗಣ::fromUSet(filter_ಗಣ));
    ಆದರೆ(fn2==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಹಿಂತಿರುಗಿ (UNormalizer2 *)fn2;
}

#ಆದರೆ_ಅಂತ್ಯ  // !UCONFIG_NO_NORMALIZATION
