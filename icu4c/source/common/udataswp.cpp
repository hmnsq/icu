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
uprv_جابجا_Array16(ثابت UDataSwapper *ds,
                 ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
                 UError_کد *p_Error_کد) {
    ثابت uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || length<0 || (length&1)!=0 || بیرون_داده==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint16_t*>(درون_داده);
    q = static_cast<uint16_t*>(بیرون_داده);
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
                 ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
                 UError_کد *p_Error_کد) {
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || length<0 || (length&1)!=0 || بیرون_داده==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    إذا(length>0 && درون_داده!=بیرون_داده) {
        uprv_memcpy(بیرون_داده, درون_داده, length);
    }
    أرجع length;
}

ساكن int32_t U_CALLCONV
uprv_جابجا_Array32(ثابت UDataSwapper *ds,
                 ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
                 UError_کد *p_Error_کد) {
    ثابت uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || length<0 || (length&3)!=0 || بیرون_داده==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint32_t*>(درون_داده);
    q = static_cast<uint32_t*>(بیرون_داده);
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
                 ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
                 UError_کد *p_Error_کد) {
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || length<0 || (length&3)!=0 || بیرون_داده==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    إذا(length>0 && درون_داده!=بیرون_داده) {
        uprv_memcpy(بیرون_داده, درون_داده, length);
    }
    أرجع length;
}

ساكن int32_t U_CALLCONV
uprv_جابجا_Array64(ثابت UDataSwapper *ds,
                 ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
                 UError_کد *p_Error_کد) {
    ثابت uint64_t *p;
    uint64_t *q;
    int32_t count;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || length<0 || (length&7)!=0 || بیرون_داده==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint64_t*>(درون_داده);
    q = static_cast<uint64_t*>(بیرون_داده);
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
                 ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
                 UError_کد *p_Error_کد) {
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || length<0 || (length&7)!=0 || بیرون_داده==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    إذا(length>0 && درون_داده!=بیرون_داده) {
        uprv_memcpy(بیرون_داده, درون_داده, length);
    }
    أرجع length;
}

ساكن uint16_t U_CALLCONV
uprv_read_جابجا_UInt16(uint16_t x) {
    أرجع static_cast<uint16_t>((x << 8) | (x >> 8));
}

ساكن uint16_t U_CALLCONV
uprv_read_مستقیم_UInt16(uint16_t x) {
    أرجع x;
}

ساكن uint32_t U_CALLCONV
uprv_read_جابجا_UInt32(uint32_t x) {
    أرجع (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ساكن uint32_t U_CALLCONV
uprv_read_مستقیم_UInt32(uint32_t x) {
    أرجع x;
}

ساكن فراغ U_CALLCONV
uprv_write_جابجا_UInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

ساكن فراغ U_CALLCONV
uprv_write_مستقیم_UInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

ساكن فراغ U_CALLCONV
uprv_write_جابجا_UInt32(uint32_t *p, uint32_t x) {
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
udata_جابجا_Inv_String_بلوک(ثابت UDataSwapper *ds,
                         ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
                         UError_کد *p_Error_کد) {
    ثابت حرف *درون_Chars;
    int32_t رشتهها_درازی;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || length<0 || (length>0 && بیرون_داده==اشاره_تهی)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    درون_Chars=(ثابت حرف *)درون_داده;
    رشتهها_درازی=length;
    طالما(رشتهها_درازی>0 && درون_Chars[رشتهها_درازی-1]!=0) {
        --رشتهها_درازی;
    }

    /* swap up to the last NUL */
    ds->جابجا_Inv_Chars(ds, درون_داده, رشتهها_درازی, بیرون_داده, p_Error_کد);

    /* copy the bytes after the last NUL */
    إذا(درون_داده!=بیرون_داده && length>رشتهها_درازی) {
        uprv_memcpy((حرف *)بیرون_داده+رشتهها_درازی, درون_Chars+رشتهها_درازی, length-رشتهها_درازی);
    }

    /* return the length including padding bytes */
    إذا(U_SUCCESS(*p_Error_کد)) {
        أرجع length;
    } وإلا {
        أرجع 0;
    }
}

U_CAPI فراغ U_EXPORT2
udata_printError(ثابت UDataSwapper *ds,
                 ثابت حرف *fmt,
                 ...) {
    va_list آرگومانها_2;

    إذا(ds->printError!=اشاره_تهی) {
        va_start(آرگومانها_2, fmt);
        ds->printError(ds->printErrorContext, fmt, آرگومانها_2);
        va_end(آرگومانها_2);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_جابجا_داده_سرآیند(ثابت UDataSwapper *ds,
                     ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
                     UError_کد *p_Error_کد) {
    ثابت داده_سرآیند *p_سرآیند;
    uint16_t سرآیند_اندازه, info_اندازه;

    /* argument checking */
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(ds==اشاره_تهی || درون_داده==اشاره_تهی || length<-1 || (length>0 && بیرون_داده==اشاره_تهی)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* check minimum length and magic bytes */
    p_سرآیند=(ثابت داده_سرآیند *)درون_داده;
    إذا( (length>=0 && length<(int32_t)حجم(داده_سرآیند)) ||
        p_سرآیند->داده_سرآیند_2.magic1!=0xda ||
        p_سرآیند->داده_سرآیند_2.magic2!=0x27 ||
        p_سرآیند->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    سرآیند_اندازه=ds->readUInt16(p_سرآیند->داده_سرآیند_2.سرآیند_اندازه);
    info_اندازه=ds->readUInt16(p_سرآیند->info.size);

    إذا( سرآیند_اندازه<حجم(داده_سرآیند) ||
        info_اندازه<حجم(UDataInfo) ||
        سرآیند_اندازه<(حجم(p_سرآیند->داده_سرآیند_2)+info_اندازه) ||
        (length>=0 && length<سرآیند_اندازه)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         سرآیند_اندازه, info_اندازه, length);
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    إذا(length>0) {
        داده_سرآیند *بیرون_سرآیند;
        ثابت حرف *s;
        int32_t max_درازی_2;

        /* Most of the fields are just bytes and need no swapping. */
        إذا(درون_داده!=بیرون_داده) {
            uprv_memcpy(بیرون_داده, درون_داده, سرآیند_اندازه);
        }
        بیرون_سرآیند=(داده_سرآیند *)بیرون_داده;

        بیرون_سرآیند->info.isBigEndian = ds->بیرون_Is_Big_Endian;
        بیرون_سرآیند->info.charsetFamily = ds->بیرون_Charset;

        /* swap headerSize */
        ds->جابجا_Array16(ds, &p_سرآیند->داده_سرآیند_2.سرآیند_اندازه, 2, &بیرون_سرآیند->داده_سرآیند_2.سرآیند_اندازه, p_Error_کد);

        /* swap UDataInfo size and reservedWord */
        ds->جابجا_Array16(ds, &p_سرآیند->info.size, 4, &بیرون_سرآیند->info.size, p_Error_کد);

        /* swap copyright statement after the UDataInfo */
        info_اندازه+=حجم(p_سرآیند->داده_سرآیند_2);
        s=(ثابت حرف *)درون_داده+info_اندازه;
        max_درازی_2=سرآیند_اندازه-info_اندازه;
        /* get the length of the string */
        لكل(length=0; length<max_درازی_2 && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->جابجا_Inv_Chars(ds, s, length, (حرف *)بیرون_داده+info_اندازه, p_Error_کد);
    }

    أرجع سرآیند_اندازه;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool درون_Is_Big_Endian, uint8_t درون_Charset,
                  UBool بیرون_Is_Big_Endian, uint8_t بیرون_Charset,
                  UError_کد *p_Error_کد) {
    UDataSwapper *swapper;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا(درون_Charset>U_EBCDIC_FAMILY || بیرون_Charset>U_EBCDIC_FAMILY) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(حجم(UDataSwapper));
    إذا(swapper==اشاره_تهی) {
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    uprv_memset(swapper, 0, حجم(UDataSwapper));

    /* set values and functions pointers according to in/out parameters */
    swapper->درون_Is_Big_Endian=درون_Is_Big_Endian;
    swapper->درون_Charset=درون_Charset;
    swapper->بیرون_Is_Big_Endian=بیرون_Is_Big_Endian;
    swapper->بیرون_Charset=بیرون_Charset;

    swapper->readUInt16= درون_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_مستقیم_UInt16 : uprv_read_جابجا_UInt16;
    swapper->readUInt32= درون_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_مستقیم_UInt32 : uprv_read_جابجا_UInt32;

    swapper->writeUInt16= بیرون_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_مستقیم_UInt16 : uprv_write_جابجا_UInt16;
    swapper->writeUInt32= بیرون_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_مستقیم_UInt32 : uprv_write_جابجا_UInt32;

    swapper->compareInvChars= بیرون_Charset==U_ASCII_FAMILY ? uprv_compareInvAscii : uprv_compareInvEbcdic;

    إذا(درون_Is_Big_Endian==بیرون_Is_Big_Endian) {
        swapper->جابجا_Array16=uprv_copyArray16;
        swapper->جابجا_Array32=uprv_copyArray32;
        swapper->جابجا_Array64=uprv_copyArray64;
    } وإلا {
        swapper->جابجا_Array16=uprv_جابجا_Array16;
        swapper->جابجا_Array32=uprv_جابجا_Array32;
        swapper->جابجا_Array64=uprv_جابجا_Array64;
    }

    إذا(درون_Charset==U_ASCII_FAMILY) {
        swapper->جابجا_Inv_Chars= بیرون_Charset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } وإلا /* U_EBCDIC_FAMILY */ {
        swapper->جابجا_Inv_Chars= بیرون_Charset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    أرجع swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_درون_داد_داده(ثابت فراغ *داده, int32_t length,
                              UBool بیرون_Is_Big_Endian, uint8_t بیرون_Charset,
                              UError_کد *p_Error_کد) {
    ثابت داده_سرآیند *p_سرآیند;
    uint16_t سرآیند_اندازه, info_اندازه;
    UBool درون_Is_Big_Endian;
    int8_t درون_Charset;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا( داده==اشاره_تهی ||
        (length>=0 && length<(int32_t)حجم(داده_سرآیند)) ||
        بیرون_Charset>U_EBCDIC_FAMILY
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    p_سرآیند=(ثابت داده_سرآیند *)داده;
    إذا( (length>=0 && length<(int32_t)حجم(داده_سرآیند)) ||
        p_سرآیند->داده_سرآیند_2.magic1!=0xda ||
        p_سرآیند->داده_سرآیند_2.magic2!=0x27 ||
        p_سرآیند->info.sizeofUChar!=2
    ) {
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        أرجع اشاره_تهی;
    }

    درون_Is_Big_Endian=(UBool)p_سرآیند->info.isBigEndian;
    درون_Charset=p_سرآیند->info.charsetFamily;

    إذا(درون_Is_Big_Endian==U_IS_BIG_ENDIAN) {
        سرآیند_اندازه=p_سرآیند->داده_سرآیند_2.سرآیند_اندازه;
        info_اندازه=p_سرآیند->info.size;
    } وإلا {
        سرآیند_اندازه=uprv_read_جابجا_UInt16(p_سرآیند->داده_سرآیند_2.سرآیند_اندازه);
        info_اندازه=uprv_read_جابجا_UInt16(p_سرآیند->info.size);
    }

    إذا( سرآیند_اندازه<حجم(داده_سرآیند) ||
        info_اندازه<حجم(UDataInfo) ||
        سرآیند_اندازه<(حجم(p_سرآیند->داده_سرآیند_2)+info_اندازه) ||
        (length>=0 && length<سرآیند_اندازه)
    ) {
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        أرجع اشاره_تهی;
    }

    أرجع udata_openSwapper(درون_Is_Big_Endian, درون_Charset, بیرون_Is_Big_Endian, بیرون_Charset, p_Error_کد);
}

U_CAPI فراغ U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
