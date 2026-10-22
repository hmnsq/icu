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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "uassert.h"

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
#నిర్వచించు లోపల_సమితి_D(c) \
    ((uint8_t)((c)-97)<26 || (uint8_t)((c)-65)<26 || /* letters */ \
     (uint8_t)((c)-48)<10 ||    /* digits */ \
     (uint8_t)((c)-39)<3 ||     /* '() */ \
     (uint8_t)((c)-44)<4 ||     /* ,-./ */ \
     (c)==58 || (c)==63         /* :? */ \
    )

#నిర్వచించు లోపల_సమితి_O(c) \
    ((uint8_t)((c)-33)<6 ||         /* !"#$%& */ \
     (uint8_t)((c)-59)<4 ||         /* ;<=> */ \
     (uint8_t)((c)-93)<4 ||         /* ]^_` */ \
     (uint8_t)((c)-123)<3 ||        /* {|} */ \
     (c)==42 || (c)==64 || (c)==91  /* *@[ */ \
    )

#నిర్వచించు isCRLFTAB(c) ((c)==13 || (c)==10 || (c)==9)
#నిర్వచించు isCRLFSPTAB(c) ((c)==32 || (c)==13 || (c)==10 || (c)==9)

#నిర్వచించు PLUS  43
#నిర్వచించు MINUS 45
#నిర్వచించు BACKSLASH 92
#నిర్వచించు TILDE 126

/* legal byte values: all US-ASCII graphic characters from space to before tilde, and CR LF TAB */
#నిర్వచించు isLegalUTF7(c) (((uint8_t)((c)-32)<94 && (c)!=BACKSLASH) || isCRLFTAB(c))

/* encode directly sets D and O and CR LF SP TAB */
స్థిర స్థిరరాశి UBool encode_Directly_గరిష్ఠ[128]={
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
స్థిర స్థిరరాశి UBool encodeDirectlyRestricted[128]={
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

స్థిర స్థిరరాశి uint8_t
గమ్యం_Base64[64]={
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

స్థిర స్థిరరాశి int8_t
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
స్థిర శూన్య U_CALLCONV
_UTF7Reset(UConverter *cnv, UConverterResetChoice choice) {
    ఒకవేళ(choice<=UCNV_RESET_గమ్యం_UNICODE) {
        /* reset toUnicode */
        cnv->గమ్యం_Unicode_స్థితి=0x1000000; /* inDirectMode=true */
        cnv->గమ్యం_ULength=0;
    }
    ఒకవేళ(choice!=UCNV_RESET_గమ్యం_UNICODE) {
        /* reset fromUnicode */
        cnv->from_Unicode_స్థితి=(cnv->from_Unicode_స్థితి&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    }
}

స్థిర శూన్య U_CALLCONV
_UTF7Open(UConverter *cnv,
          UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
          UError_సంకేతం *p_Error_సంకేతం) {
    (శూన్య)p_వాదనలు;
    ఒకవేళ(UCNV_GET_సంస్కరణ(cnv)<=1) {
        /* TODO(markus): Should just use cnv->options rather than copying the version number. */
        cnv->from_Unicode_స్థితి=UCNV_GET_సంస్కరణ(cnv)<<28;
        _UTF7Reset(cnv, UCNV_RESET_BOTH);
    } లేకపోతే {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
    }
}

స్థిర శూన్య U_CALLCONV
UTF7_గమ్యం_Unicode_With_Offsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                          UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    char16_t *లక్ష్యం;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు;
    int32_t *offsets;

    uint8_t *బైట్లు;
    uint8_t byte_సూచీ;

    int32_t length, లక్ష్యం_సామర్థ్యం;

    /* UTF-7 state */
    uint16_t బిట్లు;
    int8_t base64Counter;
    UBool లోపల_ప్రత్యక్ష_బహుళకం;

    int8_t base64_విలువ;

    int32_t ఆధారం_సూచీ, తదుపరి_ఆధారం_సూచీ;

    uint8_t b;
    /* set up the local pointers */
    cnv=p_వాదనలు->converter;

    ఆధారం=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    లక్ష్యం_హద్దు=p_వాదనలు->లక్ష్యం_హద్దు;
    offsets=p_వాదనలు->offsets;
    /* get the state machine state */
    {
        uint32_t స్థితి_2=cnv->గమ్యం_Unicode_స్థితి;
        లోపల_ప్రత్యక్ష_బహుళకం=(UBool)((స్థితి_2>>24)&1);
        base64Counter=(int8_t)(స్థితి_2>>16);
        బిట్లు=(uint16_t)స్థితి_2;
    }
    బైట్లు=cnv->గమ్యం_UBytes;
    byte_సూచీ=cnv->గమ్యం_ULength;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ=byte_సూచీ==0 ? 0 : -1;
    తదుపరి_ఆధారం_సూచీ=0;

    ఒకవేళ(లోపల_ప్రత్యక్ష_బహుళకం) {
ప్రత్యక్ష_బహుళకం:
        /*
         * In Direct Mode, most US-ASCII characters are encoded directly, i.e.,
         * with their US-ASCII byte values.
         * Backslash and Tilde and most control characters are not allowed in UTF-7.
         * A plus sign starts Unicode (or "escape") Mode.
         *
         * In Direct Mode, only the sourceIndex is used.
         */
        byte_సూచీ=0;
        length=(int32_t)(ఆధారం_హద్దు-ఆధారం);
        లక్ష్యం_సామర్థ్యం=(int32_t)(లక్ష్యం_హద్దు-లక్ష్యం);
        ఒకవేళ(length>లక్ష్యం_సామర్థ్యం) {
            length=లక్ష్యం_సామర్థ్యం;
        }
        అయితే(length>0) {
            b=*ఆధారం++;
            ఒకవేళ(!isLegalUTF7(b)) {
                /* illegal */
                బైట్లు[0]=b;
                byte_సూచీ=1;
                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                ఆపు;
            } లేకపోతే ఒకవేళ(b!=PLUS) {
                /* write directly encoded character */
                *లక్ష్యం++=b;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ++;
                }
            } లేకపోతే /* PLUS */ {
                /* switch to Unicode mode */
                తదుపరి_ఆధారం_సూచీ=++ఆధారం_సూచీ;
                లోపల_ప్రత్యక్ష_బహుళకం=అబద్ధం;
                byte_సూచీ=0;
                బిట్లు=0;
                base64Counter=-1;
                వెళ్ళు unicode_బహుళకం;
            }
            --length;
        }
        ఒకవేళ(ఆధారం<ఆధారం_హద్దు && లక్ష్యం>=లక్ష్యం_హద్దు) {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        }
    } లేకపోతే {
unicode_బహుళకం:
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
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                బైట్లు[byte_సూచీ++]=b=*ఆధారం++;
                ++తదుపరి_ఆధారం_సూచీ;
                base64_విలువ = -3; /* initialize as illegal */
                ఒకవేళ(b>=126 || (base64_విలువ=fromBase64[b])==-3 || base64_విలువ==-1) {
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
                    లోపల_ప్రత్యక్ష_బహుళకం=నిజం;
                    ఒకవేళ(base64Counter==-1) {
                        /* illegal: + immediately followed by something other than base64 or minus sign */
                        /* include the plus sign in the reported sequence, but not the subsequent char */
                        --ఆధారం;
                        బైట్లు[0]=PLUS;
                        byte_సూచీ=1;
                        *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                        ఆపు;
                    } లేకపోతే ఒకవేళ(బిట్లు!=0) {
                        /* bits are illegally left over, a char16_t is incomplete */
                        /* don't include current char (legal or illegal) in error seq */
                        --ఆధారం;
                        --byte_సూచీ;
                        *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                        ఆపు;
                    } లేకపోతే {
                        /* previous char16_t was complete */
                        ఒకవేళ(base64_విలువ==-3) {
                            /* current character is illegal, deal with it here */
                            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                            ఆపు;
                        } లేకపోతే {
                            /* un-read the current character in case it is a plus sign */
                            --ఆధారం;
                            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ-1;
                            వెళ్ళు ప్రత్యక్ష_బహుళకం;
                        }
                    }
                } లేకపోతే ఒకవేళ(base64_విలువ>=0) {
                    /* collect base64 bytes into UChars */
                    ఎంపిక(base64Counter) {
                    స్థితి -1: /* -1 is immediately after the + */
                    స్థితి 0:
                        బిట్లు=base64_విలువ;
                        base64Counter=1;
                        ఆపు;
                    స్థితి 1:
                    స్థితి 3:
                    స్థితి 4:
                    స్థితి 6:
                        బిట్లు=(uint16_t)((బిట్లు<<6)|base64_విలువ);
                        ++base64Counter;
                        ఆపు;
                    స్థితి 2:
                        *లక్ష్యం++=(char16_t)((బిట్లు<<4)|(base64_విలువ>>2));
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=ఆధారం_సూచీ;
                            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ-1;
                        }
                        బైట్లు[0]=b; /* keep this byte in case an error occurs */
                        byte_సూచీ=1;
                        బిట్లు=(uint16_t)(base64_విలువ&3);
                        base64Counter=3;
                        ఆపు;
                    స్థితి 5:
                        *లక్ష్యం++=(char16_t)((బిట్లు<<2)|(base64_విలువ>>4));
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=ఆధారం_సూచీ;
                            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ-1;
                        }
                        బైట్లు[0]=b; /* keep this byte in case an error occurs */
                        byte_సూచీ=1;
                        బిట్లు=(uint16_t)(base64_విలువ&15);
                        base64Counter=6;
                        ఆపు;
                    స్థితి 7:
                        *లక్ష్యం++=(char16_t)((బిట్లు<<6)|base64_విలువ);
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=ఆధారం_సూచీ;
                            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                        }
                        byte_సూచీ=0;
                        బిట్లు=0;
                        base64Counter=0;
                        ఆపు;
                    అప్రమేయ:
                        /* will never occur */
                        ఆపు;
                    }
                } లేకపోతే /*base64Value==-2*/ {
                    /* minus sign terminates the base64 sequence */
                    లోపల_ప్రత్యక్ష_బహుళకం=నిజం;
                    ఒకవేళ(base64Counter==-1) {
                        /* +- i.e. a minus immediately following a plus */
                        *లక్ష్యం++=PLUS;
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=ఆధారం_సూచీ-1;
                        }
                    } లేకపోతే {
                        /* absorb the minus and leave the Unicode Mode */
                        ఒకవేళ(బిట్లు!=0) {
                            /* bits are illegally left over, a char16_t is incomplete */
                            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                            ఆపు;
                        }
                    }
                    ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                    వెళ్ళు ప్రత్యక్ష_బహుళకం;
                }
            } లేకపోతే {
                /* target is full */
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }
        }
    }

    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం) && p_వాదనలు->flush && ఆధారం==ఆధారం_హద్దు && బిట్లు==0) {
        /*
         * if we are in Unicode mode, then the byteIndex might not be 0,
         * but that is ok if bits==0
         * -> we set byteIndex=0 at the end of the stream to avoid a truncated error
         * (not true for IMAP-mailbox-name where we must end in direct mode)
         */
        byte_సూచీ=0;
    }

    /* set the converter state back into UConverter */
    cnv->గమ్యం_Unicode_స్థితి=((uint32_t)లోపల_ప్రత్యక్ష_బహుళకం<<24)|((uint32_t)((uint8_t)base64Counter)<<16)|(uint32_t)బిట్లు;
    cnv->గమ్యం_ULength=byte_సూచీ;

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
    p_వాదనలు->లక్ష్యం=లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

స్థిర శూన్య U_CALLCONV
_UTF7FromUnicodeWithOffsets(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
                            UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి char16_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం, *లక్ష్యం_హద్దు;
    int32_t *offsets;

    int32_t length, లక్ష్యం_సామర్థ్యం, ఆధారం_సూచీ;
    char16_t c;

    /* UTF-7 state */
    స్థిరరాశి UBool *encodeDirectly;
    uint8_t బిట్లు;
    int8_t base64Counter;
    UBool లోపల_ప్రత్యక్ష_బహుళకం;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;

    /* set up the local pointers */
    ఆధారం=p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం=(uint8_t *)p_వాదనలు->లక్ష్యం;
    లక్ష్యం_హద్దు=(uint8_t *)p_వాదనలు->లక్ష్యం_హద్దు;
    offsets=p_వాదనలు->offsets;

    /* get the state machine state */
    {
        uint32_t స్థితి_2=cnv->from_Unicode_స్థితి;
        encodeDirectly= స్థితి_2<0x10000000 ? encode_Directly_గరిష్ఠ : encodeDirectlyRestricted;
        లోపల_ప్రత్యక్ష_బహుళకం=(UBool)((స్థితి_2>>24)&1);
        base64Counter=(int8_t)(స్థితి_2>>16);
        బిట్లు=(uint8_t)స్థితి_2;
        U_ASSERT(బిట్లు<=UPRV_LENGTHOF(గమ్యం_Base64));
    }

    /* UTF-7 always encodes UTF-16 code units, therefore we need only a simple sourceIndex */
    ఆధారం_సూచీ=0;

    ఒకవేళ(లోపల_ప్రత్యక్ష_బహుళకం) {
ప్రత్యక్ష_బహుళకం:
        length=(int32_t)(ఆధారం_హద్దు-ఆధారం);
        లక్ష్యం_సామర్థ్యం=(int32_t)(లక్ష్యం_హద్దు-లక్ష్యం);
        ఒకవేళ(length>లక్ష్యం_సామర్థ్యం) {
            length=లక్ష్యం_సామర్థ్యం;
        }
        అయితే(length>0) {
            c=*ఆధారం++;
            /* currently always encode CR LF SP TAB directly */
            ఒకవేళ(c<=127 && encodeDirectly[c]) {
                /* encode directly */
                *లక్ష్యం++=(uint8_t)c;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ++;
                }
            } లేకపోతే ఒకవేళ(c==PLUS) {
                /* output +- for + */
                *లక్ష్యం++=PLUS;
                ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                    *లక్ష్యం++=MINUS;
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                        *offsets++=ఆధారం_సూచీ++;
                    }
                    /* realign length and targetCapacity */
                    వెళ్ళు ప్రత్యక్ష_బహుళకం;
                } లేకపోతే {
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ++;
                    }
                    cnv->char_Error_బఫరు[0]=MINUS;
                    cnv->char_Error_బఫరు_నిడివి=1;
                    *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                    ఆపు;
                }
            } లేకపోతే {
                /* un-read this character and switch to Unicode Mode */
                --ఆధారం;
                *లక్ష్యం++=PLUS;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                లోపల_ప్రత్యక్ష_బహుళకం=అబద్ధం;
                base64Counter=0;
                వెళ్ళు unicode_బహుళకం;
            }
            --length;
        }
        ఒకవేళ(ఆధారం<ఆధారం_హద్దు && లక్ష్యం>=లక్ష్యం_హద్దు) {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        }
    } లేకపోతే {
unicode_బహుళకం:
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                c=*ఆధారం++;
                ఒకవేళ(c<=127 && encodeDirectly[c]) {
                    /* encode directly */
                    లోపల_ప్రత్యక్ష_బహుళకం=నిజం;

                    /* trick: back out this character to make this easier */
                    --ఆధారం;

                    /* terminate the base64 sequence */
                    ఒకవేళ(base64Counter!=0) {
                        /* write remaining bits for the previous character */
                        *లక్ష్యం++=గమ్యం_Base64[బిట్లు];
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=ఆధారం_సూచీ-1;
                        }
                    }
                    ఒకవేళ(fromBase64[c]!=-1) {
                        /* need to terminate with a minus */
                        ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                            *లక్ష్యం++=MINUS;
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ-1;
                            }
                        } లేకపోతే {
                            cnv->char_Error_బఫరు[0]=MINUS;
                            cnv->char_Error_బఫరు_నిడివి=1;
                            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                            ఆపు;
                        }
                    }
                    వెళ్ళు ప్రత్యక్ష_బహుళకం;
                } లేకపోతే {
                    /*
                     * base64 this character:
                     * Output 2 or 3 base64 bytes for the remaining bits of the previous character
                     * and the bits of this character, each implicitly in UTF-16BE.
                     *
                     * Here, bits is an 8-bit variable because only 6 bits need to be kept from one
                     * character to the next. The actual 2 or 4 bits are shifted to the left edge
                     * of the 6-bits field 5..0 to make the termination of the base64 sequence easier.
                     */
                    ఎంపిక(base64Counter) {
                    స్థితి 0:
                        *లక్ష్యం++=గమ్యం_Base64[c>>10];
                        ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                            *లక్ష్యం++=గమ్యం_Base64[(c>>4)&0x3f];
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ;
                                *offsets++=ఆధారం_సూచీ++;
                            }
                        } లేకపోతే {
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ++;
                            }
                            cnv->char_Error_బఫరు[0]=గమ్యం_Base64[(c>>4)&0x3f];
                            cnv->char_Error_బఫరు_నిడివి=1;
                            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                        }
                        బిట్లు=(uint8_t)((c&15)<<2);
                        base64Counter=1;
                        ఆపు;
                    స్థితి 1:
                        *లక్ష్యం++=గమ్యం_Base64[బిట్లు|(c>>14)];
                        ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                            *లక్ష్యం++=గమ్యం_Base64[(c>>8)&0x3f];
                            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                                *లక్ష్యం++=గమ్యం_Base64[(c>>2)&0x3f];
                                ఒకవేళ(offsets!=శూన్యసూచిక) {
                                    *offsets++=ఆధారం_సూచీ;
                                    *offsets++=ఆధారం_సూచీ;
                                    *offsets++=ఆధారం_సూచీ++;
                                }
                            } లేకపోతే {
                                ఒకవేళ(offsets!=శూన్యసూచిక) {
                                    *offsets++=ఆధారం_సూచీ;
                                    *offsets++=ఆధారం_సూచీ++;
                                }
                                cnv->char_Error_బఫరు[0]=గమ్యం_Base64[(c>>2)&0x3f];
                                cnv->char_Error_బఫరు_నిడివి=1;
                                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                            }
                        } లేకపోతే {
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ++;
                            }
                            cnv->char_Error_బఫరు[0]=గమ్యం_Base64[(c>>8)&0x3f];
                            cnv->char_Error_బఫరు[1]=గమ్యం_Base64[(c>>2)&0x3f];
                            cnv->char_Error_బఫరు_నిడివి=2;
                            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                        }
                        బిట్లు=(uint8_t)((c&3)<<4);
                        base64Counter=2;
                        ఆపు;
                    స్థితి 2:
                        *లక్ష్యం++=గమ్యం_Base64[బిట్లు|(c>>12)];
                        ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                            *లక్ష్యం++=గమ్యం_Base64[(c>>6)&0x3f];
                            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                                *లక్ష్యం++=గమ్యం_Base64[c&0x3f];
                                ఒకవేళ(offsets!=శూన్యసూచిక) {
                                    *offsets++=ఆధారం_సూచీ;
                                    *offsets++=ఆధారం_సూచీ;
                                    *offsets++=ఆధారం_సూచీ++;
                                }
                            } లేకపోతే {
                                ఒకవేళ(offsets!=శూన్యసూచిక) {
                                    *offsets++=ఆధారం_సూచీ;
                                    *offsets++=ఆధారం_సూచీ++;
                                }
                                cnv->char_Error_బఫరు[0]=గమ్యం_Base64[c&0x3f];
                                cnv->char_Error_బఫరు_నిడివి=1;
                                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                            }
                        } లేకపోతే {
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ++;
                            }
                            cnv->char_Error_బఫరు[0]=గమ్యం_Base64[(c>>6)&0x3f];
                            cnv->char_Error_బఫరు[1]=గమ్యం_Base64[c&0x3f];
                            cnv->char_Error_బఫరు_నిడివి=2;
                            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                        }
                        బిట్లు=0;
                        base64Counter=0;
                        ఆపు;
                    అప్రమేయ:
                        /* will never occur */
                        ఆపు;
                    }
                }
            } లేకపోతే {
                /* target is full */
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }
        }
    }

    ఒకవేళ(p_వాదనలు->flush && ఆధారం>=ఆధారం_హద్దు) {
        /* flush remaining bits to the target */
        ఒకవేళ(!లోపల_ప్రత్యక్ష_బహుళకం) {
            ఒకవేళ (base64Counter!=0) {
                ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                    *లక్ష్యం++=గమ్యం_Base64[బిట్లు];
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ-1;
                    }
                } లేకపోతే {
                    cnv->char_Error_బఫరు[cnv->char_Error_బఫరు_నిడివి++]=గమ్యం_Base64[బిట్లు];
                    *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                }
            }
            /* Add final MINUS to terminate unicodeMode */
            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                *లక్ష్యం++=MINUS;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ-1;
                }
            } లేకపోతే {
                cnv->char_Error_బఫరు[cnv->char_Error_బఫరు_నిడివి++]=MINUS;
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            }
        }
        /* reset the state for the next conversion */
        cnv->from_Unicode_స్థితి=(cnv->from_Unicode_స్థితి&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    } లేకపోతే {
        /* set the converter state back into UConverter */
        cnv->from_Unicode_స్థితి=
            (cnv->from_Unicode_స్థితి&0xf0000000)|    /* keep version*/
            ((uint32_t)లోపల_ప్రత్యక్ష_బహుళకం<<24)|((uint32_t)base64Counter<<16)|(uint32_t)బిట్లు;
    }

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=ఆధారం;
    p_వాదనలు->లక్ష్యం=(అక్షరం *)లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

స్థిర స్థిరరాశి అక్షరం * U_CALLCONV
_UTF7GetName(స్థిరరాశి UConverter *cnv) {
    ఎంపిక(cnv->from_Unicode_స్థితి>>28) {
    స్థితి 1:
        తిరిగి "UTF-7,version=1";
    అప్రమేయ:
        తిరిగి "UTF-7";
    }
}
U_CDECL_చివర

స్థిర స్థిరరాశి UConverterImpl _UTF7Impl={
    UCNV_UTF7,

    శూన్యసూచిక,
    శూన్యసూచిక,

    _UTF7Open,
    శూన్యసూచిక,
    _UTF7Reset,

    UTF7_గమ్యం_Unicode_With_Offsets,
    UTF7_గమ్యం_Unicode_With_Offsets,
    _UTF7FromUnicodeWithOffsets,
    _UTF7FromUnicodeWithOffsets,
    శూన్యసూచిక,

    శూన్యసూచిక,
    _UTF7GetName,
    శూన్యసూచిక, /* we don't need writeSub() because we never call a callback at fromUnicode() */
    శూన్యసూచిక,
    ucnv_get_Complete_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};

స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం UTF7_Static_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "UTF-7",
    0, /* TODO CCSID for UTF-7 */
    UCNV_IBM, UCNV_UTF7,
    1, 4,
    { 0x3f, 0, 0, 0 }, 1, /* the subchar is not used */
    అబద్ధం, అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

స్థిరరాశి UConverter_Shared_దత్తాంశం UTF7_దత్తాంశం=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&UTF7_Static_దత్తాంశం, &_UTF7Impl);

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
#నిర్వచించు AMPERSAND 0x26
#నిర్వచించు COMMA 0x2c
#నిర్వచించు SLASH 0x2f

/* legal byte values: all US-ASCII graphic characters 0x20..0x7e */
#నిర్వచించు isLegalIMAP(c) (0x20<=(c) && (c)<=0x7e)

/* direct-encode all of printable ASCII 0x20..0x7e except '&' 0x26 */
#నిర్వచించు లోపల_సమితి_DIMAP(c) (isLegalIMAP(c) && c!=AMPERSAND)

#నిర్వచించు గమ్యం_BASE64_IMAP(n) ((n)<63 ? గమ్యం_Base64[n] : COMMA)
#నిర్వచించు FROM_BASE64_IMAP(c) ((c)==COMMA ? 63 : (c)==SLASH ? -1 : fromBase64[c])

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
స్థిర శూన్య U_CALLCONV
_IMAPToUnicodeWithOffsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                          UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    char16_t *లక్ష్యం;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు;
    int32_t *offsets;

    uint8_t *బైట్లు;
    uint8_t byte_సూచీ;

    int32_t length, లక్ష్యం_సామర్థ్యం;

    /* UTF-7 state */
    uint16_t బిట్లు;
    int8_t base64Counter;
    UBool లోపల_ప్రత్యక్ష_బహుళకం;

    int8_t base64_విలువ;

    int32_t ఆధారం_సూచీ, తదుపరి_ఆధారం_సూచీ;

    char16_t c;
    uint8_t b;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;

    ఆధారం=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    లక్ష్యం_హద్దు=p_వాదనలు->లక్ష్యం_హద్దు;
    offsets=p_వాదనలు->offsets;
    /* get the state machine state */
    {
        uint32_t స్థితి_2=cnv->గమ్యం_Unicode_స్థితి;
        లోపల_ప్రత్యక్ష_బహుళకం=(UBool)((స్థితి_2>>24)&1);
        base64Counter=(int8_t)(స్థితి_2>>16);
        బిట్లు=(uint16_t)స్థితి_2;
    }
    బైట్లు=cnv->గమ్యం_UBytes;
    byte_సూచీ=cnv->గమ్యం_ULength;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ=byte_సూచీ==0 ? 0 : -1;
    తదుపరి_ఆధారం_సూచీ=0;

    ఒకవేళ(లోపల_ప్రత్యక్ష_బహుళకం) {
ప్రత్యక్ష_బహుళకం:
        /*
         * In Direct Mode, US-ASCII characters are encoded directly, i.e.,
         * with their US-ASCII byte values.
         * An ampersand starts Unicode (or "escape") Mode.
         *
         * In Direct Mode, only the sourceIndex is used.
         */
        byte_సూచీ=0;
        length=(int32_t)(ఆధారం_హద్దు-ఆధారం);
        లక్ష్యం_సామర్థ్యం=(int32_t)(లక్ష్యం_హద్దు-లక్ష్యం);
        ఒకవేళ(length>లక్ష్యం_సామర్థ్యం) {
            length=లక్ష్యం_సామర్థ్యం;
        }
        అయితే(length>0) {
            b=*ఆధారం++;
            ఒకవేళ(!isLegalIMAP(b)) {
                /* illegal */
                బైట్లు[0]=b;
                byte_సూచీ=1;
                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                ఆపు;
            } లేకపోతే ఒకవేళ(b!=AMPERSAND) {
                /* write directly encoded character */
                *లక్ష్యం++=b;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ++;
                }
            } లేకపోతే /* AMPERSAND */ {
                /* switch to Unicode mode */
                తదుపరి_ఆధారం_సూచీ=++ఆధారం_సూచీ;
                లోపల_ప్రత్యక్ష_బహుళకం=అబద్ధం;
                byte_సూచీ=0;
                బిట్లు=0;
                base64Counter=-1;
                వెళ్ళు unicode_బహుళకం;
            }
            --length;
        }
        ఒకవేళ(ఆధారం<ఆధారం_హద్దు && లక్ష్యం>=లక్ష్యం_హద్దు) {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        }
    } లేకపోతే {
unicode_బహుళకం:
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
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                బైట్లు[byte_సూచీ++]=b=*ఆధారం++;
                ++తదుపరి_ఆధారం_సూచీ;
                ఒకవేళ(b>0x7e) {
                    /* illegal - test other illegal US-ASCII values by base64Value==-3 */
                    లోపల_ప్రత్యక్ష_బహుళకం=నిజం;
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    ఆపు;
                } లేకపోతే ఒకవేళ((base64_విలువ=FROM_BASE64_IMAP(b))>=0) {
                    /* collect base64 bytes into UChars */
                    ఎంపిక(base64Counter) {
                    స్థితి -1: /* -1 is immediately after the & */
                    స్థితి 0:
                        బిట్లు=base64_విలువ;
                        base64Counter=1;
                        ఆపు;
                    స్థితి 1:
                    స్థితి 3:
                    స్థితి 4:
                    స్థితి 6:
                        బిట్లు=(uint16_t)((బిట్లు<<6)|base64_విలువ);
                        ++base64Counter;
                        ఆపు;
                    స్థితి 2:
                        c=(char16_t)((బిట్లు<<4)|(base64_విలువ>>2));
                        ఒకవేళ(isLegalIMAP(c)) {
                            /* illegal */
                            లోపల_ప్రత్యక్ష_బహుళకం=నిజం;
                            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                            వెళ్ళు endloop;
                        }
                        *లక్ష్యం++=c;
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=ఆధారం_సూచీ;
                            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ-1;
                        }
                        బైట్లు[0]=b; /* keep this byte in case an error occurs */
                        byte_సూచీ=1;
                        బిట్లు=(uint16_t)(base64_విలువ&3);
                        base64Counter=3;
                        ఆపు;
                    స్థితి 5:
                        c=(char16_t)((బిట్లు<<2)|(base64_విలువ>>4));
                        ఒకవేళ(isLegalIMAP(c)) {
                            /* illegal */
                            లోపల_ప్రత్యక్ష_బహుళకం=నిజం;
                            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                            వెళ్ళు endloop;
                        }
                        *లక్ష్యం++=c;
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=ఆధారం_సూచీ;
                            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ-1;
                        }
                        బైట్లు[0]=b; /* keep this byte in case an error occurs */
                        byte_సూచీ=1;
                        బిట్లు=(uint16_t)(base64_విలువ&15);
                        base64Counter=6;
                        ఆపు;
                    స్థితి 7:
                        c=(char16_t)((బిట్లు<<6)|base64_విలువ);
                        ఒకవేళ(isLegalIMAP(c)) {
                            /* illegal */
                            లోపల_ప్రత్యక్ష_బహుళకం=నిజం;
                            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                            వెళ్ళు endloop;
                        }
                        *లక్ష్యం++=c;
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=ఆధారం_సూచీ;
                            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                        }
                        byte_సూచీ=0;
                        బిట్లు=0;
                        base64Counter=0;
                        ఆపు;
                    అప్రమేయ:
                        /* will never occur */
                        ఆపు;
                    }
                } లేకపోతే ఒకవేళ(base64_విలువ==-2) {
                    /* minus sign terminates the base64 sequence */
                    లోపల_ప్రత్యక్ష_బహుళకం=నిజం;
                    ఒకవేళ(base64Counter==-1) {
                        /* &- i.e. a minus immediately following an ampersand */
                        *లక్ష్యం++=AMPERSAND;
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=ఆధారం_సూచీ-1;
                        }
                    } లేకపోతే {
                        /* absorb the minus and leave the Unicode Mode */
                        ఒకవేళ(బిట్లు!=0 || (base64Counter!=0 && base64Counter!=3 && base64Counter!=6)) {
                            /* bits are illegally left over, a char16_t is incomplete */
                            /* base64Counter other than 0, 3, 6 means non-minimal zero-padding, also illegal */
                            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                            ఆపు;
                        }
                    }
                    ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                    వెళ్ళు ప్రత్యక్ష_బహుళకం;
                } లేకపోతే {
                    ఒకవేళ(base64Counter==-1) {
                        /* illegal: & immediately followed by something other than base64 or minus sign */
                        /* include the ampersand in the reported sequence */
                        --ఆధారం_సూచీ;
                        బైట్లు[0]=AMPERSAND;
                        బైట్లు[1]=b;
                        byte_సూచీ=2;
                    }
                    /* base64Value==-1 for characters that are illegal only in Unicode mode */
                    /* base64Value==-3 for illegal characters */
                    /* illegal */
                    లోపల_ప్రత్యక్ష_బహుళకం=నిజం;
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    ఆపు;
                }
            } లేకపోతే {
                /* target is full */
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
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
    ఒకవేళ( U_SUCCESS(*p_Error_సంకేతం) &&
        !లోపల_ప్రత్యక్ష_బహుళకం && byte_సూచీ==0 &&
        p_వాదనలు->flush && ఆధారం>=ఆధారం_హద్దు
    ) {
        ఒకవేళ(base64Counter==-1) {
            /* & at the very end of the input */
            /* make the ampersand the reported sequence */
            బైట్లు[0]=AMPERSAND;
            byte_సూచీ=1;
        }
        /* else if(base64Counter!=-1) byteIndex remains 0 because there is no particular byte sequence */

        లోపల_ప్రత్యక్ష_బహుళకం=నిజం; /* avoid looping */
        *p_Error_సంకేతం=U_TRUNCATED_CHAR_దొరికింది;
    }

    /* set the converter state back into UConverter */
    cnv->గమ్యం_Unicode_స్థితి=((uint32_t)లోపల_ప్రత్యక్ష_బహుళకం<<24)|((uint32_t)((uint8_t)base64Counter)<<16)|(uint32_t)బిట్లు;
    cnv->గమ్యం_ULength=byte_సూచీ;

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
    p_వాదనలు->లక్ష్యం=లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

స్థిర శూన్య U_CALLCONV
_IMAPFromUnicodeWithOffsets(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
                            UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి char16_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం, *లక్ష్యం_హద్దు;
    int32_t *offsets;

    int32_t length, లక్ష్యం_సామర్థ్యం, ఆధారం_సూచీ;
    char16_t c;
    uint8_t b;

    /* UTF-7 state */
    uint8_t బిట్లు;
    int8_t base64Counter;
    UBool లోపల_ప్రత్యక్ష_బహుళకం;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;

    /* set up the local pointers */
    ఆధారం=p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం=(uint8_t *)p_వాదనలు->లక్ష్యం;
    లక్ష్యం_హద్దు=(uint8_t *)p_వాదనలు->లక్ష్యం_హద్దు;
    offsets=p_వాదనలు->offsets;

    /* get the state machine state */
    {
        uint32_t స్థితి_2=cnv->from_Unicode_స్థితి;
        లోపల_ప్రత్యక్ష_బహుళకం=(UBool)((స్థితి_2>>24)&1);
        base64Counter=(int8_t)(స్థితి_2>>16);
        బిట్లు=(uint8_t)స్థితి_2;
    }

    /* UTF-7 always encodes UTF-16 code units, therefore we need only a simple sourceIndex */
    ఆధారం_సూచీ=0;

    ఒకవేళ(లోపల_ప్రత్యక్ష_బహుళకం) {
ప్రత్యక్ష_బహుళకం:
        length=(int32_t)(ఆధారం_హద్దు-ఆధారం);
        లక్ష్యం_సామర్థ్యం=(int32_t)(లక్ష్యం_హద్దు-లక్ష్యం);
        ఒకవేళ(length>లక్ష్యం_సామర్థ్యం) {
            length=లక్ష్యం_సామర్థ్యం;
        }
        అయితే(length>0) {
            c=*ఆధారం++;
            /* encode 0x20..0x7e except '&' directly */
            ఒకవేళ(లోపల_సమితి_DIMAP(c)) {
                /* encode directly */
                *లక్ష్యం++=(uint8_t)c;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ++;
                }
            } లేకపోతే ఒకవేళ(c==AMPERSAND) {
                /* output &- for & */
                *లక్ష్యం++=AMPERSAND;
                ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                    *లక్ష్యం++=MINUS;
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                        *offsets++=ఆధారం_సూచీ++;
                    }
                    /* realign length and targetCapacity */
                    వెళ్ళు ప్రత్యక్ష_బహుళకం;
                } లేకపోతే {
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ++;
                    }
                    cnv->char_Error_బఫరు[0]=MINUS;
                    cnv->char_Error_బఫరు_నిడివి=1;
                    *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                    ఆపు;
                }
            } లేకపోతే {
                /* un-read this character and switch to Unicode Mode */
                --ఆధారం;
                *లక్ష్యం++=AMPERSAND;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                లోపల_ప్రత్యక్ష_బహుళకం=అబద్ధం;
                base64Counter=0;
                వెళ్ళు unicode_బహుళకం;
            }
            --length;
        }
        ఒకవేళ(ఆధారం<ఆధారం_హద్దు && లక్ష్యం>=లక్ష్యం_హద్దు) {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        }
    } లేకపోతే {
unicode_బహుళకం:
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                c=*ఆధారం++;
                ఒకవేళ(isLegalIMAP(c)) {
                    /* encode directly */
                    లోపల_ప్రత్యక్ష_బహుళకం=నిజం;

                    /* trick: back out this character to make this easier */
                    --ఆధారం;

                    /* terminate the base64 sequence */
                    ఒకవేళ(base64Counter!=0) {
                        /* write remaining bits for the previous character */
                        *లక్ష్యం++=గమ్యం_BASE64_IMAP(బిట్లు);
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=ఆధారం_సూచీ-1;
                        }
                    }
                    /* need to terminate with a minus */
                    ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                        *లక్ష్యం++=MINUS;
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=ఆధారం_సూచీ-1;
                        }
                    } లేకపోతే {
                        cnv->char_Error_బఫరు[0]=MINUS;
                        cnv->char_Error_బఫరు_నిడివి=1;
                        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                        ఆపు;
                    }
                    వెళ్ళు ప్రత్యక్ష_బహుళకం;
                } లేకపోతే {
                    /*
                     * base64 this character:
                     * Output 2 or 3 base64 bytes for the remaining bits of the previous character
                     * and the bits of this character, each implicitly in UTF-16BE.
                     *
                     * Here, bits is an 8-bit variable because only 6 bits need to be kept from one
                     * character to the next. The actual 2 or 4 bits are shifted to the left edge
                     * of the 6-bits field 5..0 to make the termination of the base64 sequence easier.
                     */
                    ఎంపిక(base64Counter) {
                    స్థితి 0:
                        b=(uint8_t)(c>>10);
                        *లక్ష్యం++=గమ్యం_BASE64_IMAP(b);
                        ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                            b=(uint8_t)((c>>4)&0x3f);
                            *లక్ష్యం++=గమ్యం_BASE64_IMAP(b);
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ;
                                *offsets++=ఆధారం_సూచీ++;
                            }
                        } లేకపోతే {
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ++;
                            }
                            b=(uint8_t)((c>>4)&0x3f);
                            cnv->char_Error_బఫరు[0]=గమ్యం_BASE64_IMAP(b);
                            cnv->char_Error_బఫరు_నిడివి=1;
                            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                        }
                        బిట్లు=(uint8_t)((c&15)<<2);
                        base64Counter=1;
                        ఆపు;
                    స్థితి 1:
                        b=(uint8_t)(బిట్లు|(c>>14));
                        *లక్ష్యం++=గమ్యం_BASE64_IMAP(b);
                        ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                            b=(uint8_t)((c>>8)&0x3f);
                            *లక్ష్యం++=గమ్యం_BASE64_IMAP(b);
                            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                                b=(uint8_t)((c>>2)&0x3f);
                                *లక్ష్యం++=గమ్యం_BASE64_IMAP(b);
                                ఒకవేళ(offsets!=శూన్యసూచిక) {
                                    *offsets++=ఆధారం_సూచీ;
                                    *offsets++=ఆధారం_సూచీ;
                                    *offsets++=ఆధారం_సూచీ++;
                                }
                            } లేకపోతే {
                                ఒకవేళ(offsets!=శూన్యసూచిక) {
                                    *offsets++=ఆధారం_సూచీ;
                                    *offsets++=ఆధారం_సూచీ++;
                                }
                                b=(uint8_t)((c>>2)&0x3f);
                                cnv->char_Error_బఫరు[0]=గమ్యం_BASE64_IMAP(b);
                                cnv->char_Error_బఫరు_నిడివి=1;
                                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                            }
                        } లేకపోతే {
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ++;
                            }
                            b=(uint8_t)((c>>8)&0x3f);
                            cnv->char_Error_బఫరు[0]=గమ్యం_BASE64_IMAP(b);
                            b=(uint8_t)((c>>2)&0x3f);
                            cnv->char_Error_బఫరు[1]=గమ్యం_BASE64_IMAP(b);
                            cnv->char_Error_బఫరు_నిడివి=2;
                            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                        }
                        బిట్లు=(uint8_t)((c&3)<<4);
                        base64Counter=2;
                        ఆపు;
                    స్థితి 2:
                        b=(uint8_t)(బిట్లు|(c>>12));
                        *లక్ష్యం++=గమ్యం_BASE64_IMAP(b);
                        ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                            b=(uint8_t)((c>>6)&0x3f);
                            *లక్ష్యం++=గమ్యం_BASE64_IMAP(b);
                            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                                b=(uint8_t)(c&0x3f);
                                *లక్ష్యం++=గమ్యం_BASE64_IMAP(b);
                                ఒకవేళ(offsets!=శూన్యసూచిక) {
                                    *offsets++=ఆధారం_సూచీ;
                                    *offsets++=ఆధారం_సూచీ;
                                    *offsets++=ఆధారం_సూచీ++;
                                }
                            } లేకపోతే {
                                ఒకవేళ(offsets!=శూన్యసూచిక) {
                                    *offsets++=ఆధారం_సూచీ;
                                    *offsets++=ఆధారం_సూచీ++;
                                }
                                b=(uint8_t)(c&0x3f);
                                cnv->char_Error_బఫరు[0]=గమ్యం_BASE64_IMAP(b);
                                cnv->char_Error_బఫరు_నిడివి=1;
                                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                            }
                        } లేకపోతే {
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ++;
                            }
                            b=(uint8_t)((c>>6)&0x3f);
                            cnv->char_Error_బఫరు[0]=గమ్యం_BASE64_IMAP(b);
                            b=(uint8_t)(c&0x3f);
                            cnv->char_Error_బఫరు[1]=గమ్యం_BASE64_IMAP(b);
                            cnv->char_Error_బఫరు_నిడివి=2;
                            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                        }
                        బిట్లు=0;
                        base64Counter=0;
                        ఆపు;
                    అప్రమేయ:
                        /* will never occur */
                        ఆపు;
                    }
                }
            } లేకపోతే {
                /* target is full */
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }
        }
    }

    ఒకవేళ(p_వాదనలు->flush && ఆధారం>=ఆధారం_హద్దు) {
        /* flush remaining bits to the target */
        ఒకవేళ(!లోపల_ప్రత్యక్ష_బహుళకం) {
            ఒకవేళ(base64Counter!=0) {
                ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                    *లక్ష్యం++=గమ్యం_BASE64_IMAP(బిట్లు);
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ-1;
                    }
                } లేకపోతే {
                    cnv->char_Error_బఫరు[cnv->char_Error_బఫరు_నిడివి++]=గమ్యం_BASE64_IMAP(బిట్లు);
                    *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                }
            }
            /* need to terminate with a minus */
            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                *లక్ష్యం++=MINUS;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ-1;
                }
            } లేకపోతే {
                cnv->char_Error_బఫరు[cnv->char_Error_బఫరు_నిడివి++]=MINUS;
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            }
        }
        /* reset the state for the next conversion */
        cnv->from_Unicode_స్థితి=(cnv->from_Unicode_స్థితి&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    } లేకపోతే {
        /* set the converter state back into UConverter */
        cnv->from_Unicode_స్థితి=
            (cnv->from_Unicode_స్థితి&0xf0000000)|    /* keep version*/
            ((uint32_t)లోపల_ప్రత్యక్ష_బహుళకం<<24)|((uint32_t)base64Counter<<16)|(uint32_t)బిట్లు;
    }

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=ఆధారం;
    p_వాదనలు->లక్ష్యం=(అక్షరం *)లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}
U_CDECL_చివర

స్థిర స్థిరరాశి UConverterImpl _IMAPImpl={
    UCNV_IMAP_MAILBOX,

    శూన్యసూచిక,
    శూన్యసూచిక,

    _UTF7Open,
    శూన్యసూచిక,
    _UTF7Reset,

    _IMAPToUnicodeWithOffsets,
    _IMAPToUnicodeWithOffsets,
    _IMAPFromUnicodeWithOffsets,
    _IMAPFromUnicodeWithOffsets,
    శూన్యసూచిక,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక, /* we don't need writeSub() because we never call a callback at fromUnicode() */
    శూన్యసూచిక,
    ucnv_get_Complete_Unicode_సమితి,
    శూన్యసూచిక,
    శూన్యసూచిక
};

స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం IMAPStatic_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "IMAP-mailbox-name",
    0, /* TODO CCSID for IMAP-mailbox-name */
    UCNV_IBM, UCNV_IMAP_MAILBOX,
    1, 4,
    { 0x3f, 0, 0, 0 }, 1, /* the subchar is not used */
    అబద్ధం, అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

స్థిరరాశి UConverter_Shared_దత్తాంశం _IMAPData=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&IMAPStatic_దత్తాంశం, &_IMAPImpl);

#అయితే_ముగింపు
