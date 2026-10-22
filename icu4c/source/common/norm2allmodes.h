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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_NORMALIZATION

#அந்தர்பாவ "unicode/edits.h"
#அந்தர்பாவ "unicode/normalizer2.h"
#அந்தர்பாவ "unicode/stringoptions.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "cpputils.h"
#அந்தர்பாவ "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : பொது Normalizer2 {
பொது:
    Normalizer2WithImpl(மாறிலி Normalizer2Impl &ni) : impl(ni) {}
    மெய்நிகர் ~Normalizer2WithImpl();

    // normalize
    மெய்நிகர் UnicodeString&
    normalize(மாறிலி UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) மாறிலி override {
        எனில்(U_FAILURE(errorCode)) {
            dest.setToBogus();
            திரும்பு dest;
        }
        மாறிலி char16_t *sArray=src.getBuffer();
        எனில்(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            திரும்பு dest;
        }
        dest.remove();
        ReorderingBuffer buffer(impl, dest);
        எனில்(buffer.init(src.length(), errorCode)) {
            normalize(sArray, sArray+src.length(), buffer, errorCode);
        }
        திரும்பு dest;
    }
    மெய்நிகர் வெறுமை
    normalize(மாறிலி char16_t *src, மாறிலி char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) மாறிலி = 0;

    // normalize and append
    மெய்நிகர் UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             மாறிலி UnicodeString& second,
                             UErrorCode& errorCode) மாறிலி override {
        திரும்பு normalizeSecondAndAppend(first, second, true, errorCode);
    }
    மெய்நிகர் UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           மாறிலி UnicodeString& second,
           UErrorCode& errorCode) மாறிலி override {
        திரும்பு normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             மாறிலி UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) மாறிலி {
        uprv_checkCanGetBuffer(first, errorCode);
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு first;
        }
        மாறிலி char16_t *secondArray=second.getBuffer();
        எனில்(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு first;
        }
        int32_t firstLength=first.length();
        UnicodeString safe_நடு;
        {
            ReorderingBuffer buffer(impl, first);
            எனில்(buffer.init(firstLength+second.length(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.length(), doNormalize,
                                   safe_நடு, buffer, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        எனில்(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(firstLength-safe_நடு.length(), 0x7fffffff, safe_நடு);
        }
        திரும்பு first;
    }
    மெய்நிகர் வெறுமை
    normalizeAndAppend(மாறிலி char16_t *src, மாறிலி char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_நடு,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) மாறிலி = 0;
    மெய்நிகர் UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) மாறிலி override {
        char16_t buffer[4];
        int32_t length;
        மாறிலி char16_t *d=impl.getDecomposition(c, buffer, length);
        எனில்(d==nullptr) {
            திரும்பு false;
        }
        எனில்(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (Jamos from Hangul syllable c)
        } இல்லையெனில் {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        திரும்பு true;
    }
    மெய்நிகர் UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) மாறிலி override {
        char16_t buffer[30];
        int32_t length;
        மாறிலி char16_t *d=impl.getRawDecomposition(c, buffer, length);
        எனில்(d==nullptr) {
            திரும்பு false;
        }
        எனில்(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (algorithmic decomposition)
        } இல்லையெனில் {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        திரும்பு true;
    }
    மெய்நிகர் UChar32
    composePair(UChar32 a, UChar32 b) மாறிலி override {
        திரும்பு impl.composePair(a, b);
    }

    மெய்நிகர் uint8_t
    getCombiningClass(UChar32 c) மாறிலி override {
        திரும்பு impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    மெய்நிகர் UBool
    isNormalized(மாறிலி UnicodeString &s, UErrorCode &errorCode) மாறிலி override {
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு false;
        }
        மாறிலி char16_t *sArray=s.getBuffer();
        எனில்(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு false;
        }
        மாறிலி char16_t *sLimit=sArray+s.length();
        திரும்பு sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    மெய்நிகர் UNormalizationCheckResult
    quickCheck(மாறிலி UnicodeString &s, UErrorCode &errorCode) மாறிலி override {
        திரும்பு Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    மெய்நிகர் int32_t
    spanQuickCheckYes(மாறிலி UnicodeString &s, UErrorCode &errorCode) மாறிலி override {
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு 0;
        }
        மாறிலி char16_t *sArray=s.getBuffer();
        எனில்(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு 0;
        }
        திரும்பு static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.length(), errorCode) - sArray);
    }
    மெய்நிகர் மாறிலி char16_t*
    spanQuickCheckYes(மாறிலி char16_t* src U_LIFETIME_BOUND,
                      மாறிலி char16_t* limit,
                      UErrorCode& errorCode) மாறிலி = 0;

    மெய்நிகர் UNormalizationCheckResult getQuickCheck(UChar32) மாறிலி {
        திரும்பு UNORM_YES;
    }

    மாறிலி Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : பொது Normalizer2WithImpl {
பொது:
    DecomposeNormalizer2(மாறிலி Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    மெய்நிகர் ~DecomposeNormalizer2();

தனிப்பட்ட:
    மெய்நிகர் வெறுமை
    normalize(மாறிலி char16_t *src, மாறிலி char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) மாறிலி override {
        impl.decompose(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    மெய்நிகர் வெறுமை
    normalizeAndAppend(மாறிலி char16_t *src, மாறிலி char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_நடு,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) மாறிலி override {
        impl.decomposeAndAppend(src, limit, doNormalize, safe_நடு, buffer, errorCode);
    }

    வெறுமை
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) மாறிலி override {
        எனில் (U_FAILURE(errorCode)) {
            திரும்பு;
        }
        எனில் (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        மாறிலி uint8_t *s = reinterpret_cast<மாறிலி uint8_t *>(src.data());
        impl.decomposeUTF8(options, s, s + src.length(), &sink, edits, errorCode);
        sink.Flush();
    }
    மெய்நிகர் UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) மாறிலி override {
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு false;
        }
        மாறிலி uint8_t *s = reinterpret_cast<மாறிலி uint8_t *>(sp.data());
        மாறிலி uint8_t *sLimit = s + sp.length();
        திரும்பு sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    மெய்நிகர் மாறிலி char16_t*
    spanQuickCheckYes(மாறிலி char16_t* src U_LIFETIME_BOUND,
                      மாறிலி char16_t* limit,
                      UErrorCode& errorCode) மாறிலி override {
        திரும்பு impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    மெய்நிகர் UNormalizationCheckResult getQuickCheck(UChar32 c) மாறிலி override {
        திரும்பு impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    மெய்நிகர் UBool hasBoundaryBefore(UChar32 c) மாறிலி override {
        திரும்பு impl.hasDecompBoundaryBefore(c);
    }
    மெய்நிகர் UBool hasBoundaryAfter(UChar32 c) மாறிலி override {
        திரும்பு impl.hasDecompBoundaryAfter(c);
    }
    மெய்நிகர் UBool isInert(UChar32 c) மாறிலி override {
        திரும்பு impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : பொது Normalizer2WithImpl {
பொது:
    ComposeNormalizer2(மாறிலி Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    மெய்நிகர் ~ComposeNormalizer2();

தனிப்பட்ட:
    மெய்நிகர் வெறுமை
    normalize(மாறிலி char16_t *src, மாறிலி char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) மாறிலி override {
        impl.compose(src, limit, onlyContiguous, true, buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    வெறுமை
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) மாறிலி override {
        எனில் (U_FAILURE(errorCode)) {
            திரும்பு;
        }
        எனில் (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        மாறிலி uint8_t *s = reinterpret_cast<மாறிலி uint8_t *>(src.data());
        impl.composeUTF8(options, onlyContiguous, s, s + src.length(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    மெய்நிகர் வெறுமை
    normalizeAndAppend(மாறிலி char16_t *src, மாறிலி char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_நடு,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) மாறிலி override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safe_நடு, buffer, errorCode);
    }

    மெய்நிகர் UBool
    isNormalized(மாறிலி UnicodeString &s, UErrorCode &errorCode) மாறிலி override {
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு false;
        }
        மாறிலி char16_t *sArray=s.getBuffer();
        எனில்(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு false;
        }
        UnicodeString temp;
        ReorderingBuffer buffer(impl, temp);
        எனில்(!buffer.init(5, errorCode)) {  // small destCapacity for substring normalization
            திரும்பு false;
        }
        திரும்பு impl.compose(sArray, sArray+s.length(), onlyContiguous, false, buffer, errorCode);
    }
    மெய்நிகர் UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) மாறிலி override {
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு false;
        }
        மாறிலி uint8_t *s = reinterpret_cast<மாறிலி uint8_t *>(sp.data());
        திரும்பு impl.composeUTF8(0, onlyContiguous, s, s + sp.length(), nullptr, nullptr, errorCode);
    }
    மெய்நிகர் UNormalizationCheckResult
    quickCheck(மாறிலி UnicodeString &s, UErrorCode &errorCode) மாறிலி override {
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு UNORM_MAYBE;
        }
        மாறிலி char16_t *sArray=s.getBuffer();
        எனில்(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு UNORM_MAYBE;
        }
        UNormalizationCheckResult qcResult=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.length(), onlyContiguous, &qcResult);
        திரும்பு qcResult;
    }
    மெய்நிகர் மாறிலி char16_t*
    spanQuickCheckYes(மாறிலி char16_t* src U_LIFETIME_BOUND,
                      மாறிலி char16_t* limit,
                      UErrorCode&) மாறிலி override {
        திரும்பு impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    மெய்நிகர் UNormalizationCheckResult getQuickCheck(UChar32 c) மாறிலி override {
        திரும்பு impl.getCompQuickCheck(impl.getNorm16(c));
    }
    மெய்நிகர் UBool hasBoundaryBefore(UChar32 c) மாறிலி override {
        திரும்பு impl.hasCompBoundaryBefore(c);
    }
    மெய்நிகர் UBool hasBoundaryAfter(UChar32 c) மாறிலி override {
        திரும்பு impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    மெய்நிகர் UBool isInert(UChar32 c) மாறிலி override {
        திரும்பு impl.isCompInert(c, onlyContiguous);
    }

    மாறிலி UBool onlyContiguous;
};

class FCDNormalizer2 : பொது Normalizer2WithImpl {
பொது:
    FCDNormalizer2(மாறிலி Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    மெய்நிகர் ~FCDNormalizer2();

தனிப்பட்ட:
    மெய்நிகர் வெறுமை
    normalize(மாறிலி char16_t *src, மாறிலி char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) மாறிலி override {
        impl.makeFCD(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    மெய்நிகர் வெறுமை
    normalizeAndAppend(மாறிலி char16_t *src, மாறிலி char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_நடு,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) மாறிலி override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safe_நடு, buffer, errorCode);
    }
    மெய்நிகர் மாறிலி char16_t*
    spanQuickCheckYes(மாறிலி char16_t* src U_LIFETIME_BOUND,
                      மாறிலி char16_t* limit,
                      UErrorCode& errorCode) மாறிலி override {
        திரும்பு impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    மெய்நிகர் UBool hasBoundaryBefore(UChar32 c) மாறிலி override {
        திரும்பு impl.hasFCDBoundaryBefore(c);
    }
    மெய்நிகர் UBool hasBoundaryAfter(UChar32 c) மாறிலி override {
        திரும்பு impl.hasFCDBoundaryAfter(c);
    }
    மெய்நிகர் UBool isInert(UChar32 c) மாறிலி override {
        திரும்பு impl.isFCDInert(c);
    }
};

கட்டமைப்பு Norm2AllModes : பொது UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    நிலையான Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    நிலையான Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    நிலையான Norm2AllModes *createInstance(மாறிலி எழுத்து *packageName,
                                         மாறிலி எழுத்து *name,
                                         UErrorCode &errorCode);

    நிலையான மாறிலி Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    நிலையான மாறிலி Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    நிலையான மாறிலி Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    நிலையான மாறிலி Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
