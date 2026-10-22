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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/uniset.h"
#ضمن "uvector.h"
#ضمن "uassert.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"

#ضمن "rbbisetb.h"
#ضمن "rbbinode.h"

U_NAMESPACE_BEGIN

ثابت int32_t kMaxCharCategoriesFor8BitsTrie = 255;
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
    f_Group_كدي     = 0;
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
    لكل (nextRangeDesc = fRangeList; nextRangeDesc!=nullptr;) {
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
فراغ RBBISetBuilder::buildRanges() {
    RBBINode        *uset_كلي;
    RangeDescriptor *rlRange;

    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {بغ_Sets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = جديد RangeDescriptor(*fStatus); // will check for status here
    إذا (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    صحيح  ni;
    لكل (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_كلي = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        إذا (uset_كلي==nullptr) {
            اكسر;
        }

        UnicodeSet      *شغرو_Set             = uset_كلي->f_شغرو_Set;
        int32_t          شغرو_Set_Range_كدي   = شغرو_Set->get_Range_كدي();
        صحيح              شغرو_Set_Range_Index   = 0;
                         rlRange              = fRangeList;

        لكل (;;) {
            إذا (شغرو_Set_Range_Index >= شغرو_Set_Range_كدي) {
                اكسر;
            }
            UChar32      شغرو_Set_Range_Begin  = شغرو_Set->getRangeStart(شغرو_Set_Range_Index);
            UChar32      شغرو_Set_Range_End    = شغرو_Set->getRangeEnd(شغرو_Set_Range_Index);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            طالما (rlRange->fEndChar < شغرو_Set_Range_Begin) {
                rlRange = rlRange->fNext;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            إذا (rlRange->fStartChar < شغرو_Set_Range_Begin) {
                rlRange->split(شغرو_Set_Range_Begin, *fStatus);
                إذا (U_FAILURE(*fStatus)) {
                    أرجع;
                }
                استمر;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            إذا (rlRange->fEndChar > شغرو_Set_Range_End) {
                rlRange->split(شغرو_Set_Range_End+1, *fStatus);
                إذا (U_FAILURE(*fStatus)) {
                    أرجع;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            إذا (rlRange->fIncludesSets->indexOf(uset_كلي) == -1) {
                rlRange->fIncludesSets->addElement(uset_كلي, *fStatus);
                إذا (U_FAILURE(*fStatus)) {
                    أرجع;
                }
            }

            // Advance over ranges that we are finished with.
            إذا (شغرو_Set_Range_End == rlRange->fEndChar) {
                شغرو_Set_Range_Index++;
            }
            rlRange = rlRange->fNext;
        }
    }

    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { بغ_Ranges();}

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
    RangeDescriptor *rl_نيما_Range;
    int32_t dict_Group_كدي = 0;

    لكل (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        لكل (rl_نيما_Range=fRangeList; rl_نيما_Range != rlRange; rl_نيما_Range=rl_نيما_Range->fNext) {
            إذا (rlRange->fIncludesSets->equals(*rl_نيما_Range->fIncludesSets)) {
                rlRange->f_لمب = rl_نيما_Range->f_لمب;
                rlRange->fIncludesDict = rl_نيما_Range->fIncludesDict;
                اكسر;
            }
        }
        إذا (rlRange->f_لمب == 0) {
            rlRange->fFirstInGroup = true;
            إذا (rlRange->isDictionaryRange()) {
                rlRange->f_لمب = ++dict_Group_كدي;
                rlRange->fIncludesDict = true;
            } وإلا {
                f_Group_كدي++;
                rlRange->f_لمب = f_Group_كدي+2;
                addValToSets(rlRange->fIncludesSets, rlRange->f_لمب);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_Group_كدي + 3;
    لكل (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        إذا (rlRange->fIncludesDict) {
            rlRange->f_لمب += fDictCategoriesStart - 1;
            إذا (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->f_لمب);
            }
        }
    }
    f_Group_كدي += dict_Group_كدي;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    لكل (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_كلي = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        إذا (uset_كلي==nullptr) {
            اكسر;
        }
        UnicodeSet      *شغرو_Set = uset_كلي->f_شغرو_Set;
        إذا (شغرو_Set->contains(eofString)) {
            addValToSet(uset_كلي, 1);
        }
    }


    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {بغ_Range_Groups();}
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {بغ_Sets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
فراغ RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        fStatus);

    لكل (RangeDescriptor *range = fRangeList; range!=nullptr && U_SUCCESS(*fStatus); range=range->fNext) {
        umutablecptrie_setRange(fMutableTrie,
                                range->fStartChar,     // Range start
                                range->fEndChar,       // Range end (inclusive)
                                range->f_لمب,           // value for range
                                fStatus);
    }
}


فراغ RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    لكل (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->fNext) {
        int32_t range_لمب = rd->f_لمب;
        إذا (range_لمب == categories.second) {
            rd->f_لمب = categories.first;
        } وإلا إذا (range_لمب > categories.second) {
            rd->f_لمب--;
        }
    }
    --f_Group_كدي;
    إذا (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::getTrieSize()  {
    إذا (U_FAILURE(*fStatus)) {
        أرجع 0;
    }
    إذا (fTrie == nullptr) {
        منطقي use8Bits = get_لمب_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_درج_BITS_8 : UCPTRIE_درج_BITS_16,
            fStatus);
        UErrorCode bufferStatus = *fStatus;
        fTrieSize = ucptrie_to_نبيو(fTrie, nullptr, 0, &bufferStatus);
        إذا (bufferStatus != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(bufferStatus)) {
            *fStatus = bufferStatus;
        }
    }
    أرجع fTrieSize;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
فراغ RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_to_نبيو(fTrie,
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
فراغ  RBBISetBuilder::addValToSets(UVector *sets, uint32_t val) {
    int32_t       ix;

    لكل (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_كلي = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_كلي, val);
    }
}

فراغ  RBBISetBuilder::addValToSet(RBBINode *uset_كلي, uint32_t val) {
    RBBINode *leaf_كلي = جديد RBBINode(RBBINode::leafChar, *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        delete leaf_كلي;
        أرجع;
    }
    إذا (leaf_كلي == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    leaf_كلي->fVal = static_cast<unsigned short>(val);
    إذا (uset_كلي->f_هغو_Child == nullptr) {
        uset_كلي->f_هغو_Child = leaf_كلي;
        leaf_كلي->fParent    = uset_كلي;
    } وإلا {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_كلي = جديد RBBINode(RBBINode::opOr, *fStatus);
        إذا (or_كلي == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        إذا (U_FAILURE(*fStatus)) {
            delete or_كلي;
            delete leaf_كلي;
            أرجع;
        }
        or_كلي->f_هغو_Child  = uset_كلي->f_هغو_Child;
        or_كلي->f_دام_Child = leaf_كلي;
        or_كلي->f_هغو_Child->fParent  = or_كلي;
        or_كلي->f_دام_Child->fParent = or_كلي;
        uset_كلي->f_هغو_Child = or_كلي;
        or_كلي->fParent = uset_كلي;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_لمب_Char_Categories() ثابت {
    أرجع f_Group_كدي + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() ثابت {
    أرجع fDictCategoriesStart;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::getFirstChar(int32_t category) ثابت {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    لكل (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        إذا (rlRange->f_لمب == category) {
            retVal = rlRange->fStartChar;
            اكسر;
        }
    }
    أرجع retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#ifdef RBBI_DEBUG
فراغ RBBISetBuilder::بغ_Ranges() {
    RangeDescriptor       *rlRange;
    صحيح                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    لكل (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        لكل (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_كلي    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_كلي->fParent;
            إذا (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                إذا (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                    setName = varRef->fText;
                }
            }
            RBBI_DEBUG_بغ_Unicode_String(setName); RBBIDebugPrintf("  ");
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
فراغ RBBISetBuilder::بغ_Range_Groups() {
    صحيح                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    لكل (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        إذا (rlRange->fFirstInGroup) {
            صحيح group_لمب = rlRange->f_لمب;
            RBBIDebugPrintf("%2i  ", group_لمب);

            إذا (group_لمب >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            لكل (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_كلي    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_كلي->fParent;
                إذا (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    إذا (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_بغ_Unicode_String(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            لكل (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->fNext) {
                إذا (tRange->f_لمب == rlRange->f_لمب) {
                    إذا (i++ % 5 == 0) {
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
فراغ RBBISetBuilder::بغ_Sets() {
    صحيح                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    لكل (i=0; ; i++) {
        RBBINode        *uset_كلي;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_كلي = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        إذا (uset_كلي == nullptr) {
            اكسر;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_كلي->fParent;
        إذا (setRef != nullptr) {
            varRef = setRef->fParent;
            إذا (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_بغ_Unicode_String(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_بغ_Unicode_String(uset_كلي->fText);
        RBBIDebugPrintf("\n");
        إذا (uset_كلي->f_هغو_Child != nullptr) {
            RBBINode::بغ_بشيا(uset_كلي->f_هغو_Child, true);
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

RangeDescriptor::RangeDescriptor(ثابت RangeDescriptor &other, UErrorCode &status) :
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_لمب {other.f_لمب},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    إذا (U_FAILURE(status)) {
        أرجع;
    }
    fIncludesSets = جديد UVector(status);
    إذا (هذا->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    إذا (U_FAILURE(status)) {
        أرجع;
    }

    لكل (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        هذا->fIncludesSets->addElement(other.fIncludesSets->elementAt(i), status);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UErrorCode &status) {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    fIncludesSets = جديد UVector(status);
    إذا (fIncludesSets == nullptr) {
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
فراغ RangeDescriptor::split(UChar32 where, UErrorCode &status) {
    U_ASSERT(where>fStartChar && where<=fEndChar);
    RangeDescriptor *nr = جديد RangeDescriptor(*هذا, status);
    إذا(nr == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    إذا (U_FAILURE(status)) {
        delete nr;
        أرجع;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->fStartChar = where;
    هذا->fEndChar = where-1;
    nr->fNext      = هذا->fNext;
    هذا->fNext    = nr;
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
منطقي RangeDescriptor::isDictionaryRange() {
    ساكن ثابت char16_t *dictionary = u"dictionary";
    لكل (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_كلي = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_كلي->fParent;
        إذا (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            إذا (varRef && varRef->fType == RBBINode::varRef) {
                ثابت UnicodeString *setName = &varRef->fText;
                إذا (setName->compare(dictionary, -1) == 0) {
                    أرجع true;
                }
            }
        }
    }
    أرجع خطأ;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
