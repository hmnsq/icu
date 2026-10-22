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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_NORMALIZATION

#அந்தர்பாவ "unicode/edits.h"
#அந்தர்பாவ "unicode/normalizer2.h"
#அந்தர்பாவ "unicode/stringoptions.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/unorm.h"
#அந்தர்பாவ "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(மாறிலி UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UErrorCode& errorCode) மாறிலி {
    uprv_checkCanGetBuffer(src, errorCode);
    எனில்(U_FAILURE(errorCode)) {
        dest.setToBogus();
        திரும்பு dest;
    }
    எனில்(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு dest;
    }
    dest.remove();
    திரும்பு normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(மாறிலி UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USetSpanCondition spanCondition,
                               UErrorCode& errorCode) மாறிலி {
    UnicodeString tempDest;  // Don't throw away destination buffer between iterations.
    க்கு(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        எனில்(spanCondition==USET_SPAN_NOT_CONTAINED) {
            எனில்(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } இல்லையெனில் {
            எனில்(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                எனில்(U_FAILURE(errorCode)) {
                    நிறுத்து;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    திரும்பு dest;
}

வெறுமை
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு;
    }
    எனில் (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.data(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

வெறுமை
FilteredNormalizer2::normalizeUTF8(uint32_t options, மாறிலி எழுத்து *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) மாறிலி {
    போது (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        எனில் (spanCondition == USET_SPAN_NOT_CONTAINED) {
            எனில் (spanLength != 0) {
                எனில் (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                எனில் ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } இல்லையெனில் {
            எனில் (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                எனில் (U_FAILURE(errorCode)) {
                    நிறுத்து;
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
                                              மாறிலி UnicodeString& second,
                                              UErrorCode& errorCode) மாறிலி {
    திரும்பு normalizeSecondAndAppend(first, second, உண்மை, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            மாறிலி UnicodeString& second,
                            UErrorCode& errorCode) மாறிலி {
    திரும்பு normalizeSecondAndAppend(first, second, பொய், errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              மாறிலி UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) மாறிலி {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு first;
    }
    எனில்(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு first;
    }
    எனில்(first.isEmpty()) {
        எனில்(doNormalize) {
            திரும்பு normalize(second, first, errorCode);
        } இல்லையெனில் {
            திரும்பு first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    எனில்(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        எனில்(suffixStart==0) {
            எனில்(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } இல்லையெனில் {
                norm2.append(first, prefix, errorCode);
            }
        } இல்லையெனில் {
            UnicodeString நடு(first, suffixStart, INT32_MAX);
            எனில்(doNormalize) {
                norm2.normalizeSecondAndAppend(நடு, prefix, errorCode);
            } இல்லையெனில் {
                norm2.append(நடு, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, நடு);
        }
    }
    எனில்(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        எனில்(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } இல்லையெனில் {
            first.append(rest);
        }
    }
    திரும்பு first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) மாறிலி {
    திரும்பு set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) மாறிலி {
    திரும்பு set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) மாறிலி {
    திரும்பு (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) மாறிலி {
    திரும்பு set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(மாறிலி UnicodeString &s, UErrorCode &errorCode) மாறிலி {
    uprv_checkCanGetBuffer(s, errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு பொய்;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    க்கு(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        எனில்(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } இல்லையெனில் {
            எனில்( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                திரும்பு பொய்;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    திரும்பு உண்மை;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு பொய்;
    }
    மாறிலி எழுத்து *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    போது (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        எனில் (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } இல்லையெனில் {
            எனில் (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                திரும்பு பொய்;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    திரும்பு உண்மை;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(மாறிலி UnicodeString &s, UErrorCode &errorCode) மாறிலி {
    uprv_checkCanGetBuffer(s, errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    க்கு(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        எனில்(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } இல்லையெனில் {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            எனில்(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                திரும்பு qcResult;
            } இல்லையெனில் எனில்(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    திரும்பு result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(மாறிலி UnicodeString &s, UErrorCode &errorCode) மாறிலி {
    uprv_checkCanGetBuffer(s, errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    க்கு(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        எனில்(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } இல்லையெனில் {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            எனில்(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                திரும்பு yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    திரும்பு s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) மாறிலி {
    திரும்பு !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) மாறிலி {
    திரும்பு !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) மாறிலி {
    திரும்பு !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(மாறிலி UNormalizer2* norm2 U_LIFETIME_BOUND,
                    மாறிலி USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    எனில்(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    Normalizer2 *fn2=புதிய FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    எனில்(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    திரும்பு (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
