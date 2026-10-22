// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 1999-2014 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "unicode/ucptrie.h"
#समावेश "unicode/utypes.h"
#समावेश "rbbidata.h"
#समावेश "rbbirb.h"
#समावेश "udatamem.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "umutex.h"

#समावेश "uassert.h"


U_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//    Constructors.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::RBBIDataWrapper(स्थिर RBBIDataHeader *तथ्य, UErrorCode &status) {
    init0();
    init(तथ्य, status);
}

RBBIDataWrapper::RBBIDataWrapper(स्थिर RBBIDataHeader *तथ्य, क्रमागत EDontAdopt, UErrorCode &status) {
    init0();
    init(तथ्य, status);
    f_Dont_Free_तथ्य = true;
}

RBBIDataWrapper::RBBIDataWrapper(UDataMemory* udm, UErrorCode &status) {
    init0();
    अगर (U_FAILURE(status)) {
        वापस;
    }
    स्थिर तथ्य_Header *dh = udm->pHeader;
    int32_t header_आकार = dh->तथ्य_Header_2.header_आकार;
    अगर (  !(header_आकार >= 20 &&
            dh->info.isBigEndian == U_IS_BIG_ENDIAN &&
            dh->info.charsetFamily == U_CHARSET_FAMILY &&
            dh->info.तथ्य_Format[0] == 0x42 &&  // dataFormat="Brk "
            dh->info.तथ्य_Format[1] == 0x72 &&
            dh->info.तथ्य_Format[2] == 0x6b &&
            dh->info.तथ्य_Format[3] == 0x20 &&
            is_तथ्य_Version_Acceptable(dh->info.formatVersion))
        ) {
        status = U_INVALID_FORMAT_ERROR;
        वापस;
    }
    स्थिर अक्षर *तथ्य_As_Bytes = reinterpret_cast<स्थिर अक्षर *>(dh);
    स्थिर RBBIDataHeader *rbbidh = reinterpret_cast<स्थिर RBBIDataHeader *>(तथ्य_As_Bytes + header_आकार);
    init(rbbidh, status);
    fUDataMem = udm;
}

UBool RBBIDataWrapper::is_तथ्य_Version_Acceptable(स्थिर UVersionInfo version) {
    वापस RBBI_तथ्य_FORMAT_VERSION[0] == version[0];
}


//-----------------------------------------------------------------------------
//
//    init().   Does most of the work of construction, shared between the
//              constructors.
//
//-----------------------------------------------------------------------------
व्योम RBBIDataWrapper::init0() {
    fHeader = nullptr;
    fForwardTable = nullptr;
    fReverseTable = nullptr;
    fRuleSource   = nullptr;
    fRuleStatusTable = nullptr;
    fTrie         = nullptr;
    fUDataMem     = nullptr;
    f_Ref_गिनती     = 0;
    f_Dont_Free_तथ्य = true;
}

व्योम RBBIDataWrapper::init(स्थिर RBBIDataHeader *तथ्य, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    fHeader = तथ्य;
    अगर (fHeader->fMagic != 0xb1a0 || !is_तथ्य_Version_Acceptable(fHeader->fFormatVersion)) {
        status = U_INVALID_FORMAT_ERROR;
        वापस;
    }
    // Note: in ICU version 3.2 and earlier, there was a formatVersion 1
    //       that is no longer supported.  At that time fFormatVersion was
    //       an int32_t field, rather than an array of 4 bytes.

    f_Dont_Free_तथ्य = false;
    अगर (तथ्य->fFTableLen != 0) {
        fForwardTable = reinterpret_cast<स्थिर RBBIStateTable*>(reinterpret_cast<स्थिर अक्षर*>(तथ्य) + fHeader->fFTable);
    }
    अगर (तथ्य->fRTableLen != 0) {
        fReverseTable = reinterpret_cast<स्थिर RBBIStateTable*>(reinterpret_cast<स्थिर अक्षर*>(तथ्य) + fHeader->fRTable);
    }

    fTrie = ucptrie_open_From_द्विआधारी(UCPTRIE_TYPE_FAST,
                                   UCPTRIE_मूल्य_BITS_ANY,
                                   (uint8_t *)तथ्य + fHeader->fTrie,
                                   fHeader->fTrieLen,
                                   nullptr,           // *actual length
                                   &status);
    अगर (U_FAILURE(status)) {
        वापस;
    }

    UCPTrie_मूल्य_Width width = ucptrie_get_मूल्य_Width(fTrie);
    अगर (!(width == UCPTRIE_मूल्य_BITS_8 || width == UCPTRIE_मूल्य_BITS_16)) {
        status = U_INVALID_FORMAT_ERROR;
        वापस;
    }

    fRuleSource   = ((अक्षर *)तथ्य + fHeader->fRuleSource);
    fRuleString = UnicodeString::fromUTF8(StringPiece(fRuleSource, fHeader->fRuleSourceLen));
    U_ASSERT(तथ्य->fRuleSourceLen > 0);

    fRuleStatusTable = reinterpret_cast<स्थिर int32_t*>(reinterpret_cast<स्थिर अक्षर*>(तथ्य) + fHeader->fStatusTable);
    fStatusMaxIdx    = तथ्य->fStatusTableLen / माप(int32_t);

    f_Ref_गिनती = 1;

#अगर_घोषित RBBI_DEBUG
    अक्षर *debugEnv = दो_पर्या("U_RBBIDEBUG");
    अगर (debugEnv && uprv_strstr(debugEnv, "data")) {यह->print_तथ्य();}
#पूर्ण_अगर
}


//-----------------------------------------------------------------------------
//
//    Destructor.     Don't call this - use removeReference() instead.
//
//-----------------------------------------------------------------------------
RBBIDataWrapper::~RBBIDataWrapper() {
    U_ASSERT(f_Ref_गिनती == 0);
    ucptrie_close(fTrie);
    fTrie = nullptr;
    अगर (fUDataMem) {
        udata_close(fUDataMem);
    } अन्यथा अगर (!f_Dont_Free_तथ्य) {
        uprv_free((व्योम *)fHeader);
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
bool RBBIDataWrapper::चालक ==(स्थिर RBBIDataWrapper &other) स्थिर {
    अगर (fHeader == other.fHeader) {
        वापस true;
    }
    अगर (fHeader->f_लंबाई != other.fHeader->f_लंबाई) {
        वापस false;
    }
    अगर (uprv_memcmp(fHeader, other.fHeader, fHeader->f_लंबाई) == 0) {
        वापस true;
    }
    वापस false;
}

int32_t  RBBIDataWrapper::hashCode() {
    वापस fHeader->fFTableLen;
}



//-----------------------------------------------------------------------------
//
//    Reference Counting.   A single RBBIDataWrapper object is shared among
//                          however many RulesBasedBreakIterator instances are
//                          referencing the same data.
//
//-----------------------------------------------------------------------------
व्योम RBBIDataWrapper::removeReference() {
    अगर (umtx_atomic_dec(&f_Ref_गिनती) == 0) {
        मिटाओ यह;
    }
}


RBBIDataWrapper *RBBIDataWrapper::addReference() {
   umtx_atomic_inc(&f_Ref_गिनती);
   वापस यह;
}



//-----------------------------------------------------------------------------
//
//  getRuleSourceString
//
//-----------------------------------------------------------------------------
स्थिर UnicodeString &RBBIDataWrapper::getRuleSourceString() स्थिर {
    वापस fRuleString;
}


//-----------------------------------------------------------------------------
//
//  print   -  debugging function to dump the runtime data tables.
//
//-----------------------------------------------------------------------------
#अगर_घोषित RBBI_DEBUG
व्योम  RBBIDataWrapper::printTable(स्थिर अक्षर *heading, स्थिर RBBIStateTable *table) {
    uint32_t   c;
    uint32_t   s;

    RBBIDebugPrintf("%s\n", heading);

    RBBIDebugPrintf("   fDictCategoriesStart: %d\n", table->fDictCategoriesStart);
    RBBIDebugPrintf("   fLookAheadResultsSize: %d\n", table->f_Look_Ahead_Results_आकार);
    RBBIDebugPrintf("   Flags: %4x RBBI_LOOKAHEAD_HARD_BREAK=%s RBBI_8BITS_ROWS=%s\n",
                    table->fFlags,
                    table->fFlags & RBBI_LOOKAHEAD_HARD_BREAK ? "T" : "F",
                    table->fFlags & RBBI_8BITS_ROWS ? "T" : "F");
    RBBIDebugPrintf("\nState |  Acc  LA TagIx");
    क्रम (c=0; c<fHeader->f_Cat_गिनती; c++) {RBBIDebugPrintf("%3d ", c);}
    RBBIDebugPrintf("\n------|---------------"); क्रम (c=0;c<fHeader->f_Cat_गिनती; c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    अगर (table == nullptr) {
        RBBIDebugPrintf("         N U L L   T A B L E\n\n");
        वापस;
    }
    UBool use8Bits = table->fFlags & RBBI_8BITS_ROWS;
    क्रम (s=0; s<table->f_अंक_States; s++) {
        RBBIStateTableRow *row = (RBBIStateTableRow *)
                                  (table->f_Table_तथ्य + (table->fRowLen * s));
        अगर (use8Bits) {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r8.fAccepting, row->r8.fLookAhead, row->r8.fTagsIdx);
            क्रम (c=0; c<fHeader->f_Cat_गिनती; c++)  {
                RBBIDebugPrintf("%3d ", row->r8.f_अग्र_State[c]);
            }
        } अन्यथा {
            RBBIDebugPrintf("%4d  |  %3d %3d %3d ", s, row->r16.fAccepting, row->r16.fLookAhead, row->r16.fTagsIdx);
            क्रम (c=0; c<fHeader->f_Cat_गिनती; c++)  {
                RBBIDebugPrintf("%3d ", row->r16.f_अग्र_State[c]);
            }
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n");
}
#पूर्ण_अगर


व्योम  RBBIDataWrapper::print_तथ्य() {
#अगर_घोषित RBBI_DEBUG
    RBBIDebugPrintf("RBBI Data at %p\n", (व्योम *)fHeader);
    RBBIDebugPrintf("   Version = {%d %d %d %d}\n", fHeader->fFormatVersion[0], fHeader->fFormatVersion[1],
                                                    fHeader->fFormatVersion[2], fHeader->fFormatVersion[3]);
    RBBIDebugPrintf("   total length of data  = %d\n", fHeader->f_लंबाई);
    RBBIDebugPrintf("   number of character categories = %d\n\n", fHeader->f_Cat_गिनती);

    printTable("Forward State Transition Table", fForwardTable);
    printTable("Reverse State Transition Table", fReverseTable);

    RBBIDebugPrintf("\nOriginal Rules source:\n");
    क्रम (int32_t c=0; fRuleSource[c] != 0; c++) {
        RBBIDebugPrintf("%c", fRuleSource[c]);
    }
    RBBIDebugPrintf("\n\n");
#पूर्ण_अगर
}


U_NAMESPACE_END
U_NAMESPACE_USE

//-----------------------------------------------------------------------------
//
//  ubrk_swap   -  byte swap and char encoding swap of RBBI data
//
//-----------------------------------------------------------------------------

U_CAPI int32_t U_EXPORT2
ubrk_swap(स्थिर UDataSwapper *ds, स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
           UErrorCode *status) {

    अगर (status == nullptr || U_FAILURE(*status)) {
        वापस 0;
    }
    अगर(ds==nullptr || in_तथ्य==nullptr || length<-1 || (length>0 && out_तथ्य==nullptr)) {
        *status=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    //
    //  Check that the data header is for for break data.
    //    (Header contents are defined in genbrk.cpp)
    //
    स्थिर UDataInfo *pInfo = (स्थिर UDataInfo *)((स्थिर अक्षर *)in_तथ्य+4);
    अगर(!(  pInfo->तथ्य_Format[0]==0x42 &&   /* dataFormat="Brk " */
           pInfo->तथ्य_Format[1]==0x72 &&
           pInfo->तथ्य_Format[2]==0x6b &&
           pInfo->तथ्य_Format[3]==0x20 &&
           RBBIDataWrapper::is_तथ्य_Version_Acceptable(pInfo->formatVersion) )) {
        udata_printError(ds, "ubrk_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized\n",
                         pInfo->तथ्य_Format[0], pInfo->तथ्य_Format[1],
                         pInfo->तथ्य_Format[2], pInfo->तथ्य_Format[3],
                         pInfo->formatVersion[0]);
        *status=U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    //
    // Swap the data header.  (This is the generic ICU Data Header, not the RBBI Specific
    //                         RBBIDataHeader).  This swap also conveniently gets us
    //                         the size of the ICU d.h., which lets us locate the start
    //                         of the RBBI specific data.
    //
    int32_t header_आकार=udata_swap_तथ्य_Header(ds, in_तथ्य, length, out_तथ्य, status);


    //
    // Get the RRBI Data Header, and check that it appears to be OK.
    //
    स्थिर uint8_t  *inBytes =(स्थिर uint8_t *)in_तथ्य+header_आकार;
    RBBIDataHeader *rbbiDH = (RBBIDataHeader *)inBytes;
    अगर (ds->readUInt32(rbbiDH->fMagic) != 0xb1a0 || 
            !RBBIDataWrapper::is_तथ्य_Version_Acceptable(rbbiDH->fFormatVersion) ||
            ds->readUInt32(rbbiDH->f_लंबाई)  <  माप(RBBIDataHeader)) {
        udata_printError(ds, "ubrk_swap(): RBBI Data header is invalid.\n");
        *status=U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    //
    // Prefight operation?  Just return the size
    //
    int32_t break_तथ्य_लंबाई = ds->readUInt32(rbbiDH->f_लंबाई);
    int32_t योग_आकार = header_आकार + break_तथ्य_लंबाई;
    अगर (length < 0) {
        वापस योग_आकार;
    }

    //
    // Check that length passed in is consistent with length from RBBI data header.
    //
    अगर (length < योग_आकार) {
        udata_printError(ds, "ubrk_swap(): too few bytes (%d after ICU Data header) for break data.\n",
                            break_तथ्य_लंबाई);
        *status=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
        }


    //
    // Swap the Data.  Do the data itself first, then the RBBI Data Header, because
    //                 we need to reference the header to locate the data, and an
    //                 inplace swap of the header leaves it unusable.
    //
    uint8_t         *outBytes = (uint8_t *)out_तथ्य + header_आकार;
    RBBIDataHeader  *outputDH = (RBBIDataHeader *)outBytes;

    int32_t   tableStartOffset;
    int32_t   table_लंबाई;

    //
    // If not swapping in place, zero out the output buffer before starting.
    //    Individual tables and other data items within are aligned to 8 byte boundaries
    //    when originally created.  Any unused space between items needs to be zero.
    //
    अगर (inBytes != outBytes) {
        uprv_memset(outBytes, 0, break_तथ्य_लंबाई);
    }

    //
    // Each state table begins with several 32 bit fields.  Calculate the size
    //   in bytes of these.
    //
    int32_t         शीर्ष_आकार = दुरत्व(RBBIStateTable, f_Table_तथ्य);

    // Forward state table.
    tableStartOffset = ds->readUInt32(rbbiDH->fFTable);
    table_लंबाई      = ds->readUInt32(rbbiDH->fFTableLen);

    अगर (table_लंबाई > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, शीर्ष_आकार,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        अगर (use8Bits) {
            अगर (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+शीर्ष_आकार,
                             inBytes+tableStartOffset+शीर्ष_आकार,
                             table_लंबाई-शीर्ष_आकार);
            }
        } अन्यथा {
            ds->swapArray16(ds, inBytes+tableStartOffset+शीर्ष_आकार, table_लंबाई-शीर्ष_आकार,
                                outBytes+tableStartOffset+शीर्ष_आकार, status);
        }
    }

    // Reverse state table.  Same layout as forward table, above.
    tableStartOffset = ds->readUInt32(rbbiDH->fRTable);
    table_लंबाई      = ds->readUInt32(rbbiDH->fRTableLen);

    अगर (table_लंबाई > 0) {
        RBBIStateTable *rbbiST = (RBBIStateTable *)(inBytes+tableStartOffset);
        UBool use8Bits = ds->readUInt32(rbbiST->fFlags) & RBBI_8BITS_ROWS;

        ds->swapArray32(ds, inBytes+tableStartOffset, शीर्ष_आकार,
                            outBytes+tableStartOffset, status);

        // Swap the state table if the table is in 16 bits.
        अगर (use8Bits) {
            अगर (outBytes != inBytes) {
                uprv_memmove(outBytes+tableStartOffset+शीर्ष_आकार,
                             inBytes+tableStartOffset+शीर्ष_आकार,
                             table_लंबाई-शीर्ष_आकार);
            }
        } अन्यथा {
            ds->swapArray16(ds, inBytes+tableStartOffset+शीर्ष_आकार, table_लंबाई-शीर्ष_आकार,
                                outBytes+tableStartOffset+शीर्ष_आकार, status);
        }
    }

    // Trie table for character categories
    ucptrie_swap(ds, inBytes+ds->readUInt32(rbbiDH->fTrie), ds->readUInt32(rbbiDH->fTrieLen),
                     outBytes+ds->readUInt32(rbbiDH->fTrie), status);

    // Source Rules Text.  It's UTF8 data
    अगर (outBytes != inBytes) {
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
    ds->swapArray32(ds, inBytes, माप(RBBIDataHeader), outBytes, status);
    ds->swapArray32(ds, outputDH->fFormatVersion, 4, outputDH->fFormatVersion, status);

    वापस योग_आकार;
}


#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */
