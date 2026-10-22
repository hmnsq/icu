// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 1999-2014 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "rbbidata.h"
#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "udatamem.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "umutex.h"

#अन्तर्भाव "uassert.h"


U_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//    Constructors.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::RBBIDataWrapper(ध्रुव RBBIData_शीर्षकम् *दत्त, UError_संहिता &स्थितिः) {
    init0();
    init(दत्त, स्थितिः);
}

RBBIDataWrapper::RBBIDataWrapper(ध्रुव RBBIData_शीर्षकम् *दत्त, गणना EDontAdopt, UError_संहिता &स्थितिः) {
    init0();
    init(दत्त, स्थितिः);
    f_Dont_Free_दत्त = सत्य;
}

RBBIDataWrapper::RBBIDataWrapper(UData_स्मृतिः* udm, UError_संहिता &स्थितिः) {
    init0();
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    ध्रुव दत्त_शीर्षकम् *dh = udm->p_शीर्षकम्;
    int32_t शीर्षकम्_आकार = dh->दत्त_शीर्षकम्_2.शीर्षकम्_आकार;
    यदि (  !(शीर्षकम्_आकार >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.दत्त_Format[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.दत्त_Format[1] == 0x72 &&
            dh->info.दत्त_Format[2] == 0x6b &&
            dh->info.दत्त_Format[3] == 0x20 &&
            is_दत्त_संस्करणम्_Acceptable(dh->info.format_संस्करणम्))
        ) {
        स्थितिः = U_INVALID_FORMAT_ERROR;
        प्रत्यागम;
    }
    ध्रुव वर्ण *दत्त_As_अष्टकानि = reinterpret_cast<ध्रुव वर्ण *>(dh);
    ध्रुव RBBIData_शीर्षकम् *rbbidh = reinterpret_cast<ध्रुव RBBIData_शीर्षकम् *>(दत्त_As_अष्टकानि + शीर्षकम्_आकार);
    init(rbbidh, स्थितिः);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::is_दत्त_संस्करणम्_Acceptable(ध्रुव UVersionInfo संस्करणम्) {
    प्रत्यागम RBBI_दत्त_FORMAT_संस्करणम्[0] == संस्करणम्[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
शून्य RBBIDataWrapper::init0() {
    f_शीर्षकम् = शून्यसूचकः;
    f_Forward_सारणी = शून्यसूचकः;
    f_Reverse_सारणी = शून्यसूचकः;
    f_नियमः_Source   = शून्यसूचकः;
    f_नियमः_स्थितिः_सारणी = शून्यसूचकः;
    fTrie         = शून्यसूचकः;
    fUDataMem     = शून्यसूचकः;
    f_Ref_गणनम्     = 0;
    f_Dont_Free_दत्त = सत्य;
}

शून्य RBBIDataWrapper::init(ध्रुव RBBIData_शीर्षकम् *दत्त, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    f_शीर्षकम् = दत्त;
    यदि (f_शीर्षकम्->fMagic != 0xb1a0 || !is_दत्त_संस्करणम्_Acceptable(f_शीर्षकम्->f_Format_संस्करणम्)) {
        स्थितिः = U_INVALID_FORMAT_ERROR;
        प्रत्यागम;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    f_Dont_Free_दत्त = असत्य;
    यदि (दत्त->fFTableLen != 0) {
        f_Forward_सारणी = reinterpret_cast<ध्रुव RBBIState_सारणी*>(reinterpret_cast<ध्रुव वर्ण*>(दत्त) + f_शीर्षकम्->fFTable);
    }
    यदि (दत्त->fRTableLen != 0) {
        f_Reverse_सारणी = reinterpret_cast<ध्रुव RBBIState_सारणी*>(reinterpret_cast<ध्रुव वर्ण*>(दत्त) + f_शीर्षकम्->fRTable);
    }

    fTrie = ucptrie_open_From_द्विमान(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_मानम्_द्वयङ्काः_ANY,
                                   (uint8_t *)दत्त + f_शीर्षकम्->fTrie,
                                   f_शीर्षकम्->fTrieLen,
                                   शून्यसूचकः,           // *actual length
                                   &स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }

    UCPTrie_मानम्_विस्तारः विस्तारः = ucptrie_get_मानम्_विस्तारः(fTrie);
    यदि (!(विस्तारः == UCPTRIE_मानम्_द्वयङ्काः_8 || विस्तारः == UCPTRIE_मानम्_द्वयङ्काः_16)) {
        स्थितिः = U_INVALID_FORMAT_ERROR;
        प्रत्यागम;
    }

    f_नियमः_Source   = ((वर्ण *)दत्त + f_शीर्षकम्->f_नियमः_Source);
    f_नियमः_String = UnicodeString::fromUTF8(StringPiece(f_नियमः_Source, f_शीर्षकम्->f_नियमः_Source_Len));
    U_ASSERT(दत्त->f_नियमः_Source_Len > 0);

    f_नियमः_स्थितिः_सारणी = reinterpret_cast<ध्रुव int32_t*>(reinterpret_cast<ध्रुव वर्ण*>(दत्त) + f_शीर्षकम्->f_स्थितिः_सारणी);
    f_स्थितिः_Max_Idx    = दत्त->f_स्थितिः_सारणी_Len / परिमाण(int32_t);

    f_Ref_गणनम् = 1;

#यदि_परिभाषितम् RBBI_DEBUG
    वर्ण *debugEnv = getenv("U_RBBIDEBUG");
    यदि (debugEnv && uprv_strstr(debugEnv, "data")) {एतत्->print_दत्त();}
#यदि_अन्तः
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_गणनम् == 0);
    ucptrie_close(fTrie);
    fTrie = शून्यसूचकः;
    यदि (fUDataMem) {
        udata_close(fUDataMem);
    } अन्यथा यदि (!f_Dont_Free_दत्त) {
        uprv_free((शून्य *)f_शीर्षकम्);
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
तार्किक RBBIDataWrapper::संक्रियकः ==(ध्रुव RBBIDataWrapper &other) ध्रुव {
    यदि (f_शीर्षकम् == other.f_शीर्षकम्) {
        प्रत्यागम सत्य;
    }
    यदि (f_शीर्षकम्->f_दीर्घता != other.f_शीर्षकम्->f_दीर्घता) {
        प्रत्यागम असत्य;
    }
    यदि (uprv_memcmp(f_शीर्षकम्, other.f_शीर्षकम्, f_शीर्षकम्->f_दीर्घता) == 0) {
        प्रत्यागम सत्य;
    }
    प्रत्यागम असत्य;
}

int32_t  RBBIDataWrapper::hash_संहिता() {
    प्रत्यागम f_शीर्षकम्->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
शून्य RBBIDataWrapper::removeReference() {
    यदि (umtx_atomic_dec(&f_Ref_गणनम्) == 0) {
        विलोपय एतत्;
    }
}


RBBIDataWrapper *RBBIDataWrapper::सङ्कलय_Reference() {
   umtx_atomic_inc(&f_Ref_गणनम्);
   प्रत्यागम एतत्;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ध्रुव UnicodeString &RBBIDataWrapper::get_नियमः_Source_String() ध्रुव {
    प्रत्यागम f_नियमः_String;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#यदि_परिभाषितम् RBBI_DEBUG
शून्य  RBBIDataWrapper::print_सारणी(ध्रुव वर्ण *heading, ध्रुव RBBIState_सारणी *सारणी) {
    uint32_t   c;
    uint32_t   s;

    RBBIDebugPrintf("%s\n", heading);

    RBBIDebugPrintf("   fDictCategoriesStart: %d\n", सारणी->fDictCategoriesStart);
    RBBIDebugPrintf("   fLookAheadResultsSize: %d\n", सारणी->f_Look_Ahead_Results_आकार);
    RBBIDebugPrintf("   Flags: %4x RBBI_LOOKAHEAD_HARD_BREAK=%s RBBI_8BITS_ROWS=%s\n",
                    सारणी->fFlags,
                    सारणी->fFlags & RBBI_LOOKAHEAD_HARD_BREAK ? "T" : "F",
                    सारणी->fFlags & RBBI_8_द्वयङ्काः_पङ्क्तिकाः ? "T" : "F");
    RBBIDebugPrintf("\nState |  Acc  LA TagIx");
    प्रति (c=0; c<f_शीर्षकम्->f_Cat_गणनम्; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); प्रति (c=0;c<f_शीर्षकम्->f_Cat_गणनम्; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    यदि (सारणी == शून्यसूचकः) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        प्रत्यागम;
    }
    UBool use8_द्वयङ्काः = सारणी->fFlags & RBBI_8_द्वयङ्काः_पङ्क्तिकाः;
    प्रति (s=0; s<सारणी->f_अङ्कः_अवस्थाः; s++) {
        RBBIState_सारणी_Row *row = (RBBIState_सारणी_Row *)
                                  (सारणी->f_सारणी_दत्त + (सारणी->fRowLen * s));
        यदि (use8_द्वयङ्काः) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r8.fAccepting, row->r8.fLookAhead, row->r8.f_चिह्नकानि_Idx);
            प्रति (c=0; c<f_शीर्षकम्->f_Cat_गणनम्; c++)  {
                RBBIDebugPrintf("%3d ", row->r8.f_अग्र_State[c]);
            }
        } अन्यथा {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r16.fAccepting, row->r16.fLookAhead, row->r16.f_चिह्नकानि_Idx);
            प्रति (c=0; c<f_शीर्षकम्->f_Cat_गणनम्; c++)  {
                RBBIDebugPrintf("%3d ", row->r16.f_अग्र_State[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#यदि_अन्तः


शून्य  RBBIDataWrapper::print_दत्त() {
#यदि_परिभाषितम् RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (शून्य *)f_शीर्षकम्);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", f_शीर्षकम्->f_Format_संस्करणम्[0], f_शीर्षकम्->f_Format_संस्करणम्[1],
                                                    f_शीर्षकम्->f_Format_संस्करणम्[2], f_शीर्षकम्->f_Format_संस्करणम्[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", f_शीर्षकम्->f_दीर्घता);
    RBBIDebugPrintf("   number of character categories = %d\n\n", f_शीर्षकम्->f_Cat_गणनम्);

    print_सारणी("Forward State Transition Table", f_Forward_सारणी);
    print_सारणी("Reverse State Transition Table", f_Reverse_सारणी);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    प्रति (int32_t c=0; f_नियमः_Source[c] != 0; c++) {
        RBBIDebugPrintf("%c", f_नियमः_Source[c]);
    }
    RBBIDebugPrintf("\n\n");
#यदि_अन्तः
}


U_NAMESPACE_अन्तः
U_NAMESPACE_USE

//-----------------------------------------------------------------------------
//
//  ubrk_swap   -  byte swap and char encoding swap of RBBI data
//
//-----------------------------------------------------------------------------

U_CAPI int32_t U_EXPORT2
ubrk_swap(ध्रुव UDataSwapper *ds, ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
           UError_संहिता *स्थितिः) {

    यदि (स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        प्रत्यागम 0;
    }
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || length<-1 || (length>0 && बहिर्गमनम्_दत्त==शून्यसूचकः)) {
        *स्थितिः=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ध्रुव UDataInfo *pInfo = (ध्रुव UDataInfo *)((ध्रुव वर्ण *)अन्तर्_दत्त+4);
    यदि(!(  pInfo->दत्त_Format[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->दत्त_Format[1]==0x72 &&
           pInfo->दत्त_Format[2]==0x6b &&
           pInfo->दत्त_Format[3]==0x20 &&
           RBBIDataWrapper::is_दत्त_संस्करणम्_Acceptable(pInfo->format_संस्करणम्) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->दत्त_Format[0], pInfo->दत्त_Format[1],
                         pInfo->दत्त_Format[2], pInfo->दत्त_Format[3],
                         pInfo->format_संस्करणम्[0]);
        *स्थितिः=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    //
    // Swap the data header.  (This is the generic ICU Data Header, not the RBBI Specific
    //                         RBBIDataHeader).  This swap also conveniently gets us
    //                         the size of the ICU d.h., which lets us locate the start
    //                         of the RBBI specific data.
    //
    int32_t शीर्षकम्_आकार=udata_swap_दत्त_शीर्षकम्(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, स्थितिः);


    //
    // Get the RRBI Data Header, and check that it appears to be OK.
    //
    ध्रुव uint8_t  *अन्तर्_अष्टकानि =(ध्रुव uint8_t *)अन्तर्_दत्त+शीर्षकम्_आकार;
    RBBIData_शीर्षकम् *rbbiDH = (RBBIData_शीर्षकम् *)अन्तर्_अष्टकानि;
    यदि (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::is_दत्त_संस्करणम्_Acceptable(rbbiDH->f_Format_संस्करणम्) ||
            ds->readUInt32(rbbiDH->f_दीर्घता)  <  परिमाण(RBBIData_शीर्षकम्)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *स्थितिः=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t break_दत्त_दीर्घता = ds->readUInt32(rbbiDH->f_दीर्घता);
    int32_t योग_आकार = शीर्षकम्_आकार + break_दत्त_दीर्घता;
    यदि (length < 0) {
        प्रत्यागम योग_आकार;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    यदि (length < योग_आकार) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            break_दत्त_दीर्घता);
        *स्थितिः=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
        }


    //
    // Swap the Data.  Do the data itself first, then the RBBI Data Header, because
    //                 we need to reference the header to locate the data, and an
    //                 inplace swap of the header leaves it unusable.
    //
    uint8_t         *बहिर्गमनम्_अष्टकानि = (uint8_t *)बहिर्गमनम्_दत्त + शीर्षकम्_आकार;
    RBBIData_शीर्षकम्  *outputDH = (RBBIData_शीर्षकम् *)बहिर्गमनम्_अष्टकानि;

    int32_t   सारणी_Start_विस्थापनम्;
    int32_t   सारणी_दीर्घता;

    //
    // If not swapping in place, zero out the output buffer before starting.
    //    Individual tables and other data items within are aligned to 8 byte boundaries
    //    when originally created.  Any unused space between items needs to be zero.
    //
    यदि (अन्तर्_अष्टकानि != बहिर्गमनम्_अष्टकानि) {
        uprv_memset(बहिर्गमनम्_अष्टकानि, 0, break_दत्त_दीर्घता);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         शिखरम्_आकार = offsetof(RBBIState_सारणी, f_सारणी_दत्त);

    // Forward state table.
    सारणी_Start_विस्थापनम् = ds->readUInt32(rbbiDH->fFTable);
    सारणी_दीर्घता      = ds->readUInt32(rbbiDH->fFTableLen);

    यदि (सारणी_दीर्घता > 0) {
        RBBIState_सारणी *rbbiST = (RBBIState_सारणी *)(अन्तर्_अष्टकानि+सारणी_Start_विस्थापनम्);
        UBool use8_द्वयङ्काः = ds->readUInt32(rbbiST->fFlags) & RBBI_8_द्वयङ्काः_पङ्क्तिकाः;

        ds->swapArray32(ds, अन्तर्_अष्टकानि+सारणी_Start_विस्थापनम्, शिखरम्_आकार,
                            बहिर्गमनम्_अष्टकानि+सारणी_Start_विस्थापनम्, स्थितिः);

        // Swap the state table if the table is in 16 bits.
        यदि (use8_द्वयङ्काः) {
            यदि (बहिर्गमनम्_अष्टकानि != अन्तर्_अष्टकानि) {
                uprv_memmove(बहिर्गमनम्_अष्टकानि+सारणी_Start_विस्थापनम्+शिखरम्_आकार,
                             अन्तर्_अष्टकानि+सारणी_Start_विस्थापनम्+शिखरम्_आकार,
                             सारणी_दीर्घता-शिखरम्_आकार);
            }
        } अन्यथा {
            ds->swapArray16(ds, अन्तर्_अष्टकानि+सारणी_Start_विस्थापनम्+शिखरम्_आकार, सारणी_दीर्घता-शिखरम्_आकार,
                                बहिर्गमनम्_अष्टकानि+सारणी_Start_विस्थापनम्+शिखरम्_आकार, स्थितिः);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    सारणी_Start_विस्थापनम् = ds->readUInt32(rbbiDH->fRTable);
    सारणी_दीर्घता      = ds->readUInt32(rbbiDH->fRTableLen);

    यदि (सारणी_दीर्घता > 0) {
        RBBIState_सारणी *rbbiST = (RBBIState_सारणी *)(अन्तर्_अष्टकानि+सारणी_Start_विस्थापनम्);
        UBool use8_द्वयङ्काः = ds->readUInt32(rbbiST->fFlags) & RBBI_8_द्वयङ्काः_पङ्क्तिकाः;

        ds->swapArray32(ds, अन्तर्_अष्टकानि+सारणी_Start_विस्थापनम्, शिखरम्_आकार,
                            बहिर्गमनम्_अष्टकानि+सारणी_Start_विस्थापनम्, स्थितिः);

        // Swap the state table if the table is in 16 bits.
        यदि (use8_द्वयङ्काः) {
            यदि (बहिर्गमनम्_अष्टकानि != अन्तर्_अष्टकानि) {
                uprv_memmove(बहिर्गमनम्_अष्टकानि+सारणी_Start_विस्थापनम्+शिखरम्_आकार,
                             अन्तर्_अष्टकानि+सारणी_Start_विस्थापनम्+शिखरम्_आकार,
                             सारणी_दीर्घता-शिखरम्_आकार);
            }
        } अन्यथा {
            ds->swapArray16(ds, अन्तर्_अष्टकानि+सारणी_Start_विस्थापनम्+शिखरम्_आकार, सारणी_दीर्घता-शिखरम्_आकार,
                                बहिर्गमनम्_अष्टकानि+सारणी_Start_विस्थापनम्+शिखरम्_आकार, स्थितिः);
        }
    }

    // Trie table for character categories
    ucptrie_swap(ds, अन्तर्_अष्टकानि+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     बहिर्गमनम्_अष्टकानि+ds->readUInt32(rbbiDH->fTrie), स्थितिः);

    // Source Rules Text.  It's UTF8 data
    यदि (बहिर्गमनम्_अष्टकानि != अन्तर्_अष्टकानि) {
        uprv_memmove(बहिर्गमनम्_अष्टकानि+ds->readUInt32(rbbiDH->f_नियमः_Source),
                     अन्तर्_अष्टकानि+ds->readUInt32(rbbiDH->f_नियमः_Source),
                     ds->readUInt32(rbbiDH->f_नियमः_Source_Len));
    }

    // Table of rule status values.  It's all int_32 values
    ds->swapArray32(ds, अन्तर्_अष्टकानि+ds->readUInt32(rbbiDH->f_स्थितिः_सारणी), ds->readUInt32(rbbiDH->f_स्थितिः_सारणी_Len),
                        बहिर्गमनम्_अष्टकानि+ds->readUInt32(rbbiDH->f_स्थितिः_सारणी), स्थितिः);

    // And, last, the header.
    //   It is all int32_t values except for fFormataVersion, which is an array of four bytes.
    //   Swap the whole thing as int32_t, then re-swap the one field.
    //
    ds->swapArray32(ds, अन्तर्_अष्टकानि, परिमाण(RBBIData_शीर्षकम्), बहिर्गमनम्_अष्टकानि, स्थितिः);
    ds->swapArray32(ds, outputDH->f_Format_संस्करणम्, 4, outputDH->f_Format_संस्करणम्, स्थितिः);

    प्रत्यागम योग_आकार;
}


#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */
