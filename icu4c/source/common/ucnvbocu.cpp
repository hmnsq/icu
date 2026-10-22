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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/ucnv_cb.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "uassert.h"

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
#परिभाषय BOCU1_ASCII_पूर्व        0x40

/* bounding byte values for differences */
#परिभाषय BOCU1_MIN               0x21
#परिभाषय BOCU1_मध्यः            0x90
#परिभाषय BOCU1_MAX_LEAD          0xfe
#परिभाषय BOCU1_MAX_TRAIL         0xff
#परिभाषय BOCU1_RESET             0xff

/* number of lead bytes */
#परिभाषय BOCU1_गणनम्             (BOCU1_MAX_LEAD-BOCU1_MIN+1)

/* adjust trail byte counts for the use of some C0 control byte values */
#परिभाषय BOCU1_TRAIL_CONTROLS_गणनम्  20
#परिभाषय BOCU1_TRAIL_BYTE_विस्थापनम्     (BOCU1_MIN-BOCU1_TRAIL_CONTROLS_गणनम्)

/* number of trail bytes */
#परिभाषय BOCU1_TRAIL_गणनम्       ((BOCU1_MAX_TRAIL-BOCU1_MIN+1)+BOCU1_TRAIL_CONTROLS_गणनम्)

/*
 * number of positive and negative single-byte codes
 * (counting 0==BOCU1_MIDDLE among the positive ones)
 */
#परिभाषय BOCU1_SINGLE            64

/* number of lead bytes for positive and negative 2/3/4-byte sequences */
#परिभाषय BOCU1_LEAD_2            43
#परिभाषय BOCU1_LEAD_3            3
#परिभाषय BOCU1_LEAD_4            1

/* The difference value range for single-byters. */
#परिभाषय BOCU1_प्रापणम्_POS_1   (BOCU1_SINGLE-1)
#परिभाषय BOCU1_प्रापणम्_NEG_1   (-BOCU1_SINGLE)

/* The difference value range for double-byters. */
#परिभाषय BOCU1_प्रापणम्_POS_2   (BOCU1_प्रापणम्_POS_1+BOCU1_LEAD_2*BOCU1_TRAIL_गणनम्)
#परिभाषय BOCU1_प्रापणम्_NEG_2   (BOCU1_प्रापणम्_NEG_1-BOCU1_LEAD_2*BOCU1_TRAIL_गणनम्)

/* The difference value range for 3-byters. */
#परिभाषय BOCU1_प्रापणम्_POS_3   \
    (BOCU1_प्रापणम्_POS_2+BOCU1_LEAD_3*BOCU1_TRAIL_गणनम्*BOCU1_TRAIL_गणनम्)

#परिभाषय BOCU1_प्रापणम्_NEG_3   (BOCU1_प्रापणम्_NEG_2-BOCU1_LEAD_3*BOCU1_TRAIL_गणनम्*BOCU1_TRAIL_गणनम्)

/* The lead byte start values. */
#परिभाषय BOCU1_START_POS_2   (BOCU1_मध्यः+BOCU1_प्रापणम्_POS_1+1)
#परिभाषय BOCU1_START_POS_3   (BOCU1_START_POS_2+BOCU1_LEAD_2)
#परिभाषय BOCU1_START_POS_4   (BOCU1_START_POS_3+BOCU1_LEAD_3)
     /* ==BOCU1_MAX_LEAD */

#परिभाषय BOCU1_START_NEG_2   (BOCU1_मध्यः+BOCU1_प्रापणम्_NEG_1)
#परिभाषय BOCU1_START_NEG_3   (BOCU1_START_NEG_2-BOCU1_LEAD_2)
#परिभाषय BOCU1_START_NEG_4   (BOCU1_START_NEG_3-BOCU1_LEAD_3)
     /* ==BOCU1_MIN+1 */

/* The length of a byte sequence, according to the lead byte (!=BOCU1_RESET). */
#परिभाषय BOCU1_दीर्घता_FROM_LEAD(lead) \
    ((BOCU1_START_NEG_2<=(lead) && (lead)<BOCU1_START_POS_2) ? 1 : \
     (BOCU1_START_NEG_3<=(lead) && (lead)<BOCU1_START_POS_3) ? 2 : \
     (BOCU1_START_NEG_4<=(lead) && (lead)<BOCU1_START_POS_4) ? 3 : 4)

/* The length of a byte sequence, according to its packed form. */
#परिभाषय BOCU1_दीर्घता_FROM_PACKED(packed) \
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
#परिभाषय BOCU1_TRAIL_गन्तव्यम्_BYTE(t) ((t)>=BOCU1_TRAIL_CONTROLS_गणनम् ? (t)+BOCU1_TRAIL_BYTE_विस्थापनम् : bocu1_Trail_गन्तव्यम्_Byte[t])

/*
 * Byte value map for control codes,
 * from external byte values 0x00..0x20
 * to trail byte values 0..19 (0..0x13) as used in the difference calculation.
 * External byte values that are illegal as trail bytes are mapped to -1.
 */
