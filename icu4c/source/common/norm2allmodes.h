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

#अगर_अघोषित __NORM2ALLMODES_H__
#घोषणा __NORM2ALLMODES_H__

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_NORMALIZATION

#समावेश "unicode/edits.h"
#समावेश "unicode/normalizer2.h"
#समावेश "unicode/stringoptions.h"
#समावेश "unicode/unistr.h"
#समावेश "cpputils.h"
#समावेश "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : खुला Normalizer2 {
खुला:
    Normalizer2WithImpl(स्थिर Normalizer2Impl &ni) : impl(ni) {}
    भव ~Normalizer2WithImpl();

    // normalize
    भव UnicodeString&
    normalize(स्थिर UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) स्थिर override {
        अगर(U_FAILURE(errorCode)) {
            dest.setToBogus();
            वापस dest;
        }
        स्थिर char16_t *sArray=src.get_बफर();
        अगर(&dest==&src || sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            dest.setToBogus();
            वापस dest;
        }
        dest.हटाओ();
        Reordering_बफर बफर(impl, dest);
        अगर(बफर.init(src.लंबाई(), errorCode)) {
            normalize(sArray, sArray+src.लंबाई(), बफर, errorCode);
        }
        वापस dest;
    }
    भव व्योम
    normalize(स्थिर char16_t *src, स्थिर char16_t *limit,
              Reordering_बफर &बफर, UErrorCode &errorCode) स्थिर = 0;

    // normalize and append
    भव UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             स्थिर UnicodeString& second,
                             UErrorCode& errorCode) स्थिर override {
        वापस normalizeSecondAndAppend(first, second, true, errorCode);
    }
    भव UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           स्थिर UnicodeString& second,
           UErrorCode& errorCode) स्थिर override {
        वापस normalizeSecondAndAppend(first, second, false, errorCode);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             स्थिर UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) स्थिर {
        uprv_check_Can_Get_बफर(first, errorCode);
        अगर(U_FAILURE(errorCode)) {
            वापस first;
        }
        स्थिर char16_t *secondArray=second.get_बफर();
        अगर(&first==&second || secondArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            वापस first;
        }
        int32_t first_लंबाई=first.लंबाई();
        UnicodeString safe_मध्य;
        {
            Reordering_बफर बफर(impl, first);
            अगर(बफर.init(first_लंबाई+second.लंबाई(), errorCode)) {
                normalizeAndAppend(secondArray, secondArray+second.लंबाई(), doNormalize,
                                   safe_मध्य, बफर, errorCode);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        अगर(U_FAILURE(errorCode)) {
            // Restore the modified suffix of the first string.
            first.replace(first_लंबाई-safe_मध्य.लंबाई(), 0x7fffffff, safe_मध्य);
        }
        वापस first;
    }
    भव व्योम
    normalizeAndAppend(स्थिर char16_t *src, स्थिर char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_मध्य,
                       Reordering_बफर &बफर, UErrorCode &errorCode) स्थिर = 0;
    भव UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) स्थिर override {
        char16_t बफर[4];
        int32_t लंबाई;
        स्थिर char16_t *d=impl.getDecomposition(c, बफर, लंबाई);
        अगर(d==nullptr) {
            वापस false;
        }
        अगर(d==बफर) {
            decomposition.setTo(बफर, लंबाई);  // copy the string (Jamos from Hangul syllable c)
        } अन्यथा {
            decomposition.setTo(false, d, लंबाई);  // read-only alias
        }
        वापस true;
    }
    भव UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) स्थिर override {
        char16_t बफर[30];
        int32_t लंबाई;
        स्थिर char16_t *d=impl.getRawDecomposition(c, बफर, लंबाई);
        अगर(d==nullptr) {
            वापस false;
        }
        अगर(d==बफर) {
            decomposition.setTo(बफर, लंबाई);  // copy the string (algorithmic decomposition)
        } अन्यथा {
            decomposition.setTo(false, d, लंबाई);  // read-only alias
        }
        वापस true;
    }
    भव UChar32
    composePair(UChar32 a, UChar32 b) स्थिर override {
        वापस impl.composePair(a, b);
    }

    भव uint8_t
    getCombiningClass(UChar32 c) स्थिर override {
        वापस impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    भव UBool
    isNormalized(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर override {
        अगर(U_FAILURE(errorCode)) {
            वापस false;
        }
        स्थिर char16_t *sArray=s.get_बफर();
        अगर(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            वापस false;
        }
        स्थिर char16_t *sLimit=sArray+s.लंबाई();
        वापस sLimit==spanQuickCheckYes(sArray, sLimit, errorCode);
    }
    भव UNormalization_Check_परिणाम
    quickCheck(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर override {
        वापस Normalizer2WithImpl::isNormalized(s, errorCode) ? UNORM_YES : UNORM_NO;
    }
    भव int32_t
    spanQuickCheckYes(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर override {
        अगर(U_FAILURE(errorCode)) {
            वापस 0;
        }
        स्थिर char16_t *sArray=s.get_बफर();
        अगर(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            वापस 0;
        }
        वापस static_cast<int32_t>(spanQuickCheckYes(sArray, sArray + s.लंबाई(), errorCode) - sArray);
    }
    भव स्थिर char16_t*
    spanQuickCheckYes(स्थिर char16_t* src U_LIFETIME_BOUND,
                      स्थिर char16_t* limit,
                      UErrorCode& errorCode) स्थिर = 0;

    भव UNormalization_Check_परिणाम getQuickCheck(UChar32) स्थिर {
        वापस UNORM_YES;
    }

    स्थिर Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : खुला Normalizer2WithImpl {
खुला:
    DecomposeNormalizer2(स्थिर Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    भव ~DecomposeNormalizer2();

निजी:
    भव व्योम
    normalize(स्थिर char16_t *src, स्थिर char16_t *limit,
              Reordering_बफर &बफर, UErrorCode &errorCode) स्थिर override {
        impl.decompose(src, limit, &बफर, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    भव व्योम
    normalizeAndAppend(स्थिर char16_t *src, स्थिर char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_मध्य,
                       Reordering_बफर &बफर, UErrorCode &errorCode) स्थिर override {
        impl.decomposeAndAppend(src, limit, doNormalize, safe_मध्य, बफर, errorCode);
    }

    व्योम
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) स्थिर override {
        अगर (U_FAILURE(errorCode)) {
            वापस;
        }
        अगर (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        स्थिर uint8_t *s = reinterpret_cast<स्थिर uint8_t *>(src.तथ्य());
        impl.decomposeUTF8(options, s, s + src.लंबाई(), &sink, edits, errorCode);
        sink.Flush();
    }
    भव UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) स्थिर override {
        अगर(U_FAILURE(errorCode)) {
            वापस false;
        }
        स्थिर uint8_t *s = reinterpret_cast<स्थिर uint8_t *>(sp.तथ्य());
        स्थिर uint8_t *sLimit = s + sp.लंबाई();
        वापस sLimit == impl.decomposeUTF8(0, s, sLimit, nullptr, nullptr, errorCode);
    }

    भव स्थिर char16_t*
    spanQuickCheckYes(स्थिर char16_t* src U_LIFETIME_BOUND,
                      स्थिर char16_t* limit,
                      UErrorCode& errorCode) स्थिर override {
        वापस impl.decompose(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    भव UNormalization_Check_परिणाम getQuickCheck(UChar32 c) स्थिर override {
        वापस impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    भव UBool hasBoundaryBefore(UChar32 c) स्थिर override {
        वापस impl.hasDecompBoundaryBefore(c);
    }
    भव UBool hasBoundaryAfter(UChar32 c) स्थिर override {
        वापस impl.hasDecompBoundaryAfter(c);
    }
    भव UBool isInert(UChar32 c) स्थिर override {
        वापस impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : खुला Normalizer2WithImpl {
खुला:
    ComposeNormalizer2(स्थिर Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    भव ~ComposeNormalizer2();

निजी:
    भव व्योम
    normalize(स्थिर char16_t *src, स्थिर char16_t *limit,
              Reordering_बफर &बफर, UErrorCode &errorCode) स्थिर override {
        impl.compose(src, limit, onlyContiguous, true, बफर, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    व्योम
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) स्थिर override {
        अगर (U_FAILURE(errorCode)) {
            वापस;
        }
        अगर (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        स्थिर uint8_t *s = reinterpret_cast<स्थिर uint8_t *>(src.तथ्य());
        impl.composeUTF8(options, onlyContiguous, s, s + src.लंबाई(),
                         &sink, edits, errorCode);
        sink.Flush();
    }

    भव व्योम
    normalizeAndAppend(स्थिर char16_t *src, स्थिर char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_मध्य,
                       Reordering_बफर &बफर, UErrorCode &errorCode) स्थिर override {
        impl.composeAndAppend(src, limit, doNormalize, onlyContiguous, safe_मध्य, बफर, errorCode);
    }

    भव UBool
    isNormalized(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर override {
        अगर(U_FAILURE(errorCode)) {
            वापस false;
        }
        स्थिर char16_t *sArray=s.get_बफर();
        अगर(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            वापस false;
        }
        UnicodeString अस्थायी;
        Reordering_बफर बफर(impl, अस्थायी);
        अगर(!बफर.init(5, errorCode)) {  // small destCapacity for substring normalization
            वापस false;
        }
        वापस impl.compose(sArray, sArray+s.लंबाई(), onlyContiguous, false, बफर, errorCode);
    }
    भव UBool
    isNormalizedUTF8(StringPiece sp, UErrorCode &errorCode) स्थिर override {
        अगर(U_FAILURE(errorCode)) {
            वापस false;
        }
        स्थिर uint8_t *s = reinterpret_cast<स्थिर uint8_t *>(sp.तथ्य());
        वापस impl.composeUTF8(0, onlyContiguous, s, s + sp.लंबाई(), nullptr, nullptr, errorCode);
    }
    भव UNormalization_Check_परिणाम
    quickCheck(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर override {
        अगर(U_FAILURE(errorCode)) {
            वापस UNORM_MAYBE;
        }
        स्थिर char16_t *sArray=s.get_बफर();
        अगर(sArray==nullptr) {
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            वापस UNORM_MAYBE;
        }
        UNormalization_Check_परिणाम qc_परिणाम=UNORM_YES;
        impl.composeQuickCheck(sArray, sArray+s.लंबाई(), onlyContiguous, &qc_परिणाम);
        वापस qc_परिणाम;
    }
    भव स्थिर char16_t*
    spanQuickCheckYes(स्थिर char16_t* src U_LIFETIME_BOUND,
                      स्थिर char16_t* limit,
                      UErrorCode&) स्थिर override {
        वापस impl.composeQuickCheck(src, limit, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    भव UNormalization_Check_परिणाम getQuickCheck(UChar32 c) स्थिर override {
        वापस impl.getCompQuickCheck(impl.getNorm16(c));
    }
    भव UBool hasBoundaryBefore(UChar32 c) स्थिर override {
        वापस impl.hasCompBoundaryBefore(c);
    }
    भव UBool hasBoundaryAfter(UChar32 c) स्थिर override {
        वापस impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    भव UBool isInert(UChar32 c) स्थिर override {
        वापस impl.isCompInert(c, onlyContiguous);
    }

    स्थिर UBool onlyContiguous;
};

class FCDNormalizer2 : खुला Normalizer2WithImpl {
खुला:
    FCDNormalizer2(स्थिर Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    भव ~FCDNormalizer2();

निजी:
    भव व्योम
    normalize(स्थिर char16_t *src, स्थिर char16_t *limit,
              Reordering_बफर &बफर, UErrorCode &errorCode) स्थिर override {
        impl.makeFCD(src, limit, &बफर, errorCode);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    भव व्योम
    normalizeAndAppend(स्थिर char16_t *src, स्थिर char16_t *limit, UBool doNormalize,
                       UnicodeString &safe_मध्य,
                       Reordering_बफर &बफर, UErrorCode &errorCode) स्थिर override {
        impl.makeFCDAndAppend(src, limit, doNormalize, safe_मध्य, बफर, errorCode);
    }
    भव स्थिर char16_t*
    spanQuickCheckYes(स्थिर char16_t* src U_LIFETIME_BOUND,
                      स्थिर char16_t* limit,
                      UErrorCode& errorCode) स्थिर override {
        वापस impl.makeFCD(src, limit, nullptr, errorCode);
    }
    using Normalizer2WithImpl::spanQuickCheckYes;  // Avoid warning about hiding base class function.
    भव UBool hasBoundaryBefore(UChar32 c) स्थिर override {
        वापस impl.hasFCDBoundaryBefore(c);
    }
    भव UBool hasBoundaryAfter(UChar32 c) स्थिर override {
        वापस impl.hasFCDBoundaryAfter(c);
    }
    भव UBool isInert(UChar32 c) स्थिर override {
        वापस impl.isFCDInert(c);
    }
};

काष्ठा Norm2AllModes : खुला UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    अटल Norm2AllModes *createInstance(Normalizer2Impl *impl, UErrorCode &errorCode);
    अटल Norm2AllModes *createNFCInstance(UErrorCode &errorCode);
    अटल Norm2AllModes *createInstance(स्थिर अक्षर *packageName,
                                         स्थिर अक्षर *name,
                                         UErrorCode &errorCode);

    अटल स्थिर Norm2AllModes *getNFCInstance(UErrorCode &errorCode);
    अटल स्थिर Norm2AllModes *getNFKCInstance(UErrorCode &errorCode);
    अटल स्थिर Norm2AllModes *getNFKC_CFInstance(UErrorCode &errorCode);
    अटल स्थिर Norm2AllModes *getNFKC_SCFInstance(UErrorCode &errorCode);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_END

#पूर्ण_अगर  // !UCONFIG_NO_NORMALIZATION
#पूर्ण_अगर  // __NORM2ALLMODES_H__
