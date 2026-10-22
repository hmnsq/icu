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
#تعریف_کن BOCU1_ASCII_PREV        0x40

/* bounding byte values for differences */
#تعریف_کن BOCU1_MIN               0x21
#تعریف_کن BOCU1_میانه            0x90
#تعریف_کن BOCU1_MAX_LEAD          0xfe
#تعریف_کن BOCU1_MAX_TRAIL         0xff
#تعریف_کن BOCU1_RESET             0xff

/* number of lead bytes */
#تعریف_کن BOCU1_شمار             (BOCU1_MAX_LEAD-BOCU1_MIN+1)

/* adjust trail byte counts for the use of some C0 control byte values */
#تعریف_کن BOCU1_TRAIL_CONTROLS_شمار  20
#تعریف_کن BOCU1_TRAIL_BYTE_جابه_جایی     (BOCU1_MIN-BOCU1_TRAIL_CONTROLS_شمار)

/* number of trail bytes */
#تعریف_کن BOCU1_TRAIL_شمار       ((BOCU1_MAX_TRAIL-BOCU1_MIN+1)+BOCU1_TRAIL_CONTROLS_شمار)

/*
 * number of positive and negative single-byte codes
 * (counting 0==BOCU1_MIDDLE among the positive ones)
 */
#تعریف_کن BOCU1_SINGLE            64

/* number of lead bytes for positive and negative 2/3/4-byte sequences */
#تعریف_کن BOCU1_LEAD_2            43
#تعریف_کن BOCU1_LEAD_3            3
#تعریف_کن BOCU1_LEAD_4            1

/* The difference value range for single-byters. */
#تعریف_کن BOCU1_دسترسی_POS_1   (BOCU1_SINGLE-1)
#تعریف_کن BOCU1_دسترسی_NEG_1   (-BOCU1_SINGLE)

/* The difference value range for double-byters. */
#تعریف_کن BOCU1_دسترسی_POS_2   (BOCU1_دسترسی_POS_1+BOCU1_LEAD_2*BOCU1_TRAIL_شمار)
#تعریف_کن BOCU1_دسترسی_NEG_2   (BOCU1_دسترسی_NEG_1-BOCU1_LEAD_2*BOCU1_TRAIL_شمار)

/* The difference value range for 3-byters. */
#تعریف_کن BOCU1_دسترسی_POS_3   \
    (BOCU1_دسترسی_POS_2+BOCU1_LEAD_3*BOCU1_TRAIL_شمار*BOCU1_TRAIL_شمار)

#تعریف_کن BOCU1_دسترسی_NEG_3   (BOCU1_دسترسی_NEG_2-BOCU1_LEAD_3*BOCU1_TRAIL_شمار*BOCU1_TRAIL_شمار)

/* The lead byte start values. */
#تعریف_کن BOCU1_شروع_POS_2   (BOCU1_میانه+BOCU1_دسترسی_POS_1+1)
#تعریف_کن BOCU1_شروع_POS_3   (BOCU1_شروع_POS_2+BOCU1_LEAD_2)
#تعریف_کن BOCU1_شروع_POS_4   (BOCU1_شروع_POS_3+BOCU1_LEAD_3)
     /* ==BOCU1_MAX_LEAD */

#تعریف_کن BOCU1_شروع_NEG_2   (BOCU1_میانه+BOCU1_دسترسی_NEG_1)
#تعریف_کن BOCU1_شروع_NEG_3   (BOCU1_شروع_NEG_2-BOCU1_LEAD_2)
#تعریف_کن BOCU1_شروع_NEG_4   (BOCU1_شروع_NEG_3-BOCU1_LEAD_3)
     /* ==BOCU1_MIN+1 */

/* The length of a byte sequence, according to the lead byte (!=BOCU1_RESET). */
#تعریف_کن BOCU1_درازی_FROM_LEAD(lead) \
    ((BOCU1_شروع_NEG_2<=(lead) && (lead)<BOCU1_شروع_POS_2) ? 1 : \
     (BOCU1_شروع_NEG_3<=(lead) && (lead)<BOCU1_شروع_POS_3) ? 2 : \
     (BOCU1_شروع_NEG_4<=(lead) && (lead)<BOCU1_شروع_POS_4) ? 3 : 4)

/* The length of a byte sequence, according to its packed form. */
#تعریف_کن BOCU1_درازی_FROM_PACKED(packed) \
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
#تعریف_کن BOCU1_TRAIL_مقصد_BYTE(t) ((t)>=BOCU1_TRAIL_CONTROLS_شمار ? (t)+BOCU1_TRAIL_BYTE_جابه_جایی : bocu1_Trail_مقصد_Byte[t])

/*
 * Byte value map for control codes,
 * from external byte values 0x00..0x20
 * to trail byte values 0..19 (0..0x13) as used in the difference calculation.
 * External byte values that are illegal as trail bytes are mapped to -1.
 */
ساكن ثابت int8_t
bocu1_Byte_مقصد_Trail[BOCU1_MIN]={
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
bocu1_Trail_مقصد_Byte[BOCU1_TRAIL_CONTROLS_شمار]={
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
#تعریف_کن NEGDIVMOD(n, d, m) UPRV_بلوک_MACRO_BEGIN { \
    (m)=(n)%(d); \
    (n)/=(d); \
    إذا((m)<0) { \
        --(n); \
        (m)+=(d); \
    } \
} UPRV_بلوک_MACRO_انتها

/* Faster versions of packDiff() for single-byte-encoded diff values. */

/** Is a diff value encodable in a single byte? */
#تعریف_کن DIFF_IS_SINGLE(diff) (BOCU1_دسترسی_NEG_1<=(diff) && (diff)<=BOCU1_دسترسی_POS_1)

/** Encode a diff value in a single byte. */
#تعریف_کن PACK_SINGLE_DIFF(diff) (BOCU1_میانه+(diff))

/** Is a diff value encodable in two bytes? */
#تعریف_کن DIFF_IS_DOUBLE(diff) (BOCU1_دسترسی_NEG_2<=(diff) && (diff)<=BOCU1_دسترسی_POS_2)

/* BOCU-1 implementation functions ------------------------------------------ */

#تعریف_کن BOCU1_SIMPLE_PREV(c) (((c)&~0x7f)+BOCU1_ASCII_PREV)

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
        أرجع 0x4e00-BOCU1_دسترسی_NEG_2;
    } وإلا إذا(0xac00<=c /* && c<=0xd7a3 */) {
        /* Korean Hangul */
        أرجع (0xd7a3+0xac00)/2;
    } وإلا {
        /* mostly small scripts */
        أرجع BOCU1_SIMPLE_PREV(c);
    }
}

