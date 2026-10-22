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

#अन्तर्भाव <stdarg.h>
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/udata.h" /* UDataInfo */
#अन्तर्भाव "ucmndata.h" /* DataHeader */
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

स्थिर int32_t U_CALLCONV
uprv_swapArray16(ध्रुव UDataSwapper *ds,
                 ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                 UError_संहिता *p_Error_संहिता) {
    ध्रुव uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || length<0 || (length&1)!=0 || बहिर्गमनम्_दत्त==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* setup and swapping */
    p = static_cast<ध्रुव uint16_t*>(अन्तर्_दत्त);
    q = static_cast<uint16_t*>(बहिर्गमनम्_दत्त);
    count=length/2;
    यावत्(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    प्रत्यागम length;
}

स्थिर int32_t U_CALLCONV
uprv_copyArray16(ध्रुव UDataSwapper *ds,
                 ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                 UError_संहिता *p_Error_संहिता) {
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || length<0 || (length&1)!=0 || बहिर्गमनम्_दत्त==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    यदि(length>0 && अन्तर्_दत्त!=बहिर्गमनम्_दत्त) {
        uprv_memcpy(बहिर्गमनम्_दत्त, अन्तर्_दत्त, length);
    }
    प्रत्यागम length;
}

स्थिर int32_t U_CALLCONV
uprv_swapArray32(ध्रुव UDataSwapper *ds,
                 ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                 UError_संहिता *p_Error_संहिता) {
    ध्रुव uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || length<0 || (length&3)!=0 || बहिर्गमनम्_दत्त==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* setup and swapping */
    p = static_cast<ध्रुव uint32_t*>(अन्तर्_दत्त);
    q = static_cast<uint32_t*>(बहिर्गमनम्_दत्त);
    count=length/4;
    यावत्(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    प्रत्यागम length;
}

स्थिर int32_t U_CALLCONV
uprv_copyArray32(ध्रुव UDataSwapper *ds,
                 ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                 UError_संहिता *p_Error_संहिता) {
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || length<0 || (length&3)!=0 || बहिर्गमनम्_दत्त==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    यदि(length>0 && अन्तर्_दत्त!=बहिर्गमनम्_दत्त) {
        uprv_memcpy(बहिर्गमनम्_दत्त, अन्तर्_दत्त, length);
    }
    प्रत्यागम length;
}

स्थिर int32_t U_CALLCONV
uprv_swapArray64(ध्रुव UDataSwapper *ds,
                 ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                 UError_संहिता *p_Error_संहिता) {
    ध्रुव uint64_t *p;
    uint64_t *q;
    int32_t count;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || length<0 || (length&7)!=0 || बहिर्गमनम्_दत्त==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* setup and swapping */
    p = static_cast<ध्रुव uint64_t*>(अन्तर्_दत्त);
    q = static_cast<uint64_t*>(बहिर्गमनम्_दत्त);
    count=length/8;
    यावत्(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    प्रत्यागम length;
}

स्थिर int32_t U_CALLCONV
uprv_copyArray64(ध्रुव UDataSwapper *ds,
                 ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                 UError_संहिता *p_Error_संहिता) {
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || length<0 || (length&7)!=0 || बहिर्गमनम्_दत्त==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    यदि(length>0 && अन्तर्_दत्त!=बहिर्गमनम्_दत्त) {
        uprv_memcpy(बहिर्गमनम्_दत्त, अन्तर्_दत्त, length);
    }
    प्रत्यागम length;
}

स्थिर uint16_t U_CALLCONV
uprv_readSwapUInt16(uint16_t x) {
    प्रत्यागम static_cast<uint16_t>((x << 8) | (x >> 8));
}

स्थिर uint16_t U_CALLCONV
uprv_read_प्रत्यक्षम्_UInt16(uint16_t x) {
    प्रत्यागम x;
}

स्थिर uint32_t U_CALLCONV
uprv_readSwapUInt32(uint32_t x) {
    प्रत्यागम (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

स्थिर uint32_t U_CALLCONV
uprv_read_प्रत्यक्षम्_UInt32(uint32_t x) {
    प्रत्यागम x;
}

स्थिर शून्य U_CALLCONV
uprv_writeSwapUInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

स्थिर शून्य U_CALLCONV
uprv_write_प्रत्यक्षम्_UInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

स्थिर शून्य U_CALLCONV
uprv_writeSwapUInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

स्थिर शून्य U_CALLCONV
uprv_write_प्रत्यक्षम्_UInt32(uint32_t *p, uint32_t x) {
    *p=x;
}

U_CAPI int16_t U_EXPORT2
udata_readInt16(ध्रुव UDataSwapper *ds, int16_t x) {
    प्रत्यागम (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(ध्रुव UDataSwapper *ds, int32_t x) {
    प्रत्यागम (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_swap_Inv_String_खण्डम्(ध्रुव UDataSwapper *ds,
                         ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                         UError_संहिता *p_Error_संहिता) {
    ध्रुव वर्ण *अन्तर्_Chars;
    int32_t वाक्यानि_दीर्घता;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || length<0 || (length>0 && बहिर्गमनम्_दत्त==शून्यसूचकः)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    अन्तर्_Chars=(ध्रुव वर्ण *)अन्तर्_दत्त;
    वाक्यानि_दीर्घता=length;
    यावत्(वाक्यानि_दीर्घता>0 && अन्तर्_Chars[वाक्यानि_दीर्घता-1]!=0) {
        --वाक्यानि_दीर्घता;
    }

    /* swap up to the last NUL */
    ds->swapInvChars(ds, अन्तर्_दत्त, वाक्यानि_दीर्घता, बहिर्गमनम्_दत्त, p_Error_संहिता);

    /* copy the bytes after the last NUL */
    यदि(अन्तर्_दत्त!=बहिर्गमनम्_दत्त && length>वाक्यानि_दीर्घता) {
        uprv_memcpy((वर्ण *)बहिर्गमनम्_दत्त+वाक्यानि_दीर्घता, अन्तर्_Chars+वाक्यानि_दीर्घता, length-वाक्यानि_दीर्घता);
    }

    /* return the length including padding bytes */
    यदि(U_SUCCESS(*p_Error_संहिता)) {
        प्रत्यागम length;
    } अन्यथा {
        प्रत्यागम 0;
    }
}

U_CAPI शून्य U_EXPORT2
udata_printError(ध्रुव UDataSwapper *ds,
                 ध्रुव वर्ण *fmt,
                 ...) {
    va_list तर्काः_2;

    यदि(ds->printError!=शून्यसूचकः) {
        va_start(तर्काः_2, fmt);
        ds->printError(ds->printErrorContext, fmt, तर्काः_2);
        va_end(तर्काः_2);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_swap_दत्त_शीर्षकम्(ध्रुव UDataSwapper *ds,
                     ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                     UError_संहिता *p_Error_संहिता) {
    ध्रुव दत्त_शीर्षकम् *p_शीर्षकम्;
    uint16_t शीर्षकम्_आकार, info_आकार;

    /* argument checking */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(ds==शून्यसूचकः || अन्तर्_दत्त==शून्यसूचकः || length<-1 || (length>0 && बहिर्गमनम्_दत्त==शून्यसूचकः)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* check minimum length and magic bytes */
    p_शीर्षकम्=(ध्रुव दत्त_शीर्षकम् *)अन्तर्_दत्त;
    यदि( (length>=0 && length<(int32_t)परिमाण(दत्त_शीर्षकम्)) ||
        p_शीर्षकम्->दत्त_शीर्षकम्_2.magic1!=0xda ||
        p_शीर्षकम्->दत्त_शीर्षकम्_2.magic2!=0x27 ||
        p_शीर्षकम्->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    शीर्षकम्_आकार=ds->readUInt16(p_शीर्षकम्->दत्त_शीर्षकम्_2.शीर्षकम्_आकार);
    info_आकार=ds->readUInt16(p_शीर्षकम्->info.size);

    यदि( शीर्षकम्_आकार<परिमाण(दत्त_शीर्षकम्) ||
        info_आकार<परिमाण(UDataInfo) ||
        शीर्षकम्_आकार<(परिमाण(p_शीर्षकम्->दत्त_शीर्षकम्_2)+info_आकार) ||
        (length>=0 && length<शीर्षकम्_आकार)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         शीर्षकम्_आकार, info_आकार, length);
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    यदि(length>0) {
        दत्त_शीर्षकम् *बहिर्गमनम्_शीर्षकम्;
        ध्रुव वर्ण *s;
        int32_t max_दीर्घता_2;

        /* Most of the fields are just bytes and need no swapping. */
        यदि(अन्तर्_दत्त!=बहिर्गमनम्_दत्त) {
            uprv_memcpy(बहिर्गमनम्_दत्त, अन्तर्_दत्त, शीर्षकम्_आकार);
        }
        बहिर्गमनम्_शीर्षकम्=(दत्त_शीर्षकम् *)बहिर्गमनम्_दत्त;

        बहिर्गमनम्_शीर्षकम्->info.isBigEndian = ds->बहिर्गमनम्_Is_Big_Endian;
        बहिर्गमनम्_शीर्षकम्->info.charsetFamily = ds->बहिर्गमनम्_Charset;

        /* swap headerSize */
        ds->swapArray16(ds, &p_शीर्षकम्->दत्त_शीर्षकम्_2.शीर्षकम्_आकार, 2, &बहिर्गमनम्_शीर्षकम्->दत्त_शीर्षकम्_2.शीर्षकम्_आकार, p_Error_संहिता);

        /* swap UDataInfo size and reservedWord */
        ds->swapArray16(ds, &p_शीर्षकम्->info.size, 4, &बहिर्गमनम्_शीर्षकम्->info.size, p_Error_संहिता);

        /* swap copyright statement after the UDataInfo */
        info_आकार+=परिमाण(p_शीर्षकम्->दत्त_शीर्षकम्_2);
        s=(ध्रुव वर्ण *)अन्तर्_दत्त+info_आकार;
        max_दीर्घता_2=शीर्षकम्_आकार-info_आकार;
        /* get the length of the string */
        प्रति(length=0; length<max_दीर्घता_2 && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->swapInvChars(ds, s, length, (वर्ण *)बहिर्गमनम्_दत्त+info_आकार, p_Error_संहिता);
    }

    प्रत्यागम शीर्षकम्_आकार;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool अन्तर्_Is_Big_Endian, uint8_t अन्तर्_Charset,
                  UBool बहिर्गमनम्_Is_Big_Endian, uint8_t बहिर्गमनम्_Charset,
                  UError_संहिता *p_Error_संहिता) {
    UDataSwapper *swapper;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(अन्तर्_Charset>U_EBCDIC_FAMILY || बहिर्गमनम्_Charset>U_EBCDIC_FAMILY) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(परिमाण(UDataSwapper));
    यदि(swapper==शून्यसूचकः) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    uprv_memset(swapper, 0, परिमाण(UDataSwapper));

    /* set values and functions pointers according to in/out parameters */
    swapper->अन्तर्_Is_Big_Endian=अन्तर्_Is_Big_Endian;
    swapper->अन्तर्_Charset=अन्तर्_Charset;
    swapper->बहिर्गमनम्_Is_Big_Endian=बहिर्गमनम्_Is_Big_Endian;
    swapper->बहिर्गमनम्_Charset=बहिर्गमनम्_Charset;

    swapper->readUInt16= अन्तर्_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_प्रत्यक्षम्_UInt16 : uprv_readSwapUInt16;
    swapper->readUInt32= अन्तर्_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_प्रत्यक्षम्_UInt32 : uprv_readSwapUInt32;

    swapper->writeUInt16= बहिर्गमनम्_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_प्रत्यक्षम्_UInt16 : uprv_writeSwapUInt16;
    swapper->writeUInt32= बहिर्गमनम्_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_प्रत्यक्षम्_UInt32 : uprv_writeSwapUInt32;

    swapper->compareInvChars= बहिर्गमनम्_Charset==U_ASCII_FAMILY ? uprv_compareInvAscii : uprv_compareInvEbcdic;

    यदि(अन्तर्_Is_Big_Endian==बहिर्गमनम्_Is_Big_Endian) {
        swapper->swapArray16=uprv_copyArray16;
        swapper->swapArray32=uprv_copyArray32;
        swapper->swapArray64=uprv_copyArray64;
    } अन्यथा {
        swapper->swapArray16=uprv_swapArray16;
        swapper->swapArray32=uprv_swapArray32;
        swapper->swapArray64=uprv_swapArray64;
    }

    यदि(अन्तर्_Charset==U_ASCII_FAMILY) {
        swapper->swapInvChars= बहिर्गमनम्_Charset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } अन्यथा /* U_EBCDIC_FAMILY */ {
        swapper->swapInvChars= बहिर्गमनम्_Charset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    प्रत्यागम swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_निवेशः_दत्त(ध्रुव शून्य *दत्त, int32_t length,
                              UBool बहिर्गमनम्_Is_Big_Endian, uint8_t बहिर्गमनम्_Charset,
                              UError_संहिता *p_Error_संहिता) {
    ध्रुव दत्त_शीर्षकम् *p_शीर्षकम्;
    uint16_t शीर्षकम्_आकार, info_आकार;
    UBool अन्तर्_Is_Big_Endian;
    int8_t अन्तर्_Charset;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि( दत्त==शून्यसूचकः ||
        (length>=0 && length<(int32_t)परिमाण(दत्त_शीर्षकम्)) ||
        बहिर्गमनम्_Charset>U_EBCDIC_FAMILY
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    p_शीर्षकम्=(ध्रुव दत्त_शीर्षकम् *)दत्त;
    यदि( (length>=0 && length<(int32_t)परिमाण(दत्त_शीर्षकम्)) ||
        p_शीर्षकम्->दत्त_शीर्षकम्_2.magic1!=0xda ||
        p_शीर्षकम्->दत्त_शीर्षकम्_2.magic2!=0x27 ||
        p_शीर्षकम्->info.sizeofUChar!=2
    ) {
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    अन्तर्_Is_Big_Endian=(UBool)p_शीर्षकम्->info.isBigEndian;
    अन्तर्_Charset=p_शीर्षकम्->info.charsetFamily;

    यदि(अन्तर्_Is_Big_Endian==U_IS_BIG_ENDIAN) {
        शीर्षकम्_आकार=p_शीर्षकम्->दत्त_शीर्षकम्_2.शीर्षकम्_आकार;
        info_आकार=p_शीर्षकम्->info.size;
    } अन्यथा {
        शीर्षकम्_आकार=uprv_readSwapUInt16(p_शीर्षकम्->दत्त_शीर्षकम्_2.शीर्षकम्_आकार);
        info_आकार=uprv_readSwapUInt16(p_शीर्षकम्->info.size);
    }

    यदि( शीर्षकम्_आकार<परिमाण(दत्त_शीर्षकम्) ||
        info_आकार<परिमाण(UDataInfo) ||
        शीर्षकम्_आकार<(परिमाण(p_शीर्षकम्->दत्त_शीर्षकम्_2)+info_आकार) ||
        (length>=0 && length<शीर्षकम्_आकार)
    ) {
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    प्रत्यागम udata_openSwapper(अन्तर्_Is_Big_Endian, अन्तर्_Charset, बहिर्गमनम्_Is_Big_Endian, बहिर्गमनम्_Charset, p_Error_संहिता);
}

U_CAPI शून्य U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
