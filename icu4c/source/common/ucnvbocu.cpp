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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ଅନ୍ତର୍ଭାଵ "unicode/ucnv.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucnv_cb.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_bld.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_cnv.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

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
#define BOCU1_ASCII_PREV        0x40

/* bounding byte values for differences */
#define BOCU1_MIN               0x21
#define BOCU1_ମଧ୍ୟ            0x90
#define BOCU1_MAX_LEAD          0xfe
#define BOCU1_MAX_TRAIL         0xff
#define BOCU1_RESET             0xff

/* number of lead bytes */
#define BOCU1_ଗଣନା             (BOCU1_MAX_LEAD-BOCU1_MIN+1)

/* adjust trail byte counts for the use of some C0 control byte values */
#define BOCU1_TRAIL_CONTROLS_ଗଣନା  20
#define BOCU1_TRAIL_BYTE_OFFSET     (BOCU1_MIN-BOCU1_TRAIL_CONTROLS_ଗଣନା)

/* number of trail bytes */
#define BOCU1_TRAIL_ଗଣନା       ((BOCU1_MAX_TRAIL-BOCU1_MIN+1)+BOCU1_TRAIL_CONTROLS_ଗଣନା)

/*
 * number of positive and negative single-byte codes
 * (counting 0==BOCU1_MIDDLE among the positive ones)
 */
#define BOCU1_SINGLE            64

/* number of lead bytes for positive and negative 2/3/4-byte sequences */
#define BOCU1_LEAD_2            43
#define BOCU1_LEAD_3            3
#define BOCU1_LEAD_4            1

/* The difference value range for single-byters. */
#define BOCU1_REACH_POS_1   (BOCU1_SINGLE-1)
#define BOCU1_REACH_NEG_1   (-BOCU1_SINGLE)

/* The difference value range for double-byters. */
#define BOCU1_REACH_POS_2   (BOCU1_REACH_POS_1+BOCU1_LEAD_2*BOCU1_TRAIL_ଗଣନା)
#define BOCU1_REACH_NEG_2   (BOCU1_REACH_NEG_1-BOCU1_LEAD_2*BOCU1_TRAIL_ଗଣନା)

/* The difference value range for 3-byters. */
#define BOCU1_REACH_POS_3   \
    (BOCU1_REACH_POS_2+BOCU1_LEAD_3*BOCU1_TRAIL_ଗଣନା*BOCU1_TRAIL_ଗଣନା)

#define BOCU1_REACH_NEG_3   (BOCU1_REACH_NEG_2-BOCU1_LEAD_3*BOCU1_TRAIL_ଗଣନା*BOCU1_TRAIL_ଗଣନା)

/* The lead byte start values. */
#define BOCU1_START_POS_2   (BOCU1_ମଧ୍ୟ+BOCU1_REACH_POS_1+1)
#define BOCU1_START_POS_3   (BOCU1_START_POS_2+BOCU1_LEAD_2)
#define BOCU1_START_POS_4   (BOCU1_START_POS_3+BOCU1_LEAD_3)
     /* ==BOCU1_MAX_LEAD */

#define BOCU1_START_NEG_2   (BOCU1_ମଧ୍ୟ+BOCU1_REACH_NEG_1)
#define BOCU1_START_NEG_3   (BOCU1_START_NEG_2-BOCU1_LEAD_2)
#define BOCU1_START_NEG_4   (BOCU1_START_NEG_3-BOCU1_LEAD_3)
     /* ==BOCU1_MIN+1 */

/* The length of a byte sequence, according to the lead byte (!=BOCU1_RESET). */
#define BOCU1_LENGTH_FROM_LEAD(lead) \
    ((BOCU1_START_NEG_2<=(lead) && (lead)<BOCU1_START_POS_2) ? 1 : \
     (BOCU1_START_NEG_3<=(lead) && (lead)<BOCU1_START_POS_3) ? 2 : \
     (BOCU1_START_NEG_4<=(lead) && (lead)<BOCU1_START_POS_4) ? 3 : 4)

/* The length of a byte sequence, according to its packed form. */
#define BOCU1_LENGTH_FROM_PACKED(packed) \
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
#define BOCU1_TRAIL_TO_BYTE(t) ((t)>=BOCU1_TRAIL_CONTROLS_ଗଣନା ? (t)+BOCU1_TRAIL_BYTE_OFFSET : bocu1TrailToByte[t])

/*
 * Byte value map for control codes,
 * from external byte values 0x00..0x20
 * to trail byte values 0..19 (0..0x13) as used in the difference calculation.
 * External byte values that are illegal as trail bytes are mapped to -1.
 */
ସ୍ଥିର ଅଚଳ int8_t
bocu1ByteToTrail[BOCU1_MIN]={
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
ସ୍ଥିର ଅଚଳ int8_t
bocu1TrailToByte[BOCU1_TRAIL_CONTROLS_ଗଣନା]={
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
#define NEGDIVMOD(n, d, m) UPRV_BLOCK_MACRO_BEGIN { \
    (m)=(n)%(d); \
    (n)/=(d); \
    ଯଦି((m)<0) { \
        --(n); \
        (m)+=(d); \
    } \
} UPRV_BLOCK_MACRO_END

/* Faster versions of packDiff() for single-byte-encoded diff values. */

/** Is a diff value encodable in a single byte? */
#define DIFF_IS_SINGLE(diff) (BOCU1_REACH_NEG_1<=(diff) && (diff)<=BOCU1_REACH_POS_1)

/** Encode a diff value in a single byte. */
#define PACK_SINGLE_DIFF(diff) (BOCU1_ମଧ୍ୟ+(diff))

/** Is a diff value encodable in two bytes? */
#define DIFF_IS_DOUBLE(diff) (BOCU1_REACH_NEG_2<=(diff) && (diff)<=BOCU1_REACH_POS_2)

/* BOCU-1 implementation functions ------------------------------------------ */

#define BOCU1_SIMPLE_PREV(c) (((c)&~0x7f)+BOCU1_ASCII_PREV)

/**
 * Compute the next "previous" value for differencing
 * from the current code point.
 *
 * @param c current code point, 0x3040..0xd7a3 (rest handled by macro below)
 * @return "previous code point" state value
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ int32_t
bocu1Prev(int32_t c) {
    /* compute new prev */
    ଯଦି(/* 0x3040<=c && */ c<=0x309f) {
        /* Hiragana is not 128-aligned */
        ଫେରନ୍ତୁ 0x3070;
    } ଅନ୍ୟଥା ଯଦି(0x4e00<=c && c<=0x9fa5) {
        /* CJK Unihan */
        ଫେରନ୍ତୁ 0x4e00-BOCU1_REACH_NEG_2;
    } ଅନ୍ୟଥା ଯଦି(0xac00<=c /* && c<=0xd7a3 */) {
        /* Korean Hangul */
        ଫେରନ୍ତୁ (0xd7a3+0xac00)/2;
    } ଅନ୍ୟଥା {
        /* mostly small scripts */
        ଫେରନ୍ତୁ BOCU1_SIMPLE_PREV(c);
    }
}

/** Fast version of bocu1Prev() for most scripts. */
#define BOCU1_PREV(c) ((c)<0x3040 || (c)>0xd7a3 ? BOCU1_SIMPLE_PREV(c) : bocu1Prev(c))

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
ସ୍ଥିର int32_t
packDiff(int32_t diff) {
    int32_t result, m;

    U_ASSERT(!DIFF_IS_SINGLE(diff)); /* assume we won't be called where diff==BOCU1_REACH_NEG_1=-64 */
    ଯଦି(diff>=BOCU1_REACH_NEG_1) {
        /* mostly positive differences, and single-byte negative ones */
#ଯଦି 0   /* single-byte case handled in macros, see below */
        ଯଦି(diff<=BOCU1_REACH_POS_1) {
            /* single byte */
            ଫେରନ୍ତୁ 0x01000000|(BOCU1_ମଧ୍ୟ+diff);
        } ଅନ୍ୟଥା
#endif
        ଯଦି(diff<=BOCU1_REACH_POS_2) {
            /* two bytes */
            diff-=BOCU1_REACH_POS_1+1;
            result=0x02000000;

            m=diff%BOCU1_TRAIL_ଗଣନା;
            diff/=BOCU1_TRAIL_ଗଣନା;
            result|=BOCU1_TRAIL_TO_BYTE(m);

            result|=(BOCU1_START_POS_2+diff)<<8;
        } ଅନ୍ୟଥା ଯଦି(diff<=BOCU1_REACH_POS_3) {
            /* three bytes */
            diff-=BOCU1_REACH_POS_2+1;
            result=0x03000000;

            m=diff%BOCU1_TRAIL_ଗଣନା;
            diff/=BOCU1_TRAIL_ଗଣନା;
            result|=BOCU1_TRAIL_TO_BYTE(m);

            m=diff%BOCU1_TRAIL_ଗଣନା;
            diff/=BOCU1_TRAIL_ଗଣନା;
            result|=BOCU1_TRAIL_TO_BYTE(m)<<8;

            result|=(BOCU1_START_POS_3+diff)<<16;
        } ଅନ୍ୟଥା {
            /* four bytes */
            diff-=BOCU1_REACH_POS_3+1;

            m=diff%BOCU1_TRAIL_ଗଣନା;
            diff/=BOCU1_TRAIL_ଗଣନା;
            result=BOCU1_TRAIL_TO_BYTE(m);

            m=diff%BOCU1_TRAIL_ଗଣନା;
            diff/=BOCU1_TRAIL_ଗଣନା;
            result|=BOCU1_TRAIL_TO_BYTE(m)<<8;

            /*
             * We know that / and % would deliver quotient 0 and rest=diff.
             * Avoid division and modulo for performance.
             */
            result|=BOCU1_TRAIL_TO_BYTE(diff)<<16;

            result |= static_cast<uint32_t>(BOCU1_START_POS_4) << 24;
        }
    } ଅନ୍ୟଥା {
        /* two- to four-byte negative differences */
        ଯଦି(diff>=BOCU1_REACH_NEG_2) {
            /* two bytes */
            diff-=BOCU1_REACH_NEG_1;
            result=0x02000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_ଗଣନା, m);
            result|=BOCU1_TRAIL_TO_BYTE(m);

            result|=(BOCU1_START_NEG_2+diff)<<8;
        } ଅନ୍ୟଥା ଯଦି(diff>=BOCU1_REACH_NEG_3) {
            /* three bytes */
            diff-=BOCU1_REACH_NEG_2;
            result=0x03000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_ଗଣନା, m);
            result|=BOCU1_TRAIL_TO_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_ଗଣନା, m);
            result|=BOCU1_TRAIL_TO_BYTE(m)<<8;

            result|=(BOCU1_START_NEG_3+diff)<<16;
        } ଅନ୍ୟଥା {
            /* four bytes */
            diff-=BOCU1_REACH_NEG_3;

            NEGDIVMOD(diff, BOCU1_TRAIL_ଗଣନା, m);
            result=BOCU1_TRAIL_TO_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_ଗଣନା, m);
            result|=BOCU1_TRAIL_TO_BYTE(m)<<8;

            /*
             * We know that NEGDIVMOD would deliver
             * quotient -1 and rest=diff+BOCU1_TRAIL_COUNT.
             * Avoid division and modulo for performance.
             */
            m=diff+BOCU1_TRAIL_ଗଣନା;
            result|=BOCU1_TRAIL_TO_BYTE(m)<<16;

            result|=BOCU1_MIN<<24;
        }
    }
    ଫେରନ୍ତୁ result;
}


ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
_Bocu1FromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                             UErrorCode *pErrorCode) {
    UConverter *cnv;
    ଅଚଳ char16_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity;
    int32_t *offsets;

    int32_t prev, c, diff;

    int32_t sourceIndex, nextSourceIndex;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source=pArgs->source;
    sourceLimit=pArgs->sourceLimit;
    target = reinterpret_cast<uint8_t*>(pArgs->target);
    targetCapacity = static_cast<int32_t>(pArgs->targetLimit - pArgs->target);
    offsets=pArgs->offsets;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    prev = static_cast<int32_t>(cnv->fromUnicodeStatus);
    ଯଦି(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    sourceIndex= c==0 ? 0 : -1;
    nextSourceIndex=0;

    /* conversion loop */
    ଯଦି(c!=0 && targetCapacity>0) {
        ଯାଅ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(sourceLimit - source);
    ଯଦି(targetCapacity>diff) {
        targetCapacity=diff;
    }
    ଯେତେବେଳେ(targetCapacity>0 && (c=*source)<0x3000) {
        ଯଦି(c<=0x20) {
            ଯଦି(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *target++ = static_cast<uint8_t>(c);
            *offsets++=nextSourceIndex++;
            ++source;
            --targetCapacity;
        } ଅନ୍ୟଥା {
            diff=c-prev;
            ଯଦି(DIFF_IS_SINGLE(diff)) {
                prev=BOCU1_SIMPLE_PREV(c);
                *target++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=nextSourceIndex++;
                ++source;
                --targetCapacity;
            } ଅନ୍ୟଥା {
                ଭାଙ୍ଗ;
            }
        }
    }
    /* restore real values */
    targetCapacity = static_cast<int32_t>(reinterpret_cast<ଅଚଳ uint8_t*>(pArgs->targetLimit) - target);
    sourceIndex=nextSourceIndex; /* wrong if offsets==nullptr but does not matter */

    /* regular loop for all cases */
    ଯେତେବେଳେ(source<sourceLimit) {
        ଯଦି(targetCapacity>0) {
            c=*source++;
            ++nextSourceIndex;

            ଯଦି(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                ଯଦି(c!=0x20) {
                    prev=BOCU1_ASCII_PREV;
                }
                *target++ = static_cast<uint8_t>(c);
                *offsets++=sourceIndex;
                --targetCapacity;

                sourceIndex=nextSourceIndex;
                ଜାରି;
            }

            ଯଦି(U16_IS_LEAD(c)) {
getTrail:
                ଯଦି(source<sourceLimit) {
                    /* test the following code unit */
                    char16_t trail=*source;
                    ଯଦି(U16_IS_TRAIL(trail)) {
                        ++source;
                        ++nextSourceIndex;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } ଅନ୍ୟଥା {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    ଭାଙ୍ଗ;
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
            ଯଦି(DIFF_IS_SINGLE(diff)) {
                *target++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=sourceIndex;
                --targetCapacity;
                sourceIndex=nextSourceIndex;
                ଯଦି(c<0x3000) {
                    ଯାଅ fastSingle;
                }
            } ଅନ୍ୟଥା ଯଦି(DIFF_IS_DOUBLE(diff) && 2<=targetCapacity) {
                /* optimize 2-byte case */
                int32_t m;

                ଯଦି(diff>=0) {
                    diff-=BOCU1_REACH_POS_1+1;
                    m=diff%BOCU1_TRAIL_ଗଣନା;
                    diff/=BOCU1_TRAIL_ଗଣନା;
                    diff+=BOCU1_START_POS_2;
                } ଅନ୍ୟଥା {
                    diff-=BOCU1_REACH_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_ଗଣନା, m);
                    diff+=BOCU1_START_NEG_2;
                }
                *target++ = static_cast<uint8_t>(diff);
                *target++ = static_cast<uint8_t>(BOCU1_TRAIL_TO_BYTE(m));
                *offsets++=sourceIndex;
                *offsets++=sourceIndex;
                targetCapacity-=2;
                sourceIndex=nextSourceIndex;
            } ଅନ୍ୟଥା {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_LENGTH_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                ଯଦି(length<=targetCapacity) {
                    ବାଛ(length) {
                        /* each branch falls through to the next one */
                    ସ୍ଥିତି 4:
                        *target++ = static_cast<uint8_t>(diff >> 24);
                        *offsets++=sourceIndex;
                        U_FALLTHROUGH;
                    ସ୍ଥିତି 3:
                        *target++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=sourceIndex;
                        U_FALLTHROUGH;
                    ସ୍ଥିତି 2:
                        *target++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=sourceIndex;
                    /* case 1: handled above */
                        *target++ = static_cast<uint8_t>(diff);
                        *offsets++=sourceIndex;
                        U_FALLTHROUGH;
                    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        /* will never occur */
                        ଭାଙ୍ଗ;
                    }
                    targetCapacity-=length;
                    sourceIndex=nextSourceIndex;
                } ଅନ୍ୟଥା {
                    uint8_t *charErrorBuffer;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=targetCapacity;
                    charErrorBuffer=(uint8_t *)cnv->charErrorBuffer;
                    ବାଛ(length) {
                        /* each branch falls through to the next one */
                    ସ୍ଥିତି 3:
                        *charErrorBuffer++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    ସ୍ଥିତି 2:
                        *charErrorBuffer++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    ସ୍ଥିତି 1:
                        *charErrorBuffer = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        /* will never occur */
                        ଭାଙ୍ଗ;
                    }
                    cnv->charErrorBufferLength = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    ବାଛ(targetCapacity) {
                        /* each branch falls through to the next one */
                    ସ୍ଥିତି 3:
                        *target++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=sourceIndex;
                        U_FALLTHROUGH;
                    ସ୍ଥିତି 2:
                        *target++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=sourceIndex;
                        U_FALLTHROUGH;
                    ସ୍ଥିତି 1:
                        *target++ = static_cast<uint8_t>(diff);
                        *offsets++=sourceIndex;
                        U_FALLTHROUGH;
                    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        /* will never occur */
                        ଭାଙ୍ଗ;
                    }

                    /* target overflow */
                    targetCapacity=0;
                    *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                    ଭାଙ୍ଗ;
                }
            }
        } ଅନ୍ୟଥା {
            /* target is full */
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            ଭାଙ୍ଗ;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->fromUnicodeStatus = static_cast<uint32_t>(prev);

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target = reinterpret_cast<ଅକ୍ଷର*>(target);
    pArgs->offsets=offsets;
}

/*
 * Identical to _Bocu1FromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
_Bocu1FromUnicode(UConverterFromUnicodeArgs *pArgs,
                  UErrorCode *pErrorCode) {
    UConverter *cnv;
    ଅଚଳ char16_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity;

    int32_t prev, c, diff;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source=pArgs->source;
    sourceLimit=pArgs->sourceLimit;
    target = reinterpret_cast<uint8_t*>(pArgs->target);
    targetCapacity = static_cast<int32_t>(pArgs->targetLimit - pArgs->target);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    prev = static_cast<int32_t>(cnv->fromUnicodeStatus);
    ଯଦି(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }

    /* conversion loop */
    ଯଦି(c!=0 && targetCapacity>0) {
        ଯାଅ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(sourceLimit - source);
    ଯଦି(targetCapacity>diff) {
        targetCapacity=diff;
    }
    ଯେତେବେଳେ(targetCapacity>0 && (c=*source)<0x3000) {
        ଯଦି(c<=0x20) {
            ଯଦି(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *target++ = static_cast<uint8_t>(c);
        } ଅନ୍ୟଥା {
            diff=c-prev;
            ଯଦି(DIFF_IS_SINGLE(diff)) {
                prev=BOCU1_SIMPLE_PREV(c);
                *target++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
            } ଅନ୍ୟଥା {
                ଭାଙ୍ଗ;
            }
        }
        ++source;
        --targetCapacity;
    }
    /* restore real values */
    targetCapacity = static_cast<int32_t>(reinterpret_cast<ଅଚଳ uint8_t*>(pArgs->targetLimit) - target);

    /* regular loop for all cases */
    ଯେତେବେଳେ(source<sourceLimit) {
        ଯଦି(targetCapacity>0) {
            c=*source++;

            ଯଦି(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                ଯଦି(c!=0x20) {
                    prev=BOCU1_ASCII_PREV;
                }
                *target++ = static_cast<uint8_t>(c);
                --targetCapacity;
                ଜାରି;
            }

            ଯଦି(U16_IS_LEAD(c)) {
getTrail:
                ଯଦି(source<sourceLimit) {
                    /* test the following code unit */
                    char16_t trail=*source;
                    ଯଦି(U16_IS_TRAIL(trail)) {
                        ++source;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } ଅନ୍ୟଥା {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    ଭାଙ୍ଗ;
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
            ଯଦି(DIFF_IS_SINGLE(diff)) {
                *target++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                --targetCapacity;
                ଯଦି(c<0x3000) {
                    ଯାଅ fastSingle;
                }
            } ଅନ୍ୟଥା ଯଦି(DIFF_IS_DOUBLE(diff) && 2<=targetCapacity) {
                /* optimize 2-byte case */
                int32_t m;

                ଯଦି(diff>=0) {
                    diff-=BOCU1_REACH_POS_1+1;
                    m=diff%BOCU1_TRAIL_ଗଣନା;
                    diff/=BOCU1_TRAIL_ଗଣନା;
                    diff+=BOCU1_START_POS_2;
                } ଅନ୍ୟଥା {
                    diff-=BOCU1_REACH_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_ଗଣନା, m);
                    diff+=BOCU1_START_NEG_2;
                }
                *target++ = static_cast<uint8_t>(diff);
                *target++ = static_cast<uint8_t>(BOCU1_TRAIL_TO_BYTE(m));
                targetCapacity-=2;
            } ଅନ୍ୟଥା {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_LENGTH_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                ଯଦି(length<=targetCapacity) {
                    ବାଛ(length) {
                        /* each branch falls through to the next one */
                    ସ୍ଥିତି 4:
                        *target++ = static_cast<uint8_t>(diff >> 24);
                        U_FALLTHROUGH;
                    ସ୍ଥିତି 3:
                        *target++ = static_cast<uint8_t>(diff >> 16);
                    /* case 2: handled above */
                        *target++ = static_cast<uint8_t>(diff >> 8);
                    /* case 1: handled above */
                        *target++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        /* will never occur */
                        ଭାଙ୍ଗ;
                    }
                    targetCapacity-=length;
                } ଅନ୍ୟଥା {
                    uint8_t *charErrorBuffer;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=targetCapacity;
                    charErrorBuffer=(uint8_t *)cnv->charErrorBuffer;
                    ବାଛ(length) {
                        /* each branch falls through to the next one */
                    ସ୍ଥିତି 3:
                        *charErrorBuffer++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    ସ୍ଥିତି 2:
                        *charErrorBuffer++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    ସ୍ଥିତି 1:
                        *charErrorBuffer = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        /* will never occur */
                        ଭାଙ୍ଗ;
                    }
                    cnv->charErrorBufferLength = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    ବାଛ(targetCapacity) {
                        /* each branch falls through to the next one */
                    ସ୍ଥିତି 3:
                        *target++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    ସ୍ଥିତି 2:
                        *target++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    ସ୍ଥିତି 1:
                        *target++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        /* will never occur */
                        ଭାଙ୍ଗ;
                    }

                    /* target overflow */
                    targetCapacity=0;
                    *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                    ଭାଙ୍ଗ;
                }
            }
        } ଅନ୍ୟଥା {
            /* target is full */
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            ଭାଙ୍ଗ;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->fromUnicodeStatus = static_cast<uint32_t>(prev);

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target = reinterpret_cast<ଅକ୍ଷର*>(target);
}

/* BOCU-1-to-Unicode conversion functions ----------------------------------- */

/**
 * Function for BOCU-1 decoder; handles multi-byte lead bytes.
 *
 * @param b lead byte;
 *          BOCU1_MIN<=b<BOCU1_START_NEG_2 or BOCU1_START_POS_2<=b<BOCU1_MAX_LEAD
 * @return (diff<<2)|count
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ int32_t
decodeBocu1LeadByte(int32_t b) {
    int32_t diff, count;

    ଯଦି(b>=BOCU1_START_NEG_2) {
        /* positive difference */
        ଯଦି(b<BOCU1_START_POS_3) {
            /* two bytes */
            diff = (b - BOCU1_START_POS_2) * BOCU1_TRAIL_ଗଣନା + BOCU1_REACH_POS_1 + 1;
            count=1;
        } ଅନ୍ୟଥା ଯଦି(b<BOCU1_START_POS_4) {
            /* three bytes */
            diff = (b - BOCU1_START_POS_3) * BOCU1_TRAIL_ଗଣନା * BOCU1_TRAIL_ଗଣନା + BOCU1_REACH_POS_2 + 1;
            count=2;
        } ଅନ୍ୟଥା {
            /* four bytes */
            diff=BOCU1_REACH_POS_3+1;
            count=3;
        }
    } ଅନ୍ୟଥା {
        /* negative difference */
        ଯଦି(b>=BOCU1_START_NEG_3) {
            /* two bytes */
            diff = (b - BOCU1_START_NEG_2) * BOCU1_TRAIL_ଗଣନା + BOCU1_REACH_NEG_1;
            count=1;
        } ଅନ୍ୟଥା ଯଦି(b>BOCU1_MIN) {
            /* three bytes */
            diff = (b - BOCU1_START_NEG_3) * BOCU1_TRAIL_ଗଣନା * BOCU1_TRAIL_ଗଣନା + BOCU1_REACH_NEG_2;
            count=2;
        } ଅନ୍ୟଥା {
            /* four bytes */
            diff=-BOCU1_TRAIL_ଗଣନା*BOCU1_TRAIL_ଗଣନା*BOCU1_TRAIL_ଗଣନା+BOCU1_REACH_NEG_3;
            count=3;
        }
    }

    /* return the state for decoding the trail byte(s) */
    ଫେରନ୍ତୁ (static_cast<uint32_t>(diff) << 2) | count;
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
ସ୍ଥିର ଧାଡ଼ିଭିତରେ int32_t
decodeBocu1TrailByte(int32_t count, int32_t b) {
    ଯଦି(b<=0x20) {
        /* skip some C0 controls and make the trail byte range contiguous */
        b=bocu1ByteToTrail[b];
        /* b<0 for an illegal trail byte value will result in return<0 below */
#ଯଦି BOCU1_MAX_TRAIL<0xff
    } ଅନ୍ୟଥା ଯଦି(b>BOCU1_MAX_TRAIL) {
        ଫେରନ୍ତୁ -99;
#endif
    } ଅନ୍ୟଥା {
        b-=BOCU1_TRAIL_BYTE_OFFSET;
    }

    /* add trail byte into difference and decrement count */
    ଯଦି(count==1) {
        ଫେରନ୍ତୁ b;
    } ଅନ୍ୟଥା ଯଦି(count==2) {
        ଫେରନ୍ତୁ b*BOCU1_TRAIL_ଗଣନା;
    } ଅନ୍ୟଥା /* count==3 */ {
        ଫେରନ୍ତୁ b*(BOCU1_TRAIL_ଗଣନା*BOCU1_TRAIL_ଗଣନା);
    }
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
_Bocu1ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv;
    ଅଚଳ uint8_t *source, *sourceLimit;
    char16_t *target;
    ଅଚଳ char16_t *targetLimit;
    int32_t *offsets;

    int32_t prev, count, diff, c;

    int8_t byteIndex;
    uint8_t *bytes;

    int32_t sourceIndex, nextSourceIndex;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source = reinterpret_cast<ଅଚଳ uint8_t*>(pArgs->source);
    sourceLimit = reinterpret_cast<ଅଚଳ uint8_t*>(pArgs->sourceLimit);
    target=pArgs->target;
    targetLimit=pArgs->targetLimit;
    offsets=pArgs->offsets;

    /* get the converter state from UConverter */
    prev = static_cast<int32_t>(cnv->toUnicodeStatus);
    ଯଦି(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }
    diff=cnv->mode; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byteIndex=cnv->toULength;
    bytes=cnv->toUBytes;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    sourceIndex=byteIndex==0 ? 0 : -1;
    nextSourceIndex=0;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    ଯଦି(count>0 && byteIndex>0 && target<targetLimit) {
        ଯାଅ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(sourceLimit - source);
    count = static_cast<int32_t>(pArgs->targetLimit - target);
    ଯଦି(count>diff) {
        count=diff;
    }
    ଯେତେବେଳେ(count>0) {
        ଯଦି(BOCU1_START_NEG_2<=(c=*source) && c<BOCU1_START_POS_2) {
            c=prev+(c-BOCU1_ମଧ୍ୟ);
            ଯଦି(c<0x3000) {
                *target++ = static_cast<char16_t>(c);
                *offsets++=nextSourceIndex++;
                prev=BOCU1_SIMPLE_PREV(c);
            } ଅନ୍ୟଥା {
                ଭାଙ୍ଗ;
            }
        } ଅନ୍ୟଥା ଯଦି(c<=0x20) {
            ଯଦି(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *target++ = static_cast<char16_t>(c);
            *offsets++=nextSourceIndex++;
        } ଅନ୍ୟଥା {
            ଭାଙ୍ଗ;
        }
        ++source;
        --count;
    }
    sourceIndex=nextSourceIndex; /* wrong if offsets==nullptr but does not matter */

    /* decode a sequence of single and lead bytes */
    ଯେତେବେଳେ(source<sourceLimit) {
        ଯଦି(target>=targetLimit) {
            /* target is full */
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            ଭାଙ୍ଗ;
        }

        ++nextSourceIndex;
        c=*source++;
        ଯଦି(BOCU1_START_NEG_2<=c && c<BOCU1_START_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=prev+(c-BOCU1_ମଧ୍ୟ);
            ଯଦି(c<0x3000) {
                *target++ = static_cast<char16_t>(c);
                *offsets++=sourceIndex;
                prev=BOCU1_SIMPLE_PREV(c);
                sourceIndex=nextSourceIndex;
                ଯାଅ fastSingle;
            }
        } ଅନ୍ୟଥା ଯଦି(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            ଯଦି(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *target++ = static_cast<char16_t>(c);
            *offsets++=sourceIndex;
            sourceIndex=nextSourceIndex;
            ଜାରି;
        } ଅନ୍ୟଥା ଯଦି(BOCU1_START_NEG_3<=c && c<BOCU1_START_POS_3 && source<sourceLimit) {
            /* Optimize two-byte case. */
            ଯଦି(c>=BOCU1_ମଧ୍ୟ) {
                diff = (c - BOCU1_START_POS_2) * BOCU1_TRAIL_ଗଣନା + BOCU1_REACH_POS_1 + 1;
            } ଅନ୍ୟଥା {
                diff = (c - BOCU1_START_NEG_2) * BOCU1_TRAIL_ଗଣନା + BOCU1_REACH_NEG_1;
            }

            /* trail byte */
            ++nextSourceIndex;
            c=decodeBocu1TrailByte(1, *source++);
            ଯଦି (c < 0 || static_cast<uint32_t>(c = prev + diff + c) > 0x10ffff) {
                bytes[0]=source[-2];
                bytes[1]=source[-1];
                byteIndex=2;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ଭାଙ୍ଗ;
            }
        } ଅନ୍ୟଥା ଯଦି(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            prev=BOCU1_ASCII_PREV;
            sourceIndex=nextSourceIndex;
            ଜାରି;
        } ଅନ୍ୟଥା {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            bytes[0] = static_cast<uint8_t>(c);
            byteIndex=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            ପାଇଁ(;;) {
                ଯଦି(source>=sourceLimit) {
                    ଯାଅ endloop;
                }
                ++nextSourceIndex;
                c=bytes[byteIndex++]=*source++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                ଯଦି(c<0) {
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                    ଯାଅ endloop;
                }

                diff+=c;
                ଯଦି(--count==0) {
                    /* final trail byte, deliver a code point */
                    byteIndex=0;
                    c=prev+diff;
                    ଯଦି (static_cast<uint32_t>(c) > 0x10ffff) {
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        ଯାଅ endloop;
                    }
                    ଭାଙ୍ଗ;
                }
            }
        }

        /* calculate the next prev and output c */
        prev=BOCU1_PREV(c);
        ଯଦି(c<=0xffff) {
            *target++ = static_cast<char16_t>(c);
            *offsets++=sourceIndex;
        } ଅନ୍ୟଥା {
            /* output surrogate pair */
            *target++=U16_LEAD(c);
            ଯଦି(target<targetLimit) {
                *target++=U16_TRAIL(c);
                *offsets++=sourceIndex;
                *offsets++=sourceIndex;
            } ଅନ୍ୟଥା {
                /* target overflow */
                *offsets++=sourceIndex;
                cnv->UCharErrorBuffer[0]=U16_TRAIL(c);
                cnv->UCharErrorBufferLength=1;
                *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                ଭାଙ୍ଗ;
            }
        }
        sourceIndex=nextSourceIndex;
    }
endloop:

    ଯଦି(*pErrorCode==U_ILLEGAL_CHAR_FOUND) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->toUnicodeStatus=BOCU1_ASCII_PREV;
        cnv->mode=0;
    } ଅନ୍ୟଥା {
        /* set the converter state back into UConverter */
        cnv->toUnicodeStatus = static_cast<uint32_t>(prev);
        cnv->mode = static_cast<int32_t>(static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->toULength=byteIndex;

    /* write back the updated pointers */
    pArgs->source = reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(source);
    pArgs->target=target;
    pArgs->offsets=offsets;
}

/*
 * Identical to _Bocu1ToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
_Bocu1ToUnicode(UConverterToUnicodeArgs *pArgs,
                UErrorCode *pErrorCode) {
    UConverter *cnv;
    ଅଚଳ uint8_t *source, *sourceLimit;
    char16_t *target;
    ଅଚଳ char16_t *targetLimit;

    int32_t prev, count, diff, c;

    int8_t byteIndex;
    uint8_t *bytes;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source = reinterpret_cast<ଅଚଳ uint8_t*>(pArgs->source);
    sourceLimit = reinterpret_cast<ଅଚଳ uint8_t*>(pArgs->sourceLimit);
    target=pArgs->target;
    targetLimit=pArgs->targetLimit;

    /* get the converter state from UConverter */
    prev = static_cast<int32_t>(cnv->toUnicodeStatus);
    ଯଦି(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }
    diff=cnv->mode; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byteIndex=cnv->toULength;
    bytes=cnv->toUBytes;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    ଯଦି(count>0 && byteIndex>0 && target<targetLimit) {
        ଯାଅ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(sourceLimit - source);
    count = static_cast<int32_t>(pArgs->targetLimit - target);
    ଯଦି(count>diff) {
        count=diff;
    }
    ଯେତେବେଳେ(count>0) {
        ଯଦି(BOCU1_START_NEG_2<=(c=*source) && c<BOCU1_START_POS_2) {
            c=prev+(c-BOCU1_ମଧ୍ୟ);
            ଯଦି(c<0x3000) {
                *target++ = static_cast<char16_t>(c);
                prev=BOCU1_SIMPLE_PREV(c);
            } ଅନ୍ୟଥା {
                ଭାଙ୍ଗ;
            }
        } ଅନ୍ୟଥା ଯଦି(c<=0x20) {
            ଯଦି(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *target++ = static_cast<char16_t>(c);
        } ଅନ୍ୟଥା {
            ଭାଙ୍ଗ;
        }
        ++source;
        --count;
    }

    /* decode a sequence of single and lead bytes */
    ଯେତେବେଳେ(source<sourceLimit) {
        ଯଦି(target>=targetLimit) {
            /* target is full */
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            ଭାଙ୍ଗ;
        }

        c=*source++;
        ଯଦି(BOCU1_START_NEG_2<=c && c<BOCU1_START_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=prev+(c-BOCU1_ମଧ୍ୟ);
            ଯଦି(c<0x3000) {
                *target++ = static_cast<char16_t>(c);
                prev=BOCU1_SIMPLE_PREV(c);
                ଯାଅ fastSingle;
            }
        } ଅନ୍ୟଥା ଯଦି(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            ଯଦି(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *target++ = static_cast<char16_t>(c);
            ଜାରି;
        } ଅନ୍ୟଥା ଯଦି(BOCU1_START_NEG_3<=c && c<BOCU1_START_POS_3 && source<sourceLimit) {
            /* Optimize two-byte case. */
            ଯଦି(c>=BOCU1_ମଧ୍ୟ) {
                diff = (c - BOCU1_START_POS_2) * BOCU1_TRAIL_ଗଣନା + BOCU1_REACH_POS_1 + 1;
            } ଅନ୍ୟଥା {
                diff = (c - BOCU1_START_NEG_2) * BOCU1_TRAIL_ଗଣନା + BOCU1_REACH_NEG_1;
            }

            /* trail byte */
            c=decodeBocu1TrailByte(1, *source++);
            ଯଦି (c < 0 || static_cast<uint32_t>(c = prev + diff + c) > 0x10ffff) {
                bytes[0]=source[-2];
                bytes[1]=source[-1];
                byteIndex=2;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                ଭାଙ୍ଗ;
            }
        } ଅନ୍ୟଥା ଯଦି(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            prev=BOCU1_ASCII_PREV;
            ଜାରି;
        } ଅନ୍ୟଥା {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            bytes[0] = static_cast<uint8_t>(c);
            byteIndex=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            ପାଇଁ(;;) {
                ଯଦି(source>=sourceLimit) {
                    ଯାଅ endloop;
                }
                c=bytes[byteIndex++]=*source++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                ଯଦି(c<0) {
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                    ଯାଅ endloop;
                }

                diff+=c;
                ଯଦି(--count==0) {
                    /* final trail byte, deliver a code point */
                    byteIndex=0;
                    c=prev+diff;
                    ଯଦି (static_cast<uint32_t>(c) > 0x10ffff) {
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        ଯାଅ endloop;
                    }
                    ଭାଙ୍ଗ;
                }
            }
        }

        /* calculate the next prev and output c */
        prev=BOCU1_PREV(c);
        ଯଦି(c<=0xffff) {
            *target++ = static_cast<char16_t>(c);
        } ଅନ୍ୟଥା {
            /* output surrogate pair */
            *target++=U16_LEAD(c);
            ଯଦି(target<targetLimit) {
                *target++=U16_TRAIL(c);
            } ଅନ୍ୟଥା {
                /* target overflow */
                cnv->UCharErrorBuffer[0]=U16_TRAIL(c);
                cnv->UCharErrorBufferLength=1;
                *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                ଭାଙ୍ଗ;
            }
        }
    }
endloop:

    ଯଦି(*pErrorCode==U_ILLEGAL_CHAR_FOUND) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->toUnicodeStatus=BOCU1_ASCII_PREV;
        cnv->mode=0;
    } ଅନ୍ୟଥା {
        /* set the converter state back into UConverter */
        cnv->toUnicodeStatus = static_cast<uint32_t>(prev);
        cnv->mode = (static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->toULength=byteIndex;

    /* write back the updated pointers */
    pArgs->source = reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(source);
    pArgs->target=target;
}

/* miscellaneous ------------------------------------------------------------ */

ସ୍ଥିର ଅଚଳ UConverterImpl _Bocu1Impl={
    UCNV_BOCU1,

    nullptr,
    nullptr,

    nullptr,
    nullptr,
    nullptr,

    _Bocu1ToUnicode,
    _Bocu1ToUnicodeWithOffsets,
    _Bocu1FromUnicode,
    _Bocu1FromUnicodeWithOffsets,
    nullptr,

    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ucnv_getCompleteUnicodeSet,

    nullptr,
    nullptr
};

ସ୍ଥିର ଅଚଳ UConverterStaticData _Bocu1StaticData={
    ଆକାର(UConverterStaticData),
    "BOCU-1",
    1214, /* CCSID for BOCU-1 */
    UCNV_IBM, UCNV_BOCU1,
    1, 4, /* one char16_t generates at least 1 byte and at most 4 bytes */
    { 0x1a, 0, 0, 0 }, 1, /* BOCU-1 never needs to write a subchar */
    ମିଥ୍ୟା, ମିଥ୍ୟା,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ଅଚଳ UConverterSharedData _Bocu1Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_Bocu1StaticData, &_Bocu1Impl);

#endif
