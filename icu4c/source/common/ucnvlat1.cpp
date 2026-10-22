// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/* 
**********************************************************************
*   Copyright (C) 2000-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ucnvlat1.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2000feb07
*   created by: Markus W. Scherer
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

/* control optimizations according to the platform */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
Latin1_ਮੰਜ਼ਿਲ_Unicode_With_Offsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                            UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ uint8_t *ਸਰੋਤ;
    char16_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, length;
    int32_t *offsets;

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    /* set up the local pointers */
    ਸਰੋਤ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;

    ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ);
    ਜੇ(length<=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=length;
    } ਨਹੀਂਤਾਂ {
        /* target will be full */
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        length=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    }

    ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>>3;
        length=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ&=0x7;
        ਕਰੋ {
            ਨਿਸ਼ਾਨਾ[0]=ਸਰੋਤ[0];
            ਨਿਸ਼ਾਨਾ[1]=ਸਰੋਤ[1];
            ਨਿਸ਼ਾਨਾ[2]=ਸਰੋਤ[2];
            ਨਿਸ਼ਾਨਾ[3]=ਸਰੋਤ[3];
            ਨਿਸ਼ਾਨਾ[4]=ਸਰੋਤ[4];
            ਨਿਸ਼ਾਨਾ[5]=ਸਰੋਤ[5];
            ਨਿਸ਼ਾਨਾ[6]=ਸਰੋਤ[6];
            ਨਿਸ਼ਾਨਾ[7]=ਸਰੋਤ[7];
            ਨਿਸ਼ਾਨਾ+=8;
            ਸਰੋਤ+=8;
        } ਜਦੋਂ(--count>0);

        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
            ਕਰੋ {
                offsets[0]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[1]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[2]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[3]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[4]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[5]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[6]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[7]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets+=8;
            } ਜਦੋਂ(--loops>0);
        }
    }

    /* conversion loop */
    ਜਦੋਂ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
        *ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
        --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    }

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;

    /* set offsets */
    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
        ਜਦੋਂ(length>0) {
            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
            --length;
        }
        p_ਦਲੀਲਾਂ->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
ਸਥਿਰ UChar32 U_CALLCONV
Latin1_Get_ਅਗਲਾ_UChar(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                    UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ uint8_t *ਸਰੋਤ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਜੇ(ਸਰੋਤ<(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)(ਸਰੋਤ+1);
        ਵਾਪਸ *ਸਰੋਤ;
    }

    /* no output because of empty input */
    *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
    ਵਾਪਸ 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                              UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ char16_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ, *old_ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, length;
    int32_t *offsets;

    UChar32 cp;
    char16_t c, max;

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=old_ਨਿਸ਼ਾਨਾ=(uint8_t *)p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;

    ਜੇ(cnv->shared_ਅੰਕੜੇ==&Latin1_ਅੰਕੜੇ) {
        max=0xff; /* Latin-1 */
    } ਨਹੀਂਤਾਂ {
        max=0x7f; /* US-ASCII */
    }

    /* get the converter state from UConverter */
    cp=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ= cp==0 ? 0 : -1;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ);
    ਜੇ(length<ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=length;
    }

    /* conversion loop */
    ਜੇ(cp!=0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
        ਜਾਓ getTrail;
    }

#ਜੇ LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>>4;
        ਕਰੋ {
            oredChars=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;
            oredChars|=u=*ਸਰੋਤ++;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)u;

            /* were all 16 entries really valid? */
            ਜੇ(oredChars>max) {
                /* no, return to the first of these 16 */
                ਸਰੋਤ-=16;
                ਨਿਸ਼ਾਨਾ-=16;
                ਰੋਕੋ;
            }
        } ਜਦੋਂ(--count>0);
        count=loops-count;
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=16*count;

        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
            old_ਨਿਸ਼ਾਨਾ+=16*count;
            ਜਦੋਂ(count>0) {
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                --count;
            }
        }
    }
#ਜੇ_ਅੰਤ

    /* conversion loop */
    c=0;
    ਜਦੋਂ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0 && (c=*ਸਰੋਤ++)<=max) {
        /* convert the Unicode code point */
        *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
        --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    }

    ਜੇ(c>max) {
        cp=c;
        ਜੇ(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } ਨਹੀਂਤਾਂ ਜੇ(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                /* test the following code unit */
                char16_t trail=*ਸਰੋਤ;
                ਜੇ(U16_IS_TRAIL(trail)) {
                    ++ਸਰੋਤ;
                    cp=U16_GET_SUPPLEMENTARY(cp, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } ਨਹੀਂਤਾਂ {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                }
            } ਨਹੀਂਤਾਂ {
                /* no more input */
                cnv->fromUChar32=cp;
                ਜਾਓ no_ਹੋਰ_ਨਿਵੇਸ਼;
            }
        } ਨਹੀਂਤਾਂ {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *p_Error_ਕੋਡ= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_ਮਿਲਿਆ : U_INVALID_CHAR_ਮਿਲਿਆ;
        cnv->fromUChar32=cp;
    }
no_ਹੋਰ_ਨਿਵੇਸ਼:

    /* set offsets since the start */
    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
        size_t count=ਨਿਸ਼ਾਨਾ-old_ਨਿਸ਼ਾਨਾ;
        ਜਦੋਂ(count>0) {
            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
            --count;
        }
    }

    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ) && ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ>=(uint8_t *)p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        /* target is full */
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=(ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_Latin1FromUTF8(UConverter_From_Unicode_ਦਲੀਲਾਂ *pFromUArgs,
                    UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਮੰਜ਼ਿਲ_UArgs,
                    UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *utf8;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;

    UChar32 c;
    uint8_t b, t1;

    /* set up the local pointers */
    utf8=p_ਮੰਜ਼ਿਲ_UArgs->converter;
    ਸਰੋਤ=(uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=(uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=(uint8_t *)pFromUArgs->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(pFromUArgs->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-pFromUArgs->ਨਿਸ਼ਾਨਾ);

    /* get the converter state from the UTF-8 UConverter */
    ਜੇ (utf8->ਮੰਜ਼ਿਲ_ULength > 0) {
        c=(UChar32)utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;
    } ਨਹੀਂਤਾਂ {
        c = 0;
    }
    ਜੇ(c!=0 && ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ==0) {
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ;
        } ਨਹੀਂਤਾਂ ਜੇ(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*ਸਰੋਤ-0x80)) <= 0x3f) {
            ++ਸਰੋਤ;
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)(((c&3)<<6)|t1);
            --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;

            utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=0;
            utf8->ਮੰਜ਼ਿਲ_ULength=0;
        } ਨਹੀਂਤਾਂ {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *p_Error_ਕੋਡ=U_USING_DEFAULT_WARNING;
            ਵਾਪਸ;
        }
    }

    /*
     * Make sure that the last byte sequence before sourceLimit is complete
     * or runs into a lead byte.
     * In the conversion loop compare source with sourceLimit only once
     * per multi-byte character.
     * For Latin-1, adjust sourceLimit only for 1 trail byte because
     * the conversion loop handles at most 2-byte sequences.
     */
    ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && U8_IS_LEAD(*(ਸਰੋਤ_ਹੱਦਬੰਦੀ-1))) {
        --ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    }

    /* conversion loop */
    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
            b=*ਸਰੋਤ++;
            ਜੇ(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *ਨਿਸ਼ਾਨਾ++ = b;
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
            } ਨਹੀਂਤਾਂ ਜੇ( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*ਸਰੋਤ-0x80)) <= 0x3f
            ) {
                ++ਸਰੋਤ;
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)(((b&3)<<6)|t1);
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
            } ਨਹੀਂਤਾਂ {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ=(ਅੱਖਰ *)(ਸਰੋਤ-1);
                pFromUArgs->ਨਿਸ਼ਾਨਾ=(ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ;
                *p_Error_ਕੋਡ=U_USING_DEFAULT_WARNING;
                ਵਾਪਸ;
            }
        } ਨਹੀਂਤਾਂ {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     * For Latin-1, there is at most exactly one lead byte because of the
     * smaller sourceLimit adjustment logic.
     */
    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ) && ਸਰੋਤ<(ਸਰੋਤ_ਹੱਦਬੰਦੀ=(uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ_ਹੱਦਬੰਦੀ)) {
        utf8->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=utf8->ਮੰਜ਼ਿਲ_UBytes[0]=b=*ਸਰੋਤ++;
        utf8->ਮੰਜ਼ਿਲ_ULength=1;
        utf8->ਬਹੁਲਕ=U8_ਗਿਣਤੀ_ਬਾਈਟਾਂ(b);
    }

    /* write back the updated pointers */
    p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ=(ਅੱਖਰ *)ਸਰੋਤ;
    pFromUArgs->ਨਿਸ਼ਾਨਾ=(ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
Latin1_Get_Unicode_ਸੈੱਟ(ਅਚਲ UConverter *cnv,
                     ਅਚਲ USetAdder *sa,
                     UConverter_Unicode_ਸੈੱਟ which,
                     UError_ਕੋਡ *p_Error_ਕੋਡ) {
    (ਖਾਲੀ)cnv;
    (ਖਾਲੀ)which;
    (ਖਾਲੀ)p_Error_ਕੋਡ;
    sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0, 0xff);
}
U_CDECL_ਸਿਰਾ


ਸਥਿਰ ਅਚਲ UConverterImpl _Latin1Impl={
    UCNV_LATIN_1,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    Latin1_ਮੰਜ਼ਿਲ_Unicode_With_Offsets,
    Latin1_ਮੰਜ਼ਿਲ_Unicode_With_Offsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    Latin1_Get_ਅਗਲਾ_UChar,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    Latin1_Get_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_Latin1FromUTF8
};

ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ Latin1_Static_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, ਝੂਠ, ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ Latin1_ਅੰਕੜੇ=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&Latin1_Static_ਅੰਕੜੇ, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                           UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    char16_t *ਨਿਸ਼ਾਨਾ, *old_ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, length;
    int32_t *offsets;

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    uint8_t c;

    /* set up the local pointers */
    ਸਰੋਤ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=old_ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ);
    ਜੇ(length<ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=length;
    }

    ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>>3;
        ਕਰੋ {
            oredChars=ਨਿਸ਼ਾਨਾ[0]=ਸਰੋਤ[0];
            oredChars|=ਨਿਸ਼ਾਨਾ[1]=ਸਰੋਤ[1];
            oredChars|=ਨਿਸ਼ਾਨਾ[2]=ਸਰੋਤ[2];
            oredChars|=ਨਿਸ਼ਾਨਾ[3]=ਸਰੋਤ[3];
            oredChars|=ਨਿਸ਼ਾਨਾ[4]=ਸਰੋਤ[4];
            oredChars|=ਨਿਸ਼ਾਨਾ[5]=ਸਰੋਤ[5];
            oredChars|=ਨਿਸ਼ਾਨਾ[6]=ਸਰੋਤ[6];
            oredChars|=ਨਿਸ਼ਾਨਾ[7]=ਸਰੋਤ[7];

            /* were all 16 entries really valid? */
            ਜੇ(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                ਰੋਕੋ;
            }
            ਸਰੋਤ+=8;
            ਨਿਸ਼ਾਨਾ+=8;
        } ਜਦੋਂ(--count>0);
        count=loops-count;
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=count*8;

        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
            old_ਨਿਸ਼ਾਨਾ+=count*8;
            ਜਦੋਂ(count>0) {
                offsets[0]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[1]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[2]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[3]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[4]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[5]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[6]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets[7]=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                offsets+=8;
                --count;
            }
        }
    }

    /* conversion loop */
    c=0;
    ਜਦੋਂ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0 && (c=*ਸਰੋਤ++)<=0x7f) {
        *ਨਿਸ਼ਾਨਾ++=c;
        --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    }

    ਜੇ(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=p_ਦਲੀਲਾਂ->converter;
        cnv->ਮੰਜ਼ਿਲ_UBytes[0]=c;
        cnv->ਮੰਜ਼ਿਲ_ULength=1;
        *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
    } ਨਹੀਂਤਾਂ ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ>=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        /* target is full */
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    /* set offsets since the start */
    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
        size_t count=ਨਿਸ਼ਾਨਾ-old_ਨਿਸ਼ਾਨਾ;
        ਜਦੋਂ(count>0) {
            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
ਸਥਿਰ UChar32 U_CALLCONV
ASCIIGet_ਅਗਲਾ_UChar(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ uint8_t *ਸਰੋਤ;
    uint8_t b;

    ਸਰੋਤ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਜੇ(ਸਰੋਤ<(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        b=*ਸਰੋਤ++;
        p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
        ਜੇ(b<=0x7f) {
            ਵਾਪਸ b;
        } ਨਹੀਂਤਾਂ {
            UConverter *cnv=p_ਦਲੀਲਾਂ->converter;
            cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
            cnv->ਮੰਜ਼ਿਲ_ULength=1;
            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
            ਵਾਪਸ 0xffff;
        }
    }

    /* no output because of empty input */
    *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
    ਵਾਪਸ 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_ASCIIFromUTF8(UConverter_From_Unicode_ਦਲੀਲਾਂ *pFromUArgs,
                   UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਮੰਜ਼ਿਲ_UArgs,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, length;

    uint8_t c;

    ਜੇ(p_ਮੰਜ਼ਿਲ_UArgs->converter->ਮੰਜ਼ਿਲ_ULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *p_Error_ਕੋਡ=U_USING_DEFAULT_WARNING;
        ਵਾਪਸ;
    }

    /* set up the local pointers */
    ਸਰੋਤ=(ਅਚਲ uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=(ਅਚਲ uint8_t *)p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=(uint8_t *)pFromUArgs->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(pFromUArgs->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-pFromUArgs->ਨਿਸ਼ਾਨਾ);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ);
    ਜੇ(length<ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=length;
    }

    /* unroll the loop with the most common case */
    ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>>4;
        ਕਰੋ {
            oredChars=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;
            oredChars|=*ਨਿਸ਼ਾਨਾ++=*ਸਰੋਤ++;

            /* were all 16 entries really valid? */
            ਜੇ(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                ਸਰੋਤ-=16;
                ਨਿਸ਼ਾਨਾ-=16;
                ਰੋਕੋ;
            }
        } ਜਦੋਂ(--count>0);
        count=loops-count;
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=16*count;
    }

    /* conversion loop */
    c=0;
    ਜਦੋਂ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0 && (c=*ਸਰੋਤ)<=0x7f) {
        ++ਸਰੋਤ;
        *ਨਿਸ਼ਾਨਾ++=c;
        --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    }

    ਜੇ(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *p_Error_ਕੋਡ=U_USING_DEFAULT_WARNING;
    } ਨਹੀਂਤਾਂ ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ>=(ਅਚਲ uint8_t *)pFromUArgs->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        /* target is full */
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    /* write back the updated pointers */
    p_ਮੰਜ਼ਿਲ_UArgs->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
    pFromUArgs->ਨਿਸ਼ਾਨਾ=(ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ASCIIGet_Unicode_ਸੈੱਟ(ਅਚਲ UConverter *cnv,
                    ਅਚਲ USetAdder *sa,
                    UConverter_Unicode_ਸੈੱਟ which,
                    UError_ਕੋਡ *p_Error_ਕੋਡ) {
    (ਖਾਲੀ)cnv;
    (ਖਾਲੀ)which;
    (ਖਾਲੀ)p_Error_ਕੋਡ;
    sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0, 0x7f);
}
U_CDECL_ਸਿਰਾ

ਸਥਿਰ ਅਚਲ UConverterImpl _ASCIIImpl={
    UCNV_US_ASCII,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _ASCIIToUnicodeWithOffsets,
    _ASCIIToUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    ASCIIGet_ਅਗਲਾ_UChar,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ASCIIGet_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_ASCIIFromUTF8
};

ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ ASCIIStatic_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, ਝੂਠ, ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _ASCIIData=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&ASCIIStatic_ਅੰਕੜੇ, &_ASCIIImpl);

#ਜੇ_ਅੰਤ
