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

#जर !UCONFIG_NO_NORMALIZATION

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
    Normalizer2WithImpl(स्थिरांक Normalizer2Impl &ni) : impl(ni) {}
    आभासी ~Normalizer2WithImpl();

    // normalize
    आभासी UnicodeString&
    normalize(स्थिरांक UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) स्थिरांक override {
        जर(U_FAILURE(errorCode)) {
            dest.setToBogus();
            परत dest;
        }
        स्थिरांक char16_t *sArray=src.getBuffer();
        जर(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            परत dest;
        }
        dest.हटाओ();
        ReorderingBuffer buffer(impl, dest);
        जर(buffer.init(src.length(), errorCode)) {
            normalize(sArray, sArray+src.length(), buffer, errorCode);
        }
        परत dest;
    }
    आभासी शून्य
    normalize(स्थिरांक char16_t *src, स्थिरांक char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) स्थिरांक = 0;

    // normalize and append
    आभासी UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             स्थिरांक UnicodeString& second,
                             UErrorCode& errorCode) स्थिरांक override {
        परत normalizeSecondAndAppend(first, second, true, errorCode);
    }
    आभासी UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           स्थिरांक UnicodeString& second,
           UErrorCode& errorCode) स्थिरांक override {
        परत normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             स्थिरांक UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) स्थिरांक {
        uprv_checkCanGetBuffer(first, errorCode);
        जर(U_FAILURE(errorCode)) {
            परत first;
        }
        स्थिरांक char16_t *secondArray=second.getBuffer();
        जर(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            परत first;
        }
        int32_t firstLength=first.length();
        UnicodeString safe_मधला;
        {
            ReorderingBuffer buffer(impl, first);
            जर(buffer.init(firstLength+second.length(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.length(), doNormalize,
                                   safe_मधला, buffer, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        जर(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(firstLength-safe_मधला.length(), 0x7fffffff, safe_मधला);
        }
        परत first;
    }
    आभासी शून्य
    normalizeAndAppend(स्थिरांक char16_t *src, स्थिरांक char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_मधला,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) स्थिरांक = 0;
    आभासी UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) स्थिरांक override {
        char16_t buffer[4];
        int32_t length;
        स्थिरांक char16_t *d=impl.getDecomposition(c, buffer, length);
        जर(d==nullptr) {
            परत false;
        }
        जर(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (Jamos from Hangul syllable c)
        } अन्यथा {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        परत true;
    }
    आभासी UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) स्थिरांक override {
        char16_t buffer[30];
        int32_t length;
        स्थिरांक char16_t *d=impl.getRawDecomposition(c, buffer, length);
        जर(d==nullptr) {
            परत false;
        }
        जर(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (algorithmic decomposition)
        } अन्यथा {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        परत true;
    }
    आभासी UChar32
    composePair(UChar32 a, UChar32 b) स्थिरांक override {
        परत impl.composePair(a, b);
    }

    आभासी uint8_t
    getCombiningClass(UChar32 c) स्थिरांक override {
        परत impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    आभासी UBool
    isNormalized(स्थिरांक UnicodeString &s, UErrorCode &errorCode) स्थिरांक override {
        जर(U_FAILURE(errorCode)) {
            परत false;
        }
        स्थिरांक char16_t *sArray=s.getBuffer();
        जर(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            परत false;
        }
        स्थिरांक char16_t *sLimit=sArray+s.length();
        परत sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    आभासी UNormalizationCheckResult
    quickCheck(स्थिरांक UnicodeString &s, UErrorCode &errorCode) स्थिरांक override {
        परत Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    आभासी int32_t
    spanQuickCheckYes(स्थिरांक UnicodeString &s, UErrorCode &errorCode) स्थिरांक override {
        जर(U_FAILURE(errorCode)) {
            परत 0;
        }
        स्थिरांक char16_t *sArray=s.getBuffer();
        जर(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            परत 0;
        }
        परत static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.length(), errorCode) - sArray);
    }
    आभासी स्थिरांक char16_t*
    spanQuickCheckYes(स्थिरांक char16_t* src U_LIFETIME_BOUND,
                      स्थिरांक char16_t* limit,
                      UErrorCode& errorCode) स्थिरांक = 0;

    आभासी UNormalizationCheckResult getQuickCheck(UChar32) स्थिरांक {
        परत UNORM_YES;
    }

    स्थिरांक Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : सार्वजनिक Normalizer2WithImpl {
सार्वजनिक:
    DecomposeNormalizer2(स्थिरांक Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    आभासी ~DecomposeNormalizer2();

खाजगी:
    आभासी शून्य
    normalize(स्थिरांक char16_t *src, स्थिरांक char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) स्थिरांक override {
        impl.decompose(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    आभासी शून्य
    normalizeAndAppend(स्थिरांक char16_t *src, स्थिरांक char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_मधला,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) स्थिरांक override {
        impl.decomposeAndAppend(src, limit, doNormalize, safe_मधला, buffer, errorCode);
    }

    शून्य
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) स्थिरांक override {
        जर (U_FAILURE(errorCode)) {
            परत;
        }
        जर (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        स्थिरांक uint8_t *s = reinterpret_cast<स्थिरांक uint8_t *>(src.data());
        impl.decomposeUTF8(options, s, s + src.length(), &sink, edits, errorCode);
        sink.Flush();
    }
    आभासी UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) स्थिरांक override {
        जर(U_FAILURE(errorCode)) {
            परत false;
        }
        स्थिरांक uint8_t *s = reinterpret_cast<स्थिरांक uint8_t *>(sp.data());
        स्थिरांक uint8_t *sLimit = s + sp.length();
        परत sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    आभासी स्थिरांक char16_t*
    spanQuickCheckYes(स्थिरांक char16_t* src U_LIFETIME_BOUND,
                      स्थिरांक char16_t* limit,
                      UErrorCode& errorCode) स्थिरांक override {
        परत impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    आभासी UNormalizationCheckResult getQuickCheck(UChar32 c) स्थिरांक override {
        परत impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    आभासी UBool hasBoundaryBefore(UChar32 c) स्थिरांक override {
        परत impl.hasDecompBoundaryBefore(c);
    }
    आभासी UBool hasBoundaryAfter(UChar32 c) स्थिरांक override {
        परत impl.hasDecompBoundaryAfter(c);
    }
    आभासी UBool isInert(UChar32 c) स्थिरांक override {
        परत impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : सार्वजनिक Normalizer2WithImpl {
सार्वजनिक:
    ComposeNormalizer2(स्थिरांक Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    आभासी ~ComposeNormalizer2();

खाजगी:
    आभासी शून्य
    normalize(स्थिरांक char16_t *src, स्थिरांक char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) स्थिरांक override {
        impl.compose(src, limit, onlyContiguous, true, buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    शून्य
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) स्थिरांक override {
        जर (U_FAILURE(errorCode)) {
            परत;
        }
        जर (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        स्थिरांक uint8_t *s = reinterpret_cast<स्थिरांक uint8_t *>(src.data());
        impl.composeUTF8(options, onlyContiguous, s, s + src.length(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    आभासी शून्य
    normalizeAndAppend(स्थिरांक char16_t *src, स्थिरांक char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_मधला,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) स्थिरांक override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safe_मधला, buffer, errorCode);
    }

    आभासी UBool
    isNormalized(स्थिरांक UnicodeString &s, UErrorCode &errorCode) स्थिरांक override {
        जर(U_FAILURE(errorCode)) {
            परत false;
        }
        स्थिरांक char16_t *sArray=s.getBuffer();
        जर(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            परत false;
        }
        UnicodeString temp;
        ReorderingBuffer buffer(impl, temp);
        जर(!buffer.init(5, errorCode)) {  // small destCapacity for substring normalization
            परत false;
        }
        परत impl.compose(sArray, sArray+s.length(), onlyContiguous, false, buffer, errorCode);
    }
    आभासी UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) स्थिरांक override {
        जर(U_FAILURE(errorCode)) {
            परत false;
        }
        स्थिरांक uint8_t *s = reinterpret_cast<स्थिरांक uint8_t *>(sp.data());
        परत impl.composeUTF8(0, onlyContiguous, s, s + sp.length(), nullptr, nullptr, errorCode);
    }
    आभासी UNormalizationCheckResult
    quickCheck(स्थिरांक UnicodeString &s, UErrorCode &errorCode) स्थिरांक override {
        जर(U_FAILURE(errorCode)) {
            परत UNORM_MAYBE;
        }
        स्थिरांक char16_t *sArray=s.getBuffer();
        जर(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            परत UNORM_MAYBE;
        }
        UNormalizationCheckResult qcResult=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.length(), onlyContiguous, &qcResult);
        परत qcResult;
    }
    आभासी स्थिरांक char16_t*
    spanQuickCheckYes(स्थिरांक char16_t* src U_LIFETIME_BOUND,
                      स्थिरांक char16_t* limit,
                      UErrorCode&) स्थिरांक override {
        परत impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    आभासी UNormalizationCheckResult getQuickCheck(UChar32 c) स्थिरांक override {
        परत impl.getCompQuickCheck(impl.getNorm16(c));
    }
    आभासी UBool hasBoundaryBefore(UChar32 c) स्थिरांक override {
        परत impl.hasCompBoundaryBefore(c);
    }
    आभासी UBool hasBoundaryAfter(UChar32 c) स्थिरांक override {
        परत impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    आभासी UBool isInert(UChar32 c) स्थिरांक override {
        परत impl.isCompInert(c, onlyContiguous);
    }

    स्थिरांक UBool onlyContiguous;
};

class FCDNormalizer2 : सार्वजनिक Normalizer2WithImpl {
सार्वजनिक:
    FCDNormalizer2(स्थिरांक Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    आभासी ~FCDNormalizer2();

खाजगी:
    आभासी शून्य
    normalize(स्थिरांक char16_t *src, स्थिरांक char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) स्थिरांक override {
        impl.makeFCD(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    आभासी शून्य
    normalizeAndAppend(स्थिरांक char16_t *src, स्थिरांक char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_मधला,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) स्थिरांक override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safe_मधला, buffer, errorCode);
    }
    आभासी स्थिरांक char16_t*
    spanQuickCheckYes(स्थिरांक char16_t* src U_LIFETIME_BOUND,
                      स्थिरांक char16_t* limit,
                      UErrorCode& errorCode) स्थिरांक override {
        परत impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    आभासी UBool hasBoundaryBefore(UChar32 c) स्थिरांक override {
        परत impl.hasFCDBoundaryBefore(c);
    }
    आभासी UBool hasBoundaryAfter(UChar32 c) स्थिरांक override {
        परत impl.hasFCDBoundaryAfter(c);
    }
    आभासी UBool isInert(UChar32 c) स्थिरांक override {
        परत impl.isFCDInert(c);
    }
};

संरचना Norm2AllModes : सार्वजनिक UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    स्थिर Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    स्थिर Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    स्थिर Norm2AllModes *createInstance(स्थिरांक अक्षर *packageName,
                                         स्थिरांक अक्षर *name,
                                         UErrorCode &errorCode);

    स्थिर स्थिरांक Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    स्थिर स्थिरांक Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    स्थिर स्थिरांक Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    स्थिर स्थिरांक Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
