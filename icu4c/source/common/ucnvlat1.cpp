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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "ustr_imp.h"

/* control optimizations according to the platform */
#నిర్వచించు LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
స్థిర శూన్య U_CALLCONV
Latin1_గమ్యం_Unicode_With_Offsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                            UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి uint8_t *ఆధారం;
    char16_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం, length;
    int32_t *offsets;

    int32_t ఆధారం_సూచీ;

    /* set up the local pointers */
    ఆధారం=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    లక్ష్యం_సామర్థ్యం=(int32_t)(p_వాదనలు->లక్ష్యం_హద్దు-p_వాదనలు->లక్ష్యం);
    offsets=p_వాదనలు->offsets;

    ఆధారం_సూచీ=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం_హద్దు-ఆధారం);
    ఒకవేళ(length<=లక్ష్యం_సామర్థ్యం) {
        లక్ష్యం_సామర్థ్యం=length;
    } లేకపోతే {
        /* target will be full */
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        length=లక్ష్యం_సామర్థ్యం;
    }

    ఒకవేళ(లక్ష్యం_సామర్థ్యం>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=లక్ష్యం_సామర్థ్యం>>3;
        length=లక్ష్యం_సామర్థ్యం&=0x7;
        చేయి {
            లక్ష్యం[0]=ఆధారం[0];
            లక్ష్యం[1]=ఆధారం[1];
            లక్ష్యం[2]=ఆధారం[2];
            లక్ష్యం[3]=ఆధారం[3];
            లక్ష్యం[4]=ఆధారం[4];
            లక్ష్యం[5]=ఆధారం[5];
            లక్ష్యం[6]=ఆధారం[6];
            లక్ష్యం[7]=ఆధారం[7];
            లక్ష్యం+=8;
            ఆధారం+=8;
        } అయితే(--count>0);

        ఒకవేళ(offsets!=శూన్యసూచిక) {
            చేయి {
                offsets[0]=ఆధారం_సూచీ++;
                offsets[1]=ఆధారం_సూచీ++;
                offsets[2]=ఆధారం_సూచీ++;
                offsets[3]=ఆధారం_సూచీ++;
                offsets[4]=ఆధారం_సూచీ++;
                offsets[5]=ఆధారం_సూచీ++;
                offsets[6]=ఆధారం_సూచీ++;
                offsets[7]=ఆధారం_సూచీ++;
                offsets+=8;
            } అయితే(--loops>0);
        }
    }

    /* conversion loop */
    అయితే(లక్ష్యం_సామర్థ్యం>0) {
        *లక్ష్యం++=*ఆధారం++;
        --లక్ష్యం_సామర్థ్యం;
    }

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
    p_వాదనలు->లక్ష్యం=లక్ష్యం;

    /* set offsets */
    ఒకవేళ(offsets!=శూన్యసూచిక) {
        అయితే(length>0) {
            *offsets++=ఆధారం_సూచీ++;
            --length;
        }
        p_వాదనలు->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
స్థిర UChar32 U_CALLCONV
Latin1_Get_తదుపరి_UChar(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                    UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి uint8_t *ఆధారం=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
    ఒకవేళ(ఆధారం<(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం_హద్దు) {
        p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)(ఆధారం+1);
        తిరిగి *ఆధారం;
    }

    /* no output because of empty input */
    *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
    తిరిగి 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
స్థిర శూన్య U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
                              UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి char16_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం, *old_లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం, length;
    int32_t *offsets;

    UChar32 cp;
    char16_t c, max;

    int32_t ఆధారం_సూచీ;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    ఆధారం=p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం=old_లక్ష్యం=(uint8_t *)p_వాదనలు->లక్ష్యం;
    లక్ష్యం_సామర్థ్యం=(int32_t)(p_వాదనలు->లక్ష్యం_హద్దు-p_వాదనలు->లక్ష్యం);
    offsets=p_వాదనలు->offsets;

    ఒకవేళ(cnv->shared_దత్తాంశం==&Latin1_దత్తాంశం) {
        max=0xff; /* Latin-1 */
    } లేకపోతే {
        max=0x7f; /* US-ASCII */
    }

    /* get the converter state from UConverter */
    cp=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ= cp==0 ? 0 : -1;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ఆధారం_హద్దు-ఆధారం);
    ఒకవేళ(length<లక్ష్యం_సామర్థ్యం) {
        లక్ష్యం_సామర్థ్యం=length;
    }

    /* conversion loop */
    ఒకవేళ(cp!=0 && లక్ష్యం_సామర్థ్యం>0) {
        వెళ్ళు getTrail;
    }

#ఒకవేళ LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    ఒకవేళ(లక్ష్యం_సామర్థ్యం>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=లక్ష్యం_సామర్థ్యం>>4;
        చేయి {
            oredChars=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;
            oredChars|=u=*ఆధారం++;
            *లక్ష్యం++=(uint8_t)u;

            /* were all 16 entries really valid? */
            ఒకవేళ(oredChars>max) {
                /* no, return to the first of these 16 */
                ఆధారం-=16;
                లక్ష్యం-=16;
                ఆపు;
            }
        } అయితే(--count>0);
        count=loops-count;
        లక్ష్యం_సామర్థ్యం-=16*count;

        ఒకవేళ(offsets!=శూన్యసూచిక) {
            old_లక్ష్యం+=16*count;
            అయితే(count>0) {
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                *offsets++=ఆధారం_సూచీ++;
                --count;
            }
        }
    }
#అయితే_ముగింపు

    /* conversion loop */
    c=0;
    అయితే(లక్ష్యం_సామర్థ్యం>0 && (c=*ఆధారం++)<=max) {
        /* convert the Unicode code point */
        *లక్ష్యం++=(uint8_t)c;
        --లక్ష్యం_సామర్థ్యం;
    }

    ఒకవేళ(c>max) {
        cp=c;
        ఒకవేళ(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } లేకపోతే ఒకవేళ(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            ఒకవేళ(ఆధారం<ఆధారం_హద్దు) {
                /* test the following code unit */
                char16_t trail=*ఆధారం;
                ఒకవేళ(U16_IS_TRAIL(trail)) {
                    ++ఆధారం;
                    cp=U16_GET_SUPPLEMENTARY(cp, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } లేకపోతే {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                }
            } లేకపోతే {
                /* no more input */
                cnv->fromUChar32=cp;
                వెళ్ళు no_మరిన్ని_ఆదానం;
            }
        } లేకపోతే {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *p_Error_సంకేతం= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_దొరికింది : U_INVALID_CHAR_దొరికింది;
        cnv->fromUChar32=cp;
    }
no_మరిన్ని_ఆదానం:

    /* set offsets since the start */
    ఒకవేళ(offsets!=శూన్యసూచిక) {
        size_t count=లక్ష్యం-old_లక్ష్యం;
        అయితే(count>0) {
            *offsets++=ఆధారం_సూచీ++;
            --count;
        }
    }

    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం) && ఆధారం<ఆధారం_హద్దు && లక్ష్యం>=(uint8_t *)p_వాదనలు->లక్ష్యం_హద్దు) {
        /* target is full */
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
    }

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=ఆధారం;
    p_వాదనలు->లక్ష్యం=(అక్షరం *)లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
స్థిర శూన్య U_CALLCONV
ucnv_Latin1FromUTF8(UConverter_From_Unicode_వాదనలు *pFromUArgs,
                    UConverter_గమ్యం_Unicode_వాదనలు *p_గమ్యం_UArgs,
                    UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *utf8;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం;

    UChar32 c;
    uint8_t b, t1;

    /* set up the local pointers */
    utf8=p_గమ్యం_UArgs->converter;
    ఆధారం=(uint8_t *)p_గమ్యం_UArgs->ఆధారం;
    ఆధారం_హద్దు=(uint8_t *)p_గమ్యం_UArgs->ఆధారం_హద్దు;
    లక్ష్యం=(uint8_t *)pFromUArgs->లక్ష్యం;
    లక్ష్యం_సామర్థ్యం=(int32_t)(pFromUArgs->లక్ష్యం_హద్దు-pFromUArgs->లక్ష్యం);

    /* get the converter state from the UTF-8 UConverter */
    ఒకవేళ (utf8->గమ్యం_ULength > 0) {
        c=(UChar32)utf8->గమ్యం_Unicode_స్థితి;
    } లేకపోతే {
        c = 0;
    }
    ఒకవేళ(c!=0 && ఆధారం<ఆధారం_హద్దు) {
        ఒకవేళ(లక్ష్యం_సామర్థ్యం==0) {
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి;
        } లేకపోతే ఒకవేళ(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*ఆధారం-0x80)) <= 0x3f) {
            ++ఆధారం;
            *లక్ష్యం++=(uint8_t)(((c&3)<<6)|t1);
            --లక్ష్యం_సామర్థ్యం;

            utf8->గమ్యం_Unicode_స్థితి=0;
            utf8->గమ్యం_ULength=0;
        } లేకపోతే {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *p_Error_సంకేతం=U_USING_DEFAULT_WARNING;
            తిరిగి;
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
    ఒకవేళ(ఆధారం<ఆధారం_హద్దు && U8_IS_LEAD(*(ఆధారం_హద్దు-1))) {
        --ఆధారం_హద్దు;
    }

    /* conversion loop */
    అయితే(ఆధారం<ఆధారం_హద్దు) {
        ఒకవేళ(లక్ష్యం_సామర్థ్యం>0) {
            b=*ఆధారం++;
            ఒకవేళ(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *లక్ష్యం++ = b;
                --లక్ష్యం_సామర్థ్యం;
            } లేకపోతే ఒకవేళ( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*ఆధారం-0x80)) <= 0x3f
            ) {
                ++ఆధారం;
                *లక్ష్యం++=(uint8_t)(((b&3)<<6)|t1);
                --లక్ష్యం_సామర్థ్యం;
            } లేకపోతే {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                p_గమ్యం_UArgs->ఆధారం=(అక్షరం *)(ఆధారం-1);
                pFromUArgs->లక్ష్యం=(అక్షరం *)లక్ష్యం;
                *p_Error_సంకేతం=U_USING_DEFAULT_WARNING;
                తిరిగి;
            }
        } లేకపోతే {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     * For Latin-1, there is at most exactly one lead byte because of the
     * smaller sourceLimit adjustment logic.
     */
    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం) && ఆధారం<(ఆధారం_హద్దు=(uint8_t *)p_గమ్యం_UArgs->ఆధారం_హద్దు)) {
        utf8->గమ్యం_Unicode_స్థితి=utf8->గమ్యం_UBytes[0]=b=*ఆధారం++;
        utf8->గమ్యం_ULength=1;
        utf8->బహుళకం=U8_లెక్క_బైట్లు(b);
    }

    /* write back the updated pointers */
    p_గమ్యం_UArgs->ఆధారం=(అక్షరం *)ఆధారం;
    pFromUArgs->లక్ష్యం=(అక్షరం *)లక్ష్యం;
}

స్థిర శూన్య U_CALLCONV
Latin1_Get_Unicode_సమితి(స్థిరరాశి UConverter *cnv,
                     స్థిరరాశి USetAdder *sa,
                     UConverter_Unicode_సమితి which,
                     UError_సంకేతం *p_Error_సంకేతం) {
    (శూన్య)cnv;
    (శూన్య)which;
    (శూన్య)p_Error_సంకేతం;
    sa->కూడు_Range(sa->సమితి, 0, 0xff);
}
U_CDECL_చివర


స్థిర స్థిరరాశి UConverterImpl _Latin1Impl={
    UCNV_LATIN_1,

    శూన్యసూచిక,
    శూన్యసూచిక,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,

    Latin1_గమ్యం_Unicode_With_Offsets,
    Latin1_గమ్యం_Unicode_With_Offsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    Latin1_Get_తదుపరి_UChar,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    Latin1_Get_Unicode_సమితి,

    శూన్యసూచిక,
    ucnv_Latin1FromUTF8
};

స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం Latin1_Static_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, అబద్ధం, అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

స్థిరరాశి UConverter_Shared_దత్తాంశం Latin1_దత్తాంశం=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&Latin1_Static_దత్తాంశం, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
స్థిర శూన్య U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                           UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    char16_t *లక్ష్యం, *old_లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం, length;
    int32_t *offsets;

    int32_t ఆధారం_సూచీ;

    uint8_t c;

    /* set up the local pointers */
    ఆధారం=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం=old_లక్ష్యం=p_వాదనలు->లక్ష్యం;
    లక్ష్యం_సామర్థ్యం=(int32_t)(p_వాదనలు->లక్ష్యం_హద్దు-p_వాదనలు->లక్ష్యం);
    offsets=p_వాదనలు->offsets;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ఆధారం_హద్దు-ఆధారం);
    ఒకవేళ(length<లక్ష్యం_సామర్థ్యం) {
        లక్ష్యం_సామర్థ్యం=length;
    }

    ఒకవేళ(లక్ష్యం_సామర్థ్యం>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=లక్ష్యం_సామర్థ్యం>>3;
        చేయి {
            oredChars=లక్ష్యం[0]=ఆధారం[0];
            oredChars|=లక్ష్యం[1]=ఆధారం[1];
            oredChars|=లక్ష్యం[2]=ఆధారం[2];
            oredChars|=లక్ష్యం[3]=ఆధారం[3];
            oredChars|=లక్ష్యం[4]=ఆధారం[4];
            oredChars|=లక్ష్యం[5]=ఆధారం[5];
            oredChars|=లక్ష్యం[6]=ఆధారం[6];
            oredChars|=లక్ష్యం[7]=ఆధారం[7];

            /* were all 16 entries really valid? */
            ఒకవేళ(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                ఆపు;
            }
            ఆధారం+=8;
            లక్ష్యం+=8;
        } అయితే(--count>0);
        count=loops-count;
        లక్ష్యం_సామర్థ్యం-=count*8;

        ఒకవేళ(offsets!=శూన్యసూచిక) {
            old_లక్ష్యం+=count*8;
            అయితే(count>0) {
                offsets[0]=ఆధారం_సూచీ++;
                offsets[1]=ఆధారం_సూచీ++;
                offsets[2]=ఆధారం_సూచీ++;
                offsets[3]=ఆధారం_సూచీ++;
                offsets[4]=ఆధారం_సూచీ++;
                offsets[5]=ఆధారం_సూచీ++;
                offsets[6]=ఆధారం_సూచీ++;
                offsets[7]=ఆధారం_సూచీ++;
                offsets+=8;
                --count;
            }
        }
    }

    /* conversion loop */
    c=0;
    అయితే(లక్ష్యం_సామర్థ్యం>0 && (c=*ఆధారం++)<=0x7f) {
        *లక్ష్యం++=c;
        --లక్ష్యం_సామర్థ్యం;
    }

    ఒకవేళ(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=p_వాదనలు->converter;
        cnv->గమ్యం_UBytes[0]=c;
        cnv->గమ్యం_ULength=1;
        *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
    } లేకపోతే ఒకవేళ(ఆధారం<ఆధారం_హద్దు && లక్ష్యం>=p_వాదనలు->లక్ష్యం_హద్దు) {
        /* target is full */
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
    }

    /* set offsets since the start */
    ఒకవేళ(offsets!=శూన్యసూచిక) {
        size_t count=లక్ష్యం-old_లక్ష్యం;
        అయితే(count>0) {
            *offsets++=ఆధారం_సూచీ++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
    p_వాదనలు->లక్ష్యం=లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
స్థిర UChar32 U_CALLCONV
ASCIIGet_తదుపరి_UChar(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                   UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి uint8_t *ఆధారం;
    uint8_t b;

    ఆధారం=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
    ఒకవేళ(ఆధారం<(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం_హద్దు) {
        b=*ఆధారం++;
        p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
        ఒకవేళ(b<=0x7f) {
            తిరిగి b;
        } లేకపోతే {
            UConverter *cnv=p_వాదనలు->converter;
            cnv->గమ్యం_UBytes[0]=b;
            cnv->గమ్యం_ULength=1;
            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
            తిరిగి 0xffff;
        }
    }

    /* no output because of empty input */
    *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
    తిరిగి 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
స్థిర శూన్య U_CALLCONV
ucnv_ASCIIFromUTF8(UConverter_From_Unicode_వాదనలు *pFromUArgs,
                   UConverter_గమ్యం_Unicode_వాదనలు *p_గమ్యం_UArgs,
                   UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం, length;

    uint8_t c;

    ఒకవేళ(p_గమ్యం_UArgs->converter->గమ్యం_ULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *p_Error_సంకేతం=U_USING_DEFAULT_WARNING;
        తిరిగి;
    }

    /* set up the local pointers */
    ఆధారం=(స్థిరరాశి uint8_t *)p_గమ్యం_UArgs->ఆధారం;
    ఆధారం_హద్దు=(స్థిరరాశి uint8_t *)p_గమ్యం_UArgs->ఆధారం_హద్దు;
    లక్ష్యం=(uint8_t *)pFromUArgs->లక్ష్యం;
    లక్ష్యం_సామర్థ్యం=(int32_t)(pFromUArgs->లక్ష్యం_హద్దు-pFromUArgs->లక్ష్యం);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ఆధారం_హద్దు-ఆధారం);
    ఒకవేళ(length<లక్ష్యం_సామర్థ్యం) {
        లక్ష్యం_సామర్థ్యం=length;
    }

    /* unroll the loop with the most common case */
    ఒకవేళ(లక్ష్యం_సామర్థ్యం>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=లక్ష్యం_సామర్థ్యం>>4;
        చేయి {
            oredChars=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;
            oredChars|=*లక్ష్యం++=*ఆధారం++;

            /* were all 16 entries really valid? */
            ఒకవేళ(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                ఆధారం-=16;
                లక్ష్యం-=16;
                ఆపు;
            }
        } అయితే(--count>0);
        count=loops-count;
        లక్ష్యం_సామర్థ్యం-=16*count;
    }

    /* conversion loop */
    c=0;
    అయితే(లక్ష్యం_సామర్థ్యం>0 && (c=*ఆధారం)<=0x7f) {
        ++ఆధారం;
        *లక్ష్యం++=c;
        --లక్ష్యం_సామర్థ్యం;
    }

    ఒకవేళ(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *p_Error_సంకేతం=U_USING_DEFAULT_WARNING;
    } లేకపోతే ఒకవేళ(ఆధారం<ఆధారం_హద్దు && లక్ష్యం>=(స్థిరరాశి uint8_t *)pFromUArgs->లక్ష్యం_హద్దు) {
        /* target is full */
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
    }

    /* write back the updated pointers */
    p_గమ్యం_UArgs->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
    pFromUArgs->లక్ష్యం=(అక్షరం *)లక్ష్యం;
}

స్థిర శూన్య U_CALLCONV
ASCIIGet_Unicode_సమితి(స్థిరరాశి UConverter *cnv,
                    స్థిరరాశి USetAdder *sa,
                    UConverter_Unicode_సమితి which,
                    UError_సంకేతం *p_Error_సంకేతం) {
    (శూన్య)cnv;
    (శూన్య)which;
    (శూన్య)p_Error_సంకేతం;
    sa->కూడు_Range(sa->సమితి, 0, 0x7f);
}
U_CDECL_చివర

స్థిర స్థిరరాశి UConverterImpl _ASCIIImpl={
    UCNV_US_ASCII,

    శూన్యసూచిక,
    శూన్యసూచిక,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,

    _ASCIIToUnicodeWithOffsets,
    _ASCIIToUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    ASCIIGet_తదుపరి_UChar,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    ASCIIGet_Unicode_సమితి,

    శూన్యసూచిక,
    ucnv_ASCIIFromUTF8
};

స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం ASCIIStatic_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, అబద్ధం, అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

స్థిరరాశి UConverter_Shared_దత్తాంశం _ASCIIData=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&ASCIIStatic_దత్తాంశం, &_ASCIIImpl);

#అయితే_ముగింపు
