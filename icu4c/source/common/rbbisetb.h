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

#جے_بنا_تعریف RBBISETB_H
#تعریف_کرو RBBISETB_H

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/ucptrie.h"
#ضمن "unicode/umutablecptrie.h"
#ضمن "unicode/uobject.h"
#ضمن "rbbirb.h"
#ضمن "uvector.h"

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
class RangeDescriptor : عام UMemory {
عام:
    UChar32            f_شروع_Char {};            // Start of range, unicode 32 bit value.
    UChar32            f_سرا_Char {};              // End of range, unicode 32 bit value.
    int32_t            f_عدد {0};                 // runtime-mapped input value for this range.
    bool               fIncludesDict {false};    // True if the range includes $dictionary.
    bool               f_پہلا_وچ_گروہ {false};    // True if first range in a group with the same fNum.
    UVector           *fIncludesSets {nullptr};  // vector of the original
                                                 //   Unicode sets that include this range.
                                                 //    (Contains ptrs to uset nodes)
    RangeDescriptor   *f_اگلا {nullptr};          // Next RangeDescriptor in the linked list.

    RangeDescriptor(UError_کوڈ &حالت);
    RangeDescriptor(ثابت RangeDescriptor &other, UError_کوڈ &حالت);
    ~RangeDescriptor();
    فراغ ونڈو(UChar32 where, UError_کوڈ &حالت);   // Spit this range in two at "where", with
                                        //   where appearing in the second (higher) part.
    bool isDictionaryRange();           // Check whether this range appears as part of
                                        //   the Unicode set named "dictionary"

    RangeDescriptor(ثابت RangeDescriptor &other) = delete; // forbid default copying of this class
    RangeDescriptor &عامل=(ثابت RangeDescriptor &other) = delete; // forbid assigning of this class
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


class RBBISetBuilder : عام UMemory {
عام:
    RBBISetBuilder(RBBIRuleBuilder *rb);
    ~RBBISetBuilder();

    فراغ     buildRanges();
    فراغ     buildTrie();
    فراغ     جوڑو_Val_منزل_Sets(UVector *sets,      uint32_t val);
    فراغ     جوڑو_Val_منزل_سیٹ (RBBINode *uset_گرہ, uint32_t val);
    int32_t  get_عدد_Char_Categories() ثابت;   // CharCategories are the same as input symbol set to the
                                             //    runtime state machine, which are the same as
                                             //    columns in the DFA state table
    int32_t  get_Dict_Categories_شروع() ثابت; // First char category that includes $dictionary, or
                                             // last category + 1 if there are no dictionary categories.
    int32_t  get_Trie_ناپ() /*const*/;        // Size in bytes of the serialized Trie.
    فراغ     serializeTrie(uint8_t *where);  // write out the serialized Trie.
    UChar32  get_پہلا_Char(int32_t  val) ثابت;

    /**
     * Merge two character categories that have been identified as having equivalent behavior.
     * The ranges belonging to the second category (table column) will be added to the first.
     * @param categories the pair of categories to be merged.
     */
    فراغ     ملاؤ_Categories(IntPair categories);

#جے_تعریف_شدہ RBBI_DEBUG
    فراغ     printSets();
    فراغ     printRanges();
    فراغ     printRangeGroups();
#وإلا
    #تعریف_کرو printSets()
    #تعریف_کرو printRanges()
    #تعریف_کرو printRangeGroups()
#جے_انت

خاص:
    RBBIRuleBuilder       *fRB;             // The RBBI Rule Compiler that owns us.
    UError_کوڈ            *f_حالت;

    RangeDescriptor       *fRangeList;      // Head of the linked list of RangeDescriptors

    UMutableCPTrie        *fMutableTrie;    // The mapping TRIE that is the end result of processing
    UCPTrie               *fTrie;           //  the Unicode Sets.
    uint32_t               f_Trie_ناپ;

    // Number of range groups, which are groups of ranges that are in the same original UnicodeSets.
    int32_t               f_گروہ_گنتی;

    // The number of the first dictionary char category.
    // If there are no Dictionary categories, set to the last category + 1.
    int32_t               f_Dict_Categories_شروع;

    RBBISetBuilder(ثابت RBBISetBuilder &other) = delete; // forbid copying of this class
    RBBISetBuilder &عامل=(ثابت RBBISetBuilder &other) = delete; // forbid copying of this class
};



U_NAMESPACE_سرا

#جے_انت /* #if !UCONFIG_NO_BREAK_ITERATION */

#جے_انت
