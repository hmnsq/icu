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

#അന്തര്ഭാവ <stdarg.h>
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/udata.h" /* UDataInfo */
#അന്തര്ഭാവ "ucmndata.h" /* DataHeader */
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

സ്ഥിതം int32_t U_CALLCONV
uprv_swapArray16(സ്ഥിരം UDataSwapper *ds,
                 സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
                 UErrorCode *pErrorCode) {
    സ്ഥിരം uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* setup and swapping */
    p = static_cast<സ്ഥിരം uint16_t*>(inData);
    q = static_cast<uint16_t*>(outData);
    count=length/2;
    ആകുമ്പോൾ(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    തിരികെ length;
}

സ്ഥിതം int32_t U_CALLCONV
uprv_copyArray16(സ്ഥിരം UDataSwapper *ds,
                 സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
                 UErrorCode *pErrorCode) {
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    എങ്കിൽ(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    തിരികെ length;
}

സ്ഥിതം int32_t U_CALLCONV
uprv_swapArray32(സ്ഥിരം UDataSwapper *ds,
                 സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
                 UErrorCode *pErrorCode) {
    സ്ഥിരം uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* setup and swapping */
    p = static_cast<സ്ഥിരം uint32_t*>(inData);
    q = static_cast<uint32_t*>(outData);
    count=length/4;
    ആകുമ്പോൾ(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    തിരികെ length;
}

സ്ഥിതം int32_t U_CALLCONV
uprv_copyArray32(സ്ഥിരം UDataSwapper *ds,
                 സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
                 UErrorCode *pErrorCode) {
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    എങ്കിൽ(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    തിരികെ length;
}

സ്ഥിതം int32_t U_CALLCONV
uprv_swapArray64(സ്ഥിരം UDataSwapper *ds,
                 സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
                 UErrorCode *pErrorCode) {
    സ്ഥിരം uint64_t *p;
    uint64_t *q;
    int32_t count;

    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* setup and swapping */
    p = static_cast<സ്ഥിരം uint64_t*>(inData);
    q = static_cast<uint64_t*>(outData);
    count=length/8;
    ആകുമ്പോൾ(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    തിരികെ length;
}

സ്ഥിതം int32_t U_CALLCONV
uprv_copyArray64(സ്ഥിരം UDataSwapper *ds,
                 സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
                 UErrorCode *pErrorCode) {
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    എങ്കിൽ(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    തിരികെ length;
}

സ്ഥിതം uint16_t U_CALLCONV
uprv_readSwapUInt16(uint16_t x) {
    തിരികെ static_cast<uint16_t>((x << 8) | (x >> 8));
}

സ്ഥിതം uint16_t U_CALLCONV
uprv_readDirectUInt16(uint16_t x) {
    തിരികെ x;
}

സ്ഥിതം uint32_t U_CALLCONV
uprv_readSwapUInt32(uint32_t x) {
    തിരികെ (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

സ്ഥിതം uint32_t U_CALLCONV
uprv_readDirectUInt32(uint32_t x) {
    തിരികെ x;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
uprv_writeSwapUInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

സ്ഥിതം ശൂന്യം U_CALLCONV
uprv_writeDirectUInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
uprv_writeSwapUInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

സ്ഥിതം ശൂന്യം U_CALLCONV
uprv_writeDirectUInt32(uint32_t *p, uint32_t x) {
    *p=x;
}

U_CAPI int16_t U_EXPORT2
udata_readInt16(സ്ഥിരം UDataSwapper *ds, int16_t x) {
    തിരികെ (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(സ്ഥിരം UDataSwapper *ds, int32_t x) {
    തിരികെ (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_swapInvStringBlock(സ്ഥിരം UDataSwapper *ds,
                         സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
                         UErrorCode *pErrorCode) {
    സ്ഥിരം അക്ഷരം *inChars;
    int32_t stringsLength;

    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(ds==nullptr || inData==nullptr || length<0 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    inChars=(സ്ഥിരം അക്ഷരം *)inData;
    stringsLength=length;
    ആകുമ്പോൾ(stringsLength>0 && inChars[stringsLength-1]!=0) {
        --stringsLength;
    }

    /* swap up to the last NUL */
    ds->swapInvChars(ds, inData, stringsLength, outData, pErrorCode);

    /* copy the bytes after the last NUL */
    എങ്കിൽ(inData!=outData && length>stringsLength) {
        uprv_memcpy((അക്ഷരം *)outData+stringsLength, inChars+stringsLength, length-stringsLength);
    }

    /* return the length including padding bytes */
    എങ്കിൽ(U_SUCCESS(*pErrorCode)) {
        തിരികെ length;
    } അല്ലെങ്കിൽ {
        തിരികെ 0;
    }
}

U_CAPI ശൂന്യം U_EXPORT2
udata_printError(സ്ഥിരം UDataSwapper *ds,
                 സ്ഥിരം അക്ഷരം *fmt,
                 ...) {
    va_list args;

    എങ്കിൽ(ds->printError!=nullptr) {
        va_start(args, fmt);
        ds->printError(ds->printErrorContext, fmt, args);
        va_end(args);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_swapDataHeader(സ്ഥിരം UDataSwapper *ds,
                     സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
                     UErrorCode *pErrorCode) {
    സ്ഥിരം DataHeader *pHeader;
    uint16_t headerSize, infoSize;

    /* argument checking */
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* check minimum length and magic bytes */
    pHeader=(സ്ഥിരം DataHeader *)inData;
    എങ്കിൽ( (length>=0 && length<(int32_t)വലുപ്പം(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *pErrorCode=U_UNSUPPORTED_ERROR;
        തിരികെ 0;
    }

    headerSize=ds->readUInt16(pHeader->dataHeader.headerSize);
    infoSize=ds->readUInt16(pHeader->info.size);

    എങ്കിൽ( headerSize<വലുപ്പം(DataHeader) ||
        infoSize<വലുപ്പം(UDataInfo) ||
        headerSize<(വലുപ്പം(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         headerSize, infoSize, length);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0;
    }

    എങ്കിൽ(length>0) {
        DataHeader *outHeader;
        സ്ഥിരം അക്ഷരം *s;
        int32_t maxLength;

        /* Most of the fields are just bytes and need no swapping. */
        എങ്കിൽ(inData!=outData) {
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
        infoSize+=വലുപ്പം(pHeader->dataHeader);
        s=(സ്ഥിരം അക്ഷരം *)inData+infoSize;
        maxLength=headerSize-infoSize;
        /* get the length of the string */
        വേണ്ടി(length=0; length<maxLength && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->swapInvChars(ds, s, length, (അക്ഷരം *)outData+infoSize, pErrorCode);
    }

    തിരികെ headerSize;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool inIsBigEndian, uint8_t inCharset,
                  UBool outIsBigEndian, uint8_t outCharset,
                  UErrorCode *pErrorCode) {
    UDataSwapper *swapper;

    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(inCharset>U_EBCDIC_FAMILY || outCharset>U_EBCDIC_FAMILY) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(വലുപ്പം(UDataSwapper));
    എങ്കിൽ(swapper==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    uprv_memset(swapper, 0, വലുപ്പം(UDataSwapper));

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

    എങ്കിൽ(inIsBigEndian==outIsBigEndian) {
        swapper->swapArray16=uprv_copyArray16;
        swapper->swapArray32=uprv_copyArray32;
        swapper->swapArray64=uprv_copyArray64;
    } അല്ലെങ്കിൽ {
        swapper->swapArray16=uprv_swapArray16;
        swapper->swapArray32=uprv_swapArray32;
        swapper->swapArray64=uprv_swapArray64;
    }

    എങ്കിൽ(inCharset==U_ASCII_FAMILY) {
        swapper->swapInvChars= outCharset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } അല്ലെങ്കിൽ /* U_EBCDIC_FAMILY */ {
        swapper->swapInvChars= outCharset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    തിരികെ swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_നിവേശം_Data(സ്ഥിരം ശൂന്യം *data, int32_t length,
                              UBool outIsBigEndian, uint8_t outCharset,
                              UErrorCode *pErrorCode) {
    സ്ഥിരം DataHeader *pHeader;
    uint16_t headerSize, infoSize;
    UBool inIsBigEndian;
    int8_t inCharset;

    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ( data==nullptr ||
        (length>=0 && length<(int32_t)വലുപ്പം(DataHeader)) ||
        outCharset>U_EBCDIC_FAMILY
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }

    pHeader=(സ്ഥിരം DataHeader *)data;
    എങ്കിൽ( (length>=0 && length<(int32_t)വലുപ്പം(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        തിരികെ nullptr;
    }

    inIsBigEndian=(UBool)pHeader->info.isBigEndian;
    inCharset=pHeader->info.charsetFamily;

    എങ്കിൽ(inIsBigEndian==U_IS_BIG_ENDIAN) {
        headerSize=pHeader->dataHeader.headerSize;
        infoSize=pHeader->info.size;
    } അല്ലെങ്കിൽ {
        headerSize=uprv_readSwapUInt16(pHeader->dataHeader.headerSize);
        infoSize=uprv_readSwapUInt16(pHeader->info.size);
    }

    എങ്കിൽ( headerSize<വലുപ്പം(DataHeader) ||
        infoSize<വലുപ്പം(UDataInfo) ||
        headerSize<(വലുപ്പം(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        തിരികെ nullptr;
    }

    തിരികെ udata_openSwapper(inIsBigEndian, inCharset, outIsBigEndian, outCharset, pErrorCode);
}

U_CAPI ശൂന്യം U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
