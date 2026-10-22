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
#إذا defined(UTF_ناپ) && UTF_ناپ==8
# error reimplement ubidi_writeReordered() لكل UTF-8, see comment above
#جے_انت

#تعریف_کرو IS_COMBINING(type) ((1UL<<(type))&(1UL<<U_NON_SPACING_MARK|1UL<<U_COMBINING_SPACING_MARK|1UL<<U_ENCLOSING_MARK))

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
doWriteForward(ثابت char16_t *src, int32_t src_لمائی,
               char16_t *dest, int32_t dest_ناپ,
               uint16_t options,
               UError_کوڈ *p_Error_کوڈ) {
    إذا (src_لمائی<=0) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    /* optimize for several combinations of options */
    بدل(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING)) {
    حالة 0: {
        /* simply copy the LTR run to the destination */
        int32_t length=src_لمائی;
        إذا(dest_ناپ<length) {
            *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
            أرجع src_لمائی;
        }
        افعل {
            *dest++=*src++;
        } طالما(--length>0);
        أرجع src_لمائی;
    }
    حالة UBIDI_DO_MIRRORING: {
        /* do mirroring */
        int32_t i=0, j=0;
        UChar32 c;
        افعل {
            U16_اگلا(src, i, src_لمائی, c);
            c=u_charMirror(c);
            int32_t charLen = U16_لمائی(c);
            إذا(j + charLen <= dest_ناپ) {
                int32_t عارضی = j;
                U16_APPEND_UNSAFE(dest, عارضی, c);
            }
            j += charLen;
        } طالما(i<src_لمائی);
        إذا(dest_ناپ < j) {
            *p_Error_کوڈ = U_بفر_حد_توں_ودھ_ERROR;
        }
        إذا(j > src_لمائی) {
            أرجع j;
        }
        /* If mirroring caused code unit contraction (e.g., U+1DB10 contracting to U+221D),
         * pad trailing array slots up to srcLength to avoid leaving uninitialized memory.
         * If j > 0, repeat the preceding code unit; if j == 0, fallback to 0x0020 (ASCII space).
         */
        طالما(j < src_لمائی && j < dest_ناپ) {
            dest[j] = j > 0 ? dest[j - 1] : 0x0020;
            j++;
        }
        أرجع src_لمائی;
    }
    حالة UBIDI_REMOVE_BIDI_CONTROLS: {
        /* copy the LTR run and remove any BiDi control characters */
        int32_t بچیا=dest_ناپ;
        char16_t c;
        افعل {
            c=*src++;
            إذا(!IS_BIDI_CONTROL_CHAR(c)) {
                إذا(--بچیا<0) {
                    *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;

                    /* preflight the length */
                    طالما(--src_لمائی>0) {
                        c=*src++;
                        إذا(!IS_BIDI_CONTROL_CHAR(c)) {
                            --بچیا;
                        }
                    }
                    أرجع dest_ناپ-بچیا;
                }
                *dest++=c;
            }
        } طالما(--src_لمائی>0);
        أرجع dest_ناپ-بچیا;
    }
    افتراضي: {
        /* remove BiDi control characters and do mirroring */
        int32_t i, j=0;
        UChar32 c;
        افعل {
            i=0;
            U16_اگلا(src, i, src_لمائی, c);
            src+=i;
            src_لمائی-=i;
            إذا(!IS_BIDI_CONTROL_CHAR(c)) {
                c=u_charMirror(c);
                int32_t charLen = U16_لمائی(c);
                إذا(j + charLen <= dest_ناپ) {
                    int32_t عارضی = j;
                    U16_APPEND_UNSAFE(dest, عارضی, c);
                }
                j += charLen;
            }
        } طالما(src_لمائی>0);
        إذا(dest_ناپ < j) {
            *p_Error_کوڈ = U_بفر_حد_توں_ودھ_ERROR;
        }
        أرجع j;
    }
    } /* end of switch */
}

ساكن int32_t
do_Write_الٹ(ثابت char16_t *src, int32_t src_لمائی,
               char16_t *dest, int32_t dest_ناپ,
               uint16_t options,
               UError_کوڈ *p_Error_کوڈ) {
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

    إذا(src_لمائی<=0) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* optimize for several combinations of options */
    بدل(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING|UBIDI_KEEP_ادھار_COMBINING)) {
    حالة 0:
        /*
         * With none of the "complicated" options set, the destination
         * run will have the same length as the source run,
         * and there is no mirroring and no keeping combining characters
         * with their base characters.
         */
        إذا(dest_ناپ<src_لمائی) {
            *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
            أرجع src_لمائی;
        }
        dest_ناپ=src_لمائی;

        /* preserve character integrity */
        افعل {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_لمائی;

            /* collect code units for one base character */
            U16_PREV(src, 0, src_لمائی, c);

            /* copy this base character */
            j=src_لمائی;
            افعل {
                *dest++=src[j++];
            } طالما(j<i);
        } طالما(src_لمائی>0);
        اكسر;
    حالة UBIDI_KEEP_ادھار_COMBINING:
        /*
         * Here, too, the destination
         * run will have the same length as the source run,
         * and there is no mirroring.
         * We do need to keep combining characters with their base characters.
         */
        إذا(dest_ناپ<src_لمائی) {
            *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
            أرجع src_لمائی;
        }
        dest_ناپ=src_لمائی;

        /* preserve character integrity */
        افعل {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_لمائی;

            /* collect code units and modifier letters for one base character */
            افعل {
                U16_PREV(src, 0, src_لمائی, c);
            } طالما(src_لمائی>0 && IS_COMBINING(u_charType(c)));

            /* copy this "user character" */
            j=src_لمائی;
            افعل {
                *dest++=src[j++];
            } طالما(j<i);
        } طالما(src_لمائی>0);
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
        إذا(options&UBIDI_اندرونی_RUNS_ONLY) {
            إذا(!(options&UBIDI_REMOVE_BIDI_CONTROLS)) {
                i=src_لمائی;
            } وإلا {
                int32_t length=src_لمائی;
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
                } طالما(--length>0);
                src-=src_لمائی;
            }
            إذا(dest_ناپ<i) {
                *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
                أرجع i;
            }
            char16_t *saveDest=dest;
            int32_t dest_لمائی=0;
            /* Loop backward across the source run segment by segment, using legacy 1-to-1 code unit stepping
             * to extract and reverse character clusters for internal setPara classification.
             */
            افعل {
                i=src_لمائی;
                U16_PREV(src, 0, src_لمائی, c);
                إذا(options&UBIDI_KEEP_ادھار_COMBINING) {
                    /* Loop backward over any combining marks attached to the base character so that the combining
                     * sequence remains attached after its base character when reversed.
                     */
                    طالما(src_لمائی>0 && IS_COMBINING(u_charType(c))) {
                        U16_PREV(src, 0, src_لمائی, c);
                    }
                }
                إذا(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                    استمر;
                }
                int32_t orig_ادھار_Len=U16_لمائی(c);
                j=src_لمائی;
                إذا(options&UBIDI_DO_MIRRORING) {
                    c=u_charMirror(c);
                    int32_t k=U16_لمائی(c);
                    إذا(dest_لمائی+k<=dest_ناپ) {
                        int32_t عارضی=dest_لمائی;
                        U16_APPEND_UNSAFE(saveDest, عارضی, c);
                    }
                    dest_لمائی+=k;
                    j+=orig_ادھار_Len;
                }
                /* Loop forward from the current segment index to copy any unmirrored code units (such as combining marks
                 * or surrogate halves) into dest in forward visual order.
                 */
                طالما(j<i) {
                    إذا(dest_لمائی<dest_ناپ) {
                        saveDest[dest_لمائی]=src[j];
                    }
                    dest_لمائی++;
                    j++;
                }
            } طالما(src_لمائی>0);
            إذا(dest_ناپ<dest_لمائی) {
                *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
            }
            أرجع dest_لمائی;
        }
        int32_t dest_لمائی=0;
        افعل {
            i=src_لمائی;
            U16_PREV(src, 0, src_لمائی, c);
            إذا(options&UBIDI_KEEP_ادھار_COMBINING) {
                طالما(src_لمائی>0 && IS_COMBINING(u_charType(c))) {
                    U16_PREV(src, 0, src_لمائی, c);
                }
            }
            إذا(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                استمر;
            }

            int32_t orig_ادھار_Len=U16_لمائی(c);
            j=src_لمائی;
            إذا(options&UBIDI_DO_MIRRORING) {
                /* mirror only the base character */
                c=u_charMirror(c);
                int32_t k=U16_لمائی(c);
                إذا(dest_لمائی+k<=dest_ناپ) {
                    int32_t عارضی=dest_لمائی;
                    U16_APPEND_UNSAFE(dest, عارضی, c);
                }
                dest_لمائی+=k;
                j+=orig_ادھار_Len;
            }
            طالما(j<i) {
                إذا(dest_لمائی<dest_ناپ) {
                    dest[dest_لمائی]=src[j];
                }
                dest_لمائی++;
                j++;
            }
        } طالما(src_لمائی>0);


        إذا(dest_ناپ<dest_لمائی) {
            *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
        }
        أرجع dest_لمائی;
    }
    } /* end of switch */
    أرجع dest_ناپ;
}

