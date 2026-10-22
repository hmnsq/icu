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
    UCNV_NEED_مقصد_WRITE_BOM=1
};

U_CDECL_BEGIN
/*
 * The UTF-16 toUnicode implementation is also used for the Java-specific
 * "with BOM" variants of UTF-16BE and UTF-16LE.
 */
ساكن فراغ  U_CALLCONV
UTF16_مقصد_Unicode_With_Offsets(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                           UError_کد *p_Error_کد);

/* UTF-16BE ----------------------------------------------------------------- */

#إذا U_IS_BIG_ENDIAN
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16BEFromUnicodeWithOffsets
#وإلا
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16LEFromUnicodeWithOffsets
#پایان_اگر


ساكن فراغ  U_CALLCONV
_UTF16BEFromUnicodeWithOffsets(UConverter_From_Unicode_آرگومانها *p_آرگومانها,
                               UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت char16_t *منبع;
    حرف *هدف;
    int32_t *offsets;

    uint32_t هدف_ظرفیت, length, منبع_شاخص;
    char16_t c, trail;
    حرف سرریز[4];

    منبع=p_آرگومانها->منبع;
    length=(int32_t)(p_آرگومانها->منبع_حد-منبع);
    إذا(length<=0) {
        /* no input, nothing to do */
        أرجع;
    }

    cnv=p_آرگومانها->converter;

    /* write the BOM if necessary */
    إذا(cnv->from_Unicode_وضعیت==UCNV_NEED_مقصد_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xfeu, (حرف)0xffu };
        ucnv_from_UWrite_بایتها(cnv,
                             bom, 2,
                             &p_آرگومانها->هدف, p_آرگومانها->هدف_حد,
                             &p_آرگومانها->offsets, -1,
                             p_Error_کد);
        cnv->from_Unicode_وضعیت=0;
    }

    هدف=p_آرگومانها->هدف;
    إذا(هدف >= p_آرگومانها->هدف_حد) {
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
        أرجع;
    }

    هدف_ظرفیت=(uint32_t)(p_آرگومانها->هدف_حد-هدف);
    offsets=p_آرگومانها->offsets;
    منبع_شاخص=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    إذا((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*منبع) && هدف_ظرفیت>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++منبع;
        --length;
        هدف[0]=(uint8_t)(c>>8);
        هدف[1]=(uint8_t)c;
        هدف[2]=(uint8_t)(trail>>8);
        هدف[3]=(uint8_t)trail;
        هدف+=4;
        هدف_ظرفیت-=4;
        إذا(offsets!=اشاره_تهی) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        منبع_شاخص=1;
        cnv->fromUChar32=c=0;
    }

    إذا(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        إذا(count>هدف_ظرفیت) {
            count=هدف_ظرفیت&~1;
        }
        /* count is even */
        هدف_ظرفیت-=count;
        count>>=1;
        length-=count;

        إذا(offsets==اشاره_تهی) {
            طالما(count>0) {
                c=*منبع++;
                إذا(U16_IS_SINGLE(c)) {
                    هدف[0]=(uint8_t)(c>>8);
                    هدف[1]=(uint8_t)c;
                    هدف+=2;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*منبع)) {
                    ++منبع;
                    --count;
                    هدف[0]=(uint8_t)(c>>8);
                    هدف[1]=(uint8_t)c;
                    هدف[2]=(uint8_t)(trail>>8);
                    هدف[3]=(uint8_t)trail;
                    هدف+=4;
                } وإلا {
                    اكسر;
                }
                --count;
            }
        } وإلا {
            طالما(count>0) {
                c=*منبع++;
                إذا(U16_IS_SINGLE(c)) {
                    هدف[0]=(uint8_t)(c>>8);
                    هدف[1]=(uint8_t)c;
                    هدف+=2;
                    *offsets++=منبع_شاخص;
                    *offsets++=منبع_شاخص++;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*منبع)) {
                    ++منبع;
                    --count;
                    هدف[0]=(uint8_t)(c>>8);
                    هدف[1]=(uint8_t)c;
                    هدف[2]=(uint8_t)(trail>>8);
                    هدف[3]=(uint8_t)trail;
                    هدف+=4;
                    *offsets++=منبع_شاخص;
                    *offsets++=منبع_شاخص;
                    *offsets++=منبع_شاخص;
                    *offsets++=منبع_شاخص;
                    منبع_شاخص+=2;
                } وإلا {
                    اكسر;
                }
                --count;
            }
        }

        إذا(count==0) {
            /* done with the loop for complete UChars */
            إذا(length>0 && هدف_ظرفیت>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                إذا(U16_IS_SINGLE(c=*منبع++)) {
                    سرریز[0]=(حرف)(c>>8);
                    سرریز[1]=(حرف)c;
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
            هدف_ظرفیت+=2*count;
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
            إذا(منبع<p_آرگومانها->منبع_حد) {
                إذا(U16_IS_TRAIL(trail=*منبع)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++منبع;
                    سرریز[0]=(حرف)(c>>8);
                    سرریز[1]=(حرف)c;
                    سرریز[2]=(حرف)(trail>>8);
                    سرریز[3]=(حرف)trail;
                    length=4; /* 4 bytes to output */
                    c=0;
                } وإلا {
                    /* unmatched lead surrogate */
                    *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                }
            } وإلا {
                /* see if the trail surrogate is in the next buffer */
            }
        } وإلا {
            /* unmatched trail surrogate */
            *p_Error_کد=U_ILLEGAL_CHAR_یافت;
        }
        cnv->fromUChar32=c;
    }

    إذا(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_بایتها(cnv,
                             سرریز, length,
                             &هدف, p_آرگومانها->هدف_حد,
                             &offsets, منبع_شاخص,
                             p_Error_کد);
        هدف_ظرفیت = static_cast<uint32_t>(p_آرگومانها->هدف_حد - هدف);
    }

    إذا(U_SUCCESS(*p_Error_کد) && منبع<p_آرگومانها->منبع_حد && هدف_ظرفیت==0) {
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
    }

    /* write back the updated pointers */
    p_آرگومانها->منبع=منبع;
    p_آرگومانها->هدف = هدف;
    p_آرگومانها->offsets=offsets;
}

ساكن فراغ  U_CALLCONV
_UTF16BEToUnicodeWithOffsets(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                             UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت uint8_t *منبع;
    char16_t *هدف;
    int32_t *offsets;

    uint32_t هدف_ظرفیت, length, count, منبع_شاخص;
    char16_t c, trail;

    إذا(p_آرگومانها->converter->نما<8) {
        UTF16_مقصد_Unicode_With_Offsets(p_آرگومانها, p_Error_کد);
        أرجع;
    }

    cnv=p_آرگومانها->converter;
    منبع=(ثابت uint8_t *)p_آرگومانها->منبع;
    length=(int32_t)((ثابت uint8_t *)p_آرگومانها->منبع_حد-منبع);
    إذا(length<=0 && cnv->مقصد_Unicode_وضعیت==0) {
        /* no input, nothing to do */
        أرجع;
    }

    هدف=p_آرگومانها->هدف;
    إذا(هدف >= p_آرگومانها->هدف_حد) {
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
        أرجع;
    }

    هدف_ظرفیت=(uint32_t)(p_آرگومانها->هدف_حد-هدف);
    offsets=p_آرگومانها->offsets;
    منبع_شاخص=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    إذا(cnv->مقصد_Unicode_وضعیت!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->مقصد_UBytes[0]=(uint8_t)cnv->مقصد_Unicode_وضعیت;
        cnv->مقصد_ULength=1;
        cnv->مقصد_Unicode_وضعیت=0;
    }
    إذا((count=cnv->مقصد_ULength)!=0) {
        uint8_t *p=cnv->مقصد_UBytes;
        افعل {
            p[count++]=*منبع++;
            ++منبع_شاخص;
            --length;
            إذا(count==2) {
                c=((char16_t)p[0]<<8)|p[1];
                إذا(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *هدف++=c;
                    إذا(offsets!=اشاره_تهی) {
                        *offsets++=-1;
                    }
                    --هدف_ظرفیت;
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
                    *هدف++=c;
                    إذا(هدف_ظرفیت>=2) {
                        *هدف++=trail;
                        إذا(offsets!=اشاره_تهی) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        هدف_ظرفیت-=2;
                    } وإلا /* targetCapacity==1 */ {
                        هدف_ظرفیت=0;
                        cnv->UChar_Error_میانگیر[0]=trail;
                        cnv->UChar_Error_میانگیر_درازی=1;
                        *p_Error_کد=U_میانگیر_سرریز_ERROR;
                    }
                    count=0;
                    c=0;
                    اكسر;
                } وإلا {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_کد=U_ILLEGAL_CHAR_یافت;

                    /* back out reading the code unit after it */
                    إذا(((ثابت uint8_t *)p_آرگومانها->منبع-منبع)>=2) {
                        منبع-=2;
                    } وإلا {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->مقصد_Unicode_وضعیت=0x100|p[2];
                        --منبع;
                    }
                    cnv->مقصد_ULength=2;

                    /* write back the updated pointers */
                    p_آرگومانها->منبع=(ثابت حرف *)منبع;
                    p_آرگومانها->هدف=هدف;
                    p_آرگومانها->offsets=offsets;
                    أرجع;
                }
            }
        } طالما(length>0);
        cnv->مقصد_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*هدف_ظرفیت;
    إذا(count>length) {
        count=length&~1;
    }
    إذا(c==0 && count>0) {
        length-=count;
        count>>=1;
        هدف_ظرفیت-=count;
        إذا(offsets==اشاره_تهی) {
            افعل {
                c=((char16_t)منبع[0]<<8)|منبع[1];
                منبع+=2;
                إذا(U16_IS_SINGLE(c)) {
                    *هدف++=c;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)منبع[0]<<8)|منبع[1])
                ) {
                    منبع+=2;
                    --count;
                    *هدف++=c;
                    *هدف++=trail;
                } وإلا {
                    اكسر;
                }
            } طالما(--count>0);
        } وإلا {
            افعل {
                c=((char16_t)منبع[0]<<8)|منبع[1];
                منبع+=2;
                إذا(U16_IS_SINGLE(c)) {
                    *هدف++=c;
                    *offsets++=منبع_شاخص;
                    منبع_شاخص+=2;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)منبع[0]<<8)|منبع[1])
                ) {
                    منبع+=2;
                    --count;
                    *هدف++=c;
                    *هدف++=trail;
                    *offsets++=منبع_شاخص;
                    *offsets++=منبع_شاخص;
                    منبع_شاخص+=4;
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
            هدف_ظرفیت+=count;
        }
    }

    إذا(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->مقصد_UBytes[0]=(uint8_t)(c>>8);
        cnv->مقصد_UBytes[1]=(uint8_t)c;
        cnv->مقصد_ULength=2;

        إذا(U16_IS_SURROGATE_LEAD(c)) {
            إذا(length>=2) {
                إذا(U16_IS_TRAIL(trail=((char16_t)منبع[0]<<8)|منبع[1])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    منبع+=2;
                    length-=2;
                    *هدف++=c;
                    إذا(offsets!=اشاره_تهی) {
                        *offsets++=منبع_شاخص;
                    }
                    cnv->UChar_Error_میانگیر[0]=trail;
                    cnv->UChar_Error_میانگیر_درازی=1;
                    cnv->مقصد_ULength=0;
                    *p_Error_کد=U_میانگیر_سرریز_ERROR;
                } وإلا {
                    /* unmatched lead surrogate */
                    *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                }
            } وإلا {
                /* see if the trail surrogate is in the next buffer */
            }
        } وإلا {
            /* unmatched trail surrogate */
            *p_Error_کد=U_ILLEGAL_CHAR_یافت;
        }
    }

    إذا(U_SUCCESS(*p_Error_کد)) {
        /* check for a remaining source byte */
        إذا(length>0) {
            إذا(هدف_ظرفیت==0) {
                *p_Error_کد=U_میانگیر_سرریز_ERROR;
            } وإلا {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->مقصد_UBytes[cnv->مقصد_ULength++]=*منبع++;
            }
        }
    }

    /* write back the updated pointers */
    p_آرگومانها->منبع=(ثابت حرف *)منبع;
    p_آرگومانها->هدف=هدف;
    p_آرگومانها->offsets=offsets;
}

ساكن UChar32  U_CALLCONV
UTF16_BEGet_بعدی_UChar(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها, UError_کد *err) {
    ثابت uint8_t *s, *منبع_حد;
    UChar32 c;

    إذا(p_آرگومانها->converter->نما<8) {
        أرجع UCNV_GET_بعدی_UCHAR_USE_مقصد_U;
    }

    s=(ثابت uint8_t *)p_آرگومانها->منبع;
    منبع_حد=(ثابت uint8_t *)p_آرگومانها->منبع_حد;

    إذا(s>=منبع_حد) {
        /* no input */
        *err=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    إذا(s+2>منبع_حد) {
        /* only one byte: truncated char16_t */
        p_آرگومانها->converter->مقصد_UBytes[0]=*s++;
        p_آرگومانها->converter->مقصد_ULength=1;
        p_آرگومانها->منبع=(ثابت حرف *)s;
        *err = U_TRUNCATED_CHAR_یافت;
        أرجع 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)*s<<8)|s[1];
    s+=2;

    /* check for a surrogate pair */
    إذا(U_IS_SURROGATE(c)) {
        إذا(U16_IS_SURROGATE_LEAD(c)) {
            إذا(s+2<=منبع_حد) {
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
                uint8_t *بایتها=p_آرگومانها->converter->مقصد_UBytes;
                s-=2;
                p_آرگومانها->converter->مقصد_ULength=(int8_t)(منبع_حد-s);
                افعل {
                    *بایتها++=*s++;
                } طالما(s<منبع_حد);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_یافت;
            }
        } وإلا {
            /* unmatched trail surrogate */
            c=-2;
        }

        إذا(c<0) {
            /* write the unmatched surrogate */
            uint8_t *بایتها=p_آرگومانها->converter->مقصد_UBytes;
            p_آرگومانها->converter->مقصد_ULength=2;
            *بایتها=*(s-2);
            بایتها[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_یافت;
        }
    }

    p_آرگومانها->منبع=(ثابت حرف *)s;
    أرجع c;
} 

ساكن فراغ  U_CALLCONV
_UTF16BEReset(UConverter *cnv, UConverterResetChoice choice) {
    إذا(choice<=UCNV_RESET_مقصد_UNICODE) {
        /* reset toUnicode state */
        إذا(UCNV_GET_نسخه(cnv)==0) {
            cnv->نما=8; /* no BOM handling */
        } وإلا {
            cnv->نما=0; /* Java-specific "UnicodeBig" requires BE BOM or no BOM */
        }
    }
    إذا(choice!=UCNV_RESET_مقصد_UNICODE && UCNV_GET_نسخه(cnv)==1) {
        /* reset fromUnicode for "UnicodeBig": prepare to output the UTF-16BE BOM */
        cnv->from_Unicode_وضعیت=UCNV_NEED_مقصد_WRITE_BOM;
    }
}

ساكن فراغ  U_CALLCONV
_UTF16BEOpen(UConverter *cnv,
             UConverter_بارگیری_آرگومانها *p_آرگومانها,
             UError_کد *p_Error_کد) {
    (فراغ)p_آرگومانها;
    إذا(UCNV_GET_نسخه(cnv)<=1) {
        _UTF16BEReset(cnv, UCNV_RESET_BOTH);
    } وإلا {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
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
U_CDECL_انتها

ساكن ثابت UConverterImpl _UTF16BEImpl={
    UCNV_UTF16_BigEndian,

    اشاره_تهی,
    اشاره_تهی,

    _UTF16BEOpen,
    اشاره_تهی,
    _UTF16BEReset,

    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_BEGet_بعدی_UChar,

    اشاره_تهی,
    _UTF16BEGetName,
    اشاره_تهی,
    اشاره_تهی,
    ucnv_get_Non_Surrogate_Unicode_مجموعه,

    اشاره_تهی,
    اشاره_تهی
};

ساكن ثابت UConverter_Static_داده UTF16_BEStatic_داده={
    حجم(UConverter_Static_داده),
    "UTF-16BE",
    1200, UCNV_IBM, UCNV_UTF16_BigEndian, 2, 2,
    { 0xff, 0xfd, 0, 0 },2,خطأ,خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_داده _UTF16BEData=
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&UTF16_BEStatic_داده, &_UTF16BEImpl);

/* UTF-16LE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV
_UTF16LEFromUnicodeWithOffsets(UConverter_From_Unicode_آرگومانها *p_آرگومانها,
                               UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت char16_t *منبع;
    حرف *هدف;
    int32_t *offsets;

    uint32_t هدف_ظرفیت, length, منبع_شاخص;
    char16_t c, trail;
    حرف سرریز[4];

    منبع=p_آرگومانها->منبع;
    length=(int32_t)(p_آرگومانها->منبع_حد-منبع);
    إذا(length<=0) {
        /* no input, nothing to do */
        أرجع;
    }

    cnv=p_آرگومانها->converter;

    /* write the BOM if necessary */
    إذا(cnv->from_Unicode_وضعیت==UCNV_NEED_مقصد_WRITE_BOM) {
        ساكن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu };
        ucnv_from_UWrite_بایتها(cnv,
                             bom, 2,
                             &p_آرگومانها->هدف, p_آرگومانها->هدف_حد,
                             &p_آرگومانها->offsets, -1,
                             p_Error_کد);
        cnv->from_Unicode_وضعیت=0;
    }

    هدف=p_آرگومانها->هدف;
    إذا(هدف >= p_آرگومانها->هدف_حد) {
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
        أرجع;
    }

    هدف_ظرفیت=(uint32_t)(p_آرگومانها->هدف_حد-p_آرگومانها->هدف);
    offsets=p_آرگومانها->offsets;
    منبع_شاخص=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    إذا((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*منبع) && هدف_ظرفیت>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++منبع;
        --length;
        هدف[0]=(uint8_t)c;
        هدف[1]=(uint8_t)(c>>8);
        هدف[2]=(uint8_t)trail;
        هدف[3]=(uint8_t)(trail>>8);
        هدف+=4;
        هدف_ظرفیت-=4;
        إذا(offsets!=اشاره_تهی) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        منبع_شاخص=1;
        cnv->fromUChar32=c=0;
    }

    إذا(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        إذا(count>هدف_ظرفیت) {
            count=هدف_ظرفیت&~1;
        }
        /* count is even */
        هدف_ظرفیت-=count;
        count>>=1;
        length-=count;

        إذا(offsets==اشاره_تهی) {
            طالما(count>0) {
                c=*منبع++;
                إذا(U16_IS_SINGLE(c)) {
                    هدف[0]=(uint8_t)c;
                    هدف[1]=(uint8_t)(c>>8);
                    هدف+=2;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*منبع)) {
                    ++منبع;
                    --count;
                    هدف[0]=(uint8_t)c;
                    هدف[1]=(uint8_t)(c>>8);
                    هدف[2]=(uint8_t)trail;
                    هدف[3]=(uint8_t)(trail>>8);
                    هدف+=4;
                } وإلا {
                    اكسر;
                }
                --count;
            }
        } وإلا {
            طالما(count>0) {
                c=*منبع++;
                إذا(U16_IS_SINGLE(c)) {
                    هدف[0]=(uint8_t)c;
                    هدف[1]=(uint8_t)(c>>8);
                    هدف+=2;
                    *offsets++=منبع_شاخص;
                    *offsets++=منبع_شاخص++;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*منبع)) {
                    ++منبع;
                    --count;
                    هدف[0]=(uint8_t)c;
                    هدف[1]=(uint8_t)(c>>8);
                    هدف[2]=(uint8_t)trail;
                    هدف[3]=(uint8_t)(trail>>8);
                    هدف+=4;
                    *offsets++=منبع_شاخص;
                    *offsets++=منبع_شاخص;
                    *offsets++=منبع_شاخص;
                    *offsets++=منبع_شاخص;
                    منبع_شاخص+=2;
                } وإلا {
                    اكسر;
                }
                --count;
            }
        }

        إذا(count==0) {
            /* done with the loop for complete UChars */
            إذا(length>0 && هدف_ظرفیت>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                إذا(U16_IS_SINGLE(c=*منبع++)) {
                    سرریز[0]=(حرف)c;
                    سرریز[1]=(حرف)(c>>8);
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
            هدف_ظرفیت+=2*count;
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
            إذا(منبع<p_آرگومانها->منبع_حد) {
                إذا(U16_IS_TRAIL(trail=*منبع)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++منبع;
                    سرریز[0]=(حرف)c;
                    سرریز[1]=(حرف)(c>>8);
                    سرریز[2]=(حرف)trail;
                    سرریز[3]=(حرف)(trail>>8);
                    length=4; /* 4 bytes to output */
                    c=0;
                } وإلا {
                    /* unmatched lead surrogate */
                    *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                }
            } وإلا {
                /* see if the trail surrogate is in the next buffer */
            }
        } وإلا {
            /* unmatched trail surrogate */
            *p_Error_کد=U_ILLEGAL_CHAR_یافت;
        }
        cnv->fromUChar32=c;
    }

    إذا(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_بایتها(cnv,
                             سرریز, length,
                             &هدف, p_آرگومانها->هدف_حد,
                             &offsets, منبع_شاخص,
                             p_Error_کد);
        هدف_ظرفیت = static_cast<uint32_t>(p_آرگومانها->هدف_حد - هدف);
    }

    إذا(U_SUCCESS(*p_Error_کد) && منبع<p_آرگومانها->منبع_حد && هدف_ظرفیت==0) {
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
    }

    /* write back the updated pointers */
    p_آرگومانها->منبع=منبع;
    p_آرگومانها->هدف=هدف;
    p_آرگومانها->offsets=offsets;
}

ساكن فراغ  U_CALLCONV
_UTF16LEToUnicodeWithOffsets(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                             UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت uint8_t *منبع;
    char16_t *هدف;
    int32_t *offsets;

    uint32_t هدف_ظرفیت, length, count, منبع_شاخص;
    char16_t c, trail;

    إذا(p_آرگومانها->converter->نما<8) {
        UTF16_مقصد_Unicode_With_Offsets(p_آرگومانها, p_Error_کد);
        أرجع;
    }

    cnv=p_آرگومانها->converter;
    منبع=(ثابت uint8_t *)p_آرگومانها->منبع;
    length=(int32_t)((ثابت uint8_t *)p_آرگومانها->منبع_حد-منبع);
    إذا(length<=0 && cnv->مقصد_Unicode_وضعیت==0) {
        /* no input, nothing to do */
        أرجع;
    }

    هدف=p_آرگومانها->هدف;
    إذا(هدف >= p_آرگومانها->هدف_حد) {
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
        أرجع;
    }

    هدف_ظرفیت=(uint32_t)(p_آرگومانها->هدف_حد-p_آرگومانها->هدف);
    offsets=p_آرگومانها->offsets;
    منبع_شاخص=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    إذا(cnv->مقصد_Unicode_وضعیت!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->مقصد_UBytes[0]=(uint8_t)cnv->مقصد_Unicode_وضعیت;
        cnv->مقصد_ULength=1;
        cnv->مقصد_Unicode_وضعیت=0;
    }
    إذا((count=cnv->مقصد_ULength)!=0) {
        uint8_t *p=cnv->مقصد_UBytes;
        افعل {
            p[count++]=*منبع++;
            ++منبع_شاخص;
            --length;
            إذا(count==2) {
                c=((char16_t)p[1]<<8)|p[0];
                إذا(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *هدف++=c;
                    إذا(offsets!=اشاره_تهی) {
                        *offsets++=-1;
                    }
                    --هدف_ظرفیت;
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
                    *هدف++=c;
                    إذا(هدف_ظرفیت>=2) {
                        *هدف++=trail;
                        إذا(offsets!=اشاره_تهی) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        هدف_ظرفیت-=2;
                    } وإلا /* targetCapacity==1 */ {
                        هدف_ظرفیت=0;
                        cnv->UChar_Error_میانگیر[0]=trail;
                        cnv->UChar_Error_میانگیر_درازی=1;
                        *p_Error_کد=U_میانگیر_سرریز_ERROR;
                    }
                    count=0;
                    c=0;
                    اكسر;
                } وإلا {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_کد=U_ILLEGAL_CHAR_یافت;

                    /* back out reading the code unit after it */
                    إذا(((ثابت uint8_t *)p_آرگومانها->منبع-منبع)>=2) {
                        منبع-=2;
                    } وإلا {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->مقصد_Unicode_وضعیت=0x100|p[2];
                        --منبع;
                    }
                    cnv->مقصد_ULength=2;

                    /* write back the updated pointers */
                    p_آرگومانها->منبع=(ثابت حرف *)منبع;
                    p_آرگومانها->هدف=هدف;
                    p_آرگومانها->offsets=offsets;
                    أرجع;
                }
            }
        } طالما(length>0);
        cnv->مقصد_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*هدف_ظرفیت;
    إذا(count>length) {
        count=length&~1;
    }
    إذا(c==0 && count>0) {
        length-=count;
        count>>=1;
        هدف_ظرفیت-=count;
        إذا(offsets==اشاره_تهی) {
            افعل {
                c=((char16_t)منبع[1]<<8)|منبع[0];
                منبع+=2;
                إذا(U16_IS_SINGLE(c)) {
                    *هدف++=c;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)منبع[1]<<8)|منبع[0])
                ) {
                    منبع+=2;
                    --count;
                    *هدف++=c;
                    *هدف++=trail;
                } وإلا {
                    اكسر;
                }
            } طالما(--count>0);
        } وإلا {
            افعل {
                c=((char16_t)منبع[1]<<8)|منبع[0];
                منبع+=2;
                إذا(U16_IS_SINGLE(c)) {
                    *هدف++=c;
                    *offsets++=منبع_شاخص;
                    منبع_شاخص+=2;
                } وإلا إذا(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)منبع[1]<<8)|منبع[0])
                ) {
                    منبع+=2;
                    --count;
                    *هدف++=c;
                    *هدف++=trail;
                    *offsets++=منبع_شاخص;
                    *offsets++=منبع_شاخص;
                    منبع_شاخص+=4;
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
            هدف_ظرفیت+=count;
        }
    }

    إذا(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->مقصد_UBytes[0]=(uint8_t)c;
        cnv->مقصد_UBytes[1]=(uint8_t)(c>>8);
        cnv->مقصد_ULength=2;

        إذا(U16_IS_SURROGATE_LEAD(c)) {
            إذا(length>=2) {
                إذا(U16_IS_TRAIL(trail=((char16_t)منبع[1]<<8)|منبع[0])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    منبع+=2;
                    length-=2;
                    *هدف++=c;
                    إذا(offsets!=اشاره_تهی) {
                        *offsets++=منبع_شاخص;
                    }
                    cnv->UChar_Error_میانگیر[0]=trail;
                    cnv->UChar_Error_میانگیر_درازی=1;
                    cnv->مقصد_ULength=0;
                    *p_Error_کد=U_میانگیر_سرریز_ERROR;
                } وإلا {
                    /* unmatched lead surrogate */
                    *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                }
            } وإلا {
                /* see if the trail surrogate is in the next buffer */
            }
        } وإلا {
            /* unmatched trail surrogate */
            *p_Error_کد=U_ILLEGAL_CHAR_یافت;
        }
    }

    إذا(U_SUCCESS(*p_Error_کد)) {
        /* check for a remaining source byte */
        إذا(length>0) {
            إذا(هدف_ظرفیت==0) {
                *p_Error_کد=U_میانگیر_سرریز_ERROR;
            } وإلا {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->مقصد_UBytes[cnv->مقصد_ULength++]=*منبع++;
            }
        }
    }

    /* write back the updated pointers */
    p_آرگومانها->منبع=(ثابت حرف *)منبع;
    p_آرگومانها->هدف=هدف;
    p_آرگومانها->offsets=offsets;
}

ساكن UChar32  U_CALLCONV
UTF16_LEGet_بعدی_UChar(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها, UError_کد *err) {
    ثابت uint8_t *s, *منبع_حد;
    UChar32 c;

    إذا(p_آرگومانها->converter->نما<8) {
        أرجع UCNV_GET_بعدی_UCHAR_USE_مقصد_U;
    }

    s=(ثابت uint8_t *)p_آرگومانها->منبع;
    منبع_حد=(ثابت uint8_t *)p_آرگومانها->منبع_حد;

    إذا(s>=منبع_حد) {
        /* no input */
        *err=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0xffff;
    }

    إذا(s+2>منبع_حد) {
        /* only one byte: truncated char16_t */
        p_آرگومانها->converter->مقصد_UBytes[0]=*s++;
        p_آرگومانها->converter->مقصد_ULength=1;
        p_آرگومانها->منبع=(ثابت حرف *)s;
        *err = U_TRUNCATED_CHAR_یافت;
        أرجع 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)s[1]<<8)|*s;
    s+=2;

    /* check for a surrogate pair */
    إذا(U_IS_SURROGATE(c)) {
        إذا(U16_IS_SURROGATE_LEAD(c)) {
            إذا(s+2<=منبع_حد) {
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
                uint8_t *بایتها=p_آرگومانها->converter->مقصد_UBytes;
                s-=2;
                p_آرگومانها->converter->مقصد_ULength=(int8_t)(منبع_حد-s);
                افعل {
                    *بایتها++=*s++;
                } طالما(s<منبع_حد);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_یافت;
            }
        } وإلا {
            /* unmatched trail surrogate */
            c=-2;
        }

        إذا(c<0) {
            /* write the unmatched surrogate */
            uint8_t *بایتها=p_آرگومانها->converter->مقصد_UBytes;
            p_آرگومانها->converter->مقصد_ULength=2;
            *بایتها=*(s-2);
            بایتها[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_یافت;
        }
    }

    p_آرگومانها->منبع=(ثابت حرف *)s;
    أرجع c;
} 

ساكن فراغ  U_CALLCONV
_UTF16LEReset(UConverter *cnv, UConverterResetChoice choice) {
    إذا(choice<=UCNV_RESET_مقصد_UNICODE) {
        /* reset toUnicode state */
        إذا(UCNV_GET_نسخه(cnv)==0) {
            cnv->نما=8; /* no BOM handling */
        } وإلا {
            cnv->نما=0; /* Java-specific "UnicodeLittle" requires LE BOM or no BOM */
        }
    }
    إذا(choice!=UCNV_RESET_مقصد_UNICODE && UCNV_GET_نسخه(cnv)==1) {
        /* reset fromUnicode for "UnicodeLittle": prepare to output the UTF-16LE BOM */
        cnv->from_Unicode_وضعیت=UCNV_NEED_مقصد_WRITE_BOM;
    }
}

ساكن فراغ  U_CALLCONV
_UTF16LEOpen(UConverter *cnv,
             UConverter_بارگیری_آرگومانها *p_آرگومانها,
             UError_کد *p_Error_کد) {
    (فراغ)p_آرگومانها;
    إذا(UCNV_GET_نسخه(cnv)<=1) {
        _UTF16LEReset(cnv, UCNV_RESET_BOTH);
    } وإلا {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
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
U_CDECL_انتها

ساكن ثابت UConverterImpl _UTF16LEImpl={
    UCNV_UTF16_LittleEndian,

    اشاره_تهی,
    اشاره_تهی,

    _UTF16LEOpen,
    اشاره_تهی,
    _UTF16LEReset,

    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    UTF16_LEGet_بعدی_UChar,

    اشاره_تهی,
    _UTF16LEGetName,
    اشاره_تهی,
    اشاره_تهی,
    ucnv_get_Non_Surrogate_Unicode_مجموعه,

    اشاره_تهی,
    اشاره_تهی
};


ساكن ثابت UConverter_Static_داده UTF16_LEStatic_داده={
    حجم(UConverter_Static_داده),
    "UTF-16LE",
    1202, UCNV_IBM, UCNV_UTF16_LittleEndian, 2, 2,
    { 0xfd, 0xff, 0, 0 },2,خطأ,خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_داده _UTF16LEData=
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&UTF16_LEStatic_داده, &_UTF16LEImpl);

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
    إذا(choice<=UCNV_RESET_مقصد_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->نما=0;
    }
    إذا(choice!=UCNV_RESET_مقصد_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-16PE BOM */
        cnv->from_Unicode_وضعیت=UCNV_NEED_مقصد_WRITE_BOM;
    }
}
U_CDECL_انتها
خارجي ثابت UConverter_Shared_داده UTF16v2_داده;
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
_UTF16Open(UConverter *cnv,
           UConverter_بارگیری_آرگومانها *p_آرگومانها,
           UError_کد *p_Error_کد) {
    إذا(UCNV_GET_نسخه(cnv)<=2) {
        إذا(UCNV_GET_نسخه(cnv)==2 && !p_آرگومانها->onlyTestIsLoadable) {
            /*
             * Switch implementation, and switch the staticData that's different
             * and was copied into the UConverter.
             * (See ucnv_createConverterFromSharedData() in ucnv_bld.c.)
             * UTF-16,version=2 fromUnicode() always writes a big-endian byte stream.
             */
            cnv->shared_داده=(UConverter_Shared_داده*)&UTF16v2_داده;
            uprv_memcpy(cnv->subChars, UTF16v2_داده.static_داده->subChar, UCNV_MAX_SUBCHAR_LEN);
        }
        _UTF16Reset(cnv, UCNV_RESET_BOTH);
    } وإلا {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
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
U_CDECL_انتها
خارجي ثابت UConverter_Shared_داده UTF16_داده;

ساكن مضمن منطقي IS_UTF16BE(ثابت UConverter *cnv) {
    أرجع ((cnv)->shared_داده == &_UTF16BEData);
}

ساكن مضمن منطقي IS_UTF16LE(ثابت UConverter *cnv) {
    أرجع ((cnv)->shared_داده == &_UTF16LEData);
}

ساكن مضمن منطقي IS_UTF16(ثابت UConverter *cnv) {
    أرجع ((cnv)->shared_داده==&UTF16_داده) || ((cnv)->shared_داده == &UTF16v2_داده);
}

U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
UTF16_مقصد_Unicode_With_Offsets(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                           UError_کد *p_Error_کد) {
    UConverter *cnv=p_آرگومانها->converter;
    ثابت حرف *منبع=p_آرگومانها->منبع;
    ثابت حرف *منبع_حد=p_آرگومانها->منبع_حد;
    int32_t *offsets=p_آرگومانها->offsets;

    int32_t state, جابه_جایی_Delta;
    uint8_t b;

    state=cnv->نما;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    جابه_جایی_Delta=0;

    طالما(منبع<منبع_حد && U_SUCCESS(*p_Error_کد)) {
        بدل(state) {
        حالة 0:
            cnv->مقصد_UBytes[0]=(uint8_t)*منبع++;
            cnv->مقصد_ULength=1;
            state=1;
            اكسر;
        حالة 1:
            /*
             * Only inside this switch case can the state variable
             * temporarily take two additional values:
             * 6: BOM error, continue with BE
             * 7: BOM error, continue with LE
             */
            b=*منبع;
            إذا(cnv->مقصد_UBytes[0]==0xfe && b==0xff) {
                إذا(IS_UTF16LE(cnv)) {
                    state=7; /* illegal reverse BOM for Java "UnicodeLittle" */
                } وإلا {
                    state=8; /* detect UTF-16BE */
                }
            } وإلا إذا(cnv->مقصد_UBytes[0]==0xff && b==0xfe) {
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
                ++منبع;
                cnv->مقصد_ULength=0;
                جابه_جایی_Delta=(int32_t)(منبع-p_آرگومانها->منبع);
            } وإلا إذا(state<6) {
                /* ok: no BOM, and not a reverse BOM */
                إذا(منبع!=p_آرگومانها->منبع) {
                    /* reset the source for a correct first offset */
                    منبع=p_آرگومانها->منبع;
                    cnv->مقصد_ULength=0;
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
                cnv->مقصد_UBytes[1]=b;
                cnv->مقصد_ULength=2;
                p_آرگومانها->منبع=منبع+1;
                /* continue with conversion if the callback resets the error */
                /*
                 * Make Java "Unicode" default to BE like standard UTF-16.
                 * Make Java "UnicodeBig" and "UnicodeLittle" default
                 * to their normal endiannesses.
                 */
                cnv->نما=state+2;
                *p_Error_کد=U_ILLEGAL_ESCAPE_SEQUENCE;
                أرجع;
            }
            /* convert the rest of the stream */
            cnv->نما=state;
            استمر;
        حالة 8:
            /* call UTF-16BE */
            p_آرگومانها->منبع=منبع;
            _UTF16BEToUnicodeWithOffsets(p_آرگومانها, p_Error_کد);
            منبع=p_آرگومانها->منبع;
            اكسر;
        حالة 9:
            /* call UTF-16LE */
            p_آرگومانها->منبع=منبع;
            _UTF16LEToUnicodeWithOffsets(p_آرگومانها, p_Error_کد);
            منبع=p_آرگومانها->منبع;
            اكسر;
        افتراضي:
            اكسر; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    إذا(offsets!=اشاره_تهی && جابه_جایی_Delta!=0) {
        int32_t *offsets_حد=p_آرگومانها->offsets;
        طالما(offsets<offsets_حد) {
            *offsets++ += جابه_جایی_Delta;
        }
    }

    p_آرگومانها->منبع=منبع;

    إذا(منبع==منبع_حد && p_آرگومانها->flush) {
        /* handle truncated input */
        بدل(state) {
        حالة 0:
            اكسر; /* no input at all, nothing to do */
        حالة 8:
            _UTF16BEToUnicodeWithOffsets(p_آرگومانها, p_Error_کد);
            اكسر;
        حالة 9:
            _UTF16LEToUnicodeWithOffsets(p_آرگومانها, p_Error_کد);
            اكسر;
        افتراضي:
            /* 0<state<8: framework will report truncation, nothing to do here */
            اكسر;
        }
    }

    cnv->نما=state;
}

ساكن UChar32 U_CALLCONV
UTF16_Get_بعدی_UChar(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                   UError_کد *p_Error_کد) {
    بدل(p_آرگومانها->converter->نما) {
    حالة 8:
        أرجع UTF16_BEGet_بعدی_UChar(p_آرگومانها, p_Error_کد);
    حالة 9:
        أرجع UTF16_LEGet_بعدی_UChar(p_آرگومانها, p_Error_کد);
    افتراضي:
        أرجع UCNV_GET_بعدی_UCHAR_USE_مقصد_U;
    }
}
U_CDECL_انتها

ساكن ثابت UConverterImpl _UTF16Impl = {
    UCNV_UTF16,

    اشاره_تهی,
    اشاره_تهی,

    _UTF16Open,
    اشاره_تهی,
    _UTF16Reset,

    UTF16_مقصد_Unicode_With_Offsets,
    UTF16_مقصد_Unicode_With_Offsets,
    _UTF16PEFromUnicodeWithOffsets,
    _UTF16PEFromUnicodeWithOffsets,
    UTF16_Get_بعدی_UChar,

    اشاره_تهی, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    اشاره_تهی,
    اشاره_تهی,
    ucnv_get_Non_Surrogate_Unicode_مجموعه,

    اشاره_تهی,
    اشاره_تهی
};

ساكن ثابت UConverter_Static_داده UTF16_Static_داده = {
    حجم(UConverter_Static_داده),
    "UTF-16",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
#إذا U_IS_BIG_ENDIAN
    { 0xff, 0xfd, 0, 0 }, 2,
#وإلا
    { 0xfd, 0xff, 0, 0 }, 2,
#پایان_اگر
    خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_داده UTF16_داده =
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&UTF16_Static_داده, &_UTF16Impl);

ساكن ثابت UConverterImpl _UTF16v2Impl = {
    UCNV_UTF16,

    اشاره_تهی,
    اشاره_تهی,

    _UTF16Open,
    اشاره_تهی,
    _UTF16Reset,

    UTF16_مقصد_Unicode_With_Offsets,
    UTF16_مقصد_Unicode_With_Offsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_Get_بعدی_UChar,

    اشاره_تهی, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    اشاره_تهی,
    اشاره_تهی,
    ucnv_get_Non_Surrogate_Unicode_مجموعه,

    اشاره_تهی,
    اشاره_تهی
};

ساكن ثابت UConverter_Static_داده UTF16v2_Static_داده = {
    حجم(UConverter_Static_داده),
    "UTF-16,version=2",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
    { 0xff, 0xfd, 0, 0 }, 2,
    خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_داده UTF16v2_داده =
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&UTF16v2_Static_داده, &_UTF16v2Impl);

#پایان_اگر
