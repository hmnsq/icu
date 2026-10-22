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

#ଅନ୍ତର୍ଭାଵ <stdarg.h>
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h" /* UDataInfo */
#ଅନ୍ତର୍ଭାଵ "ucmndata.h" /* DataHeader */
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

ସ୍ଥିର int32_t U_CALLCONV
uprv_swapArray16(ଅଚଳ UDataSwapper *ds,
                 ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
                 UErrorCode *pErrorCode) {
    ଅଚଳ uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* setup and swapping */
    p = static_cast<ଅଚଳ uint16_t*>(inData);
    q = static_cast<uint16_t*>(outData);
    count=length/2;
    ଯେତେବେଳେ(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    ଫେରନ୍ତୁ length;
}

ସ୍ଥିର int32_t U_CALLCONV
uprv_copyArray16(ଅଚଳ UDataSwapper *ds,
                 ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
                 UErrorCode *pErrorCode) {
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    ଫେରନ୍ତୁ length;
}

ସ୍ଥିର int32_t U_CALLCONV
uprv_swapArray32(ଅଚଳ UDataSwapper *ds,
                 ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
                 UErrorCode *pErrorCode) {
    ଅଚଳ uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* setup and swapping */
    p = static_cast<ଅଚଳ uint32_t*>(inData);
    q = static_cast<uint32_t*>(outData);
    count=length/4;
    ଯେତେବେଳେ(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    ଫେରନ୍ତୁ length;
}

ସ୍ଥିର int32_t U_CALLCONV
uprv_copyArray32(ଅଚଳ UDataSwapper *ds,
                 ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
                 UErrorCode *pErrorCode) {
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    ଫେରନ୍ତୁ length;
}

ସ୍ଥିର int32_t U_CALLCONV
uprv_swapArray64(ଅଚଳ UDataSwapper *ds,
                 ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
                 UErrorCode *pErrorCode) {
    ଅଚଳ uint64_t *p;
    uint64_t *q;
    int32_t count;

    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* setup and swapping */
    p = static_cast<ଅଚଳ uint64_t*>(inData);
    q = static_cast<uint64_t*>(outData);
    count=length/8;
    ଯେତେବେଳେ(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    ଫେରନ୍ତୁ length;
}

ସ୍ଥିର int32_t U_CALLCONV
uprv_copyArray64(ଅଚଳ UDataSwapper *ds,
                 ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
                 UErrorCode *pErrorCode) {
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    ଫେରନ୍ତୁ length;
}

ସ୍ଥିର uint16_t U_CALLCONV
uprv_readSwapUInt16(uint16_t x) {
    ଫେରନ୍ତୁ static_cast<uint16_t>((x << 8) | (x >> 8));
}

ସ୍ଥିର uint16_t U_CALLCONV
uprv_readDirectUInt16(uint16_t x) {
    ଫେରନ୍ତୁ x;
}

ସ୍ଥିର uint32_t U_CALLCONV
uprv_readSwapUInt32(uint32_t x) {
    ଫେରନ୍ତୁ (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ସ୍ଥିର uint32_t U_CALLCONV
uprv_readDirectUInt32(uint32_t x) {
    ଫେରନ୍ତୁ x;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
uprv_writeSwapUInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
uprv_writeDirectUInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
uprv_writeSwapUInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
uprv_writeDirectUInt32(uint32_t *p, uint32_t x) {
    *p=x;
}

U_CAPI int16_t U_EXPORT2
udata_readInt16(ଅଚଳ UDataSwapper *ds, int16_t x) {
    ଫେରନ୍ତୁ (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(ଅଚଳ UDataSwapper *ds, int32_t x) {
    ଫେରନ୍ତୁ (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_swapInvStringBlock(ଅଚଳ UDataSwapper *ds,
                         ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
                         UErrorCode *pErrorCode) {
    ଅଚଳ ଅକ୍ଷର *inChars;
    int32_t stringsLength;

    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(ds==nullptr || inData==nullptr || length<0 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    inChars=(ଅଚଳ ଅକ୍ଷର *)inData;
    stringsLength=length;
    ଯେତେବେଳେ(stringsLength>0 && inChars[stringsLength-1]!=0) {
        --stringsLength;
    }

    /* swap up to the last NUL */
    ds->swapInvChars(ds, inData, stringsLength, outData, pErrorCode);

    /* copy the bytes after the last NUL */
    ଯଦି(inData!=outData && length>stringsLength) {
        uprv_memcpy((ଅକ୍ଷର *)outData+stringsLength, inChars+stringsLength, length-stringsLength);
    }

    /* return the length including padding bytes */
    ଯଦି(U_SUCCESS(*pErrorCode)) {
        ଫେରନ୍ତୁ length;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ 0;
    }
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
udata_printError(ଅଚଳ UDataSwapper *ds,
                 ଅଚଳ ଅକ୍ଷର *fmt,
                 ...) {
    va_list args;

    ଯଦି(ds->printError!=nullptr) {
        va_start(args, fmt);
        ds->printError(ds->printErrorContext, fmt, args);
        va_end(args);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_swapDataHeader(ଅଚଳ UDataSwapper *ds,
                     ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
                     UErrorCode *pErrorCode) {
    ଅଚଳ DataHeader *pHeader;
    uint16_t headerSize, infoSize;

    /* argument checking */
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* check minimum length and magic bytes */
    pHeader=(ଅଚଳ DataHeader *)inData;
    ଯଦି( (length>=0 && length<(int32_t)ଆକାର(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    headerSize=ds->readUInt16(pHeader->dataHeader.headerSize);
    infoSize=ds->readUInt16(pHeader->info.size);

    ଯଦି( headerSize<ଆକାର(DataHeader) ||
        infoSize<ଆକାର(UDataInfo) ||
        headerSize<(ଆକାର(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         headerSize, infoSize, length);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି(length>0) {
        DataHeader *outHeader;
        ଅଚଳ ଅକ୍ଷର *s;
        int32_t maxLength;

        /* Most of the fields are just bytes and need no swapping. */
        ଯଦି(inData!=outData) {
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
        infoSize+=ଆକାର(pHeader->dataHeader);
        s=(ଅଚଳ ଅକ୍ଷର *)inData+infoSize;
        maxLength=headerSize-infoSize;
        /* get the length of the string */
        ପାଇଁ(length=0; length<maxLength && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->swapInvChars(ds, s, length, (ଅକ୍ଷର *)outData+infoSize, pErrorCode);
    }

    ଫେରନ୍ତୁ headerSize;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool inIsBigEndian, uint8_t inCharset,
                  UBool outIsBigEndian, uint8_t outCharset,
                  UErrorCode *pErrorCode) {
    UDataSwapper *swapper;

    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(inCharset>U_EBCDIC_FAMILY || outCharset>U_EBCDIC_FAMILY) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(ଆକାର(UDataSwapper));
    ଯଦି(swapper==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_memset(swapper, 0, ଆକାର(UDataSwapper));

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

    ଯଦି(inIsBigEndian==outIsBigEndian) {
        swapper->swapArray16=uprv_copyArray16;
        swapper->swapArray32=uprv_copyArray32;
        swapper->swapArray64=uprv_copyArray64;
    } ଅନ୍ୟଥା {
        swapper->swapArray16=uprv_swapArray16;
        swapper->swapArray32=uprv_swapArray32;
        swapper->swapArray64=uprv_swapArray64;
    }

    ଯଦି(inCharset==U_ASCII_FAMILY) {
        swapper->swapInvChars= outCharset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } ଅନ୍ୟଥା /* U_EBCDIC_FAMILY */ {
        swapper->swapInvChars= outCharset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    ଫେରନ୍ତୁ swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_ନିବେଶ_Data(ଅଚଳ ଶୂନ୍ୟ *data, int32_t length,
                              UBool outIsBigEndian, uint8_t outCharset,
                              UErrorCode *pErrorCode) {
    ଅଚଳ DataHeader *pHeader;
    uint16_t headerSize, infoSize;
    UBool inIsBigEndian;
    int8_t inCharset;

    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି( data==nullptr ||
        (length>=0 && length<(int32_t)ଆକାର(DataHeader)) ||
        outCharset>U_EBCDIC_FAMILY
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    pHeader=(ଅଚଳ DataHeader *)data;
    ଯଦି( (length>=0 && length<(int32_t)ଆକାର(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    inIsBigEndian=(UBool)pHeader->info.isBigEndian;
    inCharset=pHeader->info.charsetFamily;

    ଯଦି(inIsBigEndian==U_IS_BIG_ENDIAN) {
        headerSize=pHeader->dataHeader.headerSize;
        infoSize=pHeader->info.size;
    } ଅନ୍ୟଥା {
        headerSize=uprv_readSwapUInt16(pHeader->dataHeader.headerSize);
        infoSize=uprv_readSwapUInt16(pHeader->info.size);
    }

    ଯଦି( headerSize<ଆକାର(DataHeader) ||
        infoSize<ଆକାର(UDataInfo) ||
        headerSize<(ଆକାର(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    ଫେରନ୍ତୁ udata_openSwapper(inIsBigEndian, inCharset, outIsBigEndian, outCharset, pErrorCode);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
