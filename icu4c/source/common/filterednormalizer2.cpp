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
                               UError_ڪوڊ& error_ڪوڊ) ثابت {
    uprv_check_Can_Get_بفر(src, error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        dest.سيٽ_منزل_Bogus();
        أرجع dest;
    }
    إذا(&dest==&src) {
        error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع dest;
    }
    dest.remove();
    أرجع normalize(src, dest, USET_وسعت_SIMPLE, error_ڪوڊ);
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
                               UError_ڪوڊ& error_ڪوڊ) ثابت {
    UnicodeString عارضي_Dest;  // Don't throw away destination buffer between iterations.
    لكل(int32_t prev_وسعت_حد_بندي=0; prev_وسعت_حد_بندي<src.length();) {
        int32_t وسعت_حد_بندي=سيٽ.وسعت(src, prev_وسعت_حد_بندي, وسعت_Condition);
        int32_t وسعت_ڊيگهائي=وسعت_حد_بندي-prev_وسعت_حد_بندي;
        إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
            إذا(وسعت_ڊيگهائي!=0) {
                dest.append(src, prev_وسعت_حد_بندي, وسعت_ڊيگهائي);
            }
            وسعت_Condition=USET_وسعت_SIMPLE;
        } وإلا {
            إذا(وسعت_ڊيگهائي!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.عارضي_Sub_String_Between(prev_وسعت_حد_بندي, وسعت_حد_بندي),
                                            عارضي_Dest, error_ڪوڊ));
                إذا(U_FAILURE(error_ڪوڊ)) {
                    اكسر;
                }
            }
            وسعت_Condition=USET_وسعت_NOT_CONTAINED;
        }
        prev_وسعت_حد_بندي=وسعت_حد_بندي;
    }
    أرجع dest;
}

فراغ
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    إذا (edits != خالي_اشارو && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.معلومات(), src.length(), sink, edits, USET_وسعت_SIMPLE, error_ڪوڊ);
}

فراغ
FilteredNormalizer2::normalizeUTF8(uint32_t options, ثابت حرف *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USet_وسعت_Condition وسعت_Condition,
                                   UError_ڪوڊ &error_ڪوڊ) ثابت {
    طالما (length > 0) {
        int32_t وسعت_ڊيگهائي = سيٽ.وسعت_UTF8(src, length, وسعت_Condition);
        إذا (وسعت_Condition == USET_وسعت_NOT_CONTAINED) {
            إذا (وسعت_ڊيگهائي != 0) {
                إذا (edits != خالي_اشارو) {
                    edits->جوڙيو_Unchanged(وسعت_ڊيگهائي);
                }
                إذا ((options & U_OMIT_UNCHANGED_متن) == 0) {
                    sink.Append(src, وسعت_ڊيگهائي);
                }
            }
            وسعت_Condition = USET_وسعت_SIMPLE;
        } وإلا {
            إذا (وسعت_ڊيگهائي != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, وسعت_ڊيگهائي), sink, edits, error_ڪوڊ);
                إذا (U_FAILURE(error_ڪوڊ)) {
                    اكسر;
                }
            }
            وسعت_Condition = USET_وسعت_NOT_CONTAINED;
        }
        src += وسعت_ڊيگهائي;
        length -= وسعت_ڊيگهائي;
    }
}

UnicodeString&
FilteredNormalizer2::normalize_ٻيو_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ثابت UnicodeString& ٻيو,
                                              UError_ڪوڊ& error_ڪوڊ) ثابت {
    أرجع normalize_ٻيو_And_Append(first, ٻيو, سچ, error_ڪوڊ);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ثابت UnicodeString& ٻيو,
                            UError_ڪوڊ& error_ڪوڊ) ثابت {
    أرجع normalize_ٻيو_And_Append(first, ٻيو, خطأ, error_ڪوڊ);
}

