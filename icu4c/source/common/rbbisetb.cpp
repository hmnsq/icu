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

ثابت int32_t k_Max_Char_Categories_For8_بٽ_Trie = 255;
//------------------------------------------------------------------------
//
//   Constructor
//
//------------------------------------------------------------------------
RBBISetBuilder::RBBISetBuilder(RBBIRuleBuilder *rb)
{
    fRB             = rb;
    f_حالت         = rb->f_حالت;
    fRangeList      = خالي_اشارو;
    fMutableTrie    = خالي_اشارو;
    fTrie           = خالي_اشارو;
    f_Trie_سائيز       = 0;
    f_گروهه_ڳڻپ     = 0;
}


//------------------------------------------------------------------------
//
//   Destructor
//
//------------------------------------------------------------------------
RBBISetBuilder::~RBBISetBuilder()
{
    RangeDescriptor   *ايندڙ_Range_Desc;

    // Walk through & delete the linked list of RangeDescriptors
    لكل (ايندڙ_Range_Desc = fRangeList; ايندڙ_Range_Desc!=خالي_اشارو;) {
        RangeDescriptor *r = ايندڙ_Range_Desc;
        ايندڙ_Range_Desc      = r->f_ايندڙ;
        ميٽيو r;
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
    RBBINode        *uset_ڳنڍ;
    RangeDescriptor *rlRange;

    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = جديد RangeDescriptor(*f_حالت); // will check for status here
    إذا (fRangeList == خالي_اشارو) {
        *f_حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    fRangeList->f_شروعات_Char    = 0;
    fRangeList->f_پڇاڙي_Char      = 0x10ffff;

    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    صحيح  ni;
    لكل (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ڳنڍ = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        إذا (uset_ڳنڍ==خالي_اشارو) {
            اكسر;
        }

        Unicode_سيٽ      *داخلا_سيٽ             = uset_ڳنڍ->f_داخلا_سيٽ;
        int32_t          داخلا_سيٽ_Range_ڳڻپ   = داخلا_سيٽ->get_Range_ڳڻپ();
        صحيح              داخلا_سيٽ_Range_اشاريو   = 0;
                         rlRange              = fRangeList;

        لكل (;;) {
            إذا (داخلا_سيٽ_Range_اشاريو >= داخلا_سيٽ_Range_ڳڻپ) {
                اكسر;
            }
            UChar32      داخلا_سيٽ_Range_Begin  = داخلا_سيٽ->get_Range_شروعات(داخلا_سيٽ_Range_اشاريو);
            UChar32      داخلا_سيٽ_Range_پڇاڙي    = داخلا_سيٽ->get_Range_پڇاڙي(داخلا_سيٽ_Range_اشاريو);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            طالما (rlRange->f_پڇاڙي_Char < داخلا_سيٽ_Range_Begin) {
                rlRange = rlRange->f_ايندڙ;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            إذا (rlRange->f_شروعات_Char < داخلا_سيٽ_Range_Begin) {
                rlRange->ورهايو(داخلا_سيٽ_Range_Begin, *f_حالت);
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
            إذا (rlRange->f_پڇاڙي_Char > داخلا_سيٽ_Range_پڇاڙي) {
                rlRange->ورهايو(داخلا_سيٽ_Range_پڇاڙي+1, *f_حالت);
                إذا (U_FAILURE(*f_حالت)) {
                    أرجع;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            إذا (rlRange->fIncludesSets->اشاريو_جو(uset_ڳنڍ) == -1) {
                rlRange->fIncludesSets->جوڙيو_Element(uset_ڳنڍ, *f_حالت);
                إذا (U_FAILURE(*f_حالت)) {
                    أرجع;
                }
            }

            // Advance over ranges that we are finished with.
            إذا (داخلا_سيٽ_Range_پڇاڙي == rlRange->f_پڇاڙي_Char) {
                داخلا_سيٽ_Range_اشاريو++;
            }
            rlRange = rlRange->f_ايندڙ;
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
    RangeDescriptor *rl_ڳولا_Range;
    int32_t dict_گروهه_ڳڻپ = 0;

    لكل (rlRange = fRangeList; rlRange!=خالي_اشارو; rlRange=rlRange->f_ايندڙ) {
        لكل (rl_ڳولا_Range=fRangeList; rl_ڳولا_Range != rlRange; rl_ڳولا_Range=rl_ڳولا_Range->f_ايندڙ) {
            إذا (rlRange->fIncludesSets->equals(*rl_ڳولا_Range->fIncludesSets)) {
                rlRange->f_انگ = rl_ڳولا_Range->f_انگ;
                rlRange->fIncludesDict = rl_ڳولا_Range->fIncludesDict;
                اكسر;
            }
        }
        إذا (rlRange->f_انگ == 0) {
            rlRange->f_پهريون_اندريون_گروهه = سچ;
            إذا (rlRange->isDictionaryRange()) {
                rlRange->f_انگ = ++dict_گروهه_ڳڻپ;
                rlRange->fIncludesDict = سچ;
            } وإلا {
                f_گروهه_ڳڻپ++;
                rlRange->f_انگ = f_گروهه_ڳڻپ+2;
                جوڙيو_Val_منزل_Sets(rlRange->fIncludesSets, rlRange->f_انگ);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    f_Dict_Categories_شروعات = f_گروهه_ڳڻپ + 3;
    لكل (rlRange = fRangeList; rlRange!=خالي_اشارو; rlRange=rlRange->f_ايندڙ) {
        إذا (rlRange->fIncludesDict) {
            rlRange->f_انگ += f_Dict_Categories_شروعات - 1;
            إذا (rlRange->f_پهريون_اندريون_گروهه) {
                جوڙيو_Val_منزل_Sets(rlRange->fIncludesSets, rlRange->f_انگ);
            }
        }
    }
    f_گروهه_ڳڻپ += dict_گروهه_ڳڻپ;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    لكل (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ڳنڍ = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        إذا (uset_ڳنڍ==خالي_اشارو) {
            اكسر;
        }
        Unicode_سيٽ      *داخلا_سيٽ = uset_ڳنڍ->f_داخلا_سيٽ;
        إذا (داخلا_سيٽ->شامل_آهي(eofString)) {
            جوڙيو_Val_منزل_سيٽ(uset_ڳنڍ, 1);
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

    لكل (RangeDescriptor *range = fRangeList; range!=خالي_اشارو && U_SUCCESS(*f_حالت); range=range->f_ايندڙ) {
        umutablecptrie_سيٽ_Range(fMutableTrie,
                                range->f_شروعات_Char,     // Range start
                                range->f_پڇاڙي_Char,       // Range end (inclusive)
                                range->f_انگ,           // value for range
                                f_حالت);
    }
}


فراغ RBBISetBuilder::ملايو_Categories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.ٻيو > categories.first);
    U_ASSERT((categories.first <  f_Dict_Categories_شروعات && categories.ٻيو <  f_Dict_Categories_شروعات) ||
             (categories.first >= f_Dict_Categories_شروعات && categories.ٻيو >= f_Dict_Categories_شروعات));

    لكل (RangeDescriptor *rd = fRangeList; rd != خالي_اشارو; rd = rd->f_ايندڙ) {
        int32_t range_انگ = rd->f_انگ;
        إذا (range_انگ == categories.ٻيو) {
            rd->f_انگ = categories.first;
        } وإلا إذا (range_انگ > categories.ٻيو) {
            rd->f_انگ--;
        }
    }
    --f_گروهه_ڳڻپ;
    إذا (categories.ٻيو <= f_Dict_Categories_شروعات) {
        --f_Dict_Categories_شروعات;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::get_Trie_سائيز()  {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع 0;
    }
    إذا (fTrie == خالي_اشارو) {
        منطقي use8_بٽ = get_انگ_Char_Categories() <= k_Max_Char_Categories_For8_بٽ_Trie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8_بٽ ? UCPTRIE_قدر_بٽ_8 : UCPTRIE_قدر_بٽ_16,
            f_حالت);
        UError_ڪوڊ بفر_حالت = *f_حالت;
        f_Trie_سائيز = ucptrie_منزل_ٻٽو(fTrie, خالي_اشارو, 0, &بفر_حالت);
        إذا (بفر_حالت != U_بفر_حد_کان_مٿي_ERROR && U_FAILURE(بفر_حالت)) {
            *f_حالت = بفر_حالت;
        }
    }
    أرجع f_Trie_سائيز;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
فراغ RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_منزل_ٻٽو(fTrie,
                     where,                //بفر
                     f_Trie_سائيز,            //صلاحيت
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
فراغ  RBBISetBuilder::جوڙيو_Val_منزل_Sets(UVector *sets, uint32_t val) {
    int32_t       ix;

    لكل (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_ڳنڍ = static_cast<RBBINode*>(sets->elementAt(ix));
        جوڙيو_Val_منزل_سيٽ(uset_ڳنڍ, val);
    }
}

فراغ  RBBISetBuilder::جوڙيو_Val_منزل_سيٽ(RBBINode *uset_ڳنڍ, uint32_t val) {
    RBBINode *پن_ڳنڍ = جديد RBBINode(RBBINode::پن_Char, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        ميٽيو پن_ڳنڍ;
        أرجع;
    }
    إذا (پن_ڳنڍ == خالي_اشارو) {
        *f_حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    پن_ڳنڍ->fVal = static_cast<بنانشان ننڍو>(val);
    إذا (uset_ڳنڍ->f_کاٻو_ٻار == خالي_اشارو) {
        uset_ڳنڍ->f_کاٻو_ٻار = پن_ڳنڍ;
        پن_ڳنڍ->f_والد    = uset_ڳنڍ;
    } وإلا {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_ڳنڍ = جديد RBBINode(RBBINode::opOr, *f_حالت);
        إذا (or_ڳنڍ == خالي_اشارو) {
            *f_حالت = U_يادگيري_ورهاست_ERROR;
        }
        إذا (U_FAILURE(*f_حالت)) {
            ميٽيو or_ڳنڍ;
            ميٽيو پن_ڳنڍ;
            أرجع;
        }
        or_ڳنڍ->f_کاٻو_ٻار  = uset_ڳنڍ->f_کاٻو_ٻار;
        or_ڳنڍ->f_ساڄو_ٻار = پن_ڳنڍ;
        or_ڳنڍ->f_کاٻو_ٻار->f_والد  = or_ڳنڍ;
        or_ڳنڍ->f_ساڄو_ٻار->f_والد = or_ڳنڍ;
        uset_ڳنڍ->f_کاٻو_ٻار = or_ڳنڍ;
        or_ڳنڍ->f_والد = uset_ڳنڍ;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_انگ_Char_Categories() ثابت {
    أرجع f_گروهه_ڳڻپ + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_Dict_Categories_شروعات() ثابت {
    أرجع f_Dict_Categories_شروعات;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::get_پهريون_Char(int32_t category) ثابت {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    لكل (rlRange = fRangeList; rlRange!=خالي_اشارو; rlRange=rlRange->f_ايندڙ) {
        إذا (rlRange->f_انگ == category) {
            retVal = rlRange->f_شروعات_Char;
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
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
فراغ RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    صحيح                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    لكل (rlRange = fRangeList; rlRange!=خالي_اشارو; rlRange=rlRange->f_ايندڙ) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->f_شروعات_Char, rlRange->f_پڇاڙي_Char);

        لكل (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_ڳنڍ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   سيٽ_Name {u"anon"};
            RBBINode       *سيٽ_Ref = uset_ڳنڍ->f_والد;
            إذا (سيٽ_Ref != خالي_اشارو) {
                RBBINode *varRef = سيٽ_Ref->f_والد;
                إذا (varRef != خالي_اشارو  &&  varRef->fType == RBBINode::varRef) {
                    سيٽ_Name = varRef->f_متن;
                }
            }
            RBBI_DEBUG_printUnicodeString(سيٽ_Name); RBBIDebugPrintf("  ");
        }
        RBBIDebugPrintf("\n");
    }
}
#جيڪڏهن_پورو


//------------------------------------------------------------------------
//
//   printRangeGroups     A debugging function.
//                        dump out all of the range groups.
//
//------------------------------------------------------------------------
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
فراغ RBBISetBuilder::printRangeGroups() {
    صحيح                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    لكل (RangeDescriptor *rlRange = fRangeList; rlRange!=خالي_اشارو; rlRange=rlRange->f_ايندڙ) {
        إذا (rlRange->f_پهريون_اندريون_گروهه) {
            صحيح گروهه_انگ = rlRange->f_انگ;
            RBBIDebugPrintf("%2i  ", گروهه_انگ);

            إذا (گروهه_انگ >= f_Dict_Categories_شروعات) { RBBIDebugPrintf(" <DICT> ");}

            لكل (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_ڳنڍ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   سيٽ_Name = UNICODE_STRING("anon", 4);
                RBBINode       *سيٽ_Ref = uset_ڳنڍ->f_والد;
                إذا (سيٽ_Ref != خالي_اشارو) {
                    RBBINode *varRef = سيٽ_Ref->f_والد;
                    إذا (varRef != خالي_اشارو  &&  varRef->fType == RBBINode::varRef) {
                        سيٽ_Name = varRef->f_متن;
                    }
                }
                RBBI_DEBUG_printUnicodeString(سيٽ_Name); RBBIDebugPrintf(" ");
            }

            i = 0;
            لكل (RangeDescriptor *tRange = rlRange; tRange != خالي_اشارو; tRange = tRange->f_ايندڙ) {
                إذا (tRange->f_انگ == rlRange->f_انگ) {
                    إذا (i++ % 5 == 0) {
                        RBBIDebugPrintf("\n    ");
                    }
                    RBBIDebugPrintf("  %05x-%05x", tRange->f_شروعات_Char, tRange->f_پڇاڙي_Char);
                }
            }
            RBBIDebugPrintf("\n");
        }
    }
    RBBIDebugPrintf("\n");
}
#جيڪڏهن_پورو


//------------------------------------------------------------------------
//
//   printSets          A debugging function.
//                      dump out all of the set definitions.
//
//------------------------------------------------------------------------
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
فراغ RBBISetBuilder::printSets() {
    صحيح                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    لكل (i=0; ; i++) {
        RBBINode        *uset_ڳنڍ;
        RBBINode        *سيٽ_Ref;
        RBBINode        *varRef;
        UnicodeString    سيٽ_Name;

        uset_ڳنڍ = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        إذا (uset_ڳنڍ == خالي_اشارو) {
            اكسر;
        }

        RBBIDebugPrintf("%3d    ", i);
        سيٽ_Name = UNICODE_STRING("anonymous", 9);
        سيٽ_Ref = uset_ڳنڍ->f_والد;
        إذا (سيٽ_Ref != خالي_اشارو) {
            varRef = سيٽ_Ref->f_والد;
            إذا (varRef != خالي_اشارو  &&  varRef->fType == RBBINode::varRef) {
                سيٽ_Name = varRef->f_متن;
            }
        }
        RBBI_DEBUG_printUnicodeString(سيٽ_Name);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_ڳنڍ->f_متن);
        RBBIDebugPrintf("\n");
        إذا (uset_ڳنڍ->f_کاٻو_ٻار != خالي_اشارو) {
            RBBINode::print_وڻ(uset_ڳنڍ->f_کاٻو_ٻار, سچ);
        }
    }
    RBBIDebugPrintf("\n");
}
#جيڪڏهن_پورو



//-------------------------------------------------------------------------------------
//
//  RangeDescriptor copy constructor
//
//-------------------------------------------------------------------------------------

RangeDescriptor::RangeDescriptor(ثابت RangeDescriptor &other, UError_ڪوڊ &حالت) :
        f_شروعات_Char(other.f_شروعات_Char), f_پڇاڙي_Char {other.f_پڇاڙي_Char}, f_انگ {other.f_انگ},
        fIncludesDict{other.fIncludesDict}, f_پهريون_اندريون_گروهه{other.f_پهريون_اندريون_گروهه} {

    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    fIncludesSets = جديد UVector(حالت);
    إذا (هذا->fIncludesSets == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
    }
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    لكل (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        هذا->fIncludesSets->جوڙيو_Element(other.fIncludesSets->elementAt(i), حالت);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    fIncludesSets = جديد UVector(حالت);
    إذا (fIncludesSets == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor Destructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::~RangeDescriptor() {
    ميٽيو  fIncludesSets;
    fIncludesSets = خالي_اشارو;
}

//-------------------------------------------------------------------------------------
//
//  RangeDesriptor::split()
//
//-------------------------------------------------------------------------------------
فراغ RangeDescriptor::ورهايو(UChar32 where, UError_ڪوڊ &حالت) {
    U_ASSERT(where>f_شروعات_Char && where<=f_پڇاڙي_Char);
    RangeDescriptor *nr = جديد RangeDescriptor(*هذا, حالت);
    إذا(nr == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    إذا (U_FAILURE(حالت)) {
        ميٽيو nr;
        أرجع;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->f_شروعات_Char = where;
    هذا->f_پڇاڙي_Char = where-1;
    nr->f_ايندڙ      = هذا->f_ايندڙ;
    هذا->f_ايندڙ    = nr;
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
        RBBINode* uset_ڳنڍ = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *سيٽ_Ref = uset_ڳنڍ->f_والد;
        إذا (سيٽ_Ref != خالي_اشارو) {
            RBBINode *varRef = سيٽ_Ref->f_والد;
            إذا (varRef && varRef->fType == RBBINode::varRef) {
                ثابت UnicodeString *سيٽ_Name = &varRef->f_متن;
                إذا (سيٽ_Name->compare(dictionary, -1) == 0) {
                    أرجع سچ;
                }
            }
        }
    }
    أرجع خطأ;
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */
