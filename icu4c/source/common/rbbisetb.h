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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RBBISETB_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RBBISETB_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/umutablecptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbirb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"

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
class RangeDescriptor : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    UChar32            f_ਸ਼ੁਰੂ_Char {};            // Start of range, unicode 32 bit value.
    UChar32            f_ਸਿਰਾ_Char {};              // End of range, unicode 32 bit value.
    int32_t            f_ਅੰਕ {0};                 // runtime-mapped input value for this range.
    bool               fIncludesDict {false};    // True if the range includes $dictionary.
    bool               f_ਪਹਿਲਾ_ਵਿੱਚ_ਸਮੂਹ {false};    // True if first range in a group with the same fNum.
    UVector           *fIncludesSets {nullptr};  // vector of the original
                                                 //   Unicode sets that include this range.
                                                 //    (Contains ptrs to uset nodes)
    RangeDescriptor   *f_ਅਗਲਾ {nullptr};          // Next RangeDescriptor in the linked list.

    RangeDescriptor(UError_ਕੋਡ &ਸਥਿਤੀ);
    RangeDescriptor(ਅਚਲ RangeDescriptor &other, UError_ਕੋਡ &ਸਥਿਤੀ);
    ~RangeDescriptor();
    ਖਾਲੀ ਵੰਡੋ(UChar32 where, UError_ਕੋਡ &ਸਥਿਤੀ);   // Spit this range in two at "where", with
                                        //   where appearing in the second (higher) part.
    bool isDictionaryRange();           // Check whether this range appears as part of
                                        //   the Unicode set named "dictionary"

    RangeDescriptor(ਅਚਲ RangeDescriptor &other) = delete; // forbid default copying of this class
    RangeDescriptor &ਸੰਚਾਲਕ=(ਅਚਲ RangeDescriptor &other) = delete; // forbid assigning of this class
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


class RBBISetBuilder : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    RBBISetBuilder(RBBIRuleBuilder *rb);
    ~RBBISetBuilder();

    ਖਾਲੀ     buildRanges();
    ਖਾਲੀ     buildTrie();
    ਖਾਲੀ     ਜੋੜੋ_Val_ਮੰਜ਼ਿਲ_Sets(UVector *sets,      uint32_t val);
    ਖਾਲੀ     ਜੋੜੋ_Val_ਮੰਜ਼ਿਲ_ਸੈੱਟ (RBBINode *uset_ਗੰਢ, uint32_t val);
    int32_t  get_ਅੰਕ_Char_Categories() ਅਚਲ;   // CharCategories are the same as input symbol set to the
                                             //    runtime state machine, which are the same as
                                             //    columns in the DFA state table
    int32_t  get_Dict_Categories_ਸ਼ੁਰੂ() ਅਚਲ; // First char category that includes $dictionary, or
                                             // last category + 1 if there are no dictionary categories.
    int32_t  get_Trie_ਮਾਪ() /*const*/;        // Size in bytes of the serialized Trie.
    ਖਾਲੀ     serializeTrie(uint8_t *where);  // write out the serialized Trie.
    UChar32  get_ਪਹਿਲਾ_Char(int32_t  val) ਅਚਲ;

    /**
     * Merge two character categories that have been identified as having equivalent behavior.
     * The ranges belonging to the second category (table column) will be added to the first.
     * @param categories the pair of categories to be merged.
     */
    ਖਾਲੀ     ਮਿਲਾਓ_Categories(IntPair categories);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    ਖਾਲੀ     printSets();
    ਖਾਲੀ     printRanges();
    ਖਾਲੀ     printRangeGroups();
#ਨਹੀਂਤਾਂ
    #ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ printSets()
    #ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ printRanges()
    #ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ printRangeGroups()
#ਜੇ_ਅੰਤ

ਨਿਜੀ:
    RBBIRuleBuilder       *fRB;             // The RBBI Rule Compiler that owns us.
    UError_ਕੋਡ            *f_ਸਥਿਤੀ;

    RangeDescriptor       *fRangeList;      // Head of the linked list of RangeDescriptors

    UMutableCPTrie        *fMutableTrie;    // The mapping TRIE that is the end result of processing
    UCPTrie               *fTrie;           //  the Unicode Sets.
    uint32_t               f_Trie_ਮਾਪ;

    // Number of range groups, which are groups of ranges that are in the same original UnicodeSets.
    int32_t               f_ਸਮੂਹ_ਗਿਣਤੀ;

    // The number of the first dictionary char category.
    // If there are no Dictionary categories, set to the last category + 1.
    int32_t               f_Dict_Categories_ਸ਼ੁਰੂ;

    RBBISetBuilder(ਅਚਲ RBBISetBuilder &other) = delete; // forbid copying of this class
    RBBISetBuilder &ਸੰਚਾਲਕ=(ਅਚਲ RBBISetBuilder &other) = delete; // forbid copying of this class
};



U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */

#ਜੇ_ਅੰਤ
