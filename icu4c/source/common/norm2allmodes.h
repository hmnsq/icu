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

#यदि_अपरिभाषितम् __NORM2ALLMODES_H__
#परिभाषय __NORM2ALLMODES_H__

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
    Normalizer2WithImpl(ध्रुव Normalizer2Impl &ni) : impl(ni) {}
    आभास ~Normalizer2WithImpl();

    // normalize
    आभास UnicodeString&
    normalize(ध्रुव UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UError_संहिता& error_संहिता) ध्रुव override {
        यदि(U_FAILURE(error_संहिता)) {
            dest.समुच्चयः_गन्तव्यम्_Bogus();
            प्रत्यागम dest;
        }
        ध्रुव char16_t *sArray=src.get_बफर();
        यदि(&dest==&src || sArray==nullptr) {
            error_संहिता=U_ILLEGAL_तर्कः_ERROR;
            dest.समुच्चयः_गन्तव्यम्_Bogus();
            प्रत्यागम dest;
        }
        dest.remove();
        Reordering_बफर बफर(impl, dest);
        यदि(बफर.init(src.दीर्घता(), error_संहिता)) {
            normalize(sArray, sArray+src.दीर्घता(), बफर, error_संहिता);
        }
        प्रत्यागम dest;
    }
    आभास शून्य
    normalize(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
              Reordering_बफर &बफर, UError_संहिता &error_संहिता) ध्रुव = 0;

    // normalize and append
    आभास UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ध्रुव UnicodeString& second,
                             UError_संहिता& error_संहिता) ध्रुव override {
        प्रत्यागम normalizeSecondAndAppend(first, second, true, error_संहिता);
    }
    आभास UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           ध्रुव UnicodeString& second,
           UError_संहिता& error_संहिता) ध्रुव override {
        प्रत्यागम normalizeSecondAndAppend(first, second, false, error_संहिता);
    }
    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             ध्रुव UnicodeString& second,
                             UBool doNormalize,
                             UError_संहिता& error_संहिता) ध्रुव {
        uprv_check_Can_Get_बफर(first, error_संहिता);
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम first;
        }
        ध्रुव char16_t *secondArray=second.get_बफर();
        यदि(&first==&second || secondArray==nullptr) {
            error_संहिता=U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम first;
        }
        int32_t first_दीर्घता=first.दीर्घता();
        UnicodeString safe_मध्यः;
        {
            Reordering_बफर बफर(impl, first);
            यदि(बफर.init(first_दीर्घता+second.दीर्घता(), error_संहिता)) {
                normalizeAndAppend(secondArray, secondArray+second.दीर्घता(), doNormalize,
                                   safe_मध्यः, बफर, error_संहिता);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        यदि(U_FAILURE(error_संहिता)) {
            // Restore the modified suffix of the first string.
            first.replace(first_दीर्घता-safe_मध्यः.दीर्घता(), 0x7fffffff, safe_मध्यः);
        }
        प्रत्यागम first;
    }
    आभास शून्य
    normalizeAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *सीमा, UBool doNormalize,
                       UnicodeString &safe_मध्यः,
                       Reordering_बफर &बफर, UError_संहिता &error_संहिता) ध्रुव = 0;
    आभास UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) ध्रुव override {
        char16_t बफर[4];
        int32_t दीर्घता;
        ध्रुव char16_t *d=impl.getDecomposition(c, बफर, दीर्घता);
        यदि(d==nullptr) {
            प्रत्यागम false;
        }
        यदि(d==बफर) {
            decomposition.समुच्चयः_गन्तव्यम्(बफर, दीर्घता);  // copy the string (Jamos from Hangul syllable c)
        } अन्यथा {
            decomposition.समुच्चयः_गन्तव्यम्(false, d, दीर्घता);  // read-only alias
        }
        प्रत्यागम true;
    }
    आभास UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) ध्रुव override {
        char16_t बफर[30];
        int32_t दीर्घता;
        ध्रुव char16_t *d=impl.getRawDecomposition(c, बफर, दीर्घता);
        यदि(d==nullptr) {
            प्रत्यागम false;
        }
        यदि(d==बफर) {
            decomposition.समुच्चयः_गन्तव्यम्(बफर, दीर्घता);  // copy the string (algorithmic decomposition)
        } अन्यथा {
            decomposition.समुच्चयः_गन्तव्यम्(false, d, दीर्घता);  // read-only alias
        }
        प्रत्यागम true;
    }
    आभास UChar32
    composePair(UChar32 a, UChar32 b) ध्रुव override {
        प्रत्यागम impl.composePair(a, b);
    }

    आभास uint8_t
    getCombiningClass(UChar32 c) ध्रुव override {
        प्रत्यागम impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    आभास UBool
    isNormalized(ध्रुव UnicodeString &s, UError_संहिता &error_संहिता) ध्रुव override {
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम false;
        }
        ध्रुव char16_t *sArray=s.get_बफर();
        यदि(sArray==nullptr) {
            error_संहिता=U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम false;
        }
        ध्रुव char16_t *s_सीमा=sArray+s.दीर्घता();
        प्रत्यागम s_सीमा==विस्तृतिः_शीघ्र_Check_Yes(sArray, s_सीमा, error_संहिता);
    }
    आभास UNormalization_Check_परिणाम
    शीघ्र_Check(ध्रुव UnicodeString &s, UError_संहिता &error_संहिता) ध्रुव override {
        प्रत्यागम Normalizer2WithImpl::isNormalized(s, error_संहिता) ? UNORM_YES : UNORM_NO;
    }
    आभास int32_t
    विस्तृतिः_शीघ्र_Check_Yes(ध्रुव UnicodeString &s, UError_संहिता &error_संहिता) ध्रुव override {
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम 0;
        }
        ध्रुव char16_t *sArray=s.get_बफर();
        यदि(sArray==nullptr) {
            error_संहिता=U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम 0;
        }
        प्रत्यागम static_cast<int32_t>(विस्तृतिः_शीघ्र_Check_Yes(sArray, sArray + s.दीर्घता(), error_संहिता) - sArray);
    }
    आभास ध्रुव char16_t*
    विस्तृतिः_शीघ्र_Check_Yes(ध्रुव char16_t* src U_LIFETIME_BOUND,
                      ध्रुव char16_t* सीमा,
                      UError_संहिता& error_संहिता) ध्रुव = 0;

    आभास UNormalization_Check_परिणाम get_शीघ्र_Check(UChar32) ध्रुव {
        प्रत्यागम UNORM_YES;
    }

    ध्रुव Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : सार्वजनिक Normalizer2WithImpl {
सार्वजनिक:
    DecomposeNormalizer2(ध्रुव Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    आभास ~DecomposeNormalizer2();

गुप्त:
    आभास शून्य
    normalize(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
              Reordering_बफर &बफर, UError_संहिता &error_संहिता) ध्रुव override {
        impl.decompose(src, सीमा, &बफर, error_संहिता);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    आभास शून्य
    normalizeAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *सीमा, UBool doNormalize,
                       UnicodeString &safe_मध्यः,
                       Reordering_बफर &बफर, UError_संहिता &error_संहिता) ध्रुव override {
        impl.decomposeAndAppend(src, सीमा, doNormalize, safe_मध्यः, बफर, error_संहिता);
    }

    शून्य
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_संहिता &error_संहिता) ध्रुव override {
        यदि (U_FAILURE(error_संहिता)) {
            प्रत्यागम;
        }
        यदि (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ध्रुव uint8_t *s = reinterpret_cast<ध्रुव uint8_t *>(src.दत्त());
        impl.decomposeUTF8(options, s, s + src.दीर्घता(), &sink, edits, error_संहिता);
        sink.Flush();
    }
    आभास UBool
    isNormalizedUTF8(StringPiece sp, UError_संहिता &error_संहिता) ध्रुव override {
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम false;
        }
        ध्रुव uint8_t *s = reinterpret_cast<ध्रुव uint8_t *>(sp.दत्त());
        ध्रुव uint8_t *s_सीमा = s + sp.दीर्घता();
        प्रत्यागम s_सीमा == impl.decomposeUTF8(0, s, s_सीमा, nullptr, nullptr, error_संहिता);
    }

    आभास ध्रुव char16_t*
    विस्तृतिः_शीघ्र_Check_Yes(ध्रुव char16_t* src U_LIFETIME_BOUND,
                      ध्रुव char16_t* सीमा,
                      UError_संहिता& error_संहिता) ध्रुव override {
        प्रत्यागम impl.decompose(src, सीमा, nullptr, error_संहिता);
    }
    using Normalizer2WithImpl::विस्तृतिः_शीघ्र_Check_Yes;  // Avoid warning about hiding base class function.
    आभास UNormalization_Check_परिणाम get_शीघ्र_Check(UChar32 c) ध्रुव override {
        प्रत्यागम impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    आभास UBool hasBoundaryBefore(UChar32 c) ध्रुव override {
        प्रत्यागम impl.hasDecompBoundaryBefore(c);
    }
    आभास UBool hasBoundaryAfter(UChar32 c) ध्रुव override {
        प्रत्यागम impl.hasDecompBoundaryAfter(c);
    }
    आभास UBool isInert(UChar32 c) ध्रुव override {
        प्रत्यागम impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : सार्वजनिक Normalizer2WithImpl {
सार्वजनिक:
    ComposeNormalizer2(ध्रुव Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    आभास ~ComposeNormalizer2();

गुप्त:
    आभास शून्य
    normalize(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
              Reordering_बफर &बफर, UError_संहिता &error_संहिता) ध्रुव override {
        impl.compose(src, सीमा, onlyContiguous, true, बफर, error_संहिता);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    शून्य
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_संहिता &error_संहिता) ध्रुव override {
        यदि (U_FAILURE(error_संहिता)) {
            प्रत्यागम;
        }
        यदि (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        ध्रुव uint8_t *s = reinterpret_cast<ध्रुव uint8_t *>(src.दत्त());
        impl.composeUTF8(options, onlyContiguous, s, s + src.दीर्घता(),
                         &sink, edits, error_संहिता);
        sink.Flush();
    }

    आभास शून्य
    normalizeAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *सीमा, UBool doNormalize,
                       UnicodeString &safe_मध्यः,
                       Reordering_बफर &बफर, UError_संहिता &error_संहिता) ध्रुव override {
        impl.composeAndAppend(src, सीमा, doNormalize, onlyContiguous, safe_मध्यः, बफर, error_संहिता);
    }

    आभास UBool
    isNormalized(ध्रुव UnicodeString &s, UError_संहिता &error_संहिता) ध्रुव override {
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम false;
        }
        ध्रुव char16_t *sArray=s.get_बफर();
        यदि(sArray==nullptr) {
            error_संहिता=U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम false;
        }
        UnicodeString अस्थायी;
        Reordering_बफर बफर(impl, अस्थायी);
        यदि(!बफर.init(5, error_संहिता)) {  // small destCapacity for substring normalization
            प्रत्यागम false;
        }
        प्रत्यागम impl.compose(sArray, sArray+s.दीर्घता(), onlyContiguous, false, बफर, error_संहिता);
    }
    आभास UBool
    isNormalizedUTF8(StringPiece sp, UError_संहिता &error_संहिता) ध्रुव override {
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम false;
        }
        ध्रुव uint8_t *s = reinterpret_cast<ध्रुव uint8_t *>(sp.दत्त());
        प्रत्यागम impl.composeUTF8(0, onlyContiguous, s, s + sp.दीर्घता(), nullptr, nullptr, error_संहिता);
    }
    आभास UNormalization_Check_परिणाम
    शीघ्र_Check(ध्रुव UnicodeString &s, UError_संहिता &error_संहिता) ध्रुव override {
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम UNORM_MAYBE;
        }
        ध्रुव char16_t *sArray=s.get_बफर();
        यदि(sArray==nullptr) {
            error_संहिता=U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम UNORM_MAYBE;
        }
        UNormalization_Check_परिणाम qc_परिणाम=UNORM_YES;
        impl.compose_शीघ्र_Check(sArray, sArray+s.दीर्घता(), onlyContiguous, &qc_परिणाम);
        प्रत्यागम qc_परिणाम;
    }
    आभास ध्रुव char16_t*
    विस्तृतिः_शीघ्र_Check_Yes(ध्रुव char16_t* src U_LIFETIME_BOUND,
                      ध्रुव char16_t* सीमा,
                      UError_संहिता&) ध्रुव override {
        प्रत्यागम impl.compose_शीघ्र_Check(src, सीमा, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::विस्तृतिः_शीघ्र_Check_Yes;  // Avoid warning about hiding base class function.
    आभास UNormalization_Check_परिणाम get_शीघ्र_Check(UChar32 c) ध्रुव override {
        प्रत्यागम impl.get_Comp_शीघ्र_Check(impl.getNorm16(c));
    }
    आभास UBool hasBoundaryBefore(UChar32 c) ध्रुव override {
        प्रत्यागम impl.hasCompBoundaryBefore(c);
    }
    आभास UBool hasBoundaryAfter(UChar32 c) ध्रुव override {
        प्रत्यागम impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    आभास UBool isInert(UChar32 c) ध्रुव override {
        प्रत्यागम impl.isCompInert(c, onlyContiguous);
    }

    ध्रुव UBool onlyContiguous;
};

class FCDNormalizer2 : सार्वजनिक Normalizer2WithImpl {
सार्वजनिक:
    FCDNormalizer2(ध्रुव Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    आभास ~FCDNormalizer2();

गुप्त:
    आभास शून्य
    normalize(ध्रुव char16_t *src, ध्रुव char16_t *सीमा,
              Reordering_बफर &बफर, UError_संहिता &error_संहिता) ध्रुव override {
        impl.makeFCD(src, सीमा, &बफर, error_संहिता);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    आभास शून्य
    normalizeAndAppend(ध्रुव char16_t *src, ध्रुव char16_t *सीमा, UBool doNormalize,
                       UnicodeString &safe_मध्यः,
                       Reordering_बफर &बफर, UError_संहिता &error_संहिता) ध्रुव override {
        impl.makeFCDAndAppend(src, सीमा, doNormalize, safe_मध्यः, बफर, error_संहिता);
    }
    आभास ध्रुव char16_t*
    विस्तृतिः_शीघ्र_Check_Yes(ध्रुव char16_t* src U_LIFETIME_BOUND,
                      ध्रुव char16_t* सीमा,
                      UError_संहिता& error_संहिता) ध्रुव override {
        प्रत्यागम impl.makeFCD(src, सीमा, nullptr, error_संहिता);
    }
    using Normalizer2WithImpl::विस्तृतिः_शीघ्र_Check_Yes;  // Avoid warning about hiding base class function.
    आभास UBool hasBoundaryBefore(UChar32 c) ध्रुव override {
        प्रत्यागम impl.hasFCDBoundaryBefore(c);
    }
    आभास UBool hasBoundaryAfter(UChar32 c) ध्रुव override {
        प्रत्यागम impl.hasFCDBoundaryAfter(c);
    }
    आभास UBool isInert(UChar32 c) ध्रुव override {
        प्रत्यागम impl.isFCDInert(c);
    }
};

संरचना Norm2AllModes : सार्वजनिक UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    स्थिर Norm2AllModes *createInstance(Normalizer2Impl *impl, UError_संहिता &error_संहिता);
    स्थिर Norm2AllModes *createNFCInstance(UError_संहिता &error_संहिता);
    स्थिर Norm2AllModes *createInstance(ध्रुव वर्ण *packageName,
                                         ध्रुव वर्ण *name,
                                         UError_संहिता &error_संहिता);

    स्थिर ध्रुव Norm2AllModes *getNFCInstance(UError_संहिता &error_संहिता);
    स्थिर ध्रुव Norm2AllModes *getNFKCInstance(UError_संहिता &error_संहिता);
    स्थिर ध्रुव Norm2AllModes *getNFKC_CFInstance(UError_संहिता &error_संहिता);
    स्थिर ध्रुव Norm2AllModes *getNFKC_SCFInstance(UError_संहिता &error_संहिता);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः  // !UCONFIG_NO_NORMALIZATION
#यदि_अन्तः  // __NORM2ALLMODES_H__
