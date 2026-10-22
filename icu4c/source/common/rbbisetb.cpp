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

ثابت int32_t k_Max_Char_Categories_For8_بٹ_Trie = 255;
//------------------------------------------------------------------------
//
//   Constructor
//
//------------------------------------------------------------------------
RBBISetBuilder::RBBISetBuilder(RBBIRuleBuilder *rb)
{
    fRB             = rb;
    f_حالت         = rb->f_حالت;
    fRangeList      = خالی_سوچک;
    fMutableTrie    = خالی_سوچک;
    fTrie           = خالی_سوچک;
    f_Trie_ناپ       = 0;
    f_گروہ_گنتی     = 0;
}


//------------------------------------------------------------------------
//
//   Destructor
//
//------------------------------------------------------------------------
RBBISetBuilder::~RBBISetBuilder()
{
    RangeDescriptor   *اگلا_Range_Desc;

    // Walk through & delete the linked list of RangeDescriptors
    لكل (اگلا_Range_Desc = fRangeList; اگلا_Range_Desc!=خالی_سوچک;) {
        RangeDescriptor *r = اگلا_Range_Desc;
        اگلا_Range_Desc      = r->f_اگلا;
        مٹاؤ r;
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
    RBBINode        *uset_گرہ;
    RangeDescriptor *rlRange;

    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = جديد RangeDescriptor(*f_حالت); // will check for status here
    إذا (fRangeList == خالی_سوچک) {
        *f_حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    fRangeList->f_شروع_Char    = 0;
    fRangeList->f_سرا_Char      = 0x10ffff;

    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    صحيح  ni;
    لكل (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_گرہ = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        إذا (uset_گرہ==خالی_سوچک) {
            اكسر;
        }

        Unicode_سیٹ      *داخلہ_سیٹ             = uset_گرہ->f_داخلہ_سیٹ;
        int32_t          داخلہ_سیٹ_Range_گنتی   = داخلہ_سیٹ->get_Range_گنتی();
        صحيح              داخلہ_سیٹ_Range_اشاریہ   = 0;
                         rlRange              = fRangeList;

        لكل (;;) {
            إذا (داخلہ_سیٹ_Range_اشاریہ >= داخلہ_سیٹ_Range_گنتی) {
                اكسر;
            }
            UChar32      داخلہ_سیٹ_Range_Begin  = داخلہ_سیٹ->get_Range_شروع(داخلہ_سیٹ_Range_اشاریہ);
            UChar32      داخلہ_سیٹ_Range_سرا    = داخلہ_سیٹ->get_Range_سرا(داخلہ_سیٹ_Range_اشاریہ);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            طالما (rlRange->f_سرا_Char < داخلہ_سیٹ_Range_Begin) {
                rlRange = rlRange->f_اگلا;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            إذا (rlRange->f_شروع_Char < داخلہ_سیٹ_Range_Begin) {
                rlRange->ونڈو(داخلہ_سیٹ_Range_Begin, *f_حالت);
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
            إذا (rlRange->f_سرا_Char > داخلہ_سیٹ_Range_سرا) {
                rlRange->ونڈو(داخلہ_سیٹ_Range_سرا+1, *f_حالت);
                إذا (U_FAILURE(*f_حالت)) {
                    أرجع;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            إذا (rlRange->fIncludesSets->اشاریہ_دا(uset_گرہ) == -1) {
                rlRange->fIncludesSets->جوڑو_Element(uset_گرہ, *f_حالت);
                إذا (U_FAILURE(*f_حالت)) {
                    أرجع;
                }
            }

            // Advance over ranges that we are finished with.
            إذا (داخلہ_سیٹ_Range_سرا == rlRange->f_سرا_Char) {
                داخلہ_سیٹ_Range_اشاریہ++;
            }
            rlRange = rlRange->f_اگلا;
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
    RangeDescriptor *rl_تلاش_Range;
    int32_t dict_گروہ_گنتی = 0;

    لكل (rlRange = fRangeList; rlRange!=خالی_سوچک; rlRange=rlRange->f_اگلا) {
        لكل (rl_تلاش_Range=fRangeList; rl_تلاش_Range != rlRange; rl_تلاش_Range=rl_تلاش_Range->f_اگلا) {
            إذا (rlRange->fIncludesSets->equals(*rl_تلاش_Range->fIncludesSets)) {
                rlRange->f_عدد = rl_تلاش_Range->f_عدد;
                rlRange->fIncludesDict = rl_تلاش_Range->fIncludesDict;
                اكسر;
            }
        }
        إذا (rlRange->f_عدد == 0) {
            rlRange->f_پہلا_وچ_گروہ = سچ;
            إذا (rlRange->isDictionaryRange()) {
                rlRange->f_عدد = ++dict_گروہ_گنتی;
                rlRange->fIncludesDict = سچ;
            } وإلا {
                f_گروہ_گنتی++;
                rlRange->f_عدد = f_گروہ_گنتی+2;
                جوڑو_Val_منزل_Sets(rlRange->fIncludesSets, rlRange->f_عدد);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    f_Dict_Categories_شروع = f_گروہ_گنتی + 3;
    لكل (rlRange = fRangeList; rlRange!=خالی_سوچک; rlRange=rlRange->f_اگلا) {
        إذا (rlRange->fIncludesDict) {
            rlRange->f_عدد += f_Dict_Categories_شروع - 1;
            إذا (rlRange->f_پہلا_وچ_گروہ) {
                جوڑو_Val_منزل_Sets(rlRange->fIncludesSets, rlRange->f_عدد);
            }
        }
    }
    f_گروہ_گنتی += dict_گروہ_گنتی;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    لكل (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_گرہ = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        إذا (uset_گرہ==خالی_سوچک) {
            اكسر;
        }
        Unicode_سیٹ      *داخلہ_سیٹ = uset_گرہ->f_داخلہ_سیٹ;
        إذا (داخلہ_سیٹ->شامل_اے(eofString)) {
            جوڑو_Val_منزل_سیٹ(uset_گرہ, 1);
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

    لكل (RangeDescriptor *range = fRangeList; range!=خالی_سوچک && U_SUCCESS(*f_حالت); range=range->f_اگلا) {
        umutablecptrie_سیٹ_Range(fMutableTrie,
                                range->f_شروع_Char,     // Range start
                                range->f_سرا_Char,       // Range end (inclusive)
                                range->f_عدد,           // value for range
                                f_حالت);
    }
}


فراغ RBBISetBuilder::ملاؤ_Categories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.دوجا > categories.first);
    U_ASSERT((categories.first <  f_Dict_Categories_شروع && categories.دوجا <  f_Dict_Categories_شروع) ||
             (categories.first >= f_Dict_Categories_شروع && categories.دوجا >= f_Dict_Categories_شروع));

    لكل (RangeDescriptor *rd = fRangeList; rd != خالی_سوچک; rd = rd->f_اگلا) {
        int32_t range_عدد = rd->f_عدد;
        إذا (range_عدد == categories.دوجا) {
            rd->f_عدد = categories.first;
        } وإلا إذا (range_عدد > categories.دوجا) {
            rd->f_عدد--;
        }
    }
    --f_گروہ_گنتی;
    إذا (categories.دوجا <= f_Dict_Categories_شروع) {
        --f_Dict_Categories_شروع;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::get_Trie_ناپ()  {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع 0;
    }
    إذا (fTrie == خالی_سوچک) {
        منطقي use8_بٹ = get_عدد_Char_Categories() <= k_Max_Char_Categories_For8_بٹ_Trie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8_بٹ ? UCPTRIE_قدر_بٹ_8 : UCPTRIE_قدر_بٹ_16,
            f_حالت);
        UError_کوڈ بفر_حالت = *f_حالت;
        f_Trie_ناپ = ucptrie_منزل_ثنائی(fTrie, خالی_سوچک, 0, &بفر_حالت);
        إذا (بفر_حالت != U_بفر_حد_توں_ودھ_ERROR && U_FAILURE(بفر_حالت)) {
            *f_حالت = بفر_حالت;
        }
    }
    أرجع f_Trie_ناپ;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
فراغ RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_منزل_ثنائی(fTrie,
                     where,                //بفر
                     f_Trie_ناپ,            //سمرتھا
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
فراغ  RBBISetBuilder::جوڑو_Val_منزل_Sets(UVector *sets, uint32_t val) {
    int32_t       ix;

    لكل (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_گرہ = static_cast<RBBINode*>(sets->elementAt(ix));
        جوڑو_Val_منزل_سیٹ(uset_گرہ, val);
    }
}

فراغ  RBBISetBuilder::جوڑو_Val_منزل_سیٹ(RBBINode *uset_گرہ, uint32_t val) {
    RBBINode *پتی_گرہ = جديد RBBINode(RBBINode::پتی_Char, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        مٹاؤ پتی_گرہ;
        أرجع;
    }
    إذا (پتی_گرہ == خالی_سوچک) {
        *f_حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    پتی_گرہ->fVal = static_cast<بنانشانی چھوٹا>(val);
    إذا (uset_گرہ->f_بایاں_بچہ == خالی_سوچک) {
        uset_گرہ->f_بایاں_بچہ = پتی_گرہ;
        پتی_گرہ->f_ماپا    = uset_گرہ;
    } وإلا {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_گرہ = جديد RBBINode(RBBINode::opOr, *f_حالت);
        إذا (or_گرہ == خالی_سوچک) {
            *f_حالت = U_حافظہ_ونڈ_ERROR;
        }
        إذا (U_FAILURE(*f_حالت)) {
            مٹاؤ or_گرہ;
            مٹاؤ پتی_گرہ;
            أرجع;
        }
        or_گرہ->f_بایاں_بچہ  = uset_گرہ->f_بایاں_بچہ;
        or_گرہ->f_دایاں_بچہ = پتی_گرہ;
        or_گرہ->f_بایاں_بچہ->f_ماپا  = or_گرہ;
        or_گرہ->f_دایاں_بچہ->f_ماپا = or_گرہ;
        uset_گرہ->f_بایاں_بچہ = or_گرہ;
        or_گرہ->f_ماپا = uset_گرہ;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_عدد_Char_Categories() ثابت {
    أرجع f_گروہ_گنتی + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_Dict_Categories_شروع() ثابت {
    أرجع f_Dict_Categories_شروع;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::get_پہلا_Char(int32_t category) ثابت {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    لكل (rlRange = fRangeList; rlRange!=خالی_سوچک; rlRange=rlRange->f_اگلا) {
        إذا (rlRange->f_عدد == category) {
            retVal = rlRange->f_شروع_Char;
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
#جے_تعریف_شدہ RBBI_DEBUG
فراغ RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    صحيح                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    لكل (rlRange = fRangeList; rlRange!=خالی_سوچک; rlRange=rlRange->f_اگلا) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->f_شروع_Char, rlRange->f_سرا_Char);

        لكل (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_گرہ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   سیٹ_Name {u"anon"};
            RBBINode       *سیٹ_Ref = uset_گرہ->f_ماپا;
            إذا (سیٹ_Ref != خالی_سوچک) {
                RBBINode *varRef = سیٹ_Ref->f_ماپا;
                إذا (varRef != خالی_سوچک  &&  varRef->fType == RBBINode::varRef) {
                    سیٹ_Name = varRef->f_متن;
                }
            }
            RBBI_DEBUG_printUnicodeString(سیٹ_Name); RBBIDebugPrintf("  ");
        }
        RBBIDebugPrintf("\n");
    }
}
#جے_انت


//------------------------------------------------------------------------
//
//   printRangeGroups     A debugging function.
//                        dump out all of the range groups.
//
//------------------------------------------------------------------------
#جے_تعریف_شدہ RBBI_DEBUG
فراغ RBBISetBuilder::printRangeGroups() {
    صحيح                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    لكل (RangeDescriptor *rlRange = fRangeList; rlRange!=خالی_سوچک; rlRange=rlRange->f_اگلا) {
        إذا (rlRange->f_پہلا_وچ_گروہ) {
            صحيح گروہ_عدد = rlRange->f_عدد;
            RBBIDebugPrintf("%2i  ", گروہ_عدد);

            إذا (گروہ_عدد >= f_Dict_Categories_شروع) { RBBIDebugPrintf(" <DICT> ");}

            لكل (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_گرہ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   سیٹ_Name = UNICODE_STRING("anon", 4);
                RBBINode       *سیٹ_Ref = uset_گرہ->f_ماپا;
                إذا (سیٹ_Ref != خالی_سوچک) {
                    RBBINode *varRef = سیٹ_Ref->f_ماپا;
                    إذا (varRef != خالی_سوچک  &&  varRef->fType == RBBINode::varRef) {
                        سیٹ_Name = varRef->f_متن;
                    }
                }
                RBBI_DEBUG_printUnicodeString(سیٹ_Name); RBBIDebugPrintf(" ");
            }

            i = 0;
            لكل (RangeDescriptor *tRange = rlRange; tRange != خالی_سوچک; tRange = tRange->f_اگلا) {
                إذا (tRange->f_عدد == rlRange->f_عدد) {
                    إذا (i++ % 5 == 0) {
                        RBBIDebugPrintf("\n    ");
                    }
                    RBBIDebugPrintf("  %05x-%05x", tRange->f_شروع_Char, tRange->f_سرا_Char);
                }
            }
            RBBIDebugPrintf("\n");
        }
    }
    RBBIDebugPrintf("\n");
}
#جے_انت


//------------------------------------------------------------------------
//
//   printSets          A debugging function.
//                      dump out all of the set definitions.
//
//------------------------------------------------------------------------
#جے_تعریف_شدہ RBBI_DEBUG
فراغ RBBISetBuilder::printSets() {
    صحيح                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    لكل (i=0; ; i++) {
        RBBINode        *uset_گرہ;
        RBBINode        *سیٹ_Ref;
        RBBINode        *varRef;
        UnicodeString    سیٹ_Name;

        uset_گرہ = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        إذا (uset_گرہ == خالی_سوچک) {
            اكسر;
        }

        RBBIDebugPrintf("%3d    ", i);
        سیٹ_Name = UNICODE_STRING("anonymous", 9);
        سیٹ_Ref = uset_گرہ->f_ماپا;
        إذا (سیٹ_Ref != خالی_سوچک) {
            varRef = سیٹ_Ref->f_ماپا;
            إذا (varRef != خالی_سوچک  &&  varRef->fType == RBBINode::varRef) {
                سیٹ_Name = varRef->f_متن;
            }
        }
        RBBI_DEBUG_printUnicodeString(سیٹ_Name);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_گرہ->f_متن);
        RBBIDebugPrintf("\n");
        إذا (uset_گرہ->f_بایاں_بچہ != خالی_سوچک) {
            RBBINode::print_درخت(uset_گرہ->f_بایاں_بچہ, سچ);
        }
    }
    RBBIDebugPrintf("\n");
}
#جے_انت



//-------------------------------------------------------------------------------------
//
//  RangeDescriptor copy constructor
//
//-------------------------------------------------------------------------------------

RangeDescriptor::RangeDescriptor(ثابت RangeDescriptor &other, UError_کوڈ &حالت) :
        f_شروع_Char(other.f_شروع_Char), f_سرا_Char {other.f_سرا_Char}, f_عدد {other.f_عدد},
        fIncludesDict{other.fIncludesDict}, f_پہلا_وچ_گروہ{other.f_پہلا_وچ_گروہ} {

    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    fIncludesSets = جديد UVector(حالت);
    إذا (هذا->fIncludesSets == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
    }
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    لكل (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        هذا->fIncludesSets->جوڑو_Element(other.fIncludesSets->elementAt(i), حالت);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    fIncludesSets = جديد UVector(حالت);
    إذا (fIncludesSets == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor Destructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::~RangeDescriptor() {
    مٹاؤ  fIncludesSets;
    fIncludesSets = خالی_سوچک;
}

//-------------------------------------------------------------------------------------
//
//  RangeDesriptor::split()
//
//-------------------------------------------------------------------------------------
فراغ RangeDescriptor::ونڈو(UChar32 where, UError_کوڈ &حالت) {
    U_ASSERT(where>f_شروع_Char && where<=f_سرا_Char);
    RangeDescriptor *nr = جديد RangeDescriptor(*هذا, حالت);
    إذا(nr == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    إذا (U_FAILURE(حالت)) {
        مٹاؤ nr;
        أرجع;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->f_شروع_Char = where;
    هذا->f_سرا_Char = where-1;
    nr->f_اگلا      = هذا->f_اگلا;
    هذا->f_اگلا    = nr;
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
        RBBINode* uset_گرہ = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *سیٹ_Ref = uset_گرہ->f_ماپا;
        إذا (سیٹ_Ref != خالی_سوچک) {
            RBBINode *varRef = سیٹ_Ref->f_ماپا;
            إذا (varRef && varRef->fType == RBBINode::varRef) {
                ثابت UnicodeString *سیٹ_Name = &varRef->f_متن;
                إذا (سیٹ_Name->compare(dictionary, -1) == 0) {
                    أرجع سچ;
                }
            }
        }
    }
    أرجع خطأ;
}

U_NAMESPACE_سرا

#جے_انت /* #if !UCONFIG_NO_BREAK_ITERATION */
