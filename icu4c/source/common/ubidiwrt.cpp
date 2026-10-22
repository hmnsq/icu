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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/ubidi.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "ubidiimp.h"

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
#जइ defined(UTF_SIZE) && UTF_SIZE==8
# error reimplement ubidi_writeReordered() प्रति UTF-8, see comment above
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
स्थिर int32_t
doWriteForward(ध्रुव char16_t *src, int32_t srcLength,
               char16_t *dest, int32_t destSize,
               uint16_t options,
               UErrorCode *pErrorCode) {
    जइ (srcLength<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    /* optimize for several combinations of options */
    विकल्प(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING)) {
    पक्ष 0: {
        /* simply copy the LTR run to the destination */
        int32_t length=srcLength;
        जइ(destSize<length) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            प्रत्यागम srcLength;
        }
        करेहि {
            *dest++=*src++;
        } जाव(--length>0);
        प्रत्यागम srcLength;
    }
    पक्ष UBIDI_DO_MIRRORING: {
        /* do mirroring */
        int32_t i=0, j=0;
        UChar32 c;
        करेहि {
            U16_NEXT(src, i, srcLength, c);
            c=u_charMirror(c);
            int32_t charLen = U16_LENGTH(c);
            जइ(j + charLen <= destSize) {
                int32_t temp = j;
                U16_APPEND_UNSAFE(dest, temp, c);
            }
            j += charLen;
        } जाव(i<srcLength);
        जइ(destSize < j) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        जइ(j > srcLength) {
            प्रत्यागम j;
        }
        /* If mirroring caused code unit contraction (e.g., U+1DB10 contracting to U+221D),
         * pad trailing array slots up to srcLength to avoid leaving uninitialized memory.
         * If j > 0, repeat the preceding code unit; if j == 0, fallback to 0x0020 (ASCII space).
         */
        जाव(j < srcLength && j < destSize) {
            dest[j] = j > 0 ? dest[j - 1] : 0x0020;
            j++;
        }
        प्रत्यागम srcLength;
    }
    पक्ष UBIDI_REMOVE_BIDI_CONTROLS: {
        /* copy the LTR run and remove any BiDi control characters */
        int32_t remaining=destSize;
        char16_t c;
        करेहि {
            c=*src++;
            जइ(!IS_BIDI_CONTROL_CHAR(c)) {
                जइ(--remaining<0) {
                    *pErrorCode=U_BUFFER_OVERFLOW_ERROR;

                    /* preflight the length */
                    जाव(--srcLength>0) {
                        c=*src++;
                        जइ(!IS_BIDI_CONTROL_CHAR(c)) {
                            --remaining;
                        }
                    }
                    प्रत्यागम destSize-remaining;
                }
                *dest++=c;
            }
        } जाव(--srcLength>0);
        प्रत्यागम destSize-remaining;
    }
    स्वभाव: {
        /* remove BiDi control characters and do mirroring */
        int32_t i, j=0;
        UChar32 c;
        करेहि {
            i=0;
            U16_NEXT(src, i, srcLength, c);
            src+=i;
            srcLength-=i;
            जइ(!IS_BIDI_CONTROL_CHAR(c)) {
                c=u_charMirror(c);
                int32_t charLen = U16_LENGTH(c);
                जइ(j + charLen <= destSize) {
                    int32_t temp = j;
                    U16_APPEND_UNSAFE(dest, temp, c);
                }
                j += charLen;
            }
        } जाव(srcLength>0);
        जइ(destSize < j) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        प्रत्यागम j;
    }
    } /* end of switch */
}

स्थिर int32_t
doWriteReverse(ध्रुव char16_t *src, int32_t srcLength,
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

    जइ(srcLength<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* optimize for several combinations of options */
    विकल्प(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING|UBIDI_KEEP_BASE_COMBINING)) {
    पक्ष 0:
        /*
         * With none of the "complicated" options set, the destination
         * run will have the same length as the source run,
         * and there is no mirroring and no keeping combining characters
         * with their base characters.
         */
        जइ(destSize<srcLength) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            प्रत्यागम srcLength;
        }
        destSize=srcLength;

        /* preserve character integrity */
        करेहि {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=srcLength;

            /* collect code units for one base character */
            U16_PREV(src, 0, srcLength, c);

            /* copy this base character */
            j=srcLength;
            करेहि {
                *dest++=src[j++];
            } जाव(j<i);
        } जाव(srcLength>0);
        भङ्ग;
    पक्ष UBIDI_KEEP_BASE_COMBINING:
        /*
         * Here, too, the destination
         * run will have the same length as the source run,
         * and there is no mirroring.
         * We do need to keep combining characters with their base characters.
         */
        जइ(destSize<srcLength) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            प्रत्यागम srcLength;
        }
        destSize=srcLength;

        /* preserve character integrity */
        करेहि {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=srcLength;

            /* collect code units and modifier letters for one base character */
            करेहि {
                U16_PREV(src, 0, srcLength, c);
            } जाव(srcLength>0 && IS_COMBINING(u_charType(c)));

            /* copy this "user character" */
            j=srcLength;
            करेहि {
                *dest++=src[j++];
            } जाव(j<i);
        } जाव(srcLength>0);
        भङ्ग;
    स्वभाव: {
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
        जइ(options&UBIDI_INTERNAL_RUNS_ONLY) {
            जइ(!(options&UBIDI_REMOVE_BIDI_CONTROLS)) {
                i=srcLength;
            } अण्णहा {
                int32_t length=srcLength;
                char16_t ch;
                i=0;
                /* Loop over all code units in the source run to count those that are not Bidi control characters,
                 * determining the required destination buffer length for preflighting when UBIDI_REMOVE_BIDI_CONTROLS is set.
                 */
                करेहि {
                    ch=*src++;
                    जइ(!IS_BIDI_CONTROL_CHAR(ch)) {
                        ++i;
                    }
                } जाव(--length>0);
                src-=srcLength;
            }
            जइ(destSize<i) {
                *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                प्रत्यागम i;
            }
            char16_t *saveDest=dest;
            int32_t destLength=0;
            /* Loop backward across the source run segment by segment, using legacy 1-to-1 code unit stepping
             * to extract and reverse character clusters for internal setPara classification.
             */
            करेहि {
                i=srcLength;
                U16_PREV(src, 0, srcLength, c);
                जइ(options&UBIDI_KEEP_BASE_COMBINING) {
                    /* Loop backward over any combining marks attached to the base character so that the combining
                     * sequence remains attached after its base character when reversed.
                     */
                    जाव(srcLength>0 && IS_COMBINING(u_charType(c))) {
                        U16_PREV(src, 0, srcLength, c);
                    }
                }
                जइ(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                    अनुवर्त;
                }
                int32_t origBaseLen=U16_LENGTH(c);
                j=srcLength;
                जइ(options&UBIDI_DO_MIRRORING) {
                    c=u_charMirror(c);
                    int32_t k=U16_LENGTH(c);
                    जइ(destLength+k<=destSize) {
                        int32_t temp=destLength;
                        U16_APPEND_UNSAFE(saveDest, temp, c);
                    }
                    destLength+=k;
                    j+=origBaseLen;
                }
                /* Loop forward from the current segment index to copy any unmirrored code units (such as combining marks
                 * or surrogate halves) into dest in forward visual order.
                 */
                जाव(j<i) {
                    जइ(destLength<destSize) {
                        saveDest[destLength]=src[j];
                    }
                    destLength++;
                    j++;
                }
            } जाव(srcLength>0);
            जइ(destSize<destLength) {
                *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            }
            प्रत्यागम destLength;
        }
        int32_t destLength=0;
        करेहि {
            i=srcLength;
            U16_PREV(src, 0, srcLength, c);
            जइ(options&UBIDI_KEEP_BASE_COMBINING) {
                जाव(srcLength>0 && IS_COMBINING(u_charType(c))) {
                    U16_PREV(src, 0, srcLength, c);
                }
            }
            जइ(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                अनुवर्त;
            }

            int32_t origBaseLen=U16_LENGTH(c);
            j=srcLength;
            जइ(options&UBIDI_DO_MIRRORING) {
                /* mirror only the base character */
                c=u_charMirror(c);
                int32_t k=U16_LENGTH(c);
                जइ(destLength+k<=destSize) {
                    int32_t temp=destLength;
                    U16_APPEND_UNSAFE(dest, temp, c);
                }
                destLength+=k;
                j+=origBaseLen;
            }
            जाव(j<i) {
                जइ(destLength<destSize) {
                    dest[destLength]=src[j];
                }
                destLength++;
                j++;
            }
        } जाव(srcLength>0);


        जइ(destSize<destLength) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        }
        प्रत्यागम destLength;
    }
    } /* end of switch */
    प्रत्यागम destSize;
}

U_CAPI int32_t U_EXPORT2
ubidi_writeReverse(ध्रुव char16_t *src, int32_t srcLength,
                   char16_t *dest, int32_t destSize,
                   uint16_t options,
                   UErrorCode *pErrorCode) {
    int32_t destLength;

    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    /* more error checking */
    जइ( src==nullptr || srcLength<-1 ||
        destSize<0 || (destSize>0 && dest==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* do input and output overlap? */
    जइ( dest!=nullptr &&
        ((src>=dest && src<dest+destSize) ||
         (dest>=src && dest<src+srcLength)))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    जइ(srcLength==-1) {
        srcLength=u_strlen(src);
    }
    जइ(srcLength>0) {
        destLength=doWriteReverse(src, srcLength, dest, destSize, options, pErrorCode);
    } अण्णहा {
        /* nothing to do */
        destLength=0;
    }

    प्रत्यागम u_terminateUChars(dest, destSize, destLength, pErrorCode);
}

// Ticket 20907 - The optimizer in MSVC/Visual Studio versions below 16.4 has trouble with this
// function on Windows ARM64. As a work-around, we disable optimizations for this function.
// This work-around could/should be removed once the following versions of Visual Studio are no
// longer supported: All versions of VS2017, and versions of VS2019 below 16.4.
#जइ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
U_CAPI int32_t U_EXPORT2
ubidi_writeReordered(UBiDi *pBiDi,
                     char16_t *dest, int32_t destSize,
                     uint16_t options,
                     UErrorCode *pErrorCode) {
    ध्रुव char16_t *text;
    char16_t *saveDest;
    int32_t length, destCapacity;
    int32_t run, run_गणनम्, logicalStart, runLength;

    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    /* more error checking */
    जइ( pBiDi==nullptr ||
        (text=pBiDi->text)==nullptr || (length=pBiDi->length)<0 ||
        destSize<0 || (destSize>0 && dest==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* do input and output overlap? */
    जइ( dest!=nullptr &&
        ((text>=dest && text<dest+destSize) ||
         (dest>=text && dest<text+pBiDi->originalLength)))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    जइ(length==0) {
        /* nothing to do */
        प्रत्यागम u_terminateUChars(dest, destSize, 0, pErrorCode);
    }

    run_गणनम्=ubidi_गणनम्_Runs(pBiDi, pErrorCode);
    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    /* destSize shrinks, later destination length=destCapacity-destSize */
    saveDest=dest;
    destCapacity=destSize;
    runLength = 0;

    /*
     * Option "insert marks" implies UBIDI_INSERT_LRM_FOR_NUMERIC if the
     * reordering mode (checked below) is appropriate.
     */
    जइ(pBiDi->reorderingOptions & UBIDI_OPTION_निवेशय_MARKS) {
        options|=UBIDI_निवेशय_LRM_FOR_NUMERIC;
        options&=~UBIDI_REMOVE_BIDI_CONTROLS;
    }
    /*
     * Option "remove controls" implies UBIDI_REMOVE_BIDI_CONTROLS
     * and cancels UBIDI_INSERT_LRM_FOR_NUMERIC.
     */
    जइ(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        options|=UBIDI_REMOVE_BIDI_CONTROLS;
        options&=~UBIDI_निवेशय_LRM_FOR_NUMERIC;
    }
    /*
     * If we do not perform the "inverse BiDi" algorithm, then we
     * don't need to insert any LRMs, and don't need to test for it.
     */
    जइ((pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_NUMBERS_AS_L) &&
       (pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_LIKE_DIRECT)  &&
       (pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL) &&
       (pBiDi->reorderingMode != UBIDI_REORDER_RUNS_ONLY)) {
        options&=~UBIDI_निवेशय_LRM_FOR_NUMERIC;
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
    जइ(!(options&UBIDI_OUTPUT_REVERSE)) {
        /* forward output */
        जइ(!(options&UBIDI_निवेशय_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            प्रति(run=0; run<run_गणनम्; ++run) {
                जइ(UBIDI_LTR==ubidi_getVisualRun(pBiDi, run, &logicalStart, &runLength)) {
                    runLength=doWriteForward(text+logicalStart, runLength,
                                             dest, destSize,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                } अण्णहा {
                    runLength=doWriteReverse(text+logicalStart, runLength,
                                             dest, destSize,
                                             options, pErrorCode);
                }
                जइ(dest!=nullptr) {
                  dest+=runLength;
                }
                destSize-=runLength;
            }
        } अण्णहा {
            /* insert BiDi controls for "inverse BiDi" */
            ध्रुव DirProp *dirProps=pBiDi->dirProps;
            ध्रुव char16_t *src;
            char16_t uc;
            UBiDiDirection dir;
            int32_t markFlag;

            प्रति(run=0; run<run_गणनम्; ++run) {
                dir=ubidi_getVisualRun(pBiDi, run, &logicalStart, &runLength);
                src=text+logicalStart;
                /* check if something relevant in insertPoints */
                markFlag=pBiDi->runs[run].निवेशय_Remove;
                जइ(markFlag<0) {        /* BiDi controls count */
                    markFlag=0;
                }

                जइ(UBIDI_LTR==dir) {
                    जइ((pBiDi->isInverse) &&
                       (/*run>0 &&*/ dirProps[logicalStart]!=L)) {
                        markFlag |= LRM_BEFORE;
                    }
                    जइ (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    अण्णहा जइ (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    अण्णहा  uc=0;
                    जइ(uc) {
                        जइ(destSize>0) {
                            *dest++=uc;
                        }
                        --destSize;
                    }

                    runLength=doWriteForward(src, runLength,
                                             dest, destSize,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                    जइ(dest!=nullptr) {
                      dest+=runLength;
                    }
                    destSize-=runLength;

                    जइ((pBiDi->isInverse) &&
                           (/*run<runCount-1 &&*/
                            runLength > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            dirProps[logicalStart+runLength-1]!=L)) {
                        markFlag |= LRM_AFTER;
                    }
                    जइ (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    अण्णहा जइ (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    अण्णहा  uc=0;
                    जइ(uc) {
                        जइ(destSize>0) {
                            *dest++=uc;
                        }
                        --destSize;
                    }
                } अण्णहा {                /* RTL run */
                    जइ((pBiDi->isInverse) &&
                       (/*run>0 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart+runLength-1])))) {
                        markFlag |= RLM_BEFORE;
                    }
                    जइ (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    अण्णहा जइ (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    अण्णहा  uc=0;
                    जइ(uc) {
                        जइ(destSize>0) {
                            *dest++=uc;
                        }
                        --destSize;
                    }

                    runLength=doWriteReverse(src, runLength,
                                             dest, destSize,
                                             options, pErrorCode);
                    जइ(dest!=nullptr) {
                      dest+=runLength;
                    }
                    destSize-=runLength;

                    जइ((pBiDi->isInverse) &&
                       (/*run<runCount-1 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart])))) {
                        markFlag |= RLM_AFTER;
                    }
                    जइ (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    अण्णहा जइ (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    अण्णहा  uc=0;
                    जइ(uc) {
                        जइ(destSize>0) {
                            *dest++=uc;
                        }
                        --destSize;
                    }
                }
            }
        }
    } अण्णहा {
        /* reverse output */
        जइ(!(options&UBIDI_निवेशय_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            प्रति(run=run_गणनम्; --run>=0;) {
                जइ(UBIDI_LTR==ubidi_getVisualRun(pBiDi, run, &logicalStart, &runLength)) {
                    runLength=doWriteReverse(text+logicalStart, runLength,
                                             dest, destSize,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                } अण्णहा {
                    runLength=doWriteForward(text+logicalStart, runLength,
                                             dest, destSize,
                                             options, pErrorCode);
                }
                जइ(dest!=nullptr) {
                  dest+=runLength;
                }
                destSize-=runLength;
            }
        } अण्णहा {
            /* insert BiDi controls for "inverse BiDi" */
            ध्रुव DirProp *dirProps=pBiDi->dirProps;
            ध्रुव char16_t *src;
            UBiDiDirection dir;

            प्रति(run=run_गणनम्; --run>=0;) {
                /* reverse output */
                dir=ubidi_getVisualRun(pBiDi, run, &logicalStart, &runLength);
                src=text+logicalStart;

                जइ(UBIDI_LTR==dir) {
                    जइ(/*run<runCount-1 &&*/ dirProps[logicalStart+runLength-1]!=L) {
                        जइ(destSize>0) {
                            *dest++=LRM_CHAR;
                        }
                        --destSize;
                    }

                    runLength=doWriteReverse(src, runLength,
                                             dest, destSize,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                    जइ(dest!=nullptr) {
                      dest+=runLength;
                    }
                    destSize-=runLength;

                    जइ(/*run>0 &&*/ dirProps[logicalStart]!=L) {
                        जइ(destSize>0) {
                            *dest++=LRM_CHAR;
                        }
                        --destSize;
                    }
                } अण्णहा {
                    जइ(/*run<runCount-1 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart]))) {
                        जइ(destSize>0) {
                            *dest++=RLM_CHAR;
                        }
                        --destSize;
                    }

                    int32_t originalRunLength = runLength;
                    runLength=doWriteForward(src, runLength,
                                             dest, destSize,
                                             options, pErrorCode);
                    जइ(dest!=nullptr) {
                      dest+=runLength;
                    }
                    destSize-=runLength;

                    जइ(/*run>0 &&*/
                            runLength > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart+originalRunLength-1]))) {
                        जइ(destSize>0) {
                            *dest++=RLM_CHAR;
                        }
                        --destSize;
                    }
                }
            }
        }
    }

    प्रत्यागम u_terminateUChars(saveDest, destCapacity, destCapacity-destSize, pErrorCode);
}
#जइ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif
