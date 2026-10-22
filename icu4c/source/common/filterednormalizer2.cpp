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

#include "unicode/utypes.h"

#ꦪꦼꦤ꧀ !UCONFIG_NO_NORMALIZATION

#include "unicode/edits.h"
#include "unicode/normalizer2.h"
#include "unicode/stringoptions.h"
#include "unicode/uniset.h"
#include "unicode/unistr.h"
#include "unicode/unorm.h"
#include "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(const UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UErrorCode& errorCode) const {
    uprv_checkCanGetBuffer(src, errorCode);
    ꦪꦼꦤ꧀(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ꦮꦭꦶ dest;
    }
    ꦪꦼꦤ꧀(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ dest;
    }
    dest.remove();
    ꦮꦭꦶ normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(const UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USetSpanCondition spanCondition,
                               UErrorCode& errorCode) const {
    UnicodeString tempDest;  // Don't throw away destination buffer between iterations.
    ꦏꦁꦒꦺꦴ(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        ꦪꦼꦤ꧀(spanCondition==USET_SPAN_NOT_CONTAINED) {
            ꦪꦼꦤ꧀(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } ꦭꦶꦪ {
            ꦪꦼꦤ꧀(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                ꦪꦼꦤ꧀(U_FAILURE(errorCode)) {
                    bha_nga;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ꦮꦭꦶ dest;
}

void
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) const {
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) {
        ꦮꦭꦶ;
    }
    ꦪꦼꦤ꧀ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.data(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

void
FilteredNormalizer2::normalizeUTF8(uint32_t options, const var_na *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) const {
    ꦱꦼꦢꦼꦭ (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        ꦪꦼꦤ꧀ (spanCondition == USET_SPAN_NOT_CONTAINED) {
            ꦪꦼꦤ꧀ (spanLength != 0) {
                ꦪꦼꦤ꧀ (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                ꦪꦼꦤ꧀ ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } ꦭꦶꦪ {
            ꦪꦼꦤ꧀ (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) {
                    bha_nga;
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
                                              const UnicodeString& second,
                                              UErrorCode& errorCode) const {
    ꦮꦭꦶ normalizeSecondAndAppend(first, second, true, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            const UnicodeString& second,
                            UErrorCode& errorCode) const {
    ꦮꦭꦶ normalizeSecondAndAppend(first, second, false, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              const UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) const {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    ꦪꦼꦤ꧀(U_FAILURE(errorCode)) {
        ꦮꦭꦶ first;
    }
    ꦪꦼꦤ꧀(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ first;
    }
    ꦪꦼꦤ꧀(first.isEmpty()) {
        ꦪꦼꦤ꧀(doNormalize) {
            ꦮꦭꦶ normalize(second, first, errorCode);
        } ꦭꦶꦪ {
            ꦮꦭꦶ first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    ꦪꦼꦤ꧀(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        ꦪꦼꦤ꧀(suffixStart==0) {
            ꦪꦼꦤ꧀(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } ꦭꦶꦪ {
                norm2.append(first, prefix, errorCode);
            }
        } ꦭꦶꦪ {
            UnicodeString middle(first, suffixStart, INT32_MAX);
            ꦪꦼꦤ꧀(doNormalize) {
                norm2.normalizeSecondAndAppend(middle, prefix, errorCode);
            } ꦭꦶꦪ {
                norm2.append(middle, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, middle);
        }
    }
    ꦪꦼꦤ꧀(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        ꦪꦼꦤ꧀(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } ꦭꦶꦪ {
            first.append(rest);
        }
    }
    ꦮꦭꦶ first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) const {
    ꦮꦭꦶ set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) const {
    ꦮꦭꦶ set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) const {
    ꦮꦭꦶ (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) const {
    ꦮꦭꦶ set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(const UnicodeString &s, UErrorCode &errorCode) const {
    uprv_checkCanGetBuffer(s, errorCode);
    ꦪꦼꦤ꧀(U_FAILURE(errorCode)) {
        ꦮꦭꦶ false;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    ꦏꦁꦒꦺꦴ(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ꦪꦼꦤ꧀(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ꦭꦶꦪ {
            ꦪꦼꦤ꧀( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                ꦮꦭꦶ false;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ꦮꦭꦶ true;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) const {
    ꦪꦼꦤ꧀(U_FAILURE(errorCode)) {
        ꦮꦭꦶ false;
    }
    const var_na *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    ꦱꦼꦢꦼꦭ (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        ꦪꦼꦤ꧀ (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } ꦭꦶꦪ {
            ꦪꦼꦤ꧀ (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                ꦮꦭꦶ false;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    ꦮꦭꦶ true;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(const UnicodeString &s, UErrorCode &errorCode) const {
    uprv_checkCanGetBuffer(s, errorCode);
    ꦪꦼꦤ꧀(U_FAILURE(errorCode)) {
        ꦮꦭꦶ UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    ꦏꦁꦒꦺꦴ(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ꦪꦼꦤ꧀(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ꦭꦶꦪ {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            ꦪꦼꦤ꧀(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                ꦮꦭꦶ qcResult;
            } ꦭꦶꦪ ꦪꦼꦤ꧀(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ꦮꦭꦶ result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(const UnicodeString &s, UErrorCode &errorCode) const {
    uprv_checkCanGetBuffer(s, errorCode);
    ꦪꦼꦤ꧀(U_FAILURE(errorCode)) {
        ꦮꦭꦶ 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    ꦏꦁꦒꦺꦴ(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ꦪꦼꦤ꧀(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ꦭꦶꦪ {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            ꦪꦼꦤ꧀(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                ꦮꦭꦶ yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ꦮꦭꦶ s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) const {
    ꦮꦭꦶ !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) const {
    ꦮꦭꦶ !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) const {
    ꦮꦭꦶ !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(const UNormalizer2* norm2 U_LIFETIME_BOUND,
                    const USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    ꦪꦼꦤ꧀(U_FAILURE(*pErrorCode)) {
        ꦮꦭꦶ nullptr;
    }
    ꦪꦼꦤ꧀(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ nullptr;
    }
    Normalizer2 *fn2=new FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    ꦪꦼꦤ꧀(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ꦮꦭꦶ (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
