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

#ifndef __NORM2ALLMODES_H__
#define __NORM2ALLMODES_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_NORMALIZATION

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
    Normalizer2WithImpl(ಧ್ರುವ Normalizer2Impl &ni) : impl(ni) {}
    ಆಭಾಸ ~Normalizer2WithImpl();

    // normalize
    ಆಭಾಸ UnicodeString&
    normalize(ಧ್ರುವ UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) ಧ್ರುವ override {
        ಯದಿ(U_FAILURE(errorCode)) {
            dest.setToBogus();
            ಪ್ರತ್ಯಾಗಮ dest;
        }
        ಧ್ರುವ char16_t *sArray=src.getBuffer();
        ಯದಿ(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            ಪ್ರತ್ಯಾಗಮ dest;
        }
        dest.remove();
        ReorderingBuffer buffer(impl, dest);
        ಯದಿ(buffer.init(src.length(), errorCode)) {
            normalize(sArray, sArray+src.length(), buffer, errorCode);
        }
        ಪ್ರತ್ಯಾಗಮ dest;
    }
    ಆಭಾಸ ಶೂನ್ಯ
    normalize(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ಧ್ರುವ = 0;

    // normalize and append
    ಆಭಾಸ UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ಧ್ರುವ UnicodeString& second,
                             UErrorCode& errorCode) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ normalizeSecondAndAppend(first, second, true, errorCode);
    }
    ಆಭಾಸ UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           ಧ್ರುವ UnicodeString& second,
           UErrorCode& errorCode) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ಧ್ರುವ UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) ಧ್ರುವ {
        uprv_checkCanGetBuffer(first, errorCode);
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ first;
        }
        ಧ್ರುವ char16_t *secondArray=second.getBuffer();
        ಯದಿ(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ first;
        }
        int32_t firstLength=first.length();
        UnicodeString safe_ಮಧ್ಯ;
        {
            ReorderingBuffer buffer(impl, first);
            ಯದಿ(buffer.init(firstLength+second.length(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.length(), doNormalize,
                                   safe_ಮಧ್ಯ, buffer, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        ಯದಿ(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(firstLength-safe_ಮಧ್ಯ.length(), 0x7fffffff, safe_ಮಧ್ಯ);
        }
        ಪ್ರತ್ಯಾಗಮ first;
    }
    ಆಭಾಸ ಶೂನ್ಯ
    normalizeAndAppend(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_ಮಧ್ಯ,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ಧ್ರುವ = 0;
    ಆಭಾಸ UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ಧ್ರುವ override {
        char16_t buffer[4];
        int32_t length;
        ಧ್ರುವ char16_t *d=impl.getDecomposition(c, buffer, length);
        ಯದಿ(d==nullptr) {
            ಪ್ರತ್ಯಾಗಮ false;
        }
        ಯದಿ(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (Jamos from Hangul syllable c)
        } ಅನ್ಯಥಾ {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        ಪ್ರತ್ಯಾಗಮ true;
    }
    ಆಭಾಸ UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ಧ್ರುವ override {
        char16_t buffer[30];
        int32_t length;
        ಧ್ರುವ char16_t *d=impl.getRawDecomposition(c, buffer, length);
        ಯದಿ(d==nullptr) {
            ಪ್ರತ್ಯಾಗಮ false;
        }
        ಯದಿ(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (algorithmic decomposition)
        } ಅನ್ಯಥಾ {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        ಪ್ರತ್ಯಾಗಮ true;
    }
    ಆಭಾಸ UChar32
    composePair(UChar32 a, UChar32 b) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.composePair(a, b);
    }

    ಆಭಾಸ uint8_t
    getCombiningClass(UChar32 c) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    ಆಭಾಸ UBool
    isNormalized(ಧ್ರುವ UnicodeString &s, UErrorCode &errorCode) ಧ್ರುವ override {
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ false;
        }
        ಧ್ರುವ char16_t *sArray=s.getBuffer();
        ಯದಿ(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ false;
        }
        ಧ್ರುವ char16_t *sLimit=sArray+s.length();
        ಪ್ರತ್ಯಾಗಮ sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    ಆಭಾಸ UNormalizationCheckResult
    quickCheck(ಧ್ರುವ UnicodeString &s, UErrorCode &errorCode) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    ಆಭಾಸ int32_t
    spanQuickCheckYes(ಧ್ರುವ UnicodeString &s, UErrorCode &errorCode) ಧ್ರುವ override {
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಧ್ರುವ char16_t *sArray=s.getBuffer();
        ಯದಿ(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.length(), errorCode) - sArray);
    }
    ಆಭಾಸ ಧ್ರುವ char16_t*
    spanQuickCheckYes(ಧ್ರುವ char16_t* src U_LIFETIME_BOUND,
                      ಧ್ರುವ char16_t* limit,
                      UErrorCode& errorCode) ಧ್ರುವ = 0;

    ಆಭಾಸ UNormalizationCheckResult getQuickCheck(UChar32) ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ UNORM_YES;
    }

    ಧ್ರುವ Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : ಸಾರ್ವಜನಿಕ Normalizer2WithImpl {
ಸಾರ್ವಜನಿಕ:
    DecomposeNormalizer2(ಧ್ರುವ Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    ಆಭಾಸ ~DecomposeNormalizer2();

ಗುಪ್ತ:
    ಆಭಾಸ ಶೂನ್ಯ
    normalize(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ಧ್ರುವ override {
        impl.decompose(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    ಆಭಾಸ ಶೂನ್ಯ
    normalizeAndAppend(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_ಮಧ್ಯ,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ಧ್ರುವ override {
        impl.decomposeAndAppend(src, limit, doNormalize, safe_ಮಧ್ಯ, buffer, errorCode);
    }

    ಶೂನ್ಯ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ಧ್ರುವ override {
        ಯದಿ (U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ಧ್ರುವ uint8_t *s = reinterpret_cast<ಧ್ರುವ uint8_t *>(src.data());
        impl.decomposeUTF8(options, s, s + src.length(), &sink, edits, errorCode);
        sink.Flush();
    }
    ಆಭಾಸ UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ಧ್ರುವ override {
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ false;
        }
        ಧ್ರುವ uint8_t *s = reinterpret_cast<ಧ್ರುವ uint8_t *>(sp.data());
        ಧ್ರುವ uint8_t *sLimit = s + sp.length();
        ಪ್ರತ್ಯಾಗಮ sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    ಆಭಾಸ ಧ್ರುವ char16_t*
    spanQuickCheckYes(ಧ್ರುವ char16_t* src U_LIFETIME_BOUND,
                      ಧ್ರುವ char16_t* limit,
                      UErrorCode& errorCode) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    ಆಭಾಸ UNormalizationCheckResult getQuickCheck(UChar32 c) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    ಆಭಾಸ UBool hasBoundaryBefore(UChar32 c) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.hasDecompBoundaryBefore(c);
    }
    ಆಭಾಸ UBool hasBoundaryAfter(UChar32 c) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.hasDecompBoundaryAfter(c);
    }
    ಆಭಾಸ UBool isInert(UChar32 c) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : ಸಾರ್ವಜನಿಕ Normalizer2WithImpl {
ಸಾರ್ವಜನಿಕ:
    ComposeNormalizer2(ಧ್ರುವ Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    ಆಭಾಸ ~ComposeNormalizer2();

ಗುಪ್ತ:
    ಆಭಾಸ ಶೂನ್ಯ
    normalize(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ಧ್ರುವ override {
        impl.compose(src, limit, onlyContiguous, true, buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    ಶೂನ್ಯ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ಧ್ರುವ override {
        ಯದಿ (U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ಧ್ರುವ uint8_t *s = reinterpret_cast<ಧ್ರುವ uint8_t *>(src.data());
        impl.composeUTF8(options, onlyContiguous, s, s + src.length(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    ಆಭಾಸ ಶೂನ್ಯ
    normalizeAndAppend(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_ಮಧ್ಯ,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ಧ್ರುವ override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safe_ಮಧ್ಯ, buffer, errorCode);
    }

    ಆಭಾಸ UBool
    isNormalized(ಧ್ರುವ UnicodeString &s, UErrorCode &errorCode) ಧ್ರುವ override {
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ false;
        }
        ಧ್ರುವ char16_t *sArray=s.getBuffer();
        ಯದಿ(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ false;
        }
        UnicodeString temp;
        ReorderingBuffer buffer(impl, temp);
        ಯದಿ(!buffer.init(5, errorCode)) {  // small destCapacity for substring normalization
            ಪ್ರತ್ಯಾಗಮ false;
        }
        ಪ್ರತ್ಯಾಗಮ impl.compose(sArray, sArray+s.length(), onlyContiguous, false, buffer, errorCode);
    }
    ಆಭಾಸ UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ಧ್ರುವ override {
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ false;
        }
        ಧ್ರುವ uint8_t *s = reinterpret_cast<ಧ್ರುವ uint8_t *>(sp.data());
        ಪ್ರತ್ಯಾಗಮ impl.composeUTF8(0, onlyContiguous, s, s + sp.length(), nullptr, nullptr, errorCode);
    }
    ಆಭಾಸ UNormalizationCheckResult
    quickCheck(ಧ್ರುವ UnicodeString &s, UErrorCode &errorCode) ಧ್ರುವ override {
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ UNORM_MAYBE;
        }
        ಧ್ರುವ char16_t *sArray=s.getBuffer();
        ಯದಿ(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ UNORM_MAYBE;
        }
        UNormalizationCheckResult qcResult=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.length(), onlyContiguous, &qcResult);
        ಪ್ರತ್ಯಾಗಮ qcResult;
    }
    ಆಭಾಸ ಧ್ರುವ char16_t*
    spanQuickCheckYes(ಧ್ರುವ char16_t* src U_LIFETIME_BOUND,
                      ಧ್ರುವ char16_t* limit,
                      UErrorCode&) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    ಆಭಾಸ UNormalizationCheckResult getQuickCheck(UChar32 c) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.getCompQuickCheck(impl.getNorm16(c));
    }
    ಆಭಾಸ UBool hasBoundaryBefore(UChar32 c) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.hasCompBoundaryBefore(c);
    }
    ಆಭಾಸ UBool hasBoundaryAfter(UChar32 c) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    ಆಭಾಸ UBool isInert(UChar32 c) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.isCompInert(c, onlyContiguous);
    }

    ಧ್ರುವ UBool onlyContiguous;
};

class FCDNormalizer2 : ಸಾರ್ವಜನಿಕ Normalizer2WithImpl {
ಸಾರ್ವಜನಿಕ:
    FCDNormalizer2(ಧ್ರುವ Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    ಆಭಾಸ ~FCDNormalizer2();

ಗುಪ್ತ:
    ಆಭಾಸ ಶೂನ್ಯ
    normalize(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ಧ್ರುವ override {
        impl.makeFCD(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    ಆಭಾಸ ಶೂನ್ಯ
    normalizeAndAppend(ಧ್ರುವ char16_t *src, ಧ್ರುವ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_ಮಧ್ಯ,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ಧ್ರುವ override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safe_ಮಧ್ಯ, buffer, errorCode);
    }
    ಆಭಾಸ ಧ್ರುವ char16_t*
    spanQuickCheckYes(ಧ್ರುವ char16_t* src U_LIFETIME_BOUND,
                      ಧ್ರುವ char16_t* limit,
                      UErrorCode& errorCode) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    ಆಭಾಸ UBool hasBoundaryBefore(UChar32 c) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.hasFCDBoundaryBefore(c);
    }
    ಆಭಾಸ UBool hasBoundaryAfter(UChar32 c) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.hasFCDBoundaryAfter(c);
    }
    ಆಭಾಸ UBool isInert(UChar32 c) ಧ್ರುವ override {
        ಪ್ರತ್ಯಾಗಮ impl.isFCDInert(c);
    }
};

ಸಂರಚನಾ Norm2AllModes : ಸಾರ್ವಜನಿಕ UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    ಸ್ಥಿರ Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    ಸ್ಥಿರ Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    ಸ್ಥಿರ Norm2AllModes *createInstance(ಧ್ರುವ ವರ್ಣ *packageName,
                                         ಧ್ರುವ ವರ್ಣ *name,
                                         UErrorCode &errorCode);

    ಸ್ಥಿರ ಧ್ರುವ Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    ಸ್ಥಿರ ಧ್ರುವ Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    ಸ್ಥಿರ ಧ್ರುವ Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    ಸ್ಥಿರ ಧ್ರುವ Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
