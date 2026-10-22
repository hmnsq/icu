// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2002-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  ucnvbocu.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002mar27
*   created by: Markus W. Scherer
*
*   This is an implementation of the Binary Ordered Compression for Unicode,
*   in its MIME-friendly form as defined in http://www.unicode.org/notes/tn6/
*/

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/ucnv_cb.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "uassert.h"

/* BOCU-1 constants and macros ---------------------------------------------- */

/*
 * BOCU-1 encodes the code points of a Unicode string as
 * a sequence of byte-encoded differences (slope detection),
 * preserving lexical order.
 *
 * Optimize the difference-taking for runs of Unicode text within
 * small scripts:
 *
 * Most small scripts are allocated within aligned 128-blocks of Unicode
 * code points. Lexical order is preserved if the "previous code point" state
 * is always moved into the middle of such a block.
 *
 * Additionally, "prev" is moved from anywhere in the Unihan and Hangul
 * areas into the middle of those areas.
 *
 * C0 control codes and space are encoded with their US-ASCII bytes.
 * "prev" is reset for C0 controls but not for space.
 */

/* initial value for "prev": middle of the ASCII range */
#నిర్వచించు BOCU1_ASCII_PREV        0x40

/* bounding byte values for differences */
#నిర్వచించు BOCU1_MIN               0x21
#నిర్వచించు BOCU1_మధ్య            0x90
#నిర్వచించు BOCU1_MAX_LEAD          0xfe
#నిర్వచించు BOCU1_MAX_TRAIL         0xff
#నిర్వచించు BOCU1_RESET             0xff

/* number of lead bytes */
#నిర్వచించు BOCU1_లెక్క             (BOCU1_MAX_LEAD-BOCU1_MIN+1)

/* adjust trail byte counts for the use of some C0 control byte values */
#నిర్వచించు BOCU1_TRAIL_CONTROLS_లెక్క  20
#నిర్వచించు BOCU1_TRAIL_BYTE_స్థానభ్రంశం     (BOCU1_MIN-BOCU1_TRAIL_CONTROLS_లెక్క)

/* number of trail bytes */
#నిర్వచించు BOCU1_TRAIL_లెక్క       ((BOCU1_MAX_TRAIL-BOCU1_MIN+1)+BOCU1_TRAIL_CONTROLS_లెక్క)

/*
 * number of positive and negative single-byte codes
 * (counting 0==BOCU1_MIDDLE among the positive ones)
 */
#నిర్వచించు BOCU1_SINGLE            64

/* number of lead bytes for positive and negative 2/3/4-byte sequences */
#నిర్వచించు BOCU1_LEAD_2            43
#నిర్వచించు BOCU1_LEAD_3            3
#నిర్వచించు BOCU1_LEAD_4            1

/* The difference value range for single-byters. */
#నిర్వచించు BOCU1_వ్యాప్తి_POS_1   (BOCU1_SINGLE-1)
#నిర్వచించు BOCU1_వ్యాప్తి_NEG_1   (-BOCU1_SINGLE)

/* The difference value range for double-byters. */
#నిర్వచించు BOCU1_వ్యాప్తి_POS_2   (BOCU1_వ్యాప్తి_POS_1+BOCU1_LEAD_2*BOCU1_TRAIL_లెక్క)
#నిర్వచించు BOCU1_వ్యాప్తి_NEG_2   (BOCU1_వ్యాప్తి_NEG_1-BOCU1_LEAD_2*BOCU1_TRAIL_లెక్క)

/* The difference value range for 3-byters. */
#నిర్వచించు BOCU1_వ్యాప్తి_POS_3   \
    (BOCU1_వ్యాప్తి_POS_2+BOCU1_LEAD_3*BOCU1_TRAIL_లెక్క*BOCU1_TRAIL_లెక్క)

#నిర్వచించు BOCU1_వ్యాప్తి_NEG_3   (BOCU1_వ్యాప్తి_NEG_2-BOCU1_LEAD_3*BOCU1_TRAIL_లెక్క*BOCU1_TRAIL_లెక్క)

/* The lead byte start values. */
#నిర్వచించు BOCU1_ప్రారంభం_POS_2   (BOCU1_మధ్య+BOCU1_వ్యాప్తి_POS_1+1)
#నిర్వచించు BOCU1_ప్రారంభం_POS_3   (BOCU1_ప్రారంభం_POS_2+BOCU1_LEAD_2)
#నిర్వచించు BOCU1_ప్రారంభం_POS_4   (BOCU1_ప్రారంభం_POS_3+BOCU1_LEAD_3)
     /* ==BOCU1_MAX_LEAD */

#నిర్వచించు BOCU1_ప్రారంభం_NEG_2   (BOCU1_మధ్య+BOCU1_వ్యాప్తి_NEG_1)
#నిర్వచించు BOCU1_ప్రారంభం_NEG_3   (BOCU1_ప్రారంభం_NEG_2-BOCU1_LEAD_2)
#నిర్వచించు BOCU1_ప్రారంభం_NEG_4   (BOCU1_ప్రారంభం_NEG_3-BOCU1_LEAD_3)
     /* ==BOCU1_MIN+1 */

/* The length of a byte sequence, according to the lead byte (!=BOCU1_RESET). */
#నిర్వచించు BOCU1_నిడివి_FROM_LEAD(lead) \
    ((BOCU1_ప్రారంభం_NEG_2<=(lead) && (lead)<BOCU1_ప్రారంభం_POS_2) ? 1 : \
     (BOCU1_ప్రారంభం_NEG_3<=(lead) && (lead)<BOCU1_ప్రారంభం_POS_3) ? 2 : \
     (BOCU1_ప్రారంభం_NEG_4<=(lead) && (lead)<BOCU1_ప్రారంభం_POS_4) ? 3 : 4)

/* The length of a byte sequence, according to its packed form. */
#నిర్వచించు BOCU1_నిడివి_FROM_PACKED(packed) \
    ((uint32_t)(packed)<0x04000000 ? (packed)>>24 : 4)

