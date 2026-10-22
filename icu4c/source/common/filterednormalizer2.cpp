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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_NORMALIZATION

#অন্তর্ভাব "unicode/edits.h"
#অন্তর্ভাব "unicode/normalizer2.h"
#অন্তর্ভাব "unicode/stringoptions.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/unorm.h"
#অন্তর্ভাব "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(ধ্রুবক UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UErrorCode& errorCode) ধ্রুবক {
    uprv_check_Can_Get_বাফার(src, errorCode);
    যদি(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ফেরত dest;
    }
    যদি(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত dest;
    }
    dest.remove();
    ফেরত normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(ধ্রুবক UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USetSpanCondition spanCondition,
                               UErrorCode& errorCode) ধ্রুবক {
    UnicodeString অস্থায়ী_Dest;  // Don't throw away destination buffer between iterations.
    জন্য(int32_t পূর্ব_Span_Limit=0; পূর্ব_Span_Limit<src.length();) {
        int32_t spanLimit=set.span(src, পূর্ব_Span_Limit, spanCondition);
        int32_t span_দৈর্ঘ্য=spanLimit-পূর্ব_Span_Limit;
        যদি(spanCondition==USET_SPAN_NOT_CONTAINED) {
            যদি(span_দৈর্ঘ্য!=0) {
                dest.append(src, পূর্ব_Span_Limit, span_দৈর্ঘ্য);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } অন্যথা {
            যদি(span_দৈর্ঘ্য!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.অস্থায়ী_Sub_String_Between(পূর্ব_Span_Limit, spanLimit),
                                            অস্থায়ী_Dest, errorCode));
                যদি(U_FAILURE(errorCode)) {
                    বিরতি;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        পূর্ব_Span_Limit=spanLimit;
    }
    ফেরত dest;
}

শূন্য
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) {
        ফেরত;
    }
    যদি (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.তথ্য(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

শূন্য
FilteredNormalizer2::normalizeUTF8(uint32_t options, ধ্রুবক অক্ষর *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) ধ্রুবক {
    যখন (length > 0) {
        int32_t span_দৈর্ঘ্য = set.spanUTF8(src, length, spanCondition);
        যদি (spanCondition == USET_SPAN_NOT_CONTAINED) {
            যদি (span_দৈর্ঘ্য != 0) {
                যদি (edits != nullptr) {
                    edits->addUnchanged(span_দৈর্ঘ্য);
                }
                যদি ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, span_দৈর্ঘ্য);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } অন্যথা {
            যদি (span_দৈর্ঘ্য != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, span_দৈর্ঘ্য), sink, edits, errorCode);
                যদি (U_FAILURE(errorCode)) {
                    বিরতি;
                }
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        src += span_দৈর্ঘ্য;
        length -= span_দৈর্ঘ্য;
    }
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              ধ্রুবক UnicodeString& second,
                                              UErrorCode& errorCode) ধ্রুবক {
    ফেরত normalizeSecondAndAppend(first, second, সত্য, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ধ্রুবক UnicodeString& second,
                            UErrorCode& errorCode) ধ্রুবক {
    ফেরত normalizeSecondAndAppend(first, second, মিথ্যা, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              ধ্রুবক UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) ধ্রুবক {
    uprv_check_Can_Get_বাফার(first, errorCode);
    uprv_check_Can_Get_বাফার(second, errorCode);
    যদি(U_FAILURE(errorCode)) {
        ফেরত first;
    }
    যদি(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত first;
    }
    যদি(first.isEmpty()) {
        যদি(doNormalize) {
            ফেরত normalize(second, first, errorCode);
        } অন্যথা {
            ফেরত first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    যদি(prefixLimit!=0) {
        UnicodeString prefix(second.অস্থায়ী_Sub_String(0, prefixLimit));
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
        UnicodeString rest(second.অস্থায়ী_Sub_String(prefixLimit, INT32_MAX));
        যদি(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } অন্যথা {
            first.append(rest);
        }
    }
    ফেরত first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ধ্রুবক {
    ফেরত set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ধ্রুবক {
    ফেরত set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ধ্রুবক {
    ফেরত (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ধ্রুবক {
    ফেরত set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ধ্রুবক UnicodeString &s, UErrorCode &errorCode) ধ্রুবক {
    uprv_check_Can_Get_বাফার(s, errorCode);
    যদি(U_FAILURE(errorCode)) {
        ফেরত মিথ্যা;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    জন্য(int32_t পূর্ব_Span_Limit=0; পূর্ব_Span_Limit<s.length();) {
        int32_t spanLimit=set.span(s, পূর্ব_Span_Limit, spanCondition);
        যদি(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } অন্যথা {
            যদি( !norm2.isNormalized(s.অস্থায়ী_Sub_String_Between(পূর্ব_Span_Limit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                ফেরত মিথ্যা;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        পূর্ব_Span_Limit=spanLimit;
    }
    ফেরত সত্য;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ধ্রুবক {
    যদি(U_FAILURE(errorCode)) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক অক্ষর *s = sp.তথ্য();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    যখন (length > 0) {
        int32_t span_দৈর্ঘ্য = set.spanUTF8(s, length, spanCondition);
        যদি (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } অন্যথা {
            যদি (!norm2.isNormalizedUTF8(StringPiece(s, span_দৈর্ঘ্য), errorCode) ||
                    U_FAILURE(errorCode)) {
                ফেরত মিথ্যা;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += span_দৈর্ঘ্য;
        length -= span_দৈর্ঘ্য;
    }
    ফেরত সত্য;
}

UNormalization_Check_ফলাফল
FilteredNormalizer2::quickCheck(ধ্রুবক UnicodeString &s, UErrorCode &errorCode) ধ্রুবক {
    uprv_check_Can_Get_বাফার(s, errorCode);
    যদি(U_FAILURE(errorCode)) {
        ফেরত UNORM_MAYBE;
    }
    UNormalization_Check_ফলাফল ফলাফল=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    জন্য(int32_t পূর্ব_Span_Limit=0; পূর্ব_Span_Limit<s.length();) {
        int32_t spanLimit=set.span(s, পূর্ব_Span_Limit, spanCondition);
        যদি(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } অন্যথা {
            UNormalization_Check_ফলাফল qc_ফলাফল=
                norm2.quickCheck(s.অস্থায়ী_Sub_String_Between(পূর্ব_Span_Limit, spanLimit), errorCode);
            যদি(U_FAILURE(errorCode) || qc_ফলাফল==UNORM_NO) {
                ফেরত qc_ফলাফল;
            } অন্যথা যদি(qc_ফলাফল==UNORM_MAYBE) {
                ফলাফল=qc_ফলাফল;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        পূর্ব_Span_Limit=spanLimit;
    }
    ফেরত ফলাফল;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(ধ্রুবক UnicodeString &s, UErrorCode &errorCode) ধ্রুবক {
    uprv_check_Can_Get_বাফার(s, errorCode);
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    জন্য(int32_t পূর্ব_Span_Limit=0; পূর্ব_Span_Limit<s.length();) {
        int32_t spanLimit=set.span(s, পূর্ব_Span_Limit, spanCondition);
        যদি(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } অন্যথা {
            int32_t yesLimit=
                পূর্ব_Span_Limit+
                norm2.spanQuickCheckYes(
                    s.অস্থায়ী_Sub_String_Between(পূর্ব_Span_Limit, spanLimit), errorCode);
            যদি(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                ফেরত yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        পূর্ব_Span_Limit=spanLimit;
    }
    ফেরত s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ধ্রুবক {
    ফেরত !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ধ্রুবক {
    ফেরত !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ধ্রুবক {
    ফেরত !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ধ্রুবক UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ধ্রুবক USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    Normalizer2 *fn2=নতুন FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    যদি(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ফেরত (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
