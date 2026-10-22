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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "unicode/uversion.h"
#অন্তর্ভাব "ucnv_bld.h"
#অন্তর্ভাব "ucnv_cnv.h"
#অন্তর্ভাব "cmemory.h"

গণনা {
    UCNV_NEED_TO_WRITE_BOM=1
};

U_CDECL_BEGIN
/*
 * The UTF-16 toUnicode implementation is also used for the Java-specific
 * "with BOM" variants of UTF-16BE and UTF-16LE.
 */
স্থির শূন্য  U_CALLCONV
_UTF16ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode);

/* UTF-16BE ----------------------------------------------------------------- */

#যদি U_IS_BIG_ENDIAN
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16BEFromUnicodeWithOffsets
#অন্যথা
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16LEFromUnicodeWithOffsets
#endif


স্থির শূন্য  U_CALLCONV
_UTF16BEFromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                               UErrorCode *pErrorCode) {
    UConverter *cnv;
    ধ্রুবক char16_t *source;
    অক্ষর *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, source_সূচক;
    char16_t c, trail;
    অক্ষর overflow[4];

    source=pArgs->source;
    length=(int32_t)(pArgs->sourceLimit-source);
    যদি(length<=0) {
        /* no input, nothing to do */
        ফেরত;
    }

    cnv=pArgs->converter;

    /* write the BOM if necessary */
    যদি(cnv->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        স্থির ধ্রুবক অক্ষর bom[]={ (অক্ষর)0xfeu, (অক্ষর)0xffu };
        ucnv_fromUWriteBytes(cnv,
                             bom, 2,
                             &pArgs->target, pArgs->targetLimit,
                             &pArgs->offsets, -1,
                             pErrorCode);
        cnv->fromUnicodeStatus=0;
    }

    target=pArgs->target;
    যদি(target >= pArgs->targetLimit) {
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
        ফেরত;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-target);
    offsets=pArgs->offsets;
    source_সূচক=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    যদি((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*source) && targetCapacity>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++source;
        --length;
        target[0]=(uint8_t)(c>>8);
        target[1]=(uint8_t)c;
        target[2]=(uint8_t)(trail>>8);
        target[3]=(uint8_t)trail;
        target+=4;
        targetCapacity-=4;
        যদি(offsets!=nullptr) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        source_সূচক=1;
        cnv->fromUChar32=c=0;
    }

    যদি(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        যদি(count>targetCapacity) {
            count=targetCapacity&~1;
        }
        /* count is even */
        targetCapacity-=count;
        count>>=1;
        length-=count;

        যদি(offsets==nullptr) {
            যখন(count>0) {
                c=*source++;
                যদি(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target+=2;
                } অন্যথা যদি(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target[2]=(uint8_t)(trail>>8);
                    target[3]=(uint8_t)trail;
                    target+=4;
                } অন্যথা {
                    বিরতি;
                }
                --count;
            }
        } অন্যথা {
            যখন(count>0) {
                c=*source++;
                যদি(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target+=2;
                    *offsets++=source_সূচক;
                    *offsets++=source_সূচক++;
                } অন্যথা যদি(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target[2]=(uint8_t)(trail>>8);
                    target[3]=(uint8_t)trail;
                    target+=4;
                    *offsets++=source_সূচক;
                    *offsets++=source_সূচক;
                    *offsets++=source_সূচক;
                    *offsets++=source_সূচক;
                    source_সূচক+=2;
                } অন্যথা {
                    বিরতি;
                }
                --count;
            }
        }

        যদি(count==0) {
            /* done with the loop for complete UChars */
            যদি(length>0 && targetCapacity>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                যদি(U16_IS_SINGLE(c=*source++)) {
                    overflow[0]=(অক্ষর)(c>>8);
                    overflow[1]=(অক্ষর)c;
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } অন্যথা {
                length=0;
                c=0;
            }
        } অন্যথা {
            /* keep c for surrogate handling, length will be set there */
            targetCapacity+=2*count;
        }
    } অন্যথা {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    যদি(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        যদি(U16_IS_SURROGATE_LEAD(c)) {
            যদি(source<pArgs->sourceLimit) {
                যদি(U16_IS_TRAIL(trail=*source)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++source;
                    overflow[0]=(অক্ষর)(c>>8);
                    overflow[1]=(অক্ষর)c;
                    overflow[2]=(অক্ষর)(trail>>8);
                    overflow[3]=(অক্ষর)trail;
                    length=4; /* 4 bytes to output */
                    c=0;
                } অন্যথা {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } অন্যথা {
                /* see if the trail surrogate is in the next buffer */
            }
        } অন্যথা {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
        cnv->fromUChar32=c;
    }

    যদি(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_fromUWriteBytes(cnv,
                             overflow, length,
                             &target, pArgs->targetLimit,
                             &offsets, source_সূচক,
                             pErrorCode);
        targetCapacity = static_cast<uint32_t>(pArgs->targetLimit - target);
    }

    যদি(U_SUCCESS(*pErrorCode) && source<pArgs->sourceLimit && targetCapacity==0) {
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target = target;
    pArgs->offsets=offsets;
}

স্থির শূন্য  U_CALLCONV
_UTF16BEToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                             UErrorCode *pErrorCode) {
    UConverter *cnv;
    ধ্রুবক uint8_t *source;
    char16_t *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, count, source_সূচক;
    char16_t c, trail;

    যদি(pArgs->converter->mode<8) {
        _UTF16ToUnicodeWithOffsets(pArgs, pErrorCode);
        ফেরত;
    }

    cnv=pArgs->converter;
    source=(ধ্রুবক uint8_t *)pArgs->source;
    length=(int32_t)((ধ্রুবক uint8_t *)pArgs->sourceLimit-source);
    যদি(length<=0 && cnv->toUnicodeStatus==0) {
        /* no input, nothing to do */
        ফেরত;
    }

    target=pArgs->target;
    যদি(target >= pArgs->targetLimit) {
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
        ফেরত;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-target);
    offsets=pArgs->offsets;
    source_সূচক=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    যদি(cnv->toUnicodeStatus!=0) {
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
    যদি((count=cnv->toULength)!=0) {
        uint8_t *p=cnv->toUBytes;
        করো {
            p[count++]=*source++;
            ++source_সূচক;
            --length;
            যদি(count==2) {
                c=((char16_t)p[0]<<8)|p[1];
                যদি(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *target++=c;
                    যদি(offsets!=nullptr) {
                        *offsets++=-1;
                    }
                    --targetCapacity;
                    count=0;
                    c=0;
                    বিরতি;
                } অন্যথা যদি(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } অন্যথা {
                    /* fall through to error handling for an unmatched trail surrogate */
                    বিরতি;
                }
            } অন্যথা যদি(count==4) {
                c=((char16_t)p[0]<<8)|p[1];
                trail=((char16_t)p[2]<<8)|p[3];
                যদি(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *target++=c;
                    যদি(targetCapacity>=2) {
                        *target++=trail;
                        যদি(offsets!=nullptr) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        targetCapacity-=2;
                    } অন্যথা /* targetCapacity==1 */ {
                        targetCapacity=0;
                        cnv->UChar_Error_বাফার[0]=trail;
                        cnv->UChar_Error_বাফার_দৈর্ঘ্য=1;
                        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
                    }
                    count=0;
                    c=0;
                    বিরতি;
                } অন্যথা {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;

                    /* back out reading the code unit after it */
                    যদি(((ধ্রুবক uint8_t *)pArgs->source-source)>=2) {
                        source-=2;
                    } অন্যথা {
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
                    pArgs->source=(ধ্রুবক অক্ষর *)source;
                    pArgs->target=target;
                    pArgs->offsets=offsets;
                    ফেরত;
                }
            }
        } যখন(length>0);
        cnv->toULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*targetCapacity;
    যদি(count>length) {
        count=length&~1;
    }
    যদি(c==0 && count>0) {
        length-=count;
        count>>=1;
        targetCapacity-=count;
        যদি(offsets==nullptr) {
            করো {
                c=((char16_t)source[0]<<8)|source[1];
                source+=2;
                যদি(U16_IS_SINGLE(c)) {
                    *target++=c;
                } অন্যথা যদি(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                } অন্যথা {
                    বিরতি;
                }
            } যখন(--count>0);
        } অন্যথা {
            করো {
                c=((char16_t)source[0]<<8)|source[1];
                source+=2;
                যদি(U16_IS_SINGLE(c)) {
                    *target++=c;
                    *offsets++=source_সূচক;
                    source_সূচক+=2;
                } অন্যথা যদি(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                    *offsets++=source_সূচক;
                    *offsets++=source_সূচক;
                    source_সূচক+=4;
                } অন্যথা {
                    বিরতি;
                }
            } যখন(--count>0);
        }

        যদি(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } অন্যথা {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            targetCapacity+=count;
        }
    }

    যদি(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->toUBytes[0]=(uint8_t)(c>>8);
        cnv->toUBytes[1]=(uint8_t)c;
        cnv->toULength=2;

        যদি(U16_IS_SURROGATE_LEAD(c)) {
            যদি(length>=2) {
                যদি(U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    source+=2;
                    length-=2;
                    *target++=c;
                    যদি(offsets!=nullptr) {
                        *offsets++=source_সূচক;
                    }
                    cnv->UChar_Error_বাফার[0]=trail;
                    cnv->UChar_Error_বাফার_দৈর্ঘ্য=1;
                    cnv->toULength=0;
                    *pErrorCode=U_বাফার_OVERFLOW_ERROR;
                } অন্যথা {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } অন্যথা {
                /* see if the trail surrogate is in the next buffer */
            }
        } অন্যথা {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
    }

    যদি(U_SUCCESS(*pErrorCode)) {
        /* check for a remaining source byte */
        যদি(length>0) {
            যদি(targetCapacity==0) {
                *pErrorCode=U_বাফার_OVERFLOW_ERROR;
            } অন্যথা {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->toUBytes[cnv->toULength++]=*source++;
            }
        }
    }

    /* write back the updated pointers */
    pArgs->source=(ধ্রুবক অক্ষর *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

স্থির UChar32  U_CALLCONV
UTF16_BEGet_পরবর্তী_UChar(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    ধ্রুবক uint8_t *s, *sourceLimit;
    UChar32 c;

    যদি(pArgs->converter->mode<8) {
        ফেরত UCNV_GET_পরবর্তী_UCHAR_USE_TO_U;
    }

    s=(ধ্রুবক uint8_t *)pArgs->source;
    sourceLimit=(ধ্রুবক uint8_t *)pArgs->sourceLimit;

    যদি(s>=sourceLimit) {
        /* no input */
        *err=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0xffff;
    }

    যদি(s+2>sourceLimit) {
        /* only one byte: truncated char16_t */
        pArgs->converter->toUBytes[0]=*s++;
        pArgs->converter->toULength=1;
        pArgs->source=(ধ্রুবক অক্ষর *)s;
        *err = U_TRUNCATED_CHAR_FOUND;
        ফেরত 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)*s<<8)|s[1];
    s+=2;

    /* check for a surrogate pair */
    যদি(U_IS_SURROGATE(c)) {
        যদি(U16_IS_SURROGATE_LEAD(c)) {
            যদি(s+2<=sourceLimit) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)*s<<8)|s[1];
                যদি(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } অন্যথা {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } অন্যথা {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *bytes=pArgs->converter->toUBytes;
                s-=2;
                pArgs->converter->toULength=(int8_t)(sourceLimit-s);
                করো {
                    *bytes++=*s++;
                } যখন(s<sourceLimit);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_FOUND;
            }
        } অন্যথা {
            /* unmatched trail surrogate */
            c=-2;
        }

        যদি(c<0) {
            /* write the unmatched surrogate */
            uint8_t *bytes=pArgs->converter->toUBytes;
            pArgs->converter->toULength=2;
            *bytes=*(s-2);
            bytes[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_FOUND;
        }
    }

    pArgs->source=(ধ্রুবক অক্ষর *)s;
    ফেরত c;
} 

স্থির শূন্য  U_CALLCONV
_UTF16BEReset(UConverter *cnv, UConverterResetChoice choice) {
    যদি(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode state */
        যদি(UCNV_GET_VERSION(cnv)==0) {
            cnv->mode=8; /* no BOM handling */
        } অন্যথা {
            cnv->mode=0; /* Java-specific "UnicodeBig" requires BE BOM or no BOM */
        }
    }
    যদি(choice!=UCNV_RESET_TO_UNICODE && UCNV_GET_VERSION(cnv)==1) {
        /* reset fromUnicode for "UnicodeBig": prepare to output the UTF-16BE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

স্থির শূন্য  U_CALLCONV
_UTF16BEOpen(UConverter *cnv,
             UConverterLoadArgs *pArgs,
             UErrorCode *pErrorCode) {
    (শূন্য)pArgs;
    যদি(UCNV_GET_VERSION(cnv)<=1) {
        _UTF16BEReset(cnv, UCNV_RESET_BOTH);
    } অন্যথা {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

স্থির ধ্রুবক অক্ষর *  U_CALLCONV
_UTF16BEGetName(ধ্রুবক UConverter *cnv) {
    যদি(UCNV_GET_VERSION(cnv)==0) {
        ফেরত "UTF-16BE";
    } অন্যথা {
        ফেরত "UTF-16BE,version=1";
    }
}
U_CDECL_END

স্থির ধ্রুবক UConverterImpl _UTF16BEImpl={
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
    UTF16_BEGet_পরবর্তী_UChar,

    nullptr,
    _UTF16BEGetName,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

স্থির ধ্রুবক UConverter_Static_তথ্য UTF16_BEStatic_তথ্য={
    আকার(UConverter_Static_তথ্য),
    "UTF-16BE",
    1200, UCNV_IBM, UCNV_UTF16_BigEndian, 2, 2,
    { 0xff, 0xfd, 0, 0 },2,মিথ্যা,মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ধ্রুবক UConverter_Shared_তথ্য _UTF16BEData=
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&UTF16_BEStatic_তথ্য, &_UTF16BEImpl);

/* UTF-16LE ----------------------------------------------------------------- */
U_CDECL_BEGIN
স্থির শূন্য  U_CALLCONV
_UTF16LEFromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                               UErrorCode *pErrorCode) {
    UConverter *cnv;
    ধ্রুবক char16_t *source;
    অক্ষর *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, source_সূচক;
    char16_t c, trail;
    অক্ষর overflow[4];

    source=pArgs->source;
    length=(int32_t)(pArgs->sourceLimit-source);
    যদি(length<=0) {
        /* no input, nothing to do */
        ফেরত;
    }

    cnv=pArgs->converter;

    /* write the BOM if necessary */
    যদি(cnv->fromUnicodeStatus==UCNV_NEED_TO_WRITE_BOM) {
        স্থির ধ্রুবক অক্ষর bom[]={ (অক্ষর)0xffu, (অক্ষর)0xfeu };
        ucnv_fromUWriteBytes(cnv,
                             bom, 2,
                             &pArgs->target, pArgs->targetLimit,
                             &pArgs->offsets, -1,
                             pErrorCode);
        cnv->fromUnicodeStatus=0;
    }

    target=pArgs->target;
    যদি(target >= pArgs->targetLimit) {
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
        ফেরত;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;
    source_সূচক=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    যদি((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*source) && targetCapacity>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++source;
        --length;
        target[0]=(uint8_t)c;
        target[1]=(uint8_t)(c>>8);
        target[2]=(uint8_t)trail;
        target[3]=(uint8_t)(trail>>8);
        target+=4;
        targetCapacity-=4;
        যদি(offsets!=nullptr) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        source_সূচক=1;
        cnv->fromUChar32=c=0;
    }

    যদি(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        যদি(count>targetCapacity) {
            count=targetCapacity&~1;
        }
        /* count is even */
        targetCapacity-=count;
        count>>=1;
        length-=count;

        যদি(offsets==nullptr) {
            যখন(count>0) {
                c=*source++;
                যদি(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target+=2;
                } অন্যথা যদি(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target[2]=(uint8_t)trail;
                    target[3]=(uint8_t)(trail>>8);
                    target+=4;
                } অন্যথা {
                    বিরতি;
                }
                --count;
            }
        } অন্যথা {
            যখন(count>0) {
                c=*source++;
                যদি(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target+=2;
                    *offsets++=source_সূচক;
                    *offsets++=source_সূচক++;
                } অন্যথা যদি(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target[2]=(uint8_t)trail;
                    target[3]=(uint8_t)(trail>>8);
                    target+=4;
                    *offsets++=source_সূচক;
                    *offsets++=source_সূচক;
                    *offsets++=source_সূচক;
                    *offsets++=source_সূচক;
                    source_সূচক+=2;
                } অন্যথা {
                    বিরতি;
                }
                --count;
            }
        }

        যদি(count==0) {
            /* done with the loop for complete UChars */
            যদি(length>0 && targetCapacity>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                যদি(U16_IS_SINGLE(c=*source++)) {
                    overflow[0]=(অক্ষর)c;
                    overflow[1]=(অক্ষর)(c>>8);
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } অন্যথা {
                length=0;
                c=0;
            }
        } অন্যথা {
            /* keep c for surrogate handling, length will be set there */
            targetCapacity+=2*count;
        }
    } অন্যথা {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    যদি(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        যদি(U16_IS_SURROGATE_LEAD(c)) {
            যদি(source<pArgs->sourceLimit) {
                যদি(U16_IS_TRAIL(trail=*source)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++source;
                    overflow[0]=(অক্ষর)c;
                    overflow[1]=(অক্ষর)(c>>8);
                    overflow[2]=(অক্ষর)trail;
                    overflow[3]=(অক্ষর)(trail>>8);
                    length=4; /* 4 bytes to output */
                    c=0;
                } অন্যথা {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } অন্যথা {
                /* see if the trail surrogate is in the next buffer */
            }
        } অন্যথা {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
        cnv->fromUChar32=c;
    }

    যদি(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_fromUWriteBytes(cnv,
                             overflow, length,
                             &target, pArgs->targetLimit,
                             &offsets, source_সূচক,
                             pErrorCode);
        targetCapacity = static_cast<uint32_t>(pArgs->targetLimit - target);
    }

    যদি(U_SUCCESS(*pErrorCode) && source<pArgs->sourceLimit && targetCapacity==0) {
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
    }

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

স্থির শূন্য  U_CALLCONV
_UTF16LEToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                             UErrorCode *pErrorCode) {
    UConverter *cnv;
    ধ্রুবক uint8_t *source;
    char16_t *target;
    int32_t *offsets;

    uint32_t targetCapacity, length, count, source_সূচক;
    char16_t c, trail;

    যদি(pArgs->converter->mode<8) {
        _UTF16ToUnicodeWithOffsets(pArgs, pErrorCode);
        ফেরত;
    }

    cnv=pArgs->converter;
    source=(ধ্রুবক uint8_t *)pArgs->source;
    length=(int32_t)((ধ্রুবক uint8_t *)pArgs->sourceLimit-source);
    যদি(length<=0 && cnv->toUnicodeStatus==0) {
        /* no input, nothing to do */
        ফেরত;
    }

    target=pArgs->target;
    যদি(target >= pArgs->targetLimit) {
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
        ফেরত;
    }

    targetCapacity=(uint32_t)(pArgs->targetLimit-pArgs->target);
    offsets=pArgs->offsets;
    source_সূচক=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    যদি(cnv->toUnicodeStatus!=0) {
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
    যদি((count=cnv->toULength)!=0) {
        uint8_t *p=cnv->toUBytes;
        করো {
            p[count++]=*source++;
            ++source_সূচক;
            --length;
            যদি(count==2) {
                c=((char16_t)p[1]<<8)|p[0];
                যদি(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *target++=c;
                    যদি(offsets!=nullptr) {
                        *offsets++=-1;
                    }
                    --targetCapacity;
                    count=0;
                    c=0;
                    বিরতি;
                } অন্যথা যদি(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } অন্যথা {
                    /* fall through to error handling for an unmatched trail surrogate */
                    বিরতি;
                }
            } অন্যথা যদি(count==4) {
                c=((char16_t)p[1]<<8)|p[0];
                trail=((char16_t)p[3]<<8)|p[2];
                যদি(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *target++=c;
                    যদি(targetCapacity>=2) {
                        *target++=trail;
                        যদি(offsets!=nullptr) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        targetCapacity-=2;
                    } অন্যথা /* targetCapacity==1 */ {
                        targetCapacity=0;
                        cnv->UChar_Error_বাফার[0]=trail;
                        cnv->UChar_Error_বাফার_দৈর্ঘ্য=1;
                        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
                    }
                    count=0;
                    c=0;
                    বিরতি;
                } অন্যথা {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;

                    /* back out reading the code unit after it */
                    যদি(((ধ্রুবক uint8_t *)pArgs->source-source)>=2) {
                        source-=2;
                    } অন্যথা {
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
                    pArgs->source=(ধ্রুবক অক্ষর *)source;
                    pArgs->target=target;
                    pArgs->offsets=offsets;
                    ফেরত;
                }
            }
        } যখন(length>0);
        cnv->toULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*targetCapacity;
    যদি(count>length) {
        count=length&~1;
    }
    যদি(c==0 && count>0) {
        length-=count;
        count>>=1;
        targetCapacity-=count;
        যদি(offsets==nullptr) {
            করো {
                c=((char16_t)source[1]<<8)|source[0];
                source+=2;
                যদি(U16_IS_SINGLE(c)) {
                    *target++=c;
                } অন্যথা যদি(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                } অন্যথা {
                    বিরতি;
                }
            } যখন(--count>0);
        } অন্যথা {
            করো {
                c=((char16_t)source[1]<<8)|source[0];
                source+=2;
                যদি(U16_IS_SINGLE(c)) {
                    *target++=c;
                    *offsets++=source_সূচক;
                    source_সূচক+=2;
                } অন্যথা যদি(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                    *offsets++=source_সূচক;
                    *offsets++=source_সূচক;
                    source_সূচক+=4;
                } অন্যথা {
                    বিরতি;
                }
            } যখন(--count>0);
        }

        যদি(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } অন্যথা {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            targetCapacity+=count;
        }
    }

    যদি(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->toUBytes[0]=(uint8_t)c;
        cnv->toUBytes[1]=(uint8_t)(c>>8);
        cnv->toULength=2;

        যদি(U16_IS_SURROGATE_LEAD(c)) {
            যদি(length>=2) {
                যদি(U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    source+=2;
                    length-=2;
                    *target++=c;
                    যদি(offsets!=nullptr) {
                        *offsets++=source_সূচক;
                    }
                    cnv->UChar_Error_বাফার[0]=trail;
                    cnv->UChar_Error_বাফার_দৈর্ঘ্য=1;
                    cnv->toULength=0;
                    *pErrorCode=U_বাফার_OVERFLOW_ERROR;
                } অন্যথা {
                    /* unmatched lead surrogate */
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                }
            } অন্যথা {
                /* see if the trail surrogate is in the next buffer */
            }
        } অন্যথা {
            /* unmatched trail surrogate */
            *pErrorCode=U_ILLEGAL_CHAR_FOUND;
        }
    }

    যদি(U_SUCCESS(*pErrorCode)) {
        /* check for a remaining source byte */
        যদি(length>0) {
            যদি(targetCapacity==0) {
                *pErrorCode=U_বাফার_OVERFLOW_ERROR;
            } অন্যথা {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->toUBytes[cnv->toULength++]=*source++;
            }
        }
    }

    /* write back the updated pointers */
    pArgs->source=(ধ্রুবক অক্ষর *)source;
    pArgs->target=target;
    pArgs->offsets=offsets;
}

স্থির UChar32  U_CALLCONV
UTF16_LEGet_পরবর্তী_UChar(UConverterToUnicodeArgs *pArgs, UErrorCode *err) {
    ধ্রুবক uint8_t *s, *sourceLimit;
    UChar32 c;

    যদি(pArgs->converter->mode<8) {
        ফেরত UCNV_GET_পরবর্তী_UCHAR_USE_TO_U;
    }

    s=(ধ্রুবক uint8_t *)pArgs->source;
    sourceLimit=(ধ্রুবক uint8_t *)pArgs->sourceLimit;

    যদি(s>=sourceLimit) {
        /* no input */
        *err=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0xffff;
    }

    যদি(s+2>sourceLimit) {
        /* only one byte: truncated char16_t */
        pArgs->converter->toUBytes[0]=*s++;
        pArgs->converter->toULength=1;
        pArgs->source=(ধ্রুবক অক্ষর *)s;
        *err = U_TRUNCATED_CHAR_FOUND;
        ফেরত 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)s[1]<<8)|*s;
    s+=2;

    /* check for a surrogate pair */
    যদি(U_IS_SURROGATE(c)) {
        যদি(U16_IS_SURROGATE_LEAD(c)) {
            যদি(s+2<=sourceLimit) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)s[1]<<8)|*s;
                যদি(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } অন্যথা {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } অন্যথা {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *bytes=pArgs->converter->toUBytes;
                s-=2;
                pArgs->converter->toULength=(int8_t)(sourceLimit-s);
                করো {
                    *bytes++=*s++;
                } যখন(s<sourceLimit);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_FOUND;
            }
        } অন্যথা {
            /* unmatched trail surrogate */
            c=-2;
        }

        যদি(c<0) {
            /* write the unmatched surrogate */
            uint8_t *bytes=pArgs->converter->toUBytes;
            pArgs->converter->toULength=2;
            *bytes=*(s-2);
            bytes[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_FOUND;
        }
    }

    pArgs->source=(ধ্রুবক অক্ষর *)s;
    ফেরত c;
} 

স্থির শূন্য  U_CALLCONV
_UTF16LEReset(UConverter *cnv, UConverterResetChoice choice) {
    যদি(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode state */
        যদি(UCNV_GET_VERSION(cnv)==0) {
            cnv->mode=8; /* no BOM handling */
        } অন্যথা {
            cnv->mode=0; /* Java-specific "UnicodeLittle" requires LE BOM or no BOM */
        }
    }
    যদি(choice!=UCNV_RESET_TO_UNICODE && UCNV_GET_VERSION(cnv)==1) {
        /* reset fromUnicode for "UnicodeLittle": prepare to output the UTF-16LE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}

স্থির শূন্য  U_CALLCONV
_UTF16LEOpen(UConverter *cnv,
             UConverterLoadArgs *pArgs,
             UErrorCode *pErrorCode) {
    (শূন্য)pArgs;
    যদি(UCNV_GET_VERSION(cnv)<=1) {
        _UTF16LEReset(cnv, UCNV_RESET_BOTH);
    } অন্যথা {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

স্থির ধ্রুবক অক্ষর *  U_CALLCONV
_UTF16LEGetName(ধ্রুবক UConverter *cnv) {
    যদি(UCNV_GET_VERSION(cnv)==0) {
        ফেরত "UTF-16LE";
    } অন্যথা {
        ফেরত "UTF-16LE,version=1";
    }
}
U_CDECL_END

স্থির ধ্রুবক UConverterImpl _UTF16LEImpl={
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
    UTF16_LEGet_পরবর্তী_UChar,

    nullptr,
    _UTF16LEGetName,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};


স্থির ধ্রুবক UConverter_Static_তথ্য UTF16_LEStatic_তথ্য={
    আকার(UConverter_Static_তথ্য),
    "UTF-16LE",
    1202, UCNV_IBM, UCNV_UTF16_LittleEndian, 2, 2,
    { 0xfd, 0xff, 0, 0 },2,মিথ্যা,মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ধ্রুবক UConverter_Shared_তথ্য _UTF16LEData=
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&UTF16_LEStatic_তথ্য, &_UTF16LEImpl);

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
স্থির শূন্য  U_CALLCONV
_UTF16Reset(UConverter *cnv, UConverterResetChoice choice) {
    যদি(choice<=UCNV_RESET_TO_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->mode=0;
    }
    যদি(choice!=UCNV_RESET_TO_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-16PE BOM */
        cnv->fromUnicodeStatus=UCNV_NEED_TO_WRITE_BOM;
    }
}
U_CDECL_END
বাহ্যিক ধ্রুবক UConverter_Shared_তথ্য UTF16v2_তথ্য;
U_CDECL_BEGIN
স্থির শূন্য U_CALLCONV
_UTF16Open(UConverter *cnv,
           UConverterLoadArgs *pArgs,
           UErrorCode *pErrorCode) {
    যদি(UCNV_GET_VERSION(cnv)<=2) {
        যদি(UCNV_GET_VERSION(cnv)==2 && !pArgs->onlyTestIsLoadable) {
            /*
             * Switch implementation, and switch the staticData that's different
             * and was copied into the UConverter.
             * (See ucnv_createConverterFromSharedData() in ucnv_bld.c.)
             * UTF-16,version=2 fromUnicode() always writes a big-endian byte stream.
             */
            cnv->shared_তথ্য=(UConverter_Shared_তথ্য*)&UTF16v2_তথ্য;
            uprv_memcpy(cnv->subChars, UTF16v2_তথ্য.static_তথ্য->subChar, UCNV_MAX_SUBCHAR_LEN);
        }
        _UTF16Reset(cnv, UCNV_RESET_BOTH);
    } অন্যথা {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    }
}

স্থির ধ্রুবক অক্ষর *  U_CALLCONV
_UTF16GetName(ধ্রুবক UConverter *cnv) {
    যদি(UCNV_GET_VERSION(cnv)==0) {
        ফেরত "UTF-16";
    } অন্যথা যদি(UCNV_GET_VERSION(cnv)==1) {
        ফেরত "UTF-16,version=1";
    } অন্যথা {
        ফেরত "UTF-16,version=2";
    }
}
U_CDECL_END
বাহ্যিক ধ্রুবক UConverter_Shared_তথ্য UTF16_তথ্য;

স্থির অন্তঃরেখ যৌক্তিক IS_UTF16BE(ধ্রুবক UConverter *cnv) {
    ফেরত ((cnv)->shared_তথ্য == &_UTF16BEData);
}

স্থির অন্তঃরেখ যৌক্তিক IS_UTF16LE(ধ্রুবক UConverter *cnv) {
    ফেরত ((cnv)->shared_তথ্য == &_UTF16LEData);
}

স্থির অন্তঃরেখ যৌক্তিক IS_UTF16(ধ্রুবক UConverter *cnv) {
    ফেরত ((cnv)->shared_তথ্য==&UTF16_তথ্য) || ((cnv)->shared_তথ্য == &UTF16v2_তথ্য);
}

U_CDECL_BEGIN
স্থির শূন্য U_CALLCONV
_UTF16ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv=pArgs->converter;
    ধ্রুবক অক্ষর *source=pArgs->source;
    ধ্রুবক অক্ষর *sourceLimit=pArgs->sourceLimit;
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

    যখন(source<sourceLimit && U_SUCCESS(*pErrorCode)) {
        নির্বাচন(state) {
        ক্ষেত্র 0:
            cnv->toUBytes[0]=(uint8_t)*source++;
            cnv->toULength=1;
            state=1;
            বিরতি;
        ক্ষেত্র 1:
            /*
             * Only inside this switch case can the state variable
             * temporarily take two additional values:
             * 6: BOM error, continue with BE
             * 7: BOM error, continue with LE
             */
            b=*source;
            যদি(cnv->toUBytes[0]==0xfe && b==0xff) {
                যদি(IS_UTF16LE(cnv)) {
                    state=7; /* illegal reverse BOM for Java "UnicodeLittle" */
                } অন্যথা {
                    state=8; /* detect UTF-16BE */
                }
            } অন্যথা যদি(cnv->toUBytes[0]==0xff && b==0xfe) {
                যদি(IS_UTF16BE(cnv)) {
                    state=6; /* illegal reverse BOM for Java "UnicodeBig" */
                } অন্যথা {
                    state=9; /* detect UTF-16LE */
                }
            } অন্যথা যদি((IS_UTF16(cnv) && UCNV_GET_VERSION(cnv)==1)) {
                state=6; /* illegal missing BOM for Java "Unicode" */
            }
            যদি(state>=8) {
                /* BOM detected, consume it */
                ++source;
                cnv->toULength=0;
                offsetDelta=(int32_t)(source-pArgs->source);
            } অন্যথা যদি(state<6) {
                /* ok: no BOM, and not a reverse BOM */
                যদি(source!=pArgs->source) {
                    /* reset the source for a correct first offset */
                    source=pArgs->source;
                    cnv->toULength=0;
                }
                যদি(IS_UTF16LE(cnv)) {
                    /* Make Java "UnicodeLittle" default to LE. */
                    state=9;
                } অন্যথা {
                    /* Make standard UTF-16 and Java "UnicodeBig" default to BE. */
                    state=8;
                }
            } অন্যথা {
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
                ফেরত;
            }
            /* convert the rest of the stream */
            cnv->mode=state;
            চালু;
        ক্ষেত্র 8:
            /* call UTF-16BE */
            pArgs->source=source;
            _UTF16BEToUnicodeWithOffsets(pArgs, pErrorCode);
            source=pArgs->source;
            বিরতি;
        ক্ষেত্র 9:
            /* call UTF-16LE */
            pArgs->source=source;
            _UTF16LEToUnicodeWithOffsets(pArgs, pErrorCode);
            source=pArgs->source;
            বিরতি;
        পূর্বনির্ধারিত:
            বিরতি; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    যদি(offsets!=nullptr && offsetDelta!=0) {
        int32_t *offsetsLimit=pArgs->offsets;
        যখন(offsets<offsetsLimit) {
            *offsets++ += offsetDelta;
        }
    }

    pArgs->source=source;

    যদি(source==sourceLimit && pArgs->flush) {
        /* handle truncated input */
        নির্বাচন(state) {
        ক্ষেত্র 0:
            বিরতি; /* no input at all, nothing to do */
        ক্ষেত্র 8:
            _UTF16BEToUnicodeWithOffsets(pArgs, pErrorCode);
            বিরতি;
        ক্ষেত্র 9:
            _UTF16LEToUnicodeWithOffsets(pArgs, pErrorCode);
            বিরতি;
        পূর্বনির্ধারিত:
            /* 0<state<8: framework will report truncation, nothing to do here */
            বিরতি;
        }
    }

    cnv->mode=state;
}

স্থির UChar32 U_CALLCONV
UTF16_Get_পরবর্তী_UChar(UConverterToUnicodeArgs *pArgs,
                   UErrorCode *pErrorCode) {
    নির্বাচন(pArgs->converter->mode) {
    ক্ষেত্র 8:
        ফেরত UTF16_BEGet_পরবর্তী_UChar(pArgs, pErrorCode);
    ক্ষেত্র 9:
        ফেরত UTF16_LEGet_পরবর্তী_UChar(pArgs, pErrorCode);
    পূর্বনির্ধারিত:
        ফেরত UCNV_GET_পরবর্তী_UCHAR_USE_TO_U;
    }
}
U_CDECL_END

স্থির ধ্রুবক UConverterImpl _UTF16Impl = {
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
    UTF16_Get_পরবর্তী_UChar,

    nullptr, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

স্থির ধ্রুবক UConverter_Static_তথ্য UTF16_Static_তথ্য = {
    আকার(UConverter_Static_তথ্য),
    "UTF-16",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
#যদি U_IS_BIG_ENDIAN
    { 0xff, 0xfd, 0, 0 }, 2,
#অন্যথা
    { 0xfd, 0xff, 0, 0 }, 2,
#endif
    মিথ্যা, মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ধ্রুবক UConverter_Shared_তথ্য UTF16_তথ্য =
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&UTF16_Static_তথ্য, &_UTF16Impl);

স্থির ধ্রুবক UConverterImpl _UTF16v2Impl = {
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
    UTF16_Get_পরবর্তী_UChar,

    nullptr, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    nullptr,
    nullptr,
    ucnv_getNonSurrogateUnicodeSet,

    nullptr,
    nullptr
};

স্থির ধ্রুবক UConverter_Static_তথ্য UTF16v2_Static_তথ্য = {
    আকার(UConverter_Static_তথ্য),
    "UTF-16,version=2",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
    { 0xff, 0xfd, 0, 0 }, 2,
    মিথ্যা, মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ধ্রুবক UConverter_Shared_তথ্য UTF16v2_তথ্য =
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&UTF16v2_Static_তথ্য, &_UTF16v2Impl);

#endif
