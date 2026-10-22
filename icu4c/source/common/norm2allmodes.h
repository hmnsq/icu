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

#నిర్వచితం_కాకపోతే __NORM2ALLMODES_H__
#నిర్వచించు __NORM2ALLMODES_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_NORMALIZATION

#అన్తర్భావ "unicode/edits.h"
#అన్తర్భావ "unicode/normalizer2.h"
#అన్తర్భావ "unicode/stringoptions.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "cpputils.h"
#అన్తర్భావ "normalizer2impl.h"

U_NAMESPACE_BEGIN

// Intermediate class:
// Has Normalizer2Impl and does boilerplate argument checking and setup.
class Normalizer2WithImpl : బహిరంగ Normalizer2 {
బహిరంగ:
    Normalizer2WithImpl(స్థిరరాశి Normalizer2Impl &ni) : impl(ni) {}
    మాయా ~Normalizer2WithImpl();

    // normalize
    మాయా UnicodeString&
    normalize(స్థిరరాశి UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UError_సంకేతం& error_సంకేతం) స్థిరరాశి override {
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            dest.సమితి_గమ్యం_Bogus();
            తిరిగి dest;
        }
        స్థిరరాశి char16_t *sArray=src.get_బఫరు();
        ఒకవేళ(&dest==&src || sArray==nullptr) {
            error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
            dest.సమితి_గమ్యం_Bogus();
            తిరిగి dest;
        }
        dest.remove();
        Reordering_బఫరు బఫరు(impl, dest);
        ఒకవేళ(బఫరు.init(src.నిడివి(), error_సంకేతం)) {
            normalize(sArray, sArray+src.నిడివి(), బఫరు, error_సంకేతం);
        }
        తిరిగి dest;
    }
    మాయా శూన్య
    normalize(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు,
              Reordering_బఫరు &బఫరు, UError_సంకేతం &error_సంకేతం) స్థిరరాశి = 0;

    // normalize and append
    మాయా UnicodeString&
    normalize_రెండవ_And_Append(UnicodeString& మొదటి U_LIFETIME_BOUND,
                             స్థిరరాశి UnicodeString& రెండవ,
                             UError_సంకేతం& error_సంకేతం) స్థిరరాశి override {
        తిరిగి normalize_రెండవ_And_Append(మొదటి, రెండవ, true, error_సంకేతం);
    }
    మాయా UnicodeString&
    append(UnicodeString& మొదటి U_LIFETIME_BOUND,
           స్థిరరాశి UnicodeString& రెండవ,
           UError_సంకేతం& error_సంకేతం) స్థిరరాశి override {
        తిరిగి normalize_రెండవ_And_Append(మొదటి, రెండవ, false, error_సంకేతం);
    }
    UnicodeString&
    normalize_రెండవ_And_Append(UnicodeString& మొదటి U_LIFETIME_BOUND,
                             స్థిరరాశి UnicodeString& రెండవ,
                             UBool doNormalize,
                             UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
        uprv_check_Can_Get_బఫరు(మొదటి, error_సంకేతం);
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి మొదటి;
        }
        స్థిరరాశి char16_t *రెండవ_Array=రెండవ.get_బఫరు();
        ఒకవేళ(&మొదటి==&రెండవ || రెండవ_Array==nullptr) {
            error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
            తిరిగి మొదటి;
        }
        int32_t మొదటి_నిడివి=మొదటి.నిడివి();
        UnicodeString సురక్షిత_మధ్య;
        {
            Reordering_బఫరు బఫరు(impl, మొదటి);
            ఒకవేళ(బఫరు.init(మొదటి_నిడివి+రెండవ.నిడివి(), error_సంకేతం)) {
                normalizeAndAppend(రెండవ_Array, రెండవ_Array+రెండవ.నిడివి(), doNormalize,
                                   సురక్షిత_మధ్య, బఫరు, error_సంకేతం);
            }
        }  // The ReorderingBuffer destructor finalizes the first string.
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            // Restore the modified suffix of the first string.
            మొదటి.replace(మొదటి_నిడివి-సురక్షిత_మధ్య.నిడివి(), 0x7fffffff, సురక్షిత_మధ్య);
        }
        తిరిగి మొదటి;
    }
    మాయా శూన్య
    normalizeAndAppend(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు, UBool doNormalize,
                       UnicodeString &సురక్షిత_మధ్య,
                       Reordering_బఫరు &బఫరు, UError_సంకేతం &error_సంకేతం) స్థిరరాశి = 0;
    మాయా UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) స్థిరరాశి override {
        char16_t బఫరు[4];
        int32_t నిడివి;
        స్థిరరాశి char16_t *d=impl.getDecomposition(c, బఫరు, నిడివి);
        ఒకవేళ(d==nullptr) {
            తిరిగి false;
        }
        ఒకవేళ(d==బఫరు) {
            decomposition.సమితి_గమ్యం(బఫరు, నిడివి);  // copy the string (Jamos from Hangul syllable c)
        } లేకపోతే {
            decomposition.సమితి_గమ్యం(false, d, నిడివి);  // read-only alias
        }
        తిరిగి true;
    }
    మాయా UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) స్థిరరాశి override {
        char16_t బఫరు[30];
        int32_t నిడివి;
        స్థిరరాశి char16_t *d=impl.getRawDecomposition(c, బఫరు, నిడివి);
        ఒకవేళ(d==nullptr) {
            తిరిగి false;
        }
        ఒకవేళ(d==బఫరు) {
            decomposition.సమితి_గమ్యం(బఫరు, నిడివి);  // copy the string (algorithmic decomposition)
        } లేకపోతే {
            decomposition.సమితి_గమ్యం(false, d, నిడివి);  // read-only alias
        }
        తిరిగి true;
    }
    మాయా UChar32
    composePair(UChar32 a, UChar32 b) స్థిరరాశి override {
        తిరిగి impl.composePair(a, b);
    }

    మాయా uint8_t
    getCombiningClass(UChar32 c) స్థిరరాశి override {
        తిరిగి impl.getCC(impl.getNorm16(c));
    }

    // quick checks
    మాయా UBool
    isNormalized(స్థిరరాశి UnicodeString &s, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి false;
        }
        స్థిరరాశి char16_t *sArray=s.get_బఫరు();
        ఒకవేళ(sArray==nullptr) {
            error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
            తిరిగి false;
        }
        స్థిరరాశి char16_t *s_హద్దు=sArray+s.నిడివి();
        తిరిగి s_హద్దు==పరిధి_శీఘ్ర_Check_Yes(sArray, s_హద్దు, error_సంకేతం);
    }
    మాయా UNormalization_Check_ఫలితం
    శీఘ్ర_Check(స్థిరరాశి UnicodeString &s, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        తిరిగి Normalizer2WithImpl::isNormalized(s, error_సంకేతం) ? UNORM_YES : UNORM_NO;
    }
    మాయా int32_t
    పరిధి_శీఘ్ర_Check_Yes(స్థిరరాశి UnicodeString &s, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి 0;
        }
        స్థిరరాశి char16_t *sArray=s.get_బఫరు();
        ఒకవేళ(sArray==nullptr) {
            error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
            తిరిగి 0;
        }
        తిరిగి static_cast<int32_t>(పరిధి_శీఘ్ర_Check_Yes(sArray, sArray + s.నిడివి(), error_సంకేతం) - sArray);
    }
    మాయా స్థిరరాశి char16_t*
    పరిధి_శీఘ్ర_Check_Yes(స్థిరరాశి char16_t* src U_LIFETIME_BOUND,
                      స్థిరరాశి char16_t* హద్దు,
                      UError_సంకేతం& error_సంకేతం) స్థిరరాశి = 0;

    మాయా UNormalization_Check_ఫలితం get_శీఘ్ర_Check(UChar32) స్థిరరాశి {
        తిరిగి UNORM_YES;
    }

    స్థిరరాశి Normalizer2Impl &impl;
};

