// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
//  file:  rbbirb.cpp
//
//  Copyright (C) 2002-2011, International Business Machines Corporation and others.
//  All Rights Reserved.
//
//  This file contains the RBBIRuleBuilder class implementation.  This is the main class for
//    building (compiling) break rules into the tables required by the runtime
//    RBBI engine.
//

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/rbbi.h"
#अन्तर्भाव "unicode/ubrk.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/uchriter.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/parsepos.h"
#अन्तर्भाव "unicode/parseerr.h"

#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "rbbinode.h"
#अन्तर्भाव "rbbiscan.h"
#अन्तर्भाव "rbbisetb.h"
#अन्तर्भाव "rbbitblb.h"
#अन्तर्भाव "rbbidata.h"
#अन्तर्भाव "uassert.h"


U_NAMESPACE_BEGIN


//----------------------------------------------------------------------------------------
//
//  Constructor.
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::RBBIRuleBuilder(ध्रुव UnicodeString   &नियमाः,
                                       UParseError     *parseErr,
                                       UError_संहिता      &स्थितिः)
 : f_नियमाः(नियमाः), f_Stripped_नियमाः(नियमाः)
{
    f_स्थितिः = &स्थितिः; // status is checked below
    fParseError = parseErr;
    fDebugEnv   = शून्यसूचकः;
#यदि_परिभाषितम् RBBI_DEBUG
    fDebugEnv   = getenv("U_RBBIDEBUG");
#यदि_अन्तः


    f_Forward_वृक्षः        = शून्यसूचकः;
    f_Reverse_वृक्षः        = शून्यसूचकः;
    f_Safe_Fwd_वृक्षः        = शून्यसूचकः;
    f_Safe_Rev_वृक्षः        = शून्यसूचकः;
    f_Default_वृक्षः        = &f_Forward_वृक्षः;
    f_Forward_सारणी       = शून्यसूचकः;
    f_नियमः_स्थितिः_Vals     = शून्यसूचकः;
    f_Chain_नियमाः         = असत्य;
    fLookAheadHardBreak = असत्य;
    fUSetNodes          = शून्यसूचकः;
    f_नियमः_स्थितिः_Vals     = शून्यसूचकः;
    fScanner            = शून्यसूचकः;
    f_समुच्चयः_Builder         = शून्यसूचकः;
    यदि (parseErr) {
        uprv_memset(parseErr, 0, परिमाण(UParseError));
    }

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    fUSetNodes          = नूतन UVector(स्थितिः); // bcos status gets overwritten here
    f_नियमः_स्थितिः_Vals     = नूतन UVector(स्थितिः);
    fScanner            = नूतन RBBIRuleScanner(एतत्);
    f_समुच्चयः_Builder         = नूतन RBBISetBuilder(एतत्);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    यदि (f_समुच्चयः_Builder == शून्यसूचकः || fScanner == शून्यसूचकः ||
        fUSetNodes == शून्यसूचकः || f_नियमः_स्थितिः_Vals == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
}



//----------------------------------------------------------------------------------------
//
//  Destructor
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::~RBBIRuleBuilder() {

    पूर्णाङ्क        i;
    प्रति (i=0; ; i++) {
        RBBINode* n = static_cast<RBBINode*>(fUSetNodes->elementAt(i));
        यदि (n==शून्यसूचकः) {
            भङ्ग;
        }
        विलोपय n;
    }

    विलोपय fUSetNodes;
    विलोपय f_समुच्चयः_Builder;
    विलोपय f_Forward_सारणी;
    विलोपय f_Forward_वृक्षः;
    विलोपय f_Reverse_वृक्षः;
    विलोपय f_Safe_Fwd_वृक्षः;
    विलोपय f_Safe_Rev_वृक्षः;
    विलोपय fScanner;
    विलोपय f_नियमः_स्थितिः_Vals;
}





//----------------------------------------------------------------------------------------
//
//   flattenData() -  Collect up the compiled RBBI rule data and put it into
//                    the format for saving in ICU data files,
//                    which is also the format needed by the RBBI runtime engine.
//
//----------------------------------------------------------------------------------------
स्थिर int32_t align8(int32_t i) {प्रत्यागम (i+7) & 0xfffffff8;}

RBBIData_शीर्षकम् *RBBIRuleBuilder::flatten_दत्त() {
    int32_t    i;

    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    f_Stripped_नियमाः = fScanner->strip_नियमाः(f_Stripped_नियमाः);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t शीर्षकम्_आकार        = align8(परिमाण(RBBIData_शीर्षकम्));
    int32_t forward_सारणी_आकार  = align8(f_Forward_सारणी->get_सारणी_आकार());
    int32_t reverse_सारणी_आकार  = align8(f_Forward_सारणी->get_Safe_सारणी_आकार());
    int32_t trie_आकार          = align8(f_समुच्चयः_Builder->get_Trie_आकार());
    int32_t स्थितिः_सारणी_आकार   = align8(f_नियमः_स्थितिः_Vals->size() * परिमाण(int32_t));

    int32_t नियमाः_दीर्घता_अन्तर्_UTF8 = 0;
    u_str_गन्तव्यम्_UTF8_With_Sub(शून्यसूचकः, 0, &नियमाः_दीर्घता_अन्तर्_UTF8,
                       f_Stripped_नियमाः.get_बफर(), f_Stripped_नियमाः.length(),
                       0xfffd, शून्यसूचकः, f_स्थितिः);
    *f_स्थितिः = U_ZERO_ERROR;

    int32_t नियमाः_आकार         = align8((नियमाः_दीर्घता_अन्तर्_UTF8+1));

    int32_t         योग_आकार = शीर्षकम्_आकार
                                + forward_सारणी_आकार
                                + reverse_सारणी_आकार
                                + स्थितिः_सारणी_आकार + trie_आकार + नियमाः_आकार;

#यदि_परिभाषितम् RBBI_DEBUG
    यदि (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
        RBBIDebugPrintf("Header Size:        %8d\n", शीर्षकम्_आकार);
        RBBIDebugPrintf("Forward Table Size: %8d\n", forward_सारणी_आकार);
        RBBIDebugPrintf("Reverse Table Size: %8d\n", reverse_सारणी_आकार);
        RBBIDebugPrintf("Trie Size:          %8d\n", trie_आकार);
        RBBIDebugPrintf("Status Table Size:  %8d\n", स्थितिः_सारणी_आकार);
        RBBIDebugPrintf("Rules Size:         %8d\n", नियमाः_आकार);
        RBBIDebugPrintf("-----------------------------\n");
        RBBIDebugPrintf("Total Size:         %8d\n", योग_आकार);
    }
#यदि_अन्तः

    Local_स्मृतिः<RBBIData_शीर्षकम्> दत्त(static_cast<RBBIData_शीर्षकम्*>(uprv_malloc(योग_आकार)));
    यदि (दत्त.isNull()) {
        *f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    uprv_memset(दत्त.getAlias(), 0, योग_आकार);


    दत्त->fMagic            = 0xb1a0;
    दत्त->f_Format_संस्करणम्[0] = RBBI_दत्त_FORMAT_संस्करणम्[0];
    दत्त->f_Format_संस्करणम्[1] = RBBI_दत्त_FORMAT_संस्करणम्[1];
    दत्त->f_Format_संस्करणम्[2] = RBBI_दत्त_FORMAT_संस्करणम्[2];
    दत्त->f_Format_संस्करणम्[3] = RBBI_दत्त_FORMAT_संस्करणम्[3];
    दत्त->f_दीर्घता           = योग_आकार;
    दत्त->f_Cat_गणनम्         = f_समुच्चयः_Builder->get_अङ्कः_Char_Categories();

    दत्त->fFTable        = शीर्षकम्_आकार;
    दत्त->fFTableLen     = forward_सारणी_आकार;

    दत्त->fRTable        = दत्त->fFTable  + दत्त->fFTableLen;
    दत्त->fRTableLen     = reverse_सारणी_आकार;

    दत्त->fTrie          = दत्त->fRTable + दत्त->fRTableLen;
    दत्त->fTrieLen       = trie_आकार;
    दत्त->f_स्थितिः_सारणी   = दत्त->fTrie    + दत्त->fTrieLen;
    दत्त->f_स्थितिः_सारणी_Len= स्थितिः_सारणी_आकार;
    दत्त->f_नियमः_Source    = दत्त->f_स्थितिः_सारणी + स्थितिः_सारणी_आकार;
    दत्त->f_नियमः_Source_Len = नियमाः_दीर्घता_अन्तर्_UTF8;

    uprv_memset(दत्त->fReserved, 0, परिमाण(दत्त->fReserved));

    f_Forward_सारणी->export_सारणी(reinterpret_cast<uint8_t*>(दत्त.getAlias()) + दत्त->fFTable);
    f_Forward_सारणी->export_Safe_सारणी(reinterpret_cast<uint8_t*>(दत्त.getAlias()) + दत्त->fRTable);
    f_समुच्चयः_Builder->serializeTrie(reinterpret_cast<uint8_t*>(दत्त.getAlias()) + दत्त->fTrie);

    int32_t* नियमः_स्थितिः_सारणी = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(दत्त.getAlias()) + दत्त->f_स्थितिः_सारणी);
    प्रति (i=0; i<f_नियमः_स्थितिः_Vals->size(); i++) {
        नियमः_स्थितिः_सारणी[i] = f_नियमः_स्थितिः_Vals->elementAti(i);
    }

    u_str_गन्तव्यम्_UTF8_With_Sub(reinterpret_cast<वर्ण*>(दत्त.getAlias()) + दत्त->f_नियमः_Source, नियमाः_आकार, &नियमाः_दीर्घता_अन्तर्_UTF8,
                       f_Stripped_नियमाः.get_बफर(), f_Stripped_नियमाः.length(),
                       0xfffd, शून्यसूचकः, f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    प्रत्यागम दत्त.orphan();
}


//----------------------------------------------------------------------------------------
//
//  createRuleBasedBreakIterator    construct from source rules that are passed in
//                                  in a UnicodeString
//
//----------------------------------------------------------------------------------------
BreakIterator *
RBBIRuleBuilder::create_नियमः_Based_Break_Iterator( ध्रुव UnicodeString    &नियमाः,
                                    UParseError      *parseError,
                                    UError_संहिता       &स्थितिः)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(नियमाः, parseError, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) { // status checked here bcos build below doesn't
        प्रत्यागम शून्यसूचकः;
    }

    RBBIData_शीर्षकम् *दत्त = builder.build(स्थितिः);

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    नियमः_Based_Break_Iterator *This = नूतन नियमः_Based_Break_Iterator(दत्त, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        विलोपय This;
        This = शून्यसूचकः;
    } 
    अन्यथा यदि(This == शून्यसूचकः) { // test for nullptr
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
    प्रत्यागम This;
}

RBBIData_शीर्षकम् *RBBIRuleBuilder::build(UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    fScanner->parse();
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    f_समुच्चयः_Builder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    f_Forward_सारणी = नूतन RBBITableBuilder(एतत्, &f_Forward_वृक्षः, स्थितिः);
    यदि (f_Forward_सारणी == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    f_Forward_सारणी->build_Forward_सारणी();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    f_Forward_सारणी->build_Safe_Reverse_सारणी(स्थितिः);


#यदि_परिभाषितम् RBBI_DEBUG
    यदि (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
        f_Forward_सारणी->print_अवस्थाः();
        f_Forward_सारणी->print_नियमः_स्थितिः_सारणी();
        f_Forward_सारणी->print_Reverse_सारणी();
    }
#यदि_अन्तः

    //    Generate the mapping tables (TRIE) from input code points to
    //    the character categories.
    //
    f_समुच्चयः_Builder->buildTrie();

    //
    //   Package up the compiled data into a memory image
    //      in the run-time format.
    //
    RBBIData_शीर्षकम् *दत्त = flatten_दत्त(); // returns nullptr if error
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम दत्त;
}

शून्य RBBIRuleBuilder::optimizeTables() {
    तार्किक didSomething;
    f_Forward_सारणी->minimizeLookaheads();
    कुरु {
        didSomething = असत्य;

        // Begin looking for duplicates with char class 3.
        // Class 0 is unused and should not have other categories merged into
        // it. We allow merging categories *into* class 1 {eof} (but not the
        // reverse, so that eof remains in class 1).
        IntPair duplPair = {1, 0};
        यावत् (f_Forward_सारणी->findDuplCharClassFrom(&duplPair)) {
            f_समुच्चयः_Builder->mergeCategories(duplPair);
            f_Forward_सारणी->removeColumn(duplPair.second);
            didSomething = सत्य;
        }

        यावत् (f_Forward_सारणी->remove_Duplicate_अवस्थाः() > 0) {
            didSomething = सत्य;
        }
    } यावत् (didSomething);
}

U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */
