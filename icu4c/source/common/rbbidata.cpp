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
RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIData_سرلیک *معلومات, UError_کوډ &حالت) {
    init0();
    init(معلومات, حالت);
}

RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIData_سرلیک *معلومات, تعداد EDontAdopt, UError_کوډ &حالت) {
    init0();
    init(معلومات, حالت);
    f_Dont_Free_معلومات = ریښتیا;
}

RBBIDataWrapper::RBBIDataWrapper(UData_حافظه* udm, UError_کوډ &حالت) {
    init0();
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    ثابت معلومات_سرلیک *dh = udm->p_سرلیک;
    int32_t سرلیک_اندازه = dh->معلومات_سرلیک_2.سرلیک_اندازه;
    إذا (  !(سرلیک_اندازه >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.معلومات_Format[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.معلومات_Format[1] == 0x72 &&
            dh->info.معلومات_Format[2] == 0x6b &&
            dh->info.معلومات_Format[3] == 0x20 &&
            is_معلومات_نسخه_Acceptable(dh->info.format_نسخه))
        ) {
        حالت = U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    ثابت حرف *معلومات_As_بایټونه = reinterpret_cast<ثابت حرف *>(dh);
    ثابت RBBIData_سرلیک *rbbidh = reinterpret_cast<ثابت RBBIData_سرلیک *>(معلومات_As_بایټونه + سرلیک_اندازه);
    init(rbbidh, حالت);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::is_معلومات_نسخه_Acceptable(ثابت UVersionInfo نسخه) {
    أرجع RBBI_معلومات_FORMAT_نسخه[0] == نسخه[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
فراغ RBBIDataWrapper::init0() {
    f_سرلیک = تش_اشاره;
    f_Forward_جدول = تش_اشاره;
    f_سرچپه_جدول = تش_اشاره;
    f_قاعده_سرچینه   = تش_اشاره;
    f_قاعده_حالت_جدول = تش_اشاره;
    fTrie         = تش_اشاره;
    fUDataMem     = تش_اشاره;
    f_Ref_شمېر     = 0;
    f_Dont_Free_معلومات = ریښتیا;
}

فراغ RBBIDataWrapper::init(ثابت RBBIData_سرلیک *معلومات, UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    f_سرلیک = معلومات;
    إذا (f_سرلیک->fMagic != 0xb1a0 || !is_معلومات_نسخه_Acceptable(f_سرلیک->f_Format_نسخه)) {
        حالت = U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    f_Dont_Free_معلومات = خطأ;
    إذا (معلومات->fFTableLen != 0) {
        f_Forward_جدول = reinterpret_cast<ثابت RBBIState_جدول*>(reinterpret_cast<ثابت حرف*>(معلومات) + f_سرلیک->fFTable);
    }
    إذا (معلومات->fRTableLen != 0) {
        f_سرچپه_جدول = reinterpret_cast<ثابت RBBIState_جدول*>(reinterpret_cast<ثابت حرف*>(معلومات) + f_سرلیک->fRTable);
    }

    fTrie = ucptrie_open_From_دوګونی(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_ارزښت_بټونه_ANY,
                                   (uint8_t *)معلومات + f_سرلیک->fTrie,
                                   f_سرلیک->fTrieLen,
                                   تش_اشاره,           // *actual length
                                   &حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    UCPTrie_ارزښت_پلنوالی پلنوالی = ucptrie_get_ارزښت_پلنوالی(fTrie);
    إذا (!(پلنوالی == UCPTRIE_ارزښت_بټونه_8 || پلنوالی == UCPTRIE_ارزښت_بټونه_16)) {
        حالت = U_INVALID_FORMAT_ERROR;
        أرجع;
    }

    f_قاعده_سرچینه   = ((حرف *)معلومات + f_سرلیک->f_قاعده_سرچینه);
    f_قاعده_String = UnicodeString::fromUTF8(StringPiece(f_قاعده_سرچینه, f_سرلیک->f_قاعده_سرچینه_Len));
    U_ASSERT(معلومات->f_قاعده_سرچینه_Len > 0);

    f_قاعده_حالت_جدول = reinterpret_cast<ثابت int32_t*>(reinterpret_cast<ثابت حرف*>(معلومات) + f_سرلیک->f_حالت_جدول);
    f_حالت_Max_Idx    = معلومات->f_حالت_جدول_Len / حجم(int32_t);

    f_Ref_شمېر = 1;

#که_تعریف_شوی RBBI_DEBUG
    حرف *debugEnv = getenv("U_RBBIDEBUG");
    إذا (debugEnv && uprv_strstr(debugEnv, "data")) {هذا->print_معلومات();}
#د_که_پای
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_شمېر == 0);
    ucptrie_close(fTrie);
    fTrie = تش_اشاره;
    إذا (fUDataMem) {
        udata_close(fUDataMem);
    } وإلا إذا (!f_Dont_Free_معلومات) {
        uprv_free((فراغ *)f_سرلیک);
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
منطقي RBBIDataWrapper::چلوونکی ==(ثابت RBBIDataWrapper &other) ثابت {
    إذا (f_سرلیک == other.f_سرلیک) {
        أرجع ریښتیا;
    }
    إذا (f_سرلیک->f_اوږدتیا != other.f_سرلیک->f_اوږدتیا) {
        أرجع خطأ;
    }
    إذا (uprv_memcmp(f_سرلیک, other.f_سرلیک, f_سرلیک->f_اوږدتیا) == 0) {
        أرجع ریښتیا;
    }
    أرجع خطأ;
}

int32_t  RBBIDataWrapper::hash_کوډ() {
    أرجع f_سرلیک->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
فراغ RBBIDataWrapper::removeReference() {
    إذا (umtx_atomic_dec(&f_Ref_شمېر) == 0) {
        ړنګول هذا;
    }
}


RBBIDataWrapper *RBBIDataWrapper::جمع_کړه_Reference() {
   umtx_atomic_inc(&f_Ref_شمېر);
   أرجع هذا;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ثابت UnicodeString &RBBIDataWrapper::get_قاعده_سرچینه_String() ثابت {
    أرجع f_قاعده_String;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#که_تعریف_شوی RBBI_DEBUG
فراغ  RBBIDataWrapper::print_جدول(ثابت حرف *heading, ثابت RBBIState_جدول *جدول) {
    uint32_t   c;
    uint32_t   s;

    RBBIDebugPrintf("%s\n", heading);

    RBBIDebugPrintf("   fDictCategoriesStart: %d\n", جدول->f_Dict_Categories_پیل);
    RBBIDebugPrintf("   fLookAheadResultsSize: %d\n", جدول->f_Look_Ahead_Results_اندازه);
    RBBIDebugPrintf("   Flags: %4x RBBI_LOOKAHEAD_HARD_BREAK=%s RBBI_8BITS_ROWS=%s\n",
                    جدول->fFlags,
                    جدول->fFlags & RBBI_LOOKAHEAD_HARD_BREAK ? "T" : "F",
                    جدول->fFlags & RBBI_8_بټونه_لیکې ? "T" : "F");
    RBBIDebugPrintf("\nState |  Acc  LA TagIx");
    لكل (c=0; c<f_سرلیک->f_Cat_شمېر; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); لكل (c=0;c<f_سرلیک->f_Cat_شمېر; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    إذا (جدول == تش_اشاره) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        أرجع;
    }
    UBool use8_بټونه = جدول->fFlags & RBBI_8_بټونه_لیکې;
    لكل (s=0; s<جدول->f_عدد_حالتونه; s++) {
        RBBIState_جدول_لیکه *لیکه = (RBBIState_جدول_لیکه *)
                                  (جدول->f_جدول_معلومات + (جدول->f_لیکه_Len * s));
        إذا (use8_بټونه) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, لیکه->r8.fAccepting, لیکه->r8.fLookAhead, لیکه->r8.f_نښانونه_Idx);
            لكل (c=0; c<f_سرلیک->f_Cat_شمېر; c++)  {
                RBBIDebugPrintf("%3d ", لیکه->r8.f_بل_State[c]);
            }
        } وإلا {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, لیکه->r16.fAccepting, لیکه->r16.fLookAhead, لیکه->r16.f_نښانونه_Idx);
            لكل (c=0; c<f_سرلیک->f_Cat_شمېر; c++)  {
                RBBIDebugPrintf("%3d ", لیکه->r16.f_بل_State[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#د_که_پای


فراغ  RBBIDataWrapper::print_معلومات() {
#که_تعریف_شوی RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (فراغ *)f_سرلیک);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", f_سرلیک->f_Format_نسخه[0], f_سرلیک->f_Format_نسخه[1],
                                                    f_سرلیک->f_Format_نسخه[2], f_سرلیک->f_Format_نسخه[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", f_سرلیک->f_اوږدتیا);
    RBBIDebugPrintf("   number of character categories = %d\n\n", f_سرلیک->f_Cat_شمېر);

    print_جدول("Forward State Transition Table", f_Forward_جدول);
    print_جدول("Reverse State Transition Table", f_سرچپه_جدول);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    لكل (int32_t c=0; f_قاعده_سرچینه[c] != 0; c++) {
        RBBIDebugPrintf("%c", f_قاعده_سرچینه[c]);
    }
    RBBIDebugPrintf("\n\n");
#د_که_پای
}


U_NAMESPACE_پای_ټکی
U_NAMESPACE_USE

//-----------------------------------------------------------------------------
//
//  ubrk_swap   -  byte swap and char encoding swap of RBBI data
//
//-----------------------------------------------------------------------------

U_CAPI int32_t U_EXPORT2
ubrk_بدلون(ثابت UDataSwapper *ds, ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
           UError_کوډ *حالت) {

    إذا (حالت == تش_اشاره || U_FAILURE(*حالت)) {
        أرجع 0;
    }
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || length<-1 || (length>0 && بهر_معلومات==تش_اشاره)) {
        *حالت=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ثابت UDataInfo *pInfo = (ثابت UDataInfo *)((ثابت حرف *)دننه_معلومات+4);
    إذا(!(  pInfo->معلومات_Format[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->معلومات_Format[1]==0x72 &&
           pInfo->معلومات_Format[2]==0x6b &&
           pInfo->معلومات_Format[3]==0x20 &&
           RBBIDataWrapper::is_معلومات_نسخه_Acceptable(pInfo->format_نسخه) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخه[0]);
        *حالت=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    //
    // Swap the data header.  (This is the generic ICU Data Header, not the RBBI Specific
    //                         RBBIDataHeader).  This swap also conveniently gets us
    //                         the size of the ICU d.h., which lets us locate the start
    //                         of the RBBI specific data.
    //
    int32_t سرلیک_اندازه=udata_بدلون_معلومات_سرلیک(ds, دننه_معلومات, length, بهر_معلومات, حالت);


    //
    // Get the RRBI Data Header, and check that it appears to be OK.
    //
    ثابت uint8_t  *دننه_بایټونه =(ثابت uint8_t *)دننه_معلومات+سرلیک_اندازه;
    RBBIData_سرلیک *rbbiDH = (RBBIData_سرلیک *)دننه_بایټونه;
    إذا (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::is_معلومات_نسخه_Acceptable(rbbiDH->f_Format_نسخه) ||
            ds->readUInt32(rbbiDH->f_اوږدتیا)  <  حجم(RBBIData_سرلیک)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *حالت=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t break_معلومات_اوږدتیا = ds->readUInt32(rbbiDH->f_اوږدتیا);
    int32_t ټول_اندازه = سرلیک_اندازه + break_معلومات_اوږدتیا;
    إذا (length < 0) {
        أرجع ټول_اندازه;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    إذا (length < ټول_اندازه) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            break_معلومات_اوږدتیا);
        *حالت=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
        }


    //
    // Swap the Data.  Do the data itself first, then the RBBI Data Header, because
    //                 we need to reference the header to locate the data, and an
    //                 inplace swap of the header leaves it unusable.
    //
    uint8_t         *بهر_بایټونه = (uint8_t *)بهر_معلومات + سرلیک_اندازه;
    RBBIData_سرلیک  *outputDH = (RBBIData_سرلیک *)بهر_بایټونه;

    int32_t   جدول_پیل_دننه_واټن;
    int32_t   جدول_اوږدتیا;

    //
    // If not swapping in place, zero out the output buffer before starting.
    //    Individual tables and other data items within are aligned to 8 byte boundaries
    //    when originally created.  Any unused space between items needs to be zero.
    //
    إذا (دننه_بایټونه != بهر_بایټونه) {
        uprv_memset(بهر_بایټونه, 0, break_معلومات_اوږدتیا);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         سر_اندازه = offsetof(RBBIState_جدول, f_جدول_معلومات);

    // Forward state table.
    جدول_پیل_دننه_واټن = ds->readUInt32(rbbiDH->fFTable);
    جدول_اوږدتیا      = ds->readUInt32(rbbiDH->fFTableLen);

    إذا (جدول_اوږدتیا > 0) {
        RBBIState_جدول *rbbiST = (RBBIState_جدول *)(دننه_بایټونه+جدول_پیل_دننه_واټن);
        UBool use8_بټونه = ds->readUInt32(rbbiST->fFlags) & RBBI_8_بټونه_لیکې;

        ds->بدلون_Array32(ds, دننه_بایټونه+جدول_پیل_دننه_واټن, سر_اندازه,
                            بهر_بایټونه+جدول_پیل_دننه_واټن, حالت);

        // Swap the state table if the table is in 16 bits.
        إذا (use8_بټونه) {
            إذا (بهر_بایټونه != دننه_بایټونه) {
                uprv_memmove(بهر_بایټونه+جدول_پیل_دننه_واټن+سر_اندازه,
                             دننه_بایټونه+جدول_پیل_دننه_واټن+سر_اندازه,
                             جدول_اوږدتیا-سر_اندازه);
            }
        } وإلا {
            ds->بدلون_Array16(ds, دننه_بایټونه+جدول_پیل_دننه_واټن+سر_اندازه, جدول_اوږدتیا-سر_اندازه,
                                بهر_بایټونه+جدول_پیل_دننه_واټن+سر_اندازه, حالت);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    جدول_پیل_دننه_واټن = ds->readUInt32(rbbiDH->fRTable);
    جدول_اوږدتیا      = ds->readUInt32(rbbiDH->fRTableLen);

    إذا (جدول_اوږدتیا > 0) {
        RBBIState_جدول *rbbiST = (RBBIState_جدول *)(دننه_بایټونه+جدول_پیل_دننه_واټن);
        UBool use8_بټونه = ds->readUInt32(rbbiST->fFlags) & RBBI_8_بټونه_لیکې;

        ds->بدلون_Array32(ds, دننه_بایټونه+جدول_پیل_دننه_واټن, سر_اندازه,
                            بهر_بایټونه+جدول_پیل_دننه_واټن, حالت);

        // Swap the state table if the table is in 16 bits.
        إذا (use8_بټونه) {
            إذا (بهر_بایټونه != دننه_بایټونه) {
                uprv_memmove(بهر_بایټونه+جدول_پیل_دننه_واټن+سر_اندازه,
                             دننه_بایټونه+جدول_پیل_دننه_واټن+سر_اندازه,
                             جدول_اوږدتیا-سر_اندازه);
            }
        } وإلا {
            ds->بدلون_Array16(ds, دننه_بایټونه+جدول_پیل_دننه_واټن+سر_اندازه, جدول_اوږدتیا-سر_اندازه,
                                بهر_بایټونه+جدول_پیل_دننه_واټن+سر_اندازه, حالت);
        }
    }

    // Trie table for character categories
    ucptrie_بدلون(ds, دننه_بایټونه+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     بهر_بایټونه+ds->readUInt32(rbbiDH->fTrie), حالت);

    // Source Rules Text.  It's UTF8 data
    إذا (بهر_بایټونه != دننه_بایټونه) {
        uprv_memmove(بهر_بایټونه+ds->readUInt32(rbbiDH->f_قاعده_سرچینه),
                     دننه_بایټونه+ds->readUInt32(rbbiDH->f_قاعده_سرچینه),
                     ds->readUInt32(rbbiDH->f_قاعده_سرچینه_Len));
    }

    // Table of rule status values.  It's all int_32 values
    ds->بدلون_Array32(ds, دننه_بایټونه+ds->readUInt32(rbbiDH->f_حالت_جدول), ds->readUInt32(rbbiDH->f_حالت_جدول_Len),
                        بهر_بایټونه+ds->readUInt32(rbbiDH->f_حالت_جدول), حالت);

    // And, last, the header.
    //   It is all int32_t values except for fFormataVersion, which is an array of four bytes.
    //   Swap the whole thing as int32_t, then re-swap the one field.
    //
    ds->بدلون_Array32(ds, دننه_بایټونه, حجم(RBBIData_سرلیک), بهر_بایټونه, حالت);
    ds->بدلون_Array32(ds, outputDH->f_Format_نسخه, 4, outputDH->f_Format_نسخه, حالت);

    أرجع ټول_اندازه;
}


#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */
