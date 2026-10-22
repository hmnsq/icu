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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_NORMALIZATION

#ଅନ୍ତର୍ଭାଵ "unicode/edits.h"
#ଅନ୍ତର୍ଭାଵ "unicode/normalizer2.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringoptions.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "cpputils.h"
#ଅନ୍ତର୍ଭାଵ "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : ସର୍ବସାଧାରଣ Normalizer2 {
ସର୍ବସାଧାରଣ:
    Normalizer2WithImpl(ଅଚଳ Normalizer2Impl &ni) : impl(ni) {}
    ଆଭାସୀ ~Normalizer2WithImpl();

    // normalize
    ଆଭାସୀ UnicodeString&
    normalize(ଅଚଳ UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) ଅଚଳ override {
        ଯଦି(U_FAILURE(errorCode)) {
            dest.setToBogus();
            ଫେରନ୍ତୁ dest;
        }
        ଅଚଳ char16_t *sArray=src.getBuffer();
        ଯଦି(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            ଫେରନ୍ତୁ dest;
        }
        dest.remove();
        ReorderingBuffer buffer(impl, dest);
        ଯଦି(buffer.init(src.length(), errorCode)) {
            normalize(sArray, sArray+src.length(), buffer, errorCode);
        }
        ଫେରନ୍ତୁ dest;
    }
    ଆଭାସୀ ଶୂନ୍ୟ
    normalize(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ଅଚଳ = 0;

    // normalize and append
    ଆଭାସୀ UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ଅଚଳ UnicodeString& second,
                             UErrorCode& errorCode) ଅଚଳ override {
        ଫେରନ୍ତୁ normalizeSecondAndAppend(first, second, true, errorCode);
    }
    ଆଭାସୀ UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           ଅଚଳ UnicodeString& second,
           UErrorCode& errorCode) ଅଚଳ override {
        ଫେରନ୍ତୁ normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ଅଚଳ UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) ଅଚଳ {
        uprv_checkCanGetBuffer(first, errorCode);
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ first;
        }
        ଅଚଳ char16_t *secondArray=second.getBuffer();
        ଯଦି(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ first;
        }
        int32_t firstLength=first.length();
        UnicodeString safe_ମଧ୍ୟ;
        {
            ReorderingBuffer buffer(impl, first);
            ଯଦି(buffer.init(firstLength+second.length(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.length(), doNormalize,
                                   safe_ମଧ୍ୟ, buffer, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        ଯଦି(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(firstLength-safe_ମଧ୍ୟ.length(), 0x7fffffff, safe_ମଧ୍ୟ);
        }
        ଫେରନ୍ତୁ first;
    }
    ଆଭାସୀ ଶୂନ୍ୟ
    normalizeAndAppend(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_ମଧ୍ୟ,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ଅଚଳ = 0;
    ଆଭାସୀ UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ଅଚଳ override {
        char16_t buffer[4];
        int32_t length;
        ଅଚଳ char16_t *d=impl.getDecomposition(c, buffer, length);
        ଯଦି(d==nullptr) {
            ଫେରନ୍ତୁ false;
        }
        ଯଦି(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (Jamos from Hangul syllable c)
        } ଅନ୍ୟଥା {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        ଫେରନ୍ତୁ true;
    }
    ଆଭାସୀ UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ଅଚଳ override {
        char16_t buffer[30];
        int32_t length;
        ଅଚଳ char16_t *d=impl.getRawDecomposition(c, buffer, length);
        ଯଦି(d==nullptr) {
            ଫେରନ୍ତୁ false;
        }
        ଯଦି(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (algorithmic decomposition)
        } ଅନ୍ୟଥା {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        ଫେରନ୍ତୁ true;
    }
    ଆଭାସୀ UChar32
    composePair(UChar32 a, UChar32 b) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.composePair(a, b);
    }

    ଆଭାସୀ uint8_t
    getCombiningClass(UChar32 c) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    ଆଭାସୀ UBool
    isNormalized(ଅଚଳ UnicodeString &s, UErrorCode &errorCode) ଅଚଳ override {
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ false;
        }
        ଅଚଳ char16_t *sArray=s.getBuffer();
        ଯଦି(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ false;
        }
        ଅଚଳ char16_t *sLimit=sArray+s.length();
        ଫେରନ୍ତୁ sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    ଆଭାସୀ UNormalizationCheckResult
    quickCheck(ଅଚଳ UnicodeString &s, UErrorCode &errorCode) ଅଚଳ override {
        ଫେରନ୍ତୁ Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    ଆଭାସୀ int32_t
    spanQuickCheckYes(ଅଚଳ UnicodeString &s, UErrorCode &errorCode) ଅଚଳ override {
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ 0;
        }
        ଅଚଳ char16_t *sArray=s.getBuffer();
        ଯଦି(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        ଫେରନ୍ତୁ static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.length(), errorCode) - sArray);
    }
    ଆଭାସୀ ଅଚଳ char16_t*
    spanQuickCheckYes(ଅଚଳ char16_t* src U_LIFETIME_BOUND,
                      ଅଚଳ char16_t* limit,
                      UErrorCode& errorCode) ଅଚଳ = 0;

    ଆଭାସୀ UNormalizationCheckResult getQuickCheck(UChar32) ଅଚଳ {
        ଫେରନ୍ତୁ UNORM_YES;
    }

    ଅଚଳ Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : ସର୍ବସାଧାରଣ Normalizer2WithImpl {
ସର୍ବସାଧାରଣ:
    DecomposeNormalizer2(ଅଚଳ Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    ଆଭାସୀ ~DecomposeNormalizer2();

ବ୍ୟକ୍ତିଗତ:
    ଆଭାସୀ ଶୂନ୍ୟ
    normalize(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ଅଚଳ override {
        impl.decompose(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    ଆଭାସୀ ଶୂନ୍ୟ
    normalizeAndAppend(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_ମଧ୍ୟ,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ଅଚଳ override {
        impl.decomposeAndAppend(src, limit, doNormalize, safe_ମଧ୍ୟ, buffer, errorCode);
    }

    ଶୂନ୍ୟ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ଅଚଳ override {
        ଯଦି (U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ;
        }
        ଯଦି (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ଅଚଳ uint8_t *s = reinterpret_cast<ଅଚଳ uint8_t *>(src.data());
        impl.decomposeUTF8(options, s, s + src.length(), &sink, edits, errorCode);
        sink.Flush();
    }
    ଆଭାସୀ UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ଅଚଳ override {
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ false;
        }
        ଅଚଳ uint8_t *s = reinterpret_cast<ଅଚଳ uint8_t *>(sp.data());
        ଅଚଳ uint8_t *sLimit = s + sp.length();
        ଫେରନ୍ତୁ sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    ଆଭାସୀ ଅଚଳ char16_t*
    spanQuickCheckYes(ଅଚଳ char16_t* src U_LIFETIME_BOUND,
                      ଅଚଳ char16_t* limit,
                      UErrorCode& errorCode) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    ଆଭାସୀ UNormalizationCheckResult getQuickCheck(UChar32 c) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    ଆଭାସୀ UBool hasBoundaryBefore(UChar32 c) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.hasDecompBoundaryBefore(c);
    }
    ଆଭାସୀ UBool hasBoundaryAfter(UChar32 c) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.hasDecompBoundaryAfter(c);
    }
    ଆଭାସୀ UBool isInert(UChar32 c) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : ସର୍ବସାଧାରଣ Normalizer2WithImpl {
ସର୍ବସାଧାରଣ:
    ComposeNormalizer2(ଅଚଳ Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    ଆଭାସୀ ~ComposeNormalizer2();

ବ୍ୟକ୍ତିଗତ:
    ଆଭାସୀ ଶୂନ୍ୟ
    normalize(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ଅଚଳ override {
        impl.compose(src, limit, onlyContiguous, true, buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    ଶୂନ୍ୟ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ଅଚଳ override {
        ଯଦି (U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ;
        }
        ଯଦି (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ଅଚଳ uint8_t *s = reinterpret_cast<ଅଚଳ uint8_t *>(src.data());
        impl.composeUTF8(options, onlyContiguous, s, s + src.length(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    ଆଭାସୀ ଶୂନ୍ୟ
    normalizeAndAppend(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_ମଧ୍ୟ,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ଅଚଳ override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safe_ମଧ୍ୟ, buffer, errorCode);
    }

    ଆଭାସୀ UBool
    isNormalized(ଅଚଳ UnicodeString &s, UErrorCode &errorCode) ଅଚଳ override {
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ false;
        }
        ଅଚଳ char16_t *sArray=s.getBuffer();
        ଯଦି(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ false;
        }
        UnicodeString temp;
        ReorderingBuffer buffer(impl, temp);
        ଯଦି(!buffer.init(5, errorCode)) {  // small destCapacity for substring normalization
            ଫେରନ୍ତୁ false;
        }
        ଫେରନ୍ତୁ impl.compose(sArray, sArray+s.length(), onlyContiguous, false, buffer, errorCode);
    }
    ଆଭାସୀ UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ଅଚଳ override {
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ false;
        }
        ଅଚଳ uint8_t *s = reinterpret_cast<ଅଚଳ uint8_t *>(sp.data());
        ଫେରନ୍ତୁ impl.composeUTF8(0, onlyContiguous, s, s + sp.length(), nullptr, nullptr, errorCode);
    }
    ଆଭାସୀ UNormalizationCheckResult
    quickCheck(ଅଚଳ UnicodeString &s, UErrorCode &errorCode) ଅଚଳ override {
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ UNORM_MAYBE;
        }
        ଅଚଳ char16_t *sArray=s.getBuffer();
        ଯଦି(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ UNORM_MAYBE;
        }
        UNormalizationCheckResult qcResult=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.length(), onlyContiguous, &qcResult);
        ଫେରନ୍ତୁ qcResult;
    }
    ଆଭାସୀ ଅଚଳ char16_t*
    spanQuickCheckYes(ଅଚଳ char16_t* src U_LIFETIME_BOUND,
                      ଅଚଳ char16_t* limit,
                      UErrorCode&) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    ଆଭାସୀ UNormalizationCheckResult getQuickCheck(UChar32 c) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.getCompQuickCheck(impl.getNorm16(c));
    }
    ଆଭାସୀ UBool hasBoundaryBefore(UChar32 c) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.hasCompBoundaryBefore(c);
    }
    ଆଭାସୀ UBool hasBoundaryAfter(UChar32 c) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    ଆଭାସୀ UBool isInert(UChar32 c) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.isCompInert(c, onlyContiguous);
    }

    ଅଚଳ UBool onlyContiguous;
};

class FCDNormalizer2 : ସର୍ବସାଧାରଣ Normalizer2WithImpl {
ସର୍ବସାଧାରଣ:
    FCDNormalizer2(ଅଚଳ Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    ଆଭାସୀ ~FCDNormalizer2();

ବ୍ୟକ୍ତିଗତ:
    ଆଭାସୀ ଶୂନ୍ୟ
    normalize(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ଅଚଳ override {
        impl.makeFCD(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    ଆଭାସୀ ଶୂନ୍ୟ
    normalizeAndAppend(ଅଚଳ char16_t *src, ଅଚଳ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_ମଧ୍ୟ,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ଅଚଳ override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safe_ମଧ୍ୟ, buffer, errorCode);
    }
    ଆଭାସୀ ଅଚଳ char16_t*
    spanQuickCheckYes(ଅଚଳ char16_t* src U_LIFETIME_BOUND,
                      ଅଚଳ char16_t* limit,
                      UErrorCode& errorCode) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    ଆଭାସୀ UBool hasBoundaryBefore(UChar32 c) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.hasFCDBoundaryBefore(c);
    }
    ଆଭାସୀ UBool hasBoundaryAfter(UChar32 c) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.hasFCDBoundaryAfter(c);
    }
    ଆଭାସୀ UBool isInert(UChar32 c) ଅଚଳ override {
        ଫେରନ୍ତୁ impl.isFCDInert(c);
    }
};

ସଂରଚନା Norm2AllModes : ସର୍ବସାଧାରଣ UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    ସ୍ଥିର Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    ସ୍ଥିର Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    ସ୍ଥିର Norm2AllModes *createInstance(ଅଚଳ ଅକ୍ଷର *packageName,
                                         ଅଚଳ ଅକ୍ଷର *name,
                                         UErrorCode &errorCode);

    ସ୍ଥିର ଅଚଳ Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    ସ୍ଥିର ଅଚଳ Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    ସ୍ଥିର ଅଚଳ Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    ସ୍ଥିର ଅଚଳ Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
