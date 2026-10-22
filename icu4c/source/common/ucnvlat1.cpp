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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل "unicode/ucnv.h"
#شامل "unicode/uset.h"
#شامل "unicode/utf8.h"
#شامل "ucnv_bld.h"
#شامل "ucnv_cnv.h"
#شامل "ustr_imp.h"

/* control optimizations according to the platform */
#تعریف_کرو LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
ساکن خالی U_CALLCONV
Latin1_مقصد_Unicode_With_Offsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                            UError_رمز *p_Error_رمز) {
    ثابت uint8_t *ماخذ;
    char16_t *ہدف;
    int32_t ہدف_استعداد, length;
    int32_t *offsets;

    int32_t ماخذ_اشاریہ;

    /* set up the local pointers */
    ماخذ=(ثابت uint8_t *)p_دلائل->ماخذ;
    ہدف=p_دلائل->ہدف;
    ہدف_استعداد=(int32_t)(p_دلائل->ہدف_حد_بندی-p_دلائل->ہدف);
    offsets=p_دلائل->offsets;

    ماخذ_اشاریہ=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((ثابت uint8_t *)p_دلائل->ماخذ_حد_بندی-ماخذ);
    اگر(length<=ہدف_استعداد) {
        ہدف_استعداد=length;
    } ورنہ {
        /* target will be full */
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        length=ہدف_استعداد;
    }

    اگر(ہدف_استعداد>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=ہدف_استعداد>>3;
        length=ہدف_استعداد&=0x7;
        کرو {
            ہدف[0]=ماخذ[0];
            ہدف[1]=ماخذ[1];
            ہدف[2]=ماخذ[2];
            ہدف[3]=ماخذ[3];
            ہدف[4]=ماخذ[4];
            ہدف[5]=ماخذ[5];
            ہدف[6]=ماخذ[6];
            ہدف[7]=ماخذ[7];
            ہدف+=8;
            ماخذ+=8;
        } جب(--count>0);

        اگر(offsets!=خالی_اشاریہ) {
            کرو {
                offsets[0]=ماخذ_اشاریہ++;
                offsets[1]=ماخذ_اشاریہ++;
                offsets[2]=ماخذ_اشاریہ++;
                offsets[3]=ماخذ_اشاریہ++;
                offsets[4]=ماخذ_اشاریہ++;
                offsets[5]=ماخذ_اشاریہ++;
                offsets[6]=ماخذ_اشاریہ++;
                offsets[7]=ماخذ_اشاریہ++;
                offsets+=8;
            } جب(--loops>0);
        }
    }

    /* conversion loop */
    جب(ہدف_استعداد>0) {
        *ہدف++=*ماخذ++;
        --ہدف_استعداد;
    }

    /* write back the updated pointers */
    p_دلائل->ماخذ=(ثابت حرف *)ماخذ;
    p_دلائل->ہدف=ہدف;

    /* set offsets */
    اگر(offsets!=خالی_اشاریہ) {
        جب(length>0) {
            *offsets++=ماخذ_اشاریہ++;
            --length;
        }
        p_دلائل->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
ساکن UChar32 U_CALLCONV
Latin1_Get_اگلا_UChar(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                    UError_رمز *p_Error_رمز) {
    ثابت uint8_t *ماخذ=(ثابت uint8_t *)p_دلائل->ماخذ;
    اگر(ماخذ<(ثابت uint8_t *)p_دلائل->ماخذ_حد_بندی) {
        p_دلائل->ماخذ=(ثابت حرف *)(ماخذ+1);
        واپس *ماخذ;
    }

    /* no output because of empty input */
    *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
    واپس 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
ساکن خالی U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverter_From_Unicode_دلائل *p_دلائل,
                              UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت char16_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف, *old_ہدف;
    int32_t ہدف_استعداد, length;
    int32_t *offsets;

    UChar32 cp;
    char16_t c, max;

    int32_t ماخذ_اشاریہ;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    ماخذ=p_دلائل->ماخذ;
    ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
    ہدف=old_ہدف=(uint8_t *)p_دلائل->ہدف;
    ہدف_استعداد=(int32_t)(p_دلائل->ہدف_حد_بندی-p_دلائل->ہدف);
    offsets=p_دلائل->offsets;

    اگر(cnv->shared_ڈیٹا==&Latin1_ڈیٹا) {
        max=0xff; /* Latin-1 */
    } ورنہ {
        max=0x7f; /* US-ASCII */
    }

    /* get the converter state from UConverter */
    cp=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ= cp==0 ? 0 : -1;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ماخذ_حد_بندی-ماخذ);
    اگر(length<ہدف_استعداد) {
        ہدف_استعداد=length;
    }

    /* conversion loop */
    اگر(cp!=0 && ہدف_استعداد>0) {
        جاؤ getTrail;
    }

#اگر LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    اگر(ہدف_استعداد>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=ہدف_استعداد>>4;
        کرو {
            oredChars=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;
            oredChars|=u=*ماخذ++;
            *ہدف++=(uint8_t)u;

            /* were all 16 entries really valid? */
            اگر(oredChars>max) {
                /* no, return to the first of these 16 */
                ماخذ-=16;
                ہدف-=16;
                توڑو;
            }
        } جب(--count>0);
        count=loops-count;
        ہدف_استعداد-=16*count;

        اگر(offsets!=خالی_اشاریہ) {
            old_ہدف+=16*count;
            جب(count>0) {
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                *offsets++=ماخذ_اشاریہ++;
                --count;
            }
        }
    }
#اگر_ختم

    /* conversion loop */
    c=0;
    جب(ہدف_استعداد>0 && (c=*ماخذ++)<=max) {
        /* convert the Unicode code point */
        *ہدف++=(uint8_t)c;
        --ہدف_استعداد;
    }

    اگر(c>max) {
        cp=c;
        اگر(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } ورنہ اگر(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            اگر(ماخذ<ماخذ_حد_بندی) {
                /* test the following code unit */
                char16_t trail=*ماخذ;
                اگر(U16_IS_TRAIL(trail)) {
                    ++ماخذ;
                    cp=U16_GET_SUPPLEMENTARY(cp, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } ورنہ {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                }
            } ورنہ {
                /* no more input */
                cnv->fromUChar32=cp;
                جاؤ no_مزید_داخلہ;
            }
        } ورنہ {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *p_Error_رمز= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_ملا : U_INVALID_CHAR_ملا;
        cnv->fromUChar32=cp;
    }
no_مزید_داخلہ:

    /* set offsets since the start */
    اگر(offsets!=خالی_اشاریہ) {
        size_t count=ہدف-old_ہدف;
        جب(count>0) {
            *offsets++=ماخذ_اشاریہ++;
            --count;
        }
    }

    اگر(U_SUCCESS(*p_Error_رمز) && ماخذ<ماخذ_حد_بندی && ہدف>=(uint8_t *)p_دلائل->ہدف_حد_بندی) {
        /* target is full */
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
    }

    /* write back the updated pointers */
    p_دلائل->ماخذ=ماخذ;
    p_دلائل->ہدف=(حرف *)ہدف;
    p_دلائل->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
ساکن خالی U_CALLCONV
ucnv_Latin1FromUTF8(UConverter_From_Unicode_دلائل *pFromUArgs,
                    UConverter_مقصد_Unicode_دلائل *p_مقصد_UArgs,
                    UError_رمز *p_Error_رمز) {
    UConverter *utf8;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف;
    int32_t ہدف_استعداد;

    UChar32 c;
    uint8_t b, t1;

    /* set up the local pointers */
    utf8=p_مقصد_UArgs->converter;
    ماخذ=(uint8_t *)p_مقصد_UArgs->ماخذ;
    ماخذ_حد_بندی=(uint8_t *)p_مقصد_UArgs->ماخذ_حد_بندی;
    ہدف=(uint8_t *)pFromUArgs->ہدف;
    ہدف_استعداد=(int32_t)(pFromUArgs->ہدف_حد_بندی-pFromUArgs->ہدف);

    /* get the converter state from the UTF-8 UConverter */
    اگر (utf8->مقصد_ULength > 0) {
        c=(UChar32)utf8->مقصد_Unicode_حالت;
    } ورنہ {
        c = 0;
    }
    اگر(c!=0 && ماخذ<ماخذ_حد_بندی) {
        اگر(ہدف_استعداد==0) {
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            واپس;
        } ورنہ اگر(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*ماخذ-0x80)) <= 0x3f) {
            ++ماخذ;
            *ہدف++=(uint8_t)(((c&3)<<6)|t1);
            --ہدف_استعداد;

            utf8->مقصد_Unicode_حالت=0;
            utf8->مقصد_ULength=0;
        } ورنہ {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *p_Error_رمز=U_USING_DEFAULT_WARNING;
            واپس;
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
    اگر(ماخذ<ماخذ_حد_بندی && U8_IS_LEAD(*(ماخذ_حد_بندی-1))) {
        --ماخذ_حد_بندی;
    }

    /* conversion loop */
    جب(ماخذ<ماخذ_حد_بندی) {
        اگر(ہدف_استعداد>0) {
            b=*ماخذ++;
            اگر(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *ہدف++ = b;
                --ہدف_استعداد;
            } ورنہ اگر( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*ماخذ-0x80)) <= 0x3f
            ) {
                ++ماخذ;
                *ہدف++=(uint8_t)(((b&3)<<6)|t1);
                --ہدف_استعداد;
            } ورنہ {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                p_مقصد_UArgs->ماخذ=(حرف *)(ماخذ-1);
                pFromUArgs->ہدف=(حرف *)ہدف;
                *p_Error_رمز=U_USING_DEFAULT_WARNING;
                واپس;
            }
        } ورنہ {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     * For Latin-1, there is at most exactly one lead byte because of the
     * smaller sourceLimit adjustment logic.
     */
    اگر(U_SUCCESS(*p_Error_رمز) && ماخذ<(ماخذ_حد_بندی=(uint8_t *)p_مقصد_UArgs->ماخذ_حد_بندی)) {
        utf8->مقصد_Unicode_حالت=utf8->مقصد_UBytes[0]=b=*ماخذ++;
        utf8->مقصد_ULength=1;
        utf8->منوال=U8_گنتی_بائٹیں(b);
    }

    /* write back the updated pointers */
    p_مقصد_UArgs->ماخذ=(حرف *)ماخذ;
    pFromUArgs->ہدف=(حرف *)ہدف;
}

ساکن خالی U_CALLCONV
Latin1_Get_Unicode_سیٹ(ثابت UConverter *cnv,
                     ثابت USetAdder *sa,
                     UConverter_Unicode_سیٹ which,
                     UError_رمز *p_Error_رمز) {
    (خالی)cnv;
    (خالی)which;
    (خالی)p_Error_رمز;
    sa->جوڑو_Range(sa->سیٹ, 0, 0xff);
}
U_CDECL_انتہا


ساکن ثابت UConverterImpl _Latin1Impl={
    UCNV_LATIN_1,

    خالی_اشاریہ,
    خالی_اشاریہ,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,

    Latin1_مقصد_Unicode_With_Offsets,
    Latin1_مقصد_Unicode_With_Offsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    Latin1_Get_اگلا_UChar,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    Latin1_Get_Unicode_سیٹ,

    خالی_اشاریہ,
    ucnv_Latin1FromUTF8
};

ساکن ثابت UConverter_Static_ڈیٹا Latin1_Static_ڈیٹا={
    حجم(UConverter_Static_ڈیٹا),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, جھوٹ, جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_ڈیٹا Latin1_ڈیٹا=
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&Latin1_Static_ڈیٹا, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
ساکن خالی U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                           UError_رمز *p_Error_رمز) {
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    char16_t *ہدف, *old_ہدف;
    int32_t ہدف_استعداد, length;
    int32_t *offsets;

    int32_t ماخذ_اشاریہ;

    uint8_t c;

    /* set up the local pointers */
    ماخذ=(ثابت uint8_t *)p_دلائل->ماخذ;
    ماخذ_حد_بندی=(ثابت uint8_t *)p_دلائل->ماخذ_حد_بندی;
    ہدف=old_ہدف=p_دلائل->ہدف;
    ہدف_استعداد=(int32_t)(p_دلائل->ہدف_حد_بندی-p_دلائل->ہدف);
    offsets=p_دلائل->offsets;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ماخذ_حد_بندی-ماخذ);
    اگر(length<ہدف_استعداد) {
        ہدف_استعداد=length;
    }

    اگر(ہدف_استعداد>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=ہدف_استعداد>>3;
        کرو {
            oredChars=ہدف[0]=ماخذ[0];
            oredChars|=ہدف[1]=ماخذ[1];
            oredChars|=ہدف[2]=ماخذ[2];
            oredChars|=ہدف[3]=ماخذ[3];
            oredChars|=ہدف[4]=ماخذ[4];
            oredChars|=ہدف[5]=ماخذ[5];
            oredChars|=ہدف[6]=ماخذ[6];
            oredChars|=ہدف[7]=ماخذ[7];

            /* were all 16 entries really valid? */
            اگر(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                توڑو;
            }
            ماخذ+=8;
            ہدف+=8;
        } جب(--count>0);
        count=loops-count;
        ہدف_استعداد-=count*8;

        اگر(offsets!=خالی_اشاریہ) {
            old_ہدف+=count*8;
            جب(count>0) {
                offsets[0]=ماخذ_اشاریہ++;
                offsets[1]=ماخذ_اشاریہ++;
                offsets[2]=ماخذ_اشاریہ++;
                offsets[3]=ماخذ_اشاریہ++;
                offsets[4]=ماخذ_اشاریہ++;
                offsets[5]=ماخذ_اشاریہ++;
                offsets[6]=ماخذ_اشاریہ++;
                offsets[7]=ماخذ_اشاریہ++;
                offsets+=8;
                --count;
            }
        }
    }

    /* conversion loop */
    c=0;
    جب(ہدف_استعداد>0 && (c=*ماخذ++)<=0x7f) {
        *ہدف++=c;
        --ہدف_استعداد;
    }

    اگر(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=p_دلائل->converter;
        cnv->مقصد_UBytes[0]=c;
        cnv->مقصد_ULength=1;
        *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
    } ورنہ اگر(ماخذ<ماخذ_حد_بندی && ہدف>=p_دلائل->ہدف_حد_بندی) {
        /* target is full */
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
    }

    /* set offsets since the start */
    اگر(offsets!=خالی_اشاریہ) {
        size_t count=ہدف-old_ہدف;
        جب(count>0) {
            *offsets++=ماخذ_اشاریہ++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_دلائل->ماخذ=(ثابت حرف *)ماخذ;
    p_دلائل->ہدف=ہدف;
    p_دلائل->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
ساکن UChar32 U_CALLCONV
ASCIIGet_اگلا_UChar(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                   UError_رمز *p_Error_رمز) {
    ثابت uint8_t *ماخذ;
    uint8_t b;

    ماخذ=(ثابت uint8_t *)p_دلائل->ماخذ;
    اگر(ماخذ<(ثابت uint8_t *)p_دلائل->ماخذ_حد_بندی) {
        b=*ماخذ++;
        p_دلائل->ماخذ=(ثابت حرف *)ماخذ;
        اگر(b<=0x7f) {
            واپس b;
        } ورنہ {
            UConverter *cnv=p_دلائل->converter;
            cnv->مقصد_UBytes[0]=b;
            cnv->مقصد_ULength=1;
            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
            واپس 0xffff;
        }
    }

    /* no output because of empty input */
    *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
    واپس 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
ساکن خالی U_CALLCONV
ucnv_ASCIIFromUTF8(UConverter_From_Unicode_دلائل *pFromUArgs,
                   UConverter_مقصد_Unicode_دلائل *p_مقصد_UArgs,
                   UError_رمز *p_Error_رمز) {
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف;
    int32_t ہدف_استعداد, length;

    uint8_t c;

    اگر(p_مقصد_UArgs->converter->مقصد_ULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *p_Error_رمز=U_USING_DEFAULT_WARNING;
        واپس;
    }

    /* set up the local pointers */
    ماخذ=(ثابت uint8_t *)p_مقصد_UArgs->ماخذ;
    ماخذ_حد_بندی=(ثابت uint8_t *)p_مقصد_UArgs->ماخذ_حد_بندی;
    ہدف=(uint8_t *)pFromUArgs->ہدف;
    ہدف_استعداد=(int32_t)(pFromUArgs->ہدف_حد_بندی-pFromUArgs->ہدف);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ماخذ_حد_بندی-ماخذ);
    اگر(length<ہدف_استعداد) {
        ہدف_استعداد=length;
    }

    /* unroll the loop with the most common case */
    اگر(ہدف_استعداد>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=ہدف_استعداد>>4;
        کرو {
            oredChars=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;
            oredChars|=*ہدف++=*ماخذ++;

            /* were all 16 entries really valid? */
            اگر(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                ماخذ-=16;
                ہدف-=16;
                توڑو;
            }
        } جب(--count>0);
        count=loops-count;
        ہدف_استعداد-=16*count;
    }

    /* conversion loop */
    c=0;
    جب(ہدف_استعداد>0 && (c=*ماخذ)<=0x7f) {
        ++ماخذ;
        *ہدف++=c;
        --ہدف_استعداد;
    }

    اگر(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *p_Error_رمز=U_USING_DEFAULT_WARNING;
    } ورنہ اگر(ماخذ<ماخذ_حد_بندی && ہدف>=(ثابت uint8_t *)pFromUArgs->ہدف_حد_بندی) {
        /* target is full */
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
    }

    /* write back the updated pointers */
    p_مقصد_UArgs->ماخذ=(ثابت حرف *)ماخذ;
    pFromUArgs->ہدف=(حرف *)ہدف;
}

ساکن خالی U_CALLCONV
ASCIIGet_Unicode_سیٹ(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverter_Unicode_سیٹ which,
                    UError_رمز *p_Error_رمز) {
    (خالی)cnv;
    (خالی)which;
    (خالی)p_Error_رمز;
    sa->جوڑو_Range(sa->سیٹ, 0, 0x7f);
}
U_CDECL_انتہا

ساکن ثابت UConverterImpl _ASCIIImpl={
    UCNV_US_ASCII,

    خالی_اشاریہ,
    خالی_اشاریہ,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,

    _ASCIIToUnicodeWithOffsets,
    _ASCIIToUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    ASCIIGet_اگلا_UChar,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    ASCIIGet_Unicode_سیٹ,

    خالی_اشاریہ,
    ucnv_ASCIIFromUTF8
};

ساکن ثابت UConverter_Static_ڈیٹا ASCIIStatic_ڈیٹا={
    حجم(UConverter_Static_ڈیٹا),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, جھوٹ, جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_ڈیٹا _ASCIIData=
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&ASCIIStatic_ڈیٹا, &_ASCIIImpl);

#اگر_ختم
