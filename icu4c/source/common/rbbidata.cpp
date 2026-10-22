// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 1999-2014 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "rbbidata.h"
#ಅನ್ತರ್ಭಾವ "rbbirb.h"
#ಅನ್ತರ್ಭಾವ "udatamem.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"

#ಅನ್ತರ್ಭಾವ "uassert.h"


U_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//    Constructors.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::RBBIDataWrapper(ಸ್ಥಿರಾಂಕ RBBIData_ಶೀರ್ಷಿಕೆ *ದತ್ತಾಂಶ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    init0();
    init(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2);
}

RBBIDataWrapper::RBBIDataWrapper(ಸ್ಥಿರಾಂಕ RBBIData_ಶೀರ್ಷಿಕೆ *ದತ್ತಾಂಶ, ಎಣಿಕೆ EDontAdopt, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    init0();
    init(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2);
    f_Dont_Free_ದತ್ತಾಂಶ = ನಿಜ;
}

RBBIDataWrapper::RBBIDataWrapper(UData_ಸ್ಮರಣೆ* udm, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    init0();
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *dh = udm->p_ಶೀರ್ಷಿಕೆ;
    int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ = dh->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    ಆದರೆ (  !(ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.ದತ್ತಾಂಶ_Format[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.ದತ್ತಾಂಶ_Format[1] == 0x72 &&
            dh->info.ದತ್ತಾಂಶ_Format[2] == 0x6b &&
            dh->info.ದತ್ತಾಂಶ_Format[3] == 0x20 &&
            is_ದತ್ತಾಂಶ_ಆವೃತ್ತಿ_Acceptable(dh->info.format_ಆವೃತ್ತಿ))
        ) {
        ಸ್ಥಿತಿ_2 = U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ_As_ಬೈಟುಗಳು = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *>(dh);
    ಸ್ಥಿರಾಂಕ RBBIData_ಶೀರ್ಷಿಕೆ *rbbidh = reinterpret_cast<ಸ್ಥಿರಾಂಕ RBBIData_ಶೀರ್ಷಿಕೆ *>(ದತ್ತಾಂಶ_As_ಬೈಟುಗಳು + ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);
    init(rbbidh, ಸ್ಥಿತಿ_2);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::is_ದತ್ತಾಂಶ_ಆವೃತ್ತಿ_Acceptable(ಸ್ಥಿರಾಂಕ UVersionInfo ಆವೃತ್ತಿ) {
    ಹಿಂತಿರುಗಿ RBBI_ದತ್ತಾಂಶ_FORMAT_ಆವೃತ್ತಿ[0] == ಆವೃತ್ತಿ[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBIDataWrapper::init0() {
    f_ಶೀರ್ಷಿಕೆ = ಶೂನ್ಯಸೂಚಕ;
    f_Forward_ಕೋಷ್ಟಕ = ಶೂನ್ಯಸೂಚಕ;
    f_ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ = ಶೂನ್ಯಸೂಚಕ;
    f_ನಿಯಮ_ಮೂಲ   = ಶೂನ್ಯಸೂಚಕ;
    f_ನಿಯಮ_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ = ಶೂನ್ಯಸೂಚಕ;
    fTrie         = ಶೂನ್ಯಸೂಚಕ;
    fUDataMem     = ಶೂನ್ಯಸೂಚಕ;
    f_Ref_ಎಣಿಕೆ     = 0;
    f_Dont_Free_ದತ್ತಾಂಶ = ನಿಜ;
}

ಶೂನ್ಯ RBBIDataWrapper::init(ಸ್ಥಿರಾಂಕ RBBIData_ಶೀರ್ಷಿಕೆ *ದತ್ತಾಂಶ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    f_ಶೀರ್ಷಿಕೆ = ದತ್ತಾಂಶ;
    ಆದರೆ (f_ಶೀರ್ಷಿಕೆ->fMagic != 0xb1a0 || !is_ದತ್ತಾಂಶ_ಆವೃತ್ತಿ_Acceptable(f_ಶೀರ್ಷಿಕೆ->f_Format_ಆವೃತ್ತಿ)) {
        ಸ್ಥಿತಿ_2 = U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    f_Dont_Free_ದತ್ತಾಂಶ = ಸುಳ್ಳು;
    ಆದರೆ (ದತ್ತಾಂಶ->fFTableLen != 0) {
        f_Forward_ಕೋಷ್ಟಕ = reinterpret_cast<ಸ್ಥಿರಾಂಕ RBBIState_ಕೋಷ್ಟಕ*>(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ದತ್ತಾಂಶ) + f_ಶೀರ್ಷಿಕೆ->fFTable);
    }
    ಆದರೆ (ದತ್ತಾಂಶ->fRTableLen != 0) {
        f_ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ = reinterpret_cast<ಸ್ಥಿರಾಂಕ RBBIState_ಕೋಷ್ಟಕ*>(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ದತ್ತಾಂಶ) + f_ಶೀರ್ಷಿಕೆ->fRTable);
    }

    fTrie = ucptrie_open_From_ದ್ವಿಮಾನ(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_ANY,
                                   (uint8_t *)ದತ್ತಾಂಶ + f_ಶೀರ್ಷಿಕೆ->fTrie,
                                   f_ಶೀರ್ಷಿಕೆ->fTrieLen,
                                   ಶೂನ್ಯಸೂಚಕ,           // *actual length
                                   &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    UCPTrie_ಮೌಲ್ಯ_ಅಗಲ ಅಗಲ = ucptrie_get_ಮೌಲ್ಯ_ಅಗಲ(fTrie);
    ಆದರೆ (!(ಅಗಲ == UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_8 || ಅಗಲ == UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16)) {
        ಸ್ಥಿತಿ_2 = U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    f_ನಿಯಮ_ಮೂಲ   = ((ಅಕ್ಷರ *)ದತ್ತಾಂಶ + f_ಶೀರ್ಷಿಕೆ->f_ನಿಯಮ_ಮೂಲ);
    f_ನಿಯಮ_String = UnicodeString::fromUTF8(StringPiece(f_ನಿಯಮ_ಮೂಲ, f_ಶೀರ್ಷಿಕೆ->f_ನಿಯಮ_ಮೂಲ_Len));
    U_ASSERT(ದತ್ತಾಂಶ->f_ನಿಯಮ_ಮೂಲ_Len > 0);

    f_ನಿಯಮ_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ = reinterpret_cast<ಸ್ಥಿರಾಂಕ int32_t*>(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ದತ್ತಾಂಶ) + f_ಶೀರ್ಷಿಕೆ->f_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ);
    f_ಸ್ಥಿತಿ_Max_Idx    = ದತ್ತಾಂಶ->f_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ_Len / ಗಾತ್ರ(int32_t);

    f_Ref_ಎಣಿಕೆ = 1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    ಅಕ್ಷರ *debugEnv = getenv("U_RBBIDEBUG");
    ಆದರೆ (debugEnv && uprv_strstr(debugEnv, "data")) {ಇದು->print_ದತ್ತಾಂಶ();}
#ಆದರೆ_ಅಂತ್ಯ
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_ಎಣಿಕೆ == 0);
    ucptrie_close(fTrie);
    fTrie = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (fUDataMem) {
        udata_close(fUDataMem);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!f_Dont_Free_ದತ್ತಾಂಶ) {
        uprv_free((ಶೂನ್ಯ *)f_ಶೀರ್ಷಿಕೆ);
    }
}



//-----------------------------------------------------------------------------
//
//   Operator ==    Consider two RBBIDataWrappers to be equal if they
//                  refer to the same underlying data.  Although
//                  the data wrappers are normally shared between
//                  iterator instances, it's possible to independently
//                  open the same data twice, and get two instances, which
//                  should still be ==.
//
//-----------------------------------------------------------------------------
ತಾರ್ಕಿಕ RBBIDataWrapper::ಕ್ರಿಯಾಕಾರಕ ==(ಸ್ಥಿರಾಂಕ RBBIDataWrapper &other) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (f_ಶೀರ್ಷಿಕೆ == other.f_ಶೀರ್ಷಿಕೆ) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (f_ಶೀರ್ಷಿಕೆ->f_ಉದ್ದಳತೆ != other.f_ಶೀರ್ಷಿಕೆ->f_ಉದ್ದಳತೆ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (uprv_memcmp(f_ಶೀರ್ಷಿಕೆ, other.f_ಶೀರ್ಷಿಕೆ, f_ಶೀರ್ಷಿಕೆ->f_ಉದ್ದಳತೆ) == 0) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

int32_t  RBBIDataWrapper::hash_ಸಂಕೇತ() {
    ಹಿಂತಿರುಗಿ f_ಶೀರ್ಷಿಕೆ->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBIDataWrapper::removeReference() {
    ಆದರೆ (umtx_atomic_dec(&f_Ref_ಎಣಿಕೆ) == 0) {
        ಅಳಿಸು ಇದು;
    }
}


RBBIDataWrapper *RBBIDataWrapper::ಕೂಡಿಸು_Reference() {
   umtx_atomic_inc(&f_Ref_ಎಣಿಕೆ);
   ಹಿಂತಿರುಗಿ ಇದು;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ಸ್ಥಿರಾಂಕ UnicodeString &RBBIDataWrapper::get_ನಿಯಮ_ಮೂಲ_String() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ f_ನಿಯಮ_String;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
ಶೂನ್ಯ  RBBIDataWrapper::print_ಕೋಷ್ಟಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *heading, ಸ್ಥಿರಾಂಕ RBBIState_ಕೋಷ್ಟಕ *ಕೋಷ್ಟಕ) {
    uint32_t   c;
    uint32_t   s;

    RBBIDebugPrintf("%s\n", heading);

    RBBIDebugPrintf("   fDictCategoriesStart: %d\n", ಕೋಷ್ಟಕ->f_Dict_Categories_ಆರಂಭ);
    RBBIDebugPrintf("   fLookAheadResultsSize: %d\n", ಕೋಷ್ಟಕ->f_Look_Ahead_Results_ಗಾತ್ರ);
    RBBIDebugPrintf("   Flags: %4x RBBI_LOOKAHEAD_HARD_BREAK=%s RBBI_8BITS_ROWS=%s\n",
                    ಕೋಷ್ಟಕ->fFlags,
                    ಕೋಷ್ಟಕ->fFlags & RBBI_LOOKAHEAD_HARD_BREAK ? "T" : "F",
                    ಕೋಷ್ಟಕ->fFlags & RBBI_8_ಬಿಟ್ಟುಗಳು_ಅಡ್ಡಸಾಲುಗಳು ? "T" : "F");
    RBBIDebugPrintf("\nState |  Acc  LA TagIx");
    ಗಾಗಿ (c=0; c<f_ಶೀರ್ಷಿಕೆ->f_Cat_ಎಣಿಕೆ; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); ಗಾಗಿ (c=0;c<f_ಶೀರ್ಷಿಕೆ->f_Cat_ಎಣಿಕೆ; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    ಆದರೆ (ಕೋಷ್ಟಕ == ಶೂನ್ಯಸೂಚಕ) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        ಹಿಂತಿರುಗಿ;
    }
    UBool use8_ಬಿಟ್ಟುಗಳು = ಕೋಷ್ಟಕ->fFlags & RBBI_8_ಬಿಟ್ಟುಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
    ಗಾಗಿ (s=0; s<ಕೋಷ್ಟಕ->f_ಅಂಕೆ_ಸ್ಥಿತಿಗಳು; s++) {
        RBBIState_ಕೋಷ್ಟಕ_ಅಡ್ಡಸಾಲು *ಅಡ್ಡಸಾಲು = (RBBIState_ಕೋಷ್ಟಕ_ಅಡ್ಡಸಾಲು *)
                                  (ಕೋಷ್ಟಕ->f_ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ + (ಕೋಷ್ಟಕ->f_ಅಡ್ಡಸಾಲು_Len * s));
        ಆದರೆ (use8_ಬಿಟ್ಟುಗಳು) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, ಅಡ್ಡಸಾಲು->r8.fAccepting, ಅಡ್ಡಸಾಲು->r8.fLookAhead, ಅಡ್ಡಸಾಲು->r8.f_ಟ್ಯಾಗುಗಳು_Idx);
            ಗಾಗಿ (c=0; c<f_ಶೀರ್ಷಿಕೆ->f_Cat_ಎಣಿಕೆ; c++)  {
                RBBIDebugPrintf("%3d ", ಅಡ್ಡಸಾಲು->r8.f_ಮುಂದಿನ_State[c]);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, ಅಡ್ಡಸಾಲು->r16.fAccepting, ಅಡ್ಡಸಾಲು->r16.fLookAhead, ಅಡ್ಡಸಾಲು->r16.f_ಟ್ಯಾಗುಗಳು_Idx);
            ಗಾಗಿ (c=0; c<f_ಶೀರ್ಷಿಕೆ->f_Cat_ಎಣಿಕೆ; c++)  {
                RBBIDebugPrintf("%3d ", ಅಡ್ಡಸಾಲು->r16.f_ಮುಂದಿನ_State[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#ಆದರೆ_ಅಂತ್ಯ


ಶೂನ್ಯ  RBBIDataWrapper::print_ದತ್ತಾಂಶ() {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (ಶೂನ್ಯ *)f_ಶೀರ್ಷಿಕೆ);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", f_ಶೀರ್ಷಿಕೆ->f_Format_ಆವೃತ್ತಿ[0], f_ಶೀರ್ಷಿಕೆ->f_Format_ಆವೃತ್ತಿ[1],
                                                    f_ಶೀರ್ಷಿಕೆ->f_Format_ಆವೃತ್ತಿ[2], f_ಶೀರ್ಷಿಕೆ->f_Format_ಆವೃತ್ತಿ[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", f_ಶೀರ್ಷಿಕೆ->f_ಉದ್ದಳತೆ);
    RBBIDebugPrintf("   number of character categories = %d\n\n", f_ಶೀರ್ಷಿಕೆ->f_Cat_ಎಣಿಕೆ);

    print_ಕೋಷ್ಟಕ("Forward State Transition Table", f_Forward_ಕೋಷ್ಟಕ);
    print_ಕೋಷ್ಟಕ("Reverse State Transition Table", f_ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    ಗಾಗಿ (int32_t c=0; f_ನಿಯಮ_ಮೂಲ[c] != 0; c++) {
        RBBIDebugPrintf("%c", f_ನಿಯಮ_ಮೂಲ[c]);
    }
    RBBIDebugPrintf("\n\n");
#ಆದರೆ_ಅಂತ್ಯ
}


U_NAMESPACE_ಕೊನೆ
U_NAMESPACE_USE

//-----------------------------------------------------------------------------
//
//  ubrk_swap   -  byte swap and char encoding swap of RBBI data
//
//-----------------------------------------------------------------------------

U_CAPI int32_t U_EXPORT2
ubrk_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
           UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {

    ಆದರೆ (ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || length<-1 || (length>0 && ಹೊರಹೋಗು_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ)) {
        *ಸ್ಥಿತಿ_2=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ಸ್ಥಿರಾಂಕ UDataInfo *pInfo = (ಸ್ಥಿರಾಂಕ UDataInfo *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+4);
    ಆದರೆ(!(  pInfo->ದತ್ತಾಂಶ_Format[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->ದತ್ತಾಂಶ_Format[1]==0x72 &&
           pInfo->ದತ್ತಾಂಶ_Format[2]==0x6b &&
           pInfo->ದತ್ತಾಂಶ_Format[3]==0x20 &&
           RBBIDataWrapper::is_ದತ್ತಾಂಶ_ಆವೃತ್ತಿ_Acceptable(pInfo->format_ಆವೃತ್ತಿ) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->ದತ್ತಾಂಶ_Format[0], pInfo->ದತ್ತಾಂಶ_Format[1],
                         pInfo->ದತ್ತಾಂಶ_Format[2], pInfo->ದತ್ತಾಂಶ_Format[3],
                         pInfo->format_ಆವೃತ್ತಿ[0]);
        *ಸ್ಥಿತಿ_2=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    //
    // Swap the data header.  (This is the generic ICU Data Header, not the RBBI Specific
    //                         RBBIDataHeader).  This swap also conveniently gets us
    //                         the size of the ICU d.h., which lets us locate the start
    //                         of the RBBI specific data.
    //
    int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ=udata_ಅದಲುಬದಲು_ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2);


    //
    // Get the RRBI Data Header, and check that it appears to be OK.
    //
    ಸ್ಥಿರಾಂಕ uint8_t  *ಒಳ_ಬೈಟುಗಳು =(ಸ್ಥಿರಾಂಕ uint8_t *)ಒಳ_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    RBBIData_ಶೀರ್ಷಿಕೆ *rbbiDH = (RBBIData_ಶೀರ್ಷಿಕೆ *)ಒಳ_ಬೈಟುಗಳು;
    ಆದರೆ (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::is_ದತ್ತಾಂಶ_ಆವೃತ್ತಿ_Acceptable(rbbiDH->f_Format_ಆವೃತ್ತಿ) ||
            ds->readUInt32(rbbiDH->f_ಉದ್ದಳತೆ)  <  ಗಾತ್ರ(RBBIData_ಶೀರ್ಷಿಕೆ)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *ಸ್ಥಿತಿ_2=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t break_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = ds->readUInt32(rbbiDH->f_ಉದ್ದಳತೆ);
    int32_t ಒಟ್ಟು_ಗಾತ್ರ = ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ + break_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    ಆದರೆ (length < 0) {
        ಹಿಂತಿರುಗಿ ಒಟ್ಟು_ಗಾತ್ರ;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    ಆದರೆ (length < ಒಟ್ಟು_ಗಾತ್ರ) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            break_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);
        *ಸ್ಥಿತಿ_2=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
        }


    //
    // Swap the Data.  Do the data itself first, then the RBBI Data Header, because
    //                 we need to reference the header to locate the data, and an
    //                 inplace swap of the header leaves it unusable.
    //
    uint8_t         *ಹೊರಹೋಗು_ಬೈಟುಗಳು = (uint8_t *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ + ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    RBBIData_ಶೀರ್ಷಿಕೆ  *outputDH = (RBBIData_ಶೀರ್ಷಿಕೆ *)ಹೊರಹೋಗು_ಬೈಟುಗಳು;

    int32_t   ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ;
    int32_t   ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ;

    //
    // If not swapping in place, zero out the output buffer before starting.
    //    Individual tables and other data items within are aligned to 8 byte boundaries
    //    when originally created.  Any unused space between items needs to be zero.
    //
    ಆದರೆ (ಒಳ_ಬೈಟುಗಳು != ಹೊರಹೋಗು_ಬೈಟುಗಳು) {
        uprv_memset(ಹೊರಹೋಗು_ಬೈಟುಗಳು, 0, break_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         ತುದಿ_ಗಾತ್ರ = offsetof(RBBIState_ಕೋಷ್ಟಕ, f_ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ);

    // Forward state table.
    ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ = ds->readUInt32(rbbiDH->fFTable);
    ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ      = ds->readUInt32(rbbiDH->fFTableLen);

    ಆದರೆ (ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ > 0) {
        RBBIState_ಕೋಷ್ಟಕ *rbbiST = (RBBIState_ಕೋಷ್ಟಕ *)(ಒಳ_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ);
        UBool use8_ಬಿಟ್ಟುಗಳು = ds->readUInt32(rbbiST->fFlags) & RBBI_8_ಬಿಟ್ಟುಗಳು_ಅಡ್ಡಸಾಲುಗಳು;

        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ, ತುದಿ_ಗಾತ್ರ,
                            ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ, ಸ್ಥಿತಿ_2);

        // Swap the state table if the table is in 16 bits.
        ಆದರೆ (use8_ಬಿಟ್ಟುಗಳು) {
            ಆದರೆ (ಹೊರಹೋಗು_ಬೈಟುಗಳು != ಒಳ_ಬೈಟುಗಳು) {
                uprv_memmove(ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ+ತುದಿ_ಗಾತ್ರ,
                             ಒಳ_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ+ತುದಿ_ಗಾತ್ರ,
                             ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ-ತುದಿ_ಗಾತ್ರ);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ+ತುದಿ_ಗಾತ್ರ, ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ-ತುದಿ_ಗಾತ್ರ,
                                ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ+ತುದಿ_ಗಾತ್ರ, ಸ್ಥಿತಿ_2);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ = ds->readUInt32(rbbiDH->fRTable);
    ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ      = ds->readUInt32(rbbiDH->fRTableLen);

    ಆದರೆ (ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ > 0) {
        RBBIState_ಕೋಷ್ಟಕ *rbbiST = (RBBIState_ಕೋಷ್ಟಕ *)(ಒಳ_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ);
        UBool use8_ಬಿಟ್ಟುಗಳು = ds->readUInt32(rbbiST->fFlags) & RBBI_8_ಬಿಟ್ಟುಗಳು_ಅಡ್ಡಸಾಲುಗಳು;

        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ, ತುದಿ_ಗಾತ್ರ,
                            ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ, ಸ್ಥಿತಿ_2);

        // Swap the state table if the table is in 16 bits.
        ಆದರೆ (use8_ಬಿಟ್ಟುಗಳು) {
            ಆದರೆ (ಹೊರಹೋಗು_ಬೈಟುಗಳು != ಒಳ_ಬೈಟುಗಳು) {
                uprv_memmove(ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ+ತುದಿ_ಗಾತ್ರ,
                             ಒಳ_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ+ತುದಿ_ಗಾತ್ರ,
                             ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ-ತುದಿ_ಗಾತ್ರ);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ+ತುದಿ_ಗಾತ್ರ, ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ-ತುದಿ_ಗಾತ್ರ,
                                ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಕೋಷ್ಟಕ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ+ತುದಿ_ಗಾತ್ರ, ಸ್ಥಿತಿ_2);
        }
    }

    // Trie table for character categories
    ucptrie_ಅದಲುಬದಲು(ds, ಒಳ_ಬೈಟುಗಳು+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     ಹೊರಹೋಗು_ಬೈಟುಗಳು+ds->readUInt32(rbbiDH->fTrie), ಸ್ಥಿತಿ_2);

    // Source Rules Text.  It's UTF8 data
    ಆದರೆ (ಹೊರಹೋಗು_ಬೈಟುಗಳು != ಒಳ_ಬೈಟುಗಳು) {
        uprv_memmove(ಹೊರಹೋಗು_ಬೈಟುಗಳು+ds->readUInt32(rbbiDH->f_ನಿಯಮ_ಮೂಲ),
                     ಒಳ_ಬೈಟುಗಳು+ds->readUInt32(rbbiDH->f_ನಿಯಮ_ಮೂಲ),
                     ds->readUInt32(rbbiDH->f_ನಿಯಮ_ಮೂಲ_Len));
    }

    // Table of rule status values.  It's all int_32 values
    ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ds->readUInt32(rbbiDH->f_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ), ds->readUInt32(rbbiDH->f_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ_Len),
                        ಹೊರಹೋಗು_ಬೈಟುಗಳು+ds->readUInt32(rbbiDH->f_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ), ಸ್ಥಿತಿ_2);

    // And, last, the header.
    //   It is all int32_t values except for fFormataVersion, which is an array of four bytes.
    //   Swap the whole thing as int32_t, then re-swap the one field.
    //
    ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು, ಗಾತ್ರ(RBBIData_ಶೀರ್ಷಿಕೆ), ಹೊರಹೋಗು_ಬೈಟುಗಳು, ಸ್ಥಿತಿ_2);
    ds->ಅದಲುಬದಲು_Array32(ds, outputDH->f_Format_ಆವೃತ್ತಿ, 4, outputDH->f_Format_ಆವೃತ್ತಿ, ಸ್ಥಿತಿ_2);

    ಹಿಂತಿರುಗಿ ಒಟ್ಟು_ಗಾತ್ರ;
}


#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */
