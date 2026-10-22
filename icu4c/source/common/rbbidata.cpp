// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 1999-2014 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব "unicode/ucptrie.h"
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "rbbidata.h"
#অন্তর্ভাব "rbbirb.h"
#অন্তর্ভাব "udatamem.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "umutex.h"

#অন্তর্ভাব "uassert.h"


U_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//    Constructors.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::RBBIDataWrapper(ধ্রুবক RBBIDataHeader *তথ্য, UErrorCode &status) {
    init0();
    init(তথ্য, status);
}

RBBIDataWrapper::RBBIDataWrapper(ধ্রুবক RBBIDataHeader *তথ্য, গণনা EDontAdopt, UErrorCode &status) {
    init0();
    init(তথ্য, status);
    f_Dont_Free_তথ্য = সত্য;
}

RBBIDataWrapper::RBBIDataWrapper(UDataMemory* udm, UErrorCode &status) {
    init0();
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    ধ্রুবক তথ্য_Header *dh = udm->pHeader;
    int32_t header_আকার = dh->তথ্য_Header_2.header_আকার;
    যদি (  !(header_আকার >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.তথ্য_Format[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.তথ্য_Format[1] == 0x72 &&
            dh->info.তথ্য_Format[2] == 0x6b &&
            dh->info.তথ্য_Format[3] == 0x20 &&
            is_তথ্য_Version_Acceptable(dh->info.formatVersion))
        ) {
        status = U_INVALID_FORMAT_ERROR;
        ফেরত;
    }
    ধ্রুবক অক্ষর *তথ্য_As_Bytes = reinterpret_cast<ধ্রুবক অক্ষর *>(dh);
    ধ্রুবক RBBIDataHeader *rbbidh = reinterpret_cast<ধ্রুবক RBBIDataHeader *>(তথ্য_As_Bytes + header_আকার);
    init(rbbidh, status);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::is_তথ্য_Version_Acceptable(ধ্রুবক UVersionInfo version) {
    ফেরত RBBI_তথ্য_FORMAT_VERSION[0] == version[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
শূন্য RBBIDataWrapper::init0() {
    fHeader = nullptr;
    fForwardTable = nullptr;
    fReverseTable = nullptr;
    fRuleSource   = nullptr;
    fRuleStatusTable = nullptr;
    fTrie         = nullptr;
    fUDataMem     = nullptr;
    f_Ref_গুনতি     = 0;
    f_Dont_Free_তথ্য = সত্য;
}

শূন্য RBBIDataWrapper::init(ধ্রুবক RBBIDataHeader *তথ্য, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    fHeader = তথ্য;
    যদি (fHeader->fMagic != 0xb1a0 || !is_তথ্য_Version_Acceptable(fHeader->fFormatVersion)) {
        status = U_INVALID_FORMAT_ERROR;
        ফেরত;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    f_Dont_Free_তথ্য = মিথ্যা;
    যদি (তথ্য->fFTableLen != 0) {
        fForwardTable = reinterpret_cast<ধ্রুবক RBBIStateTable*>(reinterpret_cast<ধ্রুবক অক্ষর*>(তথ্য) + fHeader->fFTable);
    }
    যদি (তথ্য->fRTableLen != 0) {
        fReverseTable = reinterpret_cast<ধ্রুবক RBBIStateTable*>(reinterpret_cast<ধ্রুবক অক্ষর*>(তথ্য) + fHeader->fRTable);
    }

    fTrie = ucptrie_open_From_দ্বিমিক(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_মান_BITS_ANY,
                                   (uint8_t *)তথ্য + fHeader->fTrie,
                                   fHeader->fTrieLen,
                                   nullptr,           // *actual length
                                   &status);
    যদি (U_FAILURE(status)) {
        ফেরত;
    }

    UCPTrie_মান_Width width = ucptrie_get_মান_Width(fTrie);
    যদি (!(width == UCPTRIE_মান_BITS_8 || width == UCPTRIE_মান_BITS_16)) {
        status = U_INVALID_FORMAT_ERROR;
        ফেরত;
    }

    fRuleSource   = ((অক্ষর *)তথ্য + fHeader->fRuleSource);
    fRuleString = UnicodeString::fromUTF8(StringPiece(fRuleSource, fHeader->fRuleSourceLen));
    U_ASSERT(তথ্য->fRuleSourceLen > 0);

    fRuleStatusTable = reinterpret_cast<ধ্রুবক int32_t*>(reinterpret_cast<ধ্রুবক অক্ষর*>(তথ্য) + fHeader->fStatusTable);
    fStatusMaxIdx    = তথ্য->fStatusTableLen / আকার(int32_t);

    f_Ref_গুনতি = 1;

#ifdef RBBI_DEBUG
    অক্ষর *debugEnv = getenv("U_RBBIDEBUG");
    যদি (debugEnv && uprv_strstr(debugEnv, "data")) {এই->print_তথ্য();}
#endif
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_গুনতি == 0);
    ucptrie_close(fTrie);
    fTrie = nullptr;
    যদি (fUDataMem) {
        udata_close(fUDataMem);
    } অন্যথা যদি (!f_Dont_Free_তথ্য) {
        uprv_free((শূন্য *)fHeader);
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
যৌক্তিক RBBIDataWrapper::operator ==(ধ্রুবক RBBIDataWrapper &other) ধ্রুবক {
    যদি (fHeader == other.fHeader) {
        ফেরত সত্য;
    }
    যদি (fHeader->f_দৈর্ঘ্য != other.fHeader->f_দৈর্ঘ্য) {
        ফেরত মিথ্যা;
    }
    যদি (uprv_memcmp(fHeader, other.fHeader, fHeader->f_দৈর্ঘ্য) == 0) {
        ফেরত সত্য;
    }
    ফেরত মিথ্যা;
}

int32_t  RBBIDataWrapper::hashCode() {
    ফেরত fHeader->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
শূন্য RBBIDataWrapper::removeReference() {
    যদি (umtx_atomic_dec(&f_Ref_গুনতি) == 0) {
        delete এই;
    }
}


RBBIDataWrapper *RBBIDataWrapper::addReference() {
   umtx_atomic_inc(&f_Ref_গুনতি);
   ফেরত এই;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ধ্রুবক UnicodeString &RBBIDataWrapper::getRuleSourceString() ধ্রুবক {
    ফেরত fRuleString;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
শূন্য  RBBIDataWrapper::printTable(ধ্রুবক অক্ষর *heading, ধ্রুবক RBBIStateTable *table) {
    uint32_t   c;
    uint32_t   s;

    RBBIDebugPrintf("%s\n", heading);

    RBBIDebugPrintf("   fDictCategoriesStart: %d\n", table->fDictCategoriesStart);
    RBBIDebugPrintf("   fLookAheadResultsSize: %d\n", table->f_Look_Ahead_Results_আকার);
    RBBIDebugPrintf("   Flags: %4x RBBI_LOOKAHEAD_HARD_BREAK=%s RBBI_8BITS_ROWS=%s\n",
                    table->fFlags,
                    table->fFlags & RBBI_LOOKAHEAD_HARD_BREAK ? "T" : "F",
                    table->fFlags & RBBI_8BITS_ROWS ? "T" : "F");
    RBBIDebugPrintf("\nState |  Acc  LA TagIx");
    জন্য (c=0; c<fHeader->f_Cat_গুনতি; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); জন্য (c=0;c<fHeader->f_Cat_গুনতি; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    যদি (table == nullptr) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        ফেরত;
    }
    UBool use8Bits = table->fFlags & RBBI_8BITS_ROWS;
    জন্য (s=0; s<table->f_অঙ্ক_States; s++) {
        RBBIStateTableRow *row = (RBBIStateTableRow *)
                                  (table->f_Table_তথ্য + (table->fRowLen * s));
        যদি (use8Bits) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r8.fAccepting, row->r8.fLookAhead, row->r8.fTagsIdx);
            জন্য (c=0; c<fHeader->f_Cat_গুনতি; c++)  {
                RBBIDebugPrintf("%3d ", row->r8.f_পরবর্তী_State[c]);
            }
        } অন্যথা {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r16.fAccepting, row->r16.fLookAhead, row->r16.fTagsIdx);
            জন্য (c=0; c<fHeader->f_Cat_গুনতি; c++)  {
                RBBIDebugPrintf("%3d ", row->r16.f_পরবর্তী_State[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#endif


শূন্য  RBBIDataWrapper::print_তথ্য() {
#ifdef RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (শূন্য *)fHeader);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", fHeader->fFormatVersion[0], fHeader->fFormatVersion[1],
                                                    fHeader->fFormatVersion[2], fHeader->fFormatVersion[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", fHeader->f_দৈর্ঘ্য);
    RBBIDebugPrintf("   number of character categories = %d\n\n", fHeader->f_Cat_গুনতি);

    printTable("Forward State Transition Table", fForwardTable);
    printTable("Reverse State Transition Table", fReverseTable);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    জন্য (int32_t c=0; fRuleSource[c] != 0; c++) {
        RBBIDebugPrintf("%c", fRuleSource[c]);
    }
    RBBIDebugPrintf("\n\n");
#endif
}


U_NAMESPACE_END
U_NAMESPACE_USE

//-----------------------------------------------------------------------------
//
//  ubrk_swap   -  byte swap and char encoding swap of RBBI data
//
//-----------------------------------------------------------------------------

U_CAPI int32_t U_EXPORT2
ubrk_swap(ধ্রুবক UDataSwapper *ds, ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
           UErrorCode *status) {

    যদি (status == nullptr || U_FAILURE(*status)) {
        ফেরত 0;
    }
    যদি(ds==nullptr || in_তথ্য==nullptr || length<-1 || (length>0 && out_তথ্য==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ধ্রুবক UDataInfo *pInfo = (ধ্রুবক UDataInfo *)((ধ্রুবক অক্ষর *)in_তথ্য+4);
    যদি(!(  pInfo->তথ্য_Format[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->তথ্য_Format[1]==0x72 &&
           pInfo->তথ্য_Format[2]==0x6b &&
           pInfo->তথ্য_Format[3]==0x20 &&
           RBBIDataWrapper::is_তথ্য_Version_Acceptable(pInfo->formatVersion) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->তথ্য_Format[0], pInfo->তথ্য_Format[1],
                         pInfo->তথ্য_Format[2], pInfo->তথ্য_Format[3],
                         pInfo->formatVersion[0]);
        *status=U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    //
    // Swap the data header.  (This is the generic ICU Data Header, not the RBBI Specific
    //                         RBBIDataHeader).  This swap also conveniently gets us
    //                         the size of the ICU d.h., which lets us locate the start
    //                         of the RBBI specific data.
    //
    int32_t header_আকার=udata_swap_তথ্য_Header(ds, in_তথ্য, length, out_তথ্য, status);


    //
    // Get the RRBI Data Header, and check that it appears to be OK.
    //
    ধ্রুবক uint8_t  *inBytes =(ধ্রুবক uint8_t *)in_তথ্য+header_আকার;
    RBBIDataHeader *rbbiDH = (RBBIDataHeader *)inBytes;
    যদি (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::is_তথ্য_Version_Acceptable(rbbiDH->fFormatVersion) ||
            ds->readUInt32(rbbiDH->f_দৈর্ঘ্য)  <  আকার(RBBIDataHeader)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *status=U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t break_তথ্য_দৈর্ঘ্য = ds->readUInt32(rbbiDH->f_দৈর্ঘ্য);
    int32_t যোগ_আকার = header_আকার + break_তথ্য_দৈর্ঘ্য;
    যদি (length < 0) {
        ফেরত যোগ_আকার;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    যদি (length < যোগ_আকার) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            break_তথ্য_দৈর্ঘ্য);
        *status=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
        }


    //
    // Swap the Data.  Do the data itself first, then the RBBI Data Header, because
    //                 we need to reference the header to locate the data, and an
    //                 inplace swap of the header leaves it unusable.
    //
    uint8_t         *outBytes = (uint8_t *)out_তথ্য + header_আকার;
    RBBIDataHeader  *outputDH = (RBBIDataHeader *)outBytes;

    int32_t   tableStartOffset;
    int32_t   table_দৈর্ঘ্য;

    //
    // If not swapping in place, zero out the output buffer before starting.
    //    Individual tables and other data items within are aligned to 8 byte boundaries
    //    when originally created.  Any unused space between items needs to be zero.
    //
    যদি (inBytes != outBytes) {
        uprv_memset(outBytes, 0, break_তথ্য_দৈর্ঘ্য);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         শীর্ষ_আকার = offsetof(RBBIStateTable, f_Table_তথ্য);

    // Forward state table.
    tableStartOffset = ds->readUInt32(rbbiDH->fFTable);
    table_দৈর্ঘ্য      = ds->readUInt32(rbbiDH->fFTableLen);

    যদি (table_দৈর্ঘ্য > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, শীর্ষ_আকার,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        যদি (use8Bits) {
            যদি (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+শীর্ষ_আকার,
                             inBytes+tableStartOffset+শীর্ষ_আকার,
                             table_দৈর্ঘ্য-শীর্ষ_আকার);
            }
        } অন্যথা {
            ds->swapArray16(ds, inBytes+tableStartOffset+শীর্ষ_আকার, table_দৈর্ঘ্য-শীর্ষ_আকার,
                                outBytes+tableStartOffset+শীর্ষ_আকার, status);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    tableStartOffset = ds->readUInt32(rbbiDH->fRTable);
    table_দৈর্ঘ্য      = ds->readUInt32(rbbiDH->fRTableLen);

    যদি (table_দৈর্ঘ্য > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, শীর্ষ_আকার,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        যদি (use8Bits) {
            যদি (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+শীর্ষ_আকার,
                             inBytes+tableStartOffset+শীর্ষ_আকার,
                             table_দৈর্ঘ্য-শীর্ষ_আকার);
            }
        } অন্যথা {
            ds->swapArray16(ds, inBytes+tableStartOffset+শীর্ষ_আকার, table_দৈর্ঘ্য-শীর্ষ_আকার,
                                outBytes+tableStartOffset+শীর্ষ_আকার, status);
        }
    }

    // Trie table for character categories
    ucptrie_swap(ds, inBytes+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     outBytes+ds->readUInt32(rbbiDH->fTrie), status);

    // Source Rules Text.  It's UTF8 data
    যদি (outBytes != inBytes) {
        uprv_memmove(outBytes+ds->readUInt32(rbbiDH->fRuleSource),
                     inBytes+ds->readUInt32(rbbiDH->fRuleSource),
                     ds->readUInt32(rbbiDH->fRuleSourceLen));
    }

    // Table of rule status values.  It's all int_32 values
    ds->swapArray32(ds, inBytes+ds->readUInt32(rbbiDH->fStatusTable), ds->readUInt32(rbbiDH->fStatusTableLen),
                        outBytes+ds->readUInt32(rbbiDH->fStatusTable), status);

    // And, last, the header.
    //   It is all int32_t values except for fFormataVersion, which is an array of four bytes.
    //   Swap the whole thing as int32_t, then re-swap the one field.
    //
    ds->swapArray32(ds, inBytes, আকার(RBBIDataHeader), outBytes, status);
    ds->swapArray32(ds, outputDH->fFormatVersion, 4, outputDH->fFormatVersion, status);

    ফেরত যোগ_আকার;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
