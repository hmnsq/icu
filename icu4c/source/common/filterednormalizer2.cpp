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

#ಯದಿ !UCONFIG_NO_NORMALIZATION

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
FilteredNormalizer2::normalize(ಧ್ರುವ UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UErrorCode& errorCode) ಧ್ರುವ {
    uprv_checkCanGetBuffer(src, errorCode);
    ಯದಿ(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ಪ್ರತ್ಯಾಗಮ dest;
    }
    ಯದಿ(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ dest;
    }
    dest.remove();
    ಪ್ರತ್ಯಾಗಮ normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(ಧ್ರುವ UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USetSpanCondition spanCondition,
                               UErrorCode& errorCode) ಧ್ರುವ {
    UnicodeString tempDest;  // Don't throw away destination buffer between iterations.
    ಪ್ರತಿ(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        ಯದಿ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            ಯದಿ(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } ಅನ್ಯಥಾ {
            ಯದಿ(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                ಯದಿ(U_FAILURE(errorCode)) {
                    ಭಙ್ಗ;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ಪ್ರತ್ಯಾಗಮ dest;
}

ಶೂನ್ಯ
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.data(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

ಶೂನ್ಯ
FilteredNormalizer2::normalizeUTF8(uint32_t options, ಧ್ರುವ ವರ್ಣ *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) ಧ್ರುವ {
    ಯಾವತ್ (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        ಯದಿ (spanCondition == USET_SPAN_NOT_CONTAINED) {
            ಯದಿ (spanLength != 0) {
                ಯದಿ (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                ಯದಿ ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } ಅನ್ಯಥಾ {
            ಯದಿ (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                ಯದಿ (U_FAILURE(errorCode)) {
                    ಭಙ್ಗ;
                }
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        src += spanLength;
        length -= spanLength;
    }
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              ಧ್ರುವ UnicodeString& second,
                                              UErrorCode& errorCode) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ normalizeSecondAndAppend(first, second, ಸತ್ಯ, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ಧ್ರುವ UnicodeString& second,
                            UErrorCode& errorCode) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ normalizeSecondAndAppend(first, second, ಅಸತ್ಯ, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              ಧ್ರುವ UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) ಧ್ರುವ {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ first;
    }
    ಯದಿ(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ first;
    }
    ಯದಿ(first.isEmpty()) {
        ಯದಿ(doNormalize) {
            ಪ್ರತ್ಯಾಗಮ normalize(second, first, errorCode);
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    ಯದಿ(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        ಯದಿ(suffixStart==0) {
            ಯದಿ(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } ಅನ್ಯಥಾ {
                norm2.append(first, prefix, errorCode);
            }
        } ಅನ್ಯಥಾ {
            UnicodeString ಮಧ್ಯ(first, suffixStart, INT32_MAX);
            ಯದಿ(doNormalize) {
                norm2.normalizeSecondAndAppend(ಮಧ್ಯ, prefix, errorCode);
            } ಅನ್ಯಥಾ {
                norm2.append(ಮಧ್ಯ, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, ಮಧ್ಯ);
        }
    }
    ಯದಿ(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        ಯದಿ(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } ಅನ್ಯಥಾ {
            first.append(rest);
        }
    }
    ಪ್ರತ್ಯಾಗಮ first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ಧ್ರುವ UnicodeString &s, UErrorCode &errorCode) ಧ್ರುವ {
    uprv_checkCanGetBuffer(s, errorCode);
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    ಪ್ರತಿ(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ಯದಿ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ಅನ್ಯಥಾ {
            ಯದಿ( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ ವರ್ಣ *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    ಯಾವತ್ (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        ಯದಿ (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } ಅನ್ಯಥಾ {
            ಯದಿ (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(ಧ್ರುವ UnicodeString &s, UErrorCode &errorCode) ಧ್ರುವ {
    uprv_checkCanGetBuffer(s, errorCode);
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    ಪ್ರತಿ(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ಯದಿ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ಅನ್ಯಥಾ {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            ಯದಿ(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                ಪ್ರತ್ಯಾಗಮ qcResult;
            } ಅನ್ಯಥಾ ಯದಿ(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(ಧ್ರುವ UnicodeString &s, UErrorCode &errorCode) ಧ್ರುವ {
    uprv_checkCanGetBuffer(s, errorCode);
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    ಪ್ರತಿ(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ಯದಿ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ಅನ್ಯಥಾ {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            ಯದಿ(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                ಪ್ರತ್ಯಾಗಮ yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ಪ್ರತ್ಯಾಗಮ s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ಧ್ರುವ UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ಧ್ರುವ USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    Normalizer2 *fn2=ನೂತನ FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    ಯದಿ(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
