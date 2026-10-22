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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_NORMALIZATION

#অন্তর্ভাব "unicode/edits.h"
#অন্তর্ভাব "unicode/normalizer2.h"
#অন্তর্ভাব "unicode/stringoptions.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "cpputils.h"
#অন্তর্ভাব "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : সার্বজনিক Normalizer2 {
সার্বজনিক:
    Normalizer2WithImpl(ধ্রুব Normalizer2Impl &ni) : impl(ni) {}
    আভাস ~Normalizer2WithImpl();

    // normalize
    আভাস UnicodeString&
    normalize(ধ্রুব UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) ধ্রুব override {
        যদি(U_FAILURE(errorCode)) {
            dest.setToBogus();
            প্রত্যাগম dest;
        }
        ধ্রুব char16_t *sArray=src.getBuffer();
        যদি(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            প্রত্যাগম dest;
        }
        dest.remove();
        ReorderingBuffer buffer(impl, dest);
        যদি(buffer.init(src.length(), errorCode)) {
            normalize(sArray, sArray+src.length(), buffer, errorCode);
        }
        প্রত্যাগম dest;
    }
    আভাস শূন্য
    normalize(ধ্রুব char16_t *src, ধ্রুব char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুব = 0;

    // normalize and append
    আভাস UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ধ্রুব UnicodeString& second,
                             UErrorCode& errorCode) ধ্রুব override {
        প্রত্যাগম normalizeSecondAndAppend(first, second, true, errorCode);
    }
    আভাস UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           ধ্রুব UnicodeString& second,
           UErrorCode& errorCode) ধ্রুব override {
        প্রত্যাগম normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ধ্রুব UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) ধ্রুব {
        uprv_checkCanGetBuffer(first, errorCode);
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম first;
        }
        ধ্রুব char16_t *secondArray=second.getBuffer();
        যদি(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম first;
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
        প্রত্যাগম first;
    }
    আভাস শূন্য
    normalizeAndAppend(ধ্রুব char16_t *src, ধ্রুব char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_মধ্য,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুব = 0;
    আভাস UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ধ্রুব override {
        char16_t buffer[4];
        int32_t length;
        ধ্রুব char16_t *d=impl.getDecomposition(c, buffer, length);
        যদি(d==nullptr) {
            প্রত্যাগম false;
        }
        যদি(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (Jamos from Hangul syllable c)
        } অন্যথা {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        প্রত্যাগম true;
    }
    আভাস UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ধ্রুব override {
        char16_t buffer[30];
        int32_t length;
        ধ্রুব char16_t *d=impl.getRawDecomposition(c, buffer, length);
        যদি(d==nullptr) {
            প্রত্যাগম false;
        }
        যদি(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (algorithmic decomposition)
        } অন্যথা {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        প্রত্যাগম true;
    }
    আভাস UChar32
    composePair(UChar32 a, UChar32 b) ধ্রুব override {
        প্রত্যাগম impl.composePair(a, b);
    }

    আভাস uint8_t
    getCombiningClass(UChar32 c) ধ্রুব override {
        প্রত্যাগম impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    আভাস UBool
    isNormalized(ধ্রুব UnicodeString &s, UErrorCode &errorCode) ধ্রুব override {
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম false;
        }
        ধ্রুব char16_t *sArray=s.getBuffer();
        যদি(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম false;
        }
        ধ্রুব char16_t *sLimit=sArray+s.length();
        প্রত্যাগম sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    আভাস UNormalizationCheckResult
    quickCheck(ধ্রুব UnicodeString &s, UErrorCode &errorCode) ধ্রুব override {
        প্রত্যাগম Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    আভাস int32_t
    spanQuickCheckYes(ধ্রুব UnicodeString &s, UErrorCode &errorCode) ধ্রুব override {
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম 0;
        }
        ধ্রুব char16_t *sArray=s.getBuffer();
        যদি(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম 0;
        }
        প্রত্যাগম static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.length(), errorCode) - sArray);
    }
    আভাস ধ্রুব char16_t*
    spanQuickCheckYes(ধ্রুব char16_t* src U_LIFETIME_BOUND,
                      ধ্রুব char16_t* limit,
                      UErrorCode& errorCode) ধ্রুব = 0;

    আভাস UNormalizationCheckResult getQuickCheck(UChar32) ধ্রুব {
        প্রত্যাগম UNORM_YES;
    }

    ধ্রুব Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : সার্বজনিক Normalizer2WithImpl {
সার্বজনিক:
    DecomposeNormalizer2(ধ্রুব Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    আভাস ~DecomposeNormalizer2();

গুপ্ত:
    আভাস শূন্য
    normalize(ধ্রুব char16_t *src, ধ্রুব char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুব override {
        impl.decompose(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    আভাস শূন্য
    normalizeAndAppend(ধ্রুব char16_t *src, ধ্রুব char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_মধ্য,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুব override {
        impl.decomposeAndAppend(src, limit, doNormalize, safe_মধ্য, buffer, errorCode);
    }

    শূন্য
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ধ্রুব override {
        যদি (U_FAILURE(errorCode)) {
            প্রত্যাগম;
        }
        যদি (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ধ্রুব uint8_t *s = reinterpret_cast<ধ্রুব uint8_t *>(src.data());
        impl.decomposeUTF8(options, s, s + src.length(), &sink, edits, errorCode);
        sink.Flush();
    }
    আভাস UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ধ্রুব override {
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম false;
        }
        ধ্রুব uint8_t *s = reinterpret_cast<ধ্রুব uint8_t *>(sp.data());
        ধ্রুব uint8_t *sLimit = s + sp.length();
        প্রত্যাগম sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    আভাস ধ্রুব char16_t*
    spanQuickCheckYes(ধ্রুব char16_t* src U_LIFETIME_BOUND,
                      ধ্রুব char16_t* limit,
                      UErrorCode& errorCode) ধ্রুব override {
        প্রত্যাগম impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    আভাস UNormalizationCheckResult getQuickCheck(UChar32 c) ধ্রুব override {
        প্রত্যাগম impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    আভাস UBool hasBoundaryBefore(UChar32 c) ধ্রুব override {
        প্রত্যাগম impl.hasDecompBoundaryBefore(c);
    }
    আভাস UBool hasBoundaryAfter(UChar32 c) ধ্রুব override {
        প্রত্যাগম impl.hasDecompBoundaryAfter(c);
    }
    আভাস UBool isInert(UChar32 c) ধ্রুব override {
        প্রত্যাগম impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : সার্বজনিক Normalizer2WithImpl {
সার্বজনিক:
    ComposeNormalizer2(ধ্রুব Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    আভাস ~ComposeNormalizer2();

গুপ্ত:
    আভাস শূন্য
    normalize(ধ্রুব char16_t *src, ধ্রুব char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুব override {
        impl.compose(src, limit, onlyContiguous, true, buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    শূন্য
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ধ্রুব override {
        যদি (U_FAILURE(errorCode)) {
            প্রত্যাগম;
        }
        যদি (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ধ্রুব uint8_t *s = reinterpret_cast<ধ্রুব uint8_t *>(src.data());
        impl.composeUTF8(options, onlyContiguous, s, s + src.length(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    আভাস শূন্য
    normalizeAndAppend(ধ্রুব char16_t *src, ধ্রুব char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_মধ্য,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুব override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safe_মধ্য, buffer, errorCode);
    }

    আভাস UBool
    isNormalized(ধ্রুব UnicodeString &s, UErrorCode &errorCode) ধ্রুব override {
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম false;
        }
        ধ্রুব char16_t *sArray=s.getBuffer();
        যদি(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম false;
        }
        UnicodeString temp;
        ReorderingBuffer buffer(impl, temp);
        যদি(!buffer.init(5, errorCode)) {  // small destCapacity for substring normalization
            প্রত্যাগম false;
        }
        প্রত্যাগম impl.compose(sArray, sArray+s.length(), onlyContiguous, false, buffer, errorCode);
    }
    আভাস UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ধ্রুব override {
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম false;
        }
        ধ্রুব uint8_t *s = reinterpret_cast<ধ্রুব uint8_t *>(sp.data());
        প্রত্যাগম impl.composeUTF8(0, onlyContiguous, s, s + sp.length(), nullptr, nullptr, errorCode);
    }
    আভাস UNormalizationCheckResult
    quickCheck(ধ্রুব UnicodeString &s, UErrorCode &errorCode) ধ্রুব override {
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম UNORM_MAYBE;
        }
        ধ্রুব char16_t *sArray=s.getBuffer();
        যদি(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম UNORM_MAYBE;
        }
        UNormalizationCheckResult qcResult=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.length(), onlyContiguous, &qcResult);
        প্রত্যাগম qcResult;
    }
    আভাস ধ্রুব char16_t*
    spanQuickCheckYes(ধ্রুব char16_t* src U_LIFETIME_BOUND,
                      ধ্রুব char16_t* limit,
                      UErrorCode&) ধ্রুব override {
        প্রত্যাগম impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    আভাস UNormalizationCheckResult getQuickCheck(UChar32 c) ধ্রুব override {
        প্রত্যাগম impl.getCompQuickCheck(impl.getNorm16(c));
    }
    আভাস UBool hasBoundaryBefore(UChar32 c) ধ্রুব override {
        প্রত্যাগম impl.hasCompBoundaryBefore(c);
    }
    আভাস UBool hasBoundaryAfter(UChar32 c) ধ্রুব override {
        প্রত্যাগম impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    আভাস UBool isInert(UChar32 c) ধ্রুব override {
        প্রত্যাগম impl.isCompInert(c, onlyContiguous);
    }

    ধ্রুব UBool onlyContiguous;
};

class FCDNormalizer2 : সার্বজনিক Normalizer2WithImpl {
সার্বজনিক:
    FCDNormalizer2(ধ্রুব Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    আভাস ~FCDNormalizer2();

গুপ্ত:
    আভাস শূন্য
    normalize(ধ্রুব char16_t *src, ধ্রুব char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুব override {
        impl.makeFCD(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    আভাস শূন্য
    normalizeAndAppend(ধ্রুব char16_t *src, ধ্রুব char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_মধ্য,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ধ্রুব override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safe_মধ্য, buffer, errorCode);
    }
    আভাস ধ্রুব char16_t*
    spanQuickCheckYes(ধ্রুব char16_t* src U_LIFETIME_BOUND,
                      ধ্রুব char16_t* limit,
                      UErrorCode& errorCode) ধ্রুব override {
        প্রত্যাগম impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    আভাস UBool hasBoundaryBefore(UChar32 c) ধ্রুব override {
        প্রত্যাগম impl.hasFCDBoundaryBefore(c);
    }
    আভাস UBool hasBoundaryAfter(UChar32 c) ধ্রুব override {
        প্রত্যাগম impl.hasFCDBoundaryAfter(c);
    }
    আভাস UBool isInert(UChar32 c) ধ্রুব override {
        প্রত্যাগম impl.isFCDInert(c);
    }
};

সংরচনা Norm2AllModes : সার্বজনিক UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    স্থির Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    স্থির Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    স্থির Norm2AllModes *createInstance(ধ্রুব বর্ণ *packageName,
                                         ধ্রুব বর্ণ *name,
                                         UErrorCode &errorCode);

    স্থির ধ্রুব Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    স্থির ধ্রুব Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    স্থির ধ্রুব Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    স্থির ধ্রুব Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
