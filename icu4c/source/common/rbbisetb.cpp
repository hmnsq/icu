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
    f_Group_ژماردن     = 0;
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
    RBBINode        *uset_گرێ;
    RangeDescriptor *rlRange;

    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

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
        uset_گرێ = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        إذا (uset_گرێ==nullptr) {
            اكسر;
        }

        UnicodeSet      *تێخستن_Set             = uset_گرێ->f_تێخستن_Set;
        int32_t          تێخستن_Set_Range_ژماردن   = تێخستن_Set->get_Range_ژماردن();
        صحيح              تێخستن_Set_Range_Index   = 0;
                         rlRange              = fRangeList;

        لكل (;;) {
            إذا (تێخستن_Set_Range_Index >= تێخستن_Set_Range_ژماردن) {
                اكسر;
            }
            UChar32      تێخستن_Set_Range_Begin  = تێخستن_Set->getRangeStart(تێخستن_Set_Range_Index);
            UChar32      تێخستن_Set_Range_End    = تێخستن_Set->getRangeEnd(تێخستن_Set_Range_Index);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            طالما (rlRange->fEndChar < تێخستن_Set_Range_Begin) {
                rlRange = rlRange->fNext;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            إذا (rlRange->fStartChar < تێخستن_Set_Range_Begin) {
                rlRange->split(تێخستن_Set_Range_Begin, *fStatus);
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
            إذا (rlRange->fEndChar > تێخستن_Set_Range_End) {
                rlRange->split(تێخستن_Set_Range_End+1, *fStatus);
                إذا (U_FAILURE(*fStatus)) {
                    أرجع;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            إذا (rlRange->fIncludesSets->indexOf(uset_گرێ) == -1) {
                rlRange->fIncludesSets->addElement(uset_گرێ, *fStatus);
                إذا (U_FAILURE(*fStatus)) {
                    أرجع;
                }
            }

            // Advance over ranges that we are finished with.
            إذا (تێخستن_Set_Range_End == rlRange->fEndChar) {
                تێخستن_Set_Range_Index++;
            }
            rlRange = rlRange->fNext;
        }
    }

    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    RangeDescriptor *rl_گەڕان_Range;
    int32_t dict_Group_ژماردن = 0;

    لكل (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        لكل (rl_گەڕان_Range=fRangeList; rl_گەڕان_Range != rlRange; rl_گەڕان_Range=rl_گەڕان_Range->fNext) {
            إذا (rlRange->fIncludesSets->equals(*rl_گەڕان_Range->fIncludesSets)) {
                rlRange->f_ژمارە = rl_گەڕان_Range->f_ژمارە;
                rlRange->fIncludesDict = rl_گەڕان_Range->fIncludesDict;
                اكسر;
            }
        }
        إذا (rlRange->f_ژمارە == 0) {
            rlRange->fFirstInGroup = true;
            إذا (rlRange->isDictionaryRange()) {
                rlRange->f_ژمارە = ++dict_Group_ژماردن;
                rlRange->fIncludesDict = true;
            } وإلا {
                f_Group_ژماردن++;
                rlRange->f_ژمارە = f_Group_ژماردن+2;
                addValToSets(rlRange->fIncludesSets, rlRange->f_ژمارە);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_Group_ژماردن + 3;
    لكل (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        إذا (rlRange->fIncludesDict) {
            rlRange->f_ژمارە += fDictCategoriesStart - 1;
            إذا (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->f_ژمارە);
            }
        }
    }
    f_Group_ژماردن += dict_Group_ژماردن;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    لكل (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_گرێ = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        إذا (uset_گرێ==nullptr) {
            اكسر;
        }
        UnicodeSet      *تێخستن_Set = uset_گرێ->f_تێخستن_Set;
        إذا (تێخستن_Set->contains(eofString)) {
            addValToSet(uset_گرێ, 1);
        }
    }


    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
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
                                range->f_ژمارە,           // value for range
                                fStatus);
    }
}


فراغ RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    لكل (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->fNext) {
        int32_t range_ژمارە = rd->f_ژمارە;
        إذا (range_ژمارە == categories.second) {
            rd->f_ژمارە = categories.first;
        } وإلا إذا (range_ژمارە > categories.second) {
            rd->f_ژمارە--;
        }
    }
    --f_Group_ژماردن;
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
        منطقي use8Bits = get_ژمارە_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_بەها_BITS_8 : UCPTRIE_بەها_BITS_16,
            fStatus);
        UErrorCode bufferStatus = *fStatus;
        fTrieSize = ucptrie_to_دووانەیی(fTrie, nullptr, 0, &bufferStatus);
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
    ucptrie_to_دووانەیی(fTrie,
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
        RBBINode* uset_گرێ = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_گرێ, val);
    }
}

فراغ  RBBISetBuilder::addValToSet(RBBINode *uset_گرێ, uint32_t val) {
    RBBINode *leaf_گرێ = جديد RBBINode(RBBINode::leafChar, *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        delete leaf_گرێ;
        أرجع;
    }
    إذا (leaf_گرێ == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    leaf_گرێ->fVal = static_cast<unsigned short>(val);
    إذا (uset_گرێ->f_چەپ_Child == nullptr) {
        uset_گرێ->f_چەپ_Child = leaf_گرێ;
        leaf_گرێ->fParent    = uset_گرێ;
    } وإلا {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_گرێ = جديد RBBINode(RBBINode::opOr, *fStatus);
        إذا (or_گرێ == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        إذا (U_FAILURE(*fStatus)) {
            delete or_گرێ;
            delete leaf_گرێ;
            أرجع;
        }
        or_گرێ->f_چەپ_Child  = uset_گرێ->f_چەپ_Child;
        or_گرێ->f_ڕاست_Child = leaf_گرێ;
        or_گرێ->f_چەپ_Child->fParent  = or_گرێ;
        or_گرێ->f_ڕاست_Child->fParent = or_گرێ;
        uset_گرێ->f_چەپ_Child = or_گرێ;
        or_گرێ->fParent = uset_گرێ;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_ژمارە_Char_Categories() ثابت {
    أرجع f_Group_ژماردن + 3;
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
        إذا (rlRange->f_ژمارە == category) {
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
فراغ RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    صحيح                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    لكل (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        لكل (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_گرێ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_گرێ->fParent;
            إذا (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                إذا (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
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
فراغ RBBISetBuilder::printRangeGroups() {
    صحيح                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    لكل (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        إذا (rlRange->fFirstInGroup) {
            صحيح group_ژمارە = rlRange->f_ژمارە;
            RBBIDebugPrintf("%2i  ", group_ژمارە);

            إذا (group_ژمارە >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            لكل (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_گرێ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_گرێ->fParent;
                إذا (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    إذا (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            لكل (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->fNext) {
                إذا (tRange->f_ژمارە == rlRange->f_ژمارە) {
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
فراغ RBBISetBuilder::printSets() {
    صحيح                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    لكل (i=0; ; i++) {
        RBBINode        *uset_گرێ;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_گرێ = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        إذا (uset_گرێ == nullptr) {
            اكسر;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_گرێ->fParent;
        إذا (setRef != nullptr) {
            varRef = setRef->fParent;
            إذا (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_گرێ->fText);
        RBBIDebugPrintf("\n");
        إذا (uset_گرێ->f_چەپ_Child != nullptr) {
            RBBINode::print_درەخت(uset_گرێ->f_چەپ_Child, true);
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
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_ژمارە {other.f_ژمارە},
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
        RBBINode* uset_گرێ = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_گرێ->fParent;
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
