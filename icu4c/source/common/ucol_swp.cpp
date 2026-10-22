// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucol_swp.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003sep10
*   created by: Markus W. Scherer
*
*   Swap collation binaries.
*/

#অন্তর্ভাব "unicode/udata.h" /* UDataInfo */
#অন্তর্ভাব "utrie.h"
#অন্তর্ভাব "utrie2.h"
#অন্তর্ভাব "udataswp.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "ucol_data.h"
#অন্তর্ভাব "ucol_swp.h"

/* swapping ----------------------------------------------------------------- */

#যদি !UCONFIG_NO_COLLATION

U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_দ্বিমিক(ধ্রুবক UDataSwapper *ds,
                              ধ্রুবক শূন্য *in_তথ্য, int32_t length) {
    যদি(ds==nullptr || in_তথ্য==nullptr || length<-1) {
        ফেরত মিথ্যা;
    }

    // First check for format version 4+ which has a standard data header.
    UErrorCode errorCode=U_ZERO_ERROR;
    (শূন্য)udata_swap_তথ্য_Header(ds, in_তথ্য, -1, nullptr, &errorCode);
    যদি(U_SUCCESS(errorCode)) {
        ধ্রুবক UDataInfo &info=*(ধ্রুবক UDataInfo *)((ধ্রুবক অক্ষর *)in_তথ্য+4);
        যদি(info.তথ্য_Format[0]==0x55 &&   // dataFormat="UCol"
                info.তথ্য_Format[1]==0x43 &&
                info.তথ্য_Format[2]==0x6f &&
                info.তথ্য_Format[3]==0x6c) {
            ফেরত সত্য;
        }
    }

    // Else check for format version 3.
    ধ্রুবক UCATableHeader *inHeader=(ধ্রুবক UCATableHeader *)in_তথ্য;

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    UCATableHeader header;
    uprv_memset(&header, 0, আকার(header));
    যদি(length<0) {
        header.size=udata_readInt32(ds, inHeader->size);
    } অন্যথা যদি((length<(42*4) || length<(header.size=udata_readInt32(ds, inHeader->size)))) {
        ফেরত মিথ্যা;
    }

    header.magic=ds->readUInt32(inHeader->magic);
    যদি(!(
        header.magic==UCOL_HEADER_MAGIC &&
        inHeader->formatVersion[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        ফেরত মিথ্যা;
    }

    যদি(inHeader->isBigEndian!=ds->inIsBigEndian || inHeader->charSetFamily!=ds->inCharset) {
        ফেরত মিথ্যা;
    }

    ফেরত সত্য;
}

নামাকাশ {

/* swap a header-less collation formatVersion=3 binary, inside a resource bundle or ucadata.icu */
int32_t
swapFormatVersion3(ধ্রুবক UDataSwapper *ds,
                   ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
                   UErrorCode *pErrorCode) {
    ধ্রুবক uint8_t *inBytes;
    uint8_t *outBytes;

    ধ্রুবক UCATableHeader *inHeader;
    UCATableHeader *outHeader;
    UCATableHeader header;

    uint32_t count;

    /* argument checking in case we were not called from ucol_swap() */
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(ds==nullptr || in_তথ্য==nullptr || length<-1 || (length>0 && out_তথ্য==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    inBytes = static_cast<ধ্রুবক uint8_t*>(in_তথ্য);
    outBytes = static_cast<uint8_t*>(out_তথ্য);

    inHeader = static_cast<ধ্রুবক UCATableHeader*>(in_তথ্য);
    outHeader = static_cast<UCATableHeader*>(out_তথ্য);

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    uprv_memset(&header, 0, আকার(header));
    যদি(length<0) {
        header.size=udata_readInt32(ds, inHeader->size);
    } অন্যথা যদি((length<(42*4) || length<(header.size=udata_readInt32(ds, inHeader->size)))) {
        udata_printError(ds, "ucol_swap(formatVersion=3): too few bytes (%d after header) for collation data\n",
                         length);
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    header.magic=ds->readUInt32(inHeader->magic);
    যদি(!(
        header.magic==UCOL_HEADER_MAGIC &&
        inHeader->formatVersion[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        udata_printError(ds, "ucol_swap(formatVersion=3): magic 0x%08x or format version %02x.%02x is not a collation binary\n",
                         header.magic,
                         inHeader->formatVersion[0], inHeader->formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    যদি(inHeader->isBigEndian!=ds->inIsBigEndian || inHeader->charSetFamily!=ds->inCharset) {
        udata_printError(ds, "ucol_swap(formatVersion=3): endianness %d or charset %d does not match the swapper\n",
                         inHeader->isBigEndian, inHeader->charSetFamily);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ফেরত 0;
    }

    যদি(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        যদি(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, header.size);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the UCATableHeader (the size field was read above) */
        header.options=                 ds->readUInt32(inHeader->options);
        header.UCAConsts=               ds->readUInt32(inHeader->UCAConsts);
        header.contractionUCACombos=    ds->readUInt32(inHeader->contractionUCACombos);
        header.mappingPosition=         ds->readUInt32(inHeader->mappingPosition);
        header.expansion=               ds->readUInt32(inHeader->expansion);
        header.contraction_সূচক=        ds->readUInt32(inHeader->contraction_সূচক);
        header.contractionCEs=          ds->readUInt32(inHeader->contractionCEs);
        header.contraction_আকার=         ds->readUInt32(inHeader->contraction_আকার);
        header.endExpansionCE=          ds->readUInt32(inHeader->endExpansionCE);
        header.expansionCESize=         ds->readUInt32(inHeader->expansionCESize);
        header.endExpansionCECount=     udata_readInt32(ds, inHeader->endExpansionCECount);
        header.contraction_UCACombos_আকার=udata_readInt32(ds, inHeader->contraction_UCACombos_আকার);
        header.scriptToLeadByte=        ds->readUInt32(inHeader->scriptToLeadByte);
        header.leadByteToScript=        ds->readUInt32(inHeader->leadByteToScript);
        
        /* swap the 32-bit integers in the header */
        ds->swapArray32(ds, inHeader, static_cast<int32_t>(reinterpret_cast<ধ্রুবক অক্ষর*>(&inHeader->jamoSpecial) - reinterpret_cast<ধ্রুবক অক্ষর*>(inHeader)),
                           outHeader, pErrorCode);
        ds->swapArray32(ds, &(inHeader->scriptToLeadByte), আকার(header.scriptToLeadByte) + আকার(header.leadByteToScript),
                           &(outHeader->scriptToLeadByte), pErrorCode);
        /* set the output platform properties */
        outHeader->isBigEndian=ds->outIsBigEndian;
        outHeader->charSetFamily=ds->outCharset;

        /* swap the options */
        যদি(header.options!=0) {
            ds->swapArray32(ds, inBytes+header.options, header.expansion-header.options,
                               outBytes+header.options, pErrorCode);
        }

        /* swap the expansions */
        যদি(header.mappingPosition!=0 && header.expansion!=0) {
            যদি(header.contraction_সূচক!=0) {
                /* expansions bounded by contractions */
                count=header.contraction_সূচক-header.expansion;
            } অন্যথা {
                /* no contractions: expansions bounded by the main trie */
                count=header.mappingPosition-header.expansion;
            }
            ds->swapArray32(ds, inBytes + header.expansion, static_cast<int32_t>(count),
                               outBytes+header.expansion, pErrorCode);
        }

        /* swap the contractions */
        যদি(header.contraction_আকার!=0) {
            /* contractionIndex: char16_t[] */
            ds->swapArray16(ds, inBytes+header.contraction_সূচক, header.contraction_আকার*2,
                               outBytes+header.contraction_সূচক, pErrorCode);

            /* contractionCEs: CEs[] */
            ds->swapArray32(ds, inBytes+header.contractionCEs, header.contraction_আকার*4,
                               outBytes+header.contractionCEs, pErrorCode);
        }

        /* swap the main trie */
        যদি(header.mappingPosition!=0) {
            count=header.endExpansionCE-header.mappingPosition;
            utrie_swap(ds, inBytes + header.mappingPosition, static_cast<int32_t>(count),
                          outBytes+header.mappingPosition, pErrorCode);
        }

        /* swap the max expansion table */
        যদি(header.endExpansionCECount!=0) {
            ds->swapArray32(ds, inBytes+header.endExpansionCE, header.endExpansionCECount*4,
                               outBytes+header.endExpansionCE, pErrorCode);
        }

        /* expansionCESize, unsafeCP, contrEndCP: uint8_t[], no need to swap */

        /* swap UCA constants */
        যদি(header.UCAConsts!=0) {
            /*
             * if UCAConsts!=0 then contractionUCACombos because we are swapping
             * the UCA data file, and we know that the UCA contains contractions
             */
            ds->swapArray32(ds, inBytes+header.UCAConsts, header.contractionUCACombos-header.UCAConsts,
                               outBytes+header.UCAConsts, pErrorCode);
        }

        /* swap UCA contractions */
        যদি(header.contraction_UCACombos_আকার!=0) {
            count=header.contraction_UCACombos_আকার*inHeader->contractionUCACombosWidth*U_SIZEOF_UCHAR;
            ds->swapArray16(ds, inBytes + header.contractionUCACombos, static_cast<int32_t>(count),
                               outBytes+header.contractionUCACombos, pErrorCode);
        }
        
        /* swap the script to lead bytes */
        যদি(header.scriptToLeadByte!=0) {
            পূর্ণসংখ্যা সূচক_গুনতি = ds->readUInt16(*((uint16_t*)(inBytes+header.scriptToLeadByte))); // each entry = 2 * uint16
            পূর্ণসংখ্যা তথ্য_গুনতি = ds->readUInt16(*((uint16_t*)(inBytes+header.scriptToLeadByte + 2))); // each entry = uint16
            ds->swapArray16(ds, inBytes+header.scriptToLeadByte, 
                                4 + (4 * সূচক_গুনতি) + (2 * তথ্য_গুনতি),
                                outBytes+header.scriptToLeadByte, pErrorCode);
        }
        
        /* swap the lead byte to scripts */
        যদি(header.leadByteToScript!=0) {
            পূর্ণসংখ্যা সূচক_গুনতি = ds->readUInt16(*((uint16_t*)(inBytes+header.leadByteToScript))); // each entry = uint16
            পূর্ণসংখ্যা তথ্য_গুনতি = ds->readUInt16(*((uint16_t*)(inBytes+header.leadByteToScript + 2))); // each entry = uint16
            ds->swapArray16(ds, inBytes+header.leadByteToScript, 
                                4 + (2 * সূচক_গুনতি) + (2 * তথ্য_গুনতি),
                                outBytes+header.leadByteToScript, pErrorCode);
        }
    }

    ফেরত header.size;
}

// swap formatVersion 4 or 5 ----------------------------------------------- ***

// The following are copied from CollationDataReader, trading an awkward copy of constants
// for an awkward relocation of the i18n collationdatareader.h file into the common library.
// Keep them in sync!

গণনা {
    IX_INDEXES_দৈর্ঘ্য,  // 0
    IX_OPTIONS,
    IX_RESERVED2,
    IX_RESERVED3,

    IX_JAMO_CE32S_START,  // 4
    IX_REORDER_CODES_OFFSET,
    IX_REORDER_TABLE_OFFSET,
    IX_TRIE_OFFSET,

    IX_RESERVED8_OFFSET,  // 8
    IX_CES_OFFSET,
    IX_RESERVED10_OFFSET,
    IX_CE32S_OFFSET,

    IX_মূল_ELEMENTS_OFFSET,  // 12
    IX_CONTEXTS_OFFSET,
    IX_UNSAFE_BWD_OFFSET,
    IX_FAST_LATIN_TABLE_OFFSET,

    IX_SCRIPTS_OFFSET,  // 16
    IX_COMPRESSIBLE_BYTES_OFFSET,
    IX_RESERVED18_OFFSET,
    IX_যোগ_আকার
};

int32_t
swapFormatVersion4(ধ্রুবক UDataSwapper *ds,
                   ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
                   UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) { ফেরত 0; }

    ধ্রুবক uint8_t* inBytes = static_cast<ধ্রুবক uint8_t*>(in_তথ্য);
    uint8_t* outBytes = static_cast<uint8_t*>(out_তথ্য);

    ধ্রুবক int32_t* inIndexes = reinterpret_cast<ধ্রুবক int32_t*>(inBytes);
    int32_t indexes[IX_যোগ_আকার+1];

    // Need at least IX_INDEXES_LENGTH and IX_OPTIONS.
    যদি(0<=length && length<8) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    int32_t indexes_দৈর্ঘ্য=indexes[0]=udata_readInt32(ds, inIndexes[0]);
    যদি(0<=length && length<(indexes_দৈর্ঘ্য*4)) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    জন্য(int32_t i=1; i<=IX_যোগ_আকার && i<indexes_দৈর্ঘ্য; ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }
    জন্য(int32_t i=indexes_দৈর্ঘ্য; i<=IX_যোগ_আকার; ++i) {
        indexes[i]=-1;
    }
    inIndexes=nullptr;  // Make sure we do not accidentally use these instead of indexes[].

    // Get the total length of the data.
    int32_t size;
    যদি(indexes_দৈর্ঘ্য>IX_যোগ_আকার) {
        size=indexes[IX_যোগ_আকার];
    } অন্যথা যদি(indexes_দৈর্ঘ্য>IX_REORDER_CODES_OFFSET) {
        size=indexes[indexes_দৈর্ঘ্য-1];
    } অন্যথা {
        size=indexes_দৈর্ঘ্য*4;
    }
    যদি(length<0) { ফেরত size; }

    যদি(length<size) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    // Copy the data for inaccessible bytes and arrays of bytes.
    যদি(inBytes!=outBytes) {
        uprv_memcpy(outBytes, inBytes, size);
    }

    // Swap the int32_t indexes[].
    ds->swapArray32(ds, inBytes, indexes_দৈর্ঘ্য * 4, outBytes, &errorCode);

    // The following is a modified version of CollationDataReader::read().
    // Here we use indexes[] not inIndexes[] because
    // the inIndexes[] may not be in this machine's endianness.
    int32_t সূচক;  // one of the indexes[] slots
    int32_t offset;  // byte offset for the index part
    // int32_t length;  // number of bytes in the index part

    সূচক = IX_REORDER_CODES_OFFSET;
    offset = indexes[সূচক];
    length = indexes[সূচক + 1] - offset;
    যদি(length > 0) {
        ds->swapArray32(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    // Skip the IX_REORDER_TABLE_OFFSET byte array.

    সূচক = IX_TRIE_OFFSET;
    offset = indexes[সূচক];
    length = indexes[সূচক + 1] - offset;
    যদি(length > 0) {
        utrie2_swap(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    সূচক = IX_RESERVED8_OFFSET;
    offset = indexes[সূচক];
    length = indexes[সূচক + 1] - offset;
    যদি(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED8_OFFSET\n", length);
        errorCode = U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    সূচক = IX_CES_OFFSET;
    offset = indexes[সূচক];
    length = indexes[সূচক + 1] - offset;
    যদি(length > 0) {
        ds->swapArray64(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    সূচক = IX_RESERVED10_OFFSET;
    offset = indexes[সূচক];
    length = indexes[সূচক + 1] - offset;
    যদি(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED10_OFFSET\n", length);
        errorCode = U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    সূচক = IX_CE32S_OFFSET;
    offset = indexes[সূচক];
    length = indexes[সূচক + 1] - offset;
    যদি(length > 0) {
        ds->swapArray32(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    সূচক = IX_মূল_ELEMENTS_OFFSET;
    offset = indexes[সূচক];
    length = indexes[সূচক + 1] - offset;
    যদি(length > 0) {
        ds->swapArray32(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    সূচক = IX_CONTEXTS_OFFSET;
    offset = indexes[সূচক];
    length = indexes[সূচক + 1] - offset;
    যদি(length > 0) {
        ds->swapArray16(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    সূচক = IX_UNSAFE_BWD_OFFSET;
    offset = indexes[সূচক];
    length = indexes[সূচক + 1] - offset;
    যদি(length > 0) {
        ds->swapArray16(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    সূচক = IX_FAST_LATIN_TABLE_OFFSET;
    offset = indexes[সূচক];
    length = indexes[সূচক + 1] - offset;
    যদি(length > 0) {
        ds->swapArray16(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    সূচক = IX_SCRIPTS_OFFSET;
    offset = indexes[সূচক];
    length = indexes[সূচক + 1] - offset;
    যদি(length > 0) {
        ds->swapArray16(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    // Skip the  IX_COMPRESSIBLE_BYTES_OFFSET byte array.

    সূচক = IX_RESERVED18_OFFSET;
    offset = indexes[সূচক];
    length = indexes[সূচক + 1] - offset;
    যদি(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED18_OFFSET\n", length);
        errorCode = U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    ফেরত size;
}

}  // namespace

/* swap ICU collation data like ucadata.icu */
U_CAPI int32_t U_EXPORT2
ucol_swap(ধ্রুবক UDataSwapper *ds,
          ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
          UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) { ফেরত 0; }

    /* udata_swapDataHeader checks the arguments */
    int32_t header_আকার=udata_swap_তথ্য_Header(ds, in_তথ্য, length, out_তথ্য, pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        // Try to swap the old format version which did not have a standard data header.
        *pErrorCode=U_ZERO_ERROR;
        ফেরত swapFormatVersion3(ds, in_তথ্য, length, out_তথ্য, pErrorCode);
    }

    /* check data format and format version */
    ধ্রুবক UDataInfo &info=*(ধ্রুবক UDataInfo *)((ধ্রুবক অক্ষর *)in_তথ্য+4);
    যদি(!(
        info.তথ্য_Format[0]==0x55 &&   // dataFormat="UCol"
        info.তথ্য_Format[1]==0x43 &&
        info.তথ্য_Format[2]==0x6f &&
        info.তথ্য_Format[3]==0x6c &&
        (3<=info.formatVersion[0] && info.formatVersion[0]<=5)
    )) {
        udata_printError(ds, "ucol_swap(): data format %02x.%02x.%02x.%02x "
                         "(format version %02x.%02x) is not recognized as collation data\n",
                         info.তথ্য_Format[0], info.তথ্য_Format[1],
                         info.তথ্য_Format[2], info.তথ্য_Format[3],
                         info.formatVersion[0], info.formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    in_তথ্য=(ধ্রুবক অক্ষর *)in_তথ্য+header_আকার;
    যদি(length>=0) { length-=header_আকার; }
    out_তথ্য=(out_তথ্য == nullptr) ? nullptr : (অক্ষর *)out_তথ্য+header_আকার;
    int32_t collation_আকার;
    যদি(info.formatVersion[0]>=4) {
        collation_আকার=swapFormatVersion4(ds, in_তথ্য, length, out_তথ্য, *pErrorCode);
    } অন্যথা {
        collation_আকার=swapFormatVersion3(ds, in_তথ্য, length, out_তথ্য, pErrorCode);
    }
    যদি(U_SUCCESS(*pErrorCode)) {
        ফেরত header_আকার+collation_আকার;
    } অন্যথা {
        ফেরত 0;
    }
}

/* swap inverse UCA collation data (invuca.icu) */
U_CAPI int32_t U_EXPORT2
ucol_swapInverseUCA(ধ্রুবক UDataSwapper *ds,
                    ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
                    UErrorCode *pErrorCode) {
    ধ্রুবক UDataInfo *pInfo;
    int32_t header_আকার;

    ধ্রুবক uint8_t *inBytes;
    uint8_t *outBytes;

    ধ্রুবক InverseUCATableHeader *inHeader;
    InverseUCATableHeader *outHeader;
    InverseUCATableHeader header={ 0,0,0,0,0,{0,0,0,0},{0,0,0,0,0,0,0,0} };

    /* udata_swapDataHeader checks the arguments */
    header_আকার=udata_swap_তথ্য_Header(ds, in_তথ্য, length, out_তথ্য, pErrorCode);
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    /* check data format and format version */
    pInfo=(ধ্রুবক UDataInfo *)((ধ্রুবক অক্ষর *)in_তথ্য+4);
    যদি(!(
        pInfo->তথ্য_Format[0]==0x49 &&   /* dataFormat="InvC" */
        pInfo->তথ্য_Format[1]==0x6e &&
        pInfo->তথ্য_Format[2]==0x76 &&
        pInfo->তথ্য_Format[3]==0x43 &&
        pInfo->formatVersion[0]==2 &&
        pInfo->formatVersion[1]>=1
    )) {
        udata_printError(ds, "ucol_swapInverseUCA(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not an inverse UCA collation file\n",
                         pInfo->তথ্য_Format[0], pInfo->তথ্য_Format[1],
                         pInfo->তথ্য_Format[2], pInfo->তথ্য_Format[3],
                         pInfo->formatVersion[0], pInfo->formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    inBytes=(ধ্রুবক uint8_t *)in_তথ্য+header_আকার;
    outBytes=(uint8_t *)out_তথ্য+header_আকার;

    inHeader=(ধ্রুবক InverseUCATableHeader *)inBytes;
    outHeader=(InverseUCATableHeader *)outBytes;

    /*
     * The inverse UCA collation binary must contain at least the InverseUCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==8*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    যদি(length<0) {
        header.byte_আকার=udata_readInt32(ds, inHeader->byte_আকার);
    } অন্যথা যদি(
        ((length-header_আকার)<(8*4) ||
         (uint32_t)(length-header_আকার)<(header.byte_আকার=udata_readInt32(ds, inHeader->byte_আকার)))
    ) {
        udata_printError(ds, "ucol_swapInverseUCA(): too few bytes (%d after header) for inverse UCA collation data\n",
                         length);
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    যদি(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        যদি(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, header.byte_আকার);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the InverseUCATableHeader (the byteSize field was read above) */
        header.table_আকার=   ds->readUInt32(inHeader->table_আকার);
        header.conts_আকার=   ds->readUInt32(inHeader->conts_আকার);
        header.table=       ds->readUInt32(inHeader->table);
        header.conts=       ds->readUInt32(inHeader->conts);

        /* swap the 32-bit integers in the header */
        ds->swapArray32(ds, inHeader, 5*4, outHeader, pErrorCode);

        /* swap the inverse table; tableSize counts uint32_t[3] rows */
        ds->swapArray32(ds, inBytes+header.table, header.table_আকার*3*4,
                           outBytes+header.table, pErrorCode);

        /* swap the continuation table; contsSize counts UChars */
        ds->swapArray16(ds, inBytes+header.conts, header.conts_আকার*U_SIZEOF_UCHAR,
                           outBytes+header.conts, pErrorCode);
    }

    ফেরত header_আকার+header.byte_আকার;
}

#endif /* #if !UCONFIG_NO_COLLATION */
