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
RBBIDataWrapper::RBBIDataWrapper(ধ্রুব RBBIDataHeader *data, UErrorCode &status) {
    init0();
    init(data, status);
}

RBBIDataWrapper::RBBIDataWrapper(ধ্রুব RBBIDataHeader *data, গণনা EDontAdopt, UErrorCode &status) {
    init0();
    init(data, status);
    fDontFreeData = সত্য;
}

RBBIDataWrapper::RBBIDataWrapper(UDataMemory* udm, UErrorCode &status) {
    init0();
    যদি (U_FAILURE(status)) {
        প্রত্যাগম;
    }
    ধ্রুব DataHeader *dh = udm->pHeader;
    int32_t headerSize = dh->dataHeader.headerSize;
    যদি (  !(headerSize >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.dataFormat[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.dataFormat[1] == 0x72 &&
            dh->info.dataFormat[2] == 0x6b &&
            dh->info.dataFormat[3] == 0x20 &&
            isDataVersionAcceptable(dh->info.formatVersion))
        ) {
        status = U_INVALID_FORMAT_ERROR;
        প্রত্যাগম;
    }
    ধ্রুব বর্ণ *dataAsBytes = reinterpret_cast<ধ্রুব বর্ণ *>(dh);
    ধ্রুব RBBIDataHeader *rbbidh = reinterpret_cast<ধ্রুব RBBIDataHeader *>(dataAsBytes + headerSize);
    init(rbbidh, status);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::isDataVersionAcceptable(ধ্রুব UVersionInfo version) {
    প্রত্যাগম RBBI_DATA_FORMAT_VERSION[0] == version[0];
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
    fDontFreeData = সত্য;
}

শূন্য RBBIDataWrapper::init(ধ্রুব RBBIDataHeader *data, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        প্রত্যাগম;
    }
    fHeader = data;
    যদি (fHeader->fMagic != 0xb1a0 || !isDataVersionAcceptable(fHeader->fFormatVersion)) {
        status = U_INVALID_FORMAT_ERROR;
        প্রত্যাগম;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    fDontFreeData = অসত্য;
    যদি (data->fFTableLen != 0) {
        fForwardTable = reinterpret_cast<ধ্রুব RBBIStateTable*>(reinterpret_cast<ধ্রুব বর্ণ*>(data) + fHeader->fFTable);
    }
    যদি (data->fRTableLen != 0) {
        fReverseTable = reinterpret_cast<ধ্রুব RBBIStateTable*>(reinterpret_cast<ধ্রুব বর্ণ*>(data) + fHeader->fRTable);
    }

    fTrie = ucptrie_open_From_দ্বিমিক(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_মান_BITS_ANY,
                                   (uint8_t *)data + fHeader->fTrie,
                                   fHeader->fTrieLen,
                                   nullptr,           // *actual length
                                   &status);
    যদি (U_FAILURE(status)) {
        প্রত্যাগম;
    }

    UCPTrie_মান_Width width = ucptrie_get_মান_Width(fTrie);
    যদি (!(width == UCPTRIE_মান_BITS_8 || width == UCPTRIE_মান_BITS_16)) {
        status = U_INVALID_FORMAT_ERROR;
        প্রত্যাগম;
    }

    fRuleSource   = ((বর্ণ *)data + fHeader->fRuleSource);
    fRuleString = UnicodeString::fromUTF8(StringPiece(fRuleSource, fHeader->fRuleSourceLen));
    U_ASSERT(data->fRuleSourceLen > 0);

    fRuleStatusTable = reinterpret_cast<ধ্রুব int32_t*>(reinterpret_cast<ধ্রুব বর্ণ*>(data) + fHeader->fStatusTable);
    fStatusMaxIdx    = data->fStatusTableLen / পরিমাণ(int32_t);

    f_Ref_গুনতি = 1;

#ifdef RBBI_DEBUG
    বর্ণ *debugEnv = getenv("U_RBBIDEBUG");
    যদি (debugEnv && uprv_strstr(debugEnv, "data")) {এতত্->printData();}
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
    } অন্যথা যদি (!fDontFreeData) {
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
তার্কিক RBBIDataWrapper::operator ==(ধ্রুব RBBIDataWrapper &other) ধ্রুব {
    যদি (fHeader == other.fHeader) {
        প্রত্যাগম সত্য;
    }
    যদি (fHeader->fLength != other.fHeader->fLength) {
        প্রত্যাগম অসত্য;
    }
    যদি (uprv_memcmp(fHeader, other.fHeader, fHeader->fLength) == 0) {
        প্রত্যাগম সত্য;
    }
    প্রত্যাগম অসত্য;
}

int32_t  RBBIDataWrapper::hashCode() {
    প্রত্যাগম fHeader->fFTableLen;
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
        delete এতত্;
    }
}


RBBIDataWrapper *RBBIDataWrapper::addReference() {
   umtx_atomic_inc(&f_Ref_গুনতি);
   প্রত্যাগম এতত্;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ধ্রুব UnicodeString &RBBIDataWrapper::getRuleSourceString() ধ্রুব {
    প্রত্যাগম fRuleString;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
শূন্য  RBBIDataWrapper::printTable(ধ্রুব বর্ণ *heading, ধ্রুব RBBIStateTable *table) {
    uint32_t   c;
    uint32_t   s;

    RBBIDebugPrintf("%s\n", heading);

    RBBIDebugPrintf("   fDictCategoriesStart: %d\n", table->fDictCategoriesStart);
    RBBIDebugPrintf("   fLookAheadResultsSize: %d\n", table->fLookAheadResultsSize);
    RBBIDebugPrintf("   Flags: %4x RBBI_LOOKAHEAD_HARD_BREAK=%s RBBI_8BITS_ROWS=%s\n",
                    table->fFlags,
                    table->fFlags & RBBI_LOOKAHEAD_HARD_BREAK ? "T" : "F",
                    table->fFlags & RBBI_8BITS_ROWS ? "T" : "F");
    RBBIDebugPrintf("\nState |  Acc  LA TagIx");
    প্রতি (c=0; c<fHeader->f_Cat_গুনতি; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); প্রতি (c=0;c<fHeader->f_Cat_গুনতি; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    যদি (table == nullptr) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        প্রত্যাগম;
    }
    UBool use8Bits = table->fFlags & RBBI_8BITS_ROWS;
    প্রতি (s=0; s<table->f_অঙ্ক_States; s++) {
        RBBIStateTableRow *row = (RBBIStateTableRow *)
                                  (table->fTableData + (table->fRowLen * s));
        যদি (use8Bits) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r8.fAccepting, row->r8.fLookAhead, row->r8.fTagsIdx);
            প্রতি (c=0; c<fHeader->f_Cat_গুনতি; c++)  {
                RBBIDebugPrintf("%3d ", row->r8.fNextState[c]);
            }
        } অন্যথা {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r16.fAccepting, row->r16.fLookAhead, row->r16.fTagsIdx);
            প্রতি (c=0; c<fHeader->f_Cat_গুনতি; c++)  {
                RBBIDebugPrintf("%3d ", row->r16.fNextState[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#endif


শূন্য  RBBIDataWrapper::printData() {
#ifdef RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (শূন্য *)fHeader);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", fHeader->fFormatVersion[0], fHeader->fFormatVersion[1],
                                                    fHeader->fFormatVersion[2], fHeader->fFormatVersion[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", fHeader->fLength);
    RBBIDebugPrintf("   number of character categories = %d\n\n", fHeader->f_Cat_গুনতি);

    printTable("Forward State Transition Table", fForwardTable);
    printTable("Reverse State Transition Table", fReverseTable);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    প্রতি (int32_t c=0; fRuleSource[c] != 0; c++) {
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
ubrk_swap(ধ্রুব UDataSwapper *ds, ধ্রুব শূন্য *inData, int32_t length, শূন্য *outData,
           UErrorCode *status) {

    যদি (status == nullptr || U_FAILURE(*status)) {
        প্রত্যাগম 0;
    }
    যদি(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ধ্রুব UDataInfo *pInfo = (ধ্রুব UDataInfo *)((ধ্রুব বর্ণ *)inData+4);
    যদি(!(  pInfo->dataFormat[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->dataFormat[1]==0x72 &&
           pInfo->dataFormat[2]==0x6b &&
           pInfo->dataFormat[3]==0x20 &&
           RBBIDataWrapper::isDataVersionAcceptable(pInfo->formatVersion) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0]);
        *status=U_UNSUPPORTED_ERROR;
        প্রত্যাগম 0;
    }

    //
    // Swap the data header.  (This is the generic ICU Data Header, not the RBBI Specific
    //                         RBBIDataHeader).  This swap also conveniently gets us
    //                         the size of the ICU d.h., which lets us locate the start
    //                         of the RBBI specific data.
    //
    int32_t headerSize=udata_swapDataHeader(ds, inData, length, outData, status);


    //
    // Get the RRBI Data Header, and check that it appears to be OK.
    //
    ধ্রুব uint8_t  *inBytes =(ধ্রুব uint8_t *)inData+headerSize;
    RBBIDataHeader *rbbiDH = (RBBIDataHeader *)inBytes;
    যদি (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::isDataVersionAcceptable(rbbiDH->fFormatVersion) ||
            ds->readUInt32(rbbiDH->fLength)  <  পরিমাণ(RBBIDataHeader)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *status=U_UNSUPPORTED_ERROR;
        প্রত্যাগম 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t breakDataLength = ds->readUInt32(rbbiDH->fLength);
    int32_t totalSize = headerSize + breakDataLength;
    যদি (length < 0) {
        প্রত্যাগম totalSize;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    যদি (length < totalSize) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            breakDataLength);
        *status=U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম 0;
        }


    //
    // Swap the Data.  Do the data itself first, then the RBBI Data Header, because
    //                 we need to reference the header to locate the data, and an
    //                 inplace swap of the header leaves it unusable.
    //
    uint8_t         *outBytes = (uint8_t *)outData + headerSize;
    RBBIDataHeader  *outputDH = (RBBIDataHeader *)outBytes;

    int32_t   tableStartOffset;
    int32_t   tableLength;

    //
    // If not swapping in place, zero out the output buffer before starting.
    //    Individual tables and other data items within are aligned to 8 byte boundaries
    //    when originally created.  Any unused space between items needs to be zero.
    //
    যদি (inBytes != outBytes) {
        uprv_memset(outBytes, 0, breakDataLength);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         শীর্ষ_Size = offsetof(RBBIStateTable, fTableData);

    // Forward state table.
    tableStartOffset = ds->readUInt32(rbbiDH->fFTable);
    tableLength      = ds->readUInt32(rbbiDH->fFTableLen);

    যদি (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, শীর্ষ_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        যদি (use8Bits) {
            যদি (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+শীর্ষ_Size,
                             inBytes+tableStartOffset+শীর্ষ_Size,
                             tableLength-শীর্ষ_Size);
            }
        } অন্যথা {
            ds->swapArray16(ds, inBytes+tableStartOffset+শীর্ষ_Size, tableLength-শীর্ষ_Size,
                                outBytes+tableStartOffset+শীর্ষ_Size, status);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    tableStartOffset = ds->readUInt32(rbbiDH->fRTable);
    tableLength      = ds->readUInt32(rbbiDH->fRTableLen);

    যদি (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, শীর্ষ_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        যদি (use8Bits) {
            যদি (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+শীর্ষ_Size,
                             inBytes+tableStartOffset+শীর্ষ_Size,
                             tableLength-শীর্ষ_Size);
            }
        } অন্যথা {
            ds->swapArray16(ds, inBytes+tableStartOffset+শীর্ষ_Size, tableLength-শীর্ষ_Size,
                                outBytes+tableStartOffset+শীর্ষ_Size, status);
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
    ds->swapArray32(ds, inBytes, পরিমাণ(RBBIDataHeader), outBytes, status);
    ds->swapArray32(ds, outputDH->fFormatVersion, 4, outputDH->fFormatVersion, status);

    প্রত্যাগম totalSize;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
