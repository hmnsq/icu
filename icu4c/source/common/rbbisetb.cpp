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

ثابت int32_t k_Max_Char_Categories_For8_بیتها_Trie = 255;
//------------------------------------------------------------------------
//
//   Constructor
//
//------------------------------------------------------------------------
RBBISetBuilder::RBBISetBuilder(RBBIRuleBuilder *rb)
{
    fRB             = rb;
    f_وضعیت         = rb->f_وضعیت;
    fRangeList      = اشاره_تهی;
    fMutableTrie    = اشاره_تهی;
    fTrie           = اشاره_تهی;
    f_Trie_اندازه       = 0;
    f_گروه_شمار     = 0;
}


//------------------------------------------------------------------------
//
//   Destructor
//
//------------------------------------------------------------------------
RBBISetBuilder::~RBBISetBuilder()
{
    RangeDescriptor   *بعدی_Range_Desc;

    // Walk through & delete the linked list of RangeDescriptors
    لكل (بعدی_Range_Desc = fRangeList; بعدی_Range_Desc!=اشاره_تهی;) {
        RangeDescriptor *r = بعدی_Range_Desc;
        بعدی_Range_Desc      = r->f_بعدی;
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
فراغ RBBISetBuilder::buildRanges() {
    RBBINode        *uset_گره;
    RangeDescriptor *rlRange;

    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "usets")) {printSets();}

    //
    //  Initialize the process by creating a single range encompassing all characters
    //  that is in no sets.
    //
    fRangeList                = جديد RangeDescriptor(*f_وضعیت); // will check for status here
    إذا (fRangeList == اشاره_تهی) {
        *f_وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    fRangeList->f_شروع_Char    = 0;
    fRangeList->f_انتها_Char      = 0x10ffff;

    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }

    //
    //  Find the set of non-overlapping ranges of characters
    //
    صحيح  ni;
    لكل (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_گره = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        إذا (uset_گره==اشاره_تهی) {
            اكسر;
        }

        Unicode_مجموعه      *درون_داد_مجموعه             = uset_گره->f_درون_داد_مجموعه;
        int32_t          درون_داد_مجموعه_Range_شمار   = درون_داد_مجموعه->get_Range_شمار();
        صحيح              درون_داد_مجموعه_Range_شاخص   = 0;
                         rlRange              = fRangeList;

        لكل (;;) {
            إذا (درون_داد_مجموعه_Range_شاخص >= درون_داد_مجموعه_Range_شمار) {
                اكسر;
            }
            UChar32      درون_داد_مجموعه_Range_Begin  = درون_داد_مجموعه->get_Range_شروع(درون_داد_مجموعه_Range_شاخص);
            UChar32      درون_داد_مجموعه_Range_انتها    = درون_داد_مجموعه->get_Range_انتها(درون_داد_مجموعه_Range_شاخص);

            // skip over ranges from the range list that are completely
            //   below the current range from the input unicode set.
            طالما (rlRange->f_انتها_Char < درون_داد_مجموعه_Range_Begin) {
                rlRange = rlRange->f_بعدی;
            }

            // If the start of the range from the range list is before with
            //   the start of the range from the unicode set, split the range list range
            //   in two, with one part being before (wholly outside of) the unicode set
            //   and the other containing the rest.
            //   Then continue the loop; the post-split current range will then be skipped
            //     over
            إذا (rlRange->f_شروع_Char < درون_داد_مجموعه_Range_Begin) {
                rlRange->تقسیم(درون_داد_مجموعه_Range_Begin, *f_وضعیت);
                إذا (U_FAILURE(*f_وضعیت)) {
                    أرجع;
                }
                استمر;
            }

            // Same thing at the end of the ranges...
            // If the end of the range from the range list doesn't coincide with
            //   the end of the range from the unicode set, split the range list
            //   range in two.  The first part of the split range will be
            //   wholly inside the Unicode set.
            إذا (rlRange->f_انتها_Char > درون_داد_مجموعه_Range_انتها) {
                rlRange->تقسیم(درون_داد_مجموعه_Range_انتها+1, *f_وضعیت);
                إذا (U_FAILURE(*f_وضعیت)) {
                    أرجع;
                }
            }

            // The current rlRange is now entirely within the UnicodeSet range.
            // Add this unicode set to the list of sets for this rlRange
            إذا (rlRange->fIncludesSets->شاخص_از(uset_گره) == -1) {
                rlRange->fIncludesSets->جمع_کن_Element(uset_گره, *f_وضعیت);
                إذا (U_FAILURE(*f_وضعیت)) {
                    أرجع;
                }
            }

            // Advance over ranges that we are finished with.
            إذا (درون_داد_مجموعه_Range_انتها == rlRange->f_انتها_Char) {
                درون_داد_مجموعه_Range_شاخص++;
            }
            rlRange = rlRange->f_بعدی;
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
    RangeDescriptor *rl_جستجو_Range;
    int32_t dict_گروه_شمار = 0;

    لكل (rlRange = fRangeList; rlRange!=اشاره_تهی; rlRange=rlRange->f_بعدی) {
        لكل (rl_جستجو_Range=fRangeList; rl_جستجو_Range != rlRange; rl_جستجو_Range=rl_جستجو_Range->f_بعدی) {
            إذا (rlRange->fIncludesSets->equals(*rl_جستجو_Range->fIncludesSets)) {
                rlRange->f_رقم = rl_جستجو_Range->f_رقم;
                rlRange->fIncludesDict = rl_جستجو_Range->fIncludesDict;
                اكسر;
            }
        }
        إذا (rlRange->f_رقم == 0) {
            rlRange->f_اولی_درون_گروه = درست;
            إذا (rlRange->isDictionaryRange()) {
                rlRange->f_رقم = ++dict_گروه_شمار;
                rlRange->fIncludesDict = درست;
            } وإلا {
                f_گروه_شمار++;
                rlRange->f_رقم = f_گروه_شمار+2;
                جمع_کن_Val_مقصد_Sets(rlRange->fIncludesSets, rlRange->f_رقم);
            }
        }
    }

    // Move the character category numbers for any dictionary ranges up, so that they
    // immediately follow the non-dictionary ranges.

    f_Dict_Categories_شروع = f_گروه_شمار + 3;
    لكل (rlRange = fRangeList; rlRange!=اشاره_تهی; rlRange=rlRange->f_بعدی) {
        إذا (rlRange->fIncludesDict) {
            rlRange->f_رقم += f_Dict_Categories_شروع - 1;
            إذا (rlRange->f_اولی_درون_گروه) {
                جمع_کن_Val_مقصد_Sets(rlRange->fIncludesSets, rlRange->f_رقم);
            }
        }
    }
    f_گروه_شمار += dict_گروه_شمار;


    // Handle input sets that contain the special string {eof}.
    //   Column 1 of the state table is reserved for EOF on input.
    //   Add this column value (1) to the equivalent expression
    //     subtree for each UnicodeSet that contains the string {eof}
    //   Because {eof} is not a character in the normal sense,
    //   it doesn’t affect the computation of ranges or TRIE.

    UnicodeString eofString(u"eof");
    لكل (ni=0; ; ni++) {        // Loop over each of the UnicodeSets encountered in the input rules
        uset_گره = static_cast<RBBINode*>(هذا->fRB->fUSetNodes->elementAt(ni));
        إذا (uset_گره==اشاره_تهی) {
            اكسر;
        }
        Unicode_مجموعه      *درون_داد_مجموعه = uset_گره->f_درون_داد_مجموعه;
        إذا (درون_داد_مجموعه->شامل_است(eofString)) {
            جمع_کن_Val_مقصد_مجموعه(uset_گره, 1);
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
                        f_وضعیت);

    لكل (RangeDescriptor *range = fRangeList; range!=اشاره_تهی && U_SUCCESS(*f_وضعیت); range=range->f_بعدی) {
        umutablecptrie_مجموعه_Range(fMutableTrie,
                                range->f_شروع_Char,     // Range start
                                range->f_انتها_Char,       // Range end (inclusive)
                                range->f_رقم,           // value for range
                                f_وضعیت);
    }
}


فراغ RBBISetBuilder::ادغام_Categories(IntPair categories) {
    U_ASSERT(categories.first >= 1);
    U_ASSERT(categories.دومی > categories.first);
    U_ASSERT((categories.first <  f_Dict_Categories_شروع && categories.دومی <  f_Dict_Categories_شروع) ||
             (categories.first >= f_Dict_Categories_شروع && categories.دومی >= f_Dict_Categories_شروع));

    لكل (RangeDescriptor *rd = fRangeList; rd != اشاره_تهی; rd = rd->f_بعدی) {
        int32_t range_رقم = rd->f_رقم;
        إذا (range_رقم == categories.دومی) {
            rd->f_رقم = categories.first;
        } وإلا إذا (range_رقم > categories.دومی) {
            rd->f_رقم--;
        }
    }
    --f_گروه_شمار;
    إذا (categories.دومی <= f_Dict_Categories_شروع) {
        --f_Dict_Categories_شروع;
    }
}


//-----------------------------------------------------------------------------------
//
//  getTrieSize()    Return the size that will be required to serialize the Trie.
//
//-----------------------------------------------------------------------------------
int32_t RBBISetBuilder::get_Trie_اندازه()  {
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع 0;
    }
    إذا (fTrie == اشاره_تهی) {
        منطقي use8_بیتها = get_رقم_Char_Categories() <= k_Max_Char_Categories_For8_بیتها_Trie;
        fTrie = umutablecptrie_buildImmutable(
            fMutableTrie,
            UCPTRIE_TYPE_FAST,
            use8_بیتها ? UCPTRIE_مقدار_بیتها_8 : UCPTRIE_مقدار_بیتها_16,
            f_وضعیت);
        UError_کد میانگیر_وضعیت = *f_وضعیت;
        f_Trie_اندازه = ucptrie_مقصد_دودویی(fTrie, اشاره_تهی, 0, &میانگیر_وضعیت);
        إذا (میانگیر_وضعیت != U_میانگیر_سرریز_ERROR && U_FAILURE(میانگیر_وضعیت)) {
            *f_وضعیت = میانگیر_وضعیت;
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
    ucptrie_مقصد_دودویی(fTrie,
                     where,                //میانگیر
                     f_Trie_اندازه,            //ظرفیت
                     f_وضعیت);
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
فراغ  RBBISetBuilder::جمع_کن_Val_مقصد_Sets(UVector *sets, uint32_t val) {
    int32_t       ix;

    لكل (ix=0; ix<sets->size(); ix++) {
        RBBINode* uset_گره = static_cast<RBBINode*>(sets->elementAt(ix));
        جمع_کن_Val_مقصد_مجموعه(uset_گره, val);
    }
}

فراغ  RBBISetBuilder::جمع_کن_Val_مقصد_مجموعه(RBBINode *uset_گره, uint32_t val) {
    RBBINode *برگ_گره = جديد RBBINode(RBBINode::برگ_Char, *f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        حذف برگ_گره;
        أرجع;
    }
    إذا (برگ_گره == اشاره_تهی) {
        *f_وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    برگ_گره->fVal = static_cast<بیعلامت کوتاه>(val);
    إذا (uset_گره->f_چپ_فرزند == اشاره_تهی) {
        uset_گره->f_چپ_فرزند = برگ_گره;
        برگ_گره->f_والد    = uset_گره;
    } وإلا {
        // There are already input symbols present for this set.
        // Set up an OR node, with the previous stuff as the left child
        //   and the new value as the right child.
        RBBINode *or_گره = جديد RBBINode(RBBINode::opOr, *f_وضعیت);
        إذا (or_گره == اشاره_تهی) {
            *f_وضعیت = U_حافظه_تخصیص_ERROR;
        }
        إذا (U_FAILURE(*f_وضعیت)) {
            حذف or_گره;
            حذف برگ_گره;
            أرجع;
        }
        or_گره->f_چپ_فرزند  = uset_گره->f_چپ_فرزند;
        or_گره->f_راست_فرزند = برگ_گره;
        or_گره->f_چپ_فرزند->f_والد  = or_گره;
        or_گره->f_راست_فرزند->f_والد = or_گره;
        uset_گره->f_چپ_فرزند = or_گره;
        or_گره->f_والد = uset_گره;
    }
}


//------------------------------------------------------------------------
//
//   getNumCharCategories
//
//------------------------------------------------------------------------
int32_t  RBBISetBuilder::get_رقم_Char_Categories() ثابت {
    أرجع f_گروه_شمار + 3;
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
UChar32  RBBISetBuilder::get_اولی_Char(int32_t category) ثابت {
    RangeDescriptor   *rlRange;
    UChar32 retVal = static_cast<UChar32>(-1);
    لكل (rlRange = fRangeList; rlRange!=اشاره_تهی; rlRange=rlRange->f_بعدی) {
        إذا (rlRange->f_رقم == category) {
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
#اگر_تعریف_شده RBBI_DEBUG
فراغ RBBISetBuilder::printRanges() {
    RangeDescriptor       *rlRange;
    صحيح                    i;

    RBBIDebugPrintf("\n\n Nonoverlapping Ranges ...\n");
    لكل (rlRange = fRangeList; rlRange!=اشاره_تهی; rlRange=rlRange->f_بعدی) {
        RBBIDebugPrintf("%4x-%4x  ", rlRange->f_شروع_Char, rlRange->f_انتها_Char);

        لكل (i=0; i<rlRange->fIncludesSets->size(); i++) {
            RBBINode       *uset_گره    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
            UnicodeString   مجموعه_Name {u"anon"};
            RBBINode       *مجموعه_Ref = uset_گره->f_والد;
            إذا (مجموعه_Ref != اشاره_تهی) {
                RBBINode *varRef = مجموعه_Ref->f_والد;
                إذا (varRef != اشاره_تهی  &&  varRef->fType == RBBINode::varRef) {
                    مجموعه_Name = varRef->f_متن;
                }
            }
            RBBI_DEBUG_printUnicodeString(مجموعه_Name); RBBIDebugPrintf("  ");
        }
        RBBIDebugPrintf("\n");
    }
}
#پایان_اگر


//------------------------------------------------------------------------
//
//   printRangeGroups     A debugging function.
//                        dump out all of the range groups.
//
//------------------------------------------------------------------------
#اگر_تعریف_شده RBBI_DEBUG
فراغ RBBISetBuilder::printRangeGroups() {
    صحيح                    i;

    RBBIDebugPrintf("\nRanges grouped by Unicode Set Membership...\n");
    لكل (RangeDescriptor *rlRange = fRangeList; rlRange!=اشاره_تهی; rlRange=rlRange->f_بعدی) {
        إذا (rlRange->f_اولی_درون_گروه) {
            صحيح گروه_رقم = rlRange->f_رقم;
            RBBIDebugPrintf("%2i  ", گروه_رقم);

            إذا (گروه_رقم >= f_Dict_Categories_شروع) { RBBIDebugPrintf(" <DICT> ");}

            لكل (i=0; i<rlRange->fIncludesSets->size(); i++) {
                RBBINode       *uset_گره    = (RBBINode *)rlRange->fIncludesSets->elementAt(i);
                UnicodeString   مجموعه_Name = UNICODE_STRING("anon", 4);
                RBBINode       *مجموعه_Ref = uset_گره->f_والد;
                إذا (مجموعه_Ref != اشاره_تهی) {
                    RBBINode *varRef = مجموعه_Ref->f_والد;
                    إذا (varRef != اشاره_تهی  &&  varRef->fType == RBBINode::varRef) {
                        مجموعه_Name = varRef->f_متن;
                    }
                }
                RBBI_DEBUG_printUnicodeString(مجموعه_Name); RBBIDebugPrintf(" ");
            }

            i = 0;
            لكل (RangeDescriptor *tRange = rlRange; tRange != اشاره_تهی; tRange = tRange->f_بعدی) {
                إذا (tRange->f_رقم == rlRange->f_رقم) {
                    إذا (i++ % 5 == 0) {
                        RBBIDebugPrintf("\n    ");
                    }
                    RBBIDebugPrintf("  %05x-%05x", tRange->f_شروع_Char, tRange->f_انتها_Char);
                }
            }
            RBBIDebugPrintf("\n");
        }
    }
    RBBIDebugPrintf("\n");
}
#پایان_اگر


//------------------------------------------------------------------------
//
//   printSets          A debugging function.
//                      dump out all of the set definitions.
//
//------------------------------------------------------------------------
#اگر_تعریف_شده RBBI_DEBUG
فراغ RBBISetBuilder::printSets() {
    صحيح                   i;

    RBBIDebugPrintf("\n\nUnicode Sets List\n------------------\n");
    لكل (i=0; ; i++) {
        RBBINode        *uset_گره;
        RBBINode        *مجموعه_Ref;
        RBBINode        *varRef;
        UnicodeString    مجموعه_Name;

        uset_گره = (RBBINode *)fRB->fUSetNodes->elementAt(i);
        إذا (uset_گره == اشاره_تهی) {
            اكسر;
        }

        RBBIDebugPrintf("%3d    ", i);
        مجموعه_Name = UNICODE_STRING("anonymous", 9);
        مجموعه_Ref = uset_گره->f_والد;
        إذا (مجموعه_Ref != اشاره_تهی) {
            varRef = مجموعه_Ref->f_والد;
            إذا (varRef != اشاره_تهی  &&  varRef->fType == RBBINode::varRef) {
                مجموعه_Name = varRef->f_متن;
            }
        }
        RBBI_DEBUG_printUnicodeString(مجموعه_Name);
        RBBIDebugPrintf("   ");
        RBBI_DEBUG_printUnicodeString(uset_گره->f_متن);
        RBBIDebugPrintf("\n");
        إذا (uset_گره->f_چپ_فرزند != اشاره_تهی) {
            RBBINode::print_درخت(uset_گره->f_چپ_فرزند, درست);
        }
    }
    RBBIDebugPrintf("\n");
}
#پایان_اگر



//-------------------------------------------------------------------------------------
//
//  RangeDescriptor copy constructor
//
//-------------------------------------------------------------------------------------

RangeDescriptor::RangeDescriptor(ثابت RangeDescriptor &other, UError_کد &وضعیت) :
        f_شروع_Char(other.f_شروع_Char), f_انتها_Char {other.f_انتها_Char}, f_رقم {other.f_رقم},
        fIncludesDict{other.fIncludesDict}, f_اولی_درون_گروه{other.f_اولی_درون_گروه} {

    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    fIncludesSets = جديد UVector(وضعیت);
    إذا (هذا->fIncludesSets == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
    }
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    لكل (int32_t i=0; i<other.fIncludesSets->size(); i++) {
        هذا->fIncludesSets->جمع_کن_Element(other.fIncludesSets->elementAt(i), وضعیت);
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor default constructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::RangeDescriptor(UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    fIncludesSets = جديد UVector(وضعیت);
    إذا (fIncludesSets == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
    }
}


//-------------------------------------------------------------------------------------
//
//  RangeDesriptor Destructor
//
//-------------------------------------------------------------------------------------
RangeDescriptor::~RangeDescriptor() {
    حذف  fIncludesSets;
    fIncludesSets = اشاره_تهی;
}

//-------------------------------------------------------------------------------------
//
//  RangeDesriptor::split()
//
//-------------------------------------------------------------------------------------
فراغ RangeDescriptor::تقسیم(UChar32 where, UError_کد &وضعیت) {
    U_ASSERT(where>f_شروع_Char && where<=f_انتها_Char);
    RangeDescriptor *nr = جديد RangeDescriptor(*هذا, وضعیت);
    إذا(nr == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    إذا (U_FAILURE(وضعیت)) {
        حذف nr;
        أرجع;
    }
    //  RangeDescriptor copy constructor copies all fields.
    //  Only need to update those that are different after the split.
    nr->f_شروع_Char = where;
    هذا->f_انتها_Char = where-1;
    nr->f_بعدی      = هذا->f_بعدی;
    هذا->f_بعدی    = nr;
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
        RBBINode* uset_گره = static_cast<RBBINode*>(fIncludesSets->elementAt(i));
        RBBINode *مجموعه_Ref = uset_گره->f_والد;
        إذا (مجموعه_Ref != اشاره_تهی) {
            RBBINode *varRef = مجموعه_Ref->f_والد;
            إذا (varRef && varRef->fType == RBBINode::varRef) {
                ثابت UnicodeString *مجموعه_Name = &varRef->f_متن;
                إذا (مجموعه_Name->compare(dictionary, -1) == 0) {
                    أرجع درست;
                }
            }
        }
    }
    أرجع خطأ;
}

U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */
