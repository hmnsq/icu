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

#اگر_تعریف_نشده __RBBIDATA_H__
#تعریف_کن __RBBIDATA_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/udata.h"
#ضمن "udataswp.h"

/**
 * Swap RBBI data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ubrk_جابجا(ثابت UDataSwapper *ds,
          ثابت فراغ *درون_داده, int32_t درازی, فراغ *بیرون_داده,
          UError_کد *p_Error_کد);

#اگر_تعریف_شده __cplusplus

#ضمن "unicode/ucptrie.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/uversion.h"
#ضمن "umutex.h"


U_NAMESPACE_BEGIN

// The current RBBI data format version.
ساكن ثابت uint8_t RBBI_داده_FORMAT_نسخه[] = {7, 0, 0, 0};

/*  
 *   The following structs map exactly onto the raw data from ICU common data file. 
 */
بنية RBBIData_سرآیند {
    uint32_t         fMagic;           /*  == 0xbla0                                               */
    UVersionInfo     f_Format_نسخه;   /* Data Format.  Same as the value in struct UDataInfo      */
                                       /*   if there is one associated with this data.             */
                                       /*     (version originates in rbbi, is copied to UDataInfo) */
    uint32_t         f_درازی;          /*  Total length in bytes of this RBBI Data,                */
                                       /*      including all sections, not just the header.        */
    uint32_t         f_Cat_شمار;        /*  Number of character categories.                         */

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
    uint32_t         f_قاعده_منبع;     /*  Offset to the source for for the break */
    uint32_t         f_قاعده_منبع_Len;  /*    rules.  Stored char16_t *. */
    uint32_t         f_وضعیت_جدول;    /* Offset to the table of rule status values */
    uint32_t         f_وضعیت_جدول_Len;

    uint32_t         fReserved[6];    /*  Reserved for expansion */

};



قالب <typename T>
بنية RBBIState_جدول_ردیف_T {
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
    T               f_برچسبها_Idx;      //  Non-zero if this row covers a {tagged} position
                                   //    from a rule.  Value is the index in the
                                   //    StatusTable of the set of matching
                                   //    tags (rule status values)
    T               f_بعدی_State[1]; //  Next State, indexed by char category.
                                   //    Variable-length array declared with length 1
                                   //    to disable bounds checkers.
                                   //    Array Size is actually fData->fHeader->fCatCount
                                   //    CAUTION:  see RBBITableBuilder::getTableSize()
                                   //              before changing anything here.
};

تعريفنوع RBBIState_جدول_ردیف_T<uint8_t> RBBIState_جدول_Row8;
تعريفنوع RBBIState_جدول_ردیف_T<uint16_t> RBBIState_جدول_Row16;

constexpr uint16_t ACCEPTING_UNCONDITIONAL = 1;   // Value constant for RBBIStateTableRow::fAccepting

اتحاد RBBIState_جدول_ردیف {
  RBBIState_جدول_Row16 r16;
  RBBIState_جدول_Row8 r8;
};

بنية RBBIState_جدول {
    uint32_t         f_رقم_حالتها;            // Number of states.
    uint32_t         f_ردیف_Len;               // Length of a state table row, in bytes.
    uint32_t         f_Dict_Categories_شروع;  // Char category number of the first dictionary
                                            //   char class, or the largest category number + 1
                                            //   if there are no dictionary categories.
    uint32_t         f_Look_Ahead_Results_اندازه; // Size of run-time array required for holding
                                            //   look-ahead results. Indexed by row.fLookAhead.
    uint32_t         fFlags;                // Option Flags for this state table.
    حرف             f_جدول_داده[1];         // First RBBIStateTableRow begins here.
                                            //   Variable-length array declared with length 1
                                            //   to disable bounds checkers.
                                            //   (making it char[] simplifies ugly address
                                            //   arithmetic for indexing variable length rows.)
};

constexpr uint32_t RBBI_LOOKAHEAD_HARD_BREAK = 1;
// No longer used since ICU 79:
// constexpr uint32_t RBBI_BOF_REQUIRED = 2;
constexpr uint32_t RBBI_8_بیتها_ردیفها = 4;


/*                                        */
/*   The reference counting wrapper class */
/*                                        */
class RBBIDataWrapper : عام UMemory {
عام:
    تعداد EDontAdopt {
        kDontAdopt
    };
    RBBIDataWrapper(ثابت RBBIData_سرآیند *داده, UError_کد &وضعیت);
    RBBIDataWrapper(ثابت RBBIData_سرآیند *داده, تعداد EDontAdopt dontAdopt, UError_کد &وضعیت);
    RBBIDataWrapper(UData_حافظه* udm, UError_کد &وضعیت);
    ~RBBIDataWrapper();

    ساكن UBool          is_داده_نسخه_Acceptable(ثابت UVersionInfo نسخه);

    فراغ                  init0();
    فراغ                  init(ثابت RBBIData_سرآیند *داده, UError_کد &وضعیت);
    RBBIDataWrapper      *جمع_کن_Reference();
    فراغ                  removeReference();
    bool                  عملگر ==(ثابت RBBIDataWrapper &other) ثابت;
    int32_t               hash_کد();
    ثابت UnicodeString  &get_قاعده_منبع_String() ثابت;
    فراغ                  print_داده();
    فراغ                  print_جدول(ثابت حرف *heading, ثابت RBBIState_جدول *جدول);

    /*                                     */
    /*   Pointers to items within the data */
    /*                                     */
    ثابت RBBIData_سرآیند     *f_سرآیند;
    ثابت RBBIState_جدول     *f_Forward_جدول;
    ثابت RBBIState_جدول     *f_معکوس_جدول;
    ثابت حرف               *f_قاعده_منبع;
    ثابت int32_t            *f_قاعده_وضعیت_جدول; 

    /* number of int32_t values in the rule status table.   Used to sanity check indexing */
    int32_t             f_وضعیت_Max_Idx;

    UCPTrie             *fTrie;

خاص:
    u_atomic_int32_t    f_Ref_شمار;
    UData_حافظه        *fUDataMem;
    UnicodeString       f_قاعده_String;
    UBool               f_Dont_Free_داده;

    RBBIDataWrapper(ثابت RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
    RBBIDataWrapper &عملگر=(ثابت RBBIDataWrapper &other) = delete; /*  forbid copying of this class */
};



U_NAMESPACE_انتها

U_CFUNC UBool rbbi_cleanup();

#پایان_اگر /* C++ */

#پایان_اگر
