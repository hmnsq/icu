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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __RBBIDATA_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __RBBIDATA_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"

/**
 * Swap RBBI data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ubrk_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t ಉದ್ದಳತೆ, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus

#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uversion.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"


U_NAMESPACE_BEGIN

// The current RBBI data format version.
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t RBBI_ದತ್ತಾಂಶ_FORMAT_ಆವೃತ್ತಿ[] = {7, 0, 0, 0};

/*  
 *   The following structs map exactly onto the raw data from ICU common data file. 
 */
ರಚನೆ RBBIData_ಶೀರ್ಷಿಕೆ {
    uint32_t         fMagic;           /*  == 0xbla0                                               */
    UVersionInfo     f_Format_ಆವೃತ್ತಿ;   /* Data Format.  Same as the value in struct UDataInfo      */
                                       /*   if there is one associated with this data.             */
                                       /*     (version originates in rbbi, is copied to UDataInfo) */
    uint32_t         f_ಉದ್ದಳತೆ;          /*  Total length in bytes of this RBBI Data,                */
                                       /*      including all sections, not just the header.        */
    uint32_t         f_Cat_ಎಣಿಕೆ;        /*  Number of character categories.                         */

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
    uint32_t         f_ನಿಯಮ_ಮೂಲ;     /*  Offset to the source for for the break */
    uint32_t         f_ನಿಯಮ_ಮೂಲ_Len;  /*    rules.  Stored char16_t *. */
    uint32_t         f_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ;    /* Offset to the table of rule status values */
    uint32_t         f_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ_Len;

    uint32_t         fReserved[6];    /*  Reserved for expansion */

};



ಮಾದರಿ <typename T>
ರಚನೆ RBBIState_ಕೋಷ್ಟಕ_ಅಡ್ಡಸಾಲು_T {
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
    T               f_ಟ್ಯಾಗುಗಳು_Idx;      //  Non-zero if this row covers a {tagged} position
                                   //    from a rule.  Value is the index in the
                                   //    StatusTable of the set of matching
                                   //    tags (rule status values)
    T               f_ಮುಂದಿನ_State[1]; //  Next State, indexed by char category.
                                   //    Variable-length array declared with length 1
                                   //    to disable bounds checkers.
                                   //    Array Size is actually fData->fHeader->fCatCount
                                   //    CAUTION:  see RBBITableBuilder::getTableSize()
                                   //              before changing anything here.
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ RBBIState_ಕೋಷ್ಟಕ_ಅಡ್ಡಸಾಲು_T<uint8_t> RBBIState_ಕೋಷ್ಟಕ_Row8;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ RBBIState_ಕೋಷ್ಟಕ_ಅಡ್ಡಸಾಲು_T<uint16_t> RBBIState_ಕೋಷ್ಟಕ_Row16;

constexpr uint16_t ACCEPTING_UNCONDITIONAL = 1;   // Value constant for RBBIStateTableRow::fAccepting

ಒಕ್ಕೂಟ RBBIState_ಕೋಷ್ಟಕ_ಅಡ್ಡಸಾಲು {
  RBBIState_ಕೋಷ್ಟಕ_Row16 r16;
  RBBIState_ಕೋಷ್ಟಕ_Row8 r8;
};

ರಚನೆ RBBIState_ಕೋಷ್ಟಕ {
    uint32_t         f_ಅಂಕೆ_ಸ್ಥಿತಿಗಳು;            // Number of states.
    uint32_t         f_ಅಡ್ಡಸಾಲು_Len;               // Length of a state table row, in bytes.
    uint32_t         f_Dict_Categories_ಆರಂಭ;  // Char category number of the first dictionary
                                            //   char class, or the largest category number + 1
                                            //   if there are no dictionary categories.
    uint32_t         f_Look_Ahead_Results_ಗಾತ್ರ; // Size of run-time array required for holding
                                            //   look-ahead results. Indexed by row.fLookAhead.
    uint32_t         fFlags;                // Option Flags for this state table.
    ಅಕ್ಷರ             f_ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ[1];         // First RBBIStateTableRow begins here.
                                            //   Variable-length array declared with length 1
                                            //   to disable bounds checkers.
                                            //   (making it char[] simplifies ugly address
                                            //   arithmetic for indexing variable length rows.)
};

constexpr uint32_t RBBI_LOOKAHEAD_HARD_BREAK = 1;
// No longer used since ICU 79:
// constexpr uint32_t RBBI_BOF_REQUIRED = 2;
constexpr uint32_t RBBI_8_ಬಿಟ್ಟುಗಳು_ಅಡ್ಡಸಾಲುಗಳು = 4;


/*                                        */
/*   The reference counting wrapper class */
/*                                        */
class RBBIDataWrapper : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    ಎಣಿಕೆ EDontAdopt {
        kDontAdopt
    };
    RBBIDataWrapper(ಸ್ಥಿರಾಂಕ RBBIData_ಶೀರ್ಷಿಕೆ *ದತ್ತಾಂಶ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    RBBIDataWrapper(ಸ್ಥಿರಾಂಕ RBBIData_ಶೀರ್ಷಿಕೆ *ದತ್ತಾಂಶ, ಎಣಿಕೆ EDontAdopt dontAdopt, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    RBBIDataWrapper(UData_ಸ್ಮರಣೆ* udm, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    ~RBBIDataWrapper();

    ಸ್ಥಿರ UBool          is_ದತ್ತಾಂಶ_ಆವೃತ್ತಿ_Acceptable(ಸ್ಥಿರಾಂಕ UVersionInfo ಆವೃತ್ತಿ);

    ಶೂನ್ಯ                  init0();
    ಶೂನ್ಯ                  init(ಸ್ಥಿರಾಂಕ RBBIData_ಶೀರ್ಷಿಕೆ *ದತ್ತಾಂಶ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    RBBIDataWrapper      *ಕೂಡಿಸು_Reference();
    ಶೂನ್ಯ                  removeReference();
    bool                  ಕ್ರಿಯಾಕಾರಕ ==(ಸ್ಥಿರಾಂಕ RBBIDataWrapper &other) ಸ್ಥಿರಾಂಕ;
    int32_t               hash_ಸಂಕೇತ();
    ಸ್ಥಿರಾಂಕ UnicodeString  &get_ನಿಯಮ_ಮೂಲ_String() ಸ್ಥಿರಾಂಕ;
    ಶೂನ್ಯ                  print_ದತ್ತಾಂಶ();
    ಶೂನ್ಯ                  print_ಕೋಷ್ಟಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *heading, ಸ್ಥಿರಾಂಕ RBBIState_ಕೋಷ್ಟಕ *ಕೋಷ್ಟಕ);

    /*                                     */
    /*   Pointers to items within the data */
    /*                                     */
    ಸ್ಥಿರಾಂಕ RBBIData_ಶೀರ್ಷಿಕೆ     *f_ಶೀರ್ಷಿಕೆ;
    ಸ್ಥಿರಾಂಕ RBBIState_ಕೋಷ್ಟಕ     *f_Forward_ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ RBBIState_ಕೋಷ್ಟಕ     *f_ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ               *f_ನಿಯಮ_ಮೂಲ;
    ಸ್ಥಿರಾಂಕ int32_t            *f_ನಿಯಮ_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ; 

    /* number of int32_t values in the rule status table.   Used to sanity check indexing */
    int32_t             f_ಸ್ಥಿತಿ_Max_Idx;

    UCPTrie             *fTrie;

ಖಾಸಗಿ:
    u_atomic_int32_t    f_Ref_ಎಣಿಕೆ;
    UData_ಸ್ಮರಣೆ        *fUDataMem;
    UnicodeString       f_ನಿಯಮ_String;
    UBool               f_Dont_Free_ದತ್ತಾಂಶ;

    RBBIDataWrapper(ಸ್ಥಿರಾಂಕ RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
    RBBIDataWrapper &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
};



U_NAMESPACE_ಕೊನೆ

U_CFUNC UBool rbbi_cleanup();

#ಆದರೆ_ಅಂತ್ಯ /* C++ */

#ಆದರೆ_ಅಂತ್ಯ
