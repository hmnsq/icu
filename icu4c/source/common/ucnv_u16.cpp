// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*  
**********************************************************************
*   Copyright (C) 2002-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ucnv_u16.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002jul01
*   created by: Markus W. Scherer
*
*   UTF-16 converter implementation. Used to be in ucnv_utf.c.
*/

#ضمن "unicode/utypes.h"

#𐤀𐤌 !UCONFIG_NO_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "unicode/uversion.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"
#ضمن "cmemory.h"

تعداد {
    UCNV_NEED_TO_WRITE_BOM=1
};

U_CDECL_BEGIN
/*
 * The UTF-16 toUnicode implementation is also used for the Java-specific
 * "with BOM" variants of UTF-16BE and UTF-16LE.
 */
ساكن فراغ  U_CALLCONV
_UTF16ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode);

/* UTF-16BE ----------------------------------------------------------------- */

#𐤀𐤌 U_IS_BIG_ENDIAN
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16BEFromUnicodeWithOffsets
#وإلا
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16LEFromUnicodeWithOffsets
#endif


ساكن فراغ  U_CALLCONV
_UTF16BEFromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                               UErrorCode *pErrorCode) {
    UConverter *cnv;
    ثابت char16_t *source;
    حرف *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, sourceIndex;
    char16_t c, trail;
    حرف overflow[4];

    source=pArgs->source;
    length=(int32_t)(pArgs->sourceLimit-source);
    𐤀𐤌(length<=0) {
        /* no input, nothing to do */
        أرجع;
    }

    cnv=pArgs->converter;

    /* write the BOM if necessary */
    𐤀𐤌(cnv->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xfeu, (حرف)0xffu };
        ucnv_fromUWriteBytes(cnv,
                             bom, 2,
                             &pArgs->target, pArgs->targetLimit,
                             &pArgs->offsets, -1,
                             pErrorCode);
        cnv->fromUnicodeStatus=0;
    }

    target=pArgs->target;
    𐤀𐤌(target >= pArgs->targetLimit) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        أرجع;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-target);
    offsets=pArgs->offsets;
    sourceIndex=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    𐤀𐤌((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*source) && targetCapacity>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++source;
        --length;
        target[0]=(uint8_t)(c>>8);
        target[1]=(uint8_t)c;
        target[2]=(uint8_t)(trail>>8);
        target[3]=(uint8_t)trail;
        target+=4;
        targetCapacity-=4;
        𐤀𐤌(offsets!=nullptr) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        sourceIndex=1;
        cnv->fromUChar32=c=0;
    }

    𐤀𐤌(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        𐤀𐤌(count>targetCapacity) {
            count=targetCapacity&~1;
        }
        /* count is even */
        targetCapacity-=count;
        count>>=1;
        length-=count;

        𐤀𐤌(offsets==nullptr) {
            طالما(count>0) {
                c=*source++;
                𐤀𐤌(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target+=2;
                } وإلا 𐤀𐤌(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target[2]=(uint8_t)(trail>>8);
                    target[3]=(uint8_t)trail;
                    target+=4;
                } وإلا {
                    اكسر;
                }
                --count;
            }
        } وإلا {
            طالما(count>0) {
                c=*source++;
                𐤀𐤌(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target+=2;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex++;
                } وإلا 𐤀𐤌(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target[2]=(uint8_t)(trail>>8);
                    target[3]=(uint8_t)trail;
                    target+=4;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex;
                    sourceIndex+=2;
                } وإلا {
                    اكسر;
                }
                --count;
            }
        }

        𐤀𐤌(count==0) {
            /* done with the loop for complete UChars */
            𐤀𐤌(length>0 && targetCapacity>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                𐤀𐤌(U16_IS_SINGLE(c=*source++)) {
                    overflow[0]=(حرف)(c>>8);
                    overflow[1]=(حرف)c;
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } وإلا {
                length=0;
                c=0;
            }
        } وإلا {
            /* keep c for surrogate handling, length will be set there */
            targetCapacity+=2*count;
        }
    } وإلا {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    𐤀𐤌(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        𐤀𐤌(U16_IS_SURROGATE_LEAD(c)) {
            𐤀𐤌(source<pArgs->sourceLimit) {
                𐤀𐤌(U16_IS_TRAIL(trail=*source)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++source;
                    overflow[0]=(حرف)(c>>8);
                    overflow[1]=(حرف)c;
                    overflow[2]=(حرف)(trail>>8);
                    overflow[3]=(حرف)trail;
                    length=4; /* 4 bytes to output */
                    c=0;
                } وإلا {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } وإلا {
                /* see if the trail surrogate is in the next buffer */
            }
        } وإلا {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
        cnv->fromUChar32=c;
    }

    𐤀𐤌(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_fromUWriteBytes(cnv,
                             overflow, length,
                             &target, pArgs->targetLimit,
                             &offsets, sourceIndex,
                             pErrorCode);
        targetCapacity = static_cast<uint32_t>(pArgs->targetLimit - target);
    }

    𐤀𐤌(U_SUCCESS(*pErrorCode) && source<pArgs->sourceLimit && targetCapacity==0) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target = target;
    pArgs->offsets=offsets;
}

ساكن فراغ  U_CALLCONV
_UTF16BEToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                             UErrorCode *pErrorCode) {
    UConverter *cnv;
    ثابت uint8_t *source;
    char16_t *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, count, sourceIndex;
    char16_t c, trail;

    𐤀𐤌(pArgs->converter->mode<8) {
        _UTF16ToUnicodeWithOffsets(pArgs, pErrorCode);
        أرجع;
    }

    cnv=pArgs->converter;
    source=(ثابت uint8_t *)pArgs->source;
    length=(int32_t)((ثابت uint8_t *)pArgs->sourceLimit-source);
    𐤀𐤌(length<=0 && cnv->toUnicodeStatus==0) {
        /* no input, nothing to do */
        أرجع;
    }

    target=pArgs->target;
    𐤀𐤌(target >= pArgs->targetLimit) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        أرجع;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-target);
    offsets=pArgs->offsets;
    sourceIndex=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    𐤀𐤌(cnv->toUnicodeStatus!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->toUBytes[0]=(uint8_t)cnv->toUnicodeStatus;
        cnv->toULength=1;
        cnv->toUnicodeStatus=0;
    }
    𐤀𐤌((count=cnv->toULength)!=0) {
        uint8_t *p=cnv->toUBytes;
        𐤐𐤏𐤋 {
            p[count++]=*source++;
            ++sourceIndex;
            --length;
            𐤀𐤌(count==2) {
                c=((char16_t)p[0]<<8)|p[1];
                𐤀𐤌(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *target++=c;
                    𐤀𐤌(offsets!=nullptr) {
                        *offsets++=-1;
                    }
                    --targetCapacity;
                    count=0;
                    c=0;
                    اكسر;
                } وإلا 𐤀𐤌(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } وإلا {
                    /* fall through to error handling for an unmatched trail surrogate */
                    اكسر;
                }
            } وإلا 𐤀𐤌(count==4) {
                c=((char16_t)p[0]<<8)|p[1];
                trail=((char16_t)p[2]<<8)|p[3];
                𐤀𐤌(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *target++=c;
                    𐤀𐤌(targetCapacity>=2) {
                        *target++=trail;
                        𐤀𐤌(offsets!=nullptr) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        targetCapacity-=2;
                    } وإلا /* targetCapacity==1 */ {
                        targetCapacity=0;
                        cnv->UCharErrorBuffer[0]=trail;
                        cnv->UCharErrorBufferLength=1;
                        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                    }
                    count=0;
                    c=0;
                    اكسر;
                } وإلا {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;

                    /* back out reading the code unit after it */
                    𐤀𐤌(((ثابت uint8_t *)pArgs->source-source)>=2) {
                        source-=2;
                    } وإلا {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->toUnicodeStatus=0x100|p[2];
                        --source;
                    }
                    cnv->toULength=2;

                    /* write back the updated pointers */
                    pArgs->source=(ثابت حرف *)source;
                    pArgs->target=target;
                    pArgs->offsets=offsets;
                    أرجع;
                }
            }
        } طالما(length>0);
        cnv->toULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*targetCapacity;
    𐤀𐤌(count>length) {
        count=length&~1;
    }
    𐤀𐤌(c==0 && count>0) {
        length-=count;
        count>>=1;
        targetCapacity-=count;
        𐤀𐤌(offsets==nullptr) {
            𐤐𐤏𐤋 {
                c=((char16_t)source[0]<<8)|source[1];
                source+=2;
                𐤀𐤌(U16_IS_SINGLE(c)) {
                    *target++=c;
                } وإلا 𐤀𐤌(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                } وإلا {
                    اكسر;
                }
            } طالما(--count>0);
        } وإلا {
            𐤐𐤏𐤋 {
                c=((char16_t)source[0]<<8)|source[1];
                source+=2;
                𐤀𐤌(U16_IS_SINGLE(c)) {
                    *target++=c;
                    *offsets++=sourceIndex;
                    sourceIndex+=2;
                } وإلا 𐤀𐤌(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex;
                    sourceIndex+=4;
                } وإلا {
                    اكسر;
                }
            } طالما(--count>0);
        }

        𐤀𐤌(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } وإلا {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            targetCapacity+=count;
        }
    }

    𐤀𐤌(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->toUBytes[0]=(uint8_t)(c>>8);
        cnv->toUBytes[1]=(uint8_t)c;
        cnv->toULength=2;

        𐤀𐤌(U16_IS_SURROGATE_LEAD(c)) {
            𐤀𐤌(length>=2) {
                𐤀𐤌(U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    source+=2;
                    length-=2;
                    *target++=c;
                    𐤀𐤌(offsets!=nullptr) {
                        *offsets++=sourceIndex;
                    }
                    cnv->UCharErrorBuffer[0]=trail;
                    cnv->UCharErrorBufferLength=1;
                    cnv->toULength=0;
                    *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                } وإلا {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } وإلا {
                /* see if the trail surrogate is in the next buffer */
            }
        } وإلا {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
    }

    𐤀𐤌(U_SUCCESS(*pErrorCode)) {
        /* check for a remaining source byte */
        𐤀𐤌(length>0) {
            𐤀𐤌(targetCapacity==0) {
                *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            } وإلا {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->toUBytes[cnv->toULength++]=*source++;
            }
        }
    }

    /* write back the updated pointers */
    pArgs->source=(ثابت حرف *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

ساكن UChar32  U_CALLCONV
_UTF16BEGetNextUChar(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    ثابت uint8_t *s, *sourceLimit;
    UChar32 c;

    𐤀𐤌(pArgs->converter->mode<8) {
        أرجع UCNV_GET_NEXT_UCHAR_USE_TO_U;
    }

    s=(ثابت uint8_t *)pArgs->source;
    sourceLimit=(ثابت uint8_t *)pArgs->sourceLimit;

    𐤀𐤌(s>=sourceLimit) {
        /* no input */
        *err=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    𐤀𐤌(s+2>sourceLimit) {
        /* only one byte: truncated char16_t */
        pArgs->converter->toUBytes[0]=*s++;
        pArgs->converter->toULength=1;
        pArgs->source=(ثابت حرف *)s;
        *err = U_TRUNCATED_CHAR_FOUND;
        أرجع 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)*s<<8)|s[1];
    s+=2;

    /* check for a surrogate pair */
    𐤀𐤌(U_IS_SURROGATE(c)) {
        𐤀𐤌(U16_IS_SURROGATE_LEAD(c)) {
            𐤀𐤌(s+2<=sourceLimit) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)*s<<8)|s[1];
                𐤀𐤌(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } وإلا {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } وإلا {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *bytes=pArgs->converter->toUBytes;
                s-=2;
                pArgs->converter->toULength=(int8_t)(sourceLimit-s);
                𐤐𐤏𐤋 {
                    *bytes++=*s++;
                } طالما(s<sourceLimit);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_FOUND;
            }
        } وإلا {
            /* unmatched trail surrogate */
            c=-2;
        }

        𐤀𐤌(c<0) {
            /* write the unmatched surrogate */
            uint8_t *bytes=pArgs->converter->toUBytes;
            pArgs->converter->toULength=2;
            *bytes=*(s-2);
            bytes[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_FOUND;
        }
    }

    pArgs->source=(ثابت حرف *)s;
    أرجع c;
} 

ساكن فراغ  U_CALLCONV
_UTF16BEReset(UConverter *cnv, UConverterResetChoice choice) {
    𐤀𐤌(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode state */
        𐤀𐤌(UCNV_GET_VERSION(cnv)==0) {
            cnv->mode=8; /* no BOM handling */
        } وإلا {
            cnv->mode=0; /* Java-specific "UnicodeBig" requires BE BOM or no BOM */
        }
    }
    𐤀𐤌(choice!=UCNV_RESET_TO_UNICODE && UCNV_GET_VERSION(cnv)==1) {
        /* reset fromUnicode for "UnicodeBig": prepare to output the UTF-16BE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

ساكن فراغ  U_CALLCONV
_UTF16BEOpen(UConverter *cnv,
             UConverterLoadArgs *pArgs,
             UErrorCode *pErrorCode) {
    (فراغ)pArgs;
    𐤀𐤌(UCNV_GET_VERSION(cnv)<=1) {
        _UTF16BEReset(cnv, UCNV_RESET_BOTH);
    } وإلا {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

ساكن ثابت حرف *  U_CALLCONV
_UTF16BEGetName(ثابت UConverter *cnv) {
    𐤀𐤌(UCNV_GET_VERSION(cnv)==0) {
        أرجع "UTF-16BE";
    } وإلا {
        أرجع "UTF-16BE,version=1";
    }
}
U_CDECL_END

ساكن ثابت UConverterImpl _UTF16BEImpl={
    UCNV_UTF16_BigEndian,

    nullptr,
    nullptr,

    _UTF16BEOpen,
    nullptr,
    _UTF16BEReset,

    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEGetNextUChar,

    nullptr,
    _UTF16BEGetName,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

ساكن ثابت UConverterStaticData _UTF16BEStaticData={
    حجم(UConverterStaticData),
    "UTF-16BE",
    1200, UCNV_IBM, UCNV_UTF16_BigEndian, 2, 2,
    { 0xff, 0xfd, 0, 0 },2,خطأ,خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverterSharedData _UTF16BEData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF16BEStaticData, &_UTF16BEImpl);

/* UTF-16LE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV
_UTF16LEFromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                               UErrorCode *pErrorCode) {
    UConverter *cnv;
    ثابت char16_t *source;
    حرف *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, sourceIndex;
    char16_t c, trail;
    حرف overflow[4];

    source=pArgs->source;
    length=(int32_t)(pArgs->sourceLimit-source);
    𐤀𐤌(length<=0) {
        /* no input, nothing to do */
        أرجع;
    }

    cnv=pArgs->converter;

    /* write the BOM if necessary */
    𐤀𐤌(cnv->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu };
        ucnv_fromUWriteBytes(cnv,
                             bom, 2,
                             &pArgs->target, pArgs->targetLimit,
                             &pArgs->offsets, -1,
                             pErrorCode);
        cnv->fromUnicodeStatus=0;
    }

    target=pArgs->target;
    𐤀𐤌(target >= pArgs->targetLimit) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        أرجع;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;
    sourceIndex=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    𐤀𐤌((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*source) && targetCapacity>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++source;
        --length;
        target[0]=(uint8_t)c;
        target[1]=(uint8_t)(c>>8);
        target[2]=(uint8_t)trail;
        target[3]=(uint8_t)(trail>>8);
        target+=4;
        targetCapacity-=4;
        𐤀𐤌(offsets!=nullptr) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        sourceIndex=1;
        cnv->fromUChar32=c=0;
    }

    𐤀𐤌(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        𐤀𐤌(count>targetCapacity) {
            count=targetCapacity&~1;
        }
        /* count is even */
        targetCapacity-=count;
        count>>=1;
        length-=count;

        𐤀𐤌(offsets==nullptr) {
            طالما(count>0) {
                c=*source++;
                𐤀𐤌(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target+=2;
                } وإلا 𐤀𐤌(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target[2]=(uint8_t)trail;
                    target[3]=(uint8_t)(trail>>8);
                    target+=4;
                } وإلا {
                    اكسر;
                }
                --count;
            }
        } وإلا {
            طالما(count>0) {
                c=*source++;
                𐤀𐤌(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target+=2;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex++;
                } وإلا 𐤀𐤌(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target[2]=(uint8_t)trail;
                    target[3]=(uint8_t)(trail>>8);
                    target+=4;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex;
                    sourceIndex+=2;
                } وإلا {
                    اكسر;
                }
                --count;
            }
        }

        𐤀𐤌(count==0) {
            /* done with the loop for complete UChars */
            𐤀𐤌(length>0 && targetCapacity>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                𐤀𐤌(U16_IS_SINGLE(c=*source++)) {
                    overflow[0]=(حرف)c;
                    overflow[1]=(حرف)(c>>8);
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } وإلا {
                length=0;
                c=0;
            }
        } وإلا {
            /* keep c for surrogate handling, length will be set there */
            targetCapacity+=2*count;
        }
    } وإلا {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    𐤀𐤌(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        𐤀𐤌(U16_IS_SURROGATE_LEAD(c)) {
            𐤀𐤌(source<pArgs->sourceLimit) {
                𐤀𐤌(U16_IS_TRAIL(trail=*source)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++source;
                    overflow[0]=(حرف)c;
                    overflow[1]=(حرف)(c>>8);
                    overflow[2]=(حرف)trail;
                    overflow[3]=(حرف)(trail>>8);
                    length=4; /* 4 bytes to output */
                    c=0;
                } وإلا {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } وإلا {
                /* see if the trail surrogate is in the next buffer */
            }
        } وإلا {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
        cnv->fromUChar32=c;
    }

    𐤀𐤌(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_fromUWriteBytes(cnv,
                             overflow, length,
                             &target, pArgs->targetLimit,
                             &offsets, sourceIndex,
                             pErrorCode);
        targetCapacity = static_cast<uint32_t>(pArgs->targetLimit - target);
    }

    𐤀𐤌(U_SUCCESS(*pErrorCode) && source<pArgs->sourceLimit && targetCapacity==0) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

ساكن فراغ  U_CALLCONV
_UTF16LEToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                             UErrorCode *pErrorCode) {
    UConverter *cnv;
    ثابت uint8_t *source;
    char16_t *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, count, sourceIndex;
    char16_t c, trail;

    𐤀𐤌(pArgs->converter->mode<8) {
        _UTF16ToUnicodeWithOffsets(pArgs, pErrorCode);
        أرجع;
    }

    cnv=pArgs->converter;
    source=(ثابت uint8_t *)pArgs->source;
    length=(int32_t)((ثابت uint8_t *)pArgs->sourceLimit-source);
    𐤀𐤌(length<=0 && cnv->toUnicodeStatus==0) {
        /* no input, nothing to do */
        أرجع;
    }

    target=pArgs->target;
    𐤀𐤌(target >= pArgs->targetLimit) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        أرجع;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;
    sourceIndex=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    𐤀𐤌(cnv->toUnicodeStatus!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->toUBytes[0]=(uint8_t)cnv->toUnicodeStatus;
        cnv->toULength=1;
        cnv->toUnicodeStatus=0;
    }
    𐤀𐤌((count=cnv->toULength)!=0) {
        uint8_t *p=cnv->toUBytes;
        𐤐𐤏𐤋 {
            p[count++]=*source++;
            ++sourceIndex;
            --length;
            𐤀𐤌(count==2) {
                c=((char16_t)p[1]<<8)|p[0];
                𐤀𐤌(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *target++=c;
                    𐤀𐤌(offsets!=nullptr) {
                        *offsets++=-1;
                    }
                    --targetCapacity;
                    count=0;
                    c=0;
                    اكسر;
                } وإلا 𐤀𐤌(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } وإلا {
                    /* fall through to error handling for an unmatched trail surrogate */
                    اكسر;
                }
            } وإلا 𐤀𐤌(count==4) {
                c=((char16_t)p[1]<<8)|p[0];
                trail=((char16_t)p[3]<<8)|p[2];
                𐤀𐤌(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *target++=c;
                    𐤀𐤌(targetCapacity>=2) {
                        *target++=trail;
                        𐤀𐤌(offsets!=nullptr) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        targetCapacity-=2;
                    } وإلا /* targetCapacity==1 */ {
                        targetCapacity=0;
                        cnv->UCharErrorBuffer[0]=trail;
                        cnv->UCharErrorBufferLength=1;
                        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                    }
                    count=0;
                    c=0;
                    اكسر;
                } وإلا {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;

                    /* back out reading the code unit after it */
                    𐤀𐤌(((ثابت uint8_t *)pArgs->source-source)>=2) {
                        source-=2;
                    } وإلا {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->toUnicodeStatus=0x100|p[2];
                        --source;
                    }
                    cnv->toULength=2;

                    /* write back the updated pointers */
                    pArgs->source=(ثابت حرف *)source;
                    pArgs->target=target;
                    pArgs->offsets=offsets;
                    أرجع;
                }
            }
        } طالما(length>0);
        cnv->toULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*targetCapacity;
    𐤀𐤌(count>length) {
        count=length&~1;
    }
    𐤀𐤌(c==0 && count>0) {
        length-=count;
        count>>=1;
        targetCapacity-=count;
        𐤀𐤌(offsets==nullptr) {
            𐤐𐤏𐤋 {
                c=((char16_t)source[1]<<8)|source[0];
                source+=2;
                𐤀𐤌(U16_IS_SINGLE(c)) {
                    *target++=c;
                } وإلا 𐤀𐤌(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                } وإلا {
                    اكسر;
                }
            } طالما(--count>0);
        } وإلا {
            𐤐𐤏𐤋 {
                c=((char16_t)source[1]<<8)|source[0];
                source+=2;
                𐤀𐤌(U16_IS_SINGLE(c)) {
                    *target++=c;
                    *offsets++=sourceIndex;
                    sourceIndex+=2;
                } وإلا 𐤀𐤌(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex;
                    sourceIndex+=4;
                } وإلا {
                    اكسر;
                }
            } طالما(--count>0);
        }

        𐤀𐤌(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } وإلا {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            targetCapacity+=count;
        }
    }

    𐤀𐤌(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->toUBytes[0]=(uint8_t)c;
        cnv->toUBytes[1]=(uint8_t)(c>>8);
        cnv->toULength=2;

        𐤀𐤌(U16_IS_SURROGATE_LEAD(c)) {
            𐤀𐤌(length>=2) {
                𐤀𐤌(U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    source+=2;
                    length-=2;
                    *target++=c;
                    𐤀𐤌(offsets!=nullptr) {
                        *offsets++=sourceIndex;
                    }
                    cnv->UCharErrorBuffer[0]=trail;
                    cnv->UCharErrorBufferLength=1;
                    cnv->toULength=0;
                    *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                } وإلا {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } وإلا {
                /* see if the trail surrogate is in the next buffer */
            }
        } وإلا {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
    }

    𐤀𐤌(U_SUCCESS(*pErrorCode)) {
        /* check for a remaining source byte */
        𐤀𐤌(length>0) {
            𐤀𐤌(targetCapacity==0) {
                *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            } وإلا {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->toUBytes[cnv->toULength++]=*source++;
            }
        }
    }

    /* write back the updated pointers */
    pArgs->source=(ثابت حرف *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

ساكن UChar32  U_CALLCONV
_UTF16LEGetNextUChar(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    ثابت uint8_t *s, *sourceLimit;
    UChar32 c;

    𐤀𐤌(pArgs->converter->mode<8) {
        أرجع UCNV_GET_NEXT_UCHAR_USE_TO_U;
    }

    s=(ثابت uint8_t *)pArgs->source;
    sourceLimit=(ثابت uint8_t *)pArgs->sourceLimit;

    𐤀𐤌(s>=sourceLimit) {
        /* no input */
        *err=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    𐤀𐤌(s+2>sourceLimit) {
        /* only one byte: truncated char16_t */
        pArgs->converter->toUBytes[0]=*s++;
        pArgs->converter->toULength=1;
        pArgs->source=(ثابت حرف *)s;
        *err = U_TRUNCATED_CHAR_FOUND;
        أرجع 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)s[1]<<8)|*s;
    s+=2;

    /* check for a surrogate pair */
    𐤀𐤌(U_IS_SURROGATE(c)) {
        𐤀𐤌(U16_IS_SURROGATE_LEAD(c)) {
            𐤀𐤌(s+2<=sourceLimit) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)s[1]<<8)|*s;
                𐤀𐤌(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } وإلا {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } وإلا {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *bytes=pArgs->converter->toUBytes;
                s-=2;
                pArgs->converter->toULength=(int8_t)(sourceLimit-s);
                𐤐𐤏𐤋 {
                    *bytes++=*s++;
                } طالما(s<sourceLimit);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_FOUND;
            }
        } وإلا {
            /* unmatched trail surrogate */
            c=-2;
        }

        𐤀𐤌(c<0) {
            /* write the unmatched surrogate */
            uint8_t *bytes=pArgs->converter->toUBytes;
            pArgs->converter->toULength=2;
            *bytes=*(s-2);
            bytes[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_FOUND;
        }
    }

    pArgs->source=(ثابت حرف *)s;
    أرجع c;
} 

ساكن فراغ  U_CALLCONV
_UTF16LEReset(UConverter *cnv, UConverterResetChoice choice) {
    𐤀𐤌(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode state */
        𐤀𐤌(UCNV_GET_VERSION(cnv)==0) {
            cnv->mode=8; /* no BOM handling */
        } وإلا {
            cnv->mode=0; /* Java-specific "UnicodeLittle" requires LE BOM or no BOM */
        }
    }
    𐤀𐤌(choice!=UCNV_RESET_TO_UNICODE && UCNV_GET_VERSION(cnv)==1) {
        /* reset fromUnicode for "UnicodeLittle": prepare to output the UTF-16LE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

ساكن فراغ  U_CALLCONV
_UTF16LEOpen(UConverter *cnv,
             UConverterLoadArgs *pArgs,
             UErrorCode *pErrorCode) {
    (فراغ)pArgs;
    𐤀𐤌(UCNV_GET_VERSION(cnv)<=1) {
        _UTF16LEReset(cnv, UCNV_RESET_BOTH);
    } وإلا {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

ساكن ثابت حرف *  U_CALLCONV
_UTF16LEGetName(ثابت UConverter *cnv) {
    𐤀𐤌(UCNV_GET_VERSION(cnv)==0) {
        أرجع "UTF-16LE";
    } وإلا {
        أرجع "UTF-16LE,version=1";
    }
}
U_CDECL_END

ساكن ثابت UConverterImpl _UTF16LEImpl={
    UCNV_UTF16_LittleEndian,

    nullptr,
    nullptr,

    _UTF16LEOpen,
    nullptr,
    _UTF16LEReset,

    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    _UTF16LEGetNextUChar,

    nullptr,
    _UTF16LEGetName,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};


ساكن ثابت UConverterStaticData _UTF16LEStaticData={
    حجم(UConverterStaticData),
    "UTF-16LE",
    1202, UCNV_IBM, UCNV_UTF16_LittleEndian, 2, 2,
    { 0xfd, 0xff, 0, 0 },2,خطأ,خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverterSharedData _UTF16LEData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF16LEStaticData, &_UTF16LEImpl);

/* UTF-16 (Detect BOM) ------------------------------------------------------ */

/*
 * Detect a BOM at the beginning of the stream and select UTF-16BE or UTF-16LE
 * accordingly.
 * This is a simpler version of the UTF-32 converter, with
 * fewer states for shorter BOMs.
 *
 * State values:
 * 0    initial state
 * 1    saw first byte
 * 2..5 -
 * 6..7 see _UTF16ToUnicodeWithOffsets() comments in state 1
 * 8    UTF-16BE mode
 * 9    UTF-16LE mode
 *
 * During detection: state==number of initial bytes seen so far.
 *
 * On output, emit U+FEFF as the first code point.
 *
 * Variants:
 * - UTF-16,version=1 (Java "Unicode" encoding) treats a missing BOM as an error.
 * - UTF-16BE,version=1 (Java "UnicodeBig" encoding) and
 *   UTF-16LE,version=1 (Java "UnicodeLittle" encoding) treat a reverse BOM as an error.
 */
U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV
_UTF16Reset(UConverter *cnv, UConverterResetChoice choice) {
    𐤀𐤌(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->mode=0;
    }
    𐤀𐤌(choice!=UCNV_RESET_TO_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-16PE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}
U_CDECL_END
خارجي ثابت UConverterSharedData _UTF16v2Data;
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
_UTF16Open(UConverter *cnv,
           UConverterLoadArgs *pArgs,
           UErrorCode *pErrorCode) {
    𐤀𐤌(UCNV_GET_VERSION(cnv)<=2) {
        𐤀𐤌(UCNV_GET_VERSION(cnv)==2 && !pArgs->onlyTestIsLoadable) {
            /*
             * Switch implementation, and switch the staticData that's different
             * and was copied into the UConverter.
             * (See ucnv_createConverterFromSharedData() in ucnv_bld.c.)
             * UTF-16,version=2 fromUnicode() always writes a big-endian byte stream.
             */
            cnv->sharedData=(UConverterSharedData*)&_UTF16v2Data;
            uprv_memcpy(cnv->subChars, _UTF16v2Data.staticData->subChar, UCNV_MAX_SUBCHAR_LEN);
        }
        _UTF16Reset(cnv, UCNV_RESET_BOTH);
    } وإلا {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

ساكن ثابت حرف *  U_CALLCONV
_UTF16GetName(ثابت UConverter *cnv) {
    𐤀𐤌(UCNV_GET_VERSION(cnv)==0) {
        أرجع "UTF-16";
    } وإلا 𐤀𐤌(UCNV_GET_VERSION(cnv)==1) {
        أرجع "UTF-16,version=1";
    } وإلا {
        أرجع "UTF-16,version=2";
    }
}
U_CDECL_END
خارجي ثابت UConverterSharedData _UTF16Data;

ساكن مضمن منطقي IS_UTF16BE(ثابت UConverter *cnv) {
    أرجع ((cnv)->sharedData == &_UTF16BEData);
}

ساكن مضمن منطقي IS_UTF16LE(ثابت UConverter *cnv) {
    أرجع ((cnv)->sharedData == &_UTF16LEData);
}

ساكن مضمن منطقي IS_UTF16(ثابت UConverter *cnv) {
    أرجع ((cnv)->sharedData==&_UTF16Data) || ((cnv)->sharedData == &_UTF16v2Data);
}

U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
_UTF16ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    ثابت حرف *source=pArgs->source;
    ثابت حرف *sourceLimit=pArgs->sourceLimit;
    int32_t *offsets=pArgs->offsets;

    int32_t state, offsetDelta;
    uint8_t b;

    state=cnv->mode;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    offsetDelta=0;

    طالما(source<sourceLimit && U_SUCCESS(*pErrorCode)) {
        بدل(state) {
        حالة 0:
            cnv->toUBytes[0]=(uint8_t)*source++;
            cnv->toULength=1;
            state=1;
            اكسر;
        حالة 1:
            /*
             * Only inside this switch case can the state variable
             * temporarily take two additional values:
             * 6: BOM error, continue with BE
             * 7: BOM error, continue with LE
             */
            b=*source;
            𐤀𐤌(cnv->toUBytes[0]==0xfe && b==0xff) {
                𐤀𐤌(IS_UTF16LE(cnv)) {
                    state=7; /* illegal reverse BOM for Java "UnicodeLittle" */
                } وإلا {
                    state=8; /* detect UTF-16BE */
                }
            } وإلا 𐤀𐤌(cnv->toUBytes[0]==0xff && b==0xfe) {
                𐤀𐤌(IS_UTF16BE(cnv)) {
                    state=6; /* illegal reverse BOM for Java "UnicodeBig" */
                } وإلا {
                    state=9; /* detect UTF-16LE */
                }
            } وإلا 𐤀𐤌((IS_UTF16(cnv) && UCNV_GET_VERSION(cnv)==1)) {
                state=6; /* illegal missing BOM for Java "Unicode" */
            }
            𐤀𐤌(state>=8) {
                /* BOM detected, consume it */
                ++source;
                cnv->toULength=0;
                offsetDelta=(int32_t)(source-pArgs->source);
            } وإلا 𐤀𐤌(state<6) {
                /* ok: no BOM, and not a reverse BOM */
                𐤀𐤌(source!=pArgs->source) {
                    /* reset the source for a correct first offset */
                    source=pArgs->source;
                    cnv->toULength=0;
                }
                𐤀𐤌(IS_UTF16LE(cnv)) {
                    /* Make Java "UnicodeLittle" default to LE. */
                    state=9;
                } وإلا {
                    /* Make standard UTF-16 and Java "UnicodeBig" default to BE. */
                    state=8;
                }
            } وإلا {
                /*
                 * error: missing BOM, or reverse BOM
                 * UTF-16,version=1: Java-specific "Unicode" requires a BOM.
                 * UTF-16BE,version=1: Java-specific "UnicodeBig" requires a BE BOM or no BOM.
                 * UTF-16LE,version=1: Java-specific "UnicodeLittle" requires an LE BOM or no BOM.
                 */
                /* report the non-BOM or reverse BOM as an illegal sequence */
                cnv->toUBytes[1]=b;
                cnv->toULength=2;
                pArgs->source=source+1;
                /* continue with conversion if the callback resets the error */
                /*
                 * Make Java "Unicode" default to BE like standard UTF-16.
                 * Make Java "UnicodeBig" and "UnicodeLittle" default
                 * to their normal endiannesses.
                 */
                cnv->mode=state+2;
                *pErrorCode=U_ILLEGAL_ESCAPE_SEQUENCE;
                أرجع;
            }
            /* convert the rest of the stream */
            cnv->mode=state;
            استمر;
        حالة 8:
            /* call UTF-16BE */
            pArgs->source=source;
            _UTF16BEToUnicodeWithOffsets(pArgs, pErrorCode);
            source=pArgs->source;
            اكسر;
        حالة 9:
            /* call UTF-16LE */
            pArgs->source=source;
            _UTF16LEToUnicodeWithOffsets(pArgs, pErrorCode);
            source=pArgs->source;
            اكسر;
        افتراضي:
            اكسر; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    𐤀𐤌(offsets!=nullptr && offsetDelta!=0) {
        int32_t *offsetsLimit=pArgs->offsets;
        طالما(offsets<offsetsLimit) {
            *offsets++ += offsetDelta;
        }
    }

    pArgs->source=source;

    𐤀𐤌(source==sourceLimit && pArgs->flush) {
        /* handle truncated input */
        بدل(state) {
        حالة 0:
            اكسر; /* no input at all, nothing to do */
        حالة 8:
            _UTF16BEToUnicodeWithOffsets(pArgs, pErrorCode);
            اكسر;
        حالة 9:
            _UTF16LEToUnicodeWithOffsets(pArgs, pErrorCode);
            اكسر;
        افتراضي:
            /* 0<state<8: framework will report truncation, nothing to do here */
            اكسر;
        }
    }

    cnv->mode=state;
}

ساكن UChar32 U_CALLCONV
_UTF16GetNextUChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    بدل(pArgs->converter->mode) {
    حالة 8:
        أرجع _UTF16BEGetNextUChar(pArgs, pErrorCode);
    حالة 9:
        أرجع _UTF16LEGetNextUChar(pArgs, pErrorCode);
    افتراضي:
        أرجع UCNV_GET_NEXT_UCHAR_USE_TO_U;
    }
}
U_CDECL_END

ساكن ثابت UConverterImpl _UTF16Impl = {
    UCNV_UTF16,

    nullptr,
    nullptr,

    _UTF16Open,
    nullptr,
    _UTF16Reset,

    _UTF16ToUnicodeWithOffsets,
    _UTF16ToUnicodeWithOffsets,
    _UTF16PEFromUnicodeWithOffsets,
    _UTF16PEFromUnicodeWithOffsets,
    _UTF16GetNextUChar,

    nullptr, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

ساكن ثابت UConverterStaticData _UTF16StaticData = {
    حجم(UConverterStaticData),
    "UTF-16",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
#𐤀𐤌 U_IS_BIG_ENDIAN
    { 0xff, 0xfd, 0, 0 }, 2,
#وإلا
    { 0xfd, 0xff, 0, 0 }, 2,
#endif
    خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverterSharedData _UTF16Data =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF16StaticData, &_UTF16Impl);

ساكن ثابت UConverterImpl _UTF16v2Impl = {
    UCNV_UTF16,

    nullptr,
    nullptr,

    _UTF16Open,
    nullptr,
    _UTF16Reset,

    _UTF16ToUnicodeWithOffsets,
    _UTF16ToUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16GetNextUChar,

    nullptr, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

ساكن ثابت UConverterStaticData _UTF16v2StaticData = {
    حجم(UConverterStaticData),
    "UTF-16,version=2",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
    { 0xff, 0xfd, 0, 0 }, 2,
    خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverterSharedData _UTF16v2Data =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF16v2StaticData, &_UTF16v2Impl);

#endif
