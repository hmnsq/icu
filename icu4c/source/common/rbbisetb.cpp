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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "rbbisetb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbinode.h"

U_NAMESPACE_BEGIN

ਅਚਲ int32_t k_Max_Char_Categories_For8_ਬਿੱਟ_Trie = 255;
//------------------------------------------------------------------------
//
//   Constructor
//
//------------------------------------------------------------------------
RBBISetBuilder::RBBISetBuilder(RBBIRuleBuilder *rb)
{
    fRB             = rb;
    f_ਸਥਿਤੀ         = rb->f_ਸਥਿਤੀ;
    fRangeList      = ਖਾਲੀ_ਸੂਚਕ;
    fMutableTrie    = ਖਾਲੀ_ਸੂਚਕ;
    fTrie           = ਖਾਲੀ_ਸੂਚਕ;
    f_Trie_ਮਾਪ       = 0;
    f_ਸਮੂਹ_ਗਿਣਤੀ     = 0;
}


//------------------------------------------------------------------------
//
//   Destructor
//
//------------------------------------------------------------------------
RBBISetBuilder::~RBBISetBuilder()
{
    RangeDescriptor   *ਅਗਲਾ_Range_Desc;

    // Walk through & delete the linked list of RangeDescriptors
    ਲਈ (ਅਗਲਾ_Range_Desc = fRangeList; ਅਗਲਾ_Range_Desc!=ਖਾਲੀ_ਸੂਚਕ;) {
        RangeDescriptor *r = ਅਗਲਾ_Range_Desc;
        ਅਗਲਾ_Range_Desc      = r->f_ਅਗਲਾ;
        ਮਿਟਾਓ r;
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
ਖਾਲੀ RBBISetBuilder::buildRanges() {
    RBBINode        *uset_ਗੰਢ;
    RangeDescriptor *rlRange;

    ਜੇ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = ਨਵਾਂ RangeDescriptor(*f_ਸਥਿਤੀ); // will check for status here
    ਜੇ (fRangeList == ਖਾਲੀ_ਸੂਚਕ) {
        *f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    fRangeList->f_ਸ਼ੁਰੂ_Char    = 0;
    fRangeList->f_ਸਿਰਾ_Char      = 0x10ffff;

    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    ਪੂਰਨ  ni;
    ਲਈ (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ਗੰਢ = static_cast<RBBINode*>(ਇਹ->fRB->fUSetNodes->elementAt(ni));
        ਜੇ (uset_ਗੰਢ==ਖਾਲੀ_ਸੂਚਕ) {
            ਰੋਕੋ;
        }

        Unicode_ਸੈੱਟ      *ਨਿਵੇਸ਼_ਸੈੱਟ             = uset_ਗੰਢ->f_ਨਿਵੇਸ਼_ਸੈੱਟ;
        int32_t          ਨਿਵੇਸ਼_ਸੈੱਟ_Range_ਗਿਣਤੀ   = ਨਿਵੇਸ਼_ਸੈੱਟ->get_Range_ਗਿਣਤੀ();
        ਪੂਰਨ              ਨਿਵੇਸ਼_ਸੈੱਟ_Range_ਸੂਚਕਾਂਕ   = 0;
                         rlRange              = fRangeList;

        ਲਈ (;;) {
            ਜੇ (ਨਿਵੇਸ਼_ਸੈੱਟ_Range_ਸੂਚਕਾਂਕ >= ਨਿਵੇਸ਼_ਸੈੱਟ_Range_ਗਿਣਤੀ) {
                ਰੋਕੋ;
            }
            UChar32      ਨਿਵੇਸ਼_ਸੈੱਟ_Range_Begin  = ਨਿਵੇਸ਼_ਸੈੱਟ->get_Range_ਸ਼ੁਰੂ(ਨਿਵੇਸ਼_ਸੈੱਟ_Range_ਸੂਚਕਾਂਕ);
            UChar32      ਨਿਵੇਸ਼_ਸੈੱਟ_Range_ਸਿਰਾ    = ਨਿਵੇਸ਼_ਸੈੱਟ->get_Range_ਸਿਰਾ(ਨਿਵੇਸ਼_ਸੈੱਟ_Range_ਸੂਚਕਾਂਕ);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            ਜਦੋਂ (rlRange->f_ਸਿਰਾ_Char < ਨਿਵੇਸ਼_ਸੈੱਟ_Range_Begin) {
                rlRange = rlRange->f_ਅਗਲਾ;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            ਜੇ (rlRange->f_ਸ਼ੁਰੂ_Char < ਨਿਵੇਸ਼_ਸੈੱਟ_Range_Begin) {
                rlRange->ਵੰਡੋ(ਨਿਵੇਸ਼_ਸੈੱਟ_Range_Begin, *f_ਸਥਿਤੀ);
                ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
                    ਵਾਪਸ;
                }
                ਜਾਰੀ;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            ਜੇ (rlRange->f_ਸਿਰਾ_Char > ਨਿਵੇਸ਼_ਸੈੱਟ_Range_ਸਿਰਾ) {
                rlRange->ਵੰਡੋ(ਨਿਵੇਸ਼_ਸੈੱਟ_Range_ਸਿਰਾ+1, *f_ਸਥਿਤੀ);
                ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
                    ਵਾਪਸ;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            ਜੇ (rlRange->fIncludesSets->ਸੂਚਕਾਂਕ_ਦਾ(uset_ਗੰਢ) == -1) {
                rlRange->fIncludesSets->ਜੋੜੋ_Element(uset_ਗੰਢ, *f_ਸਥਿਤੀ);
                ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
                    ਵਾਪਸ;
                }
            }

            // Advance over ranges that we are finished with.
            ਜੇ (ਨਿਵੇਸ਼_ਸੈੱਟ_Range_ਸਿਰਾ == rlRange->f_ਸਿਰਾ_Char) {
                ਨਿਵੇਸ਼_ਸੈੱਟ_Range_ਸੂਚਕਾਂਕ++;
            }
            rlRange = rlRange->f_ਅਗਲਾ;
        }
    }

    ਜੇ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    RangeDescriptor *rl_ਖੋਜ_Range;
    int32_t dict_ਸਮੂਹ_ਗਿਣਤੀ = 0;

    ਲਈ (rlRange = fRangeList; rlRange!=ਖਾਲੀ_ਸੂਚਕ; rlRange=rlRange->f_ਅਗਲਾ) {
        ਲਈ (rl_ਖੋਜ_Range=fRangeList; rl_ਖੋਜ_Range != rlRange; rl_ਖੋਜ_Range=rl_ਖੋਜ_Range->f_ਅਗਲਾ) {
            ਜੇ (rlRange->fIncludesSets->equals(*rl_ਖੋਜ_Range->fIncludesSets)) {
                rlRange->f_ਅੰਕ = rl_ਖੋਜ_Range->f_ਅੰਕ;
                rlRange->fIncludesDict = rl_ਖੋਜ_Range->fIncludesDict;
                ਰੋਕੋ;
            }
        }
        ਜੇ (rlRange->f_ਅੰਕ == 0) {
            rlRange->f_ਪਹਿਲਾ_ਵਿੱਚ_ਸਮੂਹ = ਸੱਚ;
            ਜੇ (rlRange->isDictionaryRange()) {
                rlRange->f_ਅੰਕ = ++dict_ਸਮੂਹ_ਗਿਣਤੀ;
                rlRange->fIncludesDict = ਸੱਚ;
            } ਨਹੀਂਤਾਂ {
                f_ਸਮੂਹ_ਗਿਣਤੀ++;
                rlRange->f_ਅੰਕ = f_ਸਮੂਹ_ਗਿਣਤੀ+2;
                ਜੋੜੋ_Val_ਮੰਜ਼ਿਲ_Sets(rlRange->fIncludesSets, rlRange->f_ਅੰਕ);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    f_Dict_Categories_ਸ਼ੁਰੂ = f_ਸਮੂਹ_ਗਿਣਤੀ + 3;
    ਲਈ (rlRange = fRangeList; rlRange!=ਖਾਲੀ_ਸੂਚਕ; rlRange=rlRange->f_ਅਗਲਾ) {
        ਜੇ (rlRange->fIncludesDict) {
            rlRange->f_ਅੰਕ += f_Dict_Categories_ਸ਼ੁਰੂ - 1;
            ਜੇ (rlRange->f_ਪਹਿਲਾ_ਵਿੱਚ_ਸਮੂਹ) {
                ਜੋੜੋ_Val_ਮੰਜ਼ਿਲ_Sets(rlRange->fIncludesSets, rlRange->f_ਅੰਕ);
            }
        }
    }
    f_ਸਮੂਹ_ਗਿਣਤੀ += dict_ਸਮੂਹ_ਗਿਣਤੀ;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    ਲਈ (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ਗੰਢ = static_cast<RBBINode*>(ਇਹ->fRB->fUSetNodes->elementAt(ni));
        ਜੇ (uset_ਗੰਢ==ਖਾਲੀ_ਸੂਚਕ) {
            ਰੋਕੋ;
        }
        Unicode_ਸੈੱਟ      *ਨਿਵੇਸ਼_ਸੈੱਟ = uset_ਗੰਢ->f_ਨਿਵੇਸ਼_ਸੈੱਟ;
        ਜੇ (ਨਿਵੇਸ਼_ਸੈੱਟ->ਸ਼ਾਮਲ_ਹੈ(eofString)) {
            ਜੋੜੋ_Val_ਮੰਜ਼ਿਲ_ਸੈੱਟ(uset_ਗੰਢ, 1);
        }
    }


    ਜੇ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    ਜੇ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
ਖਾਲੀ RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        f_ਸਥਿਤੀ);

    ਲਈ (RangeDescriptor *range = fRangeList; range!=ਖਾਲੀ_ਸੂਚਕ && U_SUCCESS(*f_ਸਥਿਤੀ); range=range->f_ਅਗਲਾ) {
        umutablecptrie_ਸੈੱਟ_Range(fMutableTrie,
                                range->f_ਸ਼ੁਰੂ_Char,     // Range start
                                range->f_ਸਿਰਾ_Char,       // Range end (inclusive)
                                range->f_ਅੰਕ,           // value for range
                                f_ਸਥਿਤੀ);
    }
}


ਖਾਲੀ RBBISetBuilder::ਮਿਲਾਓ_Categories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.ਦੂਜਾ > categories.first);
    U_ASSERT((categories.first <  f_Dict_Categories_ਸ਼ੁਰੂ && categories.ਦੂਜਾ <  f_Dict_Categories_ਸ਼ੁਰੂ) ||
             (categories.first >= f_Dict_Categories_ਸ਼ੁਰੂ && categories.ਦੂਜਾ >= f_Dict_Categories_ਸ਼ੁਰੂ));

    ਲਈ (RangeDescriptor *rd = fRangeList; rd != ਖਾਲੀ_ਸੂਚਕ; rd = rd->f_ਅਗਲਾ) {
        int32_t range_ਅੰਕ = rd->f_ਅੰਕ;
        ਜੇ (range_ਅੰਕ == categories.ਦੂਜਾ) {
            rd->f_ਅੰਕ = categories.first;
        } ਨਹੀਂਤਾਂ ਜੇ (range_ਅੰਕ > categories.ਦੂਜਾ) {
            rd->f_ਅੰਕ--;
        }
    }
    --f_ਸਮੂਹ_ਗਿਣਤੀ;
    ਜੇ (categories.ਦੂਜਾ <= f_Dict_Categories_ਸ਼ੁਰੂ) {
        --f_Dict_Categories_ਸ਼ੁਰੂ;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::get_Trie_ਮਾਪ()  {
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ (fTrie == ਖਾਲੀ_ਸੂਚਕ) {
        ਤਾਰਕਿਕ use8_ਬਿੱਟ = get_ਅੰਕ_Char_Categories() <= k_Max_Char_Categories_For8_ਬਿੱਟ_Trie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8_ਬਿੱਟ ? UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8 : UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16,
            f_ਸਥਿਤੀ);
        UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = *f_ਸਥਿਤੀ;
        f_Trie_ਮਾਪ = ucptrie_ਮੰਜ਼ਿਲ_ਦੁਆਧਾਰੀ(fTrie, ਖਾਲੀ_ਸੂਚਕ, 0, &ਬਫਰ_ਸਥਿਤੀ);
        ਜੇ (ਬਫਰ_ਸਥਿਤੀ != U_ਬਫਰ_ਉਛਾਲ_ERROR && U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)) {
            *f_ਸਥਿਤੀ = ਬਫਰ_ਸਥਿਤੀ;
        }
    }
    ਵਾਪਸ f_Trie_ਮਾਪ;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
ਖਾਲੀ RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_ਮੰਜ਼ਿਲ_ਦੁਆਧਾਰੀ(fTrie,
                     where,                //ਬਫਰ
                     f_Trie_ਮਾਪ,            //ਸਮਰੱਥਾ
                     f_ਸਥਿਤੀ);
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
ਖਾਲੀ  RBBISetBuilder::ਜੋੜੋ_Val_ਮੰਜ਼ਿਲ_Sets(UVector *sets, uint32_t val) {
    int32_t       ix;

    ਲਈ (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_ਗੰਢ = static_cast<RBBINode*>(sets->elementAt(ix));
        ਜੋੜੋ_Val_ਮੰਜ਼ਿਲ_ਸੈੱਟ(uset_ਗੰਢ, val);
    }
}

ਖਾਲੀ  RBBISetBuilder::ਜੋੜੋ_Val_ਮੰਜ਼ਿਲ_ਸੈੱਟ(RBBINode *uset_ਗੰਢ, uint32_t val) {
    RBBINode *ਪੱਤੀ_ਗੰਢ = ਨਵਾਂ RBBINode(RBBINode::ਪੱਤੀ_Char, *f_ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
        ਮਿਟਾਓ ਪੱਤੀ_ਗੰਢ;
        ਵਾਪਸ;
    }
    ਜੇ (ਪੱਤੀ_ਗੰਢ == ਖਾਲੀ_ਸੂਚਕ) {
        *f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ਪੱਤੀ_ਗੰਢ->fVal = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ>(val);
    ਜੇ (uset_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ == ਖਾਲੀ_ਸੂਚਕ) {
        uset_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ = ਪੱਤੀ_ਗੰਢ;
        ਪੱਤੀ_ਗੰਢ->f_ਮਾਪਾ    = uset_ਗੰਢ;
    } ਨਹੀਂਤਾਂ {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_ਗੰਢ = ਨਵਾਂ RBBINode(RBBINode::opOr, *f_ਸਥਿਤੀ);
        ਜੇ (or_ਗੰਢ == ਖਾਲੀ_ਸੂਚਕ) {
            *f_ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
        ਜੇ (U_FAILURE(*f_ਸਥਿਤੀ)) {
            ਮਿਟਾਓ or_ਗੰਢ;
            ਮਿਟਾਓ ਪੱਤੀ_ਗੰਢ;
            ਵਾਪਸ;
        }
        or_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ  = uset_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ;
        or_ਗੰਢ->f_ਸੱਜਾ_ਬੱਚਾ = ਪੱਤੀ_ਗੰਢ;
        or_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ->f_ਮਾਪਾ  = or_ਗੰਢ;
        or_ਗੰਢ->f_ਸੱਜਾ_ਬੱਚਾ->f_ਮਾਪਾ = or_ਗੰਢ;
        uset_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ = or_ਗੰਢ;
        or_ਗੰਢ->f_ਮਾਪਾ = uset_ਗੰਢ;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_ਅੰਕ_Char_Categories() ਅਚਲ {
    ਵਾਪਸ f_ਸਮੂਹ_ਗਿਣਤੀ + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_Dict_Categories_ਸ਼ੁਰੂ() ਅਚਲ {
    ਵਾਪਸ f_Dict_Categories_ਸ਼ੁਰੂ;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::get_ਪਹਿਲਾ_Char(int32_t category) ਅਚਲ {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    ਲਈ (rlRange = fRangeList; rlRange!=ਖਾਲੀ_ਸੂਚਕ; rlRange=rlRange->f_ਅਗਲਾ) {
        ਜੇ (rlRange->f_ਅੰਕ == category) {
            retVal = rlRange->f_ਸ਼ੁਰੂ_Char;
            ਰੋਕੋ;
        }
    }
    ਵਾਪਸ retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
ਖਾਲੀ RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    ਪੂਰਨ                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    ਲਈ (rlRange = fRangeList; rlRange!=ਖਾਲੀ_ਸੂਚਕ; rlRange=rlRange->f_ਅਗਲਾ) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->f_ਸ਼ੁਰੂ_Char, rlRange->f_ਸਿਰਾ_Char);

        ਲਈ (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_ਗੰਢ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   ਸੈੱਟ_Name {u"anon"};
            RBBINode       *ਸੈੱਟ_Ref = uset_ਗੰਢ->f_ਮਾਪਾ;
            ਜੇ (ਸੈੱਟ_Ref != ਖਾਲੀ_ਸੂਚਕ) {
                RBBINode *varRef = ਸੈੱਟ_Ref->f_ਮਾਪਾ;
                ਜੇ (varRef != ਖਾਲੀ_ਸੂਚਕ  &&  varRef->fType == RBBINode::varRef) {
                    ਸੈੱਟ_Name = varRef->f_ਪਾਠ;
                }
            }
            RBBI_DEBUG_printUnicodeString(ਸੈੱਟ_Name); RBBIDebugPrintf("  ");
        }
        RBBIDebugPrintf("\n");
    }
}
#ਜੇ_ਅੰਤ


//------------------------------------------------------------------------
//
//   printRangeGroups     A debugging function.
//                        dump out all of the range groups.
//
//------------------------------------------------------------------------
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
ਖਾਲੀ RBBISetBuilder::printRangeGroups() {
    ਪੂਰਨ                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    ਲਈ (RangeDescriptor *rlRange = fRangeList; rlRange!=ਖਾਲੀ_ਸੂਚਕ; rlRange=rlRange->f_ਅਗਲਾ) {
        ਜੇ (rlRange->f_ਪਹਿਲਾ_ਵਿੱਚ_ਸਮੂਹ) {
            ਪੂਰਨ ਸਮੂਹ_ਅੰਕ = rlRange->f_ਅੰਕ;
            RBBIDebugPrintf("%2i  ", ਸਮੂਹ_ਅੰਕ);

            ਜੇ (ਸਮੂਹ_ਅੰਕ >= f_Dict_Categories_ਸ਼ੁਰੂ) { RBBIDebugPrintf(" <DICT> ");}

            ਲਈ (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_ਗੰਢ    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   ਸੈੱਟ_Name = UNICODE_STRING("anon", 4);
                RBBINode       *ਸੈੱਟ_Ref = uset_ਗੰਢ->f_ਮਾਪਾ;
                ਜੇ (ਸੈੱਟ_Ref != ਖਾਲੀ_ਸੂਚਕ) {
                    RBBINode *varRef = ਸੈੱਟ_Ref->f_ਮਾਪਾ;
                    ਜੇ (varRef != ਖਾਲੀ_ਸੂਚਕ  &&  varRef->fType == RBBINode::varRef) {
                        ਸੈੱਟ_Name = varRef->f_ਪਾਠ;
                    }
                }
                RBBI_DEBUG_printUnicodeString(ਸੈੱਟ_Name); RBBIDebugPrintf(" ");
            }

            i = 0;
            ਲਈ (RangeDescriptor *tRange = rlRange; tRange != ਖਾਲੀ_ਸੂਚਕ; tRange = tRange->f_ਅਗਲਾ) {
                ਜੇ (tRange->f_ਅੰਕ == rlRange->f_ਅੰਕ) {
                    ਜੇ (i++ % 5 == 0) {
                        RBBIDebugPrintf("\n    ");
                    }
                    RBBIDebugPrintf("  %05x-%05x", tRange->f_ਸ਼ੁਰੂ_Char, tRange->f_ਸਿਰਾ_Char);
                }
            }
            RBBIDebugPrintf("\n");
        }
    }
    RBBIDebugPrintf("\n");
}
#ਜੇ_ਅੰਤ


//------------------------------------------------------------------------
//
//   printSets          A debugging function.
//                      dump out all of the set definitions.
//
//------------------------------------------------------------------------
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
ਖਾਲੀ RBBISetBuilder::printSets() {
    ਪੂਰਨ                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    ਲਈ (i=0; ; i++) {
        RBBINode        *uset_ਗੰਢ;
        RBBINode        *ਸੈੱਟ_Ref;
        RBBINode        *varRef;
        UnicodeString    ਸੈੱਟ_Name;

        uset_ਗੰਢ = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        ਜੇ (uset_ਗੰਢ == ਖਾਲੀ_ਸੂਚਕ) {
            ਰੋਕੋ;
        }

        RBBIDebugPrintf("%3d    ", i);
        ਸੈੱਟ_Name = UNICODE_STRING("anonymous", 9);
        ਸੈੱਟ_Ref = uset_ਗੰਢ->f_ਮਾਪਾ;
        ਜੇ (ਸੈੱਟ_Ref != ਖਾਲੀ_ਸੂਚਕ) {
            varRef = ਸੈੱਟ_Ref->f_ਮਾਪਾ;
            ਜੇ (varRef != ਖਾਲੀ_ਸੂਚਕ  &&  varRef->fType == RBBINode::varRef) {
                ਸੈੱਟ_Name = varRef->f_ਪਾਠ;
            }
        }
        RBBI_DEBUG_printUnicodeString(ਸੈੱਟ_Name);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_ਗੰਢ->f_ਪਾਠ);
        RBBIDebugPrintf("\n");
        ਜੇ (uset_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ != ਖਾਲੀ_ਸੂਚਕ) {
            RBBINode::print_ਰੁੱਖ(uset_ਗੰਢ->f_ਖੱਬਾ_ਬੱਚਾ, ਸੱਚ);
        }
    }
    RBBIDebugPrintf("\n");
}
#ਜੇ_ਅੰਤ



//-------------------------------------------------------------------------------------
//
//  RangeDescriptor copy constructor
//
//-------------------------------------------------------------------------------------

RangeDescriptor::RangeDescriptor(ਅਚਲ RangeDescriptor &other, UError_ਕੋਡ &ਸਥਿਤੀ) :
        f_ਸ਼ੁਰੂ_Char(other.f_ਸ਼ੁਰੂ_Char), f_ਸਿਰਾ_Char {other.f_ਸਿਰਾ_Char}, f_ਅੰਕ {other.f_ਅੰਕ},
        fIncludesDict{other.fIncludesDict}, f_ਪਹਿਲਾ_ਵਿੱਚ_ਸਮੂਹ{other.f_ਪਹਿਲਾ_ਵਿੱਚ_ਸਮੂਹ} {

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    fIncludesSets = ਨਵਾਂ UVector(ਸਥਿਤੀ);
    ਜੇ (ਇਹ->fIncludesSets == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    ਲਈ (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        ਇਹ->fIncludesSets->ਜੋੜੋ_Element(other.fIncludesSets->elementAt(i), ਸਥਿਤੀ);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    fIncludesSets = ਨਵਾਂ UVector(ਸਥਿਤੀ);
    ਜੇ (fIncludesSets == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor Destructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::~RangeDescriptor() {
    ਮਿਟਾਓ  fIncludesSets;
    fIncludesSets = ਖਾਲੀ_ਸੂਚਕ;
}

//-------------------------------------------------------------------------------------
//
//  RangeDesriptor::split()
//
//-------------------------------------------------------------------------------------
ਖਾਲੀ RangeDescriptor::ਵੰਡੋ(UChar32 where, UError_ਕੋਡ &ਸਥਿਤੀ) {
    U_ASSERT(where>f_ਸ਼ੁਰੂ_Char && where<=f_ਸਿਰਾ_Char);
    RangeDescriptor *nr = ਨਵਾਂ RangeDescriptor(*ਇਹ, ਸਥਿਤੀ);
    ਜੇ(nr == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਮਿਟਾਓ nr;
        ਵਾਪਸ;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->f_ਸ਼ੁਰੂ_Char = where;
    ਇਹ->f_ਸਿਰਾ_Char = where-1;
    nr->f_ਅਗਲਾ      = ਇਹ->f_ਅਗਲਾ;
    ਇਹ->f_ਅਗਲਾ    = nr;
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
ਤਾਰਕਿਕ RangeDescriptor::isDictionaryRange() {
    ਸਥਿਰ ਅਚਲ char16_t *dictionary = u"dictionary";
    ਲਈ (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_ਗੰਢ = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *ਸੈੱਟ_Ref = uset_ਗੰਢ->f_ਮਾਪਾ;
        ਜੇ (ਸੈੱਟ_Ref != ਖਾਲੀ_ਸੂਚਕ) {
            RBBINode *varRef = ਸੈੱਟ_Ref->f_ਮਾਪਾ;
            ਜੇ (varRef && varRef->fType == RBBINode::varRef) {
                ਅਚਲ UnicodeString *ਸੈੱਟ_Name = &varRef->f_ਪਾਠ;
                ਜੇ (ਸੈੱਟ_Name->compare(dictionary, -1) == 0) {
                    ਵਾਪਸ ਸੱਚ;
                }
            }
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */
