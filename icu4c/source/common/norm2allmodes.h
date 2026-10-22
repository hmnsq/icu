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

#यदि !UCONFIG_NO_NORMALIZATION

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
    Normalizer2WithImpl(अचर Normalizer2Impl &ni) : impl(ni) {}
    आभास ~Normalizer2WithImpl();

    // normalize
    आभास UnicodeString&
    normalize(अचर UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) अचर override {
        यदि(U_FAILURE(errorCode)) {
            dest.setToBogus();
            फर्काऊ dest;
        }
        अचर char16_t *sArray=src.getBuffer();
        यदि(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            फर्काऊ dest;
        }
        dest.हटाओ();
        ReorderingBuffer buffer(impl, dest);
        यदि(buffer.init(src.length(), errorCode)) {
            normalize(sArray, sArray+src.length(), buffer, errorCode);
        }
        फर्काऊ dest;
    }
    आभास शून्य
    normalize(अचर char16_t *src, अचर char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) अचर = 0;

    // normalize and append
    आभास UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             अचर UnicodeString& second,
                             UErrorCode& errorCode) अचर override {
        फर्काऊ normalizeSecondAndAppend(first, second, true, errorCode);
    }
    आभास UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           अचर UnicodeString& second,
           UErrorCode& errorCode) अचर override {
        फर्काऊ normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             अचर UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) अचर {
        uprv_checkCanGetBuffer(first, errorCode);
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ first;
        }
        अचर char16_t *secondArray=second.getBuffer();
        यदि(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ first;
        }
        int32_t firstLength=first.length();
        UnicodeString safe_बीच;
        {
            ReorderingBuffer buffer(impl, first);
            यदि(buffer.init(firstLength+second.length(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.length(), doNormalize,
                                   safe_बीच, buffer, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        यदि(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(firstLength-safe_बीच.length(), 0x7fffffff, safe_बीच);
        }
        फर्काऊ first;
    }
    आभास शून्य
    normalizeAndAppend(अचर char16_t *src, अचर char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_बीच,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) अचर = 0;
    आभास UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) अचर override {
        char16_t buffer[4];
        int32_t length;
        अचर char16_t *d=impl.getDecomposition(c, buffer, length);
        यदि(d==nullptr) {
            फर्काऊ false;
        }
        यदि(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (Jamos from Hangul syllable c)
        } अन्यथा {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        फर्काऊ true;
    }
    आभास UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) अचर override {
        char16_t buffer[30];
        int32_t length;
        अचर char16_t *d=impl.getRawDecomposition(c, buffer, length);
        यदि(d==nullptr) {
            फर्काऊ false;
        }
        यदि(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (algorithmic decomposition)
        } अन्यथा {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        फर्काऊ true;
    }
    आभास UChar32
    composePair(UChar32 a, UChar32 b) अचर override {
        फर्काऊ impl.composePair(a, b);
    }

    आभास uint8_t
    getCombiningClass(UChar32 c) अचर override {
        फर्काऊ impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    आभास UBool
    isNormalized(अचर UnicodeString &s, UErrorCode &errorCode) अचर override {
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ false;
        }
        अचर char16_t *sArray=s.getBuffer();
        यदि(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ false;
        }
        अचर char16_t *sLimit=sArray+s.length();
        फर्काऊ sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    आभास UNormalizationCheckResult
    quickCheck(अचर UnicodeString &s, UErrorCode &errorCode) अचर override {
        फर्काऊ Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    आभास int32_t
    spanQuickCheckYes(अचर UnicodeString &s, UErrorCode &errorCode) अचर override {
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ 0;
        }
        अचर char16_t *sArray=s.getBuffer();
        यदि(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ 0;
        }
        फर्काऊ static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.length(), errorCode) - sArray);
    }
    आभास अचर char16_t*
    spanQuickCheckYes(अचर char16_t* src U_LIFETIME_BOUND,
                      अचर char16_t* limit,
                      UErrorCode& errorCode) अचर = 0;

    आभास UNormalizationCheckResult getQuickCheck(UChar32) अचर {
        फर्काऊ UNORM_YES;
    }

    अचर Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : सार्वजनिक Normalizer2WithImpl {
सार्वजनिक:
    DecomposeNormalizer2(अचर Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    आभास ~DecomposeNormalizer2();

गुप्त:
    आभास शून्य
    normalize(अचर char16_t *src, अचर char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) अचर override {
        impl.decompose(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    आभास शून्य
    normalizeAndAppend(अचर char16_t *src, अचर char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_बीच,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) अचर override {
        impl.decomposeAndAppend(src, limit, doNormalize, safe_बीच, buffer, errorCode);
    }

    शून्य
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) अचर override {
        यदि (U_FAILURE(errorCode)) {
            फर्काऊ;
        }
        यदि (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        अचर uint8_t *s = reinterpret_cast<अचर uint8_t *>(src.data());
        impl.decomposeUTF8(options, s, s + src.length(), &sink, edits, errorCode);
        sink.Flush();
    }
    आभास UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) अचर override {
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ false;
        }
        अचर uint8_t *s = reinterpret_cast<अचर uint8_t *>(sp.data());
        अचर uint8_t *sLimit = s + sp.length();
        फर्काऊ sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    आभास अचर char16_t*
    spanQuickCheckYes(अचर char16_t* src U_LIFETIME_BOUND,
                      अचर char16_t* limit,
                      UErrorCode& errorCode) अचर override {
        फर्काऊ impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    आभास UNormalizationCheckResult getQuickCheck(UChar32 c) अचर override {
        फर्काऊ impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    आभास UBool hasBoundaryBefore(UChar32 c) अचर override {
        फर्काऊ impl.hasDecompBoundaryBefore(c);
    }
    आभास UBool hasBoundaryAfter(UChar32 c) अचर override {
        फर्काऊ impl.hasDecompBoundaryAfter(c);
    }
    आभास UBool isInert(UChar32 c) अचर override {
        फर्काऊ impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : सार्वजनिक Normalizer2WithImpl {
सार्वजनिक:
    ComposeNormalizer2(अचर Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    आभास ~ComposeNormalizer2();

गुप्त:
    आभास शून्य
    normalize(अचर char16_t *src, अचर char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) अचर override {
        impl.compose(src, limit, onlyContiguous, true, buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    शून्य
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) अचर override {
        यदि (U_FAILURE(errorCode)) {
            फर्काऊ;
        }
        यदि (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        अचर uint8_t *s = reinterpret_cast<अचर uint8_t *>(src.data());
        impl.composeUTF8(options, onlyContiguous, s, s + src.length(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    आभास शून्य
    normalizeAndAppend(अचर char16_t *src, अचर char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_बीच,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) अचर override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safe_बीच, buffer, errorCode);
    }

    आभास UBool
    isNormalized(अचर UnicodeString &s, UErrorCode &errorCode) अचर override {
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ false;
        }
        अचर char16_t *sArray=s.getBuffer();
        यदि(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ false;
        }
        UnicodeString temp;
        ReorderingBuffer buffer(impl, temp);
        यदि(!buffer.init(5, errorCode)) {  // small destCapacity for substring normalization
            फर्काऊ false;
        }
        फर्काऊ impl.compose(sArray, sArray+s.length(), onlyContiguous, false, buffer, errorCode);
    }
    आभास UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) अचर override {
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ false;
        }
        अचर uint8_t *s = reinterpret_cast<अचर uint8_t *>(sp.data());
        फर्काऊ impl.composeUTF8(0, onlyContiguous, s, s + sp.length(), nullptr, nullptr, errorCode);
    }
    आभास UNormalizationCheckResult
    quickCheck(अचर UnicodeString &s, UErrorCode &errorCode) अचर override {
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ UNORM_MAYBE;
        }
        अचर char16_t *sArray=s.getBuffer();
        यदि(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ UNORM_MAYBE;
        }
        UNormalizationCheckResult qcResult=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.length(), onlyContiguous, &qcResult);
        फर्काऊ qcResult;
    }
    आभास अचर char16_t*
    spanQuickCheckYes(अचर char16_t* src U_LIFETIME_BOUND,
                      अचर char16_t* limit,
                      UErrorCode&) अचर override {
        फर्काऊ impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    आभास UNormalizationCheckResult getQuickCheck(UChar32 c) अचर override {
        फर्काऊ impl.getCompQuickCheck(impl.getNorm16(c));
    }
    आभास UBool hasBoundaryBefore(UChar32 c) अचर override {
        फर्काऊ impl.hasCompBoundaryBefore(c);
    }
    आभास UBool hasBoundaryAfter(UChar32 c) अचर override {
        फर्काऊ impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    आभास UBool isInert(UChar32 c) अचर override {
        फर्काऊ impl.isCompInert(c, onlyContiguous);
    }

    अचर UBool onlyContiguous;
};

class FCDNormalizer2 : सार्वजनिक Normalizer2WithImpl {
सार्वजनिक:
    FCDNormalizer2(अचर Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    आभास ~FCDNormalizer2();

गुप्त:
    आभास शून्य
    normalize(अचर char16_t *src, अचर char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) अचर override {
        impl.makeFCD(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    आभास शून्य
    normalizeAndAppend(अचर char16_t *src, अचर char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_बीच,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) अचर override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safe_बीच, buffer, errorCode);
    }
    आभास अचर char16_t*
    spanQuickCheckYes(अचर char16_t* src U_LIFETIME_BOUND,
                      अचर char16_t* limit,
                      UErrorCode& errorCode) अचर override {
        फर्काऊ impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    आभास UBool hasBoundaryBefore(UChar32 c) अचर override {
        फर्काऊ impl.hasFCDBoundaryBefore(c);
    }
    आभास UBool hasBoundaryAfter(UChar32 c) अचर override {
        फर्काऊ impl.hasFCDBoundaryAfter(c);
    }
    आभास UBool isInert(UChar32 c) अचर override {
        फर्काऊ impl.isFCDInert(c);
    }
};

संरचना Norm2AllModes : सार्वजनिक UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    स्थिर Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    स्थिर Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    स्थिर Norm2AllModes *createInstance(अचर वर्ण *packageName,
                                         अचर वर्ण *name,
                                         UErrorCode &errorCode);

    स्थिर अचर Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    स्थिर अचर Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    स्थिर अचर Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    स्थिर अचर Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
