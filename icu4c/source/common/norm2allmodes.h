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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __NORM2ALLMODES_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __NORM2ALLMODES_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_NORMALIZATION

#ಅನ್ತರ್ಭಾವ "unicode/edits.h"
#ಅನ್ತರ್ಭಾವ "unicode/normalizer2.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringoptions.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "cpputils.h"
#ಅನ್ತರ್ಭಾವ "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : ಸಾರ್ವಜನಿಕ Normalizer2 {
ಸಾರ್ವಜನಿಕ:
    Normalizer2WithImpl(ಸ್ಥಿರಾಂಕ Normalizer2Impl &ni) : impl(ni) {}
    ವಾಸ್ತವಿಕ ~Normalizer2WithImpl();

    // normalize
    ವಾಸ್ತವಿಕ UnicodeString&
    normalize(ಸ್ಥಿರಾಂಕ UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            dest.ಗಣ_ಗಮ್ಯ_Bogus();
            ಹಿಂತಿರುಗಿ dest;
        }
        ಸ್ಥಿರಾಂಕ char16_t *sArray=src.get_ಬಫರ್();
        ಆದರೆ(&dest==&src || sArray==nullptr) {
            error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
            dest.ಗಣ_ಗಮ್ಯ_Bogus();
            ಹಿಂತಿರುಗಿ dest;
        }
        dest.remove();
        Reordering_ಬಫರ್ ಬಫರ್(impl, dest);
        ಆದರೆ(ಬಫರ್.init(src.ಉದ್ದಳತೆ(), error_ಸಂಕೇತ)) {
            normalize(sArray, sArray+src.ಉದ್ದಳತೆ(), ಬಫರ್, error_ಸಂಕೇತ);
        }
        ಹಿಂತಿರುಗಿ dest;
    }
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ
    normalize(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
              Reordering_ಬಫರ್ &ಬಫರ್, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;

    // normalize and append
    ವಾಸ್ತವಿಕ UnicodeString&
    normalize_ಎರಡನೇ_And_Append(UnicodeString& ಮೊದಲ U_LIFETIME_BOUND,
                             ಸ್ಥಿರಾಂಕ UnicodeString& ಎರಡನೇ,
                             UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ normalize_ಎರಡನೇ_And_Append(ಮೊದಲ, ಎರಡನೇ, true, error_ಸಂಕೇತ);
    }
    ವಾಸ್ತವಿಕ UnicodeString&
    append(UnicodeString& ಮೊದಲ U_LIFETIME_BOUND,
           ಸ್ಥಿರಾಂಕ UnicodeString& ಎರಡನೇ,
           UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ normalize_ಎರಡನೇ_And_Append(ಮೊದಲ, ಎರಡನೇ, false, error_ಸಂಕೇತ);
    }
    UnicodeString&
    normalize_ಎರಡನೇ_And_Append(UnicodeString& ಮೊದಲ U_LIFETIME_BOUND,
                             ಸ್ಥಿರಾಂಕ UnicodeString& ಎರಡನೇ,
                             UBool doNormalize,
                             UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
        uprv_check_Can_Get_ಬಫರ್(ಮೊದಲ, error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ ಮೊದಲ;
        }
        ಸ್ಥಿರಾಂಕ char16_t *ಎರಡನೇ_Array=ಎರಡನೇ.get_ಬಫರ್();
        ಆದರೆ(&ಮೊದಲ==&ಎರಡನೇ || ಎರಡನೇ_Array==nullptr) {
            error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ ಮೊದಲ;
        }
        int32_t ಮೊದಲ_ಉದ್ದಳತೆ=ಮೊದಲ.ಉದ್ದಳತೆ();
        UnicodeString ಸುರಕ್ಷಿತ_ಮಧ್ಯ;
        {
            Reordering_ಬಫರ್ ಬಫರ್(impl, ಮೊದಲ);
            ಆದರೆ(ಬಫರ್.init(ಮೊದಲ_ಉದ್ದಳತೆ+ಎರಡನೇ.ಉದ್ದಳತೆ(), error_ಸಂಕೇತ)) {
                normalizeAndAppend(ಎರಡನೇ_Array, ಎರಡನೇ_Array+ಎರಡನೇ.ಉದ್ದಳತೆ(), doNormalize,
                                   ಸುರಕ್ಷಿತ_ಮಧ್ಯ, ಬಫರ್, error_ಸಂಕೇತ);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            // Restore the modified suffix of the first string.
            ಮೊದಲ.replace(ಮೊದಲ_ಉದ್ದಳತೆ-ಸುರಕ್ಷಿತ_ಮಧ್ಯ.ಉದ್ದಳತೆ(), 0x7fffffff, ಸುರಕ್ಷಿತ_ಮಧ್ಯ);
        }
        ಹಿಂತಿರುಗಿ ಮೊದಲ;
    }
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ
    normalizeAndAppend(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ, UBool doNormalize,
                       UnicodeString &ಸುರಕ್ಷಿತ_ಮಧ್ಯ,
                       Reordering_ಬಫರ್ &ಬಫರ್, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;
    ವಾಸ್ತವಿಕ UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ಸ್ಥಿರಾಂಕ override {
        char16_t ಬಫರ್[4];
        int32_t ಉದ್ದಳತೆ;
        ಸ್ಥಿರಾಂಕ char16_t *d=impl.getDecomposition(c, ಬಫರ್, ಉದ್ದಳತೆ);
        ಆದರೆ(d==nullptr) {
            ಹಿಂತಿರುಗಿ false;
        }
        ಆದರೆ(d==ಬಫರ್) {
            decomposition.ಗಣ_ಗಮ್ಯ(ಬಫರ್, ಉದ್ದಳತೆ);  // copy the string (Jamos from Hangul syllable c)
        } ಇಲ್ಲದಿದ್ದರೆ {
            decomposition.ಗಣ_ಗಮ್ಯ(false, d, ಉದ್ದಳತೆ);  // read-only alias
        }
        ಹಿಂತಿರುಗಿ true;
    }
    ವಾಸ್ತವಿಕ UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ಸ್ಥಿರಾಂಕ override {
        char16_t ಬಫರ್[30];
        int32_t ಉದ್ದಳತೆ;
        ಸ್ಥಿರಾಂಕ char16_t *d=impl.getRawDecomposition(c, ಬಫರ್, ಉದ್ದಳತೆ);
        ಆದರೆ(d==nullptr) {
            ಹಿಂತಿರುಗಿ false;
        }
        ಆದರೆ(d==ಬಫರ್) {
            decomposition.ಗಣ_ಗಮ್ಯ(ಬಫರ್, ಉದ್ದಳತೆ);  // copy the string (algorithmic decomposition)
        } ಇಲ್ಲದಿದ್ದರೆ {
            decomposition.ಗಣ_ಗಮ್ಯ(false, d, ಉದ್ದಳತೆ);  // read-only alias
        }
        ಹಿಂತಿರುಗಿ true;
    }
    ವಾಸ್ತವಿಕ UChar32
    composePair(UChar32 a, UChar32 b) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.composePair(a, b);
    }

    ವಾಸ್ತವಿಕ uint8_t
    getCombiningClass(UChar32 c) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    ವಾಸ್ತವಿಕ UBool
    isNormalized(ಸ್ಥಿರಾಂಕ UnicodeString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ false;
        }
        ಸ್ಥಿರಾಂಕ char16_t *sArray=s.get_ಬಫರ್();
        ಆದರೆ(sArray==nullptr) {
            error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ false;
        }
        ಸ್ಥಿರಾಂಕ char16_t *s_ಎಲ್ಲೆ=sArray+s.ಉದ್ದಳತೆ();
        ಹಿಂತಿರುಗಿ s_ಎಲ್ಲೆ==ಹರವು_ಶೀಘ್ರ_Check_Yes(sArray, s_ಎಲ್ಲೆ, error_ಸಂಕೇತ);
    }
    ವಾಸ್ತವಿಕ UNormalization_Check_ಫಲಿತಾಂಶ
    ಶೀಘ್ರ_Check(ಸ್ಥಿರಾಂಕ UnicodeString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ Normalizer2WithImpl::isNormalized(s, error_ಸಂಕೇತ) ? UNORM_YES : UNORM_NO;
    }
    ವಾಸ್ತವಿಕ int32_t
    ಹರವು_ಶೀಘ್ರ_Check_Yes(ಸ್ಥಿರಾಂಕ UnicodeString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ 0;
        }
        ಸ್ಥಿರಾಂಕ char16_t *sArray=s.get_ಬಫರ್();
        ಆದರೆ(sArray==nullptr) {
            error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        ಹಿಂತಿರುಗಿ static_cast<int32_t>(ಹರವು_ಶೀಘ್ರ_Check_Yes(sArray, sArray + s.ಉದ್ದಳತೆ(), error_ಸಂಕೇತ) - sArray);
    }
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ char16_t*
    ಹರವು_ಶೀಘ್ರ_Check_Yes(ಸ್ಥಿರಾಂಕ char16_t* src U_LIFETIME_BOUND,
                      ಸ್ಥಿರಾಂಕ char16_t* ಎಲ್ಲೆ,
                      UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;

    ವಾಸ್ತವಿಕ UNormalization_Check_ಫಲಿತಾಂಶ get_ಶೀಘ್ರ_Check(UChar32) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ UNORM_YES;
    }

    ಸ್ಥಿರಾಂಕ Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : ಸಾರ್ವಜನಿಕ Normalizer2WithImpl {
ಸಾರ್ವಜನಿಕ:
    DecomposeNormalizer2(ಸ್ಥಿರಾಂಕ Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    ವಾಸ್ತವಿಕ ~DecomposeNormalizer2();

ಖಾಸಗಿ:
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ
    normalize(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
              Reordering_ಬಫರ್ &ಬಫರ್, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        impl.decompose(src, ಎಲ್ಲೆ, &ಬಫರ್, error_ಸಂಕೇತ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ
    normalizeAndAppend(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ, UBool doNormalize,
                       UnicodeString &ಸುರಕ್ಷಿತ_ಮಧ್ಯ,
                       Reordering_ಬಫರ್ &ಬಫರ್, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        impl.decomposeAndAppend(src, ಎಲ್ಲೆ, doNormalize, ಸುರಕ್ಷಿತ_ಮಧ್ಯ, ಬಫರ್, error_ಸಂಕೇತ);
    }

    ಶೂನ್ಯ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ಸ್ಥಿರಾಂಕ uint8_t *s = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t *>(src.ದತ್ತಾಂಶ());
        impl.decomposeUTF8(options, s, s + src.ಉದ್ದಳತೆ(), &sink, edits, error_ಸಂಕೇತ);
        sink.Flush();
    }
    ವಾಸ್ತವಿಕ UBool
    isNormalizedUTF8(StringPiece sp, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ false;
        }
        ಸ್ಥಿರಾಂಕ uint8_t *s = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t *>(sp.ದತ್ತಾಂಶ());
        ಸ್ಥಿರಾಂಕ uint8_t *s_ಎಲ್ಲೆ = s + sp.ಉದ್ದಳತೆ();
        ಹಿಂತಿರುಗಿ s_ಎಲ್ಲೆ == impl.decomposeUTF8(0, s, s_ಎಲ್ಲೆ, nullptr, nullptr, error_ಸಂಕೇತ);
    }

    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ char16_t*
    ಹರವು_ಶೀಘ್ರ_Check_Yes(ಸ್ಥಿರಾಂಕ char16_t* src U_LIFETIME_BOUND,
                      ಸ್ಥಿರಾಂಕ char16_t* ಎಲ್ಲೆ,
                      UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.decompose(src, ಎಲ್ಲೆ, nullptr, error_ಸಂಕೇತ);
    }
    using Normalizer2WithImpl::ಹರವು_ಶೀಘ್ರ_Check_Yes;  // Avoid warning about hiding base class function.
    ವಾಸ್ತವಿಕ UNormalization_Check_ಫಲಿತಾಂಶ get_ಶೀಘ್ರ_Check(UChar32 c) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    ವಾಸ್ತವಿಕ UBool hasBoundaryBefore(UChar32 c) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.hasDecompBoundaryBefore(c);
    }
    ವಾಸ್ತವಿಕ UBool hasBoundaryAfter(UChar32 c) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.hasDecompBoundaryAfter(c);
    }
    ವಾಸ್ತವಿಕ UBool isInert(UChar32 c) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : ಸಾರ್ವಜನಿಕ Normalizer2WithImpl {
ಸಾರ್ವಜನಿಕ:
    ComposeNormalizer2(ಸ್ಥಿರಾಂಕ Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    ವಾಸ್ತವಿಕ ~ComposeNormalizer2();

ಖಾಸಗಿ:
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ
    normalize(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
              Reordering_ಬಫರ್ &ಬಫರ್, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        impl.compose(src, ಎಲ್ಲೆ, onlyContiguous, true, ಬಫರ್, error_ಸಂಕೇತ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    ಶೂನ್ಯ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ಸ್ಥಿರಾಂಕ uint8_t *s = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t *>(src.ದತ್ತಾಂಶ());
        impl.composeUTF8(options, onlyContiguous, s, s + src.ಉದ್ದಳತೆ(),
                         &sink, edits, error_ಸಂಕೇತ);
        sink.Flush();
    }

    ವಾಸ್ತವಿಕ ಶೂನ್ಯ
    normalizeAndAppend(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ, UBool doNormalize,
                       UnicodeString &ಸುರಕ್ಷಿತ_ಮಧ್ಯ,
                       Reordering_ಬಫರ್ &ಬಫರ್, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        impl.composeAndAppend(src, ಎಲ್ಲೆ, doNormalize, onlyContiguous, ಸುರಕ್ಷಿತ_ಮಧ್ಯ, ಬಫರ್, error_ಸಂಕೇತ);
    }

    ವಾಸ್ತವಿಕ UBool
    isNormalized(ಸ್ಥಿರಾಂಕ UnicodeString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ false;
        }
        ಸ್ಥಿರಾಂಕ char16_t *sArray=s.get_ಬಫರ್();
        ಆದರೆ(sArray==nullptr) {
            error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ false;
        }
        UnicodeString ತಾತ್ಕಾಲಿಕ;
        Reordering_ಬಫರ್ ಬಫರ್(impl, ತಾತ್ಕಾಲಿಕ);
        ಆದರೆ(!ಬಫರ್.init(5, error_ಸಂಕೇತ)) {  // small destCapacity for substring normalization
            ಹಿಂತಿರುಗಿ false;
        }
        ಹಿಂತಿರುಗಿ impl.compose(sArray, sArray+s.ಉದ್ದಳತೆ(), onlyContiguous, false, ಬಫರ್, error_ಸಂಕೇತ);
    }
    ವಾಸ್ತವಿಕ UBool
    isNormalizedUTF8(StringPiece sp, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ false;
        }
        ಸ್ಥಿರಾಂಕ uint8_t *s = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t *>(sp.ದತ್ತಾಂಶ());
        ಹಿಂತಿರುಗಿ impl.composeUTF8(0, onlyContiguous, s, s + sp.ಉದ್ದಳತೆ(), nullptr, nullptr, error_ಸಂಕೇತ);
    }
    ವಾಸ್ತವಿಕ UNormalization_Check_ಫಲಿತಾಂಶ
    ಶೀಘ್ರ_Check(ಸ್ಥಿರಾಂಕ UnicodeString &s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ UNORM_MAYBE;
        }
        ಸ್ಥಿರಾಂಕ char16_t *sArray=s.get_ಬಫರ್();
        ಆದರೆ(sArray==nullptr) {
            error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ UNORM_MAYBE;
        }
        UNormalization_Check_ಫಲಿತಾಂಶ qc_ಫಲಿತಾಂಶ=UNORM_YES;
        impl.compose_ಶೀಘ್ರ_Check(sArray, sArray+s.ಉದ್ದಳತೆ(), onlyContiguous, &qc_ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ qc_ಫಲಿತಾಂಶ;
    }
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ char16_t*
    ಹರವು_ಶೀಘ್ರ_Check_Yes(ಸ್ಥಿರಾಂಕ char16_t* src U_LIFETIME_BOUND,
                      ಸ್ಥಿರಾಂಕ char16_t* ಎಲ್ಲೆ,
                      UError_ಸಂಕೇತ&) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.compose_ಶೀಘ್ರ_Check(src, ಎಲ್ಲೆ, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::ಹರವು_ಶೀಘ್ರ_Check_Yes;  // Avoid warning about hiding base class function.
    ವಾಸ್ತವಿಕ UNormalization_Check_ಫಲಿತಾಂಶ get_ಶೀಘ್ರ_Check(UChar32 c) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.get_Comp_ಶೀಘ್ರ_Check(impl.getNorm16(c));
    }
    ವಾಸ್ತವಿಕ UBool hasBoundaryBefore(UChar32 c) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.hasCompBoundaryBefore(c);
    }
    ವಾಸ್ತವಿಕ UBool hasBoundaryAfter(UChar32 c) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    ವಾಸ್ತವಿಕ UBool isInert(UChar32 c) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.isCompInert(c, onlyContiguous);
    }

    ಸ್ಥಿರಾಂಕ UBool onlyContiguous;
};

