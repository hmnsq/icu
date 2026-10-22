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

#ضمن "unicode/utypes.h"

#𐡄𐡍 !UCONFIG_NO_NORMALIZATION

#ضمن "unicode/edits.h"
#ضمن "unicode/normalizer2.h"
#ضمن "unicode/stringoptions.h"
#ضمن "unicode/unistr.h"
#ضمن "cpputils.h"
#ضمن "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : عام Normalizer2 {
عام:
    Normalizer2WithImpl(ثابت Normalizer2Impl &ni) : impl(ni) {}
    virtual ~Normalizer2WithImpl();

    // normalize
    virtual UnicodeString&
    normalize(ثابت UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) ثابت override {
        𐡄𐡍(U_FAILURE(errorCode)) {
            dest.setToBogus();
            أرجع dest;
        }
        ثابت char16_t *sArray=src.getBuffer();
        𐡄𐡍(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            أرجع dest;
        }
        dest.remove();
        ReorderingBuffer buffer(impl, dest);
        𐡄𐡍(buffer.init(src.length(), errorCode)) {
            normalize(sArray, sArray+src.length(), buffer, errorCode);
        }
        أرجع dest;
    }
    virtual فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت = 0;

    // normalize and append
    virtual UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ثابت UnicodeString& second,
                             UErrorCode& errorCode) ثابت override {
        أرجع normalizeSecondAndAppend(first, second, true, errorCode);
    }
    virtual UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           ثابت UnicodeString& second,
           UErrorCode& errorCode) ثابت override {
        أرجع normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ثابت UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) ثابت {
        uprv_checkCanGetBuffer(first, errorCode);
        𐡄𐡍(U_FAILURE(errorCode)) {
            أرجع first;
        }
        ثابت char16_t *secondArray=second.getBuffer();
        𐡄𐡍(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            أرجع first;
        }
        int32_t firstLength=first.length();
        UnicodeString safeMiddle;
        {
            ReorderingBuffer buffer(impl, first);
            𐡄𐡍(buffer.init(firstLength+second.length(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.length(), doNormalize,
                                   safeMiddle, buffer, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        𐡄𐡍(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(firstLength-safeMiddle.length(), 0x7fffffff, safeMiddle);
        }
        أرجع first;
    }
    virtual فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *limit, UBool doNormalize,
                       UnicodeString &safeMiddle,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت = 0;
    virtual UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t buffer[4];
        int32_t length;
        ثابت char16_t *d=impl.getDecomposition(c, buffer, length);
        𐡄𐡍(d==nullptr) {
            أرجع false;
        }
        𐡄𐡍(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (Jamos from Hangul syllable c)
        } وإلا {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        أرجع true;
    }
    virtual UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ثابت override {
        char16_t buffer[30];
        int32_t length;
        ثابت char16_t *d=impl.getRawDecomposition(c, buffer, length);
        𐡄𐡍(d==nullptr) {
            أرجع false;
        }
        𐡄𐡍(d==buffer) {
            decomposition.setTo(buffer, length);  // copy the string (algorithmic decomposition)
        } وإلا {
            decomposition.setTo(false, d, length);  // read-only alias
        }
        أرجع true;
    }
    virtual UChar32
    composePair(UChar32 a, UChar32 b) ثابت override {
        أرجع impl.composePair(a, b);
    }

    virtual uint8_t
    getCombiningClass(UChar32 c) ثابت override {
        أرجع impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    virtual UBool
    isNormalized(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت override {
        𐡄𐡍(U_FAILURE(errorCode)) {
            أرجع false;
        }
        ثابت char16_t *sArray=s.getBuffer();
        𐡄𐡍(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            أرجع false;
        }
        ثابت char16_t *sLimit=sArray+s.length();
        أرجع sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    virtual UNormalizationCheckResult
    quickCheck(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت override {
        أرجع Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    virtual int32_t
    spanQuickCheckYes(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت override {
        𐡄𐡍(U_FAILURE(errorCode)) {
            أرجع 0;
        }
        ثابت char16_t *sArray=s.getBuffer();
        𐡄𐡍(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            أرجع 0;
        }
        أرجع static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.length(), errorCode) - sArray);
    }
    virtual ثابت char16_t*
    spanQuickCheckYes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* limit,
                      UErrorCode& errorCode) ثابت = 0;

    virtual UNormalizationCheckResult getQuickCheck(UChar32) ثابت {
        أرجع UNORM_YES;
    }

    ثابت Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : عام Normalizer2WithImpl {
عام:
    DecomposeNormalizer2(ثابت Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    virtual ~DecomposeNormalizer2();

خاص:
    virtual فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت override {
        impl.decompose(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    virtual فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *limit, UBool doNormalize,
                       UnicodeString &safeMiddle,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت override {
        impl.decomposeAndAppend(src, limit, doNormalize, safeMiddle, buffer, errorCode);
    }

    فراغ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ثابت override {
        𐡄𐡍 (U_FAILURE(errorCode)) {
            أرجع;
        }
        𐡄𐡍 (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.data());
        impl.decomposeUTF8(options, s, s + src.length(), &sink, edits, errorCode);
        sink.Flush();
    }
    virtual UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ثابت override {
        𐡄𐡍(U_FAILURE(errorCode)) {
            أرجع false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.data());
        ثابت uint8_t *sLimit = s + sp.length();
        أرجع sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    virtual ثابت char16_t*
    spanQuickCheckYes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* limit,
                      UErrorCode& errorCode) ثابت override {
        أرجع impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    virtual UNormalizationCheckResult getQuickCheck(UChar32 c) ثابت override {
        أرجع impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    virtual UBool hasBoundaryBefore(UChar32 c) ثابت override {
        أرجع impl.hasDecompBoundaryBefore(c);
    }
    virtual UBool hasBoundaryAfter(UChar32 c) ثابت override {
        أرجع impl.hasDecompBoundaryAfter(c);
    }
    virtual UBool isInert(UChar32 c) ثابت override {
        أرجع impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : عام Normalizer2WithImpl {
عام:
    ComposeNormalizer2(ثابت Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    virtual ~ComposeNormalizer2();

خاص:
    virtual فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت override {
        impl.compose(src, limit, onlyContiguous, true, buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    فراغ
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ثابت override {
        𐡄𐡍 (U_FAILURE(errorCode)) {
            أرجع;
        }
        𐡄𐡍 (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(src.data());
        impl.composeUTF8(options, onlyContiguous, s, s + src.length(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    virtual فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *limit, UBool doNormalize,
                       UnicodeString &safeMiddle,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safeMiddle, buffer, errorCode);
    }

    virtual UBool
    isNormalized(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت override {
        𐡄𐡍(U_FAILURE(errorCode)) {
            أرجع false;
        }
        ثابت char16_t *sArray=s.getBuffer();
        𐡄𐡍(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            أرجع false;
        }
        UnicodeString temp;
        ReorderingBuffer buffer(impl, temp);
        𐡄𐡍(!buffer.init(5, errorCode)) {  // small destCapacity for substring normalization
            أرجع false;
        }
        أرجع impl.compose(sArray, sArray+s.length(), onlyContiguous, false, buffer, errorCode);
    }
    virtual UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ثابت override {
        𐡄𐡍(U_FAILURE(errorCode)) {
            أرجع false;
        }
        ثابت uint8_t *s = reinterpret_cast<ثابت uint8_t *>(sp.data());
        أرجع impl.composeUTF8(0, onlyContiguous, s, s + sp.length(), nullptr, nullptr, errorCode);
    }
    virtual UNormalizationCheckResult
    quickCheck(ثابت UnicodeString &s, UErrorCode &errorCode) ثابت override {
        𐡄𐡍(U_FAILURE(errorCode)) {
            أرجع UNORM_MAYBE;
        }
        ثابت char16_t *sArray=s.getBuffer();
        𐡄𐡍(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            أرجع UNORM_MAYBE;
        }
        UNormalizationCheckResult qcResult=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.length(), onlyContiguous, &qcResult);
        أرجع qcResult;
    }
    virtual ثابت char16_t*
    spanQuickCheckYes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* limit,
                      UErrorCode&) ثابت override {
        أرجع impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    virtual UNormalizationCheckResult getQuickCheck(UChar32 c) ثابت override {
        أرجع impl.getCompQuickCheck(impl.getNorm16(c));
    }
    virtual UBool hasBoundaryBefore(UChar32 c) ثابت override {
        أرجع impl.hasCompBoundaryBefore(c);
    }
    virtual UBool hasBoundaryAfter(UChar32 c) ثابت override {
        أرجع impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    virtual UBool isInert(UChar32 c) ثابت override {
        أرجع impl.isCompInert(c, onlyContiguous);
    }

    ثابت UBool onlyContiguous;
};

class FCDNormalizer2 : عام Normalizer2WithImpl {
عام:
    FCDNormalizer2(ثابت Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    virtual ~FCDNormalizer2();

خاص:
    virtual فراغ
    normalize(ثابت char16_t *src, ثابت char16_t *limit,
              ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت override {
        impl.makeFCD(src, limit, &buffer, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    virtual فراغ
    normalizeAndAppend(ثابت char16_t *src, ثابت char16_t *limit, UBool doNormalize,
                       UnicodeString &safeMiddle,
                       ReorderingBuffer &buffer, UErrorCode &errorCode) ثابت override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safeMiddle, buffer, errorCode);
    }
    virtual ثابت char16_t*
    spanQuickCheckYes(ثابت char16_t* src U_LIFETIME_BOUND,
                      ثابت char16_t* limit,
                      UErrorCode& errorCode) ثابت override {
        أرجع impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    virtual UBool hasBoundaryBefore(UChar32 c) ثابت override {
        أرجع impl.hasFCDBoundaryBefore(c);
    }
    virtual UBool hasBoundaryAfter(UChar32 c) ثابت override {
        أرجع impl.hasFCDBoundaryAfter(c);
    }
    virtual UBool isInert(UChar32 c) ثابت override {
        أرجع impl.isFCDInert(c);
    }
};

بنية Norm2AllModes : عام UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    ساكن Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    ساكن Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    ساكن Norm2AllModes *createInstance(ثابت حرف *packageName,
                                         ثابت حرف *name,
                                         UErrorCode &errorCode);

    ساكن ثابت Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    ساكن ثابت Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    ساكن ثابت Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    ساكن ثابت Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
