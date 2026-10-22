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
#यदि defined(UTF_आकार) && UTF_आकार==8
# error reimplement ubidi_writeReordered() प्रति UTF-8, see comment above
#यदि_अन्तः

#परिभाषय IS_COMBINING(type) ((1UL<<(type))&(1UL<<U_NON_SPACING_MARK|1UL<<U_COMBINING_SPACING_MARK|1UL<<U_ENCLOSING_MARK))

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
doWriteForward(ध्रुव char16_t *src, int32_t src_दीर्घता,
               char16_t *dest, int32_t dest_आकार,
               uint16_t options,
               UError_संहिता *p_Error_संहिता) {
    यदि (src_दीर्घता<=0) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    /* optimize for several combinations of options */
    विकल्प(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING)) {
    पक्ष 0: {
        /* simply copy the LTR run to the destination */
        int32_t length=src_दीर्घता;
        यदि(dest_आकार<length) {
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम src_दीर्घता;
        }
        कुरु {
            *dest++=*src++;
        } यावत्(--length>0);
        प्रत्यागम src_दीर्घता;
    }
    पक्ष UBIDI_DO_MIRRORING: {
        /* do mirroring */
        int32_t i=0, j=0;
        UChar32 c;
        कुरु {
            U16_अग्र(src, i, src_दीर्घता, c);
            c=u_charMirror(c);
            int32_t charLen = U16_दीर्घता(c);
            यदि(j + charLen <= dest_आकार) {
                int32_t अस्थायी = j;
                U16_APPEND_UNSAFE(dest, अस्थायी, c);
            }
            j += charLen;
        } यावत्(i<src_दीर्घता);
        यदि(dest_आकार < j) {
            *p_Error_संहिता = U_बफर_अतिप्रवाहः_ERROR;
        }
        यदि(j > src_दीर्घता) {
            प्रत्यागम j;
        }
        /* If mirroring caused code unit contraction (e.g., U+1DB10 contracting to U+221D),
         * pad trailing array slots up to srcLength to avoid leaving uninitialized memory.
         * If j > 0, repeat the preceding code unit; if j == 0, fallback to 0x0020 (ASCII space).
         */
        यावत्(j < src_दीर्घता && j < dest_आकार) {
            dest[j] = j > 0 ? dest[j - 1] : 0x0020;
            j++;
        }
        प्रत्यागम src_दीर्घता;
    }
    पक्ष UBIDI_REMOVE_BIDI_CONTROLS: {
        /* copy the LTR run and remove any BiDi control characters */
        int32_t remaining=dest_आकार;
        char16_t c;
        कुरु {
            c=*src++;
            यदि(!IS_BIDI_CONTROL_CHAR(c)) {
                यदि(--remaining<0) {
                    *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;

                    /* preflight the length */
                    यावत्(--src_दीर्घता>0) {
                        c=*src++;
                        यदि(!IS_BIDI_CONTROL_CHAR(c)) {
                            --remaining;
                        }
                    }
                    प्रत्यागम dest_आकार-remaining;
                }
                *dest++=c;
            }
        } यावत्(--src_दीर्घता>0);
        प्रत्यागम dest_आकार-remaining;
    }
    स्वभाव: {
        /* remove BiDi control characters and do mirroring */
        int32_t i, j=0;
        UChar32 c;
        कुरु {
            i=0;
            U16_अग्र(src, i, src_दीर्घता, c);
            src+=i;
            src_दीर्घता-=i;
            यदि(!IS_BIDI_CONTROL_CHAR(c)) {
                c=u_charMirror(c);
                int32_t charLen = U16_दीर्घता(c);
                यदि(j + charLen <= dest_आकार) {
                    int32_t अस्थायी = j;
                    U16_APPEND_UNSAFE(dest, अस्थायी, c);
                }
                j += charLen;
            }
        } यावत्(src_दीर्घता>0);
        यदि(dest_आकार < j) {
            *p_Error_संहिता = U_बफर_अतिप्रवाहः_ERROR;
        }
        प्रत्यागम j;
    }
    } /* end of switch */
}

स्थिर int32_t
doWriteReverse(ध्रुव char16_t *src, int32_t src_दीर्घता,
               char16_t *dest, int32_t dest_आकार,
               uint16_t options,
               UError_संहिता *p_Error_संहिता) {
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

    यदि(src_दीर्घता<=0) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* optimize for several combinations of options */
    विकल्प(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING|UBIDI_KEEP_आधारः_COMBINING)) {
    पक्ष 0:
        /*
         * With none of the "complicated" options set, the destination
         * run will have the same length as the source run,
         * and there is no mirroring and no keeping combining characters
         * with their base characters.
         */
        यदि(dest_आकार<src_दीर्घता) {
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम src_दीर्घता;
        }
        dest_आकार=src_दीर्घता;

        /* preserve character integrity */
        कुरु {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_दीर्घता;

            /* collect code units for one base character */
            U16_पूर्व(src, 0, src_दीर्घता, c);

            /* copy this base character */
            j=src_दीर्घता;
            कुरु {
                *dest++=src[j++];
            } यावत्(j<i);
        } यावत्(src_दीर्घता>0);
        भङ्ग;
    पक्ष UBIDI_KEEP_आधारः_COMBINING:
        /*
         * Here, too, the destination
         * run will have the same length as the source run,
         * and there is no mirroring.
         * We do need to keep combining characters with their base characters.
         */
        यदि(dest_आकार<src_दीर्घता) {
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम src_दीर्घता;
        }
        dest_आकार=src_दीर्घता;

        /* preserve character integrity */
        कुरु {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_दीर्घता;

            /* collect code units and modifier letters for one base character */
            कुरु {
                U16_पूर्व(src, 0, src_दीर्घता, c);
            } यावत्(src_दीर्घता>0 && IS_COMBINING(u_charType(c)));

            /* copy this "user character" */
            j=src_दीर्घता;
            कुरु {
                *dest++=src[j++];
            } यावत्(j<i);
        } यावत्(src_दीर्घता>0);
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
        यदि(options&UBIDI_आन्तरिकम्_RUNS_ONLY) {
            यदि(!(options&UBIDI_REMOVE_BIDI_CONTROLS)) {
                i=src_दीर्घता;
            } अन्यथा {
                int32_t length=src_दीर्घता;
                char16_t ch;
                i=0;
                /* Loop over all code units in the source run to count those that are not Bidi control characters,
                 * determining the required destination buffer length for preflighting when UBIDI_REMOVE_BIDI_CONTROLS is set.
                 */
                कुरु {
                    ch=*src++;
                    यदि(!IS_BIDI_CONTROL_CHAR(ch)) {
                        ++i;
                    }
                } यावत्(--length>0);
                src-=src_दीर्घता;
            }
            यदि(dest_आकार<i) {
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                प्रत्यागम i;
            }
            char16_t *saveDest=dest;
            int32_t dest_दीर्घता=0;
            /* Loop backward across the source run segment by segment, using legacy 1-to-1 code unit stepping
             * to extract and reverse character clusters for internal setPara classification.
             */
            कुरु {
                i=src_दीर्घता;
                U16_पूर्व(src, 0, src_दीर्घता, c);
                यदि(options&UBIDI_KEEP_आधारः_COMBINING) {
                    /* Loop backward over any combining marks attached to the base character so that the combining
                     * sequence remains attached after its base character when reversed.
                     */
                    यावत्(src_दीर्घता>0 && IS_COMBINING(u_charType(c))) {
                        U16_पूर्व(src, 0, src_दीर्घता, c);
                    }
                }
                यदि(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                    अनुवर्त;
                }
                int32_t orig_आधारः_Len=U16_दीर्घता(c);
                j=src_दीर्घता;
                यदि(options&UBIDI_DO_MIRRORING) {
                    c=u_charMirror(c);
                    int32_t k=U16_दीर्घता(c);
                    यदि(dest_दीर्घता+k<=dest_आकार) {
                        int32_t अस्थायी=dest_दीर्घता;
                        U16_APPEND_UNSAFE(saveDest, अस्थायी, c);
                    }
                    dest_दीर्घता+=k;
                    j+=orig_आधारः_Len;
                }
                /* Loop forward from the current segment index to copy any unmirrored code units (such as combining marks
                 * or surrogate halves) into dest in forward visual order.
                 */
                यावत्(j<i) {
                    यदि(dest_दीर्घता<dest_आकार) {
                        saveDest[dest_दीर्घता]=src[j];
                    }
                    dest_दीर्घता++;
                    j++;
                }
            } यावत्(src_दीर्घता>0);
            यदि(dest_आकार<dest_दीर्घता) {
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
            }
            प्रत्यागम dest_दीर्घता;
        }
        int32_t dest_दीर्घता=0;
        कुरु {
            i=src_दीर्घता;
            U16_पूर्व(src, 0, src_दीर्घता, c);
            यदि(options&UBIDI_KEEP_आधारः_COMBINING) {
                यावत्(src_दीर्घता>0 && IS_COMBINING(u_charType(c))) {
                    U16_पूर्व(src, 0, src_दीर्घता, c);
                }
            }
            यदि(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                अनुवर्त;
            }

            int32_t orig_आधारः_Len=U16_दीर्घता(c);
            j=src_दीर्घता;
            यदि(options&UBIDI_DO_MIRRORING) {
                /* mirror only the base character */
                c=u_charMirror(c);
                int32_t k=U16_दीर्घता(c);
                यदि(dest_दीर्घता+k<=dest_आकार) {
                    int32_t अस्थायी=dest_दीर्घता;
                    U16_APPEND_UNSAFE(dest, अस्थायी, c);
                }
                dest_दीर्घता+=k;
                j+=orig_आधारः_Len;
            }
            यावत्(j<i) {
                यदि(dest_दीर्घता<dest_आकार) {
                    dest[dest_दीर्घता]=src[j];
                }
                dest_दीर्घता++;
                j++;
            }
        } यावत्(src_दीर्घता>0);


        यदि(dest_आकार<dest_दीर्घता) {
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        }
        प्रत्यागम dest_दीर्घता;
    }
    } /* end of switch */
    प्रत्यागम dest_आकार;
}

