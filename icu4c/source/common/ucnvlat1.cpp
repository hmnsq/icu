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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "unicode/uset.h"
#ضمن "unicode/utf8.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"
#ضمن "ustr_imp.h"

/* control optimizations according to the platform */
#وصف_ڪريو LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
Latin1_منزل_Unicode_With_Offsets(UConverter_منزل_Unicode_دليلون *p_دليلون,
                            UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت uint8_t *ذريعو;
    char16_t *هدف;
    int32_t هدف_صلاحيت, length;
    int32_t *offsets;

    int32_t ذريعو_اشاريو;

    /* set up the local pointers */
    ذريعو=(ثابت uint8_t *)p_دليلون->ذريعو;
    هدف=p_دليلون->هدف;
    هدف_صلاحيت=(int32_t)(p_دليلون->هدف_حد_بندي-p_دليلون->هدف);
    offsets=p_دليلون->offsets;

    ذريعو_اشاريو=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((ثابت uint8_t *)p_دليلون->ذريعو_حد_بندي-ذريعو);
    إذا(length<=هدف_صلاحيت) {
        هدف_صلاحيت=length;
    } وإلا {
        /* target will be full */
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
        length=هدف_صلاحيت;
    }

    إذا(هدف_صلاحيت>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=هدف_صلاحيت>>3;
        length=هدف_صلاحيت&=0x7;
        افعل {
            هدف[0]=ذريعو[0];
            هدف[1]=ذريعو[1];
            هدف[2]=ذريعو[2];
            هدف[3]=ذريعو[3];
            هدف[4]=ذريعو[4];
            هدف[5]=ذريعو[5];
            هدف[6]=ذريعو[6];
            هدف[7]=ذريعو[7];
            هدف+=8;
            ذريعو+=8;
        } طالما(--count>0);

        إذا(offsets!=خالي_اشارو) {
            افعل {
                offsets[0]=ذريعو_اشاريو++;
                offsets[1]=ذريعو_اشاريو++;
                offsets[2]=ذريعو_اشاريو++;
                offsets[3]=ذريعو_اشاريو++;
                offsets[4]=ذريعو_اشاريو++;
                offsets[5]=ذريعو_اشاريو++;
                offsets[6]=ذريعو_اشاريو++;
                offsets[7]=ذريعو_اشاريو++;
                offsets+=8;
            } طالما(--loops>0);
        }
    }

    /* conversion loop */
    طالما(هدف_صلاحيت>0) {
        *هدف++=*ذريعو++;
        --هدف_صلاحيت;
    }

    /* write back the updated pointers */
    p_دليلون->ذريعو=(ثابت حرف *)ذريعو;
    p_دليلون->هدف=هدف;

    /* set offsets */
    إذا(offsets!=خالي_اشارو) {
        طالما(length>0) {
            *offsets++=ذريعو_اشاريو++;
            --length;
        }
        p_دليلون->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
ساكن UChar32 U_CALLCONV
Latin1_Get_ايندڙ_UChar(UConverter_منزل_Unicode_دليلون *p_دليلون,
                    UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت uint8_t *ذريعو=(ثابت uint8_t *)p_دليلون->ذريعو;
    إذا(ذريعو<(ثابت uint8_t *)p_دليلون->ذريعو_حد_بندي) {
        p_دليلون->ذريعو=(ثابت حرف *)(ذريعو+1);
        أرجع *ذريعو;
    }

    /* no output because of empty input */
    *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
    أرجع 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
ساكن فراغ U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverter_From_Unicode_دليلون *p_دليلون,
                              UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت char16_t *ذريعو, *ذريعو_حد_بندي;
    uint8_t *هدف, *old_هدف;
    int32_t هدف_صلاحيت, length;
    int32_t *offsets;

    UChar32 cp;
    char16_t c, max;

    int32_t ذريعو_اشاريو;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    ذريعو=p_دليلون->ذريعو;
    ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
    هدف=old_هدف=(uint8_t *)p_دليلون->هدف;
    هدف_صلاحيت=(int32_t)(p_دليلون->هدف_حد_بندي-p_دليلون->هدف);
    offsets=p_دليلون->offsets;

    إذا(cnv->shared_معلومات==&Latin1_معلومات) {
        max=0xff; /* Latin-1 */
    } وإلا {
        max=0x7f; /* US-ASCII */
    }

    /* get the converter state from UConverter */
    cp=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ذريعو_اشاريو= cp==0 ? 0 : -1;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ذريعو_حد_بندي-ذريعو);
    إذا(length<هدف_صلاحيت) {
        هدف_صلاحيت=length;
    }

    /* conversion loop */
    إذا(cp!=0 && هدف_صلاحيت>0) {
        اذهب getTrail;
    }

#إذا LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    إذا(هدف_صلاحيت>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=هدف_صلاحيت>>4;
        افعل {
            oredChars=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*ذريعو++;
            *هدف++=(uint8_t)u;

            /* were all 16 entries really valid? */
            إذا(oredChars>max) {
                /* no, return to the first of these 16 */
                ذريعو-=16;
                هدف-=16;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        هدف_صلاحيت-=16*count;

        إذا(offsets!=خالي_اشارو) {
            old_هدف+=16*count;
            طالما(count>0) {
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                *offsets++=ذريعو_اشاريو++;
                --count;
            }
        }
    }
#جيڪڏهن_پورو

    /* conversion loop */
    c=0;
    طالما(هدف_صلاحيت>0 && (c=*ذريعو++)<=max) {
        /* convert the Unicode code point */
        *هدف++=(uint8_t)c;
        --هدف_صلاحيت;
    }

    إذا(c>max) {
        cp=c;
        إذا(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } وإلا إذا(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            إذا(ذريعو<ذريعو_حد_بندي) {
                /* test the following code unit */
                char16_t trail=*ذريعو;
                إذا(U16_IS_TRAIL(trail)) {
                    ++ذريعو;
                    cp=U16_GET_SUPPLEMENTARY(cp, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } وإلا {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                }
            } وإلا {
                /* no more input */
                cnv->fromUChar32=cp;
                اذهب no_وڌيڪ_داخلا;
            }
        } وإلا {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *p_Error_ڪوڊ= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_مليو : U_INVALID_CHAR_مليو;
        cnv->fromUChar32=cp;
    }
no_وڌيڪ_داخلا:

    /* set offsets since the start */
    إذا(offsets!=خالي_اشارو) {
        size_t count=هدف-old_هدف;
        طالما(count>0) {
            *offsets++=ذريعو_اشاريو++;
            --count;
        }
    }

    إذا(U_SUCCESS(*p_Error_ڪوڊ) && ذريعو<ذريعو_حد_بندي && هدف>=(uint8_t *)p_دليلون->هدف_حد_بندي) {
        /* target is full */
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
    }

    /* write back the updated pointers */
    p_دليلون->ذريعو=ذريعو;
    p_دليلون->هدف=(حرف *)هدف;
    p_دليلون->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
ساكن فراغ U_CALLCONV
ucnv_Latin1FromUTF8(UConverter_From_Unicode_دليلون *pFromUArgs,
                    UConverter_منزل_Unicode_دليلون *p_منزل_UArgs,
                    UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *utf8;
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي;
    uint8_t *هدف;
    int32_t هدف_صلاحيت;

    UChar32 c;
    uint8_t b, t1;

    /* set up the local pointers */
    utf8=p_منزل_UArgs->converter;
    ذريعو=(uint8_t *)p_منزل_UArgs->ذريعو;
    ذريعو_حد_بندي=(uint8_t *)p_منزل_UArgs->ذريعو_حد_بندي;
    هدف=(uint8_t *)pFromUArgs->هدف;
    هدف_صلاحيت=(int32_t)(pFromUArgs->هدف_حد_بندي-pFromUArgs->هدف);

    /* get the converter state from the UTF-8 UConverter */
    إذا (utf8->منزل_ULength > 0) {
        c=(UChar32)utf8->منزل_Unicode_حالت;
    } وإلا {
        c = 0;
    }
    إذا(c!=0 && ذريعو<ذريعو_حد_بندي) {
        إذا(هدف_صلاحيت==0) {
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
            أرجع;
        } وإلا إذا(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*ذريعو-0x80)) <= 0x3f) {
            ++ذريعو;
            *هدف++=(uint8_t)(((c&3)<<6)|t1);
            --هدف_صلاحيت;

            utf8->منزل_Unicode_حالت=0;
            utf8->منزل_ULength=0;
        } وإلا {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *p_Error_ڪوڊ=U_USING_DEFAULT_WARNING;
            أرجع;
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
    إذا(ذريعو<ذريعو_حد_بندي && U8_IS_LEAD(*(ذريعو_حد_بندي-1))) {
        --ذريعو_حد_بندي;
    }

    /* conversion loop */
    طالما(ذريعو<ذريعو_حد_بندي) {
        إذا(هدف_صلاحيت>0) {
            b=*ذريعو++;
            إذا(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *هدف++ = b;
                --هدف_صلاحيت;
            } وإلا إذا( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*ذريعو-0x80)) <= 0x3f
            ) {
                ++ذريعو;
                *هدف++=(uint8_t)(((b&3)<<6)|t1);
                --هدف_صلاحيت;
            } وإلا {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                p_منزل_UArgs->ذريعو=(حرف *)(ذريعو-1);
                pFromUArgs->هدف=(حرف *)هدف;
                *p_Error_ڪوڊ=U_USING_DEFAULT_WARNING;
                أرجع;
            }
        } وإلا {
            /* target is full */
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     * For Latin-1, there is at most exactly one lead byte because of the
     * smaller sourceLimit adjustment logic.
     */
    إذا(U_SUCCESS(*p_Error_ڪوڊ) && ذريعو<(ذريعو_حد_بندي=(uint8_t *)p_منزل_UArgs->ذريعو_حد_بندي)) {
        utf8->منزل_Unicode_حالت=utf8->منزل_UBytes[0]=b=*ذريعو++;
        utf8->منزل_ULength=1;
        utf8->گهڻائي=U8_ڳڻپ_بائيٽون(b);
    }

    /* write back the updated pointers */
    p_منزل_UArgs->ذريعو=(حرف *)ذريعو;
    pFromUArgs->هدف=(حرف *)هدف;
}

ساكن فراغ U_CALLCONV
Latin1_Get_Unicode_سيٽ(ثابت UConverter *cnv,
                     ثابت USetAdder *sa,
                     UConverter_Unicode_سيٽ which,
                     UError_ڪوڊ *p_Error_ڪوڊ) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_ڪوڊ;
    sa->جوڙيو_Range(sa->سيٽ, 0, 0xff);
}
U_CDECL_پڇاڙي


ساكن ثابت UConverterImpl _Latin1Impl={
    UCNV_LATIN_1,

    خالي_اشارو,
    خالي_اشارو,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,

    Latin1_منزل_Unicode_With_Offsets,
    Latin1_منزل_Unicode_With_Offsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    Latin1_Get_ايندڙ_UChar,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    Latin1_Get_Unicode_سيٽ,

    خالي_اشارو,
    ucnv_Latin1FromUTF8
};

ساكن ثابت UConverter_Static_معلومات Latin1_Static_معلومات={
    حجم(UConverter_Static_معلومات),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_معلومات Latin1_معلومات=
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&Latin1_Static_معلومات, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
ساكن فراغ U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverter_منزل_Unicode_دليلون *p_دليلون,
                           UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي;
    char16_t *هدف, *old_هدف;
    int32_t هدف_صلاحيت, length;
    int32_t *offsets;

    int32_t ذريعو_اشاريو;

    uint8_t c;

    /* set up the local pointers */
    ذريعو=(ثابت uint8_t *)p_دليلون->ذريعو;
    ذريعو_حد_بندي=(ثابت uint8_t *)p_دليلون->ذريعو_حد_بندي;
    هدف=old_هدف=p_دليلون->هدف;
    هدف_صلاحيت=(int32_t)(p_دليلون->هدف_حد_بندي-p_دليلون->هدف);
    offsets=p_دليلون->offsets;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ذريعو_اشاريو=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ذريعو_حد_بندي-ذريعو);
    إذا(length<هدف_صلاحيت) {
        هدف_صلاحيت=length;
    }

    إذا(هدف_صلاحيت>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=هدف_صلاحيت>>3;
        افعل {
            oredChars=هدف[0]=ذريعو[0];
            oredChars|=هدف[1]=ذريعو[1];
            oredChars|=هدف[2]=ذريعو[2];
            oredChars|=هدف[3]=ذريعو[3];
            oredChars|=هدف[4]=ذريعو[4];
            oredChars|=هدف[5]=ذريعو[5];
            oredChars|=هدف[6]=ذريعو[6];
            oredChars|=هدف[7]=ذريعو[7];

            /* were all 16 entries really valid? */
            إذا(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                اكسر;
            }
            ذريعو+=8;
            هدف+=8;
        } طالما(--count>0);
        count=loops-count;
        هدف_صلاحيت-=count*8;

        إذا(offsets!=خالي_اشارو) {
            old_هدف+=count*8;
            طالما(count>0) {
                offsets[0]=ذريعو_اشاريو++;
                offsets[1]=ذريعو_اشاريو++;
                offsets[2]=ذريعو_اشاريو++;
                offsets[3]=ذريعو_اشاريو++;
                offsets[4]=ذريعو_اشاريو++;
                offsets[5]=ذريعو_اشاريو++;
                offsets[6]=ذريعو_اشاريو++;
                offsets[7]=ذريعو_اشاريو++;
                offsets+=8;
                --count;
            }
        }
    }

    /* conversion loop */
    c=0;
    طالما(هدف_صلاحيت>0 && (c=*ذريعو++)<=0x7f) {
        *هدف++=c;
        --هدف_صلاحيت;
    }

    إذا(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=p_دليلون->converter;
        cnv->منزل_UBytes[0]=c;
        cnv->منزل_ULength=1;
        *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
    } وإلا إذا(ذريعو<ذريعو_حد_بندي && هدف>=p_دليلون->هدف_حد_بندي) {
        /* target is full */
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
    }

    /* set offsets since the start */
    إذا(offsets!=خالي_اشارو) {
        size_t count=هدف-old_هدف;
        طالما(count>0) {
            *offsets++=ذريعو_اشاريو++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_دليلون->ذريعو=(ثابت حرف *)ذريعو;
    p_دليلون->هدف=هدف;
    p_دليلون->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
ساكن UChar32 U_CALLCONV
ASCIIGet_ايندڙ_UChar(UConverter_منزل_Unicode_دليلون *p_دليلون,
                   UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت uint8_t *ذريعو;
    uint8_t b;

    ذريعو=(ثابت uint8_t *)p_دليلون->ذريعو;
    إذا(ذريعو<(ثابت uint8_t *)p_دليلون->ذريعو_حد_بندي) {
        b=*ذريعو++;
        p_دليلون->ذريعو=(ثابت حرف *)ذريعو;
        إذا(b<=0x7f) {
            أرجع b;
        } وإلا {
            UConverter *cnv=p_دليلون->converter;
            cnv->منزل_UBytes[0]=b;
            cnv->منزل_ULength=1;
            *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
            أرجع 0xffff;
        }
    }

    /* no output because of empty input */
    *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
    أرجع 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
ساكن فراغ U_CALLCONV
ucnv_ASCIIFromUTF8(UConverter_From_Unicode_دليلون *pFromUArgs,
                   UConverter_منزل_Unicode_دليلون *p_منزل_UArgs,
                   UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي;
    uint8_t *هدف;
    int32_t هدف_صلاحيت, length;

    uint8_t c;

    إذا(p_منزل_UArgs->converter->منزل_ULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *p_Error_ڪوڊ=U_USING_DEFAULT_WARNING;
        أرجع;
    }

    /* set up the local pointers */
    ذريعو=(ثابت uint8_t *)p_منزل_UArgs->ذريعو;
    ذريعو_حد_بندي=(ثابت uint8_t *)p_منزل_UArgs->ذريعو_حد_بندي;
    هدف=(uint8_t *)pFromUArgs->هدف;
    هدف_صلاحيت=(int32_t)(pFromUArgs->هدف_حد_بندي-pFromUArgs->هدف);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(ذريعو_حد_بندي-ذريعو);
    إذا(length<هدف_صلاحيت) {
        هدف_صلاحيت=length;
    }

    /* unroll the loop with the most common case */
    إذا(هدف_صلاحيت>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=هدف_صلاحيت>>4;
        افعل {
            oredChars=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;
            oredChars|=*هدف++=*ذريعو++;

            /* were all 16 entries really valid? */
            إذا(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                ذريعو-=16;
                هدف-=16;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        هدف_صلاحيت-=16*count;
    }

    /* conversion loop */
    c=0;
    طالما(هدف_صلاحيت>0 && (c=*ذريعو)<=0x7f) {
        ++ذريعو;
        *هدف++=c;
        --هدف_صلاحيت;
    }

    إذا(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *p_Error_ڪوڊ=U_USING_DEFAULT_WARNING;
    } وإلا إذا(ذريعو<ذريعو_حد_بندي && هدف>=(ثابت uint8_t *)pFromUArgs->هدف_حد_بندي) {
        /* target is full */
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
    }

    /* write back the updated pointers */
    p_منزل_UArgs->ذريعو=(ثابت حرف *)ذريعو;
    pFromUArgs->هدف=(حرف *)هدف;
}

ساكن فراغ U_CALLCONV
ASCIIGet_Unicode_سيٽ(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverter_Unicode_سيٽ which,
                    UError_ڪوڊ *p_Error_ڪوڊ) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_ڪوڊ;
    sa->جوڙيو_Range(sa->سيٽ, 0, 0x7f);
}
U_CDECL_پڇاڙي

ساكن ثابت UConverterImpl _ASCIIImpl={
    UCNV_US_ASCII,

    خالي_اشارو,
    خالي_اشارو,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,

    _ASCIIToUnicodeWithOffsets,
    _ASCIIToUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    ASCIIGet_ايندڙ_UChar,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    ASCIIGet_Unicode_سيٽ,

    خالي_اشارو,
    ucnv_ASCIIFromUTF8
};

ساكن ثابت UConverter_Static_معلومات ASCIIStatic_معلومات={
    حجم(UConverter_Static_معلومات),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_معلومات _ASCIIData=
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&ASCIIStatic_معلومات, &_ASCIIImpl);

#جيڪڏهن_پورو
