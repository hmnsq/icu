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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/brkiter.h"
#شامل "unicode/rbbi.h"
#شامل "unicode/ubrk.h"
#شامل "unicode/unistr.h"
#شامل "unicode/uniset.h"
#شامل "unicode/uchar.h"
#شامل "unicode/uchriter.h"
#شامل "unicode/ustring.h"
#شامل "unicode/parsepos.h"
#شامل "unicode/parseerr.h"

#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "rbbirb.h"
#شامل "rbbinode.h"
#شامل "rbbiscan.h"
#شامل "rbbisetb.h"
#شامل "rbbitblb.h"
#شامل "rbbidata.h"
#شامل "uassert.h"


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


    f_Forward_درخت        = nullptr;
    f_Reverse_درخت        = nullptr;
    f_Safe_Fwd_درخت        = nullptr;
    f_Safe_Rev_درخت        = nullptr;
    f_Default_درخت        = &f_Forward_درخت;
    fForwardTable       = nullptr;
    fRuleStatusVals     = nullptr;
    fChainRules         = نادرست;
    fLookAheadHardBreak = نادرست;
    fUSetNodes          = nullptr;
    fRuleStatusVals     = nullptr;
    fScanner            = nullptr;
    fSetBuilder         = nullptr;
    اگر (parseErr) {
        uprv_memset(parseErr, 0, اندازه(UParseError));
    }

    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    fUSetNodes          = جدید UVector(status); // bcos status gets overwritten here
    fRuleStatusVals     = جدید UVector(status);
    fScanner            = جدید RBBIRuleScanner(این);
    fSetBuilder         = جدید RBBISetBuilder(این);
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    اگر (fSetBuilder == nullptr || fScanner == nullptr ||
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

    صحیح        i;
    برای (i=0; ; i++) {
        RBBINode* n = static_cast<RBBINode*>(fUSetNodes->elementAt(i));
        اگر (n==nullptr) {
            بشکن;
        }
        delete n;
    }

    delete fUSetNodes;
    delete fSetBuilder;
    delete fForwardTable;
    delete f_Forward_درخت;
    delete f_Reverse_درخت;
    delete f_Safe_Fwd_درخت;
    delete f_Safe_Rev_درخت;
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
ایستا int32_t align8(int32_t i) {بازگشت (i+7) & 0xfffffff8;}

RBBIDataHeader *RBBIRuleBuilder::flattenData() {
    int32_t    i;

    اگر (U_FAILURE(*fStatus)) {
        بازگشت nullptr;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    fStrippedRules = fScanner->stripRules(fStrippedRules);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t headerSize        = align8(اندازه(RBBIDataHeader));
    int32_t forwardTableSize  = align8(fForwardTable->getTableSize());
    int32_t reverseTableSize  = align8(fForwardTable->getSafeTableSize());
    int32_t trieSize          = align8(fSetBuilder->getTrieSize());
    int32_t statusTableSize   = align8(fRuleStatusVals->size() * اندازه(int32_t));

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
    اگر (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
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
    اگر (data.isNull()) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    uprv_memset(data.getAlias(), 0, totalSize);


    data->fMagic            = 0xb1a0;
    data->fFormatVersion[0] = RBBI_DATA_FORMAT_VERSION[0];
    data->fFormatVersion[1] = RBBI_DATA_FORMAT_VERSION[1];
    data->fFormatVersion[2] = RBBI_DATA_FORMAT_VERSION[2];
    data->fFormatVersion[3] = RBBI_DATA_FORMAT_VERSION[3];
    data->fLength           = totalSize;
    data->f_Cat_شمار         = fSetBuilder->get_رقم_Char_Categories();

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

    uprv_memset(data->fReserved, 0, اندازه(data->fReserved));

    fForwardTable->exportTable(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fFTable);
    fForwardTable->exportSafeTable(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fRTable);
    fSetBuilder->serializeTrie(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fTrie);

    int32_t* ruleStatusTable = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(data.getAlias()) + data->fStatusTable);
    برای (i=0; i<fRuleStatusVals->size(); i++) {
        ruleStatusTable[i] = fRuleStatusVals->elementAti(i);
    }

    u_strToUTF8WithSub(reinterpret_cast<نویسه*>(data.getAlias()) + data->fRuleSource, rulesSize, &rulesLengthInUTF8,
                       fStrippedRules.getBuffer(), fStrippedRules.length(),
                       0xfffd, nullptr, fStatus);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت nullptr;
    }

    بازگشت data.orphan();
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
    اگر (U_FAILURE(status)) { // status checked here bcos build below doesn't
        بازگشت nullptr;
    }

    RBBIDataHeader *data = builder.build(status);

    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    RuleBasedBreakIterator *This = جدید RuleBasedBreakIterator(data, status);
    اگر (U_FAILURE(status)) {
        delete This;
        This = nullptr;
    } 
    وگرنه اگر(This == nullptr) { // test for nullptr
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    بازگشت This;
}

RBBIDataHeader *RBBIRuleBuilder::build(UErrorCode &status) {
    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }

    fScanner->parse();
    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    fSetBuilder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    fForwardTable = جدید RBBITableBuilder(این, &f_Forward_درخت, status);
    اگر (fForwardTable == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }

    fForwardTable->buildForwardTable();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    fForwardTable->buildSafeReverseTable(status);


#ifdef RBBI_DEBUG
    اگر (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
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
    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }
    بازگشت data;
}

تهی RBBIRuleBuilder::optimizeTables() {
    منطقی didSomething;
    fForwardTable->minimizeLookaheads();
    انجامبده {
        didSomething = نادرست;

        // Begin looking for duplicates with char class 3.
        // Class 0 is unused and should not have other categories merged into
        // it. We allow merging categories *into* class 1 {eof} (but not the
        // reverse, so that eof remains in class 1).
        IntPair duplPair = {1, 0};
        وقتی (fForwardTable->findDuplCharClassFrom(&duplPair)) {
            fSetBuilder->mergeCategories(duplPair);
            fForwardTable->removeColumn(duplPair.second);
            didSomething = درست;
        }

        وقتی (fForwardTable->removeDuplicateStates() > 0) {
            didSomething = درست;
        }
    } وقتی (didSomething);
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
