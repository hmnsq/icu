// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*  
**********************************************************************
*   Copyright (C) 2002-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ucnv_u7.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002jul01
*   created by: Markus W. Scherer
*
*   UTF-7 converter implementation. Used to be in ucnv_utf.c.
*/

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "uassert.h"

/* UTF-7 -------------------------------------------------------------------- */

/*
 * UTF-7 is a stateful encoding of Unicode.
 * It is defined in RFC 2152. (http://www.ietf.org/rfc/rfc2152.txt)
 * It was intended for use in Internet email systems, using in its bytewise
 * encoding only a subset of 7-bit US-ASCII.
 * UTF-7 is deprecated in favor of UTF-8/16/32 and SCSU, but still
 * occasionally used.
 *
 * For converting Unicode to UTF-7, the RFC allows to encode some US-ASCII
 * characters directly or in base64. Especially, the characters in set O
 * as defined in the RFC (see below) may be encoded directly but are not
 * allowed in, e.g., email headers.
 * By default, the ICU UTF-7 converter encodes set O directly.
 * By choosing the option "version=1", set O will be escaped instead.
 * For example:
 *     utf7Converter=ucnv_open("UTF-7,version=1");
 *
 * For details about email headers see RFC 2047.
 */

/*
 * Tests for US-ASCII characters belonging to character classes
 * defined in UTF-7.
 *
 * Set D (directly encoded characters) consists of the following
 * characters: the upper and lower case letters A through Z
 * and a through z, the 10 digits 0-9, and the following nine special
 * characters (note that "+" and "=" are omitted):
 *     '(),-./:?
 *
 * Set O (optional direct characters) consists of the following
 * characters (note that "\" and "~" are omitted):
 *     !"#$%&*;<=>@[]^_`{|}
 *
 * According to the rules in RFC 2152, the byte values for the following
 * US-ASCII characters are not used in UTF-7 and are therefore illegal:
 * - all C0 control codes except for CR LF TAB
 * - BACKSLASH
 * - TILDE
 * - DEL
 * - all codes beyond US-ASCII, i.e. all >127
 */
#परिभाषय अन्तर्_समुच्चयः_D(c) \
    ((uint8_t)((c)-97)<26 || (uint8_t)((c)-65)<26 || /* letters */ \
     (uint8_t)((c)-48)<10 ||    /* digits */ \
     (uint8_t)((c)-39)<3 ||     /* '() */ \
     (uint8_t)((c)-44)<4 ||     /* ,-./ */ \
     (c)==58 || (c)==63         /* :? */ \
    )

#परिभाषय अन्तर्_समुच्चयः_O(c) \
    ((uint8_t)((c)-33)<6 ||         /* !"#$%& */ \
     (uint8_t)((c)-59)<4 ||         /* ;<=> */ \
     (uint8_t)((c)-93)<4 ||         /* ]^_` */ \
     (uint8_t)((c)-123)<3 ||        /* {|} */ \
     (c)==42 || (c)==64 || (c)==91  /* *@[ */ \
    )

#परिभाषय isCRLFTAB(c) ((c)==13 || (c)==10 || (c)==9)
#परिभाषय isCRLFSPTAB(c) ((c)==32 || (c)==13 || (c)==10 || (c)==9)

#परिभाषय PLUS  43
#परिभाषय MINUS 45
#परिभाषय BACKSLASH 92
#परिभाषय TILDE 126

/* legal byte values: all US-ASCII graphic characters from space to before tilde, and CR LF TAB */
#परिभाषय isLegalUTF7(c) (((uint8_t)((c)-32)<94 && (c)!=BACKSLASH) || isCRLFTAB(c))

