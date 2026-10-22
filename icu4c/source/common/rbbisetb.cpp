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

ثابت int32_t k_Max_Char_Categories_For8_بټونه_Trie = 255;
//------------------------------------------------------------------------
//
//   Constructor
//
//------------------------------------------------------------------------
RBBISetBuilder::RBBISetBuilder(RBBIRuleBuilder *rb)
{
    fRB             = rb;
    f_حالت         = rb->f_حالت;
    fRangeList      = تش_اشاره;
    fMutableTrie    = تش_اشاره;
    fTrie           = تش_اشاره;
    f_Trie_اندازه       = 0;
    f_ډله_شمېر     = 0;
}


//------------------------------------------------------------------------
//
//   Destructor
//
//------------------------------------------------------------------------
RBBISetBuilder::~RBBISetBuilder()
{
    RangeDescriptor   *بل_Range_Desc;

    // Walk through & delete the linked list of RangeDescriptors
    لكل (بل_Range_Desc = fRangeList; بل_Range_Desc!=تش_اشاره;) {
        RangeDescriptor *r = بل_Range_Desc;
        بل_Range_Desc      = r->f_بل;
        ړنګول r;
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
    RBBINode        *uset_غوټه;
    RangeDescriptor *rlRange;

    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = جديد RangeDescriptor(*f_حالت); // will check for status here
    إذا (fRangeList == تش_اشاره) {
        *f_حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    fRangeList->f_پیل_Char    = 0;
    fRangeList->f_پای_ټکی_Char      = 0x10ffff;

    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    صحيح  ni;
    لكل (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_غوټه = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        إذا (uset_غوټه==تش_اشاره) {
            اكسر;
        }

        Unicode_سېټ      *ننوت_سېټ             = uset_غوټه->f_ننوت_سېټ;
        int32_t          ننوت_سېټ_Range_شمېر   = ننوت_سېټ->get_Range_شمېر();
        صحيح              ننوت_سېټ_Range_شاخص   = 0;
                         rlRange              = fRangeList;

        لكل (;;) {
            إذا (ننوت_سېټ_Range_شاخص >= ننوت_سېټ_Range_شمېر) {
                اكسر;
            }
            UChar32      ننوت_سېټ_Range_Begin  = ننوت_سېټ->get_Range_پیل(ننوت_سېټ_Range_شاخص);
            UChar32      ننوت_سېټ_Range_پای_ټکی    = ننوت_سېټ->get_Range_پای_ټکی(ننوت_سېټ_Range_شاخص);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            طالما (rlRange->f_پای_ټکی_Char < ننوت_سېټ_Range_Begin) {
                rlRange = rlRange->f_بل;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            إذا (rlRange->f_پیل_Char < ننوت_سېټ_Range_Begin) {
                rlRange->وېشل(ننوت_سېټ_Range_Begin, *f_حالت);
                إذا (U_FAILURE(*f_حالت)) {
                    أرجع;
                }
                استمر;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            إذا (rlRange->f_پای_ټکی_Char > ننوت_سېټ_Range_پای_ټکی) {
                rlRange->وېشل(ننوت_سېټ_Range_پای_ټکی+1, *f_حالت);
                إذا (U_FAILURE(*f_حالت)) {
                    أرجع;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            إذا (rlRange->fIncludesSets->شاخص_د(uset_غوټه) == -1) {
                rlRange->fIncludesSets->جمع_کړه_Element(uset_غوټه, *f_حالت);
                إذا (U_FAILURE(*f_حالت)) {
                    أرجع;
                }
            }

            // Advance over ranges that we are finished with.
            إذا (ننوت_سېټ_Range_پای_ټکی == rlRange->f_پای_ټکی_Char) {
                ننوت_سېټ_Range_شاخص++;
            }
            rlRange = rlRange->f_بل;
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
    RangeDescriptor *rl_پلټنه_Range;
    int32_t dict_ډله_شمېر = 0;

    لكل (rlRange = fRangeList; rlRange!=تش_اشاره; rlRange=rlRange->f_بل) {
        لكل (rl_پلټنه_Range=fRangeList; rl_پلټنه_Range != rlRange; rl_پلټنه_Range=rl_پلټنه_Range->f_بل) {
            إذا (rlRange->fIncludesSets->equals(*rl_پلټنه_Range->fIncludesSets)) {
                rlRange->f_عدد = rl_پلټنه_Range->f_عدد;
                rlRange->fIncludesDict = rl_پلټنه_Range->fIncludesDict;
                اكسر;
            }
        }
        إذا (rlRange->f_عدد == 0) {
            rlRange->f_لومړی_دننه_ډله = ریښتیا;
            إذا (rlRange->isDictionaryRange()) {
                rlRange->f_عدد = ++dict_ډله_شمېر;
                rlRange->fIncludesDict = ریښتیا;
            } وإلا {
                f_ډله_شمېر++;
                rlRange->f_عدد = f_ډله_شمېر+2;
                جمع_کړه_Val_منزل_Sets(rlRange->fIncludesSets, rlRange->f_عدد);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    f_Dict_Categories_پیل = f_ډله_شمېر + 3;
    لكل (rlRange = fRangeList; rlRange!=تش_اشاره; rlRange=rlRange->f_بل) {
        إذا (rlRange->fIncludesDict) {
            rlRange->f_عدد += f_Dict_Categories_پیل - 1;
            إذا (rlRange->f_لومړی_دننه_ډله) {
                جمع_کړه_Val_منزل_Sets(rlRange->fIncludesSets, rlRange->f_عدد);
            }
        }
    }
    f_ډله_شمېر += dict_ډله_شمېر;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    لكل (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_غوټه = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        إذا (uset_غوټه==تش_اشاره) {
            اكسر;
        }
        Unicode_سېټ      *ننوت_سېټ = uset_غوټه->f_ننوت_سېټ;
        إذا (ننوت_سېټ->لري(eofString)) {
            جمع_کړه_Val_منزل_سېټ(uset_غوټه, 1);
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
                        f_حالت);

    لكل (RangeDescriptor *range = fRangeList; range!=تش_اشاره && U_SUCCESS(*f_حالت); range=range->f_بل) {
        umutablecptrie_سېټ_Range(fMutableTrie,
                                range->f_پیل_Char,     // Range start
                                range->f_پای_ټکی_Char,       // Range end (inclusive)
                                range->f_عدد,           // value for range
                                f_حالت);
    }
}


فراغ RBBISetBuilder::یوځای_Categories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.دوهم > categories.first);
    U_ASSERT((categories.first <  f_Dict_Categories_پیل && categories.دوهم <  f_Dict_Categories_پیل) ||
             (categories.first >= f_Dict_Categories_پیل && categories.دوهم >= f_Dict_Categories_پیل));

    لكل (RangeDescriptor *rd = fRangeList; rd != تش_اشاره; rd = rd->f_بل) {
        int32_t range_عدد = rd->f_عدد;
        إذا (range_عدد == categories.دوهم) {
            rd->f_عدد = categories.first;
        } وإلا إذا (range_عدد > categories.دوهم) {
            rd->f_عدد--;
        }
    }
    --f_ډله_شمېر;
    إذا (categories.دوهم <= f_Dict_Categories_پیل) {
        --f_Dict_Categories_پیل;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::get_Trie_اندازه()  {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع 0;
    }
    إذا (fTrie == تش_اشاره) {
        منطقي use8_بټونه = get_عدد_Char_Categories() <= k_Max_Char_Categories_For8_بټونه_Trie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8_بټونه ? UCPTRIE_ارزښت_بټونه_8 : UCPTRIE_ارزښت_بټونه_16,
            f_حالت);
        UError_کوډ بفر_حالت = *f_حالت;
        f_Trie_اندازه = ucptrie_منزل_دوګونی(fTrie, تش_اشاره, 0, &بفر_حالت);
        إذا (بفر_حالت != U_بفر_ډکېدنه_ERROR && U_FAILURE(بفر_حالت)) {
            *f_حالت = بفر_حالت;
        }
    }
    أرجع f_Trie_اندازه;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
فراغ RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_منزل_دوګونی(fTrie,
                     where,                //بفر
                     f_Trie_اندازه,            //ظرفیت
                     f_حالت);
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
فراغ  RBBISetBuilder::جمع_کړه_Val_منزل_Sets(UVector *sets, uint32_t val) {
    int32_t       ix;

    لكل (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_غوټه = static_cast<RBBINode*>(sets->elementAt(ix));
        جمع_کړه_Val_منزل_سېټ(uset_غوټه, val);
    }
}

فراغ  RBBISetBuilder::جمع_کړه_Val_منزل_سېټ(RBBINode *uset_غوټه, uint32_t val) {
    RBBINode *ونې_پاڼه_غوټه = جديد RBBINode(RBBINode::ونې_پاڼه_Char, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        ړنګول ونې_پاڼه_غوټه;
        أرجع;
    }
    إذا (ونې_پاڼه_غوټه == تش_اشاره) {
        *f_حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    ونې_پاڼه_غوټه->fVal = static_cast<بینښه لنډ>(val);
    إذا (uset_غوټه->f_کیڼ_ماشوم == تش_اشاره) {
        uset_غوټه->f_کیڼ_ماشوم = ونې_پاڼه_غوټه;
        ونې_پاڼه_غوټه->f_پلار    = uset_غوټه;
    } وإلا {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_غوټه = جديد RBBINode(RBBINode::opOr, *f_حالت);
        إذا (or_غوټه == تش_اشاره) {
            *f_حالت = U_حافظه_ویش_ERROR;
        }
        إذا (U_FAILURE(*f_حالت)) {
            ړنګول or_غوټه;
            ړنګول ونې_پاڼه_غوټه;
            أرجع;
        }
        or_غوټه->f_کیڼ_ماشوم  = uset_غوټه->f_کیڼ_ماشوم;
        or_غوټه->f_ښی_ماشوم = ونې_پاڼه_غوټه;
        or_غوټه->f_کیڼ_ماشوم->f_پلار  = or_غوټه;
        or_غوټه->f_ښی_ماشوم->f_پلار = or_غوټه;
        uset_غوټه->f_کیڼ_ماشوم = or_غوټه;
        or_غوټه->f_پلار = uset_غوټه;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_عدد_Char_Categories() ثابت {
    أرجع f_ډله_شمېر + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_Dict_Categories_پیل() ثابت {
    أرجع f_Dict_Categories_پیل;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::get_لومړی_Char(int32_t category) ثابت {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    لكل (rlRange = fRangeList; rlRange!=تش_اشاره; rlRange=rlRange->f_بل) {
        إذا (rlRange->f_عدد == category) {
            retVal = rlRange->f_پیل_Char;
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
#که_تعریف_شوی RBBI_DEBUG
فراغ RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    صحيح                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    لكل (rlRange = fRangeList; rlRange!=تش_اشاره; rlRange=rlRange->f_بل) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->f_پیل_Char, rlRange->f_پای_ټکی_Char);

        لكل (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_غوټه    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   سېټ_Name {u"anon"};
            RBBINode       *سېټ_Ref = uset_غوټه->f_پلار;
            إذا (سېټ_Ref != تش_اشاره) {
                RBBINode *varRef = سېټ_Ref->f_پلار;
                إذا (varRef != تش_اشاره  &&  varRef->fType == RBBINode::varRef) {
                    سېټ_Name = varRef->f_متن;
                }
            }
            RBBI_DEBUG_printUnicodeString(سېټ_Name); RBBIDebugPrintf("  ");
        }
        RBBIDebugPrintf("\n");
    }
}
#د_که_پای


//------------------------------------------------------------------------
//
//   printRangeGroups     A debugging function.
//                        dump out all of the range groups.
//
//------------------------------------------------------------------------
#که_تعریف_شوی RBBI_DEBUG
فراغ RBBISetBuilder::printRangeGroups() {
    صحيح                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    لكل (RangeDescriptor *rlRange = fRangeList; rlRange!=تش_اشاره; rlRange=rlRange->f_بل) {
        إذا (rlRange->f_لومړی_دننه_ډله) {
            صحيح ډله_عدد = rlRange->f_عدد;
            RBBIDebugPrintf("%2i  ", ډله_عدد);

            إذا (ډله_عدد >= f_Dict_Categories_پیل) { RBBIDebugPrintf(" <DICT> ");}

            لكل (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_غوټه    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   سېټ_Name = UNICODE_STRING("anon", 4);
                RBBINode       *سېټ_Ref = uset_غوټه->f_پلار;
                إذا (سېټ_Ref != تش_اشاره) {
                    RBBINode *varRef = سېټ_Ref->f_پلار;
                    إذا (varRef != تش_اشاره  &&  varRef->fType == RBBINode::varRef) {
                        سېټ_Name = varRef->f_متن;
                    }
                }
                RBBI_DEBUG_printUnicodeString(سېټ_Name); RBBIDebugPrintf(" ");
            }

            i = 0;
            لكل (RangeDescriptor *tRange = rlRange; tRange != تش_اشاره; tRange = tRange->f_بل) {
                إذا (tRange->f_عدد == rlRange->f_عدد) {
                    إذا (i++ % 5 == 0) {
                        RBBIDebugPrintf("\n    ");
                    }
                    RBBIDebugPrintf("  %05x-%05x", tRange->f_پیل_Char, tRange->f_پای_ټکی_Char);
                }
            }
            RBBIDebugPrintf("\n");
        }
    }
    RBBIDebugPrintf("\n");
}
#د_که_پای


//------------------------------------------------------------------------
//
//   printSets          A debugging function.
//                      dump out all of the set definitions.
//
//------------------------------------------------------------------------
#که_تعریف_شوی RBBI_DEBUG
فراغ RBBISetBuilder::printSets() {
    صحيح                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    لكل (i=0; ; i++) {
        RBBINode        *uset_غوټه;
        RBBINode        *سېټ_Ref;
        RBBINode        *varRef;
        UnicodeString    سېټ_Name;

        uset_غوټه = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        إذا (uset_غوټه == تش_اشاره) {
            اكسر;
        }

        RBBIDebugPrintf("%3d    ", i);
        سېټ_Name = UNICODE_STRING("anonymous", 9);
        سېټ_Ref = uset_غوټه->f_پلار;
        إذا (سېټ_Ref != تش_اشاره) {
            varRef = سېټ_Ref->f_پلار;
            إذا (varRef != تش_اشاره  &&  varRef->fType == RBBINode::varRef) {
                سېټ_Name = varRef->f_متن;
            }
        }
        RBBI_DEBUG_printUnicodeString(سېټ_Name);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_غوټه->f_متن);
        RBBIDebugPrintf("\n");
        إذا (uset_غوټه->f_کیڼ_ماشوم != تش_اشاره) {
            RBBINode::print_ونه(uset_غوټه->f_کیڼ_ماشوم, ریښتیا);
        }
    }
    RBBIDebugPrintf("\n");
}
#د_که_پای



//-------------------------------------------------------------------------------------
//
//  RangeDescriptor copy constructor
//
//-------------------------------------------------------------------------------------

RangeDescriptor::RangeDescriptor(ثابت RangeDescriptor &other, UError_کوډ &حالت) :
        f_پیل_Char(other.f_پیل_Char), f_پای_ټکی_Char {other.f_پای_ټکی_Char}, f_عدد {other.f_عدد},
        fIncludesDict{other.fIncludesDict}, f_لومړی_دننه_ډله{other.f_لومړی_دننه_ډله} {

    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    fIncludesSets = جديد UVector(حالت);
    إذا (هذا->fIncludesSets == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
    }
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    لكل (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        هذا->fIncludesSets->جمع_کړه_Element(other.fIncludesSets->elementAt(i), حالت);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    fIncludesSets = جديد UVector(حالت);
    إذا (fIncludesSets == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor Destructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::~RangeDescriptor() {
    ړنګول  fIncludesSets;
    fIncludesSets = تش_اشاره;
}

//-------------------------------------------------------------------------------------
//
//  RangeDesriptor::split()
//
//-------------------------------------------------------------------------------------
فراغ RangeDescriptor::وېشل(UChar32 where, UError_کوډ &حالت) {
    U_ASSERT(where>f_پیل_Char && where<=f_پای_ټکی_Char);
    RangeDescriptor *nr = جديد RangeDescriptor(*هذا, حالت);
    إذا(nr == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    إذا (U_FAILURE(حالت)) {
        ړنګول nr;
        أرجع;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->f_پیل_Char = where;
    هذا->f_پای_ټکی_Char = where-1;
    nr->f_بل      = هذا->f_بل;
    هذا->f_بل    = nr;
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
        RBBINode* uset_غوټه = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *سېټ_Ref = uset_غوټه->f_پلار;
        إذا (سېټ_Ref != تش_اشاره) {
            RBBINode *varRef = سېټ_Ref->f_پلار;
            إذا (varRef && varRef->fType == RBBINode::varRef) {
                ثابت UnicodeString *سېټ_Name = &varRef->f_متن;
                إذا (سېټ_Name->compare(dictionary, -1) == 0) {
                    أرجع ریښتیا;
                }
            }
        }
    }
    أرجع خطأ;
}

U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */
