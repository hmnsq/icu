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

#ضمن "unicode/utypes.h"

#ܐܢ !UCONFIG_NO_NORMALIZATION

#ضمن "unicode/edits.h"
#ضمن "unicode/normalizer2.h"
#ضمن "unicode/stringoptions.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/unorm.h"
#ضمن "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(ثابت UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UErrorCode& errorCode) ثابت {
    uprv_checkCanGetBuffer(src, errorCode);
    ܐܢ(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ܦܢܐ dest;
    }
    ܐܢ(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ dest;
    }
    dest.remove();
    ܦܢܐ normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(ثابت UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USetSpanCondition spanCondition,
                               UErrorCode& errorCode) ثابت {
    UnicodeString tempDest;  // Don't throw away destination buffer between iterations.
    لكل(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        ܐܢ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            ܐܢ(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } ܐܚܪܢܐ {
            ܐܢ(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                ܐܢ(U_FAILURE(errorCode)) {
                    اكسر;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ܦܢܐ dest;
}

فراغ
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) ثابت {
    ܐܢ (U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    ܐܢ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.data(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

فراغ
FilteredNormalizer2::normalizeUTF8(uint32_t options, ثابت حرف *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) ثابت {
    ܟܡܐ (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        ܐܢ (spanCondition == USET_SPAN_NOT_CONTAINED) {
            ܐܢ (spanLength != 0) {
                ܐܢ (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                ܐܢ ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } ܐܚܪܢܐ {
            ܐܢ (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                ܐܢ (U_FAILURE(errorCode)) {
                    اكسر;
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
                                              ثابت UnicodeString& second,
                                              UErrorCode& errorCode) ثابت {
    ܦܢܐ normalizeSecondAndAppend(first, second, ܫܪܪܐ, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ثابت UnicodeString& second,
                            UErrorCode& errorCode) ثابت {
    ܦܢܐ normalizeSecondAndAppend(first, second, ܕܓܠܐ, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              ثابت UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) ثابت {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ first;
    }
    ܐܢ(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ first;
    }
    ܐܢ(first.isEmpty()) {
        ܐܢ(doNormalize) {
            ܦܢܐ normalize(second, first, errorCode);
        } ܐܚܪܢܐ {
            ܦܢܐ first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    ܐܢ(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        ܐܢ(suffixStart==0) {
            ܐܢ(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } ܐܚܪܢܐ {
                norm2.append(first, prefix, errorCode);
            }
        } ܐܚܪܢܐ {
            UnicodeString middle(first, suffixStart, INT32_MAX);
            ܐܢ(doNormalize) {
                norm2.normalizeSecondAndAppend(middle, prefix, errorCode);
            } ܐܚܪܢܐ {
                norm2.append(middle, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, middle);
        }
    }
    ܐܢ(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        ܐܢ(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } ܐܚܪܢܐ {
            first.append(rest);
        }
    }
    ܦܢܐ first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    ܦܢܐ set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    ܦܢܐ set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ثابت {
    ܦܢܐ (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ثابت {
    ܦܢܐ set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت {
    uprv_checkCanGetBuffer(s, errorCode);
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    لكل(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ܐܢ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ܐܚܪܢܐ {
            ܐܢ( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                ܦܢܐ ܕܓܠܐ;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ܦܢܐ ܫܪܪܐ;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ثابت {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ثابت حرف *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    ܟܡܐ (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        ܐܢ (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } ܐܚܪܢܐ {
            ܐܢ (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                ܦܢܐ ܕܓܠܐ;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    ܦܢܐ ܫܪܪܐ;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت {
    uprv_checkCanGetBuffer(s, errorCode);
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    لكل(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ܐܢ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ܐܚܪܢܐ {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            ܐܢ(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                ܦܢܐ qcResult;
            } ܐܚܪܢܐ ܐܢ(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ܦܢܐ result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت {
    uprv_checkCanGetBuffer(s, errorCode);
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    لكل(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ܐܢ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ܐܚܪܢܐ {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            ܐܢ(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                ܦܢܐ yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ܦܢܐ s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ثابت {
    ܦܢܐ !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ثابت {
    ܦܢܐ !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ثابت {
    ܦܢܐ !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ثابت UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ثابت USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    ܐܢ(U_FAILURE(*pErrorCode)) {
        ܦܢܐ nullptr;
    }
    ܐܢ(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ nullptr;
    }
    Normalizer2 *fn2=جديد FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    ܐܢ(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ܦܢܐ (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
