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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "unicode/ucnv_cb.h"
#ضمن "unicode/utf16.h"
#ضمن "putilimp.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"
#ضمن "uassert.h"

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
#وصف_ڪريو BOCU1_ASCII_PREV        0x40

/* bounding byte values for differences */
#وصف_ڪريو BOCU1_MIN               0x21
#وصف_ڪريو BOCU1_وچ            0x90
#وصف_ڪريو BOCU1_MAX_LEAD          0xfe
#وصف_ڪريو BOCU1_MAX_TRAIL         0xff
#وصف_ڪريو BOCU1_RESET             0xff

/* number of lead bytes */
#وصف_ڪريو BOCU1_ڳڻپ             (BOCU1_MAX_LEAD-BOCU1_MIN+1)

/* adjust trail byte counts for the use of some C0 control byte values */
#وصف_ڪريو BOCU1_TRAIL_CONTROLS_ڳڻپ  20
#وصف_ڪريو BOCU1_TRAIL_BYTE_وٿي     (BOCU1_MIN-BOCU1_TRAIL_CONTROLS_ڳڻپ)

/* number of trail bytes */
#وصف_ڪريو BOCU1_TRAIL_ڳڻپ       ((BOCU1_MAX_TRAIL-BOCU1_MIN+1)+BOCU1_TRAIL_CONTROLS_ڳڻپ)

/*
 * number of positive and negative single-byte codes
 * (counting 0==BOCU1_MIDDLE among the positive ones)
 */
#وصف_ڪريو BOCU1_SINGLE            64

/* number of lead bytes for positive and negative 2/3/4-byte sequences */
#وصف_ڪريو BOCU1_LEAD_2            43
#وصف_ڪريو BOCU1_LEAD_3            3
#وصف_ڪريو BOCU1_LEAD_4            1

/* The difference value range for single-byters. */
#وصف_ڪريو BOCU1_پهچ_POS_1   (BOCU1_SINGLE-1)
#وصف_ڪريو BOCU1_پهچ_NEG_1   (-BOCU1_SINGLE)

/* The difference value range for double-byters. */
#وصف_ڪريو BOCU1_پهچ_POS_2   (BOCU1_پهچ_POS_1+BOCU1_LEAD_2*BOCU1_TRAIL_ڳڻپ)
#وصف_ڪريو BOCU1_پهچ_NEG_2   (BOCU1_پهچ_NEG_1-BOCU1_LEAD_2*BOCU1_TRAIL_ڳڻپ)

/* The difference value range for 3-byters. */
#وصف_ڪريو BOCU1_پهچ_POS_3   \
    (BOCU1_پهچ_POS_2+BOCU1_LEAD_3*BOCU1_TRAIL_ڳڻپ*BOCU1_TRAIL_ڳڻپ)

#وصف_ڪريو BOCU1_پهچ_NEG_3   (BOCU1_پهچ_NEG_2-BOCU1_LEAD_3*BOCU1_TRAIL_ڳڻپ*BOCU1_TRAIL_ڳڻپ)

/* The lead byte start values. */
#وصف_ڪريو BOCU1_شروعات_POS_2   (BOCU1_وچ+BOCU1_پهچ_POS_1+1)
#وصف_ڪريو BOCU1_شروعات_POS_3   (BOCU1_شروعات_POS_2+BOCU1_LEAD_2)
#وصف_ڪريو BOCU1_شروعات_POS_4   (BOCU1_شروعات_POS_3+BOCU1_LEAD_3)
     /* ==BOCU1_MAX_LEAD */

#وصف_ڪريو BOCU1_شروعات_NEG_2   (BOCU1_وچ+BOCU1_پهچ_NEG_1)
#وصف_ڪريو BOCU1_شروعات_NEG_3   (BOCU1_شروعات_NEG_2-BOCU1_LEAD_2)
#وصف_ڪريو BOCU1_شروعات_NEG_4   (BOCU1_شروعات_NEG_3-BOCU1_LEAD_3)
     /* ==BOCU1_MIN+1 */

/* The length of a byte sequence, according to the lead byte (!=BOCU1_RESET). */
#وصف_ڪريو BOCU1_ڊيگهائي_FROM_LEAD(lead) \
    ((BOCU1_شروعات_NEG_2<=(lead) && (lead)<BOCU1_شروعات_POS_2) ? 1 : \
     (BOCU1_شروعات_NEG_3<=(lead) && (lead)<BOCU1_شروعات_POS_3) ? 2 : \
     (BOCU1_شروعات_NEG_4<=(lead) && (lead)<BOCU1_شروعات_POS_4) ? 3 : 4)

/* The length of a byte sequence, according to its packed form. */
#وصف_ڪريو BOCU1_ڊيگهائي_FROM_PACKED(packed) \
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
#وصف_ڪريو BOCU1_TRAIL_منزل_BYTE(t) ((t)>=BOCU1_TRAIL_CONTROLS_ڳڻپ ? (t)+BOCU1_TRAIL_BYTE_وٿي : bocu1_Trail_منزل_Byte[t])

/*
 * Byte value map for control codes,
 * from external byte values 0x00..0x20
 * to trail byte values 0..19 (0..0x13) as used in the difference calculation.
 * External byte values that are illegal as trail bytes are mapped to -1.
 */
ساكن ثابت int8_t
bocu1_Byte_منزل_Trail[BOCU1_MIN]={
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
ساكن ثابت int8_t
bocu1_Trail_منزل_Byte[BOCU1_TRAIL_CONTROLS_ڳڻپ]={
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
#وصف_ڪريو NEGDIVMOD(n, d, m) UPRV_بلاڪ_MACRO_BEGIN { \
    (m)=(n)%(d); \
    (n)/=(d); \
    إذا((m)<0) { \
        --(n); \
        (m)+=(d); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/* Faster versions of packDiff() for single-byte-encoded diff values. */

/** Is a diff value encodable in a single byte? */
#وصف_ڪريو DIFF_IS_SINGLE(diff) (BOCU1_پهچ_NEG_1<=(diff) && (diff)<=BOCU1_پهچ_POS_1)

/** Encode a diff value in a single byte. */
#وصف_ڪريو PACK_SINGLE_DIFF(diff) (BOCU1_وچ+(diff))

/** Is a diff value encodable in two bytes? */
#وصف_ڪريو DIFF_IS_DOUBLE(diff) (BOCU1_پهچ_NEG_2<=(diff) && (diff)<=BOCU1_پهچ_POS_2)

/* BOCU-1 implementation functions ------------------------------------------ */

#وصف_ڪريو BOCU1_SIMPLE_PREV(c) (((c)&~0x7f)+BOCU1_ASCII_PREV)

/**
 * Compute the next "previous" value for differencing
 * from the current code point.
 *
 * @param c current code point, 0x3040..0xd7a3 (rest handled by macro below)
 * @return "previous code point" state value
 */
ساكن مضمن int32_t
bocu1Prev(int32_t c) {
    /* compute new prev */
    إذا(/* 0x3040<=c && */ c<=0x309f) {
        /* Hiragana is not 128-aligned */
        أرجع 0x3070;
    } وإلا إذا(0x4e00<=c && c<=0x9fa5) {
        /* CJK Unihan */
        أرجع 0x4e00-BOCU1_پهچ_NEG_2;
    } وإلا إذا(0xac00<=c /* && c<=0xd7a3 */) {
        /* Korean Hangul */
        أرجع (0xd7a3+0xac00)/2;
    } وإلا {
        /* mostly small scripts */
        أرجع BOCU1_SIMPLE_PREV(c);
    }
}

/** Fast version of bocu1Prev() for most scripts. */
#وصف_ڪريو BOCU1_PREV(c) ((c)<0x3040 || (c)>0xd7a3 ? BOCU1_SIMPLE_PREV(c) : bocu1Prev(c))

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
ساكن int32_t
packDiff(int32_t diff) {
    int32_t نتيجو, m;

    U_ASSERT(!DIFF_IS_SINGLE(diff)); /* assume we won't be called where diff==BOCU1_REACH_NEG_1=-64 */
    إذا(diff>=BOCU1_پهچ_NEG_1) {
        /* mostly positive differences, and single-byte negative ones */
#إذا 0   /* single-byte case handled in macros, see below */
        إذا(diff<=BOCU1_پهچ_POS_1) {
            /* single byte */
            أرجع 0x01000000|(BOCU1_وچ+diff);
        } وإلا
#جيڪڏهن_پورو
        إذا(diff<=BOCU1_پهچ_POS_2) {
            /* two bytes */
            diff-=BOCU1_پهچ_POS_1+1;
            نتيجو=0x02000000;

            m=diff%BOCU1_TRAIL_ڳڻپ;
            diff/=BOCU1_TRAIL_ڳڻپ;
            نتيجو|=BOCU1_TRAIL_منزل_BYTE(m);

            نتيجو|=(BOCU1_شروعات_POS_2+diff)<<8;
        } وإلا إذا(diff<=BOCU1_پهچ_POS_3) {
            /* three bytes */
            diff-=BOCU1_پهچ_POS_2+1;
            نتيجو=0x03000000;

            m=diff%BOCU1_TRAIL_ڳڻپ;
            diff/=BOCU1_TRAIL_ڳڻپ;
            نتيجو|=BOCU1_TRAIL_منزل_BYTE(m);

            m=diff%BOCU1_TRAIL_ڳڻپ;
            diff/=BOCU1_TRAIL_ڳڻپ;
            نتيجو|=BOCU1_TRAIL_منزل_BYTE(m)<<8;

            نتيجو|=(BOCU1_شروعات_POS_3+diff)<<16;
        } وإلا {
            /* four bytes */
            diff-=BOCU1_پهچ_POS_3+1;

            m=diff%BOCU1_TRAIL_ڳڻپ;
            diff/=BOCU1_TRAIL_ڳڻپ;
            نتيجو=BOCU1_TRAIL_منزل_BYTE(m);

            m=diff%BOCU1_TRAIL_ڳڻپ;
            diff/=BOCU1_TRAIL_ڳڻپ;
            نتيجو|=BOCU1_TRAIL_منزل_BYTE(m)<<8;

            /*
             * We know that / and % would deliver quotient 0 and rest=diff.
             * Avoid division and modulo for performance.
             */
            نتيجو|=BOCU1_TRAIL_منزل_BYTE(diff)<<16;

            نتيجو |= static_cast<uint32_t>(BOCU1_شروعات_POS_4) << 24;
        }
    } وإلا {
        /* two- to four-byte negative differences */
        إذا(diff>=BOCU1_پهچ_NEG_2) {
            /* two bytes */
            diff-=BOCU1_پهچ_NEG_1;
            نتيجو=0x02000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_ڳڻپ, m);
            نتيجو|=BOCU1_TRAIL_منزل_BYTE(m);

            نتيجو|=(BOCU1_شروعات_NEG_2+diff)<<8;
        } وإلا إذا(diff>=BOCU1_پهچ_NEG_3) {
            /* three bytes */
            diff-=BOCU1_پهچ_NEG_2;
            نتيجو=0x03000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_ڳڻپ, m);
            نتيجو|=BOCU1_TRAIL_منزل_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_ڳڻپ, m);
            نتيجو|=BOCU1_TRAIL_منزل_BYTE(m)<<8;

            نتيجو|=(BOCU1_شروعات_NEG_3+diff)<<16;
        } وإلا {
            /* four bytes */
            diff-=BOCU1_پهچ_NEG_3;

            NEGDIVMOD(diff, BOCU1_TRAIL_ڳڻپ, m);
            نتيجو=BOCU1_TRAIL_منزل_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_ڳڻپ, m);
            نتيجو|=BOCU1_TRAIL_منزل_BYTE(m)<<8;

            /*
             * We know that NEGDIVMOD would deliver
             * quotient -1 and rest=diff+BOCU1_TRAIL_COUNT.
             * Avoid division and modulo for performance.
             */
            m=diff+BOCU1_TRAIL_ڳڻپ;
            نتيجو|=BOCU1_TRAIL_منزل_BYTE(m)<<16;

            نتيجو|=BOCU1_MIN<<24;
        }
    }
    أرجع نتيجو;
}


ساكن فراغ U_CALLCONV
_Bocu1FromUnicodeWithOffsets(UConverter_From_Unicode_دليلون *p_دليلون,
                             UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت char16_t *ذريعو, *ذريعو_حد_بندي;
    uint8_t *هدف;
    int32_t هدف_صلاحيت;
    int32_t *offsets;

    int32_t prev, c, diff;

    int32_t ذريعو_اشاريو, ايندڙ_ذريعو_اشاريو;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    ذريعو=p_دليلون->ذريعو;
    ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
    هدف = reinterpret_cast<uint8_t*>(p_دليلون->هدف);
    هدف_صلاحيت = static_cast<int32_t>(p_دليلون->هدف_حد_بندي - p_دليلون->هدف);
    offsets=p_دليلون->offsets;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    prev = static_cast<int32_t>(cnv->from_Unicode_حالت);
    إذا(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ذريعو_اشاريو= c==0 ? 0 : -1;
    ايندڙ_ذريعو_اشاريو=0;

    /* conversion loop */
    إذا(c!=0 && هدف_صلاحيت>0) {
        اذهب getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(ذريعو_حد_بندي - ذريعو);
    إذا(هدف_صلاحيت>diff) {
        هدف_صلاحيت=diff;
    }
    طالما(هدف_صلاحيت>0 && (c=*ذريعو)<0x3000) {
        إذا(c<=0x20) {
            إذا(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *هدف++ = static_cast<uint8_t>(c);
            *offsets++=ايندڙ_ذريعو_اشاريو++;
            ++ذريعو;
            --هدف_صلاحيت;
        } وإلا {
            diff=c-prev;
            إذا(DIFF_IS_SINGLE(diff)) {
                prev=BOCU1_SIMPLE_PREV(c);
                *هدف++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=ايندڙ_ذريعو_اشاريو++;
                ++ذريعو;
                --هدف_صلاحيت;
            } وإلا {
                اكسر;
            }
        }
    }
    /* restore real values */
    هدف_صلاحيت = static_cast<int32_t>(reinterpret_cast<ثابت uint8_t*>(p_دليلون->هدف_حد_بندي) - هدف);
    ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو; /* wrong if offsets==nullptr but does not matter */

    /* regular loop for all cases */
    طالما(ذريعو<ذريعو_حد_بندي) {
        إذا(هدف_صلاحيت>0) {
            c=*ذريعو++;
            ++ايندڙ_ذريعو_اشاريو;

            إذا(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                إذا(c!=0x20) {
                    prev=BOCU1_ASCII_PREV;
                }
                *هدف++ = static_cast<uint8_t>(c);
                *offsets++=ذريعو_اشاريو;
                --هدف_صلاحيت;

                ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                استمر;
            }

            إذا(U16_IS_LEAD(c)) {
getTrail:
                إذا(ذريعو<ذريعو_حد_بندي) {
                    /* test the following code unit */
                    char16_t trail=*ذريعو;
                    إذا(U16_IS_TRAIL(trail)) {
                        ++ذريعو;
                        ++ايندڙ_ذريعو_اشاريو;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } وإلا {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    اكسر;
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
            إذا(DIFF_IS_SINGLE(diff)) {
                *هدف++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=ذريعو_اشاريو;
                --هدف_صلاحيت;
                ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                إذا(c<0x3000) {
                    اذهب fastSingle;
                }
            } وإلا إذا(DIFF_IS_DOUBLE(diff) && 2<=هدف_صلاحيت) {
                /* optimize 2-byte case */
                int32_t m;

                إذا(diff>=0) {
                    diff-=BOCU1_پهچ_POS_1+1;
                    m=diff%BOCU1_TRAIL_ڳڻپ;
                    diff/=BOCU1_TRAIL_ڳڻپ;
                    diff+=BOCU1_شروعات_POS_2;
                } وإلا {
                    diff-=BOCU1_پهچ_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_ڳڻپ, m);
                    diff+=BOCU1_شروعات_NEG_2;
                }
                *هدف++ = static_cast<uint8_t>(diff);
                *هدف++ = static_cast<uint8_t>(BOCU1_TRAIL_منزل_BYTE(m));
                *offsets++=ذريعو_اشاريو;
                *offsets++=ذريعو_اشاريو;
                هدف_صلاحيت-=2;
                ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
            } وإلا {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_ڊيگهائي_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                إذا(length<=هدف_صلاحيت) {
                    بدل(length) {
                        /* each branch falls through to the next one */
                    حالة 4:
                        *هدف++ = static_cast<uint8_t>(diff >> 24);
                        *offsets++=ذريعو_اشاريو;
                        U_FALLTHROUGH;
                    حالة 3:
                        *هدف++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=ذريعو_اشاريو;
                        U_FALLTHROUGH;
                    حالة 2:
                        *هدف++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=ذريعو_اشاريو;
                    /* case 1: handled above */
                        *هدف++ = static_cast<uint8_t>(diff);
                        *offsets++=ذريعو_اشاريو;
                        U_FALLTHROUGH;
                    افتراضي:
                        /* will never occur */
                        اكسر;
                    }
                    هدف_صلاحيت-=length;
                    ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                } وإلا {
                    uint8_t *char_Error_بفر;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=هدف_صلاحيت;
                    char_Error_بفر=(uint8_t *)cnv->char_Error_بفر;
                    بدل(length) {
                        /* each branch falls through to the next one */
                    حالة 3:
                        *char_Error_بفر++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    حالة 2:
                        *char_Error_بفر++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    حالة 1:
                        *char_Error_بفر = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    افتراضي:
                        /* will never occur */
                        اكسر;
                    }
                    cnv->char_Error_بفر_ڊيگهائي = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    بدل(هدف_صلاحيت) {
                        /* each branch falls through to the next one */
                    حالة 3:
                        *هدف++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=ذريعو_اشاريو;
                        U_FALLTHROUGH;
                    حالة 2:
                        *هدف++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=ذريعو_اشاريو;
                        U_FALLTHROUGH;
                    حالة 1:
                        *هدف++ = static_cast<uint8_t>(diff);
                        *offsets++=ذريعو_اشاريو;
                        U_FALLTHROUGH;
                    افتراضي:
                        /* will never occur */
                        اكسر;
                    }

                    /* target overflow */
                    هدف_صلاحيت=0;
                    *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                    اكسر;
                }
            }
        } وإلا {
            /* target is full */
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_حالت = static_cast<uint32_t>(prev);

    /* write back the updated pointers */
    p_دليلون->ذريعو=ذريعو;
    p_دليلون->هدف = reinterpret_cast<حرف*>(هدف);
    p_دليلون->offsets=offsets;
}

/*
 * Identical to _Bocu1FromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ساكن فراغ U_CALLCONV
_Bocu1FromUnicode(UConverter_From_Unicode_دليلون *p_دليلون,
                  UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت char16_t *ذريعو, *ذريعو_حد_بندي;
    uint8_t *هدف;
    int32_t هدف_صلاحيت;

    int32_t prev, c, diff;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    ذريعو=p_دليلون->ذريعو;
    ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
    هدف = reinterpret_cast<uint8_t*>(p_دليلون->هدف);
    هدف_صلاحيت = static_cast<int32_t>(p_دليلون->هدف_حد_بندي - p_دليلون->هدف);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    prev = static_cast<int32_t>(cnv->from_Unicode_حالت);
    إذا(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }

    /* conversion loop */
    إذا(c!=0 && هدف_صلاحيت>0) {
        اذهب getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(ذريعو_حد_بندي - ذريعو);
    إذا(هدف_صلاحيت>diff) {
        هدف_صلاحيت=diff;
    }
    طالما(هدف_صلاحيت>0 && (c=*ذريعو)<0x3000) {
        إذا(c<=0x20) {
            إذا(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *هدف++ = static_cast<uint8_t>(c);
        } وإلا {
            diff=c-prev;
            إذا(DIFF_IS_SINGLE(diff)) {
                prev=BOCU1_SIMPLE_PREV(c);
                *هدف++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
            } وإلا {
                اكسر;
            }
        }
        ++ذريعو;
        --هدف_صلاحيت;
    }
    /* restore real values */
    هدف_صلاحيت = static_cast<int32_t>(reinterpret_cast<ثابت uint8_t*>(p_دليلون->هدف_حد_بندي) - هدف);

    /* regular loop for all cases */
    طالما(ذريعو<ذريعو_حد_بندي) {
        إذا(هدف_صلاحيت>0) {
            c=*ذريعو++;

            إذا(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                إذا(c!=0x20) {
                    prev=BOCU1_ASCII_PREV;
                }
                *هدف++ = static_cast<uint8_t>(c);
                --هدف_صلاحيت;
                استمر;
            }

            إذا(U16_IS_LEAD(c)) {
getTrail:
                إذا(ذريعو<ذريعو_حد_بندي) {
                    /* test the following code unit */
                    char16_t trail=*ذريعو;
                    إذا(U16_IS_TRAIL(trail)) {
                        ++ذريعو;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } وإلا {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    اكسر;
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
            إذا(DIFF_IS_SINGLE(diff)) {
                *هدف++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                --هدف_صلاحيت;
                إذا(c<0x3000) {
                    اذهب fastSingle;
                }
            } وإلا إذا(DIFF_IS_DOUBLE(diff) && 2<=هدف_صلاحيت) {
                /* optimize 2-byte case */
                int32_t m;

                إذا(diff>=0) {
                    diff-=BOCU1_پهچ_POS_1+1;
                    m=diff%BOCU1_TRAIL_ڳڻپ;
                    diff/=BOCU1_TRAIL_ڳڻپ;
                    diff+=BOCU1_شروعات_POS_2;
                } وإلا {
                    diff-=BOCU1_پهچ_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_ڳڻپ, m);
                    diff+=BOCU1_شروعات_NEG_2;
                }
                *هدف++ = static_cast<uint8_t>(diff);
                *هدف++ = static_cast<uint8_t>(BOCU1_TRAIL_منزل_BYTE(m));
                هدف_صلاحيت-=2;
            } وإلا {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_ڊيگهائي_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                إذا(length<=هدف_صلاحيت) {
                    بدل(length) {
                        /* each branch falls through to the next one */
                    حالة 4:
                        *هدف++ = static_cast<uint8_t>(diff >> 24);
                        U_FALLTHROUGH;
                    حالة 3:
                        *هدف++ = static_cast<uint8_t>(diff >> 16);
                    /* case 2: handled above */
                        *هدف++ = static_cast<uint8_t>(diff >> 8);
                    /* case 1: handled above */
                        *هدف++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    افتراضي:
                        /* will never occur */
                        اكسر;
                    }
                    هدف_صلاحيت-=length;
                } وإلا {
                    uint8_t *char_Error_بفر;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=هدف_صلاحيت;
                    char_Error_بفر=(uint8_t *)cnv->char_Error_بفر;
                    بدل(length) {
                        /* each branch falls through to the next one */
                    حالة 3:
                        *char_Error_بفر++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    حالة 2:
                        *char_Error_بفر++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    حالة 1:
                        *char_Error_بفر = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    افتراضي:
                        /* will never occur */
                        اكسر;
                    }
                    cnv->char_Error_بفر_ڊيگهائي = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    بدل(هدف_صلاحيت) {
                        /* each branch falls through to the next one */
                    حالة 3:
                        *هدف++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    حالة 2:
                        *هدف++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    حالة 1:
                        *هدف++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    افتراضي:
                        /* will never occur */
                        اكسر;
                    }

                    /* target overflow */
                    هدف_صلاحيت=0;
                    *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                    اكسر;
                }
            }
        } وإلا {
            /* target is full */
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_حالت = static_cast<uint32_t>(prev);

    /* write back the updated pointers */
    p_دليلون->ذريعو=ذريعو;
    p_دليلون->هدف = reinterpret_cast<حرف*>(هدف);
}

/* BOCU-1-to-Unicode conversion functions ----------------------------------- */

/**
 * Function for BOCU-1 decoder; handles multi-byte lead bytes.
 *
 * @param b lead byte;
 *          BOCU1_MIN<=b<BOCU1_START_NEG_2 or BOCU1_START_POS_2<=b<BOCU1_MAX_LEAD
 * @return (diff<<2)|count
 */
ساكن مضمن int32_t
decodeBocu1LeadByte(int32_t b) {
    int32_t diff, count;

    إذا(b>=BOCU1_شروعات_NEG_2) {
        /* positive difference */
        إذا(b<BOCU1_شروعات_POS_3) {
            /* two bytes */
            diff = (b - BOCU1_شروعات_POS_2) * BOCU1_TRAIL_ڳڻپ + BOCU1_پهچ_POS_1 + 1;
            count=1;
        } وإلا إذا(b<BOCU1_شروعات_POS_4) {
            /* three bytes */
            diff = (b - BOCU1_شروعات_POS_3) * BOCU1_TRAIL_ڳڻپ * BOCU1_TRAIL_ڳڻپ + BOCU1_پهچ_POS_2 + 1;
            count=2;
        } وإلا {
            /* four bytes */
            diff=BOCU1_پهچ_POS_3+1;
            count=3;
        }
    } وإلا {
        /* negative difference */
        إذا(b>=BOCU1_شروعات_NEG_3) {
            /* two bytes */
            diff = (b - BOCU1_شروعات_NEG_2) * BOCU1_TRAIL_ڳڻپ + BOCU1_پهچ_NEG_1;
            count=1;
        } وإلا إذا(b>BOCU1_MIN) {
            /* three bytes */
            diff = (b - BOCU1_شروعات_NEG_3) * BOCU1_TRAIL_ڳڻپ * BOCU1_TRAIL_ڳڻپ + BOCU1_پهچ_NEG_2;
            count=2;
        } وإلا {
            /* four bytes */
            diff=-BOCU1_TRAIL_ڳڻپ*BOCU1_TRAIL_ڳڻپ*BOCU1_TRAIL_ڳڻپ+BOCU1_پهچ_NEG_3;
            count=3;
        }
    }

    /* return the state for decoding the trail byte(s) */
    أرجع (static_cast<uint32_t>(diff) << 2) | count;
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
ساكن مضمن int32_t
decodeBocu1TrailByte(int32_t count, int32_t b) {
    إذا(b<=0x20) {
        /* skip some C0 controls and make the trail byte range contiguous */
        b=bocu1_Byte_منزل_Trail[b];
        /* b<0 for an illegal trail byte value will result in return<0 below */
#إذا BOCU1_MAX_TRAIL<0xff
    } وإلا إذا(b>BOCU1_MAX_TRAIL) {
        أرجع -99;
#جيڪڏهن_پورو
    } وإلا {
        b-=BOCU1_TRAIL_BYTE_وٿي;
    }

    /* add trail byte into difference and decrement count */
    إذا(count==1) {
        أرجع b;
    } وإلا إذا(count==2) {
        أرجع b*BOCU1_TRAIL_ڳڻپ;
    } وإلا /* count==3 */ {
        أرجع b*(BOCU1_TRAIL_ڳڻپ*BOCU1_TRAIL_ڳڻپ);
    }
}

ساكن فراغ U_CALLCONV
Bocu1_منزل_Unicode_With_Offsets(UConverter_منزل_Unicode_دليلون *p_دليلون,
                           UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي;
    char16_t *هدف;
    ثابت char16_t *هدف_حد_بندي;
    int32_t *offsets;

    int32_t prev, count, diff, c;

    int8_t byte_اشاريو;
    uint8_t *بائيٽون;

    int32_t ذريعو_اشاريو, ايندڙ_ذريعو_اشاريو;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    ذريعو = reinterpret_cast<ثابت uint8_t*>(p_دليلون->ذريعو);
    ذريعو_حد_بندي = reinterpret_cast<ثابت uint8_t*>(p_دليلون->ذريعو_حد_بندي);
    هدف=p_دليلون->هدف;
    هدف_حد_بندي=p_دليلون->هدف_حد_بندي;
    offsets=p_دليلون->offsets;

    /* get the converter state from UConverter */
    prev = static_cast<int32_t>(cnv->منزل_Unicode_حالت);
    إذا(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }
    diff=cnv->گهڻائي; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_اشاريو=cnv->منزل_ULength;
    بائيٽون=cnv->منزل_UBytes;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ذريعو_اشاريو=byte_اشاريو==0 ? 0 : -1;
    ايندڙ_ذريعو_اشاريو=0;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    إذا(count>0 && byte_اشاريو>0 && هدف<هدف_حد_بندي) {
        اذهب getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(ذريعو_حد_بندي - ذريعو);
    count = static_cast<int32_t>(p_دليلون->هدف_حد_بندي - هدف);
    إذا(count>diff) {
        count=diff;
    }
    طالما(count>0) {
        إذا(BOCU1_شروعات_NEG_2<=(c=*ذريعو) && c<BOCU1_شروعات_POS_2) {
            c=prev+(c-BOCU1_وچ);
            إذا(c<0x3000) {
                *هدف++ = static_cast<char16_t>(c);
                *offsets++=ايندڙ_ذريعو_اشاريو++;
                prev=BOCU1_SIMPLE_PREV(c);
            } وإلا {
                اكسر;
            }
        } وإلا إذا(c<=0x20) {
            إذا(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *هدف++ = static_cast<char16_t>(c);
            *offsets++=ايندڙ_ذريعو_اشاريو++;
        } وإلا {
            اكسر;
        }
        ++ذريعو;
        --count;
    }
    ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو; /* wrong if offsets==nullptr but does not matter */

    /* decode a sequence of single and lead bytes */
    طالما(ذريعو<ذريعو_حد_بندي) {
        إذا(هدف>=هدف_حد_بندي) {
            /* target is full */
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }

        ++ايندڙ_ذريعو_اشاريو;
        c=*ذريعو++;
        إذا(BOCU1_شروعات_NEG_2<=c && c<BOCU1_شروعات_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=prev+(c-BOCU1_وچ);
            إذا(c<0x3000) {
                *هدف++ = static_cast<char16_t>(c);
                *offsets++=ذريعو_اشاريو;
                prev=BOCU1_SIMPLE_PREV(c);
                ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                اذهب fastSingle;
            }
        } وإلا إذا(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            إذا(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *هدف++ = static_cast<char16_t>(c);
            *offsets++=ذريعو_اشاريو;
            ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
            استمر;
        } وإلا إذا(BOCU1_شروعات_NEG_3<=c && c<BOCU1_شروعات_POS_3 && ذريعو<ذريعو_حد_بندي) {
            /* Optimize two-byte case. */
            إذا(c>=BOCU1_وچ) {
                diff = (c - BOCU1_شروعات_POS_2) * BOCU1_TRAIL_ڳڻپ + BOCU1_پهچ_POS_1 + 1;
            } وإلا {
                diff = (c - BOCU1_شروعات_NEG_2) * BOCU1_TRAIL_ڳڻپ + BOCU1_پهچ_NEG_1;
            }

            /* trail byte */
            ++ايندڙ_ذريعو_اشاريو;
            c=decodeBocu1TrailByte(1, *ذريعو++);
            إذا (c < 0 || static_cast<uint32_t>(c = prev + diff + c) > 0x10ffff) {
                بائيٽون[0]=ذريعو[-2];
                بائيٽون[1]=ذريعو[-1];
                byte_اشاريو=2;
                *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                اكسر;
            }
        } وإلا إذا(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            prev=BOCU1_ASCII_PREV;
            ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
            استمر;
        } وإلا {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            بائيٽون[0] = static_cast<uint8_t>(c);
            byte_اشاريو=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            لكل(;;) {
                إذا(ذريعو>=ذريعو_حد_بندي) {
                    اذهب endloop;
                }
                ++ايندڙ_ذريعو_اشاريو;
                c=بائيٽون[byte_اشاريو++]=*ذريعو++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                إذا(c<0) {
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                    اذهب endloop;
                }

                diff+=c;
                إذا(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_اشاريو=0;
                    c=prev+diff;
                    إذا (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                        اذهب endloop;
                    }
                    اكسر;
                }
            }
        }

        /* calculate the next prev and output c */
        prev=BOCU1_PREV(c);
        إذا(c<=0xffff) {
            *هدف++ = static_cast<char16_t>(c);
            *offsets++=ذريعو_اشاريو;
        } وإلا {
            /* output surrogate pair */
            *هدف++=U16_LEAD(c);
            إذا(هدف<هدف_حد_بندي) {
                *هدف++=U16_TRAIL(c);
                *offsets++=ذريعو_اشاريو;
                *offsets++=ذريعو_اشاريو;
            } وإلا {
                /* target overflow */
                *offsets++=ذريعو_اشاريو;
                cnv->UChar_Error_بفر[0]=U16_TRAIL(c);
                cnv->UChar_Error_بفر_ڊيگهائي=1;
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                اكسر;
            }
        }
        ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
    }
endloop:

    إذا(*p_Error_ڪوڊ==U_ILLEGAL_CHAR_مليو) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->منزل_Unicode_حالت=BOCU1_ASCII_PREV;
        cnv->گهڻائي=0;
    } وإلا {
        /* set the converter state back into UConverter */
        cnv->منزل_Unicode_حالت = static_cast<uint32_t>(prev);
        cnv->گهڻائي = static_cast<int32_t>(static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->منزل_ULength=byte_اشاريو;

    /* write back the updated pointers */
    p_دليلون->ذريعو = reinterpret_cast<ثابت حرف*>(ذريعو);
    p_دليلون->هدف=هدف;
    p_دليلون->offsets=offsets;
}

/*
 * Identical to _Bocu1ToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ساكن فراغ U_CALLCONV
Bocu1_منزل_Unicode(UConverter_منزل_Unicode_دليلون *p_دليلون,
                UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي;
    char16_t *هدف;
    ثابت char16_t *هدف_حد_بندي;

    int32_t prev, count, diff, c;

    int8_t byte_اشاريو;
    uint8_t *بائيٽون;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    ذريعو = reinterpret_cast<ثابت uint8_t*>(p_دليلون->ذريعو);
    ذريعو_حد_بندي = reinterpret_cast<ثابت uint8_t*>(p_دليلون->ذريعو_حد_بندي);
    هدف=p_دليلون->هدف;
    هدف_حد_بندي=p_دليلون->هدف_حد_بندي;

    /* get the converter state from UConverter */
    prev = static_cast<int32_t>(cnv->منزل_Unicode_حالت);
    إذا(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }
    diff=cnv->گهڻائي; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_اشاريو=cnv->منزل_ULength;
    بائيٽون=cnv->منزل_UBytes;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    إذا(count>0 && byte_اشاريو>0 && هدف<هدف_حد_بندي) {
        اذهب getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(ذريعو_حد_بندي - ذريعو);
    count = static_cast<int32_t>(p_دليلون->هدف_حد_بندي - هدف);
    إذا(count>diff) {
        count=diff;
    }
    طالما(count>0) {
        إذا(BOCU1_شروعات_NEG_2<=(c=*ذريعو) && c<BOCU1_شروعات_POS_2) {
            c=prev+(c-BOCU1_وچ);
            إذا(c<0x3000) {
                *هدف++ = static_cast<char16_t>(c);
                prev=BOCU1_SIMPLE_PREV(c);
            } وإلا {
                اكسر;
            }
        } وإلا إذا(c<=0x20) {
            إذا(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *هدف++ = static_cast<char16_t>(c);
        } وإلا {
            اكسر;
        }
        ++ذريعو;
        --count;
    }

    /* decode a sequence of single and lead bytes */
    طالما(ذريعو<ذريعو_حد_بندي) {
        إذا(هدف>=هدف_حد_بندي) {
            /* target is full */
            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }

        c=*ذريعو++;
        إذا(BOCU1_شروعات_NEG_2<=c && c<BOCU1_شروعات_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=prev+(c-BOCU1_وچ);
            إذا(c<0x3000) {
                *هدف++ = static_cast<char16_t>(c);
                prev=BOCU1_SIMPLE_PREV(c);
                اذهب fastSingle;
            }
        } وإلا إذا(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            إذا(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *هدف++ = static_cast<char16_t>(c);
            استمر;
        } وإلا إذا(BOCU1_شروعات_NEG_3<=c && c<BOCU1_شروعات_POS_3 && ذريعو<ذريعو_حد_بندي) {
            /* Optimize two-byte case. */
            إذا(c>=BOCU1_وچ) {
                diff = (c - BOCU1_شروعات_POS_2) * BOCU1_TRAIL_ڳڻپ + BOCU1_پهچ_POS_1 + 1;
            } وإلا {
                diff = (c - BOCU1_شروعات_NEG_2) * BOCU1_TRAIL_ڳڻپ + BOCU1_پهچ_NEG_1;
            }

            /* trail byte */
            c=decodeBocu1TrailByte(1, *ذريعو++);
            إذا (c < 0 || static_cast<uint32_t>(c = prev + diff + c) > 0x10ffff) {
                بائيٽون[0]=ذريعو[-2];
                بائيٽون[1]=ذريعو[-1];
                byte_اشاريو=2;
                *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                اكسر;
            }
        } وإلا إذا(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            prev=BOCU1_ASCII_PREV;
            استمر;
        } وإلا {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            بائيٽون[0] = static_cast<uint8_t>(c);
            byte_اشاريو=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            لكل(;;) {
                إذا(ذريعو>=ذريعو_حد_بندي) {
                    اذهب endloop;
                }
                c=بائيٽون[byte_اشاريو++]=*ذريعو++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                إذا(c<0) {
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                    اذهب endloop;
                }

                diff+=c;
                إذا(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_اشاريو=0;
                    c=prev+diff;
                    إذا (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                        اذهب endloop;
                    }
                    اكسر;
                }
            }
        }

        /* calculate the next prev and output c */
        prev=BOCU1_PREV(c);
        إذا(c<=0xffff) {
            *هدف++ = static_cast<char16_t>(c);
        } وإلا {
            /* output surrogate pair */
            *هدف++=U16_LEAD(c);
            إذا(هدف<هدف_حد_بندي) {
                *هدف++=U16_TRAIL(c);
            } وإلا {
                /* target overflow */
                cnv->UChar_Error_بفر[0]=U16_TRAIL(c);
                cnv->UChar_Error_بفر_ڊيگهائي=1;
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                اكسر;
            }
        }
    }
endloop:

    إذا(*p_Error_ڪوڊ==U_ILLEGAL_CHAR_مليو) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->منزل_Unicode_حالت=BOCU1_ASCII_PREV;
        cnv->گهڻائي=0;
    } وإلا {
        /* set the converter state back into UConverter */
        cnv->منزل_Unicode_حالت = static_cast<uint32_t>(prev);
        cnv->گهڻائي = (static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->منزل_ULength=byte_اشاريو;

    /* write back the updated pointers */
    p_دليلون->ذريعو = reinterpret_cast<ثابت حرف*>(ذريعو);
    p_دليلون->هدف=هدف;
}

/* miscellaneous ------------------------------------------------------------ */

ساكن ثابت UConverterImpl _Bocu1Impl={
    UCNV_BOCU1,

    خالي_اشارو,
    خالي_اشارو,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,

    Bocu1_منزل_Unicode,
    Bocu1_منزل_Unicode_With_Offsets,
    _Bocu1FromUnicode,
    _Bocu1FromUnicodeWithOffsets,
    خالي_اشارو,

    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    خالي_اشارو,
    ucnv_get_Complete_Unicode_سيٽ,

    خالي_اشارو,
    خالي_اشارو
};

ساكن ثابت UConverter_Static_معلومات Bocu1_Static_معلومات={
    حجم(UConverter_Static_معلومات),
    "BOCU-1",
    1214, /* CCSID for BOCU-1 */
    UCNV_IBM, UCNV_BOCU1,
    1, 4, /* one char16_t generates at least 1 byte and at most 4 bytes */
    { 0x1a, 0, 0, 0 }, 1, /* BOCU-1 never needs to write a subchar */
    خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_معلومات Bocu1_معلومات=
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&Bocu1_Static_معلومات, &_Bocu1Impl);

#جيڪڏهن_پورو
