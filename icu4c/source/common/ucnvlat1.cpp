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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश "unicode/ucnv.h"
#समावेश "unicode/uset.h"
#समावेश "unicode/utf8.h"
#समावेश "ucnv_bld.h"
#समावेश "ucnv_cnv.h"
#समावेश "ustr_imp.h"

/* control optimizations according to the platform */
#घोषणा LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
अटल व्योम U_CALLCONV
_Latin1ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                            UErrorCode *pErrorCode) {
    स्थिर uint8_t *source;
    char16_t *target;
    int32_t targetCapacity, length;
    int32_t *offsets;

    int32_t source_सूचकांक;

    /* set up the local pointers */
    source=(स्थिर uint8_t *)pArgs->source;
    target=pArgs->target;
    targetCapacity=(int32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;

    source_सूचकांक=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((स्थिर uint8_t *)pArgs->sourceLimit-source);
    अगर(length<=targetCapacity) {
        targetCapacity=length;
    } अन्यथा {
        /* target will be full */
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
        length=targetCapacity;
    }

    अगर(targetCapacity>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=targetCapacity>>3;
        length=targetCapacity&=0x7;
        करो {
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
        } जबतक(--count>0);

        अगर(offsets!=nullptr) {
            करो {
                offsets[0]=source_सूचकांक++;
                offsets[1]=source_सूचकांक++;
                offsets[2]=source_सूचकांक++;
                offsets[3]=source_सूचकांक++;
                offsets[4]=source_सूचकांक++;
                offsets[5]=source_सूचकांक++;
                offsets[6]=source_सूचकांक++;
                offsets[7]=source_सूचकांक++;
                offsets+=8;
            } जबतक(--loops>0);
        }
    }

    /* conversion loop */
    जबतक(targetCapacity>0) {
        *target++=*source++;
        --targetCapacity;
    }

    /* write back the updated pointers */
    pArgs->source=(स्थिर अक्षर *)source;
    pArgs->target=target;

    /* set offsets */
    अगर(offsets!=nullptr) {
        जबतक(length>0) {
            *offsets++=source_सूचकांक++;
            --length;
        }
        pArgs->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
अटल UChar32 U_CALLCONV
Latin1_Get_अग्र_UChar(UConverterToUnicodeArgs *pArgs,
                    UErrorCode *pErrorCode) {
    स्थिर uint8_t *source=(स्थिर uint8_t *)pArgs->source;
    अगर(source<(स्थिर uint8_t *)pArgs->sourceLimit) {
        pArgs->source=(स्थिर अक्षर *)(source+1);
        वापस *source;
    }

    /* no output because of empty input */
    *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
    वापस 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
अटल व्योम U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                              UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर char16_t *source, *sourceLimit;
    uint8_t *target, *oldTarget;
    int32_t targetCapacity, length;
    int32_t *offsets;

    UChar32 cp;
    char16_t c, max;

    int32_t source_सूचकांक;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source=pArgs->source;
    sourceLimit=pArgs->sourceLimit;
    target=oldTarget=(uint8_t *)pArgs->target;
    targetCapacity=(int32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;

    अगर(cnv->shared_तथ्य==&Latin1_तथ्य) {
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
    length=(int32_t)(sourceLimit-source);
    अगर(length<targetCapacity) {
        targetCapacity=length;
    }

    /* conversion loop */
    अगर(cp!=0 && targetCapacity>0) {
        जाओ getTrail;
    }

#अगर LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    अगर(targetCapacity>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=targetCapacity>>4;
        करो {
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
            अगर(oredChars>max) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                अवरोध;
            }
        } जबतक(--count>0);
        count=loops-count;
        targetCapacity-=16*count;

        अगर(offsets!=nullptr) {
            oldTarget+=16*count;
            जबतक(count>0) {
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
#पूर्ण_अगर

    /* conversion loop */
    c=0;
    जबतक(targetCapacity>0 && (c=*source++)<=max) {
        /* convert the Unicode code point */
        *target++=(uint8_t)c;
        --targetCapacity;
    }

    अगर(c>max) {
        cp=c;
        अगर(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } अन्यथा अगर(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            अगर(source<sourceLimit) {
                /* test the following code unit */
                char16_t trail=*source;
                अगर(U16_IS_TRAIL(trail)) {
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
                जाओ no_More_निवेश;
            }
        } अन्यथा {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *pErrorCode= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_FOUND : U_INVALID_CHAR_FOUND;
        cnv->fromUChar32=cp;
    }
no_More_निवेश:

    /* set offsets since the start */
    अगर(offsets!=nullptr) {
        माप_प्रकार count=target-oldTarget;
        जबतक(count>0) {
            *offsets++=source_सूचकांक++;
            --count;
        }
    }

    अगर(U_SUCCESS(*pErrorCode) && source<sourceLimit && target>=(uint8_t *)pArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target=(अक्षर *)target;
    pArgs->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
अटल व्योम U_CALLCONV
ucnv_Latin1FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                    UConverterToUnicodeArgs *pToUArgs,
                    UErrorCode *pErrorCode) {
    UConverter *utf8;
    स्थिर uint8_t *source, *sourceLimit;
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
    अगर (utf8->toULength > 0) {
        c=(UChar32)utf8->toUnicodeStatus;
    } अन्यथा {
        c = 0;
    }
    अगर(c!=0 && source<sourceLimit) {
        अगर(targetCapacity==0) {
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            वापस;
        } अन्यथा अगर(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*source-0x80)) <= 0x3f) {
            ++source;
            *target++=(uint8_t)(((c&3)<<6)|t1);
            --targetCapacity;

            utf8->toUnicodeStatus=0;
            utf8->toULength=0;
        } अन्यथा {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *pErrorCode=U_USING_DEFAULT_WARNING;
            वापस;
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
    अगर(source<sourceLimit && U8_IS_LEAD(*(sourceLimit-1))) {
        --sourceLimit;
    }

    /* conversion loop */
    जबतक(source<sourceLimit) {
        अगर(targetCapacity>0) {
            b=*source++;
            अगर(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *target++ = b;
                --targetCapacity;
            } अन्यथा अगर( /* handle U+0080..U+00FF inline */
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
                वापस;
            }
        } अन्यथा {
            /* target is full */
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     * For Latin-1, there is at most exactly one lead byte because of the
     * smaller sourceLimit adjustment logic.
     */
    अगर(U_SUCCESS(*pErrorCode) && source<(sourceLimit=(uint8_t *)pToUArgs->sourceLimit)) {
        utf8->toUnicodeStatus=utf8->toUBytes[0]=b=*source++;
        utf8->toULength=1;
        utf8->mode=U8_गिनती_BYTES(b);
    }

    /* write back the updated pointers */
    pToUArgs->source=(अक्षर *)source;
    pFromUArgs->target=(अक्षर *)target;
}

अटल व्योम U_CALLCONV
_Latin1GetUnicodeSet(स्थिर UConverter *cnv,
                     स्थिर USetAdder *sa,
                     UConverterUnicodeSet which,
                     UErrorCode *pErrorCode) {
    (व्योम)cnv;
    (व्योम)which;
    (व्योम)pErrorCode;
    sa->addRange(sa->set, 0, 0xff);
}
U_CDECL_END


अटल स्थिर UConverterImpl _Latin1Impl={
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
    Latin1_Get_अग्र_UChar,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    _Latin1GetUnicodeSet,

    nullptr,
    ucnv_Latin1FromUTF8
};

अटल स्थिर UConverter_Static_तथ्य Latin1_Static_तथ्य={
    माप(UConverter_Static_तथ्य),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, false, false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

स्थिर UConverter_Shared_तथ्य Latin1_तथ्य=
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&Latin1_Static_तथ्य, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
अटल व्योम U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    स्थिर uint8_t *source, *sourceLimit;
    char16_t *target, *oldTarget;
    int32_t targetCapacity, length;
    int32_t *offsets;

    int32_t source_सूचकांक;

    uint8_t c;

    /* set up the local pointers */
    source=(स्थिर uint8_t *)pArgs->source;
    sourceLimit=(स्थिर uint8_t *)pArgs->sourceLimit;
    target=oldTarget=pArgs->target;
    targetCapacity=(int32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(sourceLimit-source);
    अगर(length<targetCapacity) {
        targetCapacity=length;
    }

    अगर(targetCapacity>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=targetCapacity>>3;
        करो {
            oredChars=target[0]=source[0];
            oredChars|=target[1]=source[1];
            oredChars|=target[2]=source[2];
            oredChars|=target[3]=source[3];
            oredChars|=target[4]=source[4];
            oredChars|=target[5]=source[5];
            oredChars|=target[6]=source[6];
            oredChars|=target[7]=source[7];

            /* were all 16 entries really valid? */
            अगर(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                अवरोध;
            }
            source+=8;
            target+=8;
        } जबतक(--count>0);
        count=loops-count;
        targetCapacity-=count*8;

        अगर(offsets!=nullptr) {
            oldTarget+=count*8;
            जबतक(count>0) {
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
    जबतक(targetCapacity>0 && (c=*source++)<=0x7f) {
        *target++=c;
        --targetCapacity;
    }

    अगर(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=pArgs->converter;
        cnv->toUBytes[0]=c;
        cnv->toULength=1;
        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
    } अन्यथा अगर(source<sourceLimit && target>=pArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
    }

    /* set offsets since the start */
    अगर(offsets!=nullptr) {
        माप_प्रकार count=target-oldTarget;
        जबतक(count>0) {
            *offsets++=source_सूचकांक++;
            --count;
        }
    }

    /* write back the updated pointers */
    pArgs->source=(स्थिर अक्षर *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
अटल UChar32 U_CALLCONV
ASCIIGet_अग्र_UChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    स्थिर uint8_t *source;
    uint8_t b;

    source=(स्थिर uint8_t *)pArgs->source;
    अगर(source<(स्थिर uint8_t *)pArgs->sourceLimit) {
        b=*source++;
        pArgs->source=(स्थिर अक्षर *)source;
        अगर(b<=0x7f) {
            वापस b;
        } अन्यथा {
            UConverter *cnv=pArgs->converter;
            cnv->toUBytes[0]=b;
            cnv->toULength=1;
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            वापस 0xffff;
        }
    }

    /* no output because of empty input */
    *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
    वापस 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
अटल व्योम U_CALLCONV
ucnv_ASCIIFromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                   UConverterToUnicodeArgs *pToUArgs,
                   UErrorCode *pErrorCode) {
    स्थिर uint8_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity, length;

    uint8_t c;

    अगर(pToUArgs->converter->toULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        वापस;
    }

    /* set up the local pointers */
    source=(स्थिर uint8_t *)pToUArgs->source;
    sourceLimit=(स्थिर uint8_t *)pToUArgs->sourceLimit;
    target=(uint8_t *)pFromUArgs->target;
    targetCapacity=(int32_t)(pFromUArgs->targetLimit-pFromUArgs->target);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(sourceLimit-source);
    अगर(length<targetCapacity) {
        targetCapacity=length;
    }

    /* unroll the loop with the most common case */
    अगर(targetCapacity>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=targetCapacity>>4;
        करो {
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
            अगर(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                अवरोध;
            }
        } जबतक(--count>0);
        count=loops-count;
        targetCapacity-=16*count;
    }

    /* conversion loop */
    c=0;
    जबतक(targetCapacity>0 && (c=*source)<=0x7f) {
        ++source;
        *target++=c;
        --targetCapacity;
    }

    अगर(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *pErrorCode=U_USING_DEFAULT_WARNING;
    } अन्यथा अगर(source<sourceLimit && target>=(स्थिर uint8_t *)pFromUArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pToUArgs->source=(स्थिर अक्षर *)source;
    pFromUArgs->target=(अक्षर *)target;
}

अटल व्योम U_CALLCONV
_ASCIIGetUnicodeSet(स्थिर UConverter *cnv,
                    स्थिर USetAdder *sa,
                    UConverterUnicodeSet which,
                    UErrorCode *pErrorCode) {
    (व्योम)cnv;
    (व्योम)which;
    (व्योम)pErrorCode;
    sa->addRange(sa->set, 0, 0x7f);
}
U_CDECL_END

अटल स्थिर UConverterImpl _ASCIIImpl={
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
    ASCIIGet_अग्र_UChar,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    _ASCIIGetUnicodeSet,

    nullptr,
    ucnv_ASCIIFromUTF8
};

अटल स्थिर UConverter_Static_तथ्य ASCIIStatic_तथ्य={
    माप(UConverter_Static_तथ्य),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, false, false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

स्थिर UConverter_Shared_तथ्य _ASCIIData=
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&ASCIIStatic_तथ्य, &_ASCIIImpl);

#पूर्ण_अगर