/** Fast version of bocu1Prev() for most scripts. */
#تعریف_کن BOCU1_PREV(c) ((c)<0x3040 || (c)>0xd7a3 ? BOCU1_SIMPLE_PREV(c) : bocu1Prev(c))

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
    int32_t نتیجه, m;

    U_ASSERT(!DIFF_IS_SINGLE(diff)); /* assume we won't be called where diff==BOCU1_REACH_NEG_1=-64 */
    إذا(diff>=BOCU1_دسترسی_NEG_1) {
        /* mostly positive differences, and single-byte negative ones */
#إذا 0   /* single-byte case handled in macros, see below */
        إذا(diff<=BOCU1_دسترسی_POS_1) {
            /* single byte */
            أرجع 0x01000000|(BOCU1_میانه+diff);
        } وإلا
#پایان_اگر
        إذا(diff<=BOCU1_دسترسی_POS_2) {
            /* two bytes */
            diff-=BOCU1_دسترسی_POS_1+1;
            نتیجه=0x02000000;

            m=diff%BOCU1_TRAIL_شمار;
            diff/=BOCU1_TRAIL_شمار;
            نتیجه|=BOCU1_TRAIL_مقصد_BYTE(m);

            نتیجه|=(BOCU1_شروع_POS_2+diff)<<8;
        } وإلا إذا(diff<=BOCU1_دسترسی_POS_3) {
            /* three bytes */
            diff-=BOCU1_دسترسی_POS_2+1;
            نتیجه=0x03000000;

            m=diff%BOCU1_TRAIL_شمار;
            diff/=BOCU1_TRAIL_شمار;
            نتیجه|=BOCU1_TRAIL_مقصد_BYTE(m);

            m=diff%BOCU1_TRAIL_شمار;
            diff/=BOCU1_TRAIL_شمار;
            نتیجه|=BOCU1_TRAIL_مقصد_BYTE(m)<<8;

            نتیجه|=(BOCU1_شروع_POS_3+diff)<<16;
        } وإلا {
            /* four bytes */
            diff-=BOCU1_دسترسی_POS_3+1;

            m=diff%BOCU1_TRAIL_شمار;
            diff/=BOCU1_TRAIL_شمار;
            نتیجه=BOCU1_TRAIL_مقصد_BYTE(m);

            m=diff%BOCU1_TRAIL_شمار;
            diff/=BOCU1_TRAIL_شمار;
            نتیجه|=BOCU1_TRAIL_مقصد_BYTE(m)<<8;

            /*
             * We know that / and % would deliver quotient 0 and rest=diff.
             * Avoid division and modulo for performance.
             */
            نتیجه|=BOCU1_TRAIL_مقصد_BYTE(diff)<<16;

            نتیجه |= static_cast<uint32_t>(BOCU1_شروع_POS_4) << 24;
        }
    } وإلا {
        /* two- to four-byte negative differences */
        إذا(diff>=BOCU1_دسترسی_NEG_2) {
            /* two bytes */
            diff-=BOCU1_دسترسی_NEG_1;
            نتیجه=0x02000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_شمار, m);
            نتیجه|=BOCU1_TRAIL_مقصد_BYTE(m);

            نتیجه|=(BOCU1_شروع_NEG_2+diff)<<8;
        } وإلا إذا(diff>=BOCU1_دسترسی_NEG_3) {
            /* three bytes */
            diff-=BOCU1_دسترسی_NEG_2;
            نتیجه=0x03000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_شمار, m);
            نتیجه|=BOCU1_TRAIL_مقصد_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_شمار, m);
            نتیجه|=BOCU1_TRAIL_مقصد_BYTE(m)<<8;

            نتیجه|=(BOCU1_شروع_NEG_3+diff)<<16;
        } وإلا {
            /* four bytes */
            diff-=BOCU1_دسترسی_NEG_3;

            NEGDIVMOD(diff, BOCU1_TRAIL_شمار, m);
            نتیجه=BOCU1_TRAIL_مقصد_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_شمار, m);
            نتیجه|=BOCU1_TRAIL_مقصد_BYTE(m)<<8;

            /*
             * We know that NEGDIVMOD would deliver
             * quotient -1 and rest=diff+BOCU1_TRAIL_COUNT.
             * Avoid division and modulo for performance.
             */
            m=diff+BOCU1_TRAIL_شمار;
            نتیجه|=BOCU1_TRAIL_مقصد_BYTE(m)<<16;

            نتیجه|=BOCU1_MIN<<24;
        }
    }
    أرجع نتیجه;
}


ساكن فراغ U_CALLCONV
_Bocu1FromUnicodeWithOffsets(UConverter_From_Unicode_آرگومانها *p_آرگومانها,
                             UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت char16_t *منبع, *منبع_حد;
    uint8_t *هدف;
    int32_t هدف_ظرفیت;
    int32_t *offsets;

    int32_t prev, c, diff;

    int32_t منبع_شاخص, بعدی_منبع_شاخص;

    /* set up the local pointers */
    cnv=p_آرگومانها->converter;
    منبع=p_آرگومانها->منبع;
    منبع_حد=p_آرگومانها->منبع_حد;
    هدف = reinterpret_cast<uint8_t*>(p_آرگومانها->هدف);
    هدف_ظرفیت = static_cast<int32_t>(p_آرگومانها->هدف_حد - p_آرگومانها->هدف);
    offsets=p_آرگومانها->offsets;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    prev = static_cast<int32_t>(cnv->from_Unicode_وضعیت);
    إذا(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    منبع_شاخص= c==0 ? 0 : -1;
    بعدی_منبع_شاخص=0;

    /* conversion loop */
    إذا(c!=0 && هدف_ظرفیت>0) {
        اذهب getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(منبع_حد - منبع);
    إذا(هدف_ظرفیت>diff) {
        هدف_ظرفیت=diff;
    }
    طالما(هدف_ظرفیت>0 && (c=*منبع)<0x3000) {
        إذا(c<=0x20) {
            إذا(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *هدف++ = static_cast<uint8_t>(c);
            *offsets++=بعدی_منبع_شاخص++;
            ++منبع;
            --هدف_ظرفیت;
        } وإلا {
            diff=c-prev;
            إذا(DIFF_IS_SINGLE(diff)) {
                prev=BOCU1_SIMPLE_PREV(c);
                *هدف++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=بعدی_منبع_شاخص++;
                ++منبع;
                --هدف_ظرفیت;
            } وإلا {
                اكسر;
            }
        }
    }
    /* restore real values */
    هدف_ظرفیت = static_cast<int32_t>(reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->هدف_حد) - هدف);
    منبع_شاخص=بعدی_منبع_شاخص; /* wrong if offsets==nullptr but does not matter */

    /* regular loop for all cases */
    طالما(منبع<منبع_حد) {
        إذا(هدف_ظرفیت>0) {
            c=*منبع++;
            ++بعدی_منبع_شاخص;

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
                *offsets++=منبع_شاخص;
                --هدف_ظرفیت;

                منبع_شاخص=بعدی_منبع_شاخص;
                استمر;
            }

            إذا(U16_IS_LEAD(c)) {
getTrail:
                إذا(منبع<منبع_حد) {
                    /* test the following code unit */
                    char16_t trail=*منبع;
                    إذا(U16_IS_TRAIL(trail)) {
                        ++منبع;
                        ++بعدی_منبع_شاخص;
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
                *offsets++=منبع_شاخص;
                --هدف_ظرفیت;
                منبع_شاخص=بعدی_منبع_شاخص;
                إذا(c<0x3000) {
                    اذهب fastSingle;
                }
            } وإلا إذا(DIFF_IS_DOUBLE(diff) && 2<=هدف_ظرفیت) {
                /* optimize 2-byte case */
                int32_t m;

                إذا(diff>=0) {
                    diff-=BOCU1_دسترسی_POS_1+1;
                    m=diff%BOCU1_TRAIL_شمار;
                    diff/=BOCU1_TRAIL_شمار;
                    diff+=BOCU1_شروع_POS_2;
                } وإلا {
                    diff-=BOCU1_دسترسی_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_شمار, m);
                    diff+=BOCU1_شروع_NEG_2;
                }
                *هدف++ = static_cast<uint8_t>(diff);
                *هدف++ = static_cast<uint8_t>(BOCU1_TRAIL_مقصد_BYTE(m));
                *offsets++=منبع_شاخص;
                *offsets++=منبع_شاخص;
                هدف_ظرفیت-=2;
                منبع_شاخص=بعدی_منبع_شاخص;
            } وإلا {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_درازی_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                إذا(length<=هدف_ظرفیت) {
                    بدل(length) {
                        /* each branch falls through to the next one */
                    حالة 4:
                        *هدف++ = static_cast<uint8_t>(diff >> 24);
                        *offsets++=منبع_شاخص;
                        U_FALLTHROUGH;
                    حالة 3:
                        *هدف++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=منبع_شاخص;
                        U_FALLTHROUGH;
                    حالة 2:
                        *هدف++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=منبع_شاخص;
                    /* case 1: handled above */
                        *هدف++ = static_cast<uint8_t>(diff);
                        *offsets++=منبع_شاخص;
                        U_FALLTHROUGH;
                    افتراضي:
                        /* will never occur */
                        اكسر;
                    }
                    هدف_ظرفیت-=length;
                    منبع_شاخص=بعدی_منبع_شاخص;
                } وإلا {
                    uint8_t *char_Error_میانگیر;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=هدف_ظرفیت;
                    char_Error_میانگیر=(uint8_t *)cnv->char_Error_میانگیر;
                    بدل(length) {
                        /* each branch falls through to the next one */
                    حالة 3:
                        *char_Error_میانگیر++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    حالة 2:
                        *char_Error_میانگیر++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    حالة 1:
                        *char_Error_میانگیر = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    افتراضي:
                        /* will never occur */
                        اكسر;
                    }
                    cnv->char_Error_میانگیر_درازی = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    بدل(هدف_ظرفیت) {
                        /* each branch falls through to the next one */
                    حالة 3:
                        *هدف++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=منبع_شاخص;
                        U_FALLTHROUGH;
                    حالة 2:
                        *هدف++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=منبع_شاخص;
                        U_FALLTHROUGH;
                    حالة 1:
                        *هدف++ = static_cast<uint8_t>(diff);
                        *offsets++=منبع_شاخص;
                        U_FALLTHROUGH;
                    افتراضي:
                        /* will never occur */
                        اكسر;
                    }

                    /* target overflow */
                    هدف_ظرفیت=0;
                    *p_Error_کد=U_میانگیر_سرریز_ERROR;
                    اكسر;
                }
            }
        } وإلا {
            /* target is full */
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
            اكسر;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_وضعیت = static_cast<uint32_t>(prev);

    /* write back the updated pointers */
    p_آرگومانها->منبع=منبع;
    p_آرگومانها->هدف = reinterpret_cast<حرف*>(هدف);
    p_آرگومانها->offsets=offsets;
}

/*
 * Identical to _Bocu1FromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ساكن فراغ U_CALLCONV
_Bocu1FromUnicode(UConverter_From_Unicode_آرگومانها *p_آرگومانها,
                  UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت char16_t *منبع, *منبع_حد;
    uint8_t *هدف;
    int32_t هدف_ظرفیت;

    int32_t prev, c, diff;

    /* set up the local pointers */
    cnv=p_آرگومانها->converter;
    منبع=p_آرگومانها->منبع;
    منبع_حد=p_آرگومانها->منبع_حد;
    هدف = reinterpret_cast<uint8_t*>(p_آرگومانها->هدف);
    هدف_ظرفیت = static_cast<int32_t>(p_آرگومانها->هدف_حد - p_آرگومانها->هدف);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    prev = static_cast<int32_t>(cnv->from_Unicode_وضعیت);
    إذا(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }

    /* conversion loop */
    إذا(c!=0 && هدف_ظرفیت>0) {
        اذهب getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(منبع_حد - منبع);
    إذا(هدف_ظرفیت>diff) {
        هدف_ظرفیت=diff;
    }
    طالما(هدف_ظرفیت>0 && (c=*منبع)<0x3000) {
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
        ++منبع;
        --هدف_ظرفیت;
    }
    /* restore real values */
    هدف_ظرفیت = static_cast<int32_t>(reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->هدف_حد) - هدف);

    /* regular loop for all cases */
    طالما(منبع<منبع_حد) {
        إذا(هدف_ظرفیت>0) {
            c=*منبع++;

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
                --هدف_ظرفیت;
                استمر;
            }

            إذا(U16_IS_LEAD(c)) {
getTrail:
                إذا(منبع<منبع_حد) {
                    /* test the following code unit */
                    char16_t trail=*منبع;
                    إذا(U16_IS_TRAIL(trail)) {
                        ++منبع;
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
                --هدف_ظرفیت;
                إذا(c<0x3000) {
                    اذهب fastSingle;
                }
            } وإلا إذا(DIFF_IS_DOUBLE(diff) && 2<=هدف_ظرفیت) {
                /* optimize 2-byte case */
                int32_t m;

                إذا(diff>=0) {
                    diff-=BOCU1_دسترسی_POS_1+1;
                    m=diff%BOCU1_TRAIL_شمار;
                    diff/=BOCU1_TRAIL_شمار;
                    diff+=BOCU1_شروع_POS_2;
                } وإلا {
                    diff-=BOCU1_دسترسی_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_شمار, m);
                    diff+=BOCU1_شروع_NEG_2;
                }
                *هدف++ = static_cast<uint8_t>(diff);
                *هدف++ = static_cast<uint8_t>(BOCU1_TRAIL_مقصد_BYTE(m));
                هدف_ظرفیت-=2;
            } وإلا {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_درازی_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                إذا(length<=هدف_ظرفیت) {
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
                    هدف_ظرفیت-=length;
                } وإلا {
                    uint8_t *char_Error_میانگیر;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=هدف_ظرفیت;
                    char_Error_میانگیر=(uint8_t *)cnv->char_Error_میانگیر;
                    بدل(length) {
                        /* each branch falls through to the next one */
                    حالة 3:
                        *char_Error_میانگیر++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    حالة 2:
                        *char_Error_میانگیر++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    حالة 1:
                        *char_Error_میانگیر = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    افتراضي:
                        /* will never occur */
                        اكسر;
                    }
                    cnv->char_Error_میانگیر_درازی = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    بدل(هدف_ظرفیت) {
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
                    هدف_ظرفیت=0;
                    *p_Error_کد=U_میانگیر_سرریز_ERROR;
                    اكسر;
                }
            }
        } وإلا {
            /* target is full */
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
            اكسر;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_وضعیت = static_cast<uint32_t>(prev);

    /* write back the updated pointers */
    p_آرگومانها->منبع=منبع;
    p_آرگومانها->هدف = reinterpret_cast<حرف*>(هدف);
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

    إذا(b>=BOCU1_شروع_NEG_2) {
        /* positive difference */
        إذا(b<BOCU1_شروع_POS_3) {
            /* two bytes */
            diff = (b - BOCU1_شروع_POS_2) * BOCU1_TRAIL_شمار + BOCU1_دسترسی_POS_1 + 1;
            count=1;
        } وإلا إذا(b<BOCU1_شروع_POS_4) {
            /* three bytes */
            diff = (b - BOCU1_شروع_POS_3) * BOCU1_TRAIL_شمار * BOCU1_TRAIL_شمار + BOCU1_دسترسی_POS_2 + 1;
            count=2;
        } وإلا {
            /* four bytes */
            diff=BOCU1_دسترسی_POS_3+1;
            count=3;
        }
    } وإلا {
        /* negative difference */
        إذا(b>=BOCU1_شروع_NEG_3) {
            /* two bytes */
            diff = (b - BOCU1_شروع_NEG_2) * BOCU1_TRAIL_شمار + BOCU1_دسترسی_NEG_1;
            count=1;
        } وإلا إذا(b>BOCU1_MIN) {
            /* three bytes */
            diff = (b - BOCU1_شروع_NEG_3) * BOCU1_TRAIL_شمار * BOCU1_TRAIL_شمار + BOCU1_دسترسی_NEG_2;
            count=2;
        } وإلا {
            /* four bytes */
            diff=-BOCU1_TRAIL_شمار*BOCU1_TRAIL_شمار*BOCU1_TRAIL_شمار+BOCU1_دسترسی_NEG_3;
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
        b=bocu1_Byte_مقصد_Trail[b];
        /* b<0 for an illegal trail byte value will result in return<0 below */
#إذا BOCU1_MAX_TRAIL<0xff
    } وإلا إذا(b>BOCU1_MAX_TRAIL) {
        أرجع -99;
#پایان_اگر
    } وإلا {
        b-=BOCU1_TRAIL_BYTE_جابه_جایی;
    }

    /* add trail byte into difference and decrement count */
    إذا(count==1) {
        أرجع b;
    } وإلا إذا(count==2) {
        أرجع b*BOCU1_TRAIL_شمار;
    } وإلا /* count==3 */ {
        أرجع b*(BOCU1_TRAIL_شمار*BOCU1_TRAIL_شمار);
    }
}

ساكن فراغ U_CALLCONV
Bocu1_مقصد_Unicode_With_Offsets(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                           UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت uint8_t *منبع, *منبع_حد;
    char16_t *هدف;
    ثابت char16_t *هدف_حد;
    int32_t *offsets;

    int32_t prev, count, diff, c;

    int8_t byte_شاخص;
    uint8_t *بایتها;

    int32_t منبع_شاخص, بعدی_منبع_شاخص;

    /* set up the local pointers */
    cnv=p_آرگومانها->converter;
    منبع = reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->منبع);
    منبع_حد = reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->منبع_حد);
    هدف=p_آرگومانها->هدف;
    هدف_حد=p_آرگومانها->هدف_حد;
    offsets=p_آرگومانها->offsets;

    /* get the converter state from UConverter */
    prev = static_cast<int32_t>(cnv->مقصد_Unicode_وضعیت);
    إذا(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }
    diff=cnv->نما; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_شاخص=cnv->مقصد_ULength;
    بایتها=cnv->مقصد_UBytes;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    منبع_شاخص=byte_شاخص==0 ? 0 : -1;
    بعدی_منبع_شاخص=0;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    إذا(count>0 && byte_شاخص>0 && هدف<هدف_حد) {
        اذهب getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(منبع_حد - منبع);
    count = static_cast<int32_t>(p_آرگومانها->هدف_حد - هدف);
    إذا(count>diff) {
        count=diff;
    }
    طالما(count>0) {
        إذا(BOCU1_شروع_NEG_2<=(c=*منبع) && c<BOCU1_شروع_POS_2) {
            c=prev+(c-BOCU1_میانه);
            إذا(c<0x3000) {
                *هدف++ = static_cast<char16_t>(c);
                *offsets++=بعدی_منبع_شاخص++;
                prev=BOCU1_SIMPLE_PREV(c);
            } وإلا {
                اكسر;
            }
        } وإلا إذا(c<=0x20) {
            إذا(c!=0x20) {
                prev=BOCU1_ASCII_PREV;
            }
            *هدف++ = static_cast<char16_t>(c);
            *offsets++=بعدی_منبع_شاخص++;
        } وإلا {
            اكسر;
        }
        ++منبع;
        --count;
    }
    منبع_شاخص=بعدی_منبع_شاخص; /* wrong if offsets==nullptr but does not matter */

    /* decode a sequence of single and lead bytes */
    طالما(منبع<منبع_حد) {
        إذا(هدف>=هدف_حد) {
            /* target is full */
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
            اكسر;
        }

        ++بعدی_منبع_شاخص;
        c=*منبع++;
        إذا(BOCU1_شروع_NEG_2<=c && c<BOCU1_شروع_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=prev+(c-BOCU1_میانه);
            إذا(c<0x3000) {
                *هدف++ = static_cast<char16_t>(c);
                *offsets++=منبع_شاخص;
                prev=BOCU1_SIMPLE_PREV(c);
                منبع_شاخص=بعدی_منبع_شاخص;
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
            *offsets++=منبع_شاخص;
            منبع_شاخص=بعدی_منبع_شاخص;
            استمر;
        } وإلا إذا(BOCU1_شروع_NEG_3<=c && c<BOCU1_شروع_POS_3 && منبع<منبع_حد) {
            /* Optimize two-byte case. */
            إذا(c>=BOCU1_میانه) {
                diff = (c - BOCU1_شروع_POS_2) * BOCU1_TRAIL_شمار + BOCU1_دسترسی_POS_1 + 1;
            } وإلا {
                diff = (c - BOCU1_شروع_NEG_2) * BOCU1_TRAIL_شمار + BOCU1_دسترسی_NEG_1;
            }

            /* trail byte */
            ++بعدی_منبع_شاخص;
            c=decodeBocu1TrailByte(1, *منبع++);
            إذا (c < 0 || static_cast<uint32_t>(c = prev + diff + c) > 0x10ffff) {
                بایتها[0]=منبع[-2];
                بایتها[1]=منبع[-1];
                byte_شاخص=2;
                *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                اكسر;
            }
        } وإلا إذا(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            prev=BOCU1_ASCII_PREV;
            منبع_شاخص=بعدی_منبع_شاخص;
            استمر;
        } وإلا {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            بایتها[0] = static_cast<uint8_t>(c);
            byte_شاخص=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            لكل(;;) {
                إذا(منبع>=منبع_حد) {
                    اذهب endloop;
                }
                ++بعدی_منبع_شاخص;
                c=بایتها[byte_شاخص++]=*منبع++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                إذا(c<0) {
                    *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                    اذهب endloop;
                }

                diff+=c;
                إذا(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_شاخص=0;
                    c=prev+diff;
                    إذا (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_کد=U_ILLEGAL_CHAR_یافت;
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
            *offsets++=منبع_شاخص;
        } وإلا {
            /* output surrogate pair */
            *هدف++=U16_LEAD(c);
            إذا(هدف<هدف_حد) {
                *هدف++=U16_TRAIL(c);
                *offsets++=منبع_شاخص;
                *offsets++=منبع_شاخص;
            } وإلا {
                /* target overflow */
                *offsets++=منبع_شاخص;
                cnv->UChar_Error_میانگیر[0]=U16_TRAIL(c);
                cnv->UChar_Error_میانگیر_درازی=1;
                *p_Error_کد=U_میانگیر_سرریز_ERROR;
                اكسر;
            }
        }
        منبع_شاخص=بعدی_منبع_شاخص;
    }
endloop:

    إذا(*p_Error_کد==U_ILLEGAL_CHAR_یافت) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->مقصد_Unicode_وضعیت=BOCU1_ASCII_PREV;
        cnv->نما=0;
    } وإلا {
        /* set the converter state back into UConverter */
        cnv->مقصد_Unicode_وضعیت = static_cast<uint32_t>(prev);
        cnv->نما = static_cast<int32_t>(static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->مقصد_ULength=byte_شاخص;

    /* write back the updated pointers */
    p_آرگومانها->منبع = reinterpret_cast<ثابت حرف*>(منبع);
    p_آرگومانها->هدف=هدف;
    p_آرگومانها->offsets=offsets;
}

/*
 * Identical to _Bocu1ToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ساكن فراغ U_CALLCONV
Bocu1_مقصد_Unicode(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                UError_کد *p_Error_کد) {
    UConverter *cnv;
    ثابت uint8_t *منبع, *منبع_حد;
    char16_t *هدف;
    ثابت char16_t *هدف_حد;

    int32_t prev, count, diff, c;

    int8_t byte_شاخص;
    uint8_t *بایتها;

    /* set up the local pointers */
    cnv=p_آرگومانها->converter;
    منبع = reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->منبع);
    منبع_حد = reinterpret_cast<ثابت uint8_t*>(p_آرگومانها->منبع_حد);
    هدف=p_آرگومانها->هدف;
    هدف_حد=p_آرگومانها->هدف_حد;

    /* get the converter state from UConverter */
    prev = static_cast<int32_t>(cnv->مقصد_Unicode_وضعیت);
    إذا(prev==0) {
        prev=BOCU1_ASCII_PREV;
    }
    diff=cnv->نما; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_شاخص=cnv->مقصد_ULength;
    بایتها=cnv->مقصد_UBytes;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    إذا(count>0 && byte_شاخص>0 && هدف<هدف_حد) {
        اذهب getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(منبع_حد - منبع);
    count = static_cast<int32_t>(p_آرگومانها->هدف_حد - هدف);
    إذا(count>diff) {
        count=diff;
    }
    طالما(count>0) {
        إذا(BOCU1_شروع_NEG_2<=(c=*منبع) && c<BOCU1_شروع_POS_2) {
            c=prev+(c-BOCU1_میانه);
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
        ++منبع;
        --count;
    }

    /* decode a sequence of single and lead bytes */
    طالما(منبع<منبع_حد) {
        إذا(هدف>=هدف_حد) {
            /* target is full */
            *p_Error_کد=U_میانگیر_سرریز_ERROR;
            اكسر;
        }

        c=*منبع++;
        إذا(BOCU1_شروع_NEG_2<=c && c<BOCU1_شروع_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=prev+(c-BOCU1_میانه);
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
        } وإلا إذا(BOCU1_شروع_NEG_3<=c && c<BOCU1_شروع_POS_3 && منبع<منبع_حد) {
            /* Optimize two-byte case. */
            إذا(c>=BOCU1_میانه) {
                diff = (c - BOCU1_شروع_POS_2) * BOCU1_TRAIL_شمار + BOCU1_دسترسی_POS_1 + 1;
            } وإلا {
                diff = (c - BOCU1_شروع_NEG_2) * BOCU1_TRAIL_شمار + BOCU1_دسترسی_NEG_1;
            }

            /* trail byte */
            c=decodeBocu1TrailByte(1, *منبع++);
            إذا (c < 0 || static_cast<uint32_t>(c = prev + diff + c) > 0x10ffff) {
                بایتها[0]=منبع[-2];
                بایتها[1]=منبع[-1];
                byte_شاخص=2;
                *p_Error_کد=U_ILLEGAL_CHAR_یافت;
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
            بایتها[0] = static_cast<uint8_t>(c);
            byte_شاخص=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            لكل(;;) {
                إذا(منبع>=منبع_حد) {
                    اذهب endloop;
                }
                c=بایتها[byte_شاخص++]=*منبع++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                إذا(c<0) {
                    *p_Error_کد=U_ILLEGAL_CHAR_یافت;
                    اذهب endloop;
                }

                diff+=c;
                إذا(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_شاخص=0;
                    c=prev+diff;
                    إذا (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_کد=U_ILLEGAL_CHAR_یافت;
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
            إذا(هدف<هدف_حد) {
                *هدف++=U16_TRAIL(c);
            } وإلا {
                /* target overflow */
                cnv->UChar_Error_میانگیر[0]=U16_TRAIL(c);
                cnv->UChar_Error_میانگیر_درازی=1;
                *p_Error_کد=U_میانگیر_سرریز_ERROR;
                اكسر;
            }
        }
    }
endloop:

    إذا(*p_Error_کد==U_ILLEGAL_CHAR_یافت) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->مقصد_Unicode_وضعیت=BOCU1_ASCII_PREV;
        cnv->نما=0;
    } وإلا {
        /* set the converter state back into UConverter */
        cnv->مقصد_Unicode_وضعیت = static_cast<uint32_t>(prev);
        cnv->نما = (static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->مقصد_ULength=byte_شاخص;

    /* write back the updated pointers */
    p_آرگومانها->منبع = reinterpret_cast<ثابت حرف*>(منبع);
    p_آرگومانها->هدف=هدف;
}

/* miscellaneous ------------------------------------------------------------ */

ساكن ثابت UConverterImpl _Bocu1Impl={
    UCNV_BOCU1,

    اشاره_تهی,
    اشاره_تهی,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,

    Bocu1_مقصد_Unicode,
    Bocu1_مقصد_Unicode_With_Offsets,
    _Bocu1FromUnicode,
    _Bocu1FromUnicodeWithOffsets,
    اشاره_تهی,

    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    ucnv_get_Complete_Unicode_مجموعه,

    اشاره_تهی,
    اشاره_تهی
};

ساكن ثابت UConverter_Static_داده Bocu1_Static_داده={
    حجم(UConverter_Static_داده),
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

ثابت UConverter_Shared_داده Bocu1_داده=
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&Bocu1_Static_داده, &_Bocu1Impl);

#پایان_اگر
