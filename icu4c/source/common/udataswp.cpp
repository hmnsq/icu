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

#અન્તર્ભાવ <stdarg.h>
#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/udata.h" /* UDataInfo */
#અન્તર્ભાવ "ucmndata.h" /* DataHeader */
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

સ્થિર int32_t U_CALLCONV
uprv_swapArray16(અચલ UDataSwapper *ds,
                 અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
                 UErrorCode *pErrorCode) {
    અચલ uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    /* setup and swapping */
    p = static_cast<અચલ uint16_t*>(inData);
    q = static_cast<uint16_t*>(outData);
    count=length/2;
    જ્યારે(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    પાછા length;
}

સ્થિર int32_t U_CALLCONV
uprv_copyArray16(અચલ UDataSwapper *ds,
                 અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
                 UErrorCode *pErrorCode) {
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    જો(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    પાછા length;
}

સ્થિર int32_t U_CALLCONV
uprv_swapArray32(અચલ UDataSwapper *ds,
                 અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
                 UErrorCode *pErrorCode) {
    અચલ uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    /* setup and swapping */
    p = static_cast<અચલ uint32_t*>(inData);
    q = static_cast<uint32_t*>(outData);
    count=length/4;
    જ્યારે(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    પાછા length;
}

સ્થિર int32_t U_CALLCONV
uprv_copyArray32(અચલ UDataSwapper *ds,
                 અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
                 UErrorCode *pErrorCode) {
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    જો(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    પાછા length;
}

સ્થિર int32_t U_CALLCONV
uprv_swapArray64(અચલ UDataSwapper *ds,
                 અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
                 UErrorCode *pErrorCode) {
    અચલ uint64_t *p;
    uint64_t *q;
    int32_t count;

    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    /* setup and swapping */
    p = static_cast<અચલ uint64_t*>(inData);
    q = static_cast<uint64_t*>(outData);
    count=length/8;
    જ્યારે(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    પાછા length;
}

સ્થિર int32_t U_CALLCONV
uprv_copyArray64(અચલ UDataSwapper *ds,
                 અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
                 UErrorCode *pErrorCode) {
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    જો(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    પાછા length;
}

સ્થિર uint16_t U_CALLCONV
uprv_readSwapUInt16(uint16_t x) {
    પાછા static_cast<uint16_t>((x << 8) | (x >> 8));
}

સ્થિર uint16_t U_CALLCONV
uprv_readDirectUInt16(uint16_t x) {
    પાછા x;
}

સ્થિર uint32_t U_CALLCONV
uprv_readSwapUInt32(uint32_t x) {
    પાછા (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

સ્થિર uint32_t U_CALLCONV
uprv_readDirectUInt32(uint32_t x) {
    પાછા x;
}

સ્થિર શૂન્ય U_CALLCONV
uprv_writeSwapUInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

સ્થિર શૂન્ય U_CALLCONV
uprv_writeDirectUInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

સ્થિર શૂન્ય U_CALLCONV
uprv_writeSwapUInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

સ્થિર શૂન્ય U_CALLCONV
uprv_writeDirectUInt32(uint32_t *p, uint32_t x) {
    *p=x;
}

U_CAPI int16_t U_EXPORT2
udata_readInt16(અચલ UDataSwapper *ds, int16_t x) {
    પાછા (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(અચલ UDataSwapper *ds, int32_t x) {
    પાછા (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_swapInvStringBlock(અચલ UDataSwapper *ds,
                         અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
                         UErrorCode *pErrorCode) {
    અચલ અક્ષર *inChars;
    int32_t stringsLength;

    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(ds==nullptr || inData==nullptr || length<0 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    inChars=(અચલ અક્ષર *)inData;
    stringsLength=length;
    જ્યારે(stringsLength>0 && inChars[stringsLength-1]!=0) {
        --stringsLength;
    }

    /* swap up to the last NUL */
    ds->swapInvChars(ds, inData, stringsLength, outData, pErrorCode);

    /* copy the bytes after the last NUL */
    જો(inData!=outData && length>stringsLength) {
        uprv_memcpy((અક્ષર *)outData+stringsLength, inChars+stringsLength, length-stringsLength);
    }

    /* return the length including padding bytes */
    જો(U_SUCCESS(*pErrorCode)) {
        પાછા length;
    } નહીંતર {
        પાછા 0;
    }
}

U_CAPI શૂન્ય U_EXPORT2
udata_printError(અચલ UDataSwapper *ds,
                 અચલ અક્ષર *fmt,
                 ...) {
    va_list args;

    જો(ds->printError!=nullptr) {
        va_start(args, fmt);
        ds->printError(ds->printErrorContext, fmt, args);
        va_end(args);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_swapDataHeader(અચલ UDataSwapper *ds,
                     અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
                     UErrorCode *pErrorCode) {
    અચલ DataHeader *pHeader;
    uint16_t headerSize, infoSize;

    /* argument checking */
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    /* check minimum length and magic bytes */
    pHeader=(અચલ DataHeader *)inData;
    જો( (length>=0 && length<(int32_t)કદ(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *pErrorCode=U_UNSUPPORTED_ERROR;
        પાછા 0;
    }

    headerSize=ds->readUInt16(pHeader->dataHeader.headerSize);
    infoSize=ds->readUInt16(pHeader->info.size);

    જો( headerSize<કદ(DataHeader) ||
        infoSize<કદ(UDataInfo) ||
        headerSize<(કદ(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         headerSize, infoSize, length);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા 0;
    }

    જો(length>0) {
        DataHeader *outHeader;
        અચલ અક્ષર *s;
        int32_t maxLength;

        /* Most of the fields are just bytes and need no swapping. */
        જો(inData!=outData) {
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
        infoSize+=કદ(pHeader->dataHeader);
        s=(અચલ અક્ષર *)inData+infoSize;
        maxLength=headerSize-infoSize;
        /* get the length of the string */
        માટે(length=0; length<maxLength && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->swapInvChars(ds, s, length, (અક્ષર *)outData+infoSize, pErrorCode);
    }

    પાછા headerSize;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool inIsBigEndian, uint8_t inCharset,
                  UBool outIsBigEndian, uint8_t outCharset,
                  UErrorCode *pErrorCode) {
    UDataSwapper *swapper;

    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    જો(inCharset>U_EBCDIC_FAMILY || outCharset>U_EBCDIC_FAMILY) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(કદ(UDataSwapper));
    જો(swapper==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    uprv_memset(swapper, 0, કદ(UDataSwapper));

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

    જો(inIsBigEndian==outIsBigEndian) {
        swapper->swapArray16=uprv_copyArray16;
        swapper->swapArray32=uprv_copyArray32;
        swapper->swapArray64=uprv_copyArray64;
    } નહીંતર {
        swapper->swapArray16=uprv_swapArray16;
        swapper->swapArray32=uprv_swapArray32;
        swapper->swapArray64=uprv_swapArray64;
    }

    જો(inCharset==U_ASCII_FAMILY) {
        swapper->swapInvChars= outCharset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } નહીંતર /* U_EBCDIC_FAMILY */ {
        swapper->swapInvChars= outCharset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    પાછા swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_આગત_Data(અચલ શૂન્ય *data, int32_t length,
                              UBool outIsBigEndian, uint8_t outCharset,
                              UErrorCode *pErrorCode) {
    અચલ DataHeader *pHeader;
    uint16_t headerSize, infoSize;
    UBool inIsBigEndian;
    int8_t inCharset;

    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    જો( data==nullptr ||
        (length>=0 && length<(int32_t)કદ(DataHeader)) ||
        outCharset>U_EBCDIC_FAMILY
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

    pHeader=(અચલ DataHeader *)data;
    જો( (length>=0 && length<(int32_t)કદ(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        પાછા nullptr;
    }

    inIsBigEndian=(UBool)pHeader->info.isBigEndian;
    inCharset=pHeader->info.charsetFamily;

    જો(inIsBigEndian==U_IS_BIG_ENDIAN) {
        headerSize=pHeader->dataHeader.headerSize;
        infoSize=pHeader->info.size;
    } નહીંતર {
        headerSize=uprv_readSwapUInt16(pHeader->dataHeader.headerSize);
        infoSize=uprv_readSwapUInt16(pHeader->info.size);
    }

    જો( headerSize<કદ(DataHeader) ||
        infoSize<કદ(UDataInfo) ||
        headerSize<(કદ(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        પાછા nullptr;
    }

    પાછા udata_openSwapper(inIsBigEndian, inCharset, outIsBigEndian, outCharset, pErrorCode);
}

U_CAPI શૂન્ય U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
