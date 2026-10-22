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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "unicode/ubidi.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "ubidiimp.h"

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
#যদি defined(UTF_আকার) && UTF_আকার==8
# error reimplement ubidi_writeReordered() জন্য UTF-8, see comment above
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
স্থির int32_t
doWriteForward(ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
               char16_t *dest, int32_t dest_আকার,
               uint16_t options,
               UErrorCode *pErrorCode) {
    যদি (src_দৈর্ঘ্য<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    /* optimize for several combinations of options */
    নির্বাচন(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING)) {
    ক্ষেত্র 0: {
        /* simply copy the LTR run to the destination */
        int32_t length=src_দৈর্ঘ্য;
        যদি(dest_আকার<length) {
            *pErrorCode=U_বাফার_OVERFLOW_ERROR;
            ফেরত src_দৈর্ঘ্য;
        }
        করো {
            *dest++=*src++;
        } যখন(--length>0);
        ফেরত src_দৈর্ঘ্য;
    }
    ক্ষেত্র UBIDI_DO_MIRRORING: {
        /* do mirroring */
        int32_t i=0, j=0;
        UChar32 c;
        করো {
            U16_পরবর্তী(src, i, src_দৈর্ঘ্য, c);
            c=u_charMirror(c);
            int32_t charLen = U16_দৈর্ঘ্য(c);
            যদি(j + charLen <= dest_আকার) {
                int32_t অস্থায়ী = j;
                U16_APPEND_UNSAFE(dest, অস্থায়ী, c);
            }
            j += charLen;
        } যখন(i<src_দৈর্ঘ্য);
        যদি(dest_আকার < j) {
            *pErrorCode = U_বাফার_OVERFLOW_ERROR;
        }
        যদি(j > src_দৈর্ঘ্য) {
            ফেরত j;
        }
        /* If mirroring caused code unit contraction (e.g., U+1DB10 contracting to U+221D),
         * pad trailing array slots up to srcLength to avoid leaving uninitialized memory.
         * If j > 0, repeat the preceding code unit; if j == 0, fallback to 0x0020 (ASCII space).
         */
        যখন(j < src_দৈর্ঘ্য && j < dest_আকার) {
            dest[j] = j > 0 ? dest[j - 1] : 0x0020;
            j++;
        }
        ফেরত src_দৈর্ঘ্য;
    }
    ক্ষেত্র UBIDI_REMOVE_BIDI_CONTROLS: {
        /* copy the LTR run and remove any BiDi control characters */
        int32_t remaining=dest_আকার;
        char16_t c;
        করো {
            c=*src++;
            যদি(!IS_BIDI_CONTROL_CHAR(c)) {
                যদি(--remaining<0) {
                    *pErrorCode=U_বাফার_OVERFLOW_ERROR;

                    /* preflight the length */
                    যখন(--src_দৈর্ঘ্য>0) {
                        c=*src++;
                        যদি(!IS_BIDI_CONTROL_CHAR(c)) {
                            --remaining;
                        }
                    }
                    ফেরত dest_আকার-remaining;
                }
                *dest++=c;
            }
        } যখন(--src_দৈর্ঘ্য>0);
        ফেরত dest_আকার-remaining;
    }
    পূর্বনির্ধারিত: {
        /* remove BiDi control characters and do mirroring */
        int32_t i, j=0;
        UChar32 c;
        করো {
            i=0;
            U16_পরবর্তী(src, i, src_দৈর্ঘ্য, c);
            src+=i;
            src_দৈর্ঘ্য-=i;
            যদি(!IS_BIDI_CONTROL_CHAR(c)) {
                c=u_charMirror(c);
                int32_t charLen = U16_দৈর্ঘ্য(c);
                যদি(j + charLen <= dest_আকার) {
                    int32_t অস্থায়ী = j;
                    U16_APPEND_UNSAFE(dest, অস্থায়ী, c);
                }
                j += charLen;
            }
        } যখন(src_দৈর্ঘ্য>0);
        যদি(dest_আকার < j) {
            *pErrorCode = U_বাফার_OVERFLOW_ERROR;
        }
        ফেরত j;
    }
    } /* end of switch */
}

স্থির int32_t
doWriteReverse(ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
               char16_t *dest, int32_t dest_আকার,
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

    যদি(src_দৈর্ঘ্য<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* optimize for several combinations of options */
    নির্বাচন(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING|UBIDI_KEEP_BASE_COMBINING)) {
    ক্ষেত্র 0:
        /*
         * With none of the "complicated" options set, the destination
         * run will have the same length as the source run,
         * and there is no mirroring and no keeping combining characters
         * with their base characters.
         */
        যদি(dest_আকার<src_দৈর্ঘ্য) {
            *pErrorCode=U_বাফার_OVERFLOW_ERROR;
            ফেরত src_দৈর্ঘ্য;
        }
        dest_আকার=src_দৈর্ঘ্য;

        /* preserve character integrity */
        করো {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_দৈর্ঘ্য;

            /* collect code units for one base character */
            U16_পূর্ব(src, 0, src_দৈর্ঘ্য, c);

            /* copy this base character */
            j=src_দৈর্ঘ্য;
            করো {
                *dest++=src[j++];
            } যখন(j<i);
        } যখন(src_দৈর্ঘ্য>0);
        বিরতি;
    ক্ষেত্র UBIDI_KEEP_BASE_COMBINING:
        /*
         * Here, too, the destination
         * run will have the same length as the source run,
         * and there is no mirroring.
         * We do need to keep combining characters with their base characters.
         */
        যদি(dest_আকার<src_দৈর্ঘ্য) {
            *pErrorCode=U_বাফার_OVERFLOW_ERROR;
            ফেরত src_দৈর্ঘ্য;
        }
        dest_আকার=src_দৈর্ঘ্য;

        /* preserve character integrity */
        করো {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_দৈর্ঘ্য;

            /* collect code units and modifier letters for one base character */
            করো {
                U16_পূর্ব(src, 0, src_দৈর্ঘ্য, c);
            } যখন(src_দৈর্ঘ্য>0 && IS_COMBINING(u_charType(c)));

            /* copy this "user character" */
            j=src_দৈর্ঘ্য;
            করো {
                *dest++=src[j++];
            } যখন(j<i);
        } যখন(src_দৈর্ঘ্য>0);
        বিরতি;
    পূর্বনির্ধারিত: {
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
        যদি(options&UBIDI_INTERNAL_RUNS_ONLY) {
            যদি(!(options&UBIDI_REMOVE_BIDI_CONTROLS)) {
                i=src_দৈর্ঘ্য;
            } অন্যথা {
                int32_t length=src_দৈর্ঘ্য;
                char16_t ch;
                i=0;
                /* Loop over all code units in the source run to count those that are not Bidi control characters,
                 * determining the required destination buffer length for preflighting when UBIDI_REMOVE_BIDI_CONTROLS is set.
                 */
                করো {
                    ch=*src++;
                    যদি(!IS_BIDI_CONTROL_CHAR(ch)) {
                        ++i;
                    }
                } যখন(--length>0);
                src-=src_দৈর্ঘ্য;
            }
            যদি(dest_আকার<i) {
                *pErrorCode=U_বাফার_OVERFLOW_ERROR;
                ফেরত i;
            }
            char16_t *saveDest=dest;
            int32_t dest_দৈর্ঘ্য=0;
            /* Loop backward across the source run segment by segment, using legacy 1-to-1 code unit stepping
             * to extract and reverse character clusters for internal setPara classification.
             */
            করো {
                i=src_দৈর্ঘ্য;
                U16_পূর্ব(src, 0, src_দৈর্ঘ্য, c);
                যদি(options&UBIDI_KEEP_BASE_COMBINING) {
                    /* Loop backward over any combining marks attached to the base character so that the combining
                     * sequence remains attached after its base character when reversed.
                     */
                    যখন(src_দৈর্ঘ্য>0 && IS_COMBINING(u_charType(c))) {
                        U16_পূর্ব(src, 0, src_দৈর্ঘ্য, c);
                    }
                }
                যদি(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                    চালু;
                }
                int32_t origBaseLen=U16_দৈর্ঘ্য(c);
                j=src_দৈর্ঘ্য;
                যদি(options&UBIDI_DO_MIRRORING) {
                    c=u_charMirror(c);
                    int32_t k=U16_দৈর্ঘ্য(c);
                    যদি(dest_দৈর্ঘ্য+k<=dest_আকার) {
                        int32_t অস্থায়ী=dest_দৈর্ঘ্য;
                        U16_APPEND_UNSAFE(saveDest, অস্থায়ী, c);
                    }
                    dest_দৈর্ঘ্য+=k;
                    j+=origBaseLen;
                }
                /* Loop forward from the current segment index to copy any unmirrored code units (such as combining marks
                 * or surrogate halves) into dest in forward visual order.
                 */
                যখন(j<i) {
                    যদি(dest_দৈর্ঘ্য<dest_আকার) {
                        saveDest[dest_দৈর্ঘ্য]=src[j];
                    }
                    dest_দৈর্ঘ্য++;
                    j++;
                }
            } যখন(src_দৈর্ঘ্য>0);
            যদি(dest_আকার<dest_দৈর্ঘ্য) {
                *pErrorCode=U_বাফার_OVERFLOW_ERROR;
            }
            ফেরত dest_দৈর্ঘ্য;
        }
        int32_t dest_দৈর্ঘ্য=0;
        করো {
            i=src_দৈর্ঘ্য;
            U16_পূর্ব(src, 0, src_দৈর্ঘ্য, c);
            যদি(options&UBIDI_KEEP_BASE_COMBINING) {
                যখন(src_দৈর্ঘ্য>0 && IS_COMBINING(u_charType(c))) {
                    U16_পূর্ব(src, 0, src_দৈর্ঘ্য, c);
                }
            }
            যদি(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                চালু;
            }

            int32_t origBaseLen=U16_দৈর্ঘ্য(c);
            j=src_দৈর্ঘ্য;
            যদি(options&UBIDI_DO_MIRRORING) {
                /* mirror only the base character */
                c=u_charMirror(c);
                int32_t k=U16_দৈর্ঘ্য(c);
                যদি(dest_দৈর্ঘ্য+k<=dest_আকার) {
                    int32_t অস্থায়ী=dest_দৈর্ঘ্য;
                    U16_APPEND_UNSAFE(dest, অস্থায়ী, c);
                }
                dest_দৈর্ঘ্য+=k;
                j+=origBaseLen;
            }
            যখন(j<i) {
                যদি(dest_দৈর্ঘ্য<dest_আকার) {
                    dest[dest_দৈর্ঘ্য]=src[j];
                }
                dest_দৈর্ঘ্য++;
                j++;
            }
        } যখন(src_দৈর্ঘ্য>0);


        যদি(dest_আকার<dest_দৈর্ঘ্য) {
            *pErrorCode=U_বাফার_OVERFLOW_ERROR;
        }
        ফেরত dest_দৈর্ঘ্য;
    }
    } /* end of switch */
    ফেরত dest_আকার;
}

U_CAPI int32_t U_EXPORT2
ubidi_writeReverse(ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                   char16_t *dest, int32_t dest_আকার,
                   uint16_t options,
                   UErrorCode *pErrorCode) {
    int32_t dest_দৈর্ঘ্য;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    /* more error checking */
    যদি( src==nullptr || src_দৈর্ঘ্য<-1 ||
        dest_আকার<0 || (dest_আকার>0 && dest==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* do input and output overlap? */
    যদি( dest!=nullptr &&
        ((src>=dest && src<dest+dest_আকার) ||
         (dest>=src && dest<src+src_দৈর্ঘ্য)))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    যদি(src_দৈর্ঘ্য==-1) {
        src_দৈর্ঘ্য=u_strlen(src);
    }
    যদি(src_দৈর্ঘ্য>0) {
        dest_দৈর্ঘ্য=doWriteReverse(src, src_দৈর্ঘ্য, dest, dest_আকার, options, pErrorCode);
    } অন্যথা {
        /* nothing to do */
        dest_দৈর্ঘ্য=0;
    }

    ফেরত u_terminateUChars(dest, dest_আকার, dest_দৈর্ঘ্য, pErrorCode);
}

// Ticket 20907 - The optimizer in MSVC/Visual Studio versions below 16.4 has trouble with this
// function on Windows ARM64. As a work-around, we disable optimizations for this function.
// This work-around could/should be removed once the following versions of Visual Studio are no
// longer supported: All versions of VS2017, and versions of VS2019 below 16.4.
#যদি (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", off )
#endif
U_CAPI int32_t U_EXPORT2
ubidi_writeReordered(UBiDi *pBiDi,
                     char16_t *dest, int32_t dest_আকার,
                     uint16_t options,
                     UErrorCode *pErrorCode) {
    ধ্রুবক char16_t *text;
    char16_t *saveDest;
    int32_t length, destCapacity;
    int32_t run, run_গুনতি, logicalStart, run_দৈর্ঘ্য;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    /* more error checking */
    যদি( pBiDi==nullptr ||
        (text=pBiDi->text)==nullptr || (length=pBiDi->length)<0 ||
        dest_আকার<0 || (dest_আকার>0 && dest==nullptr))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* do input and output overlap? */
    যদি( dest!=nullptr &&
        ((text>=dest && text<dest+dest_আকার) ||
         (dest>=text && dest<text+pBiDi->original_দৈর্ঘ্য)))
    {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    যদি(length==0) {
        /* nothing to do */
        ফেরত u_terminateUChars(dest, dest_আকার, 0, pErrorCode);
    }

    run_গুনতি=ubidi_গুনতি_Runs(pBiDi, pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    /* destSize shrinks, later destination length=destCapacity-destSize */
    saveDest=dest;
    destCapacity=dest_আকার;
    run_দৈর্ঘ্য = 0;

    /*
     * Option "insert marks" implies UBIDI_INSERT_LRM_FOR_NUMERIC if the
     * reordering mode (checked below) is appropriate.
     */
    যদি(pBiDi->reorderingOptions & UBIDI_OPTION_ঢোকাও_MARKS) {
        options|=UBIDI_ঢোকাও_LRM_FOR_NUMERIC;
        options&=~UBIDI_REMOVE_BIDI_CONTROLS;
    }
    /*
     * Option "remove controls" implies UBIDI_REMOVE_BIDI_CONTROLS
     * and cancels UBIDI_INSERT_LRM_FOR_NUMERIC.
     */
    যদি(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        options|=UBIDI_REMOVE_BIDI_CONTROLS;
        options&=~UBIDI_ঢোকাও_LRM_FOR_NUMERIC;
    }
    /*
     * If we do not perform the "inverse BiDi" algorithm, then we
     * don't need to insert any LRMs, and don't need to test for it.
     */
    যদি((pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_NUMBERS_AS_L) &&
       (pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_LIKE_DIRECT)  &&
       (pBiDi->reorderingMode != UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL) &&
       (pBiDi->reorderingMode != UBIDI_REORDER_RUNS_ONLY)) {
        options&=~UBIDI_ঢোকাও_LRM_FOR_NUMERIC;
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
    যদি(!(options&UBIDI_OUTPUT_REVERSE)) {
        /* forward output */
        যদি(!(options&UBIDI_ঢোকাও_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            জন্য(run=0; run<run_গুনতি; ++run) {
                যদি(UBIDI_LTR==ubidi_getVisualRun(pBiDi, run, &logicalStart, &run_দৈর্ঘ্য)) {
                    run_দৈর্ঘ্য=doWriteForward(text+logicalStart, run_দৈর্ঘ্য,
                                             dest, dest_আকার,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                } অন্যথা {
                    run_দৈর্ঘ্য=doWriteReverse(text+logicalStart, run_দৈর্ঘ্য,
                                             dest, dest_আকার,
                                             options, pErrorCode);
                }
                যদি(dest!=nullptr) {
                  dest+=run_দৈর্ঘ্য;
                }
                dest_আকার-=run_দৈর্ঘ্য;
            }
        } অন্যথা {
            /* insert BiDi controls for "inverse BiDi" */
            ধ্রুবক DirProp *dirProps=pBiDi->dirProps;
            ধ্রুবক char16_t *src;
            char16_t uc;
            UBiDiDirection dir;
            int32_t markFlag;

            জন্য(run=0; run<run_গুনতি; ++run) {
                dir=ubidi_getVisualRun(pBiDi, run, &logicalStart, &run_দৈর্ঘ্য);
                src=text+logicalStart;
                /* check if something relevant in insertPoints */
                markFlag=pBiDi->runs[run].ঢোকাও_Remove;
                যদি(markFlag<0) {        /* BiDi controls count */
                    markFlag=0;
                }

                যদি(UBIDI_LTR==dir) {
                    যদি((pBiDi->isInverse) &&
                       (/*run>0 &&*/ dirProps[logicalStart]!=L)) {
                        markFlag |= LRM_BEFORE;
                    }
                    যদি (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    অন্যথা যদি (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    অন্যথা  uc=0;
                    যদি(uc) {
                        যদি(dest_আকার>0) {
                            *dest++=uc;
                        }
                        --dest_আকার;
                    }

                    run_দৈর্ঘ্য=doWriteForward(src, run_দৈর্ঘ্য,
                                             dest, dest_আকার,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                    যদি(dest!=nullptr) {
                      dest+=run_দৈর্ঘ্য;
                    }
                    dest_আকার-=run_দৈর্ঘ্য;

                    যদি((pBiDi->isInverse) &&
                           (/*run<runCount-1 &&*/
                            run_দৈর্ঘ্য > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            dirProps[logicalStart+run_দৈর্ঘ্য-1]!=L)) {
                        markFlag |= LRM_AFTER;
                    }
                    যদি (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    অন্যথা যদি (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    অন্যথা  uc=0;
                    যদি(uc) {
                        যদি(dest_আকার>0) {
                            *dest++=uc;
                        }
                        --dest_আকার;
                    }
                } অন্যথা {                /* RTL run */
                    যদি((pBiDi->isInverse) &&
                       (/*run>0 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart+run_দৈর্ঘ্য-1])))) {
                        markFlag |= RLM_BEFORE;
                    }
                    যদি (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    অন্যথা যদি (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    অন্যথা  uc=0;
                    যদি(uc) {
                        যদি(dest_আকার>0) {
                            *dest++=uc;
                        }
                        --dest_আকার;
                    }

                    run_দৈর্ঘ্য=doWriteReverse(src, run_দৈর্ঘ্য,
                                             dest, dest_আকার,
                                             options, pErrorCode);
                    যদি(dest!=nullptr) {
                      dest+=run_দৈর্ঘ্য;
                    }
                    dest_আকার-=run_দৈর্ঘ্য;

                    যদি((pBiDi->isInverse) &&
                       (/*run<runCount-1 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart])))) {
                        markFlag |= RLM_AFTER;
                    }
                    যদি (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    অন্যথা যদি (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    অন্যথা  uc=0;
                    যদি(uc) {
                        যদি(dest_আকার>0) {
                            *dest++=uc;
                        }
                        --dest_আকার;
                    }
                }
            }
        }
    } অন্যথা {
        /* reverse output */
        যদি(!(options&UBIDI_ঢোকাও_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            জন্য(run=run_গুনতি; --run>=0;) {
                যদি(UBIDI_LTR==ubidi_getVisualRun(pBiDi, run, &logicalStart, &run_দৈর্ঘ্য)) {
                    run_দৈর্ঘ্য=doWriteReverse(text+logicalStart, run_দৈর্ঘ্য,
                                             dest, dest_আকার,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                } অন্যথা {
                    run_দৈর্ঘ্য=doWriteForward(text+logicalStart, run_দৈর্ঘ্য,
                                             dest, dest_আকার,
                                             options, pErrorCode);
                }
                যদি(dest!=nullptr) {
                  dest+=run_দৈর্ঘ্য;
                }
                dest_আকার-=run_দৈর্ঘ্য;
            }
        } অন্যথা {
            /* insert BiDi controls for "inverse BiDi" */
            ধ্রুবক DirProp *dirProps=pBiDi->dirProps;
            ধ্রুবক char16_t *src;
            UBiDiDirection dir;

            জন্য(run=run_গুনতি; --run>=0;) {
                /* reverse output */
                dir=ubidi_getVisualRun(pBiDi, run, &logicalStart, &run_দৈর্ঘ্য);
                src=text+logicalStart;

                যদি(UBIDI_LTR==dir) {
                    যদি(/*run<runCount-1 &&*/ dirProps[logicalStart+run_দৈর্ঘ্য-1]!=L) {
                        যদি(dest_আকার>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_আকার;
                    }

                    run_দৈর্ঘ্য=doWriteReverse(src, run_দৈর্ঘ্য,
                                             dest, dest_আকার,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), pErrorCode);
                    যদি(dest!=nullptr) {
                      dest+=run_দৈর্ঘ্য;
                    }
                    dest_আকার-=run_দৈর্ঘ্য;

                    যদি(/*run>0 &&*/ dirProps[logicalStart]!=L) {
                        যদি(dest_আকার>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_আকার;
                    }
                } অন্যথা {
                    যদি(/*run<runCount-1 &&*/ !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart]))) {
                        যদি(dest_আকার>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_আকার;
                    }

                    int32_t original_Run_দৈর্ঘ্য = run_দৈর্ঘ্য;
                    run_দৈর্ঘ্য=doWriteForward(src, run_দৈর্ঘ্য,
                                             dest, dest_আকার,
                                             options, pErrorCode);
                    যদি(dest!=nullptr) {
                      dest+=run_দৈর্ঘ্য;
                    }
                    dest_আকার-=run_দৈর্ঘ্য;

                    যদি(/*run>0 &&*/
                            run_দৈর্ঘ্য > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            !(MASK_R_AL&DIRPROP_FLAG(dirProps[logicalStart+original_Run_দৈর্ঘ্য-1]))) {
                        যদি(dest_আকার>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_আকার;
                    }
                }
            }
        }
    }

    ফেরত u_terminateUChars(saveDest, destCapacity, destCapacity-dest_আকার, pErrorCode);
}
#যদি (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#pragma optimize( "", on )
#endif
