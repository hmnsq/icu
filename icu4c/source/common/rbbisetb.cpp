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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "uvector.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"

#অন্তর্ভাব "rbbisetb.h"
#অন্তর্ভাব "rbbinode.h"

U_NAMESPACE_BEGIN

ধ্রুবক int32_t kMaxCharCategoriesFor8BitsTrie = 255;
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
    f_Trie_আকার       = 0;
    f_Group_গুনতি     = 0;
}


//------------------------------------------------------------------------
//
//   Destructor
//
//------------------------------------------------------------------------
RBBISetBuilder::~RBBISetBuilder()
{
    RangeDescriptor   *পরবর্তী_Range_Desc;

    // Walk through & delete the linked list of RangeDescriptors
    জন্য (পরবর্তী_Range_Desc = fRangeList; পরবর্তী_Range_Desc!=nullptr;) {
        RangeDescriptor *r = পরবর্তী_Range_Desc;
        পরবর্তী_Range_Desc      = r->f_পরবর্তী;
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
শূন্য RBBISetBuilder::buildRanges() {
    RBBINode        *uset_গ্রন্থি;
    RangeDescriptor *rlRange;

    যদি (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = নতুন RangeDescriptor(*fStatus); // will check for status here
    যদি (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    যদি (U_FAILURE(*fStatus)) {
        ফেরত;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    পূর্ণসংখ্যা  ni;
    জন্য (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_গ্রন্থি = static_cast<RBBINode*>(এই->fRB->fUSetNodes->elementAt(ni));
        যদি (uset_গ্রন্থি==nullptr) {
            বিরতি;
        }

        UnicodeSet      *প্রবেশ_Set             = uset_গ্রন্থি->f_প্রবেশ_Set;
        int32_t          প্রবেশ_Set_Range_গুনতি   = প্রবেশ_Set->get_Range_গুনতি();
        পূর্ণসংখ্যা              প্রবেশ_Set_Range_সূচক   = 0;
                         rlRange              = fRangeList;

        জন্য (;;) {
            যদি (প্রবেশ_Set_Range_সূচক >= প্রবেশ_Set_Range_গুনতি) {
                বিরতি;
            }
            UChar32      প্রবেশ_Set_Range_Begin  = প্রবেশ_Set->getRangeStart(প্রবেশ_Set_Range_সূচক);
            UChar32      প্রবেশ_Set_Range_End    = প্রবেশ_Set->getRangeEnd(প্রবেশ_Set_Range_সূচক);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            যখন (rlRange->fEndChar < প্রবেশ_Set_Range_Begin) {
                rlRange = rlRange->f_পরবর্তী;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            যদি (rlRange->fStartChar < প্রবেশ_Set_Range_Begin) {
                rlRange->split(প্রবেশ_Set_Range_Begin, *fStatus);
                যদি (U_FAILURE(*fStatus)) {
                    ফেরত;
                }
                চালু;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            যদি (rlRange->fEndChar > প্রবেশ_Set_Range_End) {
                rlRange->split(প্রবেশ_Set_Range_End+1, *fStatus);
                যদি (U_FAILURE(*fStatus)) {
                    ফেরত;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            যদি (rlRange->fIncludesSets->সূচক_Of(uset_গ্রন্থি) == -1) {
                rlRange->fIncludesSets->addElement(uset_গ্রন্থি, *fStatus);
                যদি (U_FAILURE(*fStatus)) {
                    ফেরত;
                }
            }

            // Advance over ranges that we are finished with.
            যদি (প্রবেশ_Set_Range_End == rlRange->fEndChar) {
                প্রবেশ_Set_Range_সূচক++;
            }
            rlRange = rlRange->f_পরবর্তী;
        }
    }

    যদি (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    RangeDescriptor *rl_অনুসন্ধান_Range;
    int32_t dict_Group_গুনতি = 0;

    জন্য (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->f_পরবর্তী) {
        জন্য (rl_অনুসন্ধান_Range=fRangeList; rl_অনুসন্ধান_Range != rlRange; rl_অনুসন্ধান_Range=rl_অনুসন্ধান_Range->f_পরবর্তী) {
            যদি (rlRange->fIncludesSets->equals(*rl_অনুসন্ধান_Range->fIncludesSets)) {
                rlRange->f_অঙ্ক = rl_অনুসন্ধান_Range->f_অঙ্ক;
                rlRange->fIncludesDict = rl_অনুসন্ধান_Range->fIncludesDict;
                বিরতি;
            }
        }
        যদি (rlRange->f_অঙ্ক == 0) {
            rlRange->fFirstInGroup = সত্য;
            যদি (rlRange->isDictionaryRange()) {
                rlRange->f_অঙ্ক = ++dict_Group_গুনতি;
                rlRange->fIncludesDict = সত্য;
            } অন্যথা {
                f_Group_গুনতি++;
                rlRange->f_অঙ্ক = f_Group_গুনতি+2;
                addValToSets(rlRange->fIncludesSets, rlRange->f_অঙ্ক);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_Group_গুনতি + 3;
    জন্য (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->f_পরবর্তী) {
        যদি (rlRange->fIncludesDict) {
            rlRange->f_অঙ্ক += fDictCategoriesStart - 1;
            যদি (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->f_অঙ্ক);
            }
        }
    }
    f_Group_গুনতি += dict_Group_গুনতি;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    জন্য (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_গ্রন্থি = static_cast<RBBINode*>(এই->fRB->fUSetNodes->elementAt(ni));
        যদি (uset_গ্রন্থি==nullptr) {
            বিরতি;
        }
        UnicodeSet      *প্রবেশ_Set = uset_গ্রন্থি->f_প্রবেশ_Set;
        যদি (প্রবেশ_Set->contains(eofString)) {
            addValToSet(uset_গ্রন্থি, 1);
        }
    }


    যদি (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    যদি (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
শূন্য RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        fStatus);

    জন্য (RangeDescriptor *range = fRangeList; range!=nullptr && U_SUCCESS(*fStatus); range=range->f_পরবর্তী) {
        umutablecptrie_setRange(fMutableTrie,
                                range->fStartChar,     // Range start
                                range->fEndChar,       // Range end (inclusive)
                                range->f_অঙ্ক,           // value for range
                                fStatus);
    }
}


শূন্য RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    জন্য (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->f_পরবর্তী) {
        int32_t range_অঙ্ক = rd->f_অঙ্ক;
        যদি (range_অঙ্ক == categories.second) {
            rd->f_অঙ্ক = categories.first;
        } অন্যথা যদি (range_অঙ্ক > categories.second) {
            rd->f_অঙ্ক--;
        }
    }
    --f_Group_গুনতি;
    যদি (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::get_Trie_আকার()  {
    যদি (U_FAILURE(*fStatus)) {
        ফেরত 0;
    }
    যদি (fTrie == nullptr) {
        যৌক্তিক use8Bits = get_অঙ্ক_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_মান_BITS_8 : UCPTRIE_মান_BITS_16,
            fStatus);
        UErrorCode বাফার_Status = *fStatus;
        f_Trie_আকার = ucptrie_to_দ্বিমিক(fTrie, nullptr, 0, &বাফার_Status);
        যদি (বাফার_Status != U_বাফার_OVERFLOW_ERROR && U_FAILURE(বাফার_Status)) {
            *fStatus = বাফার_Status;
        }
    }
    ফেরত f_Trie_আকার;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
শূন্য RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_to_দ্বিমিক(fTrie,
                     where,                //বাফার
                     f_Trie_আকার,            // Capacity
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
শূন্য  RBBISetBuilder::addValToSets(UVector *sets, uint32_t val) {
    int32_t       ix;

    জন্য (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_গ্রন্থি = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_গ্রন্থি, val);
    }
}

শূন্য  RBBISetBuilder::addValToSet(RBBINode *uset_গ্রন্থি, uint32_t val) {
    RBBINode *leaf_গ্রন্থি = নতুন RBBINode(RBBINode::leafChar, *fStatus);
    যদি (U_FAILURE(*fStatus)) {
        delete leaf_গ্রন্থি;
        ফেরত;
    }
    যদি (leaf_গ্রন্থি == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    leaf_গ্রন্থি->fVal = static_cast<unsigned short>(val);
    যদি (uset_গ্রন্থি->f_বাম_Child == nullptr) {
        uset_গ্রন্থি->f_বাম_Child = leaf_গ্রন্থি;
        leaf_গ্রন্থি->fParent    = uset_গ্রন্থি;
    } অন্যথা {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_গ্রন্থি = নতুন RBBINode(RBBINode::opOr, *fStatus);
        যদি (or_গ্রন্থি == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        যদি (U_FAILURE(*fStatus)) {
            delete or_গ্রন্থি;
            delete leaf_গ্রন্থি;
            ফেরত;
        }
        or_গ্রন্থি->f_বাম_Child  = uset_গ্রন্থি->f_বাম_Child;
        or_গ্রন্থি->f_ডান_Child = leaf_গ্রন্থি;
        or_গ্রন্থি->f_বাম_Child->fParent  = or_গ্রন্থি;
        or_গ্রন্থি->f_ডান_Child->fParent = or_গ্রন্থি;
        uset_গ্রন্থি->f_বাম_Child = or_গ্রন্থি;
        or_গ্রন্থি->fParent = uset_গ্রন্থি;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_অঙ্ক_Char_Categories() ধ্রুবক {
    ফেরত f_Group_গুনতি + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() ধ্রুবক {
    ফেরত fDictCategoriesStart;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::getFirstChar(int32_t category) ধ্রুবক {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    জন্য (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->f_পরবর্তী) {
        যদি (rlRange->f_অঙ্ক == category) {
            retVal = rlRange->fStartChar;
            বিরতি;
        }
    }
    ফেরত retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#ifdef RBBI_DEBUG
শূন্য RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    পূর্ণসংখ্যা                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    জন্য (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->f_পরবর্তী) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        জন্য (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_গ্রন্থি    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_গ্রন্থি->fParent;
            যদি (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                যদি (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
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
শূন্য RBBISetBuilder::printRangeGroups() {
    পূর্ণসংখ্যা                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    জন্য (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->f_পরবর্তী) {
        যদি (rlRange->fFirstInGroup) {
            পূর্ণসংখ্যা group_অঙ্ক = rlRange->f_অঙ্ক;
            RBBIDebugPrintf("%2i  ", group_অঙ্ক);

            যদি (group_অঙ্ক >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            জন্য (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_গ্রন্থি    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_গ্রন্থি->fParent;
                যদি (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    যদি (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            জন্য (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->f_পরবর্তী) {
                যদি (tRange->f_অঙ্ক == rlRange->f_অঙ্ক) {
                    যদি (i++ % 5 == 0) {
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
শূন্য RBBISetBuilder::printSets() {
    পূর্ণসংখ্যা                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    জন্য (i=0; ; i++) {
        RBBINode        *uset_গ্রন্থি;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_গ্রন্থি = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        যদি (uset_গ্রন্থি == nullptr) {
            বিরতি;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_গ্রন্থি->fParent;
        যদি (setRef != nullptr) {
            varRef = setRef->fParent;
            যদি (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_গ্রন্থি->fText);
        RBBIDebugPrintf("\n");
        যদি (uset_গ্রন্থি->f_বাম_Child != nullptr) {
            RBBINode::print_বৃক্ষ(uset_গ্রন্থি->f_বাম_Child, সত্য);
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

RangeDescriptor::RangeDescriptor(ধ্রুবক RangeDescriptor &other, UErrorCode &status) :
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_অঙ্ক {other.f_অঙ্ক},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    fIncludesSets = নতুন UVector(status);
    যদি (এই->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    যদি (U_FAILURE(status)) {
        ফেরত;
    }

    জন্য (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        এই->fIncludesSets->addElement(other.fIncludesSets->elementAt(i), status);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    fIncludesSets = নতুন UVector(status);
    যদি (fIncludesSets == nullptr) {
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
শূন্য RangeDescriptor::split(UChar32 where, UErrorCode &status) {
    U_ASSERT(where>fStartChar && where<=fEndChar);
    RangeDescriptor *nr = নতুন RangeDescriptor(*এই, status);
    যদি(nr == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    যদি (U_FAILURE(status)) {
        delete nr;
        ফেরত;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->fStartChar = where;
    এই->fEndChar = where-1;
    nr->f_পরবর্তী      = এই->f_পরবর্তী;
    এই->f_পরবর্তী    = nr;
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
যৌক্তিক RangeDescriptor::isDictionaryRange() {
    স্থির ধ্রুবক char16_t *dictionary = u"dictionary";
    জন্য (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_গ্রন্থি = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_গ্রন্থি->fParent;
        যদি (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            যদি (varRef && varRef->fType == RBBINode::varRef) {
                ধ্রুবক UnicodeString *setName = &varRef->fText;
                যদি (setName->compare(dictionary, -1) == 0) {
                    ফেরত সত্য;
                }
            }
        }
    }
    ফেরত মিথ্যা;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