/* encode directly sets D and O and CR LF SP TAB */
स्थिर ध्रुव UBool encode_Directly_अधिकतमम्[128]={
 /* 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0
};

/* encode directly set D and CR LF SP TAB but not set O */
स्थिर ध्रुव UBool encodeDirectlyRestricted[128]={
 /* 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,

    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,

    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0
};

स्थिर ध्रुव uint8_t
गन्तव्यम्_Base64[64]={
    /* A-Z */
    65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
    78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
    /* a-z */
    97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
    /* 0-9 */
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
    /* +/ */
    43, 47
};

स्थिर ध्रुव int8_t
fromBase64[128]={
    /* C0 controls, -1 for legal ones (CR LF TAB), -3 for illegal ones */
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -1, -1, -3, -3, -1, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,

    /* general punctuation with + and / and a special value (-2) for - */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -2, -1, 63,
    /* digits */
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,

    /* A-Z */
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -3, -1, -1, -1,

    /* a-z */
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -3, -3
};

/*
 * converter status values:
 *
 * toUnicodeStatus:
 *     24 inDirectMode (boolean)
 * 23..16 base64Counter (-1..7)
 * 15..0  bits (up to 14 bits incoming base64)
 *
 * fromUnicodeStatus:
 * 31..28 version (0: set O direct  1: set O escaped)
 *     24 inDirectMode (boolean)
 * 23..16 base64Counter (0..2)
 *  7..0  bits (6 bits outgoing base64)
 *
 */

U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
_UTF7Reset(UConverter *cnv, UConverterResetChoice choice) {
    यदि(choice<=UCNV_RESET_गन्तव्यम्_UNICODE) {
        /* reset toUnicode */
        cnv->गन्तव्यम्_Unicode_स्थितिः=0x1000000; /* inDirectMode=true */
        cnv->गन्तव्यम्_ULength=0;
    }
    यदि(choice!=UCNV_RESET_गन्तव्यम्_UNICODE) {
        /* reset fromUnicode */
        cnv->from_Unicode_स्थितिः=(cnv->from_Unicode_स्थितिः&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    }
}

स्थिर शून्य U_CALLCONV
_UTF7Open(UConverter *cnv,
          UConverter_आरोपय_तर्काः *p_तर्काः,
          UError_संहिता *p_Error_संहिता) {
    (शून्य)p_तर्काः;
    यदि(UCNV_GET_संस्करणम्(cnv)<=1) {
        /* TODO(markus): Should just use cnv->options rather than copying the version number. */
        cnv->from_Unicode_स्थितिः=UCNV_GET_संस्करणम्(cnv)<<28;
        _UTF7Reset(cnv, UCNV_RESET_BOTH);
    } अन्यथा {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
    }
}

स्थिर शून्य U_CALLCONV
UTF7_गन्तव्यम्_Unicode_With_Offsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                          UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव uint8_t *source, *source_सीमा;
    char16_t *target;
    ध्रुव char16_t *target_सीमा;
    int32_t *offsets;

    uint8_t *अष्टकानि;
    uint8_t byte_सूचकांक;

    int32_t length, target_क्षमता;

    /* UTF-7 state */
    uint16_t द्वयङ्काः;
    int8_t base64_गणक;
    UBool अन्तर्_प्रत्यक्षम्_बहुलकम्;

    int8_t base64_मानम्;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    uint8_t b;
    /* set up the local pointers */
    cnv=p_तर्काः->converter;

    source=(ध्रुव uint8_t *)p_तर्काः->source;
    source_सीमा=(ध्रुव uint8_t *)p_तर्काः->source_सीमा;
    target=p_तर्काः->target;
    target_सीमा=p_तर्काः->target_सीमा;
    offsets=p_तर्काः->offsets;
    /* get the state machine state */
    {
        uint32_t स्थितिः=cnv->गन्तव्यम्_Unicode_स्थितिः;
        अन्तर्_प्रत्यक्षम्_बहुलकम्=(UBool)((स्थितिः>>24)&1);
        base64_गणक=(int8_t)(स्थितिः>>16);
        द्वयङ्काः=(uint16_t)स्थितिः;
    }
    अष्टकानि=cnv->गन्तव्यम्_UBytes;
    byte_सूचकांक=cnv->गन्तव्यम्_ULength;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक=byte_सूचकांक==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    यदि(अन्तर्_प्रत्यक्षम्_बहुलकम्) {
प्रत्यक्षम्_बहुलकम्:
        /*
         * In Direct Mode, most US-ASCII characters are encoded directly, i.e.,
         * with their US-ASCII byte values.
         * Backslash and Tilde and most control characters are not allowed in UTF-7.
         * A plus sign starts Unicode (or "escape") Mode.
         *
         * In Direct Mode, only the sourceIndex is used.
         */
        byte_सूचकांक=0;
        length=(int32_t)(source_सीमा-source);
        target_क्षमता=(int32_t)(target_सीमा-target);
        यदि(length>target_क्षमता) {
            length=target_क्षमता;
        }
        यावत्(length>0) {
            b=*source++;
            यदि(!isLegalUTF7(b)) {
                /* illegal */
                अष्टकानि[0]=b;
                byte_सूचकांक=1;
                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                भङ्ग;
            } अन्यथा यदि(b!=PLUS) {
                /* write directly encoded character */
                *target++=b;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक++;
                }
            } अन्यथा /* PLUS */ {
                /* switch to Unicode mode */
                अग्र_Source_सूचकांक=++source_सूचकांक;
                अन्तर्_प्रत्यक्षम्_बहुलकम्=असत्य;
                byte_सूचकांक=0;
                द्वयङ्काः=0;
                base64_गणक=-1;
                गच्छ unicode_बहुलकम्;
            }
            --length;
        }
        यदि(source<source_सीमा && target>=target_सीमा) {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        }
    } अन्यथा {
unicode_बहुलकम्:
        /*
         * In Unicode (or "escape") Mode, UTF-16BE is base64-encoded.
         * The base64 sequence ends with any character that is not in the base64 alphabet.
         * A terminating minus sign is consumed.
         *
         * In Unicode Mode, the sourceIndex has the index to the start of the current
         * base64 bytes, while nextSourceIndex is precisely parallel to source,
         * keeping the index to the following byte.
         * Note that in 2 out of 3 cases, UChars overlap within a base64 byte.
         */
        यावत्(source<source_सीमा) {
            यदि(target<target_सीमा) {
                अष्टकानि[byte_सूचकांक++]=b=*source++;
                ++अग्र_Source_सूचकांक;
                base64_मानम् = -3; /* initialize as illegal */
                यदि(b>=126 || (base64_मानम्=fromBase64[b])==-3 || base64_मानम्==-1) {
                    /* either
                     * base64Value==-1 for any legal character except base64 and minus sign, or
                     * base64Value==-3 for illegal characters:
                     * 1. In either case, leave Unicode mode.
                     * 2.1. If we ended with an incomplete char16_t or none after the +, then
                     *      generate an error for the preceding erroneous sequence and deal with
                     *      the current (possibly illegal) character next time through.
                     * 2.2. Else the current char comes after a complete char16_t, which was already
                     *      pushed to the output buf, so:
                     * 2.2.1. If the current char is legal, just save it for processing next time.
                     *        It may be for example, a plus which we need to deal with in direct mode.
                     * 2.2.2. Else if the current char is illegal, we might as well deal with it here.
                     */
                    अन्तर्_प्रत्यक्षम्_बहुलकम्=सत्य;
                    यदि(base64_गणक==-1) {
                        /* illegal: + immediately followed by something other than base64 or minus sign */
                        /* include the plus sign in the reported sequence, but not the subsequent char */
                        --source;
                        अष्टकानि[0]=PLUS;
                        byte_सूचकांक=1;
                        *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                        भङ्ग;
                    } अन्यथा यदि(द्वयङ्काः!=0) {
                        /* bits are illegally left over, a char16_t is incomplete */
                        /* don't include current char (legal or illegal) in error seq */
                        --source;
                        --byte_सूचकांक;
                        *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                        भङ्ग;
                    } अन्यथा {
                        /* previous char16_t was complete */
                        यदि(base64_मानम्==-3) {
                            /* current character is illegal, deal with it here */
                            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                            भङ्ग;
                        } अन्यथा {
                            /* un-read the current character in case it is a plus sign */
                            --source;
                            source_सूचकांक=अग्र_Source_सूचकांक-1;
                            गच्छ प्रत्यक्षम्_बहुलकम्;
                        }
                    }
                } अन्यथा यदि(base64_मानम्>=0) {
                    /* collect base64 bytes into UChars */
                    विकल्प(base64_गणक) {
                    पक्ष -1: /* -1 is immediately after the + */
                    पक्ष 0:
                        द्वयङ्काः=base64_मानम्;
                        base64_गणक=1;
                        भङ्ग;
                    पक्ष 1:
                    पक्ष 3:
                    पक्ष 4:
                    पक्ष 6:
                        द्वयङ्काः=(uint16_t)((द्वयङ्काः<<6)|base64_मानम्);
                        ++base64_गणक;
                        भङ्ग;
                    पक्ष 2:
                        *target++=(char16_t)((द्वयङ्काः<<4)|(base64_मानम्>>2));
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=source_सूचकांक;
                            source_सूचकांक=अग्र_Source_सूचकांक-1;
                        }
                        अष्टकानि[0]=b; /* keep this byte in case an error occurs */
                        byte_सूचकांक=1;
                        द्वयङ्काः=(uint16_t)(base64_मानम्&3);
                        base64_गणक=3;
                        भङ्ग;
                    पक्ष 5:
                        *target++=(char16_t)((द्वयङ्काः<<2)|(base64_मानम्>>4));
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=source_सूचकांक;
                            source_सूचकांक=अग्र_Source_सूचकांक-1;
                        }
                        अष्टकानि[0]=b; /* keep this byte in case an error occurs */
                        byte_सूचकांक=1;
                        द्वयङ्काः=(uint16_t)(base64_मानम्&15);
                        base64_गणक=6;
                        भङ्ग;
                    पक्ष 7:
                        *target++=(char16_t)((द्वयङ्काः<<6)|base64_मानम्);
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=source_सूचकांक;
                            source_सूचकांक=अग्र_Source_सूचकांक;
                        }
                        byte_सूचकांक=0;
                        द्वयङ्काः=0;
                        base64_गणक=0;
                        भङ्ग;
                    स्वभाव:
                        /* will never occur */
                        भङ्ग;
                    }
                } अन्यथा /*base64Value==-2*/ {
                    /* minus sign terminates the base64 sequence */
                    अन्तर्_प्रत्यक्षम्_बहुलकम्=सत्य;
                    यदि(base64_गणक==-1) {
                        /* +- i.e. a minus immediately following a plus */
                        *target++=PLUS;
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=source_सूचकांक-1;
                        }
                    } अन्यथा {
                        /* absorb the minus and leave the Unicode Mode */
                        यदि(द्वयङ्काः!=0) {
                            /* bits are illegally left over, a char16_t is incomplete */
                            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                            भङ्ग;
                        }
                    }
                    source_सूचकांक=अग्र_Source_सूचकांक;
                    गच्छ प्रत्यक्षम्_बहुलकम्;
                }
            } अन्यथा {
                /* target is full */
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
        }
    }

    यदि(U_SUCCESS(*p_Error_संहिता) && p_तर्काः->flush && source==source_सीमा && द्वयङ्काः==0) {
        /*
         * if we are in Unicode mode, then the byteIndex might not be 0,
         * but that is ok if bits==0
         * -> we set byteIndex=0 at the end of the stream to avoid a truncated error
         * (not true for IMAP-mailbox-name where we must end in direct mode)
         */
        byte_सूचकांक=0;
    }

    /* set the converter state back into UConverter */
    cnv->गन्तव्यम्_Unicode_स्थितिः=((uint32_t)अन्तर्_प्रत्यक्षम्_बहुलकम्<<24)|((uint32_t)((uint8_t)base64_गणक)<<16)|(uint32_t)द्वयङ्काः;
    cnv->गन्तव्यम्_ULength=byte_सूचकांक;

    /* write back the updated pointers */
    p_तर्काः->source=(ध्रुव वर्ण *)source;
    p_तर्काः->target=target;
    p_तर्काः->offsets=offsets;
}

स्थिर शून्य U_CALLCONV
_UTF7FromUnicodeWithOffsets(UConverter_From_Unicode_तर्काः *p_तर्काः,
                            UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव char16_t *source, *source_सीमा;
    uint8_t *target, *target_सीमा;
    int32_t *offsets;

    int32_t length, target_क्षमता, source_सूचकांक;
    char16_t c;

    /* UTF-7 state */
    ध्रुव UBool *encodeDirectly;
    uint8_t द्वयङ्काः;
    int8_t base64_गणक;
    UBool अन्तर्_प्रत्यक्षम्_बहुलकम्;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;

    /* set up the local pointers */
    source=p_तर्काः->source;
    source_सीमा=p_तर्काः->source_सीमा;
    target=(uint8_t *)p_तर्काः->target;
    target_सीमा=(uint8_t *)p_तर्काः->target_सीमा;
    offsets=p_तर्काः->offsets;

    /* get the state machine state */
    {
        uint32_t स्थितिः=cnv->from_Unicode_स्थितिः;
        encodeDirectly= स्थितिः<0x10000000 ? encode_Directly_अधिकतमम् : encodeDirectlyRestricted;
        अन्तर्_प्रत्यक्षम्_बहुलकम्=(UBool)((स्थितिः>>24)&1);
        base64_गणक=(int8_t)(स्थितिः>>16);
        द्वयङ्काः=(uint8_t)स्थितिः;
        U_ASSERT(द्वयङ्काः<=UPRV_LENGTHOF(गन्तव्यम्_Base64));
    }

    /* UTF-7 always encodes UTF-16 code units, therefore we need only a simple sourceIndex */
    source_सूचकांक=0;

    यदि(अन्तर्_प्रत्यक्षम्_बहुलकम्) {
प्रत्यक्षम्_बहुलकम्:
        length=(int32_t)(source_सीमा-source);
        target_क्षमता=(int32_t)(target_सीमा-target);
        यदि(length>target_क्षमता) {
            length=target_क्षमता;
        }
        यावत्(length>0) {
            c=*source++;
            /* currently always encode CR LF SP TAB directly */
            यदि(c<=127 && encodeDirectly[c]) {
                /* encode directly */
                *target++=(uint8_t)c;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक++;
                }
            } अन्यथा यदि(c==PLUS) {
                /* output +- for + */
                *target++=PLUS;
                यदि(target<target_सीमा) {
                    *target++=MINUS;
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                        *offsets++=source_सूचकांक++;
                    }
                    /* realign length and targetCapacity */
                    गच्छ प्रत्यक्षम्_बहुलकम्;
                } अन्यथा {
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक++;
                    }
                    cnv->char_Error_बफर[0]=MINUS;
                    cnv->char_Error_बफर_दीर्घता=1;
                    *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                    भङ्ग;
                }
            } अन्यथा {
                /* un-read this character and switch to Unicode Mode */
                --source;
                *target++=PLUS;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                अन्तर्_प्रत्यक्षम्_बहुलकम्=असत्य;
                base64_गणक=0;
                गच्छ unicode_बहुलकम्;
            }
            --length;
        }
        यदि(source<source_सीमा && target>=target_सीमा) {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        }
    } अन्यथा {
unicode_बहुलकम्:
        यावत्(source<source_सीमा) {
            यदि(target<target_सीमा) {
                c=*source++;
                यदि(c<=127 && encodeDirectly[c]) {
                    /* encode directly */
                    अन्तर्_प्रत्यक्षम्_बहुलकम्=सत्य;

                    /* trick: back out this character to make this easier */
                    --source;

                    /* terminate the base64 sequence */
                    यदि(base64_गणक!=0) {
                        /* write remaining bits for the previous character */
                        *target++=गन्तव्यम्_Base64[द्वयङ्काः];
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=source_सूचकांक-1;
                        }
                    }
                    यदि(fromBase64[c]!=-1) {
                        /* need to terminate with a minus */
                        यदि(target<target_सीमा) {
                            *target++=MINUS;
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक-1;
                            }
                        } अन्यथा {
                            cnv->char_Error_बफर[0]=MINUS;
                            cnv->char_Error_बफर_दीर्घता=1;
                            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                            भङ्ग;
                        }
                    }
                    गच्छ प्रत्यक्षम्_बहुलकम्;
                } अन्यथा {
                    /*
                     * base64 this character:
                     * Output 2 or 3 base64 bytes for the remaining bits of the previous character
                     * and the bits of this character, each implicitly in UTF-16BE.
                     *
                     * Here, bits is an 8-bit variable because only 6 bits need to be kept from one
                     * character to the next. The actual 2 or 4 bits are shifted to the left edge
                     * of the 6-bits field 5..0 to make the termination of the base64 sequence easier.
                     */
                    विकल्प(base64_गणक) {
                    पक्ष 0:
                        *target++=गन्तव्यम्_Base64[c>>10];
                        यदि(target<target_सीमा) {
                            *target++=गन्तव्यम्_Base64[(c>>4)&0x3f];
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक;
                                *offsets++=source_सूचकांक++;
                            }
                        } अन्यथा {
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक++;
                            }
                            cnv->char_Error_बफर[0]=गन्तव्यम्_Base64[(c>>4)&0x3f];
                            cnv->char_Error_बफर_दीर्घता=1;
                            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                        }
                        द्वयङ्काः=(uint8_t)((c&15)<<2);
                        base64_गणक=1;
                        भङ्ग;
                    पक्ष 1:
                        *target++=गन्तव्यम्_Base64[द्वयङ्काः|(c>>14)];
                        यदि(target<target_सीमा) {
                            *target++=गन्तव्यम्_Base64[(c>>8)&0x3f];
                            यदि(target<target_सीमा) {
                                *target++=गन्तव्यम्_Base64[(c>>2)&0x3f];
                                यदि(offsets!=शून्यसूचकः) {
                                    *offsets++=source_सूचकांक;
                                    *offsets++=source_सूचकांक;
                                    *offsets++=source_सूचकांक++;
                                }
                            } अन्यथा {
                                यदि(offsets!=शून्यसूचकः) {
                                    *offsets++=source_सूचकांक;
                                    *offsets++=source_सूचकांक++;
                                }
                                cnv->char_Error_बफर[0]=गन्तव्यम्_Base64[(c>>2)&0x3f];
                                cnv->char_Error_बफर_दीर्घता=1;
                                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                            }
                        } अन्यथा {
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक++;
                            }
                            cnv->char_Error_बफर[0]=गन्तव्यम्_Base64[(c>>8)&0x3f];
                            cnv->char_Error_बफर[1]=गन्तव्यम्_Base64[(c>>2)&0x3f];
                            cnv->char_Error_बफर_दीर्घता=2;
                            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                        }
                        द्वयङ्काः=(uint8_t)((c&3)<<4);
                        base64_गणक=2;
                        भङ्ग;
                    पक्ष 2:
                        *target++=गन्तव्यम्_Base64[द्वयङ्काः|(c>>12)];
                        यदि(target<target_सीमा) {
                            *target++=गन्तव्यम्_Base64[(c>>6)&0x3f];
                            यदि(target<target_सीमा) {
                                *target++=गन्तव्यम्_Base64[c&0x3f];
                                यदि(offsets!=शून्यसूचकः) {
                                    *offsets++=source_सूचकांक;
                                    *offsets++=source_सूचकांक;
                                    *offsets++=source_सूचकांक++;
                                }
                            } अन्यथा {
                                यदि(offsets!=शून्यसूचकः) {
                                    *offsets++=source_सूचकांक;
                                    *offsets++=source_सूचकांक++;
                                }
                                cnv->char_Error_बफर[0]=गन्तव्यम्_Base64[c&0x3f];
                                cnv->char_Error_बफर_दीर्घता=1;
                                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                            }
                        } अन्यथा {
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक++;
                            }
                            cnv->char_Error_बफर[0]=गन्तव्यम्_Base64[(c>>6)&0x3f];
                            cnv->char_Error_बफर[1]=गन्तव्यम्_Base64[c&0x3f];
                            cnv->char_Error_बफर_दीर्घता=2;
                            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                        }
                        द्वयङ्काः=0;
                        base64_गणक=0;
                        भङ्ग;
                    स्वभाव:
                        /* will never occur */
                        भङ्ग;
                    }
                }
            } अन्यथा {
                /* target is full */
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
        }
    }

    यदि(p_तर्काः->flush && source>=source_सीमा) {
        /* flush remaining bits to the target */
        यदि(!अन्तर्_प्रत्यक्षम्_बहुलकम्) {
            यदि (base64_गणक!=0) {
                यदि(target<target_सीमा) {
                    *target++=गन्तव्यम्_Base64[द्वयङ्काः];
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक-1;
                    }
                } अन्यथा {
                    cnv->char_Error_बफर[cnv->char_Error_बफर_दीर्घता++]=गन्तव्यम्_Base64[द्वयङ्काः];
                    *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                }
            }
            /* Add final MINUS to terminate unicodeMode */
            यदि(target<target_सीमा) {
                *target++=MINUS;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक-1;
                }
            } अन्यथा {
                cnv->char_Error_बफर[cnv->char_Error_बफर_दीर्घता++]=MINUS;
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            }
        }
        /* reset the state for the next conversion */
        cnv->from_Unicode_स्थितिः=(cnv->from_Unicode_स्थितिः&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    } अन्यथा {
        /* set the converter state back into UConverter */
        cnv->from_Unicode_स्थितिः=
            (cnv->from_Unicode_स्थितिः&0xf0000000)|    /* keep version*/
            ((uint32_t)अन्तर्_प्रत्यक्षम्_बहुलकम्<<24)|((uint32_t)base64_गणक<<16)|(uint32_t)द्वयङ्काः;
    }

    /* write back the updated pointers */
    p_तर्काः->source=source;
    p_तर्काः->target=(वर्ण *)target;
    p_तर्काः->offsets=offsets;
}

स्थिर ध्रुव वर्ण * U_CALLCONV
_UTF7GetName(ध्रुव UConverter *cnv) {
    विकल्प(cnv->from_Unicode_स्थितिः>>28) {
    पक्ष 1:
        प्रत्यागम "UTF-7,version=1";
    स्वभाव:
        प्रत्यागम "UTF-7";
    }
}
U_CDECL_अन्तः

स्थिर ध्रुव UConverterImpl _UTF7Impl={
    UCNV_UTF7,

    शून्यसूचकः,
    शून्यसूचकः,

    _UTF7Open,
    शून्यसूचकः,
    _UTF7Reset,

    UTF7_गन्तव्यम्_Unicode_With_Offsets,
    UTF7_गन्तव्यम्_Unicode_With_Offsets,
    _UTF7FromUnicodeWithOffsets,
    _UTF7FromUnicodeWithOffsets,
    शून्यसूचकः,

    शून्यसूचकः,
    _UTF7GetName,
    शून्यसूचकः, /* we don't need writeSub() because we never call a callback at fromUnicode() */
    शून्यसूचकः,
    ucnv_get_Complete_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};

स्थिर ध्रुव UConverter_Static_दत्त UTF7_Static_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "UTF-7",
    0, /* TODO CCSID for UTF-7 */
    UCNV_IBM, UCNV_UTF7,
    1, 4,
    { 0x3f, 0, 0, 0 }, 1, /* the subchar is not used */
    असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ध्रुव UConverter_Shared_दत्त UTF7_दत्त=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&UTF7_Static_दत्त, &_UTF7Impl);

/* IMAP mailbox name encoding ----------------------------------------------- */

/*
 * RFC 2060: INTERNET MESSAGE ACCESS PROTOCOL - VERSION 4rev1
 * http://www.ietf.org/rfc/rfc2060.txt
 *
 * 5.1.3.  Mailbox International Naming Convention
 *
 * By convention, international mailbox names are specified using a
 * modified version of the UTF-7 encoding described in [UTF-7].  The
 * purpose of these modifications is to correct the following problems
 * with UTF-7:
 *
 *    1) UTF-7 uses the "+" character for shifting; this conflicts with
 *       the common use of "+" in mailbox names, in particular USENET
 *       newsgroup names.
 *
 *    2) UTF-7's encoding is BASE64 which uses the "/" character; this
 *       conflicts with the use of "/" as a popular hierarchy delimiter.
 *
 *    3) UTF-7 prohibits the unencoded usage of "\"; this conflicts with
 *       the use of "\" as a popular hierarchy delimiter.
 *
 *    4) UTF-7 prohibits the unencoded usage of "~"; this conflicts with
 *       the use of "~" in some servers as a home directory indicator.
 *
 *    5) UTF-7 permits multiple alternate forms to represent the same
 *       string; in particular, printable US-ASCII characters can be
 *       represented in encoded form.
 *
 * In modified UTF-7, printable US-ASCII characters except for "&"
 * represent themselves; that is, characters with octet values 0x20-0x25
 * and 0x27-0x7e.  The character "&" (0x26) is represented by the two-
 * octet sequence "&-".
 *
 * All other characters (octet values 0x00-0x1f, 0x7f-0xff, and all
 * Unicode 16-bit octets) are represented in modified BASE64, with a
 * further modification from [UTF-7] that "," is used instead of "/".
 * Modified BASE64 MUST NOT be used to represent any printing US-ASCII
 * character which can represent itself.
 *
 * "&" is used to shift to modified BASE64 and "-" to shift back to US-
 * ASCII.  All names start in US-ASCII, and MUST end in US-ASCII (that
 * is, a name that ends with a Unicode 16-bit octet MUST end with a "-
 * ").
 *
 * For example, here is a mailbox name which mixes English, Japanese,
 * and Chinese text: ~peter/mail/&ZeVnLIqe-/&U,BTFw-
 */

/*
 * Tests for US-ASCII characters belonging to character classes
 * defined in UTF-7.
 *
 * Set D (directly encoded characters) consists of the following
 * characters: the upper and lower case letters A through Z
 * and a through z, the 10 digits 0-9, and the following nine special
 * characters (note that "+" and "=" are omitted):
 *     '(),-./:?
 *
 * Set O (optional direct characters) consists of the following
 * characters (note that "\" and "~" are omitted):
 *     !"#$%&*;<=>@[]^_`{|}
 *
 * According to the rules in RFC 2152, the byte values for the following
 * US-ASCII characters are not used in UTF-7 and are therefore illegal:
 * - all C0 control codes except for CR LF TAB
 * - BACKSLASH
 * - TILDE
 * - DEL
 * - all codes beyond US-ASCII, i.e. all >127
 */

/* uses '&' not '+' to start a base64 sequence */
#परिभाषय AMPERSAND 0x26
#परिभाषय COMMA 0x2c
#परिभाषय SLASH 0x2f

/* legal byte values: all US-ASCII graphic characters 0x20..0x7e */
#परिभाषय isLegalIMAP(c) (0x20<=(c) && (c)<=0x7e)

/* direct-encode all of printable ASCII 0x20..0x7e except '&' 0x26 */
#परिभाषय अन्तर्_समुच्चयः_DIMAP(c) (isLegalIMAP(c) && c!=AMPERSAND)

#परिभाषय गन्तव्यम्_BASE64_IMAP(n) ((n)<63 ? गन्तव्यम्_Base64[n] : COMMA)
#परिभाषय FROM_BASE64_IMAP(c) ((c)==COMMA ? 63 : (c)==SLASH ? -1 : fromBase64[c])

/*
 * converter status values:
 *
 * toUnicodeStatus:
 *     24 inDirectMode (boolean)
 * 23..16 base64Counter (-1..7)
 * 15..0  bits (up to 14 bits incoming base64)
 *
 * fromUnicodeStatus:
 *     24 inDirectMode (boolean)
 * 23..16 base64Counter (0..2)
 *  7..0  bits (6 bits outgoing base64)
 *
 * ignore bits 31..25
 */

U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
_IMAPToUnicodeWithOffsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                          UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव uint8_t *source, *source_सीमा;
    char16_t *target;
    ध्रुव char16_t *target_सीमा;
    int32_t *offsets;

    uint8_t *अष्टकानि;
    uint8_t byte_सूचकांक;

    int32_t length, target_क्षमता;

    /* UTF-7 state */
    uint16_t द्वयङ्काः;
    int8_t base64_गणक;
    UBool अन्तर्_प्रत्यक्षम्_बहुलकम्;

    int8_t base64_मानम्;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    char16_t c;
    uint8_t b;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;

    source=(ध्रुव uint8_t *)p_तर्काः->source;
    source_सीमा=(ध्रुव uint8_t *)p_तर्काः->source_सीमा;
    target=p_तर्काः->target;
    target_सीमा=p_तर्काः->target_सीमा;
    offsets=p_तर्काः->offsets;
    /* get the state machine state */
    {
        uint32_t स्थितिः=cnv->गन्तव्यम्_Unicode_स्थितिः;
        अन्तर्_प्रत्यक्षम्_बहुलकम्=(UBool)((स्थितिः>>24)&1);
        base64_गणक=(int8_t)(स्थितिः>>16);
        द्वयङ्काः=(uint16_t)स्थितिः;
    }
    अष्टकानि=cnv->गन्तव्यम्_UBytes;
    byte_सूचकांक=cnv->गन्तव्यम्_ULength;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक=byte_सूचकांक==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    यदि(अन्तर्_प्रत्यक्षम्_बहुलकम्) {
प्रत्यक्षम्_बहुलकम्:
        /*
         * In Direct Mode, US-ASCII characters are encoded directly, i.e.,
         * with their US-ASCII byte values.
         * An ampersand starts Unicode (or "escape") Mode.
         *
         * In Direct Mode, only the sourceIndex is used.
         */
        byte_सूचकांक=0;
        length=(int32_t)(source_सीमा-source);
        target_क्षमता=(int32_t)(target_सीमा-target);
        यदि(length>target_क्षमता) {
            length=target_क्षमता;
        }
        यावत्(length>0) {
            b=*source++;
            यदि(!isLegalIMAP(b)) {
                /* illegal */
                अष्टकानि[0]=b;
                byte_सूचकांक=1;
                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                भङ्ग;
            } अन्यथा यदि(b!=AMPERSAND) {
                /* write directly encoded character */
                *target++=b;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक++;
                }
            } अन्यथा /* AMPERSAND */ {
                /* switch to Unicode mode */
                अग्र_Source_सूचकांक=++source_सूचकांक;
                अन्तर्_प्रत्यक्षम्_बहुलकम्=असत्य;
                byte_सूचकांक=0;
                द्वयङ्काः=0;
                base64_गणक=-1;
                गच्छ unicode_बहुलकम्;
            }
            --length;
        }
        यदि(source<source_सीमा && target>=target_सीमा) {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        }
    } अन्यथा {
unicode_बहुलकम्:
        /*
         * In Unicode (or "escape") Mode, UTF-16BE is base64-encoded.
         * The base64 sequence ends with any character that is not in the base64 alphabet.
         * A terminating minus sign is consumed.
         * US-ASCII must not be base64-ed.
         *
         * In Unicode Mode, the sourceIndex has the index to the start of the current
         * base64 bytes, while nextSourceIndex is precisely parallel to source,
         * keeping the index to the following byte.
         * Note that in 2 out of 3 cases, UChars overlap within a base64 byte.
         */
        यावत्(source<source_सीमा) {
            यदि(target<target_सीमा) {
                अष्टकानि[byte_सूचकांक++]=b=*source++;
                ++अग्र_Source_सूचकांक;
                यदि(b>0x7e) {
                    /* illegal - test other illegal US-ASCII values by base64Value==-3 */
                    अन्तर्_प्रत्यक्षम्_बहुलकम्=सत्य;
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    भङ्ग;
                } अन्यथा यदि((base64_मानम्=FROM_BASE64_IMAP(b))>=0) {
                    /* collect base64 bytes into UChars */
                    विकल्प(base64_गणक) {
                    पक्ष -1: /* -1 is immediately after the & */
                    पक्ष 0:
                        द्वयङ्काः=base64_मानम्;
                        base64_गणक=1;
                        भङ्ग;
                    पक्ष 1:
                    पक्ष 3:
                    पक्ष 4:
                    पक्ष 6:
                        द्वयङ्काः=(uint16_t)((द्वयङ्काः<<6)|base64_मानम्);
                        ++base64_गणक;
                        भङ्ग;
                    पक्ष 2:
                        c=(char16_t)((द्वयङ्काः<<4)|(base64_मानम्>>2));
                        यदि(isLegalIMAP(c)) {
                            /* illegal */
                            अन्तर्_प्रत्यक्षम्_बहुलकम्=सत्य;
                            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                            गच्छ endloop;
                        }
                        *target++=c;
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=source_सूचकांक;
                            source_सूचकांक=अग्र_Source_सूचकांक-1;
                        }
                        अष्टकानि[0]=b; /* keep this byte in case an error occurs */
                        byte_सूचकांक=1;
                        द्वयङ्काः=(uint16_t)(base64_मानम्&3);
                        base64_गणक=3;
                        भङ्ग;
                    पक्ष 5:
                        c=(char16_t)((द्वयङ्काः<<2)|(base64_मानम्>>4));
                        यदि(isLegalIMAP(c)) {
                            /* illegal */
                            अन्तर्_प्रत्यक्षम्_बहुलकम्=सत्य;
                            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                            गच्छ endloop;
                        }
                        *target++=c;
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=source_सूचकांक;
                            source_सूचकांक=अग्र_Source_सूचकांक-1;
                        }
                        अष्टकानि[0]=b; /* keep this byte in case an error occurs */
                        byte_सूचकांक=1;
                        द्वयङ्काः=(uint16_t)(base64_मानम्&15);
                        base64_गणक=6;
                        भङ्ग;
                    पक्ष 7:
                        c=(char16_t)((द्वयङ्काः<<6)|base64_मानम्);
                        यदि(isLegalIMAP(c)) {
                            /* illegal */
                            अन्तर्_प्रत्यक्षम्_बहुलकम्=सत्य;
                            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                            गच्छ endloop;
                        }
                        *target++=c;
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=source_सूचकांक;
                            source_सूचकांक=अग्र_Source_सूचकांक;
                        }
                        byte_सूचकांक=0;
                        द्वयङ्काः=0;
                        base64_गणक=0;
                        भङ्ग;
                    स्वभाव:
                        /* will never occur */
                        भङ्ग;
                    }
                } अन्यथा यदि(base64_मानम्==-2) {
                    /* minus sign terminates the base64 sequence */
                    अन्तर्_प्रत्यक्षम्_बहुलकम्=सत्य;
                    यदि(base64_गणक==-1) {
                        /* &- i.e. a minus immediately following an ampersand */
                        *target++=AMPERSAND;
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=source_सूचकांक-1;
                        }
                    } अन्यथा {
                        /* absorb the minus and leave the Unicode Mode */
                        यदि(द्वयङ्काः!=0 || (base64_गणक!=0 && base64_गणक!=3 && base64_गणक!=6)) {
                            /* bits are illegally left over, a char16_t is incomplete */
                            /* base64Counter other than 0, 3, 6 means non-minimal zero-padding, also illegal */
                            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                            भङ्ग;
                        }
                    }
                    source_सूचकांक=अग्र_Source_सूचकांक;
                    गच्छ प्रत्यक्षम्_बहुलकम्;
                } अन्यथा {
                    यदि(base64_गणक==-1) {
                        /* illegal: & immediately followed by something other than base64 or minus sign */
                        /* include the ampersand in the reported sequence */
                        --source_सूचकांक;
                        अष्टकानि[0]=AMPERSAND;
                        अष्टकानि[1]=b;
                        byte_सूचकांक=2;
                    }
                    /* base64Value==-1 for characters that are illegal only in Unicode mode */
                    /* base64Value==-3 for illegal characters */
                    /* illegal */
                    अन्तर्_प्रत्यक्षम्_बहुलकम्=सत्य;
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    भङ्ग;
                }
            } अन्यथा {
                /* target is full */
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
        }
    }
endloop:

    /*
     * the end of the input stream and detection of truncated input
     * are handled by the framework, but here we must check if we are in Unicode
     * mode and byteIndex==0 because we must end in direct mode
     *
     * conditions:
     *   successful
     *   in Unicode mode and byteIndex==0
     *   end of input and no truncated input
     */
    यदि( U_SUCCESS(*p_Error_संहिता) &&
        !अन्तर्_प्रत्यक्षम्_बहुलकम् && byte_सूचकांक==0 &&
        p_तर्काः->flush && source>=source_सीमा
    ) {
        यदि(base64_गणक==-1) {
            /* & at the very end of the input */
            /* make the ampersand the reported sequence */
            अष्टकानि[0]=AMPERSAND;
            byte_सूचकांक=1;
        }
        /* else if(base64Counter!=-1) byteIndex remains 0 because there is no particular byte sequence */

        अन्तर्_प्रत्यक्षम्_बहुलकम्=सत्य; /* avoid looping */
        *p_Error_संहिता=U_TRUNCATED_CHAR_प्राप्तम्;
    }

    /* set the converter state back into UConverter */
    cnv->गन्तव्यम्_Unicode_स्थितिः=((uint32_t)अन्तर्_प्रत्यक्षम्_बहुलकम्<<24)|((uint32_t)((uint8_t)base64_गणक)<<16)|(uint32_t)द्वयङ्काः;
    cnv->गन्तव्यम्_ULength=byte_सूचकांक;

    /* write back the updated pointers */
    p_तर्काः->source=(ध्रुव वर्ण *)source;
    p_तर्काः->target=target;
    p_तर्काः->offsets=offsets;
}

स्थिर शून्य U_CALLCONV
_IMAPFromUnicodeWithOffsets(UConverter_From_Unicode_तर्काः *p_तर्काः,
                            UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव char16_t *source, *source_सीमा;
    uint8_t *target, *target_सीमा;
    int32_t *offsets;

    int32_t length, target_क्षमता, source_सूचकांक;
    char16_t c;
    uint8_t b;

    /* UTF-7 state */
    uint8_t द्वयङ्काः;
    int8_t base64_गणक;
    UBool अन्तर्_प्रत्यक्षम्_बहुलकम्;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;

    /* set up the local pointers */
    source=p_तर्काः->source;
    source_सीमा=p_तर्काः->source_सीमा;
    target=(uint8_t *)p_तर्काः->target;
    target_सीमा=(uint8_t *)p_तर्काः->target_सीमा;
    offsets=p_तर्काः->offsets;

    /* get the state machine state */
    {
        uint32_t स्थितिः=cnv->from_Unicode_स्थितिः;
        अन्तर्_प्रत्यक्षम्_बहुलकम्=(UBool)((स्थितिः>>24)&1);
        base64_गणक=(int8_t)(स्थितिः>>16);
        द्वयङ्काः=(uint8_t)स्थितिः;
    }

    /* UTF-7 always encodes UTF-16 code units, therefore we need only a simple sourceIndex */
    source_सूचकांक=0;

    यदि(अन्तर्_प्रत्यक्षम्_बहुलकम्) {
प्रत्यक्षम्_बहुलकम्:
        length=(int32_t)(source_सीमा-source);
        target_क्षमता=(int32_t)(target_सीमा-target);
        यदि(length>target_क्षमता) {
            length=target_क्षमता;
        }
        यावत्(length>0) {
            c=*source++;
            /* encode 0x20..0x7e except '&' directly */
            यदि(अन्तर्_समुच्चयः_DIMAP(c)) {
                /* encode directly */
                *target++=(uint8_t)c;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक++;
                }
            } अन्यथा यदि(c==AMPERSAND) {
                /* output &- for & */
                *target++=AMPERSAND;
                यदि(target<target_सीमा) {
                    *target++=MINUS;
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                        *offsets++=source_सूचकांक++;
                    }
                    /* realign length and targetCapacity */
                    गच्छ प्रत्यक्षम्_बहुलकम्;
                } अन्यथा {
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक++;
                    }
                    cnv->char_Error_बफर[0]=MINUS;
                    cnv->char_Error_बफर_दीर्घता=1;
                    *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                    भङ्ग;
                }
            } अन्यथा {
                /* un-read this character and switch to Unicode Mode */
                --source;
                *target++=AMPERSAND;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                अन्तर्_प्रत्यक्षम्_बहुलकम्=असत्य;
                base64_गणक=0;
                गच्छ unicode_बहुलकम्;
            }
            --length;
        }
        यदि(source<source_सीमा && target>=target_सीमा) {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        }
    } अन्यथा {
unicode_बहुलकम्:
        यावत्(source<source_सीमा) {
            यदि(target<target_सीमा) {
                c=*source++;
                यदि(isLegalIMAP(c)) {
                    /* encode directly */
                    अन्तर्_प्रत्यक्षम्_बहुलकम्=सत्य;

                    /* trick: back out this character to make this easier */
                    --source;

                    /* terminate the base64 sequence */
                    यदि(base64_गणक!=0) {
                        /* write remaining bits for the previous character */
                        *target++=गन्तव्यम्_BASE64_IMAP(द्वयङ्काः);
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=source_सूचकांक-1;
                        }
                    }
                    /* need to terminate with a minus */
                    यदि(target<target_सीमा) {
                        *target++=MINUS;
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=source_सूचकांक-1;
                        }
                    } अन्यथा {
                        cnv->char_Error_बफर[0]=MINUS;
                        cnv->char_Error_बफर_दीर्घता=1;
                        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                        भङ्ग;
                    }
                    गच्छ प्रत्यक्षम्_बहुलकम्;
                } अन्यथा {
                    /*
                     * base64 this character:
                     * Output 2 or 3 base64 bytes for the remaining bits of the previous character
                     * and the bits of this character, each implicitly in UTF-16BE.
                     *
                     * Here, bits is an 8-bit variable because only 6 bits need to be kept from one
                     * character to the next. The actual 2 or 4 bits are shifted to the left edge
                     * of the 6-bits field 5..0 to make the termination of the base64 sequence easier.
                     */
                    विकल्प(base64_गणक) {
                    पक्ष 0:
                        b=(uint8_t)(c>>10);
                        *target++=गन्तव्यम्_BASE64_IMAP(b);
                        यदि(target<target_सीमा) {
                            b=(uint8_t)((c>>4)&0x3f);
                            *target++=गन्तव्यम्_BASE64_IMAP(b);
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक;
                                *offsets++=source_सूचकांक++;
                            }
                        } अन्यथा {
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक++;
                            }
                            b=(uint8_t)((c>>4)&0x3f);
                            cnv->char_Error_बफर[0]=गन्तव्यम्_BASE64_IMAP(b);
                            cnv->char_Error_बफर_दीर्घता=1;
                            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                        }
                        द्वयङ्काः=(uint8_t)((c&15)<<2);
                        base64_गणक=1;
                        भङ्ग;
                    पक्ष 1:
                        b=(uint8_t)(द्वयङ्काः|(c>>14));
                        *target++=गन्तव्यम्_BASE64_IMAP(b);
                        यदि(target<target_सीमा) {
                            b=(uint8_t)((c>>8)&0x3f);
                            *target++=गन्तव्यम्_BASE64_IMAP(b);
                            यदि(target<target_सीमा) {
                                b=(uint8_t)((c>>2)&0x3f);
                                *target++=गन्तव्यम्_BASE64_IMAP(b);
                                यदि(offsets!=शून्यसूचकः) {
                                    *offsets++=source_सूचकांक;
                                    *offsets++=source_सूचकांक;
                                    *offsets++=source_सूचकांक++;
                                }
                            } अन्यथा {
                                यदि(offsets!=शून्यसूचकः) {
                                    *offsets++=source_सूचकांक;
                                    *offsets++=source_सूचकांक++;
                                }
                                b=(uint8_t)((c>>2)&0x3f);
                                cnv->char_Error_बफर[0]=गन्तव्यम्_BASE64_IMAP(b);
                                cnv->char_Error_बफर_दीर्घता=1;
                                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                            }
                        } अन्यथा {
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक++;
                            }
                            b=(uint8_t)((c>>8)&0x3f);
                            cnv->char_Error_बफर[0]=गन्तव्यम्_BASE64_IMAP(b);
                            b=(uint8_t)((c>>2)&0x3f);
                            cnv->char_Error_बफर[1]=गन्तव्यम्_BASE64_IMAP(b);
                            cnv->char_Error_बफर_दीर्घता=2;
                            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                        }
                        द्वयङ्काः=(uint8_t)((c&3)<<4);
                        base64_गणक=2;
                        भङ्ग;
                    पक्ष 2:
                        b=(uint8_t)(द्वयङ्काः|(c>>12));
                        *target++=गन्तव्यम्_BASE64_IMAP(b);
                        यदि(target<target_सीमा) {
                            b=(uint8_t)((c>>6)&0x3f);
                            *target++=गन्तव्यम्_BASE64_IMAP(b);
                            यदि(target<target_सीमा) {
                                b=(uint8_t)(c&0x3f);
                                *target++=गन्तव्यम्_BASE64_IMAP(b);
                                यदि(offsets!=शून्यसूचकः) {
                                    *offsets++=source_सूचकांक;
                                    *offsets++=source_सूचकांक;
                                    *offsets++=source_सूचकांक++;
                                }
                            } अन्यथा {
                                यदि(offsets!=शून्यसूचकः) {
                                    *offsets++=source_सूचकांक;
                                    *offsets++=source_सूचकांक++;
                                }
                                b=(uint8_t)(c&0x3f);
                                cnv->char_Error_बफर[0]=गन्तव्यम्_BASE64_IMAP(b);
                                cnv->char_Error_बफर_दीर्घता=1;
                                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                            }
                        } अन्यथा {
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक++;
                            }
                            b=(uint8_t)((c>>6)&0x3f);
                            cnv->char_Error_बफर[0]=गन्तव्यम्_BASE64_IMAP(b);
                            b=(uint8_t)(c&0x3f);
                            cnv->char_Error_बफर[1]=गन्तव्यम्_BASE64_IMAP(b);
                            cnv->char_Error_बफर_दीर्घता=2;
                            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                        }
                        द्वयङ्काः=0;
                        base64_गणक=0;
                        भङ्ग;
                    स्वभाव:
                        /* will never occur */
                        भङ्ग;
                    }
                }
            } अन्यथा {
                /* target is full */
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
        }
    }

    यदि(p_तर्काः->flush && source>=source_सीमा) {
        /* flush remaining bits to the target */
        यदि(!अन्तर्_प्रत्यक्षम्_बहुलकम्) {
            यदि(base64_गणक!=0) {
                यदि(target<target_सीमा) {
                    *target++=गन्तव्यम्_BASE64_IMAP(द्वयङ्काः);
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक-1;
                    }
                } अन्यथा {
                    cnv->char_Error_बफर[cnv->char_Error_बफर_दीर्घता++]=गन्तव्यम्_BASE64_IMAP(द्वयङ्काः);
                    *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                }
            }
            /* need to terminate with a minus */
            यदि(target<target_सीमा) {
                *target++=MINUS;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक-1;
                }
            } अन्यथा {
                cnv->char_Error_बफर[cnv->char_Error_बफर_दीर्घता++]=MINUS;
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            }
        }
        /* reset the state for the next conversion */
        cnv->from_Unicode_स्थितिः=(cnv->from_Unicode_स्थितिः&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    } अन्यथा {
        /* set the converter state back into UConverter */
        cnv->from_Unicode_स्थितिः=
            (cnv->from_Unicode_स्थितिः&0xf0000000)|    /* keep version*/
            ((uint32_t)अन्तर्_प्रत्यक्षम्_बहुलकम्<<24)|((uint32_t)base64_गणक<<16)|(uint32_t)द्वयङ्काः;
    }

    /* write back the updated pointers */
    p_तर्काः->source=source;
    p_तर्काः->target=(वर्ण *)target;
    p_तर्काः->offsets=offsets;
}
U_CDECL_अन्तः

स्थिर ध्रुव UConverterImpl _IMAPImpl={
    UCNV_IMAP_MAILBOX,

    शून्यसूचकः,
    शून्यसूचकः,

    _UTF7Open,
    शून्यसूचकः,
    _UTF7Reset,

    _IMAPToUnicodeWithOffsets,
    _IMAPToUnicodeWithOffsets,
    _IMAPFromUnicodeWithOffsets,
    _IMAPFromUnicodeWithOffsets,
    शून्यसूचकः,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः, /* we don't need writeSub() because we never call a callback at fromUnicode() */
    शून्यसूचकः,
    ucnv_get_Complete_Unicode_समुच्चयः,
    शून्यसूचकः,
    शून्यसूचकः
};

स्थिर ध्रुव UConverter_Static_दत्त IMAPStatic_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "IMAP-mailbox-name",
    0, /* TODO CCSID for IMAP-mailbox-name */
    UCNV_IBM, UCNV_IMAP_MAILBOX,
    1, 4,
    { 0x3f, 0, 0, 0 }, 1, /* the subchar is not used */
    असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ध्रुव UConverter_Shared_दत्त _IMAPData=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&IMAPStatic_दत्त, &_IMAPImpl);

#यदि_अन्तः
