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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_NORMALIZATION

#અન્તર્ભાવ "unicode/edits.h"
#અન્તર્ભાવ "unicode/normalizer2.h"
#અન્તર્ભાવ "unicode/stringoptions.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/unorm.h"
#અન્તર્ભાવ "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(અચલ UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UErrorCode& errorCode) અચલ {
    uprv_checkCanGetBuffer(src, errorCode);
    જો(U_FAILURE(errorCode)) {
        dest.setToBogus();
        પાછા dest;
    }
    જો(&dest==&src) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા dest;
    }
    dest.remove();
    પાછા normalize(src, dest, USET_SPAN_SIMPLE, errorCode);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(અચલ UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USetSpanCondition spanCondition,
                               UErrorCode& errorCode) અચલ {
    UnicodeString tempDest;  // Don't throw away destination buffer between iterations.
    માટે(int32_t prevSpanLimit=0; prevSpanLimit<src.length();) {
        int32_t spanLimit=set.span(src, prevSpanLimit, spanCondition);
        int32_t spanLength=spanLimit-prevSpanLimit;
        જો(spanCondition==USET_SPAN_NOT_CONTAINED) {
            જો(spanLength!=0) {
                dest.append(src, prevSpanLimit, spanLength);
            }
            spanCondition=USET_SPAN_SIMPLE;
        } નહીંતર {
            જો(spanLength!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.tempSubStringBetween(prevSpanLimit, spanLimit),
                                            tempDest, errorCode));
                જો(U_FAILURE(errorCode)) {
                    તોડો;
                }
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    પાછા dest;
}

શૂન્ય
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) {
        પાછા;
    }
    જો (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.data(), src.length(), sink, edits, USET_SPAN_SIMPLE, errorCode);
}

