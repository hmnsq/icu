// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  udataswp.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003jun05
*   created by: Markus W. Scherer
*
*   Definitions for ICU data transformations for different platforms,
*   changing between big- and little-endian data and/or between
*   charset families (ASCII<->EBCDIC).
*/

#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h" /* UDataInfo */
#ಅನ್ತರ್ಭಾವ "ucmndata.h" /* DataHeader */
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

ಸ್ಥಿರ int32_t U_CALLCONV
uprv_swapArray16(ಧ್ರುವ UDataSwapper *ds,
                 ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
                 UErrorCode *pErrorCode) {
    ಧ್ರುವ uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* setup and swapping */
    p = static_cast<ಧ್ರುವ uint16_t*>(inData);
    q = static_cast<uint16_t*>(outData);
    count=length/2;
    ಯಾವತ್(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    ಪ್ರತ್ಯಾಗಮ length;
}

ಸ್ಥಿರ int32_t U_CALLCONV
uprv_copyArray16(ಧ್ರುವ UDataSwapper *ds,
                 ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
                 UErrorCode *pErrorCode) {
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    ಪ್ರತ್ಯಾಗಮ length;
}

ಸ್ಥಿರ int32_t U_CALLCONV
uprv_swapArray32(ಧ್ರುವ UDataSwapper *ds,
                 ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
                 UErrorCode *pErrorCode) {
    ಧ್ರುವ uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* setup and swapping */
    p = static_cast<ಧ್ರುವ uint32_t*>(inData);
    q = static_cast<uint32_t*>(outData);
    count=length/4;
    ಯಾವತ್(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    ಪ್ರತ್ಯಾಗಮ length;
}

ಸ್ಥಿರ int32_t U_CALLCONV
uprv_copyArray32(ಧ್ರುವ UDataSwapper *ds,
                 ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
                 UErrorCode *pErrorCode) {
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    ಪ್ರತ್ಯಾಗಮ length;
}

ಸ್ಥಿರ int32_t U_CALLCONV
uprv_swapArray64(ಧ್ರುವ UDataSwapper *ds,
                 ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
                 UErrorCode *pErrorCode) {
    ಧ್ರುವ uint64_t *p;
    uint64_t *q;
    int32_t count;

    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* setup and swapping */
    p = static_cast<ಧ್ರುವ uint64_t*>(inData);
    q = static_cast<uint64_t*>(outData);
    count=length/8;
    ಯಾವತ್(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    ಪ್ರತ್ಯಾಗಮ length;
}

ಸ್ಥಿರ int32_t U_CALLCONV
uprv_copyArray64(ಧ್ರುವ UDataSwapper *ds,
                 ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
                 UErrorCode *pErrorCode) {
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    ಪ್ರತ್ಯಾಗಮ length;
}

ಸ್ಥಿರ uint16_t U_CALLCONV
uprv_readSwapUInt16(uint16_t x) {
    ಪ್ರತ್ಯಾಗಮ static_cast<uint16_t>((x << 8) | (x >> 8));
}

ಸ್ಥಿರ uint16_t U_CALLCONV
uprv_readDirectUInt16(uint16_t x) {
    ಪ್ರತ್ಯಾಗಮ x;
}

ಸ್ಥಿರ uint32_t U_CALLCONV
uprv_readSwapUInt32(uint32_t x) {
    ಪ್ರತ್ಯಾಗಮ (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ಸ್ಥಿರ uint32_t U_CALLCONV
uprv_readDirectUInt32(uint32_t x) {
    ಪ್ರತ್ಯಾಗಮ x;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
uprv_writeSwapUInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
uprv_writeDirectUInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
uprv_writeSwapUInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
uprv_writeDirectUInt32(uint32_t *p, uint32_t x) {
    *p=x;
}

U_CAPI int16_t U_EXPORT2
udata_readInt16(ಧ್ರುವ UDataSwapper *ds, int16_t x) {
    ಪ್ರತ್ಯಾಗಮ (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(ಧ್ರುವ UDataSwapper *ds, int32_t x) {
    ಪ್ರತ್ಯಾಗಮ (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_swapInvStringBlock(ಧ್ರುವ UDataSwapper *ds,
                         ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
                         UErrorCode *pErrorCode) {
    ಧ್ರುವ ವರ್ಣ *inChars;
    int32_t stringsLength;

    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(ds==nullptr || inData==nullptr || length<0 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    inChars=(ಧ್ರುವ ವರ್ಣ *)inData;
    stringsLength=length;
    ಯಾವತ್(stringsLength>0 && inChars[stringsLength-1]!=0) {
        --stringsLength;
    }

    /* swap up to the last NUL */
    ds->swapInvChars(ds, inData, stringsLength, outData, pErrorCode);

    /* copy the bytes after the last NUL */
    ಯದಿ(inData!=outData && length>stringsLength) {
        uprv_memcpy((ವರ್ಣ *)outData+stringsLength, inChars+stringsLength, length-stringsLength);
    }

    /* return the length including padding bytes */
    ಯದಿ(U_SUCCESS(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ length;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
udata_printError(ಧ್ರುವ UDataSwapper *ds,
                 ಧ್ರುವ ವರ್ಣ *fmt,
                 ...) {
    va_list args;

    ಯದಿ(ds->printError!=nullptr) {
        va_start(args, fmt);
        ds->printError(ds->printErrorContext, fmt, args);
        va_end(args);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_swapDataHeader(ಧ್ರುವ UDataSwapper *ds,
                     ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
                     UErrorCode *pErrorCode) {
    ಧ್ರುವ DataHeader *pHeader;
    uint16_t headerSize, infoSize;

    /* argument checking */
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* check minimum length and magic bytes */
    pHeader=(ಧ್ರುವ DataHeader *)inData;
    ಯದಿ( (length>=0 && length<(int32_t)ಪರಿಮಾಣ(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    headerSize=ds->readUInt16(pHeader->dataHeader.headerSize);
    infoSize=ds->readUInt16(pHeader->info.size);

    ಯದಿ( headerSize<ಪರಿಮಾಣ(DataHeader) ||
        infoSize<ಪರಿಮಾಣ(UDataInfo) ||
        headerSize<(ಪರಿಮಾಣ(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         headerSize, infoSize, length);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(length>0) {
        DataHeader *outHeader;
        ಧ್ರುವ ವರ್ಣ *s;
        int32_t maxLength;

        /* Most of the fields are just bytes and need no swapping. */
        ಯದಿ(inData!=outData) {
            uprv_memcpy(outData, inData, headerSize);
        }
        outHeader=(DataHeader *)outData;

        outHeader->info.isBigEndian = ds->outIsBigEndian;
        outHeader->info.charsetFamily = ds->outCharset;

        /* swap headerSize */
        ds->swapArray16(ds, &pHeader->dataHeader.headerSize, 2, &outHeader->dataHeader.headerSize, pErrorCode);

        /* swap UDataInfo size and reservedWord */
        ds->swapArray16(ds, &pHeader->info.size, 4, &outHeader->info.size, pErrorCode);

        /* swap copyright statement after the UDataInfo */
        infoSize+=ಪರಿಮಾಣ(pHeader->dataHeader);
        s=(ಧ್ರುವ ವರ್ಣ *)inData+infoSize;
        maxLength=headerSize-infoSize;
        /* get the length of the string */
        ಪ್ರತಿ(length=0; length<maxLength && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->swapInvChars(ds, s, length, (ವರ್ಣ *)outData+infoSize, pErrorCode);
    }

    ಪ್ರತ್ಯಾಗಮ headerSize;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool inIsBigEndian, uint8_t inCharset,
                  UBool outIsBigEndian, uint8_t outCharset,
                  UErrorCode *pErrorCode) {
    UDataSwapper *swapper;

    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(inCharset>U_EBCDIC_FAMILY || outCharset>U_EBCDIC_FAMILY) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(ಪರಿಮಾಣ(UDataSwapper));
    ಯದಿ(swapper==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    uprv_memset(swapper, 0, ಪರಿಮಾಣ(UDataSwapper));

    /* set values and functions pointers according to in/out parameters */
    swapper->inIsBigEndian=inIsBigEndian;
    swapper->inCharset=inCharset;
    swapper->outIsBigEndian=outIsBigEndian;
    swapper->outCharset=outCharset;

    swapper->readUInt16= inIsBigEndian==U_IS_BIG_ENDIAN ? uprv_readDirectUInt16 : uprv_readSwapUInt16;
    swapper->readUInt32= inIsBigEndian==U_IS_BIG_ENDIAN ? uprv_readDirectUInt32 : uprv_readSwapUInt32;

    swapper->writeUInt16= outIsBigEndian==U_IS_BIG_ENDIAN ? uprv_writeDirectUInt16 : uprv_writeSwapUInt16;
    swapper->writeUInt32= outIsBigEndian==U_IS_BIG_ENDIAN ? uprv_writeDirectUInt32 : uprv_writeSwapUInt32;

    swapper->compareInvChars= outCharset==U_ASCII_FAMILY ? uprv_compareInvAscii : uprv_compareInvEbcdic;

    ಯದಿ(inIsBigEndian==outIsBigEndian) {
        swapper->swapArray16=uprv_copyArray16;
        swapper->swapArray32=uprv_copyArray32;
        swapper->swapArray64=uprv_copyArray64;
    } ಅನ್ಯಥಾ {
        swapper->swapArray16=uprv_swapArray16;
        swapper->swapArray32=uprv_swapArray32;
        swapper->swapArray64=uprv_swapArray64;
    }

    ಯದಿ(inCharset==U_ASCII_FAMILY) {
        swapper->swapInvChars= outCharset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } ಅನ್ಯಥಾ /* U_EBCDIC_FAMILY */ {
        swapper->swapInvChars= outCharset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    ಪ್ರತ್ಯಾಗಮ swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_ಒಳಹರಿವು_Data(ಧ್ರುವ ಶೂನ್ಯ *data, int32_t length,
                              UBool outIsBigEndian, uint8_t outCharset,
                              UErrorCode *pErrorCode) {
    ಧ್ರುವ DataHeader *pHeader;
    uint16_t headerSize, infoSize;
    UBool inIsBigEndian;
    int8_t inCharset;

    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ( data==nullptr ||
        (length>=0 && length<(int32_t)ಪರಿಮಾಣ(DataHeader)) ||
        outCharset>U_EBCDIC_FAMILY
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    pHeader=(ಧ್ರುವ DataHeader *)data;
    ಯದಿ( (length>=0 && length<(int32_t)ಪರಿಮಾಣ(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    inIsBigEndian=(UBool)pHeader->info.isBigEndian;
    inCharset=pHeader->info.charsetFamily;

    ಯದಿ(inIsBigEndian==U_IS_BIG_ENDIAN) {
        headerSize=pHeader->dataHeader.headerSize;
        infoSize=pHeader->info.size;
    } ಅನ್ಯಥಾ {
        headerSize=uprv_readSwapUInt16(pHeader->dataHeader.headerSize);
        infoSize=uprv_readSwapUInt16(pHeader->info.size);
    }

    ಯದಿ( headerSize<ಪರಿಮಾಣ(DataHeader) ||
        infoSize<ಪರಿಮಾಣ(UDataInfo) ||
        headerSize<(ಪರಿಮಾಣ(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಪ್ರತ್ಯಾಗಮ udata_openSwapper(inIsBigEndian, inCharset, outIsBigEndian, outCharset, pErrorCode);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
