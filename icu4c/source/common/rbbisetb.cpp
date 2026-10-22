// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
//  rbbisetb.cpp
//
/*
***************************************************************************
*   Copyright (C) 2002-2008 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/
//
//  RBBISetBuilder   Handles processing of Unicode Sets from RBBI rules
//                   (part of the rule building process.)
//
//      Starting with the rules parse tree from the scanner,
//
//                   -  Enumerate the set of UnicodeSets that are referenced
//                      by the RBBI rules.
//                   -  compute a set of non-overlapping character ranges
//                      with all characters within a range belonging to the same
//                      set of input unicode sets.
//                   -  Derive a set of non-overlapping UnicodeSet (like things)
//                      that will correspond to columns in the state table for
//                      the RBBI execution engine.  All characters within one
//                      of these sets belong to the same set of the original
//                      UnicodeSets from the user's rules.
//                   -  construct the trie table that maps input characters
//                      to the index of the matching non-overlapping set of set from
//                      the previous step.
//

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"

#ಅನ್ತರ್ಭಾವ "rbbisetb.h"
#ಅನ್ತರ್ಭಾವ "rbbinode.h"

U_NAMESPACE_BEGIN

ಸ್ಥಿರಾಂಕ int32_t k_Max_Char_Categories_For8_ಬಿಟ್ಟುಗಳು_Trie = 255;
//------------------------------------------------------------------------
//
//   Constructor
//
//------------------------------------------------------------------------
RBBISetBuilder::RBBISetBuilder(RBBIRuleBuilder *rb)
{
    fRB             = rb;
    f_ಸ್ಥಿತಿ         = rb->f_ಸ್ಥಿತಿ;
    fRangeList      = ಶೂನ್ಯಸೂಚಕ;
    fMutableTrie    = ಶೂನ್ಯಸೂಚಕ;
    fTrie           = ಶೂನ್ಯಸೂಚಕ;
    f_Trie_ಗಾತ್ರ       = 0;
    f_ಗುಂಪು_ಎಣಿಕೆ     = 0;
}


//------------------------------------------------------------------------
//
//   Destructor
//
//------------------------------------------------------------------------
RBBISetBuilder::~RBBISetBuilder()
{
    RangeDescriptor   *ಮುಂದಿನ_Range_Desc;

    // Walk through & delete the linked list of RangeDescriptors
    ಗಾಗಿ (ಮುಂದಿನ_Range_Desc = fRangeList; ಮುಂದಿನ_Range_Desc!=ಶೂನ್ಯಸೂಚಕ;) {
        RangeDescriptor *r = ಮುಂದಿನ_Range_Desc;
        ಮುಂದಿನ_Range_Desc      = r->f_ಮುಂದಿನ;
        ಅಳಿಸು r;
    }

    ucptrie_close(fTrie);
    umutablecptrie_close(fMutableTrie);
}




//------------------------------------------------------------------------
//
//   build          Build the list of non-overlapping character ranges
//                  from the Unicode Sets.
//
//------------------------------------------------------------------------
ಶೂನ್ಯ RBBISetBuilder::buildRanges() {
    RBBINode        *uset_ಗಂಟು;
    RangeDescriptor *rlRange;

    ಆದರೆ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = ಹೊಸ RangeDescriptor(*f_ಸ್ಥಿತಿ); // will check for status here
    ಆದರೆ (fRangeList == ಶೂನ್ಯಸೂಚಕ) {
        *f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    fRangeList->f_ಆರಂಭ_Char    = 0;
    fRangeList->f_ಕೊನೆ_Char      = 0x10ffff;

    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    ಪೂರ್ಣಾಂಕ  ni;
    ಗಾಗಿ (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ಗಂಟು = static_cast<RBBINode*>(ಇದು->fRB->fUSetNodes->elementAt(ni));
        ಆದರೆ (uset_ಗಂಟು==ಶೂನ್ಯಸೂಚಕ) {
            ನಿಲ್ಲಿಸು;
        }

        Unicode_ಗಣ      *ಒಳಹರಿವು_ಗಣ             = uset_ಗಂಟು->f_ಒಳಹರಿವು_ಗಣ;
        int32_t          ಒಳಹರಿವು_ಗಣ_Range_ಎಣಿಕೆ   = ಒಳಹರಿವು_ಗಣ->get_Range_ಎಣಿಕೆ();
        ಪೂರ್ಣಾಂಕ              ಒಳಹರಿವು_ಗಣ_Range_ಸೂಚ್ಯಂಕ   = 0;
                         rlRange              = fRangeList;

        ಗಾಗಿ (;;) {
            ಆದರೆ (ಒಳಹರಿವು_ಗಣ_Range_ಸೂಚ್ಯಂಕ >= ಒಳಹರಿವು_ಗಣ_Range_ಎಣಿಕೆ) {
                ನಿಲ್ಲಿಸು;
            }
            UChar32      ಒಳಹರಿವು_ಗಣ_Range_Begin  = ಒಳಹರಿವು_ಗಣ->get_Range_ಆರಂಭ(ಒಳಹರಿವು_ಗಣ_Range_ಸೂಚ್ಯಂಕ);
            UChar32      ಒಳಹರಿವು_ಗಣ_Range_ಕೊನೆ    = ಒಳಹರಿವು_ಗಣ->get_Range_ಕೊನೆ(ಒಳಹರಿವು_ಗಣ_Range_ಸೂಚ್ಯಂಕ);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            ಆಗ (rlRange->f_ಕೊನೆ_Char < ಒಳಹರಿವು_ಗಣ_Range_Begin) {
                rlRange = rlRange->f_ಮುಂದಿನ;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            ಆದರೆ (rlRange->f_ಆರಂಭ_Char < ಒಳಹರಿವು_ಗಣ_Range_Begin) {
                rlRange->ವಿಭಜಿಸು(ಒಳಹರಿವು_ಗಣ_Range_Begin, *f_ಸ್ಥಿತಿ);
                ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
                    ಹಿಂತಿರುಗಿ;
                }
                ಮುಂದುವರಿ;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            ಆದರೆ (rlRange->f_ಕೊನೆ_Char > ಒಳಹರಿವು_ಗಣ_Range_ಕೊನೆ) {
                rlRange->ವಿಭಜಿಸು(ಒಳಹರಿವು_ಗಣ_Range_ಕೊನೆ+1, *f_ಸ್ಥಿತಿ);
                ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
                    ಹಿಂತಿರುಗಿ;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            ಆದರೆ (rlRange->fIncludesSets->ಸೂಚ್ಯಂಕ_ದ(uset_ಗಂಟು) == -1) {
                rlRange->fIncludesSets->ಕೂಡಿಸು_Element(uset_ಗಂಟು, *f_ಸ್ಥಿತಿ);
                ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
                    ಹಿಂತಿರುಗಿ;
                }
            }

            // Advance over ranges that we are finished with.
            ಆದರೆ (ಒಳಹರಿವು_ಗಣ_Range_ಕೊನೆ == rlRange->f_ಕೊನೆ_Char) {
                ಒಳಹರಿವು_ಗಣ_Range_ಸೂಚ್ಯಂಕ++;
            }
            rlRange = rlRange->f_ಮುಂದಿನ;
        }
    }

    ಆದರೆ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

    //
    //  Group the above ranges, with each group consisting of one or more
    //    ranges that are in exactly the same set of original UnicodeSets.
    //    The groups are numbered, and these group numbers are the set of
    //    input symbols recognized by the run-time state machine.
    //
    //    Numbering: # 0  (state table column 0) is unused.
    //               # 1  is reserved - table column 1 is for end-of-input
    //               # 2  is reserved - table column 2 is for beginning-of-input
    //               # 3  is the first range list.
    //
    RangeDescriptor *rl_ಹುಡುಕು_Range;
    int32_t dict_ಗುಂಪು_ಎಣಿಕೆ = 0;

    ಗಾಗಿ (rlRange = fRangeList; rlRange!=ಶೂನ್ಯಸೂಚಕ; rlRange=rlRange->f_ಮುಂದಿನ) {
        ಗಾಗಿ (rl_ಹುಡುಕು_Range=fRangeList; rl_ಹುಡುಕು_Range != rlRange; rl_ಹುಡುಕು_Range=rl_ಹುಡುಕು_Range->f_ಮುಂದಿನ) {
            ಆದರೆ (rlRange->fIncludesSets->equals(*rl_ಹುಡುಕು_Range->fIncludesSets)) {
                rlRange->f_ಅಂಕೆ = rl_ಹುಡುಕು_Range->f_ಅಂಕೆ;
                rlRange->fIncludesDict = rl_ಹುಡುಕು_Range->fIncludesDict;
                ನಿಲ್ಲಿಸು;
            }
        }
        ಆದರೆ (rlRange->f_ಅಂಕೆ == 0) {
            rlRange->f_ಮೊದಲ_ಒಳ_ಗುಂಪು = ನಿಜ;
            ಆದರೆ (rlRange->isDictionaryRange()) {
                rlRange->f_ಅಂಕೆ = ++dict_ಗುಂಪು_ಎಣಿಕೆ;
                rlRange->fIncludesDict = ನಿಜ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                f_ಗುಂಪು_ಎಣಿಕೆ++;
                rlRange->f_ಅಂಕೆ = f_ಗುಂಪು_ಎಣಿಕೆ+2;
                ಕೂಡಿಸು_Val_ಗಮ್ಯ_Sets(rlRange->fIncludesSets, rlRange->f_ಅಂಕೆ);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    f_Dict_Categories_ಆರಂಭ = f_ಗುಂಪು_ಎಣಿಕೆ + 3;
    ಗಾಗಿ (rlRange = fRangeList; rlRange!=ಶೂನ್ಯಸೂಚಕ; rlRange=rlRange->f_ಮುಂದಿನ) {
        ಆದರೆ (rlRange->fIncludesDict) {
            rlRange->f_ಅಂಕೆ += f_Dict_Categories_ಆರಂಭ - 1;
            ಆದರೆ (rlRange->f_ಮೊದಲ_ಒಳ_ಗುಂಪು) {
                ಕೂಡಿಸು_Val_ಗಮ್ಯ_Sets(rlRange->fIncludesSets, rlRange->f_ಅಂಕೆ);
            }
        }
    }
    f_ಗುಂಪು_ಎಣಿಕೆ += dict_ಗುಂಪು_ಎಣಿಕೆ;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    ಗಾಗಿ (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ಗಂಟು = static_cast<RBBINode*>(ಇದು->fRB->fUSetNodes->elementAt(ni));
        ಆದರೆ (uset_ಗಂಟು==ಶೂನ್ಯಸೂಚಕ) {
            ನಿಲ್ಲಿಸು;
        }
        Unicode_ಗಣ      *ಒಳಹರಿವು_ಗಣ = uset_ಗಂಟು->f_ಒಳಹರಿವು_ಗಣ;
        ಆದರೆ (ಒಳಹರಿವು_ಗಣ->ಒಳಗೊಂಡಿದೆ(eofString)) {
            ಕೂಡಿಸು_Val_ಗಮ್ಯ_ಗಣ(uset_ಗಂಟು, 1);
        }
    }


    ಆದರೆ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    ಆದರೆ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
ಶೂನ್ಯ RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        f_ಸ್ಥಿತಿ);

    ಗಾಗಿ (RangeDescriptor *range = fRangeList; range!=ಶೂನ್ಯಸೂಚಕ && U_SUCCESS(*f_ಸ್ಥಿತಿ); range=range->f_ಮುಂದಿನ) {
        umutablecptrie_ಗಣ_Range(fMutableTrie,
                                range->f_ಆರಂಭ_Char,     // Range start
                                range->f_ಕೊನೆ_Char,       // Range end (inclusive)
                                range->f_ಅಂಕೆ,           // value for range
                                f_ಸ್ಥಿತಿ);
    }
}


ಶೂನ್ಯ RBBISetBuilder::ವಿಲೀನ_Categories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.ಎರಡನೇ > categories.first);
    U_ASSERT((categories.first <  f_Dict_Categories_ಆರಂಭ && categories.ಎರಡನೇ <  f_Dict_Categories_ಆರಂಭ) ||
             (categories.first >= f_Dict_Categories_ಆರಂಭ && categories.ಎರಡನೇ >= f_Dict_Categories_ಆರಂಭ));

    ಗಾಗಿ (RangeDescriptor *rd = fRangeList; rd != ಶೂನ್ಯಸೂಚಕ; rd = rd->f_ಮುಂದಿನ) {
        int32_t range_ಅಂಕೆ = rd->f_ಅಂಕೆ;
        ಆದರೆ (range_ಅಂಕೆ == categories.ಎರಡನೇ) {
            rd->f_ಅಂಕೆ = categories.first;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (range_ಅಂಕೆ > categories.ಎರಡನೇ) {
            rd->f_ಅಂಕೆ--;
        }
    }
    --f_ಗುಂಪು_ಎಣಿಕೆ;
    ಆದರೆ (categories.ಎರಡನೇ <= f_Dict_Categories_ಆರಂಭ) {
        --f_Dict_Categories_ಆರಂಭ;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::get_Trie_ಗಾತ್ರ()  {
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (fTrie == ಶೂನ್ಯಸೂಚಕ) {
        ತಾರ್ಕಿಕ use8_ಬಿಟ್ಟುಗಳು = get_ಅಂಕೆ_Char_Categories() <= k_Max_Char_Categories_For8_ಬಿಟ್ಟುಗಳು_Trie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8_ಬಿಟ್ಟುಗಳು ? UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8 : UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16,
            f_ಸ್ಥಿತಿ);
        UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = *f_ಸ್ಥಿತಿ;
        f_Trie_ಗಾತ್ರ = ucptrie_ಗಮ್ಯ_ದ್ವಿಮಾನ(fTrie, ಶೂನ್ಯಸೂಚಕ, 0, &ಬಫರ್_ಸ್ಥಿತಿ);
        ಆದರೆ (ಬಫರ್_ಸ್ಥಿತಿ != U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR && U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)) {
            *f_ಸ್ಥಿತಿ = ಬಫರ್_ಸ್ಥಿತಿ;
        }
    }
    ಹಿಂತಿರುಗಿ f_Trie_ಗಾತ್ರ;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
ಶೂನ್ಯ RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_ಗಮ್ಯ_ದ್ವಿಮಾನ(fTrie,
                     where,                //ಬಫರ್
                     f_Trie_ಗಾತ್ರ,            //ಸಾಮರ್ಥ್ಯ
                     f_ಸ್ಥಿತಿ);
}

//------------------------------------------------------------------------
//
//  addValToSets     Add a runtime-mapped input value to each uset from a
//                   list of uset nodes. (val corresponds to a state table column.)
//                   For each of the original Unicode sets - which correspond
//                   directly to uset nodes - a logically equivalent expression
//                   is constructed in terms of the remapped runtime input
//                   symbol set.  This function adds one runtime input symbol to
//                   a list of sets.
//
//                   The "logically equivalent expression" is the tree for an
//                   or-ing together of all of the symbols that go into the set.
//
//------------------------------------------------------------------------
ಶೂನ್ಯ  RBBISetBuilder::ಕೂಡಿಸು_Val_ಗಮ್ಯ_Sets(UVector *sets, uint32_t val) {
    int32_t       ix;

    ಗಾಗಿ (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_ಗಂಟು = static_cast<RBBINode*>(sets->elementAt(ix));
        ಕೂಡಿಸು_Val_ಗಮ್ಯ_ಗಣ(uset_ಗಂಟು, val);
    }
}

ಶೂನ್ಯ  RBBISetBuilder::ಕೂಡಿಸು_Val_ಗಮ್ಯ_ಗಣ(RBBINode *uset_ಗಂಟು, uint32_t val) {
    RBBINode *ಎಲೆ_ಗಂಟು = ಹೊಸ RBBINode(RBBINode::ಎಲೆ_Char, *f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಅಳಿಸು ಎಲೆ_ಗಂಟು;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಎಲೆ_ಗಂಟು == ಶೂನ್ಯಸೂಚಕ) {
        *f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಎಲೆ_ಗಂಟು->fVal = static_cast<ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ>(val);
    ಆದರೆ (uset_ಗಂಟು->f_ಎಡ_ಮಗು == ಶೂನ್ಯಸೂಚಕ) {
        uset_ಗಂಟು->f_ಎಡ_ಮಗು = ಎಲೆ_ಗಂಟು;
        ಎಲೆ_ಗಂಟು->f_ಪೋಷಕ    = uset_ಗಂಟು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_ಗಂಟು = ಹೊಸ RBBINode(RBBINode::opOr, *f_ಸ್ಥಿತಿ);
        ಆದರೆ (or_ಗಂಟು == ಶೂನ್ಯಸೂಚಕ) {
            *f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
        ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
            ಅಳಿಸು or_ಗಂಟು;
            ಅಳಿಸು ಎಲೆ_ಗಂಟು;
            ಹಿಂತಿರುಗಿ;
        }
        or_ಗಂಟು->f_ಎಡ_ಮಗು  = uset_ಗಂಟು->f_ಎಡ_ಮಗು;
        or_ಗಂಟು->f_ಬಲ_ಮಗು = ಎಲೆ_ಗಂಟು;
        or_ಗಂಟು->f_ಎಡ_ಮಗು->f_ಪೋಷಕ  = or_ಗಂಟು;
        or_ಗಂಟು->f_ಬಲ_ಮಗು->f_ಪೋಷಕ = or_ಗಂಟು;
        uset_ಗಂಟು->f_ಎಡ_ಮಗು = or_ಗಂಟು;
        or_ಗಂಟು->f_ಪೋಷಕ = uset_ಗಂಟು;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_ಅಂಕೆ_Char_Categories() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ f_ಗುಂಪು_ಎಣಿಕೆ + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_Dict_Categories_ಆರಂಭ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ f_Dict_Categories_ಆರಂಭ;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::get_ಮೊದಲ_Char(int32_t category) ಸ್ಥಿರಾಂಕ {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    ಗಾಗಿ (rlRange = fRangeList; rlRange!=ಶೂನ್ಯಸೂಚಕ; rlRange=rlRange->f_ಮುಂದಿನ) {
        ಆದರೆ (rlRange->f_ಅಂಕೆ == category) {
            retVal = rlRange->f_ಆರಂಭ_Char;
            ನಿಲ್ಲಿಸು;
        }
    }
    ಹಿಂತಿರುಗಿ retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
ಶೂನ್ಯ RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    ಪೂರ್ಣಾಂಕ                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    ಗಾಗಿ (rlRange = fRangeList; rlRange!=ಶೂನ್ಯಸೂಚಕ; rlRange=rlRange->f_ಮುಂದಿನ) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->f_ಆರಂಭ_Char, rlRange->f_ಕೊನೆ_Char);

        ಗಾಗಿ (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_ಗಂಟು    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   ಗಣ_Name {u"anon"};
            RBBINode       *ಗಣ_Ref = uset_ಗಂಟು->f_ಪೋಷಕ;
            ಆದರೆ (ಗಣ_Ref != ಶೂನ್ಯಸೂಚಕ) {
                RBBINode *varRef = ಗಣ_Ref->f_ಪೋಷಕ;
                ಆದರೆ (varRef != ಶೂನ್ಯಸೂಚಕ  &&  varRef->fType == RBBINode::varRef) {
                    ಗಣ_Name = varRef->f_ಪಠ್ಯ;
                }
            }
            RBBI_DEBUG_printUnicodeString(ಗಣ_Name); RBBIDebugPrintf("  ");
        }
        RBBIDebugPrintf("\n");
    }
}
#ಆದರೆ_ಅಂತ್ಯ


//------------------------------------------------------------------------
//
//   printRangeGroups     A debugging function.
//                        dump out all of the range groups.
//
//------------------------------------------------------------------------
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
ಶೂನ್ಯ RBBISetBuilder::printRangeGroups() {
    ಪೂರ್ಣಾಂಕ                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    ಗಾಗಿ (RangeDescriptor *rlRange = fRangeList; rlRange!=ಶೂನ್ಯಸೂಚಕ; rlRange=rlRange->f_ಮುಂದಿನ) {
        ಆದರೆ (rlRange->f_ಮೊದಲ_ಒಳ_ಗುಂಪು) {
            ಪೂರ್ಣಾಂಕ ಗುಂಪು_ಅಂಕೆ = rlRange->f_ಅಂಕೆ;
            RBBIDebugPrintf("%2i  ", ಗುಂಪು_ಅಂಕೆ);

            ಆದರೆ (ಗುಂಪು_ಅಂಕೆ >= f_Dict_Categories_ಆರಂಭ) { RBBIDebugPrintf(" <DICT> ");}

            ಗಾಗಿ (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_ಗಂಟು    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   ಗಣ_Name = UNICODE_STRING("anon", 4);
                RBBINode       *ಗಣ_Ref = uset_ಗಂಟು->f_ಪೋಷಕ;
                ಆದರೆ (ಗಣ_Ref != ಶೂನ್ಯಸೂಚಕ) {
                    RBBINode *varRef = ಗಣ_Ref->f_ಪೋಷಕ;
                    ಆದರೆ (varRef != ಶೂನ್ಯಸೂಚಕ  &&  varRef->fType == RBBINode::varRef) {
                        ಗಣ_Name = varRef->f_ಪಠ್ಯ;
                    }
                }
                RBBI_DEBUG_printUnicodeString(ಗಣ_Name); RBBIDebugPrintf(" ");
            }

            i = 0;
            ಗಾಗಿ (RangeDescriptor *tRange = rlRange; tRange != ಶೂನ್ಯಸೂಚಕ; tRange = tRange->f_ಮುಂದಿನ) {
                ಆದರೆ (tRange->f_ಅಂಕೆ == rlRange->f_ಅಂಕೆ) {
                    ಆದರೆ (i++ % 5 == 0) {
                        RBBIDebugPrintf("\n    ");
                    }
                    RBBIDebugPrintf("  %05x-%05x", tRange->f_ಆರಂಭ_Char, tRange->f_ಕೊನೆ_Char);
                }
            }
            RBBIDebugPrintf("\n");
        }
    }
    RBBIDebugPrintf("\n");
}
#ಆದರೆ_ಅಂತ್ಯ


//------------------------------------------------------------------------
//
//   printSets          A debugging function.
//                      dump out all of the set definitions.
//
//------------------------------------------------------------------------
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
ಶೂನ್ಯ RBBISetBuilder::printSets() {
    ಪೂರ್ಣಾಂಕ                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    ಗಾಗಿ (i=0; ; i++) {
        RBBINode        *uset_ಗಂಟು;
        RBBINode        *ಗಣ_Ref;
        RBBINode        *varRef;
        UnicodeString    ಗಣ_Name;

        uset_ಗಂಟು = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        ಆದರೆ (uset_ಗಂಟು == ಶೂನ್ಯಸೂಚಕ) {
            ನಿಲ್ಲಿಸು;
        }

        RBBIDebugPrintf("%3d    ", i);
        ಗಣ_Name = UNICODE_STRING("anonymous", 9);
        ಗಣ_Ref = uset_ಗಂಟು->f_ಪೋಷಕ;
        ಆದರೆ (ಗಣ_Ref != ಶೂನ್ಯಸೂಚಕ) {
            varRef = ಗಣ_Ref->f_ಪೋಷಕ;
            ಆದರೆ (varRef != ಶೂನ್ಯಸೂಚಕ  &&  varRef->fType == RBBINode::varRef) {
                ಗಣ_Name = varRef->f_ಪಠ್ಯ;
            }
        }
        RBBI_DEBUG_printUnicodeString(ಗಣ_Name);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_ಗಂಟು->f_ಪಠ್ಯ);
        RBBIDebugPrintf("\n");
        ಆದರೆ (uset_ಗಂಟು->f_ಎಡ_ಮಗು != ಶೂನ್ಯಸೂಚಕ) {
            RBBINode::print_ವೃಕ್ಷ(uset_ಗಂಟು->f_ಎಡ_ಮಗು, ನಿಜ);
        }
    }
    RBBIDebugPrintf("\n");
}
#ಆದರೆ_ಅಂತ್ಯ



//-------------------------------------------------------------------------------------
//
//  RangeDescriptor copy constructor
//
//-------------------------------------------------------------------------------------

RangeDescriptor::RangeDescriptor(ಸ್ಥಿರಾಂಕ RangeDescriptor &other, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
        f_ಆರಂಭ_Char(other.f_ಆರಂಭ_Char), f_ಕೊನೆ_Char {other.f_ಕೊನೆ_Char}, f_ಅಂಕೆ {other.f_ಅಂಕೆ},
        fIncludesDict{other.fIncludesDict}, f_ಮೊದಲ_ಒಳ_ಗುಂಪು{other.f_ಮೊದಲ_ಒಳ_ಗುಂಪು} {

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    fIncludesSets = ಹೊಸ UVector(ಸ್ಥಿತಿ_2);
    ಆದರೆ (ಇದು->fIncludesSets == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        ಇದು->fIncludesSets->ಕೂಡಿಸು_Element(other.fIncludesSets->elementAt(i), ಸ್ಥಿತಿ_2);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    fIncludesSets = ಹೊಸ UVector(ಸ್ಥಿತಿ_2);
    ಆದರೆ (fIncludesSets == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor Destructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::~RangeDescriptor() {
    ಅಳಿಸು  fIncludesSets;
    fIncludesSets = ಶೂನ್ಯಸೂಚಕ;
}

//-------------------------------------------------------------------------------------
//
//  RangeDesriptor::split()
//
//-------------------------------------------------------------------------------------
ಶೂನ್ಯ RangeDescriptor::ವಿಭಜಿಸು(UChar32 where, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    U_ASSERT(where>f_ಆರಂಭ_Char && where<=f_ಕೊನೆ_Char);
    RangeDescriptor *nr = ಹೊಸ RangeDescriptor(*ಇದು, ಸ್ಥಿತಿ_2);
    ಆದರೆ(nr == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಅಳಿಸು nr;
        ಹಿಂತಿರುಗಿ;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->f_ಆರಂಭ_Char = where;
    ಇದು->f_ಕೊನೆ_Char = where-1;
    nr->f_ಮುಂದಿನ      = ಇದು->f_ಮುಂದಿನ;
    ಇದು->f_ಮುಂದಿನ    = nr;
}


//-------------------------------------------------------------------------------------
//
//   RangeDescriptor::isDictionaryRange
//
//            Test whether this range includes characters from
//            the original Unicode Set named "dictionary".
//
//            This function looks through the Unicode Sets that
//            the range includes, checking for one named "dictionary"
//
//            TODO:  a faster way would be to find the set node for
//                   "dictionary" just once, rather than looking it
//                   up by name every time.
//
//-------------------------------------------------------------------------------------
ತಾರ್ಕಿಕ RangeDescriptor::isDictionaryRange() {
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t *dictionary = u"dictionary";
    ಗಾಗಿ (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_ಗಂಟು = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *ಗಣ_Ref = uset_ಗಂಟು->f_ಪೋಷಕ;
        ಆದರೆ (ಗಣ_Ref != ಶೂನ್ಯಸೂಚಕ) {
            RBBINode *varRef = ಗಣ_Ref->f_ಪೋಷಕ;
            ಆದರೆ (varRef && varRef->fType == RBBINode::varRef) {
                ಸ್ಥಿರಾಂಕ UnicodeString *ಗಣ_Name = &varRef->f_ಪಠ್ಯ;
                ಆದರೆ (ಗಣ_Name->compare(dictionary, -1) == 0) {
                    ಹಿಂತಿರುಗಿ ನಿಜ;
                }
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */
