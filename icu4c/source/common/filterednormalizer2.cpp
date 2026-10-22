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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_NORMALIZATION

#समावेश "unicode/edits.h"
#समावेश "unicode/normalizer2.h"
#समावेश "unicode/stringoptions.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/unorm.h"
#समावेश "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(स्थिर UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UErrorCode& errorCode) स्थिर {
    uprv_check_Can_Get_बफर(src, errorCode);
    अगर(U_FAILURE(errorCode)) {
        dest.setToBogus();
        वापस dest;
    }
    अगर(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस dest;
    }
    dest.हटाओ();
    वापस normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(स्थिर UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USetSpanCondition spanCondition,
                               UErrorCode& errorCode) स्थिर {
    UnicodeString अस्थायी_Dest;  // Don't throw away destination buffer between iterations.
    क्रम(int32_t पूर्व_Span_Limit=0; पूर्व_Span_Limit<src.length();) {
        int32_t spanLimit=set.span(src, पूर्व_Span_Limit, spanCondition);
        int32_t span_लंबाई=spanLimit-पूर्व_Span_Limit;
        अगर(spanCondition==USET_SPAN_NOT_CONTAINED) {
            अगर(span_लंबाई!=0) {
                dest.append(src, पूर्व_Span_Limit, span_लंबाई);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } अन्यथा {
            अगर(span_लंबाई!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.अस्थायी_Sub_String_Between(पूर्व_Span_Limit, spanLimit),
                                            अस्थायी_Dest, errorCode));
                अगर(U_FAILURE(errorCode)) {
                    अवरोध;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        पूर्व_Span_Limit=spanLimit;
    }
    वापस dest;
}

व्योम
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) {
        वापस;
    }
    अगर (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.तथ्य(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

व्योम
FilteredNormalizer2::normalizeUTF8(uint32_t options, स्थिर अक्षर *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) स्थिर {
    जबतक (length > 0) {
        int32_t span_लंबाई = set.spanUTF8(src, length, spanCondition);
        अगर (spanCondition == USET_SPAN_NOT_CONTAINED) {
            अगर (span_लंबाई != 0) {
                अगर (edits != nullptr) {
                    edits->addUnchanged(span_लंबाई);
                }
                अगर ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, span_लंबाई);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } अन्यथा {
            अगर (span_लंबाई != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, span_लंबाई), sink, edits, errorCode);
                अगर (U_FAILURE(errorCode)) {
                    अवरोध;
                }
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        src += span_लंबाई;
        length -= span_लंबाई;
    }
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              स्थिर UnicodeString& second,
                                              UErrorCode& errorCode) स्थिर {
    वापस normalizeSecondAndAppend(first, second, true, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            स्थिर UnicodeString& second,
                            UErrorCode& errorCode) स्थिर {
    वापस normalizeSecondAndAppend(first, second, false, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              स्थिर UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) स्थिर {
    uprv_check_Can_Get_बफर(first, errorCode);
    uprv_check_Can_Get_बफर(second, errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस first;
    }
    अगर(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस first;
    }
    अगर(first.isEmpty()) {
        अगर(doNormalize) {
            वापस normalize(second, first, errorCode);
        } अन्यथा {
            वापस first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    अगर(prefixLimit!=0) {
        UnicodeString prefix(second.अस्थायी_Sub_String(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        अगर(suffixStart==0) {
            अगर(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } अन्यथा {
                norm2.append(first, prefix, errorCode);
            }
        } अन्यथा {
            UnicodeString मध्य(first, suffixStart, INT32_MAX);
            अगर(doNormalize) {
                norm2.normalizeSecondAndAppend(मध्य, prefix, errorCode);
            } अन्यथा {
                norm2.append(मध्य, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, मध्य);
        }
    }
    अगर(prefixLimit<second.length()) {
        UnicodeString rest(second.अस्थायी_Sub_String(prefixLimit, INT32_MAX));
        अगर(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } अन्यथा {
            first.append(rest);
        }
    }
    वापस first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) स्थिर {
    वापस set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) स्थिर {
    वापस set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) स्थिर {
    वापस (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) स्थिर {
    वापस set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर {
    uprv_check_Can_Get_बफर(s, errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस false;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    क्रम(int32_t पूर्व_Span_Limit=0; पूर्व_Span_Limit<s.length();) {
        int32_t spanLimit=set.span(s, पूर्व_Span_Limit, spanCondition);
        अगर(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } अन्यथा {
            अगर( !norm2.isNormalized(s.अस्थायी_Sub_String_Between(पूर्व_Span_Limit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                वापस false;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        पूर्व_Span_Limit=spanLimit;
    }
    वापस true;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        वापस false;
    }
    स्थिर अक्षर *s = sp.तथ्य();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    जबतक (length > 0) {
        int32_t span_लंबाई = set.spanUTF8(s, length, spanCondition);
        अगर (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } अन्यथा {
            अगर (!norm2.isNormalizedUTF8(StringPiece(s, span_लंबाई), errorCode) ||
                    U_FAILURE(errorCode)) {
                वापस false;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += span_लंबाई;
        length -= span_लंबाई;
    }
    वापस true;
}

UNormalization_Check_परिणाम
FilteredNormalizer2::quickCheck(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर {
    uprv_check_Can_Get_बफर(s, errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस UNORM_MAYBE;
    }
    UNormalization_Check_परिणाम परिणाम=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    क्रम(int32_t पूर्व_Span_Limit=0; पूर्व_Span_Limit<s.length();) {
        int32_t spanLimit=set.span(s, पूर्व_Span_Limit, spanCondition);
        अगर(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } अन्यथा {
            UNormalization_Check_परिणाम qc_परिणाम=
                norm2.quickCheck(s.अस्थायी_Sub_String_Between(पूर्व_Span_Limit, spanLimit), errorCode);
            अगर(U_FAILURE(errorCode) || qc_परिणाम==UNORM_NO) {
                वापस qc_परिणाम;
            } अन्यथा अगर(qc_परिणाम==UNORM_MAYBE) {
                परिणाम=qc_परिणाम;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        पूर्व_Span_Limit=spanLimit;
    }
    वापस परिणाम;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर {
    uprv_check_Can_Get_बफर(s, errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    क्रम(int32_t पूर्व_Span_Limit=0; पूर्व_Span_Limit<s.length();) {
        int32_t spanLimit=set.span(s, पूर्व_Span_Limit, spanCondition);
        अगर(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } अन्यथा {
            int32_t yesLimit=
                पूर्व_Span_Limit+
                norm2.spanQuickCheckYes(
                    s.अस्थायी_Sub_String_Between(पूर्व_Span_Limit, spanLimit), errorCode);
            अगर(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                वापस yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        पूर्व_Span_Limit=spanLimit;
    }
    वापस s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) स्थिर {
    वापस !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) स्थिर {
    वापस !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) स्थिर {
    वापस !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(स्थिर UNormalizer2* norm2 U_LIFETIME_BOUND,
                    स्थिर USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    अगर(U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    अगर(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    Normalizer2 *fn2=नया FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    अगर(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    वापस (UNormalizer2 *)fn2;
}

#पूर्ण_अगर  // !UCONFIG_NO_NORMALIZATION
