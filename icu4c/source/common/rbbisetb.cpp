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

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"

#अन्तर्भाव "rbbisetb.h"
#अन्तर्भाव "rbbinode.h"

U_NAMESPACE_BEGIN

अचर int32_t kMaxCharCategoriesFor8BitsTrie = 255;
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
    f_Group_गन्ती     = 0;
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
    लागि (nextRangeDesc = fRangeList; nextRangeDesc!=nullptr;) {
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
    RBBINode        *uset_गाँठो;
    RangeDescriptor *rlRange;

    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = नूतन RangeDescriptor(*fStatus); // will check for status here
    यदि (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    पूर्णाङ्क  ni;
    लागि (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_गाँठो = static_cast<RBBINode*>(एतत्->fRB->fUSetNodes->elementAt(ni));
        यदि (uset_गाँठो==nullptr) {
            रोक;
        }

        UnicodeSet      *आगत_Set             = uset_गाँठो->f_आगत_Set;
        int32_t          आगत_Set_Range_गन्ती   = आगत_Set->get_Range_गन्ती();
        पूर्णाङ्क              आगत_Set_Range_Index   = 0;
                         rlRange              = fRangeList;

        लागि (;;) {
            यदि (आगत_Set_Range_Index >= आगत_Set_Range_गन्ती) {
                रोक;
            }
            UChar32      आगत_Set_Range_Begin  = आगत_Set->getRangeStart(आगत_Set_Range_Index);
            UChar32      आगत_Set_Range_End    = आगत_Set->getRangeEnd(आगत_Set_Range_Index);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            जबसम्म (rlRange->fEndChar < आगत_Set_Range_Begin) {
                rlRange = rlRange->fNext;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            यदि (rlRange->fStartChar < आगत_Set_Range_Begin) {
                rlRange->split(आगत_Set_Range_Begin, *fStatus);
                यदि (U_FAILURE(*fStatus)) {
                    फर्काऊ;
                }
                जारी;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            यदि (rlRange->fEndChar > आगत_Set_Range_End) {
                rlRange->split(आगत_Set_Range_End+1, *fStatus);
                यदि (U_FAILURE(*fStatus)) {
                    फर्काऊ;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            यदि (rlRange->fIncludesSets->indexOf(uset_गाँठो) == -1) {
                rlRange->fIncludesSets->addElement(uset_गाँठो, *fStatus);
                यदि (U_FAILURE(*fStatus)) {
                    फर्काऊ;
                }
            }

            // Advance over ranges that we are finished with.
            यदि (आगत_Set_Range_End == rlRange->fEndChar) {
                आगत_Set_Range_Index++;
            }
            rlRange = rlRange->fNext;
        }
    }

    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    RangeDescriptor *rl_खोज_Range;
    int32_t dict_Group_गन्ती = 0;

    लागि (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        लागि (rl_खोज_Range=fRangeList; rl_खोज_Range != rlRange; rl_खोज_Range=rl_खोज_Range->fNext) {
            यदि (rlRange->fIncludesSets->equals(*rl_खोज_Range->fIncludesSets)) {
                rlRange->f_अङ्क = rl_खोज_Range->f_अङ्क;
                rlRange->fIncludesDict = rl_खोज_Range->fIncludesDict;
                रोक;
            }
        }
        यदि (rlRange->f_अङ्क == 0) {
            rlRange->fFirstInGroup = सत्य;
            यदि (rlRange->isDictionaryRange()) {
                rlRange->f_अङ्क = ++dict_Group_गन्ती;
                rlRange->fIncludesDict = सत्य;
            } अन्यथा {
                f_Group_गन्ती++;
                rlRange->f_अङ्क = f_Group_गन्ती+2;
                addValToSets(rlRange->fIncludesSets, rlRange->f_अङ्क);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_Group_गन्ती + 3;
    लागि (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        यदि (rlRange->fIncludesDict) {
            rlRange->f_अङ्क += fDictCategoriesStart - 1;
            यदि (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->f_अङ्क);
            }
        }
    }
    f_Group_गन्ती += dict_Group_गन्ती;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    लागि (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_गाँठो = static_cast<RBBINode*>(एतत्->fRB->fUSetNodes->elementAt(ni));
        यदि (uset_गाँठो==nullptr) {
            रोक;
        }
        UnicodeSet      *आगत_Set = uset_गाँठो->f_आगत_Set;
        यदि (आगत_Set->contains(eofString)) {
            addValToSet(uset_गाँठो, 1);
        }
    }


    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
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

    लागि (RangeDescriptor *range = fRangeList; range!=nullptr && U_SUCCESS(*fStatus); range=range->fNext) {
        umutablecptrie_setRange(fMutableTrie,
                                range->fStartChar,     // Range start
                                range->fEndChar,       // Range end (inclusive)
                                range->f_अङ्क,           // value for range
                                fStatus);
    }
}


शून्य RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    लागि (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->fNext) {
        int32_t range_अङ्क = rd->f_अङ्क;
        यदि (range_अङ्क == categories.second) {
            rd->f_अङ्क = categories.first;
        } अन्यथा यदि (range_अङ्क > categories.second) {
            rd->f_अङ्क--;
        }
    }
    --f_Group_गन्ती;
    यदि (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::getTrieSize()  {
    यदि (U_FAILURE(*fStatus)) {
        फर्काऊ 0;
    }
    यदि (fTrie == nullptr) {
        तार्किक use8Bits = get_अङ्क_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_मान_BITS_8 : UCPTRIE_मान_BITS_16,
            fStatus);
        UErrorCode bufferStatus = *fStatus;
        fTrieSize = ucptrie_to_द्विआधारी(fTrie, nullptr, 0, &bufferStatus);
        यदि (bufferStatus != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(bufferStatus)) {
            *fStatus = bufferStatus;
        }
    }
    फर्काऊ fTrieSize;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
शून्य RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_to_द्विआधारी(fTrie,
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

    लागि (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_गाँठो = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_गाँठो, val);
    }
}

शून्य  RBBISetBuilder::addValToSet(RBBINode *uset_गाँठो, uint32_t val) {
    RBBINode *leaf_गाँठो = नूतन RBBINode(RBBINode::leafChar, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        delete leaf_गाँठो;
        फर्काऊ;
    }
    यदि (leaf_गाँठो == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    leaf_गाँठो->fVal = static_cast<अचिन्हित लघु>(val);
    यदि (uset_गाँठो->f_बायाँ_Child == nullptr) {
        uset_गाँठो->f_बायाँ_Child = leaf_गाँठो;
        leaf_गाँठो->fParent    = uset_गाँठो;
    } अन्यथा {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_गाँठो = नूतन RBBINode(RBBINode::opOr, *fStatus);
        यदि (or_गाँठो == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        यदि (U_FAILURE(*fStatus)) {
            delete or_गाँठो;
            delete leaf_गाँठो;
            फर्काऊ;
        }
        or_गाँठो->f_बायाँ_Child  = uset_गाँठो->f_बायाँ_Child;
        or_गाँठो->f_दायाँ_Child = leaf_गाँठो;
        or_गाँठो->f_बायाँ_Child->fParent  = or_गाँठो;
        or_गाँठो->f_दायाँ_Child->fParent = or_गाँठो;
        uset_गाँठो->f_बायाँ_Child = or_गाँठो;
        or_गाँठो->fParent = uset_गाँठो;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_अङ्क_Char_Categories() अचर {
    फर्काऊ f_Group_गन्ती + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() अचर {
    फर्काऊ fDictCategoriesStart;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::getFirstChar(int32_t category) अचर {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    लागि (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        यदि (rlRange->f_अङ्क == category) {
            retVal = rlRange->fStartChar;
            रोक;
        }
    }
    फर्काऊ retVal;
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
    पूर्णाङ्क                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    लागि (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        लागि (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_गाँठो    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_गाँठो->fParent;
            यदि (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                यदि (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
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
    पूर्णाङ्क                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    लागि (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        यदि (rlRange->fFirstInGroup) {
            पूर्णाङ्क group_अङ्क = rlRange->f_अङ्क;
            RBBIDebugPrintf("%2i  ", group_अङ्क);

            यदि (group_अङ्क >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            लागि (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_गाँठो    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_गाँठो->fParent;
                यदि (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    यदि (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            लागि (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->fNext) {
                यदि (tRange->f_अङ्क == rlRange->f_अङ्क) {
                    यदि (i++ % 5 == 0) {
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
    पूर्णाङ्क                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    लागि (i=0; ; i++) {
        RBBINode        *uset_गाँठो;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_गाँठो = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        यदि (uset_गाँठो == nullptr) {
            रोक;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_गाँठो->fParent;
        यदि (setRef != nullptr) {
            varRef = setRef->fParent;
            यदि (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_गाँठो->fText);
        RBBIDebugPrintf("\n");
        यदि (uset_गाँठो->f_बायाँ_Child != nullptr) {
            RBBINode::print_रूख(uset_गाँठो->f_बायाँ_Child, सत्य);
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

RangeDescriptor::RangeDescriptor(अचर RangeDescriptor &other, UErrorCode &status) :
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_अङ्क {other.f_अङ्क},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    fIncludesSets = नूतन UVector(status);
    यदि (एतत्->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }

    लागि (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        एतत्->fIncludesSets->addElement(other.fIncludesSets->elementAt(i), status);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UErrorCode &status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    fIncludesSets = नूतन UVector(status);
    यदि (fIncludesSets == nullptr) {
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
    RangeDescriptor *nr = नूतन RangeDescriptor(*एतत्, status);
    यदि(nr == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    यदि (U_FAILURE(status)) {
        delete nr;
        फर्काऊ;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->fStartChar = where;
    एतत्->fEndChar = where-1;
    nr->fNext      = एतत्->fNext;
    एतत्->fNext    = nr;
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
    स्थिर अचर char16_t *dictionary = u"dictionary";
    लागि (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_गाँठो = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_गाँठो->fParent;
        यदि (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            यदि (varRef && varRef->fType == RBBINode::varRef) {
                अचर UnicodeString *setName = &varRef->fText;
                यदि (setName->compare(dictionary, -1) == 0) {
                    फर्काऊ सत्य;
                }
            }
        }
    }
    फर्काऊ असत्य;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
