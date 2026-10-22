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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_ਸੈੱਟ_D(c) \
    ((uint8_t)((c)-97)<26 || (uint8_t)((c)-65)<26 || /* letters */ \
     (uint8_t)((c)-48)<10 ||    /* digits */ \
     (uint8_t)((c)-39)<3 ||     /* '() */ \
     (uint8_t)((c)-44)<4 ||     /* ,-./ */ \
     (c)==58 || (c)==63         /* :? */ \
    )

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_ਸੈੱਟ_O(c) \
    ((uint8_t)((c)-33)<6 ||         /* !"#$%& */ \
     (uint8_t)((c)-59)<4 ||         /* ;<=> */ \
     (uint8_t)((c)-93)<4 ||         /* ]^_` */ \
     (uint8_t)((c)-123)<3 ||        /* {|} */ \
     (c)==42 || (c)==64 || (c)==91  /* *@[ */ \
    )

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isCRLFTAB(c) ((c)==13 || (c)==10 || (c)==9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isCRLFSPTAB(c) ((c)==32 || (c)==13 || (c)==10 || (c)==9)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLUS  43
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MINUS 45
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BACKSLASH 92
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TILDE 126

/* legal byte values: all US-ASCII graphic characters from space to before tilde, and CR LF TAB */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isLegalUTF7(c) (((uint8_t)((c)-32)<94 && (c)!=BACKSLASH) || isCRLFTAB(c))

/* encode directly sets D and O and CR LF SP TAB */
ਸਥਿਰ ਅਚਲ UBool encode_Directly_ਵੱਧ_ਤੋਂ_ਵੱਧ[128]={
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
ਸਥਿਰ ਅਚਲ UBool encodeDirectlyRestricted[128]={
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

ਸਥਿਰ ਅਚਲ uint8_t
ਮੰਜ਼ਿਲ_Base64[64]={
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

ਸਥਿਰ ਅਚਲ int8_t
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
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_UTF7Reset(UConverter *cnv, UConverterResetChoice choice) {
    ਜੇ(choice<=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        /* reset toUnicode */
        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=0x1000000; /* inDirectMode=true */
        cnv->ਮੰਜ਼ਿਲ_ULength=0;
    }
    ਜੇ(choice!=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        /* reset fromUnicode */
        cnv->from_Unicode_ਸਥਿਤੀ=(cnv->from_Unicode_ਸਥਿਤੀ&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_UTF7Open(UConverter *cnv,
          UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
          UError_ਕੋਡ *p_Error_ਕੋਡ) {
    (ਖਾਲੀ)p_ਦਲੀਲਾਂ;
    ਜੇ(UCNV_GET_ਸੰਸਕਰਣ(cnv)<=1) {
        /* TODO(markus): Should just use cnv->options rather than copying the version number. */
        cnv->from_Unicode_ਸਥਿਤੀ=UCNV_GET_ਸੰਸਕਰਣ(cnv)<<28;
        _UTF7Reset(cnv, UCNV_RESET_BOTH);
    } ਨਹੀਂਤਾਂ {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UTF7_ਮੰਜ਼ਿਲ_Unicode_With_Offsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                          UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    char16_t *ਨਿਸ਼ਾਨਾ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    int32_t *offsets;

    uint8_t *ਬਾਈਟਾਂ;
    uint8_t byte_ਸੂਚਕਾਂਕ;

    int32_t length, ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;

    /* UTF-7 state */
    uint16_t ਬਿੱਟ;
    int8_t base64Counter;
    UBool ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ;

    int8_t base64_ਮੁੱਲ;

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ, ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    uint8_t b;
    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;

    ਸਰੋਤ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    offsets=p_ਦਲੀਲਾਂ->offsets;
    /* get the state machine state */
    {
        uint32_t ਸਥਿਤੀ=cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;
        ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=(UBool)((ਸਥਿਤੀ>>24)&1);
        base64Counter=(int8_t)(ਸਥਿਤੀ>>16);
        ਬਿੱਟ=(uint16_t)ਸਥਿਤੀ;
    }
    ਬਾਈਟਾਂ=cnv->ਮੰਜ਼ਿਲ_UBytes;
    byte_ਸੂਚਕਾਂਕ=cnv->ਮੰਜ਼ਿਲ_ULength;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=byte_ਸੂਚਕਾਂਕ==0 ? 0 : -1;
    ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    ਜੇ(ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ) {
ਸਿੱਧਾ_ਬਹੁਲਕ:
        /*
         * In Direct Mode, most US-ASCII characters are encoded directly, i.e.,
         * with their US-ASCII byte values.
         * Backslash and Tilde and most control characters are not allowed in UTF-7.
         * A plus sign starts Unicode (or "escape") Mode.
         *
         * In Direct Mode, only the sourceIndex is used.
         */
        byte_ਸੂਚਕਾਂਕ=0;
        length=(int32_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ);
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-ਨਿਸ਼ਾਨਾ);
        ਜੇ(length>ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
            length=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
        }
        ਜਦੋਂ(length>0) {
            b=*ਸਰੋਤ++;
            ਜੇ(!isLegalUTF7(b)) {
                /* illegal */
                ਬਾਈਟਾਂ[0]=b;
                byte_ਸੂਚਕਾਂਕ=1;
                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(b!=PLUS) {
                /* write directly encoded character */
                *ਨਿਸ਼ਾਨਾ++=b;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                }
            } ਨਹੀਂਤਾਂ /* PLUS */ {
                /* switch to Unicode mode */
                ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ=++ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਝੂਠ;
                byte_ਸੂਚਕਾਂਕ=0;
                ਬਿੱਟ=0;
                base64Counter=-1;
                ਜਾਓ unicode_ਬਹੁਲਕ;
            }
            --length;
        }
        ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ>=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        }
    } ਨਹੀਂਤਾਂ {
unicode_ਬਹੁਲਕ:
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
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                ਬਾਈਟਾਂ[byte_ਸੂਚਕਾਂਕ++]=b=*ਸਰੋਤ++;
                ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                base64_ਮੁੱਲ = -3; /* initialize as illegal */
                ਜੇ(b>=126 || (base64_ਮੁੱਲ=fromBase64[b])==-3 || base64_ਮੁੱਲ==-1) {
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
                    ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਸੱਚ;
                    ਜੇ(base64Counter==-1) {
                        /* illegal: + immediately followed by something other than base64 or minus sign */
                        /* include the plus sign in the reported sequence, but not the subsequent char */
                        --ਸਰੋਤ;
                        ਬਾਈਟਾਂ[0]=PLUS;
                        byte_ਸੂਚਕਾਂਕ=1;
                        *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਰੋਕੋ;
                    } ਨਹੀਂਤਾਂ ਜੇ(ਬਿੱਟ!=0) {
                        /* bits are illegally left over, a char16_t is incomplete */
                        /* don't include current char (legal or illegal) in error seq */
                        --ਸਰੋਤ;
                        --byte_ਸੂਚਕਾਂਕ;
                        *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਰੋਕੋ;
                    } ਨਹੀਂਤਾਂ {
                        /* previous char16_t was complete */
                        ਜੇ(base64_ਮੁੱਲ==-3) {
                            /* current character is illegal, deal with it here */
                            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            ਰੋਕੋ;
                        } ਨਹੀਂਤਾਂ {
                            /* un-read the current character in case it is a plus sign */
                            --ਸਰੋਤ;
                            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                            ਜਾਓ ਸਿੱਧਾ_ਬਹੁਲਕ;
                        }
                    }
                } ਨਹੀਂਤਾਂ ਜੇ(base64_ਮੁੱਲ>=0) {
                    /* collect base64 bytes into UChars */
                    ਚੋਣ(base64Counter) {
                    ਹਾਲਤ -1: /* -1 is immediately after the + */
                    ਹਾਲਤ 0:
                        ਬਿੱਟ=base64_ਮੁੱਲ;
                        base64Counter=1;
                        ਰੋਕੋ;
                    ਹਾਲਤ 1:
                    ਹਾਲਤ 3:
                    ਹਾਲਤ 4:
                    ਹਾਲਤ 6:
                        ਬਿੱਟ=(uint16_t)((ਬਿੱਟ<<6)|base64_ਮੁੱਲ);
                        ++base64Counter;
                        ਰੋਕੋ;
                    ਹਾਲਤ 2:
                        *ਨਿਸ਼ਾਨਾ++=(char16_t)((ਬਿੱਟ<<4)|(base64_ਮੁੱਲ>>2));
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                        }
                        ਬਾਈਟਾਂ[0]=b; /* keep this byte in case an error occurs */
                        byte_ਸੂਚਕਾਂਕ=1;
                        ਬਿੱਟ=(uint16_t)(base64_ਮੁੱਲ&3);
                        base64Counter=3;
                        ਰੋਕੋ;
                    ਹਾਲਤ 5:
                        *ਨਿਸ਼ਾਨਾ++=(char16_t)((ਬਿੱਟ<<2)|(base64_ਮੁੱਲ>>4));
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                        }
                        ਬਾਈਟਾਂ[0]=b; /* keep this byte in case an error occurs */
                        byte_ਸੂਚਕਾਂਕ=1;
                        ਬਿੱਟ=(uint16_t)(base64_ਮੁੱਲ&15);
                        base64Counter=6;
                        ਰੋਕੋ;
                    ਹਾਲਤ 7:
                        *ਨਿਸ਼ਾਨਾ++=(char16_t)((ਬਿੱਟ<<6)|base64_ਮੁੱਲ);
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        }
                        byte_ਸੂਚਕਾਂਕ=0;
                        ਬਿੱਟ=0;
                        base64Counter=0;
                        ਰੋਕੋ;
                    ਮੂਲ:
                        /* will never occur */
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ /*base64Value==-2*/ {
                    /* minus sign terminates the base64 sequence */
                    ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਸੱਚ;
                    ਜੇ(base64Counter==-1) {
                        /* +- i.e. a minus immediately following a plus */
                        *ਨਿਸ਼ਾਨਾ++=PLUS;
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* absorb the minus and leave the Unicode Mode */
                        ਜੇ(ਬਿੱਟ!=0) {
                            /* bits are illegally left over, a char16_t is incomplete */
                            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            ਰੋਕੋ;
                        }
                    }
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    ਜਾਓ ਸਿੱਧਾ_ਬਹੁਲਕ;
                }
            } ਨਹੀਂਤਾਂ {
                /* target is full */
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }
        }
    }

    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ) && p_ਦਲੀਲਾਂ->flush && ਸਰੋਤ==ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਬਿੱਟ==0) {
        /*
         * if we are in Unicode mode, then the byteIndex might not be 0,
         * but that is ok if bits==0
         * -> we set byteIndex=0 at the end of the stream to avoid a truncated error
         * (not true for IMAP-mailbox-name where we must end in direct mode)
         */
        byte_ਸੂਚਕਾਂਕ=0;
    }

    /* set the converter state back into UConverter */
    cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=((uint32_t)ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ<<24)|((uint32_t)((uint8_t)base64Counter)<<16)|(uint32_t)ਬਿੱਟ;
    cnv->ਮੰਜ਼ਿਲ_ULength=byte_ਸੂਚਕਾਂਕ;

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_UTF7FromUnicodeWithOffsets(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                            UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ char16_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ, *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    int32_t *offsets;

    int32_t length, ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    char16_t c;

    /* UTF-7 state */
    ਅਚਲ UBool *encodeDirectly;
    uint8_t ਬਿੱਟ;
    int8_t base64Counter;
    UBool ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;

    /* set up the local pointers */
    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=(uint8_t *)p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=(uint8_t *)p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    offsets=p_ਦਲੀਲਾਂ->offsets;

    /* get the state machine state */
    {
        uint32_t ਸਥਿਤੀ=cnv->from_Unicode_ਸਥਿਤੀ;
        encodeDirectly= ਸਥਿਤੀ<0x10000000 ? encode_Directly_ਵੱਧ_ਤੋਂ_ਵੱਧ : encodeDirectlyRestricted;
        ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=(UBool)((ਸਥਿਤੀ>>24)&1);
        base64Counter=(int8_t)(ਸਥਿਤੀ>>16);
        ਬਿੱਟ=(uint8_t)ਸਥਿਤੀ;
        U_ASSERT(ਬਿੱਟ<=UPRV_LENGTHOF(ਮੰਜ਼ਿਲ_Base64));
    }

    /* UTF-7 always encodes UTF-16 code units, therefore we need only a simple sourceIndex */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    ਜੇ(ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ) {
ਸਿੱਧਾ_ਬਹੁਲਕ:
        length=(int32_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ);
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-ਨਿਸ਼ਾਨਾ);
        ਜੇ(length>ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
            length=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
        }
        ਜਦੋਂ(length>0) {
            c=*ਸਰੋਤ++;
            /* currently always encode CR LF SP TAB directly */
            ਜੇ(c<=127 && encodeDirectly[c]) {
                /* encode directly */
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c==PLUS) {
                /* output +- for + */
                *ਨਿਸ਼ਾਨਾ++=PLUS;
                ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                    *ਨਿਸ਼ਾਨਾ++=MINUS;
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                    }
                    /* realign length and targetCapacity */
                    ਜਾਓ ਸਿੱਧਾ_ਬਹੁਲਕ;
                } ਨਹੀਂਤਾਂ {
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                    }
                    cnv->char_Error_ਬਫਰ[0]=MINUS;
                    cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                    *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ {
                /* un-read this character and switch to Unicode Mode */
                --ਸਰੋਤ;
                *ਨਿਸ਼ਾਨਾ++=PLUS;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਝੂਠ;
                base64Counter=0;
                ਜਾਓ unicode_ਬਹੁਲਕ;
            }
            --length;
        }
        ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ>=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        }
    } ਨਹੀਂਤਾਂ {
unicode_ਬਹੁਲਕ:
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                c=*ਸਰੋਤ++;
                ਜੇ(c<=127 && encodeDirectly[c]) {
                    /* encode directly */
                    ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਸੱਚ;

                    /* trick: back out this character to make this easier */
                    --ਸਰੋਤ;

                    /* terminate the base64 sequence */
                    ਜੇ(base64Counter!=0) {
                        /* write remaining bits for the previous character */
                        *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_Base64[ਬਿੱਟ];
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                        }
                    }
                    ਜੇ(fromBase64[c]!=-1) {
                        /* need to terminate with a minus */
                        ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                            *ਨਿਸ਼ਾਨਾ++=MINUS;
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                            }
                        } ਨਹੀਂਤਾਂ {
                            cnv->char_Error_ਬਫਰ[0]=MINUS;
                            cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                            ਰੋਕੋ;
                        }
                    }
                    ਜਾਓ ਸਿੱਧਾ_ਬਹੁਲਕ;
                } ਨਹੀਂਤਾਂ {
                    /*
                     * base64 this character:
                     * Output 2 or 3 base64 bytes for the remaining bits of the previous character
                     * and the bits of this character, each implicitly in UTF-16BE.
                     *
                     * Here, bits is an 8-bit variable because only 6 bits need to be kept from one
                     * character to the next. The actual 2 or 4 bits are shifted to the left edge
                     * of the 6-bits field 5..0 to make the termination of the base64 sequence easier.
                     */
                    ਚੋਣ(base64Counter) {
                    ਹਾਲਤ 0:
                        *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_Base64[c>>10];
                        ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                            *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_Base64[(c>>4)&0x3f];
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                            }
                        } ਨਹੀਂਤਾਂ {
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                            }
                            cnv->char_Error_ਬਫਰ[0]=ਮੰਜ਼ਿਲ_Base64[(c>>4)&0x3f];
                            cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                        }
                        ਬਿੱਟ=(uint8_t)((c&15)<<2);
                        base64Counter=1;
                        ਰੋਕੋ;
                    ਹਾਲਤ 1:
                        *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_Base64[ਬਿੱਟ|(c>>14)];
                        ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                            *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_Base64[(c>>8)&0x3f];
                            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                                *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_Base64[(c>>2)&0x3f];
                                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                                }
                            } ਨਹੀਂਤਾਂ {
                                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                                }
                                cnv->char_Error_ਬਫਰ[0]=ਮੰਜ਼ਿਲ_Base64[(c>>2)&0x3f];
                                cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                            }
                        } ਨਹੀਂਤਾਂ {
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                            }
                            cnv->char_Error_ਬਫਰ[0]=ਮੰਜ਼ਿਲ_Base64[(c>>8)&0x3f];
                            cnv->char_Error_ਬਫਰ[1]=ਮੰਜ਼ਿਲ_Base64[(c>>2)&0x3f];
                            cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=2;
                            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                        }
                        ਬਿੱਟ=(uint8_t)((c&3)<<4);
                        base64Counter=2;
                        ਰੋਕੋ;
                    ਹਾਲਤ 2:
                        *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_Base64[ਬਿੱਟ|(c>>12)];
                        ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                            *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_Base64[(c>>6)&0x3f];
                            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                                *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_Base64[c&0x3f];
                                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                                }
                            } ਨਹੀਂਤਾਂ {
                                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                                }
                                cnv->char_Error_ਬਫਰ[0]=ਮੰਜ਼ਿਲ_Base64[c&0x3f];
                                cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                            }
                        } ਨਹੀਂਤਾਂ {
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                            }
                            cnv->char_Error_ਬਫਰ[0]=ਮੰਜ਼ਿਲ_Base64[(c>>6)&0x3f];
                            cnv->char_Error_ਬਫਰ[1]=ਮੰਜ਼ਿਲ_Base64[c&0x3f];
                            cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=2;
                            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                        }
                        ਬਿੱਟ=0;
                        base64Counter=0;
                        ਰੋਕੋ;
                    ਮੂਲ:
                        /* will never occur */
                        ਰੋਕੋ;
                    }
                }
            } ਨਹੀਂਤਾਂ {
                /* target is full */
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }
        }
    }

    ਜੇ(p_ਦਲੀਲਾਂ->flush && ਸਰੋਤ>=ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /* flush remaining bits to the target */
        ਜੇ(!ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ) {
            ਜੇ (base64Counter!=0) {
                ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                    *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_Base64[ਬਿੱਟ];
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                    }
                } ਨਹੀਂਤਾਂ {
                    cnv->char_Error_ਬਫਰ[cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ++]=ਮੰਜ਼ਿਲ_Base64[ਬਿੱਟ];
                    *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                }
            }
            /* Add final MINUS to terminate unicodeMode */
            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                *ਨਿਸ਼ਾਨਾ++=MINUS;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                }
            } ਨਹੀਂਤਾਂ {
                cnv->char_Error_ਬਫਰ[cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ++]=MINUS;
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            }
        }
        /* reset the state for the next conversion */
        cnv->from_Unicode_ਸਥਿਤੀ=(cnv->from_Unicode_ਸਥਿਤੀ&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    } ਨਹੀਂਤਾਂ {
        /* set the converter state back into UConverter */
        cnv->from_Unicode_ਸਥਿਤੀ=
            (cnv->from_Unicode_ਸਥਿਤੀ&0xf0000000)|    /* keep version*/
            ((uint32_t)ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ<<24)|((uint32_t)base64Counter<<16)|(uint32_t)ਬਿੱਟ;
    }

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=(ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ * U_CALLCONV
_UTF7GetName(ਅਚਲ UConverter *cnv) {
    ਚੋਣ(cnv->from_Unicode_ਸਥਿਤੀ>>28) {
    ਹਾਲਤ 1:
        ਵਾਪਸ "UTF-7,version=1";
    ਮੂਲ:
        ਵਾਪਸ "UTF-7";
    }
}
U_CDECL_ਸਿਰਾ

ਸਥਿਰ ਅਚਲ UConverterImpl _UTF7Impl={
    UCNV_UTF7,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _UTF7Open,
    ਖਾਲੀ_ਸੂਚਕ,
    _UTF7Reset,

    UTF7_ਮੰਜ਼ਿਲ_Unicode_With_Offsets,
    UTF7_ਮੰਜ਼ਿਲ_Unicode_With_Offsets,
    _UTF7FromUnicodeWithOffsets,
    _UTF7FromUnicodeWithOffsets,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    _UTF7GetName,
    ਖਾਲੀ_ਸੂਚਕ, /* we don't need writeSub() because we never call a callback at fromUnicode() */
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_get_Complete_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ UTF7_Static_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "UTF-7",
    0, /* TODO CCSID for UTF-7 */
    UCNV_IBM, UCNV_UTF7,
    1, 4,
    { 0x3f, 0, 0, 0 }, 1, /* the subchar is not used */
    ਝੂਠ, ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ UTF7_ਅੰਕੜੇ=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&UTF7_Static_ਅੰਕੜੇ, &_UTF7Impl);

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMPERSAND 0x26
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMMA 0x2c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SLASH 0x2f

/* legal byte values: all US-ASCII graphic characters 0x20..0x7e */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isLegalIMAP(c) (0x20<=(c) && (c)<=0x7e)

/* direct-encode all of printable ASCII 0x20..0x7e except '&' 0x26 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਿੱਚ_ਸੈੱਟ_DIMAP(c) (isLegalIMAP(c) && c!=AMPERSAND)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੰਜ਼ਿਲ_BASE64_IMAP(n) ((n)<63 ? ਮੰਜ਼ਿਲ_Base64[n] : COMMA)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FROM_BASE64_IMAP(c) ((c)==COMMA ? 63 : (c)==SLASH ? -1 : fromBase64[c])

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
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_IMAPToUnicodeWithOffsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                          UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    char16_t *ਨਿਸ਼ਾਨਾ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    int32_t *offsets;

    uint8_t *ਬਾਈਟਾਂ;
    uint8_t byte_ਸੂਚਕਾਂਕ;

    int32_t length, ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;

    /* UTF-7 state */
    uint16_t ਬਿੱਟ;
    int8_t base64Counter;
    UBool ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ;

    int8_t base64_ਮੁੱਲ;

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ, ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    char16_t c;
    uint8_t b;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;

    ਸਰੋਤ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    offsets=p_ਦਲੀਲਾਂ->offsets;
    /* get the state machine state */
    {
        uint32_t ਸਥਿਤੀ=cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ;
        ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=(UBool)((ਸਥਿਤੀ>>24)&1);
        base64Counter=(int8_t)(ਸਥਿਤੀ>>16);
        ਬਿੱਟ=(uint16_t)ਸਥਿਤੀ;
    }
    ਬਾਈਟਾਂ=cnv->ਮੰਜ਼ਿਲ_UBytes;
    byte_ਸੂਚਕਾਂਕ=cnv->ਮੰਜ਼ਿਲ_ULength;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=byte_ਸੂਚਕਾਂਕ==0 ? 0 : -1;
    ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    ਜੇ(ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ) {
ਸਿੱਧਾ_ਬਹੁਲਕ:
        /*
         * In Direct Mode, US-ASCII characters are encoded directly, i.e.,
         * with their US-ASCII byte values.
         * An ampersand starts Unicode (or "escape") Mode.
         *
         * In Direct Mode, only the sourceIndex is used.
         */
        byte_ਸੂਚਕਾਂਕ=0;
        length=(int32_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ);
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-ਨਿਸ਼ਾਨਾ);
        ਜੇ(length>ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
            length=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
        }
        ਜਦੋਂ(length>0) {
            b=*ਸਰੋਤ++;
            ਜੇ(!isLegalIMAP(b)) {
                /* illegal */
                ਬਾਈਟਾਂ[0]=b;
                byte_ਸੂਚਕਾਂਕ=1;
                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(b!=AMPERSAND) {
                /* write directly encoded character */
                *ਨਿਸ਼ਾਨਾ++=b;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                }
            } ਨਹੀਂਤਾਂ /* AMPERSAND */ {
                /* switch to Unicode mode */
                ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ=++ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਝੂਠ;
                byte_ਸੂਚਕਾਂਕ=0;
                ਬਿੱਟ=0;
                base64Counter=-1;
                ਜਾਓ unicode_ਬਹੁਲਕ;
            }
            --length;
        }
        ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ>=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        }
    } ਨਹੀਂਤਾਂ {
unicode_ਬਹੁਲਕ:
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
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                ਬਾਈਟਾਂ[byte_ਸੂਚਕਾਂਕ++]=b=*ਸਰੋਤ++;
                ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                ਜੇ(b>0x7e) {
                    /* illegal - test other illegal US-ASCII values by base64Value==-3 */
                    ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਸੱਚ;
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ((base64_ਮੁੱਲ=FROM_BASE64_IMAP(b))>=0) {
                    /* collect base64 bytes into UChars */
                    ਚੋਣ(base64Counter) {
                    ਹਾਲਤ -1: /* -1 is immediately after the & */
                    ਹਾਲਤ 0:
                        ਬਿੱਟ=base64_ਮੁੱਲ;
                        base64Counter=1;
                        ਰੋਕੋ;
                    ਹਾਲਤ 1:
                    ਹਾਲਤ 3:
                    ਹਾਲਤ 4:
                    ਹਾਲਤ 6:
                        ਬਿੱਟ=(uint16_t)((ਬਿੱਟ<<6)|base64_ਮੁੱਲ);
                        ++base64Counter;
                        ਰੋਕੋ;
                    ਹਾਲਤ 2:
                        c=(char16_t)((ਬਿੱਟ<<4)|(base64_ਮੁੱਲ>>2));
                        ਜੇ(isLegalIMAP(c)) {
                            /* illegal */
                            ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਸੱਚ;
                            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            ਜਾਓ endloop;
                        }
                        *ਨਿਸ਼ਾਨਾ++=c;
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                        }
                        ਬਾਈਟਾਂ[0]=b; /* keep this byte in case an error occurs */
                        byte_ਸੂਚਕਾਂਕ=1;
                        ਬਿੱਟ=(uint16_t)(base64_ਮੁੱਲ&3);
                        base64Counter=3;
                        ਰੋਕੋ;
                    ਹਾਲਤ 5:
                        c=(char16_t)((ਬਿੱਟ<<2)|(base64_ਮੁੱਲ>>4));
                        ਜੇ(isLegalIMAP(c)) {
                            /* illegal */
                            ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਸੱਚ;
                            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            ਜਾਓ endloop;
                        }
                        *ਨਿਸ਼ਾਨਾ++=c;
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                        }
                        ਬਾਈਟਾਂ[0]=b; /* keep this byte in case an error occurs */
                        byte_ਸੂਚਕਾਂਕ=1;
                        ਬਿੱਟ=(uint16_t)(base64_ਮੁੱਲ&15);
                        base64Counter=6;
                        ਰੋਕੋ;
                    ਹਾਲਤ 7:
                        c=(char16_t)((ਬਿੱਟ<<6)|base64_ਮੁੱਲ);
                        ਜੇ(isLegalIMAP(c)) {
                            /* illegal */
                            ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਸੱਚ;
                            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            ਜਾਓ endloop;
                        }
                        *ਨਿਸ਼ਾਨਾ++=c;
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        }
                        byte_ਸੂਚਕਾਂਕ=0;
                        ਬਿੱਟ=0;
                        base64Counter=0;
                        ਰੋਕੋ;
                    ਮੂਲ:
                        /* will never occur */
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ ਜੇ(base64_ਮੁੱਲ==-2) {
                    /* minus sign terminates the base64 sequence */
                    ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਸੱਚ;
                    ਜੇ(base64Counter==-1) {
                        /* &- i.e. a minus immediately following an ampersand */
                        *ਨਿਸ਼ਾਨਾ++=AMPERSAND;
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* absorb the minus and leave the Unicode Mode */
                        ਜੇ(ਬਿੱਟ!=0 || (base64Counter!=0 && base64Counter!=3 && base64Counter!=6)) {
                            /* bits are illegally left over, a char16_t is incomplete */
                            /* base64Counter other than 0, 3, 6 means non-minimal zero-padding, also illegal */
                            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            ਰੋਕੋ;
                        }
                    }
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    ਜਾਓ ਸਿੱਧਾ_ਬਹੁਲਕ;
                } ਨਹੀਂਤਾਂ {
                    ਜੇ(base64Counter==-1) {
                        /* illegal: & immediately followed by something other than base64 or minus sign */
                        /* include the ampersand in the reported sequence */
                        --ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        ਬਾਈਟਾਂ[0]=AMPERSAND;
                        ਬਾਈਟਾਂ[1]=b;
                        byte_ਸੂਚਕਾਂਕ=2;
                    }
                    /* base64Value==-1 for characters that are illegal only in Unicode mode */
                    /* base64Value==-3 for illegal characters */
                    /* illegal */
                    ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਸੱਚ;
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ {
                /* target is full */
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
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
    ਜੇ( U_SUCCESS(*p_Error_ਕੋਡ) &&
        !ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ && byte_ਸੂਚਕਾਂਕ==0 &&
        p_ਦਲੀਲਾਂ->flush && ਸਰੋਤ>=ਸਰੋਤ_ਹੱਦਬੰਦੀ
    ) {
        ਜੇ(base64Counter==-1) {
            /* & at the very end of the input */
            /* make the ampersand the reported sequence */
            ਬਾਈਟਾਂ[0]=AMPERSAND;
            byte_ਸੂਚਕਾਂਕ=1;
        }
        /* else if(base64Counter!=-1) byteIndex remains 0 because there is no particular byte sequence */

        ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਸੱਚ; /* avoid looping */
        *p_Error_ਕੋਡ=U_TRUNCATED_CHAR_ਮਿਲਿਆ;
    }

    /* set the converter state back into UConverter */
    cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=((uint32_t)ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ<<24)|((uint32_t)((uint8_t)base64Counter)<<16)|(uint32_t)ਬਿੱਟ;
    cnv->ਮੰਜ਼ਿਲ_ULength=byte_ਸੂਚਕਾਂਕ;

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_IMAPFromUnicodeWithOffsets(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                            UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ char16_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ, *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    int32_t *offsets;

    int32_t length, ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    char16_t c;
    uint8_t b;

    /* UTF-7 state */
    uint8_t ਬਿੱਟ;
    int8_t base64Counter;
    UBool ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;

    /* set up the local pointers */
    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=(uint8_t *)p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=(uint8_t *)p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    offsets=p_ਦਲੀਲਾਂ->offsets;

    /* get the state machine state */
    {
        uint32_t ਸਥਿਤੀ=cnv->from_Unicode_ਸਥਿਤੀ;
        ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=(UBool)((ਸਥਿਤੀ>>24)&1);
        base64Counter=(int8_t)(ਸਥਿਤੀ>>16);
        ਬਿੱਟ=(uint8_t)ਸਥਿਤੀ;
    }

    /* UTF-7 always encodes UTF-16 code units, therefore we need only a simple sourceIndex */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    ਜੇ(ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ) {
ਸਿੱਧਾ_ਬਹੁਲਕ:
        length=(int32_t)(ਸਰੋਤ_ਹੱਦਬੰਦੀ-ਸਰੋਤ);
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-ਨਿਸ਼ਾਨਾ);
        ਜੇ(length>ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
            length=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
        }
        ਜਦੋਂ(length>0) {
            c=*ਸਰੋਤ++;
            /* encode 0x20..0x7e except '&' directly */
            ਜੇ(ਵਿੱਚ_ਸੈੱਟ_DIMAP(c)) {
                /* encode directly */
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c==AMPERSAND) {
                /* output &- for & */
                *ਨਿਸ਼ਾਨਾ++=AMPERSAND;
                ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                    *ਨਿਸ਼ਾਨਾ++=MINUS;
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                    }
                    /* realign length and targetCapacity */
                    ਜਾਓ ਸਿੱਧਾ_ਬਹੁਲਕ;
                } ਨਹੀਂਤਾਂ {
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                    }
                    cnv->char_Error_ਬਫਰ[0]=MINUS;
                    cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                    *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ {
                /* un-read this character and switch to Unicode Mode */
                --ਸਰੋਤ;
                *ਨਿਸ਼ਾਨਾ++=AMPERSAND;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਝੂਠ;
                base64Counter=0;
                ਜਾਓ unicode_ਬਹੁਲਕ;
            }
            --length;
        }
        ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ>=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        }
    } ਨਹੀਂਤਾਂ {
unicode_ਬਹੁਲਕ:
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                c=*ਸਰੋਤ++;
                ਜੇ(isLegalIMAP(c)) {
                    /* encode directly */
                    ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ=ਸੱਚ;

                    /* trick: back out this character to make this easier */
                    --ਸਰੋਤ;

                    /* terminate the base64 sequence */
                    ਜੇ(base64Counter!=0) {
                        /* write remaining bits for the previous character */
                        *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_BASE64_IMAP(ਬਿੱਟ);
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                        }
                    }
                    /* need to terminate with a minus */
                    ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                        *ਨਿਸ਼ਾਨਾ++=MINUS;
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                        }
                    } ਨਹੀਂਤਾਂ {
                        cnv->char_Error_ਬਫਰ[0]=MINUS;
                        cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                        ਰੋਕੋ;
                    }
                    ਜਾਓ ਸਿੱਧਾ_ਬਹੁਲਕ;
                } ਨਹੀਂਤਾਂ {
                    /*
                     * base64 this character:
                     * Output 2 or 3 base64 bytes for the remaining bits of the previous character
                     * and the bits of this character, each implicitly in UTF-16BE.
                     *
                     * Here, bits is an 8-bit variable because only 6 bits need to be kept from one
                     * character to the next. The actual 2 or 4 bits are shifted to the left edge
                     * of the 6-bits field 5..0 to make the termination of the base64 sequence easier.
                     */
                    ਚੋਣ(base64Counter) {
                    ਹਾਲਤ 0:
                        b=(uint8_t)(c>>10);
                        *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                        ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                            b=(uint8_t)((c>>4)&0x3f);
                            *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                            }
                        } ਨਹੀਂਤਾਂ {
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                            }
                            b=(uint8_t)((c>>4)&0x3f);
                            cnv->char_Error_ਬਫਰ[0]=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                            cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                        }
                        ਬਿੱਟ=(uint8_t)((c&15)<<2);
                        base64Counter=1;
                        ਰੋਕੋ;
                    ਹਾਲਤ 1:
                        b=(uint8_t)(ਬਿੱਟ|(c>>14));
                        *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                        ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                            b=(uint8_t)((c>>8)&0x3f);
                            *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                                b=(uint8_t)((c>>2)&0x3f);
                                *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                                }
                            } ਨਹੀਂਤਾਂ {
                                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                                }
                                b=(uint8_t)((c>>2)&0x3f);
                                cnv->char_Error_ਬਫਰ[0]=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                                cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                            }
                        } ਨਹੀਂਤਾਂ {
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                            }
                            b=(uint8_t)((c>>8)&0x3f);
                            cnv->char_Error_ਬਫਰ[0]=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                            b=(uint8_t)((c>>2)&0x3f);
                            cnv->char_Error_ਬਫਰ[1]=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                            cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=2;
                            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                        }
                        ਬਿੱਟ=(uint8_t)((c&3)<<4);
                        base64Counter=2;
                        ਰੋਕੋ;
                    ਹਾਲਤ 2:
                        b=(uint8_t)(ਬਿੱਟ|(c>>12));
                        *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                        ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                            b=(uint8_t)((c>>6)&0x3f);
                            *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                                b=(uint8_t)(c&0x3f);
                                *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                                }
                            } ਨਹੀਂਤਾਂ {
                                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                                }
                                b=(uint8_t)(c&0x3f);
                                cnv->char_Error_ਬਫਰ[0]=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                                cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                            }
                        } ਨਹੀਂਤਾਂ {
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                            }
                            b=(uint8_t)((c>>6)&0x3f);
                            cnv->char_Error_ਬਫਰ[0]=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                            b=(uint8_t)(c&0x3f);
                            cnv->char_Error_ਬਫਰ[1]=ਮੰਜ਼ਿਲ_BASE64_IMAP(b);
                            cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=2;
                            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                        }
                        ਬਿੱਟ=0;
                        base64Counter=0;
                        ਰੋਕੋ;
                    ਮੂਲ:
                        /* will never occur */
                        ਰੋਕੋ;
                    }
                }
            } ਨਹੀਂਤਾਂ {
                /* target is full */
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }
        }
    }

    ਜੇ(p_ਦਲੀਲਾਂ->flush && ਸਰੋਤ>=ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /* flush remaining bits to the target */
        ਜੇ(!ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ) {
            ਜੇ(base64Counter!=0) {
                ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                    *ਨਿਸ਼ਾਨਾ++=ਮੰਜ਼ਿਲ_BASE64_IMAP(ਬਿੱਟ);
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                    }
                } ਨਹੀਂਤਾਂ {
                    cnv->char_Error_ਬਫਰ[cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ++]=ਮੰਜ਼ਿਲ_BASE64_IMAP(ਬਿੱਟ);
                    *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                }
            }
            /* need to terminate with a minus */
            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                *ਨਿਸ਼ਾਨਾ++=MINUS;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                }
            } ਨਹੀਂਤਾਂ {
                cnv->char_Error_ਬਫਰ[cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ++]=MINUS;
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            }
        }
        /* reset the state for the next conversion */
        cnv->from_Unicode_ਸਥਿਤੀ=(cnv->from_Unicode_ਸਥਿਤੀ&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    } ਨਹੀਂਤਾਂ {
        /* set the converter state back into UConverter */
        cnv->from_Unicode_ਸਥਿਤੀ=
            (cnv->from_Unicode_ਸਥਿਤੀ&0xf0000000)|    /* keep version*/
            ((uint32_t)ਵਿੱਚ_ਸਿੱਧਾ_ਬਹੁਲਕ<<24)|((uint32_t)base64Counter<<16)|(uint32_t)ਬਿੱਟ;
    }

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=(ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}
U_CDECL_ਸਿਰਾ

ਸਥਿਰ ਅਚਲ UConverterImpl _IMAPImpl={
    UCNV_IMAP_MAILBOX,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _UTF7Open,
    ਖਾਲੀ_ਸੂਚਕ,
    _UTF7Reset,

    _IMAPToUnicodeWithOffsets,
    _IMAPToUnicodeWithOffsets,
    _IMAPFromUnicodeWithOffsets,
    _IMAPFromUnicodeWithOffsets,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ, /* we don't need writeSub() because we never call a callback at fromUnicode() */
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_get_Complete_Unicode_ਸੈੱਟ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ IMAPStatic_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "IMAP-mailbox-name",
    0, /* TODO CCSID for IMAP-mailbox-name */
    UCNV_IBM, UCNV_IMAP_MAILBOX,
    1, 4,
    { 0x3f, 0, 0, 0 }, 1, /* the subchar is not used */
    ਝੂਠ, ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _IMAPData=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&IMAPStatic_ਅੰਕੜੇ, &_IMAPImpl);

#ਜੇ_ਅੰਤ