શૂન્ય
FilteredNormalizer2::normalizeUTF8(uint32_t options, અચલ અક્ષર *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USetSpanCondition spanCondition,
                                   UErrorCode &errorCode) અચલ {
    જ્યારે (length > 0) {
        int32_t spanLength = set.spanUTF8(src, length, spanCondition);
        જો (spanCondition == USET_SPAN_NOT_CONTAINED) {
            જો (spanLength != 0) {
                જો (edits != nullptr) {
                    edits->addUnchanged(spanLength);
                }
                જો ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, spanLength);
                }
            }
            spanCondition = USET_SPAN_SIMPLE;
        } નહીંતર {
            જો (spanLength != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, spanLength), sink, edits, errorCode);
                જો (U_FAILURE(errorCode)) {
                    તોડો;
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
                                              અચલ UnicodeString& second,
                                              UErrorCode& errorCode) અચલ {
    પાછા normalizeSecondAndAppend(first, second, સાચું, errorCode);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            અચલ UnicodeString& second,
                            UErrorCode& errorCode) અચલ {
    પાછા normalizeSecondAndAppend(first, second, ખોટું, errorCode);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              અચલ UnicodeString& second,
                                              UBool doNormalize,
                                              UErrorCode& errorCode) અચલ {
    uprv_checkCanGetBuffer(first, errorCode);
    uprv_checkCanGetBuffer(second, errorCode);
    જો(U_FAILURE(errorCode)) {
        પાછા first;
    }
    જો(&first==&second) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા first;
    }
    જો(first.isEmpty()) {
        જો(doNormalize) {
            પાછા normalize(second, first, errorCode);
        } નહીંતર {
            પાછા first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t prefixLimit=set.span(second, 0, USET_SPAN_SIMPLE);
    જો(prefixLimit!=0) {
        UnicodeString prefix(second.tempSubString(0, prefixLimit));
        int32_t suffixStart=set.spanBack(first, INT32_MAX, USET_SPAN_SIMPLE);
        જો(suffixStart==0) {
            જો(doNormalize) {
                norm2.normalizeSecondAndAppend(first, prefix, errorCode);
            } નહીંતર {
                norm2.append(first, prefix, errorCode);
            }
        } નહીંતર {
            UnicodeString મધ્ય(first, suffixStart, INT32_MAX);
            જો(doNormalize) {
                norm2.normalizeSecondAndAppend(મધ્ય, prefix, errorCode);
            } નહીંતર {
                norm2.append(મધ્ય, prefix, errorCode);
            }
            first.replace(suffixStart, INT32_MAX, મધ્ય);
        }
    }
    જો(prefixLimit<second.length()) {
        UnicodeString rest(second.tempSubString(prefixLimit, INT32_MAX));
        જો(doNormalize) {
            normalize(rest, first, USET_SPAN_NOT_CONTAINED, errorCode);
        } નહીંતર {
            first.append(rest);
        }
    }
    પાછા first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) અચલ {
    પાછા set.contains(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) અચલ {
    પાછા set.contains(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) અચલ {
    પાછા (set.contains(a) && set.contains(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) અચલ {
    પાછા set.contains(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(અચલ UnicodeString &s, UErrorCode &errorCode) અચલ {
    uprv_checkCanGetBuffer(s, errorCode);
    જો(U_FAILURE(errorCode)) {
        પાછા ખોટું;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    માટે(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        જો(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } નહીંતર {
            જો( !norm2.isNormalized(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode) ||
                U_FAILURE(errorCode)
            ) {
                પાછા ખોટું;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    પાછા સાચું;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) અચલ {
    જો(U_FAILURE(errorCode)) {
        પાછા ખોટું;
    }
    અચલ અક્ષર *s = sp.data();
    int32_t length = sp.length();
    USetSpanCondition spanCondition = USET_SPAN_SIMPLE;
    જ્યારે (length > 0) {
        int32_t spanLength = set.spanUTF8(s, length, spanCondition);
        જો (spanCondition == USET_SPAN_NOT_CONTAINED) {
            spanCondition = USET_SPAN_SIMPLE;
        } નહીંતર {
            જો (!norm2.isNormalizedUTF8(StringPiece(s, spanLength), errorCode) ||
                    U_FAILURE(errorCode)) {
                પાછા ખોટું;
            }
            spanCondition = USET_SPAN_NOT_CONTAINED;
        }
        s += spanLength;
        length -= spanLength;
    }
    પાછા સાચું;
}

UNormalizationCheckResult
FilteredNormalizer2::quickCheck(અચલ UnicodeString &s, UErrorCode &errorCode) અચલ {
    uprv_checkCanGetBuffer(s, errorCode);
    જો(U_FAILURE(errorCode)) {
        પાછા UNORM_MAYBE;
    }
    UNormalizationCheckResult result=UNORM_YES;
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    માટે(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        જો(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } નહીંતર {
            UNormalizationCheckResult qcResult=
                norm2.quickCheck(s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            જો(U_FAILURE(errorCode) || qcResult==UNORM_NO) {
                પાછા qcResult;
            } નહીંતર જો(qcResult==UNORM_MAYBE) {
                result=qcResult;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    પાછા result;
}

int32_t
FilteredNormalizer2::spanQuickCheckYes(અચલ UnicodeString &s, UErrorCode &errorCode) અચલ {
    uprv_checkCanGetBuffer(s, errorCode);
    જો(U_FAILURE(errorCode)) {
        પાછા 0;
    }
    USetSpanCondition spanCondition=USET_SPAN_SIMPLE;
    માટે(int32_t prevSpanLimit=0; prevSpanLimit<s.length();) {
        int32_t spanLimit=set.span(s, prevSpanLimit, spanCondition);
        જો(spanCondition==USET_SPAN_NOT_CONTAINED) {
            spanCondition=USET_SPAN_SIMPLE;
        } નહીંતર {
            int32_t yesLimit=
                prevSpanLimit+
                norm2.spanQuickCheckYes(
                    s.tempSubStringBetween(prevSpanLimit, spanLimit), errorCode);
            જો(U_FAILURE(errorCode) || yesLimit<spanLimit) {
                પાછા yesLimit;
            }
            spanCondition=USET_SPAN_NOT_CONTAINED;
        }
        prevSpanLimit=spanLimit;
    }
    પાછા s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) અચલ {
    પાછા !set.contains(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) અચલ {
    પાછા !set.contains(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) અચલ {
    પાછા !set.contains(c) || norm2.isInert(c);
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(અચલ UNormalizer2* norm2 U_LIFETIME_BOUND,
                    અચલ USet* filterSet U_LIFETIME_BOUND,
                    UErrorCode* pErrorCode) {
    જો(U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    જો(filterSet==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    Normalizer2 *fn2=નવું FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *UnicodeSet::fromUSet(filterSet));
    જો(fn2==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    પાછા (UNormalizer2 *)fn2;
}

#endif  // !UCONFIG_NO_NORMALIZATION
