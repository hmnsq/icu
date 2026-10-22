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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h" /* UDataInfo */
#ਅਨ੍ਤਰ੍ਭਾਵ "ucmndata.h" /* DataHeader */
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

ਸਥਿਰ int32_t U_CALLCONV
uprv_ਅਦਲਾ_ਬਦਲੀ_Array16(ਅਚਲ UDataSwapper *ds,
                 ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || length<0 || (length&1)!=0 || ਬਾਹਰਲਾ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* setup and swapping */
    p = static_cast<ਅਚਲ uint16_t*>(ਵਿੱਚ_ਅੰਕੜੇ);
    q = static_cast<uint16_t*>(ਬਾਹਰਲਾ_ਅੰਕੜੇ);
    count=length/2;
    ਜਦੋਂ(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    ਵਾਪਸ length;
}

ਸਥਿਰ int32_t U_CALLCONV
uprv_copyArray16(ਅਚਲ UDataSwapper *ds,
                 ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || length<0 || (length&1)!=0 || ਬਾਹਰਲਾ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(length>0 && ਵਿੱਚ_ਅੰਕੜੇ!=ਬਾਹਰਲਾ_ਅੰਕੜੇ) {
        uprv_memcpy(ਬਾਹਰਲਾ_ਅੰਕੜੇ, ਵਿੱਚ_ਅੰਕੜੇ, length);
    }
    ਵਾਪਸ length;
}

ਸਥਿਰ int32_t U_CALLCONV
uprv_ਅਦਲਾ_ਬਦਲੀ_Array32(ਅਚਲ UDataSwapper *ds,
                 ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || length<0 || (length&3)!=0 || ਬਾਹਰਲਾ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* setup and swapping */
    p = static_cast<ਅਚਲ uint32_t*>(ਵਿੱਚ_ਅੰਕੜੇ);
    q = static_cast<uint32_t*>(ਬਾਹਰਲਾ_ਅੰਕੜੇ);
    count=length/4;
    ਜਦੋਂ(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    ਵਾਪਸ length;
}

ਸਥਿਰ int32_t U_CALLCONV
uprv_copyArray32(ਅਚਲ UDataSwapper *ds,
                 ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || length<0 || (length&3)!=0 || ਬਾਹਰਲਾ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(length>0 && ਵਿੱਚ_ਅੰਕੜੇ!=ਬਾਹਰਲਾ_ਅੰਕੜੇ) {
        uprv_memcpy(ਬਾਹਰਲਾ_ਅੰਕੜੇ, ਵਿੱਚ_ਅੰਕੜੇ, length);
    }
    ਵਾਪਸ length;
}

ਸਥਿਰ int32_t U_CALLCONV
uprv_ਅਦਲਾ_ਬਦਲੀ_Array64(ਅਚਲ UDataSwapper *ds,
                 ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ uint64_t *p;
    uint64_t *q;
    int32_t count;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || length<0 || (length&7)!=0 || ਬਾਹਰਲਾ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* setup and swapping */
    p = static_cast<ਅਚਲ uint64_t*>(ਵਿੱਚ_ਅੰਕੜੇ);
    q = static_cast<uint64_t*>(ਬਾਹਰਲਾ_ਅੰਕੜੇ);
    count=length/8;
    ਜਦੋਂ(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    ਵਾਪਸ length;
}

ਸਥਿਰ int32_t U_CALLCONV
uprv_copyArray64(ਅਚਲ UDataSwapper *ds,
                 ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || length<0 || (length&7)!=0 || ਬਾਹਰਲਾ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(length>0 && ਵਿੱਚ_ਅੰਕੜੇ!=ਬਾਹਰਲਾ_ਅੰਕੜੇ) {
        uprv_memcpy(ਬਾਹਰਲਾ_ਅੰਕੜੇ, ਵਿੱਚ_ਅੰਕੜੇ, length);
    }
    ਵਾਪਸ length;
}

ਸਥਿਰ uint16_t U_CALLCONV
uprv_read_ਅਦਲਾ_ਬਦਲੀ_UInt16(uint16_t x) {
    ਵਾਪਸ static_cast<uint16_t>((x << 8) | (x >> 8));
}

ਸਥਿਰ uint16_t U_CALLCONV
uprv_read_ਸਿੱਧਾ_UInt16(uint16_t x) {
    ਵਾਪਸ x;
}

ਸਥਿਰ uint32_t U_CALLCONV
uprv_read_ਅਦਲਾ_ਬਦਲੀ_UInt32(uint32_t x) {
    ਵਾਪਸ (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ਸਥਿਰ uint32_t U_CALLCONV
uprv_read_ਸਿੱਧਾ_UInt32(uint32_t x) {
    ਵਾਪਸ x;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
uprv_write_ਅਦਲਾ_ਬਦਲੀ_UInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
uprv_write_ਸਿੱਧਾ_UInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
uprv_write_ਅਦਲਾ_ਬਦਲੀ_UInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
uprv_write_ਸਿੱਧਾ_UInt32(uint32_t *p, uint32_t x) {
    *p=x;
}

U_CAPI int16_t U_EXPORT2
udata_readInt16(ਅਚਲ UDataSwapper *ds, int16_t x) {
    ਵਾਪਸ (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(ਅਚਲ UDataSwapper *ds, int32_t x) {
    ਵਾਪਸ (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_ਅਦਲਾ_ਬਦਲੀ_Inv_String_ਖੰਡ(ਅਚਲ UDataSwapper *ds,
                         ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                         UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ ਅੱਖਰ *ਵਿੱਚ_Chars;
    int32_t ਲੜੀਆਂ_ਲੰਮਾਈ;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || length<0 || (length>0 && ਬਾਹਰਲਾ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    ਵਿੱਚ_Chars=(ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ;
    ਲੜੀਆਂ_ਲੰਮਾਈ=length;
    ਜਦੋਂ(ਲੜੀਆਂ_ਲੰਮਾਈ>0 && ਵਿੱਚ_Chars[ਲੜੀਆਂ_ਲੰਮਾਈ-1]!=0) {
        --ਲੜੀਆਂ_ਲੰਮਾਈ;
    }

    /* swap up to the last NUL */
    ds->ਅਦਲਾ_ਬਦਲੀ_Inv_Chars(ds, ਵਿੱਚ_ਅੰਕੜੇ, ਲੜੀਆਂ_ਲੰਮਾਈ, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);

    /* copy the bytes after the last NUL */
    ਜੇ(ਵਿੱਚ_ਅੰਕੜੇ!=ਬਾਹਰਲਾ_ਅੰਕੜੇ && length>ਲੜੀਆਂ_ਲੰਮਾਈ) {
        uprv_memcpy((ਅੱਖਰ *)ਬਾਹਰਲਾ_ਅੰਕੜੇ+ਲੜੀਆਂ_ਲੰਮਾਈ, ਵਿੱਚ_Chars+ਲੜੀਆਂ_ਲੰਮਾਈ, length-ਲੜੀਆਂ_ਲੰਮਾਈ);
    }

    /* return the length including padding bytes */
    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ length;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
udata_printError(ਅਚਲ UDataSwapper *ds,
                 ਅਚਲ ਅੱਖਰ *fmt,
                 ...) {
    va_list ਦਲੀਲਾਂ_2;

    ਜੇ(ds->printError!=ਖਾਲੀ_ਸੂਚਕ) {
        va_start(ਦਲੀਲਾਂ_2, fmt);
        ds->printError(ds->printErrorContext, fmt, ਦਲੀਲਾਂ_2);
        va_end(ਦਲੀਲਾਂ_2);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ਅਚਲ UDataSwapper *ds,
                     ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                     UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *p_ਸਿਰਲੇਖ;
    uint16_t ਸਿਰਲੇਖ_ਮਾਪ, info_ਮਾਪ;

    /* argument checking */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ds==ਖਾਲੀ_ਸੂਚਕ || ਵਿੱਚ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || length<-1 || (length>0 && ਬਾਹਰਲਾ_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* check minimum length and magic bytes */
    p_ਸਿਰਲੇਖ=(ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *)ਵਿੱਚ_ਅੰਕੜੇ;
    ਜੇ( (length>=0 && length<(int32_t)ਆਕਾਰ(ਅੰਕੜੇ_ਸਿਰਲੇਖ)) ||
        p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic1!=0xda ||
        p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic2!=0x27 ||
        p_ਸਿਰਲੇਖ->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    ਸਿਰਲੇਖ_ਮਾਪ=ds->readUInt16(p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ);
    info_ਮਾਪ=ds->readUInt16(p_ਸਿਰਲੇਖ->info.size);

    ਜੇ( ਸਿਰਲੇਖ_ਮਾਪ<ਆਕਾਰ(ਅੰਕੜੇ_ਸਿਰਲੇਖ) ||
        info_ਮਾਪ<ਆਕਾਰ(UDataInfo) ||
        ਸਿਰਲੇਖ_ਮਾਪ<(ਆਕਾਰ(p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2)+info_ਮਾਪ) ||
        (length>=0 && length<ਸਿਰਲੇਖ_ਮਾਪ)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         ਸਿਰਲੇਖ_ਮਾਪ, info_ਮਾਪ, length);
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(length>0) {
        ਅੰਕੜੇ_ਸਿਰਲੇਖ *ਬਾਹਰਲਾ_ਸਿਰਲੇਖ;
        ਅਚਲ ਅੱਖਰ *s;
        int32_t max_ਲੰਮਾਈ_2;

        /* Most of the fields are just bytes and need no swapping. */
        ਜੇ(ਵਿੱਚ_ਅੰਕੜੇ!=ਬਾਹਰਲਾ_ਅੰਕੜੇ) {
            uprv_memcpy(ਬਾਹਰਲਾ_ਅੰਕੜੇ, ਵਿੱਚ_ਅੰਕੜੇ, ਸਿਰਲੇਖ_ਮਾਪ);
        }
        ਬਾਹਰਲਾ_ਸਿਰਲੇਖ=(ਅੰਕੜੇ_ਸਿਰਲੇਖ *)ਬਾਹਰਲਾ_ਅੰਕੜੇ;

        ਬਾਹਰਲਾ_ਸਿਰਲੇਖ->info.isBigEndian = ds->ਬਾਹਰਲਾ_Is_Big_Endian;
        ਬਾਹਰਲਾ_ਸਿਰਲੇਖ->info.charsetFamily = ds->ਬਾਹਰਲਾ_Charset;

        /* swap headerSize */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, &p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ, 2, &ਬਾਹਰਲਾ_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ, p_Error_ਕੋਡ);

        /* swap UDataInfo size and reservedWord */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, &p_ਸਿਰਲੇਖ->info.size, 4, &ਬਾਹਰਲਾ_ਸਿਰਲੇਖ->info.size, p_Error_ਕੋਡ);

        /* swap copyright statement after the UDataInfo */
        info_ਮਾਪ+=ਆਕਾਰ(p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2);
        s=(ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+info_ਮਾਪ;
        max_ਲੰਮਾਈ_2=ਸਿਰਲੇਖ_ਮਾਪ-info_ਮਾਪ;
        /* get the length of the string */
        ਲਈ(length=0; length<max_ਲੰਮਾਈ_2 && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->ਅਦਲਾ_ਬਦਲੀ_Inv_Chars(ds, s, length, (ਅੱਖਰ *)ਬਾਹਰਲਾ_ਅੰਕੜੇ+info_ਮਾਪ, p_Error_ਕੋਡ);
    }

    ਵਾਪਸ ਸਿਰਲੇਖ_ਮਾਪ;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool ਵਿੱਚ_Is_Big_Endian, uint8_t ਵਿੱਚ_Charset,
                  UBool ਬਾਹਰਲਾ_Is_Big_Endian, uint8_t ਬਾਹਰਲਾ_Charset,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UDataSwapper *swapper;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(ਵਿੱਚ_Charset>U_EBCDIC_FAMILY || ਬਾਹਰਲਾ_Charset>U_EBCDIC_FAMILY) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(ਆਕਾਰ(UDataSwapper));
    ਜੇ(swapper==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_memset(swapper, 0, ਆਕਾਰ(UDataSwapper));

    /* set values and functions pointers according to in/out parameters */
    swapper->ਵਿੱਚ_Is_Big_Endian=ਵਿੱਚ_Is_Big_Endian;
    swapper->ਵਿੱਚ_Charset=ਵਿੱਚ_Charset;
    swapper->ਬਾਹਰਲਾ_Is_Big_Endian=ਬਾਹਰਲਾ_Is_Big_Endian;
    swapper->ਬਾਹਰਲਾ_Charset=ਬਾਹਰਲਾ_Charset;

    swapper->readUInt16= ਵਿੱਚ_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_ਸਿੱਧਾ_UInt16 : uprv_read_ਅਦਲਾ_ਬਦਲੀ_UInt16;
    swapper->readUInt32= ਵਿੱਚ_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_ਸਿੱਧਾ_UInt32 : uprv_read_ਅਦਲਾ_ਬਦਲੀ_UInt32;

    swapper->writeUInt16= ਬਾਹਰਲਾ_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_ਸਿੱਧਾ_UInt16 : uprv_write_ਅਦਲਾ_ਬਦਲੀ_UInt16;
    swapper->writeUInt32= ਬਾਹਰਲਾ_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_ਸਿੱਧਾ_UInt32 : uprv_write_ਅਦਲਾ_ਬਦਲੀ_UInt32;

    swapper->compareInvChars= ਬਾਹਰਲਾ_Charset==U_ASCII_FAMILY ? uprv_compareInvAscii : uprv_compareInvEbcdic;

    ਜੇ(ਵਿੱਚ_Is_Big_Endian==ਬਾਹਰਲਾ_Is_Big_Endian) {
        swapper->ਅਦਲਾ_ਬਦਲੀ_Array16=uprv_copyArray16;
        swapper->ਅਦਲਾ_ਬਦਲੀ_Array32=uprv_copyArray32;
        swapper->ਅਦਲਾ_ਬਦਲੀ_Array64=uprv_copyArray64;
    } ਨਹੀਂਤਾਂ {
        swapper->ਅਦਲਾ_ਬਦਲੀ_Array16=uprv_ਅਦਲਾ_ਬਦਲੀ_Array16;
        swapper->ਅਦਲਾ_ਬਦਲੀ_Array32=uprv_ਅਦਲਾ_ਬਦਲੀ_Array32;
        swapper->ਅਦਲਾ_ਬਦਲੀ_Array64=uprv_ਅਦਲਾ_ਬਦਲੀ_Array64;
    }

    ਜੇ(ਵਿੱਚ_Charset==U_ASCII_FAMILY) {
        swapper->ਅਦਲਾ_ਬਦਲੀ_Inv_Chars= ਬਾਹਰਲਾ_Charset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } ਨਹੀਂਤਾਂ /* U_EBCDIC_FAMILY */ {
        swapper->ਅਦਲਾ_ਬਦਲੀ_Inv_Chars= ਬਾਹਰਲਾ_Charset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    ਵਾਪਸ swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_ਨਿਵੇਸ਼_ਅੰਕੜੇ(ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, int32_t length,
                              UBool ਬਾਹਰਲਾ_Is_Big_Endian, uint8_t ਬਾਹਰਲਾ_Charset,
                              UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *p_ਸਿਰਲੇਖ;
    uint16_t ਸਿਰਲੇਖ_ਮਾਪ, info_ਮਾਪ;
    UBool ਵਿੱਚ_Is_Big_Endian;
    int8_t ਵਿੱਚ_Charset;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ( ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ ||
        (length>=0 && length<(int32_t)ਆਕਾਰ(ਅੰਕੜੇ_ਸਿਰਲੇਖ)) ||
        ਬਾਹਰਲਾ_Charset>U_EBCDIC_FAMILY
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    p_ਸਿਰਲੇਖ=(ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *)ਅੰਕੜੇ;
    ਜੇ( (length>=0 && length<(int32_t)ਆਕਾਰ(ਅੰਕੜੇ_ਸਿਰਲੇਖ)) ||
        p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic1!=0xda ||
        p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic2!=0x27 ||
        p_ਸਿਰਲੇਖ->info.sizeofUChar!=2
    ) {
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਵਿੱਚ_Is_Big_Endian=(UBool)p_ਸਿਰਲੇਖ->info.isBigEndian;
    ਵਿੱਚ_Charset=p_ਸਿਰਲੇਖ->info.charsetFamily;

    ਜੇ(ਵਿੱਚ_Is_Big_Endian==U_IS_BIG_ENDIAN) {
        ਸਿਰਲੇਖ_ਮਾਪ=p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ;
        info_ਮਾਪ=p_ਸਿਰਲੇਖ->info.size;
    } ਨਹੀਂਤਾਂ {
        ਸਿਰਲੇਖ_ਮਾਪ=uprv_read_ਅਦਲਾ_ਬਦਲੀ_UInt16(p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.ਸਿਰਲੇਖ_ਮਾਪ);
        info_ਮਾਪ=uprv_read_ਅਦਲਾ_ਬਦਲੀ_UInt16(p_ਸਿਰਲੇਖ->info.size);
    }

    ਜੇ( ਸਿਰਲੇਖ_ਮਾਪ<ਆਕਾਰ(ਅੰਕੜੇ_ਸਿਰਲੇਖ) ||
        info_ਮਾਪ<ਆਕਾਰ(UDataInfo) ||
        ਸਿਰਲੇਖ_ਮਾਪ<(ਆਕਾਰ(p_ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2)+info_ਮਾਪ) ||
        (length>=0 && length<ਸਿਰਲੇਖ_ਮਾਪ)
    ) {
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਵਾਪਸ udata_openSwapper(ਵਿੱਚ_Is_Big_Endian, ਵਿੱਚ_Charset, ਬਾਹਰਲਾ_Is_Big_Endian, ਬਾਹਰਲਾ_Charset, p_Error_ਕੋਡ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
