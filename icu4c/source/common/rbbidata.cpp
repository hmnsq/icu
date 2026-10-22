// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 1999-2014 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#जइ !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "rbbidata.h"
#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "udatamem.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "umutex.h"

#अन्तर्भाव "uassert.h"


U_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//    Constructors.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::RBBIDataWrapper(ध्रुव RBBIDataHeader *data, UErrorCode &status) {
    init0();
    init(data, status);
}

RBBIDataWrapper::RBBIDataWrapper(ध्रुव RBBIDataHeader *data, गणना EDontAdopt, UErrorCode &status) {
    init0();
    init(data, status);
    fDontFreeData = सच्च;
}

RBBIDataWrapper::RBBIDataWrapper(UDataMemory* udm, UErrorCode &status) {
    init0();
    जइ (U_FAILURE(status)) {
        प्रत्यागम;
    }
    ध्रुव DataHeader *dh = udm->pHeader;
    int32_t headerSize = dh->dataHeader.headerSize;
    जइ (  !(headerSize >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.dataFormat[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.dataFormat[1] == 0x72 &&
            dh->info.dataFormat[2] == 0x6b &&
            dh->info.dataFormat[3] == 0x20 &&
            isDataVersionAcceptable(dh->info.formatVersion))
        ) {
        status = U_INVALID_FORMAT_ERROR;
        प्रत्यागम;
    }
    ध्रुव वर्ण *dataAsBytes = reinterpret_cast<ध्रुव वर्ण *>(dh);
    ध्रुव RBBIDataHeader *rbbidh = reinterpret_cast<ध्रुव RBBIDataHeader *>(dataAsBytes + headerSize);
    init(rbbidh, status);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::isDataVersionAcceptable(ध्रुव UVersionInfo version) {
    प्रत्यागम RBBI_DATA_FORMAT_VERSION[0] == version[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
शून्य RBBIDataWrapper::init0() {
    fHeader = nullptr;
    fForwardTable = nullptr;
    fReverseTable = nullptr;
    fRuleSource   = nullptr;
    fRuleStatusTable = nullptr;
    fTrie         = nullptr;
    fUDataMem     = nullptr;
    f_Ref_गणनम्     = 0;
    fDontFreeData = सच्च;
}

शून्य RBBIDataWrapper::init(ध्रुव RBBIDataHeader *data, UErrorCode &status) {
    जइ (U_FAILURE(status)) {
        प्रत्यागम;
    }
    fHeader = data;
    जइ (fHeader->fMagic != 0xb1a0 || !isDataVersionAcceptable(fHeader->fFormatVersion)) {
        status = U_INVALID_FORMAT_ERROR;
        प्रत्यागम;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    fDontFreeData = मिच्छ;
    जइ (data->fFTableLen != 0) {
        fForwardTable = reinterpret_cast<ध्रुव RBBIStateTable*>(reinterpret_cast<ध्रुव वर्ण*>(data) + fHeader->fFTable);
    }
    जइ (data->fRTableLen != 0) {
        fReverseTable = reinterpret_cast<ध्रुव RBBIStateTable*>(reinterpret_cast<ध्रुव वर्ण*>(data) + fHeader->fRTable);
    }

    fTrie = ucptrie_open_From_द्विमान(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_मानम्_BITS_ANY,
                                   (uint8_t *)data + fHeader->fTrie,
                                   fHeader->fTrieLen,
                                   nullptr,           // *actual length
                                   &status);
    जइ (U_FAILURE(status)) {
        प्रत्यागम;
    }

    UCPTrie_मानम्_Width width = ucptrie_get_मानम्_Width(fTrie);
    जइ (!(width == UCPTRIE_मानम्_BITS_8 || width == UCPTRIE_मानम्_BITS_16)) {
        status = U_INVALID_FORMAT_ERROR;
        प्रत्यागम;
    }

    fRuleSource   = ((वर्ण *)data + fHeader->fRuleSource);
    fRuleString = UnicodeString::fromUTF8(StringPiece(fRuleSource, fHeader->fRuleSourceLen));
    U_ASSERT(data->fRuleSourceLen > 0);

    fRuleStatusTable = reinterpret_cast<ध्रुव int32_t*>(reinterpret_cast<ध्रुव वर्ण*>(data) + fHeader->fStatusTable);
    fStatusMaxIdx    = data->fStatusTableLen / परिमाण(int32_t);

    f_Ref_गणनम् = 1;

#ifdef RBBI_DEBUG
    वर्ण *debugEnv = getenv("U_RBBIDEBUG");
    जइ (debugEnv && uprv_strstr(debugEnv, "data")) {एतत्->printData();}
#endif
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_गणनम् == 0);
    ucptrie_close(fTrie);
    fTrie = nullptr;
    जइ (fUDataMem) {
        udata_close(fUDataMem);
    } अण्णहा जइ (!fDontFreeData) {
        uprv_free((शून्य *)fHeader);
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
तार्किक RBBIDataWrapper::operator ==(ध्रुव RBBIDataWrapper &other) ध्रुव {
    जइ (fHeader == other.fHeader) {
        प्रत्यागम सच्च;
    }
    जइ (fHeader->fLength != other.fHeader->fLength) {
        प्रत्यागम मिच्छ;
    }
    जइ (uprv_memcmp(fHeader, other.fHeader, fHeader->fLength) == 0) {
        प्रत्यागम सच्च;
    }
    प्रत्यागम मिच्छ;
}

int32_t  RBBIDataWrapper::hashCode() {
    प्रत्यागम fHeader->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
शून्य RBBIDataWrapper::removeReference() {
    जइ (umtx_atomic_dec(&f_Ref_गणनम्) == 0) {
        delete एतत्;
    }
}


RBBIDataWrapper *RBBIDataWrapper::addReference() {
   umtx_atomic_inc(&f_Ref_गणनम्);
   प्रत्यागम एतत्;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ध्रुव UnicodeString &RBBIDataWrapper::getRuleSourceString() ध्रुव {
    प्रत्यागम fRuleString;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
शून्य  RBBIDataWrapper::printTable(ध्रुव वर्ण *heading, ध्रुव RBBIStateTable *table) {
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
    प्रति (c=0; c<fHeader->f_Cat_गणनम्; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); प्रति (c=0;c<fHeader->f_Cat_गणनम्; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    जइ (table == nullptr) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        प्रत्यागम;
    }
    UBool use8Bits = table->fFlags & RBBI_8BITS_ROWS;
    प्रति (s=0; s<table->f_अङ्कः_States; s++) {
        RBBIStateTableRow *row = (RBBIStateTableRow *)
                                  (table->fTableData + (table->fRowLen * s));
        जइ (use8Bits) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r8.fAccepting, row->r8.fLookAhead, row->r8.fTagsIdx);
            प्रति (c=0; c<fHeader->f_Cat_गणनम्; c++)  {
                RBBIDebugPrintf("%3d ", row->r8.fNextState[c]);
            }
        } अण्णहा {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r16.fAccepting, row->r16.fLookAhead, row->r16.fTagsIdx);
            प्रति (c=0; c<fHeader->f_Cat_गणनम्; c++)  {
                RBBIDebugPrintf("%3d ", row->r16.fNextState[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#endif


शून्य  RBBIDataWrapper::printData() {
#ifdef RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (शून्य *)fHeader);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", fHeader->fFormatVersion[0], fHeader->fFormatVersion[1],
                                                    fHeader->fFormatVersion[2], fHeader->fFormatVersion[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", fHeader->fLength);
    RBBIDebugPrintf("   number of character categories = %d\n\n", fHeader->f_Cat_गणनम्);

    printTable("Forward State Transition Table", fForwardTable);
    printTable("Reverse State Transition Table", fReverseTable);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    प्रति (int32_t c=0; fRuleSource[c] != 0; c++) {
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
ubrk_swap(ध्रुव UDataSwapper *ds, ध्रुव शून्य *inData, int32_t length, शून्य *outData,
           UErrorCode *status) {

    जइ (status == nullptr || U_FAILURE(*status)) {
        प्रत्यागम 0;
    }
    जइ(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ध्रुव UDataInfo *pInfo = (ध्रुव UDataInfo *)((ध्रुव वर्ण *)inData+4);
    जइ(!(  pInfo->dataFormat[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->dataFormat[1]==0x72 &&
           pInfo->dataFormat[2]==0x6b &&
           pInfo->dataFormat[3]==0x20 &&
           RBBIDataWrapper::isDataVersionAcceptable(pInfo->formatVersion) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0]);
        *status=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
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
    ध्रुव uint8_t  *inBytes =(ध्रुव uint8_t *)inData+headerSize;
    RBBIDataHeader *rbbiDH = (RBBIDataHeader *)inBytes;
    जइ (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::isDataVersionAcceptable(rbbiDH->fFormatVersion) ||
            ds->readUInt32(rbbiDH->fLength)  <  परिमाण(RBBIDataHeader)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *status=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t breakDataLength = ds->readUInt32(rbbiDH->fLength);
    int32_t totalSize = headerSize + breakDataLength;
    जइ (length < 0) {
        प्रत्यागम totalSize;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    जइ (length < totalSize) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            breakDataLength);
        *status=U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
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
    जइ (inBytes != outBytes) {
        uprv_memset(outBytes, 0, breakDataLength);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         शिखरम्_Size = offsetof(RBBIStateTable, fTableData);

    // Forward state table.
    tableStartOffset = ds->readUInt32(rbbiDH->fFTable);
    tableLength      = ds->readUInt32(rbbiDH->fFTableLen);

    जइ (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, शिखरम्_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        जइ (use8Bits) {
            जइ (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+शिखरम्_Size,
                             inBytes+tableStartOffset+शिखरम्_Size,
                             tableLength-शिखरम्_Size);
            }
        } अण्णहा {
            ds->swapArray16(ds, inBytes+tableStartOffset+शिखरम्_Size, tableLength-शिखरम्_Size,
                                outBytes+tableStartOffset+शिखरम्_Size, status);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    tableStartOffset = ds->readUInt32(rbbiDH->fRTable);
    tableLength      = ds->readUInt32(rbbiDH->fRTableLen);

    जइ (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, शिखरम्_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        जइ (use8Bits) {
            जइ (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+शिखरम्_Size,
                             inBytes+tableStartOffset+शिखरम्_Size,
                             tableLength-शिखरम्_Size);
            }
        } अण्णहा {
            ds->swapArray16(ds, inBytes+tableStartOffset+शिखरम्_Size, tableLength-शिखरम्_Size,
                                outBytes+tableStartOffset+शिखरम्_Size, status);
        }
    }

    // Trie table for character categories
    ucptrie_swap(ds, inBytes+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     outBytes+ds->readUInt32(rbbiDH->fTrie), status);

    // Source Rules Text.  It's UTF8 data
    जइ (outBytes != inBytes) {
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
    ds->swapArray32(ds, inBytes, परिमाण(RBBIDataHeader), outBytes, status);
    ds->swapArray32(ds, outputDH->fFormatVersion, 4, outputDH->fFormatVersion, status);

    प्रत्यागम totalSize;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
