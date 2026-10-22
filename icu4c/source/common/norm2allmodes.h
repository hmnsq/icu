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

#include "unicode/utypes.h"

#གལ་སྲིད !UCONFIG_NO_NORMALIZATION

#include "unicode/edits.h"
#include "unicode/normalizer2.h"
#include "unicode/stringoptions.h"
#include "unicode/unistr.h"
#include "cpputils.h"
#include "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : public Normalizer2 {
public:
    Normalizer2WithImpl(const Normalizer2Impl &ni) : impl(ni) {}
    virtual ~Normalizer2WithImpl();

    // normalize
    virtual UnicodeString&
    normalize(const UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) const override {
        གལ་སྲིད(U_FAILURE(errorCode)) {
            dest.setToBogus();
            ལོག dest;
        }
        const char16_t *sArray=src.getBuffer();
        གལ་སྲིད(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            ལོག dest;
        }
        dest.remove();
        ReorderingBuffer buffer(impl, dest);
        གལ་སྲིད(buffer.init(src.length(), errorCode)) {
            normalize(sArray, sArray+src.length(), buffer, errorCode);
        }
        ལོག dest;
    }
    virtual void
    normalize(const char16_t *src, const char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) const = 0;

    // normalize and append
    virtual UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             const UnicodeString& second,
                             UErrorCode& errorCode) const override {
        ལོག normalizeSecondAndAppend(first, second, true, errorCode);
    }
    virtual UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           const UnicodeString& second,
           UErrorCode& errorCode) const override {
        ལོག normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             const UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) const {
        uprv_checkCanGetBuffer(first, errorCode);
        གལ་སྲིད(U_FAILURE(errorCode)) {
            ལོག first;
        }
        const char16_t *secondArray=second.getBuffer();
        གལ་སྲིད(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ལོག first;
        }
        int32_t firstLength=first.length();
        UnicodeString safeMiddle;
        {
            ReorderingBuffer buffer(impl, first);
            གལ་སྲིད(buffer.init(firstLength+second.length(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.length(), doNormalize,
                                   safeMiddle, buffer, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        གལ་སྲིད(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(firstLength-safeMiddle.length(), 0x7fffffff, safeMiddle);
        }
        ལོག first;
    }
    virtual void
    normalizeAndAppend(const char16_t *src, const char16_t *limit, UBool doNormalize,
                       UnicodeString &safeMiddle,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) const = 0;
    virtual UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) const override {
        char16_t buffer[4];
        int32_t length;
        const char16_t *d=impl.getDecomposition(c, buffer, length);
        གལ་སྲིད(d==nullptr) {
            ལོག false;
        }
        གལ་སྲིད(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (Jamos from Hangul syllable c)
        } དེ་མིན {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        ལོག true;
    }
    virtual UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) const override {
        char16_t buffer[30];
        int32_t length;
        const char16_t *d=impl.getRawDecomposition(c, buffer, length);
        གལ་སྲིད(d==nullptr) {
            ལོག false;
        }
        གལ་སྲིད(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (algorithmic decomposition)
        } དེ་མིན {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        ལོག true;
    }
    virtual UChar32
    composePair(UChar32 a, UChar32 b) const override {
        ལོག impl.composePair(a, b);
    }

    virtual uint8_t
    getCombiningClass(UChar32 c) const override {
        ལོག impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    virtual UBool
    isNormalized(const UnicodeString &s, UErrorCode &errorCode) const override {
        གལ་སྲིད(U_FAILURE(errorCode)) {
            ལོག false;
        }
        const char16_t *sArray=s.getBuffer();
        གལ་སྲིད(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ལོག false;
        }
        const char16_t *sLimit=sArray+s.length();
        ལོག sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    virtual UNormalizationCheckResult
    quickCheck(const UnicodeString &s, UErrorCode &errorCode) const override {
        ལོག Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    virtual int32_t
    spanQuickCheckYes(const UnicodeString &s, UErrorCode &errorCode) const override {
        གལ་སྲིད(U_FAILURE(errorCode)) {
            ལོག 0;
        }
        const char16_t *sArray=s.getBuffer();
        གལ་སྲིད(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ལོག 0;
        }
        ལོག static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.length(), errorCode) - sArray);
    }
    virtual const char16_t*
    spanQuickCheckYes(const char16_t* src U_LIFETIME_BOUND,
                      const char16_t* limit,
                      UErrorCode& errorCode) const = 0;

    virtual UNormalizationCheckResult getQuickCheck(UChar32) const {
        ལོག UNORM_YES;
    }

    const Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : public Normalizer2WithImpl {
public:
    DecomposeNormalizer2(const Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    virtual ~DecomposeNormalizer2();

private:
    virtual void
    normalize(const char16_t *src, const char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) const override {
        impl.decompose(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    virtual void
    normalizeAndAppend(const char16_t *src, const char16_t *limit, UBool doNormalize,
                       UnicodeString &safeMiddle,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) const override {
        impl.decomposeAndAppend(src, limit, doNormalize, safeMiddle, buffer, errorCode);
    }

    void
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) const override {
        གལ་སྲིད (U_FAILURE(errorCode)) {
            ལོག;
        }
        གལ་སྲིད (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        const uint8_t *s = reinterpret_cast<const uint8_t *>(src.data());
        impl.decomposeUTF8(options, s, s + src.length(), &sink, edits, errorCode);
        sink.Flush();
    }
    virtual UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) const override {
        གལ་སྲིད(U_FAILURE(errorCode)) {
            ལོག false;
        }
        const uint8_t *s = reinterpret_cast<const uint8_t *>(sp.data());
        const uint8_t *sLimit = s + sp.length();
        ལོག sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    virtual const char16_t*
    spanQuickCheckYes(const char16_t* src U_LIFETIME_BOUND,
                      const char16_t* limit,
                      UErrorCode& errorCode) const override {
        ལོག impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    virtual UNormalizationCheckResult getQuickCheck(UChar32 c) const override {
        ལོག impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    virtual UBool hasBoundaryBefore(UChar32 c) const override {
        ལོག impl.hasDecompBoundaryBefore(c);
    }
    virtual UBool hasBoundaryAfter(UChar32 c) const override {
        ལོག impl.hasDecompBoundaryAfter(c);
    }
    virtual UBool isInert(UChar32 c) const override {
        ལོག impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : public Normalizer2WithImpl {
public:
    ComposeNormalizer2(const Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    virtual ~ComposeNormalizer2();

private:
    virtual void
    normalize(const char16_t *src, const char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) const override {
        impl.compose(src, limit, onlyContiguous, true, buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    void
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) const override {
        གལ་སྲིད (U_FAILURE(errorCode)) {
            ལོག;
        }
        གལ་སྲིད (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        const uint8_t *s = reinterpret_cast<const uint8_t *>(src.data());
        impl.composeUTF8(options, onlyContiguous, s, s + src.length(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    virtual void
    normalizeAndAppend(const char16_t *src, const char16_t *limit, UBool doNormalize,
                       UnicodeString &safeMiddle,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) const override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safeMiddle, buffer, errorCode);
    }

    virtual UBool
    isNormalized(const UnicodeString &s, UErrorCode &errorCode) const override {
        གལ་སྲིད(U_FAILURE(errorCode)) {
            ལོག false;
        }
        const char16_t *sArray=s.getBuffer();
        གལ་སྲིད(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ལོག false;
        }
        UnicodeString temp;
        ReorderingBuffer buffer(impl, temp);
        གལ་སྲིད(!buffer.init(5, errorCode)) {  // small destCapacity for substring normalization
            ལོག false;
        }
        ལོག impl.compose(sArray, sArray+s.length(), onlyContiguous, false, buffer, errorCode);
    }
    virtual UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) const override {
        གལ་སྲིད(U_FAILURE(errorCode)) {
            ལོག false;
        }
        const uint8_t *s = reinterpret_cast<const uint8_t *>(sp.data());
        ལོག impl.composeUTF8(0, onlyContiguous, s, s + sp.length(), nullptr, nullptr, errorCode);
    }
    virtual UNormalizationCheckResult
    quickCheck(const UnicodeString &s, UErrorCode &errorCode) const override {
        གལ་སྲིད(U_FAILURE(errorCode)) {
            ལོག UNORM_MAYBE;
        }
        const char16_t *sArray=s.getBuffer();
        གལ་སྲིད(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ལོག UNORM_MAYBE;
        }
        UNormalizationCheckResult qcResult=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.length(), onlyContiguous, &qcResult);
        ལོག qcResult;
    }
    virtual const char16_t*
    spanQuickCheckYes(const char16_t* src U_LIFETIME_BOUND,
                      const char16_t* limit,
                      UErrorCode&) const override {
        ལོག impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    virtual UNormalizationCheckResult getQuickCheck(UChar32 c) const override {
        ལོག impl.getCompQuickCheck(impl.getNorm16(c));
    }
    virtual UBool hasBoundaryBefore(UChar32 c) const override {
        ལོག impl.hasCompBoundaryBefore(c);
    }
    virtual UBool hasBoundaryAfter(UChar32 c) const override {
        ལོག impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    virtual UBool isInert(UChar32 c) const override {
        ལོག impl.isCompInert(c, onlyContiguous);
    }

    const UBool onlyContiguous;
};

class FCDNormalizer2 : public Normalizer2WithImpl {
public:
    FCDNormalizer2(const Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    virtual ~FCDNormalizer2();

private:
    virtual void
    normalize(const char16_t *src, const char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) const override {
        impl.makeFCD(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    virtual void
    normalizeAndAppend(const char16_t *src, const char16_t *limit, UBool doNormalize,
                       UnicodeString &safeMiddle,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) const override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safeMiddle, buffer, errorCode);
    }
    virtual const char16_t*
    spanQuickCheckYes(const char16_t* src U_LIFETIME_BOUND,
                      const char16_t* limit,
                      UErrorCode& errorCode) const override {
        ལོག impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    virtual UBool hasBoundaryBefore(UChar32 c) const override {
        ལོག impl.hasFCDBoundaryBefore(c);
    }
    virtual UBool hasBoundaryAfter(UChar32 c) const override {
        ལོག impl.hasFCDBoundaryAfter(c);
    }
    virtual UBool isInert(UChar32 c) const override {
        ལོག impl.isFCDInert(c);
    }
};

sa_nracanaa Norm2AllModes : public UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    static Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    static Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    static Norm2AllModes *createInstance(const var_na *packageName,
                                         const var_na *name,
                                         UErrorCode &errorCode);

    static const Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    static const Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    static const Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    static const Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
