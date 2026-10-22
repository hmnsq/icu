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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/ubidi.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "ubidiimp.h"

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
#ఒకవేళ defined(UTF_కొలత) && UTF_కొలత==8
# error reimplement ubidi_writeReordered() కొరకు UTF-8, see comment above
#అయితే_ముగింపు

#నిర్వచించు IS_COMBINING(type) ((1UL<<(type))&(1UL<<U_NON_SPACING_MARK|1UL<<U_COMBINING_SPACING_MARK|1UL<<U_ENCLOSING_MARK))

/*
 * When we have UBIDI_OUTPUT_REVERSE set on ubidi_writeReordered(), then we
 * semantically write RTL runs in reverse and later reverse them again.
 * Instead, we actually write them in forward order to begin with.
 * However, if the RTL run was to be mirrored, we need to mirror here now
 * since the implicit second reversal must not do it.
 * It looks strange to do mirroring in LTR output, but it is only because
 * we are writing RTL output in reverse.
 */
స్థిర int32_t
doWriteForward(స్థిరరాశి char16_t *src, int32_t src_నిడివి,
               char16_t *dest, int32_t dest_కొలత,
               uint16_t options,
               UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (src_నిడివి<=0) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    /* optimize for several combinations of options */
    ఎంపిక(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING)) {
    స్థితి 0: {
        /* simply copy the LTR run to the destination */
        int32_t length=src_నిడివి;
        ఒకవేళ(dest_కొలత<length) {
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి src_నిడివి;
        }
        చేయి {
            *dest++=*src++;
        } అయితే(--length>0);
        తిరిగి src_నిడివి;
    }
    స్థితి UBIDI_DO_MIRRORING: {
        /* do mirroring */
        int32_t i=0, j=0;
        UChar32 c;
        చేయి {
            U16_తదుపరి(src, i, src_నిడివి, c);
            c=u_charMirror(c);
            int32_t charLen = U16_నిడివి(c);
            ఒకవేళ(j + charLen <= dest_కొలత) {
                int32_t తాత్కాలిక = j;
                U16_APPEND_UNSAFE(dest, తాత్కాలిక, c);
            }
            j += charLen;
        } అయితే(i<src_నిడివి);
        ఒకవేళ(dest_కొలత < j) {
            *p_Error_సంకేతం = U_బఫరు_అధికప్రవాహం_ERROR;
        }
        ఒకవేళ(j > src_నిడివి) {
            తిరిగి j;
        }
        /* If mirroring caused code unit contraction (e.g., U+1DB10 contracting to U+221D),
         * pad trailing array slots up to srcLength to avoid leaving uninitialized memory.
         * If j > 0, repeat the preceding code unit; if j == 0, fallback to 0x0020 (ASCII space).
         */
        అయితే(j < src_నిడివి && j < dest_కొలత) {
            dest[j] = j > 0 ? dest[j - 1] : 0x0020;
            j++;
        }
        తిరిగి src_నిడివి;
    }
    స్థితి UBIDI_REMOVE_BIDI_CONTROLS: {
        /* copy the LTR run and remove any BiDi control characters */
        int32_t మిగిలిన=dest_కొలత;
        char16_t c;
        చేయి {
            c=*src++;
            ఒకవేళ(!IS_BIDI_CONTROL_CHAR(c)) {
                ఒకవేళ(--మిగిలిన<0) {
                    *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;

                    /* preflight the length */
                    అయితే(--src_నిడివి>0) {
                        c=*src++;
                        ఒకవేళ(!IS_BIDI_CONTROL_CHAR(c)) {
                            --మిగిలిన;
                        }
                    }
                    తిరిగి dest_కొలత-మిగిలిన;
                }
                *dest++=c;
            }
        } అయితే(--src_నిడివి>0);
        తిరిగి dest_కొలత-మిగిలిన;
    }
    అప్రమేయ: {
        /* remove BiDi control characters and do mirroring */
        int32_t i, j=0;
        UChar32 c;
        చేయి {
            i=0;
            U16_తదుపరి(src, i, src_నిడివి, c);
            src+=i;
            src_నిడివి-=i;
            ఒకవేళ(!IS_BIDI_CONTROL_CHAR(c)) {
                c=u_charMirror(c);
                int32_t charLen = U16_నిడివి(c);
                ఒకవేళ(j + charLen <= dest_కొలత) {
                    int32_t తాత్కాలిక = j;
                    U16_APPEND_UNSAFE(dest, తాత్కాలిక, c);
                }
                j += charLen;
            }
        } అయితే(src_నిడివి>0);
        ఒకవేళ(dest_కొలత < j) {
            *p_Error_సంకేతం = U_బఫరు_అధికప్రవాహం_ERROR;
        }
        తిరిగి j;
    }
    } /* end of switch */
}

స్థిర int32_t
do_Write_తిరగేయి(స్థిరరాశి char16_t *src, int32_t src_నిడివి,
               char16_t *dest, int32_t dest_కొలత,
               uint16_t options,
               UError_సంకేతం *p_Error_సంకేతం) {
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

    ఒకవేళ(src_నిడివి<=0) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* optimize for several combinations of options */
    ఎంపిక(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING|UBIDI_KEEP_పునాది_COMBINING)) {
    స్థితి 0:
        /*
         * With none of the "complicated" options set, the destination
         * run will have the same length as the source run,
         * and there is no mirroring and no keeping combining characters
         * with their base characters.
         */
        ఒకవేళ(dest_కొలత<src_నిడివి) {
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి src_నిడివి;
        }
        dest_కొలత=src_నిడివి;

        /* preserve character integrity */
        చేయి {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_నిడివి;

            /* collect code units for one base character */
            U16_PREV(src, 0, src_నిడివి, c);

            /* copy this base character */
            j=src_నిడివి;
            చేయి {
                *dest++=src[j++];
            } అయితే(j<i);
        } అయితే(src_నిడివి>0);
        ఆపు;
    స్థితి UBIDI_KEEP_పునాది_COMBINING:
        /*
         * Here, too, the destination
         * run will have the same length as the source run,
         * and there is no mirroring.
         * We do need to keep combining characters with their base characters.
         */
        ఒకవేళ(dest_కొలత<src_నిడివి) {
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి src_నిడివి;
        }
        dest_కొలత=src_నిడివి;

        /* preserve character integrity */
        చేయి {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_నిడివి;

            /* collect code units and modifier letters for one base character */
            చేయి {
                U16_PREV(src, 0, src_నిడివి, c);
            } అయితే(src_నిడివి>0 && IS_COMBINING(u_charType(c)));

            /* copy this "user character" */
            j=src_నిడివి;
            చేయి {
                *dest++=src[j++];
            } అయితే(j<i);
        } అయితే(src_నిడివి>0);
        ఆపు;
    అప్రమేయ: {
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
        ఒకవేళ(options&UBIDI_అంతర్గత_RUNS_ONLY) {
            ఒకవేళ(!(options&UBIDI_REMOVE_BIDI_CONTROLS)) {
                i=src_నిడివి;
            } లేకపోతే {
                int32_t length=src_నిడివి;
                char16_t ch;
                i=0;
                /* Loop over all code units in the source run to count those that are not Bidi control characters,
                 * determining the required destination buffer length for preflighting when UBIDI_REMOVE_BIDI_CONTROLS is set.
                 */
                చేయి {
                    ch=*src++;
                    ఒకవేళ(!IS_BIDI_CONTROL_CHAR(ch)) {
                        ++i;
                    }
                } అయితే(--length>0);
                src-=src_నిడివి;
            }
            ఒకవేళ(dest_కొలత<i) {
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                తిరిగి i;
            }
            char16_t *saveDest=dest;
            int32_t dest_నిడివి=0;
            /* Loop backward across the source run segment by segment, using legacy 1-to-1 code unit stepping
             * to extract and reverse character clusters for internal setPara classification.
             */
            చేయి {
                i=src_నిడివి;
                U16_PREV(src, 0, src_నిడివి, c);
                ఒకవేళ(options&UBIDI_KEEP_పునాది_COMBINING) {
                    /* Loop backward over any combining marks attached to the base character so that the combining
                     * sequence remains attached after its base character when reversed.
                     */
                    అయితే(src_నిడివి>0 && IS_COMBINING(u_charType(c))) {
                        U16_PREV(src, 0, src_నిడివి, c);
                    }
                }
                ఒకవేళ(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                    కొనసాగు;
                }
                int32_t orig_పునాది_Len=U16_నిడివి(c);
                j=src_నిడివి;
                ఒకవేళ(options&UBIDI_DO_MIRRORING) {
                    c=u_charMirror(c);
                    int32_t k=U16_నిడివి(c);
                    ఒకవేళ(dest_నిడివి+k<=dest_కొలత) {
                        int32_t తాత్కాలిక=dest_నిడివి;
                        U16_APPEND_UNSAFE(saveDest, తాత్కాలిక, c);
                    }
                    dest_నిడివి+=k;
                    j+=orig_పునాది_Len;
                }
                /* Loop forward from the current segment index to copy any unmirrored code units (such as combining marks
                 * or surrogate halves) into dest in forward visual order.
                 */
                అయితే(j<i) {
                    ఒకవేళ(dest_నిడివి<dest_కొలత) {
                        saveDest[dest_నిడివి]=src[j];
                    }
                    dest_నిడివి++;
                    j++;
                }
            } అయితే(src_నిడివి>0);
            ఒకవేళ(dest_కొలత<dest_నిడివి) {
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
            }
            తిరిగి dest_నిడివి;
        }
        int32_t dest_నిడివి=0;
        చేయి {
            i=src_నిడివి;
            U16_PREV(src, 0, src_నిడివి, c);
            ఒకవేళ(options&UBIDI_KEEP_పునాది_COMBINING) {
                అయితే(src_నిడివి>0 && IS_COMBINING(u_charType(c))) {
                    U16_PREV(src, 0, src_నిడివి, c);
                }
            }
            ఒకవేళ(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                కొనసాగు;
            }

            int32_t orig_పునాది_Len=U16_నిడివి(c);
            j=src_నిడివి;
            ఒకవేళ(options&UBIDI_DO_MIRRORING) {
                /* mirror only the base character */
                c=u_charMirror(c);
                int32_t k=U16_నిడివి(c);
                ఒకవేళ(dest_నిడివి+k<=dest_కొలత) {
                    int32_t తాత్కాలిక=dest_నిడివి;
                    U16_APPEND_UNSAFE(dest, తాత్కాలిక, c);
                }
                dest_నిడివి+=k;
                j+=orig_పునాది_Len;
            }
            అయితే(j<i) {
                ఒకవేళ(dest_నిడివి<dest_కొలత) {
                    dest[dest_నిడివి]=src[j];
                }
                dest_నిడివి++;
                j++;
            }
        } అయితే(src_నిడివి>0);


        ఒకవేళ(dest_కొలత<dest_నిడివి) {
            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        }
        తిరిగి dest_నిడివి;
    }
    } /* end of switch */
    తిరిగి dest_కొలత;
}

U_CAPI int32_t U_EXPORT2
ubidi_write_తిరగేయి(స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                   char16_t *dest, int32_t dest_కొలత,
                   uint16_t options,
                   UError_సంకేతం *p_Error_సంకేతం) {
    int32_t dest_నిడివి;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* more error checking */
    ఒకవేళ( src==శూన్యసూచిక || src_నిడివి<-1 ||
        dest_కొలత<0 || (dest_కొలత>0 && dest==శూన్యసూచిక))
    {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* do input and output overlap? */
    ఒకవేళ( dest!=శూన్యసూచిక &&
        ((src>=dest && src<dest+dest_కొలత) ||
         (dest>=src && dest<src+src_నిడివి)))
    {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(src_నిడివి==-1) {
        src_నిడివి=u_strlen(src);
    }
    ఒకవేళ(src_నిడివి>0) {
        dest_నిడివి=do_Write_తిరగేయి(src, src_నిడివి, dest, dest_కొలత, options, p_Error_సంకేతం);
    } లేకపోతే {
        /* nothing to do */
        dest_నిడివి=0;
    }

    తిరిగి u_terminateUChars(dest, dest_కొలత, dest_నిడివి, p_Error_సంకేతం);
}

// Ticket 20907 - The optimizer in MSVC/Visual Studio versions below 16.4 has trouble with this
// function on Windows ARM64. As a work-around, we disable optimizations for this function.
// This work-around could/should be removed once the following versions of Visual Studio are no
// longer supported: All versions of VS2017, and versions of VS2019 below 16.4.
#ఒకవేళ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#సూచన optimize( "", off )
#అయితే_ముగింపు
U_CAPI int32_t U_EXPORT2
ubidi_writeReordered(UBiDi *pBiDi,
                     char16_t *dest, int32_t dest_కొలత,
                     uint16_t options,
                     UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి char16_t *పాఠ్యం;
    char16_t *saveDest;
    int32_t length, dest_సామర్థ్యం;
    int32_t నిరంతరం_2, నిరంతరం_లెక్క, logical_ప్రారంభం, నిరంతరం_నిడివి;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* more error checking */
    ఒకవేళ( pBiDi==శూన్యసూచిక ||
        (పాఠ్యం=pBiDi->పాఠ్యం)==శూన్యసూచిక || (length=pBiDi->length)<0 ||
        dest_కొలత<0 || (dest_కొలత>0 && dest==శూన్యసూచిక))
    {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* do input and output overlap? */
    ఒకవేళ( dest!=శూన్యసూచిక &&
        ((పాఠ్యం>=dest && పాఠ్యం<dest+dest_కొలత) ||
         (dest>=పాఠ్యం && dest<పాఠ్యం+pBiDi->original_నిడివి)))
    {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(length==0) {
        /* nothing to do */
        తిరిగి u_terminateUChars(dest, dest_కొలత, 0, p_Error_సంకేతం);
    }

    నిరంతరం_లెక్క=ubidi_లెక్క_Runs(pBiDi, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* destSize shrinks, later destination length=destCapacity-destSize */
    saveDest=dest;
    dest_సామర్థ్యం=dest_కొలత;
    నిరంతరం_నిడివి = 0;

    /*
     * Option "insert marks" implies UBIDI_INSERT_LRM_FOR_NUMERIC if the
     * reordering mode (checked below) is appropriate.
     */
    ఒకవేళ(pBiDi->reorderingOptions & UBIDI_OPTION_చొప్పించు_MARKS) {
        options|=UBIDI_చొప్పించు_LRM_FOR_NUMERIC;
        options&=~UBIDI_REMOVE_BIDI_CONTROLS;
    }
    /*
     * Option "remove controls" implies UBIDI_REMOVE_BIDI_CONTROLS
     * and cancels UBIDI_INSERT_LRM_FOR_NUMERIC.
     */
    ఒకవేళ(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        options|=UBIDI_REMOVE_BIDI_CONTROLS;
        options&=~UBIDI_చొప్పించు_LRM_FOR_NUMERIC;
    }
    /*
     * If we do not perform the "inverse BiDi" algorithm, then we
     * don't need to insert any LRMs, and don't need to test for it.
     */
    ఒకవేళ((pBiDi->reordering_బహుళకం != UBIDI_REORDER_INVERSE_NUMBERS_AS_L) &&
       (pBiDi->reordering_బహుళకం != UBIDI_REORDER_INVERSE_LIKE_ప్రత్యక్ష)  &&
       (pBiDi->reordering_బహుళకం != UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL) &&
       (pBiDi->reordering_బహుళకం != UBIDI_REORDER_RUNS_ONLY)) {
        options&=~UBIDI_చొప్పించు_LRM_FOR_NUMERIC;
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
    ఒకవేళ(!(options&UBIDI_OUTPUT_తిరగేయి)) {
        /* forward output */
        ఒకవేళ(!(options&UBIDI_చొప్పించు_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            కొరకు(నిరంతరం_2=0; నిరంతరం_2<నిరంతరం_లెక్క; ++నిరంతరం_2) {
                ఒకవేళ(UBIDI_LTR==ubidi_get_Visual_నిరంతరం(pBiDi, నిరంతరం_2, &logical_ప్రారంభం, &నిరంతరం_నిడివి)) {
                    నిరంతరం_నిడివి=doWriteForward(పాఠ్యం+logical_ప్రారంభం, నిరంతరం_నిడివి,
                                             dest, dest_కొలత,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_సంకేతం);
                } లేకపోతే {
                    నిరంతరం_నిడివి=do_Write_తిరగేయి(పాఠ్యం+logical_ప్రారంభం, నిరంతరం_నిడివి,
                                             dest, dest_కొలత,
                                             options, p_Error_సంకేతం);
                }
                ఒకవేళ(dest!=శూన్యసూచిక) {
                  dest+=నిరంతరం_నిడివి;
                }
                dest_కొలత-=నిరంతరం_నిడివి;
            }
        } లేకపోతే {
            /* insert BiDi controls for "inverse BiDi" */
            స్థిరరాశి DirProp *dirProps=pBiDi->dirProps;
            స్థిరరాశి char16_t *src;
            char16_t uc;
            UBiDiDirection dir;
            int32_t markFlag;

            కొరకు(నిరంతరం_2=0; నిరంతరం_2<నిరంతరం_లెక్క; ++నిరంతరం_2) {
                dir=ubidi_get_Visual_నిరంతరం(pBiDi, నిరంతరం_2, &logical_ప్రారంభం, &నిరంతరం_నిడివి);
                src=పాఠ్యం+logical_ప్రారంభం;
                /* check if something relevant in insertPoints */
                markFlag=pBiDi->runs[నిరంతరం_2].చొప్పించు_Remove;
                ఒకవేళ(markFlag<0) {        /* BiDi controls count */
                    markFlag=0;
                }

                ఒకవేళ(UBIDI_LTR==dir) {
                    ఒకవేళ((pBiDi->isInverse) &&
                       (/*run>0 &&*/ dirProps[logical_ప్రారంభం]!=L)) {
                        markFlag |= LRM_BEFORE;
                    }
                    ఒకవేళ (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    లేకపోతే ఒకవేళ (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    లేకపోతే  uc=0;
                    ఒకవేళ(uc) {
                        ఒకవేళ(dest_కొలత>0) {
                            *dest++=uc;
                        }
                        --dest_కొలత;
                    }

                    నిరంతరం_నిడివి=doWriteForward(src, నిరంతరం_నిడివి,
                                             dest, dest_కొలత,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_సంకేతం);
                    ఒకవేళ(dest!=శూన్యసూచిక) {
                      dest+=నిరంతరం_నిడివి;
                    }
                    dest_కొలత-=నిరంతరం_నిడివి;

                    ఒకవేళ((pBiDi->isInverse) &&
                           (/*run<runCount-1 &&*/
                            నిరంతరం_నిడివి > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            dirProps[logical_ప్రారంభం+నిరంతరం_నిడివి-1]!=L)) {
                        markFlag |= LRM_AFTER;
                    }
                    ఒకవేళ (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    లేకపోతే ఒకవేళ (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    లేకపోతే  uc=0;
                    ఒకవేళ(uc) {
                        ఒకవేళ(dest_కొలత>0) {
                            *dest++=uc;
                        }
                        --dest_కొలత;
                    }
                } లేకపోతే {                /* RTL run */
                    ఒకవేళ((pBiDi->isInverse) &&
                       (/*run>0 &&*/ !(ముసుగు_R_AL&DIRPROP_FLAG(dirProps[logical_ప్రారంభం+నిరంతరం_నిడివి-1])))) {
                        markFlag |= RLM_BEFORE;
                    }
                    ఒకవేళ (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    లేకపోతే ఒకవేళ (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    లేకపోతే  uc=0;
                    ఒకవేళ(uc) {
                        ఒకవేళ(dest_కొలత>0) {
                            *dest++=uc;
                        }
                        --dest_కొలత;
                    }

                    నిరంతరం_నిడివి=do_Write_తిరగేయి(src, నిరంతరం_నిడివి,
                                             dest, dest_కొలత,
                                             options, p_Error_సంకేతం);
                    ఒకవేళ(dest!=శూన్యసూచిక) {
                      dest+=నిరంతరం_నిడివి;
                    }
                    dest_కొలత-=నిరంతరం_నిడివి;

                    ఒకవేళ((pBiDi->isInverse) &&
                       (/*run<runCount-1 &&*/ !(ముసుగు_R_AL&DIRPROP_FLAG(dirProps[logical_ప్రారంభం])))) {
                        markFlag |= RLM_AFTER;
                    }
                    ఒకవేళ (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    లేకపోతే ఒకవేళ (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    లేకపోతే  uc=0;
                    ఒకవేళ(uc) {
                        ఒకవేళ(dest_కొలత>0) {
                            *dest++=uc;
                        }
                        --dest_కొలత;
                    }
                }
            }
        }
    } లేకపోతే {
        /* reverse output */
        ఒకవేళ(!(options&UBIDI_చొప్పించు_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            కొరకు(నిరంతరం_2=నిరంతరం_లెక్క; --నిరంతరం_2>=0;) {
                ఒకవేళ(UBIDI_LTR==ubidi_get_Visual_నిరంతరం(pBiDi, నిరంతరం_2, &logical_ప్రారంభం, &నిరంతరం_నిడివి)) {
                    నిరంతరం_నిడివి=do_Write_తిరగేయి(పాఠ్యం+logical_ప్రారంభం, నిరంతరం_నిడివి,
                                             dest, dest_కొలత,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_సంకేతం);
                } లేకపోతే {
                    నిరంతరం_నిడివి=doWriteForward(పాఠ్యం+logical_ప్రారంభం, నిరంతరం_నిడివి,
                                             dest, dest_కొలత,
                                             options, p_Error_సంకేతం);
                }
                ఒకవేళ(dest!=శూన్యసూచిక) {
                  dest+=నిరంతరం_నిడివి;
                }
                dest_కొలత-=నిరంతరం_నిడివి;
            }
        } లేకపోతే {
            /* insert BiDi controls for "inverse BiDi" */
            స్థిరరాశి DirProp *dirProps=pBiDi->dirProps;
            స్థిరరాశి char16_t *src;
            UBiDiDirection dir;

            కొరకు(నిరంతరం_2=నిరంతరం_లెక్క; --నిరంతరం_2>=0;) {
                /* reverse output */
                dir=ubidi_get_Visual_నిరంతరం(pBiDi, నిరంతరం_2, &logical_ప్రారంభం, &నిరంతరం_నిడివి);
                src=పాఠ్యం+logical_ప్రారంభం;

                ఒకవేళ(UBIDI_LTR==dir) {
                    ఒకవేళ(/*run<runCount-1 &&*/ dirProps[logical_ప్రారంభం+నిరంతరం_నిడివి-1]!=L) {
                        ఒకవేళ(dest_కొలత>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_కొలత;
                    }

                    నిరంతరం_నిడివి=do_Write_తిరగేయి(src, నిరంతరం_నిడివి,
                                             dest, dest_కొలత,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_సంకేతం);
                    ఒకవేళ(dest!=శూన్యసూచిక) {
                      dest+=నిరంతరం_నిడివి;
                    }
                    dest_కొలత-=నిరంతరం_నిడివి;

                    ఒకవేళ(/*run>0 &&*/ dirProps[logical_ప్రారంభం]!=L) {
                        ఒకవేళ(dest_కొలత>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_కొలత;
                    }
                } లేకపోతే {
                    ఒకవేళ(/*run<runCount-1 &&*/ !(ముసుగు_R_AL&DIRPROP_FLAG(dirProps[logical_ప్రారంభం]))) {
                        ఒకవేళ(dest_కొలత>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_కొలత;
                    }

                    int32_t original_నిరంతరం_నిడివి = నిరంతరం_నిడివి;
                    నిరంతరం_నిడివి=doWriteForward(src, నిరంతరం_నిడివి,
                                             dest, dest_కొలత,
                                             options, p_Error_సంకేతం);
                    ఒకవేళ(dest!=శూన్యసూచిక) {
                      dest+=నిరంతరం_నిడివి;
                    }
                    dest_కొలత-=నిరంతరం_నిడివి;

                    ఒకవేళ(/*run>0 &&*/
                            నిరంతరం_నిడివి > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            !(ముసుగు_R_AL&DIRPROP_FLAG(dirProps[logical_ప్రారంభం+original_నిరంతరం_నిడివి-1]))) {
                        ఒకవేళ(dest_కొలత>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_కొలత;
                    }
                }
            }
        }
    }

    తిరిగి u_terminateUChars(saveDest, dest_సామర్థ్యం, dest_సామర్థ్యం-dest_కొలత, p_Error_సంకేతం);
}
#ఒకవేళ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#సూచన optimize( "", on )
#అయితే_ముగింపు
