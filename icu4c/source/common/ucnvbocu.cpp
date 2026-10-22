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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#समावेश "unicode/ucnv.h"
#समावेश "unicode/ucnv_cb.h"
#समावेश "unicode/utf16.h"
#समावेश "putilimp.h"
#समावेश "ucnv_bld.h"
#समावेश "ucnv_cnv.h"
#समावेश "uassert.h"

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
#घोषणा BOCU1_ASCII_पूर्व        0x40

/* bounding byte values for differences */
#घोषणा BOCU1_MIN               0x21
#घोषणा BOCU1_मध्य            0x90
#घोषणा BOCU1_MAX_LEAD          0xfe
#घोषणा BOCU1_MAX_TRAIL         0xff
#घोषणा BOCU1_RESET             0xff

/* number of lead bytes */
#घोषणा BOCU1_गिनती             (BOCU1_MAX_LEAD-BOCU1_MIN+1)

/* adjust trail byte counts for the use of some C0 control byte values */
#घोषणा BOCU1_TRAIL_CONTROLS_गिनती  20
#घोषणा BOCU1_TRAIL_BYTE_OFFSET     (BOCU1_MIN-BOCU1_TRAIL_CONTROLS_गिनती)

/* number of trail bytes */
#घोषणा BOCU1_TRAIL_गिनती       ((BOCU1_MAX_TRAIL-BOCU1_MIN+1)+BOCU1_TRAIL_CONTROLS_गिनती)

/*
 * number of positive and negative single-byte codes
 * (counting 0==BOCU1_MIDDLE among the positive ones)
 */
#घोषणा BOCU1_SINGLE            64

/* number of lead bytes for positive and negative 2/3/4-byte sequences */
#घोषणा BOCU1_LEAD_2            43
#घोषणा BOCU1_LEAD_3            3
#घोषणा BOCU1_LEAD_4            1

/* The difference value range for single-byters. */
#घोषणा BOCU1_REACH_POS_1   (BOCU1_SINGLE-1)
#घोषणा BOCU1_REACH_NEG_1   (-BOCU1_SINGLE)

/* The difference value range for double-byters. */
#घोषणा BOCU1_REACH_POS_2   (BOCU1_REACH_POS_1+BOCU1_LEAD_2*BOCU1_TRAIL_गिनती)
#घोषणा BOCU1_REACH_NEG_2   (BOCU1_REACH_NEG_1-BOCU1_LEAD_2*BOCU1_TRAIL_गिनती)

/* The difference value range for 3-byters. */
#घोषणा BOCU1_REACH_POS_3   \
    (BOCU1_REACH_POS_2+BOCU1_LEAD_3*BOCU1_TRAIL_गिनती*BOCU1_TRAIL_गिनती)

#घोषणा BOCU1_REACH_NEG_3   (BOCU1_REACH_NEG_2-BOCU1_LEAD_3*BOCU1_TRAIL_गिनती*BOCU1_TRAIL_गिनती)

/* The lead byte start values. */
#घोषणा BOCU1_START_POS_2   (BOCU1_मध्य+BOCU1_REACH_POS_1+1)
#घोषणा BOCU1_START_POS_3   (BOCU1_START_POS_2+BOCU1_LEAD_2)
#घोषणा BOCU1_START_POS_4   (BOCU1_START_POS_3+BOCU1_LEAD_3)
     /* ==BOCU1_MAX_LEAD */

#घोषणा BOCU1_START_NEG_2   (BOCU1_मध्य+BOCU1_REACH_NEG_1)
#घोषणा BOCU1_START_NEG_3   (BOCU1_START_NEG_2-BOCU1_LEAD_2)
#घोषणा BOCU1_START_NEG_4   (BOCU1_START_NEG_3-BOCU1_LEAD_3)
     /* ==BOCU1_MIN+1 */

/* The length of a byte sequence, according to the lead byte (!=BOCU1_RESET). */
#घोषणा BOCU1_लंबाई_FROM_LEAD(lead) \
    ((BOCU1_START_NEG_2<=(lead) && (lead)<BOCU1_START_POS_2) ? 1 : \
     (BOCU1_START_NEG_3<=(lead) && (lead)<BOCU1_START_POS_3) ? 2 : \
     (BOCU1_START_NEG_4<=(lead) && (lead)<BOCU1_START_POS_4) ? 3 : 4)

/* The length of a byte sequence, according to its packed form. */
#घोषणा BOCU1_लंबाई_FROM_PACKED(packed) \
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
#घोषणा BOCU1_TRAIL_TO_BYTE(t) ((t)>=BOCU1_TRAIL_CONTROLS_गिनती ? (t)+BOCU1_TRAIL_BYTE_OFFSET : bocu1TrailToByte[t])

/*
 * Byte value map for control codes,
 * from external byte values 0x00..0x20
 * to trail byte values 0..19 (0..0x13) as used in the difference calculation.
 * External byte values that are illegal as trail bytes are mapped to -1.
 */
अटल स्थिर int8_t
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
अटल स्थिर int8_t
bocu1TrailToByte[BOCU1_TRAIL_CONTROLS_गिनती]={
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
#घोषणा NEGDIVMOD(n, d, m) UPRV_BLOCK_MACRO_BEGIN { \
    (m)=(n)%(d); \
    (n)/=(d); \
    अगर((m)<0) { \
        --(n); \
        (m)+=(d); \
    } \
} UPRV_BLOCK_MACRO_END

/* Faster versions of packDiff() for single-byte-encoded diff values. */

/** Is a diff value encodable in a single byte? */
#घोषणा DIFF_IS_SINGLE(diff) (BOCU1_REACH_NEG_1<=(diff) && (diff)<=BOCU1_REACH_POS_1)

/** Encode a diff value in a single byte. */
#घोषणा PACK_SINGLE_DIFF(diff) (BOCU1_मध्य+(diff))

/** Is a diff value encodable in two bytes? */
#घोषणा DIFF_IS_DOUBLE(diff) (BOCU1_REACH_NEG_2<=(diff) && (diff)<=BOCU1_REACH_POS_2)

/* BOCU-1 implementation functions ------------------------------------------ */

#घोषणा BOCU1_SIMPLE_पूर्व(c) (((c)&~0x7f)+BOCU1_ASCII_पूर्व)

/**
 * Compute the next "previous" value for differencing
 * from the current code point.
 *
 * @param c current code point, 0x3040..0xd7a3 (rest handled by macro below)
 * @return "previous code point" state value
 */
अटल अंतरभूत int32_t
bocu1_पूर्व(int32_t c) {
    /* compute new prev */
    अगर(/* 0x3040<=c && */ c<=0x309f) {
        /* Hiragana is not 128-aligned */
        वापस 0x3070;
    } अन्यथा अगर(0x4e00<=c && c<=0x9fa5) {
        /* CJK Unihan */
        वापस 0x4e00-BOCU1_REACH_NEG_2;
    } अन्यथा अगर(0xac00<=c /* && c<=0xd7a3 */) {
        /* Korean Hangul */
        वापस (0xd7a3+0xac00)/2;
    } अन्यथा {
        /* mostly small scripts */
        वापस BOCU1_SIMPLE_पूर्व(c);
    }
}

/** Fast version of bocu1Prev() for most scripts. */
#घोषणा BOCU1_पूर्व(c) ((c)<0x3040 || (c)>0xd7a3 ? BOCU1_SIMPLE_पूर्व(c) : bocu1_पूर्व(c))

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
अटल int32_t
packDiff(int32_t diff) {
    int32_t परिणाम, m;

    U_ASSERT(!DIFF_IS_SINGLE(diff)); /* assume we won't be called where diff==BOCU1_REACH_NEG_1=-64 */
    अगर(diff>=BOCU1_REACH_NEG_1) {
        /* mostly positive differences, and single-byte negative ones */
#अगर 0   /* single-byte case handled in macros, see below */
        अगर(diff<=BOCU1_REACH_POS_1) {
            /* single byte */
            वापस 0x01000000|(BOCU1_मध्य+diff);
        } अन्यथा
#पूर्ण_अगर
        अगर(diff<=BOCU1_REACH_POS_2) {
            /* two bytes */
            diff-=BOCU1_REACH_POS_1+1;
            परिणाम=0x02000000;

            m=diff%BOCU1_TRAIL_गिनती;
            diff/=BOCU1_TRAIL_गिनती;
            परिणाम|=BOCU1_TRAIL_TO_BYTE(m);

            परिणाम|=(BOCU1_START_POS_2+diff)<<8;
        } अन्यथा अगर(diff<=BOCU1_REACH_POS_3) {
            /* three bytes */
            diff-=BOCU1_REACH_POS_2+1;
            परिणाम=0x03000000;

            m=diff%BOCU1_TRAIL_गिनती;
            diff/=BOCU1_TRAIL_गिनती;
            परिणाम|=BOCU1_TRAIL_TO_BYTE(m);

            m=diff%BOCU1_TRAIL_गिनती;
            diff/=BOCU1_TRAIL_गिनती;
            परिणाम|=BOCU1_TRAIL_TO_BYTE(m)<<8;

            परिणाम|=(BOCU1_START_POS_3+diff)<<16;
        } अन्यथा {
            /* four bytes */
            diff-=BOCU1_REACH_POS_3+1;

            m=diff%BOCU1_TRAIL_गिनती;
            diff/=BOCU1_TRAIL_गिनती;
            परिणाम=BOCU1_TRAIL_TO_BYTE(m);

            m=diff%BOCU1_TRAIL_गिनती;
            diff/=BOCU1_TRAIL_गिनती;
            परिणाम|=BOCU1_TRAIL_TO_BYTE(m)<<8;

            /*
             * We know that / and % would deliver quotient 0 and rest=diff.
             * Avoid division and modulo for performance.
             */
            परिणाम|=BOCU1_TRAIL_TO_BYTE(diff)<<16;

            परिणाम |= static_cast<uint32_t>(BOCU1_START_POS_4) << 24;
        }
    } अन्यथा {
        /* two- to four-byte negative differences */
        अगर(diff>=BOCU1_REACH_NEG_2) {
            /* two bytes */
            diff-=BOCU1_REACH_NEG_1;
            परिणाम=0x02000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_गिनती, m);
            परिणाम|=BOCU1_TRAIL_TO_BYTE(m);

            परिणाम|=(BOCU1_START_NEG_2+diff)<<8;
        } अन्यथा अगर(diff>=BOCU1_REACH_NEG_3) {
            /* three bytes */
            diff-=BOCU1_REACH_NEG_2;
            परिणाम=0x03000000;

            NEGDIVMOD(diff, BOCU1_TRAIL_गिनती, m);
            परिणाम|=BOCU1_TRAIL_TO_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_गिनती, m);
            परिणाम|=BOCU1_TRAIL_TO_BYTE(m)<<8;

            परिणाम|=(BOCU1_START_NEG_3+diff)<<16;
        } अन्यथा {
            /* four bytes */
            diff-=BOCU1_REACH_NEG_3;

            NEGDIVMOD(diff, BOCU1_TRAIL_गिनती, m);
            परिणाम=BOCU1_TRAIL_TO_BYTE(m);

            NEGDIVMOD(diff, BOCU1_TRAIL_गिनती, m);
            परिणाम|=BOCU1_TRAIL_TO_BYTE(m)<<8;

            /*
             * We know that NEGDIVMOD would deliver
             * quotient -1 and rest=diff+BOCU1_TRAIL_COUNT.
             * Avoid division and modulo for performance.
             */
            m=diff+BOCU1_TRAIL_गिनती;
            परिणाम|=BOCU1_TRAIL_TO_BYTE(m)<<16;

            परिणाम|=BOCU1_MIN<<24;
        }
    }
    वापस परिणाम;
}


अटल व्योम U_CALLCONV
_Bocu1FromUnicodeWithOffsets(UConverterFromUnicodeArgs *pArgs,
                             UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर char16_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity;
    int32_t *offsets;

    int32_t पूर्व, c, diff;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source=pArgs->source;
    sourceLimit=pArgs->sourceLimit;
    target = reinterpret_cast<uint8_t*>(pArgs->target);
    targetCapacity = static_cast<int32_t>(pArgs->targetLimit - pArgs->target);
    offsets=pArgs->offsets;

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    पूर्व = static_cast<int32_t>(cnv->fromUnicodeStatus);
    अगर(पूर्व==0) {
        पूर्व=BOCU1_ASCII_पूर्व;
    }

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक= c==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    /* conversion loop */
    अगर(c!=0 && targetCapacity>0) {
        जाओ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(sourceLimit - source);
    अगर(targetCapacity>diff) {
        targetCapacity=diff;
    }
    जबतक(targetCapacity>0 && (c=*source)<0x3000) {
        अगर(c<=0x20) {
            अगर(c!=0x20) {
                पूर्व=BOCU1_ASCII_पूर्व;
            }
            *target++ = static_cast<uint8_t>(c);
            *offsets++=अग्र_Source_सूचकांक++;
            ++source;
            --targetCapacity;
        } अन्यथा {
            diff=c-पूर्व;
            अगर(DIFF_IS_SINGLE(diff)) {
                पूर्व=BOCU1_SIMPLE_पूर्व(c);
                *target++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=अग्र_Source_सूचकांक++;
                ++source;
                --targetCapacity;
            } अन्यथा {
                अवरोध;
            }
        }
    }
    /* restore real values */
    targetCapacity = static_cast<int32_t>(reinterpret_cast<स्थिर uint8_t*>(pArgs->targetLimit) - target);
    source_सूचकांक=अग्र_Source_सूचकांक; /* wrong if offsets==nullptr but does not matter */

    /* regular loop for all cases */
    जबतक(source<sourceLimit) {
        अगर(targetCapacity>0) {
            c=*source++;
            ++अग्र_Source_सूचकांक;

            अगर(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                अगर(c!=0x20) {
                    पूर्व=BOCU1_ASCII_पूर्व;
                }
                *target++ = static_cast<uint8_t>(c);
                *offsets++=source_सूचकांक;
                --targetCapacity;

                source_सूचकांक=अग्र_Source_सूचकांक;
                जारी;
            }

            अगर(U16_IS_LEAD(c)) {
getTrail:
                अगर(source<sourceLimit) {
                    /* test the following code unit */
                    char16_t trail=*source;
                    अगर(U16_IS_TRAIL(trail)) {
                        ++source;
                        ++अग्र_Source_सूचकांक;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } अन्यथा {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    अवरोध;
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
            अगर(DIFF_IS_SINGLE(diff)) {
                *target++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                *offsets++=source_सूचकांक;
                --targetCapacity;
                source_सूचकांक=अग्र_Source_सूचकांक;
                अगर(c<0x3000) {
                    जाओ fastSingle;
                }
            } अन्यथा अगर(DIFF_IS_DOUBLE(diff) && 2<=targetCapacity) {
                /* optimize 2-byte case */
                int32_t m;

                अगर(diff>=0) {
                    diff-=BOCU1_REACH_POS_1+1;
                    m=diff%BOCU1_TRAIL_गिनती;
                    diff/=BOCU1_TRAIL_गिनती;
                    diff+=BOCU1_START_POS_2;
                } अन्यथा {
                    diff-=BOCU1_REACH_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_गिनती, m);
                    diff+=BOCU1_START_NEG_2;
                }
                *target++ = static_cast<uint8_t>(diff);
                *target++ = static_cast<uint8_t>(BOCU1_TRAIL_TO_BYTE(m));
                *offsets++=source_सूचकांक;
                *offsets++=source_सूचकांक;
                targetCapacity-=2;
                source_सूचकांक=अग्र_Source_सूचकांक;
            } अन्यथा {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_लंबाई_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                अगर(length<=targetCapacity) {
                    चयन(length) {
                        /* each branch falls through to the next one */
                    हाल 4:
                        *target++ = static_cast<uint8_t>(diff >> 24);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    हाल 3:
                        *target++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    हाल 2:
                        *target++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=source_सूचकांक;
                    /* case 1: handled above */
                        *target++ = static_cast<uint8_t>(diff);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    शेष:
                        /* will never occur */
                        अवरोध;
                    }
                    targetCapacity-=length;
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
                    length-=targetCapacity;
                    char_Error_बफर=(uint8_t *)cnv->char_Error_बफर;
                    चयन(length) {
                        /* each branch falls through to the next one */
                    हाल 3:
                        *char_Error_बफर++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    हाल 2:
                        *char_Error_बफर++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    हाल 1:
                        *char_Error_बफर = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    शेष:
                        /* will never occur */
                        अवरोध;
                    }
                    cnv->char_Error_बफर_लंबाई = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    चयन(targetCapacity) {
                        /* each branch falls through to the next one */
                    हाल 3:
                        *target++ = static_cast<uint8_t>(diff >> 16);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    हाल 2:
                        *target++ = static_cast<uint8_t>(diff >> 8);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    हाल 1:
                        *target++ = static_cast<uint8_t>(diff);
                        *offsets++=source_सूचकांक;
                        U_FALLTHROUGH;
                    शेष:
                        /* will never occur */
                        अवरोध;
                    }

                    /* target overflow */
                    targetCapacity=0;
                    *pErrorCode=U_बफर_OVERFLOW_ERROR;
                    अवरोध;
                }
            }
        } अन्यथा {
            /* target is full */
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->fromUnicodeStatus = static_cast<uint32_t>(पूर्व);

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target = reinterpret_cast<अक्षर*>(target);
    pArgs->offsets=offsets;
}

/*
 * Identical to _Bocu1FromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
अटल व्योम U_CALLCONV
_Bocu1FromUnicode(UConverterFromUnicodeArgs *pArgs,
                  UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर char16_t *source, *sourceLimit;
    uint8_t *target;
    int32_t targetCapacity;

    int32_t पूर्व, c, diff;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source=pArgs->source;
    sourceLimit=pArgs->sourceLimit;
    target = reinterpret_cast<uint8_t*>(pArgs->target);
    targetCapacity = static_cast<int32_t>(pArgs->targetLimit - pArgs->target);

    /* get the converter state from UConverter */
    c=cnv->fromUChar32;
    पूर्व = static_cast<int32_t>(cnv->fromUnicodeStatus);
    अगर(पूर्व==0) {
        पूर्व=BOCU1_ASCII_पूर्व;
    }

    /* conversion loop */
    अगर(c!=0 && targetCapacity>0) {
        जाओ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use only one loop counter variable, targetCapacity, not also source */
    diff = static_cast<int32_t>(sourceLimit - source);
    अगर(targetCapacity>diff) {
        targetCapacity=diff;
    }
    जबतक(targetCapacity>0 && (c=*source)<0x3000) {
        अगर(c<=0x20) {
            अगर(c!=0x20) {
                पूर्व=BOCU1_ASCII_पूर्व;
            }
            *target++ = static_cast<uint8_t>(c);
        } अन्यथा {
            diff=c-पूर्व;
            अगर(DIFF_IS_SINGLE(diff)) {
                पूर्व=BOCU1_SIMPLE_पूर्व(c);
                *target++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
            } अन्यथा {
                अवरोध;
            }
        }
        ++source;
        --targetCapacity;
    }
    /* restore real values */
    targetCapacity = static_cast<int32_t>(reinterpret_cast<स्थिर uint8_t*>(pArgs->targetLimit) - target);

    /* regular loop for all cases */
    जबतक(source<sourceLimit) {
        अगर(targetCapacity>0) {
            c=*source++;

            अगर(c<=0x20) {
                /*
                 * ISO C0 control & space:
                 * Encode directly for MIME compatibility,
                 * and reset state except for space, to not disrupt compression.
                 */
                अगर(c!=0x20) {
                    पूर्व=BOCU1_ASCII_पूर्व;
                }
                *target++ = static_cast<uint8_t>(c);
                --targetCapacity;
                जारी;
            }

            अगर(U16_IS_LEAD(c)) {
getTrail:
                अगर(source<sourceLimit) {
                    /* test the following code unit */
                    char16_t trail=*source;
                    अगर(U16_IS_TRAIL(trail)) {
                        ++source;
                        c=U16_GET_SUPPLEMENTARY(c, trail);
                    }
                } अन्यथा {
                    /* no more input */
                    c=-c; /* negative lead surrogate as "incomplete" indicator to avoid c=0 everywhere else */
                    अवरोध;
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
            अगर(DIFF_IS_SINGLE(diff)) {
                *target++ = static_cast<uint8_t>(PACK_SINGLE_DIFF(diff));
                --targetCapacity;
                अगर(c<0x3000) {
                    जाओ fastSingle;
                }
            } अन्यथा अगर(DIFF_IS_DOUBLE(diff) && 2<=targetCapacity) {
                /* optimize 2-byte case */
                int32_t m;

                अगर(diff>=0) {
                    diff-=BOCU1_REACH_POS_1+1;
                    m=diff%BOCU1_TRAIL_गिनती;
                    diff/=BOCU1_TRAIL_गिनती;
                    diff+=BOCU1_START_POS_2;
                } अन्यथा {
                    diff-=BOCU1_REACH_NEG_1;
                    NEGDIVMOD(diff, BOCU1_TRAIL_गिनती, m);
                    diff+=BOCU1_START_NEG_2;
                }
                *target++ = static_cast<uint8_t>(diff);
                *target++ = static_cast<uint8_t>(BOCU1_TRAIL_TO_BYTE(m));
                targetCapacity-=2;
            } अन्यथा {
                int32_t length; /* will be 2..4 */

                diff=packDiff(diff);
                length=BOCU1_लंबाई_FROM_PACKED(diff);

                /* write the output character bytes from diff and length */
                /* from the first if in the loop we know that targetCapacity>0 */
                अगर(length<=targetCapacity) {
                    चयन(length) {
                        /* each branch falls through to the next one */
                    हाल 4:
                        *target++ = static_cast<uint8_t>(diff >> 24);
                        U_FALLTHROUGH;
                    हाल 3:
                        *target++ = static_cast<uint8_t>(diff >> 16);
                    /* case 2: handled above */
                        *target++ = static_cast<uint8_t>(diff >> 8);
                    /* case 1: handled above */
                        *target++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    शेष:
                        /* will never occur */
                        अवरोध;
                    }
                    targetCapacity-=length;
                } अन्यथा {
                    uint8_t *char_Error_बफर;

                    /*
                     * We actually do this backwards here:
                     * In order to save an intermediate variable, we output
                     * first to the overflow buffer what does not fit into the
                     * regular target.
                     */
                    /* we know that 1<=targetCapacity<length<=4 */
                    length-=targetCapacity;
                    char_Error_बफर=(uint8_t *)cnv->char_Error_बफर;
                    चयन(length) {
                        /* each branch falls through to the next one */
                    हाल 3:
                        *char_Error_बफर++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    हाल 2:
                        *char_Error_बफर++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    हाल 1:
                        *char_Error_बफर = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    शेष:
                        /* will never occur */
                        अवरोध;
                    }
                    cnv->char_Error_बफर_लंबाई = static_cast<int8_t>(length);

                    /* now output what fits into the regular target */
                    diff>>=8*length; /* length was reduced by targetCapacity */
                    चयन(targetCapacity) {
                        /* each branch falls through to the next one */
                    हाल 3:
                        *target++ = static_cast<uint8_t>(diff >> 16);
                        U_FALLTHROUGH;
                    हाल 2:
                        *target++ = static_cast<uint8_t>(diff >> 8);
                        U_FALLTHROUGH;
                    हाल 1:
                        *target++ = static_cast<uint8_t>(diff);
                        U_FALLTHROUGH;
                    शेष:
                        /* will never occur */
                        अवरोध;
                    }

                    /* target overflow */
                    targetCapacity=0;
                    *pErrorCode=U_बफर_OVERFLOW_ERROR;
                    अवरोध;
                }
            }
        } अन्यथा {
            /* target is full */
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }
    }

    /* set the converter state back into UConverter */
    cnv->fromUChar32= c<0 ? -c : 0;
    cnv->fromUnicodeStatus = static_cast<uint32_t>(पूर्व);

    /* write back the updated pointers */
    pArgs->source=source;
    pArgs->target = reinterpret_cast<अक्षर*>(target);
}

/* BOCU-1-to-Unicode conversion functions ----------------------------------- */

/**
 * Function for BOCU-1 decoder; handles multi-byte lead bytes.
 *
 * @param b lead byte;
 *          BOCU1_MIN<=b<BOCU1_START_NEG_2 or BOCU1_START_POS_2<=b<BOCU1_MAX_LEAD
 * @return (diff<<2)|count
 */
अटल अंतरभूत int32_t
decodeBocu1LeadByte(int32_t b) {
    int32_t diff, count;

    अगर(b>=BOCU1_START_NEG_2) {
        /* positive difference */
        अगर(b<BOCU1_START_POS_3) {
            /* two bytes */
            diff = (b - BOCU1_START_POS_2) * BOCU1_TRAIL_गिनती + BOCU1_REACH_POS_1 + 1;
            count=1;
        } अन्यथा अगर(b<BOCU1_START_POS_4) {
            /* three bytes */
            diff = (b - BOCU1_START_POS_3) * BOCU1_TRAIL_गिनती * BOCU1_TRAIL_गिनती + BOCU1_REACH_POS_2 + 1;
            count=2;
        } अन्यथा {
            /* four bytes */
            diff=BOCU1_REACH_POS_3+1;
            count=3;
        }
    } अन्यथा {
        /* negative difference */
        अगर(b>=BOCU1_START_NEG_3) {
            /* two bytes */
            diff = (b - BOCU1_START_NEG_2) * BOCU1_TRAIL_गिनती + BOCU1_REACH_NEG_1;
            count=1;
        } अन्यथा अगर(b>BOCU1_MIN) {
            /* three bytes */
            diff = (b - BOCU1_START_NEG_3) * BOCU1_TRAIL_गिनती * BOCU1_TRAIL_गिनती + BOCU1_REACH_NEG_2;
            count=2;
        } अन्यथा {
            /* four bytes */
            diff=-BOCU1_TRAIL_गिनती*BOCU1_TRAIL_गिनती*BOCU1_TRAIL_गिनती+BOCU1_REACH_NEG_3;
            count=3;
        }
    }

    /* return the state for decoding the trail byte(s) */
    वापस (static_cast<uint32_t>(diff) << 2) | count;
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
अटल अंतरभूत int32_t
decodeBocu1TrailByte(int32_t count, int32_t b) {
    अगर(b<=0x20) {
        /* skip some C0 controls and make the trail byte range contiguous */
        b=bocu1ByteToTrail[b];
        /* b<0 for an illegal trail byte value will result in return<0 below */
#अगर BOCU1_MAX_TRAIL<0xff
    } अन्यथा अगर(b>BOCU1_MAX_TRAIL) {
        वापस -99;
#पूर्ण_अगर
    } अन्यथा {
        b-=BOCU1_TRAIL_BYTE_OFFSET;
    }

    /* add trail byte into difference and decrement count */
    अगर(count==1) {
        वापस b;
    } अन्यथा अगर(count==2) {
        वापस b*BOCU1_TRAIL_गिनती;
    } अन्यथा /* count==3 */ {
        वापस b*(BOCU1_TRAIL_गिनती*BOCU1_TRAIL_गिनती);
    }
}

अटल व्योम U_CALLCONV
_Bocu1ToUnicodeWithOffsets(UConverterToUnicodeArgs *pArgs,
                           UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर uint8_t *source, *sourceLimit;
    char16_t *target;
    स्थिर char16_t *targetLimit;
    int32_t *offsets;

    int32_t पूर्व, count, diff, c;

    int8_t byte_सूचकांक;
    uint8_t *bytes;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source = reinterpret_cast<स्थिर uint8_t*>(pArgs->source);
    sourceLimit = reinterpret_cast<स्थिर uint8_t*>(pArgs->sourceLimit);
    target=pArgs->target;
    targetLimit=pArgs->targetLimit;
    offsets=pArgs->offsets;

    /* get the converter state from UConverter */
    पूर्व = static_cast<int32_t>(cnv->toUnicodeStatus);
    अगर(पूर्व==0) {
        पूर्व=BOCU1_ASCII_पूर्व;
    }
    diff=cnv->mode; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_सूचकांक=cnv->toULength;
    bytes=cnv->toUBytes;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक=byte_सूचकांक==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    अगर(count>0 && byte_सूचकांक>0 && target<targetLimit) {
        जाओ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(sourceLimit - source);
    count = static_cast<int32_t>(pArgs->targetLimit - target);
    अगर(count>diff) {
        count=diff;
    }
    जबतक(count>0) {
        अगर(BOCU1_START_NEG_2<=(c=*source) && c<BOCU1_START_POS_2) {
            c=पूर्व+(c-BOCU1_मध्य);
            अगर(c<0x3000) {
                *target++ = static_cast<char16_t>(c);
                *offsets++=अग्र_Source_सूचकांक++;
                पूर्व=BOCU1_SIMPLE_पूर्व(c);
            } अन्यथा {
                अवरोध;
            }
        } अन्यथा अगर(c<=0x20) {
            अगर(c!=0x20) {
                पूर्व=BOCU1_ASCII_पूर्व;
            }
            *target++ = static_cast<char16_t>(c);
            *offsets++=अग्र_Source_सूचकांक++;
        } अन्यथा {
            अवरोध;
        }
        ++source;
        --count;
    }
    source_सूचकांक=अग्र_Source_सूचकांक; /* wrong if offsets==nullptr but does not matter */

    /* decode a sequence of single and lead bytes */
    जबतक(source<sourceLimit) {
        अगर(target>=targetLimit) {
            /* target is full */
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }

        ++अग्र_Source_सूचकांक;
        c=*source++;
        अगर(BOCU1_START_NEG_2<=c && c<BOCU1_START_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=पूर्व+(c-BOCU1_मध्य);
            अगर(c<0x3000) {
                *target++ = static_cast<char16_t>(c);
                *offsets++=source_सूचकांक;
                पूर्व=BOCU1_SIMPLE_पूर्व(c);
                source_सूचकांक=अग्र_Source_सूचकांक;
                जाओ fastSingle;
            }
        } अन्यथा अगर(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            अगर(c!=0x20) {
                पूर्व=BOCU1_ASCII_पूर्व;
            }
            *target++ = static_cast<char16_t>(c);
            *offsets++=source_सूचकांक;
            source_सूचकांक=अग्र_Source_सूचकांक;
            जारी;
        } अन्यथा अगर(BOCU1_START_NEG_3<=c && c<BOCU1_START_POS_3 && source<sourceLimit) {
            /* Optimize two-byte case. */
            अगर(c>=BOCU1_मध्य) {
                diff = (c - BOCU1_START_POS_2) * BOCU1_TRAIL_गिनती + BOCU1_REACH_POS_1 + 1;
            } अन्यथा {
                diff = (c - BOCU1_START_NEG_2) * BOCU1_TRAIL_गिनती + BOCU1_REACH_NEG_1;
            }

            /* trail byte */
            ++अग्र_Source_सूचकांक;
            c=decodeBocu1TrailByte(1, *source++);
            अगर (c < 0 || static_cast<uint32_t>(c = पूर्व + diff + c) > 0x10ffff) {
                bytes[0]=source[-2];
                bytes[1]=source[-1];
                byte_सूचकांक=2;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                अवरोध;
            }
        } अन्यथा अगर(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            पूर्व=BOCU1_ASCII_पूर्व;
            source_सूचकांक=अग्र_Source_सूचकांक;
            जारी;
        } अन्यथा {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            bytes[0] = static_cast<uint8_t>(c);
            byte_सूचकांक=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            क्रम(;;) {
                अगर(source>=sourceLimit) {
                    जाओ endloop;
                }
                ++अग्र_Source_सूचकांक;
                c=bytes[byte_सूचकांक++]=*source++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                अगर(c<0) {
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                    जाओ endloop;
                }

                diff+=c;
                अगर(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_सूचकांक=0;
                    c=पूर्व+diff;
                    अगर (static_cast<uint32_t>(c) > 0x10ffff) {
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        जाओ endloop;
                    }
                    अवरोध;
                }
            }
        }

        /* calculate the next prev and output c */
        पूर्व=BOCU1_पूर्व(c);
        अगर(c<=0xffff) {
            *target++ = static_cast<char16_t>(c);
            *offsets++=source_सूचकांक;
        } अन्यथा {
            /* output surrogate pair */
            *target++=U16_LEAD(c);
            अगर(target<targetLimit) {
                *target++=U16_TRAIL(c);
                *offsets++=source_सूचकांक;
                *offsets++=source_सूचकांक;
            } अन्यथा {
                /* target overflow */
                *offsets++=source_सूचकांक;
                cnv->UChar_Error_बफर[0]=U16_TRAIL(c);
                cnv->UChar_Error_बफर_लंबाई=1;
                *pErrorCode=U_बफर_OVERFLOW_ERROR;
                अवरोध;
            }
        }
        source_सूचकांक=अग्र_Source_सूचकांक;
    }
endloop:

    अगर(*pErrorCode==U_ILLEGAL_CHAR_FOUND) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->toUnicodeStatus=BOCU1_ASCII_पूर्व;
        cnv->mode=0;
    } अन्यथा {
        /* set the converter state back into UConverter */
        cnv->toUnicodeStatus = static_cast<uint32_t>(पूर्व);
        cnv->mode = static_cast<int32_t>(static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->toULength=byte_सूचकांक;

    /* write back the updated pointers */
    pArgs->source = reinterpret_cast<स्थिर अक्षर*>(source);
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
अटल व्योम U_CALLCONV
_Bocu1ToUnicode(UConverterToUnicodeArgs *pArgs,
                UErrorCode *pErrorCode) {
    UConverter *cnv;
    स्थिर uint8_t *source, *sourceLimit;
    char16_t *target;
    स्थिर char16_t *targetLimit;

    int32_t पूर्व, count, diff, c;

    int8_t byte_सूचकांक;
    uint8_t *bytes;

    /* set up the local pointers */
    cnv=pArgs->converter;
    source = reinterpret_cast<स्थिर uint8_t*>(pArgs->source);
    sourceLimit = reinterpret_cast<स्थिर uint8_t*>(pArgs->sourceLimit);
    target=pArgs->target;
    targetLimit=pArgs->targetLimit;

    /* get the converter state from UConverter */
    पूर्व = static_cast<int32_t>(cnv->toUnicodeStatus);
    अगर(पूर्व==0) {
        पूर्व=BOCU1_ASCII_पूर्व;
    }
    diff=cnv->mode; /* mode may be set to UCNV_SI by ucnv_bld.c but then toULength==0 */
    count=diff&3;
    diff>>=2;

    byte_सूचकांक=cnv->toULength;
    bytes=cnv->toUBytes;

    /* conversion "loop" similar to _SCSUToUnicodeWithOffsets() */
    अगर(count>0 && byte_सूचकांक>0 && target<targetLimit) {
        जाओ getTrail;
    }

fastSingle:
    /* fast loop for single-byte differences */
    /* use count as the only loop counter variable */
    diff = static_cast<int32_t>(sourceLimit - source);
    count = static_cast<int32_t>(pArgs->targetLimit - target);
    अगर(count>diff) {
        count=diff;
    }
    जबतक(count>0) {
        अगर(BOCU1_START_NEG_2<=(c=*source) && c<BOCU1_START_POS_2) {
            c=पूर्व+(c-BOCU1_मध्य);
            अगर(c<0x3000) {
                *target++ = static_cast<char16_t>(c);
                पूर्व=BOCU1_SIMPLE_पूर्व(c);
            } अन्यथा {
                अवरोध;
            }
        } अन्यथा अगर(c<=0x20) {
            अगर(c!=0x20) {
                पूर्व=BOCU1_ASCII_पूर्व;
            }
            *target++ = static_cast<char16_t>(c);
        } अन्यथा {
            अवरोध;
        }
        ++source;
        --count;
    }

    /* decode a sequence of single and lead bytes */
    जबतक(source<sourceLimit) {
        अगर(target>=targetLimit) {
            /* target is full */
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }

        c=*source++;
        अगर(BOCU1_START_NEG_2<=c && c<BOCU1_START_POS_2) {
            /* Write a code point directly from a single-byte difference. */
            c=पूर्व+(c-BOCU1_मध्य);
            अगर(c<0x3000) {
                *target++ = static_cast<char16_t>(c);
                पूर्व=BOCU1_SIMPLE_पूर्व(c);
                जाओ fastSingle;
            }
        } अन्यथा अगर(c<=0x20) {
            /*
             * Direct-encoded C0 control code or space.
             * Reset prev for C0 control codes but not for space.
             */
            अगर(c!=0x20) {
                पूर्व=BOCU1_ASCII_पूर्व;
            }
            *target++ = static_cast<char16_t>(c);
            जारी;
        } अन्यथा अगर(BOCU1_START_NEG_3<=c && c<BOCU1_START_POS_3 && source<sourceLimit) {
            /* Optimize two-byte case. */
            अगर(c>=BOCU1_मध्य) {
                diff = (c - BOCU1_START_POS_2) * BOCU1_TRAIL_गिनती + BOCU1_REACH_POS_1 + 1;
            } अन्यथा {
                diff = (c - BOCU1_START_NEG_2) * BOCU1_TRAIL_गिनती + BOCU1_REACH_NEG_1;
            }

            /* trail byte */
            c=decodeBocu1TrailByte(1, *source++);
            अगर (c < 0 || static_cast<uint32_t>(c = पूर्व + diff + c) > 0x10ffff) {
                bytes[0]=source[-2];
                bytes[1]=source[-1];
                byte_सूचकांक=2;
                *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                अवरोध;
            }
        } अन्यथा अगर(c==BOCU1_RESET) {
            /* only reset the state, no code point */
            पूर्व=BOCU1_ASCII_पूर्व;
            जारी;
        } अन्यथा {
            /*
             * For multi-byte difference lead bytes, set the decoder state
             * with the partial difference value from the lead byte and
             * with the number of trail bytes.
             */
            bytes[0] = static_cast<uint8_t>(c);
            byte_सूचकांक=1;

            diff=decodeBocu1LeadByte(c);
            count=diff&3;
            diff>>=2;
getTrail:
            क्रम(;;) {
                अगर(source>=sourceLimit) {
                    जाओ endloop;
                }
                c=bytes[byte_सूचकांक++]=*source++;

                /* trail byte in any position */
                c=decodeBocu1TrailByte(count, c);
                अगर(c<0) {
                    *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                    जाओ endloop;
                }

                diff+=c;
                अगर(--count==0) {
                    /* final trail byte, deliver a code point */
                    byte_सूचकांक=0;
                    c=पूर्व+diff;
                    अगर (static_cast<uint32_t>(c) > 0x10ffff) {
                        *pErrorCode=U_ILLEGAL_CHAR_FOUND;
                        जाओ endloop;
                    }
                    अवरोध;
                }
            }
        }

        /* calculate the next prev and output c */
        पूर्व=BOCU1_पूर्व(c);
        अगर(c<=0xffff) {
            *target++ = static_cast<char16_t>(c);
        } अन्यथा {
            /* output surrogate pair */
            *target++=U16_LEAD(c);
            अगर(target<targetLimit) {
                *target++=U16_TRAIL(c);
            } अन्यथा {
                /* target overflow */
                cnv->UChar_Error_बफर[0]=U16_TRAIL(c);
                cnv->UChar_Error_बफर_लंबाई=1;
                *pErrorCode=U_बफर_OVERFLOW_ERROR;
                अवरोध;
            }
        }
    }
endloop:

    अगर(*pErrorCode==U_ILLEGAL_CHAR_FOUND) {
        /* set the converter state in UConverter to deal with the next character */
        cnv->toUnicodeStatus=BOCU1_ASCII_पूर्व;
        cnv->mode=0;
    } अन्यथा {
        /* set the converter state back into UConverter */
        cnv->toUnicodeStatus = static_cast<uint32_t>(पूर्व);
        cnv->mode = (static_cast<uint32_t>(diff) << 2) | count;
    }
    cnv->toULength=byte_सूचकांक;

    /* write back the updated pointers */
    pArgs->source = reinterpret_cast<स्थिर अक्षर*>(source);
    pArgs->target=target;
}

/* miscellaneous ------------------------------------------------------------ */

अटल स्थिर UConverterImpl _Bocu1Impl={
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

अटल स्थिर UConverter_Static_तथ्य Bocu1_Static_तथ्य={
    माप(UConverter_Static_तथ्य),
    "BOCU-1",
    1214, /* CCSID for BOCU-1 */
    UCNV_IBM, UCNV_BOCU1,
    1, 4, /* one char16_t generates at least 1 byte and at most 4 bytes */
    { 0x1a, 0, 0, 0 }, 1, /* BOCU-1 never needs to write a subchar */
    false, false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

स्थिर UConverter_Shared_तथ्य Bocu1_तथ्य=
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&Bocu1_Static_तथ्य, &_Bocu1Impl);

#पूर्ण_अगर
