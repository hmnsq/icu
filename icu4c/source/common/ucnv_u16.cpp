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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/uversion.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "cmemory.h"

గణన {
    UCNV_NEED_గమ్యం_WRITE_BOM=1
};

U_CDECL_BEGIN
/*
 * The UTF-16 toUnicode implementation is also used for the Java-specific
 * "with BOM" variants of UTF-16BE and UTF-16LE.
 */
స్థిర శూన్య  U_CALLCONV
UTF16_గమ్యం_Unicode_With_Offsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                           UError_సంకేతం *p_Error_సంకేతం);

/* UTF-16BE ----------------------------------------------------------------- */

#ఒకవేళ U_IS_BIG_ENDIAN
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16BEFromUnicodeWithOffsets
#లేకపోతే
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16LEFromUnicodeWithOffsets
#అయితే_ముగింపు


స్థిర శూన్య  U_CALLCONV
_UTF16BEFromUnicodeWithOffsets(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
                               UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి char16_t *ఆధారం;
    అక్షరం *లక్ష్యం;
    int32_t *offsets;

    uint32_t లక్ష్యం_సామర్థ్యం, length, ఆధారం_సూచీ;
    char16_t c, trail;
    అక్షరం అధికప్రవాహం[4];

    ఆధారం=p_వాదనలు->ఆధారం;
    length=(int32_t)(p_వాదనలు->ఆధారం_హద్దు-ఆధారం);
    ఒకవేళ(length<=0) {
        /* no input, nothing to do */
        తిరిగి;
    }

    cnv=p_వాదనలు->converter;

    /* write the BOM if necessary */
    ఒకవేళ(cnv->from_Unicode_స్థితి==UCNV_NEED_గమ్యం_WRITE_BOM) {
        స్థిర స్థిరరాశి అక్షరం bom[]={ (అక్షరం)0xfeu, (అక్షరం)0xffu };
        ucnv_from_UWrite_బైట్లు(cnv,
                             bom, 2,
                             &p_వాదనలు->లక్ష్యం, p_వాదనలు->లక్ష్యం_హద్దు,
                             &p_వాదనలు->offsets, -1,
                             p_Error_సంకేతం);
        cnv->from_Unicode_స్థితి=0;
    }

    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    ఒకవేళ(లక్ష్యం >= p_వాదనలు->లక్ష్యం_హద్దు) {
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి;
    }

    లక్ష్యం_సామర్థ్యం=(uint32_t)(p_వాదనలు->లక్ష్యం_హద్దు-లక్ష్యం);
    offsets=p_వాదనలు->offsets;
    ఆధారం_సూచీ=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    ఒకవేళ((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*ఆధారం) && లక్ష్యం_సామర్థ్యం>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++ఆధారం;
        --length;
        లక్ష్యం[0]=(uint8_t)(c>>8);
        లక్ష్యం[1]=(uint8_t)c;
        లక్ష్యం[2]=(uint8_t)(trail>>8);
        లక్ష్యం[3]=(uint8_t)trail;
        లక్ష్యం+=4;
        లక్ష్యం_సామర్థ్యం-=4;
        ఒకవేళ(offsets!=శూన్యసూచిక) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        ఆధారం_సూచీ=1;
        cnv->fromUChar32=c=0;
    }

    ఒకవేళ(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        ఒకవేళ(count>లక్ష్యం_సామర్థ్యం) {
            count=లక్ష్యం_సామర్థ్యం&~1;
        }
        /* count is even */
        లక్ష్యం_సామర్థ్యం-=count;
        count>>=1;
        length-=count;

        ఒకవేళ(offsets==శూన్యసూచిక) {
            అయితే(count>0) {
                c=*ఆధారం++;
                ఒకవేళ(U16_IS_SINGLE(c)) {
                    లక్ష్యం[0]=(uint8_t)(c>>8);
                    లక్ష్యం[1]=(uint8_t)c;
                    లక్ష్యం+=2;
                } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ఆధారం)) {
                    ++ఆధారం;
                    --count;
                    లక్ష్యం[0]=(uint8_t)(c>>8);
                    లక్ష్యం[1]=(uint8_t)c;
                    లక్ష్యం[2]=(uint8_t)(trail>>8);
                    లక్ష్యం[3]=(uint8_t)trail;
                    లక్ష్యం+=4;
                } లేకపోతే {
                    ఆపు;
                }
                --count;
            }
        } లేకపోతే {
            అయితే(count>0) {
                c=*ఆధారం++;
                ఒకవేళ(U16_IS_SINGLE(c)) {
                    లక్ష్యం[0]=(uint8_t)(c>>8);
                    లక్ష్యం[1]=(uint8_t)c;
                    లక్ష్యం+=2;
                    *offsets++=ఆధారం_సూచీ;
                    *offsets++=ఆధారం_సూచీ++;
                } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ఆధారం)) {
                    ++ఆధారం;
                    --count;
                    లక్ష్యం[0]=(uint8_t)(c>>8);
                    లక్ష్యం[1]=(uint8_t)c;
                    లక్ష్యం[2]=(uint8_t)(trail>>8);
                    లక్ష్యం[3]=(uint8_t)trail;
                    లక్ష్యం+=4;
                    *offsets++=ఆధారం_సూచీ;
                    *offsets++=ఆధారం_సూచీ;
                    *offsets++=ఆధారం_సూచీ;
                    *offsets++=ఆధారం_సూచీ;
                    ఆధారం_సూచీ+=2;
                } లేకపోతే {
                    ఆపు;
                }
                --count;
            }
        }

        ఒకవేళ(count==0) {
            /* done with the loop for complete UChars */
            ఒకవేళ(length>0 && లక్ష్యం_సామర్థ్యం>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                ఒకవేళ(U16_IS_SINGLE(c=*ఆధారం++)) {
                    అధికప్రవాహం[0]=(అక్షరం)(c>>8);
                    అధికప్రవాహం[1]=(అక్షరం)c;
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } లేకపోతే {
                length=0;
                c=0;
            }
        } లేకపోతే {
            /* keep c for surrogate handling, length will be set there */
            లక్ష్యం_సామర్థ్యం+=2*count;
        }
    } లేకపోతే {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    ఒకవేళ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
            ఒకవేళ(ఆధారం<p_వాదనలు->ఆధారం_హద్దు) {
                ఒకవేళ(U16_IS_TRAIL(trail=*ఆధారం)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++ఆధారం;
                    అధికప్రవాహం[0]=(అక్షరం)(c>>8);
                    అధికప్రవాహం[1]=(అక్షరం)c;
                    అధికప్రవాహం[2]=(అక్షరం)(trail>>8);
                    అధికప్రవాహం[3]=(అక్షరం)trail;
                    length=4; /* 4 bytes to output */
                    c=0;
                } లేకపోతే {
                    /* unmatched lead surrogate */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                }
            } లేకపోతే {
                /* see if the trail surrogate is in the next buffer */
            }
        } లేకపోతే {
            /* unmatched trail surrogate */
            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
        }
        cnv->fromUChar32=c;
    }

    ఒకవేళ(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_బైట్లు(cnv,
                             అధికప్రవాహం, length,
                             &లక్ష్యం, p_వాదనలు->లక్ష్యం_హద్దు,
                             &offsets, ఆధారం_సూచీ,
                             p_Error_సంకేతం);
        లక్ష్యం_సామర్థ్యం = static_cast<uint32_t>(p_వాదనలు->లక్ష్యం_హద్దు - లక్ష్యం);
    }

    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం) && ఆధారం<p_వాదనలు->ఆధారం_హద్దు && లక్ష్యం_సామర్థ్యం==0) {
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
    }

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=ఆధారం;
    p_వాదనలు->లక్ష్యం = లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

స్థిర శూన్య  U_CALLCONV
_UTF16BEToUnicodeWithOffsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                             UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి uint8_t *ఆధారం;
    char16_t *లక్ష్యం;
    int32_t *offsets;

    uint32_t లక్ష్యం_సామర్థ్యం, length, count, ఆధారం_సూచీ;
    char16_t c, trail;

    ఒకవేళ(p_వాదనలు->converter->బహుళకం<8) {
        UTF16_గమ్యం_Unicode_With_Offsets(p_వాదనలు, p_Error_సంకేతం);
        తిరిగి;
    }

    cnv=p_వాదనలు->converter;
    ఆధారం=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
    length=(int32_t)((స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం_హద్దు-ఆధారం);
    ఒకవేళ(length<=0 && cnv->గమ్యం_Unicode_స్థితి==0) {
        /* no input, nothing to do */
        తిరిగి;
    }

    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    ఒకవేళ(లక్ష్యం >= p_వాదనలు->లక్ష్యం_హద్దు) {
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి;
    }

    లక్ష్యం_సామర్థ్యం=(uint32_t)(p_వాదనలు->లక్ష్యం_హద్దు-లక్ష్యం);
    offsets=p_వాదనలు->offsets;
    ఆధారం_సూచీ=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    ఒకవేళ(cnv->గమ్యం_Unicode_స్థితి!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->గమ్యం_UBytes[0]=(uint8_t)cnv->గమ్యం_Unicode_స్థితి;
        cnv->గమ్యం_ULength=1;
        cnv->గమ్యం_Unicode_స్థితి=0;
    }
    ఒకవేళ((count=cnv->గమ్యం_ULength)!=0) {
        uint8_t *p=cnv->గమ్యం_UBytes;
        చేయి {
            p[count++]=*ఆధారం++;
            ++ఆధారం_సూచీ;
            --length;
            ఒకవేళ(count==2) {
                c=((char16_t)p[0]<<8)|p[1];
                ఒకవేళ(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *లక్ష్యం++=c;
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=-1;
                    }
                    --లక్ష్యం_సామర్థ్యం;
                    count=0;
                    c=0;
                    ఆపు;
                } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } లేకపోతే {
                    /* fall through to error handling for an unmatched trail surrogate */
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(count==4) {
                c=((char16_t)p[0]<<8)|p[1];
                trail=((char16_t)p[2]<<8)|p[3];
                ఒకవేళ(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *లక్ష్యం++=c;
                    ఒకవేళ(లక్ష్యం_సామర్థ్యం>=2) {
                        *లక్ష్యం++=trail;
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        లక్ష్యం_సామర్థ్యం-=2;
                    } లేకపోతే /* targetCapacity==1 */ {
                        లక్ష్యం_సామర్థ్యం=0;
                        cnv->UChar_Error_బఫరు[0]=trail;
                        cnv->UChar_Error_బఫరు_నిడివి=1;
                        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                    }
                    count=0;
                    c=0;
                    ఆపు;
                } లేకపోతే {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;

                    /* back out reading the code unit after it */
                    ఒకవేళ(((స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం-ఆధారం)>=2) {
                        ఆధారం-=2;
                    } లేకపోతే {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->గమ్యం_Unicode_స్థితి=0x100|p[2];
                        --ఆధారం;
                    }
                    cnv->గమ్యం_ULength=2;

                    /* write back the updated pointers */
                    p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
                    p_వాదనలు->లక్ష్యం=లక్ష్యం;
                    p_వాదనలు->offsets=offsets;
                    తిరిగి;
                }
            }
        } అయితే(length>0);
        cnv->గమ్యం_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*లక్ష్యం_సామర్థ్యం;
    ఒకవేళ(count>length) {
        count=length&~1;
    }
    ఒకవేళ(c==0 && count>0) {
        length-=count;
        count>>=1;
        లక్ష్యం_సామర్థ్యం-=count;
        ఒకవేళ(offsets==శూన్యసూచిక) {
            చేయి {
                c=((char16_t)ఆధారం[0]<<8)|ఆధారం[1];
                ఆధారం+=2;
                ఒకవేళ(U16_IS_SINGLE(c)) {
                    *లక్ష్యం++=c;
                } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ఆధారం[0]<<8)|ఆధారం[1])
                ) {
                    ఆధారం+=2;
                    --count;
                    *లక్ష్యం++=c;
                    *లక్ష్యం++=trail;
                } లేకపోతే {
                    ఆపు;
                }
            } అయితే(--count>0);
        } లేకపోతే {
            చేయి {
                c=((char16_t)ఆధారం[0]<<8)|ఆధారం[1];
                ఆధారం+=2;
                ఒకవేళ(U16_IS_SINGLE(c)) {
                    *లక్ష్యం++=c;
                    *offsets++=ఆధారం_సూచీ;
                    ఆధారం_సూచీ+=2;
                } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ఆధారం[0]<<8)|ఆధారం[1])
                ) {
                    ఆధారం+=2;
                    --count;
                    *లక్ష్యం++=c;
                    *లక్ష్యం++=trail;
                    *offsets++=ఆధారం_సూచీ;
                    *offsets++=ఆధారం_సూచీ;
                    ఆధారం_సూచీ+=4;
                } లేకపోతే {
                    ఆపు;
                }
            } అయితే(--count>0);
        }

        ఒకవేళ(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } లేకపోతే {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            లక్ష్యం_సామర్థ్యం+=count;
        }
    }

    ఒకవేళ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->గమ్యం_UBytes[0]=(uint8_t)(c>>8);
        cnv->గమ్యం_UBytes[1]=(uint8_t)c;
        cnv->గమ్యం_ULength=2;

        ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
            ఒకవేళ(length>=2) {
                ఒకవేళ(U16_IS_TRAIL(trail=((char16_t)ఆధారం[0]<<8)|ఆధారం[1])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ఆధారం+=2;
                    length-=2;
                    *లక్ష్యం++=c;
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                    cnv->UChar_Error_బఫరు[0]=trail;
                    cnv->UChar_Error_బఫరు_నిడివి=1;
                    cnv->గమ్యం_ULength=0;
                    *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                } లేకపోతే {
                    /* unmatched lead surrogate */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                }
            } లేకపోతే {
                /* see if the trail surrogate is in the next buffer */
            }
        } లేకపోతే {
            /* unmatched trail surrogate */
            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
        }
    }

    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం)) {
        /* check for a remaining source byte */
        ఒకవేళ(length>0) {
            ఒకవేళ(లక్ష్యం_సామర్థ్యం==0) {
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            } లేకపోతే {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->గమ్యం_UBytes[cnv->గమ్యం_ULength++]=*ఆధారం++;
            }
        }
    }

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
    p_వాదనలు->లక్ష్యం=లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

స్థిర UChar32  U_CALLCONV
UTF16_BEGet_తదుపరి_UChar(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు, UError_సంకేతం *err) {
    స్థిరరాశి uint8_t *s, *ఆధారం_హద్దు;
    UChar32 c;

    ఒకవేళ(p_వాదనలు->converter->బహుళకం<8) {
        తిరిగి UCNV_GET_తదుపరి_UCHAR_USE_గమ్యం_U;
    }

    s=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం_హద్దు;

    ఒకవేళ(s>=ఆధారం_హద్దు) {
        /* no input */
        *err=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0xffff;
    }

    ఒకవేళ(s+2>ఆధారం_హద్దు) {
        /* only one byte: truncated char16_t */
        p_వాదనలు->converter->గమ్యం_UBytes[0]=*s++;
        p_వాదనలు->converter->గమ్యం_ULength=1;
        p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)s;
        *err = U_TRUNCATED_CHAR_దొరికింది;
        తిరిగి 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)*s<<8)|s[1];
    s+=2;

    /* check for a surrogate pair */
    ఒకవేళ(U_IS_SURROGATE(c)) {
        ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
            ఒకవేళ(s+2<=ఆధారం_హద్దు) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)*s<<8)|s[1];
                ఒకవేళ(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } లేకపోతే {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } లేకపోతే {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *బైట్లు=p_వాదనలు->converter->గమ్యం_UBytes;
                s-=2;
                p_వాదనలు->converter->గమ్యం_ULength=(int8_t)(ఆధారం_హద్దు-s);
                చేయి {
                    *బైట్లు++=*s++;
                } అయితే(s<ఆధారం_హద్దు);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_దొరికింది;
            }
        } లేకపోతే {
            /* unmatched trail surrogate */
            c=-2;
        }

        ఒకవేళ(c<0) {
            /* write the unmatched surrogate */
            uint8_t *బైట్లు=p_వాదనలు->converter->గమ్యం_UBytes;
            p_వాదనలు->converter->గమ్యం_ULength=2;
            *బైట్లు=*(s-2);
            బైట్లు[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_దొరికింది;
        }
    }

    p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)s;
    తిరిగి c;
} 

స్థిర శూన్య  U_CALLCONV
_UTF16BEReset(UConverter *cnv, UConverterResetChoice choice) {
    ఒకవేళ(choice<=UCNV_RESET_గమ్యం_UNICODE) {
        /* reset toUnicode state */
        ఒకవేళ(UCNV_GET_సంస్కరణ(cnv)==0) {
            cnv->బహుళకం=8; /* no BOM handling */
        } లేకపోతే {
            cnv->బహుళకం=0; /* Java-specific "UnicodeBig" requires BE BOM or no BOM */
        }
    }
    ఒకవేళ(choice!=UCNV_RESET_గమ్యం_UNICODE && UCNV_GET_సంస్కరణ(cnv)==1) {
        /* reset fromUnicode for "UnicodeBig": prepare to output the UTF-16BE BOM */
        cnv->from_Unicode_స్థితి=UCNV_NEED_గమ్యం_WRITE_BOM;
    }
}

స్థిర శూన్య  U_CALLCONV
_UTF16BEOpen(UConverter *cnv,
             UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
             UError_సంకేతం *p_Error_సంకేతం) {
    (శూన్య)p_వాదనలు;
    ఒకవేళ(UCNV_GET_సంస్కరణ(cnv)<=1) {
        _UTF16BEReset(cnv, UCNV_RESET_BOTH);
    } లేకపోతే {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
    }
}

స్థిర స్థిరరాశి అక్షరం *  U_CALLCONV
_UTF16BEGetName(స్థిరరాశి UConverter *cnv) {
    ఒకవేళ(UCNV_GET_సంస్కరణ(cnv)==0) {
        తిరిగి "UTF-16BE";
    } లేకపోతే {
        తిరిగి "UTF-16BE,version=1";
    }
}
U_CDECL_చివర

స్థిర స్థిరరాశి UConverterImpl _UTF16BEImpl={
    UCNV_UTF16_BigEndian,

    శూన్యసూచిక,
    శూన్యసూచిక,

    _UTF16BEOpen,
    శూన్యసూచిక,
    _UTF16BEReset,

    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_BEGet_తదుపరి_UChar,

    శూన్యసూచిక,
    _UTF16BEGetName,
    శూన్యసూచిక,
    శూన్యసూచిక,
    ucnv_get_Non_Surrogate_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};

స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం UTF16_BEStatic_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "UTF-16BE",
    1200, UCNV_IBM, UCNV_UTF16_BigEndian, 2, 2,
    { 0xff, 0xfd, 0, 0 },2,అబద్ధం,అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


స్థిరరాశి UConverter_Shared_దత్తాంశం _UTF16BEData=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&UTF16_BEStatic_దత్తాంశం, &_UTF16BEImpl);

/* UTF-16LE ----------------------------------------------------------------- */
U_CDECL_BEGIN
స్థిర శూన్య  U_CALLCONV
_UTF16LEFromUnicodeWithOffsets(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
                               UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి char16_t *ఆధారం;
    అక్షరం *లక్ష్యం;
    int32_t *offsets;

    uint32_t లక్ష్యం_సామర్థ్యం, length, ఆధారం_సూచీ;
    char16_t c, trail;
    అక్షరం అధికప్రవాహం[4];

    ఆధారం=p_వాదనలు->ఆధారం;
    length=(int32_t)(p_వాదనలు->ఆధారం_హద్దు-ఆధారం);
    ఒకవేళ(length<=0) {
        /* no input, nothing to do */
        తిరిగి;
    }

    cnv=p_వాదనలు->converter;

    /* write the BOM if necessary */
    ఒకవేళ(cnv->from_Unicode_స్థితి==UCNV_NEED_గమ్యం_WRITE_BOM) {
        స్థిర స్థిరరాశి అక్షరం bom[]={ (అక్షరం)0xffu, (అక్షరం)0xfeu };
        ucnv_from_UWrite_బైట్లు(cnv,
                             bom, 2,
                             &p_వాదనలు->లక్ష్యం, p_వాదనలు->లక్ష్యం_హద్దు,
                             &p_వాదనలు->offsets, -1,
                             p_Error_సంకేతం);
        cnv->from_Unicode_స్థితి=0;
    }

    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    ఒకవేళ(లక్ష్యం >= p_వాదనలు->లక్ష్యం_హద్దు) {
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి;
    }

    లక్ష్యం_సామర్థ్యం=(uint32_t)(p_వాదనలు->లక్ష్యం_హద్దు-p_వాదనలు->లక్ష్యం);
    offsets=p_వాదనలు->offsets;
    ఆధారం_సూచీ=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    ఒకవేళ((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*ఆధారం) && లక్ష్యం_సామర్థ్యం>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++ఆధారం;
        --length;
        లక్ష్యం[0]=(uint8_t)c;
        లక్ష్యం[1]=(uint8_t)(c>>8);
        లక్ష్యం[2]=(uint8_t)trail;
        లక్ష్యం[3]=(uint8_t)(trail>>8);
        లక్ష్యం+=4;
        లక్ష్యం_సామర్థ్యం-=4;
        ఒకవేళ(offsets!=శూన్యసూచిక) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        ఆధారం_సూచీ=1;
        cnv->fromUChar32=c=0;
    }

    ఒకవేళ(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        ఒకవేళ(count>లక్ష్యం_సామర్థ్యం) {
            count=లక్ష్యం_సామర్థ్యం&~1;
        }
        /* count is even */
        లక్ష్యం_సామర్థ్యం-=count;
        count>>=1;
        length-=count;

        ఒకవేళ(offsets==శూన్యసూచిక) {
            అయితే(count>0) {
                c=*ఆధారం++;
                ఒకవేళ(U16_IS_SINGLE(c)) {
                    లక్ష్యం[0]=(uint8_t)c;
                    లక్ష్యం[1]=(uint8_t)(c>>8);
                    లక్ష్యం+=2;
                } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ఆధారం)) {
                    ++ఆధారం;
                    --count;
                    లక్ష్యం[0]=(uint8_t)c;
                    లక్ష్యం[1]=(uint8_t)(c>>8);
                    లక్ష్యం[2]=(uint8_t)trail;
                    లక్ష్యం[3]=(uint8_t)(trail>>8);
                    లక్ష్యం+=4;
                } లేకపోతే {
                    ఆపు;
                }
                --count;
            }
        } లేకపోతే {
            అయితే(count>0) {
                c=*ఆధారం++;
                ఒకవేళ(U16_IS_SINGLE(c)) {
                    లక్ష్యం[0]=(uint8_t)c;
                    లక్ష్యం[1]=(uint8_t)(c>>8);
                    లక్ష్యం+=2;
                    *offsets++=ఆధారం_సూచీ;
                    *offsets++=ఆధారం_సూచీ++;
                } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ఆధారం)) {
                    ++ఆధారం;
                    --count;
                    లక్ష్యం[0]=(uint8_t)c;
                    లక్ష్యం[1]=(uint8_t)(c>>8);
                    లక్ష్యం[2]=(uint8_t)trail;
                    లక్ష్యం[3]=(uint8_t)(trail>>8);
                    లక్ష్యం+=4;
                    *offsets++=ఆధారం_సూచీ;
                    *offsets++=ఆధారం_సూచీ;
                    *offsets++=ఆధారం_సూచీ;
                    *offsets++=ఆధారం_సూచీ;
                    ఆధారం_సూచీ+=2;
                } లేకపోతే {
                    ఆపు;
                }
                --count;
            }
        }

        ఒకవేళ(count==0) {
            /* done with the loop for complete UChars */
            ఒకవేళ(length>0 && లక్ష్యం_సామర్థ్యం>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                ఒకవేళ(U16_IS_SINGLE(c=*ఆధారం++)) {
                    అధికప్రవాహం[0]=(అక్షరం)c;
                    అధికప్రవాహం[1]=(అక్షరం)(c>>8);
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } లేకపోతే {
                length=0;
                c=0;
            }
        } లేకపోతే {
            /* keep c for surrogate handling, length will be set there */
            లక్ష్యం_సామర్థ్యం+=2*count;
        }
    } లేకపోతే {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    ఒకవేళ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
            ఒకవేళ(ఆధారం<p_వాదనలు->ఆధారం_హద్దు) {
                ఒకవేళ(U16_IS_TRAIL(trail=*ఆధారం)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++ఆధారం;
                    అధికప్రవాహం[0]=(అక్షరం)c;
                    అధికప్రవాహం[1]=(అక్షరం)(c>>8);
                    అధికప్రవాహం[2]=(అక్షరం)trail;
                    అధికప్రవాహం[3]=(అక్షరం)(trail>>8);
                    length=4; /* 4 bytes to output */
                    c=0;
                } లేకపోతే {
                    /* unmatched lead surrogate */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                }
            } లేకపోతే {
                /* see if the trail surrogate is in the next buffer */
            }
        } లేకపోతే {
            /* unmatched trail surrogate */
            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
        }
        cnv->fromUChar32=c;
    }

    ఒకవేళ(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_బైట్లు(cnv,
                             అధికప్రవాహం, length,
                             &లక్ష్యం, p_వాదనలు->లక్ష్యం_హద్దు,
                             &offsets, ఆధారం_సూచీ,
                             p_Error_సంకేతం);
        లక్ష్యం_సామర్థ్యం = static_cast<uint32_t>(p_వాదనలు->లక్ష్యం_హద్దు - లక్ష్యం);
    }

    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం) && ఆధారం<p_వాదనలు->ఆధారం_హద్దు && లక్ష్యం_సామర్థ్యం==0) {
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
    }

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=ఆధారం;
    p_వాదనలు->లక్ష్యం=లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

స్థిర శూన్య  U_CALLCONV
_UTF16LEToUnicodeWithOffsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                             UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి uint8_t *ఆధారం;
    char16_t *లక్ష్యం;
    int32_t *offsets;

    uint32_t లక్ష్యం_సామర్థ్యం, length, count, ఆధారం_సూచీ;
    char16_t c, trail;

    ఒకవేళ(p_వాదనలు->converter->బహుళకం<8) {
        UTF16_గమ్యం_Unicode_With_Offsets(p_వాదనలు, p_Error_సంకేతం);
        తిరిగి;
    }

    cnv=p_వాదనలు->converter;
    ఆధారం=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
    length=(int32_t)((స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం_హద్దు-ఆధారం);
    ఒకవేళ(length<=0 && cnv->గమ్యం_Unicode_స్థితి==0) {
        /* no input, nothing to do */
        తిరిగి;
    }

    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    ఒకవేళ(లక్ష్యం >= p_వాదనలు->లక్ష్యం_హద్దు) {
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి;
    }

    లక్ష్యం_సామర్థ్యం=(uint32_t)(p_వాదనలు->లక్ష్యం_హద్దు-p_వాదనలు->లక్ష్యం);
    offsets=p_వాదనలు->offsets;
    ఆధారం_సూచీ=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    ఒకవేళ(cnv->గమ్యం_Unicode_స్థితి!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->గమ్యం_UBytes[0]=(uint8_t)cnv->గమ్యం_Unicode_స్థితి;
        cnv->గమ్యం_ULength=1;
        cnv->గమ్యం_Unicode_స్థితి=0;
    }
    ఒకవేళ((count=cnv->గమ్యం_ULength)!=0) {
        uint8_t *p=cnv->గమ్యం_UBytes;
        చేయి {
            p[count++]=*ఆధారం++;
            ++ఆధారం_సూచీ;
            --length;
            ఒకవేళ(count==2) {
                c=((char16_t)p[1]<<8)|p[0];
                ఒకవేళ(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *లక్ష్యం++=c;
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=-1;
                    }
                    --లక్ష్యం_సామర్థ్యం;
                    count=0;
                    c=0;
                    ఆపు;
                } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } లేకపోతే {
                    /* fall through to error handling for an unmatched trail surrogate */
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(count==4) {
                c=((char16_t)p[1]<<8)|p[0];
                trail=((char16_t)p[3]<<8)|p[2];
                ఒకవేళ(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *లక్ష్యం++=c;
                    ఒకవేళ(లక్ష్యం_సామర్థ్యం>=2) {
                        *లక్ష్యం++=trail;
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        లక్ష్యం_సామర్థ్యం-=2;
                    } లేకపోతే /* targetCapacity==1 */ {
                        లక్ష్యం_సామర్థ్యం=0;
                        cnv->UChar_Error_బఫరు[0]=trail;
                        cnv->UChar_Error_బఫరు_నిడివి=1;
                        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                    }
                    count=0;
                    c=0;
                    ఆపు;
                } లేకపోతే {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;

                    /* back out reading the code unit after it */
                    ఒకవేళ(((స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం-ఆధారం)>=2) {
                        ఆధారం-=2;
                    } లేకపోతే {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->గమ్యం_Unicode_స్థితి=0x100|p[2];
                        --ఆధారం;
                    }
                    cnv->గమ్యం_ULength=2;

                    /* write back the updated pointers */
                    p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
                    p_వాదనలు->లక్ష్యం=లక్ష్యం;
                    p_వాదనలు->offsets=offsets;
                    తిరిగి;
                }
            }
        } అయితే(length>0);
        cnv->గమ్యం_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*లక్ష్యం_సామర్థ్యం;
    ఒకవేళ(count>length) {
        count=length&~1;
    }
    ఒకవేళ(c==0 && count>0) {
        length-=count;
        count>>=1;
        లక్ష్యం_సామర్థ్యం-=count;
        ఒకవేళ(offsets==శూన్యసూచిక) {
            చేయి {
                c=((char16_t)ఆధారం[1]<<8)|ఆధారం[0];
                ఆధారం+=2;
                ఒకవేళ(U16_IS_SINGLE(c)) {
                    *లక్ష్యం++=c;
                } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ఆధారం[1]<<8)|ఆధారం[0])
                ) {
                    ఆధారం+=2;
                    --count;
                    *లక్ష్యం++=c;
                    *లక్ష్యం++=trail;
                } లేకపోతే {
                    ఆపు;
                }
            } అయితే(--count>0);
        } లేకపోతే {
            చేయి {
                c=((char16_t)ఆధారం[1]<<8)|ఆధారం[0];
                ఆధారం+=2;
                ఒకవేళ(U16_IS_SINGLE(c)) {
                    *లక్ష్యం++=c;
                    *offsets++=ఆధారం_సూచీ;
                    ఆధారం_సూచీ+=2;
                } లేకపోతే ఒకవేళ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ఆధారం[1]<<8)|ఆధారం[0])
                ) {
                    ఆధారం+=2;
                    --count;
                    *లక్ష్యం++=c;
                    *లక్ష్యం++=trail;
                    *offsets++=ఆధారం_సూచీ;
                    *offsets++=ఆధారం_సూచీ;
                    ఆధారం_సూచీ+=4;
                } లేకపోతే {
                    ఆపు;
                }
            } అయితే(--count>0);
        }

        ఒకవేళ(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } లేకపోతే {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            లక్ష్యం_సామర్థ్యం+=count;
        }
    }

    ఒకవేళ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->గమ్యం_UBytes[0]=(uint8_t)c;
        cnv->గమ్యం_UBytes[1]=(uint8_t)(c>>8);
        cnv->గమ్యం_ULength=2;

        ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
            ఒకవేళ(length>=2) {
                ఒకవేళ(U16_IS_TRAIL(trail=((char16_t)ఆధారం[1]<<8)|ఆధారం[0])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ఆధారం+=2;
                    length-=2;
                    *లక్ష్యం++=c;
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                    cnv->UChar_Error_బఫరు[0]=trail;
                    cnv->UChar_Error_బఫరు_నిడివి=1;
                    cnv->గమ్యం_ULength=0;
                    *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                } లేకపోతే {
                    /* unmatched lead surrogate */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                }
            } లేకపోతే {
                /* see if the trail surrogate is in the next buffer */
            }
        } లేకపోతే {
            /* unmatched trail surrogate */
            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
        }
    }

    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం)) {
        /* check for a remaining source byte */
        ఒకవేళ(length>0) {
            ఒకవేళ(లక్ష్యం_సామర్థ్యం==0) {
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            } లేకపోతే {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->గమ్యం_UBytes[cnv->గమ్యం_ULength++]=*ఆధారం++;
            }
        }
    }

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
    p_వాదనలు->లక్ష్యం=లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

స్థిర UChar32  U_CALLCONV
UTF16_LEGet_తదుపరి_UChar(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు, UError_సంకేతం *err) {
    స్థిరరాశి uint8_t *s, *ఆధారం_హద్దు;
    UChar32 c;

    ఒకవేళ(p_వాదనలు->converter->బహుళకం<8) {
        తిరిగి UCNV_GET_తదుపరి_UCHAR_USE_గమ్యం_U;
    }

    s=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం_హద్దు;

    ఒకవేళ(s>=ఆధారం_హద్దు) {
        /* no input */
        *err=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0xffff;
    }

    ఒకవేళ(s+2>ఆధారం_హద్దు) {
        /* only one byte: truncated char16_t */
        p_వాదనలు->converter->గమ్యం_UBytes[0]=*s++;
        p_వాదనలు->converter->గమ్యం_ULength=1;
        p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)s;
        *err = U_TRUNCATED_CHAR_దొరికింది;
        తిరిగి 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)s[1]<<8)|*s;
    s+=2;

    /* check for a surrogate pair */
    ఒకవేళ(U_IS_SURROGATE(c)) {
        ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
            ఒకవేళ(s+2<=ఆధారం_హద్దు) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)s[1]<<8)|*s;
                ఒకవేళ(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } లేకపోతే {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } లేకపోతే {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *బైట్లు=p_వాదనలు->converter->గమ్యం_UBytes;
                s-=2;
                p_వాదనలు->converter->గమ్యం_ULength=(int8_t)(ఆధారం_హద్దు-s);
                చేయి {
                    *బైట్లు++=*s++;
                } అయితే(s<ఆధారం_హద్దు);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_దొరికింది;
            }
        } లేకపోతే {
            /* unmatched trail surrogate */
            c=-2;
        }

        ఒకవేళ(c<0) {
            /* write the unmatched surrogate */
            uint8_t *బైట్లు=p_వాదనలు->converter->గమ్యం_UBytes;
            p_వాదనలు->converter->గమ్యం_ULength=2;
            *బైట్లు=*(s-2);
            బైట్లు[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_దొరికింది;
        }
    }

    p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)s;
    తిరిగి c;
} 

స్థిర శూన్య  U_CALLCONV
_UTF16LEReset(UConverter *cnv, UConverterResetChoice choice) {
    ఒకవేళ(choice<=UCNV_RESET_గమ్యం_UNICODE) {
        /* reset toUnicode state */
        ఒకవేళ(UCNV_GET_సంస్కరణ(cnv)==0) {
            cnv->బహుళకం=8; /* no BOM handling */
        } లేకపోతే {
            cnv->బహుళకం=0; /* Java-specific "UnicodeLittle" requires LE BOM or no BOM */
        }
    }
    ఒకవేళ(choice!=UCNV_RESET_గమ్యం_UNICODE && UCNV_GET_సంస్కరణ(cnv)==1) {
        /* reset fromUnicode for "UnicodeLittle": prepare to output the UTF-16LE BOM */
        cnv->from_Unicode_స్థితి=UCNV_NEED_గమ్యం_WRITE_BOM;
    }
}

స్థిర శూన్య  U_CALLCONV
_UTF16LEOpen(UConverter *cnv,
             UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
             UError_సంకేతం *p_Error_సంకేతం) {
    (శూన్య)p_వాదనలు;
    ఒకవేళ(UCNV_GET_సంస్కరణ(cnv)<=1) {
        _UTF16LEReset(cnv, UCNV_RESET_BOTH);
    } లేకపోతే {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
    }
}

స్థిర స్థిరరాశి అక్షరం *  U_CALLCONV
_UTF16LEGetName(స్థిరరాశి UConverter *cnv) {
    ఒకవేళ(UCNV_GET_సంస్కరణ(cnv)==0) {
        తిరిగి "UTF-16LE";
    } లేకపోతే {
        తిరిగి "UTF-16LE,version=1";
    }
}
U_CDECL_చివర

స్థిర స్థిరరాశి UConverterImpl _UTF16LEImpl={
    UCNV_UTF16_LittleEndian,

    శూన్యసూచిక,
    శూన్యసూచిక,

    _UTF16LEOpen,
    శూన్యసూచిక,
    _UTF16LEReset,

    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    UTF16_LEGet_తదుపరి_UChar,

    శూన్యసూచిక,
    _UTF16LEGetName,
    శూన్యసూచిక,
    శూన్యసూచిక,
    ucnv_get_Non_Surrogate_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};


స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం UTF16_LEStatic_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "UTF-16LE",
    1202, UCNV_IBM, UCNV_UTF16_LittleEndian, 2, 2,
    { 0xfd, 0xff, 0, 0 },2,అబద్ధం,అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


స్థిరరాశి UConverter_Shared_దత్తాంశం _UTF16LEData=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&UTF16_LEStatic_దత్తాంశం, &_UTF16LEImpl);

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
స్థిర శూన్య  U_CALLCONV
_UTF16Reset(UConverter *cnv, UConverterResetChoice choice) {
    ఒకవేళ(choice<=UCNV_RESET_గమ్యం_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->బహుళకం=0;
    }
    ఒకవేళ(choice!=UCNV_RESET_గమ్యం_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-16PE BOM */
        cnv->from_Unicode_స్థితి=UCNV_NEED_గమ్యం_WRITE_BOM;
    }
}
U_CDECL_చివర
బాహ్య స్థిరరాశి UConverter_Shared_దత్తాంశం UTF16v2_దత్తాంశం;
U_CDECL_BEGIN
స్థిర శూన్య U_CALLCONV
_UTF16Open(UConverter *cnv,
           UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
           UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(UCNV_GET_సంస్కరణ(cnv)<=2) {
        ఒకవేళ(UCNV_GET_సంస్కరణ(cnv)==2 && !p_వాదనలు->onlyTestIsLoadable) {
            /*
             * Switch implementation, and switch the staticData that's different
             * and was copied into the UConverter.
             * (See ucnv_createConverterFromSharedData() in ucnv_bld.c.)
             * UTF-16,version=2 fromUnicode() always writes a big-endian byte stream.
             */
            cnv->shared_దత్తాంశం=(UConverter_Shared_దత్తాంశం*)&UTF16v2_దత్తాంశం;
            uprv_memcpy(cnv->subChars, UTF16v2_దత్తాంశం.static_దత్తాంశం->subChar, UCNV_MAX_SUBCHAR_LEN);
        }
        _UTF16Reset(cnv, UCNV_RESET_BOTH);
    } లేకపోతే {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
    }
}

స్థిర స్థిరరాశి అక్షరం *  U_CALLCONV
_UTF16GetName(స్థిరరాశి UConverter *cnv) {
    ఒకవేళ(UCNV_GET_సంస్కరణ(cnv)==0) {
        తిరిగి "UTF-16";
    } లేకపోతే ఒకవేళ(UCNV_GET_సంస్కరణ(cnv)==1) {
        తిరిగి "UTF-16,version=1";
    } లేకపోతే {
        తిరిగి "UTF-16,version=2";
    }
}
U_CDECL_చివర
బాహ్య స్థిరరాశి UConverter_Shared_దత్తాంశం UTF16_దత్తాంశం;

స్థిర అంతఃవరుస తార్కిక IS_UTF16BE(స్థిరరాశి UConverter *cnv) {
    తిరిగి ((cnv)->shared_దత్తాంశం == &_UTF16BEData);
}

స్థిర అంతఃవరుస తార్కిక IS_UTF16LE(స్థిరరాశి UConverter *cnv) {
    తిరిగి ((cnv)->shared_దత్తాంశం == &_UTF16LEData);
}

స్థిర అంతఃవరుస తార్కిక IS_UTF16(స్థిరరాశి UConverter *cnv) {
    తిరిగి ((cnv)->shared_దత్తాంశం==&UTF16_దత్తాంశం) || ((cnv)->shared_దత్తాంశం == &UTF16v2_దత్తాంశం);
}

U_CDECL_BEGIN
స్థిర శూన్య U_CALLCONV
UTF16_గమ్యం_Unicode_With_Offsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                           UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv=p_వాదనలు->converter;
    స్థిరరాశి అక్షరం *ఆధారం=p_వాదనలు->ఆధారం;
    స్థిరరాశి అక్షరం *ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
    int32_t *offsets=p_వాదనలు->offsets;

    int32_t state, స్థానభ్రంశం_Delta;
    uint8_t b;

    state=cnv->బహుళకం;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    స్థానభ్రంశం_Delta=0;

    అయితే(ఆధారం<ఆధారం_హద్దు && U_SUCCESS(*p_Error_సంకేతం)) {
        ఎంపిక(state) {
        స్థితి 0:
            cnv->గమ్యం_UBytes[0]=(uint8_t)*ఆధారం++;
            cnv->గమ్యం_ULength=1;
            state=1;
            ఆపు;
        స్థితి 1:
            /*
             * Only inside this switch case can the state variable
             * temporarily take two additional values:
             * 6: BOM error, continue with BE
             * 7: BOM error, continue with LE
             */
            b=*ఆధారం;
            ఒకవేళ(cnv->గమ్యం_UBytes[0]==0xfe && b==0xff) {
                ఒకవేళ(IS_UTF16LE(cnv)) {
                    state=7; /* illegal reverse BOM for Java "UnicodeLittle" */
                } లేకపోతే {
                    state=8; /* detect UTF-16BE */
                }
            } లేకపోతే ఒకవేళ(cnv->గమ్యం_UBytes[0]==0xff && b==0xfe) {
                ఒకవేళ(IS_UTF16BE(cnv)) {
                    state=6; /* illegal reverse BOM for Java "UnicodeBig" */
                } లేకపోతే {
                    state=9; /* detect UTF-16LE */
                }
            } లేకపోతే ఒకవేళ((IS_UTF16(cnv) && UCNV_GET_సంస్కరణ(cnv)==1)) {
                state=6; /* illegal missing BOM for Java "Unicode" */
            }
            ఒకవేళ(state>=8) {
                /* BOM detected, consume it */
                ++ఆధారం;
                cnv->గమ్యం_ULength=0;
                స్థానభ్రంశం_Delta=(int32_t)(ఆధారం-p_వాదనలు->ఆధారం);
            } లేకపోతే ఒకవేళ(state<6) {
                /* ok: no BOM, and not a reverse BOM */
                ఒకవేళ(ఆధారం!=p_వాదనలు->ఆధారం) {
                    /* reset the source for a correct first offset */
                    ఆధారం=p_వాదనలు->ఆధారం;
                    cnv->గమ్యం_ULength=0;
                }
                ఒకవేళ(IS_UTF16LE(cnv)) {
                    /* Make Java "UnicodeLittle" default to LE. */
                    state=9;
                } లేకపోతే {
                    /* Make standard UTF-16 and Java "UnicodeBig" default to BE. */
                    state=8;
                }
            } లేకపోతే {
                /*
                 * error: missing BOM, or reverse BOM
                 * UTF-16,version=1: Java-specific "Unicode" requires a BOM.
                 * UTF-16BE,version=1: Java-specific "UnicodeBig" requires a BE BOM or no BOM.
                 * UTF-16LE,version=1: Java-specific "UnicodeLittle" requires an LE BOM or no BOM.
                 */
                /* report the non-BOM or reverse BOM as an illegal sequence */
                cnv->గమ్యం_UBytes[1]=b;
                cnv->గమ్యం_ULength=2;
                p_వాదనలు->ఆధారం=ఆధారం+1;
                /* continue with conversion if the callback resets the error */
                /*
                 * Make Java "Unicode" default to BE like standard UTF-16.
                 * Make Java "UnicodeBig" and "UnicodeLittle" default
                 * to their normal endiannesses.
                 */
                cnv->బహుళకం=state+2;
                *p_Error_సంకేతం=U_ILLEGAL_ESCAPE_SEQUENCE;
                తిరిగి;
            }
            /* convert the rest of the stream */
            cnv->బహుళకం=state;
            కొనసాగు;
        స్థితి 8:
            /* call UTF-16BE */
            p_వాదనలు->ఆధారం=ఆధారం;
            _UTF16BEToUnicodeWithOffsets(p_వాదనలు, p_Error_సంకేతం);
            ఆధారం=p_వాదనలు->ఆధారం;
            ఆపు;
        స్థితి 9:
            /* call UTF-16LE */
            p_వాదనలు->ఆధారం=ఆధారం;
            _UTF16LEToUnicodeWithOffsets(p_వాదనలు, p_Error_సంకేతం);
            ఆధారం=p_వాదనలు->ఆధారం;
            ఆపు;
        అప్రమేయ:
            ఆపు; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    ఒకవేళ(offsets!=శూన్యసూచిక && స్థానభ్రంశం_Delta!=0) {
        int32_t *offsets_హద్దు=p_వాదనలు->offsets;
        అయితే(offsets<offsets_హద్దు) {
            *offsets++ += స్థానభ్రంశం_Delta;
        }
    }

    p_వాదనలు->ఆధారం=ఆధారం;

    ఒకవేళ(ఆధారం==ఆధారం_హద్దు && p_వాదనలు->flush) {
        /* handle truncated input */
        ఎంపిక(state) {
        స్థితి 0:
            ఆపు; /* no input at all, nothing to do */
        స్థితి 8:
            _UTF16BEToUnicodeWithOffsets(p_వాదనలు, p_Error_సంకేతం);
            ఆపు;
        స్థితి 9:
            _UTF16LEToUnicodeWithOffsets(p_వాదనలు, p_Error_సంకేతం);
            ఆపు;
        అప్రమేయ:
            /* 0<state<8: framework will report truncation, nothing to do here */
            ఆపు;
        }
    }

    cnv->బహుళకం=state;
}

స్థిర UChar32 U_CALLCONV
UTF16_Get_తదుపరి_UChar(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                   UError_సంకేతం *p_Error_సంకేతం) {
    ఎంపిక(p_వాదనలు->converter->బహుళకం) {
    స్థితి 8:
        తిరిగి UTF16_BEGet_తదుపరి_UChar(p_వాదనలు, p_Error_సంకేతం);
    స్థితి 9:
        తిరిగి UTF16_LEGet_తదుపరి_UChar(p_వాదనలు, p_Error_సంకేతం);
    అప్రమేయ:
        తిరిగి UCNV_GET_తదుపరి_UCHAR_USE_గమ్యం_U;
    }
}
U_CDECL_చివర

స్థిర స్థిరరాశి UConverterImpl _UTF16Impl = {
    UCNV_UTF16,

    శూన్యసూచిక,
    శూన్యసూచిక,

    _UTF16Open,
    శూన్యసూచిక,
    _UTF16Reset,

    UTF16_గమ్యం_Unicode_With_Offsets,
    UTF16_గమ్యం_Unicode_With_Offsets,
    _UTF16PEFromUnicodeWithOffsets,
    _UTF16PEFromUnicodeWithOffsets,
    UTF16_Get_తదుపరి_UChar,

    శూన్యసూచిక, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    శూన్యసూచిక,
    శూన్యసూచిక,
    ucnv_get_Non_Surrogate_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};

స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం UTF16_Static_దత్తాంశం = {
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "UTF-16",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
#ఒకవేళ U_IS_BIG_ENDIAN
    { 0xff, 0xfd, 0, 0 }, 2,
#లేకపోతే
    { 0xfd, 0xff, 0, 0 }, 2,
#అయితే_ముగింపు
    అబద్ధం, అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

స్థిరరాశి UConverter_Shared_దత్తాంశం UTF16_దత్తాంశం =
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&UTF16_Static_దత్తాంశం, &_UTF16Impl);

స్థిర స్థిరరాశి UConverterImpl _UTF16v2Impl = {
    UCNV_UTF16,

    శూన్యసూచిక,
    శూన్యసూచిక,

    _UTF16Open,
    శూన్యసూచిక,
    _UTF16Reset,

    UTF16_గమ్యం_Unicode_With_Offsets,
    UTF16_గమ్యం_Unicode_With_Offsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_Get_తదుపరి_UChar,

    శూన్యసూచిక, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    శూన్యసూచిక,
    శూన్యసూచిక,
    ucnv_get_Non_Surrogate_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};

స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం UTF16v2_Static_దత్తాంశం = {
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "UTF-16,version=2",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
    { 0xff, 0xfd, 0, 0 }, 2,
    అబద్ధం, అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

స్థిరరాశి UConverter_Shared_దత్తాంశం UTF16v2_దత్తాంశం =
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&UTF16v2_Static_దత్తాంశం, &_UTF16v2Impl);

#అయితే_ముగింపు
