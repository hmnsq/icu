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

#अन्तर्भाव "unicode/utypes.h"

#सचे !UCONFIG_NO_NORMALIZATION

#अन्तर्भाव "unicode/edits.h"
#अन्तर्भाव "unicode/normalizer2.h"
#अन्तर्भाव "unicode/stringoptions.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "cpputils.h"
#अन्तर्भाव "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : सार्वजनिक Normalizer2 {
सार्वजनिक:
    Normalizer2WithImpl(ध्रुव Normalizer2Impl &ni) : impl(ni) {}
    आभास ~Normalizer2WithImpl();

    // normalize
    आभास UnicodeString&
    normalize(ध्रुव UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) ध्रुव override {
        सचे(U_FAILURE(errorCode)) {
            dest.setToBogus();
            पटिनिवत्त dest;
        }
        ध्रुव char16_t *sArray=src.getBuffer();
        सचे(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            पटिनिवत्त dest;
        }
        dest.remove();
        ReorderingBuffer buffer(impl, dest);
        सचे(buffer.init(src.length(), errorCode)) {
            normalize(sArray, sArray+src.length(), buffer, errorCode);
        }
        पटिनिवत्त dest;
    }
    आभास शून्य
    normalize(ध्रुव char16_t *src, ध्रुव char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ध्रुव = 0;

    // normalize and append
    आभास UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ध्रुव UnicodeString& second,
                             UErrorCode& errorCode) ध्रुव override {
        पटिनिवत्त normalizeSecondAndAppend(first, second, true, errorCode);
    }
    आभास UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           ध्रुव UnicodeString& second,
           UErrorCode& errorCode) ध्रुव override {
        पटिनिवत्त normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ध्रुव UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) ध्रुव {
        uprv_checkCanGetBuffer(first, errorCode);
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त first;
        }
        ध्रुव char16_t *secondArray=second.getBuffer();
        सचे(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            पटिनिवत्त first;
        }
        int32_t firstLength=first.length();
        UnicodeString safe_मध्यः;
        {
            ReorderingBuffer buffer(impl, first);
            सचे(buffer.init(firstLength+second.length(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.length(), doNormalize,
                                   safe_मध्यः, buffer, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        सचे(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(firstLength-safe_मध्यः.length(), 0x7fffffff, safe_मध्यः);
        }
        पटिनिवत्त first;
    }
    आभास शून्य
    normalizeAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_मध्यः,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ध्रुव = 0;
    आभास UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ध्रुव override {
        char16_t buffer[4];
        int32_t length;
        ध्रुव char16_t *d=impl.getDecomposition(c, buffer, length);
        सचे(d==nullptr) {
            पटिनिवत्त false;
        }
        सचे(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (Jamos from Hangul syllable c)
        } अञ्ञथा {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        पटिनिवत्त true;
    }
    आभास UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ध्रुव override {
        char16_t buffer[30];
        int32_t length;
        ध्रुव char16_t *d=impl.getRawDecomposition(c, buffer, length);
        सचे(d==nullptr) {
            पटिनिवत्त false;
        }
        सचे(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (algorithmic decomposition)
        } अञ्ञथा {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        पटिनिवत्त true;
    }
    आभास UChar32
    composePair(UChar32 a, UChar32 b) ध्रुव override {
        पटिनिवत्त impl.composePair(a, b);
    }

    आभास uint8_t
    getCombiningClass(UChar32 c) ध्रुव override {
        पटिनिवत्त impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    आभास UBool
    isNormalized(ध्रुव UnicodeString &s, UErrorCode &errorCode) ध्रुव override {
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त false;
        }
        ध्रुव char16_t *sArray=s.getBuffer();
        सचे(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            पटिनिवत्त false;
        }
        ध्रुव char16_t *sLimit=sArray+s.length();
        पटिनिवत्त sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    आभास UNormalizationCheckResult
    quickCheck(ध्रुव UnicodeString &s, UErrorCode &errorCode) ध्रुव override {
        पटिनिवत्त Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    आभास int32_t
    spanQuickCheckYes(ध्रुव UnicodeString &s, UErrorCode &errorCode) ध्रुव override {
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त 0;
        }
        ध्रुव char16_t *sArray=s.getBuffer();
        सचे(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            पटिनिवत्त 0;
        }
        पटिनिवत्त static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.length(), errorCode) - sArray);
    }
    आभास ध्रुव char16_t*
    spanQuickCheckYes(ध्रुव char16_t* src U_LIFETIME_BOUND,
                      ध्रुव char16_t* limit,
                      UErrorCode& errorCode) ध्रुव = 0;

    आभास UNormalizationCheckResult getQuickCheck(UChar32) ध्रुव {
        पटिनिवत्त UNORM_YES;
    }

    ध्रुव Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : सार्वजनिक Normalizer2WithImpl {
सार्वजनिक:
    DecomposeNormalizer2(ध्रुव Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    आभास ~DecomposeNormalizer2();

गुप्त:
    आभास शून्य
    normalize(ध्रुव char16_t *src, ध्रुव char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ध्रुव override {
        impl.decompose(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    आभास शून्य
    normalizeAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_मध्यः,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ध्रुव override {
        impl.decomposeAndAppend(src, limit, doNormalize, safe_मध्यः, buffer, errorCode);
    }

    शून्य
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ध्रुव override {
        सचे (U_FAILURE(errorCode)) {
            पटिनिवत्त;
        }
        सचे (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ध्रुव uint8_t *s = reinterpret_cast<ध्रुव uint8_t *>(src.data());
        impl.decomposeUTF8(options, s, s + src.length(), &sink, edits, errorCode);
        sink.Flush();
    }
    आभास UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ध्रुव override {
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त false;
        }
        ध्रुव uint8_t *s = reinterpret_cast<ध्रुव uint8_t *>(sp.data());
        ध्रुव uint8_t *sLimit = s + sp.length();
        पटिनिवत्त sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    आभास ध्रुव char16_t*
    spanQuickCheckYes(ध्रुव char16_t* src U_LIFETIME_BOUND,
                      ध्रुव char16_t* limit,
                      UErrorCode& errorCode) ध्रुव override {
        पटिनिवत्त impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    आभास UNormalizationCheckResult getQuickCheck(UChar32 c) ध्रुव override {
        पटिनिवत्त impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    आभास UBool hasBoundaryBefore(UChar32 c) ध्रुव override {
        पटिनिवत्त impl.hasDecompBoundaryBefore(c);
    }
    आभास UBool hasBoundaryAfter(UChar32 c) ध्रुव override {
        पटिनिवत्त impl.hasDecompBoundaryAfter(c);
    }
    आभास UBool isInert(UChar32 c) ध्रुव override {
        पटिनिवत्त impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : सार्वजनिक Normalizer2WithImpl {
सार्वजनिक:
    ComposeNormalizer2(ध्रुव Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    आभास ~ComposeNormalizer2();

गुप्त:
    आभास शून्य
    normalize(ध्रुव char16_t *src, ध्रुव char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ध्रुव override {
        impl.compose(src, limit, onlyContiguous, true, buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    शून्य
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ध्रुव override {
        सचे (U_FAILURE(errorCode)) {
            पटिनिवत्त;
        }
        सचे (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ध्रुव uint8_t *s = reinterpret_cast<ध्रुव uint8_t *>(src.data());
        impl.composeUTF8(options, onlyContiguous, s, s + src.length(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    आभास शून्य
    normalizeAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_मध्यः,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ध्रुव override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safe_मध्यः, buffer, errorCode);
    }

    आभास UBool
    isNormalized(ध्रुव UnicodeString &s, UErrorCode &errorCode) ध्रुव override {
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त false;
        }
        ध्रुव char16_t *sArray=s.getBuffer();
        सचे(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            पटिनिवत्त false;
        }
        UnicodeString temp;
        ReorderingBuffer buffer(impl, temp);
        सचे(!buffer.init(5, errorCode)) {  // small destCapacity for substring normalization
            पटिनिवत्त false;
        }
        पटिनिवत्त impl.compose(sArray, sArray+s.length(), onlyContiguous, false, buffer, errorCode);
    }
    आभास UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ध्रुव override {
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त false;
        }
        ध्रुव uint8_t *s = reinterpret_cast<ध्रुव uint8_t *>(sp.data());
        पटिनिवत्त impl.composeUTF8(0, onlyContiguous, s, s + sp.length(), nullptr, nullptr, errorCode);
    }
    आभास UNormalizationCheckResult
    quickCheck(ध्रुव UnicodeString &s, UErrorCode &errorCode) ध्रुव override {
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त UNORM_MAYBE;
        }
        ध्रुव char16_t *sArray=s.getBuffer();
        सचे(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            पटिनिवत्त UNORM_MAYBE;
        }
        UNormalizationCheckResult qcResult=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.length(), onlyContiguous, &qcResult);
        पटिनिवत्त qcResult;
    }
    आभास ध्रुव char16_t*
    spanQuickCheckYes(ध्रुव char16_t* src U_LIFETIME_BOUND,
                      ध्रुव char16_t* limit,
                      UErrorCode&) ध्रुव override {
        पटिनिवत्त impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    आभास UNormalizationCheckResult getQuickCheck(UChar32 c) ध्रुव override {
        पटिनिवत्त impl.getCompQuickCheck(impl.getNorm16(c));
    }
    आभास UBool hasBoundaryBefore(UChar32 c) ध्रुव override {
        पटिनिवत्त impl.hasCompBoundaryBefore(c);
    }
    आभास UBool hasBoundaryAfter(UChar32 c) ध्रुव override {
        पटिनिवत्त impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    आभास UBool isInert(UChar32 c) ध्रुव override {
        पटिनिवत्त impl.isCompInert(c, onlyContiguous);
    }

    ध्रुव UBool onlyContiguous;
};

class FCDNormalizer2 : सार्वजनिक Normalizer2WithImpl {
सार्वजनिक:
    FCDNormalizer2(ध्रुव Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    आभास ~FCDNormalizer2();

गुप्त:
    आभास शून्य
    normalize(ध्रुव char16_t *src, ध्रुव char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ध्रुव override {
        impl.makeFCD(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    आभास शून्य
    normalizeAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_मध्यः,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ध्रुव override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safe_मध्यः, buffer, errorCode);
    }
    आभास ध्रुव char16_t*
    spanQuickCheckYes(ध्रुव char16_t* src U_LIFETIME_BOUND,
                      ध्रुव char16_t* limit,
                      UErrorCode& errorCode) ध्रुव override {
        पटिनिवत्त impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    आभास UBool hasBoundaryBefore(UChar32 c) ध्रुव override {
        पटिनिवत्त impl.hasFCDBoundaryBefore(c);
    }
    आभास UBool hasBoundaryAfter(UChar32 c) ध्रुव override {
        पटिनिवत्त impl.hasFCDBoundaryAfter(c);
    }
    आभास UBool isInert(UChar32 c) ध्रुव override {
        पटिनिवत्त impl.isFCDInert(c);
    }
};

संरचना Norm2AllModes : सार्वजनिक UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    स्थिर Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    स्थिर Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    स्थिर Norm2AllModes *createInstance(ध्रुव वर्ण *packageName,
                                         ध्रुव वर्ण *name,
                                         UErrorCode &errorCode);

    स्थिर ध्रुव Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    स्थिर ध्रुव Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    स्थिर ध्रुव Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    स्थिर ध्रुव Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
