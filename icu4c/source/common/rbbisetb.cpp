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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/uniset.h"
#شامل "uvector.h"
#شامل "uassert.h"
#شامل "cmemory.h"
#شامل "cstring.h"

#شامل "rbbisetb.h"
#شامل "rbbinode.h"

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
    f_Group_شمار     = 0;
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
    برای (nextRangeDesc = fRangeList; nextRangeDesc!=nullptr;) {
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
تهی RBBISetBuilder::buildRanges() {
    RBBINode        *uset_گره;
    RangeDescriptor *rlRange;

    اگر (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = جدید RangeDescriptor(*fStatus); // will check for status here
    اگر (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    صحیح  ni;
    برای (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_گره = static_cast<RBBINode*>(این->fRB->fUSetNodes->elementAt(ni));
        اگر (uset_گره==nullptr) {
            بشکن;
        }

        UnicodeSet      *درون_داد_Set             = uset_گره->f_درون_داد_Set;
        int32_t          درون_داد_Set_Range_شمار   = درون_داد_Set->get_Range_شمار();
        صحیح              درون_داد_Set_Range_Index   = 0;
                         rlRange              = fRangeList;

        برای (;;) {
            اگر (درون_داد_Set_Range_Index >= درون_داد_Set_Range_شمار) {
                بشکن;
            }
            UChar32      درون_داد_Set_Range_Begin  = درون_داد_Set->getRangeStart(درون_داد_Set_Range_Index);
            UChar32      درون_داد_Set_Range_End    = درون_داد_Set->getRangeEnd(درون_داد_Set_Range_Index);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            وقتی (rlRange->fEndChar < درون_داد_Set_Range_Begin) {
                rlRange = rlRange->fNext;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            اگر (rlRange->fStartChar < درون_داد_Set_Range_Begin) {
                rlRange->split(درون_داد_Set_Range_Begin, *fStatus);
                اگر (U_FAILURE(*fStatus)) {
                    بازگشت;
                }
                ادامه;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            اگر (rlRange->fEndChar > درون_داد_Set_Range_End) {
                rlRange->split(درون_داد_Set_Range_End+1, *fStatus);
                اگر (U_FAILURE(*fStatus)) {
                    بازگشت;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            اگر (rlRange->fIncludesSets->indexOf(uset_گره) == -1) {
                rlRange->fIncludesSets->addElement(uset_گره, *fStatus);
                اگر (U_FAILURE(*fStatus)) {
                    بازگشت;
                }
            }

            // Advance over ranges that we are finished with.
            اگر (درون_داد_Set_Range_End == rlRange->fEndChar) {
                درون_داد_Set_Range_Index++;
            }
            rlRange = rlRange->fNext;
        }
    }

    اگر (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    RangeDescriptor *rl_جستجو_Range;
    int32_t dict_Group_شمار = 0;

    برای (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        برای (rl_جستجو_Range=fRangeList; rl_جستجو_Range != rlRange; rl_جستجو_Range=rl_جستجو_Range->fNext) {
            اگر (rlRange->fIncludesSets->equals(*rl_جستجو_Range->fIncludesSets)) {
                rlRange->f_رقم = rl_جستجو_Range->f_رقم;
                rlRange->fIncludesDict = rl_جستجو_Range->fIncludesDict;
                بشکن;
            }
        }
        اگر (rlRange->f_رقم == 0) {
            rlRange->fFirstInGroup = درست;
            اگر (rlRange->isDictionaryRange()) {
                rlRange->f_رقم = ++dict_Group_شمار;
                rlRange->fIncludesDict = درست;
            } وگرنه {
                f_Group_شمار++;
                rlRange->f_رقم = f_Group_شمار+2;
                addValToSets(rlRange->fIncludesSets, rlRange->f_رقم);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_Group_شمار + 3;
    برای (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        اگر (rlRange->fIncludesDict) {
            rlRange->f_رقم += fDictCategoriesStart - 1;
            اگر (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->f_رقم);
            }
        }
    }
    f_Group_شمار += dict_Group_شمار;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    برای (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_گره = static_cast<RBBINode*>(این->fRB->fUSetNodes->elementAt(ni));
        اگر (uset_گره==nullptr) {
            بشکن;
        }
        UnicodeSet      *درون_داد_Set = uset_گره->f_درون_داد_Set;
        اگر (درون_داد_Set->contains(eofString)) {
            addValToSet(uset_گره, 1);
        }
    }


    اگر (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    اگر (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
تهی RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        fStatus);

    برای (RangeDescriptor *range = fRangeList; range!=nullptr && U_SUCCESS(*fStatus); range=range->fNext) {
        umutablecptrie_setRange(fMutableTrie,
                                range->fStartChar,     // Range start
                                range->fEndChar,       // Range end (inclusive)
                                range->f_رقم,           // value for range
                                fStatus);
    }
}


تهی RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    برای (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->fNext) {
        int32_t range_رقم = rd->f_رقم;
        اگر (range_رقم == categories.second) {
            rd->f_رقم = categories.first;
        } وگرنه اگر (range_رقم > categories.second) {
            rd->f_رقم--;
        }
    }
    --f_Group_شمار;
    اگر (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::getTrieSize()  {
    اگر (U_FAILURE(*fStatus)) {
        بازگشت 0;
    }
    اگر (fTrie == nullptr) {
        منطقی use8Bits = get_رقم_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_مقدار_BITS_8 : UCPTRIE_مقدار_BITS_16,
            fStatus);
        UErrorCode bufferStatus = *fStatus;
        fTrieSize = ucptrie_to_دودویی(fTrie, nullptr, 0, &bufferStatus);
        اگر (bufferStatus != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(bufferStatus)) {
            *fStatus = bufferStatus;
        }
    }
    بازگشت fTrieSize;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
تهی RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_to_دودویی(fTrie,
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
تهی  RBBISetBuilder::addValToSets(UVector *sets, uint32_t val) {
    int32_t       ix;

    برای (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_گره = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_گره, val);
    }
}

تهی  RBBISetBuilder::addValToSet(RBBINode *uset_گره, uint32_t val) {
    RBBINode *leaf_گره = جدید RBBINode(RBBINode::leafChar, *fStatus);
    اگر (U_FAILURE(*fStatus)) {
        delete leaf_گره;
        بازگشت;
    }
    اگر (leaf_گره == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    leaf_گره->fVal = static_cast<unsigned short>(val);
    اگر (uset_گره->f_چپ_Child == nullptr) {
        uset_گره->f_چپ_Child = leaf_گره;
        leaf_گره->fParent    = uset_گره;
    } وگرنه {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_گره = جدید RBBINode(RBBINode::opOr, *fStatus);
        اگر (or_گره == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        اگر (U_FAILURE(*fStatus)) {
            delete or_گره;
            delete leaf_گره;
            بازگشت;
        }
        or_گره->f_چپ_Child  = uset_گره->f_چپ_Child;
        or_گره->f_راست_Child = leaf_گره;
        or_گره->f_چپ_Child->fParent  = or_گره;
        or_گره->f_راست_Child->fParent = or_گره;
        uset_گره->f_چپ_Child = or_گره;
        or_گره->fParent = uset_گره;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_رقم_Char_Categories() ثابت {
    بازگشت f_Group_شمار + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() ثابت {
    بازگشت fDictCategoriesStart;
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
    برای (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        اگر (rlRange->f_رقم == category) {
            retVal = rlRange->fStartChar;
            بشکن;
        }
    }
    بازگشت retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#ifdef RBBI_DEBUG
تهی RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    صحیح                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    برای (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        برای (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_گره    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_گره->fParent;
            اگر (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                اگر (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
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
تهی RBBISetBuilder::printRangeGroups() {
    صحیح                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    برای (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        اگر (rlRange->fFirstInGroup) {
            صحیح group_رقم = rlRange->f_رقم;
            RBBIDebugPrintf("%2i  ", group_رقم);

            اگر (group_رقم >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            برای (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_گره    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_گره->fParent;
                اگر (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    اگر (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            برای (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->fNext) {
                اگر (tRange->f_رقم == rlRange->f_رقم) {
                    اگر (i++ % 5 == 0) {
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
تهی RBBISetBuilder::printSets() {
    صحیح                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    برای (i=0; ; i++) {
        RBBINode        *uset_گره;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_گره = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        اگر (uset_گره == nullptr) {
            بشکن;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_گره->fParent;
        اگر (setRef != nullptr) {
            varRef = setRef->fParent;
            اگر (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_گره->fText);
        RBBIDebugPrintf("\n");
        اگر (uset_گره->f_چپ_Child != nullptr) {
            RBBINode::print_درخت(uset_گره->f_چپ_Child, درست);
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
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_رقم {other.f_رقم},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    fIncludesSets = جدید UVector(status);
    اگر (این->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    برای (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        این->fIncludesSets->addElement(other.fIncludesSets->elementAt(i), status);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UErrorCode &status) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    fIncludesSets = جدید UVector(status);
    اگر (fIncludesSets == nullptr) {
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
تهی RangeDescriptor::split(UChar32 where, UErrorCode &status) {
    U_ASSERT(where>fStartChar && where<=fEndChar);
    RangeDescriptor *nr = جدید RangeDescriptor(*این, status);
    اگر(nr == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    اگر (U_FAILURE(status)) {
        delete nr;
        بازگشت;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->fStartChar = where;
    این->fEndChar = where-1;
    nr->fNext      = این->fNext;
    این->fNext    = nr;
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
منطقی RangeDescriptor::isDictionaryRange() {
    ایستا ثابت char16_t *dictionary = u"dictionary";
    برای (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_گره = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_گره->fParent;
        اگر (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            اگر (varRef && varRef->fType == RBBINode::varRef) {
                ثابت UnicodeString *setName = &varRef->fText;
                اگر (setName->compare(dictionary, -1) == 0) {
                    بازگشت درست;
                }
            }
        }
    }
    بازگشت نادرست;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
