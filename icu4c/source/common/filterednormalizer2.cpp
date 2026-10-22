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
                               UError_کوڈ& error_کوڈ) ثابت {
    uprv_check_Can_Get_بفر(src, error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        dest.سیٹ_منزل_Bogus();
        أرجع dest;
    }
    إذا(&dest==&src) {
        error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع dest;
    }
    dest.remove();
    أرجع normalize(src, dest, USET_وسعت_SIMPLE, error_کوڈ);
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
                               USet_وسعت_Condition وسعت_Condition,
                               UError_کوڈ& error_کوڈ) ثابت {
    UnicodeString عارضی_Dest;  // Don't throw away destination buffer between iterations.
    لكل(int32_t prev_وسعت_حد_بندی=0; prev_وسعت_حد_بندی<src.length();) {
        int32_t وسعت_حد_بندی=سیٹ.وسعت(src, prev_وسعت_حد_بندی, وسعت_Condition);
        int32_t وسعت_لمائی=وسعت_حد_بندی-prev_وسعت_حد_بندی;
        إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
            إذا(وسعت_لمائی!=0) {
                dest.append(src, prev_وسعت_حد_بندی, وسعت_لمائی);
            }
            وسعت_Condition=USET_وسعت_SIMPLE;
        } وإلا {
            إذا(وسعت_لمائی!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.عارضی_Sub_String_Between(prev_وسعت_حد_بندی, وسعت_حد_بندی),
                                            عارضی_Dest, error_کوڈ));
                إذا(U_FAILURE(error_کوڈ)) {
                    اكسر;
                }
            }
            وسعت_Condition=USET_وسعت_NOT_CONTAINED;
        }
        prev_وسعت_حد_بندی=وسعت_حد_بندی;
    }
    أرجع dest;
}

فراغ
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    إذا (edits != خالی_سوچک && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.انکڑے(), src.length(), sink, edits, USET_وسعت_SIMPLE, error_کوڈ);
}

فراغ
FilteredNormalizer2::normalizeUTF8(uint32_t options, ثابت حرف *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USet_وسعت_Condition وسعت_Condition,
                                   UError_کوڈ &error_کوڈ) ثابت {
    طالما (length > 0) {
        int32_t وسعت_لمائی = سیٹ.وسعت_UTF8(src, length, وسعت_Condition);
        إذا (وسعت_Condition == USET_وسعت_NOT_CONTAINED) {
            إذا (وسعت_لمائی != 0) {
                إذا (edits != خالی_سوچک) {
                    edits->جوڑو_Unchanged(وسعت_لمائی);
                }
                إذا ((options & U_OMIT_UNCHANGED_متن) == 0) {
                    sink.Append(src, وسعت_لمائی);
                }
            }
            وسعت_Condition = USET_وسعت_SIMPLE;
        } وإلا {
            إذا (وسعت_لمائی != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, وسعت_لمائی), sink, edits, error_کوڈ);
                إذا (U_FAILURE(error_کوڈ)) {
                    اكسر;
                }
            }
            وسعت_Condition = USET_وسعت_NOT_CONTAINED;
        }
        src += وسعت_لمائی;
        length -= وسعت_لمائی;
    }
}

UnicodeString&
FilteredNormalizer2::normalize_دوجا_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ثابت UnicodeString& دوجا,
                                              UError_کوڈ& error_کوڈ) ثابت {
    أرجع normalize_دوجا_And_Append(first, دوجا, سچ, error_کوڈ);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ثابت UnicodeString& دوجا,
                            UError_کوڈ& error_کوڈ) ثابت {
    أرجع normalize_دوجا_And_Append(first, دوجا, خطأ, error_کوڈ);
}

