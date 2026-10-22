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

#که_نه_تعریف_شوی __NORM2ALLMODES_H__
#تعریف_کړه __NORM2ALLMODES_H__

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
              UError_کوډ& error_کوډ) ثابت override {
        إذا(U_FAILURE(error_کوډ)) {
            dest.سېټ_منزل_Bogus();
            أرجع dest;
        }
        ثابت char16_t *sArray=src.get_بفر();
        إذا(&dest==&src || sArray==nullptr) {
            error_کوډ=U_ILLEGAL_دلیل_ERROR;
            dest.سېټ_منزل_Bogus();
            أرجع dest;
        }
        dest.remove();
        Reordering_بفر بفر(impl, dest);
        إذا(بفر.init(src.اوږدتیا(), error_کوډ)) {
            normalize(sArray, sArray+src.اوږدتیا(), بفر, error_کوډ);
        }
        أرجع dest;
    }
    مجازي فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *حد,
              Reordering_بفر &بفر, UError_کوډ &error_کوډ) ثابت = 0;

    // normalize and append
    مجازي UnicodeString&
    normalize_دوهم_And_Append(UnicodeString& لومړی U_LIFETIME_BOUND,
                             ثابت UnicodeString& دوهم,
                             UError_کوډ& error_کوډ) ثابت override {
        أرجع normalize_دوهم_And_Append(لومړی, دوهم, true, error_کوډ);
    }
    مجازي UnicodeString&
    append(UnicodeString& لومړی U_LIFETIME_BOUND,
           ثابت UnicodeString& دوهم,
           UError_کوډ& error_کوډ) ثابت override {
        أرجع normalize_دوهم_And_Append(لومړی, دوهم, false, error_کوډ);
    }
    UnicodeString&
    normalize_دوهم_And_Append(UnicodeString& لومړی U_LIFETIME_BOUND,
                             ثابت UnicodeString& دوهم,
                             UBool doNormalize,
                             UError_کوډ& error_کوډ) ثابت {
        uprv_check_Can_Get_بفر(لومړی, error_کوډ);
        إذا(U_FAILURE(error_کوډ)) {
            أرجع لومړی;
        }
        ثابت char16_t *دوهم_Array=دوهم.get_بفر();
        إذا(&لومړی==&دوهم || دوهم_Array==nullptr) {
            error_کوډ=U_ILLEGAL_دلیل_ERROR;
            أرجع لومړی;
        }
        int32_t لومړی_اوږدتیا=لومړی.اوږدتیا();
        UnicodeString خوندي_منځ;
        {
            Reordering_بفر بفر(impl, لومړی);
            إذا(بفر.init(لومړی_اوږدتیا+دوهم.اوږدتیا(), error_کوډ)) {
                normalizeAndAppend(دوهم_Array, دوهم_Array+دوهم.اوږدتیا(), doNormalize,
                                   خوندي_منځ, بفر, error_کوډ);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        إذا(U_FAILURE(error_کوډ)) {
            // Restore the modified suffix of the first string.
            لومړی.replace(لومړی_اوږدتیا-خوندي_منځ.اوږدتیا(), 0x7fffffff, خوندي_منځ);
        }
        أرجع لومړی;
    }
    مجازي فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد, UBool doNormalize,
                       UnicodeString &خوندي_منځ,
                       Reordering_بفر &بفر, UError_کوډ &error_کوډ) ثابت = 0;
    مجازي UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t بفر[4];
        int32_t اوږدتیا;
        ثابت char16_t *d=impl.getDecomposition(c, بفر, اوږدتیا);
        إذا(d==nullptr) {
            أرجع false;
        }
        إذا(d==بفر) {
            decomposition.سېټ_منزل(بفر, اوږدتیا);  // copy the string (Jamos from Hangul syllable c)
        } وإلا {
            decomposition.سېټ_منزل(false, d, اوږدتیا);  // read-only alias
        }
        أرجع true;
    }
    مجازي UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t بفر[30];
        int32_t اوږدتیا;
        ثابت char16_t *d=impl.getRawDecomposition(c, بفر, اوږدتیا);
        إذا(d==nullptr) {
            أرجع false;
        }
        إذا(d==بفر) {
            decomposition.سېټ_منزل(بفر, اوږدتیا);  // copy the string (algorithmic decomposition)
        } وإلا {
            decomposition.سېټ_منزل(false, d, اوږدتیا);  // read-only alias
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
    isNormalized(ثابت UnicodeString &s, UError_کوډ &error_کوډ) ثابت override {
        إذا(U_FAILURE(error_کوډ)) {
            أرجع false;
        }
        ثابت char16_t *sArray=s.get_بفر();
        إذا(sArray==nullptr) {
            error_کوډ=U_ILLEGAL_دلیل_ERROR;
            أرجع false;
        }
        ثابت char16_t *s_حد=sArray+s.اوږدتیا();
        أرجع s_حد==پراختیا_چټک_Check_Yes(sArray, s_حد, error_کوډ);
    }
    مجازي UNormalization_Check_پایله
    چټک_Check(ثابت UnicodeString &s, UError_کوډ &error_کوډ) ثابت override {
        أرجع Normalizer2WithImpl::isNormalized(s, error_کوډ) ? UNORM_YES : UNORM_NO;
    }
    مجازي int32_t
    پراختیا_چټک_Check_Yes(ثابت UnicodeString &s, UError_کوډ &error_کوډ) ثابت override {
        إذا(U_FAILURE(error_کوډ)) {
            أرجع 0;
        }
        ثابت char16_t *sArray=s.get_بفر();
        إذا(sArray==nullptr) {
            error_کوډ=U_ILLEGAL_دلیل_ERROR;
            أرجع 0;
        }
        أرجع static_cast<int32_t>(پراختیا_چټک_Check_Yes(sArray, sArray + s.اوږدتیا(), error_کوډ) - sArray);
    }
    مجازي ثابت char16_t*
    پراختیا_چټک_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد,
                      UError_کوډ& error_کوډ) ثابت = 0;

    مجازي UNormalization_Check_پایله get_چټک_Check(UChar32) ثابت {
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
    normalize(ثابت char16_t *src, ثابت char16_t *حد,
              Reordering_بفر &بفر, UError_کوډ &error_کوډ) ثابت override {
        impl.decompose(src, حد, &بفر, error_کوډ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    مجازي فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد, UBool doNormalize,
                       UnicodeString &خوندي_منځ,
                       Reordering_بفر &بفر, UError_کوډ &error_کوډ) ثابت override {
        impl.decomposeAndAppend(src, حد, doNormalize, خوندي_منځ, بفر, error_کوډ);
    }

    فراغ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_کوډ &error_کوډ) ثابت override {
        إذا (U_FAILURE(error_کوډ)) {
            أرجع;
        }
        إذا (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.معلومات());
        impl.decomposeUTF8(options, s, s + src.اوږدتیا(), &sink, edits, error_کوډ);
        sink.Flush();
    }
    مجازي UBool
    isNormalizedUTF8(StringPiece sp, UError_کوډ &error_کوډ) ثابت override {
        إذا(U_FAILURE(error_کوډ)) {
            أرجع false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.معلومات());
        ثابت uint8_t *s_حد = s + sp.اوږدتیا();
        أرجع s_حد == impl.decomposeUTF8(0, s, s_حد, nullptr, nullptr, error_کوډ);
    }

    مجازي ثابت char16_t*
    پراختیا_چټک_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد,
                      UError_کوډ& error_کوډ) ثابت override {
        أرجع impl.decompose(src, حد, nullptr, error_کوډ);
    }
    using Normalizer2WithImpl::پراختیا_چټک_Check_Yes;  // Avoid warning about hiding base class function.
    مجازي UNormalization_Check_پایله get_چټک_Check(UChar32 c) ثابت override {
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
    normalize(ثابت char16_t *src, ثابت char16_t *حد,
              Reordering_بفر &بفر, UError_کوډ &error_کوډ) ثابت override {
        impl.compose(src, حد, onlyContiguous, true, بفر, error_کوډ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    فراغ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_کوډ &error_کوډ) ثابت override {
        إذا (U_FAILURE(error_کوډ)) {
            أرجع;
        }
        إذا (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.معلومات());
        impl.composeUTF8(options, onlyContiguous, s, s + src.اوږدتیا(),
                         &sink, edits, error_کوډ);
        sink.Flush();
    }

    مجازي فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد, UBool doNormalize,
                       UnicodeString &خوندي_منځ,
                       Reordering_بفر &بفر, UError_کوډ &error_کوډ) ثابت override {
        impl.composeAndAppend(src, حد, doNormalize, onlyContiguous, خوندي_منځ, بفر, error_کوډ);
    }

    مجازي UBool
    isNormalized(ثابت UnicodeString &s, UError_کوډ &error_کوډ) ثابت override {
        إذا(U_FAILURE(error_کوډ)) {
            أرجع false;
        }
        ثابت char16_t *sArray=s.get_بفر();
        إذا(sArray==nullptr) {
            error_کوډ=U_ILLEGAL_دلیل_ERROR;
            أرجع false;
        }
        UnicodeString لنډمهاله;
        Reordering_بفر بفر(impl, لنډمهاله);
        إذا(!بفر.init(5, error_کوډ)) {  // small destCapacity for substring normalization
            أرجع false;
        }
        أرجع impl.compose(sArray, sArray+s.اوږدتیا(), onlyContiguous, false, بفر, error_کوډ);
    }
    مجازي UBool
    isNormalizedUTF8(StringPiece sp, UError_کوډ &error_کوډ) ثابت override {
        إذا(U_FAILURE(error_کوډ)) {
            أرجع false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.معلومات());
        أرجع impl.composeUTF8(0, onlyContiguous, s, s + sp.اوږدتیا(), nullptr, nullptr, error_کوډ);
    }
    مجازي UNormalization_Check_پایله
    چټک_Check(ثابت UnicodeString &s, UError_کوډ &error_کوډ) ثابت override {
        إذا(U_FAILURE(error_کوډ)) {
            أرجع UNORM_MAYBE;
        }
        ثابت char16_t *sArray=s.get_بفر();
        إذا(sArray==nullptr) {
            error_کوډ=U_ILLEGAL_دلیل_ERROR;
            أرجع UNORM_MAYBE;
        }
        UNormalization_Check_پایله qc_پایله=UNORM_YES;
        impl.compose_چټک_Check(sArray, sArray+s.اوږدتیا(), onlyContiguous, &qc_پایله);
        أرجع qc_پایله;
    }
    مجازي ثابت char16_t*
    پراختیا_چټک_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد,
                      UError_کوډ&) ثابت override {
        أرجع impl.compose_چټک_Check(src, حد, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::پراختیا_چټک_Check_Yes;  // Avoid warning about hiding base class function.
    مجازي UNormalization_Check_پایله get_چټک_Check(UChar32 c) ثابت override {
        أرجع impl.get_Comp_چټک_Check(impl.getNorm16(c));
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
    normalize(ثابت char16_t *src, ثابت char16_t *حد,
              Reordering_بفر &بفر, UError_کوډ &error_کوډ) ثابت override {
        impl.makeFCD(src, حد, &بفر, error_کوډ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    مجازي فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد, UBool doNormalize,
                       UnicodeString &خوندي_منځ,
                       Reordering_بفر &بفر, UError_کوډ &error_کوډ) ثابت override {
        impl.makeFCDAndAppend(src, حد, doNormalize, خوندي_منځ, بفر, error_کوډ);
    }
    مجازي ثابت char16_t*
    پراختیا_چټک_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد,
                      UError_کوډ& error_کوډ) ثابت override {
        أرجع impl.makeFCD(src, حد, nullptr, error_کوډ);
    }
    using Normalizer2WithImpl::پراختیا_چټک_Check_Yes;  // Avoid warning about hiding base class function.
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

    ساكن Norm2AllModes *createInstance(Normalizer2Impl *impl, UError_کوډ &error_کوډ);
    ساكن Norm2AllModes *createNFCInstance(UError_کوډ &error_کوډ);
    ساكن Norm2AllModes *createInstance(ثابت حرف *packageName,
                                         ثابت حرف *name,
                                         UError_کوډ &error_کوډ);

    ساكن ثابت Norm2AllModes *getNFCInstance(UError_کوډ &error_کوډ);
    ساكن ثابت Norm2AllModes *getNFKCInstance(UError_کوډ &error_کوډ);
    ساكن ثابت Norm2AllModes *getNFKC_CFInstance(UError_کوډ &error_کوډ);
    ساكن ثابت Norm2AllModes *getNFKC_SCFInstance(UError_کوډ &error_کوډ);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_پای_ټکی

#د_که_پای  // !UCONFIG_NO_NORMALIZATION
#د_که_پای  // __NORM2ALLMODES_H__
