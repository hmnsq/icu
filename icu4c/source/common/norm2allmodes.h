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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_NORMALIZATION

#અન્તર્ભાવ "unicode/edits.h"
#અન્તર્ભાવ "unicode/normalizer2.h"
#અન્તર્ભાવ "unicode/stringoptions.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "cpputils.h"
#અન્તર્ભાવ "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : જાહેર Normalizer2 {
જાહેર:
    Normalizer2WithImpl(અચલ Normalizer2Impl &ni) : impl(ni) {}
    આભાસી ~Normalizer2WithImpl();

    // normalize
    આભાસી UnicodeString&
    normalize(અચલ UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) અચલ override {
        જો(U_FAILURE(errorCode)) {
            dest.setToBogus();
            પાછા dest;
        }
        અચલ char16_t *sArray=src.getBuffer();
        જો(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            પાછા dest;
        }
        dest.remove();
        ReorderingBuffer buffer(impl, dest);
        જો(buffer.init(src.length(), errorCode)) {
            normalize(sArray, sArray+src.length(), buffer, errorCode);
        }
        પાછા dest;
    }
    આભાસી શૂન્ય
    normalize(અચલ char16_t *src, અચલ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) અચલ = 0;

    // normalize and append
    આભાસી UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             અચલ UnicodeString& second,
                             UErrorCode& errorCode) અચલ override {
        પાછા normalizeSecondAndAppend(first, second, true, errorCode);
    }
    આભાસી UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           અચલ UnicodeString& second,
           UErrorCode& errorCode) અચલ override {
        પાછા normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             અચલ UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) અચલ {
        uprv_checkCanGetBuffer(first, errorCode);
        જો(U_FAILURE(errorCode)) {
            પાછા first;
        }
        અચલ char16_t *secondArray=second.getBuffer();
        જો(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            પાછા first;
        }
        int32_t firstLength=first.length();
        UnicodeString safe_મધ્ય;
        {
            ReorderingBuffer buffer(impl, first);
            જો(buffer.init(firstLength+second.length(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.length(), doNormalize,
                                   safe_મધ્ય, buffer, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        જો(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(firstLength-safe_મધ્ય.length(), 0x7fffffff, safe_મધ્ય);
        }
        પાછા first;
    }
    આભાસી શૂન્ય
    normalizeAndAppend(અચલ char16_t *src, અચલ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_મધ્ય,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) અચલ = 0;
    આભાસી UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) અચલ override {
        char16_t buffer[4];
        int32_t length;
        અચલ char16_t *d=impl.getDecomposition(c, buffer, length);
        જો(d==nullptr) {
            પાછા false;
        }
        જો(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (Jamos from Hangul syllable c)
        } નહીંતર {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        પાછા true;
    }
    આભાસી UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) અચલ override {
        char16_t buffer[30];
        int32_t length;
        અચલ char16_t *d=impl.getRawDecomposition(c, buffer, length);
        જો(d==nullptr) {
            પાછા false;
        }
        જો(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (algorithmic decomposition)
        } નહીંતર {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        પાછા true;
    }
    આભાસી UChar32
    composePair(UChar32 a, UChar32 b) અચલ override {
        પાછા impl.composePair(a, b);
    }

    આભાસી uint8_t
    getCombiningClass(UChar32 c) અચલ override {
        પાછા impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    આભાસી UBool
    isNormalized(અચલ UnicodeString &s, UErrorCode &errorCode) અચલ override {
        જો(U_FAILURE(errorCode)) {
            પાછા false;
        }
        અચલ char16_t *sArray=s.getBuffer();
        જો(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            પાછા false;
        }
        અચલ char16_t *sLimit=sArray+s.length();
        પાછા sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    આભાસી UNormalizationCheckResult
    quickCheck(અચલ UnicodeString &s, UErrorCode &errorCode) અચલ override {
        પાછા Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    આભાસી int32_t
    spanQuickCheckYes(અચલ UnicodeString &s, UErrorCode &errorCode) અચલ override {
        જો(U_FAILURE(errorCode)) {
            પાછા 0;
        }
        અચલ char16_t *sArray=s.getBuffer();
        જો(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            પાછા 0;
        }
        પાછા static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.length(), errorCode) - sArray);
    }
    આભાસી અચલ char16_t*
    spanQuickCheckYes(અચલ char16_t* src U_LIFETIME_BOUND,
                      અચલ char16_t* limit,
                      UErrorCode& errorCode) અચલ = 0;

    આભાસી UNormalizationCheckResult getQuickCheck(UChar32) અચલ {
        પાછા UNORM_YES;
    }

    અચલ Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : જાહેર Normalizer2WithImpl {
જાહેર:
    DecomposeNormalizer2(અચલ Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    આભાસી ~DecomposeNormalizer2();

ખાનગી:
    આભાસી શૂન્ય
    normalize(અચલ char16_t *src, અચલ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) અચલ override {
        impl.decompose(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    આભાસી શૂન્ય
    normalizeAndAppend(અચલ char16_t *src, અચલ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_મધ્ય,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) અચલ override {
        impl.decomposeAndAppend(src, limit, doNormalize, safe_મધ્ય, buffer, errorCode);
    }

    શૂન્ય
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) અચલ override {
        જો (U_FAILURE(errorCode)) {
            પાછા;
        }
        જો (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        અચલ uint8_t *s = reinterpret_cast<અચલ uint8_t *>(src.data());
        impl.decomposeUTF8(options, s, s + src.length(), &sink, edits, errorCode);
        sink.Flush();
    }
    આભાસી UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) અચલ override {
        જો(U_FAILURE(errorCode)) {
            પાછા false;
        }
        અચલ uint8_t *s = reinterpret_cast<અચલ uint8_t *>(sp.data());
        અચલ uint8_t *sLimit = s + sp.length();
        પાછા sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    આભાસી અચલ char16_t*
    spanQuickCheckYes(અચલ char16_t* src U_LIFETIME_BOUND,
                      અચલ char16_t* limit,
                      UErrorCode& errorCode) અચલ override {
        પાછા impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    આભાસી UNormalizationCheckResult getQuickCheck(UChar32 c) અચલ override {
        પાછા impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    આભાસી UBool hasBoundaryBefore(UChar32 c) અચલ override {
        પાછા impl.hasDecompBoundaryBefore(c);
    }
    આભાસી UBool hasBoundaryAfter(UChar32 c) અચલ override {
        પાછા impl.hasDecompBoundaryAfter(c);
    }
    આભાસી UBool isInert(UChar32 c) અચલ override {
        પાછા impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : જાહેર Normalizer2WithImpl {
જાહેર:
    ComposeNormalizer2(અચલ Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    આભાસી ~ComposeNormalizer2();

ખાનગી:
    આભાસી શૂન્ય
    normalize(અચલ char16_t *src, અચલ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) અચલ override {
        impl.compose(src, limit, onlyContiguous, true, buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    શૂન્ય
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) અચલ override {
        જો (U_FAILURE(errorCode)) {
            પાછા;
        }
        જો (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        અચલ uint8_t *s = reinterpret_cast<અચલ uint8_t *>(src.data());
        impl.composeUTF8(options, onlyContiguous, s, s + src.length(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    આભાસી શૂન્ય
    normalizeAndAppend(અચલ char16_t *src, અચલ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_મધ્ય,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) અચલ override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safe_મધ્ય, buffer, errorCode);
    }

    આભાસી UBool
    isNormalized(અચલ UnicodeString &s, UErrorCode &errorCode) અચલ override {
        જો(U_FAILURE(errorCode)) {
            પાછા false;
        }
        અચલ char16_t *sArray=s.getBuffer();
        જો(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            પાછા false;
        }
        UnicodeString temp;
        ReorderingBuffer buffer(impl, temp);
        જો(!buffer.init(5, errorCode)) {  // small destCapacity for substring normalization
            પાછા false;
        }
        પાછા impl.compose(sArray, sArray+s.length(), onlyContiguous, false, buffer, errorCode);
    }
    આભાસી UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) અચલ override {
        જો(U_FAILURE(errorCode)) {
            પાછા false;
        }
        અચલ uint8_t *s = reinterpret_cast<અચલ uint8_t *>(sp.data());
        પાછા impl.composeUTF8(0, onlyContiguous, s, s + sp.length(), nullptr, nullptr, errorCode);
    }
    આભાસી UNormalizationCheckResult
    quickCheck(અચલ UnicodeString &s, UErrorCode &errorCode) અચલ override {
        જો(U_FAILURE(errorCode)) {
            પાછા UNORM_MAYBE;
        }
        અચલ char16_t *sArray=s.getBuffer();
        જો(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            પાછા UNORM_MAYBE;
        }
        UNormalizationCheckResult qcResult=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.length(), onlyContiguous, &qcResult);
        પાછા qcResult;
    }
    આભાસી અચલ char16_t*
    spanQuickCheckYes(અચલ char16_t* src U_LIFETIME_BOUND,
                      અચલ char16_t* limit,
                      UErrorCode&) અચલ override {
        પાછા impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    આભાસી UNormalizationCheckResult getQuickCheck(UChar32 c) અચલ override {
        પાછા impl.getCompQuickCheck(impl.getNorm16(c));
    }
    આભાસી UBool hasBoundaryBefore(UChar32 c) અચલ override {
        પાછા impl.hasCompBoundaryBefore(c);
    }
    આભાસી UBool hasBoundaryAfter(UChar32 c) અચલ override {
        પાછા impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    આભાસી UBool isInert(UChar32 c) અચલ override {
        પાછા impl.isCompInert(c, onlyContiguous);
    }

    અચલ UBool onlyContiguous;
};

class FCDNormalizer2 : જાહેર Normalizer2WithImpl {
જાહેર:
    FCDNormalizer2(અચલ Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    આભાસી ~FCDNormalizer2();

ખાનગી:
    આભાસી શૂન્ય
    normalize(અચલ char16_t *src, અચલ char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) અચલ override {
        impl.makeFCD(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    આભાસી શૂન્ય
    normalizeAndAppend(અચલ char16_t *src, અચલ char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_મધ્ય,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) અચલ override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safe_મધ્ય, buffer, errorCode);
    }
    આભાસી અચલ char16_t*
    spanQuickCheckYes(અચલ char16_t* src U_LIFETIME_BOUND,
                      અચલ char16_t* limit,
                      UErrorCode& errorCode) અચલ override {
        પાછા impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    આભાસી UBool hasBoundaryBefore(UChar32 c) અચલ override {
        પાછા impl.hasFCDBoundaryBefore(c);
    }
    આભાસી UBool hasBoundaryAfter(UChar32 c) અચલ override {
        પાછા impl.hasFCDBoundaryAfter(c);
    }
    આભાસી UBool isInert(UChar32 c) અચલ override {
        પાછા impl.isFCDInert(c);
    }
};

સંરચના Norm2AllModes : જાહેર UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    સ્થિર Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    સ્થિર Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    સ્થિર Norm2AllModes *createInstance(અચલ અક્ષર *packageName,
                                         અચલ અક્ષર *name,
                                         UErrorCode &errorCode);

    સ્થિર અચલ Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    સ્થિર અચલ Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    સ્થિર અચલ Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    સ્થિર અચલ Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