U_CAPI int32_t U_EXPORT2
ubidi_writeReverse(ध्रुव char16_t *src, int32_t src_दीर्घता,
                   char16_t *dest, int32_t dest_आकार,
                   uint16_t options,
                   UError_संहिता *p_Error_संहिता) {
    int32_t dest_दीर्घता;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* more error checking */
    यदि( src==शून्यसूचकः || src_दीर्घता<-1 ||
        dest_आकार<0 || (dest_आकार>0 && dest==शून्यसूचकः))
    {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* do input and output overlap? */
    यदि( dest!=शून्यसूचकः &&
        ((src>=dest && src<dest+dest_आकार) ||
         (dest>=src && dest<src+src_दीर्घता)))
    {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    यदि(src_दीर्घता==-1) {
        src_दीर्घता=u_strlen(src);
    }
    यदि(src_दीर्घता>0) {
        dest_दीर्घता=doWriteReverse(src, src_दीर्घता, dest, dest_आकार, options, p_Error_संहिता);
    } अन्यथा {
        /* nothing to do */
        dest_दीर्घता=0;
    }

    प्रत्यागम u_terminateUChars(dest, dest_आकार, dest_दीर्घता, p_Error_संहिता);
}

// Ticket 20907 - The optimizer in MSVC/Visual Studio versions below 16.4 has trouble with this
// function on Windows ARM64. As a work-around, we disable optimizations for this function.
// This work-around could/should be removed once the following versions of Visual Studio are no
// longer supported: All versions of VS2017, and versions of VS2019 below 16.4.
#यदि (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#निर्देशः optimize( "", off )
#यदि_अन्तः
U_CAPI int32_t U_EXPORT2
ubidi_writeReordered(UBiDi *pBiDi,
                     char16_t *dest, int32_t dest_आकार,
                     uint16_t options,
                     UError_संहिता *p_Error_संहिता) {
    ध्रुव char16_t *text;
    char16_t *saveDest;
    int32_t length, dest_क्षमता;
    int32_t सततम्_2, सततम्_गणनम्, logicalStart, सततम्_दीर्घता;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* more error checking */
    यदि( pBiDi==शून्यसूचकः ||
        (text=pBiDi->text)==शून्यसूचकः || (length=pBiDi->length)<0 ||
        dest_आकार<0 || (dest_आकार>0 && dest==शून्यसूचकः))
    {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* do input and output overlap? */
    यदि( dest!=शून्यसूचकः &&
        ((text>=dest && text<dest+dest_आकार) ||
         (dest>=text && dest<text+pBiDi->original_दीर्घता)))
    {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    यदि(length==0) {
        /* nothing to do */
        प्रत्यागम u_terminateUChars(dest, dest_आकार, 0, p_Error_संहिता);
    }

    सततम्_गणनम्=ubidi_गणनम्_Runs(pBiDi, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* destSize shrinks, later destination length=destCapacity-destSize */
    saveDest=dest;
    dest_क्षमता=dest_आकार;
    सततम्_दीर्घता = 0;

    /*
     * Option "insert marks" implies UBIDI_INSERT_LRM_FOR_NUMERIC if the
     * reordering mode (checked below) is appropriate.
     */
    यदि(pBiDi->reorderingOptions & UBIDI_OPTION_निवेशय_MARKS) {
        options|=UBIDI_निवेशय_LRM_FOR_NUMERIC;
        options&=~UBIDI_REMOVE_BIDI_CONTROLS;
    }
    /*
     * Option "remove controls" implies UBIDI_REMOVE_BIDI_CONTROLS
     * and cancels UBIDI_INSERT_LRM_FOR_NUMERIC.
     */
    यदि(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        options|=UBIDI_REMOVE_BIDI_CONTROLS;
        options&=~UBIDI_निवेशय_LRM_FOR_NUMERIC;
    }
    /*
     * If we do not perform the "inverse BiDi" algorithm, then we
     * don't need to insert any LRMs, and don't need to test for it.
     */
    यदि((pBiDi->reordering_बहुलकम् != UBIDI_REORDER_INVERSE_NUMBERS_AS_L) &&
       (pBiDi->reordering_बहुलकम् != UBIDI_REORDER_INVERSE_LIKE_प्रत्यक्षम्)  &&
       (pBiDi->reordering_बहुलकम् != UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL) &&
       (pBiDi->reordering_बहुलकम् != UBIDI_REORDER_RUNS_ONLY)) {
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
    यदि(!(options&UBIDI_OUTPUT_REVERSE)) {
        /* forward output */
        यदि(!(options&UBIDI_निवेशय_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            प्रति(सततम्_2=0; सततम्_2<सततम्_गणनम्; ++सततम्_2) {
                यदि(UBIDI_LTR==ubidi_get_Visual_सततम्(pBiDi, सततम्_2, &logicalStart, &सततम्_दीर्घता)) {
                    सततम्_दीर्घता=doWriteForward(text+logicalStart, सततम्_दीर्घता,
                                             dest, dest_आकार,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_संहिता);
                } अन्यथा {
                    सततम्_दीर्घता=doWriteReverse(text+logicalStart, सततम्_दीर्घता,
                                             dest, dest_आकार,
                                             options, p_Error_संहिता);
                }
                यदि(dest!=शून्यसूचकः) {
                  dest+=सततम्_दीर्घता;
                }
                dest_आकार-=सततम्_दीर्घता;
            }
        } अन्यथा {
            /* insert BiDi controls for "inverse BiDi" */
            ध्रुव DirProp *dirProps=pBiDi->dirProps;
            ध्रुव char16_t *src;
            char16_t uc;
            UBiDiDirection dir;
            int32_t markFlag;

            प्रति(सततम्_2=0; सततम्_2<सततम्_गणनम्; ++सततम्_2) {
                dir=ubidi_get_Visual_सततम्(pBiDi, सततम्_2, &logicalStart, &सततम्_दीर्घता);
                src=text+logicalStart;
                /* check if something relevant in insertPoints */
                markFlag=pBiDi->runs[सततम्_2].निवेशय_Remove;
                यदि(markFlag<0) {        /* BiDi controls count */
                    markFlag=0;
                }

                यदि(UBIDI_LTR==dir) {
                    यदि((pBiDi->isInverse) &&
                       (/*run>0 &&*/ dirProps[logicalStart]!=L)) {
                        markFlag |= LRM_BEFORE;
                    }
                    यदि (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    अन्यथा यदि (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    अन्यथा  uc=0;
                    यदि(uc) {
                        यदि(dest_आकार>0) {
                            *dest++=uc;
                        }
                        --dest_आकार;
                    }

                    सततम्_दीर्घता=doWriteForward(src, सततम्_दीर्घता,
                                             dest, dest_आकार,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_संहिता);
                    यदि(dest!=शून्यसूचकः) {
                      dest+=सततम्_दीर्घता;
                    }
                    dest_आकार-=सततम्_दीर्घता;

                    यदि((pBiDi->isInverse) &&
                           (/*run<runCount-1 &&*/
                            सततम्_दीर्घता > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            dirProps[logicalStart+सततम्_दीर्घता-1]!=L)) {
                        markFlag |= LRM_AFTER;
                    }
                    यदि (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    अन्यथा यदि (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    अन्यथा  uc=0;
                    यदि(uc) {
                        यदि(dest_आकार>0) {
                            *dest++=uc;
                        }
                        --dest_आकार;
                    }
                } अन्यथा {                /* RTL run */
                    यदि((pBiDi->isInverse) &&
                       (/*run>0 &&*/ !(आवरणम्_R_AL&DIRPROP_FLAG(dirProps[logicalStart+सततम्_दीर्घता-1])))) {
                        markFlag |= RLM_BEFORE;
                    }
                    यदि (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    अन्यथा यदि (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    अन्यथा  uc=0;
                    यदि(uc) {
                        यदि(dest_आकार>0) {
                            *dest++=uc;
                        }
                        --dest_आकार;
                    }

                    सततम्_दीर्घता=doWriteReverse(src, सततम्_दीर्घता,
                                             dest, dest_आकार,
                                             options, p_Error_संहिता);
                    यदि(dest!=शून्यसूचकः) {
                      dest+=सततम्_दीर्घता;
                    }
                    dest_आकार-=सततम्_दीर्घता;

                    यदि((pBiDi->isInverse) &&
                       (/*run<runCount-1 &&*/ !(आवरणम्_R_AL&DIRPROP_FLAG(dirProps[logicalStart])))) {
                        markFlag |= RLM_AFTER;
                    }
                    यदि (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    अन्यथा यदि (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    अन्यथा  uc=0;
                    यदि(uc) {
                        यदि(dest_आकार>0) {
                            *dest++=uc;
                        }
                        --dest_आकार;
                    }
                }
            }
        }
    } अन्यथा {
        /* reverse output */
        यदि(!(options&UBIDI_निवेशय_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            प्रति(सततम्_2=सततम्_गणनम्; --सततम्_2>=0;) {
                यदि(UBIDI_LTR==ubidi_get_Visual_सततम्(pBiDi, सततम्_2, &logicalStart, &सततम्_दीर्घता)) {
                    सततम्_दीर्घता=doWriteReverse(text+logicalStart, सततम्_दीर्घता,
                                             dest, dest_आकार,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_संहिता);
                } अन्यथा {
                    सततम्_दीर्घता=doWriteForward(text+logicalStart, सततम्_दीर्घता,
                                             dest, dest_आकार,
                                             options, p_Error_संहिता);
                }
                यदि(dest!=शून्यसूचकः) {
                  dest+=सततम्_दीर्घता;
                }
                dest_आकार-=सततम्_दीर्घता;
            }
        } अन्यथा {
            /* insert BiDi controls for "inverse BiDi" */
            ध्रुव DirProp *dirProps=pBiDi->dirProps;
            ध्रुव char16_t *src;
            UBiDiDirection dir;

            प्रति(सततम्_2=सततम्_गणनम्; --सततम्_2>=0;) {
                /* reverse output */
                dir=ubidi_get_Visual_सततम्(pBiDi, सततम्_2, &logicalStart, &सततम्_दीर्घता);
                src=text+logicalStart;

                यदि(UBIDI_LTR==dir) {
                    यदि(/*run<runCount-1 &&*/ dirProps[logicalStart+सततम्_दीर्घता-1]!=L) {
                        यदि(dest_आकार>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_आकार;
                    }

                    सततम्_दीर्घता=doWriteReverse(src, सततम्_दीर्घता,
                                             dest, dest_आकार,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_संहिता);
                    यदि(dest!=शून्यसूचकः) {
                      dest+=सततम्_दीर्घता;
                    }
                    dest_आकार-=सततम्_दीर्घता;

                    यदि(/*run>0 &&*/ dirProps[logicalStart]!=L) {
                        यदि(dest_आकार>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_आकार;
                    }
                } अन्यथा {
                    यदि(/*run<runCount-1 &&*/ !(आवरणम्_R_AL&DIRPROP_FLAG(dirProps[logicalStart]))) {
                        यदि(dest_आकार>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_आकार;
                    }

                    int32_t original_सततम्_दीर्घता = सततम्_दीर्घता;
                    सततम्_दीर्घता=doWriteForward(src, सततम्_दीर्घता,
                                             dest, dest_आकार,
                                             options, p_Error_संहिता);
                    यदि(dest!=शून्यसूचकः) {
                      dest+=सततम्_दीर्घता;
                    }
                    dest_आकार-=सततम्_दीर्घता;

                    यदि(/*run>0 &&*/
                            सततम्_दीर्घता > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            !(आवरणम्_R_AL&DIRPROP_FLAG(dirProps[logicalStart+original_सततम्_दीर्घता-1]))) {
                        यदि(dest_आकार>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_आकार;
                    }
                }
            }
        }
    }

    प्रत्यागम u_terminateUChars(saveDest, dest_क्षमता, dest_क्षमता-dest_आकार, p_Error_संहिता);
}
#यदि (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#निर्देशः optimize( "", on )
#यदि_अन्तः
