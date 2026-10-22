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

#অন্তর্ভাৱ <stdarg.h>
#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/udata.h" /* UDataInfo */
#অন্তর্ভাৱ "ucmndata.h" /* DataHeader */
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

স্থির int32_t U_CALLCONV
uprv_swapArray16(ধ্রুৱ UDataSwapper *ds,
                 ধ্রুৱ শূন্য *inData, int32_t length, শূন্য *outData,
                 UErrorCode *pErrorCode) {
    ধ্রুৱ uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }
    যদি(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    /* setup and swapping */
    p = static_cast<ধ্রুৱ uint16_t*>(inData);
    q = static_cast<uint16_t*>(outData);
    count=length/2;
    যেতিয়ালৈ(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    ঘূৰাই length;
}

স্থির int32_t U_CALLCONV
uprv_copyArray16(ধ্রুৱ UDataSwapper *ds,
                 ধ্রুৱ শূন্য *inData, int32_t length, শূন্য *outData,
                 UErrorCode *pErrorCode) {
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }
    যদি(ds==nullptr || inData==nullptr || length<0 || (length&1)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    যদি(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    ঘূৰাই length;
}

স্থির int32_t U_CALLCONV
uprv_swapArray32(ধ্রুৱ UDataSwapper *ds,
                 ধ্রুৱ শূন্য *inData, int32_t length, শূন্য *outData,
                 UErrorCode *pErrorCode) {
    ধ্রুৱ uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }
    যদি(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    /* setup and swapping */
    p = static_cast<ধ্রুৱ uint32_t*>(inData);
    q = static_cast<uint32_t*>(outData);
    count=length/4;
    যেতিয়ালৈ(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    ঘূৰাই length;
}

স্থির int32_t U_CALLCONV
uprv_copyArray32(ধ্রুৱ UDataSwapper *ds,
                 ধ্রুৱ শূন্য *inData, int32_t length, শূন্য *outData,
                 UErrorCode *pErrorCode) {
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }
    যদি(ds==nullptr || inData==nullptr || length<0 || (length&3)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    যদি(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    ঘূৰাই length;
}

স্থির int32_t U_CALLCONV
uprv_swapArray64(ধ্রুৱ UDataSwapper *ds,
                 ধ্রুৱ শূন্য *inData, int32_t length, শূন্য *outData,
                 UErrorCode *pErrorCode) {
    ধ্রুৱ uint64_t *p;
    uint64_t *q;
    int32_t count;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }
    যদি(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    /* setup and swapping */
    p = static_cast<ধ্রুৱ uint64_t*>(inData);
    q = static_cast<uint64_t*>(outData);
    count=length/8;
    যেতিয়ালৈ(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    ঘূৰাই length;
}

স্থির int32_t U_CALLCONV
uprv_copyArray64(ধ্রুৱ UDataSwapper *ds,
                 ধ্রুৱ শূন্য *inData, int32_t length, শূন্য *outData,
                 UErrorCode *pErrorCode) {
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }
    যদি(ds==nullptr || inData==nullptr || length<0 || (length&7)!=0 || outData==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    যদি(length>0 && inData!=outData) {
        uprv_memcpy(outData, inData, length);
    }
    ঘূৰাই length;
}

স্থির uint16_t U_CALLCONV
uprv_readSwapUInt16(uint16_t x) {
    ঘূৰাই static_cast<uint16_t>((x << 8) | (x >> 8));
}

স্থির uint16_t U_CALLCONV
uprv_readDirectUInt16(uint16_t x) {
    ঘূৰাই x;
}

স্থির uint32_t U_CALLCONV
uprv_readSwapUInt32(uint32_t x) {
    ঘূৰাই (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

স্থির uint32_t U_CALLCONV
uprv_readDirectUInt32(uint32_t x) {
    ঘূৰাই x;
}

স্থির শূন্য U_CALLCONV
uprv_writeSwapUInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

স্থির শূন্য U_CALLCONV
uprv_writeDirectUInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

স্থির শূন্য U_CALLCONV
uprv_writeSwapUInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

স্থির শূন্য U_CALLCONV
uprv_writeDirectUInt32(uint32_t *p, uint32_t x) {
    *p=x;
}

U_CAPI int16_t U_EXPORT2
udata_readInt16(ধ্রুৱ UDataSwapper *ds, int16_t x) {
    ঘূৰাই (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(ধ্রুৱ UDataSwapper *ds, int32_t x) {
    ঘূৰাই (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_swapInvStringBlock(ধ্রুৱ UDataSwapper *ds,
                         ধ্রুৱ শূন্য *inData, int32_t length, শূন্য *outData,
                         UErrorCode *pErrorCode) {
    ধ্রুৱ ৱর্ণ *inChars;
    int32_t stringsLength;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }
    যদি(ds==nullptr || inData==nullptr || length<0 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    inChars=(ধ্রুৱ ৱর্ণ *)inData;
    stringsLength=length;
    যেতিয়ালৈ(stringsLength>0 && inChars[stringsLength-1]!=0) {
        --stringsLength;
    }

    /* swap up to the last NUL */
    ds->swapInvChars(ds, inData, stringsLength, outData, pErrorCode);

    /* copy the bytes after the last NUL */
    যদি(inData!=outData && length>stringsLength) {
        uprv_memcpy((ৱর্ণ *)outData+stringsLength, inChars+stringsLength, length-stringsLength);
    }

    /* return the length including padding bytes */
    যদি(U_SUCCESS(*pErrorCode)) {
        ঘূৰাই length;
    } অন্যথা {
        ঘূৰাই 0;
    }
}

U_CAPI শূন্য U_EXPORT2
udata_printError(ধ্রুৱ UDataSwapper *ds,
                 ধ্রুৱ ৱর্ণ *fmt,
                 ...) {
    va_list args;

    যদি(ds->printError!=nullptr) {
        va_start(args, fmt);
        ds->printError(ds->printErrorContext, fmt, args);
        va_end(args);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_swapDataHeader(ধ্রুৱ UDataSwapper *ds,
                     ধ্রুৱ শূন্য *inData, int32_t length, শূন্য *outData,
                     UErrorCode *pErrorCode) {
    ধ্রুৱ DataHeader *pHeader;
    uint16_t headerSize, infoSize;

    /* argument checking */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }
    যদি(ds==nullptr || inData==nullptr || length<-1 || (length>0 && outData==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    /* check minimum length and magic bytes */
    pHeader=(ধ্রুৱ DataHeader *)inData;
    যদি( (length>=0 && length<(int32_t)পরিমাণ(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ঘূৰাই 0;
    }

    headerSize=ds->readUInt16(pHeader->dataHeader.headerSize);
    infoSize=ds->readUInt16(pHeader->info.size);

    যদি( headerSize<পরিমাণ(DataHeader) ||
        infoSize<পরিমাণ(UDataInfo) ||
        headerSize<(পরিমাণ(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         headerSize, infoSize, length);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ঘূৰাই 0;
    }

    যদি(length>0) {
        DataHeader *outHeader;
        ধ্রুৱ ৱর্ণ *s;
        int32_t maxLength;

        /* Most of the fields are just bytes and need no swapping. */
        যদি(inData!=outData) {
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
        infoSize+=পরিমাণ(pHeader->dataHeader);
        s=(ধ্রুৱ ৱর্ণ *)inData+infoSize;
        maxLength=headerSize-infoSize;
        /* get the length of the string */
        বাবে(length=0; length<maxLength && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->swapInvChars(ds, s, length, (ৱর্ণ *)outData+infoSize, pErrorCode);
    }

    ঘূৰাই headerSize;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool inIsBigEndian, uint8_t inCharset,
                  UBool outIsBigEndian, uint8_t outCharset,
                  UErrorCode *pErrorCode) {
    UDataSwapper *swapper;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই nullptr;
    }
    যদি(inCharset>U_EBCDIC_FAMILY || outCharset>U_EBCDIC_FAMILY) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(পরিমাণ(UDataSwapper));
    যদি(swapper==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }
    uprv_memset(swapper, 0, পরিমাণ(UDataSwapper));

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

    যদি(inIsBigEndian==outIsBigEndian) {
        swapper->swapArray16=uprv_copyArray16;
        swapper->swapArray32=uprv_copyArray32;
        swapper->swapArray64=uprv_copyArray64;
    } অন্যথা {
        swapper->swapArray16=uprv_swapArray16;
        swapper->swapArray32=uprv_swapArray32;
        swapper->swapArray64=uprv_swapArray64;
    }

    যদি(inCharset==U_ASCII_FAMILY) {
        swapper->swapInvChars= outCharset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } অন্যথা /* U_EBCDIC_FAMILY */ {
        swapper->swapInvChars= outCharset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    ঘূৰাই swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_প্ৰবেশ_Data(ধ্রুৱ শূন্য *data, int32_t length,
                              UBool outIsBigEndian, uint8_t outCharset,
                              UErrorCode *pErrorCode) {
    ধ্রুৱ DataHeader *pHeader;
    uint16_t headerSize, infoSize;
    UBool inIsBigEndian;
    int8_t inCharset;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই nullptr;
    }
    যদি( data==nullptr ||
        (length>=0 && length<(int32_t)পরিমাণ(DataHeader)) ||
        outCharset>U_EBCDIC_FAMILY
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই nullptr;
    }

    pHeader=(ধ্রুৱ DataHeader *)data;
    যদি( (length>=0 && length<(int32_t)পরিমাণ(DataHeader)) ||
        pHeader->dataHeader.magic1!=0xda ||
        pHeader->dataHeader.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ঘূৰাই nullptr;
    }

    inIsBigEndian=(UBool)pHeader->info.isBigEndian;
    inCharset=pHeader->info.charsetFamily;

    যদি(inIsBigEndian==U_IS_BIG_ENDIAN) {
        headerSize=pHeader->dataHeader.headerSize;
        infoSize=pHeader->info.size;
    } অন্যথা {
        headerSize=uprv_readSwapUInt16(pHeader->dataHeader.headerSize);
        infoSize=uprv_readSwapUInt16(pHeader->info.size);
    }

    যদি( headerSize<পরিমাণ(DataHeader) ||
        infoSize<পরিমাণ(UDataInfo) ||
        headerSize<(পরিমাণ(pHeader->dataHeader)+infoSize) ||
        (length>=0 && length<headerSize)
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ঘূৰাই nullptr;
    }

    ঘূৰাই udata_openSwapper(inIsBigEndian, inCharset, outIsBigEndian, outCharset, pErrorCode);
}

U_CAPI শূন্য U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