स्थिर ध्रुव int8_t
bocu1_Byte_गन्तव्यम्_Trail[BOCU1_MIN]={
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
स्थिर ध्रुव int8_t
bocu1_Trail_गन्तव्यम्_Byte[BOCU1_TRAIL_CONTROLS_गणनम्]={
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
#परिभाषय NEGDIVMOD(n, d, m) UPRV_खण्डम्_MACRO_BEGIN { \
    (m)=(n)%(d); \
    (n)/=(d); \
    यदि((m)<0) { \
        --(n); \
        (m)+=(d); \
    } \
} UPRV_खण्डम्_MACRO_अन्तः

/* Faster versions of packDiff() for single-byte-encoded diff values. */

/** Is a diff value encodable in a single byte? */
#परिभाषय DIFF_IS_SINGLE(diff) (BOCU1_प्रापणम्_NEG_1<=(diff) && (diff)<=BOCU1_प्रापणम्_POS_1)

/** Encode a diff value in a single byte. */
#परिभाषय PACK_SINGLE_DIFF(diff) (BOCU1_मध्यः+(diff))

/** Is a diff value encodable in two bytes? */
#परिभाषय DIFF_IS_DOUBLE(diff) (BOCU1_प्रापणम्_NEG_2<=(diff) && (diff)<=BOCU1_प्रापणम्_POS_2)

/* BOCU-1 implementation functions ------------------------------------------ */

#परिभाषय BOCU1_SIMPLE_पूर्व(c) (((c)&~0x7f)+BOCU1_ASCII_पूर्व)

/**
 * Compute the next "previous" value for differencing
 * from the current code point.
 *
 * @param c current code point, 0x3040..0xd7a3 (rest handled by macro below)
 * @return "previous code point" state value
 */
स्थिर अन्तःस्थ int32_t
bocu1_पूर्व(int32_t c) {
    /* compute new prev */
    यदि(/* 0x3040<=c && */ c<=0x309f) {
        /* Hiragana is not 128-aligned */
        प्रत्यागम 0x3070;
    } अन्यथा यदि(0x4e00<=c && c<=0x9fa5) {
        /* CJK Unihan */
        प्रत्यागम 0x4e00-BOCU1_प्रापणम्_NEG_2;
    } अन्यथा यदि(0xac00<=c /* && c<=0xd7a3 */) {
        /* Korean Hangul */
        प्रत्यागम (0xd7a3+0xac00)/2;
    } अन्यथा {
        /* mostly small scripts */
        प्रत्यागम BOCU1_SIMPLE_पूर्व(c);
    }
}

/** Fast version of bocu1Prev() for most scripts. */
#परिभाषय BOCU1_पूर्व(c) ((c)<0x3040 || (c)>0xd7a3 ? BOCU1_SIMPLE_पूर्व(c) : bocu1_पूर्व(c))

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
स्थिर int32_t
packDiff(int32_t diff) {
    int32_t परिणाम, m;

    U_ASSERT(!DIFF_IS_SINGLE(diff)); /* assume we won't be called where diff==BOCU1_REACH_NEG_1=-64 */
    यदि(diff>=BOCU1_प्रापणम्_NEG_1) {
        /* mostly positive differences, and single-byte negative ones */
#यदि 0   /* single-byte case handled in macros, see below */
        यदि(diff<=BOCU1_प्रापणम्_POS_1) {
            /* single byte */
            प्रत्यागम 0x01000000|(BOCU1_मध्यः+diff);
        } अन्यथा
#यदि_अन्तः
        यदि(diff<=BOCU1_प्रापणम्_POS_2) {
            /* two bytes */
            diff-=BOCU1_प्रापणम्_POS_1+1;
            परिणाम=0x02000000;

            m=diff%BOCU1_TRAIL_गणनम्;
            diff/=BOCU1_TRAIL_गणनम्;
            परिणाम|=BOCU1_TRAIL_गन्तव्यम्_BYTE(m);

            परिणाम|=(BOCU1_START_POS_2+diff)<<8;
        } अन्यथा यदि(diff<=BOCU1_प्रापणम्_POS_3) {
            /* three bytes */
            diff-=BOCU1_प्रापणम्_POS_2+1;
            परिणाम=0x03000000;

            m=diff%BOCU1_TRAIL_गणनम्;
            diff/=BOCU1_TRAIL_गणनम्;
            परिणाम|=BOCU1_TRAIL_गन्तव्यम्_BYTE(m);

            m=diff%BOCU1_TRAIL_गणनम्;
            diff/=BOCU1_TRAIL_गणनम्;
            परिणाम|=BOCU1_TRAIL_गन्तव्यम्_BYTE(m)<<8;

            परिणाम|=(BOCU1_START_POS_3+diff)<<16;
        } अन्यथा {
            /* four bytes */
            diff-=BOCU1_प्रापणम्_POS_3+1;

            m=diff%BOCU1_TRAIL_गणनम्;
            diff/=BOCU1_TRAIL_गणनम्;
            परिणाम=BOCU1_TRAIL_गन्तव्यम्_BYTE(m);

            m=diff%BOCU1_TRAIL_गणनम्;
            diff/=BOCU1_TRAIL_गणनम्;
            परिणाम|=BOCU1_TRAIL_गन्तव्यम्_BYTE(m)<<8;

            /*
             * We know that / and % would deliver quotient 0 and rest=diff.
             * Avoid division and modulo for performance.
             */
            परिणाम|=BOCU1_TRAIL_गन्तव्यम्_BYTE(diff)<<16;

            परिणाम |= static_cast<uint32_t>(BOCU1_START_POS_4) << 24;
        }
    } अन्यथा {
        /* two- to four-byte negative differences */
        यदि(diff>=BOCU1_प्रापणम्_NEG_2) {
            /* two bytes */
            diff-=BOCU1_प्रापणम्_NEG_1;
            परिणाम=0x02000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_गणनम्, m);
            परिणाम|=BOCU1_TRAIL_गन्तव्यम्_BYTE(m);

            परिणाम|=(BOCU1_START_NEG_2+diff)<<8;
        } अन्यथा यदि(diff>=BOCU1_प्रापणम्_NEG_3) {
            /* three bytes */
            diff-=BOCU1_प्रापणम्_NEG_2;
            परिणाम=0x03000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_गणनम्, m);
            परिणाम|=BOCU1_TRAIL_गन्तव्यम्_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_गणनम्, m);
            परिणाम|=BOCU1_TRAIL_गन्तव्यम्_BYTE(m)<<8;

            परिणाम|=(BOCU1_START_NEG_3+diff)<<16;
        } अन्यथा {
            /* four bytes */
            diff-=BOCU1_प्रापणम्_NEG_3;

            NEGDIVMOD(diff, BOCU1_TRAIL_गणनम्, m);
            परिणाम=BOCU1_TRAIL_गन्तव्यम्_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_गणनम्, m);
            परिणाम|=BOCU1_TRAIL_गन्तव्यम्_BYTE(m)<<8;

            /*
             * We know that NEGDIVMOD would deliver
             * quotient -1 and rest=diff+BOCU1_TRAIL_COUNT.
             * Avoid division and modulo for performance.
             */
            m=diff+BOCU1_TRAIL_गणनम्;
            परिणाम|=BOCU1_TRAIL_गन्तव्यम्_BYTE(m)<<16;

            परिणाम|=BOCU1_MIN<<24;
        }
    }
    प्रत्यागम परिणाम;
}


स्थिर शून्य U_CALLCONV
_Bocu1FromUnicodeWithOffsets(UConverter_From_Unicode_तर्काः *p_तर्काः,
                             UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव char16_t *source, *source_सीमा;
    uint8_t *target;
    int32_t target_क्षमता;
    int32_t *offsets;

    int32_t पूर्व, c, diff;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    source=p_तर्काः->source;
    source_सीमा=p_तर्काः->source_सीमा;
    target = reinterpret_cast<uint8_t*>(p_तर्काः->target);
    target_क्षमता = static_cast<int32_t>(p_तर्काः->target_सीमा - p_तर्काः->target);
    offsets=p_तर्काः->offsets;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    पूर्व = static_cast<int32_t>(cnv->from_Unicode_स्थितिः);
    यदि(पूर्व==0) {
        पूर्व=BOCU1_ASCII_पूर्व;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक= c==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    /* conversion loop */
    यदि(c!=0 && target_क्षमता>0) {
        गच्छ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(source_सीमा - source);
    यदि(target_क्षमता>diff) {
        target_क्षमता=diff;
    }
    यावत्(target_क्षमता>0 && (c=*source)<0x3000) {
        यदि(c<=0x20) {
            यदि(c!=0x20) {
                पूर्व=BOCU1_ASCII_पूर्व;
            }
            *target++ = static_cast<uint8_t>(c);
            *offsets++=अग्र_Source_सूचकांक++;
            ++source;
            --target_क्षमता;
        } अन्यथा {
            diff=c-पूर्व;
            यदि(DIFF_IS_SINGLE(diff)) {
                पूर्व=BOCU1_SIMPLE_पूर्व(c);
                *target++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=अग्र_Source_सूचकांक++;
                ++source;
                --target_क्षमता;
            } अन्यथा {
                भङ्ग;
            }
        }
    }
    /* restore real values */
    target_क्षमता = static_cast<int32_t>(reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->target_सीमा) - target);
    source_सूचकांक=अग्र_Source_सूचकांक; /* wrong if offsets==nullptr but does not matter */

    /* regular loop for all cases */
    यावत्(source<source_सीमा) {
        यदि(target_क्षमता>0) {
            c=*source++;
            ++अग्र_Source_सूचकांक;

            यदि(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                यदि(c!=0x20) {
                    पूर्व=BOCU1_ASCII_पूर्व;
                }
                *target++ = static_cast<uint8_t>(c);
                *offsets++=source_सूचकांक;
                --target_क्षमता;

                source_सूचकांक=अग्र_Source_सूचकांक;
                अनुवर्त;
            }

            यदि(U16_IS_LEAD(c)) {
getTrail:
                यदि(source<source_सीमा) {
                    /* test the following code unit */
                    char16_t trail=*source;
                    यदि(U16_IS_TRAIL(trail)) {
                        ++source;
                        ++अग्र_Source_सूचकांक;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } अन्यथा {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    भङ्ग;
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
            diff=c-पूर्व;
            पूर्व=BOCU1_पूर्व(c);
            यदि(DIFF_IS_SINGLE(diff)) {
                *target++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=source_सूचकांक;
                --target_क्षमता;
                source_सूचकांक=अग्र_Source_सूचकांक;
                यदि(c<0x3000) {
                    गच्छ fastSingle;
                }
            } अन्यथा यदि(DIFF_IS_DOUBLE(diff) && 2<=target_क्षमता) {
                /* optimize 2-byte case */
                int32_t m;

                यदि(diff>=0) {
                    diff-=BOCU1_प्रापणम्_POS_1+1;
                    m=diff%BOCU1_TRAIL_गणनम्;
                    diff/=BOCU1_TRAIL_गणनम्;
                    diff+=BOCU1_START_POS_2;
                } अन्यथा {
                    diff-=BOCU1_प्रापणम्_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_गणनम्, m);
                    diff+=BOCU1_START_NEG_2;
                }
                *target++ = static_cast<uint8_t>(diff);
                *target++ = static_cast<uint8_t>(BOCU1_TRAIL_गन्तव्यम्_BYTE(m));
                *offsets++=source_सूचकांक;
                *offsets++=source_सूचकांक;
                target_क्षमता-=2;
                source_सूचकांक=अग्र_Source_सूचकांक;
            } अन्यथा {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_दीर्घता_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                यदि(length<=target_क्षमता) {
                    विकल्प(length) {
                        /* each branch falls through to the next one */
                    पक्ष 4:
                        *target++ = static_cast<uint8_t>(diff >> 24);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    पक्ष 3:
                        *target++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    पक्ष 2:
                        *target++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=source_सूचकांक;
                    /* case 1: handled above */
                        *target++ = static_cast<uint8_t>(diff);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    स्वभाव:
                        /* will never occur */
                        भङ्ग;
                    }
                    target_क्षमता-=length;
                    source_सूचकांक=अग्र_Source_सूचकांक;
                } अन्यथा {
                    uint8_t *char_Error_बफर;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=target_क्षमता;
                    char_Error_बफर=(uint8_t *)cnv->char_Error_बफर;
                    विकल्प(length) {
                        /* each branch falls through to the next one */
                    पक्ष 3:
                        *char_Error_बफर++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    पक्ष 2:
                        *char_Error_बफर++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    पक्ष 1:
                        *char_Error_बफर = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    स्वभाव:
                        /* will never occur */
                        भङ्ग;
                    }
                    cnv->char_Error_बफर_दीर्घता = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    विकल्प(target_क्षमता) {
                        /* each branch falls through to the next one */
                    पक्ष 3:
                        *target++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    पक्ष 2:
                        *target++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    पक्ष 1:
                        *target++ = static_cast<uint8_t>(diff);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    स्वभाव:
                        /* will never occur */
                        भङ्ग;
                    }

                    /* target overflow */
                    target_क्षमता=0;
                    *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                    भङ्ग;
                }
            }
        } अन्यथा {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_स्थितिः = static_cast<uint32_t>(पूर्व);

    /* write back the updated pointers */
    p_तर्काः->source=source;
    p_तर्काः->target = reinterpret_cast<वर्ण*>(target);
    p_तर्काः->offsets=offsets;
}

/*
 * Identical to _Bocu1FromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
स्थिर शून्य U_CALLCONV
_Bocu1FromUnicode(UConverter_From_Unicode_तर्काः *p_तर्काः,
                  UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव char16_t *source, *source_सीमा;
    uint8_t *target;
    int32_t target_क्षमता;

    int32_t पूर्व, c, diff;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    source=p_तर्काः->source;
    source_सीमा=p_तर्काः->source_सीमा;
    target = reinterpret_cast<uint8_t*>(p_तर्काः->target);
    target_क्षमता = static_cast<int32_t>(p_तर्काः->target_सीमा - p_तर्काः->target);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    पूर्व = static_cast<int32_t>(cnv->from_Unicode_स्थितिः);
    यदि(पूर्व==0) {
        पूर्व=BOCU1_ASCII_पूर्व;
    }

    /* conversion loop */
    यदि(c!=0 && target_क्षमता>0) {
        गच्छ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(source_सीमा - source);
    यदि(target_क्षमता>diff) {
        target_क्षमता=diff;
    }
    यावत्(target_क्षमता>0 && (c=*source)<0x3000) {
        यदि(c<=0x20) {
            यदि(c!=0x20) {
                पूर्व=BOCU1_ASCII_पूर्व;
            }
            *target++ = static_cast<uint8_t>(c);
        } अन्यथा {
            diff=c-पूर्व;
            यदि(DIFF_IS_SINGLE(diff)) {
                पूर्व=BOCU1_SIMPLE_पूर्व(c);
                *target++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
            } अन्यथा {
                भङ्ग;
            }
        }
        ++source;
        --target_क्षमता;
    }
    /* restore real values */
    target_क्षमता = static_cast<int32_t>(reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->target_सीमा) - target);

    /* regular loop for all cases */
    यावत्(source<source_सीमा) {
        यदि(target_क्षमता>0) {
            c=*source++;

            यदि(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                यदि(c!=0x20) {
                    पूर्व=BOCU1_ASCII_पूर्व;
                }
                *target++ = static_cast<uint8_t>(c);
                --target_क्षमता;
                अनुवर्त;
            }

            यदि(U16_IS_LEAD(c)) {
getTrail:
                यदि(source<source_सीमा) {
                    /* test the following code unit */
                    char16_t trail=*source;
                    यदि(U16_IS_TRAIL(trail)) {
                        ++source;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } अन्यथा {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    भङ्ग;
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
            diff=c-पूर्व;
            पूर्व=BOCU1_पूर्व(c);
            यदि(DIFF_IS_SINGLE(diff)) {
                *target++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                --target_क्षमता;
                यदि(c<0x3000) {
                    गच्छ fastSingle;
                }
            } अन्यथा यदि(DIFF_IS_DOUBLE(diff) && 2<=target_क्षमता) {
                /* optimize 2-byte case */
                int32_t m;

                यदि(diff>=0) {
                    diff-=BOCU1_प्रापणम्_POS_1+1;
                    m=diff%BOCU1_TRAIL_गणनम्;
                    diff/=BOCU1_TRAIL_गणनम्;
                    diff+=BOCU1_START_POS_2;
                } अन्यथा {
                    diff-=BOCU1_प्रापणम्_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_गणनम्, m);
                    diff+=BOCU1_START_NEG_2;
                }
                *target++ = static_cast<uint8_t>(diff);
                *target++ = static_cast<uint8_t>(BOCU1_TRAIL_गन्तव्यम्_BYTE(m));
                target_क्षमता-=2;
            } अन्यथा {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_दीर्घता_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                यदि(length<=target_क्षमता) {
                    विकल्प(length) {
                        /* each branch falls through to the next one */
                    पक्ष 4:
                        *target++ = static_cast<uint8_t>(diff >> 24);
                        U_FALLTHROUGH;
                    पक्ष 3:
                        *target++ = static_cast<uint8_t>(diff >> 16);
                    /* case 2: handled above */
                        *target++ = static_cast<uint8_t>(diff >> 8);
                    /* case 1: handled above */
                        *target++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    स्वभाव:
                        /* will never occur */
                        भङ्ग;
                    }
                    target_क्षमता-=length;
                } अन्यथा {
                    uint8_t *char_Error_बफर;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=target_क्षमता;
                    char_Error_बफर=(uint8_t *)cnv->char_Error_बफर;
                    विकल्प(length) {
                        /* each branch falls through to the next one */
                    पक्ष 3:
                        *char_Error_बफर++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    पक्ष 2:
                        *char_Error_बफर++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    पक्ष 1:
                        *char_Error_बफर = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    स्वभाव:
                        /* will never occur */
                        भङ्ग;
                    }
                    cnv->char_Error_बफर_दीर्घता = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    विकल्प(target_क्षमता) {
                        /* each branch falls through to the next one */
                    पक्ष 3:
                        *target++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    पक्ष 2:
                        *target++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    पक्ष 1:
                        *target++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    स्वभाव:
                        /* will never occur */
                        भङ्ग;
                    }

                    /* target overflow */
                    target_क्षमता=0;
                    *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                    भङ्ग;
                }
            }
        } अन्यथा {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->from_Unicode_स्थितिः = static_cast<uint32_t>(पूर्व);

    /* write back the updated pointers */
    p_तर्काः->source=source;
    p_तर्काः->target = reinterpret_cast<वर्ण*>(target);
}

/* BOCU-1-to-Unicode conversion functions ----------------------------------- */

/**
 * Function for BOCU-1 decoder; handles multi-byte lead bytes.
 *
 * @param b lead byte;
 *          BOCU1_MIN<=b<BOCU1_START_NEG_2 or BOCU1_START_POS_2<=b<BOCU1_MAX_LEAD
 * @return (diff<<2)|count
 */
स्थिर अन्तःस्थ int32_t
decodeBocu1LeadByte(int32_t b) {
    int32_t diff, count;

    यदि(b>=BOCU1_START_NEG_2) {
        /* positive difference */
        यदि(b<BOCU1_START_POS_3) {
            /* two bytes */
            diff = (b - BOCU1_START_POS_2) * BOCU1_TRAIL_गणनम् + BOCU1_प्रापणम्_POS_1 + 1;
            count=1;
        } अन्यथा यदि(b<BOCU1_START_POS_4) {
            /* three bytes */
            diff = (b - BOCU1_START_POS_3) * BOCU1_TRAIL_गणनम् * BOCU1_TRAIL_गणनम् + BOCU1_प्रापणम्_POS_2 + 1;
            count=2;
        } अन्यथा {
            /* four bytes */
            diff=BOCU1_प्रापणम्_POS_3+1;
            count=3;
        }
    } अन्यथा {
        /* negative difference */
        यदि(b>=BOCU1_START_NEG_3) {
            /* two bytes */
            diff = (b - BOCU1_START_NEG_2) * BOCU1_TRAIL_गणनम् + BOCU1_प्रापणम्_NEG_1;
            count=1;
        } अन्यथा यदि(b>BOCU1_MIN) {
            /* three bytes */
            diff = (b - BOCU1_START_NEG_3) * BOCU1_TRAIL_गणनम् * BOCU1_TRAIL_गणनम् + BOCU1_प्रापणम्_NEG_2;
            count=2;
        } अन्यथा {
            /* four bytes */
            diff=-BOCU1_TRAIL_गणनम्*BOCU1_TRAIL_गणनम्*BOCU1_TRAIL_गणनम्+BOCU1_प्रापणम्_NEG_3;
            count=3;
        }
    }

    /* return the state for decoding the trail byte(s) */
    प्रत्यागम (static_cast<uint32_t>(diff) << 2) | count;
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
स्थिर अन्तःस्थ int32_t
decodeBocu1TrailByte(int32_t count, int32_t b) {
    यदि(b<=0x20) {
        /* skip some C0 controls and make the trail byte range contiguous */
        b=bocu1_Byte_गन्तव्यम्_Trail[b];
        /* b<0 for an illegal trail byte value will result in return<0 below */
#यदि BOCU1_MAX_TRAIL<0xff
    } अन्यथा यदि(b>BOCU1_MAX_TRAIL) {
        प्रत्यागम -99;
#यदि_अन्तः
    } अन्यथा {
        b-=BOCU1_TRAIL_BYTE_विस्थापनम्;
    }

    /* add trail byte into difference and decrement count */
    यदि(count==1) {
        प्रत्यागम b;
    } अन्यथा यदि(count==2) {
        प्रत्यागम b*BOCU1_TRAIL_गणनम्;
    } अन्यथा /* count==3 */ {
        प्रत्यागम b*(BOCU1_TRAIL_गणनम्*BOCU1_TRAIL_गणनम्);
    }
}

स्थिर शून्य U_CALLCONV
Bocu1_गन्तव्यम्_Unicode_With_Offsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                           UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव uint8_t *source, *source_सीमा;
    char16_t *target;
    ध्रुव char16_t *target_सीमा;
    int32_t *offsets;

    int32_t पूर्व, count, diff, c;

    int8_t byte_सूचकांक;
    uint8_t *अष्टकानि;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    source = reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->source);
    source_सीमा = reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->source_सीमा);
    target=p_तर्काः->target;
    target_सीमा=p_तर्काः->target_सीमा;
    offsets=p_तर्काः->offsets;

    /* get the converter state from UConverter */
    पूर्व = static_cast<int32_t>(cnv->गन्तव्यम्_Unicode_स्थितिः);
    यदि(पूर्व==0) {
        पूर्व=BOCU1_ASCII_पूर्व;
    }
    diff=cnv->बहुलकम्; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_सूचकांक=cnv->गन्तव्यम्_ULength;
    अष्टकानि=cnv->गन्तव्यम्_UBytes;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक=byte_सूचकांक==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    यदि(count>0 && byte_सूचकांक>0 && target<target_सीमा) {
        गच्छ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(source_सीमा - source);
    count = static_cast<int32_t>(p_तर्काः->target_सीमा - target);
    यदि(count>diff) {
        count=diff;
    }
    यावत्(count>0) {
        यदि(BOCU1_START_NEG_2<=(c=*source) && c<BOCU1_START_POS_2) {
            c=पूर्व+(c-BOCU1_मध्यः);
            यदि(c<0x3000) {
                *target++ = static_cast<char16_t>(c);
                *offsets++=अग्र_Source_सूचकांक++;
                पूर्व=BOCU1_SIMPLE_पूर्व(c);
            } अन्यथा {
                भङ्ग;
            }
        } अन्यथा यदि(c<=0x20) {
            यदि(c!=0x20) {
                पूर्व=BOCU1_ASCII_पूर्व;
            }
            *target++ = static_cast<char16_t>(c);
            *offsets++=अग्र_Source_सूचकांक++;
        } अन्यथा {
            भङ्ग;
        }
        ++source;
        --count;
    }
    source_सूचकांक=अग्र_Source_सूचकांक; /* wrong if offsets==nullptr but does not matter */

    /* decode a sequence of single and lead bytes */
    यावत्(source<source_सीमा) {
        यदि(target>=target_सीमा) {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }

        ++अग्र_Source_सूचकांक;
        c=*source++;
        यदि(BOCU1_START_NEG_2<=c && c<BOCU1_START_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=पूर्व+(c-BOCU1_मध्यः);
            यदि(c<0x3000) {
                *target++ = static_cast<char16_t>(c);
                *offsets++=source_सूचकांक;
                पूर्व=BOCU1_SIMPLE_पूर्व(c);
                source_सूचकांक=अग्र_Source_सूचकांक;
                गच्छ fastSingle;
            }
        } अन्यथा यदि(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            यदि(c!=0x20) {
                पूर्व=BOCU1_ASCII_पूर्व;
            }
            *target++ = static_cast<char16_t>(c);
            *offsets++=source_सूचकांक;
            source_सूचकांक=अग्र_Source_सूचकांक;
            अनुवर्त;
        } अन्यथा यदि(BOCU1_START_NEG_3<=c && c<BOCU1_START_POS_3 && source<source_सीमा) {
            /* Optimize two-byte case. */
            यदि(c>=BOCU1_मध्यः) {
                diff = (c - BOCU1_START_POS_2) * BOCU1_TRAIL_गणनम् + BOCU1_प्रापणम्_POS_1 + 1;
            } अन्यथा {
                diff = (c - BOCU1_START_NEG_2) * BOCU1_TRAIL_गणनम् + BOCU1_प्रापणम्_NEG_1;
            }

            /* trail byte */
            ++अग्र_Source_सूचकांक;
            c=decodeBocu1TrailByte(1, *source++);
            यदि (c < 0 || static_cast<uint32_t>(c = पूर्व + diff + c) > 0x10ffff) {
                अष्टकानि[0]=source[-2];
                अष्टकानि[1]=source[-1];
                byte_सूचकांक=2;
                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                भङ्ग;
            }
        } अन्यथा यदि(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            पूर्व=BOCU1_ASCII_पूर्व;
            source_सूचकांक=अग्र_Source_सूचकांक;
            अनुवर्त;
        } अन्यथा {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            अष्टकानि[0] = static_cast<uint8_t>(c);
            byte_सूचकांक=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            प्रति(;;) {
                यदि(source>=source_सीमा) {
                    गच्छ endloop;
                }
                ++अग्र_Source_सूचकांक;
                c=अष्टकानि[byte_सूचकांक++]=*source++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                यदि(c<0) {
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    गच्छ endloop;
                }

                diff+=c;
                यदि(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_सूचकांक=0;
                    c=पूर्व+diff;
                    यदि (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                        गच्छ endloop;
                    }
                    भङ्ग;
                }
            }
        }

        /* calculate the next prev and output c */
        पूर्व=BOCU1_पूर्व(c);
        यदि(c<=0xffff) {
            *target++ = static_cast<char16_t>(c);
            *offsets++=source_सूचकांक;
        } अन्यथा {
            /* output surrogate pair */
            *target++=U16_LEAD(c);
            यदि(target<target_सीमा) {
                *target++=U16_TRAIL(c);
                *offsets++=source_सूचकांक;
                *offsets++=source_सूचकांक;
            } अन्यथा {
                /* target overflow */
                *offsets++=source_सूचकांक;
                cnv->UChar_Error_बफर[0]=U16_TRAIL(c);
                cnv->UChar_Error_बफर_दीर्घता=1;
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
        }
        source_सूचकांक=अग्र_Source_सूचकांक;
    }
endloop:

    यदि(*p_Error_संहिता==U_ILLEGAL_CHAR_प्राप्तम्) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->गन्तव्यम्_Unicode_स्थितिः=BOCU1_ASCII_पूर्व;
        cnv->बहुलकम्=0;
    } अन्यथा {
        /* set the converter state back into UConverter */
        cnv->गन्तव्यम्_Unicode_स्थितिः = static_cast<uint32_t>(पूर्व);
        cnv->बहुलकम् = static_cast<int32_t>(static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->गन्तव्यम्_ULength=byte_सूचकांक;

    /* write back the updated pointers */
    p_तर्काः->source = reinterpret_cast<ध्रुव वर्ण*>(source);
    p_तर्काः->target=target;
    p_तर्काः->offsets=offsets;
}

/*
 * Identical to _Bocu1ToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
स्थिर शून्य U_CALLCONV
Bocu1_गन्तव्यम्_Unicode(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    ध्रुव uint8_t *source, *source_सीमा;
    char16_t *target;
    ध्रुव char16_t *target_सीमा;

    int32_t पूर्व, count, diff, c;

    int8_t byte_सूचकांक;
    uint8_t *अष्टकानि;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    source = reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->source);
    source_सीमा = reinterpret_cast<ध्रुव uint8_t*>(p_तर्काः->source_सीमा);
    target=p_तर्काः->target;
    target_सीमा=p_तर्काः->target_सीमा;

    /* get the converter state from UConverter */
    पूर्व = static_cast<int32_t>(cnv->गन्तव्यम्_Unicode_स्थितिः);
    यदि(पूर्व==0) {
        पूर्व=BOCU1_ASCII_पूर्व;
    }
    diff=cnv->बहुलकम्; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_सूचकांक=cnv->गन्तव्यम्_ULength;
    अष्टकानि=cnv->गन्तव्यम्_UBytes;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    यदि(count>0 && byte_सूचकांक>0 && target<target_सीमा) {
        गच्छ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(source_सीमा - source);
    count = static_cast<int32_t>(p_तर्काः->target_सीमा - target);
    यदि(count>diff) {
        count=diff;
    }
    यावत्(count>0) {
        यदि(BOCU1_START_NEG_2<=(c=*source) && c<BOCU1_START_POS_2) {
            c=पूर्व+(c-BOCU1_मध्यः);
            यदि(c<0x3000) {
                *target++ = static_cast<char16_t>(c);
                पूर्व=BOCU1_SIMPLE_पूर्व(c);
            } अन्यथा {
                भङ्ग;
            }
        } अन्यथा यदि(c<=0x20) {
            यदि(c!=0x20) {
                पूर्व=BOCU1_ASCII_पूर्व;
            }
            *target++ = static_cast<char16_t>(c);
        } अन्यथा {
            भङ्ग;
        }
        ++source;
        --count;
    }

    /* decode a sequence of single and lead bytes */
    यावत्(source<source_सीमा) {
        यदि(target>=target_सीमा) {
            /* target is full */
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }

        c=*source++;
        यदि(BOCU1_START_NEG_2<=c && c<BOCU1_START_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=पूर्व+(c-BOCU1_मध्यः);
            यदि(c<0x3000) {
                *target++ = static_cast<char16_t>(c);
                पूर्व=BOCU1_SIMPLE_पूर्व(c);
                गच्छ fastSingle;
            }
        } अन्यथा यदि(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            यदि(c!=0x20) {
                पूर्व=BOCU1_ASCII_पूर्व;
            }
            *target++ = static_cast<char16_t>(c);
            अनुवर्त;
        } अन्यथा यदि(BOCU1_START_NEG_3<=c && c<BOCU1_START_POS_3 && source<source_सीमा) {
            /* Optimize two-byte case. */
            यदि(c>=BOCU1_मध्यः) {
                diff = (c - BOCU1_START_POS_2) * BOCU1_TRAIL_गणनम् + BOCU1_प्रापणम्_POS_1 + 1;
            } अन्यथा {
                diff = (c - BOCU1_START_NEG_2) * BOCU1_TRAIL_गणनम् + BOCU1_प्रापणम्_NEG_1;
            }

            /* trail byte */
            c=decodeBocu1TrailByte(1, *source++);
            यदि (c < 0 || static_cast<uint32_t>(c = पूर्व + diff + c) > 0x10ffff) {
                अष्टकानि[0]=source[-2];
                अष्टकानि[1]=source[-1];
                byte_सूचकांक=2;
                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                भङ्ग;
            }
        } अन्यथा यदि(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            पूर्व=BOCU1_ASCII_पूर्व;
            अनुवर्त;
        } अन्यथा {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            अष्टकानि[0] = static_cast<uint8_t>(c);
            byte_सूचकांक=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            प्रति(;;) {
                यदि(source>=source_सीमा) {
                    गच्छ endloop;
                }
                c=अष्टकानि[byte_सूचकांक++]=*source++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                यदि(c<0) {
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    गच्छ endloop;
                }

                diff+=c;
                यदि(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_सूचकांक=0;
                    c=पूर्व+diff;
                    यदि (static_cast<uint32_t>(c) > 0x10ffff) {
                        *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                        गच्छ endloop;
                    }
                    भङ्ग;
                }
            }
        }

        /* calculate the next prev and output c */
        पूर्व=BOCU1_पूर्व(c);
        यदि(c<=0xffff) {
            *target++ = static_cast<char16_t>(c);
        } अन्यथा {
            /* output surrogate pair */
            *target++=U16_LEAD(c);
            यदि(target<target_सीमा) {
                *target++=U16_TRAIL(c);
            } अन्यथा {
                /* target overflow */
                cnv->UChar_Error_बफर[0]=U16_TRAIL(c);
                cnv->UChar_Error_बफर_दीर्घता=1;
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
        }
    }
endloop:

    यदि(*p_Error_संहिता==U_ILLEGAL_CHAR_प्राप्तम्) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->गन्तव्यम्_Unicode_स्थितिः=BOCU1_ASCII_पूर्व;
        cnv->बहुलकम्=0;
    } अन्यथा {
        /* set the converter state back into UConverter */
        cnv->गन्तव्यम्_Unicode_स्थितिः = static_cast<uint32_t>(पूर्व);
        cnv->बहुलकम् = (static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->गन्तव्यम्_ULength=byte_सूचकांक;

    /* write back the updated pointers */
    p_तर्काः->source = reinterpret_cast<ध्रुव वर्ण*>(source);
    p_तर्काः->target=target;
}

/* miscellaneous ------------------------------------------------------------ */

स्थिर ध्रुव UConverterImpl _Bocu1Impl={
    UCNV_BOCU1,

    शून्यसूचकः,
    शून्यसूचकः,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,

    Bocu1_गन्तव्यम्_Unicode,
    Bocu1_गन्तव्यम्_Unicode_With_Offsets,
    _Bocu1FromUnicode,
    _Bocu1FromUnicodeWithOffsets,
    शून्यसूचकः,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    ucnv_get_Complete_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};

स्थिर ध्रुव UConverter_Static_दत्त Bocu1_Static_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "BOCU-1",
    1214, /* CCSID for BOCU-1 */
    UCNV_IBM, UCNV_BOCU1,
    1, 4, /* one char16_t generates at least 1 byte and at most 4 bytes */
    { 0x1a, 0, 0, 0 }, 1, /* BOCU-1 never needs to write a subchar */
    असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ध्रुव UConverter_Shared_दत्त Bocu1_दत्त=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&Bocu1_Static_दत्त, &_Bocu1Impl);

#यदि_अन्तः
