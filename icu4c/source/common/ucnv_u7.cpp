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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#شامل "cmemory.h"
#شامل "unicode/ucnv.h"
#شامل "ucnv_bld.h"
#شامل "ucnv_cnv.h"
#شامل "uassert.h"

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
#تعریف_کرو اندر_سیٹ_D(c) \
    ((uint8_t)((c)-97)<26 || (uint8_t)((c)-65)<26 || /* letters */ \
     (uint8_t)((c)-48)<10 ||    /* digits */ \
     (uint8_t)((c)-39)<3 ||     /* '() */ \
     (uint8_t)((c)-44)<4 ||     /* ,-./ */ \
     (c)==58 || (c)==63         /* :? */ \
    )

#تعریف_کرو اندر_سیٹ_O(c) \
    ((uint8_t)((c)-33)<6 ||         /* !"#$%& */ \
     (uint8_t)((c)-59)<4 ||         /* ;<=> */ \
     (uint8_t)((c)-93)<4 ||         /* ]^_` */ \
     (uint8_t)((c)-123)<3 ||        /* {|} */ \
     (c)==42 || (c)==64 || (c)==91  /* *@[ */ \
    )

#تعریف_کرو isCRLFTAB(c) ((c)==13 || (c)==10 || (c)==9)
#تعریف_کرو isCRLFSPTAB(c) ((c)==32 || (c)==13 || (c)==10 || (c)==9)

#تعریف_کرو PLUS  43
#تعریف_کرو MINUS 45
#تعریف_کرو BACKSLASH 92
#تعریف_کرو TILDE 126

/* legal byte values: all US-ASCII graphic characters from space to before tilde, and CR LF TAB */
#تعریف_کرو isLegalUTF7(c) (((uint8_t)((c)-32)<94 && (c)!=BACKSLASH) || isCRLFTAB(c))

/* encode directly sets D and O and CR LF SP TAB */
ساکن ثابت UBool encode_Directly_زیادہ_سے_زیادہ[128]={
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
ساکن ثابت UBool encodeDirectlyRestricted[128]={
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

ساکن ثابت uint8_t
مقصد_Base64[64]={
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

ساکن ثابت int8_t
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
ساکن خالی U_CALLCONV
_UTF7Reset(UConverter *cnv, UConverterResetChoice choice) {
    اگر(choice<=UCNV_RESET_مقصد_UNICODE) {
        /* reset toUnicode */
        cnv->مقصد_Unicode_حالت=0x1000000; /* inDirectMode=true */
        cnv->مقصد_ULength=0;
    }
    اگر(choice!=UCNV_RESET_مقصد_UNICODE) {
        /* reset fromUnicode */
        cnv->from_Unicode_حالت=(cnv->from_Unicode_حالت&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    }
}

ساکن خالی U_CALLCONV
_UTF7Open(UConverter *cnv,
          UConverter_بھرو_دلائل *p_دلائل,
          UError_رمز *p_Error_رمز) {
    (خالی)p_دلائل;
    اگر(UCNV_GET_نسخہ(cnv)<=1) {
        /* TODO(markus): Should just use cnv->options rather than copying the version number. */
        cnv->from_Unicode_حالت=UCNV_GET_نسخہ(cnv)<<28;
        _UTF7Reset(cnv, UCNV_RESET_BOTH);
    } ورنہ {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
    }
}

ساکن خالی U_CALLCONV
UTF7_مقصد_Unicode_With_Offsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                          UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    char16_t *ہدف;
    ثابت char16_t *ہدف_حد_بندی;
    int32_t *offsets;

    uint8_t *بائٹیں;
    uint8_t byte_اشاریہ;

    int32_t length, ہدف_استعداد;

    /* UTF-7 state */
    uint16_t بٹ;
    int8_t base64_شمار;
    UBool اندر_براہ_راست_منوال;

    int8_t base64_قدر;

    int32_t ماخذ_اشاریہ, اگلا_ماخذ_اشاریہ;

    uint8_t b;
    /* set up the local pointers */
    cnv=p_دلائل->converter;

    ماخذ=(ثابت uint8_t *)p_دلائل->ماخذ;
    ماخذ_حد_بندی=(ثابت uint8_t *)p_دلائل->ماخذ_حد_بندی;
    ہدف=p_دلائل->ہدف;
    ہدف_حد_بندی=p_دلائل->ہدف_حد_بندی;
    offsets=p_دلائل->offsets;
    /* get the state machine state */
    {
        uint32_t حالت_2=cnv->مقصد_Unicode_حالت;
        اندر_براہ_راست_منوال=(UBool)((حالت_2>>24)&1);
        base64_شمار=(int8_t)(حالت_2>>16);
        بٹ=(uint16_t)حالت_2;
    }
    بائٹیں=cnv->مقصد_UBytes;
    byte_اشاریہ=cnv->مقصد_ULength;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ=byte_اشاریہ==0 ? 0 : -1;
    اگلا_ماخذ_اشاریہ=0;

    اگر(اندر_براہ_راست_منوال) {
براہ_راست_منوال:
        /*
         * In Direct Mode, most US-ASCII characters are encoded directly, i.e.,
         * with their US-ASCII byte values.
         * Backslash and Tilde and most control characters are not allowed in UTF-7.
         * A plus sign starts Unicode (or "escape") Mode.
         *
         * In Direct Mode, only the sourceIndex is used.
         */
        byte_اشاریہ=0;
        length=(int32_t)(ماخذ_حد_بندی-ماخذ);
        ہدف_استعداد=(int32_t)(ہدف_حد_بندی-ہدف);
        اگر(length>ہدف_استعداد) {
            length=ہدف_استعداد;
        }
        جب(length>0) {
            b=*ماخذ++;
            اگر(!isLegalUTF7(b)) {
                /* illegal */
                بائٹیں[0]=b;
                byte_اشاریہ=1;
                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                توڑو;
            } ورنہ اگر(b!=PLUS) {
                /* write directly encoded character */
                *ہدف++=b;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ++;
                }
            } ورنہ /* PLUS */ {
                /* switch to Unicode mode */
                اگلا_ماخذ_اشاریہ=++ماخذ_اشاریہ;
                اندر_براہ_راست_منوال=جھوٹ;
                byte_اشاریہ=0;
                بٹ=0;
                base64_شمار=-1;
                جاؤ unicode_منوال;
            }
            --length;
        }
        اگر(ماخذ<ماخذ_حد_بندی && ہدف>=ہدف_حد_بندی) {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        }
    } ورنہ {
unicode_منوال:
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
        جب(ماخذ<ماخذ_حد_بندی) {
            اگر(ہدف<ہدف_حد_بندی) {
                بائٹیں[byte_اشاریہ++]=b=*ماخذ++;
                ++اگلا_ماخذ_اشاریہ;
                base64_قدر = -3; /* initialize as illegal */
                اگر(b>=126 || (base64_قدر=fromBase64[b])==-3 || base64_قدر==-1) {
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
                    اندر_براہ_راست_منوال=سچ;
                    اگر(base64_شمار==-1) {
                        /* illegal: + immediately followed by something other than base64 or minus sign */
                        /* include the plus sign in the reported sequence, but not the subsequent char */
                        --ماخذ;
                        بائٹیں[0]=PLUS;
                        byte_اشاریہ=1;
                        *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                        توڑو;
                    } ورنہ اگر(بٹ!=0) {
                        /* bits are illegally left over, a char16_t is incomplete */
                        /* don't include current char (legal or illegal) in error seq */
                        --ماخذ;
                        --byte_اشاریہ;
                        *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                        توڑو;
                    } ورنہ {
                        /* previous char16_t was complete */
                        اگر(base64_قدر==-3) {
                            /* current character is illegal, deal with it here */
                            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                            توڑو;
                        } ورنہ {
                            /* un-read the current character in case it is a plus sign */
                            --ماخذ;
                            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ-1;
                            جاؤ براہ_راست_منوال;
                        }
                    }
                } ورنہ اگر(base64_قدر>=0) {
                    /* collect base64 bytes into UChars */
                    انتخاب(base64_شمار) {
                    حالت -1: /* -1 is immediately after the + */
                    حالت 0:
                        بٹ=base64_قدر;
                        base64_شمار=1;
                        توڑو;
                    حالت 1:
                    حالت 3:
                    حالت 4:
                    حالت 6:
                        بٹ=(uint16_t)((بٹ<<6)|base64_قدر);
                        ++base64_شمار;
                        توڑو;
                    حالت 2:
                        *ہدف++=(char16_t)((بٹ<<4)|(base64_قدر>>2));
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=ماخذ_اشاریہ;
                            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ-1;
                        }
                        بائٹیں[0]=b; /* keep this byte in case an error occurs */
                        byte_اشاریہ=1;
                        بٹ=(uint16_t)(base64_قدر&3);
                        base64_شمار=3;
                        توڑو;
                    حالت 5:
                        *ہدف++=(char16_t)((بٹ<<2)|(base64_قدر>>4));
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=ماخذ_اشاریہ;
                            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ-1;
                        }
                        بائٹیں[0]=b; /* keep this byte in case an error occurs */
                        byte_اشاریہ=1;
                        بٹ=(uint16_t)(base64_قدر&15);
                        base64_شمار=6;
                        توڑو;
                    حالت 7:
                        *ہدف++=(char16_t)((بٹ<<6)|base64_قدر);
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=ماخذ_اشاریہ;
                            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                        }
                        byte_اشاریہ=0;
                        بٹ=0;
                        base64_شمار=0;
                        توڑو;
                    طےشدہ:
                        /* will never occur */
                        توڑو;
                    }
                } ورنہ /*base64Value==-2*/ {
                    /* minus sign terminates the base64 sequence */
                    اندر_براہ_راست_منوال=سچ;
                    اگر(base64_شمار==-1) {
                        /* +- i.e. a minus immediately following a plus */
                        *ہدف++=PLUS;
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=ماخذ_اشاریہ-1;
                        }
                    } ورنہ {
                        /* absorb the minus and leave the Unicode Mode */
                        اگر(بٹ!=0) {
                            /* bits are illegally left over, a char16_t is incomplete */
                            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                            توڑو;
                        }
                    }
                    ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                    جاؤ براہ_راست_منوال;
                }
            } ورنہ {
                /* target is full */
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }
        }
    }

    اگر(U_SUCCESS(*p_Error_رمز) && p_دلائل->flush && ماخذ==ماخذ_حد_بندی && بٹ==0) {
        /*
         * if we are in Unicode mode, then the byteIndex might not be 0,
         * but that is ok if bits==0
         * -> we set byteIndex=0 at the end of the stream to avoid a truncated error
         * (not true for IMAP-mailbox-name where we must end in direct mode)
         */
        byte_اشاریہ=0;
    }

    /* set the converter state back into UConverter */
    cnv->مقصد_Unicode_حالت=((uint32_t)اندر_براہ_راست_منوال<<24)|((uint32_t)((uint8_t)base64_شمار)<<16)|(uint32_t)بٹ;
    cnv->مقصد_ULength=byte_اشاریہ;

    /* write back the updated pointers */
    p_دلائل->ماخذ=(ثابت حرف *)ماخذ;
    p_دلائل->ہدف=ہدف;
    p_دلائل->offsets=offsets;
}

ساکن خالی U_CALLCONV
_UTF7FromUnicodeWithOffsets(UConverter_From_Unicode_دلائل *p_دلائل,
                            UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت char16_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف, *ہدف_حد_بندی;
    int32_t *offsets;

    int32_t length, ہدف_استعداد, ماخذ_اشاریہ;
    char16_t c;

    /* UTF-7 state */
    ثابت UBool *encodeDirectly;
    uint8_t بٹ;
    int8_t base64_شمار;
    UBool اندر_براہ_راست_منوال;

    /* set up the local pointers */
    cnv=p_دلائل->converter;

    /* set up the local pointers */
    ماخذ=p_دلائل->ماخذ;
    ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
    ہدف=(uint8_t *)p_دلائل->ہدف;
    ہدف_حد_بندی=(uint8_t *)p_دلائل->ہدف_حد_بندی;
    offsets=p_دلائل->offsets;

    /* get the state machine state */
    {
        uint32_t حالت_2=cnv->from_Unicode_حالت;
        encodeDirectly= حالت_2<0x10000000 ? encode_Directly_زیادہ_سے_زیادہ : encodeDirectlyRestricted;
        اندر_براہ_راست_منوال=(UBool)((حالت_2>>24)&1);
        base64_شمار=(int8_t)(حالت_2>>16);
        بٹ=(uint8_t)حالت_2;
        U_ASSERT(بٹ<=UPRV_LENGTHOF(مقصد_Base64));
    }

    /* UTF-7 always encodes UTF-16 code units, therefore we need only a simple sourceIndex */
    ماخذ_اشاریہ=0;

    اگر(اندر_براہ_راست_منوال) {
براہ_راست_منوال:
        length=(int32_t)(ماخذ_حد_بندی-ماخذ);
        ہدف_استعداد=(int32_t)(ہدف_حد_بندی-ہدف);
        اگر(length>ہدف_استعداد) {
            length=ہدف_استعداد;
        }
        جب(length>0) {
            c=*ماخذ++;
            /* currently always encode CR LF SP TAB directly */
            اگر(c<=127 && encodeDirectly[c]) {
                /* encode directly */
                *ہدف++=(uint8_t)c;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ++;
                }
            } ورنہ اگر(c==PLUS) {
                /* output +- for + */
                *ہدف++=PLUS;
                اگر(ہدف<ہدف_حد_بندی) {
                    *ہدف++=MINUS;
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                        *offsets++=ماخذ_اشاریہ++;
                    }
                    /* realign length and targetCapacity */
                    جاؤ براہ_راست_منوال;
                } ورنہ {
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ++;
                    }
                    cnv->char_Error_بفر[0]=MINUS;
                    cnv->char_Error_بفر_لمبائی=1;
                    *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                    توڑو;
                }
            } ورنہ {
                /* un-read this character and switch to Unicode Mode */
                --ماخذ;
                *ہدف++=PLUS;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                اندر_براہ_راست_منوال=جھوٹ;
                base64_شمار=0;
                جاؤ unicode_منوال;
            }
            --length;
        }
        اگر(ماخذ<ماخذ_حد_بندی && ہدف>=ہدف_حد_بندی) {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        }
    } ورنہ {
unicode_منوال:
        جب(ماخذ<ماخذ_حد_بندی) {
            اگر(ہدف<ہدف_حد_بندی) {
                c=*ماخذ++;
                اگر(c<=127 && encodeDirectly[c]) {
                    /* encode directly */
                    اندر_براہ_راست_منوال=سچ;

                    /* trick: back out this character to make this easier */
                    --ماخذ;

                    /* terminate the base64 sequence */
                    اگر(base64_شمار!=0) {
                        /* write remaining bits for the previous character */
                        *ہدف++=مقصد_Base64[بٹ];
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=ماخذ_اشاریہ-1;
                        }
                    }
                    اگر(fromBase64[c]!=-1) {
                        /* need to terminate with a minus */
                        اگر(ہدف<ہدف_حد_بندی) {
                            *ہدف++=MINUS;
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ-1;
                            }
                        } ورنہ {
                            cnv->char_Error_بفر[0]=MINUS;
                            cnv->char_Error_بفر_لمبائی=1;
                            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                            توڑو;
                        }
                    }
                    جاؤ براہ_راست_منوال;
                } ورنہ {
                    /*
                     * base64 this character:
                     * Output 2 or 3 base64 bytes for the remaining bits of the previous character
                     * and the bits of this character, each implicitly in UTF-16BE.
                     *
                     * Here, bits is an 8-bit variable because only 6 bits need to be kept from one
                     * character to the next. The actual 2 or 4 bits are shifted to the left edge
                     * of the 6-bits field 5..0 to make the termination of the base64 sequence easier.
                     */
                    انتخاب(base64_شمار) {
                    حالت 0:
                        *ہدف++=مقصد_Base64[c>>10];
                        اگر(ہدف<ہدف_حد_بندی) {
                            *ہدف++=مقصد_Base64[(c>>4)&0x3f];
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ;
                                *offsets++=ماخذ_اشاریہ++;
                            }
                        } ورنہ {
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ++;
                            }
                            cnv->char_Error_بفر[0]=مقصد_Base64[(c>>4)&0x3f];
                            cnv->char_Error_بفر_لمبائی=1;
                            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                        }
                        بٹ=(uint8_t)((c&15)<<2);
                        base64_شمار=1;
                        توڑو;
                    حالت 1:
                        *ہدف++=مقصد_Base64[بٹ|(c>>14)];
                        اگر(ہدف<ہدف_حد_بندی) {
                            *ہدف++=مقصد_Base64[(c>>8)&0x3f];
                            اگر(ہدف<ہدف_حد_بندی) {
                                *ہدف++=مقصد_Base64[(c>>2)&0x3f];
                                اگر(offsets!=خالی_اشاریہ) {
                                    *offsets++=ماخذ_اشاریہ;
                                    *offsets++=ماخذ_اشاریہ;
                                    *offsets++=ماخذ_اشاریہ++;
                                }
                            } ورنہ {
                                اگر(offsets!=خالی_اشاریہ) {
                                    *offsets++=ماخذ_اشاریہ;
                                    *offsets++=ماخذ_اشاریہ++;
                                }
                                cnv->char_Error_بفر[0]=مقصد_Base64[(c>>2)&0x3f];
                                cnv->char_Error_بفر_لمبائی=1;
                                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                            }
                        } ورنہ {
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ++;
                            }
                            cnv->char_Error_بفر[0]=مقصد_Base64[(c>>8)&0x3f];
                            cnv->char_Error_بفر[1]=مقصد_Base64[(c>>2)&0x3f];
                            cnv->char_Error_بفر_لمبائی=2;
                            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                        }
                        بٹ=(uint8_t)((c&3)<<4);
                        base64_شمار=2;
                        توڑو;
                    حالت 2:
                        *ہدف++=مقصد_Base64[بٹ|(c>>12)];
                        اگر(ہدف<ہدف_حد_بندی) {
                            *ہدف++=مقصد_Base64[(c>>6)&0x3f];
                            اگر(ہدف<ہدف_حد_بندی) {
                                *ہدف++=مقصد_Base64[c&0x3f];
                                اگر(offsets!=خالی_اشاریہ) {
                                    *offsets++=ماخذ_اشاریہ;
                                    *offsets++=ماخذ_اشاریہ;
                                    *offsets++=ماخذ_اشاریہ++;
                                }
                            } ورنہ {
                                اگر(offsets!=خالی_اشاریہ) {
                                    *offsets++=ماخذ_اشاریہ;
                                    *offsets++=ماخذ_اشاریہ++;
                                }
                                cnv->char_Error_بفر[0]=مقصد_Base64[c&0x3f];
                                cnv->char_Error_بفر_لمبائی=1;
                                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                            }
                        } ورنہ {
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ++;
                            }
                            cnv->char_Error_بفر[0]=مقصد_Base64[(c>>6)&0x3f];
                            cnv->char_Error_بفر[1]=مقصد_Base64[c&0x3f];
                            cnv->char_Error_بفر_لمبائی=2;
                            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                        }
                        بٹ=0;
                        base64_شمار=0;
                        توڑو;
                    طےشدہ:
                        /* will never occur */
                        توڑو;
                    }
                }
            } ورنہ {
                /* target is full */
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }
        }
    }

    اگر(p_دلائل->flush && ماخذ>=ماخذ_حد_بندی) {
        /* flush remaining bits to the target */
        اگر(!اندر_براہ_راست_منوال) {
            اگر (base64_شمار!=0) {
                اگر(ہدف<ہدف_حد_بندی) {
                    *ہدف++=مقصد_Base64[بٹ];
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ-1;
                    }
                } ورنہ {
                    cnv->char_Error_بفر[cnv->char_Error_بفر_لمبائی++]=مقصد_Base64[بٹ];
                    *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                }
            }
            /* Add final MINUS to terminate unicodeMode */
            اگر(ہدف<ہدف_حد_بندی) {
                *ہدف++=MINUS;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ-1;
                }
            } ورنہ {
                cnv->char_Error_بفر[cnv->char_Error_بفر_لمبائی++]=MINUS;
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            }
        }
        /* reset the state for the next conversion */
        cnv->from_Unicode_حالت=(cnv->from_Unicode_حالت&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    } ورنہ {
        /* set the converter state back into UConverter */
        cnv->from_Unicode_حالت=
            (cnv->from_Unicode_حالت&0xf0000000)|    /* keep version*/
            ((uint32_t)اندر_براہ_راست_منوال<<24)|((uint32_t)base64_شمار<<16)|(uint32_t)بٹ;
    }

    /* write back the updated pointers */
    p_دلائل->ماخذ=ماخذ;
    p_دلائل->ہدف=(حرف *)ہدف;
    p_دلائل->offsets=offsets;
}

ساکن ثابت حرف * U_CALLCONV
_UTF7GetName(ثابت UConverter *cnv) {
    انتخاب(cnv->from_Unicode_حالت>>28) {
    حالت 1:
        واپس "UTF-7,version=1";
    طےشدہ:
        واپس "UTF-7";
    }
}
U_CDECL_انتہا

ساکن ثابت UConverterImpl _UTF7Impl={
    UCNV_UTF7,

    خالی_اشاریہ,
    خالی_اشاریہ,

    _UTF7Open,
    خالی_اشاریہ,
    _UTF7Reset,

    UTF7_مقصد_Unicode_With_Offsets,
    UTF7_مقصد_Unicode_With_Offsets,
    _UTF7FromUnicodeWithOffsets,
    _UTF7FromUnicodeWithOffsets,
    خالی_اشاریہ,

    خالی_اشاریہ,
    _UTF7GetName,
    خالی_اشاریہ, /* we don't need writeSub() because we never call a callback at fromUnicode() */
    خالی_اشاریہ,
    ucnv_get_Complete_Unicode_سیٹ,

    خالی_اشاریہ,
    خالی_اشاریہ
};

ساکن ثابت UConverter_Static_ڈیٹا UTF7_Static_ڈیٹا={
    حجم(UConverter_Static_ڈیٹا),
    "UTF-7",
    0, /* TODO CCSID for UTF-7 */
    UCNV_IBM, UCNV_UTF7,
    1, 4,
    { 0x3f, 0, 0, 0 }, 1, /* the subchar is not used */
    جھوٹ, جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_ڈیٹا UTF7_ڈیٹا=
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&UTF7_Static_ڈیٹا, &_UTF7Impl);

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
#تعریف_کرو AMPERSAND 0x26
#تعریف_کرو COMMA 0x2c
#تعریف_کرو SLASH 0x2f

