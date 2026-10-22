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
uprv_مٽا_سٽا_Array16(ثابت UDataSwapper *ds,
                 ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || length<0 || (length&1)!=0 || ٻاهريون_معلومات==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint16_t*>(اندريون_معلومات);
    q = static_cast<uint16_t*>(ٻاهريون_معلومات);
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
                 ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || length<0 || (length&1)!=0 || ٻاهريون_معلومات==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    إذا(length>0 && اندريون_معلومات!=ٻاهريون_معلومات) {
        uprv_memcpy(ٻاهريون_معلومات, اندريون_معلومات, length);
    }
    أرجع length;
}

ساكن int32_t U_CALLCONV
uprv_مٽا_سٽا_Array32(ثابت UDataSwapper *ds,
                 ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || length<0 || (length&3)!=0 || ٻاهريون_معلومات==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint32_t*>(اندريون_معلومات);
    q = static_cast<uint32_t*>(ٻاهريون_معلومات);
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
                 ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || length<0 || (length&3)!=0 || ٻاهريون_معلومات==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    إذا(length>0 && اندريون_معلومات!=ٻاهريون_معلومات) {
        uprv_memcpy(ٻاهريون_معلومات, اندريون_معلومات, length);
    }
    أرجع length;
}

ساكن int32_t U_CALLCONV
uprv_مٽا_سٽا_Array64(ثابت UDataSwapper *ds,
                 ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت uint64_t *p;
    uint64_t *q;
    int32_t count;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || length<0 || (length&7)!=0 || ٻاهريون_معلومات==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint64_t*>(اندريون_معلومات);
    q = static_cast<uint64_t*>(ٻاهريون_معلومات);
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
                 ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || length<0 || (length&7)!=0 || ٻاهريون_معلومات==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    إذا(length>0 && اندريون_معلومات!=ٻاهريون_معلومات) {
        uprv_memcpy(ٻاهريون_معلومات, اندريون_معلومات, length);
    }
    أرجع length;
}

ساكن uint16_t U_CALLCONV
uprv_read_مٽا_سٽا_UInt16(uint16_t x) {
    أرجع static_cast<uint16_t>((x << 8) | (x >> 8));
}

ساكن uint16_t U_CALLCONV
uprv_read_سڌو_UInt16(uint16_t x) {
    أرجع x;
}

ساكن uint32_t U_CALLCONV
uprv_read_مٽا_سٽا_UInt32(uint32_t x) {
    أرجع (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ساكن uint32_t U_CALLCONV
uprv_read_سڌو_UInt32(uint32_t x) {
    أرجع x;
}

ساكن فراغ U_CALLCONV
uprv_write_مٽا_سٽا_UInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

ساكن فراغ U_CALLCONV
uprv_write_سڌو_UInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

ساكن فراغ U_CALLCONV
uprv_write_مٽا_سٽا_UInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ساكن فراغ U_CALLCONV
uprv_write_سڌو_UInt32(uint32_t *p, uint32_t x) {
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
udata_مٽا_سٽا_Inv_String_بلاڪ(ثابت UDataSwapper *ds,
                         ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                         UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت حرف *اندريون_Chars;
    int32_t لڙيون_ڊيگهائي;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || length<0 || (length>0 && ٻاهريون_معلومات==خالي_اشارو)) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    اندريون_Chars=(ثابت حرف *)اندريون_معلومات;
    لڙيون_ڊيگهائي=length;
    طالما(لڙيون_ڊيگهائي>0 && اندريون_Chars[لڙيون_ڊيگهائي-1]!=0) {
        --لڙيون_ڊيگهائي;
    }

    /* swap up to the last NUL */
    ds->مٽا_سٽا_Inv_Chars(ds, اندريون_معلومات, لڙيون_ڊيگهائي, ٻاهريون_معلومات, p_Error_ڪوڊ);

    /* copy the bytes after the last NUL */
    إذا(اندريون_معلومات!=ٻاهريون_معلومات && length>لڙيون_ڊيگهائي) {
        uprv_memcpy((حرف *)ٻاهريون_معلومات+لڙيون_ڊيگهائي, اندريون_Chars+لڙيون_ڊيگهائي, length-لڙيون_ڊيگهائي);
    }

    /* return the length including padding bytes */
    إذا(U_SUCCESS(*p_Error_ڪوڊ)) {
        أرجع length;
    } وإلا {
        أرجع 0;
    }
}

U_CAPI فراغ U_EXPORT2
udata_printError(ثابت UDataSwapper *ds,
                 ثابت حرف *fmt,
                 ...) {
    va_list دليلون_2;

    إذا(ds->printError!=خالي_اشارو) {
        va_start(دليلون_2, fmt);
        ds->printError(ds->printErrorContext, fmt, دليلون_2);
        va_end(دليلون_2);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_مٽا_سٽا_معلومات_سرو(ثابت UDataSwapper *ds,
                     ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                     UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت معلومات_سرو *p_سرو;
    uint16_t سرو_سائيز, info_سائيز;

    /* argument checking */
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(ds==خالي_اشارو || اندريون_معلومات==خالي_اشارو || length<-1 || (length>0 && ٻاهريون_معلومات==خالي_اشارو)) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* check minimum length and magic bytes */
    p_سرو=(ثابت معلومات_سرو *)اندريون_معلومات;
    إذا( (length>=0 && length<(int32_t)حجم(معلومات_سرو)) ||
        p_سرو->معلومات_سرو_2.magic1!=0xda ||
        p_سرو->معلومات_سرو_2.magic2!=0x27 ||
        p_سرو->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    سرو_سائيز=ds->readUInt16(p_سرو->معلومات_سرو_2.سرو_سائيز);
    info_سائيز=ds->readUInt16(p_سرو->info.size);

    إذا( سرو_سائيز<حجم(معلومات_سرو) ||
        info_سائيز<حجم(UDataInfo) ||
        سرو_سائيز<(حجم(p_سرو->معلومات_سرو_2)+info_سائيز) ||
        (length>=0 && length<سرو_سائيز)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         سرو_سائيز, info_سائيز, length);
        *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    إذا(length>0) {
        معلومات_سرو *ٻاهريون_سرو;
        ثابت حرف *s;
        int32_t max_ڊيگهائي_2;

        /* Most of the fields are just bytes and need no swapping. */
        إذا(اندريون_معلومات!=ٻاهريون_معلومات) {
            uprv_memcpy(ٻاهريون_معلومات, اندريون_معلومات, سرو_سائيز);
        }
        ٻاهريون_سرو=(معلومات_سرو *)ٻاهريون_معلومات;

        ٻاهريون_سرو->info.isBigEndian = ds->ٻاهريون_Is_Big_Endian;
        ٻاهريون_سرو->info.charsetFamily = ds->ٻاهريون_Charset;

        /* swap headerSize */
        ds->مٽا_سٽا_Array16(ds, &p_سرو->معلومات_سرو_2.سرو_سائيز, 2, &ٻاهريون_سرو->معلومات_سرو_2.سرو_سائيز, p_Error_ڪوڊ);

        /* swap UDataInfo size and reservedWord */
        ds->مٽا_سٽا_Array16(ds, &p_سرو->info.size, 4, &ٻاهريون_سرو->info.size, p_Error_ڪوڊ);

        /* swap copyright statement after the UDataInfo */
        info_سائيز+=حجم(p_سرو->معلومات_سرو_2);
        s=(ثابت حرف *)اندريون_معلومات+info_سائيز;
        max_ڊيگهائي_2=سرو_سائيز-info_سائيز;
        /* get the length of the string */
        لكل(length=0; length<max_ڊيگهائي_2 && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->مٽا_سٽا_Inv_Chars(ds, s, length, (حرف *)ٻاهريون_معلومات+info_سائيز, p_Error_ڪوڊ);
    }

    أرجع سرو_سائيز;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool اندريون_Is_Big_Endian, uint8_t اندريون_Charset,
                  UBool ٻاهريون_Is_Big_Endian, uint8_t ٻاهريون_Charset,
                  UError_ڪوڊ *p_Error_ڪوڊ) {
    UDataSwapper *swapper;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا(اندريون_Charset>U_EBCDIC_FAMILY || ٻاهريون_Charset>U_EBCDIC_FAMILY) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(حجم(UDataSwapper));
    إذا(swapper==خالي_اشارو) {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    uprv_memset(swapper, 0, حجم(UDataSwapper));

    /* set values and functions pointers according to in/out parameters */
    swapper->اندريون_Is_Big_Endian=اندريون_Is_Big_Endian;
    swapper->اندريون_Charset=اندريون_Charset;
    swapper->ٻاهريون_Is_Big_Endian=ٻاهريون_Is_Big_Endian;
    swapper->ٻاهريون_Charset=ٻاهريون_Charset;

    swapper->readUInt16= اندريون_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_سڌو_UInt16 : uprv_read_مٽا_سٽا_UInt16;
    swapper->readUInt32= اندريون_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_سڌو_UInt32 : uprv_read_مٽا_سٽا_UInt32;

    swapper->writeUInt16= ٻاهريون_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_سڌو_UInt16 : uprv_write_مٽا_سٽا_UInt16;
    swapper->writeUInt32= ٻاهريون_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_سڌو_UInt32 : uprv_write_مٽا_سٽا_UInt32;

    swapper->compareInvChars= ٻاهريون_Charset==U_ASCII_FAMILY ? uprv_compareInvAscii : uprv_compareInvEbcdic;

    إذا(اندريون_Is_Big_Endian==ٻاهريون_Is_Big_Endian) {
        swapper->مٽا_سٽا_Array16=uprv_copyArray16;
        swapper->مٽا_سٽا_Array32=uprv_copyArray32;
        swapper->مٽا_سٽا_Array64=uprv_copyArray64;
    } وإلا {
        swapper->مٽا_سٽا_Array16=uprv_مٽا_سٽا_Array16;
        swapper->مٽا_سٽا_Array32=uprv_مٽا_سٽا_Array32;
        swapper->مٽا_سٽا_Array64=uprv_مٽا_سٽا_Array64;
    }

    إذا(اندريون_Charset==U_ASCII_FAMILY) {
        swapper->مٽا_سٽا_Inv_Chars= ٻاهريون_Charset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } وإلا /* U_EBCDIC_FAMILY */ {
        swapper->مٽا_سٽا_Inv_Chars= ٻاهريون_Charset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    أرجع swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_داخلا_معلومات(ثابت فراغ *معلومات, int32_t length,
                              UBool ٻاهريون_Is_Big_Endian, uint8_t ٻاهريون_Charset,
                              UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت معلومات_سرو *p_سرو;
    uint16_t سرو_سائيز, info_سائيز;
    UBool اندريون_Is_Big_Endian;
    int8_t اندريون_Charset;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا( معلومات==خالي_اشارو ||
        (length>=0 && length<(int32_t)حجم(معلومات_سرو)) ||
        ٻاهريون_Charset>U_EBCDIC_FAMILY
    ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    p_سرو=(ثابت معلومات_سرو *)معلومات;
    إذا( (length>=0 && length<(int32_t)حجم(معلومات_سرو)) ||
        p_سرو->معلومات_سرو_2.magic1!=0xda ||
        p_سرو->معلومات_سرو_2.magic2!=0x27 ||
        p_سرو->info.sizeofUChar!=2
    ) {
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        أرجع خالي_اشارو;
    }

    اندريون_Is_Big_Endian=(UBool)p_سرو->info.isBigEndian;
    اندريون_Charset=p_سرو->info.charsetFamily;

    إذا(اندريون_Is_Big_Endian==U_IS_BIG_ENDIAN) {
        سرو_سائيز=p_سرو->معلومات_سرو_2.سرو_سائيز;
        info_سائيز=p_سرو->info.size;
    } وإلا {
        سرو_سائيز=uprv_read_مٽا_سٽا_UInt16(p_سرو->معلومات_سرو_2.سرو_سائيز);
        info_سائيز=uprv_read_مٽا_سٽا_UInt16(p_سرو->info.size);
    }

    إذا( سرو_سائيز<حجم(معلومات_سرو) ||
        info_سائيز<حجم(UDataInfo) ||
        سرو_سائيز<(حجم(p_سرو->معلومات_سرو_2)+info_سائيز) ||
        (length>=0 && length<سرو_سائيز)
    ) {
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        أرجع خالي_اشارو;
    }

    أرجع udata_openSwapper(اندريون_Is_Big_Endian, اندريون_Charset, ٻاهريون_Is_Big_Endian, ٻاهريون_Charset, p_Error_ڪوڊ);
}

U_CAPI فراغ U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
