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
    fRangeList      = خالی_اشاریہ;
    fMutableTrie    = خالی_اشاریہ;
    fTrie           = خالی_اشاریہ;
    f_Trie_سائز       = 0;
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
    برائے (اگلا_Range_Desc = fRangeList; اگلا_Range_Desc!=خالی_اشاریہ;) {
        RangeDescriptor *r = اگلا_Range_Desc;
        اگلا_Range_Desc      = r->f_اگلا;
        حذف r;
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
خالی RBBISetBuilder::buildRanges() {
    RBBINode        *uset_گرہ;
    RangeDescriptor *rlRange;

    اگر (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = نیا RangeDescriptor(*f_حالت); // will check for status here
    اگر (fRangeList == خالی_اشاریہ) {
        *f_حالت = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    fRangeList->f_آغاز_Char    = 0;
    fRangeList->f_انتہا_Char      = 0x10ffff;

    اگر (U_FAILURE(*f_حالت)) {
        واپس;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    صحیح  ni;
    برائے (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_گرہ = static_cast<RBBINode*>(یہ->fRB->fUSetNodes->elementAt(ni));
        اگر (uset_گرہ==خالی_اشاریہ) {
            توڑو;
        }

        Unicode_سیٹ      *داخلہ_سیٹ             = uset_گرہ->f_داخلہ_سیٹ;
        int32_t          داخلہ_سیٹ_Range_گنتی   = داخلہ_سیٹ->get_Range_گنتی();
        صحیح              داخلہ_سیٹ_Range_اشاریہ   = 0;
                         rlRange              = fRangeList;

        برائے (;;) {
            اگر (داخلہ_سیٹ_Range_اشاریہ >= داخلہ_سیٹ_Range_گنتی) {
                توڑو;
            }
            UChar32      داخلہ_سیٹ_Range_Begin  = داخلہ_سیٹ->get_Range_آغاز(داخلہ_سیٹ_Range_اشاریہ);
            UChar32      داخلہ_سیٹ_Range_انتہا    = داخلہ_سیٹ->get_Range_انتہا(داخلہ_سیٹ_Range_اشاریہ);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            جب (rlRange->f_انتہا_Char < داخلہ_سیٹ_Range_Begin) {
                rlRange = rlRange->f_اگلا;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            اگر (rlRange->f_آغاز_Char < داخلہ_سیٹ_Range_Begin) {
                rlRange->بانٹو(داخلہ_سیٹ_Range_Begin, *f_حالت);
                اگر (U_FAILURE(*f_حالت)) {
                    واپس;
                }
                جاری;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            اگر (rlRange->f_انتہا_Char > داخلہ_سیٹ_Range_انتہا) {
                rlRange->بانٹو(داخلہ_سیٹ_Range_انتہا+1, *f_حالت);
                اگر (U_FAILURE(*f_حالت)) {
                    واپس;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            اگر (rlRange->fIncludesSets->اشاریہ_کا(uset_گرہ) == -1) {
                rlRange->fIncludesSets->جوڑو_Element(uset_گرہ, *f_حالت);
                اگر (U_FAILURE(*f_حالت)) {
                    واپس;
                }
            }

            // Advance over ranges that we are finished with.
            اگر (داخلہ_سیٹ_Range_انتہا == rlRange->f_انتہا_Char) {
                داخلہ_سیٹ_Range_اشاریہ++;
            }
            rlRange = rlRange->f_اگلا;
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
    RangeDescriptor *rl_تلاش_Range;
    int32_t dict_گروہ_گنتی = 0;

    برائے (rlRange = fRangeList; rlRange!=خالی_اشاریہ; rlRange=rlRange->f_اگلا) {
        برائے (rl_تلاش_Range=fRangeList; rl_تلاش_Range != rlRange; rl_تلاش_Range=rl_تلاش_Range->f_اگلا) {
            اگر (rlRange->fIncludesSets->equals(*rl_تلاش_Range->fIncludesSets)) {
                rlRange->f_عدد = rl_تلاش_Range->f_عدد;
                rlRange->fIncludesDict = rl_تلاش_Range->fIncludesDict;
                توڑو;
            }
        }
        اگر (rlRange->f_عدد == 0) {
            rlRange->f_پہلا_اندر_گروہ = سچ;
            اگر (rlRange->isDictionaryRange()) {
                rlRange->f_عدد = ++dict_گروہ_گنتی;
                rlRange->fIncludesDict = سچ;
            } ورنہ {
                f_گروہ_گنتی++;
                rlRange->f_عدد = f_گروہ_گنتی+2;
                جوڑو_Val_مقصد_Sets(rlRange->fIncludesSets, rlRange->f_عدد);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    f_Dict_Categories_آغاز = f_گروہ_گنتی + 3;
    برائے (rlRange = fRangeList; rlRange!=خالی_اشاریہ; rlRange=rlRange->f_اگلا) {
        اگر (rlRange->fIncludesDict) {
            rlRange->f_عدد += f_Dict_Categories_آغاز - 1;
            اگر (rlRange->f_پہلا_اندر_گروہ) {
                جوڑو_Val_مقصد_Sets(rlRange->fIncludesSets, rlRange->f_عدد);
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
    برائے (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_گرہ = static_cast<RBBINode*>(یہ->fRB->fUSetNodes->elementAt(ni));
        اگر (uset_گرہ==خالی_اشاریہ) {
            توڑو;
        }
        Unicode_سیٹ      *داخلہ_سیٹ = uset_گرہ->f_داخلہ_سیٹ;
        اگر (داخلہ_سیٹ->شامل_ہے(eofString)) {
            جوڑو_Val_مقصد_سیٹ(uset_گرہ, 1);
        }
    }


    اگر (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    اگر (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
خالی RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        f_حالت);

    برائے (RangeDescriptor *range = fRangeList; range!=خالی_اشاریہ && U_SUCCESS(*f_حالت); range=range->f_اگلا) {
        umutablecptrie_سیٹ_Range(fMutableTrie,
                                range->f_آغاز_Char,     // Range start
                                range->f_انتہا_Char,       // Range end (inclusive)
                                range->f_عدد,           // value for range
                                f_حالت);
    }
}


خالی RBBISetBuilder::ملاؤ_Categories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.دوسرا > categories.first);
    U_ASSERT((categories.first <  f_Dict_Categories_آغاز && categories.دوسرا <  f_Dict_Categories_آغاز) ||
             (categories.first >= f_Dict_Categories_آغاز && categories.دوسرا >= f_Dict_Categories_آغاز));

    برائے (RangeDescriptor *rd = fRangeList; rd != خالی_اشاریہ; rd = rd->f_اگلا) {
        int32_t range_عدد = rd->f_عدد;
        اگر (range_عدد == categories.دوسرا) {
            rd->f_عدد = categories.first;
        } ورنہ اگر (range_عدد > categories.دوسرا) {
            rd->f_عدد--;
        }
    }
    --f_گروہ_گنتی;
    اگر (categories.دوسرا <= f_Dict_Categories_آغاز) {
        --f_Dict_Categories_آغاز;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::get_Trie_سائز()  {
    اگر (U_FAILURE(*f_حالت)) {
        واپس 0;
    }
    اگر (fTrie == خالی_اشاریہ) {
        منطقی use8_بٹ = get_عدد_Char_Categories() <= k_Max_Char_Categories_For8_بٹ_Trie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8_بٹ ? UCPTRIE_قدر_بٹ_8 : UCPTRIE_قدر_بٹ_16,
            f_حالت);
        UError_رمز بفر_حالت = *f_حالت;
        f_Trie_سائز = ucptrie_مقصد_ثنائی(fTrie, خالی_اشاریہ, 0, &بفر_حالت);
        اگر (بفر_حالت != U_بفر_حد_سے_تجاوز_ERROR && U_FAILURE(بفر_حالت)) {
            *f_حالت = بفر_حالت;
        }
    }
    واپس f_Trie_سائز;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
خالی RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_مقصد_ثنائی(fTrie,
                     where,                //بفر
                     f_Trie_سائز,            //استعداد
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
خالی  RBBISetBuilder::جوڑو_Val_مقصد_Sets(UVector *sets, uint32_t val) {
    int32_t       ix;

    برائے (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_گرہ = static_cast<RBBINode*>(sets->elementAt(ix));
        جوڑو_Val_مقصد_سیٹ(uset_گرہ, val);
    }
}

خالی  RBBISetBuilder::جوڑو_Val_مقصد_سیٹ(RBBINode *uset_گرہ, uint32_t val) {
    RBBINode *برگ_گرہ = نیا RBBINode(RBBINode::برگ_Char, *f_حالت);
    اگر (U_FAILURE(*f_حالت)) {
        حذف برگ_گرہ;
        واپس;
    }
    اگر (برگ_گرہ == خالی_اشاریہ) {
        *f_حالت = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    برگ_گرہ->fVal = static_cast<غیرعلامتی مختصر>(val);
    اگر (uset_گرہ->f_بایاں_بچہ == خالی_اشاریہ) {
        uset_گرہ->f_بایاں_بچہ = برگ_گرہ;
        برگ_گرہ->f_والد    = uset_گرہ;
    } ورنہ {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_گرہ = نیا RBBINode(RBBINode::opOr, *f_حالت);
        اگر (or_گرہ == خالی_اشاریہ) {
            *f_حالت = U_حافظہ_تخصیص_ERROR;
        }
        اگر (U_FAILURE(*f_حالت)) {
            حذف or_گرہ;
            حذف برگ_گرہ;
            واپس;
        }
        or_گرہ->f_بایاں_بچہ  = uset_گرہ->f_بایاں_بچہ;
        or_گرہ->f_دایاں_بچہ = برگ_گرہ;
        or_گرہ->f_بایاں_بچہ->f_والد  = or_گرہ;
        or_گرہ->f_دایاں_بچہ->f_والد = or_گرہ;
        uset_گرہ->f_بایاں_بچہ = or_گرہ;
        or_گرہ->f_والد = uset_گرہ;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_عدد_Char_Categories() ثابت {
    واپس f_گروہ_گنتی + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_Dict_Categories_آغاز() ثابت {
    واپس f_Dict_Categories_آغاز;
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
    برائے (rlRange = fRangeList; rlRange!=خالی_اشاریہ; rlRange=rlRange->f_اگلا) {
        اگر (rlRange->f_عدد == category) {
            retVal = rlRange->f_آغاز_Char;
            توڑو;
        }
    }
    واپس retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#اگر_تعریف_شدہ RBBI_DEBUG
خالی RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    صحیح                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    برائے (rlRange = fRangeList; rlRange!=خالی_اشاریہ; rlRange=rlRange->f_اگلا) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->f_آغاز_Char, rlRange->f_انتہا_Char);

        برائے (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_گرہ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   سیٹ_Name {u"anon"};
            RBBINode       *سیٹ_Ref = uset_گرہ->f_والد;
            اگر (سیٹ_Ref != خالی_اشاریہ) {
                RBBINode *varRef = سیٹ_Ref->f_والد;
                اگر (varRef != خالی_اشاریہ  &&  varRef->fType == RBBINode::varRef) {
                    سیٹ_Name = varRef->f_متن;
                }
            }
            RBBI_DEBUG_printUnicodeString(سیٹ_Name); RBBIDebugPrintf("  ");
        }
        RBBIDebugPrintf("\n");
    }
}
#اگر_ختم


//------------------------------------------------------------------------
//
//   printRangeGroups     A debugging function.
//                        dump out all of the range groups.
//
//------------------------------------------------------------------------
#اگر_تعریف_شدہ RBBI_DEBUG
خالی RBBISetBuilder::printRangeGroups() {
    صحیح                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    برائے (RangeDescriptor *rlRange = fRangeList; rlRange!=خالی_اشاریہ; rlRange=rlRange->f_اگلا) {
        اگر (rlRange->f_پہلا_اندر_گروہ) {
            صحیح گروہ_عدد = rlRange->f_عدد;
            RBBIDebugPrintf("%2i  ", گروہ_عدد);

            اگر (گروہ_عدد >= f_Dict_Categories_آغاز) { RBBIDebugPrintf(" <DICT> ");}

            برائے (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_گرہ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   سیٹ_Name = UNICODE_STRING("anon", 4);
                RBBINode       *سیٹ_Ref = uset_گرہ->f_والد;
                اگر (سیٹ_Ref != خالی_اشاریہ) {
                    RBBINode *varRef = سیٹ_Ref->f_والد;
                    اگر (varRef != خالی_اشاریہ  &&  varRef->fType == RBBINode::varRef) {
                        سیٹ_Name = varRef->f_متن;
                    }
                }
                RBBI_DEBUG_printUnicodeString(سیٹ_Name); RBBIDebugPrintf(" ");
            }

            i = 0;
            برائے (RangeDescriptor *tRange = rlRange; tRange != خالی_اشاریہ; tRange = tRange->f_اگلا) {
                اگر (tRange->f_عدد == rlRange->f_عدد) {
                    اگر (i++ % 5 == 0) {
                        RBBIDebugPrintf("\n    ");
                    }
                    RBBIDebugPrintf("  %05x-%05x", tRange->f_آغاز_Char, tRange->f_انتہا_Char);
                }
            }
            RBBIDebugPrintf("\n");
        }
    }
    RBBIDebugPrintf("\n");
}
#اگر_ختم


//------------------------------------------------------------------------
//
//   printSets          A debugging function.
//                      dump out all of the set definitions.
//
//------------------------------------------------------------------------
#اگر_تعریف_شدہ RBBI_DEBUG
خالی RBBISetBuilder::printSets() {
    صحیح                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    برائے (i=0; ; i++) {
        RBBINode        *uset_گرہ;
        RBBINode        *سیٹ_Ref;
        RBBINode        *varRef;
        UnicodeString    سیٹ_Name;

        uset_گرہ = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        اگر (uset_گرہ == خالی_اشاریہ) {
            توڑو;
        }

        RBBIDebugPrintf("%3d    ", i);
        سیٹ_Name = UNICODE_STRING("anonymous", 9);
        سیٹ_Ref = uset_گرہ->f_والد;
        اگر (سیٹ_Ref != خالی_اشاریہ) {
            varRef = سیٹ_Ref->f_والد;
            اگر (varRef != خالی_اشاریہ  &&  varRef->fType == RBBINode::varRef) {
                سیٹ_Name = varRef->f_متن;
            }
        }
        RBBI_DEBUG_printUnicodeString(سیٹ_Name);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_گرہ->f_متن);
        RBBIDebugPrintf("\n");
        اگر (uset_گرہ->f_بایاں_بچہ != خالی_اشاریہ) {
            RBBINode::print_درخت(uset_گرہ->f_بایاں_بچہ, سچ);
        }
    }
    RBBIDebugPrintf("\n");
}
#اگر_ختم



//-------------------------------------------------------------------------------------
//
//  RangeDescriptor copy constructor
//
//-------------------------------------------------------------------------------------

RangeDescriptor::RangeDescriptor(ثابت RangeDescriptor &other, UError_رمز &حالت_2) :
        f_آغاز_Char(other.f_آغاز_Char), f_انتہا_Char {other.f_انتہا_Char}, f_عدد {other.f_عدد},
        fIncludesDict{other.fIncludesDict}, f_پہلا_اندر_گروہ{other.f_پہلا_اندر_گروہ} {

    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    fIncludesSets = نیا UVector(حالت_2);
    اگر (یہ->fIncludesSets == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
    }
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }

    برائے (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        یہ->fIncludesSets->جوڑو_Element(other.fIncludesSets->elementAt(i), حالت_2);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    fIncludesSets = نیا UVector(حالت_2);
    اگر (fIncludesSets == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor Destructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::~RangeDescriptor() {
    حذف  fIncludesSets;
    fIncludesSets = خالی_اشاریہ;
}

//-------------------------------------------------------------------------------------
//
//  RangeDesriptor::split()
//
//-------------------------------------------------------------------------------------
خالی RangeDescriptor::بانٹو(UChar32 where, UError_رمز &حالت_2) {
    U_ASSERT(where>f_آغاز_Char && where<=f_انتہا_Char);
    RangeDescriptor *nr = نیا RangeDescriptor(*یہ, حالت_2);
    اگر(nr == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    اگر (U_FAILURE(حالت_2)) {
        حذف nr;
        واپس;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->f_آغاز_Char = where;
    یہ->f_انتہا_Char = where-1;
    nr->f_اگلا      = یہ->f_اگلا;
    یہ->f_اگلا    = nr;
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
    ساکن ثابت char16_t *dictionary = u"dictionary";
    برائے (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_گرہ = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *سیٹ_Ref = uset_گرہ->f_والد;
        اگر (سیٹ_Ref != خالی_اشاریہ) {
            RBBINode *varRef = سیٹ_Ref->f_والد;
            اگر (varRef && varRef->fType == RBBINode::varRef) {
                ثابت UnicodeString *سیٹ_Name = &varRef->f_متن;
                اگر (سیٹ_Name->compare(dictionary, -1) == 0) {
                    واپس سچ;
                }
            }
        }
    }
    واپس جھوٹ;
}

U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */
