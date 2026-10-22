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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/uversion.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "cmemory.h"

गणना {
    UCNV_NEED_गन्तव्यम्_WRITE_BOM=1
};

U_CDECL_BEGIN
/*
 * The UTF-16 toUnicode implementation is also used for the Java-specific
 * "with BOM" variants of UTF-16BE and UTF-16LE.
 */
स्थिर शून्य  U_CALLCONV
UTF16_गन्तव्यम्_Unicode_With_Offsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                           UError_संहिता *p_Error_संहिता);

/* UTF-16BE ----------------------------------------------------------------- */

#यदि U_IS_BIG_ENDIAN
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16BEFromUnicodeWithOffsets
#अन्यथा
#   define _UTF16PEFromUnicodeWithOffsets   _UTF16LEFromUnicodeWithOffsets
#यदि_अन्तः


स्थिर शून्य  U_CALLCONV
_UTF16BEFromUnicodeWithOffsets(UConverter_From_Unicode_तर्काः *p_तर्काः,
                               UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव char16_t *source;
    वर्ण *target;
    int32_t *offsets;

    uint32_t target_क्षमता, length, source_सूचकांक;
    char16_t c, trail;
    वर्ण अतिप्रवाहः[4];

    source=p_तर्काः->source;
    length=(int32_t)(p_तर्काः->source_सीमा-source);
    यदि(length<=0) {
        /* no input, nothing to do */
        प्रत्यागम;
    }

    cnv=p_तर्काः->converter;

    /* write the BOM if necessary */
    यदि(cnv->from_Unicode_स्थितिः==UCNV_NEED_गन्तव्यम्_WRITE_BOM) {
        स्थिर ध्रुव वर्ण bom[]={ (वर्ण)0xfeu, (वर्ण)0xffu };
        ucnv_from_UWrite_अष्टकानि(cnv,
                             bom, 2,
                             &p_तर्काः->target, p_तर्काः->target_सीमा,
                             &p_तर्काः->offsets, -1,
                             p_Error_संहिता);
        cnv->from_Unicode_स्थितिः=0;
    }

    target=p_तर्काः->target;
    यदि(target >= p_तर्काः->target_सीमा) {
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम;
    }

    target_क्षमता=(uint32_t)(p_तर्काः->target_सीमा-target);
    offsets=p_तर्काः->offsets;
    source_सूचकांक=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    यदि((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*source) && target_क्षमता>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++source;
        --length;
        target[0]=(uint8_t)(c>>8);
        target[1]=(uint8_t)c;
        target[2]=(uint8_t)(trail>>8);
        target[3]=(uint8_t)trail;
        target+=4;
        target_क्षमता-=4;
        यदि(offsets!=शून्यसूचकः) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        source_सूचकांक=1;
        cnv->fromUChar32=c=0;
    }

    यदि(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        यदि(count>target_क्षमता) {
            count=target_क्षमता&~1;
        }
        /* count is even */
        target_क्षमता-=count;
        count>>=1;
        length-=count;

        यदि(offsets==शून्यसूचकः) {
            यावत्(count>0) {
                c=*source++;
                यदि(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target+=2;
                } अन्यथा यदि(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target[2]=(uint8_t)(trail>>8);
                    target[3]=(uint8_t)trail;
                    target+=4;
                } अन्यथा {
                    भङ्ग;
                }
                --count;
            }
        } अन्यथा {
            यावत्(count>0) {
                c=*source++;
                यदि(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)(c>>8);
                    target[1]=(uint8_t)c;
                    target+=2;
                    *offsets++=source_सूचकांक;
                    *offsets++=source_सूचकांक++;
                } अन्यथा यदि(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
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
                    भङ्ग;
                }
                --count;
            }
        }

        यदि(count==0) {
            /* done with the loop for complete UChars */
            यदि(length>0 && target_क्षमता>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                यदि(U16_IS_SINGLE(c=*source++)) {
                    अतिप्रवाहः[0]=(वर्ण)(c>>8);
                    अतिप्रवाहः[1]=(वर्ण)c;
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
            target_क्षमता+=2*count;
        }
    } अन्यथा {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    यदि(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        यदि(U16_IS_SURROGATE_LEAD(c)) {
            यदि(source<p_तर्काः->source_सीमा) {
                यदि(U16_IS_TRAIL(trail=*source)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++source;
                    अतिप्रवाहः[0]=(वर्ण)(c>>8);
                    अतिप्रवाहः[1]=(वर्ण)c;
                    अतिप्रवाहः[2]=(वर्ण)(trail>>8);
                    अतिप्रवाहः[3]=(वर्ण)trail;
                    length=4; /* 4 bytes to output */
                    c=0;
                } अन्यथा {
                    /* unmatched lead surrogate */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                }
            } अन्यथा {
                /* see if the trail surrogate is in the next buffer */
            }
        } अन्यथा {
            /* unmatched trail surrogate */
            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
        }
        cnv->fromUChar32=c;
    }

    यदि(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_अष्टकानि(cnv,
                             अतिप्रवाहः, length,
                             &target, p_तर्काः->target_सीमा,
                             &offsets, source_सूचकांक,
                             p_Error_संहिता);
        target_क्षमता = static_cast<uint32_t>(p_तर्काः->target_सीमा - target);
    }

    यदि(U_SUCCESS(*p_Error_संहिता) && source<p_तर्काः->source_सीमा && target_क्षमता==0) {
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
    }

    /* write back the updated pointers */
    p_तर्काः->source=source;
    p_तर्काः->target = target;
    p_तर्काः->offsets=offsets;
}

स्थिर शून्य  U_CALLCONV
_UTF16BEToUnicodeWithOffsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                             UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव uint8_t *source;
    char16_t *target;
    int32_t *offsets;

    uint32_t target_क्षमता, length, count, source_सूचकांक;
    char16_t c, trail;

    यदि(p_तर्काः->converter->बहुलकम्<8) {
        UTF16_गन्तव्यम्_Unicode_With_Offsets(p_तर्काः, p_Error_संहिता);
        प्रत्यागम;
    }

    cnv=p_तर्काः->converter;
    source=(ध्रुव uint8_t *)p_तर्काः->source;
    length=(int32_t)((ध्रुव uint8_t *)p_तर्काः->source_सीमा-source);
    यदि(length<=0 && cnv->गन्तव्यम्_Unicode_स्थितिः==0) {
        /* no input, nothing to do */
        प्रत्यागम;
    }

    target=p_तर्काः->target;
    यदि(target >= p_तर्काः->target_सीमा) {
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम;
    }

    target_क्षमता=(uint32_t)(p_तर्काः->target_सीमा-target);
    offsets=p_तर्काः->offsets;
    source_सूचकांक=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    यदि(cnv->गन्तव्यम्_Unicode_स्थितिः!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->गन्तव्यम्_UBytes[0]=(uint8_t)cnv->गन्तव्यम्_Unicode_स्थितिः;
        cnv->गन्तव्यम्_ULength=1;
        cnv->गन्तव्यम्_Unicode_स्थितिः=0;
    }
    यदि((count=cnv->गन्तव्यम्_ULength)!=0) {
        uint8_t *p=cnv->गन्तव्यम्_UBytes;
        कुरु {
            p[count++]=*source++;
            ++source_सूचकांक;
            --length;
            यदि(count==2) {
                c=((char16_t)p[0]<<8)|p[1];
                यदि(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *target++=c;
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=-1;
                    }
                    --target_क्षमता;
                    count=0;
                    c=0;
                    भङ्ग;
                } अन्यथा यदि(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } अन्यथा {
                    /* fall through to error handling for an unmatched trail surrogate */
                    भङ्ग;
                }
            } अन्यथा यदि(count==4) {
                c=((char16_t)p[0]<<8)|p[1];
                trail=((char16_t)p[2]<<8)|p[3];
                यदि(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *target++=c;
                    यदि(target_क्षमता>=2) {
                        *target++=trail;
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        target_क्षमता-=2;
                    } अन्यथा /* targetCapacity==1 */ {
                        target_क्षमता=0;
                        cnv->UChar_Error_बफर[0]=trail;
                        cnv->UChar_Error_बफर_दीर्घता=1;
                        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                    }
                    count=0;
                    c=0;
                    भङ्ग;
                } अन्यथा {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;

                    /* back out reading the code unit after it */
                    यदि(((ध्रुव uint8_t *)p_तर्काः->source-source)>=2) {
                        source-=2;
                    } अन्यथा {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->गन्तव्यम्_Unicode_स्थितिः=0x100|p[2];
                        --source;
                    }
                    cnv->गन्तव्यम्_ULength=2;

                    /* write back the updated pointers */
                    p_तर्काः->source=(ध्रुव वर्ण *)source;
                    p_तर्काः->target=target;
                    p_तर्काः->offsets=offsets;
                    प्रत्यागम;
                }
            }
        } यावत्(length>0);
        cnv->गन्तव्यम्_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*target_क्षमता;
    यदि(count>length) {
        count=length&~1;
    }
    यदि(c==0 && count>0) {
        length-=count;
        count>>=1;
        target_क्षमता-=count;
        यदि(offsets==शून्यसूचकः) {
            कुरु {
                c=((char16_t)source[0]<<8)|source[1];
                source+=2;
                यदि(U16_IS_SINGLE(c)) {
                    *target++=c;
                } अन्यथा यदि(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                } अन्यथा {
                    भङ्ग;
                }
            } यावत्(--count>0);
        } अन्यथा {
            कुरु {
                c=((char16_t)source[0]<<8)|source[1];
                source+=2;
                यदि(U16_IS_SINGLE(c)) {
                    *target++=c;
                    *offsets++=source_सूचकांक;
                    source_सूचकांक+=2;
                } अन्यथा यदि(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
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
                    भङ्ग;
                }
            } यावत्(--count>0);
        }

        यदि(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } अन्यथा {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            target_क्षमता+=count;
        }
    }

    यदि(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->गन्तव्यम्_UBytes[0]=(uint8_t)(c>>8);
        cnv->गन्तव्यम्_UBytes[1]=(uint8_t)c;
        cnv->गन्तव्यम्_ULength=2;

        यदि(U16_IS_SURROGATE_LEAD(c)) {
            यदि(length>=2) {
                यदि(U16_IS_TRAIL(trail=((char16_t)source[0]<<8)|source[1])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    source+=2;
                    length-=2;
                    *target++=c;
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                    cnv->UChar_Error_बफर[0]=trail;
                    cnv->UChar_Error_बफर_दीर्घता=1;
                    cnv->गन्तव्यम्_ULength=0;
                    *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                } अन्यथा {
                    /* unmatched lead surrogate */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                }
            } अन्यथा {
                /* see if the trail surrogate is in the next buffer */
            }
        } अन्यथा {
            /* unmatched trail surrogate */
            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
        }
    }

    यदि(U_SUCCESS(*p_Error_संहिता)) {
        /* check for a remaining source byte */
        यदि(length>0) {
            यदि(target_क्षमता==0) {
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            } अन्यथा {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->गन्तव्यम्_UBytes[cnv->गन्तव्यम्_ULength++]=*source++;
            }
        }
    }

    /* write back the updated pointers */
    p_तर्काः->source=(ध्रुव वर्ण *)source;
    p_तर्काः->target=target;
    p_तर्काः->offsets=offsets;
}

स्थिर UChar32  U_CALLCONV
UTF16_BEGet_अग्र_UChar(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः, UError_संहिता *err) {
    ध्रुव uint8_t *s, *source_सीमा;
    UChar32 c;

    यदि(p_तर्काः->converter->बहुलकम्<8) {
        प्रत्यागम UCNV_GET_अग्र_UCHAR_USE_गन्तव्यम्_U;
    }

    s=(ध्रुव uint8_t *)p_तर्काः->source;
    source_सीमा=(ध्रुव uint8_t *)p_तर्काः->source_सीमा;

    यदि(s>=source_सीमा) {
        /* no input */
        *err=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0xffff;
    }

    यदि(s+2>source_सीमा) {
        /* only one byte: truncated char16_t */
        p_तर्काः->converter->गन्तव्यम्_UBytes[0]=*s++;
        p_तर्काः->converter->गन्तव्यम्_ULength=1;
        p_तर्काः->source=(ध्रुव वर्ण *)s;
        *err = U_TRUNCATED_CHAR_प्राप्तम्;
        प्रत्यागम 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)*s<<8)|s[1];
    s+=2;

    /* check for a surrogate pair */
    यदि(U_IS_SURROGATE(c)) {
        यदि(U16_IS_SURROGATE_LEAD(c)) {
            यदि(s+2<=source_सीमा) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)*s<<8)|s[1];
                यदि(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } अन्यथा {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } अन्यथा {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *अष्टकानि=p_तर्काः->converter->गन्तव्यम्_UBytes;
                s-=2;
                p_तर्काः->converter->गन्तव्यम्_ULength=(int8_t)(source_सीमा-s);
                कुरु {
                    *अष्टकानि++=*s++;
                } यावत्(s<source_सीमा);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_प्राप्तम्;
            }
        } अन्यथा {
            /* unmatched trail surrogate */
            c=-2;
        }

        यदि(c<0) {
            /* write the unmatched surrogate */
            uint8_t *अष्टकानि=p_तर्काः->converter->गन्तव्यम्_UBytes;
            p_तर्काः->converter->गन्तव्यम्_ULength=2;
            *अष्टकानि=*(s-2);
            अष्टकानि[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_प्राप्तम्;
        }
    }

    p_तर्काः->source=(ध्रुव वर्ण *)s;
    प्रत्यागम c;
} 

स्थिर शून्य  U_CALLCONV
_UTF16BEReset(UConverter *cnv, UConverterResetChoice choice) {
    यदि(choice<=UCNV_RESET_गन्तव्यम्_UNICODE) {
        /* reset toUnicode state */
        यदि(UCNV_GET_संस्करणम्(cnv)==0) {
            cnv->बहुलकम्=8; /* no BOM handling */
        } अन्यथा {
            cnv->बहुलकम्=0; /* Java-specific "UnicodeBig" requires BE BOM or no BOM */
        }
    }
    यदि(choice!=UCNV_RESET_गन्तव्यम्_UNICODE && UCNV_GET_संस्करणम्(cnv)==1) {
        /* reset fromUnicode for "UnicodeBig": prepare to output the UTF-16BE BOM */
        cnv->from_Unicode_स्थितिः=UCNV_NEED_गन्तव्यम्_WRITE_BOM;
    }
}

स्थिर शून्य  U_CALLCONV
_UTF16BEOpen(UConverter *cnv,
             UConverter_आरोपय_तर्काः *p_तर्काः,
             UError_संहिता *p_Error_संहिता) {
    (शून्य)p_तर्काः;
    यदि(UCNV_GET_संस्करणम्(cnv)<=1) {
        _UTF16BEReset(cnv, UCNV_RESET_BOTH);
    } अन्यथा {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
    }
}

स्थिर ध्रुव वर्ण *  U_CALLCONV
_UTF16BEGetName(ध्रुव UConverter *cnv) {
    यदि(UCNV_GET_संस्करणम्(cnv)==0) {
        प्रत्यागम "UTF-16BE";
    } अन्यथा {
        प्रत्यागम "UTF-16BE,version=1";
    }
}
U_CDECL_अन्तः

स्थिर ध्रुव UConverterImpl _UTF16BEImpl={
    UCNV_UTF16_BigEndian,

    शून्यसूचकः,
    शून्यसूचकः,

    _UTF16BEOpen,
    शून्यसूचकः,
    _UTF16BEReset,

    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEToUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_BEGet_अग्र_UChar,

    शून्यसूचकः,
    _UTF16BEGetName,
    शून्यसूचकः,
    शून्यसूचकः,
    ucnv_get_Non_Surrogate_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};

स्थिर ध्रुव UConverter_Static_दत्त UTF16_BEStatic_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "UTF-16BE",
    1200, UCNV_IBM, UCNV_UTF16_BigEndian, 2, 2,
    { 0xff, 0xfd, 0, 0 },2,असत्य,असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ध्रुव UConverter_Shared_दत्त _UTF16BEData=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&UTF16_BEStatic_दत्त, &_UTF16BEImpl);

/* UTF-16LE ----------------------------------------------------------------- */
U_CDECL_BEGIN
स्थिर शून्य  U_CALLCONV
_UTF16LEFromUnicodeWithOffsets(UConverter_From_Unicode_तर्काः *p_तर्काः,
                               UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव char16_t *source;
    वर्ण *target;
    int32_t *offsets;

    uint32_t target_क्षमता, length, source_सूचकांक;
    char16_t c, trail;
    वर्ण अतिप्रवाहः[4];

    source=p_तर्काः->source;
    length=(int32_t)(p_तर्काः->source_सीमा-source);
    यदि(length<=0) {
        /* no input, nothing to do */
        प्रत्यागम;
    }

    cnv=p_तर्काः->converter;

    /* write the BOM if necessary */
    यदि(cnv->from_Unicode_स्थितिः==UCNV_NEED_गन्तव्यम्_WRITE_BOM) {
        स्थिर ध्रुव वर्ण bom[]={ (वर्ण)0xffu, (वर्ण)0xfeu };
        ucnv_from_UWrite_अष्टकानि(cnv,
                             bom, 2,
                             &p_तर्काः->target, p_तर्काः->target_सीमा,
                             &p_तर्काः->offsets, -1,
                             p_Error_संहिता);
        cnv->from_Unicode_स्थितिः=0;
    }

    target=p_तर्काः->target;
    यदि(target >= p_तर्काः->target_सीमा) {
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम;
    }

    target_क्षमता=(uint32_t)(p_तर्काः->target_सीमा-p_तर्काः->target);
    offsets=p_तर्काः->offsets;
    source_सूचकांक=0;

    /* c!=0 indicates in several places outside the main loops that a surrogate was found */

    यदि((c=(char16_t)cnv->fromUChar32)!=0 && U16_IS_TRAIL(trail=*source) && target_क्षमता>=4) {
        /* the last buffer ended with a lead surrogate, output the surrogate pair */
        ++source;
        --length;
        target[0]=(uint8_t)c;
        target[1]=(uint8_t)(c>>8);
        target[2]=(uint8_t)trail;
        target[3]=(uint8_t)(trail>>8);
        target+=4;
        target_क्षमता-=4;
        यदि(offsets!=शून्यसूचकः) {
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
            *offsets++=-1;
        }
        source_सूचकांक=1;
        cnv->fromUChar32=c=0;
    }

    यदि(c==0) {
        /* copy an even number of bytes for complete UChars */
        uint32_t count=2*length;
        यदि(count>target_क्षमता) {
            count=target_क्षमता&~1;
        }
        /* count is even */
        target_क्षमता-=count;
        count>>=1;
        length-=count;

        यदि(offsets==शून्यसूचकः) {
            यावत्(count>0) {
                c=*source++;
                यदि(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target+=2;
                } अन्यथा यदि(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
                    ++source;
                    --count;
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target[2]=(uint8_t)trail;
                    target[3]=(uint8_t)(trail>>8);
                    target+=4;
                } अन्यथा {
                    भङ्ग;
                }
                --count;
            }
        } अन्यथा {
            यावत्(count>0) {
                c=*source++;
                यदि(U16_IS_SINGLE(c)) {
                    target[0]=(uint8_t)c;
                    target[1]=(uint8_t)(c>>8);
                    target+=2;
                    *offsets++=source_सूचकांक;
                    *offsets++=source_सूचकांक++;
                } अन्यथा यदि(U16_IS_SURROGATE_LEAD(c) && count>=2 && U16_IS_TRAIL(trail=*source)) {
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
                    भङ्ग;
                }
                --count;
            }
        }

        यदि(count==0) {
            /* done with the loop for complete UChars */
            यदि(length>0 && target_क्षमता>0) {
                /*
                 * there is more input and some target capacity -
                 * it must be targetCapacity==1 because otherwise
                 * the above would have copied more;
                 * prepare for overflow output
                 */
                यदि(U16_IS_SINGLE(c=*source++)) {
                    अतिप्रवाहः[0]=(वर्ण)c;
                    अतिप्रवाहः[1]=(वर्ण)(c>>8);
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
            target_क्षमता+=2*count;
        }
    } अन्यथा {
        length=0; /* from here on, length counts the bytes in overflow[] */
    }
    
    यदि(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        length=0;
        यदि(U16_IS_SURROGATE_LEAD(c)) {
            यदि(source<p_तर्काः->source_सीमा) {
                यदि(U16_IS_TRAIL(trail=*source)) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    ++source;
                    अतिप्रवाहः[0]=(वर्ण)c;
                    अतिप्रवाहः[1]=(वर्ण)(c>>8);
                    अतिप्रवाहः[2]=(वर्ण)trail;
                    अतिप्रवाहः[3]=(वर्ण)(trail>>8);
                    length=4; /* 4 bytes to output */
                    c=0;
                } अन्यथा {
                    /* unmatched lead surrogate */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                }
            } अन्यथा {
                /* see if the trail surrogate is in the next buffer */
            }
        } अन्यथा {
            /* unmatched trail surrogate */
            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
        }
        cnv->fromUChar32=c;
    }

    यदि(length>0) {
        /* output length bytes with overflow (length>targetCapacity>0) */
        ucnv_from_UWrite_अष्टकानि(cnv,
                             अतिप्रवाहः, length,
                             &target, p_तर्काः->target_सीमा,
                             &offsets, source_सूचकांक,
                             p_Error_संहिता);
        target_क्षमता = static_cast<uint32_t>(p_तर्काः->target_सीमा - target);
    }

    यदि(U_SUCCESS(*p_Error_संहिता) && source<p_तर्काः->source_सीमा && target_क्षमता==0) {
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
    }

    /* write back the updated pointers */
    p_तर्काः->source=source;
    p_तर्काः->target=target;
    p_तर्काः->offsets=offsets;
}

स्थिर शून्य  U_CALLCONV
_UTF16LEToUnicodeWithOffsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                             UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव uint8_t *source;
    char16_t *target;
    int32_t *offsets;

    uint32_t target_क्षमता, length, count, source_सूचकांक;
    char16_t c, trail;

    यदि(p_तर्काः->converter->बहुलकम्<8) {
        UTF16_गन्तव्यम्_Unicode_With_Offsets(p_तर्काः, p_Error_संहिता);
        प्रत्यागम;
    }

    cnv=p_तर्काः->converter;
    source=(ध्रुव uint8_t *)p_तर्काः->source;
    length=(int32_t)((ध्रुव uint8_t *)p_तर्काः->source_सीमा-source);
    यदि(length<=0 && cnv->गन्तव्यम्_Unicode_स्थितिः==0) {
        /* no input, nothing to do */
        प्रत्यागम;
    }

    target=p_तर्काः->target;
    यदि(target >= p_तर्काः->target_सीमा) {
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम;
    }

    target_क्षमता=(uint32_t)(p_तर्काः->target_सीमा-p_तर्काः->target);
    offsets=p_तर्काः->offsets;
    source_सूचकांक=0;
    c=0;

    /* complete a partial char16_t or pair from the last call */
    यदि(cnv->गन्तव्यम्_Unicode_स्थितिः!=0) {
        /*
         * special case: single byte from a previous buffer,
         * where the byte turned out not to belong to a trail surrogate
         * and the preceding, unmatched lead surrogate was put into toUBytes[]
         * for error handling
         */
        cnv->गन्तव्यम्_UBytes[0]=(uint8_t)cnv->गन्तव्यम्_Unicode_स्थितिः;
        cnv->गन्तव्यम्_ULength=1;
        cnv->गन्तव्यम्_Unicode_स्थितिः=0;
    }
    यदि((count=cnv->गन्तव्यम्_ULength)!=0) {
        uint8_t *p=cnv->गन्तव्यम्_UBytes;
        कुरु {
            p[count++]=*source++;
            ++source_सूचकांक;
            --length;
            यदि(count==2) {
                c=((char16_t)p[1]<<8)|p[0];
                यदि(U16_IS_SINGLE(c)) {
                    /* output the BMP code point */
                    *target++=c;
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=-1;
                    }
                    --target_क्षमता;
                    count=0;
                    c=0;
                    भङ्ग;
                } अन्यथा यदि(U16_IS_SURROGATE_LEAD(c)) {
                    /* continue collecting bytes for the trail surrogate */
                    c=0; /* avoid unnecessary surrogate handling below */
                } अन्यथा {
                    /* fall through to error handling for an unmatched trail surrogate */
                    भङ्ग;
                }
            } अन्यथा यदि(count==4) {
                c=((char16_t)p[1]<<8)|p[0];
                trail=((char16_t)p[3]<<8)|p[2];
                यदि(U16_IS_TRAIL(trail)) {
                    /* output the surrogate pair */
                    *target++=c;
                    यदि(target_क्षमता>=2) {
                        *target++=trail;
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=-1;
                            *offsets++=-1;
                        }
                        target_क्षमता-=2;
                    } अन्यथा /* targetCapacity==1 */ {
                        target_क्षमता=0;
                        cnv->UChar_Error_बफर[0]=trail;
                        cnv->UChar_Error_बफर_दीर्घता=1;
                        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                    }
                    count=0;
                    c=0;
                    भङ्ग;
                } अन्यथा {
                    /* unmatched lead surrogate, handle here for consistent toUBytes[] */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;

                    /* back out reading the code unit after it */
                    यदि(((ध्रुव uint8_t *)p_तर्काः->source-source)>=2) {
                        source-=2;
                    } अन्यथा {
                        /*
                         * if the trail unit's first byte was in a previous buffer, then
                         * we need to put it into a special place because toUBytes[] will be
                         * used for the lead unit's bytes
                         */
                        cnv->गन्तव्यम्_Unicode_स्थितिः=0x100|p[2];
                        --source;
                    }
                    cnv->गन्तव्यम्_ULength=2;

                    /* write back the updated pointers */
                    p_तर्काः->source=(ध्रुव वर्ण *)source;
                    p_तर्काः->target=target;
                    p_तर्काः->offsets=offsets;
                    प्रत्यागम;
                }
            }
        } यावत्(length>0);
        cnv->गन्तव्यम्_ULength=(int8_t)count;
    }

    /* copy an even number of bytes for complete UChars */
    count=2*target_क्षमता;
    यदि(count>length) {
        count=length&~1;
    }
    यदि(c==0 && count>0) {
        length-=count;
        count>>=1;
        target_क्षमता-=count;
        यदि(offsets==शून्यसूचकः) {
            कुरु {
                c=((char16_t)source[1]<<8)|source[0];
                source+=2;
                यदि(U16_IS_SINGLE(c)) {
                    *target++=c;
                } अन्यथा यदि(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
                          U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])
                ) {
                    source+=2;
                    --count;
                    *target++=c;
                    *target++=trail;
                } अन्यथा {
                    भङ्ग;
                }
            } यावत्(--count>0);
        } अन्यथा {
            कुरु {
                c=((char16_t)source[1]<<8)|source[0];
                source+=2;
                यदि(U16_IS_SINGLE(c)) {
                    *target++=c;
                    *offsets++=source_सूचकांक;
                    source_सूचकांक+=2;
                } अन्यथा यदि(U16_IS_SURROGATE_LEAD(c) && count>=2 &&
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
                    भङ्ग;
                }
            } यावत्(--count>0);
        }

        यदि(count==0) {
            /* done with the loop for complete UChars */
            c=0;
        } अन्यथा {
            /* keep c for surrogate handling, trail will be set there */
            length+=2*(count-1); /* one more byte pair was consumed than count decremented */
            target_क्षमता+=count;
        }
    }

    यदि(c!=0) {
        /*
         * c is a surrogate, and
         * - source or target too short
         * - or the surrogate is unmatched
         */
        cnv->गन्तव्यम्_UBytes[0]=(uint8_t)c;
        cnv->गन्तव्यम्_UBytes[1]=(uint8_t)(c>>8);
        cnv->गन्तव्यम्_ULength=2;

        यदि(U16_IS_SURROGATE_LEAD(c)) {
            यदि(length>=2) {
                यदि(U16_IS_TRAIL(trail=((char16_t)source[1]<<8)|source[0])) {
                    /* output the surrogate pair, will overflow (see conditions comment above) */
                    source+=2;
                    length-=2;
                    *target++=c;
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                    cnv->UChar_Error_बफर[0]=trail;
                    cnv->UChar_Error_बफर_दीर्घता=1;
                    cnv->गन्तव्यम्_ULength=0;
                    *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                } अन्यथा {
                    /* unmatched lead surrogate */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                }
            } अन्यथा {
                /* see if the trail surrogate is in the next buffer */
            }
        } अन्यथा {
            /* unmatched trail surrogate */
            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
        }
    }

    यदि(U_SUCCESS(*p_Error_संहिता)) {
        /* check for a remaining source byte */
        यदि(length>0) {
            यदि(target_क्षमता==0) {
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            } अन्यथा {
                /* it must be length==1 because otherwise the above would have copied more */
                cnv->गन्तव्यम्_UBytes[cnv->गन्तव्यम्_ULength++]=*source++;
            }
        }
    }

    /* write back the updated pointers */
    p_तर्काः->source=(ध्रुव वर्ण *)source;
    p_तर्काः->target=target;
    p_तर्काः->offsets=offsets;
}

स्थिर UChar32  U_CALLCONV
UTF16_LEGet_अग्र_UChar(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः, UError_संहिता *err) {
    ध्रुव uint8_t *s, *source_सीमा;
    UChar32 c;

    यदि(p_तर्काः->converter->बहुलकम्<8) {
        प्रत्यागम UCNV_GET_अग्र_UCHAR_USE_गन्तव्यम्_U;
    }

    s=(ध्रुव uint8_t *)p_तर्काः->source;
    source_सीमा=(ध्रुव uint8_t *)p_तर्काः->source_सीमा;

    यदि(s>=source_सीमा) {
        /* no input */
        *err=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0xffff;
    }

    यदि(s+2>source_सीमा) {
        /* only one byte: truncated char16_t */
        p_तर्काः->converter->गन्तव्यम्_UBytes[0]=*s++;
        p_तर्काः->converter->गन्तव्यम्_ULength=1;
        p_तर्काः->source=(ध्रुव वर्ण *)s;
        *err = U_TRUNCATED_CHAR_प्राप्तम्;
        प्रत्यागम 0xffff;
    }

    /* get one char16_t */
    c=((UChar32)s[1]<<8)|*s;
    s+=2;

    /* check for a surrogate pair */
    यदि(U_IS_SURROGATE(c)) {
        यदि(U16_IS_SURROGATE_LEAD(c)) {
            यदि(s+2<=source_सीमा) {
                char16_t trail;

                /* get a second char16_t and see if it is a trail surrogate */
                trail=((char16_t)s[1]<<8)|*s;
                यदि(U16_IS_TRAIL(trail)) {
                    c=U16_GET_SUPPLEMENTARY(c, trail);
                    s+=2;
                } अन्यथा {
                    /* unmatched lead surrogate */
                    c=-2;
                }
            } अन्यथा {
                /* too few (2 or 3) bytes for a surrogate pair: truncated code point */
                uint8_t *अष्टकानि=p_तर्काः->converter->गन्तव्यम्_UBytes;
                s-=2;
                p_तर्काः->converter->गन्तव्यम्_ULength=(int8_t)(source_सीमा-s);
                कुरु {
                    *अष्टकानि++=*s++;
                } यावत्(s<source_सीमा);

                c=0xffff;
                *err=U_TRUNCATED_CHAR_प्राप्तम्;
            }
        } अन्यथा {
            /* unmatched trail surrogate */
            c=-2;
        }

        यदि(c<0) {
            /* write the unmatched surrogate */
            uint8_t *अष्टकानि=p_तर्काः->converter->गन्तव्यम्_UBytes;
            p_तर्काः->converter->गन्तव्यम्_ULength=2;
            *अष्टकानि=*(s-2);
            अष्टकानि[1]=*(s-1);

            c=0xffff;
            *err=U_ILLEGAL_CHAR_प्राप्तम्;
        }
    }

    p_तर्काः->source=(ध्रुव वर्ण *)s;
    प्रत्यागम c;
} 

स्थिर शून्य  U_CALLCONV
_UTF16LEReset(UConverter *cnv, UConverterResetChoice choice) {
    यदि(choice<=UCNV_RESET_गन्तव्यम्_UNICODE) {
        /* reset toUnicode state */
        यदि(UCNV_GET_संस्करणम्(cnv)==0) {
            cnv->बहुलकम्=8; /* no BOM handling */
        } अन्यथा {
            cnv->बहुलकम्=0; /* Java-specific "UnicodeLittle" requires LE BOM or no BOM */
        }
    }
    यदि(choice!=UCNV_RESET_गन्तव्यम्_UNICODE && UCNV_GET_संस्करणम्(cnv)==1) {
        /* reset fromUnicode for "UnicodeLittle": prepare to output the UTF-16LE BOM */
        cnv->from_Unicode_स्थितिः=UCNV_NEED_गन्तव्यम्_WRITE_BOM;
    }
}

स्थिर शून्य  U_CALLCONV
_UTF16LEOpen(UConverter *cnv,
             UConverter_आरोपय_तर्काः *p_तर्काः,
             UError_संहिता *p_Error_संहिता) {
    (शून्य)p_तर्काः;
    यदि(UCNV_GET_संस्करणम्(cnv)<=1) {
        _UTF16LEReset(cnv, UCNV_RESET_BOTH);
    } अन्यथा {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
    }
}

स्थिर ध्रुव वर्ण *  U_CALLCONV
_UTF16LEGetName(ध्रुव UConverter *cnv) {
    यदि(UCNV_GET_संस्करणम्(cnv)==0) {
        प्रत्यागम "UTF-16LE";
    } अन्यथा {
        प्रत्यागम "UTF-16LE,version=1";
    }
}
U_CDECL_अन्तः

स्थिर ध्रुव UConverterImpl _UTF16LEImpl={
    UCNV_UTF16_LittleEndian,

    शून्यसूचकः,
    शून्यसूचकः,

    _UTF16LEOpen,
    शून्यसूचकः,
    _UTF16LEReset,

    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEToUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    _UTF16LEFromUnicodeWithOffsets,
    UTF16_LEGet_अग्र_UChar,

    शून्यसूचकः,
    _UTF16LEGetName,
    शून्यसूचकः,
    शून्यसूचकः,
    ucnv_get_Non_Surrogate_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};


स्थिर ध्रुव UConverter_Static_दत्त UTF16_LEStatic_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "UTF-16LE",
    1202, UCNV_IBM, UCNV_UTF16_LittleEndian, 2, 2,
    { 0xfd, 0xff, 0, 0 },2,असत्य,असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ध्रुव UConverter_Shared_दत्त _UTF16LEData=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&UTF16_LEStatic_दत्त, &_UTF16LEImpl);

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
स्थिर शून्य  U_CALLCONV
_UTF16Reset(UConverter *cnv, UConverterResetChoice choice) {
    यदि(choice<=UCNV_RESET_गन्तव्यम्_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->बहुलकम्=0;
    }
    यदि(choice!=UCNV_RESET_गन्तव्यम्_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-16PE BOM */
        cnv->from_Unicode_स्थितिः=UCNV_NEED_गन्तव्यम्_WRITE_BOM;
    }
}
U_CDECL_अन्तः
बाह्य ध्रुव UConverter_Shared_दत्त UTF16v2_दत्त;
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
_UTF16Open(UConverter *cnv,
           UConverter_आरोपय_तर्काः *p_तर्काः,
           UError_संहिता *p_Error_संहिता) {
    यदि(UCNV_GET_संस्करणम्(cnv)<=2) {
        यदि(UCNV_GET_संस्करणम्(cnv)==2 && !p_तर्काः->onlyTestIsLoadable) {
            /*
             * Switch implementation, and switch the staticData that's different
             * and was copied into the UConverter.
             * (See ucnv_createConverterFromSharedData() in ucnv_bld.c.)
             * UTF-16,version=2 fromUnicode() always writes a big-endian byte stream.
             */
            cnv->shared_दत्त=(UConverter_Shared_दत्त*)&UTF16v2_दत्त;
            uprv_memcpy(cnv->subChars, UTF16v2_दत्त.static_दत्त->subChar, UCNV_MAX_SUBCHAR_LEN);
        }
        _UTF16Reset(cnv, UCNV_RESET_BOTH);
    } अन्यथा {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
    }
}

स्थिर ध्रुव वर्ण *  U_CALLCONV
_UTF16GetName(ध्रुव UConverter *cnv) {
    यदि(UCNV_GET_संस्करणम्(cnv)==0) {
        प्रत्यागम "UTF-16";
    } अन्यथा यदि(UCNV_GET_संस्करणम्(cnv)==1) {
        प्रत्यागम "UTF-16,version=1";
    } अन्यथा {
        प्रत्यागम "UTF-16,version=2";
    }
}
U_CDECL_अन्तः
बाह्य ध्रुव UConverter_Shared_दत्त UTF16_दत्त;

स्थिर अन्तःस्थ तार्किक IS_UTF16BE(ध्रुव UConverter *cnv) {
    प्रत्यागम ((cnv)->shared_दत्त == &_UTF16BEData);
}

स्थिर अन्तःस्थ तार्किक IS_UTF16LE(ध्रुव UConverter *cnv) {
    प्रत्यागम ((cnv)->shared_दत्त == &_UTF16LEData);
}

स्थिर अन्तःस्थ तार्किक IS_UTF16(ध्रुव UConverter *cnv) {
    प्रत्यागम ((cnv)->shared_दत्त==&UTF16_दत्त) || ((cnv)->shared_दत्त == &UTF16v2_दत्त);
}

U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
UTF16_गन्तव्यम्_Unicode_With_Offsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                           UError_संहिता *p_Error_संहिता) {
    UConverter *cnv=p_तर्काः->converter;
    ध्रुव वर्ण *source=p_तर्काः->source;
    ध्रुव वर्ण *source_सीमा=p_तर्काः->source_सीमा;
    int32_t *offsets=p_तर्काः->offsets;

    int32_t state, विस्थापनम्_Delta;
    uint8_t b;

    state=cnv->बहुलकम्;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    विस्थापनम्_Delta=0;

    यावत्(source<source_सीमा && U_SUCCESS(*p_Error_संहिता)) {
        विकल्प(state) {
        पक्ष 0:
            cnv->गन्तव्यम्_UBytes[0]=(uint8_t)*source++;
            cnv->गन्तव्यम्_ULength=1;
            state=1;
            भङ्ग;
        पक्ष 1:
            /*
             * Only inside this switch case can the state variable
             * temporarily take two additional values:
             * 6: BOM error, continue with BE
             * 7: BOM error, continue with LE
             */
            b=*source;
            यदि(cnv->गन्तव्यम्_UBytes[0]==0xfe && b==0xff) {
                यदि(IS_UTF16LE(cnv)) {
                    state=7; /* illegal reverse BOM for Java "UnicodeLittle" */
                } अन्यथा {
                    state=8; /* detect UTF-16BE */
                }
            } अन्यथा यदि(cnv->गन्तव्यम्_UBytes[0]==0xff && b==0xfe) {
                यदि(IS_UTF16BE(cnv)) {
                    state=6; /* illegal reverse BOM for Java "UnicodeBig" */
                } अन्यथा {
                    state=9; /* detect UTF-16LE */
                }
            } अन्यथा यदि((IS_UTF16(cnv) && UCNV_GET_संस्करणम्(cnv)==1)) {
                state=6; /* illegal missing BOM for Java "Unicode" */
            }
            यदि(state>=8) {
                /* BOM detected, consume it */
                ++source;
                cnv->गन्तव्यम्_ULength=0;
                विस्थापनम्_Delta=(int32_t)(source-p_तर्काः->source);
            } अन्यथा यदि(state<6) {
                /* ok: no BOM, and not a reverse BOM */
                यदि(source!=p_तर्काः->source) {
                    /* reset the source for a correct first offset */
                    source=p_तर्काः->source;
                    cnv->गन्तव्यम्_ULength=0;
                }
                यदि(IS_UTF16LE(cnv)) {
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
                cnv->गन्तव्यम्_UBytes[1]=b;
                cnv->गन्तव्यम्_ULength=2;
                p_तर्काः->source=source+1;
                /* continue with conversion if the callback resets the error */
                /*
                 * Make Java "Unicode" default to BE like standard UTF-16.
                 * Make Java "UnicodeBig" and "UnicodeLittle" default
                 * to their normal endiannesses.
                 */
                cnv->बहुलकम्=state+2;
                *p_Error_संहिता=U_ILLEGAL_ESCAPE_SEQUENCE;
                प्रत्यागम;
            }
            /* convert the rest of the stream */
            cnv->बहुलकम्=state;
            अनुवर्त;
        पक्ष 8:
            /* call UTF-16BE */
            p_तर्काः->source=source;
            _UTF16BEToUnicodeWithOffsets(p_तर्काः, p_Error_संहिता);
            source=p_तर्काः->source;
            भङ्ग;
        पक्ष 9:
            /* call UTF-16LE */
            p_तर्काः->source=source;
            _UTF16LEToUnicodeWithOffsets(p_तर्काः, p_Error_संहिता);
            source=p_तर्काः->source;
            भङ्ग;
        स्वभाव:
            भङ्ग; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    यदि(offsets!=शून्यसूचकः && विस्थापनम्_Delta!=0) {
        int32_t *offsets_सीमा=p_तर्काः->offsets;
        यावत्(offsets<offsets_सीमा) {
            *offsets++ += विस्थापनम्_Delta;
        }
    }

    p_तर्काः->source=source;

    यदि(source==source_सीमा && p_तर्काः->flush) {
        /* handle truncated input */
        विकल्प(state) {
        पक्ष 0:
            भङ्ग; /* no input at all, nothing to do */
        पक्ष 8:
            _UTF16BEToUnicodeWithOffsets(p_तर्काः, p_Error_संहिता);
            भङ्ग;
        पक्ष 9:
            _UTF16LEToUnicodeWithOffsets(p_तर्काः, p_Error_संहिता);
            भङ्ग;
        स्वभाव:
            /* 0<state<8: framework will report truncation, nothing to do here */
            भङ्ग;
        }
    }

    cnv->बहुलकम्=state;
}

स्थिर UChar32 U_CALLCONV
UTF16_Get_अग्र_UChar(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                   UError_संहिता *p_Error_संहिता) {
    विकल्प(p_तर्काः->converter->बहुलकम्) {
    पक्ष 8:
        प्रत्यागम UTF16_BEGet_अग्र_UChar(p_तर्काः, p_Error_संहिता);
    पक्ष 9:
        प्रत्यागम UTF16_LEGet_अग्र_UChar(p_तर्काः, p_Error_संहिता);
    स्वभाव:
        प्रत्यागम UCNV_GET_अग्र_UCHAR_USE_गन्तव्यम्_U;
    }
}
U_CDECL_अन्तः

स्थिर ध्रुव UConverterImpl _UTF16Impl = {
    UCNV_UTF16,

    शून्यसूचकः,
    शून्यसूचकः,

    _UTF16Open,
    शून्यसूचकः,
    _UTF16Reset,

    UTF16_गन्तव्यम्_Unicode_With_Offsets,
    UTF16_गन्तव्यम्_Unicode_With_Offsets,
    _UTF16PEFromUnicodeWithOffsets,
    _UTF16PEFromUnicodeWithOffsets,
    UTF16_Get_अग्र_UChar,

    शून्यसूचकः, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    शून्यसूचकः,
    शून्यसूचकः,
    ucnv_get_Non_Surrogate_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};

स्थिर ध्रुव UConverter_Static_दत्त UTF16_Static_दत्त = {
    परिमाण(UConverter_Static_दत्त),
    "UTF-16",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
#यदि U_IS_BIG_ENDIAN
    { 0xff, 0xfd, 0, 0 }, 2,
#अन्यथा
    { 0xfd, 0xff, 0, 0 }, 2,
#यदि_अन्तः
    असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ध्रुव UConverter_Shared_दत्त UTF16_दत्त =
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&UTF16_Static_दत्त, &_UTF16Impl);

स्थिर ध्रुव UConverterImpl _UTF16v2Impl = {
    UCNV_UTF16,

    शून्यसूचकः,
    शून्यसूचकः,

    _UTF16Open,
    शून्यसूचकः,
    _UTF16Reset,

    UTF16_गन्तव्यम्_Unicode_With_Offsets,
    UTF16_गन्तव्यम्_Unicode_With_Offsets,
    _UTF16BEFromUnicodeWithOffsets,
    _UTF16BEFromUnicodeWithOffsets,
    UTF16_Get_अग्र_UChar,

    शून्यसूचकः, /* ### TODO implement getStarters for all Unicode encodings?! */
    _UTF16GetName,
    शून्यसूचकः,
    शून्यसूचकः,
    ucnv_get_Non_Surrogate_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};

स्थिर ध्रुव UConverter_Static_दत्त UTF16v2_Static_दत्त = {
    परिमाण(UConverter_Static_दत्त),
    "UTF-16,version=2",
    1204, /* CCSID for BOM sensitive UTF-16 */
    UCNV_IBM, UCNV_UTF16, 2, 2,
    { 0xff, 0xfd, 0, 0 }, 2,
    असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ध्रुव UConverter_Shared_दत्त UTF16v2_दत्त =
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&UTF16v2_Static_दत्त, &_UTF16v2Impl);

#यदि_अन्तः
