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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

/* control optimizations according to the platform */
#ವ್ಯಾಖ್ಯಾನಿಸು LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
Latin1_ಗಮ್ಯ_Unicode_With_Offsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ;
    char16_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ, length;
    int32_t *offsets;

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ;

    /* set up the local pointers */
    ಮೂಲ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ-p_ವಾದಗಳು->ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;

    ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ);
    ಆದರೆ(length<=ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=length;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* target will be full */
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        length=ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    }

    ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=ಗುರಿ_ಸಾಮರ್ಥ್ಯ>>3;
        length=ಗುರಿ_ಸಾಮರ್ಥ್ಯ&=0x7;
        ಮಾಡು {
            ಗುರಿ[0]=ಮೂಲ[0];
            ಗುರಿ[1]=ಮೂಲ[1];
            ಗುರಿ[2]=ಮೂಲ[2];
            ಗುರಿ[3]=ಮೂಲ[3];
            ಗುರಿ[4]=ಮೂಲ[4];
            ಗುರಿ[5]=ಮೂಲ[5];
            ಗುರಿ[6]=ಮೂಲ[6];
            ಗುರಿ[7]=ಮೂಲ[7];
            ಗುರಿ+=8;
            ಮೂಲ+=8;
        } ಆಗ(--count>0);

        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
            ಮಾಡು {
                offsets[0]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[1]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[2]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[3]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[4]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[5]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[6]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[7]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets+=8;
            } ಆಗ(--loops>0);
        }
    }

    /* conversion loop */
    ಆಗ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
        *ಗುರಿ++=*ಮೂಲ++;
        --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    }

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;

    /* set offsets */
    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
        ಆಗ(length>0) {
            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
            --length;
        }
        p_ವಾದಗಳು->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
ಸ್ಥಿರ UChar32 U_CALLCONV
Latin1_Get_ಮುಂದಿನ_UChar(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
    ಆದರೆ(ಮೂಲ<(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ) {
        p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(ಮೂಲ+1);
        ಹಿಂತಿರುಗಿ *ಮೂಲ;
    }

    /* no output because of empty input */
    *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
    ಹಿಂತಿರುಗಿ 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ, *old_ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ, length;
    int32_t *offsets;

    UChar32 cp;
    char16_t c, max;

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=old_ಗುರಿ=(uint8_t *)p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ-p_ವಾದಗಳು->ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;

    ಆದರೆ(cnv->shared_ದತ್ತಾಂಶ==&Latin1_ದತ್ತಾಂಶ) {
        max=0xff; /* Latin-1 */
    } ಇಲ್ಲದಿದ್ದರೆ {
        max=0x7f; /* US-ASCII */
    }

    /* get the converter state from UConverter */
    cp=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ= cp==0 ? 0 : -1;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ);
    ಆದರೆ(length<ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=length;
    }

    /* conversion loop */
    ಆದರೆ(cp!=0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
        ಹೋಗು getTrail;
    }

#ಆದರೆ LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=ಗುರಿ_ಸಾಮರ್ಥ್ಯ>>4;
        ಮಾಡು {
            oredChars=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;
            oredChars|=u=*ಮೂಲ++;
            *ಗುರಿ++=(uint8_t)u;

            /* were all 16 entries really valid? */
            ಆದರೆ(oredChars>max) {
                /* no, return to the first of these 16 */
                ಮೂಲ-=16;
                ಗುರಿ-=16;
                ನಿಲ್ಲಿಸು;
            }
        } ಆಗ(--count>0);
        count=loops-count;
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=16*count;

        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
            old_ಗುರಿ+=16*count;
            ಆಗ(count>0) {
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                --count;
            }
        }
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* conversion loop */
    c=0;
    ಆಗ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0 && (c=*ಮೂಲ++)<=max) {
        /* convert the Unicode code point */
        *ಗುರಿ++=(uint8_t)c;
        --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    }

    ಆದರೆ(c>max) {
        cp=c;
        ಆದರೆ(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
                /* test the following code unit */
                char16_t trail=*ಮೂಲ;
                ಆದರೆ(U16_IS_TRAIL(trail)) {
                    ++ಮೂಲ;
                    cp=U16_GET_SUPPLEMENTARY(cp, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* no more input */
                cnv->fromUChar32=cp;
                ಹೋಗು no_ಇನ್ನಷ್ಟು_ಒಳಹರಿವು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *p_Error_ಸಂಕೇತ= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು : U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
        cnv->fromUChar32=cp;
    }
no_ಇನ್ನಷ್ಟು_ಒಳಹರಿವು:

    /* set offsets since the start */
    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
        size_t count=ಗುರಿ-old_ಗುರಿ;
        ಆಗ(count>0) {
            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
            --count;
        }
    }

    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ) && ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ>=(uint8_t *)p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ) {
        /* target is full */
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=(ಅಕ್ಷರ *)ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_Latin1FromUTF8(UConverter_From_Unicode_ವಾದಗಳು *pFromUArgs,
                    UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ಗಮ್ಯ_UArgs,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *utf8;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ;

    UChar32 c;
    uint8_t b, t1;

    /* set up the local pointers */
    utf8=p_ಗಮ್ಯ_UArgs->converter;
    ಮೂಲ=(uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=(uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=(uint8_t *)pFromUArgs->ಗುರಿ;
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(pFromUArgs->ಗುರಿ_ಎಲ್ಲೆ-pFromUArgs->ಗುರಿ);

    /* get the converter state from the UTF-8 UConverter */
    ಆದರೆ (utf8->ಗಮ್ಯ_ULength > 0) {
        c=(UChar32)utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        c = 0;
    }
    ಆದರೆ(c!=0 && ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ==0) {
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*ಮೂಲ-0x80)) <= 0x3f) {
            ++ಮೂಲ;
            *ಗುರಿ++=(uint8_t)(((c&3)<<6)|t1);
            --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;

            utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=0;
            utf8->ಗಮ್ಯ_ULength=0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *p_Error_ಸಂಕೇತ=U_USING_DEFAULT_WARNING;
            ಹಿಂತಿರುಗಿ;
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
    ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && U8_IS_LEAD(*(ಮೂಲ_ಎಲ್ಲೆ-1))) {
        --ಮೂಲ_ಎಲ್ಲೆ;
    }

    /* conversion loop */
    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
            b=*ಮೂಲ++;
            ಆದರೆ(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *ಗುರಿ++ = b;
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*ಮೂಲ-0x80)) <= 0x3f
            ) {
                ++ಮೂಲ;
                *ಗುರಿ++=(uint8_t)(((b&3)<<6)|t1);
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                p_ಗಮ್ಯ_UArgs->ಮೂಲ=(ಅಕ್ಷರ *)(ಮೂಲ-1);
                pFromUArgs->ಗುರಿ=(ಅಕ್ಷರ *)ಗುರಿ;
                *p_Error_ಸಂಕೇತ=U_USING_DEFAULT_WARNING;
                ಹಿಂತಿರುಗಿ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     * For Latin-1, there is at most exactly one lead byte because of the
     * smaller sourceLimit adjustment logic.
     */
    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ) && ಮೂಲ<(ಮೂಲ_ಎಲ್ಲೆ=(uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ_ಎಲ್ಲೆ)) {
        utf8->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=utf8->ಗಮ್ಯ_UBytes[0]=b=*ಮೂಲ++;
        utf8->ಗಮ್ಯ_ULength=1;
        utf8->ಬಹುಲಕ=U8_ಎಣಿಕೆ_ಬೈಟುಗಳು(b);
    }

    /* write back the updated pointers */
    p_ಗಮ್ಯ_UArgs->ಮೂಲ=(ಅಕ್ಷರ *)ಮೂಲ;
    pFromUArgs->ಗುರಿ=(ಅಕ್ಷರ *)ಗುರಿ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
Latin1_Get_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                     ಸ್ಥಿರಾಂಕ USetAdder *sa,
                     UConverter_Unicode_ಗಣ which,
                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    (ಶೂನ್ಯ)cnv;
    (ಶೂನ್ಯ)which;
    (ಶೂನ್ಯ)p_Error_ಸಂಕೇತ;
    sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0, 0xff);
}
U_CDECL_ಕೊನೆ


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _Latin1Impl={
    UCNV_LATIN_1,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    Latin1_ಗಮ್ಯ_Unicode_With_Offsets,
    Latin1_ಗಮ್ಯ_Unicode_With_Offsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    Latin1_Get_ಮುಂದಿನ_UChar,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    Latin1_Get_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ucnv_Latin1FromUTF8
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ Latin1_Static_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, ಸುಳ್ಳು, ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ Latin1_ದತ್ತಾಂಶ=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&Latin1_Static_ದತ್ತಾಂಶ, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    char16_t *ಗುರಿ, *old_ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ, length;
    int32_t *offsets;

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ;

    uint8_t c;

    /* set up the local pointers */
    ಮೂಲ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=old_ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ-p_ವಾದಗಳು->ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ);
    ಆದರೆ(length<ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=length;
    }

    ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=ಗುರಿ_ಸಾಮರ್ಥ್ಯ>>3;
        ಮಾಡು {
            oredChars=ಗುರಿ[0]=ಮೂಲ[0];
            oredChars|=ಗುರಿ[1]=ಮೂಲ[1];
            oredChars|=ಗುರಿ[2]=ಮೂಲ[2];
            oredChars|=ಗುರಿ[3]=ಮೂಲ[3];
            oredChars|=ಗುರಿ[4]=ಮೂಲ[4];
            oredChars|=ಗುರಿ[5]=ಮೂಲ[5];
            oredChars|=ಗುರಿ[6]=ಮೂಲ[6];
            oredChars|=ಗುರಿ[7]=ಮೂಲ[7];

            /* were all 16 entries really valid? */
            ಆದರೆ(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                ನಿಲ್ಲಿಸು;
            }
            ಮೂಲ+=8;
            ಗುರಿ+=8;
        } ಆಗ(--count>0);
        count=loops-count;
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=count*8;

        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
            old_ಗುರಿ+=count*8;
            ಆಗ(count>0) {
                offsets[0]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[1]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[2]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[3]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[4]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[5]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[6]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets[7]=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                offsets+=8;
                --count;
            }
        }
    }

    /* conversion loop */
    c=0;
    ಆಗ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0 && (c=*ಮೂಲ++)<=0x7f) {
        *ಗುರಿ++=c;
        --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    }

    ಆದರೆ(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=p_ವಾದಗಳು->converter;
        cnv->ಗಮ್ಯ_UBytes[0]=c;
        cnv->ಗಮ್ಯ_ULength=1;
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ>=p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ) {
        /* target is full */
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    /* set offsets since the start */
    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
        size_t count=ಗುರಿ-old_ಗುರಿ;
        ಆಗ(count>0) {
            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
ಸ್ಥಿರ UChar32 U_CALLCONV
ASCIIGet_ಮುಂದಿನ_UChar(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ;
    uint8_t b;

    ಮೂಲ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
    ಆದರೆ(ಮೂಲ<(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ) {
        b=*ಮೂಲ++;
        p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
        ಆದರೆ(b<=0x7f) {
            ಹಿಂತಿರುಗಿ b;
        } ಇಲ್ಲದಿದ್ದರೆ {
            UConverter *cnv=p_ವಾದಗಳು->converter;
            cnv->ಗಮ್ಯ_UBytes[0]=b;
            cnv->ಗಮ್ಯ_ULength=1;
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
            ಹಿಂತಿರುಗಿ 0xffff;
        }
    }

    /* no output because of empty input */
    *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
    ಹಿಂತಿರುಗಿ 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_ASCIIFromUTF8(UConverter_From_Unicode_ವಾದಗಳು *pFromUArgs,
                   UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ಗಮ್ಯ_UArgs,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ, length;

    uint8_t c;

    ಆದರೆ(p_ಗಮ್ಯ_UArgs->converter->ಗಮ್ಯ_ULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *p_Error_ಸಂಕೇತ=U_USING_DEFAULT_WARNING;
        ಹಿಂತಿರುಗಿ;
    }

    /* set up the local pointers */
    ಮೂಲ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ಗಮ್ಯ_UArgs->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=(uint8_t *)pFromUArgs->ಗುರಿ;
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(pFromUArgs->ಗುರಿ_ಎಲ್ಲೆ-pFromUArgs->ಗುರಿ);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ);
    ಆದರೆ(length<ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=length;
    }

    /* unroll the loop with the most common case */
    ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=ಗುರಿ_ಸಾಮರ್ಥ್ಯ>>4;
        ಮಾಡು {
            oredChars=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;
            oredChars|=*ಗುರಿ++=*ಮೂಲ++;

            /* were all 16 entries really valid? */
            ಆದರೆ(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                ಮೂಲ-=16;
                ಗುರಿ-=16;
                ನಿಲ್ಲಿಸು;
            }
        } ಆಗ(--count>0);
        count=loops-count;
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=16*count;
    }

    /* conversion loop */
    c=0;
    ಆಗ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0 && (c=*ಮೂಲ)<=0x7f) {
        ++ಮೂಲ;
        *ಗುರಿ++=c;
        --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    }

    ಆದರೆ(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *p_Error_ಸಂಕೇತ=U_USING_DEFAULT_WARNING;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ>=(ಸ್ಥಿರಾಂಕ uint8_t *)pFromUArgs->ಗುರಿ_ಎಲ್ಲೆ) {
        /* target is full */
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    /* write back the updated pointers */
    p_ಗಮ್ಯ_UArgs->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
    pFromUArgs->ಗುರಿ=(ಅಕ್ಷರ *)ಗುರಿ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ASCIIGet_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                    ಸ್ಥಿರಾಂಕ USetAdder *sa,
                    UConverter_Unicode_ಗಣ which,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    (ಶೂನ್ಯ)cnv;
    (ಶೂನ್ಯ)which;
    (ಶೂನ್ಯ)p_Error_ಸಂಕೇತ;
    sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0, 0x7f);
}
U_CDECL_ಕೊನೆ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _ASCIIImpl={
    UCNV_US_ASCII,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _ASCIIToUnicodeWithOffsets,
    _ASCIIToUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    ASCIIGet_ಮುಂದಿನ_UChar,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ASCIIGet_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ucnv_ASCIIFromUTF8
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ ASCIIStatic_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, ಸುಳ್ಳು, ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _ASCIIData=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&ASCIIStatic_ದತ್ತಾಂಶ, &_ASCIIImpl);

#ಆದರೆ_ಅಂತ್ಯ
