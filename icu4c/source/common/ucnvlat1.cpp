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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION

#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "unicode/uset.h"
#അന്തര്ഭാവ "unicode/utf8.h"
#അന്തര്ഭാവ "ucnv_bld.h"
#അന്തര്ഭാവ "ucnv_cnv.h"
#അന്തര്ഭാവ "ustr_imp.h"

/* control optimizations according to the platform */
#define LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
സ്ഥിതം ശൂന്യം U_CALLCONV
_Latin1ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                            UErrorCode *pErrorCode) {
    സ്ഥിരം uint8_t *source;
    char16_t *target;
    int32_t targetCapacity, length;
    int32_t *offsets;

    int32_t sourceIndex;

    /* set up the local pointers */
    source=(സ്ഥിരം uint8_t *)pArgs->source;
    target=pArgs->target;
    targetCapacity=(int32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;

    sourceIndex=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((സ്ഥിരം uint8_t *)pArgs->sourceLimit-source);
    എങ്കിൽ(length<=targetCapacity) {
        targetCapacity=length;
    } അല്ലെങ്കിൽ {
        /* target will be full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        length=targetCapacity;
    }

    എങ്കിൽ(targetCapacity>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=targetCapacity>>3;
        length=targetCapacity&=0x7;
        ചെയ്യുക {
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
        } ആകുമ്പോൾ(--count>0);

        എങ്കിൽ(offsets!=nullptr) {
            ചെയ്യുക {
                offsets[0]=sourceIndex++;
                offsets[1]=sourceIndex++;
                offsets[2]=sourceIndex++;
                offsets[3]=sourceIndex++;
                offsets[4]=sourceIndex++;
                offsets[5]=sourceIndex++;
                offsets[6]=sourceIndex++;
                offsets[7]=sourceIndex++;
                offsets+=8;
            } ആകുമ്പോൾ(--loops>0);
        }
    }

    /* conversion loop */
    ആകുമ്പോൾ(targetCapacity>0) {
        *target++=*source++;
        --targetCapacity;
    }

    /* write back the updated pointers */
    pArgs->source=(സ്ഥിരം അക്ഷരം *)source;
    pArgs->target=target;

    /* set offsets */
    എങ്കിൽ(offsets!=nullptr) {
        ആകുമ്പോൾ(length>0) {
            *offsets++=sourceIndex++;
            --length;
        }
        pArgs->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
സ്ഥിതം UChar32 U_CALLCONV
_Latin1GetNextUChar(UConverterToUnicodeArgs *pArgs,
                    UErrorCode *pErrorCode) {
    സ്ഥിരം uint8_t *source=(സ്ഥിരം uint8_t *)pArgs->source;
    എങ്കിൽ(source<(സ്ഥിരം uint8_t *)pArgs->sourceLimit) {
        pArgs->source=(സ്ഥിരം അക്ഷരം *)(source+1);
        തിരികെ *source;
    }

    /* no output because of empty input */
    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    തിരികെ 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
സ്ഥിതം ശൂന്യം U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                              UErrorCode *pErrorCode) {
    UConverter *cnv;
    സ്ഥിരം char16_t *source, *sourceLimit;
    uint8_t *target, *oldTarget;
    int32_t targetCapacity, length;
    int32_t *offsets;

    UChar32 cp;
    char16_t c, max;

    int32_t sourceIndex;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source=pArgs->source;
    sourceLimit=pArgs->sourceLimit;
    target=oldTarget=(uint8_t *)pArgs->target;
    targetCapacity=(int32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;

    എങ്കിൽ(cnv->sharedData==&_Latin1Data) {
        max=0xff; /* Latin-1 */
    } അല്ലെങ്കിൽ {
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
    എങ്കിൽ(length<targetCapacity) {
        targetCapacity=length;
    }

    /* conversion loop */
    എങ്കിൽ(cp!=0 && targetCapacity>0) {
        പോകുക getTrail;
    }

#എങ്കിൽ LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    എങ്കിൽ(targetCapacity>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=targetCapacity>>4;
        ചെയ്യുക {
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
            എങ്കിൽ(oredChars>max) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                നിർത്തുക;
            }
        } ആകുമ്പോൾ(--count>0);
        count=loops-count;
        targetCapacity-=16*count;

        എങ്കിൽ(offsets!=nullptr) {
            oldTarget+=16*count;
            ആകുമ്പോൾ(count>0) {
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
    ആകുമ്പോൾ(targetCapacity>0 && (c=*source++)<=max) {
        /* convert the Unicode code point */
        *target++=(uint8_t)c;
        --targetCapacity;
    }

    എങ്കിൽ(c>max) {
        cp=c;
        എങ്കിൽ(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } അല്ലെങ്കിൽ എങ്കിൽ(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            എങ്കിൽ(source<sourceLimit) {
                /* test the following code unit */
                char16_t trail=*source;
                എങ്കിൽ(U16_IS_TRAIL(trail)) {
                    ++source;
                    cp=U16_GET_SUPPLEMENTARY(cp, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } അല്ലെങ്കിൽ {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                }
            } അല്ലെങ്കിൽ {
                /* no more input */
                cnv->fromUChar32=cp;
                പോകുക no_More_നിവേശം;
            }
        } അല്ലെങ്കിൽ {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *pErrorCode= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_FOUND : U_INVALID_CHAR_FOUND;
        cnv->fromUChar32=cp;
    }
no_More_നിവേശം:

    /* set offsets since the start */
    എങ്കിൽ(offsets!=nullptr) {
        size_t count=target-oldTarget;
        ആകുമ്പോൾ(count>0) {
            *offsets++=sourceIndex++;
            --count;
        }
    }

    എങ്കിൽ(U_SUCCESS(*pErrorCode) && source<sourceLimit && target>=(uint8_t *)pArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target=(അക്ഷരം *)target;
    pArgs->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
സ്ഥിതം ശൂന്യം U_CALLCONV
ucnv_Latin1FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                    UConverterToUnicodeArgs *pToUArgs,
                    UErrorCode *pErrorCode) {
    UConverter *utf8;
    സ്ഥിരം uint8_t *source, *sourceLimit;
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
    എങ്കിൽ (utf8->toULength > 0) {
        c=(UChar32)utf8->toUnicodeStatus;
    } അല്ലെങ്കിൽ {
        c = 0;
    }
    എങ്കിൽ(c!=0 && source<sourceLimit) {
        എങ്കിൽ(targetCapacity==0) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            തിരികെ;
        } അല്ലെങ്കിൽ എങ്കിൽ(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*source-0x80)) <= 0x3f) {
            ++source;
            *target++=(uint8_t)(((c&3)<<6)|t1);
            --targetCapacity;

            utf8->toUnicodeStatus=0;
            utf8->toULength=0;
        } അല്ലെങ്കിൽ {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *pErrorCode=U_USING_DEFAULT_WARNING;
            തിരികെ;
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
    എങ്കിൽ(source<sourceLimit && U8_IS_LEAD(*(sourceLimit-1))) {
        --sourceLimit;
    }

    /* conversion loop */
    ആകുമ്പോൾ(source<sourceLimit) {
        എങ്കിൽ(targetCapacity>0) {
            b=*source++;
            എങ്കിൽ(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *target++ = b;
                --targetCapacity;
            } അല്ലെങ്കിൽ എങ്കിൽ( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*source-0x80)) <= 0x3f
            ) {
                ++source;
                *target++=(uint8_t)(((b&3)<<6)|t1);
                --targetCapacity;
            } അല്ലെങ്കിൽ {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                pToUArgs->source=(അക്ഷരം *)(source-1);
                pFromUArgs->target=(അക്ഷരം *)target;
                *pErrorCode=U_USING_DEFAULT_WARNING;
                തിരികെ;
            }
        } അല്ലെങ്കിൽ {
            /* target is full */
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            നിർത്തുക;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     * For Latin-1, there is at most exactly one lead byte because of the
     * smaller sourceLimit adjustment logic.
     */
    എങ്കിൽ(U_SUCCESS(*pErrorCode) && source<(sourceLimit=(uint8_t *)pToUArgs->sourceLimit)) {
        utf8->toUnicodeStatus=utf8->toUBytes[0]=b=*source++;
        utf8->toULength=1;
        utf8->mode=U8_എണ്ണം_BYTES(b);
    }

    /* write back the updated pointers */
    pToUArgs->source=(അക്ഷരം *)source;
    pFromUArgs->target=(അക്ഷരം *)target;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
_Latin1GetUnicodeSet(സ്ഥിരം UConverter *cnv,
                     സ്ഥിരം USetAdder *sa,
                     UConverterUnicodeSet which,
                     UErrorCode *pErrorCode) {
    (ശൂന്യം)cnv;
    (ശൂന്യം)which;
    (ശൂന്യം)pErrorCode;
    sa->addRange(sa->set, 0, 0xff);
}
U_CDECL_END


സ്ഥിതം സ്ഥിരം UConverterImpl _Latin1Impl={
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

സ്ഥിതം സ്ഥിരം UConverterStaticData _Latin1StaticData={
    വലുപ്പം(UConverterStaticData),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, അസത്യം, അസത്യം,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

സ്ഥിരം UConverterSharedData _Latin1Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_Latin1StaticData, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
സ്ഥിതം ശൂന്യം U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    സ്ഥിരം uint8_t *source, *sourceLimit;
    char16_t *target, *oldTarget;
    int32_t targetCapacity, length;
    int32_t *offsets;

    int32_t sourceIndex;

    uint8_t c;

    /* set up the local pointers */
    source=(സ്ഥിരം uint8_t *)pArgs->source;
    sourceLimit=(സ്ഥിരം uint8_t *)pArgs->sourceLimit;
    target=oldTarget=pArgs->target;
    targetCapacity=(int32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    sourceIndex=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(sourceLimit-source);
    എങ്കിൽ(length<targetCapacity) {
        targetCapacity=length;
    }

    എങ്കിൽ(targetCapacity>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=targetCapacity>>3;
        ചെയ്യുക {
            oredChars=target[0]=source[0];
            oredChars|=target[1]=source[1];
            oredChars|=target[2]=source[2];
            oredChars|=target[3]=source[3];
            oredChars|=target[4]=source[4];
            oredChars|=target[5]=source[5];
            oredChars|=target[6]=source[6];
            oredChars|=target[7]=source[7];

            /* were all 16 entries really valid? */
            എങ്കിൽ(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                നിർത്തുക;
            }
            source+=8;
            target+=8;
        } ആകുമ്പോൾ(--count>0);
        count=loops-count;
        targetCapacity-=count*8;

        എങ്കിൽ(offsets!=nullptr) {
            oldTarget+=count*8;
            ആകുമ്പോൾ(count>0) {
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
    ആകുമ്പോൾ(targetCapacity>0 && (c=*source++)<=0x7f) {
        *target++=c;
        --targetCapacity;
    }

    എങ്കിൽ(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=pArgs->converter;
        cnv->toUBytes[0]=c;
        cnv->toULength=1;
        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
    } അല്ലെങ്കിൽ എങ്കിൽ(source<sourceLimit && target>=pArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* set offsets since the start */
    എങ്കിൽ(offsets!=nullptr) {
        size_t count=target-oldTarget;
        ആകുമ്പോൾ(count>0) {
            *offsets++=sourceIndex++;
            --count;
        }
    }

    /* write back the updated pointers */
    pArgs->source=(സ്ഥിരം അക്ഷരം *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
സ്ഥിതം UChar32 U_CALLCONV
_ASCIIGetNextUChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    സ്ഥിരം uint8_t *source;
    uint8_t b;

    source=(സ്ഥിരം uint8_t *)pArgs->source;
    എങ്കിൽ(source<(സ്ഥിരം uint8_t *)pArgs->sourceLimit) {
        b=*source++;
        pArgs->source=(സ്ഥിരം അക്ഷരം *)source;
        എങ്കിൽ(b<=0x7f) {
            തിരികെ b;
        } അല്ലെങ്കിൽ {
            UConverter *cnv=pArgs->converter;
            cnv->toUBytes[0]=b;
            cnv->toULength=1;
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            തിരികെ 0xffff;
        }
    }

    /* no output because of empty input */
    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    തിരികെ 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
സ്ഥിതം ശൂന്യം U_CALLCONV
ucnv_ASCIIFromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                   UConverterToUnicodeArgs *pToUArgs,
                   UErrorCode *pErrorCode) {
    സ്ഥിരം uint8_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity, length;

    uint8_t c;

    എങ്കിൽ(pToUArgs->converter->toULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        തിരികെ;
    }

    /* set up the local pointers */
    source=(സ്ഥിരം uint8_t *)pToUArgs->source;
    sourceLimit=(സ്ഥിരം uint8_t *)pToUArgs->sourceLimit;
    target=(uint8_t *)pFromUArgs->target;
    targetCapacity=(int32_t)(pFromUArgs->targetLimit-pFromUArgs->target);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(sourceLimit-source);
    എങ്കിൽ(length<targetCapacity) {
        targetCapacity=length;
    }

    /* unroll the loop with the most common case */
    എങ്കിൽ(targetCapacity>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=targetCapacity>>4;
        ചെയ്യുക {
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
            എങ്കിൽ(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                നിർത്തുക;
            }
        } ആകുമ്പോൾ(--count>0);
        count=loops-count;
        targetCapacity-=16*count;
    }

    /* conversion loop */
    c=0;
    ആകുമ്പോൾ(targetCapacity>0 && (c=*source)<=0x7f) {
        ++source;
        *target++=c;
        --targetCapacity;
    }

    എങ്കിൽ(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *pErrorCode=U_USING_DEFAULT_WARNING;
    } അല്ലെങ്കിൽ എങ്കിൽ(source<sourceLimit && target>=(സ്ഥിരം uint8_t *)pFromUArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pToUArgs->source=(സ്ഥിരം അക്ഷരം *)source;
    pFromUArgs->target=(അക്ഷരം *)target;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
_ASCIIGetUnicodeSet(സ്ഥിരം UConverter *cnv,
                    സ്ഥിരം USetAdder *sa,
                    UConverterUnicodeSet which,
                    UErrorCode *pErrorCode) {
    (ശൂന്യം)cnv;
    (ശൂന്യം)which;
    (ശൂന്യം)pErrorCode;
    sa->addRange(sa->set, 0, 0x7f);
}
U_CDECL_END

സ്ഥിതം സ്ഥിരം UConverterImpl _ASCIIImpl={
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

സ്ഥിതം സ്ഥിരം UConverterStaticData _ASCIIStaticData={
    വലുപ്പം(UConverterStaticData),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, അസത്യം, അസത്യം,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

സ്ഥിരം UConverterSharedData _ASCIIData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_ASCIIStaticData, &_ASCIIImpl);

#endif
