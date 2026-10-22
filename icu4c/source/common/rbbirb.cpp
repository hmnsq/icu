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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "unicode/brkiter.h"
#समावेश "unicode/rbbi.h"
#समावेश "unicode/ubrk.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/uchriter.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/parsepos.h"
#समावेश "unicode/parseerr.h"

#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "rbbirb.h"
#समावेश "rbbinode.h"
#समावेश "rbbiscan.h"
#समावेश "rbbisetb.h"
#समावेश "rbbitblb.h"
#समावेश "rbbidata.h"
#समावेश "uassert.h"


U_NAMESPACE_BEGIN


//----------------------------------------------------------------------------------------
//
//  Constructor.
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::RBBIRuleBuilder(स्थिर UnicodeString   &rules,
                                       UParseError     *parseErr,
                                       UErrorCode      &status)
 : fRules(rules), fStrippedRules(rules)
{
    fStatus = &status; // status is checked below
    fParseError = parseErr;
    fDebugEnv   = nullptr;
#अगर_घोषित RBBI_DEBUG
    fDebugEnv   = दो_पर्या("U_RBBIDEBUG");
#पूर्ण_अगर


    f_Forward_वृक्ष        = nullptr;
    f_Reverse_वृक्ष        = nullptr;
    f_Safe_Fwd_वृक्ष        = nullptr;
    f_Safe_Rev_वृक्ष        = nullptr;
    f_Default_वृक्ष        = &f_Forward_वृक्ष;
    fForwardTable       = nullptr;
    fRuleStatusVals     = nullptr;
    fChainRules         = false;
    fLookAheadHardBreak = false;
    fUSetNodes          = nullptr;
    fRuleStatusVals     = nullptr;
    fScanner            = nullptr;
    fSetBuilder         = nullptr;
    अगर (parseErr) {
        uprv_memset(parseErr, 0, माप(UParseError));
    }

    अगर (U_FAILURE(status)) {
        वापस;
    }

    fUSetNodes          = नया UVector(status); // bcos status gets overwritten here
    fRuleStatusVals     = नया UVector(status);
    fScanner            = नया RBBIRuleScanner(यह);
    fSetBuilder         = नया RBBISetBuilder(यह);
    अगर (U_FAILURE(status)) {
        वापस;
    }
    अगर (fSetBuilder == nullptr || fScanner == nullptr ||
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

    पूर्णांक        i;
    क्रम (i=0; ; i++) {
        RBBINode* n = static_cast<RBBINode*>(fUSetNodes->elementAt(i));
        अगर (n==nullptr) {
            अवरोध;
        }
        मिटाओ n;
    }

    मिटाओ fUSetNodes;
    मिटाओ fSetBuilder;
    मिटाओ fForwardTable;
    मिटाओ f_Forward_वृक्ष;
    मिटाओ f_Reverse_वृक्ष;
    मिटाओ f_Safe_Fwd_वृक्ष;
    मिटाओ f_Safe_Rev_वृक्ष;
    मिटाओ fScanner;
    मिटाओ fRuleStatusVals;
}





//----------------------------------------------------------------------------------------
//
//   flattenData() -  Collect up the compiled RBBI rule data and put it into
//                    the format for saving in ICU data files,
//                    which is also the format needed by the RBBI runtime engine.
//
//----------------------------------------------------------------------------------------
अटल int32_t align8(int32_t i) {वापस (i+7) & 0xfffffff8;}

RBBIDataHeader *RBBIRuleBuilder::flatten_तथ्य() {
    int32_t    i;

    अगर (U_FAILURE(*fStatus)) {
        वापस nullptr;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    fStrippedRules = fScanner->stripRules(fStrippedRules);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t header_आकार        = align8(माप(RBBIDataHeader));
    int32_t forward_Table_आकार  = align8(fForwardTable->get_Table_आकार());
    int32_t reverse_Table_आकार  = align8(fForwardTable->get_Safe_Table_आकार());
    int32_t trie_आकार          = align8(fSetBuilder->get_Trie_आकार());
    int32_t status_Table_आकार   = align8(fRuleStatusVals->size() * माप(int32_t));

    int32_t rules_लंबाई_In_UTF8 = 0;
    u_strToUTF8WithSub(nullptr, 0, &rules_लंबाई_In_UTF8,
                       fStrippedRules.get_बफर(), fStrippedRules.length(),
                       0xfffd, nullptr, fStatus);
    *fStatus = U_ZERO_ERROR;

    int32_t rules_आकार         = align8((rules_लंबाई_In_UTF8+1));

    int32_t         योग_आकार = header_आकार
                                + forward_Table_आकार
                                + reverse_Table_आकार
                                + status_Table_आकार + trie_आकार + rules_आकार;

#अगर_घोषित RBBI_DEBUG
    अगर (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
        RBBIDebugPrintf("Header Size:        %8d\n", header_आकार);
        RBBIDebugPrintf("Forward Table Size: %8d\n", forward_Table_आकार);
        RBBIDebugPrintf("Reverse Table Size: %8d\n", reverse_Table_आकार);
        RBBIDebugPrintf("Trie Size:          %8d\n", trie_आकार);
        RBBIDebugPrintf("Status Table Size:  %8d\n", status_Table_आकार);
        RBBIDebugPrintf("Rules Size:         %8d\n", rules_आकार);
        RBBIDebugPrintf("-----------------------------\n");
        RBBIDebugPrintf("Total Size:         %8d\n", योग_आकार);
    }
#पूर्ण_अगर

    LocalMemory<RBBIDataHeader> तथ्य(static_cast<RBBIDataHeader*>(uprv_malloc(योग_आकार)));
    अगर (तथ्य.isNull()) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    uprv_memset(तथ्य.getAlias(), 0, योग_आकार);


    तथ्य->fMagic            = 0xb1a0;
    तथ्य->fFormatVersion[0] = RBBI_तथ्य_FORMAT_VERSION[0];
    तथ्य->fFormatVersion[1] = RBBI_तथ्य_FORMAT_VERSION[1];
    तथ्य->fFormatVersion[2] = RBBI_तथ्य_FORMAT_VERSION[2];
    तथ्य->fFormatVersion[3] = RBBI_तथ्य_FORMAT_VERSION[3];
    तथ्य->f_लंबाई           = योग_आकार;
    तथ्य->f_Cat_गिनती         = fSetBuilder->get_अंक_Char_Categories();

    तथ्य->fFTable        = header_आकार;
    तथ्य->fFTableLen     = forward_Table_आकार;

    तथ्य->fRTable        = तथ्य->fFTable  + तथ्य->fFTableLen;
    तथ्य->fRTableLen     = reverse_Table_आकार;

    तथ्य->fTrie          = तथ्य->fRTable + तथ्य->fRTableLen;
    तथ्य->fTrieLen       = trie_आकार;
    तथ्य->fStatusTable   = तथ्य->fTrie    + तथ्य->fTrieLen;
    तथ्य->fStatusTableLen= status_Table_आकार;
    तथ्य->fRuleSource    = तथ्य->fStatusTable + status_Table_आकार;
    तथ्य->fRuleSourceLen = rules_लंबाई_In_UTF8;

    uprv_memset(तथ्य->fReserved, 0, माप(तथ्य->fReserved));

    fForwardTable->exportTable(reinterpret_cast<uint8_t*>(तथ्य.getAlias()) + तथ्य->fFTable);
    fForwardTable->exportSafeTable(reinterpret_cast<uint8_t*>(तथ्य.getAlias()) + तथ्य->fRTable);
    fSetBuilder->serializeTrie(reinterpret_cast<uint8_t*>(तथ्य.getAlias()) + तथ्य->fTrie);

    int32_t* ruleStatusTable = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(तथ्य.getAlias()) + तथ्य->fStatusTable);
    क्रम (i=0; i<fRuleStatusVals->size(); i++) {
        ruleStatusTable[i] = fRuleStatusVals->elementAti(i);
    }

    u_strToUTF8WithSub(reinterpret_cast<अक्षर*>(तथ्य.getAlias()) + तथ्य->fRuleSource, rules_आकार, &rules_लंबाई_In_UTF8,
                       fStrippedRules.get_बफर(), fStrippedRules.length(),
                       0xfffd, nullptr, fStatus);
    अगर (U_FAILURE(*fStatus)) {
        वापस nullptr;
    }

    वापस तथ्य.orphan();
}


//----------------------------------------------------------------------------------------
//
//  createRuleBasedBreakIterator    construct from source rules that are passed in
//                                  in a UnicodeString
//
//----------------------------------------------------------------------------------------
BreakIterator *
RBBIRuleBuilder::createRuleBasedBreakIterator( स्थिर UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(rules, parseError, status);
    अगर (U_FAILURE(status)) { // status checked here bcos build below doesn't
        वापस nullptr;
    }

    RBBIDataHeader *तथ्य = builder.build(status);

    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    RuleBasedBreakIterator *This = नया RuleBasedBreakIterator(तथ्य, status);
    अगर (U_FAILURE(status)) {
        मिटाओ This;
        This = nullptr;
    } 
    अन्यथा अगर(This == nullptr) { // test for nullptr
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    वापस This;
}

RBBIDataHeader *RBBIRuleBuilder::build(UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

    fScanner->parse();
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    fSetBuilder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    fForwardTable = नया RBBITableBuilder(यह, &f_Forward_वृक्ष, status);
    अगर (fForwardTable == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }

    fForwardTable->buildForwardTable();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    fForwardTable->buildSafeReverseTable(status);


#अगर_घोषित RBBI_DEBUG
    अगर (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
        fForwardTable->printStates();
        fForwardTable->printRuleStatusTable();
        fForwardTable->printReverseTable();
    }
#पूर्ण_अगर

    //    Generate the mapping tables (TRIE) from input code points to
    //    the character categories.
    //
    fSetBuilder->buildTrie();

    //
    //   Package up the compiled data into a memory image
    //      in the run-time format.
    //
    RBBIDataHeader *तथ्य = flatten_तथ्य(); // returns nullptr if error
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }
    वापस तथ्य;
}

व्योम RBBIRuleBuilder::optimizeTables() {
    bool didSomething;
    fForwardTable->minimizeLookaheads();
    करो {
        didSomething = false;

        // Begin looking for duplicates with char class 3.
        // Class 0 is unused and should not have other categories merged into
        // it. We allow merging categories *into* class 1 {eof} (but not the
        // reverse, so that eof remains in class 1).
        IntPair duplPair = {1, 0};
        जबतक (fForwardTable->findDuplCharClassFrom(&duplPair)) {
            fSetBuilder->mergeCategories(duplPair);
            fForwardTable->removeColumn(duplPair.second);
            didSomething = true;
        }

        जबतक (fForwardTable->removeDuplicateStates() > 0) {
            didSomething = true;
        }
    } जबतक (didSomething);
}

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */
