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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_NORMALIZATION

#شامل "unicode/edits.h"
#شامل "unicode/normalizer2.h"
#شامل "unicode/stringoptions.h"
#شامل "unicode/uniset.h"
#شامل "unicode/unistr.h"
#شامل "unicode/unorm.h"
#شامل "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(ثابت UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UErrorCode& errorCode) ثابت {
    uprv_checkCanGetBuffer(src, errorCode);
    اگر(U_FAILURE(errorCode)) {
        dest.setToBogus();
        بازگشت dest;
    }
    اگر(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت dest;
    }
    dest.remove();
    بازگشت normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
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
    برای(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        اگر(spanCondition==USET_SPAN_NOT_CONTAINED) {
            اگر(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } وگرنه {
            اگر(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                اگر(U_FAILURE(errorCode)) {
                    بشکن;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    بازگشت dest;
}

تهی
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) {
        بازگشت;
    }
    اگر (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.data(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

تهی
FilteredNormalizer2::normalizeUTF8(uint32_t options, ثابت نویسه *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) ثابت {
    وقتی (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        اگر (spanCondition == USET_SPAN_NOT_CONTAINED) {
            اگر (spanLength != 0) {
                اگر (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                اگر ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } وگرنه {
            اگر (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                اگر (U_FAILURE(errorCode)) {
                    بشکن;
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
    بازگشت normalizeSecondAndAppend(first, second, درست, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ثابت UnicodeString& second,
                            UErrorCode& errorCode) ثابت {
    بازگشت normalizeSecondAndAppend(first, second, نادرست, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              ثابت UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) ثابت {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    اگر(U_FAILURE(errorCode)) {
        بازگشت first;
    }
    اگر(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت first;
    }
    اگر(first.isEmpty()) {
        اگر(doNormalize) {
            بازگشت normalize(second, first, errorCode);
        } وگرنه {
            بازگشت first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    اگر(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        اگر(suffixStart==0) {
            اگر(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } وگرنه {
                norm2.append(first, prefix, errorCode);
            }
        } وگرنه {
            UnicodeString میانه(first, suffixStart, INT32_MAX);
            اگر(doNormalize) {
                norm2.normalizeSecondAndAppend(میانه, prefix, errorCode);
            } وگرنه {
                norm2.append(میانه, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, میانه);
        }
    }
    اگر(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        اگر(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } وگرنه {
            first.append(rest);
        }
    }
    بازگشت first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    بازگشت set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    بازگشت set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ثابت {
    بازگشت (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ثابت {
    بازگشت set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت {
    uprv_checkCanGetBuffer(s, errorCode);
    اگر(U_FAILURE(errorCode)) {
        بازگشت نادرست;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    برای(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        اگر(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } وگرنه {
            اگر( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                بازگشت نادرست;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    بازگشت درست;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        بازگشت نادرست;
    }
    ثابت نویسه *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    وقتی (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        اگر (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } وگرنه {
            اگر (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                بازگشت نادرست;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    بازگشت درست;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت {
    uprv_checkCanGetBuffer(s, errorCode);
    اگر(U_FAILURE(errorCode)) {
        بازگشت UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    برای(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        اگر(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } وگرنه {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            اگر(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                بازگشت qcResult;
            } وگرنه اگر(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    بازگشت result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت {
    uprv_checkCanGetBuffer(s, errorCode);
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    برای(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        اگر(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } وگرنه {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            اگر(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                بازگشت yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    بازگشت s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ثابت {
    بازگشت !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ثابت {
    بازگشت !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ثابت {
    بازگشت !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ثابت UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ثابت USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت nullptr;
    }
    اگر(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    Normalizer2 *fn2=جدید FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    اگر(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    بازگشت (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