class DecomposeNormalizer2 : బహిరంగ Normalizer2WithImpl {
బహిరంగ:
    DecomposeNormalizer2(స్థిరరాశి Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    మాయా ~DecomposeNormalizer2();

అంతరంగ:
    మాయా శూన్య
    normalize(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు,
              Reordering_బఫరు &బఫరు, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        impl.decompose(src, హద్దు, &బఫరు, error_సంకేతం);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    మాయా శూన్య
    normalizeAndAppend(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు, UBool doNormalize,
                       UnicodeString &సురక్షిత_మధ్య,
                       Reordering_బఫరు &బఫరు, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        impl.decomposeAndAppend(src, హద్దు, doNormalize, సురక్షిత_మధ్య, బఫరు, error_సంకేతం);
    }

    శూన్య
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        ఒకవేళ (U_FAILURE(error_సంకేతం)) {
            తిరిగి;
        }
        ఒకవేళ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        స్థిరరాశి uint8_t *s = reinterpret_cast<స్థిరరాశి uint8_t *>(src.దత్తాంశం());
        impl.decomposeUTF8(options, s, s + src.నిడివి(), &sink, edits, error_సంకేతం);
        sink.Flush();
    }
    మాయా UBool
    isNormalizedUTF8(StringPiece sp, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి false;
        }
        స్థిరరాశి uint8_t *s = reinterpret_cast<స్థిరరాశి uint8_t *>(sp.దత్తాంశం());
        స్థిరరాశి uint8_t *s_హద్దు = s + sp.నిడివి();
        తిరిగి s_హద్దు == impl.decomposeUTF8(0, s, s_హద్దు, nullptr, nullptr, error_సంకేతం);
    }

    మాయా స్థిరరాశి char16_t*
    పరిధి_శీఘ్ర_Check_Yes(స్థిరరాశి char16_t* src U_LIFETIME_BOUND,
                      స్థిరరాశి char16_t* హద్దు,
                      UError_సంకేతం& error_సంకేతం) స్థిరరాశి override {
        తిరిగి impl.decompose(src, హద్దు, nullptr, error_సంకేతం);
    }
    using Normalizer2WithImpl::పరిధి_శీఘ్ర_Check_Yes;  // Avoid warning about hiding base class function.
    మాయా UNormalization_Check_ఫలితం get_శీఘ్ర_Check(UChar32 c) స్థిరరాశి override {
        తిరిగి impl.isDecompYes(impl.getNorm16(c)) ? UNORM_YES : UNORM_NO;
    }
    మాయా UBool hasBoundaryBefore(UChar32 c) స్థిరరాశి override {
        తిరిగి impl.hasDecompBoundaryBefore(c);
    }
    మాయా UBool hasBoundaryAfter(UChar32 c) స్థిరరాశి override {
        తిరిగి impl.hasDecompBoundaryAfter(c);
    }
    మాయా UBool isInert(UChar32 c) స్థిరరాశి override {
        తిరిగి impl.isDecompInert(c);
    }
};

class ComposeNormalizer2 : బహిరంగ Normalizer2WithImpl {
బహిరంగ:
    ComposeNormalizer2(స్థిరరాశి Normalizer2Impl &ni, UBool fcc) :
        Normalizer2WithImpl(ni), onlyContiguous(fcc) {}
    మాయా ~ComposeNormalizer2();

అంతరంగ:
    మాయా శూన్య
    normalize(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు,
              Reordering_బఫరు &బఫరు, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        impl.compose(src, హద్దు, onlyContiguous, true, బఫరు, error_సంకేతం);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.

    శూన్య
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        ఒకవేళ (U_FAILURE(error_సంకేతం)) {
            తిరిగి;
        }
        ఒకవేళ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
            edits->reset();
        }
        స్థిరరాశి uint8_t *s = reinterpret_cast<స్థిరరాశి uint8_t *>(src.దత్తాంశం());
        impl.composeUTF8(options, onlyContiguous, s, s + src.నిడివి(),
                         &sink, edits, error_సంకేతం);
        sink.Flush();
    }

    మాయా శూన్య
    normalizeAndAppend(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు, UBool doNormalize,
                       UnicodeString &సురక్షిత_మధ్య,
                       Reordering_బఫరు &బఫరు, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        impl.composeAndAppend(src, హద్దు, doNormalize, onlyContiguous, సురక్షిత_మధ్య, బఫరు, error_సంకేతం);
    }

    మాయా UBool
    isNormalized(స్థిరరాశి UnicodeString &s, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి false;
        }
        స్థిరరాశి char16_t *sArray=s.get_బఫరు();
        ఒకవేళ(sArray==nullptr) {
            error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
            తిరిగి false;
        }
        UnicodeString తాత్కాలిక;
        Reordering_బఫరు బఫరు(impl, తాత్కాలిక);
        ఒకవేళ(!బఫరు.init(5, error_సంకేతం)) {  // small destCapacity for substring normalization
            తిరిగి false;
        }
        తిరిగి impl.compose(sArray, sArray+s.నిడివి(), onlyContiguous, false, బఫరు, error_సంకేతం);
    }
    మాయా UBool
    isNormalizedUTF8(StringPiece sp, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి false;
        }
        స్థిరరాశి uint8_t *s = reinterpret_cast<స్థిరరాశి uint8_t *>(sp.దత్తాంశం());
        తిరిగి impl.composeUTF8(0, onlyContiguous, s, s + sp.నిడివి(), nullptr, nullptr, error_సంకేతం);
    }
    మాయా UNormalization_Check_ఫలితం
    శీఘ్ర_Check(స్థిరరాశి UnicodeString &s, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి UNORM_MAYBE;
        }
        స్థిరరాశి char16_t *sArray=s.get_బఫరు();
        ఒకవేళ(sArray==nullptr) {
            error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
            తిరిగి UNORM_MAYBE;
        }
        UNormalization_Check_ఫలితం qc_ఫలితం=UNORM_YES;
        impl.compose_శీఘ్ర_Check(sArray, sArray+s.నిడివి(), onlyContiguous, &qc_ఫలితం);
        తిరిగి qc_ఫలితం;
    }
    మాయా స్థిరరాశి char16_t*
    పరిధి_శీఘ్ర_Check_Yes(స్థిరరాశి char16_t* src U_LIFETIME_BOUND,
                      స్థిరరాశి char16_t* హద్దు,
                      UError_సంకేతం&) స్థిరరాశి override {
        తిరిగి impl.compose_శీఘ్ర_Check(src, హద్దు, onlyContiguous, nullptr);
    }
    using Normalizer2WithImpl::పరిధి_శీఘ్ర_Check_Yes;  // Avoid warning about hiding base class function.
    మాయా UNormalization_Check_ఫలితం get_శీఘ్ర_Check(UChar32 c) స్థిరరాశి override {
        తిరిగి impl.get_Comp_శీఘ్ర_Check(impl.getNorm16(c));
    }
    మాయా UBool hasBoundaryBefore(UChar32 c) స్థిరరాశి override {
        తిరిగి impl.hasCompBoundaryBefore(c);
    }
    మాయా UBool hasBoundaryAfter(UChar32 c) స్థిరరాశి override {
        తిరిగి impl.hasCompBoundaryAfter(c, onlyContiguous);
    }
    మాయా UBool isInert(UChar32 c) స్థిరరాశి override {
        తిరిగి impl.isCompInert(c, onlyContiguous);
    }

    స్థిరరాశి UBool onlyContiguous;
};

class FCDNormalizer2 : బహిరంగ Normalizer2WithImpl {
బహిరంగ:
    FCDNormalizer2(స్థిరరాశి Normalizer2Impl &ni) : Normalizer2WithImpl(ni) {}
    మాయా ~FCDNormalizer2();

అంతరంగ:
    మాయా శూన్య
    normalize(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు,
              Reordering_బఫరు &బఫరు, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        impl.makeFCD(src, హద్దు, &బఫరు, error_సంకేతం);
    }
    using Normalizer2WithImpl::normalize;  // Avoid warning about hiding base class function.
    మాయా శూన్య
    normalizeAndAppend(స్థిరరాశి char16_t *src, స్థిరరాశి char16_t *హద్దు, UBool doNormalize,
                       UnicodeString &సురక్షిత_మధ్య,
                       Reordering_బఫరు &బఫరు, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override {
        impl.makeFCDAndAppend(src, హద్దు, doNormalize, సురక్షిత_మధ్య, బఫరు, error_సంకేతం);
    }
    మాయా స్థిరరాశి char16_t*
    పరిధి_శీఘ్ర_Check_Yes(స్థిరరాశి char16_t* src U_LIFETIME_BOUND,
                      స్థిరరాశి char16_t* హద్దు,
                      UError_సంకేతం& error_సంకేతం) స్థిరరాశి override {
        తిరిగి impl.makeFCD(src, హద్దు, nullptr, error_సంకేతం);
    }
    using Normalizer2WithImpl::పరిధి_శీఘ్ర_Check_Yes;  // Avoid warning about hiding base class function.
    మాయా UBool hasBoundaryBefore(UChar32 c) స్థిరరాశి override {
        తిరిగి impl.hasFCDBoundaryBefore(c);
    }
    మాయా UBool hasBoundaryAfter(UChar32 c) స్థిరరాశి override {
        తిరిగి impl.hasFCDBoundaryAfter(c);
    }
    మాయా UBool isInert(UChar32 c) స్థిరరాశి override {
        తిరిగి impl.isFCDInert(c);
    }
};

నిర్మాణం Norm2AllModes : బహిరంగ UMemory {
    Norm2AllModes(Normalizer2Impl *i)
            : impl(i), comp(*i, false), decomp(*i), fcd(*i), fcc(*i, true) {}
    ~Norm2AllModes();

    స్థిర Norm2AllModes *createInstance(Normalizer2Impl *impl, UError_సంకేతం &error_సంకేతం);
    స్థిర Norm2AllModes *createNFCInstance(UError_సంకేతం &error_సంకేతం);
    స్థిర Norm2AllModes *createInstance(స్థిరరాశి అక్షరం *packageName,
                                         స్థిరరాశి అక్షరం *name,
                                         UError_సంకేతం &error_సంకేతం);

    స్థిర స్థిరరాశి Norm2AllModes *getNFCInstance(UError_సంకేతం &error_సంకేతం);
    స్థిర స్థిరరాశి Norm2AllModes *getNFKCInstance(UError_సంకేతం &error_సంకేతం);
    స్థిర స్థిరరాశి Norm2AllModes *getNFKC_CFInstance(UError_సంకేతం &error_సంకేతం);
    స్థిర స్థిరరాశి Norm2AllModes *getNFKC_SCFInstance(UError_సంకేతం &error_సంకేతం);

    Normalizer2Impl *impl;
    ComposeNormalizer2 comp;
    DecomposeNormalizer2 decomp;
    FCDNormalizer2 fcd;
    ComposeNormalizer2 fcc;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు  // !UCONFIG_NO_NORMALIZATION
#అయితే_ముగింపు  // __NORM2ALLMODES_H__
