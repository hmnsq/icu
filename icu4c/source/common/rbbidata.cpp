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
RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIDataHeader *data, UErrorCode &status) {
    init0();
    init(data, status);
}

RBBIDataWrapper::RBBIDataWrapper(ثابت RBBIDataHeader *data, تعداد EDontAdopt, UErrorCode &status) {
    init0();
    init(data, status);
    fDontFreeData = true;
}

RBBIDataWrapper::RBBIDataWrapper(UDataMemory* udm, UErrorCode &status) {
    init0();
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    ثابت DataHeader *dh = udm->pHeader;
    int32_t headerSize = dh->dataHeader.headerSize;
    إذا (  !(headerSize >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.dataFormat[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.dataFormat[1] == 0x72 &&
            dh->info.dataFormat[2] == 0x6b &&
            dh->info.dataFormat[3] == 0x20 &&
            isDataVersionAcceptable(dh->info.formatVersion))
        ) {
        status = U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    ثابت حرف *dataAsBytes = reinterpret_cast<ثابت حرف *>(dh);
    ثابت RBBIDataHeader *rbbidh = reinterpret_cast<ثابت RBBIDataHeader *>(dataAsBytes + headerSize);
    init(rbbidh, status);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::isDataVersionAcceptable(ثابت UVersionInfo version) {
    أرجع RBBI_DATA_FORMAT_VERSION[0] == version[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
فراغ RBBIDataWrapper::init0() {
    fHeader = nullptr;
    fForwardTable = nullptr;
    fReverseTable = nullptr;
    fRuleSource   = nullptr;
    fRuleStatusTable = nullptr;
    fTrie         = nullptr;
    fUDataMem     = nullptr;
    f_Ref_عدد     = 0;
    fDontFreeData = true;
}

فراغ RBBIDataWrapper::init(ثابت RBBIDataHeader *data, UErrorCode &status) {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    fHeader = data;
    إذا (fHeader->fMagic != 0xb1a0 || !isDataVersionAcceptable(fHeader->fFormatVersion)) {
        status = U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    fDontFreeData = خطأ;
    إذا (data->fFTableLen != 0) {
        fForwardTable = reinterpret_cast<ثابت RBBIStateTable*>(reinterpret_cast<ثابت حرف*>(data) + fHeader->fFTable);
    }
    إذا (data->fRTableLen != 0) {
        fReverseTable = reinterpret_cast<ثابت RBBIStateTable*>(reinterpret_cast<ثابت حرف*>(data) + fHeader->fRTable);
    }

    fTrie = ucptrie_open_From_ثنائي(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_قيمة_BITS_ANY,
                                   (uint8_t *)data + fHeader->fTrie,
                                   fHeader->fTrieLen,
                                   nullptr,           // *actual length
                                   &status);
    إذا (U_FAILURE(status)) {
        أرجع;
    }

    UCPTrie_قيمة_Width width = ucptrie_get_قيمة_Width(fTrie);
    إذا (!(width == UCPTRIE_قيمة_BITS_8 || width == UCPTRIE_قيمة_BITS_16)) {
        status = U_INVALID_FORMAT_ERROR;
        أرجع;
    }

    fRuleSource   = ((حرف *)data + fHeader->fRuleSource);
    fRuleString = UnicodeString::fromUTF8(StringPiece(fRuleSource, fHeader->fRuleSourceLen));
    U_ASSERT(data->fRuleSourceLen > 0);

    fRuleStatusTable = reinterpret_cast<ثابت int32_t*>(reinterpret_cast<ثابت حرف*>(data) + fHeader->fStatusTable);
    fStatusMaxIdx    = data->fStatusTableLen / حجم(int32_t);

    f_Ref_عدد = 1;

#ifdef RBBI_DEBUG
    حرف *debugEnv = getenv("U_RBBIDEBUG");
    إذا (debugEnv && uprv_strstr(debugEnv, "data")) {هذا->اطبع_Data();}
#endif
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_عدد == 0);
    ucptrie_close(fTrie);
    fTrie = nullptr;
    إذا (fUDataMem) {
        udata_close(fUDataMem);
    } وإلا إذا (!fDontFreeData) {
        uprv_free((فراغ *)fHeader);
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
منطقي RBBIDataWrapper::operator ==(ثابت RBBIDataWrapper &other) ثابت {
    إذا (fHeader == other.fHeader) {
        أرجع true;
    }
    إذا (fHeader->fLength != other.fHeader->fLength) {
        أرجع خطأ;
    }
    إذا (uprv_memcmp(fHeader, other.fHeader, fHeader->fLength) == 0) {
        أرجع true;
    }
    أرجع خطأ;
}

int32_t  RBBIDataWrapper::hashCode() {
    أرجع fHeader->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
فراغ RBBIDataWrapper::removeReference() {
    إذا (umtx_atomic_dec(&f_Ref_عدد) == 0) {
        delete هذا;
    }
}


RBBIDataWrapper *RBBIDataWrapper::addReference() {
   umtx_atomic_inc(&f_Ref_عدد);
   أرجع هذا;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
ثابت UnicodeString &RBBIDataWrapper::getRuleSourceString() ثابت {
    أرجع fRuleString;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
فراغ  RBBIDataWrapper::اطبع_Table(ثابت حرف *heading, ثابت RBBIStateTable *table) {
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
    لكل (c=0; c<fHeader->f_Cat_عدد; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); لكل (c=0;c<fHeader->f_Cat_عدد; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    إذا (table == nullptr) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        أرجع;
    }
    UBool use8Bits = table->fFlags & RBBI_8BITS_ROWS;
    لكل (s=0; s<table->f_رقم_States; s++) {
        RBBIStateTableRow *row = (RBBIStateTableRow *)
                                  (table->fTableData + (table->fRowLen * s));
        إذا (use8Bits) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r8.fAccepting, row->r8.fLookAhead, row->r8.fTagsIdx);
            لكل (c=0; c<fHeader->f_Cat_عدد; c++)  {
                RBBIDebugPrintf("%3d ", row->r8.fNextState[c]);
            }
        } وإلا {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r16.fAccepting, row->r16.fLookAhead, row->r16.fTagsIdx);
            لكل (c=0; c<fHeader->f_Cat_عدد; c++)  {
                RBBIDebugPrintf("%3d ", row->r16.fNextState[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#endif


فراغ  RBBIDataWrapper::اطبع_Data() {
#ifdef RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (فراغ *)fHeader);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", fHeader->fFormatVersion[0], fHeader->fFormatVersion[1],
                                                    fHeader->fFormatVersion[2], fHeader->fFormatVersion[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", fHeader->fLength);
    RBBIDebugPrintf("   number of character categories = %d\n\n", fHeader->f_Cat_عدد);

    اطبع_Table("Forward State Transition Table", fForwardTable);
    اطبع_Table("Reverse State Transition Table", fReverseTable);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    لكل (int32_t c=0; fRuleSource[c] != 0; c++) {
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
ubrk_swap(ثابت UDataSwapper *ds, ثابت فراغ *inData, int32_t length, فراغ *outData,
           UErrorCode *status) {

    إذا (status == nullptr || U_FAILURE(*status)) {
        أرجع 0;
    }
    إذا(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    ثابت UDataInfo *pInfo = (ثابت UDataInfo *)((ثابت حرف *)inData+4);
    إذا(!(  pInfo->dataFormat[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->dataFormat[1]==0x72 &&
           pInfo->dataFormat[2]==0x6b &&
           pInfo->dataFormat[3]==0x20 &&
           RBBIDataWrapper::isDataVersionAcceptable(pInfo->formatVersion) )) {
        udata_اطبع_Error(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0]);
        *status=U_UNSUPPORTED_ERROR;
        أرجع 0;
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
    إذا (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::isDataVersionAcceptable(rbbiDH->fFormatVersion) ||
            ds->readUInt32(rbbiDH->fLength)  <  حجم(RBBIDataHeader)) {
        udata_اطبع_Error(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *status=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t breakDataLength = ds->readUInt32(rbbiDH->fLength);
    int32_t totalSize = headerSize + breakDataLength;
    إذا (length < 0) {
        أرجع totalSize;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    إذا (length < totalSize) {
        udata_اطبع_Error(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            breakDataLength);
        *status=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
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
    إذا (inBytes != outBytes) {
        uprv_memset(outBytes, 0, breakDataLength);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         قمة_Size = offsetof(RBBIStateTable, fTableData);

    // Forward state table.
    tableStartOffset = ds->readUInt32(rbbiDH->fFTable);
    tableLength      = ds->readUInt32(rbbiDH->fFTableLen);

    إذا (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, قمة_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        إذا (use8Bits) {
            إذا (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+قمة_Size,
                             inBytes+tableStartOffset+قمة_Size,
                             tableLength-قمة_Size);
            }
        } وإلا {
            ds->swapArray16(ds, inBytes+tableStartOffset+قمة_Size, tableLength-قمة_Size,
                                outBytes+tableStartOffset+قمة_Size, status);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    tableStartOffset = ds->readUInt32(rbbiDH->fRTable);
    tableLength      = ds->readUInt32(rbbiDH->fRTableLen);

    إذا (tableLength > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, قمة_Size,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        إذا (use8Bits) {
            إذا (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+قمة_Size,
                             inBytes+tableStartOffset+قمة_Size,
                             tableLength-قمة_Size);
            }
        } وإلا {
            ds->swapArray16(ds, inBytes+tableStartOffset+قمة_Size, tableLength-قمة_Size,
                                outBytes+tableStartOffset+قمة_Size, status);
        }
    }

    // Trie table for character categories
    ucptrie_swap(ds, inBytes+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     outBytes+ds->readUInt32(rbbiDH->fTrie), status);

    // Source Rules Text.  It's UTF8 data
    إذا (outBytes != inBytes) {
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
    ds->swapArray32(ds, inBytes, حجم(RBBIDataHeader), outBytes, status);
    ds->swapArray32(ds, outputDH->fFormatVersion, 4, outputDH->fFormatVersion, status);

    أرجع totalSize;
}


#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
