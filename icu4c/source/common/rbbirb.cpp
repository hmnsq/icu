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
RBBIRuleBuilder::RBBIRuleBuilder(ثابت UnicodeString   &قواعد,
                                       UParseError     *parseErr,
                                       UError_رمز      &حالت_2)
 : f_قواعد(قواعد), f_Stripped_قواعد(قواعد)
{
    f_حالت = &حالت_2; // status is checked below
    fParseError = parseErr;
    fDebugEnv   = خالی_اشاریہ;
#اگر_تعریف_شدہ RBBI_DEBUG
    fDebugEnv   = getenv("U_RBBIDEBUG");
#اگر_ختم


    f_Forward_درخت        = خالی_اشاریہ;
    f_الٹ_درخت        = خالی_اشاریہ;
    f_محفوظ_Fwd_درخت        = خالی_اشاریہ;
    f_محفوظ_Rev_درخت        = خالی_اشاریہ;
    f_Default_درخت        = &f_Forward_درخت;
    f_Forward_جدول       = خالی_اشاریہ;
    f_قاعدہ_حالت_Vals     = خالی_اشاریہ;
    f_Chain_قواعد         = جھوٹ;
    fLookAheadHardBreak = جھوٹ;
    fUSetNodes          = خالی_اشاریہ;
    f_قاعدہ_حالت_Vals     = خالی_اشاریہ;
    fScanner            = خالی_اشاریہ;
    f_سیٹ_Builder         = خالی_اشاریہ;
    اگر (parseErr) {
        uprv_memset(parseErr, 0, حجم(UParseError));
    }

    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }

    fUSetNodes          = نیا UVector(حالت_2); // bcos status gets overwritten here
    f_قاعدہ_حالت_Vals     = نیا UVector(حالت_2);
    fScanner            = نیا RBBIRuleScanner(یہ);
    f_سیٹ_Builder         = نیا RBBISetBuilder(یہ);
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    اگر (f_سیٹ_Builder == خالی_اشاریہ || fScanner == خالی_اشاریہ ||
        fUSetNodes == خالی_اشاریہ || f_قاعدہ_حالت_Vals == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
    }
}



//----------------------------------------------------------------------------------------
//
//  Destructor
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::~RBBIRuleBuilder() {

    صحیح        i;
    برائے (i=0; ; i++) {
        RBBINode* n = static_cast<RBBINode*>(fUSetNodes->elementAt(i));
        اگر (n==خالی_اشاریہ) {
            توڑو;
        }
        حذف n;
    }

    حذف fUSetNodes;
    حذف f_سیٹ_Builder;
    حذف f_Forward_جدول;
    حذف f_Forward_درخت;
    حذف f_الٹ_درخت;
    حذف f_محفوظ_Fwd_درخت;
    حذف f_محفوظ_Rev_درخت;
    حذف fScanner;
    حذف f_قاعدہ_حالت_Vals;
}





//----------------------------------------------------------------------------------------
//
//   flattenData() -  Collect up the compiled RBBI rule data and put it into
//                    the format for saving in ICU data files,
//                    which is also the format needed by the RBBI runtime engine.
//
//----------------------------------------------------------------------------------------
ساکن int32_t align8(int32_t i) {واپس (i+7) & 0xfffffff8;}

RBBIData_سرخی *RBBIRuleBuilder::flatten_ڈیٹا() {
    int32_t    i;

    اگر (U_FAILURE(*f_حالت)) {
        واپس خالی_اشاریہ;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    f_Stripped_قواعد = fScanner->strip_قواعد(f_Stripped_قواعد);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t سرخی_سائز        = align8(حجم(RBBIData_سرخی));
    int32_t forward_جدول_سائز  = align8(f_Forward_جدول->get_جدول_سائز());
    int32_t الٹ_جدول_سائز  = align8(f_Forward_جدول->get_محفوظ_جدول_سائز());
    int32_t trie_سائز          = align8(f_سیٹ_Builder->get_Trie_سائز());
    int32_t حالت_جدول_سائز   = align8(f_قاعدہ_حالت_Vals->size() * حجم(int32_t));

    int32_t قواعد_لمبائی_اندر_UTF8 = 0;
    u_str_مقصد_UTF8_With_Sub(خالی_اشاریہ, 0, &قواعد_لمبائی_اندر_UTF8,
                       f_Stripped_قواعد.get_بفر(), f_Stripped_قواعد.length(),
                       0xfffd, خالی_اشاریہ, f_حالت);
    *f_حالت = U_ZERO_ERROR;

    int32_t قواعد_سائز         = align8((قواعد_لمبائی_اندر_UTF8+1));

    int32_t         کل_سائز = سرخی_سائز
                                + forward_جدول_سائز
                                + الٹ_جدول_سائز
                                + حالت_جدول_سائز + trie_سائز + قواعد_سائز;

#اگر_تعریف_شدہ RBBI_DEBUG
    اگر (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
        RBBIDebugPrintf("Header Size:        %8d\n", سرخی_سائز);
        RBBIDebugPrintf("Forward Table Size: %8d\n", forward_جدول_سائز);
        RBBIDebugPrintf("Reverse Table Size: %8d\n", الٹ_جدول_سائز);
        RBBIDebugPrintf("Trie Size:          %8d\n", trie_سائز);
        RBBIDebugPrintf("Status Table Size:  %8d\n", حالت_جدول_سائز);
        RBBIDebugPrintf("Rules Size:         %8d\n", قواعد_سائز);
        RBBIDebugPrintf("-----------------------------\n");
        RBBIDebugPrintf("Total Size:         %8d\n", کل_سائز);
    }
#اگر_ختم

    Local_حافظہ<RBBIData_سرخی> ڈیٹا(static_cast<RBBIData_سرخی*>(uprv_malloc(کل_سائز)));
    اگر (ڈیٹا.isNull()) {
        *f_حالت = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    uprv_memset(ڈیٹا.getAlias(), 0, کل_سائز);


    ڈیٹا->fMagic            = 0xb1a0;
    ڈیٹا->f_Format_نسخہ[0] = RBBI_ڈیٹا_FORMAT_نسخہ[0];
    ڈیٹا->f_Format_نسخہ[1] = RBBI_ڈیٹا_FORMAT_نسخہ[1];
    ڈیٹا->f_Format_نسخہ[2] = RBBI_ڈیٹا_FORMAT_نسخہ[2];
    ڈیٹا->f_Format_نسخہ[3] = RBBI_ڈیٹا_FORMAT_نسخہ[3];
    ڈیٹا->f_لمبائی           = کل_سائز;
    ڈیٹا->f_Cat_گنتی         = f_سیٹ_Builder->get_عدد_Char_Categories();

    ڈیٹا->fFTable        = سرخی_سائز;
    ڈیٹا->fFTableLen     = forward_جدول_سائز;

    ڈیٹا->fRTable        = ڈیٹا->fFTable  + ڈیٹا->fFTableLen;
    ڈیٹا->fRTableLen     = الٹ_جدول_سائز;

    ڈیٹا->fTrie          = ڈیٹا->fRTable + ڈیٹا->fRTableLen;
    ڈیٹا->fTrieLen       = trie_سائز;
    ڈیٹا->f_حالت_جدول   = ڈیٹا->fTrie    + ڈیٹا->fTrieLen;
    ڈیٹا->f_حالت_جدول_Len= حالت_جدول_سائز;
    ڈیٹا->f_قاعدہ_ماخذ    = ڈیٹا->f_حالت_جدول + حالت_جدول_سائز;
    ڈیٹا->f_قاعدہ_ماخذ_Len = قواعد_لمبائی_اندر_UTF8;

    uprv_memset(ڈیٹا->fReserved, 0, حجم(ڈیٹا->fReserved));

    f_Forward_جدول->export_جدول(reinterpret_cast<uint8_t*>(ڈیٹا.getAlias()) + ڈیٹا->fFTable);
    f_Forward_جدول->export_محفوظ_جدول(reinterpret_cast<uint8_t*>(ڈیٹا.getAlias()) + ڈیٹا->fRTable);
    f_سیٹ_Builder->serializeTrie(reinterpret_cast<uint8_t*>(ڈیٹا.getAlias()) + ڈیٹا->fTrie);

    int32_t* قاعدہ_حالت_جدول = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(ڈیٹا.getAlias()) + ڈیٹا->f_حالت_جدول);
    برائے (i=0; i<f_قاعدہ_حالت_Vals->size(); i++) {
        قاعدہ_حالت_جدول[i] = f_قاعدہ_حالت_Vals->elementAti(i);
    }

    u_str_مقصد_UTF8_With_Sub(reinterpret_cast<حرف*>(ڈیٹا.getAlias()) + ڈیٹا->f_قاعدہ_ماخذ, قواعد_سائز, &قواعد_لمبائی_اندر_UTF8,
                       f_Stripped_قواعد.get_بفر(), f_Stripped_قواعد.length(),
                       0xfffd, خالی_اشاریہ, f_حالت);
    اگر (U_FAILURE(*f_حالت)) {
        واپس خالی_اشاریہ;
    }

    واپس ڈیٹا.orphan();
}


//----------------------------------------------------------------------------------------
//
//  createRuleBasedBreakIterator    construct from source rules that are passed in
//                                  in a UnicodeString
//
//----------------------------------------------------------------------------------------
BreakIterator *
RBBIRuleBuilder::create_قاعدہ_Based_Break_Iterator( ثابت UnicodeString    &قواعد,
                                    UParseError      *parseError,
                                    UError_رمز       &حالت_2)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(قواعد, parseError, حالت_2);
    اگر (U_FAILURE(حالت_2)) { // status checked here bcos build below doesn't
        واپس خالی_اشاریہ;
    }

    RBBIData_سرخی *ڈیٹا = builder.build(حالت_2);

    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    قاعدہ_Based_Break_Iterator *This = نیا قاعدہ_Based_Break_Iterator(ڈیٹا, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        حذف This;
        This = خالی_اشاریہ;
    } 
    ورنہ اگر(This == خالی_اشاریہ) { // test for nullptr
        حالت_2 = U_حافظہ_تخصیص_ERROR;
    }
    واپس This;
}

RBBIData_سرخی *RBBIRuleBuilder::build(UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

    fScanner->parse();
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    f_سیٹ_Builder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    f_Forward_جدول = نیا RBBITableBuilder(یہ, &f_Forward_درخت, حالت_2);
    اگر (f_Forward_جدول == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }

    f_Forward_جدول->build_Forward_جدول();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    f_Forward_جدول->build_محفوظ_الٹ_جدول(حالت_2);


#اگر_تعریف_شدہ RBBI_DEBUG
    اگر (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
        f_Forward_جدول->print_حالتیں();
        f_Forward_جدول->print_قاعدہ_حالت_جدول();
        f_Forward_جدول->print_الٹ_جدول();
    }
#اگر_ختم

    //    Generate the mapping tables (TRIE) from input code points to
    //    the character categories.
    //
    f_سیٹ_Builder->buildTrie();

    //
    //   Package up the compiled data into a memory image
    //      in the run-time format.
    //
    RBBIData_سرخی *ڈیٹا = flatten_ڈیٹا(); // returns nullptr if error
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }
    واپس ڈیٹا;
}

خالی RBBIRuleBuilder::optimizeTables() {
    منطقی didSomething;
    f_Forward_جدول->minimizeLookaheads();
    کرو {
        didSomething = جھوٹ;

        // Begin looking for duplicates with char class 3.
        // Class 0 is unused and should not have other categories merged into
        // it. We allow merging categories *into* class 1 {eof} (but not the
        // reverse, so that eof remains in class 1).
        IntPair duplPair = {1, 0};
        جب (f_Forward_جدول->ڈھونڈو_Dupl_Char_Class_From(&duplPair)) {
            f_سیٹ_Builder->ملاؤ_Categories(duplPair);
            f_Forward_جدول->remove_عمود(duplPair.دوسرا);
            didSomething = سچ;
        }

        جب (f_Forward_جدول->remove_Duplicate_حالتیں() > 0) {
            didSomething = سچ;
        }
    } جب (didSomething);
}

U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */
