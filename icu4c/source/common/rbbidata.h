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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __RBBIDATA_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __RBBIDATA_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"

/**
 * Swap RBBI data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ubrk_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
          ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
          UError_ਕੋਡ *p_Error_ਕੋਡ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uversion.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"


U_NAMESPACE_BEGIN

// The current RBBI data format version.
ਸਥਿਰ ਅਚਲ uint8_t RBBI_ਅੰਕੜੇ_FORMAT_ਸੰਸਕਰਣ[] = {7, 0, 0, 0};

/*  
 *   The following structs map exactly onto the raw data from ICU common data file. 
 */
ਬਣਤਰ RBBIData_ਸਿਰਲੇਖ {
    uint32_t         fMagic;           /*  == 0xbla0                                               */
    UVersionInfo     f_Format_ਸੰਸਕਰਣ;   /* Data Format.  Same as the value in struct UDataInfo      */
                                       /*   if there is one associated with this data.             */
                                       /*     (version originates in rbbi, is copied to UDataInfo) */
    uint32_t         f_ਲੰਮਾਈ;          /*  Total length in bytes of this RBBI Data,                */
                                       /*      including all sections, not just the header.        */
    uint32_t         f_Cat_ਗਿਣਤੀ;        /*  Number of character categories.                         */

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
    uint32_t         f_ਨਿਯਮ_ਸਰੋਤ;     /*  Offset to the source for for the break */
    uint32_t         f_ਨਿਯਮ_ਸਰੋਤ_Len;  /*    rules.  Stored char16_t *. */
    uint32_t         f_ਸਥਿਤੀ_ਸਾਰਣੀ;    /* Offset to the table of rule status values */
    uint32_t         f_ਸਥਿਤੀ_ਸਾਰਣੀ_Len;

    uint32_t         fReserved[6];    /*  Reserved for expansion */

};



ਸਾਂਚਾ <typename T>
ਬਣਤਰ RBBIState_ਸਾਰਣੀ_ਪੰਕਤੀ_T {
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
    T               f_ਟੈਗ_ਸੂਚੀ_Idx;      //  Non-zero if this row covers a {tagged} position
                                   //    from a rule.  Value is the index in the
                                   //    StatusTable of the set of matching
                                   //    tags (rule status values)
    T               f_ਅਗਲਾ_State[1]; //  Next State, indexed by char category.
                                   //    Variable-length array declared with length 1
                                   //    to disable bounds checkers.
                                   //    Array Size is actually fData->fHeader->fCatCount
                                   //    CAUTION:  see RBBITableBuilder::getTableSize()
                                   //              before changing anything here.
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ RBBIState_ਸਾਰਣੀ_ਪੰਕਤੀ_T<uint8_t> RBBIState_ਸਾਰਣੀ_Row8;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ RBBIState_ਸਾਰਣੀ_ਪੰਕਤੀ_T<uint16_t> RBBIState_ਸਾਰਣੀ_Row16;

constexpr uint16_t ACCEPTING_UNCONDITIONAL = 1;   // Value constant for RBBIStateTableRow::fAccepting

ਸੰਘ RBBIState_ਸਾਰਣੀ_ਪੰਕਤੀ {
  RBBIState_ਸਾਰਣੀ_Row16 r16;
  RBBIState_ਸਾਰਣੀ_Row8 r8;
};

ਬਣਤਰ RBBIState_ਸਾਰਣੀ {
    uint32_t         f_ਅੰਕ_ਹਾਲਤਾਂ;            // Number of states.
    uint32_t         f_ਪੰਕਤੀ_Len;               // Length of a state table row, in bytes.
    uint32_t         f_Dict_Categories_ਸ਼ੁਰੂ;  // Char category number of the first dictionary
                                            //   char class, or the largest category number + 1
                                            //   if there are no dictionary categories.
    uint32_t         f_Look_Ahead_Results_ਮਾਪ; // Size of run-time array required for holding
                                            //   look-ahead results. Indexed by row.fLookAhead.
    uint32_t         fFlags;                // Option Flags for this state table.
    ਅੱਖਰ             f_ਸਾਰਣੀ_ਅੰਕੜੇ[1];         // First RBBIStateTableRow begins here.
                                            //   Variable-length array declared with length 1
                                            //   to disable bounds checkers.
                                            //   (making it char[] simplifies ugly address
                                            //   arithmetic for indexing variable length rows.)
};

constexpr uint32_t RBBI_LOOKAHEAD_HARD_BREAK = 1;
// No longer used since ICU 79:
// constexpr uint32_t RBBI_BOF_REQUIRED = 2;
constexpr uint32_t RBBI_8_ਬਿੱਟ_ਪੰਕਤੀਆਂ = 4;


/*                                        */
/*   The reference counting wrapper class */
/*                                        */
class RBBIDataWrapper : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    ਗਿਣਤੀ EDontAdopt {
        kDontAdopt
    };
    RBBIDataWrapper(ਅਚਲ RBBIData_ਸਿਰਲੇਖ *ਅੰਕੜੇ, UError_ਕੋਡ &ਸਥਿਤੀ);
    RBBIDataWrapper(ਅਚਲ RBBIData_ਸਿਰਲੇਖ *ਅੰਕੜੇ, ਗਿਣਤੀ EDontAdopt dontAdopt, UError_ਕੋਡ &ਸਥਿਤੀ);
    RBBIDataWrapper(UData_ਸਮ੍ਰਿਤੀ* udm, UError_ਕੋਡ &ਸਥਿਤੀ);
    ~RBBIDataWrapper();

    ਸਥਿਰ UBool          is_ਅੰਕੜੇ_ਸੰਸਕਰਣ_Acceptable(ਅਚਲ UVersionInfo ਸੰਸਕਰਣ);

    ਖਾਲੀ                  init0();
    ਖਾਲੀ                  init(ਅਚਲ RBBIData_ਸਿਰਲੇਖ *ਅੰਕੜੇ, UError_ਕੋਡ &ਸਥਿਤੀ);
    RBBIDataWrapper      *ਜੋੜੋ_Reference();
    ਖਾਲੀ                  removeReference();
    bool                  ਸੰਚਾਲਕ ==(ਅਚਲ RBBIDataWrapper &other) ਅਚਲ;
    int32_t               hash_ਕੋਡ();
    ਅਚਲ UnicodeString  &get_ਨਿਯਮ_ਸਰੋਤ_String() ਅਚਲ;
    ਖਾਲੀ                  print_ਅੰਕੜੇ();
    ਖਾਲੀ                  print_ਸਾਰਣੀ(ਅਚਲ ਅੱਖਰ *heading, ਅਚਲ RBBIState_ਸਾਰਣੀ *ਸਾਰਣੀ);

    /*                                     */
    /*   Pointers to items within the data */
    /*                                     */
    ਅਚਲ RBBIData_ਸਿਰਲੇਖ     *f_ਸਿਰਲੇਖ;
    ਅਚਲ RBBIState_ਸਾਰਣੀ     *f_Forward_ਸਾਰਣੀ;
    ਅਚਲ RBBIState_ਸਾਰਣੀ     *f_ਉਲਟ_ਸਾਰਣੀ;
    ਅਚਲ ਅੱਖਰ               *f_ਨਿਯਮ_ਸਰੋਤ;
    ਅਚਲ int32_t            *f_ਨਿਯਮ_ਸਥਿਤੀ_ਸਾਰਣੀ; 

    /* number of int32_t values in the rule status table.   Used to sanity check indexing */
    int32_t             f_ਸਥਿਤੀ_Max_Idx;

    UCPTrie             *fTrie;

ਨਿਜੀ:
    u_atomic_int32_t    f_Ref_ਗਿਣਤੀ;
    UData_ਸਮ੍ਰਿਤੀ        *fUDataMem;
    UnicodeString       f_ਨਿਯਮ_String;
    UBool               f_Dont_Free_ਅੰਕੜੇ;

    RBBIDataWrapper(ਅਚਲ RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
    RBBIDataWrapper &ਸੰਚਾਲਕ=(ਅਚਲ RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
};



U_NAMESPACE_ਸਿਰਾ

U_CFUNC UBool rbbi_cleanup();

#ਜੇ_ਅੰਤ /* C++ */

#ਜੇ_ਅੰਤ
