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

#ܐܢ !UCONFIG_NO_BREAK_ITERATION

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
    fGroupCount     = 0;
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
    RBBINode        *usetNode;
    RangeDescriptor *rlRange;

    ܐܢ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = جديد RangeDescriptor(*fStatus); // will check for status here
    ܐܢ (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    ܐܢ (U_FAILURE(*fStatus)) {
        ܦܢܐ;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    صحيح  ni;
    لكل (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        usetNode = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        ܐܢ (usetNode==nullptr) {
            اكسر;
        }

        UnicodeSet      *inputSet             = usetNode->fInputSet;
        int32_t          inputSetRangeCount   = inputSet->getRangeCount();
        صحيح              inputSetRangeIndex   = 0;
                         rlRange              = fRangeList;

        لكل (;;) {
            ܐܢ (inputSetRangeIndex >= inputSetRangeCount) {
                اكسر;
            }
            UChar32      inputSetRangeBegin  = inputSet->getRangeStart(inputSetRangeIndex);
            UChar32      inputSetRangeEnd    = inputSet->getRangeEnd(inputSetRangeIndex);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            ܟܡܐ (rlRange->fEndChar < inputSetRangeBegin) {
                rlRange = rlRange->fNext;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            ܐܢ (rlRange->fStartChar < inputSetRangeBegin) {
                rlRange->split(inputSetRangeBegin, *fStatus);
                ܐܢ (U_FAILURE(*fStatus)) {
                    ܦܢܐ;
                }
                استمر;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            ܐܢ (rlRange->fEndChar > inputSetRangeEnd) {
                rlRange->split(inputSetRangeEnd+1, *fStatus);
                ܐܢ (U_FAILURE(*fStatus)) {
                    ܦܢܐ;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            ܐܢ (rlRange->fIncludesSets->indexOf(usetNode) == -1) {
                rlRange->fIncludesSets->addElement(usetNode, *fStatus);
                ܐܢ (U_FAILURE(*fStatus)) {
                    ܦܢܐ;
                }
            }

            // Advance over ranges that we are finished with.
            ܐܢ (inputSetRangeEnd == rlRange->fEndChar) {
                inputSetRangeIndex++;
            }
            rlRange = rlRange->fNext;
        }
    }

    ܐܢ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    RangeDescriptor *rlSearchRange;
    int32_t dictGroupCount = 0;

    لكل (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        لكل (rlSearchRange=fRangeList; rlSearchRange != rlRange; rlSearchRange=rlSearchRange->fNext) {
            ܐܢ (rlRange->fIncludesSets->equals(*rlSearchRange->fIncludesSets)) {
                rlRange->fNum = rlSearchRange->fNum;
                rlRange->fIncludesDict = rlSearchRange->fIncludesDict;
                اكسر;
            }
        }
        ܐܢ (rlRange->fNum == 0) {
            rlRange->fFirstInGroup = ܫܪܪܐ;
            ܐܢ (rlRange->isDictionaryRange()) {
                rlRange->fNum = ++dictGroupCount;
                rlRange->fIncludesDict = ܫܪܪܐ;
            } ܐܚܪܢܐ {
                fGroupCount++;
                rlRange->fNum = fGroupCount+2;
                addValToSets(rlRange->fIncludesSets, rlRange->fNum);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = fGroupCount + 3;
    لكل (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        ܐܢ (rlRange->fIncludesDict) {
            rlRange->fNum += fDictCategoriesStart - 1;
            ܐܢ (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->fNum);
            }
        }
    }
    fGroupCount += dictGroupCount;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    لكل (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        usetNode = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        ܐܢ (usetNode==nullptr) {
            اكسر;
        }
        UnicodeSet      *inputSet = usetNode->fInputSet;
        ܐܢ (inputSet->contains(eofString)) {
            addValToSet(usetNode, 1);
        }
    }


    ܐܢ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    ܐܢ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
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
                                range->fNum,           // value for range
                                fStatus);
    }
}


فراغ RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    لكل (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->fNext) {
        int32_t rangeNum = rd->fNum;
        ܐܢ (rangeNum == categories.second) {
            rd->fNum = categories.first;
        } ܐܚܪܢܐ ܐܢ (rangeNum > categories.second) {
            rd->fNum--;
        }
    }
    --fGroupCount;
    ܐܢ (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::getTrieSize()  {
    ܐܢ (U_FAILURE(*fStatus)) {
        ܦܢܐ 0;
    }
    ܐܢ (fTrie == nullptr) {
        منطقي use8Bits = getNumCharCategories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_VALUE_BITS_8 : UCPTRIE_VALUE_BITS_16,
            fStatus);
        UErrorCode bufferStatus = *fStatus;
        fTrieSize = ucptrie_toBinary(fTrie, nullptr, 0, &bufferStatus);
        ܐܢ (bufferStatus != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(bufferStatus)) {
            *fStatus = bufferStatus;
        }
    }
    ܦܢܐ fTrieSize;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
فراغ RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_toBinary(fTrie,
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
        RBBINode* usetNode = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(usetNode, val);
    }
}

فراغ  RBBISetBuilder::addValToSet(RBBINode *usetNode, uint32_t val) {
    RBBINode *leafNode = جديد RBBINode(RBBINode::leafChar, *fStatus);
    ܐܢ (U_FAILURE(*fStatus)) {
        delete leafNode;
        ܦܢܐ;
    }
    ܐܢ (leafNode == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    leafNode->fVal = static_cast<unsigned short>(val);
    ܐܢ (usetNode->fLeftChild == nullptr) {
        usetNode->fLeftChild = leafNode;
        leafNode->fParent    = usetNode;
    } ܐܚܪܢܐ {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *orNode = جديد RBBINode(RBBINode::opOr, *fStatus);
        ܐܢ (orNode == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        ܐܢ (U_FAILURE(*fStatus)) {
            delete orNode;
            delete leafNode;
            ܦܢܐ;
        }
        orNode->fLeftChild  = usetNode->fLeftChild;
        orNode->fRightChild = leafNode;
        orNode->fLeftChild->fParent  = orNode;
        orNode->fRightChild->fParent = orNode;
        usetNode->fLeftChild = orNode;
        orNode->fParent = usetNode;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getNumCharCategories() ثابت {
    ܦܢܐ fGroupCount + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() ثابت {
    ܦܢܐ fDictCategoriesStart;
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
        ܐܢ (rlRange->fNum == category) {
            retVal = rlRange->fStartChar;
            اكسر;
        }
    }
    ܦܢܐ retVal;
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
            RBBINode       *usetNode    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = usetNode->fParent;
            ܐܢ (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                ܐܢ (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
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
        ܐܢ (rlRange->fFirstInGroup) {
            صحيح groupNum = rlRange->fNum;
            RBBIDebugPrintf("%2i  ", groupNum);

            ܐܢ (groupNum >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            لكل (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *usetNode    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = usetNode->fParent;
                ܐܢ (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    ܐܢ (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            لكل (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->fNext) {
                ܐܢ (tRange->fNum == rlRange->fNum) {
                    ܐܢ (i++ % 5 == 0) {
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
        RBBINode        *usetNode;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        usetNode = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        ܐܢ (usetNode == nullptr) {
            اكسر;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = usetNode->fParent;
        ܐܢ (setRef != nullptr) {
            varRef = setRef->fParent;
            ܐܢ (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(usetNode->fText);
        RBBIDebugPrintf("\n");
        ܐܢ (usetNode->fLeftChild != nullptr) {
            RBBINode::printTree(usetNode->fLeftChild, ܫܪܪܐ);
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
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, fNum {other.fNum},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }
    fIncludesSets = جديد UVector(status);
    ܐܢ (هذا->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
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
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }
    fIncludesSets = جديد UVector(status);
    ܐܢ (fIncludesSets == nullptr) {
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
    ܐܢ(nr == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    ܐܢ (U_FAILURE(status)) {
        delete nr;
        ܦܢܐ;
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
        RBBINode* usetNode = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = usetNode->fParent;
        ܐܢ (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            ܐܢ (varRef && varRef->fType == RBBINode::varRef) {
                ثابت UnicodeString *setName = &varRef->fText;
                ܐܢ (setName->compare(dictionary, -1) == 0) {
                    ܦܢܐ ܫܪܪܐ;
                }
            }
        }
    }
    ܦܢܐ ܕܓܠܐ;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
