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

#ضمن "unicode/utypes.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/ubidi.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "ustr_imp.h"
#ضمن "ubidiimp.h"

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
#إذا defined(UTF_SIZE) && UTF_SIZE==8
# error reimplement ubidi_writeReordered() لكل UTF-8, see comment above
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
ساكن int32_t
doWriteForward(ثابت char16_t *src, int32_t srcLength,
               char16_t *dest, int32_t destSize,
               uint16_t options,
               UErrorCode *pErrorCode) {
    إذا (srcLength<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع 0;
    }
    /* optimize for several combinations of options */
    بدل(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING)) {
    حالة 0: {
        /* simply copy the LTR run to the destination */
        int32_t length=srcLength;
        إذا(destSize<length) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            إرجاع srcLength;
        }
        افعل {
            *dest++=*src++;
        } بينما(--length>0);
        إرجاع srcLength;
    }
    حالة UBIDI_DO_MIRRORING: {
        /* do mirroring */
        int32_t i=0, j=0;
        UChar32 c;
        افعل {
            U16_NEXT(src, i, srcLength, c);
            c=u_charMirror(c);
            int32_t charLen = U16_LENGTH(c);
            إذا(j + charLen <= destSize) {
                int32_t temp = j;
                U16_APPEND_UNSAFE(dest, temp, c);
            }
            j += charLen;
        } بينما(i<srcLength);
        إذا(destSize < j) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        إذا(j > srcLength) {
            إرجاع j;
        }
        /* If mirroring caused code unit contraction (e.g., U+1DB10 contracting to U+221D),
         * pad trailing array slots up to srcLength to avoid leaving uninitialized memory.
         * If j > 0, repeat the preceding code unit; if j == 0, fallback to 0x0020 (ASCII space).
         */
        بينما(j < srcLength && j < destSize) {
            dest[j] = j > 0 ? dest[j - 1] : 0x0020;
            j++;
        }
        إرجاع srcLength;
    }
    حالة UBIDI_REMOVE_BIDI_CONTROLS: {
        /* copy the LTR run and remove any BiDi control characters */
        int32_t remaining=destSize;
        char16_t c;
        افعل {
            c=*src++;
            إذا(!IS_BIDI_CONTROL_CHAR(c)) {
                إذا(--remaining<0) {
                    *pErrorCode=U_BUFFER_OVERFLOW_ERROR;

                    /* preflight the length */
                    بينما(--srcLength>0) {
                        c=*src++;
                        إذا(!IS_BIDI_CONTROL_CHAR(c)) {
                            --remaining;
                        }
                    }
                    إرجاع destSize-remaining;
                }
                *dest++=c;
            }
        } بينما(--srcLength>0);
        إرجاع destSize-remaining;
    }
    افتراضي: {
        /* remove BiDi control characters and do mirroring */
        int32_t i, j=0;
        UChar32 c;
        افعل {
            i=0;
            U16_NEXT(src, i, srcLength, c);
            src+=i;
            srcLength-=i;
            إذا(!IS_BIDI_CONTROL_CHAR(c)) {
                c=u_charMirror(c);
                int32_t charLen = U16_LENGTH(c);
                إذا(j + charLen <= destSize) {
                    int32_t temp = j;
                    U16_APPEND_UNSAFE(dest, temp, c);
                }
                j += charLen;
            }
        } بينما(srcLength>0);
        إذا(destSize < j) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        }
        إرجاع j;
    }
    } /* end of switch */
}

ساكن int32_t
doWriteReverse(ثابت char16_t *src, int32_t srcLength,
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

    إذا(srcLength<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع 0;
    }

    /* optimize for several combinations of options */
    بدل(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING|UBIDI_KEEP_BASE_COMBINING)) {
    حالة 0:
        /*
         * With none of the "complicated" options set, the destination
         * run will have the same length as the source run,
         * and there is no mirroring and no keeping combining characters
         * with their base characters.
         */
        إذا(destSize<srcLength) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            إرجاع srcLength;
        }
        destSize=srcLength;

        /* preserve character integrity */
        افعل {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=srcLength;

            /* collect code units for one base character */
            U16_PREV(src, 0, srcLength, c);

            /* copy this base character */
            j=srcLength;
            افعل {
                *dest++=src[j++];
            } بينما(j<i);
        } بينما(srcLength>0);
        اكسر;
    حالة UBIDI_KEEP_BASE_COMBINING:
        /*
         * Here, too, the destination
         * run will have the same length as the source run,
         * and there is no mirroring.
         * We do need to keep combining characters with their base characters.
         */
        إذا(destSize<srcLength) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            إرجاع srcLength;
        }
        destSize=srcLength;

        /* preserve character integrity */
        افعل {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=srcLength;

            /* collect code units and modifier letters for one base character */
            افعل {
                U16_PREV(src, 0, srcLength, c);
            } بينما(srcLength>0 && IS_COMBINING(u_charType(c)));

            /* copy this "user character" */
            j=srcLength;
            افعل {
                *dest++=src[j++];
            } بينما(j<i);
        } بينما(srcLength>0);
        اكسر;
    افتراضي: {
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
        إذا(options&UBIDI_INTERNAL_RUNS_ONLY) {
            إذا(!(options&UBIDI_REMOVE_BIDI_CONTROLS)) {
                i=srcLength;
            } وإلا {
                int32_t length=srcLength;
                char16_t ch;
                i=0;
                /* Loop over all code units in the source run to count those that are not Bidi control characters,
                 * determining the required destination buffer length for preflighting when UBIDI_REMOVE_BIDI_CONTROLS is set.
                 */
                افعل {
                    ch=*src++;
                    إذا(!IS_BIDI_CONTROL_CHAR(ch)) {
                        ++i;
                    }
                } بينما(--length>0);
                src-=srcLength;
            }
            إذا(destSize<i) {
                *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
                إرجاع i;
            }
            char16_t *saveDest=dest;
            int32_t destLength=0;
            /* Loop backward across the source run segment by segment, using legacy 1-to-1 code unit stepping
             * to extract and reverse character clusters for internal setPara classification.
             */
            افعل {
                i=srcLength;
                U16_PREV(src, 0, srcLength, c);
                إذا(options&UBIDI_KEEP_BASE_COMBINING) {
                    /* Loop backward over any combining marks attached to the base character so that the combining
                     * sequence remains attached after its base character when reversed.
                     */
                    بينما(srcLength>0 && IS_COMBINING(u_charType(c))) {
                        U16_PREV(src, 0, srcLength, c);
                    }
                }
                إذا(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                    استمر;
                }
                int32_t origBaseLen=U16_LENGTH(c);
                j=srcLength;
                إذا(options&UBIDI_DO_MIRRORING) {
                    c=u_charMirror(c);
                    int32_t k=U16_LENGTH(c);
                    إذا(destLength+k<=destSize) {
                        int32_t temp=destLength;
                        U16_APPEND_UNSAFE(saveDest, temp, c);
                    }
                    destLength+=k;
                    j+=origBaseLen;
                }
                /* Loop forward from the current segment index to copy any unmirrored code units (such as combining marks
                 * or surrogate halves) into dest in forward visual order.
                 */
                بينما(j<i) {
                    إذا(destLength<destSize) {
                        saveDest[destLength]=src[j];
                    }
                    destLength++;
                    j++;
                }
            } بينما(srcLength>0);
            إذا(destSize<destLength) {
                *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
            }
            إرجاع destLength;
        }
        int32_t destLength=0;
        افعل {
            i=srcLength;
            U16_PREV(src, 0, srcLength, c);
            إذا(options&UBIDI_KEEP_BASE_COMBINING) {
                بينما(srcLength>0 && IS_COMBINING(u_charType(c))) {
                    U16_PREV(src, 0, srcLength, c);
                }
            }
            إذا(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                استمر;
            }

            int32_t origBaseLen=U16_LENGTH(c);
            j=srcLength;
            إذا(options&UBIDI_DO_MIRRORING) {
                /* mirror only the base character */
                c=u_charMirror(c);
                int32_t k=U16_LENGTH(c);
                إذا(destLength+k<=destSize) {
                    int32_t temp=destLength;
                    U16_APPEND_UNSAFE(dest, temp, c);
                }
                destLength+=k;
                j+=origBaseLen;
            }
            بينما(j<i) {
                إذا(destLength<destSize) {
                    dest[destLength]=src[j];
                }
                destLength++;
                j++;
            }
        } بينما(srcLength>0);


        إذا(destSize<destLength) {
            *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        }
        إرجاع destLength;
    }
    } /* end of switch */
    إرجاع destSize;
}

U_CAPI int32_t U_EXPORT2
ubidi_writeReverse(ثابت char16_t *src, int32_t srcLength,
                   char16_t *dest, int32_t destSize,
                   uint16_t options,
                   UErrorCode *pErrorCode) {
    int32_t destLength;

    إذا(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        إرجاع 0;
    }

    /* more error checking */
    إذا( src==nullptr || srcLength<-1 ||
        destSize<0 || (destSize>0 && dest==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع 0;
    }

    /* do input and output overlap? */
    إذا( dest!=nullptr &&
        ((src>=dest && src<dest+destSize) ||
         (dest>=src && dest<src+srcLength)))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع 0;
    }

    إذا(srcLength==-1) {
        srcLength=u_strlen(src);
    }
    إذا(srcLength>0) {
        destLength=doWriteReverse(src, srcLength, dest, destSize, options, pErrorCode);
    } وإلا {
        /* nothing to do */
        destLength=0;
    }

    إرجاع u_terminateUChars(dest, destSize, destLength, pErrorCode);
}

// Ticket 20907 - The optimizer in MSVC/Visual Studio versions below 16.4 has trouble with this
// function on Windows ARM64. As a work-around, we disable optimizations for this function.
// This work-around could/should be removed once the following versions of Visual Studio are no
// longer supported: All versions of VS2017, and versions of VS2019 below 16.4.
#إذا (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
U_CAPI int32_t U_EXPORT2
ubidi_writeReordered(UBiDi *pBiDi,
                     char16_t *dest, int32_t destSize,
                     uint16_t options,
                     UErrorCode *pErrorCode) {
    ثابت char16_t *text;
    char16_t *saveDest;
    int32_t length, destCapacity;
    int32_t run, run_تعداد, logicalStart, runLength;

    إذا(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        إرجاع 0;
    }

    /* more error checking */
    إذا( pBiDi==nullptr ||
        (text=pBiDi->text)==nullptr || (length=pBiDi->length)<0 ||
        destSize<0 || (destSize>0 && dest==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع 0;
    }

    /* do input and output overlap? */
    إذا( dest!=nullptr &&
        ((text>=dest && text<dest+destSize) ||
         (dest>=text && dest<text+pBiDi->originalLength)))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع 0;
    }

    إذا(length==0) {
        /* nothing to do */
        إرجاع u_terminateUChars(dest, destSize, 0, pErrorCode);
    }

    run_تعداد=ubidi_تعداد_Runs(pBiDi, pErrorCode);
    إذا(U_FAILURE(*pErrorCode)) {
        إرجاع 0;
    }

    /* destSize shrinks, later destination length=destCapacity-destSize */
    saveDest=dest;
    destCapacity=destSize;
    runLength = 0;

    /*
     * Option "insert marks" implies UBIDI_INSERT_LRM_FOR_NUMERIC if the
     * reordering mode (checked below) is appropriate.
     */
    إذا(pBiDi->reorderingOptions & UBIDI_OPTION_أدرج_MARKS) {
        options|=UBIDI_أدرج_LRM_FOR_NUMERIC;
        options&=~UBIDI_REMOVE_BIDI_CONTROLS;
    }
    /*
     * Option "remove controls" implies UBIDI_REMOVE_BIDI_CONTROLS
     * and cancels UBIDI_INSERT_LRM_FOR_NUMERIC.
     */
    إذا(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        options|=UBIDI_REMOVE_BIDI_CONTROLS;
        options&=~UBIDI_أدرج_LRM_FOR_NUMERIC;
    }
    /*
     * If we do not perform the "inverse BiDi" algorithm, then we
     * don't need to insert any LRMs, and don't need to test for it.
     */
    إذا((pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_NUMBERS_AS_L) &&
       (pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_LIKE_DIRECT)  &&
       (pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL) &&
       (pBiDi->reorderingMode != UBIDI_REORDER_RUNS_ONLY)) {
        options&=~UBIDI_أدرج_LRM_FOR_NUMERIC;
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
    إذا(!(options&UBIDI_OUTPUT_REVERSE)) {
        /* forward output */
        إذا(!(options&UBIDI_أدرج_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            لكل(run=0; run<run_تعداد; ++run) {
                إذا(UBIDI_LTR==ubidi_getVisualRun(pBiDi, run, &logicalStart, &runLength)) {
                    runLength=doWriteForward(text+logicalStart, runLength,
                                             dest, destSize,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                } وإلا {
                    runLength=doWriteReverse(text+logicalStart, runLength,
                                             dest, destSize,
                                             options, pErrorCode);
                }
                إذا(dest!=nullptr) {
                  dest+=runLength;
                }
                destSize-=runLength;
            }
        } وإلا {
            /* insert BiDi controls for "inverse BiDi" */
            ثابت DirProp *dirProps=pBiDi->dirProps;
            ثابت char16_t *src;
            char16_t uc;
            UBiDiDirection dir;
            int32_t markFlag;

            لكل(run=0; run<run_تعداد; ++run) {
                dir=ubidi_getVisualRun(pBiDi, run, &logicalStart, &runLength);
                src=text+logicalStart;
                /* check if something relevant in insertPoints */
                markFlag=pBiDi->runs[run].أدرج_Remove;
                إذا(markFlag<0) {        /* BiDi controls count */
                    markFlag=0;
                }

                إذا(UBIDI_LTR==dir) {
                    إذا((pBiDi->isInverse) &&
                       (/*run>0 &&*/ dirProps[logicalStart]!=L)) {
                        markFlag |= LRM_BEFORE;
                    }
                    إذا (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    وإلا إذا (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    وإلا  uc=0;
                    إذا(uc) {
                        إذا(destSize>0) {
                            *dest++=uc;
                        }
                        --destSize;
                    }

                    runLength=doWriteForward(src, runLength,
                                             dest, destSize,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                    إذا(dest!=nullptr) {
                      dest+=runLength;
                    }
                    destSize-=runLength;

                    إذا((pBiDi->isInverse) &&
                           (/*run<runCount-1 &&*/
                            runLength > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            dirProps[logicalStart+runLength-1]!=L)) {
                        markFlag |= LRM_AFTER;
                    }
                    إذا (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    وإلا إذا (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    وإلا  uc=0;
                    إذا(uc) {
                        إذا(destSize>0) {
                            *dest++=uc;
                        }
                        --destSize;
                    }
                } وإلا {                /* RTL run */
                    إذا((pBiDi->isInverse) &&
                       (/*run>0 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart+runLength-1])))) {
                        markFlag |= RLM_BEFORE;
                    }
                    إذا (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    وإلا إذا (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    وإلا  uc=0;
                    إذا(uc) {
                        إذا(destSize>0) {
                            *dest++=uc;
                        }
                        --destSize;
                    }

                    runLength=doWriteReverse(src, runLength,
                                             dest, destSize,
                                             options, pErrorCode);
                    إذا(dest!=nullptr) {
                      dest+=runLength;
                    }
                    destSize-=runLength;

                    إذا((pBiDi->isInverse) &&
                       (/*run<runCount-1 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart])))) {
                        markFlag |= RLM_AFTER;
                    }
                    إذا (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    وإلا إذا (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    وإلا  uc=0;
                    إذا(uc) {
                        إذا(destSize>0) {
                            *dest++=uc;
                        }
                        --destSize;
                    }
                }
            }
        }
    } وإلا {
        /* reverse output */
        إذا(!(options&UBIDI_أدرج_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            لكل(run=run_تعداد; --run>=0;) {
                إذا(UBIDI_LTR==ubidi_getVisualRun(pBiDi, run, &logicalStart, &runLength)) {
                    runLength=doWriteReverse(text+logicalStart, runLength,
                                             dest, destSize,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                } وإلا {
                    runLength=doWriteForward(text+logicalStart, runLength,
                                             dest, destSize,
                                             options, pErrorCode);
                }
                إذا(dest!=nullptr) {
                  dest+=runLength;
                }
                destSize-=runLength;
            }
        } وإلا {
            /* insert BiDi controls for "inverse BiDi" */
            ثابت DirProp *dirProps=pBiDi->dirProps;
            ثابت char16_t *src;
            UBiDiDirection dir;

            لكل(run=run_تعداد; --run>=0;) {
                /* reverse output */
                dir=ubidi_getVisualRun(pBiDi, run, &logicalStart, &runLength);
                src=text+logicalStart;

                إذا(UBIDI_LTR==dir) {
                    إذا(/*run<runCount-1 &&*/ dirProps[logicalStart+runLength-1]!=L) {
                        إذا(destSize>0) {
                            *dest++=LRM_CHAR;
                        }
                        --destSize;
                    }

                    runLength=doWriteReverse(src, runLength,
                                             dest, destSize,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                    إذا(dest!=nullptr) {
                      dest+=runLength;
                    }
                    destSize-=runLength;

                    إذا(/*run>0 &&*/ dirProps[logicalStart]!=L) {
                        إذا(destSize>0) {
                            *dest++=LRM_CHAR;
                        }
                        --destSize;
                    }
                } وإلا {
                    إذا(/*run<runCount-1 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart]))) {
                        إذا(destSize>0) {
                            *dest++=RLM_CHAR;
                        }
                        --destSize;
                    }

                    int32_t originalRunLength = runLength;
                    runLength=doWriteForward(src, runLength,
                                             dest, destSize,
                                             options, pErrorCode);
                    إذا(dest!=nullptr) {
                      dest+=runLength;
                    }
                    destSize-=runLength;

                    إذا(/*run>0 &&*/
                            runLength > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart+originalRunLength-1]))) {
                        إذا(destSize>0) {
                            *dest++=RLM_CHAR;
                        }
                        --destSize;
                    }
                }
            }
        }
    }

    إرجاع u_terminateUChars(saveDest, destCapacity, destCapacity-destSize, pErrorCode);
}
#إذا (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif
