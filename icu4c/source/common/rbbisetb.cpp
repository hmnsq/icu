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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"

#ଅନ୍ତର୍ଭାଵ "rbbisetb.h"
#ଅନ୍ତର୍ଭାଵ "rbbinode.h"

U_NAMESPACE_BEGIN

ଅଚଳ int32_t kMaxCharCategoriesFor8BitsTrie = 255;
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
    f_Group_ଗଣନା     = 0;
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
    ପାଇଁ (nextRangeDesc = fRangeList; nextRangeDesc!=nullptr;) {
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
ଶୂନ୍ୟ RBBISetBuilder::buildRanges() {
    RBBINode        *uset_ଗଣ୍ଠି;
    RangeDescriptor *rlRange;

    ଯଦି (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = ନୂତନ RangeDescriptor(*fStatus); // will check for status here
    ଯଦି (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    ଯଦି (U_FAILURE(*fStatus)) {
        ଫେରନ୍ତୁ;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା  ni;
    ପାଇଁ (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ଗଣ୍ଠି = static_cast<RBBINode*>(ଏହି->fRB->fUSetNodes->elementAt(ni));
        ଯଦି (uset_ଗଣ୍ଠି==nullptr) {
            ଭାଙ୍ଗ;
        }

        UnicodeSet      *ନିବେଶ_Set             = uset_ଗଣ୍ଠି->f_ନିବେଶ_Set;
        int32_t          ନିବେଶ_Set_Range_ଗଣନା   = ନିବେଶ_Set->get_Range_ଗଣନା();
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା              ନିବେଶ_Set_Range_Index   = 0;
                         rlRange              = fRangeList;

        ପାଇଁ (;;) {
            ଯଦି (ନିବେଶ_Set_Range_Index >= ନିବେଶ_Set_Range_ଗଣନା) {
                ଭାଙ୍ଗ;
            }
            UChar32      ନିବେଶ_Set_Range_Begin  = ନିବେଶ_Set->getRangeStart(ନିବେଶ_Set_Range_Index);
            UChar32      ନିବେଶ_Set_Range_End    = ନିବେଶ_Set->getRangeEnd(ନିବେଶ_Set_Range_Index);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            ଯେତେବେଳେ (rlRange->fEndChar < ନିବେଶ_Set_Range_Begin) {
                rlRange = rlRange->fNext;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            ଯଦି (rlRange->fStartChar < ନିବେଶ_Set_Range_Begin) {
                rlRange->split(ନିବେଶ_Set_Range_Begin, *fStatus);
                ଯଦି (U_FAILURE(*fStatus)) {
                    ଫେରନ୍ତୁ;
                }
                ଜାରି;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            ଯଦି (rlRange->fEndChar > ନିବେଶ_Set_Range_End) {
                rlRange->split(ନିବେଶ_Set_Range_End+1, *fStatus);
                ଯଦି (U_FAILURE(*fStatus)) {
                    ଫେରନ୍ତୁ;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            ଯଦି (rlRange->fIncludesSets->indexOf(uset_ଗଣ୍ଠି) == -1) {
                rlRange->fIncludesSets->addElement(uset_ଗଣ୍ଠି, *fStatus);
                ଯଦି (U_FAILURE(*fStatus)) {
                    ଫେରନ୍ତୁ;
                }
            }

            // Advance over ranges that we are finished with.
            ଯଦି (ନିବେଶ_Set_Range_End == rlRange->fEndChar) {
                ନିବେଶ_Set_Range_Index++;
            }
            rlRange = rlRange->fNext;
        }
    }

    ଯଦି (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    RangeDescriptor *rl_ସନ୍ଧାନ_Range;
    int32_t dict_Group_ଗଣନା = 0;

    ପାଇଁ (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        ପାଇଁ (rl_ସନ୍ଧାନ_Range=fRangeList; rl_ସନ୍ଧାନ_Range != rlRange; rl_ସନ୍ଧାନ_Range=rl_ସନ୍ଧାନ_Range->fNext) {
            ଯଦି (rlRange->fIncludesSets->equals(*rl_ସନ୍ଧାନ_Range->fIncludesSets)) {
                rlRange->f_ଅଙ୍କ = rl_ସନ୍ଧାନ_Range->f_ଅଙ୍କ;
                rlRange->fIncludesDict = rl_ସନ୍ଧାନ_Range->fIncludesDict;
                ଭାଙ୍ଗ;
            }
        }
        ଯଦି (rlRange->f_ଅଙ୍କ == 0) {
            rlRange->fFirstInGroup = ସତ୍ୟ;
            ଯଦି (rlRange->isDictionaryRange()) {
                rlRange->f_ଅଙ୍କ = ++dict_Group_ଗଣନା;
                rlRange->fIncludesDict = ସତ୍ୟ;
            } ଅନ୍ୟଥା {
                f_Group_ଗଣନା++;
                rlRange->f_ଅଙ୍କ = f_Group_ଗଣନା+2;
                addValToSets(rlRange->fIncludesSets, rlRange->f_ଅଙ୍କ);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_Group_ଗଣନା + 3;
    ପାଇଁ (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        ଯଦି (rlRange->fIncludesDict) {
            rlRange->f_ଅଙ୍କ += fDictCategoriesStart - 1;
            ଯଦି (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->f_ଅଙ୍କ);
            }
        }
    }
    f_Group_ଗଣନା += dict_Group_ଗଣନା;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    ପାଇଁ (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ଗଣ୍ଠି = static_cast<RBBINode*>(ଏହି->fRB->fUSetNodes->elementAt(ni));
        ଯଦି (uset_ଗଣ୍ଠି==nullptr) {
            ଭାଙ୍ଗ;
        }
        UnicodeSet      *ନିବେଶ_Set = uset_ଗଣ୍ଠି->f_ନିବେଶ_Set;
        ଯଦି (ନିବେଶ_Set->contains(eofString)) {
            addValToSet(uset_ଗଣ୍ଠି, 1);
        }
    }


    ଯଦି (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    ଯଦି (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
ଶୂନ୍ୟ RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        fStatus);

    ପାଇଁ (RangeDescriptor *range = fRangeList; range!=nullptr && U_SUCCESS(*fStatus); range=range->fNext) {
        umutablecptrie_setRange(fMutableTrie,
                                range->fStartChar,     // Range start
                                range->fEndChar,       // Range end (inclusive)
                                range->f_ଅଙ୍କ,           // value for range
                                fStatus);
    }
}


ଶୂନ୍ୟ RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    ପାଇଁ (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->fNext) {
        int32_t range_ଅଙ୍କ = rd->f_ଅଙ୍କ;
        ଯଦି (range_ଅଙ୍କ == categories.second) {
            rd->f_ଅଙ୍କ = categories.first;
        } ଅନ୍ୟଥା ଯଦି (range_ଅଙ୍କ > categories.second) {
            rd->f_ଅଙ୍କ--;
        }
    }
    --f_Group_ଗଣନା;
    ଯଦି (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::getTrieSize()  {
    ଯଦି (U_FAILURE(*fStatus)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (fTrie == nullptr) {
        ତାର୍କିକ use8Bits = get_ଅଙ୍କ_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_ମୂଲ୍ୟ_BITS_8 : UCPTRIE_ମୂଲ୍ୟ_BITS_16,
            fStatus);
        UErrorCode bufferStatus = *fStatus;
        fTrieSize = ucptrie_to_ଦ୍ୱିମିକ(fTrie, nullptr, 0, &bufferStatus);
        ଯଦି (bufferStatus != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(bufferStatus)) {
            *fStatus = bufferStatus;
        }
    }
    ଫେରନ୍ତୁ fTrieSize;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
ଶୂନ୍ୟ RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_to_ଦ୍ୱିମିକ(fTrie,
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
ଶୂନ୍ୟ  RBBISetBuilder::addValToSets(UVector *sets, uint32_t val) {
    int32_t       ix;

    ପାଇଁ (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_ଗଣ୍ଠି = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_ଗଣ୍ଠି, val);
    }
}

ଶୂନ୍ୟ  RBBISetBuilder::addValToSet(RBBINode *uset_ଗଣ୍ଠି, uint32_t val) {
    RBBINode *leaf_ଗଣ୍ଠି = ନୂତନ RBBINode(RBBINode::leafChar, *fStatus);
    ଯଦି (U_FAILURE(*fStatus)) {
        delete leaf_ଗଣ୍ଠି;
        ଫେରନ୍ତୁ;
    }
    ଯଦି (leaf_ଗଣ୍ଠି == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    leaf_ଗଣ୍ଠି->fVal = static_cast<unsigned short>(val);
    ଯଦି (uset_ଗଣ୍ଠି->f_ବାମ_Child == nullptr) {
        uset_ଗଣ୍ଠି->f_ବାମ_Child = leaf_ଗଣ୍ଠି;
        leaf_ଗଣ୍ଠି->fParent    = uset_ଗଣ୍ଠି;
    } ଅନ୍ୟଥା {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_ଗଣ୍ଠି = ନୂତନ RBBINode(RBBINode::opOr, *fStatus);
        ଯଦି (or_ଗଣ୍ଠି == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        ଯଦି (U_FAILURE(*fStatus)) {
            delete or_ଗଣ୍ଠି;
            delete leaf_ଗଣ୍ଠି;
            ଫେରନ୍ତୁ;
        }
        or_ଗଣ୍ଠି->f_ବାମ_Child  = uset_ଗଣ୍ଠି->f_ବାମ_Child;
        or_ଗଣ୍ଠି->f_ଡାହାଣ_Child = leaf_ଗଣ୍ଠି;
        or_ଗଣ୍ଠି->f_ବାମ_Child->fParent  = or_ଗଣ୍ଠି;
        or_ଗଣ୍ଠି->f_ଡାହାଣ_Child->fParent = or_ଗଣ୍ଠି;
        uset_ଗଣ୍ଠି->f_ବାମ_Child = or_ଗଣ୍ଠି;
        or_ଗଣ୍ଠି->fParent = uset_ଗଣ୍ଠି;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_ଅଙ୍କ_Char_Categories() ଅଚଳ {
    ଫେରନ୍ତୁ f_Group_ଗଣନା + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() ଅଚଳ {
    ଫେରନ୍ତୁ fDictCategoriesStart;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::getFirstChar(int32_t category) ଅଚଳ {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    ପାଇଁ (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        ଯଦି (rlRange->f_ଅଙ୍କ == category) {
            retVal = rlRange->fStartChar;
            ଭାଙ୍ଗ;
        }
    }
    ଫେରନ୍ତୁ retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ଶୂନ୍ୟ RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    ପାଇଁ (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        ପାଇଁ (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_ଗଣ୍ଠି    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_ଗଣ୍ଠି->fParent;
            ଯଦି (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                ଯଦି (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
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
ଶୂନ୍ୟ RBBISetBuilder::printRangeGroups() {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    ପାଇଁ (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        ଯଦି (rlRange->fFirstInGroup) {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା group_ଅଙ୍କ = rlRange->f_ଅଙ୍କ;
            RBBIDebugPrintf("%2i  ", group_ଅଙ୍କ);

            ଯଦି (group_ଅଙ୍କ >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            ପାଇଁ (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_ଗଣ୍ଠି    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_ଗଣ୍ଠି->fParent;
                ଯଦି (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    ଯଦି (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            ପାଇଁ (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->fNext) {
                ଯଦି (tRange->f_ଅଙ୍କ == rlRange->f_ଅଙ୍କ) {
                    ଯଦି (i++ % 5 == 0) {
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
ଶୂନ୍ୟ RBBISetBuilder::printSets() {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    ପାଇଁ (i=0; ; i++) {
        RBBINode        *uset_ଗଣ୍ଠି;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_ଗଣ୍ଠି = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        ଯଦି (uset_ଗଣ୍ଠି == nullptr) {
            ଭାଙ୍ଗ;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_ଗଣ୍ଠି->fParent;
        ଯଦି (setRef != nullptr) {
            varRef = setRef->fParent;
            ଯଦି (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_ଗଣ୍ଠି->fText);
        RBBIDebugPrintf("\n");
        ଯଦି (uset_ଗଣ୍ଠି->f_ବାମ_Child != nullptr) {
            RBBINode::print_ବୃକ୍ଷ(uset_ଗଣ୍ଠି->f_ବାମ_Child, ସତ୍ୟ);
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

RangeDescriptor::RangeDescriptor(ଅଚଳ RangeDescriptor &other, UErrorCode &status) :
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_ଅଙ୍କ {other.f_ଅଙ୍କ},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    fIncludesSets = ନୂତନ UVector(status);
    ଯଦି (ଏହି->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    ପାଇଁ (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        ଏହି->fIncludesSets->addElement(other.fIncludesSets->elementAt(i), status);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    fIncludesSets = ନୂତନ UVector(status);
    ଯଦି (fIncludesSets == nullptr) {
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
ଶୂନ୍ୟ RangeDescriptor::split(UChar32 where, UErrorCode &status) {
    U_ASSERT(where>fStartChar && where<=fEndChar);
    RangeDescriptor *nr = ନୂତନ RangeDescriptor(*ଏହି, status);
    ଯଦି(nr == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    ଯଦି (U_FAILURE(status)) {
        delete nr;
        ଫେରନ୍ତୁ;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->fStartChar = where;
    ଏହି->fEndChar = where-1;
    nr->fNext      = ଏହି->fNext;
    ଏହି->fNext    = nr;
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
ତାର୍କିକ RangeDescriptor::isDictionaryRange() {
    ସ୍ଥିର ଅଚଳ char16_t *dictionary = u"dictionary";
    ପାଇଁ (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_ଗଣ୍ଠି = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_ଗଣ୍ଠି->fParent;
        ଯଦି (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            ଯଦି (varRef && varRef->fType == RBBINode::varRef) {
                ଅଚଳ UnicodeString *setName = &varRef->fText;
                ଯଦି (setName->compare(dictionary, -1) == 0) {
                    ଫେରନ୍ତୁ ସତ୍ୟ;
                }
            }
        }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
