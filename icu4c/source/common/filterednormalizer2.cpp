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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_NORMALIZATION

#অন্তর্ভাৱ "unicode/edits.h"
#অন্তর্ভাৱ "unicode/normalizer2.h"
#অন্তর্ভাৱ "unicode/stringoptions.h"
#অন্তর্ভাৱ "unicode/uniset.h"
#অন্তর্ভাৱ "unicode/unistr.h"
#অন্তর্ভাৱ "unicode/unorm.h"
#অন্তর্ভাৱ "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(ধ্রুৱ UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UErrorCode& errorCode) ধ্রুৱ {
    uprv_checkCanGetBuffer(src, errorCode);
    যদি(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ঘূৰাই dest;
    }
    যদি(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই dest;
    }
    dest.remove();
    ঘূৰাই normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(ধ্রুৱ UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USetSpanCondition spanCondition,
                               UErrorCode& errorCode) ধ্রুৱ {
    UnicodeString tempDest;  // Don't throw away destination buffer between iterations.
    বাবে(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        যদি(spanCondition==USET_SPAN_NOT_CONTAINED) {
            যদি(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } অন্যথা {
            যদি(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                যদি(U_FAILURE(errorCode)) {
                    ভঙ্গ;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ঘূৰাই dest;
}

শূন্য
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) ধ্রুৱ {
    যদি (U_FAILURE(errorCode)) {
        ঘূৰাই;
    }
    যদি (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.data(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

শূন্য
FilteredNormalizer2::normalizeUTF8(uint32_t options, ধ্রুৱ ৱর্ণ *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) ধ্রুৱ {
    যেতিয়ালৈ (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        যদি (spanCondition == USET_SPAN_NOT_CONTAINED) {
            যদি (spanLength != 0) {
                যদি (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                যদি ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } অন্যথা {
            যদি (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                যদি (U_FAILURE(errorCode)) {
                    ভঙ্গ;
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
                                              ধ্রুৱ UnicodeString& second,
                                              UErrorCode& errorCode) ধ্রুৱ {
    ঘূৰাই normalizeSecondAndAppend(first, second, সঁচা, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ধ্রুৱ UnicodeString& second,
                            UErrorCode& errorCode) ধ্রুৱ {
    ঘূৰাই normalizeSecondAndAppend(first, second, মিছা, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              ধ্রুৱ UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) ধ্রুৱ {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই first;
    }
    যদি(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই first;
    }
    যদি(first.isEmpty()) {
        যদি(doNormalize) {
            ঘূৰাই normalize(second, first, errorCode);
        } অন্যথা {
            ঘূৰাই first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    যদি(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        যদি(suffixStart==0) {
            যদি(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } অন্যথা {
                norm2.append(first, prefix, errorCode);
            }
        } অন্যথা {
            UnicodeString মধ্য(first, suffixStart, INT32_MAX);
            যদি(doNormalize) {
                norm2.normalizeSecondAndAppend(মধ্য, prefix, errorCode);
            } অন্যথা {
                norm2.append(মধ্য, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, মধ্য);
        }
    }
    যদি(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        যদি(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } অন্যথা {
            first.append(rest);
        }
    }
    ঘূৰাই first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ধ্রুৱ {
    ঘূৰাই set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ধ্রুৱ {
    ঘূৰাই set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ধ্রুৱ {
    ঘূৰাই (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ধ্রুৱ {
    ঘূৰাই set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ধ্রুৱ UnicodeString &s, UErrorCode &errorCode) ধ্রুৱ {
    uprv_checkCanGetBuffer(s, errorCode);
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই মিছা;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    বাবে(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        যদি(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } অন্যথা {
            যদি( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                ঘূৰাই মিছা;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ঘূৰাই সঁচা;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ধ্রুৱ {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই মিছা;
    }
    ধ্রুৱ ৱর্ণ *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    যেতিয়ালৈ (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        যদি (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } অন্যথা {
            যদি (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                ঘূৰাই মিছা;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    ঘূৰাই সঁচা;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(ধ্রুৱ UnicodeString &s, UErrorCode &errorCode) ধ্রুৱ {
    uprv_checkCanGetBuffer(s, errorCode);
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    বাবে(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        যদি(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } অন্যথা {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            যদি(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                ঘূৰাই qcResult;
            } অন্যথা যদি(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ঘূৰাই result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(ধ্রুৱ UnicodeString &s, UErrorCode &errorCode) ধ্রুৱ {
    uprv_checkCanGetBuffer(s, errorCode);
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    বাবে(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        যদি(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } অন্যথা {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            যদি(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                ঘূৰাই yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    ঘূৰাই s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ধ্রুৱ {
    ঘূৰাই !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ধ্রুৱ {
    ঘূৰাই !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ধ্রুৱ {
    ঘূৰাই !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ধ্রুৱ UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ধ্রুৱ USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        ঘূৰাই nullptr;
    }
    যদি(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }
    Normalizer2 *fn2=নূতন FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    যদি(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ঘূৰাই (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
