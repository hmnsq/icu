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
                                       UError_کد      &وضعیت)
 : f_قواعد(قواعد), f_Stripped_قواعد(قواعد)
{
    f_وضعیت = &وضعیت; // status is checked below
    fParseError = parseErr;
    fDebugEnv   = اشاره_تهی;
#اگر_تعریف_شده RBBI_DEBUG
    fDebugEnv   = getenv("U_RBBIDEBUG");
#پایان_اگر


    f_Forward_درخت        = اشاره_تهی;
    f_معکوس_درخت        = اشاره_تهی;
    f_امن_Fwd_درخت        = اشاره_تهی;
    f_امن_Rev_درخت        = اشاره_تهی;
    f_Default_درخت        = &f_Forward_درخت;
    f_Forward_جدول       = اشاره_تهی;
    f_قاعده_وضعیت_Vals     = اشاره_تهی;
    f_Chain_قواعد         = خطأ;
    fLookAheadHardBreak = خطأ;
    fUSetNodes          = اشاره_تهی;
    f_قاعده_وضعیت_Vals     = اشاره_تهی;
    fScanner            = اشاره_تهی;
    f_مجموعه_Builder         = اشاره_تهی;
    إذا (parseErr) {
        uprv_memset(parseErr, 0, حجم(UParseError));
    }

    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    fUSetNodes          = جديد UVector(وضعیت); // bcos status gets overwritten here
    f_قاعده_وضعیت_Vals     = جديد UVector(وضعیت);
    fScanner            = جديد RBBIRuleScanner(هذا);
    f_مجموعه_Builder         = جديد RBBISetBuilder(هذا);
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    إذا (f_مجموعه_Builder == اشاره_تهی || fScanner == اشاره_تهی ||
        fUSetNodes == اشاره_تهی || f_قاعده_وضعیت_Vals == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
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
        إذا (n==اشاره_تهی) {
            اكسر;
        }
        حذف n;
    }

    حذف fUSetNodes;
    حذف f_مجموعه_Builder;
    حذف f_Forward_جدول;
    حذف f_Forward_درخت;
    حذف f_معکوس_درخت;
    حذف f_امن_Fwd_درخت;
    حذف f_امن_Rev_درخت;
    حذف fScanner;
    حذف f_قاعده_وضعیت_Vals;
}





//----------------------------------------------------------------------------------------
//
//   flattenData() -  Collect up the compiled RBBI rule data and put it into
//                    the format for saving in ICU data files,
//                    which is also the format needed by the RBBI runtime engine.
//
//----------------------------------------------------------------------------------------
ساكن int32_t align8(int32_t i) {أرجع (i+7) & 0xfffffff8;}

RBBIData_سرآیند *RBBIRuleBuilder::flatten_داده() {
    int32_t    i;

    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع اشاره_تهی;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    f_Stripped_قواعد = fScanner->strip_قواعد(f_Stripped_قواعد);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t سرآیند_اندازه        = align8(حجم(RBBIData_سرآیند));
    int32_t forward_جدول_اندازه  = align8(f_Forward_جدول->get_جدول_اندازه());
    int32_t معکوس_جدول_اندازه  = align8(f_Forward_جدول->get_امن_جدول_اندازه());
    int32_t trie_اندازه          = align8(f_مجموعه_Builder->get_Trie_اندازه());
    int32_t وضعیت_جدول_اندازه   = align8(f_قاعده_وضعیت_Vals->size() * حجم(int32_t));

    int32_t قواعد_درازی_درون_UTF8 = 0;
    u_str_مقصد_UTF8_With_Sub(اشاره_تهی, 0, &قواعد_درازی_درون_UTF8,
                       f_Stripped_قواعد.get_میانگیر(), f_Stripped_قواعد.length(),
                       0xfffd, اشاره_تهی, f_وضعیت);
    *f_وضعیت = U_ZERO_ERROR;

    int32_t قواعد_اندازه         = align8((قواعد_درازی_درون_UTF8+1));

    int32_t         مجموع_اندازه = سرآیند_اندازه
                                + forward_جدول_اندازه
                                + معکوس_جدول_اندازه
                                + وضعیت_جدول_اندازه + trie_اندازه + قواعد_اندازه;

#اگر_تعریف_شده RBBI_DEBUG
    إذا (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
        RBBIDebugPrintf("Header Size:        %8d\n", سرآیند_اندازه);
        RBBIDebugPrintf("Forward Table Size: %8d\n", forward_جدول_اندازه);
        RBBIDebugPrintf("Reverse Table Size: %8d\n", معکوس_جدول_اندازه);
        RBBIDebugPrintf("Trie Size:          %8d\n", trie_اندازه);
        RBBIDebugPrintf("Status Table Size:  %8d\n", وضعیت_جدول_اندازه);
        RBBIDebugPrintf("Rules Size:         %8d\n", قواعد_اندازه);
        RBBIDebugPrintf("-----------------------------\n");
        RBBIDebugPrintf("Total Size:         %8d\n", مجموع_اندازه);
    }
#پایان_اگر

    Local_حافظه<RBBIData_سرآیند> داده(static_cast<RBBIData_سرآیند*>(uprv_malloc(مجموع_اندازه)));
    إذا (داده.isNull()) {
        *f_وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    uprv_memset(داده.getAlias(), 0, مجموع_اندازه);


    داده->fMagic            = 0xb1a0;
    داده->f_Format_نسخه[0] = RBBI_داده_FORMAT_نسخه[0];
    داده->f_Format_نسخه[1] = RBBI_داده_FORMAT_نسخه[1];
    داده->f_Format_نسخه[2] = RBBI_داده_FORMAT_نسخه[2];
    داده->f_Format_نسخه[3] = RBBI_داده_FORMAT_نسخه[3];
    داده->f_درازی           = مجموع_اندازه;
    داده->f_Cat_شمار         = f_مجموعه_Builder->get_رقم_Char_Categories();

    داده->fFTable        = سرآیند_اندازه;
    داده->fFTableLen     = forward_جدول_اندازه;

    داده->fRTable        = داده->fFTable  + داده->fFTableLen;
    داده->fRTableLen     = معکوس_جدول_اندازه;

    داده->fTrie          = داده->fRTable + داده->fRTableLen;
    داده->fTrieLen       = trie_اندازه;
    داده->f_وضعیت_جدول   = داده->fTrie    + داده->fTrieLen;
    داده->f_وضعیت_جدول_Len= وضعیت_جدول_اندازه;
    داده->f_قاعده_منبع    = داده->f_وضعیت_جدول + وضعیت_جدول_اندازه;
    داده->f_قاعده_منبع_Len = قواعد_درازی_درون_UTF8;

    uprv_memset(داده->fReserved, 0, حجم(داده->fReserved));

    f_Forward_جدول->export_جدول(reinterpret_cast<uint8_t*>(داده.getAlias()) + داده->fFTable);
    f_Forward_جدول->export_امن_جدول(reinterpret_cast<uint8_t*>(داده.getAlias()) + داده->fRTable);
    f_مجموعه_Builder->serializeTrie(reinterpret_cast<uint8_t*>(داده.getAlias()) + داده->fTrie);

    int32_t* قاعده_وضعیت_جدول = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(داده.getAlias()) + داده->f_وضعیت_جدول);
    لكل (i=0; i<f_قاعده_وضعیت_Vals->size(); i++) {
        قاعده_وضعیت_جدول[i] = f_قاعده_وضعیت_Vals->elementAti(i);
    }

    u_str_مقصد_UTF8_With_Sub(reinterpret_cast<حرف*>(داده.getAlias()) + داده->f_قاعده_منبع, قواعد_اندازه, &قواعد_درازی_درون_UTF8,
                       f_Stripped_قواعد.get_میانگیر(), f_Stripped_قواعد.length(),
                       0xfffd, اشاره_تهی, f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع اشاره_تهی;
    }

    أرجع داده.orphan();
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
                                    UError_کد       &وضعیت)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(قواعد, parseError, وضعیت);
    إذا (U_FAILURE(وضعیت)) { // status checked here bcos build below doesn't
        أرجع اشاره_تهی;
    }

    RBBIData_سرآیند *داده = builder.build(وضعیت);

    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    قاعده_Based_Break_Iterator *This = جديد قاعده_Based_Break_Iterator(داده, وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        حذف This;
        This = اشاره_تهی;
    } 
    وإلا إذا(This == اشاره_تهی) { // test for nullptr
        وضعیت = U_حافظه_تخصیص_ERROR;
    }
    أرجع This;
}

RBBIData_سرآیند *RBBIRuleBuilder::build(UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }

    fScanner->parse();
    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    f_مجموعه_Builder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    f_Forward_جدول = جديد RBBITableBuilder(هذا, &f_Forward_درخت, وضعیت);
    إذا (f_Forward_جدول == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }

    f_Forward_جدول->build_Forward_جدول();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    f_Forward_جدول->build_امن_معکوس_جدول(وضعیت);


#اگر_تعریف_شده RBBI_DEBUG
    إذا (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
        f_Forward_جدول->print_حالتها();
        f_Forward_جدول->print_قاعده_وضعیت_جدول();
        f_Forward_جدول->print_معکوس_جدول();
    }
#پایان_اگر

    //    Generate the mapping tables (TRIE) from input code points to
    //    the character categories.
    //
    f_مجموعه_Builder->buildTrie();

    //
    //   Package up the compiled data into a memory image
    //      in the run-time format.
    //
    RBBIData_سرآیند *داده = flatten_داده(); // returns nullptr if error
    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }
    أرجع داده;
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
        طالما (f_Forward_جدول->پیدا_کن_Dupl_Char_Class_From(&duplPair)) {
            f_مجموعه_Builder->ادغام_Categories(duplPair);
            f_Forward_جدول->remove_ستونی(duplPair.دومی);
            didSomething = درست;
        }

        طالما (f_Forward_جدول->remove_Duplicate_حالتها() > 0) {
            didSomething = درست;
        }
    } طالما (didSomething);
}

U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */
