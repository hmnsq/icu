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

#अन्तर्भाव "unicode/utypes.h"

#सचे !UCONFIG_NO_NORMALIZATION

#अन्तर्भाव "unicode/edits.h"
#अन्तर्भाव "unicode/normalizer2.h"
#अन्तर्भाव "unicode/stringoptions.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/unorm.h"
#अन्तर्भाव "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(ध्रुव UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UErrorCode& errorCode) ध्रुव {
    uprv_checkCanGetBuffer(src, errorCode);
    सचे(U_FAILURE(errorCode)) {
        dest.setToBogus();
        पटिनिवत्त dest;
    }
    सचे(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त dest;
    }
    dest.remove();
    पटिनिवत्त normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(ध्रुव UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USetSpanCondition spanCondition,
                               UErrorCode& errorCode) ध्रुव {
    UnicodeString tempDest;  // Don't throw away destination buffer between iterations.
    अत्थाय(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        सचे(spanCondition==USET_SPAN_NOT_CONTAINED) {
            सचे(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } अञ्ञथा {
            सचे(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                सचे(U_FAILURE(errorCode)) {
                    भङ्ग;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    पटिनिवत्त dest;
}

शून्य
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) ध्रुव {
    सचे (U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    सचे (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.data(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

शून्य
FilteredNormalizer2::normalizeUTF8(uint32_t options, ध्रुव वर्ण *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) ध्रुव {
    याव (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        सचे (spanCondition == USET_SPAN_NOT_CONTAINED) {
            सचे (spanLength != 0) {
                सचे (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                सचे ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } अञ्ञथा {
            सचे (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                सचे (U_FAILURE(errorCode)) {
                    भङ्ग;
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
                                              ध्रुव UnicodeString& second,
                                              UErrorCode& errorCode) ध्रुव {
    पटिनिवत्त normalizeSecondAndAppend(first, second, सच्च, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ध्रुव UnicodeString& second,
                            UErrorCode& errorCode) ध्रुव {
    पटिनिवत्त normalizeSecondAndAppend(first, second, मिच्छा, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              ध्रुव UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) ध्रुव {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त first;
    }
    सचे(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त first;
    }
    सचे(first.isEmpty()) {
        सचे(doNormalize) {
            पटिनिवत्त normalize(second, first, errorCode);
        } अञ्ञथा {
            पटिनिवत्त first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    सचे(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        सचे(suffixStart==0) {
            सचे(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } अञ्ञथा {
                norm2.append(first, prefix, errorCode);
            }
        } अञ्ञथा {
            UnicodeString मध्यः(first, suffixStart, INT32_MAX);
            सचे(doNormalize) {
                norm2.normalizeSecondAndAppend(मध्यः, prefix, errorCode);
            } अञ्ञथा {
                norm2.append(मध्यः, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, मध्यः);
        }
    }
    सचे(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        सचे(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } अञ्ञथा {
            first.append(rest);
        }
    }
    पटिनिवत्त first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ध्रुव {
    पटिनिवत्त set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ध्रुव {
    पटिनिवत्त set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ध्रुव {
    पटिनिवत्त (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ध्रुव {
    पटिनिवत्त set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ध्रुव UnicodeString &s, UErrorCode &errorCode) ध्रुव {
    uprv_checkCanGetBuffer(s, errorCode);
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त मिच्छा;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    अत्थाय(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        सचे(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } अञ्ञथा {
            सचे( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                पटिनिवत्त मिच्छा;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    पटिनिवत्त सच्च;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ध्रुव {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त मिच्छा;
    }
    ध्रुव वर्ण *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    याव (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        सचे (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } अञ्ञथा {
            सचे (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                पटिनिवत्त मिच्छा;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    पटिनिवत्त सच्च;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(ध्रुव UnicodeString &s, UErrorCode &errorCode) ध्रुव {
    uprv_checkCanGetBuffer(s, errorCode);
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    अत्थाय(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        सचे(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } अञ्ञथा {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            सचे(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                पटिनिवत्त qcResult;
            } अञ्ञथा सचे(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    पटिनिवत्त result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(ध्रुव UnicodeString &s, UErrorCode &errorCode) ध्रुव {
    uprv_checkCanGetBuffer(s, errorCode);
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    अत्थाय(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        सचे(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } अञ्ञथा {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            सचे(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                पटिनिवत्त yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    पटिनिवत्त s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ध्रुव {
    पटिनिवत्त !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ध्रुव {
    पटिनिवत्त !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ध्रुव {
    पटिनिवत्त !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ध्रुव UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ध्रुव USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    सचे(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    Normalizer2 *fn2=नूतन FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    सचे(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    पटिनिवत्त (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
