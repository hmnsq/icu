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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "unicode/uniset.h"
#समावेश "uvector.h"
#समावेश "uassert.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"

#समावेश "rbbisetb.h"
#समावेश "rbbinode.h"

U_NAMESPACE_BEGIN

स्थिर int32_t kMaxCharCategoriesFor8BitsTrie = 255;
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
    f_Trie_आकार       = 0;
    f_Group_गिनती     = 0;
}


//------------------------------------------------------------------------
//
//   Destructor
//
//------------------------------------------------------------------------
RBBISetBuilder::~RBBISetBuilder()
{
    RangeDescriptor   *अग्र_Range_Desc;

    // Walk through & delete the linked list of RangeDescriptors
    क्रम (अग्र_Range_Desc = fRangeList; अग्र_Range_Desc!=nullptr;) {
        RangeDescriptor *r = अग्र_Range_Desc;
        अग्र_Range_Desc      = r->f_अग्र;
        मिटाओ r;
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
व्योम RBBISetBuilder::buildRanges() {
    RBBINode        *uset_गाँठ;
    RangeDescriptor *rlRange;

    अगर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = नया RangeDescriptor(*fStatus); // will check for status here
    अगर (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    पूर्णांक  ni;
    क्रम (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_गाँठ = static_cast<RBBINode*>(यह->fRB->fUSetNodes->elementAt(ni));
        अगर (uset_गाँठ==nullptr) {
            अवरोध;
        }

        UnicodeSet      *निवेश_Set             = uset_गाँठ->f_निवेश_Set;
        int32_t          निवेश_Set_Range_गिनती   = निवेश_Set->get_Range_गिनती();
        पूर्णांक              निवेश_Set_Range_सूचकांक   = 0;
                         rlRange              = fRangeList;

        क्रम (;;) {
            अगर (निवेश_Set_Range_सूचकांक >= निवेश_Set_Range_गिनती) {
                अवरोध;
            }
            UChar32      निवेश_Set_Range_Begin  = निवेश_Set->getRangeStart(निवेश_Set_Range_सूचकांक);
            UChar32      निवेश_Set_Range_End    = निवेश_Set->getRangeEnd(निवेश_Set_Range_सूचकांक);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            जबतक (rlRange->fEndChar < निवेश_Set_Range_Begin) {
                rlRange = rlRange->f_अग्र;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            अगर (rlRange->fStartChar < निवेश_Set_Range_Begin) {
                rlRange->split(निवेश_Set_Range_Begin, *fStatus);
                अगर (U_FAILURE(*fStatus)) {
                    वापस;
                }
                जारी;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            अगर (rlRange->fEndChar > निवेश_Set_Range_End) {
                rlRange->split(निवेश_Set_Range_End+1, *fStatus);
                अगर (U_FAILURE(*fStatus)) {
                    वापस;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            अगर (rlRange->fIncludesSets->सूचकांक_Of(uset_गाँठ) == -1) {
                rlRange->fIncludesSets->addElement(uset_गाँठ, *fStatus);
                अगर (U_FAILURE(*fStatus)) {
                    वापस;
                }
            }

            // Advance over ranges that we are finished with.
            अगर (निवेश_Set_Range_End == rlRange->fEndChar) {
                निवेश_Set_Range_सूचकांक++;
            }
            rlRange = rlRange->f_अग्र;
        }
    }

    अगर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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

    क्रम (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->f_अग्र) {
        क्रम (rl_खोज_Range=fRangeList; rl_खोज_Range != rlRange; rl_खोज_Range=rl_खोज_Range->f_अग्र) {
            अगर (rlRange->fIncludesSets->equals(*rl_खोज_Range->fIncludesSets)) {
                rlRange->f_अंक = rl_खोज_Range->f_अंक;
                rlRange->fIncludesDict = rl_खोज_Range->fIncludesDict;
                अवरोध;
            }
        }
        अगर (rlRange->f_अंक == 0) {
            rlRange->fFirstInGroup = true;
            अगर (rlRange->isDictionaryRange()) {
                rlRange->f_अंक = ++dict_Group_गिनती;
                rlRange->fIncludesDict = true;
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
    क्रम (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->f_अग्र) {
        अगर (rlRange->fIncludesDict) {
            rlRange->f_अंक += fDictCategoriesStart - 1;
            अगर (rlRange->fFirstInGroup) {
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
    क्रम (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_गाँठ = static_cast<RBBINode*>(यह->fRB->fUSetNodes->elementAt(ni));
        अगर (uset_गाँठ==nullptr) {
            अवरोध;
        }
        UnicodeSet      *निवेश_Set = uset_गाँठ->f_निवेश_Set;
        अगर (निवेश_Set->contains(eofString)) {
            addValToSet(uset_गाँठ, 1);
        }
    }


    अगर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    अगर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
व्योम RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        fStatus);

    क्रम (RangeDescriptor *range = fRangeList; range!=nullptr && U_SUCCESS(*fStatus); range=range->f_अग्र) {
        umutablecptrie_setRange(fMutableTrie,
                                range->fStartChar,     // Range start
                                range->fEndChar,       // Range end (inclusive)
                                range->f_अंक,           // value for range
                                fStatus);
    }
}


व्योम RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    क्रम (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->f_अग्र) {
        int32_t range_अंक = rd->f_अंक;
        अगर (range_अंक == categories.second) {
            rd->f_अंक = categories.first;
        } अन्यथा अगर (range_अंक > categories.second) {
            rd->f_अंक--;
        }
    }
    --f_Group_गिनती;
    अगर (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::get_Trie_आकार()  {
    अगर (U_FAILURE(*fStatus)) {
        वापस 0;
    }
    अगर (fTrie == nullptr) {
        bool use8Bits = get_अंक_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_मूल्य_BITS_8 : UCPTRIE_मूल्य_BITS_16,
            fStatus);
        UErrorCode बफर_Status = *fStatus;
        f_Trie_आकार = ucptrie_to_द्विआधारी(fTrie, nullptr, 0, &बफर_Status);
        अगर (बफर_Status != U_बफर_OVERFLOW_ERROR && U_FAILURE(बफर_Status)) {
            *fStatus = बफर_Status;
        }
    }
    वापस f_Trie_आकार;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
व्योम RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_to_द्विआधारी(fTrie,
                     where,                //बफर
                     f_Trie_आकार,            // Capacity
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
व्योम  RBBISetBuilder::addValToSets(UVector *sets, uint32_t val) {
    int32_t       ix;

    क्रम (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_गाँठ = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_गाँठ, val);
    }
}

व्योम  RBBISetBuilder::addValToSet(RBBINode *uset_गाँठ, uint32_t val) {
    RBBINode *leaf_गाँठ = नया RBBINode(RBBINode::leafChar, *fStatus);
    अगर (U_FAILURE(*fStatus)) {
        मिटाओ leaf_गाँठ;
        वापस;
    }
    अगर (leaf_गाँठ == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    leaf_गाँठ->fVal = static_cast<अचिन्हित लघु>(val);
    अगर (uset_गाँठ->f_बायाँ_Child == nullptr) {
        uset_गाँठ->f_बायाँ_Child = leaf_गाँठ;
        leaf_गाँठ->fParent    = uset_गाँठ;
    } अन्यथा {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_गाँठ = नया RBBINode(RBBINode::opOr, *fStatus);
        अगर (or_गाँठ == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        अगर (U_FAILURE(*fStatus)) {
            मिटाओ or_गाँठ;
            मिटाओ leaf_गाँठ;
            वापस;
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
int32_t  RBBISetBuilder::get_अंक_Char_Categories() स्थिर {
    वापस f_Group_गिनती + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() स्थिर {
    वापस fDictCategoriesStart;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::getFirstChar(int32_t category) स्थिर {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    क्रम (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->f_अग्र) {
        अगर (rlRange->f_अंक == category) {
            retVal = rlRange->fStartChar;
            अवरोध;
        }
    }
    वापस retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#अगर_घोषित RBBI_DEBUG
व्योम RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    पूर्णांक                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    क्रम (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->f_अग्र) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        क्रम (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_गाँठ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_गाँठ->fParent;
            अगर (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                अगर (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                    setName = varRef->fText;
                }
            }
            RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf("  ");
        }
        RBBIDebugPrintf("\n");
    }
}
#पूर्ण_अगर


//------------------------------------------------------------------------
//
//   printRangeGroups     A debugging function.
//                        dump out all of the range groups.
//
//------------------------------------------------------------------------
#अगर_घोषित RBBI_DEBUG
व्योम RBBISetBuilder::printRangeGroups() {
    पूर्णांक                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    क्रम (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->f_अग्र) {
        अगर (rlRange->fFirstInGroup) {
            पूर्णांक group_अंक = rlRange->f_अंक;
            RBBIDebugPrintf("%2i  ", group_अंक);

            अगर (group_अंक >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            क्रम (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_गाँठ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_गाँठ->fParent;
                अगर (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    अगर (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            क्रम (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->f_अग्र) {
                अगर (tRange->f_अंक == rlRange->f_अंक) {
                    अगर (i++ % 5 == 0) {
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
#पूर्ण_अगर


//------------------------------------------------------------------------
//
//   printSets          A debugging function.
//                      dump out all of the set definitions.
//
//------------------------------------------------------------------------
#अगर_घोषित RBBI_DEBUG
व्योम RBBISetBuilder::printSets() {
    पूर्णांक                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    क्रम (i=0; ; i++) {
        RBBINode        *uset_गाँठ;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_गाँठ = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        अगर (uset_गाँठ == nullptr) {
            अवरोध;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_गाँठ->fParent;
        अगर (setRef != nullptr) {
            varRef = setRef->fParent;
            अगर (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_गाँठ->fText);
        RBBIDebugPrintf("\n");
        अगर (uset_गाँठ->f_बायाँ_Child != nullptr) {
            RBBINode::print_वृक्ष(uset_गाँठ->f_बायाँ_Child, true);
        }
    }
    RBBIDebugPrintf("\n");
}
#पूर्ण_अगर



//-------------------------------------------------------------------------------------
//
//  RangeDescriptor copy constructor
//
//-------------------------------------------------------------------------------------

RangeDescriptor::RangeDescriptor(स्थिर RangeDescriptor &other, UErrorCode &status) :
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_अंक {other.f_अंक},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    अगर (U_FAILURE(status)) {
        वापस;
    }
    fIncludesSets = नया UVector(status);
    अगर (यह->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    अगर (U_FAILURE(status)) {
        वापस;
    }

    क्रम (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        यह->fIncludesSets->addElement(other.fIncludesSets->elementAt(i), status);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    fIncludesSets = नया UVector(status);
    अगर (fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor Destructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::~RangeDescriptor() {
    मिटाओ  fIncludesSets;
    fIncludesSets = nullptr;
}

//-------------------------------------------------------------------------------------
//
//  RangeDesriptor::split()
//
//-------------------------------------------------------------------------------------
व्योम RangeDescriptor::split(UChar32 where, UErrorCode &status) {
    U_ASSERT(where>fStartChar && where<=fEndChar);
    RangeDescriptor *nr = नया RangeDescriptor(*यह, status);
    अगर(nr == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    अगर (U_FAILURE(status)) {
        मिटाओ nr;
        वापस;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->fStartChar = where;
    यह->fEndChar = where-1;
    nr->f_अग्र      = यह->f_अग्र;
    यह->f_अग्र    = nr;
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
bool RangeDescriptor::isDictionaryRange() {
    अटल स्थिर char16_t *dictionary = u"dictionary";
    क्रम (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_गाँठ = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_गाँठ->fParent;
        अगर (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            अगर (varRef && varRef->fType == RBBINode::varRef) {
                स्थिर UnicodeString *setName = &varRef->fText;
                अगर (setName->compare(dictionary, -1) == 0) {
                    वापस true;
                }
            }
        }
    }
    वापस false;
}

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */
