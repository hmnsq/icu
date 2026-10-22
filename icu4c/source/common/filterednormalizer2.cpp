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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_NORMALIZATION

#ضمن "unicode/edits.h"
#ضمن "unicode/normalizer2.h"
#ضمن "unicode/stringoptions.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/unorm.h"
#ضمن "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(ثابت UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UError_کوډ& error_کوډ) ثابت {
    uprv_check_Can_Get_بفر(src, error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        dest.سېټ_منزل_Bogus();
        أرجع dest;
    }
    إذا(&dest==&src) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع dest;
    }
    dest.remove();
    أرجع normalize(src, dest, USET_پراختیا_SIMPLE, error_کوډ);
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
                               USet_پراختیا_Condition پراختیا_Condition,
                               UError_کوډ& error_کوډ) ثابت {
    UnicodeString لنډمهاله_Dest;  // Don't throw away destination buffer between iterations.
    لكل(int32_t prev_پراختیا_حد=0; prev_پراختیا_حد<src.length();) {
        int32_t پراختیا_حد=سېټ.پراختیا(src, prev_پراختیا_حد, پراختیا_Condition);
        int32_t پراختیا_اوږدتیا=پراختیا_حد-prev_پراختیا_حد;
        إذا(پراختیا_Condition==USET_پراختیا_NOT_CONTAINED) {
            إذا(پراختیا_اوږدتیا!=0) {
                dest.append(src, prev_پراختیا_حد, پراختیا_اوږدتیا);
            }
            پراختیا_Condition=USET_پراختیا_SIMPLE;
        } وإلا {
            إذا(پراختیا_اوږدتیا!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.لنډمهاله_Sub_String_Between(prev_پراختیا_حد, پراختیا_حد),
                                            لنډمهاله_Dest, error_کوډ));
                إذا(U_FAILURE(error_کوډ)) {
                    اكسر;
                }
            }
            پراختیا_Condition=USET_پراختیا_NOT_CONTAINED;
        }
        prev_پراختیا_حد=پراختیا_حد;
    }
    أرجع dest;
}

فراغ
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع;
    }
    إذا (edits != تش_اشاره && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.معلومات(), src.length(), sink, edits, USET_پراختیا_SIMPLE, error_کوډ);
}

فراغ
FilteredNormalizer2::normalizeUTF8(uint32_t options, ثابت حرف *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USet_پراختیا_Condition پراختیا_Condition,
                                   UError_کوډ &error_کوډ) ثابت {
    طالما (length > 0) {
        int32_t پراختیا_اوږدتیا = سېټ.پراختیا_UTF8(src, length, پراختیا_Condition);
        إذا (پراختیا_Condition == USET_پراختیا_NOT_CONTAINED) {
            إذا (پراختیا_اوږدتیا != 0) {
                إذا (edits != تش_اشاره) {
                    edits->جمع_کړه_Unchanged(پراختیا_اوږدتیا);
                }
                إذا ((options & U_OMIT_UNCHANGED_متن) == 0) {
                    sink.Append(src, پراختیا_اوږدتیا);
                }
            }
            پراختیا_Condition = USET_پراختیا_SIMPLE;
        } وإلا {
            إذا (پراختیا_اوږدتیا != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, پراختیا_اوږدتیا), sink, edits, error_کوډ);
                إذا (U_FAILURE(error_کوډ)) {
                    اكسر;
                }
            }
            پراختیا_Condition = USET_پراختیا_NOT_CONTAINED;
        }
        src += پراختیا_اوږدتیا;
        length -= پراختیا_اوږدتیا;
    }
}

UnicodeString&
FilteredNormalizer2::normalize_دوهم_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ثابت UnicodeString& دوهم,
                                              UError_کوډ& error_کوډ) ثابت {
    أرجع normalize_دوهم_And_Append(first, دوهم, ریښتیا, error_کوډ);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ثابت UnicodeString& دوهم,
                            UError_کوډ& error_کوډ) ثابت {
    أرجع normalize_دوهم_And_Append(first, دوهم, خطأ, error_کوډ);
}

UnicodeString&
FilteredNormalizer2::normalize_دوهم_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ثابت UnicodeString& دوهم,
                                              UBool doNormalize,
                                              UError_کوډ& error_کوډ) ثابت {
    uprv_check_Can_Get_بفر(first, error_کوډ);
    uprv_check_Can_Get_بفر(دوهم, error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع first;
    }
    إذا(&first==&دوهم) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع first;
    }
    إذا(first.is_تشه()) {
        إذا(doNormalize) {
            أرجع normalize(دوهم, first, error_کوډ);
        } وإلا {
            أرجع first=دوهم;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t مختاړی_حد=سېټ.پراختیا(دوهم, 0, USET_پراختیا_SIMPLE);
    إذا(مختاړی_حد!=0) {
        UnicodeString مختاړی(دوهم.لنډمهاله_Sub_String(0, مختاړی_حد));
        int32_t suffix_پیل=سېټ.پراختیا_شا(first, INT32_MAX, USET_پراختیا_SIMPLE);
        إذا(suffix_پیل==0) {
            إذا(doNormalize) {
                norm2.normalize_دوهم_And_Append(first, مختاړی, error_کوډ);
            } وإلا {
                norm2.append(first, مختاړی, error_کوډ);
            }
        } وإلا {
            UnicodeString منځ(first, suffix_پیل, INT32_MAX);
            إذا(doNormalize) {
                norm2.normalize_دوهم_And_Append(منځ, مختاړی, error_کوډ);
            } وإلا {
                norm2.append(منځ, مختاړی, error_کوډ);
            }
            first.replace(suffix_پیل, INT32_MAX, منځ);
        }
    }
    إذا(مختاړی_حد<دوهم.length()) {
        UnicodeString rest(دوهم.لنډمهاله_Sub_String(مختاړی_حد, INT32_MAX));
        إذا(doNormalize) {
            normalize(rest, first, USET_پراختیا_NOT_CONTAINED, error_کوډ);
        } وإلا {
            first.append(rest);
        }
    }
    أرجع first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    أرجع سېټ.لري(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    أرجع سېټ.لري(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ثابت {
    أرجع (سېټ.لري(a) && سېټ.لري(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ثابت {
    أرجع سېټ.لري(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ثابت UnicodeString &s, UError_کوډ &error_کوډ) ثابت {
    uprv_check_Can_Get_بفر(s, error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع خطأ;
    }
    USet_پراختیا_Condition پراختیا_Condition=USET_پراختیا_SIMPLE;
    لكل(int32_t prev_پراختیا_حد=0; prev_پراختیا_حد<s.length();) {
        int32_t پراختیا_حد=سېټ.پراختیا(s, prev_پراختیا_حد, پراختیا_Condition);
        إذا(پراختیا_Condition==USET_پراختیا_NOT_CONTAINED) {
            پراختیا_Condition=USET_پراختیا_SIMPLE;
        } وإلا {
            إذا( !norm2.isNormalized(s.لنډمهاله_Sub_String_Between(prev_پراختیا_حد, پراختیا_حد), error_کوډ) ||
                U_FAILURE(error_کوډ)
            ) {
                أرجع خطأ;
            }
            پراختیا_Condition=USET_پراختیا_NOT_CONTAINED;
        }
        prev_پراختیا_حد=پراختیا_حد;
    }
    أرجع ریښتیا;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع خطأ;
    }
    ثابت حرف *s = sp.معلومات();
    int32_t length = sp.length();
    USet_پراختیا_Condition پراختیا_Condition = USET_پراختیا_SIMPLE;
    طالما (length > 0) {
        int32_t پراختیا_اوږدتیا = سېټ.پراختیا_UTF8(s, length, پراختیا_Condition);
        إذا (پراختیا_Condition == USET_پراختیا_NOT_CONTAINED) {
            پراختیا_Condition = USET_پراختیا_SIMPLE;
        } وإلا {
            إذا (!norm2.isNormalizedUTF8(StringPiece(s, پراختیا_اوږدتیا), error_کوډ) ||
                    U_FAILURE(error_کوډ)) {
                أرجع خطأ;
            }
            پراختیا_Condition = USET_پراختیا_NOT_CONTAINED;
        }
        s += پراختیا_اوږدتیا;
        length -= پراختیا_اوږدتیا;
    }
    أرجع ریښتیا;
}

UNormalization_Check_پایله
FilteredNormalizer2::چټک_Check(ثابت UnicodeString &s, UError_کوډ &error_کوډ) ثابت {
    uprv_check_Can_Get_بفر(s, error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع UNORM_MAYBE;
    }
    UNormalization_Check_پایله پایله=UNORM_YES;
    USet_پراختیا_Condition پراختیا_Condition=USET_پراختیا_SIMPLE;
    لكل(int32_t prev_پراختیا_حد=0; prev_پراختیا_حد<s.length();) {
        int32_t پراختیا_حد=سېټ.پراختیا(s, prev_پراختیا_حد, پراختیا_Condition);
        إذا(پراختیا_Condition==USET_پراختیا_NOT_CONTAINED) {
            پراختیا_Condition=USET_پراختیا_SIMPLE;
        } وإلا {
            UNormalization_Check_پایله qc_پایله=
                norm2.چټک_Check(s.لنډمهاله_Sub_String_Between(prev_پراختیا_حد, پراختیا_حد), error_کوډ);
            إذا(U_FAILURE(error_کوډ) || qc_پایله==UNORM_NO) {
                أرجع qc_پایله;
            } وإلا إذا(qc_پایله==UNORM_MAYBE) {
                پایله=qc_پایله;
            }
            پراختیا_Condition=USET_پراختیا_NOT_CONTAINED;
        }
        prev_پراختیا_حد=پراختیا_حد;
    }
    أرجع پایله;
}

int32_t
FilteredNormalizer2::پراختیا_چټک_Check_Yes(ثابت UnicodeString &s, UError_کوډ &error_کوډ) ثابت {
    uprv_check_Can_Get_بفر(s, error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    USet_پراختیا_Condition پراختیا_Condition=USET_پراختیا_SIMPLE;
    لكل(int32_t prev_پراختیا_حد=0; prev_پراختیا_حد<s.length();) {
        int32_t پراختیا_حد=سېټ.پراختیا(s, prev_پراختیا_حد, پراختیا_Condition);
        إذا(پراختیا_Condition==USET_پراختیا_NOT_CONTAINED) {
            پراختیا_Condition=USET_پراختیا_SIMPLE;
        } وإلا {
            int32_t yes_حد=
                prev_پراختیا_حد+
                norm2.پراختیا_چټک_Check_Yes(
                    s.لنډمهاله_Sub_String_Between(prev_پراختیا_حد, پراختیا_حد), error_کوډ);
            إذا(U_FAILURE(error_کوډ) || yes_حد<پراختیا_حد) {
                أرجع yes_حد;
            }
            پراختیا_Condition=USET_پراختیا_NOT_CONTAINED;
        }
        prev_پراختیا_حد=پراختیا_حد;
    }
    أرجع s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ثابت {
    أرجع !سېټ.لري(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ثابت {
    أرجع !سېټ.لري(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ثابت {
    أرجع !سېټ.لري(c) || norm2.isInert(c);
}

U_NAMESPACE_پای_ټکی

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ثابت UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ثابت USet* filter_سېټ U_LIFETIME_BOUND,
                    UError_کوډ* p_Error_کوډ) {
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا(filter_سېټ==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    Normalizer2 *fn2=جديد FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *Unicode_سېټ::fromUSet(filter_سېټ));
    إذا(fn2==تش_اشاره) {
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
    }
    أرجع (UNormalizer2 *)fn2;
}

#د_که_پای  // !UCONFIG_NO_NORMALIZATION
