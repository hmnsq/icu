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

#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"

#अन्तर्भाव "rbbisetb.h"
#अन्तर्भाव "rbbinode.h"

U_NAMESPACE_BEGIN

स्थिरांक int32_t kMaxCharCategoriesFor8BitsTrie = 255;
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
    f_Group_मोजणी     = 0;
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
    साठी (nextRangeDesc = fRangeList; nextRangeDesc!=nullptr;) {
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
शून्य RBBISetBuilder::buildRanges() {
    RBBINode        *uset_गाठ;
    RangeDescriptor *rlRange;

    जर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = नवीन RangeDescriptor(*fStatus); // will check for status here
    जर (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    जर (U_FAILURE(*fStatus)) {
        परत;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    पूर्णांक  ni;
    साठी (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_गाठ = static_cast<RBBINode*>(हे->fRB->fUSetNodes->elementAt(ni));
        जर (uset_गाठ==nullptr) {
            थांबा;
        }

        UnicodeSet      *आदान_Set             = uset_गाठ->f_आदान_Set;
        int32_t          आदान_Set_Range_मोजणी   = आदान_Set->get_Range_मोजणी();
        पूर्णांक              आदान_Set_Range_Index   = 0;
                         rlRange              = fRangeList;

        साठी (;;) {
            जर (आदान_Set_Range_Index >= आदान_Set_Range_मोजणी) {
                थांबा;
            }
            UChar32      आदान_Set_Range_Begin  = आदान_Set->getRangeStart(आदान_Set_Range_Index);
            UChar32      आदान_Set_Range_End    = आदान_Set->getRangeEnd(आदान_Set_Range_Index);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            पर्यंत (rlRange->fEndChar < आदान_Set_Range_Begin) {
                rlRange = rlRange->fNext;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            जर (rlRange->fStartChar < आदान_Set_Range_Begin) {
                rlRange->split(आदान_Set_Range_Begin, *fStatus);
                जर (U_FAILURE(*fStatus)) {
                    परत;
                }
                सुरूठेवा;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            जर (rlRange->fEndChar > आदान_Set_Range_End) {
                rlRange->split(आदान_Set_Range_End+1, *fStatus);
                जर (U_FAILURE(*fStatus)) {
                    परत;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            जर (rlRange->fIncludesSets->indexOf(uset_गाठ) == -1) {
                rlRange->fIncludesSets->addElement(uset_गाठ, *fStatus);
                जर (U_FAILURE(*fStatus)) {
                    परत;
                }
            }

            // Advance over ranges that we are finished with.
            जर (आदान_Set_Range_End == rlRange->fEndChar) {
                आदान_Set_Range_Index++;
            }
            rlRange = rlRange->fNext;
        }
    }

    जर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    RangeDescriptor *rl_शोध_Range;
    int32_t dict_Group_मोजणी = 0;

    साठी (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        साठी (rl_शोध_Range=fRangeList; rl_शोध_Range != rlRange; rl_शोध_Range=rl_शोध_Range->fNext) {
            जर (rlRange->fIncludesSets->equals(*rl_शोध_Range->fIncludesSets)) {
                rlRange->f_अंक = rl_शोध_Range->f_अंक;
                rlRange->fIncludesDict = rl_शोध_Range->fIncludesDict;
                थांबा;
            }
        }
        जर (rlRange->f_अंक == 0) {
            rlRange->fFirstInGroup = सत्य;
            जर (rlRange->isDictionaryRange()) {
                rlRange->f_अंक = ++dict_Group_मोजणी;
                rlRange->fIncludesDict = सत्य;
            } अन्यथा {
                f_Group_मोजणी++;
                rlRange->f_अंक = f_Group_मोजणी+2;
                addValToSets(rlRange->fIncludesSets, rlRange->f_अंक);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_Group_मोजणी + 3;
    साठी (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        जर (rlRange->fIncludesDict) {
            rlRange->f_अंक += fDictCategoriesStart - 1;
            जर (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->f_अंक);
            }
        }
    }
    f_Group_मोजणी += dict_Group_मोजणी;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    साठी (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_गाठ = static_cast<RBBINode*>(हे->fRB->fUSetNodes->elementAt(ni));
        जर (uset_गाठ==nullptr) {
            थांबा;
        }
        UnicodeSet      *आदान_Set = uset_गाठ->f_आदान_Set;
        जर (आदान_Set->contains(eofString)) {
            addValToSet(uset_गाठ, 1);
        }
    }


    जर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    जर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
शून्य RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        fStatus);

    साठी (RangeDescriptor *range = fRangeList; range!=nullptr && U_SUCCESS(*fStatus); range=range->fNext) {
        umutablecptrie_setRange(fMutableTrie,
                                range->fStartChar,     // Range start
                                range->fEndChar,       // Range end (inclusive)
                                range->f_अंक,           // value for range
                                fStatus);
    }
}


शून्य RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    साठी (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->fNext) {
        int32_t range_अंक = rd->f_अंक;
        जर (range_अंक == categories.second) {
            rd->f_अंक = categories.first;
        } अन्यथा जर (range_अंक > categories.second) {
            rd->f_अंक--;
        }
    }
    --f_Group_मोजणी;
    जर (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::getTrieSize()  {
    जर (U_FAILURE(*fStatus)) {
        परत 0;
    }
    जर (fTrie == nullptr) {
        तार्किक use8Bits = get_अंक_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_मूल्य_BITS_8 : UCPTRIE_मूल्य_BITS_16,
            fStatus);
        UErrorCode bufferStatus = *fStatus;
        fTrieSize = ucptrie_to_द्विमान(fTrie, nullptr, 0, &bufferStatus);
        जर (bufferStatus != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(bufferStatus)) {
            *fStatus = bufferStatus;
        }
    }
    परत fTrieSize;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
शून्य RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_to_द्विमान(fTrie,
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
शून्य  RBBISetBuilder::addValToSets(UVector *sets, uint32_t val) {
    int32_t       ix;

    साठी (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_गाठ = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_गाठ, val);
    }
}

शून्य  RBBISetBuilder::addValToSet(RBBINode *uset_गाठ, uint32_t val) {
    RBBINode *leaf_गाठ = नवीन RBBINode(RBBINode::leafChar, *fStatus);
    जर (U_FAILURE(*fStatus)) {
        delete leaf_गाठ;
        परत;
    }
    जर (leaf_गाठ == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    leaf_गाठ->fVal = static_cast<अचिन्हित लघु>(val);
    जर (uset_गाठ->f_डावा_Child == nullptr) {
        uset_गाठ->f_डावा_Child = leaf_गाठ;
        leaf_गाठ->fParent    = uset_गाठ;
    } अन्यथा {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_गाठ = नवीन RBBINode(RBBINode::opOr, *fStatus);
        जर (or_गाठ == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        जर (U_FAILURE(*fStatus)) {
            delete or_गाठ;
            delete leaf_गाठ;
            परत;
        }
        or_गाठ->f_डावा_Child  = uset_गाठ->f_डावा_Child;
        or_गाठ->f_उजवा_Child = leaf_गाठ;
        or_गाठ->f_डावा_Child->fParent  = or_गाठ;
        or_गाठ->f_उजवा_Child->fParent = or_गाठ;
        uset_गाठ->f_डावा_Child = or_गाठ;
        or_गाठ->fParent = uset_गाठ;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_अंक_Char_Categories() स्थिरांक {
    परत f_Group_मोजणी + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() स्थिरांक {
    परत fDictCategoriesStart;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::getFirstChar(int32_t category) स्थिरांक {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    साठी (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        जर (rlRange->f_अंक == category) {
            retVal = rlRange->fStartChar;
            थांबा;
        }
    }
    परत retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#ifdef RBBI_DEBUG
शून्य RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    पूर्णांक                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    साठी (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        साठी (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_गाठ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_गाठ->fParent;
            जर (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                जर (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
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
शून्य RBBISetBuilder::printRangeGroups() {
    पूर्णांक                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    साठी (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        जर (rlRange->fFirstInGroup) {
            पूर्णांक group_अंक = rlRange->f_अंक;
            RBBIDebugPrintf("%2i  ", group_अंक);

            जर (group_अंक >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            साठी (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_गाठ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_गाठ->fParent;
                जर (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    जर (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            साठी (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->fNext) {
                जर (tRange->f_अंक == rlRange->f_अंक) {
                    जर (i++ % 5 == 0) {
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
शून्य RBBISetBuilder::printSets() {
    पूर्णांक                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    साठी (i=0; ; i++) {
        RBBINode        *uset_गाठ;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_गाठ = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        जर (uset_गाठ == nullptr) {
            थांबा;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_गाठ->fParent;
        जर (setRef != nullptr) {
            varRef = setRef->fParent;
            जर (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_गाठ->fText);
        RBBIDebugPrintf("\n");
        जर (uset_गाठ->f_डावा_Child != nullptr) {
            RBBINode::print_वृक्ष(uset_गाठ->f_डावा_Child, सत्य);
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

RangeDescriptor::RangeDescriptor(स्थिरांक RangeDescriptor &other, UErrorCode &status) :
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_अंक {other.f_अंक},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    जर (U_FAILURE(status)) {
        परत;
    }
    fIncludesSets = नवीन UVector(status);
    जर (हे->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    जर (U_FAILURE(status)) {
        परत;
    }

    साठी (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        हे->fIncludesSets->addElement(other.fIncludesSets->elementAt(i), status);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UErrorCode &status) {
    जर (U_FAILURE(status)) {
        परत;
    }
    fIncludesSets = नवीन UVector(status);
    जर (fIncludesSets == nullptr) {
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
शून्य RangeDescriptor::split(UChar32 where, UErrorCode &status) {
    U_ASSERT(where>fStartChar && where<=fEndChar);
    RangeDescriptor *nr = नवीन RangeDescriptor(*हे, status);
    जर(nr == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    जर (U_FAILURE(status)) {
        delete nr;
        परत;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->fStartChar = where;
    हे->fEndChar = where-1;
    nr->fNext      = हे->fNext;
    हे->fNext    = nr;
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
तार्किक RangeDescriptor::isDictionaryRange() {
    स्थिर स्थिरांक char16_t *dictionary = u"dictionary";
    साठी (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_गाठ = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_गाठ->fParent;
        जर (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            जर (varRef && varRef->fType == RBBINode::varRef) {
                स्थिरांक UnicodeString *setName = &varRef->fText;
                जर (setName->compare(dictionary, -1) == 0) {
                    परत सत्य;
                }
            }
        }
    }
    परत असत्य;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