UnicodeString&
FilteredNormalizer2::normalize_ٻيو_And_Append(UnicodeString& first U_LIFETIME_BOUND,
                                              ثابت UnicodeString& ٻيو,
                                              UBool doNormalize,
                                              UError_ڪوڊ& error_ڪوڊ) ثابت {
    uprv_check_Can_Get_بفر(first, error_ڪوڊ);
    uprv_check_Can_Get_بفر(ٻيو, error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع first;
    }
    إذا(&first==&ٻيو) {
        error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع first;
    }
    إذا(first.is_خالي_جايون()) {
        إذا(doNormalize) {
            أرجع normalize(ٻيو, first, error_ڪوڊ);
        } وإلا {
            أرجع first=ٻيو;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t اڳياڙي_حد_بندي=سيٽ.وسعت(ٻيو, 0, USET_وسعت_SIMPLE);
    إذا(اڳياڙي_حد_بندي!=0) {
        UnicodeString اڳياڙي(ٻيو.عارضي_Sub_String(0, اڳياڙي_حد_بندي));
        int32_t suffix_شروعات=سيٽ.وسعت_پٺ(first, INT32_MAX, USET_وسعت_SIMPLE);
        إذا(suffix_شروعات==0) {
            إذا(doNormalize) {
                norm2.normalize_ٻيو_And_Append(first, اڳياڙي, error_ڪوڊ);
            } وإلا {
                norm2.append(first, اڳياڙي, error_ڪوڊ);
            }
        } وإلا {
            UnicodeString وچ(first, suffix_شروعات, INT32_MAX);
            إذا(doNormalize) {
                norm2.normalize_ٻيو_And_Append(وچ, اڳياڙي, error_ڪوڊ);
            } وإلا {
                norm2.append(وچ, اڳياڙي, error_ڪوڊ);
            }
            first.replace(suffix_شروعات, INT32_MAX, وچ);
        }
    }
    إذا(اڳياڙي_حد_بندي<ٻيو.length()) {
        UnicodeString rest(ٻيو.عارضي_Sub_String(اڳياڙي_حد_بندي, INT32_MAX));
        إذا(doNormalize) {
            normalize(rest, first, USET_وسعت_NOT_CONTAINED, error_ڪوڊ);
        } وإلا {
            first.append(rest);
        }
    }
    أرجع first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    أرجع سيٽ.شامل_آهي(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت {
    أرجع سيٽ.شامل_آهي(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ثابت {
    أرجع (سيٽ.شامل_آهي(a) && سيٽ.شامل_آهي(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ثابت {
    أرجع سيٽ.شامل_آهي(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ثابت UnicodeString &s, UError_ڪوڊ &error_ڪوڊ) ثابت {
    uprv_check_Can_Get_بفر(s, error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خطأ;
    }
    USet_وسعت_Condition وسعت_Condition=USET_وسعت_SIMPLE;
    لكل(int32_t prev_وسعت_حد_بندي=0; prev_وسعت_حد_بندي<s.length();) {
        int32_t وسعت_حد_بندي=سيٽ.وسعت(s, prev_وسعت_حد_بندي, وسعت_Condition);
        إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
            وسعت_Condition=USET_وسعت_SIMPLE;
        } وإلا {
            إذا( !norm2.isNormalized(s.عارضي_Sub_String_Between(prev_وسعت_حد_بندي, وسعت_حد_بندي), error_ڪوڊ) ||
                U_FAILURE(error_ڪوڊ)
            ) {
                أرجع خطأ;
            }
            وسعت_Condition=USET_وسعت_NOT_CONTAINED;
        }
        prev_وسعت_حد_بندي=وسعت_حد_بندي;
    }
    أرجع سچ;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خطأ;
    }
    ثابت حرف *s = sp.معلومات();
    int32_t length = sp.length();
    USet_وسعت_Condition وسعت_Condition = USET_وسعت_SIMPLE;
    طالما (length > 0) {
        int32_t وسعت_ڊيگهائي = سيٽ.وسعت_UTF8(s, length, وسعت_Condition);
        إذا (وسعت_Condition == USET_وسعت_NOT_CONTAINED) {
            وسعت_Condition = USET_وسعت_SIMPLE;
        } وإلا {
            إذا (!norm2.isNormalizedUTF8(StringPiece(s, وسعت_ڊيگهائي), error_ڪوڊ) ||
                    U_FAILURE(error_ڪوڊ)) {
                أرجع خطأ;
            }
            وسعت_Condition = USET_وسعت_NOT_CONTAINED;
        }
        s += وسعت_ڊيگهائي;
        length -= وسعت_ڊيگهائي;
    }
    أرجع سچ;
}

UNormalization_Check_نتيجو
FilteredNormalizer2::تيز_Check(ثابت UnicodeString &s, UError_ڪوڊ &error_ڪوڊ) ثابت {
    uprv_check_Can_Get_بفر(s, error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع UNORM_MAYBE;
    }
    UNormalization_Check_نتيجو نتيجو=UNORM_YES;
    USet_وسعت_Condition وسعت_Condition=USET_وسعت_SIMPLE;
    لكل(int32_t prev_وسعت_حد_بندي=0; prev_وسعت_حد_بندي<s.length();) {
        int32_t وسعت_حد_بندي=سيٽ.وسعت(s, prev_وسعت_حد_بندي, وسعت_Condition);
        إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
            وسعت_Condition=USET_وسعت_SIMPLE;
        } وإلا {
            UNormalization_Check_نتيجو qc_نتيجو=
                norm2.تيز_Check(s.عارضي_Sub_String_Between(prev_وسعت_حد_بندي, وسعت_حد_بندي), error_ڪوڊ);
            إذا(U_FAILURE(error_ڪوڊ) || qc_نتيجو==UNORM_NO) {
                أرجع qc_نتيجو;
            } وإلا إذا(qc_نتيجو==UNORM_MAYBE) {
                نتيجو=qc_نتيجو;
            }
            وسعت_Condition=USET_وسعت_NOT_CONTAINED;
        }
        prev_وسعت_حد_بندي=وسعت_حد_بندي;
    }
    أرجع نتيجو;
}

int32_t
FilteredNormalizer2::وسعت_تيز_Check_Yes(ثابت UnicodeString &s, UError_ڪوڊ &error_ڪوڊ) ثابت {
    uprv_check_Can_Get_بفر(s, error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    USet_وسعت_Condition وسعت_Condition=USET_وسعت_SIMPLE;
    لكل(int32_t prev_وسعت_حد_بندي=0; prev_وسعت_حد_بندي<s.length();) {
        int32_t وسعت_حد_بندي=سيٽ.وسعت(s, prev_وسعت_حد_بندي, وسعت_Condition);
        إذا(وسعت_Condition==USET_وسعت_NOT_CONTAINED) {
            وسعت_Condition=USET_وسعت_SIMPLE;
        } وإلا {
            int32_t yes_حد_بندي=
                prev_وسعت_حد_بندي+
                norm2.وسعت_تيز_Check_Yes(
                    s.عارضي_Sub_String_Between(prev_وسعت_حد_بندي, وسعت_حد_بندي), error_ڪوڊ);
            إذا(U_FAILURE(error_ڪوڊ) || yes_حد_بندي<وسعت_حد_بندي) {
                أرجع yes_حد_بندي;
            }
            وسعت_Condition=USET_وسعت_NOT_CONTAINED;
        }
        prev_وسعت_حد_بندي=وسعت_حد_بندي;
    }
    أرجع s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ثابت {
    أرجع !سيٽ.شامل_آهي(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ثابت {
    أرجع !سيٽ.شامل_آهي(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ثابت {
    أرجع !سيٽ.شامل_آهي(c) || norm2.isInert(c);
}

U_NAMESPACE_پڇاڙي

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ثابت UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ثابت USet* filter_سيٽ U_LIFETIME_BOUND,
                    UError_ڪوڊ* p_Error_ڪوڊ) {
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا(filter_سيٽ==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    Normalizer2 *fn2=جديد FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *Unicode_سيٽ::fromUSet(filter_سيٽ));
    إذا(fn2==خالي_اشارو) {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
    }
    أرجع (UNormalizer2 *)fn2;
}

#جيڪڏهن_پورو  // !UCONFIG_NO_NORMALIZATION
