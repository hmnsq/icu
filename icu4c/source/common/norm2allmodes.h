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

#اگر_تعریف_نشده __NORM2ALLMODES_H__
#تعریف_کن __NORM2ALLMODES_H__

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
              UError_کد& error_کد) ثابت override {
        إذا(U_FAILURE(error_کد)) {
            dest.مجموعه_مقصد_Bogus();
            أرجع dest;
        }
        ثابت char16_t *sArray=src.get_میانگیر();
        إذا(&dest==&src || sArray==nullptr) {
            error_کد=U_ILLEGAL_آرگومان_ERROR;
            dest.مجموعه_مقصد_Bogus();
            أرجع dest;
        }
        dest.remove();
        Reordering_میانگیر میانگیر(impl, dest);
        إذا(میانگیر.init(src.درازی(), error_کد)) {
            normalize(sArray, sArray+src.درازی(), میانگیر, error_کد);
        }
        أرجع dest;
    }
    مجازی فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *حد,
              Reordering_میانگیر &میانگیر, UError_کد &error_کد) ثابت = 0;

    // normalize and append
    مجازی UnicodeString&
    normalize_دومی_And_Append(UnicodeString& اولی U_LIFETIME_BOUND,
                             ثابت UnicodeString& دومی,
                             UError_کد& error_کد) ثابت override {
        أرجع normalize_دومی_And_Append(اولی, دومی, true, error_کد);
    }
    مجازی UnicodeString&
    append(UnicodeString& اولی U_LIFETIME_BOUND,
           ثابت UnicodeString& دومی,
           UError_کد& error_کد) ثابت override {
        أرجع normalize_دومی_And_Append(اولی, دومی, false, error_کد);
    }
    UnicodeString&
    normalize_دومی_And_Append(UnicodeString& اولی U_LIFETIME_BOUND,
                             ثابت UnicodeString& دومی,
                             UBool doNormalize,
                             UError_کد& error_کد) ثابت {
        uprv_check_Can_Get_میانگیر(اولی, error_کد);
        إذا(U_FAILURE(error_کد)) {
            أرجع اولی;
        }
        ثابت char16_t *دومی_Array=دومی.get_میانگیر();
        إذا(&اولی==&دومی || دومی_Array==nullptr) {
            error_کد=U_ILLEGAL_آرگومان_ERROR;
            أرجع اولی;
        }
        int32_t اولی_درازی=اولی.درازی();
        UnicodeString امن_میانه;
        {
            Reordering_میانگیر میانگیر(impl, اولی);
            إذا(میانگیر.init(اولی_درازی+دومی.درازی(), error_کد)) {
                normalizeAndAppend(دومی_Array, دومی_Array+دومی.درازی(), doNormalize,
                                   امن_میانه, میانگیر, error_کد);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        إذا(U_FAILURE(error_کد)) {
            // Restore the modified suffix of the first string.
            اولی.replace(اولی_درازی-امن_میانه.درازی(), 0x7fffffff, امن_میانه);
        }
        أرجع اولی;
    }
    مجازی فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد, UBool doNormalize,
                       UnicodeString &امن_میانه,
                       Reordering_میانگیر &میانگیر, UError_کد &error_کد) ثابت = 0;
    مجازی UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t میانگیر[4];
        int32_t درازی;
        ثابت char16_t *d=impl.getDecomposition(c, میانگیر, درازی);
        إذا(d==nullptr) {
            أرجع false;
        }
        إذا(d==میانگیر) {
            decomposition.مجموعه_مقصد(میانگیر, درازی);  // copy the string (Jamos from Hangul syllable c)
        } وإلا {
            decomposition.مجموعه_مقصد(false, d, درازی);  // read-only alias
        }
        أرجع true;
    }
    مجازی UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t میانگیر[30];
        int32_t درازی;
        ثابت char16_t *d=impl.getRawDecomposition(c, میانگیر, درازی);
        إذا(d==nullptr) {
            أرجع false;
        }
        إذا(d==میانگیر) {
            decomposition.مجموعه_مقصد(میانگیر, درازی);  // copy the string (algorithmic decomposition)
        } وإلا {
            decomposition.مجموعه_مقصد(false, d, درازی);  // read-only alias
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
    isNormalized(ثابت UnicodeString &s, UError_کد &error_کد) ثابت override {
        إذا(U_FAILURE(error_کد)) {
            أرجع false;
        }
        ثابت char16_t *sArray=s.get_میانگیر();
        إذا(sArray==nullptr) {
            error_کد=U_ILLEGAL_آرگومان_ERROR;
            أرجع false;
        }
        ثابت char16_t *s_حد=sArray+s.درازی();
        أرجع s_حد==بازه_سریع_Check_Yes(sArray, s_حد, error_کد);
    }
    مجازی UNormalization_Check_نتیجه
    سریع_Check(ثابت UnicodeString &s, UError_کد &error_کد) ثابت override {
        أرجع Normalizer2WithImpl::isNormalized(s, error_کد) ? UNORM_YES : UNORM_NO;
    }
    مجازی int32_t
    بازه_سریع_Check_Yes(ثابت UnicodeString &s, UError_کد &error_کد) ثابت override {
        إذا(U_FAILURE(error_کد)) {
            أرجع 0;
        }
        ثابت char16_t *sArray=s.get_میانگیر();
        إذا(sArray==nullptr) {
            error_کد=U_ILLEGAL_آرگومان_ERROR;
            أرجع 0;
        }
        أرجع static_cast<int32_t>(بازه_سریع_Check_Yes(sArray, sArray + s.درازی(), error_کد) - sArray);
    }
    مجازی ثابت char16_t*
    بازه_سریع_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد,
                      UError_کد& error_کد) ثابت = 0;

    مجازی UNormalization_Check_نتیجه get_سریع_Check(UChar32) ثابت {
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
    normalize(ثابت char16_t *src, ثابت char16_t *حد,
              Reordering_میانگیر &میانگیر, UError_کد &error_کد) ثابت override {
        impl.decompose(src, حد, &میانگیر, error_کد);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    مجازی فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد, UBool doNormalize,
                       UnicodeString &امن_میانه,
                       Reordering_میانگیر &میانگیر, UError_کد &error_کد) ثابت override {
        impl.decomposeAndAppend(src, حد, doNormalize, امن_میانه, میانگیر, error_کد);
    }

    فراغ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_کد &error_کد) ثابت override {
        إذا (U_FAILURE(error_کد)) {
            أرجع;
        }
        إذا (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.داده());
        impl.decomposeUTF8(options, s, s + src.درازی(), &sink, edits, error_کد);
        sink.Flush();
    }
    مجازی UBool
    isNormalizedUTF8(StringPiece sp, UError_کد &error_کد) ثابت override {
        إذا(U_FAILURE(error_کد)) {
            أرجع false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.داده());
        ثابت uint8_t *s_حد = s + sp.درازی();
        أرجع s_حد == impl.decomposeUTF8(0, s, s_حد, nullptr, nullptr, error_کد);
    }

    مجازی ثابت char16_t*
    بازه_سریع_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد,
                      UError_کد& error_کد) ثابت override {
        أرجع impl.decompose(src, حد, nullptr, error_کد);
    }
    using Normalizer2WithImpl::بازه_سریع_Check_Yes;  // Avoid warning about hiding base class function.
    مجازی UNormalization_Check_نتیجه get_سریع_Check(UChar32 c) ثابت override {
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
    normalize(ثابت char16_t *src, ثابت char16_t *حد,
              Reordering_میانگیر &میانگیر, UError_کد &error_کد) ثابت override {
        impl.compose(src, حد, onlyContiguous, true, میانگیر, error_کد);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    فراغ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_کد &error_کد) ثابت override {
        إذا (U_FAILURE(error_کد)) {
            أرجع;
        }
        إذا (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.داده());
        impl.composeUTF8(options, onlyContiguous, s, s + src.درازی(),
                         &sink, edits, error_کد);
        sink.Flush();
    }

    مجازی فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد, UBool doNormalize,
                       UnicodeString &امن_میانه,
                       Reordering_میانگیر &میانگیر, UError_کد &error_کد) ثابت override {
        impl.composeAndAppend(src, حد, doNormalize, onlyContiguous, امن_میانه, میانگیر, error_کد);
    }

    مجازی UBool
    isNormalized(ثابت UnicodeString &s, UError_کد &error_کد) ثابت override {
        إذا(U_FAILURE(error_کد)) {
            أرجع false;
        }
        ثابت char16_t *sArray=s.get_میانگیر();
        إذا(sArray==nullptr) {
            error_کد=U_ILLEGAL_آرگومان_ERROR;
            أرجع false;
        }
        UnicodeString موقت;
        Reordering_میانگیر میانگیر(impl, موقت);
        إذا(!میانگیر.init(5, error_کد)) {  // small destCapacity for substring normalization
            أرجع false;
        }
        أرجع impl.compose(sArray, sArray+s.درازی(), onlyContiguous, false, میانگیر, error_کد);
    }
    مجازی UBool
    isNormalizedUTF8(StringPiece sp, UError_کد &error_کد) ثابت override {
        إذا(U_FAILURE(error_کد)) {
            أرجع false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.داده());
        أرجع impl.composeUTF8(0, onlyContiguous, s, s + sp.درازی(), nullptr, nullptr, error_کد);
    }
    مجازی UNormalization_Check_نتیجه
    سریع_Check(ثابت UnicodeString &s, UError_کد &error_کد) ثابت override {
        إذا(U_FAILURE(error_کد)) {
            أرجع UNORM_MAYBE;
        }
        ثابت char16_t *sArray=s.get_میانگیر();
        إذا(sArray==nullptr) {
            error_کد=U_ILLEGAL_آرگومان_ERROR;
            أرجع UNORM_MAYBE;
        }
        UNormalization_Check_نتیجه qc_نتیجه=UNORM_YES;
        impl.compose_سریع_Check(sArray, sArray+s.درازی(), onlyContiguous, &qc_نتیجه);
        أرجع qc_نتیجه;
    }
    مجازی ثابت char16_t*
    بازه_سریع_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد,
                      UError_کد&) ثابت override {
        أرجع impl.compose_سریع_Check(src, حد, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::بازه_سریع_Check_Yes;  // Avoid warning about hiding base class function.
    مجازی UNormalization_Check_نتیجه get_سریع_Check(UChar32 c) ثابت override {
        أرجع impl.get_Comp_سریع_Check(impl.getNorm16(c));
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
    normalize(ثابت char16_t *src, ثابت char16_t *حد,
              Reordering_میانگیر &میانگیر, UError_کد &error_کد) ثابت override {
        impl.makeFCD(src, حد, &میانگیر, error_کد);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    مجازی فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *حد, UBool doNormalize,
                       UnicodeString &امن_میانه,
                       Reordering_میانگیر &میانگیر, UError_کد &error_کد) ثابت override {
        impl.makeFCDAndAppend(src, حد, doNormalize, امن_میانه, میانگیر, error_کد);
    }
    مجازی ثابت char16_t*
    بازه_سریع_Check_Yes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* حد,
                      UError_کد& error_کد) ثابت override {
        أرجع impl.makeFCD(src, حد, nullptr, error_کد);
    }
    using Normalizer2WithImpl::بازه_سریع_Check_Yes;  // Avoid warning about hiding base class function.
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

    ساكن Norm2AllModes *createInstance(Normalizer2Impl *impl, UError_کد &error_کد);
    ساكن Norm2AllModes *createNFCInstance(UError_کد &error_کد);
    ساكن Norm2AllModes *createInstance(ثابت حرف *packageName,
                                         ثابت حرف *name,
                                         UError_کد &error_کد);

    ساكن ثابت Norm2AllModes *getNFCInstance(UError_کد &error_کد);
    ساكن ثابت Norm2AllModes *getNFKCInstance(UError_کد &error_کد);
    ساكن ثابت Norm2AllModes *getNFKC_CFInstance(UError_کد &error_کد);
    ساكن ثابت Norm2AllModes *getNFKC_SCFInstance(UError_کد &error_کد);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_انتها

#پایان_اگر  // !UCONFIG_NO_NORMALIZATION
#پایان_اگر  // __NORM2ALLMODES_H__
