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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/rbbi.h"
#അന്തര്ഭാവ "unicode/ubrk.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/uchriter.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/parsepos.h"
#അന്തര്ഭാവ "unicode/parseerr.h"

#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "rbbirb.h"
#അന്തര്ഭാവ "rbbinode.h"
#അന്തര്ഭാവ "rbbiscan.h"
#അന്തര്ഭാവ "rbbisetb.h"
#അന്തര്ഭാവ "rbbitblb.h"
#അന്തര്ഭാവ "rbbidata.h"
#അന്തര്ഭാവ "uassert.h"


U_NAMESPACE_BEGIN


//----------------------------------------------------------------------------------------
//
//  Constructor.
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::RBBIRuleBuilder(സ്ഥിരം UnicodeString   &rules,
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


    f_Forward_വൃക്ഷം        = nullptr;
    f_Reverse_വൃക്ഷം        = nullptr;
    f_Safe_Fwd_വൃക്ഷം        = nullptr;
    f_Safe_Rev_വൃക്ഷം        = nullptr;
    f_Default_വൃക്ഷം        = &f_Forward_വൃക്ഷം;
    fForwardTable       = nullptr;
    fRuleStatusVals     = nullptr;
    fChainRules         = അസത്യം;
    fLookAheadHardBreak = അസത്യം;
    fUSetNodes          = nullptr;
    fRuleStatusVals     = nullptr;
    fScanner            = nullptr;
    fSetBuilder         = nullptr;
    എങ്കിൽ (parseErr) {
        uprv_memset(parseErr, 0, വലുപ്പം(UParseError));
    }

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    fUSetNodes          = പുതിയ UVector(status); // bcos status gets overwritten here
    fRuleStatusVals     = പുതിയ UVector(status);
    fScanner            = പുതിയ RBBIRuleScanner(ഇത്);
    fSetBuilder         = പുതിയ RBBISetBuilder(ഇത്);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    എങ്കിൽ (fSetBuilder == nullptr || fScanner == nullptr ||
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

    പൂർണ്ണസംഖ്യ        i;
    വേണ്ടി (i=0; ; i++) {
        RBBINode* n = static_cast<RBBINode*>(fUSetNodes->elementAt(i));
        എങ്കിൽ (n==nullptr) {
            നിർത്തുക;
        }
        delete n;
    }

    delete fUSetNodes;
    delete fSetBuilder;
    delete fForwardTable;
    delete f_Forward_വൃക്ഷം;
    delete f_Reverse_വൃക്ഷം;
    delete f_Safe_Fwd_വൃക്ഷം;
    delete f_Safe_Rev_വൃക്ഷം;
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
സ്ഥിതം int32_t align8(int32_t i) {തിരികെ (i+7) & 0xfffffff8;}

RBBIDataHeader *RBBIRuleBuilder::flattenData() {
    int32_t    i;

    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ nullptr;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    fStrippedRules = fScanner->stripRules(fStrippedRules);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t headerSize        = align8(വലുപ്പം(RBBIDataHeader));
    int32_t forwardTableSize  = align8(fForwardTable->getTableSize());
    int32_t reverseTableSize  = align8(fForwardTable->getSafeTableSize());
    int32_t trieSize          = align8(fSetBuilder->getTrieSize());
    int32_t statusTableSize   = align8(fRuleStatusVals->size() * വലുപ്പം(int32_t));

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
    എങ്കിൽ (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
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
    എങ്കിൽ (data.isNull()) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    uprv_memset(data.getAlias(), 0, totalSize);


    data->fMagic            = 0xb1a0;
    data->fFormatVersion[0] = RBBI_DATA_FORMAT_VERSION[0];
    data->fFormatVersion[1] = RBBI_DATA_FORMAT_VERSION[1];
    data->fFormatVersion[2] = RBBI_DATA_FORMAT_VERSION[2];
    data->fFormatVersion[3] = RBBI_DATA_FORMAT_VERSION[3];
    data->fLength           = totalSize;
    data->f_Cat_എണ്ണം         = fSetBuilder->get_അക്കം_Char_Categories();

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

    uprv_memset(data->fReserved, 0, വലുപ്പം(data->fReserved));

    fForwardTable->exportTable(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fFTable);
    fForwardTable->exportSafeTable(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fRTable);
    fSetBuilder->serializeTrie(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fTrie);

    int32_t* ruleStatusTable = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fStatusTable);
    വേണ്ടി (i=0; i<fRuleStatusVals->size(); i++) {
        ruleStatusTable[i] = fRuleStatusVals->elementAti(i);
    }

    u_strToUTF8WithSub(reinterpret_cast<അക്ഷരം*>(data.getAlias()) + data->fRuleSource, rulesSize, &rulesLengthInUTF8,
                       fStrippedRules.getBuffer(), fStrippedRules.length(),
                       0xfffd, nullptr, fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ nullptr;
    }

    തിരികെ data.orphan();
}


//----------------------------------------------------------------------------------------
//
//  createRuleBasedBreakIterator    construct from source rules that are passed in
//                                  in a UnicodeString
//
//----------------------------------------------------------------------------------------
BreakIterator *
RBBIRuleBuilder::createRuleBasedBreakIterator( സ്ഥിരം UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(rules, parseError, status);
    എങ്കിൽ (U_FAILURE(status)) { // status checked here bcos build below doesn't
        തിരികെ nullptr;
    }

    RBBIDataHeader *data = builder.build(status);

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    RuleBasedBreakIterator *This = പുതിയ RuleBasedBreakIterator(data, status);
    എങ്കിൽ (U_FAILURE(status)) {
        delete This;
        This = nullptr;
    } 
    അല്ലെങ്കിൽ എങ്കിൽ(This == nullptr) { // test for nullptr
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    തിരികെ This;
}

RBBIDataHeader *RBBIRuleBuilder::build(UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

    fScanner->parse();
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    fSetBuilder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    fForwardTable = പുതിയ RBBITableBuilder(ഇത്, &f_Forward_വൃക്ഷം, status);
    എങ്കിൽ (fForwardTable == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }

    fForwardTable->buildForwardTable();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    fForwardTable->buildSafeReverseTable(status);


#ifdef RBBI_DEBUG
    എങ്കിൽ (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
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
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    തിരികെ data;
}

ശൂന്യം RBBIRuleBuilder::optimizeTables() {
    തർക്കം didSomething;
    fForwardTable->minimizeLookaheads();
    ചെയ്യുക {
        didSomething = അസത്യം;

        // Begin looking for duplicates with char class 3.
        // Class 0 is unused and should not have other categories merged into
        // it. We allow merging categories *into* class 1 {eof} (but not the
        // reverse, so that eof remains in class 1).
        IntPair duplPair = {1, 0};
        ആകുമ്പോൾ (fForwardTable->findDuplCharClassFrom(&duplPair)) {
            fSetBuilder->mergeCategories(duplPair);
            fForwardTable->removeColumn(duplPair.second);
            didSomething = സത്യം;
        }

        ആകുമ്പോൾ (fForwardTable->removeDuplicateStates() > 0) {
            didSomething = സത്യം;
        }
    } ആകുമ്പോൾ (didSomething);
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
