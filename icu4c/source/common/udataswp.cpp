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

#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h" /* UDataInfo */
#ಅನ್ತರ್ಭಾವ "ucmndata.h" /* DataHeader */
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"

/* swapping primitives ------------------------------------------------------ */

ಸ್ಥಿರ int32_t U_CALLCONV
uprv_ಅದಲುಬದಲು_Array16(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ uint16_t *p;
    uint16_t *q;
    int32_t count;
    uint16_t x;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || length<0 || (length&1)!=0 || ಹೊರಹೋಗು_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* setup and swapping */
    p = static_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(ಒಳ_ದತ್ತಾಂಶ);
    q = static_cast<uint16_t*>(ಹೊರಹೋಗು_ದತ್ತಾಂಶ);
    count=length/2;
    ಆಗ(count>0) {
        x=*p++;
        *q++ = static_cast<uint16_t>((x << 8) | (x >> 8));
        --count;
    }

    ಹಿಂತಿರುಗಿ length;
}

ಸ್ಥಿರ int32_t U_CALLCONV
uprv_copyArray16(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || length<0 || (length&1)!=0 || ಹೊರಹೋಗು_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(length>0 && ಒಳ_ದತ್ತಾಂಶ!=ಹೊರಹೋಗು_ದತ್ತಾಂಶ) {
        uprv_memcpy(ಹೊರಹೋಗು_ದತ್ತಾಂಶ, ಒಳ_ದತ್ತಾಂಶ, length);
    }
    ಹಿಂತಿರುಗಿ length;
}

ಸ್ಥಿರ int32_t U_CALLCONV
uprv_ಅದಲುಬದಲು_Array32(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ uint32_t *p;
    uint32_t *q;
    int32_t count;
    uint32_t x;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || length<0 || (length&3)!=0 || ಹೊರಹೋಗು_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* setup and swapping */
    p = static_cast<ಸ್ಥಿರಾಂಕ uint32_t*>(ಒಳ_ದತ್ತಾಂಶ);
    q = static_cast<uint32_t*>(ಹೊರಹೋಗು_ದತ್ತಾಂಶ);
    count=length/4;
    ಆಗ(count>0) {
        x=*p++;
        *q++ = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
        --count;
    }

    ಹಿಂತಿರುಗಿ length;
}

ಸ್ಥಿರ int32_t U_CALLCONV
uprv_copyArray32(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || length<0 || (length&3)!=0 || ಹೊರಹೋಗು_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(length>0 && ಒಳ_ದತ್ತಾಂಶ!=ಹೊರಹೋಗು_ದತ್ತಾಂಶ) {
        uprv_memcpy(ಹೊರಹೋಗು_ದತ್ತಾಂಶ, ಒಳ_ದತ್ತಾಂಶ, length);
    }
    ಹಿಂತಿರುಗಿ length;
}

ಸ್ಥಿರ int32_t U_CALLCONV
uprv_ಅದಲುಬದಲು_Array64(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ uint64_t *p;
    uint64_t *q;
    int32_t count;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || length<0 || (length&7)!=0 || ಹೊರಹೋಗು_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* setup and swapping */
    p = static_cast<ಸ್ಥಿರಾಂಕ uint64_t*>(ಒಳ_ದತ್ತಾಂಶ);
    q = static_cast<uint64_t*>(ಹೊರಹೋಗು_ದತ್ತಾಂಶ);
    count=length/8;
    ಆಗ(count>0) {
        uint64_t x=*p++;
        x=(x<<56)|((x&0xff00)<<40)|((x&0xff0000)<<24)|((x&0xff000000)<<8)|
            ((x>>8)&0xff000000)|((x>>24)&0xff0000)|((x>>40)&0xff00)|(x>>56);
        *q++=x;
        --count;
    }

    ಹಿಂತಿರುಗಿ length;
}

ಸ್ಥಿರ int32_t U_CALLCONV
uprv_copyArray64(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || length<0 || (length&7)!=0 || ಹೊರಹೋಗು_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(length>0 && ಒಳ_ದತ್ತಾಂಶ!=ಹೊರಹೋಗು_ದತ್ತಾಂಶ) {
        uprv_memcpy(ಹೊರಹೋಗು_ದತ್ತಾಂಶ, ಒಳ_ದತ್ತಾಂಶ, length);
    }
    ಹಿಂತಿರುಗಿ length;
}

ಸ್ಥಿರ uint16_t U_CALLCONV
uprv_read_ಅದಲುಬದಲು_UInt16(uint16_t x) {
    ಹಿಂತಿರುಗಿ static_cast<uint16_t>((x << 8) | (x >> 8));
}

ಸ್ಥಿರ uint16_t U_CALLCONV
uprv_read_ನೇರ_UInt16(uint16_t x) {
    ಹಿಂತಿರುಗಿ x;
}

ಸ್ಥಿರ uint32_t U_CALLCONV
uprv_read_ಅದಲುಬದಲು_UInt32(uint32_t x) {
    ಹಿಂತಿರುಗಿ (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ಸ್ಥಿರ uint32_t U_CALLCONV
uprv_read_ನೇರ_UInt32(uint32_t x) {
    ಹಿಂತಿರುಗಿ x;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
uprv_write_ಅದಲುಬದಲು_UInt16(uint16_t *p, uint16_t x) {
    *p = static_cast<uint16_t>((x << 8) | (x >> 8));
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
uprv_write_ನೇರ_UInt16(uint16_t *p, uint16_t x) {
    *p=x;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
uprv_write_ಅದಲುಬದಲು_UInt32(uint32_t *p, uint32_t x) {
    *p = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
uprv_write_ನೇರ_UInt32(uint32_t *p, uint32_t x) {
    *p=x;
}

U_CAPI int16_t U_EXPORT2
udata_readInt16(ಸ್ಥಿರಾಂಕ UDataSwapper *ds, int16_t x) {
    ಹಿಂತಿರುಗಿ (int16_t)ds->readUInt16((uint16_t)x);
}

U_CAPI int32_t U_EXPORT2
udata_readInt32(ಸ್ಥಿರಾಂಕ UDataSwapper *ds, int32_t x) {
    ಹಿಂತಿರುಗಿ (int32_t)ds->readUInt32((uint32_t)x);
}

/**
 * Swap a block of invariant, NUL-terminated strings, but not padding
 * bytes after the last string.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
udata_ಅದಲುಬದಲು_Inv_String_ಖಂಡ(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                         ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                         UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳ_Chars;
    int32_t ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || length<0 || (length>0 && ಹೊರಹೋಗು_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* reduce the strings length to not include bytes after the last NUL */
    ಒಳ_Chars=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ;
    ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ=length;
    ಆಗ(ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ>0 && ಒಳ_Chars[ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ-1]!=0) {
        --ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ;
    }

    /* swap up to the last NUL */
    ds->ಅದಲುಬದಲು_Inv_Chars(ds, ಒಳ_ದತ್ತಾಂಶ, ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);

    /* copy the bytes after the last NUL */
    ಆದರೆ(ಒಳ_ದತ್ತಾಂಶ!=ಹೊರಹೋಗು_ದತ್ತಾಂಶ && length>ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ) {
        uprv_memcpy((ಅಕ್ಷರ *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ+ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ, ಒಳ_Chars+ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ, length-ಅಕ್ಷರಸರಣಿಗಳು_ಉದ್ದಳತೆ);
    }

    /* return the length including padding bytes */
    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ length;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
udata_printError(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt,
                 ...) {
    va_list ವಾದಗಳು_2;

    ಆದರೆ(ds->printError!=ಶೂನ್ಯಸೂಚಕ) {
        va_start(ವಾದಗಳು_2, fmt);
        ds->printError(ds->printErrorContext, fmt, ವಾದಗಳು_2);
        va_end(ವಾದಗಳು_2);
    }
}

/* swap a data header ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
udata_ಅದಲುಬದಲು_ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                     ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *p_ಶೀರ್ಷಿಕೆ;
    uint16_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ, info_ಗಾತ್ರ;

    /* argument checking */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ds==ಶೂನ್ಯಸೂಚಕ || ಒಳ_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || length<-1 || (length>0 && ಹೊರಹೋಗು_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* check minimum length and magic bytes */
    p_ಶೀರ್ಷಿಕೆ=(ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *)ಒಳ_ದತ್ತಾಂಶ;
    ಆದರೆ( (length>=0 && length<(int32_t)ಗಾತ್ರ(ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ)) ||
        p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic1!=0xda ||
        p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic2!=0x27 ||
        p_ಶೀರ್ಷಿಕೆ->info.sizeofUChar!=2
    ) {
        udata_printError(ds, "udata_swapDataHeader(): initial bytes do not look like ICU data\n");
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ=ds->readUInt16(p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);
    info_ಗಾತ್ರ=ds->readUInt16(p_ಶೀರ್ಷಿಕೆ->info.size);

    ಆದರೆ( ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ<ಗಾತ್ರ(ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ) ||
        info_ಗಾತ್ರ<ಗಾತ್ರ(UDataInfo) ||
        ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ<(ಗಾತ್ರ(p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2)+info_ಗಾತ್ರ) ||
        (length>=0 && length<ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ)
    ) {
        udata_printError(ds, "udata_swapDataHeader(): header size mismatch - headerSize %d infoSize %d length %d\n",
                         ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ, info_ಗಾತ್ರ, length);
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(length>0) {
        ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
        int32_t max_ಉದ್ದಳತೆ_2;

        /* Most of the fields are just bytes and need no swapping. */
        ಆದರೆ(ಒಳ_ದತ್ತಾಂಶ!=ಹೊರಹೋಗು_ದತ್ತಾಂಶ) {
            uprv_memcpy(ಹೊರಹೋಗು_ದತ್ತಾಂಶ, ಒಳ_ದತ್ತಾಂಶ, ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);
        }
        ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ=(ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ;

        ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ->info.isBigEndian = ds->ಹೊರಹೋಗು_Is_Big_Endian;
        ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ->info.charsetFamily = ds->ಹೊರಹೋಗು_Charset;

        /* swap headerSize */
        ds->ಅದಲುಬದಲು_Array16(ds, &p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ, 2, &ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ, p_Error_ಸಂಕೇತ);

        /* swap UDataInfo size and reservedWord */
        ds->ಅದಲುಬದಲು_Array16(ds, &p_ಶೀರ್ಷಿಕೆ->info.size, 4, &ಹೊರಹೋಗು_ಶೀರ್ಷಿಕೆ->info.size, p_Error_ಸಂಕೇತ);

        /* swap copyright statement after the UDataInfo */
        info_ಗಾತ್ರ+=ಗಾತ್ರ(p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2);
        s=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+info_ಗಾತ್ರ;
        max_ಉದ್ದಳತೆ_2=ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ-info_ಗಾತ್ರ;
        /* get the length of the string */
        ಗಾಗಿ(length=0; length<max_ಉದ್ದಳತೆ_2 && s[length]!=0; ++length) {}
        /* swap the string contents */
        ds->ಅದಲುಬದಲು_Inv_Chars(ds, s, length, (ಅಕ್ಷರ *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ+info_ಗಾತ್ರ, p_Error_ಸಂಕೇತ);
    }

    ಹಿಂತಿರುಗಿ ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
}

/* API functions ------------------------------------------------------------ */

U_CAPI UDataSwapper * U_EXPORT2
udata_openSwapper(UBool ಒಳ_Is_Big_Endian, uint8_t ಒಳ_Charset,
                  UBool ಹೊರಹೋಗು_Is_Big_Endian, uint8_t ಹೊರಹೋಗು_Charset,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UDataSwapper *swapper;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(ಒಳ_Charset>U_EBCDIC_FAMILY || ಹೊರಹೋಗು_Charset>U_EBCDIC_FAMILY) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* allocate the swapper */
    swapper=(UDataSwapper *)uprv_malloc(ಗಾತ್ರ(UDataSwapper));
    ಆದರೆ(swapper==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_memset(swapper, 0, ಗಾತ್ರ(UDataSwapper));

    /* set values and functions pointers according to in/out parameters */
    swapper->ಒಳ_Is_Big_Endian=ಒಳ_Is_Big_Endian;
    swapper->ಒಳ_Charset=ಒಳ_Charset;
    swapper->ಹೊರಹೋಗು_Is_Big_Endian=ಹೊರಹೋಗು_Is_Big_Endian;
    swapper->ಹೊರಹೋಗು_Charset=ಹೊರಹೋಗು_Charset;

    swapper->readUInt16= ಒಳ_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_ನೇರ_UInt16 : uprv_read_ಅದಲುಬದಲು_UInt16;
    swapper->readUInt32= ಒಳ_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_read_ನೇರ_UInt32 : uprv_read_ಅದಲುಬದಲು_UInt32;

    swapper->writeUInt16= ಹೊರಹೋಗು_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_ನೇರ_UInt16 : uprv_write_ಅದಲುಬದಲು_UInt16;
    swapper->writeUInt32= ಹೊರಹೋಗು_Is_Big_Endian==U_IS_BIG_ENDIAN ? uprv_write_ನೇರ_UInt32 : uprv_write_ಅದಲುಬದಲು_UInt32;

    swapper->compareInvChars= ಹೊರಹೋಗು_Charset==U_ASCII_FAMILY ? uprv_compareInvAscii : uprv_compareInvEbcdic;

    ಆದರೆ(ಒಳ_Is_Big_Endian==ಹೊರಹೋಗು_Is_Big_Endian) {
        swapper->ಅದಲುಬದಲು_Array16=uprv_copyArray16;
        swapper->ಅದಲುಬದಲು_Array32=uprv_copyArray32;
        swapper->ಅದಲುಬದಲು_Array64=uprv_copyArray64;
    } ಇಲ್ಲದಿದ್ದರೆ {
        swapper->ಅದಲುಬದಲು_Array16=uprv_ಅದಲುಬದಲು_Array16;
        swapper->ಅದಲುಬದಲು_Array32=uprv_ಅದಲುಬದಲು_Array32;
        swapper->ಅದಲುಬದಲು_Array64=uprv_ಅದಲುಬದಲು_Array64;
    }

    ಆದರೆ(ಒಳ_Charset==U_ASCII_FAMILY) {
        swapper->ಅದಲುಬದಲು_Inv_Chars= ಹೊರಹೋಗು_Charset==U_ASCII_FAMILY ? uprv_copyAscii : uprv_ebcdicFromAscii;
    } ಇಲ್ಲದಿದ್ದರೆ /* U_EBCDIC_FAMILY */ {
        swapper->ಅದಲುಬದಲು_Inv_Chars= ಹೊರಹೋಗು_Charset==U_EBCDIC_FAMILY ? uprv_copyEbcdic : uprv_asciiFromEbcdic;
    }

    ಹಿಂತಿರುಗಿ swapper;
}

U_CAPI UDataSwapper * U_EXPORT2
udata_open_Swapper_For_ಒಳಹರಿವು_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, int32_t length,
                              UBool ಹೊರಹೋಗು_Is_Big_Endian, uint8_t ಹೊರಹೋಗು_Charset,
                              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *p_ಶೀರ್ಷಿಕೆ;
    uint16_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ, info_ಗಾತ್ರ;
    UBool ಒಳ_Is_Big_Endian;
    int8_t ಒಳ_Charset;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ( ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ ||
        (length>=0 && length<(int32_t)ಗಾತ್ರ(ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ)) ||
        ಹೊರಹೋಗು_Charset>U_EBCDIC_FAMILY
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    p_ಶೀರ್ಷಿಕೆ=(ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ *)ದತ್ತಾಂಶ;
    ಆದರೆ( (length>=0 && length<(int32_t)ಗಾತ್ರ(ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ)) ||
        p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic1!=0xda ||
        p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.magic2!=0x27 ||
        p_ಶೀರ್ಷಿಕೆ->info.sizeofUChar!=2
    ) {
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಒಳ_Is_Big_Endian=(UBool)p_ಶೀರ್ಷಿಕೆ->info.isBigEndian;
    ಒಳ_Charset=p_ಶೀರ್ಷಿಕೆ->info.charsetFamily;

    ಆದರೆ(ಒಳ_Is_Big_Endian==U_IS_BIG_ENDIAN) {
        ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ=p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
        info_ಗಾತ್ರ=p_ಶೀರ್ಷಿಕೆ->info.size;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ=uprv_read_ಅದಲುಬದಲು_UInt16(p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);
        info_ಗಾತ್ರ=uprv_read_ಅದಲುಬದಲು_UInt16(p_ಶೀರ್ಷಿಕೆ->info.size);
    }

    ಆದರೆ( ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ<ಗಾತ್ರ(ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ) ||
        info_ಗಾತ್ರ<ಗಾತ್ರ(UDataInfo) ||
        ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ<(ಗಾತ್ರ(p_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ_2)+info_ಗಾತ್ರ) ||
        (length>=0 && length<ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ)
    ) {
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಹಿಂತಿರುಗಿ udata_openSwapper(ಒಳ_Is_Big_Endian, ಒಳ_Charset, ಹೊರಹೋಗು_Is_Big_Endian, ಹೊರಹೋಗು_Charset, p_Error_ಸಂಕೇತ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
udata_closeSwapper(UDataSwapper *ds) {
    uprv_free(ds);
}
