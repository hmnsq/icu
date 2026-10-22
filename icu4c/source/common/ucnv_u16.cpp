// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*  
**********************************************************************
*   Copyright (C) 2002-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ucnv_u16.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002jul01
*   created by: Markus W. Scherer
*
*   UTF-16 converter implementation. Used to be in ucnv_utf.c.
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uversion.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

ਗਿਣਤੀ {
    UCNV_NEED_ਮੰਜ਼ਿਲ_WRITE_BOM=1
};

U_CDECL_BEGIN
/*
 * The UTF-16 toUnicode implementation is also used for the Java-specific
 * "with BOM" variants of UTF-16BE and UTF-16LE.
 */
ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
UTF16_ਮੰਜ਼ਿਲ_Unicode_With_Offsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                           UError_ਕੋਡ *p_Error_ਕੋਡ);

/* UTF-16BE ----------------------------------------------------------------- */

#ਜੇ U_IS_BIG_ENDIAN
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16BEFromUnicodeWithOffsets
#ਨਹੀਂਤਾਂ
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16LEFromUnicodeWithOffsets
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_UTF16BEFromUnicodeWithOffsets(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                               UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ char16_t *ਸਰੋਤ;
    ਅੱਖਰ *ਨਿਸ਼ਾਨਾ;
    int32_t *offsets;

    uint32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, length, ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    char16_t c, trail;
    ਅੱਖਰ ਉਛਾਲ[4];

    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    length=(int32_t)(p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ);
    ਜੇ(length<=0) {
        /* no input, nothing to do */
        ਵਾਪਸ;
    }

    cnv=p_ਦਲੀਲਾਂ->converter;

    /* write the BOM if necessary */
    ਜੇ(cnv->from_Unicode_ਸਥਿਤੀ==UCNV_NEED_ਮੰਜ਼ਿਲ_WRITE_BOM) {
        ਸਥਿਰ ਅਚਲ ਅੱਖਰ bom[]={ (ਅੱਖਰ)0xfeu, (ਅੱਖਰ)0xffu };
        ucnv_from_UWrite_ਬਾਈਟਾਂ(cnv,
                             bom, 2,
                             &p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ, p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                             &p_ਦਲੀਲਾਂ->offsets, -1,
                             p_Error_ਕੋਡ);
        cnv->from_Unicode_ਸਥਿਤੀ=0;
    }

    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਜੇ(ਨਿਸ਼ਾਨਾ >= p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ;
    }

    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(uint32_t)(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    ਜੇ((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*ਸਰੋਤ) && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++ਸਰੋਤ;
        --length;
        ਨਿਸ਼ਾਨਾ[0]=(uint8_t)(c>>8);
        ਨਿਸ਼ਾਨਾ[1]=(uint8_t)c;
        ਨਿਸ਼ਾਨਾ[2]=(uint8_t)(trail>>8);
        ਨਿਸ਼ਾਨਾ[3]=(uint8_t)trail;
        ਨਿਸ਼ਾਨਾ+=4;
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=4;
        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        ਸਰੋਤ_ਸੂਚਕਾਂਕ=1;
        cnv->fromUChar32=c=0;
    }

    ਜੇ(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        ਜੇ(count>ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
            count=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ&~1;
        }
        /* count is even */
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=count;
        count>>=1;
        length-=count;

        ਜੇ(offsets==ਖਾਲੀ_ਸੂਚਕ) {
            ਜਦੋਂ(count>0) {
                c=*ਸਰੋਤ++;
                ਜੇ(U16_IS_SINGLE(c)) {
                    ਨਿਸ਼ਾਨਾ[0]=(uint8_t)(c>>8);
                    ਨਿਸ਼ਾਨਾ[1]=(uint8_t)c;
                    ਨਿਸ਼ਾਨਾ+=2;
                } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ਸਰੋਤ)) {
                    ++ਸਰੋਤ;
                    --count;
                    ਨਿਸ਼ਾਨਾ[0]=(uint8_t)(c>>8);
                    ਨਿਸ਼ਾਨਾ[1]=(uint8_t)c;
                    ਨਿਸ਼ਾਨਾ[2]=(uint8_t)(trail>>8);
                    ਨਿਸ਼ਾਨਾ[3]=(uint8_t)trail;
                    ਨਿਸ਼ਾਨਾ+=4;
                } ਨਹੀਂਤਾਂ {
                    ਰੋਕੋ;
                }
                --count;
            }
        } ਨਹੀਂਤਾਂ {
            ਜਦੋਂ(count>0) {
                c=*ਸਰੋਤ++;
                ਜੇ(U16_IS_SINGLE(c)) {
                    ਨਿਸ਼ਾਨਾ[0]=(uint8_t)(c>>8);
                    ਨਿਸ਼ਾਨਾ[1]=(uint8_t)c;
                    ਨਿਸ਼ਾਨਾ+=2;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ਸਰੋਤ)) {
                    ++ਸਰੋਤ;
                    --count;
                    ਨਿਸ਼ਾਨਾ[0]=(uint8_t)(c>>8);
                    ਨਿਸ਼ਾਨਾ[1]=(uint8_t)c;
                    ਨਿਸ਼ਾਨਾ[2]=(uint8_t)(trail>>8);
                    ਨਿਸ਼ਾਨਾ[3]=(uint8_t)trail;
                    ਨਿਸ਼ਾਨਾ+=4;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ+=2;
                } ਨਹੀਂਤਾਂ {
                    ਰੋਕੋ;
                }
                --count;
            }
        }

        ਜੇ(count==0) {
            /* done with the loop for complete UChars */
            ਜੇ(length>0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                ਜੇ(U16_IS_SINGLE(c=*ਸਰੋਤ++)) {
                    ਉਛਾਲ[0]=(ਅੱਖਰ)(c>>8);
                    ਉਛਾਲ[1]=(ਅੱਖਰ)c;
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } ਨਹੀਂਤਾਂ {
                length=0;
                c=0;
            }
        } ਨਹੀਂਤਾਂ {
            /* keep c for surrogate handling, length will be set there */
            ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ+=2*count;
        }
    } ਨਹੀਂਤਾਂ {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    ਜੇ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
            ਜੇ(ਸਰੋਤ<p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                ਜੇ(U16_IS_TRAIL(trail=*ਸਰੋਤ)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++ਸਰੋਤ;
                    ਉਛਾਲ[0]=(ਅੱਖਰ)(c>>8);
                    ਉਛਾਲ[1]=(ਅੱਖਰ)c;
                    ਉਛਾਲ[2]=(ਅੱਖਰ)(trail>>8);
                    ਉਛਾਲ[3]=(ਅੱਖਰ)trail;
                    length=4; /* 4 bytes to output */
                    c=0;
                } ਨਹੀਂਤਾਂ {
                    /* unmatched lead surrogate */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                }
            } ਨਹੀਂਤਾਂ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ਨਹੀਂਤਾਂ {
            /* unmatched trail surrogate */
            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
        }
        cnv->fromUChar32=c;
    }

    ਜੇ(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_ਬਾਈਟਾਂ(cnv,
                             ਉਛਾਲ, length,
                             &ਨਿਸ਼ਾਨਾ, p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                             &offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                             p_Error_ਕੋਡ);
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<uint32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - ਨਿਸ਼ਾਨਾ);
    }

    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ) && ਸਰੋਤ<p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ==0) {
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ = ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_UTF16BEToUnicodeWithOffsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ uint8_t *ਸਰੋਤ;
    char16_t *ਨਿਸ਼ਾਨਾ;
    int32_t *offsets;

    uint32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, length, count, ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    char16_t c, trail;

    ਜੇ(p_ਦਲੀਲਾਂ->converter->ਬਹੁਲਕ<8) {
        UTF16_ਮੰਜ਼ਿਲ_Unicode_With_Offsets(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
        ਵਾਪਸ;
    }

    cnv=p_ਦਲੀਲਾਂ->converter;
    ਸਰੋਤ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    length=(int32_t)((ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ);
    ਜੇ(length<=0 && cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ==0) {
        /* no input, nothing to do */
        ਵਾਪਸ;
    }

    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਜੇ(ਨਿਸ਼ਾਨਾ >= p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ;
    }

    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(uint32_t)(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    ਜੇ(cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->ਮੰਜ਼ਿਲ_UBytes[0]=(uint8_t)cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;
        cnv->ਮੰਜ਼ਿਲ_ULength=1;
        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=0;
    }
    ਜੇ((count=cnv->ਮੰਜ਼ਿਲ_ULength)!=0) {
        uint8_t *p=cnv->ਮੰਜ਼ਿਲ_UBytes;
        ਕਰੋ {
            p[count++]=*ਸਰੋਤ++;
            ++ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            --length;
            ਜੇ(count==2) {
                c=((char16_t)p[0]<<8)|p[1];
                ਜੇ(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *ਨਿਸ਼ਾਨਾ++=c;
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=-1;
                    }
                    --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                    count=0;
                    c=0;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } ਨਹੀਂਤਾਂ {
                    /* fall through to error handling for an unmatched trail surrogate */
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(count==4) {
                c=((char16_t)p[0]<<8)|p[1];
                trail=((char16_t)p[2]<<8)|p[3];
                ਜੇ(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *ਨਿਸ਼ਾਨਾ++=c;
                    ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>=2) {
                        *ਨਿਸ਼ਾਨਾ++=trail;
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=2;
                    } ਨਹੀਂਤਾਂ /* targetCapacity==1 */ {
                        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=0;
                        cnv->UChar_Error_ਬਫਰ[0]=trail;
                        cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    }
                    count=0;
                    c=0;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;

                    /* back out reading the code unit after it */
                    ਜੇ(((ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ-ਸਰੋਤ)>=2) {
                        ਸਰੋਤ-=2;
                    } ਨਹੀਂਤਾਂ {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=0x100|p[2];
                        --ਸਰੋਤ;
                    }
                    cnv->ਮੰਜ਼ਿਲ_ULength=2;

                    /* write back the updated pointers */
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
                    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
                    p_ਦਲੀਲਾਂ->offsets=offsets;
                    ਵਾਪਸ;
                }
            }
        } ਜਦੋਂ(length>0);
        cnv->ਮੰਜ਼ਿਲ_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    ਜੇ(count>length) {
        count=length&~1;
    }
    ਜੇ(c==0 && count>0) {
        length-=count;
        count>>=1;
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=count;
        ਜੇ(offsets==ਖਾਲੀ_ਸੂਚਕ) {
            ਕਰੋ {
                c=((char16_t)ਸਰੋਤ[0]<<8)|ਸਰੋਤ[1];
                ਸਰੋਤ+=2;
                ਜੇ(U16_IS_SINGLE(c)) {
                    *ਨਿਸ਼ਾਨਾ++=c;
                } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ਸਰੋਤ[0]<<8)|ਸਰੋਤ[1])
                ) {
                    ਸਰੋਤ+=2;
                    --count;
                    *ਨਿਸ਼ਾਨਾ++=c;
                    *ਨਿਸ਼ਾਨਾ++=trail;
                } ਨਹੀਂਤਾਂ {
                    ਰੋਕੋ;
                }
            } ਜਦੋਂ(--count>0);
        } ਨਹੀਂਤਾਂ {
            ਕਰੋ {
                c=((char16_t)ਸਰੋਤ[0]<<8)|ਸਰੋਤ[1];
                ਸਰੋਤ+=2;
                ਜੇ(U16_IS_SINGLE(c)) {
                    *ਨਿਸ਼ਾਨਾ++=c;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ+=2;
                } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ਸਰੋਤ[0]<<8)|ਸਰੋਤ[1])
                ) {
                    ਸਰੋਤ+=2;
                    --count;
                    *ਨਿਸ਼ਾਨਾ++=c;
                    *ਨਿਸ਼ਾਨਾ++=trail;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ+=4;
                } ਨਹੀਂਤਾਂ {
                    ਰੋਕੋ;
                }
            } ਜਦੋਂ(--count>0);
        }

        ਜੇ(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } ਨਹੀਂਤਾਂ {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ+=count;
        }
    }

    ਜੇ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->ਮੰਜ਼ਿਲ_UBytes[0]=(uint8_t)(c>>8);
        cnv->ਮੰਜ਼ਿਲ_UBytes[1]=(uint8_t)c;
        cnv->ਮੰਜ਼ਿਲ_ULength=2;

        ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
            ਜੇ(length>=2) {
                ਜੇ(U16_IS_TRAIL(trail=((char16_t)ਸਰੋਤ[0]<<8)|ਸਰੋਤ[1])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ਸਰੋਤ+=2;
                    length-=2;
                    *ਨਿਸ਼ਾਨਾ++=c;
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                    cnv->UChar_Error_ਬਫਰ[0]=trail;
                    cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                    cnv->ਮੰਜ਼ਿਲ_ULength=0;
                    *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                } ਨਹੀਂਤਾਂ {
                    /* unmatched lead surrogate */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                }
            } ਨਹੀਂਤਾਂ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ਨਹੀਂਤਾਂ {
            /* unmatched trail surrogate */
            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
        }
    }

    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ)) {
        /* check for a remaining source byte */
        ਜੇ(length>0) {
            ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ==0) {
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            } ਨਹੀਂਤਾਂ {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->ਮੰਜ਼ਿਲ_UBytes[cnv->ਮੰਜ਼ਿਲ_ULength++]=*ਸਰੋਤ++;
            }
        }
    }

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

ਸਥਿਰ UChar32  U_CALLCONV
UTF16_BEGet_ਅਗਲਾ_UChar(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ *err) {
    ਅਚਲ uint8_t *s, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    UChar32 c;

    ਜੇ(p_ਦਲੀਲਾਂ->converter->ਬਹੁਲਕ<8) {
        ਵਾਪਸ UCNV_GET_ਅਗਲਾ_UCHAR_USE_ਮੰਜ਼ਿਲ_U;
    }

    s=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;

    ਜੇ(s>=ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /* no input */
        *err=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0xffff;
    }

    ਜੇ(s+2>ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /* only one byte: truncated char16_t */
        p_ਦਲੀਲਾਂ->converter->ਮੰਜ਼ਿਲ_UBytes[0]=*s++;
        p_ਦਲੀਲਾਂ->converter->ਮੰਜ਼ਿਲ_ULength=1;
        p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)s;
        *err = U_TRUNCATED_CHAR_ਮਿਲਿਆ;
        ਵਾਪਸ 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)*s<<8)|s[1];
    s+=2;

    /* check for a surrogate pair */
    ਜੇ(U_IS_SURROGATE(c)) {
        ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
            ਜੇ(s+2<=ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)*s<<8)|s[1];
                ਜੇ(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } ਨਹੀਂਤਾਂ {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } ਨਹੀਂਤਾਂ {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *ਬਾਈਟਾਂ=p_ਦਲੀਲਾਂ->converter->ਮੰਜ਼ਿਲ_UBytes;
                s-=2;
                p_ਦਲੀਲਾਂ->converter->ਮੰਜ਼ਿਲ_ULength=(int8_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-s);
                ਕਰੋ {
                    *ਬਾਈਟਾਂ++=*s++;
                } ਜਦੋਂ(s<ਸਰੋਤ_ਹੱਦਬੰਦੀ);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_ਮਿਲਿਆ;
            }
        } ਨਹੀਂਤਾਂ {
            /* unmatched trail surrogate */
            c=-2;
        }

        ਜੇ(c<0) {
            /* write the unmatched surrogate */
            uint8_t *ਬਾਈਟਾਂ=p_ਦਲੀਲਾਂ->converter->ਮੰਜ਼ਿਲ_UBytes;
            p_ਦਲੀਲਾਂ->converter->ਮੰਜ਼ਿਲ_ULength=2;
            *ਬਾਈਟਾਂ=*(s-2);
            ਬਾਈਟਾਂ[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
        }
    }

    p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)s;
    ਵਾਪਸ c;
} 

ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_UTF16BEReset(UConverter *cnv, UConverterResetChoice choice) {
    ਜੇ(choice<=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        /* reset toUnicode state */
        ਜੇ(UCNV_GET_ਸੰਸਕਰਣ(cnv)==0) {
            cnv->ਬਹੁਲਕ=8; /* no BOM handling */
        } ਨਹੀਂਤਾਂ {
            cnv->ਬਹੁਲਕ=0; /* Java-specific "UnicodeBig" requires BE BOM or no BOM */
        }
    }
    ਜੇ(choice!=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE && UCNV_GET_ਸੰਸਕਰਣ(cnv)==1) {
        /* reset fromUnicode for "UnicodeBig": prepare to output the UTF-16BE BOM */
        cnv->from_Unicode_ਸਥਿਤੀ=UCNV_NEED_ਮੰਜ਼ਿਲ_WRITE_BOM;
    }
}

ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_UTF16BEOpen(UConverter *cnv,
             UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    (ਖਾਲੀ)p_ਦਲੀਲਾਂ;
    ਜੇ(UCNV_GET_ਸੰਸਕਰਣ(cnv)<=1) {
        _UTF16BEReset(cnv, UCNV_RESET_BOTH);
    } ਨਹੀਂਤਾਂ {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *  U_CALLCONV
_UTF16BEGetName(ਅਚਲ UConverter *cnv) {
    ਜੇ(UCNV_GET_ਸੰਸਕਰਣ(cnv)==0) {
        ਵਾਪਸ "UTF-16BE";
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ "UTF-16BE,version=1";
    }
}
U_CDECL_ਸਿਰਾ

ਸਥਿਰ ਅਚਲ UConverterImpl _UTF16BEImpl={
    UCNV_UTF16_BigEndian,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _UTF16BEOpen,
    ਖਾਲੀ_ਸੂਚਕ,
    _UTF16BEReset,

    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_BEGet_ਅਗਲਾ_UChar,

    ਖਾਲੀ_ਸੂਚਕ,
    _UTF16BEGetName,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_get_Non_Surrogate_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ UTF16_BEStatic_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "UTF-16BE",
    1200, UCNV_IBM, UCNV_UTF16_BigEndian, 2, 2,
    { 0xff, 0xfd, 0, 0 },2,ਝੂਠ,ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _UTF16BEData=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&UTF16_BEStatic_ਅੰਕੜੇ, &_UTF16BEImpl);

/* UTF-16LE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_UTF16LEFromUnicodeWithOffsets(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                               UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ char16_t *ਸਰੋਤ;
    ਅੱਖਰ *ਨਿਸ਼ਾਨਾ;
    int32_t *offsets;

    uint32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, length, ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    char16_t c, trail;
    ਅੱਖਰ ਉਛਾਲ[4];

    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    length=(int32_t)(p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ);
    ਜੇ(length<=0) {
        /* no input, nothing to do */
        ਵਾਪਸ;
    }

    cnv=p_ਦਲੀਲਾਂ->converter;

    /* write the BOM if necessary */
    ਜੇ(cnv->from_Unicode_ਸਥਿਤੀ==UCNV_NEED_ਮੰਜ਼ਿਲ_WRITE_BOM) {
        ਸਥਿਰ ਅਚਲ ਅੱਖਰ bom[]={ (ਅੱਖਰ)0xffu, (ਅੱਖਰ)0xfeu };
        ucnv_from_UWrite_ਬਾਈਟਾਂ(cnv,
                             bom, 2,
                             &p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ, p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                             &p_ਦਲੀਲਾਂ->offsets, -1,
                             p_Error_ਕੋਡ);
        cnv->from_Unicode_ਸਥਿਤੀ=0;
    }

    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਜੇ(ਨਿਸ਼ਾਨਾ >= p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ;
    }

    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(uint32_t)(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    ਜੇ((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*ਸਰੋਤ) && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++ਸਰੋਤ;
        --length;
        ਨਿਸ਼ਾਨਾ[0]=(uint8_t)c;
        ਨਿਸ਼ਾਨਾ[1]=(uint8_t)(c>>8);
        ਨਿਸ਼ਾਨਾ[2]=(uint8_t)trail;
        ਨਿਸ਼ਾਨਾ[3]=(uint8_t)(trail>>8);
        ਨਿਸ਼ਾਨਾ+=4;
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=4;
        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        ਸਰੋਤ_ਸੂਚਕਾਂਕ=1;
        cnv->fromUChar32=c=0;
    }

    ਜੇ(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        ਜੇ(count>ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
            count=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ&~1;
        }
        /* count is even */
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=count;
        count>>=1;
        length-=count;

        ਜੇ(offsets==ਖਾਲੀ_ਸੂਚਕ) {
            ਜਦੋਂ(count>0) {
                c=*ਸਰੋਤ++;
                ਜੇ(U16_IS_SINGLE(c)) {
                    ਨਿਸ਼ਾਨਾ[0]=(uint8_t)c;
                    ਨਿਸ਼ਾਨਾ[1]=(uint8_t)(c>>8);
                    ਨਿਸ਼ਾਨਾ+=2;
                } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ਸਰੋਤ)) {
                    ++ਸਰੋਤ;
                    --count;
                    ਨਿਸ਼ਾਨਾ[0]=(uint8_t)c;
                    ਨਿਸ਼ਾਨਾ[1]=(uint8_t)(c>>8);
                    ਨਿਸ਼ਾਨਾ[2]=(uint8_t)trail;
                    ਨਿਸ਼ਾਨਾ[3]=(uint8_t)(trail>>8);
                    ਨਿਸ਼ਾਨਾ+=4;
                } ਨਹੀਂਤਾਂ {
                    ਰੋਕੋ;
                }
                --count;
            }
        } ਨਹੀਂਤਾਂ {
            ਜਦੋਂ(count>0) {
                c=*ਸਰੋਤ++;
                ਜੇ(U16_IS_SINGLE(c)) {
                    ਨਿਸ਼ਾਨਾ[0]=(uint8_t)c;
                    ਨਿਸ਼ਾਨਾ[1]=(uint8_t)(c>>8);
                    ਨਿਸ਼ਾਨਾ+=2;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ਸਰੋਤ)) {
                    ++ਸਰੋਤ;
                    --count;
                    ਨਿਸ਼ਾਨਾ[0]=(uint8_t)c;
                    ਨਿਸ਼ਾਨਾ[1]=(uint8_t)(c>>8);
                    ਨਿਸ਼ਾਨਾ[2]=(uint8_t)trail;
                    ਨਿਸ਼ਾਨਾ[3]=(uint8_t)(trail>>8);
                    ਨਿਸ਼ਾਨਾ+=4;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ+=2;
                } ਨਹੀਂਤਾਂ {
                    ਰੋਕੋ;
                }
                --count;
            }
        }

        ਜੇ(count==0) {
            /* done with the loop for complete UChars */
            ਜੇ(length>0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                ਜੇ(U16_IS_SINGLE(c=*ਸਰੋਤ++)) {
                    ਉਛਾਲ[0]=(ਅੱਖਰ)c;
                    ਉਛਾਲ[1]=(ਅੱਖਰ)(c>>8);
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } ਨਹੀਂਤਾਂ {
                length=0;
                c=0;
            }
        } ਨਹੀਂਤਾਂ {
            /* keep c for surrogate handling, length will be set there */
            ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ+=2*count;
        }
    } ਨਹੀਂਤਾਂ {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    ਜੇ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
            ਜੇ(ਸਰੋਤ<p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                ਜੇ(U16_IS_TRAIL(trail=*ਸਰੋਤ)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++ਸਰੋਤ;
                    ਉਛਾਲ[0]=(ਅੱਖਰ)c;
                    ਉਛਾਲ[1]=(ਅੱਖਰ)(c>>8);
                    ਉਛਾਲ[2]=(ਅੱਖਰ)trail;
                    ਉਛਾਲ[3]=(ਅੱਖਰ)(trail>>8);
                    length=4; /* 4 bytes to output */
                    c=0;
                } ਨਹੀਂਤਾਂ {
                    /* unmatched lead surrogate */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                }
            } ਨਹੀਂਤਾਂ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ਨਹੀਂਤਾਂ {
            /* unmatched trail surrogate */
            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
        }
        cnv->fromUChar32=c;
    }

    ਜੇ(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_ਬਾਈਟਾਂ(cnv,
                             ਉਛਾਲ, length,
                             &ਨਿਸ਼ਾਨਾ, p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                             &offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                             p_Error_ਕੋਡ);
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<uint32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - ਨਿਸ਼ਾਨਾ);
    }

    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ) && ਸਰੋਤ<p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ==0) {
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_UTF16LEToUnicodeWithOffsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ uint8_t *ਸਰੋਤ;
    char16_t *ਨਿਸ਼ਾਨਾ;
    int32_t *offsets;

    uint32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, length, count, ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    char16_t c, trail;

    ਜੇ(p_ਦਲੀਲਾਂ->converter->ਬਹੁਲਕ<8) {
        UTF16_ਮੰਜ਼ਿਲ_Unicode_With_Offsets(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
        ਵਾਪਸ;
    }

    cnv=p_ਦਲੀਲਾਂ->converter;
    ਸਰੋਤ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    length=(int32_t)((ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ);
    ਜੇ(length<=0 && cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ==0) {
        /* no input, nothing to do */
        ਵਾਪਸ;
    }

    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਜੇ(ਨਿਸ਼ਾਨਾ >= p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ;
    }

    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(uint32_t)(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    ਜੇ(cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->ਮੰਜ਼ਿਲ_UBytes[0]=(uint8_t)cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;
        cnv->ਮੰਜ਼ਿਲ_ULength=1;
        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=0;
    }
    ਜੇ((count=cnv->ਮੰਜ਼ਿਲ_ULength)!=0) {
        uint8_t *p=cnv->ਮੰਜ਼ਿਲ_UBytes;
        ਕਰੋ {
            p[count++]=*ਸਰੋਤ++;
            ++ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            --length;
            ਜੇ(count==2) {
                c=((char16_t)p[1]<<8)|p[0];
                ਜੇ(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *ਨਿਸ਼ਾਨਾ++=c;
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=-1;
                    }
                    --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                    count=0;
                    c=0;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } ਨਹੀਂਤਾਂ {
                    /* fall through to error handling for an unmatched trail surrogate */
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(count==4) {
                c=((char16_t)p[1]<<8)|p[0];
                trail=((char16_t)p[3]<<8)|p[2];
                ਜੇ(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *ਨਿਸ਼ਾਨਾ++=c;
                    ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>=2) {
                        *ਨਿਸ਼ਾਨਾ++=trail;
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=2;
                    } ਨਹੀਂਤਾਂ /* targetCapacity==1 */ {
                        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=0;
                        cnv->UChar_Error_ਬਫਰ[0]=trail;
                        cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    }
                    count=0;
                    c=0;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;

                    /* back out reading the code unit after it */
                    ਜੇ(((ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ-ਸਰੋਤ)>=2) {
                        ਸਰੋਤ-=2;
                    } ਨਹੀਂਤਾਂ {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=0x100|p[2];
                        --ਸਰੋਤ;
                    }
                    cnv->ਮੰਜ਼ਿਲ_ULength=2;

                    /* write back the updated pointers */
                    p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
                    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
                    p_ਦਲੀਲਾਂ->offsets=offsets;
                    ਵਾਪਸ;
                }
            }
        } ਜਦੋਂ(length>0);
        cnv->ਮੰਜ਼ਿਲ_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    ਜੇ(count>length) {
        count=length&~1;
    }
    ਜੇ(c==0 && count>0) {
        length-=count;
        count>>=1;
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=count;
        ਜੇ(offsets==ਖਾਲੀ_ਸੂਚਕ) {
            ਕਰੋ {
                c=((char16_t)ਸਰੋਤ[1]<<8)|ਸਰੋਤ[0];
                ਸਰੋਤ+=2;
                ਜੇ(U16_IS_SINGLE(c)) {
                    *ਨਿਸ਼ਾਨਾ++=c;
                } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ਸਰੋਤ[1]<<8)|ਸਰੋਤ[0])
                ) {
                    ਸਰੋਤ+=2;
                    --count;
                    *ਨਿਸ਼ਾਨਾ++=c;
                    *ਨਿਸ਼ਾਨਾ++=trail;
                } ਨਹੀਂਤਾਂ {
                    ਰੋਕੋ;
                }
            } ਜਦੋਂ(--count>0);
        } ਨਹੀਂਤਾਂ {
            ਕਰੋ {
                c=((char16_t)ਸਰੋਤ[1]<<8)|ਸਰੋਤ[0];
                ਸਰੋਤ+=2;
                ਜੇ(U16_IS_SINGLE(c)) {
                    *ਨਿਸ਼ਾਨਾ++=c;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ+=2;
                } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ਸਰੋਤ[1]<<8)|ਸਰੋਤ[0])
                ) {
                    ਸਰੋਤ+=2;
                    --count;
                    *ਨਿਸ਼ਾਨਾ++=c;
                    *ਨਿਸ਼ਾਨਾ++=trail;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ+=4;
                } ਨਹੀਂਤਾਂ {
                    ਰੋਕੋ;
                }
            } ਜਦੋਂ(--count>0);
        }

        ਜੇ(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } ਨਹੀਂਤਾਂ {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ+=count;
        }
    }

    ਜੇ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->ਮੰਜ਼ਿਲ_UBytes[0]=(uint8_t)c;
        cnv->ਮੰਜ਼ਿਲ_UBytes[1]=(uint8_t)(c>>8);
        cnv->ਮੰਜ਼ਿਲ_ULength=2;

        ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
            ਜੇ(length>=2) {
                ਜੇ(U16_IS_TRAIL(trail=((char16_t)ਸਰੋਤ[1]<<8)|ਸਰੋਤ[0])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ਸਰੋਤ+=2;
                    length-=2;
                    *ਨਿਸ਼ਾਨਾ++=c;
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                    cnv->UChar_Error_ਬਫਰ[0]=trail;
                    cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                    cnv->ਮੰਜ਼ਿਲ_ULength=0;
                    *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                } ਨਹੀਂਤਾਂ {
                    /* unmatched lead surrogate */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                }
            } ਨਹੀਂਤਾਂ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ਨਹੀਂਤਾਂ {
            /* unmatched trail surrogate */
            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
        }
    }

    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ)) {
        /* check for a remaining source byte */
        ਜੇ(length>0) {
            ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ==0) {
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            } ਨਹੀਂਤਾਂ {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->ਮੰਜ਼ਿਲ_UBytes[cnv->ਮੰਜ਼ਿਲ_ULength++]=*ਸਰੋਤ++;
            }
        }
    }

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

ਸਥਿਰ UChar32  U_CALLCONV
UTF16_LEGet_ਅਗਲਾ_UChar(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ *err) {
    ਅਚਲ uint8_t *s, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    UChar32 c;

    ਜੇ(p_ਦਲੀਲਾਂ->converter->ਬਹੁਲਕ<8) {
        ਵਾਪਸ UCNV_GET_ਅਗਲਾ_UCHAR_USE_ਮੰਜ਼ਿਲ_U;
    }

    s=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;

    ਜੇ(s>=ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /* no input */
        *err=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0xffff;
    }

    ਜੇ(s+2>ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /* only one byte: truncated char16_t */
        p_ਦਲੀਲਾਂ->converter->ਮੰਜ਼ਿਲ_UBytes[0]=*s++;
        p_ਦਲੀਲਾਂ->converter->ਮੰਜ਼ਿਲ_ULength=1;
        p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)s;
        *err = U_TRUNCATED_CHAR_ਮਿਲਿਆ;
        ਵਾਪਸ 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)s[1]<<8)|*s;
    s+=2;

    /* check for a surrogate pair */
    ਜੇ(U_IS_SURROGATE(c)) {
        ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
            ਜੇ(s+2<=ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)s[1]<<8)|*s;
                ਜੇ(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } ਨਹੀਂਤਾਂ {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } ਨਹੀਂਤਾਂ {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *ਬਾਈਟਾਂ=p_ਦਲੀਲਾਂ->converter->ਮੰਜ਼ਿਲ_UBytes;
                s-=2;
                p_ਦਲੀਲਾਂ->converter->ਮੰਜ਼ਿਲ_ULength=(int8_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-s);
                ਕਰੋ {
                    *ਬਾਈਟਾਂ++=*s++;
                } ਜਦੋਂ(s<ਸਰੋਤ_ਹੱਦਬੰਦੀ);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_ਮਿਲਿਆ;
            }
        } ਨਹੀਂਤਾਂ {
            /* unmatched trail surrogate */
            c=-2;
        }

        ਜੇ(c<0) {
            /* write the unmatched surrogate */
            uint8_t *ਬਾਈਟਾਂ=p_ਦਲੀਲਾਂ->converter->ਮੰਜ਼ਿਲ_UBytes;
            p_ਦਲੀਲਾਂ->converter->ਮੰਜ਼ਿਲ_ULength=2;
            *ਬਾਈਟਾਂ=*(s-2);
            ਬਾਈਟਾਂ[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
        }
    }

    p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)s;
    ਵਾਪਸ c;
} 

ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_UTF16LEReset(UConverter *cnv, UConverterResetChoice choice) {
    ਜੇ(choice<=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        /* reset toUnicode state */
        ਜੇ(UCNV_GET_ਸੰਸਕਰਣ(cnv)==0) {
            cnv->ਬਹੁਲਕ=8; /* no BOM handling */
        } ਨਹੀਂਤਾਂ {
            cnv->ਬਹੁਲਕ=0; /* Java-specific "UnicodeLittle" requires LE BOM or no BOM */
        }
    }
    ਜੇ(choice!=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE && UCNV_GET_ਸੰਸਕਰਣ(cnv)==1) {
        /* reset fromUnicode for "UnicodeLittle": prepare to output the UTF-16LE BOM */
        cnv->from_Unicode_ਸਥਿਤੀ=UCNV_NEED_ਮੰਜ਼ਿਲ_WRITE_BOM;
    }
}

ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_UTF16LEOpen(UConverter *cnv,
             UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    (ਖਾਲੀ)p_ਦਲੀਲਾਂ;
    ਜੇ(UCNV_GET_ਸੰਸਕਰਣ(cnv)<=1) {
        _UTF16LEReset(cnv, UCNV_RESET_BOTH);
    } ਨਹੀਂਤਾਂ {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *  U_CALLCONV
_UTF16LEGetName(ਅਚਲ UConverter *cnv) {
    ਜੇ(UCNV_GET_ਸੰਸਕਰਣ(cnv)==0) {
        ਵਾਪਸ "UTF-16LE";
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ "UTF-16LE,version=1";
    }
}
U_CDECL_ਸਿਰਾ

ਸਥਿਰ ਅਚਲ UConverterImpl _UTF16LEImpl={
    UCNV_UTF16_LittleEndian,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _UTF16LEOpen,
    ਖਾਲੀ_ਸੂਚਕ,
    _UTF16LEReset,

    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    UTF16_LEGet_ਅਗਲਾ_UChar,

    ਖਾਲੀ_ਸੂਚਕ,
    _UTF16LEGetName,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_get_Non_Surrogate_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};


ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ UTF16_LEStatic_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "UTF-16LE",
    1202, UCNV_IBM, UCNV_UTF16_LittleEndian, 2, 2,
    { 0xfd, 0xff, 0, 0 },2,ਝੂਠ,ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _UTF16LEData=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&UTF16_LEStatic_ਅੰਕੜੇ, &_UTF16LEImpl);

/* UTF-16 (Detect BOM) ------------------------------------------------------ */

/*
 * Detect a BOM at the beginning of the stream and select UTF-16BE or UTF-16LE
 * accordingly.
 * This is a simpler version of the UTF-32 converter, with
 * fewer states for shorter BOMs.
 *
 * State values:
 * 0    initial state
 * 1    saw first byte
 * 2..5 -
 * 6..7 see _UTF16ToUnicodeWithOffsets() comments in state 1
 * 8    UTF-16BE mode
 * 9    UTF-16LE mode
 *
 * During detection: state==number of initial bytes seen so far.
 *
 * On output, emit U+FEFF as the first code point.
 *
 * Variants:
 * - UTF-16,version=1 (Java "Unicode" encoding) treats a missing BOM as an error.
 * - UTF-16BE,version=1 (Java "UnicodeBig" encoding) and
 *   UTF-16LE,version=1 (Java "UnicodeLittle" encoding) treat a reverse BOM as an error.
 */
U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_UTF16Reset(UConverter *cnv, UConverterResetChoice choice) {
    ਜੇ(choice<=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->ਬਹੁਲਕ=0;
    }
    ਜੇ(choice!=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-16PE BOM */
        cnv->from_Unicode_ਸਥਿਤੀ=UCNV_NEED_ਮੰਜ਼ਿਲ_WRITE_BOM;
    }
}
U_CDECL_ਸਿਰਾ
ਬਾਹਰੀ ਅਚਲ UConverter_Shared_ਅੰਕੜੇ UTF16v2_ਅੰਕੜੇ;
U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_UTF16Open(UConverter *cnv,
           UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
           UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(UCNV_GET_ਸੰਸਕਰਣ(cnv)<=2) {
        ਜੇ(UCNV_GET_ਸੰਸਕਰਣ(cnv)==2 && !p_ਦਲੀਲਾਂ->onlyTestIsLoadable) {
            /*
             * Switch implementation, and switch the staticData that's different
             * and was copied into the UConverter.
             * (See ucnv_createConverterFromSharedData() in ucnv_bld.c.)
             * UTF-16,version=2 fromUnicode() always writes a big-endian byte stream.
             */
            cnv->shared_ਅੰਕੜੇ=(UConverter_Shared_ਅੰਕੜੇ*)&UTF16v2_ਅੰਕੜੇ;
            uprv_memcpy(cnv->subChars, UTF16v2_ਅੰਕੜੇ.static_ਅੰਕੜੇ->subChar, UCNV_MAX_SUBCHAR_LEN);
        }
        _UTF16Reset(cnv, UCNV_RESET_BOTH);
    } ਨਹੀਂਤਾਂ {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *  U_CALLCONV
_UTF16GetName(ਅਚਲ UConverter *cnv) {
    ਜੇ(UCNV_GET_ਸੰਸਕਰਣ(cnv)==0) {
        ਵਾਪਸ "UTF-16";
    } ਨਹੀਂਤਾਂ ਜੇ(UCNV_GET_ਸੰਸਕਰਣ(cnv)==1) {
        ਵਾਪਸ "UTF-16,version=1";
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ "UTF-16,version=2";
    }
}
U_CDECL_ਸਿਰਾ
ਬਾਹਰੀ ਅਚਲ UConverter_Shared_ਅੰਕੜੇ UTF16_ਅੰਕੜੇ;

ਸਥਿਰ ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ IS_UTF16BE(ਅਚਲ UConverter *cnv) {
    ਵਾਪਸ ((cnv)->shared_ਅੰਕੜੇ == &_UTF16BEData);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ IS_UTF16LE(ਅਚਲ UConverter *cnv) {
    ਵਾਪਸ ((cnv)->shared_ਅੰਕੜੇ == &_UTF16LEData);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ IS_UTF16(ਅਚਲ UConverter *cnv) {
    ਵਾਪਸ ((cnv)->shared_ਅੰਕੜੇ==&UTF16_ਅੰਕੜੇ) || ((cnv)->shared_ਅੰਕੜੇ == &UTF16v2_ਅੰਕੜੇ);
}

U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UTF16_ਮੰਜ਼ਿਲ_Unicode_With_Offsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                           UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv=p_ਦਲੀਲਾਂ->converter;
    ਅਚਲ ਅੱਖਰ *ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਅਚਲ ਅੱਖਰ *ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    int32_t *offsets=p_ਦਲੀਲਾਂ->offsets;

    int32_t state, ਵਿੱਥ_Delta;
    uint8_t b;

    state=cnv->ਬਹੁਲਕ;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    ਵਿੱਥ_Delta=0;

    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && U_SUCCESS(*p_Error_ਕੋਡ)) {
        ਚੋਣ(state) {
        ਹਾਲਤ 0:
            cnv->ਮੰਜ਼ਿਲ_UBytes[0]=(uint8_t)*ਸਰੋਤ++;
            cnv->ਮੰਜ਼ਿਲ_ULength=1;
            state=1;
            ਰੋਕੋ;
        ਹਾਲਤ 1:
            /*
             * Only inside this switch case can the state variable
             * temporarily take two additional values:
             * 6: BOM error, continue with BE
             * 7: BOM error, continue with LE
             */
            b=*ਸਰੋਤ;
            ਜੇ(cnv->ਮੰਜ਼ਿਲ_UBytes[0]==0xfe && b==0xff) {
                ਜੇ(IS_UTF16LE(cnv)) {
                    state=7; /* illegal reverse BOM for Java "UnicodeLittle" */
                } ਨਹੀਂਤਾਂ {
                    state=8; /* detect UTF-16BE */
                }
            } ਨਹੀਂਤਾਂ ਜੇ(cnv->ਮੰਜ਼ਿਲ_UBytes[0]==0xff && b==0xfe) {
                ਜੇ(IS_UTF16BE(cnv)) {
                    state=6; /* illegal reverse BOM for Java "UnicodeBig" */
                } ਨਹੀਂਤਾਂ {
                    state=9; /* detect UTF-16LE */
                }
            } ਨਹੀਂਤਾਂ ਜੇ((IS_UTF16(cnv) && UCNV_GET_ਸੰਸਕਰਣ(cnv)==1)) {
                state=6; /* illegal missing BOM for Java "Unicode" */
            }
            ਜੇ(state>=8) {
                /* BOM detected, consume it */
                ++ਸਰੋਤ;
                cnv->ਮੰਜ਼ਿਲ_ULength=0;
                ਵਿੱਥ_Delta=(int32_t)(ਸਰੋਤ-p_ਦਲੀਲਾਂ->ਸਰੋਤ);
            } ਨਹੀਂਤਾਂ ਜੇ(state<6) {
                /* ok: no BOM, and not a reverse BOM */
                ਜੇ(ਸਰੋਤ!=p_ਦਲੀਲਾਂ->ਸਰੋਤ) {
                    /* reset the source for a correct first offset */
                    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
                    cnv->ਮੰਜ਼ਿਲ_ULength=0;
                }
                ਜੇ(IS_UTF16LE(cnv)) {
                    /* Make Java "UnicodeLittle" default to LE. */
                    state=9;
                } ਨਹੀਂਤਾਂ {
                    /* Make standard UTF-16 and Java "UnicodeBig" default to BE. */
                    state=8;
                }
            } ਨਹੀਂਤਾਂ {
                /*
                 * error: missing BOM, or reverse BOM
                 * UTF-16,version=1: Java-specific "Unicode" requires a BOM.
                 * UTF-16BE,version=1: Java-specific "UnicodeBig" requires a BE BOM or no BOM.
                 * UTF-16LE,version=1: Java-specific "UnicodeLittle" requires an LE BOM or no BOM.
                 */
                /* report the non-BOM or reverse BOM as an illegal sequence */
                cnv->ਮੰਜ਼ਿਲ_UBytes[1]=b;
                cnv->ਮੰਜ਼ਿਲ_ULength=2;
                p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ+1;
                /* continue with conversion if the callback resets the error */
                /*
                 * Make Java "Unicode" default to BE like standard UTF-16.
                 * Make Java "UnicodeBig" and "UnicodeLittle" default
                 * to their normal endiannesses.
                 */
                cnv->ਬਹੁਲਕ=state+2;
                *p_Error_ਕੋਡ=U_ILLEGAL_ESCAPE_SEQUENCE;
                ਵਾਪਸ;
            }
            /* convert the rest of the stream */
            cnv->ਬਹੁਲਕ=state;
            ਜਾਰੀ;
        ਹਾਲਤ 8:
            /* call UTF-16BE */
            p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
            _UTF16BEToUnicodeWithOffsets(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
            ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
            ਰੋਕੋ;
        ਹਾਲਤ 9:
            /* call UTF-16LE */
            p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
            _UTF16LEToUnicodeWithOffsets(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
            ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
            ਰੋਕੋ;
        ਮੂਲ:
            ਰੋਕੋ; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ && ਵਿੱਥ_Delta!=0) {
        int32_t *offsets_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->offsets;
        ਜਦੋਂ(offsets<offsets_ਹੱਦਬੰਦੀ) {
            *offsets++ += ਵਿੱਥ_Delta;
        }
    }

    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;

    ਜੇ(ਸਰੋਤ==ਸਰੋਤ_ਹੱਦਬੰਦੀ && p_ਦਲੀਲਾਂ->flush) {
        /* handle truncated input */
        ਚੋਣ(state) {
        ਹਾਲਤ 0:
            ਰੋਕੋ; /* no input at all, nothing to do */
        ਹਾਲਤ 8:
            _UTF16BEToUnicodeWithOffsets(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
            ਰੋਕੋ;
        ਹਾਲਤ 9:
            _UTF16LEToUnicodeWithOffsets(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
            ਰੋਕੋ;
        ਮੂਲ:
            /* 0<state<8: framework will report truncation, nothing to do here */
            ਰੋਕੋ;
        }
    }

    cnv->ਬਹੁਲਕ=state;
}

ਸਥਿਰ UChar32 U_CALLCONV
UTF16_Get_ਅਗਲਾ_UChar(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਚੋਣ(p_ਦਲੀਲਾਂ->converter->ਬਹੁਲਕ) {
    ਹਾਲਤ 8:
        ਵਾਪਸ UTF16_BEGet_ਅਗਲਾ_UChar(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
    ਹਾਲਤ 9:
        ਵਾਪਸ UTF16_LEGet_ਅਗਲਾ_UChar(p_ਦਲੀਲਾਂ, p_Error_ਕੋਡ);
    ਮੂਲ:
        ਵਾਪਸ UCNV_GET_ਅਗਲਾ_UCHAR_USE_ਮੰਜ਼ਿਲ_U;
    }
}
U_CDECL_ਸਿਰਾ

ਸਥਿਰ ਅਚਲ UConverterImpl _UTF16Impl = {
    UCNV_UTF16,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _UTF16Open,
    ਖਾਲੀ_ਸੂਚਕ,
    _UTF16Reset,

    UTF16_ਮੰਜ਼ਿਲ_Unicode_With_Offsets,
    UTF16_ਮੰਜ਼ਿਲ_Unicode_With_Offsets,
    _UTF16PEFromUnicodeWithOffsets,
    _UTF16PEFromUnicodeWithOffsets,
    UTF16_Get_ਅਗਲਾ_UChar,

    ਖਾਲੀ_ਸੂਚਕ, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_get_Non_Surrogate_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ UTF16_Static_ਅੰਕੜੇ = {
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "UTF-16",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
#ਜੇ U_IS_BIG_ENDIAN
    { 0xff, 0xfd, 0, 0 }, 2,
#ਨਹੀਂਤਾਂ
    { 0xfd, 0xff, 0, 0 }, 2,
#ਜੇ_ਅੰਤ
    ਝੂਠ, ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ UTF16_ਅੰਕੜੇ =
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&UTF16_Static_ਅੰਕੜੇ, &_UTF16Impl);

ਸਥਿਰ ਅਚਲ UConverterImpl _UTF16v2Impl = {
    UCNV_UTF16,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _UTF16Open,
    ਖਾਲੀ_ਸੂਚਕ,
    _UTF16Reset,

    UTF16_ਮੰਜ਼ਿਲ_Unicode_With_Offsets,
    UTF16_ਮੰਜ਼ਿਲ_Unicode_With_Offsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_Get_ਅਗਲਾ_UChar,

    ਖਾਲੀ_ਸੂਚਕ, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_get_Non_Surrogate_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ UTF16v2_Static_ਅੰਕੜੇ = {
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "UTF-16,version=2",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
    { 0xff, 0xfd, 0, 0 }, 2,
    ਝੂਠ, ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ UTF16v2_ਅੰਕੜੇ =
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&UTF16v2_Static_ਅੰਕੜੇ, &_UTF16v2Impl);

#ਜੇ_ਅੰਤ
