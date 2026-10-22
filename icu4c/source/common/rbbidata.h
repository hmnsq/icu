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

#अगर_अघोषित __RBBIDATA_H__
#घोषणा __RBBIDATA_H__

#समावेश "unicode/utypes.h"
#समावेश "unicode/udata.h"
#समावेश "udataswp.h"

/**
 * Swap RBBI data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ubrk_swap(स्थिर UDataSwapper *ds,
          स्थिर व्योम *in_तथ्य, int32_t लंबाई, व्योम *out_तथ्य,
          UErrorCode *pErrorCode);

#अगर_घोषित __cplusplus

#समावेश "unicode/ucptrie.h"
#समावेश "unicode/uobject.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/uversion.h"
#समावेश "umutex.h"


U_NAMESPACE_BEGIN

// The current RBBI data format version.
अटल स्थिर uint8_t RBBI_तथ्य_FORMAT_VERSION[] = {7, 0, 0, 0};

/*  
 *   The following structs map exactly onto the raw data from ICU common data file. 
 */
काष्ठा RBBIDataHeader {
    uint32_t         fMagic;           /*  == 0xbla0                                               */
    UVersionInfo     fFormatVersion;   /* Data Format.  Same as the value in struct UDataInfo      */
                                       /*   if there is one associated with this data.             */
                                       /*     (version originates in rbbi, is copied to UDataInfo) */
    uint32_t         f_लंबाई;          /*  Total length in bytes of this RBBI Data,                */
                                       /*      including all sections, not just the header.        */
    uint32_t         f_Cat_गिनती;        /*  Number of character categories.                         */

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



ढाँचा <typename T>
काष्ठा RBBIStateTableRowT {
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
    T               f_अग्र_State[1]; //  Next State, indexed by char category.
                                   //    Variable-length array declared with length 1
                                   //    to disable bounds checkers.
                                   //    Array Size is actually fData->fHeader->fCatCount
                                   //    CAUTION:  see RBBITableBuilder::getTableSize()
                                   //              before changing anything here.
};

प्रकार RBBIStateTableRowT<uint8_t> RBBIStateTableRow8;
प्रकार RBBIStateTableRowT<uint16_t> RBBIStateTableRow16;

constexpr uint16_t ACCEPTING_UNCONDITIONAL = 1;   // Value constant for RBBIStateTableRow::fAccepting

जोड़ RBBIStateTableRow {
  RBBIStateTableRow16 r16;
  RBBIStateTableRow8 r8;
};

काष्ठा RBBIStateTable {
    uint32_t         f_अंक_States;            // Number of states.
    uint32_t         fRowLen;               // Length of a state table row, in bytes.
    uint32_t         fDictCategoriesStart;  // Char category number of the first dictionary
                                            //   char class, or the largest category number + 1
                                            //   if there are no dictionary categories.
    uint32_t         f_Look_Ahead_Results_आकार; // Size of run-time array required for holding
                                            //   look-ahead results. Indexed by row.fLookAhead.
    uint32_t         fFlags;                // Option Flags for this state table.
    अक्षर             f_Table_तथ्य[1];         // First RBBIStateTableRow begins here.
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
class RBBIDataWrapper : खुला UMemory {
खुला:
    क्रमागत EDontAdopt {
        kDontAdopt
    };
    RBBIDataWrapper(स्थिर RBBIDataHeader *तथ्य, UErrorCode &status);
    RBBIDataWrapper(स्थिर RBBIDataHeader *तथ्य, क्रमागत EDontAdopt dontAdopt, UErrorCode &status);
    RBBIDataWrapper(UDataMemory* udm, UErrorCode &status);
    ~RBBIDataWrapper();

    अटल UBool          is_तथ्य_Version_Acceptable(स्थिर UVersionInfo version);

    व्योम                  init0();
    व्योम                  init(स्थिर RBBIDataHeader *तथ्य, UErrorCode &status);
    RBBIDataWrapper      *addReference();
    व्योम                  removeReference();
    bool                  चालक ==(स्थिर RBBIDataWrapper &other) स्थिर;
    int32_t               hashCode();
    स्थिर UnicodeString  &getRuleSourceString() स्थिर;
    व्योम                  print_तथ्य();
    व्योम                  printTable(स्थिर अक्षर *heading, स्थिर RBBIStateTable *table);

    /*                                     */
    /*   Pointers to items within the data */
    /*                                     */
    स्थिर RBBIDataHeader     *fHeader;
    स्थिर RBBIStateTable     *fForwardTable;
    स्थिर RBBIStateTable     *fReverseTable;
    स्थिर अक्षर               *fRuleSource;
    स्थिर int32_t            *fRuleStatusTable; 

    /* number of int32_t values in the rule status table.   Used to sanity check indexing */
    int32_t             fStatusMaxIdx;

    UCPTrie             *fTrie;

निजी:
    u_atomic_int32_t    f_Ref_गिनती;
    UDataMemory        *fUDataMem;
    UnicodeString       fRuleString;
    UBool               f_Dont_Free_तथ्य;

    RBBIDataWrapper(स्थिर RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
    RBBIDataWrapper &चालक=(स्थिर RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
};



U_NAMESPACE_END

U_CFUNC UBool rbbi_cleanup();

#पूर्ण_अगर /* C++ */

#पूर्ण_अगर
