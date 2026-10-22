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
RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIData_سرخی *انکڑے, UError_کوڈ &حالت) {
    init0();
    init(انکڑے, حالت);
}

RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIData_سرخی *انکڑے, تعداد EDontAdopt, UError_کوڈ &حالت) {
    init0();
    init(انکڑے, حالت);
    f_Dont_Free_انکڑے = سچ;
}

RBBIDataWrapper::RBBIDataWrapper(UData_حافظہ* udm, UError_کوڈ &حالت) {
    init0();
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    ثابت انکڑے_سرخی *dh = udm->p_سرخی;
    int32_t سرخی_ناپ = dh->انکڑے_سرخی_2.سرخی_ناپ;
    إذا (  !(سرخی_ناپ >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.انکڑے_Format[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.انکڑے_Format[1] == 0x72 &&
            dh->info.انکڑے_Format[2] == 0x6b &&
            dh->info.انکڑے_Format[3] == 0x20 &&
            is_انکڑے_نسخہ_Acceptable(dh->info.format_نسخہ))
        ) {
        حالت = U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    ثابت حرف *انکڑے_As_بائٹاں = reinterpret_cast<ثابت حرف *>(dh);
    ثابت RBBIData_سرخی *rbbidh = reinterpret_cast<ثابت RBBIData_سرخی *>(انکڑے_As_بائٹاں + سرخی_ناپ);
    init(rbbidh, حالت);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::is_انکڑے_نسخہ_Acceptable(ثابت UVersionInfo نسخہ) {
    أرجع RBBI_انکڑے_FORMAT_نسخہ[0] == نسخہ[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
فراغ RBBIDataWrapper::init0() {
    f_سرخی = خالی_سوچک;
    f_Forward_جدول = خالی_سوچک;
    f_الٹ_جدول = خالی_سوچک;
    f_قاعدہ_سروت   = خالی_سوچک;
    f_قاعدہ_حالت_جدول = خالی_سوچک;
    fTrie         = خالی_سوچک;
    fUDataMem     = خالی_سوچک;
    f_Ref_گنتی     = 0;
    f_Dont_Free_انکڑے = سچ;
}

فراغ RBBIDataWrapper::init(ثابت RBBIData_سرخی *انکڑے, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    f_سرخی = انکڑے;
    إذا (f_سرخی->fMagic != 0xb1a0 || !is_انکڑے_نسخہ_Acceptable(f_سرخی->f_Format_نسخہ)) {
        حالت = U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    f_Dont_Free_انکڑے = خطأ;
    إذا (انکڑے->fFTableLen != 0) {
        f_Forward_جدول = reinterpret_cast<ثابت RBBIState_جدول*>(reinterpret_cast<ثابت حرف*>(انکڑے) + f_سرخی->fFTable);
    }
    إذا (انکڑے->fRTableLen != 0) {
        f_الٹ_جدول = reinterpret_cast<ثابت RBBIState_جدول*>(reinterpret_cast<ثابت حرف*>(انکڑے) + f_سرخی->fRTable);
    }

    fTrie = ucptrie_open_From_ثنائی(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_قدر_بٹ_ANY,
                                   (uint8_t *)انکڑے + f_سرخی->fTrie,
                                   f_سرخی->fTrieLen,
                                   خالی_سوچک,           // *actual length
                                   &حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    UCPTrie_قدر_چوڑائی چوڑائی = ucptrie_get_قدر_چوڑائی(fTrie);
    إذا (!(چوڑائی == UCPTRIE_قدر_بٹ_8 || چوڑائی == UCPTRIE_قدر_بٹ_16)) {
        حالت = U_INVALID_FORMAT_ERROR;
        أرجع;
    }

    f_قاعدہ_سروت   = ((حرف *)انکڑے + f_سرخی->f_قاعدہ_سروت);
    f_قاعدہ_String = UnicodeString::fromUTF8(StringPiece(f_قاعدہ_سروت, f_سرخی->f_قاعدہ_سروت_Len));
    U_ASSERT(انکڑے->f_قاعدہ_سروت_Len > 0);

    f_قاعدہ_حالت_جدول = reinterpret_cast<ثابت int32_t*>(reinterpret_cast<ثابت حرف*>(انکڑے) + f_سرخی->f_حالت_جدول);
    f_حالت_Max_Idx    = انکڑے->f_حالت_جدول_Len / حجم(int32_t);

    f_Ref_گنتی = 1;

#جے_تعریف_شدہ RBBI_DEBUG
    حرف *debugEnv = getenv("U_RBBIDEBUG");
    إذا (debugEnv && uprv_strstr(debugEnv, "data")) {هذا->print_انکڑے();}
#جے_انت
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_گنتی == 0);
    ucptrie_close(fTrie);
    fTrie = خالی_سوچک;
    إذا (fUDataMem) {
        udata_close(fUDataMem);
    } وإلا إذا (!f_Dont_Free_انکڑے) {
        uprv_free((فراغ *)f_سرخی);
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
    إذا (f_سرخی == other.f_سرخی) {
        أرجع سچ;
    }
    إذا (f_سرخی->f_لمائی != other.f_سرخی->f_لمائی) {
        أرجع خطأ;
    }
    إذا (uprv_memcmp(f_سرخی, other.f_سرخی, f_سرخی->f_لمائی) == 0) {
        أرجع سچ;
    }
    أرجع خطأ;
}

int32_t  RBBIDataWrapper::hash_کوڈ() {
    أرجع f_سرخی->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
فراغ RBBIDataWrapper::removeReference() {
    إذا (umtx_atomic_dec(&f_Ref_گنتی) == 0) {
        مٹاؤ هذا;
    }
}


RBBIDataWrapper *RBBIDataWrapper::جوڑو_Reference() {
   umtx_atomic_inc(&f_Ref_گنتی);
   أرجع هذا;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ثابت UnicodeString &RBBIDataWrapper::get_قاعدہ_سروت_String() ثابت {
    أرجع f_قاعدہ_String;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#جے_تعریف_شدہ RBBI_DEBUG
فراغ  RBBIDataWrapper::print_جدول(ثابت حرف *heading, ثابت RBBIState_جدول *جدول) {
    uint32_t   c;
    uint32_t   s;

    RBBIDebugPrintf("%s\n", heading);

    RBBIDebugPrintf("   fDictCategoriesStart: %d\n", جدول->f_Dict_Categories_شروع);
    RBBIDebugPrintf("   fLookAheadResultsSize: %d\n", جدول->f_Look_Ahead_Results_ناپ);
    RBBIDebugPrintf("   Flags: %4x RBBI_LOOKAHEAD_HARD_BREAK=%s RBBI_8BITS_ROWS=%s\n",
                    جدول->fFlags,
                    جدول->fFlags & RBBI_LOOKAHEAD_HARD_BREAK ? "T" : "F",
                    جدول->fFlags & RBBI_8_بٹ_صفاں ? "T" : "F");
    RBBIDebugPrintf("\nState |  Acc  LA TagIx");
    لكل (c=0; c<f_سرخی->f_Cat_گنتی; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); لكل (c=0;c<f_سرخی->f_Cat_گنتی; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    إذا (جدول == خالی_سوچک) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        أرجع;
    }
    UBool use8_بٹ = جدول->fFlags & RBBI_8_بٹ_صفاں;
    لكل (s=0; s<جدول->f_عدد_حالتاں; s++) {
        RBBIState_جدول_صف *صف = (RBBIState_جدول_صف *)
                                  (جدول->f_جدول_انکڑے + (جدول->f_صف_Len * s));
        إذا (use8_بٹ) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, صف->r8.fAccepting, صف->r8.fLookAhead, صف->r8.f_ٹیگ_سوچی_Idx);
            لكل (c=0; c<f_سرخی->f_Cat_گنتی; c++)  {
                RBBIDebugPrintf("%3d ", صف->r8.f_اگلا_State[c]);
            }
        } وإلا {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, صف->r16.fAccepting, صف->r16.fLookAhead, صف->r16.f_ٹیگ_سوچی_Idx);
            لكل (c=0; c<f_سرخی->f_Cat_گنتی; c++)  {
                RBBIDebugPrintf("%3d ", صف->r16.f_اگلا_State[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#جے_انت


فراغ  RBBIDataWrapper::print_انکڑے() {
#جے_تعریف_شدہ RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (فراغ *)f_سرخی);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", f_سرخی->f_Format_نسخہ[0], f_سرخی->f_Format_نسخہ[1],
                                                    f_سرخی->f_Format_نسخہ[2], f_سرخی->f_Format_نسخہ[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", f_سرخی->f_لمائی);
    RBBIDebugPrintf("   number of character categories = %d\n\n", f_سرخی->f_Cat_گنتی);

    print_جدول("Forward State Transition Table", f_Forward_جدول);
    print_جدول("Reverse State Transition Table", f_الٹ_جدول);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    لكل (int32_t c=0; f_قاعدہ_سروت[c] != 0; c++) {
        RBBIDebugPrintf("%c", f_قاعدہ_سروت[c]);
    }
    RBBIDebugPrintf("\n\n");
#جے_انت
}


U_NAMESPACE_سرا
U_NAMESPACE_USE

//-----------------------------------------------------------------------------
//
//  ubrk_swap   -  byte swap and char encoding swap of RBBI data
//
//-----------------------------------------------------------------------------

U_CAPI int32_t U_EXPORT2
ubrk_وٹاندرا(ثابت UDataSwapper *ds, ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
           UError_کوڈ *حالت) {

    إذا (حالت == خالی_سوچک || U_FAILURE(*حالت)) {
        أرجع 0;
    }
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || length<-1 || (length>0 && باہرلا_انکڑے==خالی_سوچک)) {
        *حالت=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ثابت UDataInfo *pInfo = (ثابت UDataInfo *)((ثابت حرف *)وچ_انکڑے+4);
    إذا(!(  pInfo->انکڑے_Format[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->انکڑے_Format[1]==0x72 &&
           pInfo->انکڑے_Format[2]==0x6b &&
           pInfo->انکڑے_Format[3]==0x20 &&
           RBBIDataWrapper::is_انکڑے_نسخہ_Acceptable(pInfo->format_نسخہ) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->انکڑے_Format[0], pInfo->انکڑے_Format[1],
                         pInfo->انکڑے_Format[2], pInfo->انکڑے_Format[3],
                         pInfo->format_نسخہ[0]);
        *حالت=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    //
    // Swap the data header.  (This is the generic ICU Data Header, not the RBBI Specific
    //                         RBBIDataHeader).  This swap also conveniently gets us
    //                         the size of the ICU d.h., which lets us locate the start
    //                         of the RBBI specific data.
    //
    int32_t سرخی_ناپ=udata_وٹاندرا_انکڑے_سرخی(ds, وچ_انکڑے, length, باہرلا_انکڑے, حالت);


    //
    // Get the RRBI Data Header, and check that it appears to be OK.
    //
    ثابت uint8_t  *وچ_بائٹاں =(ثابت uint8_t *)وچ_انکڑے+سرخی_ناپ;
    RBBIData_سرخی *rbbiDH = (RBBIData_سرخی *)وچ_بائٹاں;
    إذا (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::is_انکڑے_نسخہ_Acceptable(rbbiDH->f_Format_نسخہ) ||
            ds->readUInt32(rbbiDH->f_لمائی)  <  حجم(RBBIData_سرخی)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *حالت=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t break_انکڑے_لمائی = ds->readUInt32(rbbiDH->f_لمائی);
    int32_t کل_ناپ = سرخی_ناپ + break_انکڑے_لمائی;
    إذا (length < 0) {
        أرجع کل_ناپ;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    إذا (length < کل_ناپ) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            break_انکڑے_لمائی);
        *حالت=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
        }


    //
    // Swap the Data.  Do the data itself first, then the RBBI Data Header, because
    //                 we need to reference the header to locate the data, and an
    //                 inplace swap of the header leaves it unusable.
    //
    uint8_t         *باہرلا_بائٹاں = (uint8_t *)باہرلا_انکڑے + سرخی_ناپ;
    RBBIData_سرخی  *outputDH = (RBBIData_سرخی *)باہرلا_بائٹاں;

    int32_t   جدول_شروع_وتھ;
    int32_t   جدول_لمائی;

    //
    // If not swapping in place, zero out the output buffer before starting.
    //    Individual tables and other data items within are aligned to 8 byte boundaries
    //    when originally created.  Any unused space between items needs to be zero.
    //
    إذا (وچ_بائٹاں != باہرلا_بائٹاں) {
        uprv_memset(باہرلا_بائٹاں, 0, break_انکڑے_لمائی);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         چوٹی_ناپ = offsetof(RBBIState_جدول, f_جدول_انکڑے);

    // Forward state table.
    جدول_شروع_وتھ = ds->readUInt32(rbbiDH->fFTable);
    جدول_لمائی      = ds->readUInt32(rbbiDH->fFTableLen);

    إذا (جدول_لمائی > 0) {
        RBBIState_جدول *rbbiST = (RBBIState_جدول *)(وچ_بائٹاں+جدول_شروع_وتھ);
        UBool use8_بٹ = ds->readUInt32(rbbiST->fFlags) & RBBI_8_بٹ_صفاں;

        ds->وٹاندرا_Array32(ds, وچ_بائٹاں+جدول_شروع_وتھ, چوٹی_ناپ,
                            باہرلا_بائٹاں+جدول_شروع_وتھ, حالت);

        // Swap the state table if the table is in 16 bits.
        إذا (use8_بٹ) {
            إذا (باہرلا_بائٹاں != وچ_بائٹاں) {
                uprv_memmove(باہرلا_بائٹاں+جدول_شروع_وتھ+چوٹی_ناپ,
                             وچ_بائٹاں+جدول_شروع_وتھ+چوٹی_ناپ,
                             جدول_لمائی-چوٹی_ناپ);
            }
        } وإلا {
            ds->وٹاندرا_Array16(ds, وچ_بائٹاں+جدول_شروع_وتھ+چوٹی_ناپ, جدول_لمائی-چوٹی_ناپ,
                                باہرلا_بائٹاں+جدول_شروع_وتھ+چوٹی_ناپ, حالت);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    جدول_شروع_وتھ = ds->readUInt32(rbbiDH->fRTable);
    جدول_لمائی      = ds->readUInt32(rbbiDH->fRTableLen);

    إذا (جدول_لمائی > 0) {
        RBBIState_جدول *rbbiST = (RBBIState_جدول *)(وچ_بائٹاں+جدول_شروع_وتھ);
        UBool use8_بٹ = ds->readUInt32(rbbiST->fFlags) & RBBI_8_بٹ_صفاں;

        ds->وٹاندرا_Array32(ds, وچ_بائٹاں+جدول_شروع_وتھ, چوٹی_ناپ,
                            باہرلا_بائٹاں+جدول_شروع_وتھ, حالت);

        // Swap the state table if the table is in 16 bits.
        إذا (use8_بٹ) {
            إذا (باہرلا_بائٹاں != وچ_بائٹاں) {
                uprv_memmove(باہرلا_بائٹاں+جدول_شروع_وتھ+چوٹی_ناپ,
                             وچ_بائٹاں+جدول_شروع_وتھ+چوٹی_ناپ,
                             جدول_لمائی-چوٹی_ناپ);
            }
        } وإلا {
            ds->وٹاندرا_Array16(ds, وچ_بائٹاں+جدول_شروع_وتھ+چوٹی_ناپ, جدول_لمائی-چوٹی_ناپ,
                                باہرلا_بائٹاں+جدول_شروع_وتھ+چوٹی_ناپ, حالت);
        }
    }

    // Trie table for character categories
    ucptrie_وٹاندرا(ds, وچ_بائٹاں+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     باہرلا_بائٹاں+ds->readUInt32(rbbiDH->fTrie), حالت);

    // Source Rules Text.  It's UTF8 data
    إذا (باہرلا_بائٹاں != وچ_بائٹاں) {
        uprv_memmove(باہرلا_بائٹاں+ds->readUInt32(rbbiDH->f_قاعدہ_سروت),
                     وچ_بائٹاں+ds->readUInt32(rbbiDH->f_قاعدہ_سروت),
                     ds->readUInt32(rbbiDH->f_قاعدہ_سروت_Len));
    }

    // Table of rule status values.  It's all int_32 values
    ds->وٹاندرا_Array32(ds, وچ_بائٹاں+ds->readUInt32(rbbiDH->f_حالت_جدول), ds->readUInt32(rbbiDH->f_حالت_جدول_Len),
                        باہرلا_بائٹاں+ds->readUInt32(rbbiDH->f_حالت_جدول), حالت);

    // And, last, the header.
    //   It is all int32_t values except for fFormataVersion, which is an array of four bytes.
    //   Swap the whole thing as int32_t, then re-swap the one field.
    //
    ds->وٹاندرا_Array32(ds, وچ_بائٹاں, حجم(RBBIData_سرخی), باہرلا_بائٹاں, حالت);
    ds->وٹاندرا_Array32(ds, outputDH->f_Format_نسخہ, 4, outputDH->f_Format_نسخہ, حالت);

    أرجع کل_ناپ;
}


#جے_انت /* #if !UCONFIG_NO_BREAK_ITERATION */
