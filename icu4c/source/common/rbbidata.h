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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "udataswp.h"

/**
 * Swap RBBI data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ubrk_swap(സ്ഥിരം UDataSwapper *ds,
          സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
          UErrorCode *pErrorCode);

#ifdef __cplusplus

#അന്തര്ഭാവ "unicode/ucptrie.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/uversion.h"
#അന്തര്ഭാവ "umutex.h"


U_NAMESPACE_BEGIN

// The current RBBI data format version.
സ്ഥിതം സ്ഥിരം uint8_t RBBI_DATA_FORMAT_VERSION[] = {7, 0, 0, 0};

/*  
 *   The following structs map exactly onto the raw data from ICU common data file. 
 */
ഘടന RBBIDataHeader {
    uint32_t         fMagic;           /*  == 0xbla0                                               */
    UVersionInfo     fFormatVersion;   /* Data Format.  Same as the value in struct UDataInfo      */
                                       /*   if there is one associated with this data.             */
                                       /*     (version originates in rbbi, is copied to UDataInfo) */
    uint32_t         fLength;          /*  Total length in bytes of this RBBI Data,                */
                                       /*      including all sections, not just the header.        */
    uint32_t         f_Cat_എണ്ണം;        /*  Number of character categories.                         */

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
ഘടന RBBIStateTableRowT {
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
    T               fNextState[1]; //  Next State, indexed by char category.
                                   //    Variable-length array declared with length 1
                                   //    to disable bounds checkers.
                                   //    Array Size is actually fData->fHeader->fCatCount
                                   //    CAUTION:  see RBBITableBuilder::getTableSize()
                                   //              before changing anything here.
};

തരനിർവചനം RBBIStateTableRowT<uint8_t> RBBIStateTableRow8;
തരനിർവചനം RBBIStateTableRowT<uint16_t> RBBIStateTableRow16;

constexpr uint16_t ACCEPTING_UNCONDITIONAL = 1;   // Value constant for RBBIStateTableRow::fAccepting

സംഘം RBBIStateTableRow {
  RBBIStateTableRow16 r16;
  RBBIStateTableRow8 r8;
};

ഘടന RBBIStateTable {
    uint32_t         f_അക്കം_States;            // Number of states.
    uint32_t         fRowLen;               // Length of a state table row, in bytes.
    uint32_t         fDictCategoriesStart;  // Char category number of the first dictionary
                                            //   char class, or the largest category number + 1
                                            //   if there are no dictionary categories.
    uint32_t         fLookAheadResultsSize; // Size of run-time array required for holding
                                            //   look-ahead results. Indexed by row.fLookAhead.
    uint32_t         fFlags;                // Option Flags for this state table.
    അക്ഷരം             fTableData[1];         // First RBBIStateTableRow begins here.
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
class RBBIDataWrapper : പൊതു UMemory {
പൊതു:
    എണ്ണൽ EDontAdopt {
        kDontAdopt
    };
    RBBIDataWrapper(സ്ഥിരം RBBIDataHeader *data, UErrorCode &status);
    RBBIDataWrapper(സ്ഥിരം RBBIDataHeader *data, എണ്ണൽ EDontAdopt dontAdopt, UErrorCode &status);
    RBBIDataWrapper(UDataMemory* udm, UErrorCode &status);
    ~RBBIDataWrapper();

    സ്ഥിതം UBool          isDataVersionAcceptable(സ്ഥിരം UVersionInfo version);

    ശൂന്യം                  init0();
    ശൂന്യം                  init(സ്ഥിരം RBBIDataHeader *data, UErrorCode &status);
    RBBIDataWrapper      *addReference();
    ശൂന്യം                  removeReference();
    bool                  operator ==(സ്ഥിരം RBBIDataWrapper &other) സ്ഥിരം;
    int32_t               hashCode();
    സ്ഥിരം UnicodeString  &getRuleSourceString() സ്ഥിരം;
    ശൂന്യം                  printData();
    ശൂന്യം                  printTable(സ്ഥിരം അക്ഷരം *heading, സ്ഥിരം RBBIStateTable *table);

    /*                                     */
    /*   Pointers to items within the data */
    /*                                     */
    സ്ഥിരം RBBIDataHeader     *fHeader;
    സ്ഥിരം RBBIStateTable     *fForwardTable;
    സ്ഥിരം RBBIStateTable     *fReverseTable;
    സ്ഥിരം അക്ഷരം               *fRuleSource;
    സ്ഥിരം int32_t            *fRuleStatusTable; 

    /* number of int32_t values in the rule status table.   Used to sanity check indexing */
    int32_t             fStatusMaxIdx;

    UCPTrie             *fTrie;

സ്വകാര്യ:
    u_atomic_int32_t    f_Ref_എണ്ണം;
    UDataMemory        *fUDataMem;
    UnicodeString       fRuleString;
    UBool               fDontFreeData;

    RBBIDataWrapper(സ്ഥിരം RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
    RBBIDataWrapper &operator=(സ്ഥിരം RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
};



U_NAMESPACE_END

U_CFUNC UBool rbbi_cleanup();

#endif /* C++ */

#endif
