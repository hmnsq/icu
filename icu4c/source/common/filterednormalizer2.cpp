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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_NORMALIZATION

#ଅନ୍ତର୍ଭାଵ "unicode/edits.h"
#ଅନ୍ତର୍ଭାଵ "unicode/normalizer2.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringoptions.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unorm.h"
#ଅନ୍ତର୍ଭାଵ "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(ଅଚଳ UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UErrorCode& errorCode) ଅଚଳ {
    uprv_checkCanGetBuffer(src, errorCode);
    ଯଦି(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ଫେରନ୍ତୁ dest;
    }
    ଯଦି(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ dest;
    }
    dest.remove();
    ଫେରନ୍ତୁ normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(ଅଚଳ UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USetSpanCondition spanCondition,
                               UErrorCode& errorCode) ଅଚଳ {
    UnicodeString tempDest;  // Don't throw away destination buffer between iterations.
    ପାଇଁ(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        ଯଦି(spanCondition==USET_SPAN_NOT_CONTAINED) {
            ଯଦି(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } ଅନ୍ୟଥା {
            ଯଦି(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                ଯଦି(U_FAILURE(errorCode)) {
                    ଭାଙ୍ଗ;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ଫେରନ୍ତୁ dest;
}

ଶୂନ୍ୟ
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.data(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

ଶୂନ୍ୟ
FilteredNormalizer2::normalizeUTF8(uint32_t options, ଅଚଳ ଅକ୍ଷର *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) ଅଚଳ {
    ଯେତେବେଳେ (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        ଯଦି (spanCondition == USET_SPAN_NOT_CONTAINED) {
            ଯଦି (spanLength != 0) {
                ଯଦି (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                ଯଦି ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } ଅନ୍ୟଥା {
            ଯଦି (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                ଯଦି (U_FAILURE(errorCode)) {
                    ଭାଙ୍ଗ;
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
                                              ଅଚଳ UnicodeString& second,
                                              UErrorCode& errorCode) ଅଚଳ {
    ଫେରନ୍ତୁ normalizeSecondAndAppend(first, second, ସତ୍ୟ, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ଅଚଳ UnicodeString& second,
                            UErrorCode& errorCode) ଅଚଳ {
    ଫେରନ୍ତୁ normalizeSecondAndAppend(first, second, ମିଥ୍ୟା, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              ଅଚଳ UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) ଅଚଳ {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ first;
    }
    ଯଦି(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ first;
    }
    ଯଦି(first.isEmpty()) {
        ଯଦି(doNormalize) {
            ଫେରନ୍ତୁ normalize(second, first, errorCode);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    ଯଦି(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        ଯଦି(suffixStart==0) {
            ଯଦି(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } ଅନ୍ୟଥା {
                norm2.append(first, prefix, errorCode);
            }
        } ଅନ୍ୟଥା {
            UnicodeString ମଧ୍ୟ(first, suffixStart, INT32_MAX);
            ଯଦି(doNormalize) {
                norm2.normalizeSecondAndAppend(ମଧ୍ୟ, prefix, errorCode);
            } ଅନ୍ୟଥା {
                norm2.append(ମଧ୍ୟ, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, ମଧ୍ୟ);
        }
    }
    ଯଦି(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        ଯଦି(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } ଅନ୍ୟଥା {
            first.append(rest);
        }
    }
    ଫେରନ୍ତୁ first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ଅଚଳ {
    ଫେରନ୍ତୁ set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ଅଚଳ {
    ଫେରନ୍ତୁ set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ଅଚଳ {
    ଫେରନ୍ତୁ (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ଅଚଳ {
    ଫେରନ୍ତୁ set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ଅଚଳ UnicodeString &s, UErrorCode &errorCode) ଅଚଳ {
    uprv_checkCanGetBuffer(s, errorCode);
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    ପାଇଁ(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ଯଦି(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ଅନ୍ୟଥା {
            ଯଦି( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ ଅକ୍ଷର *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    ଯେତେବେଳେ (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        ଯଦି (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } ଅନ୍ୟଥା {
            ଯଦି (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(ଅଚଳ UnicodeString &s, UErrorCode &errorCode) ଅଚଳ {
    uprv_checkCanGetBuffer(s, errorCode);
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    ପାଇଁ(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ଯଦି(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ଅନ୍ୟଥା {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            ଯଦି(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                ଫେରନ୍ତୁ qcResult;
            } ଅନ୍ୟଥା ଯଦି(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ଫେରନ୍ତୁ result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(ଅଚଳ UnicodeString &s, UErrorCode &errorCode) ଅଚଳ {
    uprv_checkCanGetBuffer(s, errorCode);
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    ପାଇଁ(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ଯଦି(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ଅନ୍ୟଥା {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            ଯଦି(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                ଫେରନ୍ତୁ yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ଫେରନ୍ତୁ s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ଅଚଳ {
    ଫେରନ୍ତୁ !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ଅଚଳ {
    ଫେରନ୍ତୁ !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ଅଚଳ {
    ଫେରନ୍ତୁ !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ଅଚଳ UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ଅଚଳ USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    Normalizer2 *fn2=ନୂତନ FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    ଯଦି(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ଫେରନ୍ତୁ (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
