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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"

#अन्तर्भाव "rbbisetb.h"
#अन्तर्भाव "rbbinode.h"

U_NAMESPACE_BEGIN

ध्रुव int32_t k_Max_Char_Categories_For8_द्वयङ्काः_Trie = 255;
//------------------------------------------------------------------------
//
//   Constructor
//
//------------------------------------------------------------------------
RBBISetBuilder::RBBISetBuilder(RBBIRuleBuilder *rb)
{
    fRB             = rb;
    f_स्थितिः         = rb->f_स्थितिः;
    fRangeList      = शून्यसूचकः;
    fMutableTrie    = शून्यसूचकः;
    fTrie           = शून्यसूचकः;
    f_Trie_आकार       = 0;
    f_गणः_गणनम्     = 0;
}


//------------------------------------------------------------------------
//
//   Destructor
//
//------------------------------------------------------------------------
RBBISetBuilder::~RBBISetBuilder()
{
    RangeDescriptor   *अग्र_Range_Desc;

    // Walk through & delete the linked list of RangeDescriptors
    प्रति (अग्र_Range_Desc = fRangeList; अग्र_Range_Desc!=शून्यसूचकः;) {
        RangeDescriptor *r = अग्र_Range_Desc;
        अग्र_Range_Desc      = r->f_अग्र;
        विलोपय r;
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
शून्य RBBISetBuilder::buildRanges() {
    RBBINode        *uset_ग्रन्थिः;
    RangeDescriptor *rlRange;

    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = नूतन RangeDescriptor(*f_स्थितिः); // will check for status here
    यदि (fRangeList == शून्यसूचकः) {
        *f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    fRangeList->fStartChar    = 0;
    fRangeList->f_अन्तः_Char      = 0x10ffff;

    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    पूर्णाङ्क  ni;
    प्रति (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ग्रन्थिः = static_cast<RBBINode*>(एतत्->fRB->fUSetNodes->elementAt(ni));
        यदि (uset_ग्रन्थिः==शून्यसूचकः) {
            भङ्ग;
        }

        Unicode_समुच्चयः      *निवेशः_समुच्चयः             = uset_ग्रन्थिः->f_निवेशः_समुच्चयः;
        int32_t          निवेशः_समुच्चयः_Range_गणनम्   = निवेशः_समुच्चयः->get_Range_गणनम्();
        पूर्णाङ्क              निवेशः_समुच्चयः_Range_सूचकांक   = 0;
                         rlRange              = fRangeList;

        प्रति (;;) {
            यदि (निवेशः_समुच्चयः_Range_सूचकांक >= निवेशः_समुच्चयः_Range_गणनम्) {
                भङ्ग;
            }
            UChar32      निवेशः_समुच्चयः_Range_Begin  = निवेशः_समुच्चयः->getRangeStart(निवेशः_समुच्चयः_Range_सूचकांक);
            UChar32      निवेशः_समुच्चयः_Range_अन्तः    = निवेशः_समुच्चयः->get_Range_अन्तः(निवेशः_समुच्चयः_Range_सूचकांक);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            यावत् (rlRange->f_अन्तः_Char < निवेशः_समुच्चयः_Range_Begin) {
                rlRange = rlRange->f_अग्र;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            यदि (rlRange->fStartChar < निवेशः_समुच्चयः_Range_Begin) {
                rlRange->विभज(निवेशः_समुच्चयः_Range_Begin, *f_स्थितिः);
                यदि (U_FAILURE(*f_स्थितिः)) {
                    प्रत्यागम;
                }
                अनुवर्त;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            यदि (rlRange->f_अन्तः_Char > निवेशः_समुच्चयः_Range_अन्तः) {
                rlRange->विभज(निवेशः_समुच्चयः_Range_अन्तः+1, *f_स्थितिः);
                यदि (U_FAILURE(*f_स्थितिः)) {
                    प्रत्यागम;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            यदि (rlRange->fIncludesSets->सूचकांक_सम्बन्धि(uset_ग्रन्थिः) == -1) {
                rlRange->fIncludesSets->सङ्कलय_Element(uset_ग्रन्थिः, *f_स्थितिः);
                यदि (U_FAILURE(*f_स्थितिः)) {
                    प्रत्यागम;
                }
            }

            // Advance over ranges that we are finished with.
            यदि (निवेशः_समुच्चयः_Range_अन्तः == rlRange->f_अन्तः_Char) {
                निवेशः_समुच्चयः_Range_सूचकांक++;
            }
            rlRange = rlRange->f_अग्र;
        }
    }

    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "range")) { printRanges();}

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
    RangeDescriptor *rl_अन्वेषणम्_Range;
    int32_t dict_गणः_गणनम् = 0;

    प्रति (rlRange = fRangeList; rlRange!=शून्यसूचकः; rlRange=rlRange->f_अग्र) {
        प्रति (rl_अन्वेषणम्_Range=fRangeList; rl_अन्वेषणम्_Range != rlRange; rl_अन्वेषणम्_Range=rl_अन्वेषणम्_Range->f_अग्र) {
            यदि (rlRange->fIncludesSets->equals(*rl_अन्वेषणम्_Range->fIncludesSets)) {
                rlRange->f_अङ्कः = rl_अन्वेषणम्_Range->f_अङ्कः;
                rlRange->fIncludesDict = rl_अन्वेषणम्_Range->fIncludesDict;
                भङ्ग;
            }
        }
        यदि (rlRange->f_अङ्कः == 0) {
            rlRange->f_First_अन्तर्_गणः = सत्य;
            यदि (rlRange->isDictionaryRange()) {
                rlRange->f_अङ्कः = ++dict_गणः_गणनम्;
                rlRange->fIncludesDict = सत्य;
            } अन्यथा {
                f_गणः_गणनम्++;
                rlRange->f_अङ्कः = f_गणः_गणनम्+2;
                सङ्कलय_Val_गन्तव्यम्_Sets(rlRange->fIncludesSets, rlRange->f_अङ्कः);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    fDictCategoriesStart = f_गणः_गणनम् + 3;
    प्रति (rlRange = fRangeList; rlRange!=शून्यसूचकः; rlRange=rlRange->f_अग्र) {
        यदि (rlRange->fIncludesDict) {
            rlRange->f_अङ्कः += fDictCategoriesStart - 1;
            यदि (rlRange->f_First_अन्तर्_गणः) {
                सङ्कलय_Val_गन्तव्यम्_Sets(rlRange->fIncludesSets, rlRange->f_अङ्कः);
            }
        }
    }
    f_गणः_गणनम् += dict_गणः_गणनम्;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    प्रति (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_ग्रन्थिः = static_cast<RBBINode*>(एतत्->fRB->fUSetNodes->elementAt(ni));
        यदि (uset_ग्रन्थिः==शून्यसूचकः) {
            भङ्ग;
        }
        Unicode_समुच्चयः      *निवेशः_समुच्चयः = uset_ग्रन्थिः->f_निवेशः_समुच्चयः;
        यदि (निवेशः_समुच्चयः->अन्तर्भवति(eofString)) {
            सङ्कलय_Val_गन्तव्यम्_समुच्चयः(uset_ग्रन्थिः, 1);
        }
    }


    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "rgroup")) {printRangeGroups();}
    यदि (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "esets")) {printSets();}
}


//
// Build the Trie table for mapping UChar32 values to the corresponding
// range group number.
//
शून्य RBBISetBuilder::buildTrie() {
    fMutableTrie = umutablecptrie_open(
                        0,       //  Initial value for all code points.
                        0,       //  Error value for out-of-range input.
                        f_स्थितिः);

    प्रति (RangeDescriptor *range = fRangeList; range!=शून्यसूचकः && U_SUCCESS(*f_स्थितिः); range=range->f_अग्र) {
        umutablecptrie_समुच्चयः_Range(fMutableTrie,
                                range->fStartChar,     // Range start
                                range->f_अन्तः_Char,       // Range end (inclusive)
                                range->f_अङ्कः,           // value for range
                                f_स्थितिः);
    }
}


शून्य RBBISetBuilder::mergeCategories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.second > categories.first);
    U_ASSERT((categories.first <  fDictCategoriesStart && categories.second <  fDictCategoriesStart) ||
             (categories.first >= fDictCategoriesStart && categories.second >= fDictCategoriesStart));

    प्रति (RangeDescriptor *rd = fRangeList; rd != शून्यसूचकः; rd = rd->f_अग्र) {
        int32_t range_अङ्कः = rd->f_अङ्कः;
        यदि (range_अङ्कः == categories.second) {
            rd->f_अङ्कः = categories.first;
        } अन्यथा यदि (range_अङ्कः > categories.second) {
            rd->f_अङ्कः--;
        }
    }
    --f_गणः_गणनम्;
    यदि (categories.second <= fDictCategoriesStart) {
        --fDictCategoriesStart;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::get_Trie_आकार()  {
    यदि (U_FAILURE(*f_स्थितिः)) {
        प्रत्यागम 0;
    }
    यदि (fTrie == शून्यसूचकः) {
        तार्किक use8_द्वयङ्काः = get_अङ्कः_Char_Categories() <= k_Max_Char_Categories_For8_द्वयङ्काः_Trie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8_द्वयङ्काः ? UCPTRIE_मानम्_द्वयङ्काः_8 : UCPTRIE_मानम्_द्वयङ्काः_16,
            f_स्थितिः);
        UError_संहिता बफर_स्थितिः = *f_स्थितिः;
        f_Trie_आकार = ucptrie_गन्तव्यम्_द्विमान(fTrie, शून्यसूचकः, 0, &बफर_स्थितिः);
        यदि (बफर_स्थितिः != U_बफर_अतिप्रवाहः_ERROR && U_FAILURE(बफर_स्थितिः)) {
            *f_स्थितिः = बफर_स्थितिः;
        }
    }
    प्रत्यागम f_Trie_आकार;
}


//-----------------------------------------------------------------------------------
//
//  serializeTrie()   Put the serialized trie at the specified address.
//                    Trust the caller to have given us enough memory.
//                    getTrieSize() MUST be called first.
//
//-----------------------------------------------------------------------------------
शून्य RBBISetBuilder::serializeTrie(uint8_t *where) {
    ucptrie_गन्तव्यम्_द्विमान(fTrie,
                     where,                //बफर
                     f_Trie_आकार,            //क्षमता
                     f_स्थितिः);
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
शून्य  RBBISetBuilder::सङ्कलय_Val_गन्तव्यम्_Sets(UVector *sets, uint32_t val) {
    int32_t       ix;

    प्रति (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_ग्रन्थिः = static_cast<RBBINode*>(sets->elementAt(ix));
        सङ्कलय_Val_गन्तव्यम्_समुच्चयः(uset_ग्रन्थिः, val);
    }
}

शून्य  RBBISetBuilder::सङ्कलय_Val_गन्तव्यम्_समुच्चयः(RBBINode *uset_ग्रन्थिः, uint32_t val) {
    RBBINode *पर्णम्_ग्रन्थिः = नूतन RBBINode(RBBINode::पर्णम्_Char, *f_स्थितिः);
    यदि (U_FAILURE(*f_स्थितिः)) {
        विलोपय पर्णम्_ग्रन्थिः;
        प्रत्यागम;
    }
    यदि (पर्णम्_ग्रन्थिः == शून्यसूचकः) {
        *f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    पर्णम्_ग्रन्थिः->fVal = static_cast<अचिह्नित ह्रस्व>(val);
    यदि (uset_ग्रन्थिः->f_वामः_Child == शून्यसूचकः) {
        uset_ग्रन्थिः->f_वामः_Child = पर्णम्_ग्रन्थिः;
        पर्णम्_ग्रन्थिः->fParent    = uset_ग्रन्थिः;
    } अन्यथा {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_ग्रन्थिः = नूतन RBBINode(RBBINode::opOr, *f_स्थितिः);
        यदि (or_ग्रन्थिः == शून्यसूचकः) {
            *f_स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        }
        यदि (U_FAILURE(*f_स्थितिः)) {
            विलोपय or_ग्रन्थिः;
            विलोपय पर्णम्_ग्रन्थिः;
            प्रत्यागम;
        }
        or_ग्रन्थिः->f_वामः_Child  = uset_ग्रन्थिः->f_वामः_Child;
        or_ग्रन्थिः->f_दक्षिणः_Child = पर्णम्_ग्रन्थिः;
        or_ग्रन्थिः->f_वामः_Child->fParent  = or_ग्रन्थिः;
        or_ग्रन्थिः->f_दक्षिणः_Child->fParent = or_ग्रन्थिः;
        uset_ग्रन्थिः->f_वामः_Child = or_ग्रन्थिः;
        or_ग्रन्थिः->fParent = uset_ग्रन्थिः;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_अङ्कः_Char_Categories() ध्रुव {
    प्रत्यागम f_गणः_गणनम् + 3;
}


//------------------------------------------------------------------------
//
//   getDictCategoriesStart
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::getDictCategoriesStart() ध्रुव {
    प्रत्यागम fDictCategoriesStart;
}


//------------------------------------------------------------------------
//
//   getFirstChar      Given a runtime RBBI character category, find
//                     the first UChar32 that is in the set of chars 
//                     in the category.
//------------------------------------------------------------------------
UChar32  RBBISetBuilder::getFirstChar(int32_t category) ध्रुव {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    प्रति (rlRange = fRangeList; rlRange!=शून्यसूचकः; rlRange=rlRange->f_अग्र) {
        यदि (rlRange->f_अङ्कः == category) {
            retVal = rlRange->fStartChar;
            भङ्ग;
        }
    }
    प्रत्यागम retVal;
}


//------------------------------------------------------------------------
//
//   printRanges        A debugging function.
//                      dump out all of the range definitions.
//
//------------------------------------------------------------------------
#यदि_परिभाषितम् RBBI_DEBUG
शून्य RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    पूर्णाङ्क                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    प्रति (rlRange = fRangeList; rlRange!=शून्यसूचकः; rlRange=rlRange->f_अग्र) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->fStartChar, rlRange->f_अन्तः_Char);

        प्रति (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_ग्रन्थिः    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   समुच्चयः_Name {u"anon"};
            RBBINode       *समुच्चयः_Ref = uset_ग्रन्थिः->fParent;
            यदि (समुच्चयः_Ref != शून्यसूचकः) {
                RBBINode *varRef = समुच्चयः_Ref->fParent;
                यदि (varRef != शून्यसूचकः  &&  varRef->fType == RBBINode::varRef) {
                    समुच्चयः_Name = varRef->fText;
                }
            }
            RBBI_DEBUG_printUnicodeString(समुच्चयः_Name); RBBIDebugPrintf("  ");
        }
        RBBIDebugPrintf("\n");
    }
}
#यदि_अन्तः


//------------------------------------------------------------------------
//
//   printRangeGroups     A debugging function.
//                        dump out all of the range groups.
//
//------------------------------------------------------------------------
#यदि_परिभाषितम् RBBI_DEBUG
शून्य RBBISetBuilder::printRangeGroups() {
    पूर्णाङ्क                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    प्रति (RangeDescriptor *rlRange = fRangeList; rlRange!=शून्यसूचकः; rlRange=rlRange->f_अग्र) {
        यदि (rlRange->f_First_अन्तर्_गणः) {
            पूर्णाङ्क गणः_अङ्कः = rlRange->f_अङ्कः;
            RBBIDebugPrintf("%2i  ", गणः_अङ्कः);

            यदि (गणः_अङ्कः >= fDictCategoriesStart) { RBBIDebugPrintf(" <DICT> ");}

            प्रति (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_ग्रन्थिः    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   समुच्चयः_Name = UNICODE_STRING("anon", 4);
                RBBINode       *समुच्चयः_Ref = uset_ग्रन्थिः->fParent;
                यदि (समुच्चयः_Ref != शून्यसूचकः) {
                    RBBINode *varRef = समुच्चयः_Ref->fParent;
                    यदि (varRef != शून्यसूचकः  &&  varRef->fType == RBBINode::varRef) {
                        समुच्चयः_Name = varRef->fText;
                    }
                }
                RBBI_DEBUG_printUnicodeString(समुच्चयः_Name); RBBIDebugPrintf(" ");
            }

            i = 0;
            प्रति (RangeDescriptor *tRange = rlRange; tRange != शून्यसूचकः; tRange = tRange->f_अग्र) {
                यदि (tRange->f_अङ्कः == rlRange->f_अङ्कः) {
                    यदि (i++ % 5 == 0) {
                        RBBIDebugPrintf("\n    ");
                    }
                    RBBIDebugPrintf("  %05x-%05x", tRange->fStartChar, tRange->f_अन्तः_Char);
                }
            }
            RBBIDebugPrintf("\n");
        }
    }
    RBBIDebugPrintf("\n");
}
#यदि_अन्तः


//------------------------------------------------------------------------
//
//   printSets          A debugging function.
//                      dump out all of the set definitions.
//
//------------------------------------------------------------------------
#यदि_परिभाषितम् RBBI_DEBUG
शून्य RBBISetBuilder::printSets() {
    पूर्णाङ्क                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    प्रति (i=0; ; i++) {
        RBBINode        *uset_ग्रन्थिः;
        RBBINode        *समुच्चयः_Ref;
        RBBINode        *varRef;
        UnicodeString    समुच्चयः_Name;

        uset_ग्रन्थिः = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        यदि (uset_ग्रन्थिः == शून्यसूचकः) {
            भङ्ग;
        }

        RBBIDebugPrintf("%3d    ", i);
        समुच्चयः_Name = UNICODE_STRING("anonymous", 9);
        समुच्चयः_Ref = uset_ग्रन्थिः->fParent;
        यदि (समुच्चयः_Ref != शून्यसूचकः) {
            varRef = समुच्चयः_Ref->fParent;
            यदि (varRef != शून्यसूचकः  &&  varRef->fType == RBBINode::varRef) {
                समुच्चयः_Name = varRef->fText;
            }
        }
        RBBI_DEBUG_printUnicodeString(समुच्चयः_Name);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_ग्रन्थिः->fText);
        RBBIDebugPrintf("\n");
        यदि (uset_ग्रन्थिः->f_वामः_Child != शून्यसूचकः) {
            RBBINode::print_वृक्षः(uset_ग्रन्थिः->f_वामः_Child, सत्य);
        }
    }
    RBBIDebugPrintf("\n");
}
#यदि_अन्तः



//-------------------------------------------------------------------------------------
//
//  RangeDescriptor copy constructor
//
//-------------------------------------------------------------------------------------

RangeDescriptor::RangeDescriptor(ध्रुव RangeDescriptor &other, UError_संहिता &स्थितिः) :
        fStartChar(other.fStartChar), f_अन्तः_Char {other.f_अन्तः_Char}, f_अङ्कः {other.f_अङ्कः},
        fIncludesDict{other.fIncludesDict}, f_First_अन्तर्_गणः{other.f_First_अन्तर्_गणः} {

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    fIncludesSets = नूतन UVector(स्थितिः);
    यदि (एतत्->fIncludesSets == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    प्रति (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        एतत्->fIncludesSets->सङ्कलय_Element(other.fIncludesSets->elementAt(i), स्थितिः);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    fIncludesSets = नूतन UVector(स्थितिः);
    यदि (fIncludesSets == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor Destructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::~RangeDescriptor() {
    विलोपय  fIncludesSets;
    fIncludesSets = शून्यसूचकः;
}

//-------------------------------------------------------------------------------------
//
//  RangeDesriptor::split()
//
//-------------------------------------------------------------------------------------
शून्य RangeDescriptor::विभज(UChar32 where, UError_संहिता &स्थितिः) {
    U_ASSERT(where>fStartChar && where<=f_अन्तः_Char);
    RangeDescriptor *nr = नूतन RangeDescriptor(*एतत्, स्थितिः);
    यदि(nr == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    यदि (U_FAILURE(स्थितिः)) {
        विलोपय nr;
        प्रत्यागम;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->fStartChar = where;
    एतत्->f_अन्तः_Char = where-1;
    nr->f_अग्र      = एतत्->f_अग्र;
    एतत्->f_अग्र    = nr;
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
तार्किक RangeDescriptor::isDictionaryRange() {
    स्थिर ध्रुव char16_t *dictionary = u"dictionary";
    प्रति (int32_t i=0; i<fIncludesSets->size(); i++) {
        RBBINode* uset_ग्रन्थिः = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *समुच्चयः_Ref = uset_ग्रन्थिः->fParent;
        यदि (समुच्चयः_Ref != शून्यसूचकः) {
            RBBINode *varRef = समुच्चयः_Ref->fParent;
            यदि (varRef && varRef->fType == RBBINode::varRef) {
                ध्रुव UnicodeString *समुच्चयः_Name = &varRef->fText;
                यदि (समुच्चयः_Name->compare(dictionary, -1) == 0) {
                    प्रत्यागम सत्य;
                }
            }
        }
    }
    प्रत्यागम असत्य;
}

U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */
