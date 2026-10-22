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

#यदि_अपरिभाषितम् __RBBIDATA_H__
#परिभाषय __RBBIDATA_H__

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "udataswp.h"

/**
 * Swap RBBI data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ubrk_swap(ध्रुव UDataSwapper *ds,
          ध्रुव शून्य *अन्तर्_दत्त, int32_t दीर्घता, शून्य *बहिर्गमनम्_दत्त,
          UError_संहिता *p_Error_संहिता);

#यदि_परिभाषितम् __cplusplus

#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/uversion.h"
#अन्तर्भाव "umutex.h"


U_NAMESPACE_BEGIN

// The current RBBI data format version.
स्थिर ध्रुव uint8_t RBBI_दत्त_FORMAT_संस्करणम्[] = {7, 0, 0, 0};

/*  
 *   The following structs map exactly onto the raw data from ICU common data file. 
 */
संरचना RBBIData_शीर्षकम् {
    uint32_t         fMagic;           /*  == 0xbla0                                               */
    UVersionInfo     f_Format_संस्करणम्;   /* Data Format.  Same as the value in struct UDataInfo      */
                                       /*   if there is one associated with this data.             */
                                       /*     (version originates in rbbi, is copied to UDataInfo) */
    uint32_t         f_दीर्घता;          /*  Total length in bytes of this RBBI Data,                */
                                       /*      including all sections, not just the header.        */
    uint32_t         f_Cat_गणनम्;        /*  Number of character categories.                         */

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
    uint32_t         f_नियमः_Source;     /*  Offset to the source for for the break */
    uint32_t         f_नियमः_Source_Len;  /*    rules.  Stored char16_t *. */
    uint32_t         f_स्थितिः_सारणी;    /* Offset to the table of rule status values */
    uint32_t         f_स्थितिः_सारणी_Len;

    uint32_t         fReserved[6];    /*  Reserved for expansion */

};



प्रतिरूपम् <typename T>
संरचना RBBIState_सारणी_Row_T {
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
    T               f_चिह्नकानि_Idx;      //  Non-zero if this row covers a {tagged} position
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

प्रकारनिर्देश RBBIState_सारणी_Row_T<uint8_t> RBBIState_सारणी_Row8;
प्रकारनिर्देश RBBIState_सारणी_Row_T<uint16_t> RBBIState_सारणी_Row16;

constexpr uint16_t ACCEPTING_UNCONDITIONAL = 1;   // Value constant for RBBIStateTableRow::fAccepting

सङ्घ RBBIState_सारणी_Row {
  RBBIState_सारणी_Row16 r16;
  RBBIState_सारणी_Row8 r8;
};

संरचना RBBIState_सारणी {
    uint32_t         f_अङ्कः_अवस्थाः;            // Number of states.
    uint32_t         fRowLen;               // Length of a state table row, in bytes.
    uint32_t         fDictCategoriesStart;  // Char category number of the first dictionary
                                            //   char class, or the largest category number + 1
                                            //   if there are no dictionary categories.
    uint32_t         f_Look_Ahead_Results_आकार; // Size of run-time array required for holding
                                            //   look-ahead results. Indexed by row.fLookAhead.
    uint32_t         fFlags;                // Option Flags for this state table.
    वर्ण             f_सारणी_दत्त[1];         // First RBBIStateTableRow begins here.
                                            //   Variable-length array declared with length 1
                                            //   to disable bounds checkers.
                                            //   (making it char[] simplifies ugly address
                                            //   arithmetic for indexing variable length rows.)
};

constexpr uint32_t RBBI_LOOKAHEAD_HARD_BREAK = 1;
// No longer used since ICU 79:
// constexpr uint32_t RBBI_BOF_REQUIRED = 2;
constexpr uint32_t RBBI_8_द्वयङ्काः_पङ्क्तिकाः = 4;


/*                                        */
/*   The reference counting wrapper class */
/*                                        */
class RBBIDataWrapper : सार्वजनिक UMemory {
सार्वजनिक:
    गणना EDontAdopt {
        kDontAdopt
    };
    RBBIDataWrapper(ध्रुव RBBIData_शीर्षकम् *दत्त, UError_संहिता &स्थितिः);
    RBBIDataWrapper(ध्रुव RBBIData_शीर्षकम् *दत्त, गणना EDontAdopt dontAdopt, UError_संहिता &स्थितिः);
    RBBIDataWrapper(UData_स्मृतिः* udm, UError_संहिता &स्थितिः);
    ~RBBIDataWrapper();

    स्थिर UBool          is_दत्त_संस्करणम्_Acceptable(ध्रुव UVersionInfo संस्करणम्);

    शून्य                  init0();
    शून्य                  init(ध्रुव RBBIData_शीर्षकम् *दत्त, UError_संहिता &स्थितिः);
    RBBIDataWrapper      *सङ्कलय_Reference();
    शून्य                  removeReference();
    bool                  संक्रियकः ==(ध्रुव RBBIDataWrapper &other) ध्रुव;
    int32_t               hash_संहिता();
    ध्रुव UnicodeString  &get_नियमः_Source_String() ध्रुव;
    शून्य                  print_दत्त();
    शून्य                  print_सारणी(ध्रुव वर्ण *heading, ध्रुव RBBIState_सारणी *सारणी);

    /*                                     */
    /*   Pointers to items within the data */
    /*                                     */
    ध्रुव RBBIData_शीर्षकम्     *f_शीर्षकम्;
    ध्रुव RBBIState_सारणी     *f_Forward_सारणी;
    ध्रुव RBBIState_सारणी     *f_Reverse_सारणी;
    ध्रुव वर्ण               *f_नियमः_Source;
    ध्रुव int32_t            *f_नियमः_स्थितिः_सारणी; 

    /* number of int32_t values in the rule status table.   Used to sanity check indexing */
    int32_t             f_स्थितिः_Max_Idx;

    UCPTrie             *fTrie;

गुप्त:
    u_atomic_int32_t    f_Ref_गणनम्;
    UData_स्मृतिः        *fUDataMem;
    UnicodeString       f_नियमः_String;
    UBool               f_Dont_Free_दत्त;

    RBBIDataWrapper(ध्रुव RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
    RBBIDataWrapper &संक्रियकः=(ध्रुव RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
};



U_NAMESPACE_अन्तः

U_CFUNC UBool rbbi_cleanup();

#यदि_अन्तः /* C++ */

#यदि_अन्तः
