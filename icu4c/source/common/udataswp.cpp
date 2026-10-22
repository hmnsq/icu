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

#அந்தர்பாவ <stdarg.h>
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/udata.h" /* UDataInfo */
#அந்தர்பாவ "ucmndata.h" /* DataHeader */
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

நிலையான int32_t U_CALLCONV
uprv_swapArray16(மாறிலி UDataSwapper *ds,
                 மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                 UErrorCode *pErrorCode) {
    மாறிலி uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* setup and swapping */
    p = static_cast<மாறிலி uint16_t*>(inData);
    q = static_cast<uint16_t*>(outData);
    count=length/2;
    போது(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    திரும்பு length;
}

நிலையான int32_t U_CALLCONV
uprv_copyArray16(மாறிலி UDataSwapper *ds,
                 மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                 UErrorCode *pErrorCode) {
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    எனில்(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    திரும்பு length;
}

நிலையான int32_t U_CALLCONV
uprv_swapArray32(மாறிலி UDataSwapper *ds,
                 மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                 UErrorCode *pErrorCode) {
    மாறிலி uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* setup and swapping */
    p = static_cast<மாறிலி uint32_t*>(inData);
    q = static_cast<uint32_t*>(outData);
    count=length/4;
    போது(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    திரும்பு length;
}

நிலையான int32_t U_CALLCONV
uprv_copyArray32(மாறிலி UDataSwapper *ds,
                 மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                 UErrorCode *pErrorCode) {
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    எனில்(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    திரும்பு length;
}

நிலையான int32_t U_CALLCONV
uprv_swapArray64(மாறிலி UDataSwapper *ds,
                 மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                 UErrorCode *pErrorCode) {
    மாறிலி uint64_t *p;
    uint64_t *q;
    int32_t count;

    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* setup and swapping */
    p = static_cast<மாறிலி uint64_t*>(inData);
    q = static_cast<uint64_t*>(outData);
    count=length/8;
    போது(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    திரும்பு length;
}

நிலையான int32_t U_CALLCONV
uprv_copyArray64(மாறிலி UDataSwapper *ds,
                 மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                 UErrorCode *pErrorCode) {
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    எனில்(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    திரும்பு length;
}

நிலையான uint16_t U_CALLCONV
uprv_readSwapUInt16(uint16_t x) {
    திரும்பு static_cast<uint16_t>((x << 8) | (x >> 8));
}

நிலையான uint16_t U_CALLCONV
uprv_readDirectUInt16(uint16_t x) {
    திரும்பு x;
}

நிலையான uint32_t U_CALLCONV
uprv_readSwapUInt32(uint32_t x) {
    திரும்பு (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

நிலையான uint32_t U_CALLCONV
uprv_readDirectUInt32(uint32_t x) {
    திரும்பு x;
}

நிலையான வெறுமை U_CALLCONV
uprv_writeSwapUInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

நிலையான வெறுமை U_CALLCONV
uprv_writeDirectUInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

நிலையான வெறுமை U_CALLCONV
uprv_writeSwapUInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

நிலையான வெறுமை U_CALLCONV
uprv_writeDirectUInt32(uint32_t *p, uint32_t x) {
    *p=x;
}

U_CAPI int16_t U_EXPORT2
udata_readInt16(மாறிலி UDataSwapper *ds, int16_t x) {
    திரும்பு (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(மாறிலி UDataSwapper *ds, int32_t x) {
    திரும்பு (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_swapInvStringBlock(மாறிலி UDataSwapper *ds,
                         மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                         UErrorCode *pErrorCode) {
    மாறிலி எழுத்து *inChars;
    int32_t stringsLength;

    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(ds==nullptr || inData==nullptr || length<0 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    inChars=(மாறிலி எழுத்து *)inData;
    stringsLength=length;
    போது(stringsLength>0 && inChars[stringsLength-1]!=0) {
        --stringsLength;
    }

    /* swap up to the last NUL */
    ds->swapInvChars(ds, inData, stringsLength, outData, pErrorCode);

    /* copy the bytes after the last NUL */
    எனில்(inData!=outData && length>stringsLength) {
        uprv_memcpy((எழுத்து *)outData+stringsLength, inChars+stringsLength, length-stringsLength);
    }

    /* return the length including padding bytes */
    எனில்(U_SUCCESS(*pErrorCode)) {
        திரும்பு length;
    } இல்லையெனில் {
        திரும்பு 0;
    }
}

U_CAPI வெறுமை U_EXPORT2
udata_printError(மாறிலி UDataSwapper *ds,
                 மாறிலி எழுத்து *fmt,
                 ...) {
    va_list args;

    எனில்(ds->printError!=nullptr) {
        va_start(args, fmt);
        ds->printError(ds->printErrorContext, fmt, args);
        va_end(args);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_swapDataHeader(மாறிலி UDataSwapper *ds,
                     மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                     UErrorCode *pErrorCode) {
    மாறிலி DataHeader *pHeader;
    uint16_t headerSize, infoSize;

    /* argument checking */
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* check minimum length and magic bytes */
    pHeader=(மாறிலி DataHeader *)inData;
    எனில்( (length>=0 && length<(int32_t)அளவு(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *pErrorCode=U_UNSUPPORTED_ERROR;
        திரும்பு 0;
    }

    headerSize=ds->readUInt16(pHeader->dataHeader.headerSize);
    infoSize=ds->readUInt16(pHeader->info.size);

    எனில்( headerSize<அளவு(DataHeader) ||
        infoSize<அளவு(UDataInfo) ||
        headerSize<(அளவு(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         headerSize, infoSize, length);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    எனில்(length>0) {
        DataHeader *outHeader;
        மாறிலி எழுத்து *s;
        int32_t maxLength;

        /* Most of the fields are just bytes and need no swapping. */
        எனில்(inData!=outData) {
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
        infoSize+=அளவு(pHeader->dataHeader);
        s=(மாறிலி எழுத்து *)inData+infoSize;
        maxLength=headerSize-infoSize;
        /* get the length of the string */
        க்கு(length=0; length<maxLength && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->swapInvChars(ds, s, length, (எழுத்து *)outData+infoSize, pErrorCode);
    }

    திரும்பு headerSize;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool inIsBigEndian, uint8_t inCharset,
                  UBool outIsBigEndian, uint8_t outCharset,
                  UErrorCode *pErrorCode) {
    UDataSwapper *swapper;

    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    எனில்(inCharset>U_EBCDIC_FAMILY || outCharset>U_EBCDIC_FAMILY) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(அளவு(UDataSwapper));
    எனில்(swapper==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    uprv_memset(swapper, 0, அளவு(UDataSwapper));

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

    எனில்(inIsBigEndian==outIsBigEndian) {
        swapper->swapArray16=uprv_copyArray16;
        swapper->swapArray32=uprv_copyArray32;
        swapper->swapArray64=uprv_copyArray64;
    } இல்லையெனில் {
        swapper->swapArray16=uprv_swapArray16;
        swapper->swapArray32=uprv_swapArray32;
        swapper->swapArray64=uprv_swapArray64;
    }

    எனில்(inCharset==U_ASCII_FAMILY) {
        swapper->swapInvChars= outCharset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } இல்லையெனில் /* U_EBCDIC_FAMILY */ {
        swapper->swapInvChars= outCharset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    திரும்பு swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_உள்ளீடு_Data(மாறிலி வெறுமை *data, int32_t length,
                              UBool outIsBigEndian, uint8_t outCharset,
                              UErrorCode *pErrorCode) {
    மாறிலி DataHeader *pHeader;
    uint16_t headerSize, infoSize;
    UBool inIsBigEndian;
    int8_t inCharset;

    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    எனில்( data==nullptr ||
        (length>=0 && length<(int32_t)அளவு(DataHeader)) ||
        outCharset>U_EBCDIC_FAMILY
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    pHeader=(மாறிலி DataHeader *)data;
    எனில்( (length>=0 && length<(int32_t)அளவு(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        திரும்பு nullptr;
    }

    inIsBigEndian=(UBool)pHeader->info.isBigEndian;
    inCharset=pHeader->info.charsetFamily;

    எனில்(inIsBigEndian==U_IS_BIG_ENDIAN) {
        headerSize=pHeader->dataHeader.headerSize;
        infoSize=pHeader->info.size;
    } இல்லையெனில் {
        headerSize=uprv_readSwapUInt16(pHeader->dataHeader.headerSize);
        infoSize=uprv_readSwapUInt16(pHeader->info.size);
    }

    எனில்( headerSize<அளவு(DataHeader) ||
        infoSize<அளவு(UDataInfo) ||
        headerSize<(அளவு(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        திரும்பு nullptr;
    }

    திரும்பு udata_openSwapper(inIsBigEndian, inCharset, outIsBigEndian, outCharset, pErrorCode);
}

U_CAPI வெறுமை U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
