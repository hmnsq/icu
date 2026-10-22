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
#تعریف_کړه LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
Latin1_منزل_Unicode_With_Offsets(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه,
                            UError_کوډ *p_Error_کوډ) {
    ثابت uint8_t *سرچینه;
    char16_t *موخه;
    int32_t موخه_ظرفیت, length;
    int32_t *offsets;

    int32_t سرچینه_شاخص;

    /* set up the local pointers */
    سرچینه=(ثابت uint8_t *)p_دلیلونه->سرچینه;
    موخه=p_دلیلونه->موخه;
    موخه_ظرفیت=(int32_t)(p_دلیلونه->موخه_حد-p_دلیلونه->موخه);
    offsets=p_دلیلونه->offsets;

    سرچینه_شاخص=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((ثابت uint8_t *)p_دلیلونه->سرچینه_حد-سرچینه);
    إذا(length<=موخه_ظرفیت) {
        موخه_ظرفیت=length;
    } وإلا {
        /* target will be full */
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
        length=موخه_ظرفیت;
    }

    إذا(موخه_ظرفیت>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=موخه_ظرفیت>>3;
        length=موخه_ظرفیت&=0x7;
        افعل {
            موخه[0]=سرچینه[0];
            موخه[1]=سرچینه[1];
            موخه[2]=سرچینه[2];
            موخه[3]=سرچینه[3];
            موخه[4]=سرچینه[4];
            موخه[5]=سرچینه[5];
            موخه[6]=سرچینه[6];
            موخه[7]=سرچینه[7];
            موخه+=8;
            سرچینه+=8;
        } طالما(--count>0);

        إذا(offsets!=تش_اشاره) {
            افعل {
                offsets[0]=سرچینه_شاخص++;
                offsets[1]=سرچینه_شاخص++;
                offsets[2]=سرچینه_شاخص++;
                offsets[3]=سرچینه_شاخص++;
                offsets[4]=سرچینه_شاخص++;
                offsets[5]=سرچینه_شاخص++;
                offsets[6]=سرچینه_شاخص++;
                offsets[7]=سرچینه_شاخص++;
                offsets+=8;
            } طالما(--loops>0);
        }
    }

    /* conversion loop */
    طالما(موخه_ظرفیت>0) {
        *موخه++=*سرچینه++;
        --موخه_ظرفیت;
    }

    /* write back the updated pointers */
    p_دلیلونه->سرچینه=(ثابت حرف *)سرچینه;
    p_دلیلونه->موخه=موخه;

    /* set offsets */
    إذا(offsets!=تش_اشاره) {
        طالما(length>0) {
            *offsets++=سرچینه_شاخص++;
            --length;
        }
        p_دلیلونه->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
ساكن UChar32 U_CALLCONV
Latin1_Get_بل_UChar(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه,
                    UError_کوډ *p_Error_کوډ) {
    ثابت uint8_t *سرچینه=(ثابت uint8_t *)p_دلیلونه->سرچینه;
    إذا(سرچینه<(ثابت uint8_t *)p_دلیلونه->سرچینه_حد) {
        p_دلیلونه->سرچینه=(ثابت حرف *)(سرچینه+1);
        أرجع *سرچینه;
    }

    /* no output because of empty input */
    *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
    أرجع 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
ساكن فراغ U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverter_From_Unicode_دلیلونه *p_دلیلونه,
                              UError_کوډ *p_Error_کوډ) {
    UConverter *cnv;
    ثابت char16_t *سرچینه, *سرچینه_حد;
    uint8_t *موخه, *old_موخه;
    int32_t موخه_ظرفیت, length;
    int32_t *offsets;

    UChar32 cp;
    char16_t c, max;

    int32_t سرچینه_شاخص;

    /* set up the local pointers */
    cnv=p_دلیلونه->converter;
    سرچینه=p_دلیلونه->سرچینه;
    سرچینه_حد=p_دلیلونه->سرچینه_حد;
    موخه=old_موخه=(uint8_t *)p_دلیلونه->موخه;
    موخه_ظرفیت=(int32_t)(p_دلیلونه->موخه_حد-p_دلیلونه->موخه);
    offsets=p_دلیلونه->offsets;

    إذا(cnv->shared_معلومات==&Latin1_معلومات) {
        max=0xff; /* Latin-1 */
    } وإلا {
        max=0x7f; /* US-ASCII */
    }

    /* get the converter state from UConverter */
    cp=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    سرچینه_شاخص= cp==0 ? 0 : -1;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(سرچینه_حد-سرچینه);
    إذا(length<موخه_ظرفیت) {
        موخه_ظرفیت=length;
    }

    /* conversion loop */
    إذا(cp!=0 && موخه_ظرفیت>0) {
        اذهب getTrail;
    }

#إذا LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    إذا(موخه_ظرفیت>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=موخه_ظرفیت>>4;
        افعل {
            oredChars=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;
            oredChars|=u=*سرچینه++;
            *موخه++=(uint8_t)u;

            /* were all 16 entries really valid? */
            إذا(oredChars>max) {
                /* no, return to the first of these 16 */
                سرچینه-=16;
                موخه-=16;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        موخه_ظرفیت-=16*count;

        إذا(offsets!=تش_اشاره) {
            old_موخه+=16*count;
            طالما(count>0) {
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                *offsets++=سرچینه_شاخص++;
                --count;
            }
        }
    }
#د_که_پای

    /* conversion loop */
    c=0;
    طالما(موخه_ظرفیت>0 && (c=*سرچینه++)<=max) {
        /* convert the Unicode code point */
        *موخه++=(uint8_t)c;
        --موخه_ظرفیت;
    }

    إذا(c>max) {
        cp=c;
        إذا(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } وإلا إذا(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            إذا(سرچینه<سرچینه_حد) {
                /* test the following code unit */
                char16_t trail=*سرچینه;
                إذا(U16_IS_TRAIL(trail)) {
                    ++سرچینه;
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
                اذهب no_نور_ننوت;
            }
        } وإلا {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *p_Error_کوډ= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_وموندل : U_INVALID_CHAR_وموندل;
        cnv->fromUChar32=cp;
    }
no_نور_ننوت:

    /* set offsets since the start */
    إذا(offsets!=تش_اشاره) {
        size_t count=موخه-old_موخه;
        طالما(count>0) {
            *offsets++=سرچینه_شاخص++;
            --count;
        }
    }

    إذا(U_SUCCESS(*p_Error_کوډ) && سرچینه<سرچینه_حد && موخه>=(uint8_t *)p_دلیلونه->موخه_حد) {
        /* target is full */
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
    }

    /* write back the updated pointers */
    p_دلیلونه->سرچینه=سرچینه;
    p_دلیلونه->موخه=(حرف *)موخه;
    p_دلیلونه->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
ساكن فراغ U_CALLCONV
ucnv_Latin1FromUTF8(UConverter_From_Unicode_دلیلونه *pFromUArgs,
                    UConverter_منزل_Unicode_دلیلونه *p_منزل_UArgs,
                    UError_کوډ *p_Error_کوډ) {
    UConverter *utf8;
    ثابت uint8_t *سرچینه, *سرچینه_حد;
    uint8_t *موخه;
    int32_t موخه_ظرفیت;

    UChar32 c;
    uint8_t b, t1;

    /* set up the local pointers */
    utf8=p_منزل_UArgs->converter;
    سرچینه=(uint8_t *)p_منزل_UArgs->سرچینه;
    سرچینه_حد=(uint8_t *)p_منزل_UArgs->سرچینه_حد;
    موخه=(uint8_t *)pFromUArgs->موخه;
    موخه_ظرفیت=(int32_t)(pFromUArgs->موخه_حد-pFromUArgs->موخه);

    /* get the converter state from the UTF-8 UConverter */
    إذا (utf8->منزل_ULength > 0) {
        c=(UChar32)utf8->منزل_Unicode_حالت;
    } وإلا {
        c = 0;
    }
    إذا(c!=0 && سرچینه<سرچینه_حد) {
        إذا(موخه_ظرفیت==0) {
            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
            أرجع;
        } وإلا إذا(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*سرچینه-0x80)) <= 0x3f) {
            ++سرچینه;
            *موخه++=(uint8_t)(((c&3)<<6)|t1);
            --موخه_ظرفیت;

            utf8->منزل_Unicode_حالت=0;
            utf8->منزل_ULength=0;
        } وإلا {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *p_Error_کوډ=U_USING_DEFAULT_WARNING;
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
    إذا(سرچینه<سرچینه_حد && U8_IS_LEAD(*(سرچینه_حد-1))) {
        --سرچینه_حد;
    }

    /* conversion loop */
    طالما(سرچینه<سرچینه_حد) {
        إذا(موخه_ظرفیت>0) {
            b=*سرچینه++;
            إذا(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *موخه++ = b;
                --موخه_ظرفیت;
            } وإلا إذا( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*سرچینه-0x80)) <= 0x3f
            ) {
                ++سرچینه;
                *موخه++=(uint8_t)(((b&3)<<6)|t1);
                --موخه_ظرفیت;
            } وإلا {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                p_منزل_UArgs->سرچینه=(حرف *)(سرچینه-1);
                pFromUArgs->موخه=(حرف *)موخه;
                *p_Error_کوډ=U_USING_DEFAULT_WARNING;
                أرجع;
            }
        } وإلا {
            /* target is full */
            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
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
    إذا(U_SUCCESS(*p_Error_کوډ) && سرچینه<(سرچینه_حد=(uint8_t *)p_منزل_UArgs->سرچینه_حد)) {
        utf8->منزل_Unicode_حالت=utf8->منزل_UBytes[0]=b=*سرچینه++;
        utf8->منزل_ULength=1;
        utf8->ډېرپېښ=U8_شمېر_بایټونه(b);
    }

    /* write back the updated pointers */
    p_منزل_UArgs->سرچینه=(حرف *)سرچینه;
    pFromUArgs->موخه=(حرف *)موخه;
}

ساكن فراغ U_CALLCONV
Latin1_Get_Unicode_سېټ(ثابت UConverter *cnv,
                     ثابت USetAdder *sa,
                     UConverter_Unicode_سېټ which,
                     UError_کوډ *p_Error_کوډ) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_کوډ;
    sa->جمع_کړه_Range(sa->سېټ, 0, 0xff);
}
U_CDECL_پای_ټکی


ساكن ثابت UConverterImpl _Latin1Impl={
    UCNV_LATIN_1,

    تش_اشاره,
    تش_اشاره,

    تش_اشاره,
    تش_اشاره,
    تش_اشاره,

    Latin1_منزل_Unicode_With_Offsets,
    Latin1_منزل_Unicode_With_Offsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    Latin1_Get_بل_UChar,

    تش_اشاره,
    تش_اشاره,
    تش_اشاره,
    تش_اشاره,
    Latin1_Get_Unicode_سېټ,

    تش_اشاره,
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
_ASCIIToUnicodeWithOffsets(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه,
                           UError_کوډ *p_Error_کوډ) {
    ثابت uint8_t *سرچینه, *سرچینه_حد;
    char16_t *موخه, *old_موخه;
    int32_t موخه_ظرفیت, length;
    int32_t *offsets;

    int32_t سرچینه_شاخص;

    uint8_t c;

    /* set up the local pointers */
    سرچینه=(ثابت uint8_t *)p_دلیلونه->سرچینه;
    سرچینه_حد=(ثابت uint8_t *)p_دلیلونه->سرچینه_حد;
    موخه=old_موخه=p_دلیلونه->موخه;
    موخه_ظرفیت=(int32_t)(p_دلیلونه->موخه_حد-p_دلیلونه->موخه);
    offsets=p_دلیلونه->offsets;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    سرچینه_شاخص=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(سرچینه_حد-سرچینه);
    إذا(length<موخه_ظرفیت) {
        موخه_ظرفیت=length;
    }

    إذا(موخه_ظرفیت>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=موخه_ظرفیت>>3;
        افعل {
            oredChars=موخه[0]=سرچینه[0];
            oredChars|=موخه[1]=سرچینه[1];
            oredChars|=موخه[2]=سرچینه[2];
            oredChars|=موخه[3]=سرچینه[3];
            oredChars|=موخه[4]=سرچینه[4];
            oredChars|=موخه[5]=سرچینه[5];
            oredChars|=موخه[6]=سرچینه[6];
            oredChars|=موخه[7]=سرچینه[7];

            /* were all 16 entries really valid? */
            إذا(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                اكسر;
            }
            سرچینه+=8;
            موخه+=8;
        } طالما(--count>0);
        count=loops-count;
        موخه_ظرفیت-=count*8;

        إذا(offsets!=تش_اشاره) {
            old_موخه+=count*8;
            طالما(count>0) {
                offsets[0]=سرچینه_شاخص++;
                offsets[1]=سرچینه_شاخص++;
                offsets[2]=سرچینه_شاخص++;
                offsets[3]=سرچینه_شاخص++;
                offsets[4]=سرچینه_شاخص++;
                offsets[5]=سرچینه_شاخص++;
                offsets[6]=سرچینه_شاخص++;
                offsets[7]=سرچینه_شاخص++;
                offsets+=8;
                --count;
            }
        }
    }

    /* conversion loop */
    c=0;
    طالما(موخه_ظرفیت>0 && (c=*سرچینه++)<=0x7f) {
        *موخه++=c;
        --موخه_ظرفیت;
    }

    إذا(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=p_دلیلونه->converter;
        cnv->منزل_UBytes[0]=c;
        cnv->منزل_ULength=1;
        *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
    } وإلا إذا(سرچینه<سرچینه_حد && موخه>=p_دلیلونه->موخه_حد) {
        /* target is full */
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
    }

    /* set offsets since the start */
    إذا(offsets!=تش_اشاره) {
        size_t count=موخه-old_موخه;
        طالما(count>0) {
            *offsets++=سرچینه_شاخص++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_دلیلونه->سرچینه=(ثابت حرف *)سرچینه;
    p_دلیلونه->موخه=موخه;
    p_دلیلونه->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
ساكن UChar32 U_CALLCONV
ASCIIGet_بل_UChar(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه,
                   UError_کوډ *p_Error_کوډ) {
    ثابت uint8_t *سرچینه;
    uint8_t b;

    سرچینه=(ثابت uint8_t *)p_دلیلونه->سرچینه;
    إذا(سرچینه<(ثابت uint8_t *)p_دلیلونه->سرچینه_حد) {
        b=*سرچینه++;
        p_دلیلونه->سرچینه=(ثابت حرف *)سرچینه;
        إذا(b<=0x7f) {
            أرجع b;
        } وإلا {
            UConverter *cnv=p_دلیلونه->converter;
            cnv->منزل_UBytes[0]=b;
            cnv->منزل_ULength=1;
            *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
            أرجع 0xffff;
        }
    }

    /* no output because of empty input */
    *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
    أرجع 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
ساكن فراغ U_CALLCONV
ucnv_ASCIIFromUTF8(UConverter_From_Unicode_دلیلونه *pFromUArgs,
                   UConverter_منزل_Unicode_دلیلونه *p_منزل_UArgs,
                   UError_کوډ *p_Error_کوډ) {
    ثابت uint8_t *سرچینه, *سرچینه_حد;
    uint8_t *موخه;
    int32_t موخه_ظرفیت, length;

    uint8_t c;

    إذا(p_منزل_UArgs->converter->منزل_ULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *p_Error_کوډ=U_USING_DEFAULT_WARNING;
        أرجع;
    }

    /* set up the local pointers */
    سرچینه=(ثابت uint8_t *)p_منزل_UArgs->سرچینه;
    سرچینه_حد=(ثابت uint8_t *)p_منزل_UArgs->سرچینه_حد;
    موخه=(uint8_t *)pFromUArgs->موخه;
    موخه_ظرفیت=(int32_t)(pFromUArgs->موخه_حد-pFromUArgs->موخه);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(سرچینه_حد-سرچینه);
    إذا(length<موخه_ظرفیت) {
        موخه_ظرفیت=length;
    }

    /* unroll the loop with the most common case */
    إذا(موخه_ظرفیت>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=موخه_ظرفیت>>4;
        افعل {
            oredChars=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;
            oredChars|=*موخه++=*سرچینه++;

            /* were all 16 entries really valid? */
            إذا(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                سرچینه-=16;
                موخه-=16;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        موخه_ظرفیت-=16*count;
    }

    /* conversion loop */
    c=0;
    طالما(موخه_ظرفیت>0 && (c=*سرچینه)<=0x7f) {
        ++سرچینه;
        *موخه++=c;
        --موخه_ظرفیت;
    }

    إذا(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *p_Error_کوډ=U_USING_DEFAULT_WARNING;
    } وإلا إذا(سرچینه<سرچینه_حد && موخه>=(ثابت uint8_t *)pFromUArgs->موخه_حد) {
        /* target is full */
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
    }

    /* write back the updated pointers */
    p_منزل_UArgs->سرچینه=(ثابت حرف *)سرچینه;
    pFromUArgs->موخه=(حرف *)موخه;
}

ساكن فراغ U_CALLCONV
ASCIIGet_Unicode_سېټ(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverter_Unicode_سېټ which,
                    UError_کوډ *p_Error_کوډ) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)p_Error_کوډ;
    sa->جمع_کړه_Range(sa->سېټ, 0, 0x7f);
}
U_CDECL_پای_ټکی

ساكن ثابت UConverterImpl _ASCIIImpl={
    UCNV_US_ASCII,

    تش_اشاره,
    تش_اشاره,

    تش_اشاره,
    تش_اشاره,
    تش_اشاره,

    _ASCIIToUnicodeWithOffsets,
    _ASCIIToUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    ASCIIGet_بل_UChar,

    تش_اشاره,
    تش_اشاره,
    تش_اشاره,
    تش_اشاره,
    ASCIIGet_Unicode_سېټ,

    تش_اشاره,
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

#د_که_پای
