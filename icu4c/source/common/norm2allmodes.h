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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_NORMALIZATION

#അന്തര്ഭാവ "unicode/edits.h"
#അന്തര്ഭാവ "unicode/normalizer2.h"
#അന്തര്ഭാവ "unicode/stringoptions.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "cpputils.h"
#അന്തര്ഭാവ "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : പൊതു Normalizer2 {
പൊതു:
    Normalizer2WithImpl(സ്ഥിരം Normalizer2Impl &ni) : impl(ni) {}
    മായാ ~Normalizer2WithImpl();

    // normalize
    മായാ UnicodeString&
    normalize(സ്ഥിരം UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) സ്ഥിരം override {
        എങ്കിൽ(U_FAILURE(errorCode)) {
            dest.setToBogus();
            തിരികെ dest;
        }
        സ്ഥിരം char16_t *sArray=src.getBuffer();
        എങ്കിൽ(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            തിരികെ dest;
        }
        dest.remove();
        ReorderingBuffer buffer(impl, dest);
        എങ്കിൽ(buffer.init(src.length(), errorCode)) {
            normalize(sArray, sArray+src.length(), buffer, errorCode);
        }
        തിരികെ dest;
    }
    മായാ ശൂന്യം
    normalize(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) സ്ഥിരം = 0;

    // normalize and append
    മായാ UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             സ്ഥിരം UnicodeString& second,
                             UErrorCode& errorCode) സ്ഥിരം override {
        തിരികെ normalizeSecondAndAppend(first, second, true, errorCode);
    }
    മായാ UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           സ്ഥിരം UnicodeString& second,
           UErrorCode& errorCode) സ്ഥിരം override {
        തിരികെ normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             സ്ഥിരം UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) സ്ഥിരം {
        uprv_checkCanGetBuffer(first, errorCode);
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ first;
        }
        സ്ഥിരം char16_t *secondArray=second.getBuffer();
        എങ്കിൽ(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ first;
        }
        int32_t firstLength=first.length();
        UnicodeString safe_നടുവ്;
        {
            ReorderingBuffer buffer(impl, first);
            എങ്കിൽ(buffer.init(firstLength+second.length(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.length(), doNormalize,
                                   safe_നടുവ്, buffer, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        എങ്കിൽ(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(firstLength-safe_നടുവ്.length(), 0x7fffffff, safe_നടുവ്);
        }
        തിരികെ first;
    }
    മായാ ശൂന്യം
    normalizeAndAppend(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_നടുവ്,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) സ്ഥിരം = 0;
    മായാ UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) സ്ഥിരം override {
        char16_t buffer[4];
        int32_t length;
        സ്ഥിരം char16_t *d=impl.getDecomposition(c, buffer, length);
        എങ്കിൽ(d==nullptr) {
            തിരികെ false;
        }
        എങ്കിൽ(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (Jamos from Hangul syllable c)
        } അല്ലെങ്കിൽ {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        തിരികെ true;
    }
    മായാ UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) സ്ഥിരം override {
        char16_t buffer[30];
        int32_t length;
        സ്ഥിരം char16_t *d=impl.getRawDecomposition(c, buffer, length);
        എങ്കിൽ(d==nullptr) {
            തിരികെ false;
        }
        എങ്കിൽ(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (algorithmic decomposition)
        } അല്ലെങ്കിൽ {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        തിരികെ true;
    }
    മായാ UChar32
    composePair(UChar32 a, UChar32 b) സ്ഥിരം override {
        തിരികെ impl.composePair(a, b);
    }

    മായാ uint8_t
    getCombiningClass(UChar32 c) സ്ഥിരം override {
        തിരികെ impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    മായാ UBool
    isNormalized(സ്ഥിരം UnicodeString &s, UErrorCode &errorCode) സ്ഥിരം override {
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ false;
        }
        സ്ഥിരം char16_t *sArray=s.getBuffer();
        എങ്കിൽ(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ false;
        }
        സ്ഥിരം char16_t *sLimit=sArray+s.length();
        തിരികെ sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    മായാ UNormalizationCheckResult
    quickCheck(സ്ഥിരം UnicodeString &s, UErrorCode &errorCode) സ്ഥിരം override {
        തിരികെ Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    മായാ int32_t
    spanQuickCheckYes(സ്ഥിരം UnicodeString &s, UErrorCode &errorCode) സ്ഥിരം override {
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ 0;
        }
        സ്ഥിരം char16_t *sArray=s.getBuffer();
        എങ്കിൽ(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ 0;
        }
        തിരികെ static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.length(), errorCode) - sArray);
    }
    മായാ സ്ഥിരം char16_t*
    spanQuickCheckYes(സ്ഥിരം char16_t* src U_LIFETIME_BOUND,
                      സ്ഥിരം char16_t* limit,
                      UErrorCode& errorCode) സ്ഥിരം = 0;

    മായാ UNormalizationCheckResult getQuickCheck(UChar32) സ്ഥിരം {
        തിരികെ UNORM_YES;
    }

    സ്ഥിരം Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : പൊതു Normalizer2WithImpl {
പൊതു:
    DecomposeNormalizer2(സ്ഥിരം Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    മായാ ~DecomposeNormalizer2();

സ്വകാര്യ:
    മായാ ശൂന്യം
    normalize(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) സ്ഥിരം override {
        impl.decompose(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    മായാ ശൂന്യം
    normalizeAndAppend(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_നടുവ്,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) സ്ഥിരം override {
        impl.decomposeAndAppend(src, limit, doNormalize, safe_നടുവ്, buffer, errorCode);
    }

    ശൂന്യം
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) സ്ഥിരം override {
        എങ്കിൽ (U_FAILURE(errorCode)) {
            തിരികെ;
        }
        എങ്കിൽ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        സ്ഥിരം uint8_t *s = reinterpret_cast<സ്ഥിരം uint8_t *>(src.data());
        impl.decomposeUTF8(options, s, s + src.length(), &sink, edits, errorCode);
        sink.Flush();
    }
    മായാ UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) സ്ഥിരം override {
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ false;
        }
        സ്ഥിരം uint8_t *s = reinterpret_cast<സ്ഥിരം uint8_t *>(sp.data());
        സ്ഥിരം uint8_t *sLimit = s + sp.length();
        തിരികെ sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    മായാ സ്ഥിരം char16_t*
    spanQuickCheckYes(സ്ഥിരം char16_t* src U_LIFETIME_BOUND,
                      സ്ഥിരം char16_t* limit,
                      UErrorCode& errorCode) സ്ഥിരം override {
        തിരികെ impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    മായാ UNormalizationCheckResult getQuickCheck(UChar32 c) സ്ഥിരം override {
        തിരികെ impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    മായാ UBool hasBoundaryBefore(UChar32 c) സ്ഥിരം override {
        തിരികെ impl.hasDecompBoundaryBefore(c);
    }
    മായാ UBool hasBoundaryAfter(UChar32 c) സ്ഥിരം override {
        തിരികെ impl.hasDecompBoundaryAfter(c);
    }
    മായാ UBool isInert(UChar32 c) സ്ഥിരം override {
        തിരികെ impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : പൊതു Normalizer2WithImpl {
പൊതു:
    ComposeNormalizer2(സ്ഥിരം Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    മായാ ~ComposeNormalizer2();

സ്വകാര്യ:
    മായാ ശൂന്യം
    normalize(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) സ്ഥിരം override {
        impl.compose(src, limit, onlyContiguous, true, buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    ശൂന്യം
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) സ്ഥിരം override {
        എങ്കിൽ (U_FAILURE(errorCode)) {
            തിരികെ;
        }
        എങ്കിൽ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        സ്ഥിരം uint8_t *s = reinterpret_cast<സ്ഥിരം uint8_t *>(src.data());
        impl.composeUTF8(options, onlyContiguous, s, s + src.length(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    മായാ ശൂന്യം
    normalizeAndAppend(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_നടുവ്,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) സ്ഥിരം override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safe_നടുവ്, buffer, errorCode);
    }

    മായാ UBool
    isNormalized(സ്ഥിരം UnicodeString &s, UErrorCode &errorCode) സ്ഥിരം override {
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ false;
        }
        സ്ഥിരം char16_t *sArray=s.getBuffer();
        എങ്കിൽ(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ false;
        }
        UnicodeString temp;
        ReorderingBuffer buffer(impl, temp);
        എങ്കിൽ(!buffer.init(5, errorCode)) {  // small destCapacity for substring normalization
            തിരികെ false;
        }
        തിരികെ impl.compose(sArray, sArray+s.length(), onlyContiguous, false, buffer, errorCode);
    }
    മായാ UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) സ്ഥിരം override {
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ false;
        }
        സ്ഥിരം uint8_t *s = reinterpret_cast<സ്ഥിരം uint8_t *>(sp.data());
        തിരികെ impl.composeUTF8(0, onlyContiguous, s, s + sp.length(), nullptr, nullptr, errorCode);
    }
    മായാ UNormalizationCheckResult
    quickCheck(സ്ഥിരം UnicodeString &s, UErrorCode &errorCode) സ്ഥിരം override {
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ UNORM_MAYBE;
        }
        സ്ഥിരം char16_t *sArray=s.getBuffer();
        എങ്കിൽ(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ UNORM_MAYBE;
        }
        UNormalizationCheckResult qcResult=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.length(), onlyContiguous, &qcResult);
        തിരികെ qcResult;
    }
    മായാ സ്ഥിരം char16_t*
    spanQuickCheckYes(സ്ഥിരം char16_t* src U_LIFETIME_BOUND,
                      സ്ഥിരം char16_t* limit,
                      UErrorCode&) സ്ഥിരം override {
        തിരികെ impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    മായാ UNormalizationCheckResult getQuickCheck(UChar32 c) സ്ഥിരം override {
        തിരികെ impl.getCompQuickCheck(impl.getNorm16(c));
    }
    മായാ UBool hasBoundaryBefore(UChar32 c) സ്ഥിരം override {
        തിരികെ impl.hasCompBoundaryBefore(c);
    }
    മായാ UBool hasBoundaryAfter(UChar32 c) സ്ഥിരം override {
        തിരികെ impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    മായാ UBool isInert(UChar32 c) സ്ഥിരം override {
        തിരികെ impl.isCompInert(c, onlyContiguous);
    }

    സ്ഥിരം UBool onlyContiguous;
};

class FCDNormalizer2 : പൊതു Normalizer2WithImpl {
പൊതു:
    FCDNormalizer2(സ്ഥിരം Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    മായാ ~FCDNormalizer2();

സ്വകാര്യ:
    മായാ ശൂന്യം
    normalize(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) സ്ഥിരം override {
        impl.makeFCD(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    മായാ ശൂന്യം
    normalizeAndAppend(സ്ഥിരം char16_t *src, സ്ഥിരം char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_നടുവ്,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) സ്ഥിരം override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safe_നടുവ്, buffer, errorCode);
    }
    മായാ സ്ഥിരം char16_t*
    spanQuickCheckYes(സ്ഥിരം char16_t* src U_LIFETIME_BOUND,
                      സ്ഥിരം char16_t* limit,
                      UErrorCode& errorCode) സ്ഥിരം override {
        തിരികെ impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    മായാ UBool hasBoundaryBefore(UChar32 c) സ്ഥിരം override {
        തിരികെ impl.hasFCDBoundaryBefore(c);
    }
    മായാ UBool hasBoundaryAfter(UChar32 c) സ്ഥിരം override {
        തിരികെ impl.hasFCDBoundaryAfter(c);
    }
    മായാ UBool isInert(UChar32 c) സ്ഥിരം override {
        തിരികെ impl.isFCDInert(c);
    }
};

ഘടന Norm2AllModes : പൊതു UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    സ്ഥിതം Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    സ്ഥിതം Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    സ്ഥിതം Norm2AllModes *createInstance(സ്ഥിരം അക്ഷരം *packageName,
                                         സ്ഥിരം അക്ഷരം *name,
                                         UErrorCode &errorCode);

    സ്ഥിതം സ്ഥിരം Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    സ്ഥിതം സ്ഥിരം Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    സ്ഥിതം സ്ഥിരം Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    സ്ഥിതം സ്ഥിരം Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
