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
#تعریف_کن LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
Latin1_مقصد_Unicode_With_Offsets(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                            UError_کد *p_Error_کد) {
    ثابت uint8_t *منبع;
    char16_t *هدف;
    int32_t هدف_ظرفیت, length;
    int32_t *offsets;

    int32_t منبع_شاخص;

    /* set up the local pointers */
    منبع=(ثابت uint8_t *)p_آرگومانها->منبع;
    هدف=p_آرگومانها->هدف;
    هدف_ظرفیت=(int32_t)(p_آرگومانها->هدف_حد-p_آرگومانها->هدف);
    offsets=p_آرگومانها->offsets;

    منبع_شاخص=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((ثابت uint8_t *)p_آرگومانها->منبع_حد-منبع);
    إذا(length<=هدف_ظرفیت) {
        هدف_ظرفیت=length;
    } وإلا {
        /* target will be full */
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
        length=هدف_ظرفیت;
    }

    إذا(هدف_ظرفیت>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=هدف_ظرفیت>>3;
        length=هدف_ظرفیت&=0x7;
        افعل {
            هدف[0]=منبع[0];
            هدف[1]=منبع[1];
            هدف[2]=منبع[2];
            هدف[3]=منبع[3];
            هدف[4]=منبع[4];
            هدف[5]=منبع[5];
            هدف[6]=منبع[6];
            هدف[7]=منبع[7];
            هدف+=8;
            منبع+=8;
        } طالما(--count>0);

        إذا(offsets!=اشاره_تهی) {
            افعل {
                offsets[0]=منبع_شاخص++;
                offsets[1]=منبع_شاخص++;
                offsets[2]=منبع_شاخص++;
                offsets[3]=منبع_شاخص++;
                offsets[4]=منبع_شاخص++;
                offsets[5]=منبع_شاخص++;
                offsets[6]=منبع_شاخص++;
                offsets[7]=منبع_شاخص++;
                offsets+=8;
            } طالما(--loops>0);
        }
    }

    /* conversion loop */
    طالما(هدف_ظرفیت>0) {
        *هدف++=*منبع++;
        --هدف_ظرفیت;
    }

    /* write back the updated pointers */
    p_آرگومانها->منبع=(ثابت حرف *)منبع;
    p_آرگومانها->هدف=هدف;

    /* set offsets */
    إذا(offsets!=اشاره_تهی) {
        طالما(length>0) {
            *offsets++=منبع_شاخص++;
            --length;
        }
        p_آرگومانها->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
ساكن UChar32 U_CALLCONV
Latin1_Get_بعدی_UChar(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                    UError_کد *p_Error_کد) {
    ثابت uint8_t *منبع=(ثابت uint8_t *)p_آرگومانها->منبع;
    إذا(منبع<(ثابت uint8_t *)p_آرگومانها->منبع_حد) {
        p_آرگومانها->منبع=(ثابت حرف *)(منبع+1);
        أرجع *منبع;
    }

    /* no output because of empty input */
    *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
    أرجع 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
ساكن فراغ U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverter_From_Unicode_آرگومانها *p_آرگومانها,
                              UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت char16_t *منبع, *منبع_حد;
    uint8_t *هدف, *old_هدف;
    int32_t هدف_ظرفیت, length;
    int32_t *offsets;

    UChar32 cp;
    char16_t c, max;

    int32_t منبع_شاخص;

    /* set up the local pointers */
    cnv=p_آرگومانها->converter;
    منبع=p_آرگومانها->منبع;
    منبع_حد=p_آرگومانها->منبع_حد;
    هدف=old_هدف=(uint8_t *)p_آرگومانها->هدف;
    هدف_ظرفیت=(int32_t)(p_آرگومانها->هدف_حد-p_آرگومانها->هدف);
    offsets=p_آرگومانها->offsets;

    إذا(cnv->shared_داده==&Latin1_داده) {
        max=0xff; /* Latin-1 */
    } وإلا {
        max=0x7f; /* US-ASCII */
    }

    /* get the converter state from UConverter */
    cp=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    منبع_شاخص= cp==0 ? 0 : -1;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(منبع_حد-منبع);
    إذا(length<هدف_ظرفیت) {
        هدف_ظرفیت=length;
    }

    /* conversion loop */
    إذا(cp!=0 && هدف_ظرفیت>0) {
        اذهب getTrail;
    }

#إذا LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    إذا(هدف_ظرفیت>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=هدف_ظرفیت>>4;
        افعل {
            oredChars=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;
            oredChars|=u=*منبع++;
            *هدف++=(uint8_t)u;

            /* were all 16 entries really valid? */
            إذا(oredChars>max) {
                /* no, return to the first of these 16 */
                منبع-=16;
                هدف-=16;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        هدف_ظرفیت-=16*count;

        إذا(offsets!=اشاره_تهی) {
            old_هدف+=16*count;
            طالما(count>0) {
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                *offsets++=منبع_شاخص++;
                --count;
            }
        }
    }
#پایان_اگر

    /* conversion loop */
    c=0;
    طالما(هدف_ظرفیت>0 && (c=*منبع++)<=max) {
        /* convert the Unicode code point */
        *هدف++=(uint8_t)c;
        --هدف_ظرفیت;
    }

    إذا(c>max) {
        cp=c;
        إذا(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } وإلا إذا(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            إذا(منبع<منبع_حد) {
                /* test the following code unit */
                char16_t trail=*منبع;
                إذا(U16_IS_TRAIL(trail)) {
                    ++منبع;
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
                اذهب no_بیشتر_درون_داد;
            }
        } وإلا {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *p_Error_کد= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_یافت : U_INVALID_CHAR_یافت;
        cnv->fromUChar32=cp;
    }
no_بیشتر_درون_داد:

    /* set offsets since the start */
    إذا(offsets!=اشاره_تهی) {
        size_t count=هدف-old_هدف;
        طالما(count>0) {
            *offsets++=منبع_شاخص++;
            --count;
        }
    }

    إذا(U_SUCCESS(*p_Error_کد) && منبع<منبع_حد && هدف>=(uint8_t *)p_آرگومانها->هدف_حد) {
        /* target is full */
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
    }

    /* write back the updated pointers */
    p_آرگومانها->منبع=منبع;
    p_آرگومانها->هدف=(حرف *)هدف;
    p_آرگومانها->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
ساكن فراغ U_CALLCONV
ucnv_Latin1FromUTF8(UConverter_From_Unicode_آرگومانها *pFromUArgs,
                    UConverter_مقصد_Unicode_آرگومانها *p_مقصد_UArgs,
                    UError_کد *p_Error_کد) {
    UConverter *utf8;
    ثابت uint8_t *منبع, *منبع_حد;
    uint8_t *هدف;
    int32_t هدف_ظرفیت;

    UChar32 c;
    uint8_t b, t1;

    /* set up the local pointers */
    utf8=p_مقصد_UArgs->converter;
    منبع=(uint8_t *)p_مقصد_UArgs->منبع;
    منبع_حد=(uint8_t *)p_مقصد_UArgs->منبع_حد;
    هدف=(uint8_t *)pFromUArgs->هدف;
    هدف_ظرفیت=(int32_t)(pFromUArgs->هدف_حد-pFromUArgs->هدف);

    /* get the converter state from the UTF-8 UConverter */
    إذا (utf8->مقصد_ULength > 0) {
        c=(UChar32)utf8->مقصد_Unicode_وضعیت;
    } وإلا {
        c = 0;
    }
    إذا(c!=0 && منبع<منبع_حد) {
        إذا(هدف_ظرفیت==0) {
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
            أرجع;
        } وإلا إذا(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*منبع-0x80)) <= 0x3f) {
            ++منبع;
            *هدف++=(uint8_t)(((c&3)<<6)|t1);
            --هدف_ظرفیت;

            utf8->مقصد_Unicode_وضعیت=0;
            utf8->مقصد_ULength=0;
        } وإلا {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *p_Error_کد=U_USING_DEFAULT_WARNING;
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
    إذا(منبع<منبع_حد && U8_IS_LEAD(*(منبع_حد-1))) {
        --منبع_حد;
    }

    /* conversion loop */
    طالما(منبع<منبع_حد) {
        إذا(هدف_ظرفیت>0) {
            b=*منبع++;
            إذا(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *هدف++ = b;
                --هدف_ظرفیت;
            } وإلا إذا( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*منبع-0x80)) <= 0x3f
            ) {
                ++منبع;
                *هدف++=(uint8_t)(((b&3)<<6)|t1);
                --هدف_ظرفیت;
            } وإلا {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                p_مقصد_UArgs->منبع=(حرف *)(منبع-1);
                pFromUArgs->هدف=(حرف *)هدف;
                *p_Error_کد=U_USING_DEFAULT_WARNING;
                أرجع;
            }
        } وإلا {
            /* target is full */
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
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
    إذا(U_SUCCESS(*p_Error_کد) && منبع<(منبع_حد=(uint8_t *)p_مقصد_UArgs->منبع_حد)) {
        utf8->مقصد_Unicode_وضعیت=utf8->مقصد_UBytes[0]=b=*منبع++;
        utf8->مقصد_ULength=1;
        utf8->نما=U8_شمار_بایتها(b);
    }

    /* write back the updated pointers */
    p_مقصد_UArgs->منبع=(حرف *)منبع;
    pFromUArgs->هدف=(حرف *)هدف;
}

ساكن فراغ U_CALLCONV
Latin1_Get_Unicode_مجموعه(ثابت UConverter *cnv,
                     ثابت USetAdder *sa,
                     UConverter_Unicode_مجموعه which,
                     UError_کد *p_Error_کد) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_کد;
    sa->جمع_کن_Range(sa->مجموعه, 0, 0xff);
}
U_CDECL_انتها


ساكن ثابت UConverterImpl _Latin1Impl={
    UCNV_LATIN_1,

    اشاره_تهی,
    اشاره_تهی,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,

    Latin1_مقصد_Unicode_With_Offsets,
    Latin1_مقصد_Unicode_With_Offsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    Latin1_Get_بعدی_UChar,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    Latin1_Get_Unicode_مجموعه,

    اشاره_تهی,
    ucnv_Latin1FromUTF8
};

ساكن ثابت UConverter_Static_داده Latin1_Static_داده={
    حجم(UConverter_Static_داده),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_داده Latin1_داده=
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&Latin1_Static_داده, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
ساكن فراغ U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                           UError_کد *p_Error_کد) {
    ثابت uint8_t *منبع, *منبع_حد;
    char16_t *هدف, *old_هدف;
    int32_t هدف_ظرفیت, length;
    int32_t *offsets;

    int32_t منبع_شاخص;

    uint8_t c;

    /* set up the local pointers */
    منبع=(ثابت uint8_t *)p_آرگومانها->منبع;
    منبع_حد=(ثابت uint8_t *)p_آرگومانها->منبع_حد;
    هدف=old_هدف=p_آرگومانها->هدف;
    هدف_ظرفیت=(int32_t)(p_آرگومانها->هدف_حد-p_آرگومانها->هدف);
    offsets=p_آرگومانها->offsets;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    منبع_شاخص=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(منبع_حد-منبع);
    إذا(length<هدف_ظرفیت) {
        هدف_ظرفیت=length;
    }

    إذا(هدف_ظرفیت>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=هدف_ظرفیت>>3;
        افعل {
            oredChars=هدف[0]=منبع[0];
            oredChars|=هدف[1]=منبع[1];
            oredChars|=هدف[2]=منبع[2];
            oredChars|=هدف[3]=منبع[3];
            oredChars|=هدف[4]=منبع[4];
            oredChars|=هدف[5]=منبع[5];
            oredChars|=هدف[6]=منبع[6];
            oredChars|=هدف[7]=منبع[7];

            /* were all 16 entries really valid? */
            إذا(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                اكسر;
            }
            منبع+=8;
            هدف+=8;
        } طالما(--count>0);
        count=loops-count;
        هدف_ظرفیت-=count*8;

        إذا(offsets!=اشاره_تهی) {
            old_هدف+=count*8;
            طالما(count>0) {
                offsets[0]=منبع_شاخص++;
                offsets[1]=منبع_شاخص++;
                offsets[2]=منبع_شاخص++;
                offsets[3]=منبع_شاخص++;
                offsets[4]=منبع_شاخص++;
                offsets[5]=منبع_شاخص++;
                offsets[6]=منبع_شاخص++;
                offsets[7]=منبع_شاخص++;
                offsets+=8;
                --count;
            }
        }
    }

    /* conversion loop */
    c=0;
    طالما(هدف_ظرفیت>0 && (c=*منبع++)<=0x7f) {
        *هدف++=c;
        --هدف_ظرفیت;
    }

    إذا(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=p_آرگومانها->converter;
        cnv->مقصد_UBytes[0]=c;
        cnv->مقصد_ULength=1;
        *p_Error_کد=U_ILLEGAL_CHAR_یافت;
    } وإلا إذا(منبع<منبع_حد && هدف>=p_آرگومانها->هدف_حد) {
        /* target is full */
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
    }

    /* set offsets since the start */
    إذا(offsets!=اشاره_تهی) {
        size_t count=هدف-old_هدف;
        طالما(count>0) {
            *offsets++=منبع_شاخص++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_آرگومانها->منبع=(ثابت حرف *)منبع;
    p_آرگومانها->هدف=هدف;
    p_آرگومانها->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
ساكن UChar32 U_CALLCONV
ASCIIGet_بعدی_UChar(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                   UError_کد *p_Error_کد) {
    ثابت uint8_t *منبع;
    uint8_t b;

    منبع=(ثابت uint8_t *)p_آرگومانها->منبع;
    إذا(منبع<(ثابت uint8_t *)p_آرگومانها->منبع_حد) {
        b=*منبع++;
        p_آرگومانها->منبع=(ثابت حرف *)منبع;
        إذا(b<=0x7f) {
            أرجع b;
        } وإلا {
            UConverter *cnv=p_آرگومانها->converter;
            cnv->مقصد_UBytes[0]=b;
            cnv->مقصد_ULength=1;
            *p_Error_کد=U_ILLEGAL_CHAR_یافت;
            أرجع 0xffff;
        }
    }

    /* no output because of empty input */
    *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
    أرجع 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
ساكن فراغ U_CALLCONV
ucnv_ASCIIFromUTF8(UConverter_From_Unicode_آرگومانها *pFromUArgs,
                   UConverter_مقصد_Unicode_آرگومانها *p_مقصد_UArgs,
                   UError_کد *p_Error_کد) {
    ثابت uint8_t *منبع, *منبع_حد;
    uint8_t *هدف;
    int32_t هدف_ظرفیت, length;

    uint8_t c;

    إذا(p_مقصد_UArgs->converter->مقصد_ULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *p_Error_کد=U_USING_DEFAULT_WARNING;
        أرجع;
    }

    /* set up the local pointers */
    منبع=(ثابت uint8_t *)p_مقصد_UArgs->منبع;
    منبع_حد=(ثابت uint8_t *)p_مقصد_UArgs->منبع_حد;
    هدف=(uint8_t *)pFromUArgs->هدف;
    هدف_ظرفیت=(int32_t)(pFromUArgs->هدف_حد-pFromUArgs->هدف);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(منبع_حد-منبع);
    إذا(length<هدف_ظرفیت) {
        هدف_ظرفیت=length;
    }

    /* unroll the loop with the most common case */
    إذا(هدف_ظرفیت>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=هدف_ظرفیت>>4;
        افعل {
            oredChars=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;
            oredChars|=*هدف++=*منبع++;

            /* were all 16 entries really valid? */
            إذا(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                منبع-=16;
                هدف-=16;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        هدف_ظرفیت-=16*count;
    }

    /* conversion loop */
    c=0;
    طالما(هدف_ظرفیت>0 && (c=*منبع)<=0x7f) {
        ++منبع;
        *هدف++=c;
        --هدف_ظرفیت;
    }

    إذا(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *p_Error_کد=U_USING_DEFAULT_WARNING;
    } وإلا إذا(منبع<منبع_حد && هدف>=(ثابت uint8_t *)pFromUArgs->هدف_حد) {
        /* target is full */
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
    }

    /* write back the updated pointers */
    p_مقصد_UArgs->منبع=(ثابت حرف *)منبع;
    pFromUArgs->هدف=(حرف *)هدف;
}

ساكن فراغ U_CALLCONV
ASCIIGet_Unicode_مجموعه(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverter_Unicode_مجموعه which,
                    UError_کد *p_Error_کد) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_کد;
    sa->جمع_کن_Range(sa->مجموعه, 0, 0x7f);
}
U_CDECL_انتها

ساكن ثابت UConverterImpl _ASCIIImpl={
    UCNV_US_ASCII,

    اشاره_تهی,
    اشاره_تهی,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,

    _ASCIIToUnicodeWithOffsets,
    _ASCIIToUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    ASCIIGet_بعدی_UChar,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    ASCIIGet_Unicode_مجموعه,

    اشاره_تهی,
    ucnv_ASCIIFromUTF8
};

ساكن ثابت UConverter_Static_داده ASCIIStatic_داده={
    حجم(UConverter_Static_داده),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_داده _ASCIIData=
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&ASCIIStatic_داده, &_ASCIIImpl);

#پایان_اگر
