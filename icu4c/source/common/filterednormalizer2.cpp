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

#जर !UCONFIG_NO_NORMALIZATION

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
FilteredNormalizer2::normalize(स्थिरांक UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UErrorCode& errorCode) स्थिरांक {
    uprv_checkCanGetBuffer(src, errorCode);
    जर(U_FAILURE(errorCode)) {
        dest.setToBogus();
        परत dest;
    }
    जर(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत dest;
    }
    dest.हटाओ();
    परत normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(स्थिरांक UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USetSpanCondition spanCondition,
                               UErrorCode& errorCode) स्थिरांक {
    UnicodeString tempDest;  // Don't throw away destination buffer between iterations.
    साठी(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        जर(spanCondition==USET_SPAN_NOT_CONTAINED) {
            जर(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } अन्यथा {
            जर(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                जर(U_FAILURE(errorCode)) {
                    थांबा;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    परत dest;
}

शून्य
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) {
        परत;
    }
    जर (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.data(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

शून्य
FilteredNormalizer2::normalizeUTF8(uint32_t options, स्थिरांक अक्षर *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) स्थिरांक {
    पर्यंत (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        जर (spanCondition == USET_SPAN_NOT_CONTAINED) {
            जर (spanLength != 0) {
                जर (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                जर ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } अन्यथा {
            जर (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                जर (U_FAILURE(errorCode)) {
                    थांबा;
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
                                              स्थिरांक UnicodeString& second,
                                              UErrorCode& errorCode) स्थिरांक {
    परत normalizeSecondAndAppend(first, second, सत्य, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            स्थिरांक UnicodeString& second,
                            UErrorCode& errorCode) स्थिरांक {
    परत normalizeSecondAndAppend(first, second, असत्य, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              स्थिरांक UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) स्थिरांक {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    जर(U_FAILURE(errorCode)) {
        परत first;
    }
    जर(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत first;
    }
    जर(first.isEmpty()) {
        जर(doNormalize) {
            परत normalize(second, first, errorCode);
        } अन्यथा {
            परत first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    जर(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        जर(suffixStart==0) {
            जर(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } अन्यथा {
                norm2.append(first, prefix, errorCode);
            }
        } अन्यथा {
            UnicodeString मधला(first, suffixStart, INT32_MAX);
            जर(doNormalize) {
                norm2.normalizeSecondAndAppend(मधला, prefix, errorCode);
            } अन्यथा {
                norm2.append(मधला, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, मधला);
        }
    }
    जर(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        जर(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } अन्यथा {
            first.append(rest);
        }
    }
    परत first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) स्थिरांक {
    परत set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) स्थिरांक {
    परत set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) स्थिरांक {
    परत (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) स्थिरांक {
    परत set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(स्थिरांक UnicodeString &s, UErrorCode &errorCode) स्थिरांक {
    uprv_checkCanGetBuffer(s, errorCode);
    जर(U_FAILURE(errorCode)) {
        परत असत्य;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    साठी(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        जर(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } अन्यथा {
            जर( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                परत असत्य;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    परत सत्य;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) स्थिरांक {
    जर(U_FAILURE(errorCode)) {
        परत असत्य;
    }
    स्थिरांक अक्षर *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    पर्यंत (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        जर (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } अन्यथा {
            जर (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                परत असत्य;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    परत सत्य;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(स्थिरांक UnicodeString &s, UErrorCode &errorCode) स्थिरांक {
    uprv_checkCanGetBuffer(s, errorCode);
    जर(U_FAILURE(errorCode)) {
        परत UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    साठी(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        जर(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } अन्यथा {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            जर(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                परत qcResult;
            } अन्यथा जर(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    परत result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(स्थिरांक UnicodeString &s, UErrorCode &errorCode) स्थिरांक {
    uprv_checkCanGetBuffer(s, errorCode);
    जर(U_FAILURE(errorCode)) {
        परत 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    साठी(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        जर(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } अन्यथा {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            जर(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                परत yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    परत s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) स्थिरांक {
    परत !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) स्थिरांक {
    परत !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) स्थिरांक {
    परत !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(स्थिरांक UNormalizer2* norm2 U_LIFETIME_BOUND,
                    स्थिरांक USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    जर(U_FAILURE(*pErrorCode)) {
        परत nullptr;
    }
    जर(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    Normalizer2 *fn2=नवीन FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    जर(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    परत (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
