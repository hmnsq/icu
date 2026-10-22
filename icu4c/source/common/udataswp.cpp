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

#شامل <stdarg.h>
#شامل "unicode/utypes.h"
#شامل "unicode/udata.h" /* UDataInfo */
#شامل "ucmndata.h" /* DataHeader */
#شامل "cmemory.h"
#شامل "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

ساکن int32_t U_CALLCONV
uprv_تبادلہ_Array16(ثابت UDataSwapper *ds,
                 ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                 UError_رمز *p_Error_رمز) {
    ثابت uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || length<0 || (length&1)!=0 || برآمد_ڈیٹا==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint16_t*>(اندر_ڈیٹا);
    q = static_cast<uint16_t*>(برآمد_ڈیٹا);
    count=length/2;
    جب(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    واپس length;
}

ساکن int32_t U_CALLCONV
uprv_copyArray16(ثابت UDataSwapper *ds,
                 ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                 UError_رمز *p_Error_رمز) {
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || length<0 || (length&1)!=0 || برآمد_ڈیٹا==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    اگر(length>0 && اندر_ڈیٹا!=برآمد_ڈیٹا) {
        uprv_memcpy(برآمد_ڈیٹا, اندر_ڈیٹا, length);
    }
    واپس length;
}

ساکن int32_t U_CALLCONV
uprv_تبادلہ_Array32(ثابت UDataSwapper *ds,
                 ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                 UError_رمز *p_Error_رمز) {
    ثابت uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || length<0 || (length&3)!=0 || برآمد_ڈیٹا==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint32_t*>(اندر_ڈیٹا);
    q = static_cast<uint32_t*>(برآمد_ڈیٹا);
    count=length/4;
    جب(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    واپس length;
}

ساکن int32_t U_CALLCONV
uprv_copyArray32(ثابت UDataSwapper *ds,
                 ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                 UError_رمز *p_Error_رمز) {
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || length<0 || (length&3)!=0 || برآمد_ڈیٹا==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    اگر(length>0 && اندر_ڈیٹا!=برآمد_ڈیٹا) {
        uprv_memcpy(برآمد_ڈیٹا, اندر_ڈیٹا, length);
    }
    واپس length;
}

ساکن int32_t U_CALLCONV
uprv_تبادلہ_Array64(ثابت UDataSwapper *ds,
                 ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                 UError_رمز *p_Error_رمز) {
    ثابت uint64_t *p;
    uint64_t *q;
    int32_t count;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || length<0 || (length&7)!=0 || برآمد_ڈیٹا==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* setup and swapping */
    p = static_cast<ثابت uint64_t*>(اندر_ڈیٹا);
    q = static_cast<uint64_t*>(برآمد_ڈیٹا);
    count=length/8;
    جب(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    واپس length;
}

ساکن int32_t U_CALLCONV
uprv_copyArray64(ثابت UDataSwapper *ds,
                 ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                 UError_رمز *p_Error_رمز) {
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || length<0 || (length&7)!=0 || برآمد_ڈیٹا==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    اگر(length>0 && اندر_ڈیٹا!=برآمد_ڈیٹا) {
        uprv_memcpy(برآمد_ڈیٹا, اندر_ڈیٹا, length);
    }
    واپس length;
}

ساکن uint16_t U_CALLCONV
uprv_read_تبادلہ_UInt16(uint16_t x) {
    واپس static_cast<uint16_t>((x << 8) | (x >> 8));
}

ساکن uint16_t U_CALLCONV
uprv_read_براہ_راست_UInt16(uint16_t x) {
    واپس x;
}

ساکن uint32_t U_CALLCONV
uprv_read_تبادلہ_UInt32(uint32_t x) {
    واپس (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ساکن uint32_t U_CALLCONV
uprv_read_براہ_راست_UInt32(uint32_t x) {
    واپس x;
}

ساکن خالی U_CALLCONV
uprv_write_تبادلہ_UInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

ساکن خالی U_CALLCONV
uprv_write_براہ_راست_UInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

ساکن خالی U_CALLCONV
uprv_write_تبادلہ_UInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ساکن خالی U_CALLCONV
uprv_write_براہ_راست_UInt32(uint32_t *p, uint32_t x) {
    *p=x;
}

U_CAPI int16_t U_EXPORT2
udata_readInt16(ثابت UDataSwapper *ds, int16_t x) {
    واپس (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(ثابت UDataSwapper *ds, int32_t x) {
    واپس (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_تبادلہ_Inv_String_بلاک(ثابت UDataSwapper *ds,
                         ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                         UError_رمز *p_Error_رمز) {
    ثابت حرف *اندر_Chars;
    int32_t لڑیاں_لمبائی;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || length<0 || (length>0 && برآمد_ڈیٹا==خالی_اشاریہ)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    اندر_Chars=(ثابت حرف *)اندر_ڈیٹا;
    لڑیاں_لمبائی=length;
    جب(لڑیاں_لمبائی>0 && اندر_Chars[لڑیاں_لمبائی-1]!=0) {
        --لڑیاں_لمبائی;
    }

    /* swap up to the last NUL */
    ds->تبادلہ_Inv_Chars(ds, اندر_ڈیٹا, لڑیاں_لمبائی, برآمد_ڈیٹا, p_Error_رمز);

    /* copy the bytes after the last NUL */
    اگر(اندر_ڈیٹا!=برآمد_ڈیٹا && length>لڑیاں_لمبائی) {
        uprv_memcpy((حرف *)برآمد_ڈیٹا+لڑیاں_لمبائی, اندر_Chars+لڑیاں_لمبائی, length-لڑیاں_لمبائی);
    }

    /* return the length including padding bytes */
    اگر(U_SUCCESS(*p_Error_رمز)) {
        واپس length;
    } ورنہ {
        واپس 0;
    }
}

U_CAPI خالی U_EXPORT2
udata_printError(ثابت UDataSwapper *ds,
                 ثابت حرف *fmt,
                 ...) {
    va_list دلائل_2;

    اگر(ds->printError!=خالی_اشاریہ) {
        va_start(دلائل_2, fmt);
        ds->printError(ds->printErrorContext, fmt, دلائل_2);
        va_end(دلائل_2);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_تبادلہ_ڈیٹا_سرخی(ثابت UDataSwapper *ds,
                     ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                     UError_رمز *p_Error_رمز) {
    ثابت ڈیٹا_سرخی *p_سرخی;
    uint16_t سرخی_سائز, info_سائز;

    /* argument checking */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(ds==خالی_اشاریہ || اندر_ڈیٹا==خالی_اشاریہ || length<-1 || (length>0 && برآمد_ڈیٹا==خالی_اشاریہ)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* check minimum length and magic bytes */
    p_سرخی=(ثابت ڈیٹا_سرخی *)اندر_ڈیٹا;
    اگر( (length>=0 && length<(int32_t)حجم(ڈیٹا_سرخی)) ||
        p_سرخی->ڈیٹا_سرخی_2.magic1!=0xda ||
        p_سرخی->ڈیٹا_سرخی_2.magic2!=0x27 ||
        p_سرخی->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    سرخی_سائز=ds->readUInt16(p_سرخی->ڈیٹا_سرخی_2.سرخی_سائز);
    info_سائز=ds->readUInt16(p_سرخی->info.size);

    اگر( سرخی_سائز<حجم(ڈیٹا_سرخی) ||
        info_سائز<حجم(UDataInfo) ||
        سرخی_سائز<(حجم(p_سرخی->ڈیٹا_سرخی_2)+info_سائز) ||
        (length>=0 && length<سرخی_سائز)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         سرخی_سائز, info_سائز, length);
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }

    اگر(length>0) {
        ڈیٹا_سرخی *برآمد_سرخی;
        ثابت حرف *s;
        int32_t max_لمبائی_2;

        /* Most of the fields are just bytes and need no swapping. */
        اگر(اندر_ڈیٹا!=برآمد_ڈیٹا) {
            uprv_memcpy(برآمد_ڈیٹا, اندر_ڈیٹا, سرخی_سائز);
        }
        برآمد_سرخی=(ڈیٹا_سرخی *)برآمد_ڈیٹا;

        برآمد_سرخی->info.isBigEndian = ds->برآمد_Is_Big_Endian;
        برآمد_سرخی->info.charsetFamily = ds->برآمد_Charset;

        /* swap headerSize */
        ds->تبادلہ_Array16(ds, &p_سرخی->ڈیٹا_سرخی_2.سرخی_سائز, 2, &برآمد_سرخی->ڈیٹا_سرخی_2.سرخی_سائز, p_Error_رمز);

        /* swap UDataInfo size and reservedWord */
        ds->تبادلہ_Array16(ds, &p_سرخی->info.size, 4, &برآمد_سرخی->info.size, p_Error_رمز);

        /* swap copyright statement after the UDataInfo */
        info_سائز+=حجم(p_سرخی->ڈیٹا_سرخی_2);
        s=(ثابت حرف *)اندر_ڈیٹا+info_سائز;
        max_لمبائی_2=سرخی_سائز-info_سائز;
        /* get the length of the string */
        برائے(length=0; length<max_لمبائی_2 && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->تبادلہ_Inv_Chars(ds, s, length, (حرف *)برآمد_ڈیٹا+info_سائز, p_Error_رمز);
    }

    واپس سرخی_سائز;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool اندر_Is_Big_Endian, uint8_t اندر_Charset,
                  UBool برآمد_Is_Big_Endian, uint8_t برآمد_Charset,
                  UError_رمز *p_Error_رمز) {
    UDataSwapper *swapper;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر(اندر_Charset>U_EBCDIC_FAMILY || برآمد_Charset>U_EBCDIC_FAMILY) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(حجم(UDataSwapper));
    اگر(swapper==خالی_اشاریہ) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    uprv_memset(swapper, 0, حجم(UDataSwapper));

    /* set values and functions pointers according to in/out parameters */
    swapper->اندر_Is_Big_Endian=اندر_Is_Big_Endian;
    swapper->اندر_Charset=اندر_Charset;
    swapper->برآمد_Is_Big_Endian=برآمد_Is_Big_Endian;
    swapper->برآمد_Charset=برآمد_Charset;

    swapper->readUInt16= اندر_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_براہ_راست_UInt16 : uprv_read_تبادلہ_UInt16;
    swapper->readUInt32= اندر_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_براہ_راست_UInt32 : uprv_read_تبادلہ_UInt32;

    swapper->writeUInt16= برآمد_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_براہ_راست_UInt16 : uprv_write_تبادلہ_UInt16;
    swapper->writeUInt32= برآمد_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_براہ_راست_UInt32 : uprv_write_تبادلہ_UInt32;

    swapper->compareInvChars= برآمد_Charset==U_ASCII_FAMILY ? uprv_compareInvAscii : uprv_compareInvEbcdic;

    اگر(اندر_Is_Big_Endian==برآمد_Is_Big_Endian) {
        swapper->تبادلہ_Array16=uprv_copyArray16;
        swapper->تبادلہ_Array32=uprv_copyArray32;
        swapper->تبادلہ_Array64=uprv_copyArray64;
    } ورنہ {
        swapper->تبادلہ_Array16=uprv_تبادلہ_Array16;
        swapper->تبادلہ_Array32=uprv_تبادلہ_Array32;
        swapper->تبادلہ_Array64=uprv_تبادلہ_Array64;
    }

    اگر(اندر_Charset==U_ASCII_FAMILY) {
        swapper->تبادلہ_Inv_Chars= برآمد_Charset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } ورنہ /* U_EBCDIC_FAMILY */ {
        swapper->تبادلہ_Inv_Chars= برآمد_Charset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    واپس swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_داخلہ_ڈیٹا(ثابت خالی *ڈیٹا, int32_t length,
                              UBool برآمد_Is_Big_Endian, uint8_t برآمد_Charset,
                              UError_رمز *p_Error_رمز) {
    ثابت ڈیٹا_سرخی *p_سرخی;
    uint16_t سرخی_سائز, info_سائز;
    UBool اندر_Is_Big_Endian;
    int8_t اندر_Charset;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر( ڈیٹا==خالی_اشاریہ ||
        (length>=0 && length<(int32_t)حجم(ڈیٹا_سرخی)) ||
        برآمد_Charset>U_EBCDIC_FAMILY
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    p_سرخی=(ثابت ڈیٹا_سرخی *)ڈیٹا;
    اگر( (length>=0 && length<(int32_t)حجم(ڈیٹا_سرخی)) ||
        p_سرخی->ڈیٹا_سرخی_2.magic1!=0xda ||
        p_سرخی->ڈیٹا_سرخی_2.magic2!=0x27 ||
        p_سرخی->info.sizeofUChar!=2
    ) {
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس خالی_اشاریہ;
    }

    اندر_Is_Big_Endian=(UBool)p_سرخی->info.isBigEndian;
    اندر_Charset=p_سرخی->info.charsetFamily;

    اگر(اندر_Is_Big_Endian==U_IS_BIG_ENDIAN) {
        سرخی_سائز=p_سرخی->ڈیٹا_سرخی_2.سرخی_سائز;
        info_سائز=p_سرخی->info.size;
    } ورنہ {
        سرخی_سائز=uprv_read_تبادلہ_UInt16(p_سرخی->ڈیٹا_سرخی_2.سرخی_سائز);
        info_سائز=uprv_read_تبادلہ_UInt16(p_سرخی->info.size);
    }

    اگر( سرخی_سائز<حجم(ڈیٹا_سرخی) ||
        info_سائز<حجم(UDataInfo) ||
        سرخی_سائز<(حجم(p_سرخی->ڈیٹا_سرخی_2)+info_سائز) ||
        (length>=0 && length<سرخی_سائز)
    ) {
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس خالی_اشاریہ;
    }

    واپس udata_openSwapper(اندر_Is_Big_Endian, اندر_Charset, برآمد_Is_Big_Endian, برآمد_Charset, p_Error_رمز);
}

U_CAPI خالی U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
