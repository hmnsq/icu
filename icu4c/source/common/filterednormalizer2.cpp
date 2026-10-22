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

#если !UCONFIG_NO_NORMALIZATION

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
    если(U_FAILURE(errorCode)) {
        dest.setToBogus();
        вернуть dest;
    }
    если(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        вернуть dest;
    }
    dest.remove();
    вернуть normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
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
    для(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        если(spanCondition==USET_SPAN_NOT_CONTAINED) {
            если(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } иначе {
            если(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                если(U_FAILURE(errorCode)) {
                    break;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    вернуть dest;
}

void
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) const {
    если (U_FAILURE(errorCode)) {
        вернуть;
    }
    если (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.data(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

void
FilteredNormalizer2::normalizeUTF8(uint32_t options, const char *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) const {
    пока (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        если (spanCondition == USET_SPAN_NOT_CONTAINED) {
            если (spanLength != 0) {
                если (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                если ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } иначе {
            если (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                если (U_FAILURE(errorCode)) {
                    break;
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
    вернуть normalizeSecondAndAppend(first, second, true, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            const UnicodeString& second,
                            UErrorCode& errorCode) const {
    вернуть normalizeSecondAndAppend(first, second, false, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              const UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) const {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    если(U_FAILURE(errorCode)) {
        вернуть first;
    }
    если(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        вернуть first;
    }
    если(first.isEmpty()) {
        если(doNormalize) {
            вернуть normalize(second, first, errorCode);
        } иначе {
            вернуть first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    если(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        если(suffixStart==0) {
            если(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } иначе {
                norm2.append(first, prefix, errorCode);
            }
        } иначе {
            UnicodeString middle(first, suffixStart, INT32_MAX);
            если(doNormalize) {
                norm2.normalizeSecondAndAppend(middle, prefix, errorCode);
            } иначе {
                norm2.append(middle, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, middle);
        }
    }
    если(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        если(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } иначе {
            first.append(rest);
        }
    }
    вернуть first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) const {
    вернуть set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) const {
    вернуть set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) const {
    вернуть (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) const {
    вернуть set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(const UnicodeString &s, UErrorCode &errorCode) const {
    uprv_checkCanGetBuffer(s, errorCode);
    если(U_FAILURE(errorCode)) {
        вернуть false;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    для(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        если(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } иначе {
            если( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                вернуть false;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    вернуть true;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) const {
    если(U_FAILURE(errorCode)) {
        вернуть false;
    }
    const char *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    пока (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        если (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } иначе {
            если (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                вернуть false;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    вернуть true;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(const UnicodeString &s, UErrorCode &errorCode) const {
    uprv_checkCanGetBuffer(s, errorCode);
    если(U_FAILURE(errorCode)) {
        вернуть UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    для(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        если(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } иначе {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            если(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                вернуть qcResult;
            } иначе если(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    вернуть result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(const UnicodeString &s, UErrorCode &errorCode) const {
    uprv_checkCanGetBuffer(s, errorCode);
    если(U_FAILURE(errorCode)) {
        вернуть 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    для(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        если(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } иначе {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            если(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                вернуть yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    вернуть s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) const {
    вернуть !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) const {
    вернуть !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) const {
    вернуть !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(const UNormalizer2* norm2 U_LIFETIME_BOUND,
                    const USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    если(U_FAILURE(*pErrorCode)) {
        вернуть nullptr;
    }
    если(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        вернуть nullptr;
    }
    Normalizer2 *fn2=new FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    если(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    вернуть (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
