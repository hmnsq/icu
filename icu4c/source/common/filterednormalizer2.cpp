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

#ຖ້າ !UCONFIG_NO_NORMALIZATION

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
    ຖ້າ(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ສົ່ງຄືນ dest;
    }
    ຖ້າ(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ dest;
    }
    dest.remove();
    ສົ່ງຄືນ normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
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
    ສຳລັບ(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        ຖ້າ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            ຖ້າ(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } ບໍ່ດັ່ງນັ້ນ {
            ຖ້າ(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                ຖ້າ(U_FAILURE(errorCode)) {
                    bha_nga;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ສົ່ງຄືນ dest;
}

void
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) const {
    ຖ້າ (U_FAILURE(errorCode)) {
        ສົ່ງຄືນ;
    }
    ຖ້າ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
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
    ໃນຂະນະທີ່ (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        ຖ້າ (spanCondition == USET_SPAN_NOT_CONTAINED) {
            ຖ້າ (spanLength != 0) {
                ຖ້າ (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                ຖ້າ ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } ບໍ່ດັ່ງນັ້ນ {
            ຖ້າ (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                ຖ້າ (U_FAILURE(errorCode)) {
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
    ສົ່ງຄືນ normalizeSecondAndAppend(first, second, true, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            const UnicodeString& second,
                            UErrorCode& errorCode) const {
    ສົ່ງຄືນ normalizeSecondAndAppend(first, second, false, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              const UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) const {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ first;
    }
    ຖ້າ(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ first;
    }
    ຖ້າ(first.isEmpty()) {
        ຖ້າ(doNormalize) {
            ສົ່ງຄືນ normalize(second, first, errorCode);
        } ບໍ່ດັ່ງນັ້ນ {
            ສົ່ງຄືນ first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    ຖ້າ(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        ຖ້າ(suffixStart==0) {
            ຖ້າ(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } ບໍ່ດັ່ງນັ້ນ {
                norm2.append(first, prefix, errorCode);
            }
        } ບໍ່ດັ່ງນັ້ນ {
            UnicodeString middle(first, suffixStart, INT32_MAX);
            ຖ້າ(doNormalize) {
                norm2.normalizeSecondAndAppend(middle, prefix, errorCode);
            } ບໍ່ດັ່ງນັ້ນ {
                norm2.append(middle, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, middle);
        }
    }
    ຖ້າ(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        ຖ້າ(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } ບໍ່ດັ່ງນັ້ນ {
            first.append(rest);
        }
    }
    ສົ່ງຄືນ first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) const {
    ສົ່ງຄືນ set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) const {
    ສົ່ງຄືນ set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) const {
    ສົ່ງຄືນ (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) const {
    ສົ່ງຄືນ set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(const UnicodeString &s, UErrorCode &errorCode) const {
    uprv_checkCanGetBuffer(s, errorCode);
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ false;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    ສຳລັບ(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ຖ້າ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ບໍ່ດັ່ງນັ້ນ {
            ຖ້າ( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                ສົ່ງຄືນ false;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ສົ່ງຄືນ true;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) const {
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ false;
    }
    const var_na *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    ໃນຂະນະທີ່ (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        ຖ້າ (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } ບໍ່ດັ່ງນັ້ນ {
            ຖ້າ (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                ສົ່ງຄືນ false;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    ສົ່ງຄືນ true;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(const UnicodeString &s, UErrorCode &errorCode) const {
    uprv_checkCanGetBuffer(s, errorCode);
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    ສຳລັບ(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ຖ້າ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ບໍ່ດັ່ງນັ້ນ {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            ຖ້າ(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                ສົ່ງຄືນ qcResult;
            } ບໍ່ດັ່ງນັ້ນ ຖ້າ(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ສົ່ງຄືນ result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(const UnicodeString &s, UErrorCode &errorCode) const {
    uprv_checkCanGetBuffer(s, errorCode);
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    ສຳລັບ(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ຖ້າ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } ບໍ່ດັ່ງນັ້ນ {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            ຖ້າ(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                ສົ່ງຄືນ yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ສົ່ງຄືນ s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) const {
    ສົ່ງຄືນ !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) const {
    ສົ່ງຄືນ !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) const {
    ສົ່ງຄືນ !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(const UNormalizer2* norm2 U_LIFETIME_BOUND,
                    const USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    ຖ້າ(U_FAILURE(*pErrorCode)) {
        ສົ່ງຄືນ nullptr;
    }
    ຖ້າ(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ nullptr;
    }
    Normalizer2 *fn2=new FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    ຖ້າ(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ສົ່ງຄືນ (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
