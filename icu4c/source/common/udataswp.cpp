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

#ضمن <stdarg.h>
#ضمن "unicode/utypes.h"
#ضمن "unicode/udata.h" /* UDataInfo */
#ضمن "ucmndata.h" /* DataHeader */
#ضمن "cmemory.h"
#ضمن "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

ساكن int32_t U_CALLCONV
uprv_swapArray16(ثابت UDataSwapper *ds,
                 ثابت فراغ *inData, int32_t length, فراغ *outData,
                 UErrorCode *pErrorCode) {
    ثابت uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    𐤀𐤌(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint16_t*>(inData);
    q = static_cast<uint16_t*>(outData);
    count=length/2;
    طالما(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    أرجع length;
}

ساكن int32_t U_CALLCONV
uprv_copyArray16(ثابت UDataSwapper *ds,
                 ثابت فراغ *inData, int32_t length, فراغ *outData,
                 UErrorCode *pErrorCode) {
    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    𐤀𐤌(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    𐤀𐤌(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    أرجع length;
}

ساكن int32_t U_CALLCONV
uprv_swapArray32(ثابت UDataSwapper *ds,
                 ثابت فراغ *inData, int32_t length, فراغ *outData,
                 UErrorCode *pErrorCode) {
    ثابت uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    𐤀𐤌(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint32_t*>(inData);
    q = static_cast<uint32_t*>(outData);
    count=length/4;
    طالما(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    أرجع length;
}

ساكن int32_t U_CALLCONV
uprv_copyArray32(ثابت UDataSwapper *ds,
                 ثابت فراغ *inData, int32_t length, فراغ *outData,
                 UErrorCode *pErrorCode) {
    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    𐤀𐤌(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    𐤀𐤌(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    أرجع length;
}

ساكن int32_t U_CALLCONV
uprv_swapArray64(ثابت UDataSwapper *ds,
                 ثابت فراغ *inData, int32_t length, فراغ *outData,
                 UErrorCode *pErrorCode) {
    ثابت uint64_t *p;
    uint64_t *q;
    int32_t count;

    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    𐤀𐤌(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint64_t*>(inData);
    q = static_cast<uint64_t*>(outData);
    count=length/8;
    طالما(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    أرجع length;
}

ساكن int32_t U_CALLCONV
uprv_copyArray64(ثابت UDataSwapper *ds,
                 ثابت فراغ *inData, int32_t length, فراغ *outData,
                 UErrorCode *pErrorCode) {
    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    𐤀𐤌(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    𐤀𐤌(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    أرجع length;
}

ساكن uint16_t U_CALLCONV
uprv_readSwapUInt16(uint16_t x) {
    أرجع static_cast<uint16_t>((x << 8) | (x >> 8));
}

ساكن uint16_t U_CALLCONV
uprv_readDirectUInt16(uint16_t x) {
    أرجع x;
}

ساكن uint32_t U_CALLCONV
uprv_readSwapUInt32(uint32_t x) {
    أرجع (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ساكن uint32_t U_CALLCONV
uprv_readDirectUInt32(uint32_t x) {
    أرجع x;
}

ساكن فراغ U_CALLCONV
uprv_writeSwapUInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

ساكن فراغ U_CALLCONV
uprv_writeDirectUInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

ساكن فراغ U_CALLCONV
uprv_writeSwapUInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ساكن فراغ U_CALLCONV
uprv_writeDirectUInt32(uint32_t *p, uint32_t x) {
    *p=x;
}

U_CAPI int16_t U_EXPORT2
udata_readInt16(ثابت UDataSwapper *ds, int16_t x) {
    أرجع (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(ثابت UDataSwapper *ds, int32_t x) {
    أرجع (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_swapInvStringBlock(ثابت UDataSwapper *ds,
                         ثابت فراغ *inData, int32_t length, فراغ *outData,
                         UErrorCode *pErrorCode) {
    ثابت حرف *inChars;
    int32_t stringsLength;

    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    𐤀𐤌(ds==nullptr || inData==nullptr || length<0 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    inChars=(ثابت حرف *)inData;
    stringsLength=length;
    طالما(stringsLength>0 && inChars[stringsLength-1]!=0) {
        --stringsLength;
    }

    /* swap up to the last NUL */
    ds->swapInvChars(ds, inData, stringsLength, outData, pErrorCode);

    /* copy the bytes after the last NUL */
    𐤀𐤌(inData!=outData && length>stringsLength) {
        uprv_memcpy((حرف *)outData+stringsLength, inChars+stringsLength, length-stringsLength);
    }

    /* return the length including padding bytes */
    𐤀𐤌(U_SUCCESS(*pErrorCode)) {
        أرجع length;
    } وإلا {
        أرجع 0;
    }
}

U_CAPI فراغ U_EXPORT2
udata_printError(ثابت UDataSwapper *ds,
                 ثابت حرف *fmt,
                 ...) {
    va_list args;

    𐤀𐤌(ds->printError!=nullptr) {
        va_start(args, fmt);
        ds->printError(ds->printErrorContext, fmt, args);
        va_end(args);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_swapDataHeader(ثابت UDataSwapper *ds,
                     ثابت فراغ *inData, int32_t length, فراغ *outData,
                     UErrorCode *pErrorCode) {
    ثابت DataHeader *pHeader;
    uint16_t headerSize, infoSize;

    /* argument checking */
    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    𐤀𐤌(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* check minimum length and magic bytes */
    pHeader=(ثابت DataHeader *)inData;
    𐤀𐤌( (length>=0 && length<(int32_t)حجم(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *pErrorCode=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    headerSize=ds->readUInt16(pHeader->dataHeader.headerSize);
    infoSize=ds->readUInt16(pHeader->info.size);

    𐤀𐤌( headerSize<حجم(DataHeader) ||
        infoSize<حجم(UDataInfo) ||
        headerSize<(حجم(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         headerSize, infoSize, length);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    𐤀𐤌(length>0) {
        DataHeader *outHeader;
        ثابت حرف *s;
        int32_t maxLength;

        /* Most of the fields are just bytes and need no swapping. */
        𐤀𐤌(inData!=outData) {
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
        infoSize+=حجم(pHeader->dataHeader);
        s=(ثابت حرف *)inData+infoSize;
        maxLength=headerSize-infoSize;
        /* get the length of the string */
        لكل(length=0; length<maxLength && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->swapInvChars(ds, s, length, (حرف *)outData+infoSize, pErrorCode);
    }

    أرجع headerSize;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool inIsBigEndian, uint8_t inCharset,
                  UBool outIsBigEndian, uint8_t outCharset,
                  UErrorCode *pErrorCode) {
    UDataSwapper *swapper;

    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }
    𐤀𐤌(inCharset>U_EBCDIC_FAMILY || outCharset>U_EBCDIC_FAMILY) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(حجم(UDataSwapper));
    𐤀𐤌(swapper==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    uprv_memset(swapper, 0, حجم(UDataSwapper));

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

    𐤀𐤌(inIsBigEndian==outIsBigEndian) {
        swapper->swapArray16=uprv_copyArray16;
        swapper->swapArray32=uprv_copyArray32;
        swapper->swapArray64=uprv_copyArray64;
    } وإلا {
        swapper->swapArray16=uprv_swapArray16;
        swapper->swapArray32=uprv_swapArray32;
        swapper->swapArray64=uprv_swapArray64;
    }

    𐤀𐤌(inCharset==U_ASCII_FAMILY) {
        swapper->swapInvChars= outCharset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } وإلا /* U_EBCDIC_FAMILY */ {
        swapper->swapInvChars= outCharset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    أرجع swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapperForInputData(ثابت فراغ *data, int32_t length,
                              UBool outIsBigEndian, uint8_t outCharset,
                              UErrorCode *pErrorCode) {
    ثابت DataHeader *pHeader;
    uint16_t headerSize, infoSize;
    UBool inIsBigEndian;
    int8_t inCharset;

    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }
    𐤀𐤌( data==nullptr ||
        (length>=0 && length<(int32_t)حجم(DataHeader)) ||
        outCharset>U_EBCDIC_FAMILY
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }

    pHeader=(ثابت DataHeader *)data;
    𐤀𐤌( (length>=0 && length<(int32_t)حجم(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        أرجع nullptr;
    }

    inIsBigEndian=(UBool)pHeader->info.isBigEndian;
    inCharset=pHeader->info.charsetFamily;

    𐤀𐤌(inIsBigEndian==U_IS_BIG_ENDIAN) {
        headerSize=pHeader->dataHeader.headerSize;
        infoSize=pHeader->info.size;
    } وإلا {
        headerSize=uprv_readSwapUInt16(pHeader->dataHeader.headerSize);
        infoSize=uprv_readSwapUInt16(pHeader->info.size);
    }

    𐤀𐤌( headerSize<حجم(DataHeader) ||
        infoSize<حجم(UDataInfo) ||
        headerSize<(حجم(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        أرجع nullptr;
    }

    أرجع udata_openSwapper(inIsBigEndian, inCharset, outIsBigEndian, outCharset, pErrorCode);
}

U_CAPI فراغ U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
