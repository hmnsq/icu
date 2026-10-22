// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 1999-2014 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/ucptrie.h"
#ضمن "unicode/utypes.h"
#ضمن "rbbidata.h"
#ضمن "rbbirb.h"
#ضمن "udatamem.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "umutex.h"

#ضمن "uassert.h"


U_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//    Constructors.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIData_سرو *معلومات, UError_ڪوڊ &حالت) {
    init0();
    init(معلومات, حالت);
}

RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIData_سرو *معلومات, تعداد EDontAdopt, UError_ڪوڊ &حالت) {
    init0();
    init(معلومات, حالت);
    f_Dont_Free_معلومات = سچ;
}

RBBIDataWrapper::RBBIDataWrapper(UData_يادگيري* udm, UError_ڪوڊ &حالت) {
    init0();
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    ثابت معلومات_سرو *dh = udm->p_سرو;
    int32_t سرو_سائيز = dh->معلومات_سرو_2.سرو_سائيز;
    إذا (  !(سرو_سائيز >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.معلومات_Format[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.معلومات_Format[1] == 0x72 &&
            dh->info.معلومات_Format[2] == 0x6b &&
            dh->info.معلومات_Format[3] == 0x20 &&
            is_معلومات_نسخو_Acceptable(dh->info.format_نسخو))
        ) {
        حالت = U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    ثابت حرف *معلومات_As_بائيٽون = reinterpret_cast<ثابت حرف *>(dh);
    ثابت RBBIData_سرو *rbbidh = reinterpret_cast<ثابت RBBIData_سرو *>(معلومات_As_بائيٽون + سرو_سائيز);
    init(rbbidh, حالت);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::is_معلومات_نسخو_Acceptable(ثابت UVersionInfo نسخو) {
    أرجع RBBI_معلومات_FORMAT_نسخو[0] == نسخو[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
فراغ RBBIDataWrapper::init0() {
    f_سرو = خالي_اشارو;
    f_Forward_جدول = خالي_اشارو;
    f_ابتو_جدول = خالي_اشارو;
    f_قاعدو_ذريعو   = خالي_اشارو;
    f_قاعدو_حالت_جدول = خالي_اشارو;
    fTrie         = خالي_اشارو;
    fUDataMem     = خالي_اشارو;
    f_Ref_ڳڻپ     = 0;
    f_Dont_Free_معلومات = سچ;
}

فراغ RBBIDataWrapper::init(ثابت RBBIData_سرو *معلومات, UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    f_سرو = معلومات;
    إذا (f_سرو->fMagic != 0xb1a0 || !is_معلومات_نسخو_Acceptable(f_سرو->f_Format_نسخو)) {
        حالت = U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    f_Dont_Free_معلومات = خطأ;
    إذا (معلومات->fFTableLen != 0) {
        f_Forward_جدول = reinterpret_cast<ثابت RBBIState_جدول*>(reinterpret_cast<ثابت حرف*>(معلومات) + f_سرو->fFTable);
    }
    إذا (معلومات->fRTableLen != 0) {
        f_ابتو_جدول = reinterpret_cast<ثابت RBBIState_جدول*>(reinterpret_cast<ثابت حرف*>(معلومات) + f_سرو->fRTable);
    }

    fTrie = ucptrie_open_From_ٻٽو(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_قدر_بٽ_ANY,
                                   (uint8_t *)معلومات + f_سرو->fTrie,
                                   f_سرو->fTrieLen,
                                   خالي_اشارو,           // *actual length
                                   &حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    UCPTrie_قدر_ويڪر ويڪر = ucptrie_get_قدر_ويڪر(fTrie);
    إذا (!(ويڪر == UCPTRIE_قدر_بٽ_8 || ويڪر == UCPTRIE_قدر_بٽ_16)) {
        حالت = U_INVALID_FORMAT_ERROR;
        أرجع;
    }

    f_قاعدو_ذريعو   = ((حرف *)معلومات + f_سرو->f_قاعدو_ذريعو);
    f_قاعدو_String = UnicodeString::fromUTF8(StringPiece(f_قاعدو_ذريعو, f_سرو->f_قاعدو_ذريعو_Len));
    U_ASSERT(معلومات->f_قاعدو_ذريعو_Len > 0);

    f_قاعدو_حالت_جدول = reinterpret_cast<ثابت int32_t*>(reinterpret_cast<ثابت حرف*>(معلومات) + f_سرو->f_حالت_جدول);
    f_حالت_Max_Idx    = معلومات->f_حالت_جدول_Len / حجم(int32_t);

    f_Ref_ڳڻپ = 1;

#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
    حرف *debugEnv = getenv("U_RBBIDEBUG");
    إذا (debugEnv && uprv_strstr(debugEnv, "data")) {هذا->print_معلومات();}
#جيڪڏهن_پورو
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_ڳڻپ == 0);
    ucptrie_close(fTrie);
    fTrie = خالي_اشارو;
    إذا (fUDataMem) {
        udata_close(fUDataMem);
    } وإلا إذا (!f_Dont_Free_معلومات) {
        uprv_free((فراغ *)f_سرو);
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
منطقي RBBIDataWrapper::عامل ==(ثابت RBBIDataWrapper &other) ثابت {
    إذا (f_سرو == other.f_سرو) {
        أرجع سچ;
    }
    إذا (f_سرو->f_ڊيگهائي != other.f_سرو->f_ڊيگهائي) {
        أرجع خطأ;
    }
    إذا (uprv_memcmp(f_سرو, other.f_سرو, f_سرو->f_ڊيگهائي) == 0) {
        أرجع سچ;
    }
    أرجع خطأ;
}

int32_t  RBBIDataWrapper::hash_ڪوڊ() {
    أرجع f_سرو->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
فراغ RBBIDataWrapper::removeReference() {
    إذا (umtx_atomic_dec(&f_Ref_ڳڻپ) == 0) {
        ميٽيو هذا;
    }
}


RBBIDataWrapper *RBBIDataWrapper::جوڙيو_Reference() {
   umtx_atomic_inc(&f_Ref_ڳڻپ);
   أرجع هذا;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ثابت UnicodeString &RBBIDataWrapper::get_قاعدو_ذريعو_String() ثابت {
    أرجع f_قاعدو_String;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
فراغ  RBBIDataWrapper::print_جدول(ثابت حرف *heading, ثابت RBBIState_جدول *جدول) {
    uint32_t   c;
    uint32_t   s;

    RBBIDebugPrintf("%s\n", heading);

    RBBIDebugPrintf("   fDictCategoriesStart: %d\n", جدول->f_Dict_Categories_شروعات);
    RBBIDebugPrintf("   fLookAheadResultsSize: %d\n", جدول->f_Look_Ahead_Results_سائيز);
    RBBIDebugPrintf("   Flags: %4x RBBI_LOOKAHEAD_HARD_BREAK=%s RBBI_8BITS_ROWS=%s\n",
                    جدول->fFlags,
                    جدول->fFlags & RBBI_LOOKAHEAD_HARD_BREAK ? "T" : "F",
                    جدول->fFlags & RBBI_8_بٽ_صفون ? "T" : "F");
    RBBIDebugPrintf("\nState |  Acc  LA TagIx");
    لكل (c=0; c<f_سرو->f_Cat_ڳڻپ; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); لكل (c=0;c<f_سرو->f_Cat_ڳڻپ; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    إذا (جدول == خالي_اشارو) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        أرجع;
    }
    UBool use8_بٽ = جدول->fFlags & RBBI_8_بٽ_صفون;
    لكل (s=0; s<جدول->f_انگ_حالتون; s++) {
        RBBIState_جدول_صف *صف = (RBBIState_جدول_صف *)
                                  (جدول->f_جدول_معلومات + (جدول->f_صف_Len * s));
        إذا (use8_بٽ) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, صف->r8.fAccepting, صف->r8.fLookAhead, صف->r8.f_ٽيگ_فهرست_Idx);
            لكل (c=0; c<f_سرو->f_Cat_ڳڻپ; c++)  {
                RBBIDebugPrintf("%3d ", صف->r8.f_ايندڙ_State[c]);
            }
        } وإلا {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, صف->r16.fAccepting, صف->r16.fLookAhead, صف->r16.f_ٽيگ_فهرست_Idx);
            لكل (c=0; c<f_سرو->f_Cat_ڳڻپ; c++)  {
                RBBIDebugPrintf("%3d ", صف->r16.f_ايندڙ_State[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#جيڪڏهن_پورو


فراغ  RBBIDataWrapper::print_معلومات() {
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (فراغ *)f_سرو);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", f_سرو->f_Format_نسخو[0], f_سرو->f_Format_نسخو[1],
                                                    f_سرو->f_Format_نسخو[2], f_سرو->f_Format_نسخو[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", f_سرو->f_ڊيگهائي);
    RBBIDebugPrintf("   number of character categories = %d\n\n", f_سرو->f_Cat_ڳڻپ);

    print_جدول("Forward State Transition Table", f_Forward_جدول);
    print_جدول("Reverse State Transition Table", f_ابتو_جدول);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    لكل (int32_t c=0; f_قاعدو_ذريعو[c] != 0; c++) {
        RBBIDebugPrintf("%c", f_قاعدو_ذريعو[c]);
    }
    RBBIDebugPrintf("\n\n");
#جيڪڏهن_پورو
}


U_NAMESPACE_پڇاڙي
U_NAMESPACE_USE

//-----------------------------------------------------------------------------
//
//  ubrk_swap   -  byte swap and char encoding swap of RBBI data
//
//-----------------------------------------------------------------------------

U_CAPI int32_t U_EXPORT2
ubrk_مٽا_سٽا(ثابت UDataSwapper *ds, ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
           UError_ڪوڊ *حالت) {

    إذا (حالت == خالي_اشارو || U_FAILURE(*حالت)) {
        أرجع 0;
    }
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || length<-1 || (length>0 && ٻاهريون_معلومات==خالي_اشارو)) {
        *حالت=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ثابت UDataInfo *pInfo = (ثابت UDataInfo *)((ثابت حرف *)اندريون_معلومات+4);
    إذا(!(  pInfo->معلومات_Format[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->معلومات_Format[1]==0x72 &&
           pInfo->معلومات_Format[2]==0x6b &&
           pInfo->معلومات_Format[3]==0x20 &&
           RBBIDataWrapper::is_معلومات_نسخو_Acceptable(pInfo->format_نسخو) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخو[0]);
        *حالت=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    //
    // Swap the data header.  (This is the generic ICU Data Header, not the RBBI Specific
    //                         RBBIDataHeader).  This swap also conveniently gets us
    //                         the size of the ICU d.h., which lets us locate the start
    //                         of the RBBI specific data.
    //
    int32_t سرو_سائيز=udata_مٽا_سٽا_معلومات_سرو(ds, اندريون_معلومات, length, ٻاهريون_معلومات, حالت);


    //
    // Get the RRBI Data Header, and check that it appears to be OK.
    //
    ثابت uint8_t  *اندريون_بائيٽون =(ثابت uint8_t *)اندريون_معلومات+سرو_سائيز;
    RBBIData_سرو *rbbiDH = (RBBIData_سرو *)اندريون_بائيٽون;
    إذا (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::is_معلومات_نسخو_Acceptable(rbbiDH->f_Format_نسخو) ||
            ds->readUInt32(rbbiDH->f_ڊيگهائي)  <  حجم(RBBIData_سرو)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *حالت=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t break_معلومات_ڊيگهائي = ds->readUInt32(rbbiDH->f_ڊيگهائي);
    int32_t ڪل_سائيز = سرو_سائيز + break_معلومات_ڊيگهائي;
    إذا (length < 0) {
        أرجع ڪل_سائيز;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    إذا (length < ڪل_سائيز) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            break_معلومات_ڊيگهائي);
        *حالت=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
        }


    //
    // Swap the Data.  Do the data itself first, then the RBBI Data Header, because
    //                 we need to reference the header to locate the data, and an
    //                 inplace swap of the header leaves it unusable.
    //
    uint8_t         *ٻاهريون_بائيٽون = (uint8_t *)ٻاهريون_معلومات + سرو_سائيز;
    RBBIData_سرو  *outputDH = (RBBIData_سرو *)ٻاهريون_بائيٽون;

    int32_t   جدول_شروعات_وٿي;
    int32_t   جدول_ڊيگهائي;

    //
    // If not swapping in place, zero out the output buffer before starting.
    //    Individual tables and other data items within are aligned to 8 byte boundaries
    //    when originally created.  Any unused space between items needs to be zero.
    //
    إذا (اندريون_بائيٽون != ٻاهريون_بائيٽون) {
        uprv_memset(ٻاهريون_بائيٽون, 0, break_معلومات_ڊيگهائي);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         چوٽي_سائيز = offsetof(RBBIState_جدول, f_جدول_معلومات);

    // Forward state table.
    جدول_شروعات_وٿي = ds->readUInt32(rbbiDH->fFTable);
    جدول_ڊيگهائي      = ds->readUInt32(rbbiDH->fFTableLen);

    إذا (جدول_ڊيگهائي > 0) {
        RBBIState_جدول *rbbiST = (RBBIState_جدول *)(اندريون_بائيٽون+جدول_شروعات_وٿي);
        UBool use8_بٽ = ds->readUInt32(rbbiST->fFlags) & RBBI_8_بٽ_صفون;

        ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+جدول_شروعات_وٿي, چوٽي_سائيز,
                            ٻاهريون_بائيٽون+جدول_شروعات_وٿي, حالت);

        // Swap the state table if the table is in 16 bits.
        إذا (use8_بٽ) {
            إذا (ٻاهريون_بائيٽون != اندريون_بائيٽون) {
                uprv_memmove(ٻاهريون_بائيٽون+جدول_شروعات_وٿي+چوٽي_سائيز,
                             اندريون_بائيٽون+جدول_شروعات_وٿي+چوٽي_سائيز,
                             جدول_ڊيگهائي-چوٽي_سائيز);
            }
        } وإلا {
            ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+جدول_شروعات_وٿي+چوٽي_سائيز, جدول_ڊيگهائي-چوٽي_سائيز,
                                ٻاهريون_بائيٽون+جدول_شروعات_وٿي+چوٽي_سائيز, حالت);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    جدول_شروعات_وٿي = ds->readUInt32(rbbiDH->fRTable);
    جدول_ڊيگهائي      = ds->readUInt32(rbbiDH->fRTableLen);

    إذا (جدول_ڊيگهائي > 0) {
        RBBIState_جدول *rbbiST = (RBBIState_جدول *)(اندريون_بائيٽون+جدول_شروعات_وٿي);
        UBool use8_بٽ = ds->readUInt32(rbbiST->fFlags) & RBBI_8_بٽ_صفون;

        ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+جدول_شروعات_وٿي, چوٽي_سائيز,
                            ٻاهريون_بائيٽون+جدول_شروعات_وٿي, حالت);

        // Swap the state table if the table is in 16 bits.
        إذا (use8_بٽ) {
            إذا (ٻاهريون_بائيٽون != اندريون_بائيٽون) {
                uprv_memmove(ٻاهريون_بائيٽون+جدول_شروعات_وٿي+چوٽي_سائيز,
                             اندريون_بائيٽون+جدول_شروعات_وٿي+چوٽي_سائيز,
                             جدول_ڊيگهائي-چوٽي_سائيز);
            }
        } وإلا {
            ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+جدول_شروعات_وٿي+چوٽي_سائيز, جدول_ڊيگهائي-چوٽي_سائيز,
                                ٻاهريون_بائيٽون+جدول_شروعات_وٿي+چوٽي_سائيز, حالت);
        }
    }

    // Trie table for character categories
    ucptrie_مٽا_سٽا(ds, اندريون_بائيٽون+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     ٻاهريون_بائيٽون+ds->readUInt32(rbbiDH->fTrie), حالت);

    // Source Rules Text.  It's UTF8 data
    إذا (ٻاهريون_بائيٽون != اندريون_بائيٽون) {
        uprv_memmove(ٻاهريون_بائيٽون+ds->readUInt32(rbbiDH->f_قاعدو_ذريعو),
                     اندريون_بائيٽون+ds->readUInt32(rbbiDH->f_قاعدو_ذريعو),
                     ds->readUInt32(rbbiDH->f_قاعدو_ذريعو_Len));
    }

    // Table of rule status values.  It's all int_32 values
    ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+ds->readUInt32(rbbiDH->f_حالت_جدول), ds->readUInt32(rbbiDH->f_حالت_جدول_Len),
                        ٻاهريون_بائيٽون+ds->readUInt32(rbbiDH->f_حالت_جدول), حالت);

    // And, last, the header.
    //   It is all int32_t values except for fFormataVersion, which is an array of four bytes.
    //   Swap the whole thing as int32_t, then re-swap the one field.
    //
    ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون, حجم(RBBIData_سرو), ٻاهريون_بائيٽون, حالت);
    ds->مٽا_سٽا_Array32(ds, outputDH->f_Format_نسخو, 4, outputDH->f_Format_نسخو, حالت);

    أرجع ڪل_سائيز;
}


#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */
