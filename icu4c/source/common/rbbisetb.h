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

#यदि_अपरिभाषितम् RBBISETB_H
#परिभाषय RBBISETB_H

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/umutablecptrie.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "uvector.h"

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
class RangeDescriptor : सार्वजनिक UMemory {
सार्वजनिक:
    UChar32            fStartChar {};            // Start of range, unicode 32 bit value.
    UChar32            f_अन्तः_Char {};              // End of range, unicode 32 bit value.
    int32_t            f_अङ्कः {0};                 // runtime-mapped input value for this range.
    bool               fIncludesDict {false};    // True if the range includes $dictionary.
    bool               f_First_अन्तर्_गणः {false};    // True if first range in a group with the same fNum.
    UVector           *fIncludesSets {nullptr};  // vector of the original
                                                 //   Unicode sets that include this range.
                                                 //    (Contains ptrs to uset nodes)
    RangeDescriptor   *f_अग्र {nullptr};          // Next RangeDescriptor in the linked list.

    RangeDescriptor(UError_संहिता &स्थितिः);
    RangeDescriptor(ध्रुव RangeDescriptor &other, UError_संहिता &स्थितिः);
    ~RangeDescriptor();
    शून्य विभज(UChar32 where, UError_संहिता &स्थितिः);   // Spit this range in two at "where", with
                                        //   where appearing in the second (higher) part.
    bool isDictionaryRange();           // Check whether this range appears as part of
                                        //   the Unicode set named "dictionary"

    RangeDescriptor(ध्रुव RangeDescriptor &other) = delete; // forbid default copying of this class
    RangeDescriptor &संक्रियकः=(ध्रुव RangeDescriptor &other) = delete; // forbid assigning of this class
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


class RBBISetBuilder : सार्वजनिक UMemory {
सार्वजनिक:
    RBBISetBuilder(RBBIRuleBuilder *rb);
    ~RBBISetBuilder();

    शून्य     buildRanges();
    शून्य     buildTrie();
    शून्य     सङ्कलय_Val_गन्तव्यम्_Sets(UVector *sets,      uint32_t val);
    शून्य     सङ्कलय_Val_गन्तव्यम्_समुच्चयः (RBBINode *uset_ग्रन्थिः, uint32_t val);
    int32_t  get_अङ्कः_Char_Categories() ध्रुव;   // CharCategories are the same as input symbol set to the
                                             //    runtime state machine, which are the same as
                                             //    columns in the DFA state table
    int32_t  getDictCategoriesStart() ध्रुव; // First char category that includes $dictionary, or
                                             // last category + 1 if there are no dictionary categories.
    int32_t  get_Trie_आकार() /*const*/;        // Size in bytes of the serialized Trie.
    शून्य     serializeTrie(uint8_t *where);  // write out the serialized Trie.
    UChar32  getFirstChar(int32_t  val) ध्रुव;

    /**
     * Merge two character categories that have been identified as having equivalent behavior.
     * The ranges belonging to the second category (table column) will be added to the first.
     * @param categories the pair of categories to be merged.
     */
    शून्य     mergeCategories(IntPair categories);

#यदि_परिभाषितम् RBBI_DEBUG
    शून्य     printSets();
    शून्य     printRanges();
    शून्य     printRangeGroups();
#अन्यथा
    #परिभाषय printSets()
    #परिभाषय printRanges()
    #परिभाषय printRangeGroups()
#यदि_अन्तः

गुप्त:
    RBBIRuleBuilder       *fRB;             // The RBBI Rule Compiler that owns us.
    UError_संहिता            *f_स्थितिः;

    RangeDescriptor       *fRangeList;      // Head of the linked list of RangeDescriptors

    UMutableCPTrie        *fMutableTrie;    // The mapping TRIE that is the end result of processing
    UCPTrie               *fTrie;           //  the Unicode Sets.
    uint32_t               f_Trie_आकार;

    // Number of range groups, which are groups of ranges that are in the same original UnicodeSets.
    int32_t               f_गणः_गणनम्;

    // The number of the first dictionary char category.
    // If there are no Dictionary categories, set to the last category + 1.
    int32_t               fDictCategoriesStart;

    RBBISetBuilder(ध्रुव RBBISetBuilder &other) = delete; // forbid copying of this class
    RBBISetBuilder &संक्रियकः=(ध्रुव RBBISetBuilder &other) = delete; // forbid copying of this class
};



U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */

#यदि_अन्तः
