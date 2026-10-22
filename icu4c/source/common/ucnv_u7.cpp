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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

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
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_ಗಣ_D(c) \
    ((uint8_t)((c)-97)<26 || (uint8_t)((c)-65)<26 || /* letters */ \
     (uint8_t)((c)-48)<10 ||    /* digits */ \
     (uint8_t)((c)-39)<3 ||     /* '() */ \
     (uint8_t)((c)-44)<4 ||     /* ,-./ */ \
     (c)==58 || (c)==63         /* :? */ \
    )

#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_ಗಣ_O(c) \
    ((uint8_t)((c)-33)<6 ||         /* !"#$%& */ \
     (uint8_t)((c)-59)<4 ||         /* ;<=> */ \
     (uint8_t)((c)-93)<4 ||         /* ]^_` */ \
     (uint8_t)((c)-123)<3 ||        /* {|} */ \
     (c)==42 || (c)==64 || (c)==91  /* *@[ */ \
    )

#ವ್ಯಾಖ್ಯಾನಿಸು isCRLFTAB(c) ((c)==13 || (c)==10 || (c)==9)
#ವ್ಯಾಖ್ಯಾನಿಸು isCRLFSPTAB(c) ((c)==32 || (c)==13 || (c)==10 || (c)==9)

#ವ್ಯಾಖ್ಯಾನಿಸು PLUS  43
#ವ್ಯಾಖ್ಯಾನಿಸು MINUS 45
#ವ್ಯಾಖ್ಯಾನಿಸು BACKSLASH 92
#ವ್ಯಾಖ್ಯಾನಿಸು TILDE 126

/* legal byte values: all US-ASCII graphic characters from space to before tilde, and CR LF TAB */
#ವ್ಯಾಖ್ಯಾನಿಸು isLegalUTF7(c) (((uint8_t)((c)-32)<94 && (c)!=BACKSLASH) || isCRLFTAB(c))

/* encode directly sets D and O and CR LF SP TAB */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UBool encode_Directly_ಗರಿಷ್ಠ[128]={
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
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UBool encodeDirectlyRestricted[128]={
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t
ಗಮ್ಯ_Base64[64]={
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int8_t
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
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_UTF7Reset(UConverter *cnv, UConverterResetChoice choice) {
    ಆದರೆ(choice<=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        /* reset toUnicode */
        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=0x1000000; /* inDirectMode=true */
        cnv->ಗಮ್ಯ_ULength=0;
    }
    ಆದರೆ(choice!=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        /* reset fromUnicode */
        cnv->from_Unicode_ಸ್ಥಿತಿ=(cnv->from_Unicode_ಸ್ಥಿತಿ&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_UTF7Open(UConverter *cnv,
          UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    (ಶೂನ್ಯ)p_ವಾದಗಳು;
    ಆದರೆ(UCNV_GET_ಆವೃತ್ತಿ(cnv)<=1) {
        /* TODO(markus): Should just use cnv->options rather than copying the version number. */
        cnv->from_Unicode_ಸ್ಥಿತಿ=UCNV_GET_ಆವೃತ್ತಿ(cnv)<<28;
        _UTF7Reset(cnv, UCNV_RESET_BOTH);
    } ಇಲ್ಲದಿದ್ದರೆ {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UTF7_ಗಮ್ಯ_Unicode_With_Offsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    char16_t *ಗುರಿ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ;
    int32_t *offsets;

    uint8_t *ಬೈಟುಗಳು;
    uint8_t byte_ಸೂಚ್ಯಂಕ;

    int32_t length, ಗುರಿ_ಸಾಮರ್ಥ್ಯ;

    /* UTF-7 state */
    uint16_t ಬಿಟ್ಟುಗಳು;
    int8_t base64Counter;
    UBool ಒಳ_ನೇರ_ಬಹುಲಕ;

    int8_t base64_ಮೌಲ್ಯ;

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ, ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;

    uint8_t b;
    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;

    ಮೂಲ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ;
    offsets=p_ವಾದಗಳು->offsets;
    /* get the state machine state */
    {
        uint32_t ಸ್ಥಿತಿ_2=cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;
        ಒಳ_ನೇರ_ಬಹುಲಕ=(UBool)((ಸ್ಥಿತಿ_2>>24)&1);
        base64Counter=(int8_t)(ಸ್ಥಿತಿ_2>>16);
        ಬಿಟ್ಟುಗಳು=(uint16_t)ಸ್ಥಿತಿ_2;
    }
    ಬೈಟುಗಳು=cnv->ಗಮ್ಯ_UBytes;
    byte_ಸೂಚ್ಯಂಕ=cnv->ಗಮ್ಯ_ULength;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ=byte_ಸೂಚ್ಯಂಕ==0 ? 0 : -1;
    ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    ಆದರೆ(ಒಳ_ನೇರ_ಬಹುಲಕ) {
ನೇರ_ಬಹುಲಕ:
        /*
         * In Direct Mode, most US-ASCII characters are encoded directly, i.e.,
         * with their US-ASCII byte values.
         * Backslash and Tilde and most control characters are not allowed in UTF-7.
         * A plus sign starts Unicode (or "escape") Mode.
         *
         * In Direct Mode, only the sourceIndex is used.
         */
        byte_ಸೂಚ್ಯಂಕ=0;
        length=(int32_t)(ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ);
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(ಗುರಿ_ಎಲ್ಲೆ-ಗುರಿ);
        ಆದರೆ(length>ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
            length=ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
        }
        ಆಗ(length>0) {
            b=*ಮೂಲ++;
            ಆದರೆ(!isLegalUTF7(b)) {
                /* illegal */
                ಬೈಟುಗಳು[0]=b;
                byte_ಸೂಚ್ಯಂಕ=1;
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b!=PLUS) {
                /* write directly encoded character */
                *ಗುರಿ++=b;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* PLUS */ {
                /* switch to Unicode mode */
                ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ=++ಮೂಲ_ಸೂಚ್ಯಂಕ;
                ಒಳ_ನೇರ_ಬಹುಲಕ=ಸುಳ್ಳು;
                byte_ಸೂಚ್ಯಂಕ=0;
                ಬಿಟ್ಟುಗಳು=0;
                base64Counter=-1;
                ಹೋಗು unicode_ಬಹುಲಕ;
            }
            --length;
        }
        ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ>=ಗುರಿ_ಎಲ್ಲೆ) {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
unicode_ಬಹುಲಕ:
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
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                ಬೈಟುಗಳು[byte_ಸೂಚ್ಯಂಕ++]=b=*ಮೂಲ++;
                ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                base64_ಮೌಲ್ಯ = -3; /* initialize as illegal */
                ಆದರೆ(b>=126 || (base64_ಮೌಲ್ಯ=fromBase64[b])==-3 || base64_ಮೌಲ್ಯ==-1) {
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
                    ಒಳ_ನೇರ_ಬಹುಲಕ=ನಿಜ;
                    ಆದರೆ(base64Counter==-1) {
                        /* illegal: + immediately followed by something other than base64 or minus sign */
                        /* include the plus sign in the reported sequence, but not the subsequent char */
                        --ಮೂಲ;
                        ಬೈಟುಗಳು[0]=PLUS;
                        byte_ಸೂಚ್ಯಂಕ=1;
                        *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ನಿಲ್ಲಿಸು;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಬಿಟ್ಟುಗಳು!=0) {
                        /* bits are illegally left over, a char16_t is incomplete */
                        /* don't include current char (legal or illegal) in error seq */
                        --ಮೂಲ;
                        --byte_ಸೂಚ್ಯಂಕ;
                        *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ನಿಲ್ಲಿಸು;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* previous char16_t was complete */
                        ಆದರೆ(base64_ಮೌಲ್ಯ==-3) {
                            /* current character is illegal, deal with it here */
                            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            ನಿಲ್ಲಿಸು;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* un-read the current character in case it is a plus sign */
                            --ಮೂಲ;
                            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                            ಹೋಗು ನೇರ_ಬಹುಲಕ;
                        }
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(base64_ಮೌಲ್ಯ>=0) {
                    /* collect base64 bytes into UChars */
                    ಆಯ್ಕೆ(base64Counter) {
                    ಸ್ಥಿತಿ -1: /* -1 is immediately after the + */
                    ಸ್ಥಿತಿ 0:
                        ಬಿಟ್ಟುಗಳು=base64_ಮೌಲ್ಯ;
                        base64Counter=1;
                        ನಿಲ್ಲಿಸು;
                    ಸ್ಥಿತಿ 1:
                    ಸ್ಥಿತಿ 3:
                    ಸ್ಥಿತಿ 4:
                    ಸ್ಥಿತಿ 6:
                        ಬಿಟ್ಟುಗಳು=(uint16_t)((ಬಿಟ್ಟುಗಳು<<6)|base64_ಮೌಲ್ಯ);
                        ++base64Counter;
                        ನಿಲ್ಲಿಸು;
                    ಸ್ಥಿತಿ 2:
                        *ಗುರಿ++=(char16_t)((ಬಿಟ್ಟುಗಳು<<4)|(base64_ಮೌಲ್ಯ>>2));
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                        }
                        ಬೈಟುಗಳು[0]=b; /* keep this byte in case an error occurs */
                        byte_ಸೂಚ್ಯಂಕ=1;
                        ಬಿಟ್ಟುಗಳು=(uint16_t)(base64_ಮೌಲ್ಯ&3);
                        base64Counter=3;
                        ನಿಲ್ಲಿಸು;
                    ಸ್ಥಿತಿ 5:
                        *ಗುರಿ++=(char16_t)((ಬಿಟ್ಟುಗಳು<<2)|(base64_ಮೌಲ್ಯ>>4));
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                        }
                        ಬೈಟುಗಳು[0]=b; /* keep this byte in case an error occurs */
                        byte_ಸೂಚ್ಯಂಕ=1;
                        ಬಿಟ್ಟುಗಳು=(uint16_t)(base64_ಮೌಲ್ಯ&15);
                        base64Counter=6;
                        ನಿಲ್ಲಿಸು;
                    ಸ್ಥಿತಿ 7:
                        *ಗುರಿ++=(char16_t)((ಬಿಟ್ಟುಗಳು<<6)|base64_ಮೌಲ್ಯ);
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        }
                        byte_ಸೂಚ್ಯಂಕ=0;
                        ಬಿಟ್ಟುಗಳು=0;
                        base64Counter=0;
                        ನಿಲ್ಲಿಸು;
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        /* will never occur */
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ /*base64Value==-2*/ {
                    /* minus sign terminates the base64 sequence */
                    ಒಳ_ನೇರ_ಬಹುಲಕ=ನಿಜ;
                    ಆದರೆ(base64Counter==-1) {
                        /* +- i.e. a minus immediately following a plus */
                        *ಗುರಿ++=PLUS;
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* absorb the minus and leave the Unicode Mode */
                        ಆದರೆ(ಬಿಟ್ಟುಗಳು!=0) {
                            /* bits are illegally left over, a char16_t is incomplete */
                            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                    ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    ಹೋಗು ನೇರ_ಬಹುಲಕ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* target is full */
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
        }
    }

    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ) && p_ವಾದಗಳು->flush && ಮೂಲ==ಮೂಲ_ಎಲ್ಲೆ && ಬಿಟ್ಟುಗಳು==0) {
        /*
         * if we are in Unicode mode, then the byteIndex might not be 0,
         * but that is ok if bits==0
         * -> we set byteIndex=0 at the end of the stream to avoid a truncated error
         * (not true for IMAP-mailbox-name where we must end in direct mode)
         */
        byte_ಸೂಚ್ಯಂಕ=0;
    }

    /* set the converter state back into UConverter */
    cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=((uint32_t)ಒಳ_ನೇರ_ಬಹುಲಕ<<24)|((uint32_t)((uint8_t)base64Counter)<<16)|(uint32_t)ಬಿಟ್ಟುಗಳು;
    cnv->ಗಮ್ಯ_ULength=byte_ಸೂಚ್ಯಂಕ;

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_UTF7FromUnicodeWithOffsets(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ, *ಗುರಿ_ಎಲ್ಲೆ;
    int32_t *offsets;

    int32_t length, ಗುರಿ_ಸಾಮರ್ಥ್ಯ, ಮೂಲ_ಸೂಚ್ಯಂಕ;
    char16_t c;

    /* UTF-7 state */
    ಸ್ಥಿರಾಂಕ UBool *encodeDirectly;
    uint8_t ಬಿಟ್ಟುಗಳು;
    int8_t base64Counter;
    UBool ಒಳ_ನೇರ_ಬಹುಲಕ;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;

    /* set up the local pointers */
    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=(uint8_t *)p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಎಲ್ಲೆ=(uint8_t *)p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ;
    offsets=p_ವಾದಗಳು->offsets;

    /* get the state machine state */
    {
        uint32_t ಸ್ಥಿತಿ_2=cnv->from_Unicode_ಸ್ಥಿತಿ;
        encodeDirectly= ಸ್ಥಿತಿ_2<0x10000000 ? encode_Directly_ಗರಿಷ್ಠ : encodeDirectlyRestricted;
        ಒಳ_ನೇರ_ಬಹುಲಕ=(UBool)((ಸ್ಥಿತಿ_2>>24)&1);
        base64Counter=(int8_t)(ಸ್ಥಿತಿ_2>>16);
        ಬಿಟ್ಟುಗಳು=(uint8_t)ಸ್ಥಿತಿ_2;
        U_ASSERT(ಬಿಟ್ಟುಗಳು<=UPRV_LENGTHOF(ಗಮ್ಯ_Base64));
    }

    /* UTF-7 always encodes UTF-16 code units, therefore we need only a simple sourceIndex */
    ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    ಆದರೆ(ಒಳ_ನೇರ_ಬಹುಲಕ) {
ನೇರ_ಬಹುಲಕ:
        length=(int32_t)(ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ);
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(ಗುರಿ_ಎಲ್ಲೆ-ಗುರಿ);
        ಆದರೆ(length>ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
            length=ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
        }
        ಆಗ(length>0) {
            c=*ಮೂಲ++;
            /* currently always encode CR LF SP TAB directly */
            ಆದರೆ(c<=127 && encodeDirectly[c]) {
                /* encode directly */
                *ಗುರಿ++=(uint8_t)c;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==PLUS) {
                /* output +- for + */
                *ಗುರಿ++=PLUS;
                ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                    *ಗುರಿ++=MINUS;
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                    }
                    /* realign length and targetCapacity */
                    ಹೋಗು ನೇರ_ಬಹುಲಕ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                    }
                    cnv->char_Error_ಬಫರ್[0]=MINUS;
                    cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                    *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* un-read this character and switch to Unicode Mode */
                --ಮೂಲ;
                *ಗುರಿ++=PLUS;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                ಒಳ_ನೇರ_ಬಹುಲಕ=ಸುಳ್ಳು;
                base64Counter=0;
                ಹೋಗು unicode_ಬಹುಲಕ;
            }
            --length;
        }
        ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ>=ಗುರಿ_ಎಲ್ಲೆ) {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
unicode_ಬಹುಲಕ:
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                c=*ಮೂಲ++;
                ಆದರೆ(c<=127 && encodeDirectly[c]) {
                    /* encode directly */
                    ಒಳ_ನೇರ_ಬಹುಲಕ=ನಿಜ;

                    /* trick: back out this character to make this easier */
                    --ಮೂಲ;

                    /* terminate the base64 sequence */
                    ಆದರೆ(base64Counter!=0) {
                        /* write remaining bits for the previous character */
                        *ಗುರಿ++=ಗಮ್ಯ_Base64[ಬಿಟ್ಟುಗಳು];
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                        }
                    }
                    ಆದರೆ(fromBase64[c]!=-1) {
                        /* need to terminate with a minus */
                        ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                            *ಗುರಿ++=MINUS;
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            cnv->char_Error_ಬಫರ್[0]=MINUS;
                            cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                    ಹೋಗು ನೇರ_ಬಹುಲಕ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /*
                     * base64 this character:
                     * Output 2 or 3 base64 bytes for the remaining bits of the previous character
                     * and the bits of this character, each implicitly in UTF-16BE.
                     *
                     * Here, bits is an 8-bit variable because only 6 bits need to be kept from one
                     * character to the next. The actual 2 or 4 bits are shifted to the left edge
                     * of the 6-bits field 5..0 to make the termination of the base64 sequence easier.
                     */
                    ಆಯ್ಕೆ(base64Counter) {
                    ಸ್ಥಿತಿ 0:
                        *ಗುರಿ++=ಗಮ್ಯ_Base64[c>>10];
                        ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                            *ಗುರಿ++=ಗಮ್ಯ_Base64[(c>>4)&0x3f];
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                            }
                            cnv->char_Error_ಬಫರ್[0]=ಗಮ್ಯ_Base64[(c>>4)&0x3f];
                            cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                        }
                        ಬಿಟ್ಟುಗಳು=(uint8_t)((c&15)<<2);
                        base64Counter=1;
                        ನಿಲ್ಲಿಸು;
                    ಸ್ಥಿತಿ 1:
                        *ಗುರಿ++=ಗಮ್ಯ_Base64[ಬಿಟ್ಟುಗಳು|(c>>14)];
                        ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                            *ಗುರಿ++=ಗಮ್ಯ_Base64[(c>>8)&0x3f];
                            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                                *ಗುರಿ++=ಗಮ್ಯ_Base64[(c>>2)&0x3f];
                                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                                }
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                                }
                                cnv->char_Error_ಬಫರ್[0]=ಗಮ್ಯ_Base64[(c>>2)&0x3f];
                                cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                            }
                            cnv->char_Error_ಬಫರ್[0]=ಗಮ್ಯ_Base64[(c>>8)&0x3f];
                            cnv->char_Error_ಬಫರ್[1]=ಗಮ್ಯ_Base64[(c>>2)&0x3f];
                            cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=2;
                            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                        }
                        ಬಿಟ್ಟುಗಳು=(uint8_t)((c&3)<<4);
                        base64Counter=2;
                        ನಿಲ್ಲಿಸು;
                    ಸ್ಥಿತಿ 2:
                        *ಗುರಿ++=ಗಮ್ಯ_Base64[ಬಿಟ್ಟುಗಳು|(c>>12)];
                        ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                            *ಗುರಿ++=ಗಮ್ಯ_Base64[(c>>6)&0x3f];
                            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                                *ಗುರಿ++=ಗಮ್ಯ_Base64[c&0x3f];
                                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                                }
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                                }
                                cnv->char_Error_ಬಫರ್[0]=ಗಮ್ಯ_Base64[c&0x3f];
                                cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                            }
                            cnv->char_Error_ಬಫರ್[0]=ಗಮ್ಯ_Base64[(c>>6)&0x3f];
                            cnv->char_Error_ಬಫರ್[1]=ಗಮ್ಯ_Base64[c&0x3f];
                            cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=2;
                            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                        }
                        ಬಿಟ್ಟುಗಳು=0;
                        base64Counter=0;
                        ನಿಲ್ಲಿಸು;
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        /* will never occur */
                        ನಿಲ್ಲಿಸು;
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* target is full */
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
        }
    }

    ಆದರೆ(p_ವಾದಗಳು->flush && ಮೂಲ>=ಮೂಲ_ಎಲ್ಲೆ) {
        /* flush remaining bits to the target */
        ಆದರೆ(!ಒಳ_ನೇರ_ಬಹುಲಕ) {
            ಆದರೆ (base64Counter!=0) {
                ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                    *ಗುರಿ++=ಗಮ್ಯ_Base64[ಬಿಟ್ಟುಗಳು];
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    cnv->char_Error_ಬಫರ್[cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++]=ಗಮ್ಯ_Base64[ಬಿಟ್ಟುಗಳು];
                    *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                }
            }
            /* Add final MINUS to terminate unicodeMode */
            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                *ಗುರಿ++=MINUS;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                cnv->char_Error_ಬಫರ್[cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++]=MINUS;
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            }
        }
        /* reset the state for the next conversion */
        cnv->from_Unicode_ಸ್ಥಿತಿ=(cnv->from_Unicode_ಸ್ಥಿತಿ&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* set the converter state back into UConverter */
        cnv->from_Unicode_ಸ್ಥಿತಿ=
            (cnv->from_Unicode_ಸ್ಥಿತಿ&0xf0000000)|    /* keep version*/
            ((uint32_t)ಒಳ_ನೇರ_ಬಹುಲಕ<<24)|((uint32_t)base64Counter<<16)|(uint32_t)ಬಿಟ್ಟುಗಳು;
    }

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=(ಅಕ್ಷರ *)ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_CALLCONV
_UTF7GetName(ಸ್ಥಿರಾಂಕ UConverter *cnv) {
    ಆಯ್ಕೆ(cnv->from_Unicode_ಸ್ಥಿತಿ>>28) {
    ಸ್ಥಿತಿ 1:
        ಹಿಂತಿರುಗಿ "UTF-7,version=1";
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ "UTF-7";
    }
}
U_CDECL_ಕೊನೆ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _UTF7Impl={
    UCNV_UTF7,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _UTF7Open,
    ಶೂನ್ಯಸೂಚಕ,
    _UTF7Reset,

    UTF7_ಗಮ್ಯ_Unicode_With_Offsets,
    UTF7_ಗಮ್ಯ_Unicode_With_Offsets,
    _UTF7FromUnicodeWithOffsets,
    _UTF7FromUnicodeWithOffsets,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    _UTF7GetName,
    ಶೂನ್ಯಸೂಚಕ, /* we don't need writeSub() because we never call a callback at fromUnicode() */
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_get_Complete_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ UTF7_Static_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "UTF-7",
    0, /* TODO CCSID for UTF-7 */
    UCNV_IBM, UCNV_UTF7,
    1, 4,
    { 0x3f, 0, 0, 0 }, 1, /* the subchar is not used */
    ಸುಳ್ಳು, ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ UTF7_ದತ್ತಾಂಶ=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&UTF7_Static_ದತ್ತಾಂಶ, &_UTF7Impl);

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
#ವ್ಯಾಖ್ಯಾನಿಸು AMPERSAND 0x26
#ವ್ಯಾಖ್ಯಾನಿಸು COMMA 0x2c
#ವ್ಯಾಖ್ಯಾನಿಸು SLASH 0x2f