UnicodeString&
FilteredNormalizer2::normalize_دوجا_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ثابت UnicodeString& دوجا,
                                              UBool doNormalize,
                                              UError_کوڈ& error_کوڈ) ثابت {
    uprv_check_Can_Get_بفر(first, error_کوڈ);
    uprv_check_Can_Get_بفر(دوجا, error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع first;
    }
    إذا(&first==&دوجا) {
        error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع first;
    }
    إذا(first.is_خالی_تھاواں()) {
        إذا(doNormalize) {
            أرجع normalize(دوجا, first, error_کوڈ);
        } وإلا {
            أرجع first=دوجا;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t اگیتر_حد_بندی=سیٹ.وسعت(دوجا, 0, USET_وسعت_SIMPLE);
    إذا(اگیتر_حد_بندی!=0) {
        UnicodeString اگیتر(دوجا.عارضی_Sub_String(0, اگیتر_حد_بندی));
        int32_t suffix_شروع=سیٹ.وسعت_پٹھ(first, INT32_MAX, USET_وسعت_SIMPLE);
        إذا(suffix_شروع==0) {
            إذا(doNormalize) {
                norm2.normalize_دوجا_And_Append(first, اگیتر, error_کوڈ);
            } وإلا {
                norm2.append(first, اگیتر, error_کوڈ);
            }
        } وإلا {
            UnicodeString درمیان(first, suffix_شروع, INT32_MAX);
            إذا(doNormalize) {
                norm2.normalize_دوجا_And_Append(درمیان, اگیتر, error_کوڈ);
            } وإلا {
                norm2.append(درمیان, اگیتر, error_کوڈ);
            }
            first.replace(suffix_شروع, INT32_MAX, درمیان);
        }
    }
    إذا(اگیتر_حد_بندی<دوجا.length()) {
        UnicodeString rest(دوجا.عارضی_Sub_String(اگیتر_حد_بندی, INT32_MAX));
        إذا(doNormalize) {
            normalize(rest, first, USET_وسعت_NOT_CONTAINED, error_کوڈ);
        } وإلا {
            first.append(rest);
        }
    }
    أرجع first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    أرجع سیٹ.شامل_اے(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    أرجع سیٹ.شامل_اے(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ثابت {
    أرجع (سیٹ.شامل_اے(a) && سیٹ.شامل_اے(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ثابت {
    أرجع سیٹ.شامل_اے(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ثابت UnicodeString &s, UError_کوڈ &error_کوڈ) ثابت {
    uprv_check_Can_Get_بفر(s, error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خطأ;
    }
    USet_وسعت_Condition وسعت_Condition=USET_وسعت_SIMPLE;
    لكل(int32_t prev_وسعت_حد_بندی=0; prev_وسعت_حد_بندی<s.length();) {
        int32_t وسعت_حد_بندی=سیٹ.وسعت(s, prev_وسعت_حد_بندی, وسعت_Condition);
        إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
            وسعت_Condition=USET_وسعت_SIMPLE;
        } وإلا {
            إذا( !norm2.isNormalized(s.عارضی_Sub_String_Between(prev_وسعت_حد_بندی, وسعت_حد_بندی), error_کوڈ) ||
                U_FAILURE(error_کوڈ)
            ) {
                أرجع خطأ;
            }
            وسعت_Condition=USET_وسعت_NOT_CONTAINED;
        }
        prev_وسعت_حد_بندی=وسعت_حد_بندی;
    }
    أرجع سچ;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UError_کوڈ &error_کوڈ) ثابت {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خطأ;
    }
    ثابت حرف *s = sp.انکڑے();
    int32_t length = sp.length();
    USet_وسعت_Condition وسعت_Condition = USET_وسعت_SIMPLE;
    طالما (length > 0) {
        int32_t وسعت_لمائی = سیٹ.وسعت_UTF8(s, length, وسعت_Condition);
        إذا (وسعت_Condition == USET_وسعت_NOT_CONTAINED) {
            وسعت_Condition = USET_وسعت_SIMPLE;
        } وإلا {
            إذا (!norm2.isNormalizedUTF8(StringPiece(s, وسعت_لمائی), error_کوڈ) ||
                    U_FAILURE(error_کوڈ)) {
                أرجع خطأ;
            }
            وسعت_Condition = USET_وسعت_NOT_CONTAINED;
        }
        s += وسعت_لمائی;
        length -= وسعت_لمائی;
    }
    أرجع سچ;
}

UNormalization_Check_نتیجہ
FilteredNormalizer2::تیز_Check(ثابت UnicodeString &s, UError_کوڈ &error_کوڈ) ثابت {
    uprv_check_Can_Get_بفر(s, error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع UNORM_MAYBE;
    }
    UNormalization_Check_نتیجہ نتیجہ=UNORM_YES;
    USet_وسعت_Condition وسعت_Condition=USET_وسعت_SIMPLE;
    لكل(int32_t prev_وسعت_حد_بندی=0; prev_وسعت_حد_بندی<s.length();) {
        int32_t وسعت_حد_بندی=سیٹ.وسعت(s, prev_وسعت_حد_بندی, وسعت_Condition);
        إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
            وسعت_Condition=USET_وسعت_SIMPLE;
        } وإلا {
            UNormalization_Check_نتیجہ qc_نتیجہ=
                norm2.تیز_Check(s.عارضی_Sub_String_Between(prev_وسعت_حد_بندی, وسعت_حد_بندی), error_کوڈ);
            إذا(U_FAILURE(error_کوڈ) || qc_نتیجہ==UNORM_NO) {
                أرجع qc_نتیجہ;
            } وإلا إذا(qc_نتیجہ==UNORM_MAYBE) {
                نتیجہ=qc_نتیجہ;
            }
            وسعت_Condition=USET_وسعت_NOT_CONTAINED;
        }
        prev_وسعت_حد_بندی=وسعت_حد_بندی;
    }
    أرجع نتیجہ;
}

int32_t
FilteredNormalizer2::وسعت_تیز_Check_Yes(ثابت UnicodeString &s, UError_کوڈ &error_کوڈ) ثابت {
    uprv_check_Can_Get_بفر(s, error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع 0;
    }
    USet_وسعت_Condition وسعت_Condition=USET_وسعت_SIMPLE;
    لكل(int32_t prev_وسعت_حد_بندی=0; prev_وسعت_حد_بندی<s.length();) {
        int32_t وسعت_حد_بندی=سیٹ.وسعت(s, prev_وسعت_حد_بندی, وسعت_Condition);
        إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
            وسعت_Condition=USET_وسعت_SIMPLE;
        } وإلا {
            int32_t yes_حد_بندی=
                prev_وسعت_حد_بندی+
                norm2.وسعت_تیز_Check_Yes(
                    s.عارضی_Sub_String_Between(prev_وسعت_حد_بندی, وسعت_حد_بندی), error_کوڈ);
            إذا(U_FAILURE(error_کوڈ) || yes_حد_بندی<وسعت_حد_بندی) {
                أرجع yes_حد_بندی;
            }
            وسعت_Condition=USET_وسعت_NOT_CONTAINED;
        }
        prev_وسعت_حد_بندی=وسعت_حد_بندی;
    }
    أرجع s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ثابت {
    أرجع !سیٹ.شامل_اے(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ثابت {
    أرجع !سیٹ.شامل_اے(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ثابت {
    أرجع !سیٹ.شامل_اے(c) || norm2.isInert(c);
}

U_NAMESPACE_سرا

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ثابت UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ثابت USet* filter_سیٹ U_LIFETIME_BOUND,
                    UError_کوڈ* p_Error_کوڈ) {
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا(filter_سیٹ==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    Normalizer2 *fn2=جديد FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *Unicode_سیٹ::fromUSet(filter_سیٹ));
    إذا(fn2==خالی_سوچک) {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
    }
    أرجع (UNormalizer2 *)fn2;
}

#جے_انت  // !UCONFIG_NO_NORMALIZATION
