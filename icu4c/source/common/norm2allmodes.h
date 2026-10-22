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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_NORMALIZATION

#অন্তর্ভাব "unicode/edits.h"
#অন্তর্ভাব "unicode/normalizer2.h"
#অন্তর্ভাব "unicode/stringoptions.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "cpputils.h"
#অন্তর্ভাব "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : সর্বজনীন Normalizer2 {
সর্বজনীন:
    Normalizer2WithImpl(ধ্রুবক Normalizer2Impl &ni) : impl(ni) {}
    আভাসী ~Normalizer2WithImpl();

    // normalize
    আভাসী UnicodeString&
    normalize(ধ্রুবক UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) ধ্রুবক override {
        যদি(U_FAILURE(errorCode)) {
            dest.setToBogus();
            ফেরত dest;
        }
        ধ্রুবক char16_t *sArray=src.get_বাফার();
        যদি(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            ফেরত dest;
        }
        dest.remove();
        Reordering_বাফার বাফার(impl, dest);
        যদি(বাফার.init(src.দৈর্ঘ্য(), errorCode)) {
            normalize(sArray, sArray+src.দৈর্ঘ্য(), বাফার, errorCode);
        }
        ফেরত dest;
    }
    আভাসী শূন্য
    normalize(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit,
              Reordering_বাফার &বাফার, UErrorCode &errorCode) ধ্রুবক = 0;

    // normalize and append
    আভাসী UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ধ্রুবক UnicodeString& second,
                             UErrorCode& errorCode) ধ্রুবক override {
        ফেরত normalizeSecondAndAppend(first, second, true, errorCode);
    }
    আভাসী UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           ধ্রুবক UnicodeString& second,
           UErrorCode& errorCode) ধ্রুবক override {
        ফেরত normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ধ্রুবক UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) ধ্রুবক {
        uprv_check_Can_Get_বাফার(first, errorCode);
        যদি(U_FAILURE(errorCode)) {
            ফেরত first;
        }
        ধ্রুবক char16_t *secondArray=second.get_বাফার();
        যদি(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত first;
        }
        int32_t first_দৈর্ঘ্য=first.দৈর্ঘ্য();
        UnicodeString safe_মধ্য;
        {
            Reordering_বাফার বাফার(impl, first);
            যদি(বাফার.init(first_দৈর্ঘ্য+second.দৈর্ঘ্য(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.দৈর্ঘ্য(), doNormalize,
                                   safe_মধ্য, বাফার, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        যদি(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(first_দৈর্ঘ্য-safe_মধ্য.দৈর্ঘ্য(), 0x7fffffff, safe_মধ্য);
        }
        ফেরত first;
    }
    আভাসী শূন্য
    normalizeAndAppend(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_মধ্য,
                       Reordering_বাফার &বাফার, UErrorCode &errorCode) ধ্রুবক = 0;
    আভাসী UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ধ্রুবক override {
        char16_t বাফার[4];
        int32_t দৈর্ঘ্য;
        ধ্রুবক char16_t *d=impl.getDecomposition(c, বাফার, দৈর্ঘ্য);
        যদি(d==nullptr) {
            ফেরত false;
        }
        যদি(d==বাফার) {
            decomposition.setTo(বাফার, দৈর্ঘ্য);  // copy the string (Jamos from Hangul syllable c)
        } অন্যথা {
            decomposition.setTo(false, d, দৈর্ঘ্য);  // read-only alias
        }
        ফেরত true;
    }
    আভাসী UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ধ্রুবক override {
        char16_t বাফার[30];
        int32_t দৈর্ঘ্য;
        ধ্রুবক char16_t *d=impl.getRawDecomposition(c, বাফার, দৈর্ঘ্য);
        যদি(d==nullptr) {
            ফেরত false;
        }
        যদি(d==বাফার) {
            decomposition.setTo(বাফার, দৈর্ঘ্য);  // copy the string (algorithmic decomposition)
        } অন্যথা {
            decomposition.setTo(false, d, দৈর্ঘ্য);  // read-only alias
        }
        ফেরত true;
    }
    আভাসী UChar32
    composePair(UChar32 a, UChar32 b) ধ্রুবক override {
        ফেরত impl.composePair(a, b);
    }

    আভাসী uint8_t
    getCombiningClass(UChar32 c) ধ্রুবক override {
        ফেরত impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    আভাসী UBool
    isNormalized(ধ্রুবক UnicodeString &s, UErrorCode &errorCode) ধ্রুবক override {
        যদি(U_FAILURE(errorCode)) {
            ফেরত false;
        }
        ধ্রুবক char16_t *sArray=s.get_বাফার();
        যদি(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত false;
        }
        ধ্রুবক char16_t *sLimit=sArray+s.দৈর্ঘ্য();
        ফেরত sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    আভাসী UNormalization_Check_ফলাফল
    quickCheck(ধ্রুবক UnicodeString &s, UErrorCode &errorCode) ধ্রুবক override {
        ফেরত Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    আভাসী int32_t
    spanQuickCheckYes(ধ্রুবক UnicodeString &s, UErrorCode &errorCode) ধ্রুবক override {
        যদি(U_FAILURE(errorCode)) {
            ফেরত 0;
        }
        ধ্রুবক char16_t *sArray=s.get_বাফার();
        যদি(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত 0;
        }
        ফেরত static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.দৈর্ঘ্য(), errorCode) - sArray);
    }
    আভাসী ধ্রুবক char16_t*
    spanQuickCheckYes(ধ্রুবক char16_t* src U_LIFETIME_BOUND,
                      ধ্রুবক char16_t* limit,
                      UErrorCode& errorCode) ধ্রুবক = 0;

    আভাসী UNormalization_Check_ফলাফল getQuickCheck(UChar32) ধ্রুবক {
        ফেরত UNORM_YES;
    }

    ধ্রুবক Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : সর্বজনীন Normalizer2WithImpl {
সর্বজনীন:
    DecomposeNormalizer2(ধ্রুবক Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    আভাসী ~DecomposeNormalizer2();

ব্যক্তিগত:
    আভাসী শূন্য
    normalize(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit,
              Reordering_বাফার &বাফার, UErrorCode &errorCode) ধ্রুবক override {
        impl.decompose(src, limit, &বাফার, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    আভাসী শূন্য
    normalizeAndAppend(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_মধ্য,
                       Reordering_বাফার &বাফার, UErrorCode &errorCode) ধ্রুবক override {
        impl.decomposeAndAppend(src, limit, doNormalize, safe_মধ্য, বাফার, errorCode);
    }

    শূন্য
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ধ্রুবক override {
        যদি (U_FAILURE(errorCode)) {
            ফেরত;
        }
        যদি (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ধ্রুবক uint8_t *s = reinterpret_cast<ধ্রুবক uint8_t *>(src.তথ্য());
        impl.decomposeUTF8(options, s, s + src.দৈর্ঘ্য(), &sink, edits, errorCode);
        sink.Flush();
    }
    আভাসী UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ধ্রুবক override {
        যদি(U_FAILURE(errorCode)) {
            ফেরত false;
        }
        ধ্রুবক uint8_t *s = reinterpret_cast<ধ্রুবক uint8_t *>(sp.তথ্য());
        ধ্রুবক uint8_t *sLimit = s + sp.দৈর্ঘ্য();
        ফেরত sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    আভাসী ধ্রুবক char16_t*
    spanQuickCheckYes(ধ্রুবক char16_t* src U_LIFETIME_BOUND,
                      ধ্রুবক char16_t* limit,
                      UErrorCode& errorCode) ধ্রুবক override {
        ফেরত impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    আভাসী UNormalization_Check_ফলাফল getQuickCheck(UChar32 c) ধ্রুবক override {
        ফেরত impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    আভাসী UBool hasBoundaryBefore(UChar32 c) ধ্রুবক override {
        ফেরত impl.hasDecompBoundaryBefore(c);
    }
    আভাসী UBool hasBoundaryAfter(UChar32 c) ধ্রুবক override {
        ফেরত impl.hasDecompBoundaryAfter(c);
    }
    আভাসী UBool isInert(UChar32 c) ধ্রুবক override {
        ফেরত impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : সর্বজনীন Normalizer2WithImpl {
সর্বজনীন:
    ComposeNormalizer2(ধ্রুবক Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    আভাসী ~ComposeNormalizer2();

ব্যক্তিগত:
    আভাসী শূন্য
    normalize(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit,
              Reordering_বাফার &বাফার, UErrorCode &errorCode) ধ্রুবক override {
        impl.compose(src, limit, onlyContiguous, true, বাফার, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    শূন্য
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) ধ্রুবক override {
        যদি (U_FAILURE(errorCode)) {
            ফেরত;
        }
        যদি (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ধ্রুবক uint8_t *s = reinterpret_cast<ধ্রুবক uint8_t *>(src.তথ্য());
        impl.composeUTF8(options, onlyContiguous, s, s + src.দৈর্ঘ্য(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    আভাসী শূন্য
    normalizeAndAppend(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_মধ্য,
                       Reordering_বাফার &বাফার, UErrorCode &errorCode) ধ্রুবক override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safe_মধ্য, বাফার, errorCode);
    }

    আভাসী UBool
    isNormalized(ধ্রুবক UnicodeString &s, UErrorCode &errorCode) ধ্রুবক override {
        যদি(U_FAILURE(errorCode)) {
            ফেরত false;
        }
        ধ্রুবক char16_t *sArray=s.get_বাফার();
        যদি(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত false;
        }
        UnicodeString অস্থায়ী;
        Reordering_বাফার বাফার(impl, অস্থায়ী);
        যদি(!বাফার.init(5, errorCode)) {  // small destCapacity for substring normalization
            ফেরত false;
        }
        ফেরত impl.compose(sArray, sArray+s.দৈর্ঘ্য(), onlyContiguous, false, বাফার, errorCode);
    }
    আভাসী UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) ধ্রুবক override {
        যদি(U_FAILURE(errorCode)) {
            ফেরত false;
        }
        ধ্রুবক uint8_t *s = reinterpret_cast<ধ্রুবক uint8_t *>(sp.তথ্য());
        ফেরত impl.composeUTF8(0, onlyContiguous, s, s + sp.দৈর্ঘ্য(), nullptr, nullptr, errorCode);
    }
    আভাসী UNormalization_Check_ফলাফল
    quickCheck(ধ্রুবক UnicodeString &s, UErrorCode &errorCode) ধ্রুবক override {
        যদি(U_FAILURE(errorCode)) {
            ফেরত UNORM_MAYBE;
        }
        ধ্রুবক char16_t *sArray=s.get_বাফার();
        যদি(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত UNORM_MAYBE;
        }
        UNormalization_Check_ফলাফল qc_ফলাফল=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.দৈর্ঘ্য(), onlyContiguous, &qc_ফলাফল);
        ফেরত qc_ফলাফল;
    }
    আভাসী ধ্রুবক char16_t*
    spanQuickCheckYes(ধ্রুবক char16_t* src U_LIFETIME_BOUND,
                      ধ্রুবক char16_t* limit,
                      UErrorCode&) ধ্রুবক override {
        ফেরত impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    আভাসী UNormalization_Check_ফলাফল getQuickCheck(UChar32 c) ধ্রুবক override {
        ফেরত impl.getCompQuickCheck(impl.getNorm16(c));
    }
    আভাসী UBool hasBoundaryBefore(UChar32 c) ধ্রুবক override {
        ফেরত impl.hasCompBoundaryBefore(c);
    }
    আভাসী UBool hasBoundaryAfter(UChar32 c) ধ্রুবক override {
        ফেরত impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    আভাসী UBool isInert(UChar32 c) ধ্রুবক override {
        ফেরত impl.isCompInert(c, onlyContiguous);
    }

    ধ্রুবক UBool onlyContiguous;
};

class FCDNormalizer2 : সর্বজনীন Normalizer2WithImpl {
সর্বজনীন:
    FCDNormalizer2(ধ্রুবক Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    আভাসী ~FCDNormalizer2();

ব্যক্তিগত:
    আভাসী শূন্য
    normalize(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit,
              Reordering_বাফার &বাফার, UErrorCode &errorCode) ধ্রুবক override {
        impl.makeFCD(src, limit, &বাফার, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    আভাসী শূন্য
    normalizeAndAppend(ধ্রুবক char16_t *src, ধ্রুবক char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_মধ্য,
                       Reordering_বাফার &বাফার, UErrorCode &errorCode) ধ্রুবক override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safe_মধ্য, বাফার, errorCode);
    }
    আভাসী ধ্রুবক char16_t*
    spanQuickCheckYes(ধ্রুবক char16_t* src U_LIFETIME_BOUND,
                      ধ্রুবক char16_t* limit,
                      UErrorCode& errorCode) ধ্রুবক override {
        ফেরত impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    আভাসী UBool hasBoundaryBefore(UChar32 c) ধ্রুবক override {
        ফেরত impl.hasFCDBoundaryBefore(c);
    }
    আভাসী UBool hasBoundaryAfter(UChar32 c) ধ্রুবক override {
        ফেরত impl.hasFCDBoundaryAfter(c);
    }
    আভাসী UBool isInert(UChar32 c) ধ্রুবক override {
        ফেরত impl.isFCDInert(c);
    }
};

কাঠামো Norm2AllModes : সর্বজনীন UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    স্থির Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    স্থির Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    স্থির Norm2AllModes *createInstance(ধ্রুবক অক্ষর *packageName,
                                         ধ্রুবক অক্ষর *name,
                                         UErrorCode &errorCode);

    স্থির ধ্রুবক Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    স্থির ধ্রুবক Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    স্থির ধ্রুবক Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    স্থির ধ্রুবক Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#endif  // !UCONFIG_NO_NORMALIZATION
#endif  // __NORM2ALLMODES_H__
