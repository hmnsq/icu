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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "uvector.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"

#അന്തര്ഭാവ "rbbisetb.h"
#അന്തര്ഭാവ "rbbinode.h"

U_NAMESPACE_BEGIN

സ്ഥിരം int32_t kMaxCharCategoriesFor8BitsTrie = 255;
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
    f_Group_എണ്ണം     = 0;
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
    വേണ്ടി (nextRangeDesc = fRangeList; nextRangeDesc!=nullptr;) {
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
ശൂന്യം RBBISetBuilder::buildRanges() {
    RBBINode        *uset_കണ്ണി;
    RangeDescriptor *rlRange;

    എങ്കിൽ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = പുതിയ RangeDescriptor(*fStatus); // will check for status here
    എങ്കിൽ (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    പൂർണ്ണസംഖ്യ  ni;
    വേണ്ടി (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_കണ്ണി = static_cast<RBBINode*>(ഇത്->fRB->fUSetNodes->elementAt(ni));
        എങ്കിൽ (uset_കണ്ണി==nullptr) {
            നിർത്തുക;
        }

        UnicodeSet      *നിവേശം_Set             = uset_കണ്ണി->f_നിവേശം_Set;
        int32_t          നിവേശം_Set_Range_എണ്ണം   = നിവേശം_Set->get_Range_എണ്ണം();
        പൂർണ്ണസംഖ്യ              നിവേശം_Set_Range_Index   = 0;
                         rlRange              = fRangeList;

        വേണ്ടി (;;) {
            എങ്കിൽ (നിവേശം_Set_Range_Index >= നിവേശം_Set_Range_എണ്ണം) {
                നിർത്തുക;
            }
            UChar32      നിവേശം_Set_Range_Begin  = നിവേശം_Set->getRangeStart(നിവേശം_Set_Range_Index);
            UChar32      നിവേശം_Set_Range_End    = നിവേശം_Set->getRangeEnd(നിവേശം_Set_Range_Index);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            ആകുമ്പോൾ (rlRange->fEndChar < നിവേശം_Set_Range_Begin) {
                rlRange = rlRange->fNext;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            എങ്കിൽ (rlRange->fStartChar < നിവേശം_Set_Range_Begin) {
                rlRange->split(നിവേശം_Set_Range_Begin, *fStatus);
                എങ്കിൽ (U_FAILURE(*fStatus)) {
                    തിരികെ;
                }
                തുടരുക;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            എങ്കിൽ (rlRange->fEndChar > നിവേശം_Set_Range_End) {
                rlRange->split(നിവേശം_Set_Range_End+1, *fStatus);
                എങ്കിൽ (U_FAILURE(*fStatus)) {
                    തിരികെ;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            എങ്കിൽ (rlRange->fIncludesSets->indexOf(uset_കണ്ണി) == -1) {
                rlRange->fIncludesSets->addElement(uset_കണ്ണി, *fStatus);
                എങ്കിൽ (U_FAILURE(*fStatus)) {
                    തിരികെ;
                }
            }

            // Advance over ranges that we are finished with.
            എങ്കിൽ (നിവേശം_Set_Range_End == rlRange->fEndChar) {
                നിവേശം_Set_Range_Index++;
            }
            rlRange = rlRange->fNext;
        }
    }

    എങ്കിൽ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    RangeDescriptor *rl_തിരയൽ_Range;
    int32_t dict_Group_എണ്ണം = 0;

    വേണ്ടി (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        വേണ്ടി (rl_തിരയൽ_Range=fRangeList; rl_തിരയൽ_Range != rlRange; rl_തിരയൽ_Range=rl_തിരയൽ_Range->fNext) {
            എങ്കിൽ (rlRange->fIncludesSets->equals(*rl_തിരയൽ_Range->fIncludesSets)) {
                rlRange->f_അക്കം = rl_തിരയൽ_Range->f_അക്കം;
                rlRange->fIncludesDict = rl_തിരയൽ_Range->fIncludesDict;
                നിർത്തുക;
            }
        }
        എങ്കിൽ (rlRange->f_അക്കം == 0) {
            rlRange->fFirstInGroup = സത്യം;
            എങ്കിൽ (rlRange->isDictionaryRange()) {
                rlRange->f_അക്കം = ++dict_Group_എണ്ണം;
                rlRange->fIncludesDict = സത്യം;
            } അല്ലെങ്കിൽ {
                f_Group_എണ്ണം++;
                rlRange->f_അക്കം = f_Group_എണ്ണം+2;
                addValToSets(rlRange->fIncludesSets, rlRange->f_അക്കം);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_Group_എണ്ണം + 3;
    വേണ്ടി (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        എങ്കിൽ (rlRange->fIncludesDict) {
            rlRange->f_അക്കം += fDictCategoriesStart - 1;
            എങ്കിൽ (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->f_അക്കം);
            }
        }
    }
    f_Group_എണ്ണം += dict_Group_എണ്ണം;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    വേണ്ടി (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_കണ്ണി = static_cast<RBBINode*>(ഇത്->fRB->fUSetNodes->elementAt(ni));
        എങ്കിൽ (uset_കണ്ണി==nullptr) {
            നിർത്തുക;
        }
        UnicodeSet      *നിവേശം_Set = uset_കണ്ണി->f_നിവേശം_Set;
        എങ്കിൽ (നിവേശം_Set->contains(eofString)) {
            addValToSet(uset_കണ്ണി, 1);
        }
    }


    എങ്കിൽ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    എങ്കിൽ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
ശൂന്യം RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        fStatus);

    വേണ്ടി (RangeDescriptor *range = fRangeList; range!=nullptr && U_SUCCESS(*fStatus); range=range->fNext) {
        umutablecptrie_setRange(fMutableTrie,
                                range->fStartChar,     // Range start
                                range->fEndChar,       // Range end (inclusive)
                                range->f_അക്കം,           // value for range
                                fStatus);
    }
}


ശൂന്യം RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    വേണ്ടി (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->fNext) {
        int32_t range_അക്കം = rd->f_അക്കം;
        എങ്കിൽ (range_അക്കം == categories.second) {
            rd->f_അക്കം = categories.first;
        } അല്ലെങ്കിൽ എങ്കിൽ (range_അക്കം > categories.second) {
            rd->f_അക്കം--;
        }
    }
    --f_Group_എണ്ണം;
    എങ്കിൽ (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::getTrieSize()  {
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        തിരികെ 0;
    }
    എങ്കിൽ (fTrie == nullptr) {
        തർക്കം use8Bits = get_അക്കം_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_മൂല്യം_BITS_8 : UCPTRIE_മൂല്യം_BITS_16,
            fStatus);
        UErrorCode bufferStatus = *fStatus;
        fTrieSize = ucptrie_to_ദ്വിമാന(fTrie, nullptr, 0, &bufferStatus);
        എങ്കിൽ (bufferStatus != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(bufferStatus)) {
            *fStatus = bufferStatus;
        }
    }
    തിരികെ fTrieSize;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
ശൂന്യം RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_to_ദ്വിമാന(fTrie,
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
ശൂന്യം  RBBISetBuilder::addValToSets(UVector *sets, uint32_t val) {
    int32_t       ix;

    വേണ്ടി (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_കണ്ണി = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_കണ്ണി, val);
    }
}

ശൂന്യം  RBBISetBuilder::addValToSet(RBBINode *uset_കണ്ണി, uint32_t val) {
    RBBINode *leaf_കണ്ണി = പുതിയ RBBINode(RBBINode::leafChar, *fStatus);
    എങ്കിൽ (U_FAILURE(*fStatus)) {
        delete leaf_കണ്ണി;
        തിരികെ;
    }
    എങ്കിൽ (leaf_കണ്ണി == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    leaf_കണ്ണി->fVal = static_cast<unsigned short>(val);
    എങ്കിൽ (uset_കണ്ണി->f_ഇടത്_Child == nullptr) {
        uset_കണ്ണി->f_ഇടത്_Child = leaf_കണ്ണി;
        leaf_കണ്ണി->fParent    = uset_കണ്ണി;
    } അല്ലെങ്കിൽ {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_കണ്ണി = പുതിയ RBBINode(RBBINode::opOr, *fStatus);
        എങ്കിൽ (or_കണ്ണി == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        എങ്കിൽ (U_FAILURE(*fStatus)) {
            delete or_കണ്ണി;
            delete leaf_കണ്ണി;
            തിരികെ;
        }
        or_കണ്ണി->f_ഇടത്_Child  = uset_കണ്ണി->f_ഇടത്_Child;
        or_കണ്ണി->f_വലത്_Child = leaf_കണ്ണി;
        or_കണ്ണി->f_ഇടത്_Child->fParent  = or_കണ്ണി;
        or_കണ്ണി->f_വലത്_Child->fParent = or_കണ്ണി;
        uset_കണ്ണി->f_ഇടത്_Child = or_കണ്ണി;
        or_കണ്ണി->fParent = uset_കണ്ണി;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_അക്കം_Char_Categories() സ്ഥിരം {
    തിരികെ f_Group_എണ്ണം + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() സ്ഥിരം {
    തിരികെ fDictCategoriesStart;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::getFirstChar(int32_t category) സ്ഥിരം {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    വേണ്ടി (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        എങ്കിൽ (rlRange->f_അക്കം == category) {
            retVal = rlRange->fStartChar;
            നിർത്തുക;
        }
    }
    തിരികെ retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ശൂന്യം RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    പൂർണ്ണസംഖ്യ                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    വേണ്ടി (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        വേണ്ടി (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_കണ്ണി    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_കണ്ണി->fParent;
            എങ്കിൽ (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                എങ്കിൽ (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
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
ശൂന്യം RBBISetBuilder::printRangeGroups() {
    പൂർണ്ണസംഖ്യ                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    വേണ്ടി (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        എങ്കിൽ (rlRange->fFirstInGroup) {
            പൂർണ്ണസംഖ്യ group_അക്കം = rlRange->f_അക്കം;
            RBBIDebugPrintf("%2i  ", group_അക്കം);

            എങ്കിൽ (group_അക്കം >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            വേണ്ടി (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_കണ്ണി    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_കണ്ണി->fParent;
                എങ്കിൽ (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    എങ്കിൽ (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            വേണ്ടി (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->fNext) {
                എങ്കിൽ (tRange->f_അക്കം == rlRange->f_അക്കം) {
                    എങ്കിൽ (i++ % 5 == 0) {
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
ശൂന്യം RBBISetBuilder::printSets() {
    പൂർണ്ണസംഖ്യ                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    വേണ്ടി (i=0; ; i++) {
        RBBINode        *uset_കണ്ണി;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_കണ്ണി = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        എങ്കിൽ (uset_കണ്ണി == nullptr) {
            നിർത്തുക;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_കണ്ണി->fParent;
        എങ്കിൽ (setRef != nullptr) {
            varRef = setRef->fParent;
            എങ്കിൽ (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_കണ്ണി->fText);
        RBBIDebugPrintf("\n");
        എങ്കിൽ (uset_കണ്ണി->f_ഇടത്_Child != nullptr) {
            RBBINode::print_വൃക്ഷം(uset_കണ്ണി->f_ഇടത്_Child, സത്യം);
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

RangeDescriptor::RangeDescriptor(സ്ഥിരം RangeDescriptor &other, UErrorCode &status) :
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_അക്കം {other.f_അക്കം},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    fIncludesSets = പുതിയ UVector(status);
    എങ്കിൽ (ഇത്->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    വേണ്ടി (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        ഇത്->fIncludesSets->addElement(other.fIncludesSets->elementAt(i), status);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    fIncludesSets = പുതിയ UVector(status);
    എങ്കിൽ (fIncludesSets == nullptr) {
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
ശൂന്യം RangeDescriptor::split(UChar32 where, UErrorCode &status) {
    U_ASSERT(where>fStartChar && where<=fEndChar);
    RangeDescriptor *nr = പുതിയ RangeDescriptor(*ഇത്, status);
    എങ്കിൽ(nr == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    എങ്കിൽ (U_FAILURE(status)) {
        delete nr;
        തിരികെ;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->fStartChar = where;
    ഇത്->fEndChar = where-1;
    nr->fNext      = ഇത്->fNext;
    ഇത്->fNext    = nr;
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
തർക്കം RangeDescriptor::isDictionaryRange() {
    സ്ഥിതം സ്ഥിരം char16_t *dictionary = u"dictionary";
    വേണ്ടി (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_കണ്ണി = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_കണ്ണി->fParent;
        എങ്കിൽ (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            എങ്കിൽ (varRef && varRef->fType == RBBINode::varRef) {
                സ്ഥിരം UnicodeString *setName = &varRef->fText;
                എങ്കിൽ (setName->compare(dictionary, -1) == 0) {
                    തിരികെ സത്യം;
                }
            }
        }
    }
    തിരികെ അസത്യം;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
