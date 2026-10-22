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

#समावेश <stdarg.h>
#समावेश "unicode/utypes.h"
#समावेश "unicode/udata.h" /* UDataInfo */
#समावेश "ucmndata.h" /* DataHeader */
#समावेश "cmemory.h"
#समावेश "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

अटल int32_t U_CALLCONV
uprv_swapArray16(स्थिर UDataSwapper *ds,
                 स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                 UErrorCode *pErrorCode) {
    स्थिर uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(ds==nullptr || in_तथ्य==nullptr || length<0 || (length&1)!=0 || out_तथ्य==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* setup and swapping */
    p = static_cast<स्थिर uint16_t*>(in_तथ्य);
    q = static_cast<uint16_t*>(out_तथ्य);
    count=length/2;
    जबतक(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    वापस length;
}

अटल int32_t U_CALLCONV
uprv_copyArray16(स्थिर UDataSwapper *ds,
                 स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                 UErrorCode *pErrorCode) {
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(ds==nullptr || in_तथ्य==nullptr || length<0 || (length&1)!=0 || out_तथ्य==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    अगर(length>0 && in_तथ्य!=out_तथ्य) {
        uprv_memcpy(out_तथ्य, in_तथ्य, length);
    }
    वापस length;
}

अटल int32_t U_CALLCONV
uprv_swapArray32(स्थिर UDataSwapper *ds,
                 स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                 UErrorCode *pErrorCode) {
    स्थिर uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(ds==nullptr || in_तथ्य==nullptr || length<0 || (length&3)!=0 || out_तथ्य==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* setup and swapping */
    p = static_cast<स्थिर uint32_t*>(in_तथ्य);
    q = static_cast<uint32_t*>(out_तथ्य);
    count=length/4;
    जबतक(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    वापस length;
}

अटल int32_t U_CALLCONV
uprv_copyArray32(स्थिर UDataSwapper *ds,
                 स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                 UErrorCode *pErrorCode) {
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(ds==nullptr || in_तथ्य==nullptr || length<0 || (length&3)!=0 || out_तथ्य==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    अगर(length>0 && in_तथ्य!=out_तथ्य) {
        uprv_memcpy(out_तथ्य, in_तथ्य, length);
    }
    वापस length;
}

अटल int32_t U_CALLCONV
uprv_swapArray64(स्थिर UDataSwapper *ds,
                 स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                 UErrorCode *pErrorCode) {
    स्थिर uint64_t *p;
    uint64_t *q;
    int32_t count;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(ds==nullptr || in_तथ्य==nullptr || length<0 || (length&7)!=0 || out_तथ्य==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* setup and swapping */
    p = static_cast<स्थिर uint64_t*>(in_तथ्य);
    q = static_cast<uint64_t*>(out_तथ्य);
    count=length/8;
    जबतक(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    वापस length;
}

अटल int32_t U_CALLCONV
uprv_copyArray64(स्थिर UDataSwapper *ds,
                 स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                 UErrorCode *pErrorCode) {
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(ds==nullptr || in_तथ्य==nullptr || length<0 || (length&7)!=0 || out_तथ्य==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    अगर(length>0 && in_तथ्य!=out_तथ्य) {
        uprv_memcpy(out_तथ्य, in_तथ्य, length);
    }
    वापस length;
}

अटल uint16_t U_CALLCONV
uprv_readSwapUInt16(uint16_t x) {
    वापस static_cast<uint16_t>((x << 8) | (x >> 8));
}

अटल uint16_t U_CALLCONV
uprv_readDirectUInt16(uint16_t x) {
    वापस x;
}

अटल uint32_t U_CALLCONV
uprv_readSwapUInt32(uint32_t x) {
    वापस (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

अटल uint32_t U_CALLCONV
uprv_readDirectUInt32(uint32_t x) {
    वापस x;
}

अटल व्योम U_CALLCONV
uprv_writeSwapUInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

अटल व्योम U_CALLCONV
uprv_writeDirectUInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

अटल व्योम U_CALLCONV
uprv_writeSwapUInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

अटल व्योम U_CALLCONV
uprv_writeDirectUInt32(uint32_t *p, uint32_t x) {
    *p=x;
}

U_CAPI int16_t U_EXPORT2
udata_readInt16(स्थिर UDataSwapper *ds, int16_t x) {
    वापस (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(स्थिर UDataSwapper *ds, int32_t x) {
    वापस (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_swapInvStringBlock(स्थिर UDataSwapper *ds,
                         स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                         UErrorCode *pErrorCode) {
    स्थिर अक्षर *inChars;
    int32_t strings_लंबाई;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(ds==nullptr || in_तथ्य==nullptr || length<0 || (length>0 && out_तथ्य==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    inChars=(स्थिर अक्षर *)in_तथ्य;
    strings_लंबाई=length;
    जबतक(strings_लंबाई>0 && inChars[strings_लंबाई-1]!=0) {
        --strings_लंबाई;
    }

    /* swap up to the last NUL */
    ds->swapInvChars(ds, in_तथ्य, strings_लंबाई, out_तथ्य, pErrorCode);

    /* copy the bytes after the last NUL */
    अगर(in_तथ्य!=out_तथ्य && length>strings_लंबाई) {
        uprv_memcpy((अक्षर *)out_तथ्य+strings_लंबाई, inChars+strings_लंबाई, length-strings_लंबाई);
    }

    /* return the length including padding bytes */
    अगर(U_SUCCESS(*pErrorCode)) {
        वापस length;
    } अन्यथा {
        वापस 0;
    }
}

U_CAPI व्योम U_EXPORT2
udata_printError(स्थिर UDataSwapper *ds,
                 स्थिर अक्षर *fmt,
                 ...) {
    बहु_सूची args;

    अगर(ds->printError!=nullptr) {
        बहु_शुरू(args, fmt);
        ds->printError(ds->printErrorContext, fmt, args);
        बहु_पूर्ण(args);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_swap_तथ्य_Header(स्थिर UDataSwapper *ds,
                     स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                     UErrorCode *pErrorCode) {
    स्थिर तथ्य_Header *pHeader;
    uint16_t header_आकार, info_आकार;

    /* argument checking */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(ds==nullptr || in_तथ्य==nullptr || length<-1 || (length>0 && out_तथ्य==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* check minimum length and magic bytes */
    pHeader=(स्थिर तथ्य_Header *)in_तथ्य;
    अगर( (length>=0 && length<(int32_t)माप(तथ्य_Header)) ||
        pHeader->तथ्य_Header_2.magic1!=0xda ||
        pHeader->तथ्य_Header_2.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    header_आकार=ds->readUInt16(pHeader->तथ्य_Header_2.header_आकार);
    info_आकार=ds->readUInt16(pHeader->info.size);

    अगर( header_आकार<माप(तथ्य_Header) ||
        info_आकार<माप(UDataInfo) ||
        header_आकार<(माप(pHeader->तथ्य_Header_2)+info_आकार) ||
        (length>=0 && length<header_आकार)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         header_आकार, info_आकार, length);
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    अगर(length>0) {
        तथ्य_Header *outHeader;
        स्थिर अक्षर *s;
        int32_t max_लंबाई_2;

        /* Most of the fields are just bytes and need no swapping. */
        अगर(in_तथ्य!=out_तथ्य) {
            uprv_memcpy(out_तथ्य, in_तथ्य, header_आकार);
        }
        outHeader=(तथ्य_Header *)out_तथ्य;

        outHeader->info.isBigEndian = ds->outIsBigEndian;
        outHeader->info.charsetFamily = ds->outCharset;

        /* swap headerSize */
        ds->swapArray16(ds, &pHeader->तथ्य_Header_2.header_आकार, 2, &outHeader->तथ्य_Header_2.header_आकार, pErrorCode);

        /* swap UDataInfo size and reservedWord */
        ds->swapArray16(ds, &pHeader->info.size, 4, &outHeader->info.size, pErrorCode);

        /* swap copyright statement after the UDataInfo */
        info_आकार+=माप(pHeader->तथ्य_Header_2);
        s=(स्थिर अक्षर *)in_तथ्य+info_आकार;
        max_लंबाई_2=header_आकार-info_आकार;
        /* get the length of the string */
        क्रम(length=0; length<max_लंबाई_2 && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->swapInvChars(ds, s, length, (अक्षर *)out_तथ्य+info_आकार, pErrorCode);
    }

    वापस header_आकार;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool inIsBigEndian, uint8_t inCharset,
                  UBool outIsBigEndian, uint8_t outCharset,
                  UErrorCode *pErrorCode) {
    UDataSwapper *swapper;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    अगर(inCharset>U_EBCDIC_FAMILY || outCharset>U_EBCDIC_FAMILY) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(माप(UDataSwapper));
    अगर(swapper==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    uprv_memset(swapper, 0, माप(UDataSwapper));

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

    अगर(inIsBigEndian==outIsBigEndian) {
        swapper->swapArray16=uprv_copyArray16;
        swapper->swapArray32=uprv_copyArray32;
        swapper->swapArray64=uprv_copyArray64;
    } अन्यथा {
        swapper->swapArray16=uprv_swapArray16;
        swapper->swapArray32=uprv_swapArray32;
        swapper->swapArray64=uprv_swapArray64;
    }

    अगर(inCharset==U_ASCII_FAMILY) {
        swapper->swapInvChars= outCharset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } अन्यथा /* U_EBCDIC_FAMILY */ {
        swapper->swapInvChars= outCharset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    वापस swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_निवेश_तथ्य(स्थिर व्योम *तथ्य, int32_t length,
                              UBool outIsBigEndian, uint8_t outCharset,
                              UErrorCode *pErrorCode) {
    स्थिर तथ्य_Header *pHeader;
    uint16_t header_आकार, info_आकार;
    UBool inIsBigEndian;
    int8_t inCharset;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    अगर( तथ्य==nullptr ||
        (length>=0 && length<(int32_t)माप(तथ्य_Header)) ||
        outCharset>U_EBCDIC_FAMILY
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    pHeader=(स्थिर तथ्य_Header *)तथ्य;
    अगर( (length>=0 && length<(int32_t)माप(तथ्य_Header)) ||
        pHeader->तथ्य_Header_2.magic1!=0xda ||
        pHeader->तथ्य_Header_2.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस nullptr;
    }

    inIsBigEndian=(UBool)pHeader->info.isBigEndian;
    inCharset=pHeader->info.charsetFamily;

    अगर(inIsBigEndian==U_IS_BIG_ENDIAN) {
        header_आकार=pHeader->तथ्य_Header_2.header_आकार;
        info_आकार=pHeader->info.size;
    } अन्यथा {
        header_आकार=uprv_readSwapUInt16(pHeader->तथ्य_Header_2.header_आकार);
        info_आकार=uprv_readSwapUInt16(pHeader->info.size);
    }

    अगर( header_आकार<माप(तथ्य_Header) ||
        info_आकार<माप(UDataInfo) ||
        header_आकार<(माप(pHeader->तथ्य_Header_2)+info_आकार) ||
        (length>=0 && length<header_आकार)
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस nullptr;
    }

    वापस udata_openSwapper(inIsBigEndian, inCharset, outIsBigEndian, outCharset, pErrorCode);
}

U_CAPI व्योम U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
