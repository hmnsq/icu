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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "unicode/brkiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/rbbi.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ubrk.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchriter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/parsepos.h"
#ଅନ୍ତର୍ଭାଵ "unicode/parseerr.h"

#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "rbbirb.h"
#ଅନ୍ତର୍ଭାଵ "rbbinode.h"
#ଅନ୍ତର୍ଭାଵ "rbbiscan.h"
#ଅନ୍ତର୍ଭାଵ "rbbisetb.h"
#ଅନ୍ତର୍ଭାଵ "rbbitblb.h"
#ଅନ୍ତର୍ଭାଵ "rbbidata.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"


U_NAMESPACE_BEGIN


//----------------------------------------------------------------------------------------
//
//  Constructor.
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::RBBIRuleBuilder(ଅଚଳ UnicodeString   &rules,
                                       UParseError     *parseErr,
                                       UErrorCode      &status)
 : fRules(rules), fStrippedRules(rules)
{
    fStatus = &status; // status is checked below
    fParseError = parseErr;
    fDebugEnv   = nullptr;
#ifdef RBBI_DEBUG
    fDebugEnv   = getenv("U_RBBIDEBUG");
#endif


    f_Forward_ବୃକ୍ଷ        = nullptr;
    f_Reverse_ବୃକ୍ଷ        = nullptr;
    f_Safe_Fwd_ବୃକ୍ଷ        = nullptr;
    f_Safe_Rev_ବୃକ୍ଷ        = nullptr;
    f_Default_ବୃକ୍ଷ        = &f_Forward_ବୃକ୍ଷ;
    fForwardTable       = nullptr;
    fRuleStatusVals     = nullptr;
    fChainRules         = ମିଥ୍ୟା;
    fLookAheadHardBreak = ମିଥ୍ୟା;
    fUSetNodes          = nullptr;
    fRuleStatusVals     = nullptr;
    fScanner            = nullptr;
    fSetBuilder         = nullptr;
    ଯଦି (parseErr) {
        uprv_memset(parseErr, 0, ଆକାର(UParseError));
    }

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    fUSetNodes          = ନୂତନ UVector(status); // bcos status gets overwritten here
    fRuleStatusVals     = ନୂତନ UVector(status);
    fScanner            = ନୂତନ RBBIRuleScanner(ଏହି);
    fSetBuilder         = ନୂତନ RBBISetBuilder(ଏହି);
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (fSetBuilder == nullptr || fScanner == nullptr ||
        fUSetNodes == nullptr || fRuleStatusVals == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}



//----------------------------------------------------------------------------------------
//
//  Destructor
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::~RBBIRuleBuilder() {

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା        i;
    ପାଇଁ (i=0; ; i++) {
        RBBINode* n = static_cast<RBBINode*>(fUSetNodes->elementAt(i));
        ଯଦି (n==nullptr) {
            ଭାଙ୍ଗ;
        }
        delete n;
    }

    delete fUSetNodes;
    delete fSetBuilder;
    delete fForwardTable;
    delete f_Forward_ବୃକ୍ଷ;
    delete f_Reverse_ବୃକ୍ଷ;
    delete f_Safe_Fwd_ବୃକ୍ଷ;
    delete f_Safe_Rev_ବୃକ୍ଷ;
    delete fScanner;
    delete fRuleStatusVals;
}





//----------------------------------------------------------------------------------------
//
//   flattenData() -  Collect up the compiled RBBI rule data and put it into
//                    the format for saving in ICU data files,
//                    which is also the format needed by the RBBI runtime engine.
//
//----------------------------------------------------------------------------------------
ସ୍ଥିର int32_t align8(int32_t i) {ଫେରନ୍ତୁ (i+7) & 0xfffffff8;}

RBBIDataHeader *RBBIRuleBuilder::flattenData() {
    int32_t    i;

    ଯଦି (U_FAILURE(*fStatus)) {
        ଫେରନ୍ତୁ nullptr;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    fStrippedRules = fScanner->stripRules(fStrippedRules);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t headerSize        = align8(ଆକାର(RBBIDataHeader));
    int32_t forwardTableSize  = align8(fForwardTable->getTableSize());
    int32_t reverseTableSize  = align8(fForwardTable->getSafeTableSize());
    int32_t trieSize          = align8(fSetBuilder->getTrieSize());
    int32_t statusTableSize   = align8(fRuleStatusVals->size() * ଆକାର(int32_t));

    int32_t rulesLengthInUTF8 = 0;
    u_strToUTF8WithSub(nullptr, 0, &rulesLengthInUTF8,
                       fStrippedRules.getBuffer(), fStrippedRules.length(),
                       0xfffd, nullptr, fStatus);
    *fStatus = U_ZERO_ERROR;

    int32_t rulesSize         = align8((rulesLengthInUTF8+1));

    int32_t         totalSize = headerSize
                                + forwardTableSize
                                + reverseTableSize
                                + statusTableSize + trieSize + rulesSize;

#ifdef RBBI_DEBUG
    ଯଦି (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
        RBBIDebugPrintf("Header Size:        %8d\n", headerSize);
        RBBIDebugPrintf("Forward Table Size: %8d\n", forwardTableSize);
        RBBIDebugPrintf("Reverse Table Size: %8d\n", reverseTableSize);
        RBBIDebugPrintf("Trie Size:          %8d\n", trieSize);
        RBBIDebugPrintf("Status Table Size:  %8d\n", statusTableSize);
        RBBIDebugPrintf("Rules Size:         %8d\n", rulesSize);
        RBBIDebugPrintf("-----------------------------\n");
        RBBIDebugPrintf("Total Size:         %8d\n", totalSize);
    }
#endif

    LocalMemory<RBBIDataHeader> data(static_cast<RBBIDataHeader*>(uprv_malloc(totalSize)));
    ଯଦି (data.isNull()) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_memset(data.getAlias(), 0, totalSize);


    data->fMagic            = 0xb1a0;
    data->fFormatVersion[0] = RBBI_DATA_FORMAT_VERSION[0];
    data->fFormatVersion[1] = RBBI_DATA_FORMAT_VERSION[1];
    data->fFormatVersion[2] = RBBI_DATA_FORMAT_VERSION[2];
    data->fFormatVersion[3] = RBBI_DATA_FORMAT_VERSION[3];
    data->fLength           = totalSize;
    data->f_Cat_ଗଣନା         = fSetBuilder->get_ଅଙ୍କ_Char_Categories();

    data->fFTable        = headerSize;
    data->fFTableLen     = forwardTableSize;

    data->fRTable        = data->fFTable  + data->fFTableLen;
    data->fRTableLen     = reverseTableSize;

    data->fTrie          = data->fRTable + data->fRTableLen;
    data->fTrieLen       = trieSize;
    data->fStatusTable   = data->fTrie    + data->fTrieLen;
    data->fStatusTableLen= statusTableSize;
    data->fRuleSource    = data->fStatusTable + statusTableSize;
    data->fRuleSourceLen = rulesLengthInUTF8;

    uprv_memset(data->fReserved, 0, ଆକାର(data->fReserved));

    fForwardTable->exportTable(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fFTable);
    fForwardTable->exportSafeTable(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fRTable);
    fSetBuilder->serializeTrie(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fTrie);

    int32_t* ruleStatusTable = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fStatusTable);
    ପାଇଁ (i=0; i<fRuleStatusVals->size(); i++) {
        ruleStatusTable[i] = fRuleStatusVals->elementAti(i);
    }

    u_strToUTF8WithSub(reinterpret_cast<ଅକ୍ଷର*>(data.getAlias()) + data->fRuleSource, rulesSize, &rulesLengthInUTF8,
                       fStrippedRules.getBuffer(), fStrippedRules.length(),
                       0xfffd, nullptr, fStatus);
    ଯଦି (U_FAILURE(*fStatus)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ଫେରନ୍ତୁ data.orphan();
}


//----------------------------------------------------------------------------------------
//
//  createRuleBasedBreakIterator    construct from source rules that are passed in
//                                  in a UnicodeString
//
//----------------------------------------------------------------------------------------
BreakIterator *
RBBIRuleBuilder::createRuleBasedBreakIterator( ଅଚଳ UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(rules, parseError, status);
    ଯଦି (U_FAILURE(status)) { // status checked here bcos build below doesn't
        ଫେରନ୍ତୁ nullptr;
    }

    RBBIDataHeader *data = builder.build(status);

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    RuleBasedBreakIterator *This = ନୂତନ RuleBasedBreakIterator(data, status);
    ଯଦି (U_FAILURE(status)) {
        delete This;
        This = nullptr;
    } 
    ଅନ୍ୟଥା ଯଦି(This == nullptr) { // test for nullptr
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ଫେରନ୍ତୁ This;
}

RBBIDataHeader *RBBIRuleBuilder::build(UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    fScanner->parse();
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    fSetBuilder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    fForwardTable = ନୂତନ RBBITableBuilder(ଏହି, &f_Forward_ବୃକ୍ଷ, status);
    ଯଦି (fForwardTable == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    fForwardTable->buildForwardTable();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    fForwardTable->buildSafeReverseTable(status);


#ifdef RBBI_DEBUG
    ଯଦି (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
        fForwardTable->printStates();
        fForwardTable->printRuleStatusTable();
        fForwardTable->printReverseTable();
    }
#endif

    //    Generate the mapping tables (TRIE) from input code points to
    //    the character categories.
    //
    fSetBuilder->buildTrie();

    //
    //   Package up the compiled data into a memory image
    //      in the run-time format.
    //
    RBBIDataHeader *data = flattenData(); // returns nullptr if error
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ data;
}

ଶୂନ୍ୟ RBBIRuleBuilder::optimizeTables() {
    ତାର୍କିକ didSomething;
    fForwardTable->minimizeLookaheads();
    କର {
        didSomething = ମିଥ୍ୟା;

        // Begin looking for duplicates with char class 3.
        // Class 0 is unused and should not have other categories merged into
        // it. We allow merging categories *into* class 1 {eof} (but not the
        // reverse, so that eof remains in class 1).
        IntPair duplPair = {1, 0};
        ଯେତେବେଳେ (fForwardTable->findDuplCharClassFrom(&duplPair)) {
            fSetBuilder->mergeCategories(duplPair);
            fForwardTable->removeColumn(duplPair.second);
            didSomething = ସତ୍ୟ;
        }

        ଯେତେବେଳେ (fForwardTable->removeDuplicateStates() > 0) {
            didSomething = ସତ୍ୟ;
        }
    } ଯେତେବେଳେ (didSomething);
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
