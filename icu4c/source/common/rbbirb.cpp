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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/rbbi.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubrk.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchriter.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"
#ಅನ್ತರ್ಭಾವ "unicode/parseerr.h"

#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "rbbirb.h"
#ಅನ್ತರ್ಭಾವ "rbbinode.h"
#ಅನ್ತರ್ಭಾವ "rbbiscan.h"
#ಅನ್ತರ್ಭಾವ "rbbisetb.h"
#ಅನ್ತರ್ಭಾವ "rbbitblb.h"
#ಅನ್ತರ್ಭಾವ "rbbidata.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"


U_NAMESPACE_BEGIN


//----------------------------------------------------------------------------------------
//
//  Constructor.
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::RBBIRuleBuilder(ಸ್ಥಿರಾಂಕ UnicodeString   &ನಿಯಮಗಳು,
                                       UParseError     *parseErr,
                                       UError_ಸಂಕೇತ      &ಸ್ಥಿತಿ_2)
 : f_ನಿಯಮಗಳು(ನಿಯಮಗಳು), f_Stripped_ನಿಯಮಗಳು(ನಿಯಮಗಳು)
{
    f_ಸ್ಥಿತಿ = &ಸ್ಥಿತಿ_2; // status is checked below
    fParseError = parseErr;
    fDebugEnv   = ಶೂನ್ಯಸೂಚಕ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    fDebugEnv   = getenv("U_RBBIDEBUG");
#ಆದರೆ_ಅಂತ್ಯ


    f_Forward_ವೃಕ್ಷ        = ಶೂನ್ಯಸೂಚಕ;
    f_ಹಿಮ್ಮುಖ_ವೃಕ್ಷ        = ಶೂನ್ಯಸೂಚಕ;
    f_ಸುರಕ್ಷಿತ_Fwd_ವೃಕ್ಷ        = ಶೂನ್ಯಸೂಚಕ;
    f_ಸುರಕ್ಷಿತ_Rev_ವೃಕ್ಷ        = ಶೂನ್ಯಸೂಚಕ;
    f_Default_ವೃಕ್ಷ        = &f_Forward_ವೃಕ್ಷ;
    f_Forward_ಕೋಷ್ಟಕ       = ಶೂನ್ಯಸೂಚಕ;
    f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals     = ಶೂನ್ಯಸೂಚಕ;
    f_Chain_ನಿಯಮಗಳು         = ಸುಳ್ಳು;
    fLookAheadHardBreak = ಸುಳ್ಳು;
    fUSetNodes          = ಶೂನ್ಯಸೂಚಕ;
    f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals     = ಶೂನ್ಯಸೂಚಕ;
    fScanner            = ಶೂನ್ಯಸೂಚಕ;
    f_ಗಣ_Builder         = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (parseErr) {
        uprv_memset(parseErr, 0, ಗಾತ್ರ(UParseError));
    }

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    fUSetNodes          = ಹೊಸ UVector(ಸ್ಥಿತಿ_2); // bcos status gets overwritten here
    f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals     = ಹೊಸ UVector(ಸ್ಥಿತಿ_2);
    fScanner            = ಹೊಸ RBBIRuleScanner(ಇದು);
    f_ಗಣ_Builder         = ಹೊಸ RBBISetBuilder(ಇದು);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (f_ಗಣ_Builder == ಶೂನ್ಯಸೂಚಕ || fScanner == ಶೂನ್ಯಸೂಚಕ ||
        fUSetNodes == ಶೂನ್ಯಸೂಚಕ || f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
}



//----------------------------------------------------------------------------------------
//
//  Destructor
//
//----------------------------------------------------------------------------------------
RBBIRuleBuilder::~RBBIRuleBuilder() {

    ಪೂರ್ಣಾಂಕ        i;
    ಗಾಗಿ (i=0; ; i++) {
        RBBINode* n = static_cast<RBBINode*>(fUSetNodes->elementAt(i));
        ಆದರೆ (n==ಶೂನ್ಯಸೂಚಕ) {
            ನಿಲ್ಲಿಸು;
        }
        ಅಳಿಸು n;
    }

    ಅಳಿಸು fUSetNodes;
    ಅಳಿಸು f_ಗಣ_Builder;
    ಅಳಿಸು f_Forward_ಕೋಷ್ಟಕ;
    ಅಳಿಸು f_Forward_ವೃಕ್ಷ;
    ಅಳಿಸು f_ಹಿಮ್ಮುಖ_ವೃಕ್ಷ;
    ಅಳಿಸು f_ಸುರಕ್ಷಿತ_Fwd_ವೃಕ್ಷ;
    ಅಳಿಸು f_ಸುರಕ್ಷಿತ_Rev_ವೃಕ್ಷ;
    ಅಳಿಸು fScanner;
    ಅಳಿಸು f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals;
}





//----------------------------------------------------------------------------------------
//
//   flattenData() -  Collect up the compiled RBBI rule data and put it into
//                    the format for saving in ICU data files,
//                    which is also the format needed by the RBBI runtime engine.
//
//----------------------------------------------------------------------------------------
ಸ್ಥಿರ int32_t align8(int32_t i) {ಹಿಂತಿರುಗಿ (i+7) & 0xfffffff8;}

RBBIData_ಶೀರ್ಷಿಕೆ *RBBIRuleBuilder::flatten_ದತ್ತಾಂಶ() {
    int32_t    i;

    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // Remove whitespace from the rules to make it smaller.
    // The rule parser has already removed comments.
    f_Stripped_ನಿಯಮಗಳು = fScanner->strip_ನಿಯಮಗಳು(f_Stripped_ನಿಯಮಗಳು);

    // Calculate the size of each section in the data.
    //   Sizes here are padded up to a multiple of 8 for better memory alignment.
    //   Sections sizes actually stored in the header are for the actual data
    //     without the padding.
    //
    int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ        = align8(ಗಾತ್ರ(RBBIData_ಶೀರ್ಷಿಕೆ));
    int32_t forward_ಕೋಷ್ಟಕ_ಗಾತ್ರ  = align8(f_Forward_ಕೋಷ್ಟಕ->get_ಕೋಷ್ಟಕ_ಗಾತ್ರ());
    int32_t ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ_ಗಾತ್ರ  = align8(f_Forward_ಕೋಷ್ಟಕ->get_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ_ಗಾತ್ರ());
    int32_t trie_ಗಾತ್ರ          = align8(f_ಗಣ_Builder->get_Trie_ಗಾತ್ರ());
    int32_t ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ_ಗಾತ್ರ   = align8(f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals->size() * ಗಾತ್ರ(int32_t));

    int32_t ನಿಯಮಗಳು_ಉದ್ದಳತೆ_ಒಳ_UTF8 = 0;
    u_str_ಗಮ್ಯ_UTF8_With_Sub(ಶೂನ್ಯಸೂಚಕ, 0, &ನಿಯಮಗಳು_ಉದ್ದಳತೆ_ಒಳ_UTF8,
                       f_Stripped_ನಿಯಮಗಳು.get_ಬಫರ್(), f_Stripped_ನಿಯಮಗಳು.length(),
                       0xfffd, ಶೂನ್ಯಸೂಚಕ, f_ಸ್ಥಿತಿ);
    *f_ಸ್ಥಿತಿ = U_ZERO_ERROR;

    int32_t ನಿಯಮಗಳು_ಗಾತ್ರ         = align8((ನಿಯಮಗಳು_ಉದ್ದಳತೆ_ಒಳ_UTF8+1));

    int32_t         ಒಟ್ಟು_ಗಾತ್ರ = ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ
                                + forward_ಕೋಷ್ಟಕ_ಗಾತ್ರ
                                + ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ_ಗಾತ್ರ
                                + ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ_ಗಾತ್ರ + trie_ಗಾತ್ರ + ನಿಯಮಗಳು_ಗಾತ್ರ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    ಆದರೆ (fDebugEnv && uprv_strstr(fDebugEnv, "size")) {
        RBBIDebugPrintf("Header Size:        %8d\n", ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);
        RBBIDebugPrintf("Forward Table Size: %8d\n", forward_ಕೋಷ್ಟಕ_ಗಾತ್ರ);
        RBBIDebugPrintf("Reverse Table Size: %8d\n", ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ_ಗಾತ್ರ);
        RBBIDebugPrintf("Trie Size:          %8d\n", trie_ಗಾತ್ರ);
        RBBIDebugPrintf("Status Table Size:  %8d\n", ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ_ಗಾತ್ರ);
        RBBIDebugPrintf("Rules Size:         %8d\n", ನಿಯಮಗಳು_ಗಾತ್ರ);
        RBBIDebugPrintf("-----------------------------\n");
        RBBIDebugPrintf("Total Size:         %8d\n", ಒಟ್ಟು_ಗಾತ್ರ);
    }
#ಆದರೆ_ಅಂತ್ಯ

    Local_ಸ್ಮರಣೆ<RBBIData_ಶೀರ್ಷಿಕೆ> ದತ್ತಾಂಶ(static_cast<RBBIData_ಶೀರ್ಷಿಕೆ*>(uprv_malloc(ಒಟ್ಟು_ಗಾತ್ರ)));
    ಆದರೆ (ದತ್ತಾಂಶ.isNull()) {
        *f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_memset(ದತ್ತಾಂಶ.getAlias(), 0, ಒಟ್ಟು_ಗಾತ್ರ);


    ದತ್ತಾಂಶ->fMagic            = 0xb1a0;
    ದತ್ತಾಂಶ->f_Format_ಆವೃತ್ತಿ[0] = RBBI_ದತ್ತಾಂಶ_FORMAT_ಆವೃತ್ತಿ[0];
    ದತ್ತಾಂಶ->f_Format_ಆವೃತ್ತಿ[1] = RBBI_ದತ್ತಾಂಶ_FORMAT_ಆವೃತ್ತಿ[1];
    ದತ್ತಾಂಶ->f_Format_ಆವೃತ್ತಿ[2] = RBBI_ದತ್ತಾಂಶ_FORMAT_ಆವೃತ್ತಿ[2];
    ದತ್ತಾಂಶ->f_Format_ಆವೃತ್ತಿ[3] = RBBI_ದತ್ತಾಂಶ_FORMAT_ಆವೃತ್ತಿ[3];
    ದತ್ತಾಂಶ->f_ಉದ್ದಳತೆ           = ಒಟ್ಟು_ಗಾತ್ರ;
    ದತ್ತಾಂಶ->f_Cat_ಎಣಿಕೆ         = f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories();

    ದತ್ತಾಂಶ->fFTable        = ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    ದತ್ತಾಂಶ->fFTableLen     = forward_ಕೋಷ್ಟಕ_ಗಾತ್ರ;

    ದತ್ತಾಂಶ->fRTable        = ದತ್ತಾಂಶ->fFTable  + ದತ್ತಾಂಶ->fFTableLen;
    ದತ್ತಾಂಶ->fRTableLen     = ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ_ಗಾತ್ರ;

    ದತ್ತಾಂಶ->fTrie          = ದತ್ತಾಂಶ->fRTable + ದತ್ತಾಂಶ->fRTableLen;
    ದತ್ತಾಂಶ->fTrieLen       = trie_ಗಾತ್ರ;
    ದತ್ತಾಂಶ->f_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ   = ದತ್ತಾಂಶ->fTrie    + ದತ್ತಾಂಶ->fTrieLen;
    ದತ್ತಾಂಶ->f_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ_Len= ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ_ಗಾತ್ರ;
    ದತ್ತಾಂಶ->f_ನಿಯಮ_ಮೂಲ    = ದತ್ತಾಂಶ->f_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ + ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ_ಗಾತ್ರ;
    ದತ್ತಾಂಶ->f_ನಿಯಮ_ಮೂಲ_Len = ನಿಯಮಗಳು_ಉದ್ದಳತೆ_ಒಳ_UTF8;

    uprv_memset(ದತ್ತಾಂಶ->fReserved, 0, ಗಾತ್ರ(ದತ್ತಾಂಶ->fReserved));

    f_Forward_ಕೋಷ್ಟಕ->export_ಕೋಷ್ಟಕ(reinterpret_cast<uint8_t*>(ದತ್ತಾಂಶ.getAlias()) + ದತ್ತಾಂಶ->fFTable);
    f_Forward_ಕೋಷ್ಟಕ->export_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ(reinterpret_cast<uint8_t*>(ದತ್ತಾಂಶ.getAlias()) + ದತ್ತಾಂಶ->fRTable);
    f_ಗಣ_Builder->serializeTrie(reinterpret_cast<uint8_t*>(ದತ್ತಾಂಶ.getAlias()) + ದತ್ತಾಂಶ->fTrie);

    int32_t* ನಿಯಮ_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ = reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(ದತ್ತಾಂಶ.getAlias()) + ದತ್ತಾಂಶ->f_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ);
    ಗಾಗಿ (i=0; i<f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals->size(); i++) {
        ನಿಯಮ_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ[i] = f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals->elementAti(i);
    }

    u_str_ಗಮ್ಯ_UTF8_With_Sub(reinterpret_cast<ಅಕ್ಷರ*>(ದತ್ತಾಂಶ.getAlias()) + ದತ್ತಾಂಶ->f_ನಿಯಮ_ಮೂಲ, ನಿಯಮಗಳು_ಗಾತ್ರ, &ನಿಯಮಗಳು_ಉದ್ದಳತೆ_ಒಳ_UTF8,
                       f_Stripped_ನಿಯಮಗಳು.get_ಬಫರ್(), f_Stripped_ನಿಯಮಗಳು.length(),
                       0xfffd, ಶೂನ್ಯಸೂಚಕ, f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.orphan();
}


//----------------------------------------------------------------------------------------
//
//  createRuleBasedBreakIterator    construct from source rules that are passed in
//                                  in a UnicodeString
//
//----------------------------------------------------------------------------------------
BreakIterator *
RBBIRuleBuilder::create_ನಿಯಮ_Based_Break_Iterator( ಸ್ಥಿರಾಂಕ UnicodeString    &ನಿಯಮಗಳು,
                                    UParseError      *parseError,
                                    UError_ಸಂಕೇತ       &ಸ್ಥಿತಿ_2)
{
    //
    // Read the input rules, generate a parse tree, symbol table,
    // and list of all Unicode Sets referenced by the rules.
    //
    RBBIRuleBuilder  builder(ನಿಯಮಗಳು, parseError, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { // status checked here bcos build below doesn't
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    RBBIData_ಶೀರ್ಷಿಕೆ *ದತ್ತಾಂಶ = builder.build(ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    //
    //  Create a break iterator from the compiled rules.
    //     (Identical to creation from stored pre-compiled rules)
    //
    // status is checked after init in construction.
    ನಿಯಮ_Based_Break_Iterator *This = ಹೊಸ ನಿಯಮ_Based_Break_Iterator(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಅಳಿಸು This;
        This = ಶೂನ್ಯಸೂಚಕ;
    } 
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(This == ಶೂನ್ಯಸೂಚಕ) { // test for nullptr
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಹಿಂತಿರುಗಿ This;
}

RBBIData_ಶೀರ್ಷಿಕೆ *RBBIRuleBuilder::build(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    fScanner->parse();
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    //
    // UnicodeSet processing.
    //    Munge the Unicode Sets to create an initial set of character categories.
    //
    f_ಗಣ_Builder->buildRanges();

    //
    //   Generate the DFA state transition table.
    //
    f_Forward_ಕೋಷ್ಟಕ = ಹೊಸ RBBITableBuilder(ಇದು, &f_Forward_ವೃಕ್ಷ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (f_Forward_ಕೋಷ್ಟಕ == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    f_Forward_ಕೋಷ್ಟಕ->build_Forward_ಕೋಷ್ಟಕ();

    // State table and character category optimization.
    // Merge equivalent rows and columns.
    // Note that this process alters the initial set of character categories,
    // causing the representation of UnicodeSets in the parse tree to become invalid.

    optimizeTables();
    f_Forward_ಕೋಷ್ಟಕ->build_ಸುರಕ್ಷಿತ_ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ(ಸ್ಥಿತಿ_2);


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    ಆದರೆ (fDebugEnv && uprv_strstr(fDebugEnv, "states")) {
        f_Forward_ಕೋಷ್ಟಕ->print_ಸ್ಥಿತಿಗಳು();
        f_Forward_ಕೋಷ್ಟಕ->print_ನಿಯಮ_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ();
        f_Forward_ಕೋಷ್ಟಕ->print_ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ();
    }
#ಆದರೆ_ಅಂತ್ಯ

    //    Generate the mapping tables (TRIE) from input code points to
    //    the character categories.
    //
    f_ಗಣ_Builder->buildTrie();

    //
    //   Package up the compiled data into a memory image
    //      in the run-time format.
    //
    RBBIData_ಶೀರ್ಷಿಕೆ *ದತ್ತಾಂಶ = flatten_ದತ್ತಾಂಶ(); // returns nullptr if error
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
}

ಶೂನ್ಯ RBBIRuleBuilder::optimizeTables() {
    ತಾರ್ಕಿಕ didSomething;
    f_Forward_ಕೋಷ್ಟಕ->minimizeLookaheads();
    ಮಾಡು {
        didSomething = ಸುಳ್ಳು;

        // Begin looking for duplicates with char class 3.
        // Class 0 is unused and should not have other categories merged into
        // it. We allow merging categories *into* class 1 {eof} (but not the
        // reverse, so that eof remains in class 1).
        IntPair duplPair = {1, 0};
        ಆಗ (f_Forward_ಕೋಷ್ಟಕ->ಕಂಡುಹಿಡಿ_Dupl_Char_Class_From(&duplPair)) {
            f_ಗಣ_Builder->ವಿಲೀನ_Categories(duplPair);
            f_Forward_ಕೋಷ್ಟಕ->remove_ಸ್ತಂಭ(duplPair.ಎರಡನೇ);
            didSomething = ನಿಜ;
        }

        ಆಗ (f_Forward_ಕೋಷ್ಟಕ->remove_Duplicate_ಸ್ಥಿತಿಗಳು() > 0) {
            didSomething = ನಿಜ;
        }
    } ಆಗ (didSomething);
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */
