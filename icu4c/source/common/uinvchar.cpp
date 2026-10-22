// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1999-2010, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uinvchar.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:2
*
*   created on: 2004sep14
*   created by: Markus W. Scherer
*
*   Functions for handling invariant characters, moved here from putil.c
*   for better modularization.
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "udataswp.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "uinvchar.h"

/* invariant-character handling --------------------------------------------- */

/*
 * These maps for ASCII to/from EBCDIC map invariant characters (see utypes.h)
 * appropriately for most EBCDIC codepages.
 *
 * They currently also map most other ASCII graphic characters,
 * appropriately for codepages 37 and 1047.
 * Exceptions: The characters for []^ have different codes in 37 & 1047.
 * Both versions are mapped to ASCII.
 *
 *    ASCII 37 1047
 * [     5B BA   AD
 * ]     5D BB   BD
 * ^     5E B0   5F
 *
 * There are no mappings for variant characters from Unicode to EBCDIC.
 *
 * Currently, C0 control codes are also included in these maps.
 * Exceptions: S/390 Open Edition swaps LF and NEL codes compared with other
 * EBCDIC platforms; both codes (15 and 25) are mapped to ASCII LF (0A),
 * but there is no mapping for ASCII LF back to EBCDIC.
 *
 *    ASCII EBCDIC S/390-OE
 * LF    0A     25       15
 * NEL   85     15       25
 *
 * The maps below explicitly exclude the variant
 * control and graphical characters that are in ASCII-based
 * codepages at 0x80 and above.
 * "No mapping" is expressed by mapping to a 00 byte.
 *
 * These tables do not establish a converter or a codepage.
 */

స్థిర స్థిరరాశి uint8_t asciiFromEbcdic[256]={
    0x00, 0x01, 0x02, 0x03, 0x00, 0x09, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x00, 0x0a, 0x08, 0x00, 0x18, 0x19, 0x00, 0x00, 0x1c, 0x1d, 0x1e, 0x1f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x17, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x06, 0x07,
    0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x14, 0x15, 0x00, 0x1a,

    0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2e, 0x3c, 0x28, 0x2b, 0x7c,
    0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x24, 0x2a, 0x29, 0x3b, 0x5e,
    0x2d, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x25, 0x5f, 0x3e, 0x3f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x3a, 0x23, 0x40, 0x27, 0x3d, 0x22,

    0x00, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x7e, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x00, 0x00, 0x00, 0x5b, 0x00, 0x00,
    0x5e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5b, 0x5d, 0x00, 0x5d, 0x00, 0x00,

    0x7b, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x7d, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x5c, 0x00, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

స్థిర స్థిరరాశి uint8_t ebcdicFromAscii[256]={
    0x00, 0x01, 0x02, 0x03, 0x37, 0x2d, 0x2e, 0x2f, 0x16, 0x05, 0x00, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x3c, 0x3d, 0x32, 0x26, 0x18, 0x19, 0x3f, 0x27, 0x1c, 0x1d, 0x1e, 0x1f,
    0x40, 0x00, 0x7f, 0x00, 0x00, 0x6c, 0x50, 0x7d, 0x4d, 0x5d, 0x5c, 0x4e, 0x6b, 0x60, 0x4b, 0x61,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0x7a, 0x5e, 0x4c, 0x7e, 0x6e, 0x6f,

    0x00, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6,
    0xd7, 0xd8, 0xd9, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0x00, 0x00, 0x00, 0x00, 0x6d,
    0x00, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
    0x97, 0x98, 0x99, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0x00, 0x00, 0x00, 0x00, 0x07,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Same as asciiFromEbcdic[] except maps all letters to lowercase. */
స్థిర స్థిరరాశి uint8_t lowercaseAsciiFromEbcdic[256]={
    0x00, 0x01, 0x02, 0x03, 0x00, 0x09, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x00, 0x0a, 0x08, 0x00, 0x18, 0x19, 0x00, 0x00, 0x1c, 0x1d, 0x1e, 0x1f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x17, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x06, 0x07,
    0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x14, 0x15, 0x00, 0x1a,

    0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2e, 0x3c, 0x28, 0x2b, 0x7c,
    0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x24, 0x2a, 0x29, 0x3b, 0x5e,
    0x2d, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x25, 0x5f, 0x3e, 0x3f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x3a, 0x23, 0x40, 0x27, 0x3d, 0x22,

    0x00, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x7e, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x00, 0x00, 0x00, 0x5b, 0x00, 0x00,
    0x5e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5b, 0x5d, 0x00, 0x5d, 0x00, 0x00,

    0x7b, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x7d, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x7c, 0x00, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*
 * Bit sets indicating which characters of the ASCII repertoire
 * (by ASCII/Unicode code) are "invariant".
 * See utypes.h for more details.
 *
 * As invariant are considered the characters of the ASCII repertoire except
 * for the following:
 * 21  '!' <exclamation mark>
 * 23  '#' <number sign>
 * 24  '$' <dollar sign>
 *
 * 40  '@' <commercial at>
 *
 * 5b  '[' <left bracket>
 * 5c  '\' <backslash>
 * 5d  ']' <right bracket>
 * 5e  '^' <circumflex>
 *
 * 60  '`' <grave accent>
 *
 * 7b  '{' <left brace>
 * 7c  '|' <vertical line>
 * 7d  '}' <right brace>
 * 7e  '~' <tilde>
 */
స్థిర స్థిరరాశి uint32_t invariantChars[4]={
    0xfffffbff, /* 00..1f but not 0a */
    0xffffffe5, /* 20..3f but not 21 23 24 */
    0x87fffffe, /* 40..5f but not 40 5b..5e */
    0x87fffffe  /* 60..7f but not 60 7b..7e */
};

/*
 * test unsigned types (or values known to be non-negative) for invariant characters,
 * tests ASCII-family character values
 */
#నిర్వచించు UCHAR_IS_INVARIANT(c) (((c)<=0x7f) && (invariantChars[(c)>>5]&((uint32_t)1<<((c)&0x1f)))!=0)

/* test signed types for invariant characters, adds test for positive values */
#నిర్వచించు SCHAR_IS_INVARIANT(c) ((0<=(c)) && UCHAR_IS_INVARIANT(c))

#ఒకవేళ U_CHARSET_FAMILY==U_ASCII_FAMILY
#నిర్వచించు CHAR_గమ్యం_UCHAR(c) c
#నిర్వచించు UCHAR_గమ్యం_CHAR(c) c
#లేదావేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#నిర్వచించు CHAR_గమ్యం_UCHAR(u) asciiFromEbcdic[u]
#నిర్వచించు UCHAR_గమ్యం_CHAR(u) ebcdicFromAscii[u]
#లేకపోతే
#   error U_CHARSET_FAMILY is not valid
#అయితే_ముగింపు


U_CAPI శూన్య U_EXPORT2
u_chars_గమ్యం_UChars(స్థిరరాశి అక్షరం *cs, char16_t *us, int32_t length) {
    char16_t u;
    uint8_t c;

    /*
     * Allow the entire ASCII repertoire to be mapped _to_ Unicode.
     * For EBCDIC systems, this works for characters with codes from
     * codepages 37 and 1047 or compatible.
     */
    అయితే(length>0) {
        c=(uint8_t)(*cs++);
        u=(char16_t)CHAR_గమ్యం_UCHAR(c);
        U_ASSERT((u!=0 || c==0)); /* only invariant chars converted? */
        *us++=u;
        --length;
    }
}

U_CAPI శూన్య U_EXPORT2
u_UChars_గమ్యం_Chars(స్థిరరాశి char16_t *us, అక్షరం *cs, int32_t length) {
    char16_t u;

    అయితే(length>0) {
        u=*us++;
        ఒకవేళ(!UCHAR_IS_INVARIANT(u)) {
            U_ASSERT(అబద్ధం); /* Variant characters were used. These are not portable in ICU. */
            u=0;
        }
        *cs++=(అక్షరం)UCHAR_గమ్యం_CHAR(u);
        --length;
    }
}

U_CAPI UBool U_EXPORT2
uprv_isInvariantString(స్థిరరాశి అక్షరం *s, int32_t length) {
    uint8_t c;

    కొరకు(;;) {
        ఒకవేళ(length<0) {
            /* NUL-terminated */
            c=(uint8_t)*s++;
            ఒకవేళ(c==0) {
                ఆపు;
            }
        } లేకపోతే {
            /* count length */
            ఒకవేళ(length==0) {
                ఆపు;
            }
            --length;
            c=(uint8_t)*s++;
            ఒకవేళ(c==0) {
                కొనసాగు; /* NUL is invariant */
            }
        }
        /* c!=0 now, one branch below checks c==0 for variant characters */

        /*
         * no assertions here because these functions are legitimately called
         * for strings with variant characters
         */
#ఒకవేళ U_CHARSET_FAMILY==U_ASCII_FAMILY
        ఒకవేళ(!UCHAR_IS_INVARIANT(c)) {
            తిరిగి అబద్ధం; /* found a variant char */
        }
#లేదావేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
        c=CHAR_గమ్యం_UCHAR(c);
        ఒకవేళ(c==0 || !UCHAR_IS_INVARIANT(c)) {
            తిరిగి అబద్ధం; /* found a variant char */
        }
#లేకపోతే
#   error U_CHARSET_FAMILY is not valid
#అయితే_ముగింపు
    }
    తిరిగి నిజం;
}

U_CAPI UBool U_EXPORT2
uprv_isInvariantUString(స్థిరరాశి char16_t *s, int32_t length) {
    char16_t c;

    కొరకు(;;) {
        ఒకవేళ(length<0) {
            /* NUL-terminated */
            c=*s++;
            ఒకవేళ(c==0) {
                ఆపు;
            }
        } లేకపోతే {
            /* count length */
            ఒకవేళ(length==0) {
                ఆపు;
            }
            --length;
            c=*s++;
        }

        /*
         * no assertions here because these functions are legitimately called
         * for strings with variant characters
         */
        ఒకవేళ(!UCHAR_IS_INVARIANT(c)) {
            తిరిగి అబద్ధం; /* found a variant char */
        }
    }
    తిరిగి నిజం;
}

/* UDataSwapFn implementations used in udataswp.c ------- */

/* convert ASCII to EBCDIC and verify that all characters are invariant */
U_CAPI int32_t U_EXPORT2
uprv_ebcdicFromAscii(స్థిరరాశి UDataSwapper *ds,
                     స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                     UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి uint8_t *s;
    uint8_t *t;
    uint8_t c;

    int32_t count;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<0 || (length>0 && బహిర్గతం_దత్తాంశం==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* setup and swapping */
    s=(స్థిరరాశి uint8_t *)లోపల_దత్తాంశం;
    t=(uint8_t *)బహిర్గతం_దత్తాంశం;
    count=length;
    అయితే(count>0) {
        c=*s++;
        ఒకవేళ(!UCHAR_IS_INVARIANT(c)) {
            udata_printError(ds, "uprv_ebcdicFromAscii() string[%d] contains a variant character in position %d\n",
                             length, length-count);
            *p_Error_సంకేతం=U_INVALID_CHAR_దొరికింది;
            తిరిగి 0;
        }
        *t++=ebcdicFromAscii[c];
        --count;
    }

    తిరిగి length;
}

/* this function only checks and copies ASCII strings without conversion */
U_CFUNC int32_t
uprv_copyAscii(స్థిరరాశి UDataSwapper *ds,
               స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
               UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి uint8_t *s;
    uint8_t c;

    int32_t count;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<0 || (length>0 && బహిర్గతం_దత్తాంశం==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* setup and checking */
    s=(స్థిరరాశి uint8_t *)లోపల_దత్తాంశం;
    count=length;
    అయితే(count>0) {
        c=*s++;
        ఒకవేళ(!UCHAR_IS_INVARIANT(c)) {
            udata_printError(ds, "uprv_copyFromAscii() string[%d] contains a variant character in position %d\n",
                             length, length-count);
            *p_Error_సంకేతం=U_INVALID_CHAR_దొరికింది;
            తిరిగి 0;
        }
        --count;
    }

    ఒకవేళ(length>0 && లోపల_దత్తాంశం!=బహిర్గతం_దత్తాంశం) {
        uprv_memcpy(బహిర్గతం_దత్తాంశం, లోపల_దత్తాంశం, length);
    }

    తిరిగి length;
}

/* convert EBCDIC to ASCII and verify that all characters are invariant */
U_CFUNC int32_t
uprv_asciiFromEbcdic(స్థిరరాశి UDataSwapper *ds,
                     స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                     UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి uint8_t *s;
    uint8_t *t;
    uint8_t c;

    int32_t count;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<0 ||  (length>0 && బహిర్గతం_దత్తాంశం==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* setup and swapping */
    s=(స్థిరరాశి uint8_t *)లోపల_దత్తాంశం;
    t=(uint8_t *)బహిర్గతం_దత్తాంశం;
    count=length;
    అయితే(count>0) {
        c=*s++;
        ఒకవేళ(c!=0 && ((c=asciiFromEbcdic[c])==0 || !UCHAR_IS_INVARIANT(c))) {
            udata_printError(ds, "uprv_asciiFromEbcdic() string[%d] contains a variant character in position %d\n",
                             length, length-count);
            *p_Error_సంకేతం=U_INVALID_CHAR_దొరికింది;
            తిరిగి 0;
        }
        *t++=c;
        --count;
    }

    తిరిగి length;
}

/* this function only checks and copies EBCDIC strings without conversion */
U_CFUNC int32_t
uprv_copyEbcdic(స్థిరరాశి UDataSwapper *ds,
                స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి uint8_t *s;
    uint8_t c;

    int32_t count;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(ds==శూన్యసూచిక || లోపల_దత్తాంశం==శూన్యసూచిక || length<0 || (length>0 && బహిర్గతం_దత్తాంశం==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* setup and checking */
    s=(స్థిరరాశి uint8_t *)లోపల_దత్తాంశం;
    count=length;
    అయితే(count>0) {
        c=*s++;
        ఒకవేళ(c!=0 && ((c=asciiFromEbcdic[c])==0 || !UCHAR_IS_INVARIANT(c))) {
            udata_printError(ds, "uprv_copyEbcdic() string[%] contains a variant character in position %d\n",
                             length, length-count);
            *p_Error_సంకేతం=U_INVALID_CHAR_దొరికింది;
            తిరిగి 0;
        }
        --count;
    }

    ఒకవేళ(length>0 && లోపల_దత్తాంశం!=బహిర్గతం_దత్తాంశం) {
        uprv_memcpy(బహిర్గతం_దత్తాంశం, లోపల_దత్తాంశం, length);
    }

    తిరిగి length;
}

U_CAPI UBool
uprv_isEbcdicAtSign(అక్షరం c) {
    స్థిర స్థిరరాశి uint8_t ebcdicAtSigns[] = {
        0x7C, 0x44, 0x66, 0x80, 0xAC, 0xAE, 0xAF, 0xB5, 0xEC, 0xEF, 0x00 };
    తిరిగి c != 0 && uprv_strchr((స్థిరరాశి అక్షరం *)ebcdicAtSigns, c) != శూన్యసూచిక;
}

/* compare invariant strings; variant characters compare less than others and unlike each other */
U_CFUNC int32_t
uprv_compareInvAscii(స్థిరరాశి UDataSwapper *ds,
                     స్థిరరాశి అక్షరం *బహిర్గతం_String, int32_t బహిర్గతం_నిడివి,
                     స్థిరరాశి char16_t *localString, int32_t local_నిడివి) {
    (శూన్య)ds;
    int32_t min_నిడివి;
    UChar32 c1, c2;
    uint8_t c;

    ఒకవేళ(బహిర్గతం_String==శూన్యసూచిక || బహిర్గతం_నిడివి<-1 || localString==శూన్యసూచిక || local_నిడివి<-1) {
        తిరిగి 0;
    }

    ఒకవేళ(బహిర్గతం_నిడివి<0) {
        బహిర్గతం_నిడివి=(int32_t)uprv_strlen(బహిర్గతం_String);
    }
    ఒకవేళ(local_నిడివి<0) {
        local_నిడివి=u_strlen(localString);
    }

    min_నిడివి= బహిర్గతం_నిడివి<local_నిడివి ? బహిర్గతం_నిడివి : local_నిడివి;

    అయితే(min_నిడివి>0) {
        c=(uint8_t)*బహిర్గతం_String++;
        ఒకవేళ(UCHAR_IS_INVARIANT(c)) {
            c1=c;
        } లేకపోతే {
            c1=-1;
        }

        c2=*localString++;
        ఒకవేళ(!UCHAR_IS_INVARIANT(c2)) {
            c2=-2;
        }

        ఒకవేళ((c1-=c2)!=0) {
            తిరిగి c1;
        }

        --min_నిడివి;
    }

    /* strings start with same prefix, compare lengths */
    తిరిగి బహిర్గతం_నిడివి-local_నిడివి;
}

U_CFUNC int32_t
uprv_compareInvEbcdic(స్థిరరాశి UDataSwapper *ds,
                      స్థిరరాశి అక్షరం *బహిర్గతం_String, int32_t బహిర్గతం_నిడివి,
                      స్థిరరాశి char16_t *localString, int32_t local_నిడివి) {
    (శూన్య)ds;
    int32_t min_నిడివి;
    UChar32 c1, c2;
    uint8_t c;

    ఒకవేళ(బహిర్గతం_String==శూన్యసూచిక || బహిర్గతం_నిడివి<-1 || localString==శూన్యసూచిక || local_నిడివి<-1) {
        తిరిగి 0;
    }

    ఒకవేళ(బహిర్గతం_నిడివి<0) {
        బహిర్గతం_నిడివి=(int32_t)uprv_strlen(బహిర్గతం_String);
    }
    ఒకవేళ(local_నిడివి<0) {
        local_నిడివి=u_strlen(localString);
    }

    min_నిడివి= బహిర్గతం_నిడివి<local_నిడివి ? బహిర్గతం_నిడివి : local_నిడివి;

    అయితే(min_నిడివి>0) {
        c=(uint8_t)*బహిర్గతం_String++;
        ఒకవేళ(c==0) {
            c1=0;
        } లేకపోతే ఒకవేళ((c1=asciiFromEbcdic[c])!=0 && UCHAR_IS_INVARIANT(c1)) {
            /* c1 is set */
        } లేకపోతే {
            c1=-1;
        }

        c2=*localString++;
        ఒకవేళ(!UCHAR_IS_INVARIANT(c2)) {
            c2=-2;
        }

        ఒకవేళ((c1-=c2)!=0) {
            తిరిగి c1;
        }

        --min_నిడివి;
    }

    /* strings start with same prefix, compare lengths */
    తిరిగి బహిర్గతం_నిడివి-local_నిడివి;
}

U_CAPI int32_t U_EXPORT2
uprv_compareInvEbcdicAsAscii(స్థిరరాశి అక్షరం *s1, స్థిరరాశి అక్షరం *s2) {
    int32_t c1, c2;

    కొరకు(;; ++s1, ++s2) {
        c1=(uint8_t)*s1;
        c2=(uint8_t)*s2;
        ఒకవేళ(c1!=c2) {
            ఒకవేళ(c1!=0 && ((c1=asciiFromEbcdic[c1])==0 || !UCHAR_IS_INVARIANT(c1))) {
                c1=-(int32_t)(uint8_t)*s1;
            }
            ఒకవేళ(c2!=0 && ((c2=asciiFromEbcdic[c2])==0 || !UCHAR_IS_INVARIANT(c2))) {
                c2=-(int32_t)(uint8_t)*s2;
            }
            తిరిగి c1-c2;
        } లేకపోతే ఒకవేళ(c1==0) {
            తిరిగి 0;
        }
    }
}

U_CAPI అక్షరం U_EXPORT2
uprv_ebcdic_గమ్యం_Ascii(అక్షరం c) {
    తిరిగి (అక్షరం)asciiFromEbcdic[(uint8_t)c];
}

U_CAPI అక్షరం U_EXPORT2
uprv_ebcdic_గమ్యం_Lowercase_Ascii(అక్షరం c) {
    తిరిగి (అక్షరం)lowercaseAsciiFromEbcdic[(uint8_t)c];
}

U_CAPI uint8_t* U_EXPORT2
uprv_aestrncpy(uint8_t *dst, స్థిరరాశి uint8_t *src, int32_t n)
{
  uint8_t *orig_dst = dst;

  ఒకవేళ(n==-1) { 
    n = static_cast<int32_t>(uprv_strlen((స్థిరరాశి అక్షరం*)src)+1); /* copy NUL */
  }
  /* copy non-null */
  అయితే(*src && n>0) {
    *(dst++) = asciiFromEbcdic[*(src++)];
    n--;
  }
  /* pad */
  అయితే(n>0) {
    *(dst++) = 0;
    n--;
  }
  తిరిగి orig_dst;
}

U_CAPI uint8_t* U_EXPORT2
uprv_eastrncpy(uint8_t *dst, స్థిరరాశి uint8_t *src, int32_t n)
{
  uint8_t *orig_dst = dst;

  ఒకవేళ(n==-1) { 
    n = static_cast<int32_t>(uprv_strlen((స్థిరరాశి అక్షరం*)src)+1); /* copy NUL */
  }
  /* copy non-null */
  అయితే(*src && n>0) {
    అక్షరం ch = ebcdicFromAscii[*(src++)];
    ఒకవేళ(ch == 0) {
      ch = ebcdicFromAscii[0x3f]; /* questionmark (subchar) */
    }
    *(dst++) = ch;
    n--;
  }
  /* pad */
  అయితే(n>0) {
    *(dst++) = 0;
    n--;
  }
  తిరిగి orig_dst;
}

