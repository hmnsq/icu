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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/uversion.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

ಎಣಿಕೆ {
    UCNV_NEED_ಗಮ್ಯ_WRITE_BOM=1
};

U_CDECL_BEGIN
/*
 * The UTF-16 toUnicode implementation is also used for the Java-specific
 * "with BOM" variants of UTF-16BE and UTF-16LE.
 */
ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
UTF16_ಗಮ್ಯ_Unicode_With_Offsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/* UTF-16BE ----------------------------------------------------------------- */

#ಆದರೆ U_IS_BIG_ENDIAN
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16BEFromUnicodeWithOffsets
#ಇಲ್ಲದಿದ್ದರೆ
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16LEFromUnicodeWithOffsets
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16BEFromUnicodeWithOffsets(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ;
    ಅಕ್ಷರ *ಗುರಿ;
    int32_t *offsets;

    uint32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ, length, ಮೂಲ_ಸೂಚ್ಯಂಕ;
    char16_t c, trail;
    ಅಕ್ಷರ ಅತಿಪ್ರವಾಹ[4];

    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    length=(int32_t)(p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ);
    ಆದರೆ(length<=0) {
        /* no input, nothing to do */
        ಹಿಂತಿರುಗಿ;
    }

    cnv=p_ವಾದಗಳು->converter;

    /* write the BOM if necessary */
    ಆದರೆ(cnv->from_Unicode_ಸ್ಥಿತಿ==UCNV_NEED_ಗಮ್ಯ_WRITE_BOM) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ bom[]={ (ಅಕ್ಷರ)0xfeu, (ಅಕ್ಷರ)0xffu };
        ucnv_from_UWrite_ಬೈಟುಗಳು(cnv,
                             bom, 2,
                             &p_ವಾದಗಳು->ಗುರಿ, p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ,
                             &p_ವಾದಗಳು->offsets, -1,
                             p_Error_ಸಂಕೇತ);
        cnv->from_Unicode_ಸ್ಥಿತಿ=0;
    }

    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಆದರೆ(ಗುರಿ >= p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ) {
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(uint32_t)(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ-ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;
    ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    ಆದರೆ((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*ಮೂಲ) && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++ಮೂಲ;
        --length;
        ಗುರಿ[0]=(uint8_t)(c>>8);
        ಗುರಿ[1]=(uint8_t)c;
        ಗುರಿ[2]=(uint8_t)(trail>>8);
        ಗುರಿ[3]=(uint8_t)trail;
        ಗುರಿ+=4;
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=4;
        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        ಮೂಲ_ಸೂಚ್ಯಂಕ=1;
        cnv->fromUChar32=c=0;
    }

    ಆದರೆ(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        ಆದರೆ(count>ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
            count=ಗುರಿ_ಸಾಮರ್ಥ್ಯ&~1;
        }
        /* count is even */
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=count;
        count>>=1;
        length-=count;

        ಆದರೆ(offsets==ಶೂನ್ಯಸೂಚಕ) {
            ಆಗ(count>0) {
                c=*ಮೂಲ++;
                ಆದರೆ(U16_IS_SINGLE(c)) {
                    ಗುರಿ[0]=(uint8_t)(c>>8);
                    ಗುರಿ[1]=(uint8_t)c;
                    ಗುರಿ+=2;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ಮೂಲ)) {
                    ++ಮೂಲ;
                    --count;
                    ಗುರಿ[0]=(uint8_t)(c>>8);
                    ಗುರಿ[1]=(uint8_t)c;
                    ಗುರಿ[2]=(uint8_t)(trail>>8);
                    ಗುರಿ[3]=(uint8_t)trail;
                    ಗುರಿ+=4;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ನಿಲ್ಲಿಸು;
                }
                --count;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆಗ(count>0) {
                c=*ಮೂಲ++;
                ಆದರೆ(U16_IS_SINGLE(c)) {
                    ಗುರಿ[0]=(uint8_t)(c>>8);
                    ಗುರಿ[1]=(uint8_t)c;
                    ಗುರಿ+=2;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ಮೂಲ)) {
                    ++ಮೂಲ;
                    --count;
                    ಗುರಿ[0]=(uint8_t)(c>>8);
                    ಗುರಿ[1]=(uint8_t)c;
                    ಗುರಿ[2]=(uint8_t)(trail>>8);
                    ಗುರಿ[3]=(uint8_t)trail;
                    ಗುರಿ+=4;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    ಮೂಲ_ಸೂಚ್ಯಂಕ+=2;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ನಿಲ್ಲಿಸು;
                }
                --count;
            }
        }

        ಆದರೆ(count==0) {
            /* done with the loop for complete UChars */
            ಆದರೆ(length>0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                ಆದರೆ(U16_IS_SINGLE(c=*ಮೂಲ++)) {
                    ಅತಿಪ್ರವಾಹ[0]=(ಅಕ್ಷರ)(c>>8);
                    ಅತಿಪ್ರವಾಹ[1]=(ಅಕ್ಷರ)c;
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                length=0;
                c=0;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* keep c for surrogate handling, length will be set there */
            ಗುರಿ_ಸಾಮರ್ಥ್ಯ+=2*count;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    ಆದರೆ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
            ಆದರೆ(ಮೂಲ<p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ) {
                ಆದರೆ(U16_IS_TRAIL(trail=*ಮೂಲ)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++ಮೂಲ;
                    ಅತಿಪ್ರವಾಹ[0]=(ಅಕ್ಷರ)(c>>8);
                    ಅತಿಪ್ರವಾಹ[1]=(ಅಕ್ಷರ)c;
                    ಅತಿಪ್ರವಾಹ[2]=(ಅಕ್ಷರ)(trail>>8);
                    ಅತಿಪ್ರವಾಹ[3]=(ಅಕ್ಷರ)trail;
                    length=4; /* 4 bytes to output */
                    c=0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* unmatched lead surrogate */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* unmatched trail surrogate */
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
        }
        cnv->fromUChar32=c;
    }

    ಆದರೆ(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_ಬೈಟುಗಳು(cnv,
                             ಅತಿಪ್ರವಾಹ, length,
                             &ಗುರಿ, p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ,
                             &offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
                             p_Error_ಸಂಕೇತ);
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<uint32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - ಗುರಿ);
    }

    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ) && ಮೂಲ<p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ_ಸಾಮರ್ಥ್ಯ==0) {
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ = ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16BEToUnicodeWithOffsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ;
    char16_t *ಗುರಿ;
    int32_t *offsets;

    uint32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ, length, count, ಮೂಲ_ಸೂಚ್ಯಂಕ;
    char16_t c, trail;

    ಆದರೆ(p_ವಾದಗಳು->converter->ಬಹುಲಕ<8) {
        UTF16_ಗಮ್ಯ_Unicode_With_Offsets(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ;
    }

    cnv=p_ವಾದಗಳು->converter;
    ಮೂಲ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
    length=(int32_t)((ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ);
    ಆದರೆ(length<=0 && cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ==0) {
        /* no input, nothing to do */
        ಹಿಂತಿರುಗಿ;
    }

    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಆದರೆ(ಗುರಿ >= p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ) {
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(uint32_t)(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ-ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;
    ಮೂಲ_ಸೂಚ್ಯಂಕ=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    ಆದರೆ(cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->ಗಮ್ಯ_UBytes[0]=(uint8_t)cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;
        cnv->ಗಮ್ಯ_ULength=1;
        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=0;
    }
    ಆದರೆ((count=cnv->ಗಮ್ಯ_ULength)!=0) {
        uint8_t *p=cnv->ಗಮ್ಯ_UBytes;
        ಮಾಡು {
            p[count++]=*ಮೂಲ++;
            ++ಮೂಲ_ಸೂಚ್ಯಂಕ;
            --length;
            ಆದರೆ(count==2) {
                c=((char16_t)p[0]<<8)|p[1];
                ಆದರೆ(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *ಗುರಿ++=c;
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=-1;
                    }
                    --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                    count=0;
                    c=0;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* fall through to error handling for an unmatched trail surrogate */
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(count==4) {
                c=((char16_t)p[0]<<8)|p[1];
                trail=((char16_t)p[2]<<8)|p[3];
                ಆದರೆ(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *ಗುರಿ++=c;
                    ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>=2) {
                        *ಗುರಿ++=trail;
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=2;
                    } ಇಲ್ಲದಿದ್ದರೆ /* targetCapacity==1 */ {
                        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=0;
                        cnv->UChar_Error_ಬಫರ್[0]=trail;
                        cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    }
                    count=0;
                    c=0;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;

                    /* back out reading the code unit after it */
                    ಆದರೆ(((ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ-ಮೂಲ)>=2) {
                        ಮೂಲ-=2;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=0x100|p[2];
                        --ಮೂಲ;
                    }
                    cnv->ಗಮ್ಯ_ULength=2;

                    /* write back the updated pointers */
                    p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
                    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
                    p_ವಾದಗಳು->offsets=offsets;
                    ಹಿಂತಿರುಗಿ;
                }
            }
        } ಆಗ(length>0);
        cnv->ಗಮ್ಯ_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    ಆದರೆ(count>length) {
        count=length&~1;
    }
    ಆದರೆ(c==0 && count>0) {
        length-=count;
        count>>=1;
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=count;
        ಆದರೆ(offsets==ಶೂನ್ಯಸೂಚಕ) {
            ಮಾಡು {
                c=((char16_t)ಮೂಲ[0]<<8)|ಮೂಲ[1];
                ಮೂಲ+=2;
                ಆದರೆ(U16_IS_SINGLE(c)) {
                    *ಗುರಿ++=c;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ಮೂಲ[0]<<8)|ಮೂಲ[1])
                ) {
                    ಮೂಲ+=2;
                    --count;
                    *ಗುರಿ++=c;
                    *ಗುರಿ++=trail;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ನಿಲ್ಲಿಸು;
                }
            } ಆಗ(--count>0);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಮಾಡು {
                c=((char16_t)ಮೂಲ[0]<<8)|ಮೂಲ[1];
                ಮೂಲ+=2;
                ಆದರೆ(U16_IS_SINGLE(c)) {
                    *ಗುರಿ++=c;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    ಮೂಲ_ಸೂಚ್ಯಂಕ+=2;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ಮೂಲ[0]<<8)|ಮೂಲ[1])
                ) {
                    ಮೂಲ+=2;
                    --count;
                    *ಗುರಿ++=c;
                    *ಗುರಿ++=trail;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    ಮೂಲ_ಸೂಚ್ಯಂಕ+=4;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ನಿಲ್ಲಿಸು;
                }
            } ಆಗ(--count>0);
        }

        ಆದರೆ(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            ಗುರಿ_ಸಾಮರ್ಥ್ಯ+=count;
        }
    }

    ಆದರೆ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->ಗಮ್ಯ_UBytes[0]=(uint8_t)(c>>8);
        cnv->ಗಮ್ಯ_UBytes[1]=(uint8_t)c;
        cnv->ಗಮ್ಯ_ULength=2;

        ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
            ಆದರೆ(length>=2) {
                ಆದರೆ(U16_IS_TRAIL(trail=((char16_t)ಮೂಲ[0]<<8)|ಮೂಲ[1])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ಮೂಲ+=2;
                    length-=2;
                    *ಗುರಿ++=c;
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                    cnv->UChar_Error_ಬಫರ್[0]=trail;
                    cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                    cnv->ಗಮ್ಯ_ULength=0;
                    *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* unmatched lead surrogate */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* unmatched trail surrogate */
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
        }
    }

    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        /* check for a remaining source byte */
        ಆದರೆ(length>0) {
            ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ==0) {
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->ಗಮ್ಯ_UBytes[cnv->ಗಮ್ಯ_ULength++]=*ಮೂಲ++;
            }
        }
    }

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

ಸ್ಥಿರ UChar32  U_CALLCONV
UTF16_BEGet_ಮುಂದಿನ_UChar(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ *err) {
    ಸ್ಥಿರಾಂಕ uint8_t *s, *ಮೂಲ_ಎಲ್ಲೆ;
    UChar32 c;

    ಆದರೆ(p_ವಾದಗಳು->converter->ಬಹುಲಕ<8) {
        ಹಿಂತಿರುಗಿ UCNV_GET_ಮುಂದಿನ_UCHAR_USE_ಗಮ್ಯ_U;
    }

    s=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;

    ಆದರೆ(s>=ಮೂಲ_ಎಲ್ಲೆ) {
        /* no input */
        *err=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    ಆದರೆ(s+2>ಮೂಲ_ಎಲ್ಲೆ) {
        /* only one byte: truncated char16_t */
        p_ವಾದಗಳು->converter->ಗಮ್ಯ_UBytes[0]=*s++;
        p_ವಾದಗಳು->converter->ಗಮ್ಯ_ULength=1;
        p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)s;
        *err = U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)*s<<8)|s[1];
    s+=2;

    /* check for a surrogate pair */
    ಆದರೆ(U_IS_SURROGATE(c)) {
        ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
            ಆದರೆ(s+2<=ಮೂಲ_ಎಲ್ಲೆ) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)*s<<8)|s[1];
                ಆದರೆ(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *ಬೈಟುಗಳು=p_ವಾದಗಳು->converter->ಗಮ್ಯ_UBytes;
                s-=2;
                p_ವಾದಗಳು->converter->ಗಮ್ಯ_ULength=(int8_t)(ಮೂಲ_ಎಲ್ಲೆ-s);
                ಮಾಡು {
                    *ಬೈಟುಗಳು++=*s++;
                } ಆಗ(s<ಮೂಲ_ಎಲ್ಲೆ);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* unmatched trail surrogate */
            c=-2;
        }

        ಆದರೆ(c<0) {
            /* write the unmatched surrogate */
            uint8_t *ಬೈಟುಗಳು=p_ವಾದಗಳು->converter->ಗಮ್ಯ_UBytes;
            p_ವಾದಗಳು->converter->ಗಮ್ಯ_ULength=2;
            *ಬೈಟುಗಳು=*(s-2);
            ಬೈಟುಗಳು[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
        }
    }

    p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)s;
    ಹಿಂತಿರುಗಿ c;
} 

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16BEReset(UConverter *cnv, UConverterResetChoice choice) {
    ಆದರೆ(choice<=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        /* reset toUnicode state */
        ಆದರೆ(UCNV_GET_ಆವೃತ್ತಿ(cnv)==0) {
            cnv->ಬಹುಲಕ=8; /* no BOM handling */
        } ಇಲ್ಲದಿದ್ದರೆ {
            cnv->ಬಹುಲಕ=0; /* Java-specific "UnicodeBig" requires BE BOM or no BOM */
        }
    }
    ಆದರೆ(choice!=UCNV_RESET_ಗಮ್ಯ_UNICODE && UCNV_GET_ಆವೃತ್ತಿ(cnv)==1) {
        /* reset fromUnicode for "UnicodeBig": prepare to output the UTF-16BE BOM */
        cnv->from_Unicode_ಸ್ಥಿತಿ=UCNV_NEED_ಗಮ್ಯ_WRITE_BOM;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16BEOpen(UConverter *cnv,
             UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    (ಶೂನ್ಯ)p_ವಾದಗಳು;
    ಆದರೆ(UCNV_GET_ಆವೃತ್ತಿ(cnv)<=1) {
        _UTF16BEReset(cnv, UCNV_RESET_BOTH);
    } ಇಲ್ಲದಿದ್ದರೆ {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *  U_CALLCONV
_UTF16BEGetName(ಸ್ಥಿರಾಂಕ UConverter *cnv) {
    ಆದರೆ(UCNV_GET_ಆವೃತ್ತಿ(cnv)==0) {
        ಹಿಂತಿರುಗಿ "UTF-16BE";
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ "UTF-16BE,version=1";
    }
}
U_CDECL_ಕೊನೆ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _UTF16BEImpl={
    UCNV_UTF16_BigEndian,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _UTF16BEOpen,
    ಶೂನ್ಯಸೂಚಕ,
    _UTF16BEReset,

    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_BEGet_ಮುಂದಿನ_UChar,

    ಶೂನ್ಯಸೂಚಕ,
    _UTF16BEGetName,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_get_Non_Surrogate_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ UTF16_BEStatic_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "UTF-16BE",
    1200, UCNV_IBM, UCNV_UTF16_BigEndian, 2, 2,
    { 0xff, 0xfd, 0, 0 },2,ಸುಳ್ಳು,ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _UTF16BEData=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&UTF16_BEStatic_ದತ್ತಾಂಶ, &_UTF16BEImpl);

/* UTF-16LE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16LEFromUnicodeWithOffsets(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ;
    ಅಕ್ಷರ *ಗುರಿ;
    int32_t *offsets;

    uint32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ, length, ಮೂಲ_ಸೂಚ್ಯಂಕ;
    char16_t c, trail;
    ಅಕ್ಷರ ಅತಿಪ್ರವಾಹ[4];

    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    length=(int32_t)(p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ);
    ಆದರೆ(length<=0) {
        /* no input, nothing to do */
        ಹಿಂತಿರುಗಿ;
    }

    cnv=p_ವಾದಗಳು->converter;

    /* write the BOM if necessary */
    ಆದರೆ(cnv->from_Unicode_ಸ್ಥಿತಿ==UCNV_NEED_ಗಮ್ಯ_WRITE_BOM) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ bom[]={ (ಅಕ್ಷರ)0xffu, (ಅಕ್ಷರ)0xfeu };
        ucnv_from_UWrite_ಬೈಟುಗಳು(cnv,
                             bom, 2,
                             &p_ವಾದಗಳು->ಗುರಿ, p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ,
                             &p_ವಾದಗಳು->offsets, -1,
                             p_Error_ಸಂಕೇತ);
        cnv->from_Unicode_ಸ್ಥಿತಿ=0;
    }

    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಆದರೆ(ಗುರಿ >= p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ) {
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(uint32_t)(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ-p_ವಾದಗಳು->ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;
    ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    ಆದರೆ((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*ಮೂಲ) && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++ಮೂಲ;
        --length;
        ಗುರಿ[0]=(uint8_t)c;
        ಗುರಿ[1]=(uint8_t)(c>>8);
        ಗುರಿ[2]=(uint8_t)trail;
        ಗುರಿ[3]=(uint8_t)(trail>>8);
        ಗುರಿ+=4;
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=4;
        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        ಮೂಲ_ಸೂಚ್ಯಂಕ=1;
        cnv->fromUChar32=c=0;
    }

    ಆದರೆ(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        ಆದರೆ(count>ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
            count=ಗುರಿ_ಸಾಮರ್ಥ್ಯ&~1;
        }
        /* count is even */
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=count;
        count>>=1;
        length-=count;

        ಆದರೆ(offsets==ಶೂನ್ಯಸೂಚಕ) {
            ಆಗ(count>0) {
                c=*ಮೂಲ++;
                ಆದರೆ(U16_IS_SINGLE(c)) {
                    ಗುರಿ[0]=(uint8_t)c;
                    ಗುರಿ[1]=(uint8_t)(c>>8);
                    ಗುರಿ+=2;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ಮೂಲ)) {
                    ++ಮೂಲ;
                    --count;
                    ಗುರಿ[0]=(uint8_t)c;
                    ಗುರಿ[1]=(uint8_t)(c>>8);
                    ಗುರಿ[2]=(uint8_t)trail;
                    ಗುರಿ[3]=(uint8_t)(trail>>8);
                    ಗುರಿ+=4;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ನಿಲ್ಲಿಸು;
                }
                --count;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆಗ(count>0) {
                c=*ಮೂಲ++;
                ಆದರೆ(U16_IS_SINGLE(c)) {
                    ಗುರಿ[0]=(uint8_t)c;
                    ಗುರಿ[1]=(uint8_t)(c>>8);
                    ಗುರಿ+=2;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ಮೂಲ)) {
                    ++ಮೂಲ;
                    --count;
                    ಗುರಿ[0]=(uint8_t)c;
                    ಗುರಿ[1]=(uint8_t)(c>>8);
                    ಗುರಿ[2]=(uint8_t)trail;
                    ಗುರಿ[3]=(uint8_t)(trail>>8);
                    ಗುರಿ+=4;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    ಮೂಲ_ಸೂಚ್ಯಂಕ+=2;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ನಿಲ್ಲಿಸು;
                }
                --count;
            }
        }

        ಆದರೆ(count==0) {
            /* done with the loop for complete UChars */
            ಆದರೆ(length>0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                ಆದರೆ(U16_IS_SINGLE(c=*ಮೂಲ++)) {
                    ಅತಿಪ್ರವಾಹ[0]=(ಅಕ್ಷರ)c;
                    ಅತಿಪ್ರವಾಹ[1]=(ಅಕ್ಷರ)(c>>8);
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                length=0;
                c=0;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* keep c for surrogate handling, length will be set there */
            ಗುರಿ_ಸಾಮರ್ಥ್ಯ+=2*count;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    ಆದರೆ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
            ಆದರೆ(ಮೂಲ<p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ) {
                ಆದರೆ(U16_IS_TRAIL(trail=*ಮೂಲ)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++ಮೂಲ;
                    ಅತಿಪ್ರವಾಹ[0]=(ಅಕ್ಷರ)c;
                    ಅತಿಪ್ರವಾಹ[1]=(ಅಕ್ಷರ)(c>>8);
                    ಅತಿಪ್ರವಾಹ[2]=(ಅಕ್ಷರ)trail;
                    ಅತಿಪ್ರವಾಹ[3]=(ಅಕ್ಷರ)(trail>>8);
                    length=4; /* 4 bytes to output */
                    c=0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* unmatched lead surrogate */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* unmatched trail surrogate */
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
        }
        cnv->fromUChar32=c;
    }

    ಆದರೆ(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_ಬೈಟುಗಳು(cnv,
                             ಅತಿಪ್ರವಾಹ, length,
                             &ಗುರಿ, p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ,
                             &offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
                             p_Error_ಸಂಕೇತ);
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<uint32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - ಗುರಿ);
    }

    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ) && ಮೂಲ<p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ_ಸಾಮರ್ಥ್ಯ==0) {
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16LEToUnicodeWithOffsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ;
    char16_t *ಗುರಿ;
    int32_t *offsets;

    uint32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ, length, count, ಮೂಲ_ಸೂಚ್ಯಂಕ;
    char16_t c, trail;

    ಆದರೆ(p_ವಾದಗಳು->converter->ಬಹುಲಕ<8) {
        UTF16_ಗಮ್ಯ_Unicode_With_Offsets(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ;
    }

    cnv=p_ವಾದಗಳು->converter;
    ಮೂಲ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
    length=(int32_t)((ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ);
    ಆದರೆ(length<=0 && cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ==0) {
        /* no input, nothing to do */
        ಹಿಂತಿರುಗಿ;
    }

    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಆದರೆ(ಗುರಿ >= p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ) {
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(uint32_t)(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ-p_ವಾದಗಳು->ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;
    ಮೂಲ_ಸೂಚ್ಯಂಕ=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    ಆದರೆ(cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->ಗಮ್ಯ_UBytes[0]=(uint8_t)cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;
        cnv->ಗಮ್ಯ_ULength=1;
        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=0;
    }
    ಆದರೆ((count=cnv->ಗಮ್ಯ_ULength)!=0) {
        uint8_t *p=cnv->ಗಮ್ಯ_UBytes;
        ಮಾಡು {
            p[count++]=*ಮೂಲ++;
            ++ಮೂಲ_ಸೂಚ್ಯಂಕ;
            --length;
            ಆದರೆ(count==2) {
                c=((char16_t)p[1]<<8)|p[0];
                ಆದರೆ(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *ಗುರಿ++=c;
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=-1;
                    }
                    --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                    count=0;
                    c=0;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* fall through to error handling for an unmatched trail surrogate */
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(count==4) {
                c=((char16_t)p[1]<<8)|p[0];
                trail=((char16_t)p[3]<<8)|p[2];
                ಆದರೆ(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *ಗುರಿ++=c;
                    ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>=2) {
                        *ಗುರಿ++=trail;
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=2;
                    } ಇಲ್ಲದಿದ್ದರೆ /* targetCapacity==1 */ {
                        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=0;
                        cnv->UChar_Error_ಬಫರ್[0]=trail;
                        cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    }
                    count=0;
                    c=0;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;

                    /* back out reading the code unit after it */
                    ಆದರೆ(((ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ-ಮೂಲ)>=2) {
                        ಮೂಲ-=2;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=0x100|p[2];
                        --ಮೂಲ;
                    }
                    cnv->ಗಮ್ಯ_ULength=2;

                    /* write back the updated pointers */
                    p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
                    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
                    p_ವಾದಗಳು->offsets=offsets;
                    ಹಿಂತಿರುಗಿ;
                }
            }
        } ಆಗ(length>0);
        cnv->ಗಮ್ಯ_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    ಆದರೆ(count>length) {
        count=length&~1;
    }
    ಆದರೆ(c==0 && count>0) {
        length-=count;
        count>>=1;
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=count;
        ಆದರೆ(offsets==ಶೂನ್ಯಸೂಚಕ) {
            ಮಾಡು {
                c=((char16_t)ಮೂಲ[1]<<8)|ಮೂಲ[0];
                ಮೂಲ+=2;
                ಆದರೆ(U16_IS_SINGLE(c)) {
                    *ಗುರಿ++=c;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ಮೂಲ[1]<<8)|ಮೂಲ[0])
                ) {
                    ಮೂಲ+=2;
                    --count;
                    *ಗುರಿ++=c;
                    *ಗುರಿ++=trail;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ನಿಲ್ಲಿಸು;
                }
            } ಆಗ(--count>0);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಮಾಡು {
                c=((char16_t)ಮೂಲ[1]<<8)|ಮೂಲ[0];
                ಮೂಲ+=2;
                ಆದರೆ(U16_IS_SINGLE(c)) {
                    *ಗುರಿ++=c;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    ಮೂಲ_ಸೂಚ್ಯಂಕ+=2;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ಮೂಲ[1]<<8)|ಮೂಲ[0])
                ) {
                    ಮೂಲ+=2;
                    --count;
                    *ಗುರಿ++=c;
                    *ಗುರಿ++=trail;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    ಮೂಲ_ಸೂಚ್ಯಂಕ+=4;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ನಿಲ್ಲಿಸು;
                }
            } ಆಗ(--count>0);
        }

        ಆದರೆ(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            ಗುರಿ_ಸಾಮರ್ಥ್ಯ+=count;
        }
    }

    ಆದರೆ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->ಗಮ್ಯ_UBytes[0]=(uint8_t)c;
        cnv->ಗಮ್ಯ_UBytes[1]=(uint8_t)(c>>8);
        cnv->ಗಮ್ಯ_ULength=2;

        ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
            ಆದರೆ(length>=2) {
                ಆದರೆ(U16_IS_TRAIL(trail=((char16_t)ಮೂಲ[1]<<8)|ಮೂಲ[0])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ಮೂಲ+=2;
                    length-=2;
                    *ಗುರಿ++=c;
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                    cnv->UChar_Error_ಬಫರ್[0]=trail;
                    cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                    cnv->ಗಮ್ಯ_ULength=0;
                    *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* unmatched lead surrogate */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* unmatched trail surrogate */
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
        }
    }

    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        /* check for a remaining source byte */
        ಆದರೆ(length>0) {
            ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ==0) {
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->ಗಮ್ಯ_UBytes[cnv->ಗಮ್ಯ_ULength++]=*ಮೂಲ++;
            }
        }
    }

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

ಸ್ಥಿರ UChar32  U_CALLCONV
UTF16_LEGet_ಮುಂದಿನ_UChar(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ *err) {
    ಸ್ಥಿರಾಂಕ uint8_t *s, *ಮೂಲ_ಎಲ್ಲೆ;
    UChar32 c;

    ಆದರೆ(p_ವಾದಗಳು->converter->ಬಹುಲಕ<8) {
        ಹಿಂತಿರುಗಿ UCNV_GET_ಮುಂದಿನ_UCHAR_USE_ಗಮ್ಯ_U;
    }

    s=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;

    ಆದರೆ(s>=ಮೂಲ_ಎಲ್ಲೆ) {
        /* no input */
        *err=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    ಆದರೆ(s+2>ಮೂಲ_ಎಲ್ಲೆ) {
        /* only one byte: truncated char16_t */
        p_ವಾದಗಳು->converter->ಗಮ್ಯ_UBytes[0]=*s++;
        p_ವಾದಗಳು->converter->ಗಮ್ಯ_ULength=1;
        p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)s;
        *err = U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)s[1]<<8)|*s;
    s+=2;

    /* check for a surrogate pair */
    ಆದರೆ(U_IS_SURROGATE(c)) {
        ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
            ಆದರೆ(s+2<=ಮೂಲ_ಎಲ್ಲೆ) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)s[1]<<8)|*s;
                ಆದರೆ(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *ಬೈಟುಗಳು=p_ವಾದಗಳು->converter->ಗಮ್ಯ_UBytes;
                s-=2;
                p_ವಾದಗಳು->converter->ಗಮ್ಯ_ULength=(int8_t)(ಮೂಲ_ಎಲ್ಲೆ-s);
                ಮಾಡು {
                    *ಬೈಟುಗಳು++=*s++;
                } ಆಗ(s<ಮೂಲ_ಎಲ್ಲೆ);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* unmatched trail surrogate */
            c=-2;
        }

        ಆದರೆ(c<0) {
            /* write the unmatched surrogate */
            uint8_t *ಬೈಟುಗಳು=p_ವಾದಗಳು->converter->ಗಮ್ಯ_UBytes;
            p_ವಾದಗಳು->converter->ಗಮ್ಯ_ULength=2;
            *ಬೈಟುಗಳು=*(s-2);
            ಬೈಟುಗಳು[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
        }
    }

    p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)s;
    ಹಿಂತಿರುಗಿ c;
} 

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16LEReset(UConverter *cnv, UConverterResetChoice choice) {
    ಆದರೆ(choice<=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        /* reset toUnicode state */
        ಆದರೆ(UCNV_GET_ಆವೃತ್ತಿ(cnv)==0) {
            cnv->ಬಹುಲಕ=8; /* no BOM handling */
        } ಇಲ್ಲದಿದ್ದರೆ {
            cnv->ಬಹುಲಕ=0; /* Java-specific "UnicodeLittle" requires LE BOM or no BOM */
        }
    }
    ಆದರೆ(choice!=UCNV_RESET_ಗಮ್ಯ_UNICODE && UCNV_GET_ಆವೃತ್ತಿ(cnv)==1) {
        /* reset fromUnicode for "UnicodeLittle": prepare to output the UTF-16LE BOM */
        cnv->from_Unicode_ಸ್ಥಿತಿ=UCNV_NEED_ಗಮ್ಯ_WRITE_BOM;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16LEOpen(UConverter *cnv,
             UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    (ಶೂನ್ಯ)p_ವಾದಗಳು;
    ಆದರೆ(UCNV_GET_ಆವೃತ್ತಿ(cnv)<=1) {
        _UTF16LEReset(cnv, UCNV_RESET_BOTH);
    } ಇಲ್ಲದಿದ್ದರೆ {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *  U_CALLCONV
_UTF16LEGetName(ಸ್ಥಿರಾಂಕ UConverter *cnv) {
    ಆದರೆ(UCNV_GET_ಆವೃತ್ತಿ(cnv)==0) {
        ಹಿಂತಿರುಗಿ "UTF-16LE";
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ "UTF-16LE,version=1";
    }
}
U_CDECL_ಕೊನೆ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _UTF16LEImpl={
    UCNV_UTF16_LittleEndian,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _UTF16LEOpen,
    ಶೂನ್ಯಸೂಚಕ,
    _UTF16LEReset,

    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    UTF16_LEGet_ಮುಂದಿನ_UChar,

    ಶೂನ್ಯಸೂಚಕ,
    _UTF16LEGetName,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_get_Non_Surrogate_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ UTF16_LEStatic_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "UTF-16LE",
    1202, UCNV_IBM, UCNV_UTF16_LittleEndian, 2, 2,
    { 0xfd, 0xff, 0, 0 },2,ಸುಳ್ಳು,ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _UTF16LEData=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&UTF16_LEStatic_ದತ್ತಾಂಶ, &_UTF16LEImpl);

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
ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16Reset(UConverter *cnv, UConverterResetChoice choice) {
    ಆದರೆ(choice<=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->ಬಹುಲಕ=0;
    }
    ಆದರೆ(choice!=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-16PE BOM */
        cnv->from_Unicode_ಸ್ಥಿತಿ=UCNV_NEED_ಗಮ್ಯ_WRITE_BOM;
    }
}
U_CDECL_ಕೊನೆ
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ UTF16v2_ದತ್ತಾಂಶ;
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_UTF16Open(UConverter *cnv,
           UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(UCNV_GET_ಆವೃತ್ತಿ(cnv)<=2) {
        ಆದರೆ(UCNV_GET_ಆವೃತ್ತಿ(cnv)==2 && !p_ವಾದಗಳು->onlyTestIsLoadable) {
            /*
             * Switch implementation, and switch the staticData that's different
             * and was copied into the UConverter.
             * (See ucnv_createConverterFromSharedData() in ucnv_bld.c.)
             * UTF-16,version=2 fromUnicode() always writes a big-endian byte stream.
             */
            cnv->shared_ದತ್ತಾಂಶ=(UConverter_Shared_ದತ್ತಾಂಶ*)&UTF16v2_ದತ್ತಾಂಶ;
            uprv_memcpy(cnv->subChars, UTF16v2_ದತ್ತಾಂಶ.static_ದತ್ತಾಂಶ->subChar, UCNV_MAX_SUBCHAR_LEN);
        }
        _UTF16Reset(cnv, UCNV_RESET_BOTH);
    } ಇಲ್ಲದಿದ್ದರೆ {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *  U_CALLCONV
_UTF16GetName(ಸ್ಥಿರಾಂಕ UConverter *cnv) {
    ಆದರೆ(UCNV_GET_ಆವೃತ್ತಿ(cnv)==0) {
        ಹಿಂತಿರುಗಿ "UTF-16";
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UCNV_GET_ಆವೃತ್ತಿ(cnv)==1) {
        ಹಿಂತಿರುಗಿ "UTF-16,version=1";
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ "UTF-16,version=2";
    }
}
U_CDECL_ಕೊನೆ
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ UTF16_ದತ್ತಾಂಶ;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ IS_UTF16BE(ಸ್ಥಿರಾಂಕ UConverter *cnv) {
    ಹಿಂತಿರುಗಿ ((cnv)->shared_ದತ್ತಾಂಶ == &_UTF16BEData);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ IS_UTF16LE(ಸ್ಥಿರಾಂಕ UConverter *cnv) {
    ಹಿಂತಿರುಗಿ ((cnv)->shared_ದತ್ತಾಂಶ == &_UTF16LEData);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ IS_UTF16(ಸ್ಥಿರಾಂಕ UConverter *cnv) {
    ಹಿಂತಿರುಗಿ ((cnv)->shared_ದತ್ತಾಂಶ==&UTF16_ದತ್ತಾಂಶ) || ((cnv)->shared_ದತ್ತಾಂಶ == &UTF16v2_ದತ್ತಾಂಶ);
}

U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UTF16_ಗಮ್ಯ_Unicode_With_Offsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv=p_ವಾದಗಳು->converter;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    int32_t *offsets=p_ವಾದಗಳು->offsets;

    int32_t state, ಸ್ಥಾನಾಂತರ_Delta;
    uint8_t b;

    state=cnv->ಬಹುಲಕ;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    ಸ್ಥಾನಾಂತರ_Delta=0;

    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        ಆಯ್ಕೆ(state) {
        ಸ್ಥಿತಿ 0:
            cnv->ಗಮ್ಯ_UBytes[0]=(uint8_t)*ಮೂಲ++;
            cnv->ಗಮ್ಯ_ULength=1;
            state=1;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 1:
            /*
             * Only inside this switch case can the state variable
             * temporarily take two additional values:
             * 6: BOM error, continue with BE
             * 7: BOM error, continue with LE
             */
            b=*ಮೂಲ;
            ಆದರೆ(cnv->ಗಮ್ಯ_UBytes[0]==0xfe && b==0xff) {
                ಆದರೆ(IS_UTF16LE(cnv)) {
                    state=7; /* illegal reverse BOM for Java "UnicodeLittle" */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    state=8; /* detect UTF-16BE */
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cnv->ಗಮ್ಯ_UBytes[0]==0xff && b==0xfe) {
                ಆದರೆ(IS_UTF16BE(cnv)) {
                    state=6; /* illegal reverse BOM for Java "UnicodeBig" */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    state=9; /* detect UTF-16LE */
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((IS_UTF16(cnv) && UCNV_GET_ಆವೃತ್ತಿ(cnv)==1)) {
                state=6; /* illegal missing BOM for Java "Unicode" */
            }
            ಆದರೆ(state>=8) {
                /* BOM detected, consume it */
                ++ಮೂಲ;
                cnv->ಗಮ್ಯ_ULength=0;
                ಸ್ಥಾನಾಂತರ_Delta=(int32_t)(ಮೂಲ-p_ವಾದಗಳು->ಮೂಲ);
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(state<6) {
                /* ok: no BOM, and not a reverse BOM */
                ಆದರೆ(ಮೂಲ!=p_ವಾದಗಳು->ಮೂಲ) {
                    /* reset the source for a correct first offset */
                    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
                    cnv->ಗಮ್ಯ_ULength=0;
                }
                ಆದರೆ(IS_UTF16LE(cnv)) {
                    /* Make Java "UnicodeLittle" default to LE. */
                    state=9;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* Make standard UTF-16 and Java "UnicodeBig" default to BE. */
                    state=8;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /*
                 * error: missing BOM, or reverse BOM
                 * UTF-16,version=1: Java-specific "Unicode" requires a BOM.
                 * UTF-16BE,version=1: Java-specific "UnicodeBig" requires a BE BOM or no BOM.
                 * UTF-16LE,version=1: Java-specific "UnicodeLittle" requires an LE BOM or no BOM.
                 */
                /* report the non-BOM or reverse BOM as an illegal sequence */
                cnv->ಗಮ್ಯ_UBytes[1]=b;
                cnv->ಗಮ್ಯ_ULength=2;
                p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ+1;
                /* continue with conversion if the callback resets the error */
                /*
                 * Make Java "Unicode" default to BE like standard UTF-16.
                 * Make Java "UnicodeBig" and "UnicodeLittle" default
                 * to their normal endiannesses.
                 */
                cnv->ಬಹುಲಕ=state+2;
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_ESCAPE_SEQUENCE;
                ಹಿಂತಿರುಗಿ;
            }
            /* convert the rest of the stream */
            cnv->ಬಹುಲಕ=state;
            ಮುಂದುವರಿ;
        ಸ್ಥಿತಿ 8:
            /* call UTF-16BE */
            p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
            _UTF16BEToUnicodeWithOffsets(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
            ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 9:
            /* call UTF-16LE */
            p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
            _UTF16LEToUnicodeWithOffsets(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
            ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ನಿಲ್ಲಿಸು; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ && ಸ್ಥಾನಾಂತರ_Delta!=0) {
        int32_t *offsets_ಎಲ್ಲೆ=p_ವಾದಗಳು->offsets;
        ಆಗ(offsets<offsets_ಎಲ್ಲೆ) {
            *offsets++ += ಸ್ಥಾನಾಂತರ_Delta;
        }
    }

    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;

    ಆದರೆ(ಮೂಲ==ಮೂಲ_ಎಲ್ಲೆ && p_ವಾದಗಳು->flush) {
        /* handle truncated input */
        ಆಯ್ಕೆ(state) {
        ಸ್ಥಿತಿ 0:
            ನಿಲ್ಲಿಸು; /* no input at all, nothing to do */
        ಸ್ಥಿತಿ 8:
            _UTF16BEToUnicodeWithOffsets(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 9:
            _UTF16LEToUnicodeWithOffsets(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            /* 0<state<8: framework will report truncation, nothing to do here */
            ನಿಲ್ಲಿಸು;
        }
    }

    cnv->ಬಹುಲಕ=state;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
UTF16_Get_ಮುಂದಿನ_UChar(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆಯ್ಕೆ(p_ವಾದಗಳು->converter->ಬಹುಲಕ) {
    ಸ್ಥಿತಿ 8:
        ಹಿಂತಿರುಗಿ UTF16_BEGet_ಮುಂದಿನ_UChar(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
    ಸ್ಥಿತಿ 9:
        ಹಿಂತಿರುಗಿ UTF16_LEGet_ಮುಂದಿನ_UChar(p_ವಾದಗಳು, p_Error_ಸಂಕೇತ);
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ UCNV_GET_ಮುಂದಿನ_UCHAR_USE_ಗಮ್ಯ_U;
    }
}
U_CDECL_ಕೊನೆ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _UTF16Impl = {
    UCNV_UTF16,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _UTF16Open,
    ಶೂನ್ಯಸೂಚಕ,
    _UTF16Reset,

    UTF16_ಗಮ್ಯ_Unicode_With_Offsets,
    UTF16_ಗಮ್ಯ_Unicode_With_Offsets,
    _UTF16PEFromUnicodeWithOffsets,
    _UTF16PEFromUnicodeWithOffsets,
    UTF16_Get_ಮುಂದಿನ_UChar,

    ಶೂನ್ಯಸೂಚಕ, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_get_Non_Surrogate_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ UTF16_Static_ದತ್ತಾಂಶ = {
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "UTF-16",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
#ಆದರೆ U_IS_BIG_ENDIAN
    { 0xff, 0xfd, 0, 0 }, 2,
#ಇಲ್ಲದಿದ್ದರೆ
    { 0xfd, 0xff, 0, 0 }, 2,
#ಆದರೆ_ಅಂತ್ಯ
    ಸುಳ್ಳು, ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ UTF16_ದತ್ತಾಂಶ =
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&UTF16_Static_ದತ್ತಾಂಶ, &_UTF16Impl);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _UTF16v2Impl = {
    UCNV_UTF16,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _UTF16Open,
    ಶೂನ್ಯಸೂಚಕ,
    _UTF16Reset,

    UTF16_ಗಮ್ಯ_Unicode_With_Offsets,
    UTF16_ಗಮ್ಯ_Unicode_With_Offsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_Get_ಮುಂದಿನ_UChar,

    ಶೂನ್ಯಸೂಚಕ, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_get_Non_Surrogate_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ UTF16v2_Static_ದತ್ತಾಂಶ = {
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "UTF-16,version=2",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
    { 0xff, 0xfd, 0, 0 }, 2,
    ಸುಳ್ಳು, ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ UTF16v2_ದತ್ತಾಂಶ =
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&UTF16v2_Static_ದತ್ತಾಂಶ, &_UTF16v2Impl);

#ಆದರೆ_ಅಂತ್ಯ
