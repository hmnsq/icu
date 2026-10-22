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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "uvector.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"

#అన్తర్భావ "rbbisetb.h"
#అన్తర్భావ "rbbinode.h"

U_NAMESPACE_BEGIN

స్థిరరాశి int32_t k_Max_Char_Categories_For8_బిట్లు_Trie = 255;
//------------------------------------------------------------------------
//
//   Constructor
//
//------------------------------------------------------------------------
RBBISetBuilder::RBBISetBuilder(RBBIRuleBuilder *rb)
{
    fRB             = rb;
    f_స్థితి         = rb->f_స్థితి;
    fRangeList      = శూన్యసూచిక;
    fMutableTrie    = శూన్యసూచిక;
    fTrie           = శూన్యసూచిక;
    f_Trie_కొలత       = 0;
    f_సమూహం_లెక్క     = 0;
}


//------------------------------------------------------------------------
//
//   Destructor
//
//------------------------------------------------------------------------
RBBISetBuilder::~RBBISetBuilder()
{
    RangeDescriptor   *తదుపరి_Range_Desc;

    // Walk through & delete the linked list of RangeDescriptors
    కొరకు (తదుపరి_Range_Desc = fRangeList; తదుపరి_Range_Desc!=శూన్యసూచిక;) {
        RangeDescriptor *r = తదుపరి_Range_Desc;
        తదుపరి_Range_Desc      = r->f_తదుపరి;
        తొలగించు r;
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
శూన్య RBBISetBuilder::buildRanges() {
    RBBINode        *uset_కణుపు;
    RangeDescriptor *rlRange;

    ఒకవేళ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = కొత్త RangeDescriptor(*f_స్థితి); // will check for status here
    ఒకవేళ (fRangeList == శూన్యసూచిక) {
        *f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    fRangeList->f_ప్రారంభం_Char    = 0;
    fRangeList->f_చివర_Char      = 0x10ffff;

    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    పూర్ణాంకం  ni;
    కొరకు (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_కణుపు = static_cast<RBBINode*>(ఇది->fRB->fUSetNodes->elementAt(ni));
        ఒకవేళ (uset_కణుపు==శూన్యసూచిక) {
            ఆపు;
        }

        Unicode_సమితి      *ఆదానం_సమితి             = uset_కణుపు->f_ఆదానం_సమితి;
        int32_t          ఆదానం_సమితి_Range_లెక్క   = ఆదానం_సమితి->get_Range_లెక్క();
        పూర్ణాంకం              ఆదానం_సమితి_Range_సూచీ   = 0;
                         rlRange              = fRangeList;

        కొరకు (;;) {
            ఒకవేళ (ఆదానం_సమితి_Range_సూచీ >= ఆదానం_సమితి_Range_లెక్క) {
                ఆపు;
            }
            UChar32      ఆదానం_సమితి_Range_Begin  = ఆదానం_సమితి->get_Range_ప్రారంభం(ఆదానం_సమితి_Range_సూచీ);
            UChar32      ఆదానం_సమితి_Range_చివర    = ఆదానం_సమితి->get_Range_చివర(ఆదానం_సమితి_Range_సూచీ);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            అయితే (rlRange->f_చివర_Char < ఆదానం_సమితి_Range_Begin) {
                rlRange = rlRange->f_తదుపరి;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            ఒకవేళ (rlRange->f_ప్రారంభం_Char < ఆదానం_సమితి_Range_Begin) {
                rlRange->విభజించు(ఆదానం_సమితి_Range_Begin, *f_స్థితి);
                ఒకవేళ (U_FAILURE(*f_స్థితి)) {
                    తిరిగి;
                }
                కొనసాగు;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            ఒకవేళ (rlRange->f_చివర_Char > ఆదానం_సమితి_Range_చివర) {
                rlRange->విభజించు(ఆదానం_సమితి_Range_చివర+1, *f_స్థితి);
                ఒకవేళ (U_FAILURE(*f_స్థితి)) {
                    తిరిగి;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            ఒకవేళ (rlRange->fIncludesSets->సూచీ_యొక్క(uset_కణుపు) == -1) {
                rlRange->fIncludesSets->కూడు_Element(uset_కణుపు, *f_స్థితి);
                ఒకవేళ (U_FAILURE(*f_స్థితి)) {
                    తిరిగి;
                }
            }

            // Advance over ranges that we are finished with.
            ఒకవేళ (ఆదానం_సమితి_Range_చివర == rlRange->f_చివర_Char) {
                ఆదానం_సమితి_Range_సూచీ++;
            }
            rlRange = rlRange->f_తదుపరి;
        }
    }

    ఒకవేళ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    RangeDescriptor *rl_శోధన_Range;
    int32_t dict_సమూహం_లెక్క = 0;

    కొరకు (rlRange = fRangeList; rlRange!=శూన్యసూచిక; rlRange=rlRange->f_తదుపరి) {
        కొరకు (rl_శోధన_Range=fRangeList; rl_శోధన_Range != rlRange; rl_శోధన_Range=rl_శోధన_Range->f_తదుపరి) {
            ఒకవేళ (rlRange->fIncludesSets->equals(*rl_శోధన_Range->fIncludesSets)) {
                rlRange->f_అంకె = rl_శోధన_Range->f_అంకె;
                rlRange->fIncludesDict = rl_శోధన_Range->fIncludesDict;
                ఆపు;
            }
        }
        ఒకవేళ (rlRange->f_అంకె == 0) {
            rlRange->f_మొదటి_లోపల_సమూహం = నిజం;
            ఒకవేళ (rlRange->isDictionaryRange()) {
                rlRange->f_అంకె = ++dict_సమూహం_లెక్క;
                rlRange->fIncludesDict = నిజం;
            } లేకపోతే {
                f_సమూహం_లెక్క++;
                rlRange->f_అంకె = f_సమూహం_లెక్క+2;
                కూడు_Val_గమ్యం_Sets(rlRange->fIncludesSets, rlRange->f_అంకె);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    f_Dict_Categories_ప్రారంభం = f_సమూహం_లెక్క + 3;
    కొరకు (rlRange = fRangeList; rlRange!=శూన్యసూచిక; rlRange=rlRange->f_తదుపరి) {
        ఒకవేళ (rlRange->fIncludesDict) {
            rlRange->f_అంకె += f_Dict_Categories_ప్రారంభం - 1;
            ఒకవేళ (rlRange->f_మొదటి_లోపల_సమూహం) {
                కూడు_Val_గమ్యం_Sets(rlRange->fIncludesSets, rlRange->f_అంకె);
            }
        }
    }
    f_సమూహం_లెక్క += dict_సమూహం_లెక్క;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    కొరకు (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_కణుపు = static_cast<RBBINode*>(ఇది->fRB->fUSetNodes->elementAt(ni));
        ఒకవేళ (uset_కణుపు==శూన్యసూచిక) {
            ఆపు;
        }
        Unicode_సమితి      *ఆదానం_సమితి = uset_కణుపు->f_ఆదానం_సమితి;
        ఒకవేళ (ఆదానం_సమితి->కలిగియుంది(eofString)) {
            కూడు_Val_గమ్యం_సమితి(uset_కణుపు, 1);
        }
    }


    ఒకవేళ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    ఒకవేళ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
శూన్య RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        f_స్థితి);

    కొరకు (RangeDescriptor *range = fRangeList; range!=శూన్యసూచిక && U_SUCCESS(*f_స్థితి); range=range->f_తదుపరి) {
        umutablecptrie_సమితి_Range(fMutableTrie,
                                range->f_ప్రారంభం_Char,     // Range start
                                range->f_చివర_Char,       // Range end (inclusive)
                                range->f_అంకె,           // value for range
                                f_స్థితి);
    }
}


శూన్య RBBISetBuilder::విలీనం_Categories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.రెండవ > categories.first);
    U_ASSERT((categories.first <  f_Dict_Categories_ప్రారంభం && categories.రెండవ <  f_Dict_Categories_ప్రారంభం) ||
             (categories.first >= f_Dict_Categories_ప్రారంభం && categories.రెండవ >= f_Dict_Categories_ప్రారంభం));

    కొరకు (RangeDescriptor *rd = fRangeList; rd != శూన్యసూచిక; rd = rd->f_తదుపరి) {
        int32_t range_అంకె = rd->f_అంకె;
        ఒకవేళ (range_అంకె == categories.రెండవ) {
            rd->f_అంకె = categories.first;
        } లేకపోతే ఒకవేళ (range_అంకె > categories.రెండవ) {
            rd->f_అంకె--;
        }
    }
    --f_సమూహం_లెక్క;
    ఒకవేళ (categories.రెండవ <= f_Dict_Categories_ప్రారంభం) {
        --f_Dict_Categories_ప్రారంభం;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::get_Trie_కొలత()  {
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి 0;
    }
    ఒకవేళ (fTrie == శూన్యసూచిక) {
        తార్కిక use8_బిట్లు = get_అంకె_Char_Categories() <= k_Max_Char_Categories_For8_బిట్లు_Trie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8_బిట్లు ? UCPTRIE_విలువ_బిట్లు_8 : UCPTRIE_విలువ_బిట్లు_16,
            f_స్థితి);
        UError_సంకేతం బఫరు_స్థితి = *f_స్థితి;
        f_Trie_కొలత = ucptrie_గమ్యం_ద్విమాన(fTrie, శూన్యసూచిక, 0, &బఫరు_స్థితి);
        ఒకవేళ (బఫరు_స్థితి != U_బఫరు_అధికప్రవాహం_ERROR && U_FAILURE(బఫరు_స్థితి)) {
            *f_స్థితి = బఫరు_స్థితి;
        }
    }
    తిరిగి f_Trie_కొలత;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
శూన్య RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_గమ్యం_ద్విమాన(fTrie,
                     where,                //బఫరు
                     f_Trie_కొలత,            //సామర్థ్యం
                     f_స్థితి);
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
శూన్య  RBBISetBuilder::కూడు_Val_గమ్యం_Sets(UVector *sets, uint32_t val) {
    int32_t       ix;

    కొరకు (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_కణుపు = static_cast<RBBINode*>(sets->elementAt(ix));
        కూడు_Val_గమ్యం_సమితి(uset_కణుపు, val);
    }
}

శూన్య  RBBISetBuilder::కూడు_Val_గమ్యం_సమితి(RBBINode *uset_కణుపు, uint32_t val) {
    RBBINode *ఆకు_కణుపు = కొత్త RBBINode(RBBINode::ఆకు_Char, *f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తొలగించు ఆకు_కణుపు;
        తిరిగి;
    }
    ఒకవేళ (ఆకు_కణుపు == శూన్యసూచిక) {
        *f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    ఆకు_కణుపు->fVal = static_cast<గుర్తులేని పొట్టి>(val);
    ఒకవేళ (uset_కణుపు->f_ఎడమ_బిడ్డ == శూన్యసూచిక) {
        uset_కణుపు->f_ఎడమ_బిడ్డ = ఆకు_కణుపు;
        ఆకు_కణుపు->f_మాతృ    = uset_కణుపు;
    } లేకపోతే {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_కణుపు = కొత్త RBBINode(RBBINode::opOr, *f_స్థితి);
        ఒకవేళ (or_కణుపు == శూన్యసూచిక) {
            *f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
        ఒకవేళ (U_FAILURE(*f_స్థితి)) {
            తొలగించు or_కణుపు;
            తొలగించు ఆకు_కణుపు;
            తిరిగి;
        }
        or_కణుపు->f_ఎడమ_బిడ్డ  = uset_కణుపు->f_ఎడమ_బిడ్డ;
        or_కణుపు->f_కుడి_బిడ్డ = ఆకు_కణుపు;
        or_కణుపు->f_ఎడమ_బిడ్డ->f_మాతృ  = or_కణుపు;
        or_కణుపు->f_కుడి_బిడ్డ->f_మాతృ = or_కణుపు;
        uset_కణుపు->f_ఎడమ_బిడ్డ = or_కణుపు;
        or_కణుపు->f_మాతృ = uset_కణుపు;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_అంకె_Char_Categories() స్థిరరాశి {
    తిరిగి f_సమూహం_లెక్క + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_Dict_Categories_ప్రారంభం() స్థిరరాశి {
    తిరిగి f_Dict_Categories_ప్రారంభం;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::get_మొదటి_Char(int32_t category) స్థిరరాశి {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    కొరకు (rlRange = fRangeList; rlRange!=శూన్యసూచిక; rlRange=rlRange->f_తదుపరి) {
        ఒకవేళ (rlRange->f_అంకె == category) {
            retVal = rlRange->f_ప్రారంభం_Char;
            ఆపు;
        }
    }
    తిరిగి retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#నిర్వచితమైతే RBBI_DEBUG
శూన్య RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    పూర్ణాంకం                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    కొరకు (rlRange = fRangeList; rlRange!=శూన్యసూచిక; rlRange=rlRange->f_తదుపరి) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->f_ప్రారంభం_Char, rlRange->f_చివర_Char);

        కొరకు (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_కణుపు    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   సమితి_Name {u"anon"};
            RBBINode       *సమితి_Ref = uset_కణుపు->f_మాతృ;
            ఒకవేళ (సమితి_Ref != శూన్యసూచిక) {
                RBBINode *varRef = సమితి_Ref->f_మాతృ;
                ఒకవేళ (varRef != శూన్యసూచిక  &&  varRef->fType == RBBINode::varRef) {
                    సమితి_Name = varRef->f_పాఠ్యం;
                }
            }
            RBBI_DEBUG_printUnicodeString(సమితి_Name); RBBIDebugPrintf("  ");
        }
        RBBIDebugPrintf("\n");
    }
}
#అయితే_ముగింపు


//------------------------------------------------------------------------
//
//   printRangeGroups     A debugging function.
//                        dump out all of the range groups.
//
//------------------------------------------------------------------------
#నిర్వచితమైతే RBBI_DEBUG
శూన్య RBBISetBuilder::printRangeGroups() {
    పూర్ణాంకం                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    కొరకు (RangeDescriptor *rlRange = fRangeList; rlRange!=శూన్యసూచిక; rlRange=rlRange->f_తదుపరి) {
        ఒకవేళ (rlRange->f_మొదటి_లోపల_సమూహం) {
            పూర్ణాంకం సమూహం_అంకె = rlRange->f_అంకె;
            RBBIDebugPrintf("%2i  ", సమూహం_అంకె);

            ఒకవేళ (సమూహం_అంకె >= f_Dict_Categories_ప్రారంభం) { RBBIDebugPrintf(" <DICT> ");}

            కొరకు (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_కణుపు    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   సమితి_Name = UNICODE_STRING("anon", 4);
                RBBINode       *సమితి_Ref = uset_కణుపు->f_మాతృ;
                ఒకవేళ (సమితి_Ref != శూన్యసూచిక) {
                    RBBINode *varRef = సమితి_Ref->f_మాతృ;
                    ఒకవేళ (varRef != శూన్యసూచిక  &&  varRef->fType == RBBINode::varRef) {
                        సమితి_Name = varRef->f_పాఠ్యం;
                    }
                }
                RBBI_DEBUG_printUnicodeString(సమితి_Name); RBBIDebugPrintf(" ");
            }

            i = 0;
            కొరకు (RangeDescriptor *tRange = rlRange; tRange != శూన్యసూచిక; tRange = tRange->f_తదుపరి) {
                ఒకవేళ (tRange->f_అంకె == rlRange->f_అంకె) {
                    ఒకవేళ (i++ % 5 == 0) {
                        RBBIDebugPrintf("\n    ");
                    }
                    RBBIDebugPrintf("  %05x-%05x", tRange->f_ప్రారంభం_Char, tRange->f_చివర_Char);
                }
            }
            RBBIDebugPrintf("\n");
        }
    }
    RBBIDebugPrintf("\n");
}
#అయితే_ముగింపు


//------------------------------------------------------------------------
//
//   printSets          A debugging function.
//                      dump out all of the set definitions.
//
//------------------------------------------------------------------------
#నిర్వచితమైతే RBBI_DEBUG
శూన్య RBBISetBuilder::printSets() {
    పూర్ణాంకం                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    కొరకు (i=0; ; i++) {
        RBBINode        *uset_కణుపు;
        RBBINode        *సమితి_Ref;
        RBBINode        *varRef;
        UnicodeString    సమితి_Name;

        uset_కణుపు = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        ఒకవేళ (uset_కణుపు == శూన్యసూచిక) {
            ఆపు;
        }

        RBBIDebugPrintf("%3d    ", i);
        సమితి_Name = UNICODE_STRING("anonymous", 9);
        సమితి_Ref = uset_కణుపు->f_మాతృ;
        ఒకవేళ (సమితి_Ref != శూన్యసూచిక) {
            varRef = సమితి_Ref->f_మాతృ;
            ఒకవేళ (varRef != శూన్యసూచిక  &&  varRef->fType == RBBINode::varRef) {
                సమితి_Name = varRef->f_పాఠ్యం;
            }
        }
        RBBI_DEBUG_printUnicodeString(సమితి_Name);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_కణుపు->f_పాఠ్యం);
        RBBIDebugPrintf("\n");
        ఒకవేళ (uset_కణుపు->f_ఎడమ_బిడ్డ != శూన్యసూచిక) {
            RBBINode::print_వృక్షం(uset_కణుపు->f_ఎడమ_బిడ్డ, నిజం);
        }
    }
    RBBIDebugPrintf("\n");
}
#అయితే_ముగింపు



//-------------------------------------------------------------------------------------
//
//  RangeDescriptor copy constructor
//
//-------------------------------------------------------------------------------------

RangeDescriptor::RangeDescriptor(స్థిరరాశి RangeDescriptor &other, UError_సంకేతం &స్థితి_2) :
        f_ప్రారంభం_Char(other.f_ప్రారంభం_Char), f_చివర_Char {other.f_చివర_Char}, f_అంకె {other.f_అంకె},
        fIncludesDict{other.fIncludesDict}, f_మొదటి_లోపల_సమూహం{other.f_మొదటి_లోపల_సమూహం} {

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    fIncludesSets = కొత్త UVector(స్థితి_2);
    ఒకవేళ (ఇది->fIncludesSets == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    కొరకు (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        ఇది->fIncludesSets->కూడు_Element(other.fIncludesSets->elementAt(i), స్థితి_2);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    fIncludesSets = కొత్త UVector(స్థితి_2);
    ఒకవేళ (fIncludesSets == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor Destructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::~RangeDescriptor() {
    తొలగించు  fIncludesSets;
    fIncludesSets = శూన్యసూచిక;
}

//-------------------------------------------------------------------------------------
//
//  RangeDesriptor::split()
//
//-------------------------------------------------------------------------------------
శూన్య RangeDescriptor::విభజించు(UChar32 where, UError_సంకేతం &స్థితి_2) {
    U_ASSERT(where>f_ప్రారంభం_Char && where<=f_చివర_Char);
    RangeDescriptor *nr = కొత్త RangeDescriptor(*ఇది, స్థితి_2);
    ఒకవేళ(nr == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తొలగించు nr;
        తిరిగి;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->f_ప్రారంభం_Char = where;
    ఇది->f_చివర_Char = where-1;
    nr->f_తదుపరి      = ఇది->f_తదుపరి;
    ఇది->f_తదుపరి    = nr;
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
తార్కిక RangeDescriptor::isDictionaryRange() {
    స్థిర స్థిరరాశి char16_t *dictionary = u"dictionary";
    కొరకు (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_కణుపు = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *సమితి_Ref = uset_కణుపు->f_మాతృ;
        ఒకవేళ (సమితి_Ref != శూన్యసూచిక) {
            RBBINode *varRef = సమితి_Ref->f_మాతృ;
            ఒకవేళ (varRef && varRef->fType == RBBINode::varRef) {
                స్థిరరాశి UnicodeString *సమితి_Name = &varRef->f_పాఠ్యం;
                ఒకవేళ (సమితి_Name->compare(dictionary, -1) == 0) {
                    తిరిగి నిజం;
                }
            }
        }
    }
    తిరిగి అబద్ధం;
}

U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */
