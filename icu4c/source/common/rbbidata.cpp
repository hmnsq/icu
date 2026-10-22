// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 1999-2014 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "unicode/ucptrie.h"
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "rbbidata.h"
#അന്തര്ഭാവ "rbbirb.h"
#അന്തര്ഭാവ "udatamem.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "umutex.h"

#അന്തര്ഭാവ "uassert.h"


U_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//    Constructors.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::RBBIDataWrapper(സ്ഥിരം RBBIDataHeader *data, UErrorCode &status) {
    init0();
    init(data, status);
}

RBBIDataWrapper::RBBIDataWrapper(സ്ഥിരം RBBIDataHeader *data, എണ്ണൽ EDontAdopt, UErrorCode &status) {
    init0();
    init(data, status);
    fDontFreeData = സത്യം;
}

RBBIDataWrapper::RBBIDataWrapper(UDataMemory* udm, UErrorCode &status) {
    init0();
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    സ്ഥിരം DataHeader *dh = udm->pHeader;
    int32_t headerSize = dh->dataHeader.headerSize;
    എങ്കിൽ (  !(headerSize >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.dataFormat[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.dataFormat[1] == 0x72 &&
            dh->info.dataFormat[2] == 0x6b &&
            dh->info.dataFormat[3] == 0x20 &&
            isDataVersionAcceptable(dh->info.formatVersion))
        ) {
        status = U_INVALID_FORMAT_ERROR;
        തിരികെ;
    }
    സ്ഥിരം അക്ഷരം *dataAsBytes = reinterpret_cast<സ്ഥിരം അക്ഷരം *>(dh);
    സ്ഥിരം RBBIDataHeader *rbbidh = reinterpret_cast<സ്ഥിരം RBBIDataHeader *>(dataAsBytes + headerSize);
    init(rbbidh, status);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::isDataVersionAcceptable(സ്ഥിരം UVersionInfo version) {
    തിരികെ RBBI_DATA_FORMAT_VERSION[0] == version[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
ശൂന്യം RBBIDataWrapper::init0() {
    fHeader = nullptr;
    fForwardTable = nullptr;
    fReverseTable = nullptr;
    fRuleSource   = nullptr;
    fRuleStatusTable = nullptr;
    fTrie         = nullptr;
    fUDataMem     = nullptr;
    f_Ref_എണ്ണം     = 0;
    fDontFreeData = സത്യം;
}

ശൂന്യം RBBIDataWrapper::init(സ്ഥിരം RBBIDataHeader *data, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    fHeader = data;
    എങ്കിൽ (fHeader->fMagic != 0xb1a0 || !isDataVersionAcceptable(fHeader->fFormatVersion)) {
        status = U_INVALID_FORMAT_ERROR;
        തിരികെ;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    fDontFreeData = അസത്യം;
    എങ്കിൽ (data->fFTableLen != 0) {
        fForwardTable = reinterpret_cast<സ്ഥിരം RBBIStateTable*>(reinterpret_cast<സ്ഥിരം അക്ഷരം*>(data) + fHeader->fFTable);
    }
    എങ്കിൽ (data->fRTableLen != 0) {
        fReverseTable = reinterpret_cast<സ്ഥിരം RBBIStateTable*>(reinterpret_cast<സ്ഥിരം അക്ഷരം*>(data) + fHeader->fRTable);
    }

    fTrie = ucptrie_open_From_ദ്വിമാന(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_മൂല്യം_BITS_ANY,
                                   (uint8_t *)data + fHeader->fTrie,
                                   fHeader->fTrieLen,
                                   nullptr,           // *actual length
                                   &status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }

    UCPTrie_മൂല്യം_Width width = ucptrie_get_മൂല്യം_Width(fTrie);
    എങ്കിൽ (!(width == UCPTRIE_മൂല്യം_BITS_8 || width == UCPTRIE_മൂല്യം_BITS_16)) {
        status = U_INVALID_FORMAT_ERROR;
        തിരികെ;
    }

    fRuleSource   = ((അക്ഷരം *)data + fHeader->fRuleSource);
    fRuleString = UnicodeString::fromUTF8(StringPiece(fRuleSource, fHeader->fRuleSourceLen));
    U_ASSERT(data->fRuleSourceLen > 0);

    fRuleStatusTable = reinterpret_cast<സ്ഥിരം int32_t*>(reinterpret_cast<സ്ഥിരം അക്ഷരം*>(data) + fHeader->fStatusTable);
    fStatusMaxIdx    = data->fStatusTableLen / വലുപ്പം(int32_t);

    f_Ref_എണ്ണം = 1;

#ifdef RBBI_DEBUG
    അക്ഷരം *debugEnv = getenv("U_RBBIDEBUG");
    എങ്കിൽ (debugEnv && uprv_strstr(debugEnv, "data")) {ഇത്->printData();}
#endif
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_എണ്ണം == 0);
    ucptrie_close(fTrie);
    fTrie = nullptr;
    എങ്കിൽ (fUDataMem) {
        udata_close(fUDataMem);
    } അല്ലെങ്കിൽ എങ്കിൽ (!fDontFreeData) {
        uprv_free((ശൂന്യം *)fHeader);
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
തർക്കം RBBIDataWrapper::operator ==(സ്ഥിരം RBBIDataWrapper &other) സ്ഥിരം {
    എങ്കിൽ (fHeader == other.fHeader) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ (fHeader->fLength != other.fHeader->fLength) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (uprv_memcmp(fHeader, other.fHeader, fHeader->fLength) == 0) {
        തിരികെ സത്യം;
    }
    തിരികെ അസത്യം;
}

int32_t  RBBIDataWrapper::hashCode() {
    തിരികെ fHeader->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
ശൂന്യം RBBIDataWrapper::removeReference() {
    എങ്കിൽ (umtx_atomic_dec(&f_Ref_എണ്ണം) == 0) {
        delete ഇത്;
    }
}


RBBIDataWrapper *RBBIDataWrapper::addReference() {
   umtx_atomic_inc(&f_Ref_എണ്ണം);
   തിരികെ ഇത്;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
സ്ഥിരം UnicodeString &RBBIDataWrapper::getRuleSourceString() സ്ഥിരം {
    തിരികെ fRuleString;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
ശൂന്യം  RBBIDataWrapper::printTable(സ്ഥിരം അക്ഷരം *heading, സ്ഥിരം RBBIStateTable *table) {
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
    വേണ്ടി (c=0; c<fHeader->f_Cat_എണ്ണം; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); വേണ്ടി (c=0;c<fHeader->f_Cat_എണ്ണം; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    എങ്കിൽ (table == nullptr) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        തിരികെ;
    }
    UBool use8Bits = table->fFlags & RBBI_8BITS_ROWS;
    വേണ്ടി (s=0; s<table->f_അക്കം_States; s++) {
        RBBIStateTableRow *row = (RBBIStateTableRow *)
                                  (table->fTableData + (table->fRowLen * s));
        എങ്കിൽ (use8Bits) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r8.fAccepting, row->r8.fLookAhead, row->r8.fTagsIdx);
            വേണ്ടി (c=0; c<fHeader->f_Cat_എണ്ണം; c++)  {
                RBBIDebugPrintf("%3d ", row->r8.fNextState[c]);
            }
        } അല്ലെങ്കിൽ {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r16.fAccepting, row->r16.fLookAhead, row->r16.fTagsIdx);
            വേണ്ടി (c=0; c<fHeader->f_Cat_എണ്ണം; c++)  {
                RBBIDebugPrintf("%3d ", row->r16.fNextState[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#endif


ശൂന്യം  RBBIDataWrapper::printData() {
#ifdef RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (ശൂന്യം *)fHeader);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", fHeader->fFormatVersion[0], fHeader->fFormatVersion[1],
                                                    fHeader->fFormatVersion[2], fHeader->fFormatVersion[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", fHeader->fLength);
    RBBIDebugPrintf("   number of character categories = %d\n\n", fHeader->f_Cat_എണ്ണം);

    printTable("Forward State Transition Table", fForwardTable);
    printTable("Reverse State Transition Table", fReverseTable);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    വേണ്ടി (int32_t c=0; fRuleSource[c] != 0; c++) {
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
ubrk_swap(സ്ഥിരം UDataSwapper *ds, സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
           UErrorCode *status) {

    എങ്കിൽ (status == nullptr || U_FAILURE(*status)) {
        തിരികെ 0;
    }
    എങ്കിൽ(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    സ്ഥിരം UDataInfo *pInfo = (സ്ഥിരം UDataInfo *)((സ്ഥിരം അക്ഷരം *)inData+4);
    എങ്കിൽ(!(  pInfo->dataFormat[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->dataFormat[1]==0x72 &&
           pInfo->dataFormat[2]==0x6b &&
           pInfo->dataFormat[3]==0x20 &&
           RBBIDataWrapper::isDataVersionAcceptable(pInfo->formatVersion) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0]);
        *status=U_UNSUPPORTED_ERROR;
        തിരികെ 0;
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
    സ്ഥിരം uint8_t  *inBytes =(സ്ഥിരം uint8_t *)inData+headerSize;
    RBBIDataHeader *rbbiDH = (RBBIDataHeader *)inBytes;
    എങ്കിൽ (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::isDataVersionAcceptable(rbbiDH->fFormatVersion) ||
            ds->readUInt32(rbbiDH->fLength)  <  വലുപ്പം(RBBIDataHeader)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *status=U_UNSUPPORTED_ERROR;
        തിരികെ 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t breakDataLength = ds->readUInt32(rbbiDH->fLength);
    int32_t totalSize = headerSize + breakDataLength;
    എങ്കിൽ (length < 0) {
        തിരികെ totalSize;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    എങ്കിൽ (length < totalSize) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            breakDataLength);
        *status=U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0;
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
    എങ്കിൽ (inBytes != outBytes) {
        uprv_memset(outBytes, 0, breakDataLength);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         മുകൾ_Size = offsetof(RBBIStateTable, fTableData);

    // Forward state table.
    tableStartOffset = ds->readUInt32(rbbiDH->fFTable);
    tableLength      = ds->readUInt32(rbbiDH->fFTableLen);

    എങ്കിൽ (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, മുകൾ_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        എങ്കിൽ (use8Bits) {
            എങ്കിൽ (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+മുകൾ_Size,
                             inBytes+tableStartOffset+മുകൾ_Size,
                             tableLength-മുകൾ_Size);
            }
        } അല്ലെങ്കിൽ {
            ds->swapArray16(ds, inBytes+tableStartOffset+മുകൾ_Size, tableLength-മുകൾ_Size,
                                outBytes+tableStartOffset+മുകൾ_Size, status);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    tableStartOffset = ds->readUInt32(rbbiDH->fRTable);
    tableLength      = ds->readUInt32(rbbiDH->fRTableLen);

    എങ്കിൽ (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, മുകൾ_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        എങ്കിൽ (use8Bits) {
            എങ്കിൽ (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+മുകൾ_Size,
                             inBytes+tableStartOffset+മുകൾ_Size,
                             tableLength-മുകൾ_Size);
            }
        } അല്ലെങ്കിൽ {
            ds->swapArray16(ds, inBytes+tableStartOffset+മുകൾ_Size, tableLength-മുകൾ_Size,
                                outBytes+tableStartOffset+മുകൾ_Size, status);
        }
    }

    // Trie table for character categories
    ucptrie_swap(ds, inBytes+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     outBytes+ds->readUInt32(rbbiDH->fTrie), status);

    // Source Rules Text.  It's UTF8 data
    എങ്കിൽ (outBytes != inBytes) {
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
    ds->swapArray32(ds, inBytes, വലുപ്പം(RBBIDataHeader), outBytes, status);
    ds->swapArray32(ds, outputDH->fFormatVersion, 4, outputDH->fFormatVersion, status);

    തിരികെ totalSize;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
