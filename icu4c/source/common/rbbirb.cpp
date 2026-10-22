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

#إذا !UCONFIG_NO_BREAK_ITERATION

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
RBBIRuleBuilder::RBBIRuleBuilder(ثابت UnicodeString   &قواعد,
                                       UParseError     *parseErr,
                                       UError_کوډ      &حالت)
 : f_قواعد(قواعد), f_Stripped_قواعد(قواعد)
{
    f_حالت = &حالت; // status is checked below
    fParseError = parseErr;
    fDebugEnv   = تش_اشاره;
#که_تعریف_شوی RBBI_DEBUG
    fDebugEnv   = getenv("U_RBBIDEBUG");
#د_که_پای


    f_Forward_ونه        = تش_اشاره;
    f_سرچپه_ونه        = تش_اشاره;
    f_خوندي_Fwd_ونه        = تش_اشاره;
    f_خوندي_Rev_ونه        = تش_اشاره;
    f_Default_ونه        = &f_Forward_ونه;
    f_Forward_جدول       = تش_اشاره;
    f_قاعده_حالت_Vals     = تش_اشاره;
    f_Chain_قواعد         = خطأ;
    fLookAheadHardBreak = خطأ;
    fUSetNodes          = تش_اشاره;
    f_قاعده_حالت_Vals     = تش_اشاره;
    fScanner            = تش_اشاره;
    f_سېټ_Builder         = تش_اشاره;
    إذا (parseErr) {
        uprv_memset(parseErr, 0, حجم(UParseError));
    }

    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    fUSetNodes          = جديد UVector(حالت); // bcos status gets overwritten here
    f_قاعده_حالت_Vals     = جديد UVector(حالت);
    fScanner            = جديد RBBIRuleScanner(هذا);
    f_سېټ_Builder         = جديد RBBISetBuilder(هذا);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (f_سېټ_Builder == تش_اشاره || fScanner == تش_اشاره ||
        fUSetNodes == تش_اشاره || f_قاعده_حالت_Vals == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
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
        إذا (n==تش_اشاره) {
            اكسر;
        }
        ړنګول n;
    }

    ړنګول fUSetNodes;
    ړنګول f_سېټ_Builder;
    ړنګول f_Forward_جدول;
    ړنګول f_Forward_ونه;
    ړنګول f_سرچپه_ونه;
    ړنګول f_خوندي_Fwd_ونه;
    ړنګول f_خوندي_Rev_ونه;
    ړنګول fScanner;
    ړنګول f_قاعده_حالت_Vals;
}





//----------------------------------------------------------------------------------------
//
//   flattenData() -  Collect up the compiled RBBI rule data and put it into
//                    the format for saving in ICU data files,
//                    which is also the format needed by the RBBI runtime engine.
//
//----------------------------------------------------------------------------------------
ساكن int32_t align8(int32_t i) {أرجع (i+7) & 0xfffffff8;}

