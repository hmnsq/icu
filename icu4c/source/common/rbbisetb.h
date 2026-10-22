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

#నిర్వచితం_కాకపోతే RBBISETB_H
#నిర్వచించు RBBISETB_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/ucptrie.h"
#అన్తర్భావ "unicode/umutablecptrie.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "rbbirb.h"
#అన్తర్భావ "uvector.h"

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
class RangeDescriptor : బహిరంగ UMemory {
బహిరంగ:
    UChar32            f_ప్రారంభం_Char {};            // Start of range, unicode 32 bit value.
    UChar32            f_చివర_Char {};              // End of range, unicode 32 bit value.
    int32_t            f_అంకె {0};                 // runtime-mapped input value for this range.
    bool               fIncludesDict {false};    // True if the range includes $dictionary.
    bool               f_మొదటి_లోపల_సమూహం {false};    // True if first range in a group with the same fNum.
    UVector           *fIncludesSets {nullptr};  // vector of the original
                                                 //   Unicode sets that include this range.
                                                 //    (Contains ptrs to uset nodes)
    RangeDescriptor   *f_తదుపరి {nullptr};          // Next RangeDescriptor in the linked list.

    RangeDescriptor(UError_సంకేతం &స్థితి_2);
    RangeDescriptor(స్థిరరాశి RangeDescriptor &other, UError_సంకేతం &స్థితి_2);
    ~RangeDescriptor();
    శూన్య విభజించు(UChar32 where, UError_సంకేతం &స్థితి_2);   // Spit this range in two at "where", with
                                        //   where appearing in the second (higher) part.
    bool isDictionaryRange();           // Check whether this range appears as part of
                                        //   the Unicode set named "dictionary"

    RangeDescriptor(స్థిరరాశి RangeDescriptor &other) = delete; // forbid default copying of this class
    RangeDescriptor &క్రియాకారకం=(స్థిరరాశి RangeDescriptor &other) = delete; // forbid assigning of this class
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


class RBBISetBuilder : బహిరంగ UMemory {
బహిరంగ:
    RBBISetBuilder(RBBIRuleBuilder *rb);
    ~RBBISetBuilder();

    శూన్య     buildRanges();
    శూన్య     buildTrie();
    శూన్య     కూడు_Val_గమ్యం_Sets(UVector *sets,      uint32_t val);
    శూన్య     కూడు_Val_గమ్యం_సమితి (RBBINode *uset_కణుపు, uint32_t val);
    int32_t  get_అంకె_Char_Categories() స్థిరరాశి;   // CharCategories are the same as input symbol set to the
                                             //    runtime state machine, which are the same as
                                             //    columns in the DFA state table
    int32_t  get_Dict_Categories_ప్రారంభం() స్థిరరాశి; // First char category that includes $dictionary, or
                                             // last category + 1 if there are no dictionary categories.
    int32_t  get_Trie_కొలత() /*const*/;        // Size in bytes of the serialized Trie.
    శూన్య     serializeTrie(uint8_t *where);  // write out the serialized Trie.
    UChar32  get_మొదటి_Char(int32_t  val) స్థిరరాశి;

    /**
     * Merge two character categories that have been identified as having equivalent behavior.
     * The ranges belonging to the second category (table column) will be added to the first.
     * @param categories the pair of categories to be merged.
     */
    శూన్య     విలీనం_Categories(IntPair categories);

#నిర్వచితమైతే RBBI_DEBUG
    శూన్య     printSets();
    శూన్య     printRanges();
    శూన్య     printRangeGroups();
#లేకపోతే
    #నిర్వచించు printSets()
    #నిర్వచించు printRanges()
    #నిర్వచించు printRangeGroups()
#అయితే_ముగింపు

అంతరంగ:
    RBBIRuleBuilder       *fRB;             // The RBBI Rule Compiler that owns us.
    UError_సంకేతం            *f_స్థితి;

    RangeDescriptor       *fRangeList;      // Head of the linked list of RangeDescriptors

    UMutableCPTrie        *fMutableTrie;    // The mapping TRIE that is the end result of processing
    UCPTrie               *fTrie;           //  the Unicode Sets.
    uint32_t               f_Trie_కొలత;

    // Number of range groups, which are groups of ranges that are in the same original UnicodeSets.
    int32_t               f_సమూహం_లెక్క;

    // The number of the first dictionary char category.
    // If there are no Dictionary categories, set to the last category + 1.
    int32_t               f_Dict_Categories_ప్రారంభం;

    RBBISetBuilder(స్థిరరాశి RBBISetBuilder &other) = delete; // forbid copying of this class
    RBBISetBuilder &క్రియాకారకం=(స్థిరరాశి RBBISetBuilder &other) = delete; // forbid copying of this class
};



U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */

#అయితే_ముగింపు
