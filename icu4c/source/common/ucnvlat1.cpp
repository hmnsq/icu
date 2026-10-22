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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "unicode/uset.h"
#অন্তর্ভাব "unicode/utf8.h"
#অন্তর্ভাব "ucnv_bld.h"
#অন্তর্ভাব "ucnv_cnv.h"
#অন্তর্ভাব "ustr_imp.h"

/* control optimizations according to the platform */
#define LATIN1_UNROLL_FROM_UNICODE 1

/* ISO 8859-1 --------------------------------------------------------------- */

/* This is a table-less and callback-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
U_CDECL_BEGIN
স্থির শূন্য U_CALLCONV
_Latin1ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                            UErrorCode *pErrorCode) {
    ধ্রুবক uint8_t *source;
    char16_t *target;
    int32_t targetCapacity, length;
    int32_t *offsets;

    int32_t source_সূচক;

    /* set up the local pointers */
    source=(ধ্রুবক uint8_t *)pArgs->source;
    target=pArgs->target;
    targetCapacity=(int32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;

    source_সূচক=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)((ধ্রুবক uint8_t *)pArgs->sourceLimit-source);
    যদি(length<=targetCapacity) {
        targetCapacity=length;
    } অন্যথা {
        /* target will be full */
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
        length=targetCapacity;
    }

    যদি(targetCapacity>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;

        loops=count=targetCapacity>>3;
        length=targetCapacity&=0x7;
        করো {
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
        } যখন(--count>0);

        যদি(offsets!=nullptr) {
            করো {
                offsets[0]=source_সূচক++;
                offsets[1]=source_সূচক++;
                offsets[2]=source_সূচক++;
                offsets[3]=source_সূচক++;
                offsets[4]=source_সূচক++;
                offsets[5]=source_সূচক++;
                offsets[6]=source_সূচক++;
                offsets[7]=source_সূচক++;
                offsets+=8;
            } যখন(--loops>0);
        }
    }

    /* conversion loop */
    যখন(targetCapacity>0) {
        *target++=*source++;
        --targetCapacity;
    }

    /* write back the updated pointers */
    pArgs->source=(ধ্রুবক অক্ষর *)source;
    pArgs->target=target;

    /* set offsets */
    যদি(offsets!=nullptr) {
        যখন(length>0) {
            *offsets++=source_সূচক++;
            --length;
        }
        pArgs->offsets=offsets;
    }
}

/* This is a table-less and callback-less version of ucnv_MBCSSingleGetNextUChar(). */
স্থির UChar32 U_CALLCONV
Latin1_Get_পরবর্তী_UChar(UConverterToUnicodeArgs *pArgs,
                    UErrorCode *pErrorCode) {
    ধ্রুবক uint8_t *source=(ধ্রুবক uint8_t *)pArgs->source;
    যদি(source<(ধ্রুবক uint8_t *)pArgs->sourceLimit) {
        pArgs->source=(ধ্রুবক অক্ষর *)(source+1);
        ফেরত *source;
    }

    /* no output because of empty input */
    *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
    ফেরত 0xffff;
}

/* This is a table-less version of ucnv_MBCSSingleFromBMPWithOffsets(). */
স্থির শূন্য U_CALLCONV
_Latin1FromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                              UErrorCode *pErrorCode) {
    UConverter *cnv;
    ধ্রুবক char16_t *source, *sourceLimit;
    uint8_t *target, *oldTarget;
    int32_t targetCapacity, length;
    int32_t *offsets;

    UChar32 cp;
    char16_t c, max;

    int32_t source_সূচক;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source=pArgs->source;
    sourceLimit=pArgs->sourceLimit;
    target=oldTarget=(uint8_t *)pArgs->target;
    targetCapacity=(int32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;

    যদি(cnv->shared_তথ্য==&Latin1_তথ্য) {
        max=0xff; /* Latin-1 */
    } অন্যথা {
        max=0x7f; /* US-ASCII */
    }

    /* get the converter state from UConverter */
    cp=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_সূচক= cp==0 ? 0 : -1;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(sourceLimit-source);
    যদি(length<targetCapacity) {
        targetCapacity=length;
    }

    /* conversion loop */
    যদি(cp!=0 && targetCapacity>0) {
        যাও getTrail;
    }

#যদি LATIN1_UNROLL_FROM_UNICODE
    /* unroll the loop with the most common case */
    যদি(targetCapacity>=16) {
        int32_t count, loops;
        char16_t u, oredChars;

        loops=count=targetCapacity>>4;
        করো {
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
            যদি(oredChars>max) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                বিরতি;
            }
        } যখন(--count>0);
        count=loops-count;
        targetCapacity-=16*count;

        যদি(offsets!=nullptr) {
            oldTarget+=16*count;
            যখন(count>0) {
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                *offsets++=source_সূচক++;
                --count;
            }
        }
    }
#endif

    /* conversion loop */
    c=0;
    যখন(targetCapacity>0 && (c=*source++)<=max) {
        /* convert the Unicode code point */
        *target++=(uint8_t)c;
        --targetCapacity;
    }

    যদি(c>max) {
        cp=c;
        যদি(!U_IS_SURROGATE(cp)) {
            /* callback(unassigned) */
        } অন্যথা যদি(U_IS_SURROGATE_LEAD(cp)) {
getTrail:
            যদি(source<sourceLimit) {
                /* test the following code unit */
                char16_t trail=*source;
                যদি(U16_IS_TRAIL(trail)) {
                    ++source;
                    cp=U16_GET_SUPPLEMENTARY(cp, trail);
                    /* this codepage does not map supplementary code points */
                    /* callback(unassigned) */
                } অন্যথা {
                    /* this is an unmatched lead code unit (1st surrogate) */
                    /* callback(illegal) */
                }
            } অন্যথা {
                /* no more input */
                cnv->fromUChar32=cp;
                যাও no_More_প্রবেশ;
            }
        } অন্যথা {
            /* this is an unmatched trail code unit (2nd surrogate) */
            /* callback(illegal) */
        }

        *pErrorCode= U_IS_SURROGATE(cp) ? U_ILLEGAL_CHAR_FOUND : U_INVALID_CHAR_FOUND;
        cnv->fromUChar32=cp;
    }
no_More_প্রবেশ:

    /* set offsets since the start */
    যদি(offsets!=nullptr) {
        size_t count=target-oldTarget;
        যখন(count>0) {
            *offsets++=source_সূচক++;
            --count;
        }
    }

    যদি(U_SUCCESS(*pErrorCode) && source<sourceLimit && target>=(uint8_t *)pArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target=(অক্ষর *)target;
    pArgs->offsets=offsets;
}

/* Convert UTF-8 to Latin-1. Adapted from ucnv_SBCSFromUTF8(). */
স্থির শূন্য U_CALLCONV
ucnv_Latin1FromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                    UConverterToUnicodeArgs *pToUArgs,
                    UErrorCode *pErrorCode) {
    UConverter *utf8;
    ধ্রুবক uint8_t *source, *sourceLimit;
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
    যদি (utf8->toULength > 0) {
        c=(UChar32)utf8->toUnicodeStatus;
    } অন্যথা {
        c = 0;
    }
    যদি(c!=0 && source<sourceLimit) {
        যদি(targetCapacity==0) {
            *pErrorCode=U_বাফার_OVERFLOW_ERROR;
            ফেরত;
        } অন্যথা যদি(c>=0xc2 && c<=0xc3 && (t1=(uint8_t)(*source-0x80)) <= 0x3f) {
            ++source;
            *target++=(uint8_t)(((c&3)<<6)|t1);
            --targetCapacity;

            utf8->toUnicodeStatus=0;
            utf8->toULength=0;
        } অন্যথা {
            /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
            *pErrorCode=U_USING_DEFAULT_WARNING;
            ফেরত;
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
    যদি(source<sourceLimit && U8_IS_LEAD(*(sourceLimit-1))) {
        --sourceLimit;
    }

    /* conversion loop */
    যখন(source<sourceLimit) {
        যদি(targetCapacity>0) {
            b=*source++;
            যদি(U8_IS_SINGLE(b)) {
                /* convert ASCII */
                *target++ = b;
                --targetCapacity;
            } অন্যথা যদি( /* handle U+0080..U+00FF inline */
                       b>=0xc2 && b<=0xc3 &&
                       (t1=(uint8_t)(*source-0x80)) <= 0x3f
            ) {
                ++source;
                *target++=(uint8_t)(((b&3)<<6)|t1);
                --targetCapacity;
            } অন্যথা {
                /* complicated, illegal or unmappable input: fall back to the pivoting implementation */
                pToUArgs->source=(অক্ষর *)(source-1);
                pFromUArgs->target=(অক্ষর *)target;
                *pErrorCode=U_USING_DEFAULT_WARNING;
                ফেরত;
            }
        } অন্যথা {
            /* target is full */
            *pErrorCode=U_বাফার_OVERFLOW_ERROR;
            বিরতি;
        }
    }

    /*
     * The sourceLimit may have been adjusted before the conversion loop
     * to stop before a truncated sequence.
     * If so, then collect the truncated sequence now.
     * For Latin-1, there is at most exactly one lead byte because of the
     * smaller sourceLimit adjustment logic.
     */
    যদি(U_SUCCESS(*pErrorCode) && source<(sourceLimit=(uint8_t *)pToUArgs->sourceLimit)) {
        utf8->toUnicodeStatus=utf8->toUBytes[0]=b=*source++;
        utf8->toULength=1;
        utf8->mode=U8_গুনতি_BYTES(b);
    }

    /* write back the updated pointers */
    pToUArgs->source=(অক্ষর *)source;
    pFromUArgs->target=(অক্ষর *)target;
}

স্থির শূন্য U_CALLCONV
_Latin1GetUnicodeSet(ধ্রুবক UConverter *cnv,
                     ধ্রুবক USetAdder *sa,
                     UConverterUnicodeSet which,
                     UErrorCode *pErrorCode) {
    (শূন্য)cnv;
    (শূন্য)which;
    (শূন্য)pErrorCode;
    sa->addRange(sa->set, 0, 0xff);
}
U_CDECL_END


স্থির ধ্রুবক UConverterImpl _Latin1Impl={
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
    Latin1_Get_পরবর্তী_UChar,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    _Latin1GetUnicodeSet,

    nullptr,
    ucnv_Latin1FromUTF8
};

স্থির ধ্রুবক UConverter_Static_তথ্য Latin1_Static_তথ্য={
    আকার(UConverter_Static_তথ্য),
    "ISO-8859-1",
    819, UCNV_IBM, UCNV_LATIN_1, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, মিথ্যা, মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ধ্রুবক UConverter_Shared_তথ্য Latin1_তথ্য=
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&Latin1_Static_তথ্য, &_Latin1Impl);

/* US-ASCII ----------------------------------------------------------------- */

U_CDECL_BEGIN
/* This is a table-less version of ucnv_MBCSSingleToBMPWithOffsets(). */
স্থির শূন্য U_CALLCONV
_ASCIIToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    ধ্রুবক uint8_t *source, *sourceLimit;
    char16_t *target, *oldTarget;
    int32_t targetCapacity, length;
    int32_t *offsets;

    int32_t source_সূচক;

    uint8_t c;

    /* set up the local pointers */
    source=(ধ্রুবক uint8_t *)pArgs->source;
    sourceLimit=(ধ্রুবক uint8_t *)pArgs->sourceLimit;
    target=oldTarget=pArgs->target;
    targetCapacity=(int32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_সূচক=0;

    /*
     * since the conversion here is 1:1 char16_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(sourceLimit-source);
    যদি(length<targetCapacity) {
        targetCapacity=length;
    }

    যদি(targetCapacity>=8) {
        /* This loop is unrolled for speed and improved pipelining. */
        int32_t count, loops;
        char16_t oredChars;

        loops=count=targetCapacity>>3;
        করো {
            oredChars=target[0]=source[0];
            oredChars|=target[1]=source[1];
            oredChars|=target[2]=source[2];
            oredChars|=target[3]=source[3];
            oredChars|=target[4]=source[4];
            oredChars|=target[5]=source[5];
            oredChars|=target[6]=source[6];
            oredChars|=target[7]=source[7];

            /* were all 16 entries really valid? */
            যদি(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                বিরতি;
            }
            source+=8;
            target+=8;
        } যখন(--count>0);
        count=loops-count;
        targetCapacity-=count*8;

        যদি(offsets!=nullptr) {
            oldTarget+=count*8;
            যখন(count>0) {
                offsets[0]=source_সূচক++;
                offsets[1]=source_সূচক++;
                offsets[2]=source_সূচক++;
                offsets[3]=source_সূচক++;
                offsets[4]=source_সূচক++;
                offsets[5]=source_সূচক++;
                offsets[6]=source_সূচক++;
                offsets[7]=source_সূচক++;
                offsets+=8;
                --count;
            }
        }
    }

    /* conversion loop */
    c=0;
    যখন(targetCapacity>0 && (c=*source++)<=0x7f) {
        *target++=c;
        --targetCapacity;
    }

    যদি(c>0x7f) {
        /* callback(illegal); copy the current bytes to toUBytes[] */
        UConverter *cnv=pArgs->converter;
        cnv->toUBytes[0]=c;
        cnv->toULength=1;
        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
    } অন্যথা যদি(source<sourceLimit && target>=pArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
    }

    /* set offsets since the start */
    যদি(offsets!=nullptr) {
        size_t count=target-oldTarget;
        যখন(count>0) {
            *offsets++=source_সূচক++;
            --count;
        }
    }

    /* write back the updated pointers */
    pArgs->source=(ধ্রুবক অক্ষর *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

/* This is a table-less version of ucnv_MBCSSingleGetNextUChar(). */
স্থির UChar32 U_CALLCONV
ASCIIGet_পরবর্তী_UChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    ধ্রুবক uint8_t *source;
    uint8_t b;

    source=(ধ্রুবক uint8_t *)pArgs->source;
    যদি(source<(ধ্রুবক uint8_t *)pArgs->sourceLimit) {
        b=*source++;
        pArgs->source=(ধ্রুবক অক্ষর *)source;
        যদি(b<=0x7f) {
            ফেরত b;
        } অন্যথা {
            UConverter *cnv=pArgs->converter;
            cnv->toUBytes[0]=b;
            cnv->toULength=1;
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
            ফেরত 0xffff;
        }
    }

    /* no output because of empty input */
    *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
    ফেরত 0xffff;
}

/* "Convert" UTF-8 to US-ASCII: Validate and copy. */
স্থির শূন্য U_CALLCONV
ucnv_ASCIIFromUTF8(UConverterFromUnicodeArgs *pFromUArgs,
                   UConverterToUnicodeArgs *pToUArgs,
                   UErrorCode *pErrorCode) {
    ধ্রুবক uint8_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity, length;

    uint8_t c;

    যদি(pToUArgs->converter->toULength > 0) {
        /* no handling of partial UTF-8 characters here, fall back to pivoting */
        *pErrorCode=U_USING_DEFAULT_WARNING;
        ফেরত;
    }

    /* set up the local pointers */
    source=(ধ্রুবক uint8_t *)pToUArgs->source;
    sourceLimit=(ধ্রুবক uint8_t *)pToUArgs->sourceLimit;
    target=(uint8_t *)pFromUArgs->target;
    targetCapacity=(int32_t)(pFromUArgs->targetLimit-pFromUArgs->target);

    /*
     * since the conversion here is 1:1 uint8_t:uint8_t, we need only one counter
     * for the minimum of the sourceLength and targetCapacity
     */
    length=(int32_t)(sourceLimit-source);
    যদি(length<targetCapacity) {
        targetCapacity=length;
    }

    /* unroll the loop with the most common case */
    যদি(targetCapacity>=16) {
        int32_t count, loops;
        uint8_t oredChars;

        loops=count=targetCapacity>>4;
        করো {
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
            যদি(oredChars>0x7f) {
                /* no, return to the first of these 16 */
                source-=16;
                target-=16;
                বিরতি;
            }
        } যখন(--count>0);
        count=loops-count;
        targetCapacity-=16*count;
    }

    /* conversion loop */
    c=0;
    যখন(targetCapacity>0 && (c=*source)<=0x7f) {
        ++source;
        *target++=c;
        --targetCapacity;
    }

    যদি(c>0x7f) {
        /* non-ASCII character, handle in standard converter */
        *pErrorCode=U_USING_DEFAULT_WARNING;
    } অন্যথা যদি(source<sourceLimit && target>=(ধ্রুবক uint8_t *)pFromUArgs->targetLimit) {
        /* target is full */
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pToUArgs->source=(ধ্রুবক অক্ষর *)source;
    pFromUArgs->target=(অক্ষর *)target;
}

স্থির শূন্য U_CALLCONV
_ASCIIGetUnicodeSet(ধ্রুবক UConverter *cnv,
                    ধ্রুবক USetAdder *sa,
                    UConverterUnicodeSet which,
                    UErrorCode *pErrorCode) {
    (শূন্য)cnv;
    (শূন্য)which;
    (শূন্য)pErrorCode;
    sa->addRange(sa->set, 0, 0x7f);
}
U_CDECL_END

স্থির ধ্রুবক UConverterImpl _ASCIIImpl={
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
    ASCIIGet_পরবর্তী_UChar,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    _ASCIIGetUnicodeSet,

    nullptr,
    ucnv_ASCIIFromUTF8
};

স্থির ধ্রুবক UConverter_Static_তথ্য ASCIIStatic_তথ্য={
    আকার(UConverter_Static_তথ্য),
    "US-ASCII",
    367, UCNV_IBM, UCNV_US_ASCII, 1, 1,
    { 0x1a, 0, 0, 0 }, 1, মিথ্যা, মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ধ্রুবক UConverter_Shared_তথ্য _ASCIIData=
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&ASCIIStatic_তথ্য, &_ASCIIImpl);

#endif
