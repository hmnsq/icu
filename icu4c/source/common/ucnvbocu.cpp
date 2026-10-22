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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_cb.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

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
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ASCII_PREV        0x40

/* bounding byte values for differences */
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_MIN               0x21
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ಮಧ್ಯ            0x90
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_MAX_LEAD          0xfe
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_MAX_TRAIL         0xff
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_RESET             0xff

/* number of lead bytes */
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ಎಣಿಕೆ             (BOCU1_MAX_LEAD-BOCU1_MIN+1)

/* adjust trail byte counts for the use of some C0 control byte values */
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_TRAIL_CONTROLS_ಎಣಿಕೆ  20
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_TRAIL_BYTE_ಸ್ಥಾನಾಂತರ     (BOCU1_MIN-BOCU1_TRAIL_CONTROLS_ಎಣಿಕೆ)

/* number of trail bytes */
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_TRAIL_ಎಣಿಕೆ       ((BOCU1_MAX_TRAIL-BOCU1_MIN+1)+BOCU1_TRAIL_CONTROLS_ಎಣಿಕೆ)

/*
 * number of positive and negative single-byte codes
 * (counting 0==BOCU1_MIDDLE among the positive ones)
 */
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_SINGLE            64

/* number of lead bytes for positive and negative 2/3/4-byte sequences */
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_LEAD_2            43
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_LEAD_3            3
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_LEAD_4            1

/* The difference value range for single-byters. */
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ವ್ಯಾಪ್ತಿ_POS_1   (BOCU1_SINGLE-1)
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ವ್ಯಾಪ್ತಿ_NEG_1   (-BOCU1_SINGLE)

/* The difference value range for double-byters. */
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ವ್ಯಾಪ್ತಿ_POS_2   (BOCU1_ವ್ಯಾಪ್ತಿ_POS_1+BOCU1_LEAD_2*BOCU1_TRAIL_ಎಣಿಕೆ)
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ವ್ಯಾಪ್ತಿ_NEG_2   (BOCU1_ವ್ಯಾಪ್ತಿ_NEG_1-BOCU1_LEAD_2*BOCU1_TRAIL_ಎಣಿಕೆ)

/* The difference value range for 3-byters. */
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ವ್ಯಾಪ್ತಿ_POS_3   \
    (BOCU1_ವ್ಯಾಪ್ತಿ_POS_2+BOCU1_LEAD_3*BOCU1_TRAIL_ಎಣಿಕೆ*BOCU1_TRAIL_ಎಣಿಕೆ)

#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ವ್ಯಾಪ್ತಿ_NEG_3   (BOCU1_ವ್ಯಾಪ್ತಿ_NEG_2-BOCU1_LEAD_3*BOCU1_TRAIL_ಎಣಿಕೆ*BOCU1_TRAIL_ಎಣಿಕೆ)

/* The lead byte start values. */
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ಆರಂಭ_POS_2   (BOCU1_ಮಧ್ಯ+BOCU1_ವ್ಯಾಪ್ತಿ_POS_1+1)
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ಆರಂಭ_POS_3   (BOCU1_ಆರಂಭ_POS_2+BOCU1_LEAD_2)
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ಆರಂಭ_POS_4   (BOCU1_ಆರಂಭ_POS_3+BOCU1_LEAD_3)
     /* ==BOCU1_MAX_LEAD */

#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ಆರಂಭ_NEG_2   (BOCU1_ಮಧ್ಯ+BOCU1_ವ್ಯಾಪ್ತಿ_NEG_1)
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ಆರಂಭ_NEG_3   (BOCU1_ಆರಂಭ_NEG_2-BOCU1_LEAD_2)
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ಆರಂಭ_NEG_4   (BOCU1_ಆರಂಭ_NEG_3-BOCU1_LEAD_3)
     /* ==BOCU1_MIN+1 */

/* The length of a byte sequence, according to the lead byte (!=BOCU1_RESET). */
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ಉದ್ದಳತೆ_FROM_LEAD(lead) \
    ((BOCU1_ಆರಂಭ_NEG_2<=(lead) && (lead)<BOCU1_ಆರಂಭ_POS_2) ? 1 : \
     (BOCU1_ಆರಂಭ_NEG_3<=(lead) && (lead)<BOCU1_ಆರಂಭ_POS_3) ? 2 : \
     (BOCU1_ಆರಂಭ_NEG_4<=(lead) && (lead)<BOCU1_ಆರಂಭ_POS_4) ? 3 : 4)

/* The length of a byte sequence, according to its packed form. */
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_ಉದ್ದಳತೆ_FROM_PACKED(packed) \
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
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_TRAIL_ಗಮ್ಯ_BYTE(t) ((t)>=BOCU1_TRAIL_CONTROLS_ಎಣಿಕೆ ? (t)+BOCU1_TRAIL_BYTE_ಸ್ಥಾನಾಂತರ : bocu1_Trail_ಗಮ್ಯ_Byte[t])

/*
 * Byte value map for control codes,
 * from external byte values 0x00..0x20
 * to trail byte values 0..19 (0..0x13) as used in the difference calculation.
 * External byte values that are illegal as trail bytes are mapped to -1.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int8_t
bocu1_Byte_ಗಮ್ಯ_Trail[BOCU1_MIN]={
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
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int8_t
bocu1_Trail_ಗಮ್ಯ_Byte[BOCU1_TRAIL_CONTROLS_ಎಣಿಕೆ]={
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
#ವ್ಯಾಖ್ಯಾನಿಸು NEGDIVMOD(n, d, m) UPRV_ಖಂಡ_MACRO_BEGIN { \
    (m)=(n)%(d); \
    (n)/=(d); \
    ಆದರೆ((m)<0) { \
        --(n); \
        (m)+=(d); \
    } \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

/* Faster versions of packDiff() for single-byte-encoded diff values. */

/** Is a diff value encodable in a single byte? */
#ವ್ಯಾಖ್ಯಾನಿಸು DIFF_IS_SINGLE(diff) (BOCU1_ವ್ಯಾಪ್ತಿ_NEG_1<=(diff) && (diff)<=BOCU1_ವ್ಯಾಪ್ತಿ_POS_1)

/** Encode a diff value in a single byte. */
#ವ್ಯಾಖ್ಯಾನಿಸು PACK_SINGLE_DIFF(diff) (BOCU1_ಮಧ್ಯ+(diff))

/** Is a diff value encodable in two bytes? */
#ವ್ಯಾಖ್ಯಾನಿಸು DIFF_IS_DOUBLE(diff) (BOCU1_ವ್ಯಾಪ್ತಿ_NEG_2<=(diff) && (diff)<=BOCU1_ವ್ಯಾಪ್ತಿ_POS_2)

/* BOCU-1 implementation functions ------------------------------------------ */

#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_SIMPLE_PREV(c) (((c)&~0x7f)+BOCU1_ASCII_PREV)

/**
 * Compute the next "previous" value for differencing
 * from the current code point.
 *
 * @param c current code point, 0x3040..0xd7a3 (rest handled by macro below)
 * @return "previous code point" state value
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
bocu1Prev(int32_t c) {
    /* compute new prev */
    ಆದರೆ(/* 0x3040<=c && */ c<=0x309f) {
        /* Hiragana is not 128-aligned */
        ಹಿಂತಿರುಗಿ 0x3070;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(0x4e00<=c && c<=0x9fa5) {
        /* CJK Unihan */
        ಹಿಂತಿರುಗಿ 0x4e00-BOCU1_ವ್ಯಾಪ್ತಿ_NEG_2;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(0xac00<=c /* && c<=0xd7a3 */) {
        /* Korean Hangul */
        ಹಿಂತಿರುಗಿ (0xd7a3+0xac00)/2;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* mostly small scripts */
        ಹಿಂತಿರುಗಿ BOCU1_SIMPLE_PREV(c);
    }
}

/** Fast version of bocu1Prev() for most scripts. */
#ವ್ಯಾಖ್ಯಾನಿಸು BOCU1_PREV(c) ((c)<0x3040 || (c)>0xd7a3 ? BOCU1_SIMPLE_PREV(c) : bocu1Prev(c))

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
ಸ್ಥಿರ int32_t
packDiff(int32_t diff) {
    int32_t ಫಲಿತಾಂಶ, m;

    U_ASSERT(!DIFF_IS_SINGLE(diff)); /* assume we won't be called where diff==BOCU1_REACH_NEG_1=-64 */
    ಆದರೆ(diff>=BOCU1_ವ್ಯಾಪ್ತಿ_NEG_1) {
        /* mostly positive differences, and single-byte negative ones */
#ಆದರೆ 0   /* single-byte case handled in macros, see below */
        ಆದರೆ(diff<=BOCU1_ವ್ಯಾಪ್ತಿ_POS_1) {
            /* single byte */
            ಹಿಂತಿರುಗಿ 0x01000000|(BOCU1_ಮಧ್ಯ+diff);
        } ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ(diff<=BOCU1_ವ್ಯಾಪ್ತಿ_POS_2) {
            /* two bytes */
            diff-=BOCU1_ವ್ಯಾಪ್ತಿ_POS_1+1;
            ಫಲಿತಾಂಶ=0x02000000;

            m=diff%BOCU1_TRAIL_ಎಣಿಕೆ;
            diff/=BOCU1_TRAIL_ಎಣಿಕೆ;
            ಫಲಿತಾಂಶ|=BOCU1_TRAIL_ಗಮ್ಯ_BYTE(m);

            ಫಲಿತಾಂಶ|=(BOCU1_ಆರಂಭ_POS_2+diff)<<8;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(diff<=BOCU1_ವ್ಯಾಪ್ತಿ_POS_3) {
            /* three bytes */
            diff-=BOCU1_ವ್ಯಾಪ್ತಿ_POS_2+1;
            ಫಲಿತಾಂಶ=0x03000000;

            m=diff%BOCU1_TRAIL_ಎಣಿಕೆ;
            diff/=BOCU1_TRAIL_ಎಣಿಕೆ;
            ಫಲಿತಾಂಶ|=BOCU1_TRAIL_ಗಮ್ಯ_BYTE(m);

            m=diff%BOCU1_TRAIL_ಎಣಿಕೆ;
            diff/=BOCU1_TRAIL_ಎಣಿಕೆ;
            ಫಲಿತಾಂಶ|=BOCU1_TRAIL_ಗಮ್ಯ_BYTE(m)<<8;

            ಫಲಿತಾಂಶ|=(BOCU1_ಆರಂಭ_POS_3+diff)<<16;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* four bytes */
            diff-=BOCU1_ವ್ಯಾಪ್ತಿ_POS_3+1;

            m=diff%BOCU1_TRAIL_ಎಣಿಕೆ;
            diff/=BOCU1_TRAIL_ಎಣಿಕೆ;
            ಫಲಿತಾಂಶ=BOCU1_TRAIL_ಗಮ್ಯ_BYTE(m);

            m=diff%BOCU1_TRAIL_ಎಣಿಕೆ;
            diff/=BOCU1_TRAIL_ಎಣಿಕೆ;
            ಫಲಿತಾಂಶ|=BOCU1_TRAIL_ಗಮ್ಯ_BYTE(m)<<8;

            /*
             * We know that / and % would deliver quotient 0 and rest=diff.
             * Avoid division and modulo for performance.
             */
            ಫಲಿತಾಂಶ|=BOCU1_TRAIL_ಗಮ್ಯ_BYTE(diff)<<16;

            ಫಲಿತಾಂಶ |= static_cast<uint32_t>(BOCU1_ಆರಂಭ_POS_4) << 24;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* two- to four-byte negative differences */
        ಆದರೆ(diff>=BOCU1_ವ್ಯಾಪ್ತಿ_NEG_2) {
            /* two bytes */
            diff-=BOCU1_ವ್ಯಾಪ್ತಿ_NEG_1;
            ಫಲಿತಾಂಶ=0x02000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_ಎಣಿಕೆ, m);
            ಫಲಿತಾಂಶ|=BOCU1_TRAIL_ಗಮ್ಯ_BYTE(m);

            ಫಲಿತಾಂಶ|=(BOCU1_ಆರಂಭ_NEG_2+diff)<<8;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(diff>=BOCU1_ವ್ಯಾಪ್ತಿ_NEG_3) {
            /* three bytes */
            diff-=BOCU1_ವ್ಯಾಪ್ತಿ_NEG_2;
            ಫಲಿತಾಂಶ=0x03000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_ಎಣಿಕೆ, m);
            ಫಲಿತಾಂಶ|=BOCU1_TRAIL_ಗಮ್ಯ_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_ಎಣಿಕೆ, m);
            ಫಲಿತಾಂಶ|=BOCU1_TRAIL_ಗಮ್ಯ_BYTE(m)<<8;

            ಫಲಿತಾಂಶ|=(BOCU1_ಆರಂಭ_NEG_3+diff)<<16;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* four bytes */
            diff-=BOCU1_ವ್ಯಾಪ್ತಿ_NEG_3;

            NEGDIVMOD(diff, BOCU1_TRAIL_ಎಣಿಕೆ, m);
            ಫಲಿತಾಂಶ=BOCU1_TRAIL_ಗಮ್ಯ_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_ಎಣಿಕೆ, m);
            ಫಲಿತಾಂಶ|=BOCU1_TRAIL_ಗಮ್ಯ_BYTE(m)<<8;

            /*
             * We know that NEGDIVMOD would deliver
             * quotient -1 and rest=diff+BOCU1_TRAIL_COUNT.
             * Avoid division and modulo for performance.
             */
            m=diff+BOCU1_TRAIL_ಎಣಿಕೆ;
            ಫಲಿತಾಂಶ|=BOCU1_TRAIL_ಗಮ್ಯ_BYTE(m)<<16;

            ಫಲಿತಾಂಶ|=BOCU1_MIN<<24;
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}


ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_Bocu1FromUnicodeWithOffsets(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    int32_t *offsets;

    int32_t prev, c, diff;

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ, ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ = reinterpret_cast<uint8_t*>(p_ವಾದಗಳು->ಗುರಿ);
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - p_ವಾದಗಳು->ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    prev = static_cast<int32_t>(cnv->from_Unicode_ಸ್ಥಿತಿ);
    ಆದರೆ(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ= c==0 ? 0 : -1;
    ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    /* conversion loop */
    ಆದರೆ(c!=0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
        ಹೋಗು getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(ಮೂಲ_ಎಲ್ಲೆ - ಮೂಲ);
    ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>diff) {
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=diff;
    }
    ಆಗ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0 && (c=*ಮೂಲ)<0x3000) {
        ಆದರೆ(c<=0x20) {
            ಆದರೆ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *ಗುರಿ++ = static_cast<uint8_t>(c);
            *offsets++=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ++;
            ++ಮೂಲ;
            --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            diff=c-prev;
            ಆದರೆ(DIFF_IS_SINGLE(diff)) {
                prev=BOCU1_SIMPLE_PREV(c);
                *ಗುರಿ++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                ++ಮೂಲ;
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ನಿಲ್ಲಿಸು;
            }
        }
    }
    /* restore real values */
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ) - ಗುರಿ);
    ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ; /* wrong if offsets==nullptr but does not matter */

    /* regular loop for all cases */
    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
            c=*ಮೂಲ++;
            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;

            ಆದರೆ(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                ಆದರೆ(c!=0x20) {
                    prev=BOCU1_ASCII_PREV;
                }
                *ಗುರಿ++ = static_cast<uint8_t>(c);
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;

                ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                ಮುಂದುವರಿ;
            }

            ಆದರೆ(U16_IS_LEAD(c)) {
getTrail:
                ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
                    /* test the following code unit */
                    char16_t trail=*ಮೂಲ;
                    ಆದರೆ(U16_IS_TRAIL(trail)) {
                        ++ಮೂಲ;
                        ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    ನಿಲ್ಲಿಸು;
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
            ಆದರೆ(DIFF_IS_SINGLE(diff)) {
                *ಗುರಿ++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                ಆದರೆ(c<0x3000) {
                    ಹೋಗು fastSingle;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(DIFF_IS_DOUBLE(diff) && 2<=ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
                /* optimize 2-byte case */
                int32_t m;

                ಆದರೆ(diff>=0) {
                    diff-=BOCU1_ವ್ಯಾಪ್ತಿ_POS_1+1;
                    m=diff%BOCU1_TRAIL_ಎಣಿಕೆ;
                    diff/=BOCU1_TRAIL_ಎಣಿಕೆ;
                    diff+=BOCU1_ಆರಂಭ_POS_2;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    diff-=BOCU1_ವ್ಯಾಪ್ತಿ_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_ಎಣಿಕೆ, m);
                    diff+=BOCU1_ಆರಂಭ_NEG_2;
                }
                *ಗುರಿ++ = static_cast<uint8_t>(diff);
                *ಗುರಿ++ = static_cast<uint8_t>(BOCU1_TRAIL_ಗಮ್ಯ_BYTE(m));
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=2;
                ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_ಉದ್ದಳತೆ_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                ಆದರೆ(length<=ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
                    ಆಯ್ಕೆ(length) {
                        /* each branch falls through to the next one */
                    ಸ್ಥಿತಿ 4:
                        *ಗುರಿ++ = static_cast<uint8_t>(diff >> 24);
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 3:
                        *ಗುರಿ++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 2:
                        *ಗುರಿ++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    /* case 1: handled above */
                        *ಗುರಿ++ = static_cast<uint8_t>(diff);
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        U_FALLTHROUGH;
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        /* will never occur */
                        ನಿಲ್ಲಿಸು;
                    }
                    ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=length;
                    ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    uint8_t *char_Error_ಬಫರ್;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                    char_Error_ಬಫರ್=(uint8_t *)cnv->char_Error_ಬಫರ್;
                    ಆಯ್ಕೆ(length) {
                        /* each branch falls through to the next one */
                    ಸ್ಥಿತಿ 3:
                        *char_Error_ಬಫರ್++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 2:
                        *char_Error_ಬಫರ್++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 1:
                        *char_Error_ಬಫರ್ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        /* will never occur */
                        ನಿಲ್ಲಿಸು;
                    }
                    cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    ಆಯ್ಕೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
                        /* each branch falls through to the next one */
                    ಸ್ಥಿತಿ 3:
                        *ಗುರಿ++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 2:
                        *ಗುರಿ++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 1:
                        *ಗುರಿ++ = static_cast<uint8_t>(diff);
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        U_FALLTHROUGH;
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        /* will never occur */
                        ನಿಲ್ಲಿಸು;
                    }

                    /* target overflow */
                    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=0;
                    *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    ನಿಲ್ಲಿಸು;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_ಸ್ಥಿತಿ = static_cast<uint32_t>(prev);

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
    p_ವಾದಗಳು->offsets=offsets;
}

/*
 * Identical to _Bocu1FromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_Bocu1FromUnicode(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ;

    int32_t prev, c, diff;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ = reinterpret_cast<uint8_t*>(p_ವಾದಗಳು->ಗುರಿ);
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - p_ವಾದಗಳು->ಗುರಿ);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    prev = static_cast<int32_t>(cnv->from_Unicode_ಸ್ಥಿತಿ);
    ಆದರೆ(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }

    /* conversion loop */
    ಆದರೆ(c!=0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
        ಹೋಗು getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(ಮೂಲ_ಎಲ್ಲೆ - ಮೂಲ);
    ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>diff) {
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=diff;
    }
    ಆಗ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0 && (c=*ಮೂಲ)<0x3000) {
        ಆದರೆ(c<=0x20) {
            ಆದರೆ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *ಗುರಿ++ = static_cast<uint8_t>(c);
        } ಇಲ್ಲದಿದ್ದರೆ {
            diff=c-prev;
            ಆದರೆ(DIFF_IS_SINGLE(diff)) {
                prev=BOCU1_SIMPLE_PREV(c);
                *ಗುರಿ++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
            } ಇಲ್ಲದಿದ್ದರೆ {
                ನಿಲ್ಲಿಸು;
            }
        }
        ++ಮೂಲ;
        --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    }
    /* restore real values */
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ) - ಗುರಿ);

    /* regular loop for all cases */
    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
            c=*ಮೂಲ++;

            ಆದರೆ(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                ಆದರೆ(c!=0x20) {
                    prev=BOCU1_ASCII_PREV;
                }
                *ಗುರಿ++ = static_cast<uint8_t>(c);
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                ಮುಂದುವರಿ;
            }

            ಆದರೆ(U16_IS_LEAD(c)) {
getTrail:
                ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
                    /* test the following code unit */
                    char16_t trail=*ಮೂಲ;
                    ಆದರೆ(U16_IS_TRAIL(trail)) {
                        ++ಮೂಲ;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    ನಿಲ್ಲಿಸು;
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
            ಆದರೆ(DIFF_IS_SINGLE(diff)) {
                *ಗುರಿ++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                ಆದರೆ(c<0x3000) {
                    ಹೋಗು fastSingle;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(DIFF_IS_DOUBLE(diff) && 2<=ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
                /* optimize 2-byte case */
                int32_t m;

                ಆದರೆ(diff>=0) {
                    diff-=BOCU1_ವ್ಯಾಪ್ತಿ_POS_1+1;
                    m=diff%BOCU1_TRAIL_ಎಣಿಕೆ;
                    diff/=BOCU1_TRAIL_ಎಣಿಕೆ;
                    diff+=BOCU1_ಆರಂಭ_POS_2;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    diff-=BOCU1_ವ್ಯಾಪ್ತಿ_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_ಎಣಿಕೆ, m);
                    diff+=BOCU1_ಆರಂಭ_NEG_2;
                }
                *ಗುರಿ++ = static_cast<uint8_t>(diff);
                *ಗುರಿ++ = static_cast<uint8_t>(BOCU1_TRAIL_ಗಮ್ಯ_BYTE(m));
                ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=2;
            } ಇಲ್ಲದಿದ್ದರೆ {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_ಉದ್ದಳತೆ_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                ಆದರೆ(length<=ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
                    ಆಯ್ಕೆ(length) {
                        /* each branch falls through to the next one */
                    ಸ್ಥಿತಿ 4:
                        *ಗುರಿ++ = static_cast<uint8_t>(diff >> 24);
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 3:
                        *ಗುರಿ++ = static_cast<uint8_t>(diff >> 16);
                    /* case 2: handled above */
                        *ಗುರಿ++ = static_cast<uint8_t>(diff >> 8);
                    /* case 1: handled above */
                        *ಗುರಿ++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        /* will never occur */
                        ನಿಲ್ಲಿಸು;
                    }
                    ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=length;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    uint8_t *char_Error_ಬಫರ್;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                    char_Error_ಬಫರ್=(uint8_t *)cnv->char_Error_ಬಫರ್;
                    ಆಯ್ಕೆ(length) {
                        /* each branch falls through to the next one */
                    ಸ್ಥಿತಿ 3:
                        *char_Error_ಬಫರ್++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 2:
                        *char_Error_ಬಫರ್++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 1:
                        *char_Error_ಬಫರ್ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        /* will never occur */
                        ನಿಲ್ಲಿಸು;
                    }
                    cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    ಆಯ್ಕೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
                        /* each branch falls through to the next one */
                    ಸ್ಥಿತಿ 3:
                        *ಗುರಿ++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 2:
                        *ಗುರಿ++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    ಸ್ಥಿತಿ 1:
                        *ಗುರಿ++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    ಪೂರ್ವನಿಯೋಜಿತ:
                        /* will never occur */
                        ನಿಲ್ಲಿಸು;
                    }

                    /* target overflow */
                    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=0;
                    *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    ನಿಲ್ಲಿಸು;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_ಸ್ಥಿತಿ = static_cast<uint32_t>(prev);

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
}

/* BOCU-1-to-Unicode conversion functions ----------------------------------- */

/**
 * Function for BOCU-1 decoder; handles multi-byte lead bytes.
 *
 * @param b lead byte;
 *          BOCU1_MIN<=b<BOCU1_START_NEG_2 or BOCU1_START_POS_2<=b<BOCU1_MAX_LEAD
 * @return (diff<<2)|count
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
decodeBocu1LeadByte(int32_t b) {
    int32_t diff, count;

    ಆದರೆ(b>=BOCU1_ಆರಂಭ_NEG_2) {
        /* positive difference */
        ಆದರೆ(b<BOCU1_ಆರಂಭ_POS_3) {
            /* two bytes */
            diff = (b - BOCU1_ಆರಂಭ_POS_2) * BOCU1_TRAIL_ಎಣಿಕೆ + BOCU1_ವ್ಯಾಪ್ತಿ_POS_1 + 1;
            count=1;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b<BOCU1_ಆರಂಭ_POS_4) {
            /* three bytes */
            diff = (b - BOCU1_ಆರಂಭ_POS_3) * BOCU1_TRAIL_ಎಣಿಕೆ * BOCU1_TRAIL_ಎಣಿಕೆ + BOCU1_ವ್ಯಾಪ್ತಿ_POS_2 + 1;
            count=2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* four bytes */
            diff=BOCU1_ವ್ಯಾಪ್ತಿ_POS_3+1;
            count=3;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* negative difference */
        ಆದರೆ(b>=BOCU1_ಆರಂಭ_NEG_3) {
            /* two bytes */
            diff = (b - BOCU1_ಆರಂಭ_NEG_2) * BOCU1_TRAIL_ಎಣಿಕೆ + BOCU1_ವ್ಯಾಪ್ತಿ_NEG_1;
            count=1;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b>BOCU1_MIN) {
            /* three bytes */
            diff = (b - BOCU1_ಆರಂಭ_NEG_3) * BOCU1_TRAIL_ಎಣಿಕೆ * BOCU1_TRAIL_ಎಣಿಕೆ + BOCU1_ವ್ಯಾಪ್ತಿ_NEG_2;
            count=2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* four bytes */
            diff=-BOCU1_TRAIL_ಎಣಿಕೆ*BOCU1_TRAIL_ಎಣಿಕೆ*BOCU1_TRAIL_ಎಣಿಕೆ+BOCU1_ವ್ಯಾಪ್ತಿ_NEG_3;
            count=3;
        }
    }

    /* return the state for decoding the trail byte(s) */
    ಹಿಂತಿರುಗಿ (static_cast<uint32_t>(diff) << 2) | count;
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
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
decodeBocu1TrailByte(int32_t count, int32_t b) {
    ಆದರೆ(b<=0x20) {
        /* skip some C0 controls and make the trail byte range contiguous */
        b=bocu1_Byte_ಗಮ್ಯ_Trail[b];
        /* b<0 for an illegal trail byte value will result in return<0 below */
#ಆದರೆ BOCU1_MAX_TRAIL<0xff
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b>BOCU1_MAX_TRAIL) {
        ಹಿಂತಿರುಗಿ -99;
#ಆದರೆ_ಅಂತ್ಯ
    } ಇಲ್ಲದಿದ್ದರೆ {
        b-=BOCU1_TRAIL_BYTE_ಸ್ಥಾನಾಂತರ;
    }

    /* add trail byte into difference and decrement count */
    ಆದರೆ(count==1) {
        ಹಿಂತಿರುಗಿ b;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(count==2) {
        ಹಿಂತಿರುಗಿ b*BOCU1_TRAIL_ಎಣಿಕೆ;
    } ಇಲ್ಲದಿದ್ದರೆ /* count==3 */ {
        ಹಿಂತಿರುಗಿ b*(BOCU1_TRAIL_ಎಣಿಕೆ*BOCU1_TRAIL_ಎಣಿಕೆ);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
Bocu1_ಗಮ್ಯ_Unicode_With_Offsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    char16_t *ಗುರಿ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ;
    int32_t *offsets;

    int32_t prev, count, diff, c;

    int8_t byte_ಸೂಚ್ಯಂಕ;
    uint8_t *ಬೈಟುಗಳು;

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ, ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಮೂಲ);
    ಮೂಲ_ಎಲ್ಲೆ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ);
    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ;
    offsets=p_ವಾದಗಳು->offsets;

    /* get the converter state from UConverter */
    prev = static_cast<int32_t>(cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ);
    ಆದರೆ(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }
    diff=cnv->ಬಹುಲಕ; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_ಸೂಚ್ಯಂಕ=cnv->ಗಮ್ಯ_ULength;
    ಬೈಟುಗಳು=cnv->ಗಮ್ಯ_UBytes;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ=byte_ಸೂಚ್ಯಂಕ==0 ? 0 : -1;
    ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    ಆದರೆ(count>0 && byte_ಸೂಚ್ಯಂಕ>0 && ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
        ಹೋಗು getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(ಮೂಲ_ಎಲ್ಲೆ - ಮೂಲ);
    count = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - ಗುರಿ);
    ಆದರೆ(count>diff) {
        count=diff;
    }
    ಆಗ(count>0) {
        ಆದರೆ(BOCU1_ಆರಂಭ_NEG_2<=(c=*ಮೂಲ) && c<BOCU1_ಆರಂಭ_POS_2) {
            c=prev+(c-BOCU1_ಮಧ್ಯ);
            ಆದರೆ(c<0x3000) {
                *ಗುರಿ++ = static_cast<char16_t>(c);
                *offsets++=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ++;
                prev=BOCU1_SIMPLE_PREV(c);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0x20) {
            ಆದರೆ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *ಗುರಿ++ = static_cast<char16_t>(c);
            *offsets++=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ನಿಲ್ಲಿಸು;
        }
        ++ಮೂಲ;
        --count;
    }
    ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ; /* wrong if offsets==nullptr but does not matter */

    /* decode a sequence of single and lead bytes */
    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        ಆದರೆ(ಗುರಿ>=ಗುರಿ_ಎಲ್ಲೆ) {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }

        ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
        c=*ಮೂಲ++;
        ಆದರೆ(BOCU1_ಆರಂಭ_NEG_2<=c && c<BOCU1_ಆರಂಭ_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=prev+(c-BOCU1_ಮಧ್ಯ);
            ಆದರೆ(c<0x3000) {
                *ಗುರಿ++ = static_cast<char16_t>(c);
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                prev=BOCU1_SIMPLE_PREV(c);
                ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                ಹೋಗು fastSingle;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            ಆದರೆ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *ಗುರಿ++ = static_cast<char16_t>(c);
            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(BOCU1_ಆರಂಭ_NEG_3<=c && c<BOCU1_ಆರಂಭ_POS_3 && ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            /* Optimize two-byte case. */
            ಆದರೆ(c>=BOCU1_ಮಧ್ಯ) {
                diff = (c - BOCU1_ಆರಂಭ_POS_2) * BOCU1_TRAIL_ಎಣಿಕೆ + BOCU1_ವ್ಯಾಪ್ತಿ_POS_1 + 1;
            } ಇಲ್ಲದಿದ್ದರೆ {
                diff = (c - BOCU1_ಆರಂಭ_NEG_2) * BOCU1_TRAIL_ಎಣಿಕೆ + BOCU1_ವ್ಯಾಪ್ತಿ_NEG_1;
            }

            /* trail byte */
            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            c=decodeBocu1TrailByte(1, *ಮೂಲ++);
            ಆದರೆ (c < 0 || static_cast<uint32_t>(c = prev + diff + c) > 0x10ffff) {
                ಬೈಟುಗಳು[0]=ಮೂಲ[-2];
                ಬೈಟುಗಳು[1]=ಮೂಲ[-1];
                byte_ಸೂಚ್ಯಂಕ=2;
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            prev=BOCU1_ASCII_PREV;
            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            ಬೈಟುಗಳು[0] = static_cast<uint8_t>(c);
            byte_ಸೂಚ್ಯಂಕ=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            ಗಾಗಿ(;;) {
                ಆದರೆ(ಮೂಲ>=ಮೂಲ_ಎಲ್ಲೆ) {
                    ಹೋಗು endloop;
                }
                ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                c=ಬೈಟುಗಳು[byte_ಸೂಚ್ಯಂಕ++]=*ಮೂಲ++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                ಆದರೆ(c<0) {
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    ಹೋಗು endloop;
                }

                diff+=c;
                ಆದರೆ(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_ಸೂಚ್ಯಂಕ=0;
                    c=prev+diff;
                    ಆದರೆ (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ಹೋಗು endloop;
                    }
                    ನಿಲ್ಲಿಸು;
                }
            }
        }

        /* calculate the next prev and output c */
        prev=BOCU1_PREV(c);
        ಆದರೆ(c<=0xffff) {
            *ಗುರಿ++ = static_cast<char16_t>(c);
            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* output surrogate pair */
            *ಗುರಿ++=U16_LEAD(c);
            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                *ಗುರಿ++=U16_TRAIL(c);
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* target overflow */
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                cnv->UChar_Error_ಬಫರ್[0]=U16_TRAIL(c);
                cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
        }
        ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
    }
endloop:

    ಆದರೆ(*p_Error_ಸಂಕೇತ==U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=BOCU1_ASCII_PREV;
        cnv->ಬಹುಲಕ=0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* set the converter state back into UConverter */
        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = static_cast<uint32_t>(prev);
        cnv->ಬಹುಲಕ = static_cast<int32_t>(static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->ಗಮ್ಯ_ULength=byte_ಸೂಚ್ಯಂಕ;

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಮೂಲ);
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

/*
 * Identical to _Bocu1ToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
Bocu1_ಗಮ್ಯ_Unicode(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    char16_t *ಗುರಿ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ;

    int32_t prev, count, diff, c;

    int8_t byte_ಸೂಚ್ಯಂಕ;
    uint8_t *ಬೈಟುಗಳು;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಮೂಲ);
    ಮೂಲ_ಎಲ್ಲೆ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ);
    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ;

    /* get the converter state from UConverter */
    prev = static_cast<int32_t>(cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ);
    ಆದರೆ(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }
    diff=cnv->ಬಹುಲಕ; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_ಸೂಚ್ಯಂಕ=cnv->ಗಮ್ಯ_ULength;
    ಬೈಟುಗಳು=cnv->ಗಮ್ಯ_UBytes;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    ಆದರೆ(count>0 && byte_ಸೂಚ್ಯಂಕ>0 && ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
        ಹೋಗು getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(ಮೂಲ_ಎಲ್ಲೆ - ಮೂಲ);
    count = static_cast<int32_t>(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ - ಗುರಿ);
    ಆದರೆ(count>diff) {
        count=diff;
    }
    ಆಗ(count>0) {
        ಆದರೆ(BOCU1_ಆರಂಭ_NEG_2<=(c=*ಮೂಲ) && c<BOCU1_ಆರಂಭ_POS_2) {
            c=prev+(c-BOCU1_ಮಧ್ಯ);
            ಆದರೆ(c<0x3000) {
                *ಗುರಿ++ = static_cast<char16_t>(c);
                prev=BOCU1_SIMPLE_PREV(c);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0x20) {
            ಆದರೆ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *ಗುರಿ++ = static_cast<char16_t>(c);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ನಿಲ್ಲಿಸು;
        }
        ++ಮೂಲ;
        --count;
    }

    /* decode a sequence of single and lead bytes */
    ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
        ಆದರೆ(ಗುರಿ>=ಗುರಿ_ಎಲ್ಲೆ) {
            /* target is full */
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }

        c=*ಮೂಲ++;
        ಆದರೆ(BOCU1_ಆರಂಭ_NEG_2<=c && c<BOCU1_ಆರಂಭ_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=prev+(c-BOCU1_ಮಧ್ಯ);
            ಆದರೆ(c<0x3000) {
                *ಗುರಿ++ = static_cast<char16_t>(c);
                prev=BOCU1_SIMPLE_PREV(c);
                ಹೋಗು fastSingle;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            ಆದರೆ(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *ಗುರಿ++ = static_cast<char16_t>(c);
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(BOCU1_ಆರಂಭ_NEG_3<=c && c<BOCU1_ಆರಂಭ_POS_3 && ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            /* Optimize two-byte case. */
            ಆದರೆ(c>=BOCU1_ಮಧ್ಯ) {
                diff = (c - BOCU1_ಆರಂಭ_POS_2) * BOCU1_TRAIL_ಎಣಿಕೆ + BOCU1_ವ್ಯಾಪ್ತಿ_POS_1 + 1;
            } ಇಲ್ಲದಿದ್ದರೆ {
                diff = (c - BOCU1_ಆರಂಭ_NEG_2) * BOCU1_TRAIL_ಎಣಿಕೆ + BOCU1_ವ್ಯಾಪ್ತಿ_NEG_1;
            }

            /* trail byte */
            c=decodeBocu1TrailByte(1, *ಮೂಲ++);
            ಆದರೆ (c < 0 || static_cast<uint32_t>(c = prev + diff + c) > 0x10ffff) {
                ಬೈಟುಗಳು[0]=ಮೂಲ[-2];
                ಬೈಟುಗಳು[1]=ಮೂಲ[-1];
                byte_ಸೂಚ್ಯಂಕ=2;
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            prev=BOCU1_ASCII_PREV;
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            ಬೈಟುಗಳು[0] = static_cast<uint8_t>(c);
            byte_ಸೂಚ್ಯಂಕ=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            ಗಾಗಿ(;;) {
                ಆದರೆ(ಮೂಲ>=ಮೂಲ_ಎಲ್ಲೆ) {
                    ಹೋಗು endloop;
                }
                c=ಬೈಟುಗಳು[byte_ಸೂಚ್ಯಂಕ++]=*ಮೂಲ++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                ಆದರೆ(c<0) {
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    ಹೋಗು endloop;
                }

                diff+=c;
                ಆದರೆ(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_ಸೂಚ್ಯಂಕ=0;
                    c=prev+diff;
                    ಆದರೆ (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ಹೋಗು endloop;
                    }
                    ನಿಲ್ಲಿಸು;
                }
            }
        }

        /* calculate the next prev and output c */
        prev=BOCU1_PREV(c);
        ಆದರೆ(c<=0xffff) {
            *ಗುರಿ++ = static_cast<char16_t>(c);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* output surrogate pair */
            *ಗುರಿ++=U16_LEAD(c);
            ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                *ಗುರಿ++=U16_TRAIL(c);
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* target overflow */
                cnv->UChar_Error_ಬಫರ್[0]=U16_TRAIL(c);
                cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
        }
    }
endloop:

    ಆದರೆ(*p_Error_ಸಂಕೇತ==U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=BOCU1_ASCII_PREV;
        cnv->ಬಹುಲಕ=0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* set the converter state back into UConverter */
        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = static_cast<uint32_t>(prev);
        cnv->ಬಹುಲಕ = (static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->ಗಮ್ಯ_ULength=byte_ಸೂಚ್ಯಂಕ;

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಮೂಲ);
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
}

/* miscellaneous ------------------------------------------------------------ */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _Bocu1Impl={
    UCNV_BOCU1,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    Bocu1_ಗಮ್ಯ_Unicode,
    Bocu1_ಗಮ್ಯ_Unicode_With_Offsets,
    _Bocu1FromUnicode,
    _Bocu1FromUnicodeWithOffsets,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_get_Complete_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ Bocu1_Static_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "BOCU-1",
    1214, /* CCSID for BOCU-1 */
    UCNV_IBM, UCNV_BOCU1,
    1, 4, /* one char16_t generates at least 1 byte and at most 4 bytes */
    { 0x1a, 0, 0, 0 }, 1, /* BOCU-1 never needs to write a subchar */
    ಸುಳ್ಳು, ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ Bocu1_ದತ್ತಾಂಶ=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&Bocu1_Static_ದತ್ತಾಂಶ, &_Bocu1Impl);

#ಆದರೆ_ಅಂತ್ಯ