/*
 * 12 commonly used C0 control codes (and space) are only used to encode
 * themselves directly,
 * which makes BOCU-1 MIME-usable and reasonably safe for
 * ASCII-oriented software.
 *
 * These controls are
 *  0   NUL
 *
 *  7   BEL
 *  8   BS
 *
 *  9   TAB
 *  a   LF
 *  b   VT
 *  c   FF
 *  d   CR
 *
 *  e   SO
 *  f   SI
 *
 * 1a   SUB
 * 1b   ESC
 *
 * The other 20 C0 controls are also encoded directly (to preserve order)
 * but are also used as trail bytes in difference encoding
 * (for better compression).
 */
#నిర్వచించు BOCU1_TRAIL_గమ్యం_BYTE(t) ((t)>=BOCU1_TRAIL_CONTROLS_లెక్క ? (t)+BOCU1_TRAIL_BYTE_స్థానభ్రంశం : bocu1_Trail_గమ్యం_Byte[t])

/*
 * Byte value map for control codes,
 * from external byte values 0x00..0x20
 * to trail byte values 0..19 (0..0x13) as used in the difference calculation.
 * External byte values that are illegal as trail bytes are mapped to -1.
 */
స్థిర స్థిరరాశి int8_t
bocu1_Byte_గమ్యం_Trail[BOCU1_MIN]={
/*  0     1     2     3     4     5     6     7    */
    -1,   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, -1,

/*  8     9     a     b     c     d     e     f    */
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,

/*  10    11    12    13    14    15    16    17   */
    0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,

/*  18    19    1a    1b    1c    1d    1e    1f   */
    0x0e, 0x0f, -1,   -1,   0x10, 0x11, 0x12, 0x13,

/*  20   */
    -1
};

/*
 * Byte value map for control codes,
 * from trail byte values 0..19 (0..0x13) as used in the difference calculation
 * to external byte values 0x00..0x20.
 */
స్థిర స్థిరరాశి int8_t
bocu1_Trail_గమ్యం_Byte[BOCU1_TRAIL_CONTROLS_లెక్క]={
/*  0     1     2     3     4     5     6     7    */
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x10, 0x11,

/*  8     9     a     b     c     d     e     f    */
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,

/*  10    11    12    13   */
    0x1c, 0x1d, 0x1e, 0x1f
};

/**
 * Integer division and modulo with negative numerators
 * yields negative modulo results and quotients that are one more than
 * what we need here.
 * This macro adjust the results so that the modulo-value m is always >=0.
 *
 * For positive n, the if() condition is always false.
 *
 * @param n Number to be split into quotient and rest.
 *          Will be modified to contain the quotient.
 * @param d Divisor.
 * @param m Output variable for the rest (modulo result).
 */
#నిర్వచించు NEGDIVMOD(n, d, m) UPRV_ఖండం_MACRO_BEGIN { \
    (m)=(n)%(d); \
    (n)/=(d); \
    ఒకవేళ((m)<0) { \
        --(n); \
        (m)+=(d); \
    } \
} UPRV_ఖండం_MACRO_చివర

/* Faster versions of packDiff() for single-byte-encoded diff values. */

/** Is a diff value encodable in a single byte? */
#నిర్వచించు DIFF_IS_SINGLE(diff) (BOCU1_వ్యాప్తి_NEG_1<=(diff) && (diff)<=BOCU1_వ్యాప్తి_POS_1)

/** Encode a diff value in a single byte. */
#నిర్వచించు PACK_SINGLE_DIFF(diff) (BOCU1_మధ్య+(diff))

/** Is a diff value encodable in two bytes? */
#నిర్వచించు DIFF_IS_DOUBLE(diff) (BOCU1_వ్యాప్తి_NEG_2<=(diff) && (diff)<=BOCU1_వ్యాప్తి_POS_2)

/* BOCU-1 implementation functions ------------------------------------------ */

#నిర్వచించు BOCU1_SIMPLE_PREV(c) (((c)&~0x7f)+BOCU1_ASCII_PREV)

/**
 * Compute the next "previous" value for differencing
 * from the current code point.
 *
 * @param c current code point, 0x3040..0xd7a3 (rest handled by macro below)
 * @return "previous code point" state value
 */
స్థిర అంతఃవరుస int32_t
bocu1Prev(int32_t c) {
    /* compute new prev */
    ఒకవేళ(/* 0x3040<=c && */ c<=0x309f) {
        /* Hiragana is not 128-aligned */
        తిరిగి 0x3070;
    } లేకపోతే ఒకవేళ(0x4e00<=c && c<=0x9fa5) {
        /* CJK Unihan */
        తిరిగి 0x4e00-BOCU1_వ్యాప్తి_NEG_2;
    } లేకపోతే ఒకవేళ(0xac00<=c /* && c<=0xd7a3 */) {
        /* Korean Hangul */
        తిరిగి (0xd7a3+0xac00)/2;
    } లేకపోతే {
        /* mostly small scripts */
        తిరిగి BOCU1_SIMPLE_PREV(c);
    }
}

/** Fast version of bocu1Prev() for most scripts. */
#నిర్వచించు BOCU1_PREV(c) ((c)<0x3040 || (c)>0xd7a3 ? BOCU1_SIMPLE_PREV(c) : bocu1Prev(c))

/*
 * The BOCU-1 converter uses the standard setup code in ucnv.c/ucnv_bld.c.
 * The UConverter fields are used as follows:
 *
 * fromUnicodeStatus    encoder's prev (0 will be interpreted as BOCU1_ASCII_PREV)
 *
 * toUnicodeStatus      decoder's prev (0 will be interpreted as BOCU1_ASCII_PREV)
 * mode                 decoder's incomplete (diff<<2)|count (ignored when toULength==0)
 */

/* BOCU-1-from-Unicode conversion functions --------------------------------- */

/**
 * Encode a difference -0x10ffff..0x10ffff in 1..4 bytes
 * and return a packed integer with them.
 *
 * The encoding favors small absolute differences with short encodings
 * to compress runs of same-script characters.
 *
 * Optimized version with unrolled loops and fewer floating-point operations
 * than the standard packDiff().
 *
 * @param diff difference value -0x10ffff..0x10ffff
 * @return
 *      0x010000zz for 1-byte sequence zz
 *      0x0200yyzz for 2-byte sequence yy zz
 *      0x03xxyyzz for 3-byte sequence xx yy zz
 *      0xwwxxyyzz for 4-byte sequence ww xx yy zz (ww>0x03)
 */
స్థిర int32_t
packDiff(int32_t diff) {
    int32_t ఫలితం, m;

    U_ASSERT(!DIFF_IS_SINGLE(diff)); /* assume we won't be called where diff==BOCU1_REACH_NEG_1=-64 */
    ఒకవేళ(diff>=BOCU1_వ్యాప్తి_NEG_1) {
        /* mostly positive differences, and single-byte negative ones */
#ఒకవేళ 0   /* single-byte case handled in macros, see below */
        ఒకవేళ(diff<=BOCU1_వ్యాప్తి_POS_1) {
            /* single byte */
            తిరిగి 0x01000000|(BOCU1_మధ్య+diff);
        } లేకపోతే
#అయితే_ముగింపు
        ఒకవేళ(diff<=BOCU1_వ్యాప్తి_POS_2) {
            /* two bytes */
            diff-=BOCU1_వ్యాప్తి_POS_1+1;
            ఫలితం=0x02000000;

            m=diff%BOCU1_TRAIL_లెక్క;
            diff/=BOCU1_TRAIL_లెక్క;
            ఫలితం|=BOCU1_TRAIL_గమ్యం_BYTE(m);

            ఫలితం|=(BOCU1_ప్రారంభం_POS_2+diff)<<8;
        } లేకపోతే ఒకవేళ(diff<=BOCU1_వ్యాప్తి_POS_3) {
            /* three bytes */
            diff-=BOCU1_వ్యాప్తి_POS_2+1;
            ఫలితం=0x03000000;

            m=diff%BOCU1_TRAIL_లెక్క;
            diff/=BOCU1_TRAIL_లెక్క;
            ఫలితం|=BOCU1_TRAIL_గమ్యం_BYTE(m);

            m=diff%BOCU1_TRAIL_లెక్క;
            diff/=BOCU1_TRAIL_లెక్క;
            ఫలితం|=BOCU1_TRAIL_గమ్యం_BYTE(m)<<8;

            ఫలితం|=(BOCU1_ప్రారంభం_POS_3+diff)<<16;
        } లేకపోతే {
            /* four bytes */
            diff-=BOCU1_వ్యాప్తి_POS_3+1;

            m=diff%BOCU1_TRAIL_లెక్క;
            diff/=BOCU1_TRAIL_లెక్క;
            ఫలితం=BOCU1_TRAIL_గమ్యం_BYTE(m);

            m=diff%BOCU1_TRAIL_లెక్క;
            diff/=BOCU1_TRAIL_లెక్క;
            ఫలితం|=BOCU1_TRAIL_గమ్యం_BYTE(m)<<8;

            /*
             * We know that / and % would deliver quotient 0 and rest=diff.
             * Avoid division and modulo for performance.
             */
            ఫలితం|=BOCU1_TRAIL_గమ్యం_BYTE(diff)<<16;

            ఫలితం |= static_cast<uint32_t>(BOCU1_ప్రారంభం_POS_4) << 24;
        }
    } లేకపోతే {
        /* two- to four-byte negative differences */
        ఒకవేళ(diff>=BOCU1_వ్యాప్తి_NEG_2) {
            /* two bytes */
            diff-=BOCU1_వ్యాప్తి_NEG_1;
            ఫలితం=0x02000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_లెక్క, m);
            ఫలితం|=BOCU1_TRAIL_గమ్యం_BYTE(m);

            ఫలితం|=(BOCU1_ప్రారంభం_NEG_2+diff)<<8;
        } లేకపోతే ఒకవేళ(diff>=BOCU1_వ్యాప్తి_NEG_3) {
            /* three bytes */
            diff-=BOCU1_వ్యాప్తి_NEG_2;
            ఫలితం=0x03000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_లెక్క, m);
            ఫలితం|=BOCU1_TRAIL_గమ్యం_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_లెక్క, m);
            ఫలితం|=BOCU1_TRAIL_గమ్యం_BYTE(m)<<8;

            ఫలితం|=(BOCU1_ప్రారంభం_NEG_3+diff)<<16;
        } లేకపోతే {
            /* four bytes */
            diff-=BOCU1_వ్యాప్తి_NEG_3;

            NEGDIVMOD(diff, BOCU1_TRAIL_లెక్క, m);
            ఫలితం=BOCU1_TRAIL_గమ్యం_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_లెక్క, m);
            ఫలితం|=BOCU1_TRAIL_గమ్యం_BYTE(m)<<8;

            /*
             * We know that NEGDIVMOD would deliver
             * quotient -1 and rest=diff+BOCU1_TRAIL_COUNT.
             * Avoid division and modulo for performance.
             */
            m=diff+BOCU1_TRAIL_లెక్క;
            ఫలితం|=BOCU1_TRAIL_గమ్యం_BYTE(m)<<16;

            ఫలితం|=BOCU1_MIN<<24;
        }
    }
    తిరిగి ఫలితం;
}


స్థిర శూన్య U_CALLCONV
_Bocu1FromUnicodeWithOffsets(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
                             UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి char16_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం;
    int32_t *offsets;

    int32_t prev, c, diff;

    int32_t ఆధారం_సూచీ, తదుపరి_ఆధారం_సూచీ;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    ఆధారం=p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం = reinterpret_cast<uint8_t*>(p_వాదనలు->లక్ష్యం);
    లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(p_వాదనలు->లక్ష్యం_హద్దు - p_వాదనలు->లక్ష్యం);
    offsets=p_వాదనలు->offsets;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    prev = static_cast<int32_t>(cnv->from_Unicode_స్థితి);
    ఒకవేళ(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ= c==0 ? 0 : -1;
    తదుపరి_ఆధారం_సూచీ=0;

    /* conversion loop */
    ఒకవేళ(c!=0 && లక్ష్యం_సామర్థ్యం>0) {
        వెళ్ళు getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(ఆధారం_హద్దు - ఆధారం);
    ఒకవేళ(లక్ష్యం_సామర్థ్యం>diff) {
        లక్ష్యం_సామర్థ్యం=diff;
    }
    అయితే(లక్ష్యం_సామర్థ్యం>0 && (c=*ఆధారం)<0x3000) {
        ఒకవేళ(c<=0x20) {
            ఒకవేళ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *లక్ష్యం++ = static_cast<uint8_t>(c);
            *offsets++=తదుపరి_ఆధారం_సూచీ++;
            ++ఆధారం;
            --లక్ష్యం_సామర్థ్యం;
        } లేకపోతే {
            diff=c-prev;
            ఒకవేళ(DIFF_IS_SINGLE(diff)) {
                prev=BOCU1_SIMPLE_PREV(c);
                *లక్ష్యం++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=తదుపరి_ఆధారం_సూచీ++;
                ++ఆధారం;
                --లక్ష్యం_సామర్థ్యం;
            } లేకపోతే {
                ఆపు;
            }
        }
    }
    /* restore real values */
    లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->లక్ష్యం_హద్దు) - లక్ష్యం);
    ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ; /* wrong if offsets==nullptr but does not matter */

    /* regular loop for all cases */
    అయితే(ఆధారం<ఆధారం_హద్దు) {
        ఒకవేళ(లక్ష్యం_సామర్థ్యం>0) {
            c=*ఆధారం++;
            ++తదుపరి_ఆధారం_సూచీ;

            ఒకవేళ(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                ఒకవేళ(c!=0x20) {
                    prev=BOCU1_ASCII_PREV;
                }
                *లక్ష్యం++ = static_cast<uint8_t>(c);
                *offsets++=ఆధారం_సూచీ;
                --లక్ష్యం_సామర్థ్యం;

                ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                కొనసాగు;
            }

            ఒకవేళ(U16_IS_LEAD(c)) {
getTrail:
                ఒకవేళ(ఆధారం<ఆధారం_హద్దు) {
                    /* test the following code unit */
                    char16_t trail=*ఆధారం;
                    ఒకవేళ(U16_IS_TRAIL(trail)) {
                        ++ఆధారం;
                        ++తదుపరి_ఆధారం_సూచీ;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } లేకపోతే {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    ఆపు;
                }
            }

            /*
             * all other Unicode code points c==U+0021..U+10ffff
             * are encoded with the difference c-prev
             *
             * a new prev is computed from c,
             * placed in the middle of a 0x80-block (for most small scripts) or
             * in the middle of the Unihan and Hangul blocks
             * to statistically minimize the following difference
             */
            diff=c-prev;
            prev=BOCU1_PREV(c);
            ఒకవేళ(DIFF_IS_SINGLE(diff)) {
                *లక్ష్యం++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=ఆధారం_సూచీ;
                --లక్ష్యం_సామర్థ్యం;
                ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                ఒకవేళ(c<0x3000) {
                    వెళ్ళు fastSingle;
                }
            } లేకపోతే ఒకవేళ(DIFF_IS_DOUBLE(diff) && 2<=లక్ష్యం_సామర్థ్యం) {
                /* optimize 2-byte case */
                int32_t m;

                ఒకవేళ(diff>=0) {
                    diff-=BOCU1_వ్యాప్తి_POS_1+1;
                    m=diff%BOCU1_TRAIL_లెక్క;
                    diff/=BOCU1_TRAIL_లెక్క;
                    diff+=BOCU1_ప్రారంభం_POS_2;
                } లేకపోతే {
                    diff-=BOCU1_వ్యాప్తి_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_లెక్క, m);
                    diff+=BOCU1_ప్రారంభం_NEG_2;
                }
                *లక్ష్యం++ = static_cast<uint8_t>(diff);
                *లక్ష్యం++ = static_cast<uint8_t>(BOCU1_TRAIL_గమ్యం_BYTE(m));
                *offsets++=ఆధారం_సూచీ;
                *offsets++=ఆధారం_సూచీ;
                లక్ష్యం_సామర్థ్యం-=2;
                ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
            } లేకపోతే {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_నిడివి_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                ఒకవేళ(length<=లక్ష్యం_సామర్థ్యం) {
                    ఎంపిక(length) {
                        /* each branch falls through to the next one */
                    స్థితి 4:
                        *లక్ష్యం++ = static_cast<uint8_t>(diff >> 24);
                        *offsets++=ఆధారం_సూచీ;
                        U_FALLTHROUGH;
                    స్థితి 3:
                        *లక్ష్యం++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=ఆధారం_సూచీ;
                        U_FALLTHROUGH;
                    స్థితి 2:
                        *లక్ష్యం++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=ఆధారం_సూచీ;
                    /* case 1: handled above */
                        *లక్ష్యం++ = static_cast<uint8_t>(diff);
                        *offsets++=ఆధారం_సూచీ;
                        U_FALLTHROUGH;
                    అప్రమేయ:
                        /* will never occur */
                        ఆపు;
                    }
                    లక్ష్యం_సామర్థ్యం-=length;
                    ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                } లేకపోతే {
                    uint8_t *char_Error_బఫరు;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=లక్ష్యం_సామర్థ్యం;
                    char_Error_బఫరు=(uint8_t *)cnv->char_Error_బఫరు;
                    ఎంపిక(length) {
                        /* each branch falls through to the next one */
                    స్థితి 3:
                        *char_Error_బఫరు++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    స్థితి 2:
                        *char_Error_బఫరు++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    స్థితి 1:
                        *char_Error_బఫరు = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    అప్రమేయ:
                        /* will never occur */
                        ఆపు;
                    }
                    cnv->char_Error_బఫరు_నిడివి = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    ఎంపిక(లక్ష్యం_సామర్థ్యం) {
                        /* each branch falls through to the next one */
                    స్థితి 3:
                        *లక్ష్యం++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=ఆధారం_సూచీ;
                        U_FALLTHROUGH;
                    స్థితి 2:
                        *లక్ష్యం++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=ఆధారం_సూచీ;
                        U_FALLTHROUGH;
                    స్థితి 1:
                        *లక్ష్యం++ = static_cast<uint8_t>(diff);
                        *offsets++=ఆధారం_సూచీ;
                        U_FALLTHROUGH;
                    అప్రమేయ:
                        /* will never occur */
                        ఆపు;
                    }

                    /* target overflow */
                    లక్ష్యం_సామర్థ్యం=0;
                    *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                    ఆపు;
                }
            }
        } లేకపోతే {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_స్థితి = static_cast<uint32_t>(prev);

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=ఆధారం;
    p_వాదనలు->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
    p_వాదనలు->offsets=offsets;
}

/*
 * Identical to _Bocu1FromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
స్థిర శూన్య U_CALLCONV
_Bocu1FromUnicode(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
                  UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి char16_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం;

    int32_t prev, c, diff;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    ఆధారం=p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం = reinterpret_cast<uint8_t*>(p_వాదనలు->లక్ష్యం);
    లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(p_వాదనలు->లక్ష్యం_హద్దు - p_వాదనలు->లక్ష్యం);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    prev = static_cast<int32_t>(cnv->from_Unicode_స్థితి);
    ఒకవేళ(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }

    /* conversion loop */
    ఒకవేళ(c!=0 && లక్ష్యం_సామర్థ్యం>0) {
        వెళ్ళు getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(ఆధారం_హద్దు - ఆధారం);
    ఒకవేళ(లక్ష్యం_సామర్థ్యం>diff) {
        లక్ష్యం_సామర్థ్యం=diff;
    }
    అయితే(లక్ష్యం_సామర్థ్యం>0 && (c=*ఆధారం)<0x3000) {
        ఒకవేళ(c<=0x20) {
            ఒకవేళ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *లక్ష్యం++ = static_cast<uint8_t>(c);
        } లేకపోతే {
            diff=c-prev;
            ఒకవేళ(DIFF_IS_SINGLE(diff)) {
                prev=BOCU1_SIMPLE_PREV(c);
                *లక్ష్యం++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
            } లేకపోతే {
                ఆపు;
            }
        }
        ++ఆధారం;
        --లక్ష్యం_సామర్థ్యం;
    }
    /* restore real values */
    లక్ష్యం_సామర్థ్యం = static_cast<int32_t>(reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->లక్ష్యం_హద్దు) - లక్ష్యం);

    /* regular loop for all cases */
    అయితే(ఆధారం<ఆధారం_హద్దు) {
        ఒకవేళ(లక్ష్యం_సామర్థ్యం>0) {
            c=*ఆధారం++;

            ఒకవేళ(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                ఒకవేళ(c!=0x20) {
                    prev=BOCU1_ASCII_PREV;
                }
                *లక్ష్యం++ = static_cast<uint8_t>(c);
                --లక్ష్యం_సామర్థ్యం;
                కొనసాగు;
            }

            ఒకవేళ(U16_IS_LEAD(c)) {
getTrail:
                ఒకవేళ(ఆధారం<ఆధారం_హద్దు) {
                    /* test the following code unit */
                    char16_t trail=*ఆధారం;
                    ఒకవేళ(U16_IS_TRAIL(trail)) {
                        ++ఆధారం;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } లేకపోతే {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    ఆపు;
                }
            }

            /*
             * all other Unicode code points c==U+0021..U+10ffff
             * are encoded with the difference c-prev
             *
             * a new prev is computed from c,
             * placed in the middle of a 0x80-block (for most small scripts) or
             * in the middle of the Unihan and Hangul blocks
             * to statistically minimize the following difference
             */
            diff=c-prev;
            prev=BOCU1_PREV(c);
            ఒకవేళ(DIFF_IS_SINGLE(diff)) {
                *లక్ష్యం++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                --లక్ష్యం_సామర్థ్యం;
                ఒకవేళ(c<0x3000) {
                    వెళ్ళు fastSingle;
                }
            } లేకపోతే ఒకవేళ(DIFF_IS_DOUBLE(diff) && 2<=లక్ష్యం_సామర్థ్యం) {
                /* optimize 2-byte case */
                int32_t m;

                ఒకవేళ(diff>=0) {
                    diff-=BOCU1_వ్యాప్తి_POS_1+1;
                    m=diff%BOCU1_TRAIL_లెక్క;
                    diff/=BOCU1_TRAIL_లెక్క;
                    diff+=BOCU1_ప్రారంభం_POS_2;
                } లేకపోతే {
                    diff-=BOCU1_వ్యాప్తి_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_లెక్క, m);
                    diff+=BOCU1_ప్రారంభం_NEG_2;
                }
                *లక్ష్యం++ = static_cast<uint8_t>(diff);
                *లక్ష్యం++ = static_cast<uint8_t>(BOCU1_TRAIL_గమ్యం_BYTE(m));
                లక్ష్యం_సామర్థ్యం-=2;
            } లేకపోతే {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_నిడివి_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                ఒకవేళ(length<=లక్ష్యం_సామర్థ్యం) {
                    ఎంపిక(length) {
                        /* each branch falls through to the next one */
                    స్థితి 4:
                        *లక్ష్యం++ = static_cast<uint8_t>(diff >> 24);
                        U_FALLTHROUGH;
                    స్థితి 3:
                        *లక్ష్యం++ = static_cast<uint8_t>(diff >> 16);
                    /* case 2: handled above */
                        *లక్ష్యం++ = static_cast<uint8_t>(diff >> 8);
                    /* case 1: handled above */
                        *లక్ష్యం++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    అప్రమేయ:
                        /* will never occur */
                        ఆపు;
                    }
                    లక్ష్యం_సామర్థ్యం-=length;
                } లేకపోతే {
                    uint8_t *char_Error_బఫరు;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=లక్ష్యం_సామర్థ్యం;
                    char_Error_బఫరు=(uint8_t *)cnv->char_Error_బఫరు;
                    ఎంపిక(length) {
                        /* each branch falls through to the next one */
                    స్థితి 3:
                        *char_Error_బఫరు++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    స్థితి 2:
                        *char_Error_బఫరు++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    స్థితి 1:
                        *char_Error_బఫరు = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    అప్రమేయ:
                        /* will never occur */
                        ఆపు;
                    }
                    cnv->char_Error_బఫరు_నిడివి = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    ఎంపిక(లక్ష్యం_సామర్థ్యం) {
                        /* each branch falls through to the next one */
                    స్థితి 3:
                        *లక్ష్యం++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    స్థితి 2:
                        *లక్ష్యం++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    స్థితి 1:
                        *లక్ష్యం++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    అప్రమేయ:
                        /* will never occur */
                        ఆపు;
                    }

                    /* target overflow */
                    లక్ష్యం_సామర్థ్యం=0;
                    *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                    ఆపు;
                }
            }
        } లేకపోతే {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_స్థితి = static_cast<uint32_t>(prev);

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=ఆధారం;
    p_వాదనలు->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
}

/* BOCU-1-to-Unicode conversion functions ----------------------------------- */

/**
 * Function for BOCU-1 decoder; handles multi-byte lead bytes.
 *
 * @param b lead byte;
 *          BOCU1_MIN<=b<BOCU1_START_NEG_2 or BOCU1_START_POS_2<=b<BOCU1_MAX_LEAD
 * @return (diff<<2)|count
 */
స్థిర అంతఃవరుస int32_t
decodeBocu1LeadByte(int32_t b) {
    int32_t diff, count;

    ఒకవేళ(b>=BOCU1_ప్రారంభం_NEG_2) {
        /* positive difference */
        ఒకవేళ(b<BOCU1_ప్రారంభం_POS_3) {
            /* two bytes */
            diff = (b - BOCU1_ప్రారంభం_POS_2) * BOCU1_TRAIL_లెక్క + BOCU1_వ్యాప్తి_POS_1 + 1;
            count=1;
        } లేకపోతే ఒకవేళ(b<BOCU1_ప్రారంభం_POS_4) {
            /* three bytes */
            diff = (b - BOCU1_ప్రారంభం_POS_3) * BOCU1_TRAIL_లెక్క * BOCU1_TRAIL_లెక్క + BOCU1_వ్యాప్తి_POS_2 + 1;
            count=2;
        } లేకపోతే {
            /* four bytes */
            diff=BOCU1_వ్యాప్తి_POS_3+1;
            count=3;
        }
    } లేకపోతే {
        /* negative difference */
        ఒకవేళ(b>=BOCU1_ప్రారంభం_NEG_3) {
            /* two bytes */
            diff = (b - BOCU1_ప్రారంభం_NEG_2) * BOCU1_TRAIL_లెక్క + BOCU1_వ్యాప్తి_NEG_1;
            count=1;
        } లేకపోతే ఒకవేళ(b>BOCU1_MIN) {
            /* three bytes */
            diff = (b - BOCU1_ప్రారంభం_NEG_3) * BOCU1_TRAIL_లెక్క * BOCU1_TRAIL_లెక్క + BOCU1_వ్యాప్తి_NEG_2;
            count=2;
        } లేకపోతే {
            /* four bytes */
            diff=-BOCU1_TRAIL_లెక్క*BOCU1_TRAIL_లెక్క*BOCU1_TRAIL_లెక్క+BOCU1_వ్యాప్తి_NEG_3;
            count=3;
        }
    }

    /* return the state for decoding the trail byte(s) */
    తిరిగి (static_cast<uint32_t>(diff) << 2) | count;
}

/**
 * Function for BOCU-1 decoder; handles multi-byte trail bytes.
 *
 * @param count number of remaining trail bytes including this one
 * @param b trail byte
 * @return new delta for diff including b - <0 indicates an error
 *
 * @see decodeBocu1
 */
స్థిర అంతఃవరుస int32_t
decodeBocu1TrailByte(int32_t count, int32_t b) {
    ఒకవేళ(b<=0x20) {
        /* skip some C0 controls and make the trail byte range contiguous */
        b=bocu1_Byte_గమ్యం_Trail[b];
        /* b<0 for an illegal trail byte value will result in return<0 below */
#ఒకవేళ BOCU1_MAX_TRAIL<0xff
    } లేకపోతే ఒకవేళ(b>BOCU1_MAX_TRAIL) {
        తిరిగి -99;
#అయితే_ముగింపు
    } లేకపోతే {
        b-=BOCU1_TRAIL_BYTE_స్థానభ్రంశం;
    }

    /* add trail byte into difference and decrement count */
    ఒకవేళ(count==1) {
        తిరిగి b;
    } లేకపోతే ఒకవేళ(count==2) {
        తిరిగి b*BOCU1_TRAIL_లెక్క;
    } లేకపోతే /* count==3 */ {
        తిరిగి b*(BOCU1_TRAIL_లెక్క*BOCU1_TRAIL_లెక్క);
    }
}

స్థిర శూన్య U_CALLCONV
Bocu1_గమ్యం_Unicode_With_Offsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                           UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    char16_t *లక్ష్యం;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు;
    int32_t *offsets;

    int32_t prev, count, diff, c;

    int8_t byte_సూచీ;
    uint8_t *బైట్లు;

    int32_t ఆధారం_సూచీ, తదుపరి_ఆధారం_సూచీ;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    ఆధారం = reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->ఆధారం);
    ఆధారం_హద్దు = reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->ఆధారం_హద్దు);
    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    లక్ష్యం_హద్దు=p_వాదనలు->లక్ష్యం_హద్దు;
    offsets=p_వాదనలు->offsets;

    /* get the converter state from UConverter */
    prev = static_cast<int32_t>(cnv->గమ్యం_Unicode_స్థితి);
    ఒకవేళ(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }
    diff=cnv->బహుళకం; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_సూచీ=cnv->గమ్యం_ULength;
    బైట్లు=cnv->గమ్యం_UBytes;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ=byte_సూచీ==0 ? 0 : -1;
    తదుపరి_ఆధారం_సూచీ=0;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    ఒకవేళ(count>0 && byte_సూచీ>0 && లక్ష్యం<లక్ష్యం_హద్దు) {
        వెళ్ళు getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(ఆధారం_హద్దు - ఆధారం);
    count = static_cast<int32_t>(p_వాదనలు->లక్ష్యం_హద్దు - లక్ష్యం);
    ఒకవేళ(count>diff) {
        count=diff;
    }
    అయితే(count>0) {
        ఒకవేళ(BOCU1_ప్రారంభం_NEG_2<=(c=*ఆధారం) && c<BOCU1_ప్రారంభం_POS_2) {
            c=prev+(c-BOCU1_మధ్య);
            ఒకవేళ(c<0x3000) {
                *లక్ష్యం++ = static_cast<char16_t>(c);
                *offsets++=తదుపరి_ఆధారం_సూచీ++;
                prev=BOCU1_SIMPLE_PREV(c);
            } లేకపోతే {
                ఆపు;
            }
        } లేకపోతే ఒకవేళ(c<=0x20) {
            ఒకవేళ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *లక్ష్యం++ = static_cast<char16_t>(c);
            *offsets++=తదుపరి_ఆధారం_సూచీ++;
        } లేకపోతే {
            ఆపు;
        }
        ++ఆధారం;
        --count;
    }
    ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ; /* wrong if offsets==nullptr but does not matter */

    /* decode a sequence of single and lead bytes */
    అయితే(ఆధారం<ఆధారం_హద్దు) {
        ఒకవేళ(లక్ష్యం>=లక్ష్యం_హద్దు) {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }

        ++తదుపరి_ఆధారం_సూచీ;
        c=*ఆధారం++;
        ఒకవేళ(BOCU1_ప్రారంభం_NEG_2<=c && c<BOCU1_ప్రారంభం_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=prev+(c-BOCU1_మధ్య);
            ఒకవేళ(c<0x3000) {
                *లక్ష్యం++ = static_cast<char16_t>(c);
                *offsets++=ఆధారం_సూచీ;
                prev=BOCU1_SIMPLE_PREV(c);
                ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                వెళ్ళు fastSingle;
            }
        } లేకపోతే ఒకవేళ(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            ఒకవేళ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *లక్ష్యం++ = static_cast<char16_t>(c);
            *offsets++=ఆధారం_సూచీ;
            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
            కొనసాగు;
        } లేకపోతే ఒకవేళ(BOCU1_ప్రారంభం_NEG_3<=c && c<BOCU1_ప్రారంభం_POS_3 && ఆధారం<ఆధారం_హద్దు) {
            /* Optimize two-byte case. */
            ఒకవేళ(c>=BOCU1_మధ్య) {
                diff = (c - BOCU1_ప్రారంభం_POS_2) * BOCU1_TRAIL_లెక్క + BOCU1_వ్యాప్తి_POS_1 + 1;
            } లేకపోతే {
                diff = (c - BOCU1_ప్రారంభం_NEG_2) * BOCU1_TRAIL_లెక్క + BOCU1_వ్యాప్తి_NEG_1;
            }

            /* trail byte */
            ++తదుపరి_ఆధారం_సూచీ;
            c=decodeBocu1TrailByte(1, *ఆధారం++);
            ఒకవేళ (c < 0 || static_cast<uint32_t>(c = prev + diff + c) > 0x10ffff) {
                బైట్లు[0]=ఆధారం[-2];
                బైట్లు[1]=ఆధారం[-1];
                byte_సూచీ=2;
                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                ఆపు;
            }
        } లేకపోతే ఒకవేళ(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            prev=BOCU1_ASCII_PREV;
            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
            కొనసాగు;
        } లేకపోతే {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            బైట్లు[0] = static_cast<uint8_t>(c);
            byte_సూచీ=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            కొరకు(;;) {
                ఒకవేళ(ఆధారం>=ఆధారం_హద్దు) {
                    వెళ్ళు endloop;
                }
                ++తదుపరి_ఆధారం_సూచీ;
                c=బైట్లు[byte_సూచీ++]=*ఆధారం++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                ఒకవేళ(c<0) {
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    వెళ్ళు endloop;
                }

                diff+=c;
                ఒకవేళ(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_సూచీ=0;
                    c=prev+diff;
                    ఒకవేళ (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                        వెళ్ళు endloop;
                    }
                    ఆపు;
                }
            }
        }

        /* calculate the next prev and output c */
        prev=BOCU1_PREV(c);
        ఒకవేళ(c<=0xffff) {
            *లక్ష్యం++ = static_cast<char16_t>(c);
            *offsets++=ఆధారం_సూచీ;
        } లేకపోతే {
            /* output surrogate pair */
            *లక్ష్యం++=U16_LEAD(c);
            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                *లక్ష్యం++=U16_TRAIL(c);
                *offsets++=ఆధారం_సూచీ;
                *offsets++=ఆధారం_సూచీ;
            } లేకపోతే {
                /* target overflow */
                *offsets++=ఆధారం_సూచీ;
                cnv->UChar_Error_బఫరు[0]=U16_TRAIL(c);
                cnv->UChar_Error_బఫరు_నిడివి=1;
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }
        }
        ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
    }
endloop:

    ఒకవేళ(*p_Error_సంకేతం==U_ILLEGAL_CHAR_దొరికింది) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->గమ్యం_Unicode_స్థితి=BOCU1_ASCII_PREV;
        cnv->బహుళకం=0;
    } లేకపోతే {
        /* set the converter state back into UConverter */
        cnv->గమ్యం_Unicode_స్థితి = static_cast<uint32_t>(prev);
        cnv->బహుళకం = static_cast<int32_t>(static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->గమ్యం_ULength=byte_సూచీ;

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం = reinterpret_cast<స్థిరరాశి అక్షరం*>(ఆధారం);
    p_వాదనలు->లక్ష్యం=లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

/*
 * Identical to _Bocu1ToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
స్థిర శూన్య U_CALLCONV
Bocu1_గమ్యం_Unicode(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    char16_t *లక్ష్యం;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు;

    int32_t prev, count, diff, c;

    int8_t byte_సూచీ;
    uint8_t *బైట్లు;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    ఆధారం = reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->ఆధారం);
    ఆధారం_హద్దు = reinterpret_cast<స్థిరరాశి uint8_t*>(p_వాదనలు->ఆధారం_హద్దు);
    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    లక్ష్యం_హద్దు=p_వాదనలు->లక్ష్యం_హద్దు;

    /* get the converter state from UConverter */
    prev = static_cast<int32_t>(cnv->గమ్యం_Unicode_స్థితి);
    ఒకవేళ(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }
    diff=cnv->బహుళకం; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_సూచీ=cnv->గమ్యం_ULength;
    బైట్లు=cnv->గమ్యం_UBytes;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    ఒకవేళ(count>0 && byte_సూచీ>0 && లక్ష్యం<లక్ష్యం_హద్దు) {
        వెళ్ళు getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(ఆధారం_హద్దు - ఆధారం);
    count = static_cast<int32_t>(p_వాదనలు->లక్ష్యం_హద్దు - లక్ష్యం);
    ఒకవేళ(count>diff) {
        count=diff;
    }
    అయితే(count>0) {
        ఒకవేళ(BOCU1_ప్రారంభం_NEG_2<=(c=*ఆధారం) && c<BOCU1_ప్రారంభం_POS_2) {
            c=prev+(c-BOCU1_మధ్య);
            ఒకవేళ(c<0x3000) {
                *లక్ష్యం++ = static_cast<char16_t>(c);
                prev=BOCU1_SIMPLE_PREV(c);
            } లేకపోతే {
                ఆపు;
            }
        } లేకపోతే ఒకవేళ(c<=0x20) {
            ఒకవేళ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *లక్ష్యం++ = static_cast<char16_t>(c);
        } లేకపోతే {
            ఆపు;
        }
        ++ఆధారం;
        --count;
    }

    /* decode a sequence of single and lead bytes */
    అయితే(ఆధారం<ఆధారం_హద్దు) {
        ఒకవేళ(లక్ష్యం>=లక్ష్యం_హద్దు) {
            /* target is full */
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }

        c=*ఆధారం++;
        ఒకవేళ(BOCU1_ప్రారంభం_NEG_2<=c && c<BOCU1_ప్రారంభం_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=prev+(c-BOCU1_మధ్య);
            ఒకవేళ(c<0x3000) {
                *లక్ష్యం++ = static_cast<char16_t>(c);
                prev=BOCU1_SIMPLE_PREV(c);
                వెళ్ళు fastSingle;
            }
        } లేకపోతే ఒకవేళ(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            ఒకవేళ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *లక్ష్యం++ = static_cast<char16_t>(c);
            కొనసాగు;
        } లేకపోతే ఒకవేళ(BOCU1_ప్రారంభం_NEG_3<=c && c<BOCU1_ప్రారంభం_POS_3 && ఆధారం<ఆధారం_హద్దు) {
            /* Optimize two-byte case. */
            ఒకవేళ(c>=BOCU1_మధ్య) {
                diff = (c - BOCU1_ప్రారంభం_POS_2) * BOCU1_TRAIL_లెక్క + BOCU1_వ్యాప్తి_POS_1 + 1;
            } లేకపోతే {
                diff = (c - BOCU1_ప్రారంభం_NEG_2) * BOCU1_TRAIL_లెక్క + BOCU1_వ్యాప్తి_NEG_1;
            }

            /* trail byte */
            c=decodeBocu1TrailByte(1, *ఆధారం++);
            ఒకవేళ (c < 0 || static_cast<uint32_t>(c = prev + diff + c) > 0x10ffff) {
                బైట్లు[0]=ఆధారం[-2];
                బైట్లు[1]=ఆధారం[-1];
                byte_సూచీ=2;
                *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                ఆపు;
            }
        } లేకపోతే ఒకవేళ(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            prev=BOCU1_ASCII_PREV;
            కొనసాగు;
        } లేకపోతే {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            బైట్లు[0] = static_cast<uint8_t>(c);
            byte_సూచీ=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            కొరకు(;;) {
                ఒకవేళ(ఆధారం>=ఆధారం_హద్దు) {
                    వెళ్ళు endloop;
                }
                c=బైట్లు[byte_సూచీ++]=*ఆధారం++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                ఒకవేళ(c<0) {
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    వెళ్ళు endloop;
                }

                diff+=c;
                ఒకవేళ(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_సూచీ=0;
                    c=prev+diff;
                    ఒకవేళ (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                        వెళ్ళు endloop;
                    }
                    ఆపు;
                }
            }
        }

        /* calculate the next prev and output c */
        prev=BOCU1_PREV(c);
        ఒకవేళ(c<=0xffff) {
            *లక్ష్యం++ = static_cast<char16_t>(c);
        } లేకపోతే {
            /* output surrogate pair */
            *లక్ష్యం++=U16_LEAD(c);
            ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                *లక్ష్యం++=U16_TRAIL(c);
            } లేకపోతే {
                /* target overflow */
                cnv->UChar_Error_బఫరు[0]=U16_TRAIL(c);
                cnv->UChar_Error_బఫరు_నిడివి=1;
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }
        }
    }
endloop:

    ఒకవేళ(*p_Error_సంకేతం==U_ILLEGAL_CHAR_దొరికింది) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->గమ్యం_Unicode_స్థితి=BOCU1_ASCII_PREV;
        cnv->బహుళకం=0;
    } లేకపోతే {
        /* set the converter state back into UConverter */
        cnv->గమ్యం_Unicode_స్థితి = static_cast<uint32_t>(prev);
        cnv->బహుళకం = (static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->గమ్యం_ULength=byte_సూచీ;

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం = reinterpret_cast<స్థిరరాశి అక్షరం*>(ఆధారం);
    p_వాదనలు->లక్ష్యం=లక్ష్యం;
}

/* miscellaneous ------------------------------------------------------------ */

స్థిర స్థిరరాశి UConverterImpl _Bocu1Impl={
    UCNV_BOCU1,

    శూన్యసూచిక,
    శూన్యసూచిక,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,

    Bocu1_గమ్యం_Unicode,
    Bocu1_గమ్యం_Unicode_With_Offsets,
    _Bocu1FromUnicode,
    _Bocu1FromUnicodeWithOffsets,
    శూన్యసూచిక,

    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    ucnv_get_Complete_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};

స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం Bocu1_Static_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "BOCU-1",
    1214, /* CCSID for BOCU-1 */
    UCNV_IBM, UCNV_BOCU1,
    1, 4, /* one char16_t generates at least 1 byte and at most 4 bytes */
    { 0x1a, 0, 0, 0 }, 1, /* BOCU-1 never needs to write a subchar */
    అబద్ధం, అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

స్థిరరాశి UConverter_Shared_దత్తాంశం Bocu1_దత్తాంశం=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&Bocu1_Static_దత్తాంశం, &_Bocu1Impl);

#అయితే_ముగింపు
