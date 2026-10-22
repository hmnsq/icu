// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
//  rbbisetb.h
/*
**********************************************************************
*   Copyright (c) 2001-2005, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RBBISETB_H
#ವ್ಯಾಖ್ಯಾನಿಸು RBBISETB_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/umutablecptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "rbbirb.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

U_NAMESPACE_BEGIN

//
//  RBBISetBuilder   Derives the character categories used by the runtime RBBI engine
//                   from the Unicode Sets appearing in the source  RBBI rules, and
//                   creates the TRIE table used to map from Unicode to the
//                   character categories.
//


//
//  RangeDescriptor
//
//     Each of the non-overlapping character ranges gets one of these descriptors.
//     All of them are strung together in a linked list, which is kept in order
//     (by character)
//
class RangeDescriptor : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    UChar32            f_ಆರಂಭ_Char {};            // Start of range, unicode 32 bit value.
    UChar32            f_ಕೊನೆ_Char {};              // End of range, unicode 32 bit value.
    int32_t            f_ಅಂಕೆ {0};                 // runtime-mapped input value for this range.
    bool               fIncludesDict {false};    // True if the range includes $dictionary.
    bool               f_ಮೊದಲ_ಒಳ_ಗುಂಪು {false};    // True if first range in a group with the same fNum.
    UVector           *fIncludesSets {nullptr};  // vector of the original
                                                 //   Unicode sets that include this range.
                                                 //    (Contains ptrs to uset nodes)
    RangeDescriptor   *f_ಮುಂದಿನ {nullptr};          // Next RangeDescriptor in the linked list.

    RangeDescriptor(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    RangeDescriptor(ಸ್ಥಿರಾಂಕ RangeDescriptor &other, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    ~RangeDescriptor();
    ಶೂನ್ಯ ವಿಭಜಿಸು(UChar32 where, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);   // Spit this range in two at "where", with
                                        //   where appearing in the second (higher) part.
    bool isDictionaryRange();           // Check whether this range appears as part of
                                        //   the Unicode set named "dictionary"

    RangeDescriptor(ಸ್ಥಿರಾಂಕ RangeDescriptor &other) = delete; // forbid default copying of this class
    RangeDescriptor &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ RangeDescriptor &other) = delete; // forbid assigning of this class
};


//
//  RBBISetBuilder   Handles processing of Unicode Sets from RBBI rules.
//
//      Starting with the rules parse tree from the scanner,
//
//                   -  Enumerate the set of UnicodeSets that are referenced
//                      by the RBBI rules.
//                   -  compute a derived set of non-overlapping UnicodeSets
//                      that will correspond to columns in the state table for
//                      the RBBI execution engine.
//                   -  construct the trie table that maps input characters
//                      to set numbers in the non-overlapping set of sets.
//


class RBBISetBuilder : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    RBBISetBuilder(RBBIRuleBuilder *rb);
    ~RBBISetBuilder();

    ಶೂನ್ಯ     buildRanges();
    ಶೂನ್ಯ     buildTrie();
    ಶೂನ್ಯ     ಕೂಡಿಸು_Val_ಗಮ್ಯ_Sets(UVector *sets,      uint32_t val);
    ಶೂನ್ಯ     ಕೂಡಿಸು_Val_ಗಮ್ಯ_ಗಣ (RBBINode *uset_ಗಂಟು, uint32_t val);
    int32_t  get_ಅಂಕೆ_Char_Categories() ಸ್ಥಿರಾಂಕ;   // CharCategories are the same as input symbol set to the
                                             //    runtime state machine, which are the same as
                                             //    columns in the DFA state table
    int32_t  get_Dict_Categories_ಆರಂಭ() ಸ್ಥಿರಾಂಕ; // First char category that includes $dictionary, or
                                             // last category + 1 if there are no dictionary categories.
    int32_t  get_Trie_ಗಾತ್ರ() /*const*/;        // Size in bytes of the serialized Trie.
    ಶೂನ್ಯ     serializeTrie(uint8_t *where);  // write out the serialized Trie.
    UChar32  get_ಮೊದಲ_Char(int32_t  val) ಸ್ಥಿರಾಂಕ;

    /**
     * Merge two character categories that have been identified as having equivalent behavior.
     * The ranges belonging to the second category (table column) will be added to the first.
     * @param categories the pair of categories to be merged.
     */
    ಶೂನ್ಯ     ವಿಲೀನ_Categories(IntPair categories);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    ಶೂನ್ಯ     printSets();
    ಶೂನ್ಯ     printRanges();
    ಶೂನ್ಯ     printRangeGroups();
#ಇಲ್ಲದಿದ್ದರೆ
    #ವ್ಯಾಖ್ಯಾನಿಸು printSets()
    #ವ್ಯಾಖ್ಯಾನಿಸು printRanges()
    #ವ್ಯಾಖ್ಯಾನಿಸು printRangeGroups()
#ಆದರೆ_ಅಂತ್ಯ

ಖಾಸಗಿ:
    RBBIRuleBuilder       *fRB;             // The RBBI Rule Compiler that owns us.
    UError_ಸಂಕೇತ            *f_ಸ್ಥಿತಿ;

    RangeDescriptor       *fRangeList;      // Head of the linked list of RangeDescriptors

    UMutableCPTrie        *fMutableTrie;    // The mapping TRIE that is the end result of processing
    UCPTrie               *fTrie;           //  the Unicode Sets.
    uint32_t               f_Trie_ಗಾತ್ರ;

    // Number of range groups, which are groups of ranges that are in the same original UnicodeSets.
    int32_t               f_ಗುಂಪು_ಎಣಿಕೆ;

    // The number of the first dictionary char category.
    // If there are no Dictionary categories, set to the last category + 1.
    int32_t               f_Dict_Categories_ಆರಂಭ;

    RBBISetBuilder(ಸ್ಥಿರಾಂಕ RBBISetBuilder &other) = delete; // forbid copying of this class
    RBBISetBuilder &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ RBBISetBuilder &other) = delete; // forbid copying of this class
};



U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */

#ಆದರೆ_ಅಂತ್ಯ
