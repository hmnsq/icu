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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_NORMALIZATION

#شامل "unicode/edits.h"
#شامل "unicode/normalizer2.h"
#شامل "unicode/stringoptions.h"
#شامل "unicode/uniset.h"
#شامل "unicode/unistr.h"
#شامل "unicode/unorm.h"
#شامل "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(ثابت UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UError_رمز& error_رمز) ثابت {
    uprv_check_Can_Get_بفر(src, error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        dest.سیٹ_مقصد_Bogus();
        واپس dest;
    }
    اگر(&dest==&src) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس dest;
    }
    dest.remove();
    واپس normalize(src, dest, USET_وسعت_SIMPLE, error_رمز);
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
                               UError_رمز& error_رمز) ثابت {
    UnicodeString عارضی_Dest;  // Don't throw away destination buffer between iterations.
    برائے(int32_t پچھلا_وسعت_حد_بندی=0; پچھلا_وسعت_حد_بندی<src.length();) {
        int32_t وسعت_حد_بندی=سیٹ.وسعت(src, پچھلا_وسعت_حد_بندی, وسعت_Condition);
        int32_t وسعت_لمبائی=وسعت_حد_بندی-پچھلا_وسعت_حد_بندی;
        اگر(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
            اگر(وسعت_لمبائی!=0) {
                dest.append(src, پچھلا_وسعت_حد_بندی, وسعت_لمبائی);
            }
            وسعت_Condition=USET_وسعت_SIMPLE;
        } ورنہ {
            اگر(وسعت_لمبائی!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.عارضی_Sub_String_Between(پچھلا_وسعت_حد_بندی, وسعت_حد_بندی),
                                            عارضی_Dest, error_رمز));
                اگر(U_FAILURE(error_رمز)) {
                    توڑو;
                }
            }
            وسعت_Condition=USET_وسعت_NOT_CONTAINED;
        }
        پچھلا_وسعت_حد_بندی=وسعت_حد_بندی;
    }
    واپس dest;
}

خالی
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) {
        واپس;
    }
    اگر (edits != خالی_اشاریہ && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.ڈیٹا(), src.length(), sink, edits, USET_وسعت_SIMPLE, error_رمز);
}

خالی
FilteredNormalizer2::normalizeUTF8(uint32_t options, ثابت حرف *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USet_وسعت_Condition وسعت_Condition,
                                   UError_رمز &error_رمز) ثابت {
    جب (length > 0) {
        int32_t وسعت_لمبائی = سیٹ.وسعت_UTF8(src, length, وسعت_Condition);
        اگر (وسعت_Condition == USET_وسعت_NOT_CONTAINED) {
            اگر (وسعت_لمبائی != 0) {
                اگر (edits != خالی_اشاریہ) {
                    edits->جوڑو_Unchanged(وسعت_لمبائی);
                }
                اگر ((options & U_OMIT_UNCHANGED_متن) == 0) {
                    sink.Append(src, وسعت_لمبائی);
                }
            }
            وسعت_Condition = USET_وسعت_SIMPLE;
        } ورنہ {
            اگر (وسعت_لمبائی != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, وسعت_لمبائی), sink, edits, error_رمز);
                اگر (U_FAILURE(error_رمز)) {
                    توڑو;
                }
            }
            وسعت_Condition = USET_وسعت_NOT_CONTAINED;
        }
        src += وسعت_لمبائی;
        length -= وسعت_لمبائی;
    }
}

UnicodeString&
FilteredNormalizer2::normalize_دوسرا_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ثابت UnicodeString& دوسرا,
                                              UError_رمز& error_رمز) ثابت {
    واپس normalize_دوسرا_And_Append(first, دوسرا, سچ, error_رمز);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ثابت UnicodeString& دوسرا,
                            UError_رمز& error_رمز) ثابت {
    واپس normalize_دوسرا_And_Append(first, دوسرا, جھوٹ, error_رمز);
}

UnicodeString&
FilteredNormalizer2::normalize_دوسرا_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ثابت UnicodeString& دوسرا,
                                              UBool doNormalize,
                                              UError_رمز& error_رمز) ثابت {
    uprv_check_Can_Get_بفر(first, error_رمز);
    uprv_check_Can_Get_بفر(دوسرا, error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس first;
    }
    اگر(&first==&دوسرا) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس first;
    }
    اگر(first.is_خالی_خانے()) {
        اگر(doNormalize) {
            واپس normalize(دوسرا, first, error_رمز);
        } ورنہ {
            واپس first=دوسرا;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t سابقہ_حد_بندی=سیٹ.وسعت(دوسرا, 0, USET_وسعت_SIMPLE);
    اگر(سابقہ_حد_بندی!=0) {
        UnicodeString سابقہ(دوسرا.عارضی_Sub_String(0, سابقہ_حد_بندی));
        int32_t suffix_آغاز=سیٹ.وسعت_عقب(first, INT32_MAX, USET_وسعت_SIMPLE);
        اگر(suffix_آغاز==0) {
            اگر(doNormalize) {
                norm2.normalize_دوسرا_And_Append(first, سابقہ, error_رمز);
            } ورنہ {
                norm2.append(first, سابقہ, error_رمز);
            }
        } ورنہ {
            UnicodeString درمیان(first, suffix_آغاز, INT32_MAX);
            اگر(doNormalize) {
                norm2.normalize_دوسرا_And_Append(درمیان, سابقہ, error_رمز);
            } ورنہ {
                norm2.append(درمیان, سابقہ, error_رمز);
            }
            first.replace(suffix_آغاز, INT32_MAX, درمیان);
        }
    }
    اگر(سابقہ_حد_بندی<دوسرا.length()) {
        UnicodeString rest(دوسرا.عارضی_Sub_String(سابقہ_حد_بندی, INT32_MAX));
        اگر(doNormalize) {
            normalize(rest, first, USET_وسعت_NOT_CONTAINED, error_رمز);
        } ورنہ {
            first.append(rest);
        }
    }
    واپس first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    واپس سیٹ.شامل_ہے(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    واپس سیٹ.شامل_ہے(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ثابت {
    واپس (سیٹ.شامل_ہے(a) && سیٹ.شامل_ہے(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ثابت {
    واپس سیٹ.شامل_ہے(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ثابت UnicodeString &s, UError_رمز &error_رمز) ثابت {
    uprv_check_Can_Get_بفر(s, error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس جھوٹ;
    }
    USet_وسعت_Condition وسعت_Condition=USET_وسعت_SIMPLE;
    برائے(int32_t پچھلا_وسعت_حد_بندی=0; پچھلا_وسعت_حد_بندی<s.length();) {
        int32_t وسعت_حد_بندی=سیٹ.وسعت(s, پچھلا_وسعت_حد_بندی, وسعت_Condition);
        اگر(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
            وسعت_Condition=USET_وسعت_SIMPLE;
        } ورنہ {
            اگر( !norm2.isNormalized(s.عارضی_Sub_String_Between(پچھلا_وسعت_حد_بندی, وسعت_حد_بندی), error_رمز) ||
                U_FAILURE(error_رمز)
            ) {
                واپس جھوٹ;
            }
            وسعت_Condition=USET_وسعت_NOT_CONTAINED;
        }
        پچھلا_وسعت_حد_بندی=وسعت_حد_بندی;
    }
    واپس سچ;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        واپس جھوٹ;
    }
    ثابت حرف *s = sp.ڈیٹا();
    int32_t length = sp.length();
    USet_وسعت_Condition وسعت_Condition = USET_وسعت_SIMPLE;
    جب (length > 0) {
        int32_t وسعت_لمبائی = سیٹ.وسعت_UTF8(s, length, وسعت_Condition);
        اگر (وسعت_Condition == USET_وسعت_NOT_CONTAINED) {
            وسعت_Condition = USET_وسعت_SIMPLE;
        } ورنہ {
            اگر (!norm2.isNormalizedUTF8(StringPiece(s, وسعت_لمبائی), error_رمز) ||
                    U_FAILURE(error_رمز)) {
                واپس جھوٹ;
            }
            وسعت_Condition = USET_وسعت_NOT_CONTAINED;
        }
        s += وسعت_لمبائی;
        length -= وسعت_لمبائی;
    }
    واپس سچ;
}

UNormalization_Check_نتیجہ
FilteredNormalizer2::تیز_Check(ثابت UnicodeString &s, UError_رمز &error_رمز) ثابت {
    uprv_check_Can_Get_بفر(s, error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس UNORM_MAYBE;
    }
    UNormalization_Check_نتیجہ نتیجہ=UNORM_YES;
    USet_وسعت_Condition وسعت_Condition=USET_وسعت_SIMPLE;
    برائے(int32_t پچھلا_وسعت_حد_بندی=0; پچھلا_وسعت_حد_بندی<s.length();) {
        int32_t وسعت_حد_بندی=سیٹ.وسعت(s, پچھلا_وسعت_حد_بندی, وسعت_Condition);
        اگر(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
            وسعت_Condition=USET_وسعت_SIMPLE;
        } ورنہ {
            UNormalization_Check_نتیجہ qc_نتیجہ=
                norm2.تیز_Check(s.عارضی_Sub_String_Between(پچھلا_وسعت_حد_بندی, وسعت_حد_بندی), error_رمز);
            اگر(U_FAILURE(error_رمز) || qc_نتیجہ==UNORM_NO) {
                واپس qc_نتیجہ;
            } ورنہ اگر(qc_نتیجہ==UNORM_MAYBE) {
                نتیجہ=qc_نتیجہ;
            }
            وسعت_Condition=USET_وسعت_NOT_CONTAINED;
        }
        پچھلا_وسعت_حد_بندی=وسعت_حد_بندی;
    }
    واپس نتیجہ;
}

int32_t
FilteredNormalizer2::وسعت_تیز_Check_Yes(ثابت UnicodeString &s, UError_رمز &error_رمز) ثابت {
    uprv_check_Can_Get_بفر(s, error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    USet_وسعت_Condition وسعت_Condition=USET_وسعت_SIMPLE;
    برائے(int32_t پچھلا_وسعت_حد_بندی=0; پچھلا_وسعت_حد_بندی<s.length();) {
        int32_t وسعت_حد_بندی=سیٹ.وسعت(s, پچھلا_وسعت_حد_بندی, وسعت_Condition);
        اگر(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
            وسعت_Condition=USET_وسعت_SIMPLE;
        } ورنہ {
            int32_t yes_حد_بندی=
                پچھلا_وسعت_حد_بندی+
                norm2.وسعت_تیز_Check_Yes(
                    s.عارضی_Sub_String_Between(پچھلا_وسعت_حد_بندی, وسعت_حد_بندی), error_رمز);
            اگر(U_FAILURE(error_رمز) || yes_حد_بندی<وسعت_حد_بندی) {
                واپس yes_حد_بندی;
            }
            وسعت_Condition=USET_وسعت_NOT_CONTAINED;
        }
        پچھلا_وسعت_حد_بندی=وسعت_حد_بندی;
    }
    واپس s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ثابت {
    واپس !سیٹ.شامل_ہے(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ثابت {
    واپس !سیٹ.شامل_ہے(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ثابت {
    واپس !سیٹ.شامل_ہے(c) || norm2.isInert(c);
}

U_NAMESPACE_انتہا

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ثابت UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ثابت USet* filter_سیٹ U_LIFETIME_BOUND,
                    UError_رمز* p_Error_رمز) {
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر(filter_سیٹ==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    Normalizer2 *fn2=نیا FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *Unicode_سیٹ::fromUSet(filter_سیٹ));
    اگر(fn2==خالی_اشاریہ) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
    }
    واپس (UNormalizer2 *)fn2;
}

#اگر_ختم  // !UCONFIG_NO_NORMALIZATION