/* legal byte values: all US-ASCII graphic characters 0x20..0x7e */
#تعریف_کرو isLegalIMAP(c) (0x20<=(c) && (c)<=0x7e)

/* direct-encode all of printable ASCII 0x20..0x7e except '&' 0x26 */
#تعریف_کرو اندر_سیٹ_DIMAP(c) (isLegalIMAP(c) && c!=AMPERSAND)

#تعریف_کرو مقصد_BASE64_IMAP(n) ((n)<63 ? مقصد_Base64[n] : COMMA)
#تعریف_کرو FROM_BASE64_IMAP(c) ((c)==COMMA ? 63 : (c)==SLASH ? -1 : fromBase64[c])

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
ساکن خالی U_CALLCONV
_IMAPToUnicodeWithOffsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                          UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    char16_t *ہدف;
    ثابت char16_t *ہدف_حد_بندی;
    int32_t *offsets;

    uint8_t *بائٹیں;
    uint8_t byte_اشاریہ;

    int32_t length, ہدف_استعداد;

    /* UTF-7 state */
    uint16_t بٹ;
    int8_t base64_شمار;
    UBool اندر_براہ_راست_منوال;

    int8_t base64_قدر;

    int32_t ماخذ_اشاریہ, اگلا_ماخذ_اشاریہ;

    char16_t c;
    uint8_t b;

    /* set up the local pointers */
    cnv=p_دلائل->converter;

    ماخذ=(ثابت uint8_t *)p_دلائل->ماخذ;
    ماخذ_حد_بندی=(ثابت uint8_t *)p_دلائل->ماخذ_حد_بندی;
    ہدف=p_دلائل->ہدف;
    ہدف_حد_بندی=p_دلائل->ہدف_حد_بندی;
    offsets=p_دلائل->offsets;
    /* get the state machine state */
    {
        uint32_t حالت_2=cnv->مقصد_Unicode_حالت;
        اندر_براہ_راست_منوال=(UBool)((حالت_2>>24)&1);
        base64_شمار=(int8_t)(حالت_2>>16);
        بٹ=(uint16_t)حالت_2;
    }
    بائٹیں=cnv->مقصد_UBytes;
    byte_اشاریہ=cnv->مقصد_ULength;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ=byte_اشاریہ==0 ? 0 : -1;
    اگلا_ماخذ_اشاریہ=0;

    اگر(اندر_براہ_راست_منوال) {
براہ_راست_منوال:
        /*
         * In Direct Mode, US-ASCII characters are encoded directly, i.e.,
         * with their US-ASCII byte values.
         * An ampersand starts Unicode (or "escape") Mode.
         *
         * In Direct Mode, only the sourceIndex is used.
         */
        byte_اشاریہ=0;
        length=(int32_t)(ماخذ_حد_بندی-ماخذ);
        ہدف_استعداد=(int32_t)(ہدف_حد_بندی-ہدف);
        اگر(length>ہدف_استعداد) {
            length=ہدف_استعداد;
        }
        جب(length>0) {
            b=*ماخذ++;
            اگر(!isLegalIMAP(b)) {
                /* illegal */
                بائٹیں[0]=b;
                byte_اشاریہ=1;
                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                توڑو;
            } ورنہ اگر(b!=AMPERSAND) {
                /* write directly encoded character */
                *ہدف++=b;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ++;
                }
            } ورنہ /* AMPERSAND */ {
                /* switch to Unicode mode */
                اگلا_ماخذ_اشاریہ=++ماخذ_اشاریہ;
                اندر_براہ_راست_منوال=جھوٹ;
                byte_اشاریہ=0;
                بٹ=0;
                base64_شمار=-1;
                جاؤ unicode_منوال;
            }
            --length;
        }
        اگر(ماخذ<ماخذ_حد_بندی && ہدف>=ہدف_حد_بندی) {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        }
    } ورنہ {
unicode_منوال:
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
        جب(ماخذ<ماخذ_حد_بندی) {
            اگر(ہدف<ہدف_حد_بندی) {
                بائٹیں[byte_اشاریہ++]=b=*ماخذ++;
                ++اگلا_ماخذ_اشاریہ;
                اگر(b>0x7e) {
                    /* illegal - test other illegal US-ASCII values by base64Value==-3 */
                    اندر_براہ_راست_منوال=سچ;
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    توڑو;
                } ورنہ اگر((base64_قدر=FROM_BASE64_IMAP(b))>=0) {
                    /* collect base64 bytes into UChars */
                    انتخاب(base64_شمار) {
                    حالت -1: /* -1 is immediately after the & */
                    حالت 0:
                        بٹ=base64_قدر;
                        base64_شمار=1;
                        توڑو;
                    حالت 1:
                    حالت 3:
                    حالت 4:
                    حالت 6:
                        بٹ=(uint16_t)((بٹ<<6)|base64_قدر);
                        ++base64_شمار;
                        توڑو;
                    حالت 2:
                        c=(char16_t)((بٹ<<4)|(base64_قدر>>2));
                        اگر(isLegalIMAP(c)) {
                            /* illegal */
                            اندر_براہ_راست_منوال=سچ;
                            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                            جاؤ endloop;
                        }
                        *ہدف++=c;
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=ماخذ_اشاریہ;
                            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ-1;
                        }
                        بائٹیں[0]=b; /* keep this byte in case an error occurs */
                        byte_اشاریہ=1;
                        بٹ=(uint16_t)(base64_قدر&3);
                        base64_شمار=3;
                        توڑو;
                    حالت 5:
                        c=(char16_t)((بٹ<<2)|(base64_قدر>>4));
                        اگر(isLegalIMAP(c)) {
                            /* illegal */
                            اندر_براہ_راست_منوال=سچ;
                            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                            جاؤ endloop;
                        }
                        *ہدف++=c;
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=ماخذ_اشاریہ;
                            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ-1;
                        }
                        بائٹیں[0]=b; /* keep this byte in case an error occurs */
                        byte_اشاریہ=1;
                        بٹ=(uint16_t)(base64_قدر&15);
                        base64_شمار=6;
                        توڑو;
                    حالت 7:
                        c=(char16_t)((بٹ<<6)|base64_قدر);
                        اگر(isLegalIMAP(c)) {
                            /* illegal */
                            اندر_براہ_راست_منوال=سچ;
                            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                            جاؤ endloop;
                        }
                        *ہدف++=c;
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=ماخذ_اشاریہ;
                            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                        }
                        byte_اشاریہ=0;
                        بٹ=0;
                        base64_شمار=0;
                        توڑو;
                    طےشدہ:
                        /* will never occur */
                        توڑو;
                    }
                } ورنہ اگر(base64_قدر==-2) {
                    /* minus sign terminates the base64 sequence */
                    اندر_براہ_راست_منوال=سچ;
                    اگر(base64_شمار==-1) {
                        /* &- i.e. a minus immediately following an ampersand */
                        *ہدف++=AMPERSAND;
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=ماخذ_اشاریہ-1;
                        }
                    } ورنہ {
                        /* absorb the minus and leave the Unicode Mode */
                        اگر(بٹ!=0 || (base64_شمار!=0 && base64_شمار!=3 && base64_شمار!=6)) {
                            /* bits are illegally left over, a char16_t is incomplete */
                            /* base64Counter other than 0, 3, 6 means non-minimal zero-padding, also illegal */
                            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                            توڑو;
                        }
                    }
                    ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                    جاؤ براہ_راست_منوال;
                } ورنہ {
                    اگر(base64_شمار==-1) {
                        /* illegal: & immediately followed by something other than base64 or minus sign */
                        /* include the ampersand in the reported sequence */
                        --ماخذ_اشاریہ;
                        بائٹیں[0]=AMPERSAND;
                        بائٹیں[1]=b;
                        byte_اشاریہ=2;
                    }
                    /* base64Value==-1 for characters that are illegal only in Unicode mode */
                    /* base64Value==-3 for illegal characters */
                    /* illegal */
                    اندر_براہ_راست_منوال=سچ;
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    توڑو;
                }
            } ورنہ {
                /* target is full */
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
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
    اگر( U_SUCCESS(*p_Error_رمز) &&
        !اندر_براہ_راست_منوال && byte_اشاریہ==0 &&
        p_دلائل->flush && ماخذ>=ماخذ_حد_بندی
    ) {
        اگر(base64_شمار==-1) {
            /* & at the very end of the input */
            /* make the ampersand the reported sequence */
            بائٹیں[0]=AMPERSAND;
            byte_اشاریہ=1;
        }
        /* else if(base64Counter!=-1) byteIndex remains 0 because there is no particular byte sequence */

        اندر_براہ_راست_منوال=سچ; /* avoid looping */
        *p_Error_رمز=U_TRUNCATED_CHAR_ملا;
    }

    /* set the converter state back into UConverter */
    cnv->مقصد_Unicode_حالت=((uint32_t)اندر_براہ_راست_منوال<<24)|((uint32_t)((uint8_t)base64_شمار)<<16)|(uint32_t)بٹ;
    cnv->مقصد_ULength=byte_اشاریہ;

    /* write back the updated pointers */
    p_دلائل->ماخذ=(ثابت حرف *)ماخذ;
    p_دلائل->ہدف=ہدف;
    p_دلائل->offsets=offsets;
}

ساکن خالی U_CALLCONV
_IMAPFromUnicodeWithOffsets(UConverter_From_Unicode_دلائل *p_دلائل,
                            UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت char16_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف, *ہدف_حد_بندی;
    int32_t *offsets;

    int32_t length, ہدف_استعداد, ماخذ_اشاریہ;
    char16_t c;
    uint8_t b;

    /* UTF-7 state */
    uint8_t بٹ;
    int8_t base64_شمار;
    UBool اندر_براہ_راست_منوال;

    /* set up the local pointers */
    cnv=p_دلائل->converter;

    /* set up the local pointers */
    ماخذ=p_دلائل->ماخذ;
    ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
    ہدف=(uint8_t *)p_دلائل->ہدف;
    ہدف_حد_بندی=(uint8_t *)p_دلائل->ہدف_حد_بندی;
    offsets=p_دلائل->offsets;

    /* get the state machine state */
    {
        uint32_t حالت_2=cnv->from_Unicode_حالت;
        اندر_براہ_راست_منوال=(UBool)((حالت_2>>24)&1);
        base64_شمار=(int8_t)(حالت_2>>16);
        بٹ=(uint8_t)حالت_2;
    }

    /* UTF-7 always encodes UTF-16 code units, therefore we need only a simple sourceIndex */
    ماخذ_اشاریہ=0;

    اگر(اندر_براہ_راست_منوال) {
براہ_راست_منوال:
        length=(int32_t)(ماخذ_حد_بندی-ماخذ);
        ہدف_استعداد=(int32_t)(ہدف_حد_بندی-ہدف);
        اگر(length>ہدف_استعداد) {
            length=ہدف_استعداد;
        }
        جب(length>0) {
            c=*ماخذ++;
            /* encode 0x20..0x7e except '&' directly */
            اگر(اندر_سیٹ_DIMAP(c)) {
                /* encode directly */
                *ہدف++=(uint8_t)c;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ++;
                }
            } ورنہ اگر(c==AMPERSAND) {
                /* output &- for & */
                *ہدف++=AMPERSAND;
                اگر(ہدف<ہدف_حد_بندی) {
                    *ہدف++=MINUS;
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                        *offsets++=ماخذ_اشاریہ++;
                    }
                    /* realign length and targetCapacity */
                    جاؤ براہ_راست_منوال;
                } ورنہ {
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ++;
                    }
                    cnv->char_Error_بفر[0]=MINUS;
                    cnv->char_Error_بفر_لمبائی=1;
                    *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                    توڑو;
                }
            } ورنہ {
                /* un-read this character and switch to Unicode Mode */
                --ماخذ;
                *ہدف++=AMPERSAND;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                اندر_براہ_راست_منوال=جھوٹ;
                base64_شمار=0;
                جاؤ unicode_منوال;
            }
            --length;
        }
        اگر(ماخذ<ماخذ_حد_بندی && ہدف>=ہدف_حد_بندی) {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        }
    } ورنہ {
unicode_منوال:
        جب(ماخذ<ماخذ_حد_بندی) {
            اگر(ہدف<ہدف_حد_بندی) {
                c=*ماخذ++;
                اگر(isLegalIMAP(c)) {
                    /* encode directly */
                    اندر_براہ_راست_منوال=سچ;

                    /* trick: back out this character to make this easier */
                    --ماخذ;

                    /* terminate the base64 sequence */
                    اگر(base64_شمار!=0) {
                        /* write remaining bits for the previous character */
                        *ہدف++=مقصد_BASE64_IMAP(بٹ);
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=ماخذ_اشاریہ-1;
                        }
                    }
                    /* need to terminate with a minus */
                    اگر(ہدف<ہدف_حد_بندی) {
                        *ہدف++=MINUS;
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=ماخذ_اشاریہ-1;
                        }
                    } ورنہ {
                        cnv->char_Error_بفر[0]=MINUS;
                        cnv->char_Error_بفر_لمبائی=1;
                        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                        توڑو;
                    }
                    جاؤ براہ_راست_منوال;
                } ورنہ {
                    /*
                     * base64 this character:
                     * Output 2 or 3 base64 bytes for the remaining bits of the previous character
                     * and the bits of this character, each implicitly in UTF-16BE.
                     *
                     * Here, bits is an 8-bit variable because only 6 bits need to be kept from one
                     * character to the next. The actual 2 or 4 bits are shifted to the left edge
                     * of the 6-bits field 5..0 to make the termination of the base64 sequence easier.
                     */
                    انتخاب(base64_شمار) {
                    حالت 0:
                        b=(uint8_t)(c>>10);
                        *ہدف++=مقصد_BASE64_IMAP(b);
                        اگر(ہدف<ہدف_حد_بندی) {
                            b=(uint8_t)((c>>4)&0x3f);
                            *ہدف++=مقصد_BASE64_IMAP(b);
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ;
                                *offsets++=ماخذ_اشاریہ++;
                            }
                        } ورنہ {
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ++;
                            }
                            b=(uint8_t)((c>>4)&0x3f);
                            cnv->char_Error_بفر[0]=مقصد_BASE64_IMAP(b);
                            cnv->char_Error_بفر_لمبائی=1;
                            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                        }
                        بٹ=(uint8_t)((c&15)<<2);
                        base64_شمار=1;
                        توڑو;
                    حالت 1:
                        b=(uint8_t)(بٹ|(c>>14));
                        *ہدف++=مقصد_BASE64_IMAP(b);
                        اگر(ہدف<ہدف_حد_بندی) {
                            b=(uint8_t)((c>>8)&0x3f);
                            *ہدف++=مقصد_BASE64_IMAP(b);
                            اگر(ہدف<ہدف_حد_بندی) {
                                b=(uint8_t)((c>>2)&0x3f);
                                *ہدف++=مقصد_BASE64_IMAP(b);
                                اگر(offsets!=خالی_اشاریہ) {
                                    *offsets++=ماخذ_اشاریہ;
                                    *offsets++=ماخذ_اشاریہ;
                                    *offsets++=ماخذ_اشاریہ++;
                                }
                            } ورنہ {
                                اگر(offsets!=خالی_اشاریہ) {
                                    *offsets++=ماخذ_اشاریہ;
                                    *offsets++=ماخذ_اشاریہ++;
                                }
                                b=(uint8_t)((c>>2)&0x3f);
                                cnv->char_Error_بفر[0]=مقصد_BASE64_IMAP(b);
                                cnv->char_Error_بفر_لمبائی=1;
                                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                            }
                        } ورنہ {
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ++;
                            }
                            b=(uint8_t)((c>>8)&0x3f);
                            cnv->char_Error_بفر[0]=مقصد_BASE64_IMAP(b);
                            b=(uint8_t)((c>>2)&0x3f);
                            cnv->char_Error_بفر[1]=مقصد_BASE64_IMAP(b);
                            cnv->char_Error_بفر_لمبائی=2;
                            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                        }
                        بٹ=(uint8_t)((c&3)<<4);
                        base64_شمار=2;
                        توڑو;
                    حالت 2:
                        b=(uint8_t)(بٹ|(c>>12));
                        *ہدف++=مقصد_BASE64_IMAP(b);
                        اگر(ہدف<ہدف_حد_بندی) {
                            b=(uint8_t)((c>>6)&0x3f);
                            *ہدف++=مقصد_BASE64_IMAP(b);
                            اگر(ہدف<ہدف_حد_بندی) {
                                b=(uint8_t)(c&0x3f);
                                *ہدف++=مقصد_BASE64_IMAP(b);
                                اگر(offsets!=خالی_اشاریہ) {
                                    *offsets++=ماخذ_اشاریہ;
                                    *offsets++=ماخذ_اشاریہ;
                                    *offsets++=ماخذ_اشاریہ++;
                                }
                            } ورنہ {
                                اگر(offsets!=خالی_اشاریہ) {
                                    *offsets++=ماخذ_اشاریہ;
                                    *offsets++=ماخذ_اشاریہ++;
                                }
                                b=(uint8_t)(c&0x3f);
                                cnv->char_Error_بفر[0]=مقصد_BASE64_IMAP(b);
                                cnv->char_Error_بفر_لمبائی=1;
                                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                            }
                        } ورنہ {
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ++;
                            }
                            b=(uint8_t)((c>>6)&0x3f);
                            cnv->char_Error_بفر[0]=مقصد_BASE64_IMAP(b);
                            b=(uint8_t)(c&0x3f);
                            cnv->char_Error_بفر[1]=مقصد_BASE64_IMAP(b);
                            cnv->char_Error_بفر_لمبائی=2;
                            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                        }
                        بٹ=0;
                        base64_شمار=0;
                        توڑو;
                    طےشدہ:
                        /* will never occur */
                        توڑو;
                    }
                }
            } ورنہ {
                /* target is full */
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }
        }
    }

    اگر(p_دلائل->flush && ماخذ>=ماخذ_حد_بندی) {
        /* flush remaining bits to the target */
        اگر(!اندر_براہ_راست_منوال) {
            اگر(base64_شمار!=0) {
                اگر(ہدف<ہدف_حد_بندی) {
                    *ہدف++=مقصد_BASE64_IMAP(بٹ);
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ-1;
                    }
                } ورنہ {
                    cnv->char_Error_بفر[cnv->char_Error_بفر_لمبائی++]=مقصد_BASE64_IMAP(بٹ);
                    *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                }
            }
            /* need to terminate with a minus */
            اگر(ہدف<ہدف_حد_بندی) {
                *ہدف++=MINUS;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ-1;
                }
            } ورنہ {
                cnv->char_Error_بفر[cnv->char_Error_بفر_لمبائی++]=MINUS;
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            }
        }
        /* reset the state for the next conversion */
        cnv->from_Unicode_حالت=(cnv->from_Unicode_حالت&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    } ورنہ {
        /* set the converter state back into UConverter */
        cnv->from_Unicode_حالت=
            (cnv->from_Unicode_حالت&0xf0000000)|    /* keep version*/
            ((uint32_t)اندر_براہ_راست_منوال<<24)|((uint32_t)base64_شمار<<16)|(uint32_t)بٹ;
    }

    /* write back the updated pointers */
    p_دلائل->ماخذ=ماخذ;
    p_دلائل->ہدف=(حرف *)ہدف;
    p_دلائل->offsets=offsets;
}
U_CDECL_انتہا

ساکن ثابت UConverterImpl _IMAPImpl={
    UCNV_IMAP_MAILBOX,

    خالی_اشاریہ,
    خالی_اشاریہ,

    _UTF7Open,
    خالی_اشاریہ,
    _UTF7Reset,

    _IMAPToUnicodeWithOffsets,
    _IMAPToUnicodeWithOffsets,
    _IMAPFromUnicodeWithOffsets,
    _IMAPFromUnicodeWithOffsets,
    خالی_اشاریہ,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ, /* we don't need writeSub() because we never call a callback at fromUnicode() */
    خالی_اشاریہ,
    ucnv_get_Complete_Unicode_سیٹ,
    خالی_اشاریہ,
    خالی_اشاریہ
};

ساکن ثابت UConverter_Static_ڈیٹا IMAPStatic_ڈیٹا={
    حجم(UConverter_Static_ڈیٹا),
    "IMAP-mailbox-name",
    0, /* TODO CCSID for IMAP-mailbox-name */
    UCNV_IBM, UCNV_IMAP_MAILBOX,
    1, 4,
    { 0x3f, 0, 0, 0 }, 1, /* the subchar is not used */
    جھوٹ, جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_ڈیٹا _IMAPData=
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&IMAPStatic_ڈیٹا, &_IMAPImpl);

#اگر_ختم
