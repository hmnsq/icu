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
uprv_وٹاندرا_Array16(ثابت UDataSwapper *ds,
                 ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                 UError_کوڈ *p_Error_کوڈ) {
    ثابت uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || length<0 || (length&1)!=0 || باہرلا_انکڑے==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint16_t*>(وچ_انکڑے);
    q = static_cast<uint16_t*>(باہرلا_انکڑے);
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
                 ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                 UError_کوڈ *p_Error_کوڈ) {
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || length<0 || (length&1)!=0 || باہرلا_انکڑے==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(length>0 && وچ_انکڑے!=باہرلا_انکڑے) {
        uprv_memcpy(باہرلا_انکڑے, وچ_انکڑے, length);
    }
    أرجع length;
}

ساكن int32_t U_CALLCONV
uprv_وٹاندرا_Array32(ثابت UDataSwapper *ds,
                 ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                 UError_کوڈ *p_Error_کوڈ) {
    ثابت uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || length<0 || (length&3)!=0 || باہرلا_انکڑے==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint32_t*>(وچ_انکڑے);
    q = static_cast<uint32_t*>(باہرلا_انکڑے);
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
                 ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                 UError_کوڈ *p_Error_کوڈ) {
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || length<0 || (length&3)!=0 || باہرلا_انکڑے==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(length>0 && وچ_انکڑے!=باہرلا_انکڑے) {
        uprv_memcpy(باہرلا_انکڑے, وچ_انکڑے, length);
    }
    أرجع length;
}

ساكن int32_t U_CALLCONV
uprv_وٹاندرا_Array64(ثابت UDataSwapper *ds,
                 ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                 UError_کوڈ *p_Error_کوڈ) {
    ثابت uint64_t *p;
    uint64_t *q;
    int32_t count;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || length<0 || (length&7)!=0 || باہرلا_انکڑے==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint64_t*>(وچ_انکڑے);
    q = static_cast<uint64_t*>(باہرلا_انکڑے);
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
                 ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                 UError_کوڈ *p_Error_کوڈ) {
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || length<0 || (length&7)!=0 || باہرلا_انکڑے==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(length>0 && وچ_انکڑے!=باہرلا_انکڑے) {
        uprv_memcpy(باہرلا_انکڑے, وچ_انکڑے, length);
    }
    أرجع length;
}

ساكن uint16_t U_CALLCONV
uprv_read_وٹاندرا_UInt16(uint16_t x) {
    أرجع static_cast<uint16_t>((x << 8) | (x >> 8));
}

ساكن uint16_t U_CALLCONV
uprv_read_سدھا_UInt16(uint16_t x) {
    أرجع x;
}

