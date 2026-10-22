// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 1999-2014 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/ucptrie.h"
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "rbbidata.h"
#అన్తర్భావ "rbbirb.h"
#అన్తర్భావ "udatamem.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "umutex.h"

#అన్తర్భావ "uassert.h"


U_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//    Constructors.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::RBBIDataWrapper(స్థిరరాశి RBBIData_శీర్షిక *దత్తాంశం, UError_సంకేతం &స్థితి_2) {
    init0();
    init(దత్తాంశం, స్థితి_2);
}

RBBIDataWrapper::RBBIDataWrapper(స్థిరరాశి RBBIData_శీర్షిక *దత్తాంశం, గణన EDontAdopt, UError_సంకేతం &స్థితి_2) {
    init0();
    init(దత్తాంశం, స్థితి_2);
    f_Dont_Free_దత్తాంశం = నిజం;
}

RBBIDataWrapper::RBBIDataWrapper(UData_జ్ఞాపకం* udm, UError_సంకేతం &స్థితి_2) {
    init0();
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    స్థిరరాశి దత్తాంశం_శీర్షిక *dh = udm->p_శీర్షిక;
    int32_t శీర్షిక_కొలత = dh->దత్తాంశం_శీర్షిక_2.శీర్షిక_కొలత;
    ఒకవేళ (  !(శీర్షిక_కొలత >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.దత్తాంశం_Format[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.దత్తాంశం_Format[1] == 0x72 &&
            dh->info.దత్తాంశం_Format[2] == 0x6b &&
            dh->info.దత్తాంశం_Format[3] == 0x20 &&
            is_దత్తాంశం_సంస్కరణ_Acceptable(dh->info.format_సంస్కరణ))
        ) {
        స్థితి_2 = U_INVALID_FORMAT_ERROR;
        తిరిగి;
    }
    స్థిరరాశి అక్షరం *దత్తాంశం_As_బైట్లు = reinterpret_cast<స్థిరరాశి అక్షరం *>(dh);
    స్థిరరాశి RBBIData_శీర్షిక *rbbidh = reinterpret_cast<స్థిరరాశి RBBIData_శీర్షిక *>(దత్తాంశం_As_బైట్లు + శీర్షిక_కొలత);
    init(rbbidh, స్థితి_2);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::is_దత్తాంశం_సంస్కరణ_Acceptable(స్థిరరాశి UVersionInfo సంస్కరణ) {
    తిరిగి RBBI_దత్తాంశం_FORMAT_సంస్కరణ[0] == సంస్కరణ[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
శూన్య RBBIDataWrapper::init0() {
    f_శీర్షిక = శూన్యసూచిక;
    f_Forward_పట్టిక = శూన్యసూచిక;
    f_తిరగేయి_పట్టిక = శూన్యసూచిక;
    f_నియమం_ఆధారం   = శూన్యసూచిక;
    f_నియమం_స్థితి_పట్టిక = శూన్యసూచిక;
    fTrie         = శూన్యసూచిక;
    fUDataMem     = శూన్యసూచిక;
    f_Ref_లెక్క     = 0;
    f_Dont_Free_దత్తాంశం = నిజం;
}

శూన్య RBBIDataWrapper::init(స్థిరరాశి RBBIData_శీర్షిక *దత్తాంశం, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    f_శీర్షిక = దత్తాంశం;
    ఒకవేళ (f_శీర్షిక->fMagic != 0xb1a0 || !is_దత్తాంశం_సంస్కరణ_Acceptable(f_శీర్షిక->f_Format_సంస్కరణ)) {
        స్థితి_2 = U_INVALID_FORMAT_ERROR;
        తిరిగి;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    f_Dont_Free_దత్తాంశం = అబద్ధం;
    ఒకవేళ (దత్తాంశం->fFTableLen != 0) {
        f_Forward_పట్టిక = reinterpret_cast<స్థిరరాశి RBBIState_పట్టిక*>(reinterpret_cast<స్థిరరాశి అక్షరం*>(దత్తాంశం) + f_శీర్షిక->fFTable);
    }
    ఒకవేళ (దత్తాంశం->fRTableLen != 0) {
        f_తిరగేయి_పట్టిక = reinterpret_cast<స్థిరరాశి RBBIState_పట్టిక*>(reinterpret_cast<స్థిరరాశి అక్షరం*>(దత్తాంశం) + f_శీర్షిక->fRTable);
    }

    fTrie = ucptrie_open_From_ద్విమాన(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_విలువ_బిట్లు_ANY,
                                   (uint8_t *)దత్తాంశం + f_శీర్షిక->fTrie,
                                   f_శీర్షిక->fTrieLen,
                                   శూన్యసూచిక,           // *actual length
                                   &స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    UCPTrie_విలువ_వెడల్పు వెడల్పు = ucptrie_get_విలువ_వెడల్పు(fTrie);
    ఒకవేళ (!(వెడల్పు == UCPTRIE_విలువ_బిట్లు_8 || వెడల్పు == UCPTRIE_విలువ_బిట్లు_16)) {
        స్థితి_2 = U_INVALID_FORMAT_ERROR;
        తిరిగి;
    }

    f_నియమం_ఆధారం   = ((అక్షరం *)దత్తాంశం + f_శీర్షిక->f_నియమం_ఆధారం);
    f_నియమం_String = UnicodeString::fromUTF8(StringPiece(f_నియమం_ఆధారం, f_శీర్షిక->f_నియమం_ఆధారం_Len));
    U_ASSERT(దత్తాంశం->f_నియమం_ఆధారం_Len > 0);

    f_నియమం_స్థితి_పట్టిక = reinterpret_cast<స్థిరరాశి int32_t*>(reinterpret_cast<స్థిరరాశి అక్షరం*>(దత్తాంశం) + f_శీర్షిక->f_స్థితి_పట్టిక);
    f_స్థితి_Max_Idx    = దత్తాంశం->f_స్థితి_పట్టిక_Len / పరిమాణం(int32_t);

    f_Ref_లెక్క = 1;

#నిర్వచితమైతే RBBI_DEBUG
    అక్షరం *debugEnv = getenv("U_RBBIDEBUG");
    ఒకవేళ (debugEnv && uprv_strstr(debugEnv, "data")) {ఇది->print_దత్తాంశం();}
#అయితే_ముగింపు
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_లెక్క == 0);
    ucptrie_close(fTrie);
    fTrie = శూన్యసూచిక;
    ఒకవేళ (fUDataMem) {
        udata_close(fUDataMem);
    } లేకపోతే ఒకవేళ (!f_Dont_Free_దత్తాంశం) {
        uprv_free((శూన్య *)f_శీర్షిక);
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
తార్కిక RBBIDataWrapper::క్రియాకారకం ==(స్థిరరాశి RBBIDataWrapper &other) స్థిరరాశి {
    ఒకవేళ (f_శీర్షిక == other.f_శీర్షిక) {
        తిరిగి నిజం;
    }
    ఒకవేళ (f_శీర్షిక->f_నిడివి != other.f_శీర్షిక->f_నిడివి) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (uprv_memcmp(f_శీర్షిక, other.f_శీర్షిక, f_శీర్షిక->f_నిడివి) == 0) {
        తిరిగి నిజం;
    }
    తిరిగి అబద్ధం;
}

int32_t  RBBIDataWrapper::hash_సంకేతం() {
    తిరిగి f_శీర్షిక->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
శూన్య RBBIDataWrapper::removeReference() {
    ఒకవేళ (umtx_atomic_dec(&f_Ref_లెక్క) == 0) {
        తొలగించు ఇది;
    }
}


RBBIDataWrapper *RBBIDataWrapper::కూడు_Reference() {
   umtx_atomic_inc(&f_Ref_లెక్క);
   తిరిగి ఇది;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
స్థిరరాశి UnicodeString &RBBIDataWrapper::get_నియమం_ఆధారం_String() స్థిరరాశి {
    తిరిగి f_నియమం_String;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#నిర్వచితమైతే RBBI_DEBUG
శూన్య  RBBIDataWrapper::print_పట్టిక(స్థిరరాశి అక్షరం *heading, స్థిరరాశి RBBIState_పట్టిక *పట్టిక) {
    uint32_t   c;
    uint32_t   s;

    RBBIDebugPrintf("%s\n", heading);

    RBBIDebugPrintf("   fDictCategoriesStart: %d\n", పట్టిక->f_Dict_Categories_ప్రారంభం);
    RBBIDebugPrintf("   fLookAheadResultsSize: %d\n", పట్టిక->f_Look_Ahead_Results_కొలత);
    RBBIDebugPrintf("   Flags: %4x RBBI_LOOKAHEAD_HARD_BREAK=%s RBBI_8BITS_ROWS=%s\n",
                    పట్టిక->fFlags,
                    పట్టిక->fFlags & RBBI_LOOKAHEAD_HARD_BREAK ? "T" : "F",
                    పట్టిక->fFlags & RBBI_8_బిట్లు_అడ్డువరుసలు ? "T" : "F");
    RBBIDebugPrintf("\nState |  Acc  LA TagIx");
    కొరకు (c=0; c<f_శీర్షిక->f_Cat_లెక్క; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); కొరకు (c=0;c<f_శీర్షిక->f_Cat_లెక్క; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    ఒకవేళ (పట్టిక == శూన్యసూచిక) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        తిరిగి;
    }
    UBool use8_బిట్లు = పట్టిక->fFlags & RBBI_8_బిట్లు_అడ్డువరుసలు;
    కొరకు (s=0; s<పట్టిక->f_అంకె_స్థితులు; s++) {
        RBBIState_పట్టిక_అడ్డువరుస *అడ్డువరుస = (RBBIState_పట్టిక_అడ్డువరుస *)
                                  (పట్టిక->f_పట్టిక_దత్తాంశం + (పట్టిక->f_అడ్డువరుస_Len * s));
        ఒకవేళ (use8_బిట్లు) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, అడ్డువరుస->r8.fAccepting, అడ్డువరుస->r8.fLookAhead, అడ్డువరుస->r8.f_ట్యాగులు_Idx);
            కొరకు (c=0; c<f_శీర్షిక->f_Cat_లెక్క; c++)  {
                RBBIDebugPrintf("%3d ", అడ్డువరుస->r8.f_తదుపరి_State[c]);
            }
        } లేకపోతే {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, అడ్డువరుస->r16.fAccepting, అడ్డువరుస->r16.fLookAhead, అడ్డువరుస->r16.f_ట్యాగులు_Idx);
            కొరకు (c=0; c<f_శీర్షిక->f_Cat_లెక్క; c++)  {
                RBBIDebugPrintf("%3d ", అడ్డువరుస->r16.f_తదుపరి_State[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#అయితే_ముగింపు


శూన్య  RBBIDataWrapper::print_దత్తాంశం() {
#నిర్వచితమైతే RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (శూన్య *)f_శీర్షిక);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", f_శీర్షిక->f_Format_సంస్కరణ[0], f_శీర్షిక->f_Format_సంస్కరణ[1],
                                                    f_శీర్షిక->f_Format_సంస్కరణ[2], f_శీర్షిక->f_Format_సంస్కరణ[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", f_శీర్షిక->f_నిడివి);
    RBBIDebugPrintf("   number of character categories = %d\n\n", f_శీర్షిక->f_Cat_లెక్క);

    print_పట్టిక("Forward State Transition Table", f_Forward_పట్టిక);
    print_పట్టిక("Reverse State Transition Table", f_తిరగేయి_పట్టిక);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    కొరకు (int32_t c=0; f_నియమం_ఆధారం[c] != 0; c++) {
        RBBIDebugPrintf("%c", f_నియమం_ఆధారం[c]);
    }
    RBBIDebugPrintf("\n\n");
#అయితే_ముగింపు
}


U_NAMESPACE_చివర
U_NAMESPACE_USE

//-----------------------------------------------------------------------------
//
//  ubrk_swap   -  byte swap and char encoding swap of RBBI data
//
//-----------------------------------------------------------------------------

U_CAPI int32_t U_EXPORT2
ubrk_మార్పిడి(స్థిరరాశి UDataSwapper *ds, స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
           UError_సంకేతం *స్థితి_2) {

    ఒకవేళ (స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<-1 || (length>0 && బహిర్గతం_దత్తాంశం==శూన్యసూచిక)) {
        *స్థితి_2=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    స్థిరరాశి UDataInfo *pInfo = (స్థిరరాశి UDataInfo *)((స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+4);
    ఒకవేళ(!(  pInfo->దత్తాంశం_Format[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->దత్తాంశం_Format[1]==0x72 &&
           pInfo->దత్తాంశం_Format[2]==0x6b &&
           pInfo->దత్తాంశం_Format[3]==0x20 &&
           RBBIDataWrapper::is_దత్తాంశం_సంస్కరణ_Acceptable(pInfo->format_సంస్కరణ) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->దత్తాంశం_Format[0], pInfo->దత్తాంశం_Format[1],
                         pInfo->దత్తాంశం_Format[2], pInfo->దత్తాంశం_Format[3],
                         pInfo->format_సంస్కరణ[0]);
        *స్థితి_2=U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    //
    // Swap the data header.  (This is the generic ICU Data Header, not the RBBI Specific
    //                         RBBIDataHeader).  This swap also conveniently gets us
    //                         the size of the ICU d.h., which lets us locate the start
    //                         of the RBBI specific data.
    //
    int32_t శీర్షిక_కొలత=udata_మార్పిడి_దత్తాంశం_శీర్షిక(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, స్థితి_2);


    //
    // Get the RRBI Data Header, and check that it appears to be OK.
    //
    స్థిరరాశి uint8_t  *లోపల_బైట్లు =(స్థిరరాశి uint8_t *)లోపల_దత్తాంశం+శీర్షిక_కొలత;
    RBBIData_శీర్షిక *rbbiDH = (RBBIData_శీర్షిక *)లోపల_బైట్లు;
    ఒకవేళ (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::is_దత్తాంశం_సంస్కరణ_Acceptable(rbbiDH->f_Format_సంస్కరణ) ||
            ds->readUInt32(rbbiDH->f_నిడివి)  <  పరిమాణం(RBBIData_శీర్షిక)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *స్థితి_2=U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t break_దత్తాంశం_నిడివి = ds->readUInt32(rbbiDH->f_నిడివి);
    int32_t మొత్తం_కొలత = శీర్షిక_కొలత + break_దత్తాంశం_నిడివి;
    ఒకవేళ (length < 0) {
        తిరిగి మొత్తం_కొలత;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    ఒకవేళ (length < మొత్తం_కొలత) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            break_దత్తాంశం_నిడివి);
        *స్థితి_2=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
        }


    //
    // Swap the Data.  Do the data itself first, then the RBBI Data Header, because
    //                 we need to reference the header to locate the data, and an
    //                 inplace swap of the header leaves it unusable.
    //
    uint8_t         *బహిర్గతం_బైట్లు = (uint8_t *)బహిర్గతం_దత్తాంశం + శీర్షిక_కొలత;
    RBBIData_శీర్షిక  *outputDH = (RBBIData_శీర్షిక *)బహిర్గతం_బైట్లు;

    int32_t   పట్టిక_ప్రారంభం_స్థానభ్రంశం;
    int32_t   పట్టిక_నిడివి;

    //
    // If not swapping in place, zero out the output buffer before starting.
    //    Individual tables and other data items within are aligned to 8 byte boundaries
    //    when originally created.  Any unused space between items needs to be zero.
    //
    ఒకవేళ (లోపల_బైట్లు != బహిర్గతం_బైట్లు) {
        uprv_memset(బహిర్గతం_బైట్లు, 0, break_దత్తాంశం_నిడివి);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         శిఖరం_కొలత = offsetof(RBBIState_పట్టిక, f_పట్టిక_దత్తాంశం);

    // Forward state table.
    పట్టిక_ప్రారంభం_స్థానభ్రంశం = ds->readUInt32(rbbiDH->fFTable);
    పట్టిక_నిడివి      = ds->readUInt32(rbbiDH->fFTableLen);

    ఒకవేళ (పట్టిక_నిడివి > 0) {
        RBBIState_పట్టిక *rbbiST = (RBBIState_పట్టిక *)(లోపల_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం);
        UBool use8_బిట్లు = ds->readUInt32(rbbiST->fFlags) & RBBI_8_బిట్లు_అడ్డువరుసలు;

        ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం, శిఖరం_కొలత,
                            బహిర్గతం_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం, స్థితి_2);

        // Swap the state table if the table is in 16 bits.
        ఒకవేళ (use8_బిట్లు) {
            ఒకవేళ (బహిర్గతం_బైట్లు != లోపల_బైట్లు) {
                uprv_memmove(బహిర్గతం_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం+శిఖరం_కొలత,
                             లోపల_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం+శిఖరం_కొలత,
                             పట్టిక_నిడివి-శిఖరం_కొలత);
            }
        } లేకపోతే {
            ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం+శిఖరం_కొలత, పట్టిక_నిడివి-శిఖరం_కొలత,
                                బహిర్గతం_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం+శిఖరం_కొలత, స్థితి_2);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    పట్టిక_ప్రారంభం_స్థానభ్రంశం = ds->readUInt32(rbbiDH->fRTable);
    పట్టిక_నిడివి      = ds->readUInt32(rbbiDH->fRTableLen);

    ఒకవేళ (పట్టిక_నిడివి > 0) {
        RBBIState_పట్టిక *rbbiST = (RBBIState_పట్టిక *)(లోపల_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం);
        UBool use8_బిట్లు = ds->readUInt32(rbbiST->fFlags) & RBBI_8_బిట్లు_అడ్డువరుసలు;

        ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం, శిఖరం_కొలత,
                            బహిర్గతం_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం, స్థితి_2);

        // Swap the state table if the table is in 16 bits.
        ఒకవేళ (use8_బిట్లు) {
            ఒకవేళ (బహిర్గతం_బైట్లు != లోపల_బైట్లు) {
                uprv_memmove(బహిర్గతం_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం+శిఖరం_కొలత,
                             లోపల_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం+శిఖరం_కొలత,
                             పట్టిక_నిడివి-శిఖరం_కొలత);
            }
        } లేకపోతే {
            ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం+శిఖరం_కొలత, పట్టిక_నిడివి-శిఖరం_కొలత,
                                బహిర్గతం_బైట్లు+పట్టిక_ప్రారంభం_స్థానభ్రంశం+శిఖరం_కొలత, స్థితి_2);
        }
    }

    // Trie table for character categories
    ucptrie_మార్పిడి(ds, లోపల_బైట్లు+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     బహిర్గతం_బైట్లు+ds->readUInt32(rbbiDH->fTrie), స్థితి_2);

    // Source Rules Text.  It's UTF8 data
    ఒకవేళ (బహిర్గతం_బైట్లు != లోపల_బైట్లు) {
        uprv_memmove(బహిర్గతం_బైట్లు+ds->readUInt32(rbbiDH->f_నియమం_ఆధారం),
                     లోపల_బైట్లు+ds->readUInt32(rbbiDH->f_నియమం_ఆధారం),
                     ds->readUInt32(rbbiDH->f_నియమం_ఆధారం_Len));
    }

    // Table of rule status values.  It's all int_32 values
    ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+ds->readUInt32(rbbiDH->f_స్థితి_పట్టిక), ds->readUInt32(rbbiDH->f_స్థితి_పట్టిక_Len),
                        బహిర్గతం_బైట్లు+ds->readUInt32(rbbiDH->f_స్థితి_పట్టిక), స్థితి_2);

    // And, last, the header.
    //   It is all int32_t values except for fFormataVersion, which is an array of four bytes.
    //   Swap the whole thing as int32_t, then re-swap the one field.
    //
    ds->మార్పిడి_Array32(ds, లోపల_బైట్లు, పరిమాణం(RBBIData_శీర్షిక), బహిర్గతం_బైట్లు, స్థితి_2);
    ds->మార్పిడి_Array32(ds, outputDH->f_Format_సంస్కరణ, 4, outputDH->f_Format_సంస్కరణ, స్థితి_2);

    తిరిగి మొత్తం_కొలత;
}


#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */
