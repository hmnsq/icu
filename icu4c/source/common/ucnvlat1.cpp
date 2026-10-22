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
#تعریف_کرو LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
Latin1_منزل_Unicode_With_Offsets(UConverter_منزل_Unicode_دلیلاں *p_دلیلاں,
                            UError_کوڈ *p_Error_کوڈ) {
    ثابت uint8_t *سروت;
    char16_t *نشانہ;
    int32_t نشانہ_سمرتھا, length;
    int32_t *offsets;

    int32_t سروت_اشاریہ;

    /* set up the local pointers */
    سروت=(ثابت uint8_t *)p_دلیلاں->سروت;
    نشانہ=p_دلیلاں->نشانہ;
    نشانہ_سمرتھا=(int32_t)(p_دلیلاں->نشانہ_حد_بندی-p_دلیلاں->نشانہ);
    offsets=p_دلیلاں->offsets;

    سروت_اشاریہ=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((ثابت uint8_t *)p_دلیلاں->سروت_حد_بندی-سروت);
    إذا(length<=نشانہ_سمرتھا) {
        نشانہ_سمرتھا=length;
    } وإلا {
        /* target will be full */
        *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
        length=نشانہ_سمرتھا;
    }

    إذا(نشانہ_سمرتھا>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=نشانہ_سمرتھا>>3;
        length=نشانہ_سمرتھا&=0x7;
        افعل {
            نشانہ[0]=سروت[0];
            نشانہ[1]=سروت[1];
            نشانہ[2]=سروت[2];
            نشانہ[3]=سروت[3];
            نشانہ[4]=سروت[4];
            نشانہ[5]=سروت[5];
            نشانہ[6]=سروت[6];
            نشانہ[7]=سروت[7];
            نشانہ+=8;
            سروت+=8;
        } طالما(--count>0);

        إذا(offsets!=خالی_سوچک) {
            افعل {
                offsets[0]=سروت_اشاریہ++;
                offsets[1]=سروت_اشاریہ++;
                offsets[2]=سروت_اشاریہ++;
                offsets[3]=سروت_اشاریہ++;
                offsets[4]=سروت_اشاریہ++;
                offsets[5]=سروت_اشاریہ++;
                offsets[6]=سروت_اشاریہ++;
                offsets[7]=سروت_اشاریہ++;
                offsets+=8;
            } طالما(--loops>0);
        }
    }

    /* conversion loop */
    طالما(نشانہ_سمرتھا>0) {
        *نشانہ++=*سروت++;
        --نشانہ_سمرتھا;
    }

    /* write back the updated pointers */
    p_دلیلاں->سروت=(ثابت حرف *)سروت;
    p_دلیلاں->نشانہ=نشانہ;

    /* set offsets */
    إذا(offsets!=خالی_سوچک) {
        طالما(length>0) {
            *offsets++=سروت_اشاریہ++;
            --length;
        }
        p_دلیلاں->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
ساكن UChar32 U_CALLCONV
Latin1_Get_اگلا_UChar(UConverter_منزل_Unicode_دلیلاں *p_دلیلاں,
                    UError_کوڈ *p_Error_کوڈ) {
    ثابت uint8_t *سروت=(ثابت uint8_t *)p_دلیلاں->سروت;
    إذا(سروت<(ثابت uint8_t *)p_دلیلاں->سروت_حد_بندی) {
        p_دلیلاں->سروت=(ثابت حرف *)(سروت+1);
        أرجع *سروت;
    }

    /* no output because of empty input */
    *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
    أرجع 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
ساكن فراغ U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverter_From_Unicode_دلیلاں *p_دلیلاں,
                              UError_کوڈ *p_Error_کوڈ) {
    UConverter *cnv;
    ثابت char16_t *سروت, *سروت_حد_بندی;
    uint8_t *نشانہ, *old_نشانہ;
    int32_t نشانہ_سمرتھا, length;
    int32_t *offsets;

    UChar32 cp;
    char16_t c, max;

    int32_t سروت_اشاریہ;

    /* set up the local pointers */
    cnv=p_دلیلاں->converter;
    سروت=p_دلیلاں->سروت;
    سروت_حد_بندی=p_دلیلاں->سروت_حد_بندی;
    نشانہ=old_نشانہ=(uint8_t *)p_دلیلاں->نشانہ;
    نشانہ_سمرتھا=(int32_t)(p_دلیلاں->نشانہ_حد_بندی-p_دلیلاں->نشانہ);
    offsets=p_دلیلاں->offsets;

    إذا(cnv->shared_انکڑے==&Latin1_انکڑے) {
        max=0xff; /* Latin-1 */
    } وإلا {
        max=0x7f; /* US-ASCII */
    }

    /* get the converter state from UConverter */
    cp=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    سروت_اشاریہ= cp==0 ? 0 : -1;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(سروت_حد_بندی-سروت);
    إذا(length<نشانہ_سمرتھا) {
        نشانہ_سمرتھا=length;
    }

    /* conversion loop */
    إذا(cp!=0 && نشانہ_سمرتھا>0) {
        اذهب getTrail;
    }

#إذا LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    إذا(نشانہ_سمرتھا>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=نشانہ_سمرتھا>>4;
        افعل {
            oredChars=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;
            oredChars|=u=*سروت++;
            *نشانہ++=(uint8_t)u;

            /* were all 16 entries really valid? */
            إذا(oredChars>max) {
                /* no, return to the first of these 16 */
                سروت-=16;
                نشانہ-=16;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        نشانہ_سمرتھا-=16*count;

        إذا(offsets!=خالی_سوچک) {
            old_نشانہ+=16*count;
            طالما(count>0) {
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                *offsets++=سروت_اشاریہ++;
                --count;
            }
        }
    }
#جے_انت

    /* conversion loop */
    c=0;
    طالما(نشانہ_سمرتھا>0 && (c=*سروت++)<=max) {
        /* convert the Unicode code point */
        *نشانہ++=(uint8_t)c;
        --نشانہ_سمرتھا;
    }

    إذا(c>max) {
        cp=c;
        إذا(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } وإلا إذا(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            إذا(سروت<سروت_حد_بندی) {
                /* test the following code unit */
                char16_t trail=*سروت;
                إذا(U16_IS_TRAIL(trail)) {
                    ++سروت;
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
                اذهب no_ہور_داخلہ;
            }
        } وإلا {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *p_Error_کوڈ= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_لبھیا : U_INVALID_CHAR_لبھیا;
        cnv->fromUChar32=cp;
    }
no_ہور_داخلہ:

    /* set offsets since the start */
    إذا(offsets!=خالی_سوچک) {
        size_t count=نشانہ-old_نشانہ;
        طالما(count>0) {
            *offsets++=سروت_اشاریہ++;
            --count;
        }
    }

    إذا(U_SUCCESS(*p_Error_کوڈ) && سروت<سروت_حد_بندی && نشانہ>=(uint8_t *)p_دلیلاں->نشانہ_حد_بندی) {
        /* target is full */
        *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
    }

    /* write back the updated pointers */
    p_دلیلاں->سروت=سروت;
    p_دلیلاں->نشانہ=(حرف *)نشانہ;
    p_دلیلاں->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
ساكن فراغ U_CALLCONV
ucnv_Latin1FromUTF8(UConverter_From_Unicode_دلیلاں *pFromUArgs,
                    UConverter_منزل_Unicode_دلیلاں *p_منزل_UArgs,
                    UError_کوڈ *p_Error_کوڈ) {
    UConverter *utf8;
    ثابت uint8_t *سروت, *سروت_حد_بندی;
    uint8_t *نشانہ;
    int32_t نشانہ_سمرتھا;

    UChar32 c;
    uint8_t b, t1;

    /* set up the local pointers */
    utf8=p_منزل_UArgs->converter;
    سروت=(uint8_t *)p_منزل_UArgs->سروت;
    سروت_حد_بندی=(uint8_t *)p_منزل_UArgs->سروت_حد_بندی;
    نشانہ=(uint8_t *)pFromUArgs->نشانہ;
    نشانہ_سمرتھا=(int32_t)(pFromUArgs->نشانہ_حد_بندی-pFromUArgs->نشانہ);

    /* get the converter state from the UTF-8 UConverter */
    إذا (utf8->منزل_ULength > 0) {
        c=(UChar32)utf8->منزل_Unicode_حالت;
    } وإلا {
        c = 0;
    }
    إذا(c!=0 && سروت<سروت_حد_بندی) {
        إذا(نشانہ_سمرتھا==0) {
            *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
            أرجع;
        } وإلا إذا(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*سروت-0x80)) <= 0x3f) {
            ++سروت;
            *نشانہ++=(uint8_t)(((c&3)<<6)|t1);
            --نشانہ_سمرتھا;

            utf8->منزل_Unicode_حالت=0;
            utf8->منزل_ULength=0;
        } وإلا {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *p_Error_کوڈ=U_USING_DEFAULT_WARNING;
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
    إذا(سروت<سروت_حد_بندی && U8_IS_LEAD(*(سروت_حد_بندی-1))) {
        --سروت_حد_بندی;
    }

    /* conversion loop */
    طالما(سروت<سروت_حد_بندی) {
        إذا(نشانہ_سمرتھا>0) {
            b=*سروت++;
            إذا(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *نشانہ++ = b;
                --نشانہ_سمرتھا;
            } وإلا إذا( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*سروت-0x80)) <= 0x3f
            ) {
                ++سروت;
                *نشانہ++=(uint8_t)(((b&3)<<6)|t1);
                --نشانہ_سمرتھا;
            } وإلا {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                p_منزل_UArgs->سروت=(حرف *)(سروت-1);
                pFromUArgs->نشانہ=(حرف *)نشانہ;
                *p_Error_کوڈ=U_USING_DEFAULT_WARNING;
                أرجع;
            }
        } وإلا {
            /* target is full */
            *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
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
    إذا(U_SUCCESS(*p_Error_کوڈ) && سروت<(سروت_حد_بندی=(uint8_t *)p_منزل_UArgs->سروت_حد_بندی)) {
        utf8->منزل_Unicode_حالت=utf8->منزل_UBytes[0]=b=*سروت++;
        utf8->منزل_ULength=1;
        utf8->منوال=U8_گنتی_بائٹاں(b);
    }

    /* write back the updated pointers */
    p_منزل_UArgs->سروت=(حرف *)سروت;
    pFromUArgs->نشانہ=(حرف *)نشانہ;
}

ساكن فراغ U_CALLCONV
Latin1_Get_Unicode_سیٹ(ثابت UConverter *cnv,
                     ثابت USetAdder *sa,
                     UConverter_Unicode_سیٹ which,
                     UError_کوڈ *p_Error_کوڈ) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_کوڈ;
    sa->جوڑو_Range(sa->سیٹ, 0, 0xff);
}
U_CDECL_سرا


ساكن ثابت UConverterImpl _Latin1Impl={
    UCNV_LATIN_1,

    خالی_سوچک,
    خالی_سوچک,

    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,

    Latin1_منزل_Unicode_With_Offsets,
    Latin1_منزل_Unicode_With_Offsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    Latin1_Get_اگلا_UChar,

    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,
    Latin1_Get_Unicode_سیٹ,

    خالی_سوچک,
    ucnv_Latin1FromUTF8
};

ساكن ثابت UConverter_Static_انکڑے Latin1_Static_انکڑے={
    حجم(UConverter_Static_انکڑے),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_انکڑے Latin1_انکڑے=
        UCNV_IMMUTABLE_SHARED_انکڑے_INITIALIZER(&Latin1_Static_انکڑے, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
ساكن فراغ U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverter_منزل_Unicode_دلیلاں *p_دلیلاں,
                           UError_کوڈ *p_Error_کوڈ) {
    ثابت uint8_t *سروت, *سروت_حد_بندی;
    char16_t *نشانہ, *old_نشانہ;
    int32_t نشانہ_سمرتھا, length;
    int32_t *offsets;

    int32_t سروت_اشاریہ;

    uint8_t c;

    /* set up the local pointers */
    سروت=(ثابت uint8_t *)p_دلیلاں->سروت;
    سروت_حد_بندی=(ثابت uint8_t *)p_دلیلاں->سروت_حد_بندی;
    نشانہ=old_نشانہ=p_دلیلاں->نشانہ;
    نشانہ_سمرتھا=(int32_t)(p_دلیلاں->نشانہ_حد_بندی-p_دلیلاں->نشانہ);
    offsets=p_دلیلاں->offsets;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    سروت_اشاریہ=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(سروت_حد_بندی-سروت);
    إذا(length<نشانہ_سمرتھا) {
        نشانہ_سمرتھا=length;
    }

    إذا(نشانہ_سمرتھا>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=نشانہ_سمرتھا>>3;
        افعل {
            oredChars=نشانہ[0]=سروت[0];
            oredChars|=نشانہ[1]=سروت[1];
            oredChars|=نشانہ[2]=سروت[2];
            oredChars|=نشانہ[3]=سروت[3];
            oredChars|=نشانہ[4]=سروت[4];
            oredChars|=نشانہ[5]=سروت[5];
            oredChars|=نشانہ[6]=سروت[6];
            oredChars|=نشانہ[7]=سروت[7];

            /* were all 16 entries really valid? */
            إذا(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                اكسر;
            }
            سروت+=8;
            نشانہ+=8;
        } طالما(--count>0);
        count=loops-count;
        نشانہ_سمرتھا-=count*8;

        إذا(offsets!=خالی_سوچک) {
            old_نشانہ+=count*8;
            طالما(count>0) {
                offsets[0]=سروت_اشاریہ++;
                offsets[1]=سروت_اشاریہ++;
                offsets[2]=سروت_اشاریہ++;
                offsets[3]=سروت_اشاریہ++;
                offsets[4]=سروت_اشاریہ++;
                offsets[5]=سروت_اشاریہ++;
                offsets[6]=سروت_اشاریہ++;
                offsets[7]=سروت_اشاریہ++;
                offsets+=8;
                --count;
            }
        }
    }

    /* conversion loop */
    c=0;
    طالما(نشانہ_سمرتھا>0 && (c=*سروت++)<=0x7f) {
        *نشانہ++=c;
        --نشانہ_سمرتھا;
    }

    إذا(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=p_دلیلاں->converter;
        cnv->منزل_UBytes[0]=c;
        cnv->منزل_ULength=1;
        *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
    } وإلا إذا(سروت<سروت_حد_بندی && نشانہ>=p_دلیلاں->نشانہ_حد_بندی) {
        /* target is full */
        *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
    }

    /* set offsets since the start */
    إذا(offsets!=خالی_سوچک) {
        size_t count=نشانہ-old_نشانہ;
        طالما(count>0) {
            *offsets++=سروت_اشاریہ++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_دلیلاں->سروت=(ثابت حرف *)سروت;
    p_دلیلاں->نشانہ=نشانہ;
    p_دلیلاں->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
ساكن UChar32 U_CALLCONV
ASCIIGet_اگلا_UChar(UConverter_منزل_Unicode_دلیلاں *p_دلیلاں,
                   UError_کوڈ *p_Error_کوڈ) {
    ثابت uint8_t *سروت;
    uint8_t b;

    سروت=(ثابت uint8_t *)p_دلیلاں->سروت;
    إذا(سروت<(ثابت uint8_t *)p_دلیلاں->سروت_حد_بندی) {
        b=*سروت++;
        p_دلیلاں->سروت=(ثابت حرف *)سروت;
        إذا(b<=0x7f) {
            أرجع b;
        } وإلا {
            UConverter *cnv=p_دلیلاں->converter;
            cnv->منزل_UBytes[0]=b;
            cnv->منزل_ULength=1;
            *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
            أرجع 0xffff;
        }
    }

    /* no output because of empty input */
    *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
    أرجع 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
ساكن فراغ U_CALLCONV
ucnv_ASCIIFromUTF8(UConverter_From_Unicode_دلیلاں *pFromUArgs,
                   UConverter_منزل_Unicode_دلیلاں *p_منزل_UArgs,
                   UError_کوڈ *p_Error_کوڈ) {
    ثابت uint8_t *سروت, *سروت_حد_بندی;
    uint8_t *نشانہ;
    int32_t نشانہ_سمرتھا, length;

    uint8_t c;

    إذا(p_منزل_UArgs->converter->منزل_ULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *p_Error_کوڈ=U_USING_DEFAULT_WARNING;
        أرجع;
    }

    /* set up the local pointers */
    سروت=(ثابت uint8_t *)p_منزل_UArgs->سروت;
    سروت_حد_بندی=(ثابت uint8_t *)p_منزل_UArgs->سروت_حد_بندی;
    نشانہ=(uint8_t *)pFromUArgs->نشانہ;
    نشانہ_سمرتھا=(int32_t)(pFromUArgs->نشانہ_حد_بندی-pFromUArgs->نشانہ);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(سروت_حد_بندی-سروت);
    إذا(length<نشانہ_سمرتھا) {
        نشانہ_سمرتھا=length;
    }

    /* unroll the loop with the most common case */
    إذا(نشانہ_سمرتھا>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=نشانہ_سمرتھا>>4;
        افعل {
            oredChars=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;
            oredChars|=*نشانہ++=*سروت++;

            /* were all 16 entries really valid? */
            إذا(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                سروت-=16;
                نشانہ-=16;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        نشانہ_سمرتھا-=16*count;
    }

    /* conversion loop */
    c=0;
    طالما(نشانہ_سمرتھا>0 && (c=*سروت)<=0x7f) {
        ++سروت;
        *نشانہ++=c;
        --نشانہ_سمرتھا;
    }

    إذا(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *p_Error_کوڈ=U_USING_DEFAULT_WARNING;
    } وإلا إذا(سروت<سروت_حد_بندی && نشانہ>=(ثابت uint8_t *)pFromUArgs->نشانہ_حد_بندی) {
        /* target is full */
        *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
    }

    /* write back the updated pointers */
    p_منزل_UArgs->سروت=(ثابت حرف *)سروت;
    pFromUArgs->نشانہ=(حرف *)نشانہ;
}

ساكن فراغ U_CALLCONV
ASCIIGet_Unicode_سیٹ(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverter_Unicode_سیٹ which,
                    UError_کوڈ *p_Error_کوڈ) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_کوڈ;
    sa->جوڑو_Range(sa->سیٹ, 0, 0x7f);
}
U_CDECL_سرا

ساكن ثابت UConverterImpl _ASCIIImpl={
    UCNV_US_ASCII,

    خالی_سوچک,
    خالی_سوچک,

    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,

    _ASCIIToUnicodeWithOffsets,
    _ASCIIToUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    ASCIIGet_اگلا_UChar,

    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,
    خالی_سوچک,
    ASCIIGet_Unicode_سیٹ,

    خالی_سوچک,
    ucnv_ASCIIFromUTF8
};

ساكن ثابت UConverter_Static_انکڑے ASCIIStatic_انکڑے={
    حجم(UConverter_Static_انکڑے),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_انکڑے _ASCIIData=
        UCNV_IMMUTABLE_SHARED_انکڑے_INITIALIZER(&ASCIIStatic_انکڑے, &_ASCIIImpl);

#جے_انت
