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
class Normalizer2WithImpl : عمومی Normalizer2 {
عمومی:
    Normalizer2WithImpl(ثابت Normalizer2Impl &ni) : impl(ni) {}
    مجازی ~Normalizer2WithImpl();

    // normalize
    مجازی UnicodeString&
    normalize(ثابت UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) ثابت override {
        اگر(U_FAILURE(errorCode)) {
            dest.setToBogus();
            بازگشت dest;
        }
        ثابت char16_t *sArray=src.getBuffer();
        اگر(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            بازگشت dest;
        }
        dest.remove();
        ReorderingBuffer buffer(impl, dest);
        اگر(buffer.init(src.length(), errorCode)) {
            normalize(sArray, sArray+src.length(), buffer, errorCode);
        }
        بازگشت dest;
    }
    مجازی تهی
    normalize(ثابت char16_t *src, ثابت char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت = 0;

    // normalize and append
    مجازی UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ثابت UnicodeString& second,
                             UErrorCode& errorCode) ثابت override {
        بازگشت normalizeSecondAndAppend(first, second, true, errorCode);
    }
    مجازی UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           ثابت UnicodeString& second,
           UErrorCode& errorCode) ثابت override {
        بازگشت normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ثابت UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) ثابت {
        uprv_checkCanGetBuffer(first, errorCode);
        اگر(U_FAILURE(errorCode)) {
            بازگشت first;
        }
        ثابت char16_t *secondArray=second.getBuffer();
        اگر(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت first;
        }
        int32_t firstLength=first.length();
        UnicodeString safe_میانه;
        {
            ReorderingBuffer buffer(impl, first);
            اگر(buffer.init(firstLength+second.length(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.length(), doNormalize,
                                   safe_میانه, buffer, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        اگر(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(firstLength-safe_میانه.length(), 0x7fffffff, safe_میانه);
        }
        بازگشت first;
    }
    مجازی تهی
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_میانه,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت = 0;
    مجازی UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t buffer[4];
        int32_t length;
        ثابت char16_t *d=impl.getDecomposition(c, buffer, length);
        اگر(d==nullptr) {
            بازگشت false;
        }
        اگر(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (Jamos from Hangul syllable c)
        } وگرنه {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        بازگشت true;
    }
    مجازی UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t buffer[30];
        int32_t length;
        ثابت char16_t *d=impl.getRawDecomposition(c, buffer, length);
        اگر(d==nullptr) {
            بازگشت false;
        }
        اگر(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (algorithmic decomposition)
        } وگرنه {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        بازگشت true;
    }
    مجازی UChar32
    composePair(UChar32 a, UChar32 b) ثابت override {
        بازگشت impl.composePair(a, b);
    }

    مجازی uint8_t
    getCombiningClass(UChar32 c) ثابت override {
        بازگشت impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    مجازی UBool
    isNormalized(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت override {
        اگر(U_FAILURE(errorCode)) {
            بازگشت false;
        }
        ثابت char16_t *sArray=s.getBuffer();
        اگر(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت false;
        }
        ثابت char16_t *sLimit=sArray+s.length();
        بازگشت sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    مجازی UNormalizationCheckResult
    quickCheck(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت override {
        بازگشت Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    مجازی int32_t
    spanQuickCheckYes(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت override {
        اگر(U_FAILURE(errorCode)) {
            بازگشت 0;
        }
        ثابت char16_t *sArray=s.getBuffer();
        اگر(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت 0;
        }
        بازگشت static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.length(), errorCode) - sArray);
    }
    مجازی ثابت char16_t*
    spanQuickCheckYes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* limit,
                      UErrorCode& errorCode) ثابت = 0;

    مجازی UNormalizationCheckResult getQuickCheck(UChar32) ثابت {
        بازگشت UNORM_YES;
    }

    ثابت Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : عمومی Normalizer2WithImpl {
عمومی:
    DecomposeNormalizer2(ثابت Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    مجازی ~DecomposeNormalizer2();

خصوصی:
    مجازی تهی
    normalize(ثابت char16_t *src, ثابت char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت override {
        impl.decompose(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    مجازی تهی
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_میانه,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت override {
        impl.decomposeAndAppend(src, limit, doNormalize, safe_میانه, buffer, errorCode);
    }

    تهی
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ثابت override {
        اگر (U_FAILURE(errorCode)) {
            بازگشت;
        }
        اگر (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.data());
        impl.decomposeUTF8(options, s, s + src.length(), &sink, edits, errorCode);
        sink.Flush();
    }
    مجازی UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ثابت override {
        اگر(U_FAILURE(errorCode)) {
            بازگشت false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.data());
        ثابت uint8_t *sLimit = s + sp.length();
        بازگشت sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    مجازی ثابت char16_t*
    spanQuickCheckYes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* limit,
                      UErrorCode& errorCode) ثابت override {
        بازگشت impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    مجازی UNormalizationCheckResult getQuickCheck(UChar32 c) ثابت override {
        بازگشت impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    مجازی UBool hasBoundaryBefore(UChar32 c) ثابت override {
        بازگشت impl.hasDecompBoundaryBefore(c);
    }
    مجازی UBool hasBoundaryAfter(UChar32 c) ثابت override {
        بازگشت impl.hasDecompBoundaryAfter(c);
    }
    مجازی UBool isInert(UChar32 c) ثابت override {
        بازگشت impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : عمومی Normalizer2WithImpl {
عمومی:
    ComposeNormalizer2(ثابت Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    مجازی ~ComposeNormalizer2();

خصوصی:
    مجازی تهی
    normalize(ثابت char16_t *src, ثابت char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت override {
        impl.compose(src, limit, onlyContiguous, true, buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    تهی
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ثابت override {
        اگر (U_FAILURE(errorCode)) {
            بازگشت;
        }
        اگر (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.data());
        impl.composeUTF8(options, onlyContiguous, s, s + src.length(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    مجازی تهی
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_میانه,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safe_میانه, buffer, errorCode);
    }

    مجازی UBool
    isNormalized(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت override {
        اگر(U_FAILURE(errorCode)) {
            بازگشت false;
        }
        ثابت char16_t *sArray=s.getBuffer();
        اگر(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت false;
        }
        UnicodeString temp;
        ReorderingBuffer buffer(impl, temp);
        اگر(!buffer.init(5, errorCode)) {  // small destCapacity for substring normalization
            بازگشت false;
        }
        بازگشت impl.compose(sArray, sArray+s.length(), onlyContiguous, false, buffer, errorCode);
    }
    مجازی UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ثابت override {
        اگر(U_FAILURE(errorCode)) {
            بازگشت false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.data());
        بازگشت impl.composeUTF8(0, onlyContiguous, s, s + sp.length(), nullptr, nullptr, errorCode);
    }
    مجازی UNormalizationCheckResult
    quickCheck(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت override {
        اگر(U_FAILURE(errorCode)) {
            بازگشت UNORM_MAYBE;
        }
        ثابت char16_t *sArray=s.getBuffer();
        اگر(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت UNORM_MAYBE;
        }
        UNormalizationCheckResult qcResult=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.length(), onlyContiguous, &qcResult);
        بازگشت qcResult;
    }
    مجازی ثابت char16_t*
    spanQuickCheckYes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* limit,
                      UErrorCode&) ثابت override {
        بازگشت impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    مجازی UNormalizationCheckResult getQuickCheck(UChar32 c) ثابت override {
        بازگشت impl.getCompQuickCheck(impl.getNorm16(c));
    }
    مجازی UBool hasBoundaryBefore(UChar32 c) ثابت override {
        بازگشت impl.hasCompBoundaryBefore(c);
    }
    مجازی UBool hasBoundaryAfter(UChar32 c) ثابت override {
        بازگشت impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    مجازی UBool isInert(UChar32 c) ثابت override {
        بازگشت impl.isCompInert(c, onlyContiguous);
    }

    ثابت UBool onlyContiguous;
};

class FCDNormalizer2 : عمومی Normalizer2WithImpl {
عمومی:
    FCDNormalizer2(ثابت Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    مجازی ~FCDNormalizer2();

خصوصی:
    مجازی تهی
    normalize(ثابت char16_t *src, ثابت char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت override {
        impl.makeFCD(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    مجازی تهی
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_میانه,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safe_میانه, buffer, errorCode);
    }
    مجازی ثابت char16_t*
    spanQuickCheckYes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* limit,
                      UErrorCode& errorCode) ثابت override {
        بازگشت impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    مجازی UBool hasBoundaryBefore(UChar32 c) ثابت override {
        بازگشت impl.hasFCDBoundaryBefore(c);
    }
    مجازی UBool hasBoundaryAfter(UChar32 c) ثابت override {
        بازگشت impl.hasFCDBoundaryAfter(c);
    }
    مجازی UBool isInert(UChar32 c) ثابت override {
        بازگشت impl.isFCDInert(c);
    }
};

ساختار Norm2AllModes : عمومی UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    ایستا Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    ایستا Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    ایستا Norm2AllModes *createInstance(ثابت نویسه *packageName,
                                         ثابت نویسه *name,
                                         UErrorCode &errorCode);

    ایستا ثابت Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    ایستا ثابت Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    ایستا ثابت Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    ایستا ثابت Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
