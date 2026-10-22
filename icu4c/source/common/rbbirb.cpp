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
RBBIRuleBuilder::RBBIRuleBuilder(ثابت UnicodeString   &قاعدے,
                                       UParseError     *parseErr,
                                       UError_کوڈ      &حالت)
 : f_قاعدے(قاعدے), f_Stripped_قاعدے(قاعدے)
{
    f_حالت = &حالت; // status is checked below
    fParseError = parseErr;
    fDebugEnv   = خالی_سوچک;
#جے_تعریف_شدہ RBBI_DEBUG
    fDebugEnv   = getenv("U_RBBIDEBUG");
#جے_انت


    f_Forward_درخت        = خالی_سوچک;
    f_الٹ_درخت        = خالی_سوچک;
    f_محفوظ_Fwd_درخت        = خالی_سوچک;
    f_محفوظ_Rev_درخت        = خالی_سوچک;
    f_Default_درخت        = &f_Forward_درخت;
    f_Forward_جدول       = خالی_سوچک;
    f_قاعدہ_حالت_Vals     = خالی_سوچک;
    f_Chain_قاعدے         = خطأ;
    fLookAheadHardBreak = خطأ;
    fUSetNodes          = خالی_سوچک;
    f_قاعدہ_حالت_Vals     = خالی_سوچک;
    fScanner            = خالی_سوچک;
    f_سیٹ_Builder         = خالی_سوچک;
    إذا (parseErr) {
        uprv_memset(parseErr, 0, حجم(UParseError));
    }

    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    fUSetNodes          = جديد UVector(حالت); // bcos status gets overwritten here
    f_قاعدہ_حالت_Vals     = جديد UVector(حالت);
    fScanner            = جديد RBBIRuleScanner(هذا);
    f_سیٹ_Builder         = جديد RBBISetBuilder(هذا);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (f_سیٹ_Builder == خالی_سوچک || fScanner == خالی_سوچک ||
        fUSetNodes == خالی_سوچک || f_قاعدہ_حالت_Vals == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
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
        إذا (n==خالی_سوچک) {
            اكسر;
        }
        مٹاؤ n;
    }

    مٹاؤ fUSetNodes;
    مٹاؤ f_سیٹ_Builder;
    مٹاؤ f_Forward_جدول;
    مٹاؤ f_Forward_درخت;
    مٹاؤ f_الٹ_درخت;
    مٹاؤ f_محفوظ_Fwd_درخت;
    مٹاؤ f_محفوظ_Rev_درخت;
    مٹاؤ fScanner;
    مٹاؤ f_قاعدہ_حالت_Vals;
}





//----------------------------------------------------------------------------------------
//
//   flattenData() -  Collect up the compiled RBBI rule data and put it into
//                    the format for saving in ICU data files,
//                    which is also the format needed by the RBBI runtime engine.
//
//----------------------------------------------------------------------------------------
ساكن int32_t align8(int32_t i) {أرجع (i+7) & 0xfffffff8;}

RBBIData_سرخی *RBBIRuleBuilder::flatten_انکڑے() {
    int32_t    i;

    إذا (U_FAILURE(*f_حالت)) {
        أرجع خالی_سوچک;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    f_Stripped_قاعدے = fScanner->strip_قاعدے(f_Stripped_قاعدے);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t سرخی_ناپ        = align8(حجم(RBBIData_سرخی));
    int32_t forward_جدول_ناپ  = align8(f_Forward_جدول->get_جدول_ناپ());
    int32_t الٹ_جدول_ناپ  = align8(f_Forward_جدول->get_محفوظ_جدول_ناپ());
    int32_t trie_ناپ          = align8(f_سیٹ_Builder->get_Trie_ناپ());
    int32_t حالت_جدول_ناپ   = align8(f_قاعدہ_حالت_Vals->size() * حجم(int32_t));

    int32_t قاعدے_لمائی_وچ_UTF8 = 0;
    u_str_منزل_UTF8_With_Sub(خالی_سوچک, 0, &قاعدے_لمائی_وچ_UTF8,
                       f_Stripped_قاعدے.get_بفر(), f_Stripped_قاعدے.length(),
                       0xfffd, خالی_سوچک, f_حالت);
    *f_حالت = U_ZERO_ERROR;

    int32_t قاعدے_ناپ         = align8((قاعدے_لمائی_وچ_UTF8+1));

    int32_t         کل_ناپ = سرخی_ناپ
                                + forward_جدول_ناپ
                                + الٹ_جدول_ناپ
                                + حالت_جدول_ناپ + trie_ناپ + قاعدے_ناپ;

#جے_تعریف_شدہ RBBI_DEBUG
    إذا (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
        RBBIDebugPrintf("Header Size:        %8d\n", سرخی_ناپ);
        RBBIDebugPrintf("Forward Table Size: %8d\n", forward_جدول_ناپ);
        RBBIDebugPrintf("Reverse Table Size: %8d\n", الٹ_جدول_ناپ);
        RBBIDebugPrintf("Trie Size:          %8d\n", trie_ناپ);
        RBBIDebugPrintf("Status Table Size:  %8d\n", حالت_جدول_ناپ);
        RBBIDebugPrintf("Rules Size:         %8d\n", قاعدے_ناپ);
        RBBIDebugPrintf("-----------------------------\n");
        RBBIDebugPrintf("Total Size:         %8d\n", کل_ناپ);
    }
#جے_انت

    Local_حافظہ<RBBIData_سرخی> انکڑے(static_cast<RBBIData_سرخی*>(uprv_malloc(کل_ناپ)));
    إذا (انکڑے.isNull()) {
        *f_حالت = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    uprv_memset(انکڑے.getAlias(), 0, کل_ناپ);


    انکڑے->fMagic            = 0xb1a0;
    انکڑے->f_Format_نسخہ[0] = RBBI_انکڑے_FORMAT_نسخہ[0];
    انکڑے->f_Format_نسخہ[1] = RBBI_انکڑے_FORMAT_نسخہ[1];
    انکڑے->f_Format_نسخہ[2] = RBBI_انکڑے_FORMAT_نسخہ[2];
    انکڑے->f_Format_نسخہ[3] = RBBI_انکڑے_FORMAT_نسخہ[3];
    انکڑے->f_لمائی           = کل_ناپ;
    انکڑے->f_Cat_گنتی         = f_سیٹ_Builder->get_عدد_Char_Categories();

    انکڑے->fFTable        = سرخی_ناپ;
    انکڑے->fFTableLen     = forward_جدول_ناپ;

    انکڑے->fRTable        = انکڑے->fFTable  + انکڑے->fFTableLen;
    انکڑے->fRTableLen     = الٹ_جدول_ناپ;

    انکڑے->fTrie          = انکڑے->fRTable + انکڑے->fRTableLen;
    انکڑے->fTrieLen       = trie_ناپ;
    انکڑے->f_حالت_جدول   = انکڑے->fTrie    + انکڑے->fTrieLen;
    انکڑے->f_حالت_جدول_Len= حالت_جدول_ناپ;
    انکڑے->f_قاعدہ_سروت    = انکڑے->f_حالت_جدول + حالت_جدول_ناپ;
    انکڑے->f_قاعدہ_سروت_Len = قاعدے_لمائی_وچ_UTF8;

    uprv_memset(انکڑے->fReserved, 0, حجم(انکڑے->fReserved));

    f_Forward_جدول->export_جدول(reinterpret_cast<uint8_t*>(انکڑے.getAlias()) + انکڑے->fFTable);
    f_Forward_جدول->export_محفوظ_جدول(reinterpret_cast<uint8_t*>(انکڑے.getAlias()) + انکڑے->fRTable);
    f_سیٹ_Builder->serializeTrie(reinterpret_cast<uint8_t*>(انکڑے.getAlias()) + انکڑے->fTrie);

    int32_t* قاعدہ_حالت_جدول = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(انکڑے.getAlias()) + انکڑے->f_حالت_جدول);
    لكل (i=0; i<f_قاعدہ_حالت_Vals->size(); i++) {
        قاعدہ_حالت_جدول[i] = f_قاعدہ_حالت_Vals->elementAti(i);
    }

    u_str_منزل_UTF8_With_Sub(reinterpret_cast<حرف*>(انکڑے.getAlias()) + انکڑے->f_قاعدہ_سروت, قاعدے_ناپ, &قاعدے_لمائی_وچ_UTF8,
                       f_Stripped_قاعدے.get_بفر(), f_Stripped_قاعدے.length(),
                       0xfffd, خالی_سوچک, f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع خالی_سوچک;
    }

    أرجع انکڑے.orphan();
}


//----------------------------------------------------------------------------------------
//
//  createRuleBasedBreakIterator    construct from source rules that are passed in
//                                  in a UnicodeString
//
//----------------------------------------------------------------------------------------
BreakIterator *
RBBIRuleBuilder::create_قاعدہ_Based_Break_Iterator( ثابت UnicodeString    &قاعدے,
                                    UParseError      *parseError,
                                    UError_کوڈ       &حالت)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(قاعدے, parseError, حالت);
    إذا (U_FAILURE(حالت)) { // status checked here bcos build below doesn't
        أرجع خالی_سوچک;
    }

    RBBIData_سرخی *انکڑے = builder.build(حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    قاعدہ_Based_Break_Iterator *This = جديد قاعدہ_Based_Break_Iterator(انکڑے, حالت);
    إذا (U_FAILURE(حالت)) {
        مٹاؤ This;
        This = خالی_سوچک;
    } 
    وإلا إذا(This == خالی_سوچک) { // test for nullptr
        حالت = U_حافظہ_ونڈ_ERROR;
    }
    أرجع This;
}

RBBIData_سرخی *RBBIRuleBuilder::build(UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

    fScanner->parse();
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    f_سیٹ_Builder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    f_Forward_جدول = جديد RBBITableBuilder(هذا, &f_Forward_درخت, حالت);
    إذا (f_Forward_جدول == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }

    f_Forward_جدول->build_Forward_جدول();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    f_Forward_جدول->build_محفوظ_الٹ_جدول(حالت);


#جے_تعریف_شدہ RBBI_DEBUG
    إذا (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
        f_Forward_جدول->print_حالتاں();
        f_Forward_جدول->print_قاعدہ_حالت_جدول();
        f_Forward_جدول->print_الٹ_جدول();
    }
#جے_انت

    //    Generate the mapping tables (TRIE) from input code points to
    //    the character categories.
    //
    f_سیٹ_Builder->buildTrie();

    //
    //   Package up the compiled data into a memory image
    //      in the run-time format.
    //
    RBBIData_سرخی *انکڑے = flatten_انکڑے(); // returns nullptr if error
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }
    أرجع انکڑے;
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
        طالما (f_Forward_جدول->لبھو_Dupl_Char_Class_From(&duplPair)) {
            f_سیٹ_Builder->ملاؤ_Categories(duplPair);
            f_Forward_جدول->remove_کالم(duplPair.دوجا);
            didSomething = سچ;
        }

        طالما (f_Forward_جدول->remove_Duplicate_حالتاں() > 0) {
            didSomething = سچ;
        }
    } طالما (didSomething);
}

U_NAMESPACE_سرا

#جے_انت /* #if !UCONFIG_NO_BREAK_ITERATION */
