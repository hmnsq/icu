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

#यदि !UCONFIG_NO_NORMALIZATION

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
    यदि(U_FAILURE(errorCode)) {
        dest.setToBogus();
        प्रत्यागम dest;
    }
    यदि(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम dest;
    }
    dest.remove();
    प्रत्यागम normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
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
    प्रति(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        यदि(spanCondition==USET_SPAN_NOT_CONTAINED) {
            यदि(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } अन्यथा {
            यदि(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                यदि(U_FAILURE(errorCode)) {
                    भङ्ग;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    प्रत्यागम dest;
}

शून्य
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) ध्रुव {
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम;
    }
    यदि (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
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
    यावत् (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        यदि (spanCondition == USET_SPAN_NOT_CONTAINED) {
            यदि (spanLength != 0) {
                यदि (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                यदि ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } अन्यथा {
            यदि (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                यदि (U_FAILURE(errorCode)) {
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
    प्रत्यागम normalizeSecondAndAppend(first, second, सत्य, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ध्रुव UnicodeString& second,
                            UErrorCode& errorCode) ध्रुव {
    प्रत्यागम normalizeSecondAndAppend(first, second, असत्य, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              ध्रुव UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) ध्रुव {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम first;
    }
    यदि(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम first;
    }
    यदि(first.isEmpty()) {
        यदि(doNormalize) {
            प्रत्यागम normalize(second, first, errorCode);
        } अन्यथा {
            प्रत्यागम first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    यदि(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        यदि(suffixStart==0) {
            यदि(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } अन्यथा {
                norm2.append(first, prefix, errorCode);
            }
        } अन्यथा {
            UnicodeString मध्य(first, suffixStart, INT32_MAX);
            यदि(doNormalize) {
                norm2.normalizeSecondAndAppend(मध्य, prefix, errorCode);
            } अन्यथा {
                norm2.append(मध्य, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, मध्य);
        }
    }
    यदि(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        यदि(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } अन्यथा {
            first.append(rest);
        }
    }
    प्रत्यागम first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ध्रुव {
    प्रत्यागम set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ध्रुव {
    प्रत्यागम set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ध्रुव {
    प्रत्यागम (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ध्रुव {
    प्रत्यागम set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ध्रुव UnicodeString &s, UErrorCode &errorCode) ध्रुव {
    uprv_checkCanGetBuffer(s, errorCode);
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम असत्य;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    प्रति(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        यदि(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } अन्यथा {
            यदि( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                प्रत्यागम असत्य;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    प्रत्यागम सत्य;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ध्रुव {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम असत्य;
    }
    ध्रुव वर्ण *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    यावत् (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        यदि (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } अन्यथा {
            यदि (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                प्रत्यागम असत्य;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    प्रत्यागम सत्य;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(ध्रुव UnicodeString &s, UErrorCode &errorCode) ध्रुव {
    uprv_checkCanGetBuffer(s, errorCode);
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    प्रति(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        यदि(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } अन्यथा {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            यदि(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                प्रत्यागम qcResult;
            } अन्यथा यदि(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    प्रत्यागम result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(ध्रुव UnicodeString &s, UErrorCode &errorCode) ध्रुव {
    uprv_checkCanGetBuffer(s, errorCode);
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    प्रति(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        यदि(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } अन्यथा {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            यदि(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                प्रत्यागम yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    प्रत्यागम s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ध्रुव {
    प्रत्यागम !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ध्रुव {
    प्रत्यागम !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ध्रुव {
    प्रत्यागम !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ध्रुव UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ध्रुव USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    यदि(U_FAILURE(*pErrorCode)) {
        प्रत्यागम nullptr;
    }
    यदि(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    Normalizer2 *fn2=नूतन FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    यदि(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    प्रत्यागम (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
