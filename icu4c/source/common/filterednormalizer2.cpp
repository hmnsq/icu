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

#𐤀𐤌 !UCONFIG_NO_NORMALIZATION

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
    𐤀𐤌(U_FAILURE(errorCode)) {
        dest.setToBogus();
        أرجع dest;
    }
    𐤀𐤌(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع dest;
    }
    dest.remove();
    أرجع normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
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
        𐤀𐤌(spanCondition==USET_SPAN_NOT_CONTAINED) {
            𐤀𐤌(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } وإلا {
            𐤀𐤌(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                𐤀𐤌(U_FAILURE(errorCode)) {
                    اكسر;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    أرجع dest;
}

فراغ
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) ثابت {
    𐤀𐤌 (U_FAILURE(errorCode)) {
        أرجع;
    }
    𐤀𐤌 (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
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
    طالما (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        𐤀𐤌 (spanCondition == USET_SPAN_NOT_CONTAINED) {
            𐤀𐤌 (spanLength != 0) {
                𐤀𐤌 (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                𐤀𐤌 ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } وإلا {
            𐤀𐤌 (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                𐤀𐤌 (U_FAILURE(errorCode)) {
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
    أرجع normalizeSecondAndAppend(first, second, true, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ثابت UnicodeString& second,
                            UErrorCode& errorCode) ثابت {
    أرجع normalizeSecondAndAppend(first, second, خطأ, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              ثابت UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) ثابت {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع first;
    }
    𐤀𐤌(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع first;
    }
    𐤀𐤌(first.isEmpty()) {
        𐤀𐤌(doNormalize) {
            أرجع normalize(second, first, errorCode);
        } وإلا {
            أرجع first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    𐤀𐤌(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        𐤀𐤌(suffixStart==0) {
            𐤀𐤌(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } وإلا {
                norm2.append(first, prefix, errorCode);
            }
        } وإلا {
            UnicodeString middle(first, suffixStart, INT32_MAX);
            𐤀𐤌(doNormalize) {
                norm2.normalizeSecondAndAppend(middle, prefix, errorCode);
            } وإلا {
                norm2.append(middle, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, middle);
        }
    }
    𐤀𐤌(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        𐤀𐤌(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } وإلا {
            first.append(rest);
        }
    }
    أرجع first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    أرجع set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    أرجع set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ثابت {
    أرجع (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ثابت {
    أرجع set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت {
    uprv_checkCanGetBuffer(s, errorCode);
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع خطأ;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    لكل(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        𐤀𐤌(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } وإلا {
            𐤀𐤌( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                أرجع خطأ;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    أرجع true;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ثابت {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع خطأ;
    }
    ثابت حرف *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    طالما (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        𐤀𐤌 (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } وإلا {
            𐤀𐤌 (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                أرجع خطأ;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    أرجع true;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت {
    uprv_checkCanGetBuffer(s, errorCode);
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    لكل(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        𐤀𐤌(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } وإلا {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            𐤀𐤌(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                أرجع qcResult;
            } وإلا 𐤀𐤌(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    أرجع result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت {
    uprv_checkCanGetBuffer(s, errorCode);
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    لكل(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        𐤀𐤌(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } وإلا {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            𐤀𐤌(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                أرجع yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    أرجع s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ثابت {
    أرجع !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ثابت {
    أرجع !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ثابت {
    أرجع !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ثابت UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ثابت USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    𐤀𐤌(U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }
    𐤀𐤌(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    Normalizer2 *fn2=جديد FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    𐤀𐤌(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    أرجع (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
