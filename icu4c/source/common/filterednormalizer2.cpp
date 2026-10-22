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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_NORMALIZATION

#അന്തര്ഭാവ "unicode/edits.h"
#അന്തര്ഭാവ "unicode/normalizer2.h"
#അന്തര്ഭാവ "unicode/stringoptions.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/unorm.h"
#അന്തര്ഭാവ "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(സ്ഥിരം UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UErrorCode& errorCode) സ്ഥിരം {
    uprv_checkCanGetBuffer(src, errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        dest.setToBogus();
        തിരികെ dest;
    }
    എങ്കിൽ(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ dest;
    }
    dest.remove();
    തിരികെ normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(സ്ഥിരം UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USetSpanCondition spanCondition,
                               UErrorCode& errorCode) സ്ഥിരം {
    UnicodeString tempDest;  // Don't throw away destination buffer between iterations.
    വേണ്ടി(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        എങ്കിൽ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            എങ്കിൽ(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } അല്ലെങ്കിൽ {
            എങ്കിൽ(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                എങ്കിൽ(U_FAILURE(errorCode)) {
                    നിർത്തുക;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    തിരികെ dest;
}

ശൂന്യം
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ;
    }
    എങ്കിൽ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.data(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

ശൂന്യം
FilteredNormalizer2::normalizeUTF8(uint32_t options, സ്ഥിരം അക്ഷരം *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) സ്ഥിരം {
    ആകുമ്പോൾ (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        എങ്കിൽ (spanCondition == USET_SPAN_NOT_CONTAINED) {
            എങ്കിൽ (spanLength != 0) {
                എങ്കിൽ (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                എങ്കിൽ ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } അല്ലെങ്കിൽ {
            എങ്കിൽ (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                എങ്കിൽ (U_FAILURE(errorCode)) {
                    നിർത്തുക;
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
                                              സ്ഥിരം UnicodeString& second,
                                              UErrorCode& errorCode) സ്ഥിരം {
    തിരികെ normalizeSecondAndAppend(first, second, സത്യം, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            സ്ഥിരം UnicodeString& second,
                            UErrorCode& errorCode) സ്ഥിരം {
    തിരികെ normalizeSecondAndAppend(first, second, അസത്യം, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              സ്ഥിരം UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) സ്ഥിരം {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ first;
    }
    എങ്കിൽ(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ first;
    }
    എങ്കിൽ(first.isEmpty()) {
        എങ്കിൽ(doNormalize) {
            തിരികെ normalize(second, first, errorCode);
        } അല്ലെങ്കിൽ {
            തിരികെ first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    എങ്കിൽ(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        എങ്കിൽ(suffixStart==0) {
            എങ്കിൽ(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } അല്ലെങ്കിൽ {
                norm2.append(first, prefix, errorCode);
            }
        } അല്ലെങ്കിൽ {
            UnicodeString നടുവ്(first, suffixStart, INT32_MAX);
            എങ്കിൽ(doNormalize) {
                norm2.normalizeSecondAndAppend(നടുവ്, prefix, errorCode);
            } അല്ലെങ്കിൽ {
                norm2.append(നടുവ്, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, നടുവ്);
        }
    }
    എങ്കിൽ(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        എങ്കിൽ(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } അല്ലെങ്കിൽ {
            first.append(rest);
        }
    }
    തിരികെ first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) സ്ഥിരം {
    തിരികെ set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) സ്ഥിരം {
    തിരികെ set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) സ്ഥിരം {
    തിരികെ (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) സ്ഥിരം {
    തിരികെ set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(സ്ഥിരം UnicodeString &s, UErrorCode &errorCode) സ്ഥിരം {
    uprv_checkCanGetBuffer(s, errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ അസത്യം;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    വേണ്ടി(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        എങ്കിൽ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } അല്ലെങ്കിൽ {
            എങ്കിൽ( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                തിരികെ അസത്യം;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    തിരികെ സത്യം;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം അക്ഷരം *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    ആകുമ്പോൾ (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        എങ്കിൽ (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } അല്ലെങ്കിൽ {
            എങ്കിൽ (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                തിരികെ അസത്യം;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    തിരികെ സത്യം;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(സ്ഥിരം UnicodeString &s, UErrorCode &errorCode) സ്ഥിരം {
    uprv_checkCanGetBuffer(s, errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    വേണ്ടി(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        എങ്കിൽ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } അല്ലെങ്കിൽ {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            എങ്കിൽ(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                തിരികെ qcResult;
            } അല്ലെങ്കിൽ എങ്കിൽ(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    തിരികെ result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(സ്ഥിരം UnicodeString &s, UErrorCode &errorCode) സ്ഥിരം {
    uprv_checkCanGetBuffer(s, errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    വേണ്ടി(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        എങ്കിൽ(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } അല്ലെങ്കിൽ {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            എങ്കിൽ(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                തിരികെ yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    തിരികെ s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) സ്ഥിരം {
    തിരികെ !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) സ്ഥിരം {
    തിരികെ !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) സ്ഥിരം {
    തിരികെ !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(സ്ഥിരം UNormalizer2* norm2 U_LIFETIME_BOUND,
                    സ്ഥിരം USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    Normalizer2 *fn2=പുതിയ FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    എങ്കിൽ(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    തിരികെ (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
