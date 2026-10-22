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
#define LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
_Latin1ToUnicodeWithOffsets(UConverter_To_Unicode_حجوجي *p_حجوجي,
                            UErrorCode *pErrorCode) {
    ثابت uint8_t *source;
    char16_t *target;
    int32_t targetCapacity, length;
    int32_t *offsets;

    int32_t sourceIndex;

    /* set up the local pointers */
    source=(ثابت uint8_t *)p_حجوجي->source;
    target=p_حجوجي->target;
    targetCapacity=(int32_t)(p_حجوجي->targetLimit-p_حجوجي->target);
    offsets=p_حجوجي->offsets;

    sourceIndex=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((ثابت uint8_t *)p_حجوجي->sourceLimit-source);
    إذا(length<=targetCapacity) {
        targetCapacity=length;
    } وإلا {
        /* target will be full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        length=targetCapacity;
    }

    إذا(targetCapacity>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=targetCapacity>>3;
        length=targetCapacity&=0x7;
        افعل {
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
        } طالما(--count>0);

        إذا(offsets!=nullptr) {
            افعل {
                offsets[0]=sourceIndex++;
                offsets[1]=sourceIndex++;
                offsets[2]=sourceIndex++;
                offsets[3]=sourceIndex++;
                offsets[4]=sourceIndex++;
                offsets[5]=sourceIndex++;
                offsets[6]=sourceIndex++;
                offsets[7]=sourceIndex++;
                offsets+=8;
            } طالما(--loops>0);
        }
    }

    /* conversion loop */
    طالما(targetCapacity>0) {
        *target++=*source++;
        --targetCapacity;
    }

    /* write back the updated pointers */
    p_حجوجي->source=(ثابت حرف *)source;
    p_حجوجي->target=target;

    /* set offsets */
    إذا(offsets!=nullptr) {
        طالما(length>0) {
            *offsets++=sourceIndex++;
            --length;
        }
        p_حجوجي->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
ساكن UChar32 U_CALLCONV
_Latin1GetNextUChar(UConverter_To_Unicode_حجوجي *p_حجوجي,
                    UErrorCode *pErrorCode) {
    ثابت uint8_t *source=(ثابت uint8_t *)p_حجوجي->source;
    إذا(source<(ثابت uint8_t *)p_حجوجي->sourceLimit) {
        p_حجوجي->source=(ثابت حرف *)(source+1);
        أرجع *source;
    }

    /* no output because of empty input */
    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    أرجع 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
ساكن فراغ U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverter_From_Unicode_حجوجي *p_حجوجي,
                              UErrorCode *pErrorCode) {
    UConverter *cnv;
    ثابت char16_t *source, *sourceLimit;
    uint8_t *target, *oldTarget;
    int32_t targetCapacity, length;
    int32_t *offsets;

    UChar32 cp;
    char16_t c, max;

    int32_t sourceIndex;

    /* set up the local pointers */
    cnv=p_حجوجي->converter;
    source=p_حجوجي->source;
    sourceLimit=p_حجوجي->sourceLimit;
    target=oldTarget=(uint8_t *)p_حجوجي->target;
    targetCapacity=(int32_t)(p_حجوجي->targetLimit-p_حجوجي->target);
    offsets=p_حجوجي->offsets;

    إذا(cnv->sharedData==&_Latin1Data) {
        max=0xff; /* Latin-1 */
    } وإلا {
        max=0x7f; /* US-ASCII */
    }

    /* get the converter state from UConverter */
    cp=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    sourceIndex= cp==0 ? 0 : -1;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(sourceLimit-source);
    إذا(length<targetCapacity) {
        targetCapacity=length;
    }

    /* conversion loop */
    إذا(cp!=0 && targetCapacity>0) {
        اذهب getTrail;
    }

#إذا LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    إذا(targetCapacity>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=targetCapacity>>4;
        افعل {
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
            إذا(oredChars>max) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        targetCapacity-=16*count;

        إذا(offsets!=nullptr) {
            oldTarget+=16*count;
            طالما(count>0) {
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                *offsets++=sourceIndex++;
                --count;
            }
        }
    }
#endif

    /* conversion loop */
    c=0;
    طالما(targetCapacity>0 && (c=*source++)<=max) {
        /* convert the Unicode code point */
        *target++=(uint8_t)c;
        --targetCapacity;
    }

    إذا(c>max) {
        cp=c;
        إذا(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } وإلا إذا(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            إذا(source<sourceLimit) {
                /* test the following code unit */
                char16_t trail=*source;
                إذا(U16_IS_TRAIL(trail)) {
                    ++source;
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
                اذهب no_More_شغرو;
            }
        } وإلا {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *pErrorCode= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_FOUND : U_INVALID_CHAR_FOUND;
        cnv->fromUChar32=cp;
    }
no_More_شغرو:

    /* set offsets since the start */
    إذا(offsets!=nullptr) {
        size_t count=target-oldTarget;
        طالما(count>0) {
            *offsets++=sourceIndex++;
            --count;
        }
    }

    إذا(U_SUCCESS(*pErrorCode) && source<sourceLimit && target>=(uint8_t *)p_حجوجي->targetLimit) {
        /* target is full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    p_حجوجي->source=source;
    p_حجوجي->target=(حرف *)target;
    p_حجوجي->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
ساكن فراغ U_CALLCONV
ucnv_Latin1FromUTF8(UConverter_From_Unicode_حجوجي *pFromUArgs,
                    UConverter_To_Unicode_حجوجي *pToUArgs,
                    UErrorCode *pErrorCode) {
    UConverter *utf8;
    ثابت uint8_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity;

    UChar32 c;
    uint8_t b, t1;

    /* set up the local pointers */
    utf8=pToUArgs->converter;
    source=(uint8_t *)pToUArgs->source;
    sourceLimit=(uint8_t *)pToUArgs->sourceLimit;
    target=(uint8_t *)pFromUArgs->target;
    targetCapacity=(int32_t)(pFromUArgs->targetLimit-pFromUArgs->target);

    /* get the converter state from the UTF-8 UConverter */
    إذا (utf8->toULength > 0) {
        c=(UChar32)utf8->toUnicodeStatus;
    } وإلا {
        c = 0;
    }
    إذا(c!=0 && source<sourceLimit) {
        إذا(targetCapacity==0) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            أرجع;
        } وإلا إذا(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*source-0x80)) <= 0x3f) {
            ++source;
            *target++=(uint8_t)(((c&3)<<6)|t1);
            --targetCapacity;

            utf8->toUnicodeStatus=0;
            utf8->toULength=0;
        } وإلا {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *pErrorCode=U_USING_DEFAULT_WARNING;
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
    إذا(source<sourceLimit && U8_IS_LEAD(*(sourceLimit-1))) {
        --sourceLimit;
    }

    /* conversion loop */
    طالما(source<sourceLimit) {
        إذا(targetCapacity>0) {
            b=*source++;
            إذا(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *target++ = b;
                --targetCapacity;
            } وإلا إذا( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*source-0x80)) <= 0x3f
            ) {
                ++source;
                *target++=(uint8_t)(((b&3)<<6)|t1);
                --targetCapacity;
            } وإلا {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                pToUArgs->source=(حرف *)(source-1);
                pFromUArgs->target=(حرف *)target;
                *pErrorCode=U_USING_DEFAULT_WARNING;
                أرجع;
            }
        } وإلا {
            /* target is full */
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
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
    إذا(U_SUCCESS(*pErrorCode) && source<(sourceLimit=(uint8_t *)pToUArgs->sourceLimit)) {
        utf8->toUnicodeStatus=utf8->toUBytes[0]=b=*source++;
        utf8->toULength=1;
        utf8->mode=U8_كدي_BYTES(b);
    }

    /* write back the updated pointers */
    pToUArgs->source=(حرف *)source;
    pFromUArgs->target=(حرف *)target;
}

ساكن فراغ U_CALLCONV
_Latin1GetUnicodeSet(ثابت UConverter *cnv,
                     ثابت USetAdder *sa,
                     UConverterUnicodeSet which,
                     UErrorCode *pErrorCode) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)pErrorCode;
    sa->addRange(sa->set, 0, 0xff);
}
U_CDECL_END


ساكن ثابت UConverterImpl _Latin1Impl={
    UCNV_LATIN_1,

    nullptr,
    nullptr,

    nullptr,
    nullptr,
    nullptr,

    _Latin1ToUnicodeWithOffsets,
    _Latin1ToUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1GetNextUChar,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    _Latin1GetUnicodeSet,

    nullptr,
    ucnv_Latin1FromUTF8
};

ساكن ثابت UConverterStaticData _Latin1StaticData={
    حجم(UConverterStaticData),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverterSharedData _Latin1Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_Latin1StaticData, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
ساكن فراغ U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverter_To_Unicode_حجوجي *p_حجوجي,
                           UErrorCode *pErrorCode) {
    ثابت uint8_t *source, *sourceLimit;
    char16_t *target, *oldTarget;
    int32_t targetCapacity, length;
    int32_t *offsets;

    int32_t sourceIndex;

    uint8_t c;

    /* set up the local pointers */
    source=(ثابت uint8_t *)p_حجوجي->source;
    sourceLimit=(ثابت uint8_t *)p_حجوجي->sourceLimit;
    target=oldTarget=p_حجوجي->target;
    targetCapacity=(int32_t)(p_حجوجي->targetLimit-p_حجوجي->target);
    offsets=p_حجوجي->offsets;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    sourceIndex=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(sourceLimit-source);
    إذا(length<targetCapacity) {
        targetCapacity=length;
    }

    إذا(targetCapacity>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=targetCapacity>>3;
        افعل {
            oredChars=target[0]=source[0];
            oredChars|=target[1]=source[1];
            oredChars|=target[2]=source[2];
            oredChars|=target[3]=source[3];
            oredChars|=target[4]=source[4];
            oredChars|=target[5]=source[5];
            oredChars|=target[6]=source[6];
            oredChars|=target[7]=source[7];

            /* were all 16 entries really valid? */
            إذا(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                اكسر;
            }
            source+=8;
            target+=8;
        } طالما(--count>0);
        count=loops-count;
        targetCapacity-=count*8;

        إذا(offsets!=nullptr) {
            oldTarget+=count*8;
            طالما(count>0) {
                offsets[0]=sourceIndex++;
                offsets[1]=sourceIndex++;
                offsets[2]=sourceIndex++;
                offsets[3]=sourceIndex++;
                offsets[4]=sourceIndex++;
                offsets[5]=sourceIndex++;
                offsets[6]=sourceIndex++;
                offsets[7]=sourceIndex++;
                offsets+=8;
                --count;
            }
        }
    }

    /* conversion loop */
    c=0;
    طالما(targetCapacity>0 && (c=*source++)<=0x7f) {
        *target++=c;
        --targetCapacity;
    }

    إذا(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=p_حجوجي->converter;
        cnv->toUBytes[0]=c;
        cnv->toULength=1;
        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
    } وإلا إذا(source<sourceLimit && target>=p_حجوجي->targetLimit) {
        /* target is full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* set offsets since the start */
    إذا(offsets!=nullptr) {
        size_t count=target-oldTarget;
        طالما(count>0) {
            *offsets++=sourceIndex++;
            --count;
        }
    }

    /* write back the updated pointers */
    p_حجوجي->source=(ثابت حرف *)source;
    p_حجوجي->target=target;
    p_حجوجي->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
ساكن UChar32 U_CALLCONV
_ASCIIGetNextUChar(UConverter_To_Unicode_حجوجي *p_حجوجي,
                   UErrorCode *pErrorCode) {
    ثابت uint8_t *source;
    uint8_t b;

    source=(ثابت uint8_t *)p_حجوجي->source;
    إذا(source<(ثابت uint8_t *)p_حجوجي->sourceLimit) {
        b=*source++;
        p_حجوجي->source=(ثابت حرف *)source;
        إذا(b<=0x7f) {
            أرجع b;
        } وإلا {
            UConverter *cnv=p_حجوجي->converter;
            cnv->toUBytes[0]=b;
            cnv->toULength=1;
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            أرجع 0xffff;
        }
    }

    /* no output because of empty input */
    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    أرجع 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
ساكن فراغ U_CALLCONV
ucnv_ASCIIFromUTF8(UConverter_From_Unicode_حجوجي *pFromUArgs,
                   UConverter_To_Unicode_حجوجي *pToUArgs,
                   UErrorCode *pErrorCode) {
    ثابت uint8_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity, length;

    uint8_t c;

    إذا(pToUArgs->converter->toULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        أرجع;
    }

    /* set up the local pointers */
    source=(ثابت uint8_t *)pToUArgs->source;
    sourceLimit=(ثابت uint8_t *)pToUArgs->sourceLimit;
    target=(uint8_t *)pFromUArgs->target;
    targetCapacity=(int32_t)(pFromUArgs->targetLimit-pFromUArgs->target);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(sourceLimit-source);
    إذا(length<targetCapacity) {
        targetCapacity=length;
    }

    /* unroll the loop with the most common case */
    إذا(targetCapacity>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=targetCapacity>>4;
        افعل {
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
            إذا(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                اكسر;
            }
        } طالما(--count>0);
        count=loops-count;
        targetCapacity-=16*count;
    }

    /* conversion loop */
    c=0;
    طالما(targetCapacity>0 && (c=*source)<=0x7f) {
        ++source;
        *target++=c;
        --targetCapacity;
    }

    إذا(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *pErrorCode=U_USING_DEFAULT_WARNING;
    } وإلا إذا(source<sourceLimit && target>=(ثابت uint8_t *)pFromUArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pToUArgs->source=(ثابت حرف *)source;
    pFromUArgs->target=(حرف *)target;
}

ساكن فراغ U_CALLCONV
_ASCIIGetUnicodeSet(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverterUnicodeSet which,
                    UErrorCode *pErrorCode) {
    (فراغ)cnv;
    (فراغ)which;
    (فراغ)pErrorCode;
    sa->addRange(sa->set, 0, 0x7f);
}
U_CDECL_END

ساكن ثابت UConverterImpl _ASCIIImpl={
    UCNV_US_ASCII,

    nullptr,
    nullptr,

    nullptr,
    nullptr,
    nullptr,

    _ASCIIToUnicodeWithOffsets,
    _ASCIIToUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    _Latin1FromUnicodeWithOffsets,
    _ASCIIGetNextUChar,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    _ASCIIGetUnicodeSet,

    nullptr,
    ucnv_ASCIIFromUTF8
};

ساكن ثابت UConverterStaticData _ASCIIStaticData={
    حجم(UConverterStaticData),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverterSharedData _ASCIIData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_ASCIIStaticData, &_ASCIIImpl);

#endif
