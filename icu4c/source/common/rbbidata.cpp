// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 1999-2014 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "unicode/ucptrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "rbbidata.h"
#ଅନ୍ତର୍ଭାଵ "rbbirb.h"
#ଅନ୍ତର୍ଭାଵ "udatamem.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"

#ଅନ୍ତର୍ଭାଵ "uassert.h"


U_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//    Constructors.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::RBBIDataWrapper(ଅଚଳ RBBIDataHeader *data, UErrorCode &status) {
    init0();
    init(data, status);
}

RBBIDataWrapper::RBBIDataWrapper(ଅଚଳ RBBIDataHeader *data, ଗଣନା EDontAdopt, UErrorCode &status) {
    init0();
    init(data, status);
    fDontFreeData = ସତ୍ୟ;
}

RBBIDataWrapper::RBBIDataWrapper(UDataMemory* udm, UErrorCode &status) {
    init0();
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    ଅଚଳ DataHeader *dh = udm->pHeader;
    int32_t headerSize = dh->dataHeader.headerSize;
    ଯଦି (  !(headerSize >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.dataFormat[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.dataFormat[1] == 0x72 &&
            dh->info.dataFormat[2] == 0x6b &&
            dh->info.dataFormat[3] == 0x20 &&
            isDataVersionAcceptable(dh->info.formatVersion))
        ) {
        status = U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ;
    }
    ଅଚଳ ଅକ୍ଷର *dataAsBytes = reinterpret_cast<ଅଚଳ ଅକ୍ଷର *>(dh);
    ଅଚଳ RBBIDataHeader *rbbidh = reinterpret_cast<ଅଚଳ RBBIDataHeader *>(dataAsBytes + headerSize);
    init(rbbidh, status);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::isDataVersionAcceptable(ଅଚଳ UVersionInfo version) {
    ଫେରନ୍ତୁ RBBI_DATA_FORMAT_VERSION[0] == version[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
ଶୂନ୍ୟ RBBIDataWrapper::init0() {
    fHeader = nullptr;
    fForwardTable = nullptr;
    fReverseTable = nullptr;
    fRuleSource   = nullptr;
    fRuleStatusTable = nullptr;
    fTrie         = nullptr;
    fUDataMem     = nullptr;
    f_Ref_ଗଣନା     = 0;
    fDontFreeData = ସତ୍ୟ;
}

ଶୂନ୍ୟ RBBIDataWrapper::init(ଅଚଳ RBBIDataHeader *data, UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    fHeader = data;
    ଯଦି (fHeader->fMagic != 0xb1a0 || !isDataVersionAcceptable(fHeader->fFormatVersion)) {
        status = U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    fDontFreeData = ମିଥ୍ୟା;
    ଯଦି (data->fFTableLen != 0) {
        fForwardTable = reinterpret_cast<ଅଚଳ RBBIStateTable*>(reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(data) + fHeader->fFTable);
    }
    ଯଦି (data->fRTableLen != 0) {
        fReverseTable = reinterpret_cast<ଅଚଳ RBBIStateTable*>(reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(data) + fHeader->fRTable);
    }

    fTrie = ucptrie_open_From_ଦ୍ୱିମିକ(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_ମୂଲ୍ୟ_BITS_ANY,
                                   (uint8_t *)data + fHeader->fTrie,
                                   fHeader->fTrieLen,
                                   nullptr,           // *actual length
                                   &status);
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }

    UCPTrie_ମୂଲ୍ୟ_Width width = ucptrie_get_ମୂଲ୍ୟ_Width(fTrie);
    ଯଦି (!(width == UCPTRIE_ମୂଲ୍ୟ_BITS_8 || width == UCPTRIE_ମୂଲ୍ୟ_BITS_16)) {
        status = U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ;
    }

    fRuleSource   = ((ଅକ୍ଷର *)data + fHeader->fRuleSource);
    fRuleString = UnicodeString::fromUTF8(StringPiece(fRuleSource, fHeader->fRuleSourceLen));
    U_ASSERT(data->fRuleSourceLen > 0);

    fRuleStatusTable = reinterpret_cast<ଅଚଳ int32_t*>(reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(data) + fHeader->fStatusTable);
    fStatusMaxIdx    = data->fStatusTableLen / ଆକାର(int32_t);

    f_Ref_ଗଣନା = 1;

#ifdef RBBI_DEBUG
    ଅକ୍ଷର *debugEnv = getenv("U_RBBIDEBUG");
    ଯଦି (debugEnv && uprv_strstr(debugEnv, "data")) {ଏହି->printData();}
#endif
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_ଗଣନା == 0);
    ucptrie_close(fTrie);
    fTrie = nullptr;
    ଯଦି (fUDataMem) {
        udata_close(fUDataMem);
    } ଅନ୍ୟଥା ଯଦି (!fDontFreeData) {
        uprv_free((ଶୂନ୍ୟ *)fHeader);
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
ତାର୍କିକ RBBIDataWrapper::operator ==(ଅଚଳ RBBIDataWrapper &other) ଅଚଳ {
    ଯଦି (fHeader == other.fHeader) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (fHeader->fLength != other.fHeader->fLength) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (uprv_memcmp(fHeader, other.fHeader, fHeader->fLength) == 0) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

int32_t  RBBIDataWrapper::hashCode() {
    ଫେରନ୍ତୁ fHeader->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
ଶୂନ୍ୟ RBBIDataWrapper::removeReference() {
    ଯଦି (umtx_atomic_dec(&f_Ref_ଗଣନା) == 0) {
        delete ଏହି;
    }
}


RBBIDataWrapper *RBBIDataWrapper::addReference() {
   umtx_atomic_inc(&f_Ref_ଗଣନା);
   ଫେରନ୍ତୁ ଏହି;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ଅଚଳ UnicodeString &RBBIDataWrapper::getRuleSourceString() ଅଚଳ {
    ଫେରନ୍ତୁ fRuleString;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ଶୂନ୍ୟ  RBBIDataWrapper::printTable(ଅଚଳ ଅକ୍ଷର *heading, ଅଚଳ RBBIStateTable *table) {
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
    ପାଇଁ (c=0; c<fHeader->f_Cat_ଗଣନା; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); ପାଇଁ (c=0;c<fHeader->f_Cat_ଗଣନା; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    ଯଦି (table == nullptr) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        ଫେରନ୍ତୁ;
    }
    UBool use8Bits = table->fFlags & RBBI_8BITS_ROWS;
    ପାଇଁ (s=0; s<table->f_ଅଙ୍କ_States; s++) {
        RBBIStateTableRow *row = (RBBIStateTableRow *)
                                  (table->fTableData + (table->fRowLen * s));
        ଯଦି (use8Bits) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r8.fAccepting, row->r8.fLookAhead, row->r8.fTagsIdx);
            ପାଇଁ (c=0; c<fHeader->f_Cat_ଗଣନା; c++)  {
                RBBIDebugPrintf("%3d ", row->r8.fNextState[c]);
            }
        } ଅନ୍ୟଥା {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r16.fAccepting, row->r16.fLookAhead, row->r16.fTagsIdx);
            ପାଇଁ (c=0; c<fHeader->f_Cat_ଗଣନା; c++)  {
                RBBIDebugPrintf("%3d ", row->r16.fNextState[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#endif


ଶୂନ୍ୟ  RBBIDataWrapper::printData() {
#ifdef RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (ଶୂନ୍ୟ *)fHeader);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", fHeader->fFormatVersion[0], fHeader->fFormatVersion[1],
                                                    fHeader->fFormatVersion[2], fHeader->fFormatVersion[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", fHeader->fLength);
    RBBIDebugPrintf("   number of character categories = %d\n\n", fHeader->f_Cat_ଗଣନା);

    printTable("Forward State Transition Table", fForwardTable);
    printTable("Reverse State Transition Table", fReverseTable);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    ପାଇଁ (int32_t c=0; fRuleSource[c] != 0; c++) {
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
ubrk_swap(ଅଚଳ UDataSwapper *ds, ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
           UErrorCode *status) {

    ଯଦି (status == nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ଅଚଳ UDataInfo *pInfo = (ଅଚଳ UDataInfo *)((ଅଚଳ ଅକ୍ଷର *)inData+4);
    ଯଦି(!(  pInfo->dataFormat[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->dataFormat[1]==0x72 &&
           pInfo->dataFormat[2]==0x6b &&
           pInfo->dataFormat[3]==0x20 &&
           RBBIDataWrapper::isDataVersionAcceptable(pInfo->formatVersion) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0]);
        *status=U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ 0;
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
    ଅଚଳ uint8_t  *inBytes =(ଅଚଳ uint8_t *)inData+headerSize;
    RBBIDataHeader *rbbiDH = (RBBIDataHeader *)inBytes;
    ଯଦି (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::isDataVersionAcceptable(rbbiDH->fFormatVersion) ||
            ds->readUInt32(rbbiDH->fLength)  <  ଆକାର(RBBIDataHeader)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *status=U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t breakDataLength = ds->readUInt32(rbbiDH->fLength);
    int32_t totalSize = headerSize + breakDataLength;
    ଯଦି (length < 0) {
        ଫେରନ୍ତୁ totalSize;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    ଯଦି (length < totalSize) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            breakDataLength);
        *status=U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
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
    ଯଦି (inBytes != outBytes) {
        uprv_memset(outBytes, 0, breakDataLength);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         ଶୀର୍ଷ_Size = offsetof(RBBIStateTable, fTableData);

    // Forward state table.
    tableStartOffset = ds->readUInt32(rbbiDH->fFTable);
    tableLength      = ds->readUInt32(rbbiDH->fFTableLen);

    ଯଦି (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, ଶୀର୍ଷ_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        ଯଦି (use8Bits) {
            ଯଦି (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+ଶୀର୍ଷ_Size,
                             inBytes+tableStartOffset+ଶୀର୍ଷ_Size,
                             tableLength-ଶୀର୍ଷ_Size);
            }
        } ଅନ୍ୟଥା {
            ds->swapArray16(ds, inBytes+tableStartOffset+ଶୀର୍ଷ_Size, tableLength-ଶୀର୍ଷ_Size,
                                outBytes+tableStartOffset+ଶୀର୍ଷ_Size, status);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    tableStartOffset = ds->readUInt32(rbbiDH->fRTable);
    tableLength      = ds->readUInt32(rbbiDH->fRTableLen);

    ଯଦି (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, ଶୀର୍ଷ_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        ଯଦି (use8Bits) {
            ଯଦି (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+ଶୀର୍ଷ_Size,
                             inBytes+tableStartOffset+ଶୀର୍ଷ_Size,
                             tableLength-ଶୀର୍ଷ_Size);
            }
        } ଅନ୍ୟଥା {
            ds->swapArray16(ds, inBytes+tableStartOffset+ଶୀର୍ଷ_Size, tableLength-ଶୀର୍ଷ_Size,
                                outBytes+tableStartOffset+ଶୀର୍ଷ_Size, status);
        }
    }

    // Trie table for character categories
    ucptrie_swap(ds, inBytes+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     outBytes+ds->readUInt32(rbbiDH->fTrie), status);

    // Source Rules Text.  It's UTF8 data
    ଯଦି (outBytes != inBytes) {
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
    ds->swapArray32(ds, inBytes, ଆକାର(RBBIDataHeader), outBytes, status);
    ds->swapArray32(ds, outputDH->fFormatVersion, 4, outputDH->fFormatVersion, status);

    ଫେରନ୍ତୁ totalSize;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
