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
RBBIRuleBuilder::RBBIRuleBuilder(ثابت UnicodeString   &قاعدا,
                                       UParseError     *parseErr,
                                       UError_ڪوڊ      &حالت)
 : f_قاعدا(قاعدا), f_Stripped_قاعدا(قاعدا)
{
    f_حالت = &حالت; // status is checked below
    fParseError = parseErr;
    fDebugEnv   = خالي_اشارو;
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
    fDebugEnv   = getenv("U_RBBIDEBUG");
#جيڪڏهن_پورو


    f_Forward_وڻ        = خالي_اشارو;
    f_ابتو_وڻ        = خالي_اشارو;
    f_محفوظ_Fwd_وڻ        = خالي_اشارو;
    f_محفوظ_Rev_وڻ        = خالي_اشارو;
    f_Default_وڻ        = &f_Forward_وڻ;
    f_Forward_جدول       = خالي_اشارو;
    f_قاعدو_حالت_Vals     = خالي_اشارو;
    f_Chain_قاعدا         = خطأ;
    fLookAheadHardBreak = خطأ;
    fUSetNodes          = خالي_اشارو;
    f_قاعدو_حالت_Vals     = خالي_اشارو;
    fScanner            = خالي_اشارو;
    f_سيٽ_Builder         = خالي_اشارو;
    إذا (parseErr) {
        uprv_memset(parseErr, 0, حجم(UParseError));
    }

    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    fUSetNodes          = جديد UVector(حالت); // bcos status gets overwritten here
    f_قاعدو_حالت_Vals     = جديد UVector(حالت);
    fScanner            = جديد RBBIRuleScanner(هذا);
    f_سيٽ_Builder         = جديد RBBISetBuilder(هذا);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (f_سيٽ_Builder == خالي_اشارو || fScanner == خالي_اشارو ||
        fUSetNodes == خالي_اشارو || f_قاعدو_حالت_Vals == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
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
        إذا (n==خالي_اشارو) {
            اكسر;
        }
        ميٽيو n;
    }

    ميٽيو fUSetNodes;
    ميٽيو f_سيٽ_Builder;
    ميٽيو f_Forward_جدول;
    ميٽيو f_Forward_وڻ;
    ميٽيو f_ابتو_وڻ;
    ميٽيو f_محفوظ_Fwd_وڻ;
    ميٽيو f_محفوظ_Rev_وڻ;
    ميٽيو fScanner;
    ميٽيو f_قاعدو_حالت_Vals;
}





//----------------------------------------------------------------------------------------
//
//   flattenData() -  Collect up the compiled RBBI rule data and put it into
//                    the format for saving in ICU data files,
//                    which is also the format needed by the RBBI runtime engine.
//
//----------------------------------------------------------------------------------------
ساكن int32_t align8(int32_t i) {أرجع (i+7) & 0xfffffff8;}

RBBIData_سرو *RBBIRuleBuilder::flatten_معلومات() {
    int32_t    i;

    إذا (U_FAILURE(*f_حالت)) {
        أرجع خالي_اشارو;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    f_Stripped_قاعدا = fScanner->strip_قاعدا(f_Stripped_قاعدا);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t سرو_سائيز        = align8(حجم(RBBIData_سرو));
    int32_t forward_جدول_سائيز  = align8(f_Forward_جدول->get_جدول_سائيز());
    int32_t ابتو_جدول_سائيز  = align8(f_Forward_جدول->get_محفوظ_جدول_سائيز());
    int32_t trie_سائيز          = align8(f_سيٽ_Builder->get_Trie_سائيز());
    int32_t حالت_جدول_سائيز   = align8(f_قاعدو_حالت_Vals->size() * حجم(int32_t));

    int32_t قاعدا_ڊيگهائي_اندريون_UTF8 = 0;
    u_str_منزل_UTF8_With_Sub(خالي_اشارو, 0, &قاعدا_ڊيگهائي_اندريون_UTF8,
                       f_Stripped_قاعدا.get_بفر(), f_Stripped_قاعدا.length(),
                       0xfffd, خالي_اشارو, f_حالت);
    *f_حالت = U_ZERO_ERROR;

    int32_t قاعدا_سائيز         = align8((قاعدا_ڊيگهائي_اندريون_UTF8+1));

    int32_t         ڪل_سائيز = سرو_سائيز
                                + forward_جدول_سائيز
                                + ابتو_جدول_سائيز
                                + حالت_جدول_سائيز + trie_سائيز + قاعدا_سائيز;

#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
    إذا (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
        RBBIDebugPrintf("Header Size:        %8d\n", سرو_سائيز);
        RBBIDebugPrintf("Forward Table Size: %8d\n", forward_جدول_سائيز);
        RBBIDebugPrintf("Reverse Table Size: %8d\n", ابتو_جدول_سائيز);
        RBBIDebugPrintf("Trie Size:          %8d\n", trie_سائيز);
        RBBIDebugPrintf("Status Table Size:  %8d\n", حالت_جدول_سائيز);
        RBBIDebugPrintf("Rules Size:         %8d\n", قاعدا_سائيز);
        RBBIDebugPrintf("-----------------------------\n");
        RBBIDebugPrintf("Total Size:         %8d\n", ڪل_سائيز);
    }
#جيڪڏهن_پورو

    Local_يادگيري<RBBIData_سرو> معلومات(static_cast<RBBIData_سرو*>(uprv_malloc(ڪل_سائيز)));
    إذا (معلومات.isNull()) {
        *f_حالت = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    uprv_memset(معلومات.getAlias(), 0, ڪل_سائيز);


    معلومات->fMagic            = 0xb1a0;
    معلومات->f_Format_نسخو[0] = RBBI_معلومات_FORMAT_نسخو[0];
    معلومات->f_Format_نسخو[1] = RBBI_معلومات_FORMAT_نسخو[1];
    معلومات->f_Format_نسخو[2] = RBBI_معلومات_FORMAT_نسخو[2];
    معلومات->f_Format_نسخو[3] = RBBI_معلومات_FORMAT_نسخو[3];
    معلومات->f_ڊيگهائي           = ڪل_سائيز;
    معلومات->f_Cat_ڳڻپ         = f_سيٽ_Builder->get_انگ_Char_Categories();

    معلومات->fFTable        = سرو_سائيز;
    معلومات->fFTableLen     = forward_جدول_سائيز;

    معلومات->fRTable        = معلومات->fFTable  + معلومات->fFTableLen;
    معلومات->fRTableLen     = ابتو_جدول_سائيز;

    معلومات->fTrie          = معلومات->fRTable + معلومات->fRTableLen;
    معلومات->fTrieLen       = trie_سائيز;
    معلومات->f_حالت_جدول   = معلومات->fTrie    + معلومات->fTrieLen;
    معلومات->f_حالت_جدول_Len= حالت_جدول_سائيز;
    معلومات->f_قاعدو_ذريعو    = معلومات->f_حالت_جدول + حالت_جدول_سائيز;
    معلومات->f_قاعدو_ذريعو_Len = قاعدا_ڊيگهائي_اندريون_UTF8;

    uprv_memset(معلومات->fReserved, 0, حجم(معلومات->fReserved));

    f_Forward_جدول->export_جدول(reinterpret_cast<uint8_t*>(معلومات.getAlias()) + معلومات->fFTable);
    f_Forward_جدول->export_محفوظ_جدول(reinterpret_cast<uint8_t*>(معلومات.getAlias()) + معلومات->fRTable);
    f_سيٽ_Builder->serializeTrie(reinterpret_cast<uint8_t*>(معلومات.getAlias()) + معلومات->fTrie);

    int32_t* قاعدو_حالت_جدول = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(معلومات.getAlias()) + معلومات->f_حالت_جدول);
    لكل (i=0; i<f_قاعدو_حالت_Vals->size(); i++) {
        قاعدو_حالت_جدول[i] = f_قاعدو_حالت_Vals->elementAti(i);
    }

    u_str_منزل_UTF8_With_Sub(reinterpret_cast<حرف*>(معلومات.getAlias()) + معلومات->f_قاعدو_ذريعو, قاعدا_سائيز, &قاعدا_ڊيگهائي_اندريون_UTF8,
                       f_Stripped_قاعدا.get_بفر(), f_Stripped_قاعدا.length(),
                       0xfffd, خالي_اشارو, f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع خالي_اشارو;
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
RBBIRuleBuilder::create_قاعدو_Based_Break_Iterator( ثابت UnicodeString    &قاعدا,
                                    UParseError      *parseError,
                                    UError_ڪوڊ       &حالت)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(قاعدا, parseError, حالت);
    إذا (U_FAILURE(حالت)) { // status checked here bcos build below doesn't
        أرجع خالي_اشارو;
    }

    RBBIData_سرو *معلومات = builder.build(حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    قاعدو_Based_Break_Iterator *This = جديد قاعدو_Based_Break_Iterator(معلومات, حالت);
    إذا (U_FAILURE(حالت)) {
        ميٽيو This;
        This = خالي_اشارو;
    } 
    وإلا إذا(This == خالي_اشارو) { // test for nullptr
        حالت = U_يادگيري_ورهاست_ERROR;
    }
    أرجع This;
}

RBBIData_سرو *RBBIRuleBuilder::build(UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }

    fScanner->parse();
    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    f_سيٽ_Builder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    f_Forward_جدول = جديد RBBITableBuilder(هذا, &f_Forward_وڻ, حالت);
    إذا (f_Forward_جدول == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }

    f_Forward_جدول->build_Forward_جدول();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    f_Forward_جدول->build_محفوظ_ابتو_جدول(حالت);


#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
    إذا (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
        f_Forward_جدول->print_حالتون();
        f_Forward_جدول->print_قاعدو_حالت_جدول();
        f_Forward_جدول->print_ابتو_جدول();
    }
#جيڪڏهن_پورو

    //    Generate the mapping tables (TRIE) from input code points to
    //    the character categories.
    //
    f_سيٽ_Builder->buildTrie();

    //
    //   Package up the compiled data into a memory image
    //      in the run-time format.
    //
    RBBIData_سرو *معلومات = flatten_معلومات(); // returns nullptr if error
    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
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
        طالما (f_Forward_جدول->ڳوليو_Dupl_Char_Class_From(&duplPair)) {
            f_سيٽ_Builder->ملايو_Categories(duplPair);
            f_Forward_جدول->remove_ڪالم(duplPair.ٻيو);
            didSomething = سچ;
        }

        طالما (f_Forward_جدول->remove_Duplicate_حالتون() > 0) {
            didSomething = سچ;
        }
    } طالما (didSomething);
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */
