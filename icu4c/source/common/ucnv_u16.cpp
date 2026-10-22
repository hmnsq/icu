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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश "unicode/ucnv.h"
#समावेश "unicode/uversion.h"
#समावेश "ucnv_bld.h"
#समावेश "ucnv_cnv.h"
#समावेश "cmemory.h"

क्रमागत {
    UCNV_NEED_TO_WRITE_BOM=1
};

U_CDECL_BEGIN
/*
 * The UTF-16 toUnicode implementation is also used for the Java-specific
 * "with BOM" variants of UTF-16BE and UTF-16LE.
 */
अटल व्योम  U_CALLCONV
_UTF16ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode);

/* UTF-16BE ----------------------------------------------------------------- */

#अगर U_IS_BIG_ENDIAN
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16BEFromUnicodeWithOffsets
#अन्यथा
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16LEFromUnicodeWithOffsets
#पूर्ण_अगर


अटल व्योम  U_CALLCONV
_UTF16BEFromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                               UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर char16_t *source;
    अक्षर *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, source_सूचकांक;
    char16_t c, trail;
    अक्षर overflow[4];

    source=pArgs->source;
    length=(int32_t)(pArgs->sourceLimit-source);
    अगर(length<=0) {
        /* no input, nothing to do */
        वापस;
    }

    cnv=pArgs->converter;

    /* write the BOM if necessary */
    अगर(cnv->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        अटल स्थिर अक्षर bom[]={ (अक्षर)0xfeu, (अक्षर)0xffu };
        ucnv_fromUWriteBytes(cnv,
                             bom, 2,
                             &pArgs->target, pArgs->targetLimit,
                             &pArgs->offsets, -1,
                             pErrorCode);
        cnv->fromUnicodeStatus=0;
    }

    target=pArgs->target;
    अगर(target >= pArgs->targetLimit) {
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
        वापस;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-target);
    offsets=pArgs->offsets;
    source_सूचकांक=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    अगर((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*source) && targetCapacity>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++source;
        --length;
        target[0]=(uint8_t)(c>>8);
        target[1]=(uint8_t)c;
        target[2]=(uint8_t)(trail>>8);
        target[3]=(uint8_t)trail;
        target+=4;
        targetCapacity-=4;
        अगर(offsets!=nullptr) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        source_सूचकांक=1;
        cnv->fromUChar32=c=0;
    }

    अगर(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        अगर(count>targetCapacity) {
            count=targetCapacity&~1;
        }
        /* count is even */
        targetCapacity-=count;
        count>>=1;
        length-=count;

        अगर(offsets==nullptr) {
            जबतक(count>0) {
                c=*source++;
                अगर(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target+=2;
                } अन्यथा अगर(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target[2]=(uint8_t)(trail>>8);
                    target[3]=(uint8_t)trail;
                    target+=4;
                } अन्यथा {
                    अवरोध;
                }
                --count;
            }
        } अन्यथा {
            जबतक(count>0) {
                c=*source++;
                अगर(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target+=2;
                    *offsets++=source_सूचकांक;
                    *offsets++=source_सूचकांक++;
                } अन्यथा अगर(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target[2]=(uint8_t)(trail>>8);
                    target[3]=(uint8_t)trail;
                    target+=4;
                    *offsets++=source_सूचकांक;
                    *offsets++=source_सूचकांक;
                    *offsets++=source_सूचकांक;
                    *offsets++=source_सूचकांक;
                    source_सूचकांक+=2;
                } अन्यथा {
                    अवरोध;
                }
                --count;
            }
        }

        अगर(count==0) {
            /* done with the loop for complete UChars */
            अगर(length>0 && targetCapacity>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                अगर(U16_IS_SINGLE(c=*source++)) {
                    overflow[0]=(अक्षर)(c>>8);
                    overflow[1]=(अक्षर)c;
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } अन्यथा {
                length=0;
                c=0;
            }
        } अन्यथा {
            /* keep c for surrogate handling, length will be set there */
            targetCapacity+=2*count;
        }
    } अन्यथा {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    अगर(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        अगर(U16_IS_SURROGATE_LEAD(c)) {
            अगर(source<pArgs->sourceLimit) {
                अगर(U16_IS_TRAIL(trail=*source)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++source;
                    overflow[0]=(अक्षर)(c>>8);
                    overflow[1]=(अक्षर)c;
                    overflow[2]=(अक्षर)(trail>>8);
                    overflow[3]=(अक्षर)trail;
                    length=4; /* 4 bytes to output */
                    c=0;
                } अन्यथा {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } अन्यथा {
                /* see if the trail surrogate is in the next buffer */
            }
        } अन्यथा {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
        cnv->fromUChar32=c;
    }

    अगर(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_fromUWriteBytes(cnv,
                             overflow, length,
                             &target, pArgs->targetLimit,
                             &offsets, source_सूचकांक,
                             pErrorCode);
        targetCapacity = static_cast<uint32_t>(pArgs->targetLimit - target);
    }

    अगर(U_SUCCESS(*pErrorCode) && source<pArgs->sourceLimit && targetCapacity==0) {
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target = target;
    pArgs->offsets=offsets;
}

अटल व्योम  U_CALLCONV
_UTF16BEToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                             UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर uint8_t *source;
    char16_t *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, count, source_सूचकांक;
    char16_t c, trail;

    अगर(pArgs->converter->mode<8) {
        _UTF16ToUnicodeWithOffsets(pArgs, pErrorCode);
        वापस;
    }

    cnv=pArgs->converter;
    source=(स्थिर uint8_t *)pArgs->source;
    length=(int32_t)((स्थिर uint8_t *)pArgs->sourceLimit-source);
    अगर(length<=0 && cnv->toUnicodeStatus==0) {
        /* no input, nothing to do */
        वापस;
    }

    target=pArgs->target;
    अगर(target >= pArgs->targetLimit) {
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
        वापस;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-target);
    offsets=pArgs->offsets;
    source_सूचकांक=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    अगर(cnv->toUnicodeStatus!=0) {
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
    अगर((count=cnv->toULength)!=0) {
        uint8_t *p=cnv->toUBytes;
        करो {
            p[count++]=*source++;
            ++source_सूचकांक;
            --length;
            अगर(count==2) {
                c=((char16_t)p[0]<<8)|p[1];
                अगर(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *target++=c;
                    अगर(offsets!=nullptr) {
                        *offsets++=-1;
                    }
                    --targetCapacity;
                    count=0;
                    c=0;
                    अवरोध;
                } अन्यथा अगर(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } अन्यथा {
                    /* fall through to error handling for an unmatched trail surrogate */
                    अवरोध;
                }
            } अन्यथा अगर(count==4) {
                c=((char16_t)p[0]<<8)|p[1];
                trail=((char16_t)p[2]<<8)|p[3];
                अगर(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *target++=c;
                    अगर(targetCapacity>=2) {
                        *target++=trail;
                        अगर(offsets!=nullptr) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        targetCapacity-=2;
                    } अन्यथा /* targetCapacity==1 */ {
                        targetCapacity=0;
                        cnv->UChar_Error_बफर[0]=trail;
                        cnv->UChar_Error_बफर_लंबाई=1;
                        *pErrorCode=U_बफर_OVERFLOW_ERROR;
                    }
                    count=0;
                    c=0;
                    अवरोध;
                } अन्यथा {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;

                    /* back out reading the code unit after it */
                    अगर(((स्थिर uint8_t *)pArgs->source-source)>=2) {
                        source-=2;
                    } अन्यथा {
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
                    pArgs->source=(स्थिर अक्षर *)source;
                    pArgs->target=target;
                    pArgs->offsets=offsets;
                    वापस;
                }
            }
        } जबतक(length>0);
        cnv->toULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*targetCapacity;
    अगर(count>length) {
        count=length&~1;
    }
    अगर(c==0 && count>0) {
        length-=count;
        count>>=1;
        targetCapacity-=count;
        अगर(offsets==nullptr) {
            करो {
                c=((char16_t)source[0]<<8)|source[1];
                source+=2;
                अगर(U16_IS_SINGLE(c)) {
                    *target++=c;
                } अन्यथा अगर(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                } अन्यथा {
                    अवरोध;
                }
            } जबतक(--count>0);
        } अन्यथा {
            करो {
                c=((char16_t)source[0]<<8)|source[1];
                source+=2;
                अगर(U16_IS_SINGLE(c)) {
                    *target++=c;
                    *offsets++=source_सूचकांक;
                    source_सूचकांक+=2;
                } अन्यथा अगर(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                    *offsets++=source_सूचकांक;
                    *offsets++=source_सूचकांक;
                    source_सूचकांक+=4;
                } अन्यथा {
                    अवरोध;
                }
            } जबतक(--count>0);
        }

        अगर(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } अन्यथा {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            targetCapacity+=count;
        }
    }

    अगर(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->toUBytes[0]=(uint8_t)(c>>8);
        cnv->toUBytes[1]=(uint8_t)c;
        cnv->toULength=2;

        अगर(U16_IS_SURROGATE_LEAD(c)) {
            अगर(length>=2) {
                अगर(U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    source+=2;
                    length-=2;
                    *target++=c;
                    अगर(offsets!=nullptr) {
                        *offsets++=source_सूचकांक;
                    }
                    cnv->UChar_Error_बफर[0]=trail;
                    cnv->UChar_Error_बफर_लंबाई=1;
                    cnv->toULength=0;
                    *pErrorCode=U_बफर_OVERFLOW_ERROR;
                } अन्यथा {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } अन्यथा {
                /* see if the trail surrogate is in the next buffer */
            }
        } अन्यथा {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
    }

    अगर(U_SUCCESS(*pErrorCode)) {
        /* check for a remaining source byte */
        अगर(length>0) {
            अगर(targetCapacity==0) {
                *pErrorCode=U_बफर_OVERFLOW_ERROR;
            } अन्यथा {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->toUBytes[cnv->toULength++]=*source++;
            }
        }
    }

    /* write back the updated pointers */
    pArgs->source=(स्थिर अक्षर *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

अटल UChar32  U_CALLCONV
UTF16_BEGet_अग्र_UChar(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    स्थिर uint8_t *s, *sourceLimit;
    UChar32 c;

    अगर(pArgs->converter->mode<8) {
        वापस UCNV_GET_अग्र_UCHAR_USE_TO_U;
    }

    s=(स्थिर uint8_t *)pArgs->source;
    sourceLimit=(स्थिर uint8_t *)pArgs->sourceLimit;

    अगर(s>=sourceLimit) {
        /* no input */
        *err=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0xffff;
    }

    अगर(s+2>sourceLimit) {
        /* only one byte: truncated char16_t */
        pArgs->converter->toUBytes[0]=*s++;
        pArgs->converter->toULength=1;
        pArgs->source=(स्थिर अक्षर *)s;
        *err = U_TRUNCATED_CHAR_FOUND;
        वापस 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)*s<<8)|s[1];
    s+=2;

    /* check for a surrogate pair */
    अगर(U_IS_SURROGATE(c)) {
        अगर(U16_IS_SURROGATE_LEAD(c)) {
            अगर(s+2<=sourceLimit) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)*s<<8)|s[1];
                अगर(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } अन्यथा {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } अन्यथा {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *bytes=pArgs->converter->toUBytes;
                s-=2;
                pArgs->converter->toULength=(int8_t)(sourceLimit-s);
                करो {
                    *bytes++=*s++;
                } जबतक(s<sourceLimit);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_FOUND;
            }
        } अन्यथा {
            /* unmatched trail surrogate */
            c=-2;
        }

        अगर(c<0) {
            /* write the unmatched surrogate */
            uint8_t *bytes=pArgs->converter->toUBytes;
            pArgs->converter->toULength=2;
            *bytes=*(s-2);
            bytes[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_FOUND;
        }
    }

    pArgs->source=(स्थिर अक्षर *)s;
    वापस c;
} 

अटल व्योम  U_CALLCONV
_UTF16BEReset(UConverter *cnv, UConverterResetChoice choice) {
    अगर(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode state */
        अगर(UCNV_GET_VERSION(cnv)==0) {
            cnv->mode=8; /* no BOM handling */
        } अन्यथा {
            cnv->mode=0; /* Java-specific "UnicodeBig" requires BE BOM or no BOM */
        }
    }
    अगर(choice!=UCNV_RESET_TO_UNICODE && UCNV_GET_VERSION(cnv)==1) {
        /* reset fromUnicode for "UnicodeBig": prepare to output the UTF-16BE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

अटल व्योम  U_CALLCONV
_UTF16BEOpen(UConverter *cnv,
             UConverterLoadArgs *pArgs,
             UErrorCode *pErrorCode) {
    (व्योम)pArgs;
    अगर(UCNV_GET_VERSION(cnv)<=1) {
        _UTF16BEReset(cnv, UCNV_RESET_BOTH);
    } अन्यथा {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

अटल स्थिर अक्षर *  U_CALLCONV
_UTF16BEGetName(स्थिर UConverter *cnv) {
    अगर(UCNV_GET_VERSION(cnv)==0) {
        वापस "UTF-16BE";
    } अन्यथा {
        वापस "UTF-16BE,version=1";
    }
}
U_CDECL_END

अटल स्थिर UConverterImpl _UTF16BEImpl={
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
    UTF16_BEGet_अग्र_UChar,

    nullptr,
    _UTF16BEGetName,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

अटल स्थिर UConverter_Static_तथ्य UTF16_BEStatic_तथ्य={
    माप(UConverter_Static_तथ्य),
    "UTF-16BE",
    1200, UCNV_IBM, UCNV_UTF16_BigEndian, 2, 2,
    { 0xff, 0xfd, 0, 0 },2,false,false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


स्थिर UConverter_Shared_तथ्य _UTF16BEData=
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&UTF16_BEStatic_तथ्य, &_UTF16BEImpl);

/* UTF-16LE ----------------------------------------------------------------- */
U_CDECL_BEGIN
अटल व्योम  U_CALLCONV
_UTF16LEFromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                               UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर char16_t *source;
    अक्षर *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, source_सूचकांक;
    char16_t c, trail;
    अक्षर overflow[4];

    source=pArgs->source;
    length=(int32_t)(pArgs->sourceLimit-source);
    अगर(length<=0) {
        /* no input, nothing to do */
        वापस;
    }

    cnv=pArgs->converter;

    /* write the BOM if necessary */
    अगर(cnv->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        अटल स्थिर अक्षर bom[]={ (अक्षर)0xffu, (अक्षर)0xfeu };
        ucnv_fromUWriteBytes(cnv,
                             bom, 2,
                             &pArgs->target, pArgs->targetLimit,
                             &pArgs->offsets, -1,
                             pErrorCode);
        cnv->fromUnicodeStatus=0;
    }

    target=pArgs->target;
    अगर(target >= pArgs->targetLimit) {
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
        वापस;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;
    source_सूचकांक=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    अगर((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*source) && targetCapacity>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++source;
        --length;
        target[0]=(uint8_t)c;
        target[1]=(uint8_t)(c>>8);
        target[2]=(uint8_t)trail;
        target[3]=(uint8_t)(trail>>8);
        target+=4;
        targetCapacity-=4;
        अगर(offsets!=nullptr) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        source_सूचकांक=1;
        cnv->fromUChar32=c=0;
    }

    अगर(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        अगर(count>targetCapacity) {
            count=targetCapacity&~1;
        }
        /* count is even */
        targetCapacity-=count;
        count>>=1;
        length-=count;

        अगर(offsets==nullptr) {
            जबतक(count>0) {
                c=*source++;
                अगर(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target+=2;
                } अन्यथा अगर(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target[2]=(uint8_t)trail;
                    target[3]=(uint8_t)(trail>>8);
                    target+=4;
                } अन्यथा {
                    अवरोध;
                }
                --count;
            }
        } अन्यथा {
            जबतक(count>0) {
                c=*source++;
                अगर(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target+=2;
                    *offsets++=source_सूचकांक;
                    *offsets++=source_सूचकांक++;
                } अन्यथा अगर(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target[2]=(uint8_t)trail;
                    target[3]=(uint8_t)(trail>>8);
                    target+=4;
                    *offsets++=source_सूचकांक;
                    *offsets++=source_सूचकांक;
                    *offsets++=source_सूचकांक;
                    *offsets++=source_सूचकांक;
                    source_सूचकांक+=2;
                } अन्यथा {
                    अवरोध;
                }
                --count;
            }
        }

        अगर(count==0) {
            /* done with the loop for complete UChars */
            अगर(length>0 && targetCapacity>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                अगर(U16_IS_SINGLE(c=*source++)) {
                    overflow[0]=(अक्षर)c;
                    overflow[1]=(अक्षर)(c>>8);
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } अन्यथा {
                length=0;
                c=0;
            }
        } अन्यथा {
            /* keep c for surrogate handling, length will be set there */
            targetCapacity+=2*count;
        }
    } अन्यथा {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    अगर(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        अगर(U16_IS_SURROGATE_LEAD(c)) {
            अगर(source<pArgs->sourceLimit) {
                अगर(U16_IS_TRAIL(trail=*source)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++source;
                    overflow[0]=(अक्षर)c;
                    overflow[1]=(अक्षर)(c>>8);
                    overflow[2]=(अक्षर)trail;
                    overflow[3]=(अक्षर)(trail>>8);
                    length=4; /* 4 bytes to output */
                    c=0;
                } अन्यथा {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } अन्यथा {
                /* see if the trail surrogate is in the next buffer */
            }
        } अन्यथा {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
        cnv->fromUChar32=c;
    }

    अगर(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_fromUWriteBytes(cnv,
                             overflow, length,
                             &target, pArgs->targetLimit,
                             &offsets, source_सूचकांक,
                             pErrorCode);
        targetCapacity = static_cast<uint32_t>(pArgs->targetLimit - target);
    }

    अगर(U_SUCCESS(*pErrorCode) && source<pArgs->sourceLimit && targetCapacity==0) {
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

अटल व्योम  U_CALLCONV
_UTF16LEToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                             UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर uint8_t *source;
    char16_t *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, count, source_सूचकांक;
    char16_t c, trail;

    अगर(pArgs->converter->mode<8) {
        _UTF16ToUnicodeWithOffsets(pArgs, pErrorCode);
        वापस;
    }

    cnv=pArgs->converter;
    source=(स्थिर uint8_t *)pArgs->source;
    length=(int32_t)((स्थिर uint8_t *)pArgs->sourceLimit-source);
    अगर(length<=0 && cnv->toUnicodeStatus==0) {
        /* no input, nothing to do */
        वापस;
    }

    target=pArgs->target;
    अगर(target >= pArgs->targetLimit) {
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
        वापस;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;
    source_सूचकांक=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    अगर(cnv->toUnicodeStatus!=0) {
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
    अगर((count=cnv->toULength)!=0) {
        uint8_t *p=cnv->toUBytes;
        करो {
            p[count++]=*source++;
            ++source_सूचकांक;
            --length;
            अगर(count==2) {
                c=((char16_t)p[1]<<8)|p[0];
                अगर(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *target++=c;
                    अगर(offsets!=nullptr) {
                        *offsets++=-1;
                    }
                    --targetCapacity;
                    count=0;
                    c=0;
                    अवरोध;
                } अन्यथा अगर(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } अन्यथा {
                    /* fall through to error handling for an unmatched trail surrogate */
                    अवरोध;
                }
            } अन्यथा अगर(count==4) {
                c=((char16_t)p[1]<<8)|p[0];
                trail=((char16_t)p[3]<<8)|p[2];
                अगर(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *target++=c;
                    अगर(targetCapacity>=2) {
                        *target++=trail;
                        अगर(offsets!=nullptr) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        targetCapacity-=2;
                    } अन्यथा /* targetCapacity==1 */ {
                        targetCapacity=0;
                        cnv->UChar_Error_बफर[0]=trail;
                        cnv->UChar_Error_बफर_लंबाई=1;
                        *pErrorCode=U_बफर_OVERFLOW_ERROR;
                    }
                    count=0;
                    c=0;
                    अवरोध;
                } अन्यथा {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;

                    /* back out reading the code unit after it */
                    अगर(((स्थिर uint8_t *)pArgs->source-source)>=2) {
                        source-=2;
                    } अन्यथा {
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
                    pArgs->source=(स्थिर अक्षर *)source;
                    pArgs->target=target;
                    pArgs->offsets=offsets;
                    वापस;
                }
            }
        } जबतक(length>0);
        cnv->toULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*targetCapacity;
    अगर(count>length) {
        count=length&~1;
    }
    अगर(c==0 && count>0) {
        length-=count;
        count>>=1;
        targetCapacity-=count;
        अगर(offsets==nullptr) {
            करो {
                c=((char16_t)source[1]<<8)|source[0];
                source+=2;
                अगर(U16_IS_SINGLE(c)) {
                    *target++=c;
                } अन्यथा अगर(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                } अन्यथा {
                    अवरोध;
                }
            } जबतक(--count>0);
        } अन्यथा {
            करो {
                c=((char16_t)source[1]<<8)|source[0];
                source+=2;
                अगर(U16_IS_SINGLE(c)) {
                    *target++=c;
                    *offsets++=source_सूचकांक;
                    source_सूचकांक+=2;
                } अन्यथा अगर(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                    *offsets++=source_सूचकांक;
                    *offsets++=source_सूचकांक;
                    source_सूचकांक+=4;
                } अन्यथा {
                    अवरोध;
                }
            } जबतक(--count>0);
        }

        अगर(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } अन्यथा {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            targetCapacity+=count;
        }
    }

    अगर(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->toUBytes[0]=(uint8_t)c;
        cnv->toUBytes[1]=(uint8_t)(c>>8);
        cnv->toULength=2;

        अगर(U16_IS_SURROGATE_LEAD(c)) {
            अगर(length>=2) {
                अगर(U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    source+=2;
                    length-=2;
                    *target++=c;
                    अगर(offsets!=nullptr) {
                        *offsets++=source_सूचकांक;
                    }
                    cnv->UChar_Error_बफर[0]=trail;
                    cnv->UChar_Error_बफर_लंबाई=1;
                    cnv->toULength=0;
                    *pErrorCode=U_बफर_OVERFLOW_ERROR;
                } अन्यथा {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } अन्यथा {
                /* see if the trail surrogate is in the next buffer */
            }
        } अन्यथा {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
    }

    अगर(U_SUCCESS(*pErrorCode)) {
        /* check for a remaining source byte */
        अगर(length>0) {
            अगर(targetCapacity==0) {
                *pErrorCode=U_बफर_OVERFLOW_ERROR;
            } अन्यथा {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->toUBytes[cnv->toULength++]=*source++;
            }
        }
    }

    /* write back the updated pointers */
    pArgs->source=(स्थिर अक्षर *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

अटल UChar32  U_CALLCONV
UTF16_LEGet_अग्र_UChar(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    स्थिर uint8_t *s, *sourceLimit;
    UChar32 c;

    अगर(pArgs->converter->mode<8) {
        वापस UCNV_GET_अग्र_UCHAR_USE_TO_U;
    }

    s=(स्थिर uint8_t *)pArgs->source;
    sourceLimit=(स्थिर uint8_t *)pArgs->sourceLimit;

    अगर(s>=sourceLimit) {
        /* no input */
        *err=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0xffff;
    }

    अगर(s+2>sourceLimit) {
        /* only one byte: truncated char16_t */
        pArgs->converter->toUBytes[0]=*s++;
        pArgs->converter->toULength=1;
        pArgs->source=(स्थिर अक्षर *)s;
        *err = U_TRUNCATED_CHAR_FOUND;
        वापस 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)s[1]<<8)|*s;
    s+=2;

    /* check for a surrogate pair */
    अगर(U_IS_SURROGATE(c)) {
        अगर(U16_IS_SURROGATE_LEAD(c)) {
            अगर(s+2<=sourceLimit) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)s[1]<<8)|*s;
                अगर(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } अन्यथा {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } अन्यथा {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *bytes=pArgs->converter->toUBytes;
                s-=2;
                pArgs->converter->toULength=(int8_t)(sourceLimit-s);
                करो {
                    *bytes++=*s++;
                } जबतक(s<sourceLimit);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_FOUND;
            }
        } अन्यथा {
            /* unmatched trail surrogate */
            c=-2;
        }

        अगर(c<0) {
            /* write the unmatched surrogate */
            uint8_t *bytes=pArgs->converter->toUBytes;
            pArgs->converter->toULength=2;
            *bytes=*(s-2);
            bytes[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_FOUND;
        }
    }

    pArgs->source=(स्थिर अक्षर *)s;
    वापस c;
} 

अटल व्योम  U_CALLCONV
_UTF16LEReset(UConverter *cnv, UConverterResetChoice choice) {
    अगर(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode state */
        अगर(UCNV_GET_VERSION(cnv)==0) {
            cnv->mode=8; /* no BOM handling */
        } अन्यथा {
            cnv->mode=0; /* Java-specific "UnicodeLittle" requires LE BOM or no BOM */
        }
    }
    अगर(choice!=UCNV_RESET_TO_UNICODE && UCNV_GET_VERSION(cnv)==1) {
        /* reset fromUnicode for "UnicodeLittle": prepare to output the UTF-16LE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

अटल व्योम  U_CALLCONV
_UTF16LEOpen(UConverter *cnv,
             UConverterLoadArgs *pArgs,
             UErrorCode *pErrorCode) {
    (व्योम)pArgs;
    अगर(UCNV_GET_VERSION(cnv)<=1) {
        _UTF16LEReset(cnv, UCNV_RESET_BOTH);
    } अन्यथा {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

अटल स्थिर अक्षर *  U_CALLCONV
_UTF16LEGetName(स्थिर UConverter *cnv) {
    अगर(UCNV_GET_VERSION(cnv)==0) {
        वापस "UTF-16LE";
    } अन्यथा {
        वापस "UTF-16LE,version=1";
    }
}
U_CDECL_END

अटल स्थिर UConverterImpl _UTF16LEImpl={
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
    UTF16_LEGet_अग्र_UChar,

    nullptr,
    _UTF16LEGetName,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};


अटल स्थिर UConverter_Static_तथ्य UTF16_LEStatic_तथ्य={
    माप(UConverter_Static_तथ्य),
    "UTF-16LE",
    1202, UCNV_IBM, UCNV_UTF16_LittleEndian, 2, 2,
    { 0xfd, 0xff, 0, 0 },2,false,false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


स्थिर UConverter_Shared_तथ्य _UTF16LEData=
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&UTF16_LEStatic_तथ्य, &_UTF16LEImpl);

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
अटल व्योम  U_CALLCONV
_UTF16Reset(UConverter *cnv, UConverterResetChoice choice) {
    अगर(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->mode=0;
    }
    अगर(choice!=UCNV_RESET_TO_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-16PE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}
U_CDECL_END
बाह्य स्थिर UConverter_Shared_तथ्य UTF16v2_तथ्य;
U_CDECL_BEGIN
अटल व्योम U_CALLCONV
_UTF16Open(UConverter *cnv,
           UConverterLoadArgs *pArgs,
           UErrorCode *pErrorCode) {
    अगर(UCNV_GET_VERSION(cnv)<=2) {
        अगर(UCNV_GET_VERSION(cnv)==2 && !pArgs->onlyTestIsLoadable) {
            /*
             * Switch implementation, and switch the staticData that's different
             * and was copied into the UConverter.
             * (See ucnv_createConverterFromSharedData() in ucnv_bld.c.)
             * UTF-16,version=2 fromUnicode() always writes a big-endian byte stream.
             */
            cnv->shared_तथ्य=(UConverter_Shared_तथ्य*)&UTF16v2_तथ्य;
            uprv_memcpy(cnv->subChars, UTF16v2_तथ्य.static_तथ्य->subChar, UCNV_MAX_SUBCHAR_LEN);
        }
        _UTF16Reset(cnv, UCNV_RESET_BOTH);
    } अन्यथा {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

अटल स्थिर अक्षर *  U_CALLCONV
_UTF16GetName(स्थिर UConverter *cnv) {
    अगर(UCNV_GET_VERSION(cnv)==0) {
        वापस "UTF-16";
    } अन्यथा अगर(UCNV_GET_VERSION(cnv)==1) {
        वापस "UTF-16,version=1";
    } अन्यथा {
        वापस "UTF-16,version=2";
    }
}
U_CDECL_END
बाह्य स्थिर UConverter_Shared_तथ्य UTF16_तथ्य;

अटल अंतरभूत bool IS_UTF16BE(स्थिर UConverter *cnv) {
    वापस ((cnv)->shared_तथ्य == &_UTF16BEData);
}

अटल अंतरभूत bool IS_UTF16LE(स्थिर UConverter *cnv) {
    वापस ((cnv)->shared_तथ्य == &_UTF16LEData);
}

अटल अंतरभूत bool IS_UTF16(स्थिर UConverter *cnv) {
    वापस ((cnv)->shared_तथ्य==&UTF16_तथ्य) || ((cnv)->shared_तथ्य == &UTF16v2_तथ्य);
}

U_CDECL_BEGIN
अटल व्योम U_CALLCONV
_UTF16ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    स्थिर अक्षर *source=pArgs->source;
    स्थिर अक्षर *sourceLimit=pArgs->sourceLimit;
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

    जबतक(source<sourceLimit && U_SUCCESS(*pErrorCode)) {
        चयन(state) {
        हाल 0:
            cnv->toUBytes[0]=(uint8_t)*source++;
            cnv->toULength=1;
            state=1;
            अवरोध;
        हाल 1:
            /*
             * Only inside this switch case can the state variable
             * temporarily take two additional values:
             * 6: BOM error, continue with BE
             * 7: BOM error, continue with LE
             */
            b=*source;
            अगर(cnv->toUBytes[0]==0xfe && b==0xff) {
                अगर(IS_UTF16LE(cnv)) {
                    state=7; /* illegal reverse BOM for Java "UnicodeLittle" */
                } अन्यथा {
                    state=8; /* detect UTF-16BE */
                }
            } अन्यथा अगर(cnv->toUBytes[0]==0xff && b==0xfe) {
                अगर(IS_UTF16BE(cnv)) {
                    state=6; /* illegal reverse BOM for Java "UnicodeBig" */
                } अन्यथा {
                    state=9; /* detect UTF-16LE */
                }
            } अन्यथा अगर((IS_UTF16(cnv) && UCNV_GET_VERSION(cnv)==1)) {
                state=6; /* illegal missing BOM for Java "Unicode" */
            }
            अगर(state>=8) {
                /* BOM detected, consume it */
                ++source;
                cnv->toULength=0;
                offsetDelta=(int32_t)(source-pArgs->source);
            } अन्यथा अगर(state<6) {
                /* ok: no BOM, and not a reverse BOM */
                अगर(source!=pArgs->source) {
                    /* reset the source for a correct first offset */
                    source=pArgs->source;
                    cnv->toULength=0;
                }
                अगर(IS_UTF16LE(cnv)) {
                    /* Make Java "UnicodeLittle" default to LE. */
                    state=9;
                } अन्यथा {
                    /* Make standard UTF-16 and Java "UnicodeBig" default to BE. */
                    state=8;
                }
            } अन्यथा {
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
                वापस;
            }
            /* convert the rest of the stream */
            cnv->mode=state;
            जारी;
        हाल 8:
            /* call UTF-16BE */
            pArgs->source=source;
            _UTF16BEToUnicodeWithOffsets(pArgs, pErrorCode);
            source=pArgs->source;
            अवरोध;
        हाल 9:
            /* call UTF-16LE */
            pArgs->source=source;
            _UTF16LEToUnicodeWithOffsets(pArgs, pErrorCode);
            source=pArgs->source;
            अवरोध;
        शेष:
            अवरोध; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    अगर(offsets!=nullptr && offsetDelta!=0) {
        int32_t *offsetsLimit=pArgs->offsets;
        जबतक(offsets<offsetsLimit) {
            *offsets++ += offsetDelta;
        }
    }

    pArgs->source=source;

    अगर(source==sourceLimit && pArgs->सफाई) {
        /* handle truncated input */
        चयन(state) {
        हाल 0:
            अवरोध; /* no input at all, nothing to do */
        हाल 8:
            _UTF16BEToUnicodeWithOffsets(pArgs, pErrorCode);
            अवरोध;
        हाल 9:
            _UTF16LEToUnicodeWithOffsets(pArgs, pErrorCode);
            अवरोध;
        शेष:
            /* 0<state<8: framework will report truncation, nothing to do here */
            अवरोध;
        }
    }

    cnv->mode=state;
}

अटल UChar32 U_CALLCONV
UTF16_Get_अग्र_UChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    चयन(pArgs->converter->mode) {
    हाल 8:
        वापस UTF16_BEGet_अग्र_UChar(pArgs, pErrorCode);
    हाल 9:
        वापस UTF16_LEGet_अग्र_UChar(pArgs, pErrorCode);
    शेष:
        वापस UCNV_GET_अग्र_UCHAR_USE_TO_U;
    }
}
U_CDECL_END

अटल स्थिर UConverterImpl _UTF16Impl = {
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
    UTF16_Get_अग्र_UChar,

    nullptr, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

अटल स्थिर UConverter_Static_तथ्य UTF16_Static_तथ्य = {
    माप(UConverter_Static_तथ्य),
    "UTF-16",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
#अगर U_IS_BIG_ENDIAN
    { 0xff, 0xfd, 0, 0 }, 2,
#अन्यथा
    { 0xfd, 0xff, 0, 0 }, 2,
#पूर्ण_अगर
    false, false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

स्थिर UConverter_Shared_तथ्य UTF16_तथ्य =
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&UTF16_Static_तथ्य, &_UTF16Impl);

अटल स्थिर UConverterImpl _UTF16v2Impl = {
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
    UTF16_Get_अग्र_UChar,

    nullptr, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

अटल स्थिर UConverter_Static_तथ्य UTF16v2_Static_तथ्य = {
    माप(UConverter_Static_तथ्य),
    "UTF-16,version=2",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
    { 0xff, 0xfd, 0, 0 }, 2,
    false, false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

स्थिर UConverter_Shared_तथ्य UTF16v2_तथ्य =
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&UTF16v2_Static_तथ्य, &_UTF16v2Impl);

#पूर्ण_अगर
