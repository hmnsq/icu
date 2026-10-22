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

#include "unicode/utypes.h"

#ако !UCONFIG_NO_CONVERSION

#include "unicode/ucnv.h"
#include "unicode/uset.h"
#include "unicode/utf8.h"
#include "ucnv_bld.h"
#include "ucnv_cnv.h"
#include "ustr_imp.h"

/* control optimizations according to the platform */
#define LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
static void U_CALLCONV
_Latin1ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                            UErrorCode *pErrorCode) {
    const uint8_t *source;
    char16_t *target;
    int32_t targetCapacity, length;
    int32_t *offsets;

    int32_t sourceIndex;

    /* set up the local pointers */
    source=(const uint8_t *)pArgs->source;
    target=pArgs->target;
    targetCapacity=(int32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;

    sourceIndex=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((const uint8_t *)pArgs->sourceLimit-source);
    ако(length<=targetCapacity) {
        targetCapacity=length;
    } иначе {
        /* target will be full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        length=targetCapacity;
    }

    ако(targetCapacity>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=targetCapacity>>3;
        length=targetCapacity&=0x7;
        do {
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
        } докато(--count>0);

        ако(offsets!=nullptr) {
            do {
                offsets[0]=sourceIndex++;
                offsets[1]=sourceIndex++;
                offsets[2]=sourceIndex++;
                offsets[3]=sourceIndex++;
                offsets[4]=sourceIndex++;
                offsets[5]=sourceIndex++;
                offsets[6]=sourceIndex++;
                offsets[7]=sourceIndex++;
                offsets+=8;
            } докато(--loops>0);
        }
    }

    /* conversion loop */
    докато(targetCapacity>0) {
        *target++=*source++;
        --targetCapacity;
    }

    /* write back the updated pointers */
    pArgs->source=(const char *)source;
    pArgs->target=target;

    /* set offsets */
    ако(offsets!=nullptr) {
        докато(length>0) {
            *offsets++=sourceIndex++;
            --length;
        }
        pArgs->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
static UChar32 U_CALLCONV
_Latin1GetNextUChar(UConverterToUnicodeArgs *pArgs,
                    UErrorCode *pErrorCode) {
    const uint8_t *source=(const uint8_t *)pArgs->source;
    ако(source<(const uint8_t *)pArgs->sourceLimit) {
        pArgs->source=(const char *)(source+1);
        върни *source;
    }

    /* no output because of empty input */
    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    върни 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
static void U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                              UErrorCode *pErrorCode) {
    UConverter *cnv;
    const char16_t *source, *sourceLimit;
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

    ако(cnv->sharedData==&_Latin1Data) {
        max=0xff; /* Latin-1 */
    } иначе {
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
    ако(length<targetCapacity) {
        targetCapacity=length;
    }

    /* conversion loop */
    ако(cp!=0 && targetCapacity>0) {
        goto getTrail;
    }

#ако LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    ако(targetCapacity>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=targetCapacity>>4;
        do {
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
            ако(oredChars>max) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                break;
            }
        } докато(--count>0);
        count=loops-count;
        targetCapacity-=16*count;

        ако(offsets!=nullptr) {
            oldTarget+=16*count;
            докато(count>0) {
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
    докато(targetCapacity>0 && (c=*source++)<=max) {
        /* convert the Unicode code point */
        *target++=(uint8_t)c;
        --targetCapacity;
    }

    ако(c>max) {
        cp=c;
        ако(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } иначе ако(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            ако(source<sourceLimit) {
                /* test the following code unit */
                char16_t trail=*source;
                ако(U16_IS_TRAIL(trail)) {
                    ++source;
                    cp=U16_GET_SUPPLEMENTARY(cp, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } иначе {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                }
            } иначе {
                /* no more input */
                cnv->fromUChar32=cp;
                goto noMoreInput;
            }
        } иначе {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *pErrorCode= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_FOUND : U_INVALID_CHAR_FOUND;
        cnv->fromUChar32=cp;
    }
noMoreInput:

    /* set offsets since the start */
    ако(offsets!=nullptr) {
        size_t count=target-oldTarget;
        докато(count>0) {
            *offsets++=sourceIndex++;
            --count;
        }
    }

    ако(U_SUCCESS(*pErrorCode) && source<sourceLimit && target>=(uint8_t *)pArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target=(char *)target;
    pArgs->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
static void U_CALLCONV
ucnv_Latin1FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                    UConverterToUnicodeArgs *pToUArgs,
                    UErrorCode *pErrorCode) {
    UConverter *utf8;
    const uint8_t *source, *sourceLimit;
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
    ако (utf8->toULength > 0) {
        c=(UChar32)utf8->toUnicodeStatus;
    } иначе {
        c = 0;
    }
    ако(c!=0 && source<sourceLimit) {
        ако(targetCapacity==0) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            върни;
        } иначе ако(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*source-0x80)) <= 0x3f) {
            ++source;
            *target++=(uint8_t)(((c&3)<<6)|t1);
            --targetCapacity;

            utf8->toUnicodeStatus=0;
            utf8->toULength=0;
        } иначе {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *pErrorCode=U_USING_DEFAULT_WARNING;
            върни;
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
    ако(source<sourceLimit && U8_IS_LEAD(*(sourceLimit-1))) {
        --sourceLimit;
    }

    /* conversion loop */
    докато(source<sourceLimit) {
        ако(targetCapacity>0) {
            b=*source++;
            ако(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *target++ = b;
                --targetCapacity;
            } иначе ако( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*source-0x80)) <= 0x3f
            ) {
                ++source;
                *target++=(uint8_t)(((b&3)<<6)|t1);
                --targetCapacity;
            } иначе {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                pToUArgs->source=(char *)(source-1);
                pFromUArgs->target=(char *)target;
                *pErrorCode=U_USING_DEFAULT_WARNING;
                върни;
            }
        } иначе {
            /* target is full */
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            break;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     * For Latin-1, there is at most exactly one lead byte because of the
     * smaller sourceLimit adjustment logic.
     */
    ако(U_SUCCESS(*pErrorCode) && source<(sourceLimit=(uint8_t *)pToUArgs->sourceLimit)) {
        utf8->toUnicodeStatus=utf8->toUBytes[0]=b=*source++;
        utf8->toULength=1;
        utf8->mode=U8_COUNT_BYTES(b);
    }

    /* write back the updated pointers */
    pToUArgs->source=(char *)source;
    pFromUArgs->target=(char *)target;
}

static void U_CALLCONV
_Latin1GetUnicodeSet(const UConverter *cnv,
                     const USetAdder *sa,
                     UConverterUnicodeSet which,
                     UErrorCode *pErrorCode) {
    (void)cnv;
    (void)which;
    (void)pErrorCode;
    sa->addRange(sa->set, 0, 0xff);
}
U_CDECL_END


static const UConverterImpl _Latin1Impl={
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

static const UConverterStaticData _Latin1StaticData={
    sizeof(UConverterStaticData),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, false, false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

const UConverterSharedData _Latin1Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_Latin1StaticData, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
static void U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    const uint8_t *source, *sourceLimit;
    char16_t *target, *oldTarget;
    int32_t targetCapacity, length;
    int32_t *offsets;

    int32_t sourceIndex;

    uint8_t c;

    /* set up the local pointers */
    source=(const uint8_t *)pArgs->source;
    sourceLimit=(const uint8_t *)pArgs->sourceLimit;
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
    ако(length<targetCapacity) {
        targetCapacity=length;
    }

    ако(targetCapacity>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=targetCapacity>>3;
        do {
            oredChars=target[0]=source[0];
            oredChars|=target[1]=source[1];
            oredChars|=target[2]=source[2];
            oredChars|=target[3]=source[3];
            oredChars|=target[4]=source[4];
            oredChars|=target[5]=source[5];
            oredChars|=target[6]=source[6];
            oredChars|=target[7]=source[7];

            /* were all 16 entries really valid? */
            ако(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                break;
            }
            source+=8;
            target+=8;
        } докато(--count>0);
        count=loops-count;
        targetCapacity-=count*8;

        ако(offsets!=nullptr) {
            oldTarget+=count*8;
            докато(count>0) {
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
    докато(targetCapacity>0 && (c=*source++)<=0x7f) {
        *target++=c;
        --targetCapacity;
    }

    ако(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=pArgs->converter;
        cnv->toUBytes[0]=c;
        cnv->toULength=1;
        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
    } иначе ако(source<sourceLimit && target>=pArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* set offsets since the start */
    ако(offsets!=nullptr) {
        size_t count=target-oldTarget;
        докато(count>0) {
            *offsets++=sourceIndex++;
            --count;
        }
    }

    /* write back the updated pointers */
    pArgs->source=(const char *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
static UChar32 U_CALLCONV
_ASCIIGetNextUChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    const uint8_t *source;
    uint8_t b;

    source=(const uint8_t *)pArgs->source;
    ако(source<(const uint8_t *)pArgs->sourceLimit) {
        b=*source++;
        pArgs->source=(const char *)source;
        ако(b<=0x7f) {
            върни b;
        } иначе {
            UConverter *cnv=pArgs->converter;
            cnv->toUBytes[0]=b;
            cnv->toULength=1;
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            върни 0xffff;
        }
    }

    /* no output because of empty input */
    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    върни 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
static void U_CALLCONV
ucnv_ASCIIFromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                   UConverterToUnicodeArgs *pToUArgs,
                   UErrorCode *pErrorCode) {
    const uint8_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity, length;

    uint8_t c;

    ако(pToUArgs->converter->toULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        върни;
    }

    /* set up the local pointers */
    source=(const uint8_t *)pToUArgs->source;
    sourceLimit=(const uint8_t *)pToUArgs->sourceLimit;
    target=(uint8_t *)pFromUArgs->target;
    targetCapacity=(int32_t)(pFromUArgs->targetLimit-pFromUArgs->target);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(sourceLimit-source);
    ако(length<targetCapacity) {
        targetCapacity=length;
    }

    /* unroll the loop with the most common case */
    ако(targetCapacity>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=targetCapacity>>4;
        do {
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
            ако(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                break;
            }
        } докато(--count>0);
        count=loops-count;
        targetCapacity-=16*count;
    }

    /* conversion loop */
    c=0;
    докато(targetCapacity>0 && (c=*source)<=0x7f) {
        ++source;
        *target++=c;
        --targetCapacity;
    }

    ако(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *pErrorCode=U_USING_DEFAULT_WARNING;
    } иначе ако(source<sourceLimit && target>=(const uint8_t *)pFromUArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pToUArgs->source=(const char *)source;
    pFromUArgs->target=(char *)target;
}

static void U_CALLCONV
_ASCIIGetUnicodeSet(const UConverter *cnv,
                    const USetAdder *sa,
                    UConverterUnicodeSet which,
                    UErrorCode *pErrorCode) {
    (void)cnv;
    (void)which;
    (void)pErrorCode;
    sa->addRange(sa->set, 0, 0x7f);
}
U_CDECL_END

static const UConverterImpl _ASCIIImpl={
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

static const UConverterStaticData _ASCIIStaticData={
    sizeof(UConverterStaticData),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, false, false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

const UConverterSharedData _ASCIIData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_ASCIIStaticData, &_ASCIIImpl);

#endif
