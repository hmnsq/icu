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
uprv_بدلون_Array16(ثابت UDataSwapper *ds,
                 ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                 UError_کوډ *p_Error_کوډ) {
    ثابت uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || length<0 || (length&1)!=0 || بهر_معلومات==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint16_t*>(دننه_معلومات);
    q = static_cast<uint16_t*>(بهر_معلومات);
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
                 ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                 UError_کوډ *p_Error_کوډ) {
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || length<0 || (length&1)!=0 || بهر_معلومات==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(length>0 && دننه_معلومات!=بهر_معلومات) {
        uprv_memcpy(بهر_معلومات, دننه_معلومات, length);
    }
    أرجع length;
}

ساكن int32_t U_CALLCONV
uprv_بدلون_Array32(ثابت UDataSwapper *ds,
                 ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                 UError_کوډ *p_Error_کوډ) {
    ثابت uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || length<0 || (length&3)!=0 || بهر_معلومات==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint32_t*>(دننه_معلومات);
    q = static_cast<uint32_t*>(بهر_معلومات);
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
                 ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                 UError_کوډ *p_Error_کوډ) {
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || length<0 || (length&3)!=0 || بهر_معلومات==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(length>0 && دننه_معلومات!=بهر_معلومات) {
        uprv_memcpy(بهر_معلومات, دننه_معلومات, length);
    }
    أرجع length;
}

ساكن int32_t U_CALLCONV
uprv_بدلون_Array64(ثابت UDataSwapper *ds,
                 ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                 UError_کوډ *p_Error_کوډ) {
    ثابت uint64_t *p;
    uint64_t *q;
    int32_t count;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || length<0 || (length&7)!=0 || بهر_معلومات==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint64_t*>(دننه_معلومات);
    q = static_cast<uint64_t*>(بهر_معلومات);
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
                 ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                 UError_کوډ *p_Error_کوډ) {
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || length<0 || (length&7)!=0 || بهر_معلومات==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(length>0 && دننه_معلومات!=بهر_معلومات) {
        uprv_memcpy(بهر_معلومات, دننه_معلومات, length);
    }
    أرجع length;
}

ساكن uint16_t U_CALLCONV
uprv_read_بدلون_UInt16(uint16_t x) {
    أرجع static_cast<uint16_t>((x << 8) | (x >> 8));
}

ساكن uint16_t U_CALLCONV
uprv_read_مستقیم_UInt16(uint16_t x) {
    أرجع x;
}

ساكن uint32_t U_CALLCONV
uprv_read_بدلون_UInt32(uint32_t x) {
    أرجع (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ساكن uint32_t U_CALLCONV
uprv_read_مستقیم_UInt32(uint32_t x) {
    أرجع x;
}

ساكن فراغ U_CALLCONV
uprv_write_بدلون_UInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

ساكن فراغ U_CALLCONV
uprv_write_مستقیم_UInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

ساكن فراغ U_CALLCONV
uprv_write_بدلون_UInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ساكن فراغ U_CALLCONV
uprv_write_مستقیم_UInt32(uint32_t *p, uint32_t x) {
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
udata_بدلون_Inv_String_بلاک(ثابت UDataSwapper *ds,
                         ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                         UError_کوډ *p_Error_کوډ) {
    ثابت حرف *دننه_Chars;
    int32_t مزي_ګانې_اوږدتیا;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || length<0 || (length>0 && بهر_معلومات==تش_اشاره)) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    دننه_Chars=(ثابت حرف *)دننه_معلومات;
    مزي_ګانې_اوږدتیا=length;
    طالما(مزي_ګانې_اوږدتیا>0 && دننه_Chars[مزي_ګانې_اوږدتیا-1]!=0) {
        --مزي_ګانې_اوږدتیا;
    }

    /* swap up to the last NUL */
    ds->بدلون_Inv_Chars(ds, دننه_معلومات, مزي_ګانې_اوږدتیا, بهر_معلومات, p_Error_کوډ);

    /* copy the bytes after the last NUL */
    إذا(دننه_معلومات!=بهر_معلومات && length>مزي_ګانې_اوږدتیا) {
        uprv_memcpy((حرف *)بهر_معلومات+مزي_ګانې_اوږدتیا, دننه_Chars+مزي_ګانې_اوږدتیا, length-مزي_ګانې_اوږدتیا);
    }

    /* return the length including padding bytes */
    إذا(U_SUCCESS(*p_Error_کوډ)) {
        أرجع length;
    } وإلا {
        أرجع 0;
    }
}

U_CAPI فراغ U_EXPORT2
udata_printError(ثابت UDataSwapper *ds,
                 ثابت حرف *fmt,
                 ...) {
    va_list دلیلونه_2;

    إذا(ds->printError!=تش_اشاره) {
        va_start(دلیلونه_2, fmt);
        ds->printError(ds->printErrorContext, fmt, دلیلونه_2);
        va_end(دلیلونه_2);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_بدلون_معلومات_سرلیک(ثابت UDataSwapper *ds,
                     ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                     UError_کوډ *p_Error_کوډ) {
    ثابت معلومات_سرلیک *p_سرلیک;
    uint16_t سرلیک_اندازه, info_اندازه;

    /* argument checking */
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(ds==تش_اشاره || دننه_معلومات==تش_اشاره || length<-1 || (length>0 && بهر_معلومات==تش_اشاره)) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* check minimum length and magic bytes */
    p_سرلیک=(ثابت معلومات_سرلیک *)دننه_معلومات;
    إذا( (length>=0 && length<(int32_t)حجم(معلومات_سرلیک)) ||
        p_سرلیک->معلومات_سرلیک_2.magic1!=0xda ||
        p_سرلیک->معلومات_سرلیک_2.magic2!=0x27 ||
        p_سرلیک->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    سرلیک_اندازه=ds->readUInt16(p_سرلیک->معلومات_سرلیک_2.سرلیک_اندازه);
    info_اندازه=ds->readUInt16(p_سرلیک->info.size);

    إذا( سرلیک_اندازه<حجم(معلومات_سرلیک) ||
        info_اندازه<حجم(UDataInfo) ||
        سرلیک_اندازه<(حجم(p_سرلیک->معلومات_سرلیک_2)+info_اندازه) ||
        (length>=0 && length<سرلیک_اندازه)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         سرلیک_اندازه, info_اندازه, length);
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    إذا(length>0) {
        معلومات_سرلیک *بهر_سرلیک;
        ثابت حرف *s;
        int32_t max_اوږدتیا_2;

        /* Most of the fields are just bytes and need no swapping. */
        إذا(دننه_معلومات!=بهر_معلومات) {
            uprv_memcpy(بهر_معلومات, دننه_معلومات, سرلیک_اندازه);
        }
        بهر_سرلیک=(معلومات_سرلیک *)بهر_معلومات;

        بهر_سرلیک->info.isBigEndian = ds->بهر_Is_Big_Endian;
        بهر_سرلیک->info.charsetFamily = ds->بهر_Charset;

        /* swap headerSize */
        ds->بدلون_Array16(ds, &p_سرلیک->معلومات_سرلیک_2.سرلیک_اندازه, 2, &بهر_سرلیک->معلومات_سرلیک_2.سرلیک_اندازه, p_Error_کوډ);

        /* swap UDataInfo size and reservedWord */
        ds->بدلون_Array16(ds, &p_سرلیک->info.size, 4, &بهر_سرلیک->info.size, p_Error_کوډ);

        /* swap copyright statement after the UDataInfo */
        info_اندازه+=حجم(p_سرلیک->معلومات_سرلیک_2);
        s=(ثابت حرف *)دننه_معلومات+info_اندازه;
        max_اوږدتیا_2=سرلیک_اندازه-info_اندازه;
        /* get the length of the string */
        لكل(length=0; length<max_اوږدتیا_2 && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->بدلون_Inv_Chars(ds, s, length, (حرف *)بهر_معلومات+info_اندازه, p_Error_کوډ);
    }

    أرجع سرلیک_اندازه;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool دننه_Is_Big_Endian, uint8_t دننه_Charset,
                  UBool بهر_Is_Big_Endian, uint8_t بهر_Charset,
                  UError_کوډ *p_Error_کوډ) {
    UDataSwapper *swapper;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا(دننه_Charset>U_EBCDIC_FAMILY || بهر_Charset>U_EBCDIC_FAMILY) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(حجم(UDataSwapper));
    إذا(swapper==تش_اشاره) {
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    uprv_memset(swapper, 0, حجم(UDataSwapper));

    /* set values and functions pointers according to in/out parameters */
    swapper->دننه_Is_Big_Endian=دننه_Is_Big_Endian;
    swapper->دننه_Charset=دننه_Charset;
    swapper->بهر_Is_Big_Endian=بهر_Is_Big_Endian;
    swapper->بهر_Charset=بهر_Charset;

    swapper->readUInt16= دننه_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_مستقیم_UInt16 : uprv_read_بدلون_UInt16;
    swapper->readUInt32= دننه_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_مستقیم_UInt32 : uprv_read_بدلون_UInt32;

    swapper->writeUInt16= بهر_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_مستقیم_UInt16 : uprv_write_بدلون_UInt16;
    swapper->writeUInt32= بهر_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_مستقیم_UInt32 : uprv_write_بدلون_UInt32;

    swapper->compareInvChars= بهر_Charset==U_ASCII_FAMILY ? uprv_compareInvAscii : uprv_compareInvEbcdic;

    إذا(دننه_Is_Big_Endian==بهر_Is_Big_Endian) {
        swapper->بدلون_Array16=uprv_copyArray16;
        swapper->بدلون_Array32=uprv_copyArray32;
        swapper->بدلون_Array64=uprv_copyArray64;
    } وإلا {
        swapper->بدلون_Array16=uprv_بدلون_Array16;
        swapper->بدلون_Array32=uprv_بدلون_Array32;
        swapper->بدلون_Array64=uprv_بدلون_Array64;
    }

    إذا(دننه_Charset==U_ASCII_FAMILY) {
        swapper->بدلون_Inv_Chars= بهر_Charset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } وإلا /* U_EBCDIC_FAMILY */ {
        swapper->بدلون_Inv_Chars= بهر_Charset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    أرجع swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_ننوت_معلومات(ثابت فراغ *معلومات, int32_t length,
                              UBool بهر_Is_Big_Endian, uint8_t بهر_Charset,
                              UError_کوډ *p_Error_کوډ) {
    ثابت معلومات_سرلیک *p_سرلیک;
    uint16_t سرلیک_اندازه, info_اندازه;
    UBool دننه_Is_Big_Endian;
    int8_t دننه_Charset;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا( معلومات==تش_اشاره ||
        (length>=0 && length<(int32_t)حجم(معلومات_سرلیک)) ||
        بهر_Charset>U_EBCDIC_FAMILY
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    p_سرلیک=(ثابت معلومات_سرلیک *)معلومات;
    إذا( (length>=0 && length<(int32_t)حجم(معلومات_سرلیک)) ||
        p_سرلیک->معلومات_سرلیک_2.magic1!=0xda ||
        p_سرلیک->معلومات_سرلیک_2.magic2!=0x27 ||
        p_سرلیک->info.sizeofUChar!=2
    ) {
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع تش_اشاره;
    }

    دننه_Is_Big_Endian=(UBool)p_سرلیک->info.isBigEndian;
    دننه_Charset=p_سرلیک->info.charsetFamily;

    إذا(دننه_Is_Big_Endian==U_IS_BIG_ENDIAN) {
        سرلیک_اندازه=p_سرلیک->معلومات_سرلیک_2.سرلیک_اندازه;
        info_اندازه=p_سرلیک->info.size;
    } وإلا {
        سرلیک_اندازه=uprv_read_بدلون_UInt16(p_سرلیک->معلومات_سرلیک_2.سرلیک_اندازه);
        info_اندازه=uprv_read_بدلون_UInt16(p_سرلیک->info.size);
    }

    إذا( سرلیک_اندازه<حجم(معلومات_سرلیک) ||
        info_اندازه<حجم(UDataInfo) ||
        سرلیک_اندازه<(حجم(p_سرلیک->معلومات_سرلیک_2)+info_اندازه) ||
        (length>=0 && length<سرلیک_اندازه)
    ) {
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع تش_اشاره;
    }

    أرجع udata_openSwapper(دننه_Is_Big_Endian, دننه_Charset, بهر_Is_Big_Endian, بهر_Charset, p_Error_کوډ);
}

U_CAPI فراغ U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
