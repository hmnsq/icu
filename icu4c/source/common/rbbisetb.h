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

#ifndef RBBISETB_H
#define RBBISETB_H

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_BREAK_ITERATION

#અન્તર્ભાવ "unicode/ucptrie.h"
#અન્તર્ભાવ "unicode/umutablecptrie.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "rbbirb.h"
#અન્તર્ભાવ "uvector.h"

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
class RangeDescriptor : જાહેર UMemory {
જાહેર:
    UChar32            fStartChar {};            // Start of range, unicode 32 bit value.
    UChar32            fEndChar {};              // End of range, unicode 32 bit value.
    int32_t            f_અંક {0};                 // runtime-mapped input value for this range.
    bool               fIncludesDict {false};    // True if the range includes $dictionary.
    bool               fFirstInGroup {false};    // True if first range in a group with the same fNum.
    UVector           *fIncludesSets {nullptr};  // vector of the original
                                                 //   Unicode sets that include this range.
                                                 //    (Contains ptrs to uset nodes)
    RangeDescriptor   *fNext {nullptr};          // Next RangeDescriptor in the linked list.

    RangeDescriptor(UErrorCode &status);
    RangeDescriptor(અચલ RangeDescriptor &other, UErrorCode &status);
    ~RangeDescriptor();
    શૂન્ય split(UChar32 where, UErrorCode &status);   // Spit this range in two at "where", with
                                        //   where appearing in the second (higher) part.
    bool isDictionaryRange();           // Check whether this range appears as part of
                                        //   the Unicode set named "dictionary"

    RangeDescriptor(અચલ RangeDescriptor &other) = delete; // forbid default copying of this class
    RangeDescriptor &operator=(અચલ RangeDescriptor &other) = delete; // forbid assigning of this class
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


class RBBISetBuilder : જાહેર UMemory {
જાહેર:
    RBBISetBuilder(RBBIRuleBuilder *rb);
    ~RBBISetBuilder();

    શૂન્ય     buildRanges();
    શૂન્ય     buildTrie();
    શૂન્ય     addValToSets(UVector *sets,      uint32_t val);
    શૂન્ય     addValToSet (RBBINode *uset_ગાંઠ, uint32_t val);
    int32_t  get_અંક_Char_Categories() અચલ;   // CharCategories are the same as input symbol set to the
                                             //    runtime state machine, which are the same as
                                             //    columns in the DFA state table
    int32_t  getDictCategoriesStart() અચલ; // First char category that includes $dictionary, or
                                             // last category + 1 if there are no dictionary categories.
    int32_t  getTrieSize() /*const*/;        // Size in bytes of the serialized Trie.
    શૂન્ય     serializeTrie(uint8_t *where);  // write out the serialized Trie.
    UChar32  getFirstChar(int32_t  val) અચલ;

    /**
     * Merge two character categories that have been identified as having equivalent behavior.
     * The ranges belonging to the second category (table column) will be added to the first.
     * @param categories the pair of categories to be merged.
     */
    શૂન્ય     mergeCategories(IntPair categories);

#ifdef RBBI_DEBUG
    શૂન્ય     printSets();
    શૂન્ય     printRanges();
    શૂન્ય     printRangeGroups();
#નહીંતર
    #define printSets()
    #define printRanges()
    #define printRangeGroups()
#endif

ખાનગી:
    RBBIRuleBuilder       *fRB;             // The RBBI Rule Compiler that owns us.
    UErrorCode            *fStatus;

    RangeDescriptor       *fRangeList;      // Head of the linked list of RangeDescriptors

    UMutableCPTrie        *fMutableTrie;    // The mapping TRIE that is the end result of processing
    UCPTrie               *fTrie;           //  the Unicode Sets.
    uint32_t               fTrieSize;

    // Number of range groups, which are groups of ranges that are in the same original UnicodeSets.
    int32_t               f_Group_ગણતરી;

    // The number of the first dictionary char category.
    // If there are no Dictionary categories, set to the last category + 1.
    int32_t               fDictCategoriesStart;

    RBBISetBuilder(અચલ RBBISetBuilder &other) = delete; // forbid copying of this class
    RBBISetBuilder &operator=(અચલ RBBISetBuilder &other) = delete; // forbid copying of this class
};



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif
