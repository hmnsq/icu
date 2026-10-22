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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#شامل "unicode/ucnv.h"
#شامل "unicode/ucnv_cb.h"
#شامل "unicode/utf16.h"
#شامل "putilimp.h"
#شامل "ucnv_bld.h"
#شامل "ucnv_cnv.h"
#شامل "uassert.h"

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
#تعریف_کرو BOCU1_ASCII_پچھلا        0x40

/* bounding byte values for differences */
#تعریف_کرو BOCU1_MIN               0x21
#تعریف_کرو BOCU1_درمیان            0x90
#تعریف_کرو BOCU1_MAX_LEAD          0xfe
#تعریف_کرو BOCU1_MAX_TRAIL         0xff
#تعریف_کرو BOCU1_RESET             0xff

/* number of lead bytes */
#تعریف_کرو BOCU1_گنتی             (BOCU1_MAX_LEAD-BOCU1_MIN+1)

/* adjust trail byte counts for the use of some C0 control byte values */
#تعریف_کرو BOCU1_TRAIL_CONTROLS_گنتی  20
#تعریف_کرو BOCU1_TRAIL_BYTE_انحراف     (BOCU1_MIN-BOCU1_TRAIL_CONTROLS_گنتی)

/* number of trail bytes */
#تعریف_کرو BOCU1_TRAIL_گنتی       ((BOCU1_MAX_TRAIL-BOCU1_MIN+1)+BOCU1_TRAIL_CONTROLS_گنتی)

/*
 * number of positive and negative single-byte codes
 * (counting 0==BOCU1_MIDDLE among the positive ones)
 */
#تعریف_کرو BOCU1_SINGLE            64

/* number of lead bytes for positive and negative 2/3/4-byte sequences */
#تعریف_کرو BOCU1_LEAD_2            43
#تعریف_کرو BOCU1_LEAD_3            3
#تعریف_کرو BOCU1_LEAD_4            1

/* The difference value range for single-byters. */
#تعریف_کرو BOCU1_رسائی_POS_1   (BOCU1_SINGLE-1)
#تعریف_کرو BOCU1_رسائی_NEG_1   (-BOCU1_SINGLE)

/* The difference value range for double-byters. */
#تعریف_کرو BOCU1_رسائی_POS_2   (BOCU1_رسائی_POS_1+BOCU1_LEAD_2*BOCU1_TRAIL_گنتی)
#تعریف_کرو BOCU1_رسائی_NEG_2   (BOCU1_رسائی_NEG_1-BOCU1_LEAD_2*BOCU1_TRAIL_گنتی)

/* The difference value range for 3-byters. */
#تعریف_کرو BOCU1_رسائی_POS_3   \
    (BOCU1_رسائی_POS_2+BOCU1_LEAD_3*BOCU1_TRAIL_گنتی*BOCU1_TRAIL_گنتی)

#تعریف_کرو BOCU1_رسائی_NEG_3   (BOCU1_رسائی_NEG_2-BOCU1_LEAD_3*BOCU1_TRAIL_گنتی*BOCU1_TRAIL_گنتی)

/* The lead byte start values. */
#تعریف_کرو BOCU1_آغاز_POS_2   (BOCU1_درمیان+BOCU1_رسائی_POS_1+1)
#تعریف_کرو BOCU1_آغاز_POS_3   (BOCU1_آغاز_POS_2+BOCU1_LEAD_2)
#تعریف_کرو BOCU1_آغاز_POS_4   (BOCU1_آغاز_POS_3+BOCU1_LEAD_3)
     /* ==BOCU1_MAX_LEAD */

#تعریف_کرو BOCU1_آغاز_NEG_2   (BOCU1_درمیان+BOCU1_رسائی_NEG_1)
#تعریف_کرو BOCU1_آغاز_NEG_3   (BOCU1_آغاز_NEG_2-BOCU1_LEAD_2)
#تعریف_کرو BOCU1_آغاز_NEG_4   (BOCU1_آغاز_NEG_3-BOCU1_LEAD_3)
     /* ==BOCU1_MIN+1 */

/* The length of a byte sequence, according to the lead byte (!=BOCU1_RESET). */
#تعریف_کرو BOCU1_لمبائی_FROM_LEAD(lead) \
    ((BOCU1_آغاز_NEG_2<=(lead) && (lead)<BOCU1_آغاز_POS_2) ? 1 : \
     (BOCU1_آغاز_NEG_3<=(lead) && (lead)<BOCU1_آغاز_POS_3) ? 2 : \
     (BOCU1_آغاز_NEG_4<=(lead) && (lead)<BOCU1_آغاز_POS_4) ? 3 : 4)

/* The length of a byte sequence, according to its packed form. */
#تعریف_کرو BOCU1_لمبائی_FROM_PACKED(packed) \
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
#تعریف_کرو BOCU1_TRAIL_مقصد_BYTE(t) ((t)>=BOCU1_TRAIL_CONTROLS_گنتی ? (t)+BOCU1_TRAIL_BYTE_انحراف : bocu1_Trail_مقصد_Byte[t])

/*
 * Byte value map for control codes,
 * from external byte values 0x00..0x20
 * to trail byte values 0..19 (0..0x13) as used in the difference calculation.
 * External byte values that are illegal as trail bytes are mapped to -1.
 */
ساکن ثابت int8_t
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
ساکن ثابت int8_t
bocu1_Trail_مقصد_Byte[BOCU1_TRAIL_CONTROLS_گنتی]={
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
#تعریف_کرو NEGDIVMOD(n, d, m) UPRV_بلاک_MACRO_BEGIN { \
    (m)=(n)%(d); \
    (n)/=(d); \
    اگر((m)<0) { \
        --(n); \
        (m)+=(d); \
    } \
} UPRV_بلاک_MACRO_انتہا

/* Faster versions of packDiff() for single-byte-encoded diff values. */

/** Is a diff value encodable in a single byte? */
#تعریف_کرو DIFF_IS_SINGLE(diff) (BOCU1_رسائی_NEG_1<=(diff) && (diff)<=BOCU1_رسائی_POS_1)

/** Encode a diff value in a single byte. */
#تعریف_کرو PACK_SINGLE_DIFF(diff) (BOCU1_درمیان+(diff))

/** Is a diff value encodable in two bytes? */
#تعریف_کرو DIFF_IS_DOUBLE(diff) (BOCU1_رسائی_NEG_2<=(diff) && (diff)<=BOCU1_رسائی_POS_2)

/* BOCU-1 implementation functions ------------------------------------------ */

#تعریف_کرو BOCU1_SIMPLE_پچھلا(c) (((c)&~0x7f)+BOCU1_ASCII_پچھلا)

/**
 * Compute the next "previous" value for differencing
 * from the current code point.
 *
 * @param c current code point, 0x3040..0xd7a3 (rest handled by macro below)
 * @return "previous code point" state value
 */
ساکن اندرونی int32_t
bocu1_پچھلا(int32_t c) {
    /* compute new prev */
    اگر(/* 0x3040<=c && */ c<=0x309f) {
        /* Hiragana is not 128-aligned */
        واپس 0x3070;
    } ورنہ اگر(0x4e00<=c && c<=0x9fa5) {
        /* CJK Unihan */
        واپس 0x4e00-BOCU1_رسائی_NEG_2;
    } ورنہ اگر(0xac00<=c /* && c<=0xd7a3 */) {
        /* Korean Hangul */
        واپس (0xd7a3+0xac00)/2;
    } ورنہ {
        /* mostly small scripts */
        واپس BOCU1_SIMPLE_پچھلا(c);
    }
}

/** Fast version of bocu1Prev() for most scripts. */
#تعریف_کرو BOCU1_پچھلا(c) ((c)<0x3040 || (c)>0xd7a3 ? BOCU1_SIMPLE_پچھلا(c) : bocu1_پچھلا(c))

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
ساکن int32_t
packDiff(int32_t diff) {
    int32_t نتیجہ, m;

    U_ASSERT(!DIFF_IS_SINGLE(diff)); /* assume we won't be called where diff==BOCU1_REACH_NEG_1=-64 */
    اگر(diff>=BOCU1_رسائی_NEG_1) {
        /* mostly positive differences, and single-byte negative ones */
#اگر 0   /* single-byte case handled in macros, see below */
        اگر(diff<=BOCU1_رسائی_POS_1) {
            /* single byte */
            واپس 0x01000000|(BOCU1_درمیان+diff);
        } ورنہ
#اگر_ختم
        اگر(diff<=BOCU1_رسائی_POS_2) {
            /* two bytes */
            diff-=BOCU1_رسائی_POS_1+1;
            نتیجہ=0x02000000;

            m=diff%BOCU1_TRAIL_گنتی;
            diff/=BOCU1_TRAIL_گنتی;
            نتیجہ|=BOCU1_TRAIL_مقصد_BYTE(m);

            نتیجہ|=(BOCU1_آغاز_POS_2+diff)<<8;
        } ورنہ اگر(diff<=BOCU1_رسائی_POS_3) {
            /* three bytes */
            diff-=BOCU1_رسائی_POS_2+1;
            نتیجہ=0x03000000;

            m=diff%BOCU1_TRAIL_گنتی;
            diff/=BOCU1_TRAIL_گنتی;
            نتیجہ|=BOCU1_TRAIL_مقصد_BYTE(m);

            m=diff%BOCU1_TRAIL_گنتی;
            diff/=BOCU1_TRAIL_گنتی;
            نتیجہ|=BOCU1_TRAIL_مقصد_BYTE(m)<<8;

            نتیجہ|=(BOCU1_آغاز_POS_3+diff)<<16;
        } ورنہ {
            /* four bytes */
            diff-=BOCU1_رسائی_POS_3+1;

            m=diff%BOCU1_TRAIL_گنتی;
            diff/=BOCU1_TRAIL_گنتی;
            نتیجہ=BOCU1_TRAIL_مقصد_BYTE(m);

            m=diff%BOCU1_TRAIL_گنتی;
            diff/=BOCU1_TRAIL_گنتی;
            نتیجہ|=BOCU1_TRAIL_مقصد_BYTE(m)<<8;

            /*
             * We know that / and % would deliver quotient 0 and rest=diff.
             * Avoid division and modulo for performance.
             */
            نتیجہ|=BOCU1_TRAIL_مقصد_BYTE(diff)<<16;

            نتیجہ |= static_cast<uint32_t>(BOCU1_آغاز_POS_4) << 24;
        }
    } ورنہ {
        /* two- to four-byte negative differences */
        اگر(diff>=BOCU1_رسائی_NEG_2) {
            /* two bytes */
            diff-=BOCU1_رسائی_NEG_1;
            نتیجہ=0x02000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_گنتی, m);
            نتیجہ|=BOCU1_TRAIL_مقصد_BYTE(m);

            نتیجہ|=(BOCU1_آغاز_NEG_2+diff)<<8;
        } ورنہ اگر(diff>=BOCU1_رسائی_NEG_3) {
            /* three bytes */
            diff-=BOCU1_رسائی_NEG_2;
            نتیجہ=0x03000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_گنتی, m);
            نتیجہ|=BOCU1_TRAIL_مقصد_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_گنتی, m);
            نتیجہ|=BOCU1_TRAIL_مقصد_BYTE(m)<<8;

            نتیجہ|=(BOCU1_آغاز_NEG_3+diff)<<16;
        } ورنہ {
            /* four bytes */
            diff-=BOCU1_رسائی_NEG_3;

            NEGDIVMOD(diff, BOCU1_TRAIL_گنتی, m);
            نتیجہ=BOCU1_TRAIL_مقصد_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_گنتی, m);
            نتیجہ|=BOCU1_TRAIL_مقصد_BYTE(m)<<8;

            /*
             * We know that NEGDIVMOD would deliver
             * quotient -1 and rest=diff+BOCU1_TRAIL_COUNT.
             * Avoid division and modulo for performance.
             */
            m=diff+BOCU1_TRAIL_گنتی;
            نتیجہ|=BOCU1_TRAIL_مقصد_BYTE(m)<<16;

            نتیجہ|=BOCU1_MIN<<24;
        }
    }
    واپس نتیجہ;
}


ساکن خالی U_CALLCONV
_Bocu1FromUnicodeWithOffsets(UConverter_From_Unicode_دلائل *p_دلائل,
                             UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت char16_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف;
    int32_t ہدف_استعداد;
    int32_t *offsets;

    int32_t پچھلا, c, diff;

    int32_t ماخذ_اشاریہ, اگلا_ماخذ_اشاریہ;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    ماخذ=p_دلائل->ماخذ;
    ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
    ہدف = reinterpret_cast<uint8_t*>(p_دلائل->ہدف);
    ہدف_استعداد = static_cast<int32_t>(p_دلائل->ہدف_حد_بندی - p_دلائل->ہدف);
    offsets=p_دلائل->offsets;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    پچھلا = static_cast<int32_t>(cnv->from_Unicode_حالت);
    اگر(پچھلا==0) {
        پچھلا=BOCU1_ASCII_پچھلا;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ= c==0 ? 0 : -1;
    اگلا_ماخذ_اشاریہ=0;

    /* conversion loop */
    اگر(c!=0 && ہدف_استعداد>0) {
        جاؤ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(ماخذ_حد_بندی - ماخذ);
    اگر(ہدف_استعداد>diff) {
        ہدف_استعداد=diff;
    }
    جب(ہدف_استعداد>0 && (c=*ماخذ)<0x3000) {
        اگر(c<=0x20) {
            اگر(c!=0x20) {
                پچھلا=BOCU1_ASCII_پچھلا;
            }
            *ہدف++ = static_cast<uint8_t>(c);
            *offsets++=اگلا_ماخذ_اشاریہ++;
            ++ماخذ;
            --ہدف_استعداد;
        } ورنہ {
            diff=c-پچھلا;
            اگر(DIFF_IS_SINGLE(diff)) {
                پچھلا=BOCU1_SIMPLE_پچھلا(c);
                *ہدف++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=اگلا_ماخذ_اشاریہ++;
                ++ماخذ;
                --ہدف_استعداد;
            } ورنہ {
                توڑو;
            }
        }
    }
    /* restore real values */
    ہدف_استعداد = static_cast<int32_t>(reinterpret_cast<ثابت uint8_t*>(p_دلائل->ہدف_حد_بندی) - ہدف);
    ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ; /* wrong if offsets==nullptr but does not matter */

    /* regular loop for all cases */
    جب(ماخذ<ماخذ_حد_بندی) {
        اگر(ہدف_استعداد>0) {
            c=*ماخذ++;
            ++اگلا_ماخذ_اشاریہ;

            اگر(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                اگر(c!=0x20) {
                    پچھلا=BOCU1_ASCII_پچھلا;
                }
                *ہدف++ = static_cast<uint8_t>(c);
                *offsets++=ماخذ_اشاریہ;
                --ہدف_استعداد;

                ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                جاری;
            }

            اگر(U16_IS_LEAD(c)) {
getTrail:
                اگر(ماخذ<ماخذ_حد_بندی) {
                    /* test the following code unit */
                    char16_t trail=*ماخذ;
                    اگر(U16_IS_TRAIL(trail)) {
                        ++ماخذ;
                        ++اگلا_ماخذ_اشاریہ;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } ورنہ {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    توڑو;
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
            diff=c-پچھلا;
            پچھلا=BOCU1_پچھلا(c);
            اگر(DIFF_IS_SINGLE(diff)) {
                *ہدف++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=ماخذ_اشاریہ;
                --ہدف_استعداد;
                ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                اگر(c<0x3000) {
                    جاؤ fastSingle;
                }
            } ورنہ اگر(DIFF_IS_DOUBLE(diff) && 2<=ہدف_استعداد) {
                /* optimize 2-byte case */
                int32_t m;

                اگر(diff>=0) {
                    diff-=BOCU1_رسائی_POS_1+1;
                    m=diff%BOCU1_TRAIL_گنتی;
                    diff/=BOCU1_TRAIL_گنتی;
                    diff+=BOCU1_آغاز_POS_2;
                } ورنہ {
                    diff-=BOCU1_رسائی_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_گنتی, m);
                    diff+=BOCU1_آغاز_NEG_2;
                }
                *ہدف++ = static_cast<uint8_t>(diff);
                *ہدف++ = static_cast<uint8_t>(BOCU1_TRAIL_مقصد_BYTE(m));
                *offsets++=ماخذ_اشاریہ;
                *offsets++=ماخذ_اشاریہ;
                ہدف_استعداد-=2;
                ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
            } ورنہ {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_لمبائی_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                اگر(length<=ہدف_استعداد) {
                    انتخاب(length) {
                        /* each branch falls through to the next one */
                    حالت 4:
                        *ہدف++ = static_cast<uint8_t>(diff >> 24);
                        *offsets++=ماخذ_اشاریہ;
                        U_FALLTHROUGH;
                    حالت 3:
                        *ہدف++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=ماخذ_اشاریہ;
                        U_FALLTHROUGH;
                    حالت 2:
                        *ہدف++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=ماخذ_اشاریہ;
                    /* case 1: handled above */
                        *ہدف++ = static_cast<uint8_t>(diff);
                        *offsets++=ماخذ_اشاریہ;
                        U_FALLTHROUGH;
                    طےشدہ:
                        /* will never occur */
                        توڑو;
                    }
                    ہدف_استعداد-=length;
                    ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                } ورنہ {
                    uint8_t *char_Error_بفر;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=ہدف_استعداد;
                    char_Error_بفر=(uint8_t *)cnv->char_Error_بفر;
                    انتخاب(length) {
                        /* each branch falls through to the next one */
                    حالت 3:
                        *char_Error_بفر++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    حالت 2:
                        *char_Error_بفر++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    حالت 1:
                        *char_Error_بفر = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    طےشدہ:
                        /* will never occur */
                        توڑو;
                    }
                    cnv->char_Error_بفر_لمبائی = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    انتخاب(ہدف_استعداد) {
                        /* each branch falls through to the next one */
                    حالت 3:
                        *ہدف++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=ماخذ_اشاریہ;
                        U_FALLTHROUGH;
                    حالت 2:
                        *ہدف++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=ماخذ_اشاریہ;
                        U_FALLTHROUGH;
                    حالت 1:
                        *ہدف++ = static_cast<uint8_t>(diff);
                        *offsets++=ماخذ_اشاریہ;
                        U_FALLTHROUGH;
                    طےشدہ:
                        /* will never occur */
                        توڑو;
                    }

                    /* target overflow */
                    ہدف_استعداد=0;
                    *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                    توڑو;
                }
            }
        } ورنہ {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_حالت = static_cast<uint32_t>(پچھلا);

    /* write back the updated pointers */
    p_دلائل->ماخذ=ماخذ;
    p_دلائل->ہدف = reinterpret_cast<حرف*>(ہدف);
    p_دلائل->offsets=offsets;
}

/*
 * Identical to _Bocu1FromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ساکن خالی U_CALLCONV
_Bocu1FromUnicode(UConverter_From_Unicode_دلائل *p_دلائل,
                  UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت char16_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف;
    int32_t ہدف_استعداد;

    int32_t پچھلا, c, diff;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    ماخذ=p_دلائل->ماخذ;
    ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
    ہدف = reinterpret_cast<uint8_t*>(p_دلائل->ہدف);
    ہدف_استعداد = static_cast<int32_t>(p_دلائل->ہدف_حد_بندی - p_دلائل->ہدف);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    پچھلا = static_cast<int32_t>(cnv->from_Unicode_حالت);
    اگر(پچھلا==0) {
        پچھلا=BOCU1_ASCII_پچھلا;
    }

    /* conversion loop */
    اگر(c!=0 && ہدف_استعداد>0) {
        جاؤ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(ماخذ_حد_بندی - ماخذ);
    اگر(ہدف_استعداد>diff) {
        ہدف_استعداد=diff;
    }
    جب(ہدف_استعداد>0 && (c=*ماخذ)<0x3000) {
        اگر(c<=0x20) {
            اگر(c!=0x20) {
                پچھلا=BOCU1_ASCII_پچھلا;
            }
            *ہدف++ = static_cast<uint8_t>(c);
        } ورنہ {
            diff=c-پچھلا;
            اگر(DIFF_IS_SINGLE(diff)) {
                پچھلا=BOCU1_SIMPLE_پچھلا(c);
                *ہدف++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
            } ورنہ {
                توڑو;
            }
        }
        ++ماخذ;
        --ہدف_استعداد;
    }
    /* restore real values */
    ہدف_استعداد = static_cast<int32_t>(reinterpret_cast<ثابت uint8_t*>(p_دلائل->ہدف_حد_بندی) - ہدف);

    /* regular loop for all cases */
    جب(ماخذ<ماخذ_حد_بندی) {
        اگر(ہدف_استعداد>0) {
            c=*ماخذ++;

            اگر(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                اگر(c!=0x20) {
                    پچھلا=BOCU1_ASCII_پچھلا;
                }
                *ہدف++ = static_cast<uint8_t>(c);
                --ہدف_استعداد;
                جاری;
            }

            اگر(U16_IS_LEAD(c)) {
getTrail:
                اگر(ماخذ<ماخذ_حد_بندی) {
                    /* test the following code unit */
                    char16_t trail=*ماخذ;
                    اگر(U16_IS_TRAIL(trail)) {
                        ++ماخذ;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } ورنہ {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    توڑو;
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
            diff=c-پچھلا;
            پچھلا=BOCU1_پچھلا(c);
            اگر(DIFF_IS_SINGLE(diff)) {
                *ہدف++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                --ہدف_استعداد;
                اگر(c<0x3000) {
                    جاؤ fastSingle;
                }
            } ورنہ اگر(DIFF_IS_DOUBLE(diff) && 2<=ہدف_استعداد) {
                /* optimize 2-byte case */
                int32_t m;

                اگر(diff>=0) {
                    diff-=BOCU1_رسائی_POS_1+1;
                    m=diff%BOCU1_TRAIL_گنتی;
                    diff/=BOCU1_TRAIL_گنتی;
                    diff+=BOCU1_آغاز_POS_2;
                } ورنہ {
                    diff-=BOCU1_رسائی_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_گنتی, m);
                    diff+=BOCU1_آغاز_NEG_2;
                }
                *ہدف++ = static_cast<uint8_t>(diff);
                *ہدف++ = static_cast<uint8_t>(BOCU1_TRAIL_مقصد_BYTE(m));
                ہدف_استعداد-=2;
            } ورنہ {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_لمبائی_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                اگر(length<=ہدف_استعداد) {
                    انتخاب(length) {
                        /* each branch falls through to the next one */
                    حالت 4:
                        *ہدف++ = static_cast<uint8_t>(diff >> 24);
                        U_FALLTHROUGH;
                    حالت 3:
                        *ہدف++ = static_cast<uint8_t>(diff >> 16);
                    /* case 2: handled above */
                        *ہدف++ = static_cast<uint8_t>(diff >> 8);
                    /* case 1: handled above */
                        *ہدف++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    طےشدہ:
                        /* will never occur */
                        توڑو;
                    }
                    ہدف_استعداد-=length;
                } ورنہ {
                    uint8_t *char_Error_بفر;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=ہدف_استعداد;
                    char_Error_بفر=(uint8_t *)cnv->char_Error_بفر;
                    انتخاب(length) {
                        /* each branch falls through to the next one */
                    حالت 3:
                        *char_Error_بفر++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    حالت 2:
                        *char_Error_بفر++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    حالت 1:
                        *char_Error_بفر = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    طےشدہ:
                        /* will never occur */
                        توڑو;
                    }
                    cnv->char_Error_بفر_لمبائی = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    انتخاب(ہدف_استعداد) {
                        /* each branch falls through to the next one */
                    حالت 3:
                        *ہدف++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    حالت 2:
                        *ہدف++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    حالت 1:
                        *ہدف++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    طےشدہ:
                        /* will never occur */
                        توڑو;
                    }

                    /* target overflow */
                    ہدف_استعداد=0;
                    *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                    توڑو;
                }
            }
        } ورنہ {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_حالت = static_cast<uint32_t>(پچھلا);

    /* write back the updated pointers */
    p_دلائل->ماخذ=ماخذ;
    p_دلائل->ہدف = reinterpret_cast<حرف*>(ہدف);
}

/* BOCU-1-to-Unicode conversion functions ----------------------------------- */

/**
 * Function for BOCU-1 decoder; handles multi-byte lead bytes.
 *
 * @param b lead byte;
 *          BOCU1_MIN<=b<BOCU1_START_NEG_2 or BOCU1_START_POS_2<=b<BOCU1_MAX_LEAD
 * @return (diff<<2)|count
 */
ساکن اندرونی int32_t
decodeBocu1LeadByte(int32_t b) {
    int32_t diff, count;

    اگر(b>=BOCU1_آغاز_NEG_2) {
        /* positive difference */
        اگر(b<BOCU1_آغاز_POS_3) {
            /* two bytes */
            diff = (b - BOCU1_آغاز_POS_2) * BOCU1_TRAIL_گنتی + BOCU1_رسائی_POS_1 + 1;
            count=1;
        } ورنہ اگر(b<BOCU1_آغاز_POS_4) {
            /* three bytes */
            diff = (b - BOCU1_آغاز_POS_3) * BOCU1_TRAIL_گنتی * BOCU1_TRAIL_گنتی + BOCU1_رسائی_POS_2 + 1;
            count=2;
        } ورنہ {
            /* four bytes */
            diff=BOCU1_رسائی_POS_3+1;
            count=3;
        }
    } ورنہ {
        /* negative difference */
        اگر(b>=BOCU1_آغاز_NEG_3) {
            /* two bytes */
            diff = (b - BOCU1_آغاز_NEG_2) * BOCU1_TRAIL_گنتی + BOCU1_رسائی_NEG_1;
            count=1;
        } ورنہ اگر(b>BOCU1_MIN) {
            /* three bytes */
            diff = (b - BOCU1_آغاز_NEG_3) * BOCU1_TRAIL_گنتی * BOCU1_TRAIL_گنتی + BOCU1_رسائی_NEG_2;
            count=2;
        } ورنہ {
            /* four bytes */
            diff=-BOCU1_TRAIL_گنتی*BOCU1_TRAIL_گنتی*BOCU1_TRAIL_گنتی+BOCU1_رسائی_NEG_3;
            count=3;
        }
    }

    /* return the state for decoding the trail byte(s) */
    واپس (static_cast<uint32_t>(diff) << 2) | count;
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
ساکن اندرونی int32_t
decodeBocu1TrailByte(int32_t count, int32_t b) {
    اگر(b<=0x20) {
        /* skip some C0 controls and make the trail byte range contiguous */
        b=bocu1_Byte_مقصد_Trail[b];
        /* b<0 for an illegal trail byte value will result in return<0 below */
#اگر BOCU1_MAX_TRAIL<0xff
    } ورنہ اگر(b>BOCU1_MAX_TRAIL) {
        واپس -99;
#اگر_ختم
    } ورنہ {
        b-=BOCU1_TRAIL_BYTE_انحراف;
    }

    /* add trail byte into difference and decrement count */
    اگر(count==1) {
        واپس b;
    } ورنہ اگر(count==2) {
        واپس b*BOCU1_TRAIL_گنتی;
    } ورنہ /* count==3 */ {
        واپس b*(BOCU1_TRAIL_گنتی*BOCU1_TRAIL_گنتی);
    }
}

ساکن خالی U_CALLCONV
Bocu1_مقصد_Unicode_With_Offsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                           UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    char16_t *ہدف;
    ثابت char16_t *ہدف_حد_بندی;
    int32_t *offsets;

    int32_t پچھلا, count, diff, c;

    int8_t byte_اشاریہ;
    uint8_t *بائٹیں;

    int32_t ماخذ_اشاریہ, اگلا_ماخذ_اشاریہ;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    ماخذ = reinterpret_cast<ثابت uint8_t*>(p_دلائل->ماخذ);
    ماخذ_حد_بندی = reinterpret_cast<ثابت uint8_t*>(p_دلائل->ماخذ_حد_بندی);
    ہدف=p_دلائل->ہدف;
    ہدف_حد_بندی=p_دلائل->ہدف_حد_بندی;
    offsets=p_دلائل->offsets;

    /* get the converter state from UConverter */
    پچھلا = static_cast<int32_t>(cnv->مقصد_Unicode_حالت);
    اگر(پچھلا==0) {
        پچھلا=BOCU1_ASCII_پچھلا;
    }
    diff=cnv->منوال; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_اشاریہ=cnv->مقصد_ULength;
    بائٹیں=cnv->مقصد_UBytes;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ=byte_اشاریہ==0 ? 0 : -1;
    اگلا_ماخذ_اشاریہ=0;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    اگر(count>0 && byte_اشاریہ>0 && ہدف<ہدف_حد_بندی) {
        جاؤ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(ماخذ_حد_بندی - ماخذ);
    count = static_cast<int32_t>(p_دلائل->ہدف_حد_بندی - ہدف);
    اگر(count>diff) {
        count=diff;
    }
    جب(count>0) {
        اگر(BOCU1_آغاز_NEG_2<=(c=*ماخذ) && c<BOCU1_آغاز_POS_2) {
            c=پچھلا+(c-BOCU1_درمیان);
            اگر(c<0x3000) {
                *ہدف++ = static_cast<char16_t>(c);
                *offsets++=اگلا_ماخذ_اشاریہ++;
                پچھلا=BOCU1_SIMPLE_پچھلا(c);
            } ورنہ {
                توڑو;
            }
        } ورنہ اگر(c<=0x20) {
            اگر(c!=0x20) {
                پچھلا=BOCU1_ASCII_پچھلا;
            }
            *ہدف++ = static_cast<char16_t>(c);
            *offsets++=اگلا_ماخذ_اشاریہ++;
        } ورنہ {
            توڑو;
        }
        ++ماخذ;
        --count;
    }
    ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ; /* wrong if offsets==nullptr but does not matter */

    /* decode a sequence of single and lead bytes */
    جب(ماخذ<ماخذ_حد_بندی) {
        اگر(ہدف>=ہدف_حد_بندی) {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }

        ++اگلا_ماخذ_اشاریہ;
        c=*ماخذ++;
        اگر(BOCU1_آغاز_NEG_2<=c && c<BOCU1_آغاز_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=پچھلا+(c-BOCU1_درمیان);
            اگر(c<0x3000) {
                *ہدف++ = static_cast<char16_t>(c);
                *offsets++=ماخذ_اشاریہ;
                پچھلا=BOCU1_SIMPLE_پچھلا(c);
                ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                جاؤ fastSingle;
            }
        } ورنہ اگر(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            اگر(c!=0x20) {
                پچھلا=BOCU1_ASCII_پچھلا;
            }
            *ہدف++ = static_cast<char16_t>(c);
            *offsets++=ماخذ_اشاریہ;
            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
            جاری;
        } ورنہ اگر(BOCU1_آغاز_NEG_3<=c && c<BOCU1_آغاز_POS_3 && ماخذ<ماخذ_حد_بندی) {
            /* Optimize two-byte case. */
            اگر(c>=BOCU1_درمیان) {
                diff = (c - BOCU1_آغاز_POS_2) * BOCU1_TRAIL_گنتی + BOCU1_رسائی_POS_1 + 1;
            } ورنہ {
                diff = (c - BOCU1_آغاز_NEG_2) * BOCU1_TRAIL_گنتی + BOCU1_رسائی_NEG_1;
            }

            /* trail byte */
            ++اگلا_ماخذ_اشاریہ;
            c=decodeBocu1TrailByte(1, *ماخذ++);
            اگر (c < 0 || static_cast<uint32_t>(c = پچھلا + diff + c) > 0x10ffff) {
                بائٹیں[0]=ماخذ[-2];
                بائٹیں[1]=ماخذ[-1];
                byte_اشاریہ=2;
                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                توڑو;
            }
        } ورنہ اگر(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            پچھلا=BOCU1_ASCII_پچھلا;
            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
            جاری;
        } ورنہ {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            بائٹیں[0] = static_cast<uint8_t>(c);
            byte_اشاریہ=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            برائے(;;) {
                اگر(ماخذ>=ماخذ_حد_بندی) {
                    جاؤ endloop;
                }
                ++اگلا_ماخذ_اشاریہ;
                c=بائٹیں[byte_اشاریہ++]=*ماخذ++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                اگر(c<0) {
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    جاؤ endloop;
                }

                diff+=c;
                اگر(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_اشاریہ=0;
                    c=پچھلا+diff;
                    اگر (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                        جاؤ endloop;
                    }
                    توڑو;
                }
            }
        }

        /* calculate the next prev and output c */
        پچھلا=BOCU1_پچھلا(c);
        اگر(c<=0xffff) {
            *ہدف++ = static_cast<char16_t>(c);
            *offsets++=ماخذ_اشاریہ;
        } ورنہ {
            /* output surrogate pair */
            *ہدف++=U16_LEAD(c);
            اگر(ہدف<ہدف_حد_بندی) {
                *ہدف++=U16_TRAIL(c);
                *offsets++=ماخذ_اشاریہ;
                *offsets++=ماخذ_اشاریہ;
            } ورنہ {
                /* target overflow */
                *offsets++=ماخذ_اشاریہ;
                cnv->UChar_Error_بفر[0]=U16_TRAIL(c);
                cnv->UChar_Error_بفر_لمبائی=1;
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }
        }
        ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
    }
endloop:

    اگر(*p_Error_رمز==U_ILLEGAL_CHAR_ملا) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->مقصد_Unicode_حالت=BOCU1_ASCII_پچھلا;
        cnv->منوال=0;
    } ورنہ {
        /* set the converter state back into UConverter */
        cnv->مقصد_Unicode_حالت = static_cast<uint32_t>(پچھلا);
        cnv->منوال = static_cast<int32_t>(static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->مقصد_ULength=byte_اشاریہ;

    /* write back the updated pointers */
    p_دلائل->ماخذ = reinterpret_cast<ثابت حرف*>(ماخذ);
    p_دلائل->ہدف=ہدف;
    p_دلائل->offsets=offsets;
}

/*
 * Identical to _Bocu1ToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ساکن خالی U_CALLCONV
Bocu1_مقصد_Unicode(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    char16_t *ہدف;
    ثابت char16_t *ہدف_حد_بندی;

    int32_t پچھلا, count, diff, c;

    int8_t byte_اشاریہ;
    uint8_t *بائٹیں;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    ماخذ = reinterpret_cast<ثابت uint8_t*>(p_دلائل->ماخذ);
    ماخذ_حد_بندی = reinterpret_cast<ثابت uint8_t*>(p_دلائل->ماخذ_حد_بندی);
    ہدف=p_دلائل->ہدف;
    ہدف_حد_بندی=p_دلائل->ہدف_حد_بندی;

    /* get the converter state from UConverter */
    پچھلا = static_cast<int32_t>(cnv->مقصد_Unicode_حالت);
    اگر(پچھلا==0) {
        پچھلا=BOCU1_ASCII_پچھلا;
    }
    diff=cnv->منوال; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_اشاریہ=cnv->مقصد_ULength;
    بائٹیں=cnv->مقصد_UBytes;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    اگر(count>0 && byte_اشاریہ>0 && ہدف<ہدف_حد_بندی) {
        جاؤ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(ماخذ_حد_بندی - ماخذ);
    count = static_cast<int32_t>(p_دلائل->ہدف_حد_بندی - ہدف);
    اگر(count>diff) {
        count=diff;
    }
    جب(count>0) {
        اگر(BOCU1_آغاز_NEG_2<=(c=*ماخذ) && c<BOCU1_آغاز_POS_2) {
            c=پچھلا+(c-BOCU1_درمیان);
            اگر(c<0x3000) {
                *ہدف++ = static_cast<char16_t>(c);
                پچھلا=BOCU1_SIMPLE_پچھلا(c);
            } ورنہ {
                توڑو;
            }
        } ورنہ اگر(c<=0x20) {
            اگر(c!=0x20) {
                پچھلا=BOCU1_ASCII_پچھلا;
            }
            *ہدف++ = static_cast<char16_t>(c);
        } ورنہ {
            توڑو;
        }
        ++ماخذ;
        --count;
    }

    /* decode a sequence of single and lead bytes */
    جب(ماخذ<ماخذ_حد_بندی) {
        اگر(ہدف>=ہدف_حد_بندی) {
            /* target is full */
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }

        c=*ماخذ++;
        اگر(BOCU1_آغاز_NEG_2<=c && c<BOCU1_آغاز_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=پچھلا+(c-BOCU1_درمیان);
            اگر(c<0x3000) {
                *ہدف++ = static_cast<char16_t>(c);
                پچھلا=BOCU1_SIMPLE_پچھلا(c);
                جاؤ fastSingle;
            }
        } ورنہ اگر(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            اگر(c!=0x20) {
                پچھلا=BOCU1_ASCII_پچھلا;
            }
            *ہدف++ = static_cast<char16_t>(c);
            جاری;
        } ورنہ اگر(BOCU1_آغاز_NEG_3<=c && c<BOCU1_آغاز_POS_3 && ماخذ<ماخذ_حد_بندی) {
            /* Optimize two-byte case. */
            اگر(c>=BOCU1_درمیان) {
                diff = (c - BOCU1_آغاز_POS_2) * BOCU1_TRAIL_گنتی + BOCU1_رسائی_POS_1 + 1;
            } ورنہ {
                diff = (c - BOCU1_آغاز_NEG_2) * BOCU1_TRAIL_گنتی + BOCU1_رسائی_NEG_1;
            }

            /* trail byte */
            c=decodeBocu1TrailByte(1, *ماخذ++);
            اگر (c < 0 || static_cast<uint32_t>(c = پچھلا + diff + c) > 0x10ffff) {
                بائٹیں[0]=ماخذ[-2];
                بائٹیں[1]=ماخذ[-1];
                byte_اشاریہ=2;
                *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                توڑو;
            }
        } ورنہ اگر(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            پچھلا=BOCU1_ASCII_پچھلا;
            جاری;
        } ورنہ {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            بائٹیں[0] = static_cast<uint8_t>(c);
            byte_اشاریہ=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            برائے(;;) {
                اگر(ماخذ>=ماخذ_حد_بندی) {
                    جاؤ endloop;
                }
                c=بائٹیں[byte_اشاریہ++]=*ماخذ++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                اگر(c<0) {
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    جاؤ endloop;
                }

                diff+=c;
                اگر(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_اشاریہ=0;
                    c=پچھلا+diff;
                    اگر (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                        جاؤ endloop;
                    }
                    توڑو;
                }
            }
        }

        /* calculate the next prev and output c */
        پچھلا=BOCU1_پچھلا(c);
        اگر(c<=0xffff) {
            *ہدف++ = static_cast<char16_t>(c);
        } ورنہ {
            /* output surrogate pair */
            *ہدف++=U16_LEAD(c);
            اگر(ہدف<ہدف_حد_بندی) {
                *ہدف++=U16_TRAIL(c);
            } ورنہ {
                /* target overflow */
                cnv->UChar_Error_بفر[0]=U16_TRAIL(c);
                cnv->UChar_Error_بفر_لمبائی=1;
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }
        }
    }
endloop:

    اگر(*p_Error_رمز==U_ILLEGAL_CHAR_ملا) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->مقصد_Unicode_حالت=BOCU1_ASCII_پچھلا;
        cnv->منوال=0;
    } ورنہ {
        /* set the converter state back into UConverter */
        cnv->مقصد_Unicode_حالت = static_cast<uint32_t>(پچھلا);
        cnv->منوال = (static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->مقصد_ULength=byte_اشاریہ;

    /* write back the updated pointers */
    p_دلائل->ماخذ = reinterpret_cast<ثابت حرف*>(ماخذ);
    p_دلائل->ہدف=ہدف;
}

/* miscellaneous ------------------------------------------------------------ */

ساکن ثابت UConverterImpl _Bocu1Impl={
    UCNV_BOCU1,

    خالی_اشاریہ,
    خالی_اشاریہ,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,

    Bocu1_مقصد_Unicode,
    Bocu1_مقصد_Unicode_With_Offsets,
    _Bocu1FromUnicode,
    _Bocu1FromUnicodeWithOffsets,
    خالی_اشاریہ,

    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    خالی_اشاریہ,
    ucnv_get_Complete_Unicode_سیٹ,

    خالی_اشاریہ,
    خالی_اشاریہ
};

ساکن ثابت UConverter_Static_ڈیٹا Bocu1_Static_ڈیٹا={
    حجم(UConverter_Static_ڈیٹا),
    "BOCU-1",
    1214, /* CCSID for BOCU-1 */
    UCNV_IBM, UCNV_BOCU1,
    1, 4, /* one char16_t generates at least 1 byte and at most 4 bytes */
    { 0x1a, 0, 0, 0 }, 1, /* BOCU-1 never needs to write a subchar */
    جھوٹ, جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_ڈیٹا Bocu1_ڈیٹا=
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&Bocu1_Static_ڈیٹا, &_Bocu1Impl);

#اگر_ختم
