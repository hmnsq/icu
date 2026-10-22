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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/uversion.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

ಗಣನಾ {
    UCNV_NEED_TO_WRITE_BOM=1
};

U_CDECL_BEGIN
/*
 * The UTF-16 toUnicode implementation is also used for the Java-specific
 * "with BOM" variants of UTF-16BE and UTF-16LE.
 */
ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode);

/* UTF-16BE ----------------------------------------------------------------- */

#ಯದಿ U_IS_BIG_ENDIAN
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16BEFromUnicodeWithOffsets
#ಅನ್ಯಥಾ
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16LEFromUnicodeWithOffsets
#endif


ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16BEFromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                               UErrorCode *pErrorCode) {
    UConverter *cnv;
    ಧ್ರುವ char16_t *source;
    ವರ್ಣ *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, sourceIndex;
    char16_t c, trail;
    ವರ್ಣ overflow[4];

    source=pArgs->source;
    length=(int32_t)(pArgs->sourceLimit-source);
    ಯದಿ(length<=0) {
        /* no input, nothing to do */
        ಪ್ರತ್ಯಾಗಮ;
    }

    cnv=pArgs->converter;

    /* write the BOM if necessary */
    ಯದಿ(cnv->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ bom[]={ (ವರ್ಣ)0xfeu, (ವರ್ಣ)0xffu };
        ucnv_fromUWriteBytes(cnv,
                             bom, 2,
                             &pArgs->target, pArgs->targetLimit,
                             &pArgs->offsets, -1,
                             pErrorCode);
        cnv->fromUnicodeStatus=0;
    }

    target=pArgs->target;
    ಯದಿ(target >= pArgs->targetLimit) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-target);
    offsets=pArgs->offsets;
    sourceIndex=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    ಯದಿ((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*source) && targetCapacity>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++source;
        --length;
        target[0]=(uint8_t)(c>>8);
        target[1]=(uint8_t)c;
        target[2]=(uint8_t)(trail>>8);
        target[3]=(uint8_t)trail;
        target+=4;
        targetCapacity-=4;
        ಯದಿ(offsets!=nullptr) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        sourceIndex=1;
        cnv->fromUChar32=c=0;
    }

    ಯದಿ(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        ಯದಿ(count>targetCapacity) {
            count=targetCapacity&~1;
        }
        /* count is even */
        targetCapacity-=count;
        count>>=1;
        length-=count;

        ಯದಿ(offsets==nullptr) {
            ಯಾವತ್(count>0) {
                c=*source++;
                ಯದಿ(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target+=2;
                } ಅನ್ಯಥಾ ಯದಿ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target[2]=(uint8_t)(trail>>8);
                    target[3]=(uint8_t)trail;
                    target+=4;
                } ಅನ್ಯಥಾ {
                    ಭಙ್ಗ;
                }
                --count;
            }
        } ಅನ್ಯಥಾ {
            ಯಾವತ್(count>0) {
                c=*source++;
                ಯದಿ(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target+=2;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex++;
                } ಅನ್ಯಥಾ ಯದಿ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
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
                } ಅನ್ಯಥಾ {
                    ಭಙ್ಗ;
                }
                --count;
            }
        }

        ಯದಿ(count==0) {
            /* done with the loop for complete UChars */
            ಯದಿ(length>0 && targetCapacity>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                ಯದಿ(U16_IS_SINGLE(c=*source++)) {
                    overflow[0]=(ವರ್ಣ)(c>>8);
                    overflow[1]=(ವರ್ಣ)c;
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } ಅನ್ಯಥಾ {
                length=0;
                c=0;
            }
        } ಅನ್ಯಥಾ {
            /* keep c for surrogate handling, length will be set there */
            targetCapacity+=2*count;
        }
    } ಅನ್ಯಥಾ {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    ಯದಿ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        ಯದಿ(U16_IS_SURROGATE_LEAD(c)) {
            ಯದಿ(source<pArgs->sourceLimit) {
                ಯದಿ(U16_IS_TRAIL(trail=*source)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++source;
                    overflow[0]=(ವರ್ಣ)(c>>8);
                    overflow[1]=(ವರ್ಣ)c;
                    overflow[2]=(ವರ್ಣ)(trail>>8);
                    overflow[3]=(ವರ್ಣ)trail;
                    length=4; /* 4 bytes to output */
                    c=0;
                } ಅನ್ಯಥಾ {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } ಅನ್ಯಥಾ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ಅನ್ಯಥಾ {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
        cnv->fromUChar32=c;
    }

    ಯದಿ(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_fromUWriteBytes(cnv,
                             overflow, length,
                             &target, pArgs->targetLimit,
                             &offsets, sourceIndex,
                             pErrorCode);
        targetCapacity = static_cast<uint32_t>(pArgs->targetLimit - target);
    }

    ಯದಿ(U_SUCCESS(*pErrorCode) && source<pArgs->sourceLimit && targetCapacity==0) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target = target;
    pArgs->offsets=offsets;
}

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16BEToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                             UErrorCode *pErrorCode) {
    UConverter *cnv;
    ಧ್ರುವ uint8_t *source;
    char16_t *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, count, sourceIndex;
    char16_t c, trail;

    ಯದಿ(pArgs->converter->mode<8) {
        _UTF16ToUnicodeWithOffsets(pArgs, pErrorCode);
        ಪ್ರತ್ಯಾಗಮ;
    }

    cnv=pArgs->converter;
    source=(ಧ್ರುವ uint8_t *)pArgs->source;
    length=(int32_t)((ಧ್ರುವ uint8_t *)pArgs->sourceLimit-source);
    ಯದಿ(length<=0 && cnv->toUnicodeStatus==0) {
        /* no input, nothing to do */
        ಪ್ರತ್ಯಾಗಮ;
    }

    target=pArgs->target;
    ಯದಿ(target >= pArgs->targetLimit) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-target);
    offsets=pArgs->offsets;
    sourceIndex=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    ಯದಿ(cnv->toUnicodeStatus!=0) {
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
    ಯದಿ((count=cnv->toULength)!=0) {
        uint8_t *p=cnv->toUBytes;
        ಕುರು {
            p[count++]=*source++;
            ++sourceIndex;
            --length;
            ಯದಿ(count==2) {
                c=((char16_t)p[0]<<8)|p[1];
                ಯದಿ(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *target++=c;
                    ಯದಿ(offsets!=nullptr) {
                        *offsets++=-1;
                    }
                    --targetCapacity;
                    count=0;
                    c=0;
                    ಭಙ್ಗ;
                } ಅನ್ಯಥಾ ಯದಿ(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } ಅನ್ಯಥಾ {
                    /* fall through to error handling for an unmatched trail surrogate */
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ ಯದಿ(count==4) {
                c=((char16_t)p[0]<<8)|p[1];
                trail=((char16_t)p[2]<<8)|p[3];
                ಯದಿ(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *target++=c;
                    ಯದಿ(targetCapacity>=2) {
                        *target++=trail;
                        ಯದಿ(offsets!=nullptr) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        targetCapacity-=2;
                    } ಅನ್ಯಥಾ /* targetCapacity==1 */ {
                        targetCapacity=0;
                        cnv->UCharErrorBuffer[0]=trail;
                        cnv->UCharErrorBufferLength=1;
                        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                    }
                    count=0;
                    c=0;
                    ಭಙ್ಗ;
                } ಅನ್ಯಥಾ {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;

                    /* back out reading the code unit after it */
                    ಯದಿ(((ಧ್ರುವ uint8_t *)pArgs->source-source)>=2) {
                        source-=2;
                    } ಅನ್ಯಥಾ {
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
                    pArgs->source=(ಧ್ರುವ ವರ್ಣ *)source;
                    pArgs->target=target;
                    pArgs->offsets=offsets;
                    ಪ್ರತ್ಯಾಗಮ;
                }
            }
        } ಯಾವತ್(length>0);
        cnv->toULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*targetCapacity;
    ಯದಿ(count>length) {
        count=length&~1;
    }
    ಯದಿ(c==0 && count>0) {
        length-=count;
        count>>=1;
        targetCapacity-=count;
        ಯದಿ(offsets==nullptr) {
            ಕುರು {
                c=((char16_t)source[0]<<8)|source[1];
                source+=2;
                ಯದಿ(U16_IS_SINGLE(c)) {
                    *target++=c;
                } ಅನ್ಯಥಾ ಯದಿ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                } ಅನ್ಯಥಾ {
                    ಭಙ್ಗ;
                }
            } ಯಾವತ್(--count>0);
        } ಅನ್ಯಥಾ {
            ಕುರು {
                c=((char16_t)source[0]<<8)|source[1];
                source+=2;
                ಯದಿ(U16_IS_SINGLE(c)) {
                    *target++=c;
                    *offsets++=sourceIndex;
                    sourceIndex+=2;
                } ಅನ್ಯಥಾ ಯದಿ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex;
                    sourceIndex+=4;
                } ಅನ್ಯಥಾ {
                    ಭಙ್ಗ;
                }
            } ಯಾವತ್(--count>0);
        }

        ಯದಿ(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } ಅನ್ಯಥಾ {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            targetCapacity+=count;
        }
    }

    ಯದಿ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->toUBytes[0]=(uint8_t)(c>>8);
        cnv->toUBytes[1]=(uint8_t)c;
        cnv->toULength=2;

        ಯದಿ(U16_IS_SURROGATE_LEAD(c)) {
            ಯದಿ(length>=2) {
                ಯದಿ(U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    source+=2;
                    length-=2;
                    *target++=c;
                    ಯದಿ(offsets!=nullptr) {
                        *offsets++=sourceIndex;
                    }
                    cnv->UCharErrorBuffer[0]=trail;
                    cnv->UCharErrorBufferLength=1;
                    cnv->toULength=0;
                    *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                } ಅನ್ಯಥಾ {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } ಅನ್ಯಥಾ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ಅನ್ಯಥಾ {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
    }

    ಯದಿ(U_SUCCESS(*pErrorCode)) {
        /* check for a remaining source byte */
        ಯದಿ(length>0) {
            ಯದಿ(targetCapacity==0) {
                *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            } ಅನ್ಯಥಾ {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->toUBytes[cnv->toULength++]=*source++;
            }
        }
    }

    /* write back the updated pointers */
    pArgs->source=(ಧ್ರುವ ವರ್ಣ *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

ಸ್ಥಿರ UChar32  U_CALLCONV
_UTF16BEGetNextUChar(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    ಧ್ರುವ uint8_t *s, *sourceLimit;
    UChar32 c;

    ಯದಿ(pArgs->converter->mode<8) {
        ಪ್ರತ್ಯಾಗಮ UCNV_GET_NEXT_UCHAR_USE_TO_U;
    }

    s=(ಧ್ರುವ uint8_t *)pArgs->source;
    sourceLimit=(ಧ್ರುವ uint8_t *)pArgs->sourceLimit;

    ಯದಿ(s>=sourceLimit) {
        /* no input */
        *err=U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0xffff;
    }

    ಯದಿ(s+2>sourceLimit) {
        /* only one byte: truncated char16_t */
        pArgs->converter->toUBytes[0]=*s++;
        pArgs->converter->toULength=1;
        pArgs->source=(ಧ್ರುವ ವರ್ಣ *)s;
        *err = U_TRUNCATED_CHAR_FOUND;
        ಪ್ರತ್ಯಾಗಮ 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)*s<<8)|s[1];
    s+=2;

    /* check for a surrogate pair */
    ಯದಿ(U_IS_SURROGATE(c)) {
        ಯದಿ(U16_IS_SURROGATE_LEAD(c)) {
            ಯದಿ(s+2<=sourceLimit) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)*s<<8)|s[1];
                ಯದಿ(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } ಅನ್ಯಥಾ {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } ಅನ್ಯಥಾ {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *bytes=pArgs->converter->toUBytes;
                s-=2;
                pArgs->converter->toULength=(int8_t)(sourceLimit-s);
                ಕುರು {
                    *bytes++=*s++;
                } ಯಾವತ್(s<sourceLimit);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_FOUND;
            }
        } ಅನ್ಯಥಾ {
            /* unmatched trail surrogate */
            c=-2;
        }

        ಯದಿ(c<0) {
            /* write the unmatched surrogate */
            uint8_t *bytes=pArgs->converter->toUBytes;
            pArgs->converter->toULength=2;
            *bytes=*(s-2);
            bytes[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_FOUND;
        }
    }

    pArgs->source=(ಧ್ರುವ ವರ್ಣ *)s;
    ಪ್ರತ್ಯಾಗಮ c;
} 

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16BEReset(UConverter *cnv, UConverterResetChoice choice) {
    ಯದಿ(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode state */
        ಯದಿ(UCNV_GET_VERSION(cnv)==0) {
            cnv->mode=8; /* no BOM handling */
        } ಅನ್ಯಥಾ {
            cnv->mode=0; /* Java-specific "UnicodeBig" requires BE BOM or no BOM */
        }
    }
    ಯದಿ(choice!=UCNV_RESET_TO_UNICODE && UCNV_GET_VERSION(cnv)==1) {
        /* reset fromUnicode for "UnicodeBig": prepare to output the UTF-16BE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16BEOpen(UConverter *cnv,
             UConverterLoadArgs *pArgs,
             UErrorCode *pErrorCode) {
    (ಶೂನ್ಯ)pArgs;
    ಯದಿ(UCNV_GET_VERSION(cnv)<=1) {
        _UTF16BEReset(cnv, UCNV_RESET_BOTH);
    } ಅನ್ಯಥಾ {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *  U_CALLCONV
_UTF16BEGetName(ಧ್ರುವ UConverter *cnv) {
    ಯದಿ(UCNV_GET_VERSION(cnv)==0) {
        ಪ್ರತ್ಯಾಗಮ "UTF-16BE";
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ "UTF-16BE,version=1";
    }
}
U_CDECL_END

ಸ್ಥಿರ ಧ್ರುವ UConverterImpl _UTF16BEImpl={
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

ಸ್ಥಿರ ಧ್ರುವ UConverterStaticData _UTF16BEStaticData={
    ಪರಿಮಾಣ(UConverterStaticData),
    "UTF-16BE",
    1200, UCNV_IBM, UCNV_UTF16_BigEndian, 2, 2,
    { 0xff, 0xfd, 0, 0 },2,ಅಸತ್ಯ,ಅಸತ್ಯ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ಧ್ರುವ UConverterSharedData _UTF16BEData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF16BEStaticData, &_UTF16BEImpl);

/* UTF-16LE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16LEFromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                               UErrorCode *pErrorCode) {
    UConverter *cnv;
    ಧ್ರುವ char16_t *source;
    ವರ್ಣ *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, sourceIndex;
    char16_t c, trail;
    ವರ್ಣ overflow[4];

    source=pArgs->source;
    length=(int32_t)(pArgs->sourceLimit-source);
    ಯದಿ(length<=0) {
        /* no input, nothing to do */
        ಪ್ರತ್ಯಾಗಮ;
    }

    cnv=pArgs->converter;

    /* write the BOM if necessary */
    ಯದಿ(cnv->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ bom[]={ (ವರ್ಣ)0xffu, (ವರ್ಣ)0xfeu };
        ucnv_fromUWriteBytes(cnv,
                             bom, 2,
                             &pArgs->target, pArgs->targetLimit,
                             &pArgs->offsets, -1,
                             pErrorCode);
        cnv->fromUnicodeStatus=0;
    }

    target=pArgs->target;
    ಯದಿ(target >= pArgs->targetLimit) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;
    sourceIndex=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    ಯದಿ((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*source) && targetCapacity>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++source;
        --length;
        target[0]=(uint8_t)c;
        target[1]=(uint8_t)(c>>8);
        target[2]=(uint8_t)trail;
        target[3]=(uint8_t)(trail>>8);
        target+=4;
        targetCapacity-=4;
        ಯದಿ(offsets!=nullptr) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        sourceIndex=1;
        cnv->fromUChar32=c=0;
    }

    ಯದಿ(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        ಯದಿ(count>targetCapacity) {
            count=targetCapacity&~1;
        }
        /* count is even */
        targetCapacity-=count;
        count>>=1;
        length-=count;

        ಯದಿ(offsets==nullptr) {
            ಯಾವತ್(count>0) {
                c=*source++;
                ಯದಿ(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target+=2;
                } ಅನ್ಯಥಾ ಯದಿ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target[2]=(uint8_t)trail;
                    target[3]=(uint8_t)(trail>>8);
                    target+=4;
                } ಅನ್ಯಥಾ {
                    ಭಙ್ಗ;
                }
                --count;
            }
        } ಅನ್ಯಥಾ {
            ಯಾವತ್(count>0) {
                c=*source++;
                ಯದಿ(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target+=2;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex++;
                } ಅನ್ಯಥಾ ಯದಿ(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
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
                } ಅನ್ಯಥಾ {
                    ಭಙ್ಗ;
                }
                --count;
            }
        }

        ಯದಿ(count==0) {
            /* done with the loop for complete UChars */
            ಯದಿ(length>0 && targetCapacity>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                ಯದಿ(U16_IS_SINGLE(c=*source++)) {
                    overflow[0]=(ವರ್ಣ)c;
                    overflow[1]=(ವರ್ಣ)(c>>8);
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } ಅನ್ಯಥಾ {
                length=0;
                c=0;
            }
        } ಅನ್ಯಥಾ {
            /* keep c for surrogate handling, length will be set there */
            targetCapacity+=2*count;
        }
    } ಅನ್ಯಥಾ {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    ಯದಿ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        ಯದಿ(U16_IS_SURROGATE_LEAD(c)) {
            ಯದಿ(source<pArgs->sourceLimit) {
                ಯದಿ(U16_IS_TRAIL(trail=*source)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++source;
                    overflow[0]=(ವರ್ಣ)c;
                    overflow[1]=(ವರ್ಣ)(c>>8);
                    overflow[2]=(ವರ್ಣ)trail;
                    overflow[3]=(ವರ್ಣ)(trail>>8);
                    length=4; /* 4 bytes to output */
                    c=0;
                } ಅನ್ಯಥಾ {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } ಅನ್ಯಥಾ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ಅನ್ಯಥಾ {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
        cnv->fromUChar32=c;
    }

    ಯದಿ(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_fromUWriteBytes(cnv,
                             overflow, length,
                             &target, pArgs->targetLimit,
                             &offsets, sourceIndex,
                             pErrorCode);
        targetCapacity = static_cast<uint32_t>(pArgs->targetLimit - target);
    }

    ಯದಿ(U_SUCCESS(*pErrorCode) && source<pArgs->sourceLimit && targetCapacity==0) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16LEToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                             UErrorCode *pErrorCode) {
    UConverter *cnv;
    ಧ್ರುವ uint8_t *source;
    char16_t *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, count, sourceIndex;
    char16_t c, trail;

    ಯದಿ(pArgs->converter->mode<8) {
        _UTF16ToUnicodeWithOffsets(pArgs, pErrorCode);
        ಪ್ರತ್ಯಾಗಮ;
    }

    cnv=pArgs->converter;
    source=(ಧ್ರುವ uint8_t *)pArgs->source;
    length=(int32_t)((ಧ್ರುವ uint8_t *)pArgs->sourceLimit-source);
    ಯದಿ(length<=0 && cnv->toUnicodeStatus==0) {
        /* no input, nothing to do */
        ಪ್ರತ್ಯಾಗಮ;
    }

    target=pArgs->target;
    ಯದಿ(target >= pArgs->targetLimit) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;
    sourceIndex=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    ಯದಿ(cnv->toUnicodeStatus!=0) {
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
    ಯದಿ((count=cnv->toULength)!=0) {
        uint8_t *p=cnv->toUBytes;
        ಕುರು {
            p[count++]=*source++;
            ++sourceIndex;
            --length;
            ಯದಿ(count==2) {
                c=((char16_t)p[1]<<8)|p[0];
                ಯದಿ(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *target++=c;
                    ಯದಿ(offsets!=nullptr) {
                        *offsets++=-1;
                    }
                    --targetCapacity;
                    count=0;
                    c=0;
                    ಭಙ್ಗ;
                } ಅನ್ಯಥಾ ಯದಿ(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } ಅನ್ಯಥಾ {
                    /* fall through to error handling for an unmatched trail surrogate */
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ ಯದಿ(count==4) {
                c=((char16_t)p[1]<<8)|p[0];
                trail=((char16_t)p[3]<<8)|p[2];
                ಯದಿ(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *target++=c;
                    ಯದಿ(targetCapacity>=2) {
                        *target++=trail;
                        ಯದಿ(offsets!=nullptr) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        targetCapacity-=2;
                    } ಅನ್ಯಥಾ /* targetCapacity==1 */ {
                        targetCapacity=0;
                        cnv->UCharErrorBuffer[0]=trail;
                        cnv->UCharErrorBufferLength=1;
                        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                    }
                    count=0;
                    c=0;
                    ಭಙ್ಗ;
                } ಅನ್ಯಥಾ {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;

                    /* back out reading the code unit after it */
                    ಯದಿ(((ಧ್ರುವ uint8_t *)pArgs->source-source)>=2) {
                        source-=2;
                    } ಅನ್ಯಥಾ {
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
                    pArgs->source=(ಧ್ರುವ ವರ್ಣ *)source;
                    pArgs->target=target;
                    pArgs->offsets=offsets;
                    ಪ್ರತ್ಯಾಗಮ;
                }
            }
        } ಯಾವತ್(length>0);
        cnv->toULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*targetCapacity;
    ಯದಿ(count>length) {
        count=length&~1;
    }
    ಯದಿ(c==0 && count>0) {
        length-=count;
        count>>=1;
        targetCapacity-=count;
        ಯದಿ(offsets==nullptr) {
            ಕುರು {
                c=((char16_t)source[1]<<8)|source[0];
                source+=2;
                ಯದಿ(U16_IS_SINGLE(c)) {
                    *target++=c;
                } ಅನ್ಯಥಾ ಯದಿ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                } ಅನ್ಯಥಾ {
                    ಭಙ್ಗ;
                }
            } ಯಾವತ್(--count>0);
        } ಅನ್ಯಥಾ {
            ಕುರು {
                c=((char16_t)source[1]<<8)|source[0];
                source+=2;
                ಯದಿ(U16_IS_SINGLE(c)) {
                    *target++=c;
                    *offsets++=sourceIndex;
                    sourceIndex+=2;
                } ಅನ್ಯಥಾ ಯದಿ(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                    *offsets++=sourceIndex;
                    *offsets++=sourceIndex;
                    sourceIndex+=4;
                } ಅನ್ಯಥಾ {
                    ಭಙ್ಗ;
                }
            } ಯಾವತ್(--count>0);
        }

        ಯದಿ(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } ಅನ್ಯಥಾ {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            targetCapacity+=count;
        }
    }

    ಯದಿ(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->toUBytes[0]=(uint8_t)c;
        cnv->toUBytes[1]=(uint8_t)(c>>8);
        cnv->toULength=2;

        ಯದಿ(U16_IS_SURROGATE_LEAD(c)) {
            ಯದಿ(length>=2) {
                ಯದಿ(U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    source+=2;
                    length-=2;
                    *target++=c;
                    ಯದಿ(offsets!=nullptr) {
                        *offsets++=sourceIndex;
                    }
                    cnv->UCharErrorBuffer[0]=trail;
                    cnv->UCharErrorBufferLength=1;
                    cnv->toULength=0;
                    *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                } ಅನ್ಯಥಾ {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } ಅನ್ಯಥಾ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ಅನ್ಯಥಾ {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
    }

    ಯದಿ(U_SUCCESS(*pErrorCode)) {
        /* check for a remaining source byte */
        ಯದಿ(length>0) {
            ಯದಿ(targetCapacity==0) {
                *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            } ಅನ್ಯಥಾ {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->toUBytes[cnv->toULength++]=*source++;
            }
        }
    }

    /* write back the updated pointers */
    pArgs->source=(ಧ್ರುವ ವರ್ಣ *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

ಸ್ಥಿರ UChar32  U_CALLCONV
_UTF16LEGetNextUChar(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    ಧ್ರುವ uint8_t *s, *sourceLimit;
    UChar32 c;

    ಯದಿ(pArgs->converter->mode<8) {
        ಪ್ರತ್ಯಾಗಮ UCNV_GET_NEXT_UCHAR_USE_TO_U;
    }

    s=(ಧ್ರುವ uint8_t *)pArgs->source;
    sourceLimit=(ಧ್ರುವ uint8_t *)pArgs->sourceLimit;

    ಯದಿ(s>=sourceLimit) {
        /* no input */
        *err=U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0xffff;
    }

    ಯದಿ(s+2>sourceLimit) {
        /* only one byte: truncated char16_t */
        pArgs->converter->toUBytes[0]=*s++;
        pArgs->converter->toULength=1;
        pArgs->source=(ಧ್ರುವ ವರ್ಣ *)s;
        *err = U_TRUNCATED_CHAR_FOUND;
        ಪ್ರತ್ಯಾಗಮ 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)s[1]<<8)|*s;
    s+=2;

    /* check for a surrogate pair */
    ಯದಿ(U_IS_SURROGATE(c)) {
        ಯದಿ(U16_IS_SURROGATE_LEAD(c)) {
            ಯದಿ(s+2<=sourceLimit) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)s[1]<<8)|*s;
                ಯದಿ(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } ಅನ್ಯಥಾ {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } ಅನ್ಯಥಾ {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *bytes=pArgs->converter->toUBytes;
                s-=2;
                pArgs->converter->toULength=(int8_t)(sourceLimit-s);
                ಕುರು {
                    *bytes++=*s++;
                } ಯಾವತ್(s<sourceLimit);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_FOUND;
            }
        } ಅನ್ಯಥಾ {
            /* unmatched trail surrogate */
            c=-2;
        }

        ಯದಿ(c<0) {
            /* write the unmatched surrogate */
            uint8_t *bytes=pArgs->converter->toUBytes;
            pArgs->converter->toULength=2;
            *bytes=*(s-2);
            bytes[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_FOUND;
        }
    }

    pArgs->source=(ಧ್ರುವ ವರ್ಣ *)s;
    ಪ್ರತ್ಯಾಗಮ c;
} 

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16LEReset(UConverter *cnv, UConverterResetChoice choice) {
    ಯದಿ(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode state */
        ಯದಿ(UCNV_GET_VERSION(cnv)==0) {
            cnv->mode=8; /* no BOM handling */
        } ಅನ್ಯಥಾ {
            cnv->mode=0; /* Java-specific "UnicodeLittle" requires LE BOM or no BOM */
        }
    }
    ಯದಿ(choice!=UCNV_RESET_TO_UNICODE && UCNV_GET_VERSION(cnv)==1) {
        /* reset fromUnicode for "UnicodeLittle": prepare to output the UTF-16LE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16LEOpen(UConverter *cnv,
             UConverterLoadArgs *pArgs,
             UErrorCode *pErrorCode) {
    (ಶೂನ್ಯ)pArgs;
    ಯದಿ(UCNV_GET_VERSION(cnv)<=1) {
        _UTF16LEReset(cnv, UCNV_RESET_BOTH);
    } ಅನ್ಯಥಾ {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *  U_CALLCONV
_UTF16LEGetName(ಧ್ರುವ UConverter *cnv) {
    ಯದಿ(UCNV_GET_VERSION(cnv)==0) {
        ಪ್ರತ್ಯಾಗಮ "UTF-16LE";
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ "UTF-16LE,version=1";
    }
}
U_CDECL_END

ಸ್ಥಿರ ಧ್ರುವ UConverterImpl _UTF16LEImpl={
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


ಸ್ಥಿರ ಧ್ರುವ UConverterStaticData _UTF16LEStaticData={
    ಪರಿಮಾಣ(UConverterStaticData),
    "UTF-16LE",
    1202, UCNV_IBM, UCNV_UTF16_LittleEndian, 2, 2,
    { 0xfd, 0xff, 0, 0 },2,ಅಸತ್ಯ,ಅಸತ್ಯ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ಧ್ರುವ UConverterSharedData _UTF16LEData=
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
ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_UTF16Reset(UConverter *cnv, UConverterResetChoice choice) {
    ಯದಿ(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->mode=0;
    }
    ಯದಿ(choice!=UCNV_RESET_TO_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-16PE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}
U_CDECL_END
ಬಾಹ್ಯ ಧ್ರುವ UConverterSharedData _UTF16v2Data;
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_UTF16Open(UConverter *cnv,
           UConverterLoadArgs *pArgs,
           UErrorCode *pErrorCode) {
    ಯದಿ(UCNV_GET_VERSION(cnv)<=2) {
        ಯದಿ(UCNV_GET_VERSION(cnv)==2 && !pArgs->onlyTestIsLoadable) {
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
    } ಅನ್ಯಥಾ {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *  U_CALLCONV
_UTF16GetName(ಧ್ರುವ UConverter *cnv) {
    ಯದಿ(UCNV_GET_VERSION(cnv)==0) {
        ಪ್ರತ್ಯಾಗಮ "UTF-16";
    } ಅನ್ಯಥಾ ಯದಿ(UCNV_GET_VERSION(cnv)==1) {
        ಪ್ರತ್ಯಾಗಮ "UTF-16,version=1";
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ "UTF-16,version=2";
    }
}
U_CDECL_END
ಬಾಹ್ಯ ಧ್ರುವ UConverterSharedData _UTF16Data;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ IS_UTF16BE(ಧ್ರುವ UConverter *cnv) {
    ಪ್ರತ್ಯಾಗಮ ((cnv)->sharedData == &_UTF16BEData);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ IS_UTF16LE(ಧ್ರುವ UConverter *cnv) {
    ಪ್ರತ್ಯಾಗಮ ((cnv)->sharedData == &_UTF16LEData);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ತಾರ್ಕಿಕ IS_UTF16(ಧ್ರುವ UConverter *cnv) {
    ಪ್ರತ್ಯಾಗಮ ((cnv)->sharedData==&_UTF16Data) || ((cnv)->sharedData == &_UTF16v2Data);
}

U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_UTF16ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    ಧ್ರುವ ವರ್ಣ *source=pArgs->source;
    ಧ್ರುವ ವರ್ಣ *sourceLimit=pArgs->sourceLimit;
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

    ಯಾವತ್(source<sourceLimit && U_SUCCESS(*pErrorCode)) {
        ವಿಕಲ್ಪ(state) {
        ಪಕ್ಷ 0:
            cnv->toUBytes[0]=(uint8_t)*source++;
            cnv->toULength=1;
            state=1;
            ಭಙ್ಗ;
        ಪಕ್ಷ 1:
            /*
             * Only inside this switch case can the state variable
             * temporarily take two additional values:
             * 6: BOM error, continue with BE
             * 7: BOM error, continue with LE
             */
            b=*source;
            ಯದಿ(cnv->toUBytes[0]==0xfe && b==0xff) {
                ಯದಿ(IS_UTF16LE(cnv)) {
                    state=7; /* illegal reverse BOM for Java "UnicodeLittle" */
                } ಅನ್ಯಥಾ {
                    state=8; /* detect UTF-16BE */
                }
            } ಅನ್ಯಥಾ ಯದಿ(cnv->toUBytes[0]==0xff && b==0xfe) {
                ಯದಿ(IS_UTF16BE(cnv)) {
                    state=6; /* illegal reverse BOM for Java "UnicodeBig" */
                } ಅನ್ಯಥಾ {
                    state=9; /* detect UTF-16LE */
                }
            } ಅನ್ಯಥಾ ಯದಿ((IS_UTF16(cnv) && UCNV_GET_VERSION(cnv)==1)) {
                state=6; /* illegal missing BOM for Java "Unicode" */
            }
            ಯದಿ(state>=8) {
                /* BOM detected, consume it */
                ++source;
                cnv->toULength=0;
                offsetDelta=(int32_t)(source-pArgs->source);
            } ಅನ್ಯಥಾ ಯದಿ(state<6) {
                /* ok: no BOM, and not a reverse BOM */
                ಯದಿ(source!=pArgs->source) {
                    /* reset the source for a correct first offset */
                    source=pArgs->source;
                    cnv->toULength=0;
                }
                ಯದಿ(IS_UTF16LE(cnv)) {
                    /* Make Java "UnicodeLittle" default to LE. */
                    state=9;
                } ಅನ್ಯಥಾ {
                    /* Make standard UTF-16 and Java "UnicodeBig" default to BE. */
                    state=8;
                }
            } ಅನ್ಯಥಾ {
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
                ಪ್ರತ್ಯಾಗಮ;
            }
            /* convert the rest of the stream */
            cnv->mode=state;
            ಅನುವರ್ತ;
        ಪಕ್ಷ 8:
            /* call UTF-16BE */
            pArgs->source=source;
            _UTF16BEToUnicodeWithOffsets(pArgs, pErrorCode);
            source=pArgs->source;
            ಭಙ್ಗ;
        ಪಕ್ಷ 9:
            /* call UTF-16LE */
            pArgs->source=source;
            _UTF16LEToUnicodeWithOffsets(pArgs, pErrorCode);
            source=pArgs->source;
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            ಭಙ್ಗ; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    ಯದಿ(offsets!=nullptr && offsetDelta!=0) {
        int32_t *offsetsLimit=pArgs->offsets;
        ಯಾವತ್(offsets<offsetsLimit) {
            *offsets++ += offsetDelta;
        }
    }

    pArgs->source=source;

    ಯದಿ(source==sourceLimit && pArgs->flush) {
        /* handle truncated input */
        ವಿಕಲ್ಪ(state) {
        ಪಕ್ಷ 0:
            ಭಙ್ಗ; /* no input at all, nothing to do */
        ಪಕ್ಷ 8:
            _UTF16BEToUnicodeWithOffsets(pArgs, pErrorCode);
            ಭಙ್ಗ;
        ಪಕ್ಷ 9:
            _UTF16LEToUnicodeWithOffsets(pArgs, pErrorCode);
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            /* 0<state<8: framework will report truncation, nothing to do here */
            ಭಙ್ಗ;
        }
    }

    cnv->mode=state;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
_UTF16GetNextUChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    ವಿಕಲ್ಪ(pArgs->converter->mode) {
    ಪಕ್ಷ 8:
        ಪ್ರತ್ಯಾಗಮ _UTF16BEGetNextUChar(pArgs, pErrorCode);
    ಪಕ್ಷ 9:
        ಪ್ರತ್ಯಾಗಮ _UTF16LEGetNextUChar(pArgs, pErrorCode);
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ UCNV_GET_NEXT_UCHAR_USE_TO_U;
    }
}
U_CDECL_END

ಸ್ಥಿರ ಧ್ರುವ UConverterImpl _UTF16Impl = {
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

ಸ್ಥಿರ ಧ್ರುವ UConverterStaticData _UTF16StaticData = {
    ಪರಿಮಾಣ(UConverterStaticData),
    "UTF-16",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
#ಯದಿ U_IS_BIG_ENDIAN
    { 0xff, 0xfd, 0, 0 }, 2,
#ಅನ್ಯಥಾ
    { 0xfd, 0xff, 0, 0 }, 2,
#endif
    ಅಸತ್ಯ, ಅಸತ್ಯ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಧ್ರುವ UConverterSharedData _UTF16Data =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF16StaticData, &_UTF16Impl);

ಸ್ಥಿರ ಧ್ರುವ UConverterImpl _UTF16v2Impl = {
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

ಸ್ಥಿರ ಧ್ರುವ UConverterStaticData _UTF16v2StaticData = {
    ಪರಿಮಾಣ(UConverterStaticData),
    "UTF-16,version=2",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
    { 0xff, 0xfd, 0, 0 }, 2,
    ಅಸತ್ಯ, ಅಸತ್ಯ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಧ್ರುವ UConverterSharedData _UTF16v2Data =
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_UTF16v2StaticData, &_UTF16v2Impl);

#endif
