// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014, International Business Machines
* Corporation and others.  All Rights Reserved.
*******************************************************************************
* norm2allmodes.h
*
* created on: 2014sep07
* created by: Markus W. Scherer
*/

#جيڪڏهن_وصف_نه_ٿيل __NORM2ALLMODES_H__
#وصف_ڪريو __NORM2ALLMODES_H__

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_NORMALIZATION

#ضمن "unicode/edits.h"
#ضمن "unicode/normalizer2.h"
#ضمن "unicode/stringoptions.h"
#ضمن "unicode/unistr.h"
#ضمن "cpputils.h"
#ضمن "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : عام Normalizer2 {
عام:
    Normalizer2WithImpl(ثابت Normalizer2Impl &ni) : impl(ni) {}
    مجازي ~Normalizer2WithImpl();

    // normalize
    مجازي UnicodeString&
    normalize(ثابت UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UError_ڪوڊ& error_ڪوڊ) ثابت override {
        إذا(U_FAILURE(error_ڪوڊ)) {
            dest.سيٽ_منزل_Bogus();
            أرجع dest;
        }
        ثابت char16_t *sArray=src.get_بفر();
        إذا(&dest==&src || sArray==nullptr) {
            error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
            dest.سيٽ_منزل_Bogus();
            أرجع dest;
        }
        dest.remove();
        Reordering_بفر بفر(impl, dest);
        إذا(بفر.init(src.ڊيگهائي(), error_ڪوڊ)) {
            normalize(sArray, sArray+src.ڊيگهائي(), بفر, error_ڪوڊ);
        }
        أرجع dest;
    }
    مجازي فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *حد_بندي,
              Reordering_بفر &بفر, UError_ڪوڊ &error_ڪوڊ) ثابت = 0;

    // normalize and append
    مجازي UnicodeString&
    normalize_ٻيو_And_Append(UnicodeString& پهريون U_LIFETIME_BOUND,
                             ثابت UnicodeString& ٻيو,
                             UError_ڪوڊ& error_ڪوڊ) ثابت override {
        أرجع normalize_ٻيو_And_Append(پهريون, ٻيو, true, error_ڪوڊ);
    }
    مجازي UnicodeString&
    append(UnicodeString& پهريون U_LIFETIME_BOUND,
           ثابت UnicodeString& ٻيو,
           UError_ڪوڊ& error_ڪوڊ) ثابت override {
        أرجع normalize_ٻيو_And_Append(پهريون, ٻيو, false, error_ڪوڊ);
    }
    UnicodeString&
    normalize_ٻيو_And_Append(UnicodeString& پهريون U_LIFETIME_BOUND,
                             ثابت UnicodeString& ٻيو,
                             UBool doNormalize,
                             UError_ڪوڊ& error_ڪوڊ) ثابت {
        uprv_check_Can_Get_بفر(پهريون, error_ڪوڊ);
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع پهريون;
        }
        ثابت char16_t *ٻيو_Array=ٻيو.get_بفر();
        إذا(&پهريون==&ٻيو || ٻيو_Array==nullptr) {
            error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
            أرجع پهريون;
        }
        int32_t پهريون_ڊيگهائي=پهريون.ڊيگهائي();
        UnicodeString محفوظ_وچ;
        {
            Reordering_بفر بفر(impl, پهريون);
            إذا(بفر.init(پهريون_ڊيگهائي+ٻيو.ڊيگهائي(), error_ڪوڊ)) {
                normalizeAndAppend(ٻيو_Array, ٻيو_Array+ٻيو.ڊيگهائي(), doNormalize,
                                   محفوظ_وچ, بفر, error_ڪوڊ);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        إذا(U_FAILURE(error_ڪوڊ)) {
            // Restore the modified suffix of the first string.
            پهريون.replace(پهريون_ڊيگهائي-محفوظ_وچ.ڊيگهائي(), 0x7fffffff, محفوظ_وچ);
        }
        أرجع پهريون;
    }
    مجازي فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندي, UBool doNormalize,
                       UnicodeString &محفوظ_وچ,
                       Reordering_بفر &بفر, UError_ڪوڊ &error_ڪوڊ) ثابت = 0;
    مجازي UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t بفر[4];
        int32_t ڊيگهائي;
        ثابت char16_t *d=impl.getDecomposition(c, بفر, ڊيگهائي);
        إذا(d==nullptr) {
            أرجع false;
        }
        إذا(d==بفر) {
            decomposition.سيٽ_منزل(بفر, ڊيگهائي);  // copy the string (Jamos from Hangul syllable c)
        } وإلا {
            decomposition.سيٽ_منزل(false, d, ڊيگهائي);  // read-only alias
        }
        أرجع true;
    }
    مجازي UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t بفر[30];
        int32_t ڊيگهائي;
        ثابت char16_t *d=impl.getRawDecomposition(c, بفر, ڊيگهائي);
        إذا(d==nullptr) {
            أرجع false;
        }
        إذا(d==بفر) {
            decomposition.سيٽ_منزل(بفر, ڊيگهائي);  // copy the string (algorithmic decomposition)
        } وإلا {
            decomposition.سيٽ_منزل(false, d, ڊيگهائي);  // read-only alias
        }
        أرجع true;
    }
    مجازي UChar32
    composePair(UChar32 a, UChar32 b) ثابت override {
        أرجع impl.composePair(a, b);
    }

    مجازي uint8_t
    getCombiningClass(UChar32 c) ثابت override {
        أرجع impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    مجازي UBool
    isNormalized(ثابت UnicodeString &s, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع false;
        }
        ثابت char16_t *sArray=s.get_بفر();
        إذا(sArray==nullptr) {
            error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
            أرجع false;
        }
        ثابت char16_t *s_حد_بندي=sArray+s.ڊيگهائي();
        أرجع s_حد_بندي==وسعت_تيز_Check_Yes(sArray, s_حد_بندي, error_ڪوڊ);
    }
    مجازي UNormalization_Check_نتيجو
    تيز_Check(ثابت UnicodeString &s, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        أرجع Normalizer2WithImpl::isNormalized(s, error_ڪوڊ) ? UNORM_YES : UNORM_NO;
    }
    مجازي int32_t
    وسعت_تيز_Check_Yes(ثابت UnicodeString &s, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع 0;
        }
        ثابت char16_t *sArray=s.get_بفر();
        إذا(sArray==nullptr) {
            error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
            أرجع 0;
        }
        أرجع static_cast<int32_t>(وسعت_تيز_Check_Yes(sArray, sArray + s.ڊيگهائي(), error_ڪوڊ) - sArray);
    }
    مجازي ثابت char16_t*
    وسعت_تيز_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد_بندي,
                      UError_ڪوڊ& error_ڪوڊ) ثابت = 0;

    مجازي UNormalization_Check_نتيجو get_تيز_Check(UChar32) ثابت {
        أرجع UNORM_YES;
    }

    ثابت Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : عام Normalizer2WithImpl {
عام:
    DecomposeNormalizer2(ثابت Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    مجازي ~DecomposeNormalizer2();

خاص:
    مجازي فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *حد_بندي,
              Reordering_بفر &بفر, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        impl.decompose(src, حد_بندي, &بفر, error_ڪوڊ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    مجازي فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندي, UBool doNormalize,
                       UnicodeString &محفوظ_وچ,
                       Reordering_بفر &بفر, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        impl.decomposeAndAppend(src, حد_بندي, doNormalize, محفوظ_وچ, بفر, error_ڪوڊ);
    }

    فراغ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        إذا (U_FAILURE(error_ڪوڊ)) {
            أرجع;
        }
        إذا (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.معلومات());
        impl.decomposeUTF8(options, s, s + src.ڊيگهائي(), &sink, edits, error_ڪوڊ);
        sink.Flush();
    }
    مجازي UBool
    isNormalizedUTF8(StringPiece sp, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.معلومات());
        ثابت uint8_t *s_حد_بندي = s + sp.ڊيگهائي();
        أرجع s_حد_بندي == impl.decomposeUTF8(0, s, s_حد_بندي, nullptr, nullptr, error_ڪوڊ);
    }

    مجازي ثابت char16_t*
    وسعت_تيز_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد_بندي,
                      UError_ڪوڊ& error_ڪوڊ) ثابت override {
        أرجع impl.decompose(src, حد_بندي, nullptr, error_ڪوڊ);
    }
    using Normalizer2WithImpl::وسعت_تيز_Check_Yes;  // Avoid warning about hiding base class function.
    مجازي UNormalization_Check_نتيجو get_تيز_Check(UChar32 c) ثابت override {
        أرجع impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    مجازي UBool hasBoundaryBefore(UChar32 c) ثابت override {
        أرجع impl.hasDecompBoundaryBefore(c);
    }
    مجازي UBool hasBoundaryAfter(UChar32 c) ثابت override {
        أرجع impl.hasDecompBoundaryAfter(c);
    }
    مجازي UBool isInert(UChar32 c) ثابت override {
        أرجع impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : عام Normalizer2WithImpl {
عام:
    ComposeNormalizer2(ثابت Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    مجازي ~ComposeNormalizer2();

خاص:
    مجازي فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *حد_بندي,
              Reordering_بفر &بفر, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        impl.compose(src, حد_بندي, onlyContiguous, true, بفر, error_ڪوڊ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    فراغ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        إذا (U_FAILURE(error_ڪوڊ)) {
            أرجع;
        }
        إذا (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.معلومات());
        impl.composeUTF8(options, onlyContiguous, s, s + src.ڊيگهائي(),
                         &sink, edits, error_ڪوڊ);
        sink.Flush();
    }

    مجازي فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندي, UBool doNormalize,
                       UnicodeString &محفوظ_وچ,
                       Reordering_بفر &بفر, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        impl.composeAndAppend(src, حد_بندي, doNormalize, onlyContiguous, محفوظ_وچ, بفر, error_ڪوڊ);
    }

    مجازي UBool
    isNormalized(ثابت UnicodeString &s, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع false;
        }
        ثابت char16_t *sArray=s.get_بفر();
        إذا(sArray==nullptr) {
            error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
            أرجع false;
        }
        UnicodeString عارضي;
        Reordering_بفر بفر(impl, عارضي);
        إذا(!بفر.init(5, error_ڪوڊ)) {  // small destCapacity for substring normalization
            أرجع false;
        }
        أرجع impl.compose(sArray, sArray+s.ڊيگهائي(), onlyContiguous, false, بفر, error_ڪوڊ);
    }
    مجازي UBool
    isNormalizedUTF8(StringPiece sp, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.معلومات());
        أرجع impl.composeUTF8(0, onlyContiguous, s, s + sp.ڊيگهائي(), nullptr, nullptr, error_ڪوڊ);
    }
    مجازي UNormalization_Check_نتيجو
    تيز_Check(ثابت UnicodeString &s, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع UNORM_MAYBE;
        }
        ثابت char16_t *sArray=s.get_بفر();
        إذا(sArray==nullptr) {
            error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
            أرجع UNORM_MAYBE;
        }
        UNormalization_Check_نتيجو qc_نتيجو=UNORM_YES;
        impl.compose_تيز_Check(sArray, sArray+s.ڊيگهائي(), onlyContiguous, &qc_نتيجو);
        أرجع qc_نتيجو;
    }
    مجازي ثابت char16_t*
    وسعت_تيز_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد_بندي,
                      UError_ڪوڊ&) ثابت override {
        أرجع impl.compose_تيز_Check(src, حد_بندي, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::وسعت_تيز_Check_Yes;  // Avoid warning about hiding base class function.
    مجازي UNormalization_Check_نتيجو get_تيز_Check(UChar32 c) ثابت override {
        أرجع impl.get_Comp_تيز_Check(impl.getNorm16(c));
    }
    مجازي UBool hasBoundaryBefore(UChar32 c) ثابت override {
        أرجع impl.hasCompBoundaryBefore(c);
    }
    مجازي UBool hasBoundaryAfter(UChar32 c) ثابت override {
        أرجع impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    مجازي UBool isInert(UChar32 c) ثابت override {
        أرجع impl.isCompInert(c, onlyContiguous);
    }

    ثابت UBool onlyContiguous;
};

class FCDNormalizer2 : عام Normalizer2WithImpl {
عام:
    FCDNormalizer2(ثابت Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    مجازي ~FCDNormalizer2();

خاص:
    مجازي فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *حد_بندي,
              Reordering_بفر &بفر, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        impl.makeFCD(src, حد_بندي, &بفر, error_ڪوڊ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    مجازي فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندي, UBool doNormalize,
                       UnicodeString &محفوظ_وچ,
                       Reordering_بفر &بفر, UError_ڪوڊ &error_ڪوڊ) ثابت override {
        impl.makeFCDAndAppend(src, حد_بندي, doNormalize, محفوظ_وچ, بفر, error_ڪوڊ);
    }
    مجازي ثابت char16_t*
    وسعت_تيز_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد_بندي,
                      UError_ڪوڊ& error_ڪوڊ) ثابت override {
        أرجع impl.makeFCD(src, حد_بندي, nullptr, error_ڪوڊ);
    }
    using Normalizer2WithImpl::وسعت_تيز_Check_Yes;  // Avoid warning about hiding base class function.
    مجازي UBool hasBoundaryBefore(UChar32 c) ثابت override {
        أرجع impl.hasFCDBoundaryBefore(c);
    }
    مجازي UBool hasBoundaryAfter(UChar32 c) ثابت override {
        أرجع impl.hasFCDBoundaryAfter(c);
    }
    مجازي UBool isInert(UChar32 c) ثابت override {
        أرجع impl.isFCDInert(c);
    }
};

بنية Norm2AllModes : عام UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    ساكن Norm2AllModes *createInstance(Normalizer2Impl *impl, UError_ڪوڊ &error_ڪوڊ);
    ساكن Norm2AllModes *createNFCInstance(UError_ڪوڊ &error_ڪوڊ);
    ساكن Norm2AllModes *createInstance(ثابت حرف *packageName,
                                         ثابت حرف *name,
                                         UError_ڪوڊ &error_ڪوڊ);

    ساكن ثابت Norm2AllModes *getNFCInstance(UError_ڪوڊ &error_ڪوڊ);
    ساكن ثابت Norm2AllModes *getNFKCInstance(UError_ڪوڊ &error_ڪوڊ);
    ساكن ثابت Norm2AllModes *getNFKC_CFInstance(UError_ڪوڊ &error_ڪوڊ);
    ساكن ثابت Norm2AllModes *getNFKC_SCFInstance(UError_ڪوڊ &error_ڪوڊ);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو  // !UCONFIG_NO_NORMALIZATION
#جيڪڏهن_پورو  // __NORM2ALLMODES_H__
