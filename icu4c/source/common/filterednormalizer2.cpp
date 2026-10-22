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

#ប្រសិនបើ !UCONFIG_NO_NORMALIZATION

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
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ត្រឡប់ dest;
    }
    ប្រសិនបើ(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ dest;
    }
    dest.remove();
    ត្រឡប់ normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
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
    សម្រាប់(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        ប្រសិនបើ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            ប្រសិនបើ(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } បើមិនដូច្នេះ {
            ប្រសិនបើ(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                ប្រសិនបើ(U_FAILURE(errorCode)) {
                    bha_nga;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ត្រឡប់ dest;
}

void
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) const {
    ប្រសិនបើ (U_FAILURE(errorCode)) {
        ត្រឡប់;
    }
    ប្រសិនបើ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
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
    ខណៈពេល (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        ប្រសិនបើ (spanCondition == USET_SPAN_NOT_CONTAINED) {
            ប្រសិនបើ (spanLength != 0) {
                ប្រសិនបើ (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                ប្រសិនបើ ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } បើមិនដូច្នេះ {
            ប្រសិនបើ (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                ប្រសិនបើ (U_FAILURE(errorCode)) {
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
    ត្រឡប់ normalizeSecondAndAppend(first, second, true, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            const UnicodeString& second,
                            UErrorCode& errorCode) const {
    ត្រឡប់ normalizeSecondAndAppend(first, second, false, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              const UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) const {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ first;
    }
    ប្រសិនបើ(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ first;
    }
    ប្រសិនបើ(first.isEmpty()) {
        ប្រសិនបើ(doNormalize) {
            ត្រឡប់ normalize(second, first, errorCode);
        } បើមិនដូច្នេះ {
            ត្រឡប់ first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    ប្រសិនបើ(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        ប្រសិនបើ(suffixStart==0) {
            ប្រសិនបើ(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } បើមិនដូច្នេះ {
                norm2.append(first, prefix, errorCode);
            }
        } បើមិនដូច្នេះ {
            UnicodeString middle(first, suffixStart, INT32_MAX);
            ប្រសិនបើ(doNormalize) {
                norm2.normalizeSecondAndAppend(middle, prefix, errorCode);
            } បើមិនដូច្នេះ {
                norm2.append(middle, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, middle);
        }
    }
    ប្រសិនបើ(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        ប្រសិនបើ(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } បើមិនដូច្នេះ {
            first.append(rest);
        }
    }
    ត្រឡប់ first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) const {
    ត្រឡប់ set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) const {
    ត្រឡប់ set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) const {
    ត្រឡប់ (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) const {
    ត្រឡប់ set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(const UnicodeString &s, UErrorCode &errorCode) const {
    uprv_checkCanGetBuffer(s, errorCode);
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ false;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    សម្រាប់(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ប្រសិនបើ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } បើមិនដូច្នេះ {
            ប្រសិនបើ( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                ត្រឡប់ false;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ត្រឡប់ true;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) const {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ false;
    }
    const var_na *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    ខណៈពេល (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        ប្រសិនបើ (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } បើមិនដូច្នេះ {
            ប្រសិនបើ (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                ត្រឡប់ false;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    ត្រឡប់ true;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(const UnicodeString &s, UErrorCode &errorCode) const {
    uprv_checkCanGetBuffer(s, errorCode);
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    សម្រាប់(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ប្រសិនបើ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } បើមិនដូច្នេះ {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            ប្រសិនបើ(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                ត្រឡប់ qcResult;
            } បើមិនដូច្នេះ ប្រសិនបើ(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ត្រឡប់ result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(const UnicodeString &s, UErrorCode &errorCode) const {
    uprv_checkCanGetBuffer(s, errorCode);
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    សម្រាប់(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        ប្រសិនបើ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } បើមិនដូច្នេះ {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            ប្រសិនបើ(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                ត្រឡប់ yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ត្រឡប់ s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) const {
    ត្រឡប់ !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) const {
    ត្រឡប់ !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) const {
    ត្រឡប់ !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(const UNormalizer2* norm2 U_LIFETIME_BOUND,
                    const USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    ប្រសិនបើ(U_FAILURE(*pErrorCode)) {
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ nullptr;
    }
    Normalizer2 *fn2=new FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    ប្រសិនបើ(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ត្រឡប់ (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