ساكن uint32_t U_CALLCONV
uprv_read_وٹاندرا_UInt32(uint32_t x) {
    أرجع (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ساكن uint32_t U_CALLCONV
uprv_read_سدھا_UInt32(uint32_t x) {
    أرجع x;
}

ساكن فراغ U_CALLCONV
uprv_write_وٹاندرا_UInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

ساكن فراغ U_CALLCONV
uprv_write_سدھا_UInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

ساكن فراغ U_CALLCONV
uprv_write_وٹاندرا_UInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ساكن فراغ U_CALLCONV
uprv_write_سدھا_UInt32(uint32_t *p, uint32_t x) {
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
udata_وٹاندرا_Inv_String_بلاک(ثابت UDataSwapper *ds,
                         ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                         UError_کوڈ *p_Error_کوڈ) {
    ثابت حرف *وچ_Chars;
    int32_t لڑیاں_لمائی;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || length<0 || (length>0 && باہرلا_انکڑے==خالی_سوچک)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    وچ_Chars=(ثابت حرف *)وچ_انکڑے;
    لڑیاں_لمائی=length;
    طالما(لڑیاں_لمائی>0 && وچ_Chars[لڑیاں_لمائی-1]!=0) {
        --لڑیاں_لمائی;
    }

    /* swap up to the last NUL */
    ds->وٹاندرا_Inv_Chars(ds, وچ_انکڑے, لڑیاں_لمائی, باہرلا_انکڑے, p_Error_کوڈ);

    /* copy the bytes after the last NUL */
    إذا(وچ_انکڑے!=باہرلا_انکڑے && length>لڑیاں_لمائی) {
        uprv_memcpy((حرف *)باہرلا_انکڑے+لڑیاں_لمائی, وچ_Chars+لڑیاں_لمائی, length-لڑیاں_لمائی);
    }

    /* return the length including padding bytes */
    إذا(U_SUCCESS(*p_Error_کوڈ)) {
        أرجع length;
    } وإلا {
        أرجع 0;
    }
}

U_CAPI فراغ U_EXPORT2
udata_printError(ثابت UDataSwapper *ds,
                 ثابت حرف *fmt,
                 ...) {
    va_list دلیلاں_2;

    إذا(ds->printError!=خالی_سوچک) {
        va_start(دلیلاں_2, fmt);
        ds->printError(ds->printErrorContext, fmt, دلیلاں_2);
        va_end(دلیلاں_2);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_وٹاندرا_انکڑے_سرخی(ثابت UDataSwapper *ds,
                     ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                     UError_کوڈ *p_Error_کوڈ) {
    ثابت انکڑے_سرخی *p_سرخی;
    uint16_t سرخی_ناپ, info_ناپ;

    /* argument checking */
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(ds==خالی_سوچک || وچ_انکڑے==خالی_سوچک || length<-1 || (length>0 && باہرلا_انکڑے==خالی_سوچک)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* check minimum length and magic bytes */
    p_سرخی=(ثابت انکڑے_سرخی *)وچ_انکڑے;
    إذا( (length>=0 && length<(int32_t)حجم(انکڑے_سرخی)) ||
        p_سرخی->انکڑے_سرخی_2.magic1!=0xda ||
        p_سرخی->انکڑے_سرخی_2.magic2!=0x27 ||
        p_سرخی->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    سرخی_ناپ=ds->readUInt16(p_سرخی->انکڑے_سرخی_2.سرخی_ناپ);
    info_ناپ=ds->readUInt16(p_سرخی->info.size);

    إذا( سرخی_ناپ<حجم(انکڑے_سرخی) ||
        info_ناپ<حجم(UDataInfo) ||
        سرخی_ناپ<(حجم(p_سرخی->انکڑے_سرخی_2)+info_ناپ) ||
        (length>=0 && length<سرخی_ناپ)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         سرخی_ناپ, info_ناپ, length);
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    إذا(length>0) {
        انکڑے_سرخی *باہرلا_سرخی;
        ثابت حرف *s;
        int32_t max_لمائی_2;

        /* Most of the fields are just bytes and need no swapping. */
        إذا(وچ_انکڑے!=باہرلا_انکڑے) {
            uprv_memcpy(باہرلا_انکڑے, وچ_انکڑے, سرخی_ناپ);
        }
        باہرلا_سرخی=(انکڑے_سرخی *)باہرلا_انکڑے;

        باہرلا_سرخی->info.isBigEndian = ds->باہرلا_Is_Big_Endian;
        باہرلا_سرخی->info.charsetFamily = ds->باہرلا_Charset;

        /* swap headerSize */
        ds->وٹاندرا_Array16(ds, &p_سرخی->انکڑے_سرخی_2.سرخی_ناپ, 2, &باہرلا_سرخی->انکڑے_سرخی_2.سرخی_ناپ, p_Error_کوڈ);

        /* swap UDataInfo size and reservedWord */
        ds->وٹاندرا_Array16(ds, &p_سرخی->info.size, 4, &باہرلا_سرخی->info.size, p_Error_کوڈ);

        /* swap copyright statement after the UDataInfo */
        info_ناپ+=حجم(p_سرخی->انکڑے_سرخی_2);
        s=(ثابت حرف *)وچ_انکڑے+info_ناپ;
        max_لمائی_2=سرخی_ناپ-info_ناپ;
        /* get the length of the string */
        لكل(length=0; length<max_لمائی_2 && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->وٹاندرا_Inv_Chars(ds, s, length, (حرف *)باہرلا_انکڑے+info_ناپ, p_Error_کوڈ);
    }

    أرجع سرخی_ناپ;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool وچ_Is_Big_Endian, uint8_t وچ_Charset,
                  UBool باہرلا_Is_Big_Endian, uint8_t باہرلا_Charset,
                  UError_کوڈ *p_Error_کوڈ) {
    UDataSwapper *swapper;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا(وچ_Charset>U_EBCDIC_FAMILY || باہرلا_Charset>U_EBCDIC_FAMILY) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(حجم(UDataSwapper));
    إذا(swapper==خالی_سوچک) {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    uprv_memset(swapper, 0, حجم(UDataSwapper));

    /* set values and functions pointers according to in/out parameters */
    swapper->وچ_Is_Big_Endian=وچ_Is_Big_Endian;
    swapper->وچ_Charset=وچ_Charset;
    swapper->باہرلا_Is_Big_Endian=باہرلا_Is_Big_Endian;
    swapper->باہرلا_Charset=باہرلا_Charset;

    swapper->readUInt16= وچ_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_سدھا_UInt16 : uprv_read_وٹاندرا_UInt16;
    swapper->readUInt32= وچ_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_سدھا_UInt32 : uprv_read_وٹاندرا_UInt32;

    swapper->writeUInt16= باہرلا_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_سدھا_UInt16 : uprv_write_وٹاندرا_UInt16;
    swapper->writeUInt32= باہرلا_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_سدھا_UInt32 : uprv_write_وٹاندرا_UInt32;

    swapper->compareInvChars= باہرلا_Charset==U_ASCII_FAMILY ? uprv_compareInvAscii : uprv_compareInvEbcdic;

    إذا(وچ_Is_Big_Endian==باہرلا_Is_Big_Endian) {
        swapper->وٹاندرا_Array16=uprv_copyArray16;
        swapper->وٹاندرا_Array32=uprv_copyArray32;
        swapper->وٹاندرا_Array64=uprv_copyArray64;
    } وإلا {
        swapper->وٹاندرا_Array16=uprv_وٹاندرا_Array16;
        swapper->وٹاندرا_Array32=uprv_وٹاندرا_Array32;
        swapper->وٹاندرا_Array64=uprv_وٹاندرا_Array64;
    }

    إذا(وچ_Charset==U_ASCII_FAMILY) {
        swapper->وٹاندرا_Inv_Chars= باہرلا_Charset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } وإلا /* U_EBCDIC_FAMILY */ {
        swapper->وٹاندرا_Inv_Chars= باہرلا_Charset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    أرجع swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_داخلہ_انکڑے(ثابت فراغ *انکڑے, int32_t length,
                              UBool باہرلا_Is_Big_Endian, uint8_t باہرلا_Charset,
                              UError_کوڈ *p_Error_کوڈ) {
    ثابت انکڑے_سرخی *p_سرخی;
    uint16_t سرخی_ناپ, info_ناپ;
    UBool وچ_Is_Big_Endian;
    int8_t وچ_Charset;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا( انکڑے==خالی_سوچک ||
        (length>=0 && length<(int32_t)حجم(انکڑے_سرخی)) ||
        باہرلا_Charset>U_EBCDIC_FAMILY
    ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    p_سرخی=(ثابت انکڑے_سرخی *)انکڑے;
    إذا( (length>=0 && length<(int32_t)حجم(انکڑے_سرخی)) ||
        p_سرخی->انکڑے_سرخی_2.magic1!=0xda ||
        p_سرخی->انکڑے_سرخی_2.magic2!=0x27 ||
        p_سرخی->info.sizeofUChar!=2
    ) {
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        أرجع خالی_سوچک;
    }

    وچ_Is_Big_Endian=(UBool)p_سرخی->info.isBigEndian;
    وچ_Charset=p_سرخی->info.charsetFamily;

    إذا(وچ_Is_Big_Endian==U_IS_BIG_ENDIAN) {
        سرخی_ناپ=p_سرخی->انکڑے_سرخی_2.سرخی_ناپ;
        info_ناپ=p_سرخی->info.size;
    } وإلا {
        سرخی_ناپ=uprv_read_وٹاندرا_UInt16(p_سرخی->انکڑے_سرخی_2.سرخی_ناپ);
        info_ناپ=uprv_read_وٹاندرا_UInt16(p_سرخی->info.size);
    }

    إذا( سرخی_ناپ<حجم(انکڑے_سرخی) ||
        info_ناپ<حجم(UDataInfo) ||
        سرخی_ناپ<(حجم(p_سرخی->انکڑے_سرخی_2)+info_ناپ) ||
        (length>=0 && length<سرخی_ناپ)
    ) {
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        أرجع خالی_سوچک;
    }

    أرجع udata_openSwapper(وچ_Is_Big_Endian, وچ_Charset, باہرلا_Is_Big_Endian, باہرلا_Charset, p_Error_کوڈ);
}

U_CAPI فراغ U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
