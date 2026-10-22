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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_NORMALIZATION

#అన్తర్భావ "unicode/edits.h"
#అన్తర్భావ "unicode/normalizer2.h"
#అన్తర్భావ "unicode/stringoptions.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/unorm.h"
#అన్తర్భావ "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(స్థిరరాశి UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    uprv_check_Can_Get_బఫరు(src, error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        dest.సమితి_గమ్యం_Bogus();
        తిరిగి dest;
    }
    ఒకవేళ(&dest==&src) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి dest;
    }
    dest.remove();
    తిరిగి normalize(src, dest, USET_పరిధి_SIMPLE, error_సంకేతం);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(స్థిరరాశి UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USet_పరిధి_Condition పరిధి_Condition,
                               UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    UnicodeString తాత్కాలిక_Dest;  // Don't throw away destination buffer between iterations.
    కొరకు(int32_t prev_పరిధి_హద్దు=0; prev_పరిధి_హద్దు<src.length();) {
        int32_t పరిధి_హద్దు=సమితి.పరిధి(src, prev_పరిధి_హద్దు, పరిధి_Condition);
        int32_t పరిధి_నిడివి=పరిధి_హద్దు-prev_పరిధి_హద్దు;
        ఒకవేళ(పరిధి_Condition==USET_పరిధి_NOT_CONTAINED) {
            ఒకవేళ(పరిధి_నిడివి!=0) {
                dest.append(src, prev_పరిధి_హద్దు, పరిధి_నిడివి);
            }
            పరిధి_Condition=USET_పరిధి_SIMPLE;
        } లేకపోతే {
            ఒకవేళ(పరిధి_నిడివి!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.తాత్కాలిక_Sub_String_Between(prev_పరిధి_హద్దు, పరిధి_హద్దు),
                                            తాత్కాలిక_Dest, error_సంకేతం));
                ఒకవేళ(U_FAILURE(error_సంకేతం)) {
                    ఆపు;
                }
            }
            పరిధి_Condition=USET_పరిధి_NOT_CONTAINED;
        }
        prev_పరిధి_హద్దు=పరిధి_హద్దు;
    }
    తిరిగి dest;
}

శూన్య
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ (edits != శూన్యసూచిక && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.దత్తాంశం(), src.length(), sink, edits, USET_పరిధి_SIMPLE, error_సంకేతం);
}

శూన్య
FilteredNormalizer2::normalizeUTF8(uint32_t options, స్థిరరాశి అక్షరం *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USet_పరిధి_Condition పరిధి_Condition,
                                   UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    అయితే (length > 0) {
        int32_t పరిధి_నిడివి = సమితి.పరిధి_UTF8(src, length, పరిధి_Condition);
        ఒకవేళ (పరిధి_Condition == USET_పరిధి_NOT_CONTAINED) {
            ఒకవేళ (పరిధి_నిడివి != 0) {
                ఒకవేళ (edits != శూన్యసూచిక) {
                    edits->కూడు_Unchanged(పరిధి_నిడివి);
                }
                ఒకవేళ ((options & U_OMIT_UNCHANGED_పాఠ్యం) == 0) {
                    sink.Append(src, పరిధి_నిడివి);
                }
            }
            పరిధి_Condition = USET_పరిధి_SIMPLE;
        } లేకపోతే {
            ఒకవేళ (పరిధి_నిడివి != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, పరిధి_నిడివి), sink, edits, error_సంకేతం);
                ఒకవేళ (U_FAILURE(error_సంకేతం)) {
                    ఆపు;
                }
            }
            పరిధి_Condition = USET_పరిధి_NOT_CONTAINED;
        }
        src += పరిధి_నిడివి;
        length -= పరిధి_నిడివి;
    }
}

UnicodeString&
FilteredNormalizer2::normalize_రెండవ_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              స్థిరరాశి UnicodeString& రెండవ,
                                              UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    తిరిగి normalize_రెండవ_And_Append(first, రెండవ, నిజం, error_సంకేతం);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            స్థిరరాశి UnicodeString& రెండవ,
                            UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    తిరిగి normalize_రెండవ_And_Append(first, రెండవ, అబద్ధం, error_సంకేతం);
}

UnicodeString&
FilteredNormalizer2::normalize_రెండవ_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              స్థిరరాశి UnicodeString& రెండవ,
                                              UBool doNormalize,
                                              UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    uprv_check_Can_Get_బఫరు(first, error_సంకేతం);
    uprv_check_Can_Get_బఫరు(రెండవ, error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి first;
    }
    ఒకవేళ(&first==&రెండవ) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి first;
    }
    ఒకవేళ(first.is_ఖాళీలు()) {
        ఒకవేళ(doNormalize) {
            తిరిగి normalize(రెండవ, first, error_సంకేతం);
        } లేకపోతే {
            తిరిగి first=రెండవ;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t ఉపసర్గ_హద్దు=సమితి.పరిధి(రెండవ, 0, USET_పరిధి_SIMPLE);
    ఒకవేళ(ఉపసర్గ_హద్దు!=0) {
        UnicodeString ఉపసర్గ(రెండవ.తాత్కాలిక_Sub_String(0, ఉపసర్గ_హద్దు));
        int32_t suffix_ప్రారంభం=సమితి.పరిధి_వెనుక(first, INT32_MAX, USET_పరిధి_SIMPLE);
        ఒకవేళ(suffix_ప్రారంభం==0) {
            ఒకవేళ(doNormalize) {
                norm2.normalize_రెండవ_And_Append(first, ఉపసర్గ, error_సంకేతం);
            } లేకపోతే {
                norm2.append(first, ఉపసర్గ, error_సంకేతం);
            }
        } లేకపోతే {
            UnicodeString మధ్య(first, suffix_ప్రారంభం, INT32_MAX);
            ఒకవేళ(doNormalize) {
                norm2.normalize_రెండవ_And_Append(మధ్య, ఉపసర్గ, error_సంకేతం);
            } లేకపోతే {
                norm2.append(మధ్య, ఉపసర్గ, error_సంకేతం);
            }
            first.replace(suffix_ప్రారంభం, INT32_MAX, మధ్య);
        }
    }
    ఒకవేళ(ఉపసర్గ_హద్దు<రెండవ.length()) {
        UnicodeString rest(రెండవ.తాత్కాలిక_Sub_String(ఉపసర్గ_హద్దు, INT32_MAX));
        ఒకవేళ(doNormalize) {
            normalize(rest, first, USET_పరిధి_NOT_CONTAINED, error_సంకేతం);
        } లేకపోతే {
            first.append(rest);
        }
    }
    తిరిగి first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) స్థిరరాశి {
    తిరిగి సమితి.కలిగియుంది(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) స్థిరరాశి {
    తిరిగి సమితి.కలిగియుంది(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) స్థిరరాశి {
    తిరిగి (సమితి.కలిగియుంది(a) && సమితి.కలిగియుంది(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) స్థిరరాశి {
    తిరిగి సమితి.కలిగియుంది(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(స్థిరరాశి UnicodeString &s, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    uprv_check_Can_Get_బఫరు(s, error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    USet_పరిధి_Condition పరిధి_Condition=USET_పరిధి_SIMPLE;
    కొరకు(int32_t prev_పరిధి_హద్దు=0; prev_పరిధి_హద్దు<s.length();) {
        int32_t పరిధి_హద్దు=సమితి.పరిధి(s, prev_పరిధి_హద్దు, పరిధి_Condition);
        ఒకవేళ(పరిధి_Condition==USET_పరిధి_NOT_CONTAINED) {
            పరిధి_Condition=USET_పరిధి_SIMPLE;
        } లేకపోతే {
            ఒకవేళ( !norm2.isNormalized(s.తాత్కాలిక_Sub_String_Between(prev_పరిధి_హద్దు, పరిధి_హద్దు), error_సంకేతం) ||
                U_FAILURE(error_సంకేతం)
            ) {
                తిరిగి అబద్ధం;
            }
            పరిధి_Condition=USET_పరిధి_NOT_CONTAINED;
        }
        prev_పరిధి_హద్దు=పరిధి_హద్దు;
    }
    తిరిగి నిజం;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి అక్షరం *s = sp.దత్తాంశం();
    int32_t length = sp.length();
    USet_పరిధి_Condition పరిధి_Condition = USET_పరిధి_SIMPLE;
    అయితే (length > 0) {
        int32_t పరిధి_నిడివి = సమితి.పరిధి_UTF8(s, length, పరిధి_Condition);
        ఒకవేళ (పరిధి_Condition == USET_పరిధి_NOT_CONTAINED) {
            పరిధి_Condition = USET_పరిధి_SIMPLE;
        } లేకపోతే {
            ఒకవేళ (!norm2.isNormalizedUTF8(StringPiece(s, పరిధి_నిడివి), error_సంకేతం) ||
                    U_FAILURE(error_సంకేతం)) {
                తిరిగి అబద్ధం;
            }
            పరిధి_Condition = USET_పరిధి_NOT_CONTAINED;
        }
        s += పరిధి_నిడివి;
        length -= పరిధి_నిడివి;
    }
    తిరిగి నిజం;
}

UNormalization_Check_ఫలితం
FilteredNormalizer2::శీఘ్ర_Check(స్థిరరాశి UnicodeString &s, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    uprv_check_Can_Get_బఫరు(s, error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి UNORM_MAYBE;
    }
    UNormalization_Check_ఫలితం ఫలితం=UNORM_YES;
    USet_పరిధి_Condition పరిధి_Condition=USET_పరిధి_SIMPLE;
    కొరకు(int32_t prev_పరిధి_హద్దు=0; prev_పరిధి_హద్దు<s.length();) {
        int32_t పరిధి_హద్దు=సమితి.పరిధి(s, prev_పరిధి_హద్దు, పరిధి_Condition);
        ఒకవేళ(పరిధి_Condition==USET_పరిధి_NOT_CONTAINED) {
            పరిధి_Condition=USET_పరిధి_SIMPLE;
        } లేకపోతే {
            UNormalization_Check_ఫలితం qc_ఫలితం=
                norm2.శీఘ్ర_Check(s.తాత్కాలిక_Sub_String_Between(prev_పరిధి_హద్దు, పరిధి_హద్దు), error_సంకేతం);
            ఒకవేళ(U_FAILURE(error_సంకేతం) || qc_ఫలితం==UNORM_NO) {
                తిరిగి qc_ఫలితం;
            } లేకపోతే ఒకవేళ(qc_ఫలితం==UNORM_MAYBE) {
                ఫలితం=qc_ఫలితం;
            }
            పరిధి_Condition=USET_పరిధి_NOT_CONTAINED;
        }
        prev_పరిధి_హద్దు=పరిధి_హద్దు;
    }
    తిరిగి ఫలితం;
}

int32_t
FilteredNormalizer2::పరిధి_శీఘ్ర_Check_Yes(స్థిరరాశి UnicodeString &s, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    uprv_check_Can_Get_బఫరు(s, error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    USet_పరిధి_Condition పరిధి_Condition=USET_పరిధి_SIMPLE;
    కొరకు(int32_t prev_పరిధి_హద్దు=0; prev_పరిధి_హద్దు<s.length();) {
        int32_t పరిధి_హద్దు=సమితి.పరిధి(s, prev_పరిధి_హద్దు, పరిధి_Condition);
        ఒకవేళ(పరిధి_Condition==USET_పరిధి_NOT_CONTAINED) {
            పరిధి_Condition=USET_పరిధి_SIMPLE;
        } లేకపోతే {
            int32_t yes_హద్దు=
                prev_పరిధి_హద్దు+
                norm2.పరిధి_శీఘ్ర_Check_Yes(
                    s.తాత్కాలిక_Sub_String_Between(prev_పరిధి_హద్దు, పరిధి_హద్దు), error_సంకేతం);
            ఒకవేళ(U_FAILURE(error_సంకేతం) || yes_హద్దు<పరిధి_హద్దు) {
                తిరిగి yes_హద్దు;
            }
            పరిధి_Condition=USET_పరిధి_NOT_CONTAINED;
        }
        prev_పరిధి_హద్దు=పరిధి_హద్దు;
    }
    తిరిగి s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) స్థిరరాశి {
    తిరిగి !సమితి.కలిగియుంది(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) స్థిరరాశి {
    తిరిగి !సమితి.కలిగియుంది(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) స్థిరరాశి {
    తిరిగి !సమితి.కలిగియుంది(c) || norm2.isInert(c);
}

U_NAMESPACE_చివర

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(స్థిరరాశి UNormalizer2* norm2 U_LIFETIME_BOUND,
                    స్థిరరాశి USet* filter_సమితి U_LIFETIME_BOUND,
                    UError_సంకేతం* p_Error_సంకేతం) {
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(filter_సమితి==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    Normalizer2 *fn2=కొత్త FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *Unicode_సమితి::fromUSet(filter_సమితి));
    ఒకవేళ(fn2==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    తిరిగి (UNormalizer2 *)fn2;
}

#అయితే_ముగింపు  // !UCONFIG_NO_NORMALIZATION
