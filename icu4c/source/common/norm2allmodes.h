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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __NORM2ALLMODES_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __NORM2ALLMODES_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_NORMALIZATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/edits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/normalizer2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringoptions.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cpputils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : ਜਨਤਕ Normalizer2 {
ਜਨਤਕ:
    Normalizer2WithImpl(ਅਚਲ Normalizer2Impl &ni) : impl(ni) {}
    ਆਭਾਸੀ ~Normalizer2WithImpl();

    // normalize
    ਆਭਾਸੀ UnicodeString&
    normalize(ਅਚਲ UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ override {
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            dest.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
            ਵਾਪਸ dest;
        }
        ਅਚਲ char16_t *sArray=src.get_ਬਫਰ();
        ਜੇ(&dest==&src || sArray==nullptr) {
            error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
            dest.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
            ਵਾਪਸ dest;
        }
        dest.remove();
        Reordering_ਬਫਰ ਬਫਰ(impl, dest);
        ਜੇ(ਬਫਰ.init(src.ਲੰਮਾਈ(), error_ਕੋਡ)) {
            normalize(sArray, sArray+src.ਲੰਮਾਈ(), ਬਫਰ, error_ਕੋਡ);
        }
        ਵਾਪਸ dest;
    }
    ਆਭਾਸੀ ਖਾਲੀ
    normalize(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
              Reordering_ਬਫਰ &ਬਫਰ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ = 0;

    // normalize and append
    ਆਭਾਸੀ UnicodeString&
    normalize_ਦੂਜਾ_And_Append(UnicodeString& ਪਹਿਲਾ U_LIFETIME_BOUND,
                             ਅਚਲ UnicodeString& ਦੂਜਾ,
                             UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ override {
        ਵਾਪਸ normalize_ਦੂਜਾ_And_Append(ਪਹਿਲਾ, ਦੂਜਾ, true, error_ਕੋਡ);
    }
    ਆਭਾਸੀ UnicodeString&
    append(UnicodeString& ਪਹਿਲਾ U_LIFETIME_BOUND,
           ਅਚਲ UnicodeString& ਦੂਜਾ,
           UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ override {
        ਵਾਪਸ normalize_ਦੂਜਾ_And_Append(ਪਹਿਲਾ, ਦੂਜਾ, false, error_ਕੋਡ);
    }
    UnicodeString&
    normalize_ਦੂਜਾ_And_Append(UnicodeString& ਪਹਿਲਾ U_LIFETIME_BOUND,
                             ਅਚਲ UnicodeString& ਦੂਜਾ,
                             UBool doNormalize,
                             UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
        uprv_check_Can_Get_ਬਫਰ(ਪਹਿਲਾ, error_ਕੋਡ);
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ ਪਹਿਲਾ;
        }
        ਅਚਲ char16_t *ਦੂਜਾ_Array=ਦੂਜਾ.get_ਬਫਰ();
        ਜੇ(&ਪਹਿਲਾ==&ਦੂਜਾ || ਦੂਜਾ_Array==nullptr) {
            error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ ਪਹਿਲਾ;
        }
        int32_t ਪਹਿਲਾ_ਲੰਮਾਈ=ਪਹਿਲਾ.ਲੰਮਾਈ();
        UnicodeString ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ;
        {
            Reordering_ਬਫਰ ਬਫਰ(impl, ਪਹਿਲਾ);
            ਜੇ(ਬਫਰ.init(ਪਹਿਲਾ_ਲੰਮਾਈ+ਦੂਜਾ.ਲੰਮਾਈ(), error_ਕੋਡ)) {
                normalizeAndAppend(ਦੂਜਾ_Array, ਦੂਜਾ_Array+ਦੂਜਾ.ਲੰਮਾਈ(), doNormalize,
                                   ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ, ਬਫਰ, error_ਕੋਡ);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            // Restore the modified suffix of the first string.
            ਪਹਿਲਾ.replace(ਪਹਿਲਾ_ਲੰਮਾਈ-ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ.ਲੰਮਾਈ(), 0x7fffffff, ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ);
        }
        ਵਾਪਸ ਪਹਿਲਾ;
    }
    ਆਭਾਸੀ ਖਾਲੀ
    normalizeAndAppend(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ, UBool doNormalize,
                       UnicodeString &ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ,
                       Reordering_ਬਫਰ &ਬਫਰ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ = 0;
    ਆਭਾਸੀ UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ਅਚਲ override {
        char16_t ਬਫਰ[4];
        int32_t ਲੰਮਾਈ;
        ਅਚਲ char16_t *d=impl.getDecomposition(c, ਬਫਰ, ਲੰਮਾਈ);
        ਜੇ(d==nullptr) {
            ਵਾਪਸ false;
        }
        ਜੇ(d==ਬਫਰ) {
            decomposition.ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਬਫਰ, ਲੰਮਾਈ);  // copy the string (Jamos from Hangul syllable c)
        } ਨਹੀਂਤਾਂ {
            decomposition.ਸੈੱਟ_ਮੰਜ਼ਿਲ(false, d, ਲੰਮਾਈ);  // read-only alias
        }
        ਵਾਪਸ true;
    }
    ਆਭਾਸੀ UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ਅਚਲ override {
        char16_t ਬਫਰ[30];
        int32_t ਲੰਮਾਈ;
        ਅਚਲ char16_t *d=impl.getRawDecomposition(c, ਬਫਰ, ਲੰਮਾਈ);
        ਜੇ(d==nullptr) {
            ਵਾਪਸ false;
        }
        ਜੇ(d==ਬਫਰ) {
            decomposition.ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਬਫਰ, ਲੰਮਾਈ);  // copy the string (algorithmic decomposition)
        } ਨਹੀਂਤਾਂ {
            decomposition.ਸੈੱਟ_ਮੰਜ਼ਿਲ(false, d, ਲੰਮਾਈ);  // read-only alias
        }
        ਵਾਪਸ true;
    }
    ਆਭਾਸੀ UChar32
    composePair(UChar32 a, UChar32 b) ਅਚਲ override {
        ਵਾਪਸ impl.composePair(a, b);
    }

    ਆਭਾਸੀ uint8_t
    getCombiningClass(UChar32 c) ਅਚਲ override {
        ਵਾਪਸ impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    ਆਭਾਸੀ UBool
    isNormalized(ਅਚਲ UnicodeString &s, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ false;
        }
        ਅਚਲ char16_t *sArray=s.get_ਬਫਰ();
        ਜੇ(sArray==nullptr) {
            error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ false;
        }
        ਅਚਲ char16_t *s_ਹੱਦਬੰਦੀ=sArray+s.ਲੰਮਾਈ();
        ਵਾਪਸ s_ਹੱਦਬੰਦੀ==ਵਿਸਤਾਰ_ਤੇਜ਼_Check_Yes(sArray, s_ਹੱਦਬੰਦੀ, error_ਕੋਡ);
    }
    ਆਭਾਸੀ UNormalization_Check_ਨਤੀਜਾ
    ਤੇਜ਼_Check(ਅਚਲ UnicodeString &s, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        ਵਾਪਸ Normalizer2WithImpl::isNormalized(s, error_ਕੋਡ) ? UNORM_YES : UNORM_NO;
    }
    ਆਭਾਸੀ int32_t
    ਵਿਸਤਾਰ_ਤੇਜ਼_Check_Yes(ਅਚਲ UnicodeString &s, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ 0;
        }
        ਅਚਲ char16_t *sArray=s.get_ਬਫਰ();
        ਜੇ(sArray==nullptr) {
            error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ 0;
        }
        ਵਾਪਸ static_cast<int32_t>(ਵਿਸਤਾਰ_ਤੇਜ਼_Check_Yes(sArray, sArray + s.ਲੰਮਾਈ(), error_ਕੋਡ) - sArray);
    }
    ਆਭਾਸੀ ਅਚਲ char16_t*
    ਵਿਸਤਾਰ_ਤੇਜ਼_Check_Yes(ਅਚਲ char16_t* src U_LIFETIME_BOUND,
                      ਅਚਲ char16_t* ਹੱਦਬੰਦੀ,
                      UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ = 0;

    ਆਭਾਸੀ UNormalization_Check_ਨਤੀਜਾ get_ਤੇਜ਼_Check(UChar32) ਅਚਲ {
        ਵਾਪਸ UNORM_YES;
    }

    ਅਚਲ Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : ਜਨਤਕ Normalizer2WithImpl {
ਜਨਤਕ:
    DecomposeNormalizer2(ਅਚਲ Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    ਆਭਾਸੀ ~DecomposeNormalizer2();

ਨਿਜੀ:
    ਆਭਾਸੀ ਖਾਲੀ
    normalize(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
              Reordering_ਬਫਰ &ਬਫਰ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        impl.decompose(src, ਹੱਦਬੰਦੀ, &ਬਫਰ, error_ਕੋਡ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    ਆਭਾਸੀ ਖਾਲੀ
    normalizeAndAppend(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ, UBool doNormalize,
                       UnicodeString &ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ,
                       Reordering_ਬਫਰ &ਬਫਰ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        impl.decomposeAndAppend(src, ਹੱਦਬੰਦੀ, doNormalize, ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ, ਬਫਰ, error_ਕੋਡ);
    }

    ਖਾਲੀ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        ਜੇ (U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ;
        }
        ਜੇ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ਅਚਲ uint8_t *s = reinterpret_cast<ਅਚਲ uint8_t *>(src.ਅੰਕੜੇ());
        impl.decomposeUTF8(options, s, s + src.ਲੰਮਾਈ(), &sink, edits, error_ਕੋਡ);
        sink.Flush();
    }
    ਆਭਾਸੀ UBool
    isNormalizedUTF8(StringPiece sp, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ false;
        }
        ਅਚਲ uint8_t *s = reinterpret_cast<ਅਚਲ uint8_t *>(sp.ਅੰਕੜੇ());
        ਅਚਲ uint8_t *s_ਹੱਦਬੰਦੀ = s + sp.ਲੰਮਾਈ();
        ਵਾਪਸ s_ਹੱਦਬੰਦੀ == impl.decomposeUTF8(0, s, s_ਹੱਦਬੰਦੀ, nullptr, nullptr, error_ਕੋਡ);
    }

    ਆਭਾਸੀ ਅਚਲ char16_t*
    ਵਿਸਤਾਰ_ਤੇਜ਼_Check_Yes(ਅਚਲ char16_t* src U_LIFETIME_BOUND,
                      ਅਚਲ char16_t* ਹੱਦਬੰਦੀ,
                      UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ override {
        ਵਾਪਸ impl.decompose(src, ਹੱਦਬੰਦੀ, nullptr, error_ਕੋਡ);
    }
    using Normalizer2WithImpl::ਵਿਸਤਾਰ_ਤੇਜ਼_Check_Yes;  // Avoid warning about hiding base class function.
    ਆਭਾਸੀ UNormalization_Check_ਨਤੀਜਾ get_ਤੇਜ਼_Check(UChar32 c) ਅਚਲ override {
        ਵਾਪਸ impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    ਆਭਾਸੀ UBool hasBoundaryBefore(UChar32 c) ਅਚਲ override {
        ਵਾਪਸ impl.hasDecompBoundaryBefore(c);
    }
    ਆਭਾਸੀ UBool hasBoundaryAfter(UChar32 c) ਅਚਲ override {
        ਵਾਪਸ impl.hasDecompBoundaryAfter(c);
    }
    ਆਭਾਸੀ UBool isInert(UChar32 c) ਅਚਲ override {
        ਵਾਪਸ impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : ਜਨਤਕ Normalizer2WithImpl {
ਜਨਤਕ:
    ComposeNormalizer2(ਅਚਲ Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    ਆਭਾਸੀ ~ComposeNormalizer2();

ਨਿਜੀ:
    ਆਭਾਸੀ ਖਾਲੀ
    normalize(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
              Reordering_ਬਫਰ &ਬਫਰ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        impl.compose(src, ਹੱਦਬੰਦੀ, onlyContiguous, true, ਬਫਰ, error_ਕੋਡ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    ਖਾਲੀ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        ਜੇ (U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ;
        }
        ਜੇ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ਅਚਲ uint8_t *s = reinterpret_cast<ਅਚਲ uint8_t *>(src.ਅੰਕੜੇ());
        impl.composeUTF8(options, onlyContiguous, s, s + src.ਲੰਮਾਈ(),
                         &sink, edits, error_ਕੋਡ);
        sink.Flush();
    }

    ਆਭਾਸੀ ਖਾਲੀ
    normalizeAndAppend(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ, UBool doNormalize,
                       UnicodeString &ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ,
                       Reordering_ਬਫਰ &ਬਫਰ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        impl.composeAndAppend(src, ਹੱਦਬੰਦੀ, doNormalize, onlyContiguous, ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ, ਬਫਰ, error_ਕੋਡ);
    }

    ਆਭਾਸੀ UBool
    isNormalized(ਅਚਲ UnicodeString &s, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ false;
        }
        ਅਚਲ char16_t *sArray=s.get_ਬਫਰ();
        ਜੇ(sArray==nullptr) {
            error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ false;
        }
        UnicodeString ਆਰਜ਼ੀ;
        Reordering_ਬਫਰ ਬਫਰ(impl, ਆਰਜ਼ੀ);
        ਜੇ(!ਬਫਰ.init(5, error_ਕੋਡ)) {  // small destCapacity for substring normalization
            ਵਾਪਸ false;
        }
        ਵਾਪਸ impl.compose(sArray, sArray+s.ਲੰਮਾਈ(), onlyContiguous, false, ਬਫਰ, error_ਕੋਡ);
    }
    ਆਭਾਸੀ UBool
    isNormalizedUTF8(StringPiece sp, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ false;
        }
        ਅਚਲ uint8_t *s = reinterpret_cast<ਅਚਲ uint8_t *>(sp.ਅੰਕੜੇ());
        ਵਾਪਸ impl.composeUTF8(0, onlyContiguous, s, s + sp.ਲੰਮਾਈ(), nullptr, nullptr, error_ਕੋਡ);
    }
    ਆਭਾਸੀ UNormalization_Check_ਨਤੀਜਾ
    ਤੇਜ਼_Check(ਅਚਲ UnicodeString &s, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ UNORM_MAYBE;
        }
        ਅਚਲ char16_t *sArray=s.get_ਬਫਰ();
        ਜੇ(sArray==nullptr) {
            error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ UNORM_MAYBE;
        }
        UNormalization_Check_ਨਤੀਜਾ qc_ਨਤੀਜਾ=UNORM_YES;
        impl.compose_ਤੇਜ਼_Check(sArray, sArray+s.ਲੰਮਾਈ(), onlyContiguous, &qc_ਨਤੀਜਾ);
        ਵਾਪਸ qc_ਨਤੀਜਾ;
    }
    ਆਭਾਸੀ ਅਚਲ char16_t*
    ਵਿਸਤਾਰ_ਤੇਜ਼_Check_Yes(ਅਚਲ char16_t* src U_LIFETIME_BOUND,
                      ਅਚਲ char16_t* ਹੱਦਬੰਦੀ,
                      UError_ਕੋਡ&) ਅਚਲ override {
        ਵਾਪਸ impl.compose_ਤੇਜ਼_Check(src, ਹੱਦਬੰਦੀ, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::ਵਿਸਤਾਰ_ਤੇਜ਼_Check_Yes;  // Avoid warning about hiding base class function.
    ਆਭਾਸੀ UNormalization_Check_ਨਤੀਜਾ get_ਤੇਜ਼_Check(UChar32 c) ਅਚਲ override {
        ਵਾਪਸ impl.get_Comp_ਤੇਜ਼_Check(impl.getNorm16(c));
    }
    ਆਭਾਸੀ UBool hasBoundaryBefore(UChar32 c) ਅਚਲ override {
        ਵਾਪਸ impl.hasCompBoundaryBefore(c);
    }
    ਆਭਾਸੀ UBool hasBoundaryAfter(UChar32 c) ਅਚਲ override {
        ਵਾਪਸ impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    ਆਭਾਸੀ UBool isInert(UChar32 c) ਅਚਲ override {
        ਵਾਪਸ impl.isCompInert(c, onlyContiguous);
    }

    ਅਚਲ UBool onlyContiguous;
};

class FCDNormalizer2 : ਜਨਤਕ Normalizer2WithImpl {
ਜਨਤਕ:
    FCDNormalizer2(ਅਚਲ Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    ਆਭਾਸੀ ~FCDNormalizer2();

ਨਿਜੀ:
    ਆਭਾਸੀ ਖਾਲੀ
    normalize(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ,
              Reordering_ਬਫਰ &ਬਫਰ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        impl.makeFCD(src, ਹੱਦਬੰਦੀ, &ਬਫਰ, error_ਕੋਡ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    ਆਭਾਸੀ ਖਾਲੀ
    normalizeAndAppend(ਅਚਲ char16_t *src, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ, UBool doNormalize,
                       UnicodeString &ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ,
                       Reordering_ਬਫਰ &ਬਫਰ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override {
        impl.makeFCDAndAppend(src, ਹੱਦਬੰਦੀ, doNormalize, ਸੁਰੱਖਿਅਤ_ਵਿਚਕਾਰ, ਬਫਰ, error_ਕੋਡ);
    }
    ਆਭਾਸੀ ਅਚਲ char16_t*
    ਵਿਸਤਾਰ_ਤੇਜ਼_Check_Yes(ਅਚਲ char16_t* src U_LIFETIME_BOUND,
                      ਅਚਲ char16_t* ਹੱਦਬੰਦੀ,
                      UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ override {
        ਵਾਪਸ impl.makeFCD(src, ਹੱਦਬੰਦੀ, nullptr, error_ਕੋਡ);
    }
    using Normalizer2WithImpl::ਵਿਸਤਾਰ_ਤੇਜ਼_Check_Yes;  // Avoid warning about hiding base class function.
    ਆਭਾਸੀ UBool hasBoundaryBefore(UChar32 c) ਅਚਲ override {
        ਵਾਪਸ impl.hasFCDBoundaryBefore(c);
    }
    ਆਭਾਸੀ UBool hasBoundaryAfter(UChar32 c) ਅਚਲ override {
        ਵਾਪਸ impl.hasFCDBoundaryAfter(c);
    }
    ਆਭਾਸੀ UBool isInert(UChar32 c) ਅਚਲ override {
        ਵਾਪਸ impl.isFCDInert(c);
    }
};

ਬਣਤਰ Norm2AllModes : ਜਨਤਕ UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    ਸਥਿਰ Norm2AllModes *createInstance(Normalizer2Impl *impl, UError_ਕੋਡ &error_ਕੋਡ);
    ਸਥਿਰ Norm2AllModes *createNFCInstance(UError_ਕੋਡ &error_ਕੋਡ);
    ਸਥਿਰ Norm2AllModes *createInstance(ਅਚਲ ਅੱਖਰ *packageName,
                                         ਅਚਲ ਅੱਖਰ *name,
                                         UError_ਕੋਡ &error_ਕੋਡ);

    ਸਥਿਰ ਅਚਲ Norm2AllModes *getNFCInstance(UError_ਕੋਡ &error_ਕੋਡ);
    ਸਥਿਰ ਅਚਲ Norm2AllModes *getNFKCInstance(UError_ਕੋਡ &error_ਕੋਡ);
    ਸਥਿਰ ਅਚਲ Norm2AllModes *getNFKC_CFInstance(UError_ਕੋਡ &error_ਕੋਡ);
    ਸਥਿਰ ਅਚਲ Norm2AllModes *getNFKC_SCFInstance(UError_ਕੋਡ &error_ਕੋਡ);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // !UCONFIG_NO_NORMALIZATION
#ਜੇ_ਅੰਤ  // __NORM2ALLMODES_H__
