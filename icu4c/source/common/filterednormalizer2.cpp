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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_NORMALIZATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/edits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/normalizer2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringoptions.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unorm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(ਅਚਲ UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    uprv_check_Can_Get_ਬਫਰ(src, error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        dest.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਵਾਪਸ dest;
    }
    ਜੇ(&dest==&src) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ dest;
    }
    dest.remove();
    ਵਾਪਸ normalize(src, dest, USET_ਵਿਸਤਾਰ_SIMPLE, error_ਕੋਡ);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(ਅਚਲ UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition,
                               UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    UnicodeString ਆਰਜ਼ੀ_Dest;  // Don't throw away destination buffer between iterations.
    ਲਈ(int32_t prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ=0; prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ<src.length();) {
        int32_t ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ=ਸੈੱਟ.ਵਿਸਤਾਰ(src, prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ, ਵਿਸਤਾਰ_Condition);
        int32_t ਵਿਸਤਾਰ_ਲੰਮਾਈ=ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ-prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ;
        ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
            ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ!=0) {
                dest.append(src, prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ, ਵਿਸਤਾਰ_ਲੰਮਾਈ);
            }
            ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_SIMPLE;
        } ਨਹੀਂਤਾਂ {
            ਜੇ(ਵਿਸਤਾਰ_ਲੰਮਾਈ!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.ਆਰਜ਼ੀ_Sub_String_Between(prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ, ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ),
                                            ਆਰਜ਼ੀ_Dest, error_ਕੋਡ));
                ਜੇ(U_FAILURE(error_ਕੋਡ)) {
                    ਰੋਕੋ;
                }
            }
            ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_NOT_CONTAINED;
        }
        prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ=ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ;
    }
    ਵਾਪਸ dest;
}

ਖਾਲੀ
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.ਅੰਕੜੇ(), src.length(), sink, edits, USET_ਵਿਸਤਾਰ_SIMPLE, error_ਕੋਡ);
}

ਖਾਲੀ
FilteredNormalizer2::normalizeUTF8(uint32_t options, ਅਚਲ ਅੱਖਰ *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition,
                                   UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜਦੋਂ (length > 0) {
        int32_t ਵਿਸਤਾਰ_ਲੰਮਾਈ = ਸੈੱਟ.ਵਿਸਤਾਰ_UTF8(src, length, ਵਿਸਤਾਰ_Condition);
        ਜੇ (ਵਿਸਤਾਰ_Condition == USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
            ਜੇ (ਵਿਸਤਾਰ_ਲੰਮਾਈ != 0) {
                ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
                    edits->ਜੋੜੋ_Unchanged(ਵਿਸਤਾਰ_ਲੰਮਾਈ);
                }
                ਜੇ ((options & U_OMIT_UNCHANGED_ਪਾਠ) == 0) {
                    sink.Append(src, ਵਿਸਤਾਰ_ਲੰਮਾਈ);
                }
            }
            ਵਿਸਤਾਰ_Condition = USET_ਵਿਸਤਾਰ_SIMPLE;
        } ਨਹੀਂਤਾਂ {
            ਜੇ (ਵਿਸਤਾਰ_ਲੰਮਾਈ != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, ਵਿਸਤਾਰ_ਲੰਮਾਈ), sink, edits, error_ਕੋਡ);
                ਜੇ (U_FAILURE(error_ਕੋਡ)) {
                    ਰੋਕੋ;
                }
            }
            ਵਿਸਤਾਰ_Condition = USET_ਵਿਸਤਾਰ_NOT_CONTAINED;
        }
        src += ਵਿਸਤਾਰ_ਲੰਮਾਈ;
        length -= ਵਿਸਤਾਰ_ਲੰਮਾਈ;
    }
}

UnicodeString&
FilteredNormalizer2::normalize_ਦੂਜਾ_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ਅਚਲ UnicodeString& ਦੂਜਾ,
                                              UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    ਵਾਪਸ normalize_ਦੂਜਾ_And_Append(first, ਦੂਜਾ, ਸੱਚ, error_ਕੋਡ);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ਅਚਲ UnicodeString& ਦੂਜਾ,
                            UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    ਵਾਪਸ normalize_ਦੂਜਾ_And_Append(first, ਦੂਜਾ, ਝੂਠ, error_ਕੋਡ);
}

UnicodeString&
FilteredNormalizer2::normalize_ਦੂਜਾ_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ਅਚਲ UnicodeString& ਦੂਜਾ,
                                              UBool doNormalize,
                                              UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    uprv_check_Can_Get_ਬਫਰ(first, error_ਕੋਡ);
    uprv_check_Can_Get_ਬਫਰ(ਦੂਜਾ, error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ first;
    }
    ਜੇ(&first==&ਦੂਜਾ) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ first;
    }
    ਜੇ(first.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        ਜੇ(doNormalize) {
            ਵਾਪਸ normalize(ਦੂਜਾ, first, error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ first=ਦੂਜਾ;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t ਅਗੇਤਰ_ਹੱਦਬੰਦੀ=ਸੈੱਟ.ਵਿਸਤਾਰ(ਦੂਜਾ, 0, USET_ਵਿਸਤਾਰ_SIMPLE);
    ਜੇ(ਅਗੇਤਰ_ਹੱਦਬੰਦੀ!=0) {
        UnicodeString ਅਗੇਤਰ(ਦੂਜਾ.ਆਰਜ਼ੀ_Sub_String(0, ਅਗੇਤਰ_ਹੱਦਬੰਦੀ));
        int32_t suffix_ਸ਼ੁਰੂ=ਸੈੱਟ.ਵਿਸਤਾਰ_ਪਿੱਠ(first, INT32_MAX, USET_ਵਿਸਤਾਰ_SIMPLE);
        ਜੇ(suffix_ਸ਼ੁਰੂ==0) {
            ਜੇ(doNormalize) {
                norm2.normalize_ਦੂਜਾ_And_Append(first, ਅਗੇਤਰ, error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ {
                norm2.append(first, ਅਗੇਤਰ, error_ਕੋਡ);
            }
        } ਨਹੀਂਤਾਂ {
            UnicodeString ਵਿਚਕਾਰ(first, suffix_ਸ਼ੁਰੂ, INT32_MAX);
            ਜੇ(doNormalize) {
                norm2.normalize_ਦੂਜਾ_And_Append(ਵਿਚਕਾਰ, ਅਗੇਤਰ, error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ {
                norm2.append(ਵਿਚਕਾਰ, ਅਗੇਤਰ, error_ਕੋਡ);
            }
            first.replace(suffix_ਸ਼ੁਰੂ, INT32_MAX, ਵਿਚਕਾਰ);
        }
    }
    ਜੇ(ਅਗੇਤਰ_ਹੱਦਬੰਦੀ<ਦੂਜਾ.length()) {
        UnicodeString rest(ਦੂਜਾ.ਆਰਜ਼ੀ_Sub_String(ਅਗੇਤਰ_ਹੱਦਬੰਦੀ, INT32_MAX));
        ਜੇ(doNormalize) {
            normalize(rest, first, USET_ਵਿਸਤਾਰ_NOT_CONTAINED, error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ {
            first.append(rest);
        }
    }
    ਵਾਪਸ first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ਅਚਲ {
    ਵਾਪਸ ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ਅਚਲ {
    ਵਾਪਸ ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ਅਚਲ {
    ਵਾਪਸ (ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(a) && ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ਅਚਲ {
    ਵਾਪਸ ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ਅਚਲ UnicodeString &s, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    uprv_check_Can_Get_ਬਫਰ(s, error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_SIMPLE;
    ਲਈ(int32_t prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ=0; prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ<s.length();) {
        int32_t ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ=ਸੈੱਟ.ਵਿਸਤਾਰ(s, prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ, ਵਿਸਤਾਰ_Condition);
        ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
            ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_SIMPLE;
        } ਨਹੀਂਤਾਂ {
            ਜੇ( !norm2.isNormalized(s.ਆਰਜ਼ੀ_Sub_String_Between(prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ, ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ), error_ਕੋਡ) ||
                U_FAILURE(error_ਕੋਡ)
            ) {
                ਵਾਪਸ ਝੂਠ;
            }
            ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_NOT_CONTAINED;
        }
        prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ=ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ;
    }
    ਵਾਪਸ ਸੱਚ;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ ਅੱਖਰ *s = sp.ਅੰਕੜੇ();
    int32_t length = sp.length();
    USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition = USET_ਵਿਸਤਾਰ_SIMPLE;
    ਜਦੋਂ (length > 0) {
        int32_t ਵਿਸਤਾਰ_ਲੰਮਾਈ = ਸੈੱਟ.ਵਿਸਤਾਰ_UTF8(s, length, ਵਿਸਤਾਰ_Condition);
        ਜੇ (ਵਿਸਤਾਰ_Condition == USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
            ਵਿਸਤਾਰ_Condition = USET_ਵਿਸਤਾਰ_SIMPLE;
        } ਨਹੀਂਤਾਂ {
            ਜੇ (!norm2.isNormalizedUTF8(StringPiece(s, ਵਿਸਤਾਰ_ਲੰਮਾਈ), error_ਕੋਡ) ||
                    U_FAILURE(error_ਕੋਡ)) {
                ਵਾਪਸ ਝੂਠ;
            }
            ਵਿਸਤਾਰ_Condition = USET_ਵਿਸਤਾਰ_NOT_CONTAINED;
        }
        s += ਵਿਸਤਾਰ_ਲੰਮਾਈ;
        length -= ਵਿਸਤਾਰ_ਲੰਮਾਈ;
    }
    ਵਾਪਸ ਸੱਚ;
}

UNormalization_Check_ਨਤੀਜਾ
FilteredNormalizer2::ਤੇਜ਼_Check(ਅਚਲ UnicodeString &s, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    uprv_check_Can_Get_ਬਫਰ(s, error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ UNORM_MAYBE;
    }
    UNormalization_Check_ਨਤੀਜਾ ਨਤੀਜਾ=UNORM_YES;
    USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_SIMPLE;
    ਲਈ(int32_t prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ=0; prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ<s.length();) {
        int32_t ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ=ਸੈੱਟ.ਵਿਸਤਾਰ(s, prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ, ਵਿਸਤਾਰ_Condition);
        ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
            ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_SIMPLE;
        } ਨਹੀਂਤਾਂ {
            UNormalization_Check_ਨਤੀਜਾ qc_ਨਤੀਜਾ=
                norm2.ਤੇਜ਼_Check(s.ਆਰਜ਼ੀ_Sub_String_Between(prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ, ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ), error_ਕੋਡ);
            ਜੇ(U_FAILURE(error_ਕੋਡ) || qc_ਨਤੀਜਾ==UNORM_NO) {
                ਵਾਪਸ qc_ਨਤੀਜਾ;
            } ਨਹੀਂਤਾਂ ਜੇ(qc_ਨਤੀਜਾ==UNORM_MAYBE) {
                ਨਤੀਜਾ=qc_ਨਤੀਜਾ;
            }
            ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_NOT_CONTAINED;
        }
        prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ=ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

int32_t
FilteredNormalizer2::ਵਿਸਤਾਰ_ਤੇਜ਼_Check_Yes(ਅਚਲ UnicodeString &s, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    uprv_check_Can_Get_ਬਫਰ(s, error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_SIMPLE;
    ਲਈ(int32_t prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ=0; prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ<s.length();) {
        int32_t ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ=ਸੈੱਟ.ਵਿਸਤਾਰ(s, prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ, ਵਿਸਤਾਰ_Condition);
        ਜੇ(ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
            ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_SIMPLE;
        } ਨਹੀਂਤਾਂ {
            int32_t yes_ਹੱਦਬੰਦੀ=
                prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ+
                norm2.ਵਿਸਤਾਰ_ਤੇਜ਼_Check_Yes(
                    s.ਆਰਜ਼ੀ_Sub_String_Between(prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ, ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ), error_ਕੋਡ);
            ਜੇ(U_FAILURE(error_ਕੋਡ) || yes_ਹੱਦਬੰਦੀ<ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ) {
                ਵਾਪਸ yes_ਹੱਦਬੰਦੀ;
            }
            ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_NOT_CONTAINED;
        }
        prev_ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ=ਵਿਸਤਾਰ_ਹੱਦਬੰਦੀ;
    }
    ਵਾਪਸ s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ਅਚਲ {
    ਵਾਪਸ !ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ਅਚਲ {
    ਵਾਪਸ !ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ਅਚਲ {
    ਵਾਪਸ !ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c) || norm2.isInert(c);
}

U_NAMESPACE_ਸਿਰਾ

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ਅਚਲ UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ਅਚਲ USet* filter_ਸੈੱਟ U_LIFETIME_BOUND,
                    UError_ਕੋਡ* p_Error_ਕੋਡ) {
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(filter_ਸੈੱਟ==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    Normalizer2 *fn2=ਨਵਾਂ FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *Unicode_ਸੈੱਟ::fromUSet(filter_ਸੈੱਟ));
    ਜੇ(fn2==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਵਾਪਸ (UNormalizer2 *)fn2;
}

#ਜੇ_ਅੰਤ  // !UCONFIG_NO_NORMALIZATION
