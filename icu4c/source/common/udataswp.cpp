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

#অন্তর্ভাব <stdarg.h>
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/udata.h" /* UDataInfo */
#অন্তর্ভাব "ucmndata.h" /* DataHeader */
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

স্থির int32_t U_CALLCONV
uprv_swapArray16(ধ্রুবক UDataSwapper *ds,
                 ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
                 UErrorCode *pErrorCode) {
    ধ্রুবক uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(ds==nullptr || in_তথ্য==nullptr || length<0 || (length&1)!=0 || out_তথ্য==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* setup and swapping */
    p = static_cast<ধ্রুবক uint16_t*>(in_তথ্য);
    q = static_cast<uint16_t*>(out_তথ্য);
    count=length/2;
    যখন(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    ফেরত length;
}

স্থির int32_t U_CALLCONV
uprv_copyArray16(ধ্রুবক UDataSwapper *ds,
                 ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
                 UErrorCode *pErrorCode) {
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(ds==nullptr || in_তথ্য==nullptr || length<0 || (length&1)!=0 || out_তথ্য==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    যদি(length>0 && in_তথ্য!=out_তথ্য) {
        uprv_memcpy(out_তথ্য, in_তথ্য, length);
    }
    ফেরত length;
}

স্থির int32_t U_CALLCONV
uprv_swapArray32(ধ্রুবক UDataSwapper *ds,
                 ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
                 UErrorCode *pErrorCode) {
    ধ্রুবক uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(ds==nullptr || in_তথ্য==nullptr || length<0 || (length&3)!=0 || out_তথ্য==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* setup and swapping */
    p = static_cast<ধ্রুবক uint32_t*>(in_তথ্য);
    q = static_cast<uint32_t*>(out_তথ্য);
    count=length/4;
    যখন(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    ফেরত length;
}

স্থির int32_t U_CALLCONV
uprv_copyArray32(ধ্রুবক UDataSwapper *ds,
                 ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
                 UErrorCode *pErrorCode) {
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(ds==nullptr || in_তথ্য==nullptr || length<0 || (length&3)!=0 || out_তথ্য==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    যদি(length>0 && in_তথ্য!=out_তথ্য) {
        uprv_memcpy(out_তথ্য, in_তথ্য, length);
    }
    ফেরত length;
}

স্থির int32_t U_CALLCONV
uprv_swapArray64(ধ্রুবক UDataSwapper *ds,
                 ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
                 UErrorCode *pErrorCode) {
    ধ্রুবক uint64_t *p;
    uint64_t *q;
    int32_t count;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(ds==nullptr || in_তথ্য==nullptr || length<0 || (length&7)!=0 || out_তথ্য==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* setup and swapping */
    p = static_cast<ধ্রুবক uint64_t*>(in_তথ্য);
    q = static_cast<uint64_t*>(out_তথ্য);
    count=length/8;
    যখন(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    ফেরত length;
}

স্থির int32_t U_CALLCONV
uprv_copyArray64(ধ্রুবক UDataSwapper *ds,
                 ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
                 UErrorCode *pErrorCode) {
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(ds==nullptr || in_তথ্য==nullptr || length<0 || (length&7)!=0 || out_তথ্য==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    যদি(length>0 && in_তথ্য!=out_তথ্য) {
        uprv_memcpy(out_তথ্য, in_তথ্য, length);
    }
    ফেরত length;
}

স্থির uint16_t U_CALLCONV
uprv_readSwapUInt16(uint16_t x) {
    ফেরত static_cast<uint16_t>((x << 8) | (x >> 8));
}

স্থির uint16_t U_CALLCONV
uprv_readDirectUInt16(uint16_t x) {
    ফেরত x;
}

স্থির uint32_t U_CALLCONV
uprv_readSwapUInt32(uint32_t x) {
    ফেরত (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

স্থির uint32_t U_CALLCONV
uprv_readDirectUInt32(uint32_t x) {
    ফেরত x;
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
udata_readInt16(ধ্রুবক UDataSwapper *ds, int16_t x) {
    ফেরত (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(ধ্রুবক UDataSwapper *ds, int32_t x) {
    ফেরত (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_swapInvStringBlock(ধ্রুবক UDataSwapper *ds,
                         ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
                         UErrorCode *pErrorCode) {
    ধ্রুবক অক্ষর *inChars;
    int32_t strings_দৈর্ঘ্য;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(ds==nullptr || in_তথ্য==nullptr || length<0 || (length>0 && out_তথ্য==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    inChars=(ধ্রুবক অক্ষর *)in_তথ্য;
    strings_দৈর্ঘ্য=length;
    যখন(strings_দৈর্ঘ্য>0 && inChars[strings_দৈর্ঘ্য-1]!=0) {
        --strings_দৈর্ঘ্য;
    }

    /* swap up to the last NUL */
    ds->swapInvChars(ds, in_তথ্য, strings_দৈর্ঘ্য, out_তথ্য, pErrorCode);

    /* copy the bytes after the last NUL */
    যদি(in_তথ্য!=out_তথ্য && length>strings_দৈর্ঘ্য) {
        uprv_memcpy((অক্ষর *)out_তথ্য+strings_দৈর্ঘ্য, inChars+strings_দৈর্ঘ্য, length-strings_দৈর্ঘ্য);
    }

    /* return the length including padding bytes */
    যদি(U_SUCCESS(*pErrorCode)) {
        ফেরত length;
    } অন্যথা {
        ফেরত 0;
    }
}

U_CAPI শূন্য U_EXPORT2
udata_printError(ধ্রুবক UDataSwapper *ds,
                 ধ্রুবক অক্ষর *fmt,
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
udata_swap_তথ্য_Header(ধ্রুবক UDataSwapper *ds,
                     ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
                     UErrorCode *pErrorCode) {
    ধ্রুবক তথ্য_Header *pHeader;
    uint16_t header_আকার, info_আকার;

    /* argument checking */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(ds==nullptr || in_তথ্য==nullptr || length<-1 || (length>0 && out_তথ্য==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* check minimum length and magic bytes */
    pHeader=(ধ্রুবক তথ্য_Header *)in_তথ্য;
    যদি( (length>=0 && length<(int32_t)আকার(তথ্য_Header)) ||
        pHeader->তথ্য_Header_2.magic1!=0xda ||
        pHeader->তথ্য_Header_2.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    header_আকার=ds->readUInt16(pHeader->তথ্য_Header_2.header_আকার);
    info_আকার=ds->readUInt16(pHeader->info.size);

    যদি( header_আকার<আকার(তথ্য_Header) ||
        info_আকার<আকার(UDataInfo) ||
        header_আকার<(আকার(pHeader->তথ্য_Header_2)+info_আকার) ||
        (length>=0 && length<header_আকার)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         header_আকার, info_আকার, length);
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    যদি(length>0) {
        তথ্য_Header *outHeader;
        ধ্রুবক অক্ষর *s;
        int32_t max_দৈর্ঘ্য_2;

        /* Most of the fields are just bytes and need no swapping. */
        যদি(in_তথ্য!=out_তথ্য) {
            uprv_memcpy(out_তথ্য, in_তথ্য, header_আকার);
        }
        outHeader=(তথ্য_Header *)out_তথ্য;

        outHeader->info.isBigEndian = ds->outIsBigEndian;
        outHeader->info.charsetFamily = ds->outCharset;

        /* swap headerSize */
        ds->swapArray16(ds, &pHeader->তথ্য_Header_2.header_আকার, 2, &outHeader->তথ্য_Header_2.header_আকার, pErrorCode);

        /* swap UDataInfo size and reservedWord */
        ds->swapArray16(ds, &pHeader->info.size, 4, &outHeader->info.size, pErrorCode);

        /* swap copyright statement after the UDataInfo */
        info_আকার+=আকার(pHeader->তথ্য_Header_2);
        s=(ধ্রুবক অক্ষর *)in_তথ্য+info_আকার;
        max_দৈর্ঘ্য_2=header_আকার-info_আকার;
        /* get the length of the string */
        জন্য(length=0; length<max_দৈর্ঘ্য_2 && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->swapInvChars(ds, s, length, (অক্ষর *)out_তথ্য+info_আকার, pErrorCode);
    }

    ফেরত header_আকার;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool inIsBigEndian, uint8_t inCharset,
                  UBool outIsBigEndian, uint8_t outCharset,
                  UErrorCode *pErrorCode) {
    UDataSwapper *swapper;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি(inCharset>U_EBCDIC_FAMILY || outCharset>U_EBCDIC_FAMILY) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(আকার(UDataSwapper));
    যদি(swapper==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    uprv_memset(swapper, 0, আকার(UDataSwapper));

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

    ফেরত swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_প্রবেশ_তথ্য(ধ্রুবক শূন্য *তথ্য, int32_t length,
                              UBool outIsBigEndian, uint8_t outCharset,
                              UErrorCode *pErrorCode) {
    ধ্রুবক তথ্য_Header *pHeader;
    uint16_t header_আকার, info_আকার;
    UBool inIsBigEndian;
    int8_t inCharset;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি( তথ্য==nullptr ||
        (length>=0 && length<(int32_t)আকার(তথ্য_Header)) ||
        outCharset>U_EBCDIC_FAMILY
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    pHeader=(ধ্রুবক তথ্য_Header *)তথ্য;
    যদি( (length>=0 && length<(int32_t)আকার(তথ্য_Header)) ||
        pHeader->তথ্য_Header_2.magic1!=0xda ||
        pHeader->তথ্য_Header_2.magic2!=0x27 ||
        pHeader->info.sizeofUChar!=2
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ফেরত nullptr;
    }

    inIsBigEndian=(UBool)pHeader->info.isBigEndian;
    inCharset=pHeader->info.charsetFamily;

    যদি(inIsBigEndian==U_IS_BIG_ENDIAN) {
        header_আকার=pHeader->তথ্য_Header_2.header_আকার;
        info_আকার=pHeader->info.size;
    } অন্যথা {
        header_আকার=uprv_readSwapUInt16(pHeader->তথ্য_Header_2.header_আকার);
        info_আকার=uprv_readSwapUInt16(pHeader->info.size);
    }

    যদি( header_আকার<আকার(তথ্য_Header) ||
        info_আকার<আকার(UDataInfo) ||
        header_আকার<(আকার(pHeader->তথ্য_Header_2)+info_আকার) ||
        (length>=0 && length<header_আকার)
    ) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ফেরত nullptr;
    }

    ফেরত udata_openSwapper(inIsBigEndian, inCharset, outIsBigEndian, outCharset, pErrorCode);
}

U_CAPI শূন্য U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
