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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/rbbi.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubrk.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchriter.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"
#ಅನ್ತರ್ಭಾವ "unicode/parseerr.h"

#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "rbbirb.h"
#ಅನ್ತರ್ಭಾವ "rbbinode.h"
#ಅನ್ತರ್ಭಾವ "rbbiscan.h"
#ಅನ್ತರ್ಭಾವ "rbbisetb.h"
#ಅನ್ತರ್ಭಾವ "rbbitblb.h"
#ಅನ್ತರ್ಭಾವ "rbbidata.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"


U_NAMESPACE_BEGIN


//----------------------------------------------------------------------------------------
//
//  Constructor.
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::RBBIRuleBuilder(ಧ್ರುವ UnicodeString   &rules,
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


    f_Forward_ವೃಕ್ಷ        = nullptr;
    f_Reverse_ವೃಕ್ಷ        = nullptr;
    f_Safe_Fwd_ವೃಕ್ಷ        = nullptr;
    f_Safe_Rev_ವೃಕ್ಷ        = nullptr;
    f_Default_ವೃಕ್ಷ        = &f_Forward_ವೃಕ್ಷ;
    fForwardTable       = nullptr;
    fRuleStatusVals     = nullptr;
    fChainRules         = ಅಸತ್ಯ;
    fLookAheadHardBreak = ಅಸತ್ಯ;
    fUSetNodes          = nullptr;
    fRuleStatusVals     = nullptr;
    fScanner            = nullptr;
    fSetBuilder         = nullptr;
    ಯದಿ (parseErr) {
        uprv_memset(parseErr, 0, ಪರಿಮಾಣ(UParseError));
    }

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    fUSetNodes          = ನೂತನ UVector(status); // bcos status gets overwritten here
    fRuleStatusVals     = ನೂತನ UVector(status);
    fScanner            = ನೂತನ RBBIRuleScanner(ಏತತ್);
    fSetBuilder         = ನೂತನ RBBISetBuilder(ಏತತ್);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (fSetBuilder == nullptr || fScanner == nullptr ||
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

    ಪೂರ್ಣಾಙ್ಕ        i;
    ಪ್ರತಿ (i=0; ; i++) {
        RBBINode* n = static_cast<RBBINode*>(fUSetNodes->elementAt(i));
        ಯದಿ (n==nullptr) {
            ಭಙ್ಗ;
        }
        delete n;
    }

    delete fUSetNodes;
    delete fSetBuilder;
    delete fForwardTable;
    delete f_Forward_ವೃಕ್ಷ;
    delete f_Reverse_ವೃಕ್ಷ;
    delete f_Safe_Fwd_ವೃಕ್ಷ;
    delete f_Safe_Rev_ವೃಕ್ಷ;
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
ಸ್ಥಿರ int32_t align8(int32_t i) {ಪ್ರತ್ಯಾಗಮ (i+7) & 0xfffffff8;}

RBBIDataHeader *RBBIRuleBuilder::flattenData() {
    int32_t    i;

    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    fStrippedRules = fScanner->stripRules(fStrippedRules);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t headerSize        = align8(ಪರಿಮಾಣ(RBBIDataHeader));
    int32_t forwardTableSize  = align8(fForwardTable->getTableSize());
    int32_t reverseTableSize  = align8(fForwardTable->getSafeTableSize());
    int32_t trieSize          = align8(fSetBuilder->getTrieSize());
    int32_t statusTableSize   = align8(fRuleStatusVals->size() * ಪರಿಮಾಣ(int32_t));

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
    ಯದಿ (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
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
    ಯದಿ (data.isNull()) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    uprv_memset(data.getAlias(), 0, totalSize);


    data->fMagic            = 0xb1a0;
    data->fFormatVersion[0] = RBBI_DATA_FORMAT_VERSION[0];
    data->fFormatVersion[1] = RBBI_DATA_FORMAT_VERSION[1];
    data->fFormatVersion[2] = RBBI_DATA_FORMAT_VERSION[2];
    data->fFormatVersion[3] = RBBI_DATA_FORMAT_VERSION[3];
    data->fLength           = totalSize;
    data->f_Cat_ಎಣಿಕೆ         = fSetBuilder->get_ಅಂಕೆ_Char_Categories();

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

    uprv_memset(data->fReserved, 0, ಪರಿಮಾಣ(data->fReserved));

    fForwardTable->exportTable(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fFTable);
    fForwardTable->exportSafeTable(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fRTable);
    fSetBuilder->serializeTrie(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fTrie);

    int32_t* ruleStatusTable = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fStatusTable);
    ಪ್ರತಿ (i=0; i<fRuleStatusVals->size(); i++) {
        ruleStatusTable[i] = fRuleStatusVals->elementAti(i);
    }

    u_strToUTF8WithSub(reinterpret_cast<ವರ್ಣ*>(data.getAlias()) + data->fRuleSource, rulesSize, &rulesLengthInUTF8,
                       fStrippedRules.getBuffer(), fStrippedRules.length(),
                       0xfffd, nullptr, fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಪ್ರತ್ಯಾಗಮ data.orphan();
}


//----------------------------------------------------------------------------------------
//
//  createRuleBasedBreakIterator    construct from source rules that are passed in
//                                  in a UnicodeString
//
//----------------------------------------------------------------------------------------
BreakIterator *
RBBIRuleBuilder::createRuleBasedBreakIterator( ಧ್ರುವ UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(rules, parseError, status);
    ಯದಿ (U_FAILURE(status)) { // status checked here bcos build below doesn't
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    RBBIDataHeader *data = builder.build(status);

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    RuleBasedBreakIterator *This = ನೂತನ RuleBasedBreakIterator(data, status);
    ಯದಿ (U_FAILURE(status)) {
        delete This;
        This = nullptr;
    } 
    ಅನ್ಯಥಾ ಯದಿ(This == nullptr) { // test for nullptr
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ This;
}

RBBIDataHeader *RBBIRuleBuilder::build(UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    fScanner->parse();
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    fSetBuilder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    fForwardTable = ನೂತನ RBBITableBuilder(ಏತತ್, &f_Forward_ವೃಕ್ಷ, status);
    ಯದಿ (fForwardTable == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    fForwardTable->buildForwardTable();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    fForwardTable->buildSafeReverseTable(status);


#ifdef RBBI_DEBUG
    ಯದಿ (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
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
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ data;
}

ಶೂನ್ಯ RBBIRuleBuilder::optimizeTables() {
    ತಾರ್ಕಿಕ didSomething;
    fForwardTable->minimizeLookaheads();
    ಕುರು {
        didSomething = ಅಸತ್ಯ;

        // Begin looking for duplicates with char class 3.
        // Class 0 is unused and should not have other categories merged into
        // it. We allow merging categories *into* class 1 {eof} (but not the
        // reverse, so that eof remains in class 1).
        IntPair duplPair = {1, 0};
        ಯಾವತ್ (fForwardTable->findDuplCharClassFrom(&duplPair)) {
            fSetBuilder->mergeCategories(duplPair);
            fForwardTable->removeColumn(duplPair.second);
            didSomething = ಸತ್ಯ;
        }

        ಯಾವತ್ (fForwardTable->removeDuplicateStates() > 0) {
            didSomething = ಸತ್ಯ;
        }
    } ಯಾವತ್ (didSomething);
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
