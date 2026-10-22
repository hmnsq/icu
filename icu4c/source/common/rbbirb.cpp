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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব "unicode/brkiter.h"
#অন্তর্ভাব "unicode/rbbi.h"
#অন্তর্ভাব "unicode/ubrk.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "unicode/uchriter.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/parsepos.h"
#অন্তর্ভাব "unicode/parseerr.h"

#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "rbbirb.h"
#অন্তর্ভাব "rbbinode.h"
#অন্তর্ভাব "rbbiscan.h"
#অন্তর্ভাব "rbbisetb.h"
#অন্তর্ভাব "rbbitblb.h"
#অন্তর্ভাব "rbbidata.h"
#অন্তর্ভাব "uassert.h"


U_NAMESPACE_BEGIN


//----------------------------------------------------------------------------------------
//
//  Constructor.
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::RBBIRuleBuilder(ধ্রুবক UnicodeString   &rules,
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


    f_Forward_বৃক্ষ        = nullptr;
    f_Reverse_বৃক্ষ        = nullptr;
    f_Safe_Fwd_বৃক্ষ        = nullptr;
    f_Safe_Rev_বৃক্ষ        = nullptr;
    f_Default_বৃক্ষ        = &f_Forward_বৃক্ষ;
    fForwardTable       = nullptr;
    fRuleStatusVals     = nullptr;
    fChainRules         = মিথ্যা;
    fLookAheadHardBreak = মিথ্যা;
    fUSetNodes          = nullptr;
    fRuleStatusVals     = nullptr;
    fScanner            = nullptr;
    fSetBuilder         = nullptr;
    যদি (parseErr) {
        uprv_memset(parseErr, 0, আকার(UParseError));
    }

    যদি (U_FAILURE(status)) {
        ফেরত;
    }

    fUSetNodes          = নতুন UVector(status); // bcos status gets overwritten here
    fRuleStatusVals     = নতুন UVector(status);
    fScanner            = নতুন RBBIRuleScanner(এই);
    fSetBuilder         = নতুন RBBISetBuilder(এই);
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    যদি (fSetBuilder == nullptr || fScanner == nullptr ||
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

    পূর্ণসংখ্যা        i;
    জন্য (i=0; ; i++) {
        RBBINode* n = static_cast<RBBINode*>(fUSetNodes->elementAt(i));
        যদি (n==nullptr) {
            বিরতি;
        }
        delete n;
    }

    delete fUSetNodes;
    delete fSetBuilder;
    delete fForwardTable;
    delete f_Forward_বৃক্ষ;
    delete f_Reverse_বৃক্ষ;
    delete f_Safe_Fwd_বৃক্ষ;
    delete f_Safe_Rev_বৃক্ষ;
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
স্থির int32_t align8(int32_t i) {ফেরত (i+7) & 0xfffffff8;}

RBBIDataHeader *RBBIRuleBuilder::flatten_তথ্য() {
    int32_t    i;

    যদি (U_FAILURE(*fStatus)) {
        ফেরত nullptr;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    fStrippedRules = fScanner->stripRules(fStrippedRules);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t header_আকার        = align8(আকার(RBBIDataHeader));
    int32_t forward_Table_আকার  = align8(fForwardTable->get_Table_আকার());
    int32_t reverse_Table_আকার  = align8(fForwardTable->get_Safe_Table_আকার());
    int32_t trie_আকার          = align8(fSetBuilder->get_Trie_আকার());
    int32_t status_Table_আকার   = align8(fRuleStatusVals->size() * আকার(int32_t));

    int32_t rules_দৈর্ঘ্য_In_UTF8 = 0;
    u_strToUTF8WithSub(nullptr, 0, &rules_দৈর্ঘ্য_In_UTF8,
                       fStrippedRules.get_বাফার(), fStrippedRules.length(),
                       0xfffd, nullptr, fStatus);
    *fStatus = U_ZERO_ERROR;

    int32_t rules_আকার         = align8((rules_দৈর্ঘ্য_In_UTF8+1));

    int32_t         যোগ_আকার = header_আকার
                                + forward_Table_আকার
                                + reverse_Table_আকার
                                + status_Table_আকার + trie_আকার + rules_আকার;

#ifdef RBBI_DEBUG
    যদি (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
        RBBIDebugPrintf("Header Size:        %8d\n", header_আকার);
        RBBIDebugPrintf("Forward Table Size: %8d\n", forward_Table_আকার);
        RBBIDebugPrintf("Reverse Table Size: %8d\n", reverse_Table_আকার);
        RBBIDebugPrintf("Trie Size:          %8d\n", trie_আকার);
        RBBIDebugPrintf("Status Table Size:  %8d\n", status_Table_আকার);
        RBBIDebugPrintf("Rules Size:         %8d\n", rules_আকার);
        RBBIDebugPrintf("-----------------------------\n");
        RBBIDebugPrintf("Total Size:         %8d\n", যোগ_আকার);
    }
#endif

    LocalMemory<RBBIDataHeader> তথ্য(static_cast<RBBIDataHeader*>(uprv_malloc(যোগ_আকার)));
    যদি (তথ্য.isNull()) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    uprv_memset(তথ্য.getAlias(), 0, যোগ_আকার);


    তথ্য->fMagic            = 0xb1a0;
    তথ্য->fFormatVersion[0] = RBBI_তথ্য_FORMAT_VERSION[0];
    তথ্য->fFormatVersion[1] = RBBI_তথ্য_FORMAT_VERSION[1];
    তথ্য->fFormatVersion[2] = RBBI_তথ্য_FORMAT_VERSION[2];
    তথ্য->fFormatVersion[3] = RBBI_তথ্য_FORMAT_VERSION[3];
    তথ্য->f_দৈর্ঘ্য           = যোগ_আকার;
    তথ্য->f_Cat_গুনতি         = fSetBuilder->get_অঙ্ক_Char_Categories();

    তথ্য->fFTable        = header_আকার;
    তথ্য->fFTableLen     = forward_Table_আকার;

    তথ্য->fRTable        = তথ্য->fFTable  + তথ্য->fFTableLen;
    তথ্য->fRTableLen     = reverse_Table_আকার;

    তথ্য->fTrie          = তথ্য->fRTable + তথ্য->fRTableLen;
    তথ্য->fTrieLen       = trie_আকার;
    তথ্য->fStatusTable   = তথ্য->fTrie    + তথ্য->fTrieLen;
    তথ্য->fStatusTableLen= status_Table_আকার;
    তথ্য->fRuleSource    = তথ্য->fStatusTable + status_Table_আকার;
    তথ্য->fRuleSourceLen = rules_দৈর্ঘ্য_In_UTF8;

    uprv_memset(তথ্য->fReserved, 0, আকার(তথ্য->fReserved));

    fForwardTable->exportTable(reinterpret_cast<uint8_t*>(তথ্য.getAlias()) + তথ্য->fFTable);
    fForwardTable->exportSafeTable(reinterpret_cast<uint8_t*>(তথ্য.getAlias()) + তথ্য->fRTable);
    fSetBuilder->serializeTrie(reinterpret_cast<uint8_t*>(তথ্য.getAlias()) + তথ্য->fTrie);

    int32_t* ruleStatusTable = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(তথ্য.getAlias()) + তথ্য->fStatusTable);
    জন্য (i=0; i<fRuleStatusVals->size(); i++) {
        ruleStatusTable[i] = fRuleStatusVals->elementAti(i);
    }

    u_strToUTF8WithSub(reinterpret_cast<অক্ষর*>(তথ্য.getAlias()) + তথ্য->fRuleSource, rules_আকার, &rules_দৈর্ঘ্য_In_UTF8,
                       fStrippedRules.get_বাফার(), fStrippedRules.length(),
                       0xfffd, nullptr, fStatus);
    যদি (U_FAILURE(*fStatus)) {
        ফেরত nullptr;
    }

    ফেরত তথ্য.orphan();
}


//----------------------------------------------------------------------------------------
//
//  createRuleBasedBreakIterator    construct from source rules that are passed in
//                                  in a UnicodeString
//
//----------------------------------------------------------------------------------------
BreakIterator *
RBBIRuleBuilder::createRuleBasedBreakIterator( ধ্রুবক UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(rules, parseError, status);
    যদি (U_FAILURE(status)) { // status checked here bcos build below doesn't
        ফেরত nullptr;
    }

    RBBIDataHeader *তথ্য = builder.build(status);

    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    RuleBasedBreakIterator *This = নতুন RuleBasedBreakIterator(তথ্য, status);
    যদি (U_FAILURE(status)) {
        delete This;
        This = nullptr;
    } 
    অন্যথা যদি(This == nullptr) { // test for nullptr
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ফেরত This;
}

RBBIDataHeader *RBBIRuleBuilder::build(UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }

    fScanner->parse();
    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    fSetBuilder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    fForwardTable = নতুন RBBITableBuilder(এই, &f_Forward_বৃক্ষ, status);
    যদি (fForwardTable == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }

    fForwardTable->buildForwardTable();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    fForwardTable->buildSafeReverseTable(status);


#ifdef RBBI_DEBUG
    যদি (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
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
    RBBIDataHeader *তথ্য = flatten_তথ্য(); // returns nullptr if error
    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }
    ফেরত তথ্য;
}

শূন্য RBBIRuleBuilder::optimizeTables() {
    যৌক্তিক didSomething;
    fForwardTable->minimizeLookaheads();
    করো {
        didSomething = মিথ্যা;

        // Begin looking for duplicates with char class 3.
        // Class 0 is unused and should not have other categories merged into
        // it. We allow merging categories *into* class 1 {eof} (but not the
        // reverse, so that eof remains in class 1).
        IntPair duplPair = {1, 0};
        যখন (fForwardTable->findDuplCharClassFrom(&duplPair)) {
            fSetBuilder->mergeCategories(duplPair);
            fForwardTable->removeColumn(duplPair.second);
            didSomething = সত্য;
        }

        যখন (fForwardTable->removeDuplicateStates() > 0) {
            didSomething = সত্য;
        }
    } যখন (didSomething);
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
