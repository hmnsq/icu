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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/rbbi.h"
#అన్తర్భావ "unicode/ubrk.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/uchriter.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/parsepos.h"
#అన్తర్భావ "unicode/parseerr.h"

#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "rbbirb.h"
#అన్తర్భావ "rbbinode.h"
#అన్తర్భావ "rbbiscan.h"
#అన్తర్భావ "rbbisetb.h"
#అన్తర్భావ "rbbitblb.h"
#అన్తర్భావ "rbbidata.h"
#అన్తర్భావ "uassert.h"


U_NAMESPACE_BEGIN


//----------------------------------------------------------------------------------------
//
//  Constructor.
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::RBBIRuleBuilder(స్థిరరాశి UnicodeString   &నియమాలు,
                                       UParseError     *parseErr,
                                       UError_సంకేతం      &స్థితి_2)
 : f_నియమాలు(నియమాలు), f_Stripped_నియమాలు(నియమాలు)
{
    f_స్థితి = &స్థితి_2; // status is checked below
    fParseError = parseErr;
    fDebugEnv   = శూన్యసూచిక;
#నిర్వచితమైతే RBBI_DEBUG
    fDebugEnv   = getenv("U_RBBIDEBUG");
#అయితే_ముగింపు


    f_Forward_వృక్షం        = శూన్యసూచిక;
    f_తిరగేయి_వృక్షం        = శూన్యసూచిక;
    f_సురక్షిత_Fwd_వృక్షం        = శూన్యసూచిక;
    f_సురక్షిత_Rev_వృక్షం        = శూన్యసూచిక;
    f_Default_వృక్షం        = &f_Forward_వృక్షం;
    f_Forward_పట్టిక       = శూన్యసూచిక;
    f_నియమం_స్థితి_Vals     = శూన్యసూచిక;
    f_Chain_నియమాలు         = అబద్ధం;
    fLookAheadHardBreak = అబద్ధం;
    fUSetNodes          = శూన్యసూచిక;
    f_నియమం_స్థితి_Vals     = శూన్యసూచిక;
    fScanner            = శూన్యసూచిక;
    f_సమితి_Builder         = శూన్యసూచిక;
    ఒకవేళ (parseErr) {
        uprv_memset(parseErr, 0, పరిమాణం(UParseError));
    }

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    fUSetNodes          = కొత్త UVector(స్థితి_2); // bcos status gets overwritten here
    f_నియమం_స్థితి_Vals     = కొత్త UVector(స్థితి_2);
    fScanner            = కొత్త RBBIRuleScanner(ఇది);
    f_సమితి_Builder         = కొత్త RBBISetBuilder(ఇది);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (f_సమితి_Builder == శూన్యసూచిక || fScanner == శూన్యసూచిక ||
        fUSetNodes == శూన్యసూచిక || f_నియమం_స్థితి_Vals == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}



//----------------------------------------------------------------------------------------
//
//  Destructor
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::~RBBIRuleBuilder() {

    పూర్ణాంకం        i;
    కొరకు (i=0; ; i++) {
        RBBINode* n = static_cast<RBBINode*>(fUSetNodes->elementAt(i));
        ఒకవేళ (n==శూన్యసూచిక) {
            ఆపు;
        }
        తొలగించు n;
    }

    తొలగించు fUSetNodes;
    తొలగించు f_సమితి_Builder;
    తొలగించు f_Forward_పట్టిక;
    తొలగించు f_Forward_వృక్షం;
    తొలగించు f_తిరగేయి_వృక్షం;
    తొలగించు f_సురక్షిత_Fwd_వృక్షం;
    తొలగించు f_సురక్షిత_Rev_వృక్షం;
    తొలగించు fScanner;
    తొలగించు f_నియమం_స్థితి_Vals;
}





//----------------------------------------------------------------------------------------
//
//   flattenData() -  Collect up the compiled RBBI rule data and put it into
//                    the format for saving in ICU data files,
//                    which is also the format needed by the RBBI runtime engine.
//
//----------------------------------------------------------------------------------------
స్థిర int32_t align8(int32_t i) {తిరిగి (i+7) & 0xfffffff8;}

RBBIData_శీర్షిక *RBBIRuleBuilder::flatten_దత్తాంశం() {
    int32_t    i;

    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి శూన్యసూచిక;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    f_Stripped_నియమాలు = fScanner->strip_నియమాలు(f_Stripped_నియమాలు);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t శీర్షిక_కొలత        = align8(పరిమాణం(RBBIData_శీర్షిక));
    int32_t forward_పట్టిక_కొలత  = align8(f_Forward_పట్టిక->get_పట్టిక_కొలత());
    int32_t తిరగేయి_పట్టిక_కొలత  = align8(f_Forward_పట్టిక->get_సురక్షిత_పట్టిక_కొలత());
    int32_t trie_కొలత          = align8(f_సమితి_Builder->get_Trie_కొలత());
    int32_t స్థితి_పట్టిక_కొలత   = align8(f_నియమం_స్థితి_Vals->size() * పరిమాణం(int32_t));

    int32_t నియమాలు_నిడివి_లోపల_UTF8 = 0;
    u_str_గమ్యం_UTF8_With_Sub(శూన్యసూచిక, 0, &నియమాలు_నిడివి_లోపల_UTF8,
                       f_Stripped_నియమాలు.get_బఫరు(), f_Stripped_నియమాలు.length(),
                       0xfffd, శూన్యసూచిక, f_స్థితి);
    *f_స్థితి = U_ZERO_ERROR;

    int32_t నియమాలు_కొలత         = align8((నియమాలు_నిడివి_లోపల_UTF8+1));

    int32_t         మొత్తం_కొలత = శీర్షిక_కొలత
                                + forward_పట్టిక_కొలత
                                + తిరగేయి_పట్టిక_కొలత
                                + స్థితి_పట్టిక_కొలత + trie_కొలత + నియమాలు_కొలత;

#నిర్వచితమైతే RBBI_DEBUG
    ఒకవేళ (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
        RBBIDebugPrintf("Header Size:        %8d\n", శీర్షిక_కొలత);
        RBBIDebugPrintf("Forward Table Size: %8d\n", forward_పట్టిక_కొలత);
        RBBIDebugPrintf("Reverse Table Size: %8d\n", తిరగేయి_పట్టిక_కొలత);
        RBBIDebugPrintf("Trie Size:          %8d\n", trie_కొలత);
        RBBIDebugPrintf("Status Table Size:  %8d\n", స్థితి_పట్టిక_కొలత);
        RBBIDebugPrintf("Rules Size:         %8d\n", నియమాలు_కొలత);
        RBBIDebugPrintf("-----------------------------\n");
        RBBIDebugPrintf("Total Size:         %8d\n", మొత్తం_కొలత);
    }
#అయితే_ముగింపు

    Local_జ్ఞాపకం<RBBIData_శీర్షిక> దత్తాంశం(static_cast<RBBIData_శీర్షిక*>(uprv_malloc(మొత్తం_కొలత)));
    ఒకవేళ (దత్తాంశం.isNull()) {
        *f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    uprv_memset(దత్తాంశం.getAlias(), 0, మొత్తం_కొలత);


    దత్తాంశం->fMagic            = 0xb1a0;
    దత్తాంశం->f_Format_సంస్కరణ[0] = RBBI_దత్తాంశం_FORMAT_సంస్కరణ[0];
    దత్తాంశం->f_Format_సంస్కరణ[1] = RBBI_దత్తాంశం_FORMAT_సంస్కరణ[1];
    దత్తాంశం->f_Format_సంస్కరణ[2] = RBBI_దత్తాంశం_FORMAT_సంస్కరణ[2];
    దత్తాంశం->f_Format_సంస్కరణ[3] = RBBI_దత్తాంశం_FORMAT_సంస్కరణ[3];
    దత్తాంశం->f_నిడివి           = మొత్తం_కొలత;
    దత్తాంశం->f_Cat_లెక్క         = f_సమితి_Builder->get_అంకె_Char_Categories();

    దత్తాంశం->fFTable        = శీర్షిక_కొలత;
    దత్తాంశం->fFTableLen     = forward_పట్టిక_కొలత;

    దత్తాంశం->fRTable        = దత్తాంశం->fFTable  + దత్తాంశం->fFTableLen;
    దత్తాంశం->fRTableLen     = తిరగేయి_పట్టిక_కొలత;

    దత్తాంశం->fTrie          = దత్తాంశం->fRTable + దత్తాంశం->fRTableLen;
    దత్తాంశం->fTrieLen       = trie_కొలత;
    దత్తాంశం->f_స్థితి_పట్టిక   = దత్తాంశం->fTrie    + దత్తాంశం->fTrieLen;
    దత్తాంశం->f_స్థితి_పట్టిక_Len= స్థితి_పట్టిక_కొలత;
    దత్తాంశం->f_నియమం_ఆధారం    = దత్తాంశం->f_స్థితి_పట్టిక + స్థితి_పట్టిక_కొలత;
    దత్తాంశం->f_నియమం_ఆధారం_Len = నియమాలు_నిడివి_లోపల_UTF8;

    uprv_memset(దత్తాంశం->fReserved, 0, పరిమాణం(దత్తాంశం->fReserved));

    f_Forward_పట్టిక->export_పట్టిక(reinterpret_cast<uint8_t*>(దత్తాంశం.getAlias()) + దత్తాంశం->fFTable);
    f_Forward_పట్టిక->export_సురక్షిత_పట్టిక(reinterpret_cast<uint8_t*>(దత్తాంశం.getAlias()) + దత్తాంశం->fRTable);
    f_సమితి_Builder->serializeTrie(reinterpret_cast<uint8_t*>(దత్తాంశం.getAlias()) + దత్తాంశం->fTrie);

    int32_t* నియమం_స్థితి_పట్టిక = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(దత్తాంశం.getAlias()) + దత్తాంశం->f_స్థితి_పట్టిక);
    కొరకు (i=0; i<f_నియమం_స్థితి_Vals->size(); i++) {
        నియమం_స్థితి_పట్టిక[i] = f_నియమం_స్థితి_Vals->elementAti(i);
    }

    u_str_గమ్యం_UTF8_With_Sub(reinterpret_cast<అక్షరం*>(దత్తాంశం.getAlias()) + దత్తాంశం->f_నియమం_ఆధారం, నియమాలు_కొలత, &నియమాలు_నిడివి_లోపల_UTF8,
                       f_Stripped_నియమాలు.get_బఫరు(), f_Stripped_నియమాలు.length(),
                       0xfffd, శూన్యసూచిక, f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి శూన్యసూచిక;
    }

    తిరిగి దత్తాంశం.orphan();
}


//----------------------------------------------------------------------------------------
//
//  createRuleBasedBreakIterator    construct from source rules that are passed in
//                                  in a UnicodeString
//
//----------------------------------------------------------------------------------------
BreakIterator *
RBBIRuleBuilder::create_నియమం_Based_Break_Iterator( స్థిరరాశి UnicodeString    &నియమాలు,
                                    UParseError      *parseError,
                                    UError_సంకేతం       &స్థితి_2)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(నియమాలు, parseError, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) { // status checked here bcos build below doesn't
        తిరిగి శూన్యసూచిక;
    }

    RBBIData_శీర్షిక *దత్తాంశం = builder.build(స్థితి_2);

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    నియమం_Based_Break_Iterator *This = కొత్త నియమం_Based_Break_Iterator(దత్తాంశం, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తొలగించు This;
        This = శూన్యసూచిక;
    } 
    లేకపోతే ఒకవేళ(This == శూన్యసూచిక) { // test for nullptr
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    తిరిగి This;
}

RBBIData_శీర్షిక *RBBIRuleBuilder::build(UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    fScanner->parse();
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    f_సమితి_Builder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    f_Forward_పట్టిక = కొత్త RBBITableBuilder(ఇది, &f_Forward_వృక్షం, స్థితి_2);
    ఒకవేళ (f_Forward_పట్టిక == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    f_Forward_పట్టిక->build_Forward_పట్టిక();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    f_Forward_పట్టిక->build_సురక్షిత_తిరగేయి_పట్టిక(స్థితి_2);


#నిర్వచితమైతే RBBI_DEBUG
    ఒకవేళ (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
        f_Forward_పట్టిక->print_స్థితులు();
        f_Forward_పట్టిక->print_నియమం_స్థితి_పట్టిక();
        f_Forward_పట్టిక->print_తిరగేయి_పట్టిక();
    }
#అయితే_ముగింపు

    //    Generate the mapping tables (TRIE) from input code points to
    //    the character categories.
    //
    f_సమితి_Builder->buildTrie();

    //
    //   Package up the compiled data into a memory image
    //      in the run-time format.
    //
    RBBIData_శీర్షిక *దత్తాంశం = flatten_దత్తాంశం(); // returns nullptr if error
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి దత్తాంశం;
}

శూన్య RBBIRuleBuilder::optimizeTables() {
    తార్కిక didSomething;
    f_Forward_పట్టిక->minimizeLookaheads();
    చేయి {
        didSomething = అబద్ధం;

        // Begin looking for duplicates with char class 3.
        // Class 0 is unused and should not have other categories merged into
        // it. We allow merging categories *into* class 1 {eof} (but not the
        // reverse, so that eof remains in class 1).
        IntPair duplPair = {1, 0};
        అయితే (f_Forward_పట్టిక->కనుగొను_Dupl_Char_Class_From(&duplPair)) {
            f_సమితి_Builder->విలీనం_Categories(duplPair);
            f_Forward_పట్టిక->remove_స్తంభం(duplPair.రెండవ);
            didSomething = నిజం;
        }

        అయితే (f_Forward_పట్టిక->remove_Duplicate_స్థితులు() > 0) {
            didSomething = నిజం;
        }
    } అయితే (didSomething);
}

U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */
