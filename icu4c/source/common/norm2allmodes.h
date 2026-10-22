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

#اگر_غیر_تعریف_شدہ __NORM2ALLMODES_H__
#تعریف_کرو __NORM2ALLMODES_H__

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_NORMALIZATION

#شامل "unicode/edits.h"
#شامل "unicode/normalizer2.h"
#شامل "unicode/stringoptions.h"
#شامل "unicode/unistr.h"
#شامل "cpputils.h"
#شامل "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : عوامی Normalizer2 {
عوامی:
    Normalizer2WithImpl(ثابت Normalizer2Impl &ni) : impl(ni) {}
    مجازی ~Normalizer2WithImpl();

    // normalize
    مجازی UnicodeString&
    normalize(ثابت UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UError_رمز& error_رمز) ثابت override {
        اگر(U_FAILURE(error_رمز)) {
            dest.سیٹ_مقصد_Bogus();
            واپس dest;
        }
        ثابت char16_t *sArray=src.get_بفر();
        اگر(&dest==&src || sArray==nullptr) {
            error_رمز=U_ILLEGAL_دلیل_ERROR;
            dest.سیٹ_مقصد_Bogus();
            واپس dest;
        }
        dest.remove();
        Reordering_بفر بفر(impl, dest);
        اگر(بفر.init(src.لمبائی(), error_رمز)) {
            normalize(sArray, sArray+src.لمبائی(), بفر, error_رمز);
        }
        واپس dest;
    }
    مجازی خالی
    normalize(ثابت char16_t *src, ثابت char16_t *حد_بندی,
              Reordering_بفر &بفر, UError_رمز &error_رمز) ثابت = 0;

    // normalize and append
    مجازی UnicodeString&
    normalize_دوسرا_And_Append(UnicodeString& پہلا U_LIFETIME_BOUND,
                             ثابت UnicodeString& دوسرا,
                             UError_رمز& error_رمز) ثابت override {
        واپس normalize_دوسرا_And_Append(پہلا, دوسرا, true, error_رمز);
    }
    مجازی UnicodeString&
    append(UnicodeString& پہلا U_LIFETIME_BOUND,
           ثابت UnicodeString& دوسرا,
           UError_رمز& error_رمز) ثابت override {
        واپس normalize_دوسرا_And_Append(پہلا, دوسرا, false, error_رمز);
    }
    UnicodeString&
    normalize_دوسرا_And_Append(UnicodeString& پہلا U_LIFETIME_BOUND,
                             ثابت UnicodeString& دوسرا,
                             UBool doNormalize,
                             UError_رمز& error_رمز) ثابت {
        uprv_check_Can_Get_بفر(پہلا, error_رمز);
        اگر(U_FAILURE(error_رمز)) {
            واپس پہلا;
        }
        ثابت char16_t *دوسرا_Array=دوسرا.get_بفر();
        اگر(&پہلا==&دوسرا || دوسرا_Array==nullptr) {
            error_رمز=U_ILLEGAL_دلیل_ERROR;
            واپس پہلا;
        }
        int32_t پہلا_لمبائی=پہلا.لمبائی();
        UnicodeString محفوظ_درمیان;
        {
            Reordering_بفر بفر(impl, پہلا);
            اگر(بفر.init(پہلا_لمبائی+دوسرا.لمبائی(), error_رمز)) {
                normalizeAndAppend(دوسرا_Array, دوسرا_Array+دوسرا.لمبائی(), doNormalize,
                                   محفوظ_درمیان, بفر, error_رمز);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        اگر(U_FAILURE(error_رمز)) {
            // Restore the modified suffix of the first string.
            پہلا.replace(پہلا_لمبائی-محفوظ_درمیان.لمبائی(), 0x7fffffff, محفوظ_درمیان);
        }
        واپس پہلا;
    }
    مجازی خالی
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندی, UBool doNormalize,
                       UnicodeString &محفوظ_درمیان,
                       Reordering_بفر &بفر, UError_رمز &error_رمز) ثابت = 0;
    مجازی UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t بفر[4];
        int32_t لمبائی;
        ثابت char16_t *d=impl.getDecomposition(c, بفر, لمبائی);
        اگر(d==nullptr) {
            واپس false;
        }
        اگر(d==بفر) {
            decomposition.سیٹ_مقصد(بفر, لمبائی);  // copy the string (Jamos from Hangul syllable c)
        } ورنہ {
            decomposition.سیٹ_مقصد(false, d, لمبائی);  // read-only alias
        }
        واپس true;
    }
    مجازی UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t بفر[30];
        int32_t لمبائی;
        ثابت char16_t *d=impl.getRawDecomposition(c, بفر, لمبائی);
        اگر(d==nullptr) {
            واپس false;
        }
        اگر(d==بفر) {
            decomposition.سیٹ_مقصد(بفر, لمبائی);  // copy the string (algorithmic decomposition)
        } ورنہ {
            decomposition.سیٹ_مقصد(false, d, لمبائی);  // read-only alias
        }
        واپس true;
    }
    مجازی UChar32
    composePair(UChar32 a, UChar32 b) ثابت override {
        واپس impl.composePair(a, b);
    }

    مجازی uint8_t
    getCombiningClass(UChar32 c) ثابت override {
        واپس impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    مجازی UBool
    isNormalized(ثابت UnicodeString &s, UError_رمز &error_رمز) ثابت override {
        اگر(U_FAILURE(error_رمز)) {
            واپس false;
        }
        ثابت char16_t *sArray=s.get_بفر();
        اگر(sArray==nullptr) {
            error_رمز=U_ILLEGAL_دلیل_ERROR;
            واپس false;
        }
        ثابت char16_t *s_حد_بندی=sArray+s.لمبائی();
        واپس s_حد_بندی==وسعت_تیز_Check_Yes(sArray, s_حد_بندی, error_رمز);
    }
    مجازی UNormalization_Check_نتیجہ
    تیز_Check(ثابت UnicodeString &s, UError_رمز &error_رمز) ثابت override {
        واپس Normalizer2WithImpl::isNormalized(s, error_رمز) ? UNORM_YES : UNORM_NO;
    }
    مجازی int32_t
    وسعت_تیز_Check_Yes(ثابت UnicodeString &s, UError_رمز &error_رمز) ثابت override {
        اگر(U_FAILURE(error_رمز)) {
            واپس 0;
        }
        ثابت char16_t *sArray=s.get_بفر();
        اگر(sArray==nullptr) {
            error_رمز=U_ILLEGAL_دلیل_ERROR;
            واپس 0;
        }
        واپس static_cast<int32_t>(وسعت_تیز_Check_Yes(sArray, sArray + s.لمبائی(), error_رمز) - sArray);
    }
    مجازی ثابت char16_t*
    وسعت_تیز_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد_بندی,
                      UError_رمز& error_رمز) ثابت = 0;

    مجازی UNormalization_Check_نتیجہ get_تیز_Check(UChar32) ثابت {
        واپس UNORM_YES;
    }

    ثابت Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : عوامی Normalizer2WithImpl {
عوامی:
    DecomposeNormalizer2(ثابت Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    مجازی ~DecomposeNormalizer2();

نجی:
    مجازی خالی
    normalize(ثابت char16_t *src, ثابت char16_t *حد_بندی,
              Reordering_بفر &بفر, UError_رمز &error_رمز) ثابت override {
        impl.decompose(src, حد_بندی, &بفر, error_رمز);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    مجازی خالی
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندی, UBool doNormalize,
                       UnicodeString &محفوظ_درمیان,
                       Reordering_بفر &بفر, UError_رمز &error_رمز) ثابت override {
        impl.decomposeAndAppend(src, حد_بندی, doNormalize, محفوظ_درمیان, بفر, error_رمز);
    }

    خالی
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_رمز &error_رمز) ثابت override {
        اگر (U_FAILURE(error_رمز)) {
            واپس;
        }
        اگر (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.ڈیٹا());
        impl.decomposeUTF8(options, s, s + src.لمبائی(), &sink, edits, error_رمز);
        sink.Flush();
    }
    مجازی UBool
    isNormalizedUTF8(StringPiece sp, UError_رمز &error_رمز) ثابت override {
        اگر(U_FAILURE(error_رمز)) {
            واپس false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.ڈیٹا());
        ثابت uint8_t *s_حد_بندی = s + sp.لمبائی();
        واپس s_حد_بندی == impl.decomposeUTF8(0, s, s_حد_بندی, nullptr, nullptr, error_رمز);
    }

    مجازی ثابت char16_t*
    وسعت_تیز_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد_بندی,
                      UError_رمز& error_رمز) ثابت override {
        واپس impl.decompose(src, حد_بندی, nullptr, error_رمز);
    }
    using Normalizer2WithImpl::وسعت_تیز_Check_Yes;  // Avoid warning about hiding base class function.
    مجازی UNormalization_Check_نتیجہ get_تیز_Check(UChar32 c) ثابت override {
        واپس impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    مجازی UBool hasBoundaryBefore(UChar32 c) ثابت override {
        واپس impl.hasDecompBoundaryBefore(c);
    }
    مجازی UBool hasBoundaryAfter(UChar32 c) ثابت override {
        واپس impl.hasDecompBoundaryAfter(c);
    }
    مجازی UBool isInert(UChar32 c) ثابت override {
        واپس impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : عوامی Normalizer2WithImpl {
عوامی:
    ComposeNormalizer2(ثابت Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    مجازی ~ComposeNormalizer2();

نجی:
    مجازی خالی
    normalize(ثابت char16_t *src, ثابت char16_t *حد_بندی,
              Reordering_بفر &بفر, UError_رمز &error_رمز) ثابت override {
        impl.compose(src, حد_بندی, onlyContiguous, true, بفر, error_رمز);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    خالی
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_رمز &error_رمز) ثابت override {
        اگر (U_FAILURE(error_رمز)) {
            واپس;
        }
        اگر (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.ڈیٹا());
        impl.composeUTF8(options, onlyContiguous, s, s + src.لمبائی(),
                         &sink, edits, error_رمز);
        sink.Flush();
    }

    مجازی خالی
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندی, UBool doNormalize,
                       UnicodeString &محفوظ_درمیان,
                       Reordering_بفر &بفر, UError_رمز &error_رمز) ثابت override {
        impl.composeAndAppend(src, حد_بندی, doNormalize, onlyContiguous, محفوظ_درمیان, بفر, error_رمز);
    }

    مجازی UBool
    isNormalized(ثابت UnicodeString &s, UError_رمز &error_رمز) ثابت override {
        اگر(U_FAILURE(error_رمز)) {
            واپس false;
        }
        ثابت char16_t *sArray=s.get_بفر();
        اگر(sArray==nullptr) {
            error_رمز=U_ILLEGAL_دلیل_ERROR;
            واپس false;
        }
        UnicodeString عارضی;
        Reordering_بفر بفر(impl, عارضی);
        اگر(!بفر.init(5, error_رمز)) {  // small destCapacity for substring normalization
            واپس false;
        }
        واپس impl.compose(sArray, sArray+s.لمبائی(), onlyContiguous, false, بفر, error_رمز);
    }
    مجازی UBool
    isNormalizedUTF8(StringPiece sp, UError_رمز &error_رمز) ثابت override {
        اگر(U_FAILURE(error_رمز)) {
            واپس false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.ڈیٹا());
        واپس impl.composeUTF8(0, onlyContiguous, s, s + sp.لمبائی(), nullptr, nullptr, error_رمز);
    }
    مجازی UNormalization_Check_نتیجہ
    تیز_Check(ثابت UnicodeString &s, UError_رمز &error_رمز) ثابت override {
        اگر(U_FAILURE(error_رمز)) {
            واپس UNORM_MAYBE;
        }
        ثابت char16_t *sArray=s.get_بفر();
        اگر(sArray==nullptr) {
            error_رمز=U_ILLEGAL_دلیل_ERROR;
            واپس UNORM_MAYBE;
        }
        UNormalization_Check_نتیجہ qc_نتیجہ=UNORM_YES;
        impl.compose_تیز_Check(sArray, sArray+s.لمبائی(), onlyContiguous, &qc_نتیجہ);
        واپس qc_نتیجہ;
    }
    مجازی ثابت char16_t*
    وسعت_تیز_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد_بندی,
                      UError_رمز&) ثابت override {
        واپس impl.compose_تیز_Check(src, حد_بندی, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::وسعت_تیز_Check_Yes;  // Avoid warning about hiding base class function.
    مجازی UNormalization_Check_نتیجہ get_تیز_Check(UChar32 c) ثابت override {
        واپس impl.get_Comp_تیز_Check(impl.getNorm16(c));
    }
    مجازی UBool hasBoundaryBefore(UChar32 c) ثابت override {
        واپس impl.hasCompBoundaryBefore(c);
    }
    مجازی UBool hasBoundaryAfter(UChar32 c) ثابت override {
        واپس impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    مجازی UBool isInert(UChar32 c) ثابت override {
        واپس impl.isCompInert(c, onlyContiguous);
    }

    ثابت UBool onlyContiguous;
};

class FCDNormalizer2 : عوامی Normalizer2WithImpl {
عوامی:
    FCDNormalizer2(ثابت Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    مجازی ~FCDNormalizer2();

نجی:
    مجازی خالی
    normalize(ثابت char16_t *src, ثابت char16_t *حد_بندی,
              Reordering_بفر &بفر, UError_رمز &error_رمز) ثابت override {
        impl.makeFCD(src, حد_بندی, &بفر, error_رمز);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    مجازی خالی
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد_بندی, UBool doNormalize,
                       UnicodeString &محفوظ_درمیان,
                       Reordering_بفر &بفر, UError_رمز &error_رمز) ثابت override {
        impl.makeFCDAndAppend(src, حد_بندی, doNormalize, محفوظ_درمیان, بفر, error_رمز);
    }
    مجازی ثابت char16_t*
    وسعت_تیز_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد_بندی,
                      UError_رمز& error_رمز) ثابت override {
        واپس impl.makeFCD(src, حد_بندی, nullptr, error_رمز);
    }
    using Normalizer2WithImpl::وسعت_تیز_Check_Yes;  // Avoid warning about hiding base class function.
    مجازی UBool hasBoundaryBefore(UChar32 c) ثابت override {
        واپس impl.hasFCDBoundaryBefore(c);
    }
    مجازی UBool hasBoundaryAfter(UChar32 c) ثابت override {
        واپس impl.hasFCDBoundaryAfter(c);
    }
    مجازی UBool isInert(UChar32 c) ثابت override {
        واپس impl.isFCDInert(c);
    }
};

ساخت Norm2AllModes : عوامی UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    ساکن Norm2AllModes *createInstance(Normalizer2Impl *impl, UError_رمز &error_رمز);
    ساکن Norm2AllModes *createNFCInstance(UError_رمز &error_رمز);
    ساکن Norm2AllModes *createInstance(ثابت حرف *packageName,
                                         ثابت حرف *name,
                                         UError_رمز &error_رمز);

    ساکن ثابت Norm2AllModes *getNFCInstance(UError_رمز &error_رمز);
    ساکن ثابت Norm2AllModes *getNFKCInstance(UError_رمز &error_رمز);
    ساکن ثابت Norm2AllModes *getNFKC_CFInstance(UError_رمز &error_رمز);
    ساکن ثابت Norm2AllModes *getNFKC_SCFInstance(UError_رمز &error_رمز);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_انتہا

#اگر_ختم  // !UCONFIG_NO_NORMALIZATION
#اگر_ختم  // __NORM2ALLMODES_H__
