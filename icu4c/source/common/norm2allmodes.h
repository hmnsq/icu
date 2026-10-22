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

#جے_بنا_تعریف __NORM2ALLMODES_H__
#تعریف_کرو __NORM2ALLMODES_H__

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
    مجازی ~Normalizer2WithImpl();

    // normalize
    مجازی UnicodeString&
    normalize(ثابت UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UError_کوڈ& error_کوڈ) ثابت override {
        إذا(U_FAILURE(error_کوڈ)) {
            dest.سیٹ_منزل_Bogus();
            أرجع dest;
        }
        ثابت char16_t *sArray=src.get_بفر();
        إذا(&dest==&src || sArray==nullptr) {
            error_کوڈ=U_ILLEGAL_دلیل_ERROR;
            dest.سیٹ_منزل_Bogus();
            أرجع dest;
        }
        dest.remove();
        Reordering_بفر بفر(impl, dest);
        إذا(بفر.init(src.لمائی(), error_کوڈ)) {
            normalize(sArray, sArray+src.لمائی(), بفر, error_کوڈ);
        }
        أرجع dest;
    }
    مجازی فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *حد_بندی,
              Reordering_بفر &بفر, UError_کوڈ &error_کوڈ) ثابت = 0;

    // normalize and append
    مجازی UnicodeString&
    normalize_دوجا_And_Append(UnicodeString& پہلا U_LIFETIME_BOUND,
                             ثابت UnicodeString& دوجا,
                             UError_کوڈ& error_کوڈ) ثابت override {
        أرجع normalize_دوجا_And_Append(پہلا, دوجا, true, error_کوڈ);
    }
    مجازی UnicodeString&
    append(UnicodeString& پہلا U_LIFETIME_BOUND,
           ثابت UnicodeString& دوجا,
           UError_کوڈ& error_کوڈ) ثابت override {
        أرجع normalize_دوجا_And_Append(پہلا, دوجا, false, error_کوڈ);
    }
    UnicodeString&
    normalize_دوجا_And_Append(UnicodeString& پہلا U_LIFETIME_BOUND,
                             ثابت UnicodeString& دوجا,
                             UBool doNormalize,
                             UError_کوڈ& error_کوڈ) ثابت {
        uprv_check_Can_Get_بفر(پہلا, error_کوڈ);
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع پہلا;
        }
        ثابت char16_t *دوجا_Array=دوجا.get_بفر();
        إذا(&پہلا==&دوجا || دوجا_Array==nullptr) {
            error_کوڈ=U_ILLEGAL_دلیل_ERROR;
            أرجع پہلا;
        }
        int32_t پہلا_لمائی=پہلا.لمائی();
        UnicodeString محفوظ_درمیان;
        {
            Reordering_بفر بفر(impl, پہلا);
            إذا(بفر.init(پہلا_لمائی+دوجا.لمائی(), error_کوڈ)) {
                normalizeAndAppend(دوجا_Array, دوجا_Array+دوجا.لمائی(), doNormalize,
                                   محفوظ_درمیان, بفر, error_کوڈ);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        إذا(U_FAILURE(error_کوڈ)) {
            // Restore the modified suffix of the first string.
            پہلا.replace(پہلا_لمائی-محفوظ_درمیان.لمائی(), 0x7fffffff, محفوظ_درمیان);
        }
        أرجع پہلا;
    }
    مجازی فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندی, UBool doNormalize,
                       UnicodeString &محفوظ_درمیان,
                       Reordering_بفر &بفر, UError_کوڈ &error_کوڈ) ثابت = 0;
    مجازی UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t بفر[4];
        int32_t لمائی;
        ثابت char16_t *d=impl.getDecomposition(c, بفر, لمائی);
        إذا(d==nullptr) {
            أرجع false;
        }
        إذا(d==بفر) {
            decomposition.سیٹ_منزل(بفر, لمائی);  // copy the string (Jamos from Hangul syllable c)
        } وإلا {
            decomposition.سیٹ_منزل(false, d, لمائی);  // read-only alias
        }
        أرجع true;
    }
    مجازی UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t بفر[30];
        int32_t لمائی;
        ثابت char16_t *d=impl.getRawDecomposition(c, بفر, لمائی);
        إذا(d==nullptr) {
            أرجع false;
        }
        إذا(d==بفر) {
            decomposition.سیٹ_منزل(بفر, لمائی);  // copy the string (algorithmic decomposition)
        } وإلا {
            decomposition.سیٹ_منزل(false, d, لمائی);  // read-only alias
        }
        أرجع true;
    }
    مجازی UChar32
    composePair(UChar32 a, UChar32 b) ثابت override {
        أرجع impl.composePair(a, b);
    }

    مجازی uint8_t
    getCombiningClass(UChar32 c) ثابت override {
        أرجع impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    مجازی UBool
    isNormalized(ثابت UnicodeString &s, UError_کوڈ &error_کوڈ) ثابت override {
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع false;
        }
        ثابت char16_t *sArray=s.get_بفر();
        إذا(sArray==nullptr) {
            error_کوڈ=U_ILLEGAL_دلیل_ERROR;
            أرجع false;
        }
        ثابت char16_t *s_حد_بندی=sArray+s.لمائی();
        أرجع s_حد_بندی==وسعت_تیز_Check_Yes(sArray, s_حد_بندی, error_کوڈ);
    }
    مجازی UNormalization_Check_نتیجہ
    تیز_Check(ثابت UnicodeString &s, UError_کوڈ &error_کوڈ) ثابت override {
        أرجع Normalizer2WithImpl::isNormalized(s, error_کوڈ) ? UNORM_YES : UNORM_NO;
    }
    مجازی int32_t
    وسعت_تیز_Check_Yes(ثابت UnicodeString &s, UError_کوڈ &error_کوڈ) ثابت override {
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع 0;
        }
        ثابت char16_t *sArray=s.get_بفر();
        إذا(sArray==nullptr) {
            error_کوڈ=U_ILLEGAL_دلیل_ERROR;
            أرجع 0;
        }
        أرجع static_cast<int32_t>(وسعت_تیز_Check_Yes(sArray, sArray + s.لمائی(), error_کوڈ) - sArray);
    }
    مجازی ثابت char16_t*
    وسعت_تیز_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد_بندی,
                      UError_کوڈ& error_کوڈ) ثابت = 0;

    مجازی UNormalization_Check_نتیجہ get_تیز_Check(UChar32) ثابت {
        أرجع UNORM_YES;
    }

    ثابت Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : عام Normalizer2WithImpl {
عام:
    DecomposeNormalizer2(ثابت Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    مجازی ~DecomposeNormalizer2();

خاص:
    مجازی فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *حد_بندی,
              Reordering_بفر &بفر, UError_کوڈ &error_کوڈ) ثابت override {
        impl.decompose(src, حد_بندی, &بفر, error_کوڈ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    مجازی فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندی, UBool doNormalize,
                       UnicodeString &محفوظ_درمیان,
                       Reordering_بفر &بفر, UError_کوڈ &error_کوڈ) ثابت override {
        impl.decomposeAndAppend(src, حد_بندی, doNormalize, محفوظ_درمیان, بفر, error_کوڈ);
    }

    فراغ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_کوڈ &error_کوڈ) ثابت override {
        إذا (U_FAILURE(error_کوڈ)) {
            أرجع;
        }
        إذا (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.انکڑے());
        impl.decomposeUTF8(options, s, s + src.لمائی(), &sink, edits, error_کوڈ);
        sink.Flush();
    }
    مجازی UBool
    isNormalizedUTF8(StringPiece sp, UError_کوڈ &error_کوڈ) ثابت override {
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.انکڑے());
        ثابت uint8_t *s_حد_بندی = s + sp.لمائی();
        أرجع s_حد_بندی == impl.decomposeUTF8(0, s, s_حد_بندی, nullptr, nullptr, error_کوڈ);
    }

    مجازی ثابت char16_t*
    وسعت_تیز_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد_بندی,
                      UError_کوڈ& error_کوڈ) ثابت override {
        أرجع impl.decompose(src, حد_بندی, nullptr, error_کوڈ);
    }
    using Normalizer2WithImpl::وسعت_تیز_Check_Yes;  // Avoid warning about hiding base class function.
    مجازی UNormalization_Check_نتیجہ get_تیز_Check(UChar32 c) ثابت override {
        أرجع impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    مجازی UBool hasBoundaryBefore(UChar32 c) ثابت override {
        أرجع impl.hasDecompBoundaryBefore(c);
    }
    مجازی UBool hasBoundaryAfter(UChar32 c) ثابت override {
        أرجع impl.hasDecompBoundaryAfter(c);
    }
    مجازی UBool isInert(UChar32 c) ثابت override {
        أرجع impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : عام Normalizer2WithImpl {
عام:
    ComposeNormalizer2(ثابت Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    مجازی ~ComposeNormalizer2();

خاص:
    مجازی فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *حد_بندی,
              Reordering_بفر &بفر, UError_کوڈ &error_کوڈ) ثابت override {
        impl.compose(src, حد_بندی, onlyContiguous, true, بفر, error_کوڈ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    فراغ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_کوڈ &error_کوڈ) ثابت override {
        إذا (U_FAILURE(error_کوڈ)) {
            أرجع;
        }
        إذا (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.انکڑے());
        impl.composeUTF8(options, onlyContiguous, s, s + src.لمائی(),
                         &sink, edits, error_کوڈ);
        sink.Flush();
    }

    مجازی فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندی, UBool doNormalize,
                       UnicodeString &محفوظ_درمیان,
                       Reordering_بفر &بفر, UError_کوڈ &error_کوڈ) ثابت override {
        impl.composeAndAppend(src, حد_بندی, doNormalize, onlyContiguous, محفوظ_درمیان, بفر, error_کوڈ);
    }

    مجازی UBool
    isNormalized(ثابت UnicodeString &s, UError_کوڈ &error_کوڈ) ثابت override {
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع false;
        }
        ثابت char16_t *sArray=s.get_بفر();
        إذا(sArray==nullptr) {
            error_کوڈ=U_ILLEGAL_دلیل_ERROR;
            أرجع false;
        }
        UnicodeString عارضی;
        Reordering_بفر بفر(impl, عارضی);
        إذا(!بفر.init(5, error_کوڈ)) {  // small destCapacity for substring normalization
            أرجع false;
        }
        أرجع impl.compose(sArray, sArray+s.لمائی(), onlyContiguous, false, بفر, error_کوڈ);
    }
    مجازی UBool
    isNormalizedUTF8(StringPiece sp, UError_کوڈ &error_کوڈ) ثابت override {
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.انکڑے());
        أرجع impl.composeUTF8(0, onlyContiguous, s, s + sp.لمائی(), nullptr, nullptr, error_کوڈ);
    }
    مجازی UNormalization_Check_نتیجہ
    تیز_Check(ثابت UnicodeString &s, UError_کوڈ &error_کوڈ) ثابت override {
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع UNORM_MAYBE;
        }
        ثابت char16_t *sArray=s.get_بفر();
        إذا(sArray==nullptr) {
            error_کوڈ=U_ILLEGAL_دلیل_ERROR;
            أرجع UNORM_MAYBE;
        }
        UNormalization_Check_نتیجہ qc_نتیجہ=UNORM_YES;
        impl.compose_تیز_Check(sArray, sArray+s.لمائی(), onlyContiguous, &qc_نتیجہ);
        أرجع qc_نتیجہ;
    }
    مجازی ثابت char16_t*
    وسعت_تیز_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد_بندی,
                      UError_کوڈ&) ثابت override {
        أرجع impl.compose_تیز_Check(src, حد_بندی, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::وسعت_تیز_Check_Yes;  // Avoid warning about hiding base class function.
    مجازی UNormalization_Check_نتیجہ get_تیز_Check(UChar32 c) ثابت override {
        أرجع impl.get_Comp_تیز_Check(impl.getNorm16(c));
    }
    مجازی UBool hasBoundaryBefore(UChar32 c) ثابت override {
        أرجع impl.hasCompBoundaryBefore(c);
    }
    مجازی UBool hasBoundaryAfter(UChar32 c) ثابت override {
        أرجع impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    مجازی UBool isInert(UChar32 c) ثابت override {
        أرجع impl.isCompInert(c, onlyContiguous);
    }

    ثابت UBool onlyContiguous;
};

class FCDNormalizer2 : عام Normalizer2WithImpl {
عام:
    FCDNormalizer2(ثابت Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    مجازی ~FCDNormalizer2();

خاص:
    مجازی فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *حد_بندی,
              Reordering_بفر &بفر, UError_کوڈ &error_کوڈ) ثابت override {
        impl.makeFCD(src, حد_بندی, &بفر, error_کوڈ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    مجازی فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندی, UBool doNormalize,
                       UnicodeString &محفوظ_درمیان,
                       Reordering_بفر &بفر, UError_کوڈ &error_کوڈ) ثابت override {
        impl.makeFCDAndAppend(src, حد_بندی, doNormalize, محفوظ_درمیان, بفر, error_کوڈ);
    }
    مجازی ثابت char16_t*
    وسعت_تیز_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد_بندی,
                      UError_کوڈ& error_کوڈ) ثابت override {
        أرجع impl.makeFCD(src, حد_بندی, nullptr, error_کوڈ);
    }
    using Normalizer2WithImpl::وسعت_تیز_Check_Yes;  // Avoid warning about hiding base class function.
    مجازی UBool hasBoundaryBefore(UChar32 c) ثابت override {
        أرجع impl.hasFCDBoundaryBefore(c);
    }
    مجازی UBool hasBoundaryAfter(UChar32 c) ثابت override {
        أرجع impl.hasFCDBoundaryAfter(c);
    }
    مجازی UBool isInert(UChar32 c) ثابت override {
        أرجع impl.isFCDInert(c);
    }
};

بنية Norm2AllModes : عام UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    ساكن Norm2AllModes *createInstance(Normalizer2Impl *impl, UError_کوڈ &error_کوڈ);
    ساكن Norm2AllModes *createNFCInstance(UError_کوڈ &error_کوڈ);
    ساكن Norm2AllModes *createInstance(ثابت حرف *packageName,
                                         ثابت حرف *name,
                                         UError_کوڈ &error_کوڈ);

    ساكن ثابت Norm2AllModes *getNFCInstance(UError_کوڈ &error_کوڈ);
    ساكن ثابت Norm2AllModes *getNFKCInstance(UError_کوڈ &error_کوڈ);
    ساكن ثابت Norm2AllModes *getNFKC_CFInstance(UError_کوڈ &error_کوڈ);
    ساكن ثابت Norm2AllModes *getNFKC_SCFInstance(UError_کوڈ &error_کوڈ);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_سرا

#جے_انت  // !UCONFIG_NO_NORMALIZATION
#جے_انت  // __NORM2ALLMODES_H__
