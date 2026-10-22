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

#समावेश "unicode/udata.h" /* UDataInfo */
#समावेश "utrie.h"
#समावेश "utrie2.h"
#समावेश "udataswp.h"
#समावेश "cmemory.h"
#समावेश "ucol_data.h"
#समावेश "ucol_swp.h"

/* swapping ----------------------------------------------------------------- */

#अगर !UCONFIG_NO_COLLATION

U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_द्विआधारी(स्थिर UDataSwapper *ds,
                              स्थिर व्योम *in_तथ्य, int32_t length) {
    अगर(ds==nullptr || in_तथ्य==nullptr || length<-1) {
        वापस false;
    }

    // First check for format version 4+ which has a standard data header.
    UErrorCode errorCode=U_ZERO_ERROR;
    (व्योम)udata_swap_तथ्य_Header(ds, in_तथ्य, -1, nullptr, &errorCode);
    अगर(U_SUCCESS(errorCode)) {
        स्थिर UDataInfo &info=*(स्थिर UDataInfo *)((स्थिर अक्षर *)in_तथ्य+4);
        अगर(info.तथ्य_Format[0]==0x55 &&   // dataFormat="UCol"
                info.तथ्य_Format[1]==0x43 &&
                info.तथ्य_Format[2]==0x6f &&
                info.तथ्य_Format[3]==0x6c) {
            वापस true;
        }
    }

    // Else check for format version 3.
    स्थिर UCATableHeader *inHeader=(स्थिर UCATableHeader *)in_तथ्य;

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    UCATableHeader header;
    uprv_memset(&header, 0, माप(header));
    अगर(length<0) {
        header.size=udata_readInt32(ds, inHeader->size);
    } अन्यथा अगर((length<(42*4) || length<(header.size=udata_readInt32(ds, inHeader->size)))) {
        वापस false;
    }

    header.magic=ds->readUInt32(inHeader->magic);
    अगर(!(
        header.magic==UCOL_HEADER_MAGIC &&
        inHeader->formatVersion[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        वापस false;
    }

    अगर(inHeader->isBigEndian!=ds->inIsBigEndian || inHeader->charSetFamily!=ds->inCharset) {
        वापस false;
    }

    वापस true;
}

namespace {

/* swap a header-less collation formatVersion=3 binary, inside a resource bundle or ucadata.icu */
int32_t
swapFormatVersion3(स्थिर UDataSwapper *ds,
                   स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                   UErrorCode *pErrorCode) {
    स्थिर uint8_t *inBytes;
    uint8_t *outBytes;

    स्थिर UCATableHeader *inHeader;
    UCATableHeader *outHeader;
    UCATableHeader header;

    uint32_t count;

    /* argument checking in case we were not called from ucol_swap() */
    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(ds==nullptr || in_तथ्य==nullptr || length<-1 || (length>0 && out_तथ्य==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    inBytes = static_cast<स्थिर uint8_t*>(in_तथ्य);
    outBytes = static_cast<uint8_t*>(out_तथ्य);

    inHeader = static_cast<स्थिर UCATableHeader*>(in_तथ्य);
    outHeader = static_cast<UCATableHeader*>(out_तथ्य);

    /*
     * The collation binary must contain at least the UCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==42*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    uprv_memset(&header, 0, माप(header));
    अगर(length<0) {
        header.size=udata_readInt32(ds, inHeader->size);
    } अन्यथा अगर((length<(42*4) || length<(header.size=udata_readInt32(ds, inHeader->size)))) {
        udata_printError(ds, "ucol_swap(formatVersion=3): too few bytes (%d after header) for collation data\n",
                         length);
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    header.magic=ds->readUInt32(inHeader->magic);
    अगर(!(
        header.magic==UCOL_HEADER_MAGIC &&
        inHeader->formatVersion[0]==3 /*&&
        inHeader->formatVersion[1]>=0*/
    )) {
        udata_printError(ds, "ucol_swap(formatVersion=3): magic 0x%08x or format version %02x.%02x is not a collation binary\n",
                         header.magic,
                         inHeader->formatVersion[0], inHeader->formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    अगर(inHeader->isBigEndian!=ds->inIsBigEndian || inHeader->charSetFamily!=ds->inCharset) {
        udata_printError(ds, "ucol_swap(formatVersion=3): endianness %d or charset %d does not match the swapper\n",
                         inHeader->isBigEndian, inHeader->charSetFamily);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस 0;
    }

    अगर(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        अगर(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, header.size);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the UCATableHeader (the size field was read above) */
        header.options=                 ds->readUInt32(inHeader->options);
        header.UCAConsts=               ds->readUInt32(inHeader->UCAConsts);
        header.contractionUCACombos=    ds->readUInt32(inHeader->contractionUCACombos);
        header.mappingPosition=         ds->readUInt32(inHeader->mappingPosition);
        header.expansion=               ds->readUInt32(inHeader->expansion);
        header.contraction_सूचकांक=        ds->readUInt32(inHeader->contraction_सूचकांक);
        header.contractionCEs=          ds->readUInt32(inHeader->contractionCEs);
        header.contraction_आकार=         ds->readUInt32(inHeader->contraction_आकार);
        header.endExpansionCE=          ds->readUInt32(inHeader->endExpansionCE);
        header.expansionCESize=         ds->readUInt32(inHeader->expansionCESize);
        header.endExpansionCECount=     udata_readInt32(ds, inHeader->endExpansionCECount);
        header.contraction_UCACombos_आकार=udata_readInt32(ds, inHeader->contraction_UCACombos_आकार);
        header.scriptToLeadByte=        ds->readUInt32(inHeader->scriptToLeadByte);
        header.leadByteToScript=        ds->readUInt32(inHeader->leadByteToScript);
        
        /* swap the 32-bit integers in the header */
        ds->swapArray32(ds, inHeader, static_cast<int32_t>(reinterpret_cast<स्थिर अक्षर*>(&inHeader->jamoSpecial) - reinterpret_cast<स्थिर अक्षर*>(inHeader)),
                           outHeader, pErrorCode);
        ds->swapArray32(ds, &(inHeader->scriptToLeadByte), माप(header.scriptToLeadByte) + माप(header.leadByteToScript),
                           &(outHeader->scriptToLeadByte), pErrorCode);
        /* set the output platform properties */
        outHeader->isBigEndian=ds->outIsBigEndian;
        outHeader->charSetFamily=ds->outCharset;

        /* swap the options */
        अगर(header.options!=0) {
            ds->swapArray32(ds, inBytes+header.options, header.expansion-header.options,
                               outBytes+header.options, pErrorCode);
        }

        /* swap the expansions */
        अगर(header.mappingPosition!=0 && header.expansion!=0) {
            अगर(header.contraction_सूचकांक!=0) {
                /* expansions bounded by contractions */
                count=header.contraction_सूचकांक-header.expansion;
            } अन्यथा {
                /* no contractions: expansions bounded by the main trie */
                count=header.mappingPosition-header.expansion;
            }
            ds->swapArray32(ds, inBytes + header.expansion, static_cast<int32_t>(count),
                               outBytes+header.expansion, pErrorCode);
        }

        /* swap the contractions */
        अगर(header.contraction_आकार!=0) {
            /* contractionIndex: char16_t[] */
            ds->swapArray16(ds, inBytes+header.contraction_सूचकांक, header.contraction_आकार*2,
                               outBytes+header.contraction_सूचकांक, pErrorCode);

            /* contractionCEs: CEs[] */
            ds->swapArray32(ds, inBytes+header.contractionCEs, header.contraction_आकार*4,
                               outBytes+header.contractionCEs, pErrorCode);
        }

        /* swap the main trie */
        अगर(header.mappingPosition!=0) {
            count=header.endExpansionCE-header.mappingPosition;
            utrie_swap(ds, inBytes + header.mappingPosition, static_cast<int32_t>(count),
                          outBytes+header.mappingPosition, pErrorCode);
        }

        /* swap the max expansion table */
        अगर(header.endExpansionCECount!=0) {
            ds->swapArray32(ds, inBytes+header.endExpansionCE, header.endExpansionCECount*4,
                               outBytes+header.endExpansionCE, pErrorCode);
        }

        /* expansionCESize, unsafeCP, contrEndCP: uint8_t[], no need to swap */

        /* swap UCA constants */
        अगर(header.UCAConsts!=0) {
            /*
             * if UCAConsts!=0 then contractionUCACombos because we are swapping
             * the UCA data file, and we know that the UCA contains contractions
             */
            ds->swapArray32(ds, inBytes+header.UCAConsts, header.contractionUCACombos-header.UCAConsts,
                               outBytes+header.UCAConsts, pErrorCode);
        }

        /* swap UCA contractions */
        अगर(header.contraction_UCACombos_आकार!=0) {
            count=header.contraction_UCACombos_आकार*inHeader->contractionUCACombosWidth*U_SIZEOF_UCHAR;
            ds->swapArray16(ds, inBytes + header.contractionUCACombos, static_cast<int32_t>(count),
                               outBytes+header.contractionUCACombos, pErrorCode);
        }
        
        /* swap the script to lead bytes */
        अगर(header.scriptToLeadByte!=0) {
            पूर्णांक सूचकांक_गिनती = ds->readUInt16(*((uint16_t*)(inBytes+header.scriptToLeadByte))); // each entry = 2 * uint16
            पूर्णांक तथ्य_गिनती = ds->readUInt16(*((uint16_t*)(inBytes+header.scriptToLeadByte + 2))); // each entry = uint16
            ds->swapArray16(ds, inBytes+header.scriptToLeadByte, 
                                4 + (4 * सूचकांक_गिनती) + (2 * तथ्य_गिनती),
                                outBytes+header.scriptToLeadByte, pErrorCode);
        }
        
        /* swap the lead byte to scripts */
        अगर(header.leadByteToScript!=0) {
            पूर्णांक सूचकांक_गिनती = ds->readUInt16(*((uint16_t*)(inBytes+header.leadByteToScript))); // each entry = uint16
            पूर्णांक तथ्य_गिनती = ds->readUInt16(*((uint16_t*)(inBytes+header.leadByteToScript + 2))); // each entry = uint16
            ds->swapArray16(ds, inBytes+header.leadByteToScript, 
                                4 + (2 * सूचकांक_गिनती) + (2 * तथ्य_गिनती),
                                outBytes+header.leadByteToScript, pErrorCode);
        }
    }

    वापस header.size;
}

// swap formatVersion 4 or 5 ----------------------------------------------- ***

// The following are copied from CollationDataReader, trading an awkward copy of constants
// for an awkward relocation of the i18n collationdatareader.h file into the common library.
// Keep them in sync!

क्रमागत {
    IX_INDEXES_लंबाई,  // 0
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

    IX_मूल_ELEMENTS_OFFSET,  // 12
    IX_CONTEXTS_OFFSET,
    IX_UNSAFE_BWD_OFFSET,
    IX_FAST_LATIN_TABLE_OFFSET,

    IX_SCRIPTS_OFFSET,  // 16
    IX_COMPRESSIBLE_BYTES_OFFSET,
    IX_RESERVED18_OFFSET,
    IX_योग_आकार
};

int32_t
swapFormatVersion4(स्थिर UDataSwapper *ds,
                   स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                   UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) { वापस 0; }

    स्थिर uint8_t* inBytes = static_cast<स्थिर uint8_t*>(in_तथ्य);
    uint8_t* outBytes = static_cast<uint8_t*>(out_तथ्य);

    स्थिर int32_t* inIndexes = reinterpret_cast<स्थिर int32_t*>(inBytes);
    int32_t indexes[IX_योग_आकार+1];

    // Need at least IX_INDEXES_LENGTH and IX_OPTIONS.
    अगर(0<=length && length<8) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    int32_t indexes_लंबाई=indexes[0]=udata_readInt32(ds, inIndexes[0]);
    अगर(0<=length && length<(indexes_लंबाई*4)) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    क्रम(int32_t i=1; i<=IX_योग_आकार && i<indexes_लंबाई; ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }
    क्रम(int32_t i=indexes_लंबाई; i<=IX_योग_आकार; ++i) {
        indexes[i]=-1;
    }
    inIndexes=nullptr;  // Make sure we do not accidentally use these instead of indexes[].

    // Get the total length of the data.
    int32_t size;
    अगर(indexes_लंबाई>IX_योग_आकार) {
        size=indexes[IX_योग_आकार];
    } अन्यथा अगर(indexes_लंबाई>IX_REORDER_CODES_OFFSET) {
        size=indexes[indexes_लंबाई-1];
    } अन्यथा {
        size=indexes_लंबाई*4;
    }
    अगर(length<0) { वापस size; }

    अगर(length<size) {
        udata_printError(ds, "ucol_swap(formatVersion=4): too few bytes "
                         "(%d after header) for collation data\n",
                         length);
        errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    // Copy the data for inaccessible bytes and arrays of bytes.
    अगर(inBytes!=outBytes) {
        uprv_memcpy(outBytes, inBytes, size);
    }

    // Swap the int32_t indexes[].
    ds->swapArray32(ds, inBytes, indexes_लंबाई * 4, outBytes, &errorCode);

    // The following is a modified version of CollationDataReader::read().
    // Here we use indexes[] not inIndexes[] because
    // the inIndexes[] may not be in this machine's endianness.
    int32_t सूचकांक;  // one of the indexes[] slots
    int32_t offset;  // byte offset for the index part
    // int32_t length;  // number of bytes in the index part

    सूचकांक = IX_REORDER_CODES_OFFSET;
    offset = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - offset;
    अगर(length > 0) {
        ds->swapArray32(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    // Skip the IX_REORDER_TABLE_OFFSET byte array.

    सूचकांक = IX_TRIE_OFFSET;
    offset = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - offset;
    अगर(length > 0) {
        utrie2_swap(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    सूचकांक = IX_RESERVED8_OFFSET;
    offset = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - offset;
    अगर(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED8_OFFSET\n", length);
        errorCode = U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    सूचकांक = IX_CES_OFFSET;
    offset = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - offset;
    अगर(length > 0) {
        ds->swapArray64(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    सूचकांक = IX_RESERVED10_OFFSET;
    offset = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - offset;
    अगर(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED10_OFFSET\n", length);
        errorCode = U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    सूचकांक = IX_CE32S_OFFSET;
    offset = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - offset;
    अगर(length > 0) {
        ds->swapArray32(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    सूचकांक = IX_मूल_ELEMENTS_OFFSET;
    offset = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - offset;
    अगर(length > 0) {
        ds->swapArray32(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    सूचकांक = IX_CONTEXTS_OFFSET;
    offset = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - offset;
    अगर(length > 0) {
        ds->swapArray16(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    सूचकांक = IX_UNSAFE_BWD_OFFSET;
    offset = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - offset;
    अगर(length > 0) {
        ds->swapArray16(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    सूचकांक = IX_FAST_LATIN_TABLE_OFFSET;
    offset = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - offset;
    अगर(length > 0) {
        ds->swapArray16(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    सूचकांक = IX_SCRIPTS_OFFSET;
    offset = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - offset;
    अगर(length > 0) {
        ds->swapArray16(ds, inBytes + offset, length, outBytes + offset, &errorCode);
    }

    // Skip the  IX_COMPRESSIBLE_BYTES_OFFSET byte array.

    सूचकांक = IX_RESERVED18_OFFSET;
    offset = indexes[सूचकांक];
    length = indexes[सूचकांक + 1] - offset;
    अगर(length > 0) {
        udata_printError(ds, "ucol_swap(formatVersion=4): unknown data at IX_RESERVED18_OFFSET\n", length);
        errorCode = U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    वापस size;
}

}  // namespace

/* swap ICU collation data like ucadata.icu */
U_CAPI int32_t U_EXPORT2
ucol_swap(स्थिर UDataSwapper *ds,
          स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
          UErrorCode *pErrorCode) {
    अगर(U_FAILURE(*pErrorCode)) { वापस 0; }

    /* udata_swapDataHeader checks the arguments */
    int32_t header_आकार=udata_swap_तथ्य_Header(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        // Try to swap the old format version which did not have a standard data header.
        *pErrorCode=U_ZERO_ERROR;
        वापस swapFormatVersion3(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    }

    /* check data format and format version */
    स्थिर UDataInfo &info=*(स्थिर UDataInfo *)((स्थिर अक्षर *)in_तथ्य+4);
    अगर(!(
        info.तथ्य_Format[0]==0x55 &&   // dataFormat="UCol"
        info.तथ्य_Format[1]==0x43 &&
        info.तथ्य_Format[2]==0x6f &&
        info.तथ्य_Format[3]==0x6c &&
        (3<=info.formatVersion[0] && info.formatVersion[0]<=5)
    )) {
        udata_printError(ds, "ucol_swap(): data format %02x.%02x.%02x.%02x "
                         "(format version %02x.%02x) is not recognized as collation data\n",
                         info.तथ्य_Format[0], info.तथ्य_Format[1],
                         info.तथ्य_Format[2], info.तथ्य_Format[3],
                         info.formatVersion[0], info.formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    in_तथ्य=(स्थिर अक्षर *)in_तथ्य+header_आकार;
    अगर(length>=0) { length-=header_आकार; }
    out_तथ्य=(out_तथ्य == nullptr) ? nullptr : (अक्षर *)out_तथ्य+header_आकार;
    int32_t collation_आकार;
    अगर(info.formatVersion[0]>=4) {
        collation_आकार=swapFormatVersion4(ds, in_तथ्य, length, out_तथ्य, *pErrorCode);
    } अन्यथा {
        collation_आकार=swapFormatVersion3(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    }
    अगर(U_SUCCESS(*pErrorCode)) {
        वापस header_आकार+collation_आकार;
    } अन्यथा {
        वापस 0;
    }
}

/* swap inverse UCA collation data (invuca.icu) */
U_CAPI int32_t U_EXPORT2
ucol_swapInverseUCA(स्थिर UDataSwapper *ds,
                    स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                    UErrorCode *pErrorCode) {
    स्थिर UDataInfo *pInfo;
    int32_t header_आकार;

    स्थिर uint8_t *inBytes;
    uint8_t *outBytes;

    स्थिर InverseUCATableHeader *inHeader;
    InverseUCATableHeader *outHeader;
    InverseUCATableHeader header={ 0,0,0,0,0,{0,0,0,0},{0,0,0,0,0,0,0,0} };

    /* udata_swapDataHeader checks the arguments */
    header_आकार=udata_swap_तथ्य_Header(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    /* check data format and format version */
    pInfo=(स्थिर UDataInfo *)((स्थिर अक्षर *)in_तथ्य+4);
    अगर(!(
        pInfo->तथ्य_Format[0]==0x49 &&   /* dataFormat="InvC" */
        pInfo->तथ्य_Format[1]==0x6e &&
        pInfo->तथ्य_Format[2]==0x76 &&
        pInfo->तथ्य_Format[3]==0x43 &&
        pInfo->formatVersion[0]==2 &&
        pInfo->formatVersion[1]>=1
    )) {
        udata_printError(ds, "ucol_swapInverseUCA(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not an inverse UCA collation file\n",
                         pInfo->तथ्य_Format[0], pInfo->तथ्य_Format[1],
                         pInfo->तथ्य_Format[2], pInfo->तथ्य_Format[3],
                         pInfo->formatVersion[0], pInfo->formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    inBytes=(स्थिर uint8_t *)in_तथ्य+header_आकार;
    outBytes=(uint8_t *)out_तथ्य+header_आकार;

    inHeader=(स्थिर InverseUCATableHeader *)inBytes;
    outHeader=(InverseUCATableHeader *)outBytes;

    /*
     * The inverse UCA collation binary must contain at least the InverseUCATableHeader,
     * starting with its size field.
     * sizeof(UCATableHeader)==8*4 in ICU 2.8
     * check the length against the header size before reading the size field
     */
    अगर(length<0) {
        header.byte_आकार=udata_readInt32(ds, inHeader->byte_आकार);
    } अन्यथा अगर(
        ((length-header_आकार)<(8*4) ||
         (uint32_t)(length-header_आकार)<(header.byte_आकार=udata_readInt32(ds, inHeader->byte_आकार)))
    ) {
        udata_printError(ds, "ucol_swapInverseUCA(): too few bytes (%d after header) for inverse UCA collation data\n",
                         length);
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    अगर(length>=0) {
        /* copy everything, takes care of data that needs no swapping */
        अगर(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, header.byte_आकार);
        }

        /* swap the necessary pieces in the order of their occurrence in the data */

        /* read more of the InverseUCATableHeader (the byteSize field was read above) */
        header.table_आकार=   ds->readUInt32(inHeader->table_आकार);
        header.conts_आकार=   ds->readUInt32(inHeader->conts_आकार);
        header.table=       ds->readUInt32(inHeader->table);
        header.conts=       ds->readUInt32(inHeader->conts);

        /* swap the 32-bit integers in the header */
        ds->swapArray32(ds, inHeader, 5*4, outHeader, pErrorCode);

        /* swap the inverse table; tableSize counts uint32_t[3] rows */
        ds->swapArray32(ds, inBytes+header.table, header.table_आकार*3*4,
                           outBytes+header.table, pErrorCode);

        /* swap the continuation table; contsSize counts UChars */
        ds->swapArray16(ds, inBytes+header.conts, header.conts_आकार*U_SIZEOF_UCHAR,
                           outBytes+header.conts, pErrorCode);
    }

    वापस header_आकार+header.byte_आकार;
}

#पूर्ण_अगर /* #if !UCONFIG_NO_COLLATION */
