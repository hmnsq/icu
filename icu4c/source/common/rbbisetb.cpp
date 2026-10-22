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

#include "unicode/utypes.h"

#if !UCONFIG_NO_BREAK_ITERATION

#include "unicode/uniset.h"
#include "uvector.h"
#include "uassert.h"
#include "cmemory.h"
#include "cstring.h"

#include "rbbisetb.h"
#include "rbbinode.h"

U_NAMESPACE_BEGIN

const int32_t kMaxCharCategoriesFor8BitsTrie = 255;
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
    f_Group_बिलांग     = 0;
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
    for (nextRangeDesc = fRangeList; nextRangeDesc!=nullptr;) {
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
void RBBISetBuilder::buildRanges() {
    RBBINode        *uset_बुहोल;
    RangeDescriptor *rlRange;

    if (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {इलिम्बाग_Sets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = new RangeDescriptor(*fStatus); // will check for status here
    if (fRangeList == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        return;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->fEndChar      = 0x10ffff;

    if (U_FAILURE(*fStatus)) {
        return;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    int  ni;
    for (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_बुहोल = static_cast<RBBINode*>(this->fRB->fUSetNodes->elementAt(ni));
        if (uset_बुहोल==nullptr) {
            break;
        }

        UnicodeSet      *पासोक_Set             = uset_बुहोल->f_पासोक_Set;
        int32_t          पासोक_Set_Range_बिलांग   = पासोक_Set->get_Range_बिलांग();
        int              पासोक_Set_Range_Index   = 0;
                         rlRange              = fRangeList;

        for (;;) {
            if (पासोक_Set_Range_Index >= पासोक_Set_Range_बिलांग) {
                break;
            }
            UChar32      पासोक_Set_Range_Begin  = पासोक_Set->getRangeStart(पासोक_Set_Range_Index);
            UChar32      पासोक_Set_Range_End    = पासोक_Set->getRangeEnd(पासोक_Set_Range_Index);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            while (rlRange->fEndChar < पासोक_Set_Range_Begin) {
                rlRange = rlRange->fNext;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            if (rlRange->fStartChar < पासोक_Set_Range_Begin) {
                rlRange->split(पासोक_Set_Range_Begin, *fStatus);
                if (U_FAILURE(*fStatus)) {
                    return;
                }
                continue;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            if (rlRange->fEndChar > पासोक_Set_Range_End) {
                rlRange->split(पासोक_Set_Range_End+1, *fStatus);
                if (U_FAILURE(*fStatus)) {
                    return;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            if (rlRange->fIncludesSets->indexOf(uset_बुहोल) == -1) {
                rlRange->fIncludesSets->addElement(uset_बुहोल, *fStatus);
                if (U_FAILURE(*fStatus)) {
                    return;
                }
            }

            // Advance over ranges that we are finished with.
            if (पासोक_Set_Range_End == rlRange->fEndChar) {
                पासोक_Set_Range_Index++;
            }
            rlRange = rlRange->fNext;
        }
    }

    if (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { इलिम्बाग_Ranges();}

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
    RangeDescriptor *rl_हानाप_Range;
    int32_t dict_Group_बिलांग = 0;

    for (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        for (rl_हानाप_Range=fRangeList; rl_हानाप_Range != rlRange; rl_हानाप_Range=rl_हानाप_Range->fNext) {
            if (rlRange->fIncludesSets->equals(*rl_हानाप_Range->fIncludesSets)) {
                rlRange->f_नुमेरो = rl_हानाप_Range->f_नुमेरो;
                rlRange->fIncludesDict = rl_हानाप_Range->fIncludesDict;
                break;
            }
        }
        if (rlRange->f_नुमेरो == 0) {
            rlRange->fFirstInGroup = true;
            if (rlRange->isDictionaryRange()) {
                rlRange->f_नुमेरो = ++dict_Group_बिलांग;
                rlRange->fIncludesDict = true;
            } else {
                f_Group_बिलांग++;
                rlRange->f_नुमेरो = f_Group_बिलांग+2;
                addValToSets(rlRange->fIncludesSets, rlRange->f_नुमेरो);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_Group_बिलांग + 3;
    for (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        if (rlRange->fIncludesDict) {
            rlRange->f_नुमेरो += fDictCategoriesStart - 1;
            if (rlRange->fFirstInGroup) {
                addValToSets(rlRange->fIncludesSets, rlRange->f_नुमेरो);
            }
        }
    }
    f_Group_बिलांग += dict_Group_बिलांग;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    for (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_बुहोल = static_cast<RBBINode*>(this->fRB->fUSetNodes->elementAt(ni));
        if (uset_बुहोल==nullptr) {
            break;
        }
        UnicodeSet      *पासोक_Set = uset_बुहोल->f_पासोक_Set;
        if (पासोक_Set->contains(eofString)) {
            addValToSet(uset_बुहोल, 1);
        }
    }


    if (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {इलिम्बाग_Range_Groups();}
    if (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {इलिम्बाग_Sets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
void RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        fStatus);

    for (RangeDescriptor *range = fRangeList; range!=nullptr && U_SUCCESS(*fStatus); range=range->fNext) {
        umutablecptrie_setRange(fMutableTrie,
                                range->fStartChar,     // Range start
                                range->fEndChar,       // Range end (inclusive)
                                range->f_नुमेरो,           // value for range
                                fStatus);
    }
}


void RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    for (RangeDescriptor *rd = fRangeList; rd != nullptr; rd = rd->fNext) {
        int32_t range_नुमेरो = rd->f_नुमेरो;
        if (range_नुमेरो == categories.second) {
            rd->f_नुमेरो = categories.first;
        } else if (range_नुमेरो > categories.second) {
            rd->f_नुमेरो--;
        }
    }
    --f_Group_बिलांग;
    if (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::getTrieSize()  {
    if (U_FAILURE(*fStatus)) {
        return 0;
    }
    if (fTrie == nullptr) {
        bool use8Bits = get_नुमेरो_Char_Categories() <= kMaxCharCategoriesFor8BitsTrie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8Bits ? UCPTRIE_हालागा_BITS_8 : UCPTRIE_हालागा_BITS_16,
            fStatus);
        UErrorCode bufferStatus = *fStatus;
        fTrieSize = ucptrie_to_दालावाहान(fTrie, nullptr, 0, &bufferStatus);
        if (bufferStatus != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(bufferStatus)) {
            *fStatus = bufferStatus;
        }
    }
    return fTrieSize;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
void RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_to_दालावाहान(fTrie,
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
void  RBBISetBuilder::addValToSets(UVector *sets, uint32_t val) {
    int32_t       ix;

    for (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_बुहोल = static_cast<RBBINode*>(sets->elementAt(ix));
        addValToSet(uset_बुहोल, val);
    }
}

void  RBBISetBuilder::addValToSet(RBBINode *uset_बुहोल, uint32_t val) {
    RBBINode *leaf_बुहोल = new RBBINode(RBBINode::leafChar, *fStatus);
    if (U_FAILURE(*fStatus)) {
        delete leaf_बुहोल;
        return;
    }
    if (leaf_बुहोल == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        return;
    }
    leaf_बुहोल->fVal = static_cast<unsigned short>(val);
    if (uset_बुहोल->f_कालिवा_Child == nullptr) {
        uset_बुहोल->f_कालिवा_Child = leaf_बुहोल;
        leaf_बुहोल->fParent    = uset_बुहोल;
    } else {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_बुहोल = new RBBINode(RBBINode::opOr, *fStatus);
        if (or_बुहोल == nullptr) {
            *fStatus = U_MEMORY_ALLOCATION_ERROR;
        }
        if (U_FAILURE(*fStatus)) {
            delete or_बुहोल;
            delete leaf_बुहोल;
            return;
        }
        or_बुहोल->f_कालिवा_Child  = uset_बुहोल->f_कालिवा_Child;
        or_बुहोल->f_कानान_Child = leaf_बुहोल;
        or_बुहोल->f_कालिवा_Child->fParent  = or_बुहोल;
        or_बुहोल->f_कानान_Child->fParent = or_बुहोल;
        uset_बुहोल->f_कालिवा_Child = or_बुहोल;
        or_बुहोल->fParent = uset_बुहोल;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_नुमेरो_Char_Categories() const {
    return f_Group_बिलांग + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() const {
    return fDictCategoriesStart;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::getFirstChar(int32_t category) const {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    for (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        if (rlRange->f_नुमेरो == category) {
            retVal = rlRange->fStartChar;
            break;
        }
    }
    return retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#ifdef RBBI_DEBUG
void RBBISetBuilder::इलिम्बाग_Ranges() {
    RangeDescriptor       *rlRange;
    int                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    for (rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->fEndChar);

        for (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_बुहोल    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   setName {u"anon"};
            RBBINode       *setRef = uset_बुहोल->fParent;
            if (setRef != nullptr) {
                RBBINode *varRef = setRef->fParent;
                if (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                    setName = varRef->fText;
                }
            }
            RBBI_DEBUG_इलिम्बाग_Unicode_String(setName); RBBIDebugPrintf("  ");
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
void RBBISetBuilder::इलिम्बाग_Range_Groups() {
    int                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    for (RangeDescriptor *rlRange = fRangeList; rlRange!=nullptr; rlRange=rlRange->fNext) {
        if (rlRange->fFirstInGroup) {
            int group_नुमेरो = rlRange->f_नुमेरो;
            RBBIDebugPrintf("%2i  ", group_नुमेरो);

            if (group_नुमेरो >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            for (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_बुहोल    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   setName = UNICODE_STRING("anon", 4);
                RBBINode       *setRef = uset_बुहोल->fParent;
                if (setRef != nullptr) {
                    RBBINode *varRef = setRef->fParent;
                    if (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                        setName = varRef->fText;
                    }
                }
                RBBI_DEBUG_इलिम्बाग_Unicode_String(setName); RBBIDebugPrintf(" ");
            }

            i = 0;
            for (RangeDescriptor *tRange = rlRange; tRange != nullptr; tRange = tRange->fNext) {
                if (tRange->f_नुमेरो == rlRange->f_नुमेरो) {
                    if (i++ % 5 == 0) {
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
void RBBISetBuilder::इलिम्बाग_Sets() {
    int                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    for (i=0; ; i++) {
        RBBINode        *uset_बुहोल;
        RBBINode        *setRef;
        RBBINode        *varRef;
        UnicodeString    setName;

        uset_बुहोल = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        if (uset_बुहोल == nullptr) {
            break;
        }

        RBBIDebugPrintf("%3d    ", i);
        setName = UNICODE_STRING("anonymous", 9);
        setRef = uset_बुहोल->fParent;
        if (setRef != nullptr) {
            varRef = setRef->fParent;
            if (varRef != nullptr  &&  varRef->fType == RBBINode::varRef) {
                setName = varRef->fText;
            }
        }
        RBBI_DEBUG_इलिम्बाग_Unicode_String(setName);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_इलिम्बाग_Unicode_String(uset_बुहोल->fText);
        RBBIDebugPrintf("\n");
        if (uset_बुहोल->f_कालिवा_Child != nullptr) {
            RBBINode::इलिम्बाग_पुनो(uset_बुहोल->f_कालिवा_Child, true);
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

RangeDescriptor::RangeDescriptor(const RangeDescriptor &other, UErrorCode &status) :
        fStartChar(other.fStartChar), fEndChar {other.fEndChar}, f_नुमेरो {other.f_नुमेरो},
        fIncludesDict{other.fIncludesDict}, fFirstInGroup{other.fFirstInGroup} {

    if (U_FAILURE(status)) {
        return;
    }
    fIncludesSets = new UVector(status);
    if (this->fIncludesSets == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    if (U_FAILURE(status)) {
        return;
    }

    for (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        this->fIncludesSets->addElement(other.fIncludesSets->elementAt(i), status);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UErrorCode &status) {
    if (U_FAILURE(status)) {
        return;
    }
    fIncludesSets = new UVector(status);
    if (fIncludesSets == nullptr) {
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
void RangeDescriptor::split(UChar32 where, UErrorCode &status) {
    U_ASSERT(where>fStartChar && where<=fEndChar);
    RangeDescriptor *nr = new RangeDescriptor(*this, status);
    if(nr == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return;
    }
    if (U_FAILURE(status)) {
        delete nr;
        return;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->fStartChar = where;
    this->fEndChar = where-1;
    nr->fNext      = this->fNext;
    this->fNext    = nr;
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
    static const char16_t *dictionary = u"dictionary";
    for (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_बुहोल = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *setRef = uset_बुहोल->fParent;
        if (setRef != nullptr) {
            RBBINode *varRef = setRef->fParent;
            if (varRef && varRef->fType == RBBINode::varRef) {
                const UnicodeString *setName = &varRef->fText;
                if (setName->compare(dictionary, -1) == 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