/* legal byte values: all US-ASCII graphic characters 0x20..0x7e */
#ವ್ಯಾಖ್ಯಾನಿಸು isLegalIMAP(c) (0x20<=(c) && (c)<=0x7e)

/* direct-encode all of printable ASCII 0x20..0x7e except '&' 0x26 */
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳ_ಗಣ_DIMAP(c) (isLegalIMAP(c) && c!=AMPERSAND)

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಮ್ಯ_BASE64_IMAP(n) ((n)<63 ? ಗಮ್ಯ_Base64[n] : COMMA)
#ವ್ಯಾಖ್ಯಾನಿಸು FROM_BASE64_IMAP(c) ((c)==COMMA ? 63 : (c)==SLASH ? -1 : fromBase64[c])

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
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_IMAPToUnicodeWithOffsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    char16_t *ಗುರಿ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ;
    int32_t *offsets;

    uint8_t *ಬೈಟುಗಳು;
    uint8_t byte_ಸೂಚ್ಯಂಕ;

    int32_t length, ಗುರಿ_ಸಾಮರ್ಥ್ಯ;

    /* UTF-7 state */
    uint16_t ಬಿಟ್ಟುಗಳು;
    int8_t base64Counter;
    UBool ಒಳ_ನೇರ_ಬಹುಲಕ;

    int8_t base64_ಮೌಲ್ಯ;

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ, ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;

    char16_t c;
    uint8_t b;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;

    ಮೂಲ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ;
    offsets=p_ವಾದಗಳು->offsets;
    /* get the state machine state */
    {
        uint32_t ಸ್ಥಿತಿ_2=cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ;
        ಒಳ_ನೇರ_ಬಹುಲಕ=(UBool)((ಸ್ಥಿತಿ_2>>24)&1);
        base64Counter=(int8_t)(ಸ್ಥಿತಿ_2>>16);
        ಬಿಟ್ಟುಗಳು=(uint16_t)ಸ್ಥಿತಿ_2;
    }
    ಬೈಟುಗಳು=cnv->ಗಮ್ಯ_UBytes;
    byte_ಸೂಚ್ಯಂಕ=cnv->ಗಮ್ಯ_ULength;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ=byte_ಸೂಚ್ಯಂಕ==0 ? 0 : -1;
    ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    ಆದರೆ(ಒಳ_ನೇರ_ಬಹುಲಕ) {
ನೇರ_ಬಹುಲಕ:
        /*
         * In Direct Mode, US-ASCII characters are encoded directly, i.e.,
         * with their US-ASCII byte values.
         * An ampersand starts Unicode (or "escape") Mode.
         *
         * In Direct Mode, only the sourceIndex is used.
         */
        byte_ಸೂಚ್ಯಂಕ=0;
        length=(int32_t)(ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ);
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(ಗುರಿ_ಎಲ್ಲೆ-ಗುರಿ);
        ಆದರೆ(length>ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
            length=ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
        }
        ಆಗ(length>0) {
            b=*ಮೂಲ++;
            ಆದರೆ(!isLegalIMAP(b)) {
                /* illegal */
                ಬೈಟುಗಳು[0]=b;
                byte_ಸೂಚ್ಯಂಕ=1;
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b!=AMPERSAND) {
                /* write directly encoded character */
                *ಗುರಿ++=b;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* AMPERSAND */ {
                /* switch to Unicode mode */
                ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ=++ಮೂಲ_ಸೂಚ್ಯಂಕ;
                ಒಳ_ನೇರ_ಬಹುಲಕ=ಸುಳ್ಳು;
                byte_ಸೂಚ್ಯಂಕ=0;
                ಬಿಟ್ಟುಗಳು=0;
                base64Counter=-1;
                ಹೋಗು unicode_ಬಹುಲಕ;
            }
            --length;
        }
        ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ>=ಗುರಿ_ಎಲ್ಲೆ) {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
unicode_ಬಹುಲಕ:
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
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                ಬೈಟುಗಳು[byte_ಸೂಚ್ಯಂಕ++]=b=*ಮೂಲ++;
                ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                ಆದರೆ(b>0x7e) {
                    /* illegal - test other illegal US-ASCII values by base64Value==-3 */
                    ಒಳ_ನೇರ_ಬಹುಲಕ=ನಿಜ;
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((base64_ಮೌಲ್ಯ=FROM_BASE64_IMAP(b))>=0) {
                    /* collect base64 bytes into UChars */
                    ಆಯ್ಕೆ(base64Counter) {
                    ಸ್ಥಿತಿ -1: /* -1 is immediately after the & */
                    ಸ್ಥಿತಿ 0:
                        ಬಿಟ್ಟುಗಳು=base64_ಮೌಲ್ಯ;
                        base64Counter=1;
                        ನಿಲ್ಲಿಸು;
                    ಸ್ಥಿತಿ 1:
                    ಸ್ಥಿತಿ 3:
                    ಸ್ಥಿತಿ 4:
                    ಸ್ಥಿತಿ 6:
                        ಬಿಟ್ಟುಗಳು=(uint16_t)((ಬಿಟ್ಟುಗಳು<<6)|base64_ಮೌಲ್ಯ);
                        ++base64Counter;
                        ನಿಲ್ಲಿಸು;
                    ಸ್ಥಿತಿ 2:
                        c=(char16_t)((ಬಿಟ್ಟುಗಳು<<4)|(base64_ಮೌಲ್ಯ>>2));
                        ಆದರೆ(isLegalIMAP(c)) {
                            /* illegal */
                            ಒಳ_ನೇರ_ಬಹುಲಕ=ನಿಜ;
                            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            ಹೋಗು endloop;
                        }
                        *ಗುರಿ++=c;
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                        }
                        ಬೈಟುಗಳು[0]=b; /* keep this byte in case an error occurs */
                        byte_ಸೂಚ್ಯಂಕ=1;
                        ಬಿಟ್ಟುಗಳು=(uint16_t)(base64_ಮೌಲ್ಯ&3);
                        base64Counter=3;
                        ನಿಲ್ಲಿಸು;
                    ಸ್ಥಿತಿ 5:
                        c=(char16_t)((ಬಿಟ್ಟುಗಳು<<2)|(base64_ಮೌಲ್ಯ>>4));
                        ಆದರೆ(isLegalIMAP(c)) {
                            /* illegal */
                            ಒಳ_ನೇರ_ಬಹುಲಕ=ನಿಜ;
                            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            ಹೋಗು endloop;
                        }
                        *ಗುರಿ++=c;
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                        }
                        ಬೈಟುಗಳು[0]=b; /* keep this byte in case an error occurs */
                        byte_ಸೂಚ್ಯಂಕ=1;
                        ಬಿಟ್ಟುಗಳು=(uint16_t)(base64_ಮೌಲ್ಯ&15);
                        base64Counter=6;
                        ನಿಲ್ಲಿಸು;
                    ಸ್ಥಿತಿ 7:
                        c=(char16_t)((ಬಿಟ್ಟುಗಳು<<6)|base64_ಮೌಲ್ಯ);
                        ಆದರೆ(isLegalIMAP(c)) {
                            /* illegal */
                            ಒಳ_ನೇರ_ಬಹುಲಕ=ನಿಜ;
                            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            ಹೋಗು endloop;
                        }
                        *ಗುರಿ++=c;
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        }
                        byte_ಸೂಚ್ಯಂಕ=0;
                        ಬಿಟ್ಟುಗಳು=0;
                        base64Counter=0;
                        ನಿಲ್ಲಿಸು;
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        /* will never occur */
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(base64_ಮೌಲ್ಯ==-2) {
                    /* minus sign terminates the base64 sequence */
                    ಒಳ_ನೇರ_ಬಹುಲಕ=ನಿಜ;
                    ಆದರೆ(base64Counter==-1) {
                        /* &- i.e. a minus immediately following an ampersand */
                        *ಗುರಿ++=AMPERSAND;
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* absorb the minus and leave the Unicode Mode */
                        ಆದರೆ(ಬಿಟ್ಟುಗಳು!=0 || (base64Counter!=0 && base64Counter!=3 && base64Counter!=6)) {
                            /* bits are illegally left over, a char16_t is incomplete */
                            /* base64Counter other than 0, 3, 6 means non-minimal zero-padding, also illegal */
                            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                    ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    ಹೋಗು ನೇರ_ಬಹುಲಕ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ(base64Counter==-1) {
                        /* illegal: & immediately followed by something other than base64 or minus sign */
                        /* include the ampersand in the reported sequence */
                        --ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        ಬೈಟುಗಳು[0]=AMPERSAND;
                        ಬೈಟುಗಳು[1]=b;
                        byte_ಸೂಚ್ಯಂಕ=2;
                    }
                    /* base64Value==-1 for characters that are illegal only in Unicode mode */
                    /* base64Value==-3 for illegal characters */
                    /* illegal */
                    ಒಳ_ನೇರ_ಬಹುಲಕ=ನಿಜ;
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* target is full */
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
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
    ಆದರೆ( U_SUCCESS(*p_Error_ಸಂಕೇತ) &&
        !ಒಳ_ನೇರ_ಬಹುಲಕ && byte_ಸೂಚ್ಯಂಕ==0 &&
        p_ವಾದಗಳು->flush && ಮೂಲ>=ಮೂಲ_ಎಲ್ಲೆ
    ) {
        ಆದರೆ(base64Counter==-1) {
            /* & at the very end of the input */
            /* make the ampersand the reported sequence */
            ಬೈಟುಗಳು[0]=AMPERSAND;
            byte_ಸೂಚ್ಯಂಕ=1;
        }
        /* else if(base64Counter!=-1) byteIndex remains 0 because there is no particular byte sequence */

        ಒಳ_ನೇರ_ಬಹುಲಕ=ನಿಜ; /* avoid looping */
        *p_Error_ಸಂಕೇತ=U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು;
    }

    /* set the converter state back into UConverter */
    cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=((uint32_t)ಒಳ_ನೇರ_ಬಹುಲಕ<<24)|((uint32_t)((uint8_t)base64Counter)<<16)|(uint32_t)ಬಿಟ್ಟುಗಳು;
    cnv->ಗಮ್ಯ_ULength=byte_ಸೂಚ್ಯಂಕ;

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_IMAPFromUnicodeWithOffsets(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ, *ಗುರಿ_ಎಲ್ಲೆ;
    int32_t *offsets;

    int32_t length, ಗುರಿ_ಸಾಮರ್ಥ್ಯ, ಮೂಲ_ಸೂಚ್ಯಂಕ;
    char16_t c;
    uint8_t b;

    /* UTF-7 state */
    uint8_t ಬಿಟ್ಟುಗಳು;
    int8_t base64Counter;
    UBool ಒಳ_ನೇರ_ಬಹುಲಕ;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;

    /* set up the local pointers */
    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=(uint8_t *)p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಎಲ್ಲೆ=(uint8_t *)p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ;
    offsets=p_ವಾದಗಳು->offsets;

    /* get the state machine state */
    {
        uint32_t ಸ್ಥಿತಿ_2=cnv->from_Unicode_ಸ್ಥಿತಿ;
        ಒಳ_ನೇರ_ಬಹುಲಕ=(UBool)((ಸ್ಥಿತಿ_2>>24)&1);
        base64Counter=(int8_t)(ಸ್ಥಿತಿ_2>>16);
        ಬಿಟ್ಟುಗಳು=(uint8_t)ಸ್ಥಿತಿ_2;
    }

    /* UTF-7 always encodes UTF-16 code units, therefore we need only a simple sourceIndex */
    ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    ಆದರೆ(ಒಳ_ನೇರ_ಬಹುಲಕ) {
ನೇರ_ಬಹುಲಕ:
        length=(int32_t)(ಮೂಲ_ಎಲ್ಲೆ-ಮೂಲ);
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(ಗುರಿ_ಎಲ್ಲೆ-ಗುರಿ);
        ಆದರೆ(length>ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
            length=ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
        }
        ಆಗ(length>0) {
            c=*ಮೂಲ++;
            /* encode 0x20..0x7e except '&' directly */
            ಆದರೆ(ಒಳ_ಗಣ_DIMAP(c)) {
                /* encode directly */
                *ಗುರಿ++=(uint8_t)c;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==AMPERSAND) {
                /* output &- for & */
                *ಗುರಿ++=AMPERSAND;
                ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                    *ಗುರಿ++=MINUS;
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                    }
                    /* realign length and targetCapacity */
                    ಹೋಗು ನೇರ_ಬಹುಲಕ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                    }
                    cnv->char_Error_ಬಫರ್[0]=MINUS;
                    cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                    *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* un-read this character and switch to Unicode Mode */
                --ಮೂಲ;
                *ಗುರಿ++=AMPERSAND;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                ಒಳ_ನೇರ_ಬಹುಲಕ=ಸುಳ್ಳು;
                base64Counter=0;
                ಹೋಗು unicode_ಬಹುಲಕ;
            }
            --length;
        }
        ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ>=ಗುರಿ_ಎಲ್ಲೆ) {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
unicode_ಬಹುಲಕ:
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                c=*ಮೂಲ++;
                ಆದರೆ(isLegalIMAP(c)) {
                    /* encode directly */
                    ಒಳ_ನೇರ_ಬಹುಲಕ=ನಿಜ;

                    /* trick: back out this character to make this easier */
                    --ಮೂಲ;

                    /* terminate the base64 sequence */
                    ಆದರೆ(base64Counter!=0) {
                        /* write remaining bits for the previous character */
                        *ಗುರಿ++=ಗಮ್ಯ_BASE64_IMAP(ಬಿಟ್ಟುಗಳು);
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                        }
                    }
                    /* need to terminate with a minus */
                    ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                        *ಗುರಿ++=MINUS;
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        cnv->char_Error_ಬಫರ್[0]=MINUS;
                        cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                        ನಿಲ್ಲಿಸು;
                    }
                    ಹೋಗು ನೇರ_ಬಹುಲಕ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /*
                     * base64 this character:
                     * Output 2 or 3 base64 bytes for the remaining bits of the previous character
                     * and the bits of this character, each implicitly in UTF-16BE.
                     *
                     * Here, bits is an 8-bit variable because only 6 bits need to be kept from one
                     * character to the next. The actual 2 or 4 bits are shifted to the left edge
                     * of the 6-bits field 5..0 to make the termination of the base64 sequence easier.
                     */
                    ಆಯ್ಕೆ(base64Counter) {
                    ಸ್ಥಿತಿ 0:
                        b=(uint8_t)(c>>10);
                        *ಗುರಿ++=ಗಮ್ಯ_BASE64_IMAP(b);
                        ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                            b=(uint8_t)((c>>4)&0x3f);
                            *ಗುರಿ++=ಗಮ್ಯ_BASE64_IMAP(b);
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                            }
                            b=(uint8_t)((c>>4)&0x3f);
                            cnv->char_Error_ಬಫರ್[0]=ಗಮ್ಯ_BASE64_IMAP(b);
                            cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                        }
                        ಬಿಟ್ಟುಗಳು=(uint8_t)((c&15)<<2);
                        base64Counter=1;
                        ನಿಲ್ಲಿಸು;
                    ಸ್ಥಿತಿ 1:
                        b=(uint8_t)(ಬಿಟ್ಟುಗಳು|(c>>14));
                        *ಗುರಿ++=ಗಮ್ಯ_BASE64_IMAP(b);
                        ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                            b=(uint8_t)((c>>8)&0x3f);
                            *ಗುರಿ++=ಗಮ್ಯ_BASE64_IMAP(b);
                            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                                b=(uint8_t)((c>>2)&0x3f);
                                *ಗುರಿ++=ಗಮ್ಯ_BASE64_IMAP(b);
                                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                                }
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                                }
                                b=(uint8_t)((c>>2)&0x3f);
                                cnv->char_Error_ಬಫರ್[0]=ಗಮ್ಯ_BASE64_IMAP(b);
                                cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                            }
                            b=(uint8_t)((c>>8)&0x3f);
                            cnv->char_Error_ಬಫರ್[0]=ಗಮ್ಯ_BASE64_IMAP(b);
                            b=(uint8_t)((c>>2)&0x3f);
                            cnv->char_Error_ಬಫರ್[1]=ಗಮ್ಯ_BASE64_IMAP(b);
                            cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=2;
                            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                        }
                        ಬಿಟ್ಟುಗಳು=(uint8_t)((c&3)<<4);
                        base64Counter=2;
                        ನಿಲ್ಲಿಸು;
                    ಸ್ಥಿತಿ 2:
                        b=(uint8_t)(ಬಿಟ್ಟುಗಳು|(c>>12));
                        *ಗುರಿ++=ಗಮ್ಯ_BASE64_IMAP(b);
                        ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                            b=(uint8_t)((c>>6)&0x3f);
                            *ಗುರಿ++=ಗಮ್ಯ_BASE64_IMAP(b);
                            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                                b=(uint8_t)(c&0x3f);
                                *ಗುರಿ++=ಗಮ್ಯ_BASE64_IMAP(b);
                                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                                }
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                                }
                                b=(uint8_t)(c&0x3f);
                                cnv->char_Error_ಬಫರ್[0]=ಗಮ್ಯ_BASE64_IMAP(b);
                                cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                            }
                            b=(uint8_t)((c>>6)&0x3f);
                            cnv->char_Error_ಬಫರ್[0]=ಗಮ್ಯ_BASE64_IMAP(b);
                            b=(uint8_t)(c&0x3f);
                            cnv->char_Error_ಬಫರ್[1]=ಗಮ್ಯ_BASE64_IMAP(b);
                            cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=2;
                            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                        }
                        ಬಿಟ್ಟುಗಳು=0;
                        base64Counter=0;
                        ನಿಲ್ಲಿಸು;
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        /* will never occur */
                        ನಿಲ್ಲಿಸು;
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* target is full */
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
        }
    }

    ಆದರೆ(p_ವಾದಗಳು->flush && ಮೂಲ>=ಮೂಲ_ಎಲ್ಲೆ) {
        /* flush remaining bits to the target */
        ಆದರೆ(!ಒಳ_ನೇರ_ಬಹುಲಕ) {
            ಆದರೆ(base64Counter!=0) {
                ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                    *ಗುರಿ++=ಗಮ್ಯ_BASE64_IMAP(ಬಿಟ್ಟುಗಳು);
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    cnv->char_Error_ಬಫರ್[cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++]=ಗಮ್ಯ_BASE64_IMAP(ಬಿಟ್ಟುಗಳು);
                    *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                }
            }
            /* need to terminate with a minus */
            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                *ಗುರಿ++=MINUS;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                cnv->char_Error_ಬಫರ್[cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++]=MINUS;
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            }
        }
        /* reset the state for the next conversion */
        cnv->from_Unicode_ಸ್ಥಿತಿ=(cnv->from_Unicode_ಸ್ಥಿತಿ&0xf0000000)|0x1000000; /* keep version, inDirectMode=true */
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* set the converter state back into UConverter */
        cnv->from_Unicode_ಸ್ಥಿತಿ=
            (cnv->from_Unicode_ಸ್ಥಿತಿ&0xf0000000)|    /* keep version*/
            ((uint32_t)ಒಳ_ನೇರ_ಬಹುಲಕ<<24)|((uint32_t)base64Counter<<16)|(uint32_t)ಬಿಟ್ಟುಗಳು;
    }

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=(ಅಕ್ಷರ *)ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}
U_CDECL_ಕೊನೆ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _IMAPImpl={
    UCNV_IMAP_MAILBOX,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _UTF7Open,
    ಶೂನ್ಯಸೂಚಕ,
    _UTF7Reset,

    _IMAPToUnicodeWithOffsets,
    _IMAPToUnicodeWithOffsets,
    _IMAPFromUnicodeWithOffsets,
    _IMAPFromUnicodeWithOffsets,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ, /* we don't need writeSub() because we never call a callback at fromUnicode() */
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_get_Complete_Unicode_ಗಣ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ IMAPStatic_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "IMAP-mailbox-name",
    0, /* TODO CCSID for IMAP-mailbox-name */
    UCNV_IBM, UCNV_IMAP_MAILBOX,
    1, 4,
    { 0x3f, 0, 0, 0 }, 1, /* the subchar is not used */
    ಸುಳ್ಳು, ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _IMAPData=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&IMAPStatic_ದತ್ತಾಂಶ, &_IMAPImpl);

#ಆದರೆ_ಅಂತ್ಯ
