// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 1999-2014 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ "unicode/ucptrie.h"
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "rbbidata.h"
#அந்தர்பாவ "rbbirb.h"
#அந்தர்பாவ "udatamem.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "umutex.h"

#அந்தர்பாவ "uassert.h"


U_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//    Constructors.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::RBBIDataWrapper(மாறிலி RBBIDataHeader *data, UErrorCode &status) {
    init0();
    init(data, status);
}

RBBIDataWrapper::RBBIDataWrapper(மாறிலி RBBIDataHeader *data, எண்ணிக்கை EDontAdopt, UErrorCode &status) {
    init0();
    init(data, status);
    fDontFreeData = உண்மை;
}

RBBIDataWrapper::RBBIDataWrapper(UDataMemory* udm, UErrorCode &status) {
    init0();
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    மாறிலி DataHeader *dh = udm->pHeader;
    int32_t headerSize = dh->dataHeader.headerSize;
    எனில் (  !(headerSize >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.dataFormat[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.dataFormat[1] == 0x72 &&
            dh->info.dataFormat[2] == 0x6b &&
            dh->info.dataFormat[3] == 0x20 &&
            isDataVersionAcceptable(dh->info.formatVersion))
        ) {
        status = U_INVALID_FORMAT_ERROR;
        திரும்பு;
    }
    மாறிலி எழுத்து *dataAsBytes = reinterpret_cast<மாறிலி எழுத்து *>(dh);
    மாறிலி RBBIDataHeader *rbbidh = reinterpret_cast<மாறிலி RBBIDataHeader *>(dataAsBytes + headerSize);
    init(rbbidh, status);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::isDataVersionAcceptable(மாறிலி UVersionInfo version) {
    திரும்பு RBBI_DATA_FORMAT_VERSION[0] == version[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
வெறுமை RBBIDataWrapper::init0() {
    fHeader = nullptr;
    fForwardTable = nullptr;
    fReverseTable = nullptr;
    fRuleSource   = nullptr;
    fRuleStatusTable = nullptr;
    fTrie         = nullptr;
    fUDataMem     = nullptr;
    f_Ref_மொத்தம்     = 0;
    fDontFreeData = உண்மை;
}

வெறுமை RBBIDataWrapper::init(மாறிலி RBBIDataHeader *data, UErrorCode &status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    fHeader = data;
    எனில் (fHeader->fMagic != 0xb1a0 || !isDataVersionAcceptable(fHeader->fFormatVersion)) {
        status = U_INVALID_FORMAT_ERROR;
        திரும்பு;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    fDontFreeData = பொய்;
    எனில் (data->fFTableLen != 0) {
        fForwardTable = reinterpret_cast<மாறிலி RBBIStateTable*>(reinterpret_cast<மாறிலி எழுத்து*>(data) + fHeader->fFTable);
    }
    எனில் (data->fRTableLen != 0) {
        fReverseTable = reinterpret_cast<மாறிலி RBBIStateTable*>(reinterpret_cast<மாறிலி எழுத்து*>(data) + fHeader->fRTable);
    }

    fTrie = ucptrie_open_From_இருமம்(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_மதிப்பு_BITS_ANY,
                                   (uint8_t *)data + fHeader->fTrie,
                                   fHeader->fTrieLen,
                                   nullptr,           // *actual length
                                   &status);
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }

    UCPTrie_மதிப்பு_Width width = ucptrie_get_மதிப்பு_Width(fTrie);
    எனில் (!(width == UCPTRIE_மதிப்பு_BITS_8 || width == UCPTRIE_மதிப்பு_BITS_16)) {
        status = U_INVALID_FORMAT_ERROR;
        திரும்பு;
    }

    fRuleSource   = ((எழுத்து *)data + fHeader->fRuleSource);
    fRuleString = UnicodeString::fromUTF8(StringPiece(fRuleSource, fHeader->fRuleSourceLen));
    U_ASSERT(data->fRuleSourceLen > 0);

    fRuleStatusTable = reinterpret_cast<மாறிலி int32_t*>(reinterpret_cast<மாறிலி எழுத்து*>(data) + fHeader->fStatusTable);
    fStatusMaxIdx    = data->fStatusTableLen / அளவு(int32_t);

    f_Ref_மொத்தம் = 1;

#ifdef RBBI_DEBUG
    எழுத்து *debugEnv = getenv("U_RBBIDEBUG");
    எனில் (debugEnv && uprv_strstr(debugEnv, "data")) {இது->printData();}
#endif
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_மொத்தம் == 0);
    ucptrie_close(fTrie);
    fTrie = nullptr;
    எனில் (fUDataMem) {
        udata_close(fUDataMem);
    } இல்லையெனில் எனில் (!fDontFreeData) {
        uprv_free((வெறுமை *)fHeader);
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
தருக்க RBBIDataWrapper::operator ==(மாறிலி RBBIDataWrapper &other) மாறிலி {
    எனில் (fHeader == other.fHeader) {
        திரும்பு உண்மை;
    }
    எனில் (fHeader->fLength != other.fHeader->fLength) {
        திரும்பு பொய்;
    }
    எனில் (uprv_memcmp(fHeader, other.fHeader, fHeader->fLength) == 0) {
        திரும்பு உண்மை;
    }
    திரும்பு பொய்;
}

int32_t  RBBIDataWrapper::hashCode() {
    திரும்பு fHeader->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
வெறுமை RBBIDataWrapper::removeReference() {
    எனில் (umtx_atomic_dec(&f_Ref_மொத்தம்) == 0) {
        delete இது;
    }
}


RBBIDataWrapper *RBBIDataWrapper::addReference() {
   umtx_atomic_inc(&f_Ref_மொத்தம்);
   திரும்பு இது;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
மாறிலி UnicodeString &RBBIDataWrapper::getRuleSourceString() மாறிலி {
    திரும்பு fRuleString;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
வெறுமை  RBBIDataWrapper::printTable(மாறிலி எழுத்து *heading, மாறிலி RBBIStateTable *table) {
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
    க்கு (c=0; c<fHeader->f_Cat_மொத்தம்; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); க்கு (c=0;c<fHeader->f_Cat_மொத்தம்; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    எனில் (table == nullptr) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        திரும்பு;
    }
    UBool use8Bits = table->fFlags & RBBI_8BITS_ROWS;
    க்கு (s=0; s<table->f_இலக்கம்_States; s++) {
        RBBIStateTableRow *row = (RBBIStateTableRow *)
                                  (table->fTableData + (table->fRowLen * s));
        எனில் (use8Bits) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r8.fAccepting, row->r8.fLookAhead, row->r8.fTagsIdx);
            க்கு (c=0; c<fHeader->f_Cat_மொத்தம்; c++)  {
                RBBIDebugPrintf("%3d ", row->r8.fNextState[c]);
            }
        } இல்லையெனில் {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r16.fAccepting, row->r16.fLookAhead, row->r16.fTagsIdx);
            க்கு (c=0; c<fHeader->f_Cat_மொத்தம்; c++)  {
                RBBIDebugPrintf("%3d ", row->r16.fNextState[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#endif


வெறுமை  RBBIDataWrapper::printData() {
#ifdef RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (வெறுமை *)fHeader);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", fHeader->fFormatVersion[0], fHeader->fFormatVersion[1],
                                                    fHeader->fFormatVersion[2], fHeader->fFormatVersion[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", fHeader->fLength);
    RBBIDebugPrintf("   number of character categories = %d\n\n", fHeader->f_Cat_மொத்தம்);

    printTable("Forward State Transition Table", fForwardTable);
    printTable("Reverse State Transition Table", fReverseTable);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    க்கு (int32_t c=0; fRuleSource[c] != 0; c++) {
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
ubrk_swap(மாறிலி UDataSwapper *ds, மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
           UErrorCode *status) {

    எனில் (status == nullptr || U_FAILURE(*status)) {
        திரும்பு 0;
    }
    எனில்(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    மாறிலி UDataInfo *pInfo = (மாறிலி UDataInfo *)((மாறிலி எழுத்து *)inData+4);
    எனில்(!(  pInfo->dataFormat[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->dataFormat[1]==0x72 &&
           pInfo->dataFormat[2]==0x6b &&
           pInfo->dataFormat[3]==0x20 &&
           RBBIDataWrapper::isDataVersionAcceptable(pInfo->formatVersion) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0]);
        *status=U_UNSUPPORTED_ERROR;
        திரும்பு 0;
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
    மாறிலி uint8_t  *inBytes =(மாறிலி uint8_t *)inData+headerSize;
    RBBIDataHeader *rbbiDH = (RBBIDataHeader *)inBytes;
    எனில் (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::isDataVersionAcceptable(rbbiDH->fFormatVersion) ||
            ds->readUInt32(rbbiDH->fLength)  <  அளவு(RBBIDataHeader)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *status=U_UNSUPPORTED_ERROR;
        திரும்பு 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t breakDataLength = ds->readUInt32(rbbiDH->fLength);
    int32_t totalSize = headerSize + breakDataLength;
    எனில் (length < 0) {
        திரும்பு totalSize;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    எனில் (length < totalSize) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            breakDataLength);
        *status=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
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
    எனில் (inBytes != outBytes) {
        uprv_memset(outBytes, 0, breakDataLength);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         உச்சி_Size = offsetof(RBBIStateTable, fTableData);

    // Forward state table.
    tableStartOffset = ds->readUInt32(rbbiDH->fFTable);
    tableLength      = ds->readUInt32(rbbiDH->fFTableLen);

    எனில் (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, உச்சி_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        எனில் (use8Bits) {
            எனில் (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+உச்சி_Size,
                             inBytes+tableStartOffset+உச்சி_Size,
                             tableLength-உச்சி_Size);
            }
        } இல்லையெனில் {
            ds->swapArray16(ds, inBytes+tableStartOffset+உச்சி_Size, tableLength-உச்சி_Size,
                                outBytes+tableStartOffset+உச்சி_Size, status);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    tableStartOffset = ds->readUInt32(rbbiDH->fRTable);
    tableLength      = ds->readUInt32(rbbiDH->fRTableLen);

    எனில் (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, உச்சி_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        எனில் (use8Bits) {
            எனில் (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+உச்சி_Size,
                             inBytes+tableStartOffset+உச்சி_Size,
                             tableLength-உச்சி_Size);
            }
        } இல்லையெனில் {
            ds->swapArray16(ds, inBytes+tableStartOffset+உச்சி_Size, tableLength-உச்சி_Size,
                                outBytes+tableStartOffset+உச்சி_Size, status);
        }
    }

    // Trie table for character categories
    ucptrie_swap(ds, inBytes+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     outBytes+ds->readUInt32(rbbiDH->fTrie), status);

    // Source Rules Text.  It's UTF8 data
    எனில் (outBytes != inBytes) {
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
    ds->swapArray32(ds, inBytes, அளவு(RBBIDataHeader), outBytes, status);
    ds->swapArray32(ds, outputDH->fFormatVersion, 4, outputDH->fFormatVersion, status);

    திரும்பு totalSize;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