RBBIData_سرلیک *RBBIRuleBuilder::flatten_معلومات() {
    int32_t    i;

    إذا (U_FAILURE(*f_حالت)) {
        أرجع تش_اشاره;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    f_Stripped_قواعد = fScanner->strip_قواعد(f_Stripped_قواعد);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t سرلیک_اندازه        = align8(حجم(RBBIData_سرلیک));
    int32_t forward_جدول_اندازه  = align8(f_Forward_جدول->get_جدول_اندازه());
    int32_t سرچپه_جدول_اندازه  = align8(f_Forward_جدول->get_خوندي_جدول_اندازه());
    int32_t trie_اندازه          = align8(f_سېټ_Builder->get_Trie_اندازه());
    int32_t حالت_جدول_اندازه   = align8(f_قاعده_حالت_Vals->size() * حجم(int32_t));

    int32_t قواعد_اوږدتیا_دننه_UTF8 = 0;
    u_str_منزل_UTF8_With_Sub(تش_اشاره, 0, &قواعد_اوږدتیا_دننه_UTF8,
                       f_Stripped_قواعد.get_بفر(), f_Stripped_قواعد.length(),
                       0xfffd, تش_اشاره, f_حالت);
    *f_حالت = U_ZERO_ERROR;

    int32_t قواعد_اندازه         = align8((قواعد_اوږدتیا_دننه_UTF8+1));

    int32_t         ټول_اندازه = سرلیک_اندازه
                                + forward_جدول_اندازه
                                + سرچپه_جدول_اندازه
                                + حالت_جدول_اندازه + trie_اندازه + قواعد_اندازه;

#که_تعریف_شوی RBBI_DEBUG
    إذا (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
        RBBIDebugPrintf("Header Size:        %8d\n", سرلیک_اندازه);
        RBBIDebugPrintf("Forward Table Size: %8d\n", forward_جدول_اندازه);
        RBBIDebugPrintf("Reverse Table Size: %8d\n", سرچپه_جدول_اندازه);
        RBBIDebugPrintf("Trie Size:          %8d\n", trie_اندازه);
        RBBIDebugPrintf("Status Table Size:  %8d\n", حالت_جدول_اندازه);
        RBBIDebugPrintf("Rules Size:         %8d\n", قواعد_اندازه);
        RBBIDebugPrintf("-----------------------------\n");
        RBBIDebugPrintf("Total Size:         %8d\n", ټول_اندازه);
    }
#د_که_پای

    Local_حافظه<RBBIData_سرلیک> معلومات(static_cast<RBBIData_سرلیک*>(uprv_malloc(ټول_اندازه)));
    إذا (معلومات.isNull()) {
        *f_حالت = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    uprv_memset(معلومات.getAlias(), 0, ټول_اندازه);


    معلومات->fMagic            = 0xb1a0;
    معلومات->f_Format_نسخه[0] = RBBI_معلومات_FORMAT_نسخه[0];
    معلومات->f_Format_نسخه[1] = RBBI_معلومات_FORMAT_نسخه[1];
    معلومات->f_Format_نسخه[2] = RBBI_معلومات_FORMAT_نسخه[2];
    معلومات->f_Format_نسخه[3] = RBBI_معلومات_FORMAT_نسخه[3];
    معلومات->f_اوږدتیا           = ټول_اندازه;
    معلومات->f_Cat_شمېر         = f_سېټ_Builder->get_عدد_Char_Categories();

    معلومات->fFTable        = سرلیک_اندازه;
    معلومات->fFTableLen     = forward_جدول_اندازه;

    معلومات->fRTable        = معلومات->fFTable  + معلومات->fFTableLen;
    معلومات->fRTableLen     = سرچپه_جدول_اندازه;

    معلومات->fTrie          = معلومات->fRTable + معلومات->fRTableLen;
    معلومات->fTrieLen       = trie_اندازه;
    معلومات->f_حالت_جدول   = معلومات->fTrie    + معلومات->fTrieLen;
    معلومات->f_حالت_جدول_Len= حالت_جدول_اندازه;
    معلومات->f_قاعده_سرچینه    = معلومات->f_حالت_جدول + حالت_جدول_اندازه;
    معلومات->f_قاعده_سرچینه_Len = قواعد_اوږدتیا_دننه_UTF8;

    uprv_memset(معلومات->fReserved, 0, حجم(معلومات->fReserved));

    f_Forward_جدول->export_جدول(reinterpret_cast<uint8_t*>(معلومات.getAlias()) + معلومات->fFTable);
    f_Forward_جدول->export_خوندي_جدول(reinterpret_cast<uint8_t*>(معلومات.getAlias()) + معلومات->fRTable);
    f_سېټ_Builder->serializeTrie(reinterpret_cast<uint8_t*>(معلومات.getAlias()) + معلومات->fTrie);

    int32_t* قاعده_حالت_جدول = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(معلومات.getAlias()) + معلومات->f_حالت_جدول);
    لكل (i=0; i<f_قاعده_حالت_Vals->size(); i++) {
        قاعده_حالت_جدول[i] = f_قاعده_حالت_Vals->elementAti(i);
    }

    u_str_منزل_UTF8_With_Sub(reinterpret_cast<حرف*>(معلومات.getAlias()) + معلومات->f_قاعده_سرچینه, قواعد_اندازه, &قواعد_اوږدتیا_دننه_UTF8,
                       f_Stripped_قواعد.get_بفر(), f_Stripped_قواعد.length(),
                       0xfffd, تش_اشاره, f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع تش_اشاره;
    }

    أرجع معلومات.orphan();
}


//----------------------------------------------------------------------------------------
//
//  createRuleBasedBreakIterator    construct from source rules that are passed in
//                                  in a UnicodeString
//
//----------------------------------------------------------------------------------------
BreakIterator *
RBBIRuleBuilder::create_قاعده_Based_Break_Iterator( ثابت UnicodeString    &قواعد,
                                    UParseError      *parseError,
                                    UError_کوډ       &حالت)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(قواعد, parseError, حالت);
    إذا (U_FAILURE(حالت)) { // status checked here bcos build below doesn't
        أرجع تش_اشاره;
    }

    RBBIData_سرلیک *معلومات = builder.build(حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    قاعده_Based_Break_Iterator *This = جديد قاعده_Based_Break_Iterator(معلومات, حالت);
    إذا (U_FAILURE(حالت)) {
        ړنګول This;
        This = تش_اشاره;
    } 
    وإلا إذا(This == تش_اشاره) { // test for nullptr
        حالت = U_حافظه_ویش_ERROR;
    }
    أرجع This;
}

RBBIData_سرلیک *RBBIRuleBuilder::build(UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }

    fScanner->parse();
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    f_سېټ_Builder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    f_Forward_جدول = جديد RBBITableBuilder(هذا, &f_Forward_ونه, حالت);
    إذا (f_Forward_جدول == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }

    f_Forward_جدول->build_Forward_جدول();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    f_Forward_جدول->build_خوندي_سرچپه_جدول(حالت);


#که_تعریف_شوی RBBI_DEBUG
    إذا (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
        f_Forward_جدول->print_حالتونه();
        f_Forward_جدول->print_قاعده_حالت_جدول();
        f_Forward_جدول->print_سرچپه_جدول();
    }
#د_که_پای

    //    Generate the mapping tables (TRIE) from input code points to
    //    the character categories.
    //
    f_سېټ_Builder->buildTrie();

    //
    //   Package up the compiled data into a memory image
    //      in the run-time format.
    //
    RBBIData_سرلیک *معلومات = flatten_معلومات(); // returns nullptr if error
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }
    أرجع معلومات;
}

فراغ RBBIRuleBuilder::optimizeTables() {
    منطقي didSomething;
    f_Forward_جدول->minimizeLookaheads();
    افعل {
        didSomething = خطأ;

        // Begin looking for duplicates with char class 3.
        // Class 0 is unused and should not have other categories merged into
        // it. We allow merging categories *into* class 1 {eof} (but not the
        // reverse, so that eof remains in class 1).
        IntPair duplPair = {1, 0};
        طالما (f_Forward_جدول->پیدا_کړه_Dupl_Char_Class_From(&duplPair)) {
            f_سېټ_Builder->یوځای_Categories(duplPair);
            f_Forward_جدول->remove_ستنه(duplPair.دوهم);
            didSomething = ریښتیا;
        }

        طالما (f_Forward_جدول->remove_Duplicate_حالتونه() > 0) {
            didSomething = ریښتیا;
        }
    } طالما (didSomething);
}

U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */
