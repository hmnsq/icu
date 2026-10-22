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
RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIData_سرآیند *داده, UError_کد &وضعیت) {
    init0();
    init(داده, وضعیت);
}

RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIData_سرآیند *داده, تعداد EDontAdopt, UError_کد &وضعیت) {
    init0();
    init(داده, وضعیت);
    f_Dont_Free_داده = درست;
}

RBBIDataWrapper::RBBIDataWrapper(UData_حافظه* udm, UError_کد &وضعیت) {
    init0();
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    ثابت داده_سرآیند *dh = udm->p_سرآیند;
    int32_t سرآیند_اندازه = dh->داده_سرآیند_2.سرآیند_اندازه;
    إذا (  !(سرآیند_اندازه >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.داده_Format[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.داده_Format[1] == 0x72 &&
            dh->info.داده_Format[2] == 0x6b &&
            dh->info.داده_Format[3] == 0x20 &&
            is_داده_نسخه_Acceptable(dh->info.format_نسخه))
        ) {
        وضعیت = U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    ثابت حرف *داده_As_بایتها = reinterpret_cast<ثابت حرف *>(dh);
    ثابت RBBIData_سرآیند *rbbidh = reinterpret_cast<ثابت RBBIData_سرآیند *>(داده_As_بایتها + سرآیند_اندازه);
    init(rbbidh, وضعیت);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::is_داده_نسخه_Acceptable(ثابت UVersionInfo نسخه) {
    أرجع RBBI_داده_FORMAT_نسخه[0] == نسخه[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
فراغ RBBIDataWrapper::init0() {
    f_سرآیند = اشاره_تهی;
    f_Forward_جدول = اشاره_تهی;
    f_معکوس_جدول = اشاره_تهی;
    f_قاعده_منبع   = اشاره_تهی;
    f_قاعده_وضعیت_جدول = اشاره_تهی;
    fTrie         = اشاره_تهی;
    fUDataMem     = اشاره_تهی;
    f_Ref_شمار     = 0;
    f_Dont_Free_داده = درست;
}

فراغ RBBIDataWrapper::init(ثابت RBBIData_سرآیند *داده, UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    f_سرآیند = داده;
    إذا (f_سرآیند->fMagic != 0xb1a0 || !is_داده_نسخه_Acceptable(f_سرآیند->f_Format_نسخه)) {
        وضعیت = U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    f_Dont_Free_داده = خطأ;
    إذا (داده->fFTableLen != 0) {
        f_Forward_جدول = reinterpret_cast<ثابت RBBIState_جدول*>(reinterpret_cast<ثابت حرف*>(داده) + f_سرآیند->fFTable);
    }
    إذا (داده->fRTableLen != 0) {
        f_معکوس_جدول = reinterpret_cast<ثابت RBBIState_جدول*>(reinterpret_cast<ثابت حرف*>(داده) + f_سرآیند->fRTable);
    }

    fTrie = ucptrie_open_From_دودویی(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_مقدار_بیتها_ANY,
                                   (uint8_t *)داده + f_سرآیند->fTrie,
                                   f_سرآیند->fTrieLen,
                                   اشاره_تهی,           // *actual length
                                   &وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    UCPTrie_مقدار_عرض عرض = ucptrie_get_مقدار_عرض(fTrie);
    إذا (!(عرض == UCPTRIE_مقدار_بیتها_8 || عرض == UCPTRIE_مقدار_بیتها_16)) {
        وضعیت = U_INVALID_FORMAT_ERROR;
        أرجع;
    }

    f_قاعده_منبع   = ((حرف *)داده + f_سرآیند->f_قاعده_منبع);
    f_قاعده_String = UnicodeString::fromUTF8(StringPiece(f_قاعده_منبع, f_سرآیند->f_قاعده_منبع_Len));
    U_ASSERT(داده->f_قاعده_منبع_Len > 0);

    f_قاعده_وضعیت_جدول = reinterpret_cast<ثابت int32_t*>(reinterpret_cast<ثابت حرف*>(داده) + f_سرآیند->f_وضعیت_جدول);
    f_وضعیت_Max_Idx    = داده->f_وضعیت_جدول_Len / حجم(int32_t);

    f_Ref_شمار = 1;

#اگر_تعریف_شده RBBI_DEBUG
    حرف *debugEnv = getenv("U_RBBIDEBUG");
    إذا (debugEnv && uprv_strstr(debugEnv, "data")) {هذا->print_داده();}
#پایان_اگر
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_شمار == 0);
    ucptrie_close(fTrie);
    fTrie = اشاره_تهی;
    إذا (fUDataMem) {
        udata_close(fUDataMem);
    } وإلا إذا (!f_Dont_Free_داده) {
        uprv_free((فراغ *)f_سرآیند);
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
منطقي RBBIDataWrapper::عملگر ==(ثابت RBBIDataWrapper &other) ثابت {
    إذا (f_سرآیند == other.f_سرآیند) {
        أرجع درست;
    }
    إذا (f_سرآیند->f_درازی != other.f_سرآیند->f_درازی) {
        أرجع خطأ;
    }
    إذا (uprv_memcmp(f_سرآیند, other.f_سرآیند, f_سرآیند->f_درازی) == 0) {
        أرجع درست;
    }
    أرجع خطأ;
}

int32_t  RBBIDataWrapper::hash_کد() {
    أرجع f_سرآیند->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
فراغ RBBIDataWrapper::removeReference() {
    إذا (umtx_atomic_dec(&f_Ref_شمار) == 0) {
        حذف هذا;
    }
}


RBBIDataWrapper *RBBIDataWrapper::جمع_کن_Reference() {
   umtx_atomic_inc(&f_Ref_شمار);
   أرجع هذا;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ثابت UnicodeString &RBBIDataWrapper::get_قاعده_منبع_String() ثابت {
    أرجع f_قاعده_String;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#اگر_تعریف_شده RBBI_DEBUG
فراغ  RBBIDataWrapper::print_جدول(ثابت حرف *heading, ثابت RBBIState_جدول *جدول) {
    uint32_t   c;
    uint32_t   s;

    RBBIDebugPrintf("%s\n", heading);

    RBBIDebugPrintf("   fDictCategoriesStart: %d\n", جدول->f_Dict_Categories_شروع);
    RBBIDebugPrintf("   fLookAheadResultsSize: %d\n", جدول->f_Look_Ahead_Results_اندازه);
    RBBIDebugPrintf("   Flags: %4x RBBI_LOOKAHEAD_HARD_BREAK=%s RBBI_8BITS_ROWS=%s\n",
                    جدول->fFlags,
                    جدول->fFlags & RBBI_LOOKAHEAD_HARD_BREAK ? "T" : "F",
                    جدول->fFlags & RBBI_8_بیتها_ردیفها ? "T" : "F");
    RBBIDebugPrintf("\nState |  Acc  LA TagIx");
    لكل (c=0; c<f_سرآیند->f_Cat_شمار; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); لكل (c=0;c<f_سرآیند->f_Cat_شمار; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    إذا (جدول == اشاره_تهی) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        أرجع;
    }
    UBool use8_بیتها = جدول->fFlags & RBBI_8_بیتها_ردیفها;
    لكل (s=0; s<جدول->f_رقم_حالتها; s++) {
        RBBIState_جدول_ردیف *ردیف = (RBBIState_جدول_ردیف *)
                                  (جدول->f_جدول_داده + (جدول->f_ردیف_Len * s));
        إذا (use8_بیتها) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, ردیف->r8.fAccepting, ردیف->r8.fLookAhead, ردیف->r8.f_برچسبها_Idx);
            لكل (c=0; c<f_سرآیند->f_Cat_شمار; c++)  {
                RBBIDebugPrintf("%3d ", ردیف->r8.f_بعدی_State[c]);
            }
        } وإلا {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, ردیف->r16.fAccepting, ردیف->r16.fLookAhead, ردیف->r16.f_برچسبها_Idx);
            لكل (c=0; c<f_سرآیند->f_Cat_شمار; c++)  {
                RBBIDebugPrintf("%3d ", ردیف->r16.f_بعدی_State[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#پایان_اگر


فراغ  RBBIDataWrapper::print_داده() {
#اگر_تعریف_شده RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (فراغ *)f_سرآیند);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", f_سرآیند->f_Format_نسخه[0], f_سرآیند->f_Format_نسخه[1],
                                                    f_سرآیند->f_Format_نسخه[2], f_سرآیند->f_Format_نسخه[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", f_سرآیند->f_درازی);
    RBBIDebugPrintf("   number of character categories = %d\n\n", f_سرآیند->f_Cat_شمار);

    print_جدول("Forward State Transition Table", f_Forward_جدول);
    print_جدول("Reverse State Transition Table", f_معکوس_جدول);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    لكل (int32_t c=0; f_قاعده_منبع[c] != 0; c++) {
        RBBIDebugPrintf("%c", f_قاعده_منبع[c]);
    }
    RBBIDebugPrintf("\n\n");
#پایان_اگر
}


U_NAMESPACE_انتها
U_NAMESPACE_USE

//-----------------------------------------------------------------------------
//
//  ubrk_swap   -  byte swap and char encoding swap of RBBI data
//
//-----------------------------------------------------------------------------

U_CAPI int32_t U_EXPORT2
ubrk_جابجا(ثابت UDataSwapper *ds, ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
           UError_کد *وضعیت) {

    إذا (وضعیت == اشاره_تهی || U_FAILURE(*وضعیت)) {
        أرجع 0;
    }
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || length<-1 || (length>0 && بیرون_داده==اشاره_تهی)) {
        *وضعیت=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ثابت UDataInfo *pInfo = (ثابت UDataInfo *)((ثابت حرف *)درون_داده+4);
    إذا(!(  pInfo->داده_Format[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->داده_Format[1]==0x72 &&
           pInfo->داده_Format[2]==0x6b &&
           pInfo->داده_Format[3]==0x20 &&
           RBBIDataWrapper::is_داده_نسخه_Acceptable(pInfo->format_نسخه) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->داده_Format[0], pInfo->داده_Format[1],
                         pInfo->داده_Format[2], pInfo->داده_Format[3],
                         pInfo->format_نسخه[0]);
        *وضعیت=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    //
    // Swap the data header.  (This is the generic ICU Data Header, not the RBBI Specific
    //                         RBBIDataHeader).  This swap also conveniently gets us
    //                         the size of the ICU d.h., which lets us locate the start
    //                         of the RBBI specific data.
    //
    int32_t سرآیند_اندازه=udata_جابجا_داده_سرآیند(ds, درون_داده, length, بیرون_داده, وضعیت);


    //
    // Get the RRBI Data Header, and check that it appears to be OK.
    //
    ثابت uint8_t  *درون_بایتها =(ثابت uint8_t *)درون_داده+سرآیند_اندازه;
    RBBIData_سرآیند *rbbiDH = (RBBIData_سرآیند *)درون_بایتها;
    إذا (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::is_داده_نسخه_Acceptable(rbbiDH->f_Format_نسخه) ||
            ds->readUInt32(rbbiDH->f_درازی)  <  حجم(RBBIData_سرآیند)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *وضعیت=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t break_داده_درازی = ds->readUInt32(rbbiDH->f_درازی);
    int32_t مجموع_اندازه = سرآیند_اندازه + break_داده_درازی;
    إذا (length < 0) {
        أرجع مجموع_اندازه;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    إذا (length < مجموع_اندازه) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            break_داده_درازی);
        *وضعیت=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
        }


    //
    // Swap the Data.  Do the data itself first, then the RBBI Data Header, because
    //                 we need to reference the header to locate the data, and an
    //                 inplace swap of the header leaves it unusable.
    //
    uint8_t         *بیرون_بایتها = (uint8_t *)بیرون_داده + سرآیند_اندازه;
    RBBIData_سرآیند  *outputDH = (RBBIData_سرآیند *)بیرون_بایتها;

    int32_t   جدول_شروع_جابه_جایی;
    int32_t   جدول_درازی;

    //
    // If not swapping in place, zero out the output buffer before starting.
    //    Individual tables and other data items within are aligned to 8 byte boundaries
    //    when originally created.  Any unused space between items needs to be zero.
    //
    إذا (درون_بایتها != بیرون_بایتها) {
        uprv_memset(بیرون_بایتها, 0, break_داده_درازی);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         بالاترین_اندازه = offsetof(RBBIState_جدول, f_جدول_داده);

    // Forward state table.
    جدول_شروع_جابه_جایی = ds->readUInt32(rbbiDH->fFTable);
    جدول_درازی      = ds->readUInt32(rbbiDH->fFTableLen);

    إذا (جدول_درازی > 0) {
        RBBIState_جدول *rbbiST = (RBBIState_جدول *)(درون_بایتها+جدول_شروع_جابه_جایی);
        UBool use8_بیتها = ds->readUInt32(rbbiST->fFlags) & RBBI_8_بیتها_ردیفها;

        ds->جابجا_Array32(ds, درون_بایتها+جدول_شروع_جابه_جایی, بالاترین_اندازه,
                            بیرون_بایتها+جدول_شروع_جابه_جایی, وضعیت);

        // Swap the state table if the table is in 16 bits.
        إذا (use8_بیتها) {
            إذا (بیرون_بایتها != درون_بایتها) {
                uprv_memmove(بیرون_بایتها+جدول_شروع_جابه_جایی+بالاترین_اندازه,
                             درون_بایتها+جدول_شروع_جابه_جایی+بالاترین_اندازه,
                             جدول_درازی-بالاترین_اندازه);
            }
        } وإلا {
            ds->جابجا_Array16(ds, درون_بایتها+جدول_شروع_جابه_جایی+بالاترین_اندازه, جدول_درازی-بالاترین_اندازه,
                                بیرون_بایتها+جدول_شروع_جابه_جایی+بالاترین_اندازه, وضعیت);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    جدول_شروع_جابه_جایی = ds->readUInt32(rbbiDH->fRTable);
    جدول_درازی      = ds->readUInt32(rbbiDH->fRTableLen);

    إذا (جدول_درازی > 0) {
        RBBIState_جدول *rbbiST = (RBBIState_جدول *)(درون_بایتها+جدول_شروع_جابه_جایی);
        UBool use8_بیتها = ds->readUInt32(rbbiST->fFlags) & RBBI_8_بیتها_ردیفها;

        ds->جابجا_Array32(ds, درون_بایتها+جدول_شروع_جابه_جایی, بالاترین_اندازه,
                            بیرون_بایتها+جدول_شروع_جابه_جایی, وضعیت);

        // Swap the state table if the table is in 16 bits.
        إذا (use8_بیتها) {
            إذا (بیرون_بایتها != درون_بایتها) {
                uprv_memmove(بیرون_بایتها+جدول_شروع_جابه_جایی+بالاترین_اندازه,
                             درون_بایتها+جدول_شروع_جابه_جایی+بالاترین_اندازه,
                             جدول_درازی-بالاترین_اندازه);
            }
        } وإلا {
            ds->جابجا_Array16(ds, درون_بایتها+جدول_شروع_جابه_جایی+بالاترین_اندازه, جدول_درازی-بالاترین_اندازه,
                                بیرون_بایتها+جدول_شروع_جابه_جایی+بالاترین_اندازه, وضعیت);
        }
    }

    // Trie table for character categories
    ucptrie_جابجا(ds, درون_بایتها+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     بیرون_بایتها+ds->readUInt32(rbbiDH->fTrie), وضعیت);

    // Source Rules Text.  It's UTF8 data
    إذا (بیرون_بایتها != درون_بایتها) {
        uprv_memmove(بیرون_بایتها+ds->readUInt32(rbbiDH->f_قاعده_منبع),
                     درون_بایتها+ds->readUInt32(rbbiDH->f_قاعده_منبع),
                     ds->readUInt32(rbbiDH->f_قاعده_منبع_Len));
    }

    // Table of rule status values.  It's all int_32 values
    ds->جابجا_Array32(ds, درون_بایتها+ds->readUInt32(rbbiDH->f_وضعیت_جدول), ds->readUInt32(rbbiDH->f_وضعیت_جدول_Len),
                        بیرون_بایتها+ds->readUInt32(rbbiDH->f_وضعیت_جدول), وضعیت);

    // And, last, the header.
    //   It is all int32_t values except for fFormataVersion, which is an array of four bytes.
    //   Swap the whole thing as int32_t, then re-swap the one field.
    //
    ds->جابجا_Array32(ds, درون_بایتها, حجم(RBBIData_سرآیند), بیرون_بایتها, وضعیت);
    ds->جابجا_Array32(ds, outputDH->f_Format_نسخه, 4, outputDH->f_Format_نسخه, وضعیت);

    أرجع مجموع_اندازه;
}


#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */
