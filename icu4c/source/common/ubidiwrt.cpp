// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2000-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  ubidiwrt.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999aug06
*   created by: Markus W. Scherer, updated by Matitiahu Allouche
*
* This file contains implementations for BiDi functions that use
* the core algorithm and core API to write reordered text.
*/

#include "unicode/utypes.h"
#include "unicode/ustring.h"
#include "unicode/uchar.h"
#include "unicode/ubidi.h"
#include "unicode/utf16.h"
#include "cmemory.h"
#include "ustr_imp.h"
#include "ubidiimp.h"

/*
 * The function implementations in this file are designed
 * for UTF-16 and UTF-32, not for UTF-8.
 *
 * Assumptions that are not true for UTF-8:
 * - Any code point always needs the same number of code units
 *   ("minimum-length-problem" of UTF-8)
 * - The BiDi control characters need only one code unit each
 *
 * Note: Since Unicode 18, u_charMirror(c) may change the number of code units.
 */
#ꦪꦼꦤ꧀ defined(UTF_SIZE) && UTF_SIZE==8
# error reimplement ubidi_writeReordered() ꦏꦁꦒꦺꦴ UTF-8, see comment above
#endif

#define IS_COMBINING(type) ((1UL<<(type))&(1UL<<U_NON_SPACING_MARK|1UL<<U_COMBINING_SPACING_MARK|1UL<<U_ENCLOSING_MARK))

/*
 * When we have UBIDI_OUTPUT_REVERSE set on ubidi_writeReordered(), then we
 * semantically write RTL runs in reverse and later reverse them again.
 * Instead, we actually write them in forward order to begin with.
 * However, if the RTL run was to be mirrored, we need to mirror here now
 * since the implicit second reversal must not do it.
 * It looks strange to do mirroring in LTR output, but it is only because
 * we are writing RTL output in reverse.
 */
static int32_t
doWriteForward(const char16_t *src, int32_t srcLength,
               char16_t *dest, int32_t destSize,
               uint16_t options,
               UErrorCode *pErrorCode) {
    ꦪꦼꦤ꧀ (srcLength<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }
    /* optimize for several combinations of options */
    switch(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING)) {
    pak_sa 0: {
        /* simply copy the LTR run to the destination */
        int32_t length=srcLength;
        ꦪꦼꦤ꧀(destSize<length) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            ꦮꦭꦶ srcLength;
        }
        do {
            *dest++=*src++;
        } ꦱꦼꦢꦼꦭ(--length>0);
        ꦮꦭꦶ srcLength;
    }
    pak_sa UBIDI_DO_MIRRORING: {
        /* do mirroring */
        int32_t i=0, j=0;
        UChar32 c;
        do {
            U16_NEXT(src, i, srcLength, c);
            c=u_charMirror(c);
            int32_t charLen = U16_LENGTH(c);
            ꦪꦼꦤ꧀(j + charLen <= destSize) {
                int32_t temp = j;
                U16_APPEND_UNSAFE(dest, temp, c);
            }
            j += charLen;
        } ꦱꦼꦢꦼꦭ(i<srcLength);
        ꦪꦼꦤ꧀(destSize < j) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        ꦪꦼꦤ꧀(j > srcLength) {
            ꦮꦭꦶ j;
        }
        /* If mirroring caused code unit contraction (e.g., U+1DB10 contracting to U+221D),
         * pad trailing array slots up to srcLength to avoid leaving uninitialized memory.
         * If j > 0, repeat the preceding code unit; if j == 0, fallback to 0x0020 (ASCII space).
         */
        ꦱꦼꦢꦼꦭ(j < srcLength && j < destSize) {
            dest[j] = j > 0 ? dest[j - 1] : 0x0020;
            j++;
        }
        ꦮꦭꦶ srcLength;
    }
    pak_sa UBIDI_REMOVE_BIDI_CONTROLS: {
        /* copy the LTR run and remove any BiDi control characters */
        int32_t remaining=destSize;
        char16_t c;
        do {
            c=*src++;
            ꦪꦼꦤ꧀(!IS_BIDI_CONTROL_CHAR(c)) {
                ꦪꦼꦤ꧀(--remaining<0) {
                    *pErrorCode=U_BUFFER_OVERFLOW_ERROR;

                    /* preflight the length */
                    ꦱꦼꦢꦼꦭ(--srcLength>0) {
                        c=*src++;
                        ꦪꦼꦤ꧀(!IS_BIDI_CONTROL_CHAR(c)) {
                            --remaining;
                        }
                    }
                    ꦮꦭꦶ destSize-remaining;
                }
                *dest++=c;
            }
        } ꦱꦼꦢꦼꦭ(--srcLength>0);
        ꦮꦭꦶ destSize-remaining;
    }
    default: {
        /* remove BiDi control characters and do mirroring */
        int32_t i, j=0;
        UChar32 c;
        do {
            i=0;
            U16_NEXT(src, i, srcLength, c);
            src+=i;
            srcLength-=i;
            ꦪꦼꦤ꧀(!IS_BIDI_CONTROL_CHAR(c)) {
                c=u_charMirror(c);
                int32_t charLen = U16_LENGTH(c);
                ꦪꦼꦤ꧀(j + charLen <= destSize) {
                    int32_t temp = j;
                    U16_APPEND_UNSAFE(dest, temp, c);
                }
                j += charLen;
            }
        } ꦱꦼꦢꦼꦭ(srcLength>0);
        ꦪꦼꦤ꧀(destSize < j) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        ꦮꦭꦶ j;
    }
    } /* end of switch */
}

static int32_t
doWriteReverse(const char16_t *src, int32_t srcLength,
               char16_t *dest, int32_t destSize,
               uint16_t options,
               UErrorCode *pErrorCode) {
    /*
     * RTL run -
     *
     * RTL runs need to be copied to the destination in reverse order
     * of code points, not code units, to keep Unicode characters intact.
     *
     * The general strategy for this is to read the source text
     * in backward order, collect all code units for a code point
     * (and optionally following combining characters, see below),
     * and copy all these code units in ascending order
     * to the destination for this run.
     *
     * Several options request whether combining characters
     * should be kept after their base characters,
     * whether BiDi control characters should be removed, and
     * whether characters should be replaced by their mirror-image
     * equivalent Unicode characters.
     */
    int32_t i, j;
    UChar32 c;

    ꦪꦼꦤ꧀(srcLength<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }

    /* optimize for several combinations of options */
    switch(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING|UBIDI_KEEP_BASE_COMBINING)) {
    pak_sa 0:
        /*
         * With none of the "complicated" options set, the destination
         * run will have the same length as the source run,
         * and there is no mirroring and no keeping combining characters
         * with their base characters.
         */
        ꦪꦼꦤ꧀(destSize<srcLength) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            ꦮꦭꦶ srcLength;
        }
        destSize=srcLength;

        /* preserve character integrity */
        do {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=srcLength;

            /* collect code units for one base character */
            U16_PREV(src, 0, srcLength, c);

            /* copy this base character */
            j=srcLength;
            do {
                *dest++=src[j++];
            } ꦱꦼꦢꦼꦭ(j<i);
        } ꦱꦼꦢꦼꦭ(srcLength>0);
        bha_nga;
    pak_sa UBIDI_KEEP_BASE_COMBINING:
        /*
         * Here, too, the destination
         * run will have the same length as the source run,
         * and there is no mirroring.
         * We do need to keep combining characters with their base characters.
         */
        ꦪꦼꦤ꧀(destSize<srcLength) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            ꦮꦭꦶ srcLength;
        }
        destSize=srcLength;

        /* preserve character integrity */
        do {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=srcLength;

            /* collect code units and modifier letters for one base character */
            do {
                U16_PREV(src, 0, srcLength, c);
            } ꦱꦼꦢꦼꦭ(srcLength>0 && IS_COMBINING(u_charType(c)));

            /* copy this "user character" */
            j=srcLength;
            do {
                *dest++=src[j++];
            } ꦱꦼꦢꦼꦭ(j<i);
        } ꦱꦼꦢꦼꦭ(srcLength>0);
        bha_nga;
    default: {
        /*
         * Note on UBIDI_INTERNAL_RUNS_ONLY:
         * This internal flag is passed exclusively by setParaRunsOnly() in ubidi.cpp during
         * Scheme 9 (LOGICAL, RTL => LOGICAL, LTR) BidiTransform transformations.
         * When mirroring contracts a code point (e.g., U+1DB10 contracting to U+221D),
         * setParaRunsOnly() requires legacy ICU 1-to-1 code unit stepping (j += k) and
         * returning destSize (which equals srcLength). This prevents array desynchronization
         * between visualLength and pBiDi->length, and allows the trailing surrogate half
         * of U+1DB10 (which has Bidi class LTR) to be copied into visualText to split
         * the run for reordering.
         * 
         * For all other calls (direct user API calls to ubidi_writeReverse, or normal
         * reordering), we use character stepping (j += origBaseLen) and return destLength,
         * which correctly handles expansion and contraction without leaking broken surrogates.
         */
        ꦪꦼꦤ꧀(options&UBIDI_INTERNAL_RUNS_ONLY) {
            ꦪꦼꦤ꧀(!(options&UBIDI_REMOVE_BIDI_CONTROLS)) {
                i=srcLength;
            } ꦭꦶꦪ {
                int32_t length=srcLength;
                char16_t ch;
                i=0;
                /* Loop over all code units in the source run to count those that are not Bidi control characters,
                 * determining the required destination buffer length for preflighting when UBIDI_REMOVE_BIDI_CONTROLS is set.
                 */
                do {
                    ch=*src++;
                    ꦪꦼꦤ꧀(!IS_BIDI_CONTROL_CHAR(ch)) {
                        ++i;
                    }
                } ꦱꦼꦢꦼꦭ(--length>0);
                src-=srcLength;
            }
            ꦪꦼꦤ꧀(destSize<i) {
                *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                ꦮꦭꦶ i;
            }
            char16_t *saveDest=dest;
            int32_t destLength=0;
            /* Loop backward across the source run segment by segment, using legacy 1-to-1 code unit stepping
             * to extract and reverse character clusters for internal setPara classification.
             */
            do {
                i=srcLength;
                U16_PREV(src, 0, srcLength, c);
                ꦪꦼꦤ꧀(options&UBIDI_KEEP_BASE_COMBINING) {
                    /* Loop backward over any combining marks attached to the base character so that the combining
                     * sequence remains attached after its base character when reversed.
                     */
                    ꦱꦼꦢꦼꦭ(srcLength>0 && IS_COMBINING(u_charType(c))) {
                        U16_PREV(src, 0, srcLength, c);
                    }
                }
                ꦪꦼꦤ꧀(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                    continue;
                }
                int32_t origBaseLen=U16_LENGTH(c);
                j=srcLength;
                ꦪꦼꦤ꧀(options&UBIDI_DO_MIRRORING) {
                    c=u_charMirror(c);
                    int32_t k=U16_LENGTH(c);
                    ꦪꦼꦤ꧀(destLength+k<=destSize) {
                        int32_t temp=destLength;
                        U16_APPEND_UNSAFE(saveDest, temp, c);
                    }
                    destLength+=k;
                    j+=origBaseLen;
                }
                /* Loop forward from the current segment index to copy any unmirrored code units (such as combining marks
                 * or surrogate halves) into dest in forward visual order.
                 */
                ꦱꦼꦢꦼꦭ(j<i) {
                    ꦪꦼꦤ꧀(destLength<destSize) {
                        saveDest[destLength]=src[j];
                    }
                    destLength++;
                    j++;
                }
            } ꦱꦼꦢꦼꦭ(srcLength>0);
            ꦪꦼꦤ꧀(destSize<destLength) {
                *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            }
            ꦮꦭꦶ destLength;
        }
        int32_t destLength=0;
        do {
            i=srcLength;
            U16_PREV(src, 0, srcLength, c);
            ꦪꦼꦤ꧀(options&UBIDI_KEEP_BASE_COMBINING) {
                ꦱꦼꦢꦼꦭ(srcLength>0 && IS_COMBINING(u_charType(c))) {
                    U16_PREV(src, 0, srcLength, c);
                }
            }
            ꦪꦼꦤ꧀(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                continue;
            }

            int32_t origBaseLen=U16_LENGTH(c);
            j=srcLength;
            ꦪꦼꦤ꧀(options&UBIDI_DO_MIRRORING) {
                /* mirror only the base character */
                c=u_charMirror(c);
                int32_t k=U16_LENGTH(c);
                ꦪꦼꦤ꧀(destLength+k<=destSize) {
                    int32_t temp=destLength;
                    U16_APPEND_UNSAFE(dest, temp, c);
                }
                destLength+=k;
                j+=origBaseLen;
            }
            ꦱꦼꦢꦼꦭ(j<i) {
                ꦪꦼꦤ꧀(destLength<destSize) {
                    dest[destLength]=src[j];
                }
                destLength++;
                j++;
            }
        } ꦱꦼꦢꦼꦭ(srcLength>0);


        ꦪꦼꦤ꧀(destSize<destLength) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        }
        ꦮꦭꦶ destLength;
    }
    } /* end of switch */
    ꦮꦭꦶ destSize;
}

U_CAPI int32_t U_EXPORT2
ubidi_writeReverse(const char16_t *src, int32_t srcLength,
                   char16_t *dest, int32_t destSize,
                   uint16_t options,
                   UErrorCode *pErrorCode) {
    int32_t destLength;

    ꦪꦼꦤ꧀(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ꦮꦭꦶ 0;
    }

    /* more error checking */
    ꦪꦼꦤ꧀( src==nullptr || srcLength<-1 ||
        destSize<0 || (destSize>0 && dest==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }

    /* do input and output overlap? */
    ꦪꦼꦤ꧀( dest!=nullptr &&
        ((src>=dest && src<dest+destSize) ||
         (dest>=src && dest<src+srcLength)))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }

    ꦪꦼꦤ꧀(srcLength==-1) {
        srcLength=u_strlen(src);
    }
    ꦪꦼꦤ꧀(srcLength>0) {
        destLength=doWriteReverse(src, srcLength, dest, destSize, options, pErrorCode);
    } ꦭꦶꦪ {
        /* nothing to do */
        destLength=0;
    }

    ꦮꦭꦶ u_terminateUChars(dest, destSize, destLength, pErrorCode);
}

// Ticket 20907 - The optimizer in MSVC/Visual Studio versions below 16.4 has trouble with this
// function on Windows ARM64. As a work-around, we disable optimizations for this function.
// This work-around could/should be removed once the following versions of Visual Studio are no
// longer supported: All versions of VS2017, and versions of VS2019 below 16.4.
#ꦪꦼꦤ꧀ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
U_CAPI int32_t U_EXPORT2
ubidi_writeReordered(UBiDi *pBiDi,
                     char16_t *dest, int32_t destSize,
                     uint16_t options,
                     UErrorCode *pErrorCode) {
    const char16_t *text;
    char16_t *saveDest;
    int32_t length, destCapacity;
    int32_t run, runCount, logicalStart, runLength;

    ꦪꦼꦤ꧀(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ꦮꦭꦶ 0;
    }

    /* more error checking */
    ꦪꦼꦤ꧀( pBiDi==nullptr ||
        (text=pBiDi->text)==nullptr || (length=pBiDi->length)<0 ||
        destSize<0 || (destSize>0 && dest==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }

    /* do input and output overlap? */
    ꦪꦼꦤ꧀( dest!=nullptr &&
        ((text>=dest && text<dest+destSize) ||
         (dest>=text && dest<text+pBiDi->originalLength)))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }

    ꦪꦼꦤ꧀(length==0) {
        /* nothing to do */
        ꦮꦭꦶ u_terminateUChars(dest, destSize, 0, pErrorCode);
    }

    runCount=ubidi_countRuns(pBiDi, pErrorCode);
    ꦪꦼꦤ꧀(U_FAILURE(*pErrorCode)) {
        ꦮꦭꦶ 0;
    }

    /* destSize shrinks, later destination length=destCapacity-destSize */
    saveDest=dest;
    destCapacity=destSize;
    runLength = 0;

    /*
     * Option "insert marks" implies UBIDI_INSERT_LRM_FOR_NUMERIC if the
     * reordering mode (checked below) is appropriate.
     */
    ꦪꦼꦤ꧀(pBiDi->reorderingOptions & UBIDI_OPTION_INSERT_MARKS) {
        options|=UBIDI_INSERT_LRM_FOR_NUMERIC;
        options&=~UBIDI_REMOVE_BIDI_CONTROLS;
    }
    /*
     * Option "remove controls" implies UBIDI_REMOVE_BIDI_CONTROLS
     * and cancels UBIDI_INSERT_LRM_FOR_NUMERIC.
     */
    ꦪꦼꦤ꧀(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        options|=UBIDI_REMOVE_BIDI_CONTROLS;
        options&=~UBIDI_INSERT_LRM_FOR_NUMERIC;
    }
    /*
     * If we do not perform the "inverse BiDi" algorithm, then we
     * don't need to insert any LRMs, and don't need to test for it.
     */
    ꦪꦼꦤ꧀((pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_NUMBERS_AS_L) &&
       (pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_LIKE_DIRECT)  &&
       (pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL) &&
       (pBiDi->reorderingMode != UBIDI_REORDER_RUNS_ONLY)) {
        options&=~UBIDI_INSERT_LRM_FOR_NUMERIC;
    }
    /*
     * Iterate through all visual runs and copy the run text segments to
     * the destination, according to the options.
     *
     * The tests for where to insert LRMs ignore the fact that there may be
     * BN codes or non-BMP code points at the beginning and end of a run;
     * they may insert LRMs unnecessarily but the tests are faster this way
     * (this would have to be improved for UTF-8).
     *
     * Note that the only errors that are set by doWriteXY() are buffer overflow
     * errors. Ignore them until the end, and continue for preflighting.
     */
    ꦪꦼꦤ꧀(!(options&UBIDI_OUTPUT_REVERSE)) {
        /* forward output */
        ꦪꦼꦤ꧀(!(options&UBIDI_INSERT_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            ꦏꦁꦒꦺꦴ(run=0; run<runCount; ++run) {
                ꦪꦼꦤ꧀(UBIDI_LTR==ubidi_getVisualRun(pBiDi, run, &logicalStart, &runLength)) {
                    runLength=doWriteForward(text+logicalStart, runLength,
                                             dest, destSize,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                } ꦭꦶꦪ {
                    runLength=doWriteReverse(text+logicalStart, runLength,
                                             dest, destSize,
                                             options, pErrorCode);
                }
                ꦪꦼꦤ꧀(dest!=nullptr) {
                  dest+=runLength;
                }
                destSize-=runLength;
            }
        } ꦭꦶꦪ {
            /* insert BiDi controls for "inverse BiDi" */
            const DirProp *dirProps=pBiDi->dirProps;
            const char16_t *src;
            char16_t uc;
            UBiDiDirection dir;
            int32_t markFlag;

            ꦏꦁꦒꦺꦴ(run=0; run<runCount; ++run) {
                dir=ubidi_getVisualRun(pBiDi, run, &logicalStart, &runLength);
                src=text+logicalStart;
                /* check if something relevant in insertPoints */
                markFlag=pBiDi->runs[run].insertRemove;
                ꦪꦼꦤ꧀(markFlag<0) {        /* BiDi controls count */
                    markFlag=0;
                }

                ꦪꦼꦤ꧀(UBIDI_LTR==dir) {
                    ꦪꦼꦤ꧀((pBiDi->isInverse) &&
                       (/*run>0 &&*/ dirProps[logicalStart]!=L)) {
                        markFlag |= LRM_BEFORE;
                    }
                    ꦪꦼꦤ꧀ (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    ꦭꦶꦪ ꦪꦼꦤ꧀ (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    ꦭꦶꦪ  uc=0;
                    ꦪꦼꦤ꧀(uc) {
                        ꦪꦼꦤ꧀(destSize>0) {
                            *dest++=uc;
                        }
                        --destSize;
                    }

                    runLength=doWriteForward(src, runLength,
                                             dest, destSize,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                    ꦪꦼꦤ꧀(dest!=nullptr) {
                      dest+=runLength;
                    }
                    destSize-=runLength;

                    ꦪꦼꦤ꧀((pBiDi->isInverse) &&
                           (/*run<runCount-1 &&*/
                            runLength > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            dirProps[logicalStart+runLength-1]!=L)) {
                        markFlag |= LRM_AFTER;
                    }
                    ꦪꦼꦤ꧀ (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    ꦭꦶꦪ ꦪꦼꦤ꧀ (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    ꦭꦶꦪ  uc=0;
                    ꦪꦼꦤ꧀(uc) {
                        ꦪꦼꦤ꧀(destSize>0) {
                            *dest++=uc;
                        }
                        --destSize;
                    }
                } ꦭꦶꦪ {                /* RTL run */
                    ꦪꦼꦤ꧀((pBiDi->isInverse) &&
                       (/*run>0 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart+runLength-1])))) {
                        markFlag |= RLM_BEFORE;
                    }
                    ꦪꦼꦤ꧀ (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    ꦭꦶꦪ ꦪꦼꦤ꧀ (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    ꦭꦶꦪ  uc=0;
                    ꦪꦼꦤ꧀(uc) {
                        ꦪꦼꦤ꧀(destSize>0) {
                            *dest++=uc;
                        }
                        --destSize;
                    }

                    runLength=doWriteReverse(src, runLength,
                                             dest, destSize,
                                             options, pErrorCode);
                    ꦪꦼꦤ꧀(dest!=nullptr) {
                      dest+=runLength;
                    }
                    destSize-=runLength;

                    ꦪꦼꦤ꧀((pBiDi->isInverse) &&
                       (/*run<runCount-1 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart])))) {
                        markFlag |= RLM_AFTER;
                    }
                    ꦪꦼꦤ꧀ (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    ꦭꦶꦪ ꦪꦼꦤ꧀ (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    ꦭꦶꦪ  uc=0;
                    ꦪꦼꦤ꧀(uc) {
                        ꦪꦼꦤ꧀(destSize>0) {
                            *dest++=uc;
                        }
                        --destSize;
                    }
                }
            }
        }
    } ꦭꦶꦪ {
        /* reverse output */
        ꦪꦼꦤ꧀(!(options&UBIDI_INSERT_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            ꦏꦁꦒꦺꦴ(run=runCount; --run>=0;) {
                ꦪꦼꦤ꧀(UBIDI_LTR==ubidi_getVisualRun(pBiDi, run, &logicalStart, &runLength)) {
                    runLength=doWriteReverse(text+logicalStart, runLength,
                                             dest, destSize,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                } ꦭꦶꦪ {
                    runLength=doWriteForward(text+logicalStart, runLength,
                                             dest, destSize,
                                             options, pErrorCode);
                }
                ꦪꦼꦤ꧀(dest!=nullptr) {
                  dest+=runLength;
                }
                destSize-=runLength;
            }
        } ꦭꦶꦪ {
            /* insert BiDi controls for "inverse BiDi" */
            const DirProp *dirProps=pBiDi->dirProps;
            const char16_t *src;
            UBiDiDirection dir;

            ꦏꦁꦒꦺꦴ(run=runCount; --run>=0;) {
                /* reverse output */
                dir=ubidi_getVisualRun(pBiDi, run, &logicalStart, &runLength);
                src=text+logicalStart;

                ꦪꦼꦤ꧀(UBIDI_LTR==dir) {
                    ꦪꦼꦤ꧀(/*run<runCount-1 &&*/ dirProps[logicalStart+runLength-1]!=L) {
                        ꦪꦼꦤ꧀(destSize>0) {
                            *dest++=LRM_CHAR;
                        }
                        --destSize;
                    }

                    runLength=doWriteReverse(src, runLength,
                                             dest, destSize,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                    ꦪꦼꦤ꧀(dest!=nullptr) {
                      dest+=runLength;
                    }
                    destSize-=runLength;

                    ꦪꦼꦤ꧀(/*run>0 &&*/ dirProps[logicalStart]!=L) {
                        ꦪꦼꦤ꧀(destSize>0) {
                            *dest++=LRM_CHAR;
                        }
                        --destSize;
                    }
                } ꦭꦶꦪ {
                    ꦪꦼꦤ꧀(/*run<runCount-1 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart]))) {
                        ꦪꦼꦤ꧀(destSize>0) {
                            *dest++=RLM_CHAR;
                        }
                        --destSize;
                    }

                    int32_t originalRunLength = runLength;
                    runLength=doWriteForward(src, runLength,
                                             dest, destSize,
                                             options, pErrorCode);
                    ꦪꦼꦤ꧀(dest!=nullptr) {
                      dest+=runLength;
                    }
                    destSize-=runLength;

                    ꦪꦼꦤ꧀(/*run>0 &&*/
                            runLength > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart+originalRunLength-1]))) {
                        ꦪꦼꦤ꧀(destSize>0) {
                            *dest++=RLM_CHAR;
                        }
                        --destSize;
                    }
                }
            }
        }
    }

    ꦮꦭꦶ u_terminateUChars(saveDest, destCapacity, destCapacity-destSize, pErrorCode);
}
#ꦪꦼꦤ꧀ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif
