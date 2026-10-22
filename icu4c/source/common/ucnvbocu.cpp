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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_cb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ASCII_PREV        0x40

/* bounding byte values for differences */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_MIN               0x21
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਵਿਚਕਾਰ            0x90
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_MAX_LEAD          0xfe
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_MAX_TRAIL         0xff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_RESET             0xff

/* number of lead bytes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਗਿਣਤੀ             (BOCU1_MAX_LEAD-BOCU1_MIN+1)

/* adjust trail byte counts for the use of some C0 control byte values */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_TRAIL_CONTROLS_ਗਿਣਤੀ  20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_TRAIL_BYTE_ਵਿੱਥ     (BOCU1_MIN-BOCU1_TRAIL_CONTROLS_ਗਿਣਤੀ)

/* number of trail bytes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_TRAIL_ਗਿਣਤੀ       ((BOCU1_MAX_TRAIL-BOCU1_MIN+1)+BOCU1_TRAIL_CONTROLS_ਗਿਣਤੀ)

/*
 * number of positive and negative single-byte codes
 * (counting 0==BOCU1_MIDDLE among the positive ones)
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_SINGLE            64

/* number of lead bytes for positive and negative 2/3/4-byte sequences */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_LEAD_2            43
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_LEAD_3            3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_LEAD_4            1

/* The difference value range for single-byters. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਪਹੁੰਚ_POS_1   (BOCU1_SINGLE-1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਪਹੁੰਚ_NEG_1   (-BOCU1_SINGLE)

/* The difference value range for double-byters. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਪਹੁੰਚ_POS_2   (BOCU1_ਪਹੁੰਚ_POS_1+BOCU1_LEAD_2*BOCU1_TRAIL_ਗਿਣਤੀ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਪਹੁੰਚ_NEG_2   (BOCU1_ਪਹੁੰਚ_NEG_1-BOCU1_LEAD_2*BOCU1_TRAIL_ਗਿਣਤੀ)

/* The difference value range for 3-byters. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਪਹੁੰਚ_POS_3   \
    (BOCU1_ਪਹੁੰਚ_POS_2+BOCU1_LEAD_3*BOCU1_TRAIL_ਗਿਣਤੀ*BOCU1_TRAIL_ਗਿਣਤੀ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਪਹੁੰਚ_NEG_3   (BOCU1_ਪਹੁੰਚ_NEG_2-BOCU1_LEAD_3*BOCU1_TRAIL_ਗਿਣਤੀ*BOCU1_TRAIL_ਗਿਣਤੀ)

/* The lead byte start values. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਸ਼ੁਰੂ_POS_2   (BOCU1_ਵਿਚਕਾਰ+BOCU1_ਪਹੁੰਚ_POS_1+1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਸ਼ੁਰੂ_POS_3   (BOCU1_ਸ਼ੁਰੂ_POS_2+BOCU1_LEAD_2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਸ਼ੁਰੂ_POS_4   (BOCU1_ਸ਼ੁਰੂ_POS_3+BOCU1_LEAD_3)
     /* ==BOCU1_MAX_LEAD */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਸ਼ੁਰੂ_NEG_2   (BOCU1_ਵਿਚਕਾਰ+BOCU1_ਪਹੁੰਚ_NEG_1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਸ਼ੁਰੂ_NEG_3   (BOCU1_ਸ਼ੁਰੂ_NEG_2-BOCU1_LEAD_2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਸ਼ੁਰੂ_NEG_4   (BOCU1_ਸ਼ੁਰੂ_NEG_3-BOCU1_LEAD_3)
     /* ==BOCU1_MIN+1 */

/* The length of a byte sequence, according to the lead byte (!=BOCU1_RESET). */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਲੰਮਾਈ_FROM_LEAD(lead) \
    ((BOCU1_ਸ਼ੁਰੂ_NEG_2<=(lead) && (lead)<BOCU1_ਸ਼ੁਰੂ_POS_2) ? 1 : \
     (BOCU1_ਸ਼ੁਰੂ_NEG_3<=(lead) && (lead)<BOCU1_ਸ਼ੁਰੂ_POS_3) ? 2 : \
     (BOCU1_ਸ਼ੁਰੂ_NEG_4<=(lead) && (lead)<BOCU1_ਸ਼ੁਰੂ_POS_4) ? 3 : 4)

/* The length of a byte sequence, according to its packed form. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_ਲੰਮਾਈ_FROM_PACKED(packed) \
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(t) ((t)>=BOCU1_TRAIL_CONTROLS_ਗਿਣਤੀ ? (t)+BOCU1_TRAIL_BYTE_ਵਿੱਥ : bocu1_Trail_ਮੰਜ਼ਿਲ_Byte[t])

/*
 * Byte value map for control codes,
 * from external byte values 0x00..0x20
 * to trail byte values 0..19 (0..0x13) as used in the difference calculation.
 * External byte values that are illegal as trail bytes are mapped to -1.
 */
ਸਥਿਰ ਅਚਲ int8_t
bocu1_Byte_ਮੰਜ਼ਿਲ_Trail[BOCU1_MIN]={
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
ਸਥਿਰ ਅਚਲ int8_t
bocu1_Trail_ਮੰਜ਼ਿਲ_Byte[BOCU1_TRAIL_CONTROLS_ਗਿਣਤੀ]={
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NEGDIVMOD(n, d, m) UPRV_ਖੰਡ_MACRO_BEGIN { \
    (m)=(n)%(d); \
    (n)/=(d); \
    ਜੇ((m)<0) { \
        --(n); \
        (m)+=(d); \
    } \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ

/* Faster versions of packDiff() for single-byte-encoded diff values. */

/** Is a diff value encodable in a single byte? */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIFF_IS_SINGLE(diff) (BOCU1_ਪਹੁੰਚ_NEG_1<=(diff) && (diff)<=BOCU1_ਪਹੁੰਚ_POS_1)

/** Encode a diff value in a single byte. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PACK_SINGLE_DIFF(diff) (BOCU1_ਵਿਚਕਾਰ+(diff))

/** Is a diff value encodable in two bytes? */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIFF_IS_DOUBLE(diff) (BOCU1_ਪਹੁੰਚ_NEG_2<=(diff) && (diff)<=BOCU1_ਪਹੁੰਚ_POS_2)

/* BOCU-1 implementation functions ------------------------------------------ */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_SIMPLE_PREV(c) (((c)&~0x7f)+BOCU1_ASCII_PREV)

/**
 * Compute the next "previous" value for differencing
 * from the current code point.
 *
 * @param c current code point, 0x3040..0xd7a3 (rest handled by macro below)
 * @return "previous code point" state value
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
bocu1Prev(int32_t c) {
    /* compute new prev */
    ਜੇ(/* 0x3040<=c && */ c<=0x309f) {
        /* Hiragana is not 128-aligned */
        ਵਾਪਸ 0x3070;
    } ਨਹੀਂਤਾਂ ਜੇ(0x4e00<=c && c<=0x9fa5) {
        /* CJK Unihan */
        ਵਾਪਸ 0x4e00-BOCU1_ਪਹੁੰਚ_NEG_2;
    } ਨਹੀਂਤਾਂ ਜੇ(0xac00<=c /* && c<=0xd7a3 */) {
        /* Korean Hangul */
        ਵਾਪਸ (0xd7a3+0xac00)/2;
    } ਨਹੀਂਤਾਂ {
        /* mostly small scripts */
        ਵਾਪਸ BOCU1_SIMPLE_PREV(c);
    }
}

/** Fast version of bocu1Prev() for most scripts. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOCU1_PREV(c) ((c)<0x3040 || (c)>0xd7a3 ? BOCU1_SIMPLE_PREV(c) : bocu1Prev(c))

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
ਸਥਿਰ int32_t
packDiff(int32_t diff) {
    int32_t ਨਤੀਜਾ, m;

    U_ASSERT(!DIFF_IS_SINGLE(diff)); /* assume we won't be called where diff==BOCU1_REACH_NEG_1=-64 */
    ਜੇ(diff>=BOCU1_ਪਹੁੰਚ_NEG_1) {
        /* mostly positive differences, and single-byte negative ones */
#ਜੇ 0   /* single-byte case handled in macros, see below */
        ਜੇ(diff<=BOCU1_ਪਹੁੰਚ_POS_1) {
            /* single byte */
            ਵਾਪਸ 0x01000000|(BOCU1_ਵਿਚਕਾਰ+diff);
        } ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
        ਜੇ(diff<=BOCU1_ਪਹੁੰਚ_POS_2) {
            /* two bytes */
            diff-=BOCU1_ਪਹੁੰਚ_POS_1+1;
            ਨਤੀਜਾ=0x02000000;

            m=diff%BOCU1_TRAIL_ਗਿਣਤੀ;
            diff/=BOCU1_TRAIL_ਗਿਣਤੀ;
            ਨਤੀਜਾ|=BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(m);

            ਨਤੀਜਾ|=(BOCU1_ਸ਼ੁਰੂ_POS_2+diff)<<8;
        } ਨਹੀਂਤਾਂ ਜੇ(diff<=BOCU1_ਪਹੁੰਚ_POS_3) {
            /* three bytes */
            diff-=BOCU1_ਪਹੁੰਚ_POS_2+1;
            ਨਤੀਜਾ=0x03000000;

            m=diff%BOCU1_TRAIL_ਗਿਣਤੀ;
            diff/=BOCU1_TRAIL_ਗਿਣਤੀ;
            ਨਤੀਜਾ|=BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(m);

            m=diff%BOCU1_TRAIL_ਗਿਣਤੀ;
            diff/=BOCU1_TRAIL_ਗਿਣਤੀ;
            ਨਤੀਜਾ|=BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(m)<<8;

            ਨਤੀਜਾ|=(BOCU1_ਸ਼ੁਰੂ_POS_3+diff)<<16;
        } ਨਹੀਂਤਾਂ {
            /* four bytes */
            diff-=BOCU1_ਪਹੁੰਚ_POS_3+1;

            m=diff%BOCU1_TRAIL_ਗਿਣਤੀ;
            diff/=BOCU1_TRAIL_ਗਿਣਤੀ;
            ਨਤੀਜਾ=BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(m);

            m=diff%BOCU1_TRAIL_ਗਿਣਤੀ;
            diff/=BOCU1_TRAIL_ਗਿਣਤੀ;
            ਨਤੀਜਾ|=BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(m)<<8;

            /*
             * We know that / and % would deliver quotient 0 and rest=diff.
             * Avoid division and modulo for performance.
             */
            ਨਤੀਜਾ|=BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(diff)<<16;

            ਨਤੀਜਾ |= static_cast<uint32_t>(BOCU1_ਸ਼ੁਰੂ_POS_4) << 24;
        }
    } ਨਹੀਂਤਾਂ {
        /* two- to four-byte negative differences */
        ਜੇ(diff>=BOCU1_ਪਹੁੰਚ_NEG_2) {
            /* two bytes */
            diff-=BOCU1_ਪਹੁੰਚ_NEG_1;
            ਨਤੀਜਾ=0x02000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_ਗਿਣਤੀ, m);
            ਨਤੀਜਾ|=BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(m);

            ਨਤੀਜਾ|=(BOCU1_ਸ਼ੁਰੂ_NEG_2+diff)<<8;
        } ਨਹੀਂਤਾਂ ਜੇ(diff>=BOCU1_ਪਹੁੰਚ_NEG_3) {
            /* three bytes */
            diff-=BOCU1_ਪਹੁੰਚ_NEG_2;
            ਨਤੀਜਾ=0x03000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_ਗਿਣਤੀ, m);
            ਨਤੀਜਾ|=BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_ਗਿਣਤੀ, m);
            ਨਤੀਜਾ|=BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(m)<<8;

            ਨਤੀਜਾ|=(BOCU1_ਸ਼ੁਰੂ_NEG_3+diff)<<16;
        } ਨਹੀਂਤਾਂ {
            /* four bytes */
            diff-=BOCU1_ਪਹੁੰਚ_NEG_3;

            NEGDIVMOD(diff, BOCU1_TRAIL_ਗਿਣਤੀ, m);
            ਨਤੀਜਾ=BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_ਗਿਣਤੀ, m);
            ਨਤੀਜਾ|=BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(m)<<8;

            /*
             * We know that NEGDIVMOD would deliver
             * quotient -1 and rest=diff+BOCU1_TRAIL_COUNT.
             * Avoid division and modulo for performance.
             */
            m=diff+BOCU1_TRAIL_ਗਿਣਤੀ;
            ਨਤੀਜਾ|=BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(m)<<16;

            ਨਤੀਜਾ|=BOCU1_MIN<<24;
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}


ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_Bocu1FromUnicodeWithOffsets(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ char16_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    int32_t *offsets;

    int32_t prev, c, diff;

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ, ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ = reinterpret_cast<uint8_t*>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    prev = static_cast<int32_t>(cnv->from_Unicode_ਸਥਿਤੀ);
    ਜੇ(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ= c==0 ? 0 : -1;
    ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    /* conversion loop */
    ਜੇ(c!=0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
        ਜਾਓ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(ਸਰੋਤ_ਹੱਦਬੰਦੀ - ਸਰੋਤ);
    ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>diff) {
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=diff;
    }
    ਜਦੋਂ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0 && (c=*ਸਰੋਤ)<0x3000) {
        ਜੇ(c<=0x20) {
            ਜੇ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(c);
            *offsets++=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
            ++ਸਰੋਤ;
            --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
        } ਨਹੀਂਤਾਂ {
            diff=c-prev;
            ਜੇ(DIFF_IS_SINGLE(diff)) {
                prev=BOCU1_SIMPLE_PREV(c);
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                ++ਸਰੋਤ;
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
            } ਨਹੀਂਤਾਂ {
                ਰੋਕੋ;
            }
        }
    }
    /* restore real values */
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) - ਨਿਸ਼ਾਨਾ);
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ; /* wrong if offsets==nullptr but does not matter */

    /* regular loop for all cases */
    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
            c=*ਸਰੋਤ++;
            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

            ਜੇ(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                ਜੇ(c!=0x20) {
                    prev=BOCU1_ASCII_PREV;
                }
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(c);
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;

                ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                ਜਾਰੀ;
            }

            ਜੇ(U16_IS_LEAD(c)) {
getTrail:
                ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                    /* test the following code unit */
                    char16_t trail=*ਸਰੋਤ;
                    ਜੇ(U16_IS_TRAIL(trail)) {
                        ++ਸਰੋਤ;
                        ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } ਨਹੀਂਤਾਂ {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    ਰੋਕੋ;
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
            ਜੇ(DIFF_IS_SINGLE(diff)) {
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                ਜੇ(c<0x3000) {
                    ਜਾਓ fastSingle;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(DIFF_IS_DOUBLE(diff) && 2<=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
                /* optimize 2-byte case */
                int32_t m;

                ਜੇ(diff>=0) {
                    diff-=BOCU1_ਪਹੁੰਚ_POS_1+1;
                    m=diff%BOCU1_TRAIL_ਗਿਣਤੀ;
                    diff/=BOCU1_TRAIL_ਗਿਣਤੀ;
                    diff+=BOCU1_ਸ਼ੁਰੂ_POS_2;
                } ਨਹੀਂਤਾਂ {
                    diff-=BOCU1_ਪਹੁੰਚ_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_ਗਿਣਤੀ, m);
                    diff+=BOCU1_ਸ਼ੁਰੂ_NEG_2;
                }
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff);
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(m));
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=2;
                ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            } ਨਹੀਂਤਾਂ {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_ਲੰਮਾਈ_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                ਜੇ(length<=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
                    ਚੋਣ(length) {
                        /* each branch falls through to the next one */
                    ਹਾਲਤ 4:
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff >> 24);
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        U_FALLTHROUGH;
                    ਹਾਲਤ 3:
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        U_FALLTHROUGH;
                    ਹਾਲਤ 2:
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    /* case 1: handled above */
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff);
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        U_FALLTHROUGH;
                    ਮੂਲ:
                        /* will never occur */
                        ਰੋਕੋ;
                    }
                    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=length;
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                } ਨਹੀਂਤਾਂ {
                    uint8_t *char_Error_ਬਫਰ;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                    char_Error_ਬਫਰ=(uint8_t *)cnv->char_Error_ਬਫਰ;
                    ਚੋਣ(length) {
                        /* each branch falls through to the next one */
                    ਹਾਲਤ 3:
                        *char_Error_ਬਫਰ++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    ਹਾਲਤ 2:
                        *char_Error_ਬਫਰ++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    ਹਾਲਤ 1:
                        *char_Error_ਬਫਰ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    ਮੂਲ:
                        /* will never occur */
                        ਰੋਕੋ;
                    }
                    cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    ਚੋਣ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
                        /* each branch falls through to the next one */
                    ਹਾਲਤ 3:
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        U_FALLTHROUGH;
                    ਹਾਲਤ 2:
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        U_FALLTHROUGH;
                    ਹਾਲਤ 1:
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff);
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        U_FALLTHROUGH;
                    ਮੂਲ:
                        /* will never occur */
                        ਰੋਕੋ;
                    }

                    /* target overflow */
                    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=0;
                    *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    ਰੋਕੋ;
                }
            }
        } ਨਹੀਂਤਾਂ {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_ਸਥਿਤੀ = static_cast<uint32_t>(prev);

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

/*
 * Identical to _Bocu1FromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_Bocu1FromUnicode(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ char16_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;

    int32_t prev, c, diff;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ = reinterpret_cast<uint8_t*>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    prev = static_cast<int32_t>(cnv->from_Unicode_ਸਥਿਤੀ);
    ਜੇ(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }

    /* conversion loop */
    ਜੇ(c!=0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
        ਜਾਓ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(ਸਰੋਤ_ਹੱਦਬੰਦੀ - ਸਰੋਤ);
    ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>diff) {
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=diff;
    }
    ਜਦੋਂ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0 && (c=*ਸਰੋਤ)<0x3000) {
        ਜੇ(c<=0x20) {
            ਜੇ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(c);
        } ਨਹੀਂਤਾਂ {
            diff=c-prev;
            ਜੇ(DIFF_IS_SINGLE(diff)) {
                prev=BOCU1_SIMPLE_PREV(c);
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
            } ਨਹੀਂਤਾਂ {
                ਰੋਕੋ;
            }
        }
        ++ਸਰੋਤ;
        --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    }
    /* restore real values */
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ = static_cast<int32_t>(reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) - ਨਿਸ਼ਾਨਾ);

    /* regular loop for all cases */
    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
            c=*ਸਰੋਤ++;

            ਜੇ(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                ਜੇ(c!=0x20) {
                    prev=BOCU1_ASCII_PREV;
                }
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(c);
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                ਜਾਰੀ;
            }

            ਜੇ(U16_IS_LEAD(c)) {
getTrail:
                ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                    /* test the following code unit */
                    char16_t trail=*ਸਰੋਤ;
                    ਜੇ(U16_IS_TRAIL(trail)) {
                        ++ਸਰੋਤ;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } ਨਹੀਂਤਾਂ {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    ਰੋਕੋ;
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
            ਜੇ(DIFF_IS_SINGLE(diff)) {
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                ਜੇ(c<0x3000) {
                    ਜਾਓ fastSingle;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(DIFF_IS_DOUBLE(diff) && 2<=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
                /* optimize 2-byte case */
                int32_t m;

                ਜੇ(diff>=0) {
                    diff-=BOCU1_ਪਹੁੰਚ_POS_1+1;
                    m=diff%BOCU1_TRAIL_ਗਿਣਤੀ;
                    diff/=BOCU1_TRAIL_ਗਿਣਤੀ;
                    diff+=BOCU1_ਸ਼ੁਰੂ_POS_2;
                } ਨਹੀਂਤਾਂ {
                    diff-=BOCU1_ਪਹੁੰਚ_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_ਗਿਣਤੀ, m);
                    diff+=BOCU1_ਸ਼ੁਰੂ_NEG_2;
                }
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff);
                *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(BOCU1_TRAIL_ਮੰਜ਼ਿਲ_BYTE(m));
                ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=2;
            } ਨਹੀਂਤਾਂ {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_ਲੰਮਾਈ_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                ਜੇ(length<=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
                    ਚੋਣ(length) {
                        /* each branch falls through to the next one */
                    ਹਾਲਤ 4:
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff >> 24);
                        U_FALLTHROUGH;
                    ਹਾਲਤ 3:
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff >> 16);
                    /* case 2: handled above */
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff >> 8);
                    /* case 1: handled above */
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    ਮੂਲ:
                        /* will never occur */
                        ਰੋਕੋ;
                    }
                    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=length;
                } ਨਹੀਂਤਾਂ {
                    uint8_t *char_Error_ਬਫਰ;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                    char_Error_ਬਫਰ=(uint8_t *)cnv->char_Error_ਬਫਰ;
                    ਚੋਣ(length) {
                        /* each branch falls through to the next one */
                    ਹਾਲਤ 3:
                        *char_Error_ਬਫਰ++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    ਹਾਲਤ 2:
                        *char_Error_ਬਫਰ++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    ਹਾਲਤ 1:
                        *char_Error_ਬਫਰ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    ਮੂਲ:
                        /* will never occur */
                        ਰੋਕੋ;
                    }
                    cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    ਚੋਣ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
                        /* each branch falls through to the next one */
                    ਹਾਲਤ 3:
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    ਹਾਲਤ 2:
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    ਹਾਲਤ 1:
                        *ਨਿਸ਼ਾਨਾ++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    ਮੂਲ:
                        /* will never occur */
                        ਰੋਕੋ;
                    }

                    /* target overflow */
                    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=0;
                    *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    ਰੋਕੋ;
                }
            }
        } ਨਹੀਂਤਾਂ {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_ਸਥਿਤੀ = static_cast<uint32_t>(prev);

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
}

/* BOCU-1-to-Unicode conversion functions ----------------------------------- */

/**
 * Function for BOCU-1 decoder; handles multi-byte lead bytes.
 *
 * @param b lead byte;
 *          BOCU1_MIN<=b<BOCU1_START_NEG_2 or BOCU1_START_POS_2<=b<BOCU1_MAX_LEAD
 * @return (diff<<2)|count
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
decodeBocu1LeadByte(int32_t b) {
    int32_t diff, count;

    ਜੇ(b>=BOCU1_ਸ਼ੁਰੂ_NEG_2) {
        /* positive difference */
        ਜੇ(b<BOCU1_ਸ਼ੁਰੂ_POS_3) {
            /* two bytes */
            diff = (b - BOCU1_ਸ਼ੁਰੂ_POS_2) * BOCU1_TRAIL_ਗਿਣਤੀ + BOCU1_ਪਹੁੰਚ_POS_1 + 1;
            count=1;
        } ਨਹੀਂਤਾਂ ਜੇ(b<BOCU1_ਸ਼ੁਰੂ_POS_4) {
            /* three bytes */
            diff = (b - BOCU1_ਸ਼ੁਰੂ_POS_3) * BOCU1_TRAIL_ਗਿਣਤੀ * BOCU1_TRAIL_ਗਿਣਤੀ + BOCU1_ਪਹੁੰਚ_POS_2 + 1;
            count=2;
        } ਨਹੀਂਤਾਂ {
            /* four bytes */
            diff=BOCU1_ਪਹੁੰਚ_POS_3+1;
            count=3;
        }
    } ਨਹੀਂਤਾਂ {
        /* negative difference */
        ਜੇ(b>=BOCU1_ਸ਼ੁਰੂ_NEG_3) {
            /* two bytes */
            diff = (b - BOCU1_ਸ਼ੁਰੂ_NEG_2) * BOCU1_TRAIL_ਗਿਣਤੀ + BOCU1_ਪਹੁੰਚ_NEG_1;
            count=1;
        } ਨਹੀਂਤਾਂ ਜੇ(b>BOCU1_MIN) {
            /* three bytes */
            diff = (b - BOCU1_ਸ਼ੁਰੂ_NEG_3) * BOCU1_TRAIL_ਗਿਣਤੀ * BOCU1_TRAIL_ਗਿਣਤੀ + BOCU1_ਪਹੁੰਚ_NEG_2;
            count=2;
        } ਨਹੀਂਤਾਂ {
            /* four bytes */
            diff=-BOCU1_TRAIL_ਗਿਣਤੀ*BOCU1_TRAIL_ਗਿਣਤੀ*BOCU1_TRAIL_ਗਿਣਤੀ+BOCU1_ਪਹੁੰਚ_NEG_3;
            count=3;
        }
    }

    /* return the state for decoding the trail byte(s) */
    ਵਾਪਸ (static_cast<uint32_t>(diff) << 2) | count;
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
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
decodeBocu1TrailByte(int32_t count, int32_t b) {
    ਜੇ(b<=0x20) {
        /* skip some C0 controls and make the trail byte range contiguous */
        b=bocu1_Byte_ਮੰਜ਼ਿਲ_Trail[b];
        /* b<0 for an illegal trail byte value will result in return<0 below */
#ਜੇ BOCU1_MAX_TRAIL<0xff
    } ਨਹੀਂਤਾਂ ਜੇ(b>BOCU1_MAX_TRAIL) {
        ਵਾਪਸ -99;
#ਜੇ_ਅੰਤ
    } ਨਹੀਂਤਾਂ {
        b-=BOCU1_TRAIL_BYTE_ਵਿੱਥ;
    }

    /* add trail byte into difference and decrement count */
    ਜੇ(count==1) {
        ਵਾਪਸ b;
    } ਨਹੀਂਤਾਂ ਜੇ(count==2) {
        ਵਾਪਸ b*BOCU1_TRAIL_ਗਿਣਤੀ;
    } ਨਹੀਂਤਾਂ /* count==3 */ {
        ਵਾਪਸ b*(BOCU1_TRAIL_ਗਿਣਤੀ*BOCU1_TRAIL_ਗਿਣਤੀ);
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
Bocu1_ਮੰਜ਼ਿਲ_Unicode_With_Offsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                           UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    char16_t *ਨਿਸ਼ਾਨਾ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    int32_t *offsets;

    int32_t prev, count, diff, c;

    int8_t byte_ਸੂਚਕਾਂਕ;
    uint8_t *ਬਾਈਟਾਂ;

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ, ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    ਸਰੋਤ = reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਸਰੋਤ);
    ਸਰੋਤ_ਹੱਦਬੰਦੀ = reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ);
    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    offsets=p_ਦਲੀਲਾਂ->offsets;

    /* get the converter state from UConverter */
    prev = static_cast<int32_t>(cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ);
    ਜੇ(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }
    diff=cnv->ਬਹੁਲਕ; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_ਸੂਚਕਾਂਕ=cnv->ਮੰਜ਼ਿਲ_ULength;
    ਬਾਈਟਾਂ=cnv->ਮੰਜ਼ਿਲ_UBytes;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=byte_ਸੂਚਕਾਂਕ==0 ? 0 : -1;
    ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    ਜੇ(count>0 && byte_ਸੂਚਕਾਂਕ>0 && ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        ਜਾਓ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(ਸਰੋਤ_ਹੱਦਬੰਦੀ - ਸਰੋਤ);
    count = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - ਨਿਸ਼ਾਨਾ);
    ਜੇ(count>diff) {
        count=diff;
    }
    ਜਦੋਂ(count>0) {
        ਜੇ(BOCU1_ਸ਼ੁਰੂ_NEG_2<=(c=*ਸਰੋਤ) && c<BOCU1_ਸ਼ੁਰੂ_POS_2) {
            c=prev+(c-BOCU1_ਵਿਚਕਾਰ);
            ਜੇ(c<0x3000) {
                *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(c);
                *offsets++=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
                prev=BOCU1_SIMPLE_PREV(c);
            } ਨਹੀਂਤਾਂ {
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(c<=0x20) {
            ਜੇ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(c);
            *offsets++=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ++;
        } ਨਹੀਂਤਾਂ {
            ਰੋਕੋ;
        }
        ++ਸਰੋਤ;
        --count;
    }
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ; /* wrong if offsets==nullptr but does not matter */

    /* decode a sequence of single and lead bytes */
    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਜੇ(ਨਿਸ਼ਾਨਾ>=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }

        ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
        c=*ਸਰੋਤ++;
        ਜੇ(BOCU1_ਸ਼ੁਰੂ_NEG_2<=c && c<BOCU1_ਸ਼ੁਰੂ_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=prev+(c-BOCU1_ਵਿਚਕਾਰ);
            ਜੇ(c<0x3000) {
                *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(c);
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                prev=BOCU1_SIMPLE_PREV(c);
                ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                ਜਾਓ fastSingle;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            ਜੇ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(c);
            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ ਜੇ(BOCU1_ਸ਼ੁਰੂ_NEG_3<=c && c<BOCU1_ਸ਼ੁਰੂ_POS_3 && ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            /* Optimize two-byte case. */
            ਜੇ(c>=BOCU1_ਵਿਚਕਾਰ) {
                diff = (c - BOCU1_ਸ਼ੁਰੂ_POS_2) * BOCU1_TRAIL_ਗਿਣਤੀ + BOCU1_ਪਹੁੰਚ_POS_1 + 1;
            } ਨਹੀਂਤਾਂ {
                diff = (c - BOCU1_ਸ਼ੁਰੂ_NEG_2) * BOCU1_TRAIL_ਗਿਣਤੀ + BOCU1_ਪਹੁੰਚ_NEG_1;
            }

            /* trail byte */
            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            c=decodeBocu1TrailByte(1, *ਸਰੋਤ++);
            ਜੇ (c < 0 || static_cast<uint32_t>(c = prev + diff + c) > 0x10ffff) {
                ਬਾਈਟਾਂ[0]=ਸਰੋਤ[-2];
                ਬਾਈਟਾਂ[1]=ਸਰੋਤ[-1];
                byte_ਸੂਚਕਾਂਕ=2;
                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            prev=BOCU1_ASCII_PREV;
            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            ਬਾਈਟਾਂ[0] = static_cast<uint8_t>(c);
            byte_ਸੂਚਕਾਂਕ=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            ਲਈ(;;) {
                ਜੇ(ਸਰੋਤ>=ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                    ਜਾਓ endloop;
                }
                ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                c=ਬਾਈਟਾਂ[byte_ਸੂਚਕਾਂਕ++]=*ਸਰੋਤ++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                ਜੇ(c<0) {
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    ਜਾਓ endloop;
                }

                diff+=c;
                ਜੇ(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_ਸੂਚਕਾਂਕ=0;
                    c=prev+diff;
                    ਜੇ (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਜਾਓ endloop;
                    }
                    ਰੋਕੋ;
                }
            }
        }

        /* calculate the next prev and output c */
        prev=BOCU1_PREV(c);
        ਜੇ(c<=0xffff) {
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(c);
            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
        } ਨਹੀਂਤਾਂ {
            /* output surrogate pair */
            *ਨਿਸ਼ਾਨਾ++=U16_LEAD(c);
            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                *ਨਿਸ਼ਾਨਾ++=U16_TRAIL(c);
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            } ਨਹੀਂਤਾਂ {
                /* target overflow */
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                cnv->UChar_Error_ਬਫਰ[0]=U16_TRAIL(c);
                cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }
        }
        ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    }
endloop:

    ਜੇ(*p_Error_ਕੋਡ==U_ILLEGAL_CHAR_ਮਿਲਿਆ) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=BOCU1_ASCII_PREV;
        cnv->ਬਹੁਲਕ=0;
    } ਨਹੀਂਤਾਂ {
        /* set the converter state back into UConverter */
        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = static_cast<uint32_t>(prev);
        cnv->ਬਹੁਲਕ = static_cast<int32_t>(static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->ਮੰਜ਼ਿਲ_ULength=byte_ਸੂਚਕਾਂਕ;

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਸਰੋਤ);
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

/*
 * Identical to _Bocu1ToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
Bocu1_ਮੰਜ਼ਿਲ_Unicode(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    char16_t *ਨਿਸ਼ਾਨਾ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;

    int32_t prev, count, diff, c;

    int8_t byte_ਸੂਚਕਾਂਕ;
    uint8_t *ਬਾਈਟਾਂ;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    ਸਰੋਤ = reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਸਰੋਤ);
    ਸਰੋਤ_ਹੱਦਬੰਦੀ = reinterpret_cast<ਅਚਲ uint8_t*>(p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ);
    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;

    /* get the converter state from UConverter */
    prev = static_cast<int32_t>(cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ);
    ਜੇ(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }
    diff=cnv->ਬਹੁਲਕ; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_ਸੂਚਕਾਂਕ=cnv->ਮੰਜ਼ਿਲ_ULength;
    ਬਾਈਟਾਂ=cnv->ਮੰਜ਼ਿਲ_UBytes;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    ਜੇ(count>0 && byte_ਸੂਚਕਾਂਕ>0 && ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        ਜਾਓ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(ਸਰੋਤ_ਹੱਦਬੰਦੀ - ਸਰੋਤ);
    count = static_cast<int32_t>(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - ਨਿਸ਼ਾਨਾ);
    ਜੇ(count>diff) {
        count=diff;
    }
    ਜਦੋਂ(count>0) {
        ਜੇ(BOCU1_ਸ਼ੁਰੂ_NEG_2<=(c=*ਸਰੋਤ) && c<BOCU1_ਸ਼ੁਰੂ_POS_2) {
            c=prev+(c-BOCU1_ਵਿਚਕਾਰ);
            ਜੇ(c<0x3000) {
                *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(c);
                prev=BOCU1_SIMPLE_PREV(c);
            } ਨਹੀਂਤਾਂ {
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(c<=0x20) {
            ਜੇ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(c);
        } ਨਹੀਂਤਾਂ {
            ਰੋਕੋ;
        }
        ++ਸਰੋਤ;
        --count;
    }

    /* decode a sequence of single and lead bytes */
    ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਜੇ(ਨਿਸ਼ਾਨਾ>=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
            /* target is full */
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }

        c=*ਸਰੋਤ++;
        ਜੇ(BOCU1_ਸ਼ੁਰੂ_NEG_2<=c && c<BOCU1_ਸ਼ੁਰੂ_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=prev+(c-BOCU1_ਵਿਚਕਾਰ);
            ਜੇ(c<0x3000) {
                *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(c);
                prev=BOCU1_SIMPLE_PREV(c);
                ਜਾਓ fastSingle;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            ਜੇ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(c);
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ ਜੇ(BOCU1_ਸ਼ੁਰੂ_NEG_3<=c && c<BOCU1_ਸ਼ੁਰੂ_POS_3 && ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            /* Optimize two-byte case. */
            ਜੇ(c>=BOCU1_ਵਿਚਕਾਰ) {
                diff = (c - BOCU1_ਸ਼ੁਰੂ_POS_2) * BOCU1_TRAIL_ਗਿਣਤੀ + BOCU1_ਪਹੁੰਚ_POS_1 + 1;
            } ਨਹੀਂਤਾਂ {
                diff = (c - BOCU1_ਸ਼ੁਰੂ_NEG_2) * BOCU1_TRAIL_ਗਿਣਤੀ + BOCU1_ਪਹੁੰਚ_NEG_1;
            }

            /* trail byte */
            c=decodeBocu1TrailByte(1, *ਸਰੋਤ++);
            ਜੇ (c < 0 || static_cast<uint32_t>(c = prev + diff + c) > 0x10ffff) {
                ਬਾਈਟਾਂ[0]=ਸਰੋਤ[-2];
                ਬਾਈਟਾਂ[1]=ਸਰੋਤ[-1];
                byte_ਸੂਚਕਾਂਕ=2;
                *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            prev=BOCU1_ASCII_PREV;
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            ਬਾਈਟਾਂ[0] = static_cast<uint8_t>(c);
            byte_ਸੂਚਕਾਂਕ=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            ਲਈ(;;) {
                ਜੇ(ਸਰੋਤ>=ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                    ਜਾਓ endloop;
                }
                c=ਬਾਈਟਾਂ[byte_ਸੂਚਕਾਂਕ++]=*ਸਰੋਤ++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                ਜੇ(c<0) {
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    ਜਾਓ endloop;
                }

                diff+=c;
                ਜੇ(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_ਸੂਚਕਾਂਕ=0;
                    c=prev+diff;
                    ਜੇ (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਜਾਓ endloop;
                    }
                    ਰੋਕੋ;
                }
            }
        }

        /* calculate the next prev and output c */
        prev=BOCU1_PREV(c);
        ਜੇ(c<=0xffff) {
            *ਨਿਸ਼ਾਨਾ++ = static_cast<char16_t>(c);
        } ਨਹੀਂਤਾਂ {
            /* output surrogate pair */
            *ਨਿਸ਼ਾਨਾ++=U16_LEAD(c);
            ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                *ਨਿਸ਼ਾਨਾ++=U16_TRAIL(c);
            } ਨਹੀਂਤਾਂ {
                /* target overflow */
                cnv->UChar_Error_ਬਫਰ[0]=U16_TRAIL(c);
                cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }
        }
    }
endloop:

    ਜੇ(*p_Error_ਕੋਡ==U_ILLEGAL_CHAR_ਮਿਲਿਆ) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=BOCU1_ASCII_PREV;
        cnv->ਬਹੁਲਕ=0;
    } ਨਹੀਂਤਾਂ {
        /* set the converter state back into UConverter */
        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = static_cast<uint32_t>(prev);
        cnv->ਬਹੁਲਕ = (static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->ਮੰਜ਼ਿਲ_ULength=byte_ਸੂਚਕਾਂਕ;

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਸਰੋਤ);
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
}

/* miscellaneous ------------------------------------------------------------ */

ਸਥਿਰ ਅਚਲ UConverterImpl _Bocu1Impl={
    UCNV_BOCU1,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    Bocu1_ਮੰਜ਼ਿਲ_Unicode,
    Bocu1_ਮੰਜ਼ਿਲ_Unicode_With_Offsets,
    _Bocu1FromUnicode,
    _Bocu1FromUnicodeWithOffsets,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_get_Complete_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ Bocu1_Static_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "BOCU-1",
    1214, /* CCSID for BOCU-1 */
    UCNV_IBM, UCNV_BOCU1,
    1, 4, /* one char16_t generates at least 1 byte and at most 4 bytes */
    { 0x1a, 0, 0, 0 }, 1, /* BOCU-1 never needs to write a subchar */
    ਝੂਠ, ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ Bocu1_ਅੰਕੜੇ=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&Bocu1_Static_ਅੰਕੜੇ, &_Bocu1Impl);

#ਜੇ_ਅੰਤ
