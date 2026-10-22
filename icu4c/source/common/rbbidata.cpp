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
RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIDataHeader *data, UErrorCode &status) {
    init0();
    init(data, status);
}

RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIDataHeader *data, شمارش EDontAdopt, UErrorCode &status) {
    init0();
    init(data, status);
    fDontFreeData = درست;
}

RBBIDataWrapper::RBBIDataWrapper(UDataMemory* udm, UErrorCode &status) {
    init0();
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    ثابت DataHeader *dh = udm->pHeader;
    int32_t headerSize = dh->dataHeader.headerSize;
    اگر (  !(headerSize >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.dataFormat[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.dataFormat[1] == 0x72 &&
            dh->info.dataFormat[2] == 0x6b &&
            dh->info.dataFormat[3] == 0x20 &&
            isDataVersionAcceptable(dh->info.formatVersion))
        ) {
        status = U_INVALID_FORMAT_ERROR;
        بازگشت;
    }
    ثابت نویسه *dataAsBytes = reinterpret_cast<ثابت نویسه *>(dh);
    ثابت RBBIDataHeader *rbbidh = reinterpret_cast<ثابت RBBIDataHeader *>(dataAsBytes + headerSize);
    init(rbbidh, status);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::isDataVersionAcceptable(ثابت UVersionInfo version) {
    بازگشت RBBI_DATA_FORMAT_VERSION[0] == version[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
تهی RBBIDataWrapper::init0() {
    fHeader = nullptr;
    fForwardTable = nullptr;
    fReverseTable = nullptr;
    fRuleSource   = nullptr;
    fRuleStatusTable = nullptr;
    fTrie         = nullptr;
    fUDataMem     = nullptr;
    f_Ref_شمار     = 0;
    fDontFreeData = درست;
}

تهی RBBIDataWrapper::init(ثابت RBBIDataHeader *data, UErrorCode &status) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    fHeader = data;
    اگر (fHeader->fMagic != 0xb1a0 || !isDataVersionAcceptable(fHeader->fFormatVersion)) {
        status = U_INVALID_FORMAT_ERROR;
        بازگشت;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    fDontFreeData = نادرست;
    اگر (data->fFTableLen != 0) {
        fForwardTable = reinterpret_cast<ثابت RBBIStateTable*>(reinterpret_cast<ثابت نویسه*>(data) + fHeader->fFTable);
    }
    اگر (data->fRTableLen != 0) {
        fReverseTable = reinterpret_cast<ثابت RBBIStateTable*>(reinterpret_cast<ثابت نویسه*>(data) + fHeader->fRTable);
    }

    fTrie = ucptrie_open_From_دودویی(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_مقدار_BITS_ANY,
                                   (uint8_t *)data + fHeader->fTrie,
                                   fHeader->fTrieLen,
                                   nullptr,           // *actual length
                                   &status);
    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    UCPTrie_مقدار_Width width = ucptrie_get_مقدار_Width(fTrie);
    اگر (!(width == UCPTRIE_مقدار_BITS_8 || width == UCPTRIE_مقدار_BITS_16)) {
        status = U_INVALID_FORMAT_ERROR;
        بازگشت;
    }

    fRuleSource   = ((نویسه *)data + fHeader->fRuleSource);
    fRuleString = UnicodeString::fromUTF8(StringPiece(fRuleSource, fHeader->fRuleSourceLen));
    U_ASSERT(data->fRuleSourceLen > 0);

    fRuleStatusTable = reinterpret_cast<ثابت int32_t*>(reinterpret_cast<ثابت نویسه*>(data) + fHeader->fStatusTable);
    fStatusMaxIdx    = data->fStatusTableLen / اندازه(int32_t);

    f_Ref_شمار = 1;

#ifdef RBBI_DEBUG
    نویسه *debugEnv = getenv("U_RBBIDEBUG");
    اگر (debugEnv && uprv_strstr(debugEnv, "data")) {این->printData();}
#endif
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_شمار == 0);
    ucptrie_close(fTrie);
    fTrie = nullptr;
    اگر (fUDataMem) {
        udata_close(fUDataMem);
    } وگرنه اگر (!fDontFreeData) {
        uprv_free((تهی *)fHeader);
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
منطقی RBBIDataWrapper::operator ==(ثابت RBBIDataWrapper &other) ثابت {
    اگر (fHeader == other.fHeader) {
        بازگشت درست;
    }
    اگر (fHeader->fLength != other.fHeader->fLength) {
        بازگشت نادرست;
    }
    اگر (uprv_memcmp(fHeader, other.fHeader, fHeader->fLength) == 0) {
        بازگشت درست;
    }
    بازگشت نادرست;
}

int32_t  RBBIDataWrapper::hashCode() {
    بازگشت fHeader->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
تهی RBBIDataWrapper::removeReference() {
    اگر (umtx_atomic_dec(&f_Ref_شمار) == 0) {
        delete این;
    }
}


RBBIDataWrapper *RBBIDataWrapper::addReference() {
   umtx_atomic_inc(&f_Ref_شمار);
   بازگشت این;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ثابت UnicodeString &RBBIDataWrapper::getRuleSourceString() ثابت {
    بازگشت fRuleString;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
تهی  RBBIDataWrapper::printTable(ثابت نویسه *heading, ثابت RBBIStateTable *table) {
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
    برای (c=0; c<fHeader->f_Cat_شمار; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); برای (c=0;c<fHeader->f_Cat_شمار; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    اگر (table == nullptr) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        بازگشت;
    }
    UBool use8Bits = table->fFlags & RBBI_8BITS_ROWS;
    برای (s=0; s<table->f_رقم_States; s++) {
        RBBIStateTableRow *row = (RBBIStateTableRow *)
                                  (table->fTableData + (table->fRowLen * s));
        اگر (use8Bits) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r8.fAccepting, row->r8.fLookAhead, row->r8.fTagsIdx);
            برای (c=0; c<fHeader->f_Cat_شمار; c++)  {
                RBBIDebugPrintf("%3d ", row->r8.fNextState[c]);
            }
        } وگرنه {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r16.fAccepting, row->r16.fLookAhead, row->r16.fTagsIdx);
            برای (c=0; c<fHeader->f_Cat_شمار; c++)  {
                RBBIDebugPrintf("%3d ", row->r16.fNextState[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#endif


تهی  RBBIDataWrapper::printData() {
#ifdef RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (تهی *)fHeader);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", fHeader->fFormatVersion[0], fHeader->fFormatVersion[1],
                                                    fHeader->fFormatVersion[2], fHeader->fFormatVersion[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", fHeader->fLength);
    RBBIDebugPrintf("   number of character categories = %d\n\n", fHeader->f_Cat_شمار);

    printTable("Forward State Transition Table", fForwardTable);
    printTable("Reverse State Transition Table", fReverseTable);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    برای (int32_t c=0; fRuleSource[c] != 0; c++) {
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
ubrk_swap(ثابت UDataSwapper *ds, ثابت تهی *inData, int32_t length, تهی *outData,
           UErrorCode *status) {

    اگر (status == nullptr || U_FAILURE(*status)) {
        بازگشت 0;
    }
    اگر(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ثابت UDataInfo *pInfo = (ثابت UDataInfo *)((ثابت نویسه *)inData+4);
    اگر(!(  pInfo->dataFormat[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->dataFormat[1]==0x72 &&
           pInfo->dataFormat[2]==0x6b &&
           pInfo->dataFormat[3]==0x20 &&
           RBBIDataWrapper::isDataVersionAcceptable(pInfo->formatVersion) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0]);
        *status=U_UNSUPPORTED_ERROR;
        بازگشت 0;
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
    ثابت uint8_t  *inBytes =(ثابت uint8_t *)inData+headerSize;
    RBBIDataHeader *rbbiDH = (RBBIDataHeader *)inBytes;
    اگر (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::isDataVersionAcceptable(rbbiDH->fFormatVersion) ||
            ds->readUInt32(rbbiDH->fLength)  <  اندازه(RBBIDataHeader)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *status=U_UNSUPPORTED_ERROR;
        بازگشت 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t breakDataLength = ds->readUInt32(rbbiDH->fLength);
    int32_t totalSize = headerSize + breakDataLength;
    اگر (length < 0) {
        بازگشت totalSize;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    اگر (length < totalSize) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            breakDataLength);
        *status=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت 0;
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
    اگر (inBytes != outBytes) {
        uprv_memset(outBytes, 0, breakDataLength);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         بالاترین_Size = offsetof(RBBIStateTable, fTableData);

    // Forward state table.
    tableStartOffset = ds->readUInt32(rbbiDH->fFTable);
    tableLength      = ds->readUInt32(rbbiDH->fFTableLen);

    اگر (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, بالاترین_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        اگر (use8Bits) {
            اگر (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+بالاترین_Size,
                             inBytes+tableStartOffset+بالاترین_Size,
                             tableLength-بالاترین_Size);
            }
        } وگرنه {
            ds->swapArray16(ds, inBytes+tableStartOffset+بالاترین_Size, tableLength-بالاترین_Size,
                                outBytes+tableStartOffset+بالاترین_Size, status);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    tableStartOffset = ds->readUInt32(rbbiDH->fRTable);
    tableLength      = ds->readUInt32(rbbiDH->fRTableLen);

    اگر (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, بالاترین_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        اگر (use8Bits) {
            اگر (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+بالاترین_Size,
                             inBytes+tableStartOffset+بالاترین_Size,
                             tableLength-بالاترین_Size);
            }
        } وگرنه {
            ds->swapArray16(ds, inBytes+tableStartOffset+بالاترین_Size, tableLength-بالاترین_Size,
                                outBytes+tableStartOffset+بالاترین_Size, status);
        }
    }

    // Trie table for character categories
    ucptrie_swap(ds, inBytes+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     outBytes+ds->readUInt32(rbbiDH->fTrie), status);

    // Source Rules Text.  It's UTF8 data
    اگر (outBytes != inBytes) {
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
    ds->swapArray32(ds, inBytes, اندازه(RBBIDataHeader), outBytes, status);
    ds->swapArray32(ds, outputDH->fFormatVersion, 4, outputDH->fFormatVersion, status);

    بازگشت totalSize;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
