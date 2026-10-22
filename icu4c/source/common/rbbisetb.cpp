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

ध्रुव int32_t kMaxCharCategoriesFor8BitsTrie = 255;
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
    f_Group_गिनती     = 0;
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
    प्रति (nextRangeDesc = fRangeList; nextRangeDesc!=nullptr;) {
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
    RBBINode        *uset_गाँठ;
    RangeDescriptor *rlRange;

    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = नूतन RangeDescriptor(*fStatus); // will check for status here
    यदि (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    यदि (U_FAILURE(*fStatus)) {
        प्रत्यागम;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    पूर्णाङ्क  ni;
    प्रति (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_गाँठ = static_cast<RBBINode*>(एतत्->fRB->fUSetNodes->elementAt(ni));
        यदि (uset_गाँठ==nullptr) {
            भङ्ग;
        }

        UnicodeSet      *निवेश_Set             = uset_गाँठ->f_निवेश_Set;
        int32_t          निवेश_Set_Range_गिनती   = निवेश_Set->get_Range_गिनती();
        पूर्णाङ्क              निवेश_Set_Range_Index   = 0;
                         rlRange              = fRangeList;

        प्रति (;;) {
            यदि (निवेश_Set_Range_Index >= निवेश_Set_Range_गिनती) {
                भङ्ग;
            }
            UChar32      निवेश_Set_Range_Begin  = निवेश_Set->getRangeStart(निवेश_Set_Range_Index);
            UChar32      निवेश_Set_Range_End    = निवेश_Set->getRangeEnd(निवेश_Set_Range_Index);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            यावत् (rlRange->fEndChar < निवेश_Set_Range_Begin) {
                rlRange = rlRange->fNext;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            यदि (rlRange->fStartChar < निवेश_Set_Range_Begin) {
                rlRange->split(निवेश_Set_Range_Begin, *fStatus);
                यदि (U_FAILURE(*fStatus)) {
                    प्रत्यागम;
                }
                अनुवर्त;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            यदि (rlRange->fEndChar > निवेश_Set_Range_End) {
                rlRange->split(निवेश_Set_Range_End+1, *fStatus);
                यदि (U_FAILURE(*fStatus)) {
                    प्रत्यागम;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            यदि (rlRange->fIncludesSets->indexOf(uset_गाँठ) == -1) {
                rlRange->fIncludesSets->addElement(uset_गाँठ, *fStatus);
                यदि (U_FAILURE(*fStatus)) {
                    प्रत्यागम;
                }
            }

            // Advance over ranges that we are finished with.
            यदि (निवेश_Set_Range_End == rlRange->fEndChar) {
                निवेश_Set_Range_Index++;
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
    int32_t dict_Group_गिनती = 0;

    प्रति (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        प्रति (rl_खोज_Range=fRangeList; rl_खोज_Range != rlRange; rl_खोज_Range=rl_खोज_Range->fNext) {
            यदि (rlRange->fIncludesSets->equals(*rl_खोज_Range->fIncludesSets)) {
                rlRange->f_अंक = rl_खोज_Range->f_अंक;
                rlRange->fIncludesDict = rl_खोज_Range->fIncludesDict;
                भङ्ग;
            }
        }
        यदि (rlRange->f_अंक == 0) {
            rlRange->fFirstInGroup = सत्य;
            यदि (rlRange->isDictionaryRange()) {
                rlRange->f_अंक = ++dict_Group_गिनती;
                rlRange->fIncludesDict = सत्य;
            } अन्यथा {
                f_Group_गिनती++;
                rlRange->f_अंक = f_Group_गिनती+2;
                addValToSets(rlRange->fIncludesSets, rlRange->f_अंक);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_Group_गिनती + 3;
    प्रति (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        यदि (rlRange->fIncludesDict) {
            rlRange->f_अंक += fDictCategoriesStart - 1;
            यदि (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->f_अंक);
            }
        }
    }
    f_Group_गिनती += dict_Group_गिनती;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    प्रति (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_गाँठ = static_cast<RBBINode*>(एतत्->fRB->fUSetNodes->elementAt(ni));
        यदि (uset_गाँठ==nullptr) {
            भङ्ग;
        }
        UnicodeSet      *निवेश_Set = uset_गाँठ->f_निवेश_Set;
        यदि (निवेश_Set->contains(eofString)) {
            addValToSet(uset_गाँठ, 1);
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

    प्रति (RangeDescriptor *range = fRangeList; range!=nullptr && U_SUCCESS(*fStatus); range=range->fNext) {
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

    प्रति (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->fNext) {
        int32_t range_अंक = rd->f_अंक;
        यदि (range_अंक == categories.second) {
            rd->f_अंक = categories.first;
        } अन्यथा यदि (range_अंक > categories.second) {
            rd->f_अंक--;
        }
    }
    --f_Group_गिनती;
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
        प्रत्यागम 0;
    }
    यदि (fTrie == nullptr) {
        तार्किक use8Bits = get_अंक_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_मूल्य_BITS_8 : UCPTRIE_मूल्य_BITS_16,
            fStatus);
        UErrorCode bufferStatus = *fStatus;
        fTrieSize = ucptrie_to_द्विआधारी(fTrie, nullptr, 0, &bufferStatus);
        यदि (bufferStatus != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(bufferStatus)) {
            *fStatus = bufferStatus;
        }
    }
    प्रत्यागम fTrieSize;
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

    प्रति (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_गाँठ = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_गाँठ, val);
    }
}

शून्य  RBBISetBuilder::addValToSet(RBBINode *uset_गाँठ, uint32_t val) {
    RBBINode *leaf_गाँठ = नूतन RBBINode(RBBINode::leafChar, *fStatus);
    यदि (U_FAILURE(*fStatus)) {
        delete leaf_गाँठ;
        प्रत्यागम;
    }
    यदि (leaf_गाँठ == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    leaf_गाँठ->fVal = static_cast<unsigned short>(val);
    यदि (uset_गाँठ->f_बायाँ_Child == nullptr) {
        uset_गाँठ->f_बायाँ_Child = leaf_गाँठ;
        leaf_गाँठ->fParent    = uset_गाँठ;
    } अन्यथा {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_गाँठ = नूतन RBBINode(RBBINode::opOr, *fStatus);
        यदि (or_गाँठ == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        यदि (U_FAILURE(*fStatus)) {
            delete or_गाँठ;
            delete leaf_गाँठ;
            प्रत्यागम;
        }
        or_गाँठ->f_बायाँ_Child  = uset_गाँठ->f_बायाँ_Child;
        or_गाँठ->f_दायाँ_Child = leaf_गाँठ;
        or_गाँठ->f_बायाँ_Child->fParent  = or_गाँठ;
        or_गाँठ->f_दायाँ_Child->fParent = or_गाँठ;
        uset_गाँठ->f_बायाँ_Child = or_गाँठ;
        or_गाँठ->fParent = uset_गाँठ;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_अंक_Char_Categories() ध्रुव {
    प्रत्यागम f_Group_गिनती + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() ध्रुव {
    प्रत्यागम fDictCategoriesStart;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::getFirstChar(int32_t category) ध्रुव {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    प्रति (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        यदि (rlRange->f_अंक == category) {
            retVal = rlRange->fStartChar;
            भङ्ग;
        }
    }
    प्रत्यागम retVal;
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
    प्रति (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        प्रति (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_गाँठ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_गाँठ->fParent;
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
    प्रति (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        यदि (rlRange->fFirstInGroup) {
            पूर्णाङ्क group_अंक = rlRange->f_अंक;
            RBBIDebugPrintf("%2i  ", group_अंक);

            यदि (group_अंक >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            प्रति (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_गाँठ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_गाँठ->fParent;
                यदि (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    यदि (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            प्रति (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->fNext) {
                यदि (tRange->f_अंक == rlRange->f_अंक) {
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
    प्रति (i=0; ; i++) {
        RBBINode        *uset_गाँठ;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_गाँठ = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        यदि (uset_गाँठ == nullptr) {
            भङ्ग;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_गाँठ->fParent;
        यदि (setRef != nullptr) {
            varRef = setRef->fParent;
            यदि (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_गाँठ->fText);
        RBBIDebugPrintf("\n");
        यदि (uset_गाँठ->f_बायाँ_Child != nullptr) {
            RBBINode::print_वृक्ष(uset_गाँठ->f_बायाँ_Child, सत्य);
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

RangeDescriptor::RangeDescriptor(ध्रुव RangeDescriptor &other, UErrorCode &status) :
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_अंक {other.f_अंक},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }
    fIncludesSets = नूतन UVector(status);
    यदि (एतत्->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }

    प्रति (int32_t i=0; i<other.fIncludesSets->size(); i++) {
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
        प्रत्यागम;
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
        प्रत्यागम;
    }
    यदि (U_FAILURE(status)) {
        delete nr;
        प्रत्यागम;
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
    स्थिर ध्रुव char16_t *dictionary = u"dictionary";
    प्रति (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_गाँठ = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_गाँठ->fParent;
        यदि (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            यदि (varRef && varRef->fType == RBBINode::varRef) {
                ध्रुव UnicodeString *setName = &varRef->fText;
                यदि (setName->compare(dictionary, -1) == 0) {
                    प्रत्यागम सत्य;
                }
            }
        }
    }
    प्रत्यागम असत्य;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
