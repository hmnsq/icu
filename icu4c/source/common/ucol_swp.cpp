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

#அந்தர்பாவ "unicode/udata.h" /* UDataInfo */
#அந்தர்பாவ "utrie.h"
#அந்தர்பாவ "utrie2.h"
#அந்தர்பாவ "udataswp.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "ucol_data.h"
#அந்தர்பாவ "ucol_swp.h"

/* swapping ----------------------------------------------------------------- */

#எனில் !UCONFIG_NO_COLLATION

U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_இருமம்(மாறிலி UDataSwapper *ds,
                              மாறிலி வெறுமை *inData, int32_t length) {
    எனில்(ds==nullptr || inData==nullptr || length<-1) {
        திரும்பு பொய்;
    }

    // First check for format version 4+ which has a standard data header.
    UErrorCode errorCode=U_ZERO_ERROR;
    (வெறுமை)udata_swapDataHeader(ds, inData, -1, nullptr, &errorCode);
    எனில்(U_SUCCESS(errorCode)) {
        மாறிலி UDataInfo &info=*(மாறிலி UDataInfo *)((மாறிலி எழுத்து *)inData+4);
        எனில்(info.dataFormat[0]==0x55 &&   // dataFormat="UCol"
                info.dataFormat[1]==0x43 &&
                info.dataFormat[2]==0x6f &&
                info.dataFormat[3]==0x6c) {
            திரும்பு உண்மை;
        }
    }

    // Else check for format version 3.
    மாறிலி UCATableHeader *inHeader=(மாறிலி UCATableHeader *)inData;

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    UCATableHeader header;
    uprv_memset(&header, 0, அளவு(header));
    எனில்(length<0) {
        header.size=udata_readInt32(ds, inHeader->size);
    } இல்லையெனில் எனில்((length<(42*4) || length<(header.size=udata_readInt32(ds, inHeader->size)))) {
        திரும்பு பொய்;
    }

    header.magic=ds->readUInt32(inHeader->magic);
    எனில்(!(
        header.magic==UCOL_HEADER_MAGIC &&
        inHeader->formatVersion[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        திரும்பு பொய்;
    }

    எனில்(inHeader->isBigEndian!=ds->inIsBigEndian || inHeader->charSetFamily!=ds->inCharset) {
        திரும்பு பொய்;
    }

    திரும்பு உண்மை;
}

நாமாகாஶ {

/* swap a header-less collation formatVersion=3 binary, inside a resource bundle or ucadata.icu */
int32_t
swapFormatVersion3(மாறிலி UDataSwapper *ds,
                   மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                   UErrorCode *pErrorCode) {
    மாறிலி uint8_t *inBytes;
    uint8_t *outBytes;

    மாறிலி UCATableHeader *inHeader;
    UCATableHeader *outHeader;
    UCATableHeader header;

    uint32_t count;

    /* argument checking in case we were not called from ucol_swap() */
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    inBytes = static_cast<மாறிலி uint8_t*>(inData);
    outBytes = static_cast<uint8_t*>(outData);

    inHeader = static_cast<மாறிலி UCATableHeader*>(inData);
    outHeader = static_cast<UCATableHeader*>(outData);

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    uprv_memset(&header, 0, அளவு(header));
    எனில்(length<0) {
        header.size=udata_readInt32(ds, inHeader->size);
    } இல்லையெனில் எனில்((length<(42*4) || length<(header.size=udata_readInt32(ds, inHeader->size)))) {
        udata_printError(ds, "ucol_swap(formatVersion=3): too few bytes (%d after header) for collation data\n",
                         length);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    header.magic=ds->readUInt32(inHeader->magic);
    எனில்(!(
        header.magic==UCOL_HEADER_MAGIC &&
        inHeader->formatVersion[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        udata_printError(ds, "ucol_swap(formatVersion=3): magic 0x%08x or format version %02x.%02x is not a collation binary\n",
                         header.magic,
                         inHeader->formatVersion[0], inHeader->formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        திரும்பு 0;
    }

    எனில்(inHeader->isBigEndian!=ds->inIsBigEndian || inHeader->charSetFamily!=ds->inCharset) {
        udata_printError(ds, "ucol_swap(formatVersion=3): endianness %d or charset %d does not match the swapper\n",
                         inHeader->isBigEndian, inHeader->charSetFamily);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        திரும்பு 0;
    }

    எனில்(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        எனில்(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, header.size);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the UCATableHeader (the size field was read above) */
        header.options=                 ds->readUInt32(inHeader->options);
        header.UCAConsts=               ds->readUInt32(inHeader->UCAConsts);
        header.contractionUCACombos=    ds->readUInt32(inHeader->contractionUCACombos);
        header.mappingPosition=         ds->readUInt32(inHeader->mappingPosition);
        header.expansion=               ds->readUInt32(inHeader->expansion);
        header.contractionIndex=        ds->readUInt32(inHeader->contractionIndex);
        header.contractionCEs=          ds->readUInt32(inHeader->contractionCEs);
        header.contractionSize=         ds->readUInt32(inHeader->contractionSize);
        header.endExpansionCE=          ds->readUInt32(inHeader->endExpansionCE);
        header.expansionCESize=         ds->readUInt32(inHeader->expansionCESize);
        header.endExpansionCECount=     udata_readInt32(ds, inHeader->endExpansionCECount);
        header.contractionUCACombosSize=udata_readInt32(ds, inHeader->contractionUCACombosSize);
        header.scriptToLeadByte=        ds->readUInt32(inHeader->scriptToLeadByte);
        header.leadByteToScript=        ds->readUInt32(inHeader->leadByteToScript);
        
        /* swap the 32-bit integers in the header */
        ds->swapArray32(ds, inHeader, static_cast<int32_t>(reinterpret_cast<மாறிலி எழுத்து*>(&inHeader->jamoSpecial) - reinterpret_cast<மாறிலி எழுத்து*>(inHeader)),
                           outHeader, pErrorCode);
        ds->swapArray32(ds, &(inHeader->scriptToLeadByte), அளவு(header.scriptToLeadByte) + அளவு(header.leadByteToScript),
                           &(outHeader->scriptToLeadByte), pErrorCode);
        /* set the output platform properties */
        outHeader->isBigEndian=ds->outIsBigEndian;
        outHeader->charSetFamily=ds->outCharset;

        /* swap the options */
        எனில்(header.options!=0) {
            ds->swapArray32(ds, inBytes+header.options, header.expansion-header.options,
                               outBytes+header.options, pErrorCode);
        }

        /* swap the expansions */
        எனில்(header.mappingPosition!=0 && header.expansion!=0) {
            எனில்(header.contractionIndex!=0) {
                /* expansions bounded by contractions */
                count=header.contractionIndex-header.expansion;
            } இல்லையெனில் {
                /* no contractions: expansions bounded by the main trie */
                count=header.mappingPosition-header.expansion;
            }
            ds->swapArray32(ds, inBytes + header.expansion, static_cast<int32_t>(count),
                               outBytes+header.expansion, pErrorCode);
        }

        /* swap the contractions */
        எனில்(header.contractionSize!=0) {
            /* contractionIndex: char16_t[] */
            ds->swapArray16(ds, inBytes+header.contractionIndex, header.contractionSize*2,
                               outBytes+header.contractionIndex, pErrorCode);

            /* contractionCEs: CEs[] */
            ds->swapArray32(ds, inBytes+header.contractionCEs, header.contractionSize*4,
                               outBytes+header.contractionCEs, pErrorCode);
        }

        /* swap the main trie */
        எனில்(header.mappingPosition!=0) {
            count=header.endExpansionCE-header.mappingPosition;
            utrie_swap(ds, inBytes + header.mappingPosition, static_cast<int32_t>(count),
                          outBytes+header.mappingPosition, pErrorCode);
        }

        /* swap the max expansion table */
        எனில்(header.endExpansionCECount!=0) {
            ds->swapArray32(ds, inBytes+header.endExpansionCE, header.endExpansionCECount*4,
                               outBytes+header.endExpansionCE, pErrorCode);
        }

        /* expansionCESize, unsafeCP, contrEndCP: uint8_t[], no need to swap */

        /* swap UCA constants */
        எனில்(header.UCAConsts!=0) {
            /*
             * if UCAConsts!=0 then contractionUCACombos because we are swapping
             * the UCA data file, and we know that the UCA contains contractions
             */
            ds->swapArray32(ds, inBytes+header.UCAConsts, header.contractionUCACombos-header.UCAConsts,
                               outBytes+header.UCAConsts, pErrorCode);
        }

        /* swap UCA contractions */
        எனில்(header.contractionUCACombosSize!=0) {
            count=header.contractionUCACombosSize*inHeader->contractionUCACombosWidth*U_SIZEOF_UCHAR;
            ds->swapArray16(ds, inBytes + header.contractionUCACombos, static_cast<int32_t>(count),
                               outBytes+header.contractionUCACombos, pErrorCode);
        }
        
        /* swap the script to lead bytes */
        எனில்(header.scriptToLeadByte!=0) {
            முழு index_மொத்தம் = ds->readUInt16(*((uint16_t*)(inBytes+header.scriptToLeadByte))); // each entry = 2 * uint16
            முழு data_மொத்தம் = ds->readUInt16(*((uint16_t*)(inBytes+header.scriptToLeadByte + 2))); // each entry = uint16
            ds->swapArray16(ds, inBytes+header.scriptToLeadByte, 
                                4 + (4 * index_மொத்தம்) + (2 * data_மொத்தம்),
                                outBytes+header.scriptToLeadByte, pErrorCode);
        }
        
        /* swap the lead byte to scripts */
        எனில்(header.leadByteToScript!=0) {
            முழு index_மொத்தம் = ds->readUInt16(*((uint16_t*)(inBytes+header.leadByteToScript))); // each entry = uint16
            முழு data_மொத்தம் = ds->readUInt16(*((uint16_t*)(inBytes+header.leadByteToScript + 2))); // each entry = uint16
            ds->swapArray16(ds, inBytes+header.leadByteToScript, 
                                4 + (2 * index_மொத்தம்) + (2 * data_மொத்தம்),
                                outBytes+header.leadByteToScript, pErrorCode);
        }
    }

    திரும்பு header.size;
}

// swap formatVersion 4 or 5 ----------------------------------------------- ***

// The following are copied from CollationDataReader, trading an awkward copy of constants
// for an awkward relocation of the i18n collationdatareader.h file into the common library.
// Keep them in sync!

எண்ணிக்கை {
    IX_INDEXES_LENGTH,  // 0
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

    IX_வேர்_ELEMENTS_OFFSET,  // 12
    IX_CONTEXTS_OFFSET,
    IX_UNSAFE_BWD_OFFSET,
    IX_FAST_LATIN_TABLE_OFFSET,

    IX_SCRIPTS_OFFSET,  // 16
    IX_COMPRESSIBLE_BYTES_OFFSET,
    IX_RESERVED18_OFFSET,
    IX_TOTAL_SIZE
};

int32_t
swapFormatVersion4(மாறிலி UDataSwapper *ds,
                   மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                   UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) { திரும்பு 0; }

    மாறிலி uint8_t* inBytes = static_cast<மாறிலி uint8_t*>(inData);
    uint8_t* outBytes = static_cast<uint8_t*>(outData);

    மாறிலி int32_t* inIndexes = reinterpret_cast<மாறிலி int32_t*>(inBytes);
    int32_t indexes[IX_TOTAL_SIZE+1];

    // Need at least IX_INDEXES_LENGTH and IX_OPTIONS.
    எனில்(0<=length && length<8) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    int32_t indexesLength=indexes[0]=udata_readInt32(ds, inIndexes[0]);
    எனில்(0<=length && length<(indexesLength*4)) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    க்கு(int32_t i=1; i<=IX_TOTAL_SIZE && i<indexesLength; ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }
    க்கு(int32_t i=indexesLength; i<=IX_TOTAL_SIZE; ++i) {
        indexes[i]=-1;
    }
    inIndexes=nullptr;  // Make sure we do not accidentally use these instead of indexes[].

    // Get the total length of the data.
    int32_t size;
    எனில்(indexesLength>IX_TOTAL_SIZE) {
        size=indexes[IX_TOTAL_SIZE];
    } இல்லையெனில் எனில்(indexesLength>IX_REORDER_CODES_OFFSET) {
        size=indexes[indexesLength-1];
    } இல்லையெனில் {
        size=indexesLength*4;
    }
    எனில்(length<0) { திரும்பு size; }

    எனில்(length<size) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    // Copy the data for inaccessible bytes and arrays of bytes.
    எனில்(inBytes!=outBytes) {
        uprv_memcpy(outBytes, inBytes, size);
    }

    // Swap the int32_t indexes[].
    ds->swapArray32(ds, inBytes, indexesLength * 4, outBytes, &errorCode);

    // The following is a modified version of CollationDataReader::read().
    // Here we use indexes[] not inIndexes[] because
    // the inIndexes[] may not be in this machine's endianness.
    int32_t index;  // one of the indexes[] slots
    int32_t offset;  // byte offset for the index part
    // int32_t length;  // number of bytes in the index part

    index = IX_REORDER_CODES_OFFSET;
    offset = indexes[index];
    length = indexes[index + 1] - offset;
    எனில்(length > 0) {
        ds->swapArray32(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    // Skip the IX_REORDER_TABLE_OFFSET byte array.

    index = IX_TRIE_OFFSET;
    offset = indexes[index];
    length = indexes[index + 1] - offset;
    எனில்(length > 0) {
        utrie2_swap(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    index = IX_RESERVED8_OFFSET;
    offset = indexes[index];
    length = indexes[index + 1] - offset;
    எனில்(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED8_OFFSET\n", length);
        errorCode = U_UNSUPPORTED_ERROR;
        திரும்பு 0;
    }

    index = IX_CES_OFFSET;
    offset = indexes[index];
    length = indexes[index + 1] - offset;
    எனில்(length > 0) {
        ds->swapArray64(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    index = IX_RESERVED10_OFFSET;
    offset = indexes[index];
    length = indexes[index + 1] - offset;
    எனில்(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED10_OFFSET\n", length);
        errorCode = U_UNSUPPORTED_ERROR;
        திரும்பு 0;
    }

    index = IX_CE32S_OFFSET;
    offset = indexes[index];
    length = indexes[index + 1] - offset;
    எனில்(length > 0) {
        ds->swapArray32(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    index = IX_வேர்_ELEMENTS_OFFSET;
    offset = indexes[index];
    length = indexes[index + 1] - offset;
    எனில்(length > 0) {
        ds->swapArray32(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    index = IX_CONTEXTS_OFFSET;
    offset = indexes[index];
    length = indexes[index + 1] - offset;
    எனில்(length > 0) {
        ds->swapArray16(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    index = IX_UNSAFE_BWD_OFFSET;
    offset = indexes[index];
    length = indexes[index + 1] - offset;
    எனில்(length > 0) {
        ds->swapArray16(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    index = IX_FAST_LATIN_TABLE_OFFSET;
    offset = indexes[index];
    length = indexes[index + 1] - offset;
    எனில்(length > 0) {
        ds->swapArray16(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    index = IX_SCRIPTS_OFFSET;
    offset = indexes[index];
    length = indexes[index + 1] - offset;
    எனில்(length > 0) {
        ds->swapArray16(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    // Skip the  IX_COMPRESSIBLE_BYTES_OFFSET byte array.

    index = IX_RESERVED18_OFFSET;
    offset = indexes[index];
    length = indexes[index + 1] - offset;
    எனில்(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED18_OFFSET\n", length);
        errorCode = U_UNSUPPORTED_ERROR;
        திரும்பு 0;
    }

    திரும்பு size;
}

}  // namespace

/* swap ICU collation data like ucadata.icu */
U_CAPI int32_t U_EXPORT2
ucol_swap(மாறிலி UDataSwapper *ds,
          மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
          UErrorCode *pErrorCode) {
    எனில்(U_FAILURE(*pErrorCode)) { திரும்பு 0; }

    /* udata_swapDataHeader checks the arguments */
    int32_t headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    எனில்(U_FAILURE(*pErrorCode)) {
        // Try to swap the old format version which did not have a standard data header.
        *pErrorCode=U_ZERO_ERROR;
        திரும்பு swapFormatVersion3(ds, inData, length, outData, pErrorCode);
    }

    /* check data format and format version */
    மாறிலி UDataInfo &info=*(மாறிலி UDataInfo *)((மாறிலி எழுத்து *)inData+4);
    எனில்(!(
        info.dataFormat[0]==0x55 &&   // dataFormat="UCol"
        info.dataFormat[1]==0x43 &&
        info.dataFormat[2]==0x6f &&
        info.dataFormat[3]==0x6c &&
        (3<=info.formatVersion[0] && info.formatVersion[0]<=5)
    )) {
        udata_printError(ds, "ucol_swap(): data format %02x.%02x.%02x.%02x "
                         "(format version %02x.%02x) is not recognized as collation data\n",
                         info.dataFormat[0], info.dataFormat[1],
                         info.dataFormat[2], info.dataFormat[3],
                         info.formatVersion[0], info.formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        திரும்பு 0;
    }

    inData=(மாறிலி எழுத்து *)inData+headerSize;
    எனில்(length>=0) { length-=headerSize; }
    outData=(outData == nullptr) ? nullptr : (எழுத்து *)outData+headerSize;
    int32_t collationSize;
    எனில்(info.formatVersion[0]>=4) {
        collationSize=swapFormatVersion4(ds, inData, length, outData, *pErrorCode);
    } இல்லையெனில் {
        collationSize=swapFormatVersion3(ds, inData, length, outData, pErrorCode);
    }
    எனில்(U_SUCCESS(*pErrorCode)) {
        திரும்பு headerSize+collationSize;
    } இல்லையெனில் {
        திரும்பு 0;
    }
}

/* swap inverse UCA collation data (invuca.icu) */
U_CAPI int32_t U_EXPORT2
ucol_swapInverseUCA(மாறிலி UDataSwapper *ds,
                    மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                    UErrorCode *pErrorCode) {
    மாறிலி UDataInfo *pInfo;
    int32_t headerSize;

    மாறிலி uint8_t *inBytes;
    uint8_t *outBytes;

    மாறிலி InverseUCATableHeader *inHeader;
    InverseUCATableHeader *outHeader;
    InverseUCATableHeader header={ 0,0,0,0,0,{0,0,0,0},{0,0,0,0,0,0,0,0} };

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }

    /* check data format and format version */
    pInfo=(மாறிலி UDataInfo *)((மாறிலி எழுத்து *)inData+4);
    எனில்(!(
        pInfo->dataFormat[0]==0x49 &&   /* dataFormat="InvC" */
        pInfo->dataFormat[1]==0x6e &&
        pInfo->dataFormat[2]==0x76 &&
        pInfo->dataFormat[3]==0x43 &&
        pInfo->formatVersion[0]==2 &&
        pInfo->formatVersion[1]>=1
    )) {
        udata_printError(ds, "ucol_swapInverseUCA(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not an inverse UCA collation file\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0], pInfo->formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        திரும்பு 0;
    }

    inBytes=(மாறிலி uint8_t *)inData+headerSize;
    outBytes=(uint8_t *)outData+headerSize;

    inHeader=(மாறிலி InverseUCATableHeader *)inBytes;
    outHeader=(InverseUCATableHeader *)outBytes;

    /*
     * The inverse UCA collation binary must contain at least the InverseUCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==8*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    எனில்(length<0) {
        header.byteSize=udata_readInt32(ds, inHeader->byteSize);
    } இல்லையெனில் எனில்(
        ((length-headerSize)<(8*4) ||
         (uint32_t)(length-headerSize)<(header.byteSize=udata_readInt32(ds, inHeader->byteSize)))
    ) {
        udata_printError(ds, "ucol_swapInverseUCA(): too few bytes (%d after header) for inverse UCA collation data\n",
                         length);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    எனில்(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        எனில்(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, header.byteSize);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the InverseUCATableHeader (the byteSize field was read above) */
        header.tableSize=   ds->readUInt32(inHeader->tableSize);
        header.contsSize=   ds->readUInt32(inHeader->contsSize);
        header.table=       ds->readUInt32(inHeader->table);
        header.conts=       ds->readUInt32(inHeader->conts);

        /* swap the 32-bit integers in the header */
        ds->swapArray32(ds, inHeader, 5*4, outHeader, pErrorCode);

        /* swap the inverse table; tableSize counts uint32_t[3] rows */
        ds->swapArray32(ds, inBytes+header.table, header.tableSize*3*4,
                           outBytes+header.table, pErrorCode);

        /* swap the continuation table; contsSize counts UChars */
        ds->swapArray16(ds, inBytes+header.conts, header.contsSize*U_SIZEOF_UCHAR,
                           outBytes+header.conts, pErrorCode);
    }

    திரும்பு headerSize+header.byteSize;
}

#endif /* #if !UCONFIG_NO_COLLATION */
