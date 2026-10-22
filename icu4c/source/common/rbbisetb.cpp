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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "uvector.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"

#அந்தர்பாவ "rbbisetb.h"
#அந்தர்பாவ "rbbinode.h"

U_NAMESPACE_BEGIN

மாறிலி int32_t kMaxCharCategoriesFor8BitsTrie = 255;
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
    f_Group_மொத்தம்     = 0;
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
    க்கு (nextRangeDesc = fRangeList; nextRangeDesc!=nullptr;) {
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
வெறுமை RBBISetBuilder::buildRanges() {
    RBBINode        *uset_முனை;
    RangeDescriptor *rlRange;

    எனில் (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = புதிய RangeDescriptor(*fStatus); // will check for status here
    எனில் (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    முழு  ni;
    க்கு (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_முனை = static_cast<RBBINode*>(இது->fRB->fUSetNodes->elementAt(ni));
        எனில் (uset_முனை==nullptr) {
            நிறுத்து;
        }

        UnicodeSet      *உள்ளீடு_Set             = uset_முனை->f_உள்ளீடு_Set;
        int32_t          உள்ளீடு_Set_Range_மொத்தம்   = உள்ளீடு_Set->get_Range_மொத்தம்();
        முழு              உள்ளீடு_Set_Range_Index   = 0;
                         rlRange              = fRangeList;

        க்கு (;;) {
            எனில் (உள்ளீடு_Set_Range_Index >= உள்ளீடு_Set_Range_மொத்தம்) {
                நிறுத்து;
            }
            UChar32      உள்ளீடு_Set_Range_Begin  = உள்ளீடு_Set->getRangeStart(உள்ளீடு_Set_Range_Index);
            UChar32      உள்ளீடு_Set_Range_End    = உள்ளீடு_Set->getRangeEnd(உள்ளீடு_Set_Range_Index);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            போது (rlRange->fEndChar < உள்ளீடு_Set_Range_Begin) {
                rlRange = rlRange->fNext;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            எனில் (rlRange->fStartChar < உள்ளீடு_Set_Range_Begin) {
                rlRange->split(உள்ளீடு_Set_Range_Begin, *fStatus);
                எனில் (U_FAILURE(*fStatus)) {
                    திரும்பு;
                }
                தொடர்;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            எனில் (rlRange->fEndChar > உள்ளீடு_Set_Range_End) {
                rlRange->split(உள்ளீடு_Set_Range_End+1, *fStatus);
                எனில் (U_FAILURE(*fStatus)) {
                    திரும்பு;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            எனில் (rlRange->fIncludesSets->indexOf(uset_முனை) == -1) {
                rlRange->fIncludesSets->addElement(uset_முனை, *fStatus);
                எனில் (U_FAILURE(*fStatus)) {
                    திரும்பு;
                }
            }

            // Advance over ranges that we are finished with.
            எனில் (உள்ளீடு_Set_Range_End == rlRange->fEndChar) {
                உள்ளீடு_Set_Range_Index++;
            }
            rlRange = rlRange->fNext;
        }
    }

    எனில் (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    RangeDescriptor *rl_தேடல்_Range;
    int32_t dict_Group_மொத்தம் = 0;

    க்கு (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        க்கு (rl_தேடல்_Range=fRangeList; rl_தேடல்_Range != rlRange; rl_தேடல்_Range=rl_தேடல்_Range->fNext) {
            எனில் (rlRange->fIncludesSets->equals(*rl_தேடல்_Range->fIncludesSets)) {
                rlRange->f_இலக்கம் = rl_தேடல்_Range->f_இலக்கம்;
                rlRange->fIncludesDict = rl_தேடல்_Range->fIncludesDict;
                நிறுத்து;
            }
        }
        எனில் (rlRange->f_இலக்கம் == 0) {
            rlRange->fFirstInGroup = உண்மை;
            எனில் (rlRange->isDictionaryRange()) {
                rlRange->f_இலக்கம் = ++dict_Group_மொத்தம்;
                rlRange->fIncludesDict = உண்மை;
            } இல்லையெனில் {
                f_Group_மொத்தம்++;
                rlRange->f_இலக்கம் = f_Group_மொத்தம்+2;
                addValToSets(rlRange->fIncludesSets, rlRange->f_இலக்கம்);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_Group_மொத்தம் + 3;
    க்கு (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        எனில் (rlRange->fIncludesDict) {
            rlRange->f_இலக்கம் += fDictCategoriesStart - 1;
            எனில் (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->f_இலக்கம்);
            }
        }
    }
    f_Group_மொத்தம் += dict_Group_மொத்தம்;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    க்கு (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_முனை = static_cast<RBBINode*>(இது->fRB->fUSetNodes->elementAt(ni));
        எனில் (uset_முனை==nullptr) {
            நிறுத்து;
        }
        UnicodeSet      *உள்ளீடு_Set = uset_முனை->f_உள்ளீடு_Set;
        எனில் (உள்ளீடு_Set->contains(eofString)) {
            addValToSet(uset_முனை, 1);
        }
    }


    எனில் (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    எனில் (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
வெறுமை RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        fStatus);

    க்கு (RangeDescriptor *range = fRangeList; range!=nullptr && U_SUCCESS(*fStatus); range=range->fNext) {
        umutablecptrie_setRange(fMutableTrie,
                                range->fStartChar,     // Range start
                                range->fEndChar,       // Range end (inclusive)
                                range->f_இலக்கம்,           // value for range
                                fStatus);
    }
}


வெறுமை RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    க்கு (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->fNext) {
        int32_t range_இலக்கம் = rd->f_இலக்கம்;
        எனில் (range_இலக்கம் == categories.second) {
            rd->f_இலக்கம் = categories.first;
        } இல்லையெனில் எனில் (range_இலக்கம் > categories.second) {
            rd->f_இலக்கம்--;
        }
    }
    --f_Group_மொத்தம்;
    எனில் (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::getTrieSize()  {
    எனில் (U_FAILURE(*fStatus)) {
        திரும்பு 0;
    }
    எனில் (fTrie == nullptr) {
        தருக்க use8Bits = get_இலக்கம்_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_மதிப்பு_BITS_8 : UCPTRIE_மதிப்பு_BITS_16,
            fStatus);
        UErrorCode bufferStatus = *fStatus;
        fTrieSize = ucptrie_to_இருமம்(fTrie, nullptr, 0, &bufferStatus);
        எனில் (bufferStatus != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(bufferStatus)) {
            *fStatus = bufferStatus;
        }
    }
    திரும்பு fTrieSize;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
வெறுமை RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_to_இருமம்(fTrie,
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
வெறுமை  RBBISetBuilder::addValToSets(UVector *sets, uint32_t val) {
    int32_t       ix;

    க்கு (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_முனை = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_முனை, val);
    }
}

வெறுமை  RBBISetBuilder::addValToSet(RBBINode *uset_முனை, uint32_t val) {
    RBBINode *leaf_முனை = புதிய RBBINode(RBBINode::leafChar, *fStatus);
    எனில் (U_FAILURE(*fStatus)) {
        delete leaf_முனை;
        திரும்பு;
    }
    எனில் (leaf_முனை == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    leaf_முனை->fVal = static_cast<unsigned short>(val);
    எனில் (uset_முனை->f_இடது_Child == nullptr) {
        uset_முனை->f_இடது_Child = leaf_முனை;
        leaf_முனை->fParent    = uset_முனை;
    } இல்லையெனில் {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_முனை = புதிய RBBINode(RBBINode::opOr, *fStatus);
        எனில் (or_முனை == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        எனில் (U_FAILURE(*fStatus)) {
            delete or_முனை;
            delete leaf_முனை;
            திரும்பு;
        }
        or_முனை->f_இடது_Child  = uset_முனை->f_இடது_Child;
        or_முனை->f_வலது_Child = leaf_முனை;
        or_முனை->f_இடது_Child->fParent  = or_முனை;
        or_முனை->f_வலது_Child->fParent = or_முனை;
        uset_முனை->f_இடது_Child = or_முனை;
        or_முனை->fParent = uset_முனை;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_இலக்கம்_Char_Categories() மாறிலி {
    திரும்பு f_Group_மொத்தம் + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() மாறிலி {
    திரும்பு fDictCategoriesStart;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::getFirstChar(int32_t category) மாறிலி {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    க்கு (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        எனில் (rlRange->f_இலக்கம் == category) {
            retVal = rlRange->fStartChar;
            நிறுத்து;
        }
    }
    திரும்பு retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#ifdef RBBI_DEBUG
வெறுமை RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    முழு                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    க்கு (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        க்கு (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_முனை    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_முனை->fParent;
            எனில் (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                எனில் (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
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
வெறுமை RBBISetBuilder::printRangeGroups() {
    முழு                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    க்கு (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        எனில் (rlRange->fFirstInGroup) {
            முழு group_இலக்கம் = rlRange->f_இலக்கம்;
            RBBIDebugPrintf("%2i  ", group_இலக்கம்);

            எனில் (group_இலக்கம் >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            க்கு (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_முனை    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_முனை->fParent;
                எனில் (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    எனில் (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            க்கு (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->fNext) {
                எனில் (tRange->f_இலக்கம் == rlRange->f_இலக்கம்) {
                    எனில் (i++ % 5 == 0) {
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
வெறுமை RBBISetBuilder::printSets() {
    முழு                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    க்கு (i=0; ; i++) {
        RBBINode        *uset_முனை;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_முனை = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        எனில் (uset_முனை == nullptr) {
            நிறுத்து;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_முனை->fParent;
        எனில் (setRef != nullptr) {
            varRef = setRef->fParent;
            எனில் (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_முனை->fText);
        RBBIDebugPrintf("\n");
        எனில் (uset_முனை->f_இடது_Child != nullptr) {
            RBBINode::print_மரம்(uset_முனை->f_இடது_Child, உண்மை);
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

RangeDescriptor::RangeDescriptor(மாறிலி RangeDescriptor &other, UErrorCode &status) :
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_இலக்கம் {other.f_இலக்கம்},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    fIncludesSets = புதிய UVector(status);
    எனில் (இது->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }

    க்கு (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        இது->fIncludesSets->addElement(other.fIncludesSets->elementAt(i), status);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UErrorCode &status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    fIncludesSets = புதிய UVector(status);
    எனில் (fIncludesSets == nullptr) {
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
வெறுமை RangeDescriptor::split(UChar32 where, UErrorCode &status) {
    U_ASSERT(where>fStartChar && where<=fEndChar);
    RangeDescriptor *nr = புதிய RangeDescriptor(*இது, status);
    எனில்(nr == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    எனில் (U_FAILURE(status)) {
        delete nr;
        திரும்பு;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->fStartChar = where;
    இது->fEndChar = where-1;
    nr->fNext      = இது->fNext;
    இது->fNext    = nr;
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
தருக்க RangeDescriptor::isDictionaryRange() {
    நிலையான மாறிலி char16_t *dictionary = u"dictionary";
    க்கு (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_முனை = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_முனை->fParent;
        எனில் (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            எனில் (varRef && varRef->fType == RBBINode::varRef) {
                மாறிலி UnicodeString *setName = &varRef->fText;
                எனில் (setName->compare(dictionary, -1) == 0) {
                    திரும்பு உண்மை;
                }
            }
        }
    }
    திரும்பு பொய்;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
