// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 1999-2014 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/ucptrie.h"
#شامل "unicode/utypes.h"
#شامل "rbbidata.h"
#شامل "rbbirb.h"
#شامل "udatamem.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "umutex.h"

#شامل "uassert.h"


U_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//    Constructors.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIData_سرخی *ڈیٹا, UError_رمز &حالت_2) {
    init0();
    init(ڈیٹا, حالت_2);
}

RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIData_سرخی *ڈیٹا, شمار EDontAdopt, UError_رمز &حالت_2) {
    init0();
    init(ڈیٹا, حالت_2);
    f_Dont_Free_ڈیٹا = سچ;
}

RBBIDataWrapper::RBBIDataWrapper(UData_حافظہ* udm, UError_رمز &حالت_2) {
    init0();
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    ثابت ڈیٹا_سرخی *dh = udm->p_سرخی;
    int32_t سرخی_سائز = dh->ڈیٹا_سرخی_2.سرخی_سائز;
    اگر (  !(سرخی_سائز >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.ڈیٹا_Format[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.ڈیٹا_Format[1] == 0x72 &&
            dh->info.ڈیٹا_Format[2] == 0x6b &&
            dh->info.ڈیٹا_Format[3] == 0x20 &&
            is_ڈیٹا_نسخہ_Acceptable(dh->info.format_نسخہ))
        ) {
        حالت_2 = U_INVALID_FORMAT_ERROR;
        واپس;
    }
    ثابت حرف *ڈیٹا_As_بائٹیں = reinterpret_cast<ثابت حرف *>(dh);
    ثابت RBBIData_سرخی *rbbidh = reinterpret_cast<ثابت RBBIData_سرخی *>(ڈیٹا_As_بائٹیں + سرخی_سائز);
    init(rbbidh, حالت_2);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::is_ڈیٹا_نسخہ_Acceptable(ثابت UVersionInfo نسخہ) {
    واپس RBBI_ڈیٹا_FORMAT_نسخہ[0] == نسخہ[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
خالی RBBIDataWrapper::init0() {
    f_سرخی = خالی_اشاریہ;
    f_Forward_جدول = خالی_اشاریہ;
    f_الٹ_جدول = خالی_اشاریہ;
    f_قاعدہ_ماخذ   = خالی_اشاریہ;
    f_قاعدہ_حالت_جدول = خالی_اشاریہ;
    fTrie         = خالی_اشاریہ;
    fUDataMem     = خالی_اشاریہ;
    f_Ref_گنتی     = 0;
    f_Dont_Free_ڈیٹا = سچ;
}

خالی RBBIDataWrapper::init(ثابت RBBIData_سرخی *ڈیٹا, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    f_سرخی = ڈیٹا;
    اگر (f_سرخی->fMagic != 0xb1a0 || !is_ڈیٹا_نسخہ_Acceptable(f_سرخی->f_Format_نسخہ)) {
        حالت_2 = U_INVALID_FORMAT_ERROR;
        واپس;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    f_Dont_Free_ڈیٹا = جھوٹ;
    اگر (ڈیٹا->fFTableLen != 0) {
        f_Forward_جدول = reinterpret_cast<ثابت RBBIState_جدول*>(reinterpret_cast<ثابت حرف*>(ڈیٹا) + f_سرخی->fFTable);
    }
    اگر (ڈیٹا->fRTableLen != 0) {
        f_الٹ_جدول = reinterpret_cast<ثابت RBBIState_جدول*>(reinterpret_cast<ثابت حرف*>(ڈیٹا) + f_سرخی->fRTable);
    }

    fTrie = ucptrie_open_From_ثنائی(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_قدر_بٹ_ANY,
                                   (uint8_t *)ڈیٹا + f_سرخی->fTrie,
                                   f_سرخی->fTrieLen,
                                   خالی_اشاریہ,           // *actual length
                                   &حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }

    UCPTrie_قدر_چوڑائی چوڑائی = ucptrie_get_قدر_چوڑائی(fTrie);
    اگر (!(چوڑائی == UCPTRIE_قدر_بٹ_8 || چوڑائی == UCPTRIE_قدر_بٹ_16)) {
        حالت_2 = U_INVALID_FORMAT_ERROR;
        واپس;
    }

    f_قاعدہ_ماخذ   = ((حرف *)ڈیٹا + f_سرخی->f_قاعدہ_ماخذ);
    f_قاعدہ_String = UnicodeString::fromUTF8(StringPiece(f_قاعدہ_ماخذ, f_سرخی->f_قاعدہ_ماخذ_Len));
    U_ASSERT(ڈیٹا->f_قاعدہ_ماخذ_Len > 0);

    f_قاعدہ_حالت_جدول = reinterpret_cast<ثابت int32_t*>(reinterpret_cast<ثابت حرف*>(ڈیٹا) + f_سرخی->f_حالت_جدول);
    f_حالت_Max_Idx    = ڈیٹا->f_حالت_جدول_Len / حجم(int32_t);

    f_Ref_گنتی = 1;

#اگر_تعریف_شدہ RBBI_DEBUG
    حرف *debugEnv = getenv("U_RBBIDEBUG");
    اگر (debugEnv && uprv_strstr(debugEnv, "data")) {یہ->print_ڈیٹا();}
#اگر_ختم
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_گنتی == 0);
    ucptrie_close(fTrie);
    fTrie = خالی_اشاریہ;
    اگر (fUDataMem) {
        udata_close(fUDataMem);
    } ورنہ اگر (!f_Dont_Free_ڈیٹا) {
        uprv_free((خالی *)f_سرخی);
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
منطقی RBBIDataWrapper::عامل ==(ثابت RBBIDataWrapper &other) ثابت {
    اگر (f_سرخی == other.f_سرخی) {
        واپس سچ;
    }
    اگر (f_سرخی->f_لمبائی != other.f_سرخی->f_لمبائی) {
        واپس جھوٹ;
    }
    اگر (uprv_memcmp(f_سرخی, other.f_سرخی, f_سرخی->f_لمبائی) == 0) {
        واپس سچ;
    }
    واپس جھوٹ;
}

int32_t  RBBIDataWrapper::hash_رمز() {
    واپس f_سرخی->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
خالی RBBIDataWrapper::removeReference() {
    اگر (umtx_atomic_dec(&f_Ref_گنتی) == 0) {
        حذف یہ;
    }
}


RBBIDataWrapper *RBBIDataWrapper::جوڑو_Reference() {
   umtx_atomic_inc(&f_Ref_گنتی);
   واپس یہ;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ثابت UnicodeString &RBBIDataWrapper::get_قاعدہ_ماخذ_String() ثابت {
    واپس f_قاعدہ_String;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#اگر_تعریف_شدہ RBBI_DEBUG
خالی  RBBIDataWrapper::print_جدول(ثابت حرف *heading, ثابت RBBIState_جدول *جدول) {
    uint32_t   c;
    uint32_t   s;

    RBBIDebugPrintf("%s\n", heading);

    RBBIDebugPrintf("   fDictCategoriesStart: %d\n", جدول->f_Dict_Categories_آغاز);
    RBBIDebugPrintf("   fLookAheadResultsSize: %d\n", جدول->f_Look_Ahead_Results_سائز);
    RBBIDebugPrintf("   Flags: %4x RBBI_LOOKAHEAD_HARD_BREAK=%s RBBI_8BITS_ROWS=%s\n",
                    جدول->fFlags,
                    جدول->fFlags & RBBI_LOOKAHEAD_HARD_BREAK ? "T" : "F",
                    جدول->fFlags & RBBI_8_بٹ_صفیں ? "T" : "F");
    RBBIDebugPrintf("\nState |  Acc  LA TagIx");
    برائے (c=0; c<f_سرخی->f_Cat_گنتی; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); برائے (c=0;c<f_سرخی->f_Cat_گنتی; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    اگر (جدول == خالی_اشاریہ) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        واپس;
    }
    UBool use8_بٹ = جدول->fFlags & RBBI_8_بٹ_صفیں;
    برائے (s=0; s<جدول->f_عدد_حالتیں; s++) {
        RBBIState_جدول_صف *صف = (RBBIState_جدول_صف *)
                                  (جدول->f_جدول_ڈیٹا + (جدول->f_صف_Len * s));
        اگر (use8_بٹ) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, صف->r8.fAccepting, صف->r8.fLookAhead, صف->r8.f_لیبل_فہرست_Idx);
            برائے (c=0; c<f_سرخی->f_Cat_گنتی; c++)  {
                RBBIDebugPrintf("%3d ", صف->r8.f_اگلا_State[c]);
            }
        } ورنہ {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, صف->r16.fAccepting, صف->r16.fLookAhead, صف->r16.f_لیبل_فہرست_Idx);
            برائے (c=0; c<f_سرخی->f_Cat_گنتی; c++)  {
                RBBIDebugPrintf("%3d ", صف->r16.f_اگلا_State[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#اگر_ختم


خالی  RBBIDataWrapper::print_ڈیٹا() {
#اگر_تعریف_شدہ RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (خالی *)f_سرخی);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", f_سرخی->f_Format_نسخہ[0], f_سرخی->f_Format_نسخہ[1],
                                                    f_سرخی->f_Format_نسخہ[2], f_سرخی->f_Format_نسخہ[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", f_سرخی->f_لمبائی);
    RBBIDebugPrintf("   number of character categories = %d\n\n", f_سرخی->f_Cat_گنتی);

    print_جدول("Forward State Transition Table", f_Forward_جدول);
    print_جدول("Reverse State Transition Table", f_الٹ_جدول);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    برائے (int32_t c=0; f_قاعدہ_ماخذ[c] != 0; c++) {
        RBBIDebugPrintf("%c", f_قاعدہ_ماخذ[c]);
    }
    RBBIDebugPrintf("\n\n");
#اگر_ختم
}


U_NAMESPACE_انتہا
U_NAMESPACE_USE

//-----------------------------------------------------------------------------
//
//  ubrk_swap   -  byte swap and char encoding swap of RBBI data
//
//-----------------------------------------------------------------------------

U_CAPI int32_t U_EXPORT2
ubrk_تبادلہ(ثابت UDataSwapper *ds, ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
           UError_رمز *حالت_2) {

    اگر (حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        واپس 0;
    }
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || length<-1 || (length>0 && برآمد_ڈیٹا==خالی_اشاریہ)) {
        *حالت_2=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ثابت UDataInfo *pInfo = (ثابت UDataInfo *)((ثابت حرف *)اندر_ڈیٹا+4);
    اگر(!(  pInfo->ڈیٹا_Format[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->ڈیٹا_Format[1]==0x72 &&
           pInfo->ڈیٹا_Format[2]==0x6b &&
           pInfo->ڈیٹا_Format[3]==0x20 &&
           RBBIDataWrapper::is_ڈیٹا_نسخہ_Acceptable(pInfo->format_نسخہ) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->ڈیٹا_Format[0], pInfo->ڈیٹا_Format[1],
                         pInfo->ڈیٹا_Format[2], pInfo->ڈیٹا_Format[3],
                         pInfo->format_نسخہ[0]);
        *حالت_2=U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    //
    // Swap the data header.  (This is the generic ICU Data Header, not the RBBI Specific
    //                         RBBIDataHeader).  This swap also conveniently gets us
    //                         the size of the ICU d.h., which lets us locate the start
    //                         of the RBBI specific data.
    //
    int32_t سرخی_سائز=udata_تبادلہ_ڈیٹا_سرخی(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, حالت_2);


    //
    // Get the RRBI Data Header, and check that it appears to be OK.
    //
    ثابت uint8_t  *اندر_بائٹیں =(ثابت uint8_t *)اندر_ڈیٹا+سرخی_سائز;
    RBBIData_سرخی *rbbiDH = (RBBIData_سرخی *)اندر_بائٹیں;
    اگر (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::is_ڈیٹا_نسخہ_Acceptable(rbbiDH->f_Format_نسخہ) ||
            ds->readUInt32(rbbiDH->f_لمبائی)  <  حجم(RBBIData_سرخی)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *حالت_2=U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t break_ڈیٹا_لمبائی = ds->readUInt32(rbbiDH->f_لمبائی);
    int32_t کل_سائز = سرخی_سائز + break_ڈیٹا_لمبائی;
    اگر (length < 0) {
        واپس کل_سائز;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    اگر (length < کل_سائز) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            break_ڈیٹا_لمبائی);
        *حالت_2=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
        }


    //
    // Swap the Data.  Do the data itself first, then the RBBI Data Header, because
    //                 we need to reference the header to locate the data, and an
    //                 inplace swap of the header leaves it unusable.
    //
    uint8_t         *برآمد_بائٹیں = (uint8_t *)برآمد_ڈیٹا + سرخی_سائز;
    RBBIData_سرخی  *outputDH = (RBBIData_سرخی *)برآمد_بائٹیں;

    int32_t   جدول_آغاز_انحراف;
    int32_t   جدول_لمبائی;

    //
    // If not swapping in place, zero out the output buffer before starting.
    //    Individual tables and other data items within are aligned to 8 byte boundaries
    //    when originally created.  Any unused space between items needs to be zero.
    //
    اگر (اندر_بائٹیں != برآمد_بائٹیں) {
        uprv_memset(برآمد_بائٹیں, 0, break_ڈیٹا_لمبائی);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         چوٹی_سائز = offsetof(RBBIState_جدول, f_جدول_ڈیٹا);

    // Forward state table.
    جدول_آغاز_انحراف = ds->readUInt32(rbbiDH->fFTable);
    جدول_لمبائی      = ds->readUInt32(rbbiDH->fFTableLen);

    اگر (جدول_لمبائی > 0) {
        RBBIState_جدول *rbbiST = (RBBIState_جدول *)(اندر_بائٹیں+جدول_آغاز_انحراف);
        UBool use8_بٹ = ds->readUInt32(rbbiST->fFlags) & RBBI_8_بٹ_صفیں;

        ds->تبادلہ_Array32(ds, اندر_بائٹیں+جدول_آغاز_انحراف, چوٹی_سائز,
                            برآمد_بائٹیں+جدول_آغاز_انحراف, حالت_2);

        // Swap the state table if the table is in 16 bits.
        اگر (use8_بٹ) {
            اگر (برآمد_بائٹیں != اندر_بائٹیں) {
                uprv_memmove(برآمد_بائٹیں+جدول_آغاز_انحراف+چوٹی_سائز,
                             اندر_بائٹیں+جدول_آغاز_انحراف+چوٹی_سائز,
                             جدول_لمبائی-چوٹی_سائز);
            }
        } ورنہ {
            ds->تبادلہ_Array16(ds, اندر_بائٹیں+جدول_آغاز_انحراف+چوٹی_سائز, جدول_لمبائی-چوٹی_سائز,
                                برآمد_بائٹیں+جدول_آغاز_انحراف+چوٹی_سائز, حالت_2);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    جدول_آغاز_انحراف = ds->readUInt32(rbbiDH->fRTable);
    جدول_لمبائی      = ds->readUInt32(rbbiDH->fRTableLen);

    اگر (جدول_لمبائی > 0) {
        RBBIState_جدول *rbbiST = (RBBIState_جدول *)(اندر_بائٹیں+جدول_آغاز_انحراف);
        UBool use8_بٹ = ds->readUInt32(rbbiST->fFlags) & RBBI_8_بٹ_صفیں;

        ds->تبادلہ_Array32(ds, اندر_بائٹیں+جدول_آغاز_انحراف, چوٹی_سائز,
                            برآمد_بائٹیں+جدول_آغاز_انحراف, حالت_2);

        // Swap the state table if the table is in 16 bits.
        اگر (use8_بٹ) {
            اگر (برآمد_بائٹیں != اندر_بائٹیں) {
                uprv_memmove(برآمد_بائٹیں+جدول_آغاز_انحراف+چوٹی_سائز,
                             اندر_بائٹیں+جدول_آغاز_انحراف+چوٹی_سائز,
                             جدول_لمبائی-چوٹی_سائز);
            }
        } ورنہ {
            ds->تبادلہ_Array16(ds, اندر_بائٹیں+جدول_آغاز_انحراف+چوٹی_سائز, جدول_لمبائی-چوٹی_سائز,
                                برآمد_بائٹیں+جدول_آغاز_انحراف+چوٹی_سائز, حالت_2);
        }
    }

    // Trie table for character categories
    ucptrie_تبادلہ(ds, اندر_بائٹیں+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     برآمد_بائٹیں+ds->readUInt32(rbbiDH->fTrie), حالت_2);

    // Source Rules Text.  It's UTF8 data
    اگر (برآمد_بائٹیں != اندر_بائٹیں) {
        uprv_memmove(برآمد_بائٹیں+ds->readUInt32(rbbiDH->f_قاعدہ_ماخذ),
                     اندر_بائٹیں+ds->readUInt32(rbbiDH->f_قاعدہ_ماخذ),
                     ds->readUInt32(rbbiDH->f_قاعدہ_ماخذ_Len));
    }

    // Table of rule status values.  It's all int_32 values
    ds->تبادلہ_Array32(ds, اندر_بائٹیں+ds->readUInt32(rbbiDH->f_حالت_جدول), ds->readUInt32(rbbiDH->f_حالت_جدول_Len),
                        برآمد_بائٹیں+ds->readUInt32(rbbiDH->f_حالت_جدول), حالت_2);

    // And, last, the header.
    //   It is all int32_t values except for fFormataVersion, which is an array of four bytes.
    //   Swap the whole thing as int32_t, then re-swap the one field.
    //
    ds->تبادلہ_Array32(ds, اندر_بائٹیں, حجم(RBBIData_سرخی), برآمد_بائٹیں, حالت_2);
    ds->تبادلہ_Array32(ds, outputDH->f_Format_نسخہ, 4, outputDH->f_Format_نسخہ, حالت_2);

    واپس کل_سائز;
}


#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */
