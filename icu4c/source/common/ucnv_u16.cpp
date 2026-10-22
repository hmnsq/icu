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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل "unicode/ucnv.h"
#شامل "unicode/uversion.h"
#شامل "ucnv_bld.h"
#شامل "ucnv_cnv.h"
#شامل "cmemory.h"

شمار {
    UCNV_NEED_مقصد_WRITE_BOM=1
};

U_CDECL_BEGIN
/*
 * The UTF-16 toUnicode implementation is also used for the Java-specific
 * "with BOM" variants of UTF-16BE and UTF-16LE.
 */
ساکن خالی  U_CALLCONV
UTF16_مقصد_Unicode_With_Offsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                           UError_رمز *p_Error_رمز);

/* UTF-16BE ----------------------------------------------------------------- */

#اگر U_IS_BIG_ENDIAN
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16BEFromUnicodeWithOffsets
#ورنہ
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16LEFromUnicodeWithOffsets
#اگر_ختم


ساکن خالی  U_CALLCONV
_UTF16BEFromUnicodeWithOffsets(UConverter_From_Unicode_دلائل *p_دلائل,
                               UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت char16_t *ماخذ;
    حرف *ہدف;
    int32_t *offsets;

    uint32_t ہدف_استعداد, length, ماخذ_اشاریہ;
    char16_t c, trail;
    حرف حد_سے_تجاوز[4];

    ماخذ=p_دلائل->ماخذ;
    length=(int32_t)(p_دلائل->ماخذ_حد_بندی-ماخذ);
    اگر(length<=0) {
        /* no input, nothing to do */
        واپس;
    }

    cnv=p_دلائل->converter;

    /* write the BOM if necessary */
    اگر(cnv->from_Unicode_حالت==UCNV_NEED_مقصد_WRITE_BOM) {
        ساکن ثابت حرف bom[]={ (حرف)0xfeu, (حرف)0xffu };
        ucnv_from_UWrite_بائٹیں(cnv,
                             bom, 2,
                             &p_دلائل->ہدف, p_دلائل->ہدف_حد_بندی,
                             &p_دلائل->offsets, -1,
                             p_Error_رمز);
        cnv->from_Unicode_حالت=0;
    }

    ہدف=p_دلائل->ہدف;
    اگر(ہدف >= p_دلائل->ہدف_حد_بندی) {
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        واپس;
    }

    ہدف_استعداد=(uint32_t)(p_دلائل->ہدف_حد_بندی-ہدف);
    offsets=p_دلائل->offsets;
    ماخذ_اشاریہ=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    اگر((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*ماخذ) && ہدف_استعداد>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++ماخذ;
        --length;
        ہدف[0]=(uint8_t)(c>>8);
        ہدف[1]=(uint8_t)c;
        ہدف[2]=(uint8_t)(trail>>8);
        ہدف[3]=(uint8_t)trail;
        ہدف+=4;
        ہدف_استعداد-=4;
        اگر(offsets!=خالی_اشاریہ) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        ماخذ_اشاریہ=1;
        cnv->fromUChar32=c=0;
    }

    اگر(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        اگر(count>ہدف_استعداد) {
            count=ہدف_استعداد&~1;
        }
        /* count is even */
        ہدف_استعداد-=count;
        count>>=1;
        length-=count;

        اگر(offsets==خالی_اشاریہ) {
            جب(count>0) {
                c=*ماخذ++;
                اگر(U16_IS_SINGLE(c)) {
                    ہدف[0]=(uint8_t)(c>>8);
                    ہدف[1]=(uint8_t)c;
                    ہدف+=2;
                } ورنہ اگر(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ماخذ)) {
                    ++ماخذ;
                    --count;
                    ہدف[0]=(uint8_t)(c>>8);
                    ہدف[1]=(uint8_t)c;
                    ہدف[2]=(uint8_t)(trail>>8);
                    ہدف[3]=(uint8_t)trail;
                    ہدف+=4;
                } ورنہ {
                    توڑو;
                }
                --count;
            }
        } ورنہ {
            جب(count>0) {
                c=*ماخذ++;
                اگر(U16_IS_SINGLE(c)) {
                    ہدف[0]=(uint8_t)(c>>8);
                    ہدف[1]=(uint8_t)c;
                    ہدف+=2;
                    *offsets++=ماخذ_اشاریہ;
                    *offsets++=ماخذ_اشاریہ++;
                } ورنہ اگر(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ماخذ)) {
                    ++ماخذ;
                    --count;
                    ہدف[0]=(uint8_t)(c>>8);
                    ہدف[1]=(uint8_t)c;
                    ہدف[2]=(uint8_t)(trail>>8);
                    ہدف[3]=(uint8_t)trail;
                    ہدف+=4;
                    *offsets++=ماخذ_اشاریہ;
                    *offsets++=ماخذ_اشاریہ;
                    *offsets++=ماخذ_اشاریہ;
                    *offsets++=ماخذ_اشاریہ;
                    ماخذ_اشاریہ+=2;
                } ورنہ {
                    توڑو;
                }
                --count;
            }
        }

        اگر(count==0) {
            /* done with the loop for complete UChars */
            اگر(length>0 && ہدف_استعداد>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                اگر(U16_IS_SINGLE(c=*ماخذ++)) {
                    حد_سے_تجاوز[0]=(حرف)(c>>8);
                    حد_سے_تجاوز[1]=(حرف)c;
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } ورنہ {
                length=0;
                c=0;
            }
        } ورنہ {
            /* keep c for surrogate handling, length will be set there */
            ہدف_استعداد+=2*count;
        }
    } ورنہ {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    اگر(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        اگر(U16_IS_SURROGATE_LEAD(c)) {
            اگر(ماخذ<p_دلائل->ماخذ_حد_بندی) {
                اگر(U16_IS_TRAIL(trail=*ماخذ)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++ماخذ;
                    حد_سے_تجاوز[0]=(حرف)(c>>8);
                    حد_سے_تجاوز[1]=(حرف)c;
                    حد_سے_تجاوز[2]=(حرف)(trail>>8);
                    حد_سے_تجاوز[3]=(حرف)trail;
                    length=4; /* 4 bytes to output */
                    c=0;
                } ورنہ {
                    /* unmatched lead surrogate */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                }
            } ورنہ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ورنہ {
            /* unmatched trail surrogate */
            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
        }
        cnv->fromUChar32=c;
    }

    اگر(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_بائٹیں(cnv,
                             حد_سے_تجاوز, length,
                             &ہدف, p_دلائل->ہدف_حد_بندی,
                             &offsets, ماخذ_اشاریہ,
                             p_Error_رمز);
        ہدف_استعداد = static_cast<uint32_t>(p_دلائل->ہدف_حد_بندی - ہدف);
    }

    اگر(U_SUCCESS(*p_Error_رمز) && ماخذ<p_دلائل->ماخذ_حد_بندی && ہدف_استعداد==0) {
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
    }

    /* write back the updated pointers */
    p_دلائل->ماخذ=ماخذ;
    p_دلائل->ہدف = ہدف;
    p_دلائل->offsets=offsets;
}

ساکن خالی  U_CALLCONV
_UTF16BEToUnicodeWithOffsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                             UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت uint8_t *ماخذ;
    char16_t *ہدف;
    int32_t *offsets;

    uint32_t ہدف_استعداد, length, count, ماخذ_اشاریہ;
    char16_t c, trail;

    اگر(p_دلائل->converter->منوال<8) {
        UTF16_مقصد_Unicode_With_Offsets(p_دلائل, p_Error_رمز);
        واپس;
    }

    cnv=p_دلائل->converter;
    ماخذ=(ثابت uint8_t *)p_دلائل->ماخذ;
    length=(int32_t)((ثابت uint8_t *)p_دلائل->ماخذ_حد_بندی-ماخذ);
    اگر(length<=0 && cnv->مقصد_Unicode_حالت==0) {
        /* no input, nothing to do */
        واپس;
    }

    ہدف=p_دلائل->ہدف;
    اگر(ہدف >= p_دلائل->ہدف_حد_بندی) {
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        واپس;
    }

    ہدف_استعداد=(uint32_t)(p_دلائل->ہدف_حد_بندی-ہدف);
    offsets=p_دلائل->offsets;
    ماخذ_اشاریہ=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    اگر(cnv->مقصد_Unicode_حالت!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->مقصد_UBytes[0]=(uint8_t)cnv->مقصد_Unicode_حالت;
        cnv->مقصد_ULength=1;
        cnv->مقصد_Unicode_حالت=0;
    }
    اگر((count=cnv->مقصد_ULength)!=0) {
        uint8_t *p=cnv->مقصد_UBytes;
        کرو {
            p[count++]=*ماخذ++;
            ++ماخذ_اشاریہ;
            --length;
            اگر(count==2) {
                c=((char16_t)p[0]<<8)|p[1];
                اگر(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *ہدف++=c;
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=-1;
                    }
                    --ہدف_استعداد;
                    count=0;
                    c=0;
                    توڑو;
                } ورنہ اگر(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } ورنہ {
                    /* fall through to error handling for an unmatched trail surrogate */
                    توڑو;
                }
            } ورنہ اگر(count==4) {
                c=((char16_t)p[0]<<8)|p[1];
                trail=((char16_t)p[2]<<8)|p[3];
                اگر(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *ہدف++=c;
                    اگر(ہدف_استعداد>=2) {
                        *ہدف++=trail;
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        ہدف_استعداد-=2;
                    } ورنہ /* targetCapacity==1 */ {
                        ہدف_استعداد=0;
                        cnv->UChar_Error_بفر[0]=trail;
                        cnv->UChar_Error_بفر_لمبائی=1;
                        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                    }
                    count=0;
                    c=0;
                    توڑو;
                } ورنہ {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;

                    /* back out reading the code unit after it */
                    اگر(((ثابت uint8_t *)p_دلائل->ماخذ-ماخذ)>=2) {
                        ماخذ-=2;
                    } ورنہ {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->مقصد_Unicode_حالت=0x100|p[2];
                        --ماخذ;
                    }
                    cnv->مقصد_ULength=2;

                    /* write back the updated pointers */
                    p_دلائل->ماخذ=(ثابت حرف *)ماخذ;
                    p_دلائل->ہدف=ہدف;
                    p_دلائل->offsets=offsets;
                    واپس;
                }
            }
        } جب(length>0);
        cnv->مقصد_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*ہدف_استعداد;
    اگر(count>length) {
        count=length&~1;
    }
    اگر(c==0 && count>0) {
        length-=count;
        count>>=1;
        ہدف_استعداد-=count;
        اگر(offsets==خالی_اشاریہ) {
            کرو {
                c=((char16_t)ماخذ[0]<<8)|ماخذ[1];
                ماخذ+=2;
                اگر(U16_IS_SINGLE(c)) {
                    *ہدف++=c;
                } ورنہ اگر(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ماخذ[0]<<8)|ماخذ[1])
                ) {
                    ماخذ+=2;
                    --count;
                    *ہدف++=c;
                    *ہدف++=trail;
                } ورنہ {
                    توڑو;
                }
            } جب(--count>0);
        } ورنہ {
            کرو {
                c=((char16_t)ماخذ[0]<<8)|ماخذ[1];
                ماخذ+=2;
                اگر(U16_IS_SINGLE(c)) {
                    *ہدف++=c;
                    *offsets++=ماخذ_اشاریہ;
                    ماخذ_اشاریہ+=2;
                } ورنہ اگر(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ماخذ[0]<<8)|ماخذ[1])
                ) {
                    ماخذ+=2;
                    --count;
                    *ہدف++=c;
                    *ہدف++=trail;
                    *offsets++=ماخذ_اشاریہ;
                    *offsets++=ماخذ_اشاریہ;
                    ماخذ_اشاریہ+=4;
                } ورنہ {
                    توڑو;
                }
            } جب(--count>0);
        }

        اگر(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } ورنہ {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            ہدف_استعداد+=count;
        }
    }

    اگر(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->مقصد_UBytes[0]=(uint8_t)(c>>8);
        cnv->مقصد_UBytes[1]=(uint8_t)c;
        cnv->مقصد_ULength=2;

        اگر(U16_IS_SURROGATE_LEAD(c)) {
            اگر(length>=2) {
                اگر(U16_IS_TRAIL(trail=((char16_t)ماخذ[0]<<8)|ماخذ[1])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ماخذ+=2;
                    length-=2;
                    *ہدف++=c;
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                    cnv->UChar_Error_بفر[0]=trail;
                    cnv->UChar_Error_بفر_لمبائی=1;
                    cnv->مقصد_ULength=0;
                    *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                } ورنہ {
                    /* unmatched lead surrogate */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                }
            } ورنہ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ورنہ {
            /* unmatched trail surrogate */
            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
        }
    }

    اگر(U_SUCCESS(*p_Error_رمز)) {
        /* check for a remaining source byte */
        اگر(length>0) {
            اگر(ہدف_استعداد==0) {
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            } ورنہ {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->مقصد_UBytes[cnv->مقصد_ULength++]=*ماخذ++;
            }
        }
    }

    /* write back the updated pointers */
    p_دلائل->ماخذ=(ثابت حرف *)ماخذ;
    p_دلائل->ہدف=ہدف;
    p_دلائل->offsets=offsets;
}

ساکن UChar32  U_CALLCONV
UTF16_BEGet_اگلا_UChar(UConverter_مقصد_Unicode_دلائل *p_دلائل, UError_رمز *err) {
    ثابت uint8_t *s, *ماخذ_حد_بندی;
    UChar32 c;

    اگر(p_دلائل->converter->منوال<8) {
        واپس UCNV_GET_اگلا_UCHAR_USE_مقصد_U;
    }

    s=(ثابت uint8_t *)p_دلائل->ماخذ;
    ماخذ_حد_بندی=(ثابت uint8_t *)p_دلائل->ماخذ_حد_بندی;

    اگر(s>=ماخذ_حد_بندی) {
        /* no input */
        *err=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0xffff;
    }

    اگر(s+2>ماخذ_حد_بندی) {
        /* only one byte: truncated char16_t */
        p_دلائل->converter->مقصد_UBytes[0]=*s++;
        p_دلائل->converter->مقصد_ULength=1;
        p_دلائل->ماخذ=(ثابت حرف *)s;
        *err = U_TRUNCATED_CHAR_ملا;
        واپس 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)*s<<8)|s[1];
    s+=2;

    /* check for a surrogate pair */
    اگر(U_IS_SURROGATE(c)) {
        اگر(U16_IS_SURROGATE_LEAD(c)) {
            اگر(s+2<=ماخذ_حد_بندی) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)*s<<8)|s[1];
                اگر(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } ورنہ {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } ورنہ {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *بائٹیں=p_دلائل->converter->مقصد_UBytes;
                s-=2;
                p_دلائل->converter->مقصد_ULength=(int8_t)(ماخذ_حد_بندی-s);
                کرو {
                    *بائٹیں++=*s++;
                } جب(s<ماخذ_حد_بندی);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_ملا;
            }
        } ورنہ {
            /* unmatched trail surrogate */
            c=-2;
        }

        اگر(c<0) {
            /* write the unmatched surrogate */
            uint8_t *بائٹیں=p_دلائل->converter->مقصد_UBytes;
            p_دلائل->converter->مقصد_ULength=2;
            *بائٹیں=*(s-2);
            بائٹیں[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_ملا;
        }
    }

    p_دلائل->ماخذ=(ثابت حرف *)s;
    واپس c;
} 

ساکن خالی  U_CALLCONV
_UTF16BEReset(UConverter *cnv, UConverterResetChoice choice) {
    اگر(choice<=UCNV_RESET_مقصد_UNICODE) {
        /* reset toUnicode state */
        اگر(UCNV_GET_نسخہ(cnv)==0) {
            cnv->منوال=8; /* no BOM handling */
        } ورنہ {
            cnv->منوال=0; /* Java-specific "UnicodeBig" requires BE BOM or no BOM */
        }
    }
    اگر(choice!=UCNV_RESET_مقصد_UNICODE && UCNV_GET_نسخہ(cnv)==1) {
        /* reset fromUnicode for "UnicodeBig": prepare to output the UTF-16BE BOM */
        cnv->from_Unicode_حالت=UCNV_NEED_مقصد_WRITE_BOM;
    }
}

ساکن خالی  U_CALLCONV
_UTF16BEOpen(UConverter *cnv,
             UConverter_بھرو_دلائل *p_دلائل,
             UError_رمز *p_Error_رمز) {
    (خالی)p_دلائل;
    اگر(UCNV_GET_نسخہ(cnv)<=1) {
        _UTF16BEReset(cnv, UCNV_RESET_BOTH);
    } ورنہ {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
    }
}

ساکن ثابت حرف *  U_CALLCONV
_UTF16BEGetName(ثابت UConverter *cnv) {
    اگر(UCNV_GET_نسخہ(cnv)==0) {
        واپس "UTF-16BE";
    } ورنہ {
        واپس "UTF-16BE,version=1";
    }
}
U_CDECL_انتہا

ساکن ثابت UConverterImpl _UTF16BEImpl={
    UCNV_UTF16_BigEndian,

    خالی_اشاریہ,
    خالی_اشاریہ,

    _UTF16BEOpen,
    خالی_اشاریہ,
    _UTF16BEReset,

    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_BEGet_اگلا_UChar,

    خالی_اشاریہ,
    _UTF16BEGetName,
    خالی_اشاریہ,
    خالی_اشاریہ,
    ucnv_get_Non_Surrogate_Unicode_سیٹ,

    خالی_اشاریہ,
    خالی_اشاریہ
};

ساکن ثابت UConverter_Static_ڈیٹا UTF16_BEStatic_ڈیٹا={
    حجم(UConverter_Static_ڈیٹا),
    "UTF-16BE",
    1200, UCNV_IBM, UCNV_UTF16_BigEndian, 2, 2,
    { 0xff, 0xfd, 0, 0 },2,جھوٹ,جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_ڈیٹا _UTF16BEData=
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&UTF16_BEStatic_ڈیٹا, &_UTF16BEImpl);

/* UTF-16LE ----------------------------------------------------------------- */
U_CDECL_BEGIN
ساکن خالی  U_CALLCONV
_UTF16LEFromUnicodeWithOffsets(UConverter_From_Unicode_دلائل *p_دلائل,
                               UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت char16_t *ماخذ;
    حرف *ہدف;
    int32_t *offsets;

    uint32_t ہدف_استعداد, length, ماخذ_اشاریہ;
    char16_t c, trail;
    حرف حد_سے_تجاوز[4];

    ماخذ=p_دلائل->ماخذ;
    length=(int32_t)(p_دلائل->ماخذ_حد_بندی-ماخذ);
    اگر(length<=0) {
        /* no input, nothing to do */
        واپس;
    }

    cnv=p_دلائل->converter;

    /* write the BOM if necessary */
    اگر(cnv->from_Unicode_حالت==UCNV_NEED_مقصد_WRITE_BOM) {
        ساکن ثابت حرف bom[]={ (حرف)0xffu, (حرف)0xfeu };
        ucnv_from_UWrite_بائٹیں(cnv,
                             bom, 2,
                             &p_دلائل->ہدف, p_دلائل->ہدف_حد_بندی,
                             &p_دلائل->offsets, -1,
                             p_Error_رمز);
        cnv->from_Unicode_حالت=0;
    }

    ہدف=p_دلائل->ہدف;
    اگر(ہدف >= p_دلائل->ہدف_حد_بندی) {
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        واپس;
    }

    ہدف_استعداد=(uint32_t)(p_دلائل->ہدف_حد_بندی-p_دلائل->ہدف);
    offsets=p_دلائل->offsets;
    ماخذ_اشاریہ=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    اگر((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*ماخذ) && ہدف_استعداد>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++ماخذ;
        --length;
        ہدف[0]=(uint8_t)c;
        ہدف[1]=(uint8_t)(c>>8);
        ہدف[2]=(uint8_t)trail;
        ہدف[3]=(uint8_t)(trail>>8);
        ہدف+=4;
        ہدف_استعداد-=4;
        اگر(offsets!=خالی_اشاریہ) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        ماخذ_اشاریہ=1;
        cnv->fromUChar32=c=0;
    }

    اگر(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        اگر(count>ہدف_استعداد) {
            count=ہدف_استعداد&~1;
        }
        /* count is even */
        ہدف_استعداد-=count;
        count>>=1;
        length-=count;

        اگر(offsets==خالی_اشاریہ) {
            جب(count>0) {
                c=*ماخذ++;
                اگر(U16_IS_SINGLE(c)) {
                    ہدف[0]=(uint8_t)c;
                    ہدف[1]=(uint8_t)(c>>8);
                    ہدف+=2;
                } ورنہ اگر(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ماخذ)) {
                    ++ماخذ;
                    --count;
                    ہدف[0]=(uint8_t)c;
                    ہدف[1]=(uint8_t)(c>>8);
                    ہدف[2]=(uint8_t)trail;
                    ہدف[3]=(uint8_t)(trail>>8);
                    ہدف+=4;
                } ورنہ {
                    توڑو;
                }
                --count;
            }
        } ورنہ {
            جب(count>0) {
                c=*ماخذ++;
                اگر(U16_IS_SINGLE(c)) {
                    ہدف[0]=(uint8_t)c;
                    ہدف[1]=(uint8_t)(c>>8);
                    ہدف+=2;
                    *offsets++=ماخذ_اشاریہ;
                    *offsets++=ماخذ_اشاریہ++;
                } ورنہ اگر(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*ماخذ)) {
                    ++ماخذ;
                    --count;
                    ہدف[0]=(uint8_t)c;
                    ہدف[1]=(uint8_t)(c>>8);
                    ہدف[2]=(uint8_t)trail;
                    ہدف[3]=(uint8_t)(trail>>8);
                    ہدف+=4;
                    *offsets++=ماخذ_اشاریہ;
                    *offsets++=ماخذ_اشاریہ;
                    *offsets++=ماخذ_اشاریہ;
                    *offsets++=ماخذ_اشاریہ;
                    ماخذ_اشاریہ+=2;
                } ورنہ {
                    توڑو;
                }
                --count;
            }
        }

        اگر(count==0) {
            /* done with the loop for complete UChars */
            اگر(length>0 && ہدف_استعداد>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                اگر(U16_IS_SINGLE(c=*ماخذ++)) {
                    حد_سے_تجاوز[0]=(حرف)c;
                    حد_سے_تجاوز[1]=(حرف)(c>>8);
                    length=2; /* 2 bytes to output */
                    c=0;
                /* } else { keep c for surrogate handling, length will be set there */
                }
            } ورنہ {
                length=0;
                c=0;
            }
        } ورنہ {
            /* keep c for surrogate handling, length will be set there */
            ہدف_استعداد+=2*count;
        }
    } ورنہ {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    اگر(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        اگر(U16_IS_SURROGATE_LEAD(c)) {
            اگر(ماخذ<p_دلائل->ماخذ_حد_بندی) {
                اگر(U16_IS_TRAIL(trail=*ماخذ)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++ماخذ;
                    حد_سے_تجاوز[0]=(حرف)c;
                    حد_سے_تجاوز[1]=(حرف)(c>>8);
                    حد_سے_تجاوز[2]=(حرف)trail;
                    حد_سے_تجاوز[3]=(حرف)(trail>>8);
                    length=4; /* 4 bytes to output */
                    c=0;
                } ورنہ {
                    /* unmatched lead surrogate */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                }
            } ورنہ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ورنہ {
            /* unmatched trail surrogate */
            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
        }
        cnv->fromUChar32=c;
    }

    اگر(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_بائٹیں(cnv,
                             حد_سے_تجاوز, length,
                             &ہدف, p_دلائل->ہدف_حد_بندی,
                             &offsets, ماخذ_اشاریہ,
                             p_Error_رمز);
        ہدف_استعداد = static_cast<uint32_t>(p_دلائل->ہدف_حد_بندی - ہدف);
    }

    اگر(U_SUCCESS(*p_Error_رمز) && ماخذ<p_دلائل->ماخذ_حد_بندی && ہدف_استعداد==0) {
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
    }

    /* write back the updated pointers */
    p_دلائل->ماخذ=ماخذ;
    p_دلائل->ہدف=ہدف;
    p_دلائل->offsets=offsets;
}

ساکن خالی  U_CALLCONV
_UTF16LEToUnicodeWithOffsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                             UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت uint8_t *ماخذ;
    char16_t *ہدف;
    int32_t *offsets;

    uint32_t ہدف_استعداد, length, count, ماخذ_اشاریہ;
    char16_t c, trail;

    اگر(p_دلائل->converter->منوال<8) {
        UTF16_مقصد_Unicode_With_Offsets(p_دلائل, p_Error_رمز);
        واپس;
    }

    cnv=p_دلائل->converter;
    ماخذ=(ثابت uint8_t *)p_دلائل->ماخذ;
    length=(int32_t)((ثابت uint8_t *)p_دلائل->ماخذ_حد_بندی-ماخذ);
    اگر(length<=0 && cnv->مقصد_Unicode_حالت==0) {
        /* no input, nothing to do */
        واپس;
    }

    ہدف=p_دلائل->ہدف;
    اگر(ہدف >= p_دلائل->ہدف_حد_بندی) {
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        واپس;
    }

    ہدف_استعداد=(uint32_t)(p_دلائل->ہدف_حد_بندی-p_دلائل->ہدف);
    offsets=p_دلائل->offsets;
    ماخذ_اشاریہ=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    اگر(cnv->مقصد_Unicode_حالت!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->مقصد_UBytes[0]=(uint8_t)cnv->مقصد_Unicode_حالت;
        cnv->مقصد_ULength=1;
        cnv->مقصد_Unicode_حالت=0;
    }
    اگر((count=cnv->مقصد_ULength)!=0) {
        uint8_t *p=cnv->مقصد_UBytes;
        کرو {
            p[count++]=*ماخذ++;
            ++ماخذ_اشاریہ;
            --length;
            اگر(count==2) {
                c=((char16_t)p[1]<<8)|p[0];
                اگر(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *ہدف++=c;
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=-1;
                    }
                    --ہدف_استعداد;
                    count=0;
                    c=0;
                    توڑو;
                } ورنہ اگر(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } ورنہ {
                    /* fall through to error handling for an unmatched trail surrogate */
                    توڑو;
                }
            } ورنہ اگر(count==4) {
                c=((char16_t)p[1]<<8)|p[0];
                trail=((char16_t)p[3]<<8)|p[2];
                اگر(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *ہدف++=c;
                    اگر(ہدف_استعداد>=2) {
                        *ہدف++=trail;
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        ہدف_استعداد-=2;
                    } ورنہ /* targetCapacity==1 */ {
                        ہدف_استعداد=0;
                        cnv->UChar_Error_بفر[0]=trail;
                        cnv->UChar_Error_بفر_لمبائی=1;
                        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                    }
                    count=0;
                    c=0;
                    توڑو;
                } ورنہ {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;

                    /* back out reading the code unit after it */
                    اگر(((ثابت uint8_t *)p_دلائل->ماخذ-ماخذ)>=2) {
                        ماخذ-=2;
                    } ورنہ {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->مقصد_Unicode_حالت=0x100|p[2];
                        --ماخذ;
                    }
                    cnv->مقصد_ULength=2;

                    /* write back the updated pointers */
                    p_دلائل->ماخذ=(ثابت حرف *)ماخذ;
                    p_دلائل->ہدف=ہدف;
                    p_دلائل->offsets=offsets;
                    واپس;
                }
            }
        } جب(length>0);
        cnv->مقصد_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*ہدف_استعداد;
    اگر(count>length) {
        count=length&~1;
    }
    اگر(c==0 && count>0) {
        length-=count;
        count>>=1;
        ہدف_استعداد-=count;
        اگر(offsets==خالی_اشاریہ) {
            کرو {
                c=((char16_t)ماخذ[1]<<8)|ماخذ[0];
                ماخذ+=2;
                اگر(U16_IS_SINGLE(c)) {
                    *ہدف++=c;
                } ورنہ اگر(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ماخذ[1]<<8)|ماخذ[0])
                ) {
                    ماخذ+=2;
                    --count;
                    *ہدف++=c;
                    *ہدف++=trail;
                } ورنہ {
                    توڑو;
                }
            } جب(--count>0);
        } ورنہ {
            کرو {
                c=((char16_t)ماخذ[1]<<8)|ماخذ[0];
                ماخذ+=2;
                اگر(U16_IS_SINGLE(c)) {
                    *ہدف++=c;
                    *offsets++=ماخذ_اشاریہ;
                    ماخذ_اشاریہ+=2;
                } ورنہ اگر(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)ماخذ[1]<<8)|ماخذ[0])
                ) {
                    ماخذ+=2;
                    --count;
                    *ہدف++=c;
                    *ہدف++=trail;
                    *offsets++=ماخذ_اشاریہ;
                    *offsets++=ماخذ_اشاریہ;
                    ماخذ_اشاریہ+=4;
                } ورنہ {
                    توڑو;
                }
            } جب(--count>0);
        }

        اگر(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } ورنہ {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            ہدف_استعداد+=count;
        }
    }

    اگر(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->مقصد_UBytes[0]=(uint8_t)c;
        cnv->مقصد_UBytes[1]=(uint8_t)(c>>8);
        cnv->مقصد_ULength=2;

        اگر(U16_IS_SURROGATE_LEAD(c)) {
            اگر(length>=2) {
                اگر(U16_IS_TRAIL(trail=((char16_t)ماخذ[1]<<8)|ماخذ[0])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ماخذ+=2;
                    length-=2;
                    *ہدف++=c;
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                    cnv->UChar_Error_بفر[0]=trail;
                    cnv->UChar_Error_بفر_لمبائی=1;
                    cnv->مقصد_ULength=0;
                    *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                } ورنہ {
                    /* unmatched lead surrogate */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                }
            } ورنہ {
                /* see if the trail surrogate is in the next buffer */
            }
        } ورنہ {
            /* unmatched trail surrogate */
            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
        }
    }

    اگر(U_SUCCESS(*p_Error_رمز)) {
        /* check for a remaining source byte */
        اگر(length>0) {
            اگر(ہدف_استعداد==0) {
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            } ورنہ {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->مقصد_UBytes[cnv->مقصد_ULength++]=*ماخذ++;
            }
        }
    }

    /* write back the updated pointers */
    p_دلائل->ماخذ=(ثابت حرف *)ماخذ;
    p_دلائل->ہدف=ہدف;
    p_دلائل->offsets=offsets;
}

ساکن UChar32  U_CALLCONV
UTF16_LEGet_اگلا_UChar(UConverter_مقصد_Unicode_دلائل *p_دلائل, UError_رمز *err) {
    ثابت uint8_t *s, *ماخذ_حد_بندی;
    UChar32 c;

    اگر(p_دلائل->converter->منوال<8) {
        واپس UCNV_GET_اگلا_UCHAR_USE_مقصد_U;
    }

    s=(ثابت uint8_t *)p_دلائل->ماخذ;
    ماخذ_حد_بندی=(ثابت uint8_t *)p_دلائل->ماخذ_حد_بندی;

    اگر(s>=ماخذ_حد_بندی) {
        /* no input */
        *err=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0xffff;
    }

    اگر(s+2>ماخذ_حد_بندی) {
        /* only one byte: truncated char16_t */
        p_دلائل->converter->مقصد_UBytes[0]=*s++;
        p_دلائل->converter->مقصد_ULength=1;
        p_دلائل->ماخذ=(ثابت حرف *)s;
        *err = U_TRUNCATED_CHAR_ملا;
        واپس 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)s[1]<<8)|*s;
    s+=2;

    /* check for a surrogate pair */
    اگر(U_IS_SURROGATE(c)) {
        اگر(U16_IS_SURROGATE_LEAD(c)) {
            اگر(s+2<=ماخذ_حد_بندی) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)s[1]<<8)|*s;
                اگر(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } ورنہ {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } ورنہ {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *بائٹیں=p_دلائل->converter->مقصد_UBytes;
                s-=2;
                p_دلائل->converter->مقصد_ULength=(int8_t)(ماخذ_حد_بندی-s);
                کرو {
                    *بائٹیں++=*s++;
                } جب(s<ماخذ_حد_بندی);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_ملا;
            }
        } ورنہ {
            /* unmatched trail surrogate */
            c=-2;
        }

        اگر(c<0) {
            /* write the unmatched surrogate */
            uint8_t *بائٹیں=p_دلائل->converter->مقصد_UBytes;
            p_دلائل->converter->مقصد_ULength=2;
            *بائٹیں=*(s-2);
            بائٹیں[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_ملا;
        }
    }

    p_دلائل->ماخذ=(ثابت حرف *)s;
    واپس c;
} 

ساکن خالی  U_CALLCONV
_UTF16LEReset(UConverter *cnv, UConverterResetChoice choice) {
    اگر(choice<=UCNV_RESET_مقصد_UNICODE) {
        /* reset toUnicode state */
        اگر(UCNV_GET_نسخہ(cnv)==0) {
            cnv->منوال=8; /* no BOM handling */
        } ورنہ {
            cnv->منوال=0; /* Java-specific "UnicodeLittle" requires LE BOM or no BOM */
        }
    }
    اگر(choice!=UCNV_RESET_مقصد_UNICODE && UCNV_GET_نسخہ(cnv)==1) {
        /* reset fromUnicode for "UnicodeLittle": prepare to output the UTF-16LE BOM */
        cnv->from_Unicode_حالت=UCNV_NEED_مقصد_WRITE_BOM;
    }
}

ساکن خالی  U_CALLCONV
_UTF16LEOpen(UConverter *cnv,
             UConverter_بھرو_دلائل *p_دلائل,
             UError_رمز *p_Error_رمز) {
    (خالی)p_دلائل;
    اگر(UCNV_GET_نسخہ(cnv)<=1) {
        _UTF16LEReset(cnv, UCNV_RESET_BOTH);
    } ورنہ {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
    }
}

ساکن ثابت حرف *  U_CALLCONV
_UTF16LEGetName(ثابت UConverter *cnv) {
    اگر(UCNV_GET_نسخہ(cnv)==0) {
        واپس "UTF-16LE";
    } ورنہ {
        واپس "UTF-16LE,version=1";
    }
}
U_CDECL_انتہا

ساکن ثابت UConverterImpl _UTF16LEImpl={
    UCNV_UTF16_LittleEndian,

    خالی_اشاریہ,
    خالی_اشاریہ,

    _UTF16LEOpen,
    خالی_اشاریہ,
    _UTF16LEReset,

    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    UTF16_LEGet_اگلا_UChar,

    خالی_اشاریہ,
    _UTF16LEGetName,
    خالی_اشاریہ,
    خالی_اشاریہ,
    ucnv_get_Non_Surrogate_Unicode_سیٹ,

    خالی_اشاریہ,
    خالی_اشاریہ
};


ساکن ثابت UConverter_Static_ڈیٹا UTF16_LEStatic_ڈیٹا={
    حجم(UConverter_Static_ڈیٹا),
    "UTF-16LE",
    1202, UCNV_IBM, UCNV_UTF16_LittleEndian, 2, 2,
    { 0xfd, 0xff, 0, 0 },2,جھوٹ,جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ثابت UConverter_Shared_ڈیٹا _UTF16LEData=
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&UTF16_LEStatic_ڈیٹا, &_UTF16LEImpl);

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
ساکن خالی  U_CALLCONV
_UTF16Reset(UConverter *cnv, UConverterResetChoice choice) {
    اگر(choice<=UCNV_RESET_مقصد_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->منوال=0;
    }
    اگر(choice!=UCNV_RESET_مقصد_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-16PE BOM */
        cnv->from_Unicode_حالت=UCNV_NEED_مقصد_WRITE_BOM;
    }
}
U_CDECL_انتہا
خارجی ثابت UConverter_Shared_ڈیٹا UTF16v2_ڈیٹا;
U_CDECL_BEGIN
ساکن خالی U_CALLCONV
_UTF16Open(UConverter *cnv,
           UConverter_بھرو_دلائل *p_دلائل,
           UError_رمز *p_Error_رمز) {
    اگر(UCNV_GET_نسخہ(cnv)<=2) {
        اگر(UCNV_GET_نسخہ(cnv)==2 && !p_دلائل->onlyTestIsLoadable) {
            /*
             * Switch implementation, and switch the staticData that's different
             * and was copied into the UConverter.
             * (See ucnv_createConverterFromSharedData() in ucnv_bld.c.)
             * UTF-16,version=2 fromUnicode() always writes a big-endian byte stream.
             */
            cnv->shared_ڈیٹا=(UConverter_Shared_ڈیٹا*)&UTF16v2_ڈیٹا;
            uprv_memcpy(cnv->subChars, UTF16v2_ڈیٹا.static_ڈیٹا->subChar, UCNV_MAX_SUBCHAR_LEN);
        }
        _UTF16Reset(cnv, UCNV_RESET_BOTH);
    } ورنہ {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
    }
}

ساکن ثابت حرف *  U_CALLCONV
_UTF16GetName(ثابت UConverter *cnv) {
    اگر(UCNV_GET_نسخہ(cnv)==0) {
        واپس "UTF-16";
    } ورنہ اگر(UCNV_GET_نسخہ(cnv)==1) {
        واپس "UTF-16,version=1";
    } ورنہ {
        واپس "UTF-16,version=2";
    }
}
U_CDECL_انتہا
خارجی ثابت UConverter_Shared_ڈیٹا UTF16_ڈیٹا;

ساکن اندرونی منطقی IS_UTF16BE(ثابت UConverter *cnv) {
    واپس ((cnv)->shared_ڈیٹا == &_UTF16BEData);
}

ساکن اندرونی منطقی IS_UTF16LE(ثابت UConverter *cnv) {
    واپس ((cnv)->shared_ڈیٹا == &_UTF16LEData);
}

ساکن اندرونی منطقی IS_UTF16(ثابت UConverter *cnv) {
    واپس ((cnv)->shared_ڈیٹا==&UTF16_ڈیٹا) || ((cnv)->shared_ڈیٹا == &UTF16v2_ڈیٹا);
}

U_CDECL_BEGIN
ساکن خالی U_CALLCONV
UTF16_مقصد_Unicode_With_Offsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                           UError_رمز *p_Error_رمز) {
    UConverter *cnv=p_دلائل->converter;
    ثابت حرف *ماخذ=p_دلائل->ماخذ;
    ثابت حرف *ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
    int32_t *offsets=p_دلائل->offsets;

    int32_t state, انحراف_Delta;
    uint8_t b;

    state=cnv->منوال;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    انحراف_Delta=0;

    جب(ماخذ<ماخذ_حد_بندی && U_SUCCESS(*p_Error_رمز)) {
        انتخاب(state) {
        حالت 0:
            cnv->مقصد_UBytes[0]=(uint8_t)*ماخذ++;
            cnv->مقصد_ULength=1;
            state=1;
            توڑو;
        حالت 1:
            /*
             * Only inside this switch case can the state variable
             * temporarily take two additional values:
             * 6: BOM error, continue with BE
             * 7: BOM error, continue with LE
             */
            b=*ماخذ;
            اگر(cnv->مقصد_UBytes[0]==0xfe && b==0xff) {
                اگر(IS_UTF16LE(cnv)) {
                    state=7; /* illegal reverse BOM for Java "UnicodeLittle" */
                } ورنہ {
                    state=8; /* detect UTF-16BE */
                }
            } ورنہ اگر(cnv->مقصد_UBytes[0]==0xff && b==0xfe) {
                اگر(IS_UTF16BE(cnv)) {
                    state=6; /* illegal reverse BOM for Java "UnicodeBig" */
                } ورنہ {
                    state=9; /* detect UTF-16LE */
                }
            } ورنہ اگر((IS_UTF16(cnv) && UCNV_GET_نسخہ(cnv)==1)) {
                state=6; /* illegal missing BOM for Java "Unicode" */
            }
            اگر(state>=8) {
                /* BOM detected, consume it */
                ++ماخذ;
                cnv->مقصد_ULength=0;
                انحراف_Delta=(int32_t)(ماخذ-p_دلائل->ماخذ);
            } ورنہ اگر(state<6) {
                /* ok: no BOM, and not a reverse BOM */
                اگر(ماخذ!=p_دلائل->ماخذ) {
                    /* reset the source for a correct first offset */
                    ماخذ=p_دلائل->ماخذ;
                    cnv->مقصد_ULength=0;
                }
                اگر(IS_UTF16LE(cnv)) {
                    /* Make Java "UnicodeLittle" default to LE. */
                    state=9;
                } ورنہ {
                    /* Make standard UTF-16 and Java "UnicodeBig" default to BE. */
                    state=8;
                }
            } ورنہ {
                /*
                 * error: missing BOM, or reverse BOM
                 * UTF-16,version=1: Java-specific "Unicode" requires a BOM.
                 * UTF-16BE,version=1: Java-specific "UnicodeBig" requires a BE BOM or no BOM.
                 * UTF-16LE,version=1: Java-specific "UnicodeLittle" requires an LE BOM or no BOM.
                 */
                /* report the non-BOM or reverse BOM as an illegal sequence */
                cnv->مقصد_UBytes[1]=b;
                cnv->مقصد_ULength=2;
                p_دلائل->ماخذ=ماخذ+1;
                /* continue with conversion if the callback resets the error */
                /*
                 * Make Java "Unicode" default to BE like standard UTF-16.
                 * Make Java "UnicodeBig" and "UnicodeLittle" default
                 * to their normal endiannesses.
                 */
                cnv->منوال=state+2;
                *p_Error_رمز=U_ILLEGAL_ESCAPE_SEQUENCE;
                واپس;
            }
            /* convert the rest of the stream */
            cnv->منوال=state;
            جاری;
        حالت 8:
            /* call UTF-16BE */
            p_دلائل->ماخذ=ماخذ;
            _UTF16BEToUnicodeWithOffsets(p_دلائل, p_Error_رمز);
            ماخذ=p_دلائل->ماخذ;
            توڑو;
        حالت 9:
            /* call UTF-16LE */
            p_دلائل->ماخذ=ماخذ;
            _UTF16LEToUnicodeWithOffsets(p_دلائل, p_Error_رمز);
            ماخذ=p_دلائل->ماخذ;
            توڑو;
        طےشدہ:
            توڑو; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    اگر(offsets!=خالی_اشاریہ && انحراف_Delta!=0) {
        int32_t *offsets_حد_بندی=p_دلائل->offsets;
        جب(offsets<offsets_حد_بندی) {
            *offsets++ += انحراف_Delta;
        }
    }

    p_دلائل->ماخذ=ماخذ;

    اگر(ماخذ==ماخذ_حد_بندی && p_دلائل->flush) {
        /* handle truncated input */
        انتخاب(state) {
        حالت 0:
            توڑو; /* no input at all, nothing to do */
        حالت 8:
            _UTF16BEToUnicodeWithOffsets(p_دلائل, p_Error_رمز);
            توڑو;
        حالت 9:
            _UTF16LEToUnicodeWithOffsets(p_دلائل, p_Error_رمز);
            توڑو;
        طےشدہ:
            /* 0<state<8: framework will report truncation, nothing to do here */
            توڑو;
        }
    }

    cnv->منوال=state;
}

ساکن UChar32 U_CALLCONV
UTF16_Get_اگلا_UChar(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                   UError_رمز *p_Error_رمز) {
    انتخاب(p_دلائل->converter->منوال) {
    حالت 8:
        واپس UTF16_BEGet_اگلا_UChar(p_دلائل, p_Error_رمز);
    حالت 9:
        واپس UTF16_LEGet_اگلا_UChar(p_دلائل, p_Error_رمز);
    طےشدہ:
        واپس UCNV_GET_اگلا_UCHAR_USE_مقصد_U;
    }
}
U_CDECL_انتہا

ساکن ثابت UConverterImpl _UTF16Impl = {
    UCNV_UTF16,

    خالی_اشاریہ,
    خالی_اشاریہ,

    _UTF16Open,
    خالی_اشاریہ,
    _UTF16Reset,

    UTF16_مقصد_Unicode_With_Offsets,
    UTF16_مقصد_Unicode_With_Offsets,
    _UTF16PEFromUnicodeWithOffsets,
    _UTF16PEFromUnicodeWithOffsets,
    UTF16_Get_اگلا_UChar,

    خالی_اشاریہ, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    خالی_اشاریہ,
    خالی_اشاریہ,
    ucnv_get_Non_Surrogate_Unicode_سیٹ,

    خالی_اشاریہ,
    خالی_اشاریہ
};

ساکن ثابت UConverter_Static_ڈیٹا UTF16_Static_ڈیٹا = {
    حجم(UConverter_Static_ڈیٹا),
    "UTF-16",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
#اگر U_IS_BIG_ENDIAN
    { 0xff, 0xfd, 0, 0 }, 2,
#ورنہ
    { 0xfd, 0xff, 0, 0 }, 2,
#اگر_ختم
    جھوٹ, جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_ڈیٹا UTF16_ڈیٹا =
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&UTF16_Static_ڈیٹا, &_UTF16Impl);

ساکن ثابت UConverterImpl _UTF16v2Impl = {
    UCNV_UTF16,

    خالی_اشاریہ,
    خالی_اشاریہ,

    _UTF16Open,
    خالی_اشاریہ,
    _UTF16Reset,

    UTF16_مقصد_Unicode_With_Offsets,
    UTF16_مقصد_Unicode_With_Offsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_Get_اگلا_UChar,

    خالی_اشاریہ, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    خالی_اشاریہ,
    خالی_اشاریہ,
    ucnv_get_Non_Surrogate_Unicode_سیٹ,

    خالی_اشاریہ,
    خالی_اشاریہ
};

ساکن ثابت UConverter_Static_ڈیٹا UTF16v2_Static_ڈیٹا = {
    حجم(UConverter_Static_ڈیٹا),
    "UTF-16,version=2",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
    { 0xff, 0xfd, 0, 0 }, 2,
    جھوٹ, جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_ڈیٹا UTF16v2_ڈیٹا =
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&UTF16v2_Static_ڈیٹا, &_UTF16v2Impl);

#اگر_ختم
