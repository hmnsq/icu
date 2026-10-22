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

#समावेश "unicode/utypes.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/ubidi.h"
#समावेश "unicode/utf16.h"
#समावेश "cmemory.h"
#समावेश "ustr_imp.h"
#समावेश "ubidiimp.h"

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
#अगर defined(UTF_आकार) && UTF_आकार==8
# error reimplement ubidi_writeReordered() क्रम UTF-8, see comment above
#पूर्ण_अगर

#घोषणा IS_COMBINING(type) ((1UL<<(type))&(1UL<<U_NON_SPACING_MARK|1UL<<U_COMBINING_SPACING_MARK|1UL<<U_ENCLOSING_MARK))

/*
 * When we have UBIDI_OUTPUT_REVERSE set on ubidi_writeReordered(), then we
 * semantically write RTL runs in reverse and later reverse them again.
 * Instead, we actually write them in forward order to begin with.
 * However, if the RTL run was to be mirrored, we need to mirror here now
 * since the implicit second reversal must not do it.
 * It looks strange to do mirroring in LTR output, but it is only because
 * we are writing RTL output in reverse.
 */
अटल int32_t
doWriteForward(स्थिर char16_t *src, int32_t src_लंबाई,
               char16_t *dest, int32_t dest_आकार,
               uint16_t options,
               UErrorCode *pErrorCode) {
    अगर (src_लंबाई<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    /* optimize for several combinations of options */
    चयन(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING)) {
    हाल 0: {
        /* simply copy the LTR run to the destination */
        int32_t length=src_लंबाई;
        अगर(dest_आकार<length) {
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            वापस src_लंबाई;
        }
        करो {
            *dest++=*src++;
        } जबतक(--length>0);
        वापस src_लंबाई;
    }
    हाल UBIDI_DO_MIRRORING: {
        /* do mirroring */
        int32_t i=0, j=0;
        UChar32 c;
        करो {
            U16_अग्र(src, i, src_लंबाई, c);
            c=u_charMirror(c);
            int32_t charLen = U16_लंबाई(c);
            अगर(j + charLen <= dest_आकार) {
                int32_t अस्थायी = j;
                U16_APPEND_UNSAFE(dest, अस्थायी, c);
            }
            j += charLen;
        } जबतक(i<src_लंबाई);
        अगर(dest_आकार < j) {
            *pErrorCode = U_बफर_OVERFLOW_ERROR;
        }
        अगर(j > src_लंबाई) {
            वापस j;
        }
        /* If mirroring caused code unit contraction (e.g., U+1DB10 contracting to U+221D),
         * pad trailing array slots up to srcLength to avoid leaving uninitialized memory.
         * If j > 0, repeat the preceding code unit; if j == 0, fallback to 0x0020 (ASCII space).
         */
        जबतक(j < src_लंबाई && j < dest_आकार) {
            dest[j] = j > 0 ? dest[j - 1] : 0x0020;
            j++;
        }
        वापस src_लंबाई;
    }
    हाल UBIDI_REMOVE_BIDI_CONTROLS: {
        /* copy the LTR run and remove any BiDi control characters */
        int32_t remaining=dest_आकार;
        char16_t c;
        करो {
            c=*src++;
            अगर(!IS_BIDI_CONTROL_CHAR(c)) {
                अगर(--remaining<0) {
                    *pErrorCode=U_बफर_OVERFLOW_ERROR;

                    /* preflight the length */
                    जबतक(--src_लंबाई>0) {
                        c=*src++;
                        अगर(!IS_BIDI_CONTROL_CHAR(c)) {
                            --remaining;
                        }
                    }
                    वापस dest_आकार-remaining;
                }
                *dest++=c;
            }
        } जबतक(--src_लंबाई>0);
        वापस dest_आकार-remaining;
    }
    शेष: {
        /* remove BiDi control characters and do mirroring */
        int32_t i, j=0;
        UChar32 c;
        करो {
            i=0;
            U16_अग्र(src, i, src_लंबाई, c);
            src+=i;
            src_लंबाई-=i;
            अगर(!IS_BIDI_CONTROL_CHAR(c)) {
                c=u_charMirror(c);
                int32_t charLen = U16_लंबाई(c);
                अगर(j + charLen <= dest_आकार) {
                    int32_t अस्थायी = j;
                    U16_APPEND_UNSAFE(dest, अस्थायी, c);
                }
                j += charLen;
            }
        } जबतक(src_लंबाई>0);
        अगर(dest_आकार < j) {
            *pErrorCode = U_बफर_OVERFLOW_ERROR;
        }
        वापस j;
    }
    } /* end of switch */
}

अटल int32_t
doWriteReverse(स्थिर char16_t *src, int32_t src_लंबाई,
               char16_t *dest, int32_t dest_आकार,
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

    अगर(src_लंबाई<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* optimize for several combinations of options */
    चयन(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING|UBIDI_KEEP_BASE_COMBINING)) {
    हाल 0:
        /*
         * With none of the "complicated" options set, the destination
         * run will have the same length as the source run,
         * and there is no mirroring and no keeping combining characters
         * with their base characters.
         */
        अगर(dest_आकार<src_लंबाई) {
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            वापस src_लंबाई;
        }
        dest_आकार=src_लंबाई;

        /* preserve character integrity */
        करो {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_लंबाई;

            /* collect code units for one base character */
            U16_पूर्व(src, 0, src_लंबाई, c);

            /* copy this base character */
            j=src_लंबाई;
            करो {
                *dest++=src[j++];
            } जबतक(j<i);
        } जबतक(src_लंबाई>0);
        अवरोध;
    हाल UBIDI_KEEP_BASE_COMBINING:
        /*
         * Here, too, the destination
         * run will have the same length as the source run,
         * and there is no mirroring.
         * We do need to keep combining characters with their base characters.
         */
        अगर(dest_आकार<src_लंबाई) {
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
            वापस src_लंबाई;
        }
        dest_आकार=src_लंबाई;

        /* preserve character integrity */
        करो {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_लंबाई;

            /* collect code units and modifier letters for one base character */
            करो {
                U16_पूर्व(src, 0, src_लंबाई, c);
            } जबतक(src_लंबाई>0 && IS_COMBINING(u_charType(c)));

            /* copy this "user character" */
            j=src_लंबाई;
            करो {
                *dest++=src[j++];
            } जबतक(j<i);
        } जबतक(src_लंबाई>0);
        अवरोध;
    शेष: {
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
        अगर(options&UBIDI_INTERNAL_RUNS_ONLY) {
            अगर(!(options&UBIDI_REMOVE_BIDI_CONTROLS)) {
                i=src_लंबाई;
            } अन्यथा {
                int32_t length=src_लंबाई;
                char16_t ch;
                i=0;
                /* Loop over all code units in the source run to count those that are not Bidi control characters,
                 * determining the required destination buffer length for preflighting when UBIDI_REMOVE_BIDI_CONTROLS is set.
                 */
                करो {
                    ch=*src++;
                    अगर(!IS_BIDI_CONTROL_CHAR(ch)) {
                        ++i;
                    }
                } जबतक(--length>0);
                src-=src_लंबाई;
            }
            अगर(dest_आकार<i) {
                *pErrorCode=U_बफर_OVERFLOW_ERROR;
                वापस i;
            }
            char16_t *saveDest=dest;
            int32_t dest_लंबाई=0;
            /* Loop backward across the source run segment by segment, using legacy 1-to-1 code unit stepping
             * to extract and reverse character clusters for internal setPara classification.
             */
            करो {
                i=src_लंबाई;
                U16_पूर्व(src, 0, src_लंबाई, c);
                अगर(options&UBIDI_KEEP_BASE_COMBINING) {
                    /* Loop backward over any combining marks attached to the base character so that the combining
                     * sequence remains attached after its base character when reversed.
                     */
                    जबतक(src_लंबाई>0 && IS_COMBINING(u_charType(c))) {
                        U16_पूर्व(src, 0, src_लंबाई, c);
                    }
                }
                अगर(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                    जारी;
                }
                int32_t origBaseLen=U16_लंबाई(c);
                j=src_लंबाई;
                अगर(options&UBIDI_DO_MIRRORING) {
                    c=u_charMirror(c);
                    int32_t k=U16_लंबाई(c);
                    अगर(dest_लंबाई+k<=dest_आकार) {
                        int32_t अस्थायी=dest_लंबाई;
                        U16_APPEND_UNSAFE(saveDest, अस्थायी, c);
                    }
                    dest_लंबाई+=k;
                    j+=origBaseLen;
                }
                /* Loop forward from the current segment index to copy any unmirrored code units (such as combining marks
                 * or surrogate halves) into dest in forward visual order.
                 */
                जबतक(j<i) {
                    अगर(dest_लंबाई<dest_आकार) {
                        saveDest[dest_लंबाई]=src[j];
                    }
                    dest_लंबाई++;
                    j++;
                }
            } जबतक(src_लंबाई>0);
            अगर(dest_आकार<dest_लंबाई) {
                *pErrorCode=U_बफर_OVERFLOW_ERROR;
            }
            वापस dest_लंबाई;
        }
        int32_t dest_लंबाई=0;
        करो {
            i=src_लंबाई;
            U16_पूर्व(src, 0, src_लंबाई, c);
            अगर(options&UBIDI_KEEP_BASE_COMBINING) {
                जबतक(src_लंबाई>0 && IS_COMBINING(u_charType(c))) {
                    U16_पूर्व(src, 0, src_लंबाई, c);
                }
            }
            अगर(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                जारी;
            }

            int32_t origBaseLen=U16_लंबाई(c);
            j=src_लंबाई;
            अगर(options&UBIDI_DO_MIRRORING) {
                /* mirror only the base character */
                c=u_charMirror(c);
                int32_t k=U16_लंबाई(c);
                अगर(dest_लंबाई+k<=dest_आकार) {
                    int32_t अस्थायी=dest_लंबाई;
                    U16_APPEND_UNSAFE(dest, अस्थायी, c);
                }
                dest_लंबाई+=k;
                j+=origBaseLen;
            }
            जबतक(j<i) {
                अगर(dest_लंबाई<dest_आकार) {
                    dest[dest_लंबाई]=src[j];
                }
                dest_लंबाई++;
                j++;
            }
        } जबतक(src_लंबाई>0);


        अगर(dest_आकार<dest_लंबाई) {
            *pErrorCode=U_बफर_OVERFLOW_ERROR;
        }
        वापस dest_लंबाई;
    }
    } /* end of switch */
    वापस dest_आकार;
}

U_CAPI int32_t U_EXPORT2
ubidi_writeReverse(स्थिर char16_t *src, int32_t src_लंबाई,
                   char16_t *dest, int32_t dest_आकार,
                   uint16_t options,
                   UErrorCode *pErrorCode) {
    int32_t dest_लंबाई;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    /* more error checking */
    अगर( src==nullptr || src_लंबाई<-1 ||
        dest_आकार<0 || (dest_आकार>0 && dest==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* do input and output overlap? */
    अगर( dest!=nullptr &&
        ((src>=dest && src<dest+dest_आकार) ||
         (dest>=src && dest<src+src_लंबाई)))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    अगर(src_लंबाई==-1) {
        src_लंबाई=u_strlen(src);
    }
    अगर(src_लंबाई>0) {
        dest_लंबाई=doWriteReverse(src, src_लंबाई, dest, dest_आकार, options, pErrorCode);
    } अन्यथा {
        /* nothing to do */
        dest_लंबाई=0;
    }

    वापस u_terminateUChars(dest, dest_आकार, dest_लंबाई, pErrorCode);
}

// Ticket 20907 - The optimizer in MSVC/Visual Studio versions below 16.4 has trouble with this
// function on Windows ARM64. As a work-around, we disable optimizations for this function.
// This work-around could/should be removed once the following versions of Visual Studio are no
// longer supported: All versions of VS2017, and versions of VS2019 below 16.4.
#अगर (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#आशय optimize( "", off )
#पूर्ण_अगर
U_CAPI int32_t U_EXPORT2
ubidi_writeReordered(UBiDi *pBiDi,
                     char16_t *dest, int32_t dest_आकार,
                     uint16_t options,
                     UErrorCode *pErrorCode) {
    स्थिर char16_t *text;
    char16_t *saveDest;
    int32_t length, destCapacity;
    int32_t run, run_गिनती, logicalStart, run_लंबाई;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    /* more error checking */
    अगर( pBiDi==nullptr ||
        (text=pBiDi->text)==nullptr || (length=pBiDi->length)<0 ||
        dest_आकार<0 || (dest_आकार>0 && dest==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* do input and output overlap? */
    अगर( dest!=nullptr &&
        ((text>=dest && text<dest+dest_आकार) ||
         (dest>=text && dest<text+pBiDi->original_लंबाई)))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    अगर(length==0) {
        /* nothing to do */
        वापस u_terminateUChars(dest, dest_आकार, 0, pErrorCode);
    }

    run_गिनती=ubidi_गिनती_Runs(pBiDi, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    /* destSize shrinks, later destination length=destCapacity-destSize */
    saveDest=dest;
    destCapacity=dest_आकार;
    run_लंबाई = 0;

    /*
     * Option "insert marks" implies UBIDI_INSERT_LRM_FOR_NUMERIC if the
     * reordering mode (checked below) is appropriate.
     */
    अगर(pBiDi->reorderingOptions & UBIDI_OPTION_जोड़ो_MARKS) {
        options|=UBIDI_जोड़ो_LRM_FOR_NUMERIC;
        options&=~UBIDI_REMOVE_BIDI_CONTROLS;
    }
    /*
     * Option "remove controls" implies UBIDI_REMOVE_BIDI_CONTROLS
     * and cancels UBIDI_INSERT_LRM_FOR_NUMERIC.
     */
    अगर(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        options|=UBIDI_REMOVE_BIDI_CONTROLS;
        options&=~UBIDI_जोड़ो_LRM_FOR_NUMERIC;
    }
    /*
     * If we do not perform the "inverse BiDi" algorithm, then we
     * don't need to insert any LRMs, and don't need to test for it.
     */
    अगर((pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_NUMBERS_AS_L) &&
       (pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_LIKE_DIRECT)  &&
       (pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL) &&
       (pBiDi->reorderingMode != UBIDI_REORDER_RUNS_ONLY)) {
        options&=~UBIDI_जोड़ो_LRM_FOR_NUMERIC;
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
    अगर(!(options&UBIDI_OUTPUT_REVERSE)) {
        /* forward output */
        अगर(!(options&UBIDI_जोड़ो_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            क्रम(run=0; run<run_गिनती; ++run) {
                अगर(UBIDI_LTR==ubidi_getVisualRun(pBiDi, run, &logicalStart, &run_लंबाई)) {
                    run_लंबाई=doWriteForward(text+logicalStart, run_लंबाई,
                                             dest, dest_आकार,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                } अन्यथा {
                    run_लंबाई=doWriteReverse(text+logicalStart, run_लंबाई,
                                             dest, dest_आकार,
                                             options, pErrorCode);
                }
                अगर(dest!=nullptr) {
                  dest+=run_लंबाई;
                }
                dest_आकार-=run_लंबाई;
            }
        } अन्यथा {
            /* insert BiDi controls for "inverse BiDi" */
            स्थिर DirProp *dirProps=pBiDi->dirProps;
            स्थिर char16_t *src;
            char16_t uc;
            UBiDiDirection dir;
            int32_t markFlag;

            क्रम(run=0; run<run_गिनती; ++run) {
                dir=ubidi_getVisualRun(pBiDi, run, &logicalStart, &run_लंबाई);
                src=text+logicalStart;
                /* check if something relevant in insertPoints */
                markFlag=pBiDi->runs[run].जोड़ो_Remove;
                अगर(markFlag<0) {        /* BiDi controls count */
                    markFlag=0;
                }

                अगर(UBIDI_LTR==dir) {
                    अगर((pBiDi->isInverse) &&
                       (/*run>0 &&*/ dirProps[logicalStart]!=L)) {
                        markFlag |= LRM_BEFORE;
                    }
                    अगर (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    अन्यथा अगर (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    अन्यथा  uc=0;
                    अगर(uc) {
                        अगर(dest_आकार>0) {
                            *dest++=uc;
                        }
                        --dest_आकार;
                    }

                    run_लंबाई=doWriteForward(src, run_लंबाई,
                                             dest, dest_आकार,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                    अगर(dest!=nullptr) {
                      dest+=run_लंबाई;
                    }
                    dest_आकार-=run_लंबाई;

                    अगर((pBiDi->isInverse) &&
                           (/*run<runCount-1 &&*/
                            run_लंबाई > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            dirProps[logicalStart+run_लंबाई-1]!=L)) {
                        markFlag |= LRM_AFTER;
                    }
                    अगर (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    अन्यथा अगर (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    अन्यथा  uc=0;
                    अगर(uc) {
                        अगर(dest_आकार>0) {
                            *dest++=uc;
                        }
                        --dest_आकार;
                    }
                } अन्यथा {                /* RTL run */
                    अगर((pBiDi->isInverse) &&
                       (/*run>0 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart+run_लंबाई-1])))) {
                        markFlag |= RLM_BEFORE;
                    }
                    अगर (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    अन्यथा अगर (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    अन्यथा  uc=0;
                    अगर(uc) {
                        अगर(dest_आकार>0) {
                            *dest++=uc;
                        }
                        --dest_आकार;
                    }

                    run_लंबाई=doWriteReverse(src, run_लंबाई,
                                             dest, dest_आकार,
                                             options, pErrorCode);
                    अगर(dest!=nullptr) {
                      dest+=run_लंबाई;
                    }
                    dest_आकार-=run_लंबाई;

                    अगर((pBiDi->isInverse) &&
                       (/*run<runCount-1 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart])))) {
                        markFlag |= RLM_AFTER;
                    }
                    अगर (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    अन्यथा अगर (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    अन्यथा  uc=0;
                    अगर(uc) {
                        अगर(dest_आकार>0) {
                            *dest++=uc;
                        }
                        --dest_आकार;
                    }
                }
            }
        }
    } अन्यथा {
        /* reverse output */
        अगर(!(options&UBIDI_जोड़ो_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            क्रम(run=run_गिनती; --run>=0;) {
                अगर(UBIDI_LTR==ubidi_getVisualRun(pBiDi, run, &logicalStart, &run_लंबाई)) {
                    run_लंबाई=doWriteReverse(text+logicalStart, run_लंबाई,
                                             dest, dest_आकार,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                } अन्यथा {
                    run_लंबाई=doWriteForward(text+logicalStart, run_लंबाई,
                                             dest, dest_आकार,
                                             options, pErrorCode);
                }
                अगर(dest!=nullptr) {
                  dest+=run_लंबाई;
                }
                dest_आकार-=run_लंबाई;
            }
        } अन्यथा {
            /* insert BiDi controls for "inverse BiDi" */
            स्थिर DirProp *dirProps=pBiDi->dirProps;
            स्थिर char16_t *src;
            UBiDiDirection dir;

            क्रम(run=run_गिनती; --run>=0;) {
                /* reverse output */
                dir=ubidi_getVisualRun(pBiDi, run, &logicalStart, &run_लंबाई);
                src=text+logicalStart;

                अगर(UBIDI_LTR==dir) {
                    अगर(/*run<runCount-1 &&*/ dirProps[logicalStart+run_लंबाई-1]!=L) {
                        अगर(dest_आकार>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_आकार;
                    }

                    run_लंबाई=doWriteReverse(src, run_लंबाई,
                                             dest, dest_आकार,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                    अगर(dest!=nullptr) {
                      dest+=run_लंबाई;
                    }
                    dest_आकार-=run_लंबाई;

                    अगर(/*run>0 &&*/ dirProps[logicalStart]!=L) {
                        अगर(dest_आकार>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_आकार;
                    }
                } अन्यथा {
                    अगर(/*run<runCount-1 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart]))) {
                        अगर(dest_आकार>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_आकार;
                    }

                    int32_t original_Run_लंबाई = run_लंबाई;
                    run_लंबाई=doWriteForward(src, run_लंबाई,
                                             dest, dest_आकार,
                                             options, pErrorCode);
                    अगर(dest!=nullptr) {
                      dest+=run_लंबाई;
                    }
                    dest_आकार-=run_लंबाई;

                    अगर(/*run>0 &&*/
                            run_लंबाई > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart+original_Run_लंबाई-1]))) {
                        अगर(dest_आकार>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_आकार;
                    }
                }
            }
        }
    }

    वापस u_terminateUChars(saveDest, destCapacity, destCapacity-dest_आकार, pErrorCode);
}
#अगर (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#आशय optimize( "", on )
#पूर्ण_अगर
