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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/rbbi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubrk.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchriter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parsepos.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parseerr.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbirb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbinode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbiscan.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbisetb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbitblb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbidata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"


U_NAMESPACE_BEGIN


//----------------------------------------------------------------------------------------
//
//  Constructor.
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::RBBIRuleBuilder(ਅਚਲ UnicodeString   &ਨਿਯਮਾਂ,
                                       UParseError     *parseErr,
                                       UError_ਕੋਡ      &ਸਥਿਤੀ)
 : f_ਨਿਯਮਾਂ(ਨਿਯਮਾਂ), f_Stripped_ਨਿਯਮਾਂ(ਨਿਯਮਾਂ)
{
    f_ਸਥਿਤੀ = &ਸਥਿਤੀ; // status is checked below
    fParseError = parseErr;
    fDebugEnv   = ਖਾਲੀ_ਸੂਚਕ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    fDebugEnv   = getenv("U_RBBIDEBUG");
#ਜੇ_ਅੰਤ


    f_Forward_ਰੁੱਖ        = ਖਾਲੀ_ਸੂਚਕ;
    f_ਉਲਟ_ਰੁੱਖ        = ਖਾਲੀ_ਸੂਚਕ;
    f_ਸੁਰੱਖਿਅਤ_Fwd_ਰੁੱਖ        = ਖਾਲੀ_ਸੂਚਕ;
    f_ਸੁਰੱਖਿਅਤ_Rev_ਰੁੱਖ        = ਖਾਲੀ_ਸੂਚਕ;
    f_Default_ਰੁੱਖ        = &f_Forward_ਰੁੱਖ;
    f_Forward_ਸਾਰਣੀ       = ਖਾਲੀ_ਸੂਚਕ;
    f_ਨਿਯਮ_ਸਥਿਤੀ_Vals     = ਖਾਲੀ_ਸੂਚਕ;
    f_Chain_ਨਿਯਮਾਂ         = ਝੂਠ;
    fLookAheadHardBreak = ਝੂਠ;
    fUSetNodes          = ਖਾਲੀ_ਸੂਚਕ;
    f_ਨਿਯਮ_ਸਥਿਤੀ_Vals     = ਖਾਲੀ_ਸੂਚਕ;
    fScanner            = ਖਾਲੀ_ਸੂਚਕ;
    f_ਸੈੱਟ_Builder         = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (parseErr) {
        uprv_memset(parseErr, 0, ਆਕਾਰ(UParseError));
    }

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    fUSetNodes          = ਨਵਾਂ UVector(ਸਥਿਤੀ); // bcos status gets overwritten here
    f_ਨਿਯਮ_ਸਥਿਤੀ_Vals     = ਨਵਾਂ UVector(ਸਥਿਤੀ);
    fScanner            = ਨਵਾਂ RBBIRuleScanner(ਇਹ);
    f_ਸੈੱਟ_Builder         = ਨਵਾਂ RBBISetBuilder(ਇਹ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (f_ਸੈੱਟ_Builder == ਖਾਲੀ_ਸੂਚਕ || fScanner == ਖਾਲੀ_ਸੂਚਕ ||
        fUSetNodes == ਖਾਲੀ_ਸੂਚਕ || f_ਨਿਯਮ_ਸਥਿਤੀ_Vals == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}



//----------------------------------------------------------------------------------------
//
//  Destructor
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::~RBBIRuleBuilder() {

    ਪੂਰਨ        i;
    ਲਈ (i=0; ; i++) {
        RBBINode* n = static_cast<RBBINode*>(fUSetNodes->elementAt(i));
        ਜੇ (n==ਖਾਲੀ_ਸੂਚਕ) {
            ਰੋਕੋ;
        }
        ਮਿਟਾਓ n;
    }

    ਮਿਟਾਓ fUSetNodes;
    ਮਿਟਾਓ f_ਸੈੱਟ_Builder;
    ਮਿਟਾਓ f_Forward_ਸਾਰਣੀ;
    ਮਿਟਾਓ f_Forward_ਰੁੱਖ;
    ਮਿਟਾਓ f_ਉਲਟ_ਰੁੱਖ;
    ਮਿਟਾਓ f_ਸੁਰੱਖਿਅਤ_Fwd_ਰੁੱਖ;
    ਮਿਟਾਓ f_ਸੁਰੱਖਿਅਤ_Rev_ਰੁੱਖ;
    ਮਿਟਾਓ fScanner;
    ਮਿਟਾਓ f_ਨਿਯਮ_ਸਥਿਤੀ_Vals;
}





//----------------------------------------------------------------------------------------
//
//   flattenData() -  Collect up the compiled RBBI rule data and put it into
//                    the format for saving in ICU data files,
//                    which is also the format needed by the RBBI runtime engine.
//
//----------------------------------------------------------------------------------------
ਸਥਿਰ int32_t align8(int32_t i) {ਵਾਪਸ (i+7) & 0xfffffff8;}

RBBIData_ਸਿਰਲੇਖ *RBBIRuleBuilder::flatten_ਅੰਕੜੇ() {
    int32_t    i;

    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    f_Stripped_ਨਿਯਮਾਂ = fScanner->strip_ਨਿਯਮਾਂ(f_Stripped_ਨਿਯਮਾਂ);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t ਸਿਰਲੇਖ_ਮਾਪ        = align8(ਆਕਾਰ(RBBIData_ਸਿਰਲੇਖ));
    int32_t forward_ਸਾਰਣੀ_ਮਾਪ  = align8(f_Forward_ਸਾਰਣੀ->get_ਸਾਰਣੀ_ਮਾਪ());
    int32_t ਉਲਟ_ਸਾਰਣੀ_ਮਾਪ  = align8(f_Forward_ਸਾਰਣੀ->get_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ_ਮਾਪ());
    int32_t trie_ਮਾਪ          = align8(f_ਸੈੱਟ_Builder->get_Trie_ਮਾਪ());
    int32_t ਸਥਿਤੀ_ਸਾਰਣੀ_ਮਾਪ   = align8(f_ਨਿਯਮ_ਸਥਿਤੀ_Vals->size() * ਆਕਾਰ(int32_t));

    int32_t ਨਿਯਮਾਂ_ਲੰਮਾਈ_ਵਿੱਚ_UTF8 = 0;
    u_str_ਮੰਜ਼ਿਲ_UTF8_With_Sub(ਖਾਲੀ_ਸੂਚਕ, 0, &ਨਿਯਮਾਂ_ਲੰਮਾਈ_ਵਿੱਚ_UTF8,
                       f_Stripped_ਨਿਯਮਾਂ.get_ਬਫਰ(), f_Stripped_ਨਿਯਮਾਂ.length(),
                       0xfffd, ਖਾਲੀ_ਸੂਚਕ, f_ਸਥਿਤੀ);
    *f_ਸਥਿਤੀ = U_ZERO_ERROR;

    int32_t ਨਿਯਮਾਂ_ਮਾਪ         = align8((ਨਿਯਮਾਂ_ਲੰਮਾਈ_ਵਿੱਚ_UTF8+1));

    int32_t         ਕੁੱਲ_ਮਾਪ = ਸਿਰਲੇਖ_ਮਾਪ
                                + forward_ਸਾਰਣੀ_ਮਾਪ
                                + ਉਲਟ_ਸਾਰਣੀ_ਮਾਪ
                                + ਸਥਿਤੀ_ਸਾਰਣੀ_ਮਾਪ + trie_ਮਾਪ + ਨਿਯਮਾਂ_ਮਾਪ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    ਜੇ (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
        RBBIDebugPrintf("Header Size:        %8d\n", ਸਿਰਲੇਖ_ਮਾਪ);
        RBBIDebugPrintf("Forward Table Size: %8d\n", forward_ਸਾਰਣੀ_ਮਾਪ);
        RBBIDebugPrintf("Reverse Table Size: %8d\n", ਉਲਟ_ਸਾਰਣੀ_ਮਾਪ);
        RBBIDebugPrintf("Trie Size:          %8d\n", trie_ਮਾਪ);
        RBBIDebugPrintf("Status Table Size:  %8d\n", ਸਥਿਤੀ_ਸਾਰਣੀ_ਮਾਪ);
        RBBIDebugPrintf("Rules Size:         %8d\n", ਨਿਯਮਾਂ_ਮਾਪ);
        RBBIDebugPrintf("-----------------------------\n");
        RBBIDebugPrintf("Total Size:         %8d\n", ਕੁੱਲ_ਮਾਪ);
    }
#ਜੇ_ਅੰਤ

    Local_ਸਮ੍ਰਿਤੀ<RBBIData_ਸਿਰਲੇਖ> ਅੰਕੜੇ(static_cast<RBBIData_ਸਿਰਲੇਖ*>(uprv_malloc(ਕੁੱਲ_ਮਾਪ)));
    ਜੇ (ਅੰਕੜੇ.isNull()) {
        *f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_memset(ਅੰਕੜੇ.getAlias(), 0, ਕੁੱਲ_ਮਾਪ);


    ਅੰਕੜੇ->fMagic            = 0xb1a0;
    ਅੰਕੜੇ->f_Format_ਸੰਸਕਰਣ[0] = RBBI_ਅੰਕੜੇ_FORMAT_ਸੰਸਕਰਣ[0];
    ਅੰਕੜੇ->f_Format_ਸੰਸਕਰਣ[1] = RBBI_ਅੰਕੜੇ_FORMAT_ਸੰਸਕਰਣ[1];
    ਅੰਕੜੇ->f_Format_ਸੰਸਕਰਣ[2] = RBBI_ਅੰਕੜੇ_FORMAT_ਸੰਸਕਰਣ[2];
    ਅੰਕੜੇ->f_Format_ਸੰਸਕਰਣ[3] = RBBI_ਅੰਕੜੇ_FORMAT_ਸੰਸਕਰਣ[3];
    ਅੰਕੜੇ->f_ਲੰਮਾਈ           = ਕੁੱਲ_ਮਾਪ;
    ਅੰਕੜੇ->f_Cat_ਗਿਣਤੀ         = f_ਸੈੱਟ_Builder->get_ਅੰਕ_Char_Categories();

    ਅੰਕੜੇ->fFTable        = ਸਿਰਲੇਖ_ਮਾਪ;
    ਅੰਕੜੇ->fFTableLen     = forward_ਸਾਰਣੀ_ਮਾਪ;

    ਅੰਕੜੇ->fRTable        = ਅੰਕੜੇ->fFTable  + ਅੰਕੜੇ->fFTableLen;
    ਅੰਕੜੇ->fRTableLen     = ਉਲਟ_ਸਾਰਣੀ_ਮਾਪ;

    ਅੰਕੜੇ->fTrie          = ਅੰਕੜੇ->fRTable + ਅੰਕੜੇ->fRTableLen;
    ਅੰਕੜੇ->fTrieLen       = trie_ਮਾਪ;
    ਅੰਕੜੇ->f_ਸਥਿਤੀ_ਸਾਰਣੀ   = ਅੰਕੜੇ->fTrie    + ਅੰਕੜੇ->fTrieLen;
    ਅੰਕੜੇ->f_ਸਥਿਤੀ_ਸਾਰਣੀ_Len= ਸਥਿਤੀ_ਸਾਰਣੀ_ਮਾਪ;
    ਅੰਕੜੇ->f_ਨਿਯਮ_ਸਰੋਤ    = ਅੰਕੜੇ->f_ਸਥਿਤੀ_ਸਾਰਣੀ + ਸਥਿਤੀ_ਸਾਰਣੀ_ਮਾਪ;
    ਅੰਕੜੇ->f_ਨਿਯਮ_ਸਰੋਤ_Len = ਨਿਯਮਾਂ_ਲੰਮਾਈ_ਵਿੱਚ_UTF8;

    uprv_memset(ਅੰਕੜੇ->fReserved, 0, ਆਕਾਰ(ਅੰਕੜੇ->fReserved));

    f_Forward_ਸਾਰਣੀ->export_ਸਾਰਣੀ(reinterpret_cast<uint8_t*>(ਅੰਕੜੇ.getAlias()) + ਅੰਕੜੇ->fFTable);
    f_Forward_ਸਾਰਣੀ->export_ਸੁਰੱਖਿਅਤ_ਸਾਰਣੀ(reinterpret_cast<uint8_t*>(ਅੰਕੜੇ.getAlias()) + ਅੰਕੜੇ->fRTable);
    f_ਸੈੱਟ_Builder->serializeTrie(reinterpret_cast<uint8_t*>(ਅੰਕੜੇ.getAlias()) + ਅੰਕੜੇ->fTrie);

    int32_t* ਨਿਯਮ_ਸਥਿਤੀ_ਸਾਰਣੀ = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(ਅੰਕੜੇ.getAlias()) + ਅੰਕੜੇ->f_ਸਥਿਤੀ_ਸਾਰਣੀ);
    ਲਈ (i=0; i<f_ਨਿਯਮ_ਸਥਿਤੀ_Vals->size(); i++) {
        ਨਿਯਮ_ਸਥਿਤੀ_ਸਾਰਣੀ[i] = f_ਨਿਯਮ_ਸਥਿਤੀ_Vals->elementAti(i);
    }

    u_str_ਮੰਜ਼ਿਲ_UTF8_With_Sub(reinterpret_cast<ਅੱਖਰ*>(ਅੰਕੜੇ.getAlias()) + ਅੰਕੜੇ->f_ਨਿਯਮ_ਸਰੋਤ, ਨਿਯਮਾਂ_ਮਾਪ, &ਨਿਯਮਾਂ_ਲੰਮਾਈ_ਵਿੱਚ_UTF8,
                       f_Stripped_ਨਿਯਮਾਂ.get_ਬਫਰ(), f_Stripped_ਨਿਯਮਾਂ.length(),
                       0xfffd, ਖਾਲੀ_ਸੂਚਕ, f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਵਾਪਸ ਅੰਕੜੇ.orphan();
}


//----------------------------------------------------------------------------------------
//
//  createRuleBasedBreakIterator    construct from source rules that are passed in
//                                  in a UnicodeString
//
//----------------------------------------------------------------------------------------
BreakIterator *
RBBIRuleBuilder::create_ਨਿਯਮ_Based_Break_Iterator( ਅਚਲ UnicodeString    &ਨਿਯਮਾਂ,
                                    UParseError      *parseError,
                                    UError_ਕੋਡ       &ਸਥਿਤੀ)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(ਨਿਯਮਾਂ, parseError, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { // status checked here bcos build below doesn't
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    RBBIData_ਸਿਰਲੇਖ *ਅੰਕੜੇ = builder.build(ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    ਨਿਯਮ_Based_Break_Iterator *This = ਨਵਾਂ ਨਿਯਮ_Based_Break_Iterator(ਅੰਕੜੇ, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਮਿਟਾਓ This;
        This = ਖਾਲੀ_ਸੂਚਕ;
    } 
    ਨਹੀਂਤਾਂ ਜੇ(This == ਖਾਲੀ_ਸੂਚਕ) { // test for nullptr
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਵਾਪਸ This;
}

RBBIData_ਸਿਰਲੇਖ *RBBIRuleBuilder::build(UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    fScanner->parse();
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    f_ਸੈੱਟ_Builder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    f_Forward_ਸਾਰਣੀ = ਨਵਾਂ RBBITableBuilder(ਇਹ, &f_Forward_ਰੁੱਖ, ਸਥਿਤੀ);
    ਜੇ (f_Forward_ਸਾਰਣੀ == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    f_Forward_ਸਾਰਣੀ->build_Forward_ਸਾਰਣੀ();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    f_Forward_ਸਾਰਣੀ->build_ਸੁਰੱਖਿਅਤ_ਉਲਟ_ਸਾਰਣੀ(ਸਥਿਤੀ);


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    ਜੇ (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
        f_Forward_ਸਾਰਣੀ->print_ਹਾਲਤਾਂ();
        f_Forward_ਸਾਰਣੀ->print_ਨਿਯਮ_ਸਥਿਤੀ_ਸਾਰਣੀ();
        f_Forward_ਸਾਰਣੀ->print_ਉਲਟ_ਸਾਰਣੀ();
    }
#ਜੇ_ਅੰਤ

    //    Generate the mapping tables (TRIE) from input code points to
    //    the character categories.
    //
    f_ਸੈੱਟ_Builder->buildTrie();

    //
    //   Package up the compiled data into a memory image
    //      in the run-time format.
    //
    RBBIData_ਸਿਰਲੇਖ *ਅੰਕੜੇ = flatten_ਅੰਕੜੇ(); // returns nullptr if error
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ ਅੰਕੜੇ;
}

ਖਾਲੀ RBBIRuleBuilder::optimizeTables() {
    ਤਾਰਕਿਕ didSomething;
    f_Forward_ਸਾਰਣੀ->minimizeLookaheads();
    ਕਰੋ {
        didSomething = ਝੂਠ;

        // Begin looking for duplicates with char class 3.
        // Class 0 is unused and should not have other categories merged into
        // it. We allow merging categories *into* class 1 {eof} (but not the
        // reverse, so that eof remains in class 1).
        IntPair duplPair = {1, 0};
        ਜਦੋਂ (f_Forward_ਸਾਰਣੀ->ਲੱਭੋ_Dupl_Char_Class_From(&duplPair)) {
            f_ਸੈੱਟ_Builder->ਮਿਲਾਓ_Categories(duplPair);
            f_Forward_ਸਾਰਣੀ->remove_ਕਾਲਮ(duplPair.ਦੂਜਾ);
            didSomething = ਸੱਚ;
        }

        ਜਦੋਂ (f_Forward_ਸਾਰਣੀ->remove_Duplicate_ਹਾਲਤਾਂ() > 0) {
            didSomething = ਸੱਚ;
        }
    } ਜਦੋਂ (didSomething);
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */
