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

#నిర్వచితం_కాకపోతే __RBBIDATA_H__
#నిర్వచించు __RBBIDATA_H__

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "udataswp.h"

/**
 * Swap RBBI data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ubrk_మార్పిడి(స్థిరరాశి UDataSwapper *ds,
          స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t నిడివి, శూన్య *బహిర్గతం_దత్తాంశం,
          UError_సంకేతం *p_Error_సంకేతం);

#నిర్వచితమైతే __cplusplus

#అన్తర్భావ "unicode/ucptrie.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/uversion.h"
#అన్తర్భావ "umutex.h"


U_NAMESPACE_BEGIN

// The current RBBI data format version.
స్థిర స్థిరరాశి uint8_t RBBI_దత్తాంశం_FORMAT_సంస్కరణ[] = {7, 0, 0, 0};

/*  
 *   The following structs map exactly onto the raw data from ICU common data file. 
 */
నిర్మాణం RBBIData_శీర్షిక {
    uint32_t         fMagic;           /*  == 0xbla0                                               */
    UVersionInfo     f_Format_సంస్కరణ;   /* Data Format.  Same as the value in struct UDataInfo      */
                                       /*   if there is one associated with this data.             */
                                       /*     (version originates in rbbi, is copied to UDataInfo) */
    uint32_t         f_నిడివి;          /*  Total length in bytes of this RBBI Data,                */
                                       /*      including all sections, not just the header.        */
    uint32_t         f_Cat_లెక్క;        /*  Number of character categories.                         */

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
    uint32_t         f_నియమం_ఆధారం;     /*  Offset to the source for for the break */
    uint32_t         f_నియమం_ఆధారం_Len;  /*    rules.  Stored char16_t *. */
    uint32_t         f_స్థితి_పట్టిక;    /* Offset to the table of rule status values */
    uint32_t         f_స్థితి_పట్టిక_Len;

    uint32_t         fReserved[6];    /*  Reserved for expansion */

};



మూస <typename T>
నిర్మాణం RBBIState_పట్టిక_అడ్డువరుస_T {
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
    T               f_ట్యాగులు_Idx;      //  Non-zero if this row covers a {tagged} position
                                   //    from a rule.  Value is the index in the
                                   //    StatusTable of the set of matching
                                   //    tags (rule status values)
    T               f_తదుపరి_State[1]; //  Next State, indexed by char category.
                                   //    Variable-length array declared with length 1
                                   //    to disable bounds checkers.
                                   //    Array Size is actually fData->fHeader->fCatCount
                                   //    CAUTION:  see RBBITableBuilder::getTableSize()
                                   //              before changing anything here.
};

రకనిర్వచనం RBBIState_పట్టిక_అడ్డువరుస_T<uint8_t> RBBIState_పట్టిక_Row8;
రకనిర్వచనం RBBIState_పట్టిక_అడ్డువరుస_T<uint16_t> RBBIState_పట్టిక_Row16;

constexpr uint16_t ACCEPTING_UNCONDITIONAL = 1;   // Value constant for RBBIStateTableRow::fAccepting

సంఘం RBBIState_పట్టిక_అడ్డువరుస {
  RBBIState_పట్టిక_Row16 r16;
  RBBIState_పట్టిక_Row8 r8;
};

నిర్మాణం RBBIState_పట్టిక {
    uint32_t         f_అంకె_స్థితులు;            // Number of states.
    uint32_t         f_అడ్డువరుస_Len;               // Length of a state table row, in bytes.
    uint32_t         f_Dict_Categories_ప్రారంభం;  // Char category number of the first dictionary
                                            //   char class, or the largest category number + 1
                                            //   if there are no dictionary categories.
    uint32_t         f_Look_Ahead_Results_కొలత; // Size of run-time array required for holding
                                            //   look-ahead results. Indexed by row.fLookAhead.
    uint32_t         fFlags;                // Option Flags for this state table.
    అక్షరం             f_పట్టిక_దత్తాంశం[1];         // First RBBIStateTableRow begins here.
                                            //   Variable-length array declared with length 1
                                            //   to disable bounds checkers.
                                            //   (making it char[] simplifies ugly address
                                            //   arithmetic for indexing variable length rows.)
};

constexpr uint32_t RBBI_LOOKAHEAD_HARD_BREAK = 1;
// No longer used since ICU 79:
// constexpr uint32_t RBBI_BOF_REQUIRED = 2;
constexpr uint32_t RBBI_8_బిట్లు_అడ్డువరుసలు = 4;


/*                                        */
/*   The reference counting wrapper class */
/*                                        */
class RBBIDataWrapper : బహిరంగ UMemory {
బహిరంగ:
    గణన EDontAdopt {
        kDontAdopt
    };
    RBBIDataWrapper(స్థిరరాశి RBBIData_శీర్షిక *దత్తాంశం, UError_సంకేతం &స్థితి_2);
    RBBIDataWrapper(స్థిరరాశి RBBIData_శీర్షిక *దత్తాంశం, గణన EDontAdopt dontAdopt, UError_సంకేతం &స్థితి_2);
    RBBIDataWrapper(UData_జ్ఞాపకం* udm, UError_సంకేతం &స్థితి_2);
    ~RBBIDataWrapper();

    స్థిర UBool          is_దత్తాంశం_సంస్కరణ_Acceptable(స్థిరరాశి UVersionInfo సంస్కరణ);

    శూన్య                  init0();
    శూన్య                  init(స్థిరరాశి RBBIData_శీర్షిక *దత్తాంశం, UError_సంకేతం &స్థితి_2);
    RBBIDataWrapper      *కూడు_Reference();
    శూన్య                  removeReference();
    bool                  క్రియాకారకం ==(స్థిరరాశి RBBIDataWrapper &other) స్థిరరాశి;
    int32_t               hash_సంకేతం();
    స్థిరరాశి UnicodeString  &get_నియమం_ఆధారం_String() స్థిరరాశి;
    శూన్య                  print_దత్తాంశం();
    శూన్య                  print_పట్టిక(స్థిరరాశి అక్షరం *heading, స్థిరరాశి RBBIState_పట్టిక *పట్టిక);

    /*                                     */
    /*   Pointers to items within the data */
    /*                                     */
    స్థిరరాశి RBBIData_శీర్షిక     *f_శీర్షిక;
    స్థిరరాశి RBBIState_పట్టిక     *f_Forward_పట్టిక;
    స్థిరరాశి RBBIState_పట్టిక     *f_తిరగేయి_పట్టిక;
    స్థిరరాశి అక్షరం               *f_నియమం_ఆధారం;
    స్థిరరాశి int32_t            *f_నియమం_స్థితి_పట్టిక; 

    /* number of int32_t values in the rule status table.   Used to sanity check indexing */
    int32_t             f_స్థితి_Max_Idx;

    UCPTrie             *fTrie;

అంతరంగ:
    u_atomic_int32_t    f_Ref_లెక్క;
    UData_జ్ఞాపకం        *fUDataMem;
    UnicodeString       f_నియమం_String;
    UBool               f_Dont_Free_దత్తాంశం;

    RBBIDataWrapper(స్థిరరాశి RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
    RBBIDataWrapper &క్రియాకారకం=(స్థిరరాశి RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
};



U_NAMESPACE_చివర

U_CFUNC UBool rbbi_cleanup();

#అయితే_ముగింపు /* C++ */

#అయితే_ముగింపు
