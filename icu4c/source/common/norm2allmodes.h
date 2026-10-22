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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_NORMALIZATION

#অন্তর্ভাৱ "unicode/edits.h"
#অন্তর্ভাৱ "unicode/normalizer2.h"
#অন্তর্ভাৱ "unicode/stringoptions.h"
#অন্তর্ভাৱ "unicode/unistr.h"
#অন্তর্ভাৱ "cpputils.h"
#অন্তর্ভাৱ "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : সার্ৱজনিক Normalizer2 {
সার্ৱজনিক:
    Normalizer2WithImpl(ধ্রুৱ Normalizer2Impl &ni) : impl(ni) {}
    আভাস ~Normalizer2WithImpl();

    // normalize
    আভাস UnicodeString&
    normalize(ধ্রুৱ UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) ধ্রুৱ override {
        যদি(U_FAILURE(errorCode)) {
            dest.setToBogus();
            ঘূৰাই dest;
        }
        ধ্রুৱ char16_t *sArray=src.getBuffer();
        যদি(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            ঘূৰাই dest;
        }
        dest.remove();
        ReorderingBuffer buffer(impl, dest);
        যদি(buffer.init(src.length(), errorCode)) {
            normalize(sArray, sArray+src.length(), buffer, errorCode);
        }
        ঘূৰাই dest;
    }
    আভাস শূন্য
    normalize(ধ্রুৱ char16_t *src, ধ্রুৱ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুৱ = 0;

    // normalize and append
    আভাস UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ধ্রুৱ UnicodeString& second,
                             UErrorCode& errorCode) ধ্রুৱ override {
        ঘূৰাই normalizeSecondAndAppend(first, second, true, errorCode);
    }
    আভাস UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           ধ্রুৱ UnicodeString& second,
           UErrorCode& errorCode) ধ্রুৱ override {
        ঘূৰাই normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ধ্রুৱ UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) ধ্রুৱ {
        uprv_checkCanGetBuffer(first, errorCode);
        যদি(U_FAILURE(errorCode)) {
            ঘূৰাই first;
        }
        ধ্রুৱ char16_t *secondArray=second.getBuffer();
        যদি(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই first;
        }
        int32_t firstLength=first.length();
        UnicodeString safe_মধ্য;
        {
            ReorderingBuffer buffer(impl, first);
            যদি(buffer.init(firstLength+second.length(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.length(), doNormalize,
                                   safe_মধ্য, buffer, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        যদি(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(firstLength-safe_মধ্য.length(), 0x7fffffff, safe_মধ্য);
        }
        ঘূৰাই first;
    }
    আভাস শূন্য
    normalizeAndAppend(ধ্রুৱ char16_t *src, ধ্রুৱ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_মধ্য,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুৱ = 0;
    আভাস UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ধ্রুৱ override {
        char16_t buffer[4];
        int32_t length;
        ধ্রুৱ char16_t *d=impl.getDecomposition(c, buffer, length);
        যদি(d==nullptr) {
            ঘূৰাই false;
        }
        যদি(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (Jamos from Hangul syllable c)
        } অন্যথা {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        ঘূৰাই true;
    }
    আভাস UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ধ্রুৱ override {
        char16_t buffer[30];
        int32_t length;
        ধ্রুৱ char16_t *d=impl.getRawDecomposition(c, buffer, length);
        যদি(d==nullptr) {
            ঘূৰাই false;
        }
        যদি(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (algorithmic decomposition)
        } অন্যথা {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        ঘূৰাই true;
    }
    আভাস UChar32
    composePair(UChar32 a, UChar32 b) ধ্রুৱ override {
        ঘূৰাই impl.composePair(a, b);
    }

    আভাস uint8_t
    getCombiningClass(UChar32 c) ধ্রুৱ override {
        ঘূৰাই impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    আভাস UBool
    isNormalized(ধ্রুৱ UnicodeString &s, UErrorCode &errorCode) ধ্রুৱ override {
        যদি(U_FAILURE(errorCode)) {
            ঘূৰাই false;
        }
        ধ্রুৱ char16_t *sArray=s.getBuffer();
        যদি(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই false;
        }
        ধ্রুৱ char16_t *sLimit=sArray+s.length();
        ঘূৰাই sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    আভাস UNormalizationCheckResult
    quickCheck(ধ্রুৱ UnicodeString &s, UErrorCode &errorCode) ধ্রুৱ override {
        ঘূৰাই Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    আভাস int32_t
    spanQuickCheckYes(ধ্রুৱ UnicodeString &s, UErrorCode &errorCode) ধ্রুৱ override {
        যদি(U_FAILURE(errorCode)) {
            ঘূৰাই 0;
        }
        ধ্রুৱ char16_t *sArray=s.getBuffer();
        যদি(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই 0;
        }
        ঘূৰাই static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.length(), errorCode) - sArray);
    }
    আভাস ধ্রুৱ char16_t*
    spanQuickCheckYes(ধ্রুৱ char16_t* src U_LIFETIME_BOUND,
                      ধ্রুৱ char16_t* limit,
                      UErrorCode& errorCode) ধ্রুৱ = 0;

    আভাস UNormalizationCheckResult getQuickCheck(UChar32) ধ্রুৱ {
        ঘূৰাই UNORM_YES;
    }

    ধ্রুৱ Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : সার্ৱজনিক Normalizer2WithImpl {
সার্ৱজনিক:
    DecomposeNormalizer2(ধ্রুৱ Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    আভাস ~DecomposeNormalizer2();

গুপ্ত:
    আভাস শূন্য
    normalize(ধ্রুৱ char16_t *src, ধ্রুৱ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুৱ override {
        impl.decompose(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    আভাস শূন্য
    normalizeAndAppend(ধ্রুৱ char16_t *src, ধ্রুৱ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_মধ্য,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুৱ override {
        impl.decomposeAndAppend(src, limit, doNormalize, safe_মধ্য, buffer, errorCode);
    }

    শূন্য
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ধ্রুৱ override {
        যদি (U_FAILURE(errorCode)) {
            ঘূৰাই;
        }
        যদি (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ধ্রুৱ uint8_t *s = reinterpret_cast<ধ্রুৱ uint8_t *>(src.data());
        impl.decomposeUTF8(options, s, s + src.length(), &sink, edits, errorCode);
        sink.Flush();
    }
    আভাস UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ধ্রুৱ override {
        যদি(U_FAILURE(errorCode)) {
            ঘূৰাই false;
        }
        ধ্রুৱ uint8_t *s = reinterpret_cast<ধ্রুৱ uint8_t *>(sp.data());
        ধ্রুৱ uint8_t *sLimit = s + sp.length();
        ঘূৰাই sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    আভাস ধ্রুৱ char16_t*
    spanQuickCheckYes(ধ্রুৱ char16_t* src U_LIFETIME_BOUND,
                      ধ্রুৱ char16_t* limit,
                      UErrorCode& errorCode) ধ্রুৱ override {
        ঘূৰাই impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    আভাস UNormalizationCheckResult getQuickCheck(UChar32 c) ধ্রুৱ override {
        ঘূৰাই impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    আভাস UBool hasBoundaryBefore(UChar32 c) ধ্রুৱ override {
        ঘূৰাই impl.hasDecompBoundaryBefore(c);
    }
    আভাস UBool hasBoundaryAfter(UChar32 c) ধ্রুৱ override {
        ঘূৰাই impl.hasDecompBoundaryAfter(c);
    }
    আভাস UBool isInert(UChar32 c) ধ্রুৱ override {
        ঘূৰাই impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : সার্ৱজনিক Normalizer2WithImpl {
সার্ৱজনিক:
    ComposeNormalizer2(ধ্রুৱ Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    আভাস ~ComposeNormalizer2();

গুপ্ত:
    আভাস শূন্য
    normalize(ধ্রুৱ char16_t *src, ধ্রুৱ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুৱ override {
        impl.compose(src, limit, onlyContiguous, true, buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    শূন্য
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ধ্রুৱ override {
        যদি (U_FAILURE(errorCode)) {
            ঘূৰাই;
        }
        যদি (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ধ্রুৱ uint8_t *s = reinterpret_cast<ধ্রুৱ uint8_t *>(src.data());
        impl.composeUTF8(options, onlyContiguous, s, s + src.length(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    আভাস শূন্য
    normalizeAndAppend(ধ্রুৱ char16_t *src, ধ্রুৱ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_মধ্য,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুৱ override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safe_মধ্য, buffer, errorCode);
    }

    আভাস UBool
    isNormalized(ধ্রুৱ UnicodeString &s, UErrorCode &errorCode) ধ্রুৱ override {
        যদি(U_FAILURE(errorCode)) {
            ঘূৰাই false;
        }
        ধ্রুৱ char16_t *sArray=s.getBuffer();
        যদি(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই false;
        }
        UnicodeString temp;
        ReorderingBuffer buffer(impl, temp);
        যদি(!buffer.init(5, errorCode)) {  // small destCapacity for substring normalization
            ঘূৰাই false;
        }
        ঘূৰাই impl.compose(sArray, sArray+s.length(), onlyContiguous, false, buffer, errorCode);
    }
    আভাস UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ধ্রুৱ override {
        যদি(U_FAILURE(errorCode)) {
            ঘূৰাই false;
        }
        ধ্রুৱ uint8_t *s = reinterpret_cast<ধ্রুৱ uint8_t *>(sp.data());
        ঘূৰাই impl.composeUTF8(0, onlyContiguous, s, s + sp.length(), nullptr, nullptr, errorCode);
    }
    আভাস UNormalizationCheckResult
    quickCheck(ধ্রুৱ UnicodeString &s, UErrorCode &errorCode) ধ্রুৱ override {
        যদি(U_FAILURE(errorCode)) {
            ঘূৰাই UNORM_MAYBE;
        }
        ধ্রুৱ char16_t *sArray=s.getBuffer();
        যদি(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই UNORM_MAYBE;
        }
        UNormalizationCheckResult qcResult=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.length(), onlyContiguous, &qcResult);
        ঘূৰাই qcResult;
    }
    আভাস ধ্রুৱ char16_t*
    spanQuickCheckYes(ধ্রুৱ char16_t* src U_LIFETIME_BOUND,
                      ধ্রুৱ char16_t* limit,
                      UErrorCode&) ধ্রুৱ override {
        ঘূৰাই impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    আভাস UNormalizationCheckResult getQuickCheck(UChar32 c) ধ্রুৱ override {
        ঘূৰাই impl.getCompQuickCheck(impl.getNorm16(c));
    }
    আভাস UBool hasBoundaryBefore(UChar32 c) ধ্রুৱ override {
        ঘূৰাই impl.hasCompBoundaryBefore(c);
    }
    আভাস UBool hasBoundaryAfter(UChar32 c) ধ্রুৱ override {
        ঘূৰাই impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    আভাস UBool isInert(UChar32 c) ধ্রুৱ override {
        ঘূৰাই impl.isCompInert(c, onlyContiguous);
    }

    ধ্রুৱ UBool onlyContiguous;
};

class FCDNormalizer2 : সার্ৱজনিক Normalizer2WithImpl {
সার্ৱজনিক:
    FCDNormalizer2(ধ্রুৱ Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    আভাস ~FCDNormalizer2();

গুপ্ত:
    আভাস শূন্য
    normalize(ধ্রুৱ char16_t *src, ধ্রুৱ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুৱ override {
        impl.makeFCD(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    আভাস শূন্য
    normalizeAndAppend(ধ্রুৱ char16_t *src, ধ্রুৱ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_মধ্য,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুৱ override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safe_মধ্য, buffer, errorCode);
    }
    আভাস ধ্রুৱ char16_t*
    spanQuickCheckYes(ধ্রুৱ char16_t* src U_LIFETIME_BOUND,
                      ধ্রুৱ char16_t* limit,
                      UErrorCode& errorCode) ধ্রুৱ override {
        ঘূৰাই impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    আভাস UBool hasBoundaryBefore(UChar32 c) ধ্রুৱ override {
        ঘূৰাই impl.hasFCDBoundaryBefore(c);
    }
    আভাস UBool hasBoundaryAfter(UChar32 c) ধ্রুৱ override {
        ঘূৰাই impl.hasFCDBoundaryAfter(c);
    }
    আভাস UBool isInert(UChar32 c) ধ্রুৱ override {
        ঘূৰাই impl.isFCDInert(c);
    }
};

সংরচনা Norm2AllModes : সার্ৱজনিক UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    স্থির Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    স্থির Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    স্থির Norm2AllModes *createInstance(ধ্রুৱ ৱর্ণ *packageName,
                                         ধ্রুৱ ৱর্ণ *name,
                                         UErrorCode &errorCode);

    স্থির ধ্রুৱ Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    স্থির ধ্রুৱ Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    স্থির ধ্রুৱ Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    স্থির ধ্রুৱ Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
