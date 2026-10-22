// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1999-2014 International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  rbbidata.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   RBBI data formats  Includes
*
*                          Structs that describes the format of the Binary RBBI data,
*                          as it is stored in ICU's data file.
*
*      RBBIDataWrapper  -  Instances of this class sit between the
*                          raw data structs and the RulesBasedBreakIterator objects
*                          that are created by applications.  The wrapper class
*                          provides reference counting for the underlying data,
*                          and direct pointers to data that would not otherwise
*                          be accessible without ugly pointer arithmetic.  The
*                          wrapper does not attempt to provide any higher level
*                          abstractions for the data itself.
*
*                          There will be only one instance of RBBIDataWrapper for any
*                          set of RBBI run time data being shared by instances
*                          (clones) of RulesBasedBreakIterator.
*/

#ifndef __RBBIDATA_H__
#define __RBBIDATA_H__

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "udataswp.h"

/**
 * Swap RBBI data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ubrk_swap(ধ্রুবক UDataSwapper *ds,
          ধ্রুবক শূন্য *in_তথ্য, int32_t দৈর্ঘ্য, শূন্য *out_তথ্য,
          UErrorCode *pErrorCode);

#ifdef __cplusplus

#অন্তর্ভাব "unicode/ucptrie.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/uversion.h"
#অন্তর্ভাব "umutex.h"


U_NAMESPACE_BEGIN

// The current RBBI data format version.
স্থির ধ্রুবক uint8_t RBBI_তথ্য_FORMAT_VERSION[] = {7, 0, 0, 0};

/*  
 *   The following structs map exactly onto the raw data from ICU common data file. 
 */
কাঠামো RBBIDataHeader {
    uint32_t         fMagic;           /*  == 0xbla0                                               */
    UVersionInfo     fFormatVersion;   /* Data Format.  Same as the value in struct UDataInfo      */
                                       /*   if there is one associated with this data.             */
                                       /*     (version originates in rbbi, is copied to UDataInfo) */
    uint32_t         f_দৈর্ঘ্য;          /*  Total length in bytes of this RBBI Data,                */
                                       /*      including all sections, not just the header.        */
    uint32_t         f_Cat_গুনতি;        /*  Number of character categories.                         */

    /*                                                                        */
    /*  Offsets and sizes of each of the subsections within the RBBI data.    */
    /*  All offsets are bytes from the start of the RBBIDataHeader.           */
    /*  All sizes are in bytes.                                               */
    /*                                                                        */
    uint32_t         fFTable;         /*  forward state transition table. */
    uint32_t         fFTableLen;
    uint32_t         fRTable;         /*  Offset to the reverse state transition table. */
    uint32_t         fRTableLen;
    uint32_t         fTrie;           /*  Offset to Trie data for character categories */
    uint32_t         fTrieLen;
    uint32_t         fRuleSource;     /*  Offset to the source for for the break */
    uint32_t         fRuleSourceLen;  /*    rules.  Stored char16_t *. */
    uint32_t         fStatusTable;    /* Offset to the table of rule status values */
    uint32_t         fStatusTableLen;

    uint32_t         fReserved[6];    /*  Reserved for expansion */

};



template <typename T>
কাঠামো RBBIStateTableRowT {
    T               fAccepting;    //  Non-zero if this row is for an accepting state.
                                   //  Value 0: not an accepting state.
                                   //        1: (ACCEPTING_UNCONDITIONAL) Unconditional Accepting state.
                                   //       >1: Look-ahead match has completed.
                                   //           Actual boundary position happened earlier.
                                   //           Value here == fLookAhead in earlier
                                   //           state, at actual boundary pos.
    T               fLookAhead;    //  Non-zero if this row is for a state that
                                   //    corresponds to a '/' in the rule source.
                                   //    Value is the same as the fAccepting
                                   //    value for the rule (which will appear
                                   //    in a different state.
    T               fTagsIdx;      //  Non-zero if this row covers a {tagged} position
                                   //    from a rule.  Value is the index in the
                                   //    StatusTable of the set of matching
                                   //    tags (rule status values)
    T               f_পরবর্তী_State[1]; //  Next State, indexed by char category.
                                   //    Variable-length array declared with length 1
                                   //    to disable bounds checkers.
                                   //    Array Size is actually fData->fHeader->fCatCount
                                   //    CAUTION:  see RBBITableBuilder::getTableSize()
                                   //              before changing anything here.
};

প্রকারলেখ RBBIStateTableRowT<uint8_t> RBBIStateTableRow8;
প্রকারলেখ RBBIStateTableRowT<uint16_t> RBBIStateTableRow16;

constexpr uint16_t ACCEPTING_UNCONDITIONAL = 1;   // Value constant for RBBIStateTableRow::fAccepting

সংঘ RBBIStateTableRow {
  RBBIStateTableRow16 r16;
  RBBIStateTableRow8 r8;
};

কাঠামো RBBIStateTable {
    uint32_t         f_অঙ্ক_States;            // Number of states.
    uint32_t         fRowLen;               // Length of a state table row, in bytes.
    uint32_t         fDictCategoriesStart;  // Char category number of the first dictionary
                                            //   char class, or the largest category number + 1
                                            //   if there are no dictionary categories.
    uint32_t         f_Look_Ahead_Results_আকার; // Size of run-time array required for holding
                                            //   look-ahead results. Indexed by row.fLookAhead.
    uint32_t         fFlags;                // Option Flags for this state table.
    অক্ষর             f_Table_তথ্য[1];         // First RBBIStateTableRow begins here.
                                            //   Variable-length array declared with length 1
                                            //   to disable bounds checkers.
                                            //   (making it char[] simplifies ugly address
                                            //   arithmetic for indexing variable length rows.)
};

constexpr uint32_t RBBI_LOOKAHEAD_HARD_BREAK = 1;
// No longer used since ICU 79:
// constexpr uint32_t RBBI_BOF_REQUIRED = 2;
constexpr uint32_t RBBI_8BITS_ROWS = 4;


/*                                        */
/*   The reference counting wrapper class */
/*                                        */
class RBBIDataWrapper : সর্বজনীন UMemory {
সর্বজনীন:
    গণনা EDontAdopt {
        kDontAdopt
    };
    RBBIDataWrapper(ধ্রুবক RBBIDataHeader *তথ্য, UErrorCode &status);
    RBBIDataWrapper(ধ্রুবক RBBIDataHeader *তথ্য, গণনা EDontAdopt dontAdopt, UErrorCode &status);
    RBBIDataWrapper(UDataMemory* udm, UErrorCode &status);
    ~RBBIDataWrapper();

    স্থির UBool          is_তথ্য_Version_Acceptable(ধ্রুবক UVersionInfo version);

    শূন্য                  init0();
    শূন্য                  init(ধ্রুবক RBBIDataHeader *তথ্য, UErrorCode &status);
    RBBIDataWrapper      *addReference();
    শূন্য                  removeReference();
    bool                  operator ==(ধ্রুবক RBBIDataWrapper &other) ধ্রুবক;
    int32_t               hashCode();
    ধ্রুবক UnicodeString  &getRuleSourceString() ধ্রুবক;
    শূন্য                  print_তথ্য();
    শূন্য                  printTable(ধ্রুবক অক্ষর *heading, ধ্রুবক RBBIStateTable *table);

    /*                                     */
    /*   Pointers to items within the data */
    /*                                     */
    ধ্রুবক RBBIDataHeader     *fHeader;
    ধ্রুবক RBBIStateTable     *fForwardTable;
    ধ্রুবক RBBIStateTable     *fReverseTable;
    ধ্রুবক অক্ষর               *fRuleSource;
    ধ্রুবক int32_t            *fRuleStatusTable; 

    /* number of int32_t values in the rule status table.   Used to sanity check indexing */
    int32_t             fStatusMaxIdx;

    UCPTrie             *fTrie;

ব্যক্তিগত:
    u_atomic_int32_t    f_Ref_গুনতি;
    UDataMemory        *fUDataMem;
    UnicodeString       fRuleString;
    UBool               f_Dont_Free_তথ্য;

    RBBIDataWrapper(ধ্রুবক RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
    RBBIDataWrapper &operator=(ধ্রুবক RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
};



U_NAMESPACE_END

U_CFUNC UBool rbbi_cleanup();

#endif /* C++ */

#endif
