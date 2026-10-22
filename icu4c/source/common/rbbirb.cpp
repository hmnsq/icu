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

#ضمن "unicode/utypes.h"

#ܐܢ !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/brkiter.h"
#ضمن "unicode/rbbi.h"
#ضمن "unicode/ubrk.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/uchriter.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/parsepos.h"
#ضمن "unicode/parseerr.h"

#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "rbbirb.h"
#ضمن "rbbinode.h"
#ضمن "rbbiscan.h"
#ضمن "rbbisetb.h"
#ضمن "rbbitblb.h"
#ضمن "rbbidata.h"
#ضمن "uassert.h"


U_NAMESPACE_BEGIN


//----------------------------------------------------------------------------------------
//
//  Constructor.
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::RBBIRuleBuilder(ثابت UnicodeString   &rules,
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


    fForwardTree        = nullptr;
    fReverseTree        = nullptr;
    fSafeFwdTree        = nullptr;
    fSafeRevTree        = nullptr;
    fDefaultTree        = &fForwardTree;
    fForwardTable       = nullptr;
    fRuleStatusVals     = nullptr;
    fChainRules         = ܕܓܠܐ;
    fLookAheadHardBreak = ܕܓܠܐ;
    fUSetNodes          = nullptr;
    fRuleStatusVals     = nullptr;
    fScanner            = nullptr;
    fSetBuilder         = nullptr;
    ܐܢ (parseErr) {
        uprv_memset(parseErr, 0, حجم(UParseError));
    }

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }

    fUSetNodes          = جديد UVector(status); // bcos status gets overwritten here
    fRuleStatusVals     = جديد UVector(status);
    fScanner            = جديد RBBIRuleScanner(هذا);
    fSetBuilder         = جديد RBBISetBuilder(هذا);
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }
    ܐܢ (fSetBuilder == nullptr || fScanner == nullptr ||
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

    صحيح        i;
    لكل (i=0; ; i++) {
        RBBINode* n = static_cast<RBBINode*>(fUSetNodes->elementAt(i));
        ܐܢ (n==nullptr) {
            اكسر;
        }
        delete n;
    }

    delete fUSetNodes;
    delete fSetBuilder;
    delete fForwardTable;
    delete fForwardTree;
    delete fReverseTree;
    delete fSafeFwdTree;
    delete fSafeRevTree;
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
ساكن int32_t align8(int32_t i) {ܦܢܐ (i+7) & 0xfffffff8;}

RBBIDataHeader *RBBIRuleBuilder::flattenData() {
    int32_t    i;

    ܐܢ (U_FAILURE(*fStatus)) {
        ܦܢܐ nullptr;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    fStrippedRules = fScanner->stripRules(fStrippedRules);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t headerSize        = align8(حجم(RBBIDataHeader));
    int32_t forwardTableSize  = align8(fForwardTable->getTableSize());
    int32_t reverseTableSize  = align8(fForwardTable->getSafeTableSize());
    int32_t trieSize          = align8(fSetBuilder->getTrieSize());
    int32_t statusTableSize   = align8(fRuleStatusVals->size() * حجم(int32_t));

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
    ܐܢ (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
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
    ܐܢ (data.isNull()) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ nullptr;
    }
    uprv_memset(data.getAlias(), 0, totalSize);


    data->fMagic            = 0xb1a0;
    data->fFormatVersion[0] = RBBI_DATA_FORMAT_VERSION[0];
    data->fFormatVersion[1] = RBBI_DATA_FORMAT_VERSION[1];
    data->fFormatVersion[2] = RBBI_DATA_FORMAT_VERSION[2];
    data->fFormatVersion[3] = RBBI_DATA_FORMAT_VERSION[3];
    data->fLength           = totalSize;
    data->fCatCount         = fSetBuilder->getNumCharCategories();

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

    uprv_memset(data->fReserved, 0, حجم(data->fReserved));

    fForwardTable->exportTable(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fFTable);
    fForwardTable->exportSafeTable(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fRTable);
    fSetBuilder->serializeTrie(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fTrie);

    int32_t* ruleStatusTable = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fStatusTable);
    لكل (i=0; i<fRuleStatusVals->size(); i++) {
        ruleStatusTable[i] = fRuleStatusVals->elementAti(i);
    }

    u_strToUTF8WithSub(reinterpret_cast<حرف*>(data.getAlias()) + data->fRuleSource, rulesSize, &rulesLengthInUTF8,
                       fStrippedRules.getBuffer(), fStrippedRules.length(),
                       0xfffd, nullptr, fStatus);
    ܐܢ (U_FAILURE(*fStatus)) {
        ܦܢܐ nullptr;
    }

    ܦܢܐ data.orphan();
}


//----------------------------------------------------------------------------------------
//
//  createRuleBasedBreakIterator    construct from source rules that are passed in
//                                  in a UnicodeString
//
//----------------------------------------------------------------------------------------
BreakIterator *
RBBIRuleBuilder::createRuleBasedBreakIterator( ثابت UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(rules, parseError, status);
    ܐܢ (U_FAILURE(status)) { // status checked here bcos build below doesn't
        ܦܢܐ nullptr;
    }

    RBBIDataHeader *data = builder.build(status);

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ nullptr;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    RuleBasedBreakIterator *This = جديد RuleBasedBreakIterator(data, status);
    ܐܢ (U_FAILURE(status)) {
        delete This;
        This = nullptr;
    } 
    ܐܚܪܢܐ ܐܢ(This == nullptr) { // test for nullptr
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ܦܢܐ This;
}

RBBIDataHeader *RBBIRuleBuilder::build(UErrorCode &status) {
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ nullptr;
    }

    fScanner->parse();
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ nullptr;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    fSetBuilder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    fForwardTable = جديد RBBITableBuilder(هذا, &fForwardTree, status);
    ܐܢ (fForwardTable == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ nullptr;
    }

    fForwardTable->buildForwardTable();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    fForwardTable->buildSafeReverseTable(status);


#ifdef RBBI_DEBUG
    ܐܢ (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
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
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ nullptr;
    }
    ܦܢܐ data;
}

فراغ RBBIRuleBuilder::optimizeTables() {
    منطقي didSomething;
    fForwardTable->minimizeLookaheads();
    ܥܒܕ {
        didSomething = ܕܓܠܐ;

        // Begin looking for duplicates with char class 3.
        // Class 0 is unused and should not have other categories merged into
        // it. We allow merging categories *into* class 1 {eof} (but not the
        // reverse, so that eof remains in class 1).
        IntPair duplPair = {1, 0};
        ܟܡܐ (fForwardTable->findDuplCharClassFrom(&duplPair)) {
            fSetBuilder->mergeCategories(duplPair);
            fForwardTable->removeColumn(duplPair.second);
            didSomething = ܫܪܪܐ;
        }

        ܟܡܐ (fForwardTable->removeDuplicateStates() > 0) {
            didSomething = ܫܪܪܐ;
        }
    } ܟܡܐ (didSomething);
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