U_CAPI int32_t U_EXPORT2
ubidi_write_الٹ(ثابت char16_t *src, int32_t src_لمائی,
                   char16_t *dest, int32_t dest_ناپ,
                   uint16_t options,
                   UError_کوڈ *p_Error_کوڈ) {
    int32_t dest_لمائی;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    /* more error checking */
    إذا( src==خالی_سوچک || src_لمائی<-1 ||
        dest_ناپ<0 || (dest_ناپ>0 && dest==خالی_سوچک))
    {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* do input and output overlap? */
    إذا( dest!=خالی_سوچک &&
        ((src>=dest && src<dest+dest_ناپ) ||
         (dest>=src && dest<src+src_لمائی)))
    {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(src_لمائی==-1) {
        src_لمائی=u_strlen(src);
    }
    إذا(src_لمائی>0) {
        dest_لمائی=do_Write_الٹ(src, src_لمائی, dest, dest_ناپ, options, p_Error_کوڈ);
    } وإلا {
        /* nothing to do */
        dest_لمائی=0;
    }

    أرجع u_terminateUChars(dest, dest_ناپ, dest_لمائی, p_Error_کوڈ);
}

// Ticket 20907 - The optimizer in MSVC/Visual Studio versions below 16.4 has trouble with this
// function on Windows ARM64. As a work-around, we disable optimizations for this function.
// This work-around could/should be removed once the following versions of Visual Studio are no
// longer supported: All versions of VS2017, and versions of VS2019 below 16.4.
#إذا (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ہدایت optimize( "", off )
#جے_انت
U_CAPI int32_t U_EXPORT2
ubidi_writeReordered(UBiDi *pBiDi,
                     char16_t *dest, int32_t dest_ناپ,
                     uint16_t options,
                     UError_کوڈ *p_Error_کوڈ) {
    ثابت char16_t *متن;
    char16_t *saveDest;
    int32_t length, dest_سمرتھا;
    int32_t لگاتار_2, لگاتار_گنتی, logical_شروع, لگاتار_لمائی;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    /* more error checking */
    إذا( pBiDi==خالی_سوچک ||
        (متن=pBiDi->متن)==خالی_سوچک || (length=pBiDi->length)<0 ||
        dest_ناپ<0 || (dest_ناپ>0 && dest==خالی_سوچک))
    {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* do input and output overlap? */
    إذا( dest!=خالی_سوچک &&
        ((متن>=dest && متن<dest+dest_ناپ) ||
         (dest>=متن && dest<متن+pBiDi->original_لمائی)))
    {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(length==0) {
        /* nothing to do */
        أرجع u_terminateUChars(dest, dest_ناپ, 0, p_Error_کوڈ);
    }

    لگاتار_گنتی=ubidi_گنتی_Runs(pBiDi, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    /* destSize shrinks, later destination length=destCapacity-destSize */
    saveDest=dest;
    dest_سمرتھا=dest_ناپ;
    لگاتار_لمائی = 0;

    /*
     * Option "insert marks" implies UBIDI_INSERT_LRM_FOR_NUMERIC if the
     * reordering mode (checked below) is appropriate.
     */
    إذا(pBiDi->reorderingOptions & UBIDI_OPTION_داخل_کرو_MARKS) {
        options|=UBIDI_داخل_کرو_LRM_FOR_NUMERIC;
        options&=~UBIDI_REMOVE_BIDI_CONTROLS;
    }
    /*
     * Option "remove controls" implies UBIDI_REMOVE_BIDI_CONTROLS
     * and cancels UBIDI_INSERT_LRM_FOR_NUMERIC.
     */
    إذا(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        options|=UBIDI_REMOVE_BIDI_CONTROLS;
        options&=~UBIDI_داخل_کرو_LRM_FOR_NUMERIC;
    }
    /*
     * If we do not perform the "inverse BiDi" algorithm, then we
     * don't need to insert any LRMs, and don't need to test for it.
     */
    إذا((pBiDi->reordering_منوال != UBIDI_REORDER_INVERSE_NUMBERS_AS_L) &&
       (pBiDi->reordering_منوال != UBIDI_REORDER_INVERSE_LIKE_سدھا)  &&
       (pBiDi->reordering_منوال != UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL) &&
       (pBiDi->reordering_منوال != UBIDI_REORDER_RUNS_ONLY)) {
        options&=~UBIDI_داخل_کرو_LRM_FOR_NUMERIC;
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
    إذا(!(options&UBIDI_OUTPUT_الٹ)) {
        /* forward output */
        إذا(!(options&UBIDI_داخل_کرو_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            لكل(لگاتار_2=0; لگاتار_2<لگاتار_گنتی; ++لگاتار_2) {
                إذا(UBIDI_LTR==ubidi_get_Visual_لگاتار(pBiDi, لگاتار_2, &logical_شروع, &لگاتار_لمائی)) {
                    لگاتار_لمائی=doWriteForward(متن+logical_شروع, لگاتار_لمائی,
                                             dest, dest_ناپ,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_کوڈ);
                } وإلا {
                    لگاتار_لمائی=do_Write_الٹ(متن+logical_شروع, لگاتار_لمائی,
                                             dest, dest_ناپ,
                                             options, p_Error_کوڈ);
                }
                إذا(dest!=خالی_سوچک) {
                  dest+=لگاتار_لمائی;
                }
                dest_ناپ-=لگاتار_لمائی;
            }
        } وإلا {
            /* insert BiDi controls for "inverse BiDi" */
            ثابت DirProp *dirProps=pBiDi->dirProps;
            ثابت char16_t *src;
            char16_t uc;
            UBiDiDirection dir;
            int32_t markFlag;

            لكل(لگاتار_2=0; لگاتار_2<لگاتار_گنتی; ++لگاتار_2) {
                dir=ubidi_get_Visual_لگاتار(pBiDi, لگاتار_2, &logical_شروع, &لگاتار_لمائی);
                src=متن+logical_شروع;
                /* check if something relevant in insertPoints */
                markFlag=pBiDi->runs[لگاتار_2].داخل_کرو_Remove;
                إذا(markFlag<0) {        /* BiDi controls count */
                    markFlag=0;
                }

                إذا(UBIDI_LTR==dir) {
                    إذا((pBiDi->isInverse) &&
                       (/*run>0 &&*/ dirProps[logical_شروع]!=L)) {
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
                        إذا(dest_ناپ>0) {
                            *dest++=uc;
                        }
                        --dest_ناپ;
                    }

                    لگاتار_لمائی=doWriteForward(src, لگاتار_لمائی,
                                             dest, dest_ناپ,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_کوڈ);
                    إذا(dest!=خالی_سوچک) {
                      dest+=لگاتار_لمائی;
                    }
                    dest_ناپ-=لگاتار_لمائی;

                    إذا((pBiDi->isInverse) &&
                           (/*run<runCount-1 &&*/
                            لگاتار_لمائی > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            dirProps[logical_شروع+لگاتار_لمائی-1]!=L)) {
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
                        إذا(dest_ناپ>0) {
                            *dest++=uc;
                        }
                        --dest_ناپ;
                    }
                } وإلا {                /* RTL run */
                    إذا((pBiDi->isInverse) &&
                       (/*run>0 &&*/ !(نقاب_R_AL&DIRPROP_FLAG(dirProps[logical_شروع+لگاتار_لمائی-1])))) {
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
                        إذا(dest_ناپ>0) {
                            *dest++=uc;
                        }
                        --dest_ناپ;
                    }

                    لگاتار_لمائی=do_Write_الٹ(src, لگاتار_لمائی,
                                             dest, dest_ناپ,
                                             options, p_Error_کوڈ);
                    إذا(dest!=خالی_سوچک) {
                      dest+=لگاتار_لمائی;
                    }
                    dest_ناپ-=لگاتار_لمائی;

                    إذا((pBiDi->isInverse) &&
                       (/*run<runCount-1 &&*/ !(نقاب_R_AL&DIRPROP_FLAG(dirProps[logical_شروع])))) {
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
                        إذا(dest_ناپ>0) {
                            *dest++=uc;
                        }
                        --dest_ناپ;
                    }
                }
            }
        }
    } وإلا {
        /* reverse output */
        إذا(!(options&UBIDI_داخل_کرو_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            لكل(لگاتار_2=لگاتار_گنتی; --لگاتار_2>=0;) {
                إذا(UBIDI_LTR==ubidi_get_Visual_لگاتار(pBiDi, لگاتار_2, &logical_شروع, &لگاتار_لمائی)) {
                    لگاتار_لمائی=do_Write_الٹ(متن+logical_شروع, لگاتار_لمائی,
                                             dest, dest_ناپ,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_کوڈ);
                } وإلا {
                    لگاتار_لمائی=doWriteForward(متن+logical_شروع, لگاتار_لمائی,
                                             dest, dest_ناپ,
                                             options, p_Error_کوڈ);
                }
                إذا(dest!=خالی_سوچک) {
                  dest+=لگاتار_لمائی;
                }
                dest_ناپ-=لگاتار_لمائی;
            }
        } وإلا {
            /* insert BiDi controls for "inverse BiDi" */
            ثابت DirProp *dirProps=pBiDi->dirProps;
            ثابت char16_t *src;
            UBiDiDirection dir;

            لكل(لگاتار_2=لگاتار_گنتی; --لگاتار_2>=0;) {
                /* reverse output */
                dir=ubidi_get_Visual_لگاتار(pBiDi, لگاتار_2, &logical_شروع, &لگاتار_لمائی);
                src=متن+logical_شروع;

                إذا(UBIDI_LTR==dir) {
                    إذا(/*run<runCount-1 &&*/ dirProps[logical_شروع+لگاتار_لمائی-1]!=L) {
                        إذا(dest_ناپ>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_ناپ;
                    }

                    لگاتار_لمائی=do_Write_الٹ(src, لگاتار_لمائی,
                                             dest, dest_ناپ,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_کوڈ);
                    إذا(dest!=خالی_سوچک) {
                      dest+=لگاتار_لمائی;
                    }
                    dest_ناپ-=لگاتار_لمائی;

                    إذا(/*run>0 &&*/ dirProps[logical_شروع]!=L) {
                        إذا(dest_ناپ>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_ناپ;
                    }
                } وإلا {
                    إذا(/*run<runCount-1 &&*/ !(نقاب_R_AL&DIRPROP_FLAG(dirProps[logical_شروع]))) {
                        إذا(dest_ناپ>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_ناپ;
                    }

                    int32_t original_لگاتار_لمائی = لگاتار_لمائی;
                    لگاتار_لمائی=doWriteForward(src, لگاتار_لمائی,
                                             dest, dest_ناپ,
                                             options, p_Error_کوڈ);
                    إذا(dest!=خالی_سوچک) {
                      dest+=لگاتار_لمائی;
                    }
                    dest_ناپ-=لگاتار_لمائی;

                    إذا(/*run>0 &&*/
                            لگاتار_لمائی > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            !(نقاب_R_AL&DIRPROP_FLAG(dirProps[logical_شروع+original_لگاتار_لمائی-1]))) {
                        إذا(dest_ناپ>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_ناپ;
                    }
                }
            }
        }
    }

    أرجع u_terminateUChars(saveDest, dest_سمرتھا, dest_سمرتھا-dest_ناپ, p_Error_کوڈ);
}
#إذا (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ہدایت optimize( "", on )
#جے_انت
