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

#जर !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "ustr_imp.h"

/* control optimizations according to the platform */
#define LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
_Latin1ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                            UErrorCode *pErrorCode) {
    स्थिरांक uint8_t *source;
    char16_t *target;
    int32_t targetCapacity, length;
    int32_t *offsets;

    int32_t sourceIndex;

    /* set up the local pointers */
    source=(स्थिरांक uint8_t *)pArgs->source;
    target=pArgs->target;
    targetCapacity=(int32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;

    sourceIndex=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((स्थिरांक uint8_t *)pArgs->sourceLimit-source);
    जर(length<=targetCapacity) {
        targetCapacity=length;
    } अन्यथा {
        /* target will be full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        length=targetCapacity;
    }

    जर(targetCapacity>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=targetCapacity>>3;
        length=targetCapacity&=0x7;
        करा {
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
        } पर्यंत(--count>0);

        जर(offsets!=nullptr) {
            करा {
                offsets[0]=sourceIndex++;
                offsets[1]=sourceIndex++;
                offsets[2]=sourceIndex++;
                offsets[3]=sourceIndex++;
                offsets[4]=sourceIndex++;
                offsets[5]=sourceIndex++;
                offsets[6]=sourceIndex++;
                offsets[7]=sourceIndex++;
                offsets+=8;
            } पर्यंत(--loops>0);
        }
    }

    /* conversion loop */
    पर्यंत(targetCapacity>0) {
        *target++=*source++;
        --targetCapacity;
    }

    /* write back the updated pointers */
    pArgs->source=(स्थिरांक अक्षर *)source;
    pArgs->target=target;

    /* set offsets */
    जर(offsets!=nullptr) {
        पर्यंत(length>0) {
            *offsets++=sourceIndex++;
            --length;
        }
        pArgs->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
स्थिर UChar32 U_CALLCONV
_Latin1GetNextUChar(UConverterToUnicodeArgs *pArgs,
                    UErrorCode *pErrorCode) {
    स्थिरांक uint8_t *source=(स्थिरांक uint8_t *)pArgs->source;
    जर(source<(स्थिरांक uint8_t *)pArgs->sourceLimit) {
        pArgs->source=(स्थिरांक अक्षर *)(source+1);
        परत *source;
    }

    /* no output because of empty input */
    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    परत 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
स्थिर शून्य U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                              UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिरांक char16_t *source, *sourceLimit;
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

    जर(cnv->sharedData==&_Latin1Data) {
        max=0xff; /* Latin-1 */
    } अन्यथा {
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
    जर(length<targetCapacity) {
        targetCapacity=length;
    }

    /* conversion loop */
    जर(cp!=0 && targetCapacity>0) {
        जा getTrail;
    }

#जर LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    जर(targetCapacity>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=targetCapacity>>4;
        करा {
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
            जर(oredChars>max) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                थांबा;
            }
        } पर्यंत(--count>0);
        count=loops-count;
        targetCapacity-=16*count;

        जर(offsets!=nullptr) {
            oldTarget+=16*count;
            पर्यंत(count>0) {
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
    पर्यंत(targetCapacity>0 && (c=*source++)<=max) {
        /* convert the Unicode code point */
        *target++=(uint8_t)c;
        --targetCapacity;
    }

    जर(c>max) {
        cp=c;
        जर(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } अन्यथा जर(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            जर(source<sourceLimit) {
                /* test the following code unit */
                char16_t trail=*source;
                जर(U16_IS_TRAIL(trail)) {
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
                जा no_More_आदान;
            }
        } अन्यथा {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *pErrorCode= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_FOUND : U_INVALID_CHAR_FOUND;
        cnv->fromUChar32=cp;
    }
no_More_आदान:

    /* set offsets since the start */
    जर(offsets!=nullptr) {
        माप_प्रकार count=target-oldTarget;
        पर्यंत(count>0) {
            *offsets++=sourceIndex++;
            --count;
        }
    }

    जर(U_SUCCESS(*pErrorCode) && source<sourceLimit && target>=(uint8_t *)pArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target=(अक्षर *)target;
    pArgs->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
स्थिर शून्य U_CALLCONV
ucnv_Latin1FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                    UConverterToUnicodeArgs *pToUArgs,
                    UErrorCode *pErrorCode) {
    UConverter *utf8;
    स्थिरांक uint8_t *source, *sourceLimit;
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
    जर (utf8->toULength > 0) {
        c=(UChar32)utf8->toUnicodeStatus;
    } अन्यथा {
        c = 0;
    }
    जर(c!=0 && source<sourceLimit) {
        जर(targetCapacity==0) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            परत;
        } अन्यथा जर(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*source-0x80)) <= 0x3f) {
            ++source;
            *target++=(uint8_t)(((c&3)<<6)|t1);
            --targetCapacity;

            utf8->toUnicodeStatus=0;
            utf8->toULength=0;
        } अन्यथा {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *pErrorCode=U_USING_DEFAULT_WARNING;
            परत;
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
    जर(source<sourceLimit && U8_IS_LEAD(*(sourceLimit-1))) {
        --sourceLimit;
    }

    /* conversion loop */
    पर्यंत(source<sourceLimit) {
        जर(targetCapacity>0) {
            b=*source++;
            जर(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *target++ = b;
                --targetCapacity;
            } अन्यथा जर( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*source-0x80)) <= 0x3f
            ) {
                ++source;
                *target++=(uint8_t)(((b&3)<<6)|t1);
                --targetCapacity;
            } अन्यथा {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                pToUArgs->source=(अक्षर *)(source-1);
                pFromUArgs->target=(अक्षर *)target;
                *pErrorCode=U_USING_DEFAULT_WARNING;
                परत;
            }
        } अन्यथा {
            /* target is full */
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            थांबा;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     * For Latin-1, there is at most exactly one lead byte because of the
     * smaller sourceLimit adjustment logic.
     */
    जर(U_SUCCESS(*pErrorCode) && source<(sourceLimit=(uint8_t *)pToUArgs->sourceLimit)) {
        utf8->toUnicodeStatus=utf8->toUBytes[0]=b=*source++;
        utf8->toULength=1;
        utf8->mode=U8_मोजणी_BYTES(b);
    }

    /* write back the updated pointers */
    pToUArgs->source=(अक्षर *)source;
    pFromUArgs->target=(अक्षर *)target;
}

स्थिर शून्य U_CALLCONV
_Latin1GetUnicodeSet(स्थिरांक UConverter *cnv,
                     स्थिरांक USetAdder *sa,
                     UConverterUnicodeSet which,
                     UErrorCode *pErrorCode) {
    (शून्य)cnv;
    (शून्य)which;
    (शून्य)pErrorCode;
    sa->addRange(sa->set, 0, 0xff);
}
U_CDECL_END


स्थिर स्थिरांक UConverterImpl _Latin1Impl={
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

स्थिर स्थिरांक UConverterStaticData _Latin1StaticData={
    आकार(UConverterStaticData),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

स्थिरांक UConverterSharedData _Latin1Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_Latin1StaticData, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
स्थिर शून्य U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    स्थिरांक uint8_t *source, *sourceLimit;
    char16_t *target, *oldTarget;
    int32_t targetCapacity, length;
    int32_t *offsets;

    int32_t sourceIndex;

    uint8_t c;

    /* set up the local pointers */
    source=(स्थिरांक uint8_t *)pArgs->source;
    sourceLimit=(स्थिरांक uint8_t *)pArgs->sourceLimit;
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
    जर(length<targetCapacity) {
        targetCapacity=length;
    }

    जर(targetCapacity>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=targetCapacity>>3;
        करा {
            oredChars=target[0]=source[0];
            oredChars|=target[1]=source[1];
            oredChars|=target[2]=source[2];
            oredChars|=target[3]=source[3];
            oredChars|=target[4]=source[4];
            oredChars|=target[5]=source[5];
            oredChars|=target[6]=source[6];
            oredChars|=target[7]=source[7];

            /* were all 16 entries really valid? */
            जर(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                थांबा;
            }
            source+=8;
            target+=8;
        } पर्यंत(--count>0);
        count=loops-count;
        targetCapacity-=count*8;

        जर(offsets!=nullptr) {
            oldTarget+=count*8;
            पर्यंत(count>0) {
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
    पर्यंत(targetCapacity>0 && (c=*source++)<=0x7f) {
        *target++=c;
        --targetCapacity;
    }

    जर(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=pArgs->converter;
        cnv->toUBytes[0]=c;
        cnv->toULength=1;
        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
    } अन्यथा जर(source<sourceLimit && target>=pArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* set offsets since the start */
    जर(offsets!=nullptr) {
        माप_प्रकार count=target-oldTarget;
        पर्यंत(count>0) {
            *offsets++=sourceIndex++;
            --count;
        }
    }

    /* write back the updated pointers */
    pArgs->source=(स्थिरांक अक्षर *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
स्थिर UChar32 U_CALLCONV
_ASCIIGetNextUChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    स्थिरांक uint8_t *source;
    uint8_t b;

    source=(स्थिरांक uint8_t *)pArgs->source;
    जर(source<(स्थिरांक uint8_t *)pArgs->sourceLimit) {
        b=*source++;
        pArgs->source=(स्थिरांक अक्षर *)source;
        जर(b<=0x7f) {
            परत b;
        } अन्यथा {
            UConverter *cnv=pArgs->converter;
            cnv->toUBytes[0]=b;
            cnv->toULength=1;
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            परत 0xffff;
        }
    }

    /* no output because of empty input */
    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    परत 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
स्थिर शून्य U_CALLCONV
ucnv_ASCIIFromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                   UConverterToUnicodeArgs *pToUArgs,
                   UErrorCode *pErrorCode) {
    स्थिरांक uint8_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity, length;

    uint8_t c;

    जर(pToUArgs->converter->toULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        परत;
    }

    /* set up the local pointers */
    source=(स्थिरांक uint8_t *)pToUArgs->source;
    sourceLimit=(स्थिरांक uint8_t *)pToUArgs->sourceLimit;
    target=(uint8_t *)pFromUArgs->target;
    targetCapacity=(int32_t)(pFromUArgs->targetLimit-pFromUArgs->target);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(sourceLimit-source);
    जर(length<targetCapacity) {
        targetCapacity=length;
    }

    /* unroll the loop with the most common case */
    जर(targetCapacity>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=targetCapacity>>4;
        करा {
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
            जर(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                थांबा;
            }
        } पर्यंत(--count>0);
        count=loops-count;
        targetCapacity-=16*count;
    }

    /* conversion loop */
    c=0;
    पर्यंत(targetCapacity>0 && (c=*source)<=0x7f) {
        ++source;
        *target++=c;
        --targetCapacity;
    }

    जर(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *pErrorCode=U_USING_DEFAULT_WARNING;
    } अन्यथा जर(source<sourceLimit && target>=(स्थिरांक uint8_t *)pFromUArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pToUArgs->source=(स्थिरांक अक्षर *)source;
    pFromUArgs->target=(अक्षर *)target;
}

स्थिर शून्य U_CALLCONV
_ASCIIGetUnicodeSet(स्थिरांक UConverter *cnv,
                    स्थिरांक USetAdder *sa,
                    UConverterUnicodeSet which,
                    UErrorCode *pErrorCode) {
    (शून्य)cnv;
    (शून्य)which;
    (शून्य)pErrorCode;
    sa->addRange(sa->set, 0, 0x7f);
}
U_CDECL_END

स्थिर स्थिरांक UConverterImpl _ASCIIImpl={
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

स्थिर स्थिरांक UConverterStaticData _ASCIIStaticData={
    आकार(UConverterStaticData),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

स्थिरांक UConverterSharedData _ASCIIData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_ASCIIStaticData, &_ASCIIImpl);

#endif
