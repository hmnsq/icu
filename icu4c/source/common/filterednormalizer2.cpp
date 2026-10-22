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
                               UError_کد& error_کد) ثابت {
    uprv_check_Can_Get_میانگیر(src, error_کد);
    إذا(U_FAILURE(error_کد)) {
        dest.مجموعه_مقصد_Bogus();
        أرجع dest;
    }
    إذا(&dest==&src) {
        error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع dest;
    }
    dest.remove();
    أرجع normalize(src, dest, USET_بازه_SIMPLE, error_کد);
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
                               USet_بازه_Condition بازه_Condition,
                               UError_کد& error_کد) ثابت {
    UnicodeString موقت_Dest;  // Don't throw away destination buffer between iterations.
    لكل(int32_t prev_بازه_حد=0; prev_بازه_حد<src.length();) {
        int32_t بازه_حد=مجموعه.بازه(src, prev_بازه_حد, بازه_Condition);
        int32_t بازه_درازی=بازه_حد-prev_بازه_حد;
        إذا(بازه_Condition==USET_بازه_NOT_CONTAINED) {
            إذا(بازه_درازی!=0) {
                dest.append(src, prev_بازه_حد, بازه_درازی);
            }
            بازه_Condition=USET_بازه_SIMPLE;
        } وإلا {
            إذا(بازه_درازی!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.موقت_Sub_String_Between(prev_بازه_حد, بازه_حد),
                                            موقت_Dest, error_کد));
                إذا(U_FAILURE(error_کد)) {
                    اكسر;
                }
            }
            بازه_Condition=USET_بازه_NOT_CONTAINED;
        }
        prev_بازه_حد=بازه_حد;
    }
    أرجع dest;
}

فراغ
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) {
        أرجع;
    }
    إذا (edits != اشاره_تهی && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.داده(), src.length(), sink, edits, USET_بازه_SIMPLE, error_کد);
}

فراغ
FilteredNormalizer2::normalizeUTF8(uint32_t options, ثابت حرف *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USet_بازه_Condition بازه_Condition,
                                   UError_کد &error_کد) ثابت {
    طالما (length > 0) {
        int32_t بازه_درازی = مجموعه.بازه_UTF8(src, length, بازه_Condition);
        إذا (بازه_Condition == USET_بازه_NOT_CONTAINED) {
            إذا (بازه_درازی != 0) {
                إذا (edits != اشاره_تهی) {
                    edits->جمع_کن_Unchanged(بازه_درازی);
                }
                إذا ((options & U_OMIT_UNCHANGED_متن) == 0) {
                    sink.Append(src, بازه_درازی);
                }
            }
            بازه_Condition = USET_بازه_SIMPLE;
        } وإلا {
            إذا (بازه_درازی != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, بازه_درازی), sink, edits, error_کد);
                إذا (U_FAILURE(error_کد)) {
                    اكسر;
                }
            }
            بازه_Condition = USET_بازه_NOT_CONTAINED;
        }
        src += بازه_درازی;
        length -= بازه_درازی;
    }
}

UnicodeString&
FilteredNormalizer2::normalize_دومی_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ثابت UnicodeString& دومی,
                                              UError_کد& error_کد) ثابت {
    أرجع normalize_دومی_And_Append(first, دومی, درست, error_کد);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ثابت UnicodeString& دومی,
                            UError_کد& error_کد) ثابت {
    أرجع normalize_دومی_And_Append(first, دومی, خطأ, error_کد);
}

UnicodeString&
FilteredNormalizer2::normalize_دومی_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ثابت UnicodeString& دومی,
                                              UBool doNormalize,
                                              UError_کد& error_کد) ثابت {
    uprv_check_Can_Get_میانگیر(first, error_کد);
    uprv_check_Can_Get_میانگیر(دومی, error_کد);
    إذا(U_FAILURE(error_کد)) {
        أرجع first;
    }
    إذا(&first==&دومی) {
        error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع first;
    }
    إذا(first.is_خالی()) {
        إذا(doNormalize) {
            أرجع normalize(دومی, first, error_کد);
        } وإلا {
            أرجع first=دومی;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t پیشوند_حد=مجموعه.بازه(دومی, 0, USET_بازه_SIMPLE);
    إذا(پیشوند_حد!=0) {
        UnicodeString پیشوند(دومی.موقت_Sub_String(0, پیشوند_حد));
        int32_t suffix_شروع=مجموعه.بازه_پشت(first, INT32_MAX, USET_بازه_SIMPLE);
        إذا(suffix_شروع==0) {
            إذا(doNormalize) {
                norm2.normalize_دومی_And_Append(first, پیشوند, error_کد);
            } وإلا {
                norm2.append(first, پیشوند, error_کد);
            }
        } وإلا {
            UnicodeString میانه(first, suffix_شروع, INT32_MAX);
            إذا(doNormalize) {
                norm2.normalize_دومی_And_Append(میانه, پیشوند, error_کد);
            } وإلا {
                norm2.append(میانه, پیشوند, error_کد);
            }
            first.replace(suffix_شروع, INT32_MAX, میانه);
        }
    }
    إذا(پیشوند_حد<دومی.length()) {
        UnicodeString rest(دومی.موقت_Sub_String(پیشوند_حد, INT32_MAX));
        إذا(doNormalize) {
            normalize(rest, first, USET_بازه_NOT_CONTAINED, error_کد);
        } وإلا {
            first.append(rest);
        }
    }
    أرجع first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    أرجع مجموعه.شامل_است(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    أرجع مجموعه.شامل_است(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ثابت {
    أرجع (مجموعه.شامل_است(a) && مجموعه.شامل_است(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ثابت {
    أرجع مجموعه.شامل_است(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ثابت UnicodeString &s, UError_کد &error_کد) ثابت {
    uprv_check_Can_Get_میانگیر(s, error_کد);
    إذا(U_FAILURE(error_کد)) {
        أرجع خطأ;
    }
    USet_بازه_Condition بازه_Condition=USET_بازه_SIMPLE;
    لكل(int32_t prev_بازه_حد=0; prev_بازه_حد<s.length();) {
        int32_t بازه_حد=مجموعه.بازه(s, prev_بازه_حد, بازه_Condition);
        إذا(بازه_Condition==USET_بازه_NOT_CONTAINED) {
            بازه_Condition=USET_بازه_SIMPLE;
        } وإلا {
            إذا( !norm2.isNormalized(s.موقت_Sub_String_Between(prev_بازه_حد, بازه_حد), error_کد) ||
                U_FAILURE(error_کد)
            ) {
                أرجع خطأ;
            }
            بازه_Condition=USET_بازه_NOT_CONTAINED;
        }
        prev_بازه_حد=بازه_حد;
    }
    أرجع درست;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        أرجع خطأ;
    }
    ثابت حرف *s = sp.داده();
    int32_t length = sp.length();
    USet_بازه_Condition بازه_Condition = USET_بازه_SIMPLE;
    طالما (length > 0) {
        int32_t بازه_درازی = مجموعه.بازه_UTF8(s, length, بازه_Condition);
        إذا (بازه_Condition == USET_بازه_NOT_CONTAINED) {
            بازه_Condition = USET_بازه_SIMPLE;
        } وإلا {
            إذا (!norm2.isNormalizedUTF8(StringPiece(s, بازه_درازی), error_کد) ||
                    U_FAILURE(error_کد)) {
                أرجع خطأ;
            }
            بازه_Condition = USET_بازه_NOT_CONTAINED;
        }
        s += بازه_درازی;
        length -= بازه_درازی;
    }
    أرجع درست;
}

UNormalization_Check_نتیجه
FilteredNormalizer2::سریع_Check(ثابت UnicodeString &s, UError_کد &error_کد) ثابت {
    uprv_check_Can_Get_میانگیر(s, error_کد);
    إذا(U_FAILURE(error_کد)) {
        أرجع UNORM_MAYBE;
    }
    UNormalization_Check_نتیجه نتیجه=UNORM_YES;
    USet_بازه_Condition بازه_Condition=USET_بازه_SIMPLE;
    لكل(int32_t prev_بازه_حد=0; prev_بازه_حد<s.length();) {
        int32_t بازه_حد=مجموعه.بازه(s, prev_بازه_حد, بازه_Condition);
        إذا(بازه_Condition==USET_بازه_NOT_CONTAINED) {
            بازه_Condition=USET_بازه_SIMPLE;
        } وإلا {
            UNormalization_Check_نتیجه qc_نتیجه=
                norm2.سریع_Check(s.موقت_Sub_String_Between(prev_بازه_حد, بازه_حد), error_کد);
            إذا(U_FAILURE(error_کد) || qc_نتیجه==UNORM_NO) {
                أرجع qc_نتیجه;
            } وإلا إذا(qc_نتیجه==UNORM_MAYBE) {
                نتیجه=qc_نتیجه;
            }
            بازه_Condition=USET_بازه_NOT_CONTAINED;
        }
        prev_بازه_حد=بازه_حد;
    }
    أرجع نتیجه;
}

int32_t
FilteredNormalizer2::بازه_سریع_Check_Yes(ثابت UnicodeString &s, UError_کد &error_کد) ثابت {
    uprv_check_Can_Get_میانگیر(s, error_کد);
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    USet_بازه_Condition بازه_Condition=USET_بازه_SIMPLE;
    لكل(int32_t prev_بازه_حد=0; prev_بازه_حد<s.length();) {
        int32_t بازه_حد=مجموعه.بازه(s, prev_بازه_حد, بازه_Condition);
        إذا(بازه_Condition==USET_بازه_NOT_CONTAINED) {
            بازه_Condition=USET_بازه_SIMPLE;
        } وإلا {
            int32_t yes_حد=
                prev_بازه_حد+
                norm2.بازه_سریع_Check_Yes(
                    s.موقت_Sub_String_Between(prev_بازه_حد, بازه_حد), error_کد);
            إذا(U_FAILURE(error_کد) || yes_حد<بازه_حد) {
                أرجع yes_حد;
            }
            بازه_Condition=USET_بازه_NOT_CONTAINED;
        }
        prev_بازه_حد=بازه_حد;
    }
    أرجع s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ثابت {
    أرجع !مجموعه.شامل_است(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ثابت {
    أرجع !مجموعه.شامل_است(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ثابت {
    أرجع !مجموعه.شامل_است(c) || norm2.isInert(c);
}

U_NAMESPACE_انتها

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ثابت UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ثابت USet* filter_مجموعه U_LIFETIME_BOUND,
                    UError_کد* p_Error_کد) {
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا(filter_مجموعه==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    Normalizer2 *fn2=جديد FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *Unicode_مجموعه::fromUSet(filter_مجموعه));
    إذا(fn2==اشاره_تهی) {
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
    }
    أرجع (UNormalizer2 *)fn2;
}

#پایان_اگر  // !UCONFIG_NO_NORMALIZATION
