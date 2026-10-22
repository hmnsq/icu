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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "ustr_imp.h"

/* control optimizations according to the platform */
#परिभाषय LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
Latin1_गन्तव्यम्_Unicode_With_Offsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                            UError_संहिता *p_Error_संहिता) {
    ध्रुव uint8_t *source;
    char16_t *target;
    int32_t target_क्षमता, length;
    int32_t *offsets;

    int32_t source_सूचकांक;

    /* set up the local pointers */
    source=(ध्रुव uint8_t *)p_तर्काः->source;
    target=p_तर्काः->target;
    target_क्षमता=(int32_t)(p_तर्काः->target_सीमा-p_तर्काः->target);
    offsets=p_तर्काः->offsets;

    source_सूचकांक=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((ध्रुव uint8_t *)p_तर्काः->source_सीमा-source);
    यदि(length<=target_क्षमता) {
        target_क्षमता=length;
    } अन्यथा {
        /* target will be full */
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        length=target_क्षमता;
    }

    यदि(target_क्षमता>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=target_क्षमता>>3;
        length=target_क्षमता&=0x7;
        कुरु {
            target[0]=source[0];
            target[1]=source[1];
            target[2]=source[2];
            target[3]=source[3];
            target[4]=source[4];
            target[5]=source[5];
            target[6]=source[6];
            target[7]=source[7];
            target+=8;
            source+=8;
        } यावत्(--count>0);

        यदि(offsets!=शून्यसूचकः) {
            कुरु {
                offsets[0]=source_सूचकांक++;
                offsets[1]=source_सूचकांक++;
                offsets[2]=source_सूचकांक++;
                offsets[3]=source_सूचकांक++;
                offsets[4]=source_सूचकांक++;
                offsets[5]=source_सूचकांक++;
                offsets[6]=source_सूचकांक++;
                offsets[7]=source_सूचकांक++;
                offsets+=8;
            } यावत्(--loops>0);
        }
    }

    /* conversion loop */
    यावत्(target_क्षमता>0) {
        *target++=*source++;
        --target_क्षमता;
    }

    /* write back the updated pointers */
    p_तर्काः->source=(ध्रुव वर्ण *)source;
    p_तर्काः->target=target;

    /* set offsets */
    यदि(offsets!=शून्यसूचकः) {
        यावत्(length>0) {
            *offsets++=source_सूचकांक++;
            --length;
        }
        p_तर्काः->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
स्थिर UChar32 U_CALLCONV
Latin1_Get_अग्र_UChar(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                    UError_संहिता *p_Error_संहिता) {
    ध्रुव uint8_t *source=(ध्रुव uint8_t *)p_तर्काः->source;
    यदि(source<(ध्रुव uint8_t *)p_तर्काः->source_सीमा) {
        p_तर्काः->source=(ध्रुव वर्ण *)(source+1);
        प्रत्यागम *source;
    }

    /* no output because of empty input */
    *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
    प्रत्यागम 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
स्थिर शून्य U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverter_From_Unicode_तर्काः *p_तर्काः,
                              UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव char16_t *source, *source_सीमा;
    uint8_t *target, *oldTarget;
    int32_t target_क्षमता, length;
    int32_t *offsets;

    UChar32 cp;
    char16_t c, max;

    int32_t source_सूचकांक;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    source=p_तर्काः->source;
    source_सीमा=p_तर्काः->source_सीमा;
    target=oldTarget=(uint8_t *)p_तर्काः->target;
    target_क्षमता=(int32_t)(p_तर्काः->target_सीमा-p_तर्काः->target);
    offsets=p_तर्काः->offsets;

    यदि(cnv->shared_दत्त==&Latin1_दत्त) {
        max=0xff; /* Latin-1 */
    } अन्यथा {
        max=0x7f; /* US-ASCII */
    }

    /* get the converter state from UConverter */
    cp=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक= cp==0 ? 0 : -1;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(source_सीमा-source);
    यदि(length<target_क्षमता) {
        target_क्षमता=length;
    }

    /* conversion loop */
    यदि(cp!=0 && target_क्षमता>0) {
        गच्छ getTrail;
    }

#यदि LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    यदि(target_क्षमता>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=target_क्षमता>>4;
        कुरु {
            oredChars=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;
            oredChars|=u=*source++;
            *target++=(uint8_t)u;

            /* were all 16 entries really valid? */
            यदि(oredChars>max) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                भङ्ग;
            }
        } यावत्(--count>0);
        count=loops-count;
        target_क्षमता-=16*count;

        यदि(offsets!=शून्यसूचकः) {
            oldTarget+=16*count;
            यावत्(count>0) {
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                *offsets++=source_सूचकांक++;
                --count;
            }
        }
    }
#यदि_अन्तः

    /* conversion loop */
    c=0;
    यावत्(target_क्षमता>0 && (c=*source++)<=max) {
        /* convert the Unicode code point */
        *target++=(uint8_t)c;
        --target_क्षमता;
    }

    यदि(c>max) {
        cp=c;
        यदि(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } अन्यथा यदि(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            यदि(source<source_सीमा) {
                /* test the following code unit */
                char16_t trail=*source;
                यदि(U16_IS_TRAIL(trail)) {
                    ++source;
                    cp=U16_GET_SUPPLEMENTARY(cp, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } अन्यथा {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                }
            } अन्यथा {
                /* no more input */
                cnv->fromUChar32=cp;
                गच्छ no_अधिकम्_निवेशः;
            }
        } अन्यथा {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *p_Error_संहिता= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_प्राप्तम् : U_INVALID_CHAR_प्राप्तम्;
        cnv->fromUChar32=cp;
    }
no_अधिकम्_निवेशः:

    /* set offsets since the start */
    यदि(offsets!=शून्यसूचकः) {
        size_t count=target-oldTarget;
        यावत्(count>0) {
            *offsets++=source_सूचकांक++;
            --count;
        }
    }

    यदि(U_SUCCESS(*p_Error_संहिता) && source<source_सीमा && target>=(uint8_t *)p_तर्काः->target_सीमा) {
        /* target is full */
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
    }

    /* write back the updated pointers */
    p_तर्काः->source=source;
    p_तर्काः->target=(वर्ण *)target;
    p_तर्काः->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
स्थिर शून्य U_CALLCONV
ucnv_Latin1FromUTF8(UConverter_From_Unicode_तर्काः *pFromUArgs,
                    UConverter_गन्तव्यम्_Unicode_तर्काः *p_गन्तव्यम्_UArgs,
                    UError_संहिता *p_Error_संहिता) {
    UConverter *utf8;
    ध्रुव uint8_t *source, *source_सीमा;
    uint8_t *target;
    int32_t target_क्षमता;

    UChar32 c;
    uint8_t b, t1;

    /* set up the local pointers */
    utf8=p_गन्तव्यम्_UArgs->converter;
    source=(uint8_t *)p_गन्तव्यम्_UArgs->source;
    source_सीमा=(uint8_t *)p_गन्तव्यम्_UArgs->source_सीमा;
    target=(uint8_t *)pFromUArgs->target;
    target_क्षमता=(int32_t)(pFromUArgs->target_सीमा-pFromUArgs->target);

    /* get the converter state from the UTF-8 UConverter */
    यदि (utf8->गन्तव्यम्_ULength > 0) {
        c=(UChar32)utf8->गन्तव्यम्_Unicode_स्थितिः;
    } अन्यथा {
        c = 0;
    }
    यदि(c!=0 && source<source_सीमा) {
        यदि(target_क्षमता==0) {
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम;
        } अन्यथा यदि(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*source-0x80)) <= 0x3f) {
            ++source;
            *target++=(uint8_t)(((c&3)<<6)|t1);
            --target_क्षमता;

            utf8->गन्तव्यम्_Unicode_स्थितिः=0;
            utf8->गन्तव्यम्_ULength=0;
        } अन्यथा {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *p_Error_संहिता=U_USING_DEFAULT_WARNING;
            प्रत्यागम;
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
    यदि(source<source_सीमा && U8_IS_LEAD(*(source_सीमा-1))) {
        --source_सीमा;
    }

    /* conversion loop */
    यावत्(source<source_सीमा) {
        यदि(target_क्षमता>0) {
            b=*source++;
            यदि(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *target++ = b;
                --target_क्षमता;
            } अन्यथा यदि( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*source-0x80)) <= 0x3f
            ) {
                ++source;
                *target++=(uint8_t)(((b&3)<<6)|t1);
                --target_क्षमता;
            } अन्यथा {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                p_गन्तव्यम्_UArgs->source=(वर्ण *)(source-1);
                pFromUArgs->target=(वर्ण *)target;
                *p_Error_संहिता=U_USING_DEFAULT_WARNING;
                प्रत्यागम;
            }
        } अन्यथा {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     * For Latin-1, there is at most exactly one lead byte because of the
     * smaller sourceLimit adjustment logic.
     */
    यदि(U_SUCCESS(*p_Error_संहिता) && source<(source_सीमा=(uint8_t *)p_गन्तव्यम्_UArgs->source_सीमा)) {
        utf8->गन्तव्यम्_Unicode_स्थितिः=utf8->गन्तव्यम्_UBytes[0]=b=*source++;
        utf8->गन्तव्यम्_ULength=1;
        utf8->बहुलकम्=U8_गणनम्_अष्टकानि(b);
    }

    /* write back the updated pointers */
    p_गन्तव्यम्_UArgs->source=(वर्ण *)source;
    pFromUArgs->target=(वर्ण *)target;
}

स्थिर शून्य U_CALLCONV
Latin1_Get_Unicode_समुच्चयः(ध्रुव UConverter *cnv,
                     ध्रुव USetAdder *sa,
                     UConverter_Unicode_समुच्चयः which,
                     UError_संहिता *p_Error_संहिता) {
    (शून्य)cnv;
    (शून्य)which;
    (शून्य)p_Error_संहिता;
    sa->सङ्कलय_Range(sa->समुच्चयः, 0, 0xff);
}
U_CDECL_अन्तः


स्थिर ध्रुव UConverterImpl _Latin1Impl={
    UCNV_LATIN_1,

    शून्यसूचकः,
    शून्यसूचकः,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,

    Latin1_गन्तव्यम्_Unicode_With_Offsets,
    Latin1_गन्तव्यम्_Unicode_With_Offsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    Latin1_Get_अग्र_UChar,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    Latin1_Get_Unicode_समुच्चयः,

    शून्यसूचकः,
    ucnv_Latin1FromUTF8
};

स्थिर ध्रुव UConverter_Static_दत्त Latin1_Static_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ध्रुव UConverter_Shared_दत्त Latin1_दत्त=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&Latin1_Static_दत्त, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
स्थिर शून्य U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                           UError_संहिता *p_Error_संहिता) {
    ध्रुव uint8_t *source, *source_सीमा;
    char16_t *target, *oldTarget;
    int32_t target_क्षमता, length;
    int32_t *offsets;

    int32_t source_सूचकांक;

    uint8_t c;

    /* set up the local pointers */
    source=(ध्रुव uint8_t *)p_तर्काः->source;
    source_सीमा=(ध्रुव uint8_t *)p_तर्काः->source_सीमा;
    target=oldTarget=p_तर्काः->target;
    target_क्षमता=(int32_t)(p_तर्काः->target_सीमा-p_तर्काः->target);
    offsets=p_तर्काः->offsets;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(source_सीमा-source);
    यदि(length<target_क्षमता) {
        target_क्षमता=length;
    }

    यदि(target_क्षमता>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=target_क्षमता>>3;
        कुरु {
            oredChars=target[0]=source[0];
            oredChars|=target[1]=source[1];
            oredChars|=target[2]=source[2];
            oredChars|=target[3]=source[3];
            oredChars|=target[4]=source[4];
            oredChars|=target[5]=source[5];
            oredChars|=target[6]=source[6];
            oredChars|=target[7]=source[7];

            /* were all 16 entries really valid? */
            यदि(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                भङ्ग;
            }
            source+=8;
            target+=8;
        } यावत्(--count>0);
        count=loops-count;
        target_क्षमता-=count*8;

        यदि(offsets!=शून्यसूचकः) {
            oldTarget+=count*8;
            यावत्(count>0) {
                offsets[0]=source_सूचकांक++;
                offsets[1]=source_सूचकांक++;
                offsets[2]=source_सूचकांक++;
                offsets[3]=source_सूचकांक++;
                offsets[4]=source_सूचकांक++;
                offsets[5]=source_सूचकांक++;
                offsets[6]=source_सूचकांक++;
                offsets[7]=source_सूचकांक++;
                offsets+=8;
                --count;
            }
        }
    }

    /* conversion loop */
    c=0;
    यावत्(target_क्षमता>0 && (c=*source++)<=0x7f) {
        *target++=c;
        --target_क्षमता;
    }

    यदि(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=p_तर्काः->converter;
        cnv->गन्तव्यम्_UBytes[0]=c;
        cnv->गन्तव्यम्_ULength=1;
        *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
    } अन्यथा यदि(source<source_सीमा && target>=p_तर्काः->target_सीमा) {
        /* target is full */
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
    }

    /* set offsets since the start */
    यदि(offsets!=शून्यसूचकः) {
        size_t count=target-oldTarget;
        यावत्(count>0) {
            *offsets++=source_सूचकांक++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_तर्काः->source=(ध्रुव वर्ण *)source;
    p_तर्काः->target=target;
    p_तर्काः->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
स्थिर UChar32 U_CALLCONV
ASCIIGet_अग्र_UChar(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                   UError_संहिता *p_Error_संहिता) {
    ध्रुव uint8_t *source;
    uint8_t b;

    source=(ध्रुव uint8_t *)p_तर्काः->source;
    यदि(source<(ध्रुव uint8_t *)p_तर्काः->source_सीमा) {
        b=*source++;
        p_तर्काः->source=(ध्रुव वर्ण *)source;
        यदि(b<=0x7f) {
            प्रत्यागम b;
        } अन्यथा {
            UConverter *cnv=p_तर्काः->converter;
            cnv->गन्तव्यम्_UBytes[0]=b;
            cnv->गन्तव्यम्_ULength=1;
            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
            प्रत्यागम 0xffff;
        }
    }

    /* no output because of empty input */
    *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
    प्रत्यागम 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
स्थिर शून्य U_CALLCONV
ucnv_ASCIIFromUTF8(UConverter_From_Unicode_तर्काः *pFromUArgs,
                   UConverter_गन्तव्यम्_Unicode_तर्काः *p_गन्तव्यम्_UArgs,
                   UError_संहिता *p_Error_संहिता) {
    ध्रुव uint8_t *source, *source_सीमा;
    uint8_t *target;
    int32_t target_क्षमता, length;

    uint8_t c;

    यदि(p_गन्तव्यम्_UArgs->converter->गन्तव्यम्_ULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *p_Error_संहिता=U_USING_DEFAULT_WARNING;
        प्रत्यागम;
    }

    /* set up the local pointers */
    source=(ध्रुव uint8_t *)p_गन्तव्यम्_UArgs->source;
    source_सीमा=(ध्रुव uint8_t *)p_गन्तव्यम्_UArgs->source_सीमा;
    target=(uint8_t *)pFromUArgs->target;
    target_क्षमता=(int32_t)(pFromUArgs->target_सीमा-pFromUArgs->target);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(source_सीमा-source);
    यदि(length<target_क्षमता) {
        target_क्षमता=length;
    }

    /* unroll the loop with the most common case */
    यदि(target_क्षमता>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=target_क्षमता>>4;
        कुरु {
            oredChars=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;
            oredChars|=*target++=*source++;

            /* were all 16 entries really valid? */
            यदि(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                भङ्ग;
            }
        } यावत्(--count>0);
        count=loops-count;
        target_क्षमता-=16*count;
    }

    /* conversion loop */
    c=0;
    यावत्(target_क्षमता>0 && (c=*source)<=0x7f) {
        ++source;
        *target++=c;
        --target_क्षमता;
    }

    यदि(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *p_Error_संहिता=U_USING_DEFAULT_WARNING;
    } अन्यथा यदि(source<source_सीमा && target>=(ध्रुव uint8_t *)pFromUArgs->target_सीमा) {
        /* target is full */
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
    }

    /* write back the updated pointers */
    p_गन्तव्यम्_UArgs->source=(ध्रुव वर्ण *)source;
    pFromUArgs->target=(वर्ण *)target;
}

स्थिर शून्य U_CALLCONV
ASCIIGet_Unicode_समुच्चयः(ध्रुव UConverter *cnv,
                    ध्रुव USetAdder *sa,
                    UConverter_Unicode_समुच्चयः which,
                    UError_संहिता *p_Error_संहिता) {
    (शून्य)cnv;
    (शून्य)which;
    (शून्य)p_Error_संहिता;
    sa->सङ्कलय_Range(sa->समुच्चयः, 0, 0x7f);
}
U_CDECL_अन्तः

स्थिर ध्रुव UConverterImpl _ASCIIImpl={
    UCNV_US_ASCII,

    शून्यसूचकः,
    शून्यसूचकः,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,

    _ASCIIToUnicodeWithOffsets,
    _ASCIIToUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    ASCIIGet_अग्र_UChar,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    ASCIIGet_Unicode_समुच्चयः,

    शून्यसूचकः,
    ucnv_ASCIIFromUTF8
};

स्थिर ध्रुव UConverter_Static_दत्त ASCIIStatic_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ध्रुव UConverter_Shared_दत्त _ASCIIData=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&ASCIIStatic_दत्त, &_ASCIIImpl);

#यदि_अन्तः
