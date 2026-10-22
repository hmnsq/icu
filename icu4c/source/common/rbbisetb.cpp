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

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"

#ಅನ್ತರ್ಭಾವ "rbbisetb.h"
#ಅನ್ತರ್ಭಾವ "rbbinode.h"

U_NAMESPACE_BEGIN

ಧ್ರುವ int32_t kMaxCharCategoriesFor8BitsTrie = 255;
//------------------------------------------------------------------------
//
//   Constructor
//
//------------------------------------------------------------------------
RBBISetBuilder::RBBISetBuilder(RBBIRuleBuilder *rb)
{
    fRB             = rb;
    fStatus         = rb->fStatus;
    fRangeList      = nullptr;
    fMutableTrie    = nullptr;
    fTrie           = nullptr;
    fTrieSize       = 0;
    f_Group_ಎಣಿಕೆ     = 0;
}


//------------------------------------------------------------------------
//
//   Destructor
//
//------------------------------------------------------------------------
RBBISetBuilder::~RBBISetBuilder()
{
    RangeDescriptor   *nextRangeDesc;

    // Walk through & delete the linked list of RangeDescriptors
    ಪ್ರತಿ (nextRangeDesc = fRangeList; nextRangeDesc!=nullptr;) {
        RangeDescriptor *r = nextRangeDesc;
        nextRangeDesc      = r->fNext;
        delete r;
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

    ಯದಿ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = ನೂತನ RangeDescriptor(*fStatus); // will check for status here
    ಯದಿ (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    ಪೂರ್ಣಾಙ್ಕ  ni;
    ಪ್ರತಿ (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ಗಂಟು = static_cast<RBBINode*>(ಏತತ್->fRB->fUSetNodes->elementAt(ni));
        ಯದಿ (uset_ಗಂಟು==nullptr) {
            ಭಙ್ಗ;
        }

        UnicodeSet      *ಒಳಹರಿವು_Set             = uset_ಗಂಟು->f_ಒಳಹರಿವು_Set;
        int32_t          ಒಳಹರಿವು_Set_Range_ಎಣಿಕೆ   = ಒಳಹರಿವು_Set->get_Range_ಎಣಿಕೆ();
        ಪೂರ್ಣಾಙ್ಕ              ಒಳಹರಿವು_Set_Range_Index   = 0;
                         rlRange              = fRangeList;

        ಪ್ರತಿ (;;) {
            ಯದಿ (ಒಳಹರಿವು_Set_Range_Index >= ಒಳಹರಿವು_Set_Range_ಎಣಿಕೆ) {
                ಭಙ್ಗ;
            }
            UChar32      ಒಳಹರಿವು_Set_Range_Begin  = ಒಳಹರಿವು_Set->getRangeStart(ಒಳಹರಿವು_Set_Range_Index);
            UChar32      ಒಳಹರಿವು_Set_Range_End    = ಒಳಹರಿವು_Set->getRangeEnd(ಒಳಹರಿವು_Set_Range_Index);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            ಯಾವತ್ (rlRange->fEndChar < ಒಳಹರಿವು_Set_Range_Begin) {
                rlRange = rlRange->fNext;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            ಯದಿ (rlRange->fStartChar < ಒಳಹರಿವು_Set_Range_Begin) {
                rlRange->split(ಒಳಹರಿವು_Set_Range_Begin, *fStatus);
                ಯದಿ (U_FAILURE(*fStatus)) {
                    ಪ್ರತ್ಯಾಗಮ;
                }
                ಅನುವರ್ತ;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            ಯದಿ (rlRange->fEndChar > ಒಳಹರಿವು_Set_Range_End) {
                rlRange->split(ಒಳಹರಿವು_Set_Range_End+1, *fStatus);
                ಯದಿ (U_FAILURE(*fStatus)) {
                    ಪ್ರತ್ಯಾಗಮ;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            ಯದಿ (rlRange->fIncludesSets->indexOf(uset_ಗಂಟು) == -1) {
                rlRange->fIncludesSets->addElement(uset_ಗಂಟು, *fStatus);
                ಯದಿ (U_FAILURE(*fStatus)) {
                    ಪ್ರತ್ಯಾಗಮ;
                }
            }

            // Advance over ranges that we are finished with.
            ಯದಿ (ಒಳಹರಿವು_Set_Range_End == rlRange->fEndChar) {
                ಒಳಹರಿವು_Set_Range_Index++;
            }
            rlRange = rlRange->fNext;
        }
    }

    ಯದಿ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    int32_t dict_Group_ಎಣಿಕೆ = 0;

    ಪ್ರತಿ (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        ಪ್ರತಿ (rl_ಹುಡುಕು_Range=fRangeList; rl_ಹುಡುಕು_Range != rlRange; rl_ಹುಡುಕು_Range=rl_ಹುಡುಕು_Range->fNext) {
            ಯದಿ (rlRange->fIncludesSets->equals(*rl_ಹುಡುಕು_Range->fIncludesSets)) {
                rlRange->f_ಅಂಕೆ = rl_ಹುಡುಕು_Range->f_ಅಂಕೆ;
                rlRange->fIncludesDict = rl_ಹುಡುಕು_Range->fIncludesDict;
                ಭಙ್ಗ;
            }
        }
        ಯದಿ (rlRange->f_ಅಂಕೆ == 0) {
            rlRange->fFirstInGroup = ಸತ್ಯ;
            ಯದಿ (rlRange->isDictionaryRange()) {
                rlRange->f_ಅಂಕೆ = ++dict_Group_ಎಣಿಕೆ;
                rlRange->fIncludesDict = ಸತ್ಯ;
            } ಅನ್ಯಥಾ {
                f_Group_ಎಣಿಕೆ++;
                rlRange->f_ಅಂಕೆ = f_Group_ಎಣಿಕೆ+2;
                addValToSets(rlRange->fIncludesSets, rlRange->f_ಅಂಕೆ);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_Group_ಎಣಿಕೆ + 3;
    ಪ್ರತಿ (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        ಯದಿ (rlRange->fIncludesDict) {
            rlRange->f_ಅಂಕೆ += fDictCategoriesStart - 1;
            ಯದಿ (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->f_ಅಂಕೆ);
            }
        }
    }
    f_Group_ಎಣಿಕೆ += dict_Group_ಎಣಿಕೆ;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    ಪ್ರತಿ (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ಗಂಟು = static_cast<RBBINode*>(ಏತತ್->fRB->fUSetNodes->elementAt(ni));
        ಯದಿ (uset_ಗಂಟು==nullptr) {
            ಭಙ್ಗ;
        }
        UnicodeSet      *ಒಳಹರಿವು_Set = uset_ಗಂಟು->f_ಒಳಹರಿವು_Set;
        ಯದಿ (ಒಳಹರಿವು_Set->contains(eofString)) {
            addValToSet(uset_ಗಂಟು, 1);
        }
    }


    ಯದಿ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    ಯದಿ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
ಶೂನ್ಯ RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        fStatus);

    ಪ್ರತಿ (RangeDescriptor *range = fRangeList; range!=nullptr && U_SUCCESS(*fStatus); range=range->fNext) {
        umutablecptrie_setRange(fMutableTrie,
                                range->fStartChar,     // Range start
                                range->fEndChar,       // Range end (inclusive)
                                range->f_ಅಂಕೆ,           // value for range
                                fStatus);
    }
}


ಶೂನ್ಯ RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    ಪ್ರತಿ (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->fNext) {
        int32_t range_ಅಂಕೆ = rd->f_ಅಂಕೆ;
        ಯದಿ (range_ಅಂಕೆ == categories.second) {
            rd->f_ಅಂಕೆ = categories.first;
        } ಅನ್ಯಥಾ ಯದಿ (range_ಅಂಕೆ > categories.second) {
            rd->f_ಅಂಕೆ--;
        }
    }
    --f_Group_ಎಣಿಕೆ;
    ಯದಿ (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::getTrieSize()  {
    ಯದಿ (U_FAILURE(*fStatus)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (fTrie == nullptr) {
        ತಾರ್ಕಿಕ use8Bits = get_ಅಂಕೆ_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_ಮೌಲ್ಯ_BITS_8 : UCPTRIE_ಮೌಲ್ಯ_BITS_16,
            fStatus);
        UErrorCode bufferStatus = *fStatus;
        fTrieSize = ucptrie_to_ದ್ವಿಮಾನ(fTrie, nullptr, 0, &bufferStatus);
        ಯದಿ (bufferStatus != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(bufferStatus)) {
            *fStatus = bufferStatus;
        }
    }
    ಪ್ರತ್ಯಾಗಮ fTrieSize;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
ಶೂನ್ಯ RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_to_ದ್ವಿಮಾನ(fTrie,
                     where,                // Buffer
                     fTrieSize,            // Capacity
                     fStatus);
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
ಶೂನ್ಯ  RBBISetBuilder::addValToSets(UVector *sets, uint32_t val) {
    int32_t       ix;

    ಪ್ರತಿ (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_ಗಂಟು = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_ಗಂಟು, val);
    }
}

ಶೂನ್ಯ  RBBISetBuilder::addValToSet(RBBINode *uset_ಗಂಟು, uint32_t val) {
    RBBINode *leaf_ಗಂಟು = ನೂತನ RBBINode(RBBINode::leafChar, *fStatus);
    ಯದಿ (U_FAILURE(*fStatus)) {
        delete leaf_ಗಂಟು;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (leaf_ಗಂಟು == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    leaf_ಗಂಟು->fVal = static_cast<unsigned short>(val);
    ಯದಿ (uset_ಗಂಟು->f_ಎಡ_Child == nullptr) {
        uset_ಗಂಟು->f_ಎಡ_Child = leaf_ಗಂಟು;
        leaf_ಗಂಟು->fParent    = uset_ಗಂಟು;
    } ಅನ್ಯಥಾ {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_ಗಂಟು = ನೂತನ RBBINode(RBBINode::opOr, *fStatus);
        ಯದಿ (or_ಗಂಟು == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        ಯದಿ (U_FAILURE(*fStatus)) {
            delete or_ಗಂಟು;
            delete leaf_ಗಂಟು;
            ಪ್ರತ್ಯಾಗಮ;
        }
        or_ಗಂಟು->f_ಎಡ_Child  = uset_ಗಂಟು->f_ಎಡ_Child;
        or_ಗಂಟು->f_ಬಲ_Child = leaf_ಗಂಟು;
        or_ಗಂಟು->f_ಎಡ_Child->fParent  = or_ಗಂಟು;
        or_ಗಂಟು->f_ಬಲ_Child->fParent = or_ಗಂಟು;
        uset_ಗಂಟು->f_ಎಡ_Child = or_ಗಂಟು;
        or_ಗಂಟು->fParent = uset_ಗಂಟು;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_ಅಂಕೆ_Char_Categories() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ f_Group_ಎಣಿಕೆ + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ fDictCategoriesStart;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::getFirstChar(int32_t category) ಧ್ರುವ {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    ಪ್ರತಿ (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        ಯದಿ (rlRange->f_ಅಂಕೆ == category) {
            retVal = rlRange->fStartChar;
            ಭಙ್ಗ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ಶೂನ್ಯ RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    ಪೂರ್ಣಾಙ್ಕ                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    ಪ್ರತಿ (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        ಪ್ರತಿ (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_ಗಂಟು    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_ಗಂಟು->fParent;
            ಯದಿ (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                ಯದಿ (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                    setName = varRef->fText;
                }
            }
            RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf("  ");
        }
        RBBIDebugPrintf("\n");
    }
}
#endif


//------------------------------------------------------------------------
//
//   printRangeGroups     A debugging function.
//                        dump out all of the range groups.
//
//------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ಶೂನ್ಯ RBBISetBuilder::printRangeGroups() {
    ಪೂರ್ಣಾಙ್ಕ                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    ಪ್ರತಿ (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        ಯದಿ (rlRange->fFirstInGroup) {
            ಪೂರ್ಣಾಙ್ಕ group_ಅಂಕೆ = rlRange->f_ಅಂಕೆ;
            RBBIDebugPrintf("%2i  ", group_ಅಂಕೆ);

            ಯದಿ (group_ಅಂಕೆ >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            ಪ್ರತಿ (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_ಗಂಟು    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_ಗಂಟು->fParent;
                ಯದಿ (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    ಯದಿ (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            ಪ್ರತಿ (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->fNext) {
                ಯದಿ (tRange->f_ಅಂಕೆ == rlRange->f_ಅಂಕೆ) {
                    ಯದಿ (i++ % 5 == 0) {
                        RBBIDebugPrintf("\n    ");
                    }
                    RBBIDebugPrintf("  %05x-%05x", tRange->fStartChar, tRange->fEndChar);
                }
            }
            RBBIDebugPrintf("\n");
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


//------------------------------------------------------------------------
//
//   printSets          A debugging function.
//                      dump out all of the set definitions.
//
//------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ಶೂನ್ಯ RBBISetBuilder::printSets() {
    ಪೂರ್ಣಾಙ್ಕ                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    ಪ್ರತಿ (i=0; ; i++) {
        RBBINode        *uset_ಗಂಟು;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_ಗಂಟು = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        ಯದಿ (uset_ಗಂಟು == nullptr) {
            ಭಙ್ಗ;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_ಗಂಟು->fParent;
        ಯದಿ (setRef != nullptr) {
            varRef = setRef->fParent;
            ಯದಿ (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_ಗಂಟು->fText);
        RBBIDebugPrintf("\n");
        ಯದಿ (uset_ಗಂಟು->f_ಎಡ_Child != nullptr) {
            RBBINode::print_ವೃಕ್ಷ(uset_ಗಂಟು->f_ಎಡ_Child, ಸತ್ಯ);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif



//-------------------------------------------------------------------------------------
//
//  RangeDescriptor copy constructor
//
//-------------------------------------------------------------------------------------

RangeDescriptor::RangeDescriptor(ಧ್ರುವ RangeDescriptor &other, UErrorCode &status) :
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_ಅಂಕೆ {other.f_ಅಂಕೆ},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    fIncludesSets = ನೂತನ UVector(status);
    ಯದಿ (ಏತತ್->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        ಏತತ್->fIncludesSets->addElement(other.fIncludesSets->elementAt(i), status);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    fIncludesSets = ನೂತನ UVector(status);
    ಯದಿ (fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor Destructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::~RangeDescriptor() {
    delete  fIncludesSets;
    fIncludesSets = nullptr;
}

//-------------------------------------------------------------------------------------
//
//  RangeDesriptor::split()
//
//-------------------------------------------------------------------------------------
ಶೂನ್ಯ RangeDescriptor::split(UChar32 where, UErrorCode &status) {
    U_ASSERT(where>fStartChar && where<=fEndChar);
    RangeDescriptor *nr = ನೂತನ RangeDescriptor(*ಏತತ್, status);
    ಯದಿ(nr == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (U_FAILURE(status)) {
        delete nr;
        ಪ್ರತ್ಯಾಗಮ;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->fStartChar = where;
    ಏತತ್->fEndChar = where-1;
    nr->fNext      = ಏತತ್->fNext;
    ಏತತ್->fNext    = nr;
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
    ಸ್ಥಿರ ಧ್ರುವ char16_t *dictionary = u"dictionary";
    ಪ್ರತಿ (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_ಗಂಟು = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_ಗಂಟು->fParent;
        ಯದಿ (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            ಯದಿ (varRef && varRef->fType == RBBINode::varRef) {
                ಧ್ರುವ UnicodeString *setName = &varRef->fText;
                ಯದಿ (setName->compare(dictionary, -1) == 0) {
                    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
                }
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
