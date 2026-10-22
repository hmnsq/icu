// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2004, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
*   uconv_cnv.c:
*   Implements all the low level conversion functions
*   T_UnicodeConverter_{to,from}Unicode_$ConversionType
*
*   Change history:
*
*   06/29/2000  helena      Major rewrite of the callback APIs.
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_err.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

U_CFUNC ਖਾਲੀ
ucnv_get_Complete_Unicode_ਸੈੱਟ(ਅਚਲ UConverter *cnv,
                   ਅਚਲ USetAdder *sa,
                   UConverter_Unicode_ਸੈੱਟ which,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    (ਖਾਲੀ)cnv;
    (ਖਾਲੀ)which;
    (ਖਾਲੀ)p_Error_ਕੋਡ;
    sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0, 0x10ffff);
}

U_CFUNC ਖਾਲੀ
ucnv_get_Non_Surrogate_Unicode_ਸੈੱਟ(ਅਚਲ UConverter *cnv,
                               ਅਚਲ USetAdder *sa,
                               UConverter_Unicode_ਸੈੱਟ which,
                               UError_ਕੋਡ *p_Error_ਕੋਡ) {
    (ਖਾਲੀ)cnv;
    (ਖਾਲੀ)which;
    (ਖਾਲੀ)p_Error_ਕੋਡ;
    sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0, 0xd7ff);
    sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0xe000, 0x10ffff);
}

U_CFUNC ਖਾਲੀ
ucnv_from_UWrite_ਬਾਈਟਾਂ(UConverter *cnv,
                     ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, int32_t length,
                     ਅੱਖਰ **ਨਿਸ਼ਾਨਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                     int32_t **offsets,
                     int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                     UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅੱਖਰ *t=*ਨਿਸ਼ਾਨਾ;
    int32_t *o;

    /* write bytes */
    ਜੇ(offsets==ਖਾਲੀ_ਸੂਚਕ || (o=*offsets)==ਖਾਲੀ_ਸੂਚਕ) {
        ਜਦੋਂ(length>0 && t<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            *t++=*ਬਾਈਟਾਂ++;
            --length;
        }
    } ਨਹੀਂਤਾਂ {
        /* output with offsets */
        ਜਦੋਂ(length>0 && t<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            *t++=*ਬਾਈਟਾਂ++;
            *o++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            --length;
        }
        *offsets=o;
    }
    *ਨਿਸ਼ਾਨਾ=t;

    /* write overflow */
    ਜੇ(length>0) {
        ਜੇ(cnv!=ਖਾਲੀ_ਸੂਚਕ) {
            t=(ਅੱਖਰ *)cnv->char_Error_ਬਫਰ;
            cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=(int8_t)length;
            ਕਰੋ {
                *t++=(uint8_t)*ਬਾਈਟਾਂ++;
            } ਜਦੋਂ(--length>0);
        }
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }
}

U_CFUNC ਖਾਲੀ
ucnv_ਮੰਜ਼ਿਲ_UWrite_UChars(UConverter *cnv,
                    ਅਚਲ char16_t *uchars, int32_t length,
                    char16_t **ਨਿਸ਼ਾਨਾ, ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                    int32_t **offsets,
                    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                    UError_ਕੋਡ *p_Error_ਕੋਡ) {
    char16_t *t=*ਨਿਸ਼ਾਨਾ;
    int32_t *o;

    /* write UChars */
    ਜੇ(offsets==ਖਾਲੀ_ਸੂਚਕ || (o=*offsets)==ਖਾਲੀ_ਸੂਚਕ) {
        ਜਦੋਂ(length>0 && t<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            *t++=*uchars++;
            --length;
        }
    } ਨਹੀਂਤਾਂ {
        /* output with offsets */
        ਜਦੋਂ(length>0 && t<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            *t++=*uchars++;
            *o++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            --length;
        }
        *offsets=o;
    }
    *ਨਿਸ਼ਾਨਾ=t;

    /* write overflow */
    ਜੇ(length>0) {
        ਜੇ(cnv!=ਖਾਲੀ_ਸੂਚਕ) {
            t=cnv->UChar_Error_ਬਫਰ;
            cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=(int8_t)length;
            ਕਰੋ {
                *t++=*uchars++;
            } ਜਦੋਂ(--length>0);
        }
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }
}

U_CFUNC ਖਾਲੀ
ucnv_ਮੰਜ਼ਿਲ_UWrite_ਕੋਡ_ਬਿੰਦੂ(UConverter *cnv,
                       UChar32 c,
                       char16_t **ਨਿਸ਼ਾਨਾ, ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                       int32_t **offsets,
                       int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                       UError_ਕੋਡ *p_Error_ਕੋਡ) {
    char16_t *t;
    int32_t *o;

    t=*ਨਿਸ਼ਾਨਾ;

    ਜੇ(t<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        ਜੇ(c<=0xffff) {
            *t++=(char16_t)c;
            c=U_SENTINEL;
        } ਨਹੀਂਤਾਂ /* c is a supplementary code point */ {
            *t++=U16_LEAD(c);
            c=U16_TRAIL(c);
            ਜੇ(t<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                *t++=(char16_t)c;
                c=U_SENTINEL;
            }
        }

        /* write offsets */
        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ && (o=*offsets)!=ਖਾਲੀ_ਸੂਚਕ) {
            *o++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਜੇ((*ਨਿਸ਼ਾਨਾ+1)<t) {
                *o++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }
            *offsets=o;
        }
    }

    *ਨਿਸ਼ਾਨਾ=t;

    /* write overflow from c */
    ਜੇ(c>=0) {
        ਜੇ(cnv!=ਖਾਲੀ_ਸੂਚਕ) {
            int8_t i=0;
            U16_APPEND_UNSAFE(cnv->UChar_Error_ਬਫਰ, i, c);
            cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=i;
        }
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }
}

#ਜੇ_ਅੰਤ