class FCDNormalizer2 : ಸಾರ್ವಜನಿಕ Normalizer2WithImpl {
ಸಾರ್ವಜನಿಕ:
    FCDNormalizer2(ಸ್ಥಿರಾಂಕ Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    ವಾಸ್ತವಿಕ ~FCDNormalizer2();

ಖಾಸಗಿ:
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ
    normalize(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ,
              Reordering_ಬಫರ್ &ಬಫರ್, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        impl.makeFCD(src, ಎಲ್ಲೆ, &ಬಫರ್, error_ಸಂಕೇತ);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ
    normalizeAndAppend(ಸ್ಥಿರಾಂಕ char16_t *src, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ, UBool doNormalize,
                       UnicodeString &ಸುರಕ್ಷಿತ_ಮಧ್ಯ,
                       Reordering_ಬಫರ್ &ಬಫರ್, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        impl.makeFCDAndAppend(src, ಎಲ್ಲೆ, doNormalize, ಸುರಕ್ಷಿತ_ಮಧ್ಯ, ಬಫರ್, error_ಸಂಕೇತ);
    }
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ char16_t*
    ಹರವು_ಶೀಘ್ರ_Check_Yes(ಸ್ಥಿರಾಂಕ char16_t* src U_LIFETIME_BOUND,
                      ಸ್ಥಿರಾಂಕ char16_t* ಎಲ್ಲೆ,
                      UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.makeFCD(src, ಎಲ್ಲೆ, nullptr, error_ಸಂಕೇತ);
    }
    using Normalizer2WithImpl::ಹರವು_ಶೀಘ್ರ_Check_Yes;  // Avoid warning about hiding base class function.
    ವಾಸ್ತವಿಕ UBool hasBoundaryBefore(UChar32 c) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.hasFCDBoundaryBefore(c);
    }
    ವಾಸ್ತವಿಕ UBool hasBoundaryAfter(UChar32 c) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.hasFCDBoundaryAfter(c);
    }
    ವಾಸ್ತವಿಕ UBool isInert(UChar32 c) ಸ್ಥಿರಾಂಕ override {
        ಹಿಂತಿರುಗಿ impl.isFCDInert(c);
    }
};

ರಚನೆ Norm2AllModes : ಸಾರ್ವಜನಿಕ UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    ಸ್ಥಿರ Norm2AllModes *createInstance(Normalizer2Impl *impl, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    ಸ್ಥಿರ Norm2AllModes *createNFCInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    ಸ್ಥಿರ Norm2AllModes *createInstance(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *packageName,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                         UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Norm2AllModes *getNFCInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Norm2AllModes *getNFKCInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Norm2AllModes *getNFKC_CFInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Norm2AllModes *getNFKC_SCFInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // !UCONFIG_NO_NORMALIZATION
#ಆದರೆ_ಅಂತ್ಯ  // __NORM2ALLMODES_H__
