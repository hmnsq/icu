// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 1999-2014 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbidata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbirb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udatamem.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"


U_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//    Constructors.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::RBBIDataWrapper(ਅਚਲ RBBIData_ਸਿਰਲੇਖ *ਅੰਕੜੇ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    init0();
    init(ਅੰਕੜੇ, ਸਥਿਤੀ);
}

RBBIDataWrapper::RBBIDataWrapper(ਅਚਲ RBBIData_ਸਿਰਲੇਖ *ਅੰਕੜੇ, ਗਿਣਤੀ EDontAdopt, UError_ਕੋਡ &ਸਥਿਤੀ) {
    init0();
    init(ਅੰਕੜੇ, ਸਥਿਤੀ);
    f_Dont_Free_ਅੰਕੜੇ = ਸੱਚ;
}

RBBIDataWrapper::RBBIDataWrapper(UData_ਸਮ੍ਰਿਤੀ* udm, UError_ਕੋਡ &ਸਥਿਤੀ) {
    init0();
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *dh = udm->p_ਸਿਰਲੇਖ;
    int32_t ਸਿਰਲੇਖ_ਮਾਪ = dh->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ;
    ਜੇ (  !(ਸਿਰਲੇਖ_ਮਾਪ >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.ਅੰਕੜੇ_Format[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.ਅੰਕੜੇ_Format[1] == 0x72 &&
            dh->info.ਅੰਕੜੇ_Format[2] == 0x6b &&
            dh->info.ਅੰਕੜੇ_Format[3] == 0x20 &&
            is_ਅੰਕੜੇ_ਸੰਸਕਰਣ_Acceptable(dh->info.format_ਸੰਸਕਰਣ))
        ) {
        ਸਥਿਤੀ = U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ;
    }
    ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ_As_ਬਾਈਟਾਂ = reinterpret_cast<ਅਚਲ ਅੱਖਰ *>(dh);
    ਅਚਲ RBBIData_ਸਿਰਲੇਖ *rbbidh = reinterpret_cast<ਅਚਲ RBBIData_ਸਿਰਲੇਖ *>(ਅੰਕੜੇ_As_ਬਾਈਟਾਂ + ਸਿਰਲੇਖ_ਮਾਪ);
    init(rbbidh, ਸਥਿਤੀ);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::is_ਅੰਕੜੇ_ਸੰਸਕਰਣ_Acceptable(ਅਚਲ UVersionInfo ਸੰਸਕਰਣ) {
    ਵਾਪਸ RBBI_ਅੰਕੜੇ_FORMAT_ਸੰਸਕਰਣ[0] == ਸੰਸਕਰਣ[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
ਖਾਲੀ RBBIDataWrapper::init0() {
    f_ਸਿਰਲੇਖ = ਖਾਲੀ_ਸੂਚਕ;
    f_Forward_ਸਾਰਣੀ = ਖਾਲੀ_ਸੂਚਕ;
    f_ਉਲਟ_ਸਾਰਣੀ = ਖਾਲੀ_ਸੂਚਕ;
    f_ਨਿਯਮ_ਸਰੋਤ   = ਖਾਲੀ_ਸੂਚਕ;
    f_ਨਿਯਮ_ਸਥਿਤੀ_ਸਾਰਣੀ = ਖਾਲੀ_ਸੂਚਕ;
    fTrie         = ਖਾਲੀ_ਸੂਚਕ;
    fUDataMem     = ਖਾਲੀ_ਸੂਚਕ;
    f_Ref_ਗਿਣਤੀ     = 0;
    f_Dont_Free_ਅੰਕੜੇ = ਸੱਚ;
}

ਖਾਲੀ RBBIDataWrapper::init(ਅਚਲ RBBIData_ਸਿਰਲੇਖ *ਅੰਕੜੇ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    f_ਸਿਰਲੇਖ = ਅੰਕੜੇ;
    ਜੇ (f_ਸਿਰਲੇਖ->fMagic != 0xb1a0 || !is_ਅੰਕੜੇ_ਸੰਸਕਰਣ_Acceptable(f_ਸਿਰਲੇਖ->f_Format_ਸੰਸਕਰਣ)) {
        ਸਥਿਤੀ = U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    f_Dont_Free_ਅੰਕੜੇ = ਝੂਠ;
    ਜੇ (ਅੰਕੜੇ->fFTableLen != 0) {
        f_Forward_ਸਾਰਣੀ = reinterpret_cast<ਅਚਲ RBBIState_ਸਾਰਣੀ*>(reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਅੰਕੜੇ) + f_ਸਿਰਲੇਖ->fFTable);
    }
    ਜੇ (ਅੰਕੜੇ->fRTableLen != 0) {
        f_ਉਲਟ_ਸਾਰਣੀ = reinterpret_cast<ਅਚਲ RBBIState_ਸਾਰਣੀ*>(reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਅੰਕੜੇ) + f_ਸਿਰਲੇਖ->fRTable);
    }

    fTrie = ucptrie_open_From_ਦੁਆਧਾਰੀ(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_ANY,
                                   (uint8_t *)ਅੰਕੜੇ + f_ਸਿਰਲੇਖ->fTrie,
                                   f_ਸਿਰਲੇਖ->fTrieLen,
                                   ਖਾਲੀ_ਸੂਚਕ,           // *actual length
                                   &ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }

    UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ ਚੌੜਾਈ = ucptrie_get_ਮੁੱਲ_ਚੌੜਾਈ(fTrie);
    ਜੇ (!(ਚੌੜਾਈ == UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_8 || ਚੌੜਾਈ == UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16)) {
        ਸਥਿਤੀ = U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ;
    }

    f_ਨਿਯਮ_ਸਰੋਤ   = ((ਅੱਖਰ *)ਅੰਕੜੇ + f_ਸਿਰਲੇਖ->f_ਨਿਯਮ_ਸਰੋਤ);
    f_ਨਿਯਮ_String = UnicodeString::fromUTF8(StringPiece(f_ਨਿਯਮ_ਸਰੋਤ, f_ਸਿਰਲੇਖ->f_ਨਿਯਮ_ਸਰੋਤ_Len));
    U_ASSERT(ਅੰਕੜੇ->f_ਨਿਯਮ_ਸਰੋਤ_Len > 0);

    f_ਨਿਯਮ_ਸਥਿਤੀ_ਸਾਰਣੀ = reinterpret_cast<ਅਚਲ int32_t*>(reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਅੰਕੜੇ) + f_ਸਿਰਲੇਖ->f_ਸਥਿਤੀ_ਸਾਰਣੀ);
    f_ਸਥਿਤੀ_Max_Idx    = ਅੰਕੜੇ->f_ਸਥਿਤੀ_ਸਾਰਣੀ_Len / ਆਕਾਰ(int32_t);

    f_Ref_ਗਿਣਤੀ = 1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    ਅੱਖਰ *debugEnv = getenv("U_RBBIDEBUG");
    ਜੇ (debugEnv && uprv_strstr(debugEnv, "data")) {ਇਹ->print_ਅੰਕੜੇ();}
#ਜੇ_ਅੰਤ
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_ਗਿਣਤੀ == 0);
    ucptrie_close(fTrie);
    fTrie = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (fUDataMem) {
        udata_close(fUDataMem);
    } ਨਹੀਂਤਾਂ ਜੇ (!f_Dont_Free_ਅੰਕੜੇ) {
        uprv_free((ਖਾਲੀ *)f_ਸਿਰਲੇਖ);
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
ਤਾਰਕਿਕ RBBIDataWrapper::ਸੰਚਾਲਕ ==(ਅਚਲ RBBIDataWrapper &other) ਅਚਲ {
    ਜੇ (f_ਸਿਰਲੇਖ == other.f_ਸਿਰਲੇਖ) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (f_ਸਿਰਲੇਖ->f_ਲੰਮਾਈ != other.f_ਸਿਰਲੇਖ->f_ਲੰਮਾਈ) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (uprv_memcmp(f_ਸਿਰਲੇਖ, other.f_ਸਿਰਲੇਖ, f_ਸਿਰਲੇਖ->f_ਲੰਮਾਈ) == 0) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਵਾਪਸ ਝੂਠ;
}

int32_t  RBBIDataWrapper::hash_ਕੋਡ() {
    ਵਾਪਸ f_ਸਿਰਲੇਖ->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
ਖਾਲੀ RBBIDataWrapper::removeReference() {
    ਜੇ (umtx_atomic_dec(&f_Ref_ਗਿਣਤੀ) == 0) {
        ਮਿਟਾਓ ਇਹ;
    }
}


RBBIDataWrapper *RBBIDataWrapper::ਜੋੜੋ_Reference() {
   umtx_atomic_inc(&f_Ref_ਗਿਣਤੀ);
   ਵਾਪਸ ਇਹ;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ਅਚਲ UnicodeString &RBBIDataWrapper::get_ਨਿਯਮ_ਸਰੋਤ_String() ਅਚਲ {
    ਵਾਪਸ f_ਨਿਯਮ_String;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
ਖਾਲੀ  RBBIDataWrapper::print_ਸਾਰਣੀ(ਅਚਲ ਅੱਖਰ *heading, ਅਚਲ RBBIState_ਸਾਰਣੀ *ਸਾਰਣੀ) {
    uint32_t   c;
    uint32_t   s;

    RBBIDebugPrintf("%s\n", heading);

    RBBIDebugPrintf("   fDictCategoriesStart: %d\n", ਸਾਰਣੀ->f_Dict_Categories_ਸ਼ੁਰੂ);
    RBBIDebugPrintf("   fLookAheadResultsSize: %d\n", ਸਾਰਣੀ->f_Look_Ahead_Results_ਮਾਪ);
    RBBIDebugPrintf("   Flags: %4x RBBI_LOOKAHEAD_HARD_BREAK=%s RBBI_8BITS_ROWS=%s\n",
                    ਸਾਰਣੀ->fFlags,
                    ਸਾਰਣੀ->fFlags & RBBI_LOOKAHEAD_HARD_BREAK ? "T" : "F",
                    ਸਾਰਣੀ->fFlags & RBBI_8_ਬਿੱਟ_ਪੰਕਤੀਆਂ ? "T" : "F");
    RBBIDebugPrintf("\nState |  Acc  LA TagIx");
    ਲਈ (c=0; c<f_ਸਿਰਲੇਖ->f_Cat_ਗਿਣਤੀ; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); ਲਈ (c=0;c<f_ਸਿਰਲੇਖ->f_Cat_ਗਿਣਤੀ; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    ਜੇ (ਸਾਰਣੀ == ਖਾਲੀ_ਸੂਚਕ) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        ਵਾਪਸ;
    }
    UBool use8_ਬਿੱਟ = ਸਾਰਣੀ->fFlags & RBBI_8_ਬਿੱਟ_ਪੰਕਤੀਆਂ;
    ਲਈ (s=0; s<ਸਾਰਣੀ->f_ਅੰਕ_ਹਾਲਤਾਂ; s++) {
        RBBIState_ਸਾਰਣੀ_ਪੰਕਤੀ *ਪੰਕਤੀ = (RBBIState_ਸਾਰਣੀ_ਪੰਕਤੀ *)
                                  (ਸਾਰਣੀ->f_ਸਾਰਣੀ_ਅੰਕੜੇ + (ਸਾਰਣੀ->f_ਪੰਕਤੀ_Len * s));
        ਜੇ (use8_ਬਿੱਟ) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, ਪੰਕਤੀ->r8.fAccepting, ਪੰਕਤੀ->r8.fLookAhead, ਪੰਕਤੀ->r8.f_ਟੈਗ_ਸੂਚੀ_Idx);
            ਲਈ (c=0; c<f_ਸਿਰਲੇਖ->f_Cat_ਗਿਣਤੀ; c++)  {
                RBBIDebugPrintf("%3d ", ਪੰਕਤੀ->r8.f_ਅਗਲਾ_State[c]);
            }
        } ਨਹੀਂਤਾਂ {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, ਪੰਕਤੀ->r16.fAccepting, ਪੰਕਤੀ->r16.fLookAhead, ਪੰਕਤੀ->r16.f_ਟੈਗ_ਸੂਚੀ_Idx);
            ਲਈ (c=0; c<f_ਸਿਰਲੇਖ->f_Cat_ਗਿਣਤੀ; c++)  {
                RBBIDebugPrintf("%3d ", ਪੰਕਤੀ->r16.f_ਅਗਲਾ_State[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#ਜੇ_ਅੰਤ


ਖਾਲੀ  RBBIDataWrapper::print_ਅੰਕੜੇ() {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (ਖਾਲੀ *)f_ਸਿਰਲੇਖ);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", f_ਸਿਰਲੇਖ->f_Format_ਸੰਸਕਰਣ[0], f_ਸਿਰਲੇਖ->f_Format_ਸੰਸਕਰਣ[1],
                                                    f_ਸਿਰਲੇਖ->f_Format_ਸੰਸਕਰਣ[2], f_ਸਿਰਲੇਖ->f_Format_ਸੰਸਕਰਣ[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", f_ਸਿਰਲੇਖ->f_ਲੰਮਾਈ);
    RBBIDebugPrintf("   number of character categories = %d\n\n", f_ਸਿਰਲੇਖ->f_Cat_ਗਿਣਤੀ);

    print_ਸਾਰਣੀ("Forward State Transition Table", f_Forward_ਸਾਰਣੀ);
    print_ਸਾਰਣੀ("Reverse State Transition Table", f_ਉਲਟ_ਸਾਰਣੀ);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    ਲਈ (int32_t c=0; f_ਨਿਯਮ_ਸਰੋਤ[c] != 0; c++) {
        RBBIDebugPrintf("%c", f_ਨਿਯਮ_ਸਰੋਤ[c]);
    }
    RBBIDebugPrintf("\n\n");
#ਜੇ_ਅੰਤ
}


U_NAMESPACE_ਸਿਰਾ
U_NAMESPACE_USE

//-----------------------------------------------------------------------------
//
//  ubrk_swap   -  byte swap and char encoding swap of RBBI data
//
//-----------------------------------------------------------------------------

U_CAPI int32_t U_EXPORT2
ubrk_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds, ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
           UError_ਕੋਡ *ਸਥਿਤੀ) {

    ਜੇ (ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || length<-1 || (length>0 && ਬਾਹਰਲਾ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ)) {
        *ਸਥਿਤੀ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ਅਚਲ UDataInfo *pInfo = (ਅਚਲ UDataInfo *)((ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+4);
    ਜੇ(!(  pInfo->ਅੰਕੜੇ_Format[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->ਅੰਕੜੇ_Format[1]==0x72 &&
           pInfo->ਅੰਕੜੇ_Format[2]==0x6b &&
           pInfo->ਅੰਕੜੇ_Format[3]==0x20 &&
           RBBIDataWrapper::is_ਅੰਕੜੇ_ਸੰਸਕਰਣ_Acceptable(pInfo->format_ਸੰਸਕਰਣ) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->ਅੰਕੜੇ_Format[0], pInfo->ਅੰਕੜੇ_Format[1],
                         pInfo->ਅੰਕੜੇ_Format[2], pInfo->ਅੰਕੜੇ_Format[3],
                         pInfo->format_ਸੰਸਕਰਣ[0]);
        *ਸਥਿਤੀ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    //
    // Swap the data header.  (This is the generic ICU Data Header, not the RBBI Specific
    //                         RBBIDataHeader).  This swap also conveniently gets us
    //                         the size of the ICU d.h., which lets us locate the start
    //                         of the RBBI specific data.
    //
    int32_t ਸਿਰਲੇਖ_ਮਾਪ=udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, ਸਥਿਤੀ);


    //
    // Get the RRBI Data Header, and check that it appears to be OK.
    //
    ਅਚਲ uint8_t  *ਵਿੱਚ_ਬਾਈਟਾਂ =(ਅਚਲ uint8_t *)ਵਿੱਚ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ;
    RBBIData_ਸਿਰਲੇਖ *rbbiDH = (RBBIData_ਸਿਰਲੇਖ *)ਵਿੱਚ_ਬਾਈਟਾਂ;
    ਜੇ (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::is_ਅੰਕੜੇ_ਸੰਸਕਰਣ_Acceptable(rbbiDH->f_Format_ਸੰਸਕਰਣ) ||
            ds->readUInt32(rbbiDH->f_ਲੰਮਾਈ)  <  ਆਕਾਰ(RBBIData_ਸਿਰਲੇਖ)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *ਸਥਿਤੀ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t break_ਅੰਕੜੇ_ਲੰਮਾਈ = ds->readUInt32(rbbiDH->f_ਲੰਮਾਈ);
    int32_t ਕੁੱਲ_ਮਾਪ = ਸਿਰਲੇਖ_ਮਾਪ + break_ਅੰਕੜੇ_ਲੰਮਾਈ;
    ਜੇ (length < 0) {
        ਵਾਪਸ ਕੁੱਲ_ਮਾਪ;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    ਜੇ (length < ਕੁੱਲ_ਮਾਪ) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            break_ਅੰਕੜੇ_ਲੰਮਾਈ);
        *ਸਥਿਤੀ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
        }


    //
    // Swap the Data.  Do the data itself first, then the RBBI Data Header, because
    //                 we need to reference the header to locate the data, and an
    //                 inplace swap of the header leaves it unusable.
    //
    uint8_t         *ਬਾਹਰਲਾ_ਬਾਈਟਾਂ = (uint8_t *)ਬਾਹਰਲਾ_ਅੰਕੜੇ + ਸਿਰਲੇਖ_ਮਾਪ;
    RBBIData_ਸਿਰਲੇਖ  *outputDH = (RBBIData_ਸਿਰਲੇਖ *)ਬਾਹਰਲਾ_ਬਾਈਟਾਂ;

    int32_t   ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ;
    int32_t   ਸਾਰਣੀ_ਲੰਮਾਈ;

    //
    // If not swapping in place, zero out the output buffer before starting.
    //    Individual tables and other data items within are aligned to 8 byte boundaries
    //    when originally created.  Any unused space between items needs to be zero.
    //
    ਜੇ (ਵਿੱਚ_ਬਾਈਟਾਂ != ਬਾਹਰਲਾ_ਬਾਈਟਾਂ) {
        uprv_memset(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, 0, break_ਅੰਕੜੇ_ਲੰਮਾਈ);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         ਸਿਖਰ_ਮਾਪ = offsetof(RBBIState_ਸਾਰਣੀ, f_ਸਾਰਣੀ_ਅੰਕੜੇ);

    // Forward state table.
    ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ = ds->readUInt32(rbbiDH->fFTable);
    ਸਾਰਣੀ_ਲੰਮਾਈ      = ds->readUInt32(rbbiDH->fFTableLen);

    ਜੇ (ਸਾਰਣੀ_ਲੰਮਾਈ > 0) {
        RBBIState_ਸਾਰਣੀ *rbbiST = (RBBIState_ਸਾਰਣੀ *)(ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ);
        UBool use8_ਬਿੱਟ = ds->readUInt32(rbbiST->fFlags) & RBBI_8_ਬਿੱਟ_ਪੰਕਤੀਆਂ;

        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ, ਸਿਖਰ_ਮਾਪ,
                            ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ, ਸਥਿਤੀ);

        // Swap the state table if the table is in 16 bits.
        ਜੇ (use8_ਬਿੱਟ) {
            ਜੇ (ਬਾਹਰਲਾ_ਬਾਈਟਾਂ != ਵਿੱਚ_ਬਾਈਟਾਂ) {
                uprv_memmove(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ+ਸਿਖਰ_ਮਾਪ,
                             ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ+ਸਿਖਰ_ਮਾਪ,
                             ਸਾਰਣੀ_ਲੰਮਾਈ-ਸਿਖਰ_ਮਾਪ);
            }
        } ਨਹੀਂਤਾਂ {
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ+ਸਿਖਰ_ਮਾਪ, ਸਾਰਣੀ_ਲੰਮਾਈ-ਸਿਖਰ_ਮਾਪ,
                                ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ+ਸਿਖਰ_ਮਾਪ, ਸਥਿਤੀ);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ = ds->readUInt32(rbbiDH->fRTable);
    ਸਾਰਣੀ_ਲੰਮਾਈ      = ds->readUInt32(rbbiDH->fRTableLen);

    ਜੇ (ਸਾਰਣੀ_ਲੰਮਾਈ > 0) {
        RBBIState_ਸਾਰਣੀ *rbbiST = (RBBIState_ਸਾਰਣੀ *)(ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ);
        UBool use8_ਬਿੱਟ = ds->readUInt32(rbbiST->fFlags) & RBBI_8_ਬਿੱਟ_ਪੰਕਤੀਆਂ;

        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ, ਸਿਖਰ_ਮਾਪ,
                            ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ, ਸਥਿਤੀ);

        // Swap the state table if the table is in 16 bits.
        ਜੇ (use8_ਬਿੱਟ) {
            ਜੇ (ਬਾਹਰਲਾ_ਬਾਈਟਾਂ != ਵਿੱਚ_ਬਾਈਟਾਂ) {
                uprv_memmove(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ+ਸਿਖਰ_ਮਾਪ,
                             ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ+ਸਿਖਰ_ਮਾਪ,
                             ਸਾਰਣੀ_ਲੰਮਾਈ-ਸਿਖਰ_ਮਾਪ);
            }
        } ਨਹੀਂਤਾਂ {
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ+ਸਿਖਰ_ਮਾਪ, ਸਾਰਣੀ_ਲੰਮਾਈ-ਸਿਖਰ_ਮਾਪ,
                                ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਵਿੱਥ+ਸਿਖਰ_ਮਾਪ, ਸਥਿਤੀ);
        }
    }

    // Trie table for character categories
    ucptrie_ਅਦਲਾ_ਬਦਲੀ(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ds->readUInt32(rbbiDH->fTrie), ਸਥਿਤੀ);

    // Source Rules Text.  It's UTF8 data
    ਜੇ (ਬਾਹਰਲਾ_ਬਾਈਟਾਂ != ਵਿੱਚ_ਬਾਈਟਾਂ) {
        uprv_memmove(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ds->readUInt32(rbbiDH->f_ਨਿਯਮ_ਸਰੋਤ),
                     ਵਿੱਚ_ਬਾਈਟਾਂ+ds->readUInt32(rbbiDH->f_ਨਿਯਮ_ਸਰੋਤ),
                     ds->readUInt32(rbbiDH->f_ਨਿਯਮ_ਸਰੋਤ_Len));
    }

    // Table of rule status values.  It's all int_32 values
    ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ds->readUInt32(rbbiDH->f_ਸਥਿਤੀ_ਸਾਰਣੀ), ds->readUInt32(rbbiDH->f_ਸਥਿਤੀ_ਸਾਰਣੀ_Len),
                        ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ds->readUInt32(rbbiDH->f_ਸਥਿਤੀ_ਸਾਰਣੀ), ਸਥਿਤੀ);

    // And, last, the header.
    //   It is all int32_t values except for fFormataVersion, which is an array of four bytes.
    //   Swap the whole thing as int32_t, then re-swap the one field.
    //
    ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ, ਆਕਾਰ(RBBIData_ਸਿਰਲੇਖ), ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, ਸਥਿਤੀ);
    ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, outputDH->f_Format_ਸੰਸਕਰਣ, 4, outputDH->f_Format_ਸੰਸਕਰਣ, ਸਥਿਤੀ);

    ਵਾਪਸ ਕੁੱਲ_ਮਾਪ;
}


#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */
