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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_BREAK_ITERATION

#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "uvector.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"

#અન્તર્ભાવ "rbbisetb.h"
#અન્તર્ભાવ "rbbinode.h"

U_NAMESPACE_BEGIN

અચલ int32_t kMaxCharCategoriesFor8BitsTrie = 255;
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
    f_Group_ગણતરી     = 0;
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
    માટે (nextRangeDesc = fRangeList; nextRangeDesc!=nullptr;) {
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
શૂન્ય RBBISetBuilder::buildRanges() {
    RBBINode        *uset_ગાંઠ;
    RangeDescriptor *rlRange;

    જો (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = નવું RangeDescriptor(*fStatus); // will check for status here
    જો (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    જો (U_FAILURE(*fStatus)) {
        પાછા;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    પૂર્ણાંક  ni;
    માટે (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ગાંઠ = static_cast<RBBINode*>(આ->fRB->fUSetNodes->elementAt(ni));
        જો (uset_ગાંઠ==nullptr) {
            તોડો;
        }

        UnicodeSet      *આગત_Set             = uset_ગાંઠ->f_આગત_Set;
        int32_t          આગત_Set_Range_ગણતરી   = આગત_Set->get_Range_ગણતરી();
        પૂર્ણાંક              આગત_Set_Range_Index   = 0;
                         rlRange              = fRangeList;

        માટે (;;) {
            જો (આગત_Set_Range_Index >= આગત_Set_Range_ગણતરી) {
                તોડો;
            }
            UChar32      આગત_Set_Range_Begin  = આગત_Set->getRangeStart(આગત_Set_Range_Index);
            UChar32      આગત_Set_Range_End    = આગત_Set->getRangeEnd(આગત_Set_Range_Index);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            જ્યારે (rlRange->fEndChar < આગત_Set_Range_Begin) {
                rlRange = rlRange->fNext;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            જો (rlRange->fStartChar < આગત_Set_Range_Begin) {
                rlRange->split(આગત_Set_Range_Begin, *fStatus);
                જો (U_FAILURE(*fStatus)) {
                    પાછા;
                }
                ચાલુ;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            જો (rlRange->fEndChar > આગત_Set_Range_End) {
                rlRange->split(આગત_Set_Range_End+1, *fStatus);
                જો (U_FAILURE(*fStatus)) {
                    પાછા;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            જો (rlRange->fIncludesSets->indexOf(uset_ગાંઠ) == -1) {
                rlRange->fIncludesSets->addElement(uset_ગાંઠ, *fStatus);
                જો (U_FAILURE(*fStatus)) {
                    પાછા;
                }
            }

            // Advance over ranges that we are finished with.
            જો (આગત_Set_Range_End == rlRange->fEndChar) {
                આગત_Set_Range_Index++;
            }
            rlRange = rlRange->fNext;
        }
    }

    જો (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    RangeDescriptor *rl_શોધ_Range;
    int32_t dict_Group_ગણતરી = 0;

    માટે (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        માટે (rl_શોધ_Range=fRangeList; rl_શોધ_Range != rlRange; rl_શોધ_Range=rl_શોધ_Range->fNext) {
            જો (rlRange->fIncludesSets->equals(*rl_શોધ_Range->fIncludesSets)) {
                rlRange->f_અંક = rl_શોધ_Range->f_અંક;
                rlRange->fIncludesDict = rl_શોધ_Range->fIncludesDict;
                તોડો;
            }
        }
        જો (rlRange->f_અંક == 0) {
            rlRange->fFirstInGroup = સાચું;
            જો (rlRange->isDictionaryRange()) {
                rlRange->f_અંક = ++dict_Group_ગણતરી;
                rlRange->fIncludesDict = સાચું;
            } નહીંતર {
                f_Group_ગણતરી++;
                rlRange->f_અંક = f_Group_ગણતરી+2;
                addValToSets(rlRange->fIncludesSets, rlRange->f_અંક);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_Group_ગણતરી + 3;
    માટે (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        જો (rlRange->fIncludesDict) {
            rlRange->f_અંક += fDictCategoriesStart - 1;
            જો (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->f_અંક);
            }
        }
    }
    f_Group_ગણતરી += dict_Group_ગણતરી;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    માટે (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ગાંઠ = static_cast<RBBINode*>(આ->fRB->fUSetNodes->elementAt(ni));
        જો (uset_ગાંઠ==nullptr) {
            તોડો;
        }
        UnicodeSet      *આગત_Set = uset_ગાંઠ->f_આગત_Set;
        જો (આગત_Set->contains(eofString)) {
            addValToSet(uset_ગાંઠ, 1);
        }
    }


    જો (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    જો (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
શૂન્ય RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        fStatus);

    માટે (RangeDescriptor *range = fRangeList; range!=nullptr && U_SUCCESS(*fStatus); range=range->fNext) {
        umutablecptrie_setRange(fMutableTrie,
                                range->fStartChar,     // Range start
                                range->fEndChar,       // Range end (inclusive)
                                range->f_અંક,           // value for range
                                fStatus);
    }
}


શૂન્ય RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    માટે (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->fNext) {
        int32_t range_અંક = rd->f_અંક;
        જો (range_અંક == categories.second) {
            rd->f_અંક = categories.first;
        } નહીંતર જો (range_અંક > categories.second) {
            rd->f_અંક--;
        }
    }
    --f_Group_ગણતરી;
    જો (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::getTrieSize()  {
    જો (U_FAILURE(*fStatus)) {
        પાછા 0;
    }
    જો (fTrie == nullptr) {
        તાર્કિક use8Bits = get_અંક_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_કિંમત_BITS_8 : UCPTRIE_કિંમત_BITS_16,
            fStatus);
        UErrorCode bufferStatus = *fStatus;
        fTrieSize = ucptrie_to_દ્વિઅંકી(fTrie, nullptr, 0, &bufferStatus);
        જો (bufferStatus != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(bufferStatus)) {
            *fStatus = bufferStatus;
        }
    }
    પાછા fTrieSize;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
શૂન્ય RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_to_દ્વિઅંકી(fTrie,
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
શૂન્ય  RBBISetBuilder::addValToSets(UVector *sets, uint32_t val) {
    int32_t       ix;

    માટે (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_ગાંઠ = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_ગાંઠ, val);
    }
}

શૂન્ય  RBBISetBuilder::addValToSet(RBBINode *uset_ગાંઠ, uint32_t val) {
    RBBINode *leaf_ગાંઠ = નવું RBBINode(RBBINode::leafChar, *fStatus);
    જો (U_FAILURE(*fStatus)) {
        delete leaf_ગાંઠ;
        પાછા;
    }
    જો (leaf_ગાંઠ == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    leaf_ગાંઠ->fVal = static_cast<unsigned short>(val);
    જો (uset_ગાંઠ->f_ડાબું_Child == nullptr) {
        uset_ગાંઠ->f_ડાબું_Child = leaf_ગાંઠ;
        leaf_ગાંઠ->fParent    = uset_ગાંઠ;
    } નહીંતર {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_ગાંઠ = નવું RBBINode(RBBINode::opOr, *fStatus);
        જો (or_ગાંઠ == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        જો (U_FAILURE(*fStatus)) {
            delete or_ગાંઠ;
            delete leaf_ગાંઠ;
            પાછા;
        }
        or_ગાંઠ->f_ડાબું_Child  = uset_ગાંઠ->f_ડાબું_Child;
        or_ગાંઠ->f_જમણું_Child = leaf_ગાંઠ;
        or_ગાંઠ->f_ડાબું_Child->fParent  = or_ગાંઠ;
        or_ગાંઠ->f_જમણું_Child->fParent = or_ગાંઠ;
        uset_ગાંઠ->f_ડાબું_Child = or_ગાંઠ;
        or_ગાંઠ->fParent = uset_ગાંઠ;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_અંક_Char_Categories() અચલ {
    પાછા f_Group_ગણતરી + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() અચલ {
    પાછા fDictCategoriesStart;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::getFirstChar(int32_t category) અચલ {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    માટે (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        જો (rlRange->f_અંક == category) {
            retVal = rlRange->fStartChar;
            તોડો;
        }
    }
    પાછા retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#ifdef RBBI_DEBUG
શૂન્ય RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    પૂર્ણાંક                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    માટે (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        માટે (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_ગાંઠ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_ગાંઠ->fParent;
            જો (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                જો (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
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
શૂન્ય RBBISetBuilder::printRangeGroups() {
    પૂર્ણાંક                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    માટે (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        જો (rlRange->fFirstInGroup) {
            પૂર્ણાંક group_અંક = rlRange->f_અંક;
            RBBIDebugPrintf("%2i  ", group_અંક);

            જો (group_અંક >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            માટે (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_ગાંઠ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_ગાંઠ->fParent;
                જો (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    જો (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            માટે (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->fNext) {
                જો (tRange->f_અંક == rlRange->f_અંક) {
                    જો (i++ % 5 == 0) {
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
શૂન્ય RBBISetBuilder::printSets() {
    પૂર્ણાંક                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    માટે (i=0; ; i++) {
        RBBINode        *uset_ગાંઠ;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_ગાંઠ = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        જો (uset_ગાંઠ == nullptr) {
            તોડો;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_ગાંઠ->fParent;
        જો (setRef != nullptr) {
            varRef = setRef->fParent;
            જો (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_ગાંઠ->fText);
        RBBIDebugPrintf("\n");
        જો (uset_ગાંઠ->f_ડાબું_Child != nullptr) {
            RBBINode::print_વૃક્ષ(uset_ગાંઠ->f_ડાબું_Child, સાચું);
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

RangeDescriptor::RangeDescriptor(અચલ RangeDescriptor &other, UErrorCode &status) :
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_અંક {other.f_અંક},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    જો (U_FAILURE(status)) {
        પાછા;
    }
    fIncludesSets = નવું UVector(status);
    જો (આ->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    જો (U_FAILURE(status)) {
        પાછા;
    }

    માટે (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        આ->fIncludesSets->addElement(other.fIncludesSets->elementAt(i), status);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UErrorCode &status) {
    જો (U_FAILURE(status)) {
        પાછા;
    }
    fIncludesSets = નવું UVector(status);
    જો (fIncludesSets == nullptr) {
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
શૂન્ય RangeDescriptor::split(UChar32 where, UErrorCode &status) {
    U_ASSERT(where>fStartChar && where<=fEndChar);
    RangeDescriptor *nr = નવું RangeDescriptor(*આ, status);
    જો(nr == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    જો (U_FAILURE(status)) {
        delete nr;
        પાછા;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->fStartChar = where;
    આ->fEndChar = where-1;
    nr->fNext      = આ->fNext;
    આ->fNext    = nr;
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
તાર્કિક RangeDescriptor::isDictionaryRange() {
    સ્થિર અચલ char16_t *dictionary = u"dictionary";
    માટે (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_ગાંઠ = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_ગાંઠ->fParent;
        જો (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            જો (varRef && varRef->fType == RBBINode::varRef) {
                અચલ UnicodeString *setName = &varRef->fText;
                જો (setName->compare(dictionary, -1) == 0) {
                    પાછા સાચું;
                }
            }
        }
    }
    પાછા ખોટું;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
