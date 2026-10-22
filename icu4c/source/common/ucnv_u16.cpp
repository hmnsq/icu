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

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "unicode/uversion.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"
#ضمن "cmemory.h"

تعداد {
    UCNV_NEED_منزل_WRITE_BOM=1
};

U_CDECL_BEGIN
/*
 * The UTF-16 toUnicode implementation is also used for the Java-specific
 * "with BOM" variants of UTF-16BE and UTF-16LE.
 */
ساكن فراغ  U_CALLCONV
UTF16_منزل_Unicode_With_Offsets(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه,
                           UError_کوډ *p_Error_کوډ);

/* UTF-16BE ----------------------------------------------------------------- */

#إذا U_IS_BIG_ENDIAN
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16BEFromUnicodeWithOffsets
#وإلا
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16LEFromUnicodeWithOffsets
#د_که_پای


ساكن فراغ  U_CALLCONV
_UTF16BEFromUnicodeWithOffsets(UConverter_From_Unicode_دلیلونه *p_دلیلونه,
                               UError_کوډ *p_Error_کوډ) {
    UConverter *cnv;
    ثابت char16_t *سرچینه;
    حرف *موخه;
    int32_t *offsets;

    uint32_t موخه_ظرفیت, length, سرچینه_شاخص;
    char16_t c, trail;
    حرف ډکېدنه[4];

    سرچینه=p_دلیلونه->سرچینه;
    length=(int32_t)(p_دلیلونه->سرچینه_حد-سرچینه);
    إذا(length<=0) {
        /* no input, nothing to do */
        أرجع;
    }

    cnv=p_دلیلونه->converter;

    /* write the BOM if necessary */
    إذا(cnv->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xfeu, (حرف)0xffu };
        ucnv_from_UWrite_بایټونه(cnv,
                             bom, 2,
                             &p_دلیلونه->موخه, p_دلیلونه->موخه_حد,
                             &p_دلیلونه->offsets, -1,
                             p_Error_کوډ);
        cnv->from_Unicode_حالت=0;
    }

    موخه=p_دلیلونه->موخه;
    إذا(موخه >= p_دلیلونه->موخه_حد) {
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
        أرجع;
    }

    موخه_ظرفیت=(uint32_t)(p_دلیلونه->موخه_حد-موخه);
    offsets=p_دلیلونه->offsets;
    سرچینه_شاخص=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    إذا((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*سرچینه) && موخه_ظرفیت>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++سرچینه;
        --length;
        موخه[0]=(uint8_t)(c>>8);
        موخه[1]=(uint8_t)c;
        موخه[2]=(uint8_t)(trail>>8);
        موخه[3]=(uint8_t)trail;
        موخه+=4;
        موخه_ظرفیت-=4;
        إذا(offsets!=تش_اشاره) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        سرچینه_شاخص=1;
        cnv->fromUChar32=c=0;
    }

    إذا(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        إذا(count>موخه_ظرفیت) {
            count=موخه_ظرفیت&~1;
        }
        /* count is even */
        موخه_ظرفیت-=count;
        count>>=1;
        length-=count;

        إذا(offsets==تش_اشاره) {
            طالما(count>0) {
                c=*سرچینه++;
                إذا(U16_IS_SINGLE(c)) {
                    موخه[0]=(uint8_t)(c>>8);
                    موخه[1]=(uint8_t)c;
                    موخه+=2;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*سرچینه)) {
                    ++سرچینه;
                    --count;
                    موخه[0]=(uint8_t)(c>>8);
                    موخه[1]=(uint8_t)c;
                    موخه[2]=(uint8_t)(trail>>8);
                    موخه[3]=(uint8_t)trail;
                    موخه+=4;
                } وإلا {
                    اكسر;
                }
                --count;
            }
        } وإلا {
            طالما(count>0) {
                c=*سرچینه++;
                إذا(U16_IS_SINGLE(c)) {
                    موخه[0]=(uint8_t)(c>>8);
                    موخه[1]=(uint8_t)c;
                    موخه+=2;
                    *offsets++=سرچینه_شاخص;
                    *offsets++=سرچینه_شاخص++;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*سرچینه)) {
                    ++سرچینه;
                    --count;
                    موخه[0]=(uint8_t)(c>>8);
                    موخه[1]=(uint8_t)c;
                    موخه[2]=(uint8_t)(trail>>8);
                    موخه[3]=(uint8_t)trail;
                    موخه+=4;
                    *offsets++=سرچینه_شاخص;
                    *offsets++=سرچینه_شاخص;
                    *offsets++=سرچینه_شاخص;
                    *offsets++=سرچینه_شاخص;
                    سرچینه_شاخص+=2;
                } وإلا {
                    اكسر;
                }
                --count;
            }
        }

        إذا(count==0) {
            /* done with the loop for complete UChars */
            إذا(length>0 && موخه_ظرفیت>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                إذا(U16_IS_SINGLE(c=*سرچینه++)) {
                    ډکېدنه[0]=(حرف)(c>>8);
                    ډکېدنه[1]=(حرف)c;
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
            موخه_ظرفیت+=2*count;
        }
    } وإلا {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    إذا(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        إذا(U16_IS_SURROGATE_LEAD(c)) {
            إذا(سرچینه<p_دلیلونه->سرچینه_حد) {
                إذا(U16_IS_TRAIL(trail=*سرچینه)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++سرچینه;
                    ډکېدنه[0]=(حرف)(c>>8);
                    ډکېدنه[1]=(حرف)c;
                    ډکېدنه[2]=(حرف)(trail>>8);
                    ډکېدنه[3]=(حرف)trail;
                    length=4; /* 4 bytes to output */
                    c=0;
                } وإلا {
                    /* unmatched lead surrogate */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                }
            } وإلا {
                /* see if the trail surrogate is in the next buffer */
            }
        } وإلا {
            /* unmatched trail surrogate */
            *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
        }
        cnv->fromUChar32=c;
    }

    إذا(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_بایټونه(cnv,
                             ډکېدنه, length,
                             &موخه, p_دلیلونه->موخه_حد,
                             &offsets, سرچینه_شاخص,
                             p_Error_کوډ);
        موخه_ظرفیت = static_cast<uint32_t>(p_دلیلونه->موخه_حد - موخه);
    }

    إذا(U_SUCCESS(*p_Error_کوډ) && سرچینه<p_دلیلونه->سرچینه_حد && موخه_ظرفیت==0) {
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
    }

    /* write back the updated pointers */
    p_دلیلونه->سرچینه=سرچینه;
    p_دلیلونه->موخه = موخه;
    p_دلیلونه->offsets=offsets;
}

ساكن فراغ  U_CALLCONV
_UTF16BEToUnicodeWithOffsets(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه,
                             UError_کوډ *p_Error_کوډ) {
    UConverter *cnv;
    ثابت uint8_t *سرچینه;
    char16_t *موخه;
    int32_t *offsets;

    uint32_t موخه_ظرفیت, length, count, سرچینه_شاخص;
    char16_t c, trail;

    إذا(p_دلیلونه->converter->ډېرپېښ<8) {
        UTF16_منزل_Unicode_With_Offsets(p_دلیلونه, p_Error_کوډ);
        أرجع;
    }

    cnv=p_دلیلونه->converter;
    سرچینه=(ثابت uint8_t *)p_دلیلونه->سرچینه;
    length=(int32_t)((ثابت uint8_t *)p_دلیلونه->سرچینه_حد-سرچینه);
    إذا(length<=0 && cnv->منزل_Unicode_حالت==0) {
        /* no input, nothing to do */
        أرجع;
    }

    موخه=p_دلیلونه->موخه;
    إذا(موخه >= p_دلیلونه->موخه_حد) {
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
        أرجع;
    }

    موخه_ظرفیت=(uint32_t)(p_دلیلونه->موخه_حد-موخه);
    offsets=p_دلیلونه->offsets;
    سرچینه_شاخص=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    إذا(cnv->منزل_Unicode_حالت!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->منزل_UBytes[0]=(uint8_t)cnv->منزل_Unicode_حالت;
        cnv->منزل_ULength=1;
        cnv->منزل_Unicode_حالت=0;
    }
    إذا((count=cnv->منزل_ULength)!=0) {
        uint8_t *p=cnv->منزل_UBytes;
        افعل {
            p[count++]=*سرچینه++;
            ++سرچینه_شاخص;
            --length;
            إذا(count==2) {
                c=((char16_t)p[0]<<8)|p[1];
                إذا(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *موخه++=c;
                    إذا(offsets!=تش_اشاره) {
                        *offsets++=-1;
                    }
                    --موخه_ظرفیت;
                    count=0;
                    c=0;
                    اكسر;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } وإلا {
                    /* fall through to error handling for an unmatched trail surrogate */
                    اكسر;
                }
            } وإلا إذا(count==4) {
                c=((char16_t)p[0]<<8)|p[1];
                trail=((char16_t)p[2]<<8)|p[3];
                إذا(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *موخه++=c;
                    إذا(موخه_ظرفیت>=2) {
                        *موخه++=trail;
                        إذا(offsets!=تش_اشاره) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        موخه_ظرفیت-=2;
                    } وإلا /* targetCapacity==1 */ {
                        موخه_ظرفیت=0;
                        cnv->UChar_Error_بفر[0]=trail;
                        cnv->UChar_Error_بفر_اوږدتیا=1;
                        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                    }
                    count=0;
                    c=0;
                    اكسر;
                } وإلا {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;

                    /* back out reading the code unit after it */
                    إذا(((ثابت uint8_t *)p_دلیلونه->سرچینه-سرچینه)>=2) {
                        سرچینه-=2;
                    } وإلا {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->منزل_Unicode_حالت=0x100|p[2];
                        --سرچینه;
                    }
                    cnv->منزل_ULength=2;

                    /* write back the updated pointers */
                    p_دلیلونه->سرچینه=(ثابت حرف *)سرچینه;
                    p_دلیلونه->موخه=موخه;
                    p_دلیلونه->offsets=offsets;
                    أرجع;
                }
            }
        } طالما(length>0);
        cnv->منزل_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*موخه_ظرفیت;
    إذا(count>length) {
        count=length&~1;
    }
    إذا(c==0 && count>0) {
        length-=count;
        count>>=1;
        موخه_ظرفیت-=count;
        إذا(offsets==تش_اشاره) {
            افعل {
                c=((char16_t)سرچینه[0]<<8)|سرچینه[1];
                سرچینه+=2;
                إذا(U16_IS_SINGLE(c)) {
                    *موخه++=c;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)سرچینه[0]<<8)|سرچینه[1])
                ) {
                    سرچینه+=2;
                    --count;
                    *موخه++=c;
                    *موخه++=trail;
                } وإلا {
                    اكسر;
                }
            } طالما(--count>0);
        } وإلا {
            افعل {
                c=((char16_t)سرچینه[0]<<8)|سرچینه[1];
                سرچینه+=2;
                إذا(U16_IS_SINGLE(c)) {
                    *موخه++=c;
                    *offsets++=سرچینه_شاخص;
                    سرچینه_شاخص+=2;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)سرچینه[0]<<8)|سرچینه[1])
                ) {
                    سرچینه+=2;
                    --count;
                    *موخه++=c;
                    *موخه++=trail;
                    *offsets++=سرچینه_شاخص;
                    *offsets++=سرچینه_شاخص;
                    سرچینه_شاخص+=4;
                } وإلا {
                    اكسر;
                }
            } طالما(--count>0);
        }

        إذا(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } وإلا {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            موخه_ظرفیت+=count;
        }
    }

    إذا(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->منزل_UBytes[0]=(uint8_t)(c>>8);
        cnv->منزل_UBytes[1]=(uint8_t)c;
        cnv->منزل_ULength=2;

        إذا(U16_IS_SURROGATE_LEAD(c)) {
            إذا(length>=2) {
                إذا(U16_IS_TRAIL(trail=((char16_t)سرچینه[0]<<8)|سرچینه[1])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    سرچینه+=2;
                    length-=2;
                    *موخه++=c;
                    إذا(offsets!=تش_اشاره) {
                        *offsets++=سرچینه_شاخص;
                    }
                    cnv->UChar_Error_بفر[0]=trail;
                    cnv->UChar_Error_بفر_اوږدتیا=1;
                    cnv->منزل_ULength=0;
                    *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                } وإلا {
                    /* unmatched lead surrogate */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                }
            } وإلا {
                /* see if the trail surrogate is in the next buffer */
            }
        } وإلا {
            /* unmatched trail surrogate */
            *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
        }
    }

    إذا(U_SUCCESS(*p_Error_کوډ)) {
        /* check for a remaining source byte */
        إذا(length>0) {
            إذا(موخه_ظرفیت==0) {
                *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
            } وإلا {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->منزل_UBytes[cnv->منزل_ULength++]=*سرچینه++;
            }
        }
    }

    /* write back the updated pointers */
    p_دلیلونه->سرچینه=(ثابت حرف *)سرچینه;
    p_دلیلونه->موخه=موخه;
    p_دلیلونه->offsets=offsets;
}

ساكن UChar32  U_CALLCONV
UTF16_BEGet_بل_UChar(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه, UError_کوډ *err) {
    ثابت uint8_t *s, *سرچینه_حد;
    UChar32 c;

    إذا(p_دلیلونه->converter->ډېرپېښ<8) {
        أرجع UCNV_GET_بل_UCHAR_USE_منزل_U;
    }

    s=(ثابت uint8_t *)p_دلیلونه->سرچینه;
    سرچینه_حد=(ثابت uint8_t *)p_دلیلونه->سرچینه_حد;

    إذا(s>=سرچینه_حد) {
        /* no input */
        *err=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    إذا(s+2>سرچینه_حد) {
        /* only one byte: truncated char16_t */
        p_دلیلونه->converter->منزل_UBytes[0]=*s++;
        p_دلیلونه->converter->منزل_ULength=1;
        p_دلیلونه->سرچینه=(ثابت حرف *)s;
        *err = U_TRUNCATED_CHAR_وموندل;
        أرجع 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)*s<<8)|s[1];
    s+=2;

    /* check for a surrogate pair */
    إذا(U_IS_SURROGATE(c)) {
        إذا(U16_IS_SURROGATE_LEAD(c)) {
            إذا(s+2<=سرچینه_حد) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)*s<<8)|s[1];
                إذا(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } وإلا {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } وإلا {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *بایټونه=p_دلیلونه->converter->منزل_UBytes;
                s-=2;
                p_دلیلونه->converter->منزل_ULength=(int8_t)(سرچینه_حد-s);
                افعل {
                    *بایټونه++=*s++;
                } طالما(s<سرچینه_حد);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_وموندل;
            }
        } وإلا {
            /* unmatched trail surrogate */
            c=-2;
        }

        إذا(c<0) {
            /* write the unmatched surrogate */
            uint8_t *بایټونه=p_دلیلونه->converter->منزل_UBytes;
            p_دلیلونه->converter->منزل_ULength=2;
            *بایټونه=*(s-2);
            بایټونه[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_وموندل;
        }
    }

    p_دلیلونه->سرچینه=(ثابت حرف *)s;
    أرجع c;
} 

ساكن فراغ  U_CALLCONV
_UTF16BEReset(UConverter *cnv, UConverterResetChoice choice) {
    إذا(choice<=UCNV_RESET_منزل_UNICODE) {
        /* reset toUnicode state */
        إذا(UCNV_GET_نسخه(cnv)==0) {
            cnv->ډېرپېښ=8; /* no BOM handling */
        } وإلا {
            cnv->ډېرپېښ=0; /* Java-specific "UnicodeBig" requires BE BOM or no BOM */
        }
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE && UCNV_GET_نسخه(cnv)==1) {
        /* reset fromUnicode for "UnicodeBig": prepare to output the UTF-16BE BOM */
        cnv->from_Unicode_حالت=UCNV_NEED_منزل_WRITE_BOM;
    }
}

ساكن فراغ  U_CALLCONV
_UTF16BEOpen(UConverter *cnv,
             UConverter_بارول_دلیلونه *p_دلیلونه,
             UError_کوډ *p_Error_کوډ) {
    (فراغ)p_دلیلونه;
    إذا(UCNV_GET_نسخه(cnv)<=1) {
        _UTF16BEReset(cnv, UCNV_RESET_BOTH);
    } وإلا {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
    }
}

ساكن ثابت حرف *  U_CALLCONV
_UTF16BEGetName(ثابت UConverter *cnv) {
    إذا(UCNV_GET_نسخه(cnv)==0) {
        أرجع "UTF-16BE";
    } وإلا {
        أرجع "UTF-16BE,version=1";
    }
}
U_CDECL_پای_ټکی

ساكن ثابت UConverterImpl _UTF16BEImpl={
    UCNV_UTF16_BigEndian,

    تش_اشاره,
    تش_اشاره,

    _UTF16BEOpen,
    تش_اشاره,
    _UTF16BEReset,

    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_BEGet_بل_UChar,

    تش_اشاره,
    _UTF16BEGetName,
    تش_اشاره,
    تش_اشاره,
    ucnv_get_Non_Surrogate_Unicode_سېټ,

    تش_اشاره,
    تش_اشاره
};

ساكن ثابت UConverter_Static_معلومات UTF16_BEStatic_معلومات={
    حجم(UConverter_Static_معلومات),
    "UTF-16BE",
    1200, UCNV_IBM, UCNV_UTF16_BigEndian, 2, 2,
    { 0xff, 0xfd, 0, 0 },2,خطأ,خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_معلومات _UTF16BEData=
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&UTF16_BEStatic_معلومات, &_UTF16BEImpl);

/* UTF-16LE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV
_UTF16LEFromUnicodeWithOffsets(UConverter_From_Unicode_دلیلونه *p_دلیلونه,
                               UError_کوډ *p_Error_کوډ) {
    UConverter *cnv;
    ثابت char16_t *سرچینه;
    حرف *موخه;
    int32_t *offsets;

    uint32_t موخه_ظرفیت, length, سرچینه_شاخص;
    char16_t c, trail;
    حرف ډکېدنه[4];

    سرچینه=p_دلیلونه->سرچینه;
    length=(int32_t)(p_دلیلونه->سرچینه_حد-سرچینه);
    إذا(length<=0) {
        /* no input, nothing to do */
        أرجع;
    }

    cnv=p_دلیلونه->converter;

    /* write the BOM if necessary */
    إذا(cnv->from_Unicode_حالت==UCNV_NEED_منزل_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu };
        ucnv_from_UWrite_بایټونه(cnv,
                             bom, 2,
                             &p_دلیلونه->موخه, p_دلیلونه->موخه_حد,
                             &p_دلیلونه->offsets, -1,
                             p_Error_کوډ);
        cnv->from_Unicode_حالت=0;
    }

    موخه=p_دلیلونه->موخه;
    إذا(موخه >= p_دلیلونه->موخه_حد) {
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
        أرجع;
    }

    موخه_ظرفیت=(uint32_t)(p_دلیلونه->موخه_حد-p_دلیلونه->موخه);
    offsets=p_دلیلونه->offsets;
    سرچینه_شاخص=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    إذا((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*سرچینه) && موخه_ظرفیت>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++سرچینه;
        --length;
        موخه[0]=(uint8_t)c;
        موخه[1]=(uint8_t)(c>>8);
        موخه[2]=(uint8_t)trail;
        موخه[3]=(uint8_t)(trail>>8);
        موخه+=4;
        موخه_ظرفیت-=4;
        إذا(offsets!=تش_اشاره) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        سرچینه_شاخص=1;
        cnv->fromUChar32=c=0;
    }

    إذا(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        إذا(count>موخه_ظرفیت) {
            count=موخه_ظرفیت&~1;
        }
        /* count is even */
        موخه_ظرفیت-=count;
        count>>=1;
        length-=count;

        إذا(offsets==تش_اشاره) {
            طالما(count>0) {
                c=*سرچینه++;
                إذا(U16_IS_SINGLE(c)) {
                    موخه[0]=(uint8_t)c;
                    موخه[1]=(uint8_t)(c>>8);
                    موخه+=2;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*سرچینه)) {
                    ++سرچینه;
                    --count;
                    موخه[0]=(uint8_t)c;
                    موخه[1]=(uint8_t)(c>>8);
                    موخه[2]=(uint8_t)trail;
                    موخه[3]=(uint8_t)(trail>>8);
                    موخه+=4;
                } وإلا {
                    اكسر;
                }
                --count;
            }
        } وإلا {
            طالما(count>0) {
                c=*سرچینه++;
                إذا(U16_IS_SINGLE(c)) {
                    موخه[0]=(uint8_t)c;
                    موخه[1]=(uint8_t)(c>>8);
                    موخه+=2;
                    *offsets++=سرچینه_شاخص;
                    *offsets++=سرچینه_شاخص++;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*سرچینه)) {
                    ++سرچینه;
                    --count;
                    موخه[0]=(uint8_t)c;
                    موخه[1]=(uint8_t)(c>>8);
                    موخه[2]=(uint8_t)trail;
                    موخه[3]=(uint8_t)(trail>>8);
                    موخه+=4;
                    *offsets++=سرچینه_شاخص;
                    *offsets++=سرچینه_شاخص;
                    *offsets++=سرچینه_شاخص;
                    *offsets++=سرچینه_شاخص;
                    سرچینه_شاخص+=2;
                } وإلا {
                    اكسر;
                }
                --count;
            }
        }

        إذا(count==0) {
            /* done with the loop for complete UChars */
            إذا(length>0 && موخه_ظرفیت>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                إذا(U16_IS_SINGLE(c=*سرچینه++)) {
                    ډکېدنه[0]=(حرف)c;
                    ډکېدنه[1]=(حرف)(c>>8);
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
            موخه_ظرفیت+=2*count;
        }
    } وإلا {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    إذا(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        إذا(U16_IS_SURROGATE_LEAD(c)) {
            إذا(سرچینه<p_دلیلونه->سرچینه_حد) {
                إذا(U16_IS_TRAIL(trail=*سرچینه)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++سرچینه;
                    ډکېدنه[0]=(حرف)c;
                    ډکېدنه[1]=(حرف)(c>>8);
                    ډکېدنه[2]=(حرف)trail;
                    ډکېدنه[3]=(حرف)(trail>>8);
                    length=4; /* 4 bytes to output */
                    c=0;
                } وإلا {
                    /* unmatched lead surrogate */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                }
            } وإلا {
                /* see if the trail surrogate is in the next buffer */
            }
        } وإلا {
            /* unmatched trail surrogate */
            *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
        }
        cnv->fromUChar32=c;
    }

    إذا(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_بایټونه(cnv,
                             ډکېدنه, length,
                             &موخه, p_دلیلونه->موخه_حد,
                             &offsets, سرچینه_شاخص,
                             p_Error_کوډ);
        موخه_ظرفیت = static_cast<uint32_t>(p_دلیلونه->موخه_حد - موخه);
    }

    إذا(U_SUCCESS(*p_Error_کوډ) && سرچینه<p_دلیلونه->سرچینه_حد && موخه_ظرفیت==0) {
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
    }

    /* write back the updated pointers */
    p_دلیلونه->سرچینه=سرچینه;
    p_دلیلونه->موخه=موخه;
    p_دلیلونه->offsets=offsets;
}

ساكن فراغ  U_CALLCONV
_UTF16LEToUnicodeWithOffsets(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه,
                             UError_کوډ *p_Error_کوډ) {
    UConverter *cnv;
    ثابت uint8_t *سرچینه;
    char16_t *موخه;
    int32_t *offsets;

    uint32_t موخه_ظرفیت, length, count, سرچینه_شاخص;
    char16_t c, trail;

    إذا(p_دلیلونه->converter->ډېرپېښ<8) {
        UTF16_منزل_Unicode_With_Offsets(p_دلیلونه, p_Error_کوډ);
        أرجع;
    }

    cnv=p_دلیلونه->converter;
    سرچینه=(ثابت uint8_t *)p_دلیلونه->سرچینه;
    length=(int32_t)((ثابت uint8_t *)p_دلیلونه->سرچینه_حد-سرچینه);
    إذا(length<=0 && cnv->منزل_Unicode_حالت==0) {
        /* no input, nothing to do */
        أرجع;
    }

    موخه=p_دلیلونه->موخه;
    إذا(موخه >= p_دلیلونه->موخه_حد) {
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
        أرجع;
    }

    موخه_ظرفیت=(uint32_t)(p_دلیلونه->موخه_حد-p_دلیلونه->موخه);
    offsets=p_دلیلونه->offsets;
    سرچینه_شاخص=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    إذا(cnv->منزل_Unicode_حالت!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->منزل_UBytes[0]=(uint8_t)cnv->منزل_Unicode_حالت;
        cnv->منزل_ULength=1;
        cnv->منزل_Unicode_حالت=0;
    }
    إذا((count=cnv->منزل_ULength)!=0) {
        uint8_t *p=cnv->منزل_UBytes;
        افعل {
            p[count++]=*سرچینه++;
            ++سرچینه_شاخص;
            --length;
            إذا(count==2) {
                c=((char16_t)p[1]<<8)|p[0];
                إذا(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *موخه++=c;
                    إذا(offsets!=تش_اشاره) {
                        *offsets++=-1;
                    }
                    --موخه_ظرفیت;
                    count=0;
                    c=0;
                    اكسر;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } وإلا {
                    /* fall through to error handling for an unmatched trail surrogate */
                    اكسر;
                }
            } وإلا إذا(count==4) {
                c=((char16_t)p[1]<<8)|p[0];
                trail=((char16_t)p[3]<<8)|p[2];
                إذا(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *موخه++=c;
                    إذا(موخه_ظرفیت>=2) {
                        *موخه++=trail;
                        إذا(offsets!=تش_اشاره) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        موخه_ظرفیت-=2;
                    } وإلا /* targetCapacity==1 */ {
                        موخه_ظرفیت=0;
                        cnv->UChar_Error_بفر[0]=trail;
                        cnv->UChar_Error_بفر_اوږدتیا=1;
                        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                    }
                    count=0;
                    c=0;
                    اكسر;
                } وإلا {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;

                    /* back out reading the code unit after it */
                    إذا(((ثابت uint8_t *)p_دلیلونه->سرچینه-سرچینه)>=2) {
                        سرچینه-=2;
                    } وإلا {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->منزل_Unicode_حالت=0x100|p[2];
                        --سرچینه;
                    }
                    cnv->منزل_ULength=2;

                    /* write back the updated pointers */
                    p_دلیلونه->سرچینه=(ثابت حرف *)سرچینه;
                    p_دلیلونه->موخه=موخه;
                    p_دلیلونه->offsets=offsets;
                    أرجع;
                }
            }
        } طالما(length>0);
        cnv->منزل_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*موخه_ظرفیت;
    إذا(count>length) {
        count=length&~1;
    }
    إذا(c==0 && count>0) {
        length-=count;
        count>>=1;
        موخه_ظرفیت-=count;
        إذا(offsets==تش_اشاره) {
            افعل {
                c=((char16_t)سرچینه[1]<<8)|سرچینه[0];
                سرچینه+=2;
                إذا(U16_IS_SINGLE(c)) {
                    *موخه++=c;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)سرچینه[1]<<8)|سرچینه[0])
                ) {
                    سرچینه+=2;
                    --count;
                    *موخه++=c;
                    *موخه++=trail;
                } وإلا {
                    اكسر;
                }
            } طالما(--count>0);
        } وإلا {
            افعل {
                c=((char16_t)سرچینه[1]<<8)|سرچینه[0];
                سرچینه+=2;
                إذا(U16_IS_SINGLE(c)) {
                    *موخه++=c;
                    *offsets++=سرچینه_شاخص;
                    سرچینه_شاخص+=2;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)سرچینه[1]<<8)|سرچینه[0])
                ) {
                    سرچینه+=2;
                    --count;
                    *موخه++=c;
                    *موخه++=trail;
                    *offsets++=سرچینه_شاخص;
                    *offsets++=سرچینه_شاخص;
                    سرچینه_شاخص+=4;
                } وإلا {
                    اكسر;
                }
            } طالما(--count>0);
        }

        إذا(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } وإلا {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            موخه_ظرفیت+=count;
        }
    }

    إذا(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->منزل_UBytes[0]=(uint8_t)c;
        cnv->منزل_UBytes[1]=(uint8_t)(c>>8);
        cnv->منزل_ULength=2;

        إذا(U16_IS_SURROGATE_LEAD(c)) {
            إذا(length>=2) {
                إذا(U16_IS_TRAIL(trail=((char16_t)سرچینه[1]<<8)|سرچینه[0])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    سرچینه+=2;
                    length-=2;
                    *موخه++=c;
                    إذا(offsets!=تش_اشاره) {
                        *offsets++=سرچینه_شاخص;
                    }
                    cnv->UChar_Error_بفر[0]=trail;
                    cnv->UChar_Error_بفر_اوږدتیا=1;
                    cnv->منزل_ULength=0;
                    *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                } وإلا {
                    /* unmatched lead surrogate */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                }
            } وإلا {
                /* see if the trail surrogate is in the next buffer */
            }
        } وإلا {
            /* unmatched trail surrogate */
            *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
        }
    }

    إذا(U_SUCCESS(*p_Error_کوډ)) {
        /* check for a remaining source byte */
        إذا(length>0) {
            إذا(موخه_ظرفیت==0) {
                *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
            } وإلا {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->منزل_UBytes[cnv->منزل_ULength++]=*سرچینه++;
            }
        }
    }

    /* write back the updated pointers */
    p_دلیلونه->سرچینه=(ثابت حرف *)سرچینه;
    p_دلیلونه->موخه=موخه;
    p_دلیلونه->offsets=offsets;
}

ساكن UChar32  U_CALLCONV
UTF16_LEGet_بل_UChar(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه, UError_کوډ *err) {
    ثابت uint8_t *s, *سرچینه_حد;
    UChar32 c;

    إذا(p_دلیلونه->converter->ډېرپېښ<8) {
        أرجع UCNV_GET_بل_UCHAR_USE_منزل_U;
    }

    s=(ثابت uint8_t *)p_دلیلونه->سرچینه;
    سرچینه_حد=(ثابت uint8_t *)p_دلیلونه->سرچینه_حد;

    إذا(s>=سرچینه_حد) {
        /* no input */
        *err=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    إذا(s+2>سرچینه_حد) {
        /* only one byte: truncated char16_t */
        p_دلیلونه->converter->منزل_UBytes[0]=*s++;
        p_دلیلونه->converter->منزل_ULength=1;
        p_دلیلونه->سرچینه=(ثابت حرف *)s;
        *err = U_TRUNCATED_CHAR_وموندل;
        أرجع 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)s[1]<<8)|*s;
    s+=2;

    /* check for a surrogate pair */
    إذا(U_IS_SURROGATE(c)) {
        إذا(U16_IS_SURROGATE_LEAD(c)) {
            إذا(s+2<=سرچینه_حد) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)s[1]<<8)|*s;
                إذا(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } وإلا {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } وإلا {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *بایټونه=p_دلیلونه->converter->منزل_UBytes;
                s-=2;
                p_دلیلونه->converter->منزل_ULength=(int8_t)(سرچینه_حد-s);
                افعل {
                    *بایټونه++=*s++;
                } طالما(s<سرچینه_حد);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_وموندل;
            }
        } وإلا {
            /* unmatched trail surrogate */
            c=-2;
        }

        إذا(c<0) {
            /* write the unmatched surrogate */
            uint8_t *بایټونه=p_دلیلونه->converter->منزل_UBytes;
            p_دلیلونه->converter->منزل_ULength=2;
            *بایټونه=*(s-2);
            بایټونه[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_وموندل;
        }
    }

    p_دلیلونه->سرچینه=(ثابت حرف *)s;
    أرجع c;
} 

ساكن فراغ  U_CALLCONV
_UTF16LEReset(UConverter *cnv, UConverterResetChoice choice) {
    إذا(choice<=UCNV_RESET_منزل_UNICODE) {
        /* reset toUnicode state */
        إذا(UCNV_GET_نسخه(cnv)==0) {
            cnv->ډېرپېښ=8; /* no BOM handling */
        } وإلا {
            cnv->ډېرپېښ=0; /* Java-specific "UnicodeLittle" requires LE BOM or no BOM */
        }
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE && UCNV_GET_نسخه(cnv)==1) {
        /* reset fromUnicode for "UnicodeLittle": prepare to output the UTF-16LE BOM */
        cnv->from_Unicode_حالت=UCNV_NEED_منزل_WRITE_BOM;
    }
}

ساكن فراغ  U_CALLCONV
_UTF16LEOpen(UConverter *cnv,
             UConverter_بارول_دلیلونه *p_دلیلونه,
             UError_کوډ *p_Error_کوډ) {
    (فراغ)p_دلیلونه;
    إذا(UCNV_GET_نسخه(cnv)<=1) {
        _UTF16LEReset(cnv, UCNV_RESET_BOTH);
    } وإلا {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
    }
}

ساكن ثابت حرف *  U_CALLCONV
_UTF16LEGetName(ثابت UConverter *cnv) {
    إذا(UCNV_GET_نسخه(cnv)==0) {
        أرجع "UTF-16LE";
    } وإلا {
        أرجع "UTF-16LE,version=1";
    }
}
U_CDECL_پای_ټکی

ساكن ثابت UConverterImpl _UTF16LEImpl={
    UCNV_UTF16_LittleEndian,

    تش_اشاره,
    تش_اشاره,

    _UTF16LEOpen,
    تش_اشاره,
    _UTF16LEReset,

    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    UTF16_LEGet_بل_UChar,

    تش_اشاره,
    _UTF16LEGetName,
    تش_اشاره,
    تش_اشاره,
    ucnv_get_Non_Surrogate_Unicode_سېټ,

    تش_اشاره,
    تش_اشاره
};


ساكن ثابت UConverter_Static_معلومات UTF16_LEStatic_معلومات={
    حجم(UConverter_Static_معلومات),
    "UTF-16LE",
    1202, UCNV_IBM, UCNV_UTF16_LittleEndian, 2, 2,
    { 0xfd, 0xff, 0, 0 },2,خطأ,خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_معلومات _UTF16LEData=
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&UTF16_LEStatic_معلومات, &_UTF16LEImpl);

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
    إذا(choice<=UCNV_RESET_منزل_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->ډېرپېښ=0;
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-16PE BOM */
        cnv->from_Unicode_حالت=UCNV_NEED_منزل_WRITE_BOM;
    }
}
U_CDECL_پای_ټکی
خارجي ثابت UConverter_Shared_معلومات UTF16v2_معلومات;
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
_UTF16Open(UConverter *cnv,
           UConverter_بارول_دلیلونه *p_دلیلونه,
           UError_کوډ *p_Error_کوډ) {
    إذا(UCNV_GET_نسخه(cnv)<=2) {
        إذا(UCNV_GET_نسخه(cnv)==2 && !p_دلیلونه->onlyTestIsLoadable) {
            /*
             * Switch implementation, and switch the staticData that's different
             * and was copied into the UConverter.
             * (See ucnv_createConverterFromSharedData() in ucnv_bld.c.)
             * UTF-16,version=2 fromUnicode() always writes a big-endian byte stream.
             */
            cnv->shared_معلومات=(UConverter_Shared_معلومات*)&UTF16v2_معلومات;
            uprv_memcpy(cnv->subChars, UTF16v2_معلومات.static_معلومات->subChar, UCNV_MAX_SUBCHAR_LEN);
        }
        _UTF16Reset(cnv, UCNV_RESET_BOTH);
    } وإلا {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
    }
}

ساكن ثابت حرف *  U_CALLCONV
_UTF16GetName(ثابت UConverter *cnv) {
    إذا(UCNV_GET_نسخه(cnv)==0) {
        أرجع "UTF-16";
    } وإلا إذا(UCNV_GET_نسخه(cnv)==1) {
        أرجع "UTF-16,version=1";
    } وإلا {
        أرجع "UTF-16,version=2";
    }
}
U_CDECL_پای_ټکی
خارجي ثابت UConverter_Shared_معلومات UTF16_معلومات;

ساكن مضمن منطقي IS_UTF16BE(ثابت UConverter *cnv) {
    أرجع ((cnv)->shared_معلومات == &_UTF16BEData);
}

ساكن مضمن منطقي IS_UTF16LE(ثابت UConverter *cnv) {
    أرجع ((cnv)->shared_معلومات == &_UTF16LEData);
}

ساكن مضمن منطقي IS_UTF16(ثابت UConverter *cnv) {
    أرجع ((cnv)->shared_معلومات==&UTF16_معلومات) || ((cnv)->shared_معلومات == &UTF16v2_معلومات);
}

U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
UTF16_منزل_Unicode_With_Offsets(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه,
                           UError_کوډ *p_Error_کوډ) {
    UConverter *cnv=p_دلیلونه->converter;
    ثابت حرف *سرچینه=p_دلیلونه->سرچینه;
    ثابت حرف *سرچینه_حد=p_دلیلونه->سرچینه_حد;
    int32_t *offsets=p_دلیلونه->offsets;

    int32_t state, دننه_واټن_Delta;
    uint8_t b;

    state=cnv->ډېرپېښ;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    دننه_واټن_Delta=0;

    طالما(سرچینه<سرچینه_حد && U_SUCCESS(*p_Error_کوډ)) {
        بدل(state) {
        حالة 0:
            cnv->منزل_UBytes[0]=(uint8_t)*سرچینه++;
            cnv->منزل_ULength=1;
            state=1;
            اكسر;
        حالة 1:
            /*
             * Only inside this switch case can the state variable
             * temporarily take two additional values:
             * 6: BOM error, continue with BE
             * 7: BOM error, continue with LE
             */
            b=*سرچینه;
            إذا(cnv->منزل_UBytes[0]==0xfe && b==0xff) {
                إذا(IS_UTF16LE(cnv)) {
                    state=7; /* illegal reverse BOM for Java "UnicodeLittle" */
                } وإلا {
                    state=8; /* detect UTF-16BE */
                }
            } وإلا إذا(cnv->منزل_UBytes[0]==0xff && b==0xfe) {
                إذا(IS_UTF16BE(cnv)) {
                    state=6; /* illegal reverse BOM for Java "UnicodeBig" */
                } وإلا {
                    state=9; /* detect UTF-16LE */
                }
            } وإلا إذا((IS_UTF16(cnv) && UCNV_GET_نسخه(cnv)==1)) {
                state=6; /* illegal missing BOM for Java "Unicode" */
            }
            إذا(state>=8) {
                /* BOM detected, consume it */
                ++سرچینه;
                cnv->منزل_ULength=0;
                دننه_واټن_Delta=(int32_t)(سرچینه-p_دلیلونه->سرچینه);
            } وإلا إذا(state<6) {
                /* ok: no BOM, and not a reverse BOM */
                إذا(سرچینه!=p_دلیلونه->سرچینه) {
                    /* reset the source for a correct first offset */
                    سرچینه=p_دلیلونه->سرچینه;
                    cnv->منزل_ULength=0;
                }
                إذا(IS_UTF16LE(cnv)) {
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
                cnv->منزل_UBytes[1]=b;
                cnv->منزل_ULength=2;
                p_دلیلونه->سرچینه=سرچینه+1;
                /* continue with conversion if the callback resets the error */
                /*
                 * Make Java "Unicode" default to BE like standard UTF-16.
                 * Make Java "UnicodeBig" and "UnicodeLittle" default
                 * to their normal endiannesses.
                 */
                cnv->ډېرپېښ=state+2;
                *p_Error_کوډ=U_ILLEGAL_ESCAPE_SEQUENCE;
                أرجع;
            }
            /* convert the rest of the stream */
            cnv->ډېرپېښ=state;
            استمر;
        حالة 8:
            /* call UTF-16BE */
            p_دلیلونه->سرچینه=سرچینه;
            _UTF16BEToUnicodeWithOffsets(p_دلیلونه, p_Error_کوډ);
            سرچینه=p_دلیلونه->سرچینه;
            اكسر;
        حالة 9:
            /* call UTF-16LE */
            p_دلیلونه->سرچینه=سرچینه;
            _UTF16LEToUnicodeWithOffsets(p_دلیلونه, p_Error_کوډ);
            سرچینه=p_دلیلونه->سرچینه;
            اكسر;
        افتراضي:
            اكسر; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    إذا(offsets!=تش_اشاره && دننه_واټن_Delta!=0) {
        int32_t *offsets_حد=p_دلیلونه->offsets;
        طالما(offsets<offsets_حد) {
            *offsets++ += دننه_واټن_Delta;
        }
    }

    p_دلیلونه->سرچینه=سرچینه;

    إذا(سرچینه==سرچینه_حد && p_دلیلونه->flush) {
        /* handle truncated input */
        بدل(state) {
        حالة 0:
            اكسر; /* no input at all, nothing to do */
        حالة 8:
            _UTF16BEToUnicodeWithOffsets(p_دلیلونه, p_Error_کوډ);
            اكسر;
        حالة 9:
            _UTF16LEToUnicodeWithOffsets(p_دلیلونه, p_Error_کوډ);
            اكسر;
        افتراضي:
            /* 0<state<8: framework will report truncation, nothing to do here */
            اكسر;
        }
    }

    cnv->ډېرپېښ=state;
}

ساكن UChar32 U_CALLCONV
UTF16_Get_بل_UChar(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه,
                   UError_کوډ *p_Error_کوډ) {
    بدل(p_دلیلونه->converter->ډېرپېښ) {
    حالة 8:
        أرجع UTF16_BEGet_بل_UChar(p_دلیلونه, p_Error_کوډ);
    حالة 9:
        أرجع UTF16_LEGet_بل_UChar(p_دلیلونه, p_Error_کوډ);
    افتراضي:
        أرجع UCNV_GET_بل_UCHAR_USE_منزل_U;
    }
}
U_CDECL_پای_ټکی

ساكن ثابت UConverterImpl _UTF16Impl = {
    UCNV_UTF16,

    تش_اشاره,
    تش_اشاره,

    _UTF16Open,
    تش_اشاره,
    _UTF16Reset,

    UTF16_منزل_Unicode_With_Offsets,
    UTF16_منزل_Unicode_With_Offsets,
    _UTF16PEFromUnicodeWithOffsets,
    _UTF16PEFromUnicodeWithOffsets,
    UTF16_Get_بل_UChar,

    تش_اشاره, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    تش_اشاره,
    تش_اشاره,
    ucnv_get_Non_Surrogate_Unicode_سېټ,

    تش_اشاره,
    تش_اشاره
};

ساكن ثابت UConverter_Static_معلومات UTF16_Static_معلومات = {
    حجم(UConverter_Static_معلومات),
    "UTF-16",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
#إذا U_IS_BIG_ENDIAN
    { 0xff, 0xfd, 0, 0 }, 2,
#وإلا
    { 0xfd, 0xff, 0, 0 }, 2,
#د_که_پای
    خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_معلومات UTF16_معلومات =
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&UTF16_Static_معلومات, &_UTF16Impl);

ساكن ثابت UConverterImpl _UTF16v2Impl = {
    UCNV_UTF16,

    تش_اشاره,
    تش_اشاره,

    _UTF16Open,
    تش_اشاره,
    _UTF16Reset,

    UTF16_منزل_Unicode_With_Offsets,
    UTF16_منزل_Unicode_With_Offsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_Get_بل_UChar,

    تش_اشاره, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    تش_اشاره,
    تش_اشاره,
    ucnv_get_Non_Surrogate_Unicode_سېټ,

    تش_اشاره,
    تش_اشاره
};

ساكن ثابت UConverter_Static_معلومات UTF16v2_Static_معلومات = {
    حجم(UConverter_Static_معلومات),
    "UTF-16,version=2",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
    { 0xff, 0xfd, 0, 0 }, 2,
    خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_معلومات UTF16v2_معلومات =
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&UTF16v2_Static_معلومات, &_UTF16v2Impl);

#د_که_پای
