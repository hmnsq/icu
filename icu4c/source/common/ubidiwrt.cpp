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
#إذا defined(UTF_اندازه) && UTF_اندازه==8
# error reimplement ubidi_writeReordered() لكل UTF-8, see comment above
#د_که_پای

#تعریف_کړه IS_COMBINING(type) ((1UL<<(type))&(1UL<<U_NON_SPACING_MARK|1UL<<U_COMBINING_SPACING_MARK|1UL<<U_ENCLOSING_MARK))

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
doWriteForward(ثابت char16_t *src, int32_t src_اوږدتیا,
               char16_t *dest, int32_t dest_اندازه,
               uint16_t options,
               UError_کوډ *p_Error_کوډ) {
    إذا (src_اوږدتیا<=0) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    /* optimize for several combinations of options */
    بدل(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING)) {
    حالة 0: {
        /* simply copy the LTR run to the destination */
        int32_t length=src_اوږدتیا;
        إذا(dest_اندازه<length) {
            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
            أرجع src_اوږدتیا;
        }
        افعل {
            *dest++=*src++;
        } طالما(--length>0);
        أرجع src_اوږدتیا;
    }
    حالة UBIDI_DO_MIRRORING: {
        /* do mirroring */
        int32_t i=0, j=0;
        UChar32 c;
        افعل {
            U16_بل(src, i, src_اوږدتیا, c);
            c=u_charMirror(c);
            int32_t charLen = U16_اوږدتیا(c);
            إذا(j + charLen <= dest_اندازه) {
                int32_t لنډمهاله = j;
                U16_APPEND_UNSAFE(dest, لنډمهاله, c);
            }
            j += charLen;
        } طالما(i<src_اوږدتیا);
        إذا(dest_اندازه < j) {
            *p_Error_کوډ = U_بفر_ډکېدنه_ERROR;
        }
        إذا(j > src_اوږدتیا) {
            أرجع j;
        }
        /* If mirroring caused code unit contraction (e.g., U+1DB10 contracting to U+221D),
         * pad trailing array slots up to srcLength to avoid leaving uninitialized memory.
         * If j > 0, repeat the preceding code unit; if j == 0, fallback to 0x0020 (ASCII space).
         */
        طالما(j < src_اوږدتیا && j < dest_اندازه) {
            dest[j] = j > 0 ? dest[j - 1] : 0x0020;
            j++;
        }
        أرجع src_اوږدتیا;
    }
    حالة UBIDI_REMOVE_BIDI_CONTROLS: {
        /* copy the LTR run and remove any BiDi control characters */
        int32_t پاتې_شوی=dest_اندازه;
        char16_t c;
        افعل {
            c=*src++;
            إذا(!IS_BIDI_CONTROL_CHAR(c)) {
                إذا(--پاتې_شوی<0) {
                    *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;

                    /* preflight the length */
                    طالما(--src_اوږدتیا>0) {
                        c=*src++;
                        إذا(!IS_BIDI_CONTROL_CHAR(c)) {
                            --پاتې_شوی;
                        }
                    }
                    أرجع dest_اندازه-پاتې_شوی;
                }
                *dest++=c;
            }
        } طالما(--src_اوږدتیا>0);
        أرجع dest_اندازه-پاتې_شوی;
    }
    افتراضي: {
        /* remove BiDi control characters and do mirroring */
        int32_t i, j=0;
        UChar32 c;
        افعل {
            i=0;
            U16_بل(src, i, src_اوږدتیا, c);
            src+=i;
            src_اوږدتیا-=i;
            إذا(!IS_BIDI_CONTROL_CHAR(c)) {
                c=u_charMirror(c);
                int32_t charLen = U16_اوږدتیا(c);
                إذا(j + charLen <= dest_اندازه) {
                    int32_t لنډمهاله = j;
                    U16_APPEND_UNSAFE(dest, لنډمهاله, c);
                }
                j += charLen;
            }
        } طالما(src_اوږدتیا>0);
        إذا(dest_اندازه < j) {
            *p_Error_کوډ = U_بفر_ډکېدنه_ERROR;
        }
        أرجع j;
    }
    } /* end of switch */
}

ساكن int32_t
do_Write_سرچپه(ثابت char16_t *src, int32_t src_اوږدتیا,
               char16_t *dest, int32_t dest_اندازه,
               uint16_t options,
               UError_کوډ *p_Error_کوډ) {
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

    إذا(src_اوږدتیا<=0) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* optimize for several combinations of options */
    بدل(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING|UBIDI_KEEP_بنسټ_COMBINING)) {
    حالة 0:
        /*
         * With none of the "complicated" options set, the destination
         * run will have the same length as the source run,
         * and there is no mirroring and no keeping combining characters
         * with their base characters.
         */
        إذا(dest_اندازه<src_اوږدتیا) {
            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
            أرجع src_اوږدتیا;
        }
        dest_اندازه=src_اوږدتیا;

        /* preserve character integrity */
        افعل {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_اوږدتیا;

            /* collect code units for one base character */
            U16_PREV(src, 0, src_اوږدتیا, c);

            /* copy this base character */
            j=src_اوږدتیا;
            افعل {
                *dest++=src[j++];
            } طالما(j<i);
        } طالما(src_اوږدتیا>0);
        اكسر;
    حالة UBIDI_KEEP_بنسټ_COMBINING:
        /*
         * Here, too, the destination
         * run will have the same length as the source run,
         * and there is no mirroring.
         * We do need to keep combining characters with their base characters.
         */
        إذا(dest_اندازه<src_اوږدتیا) {
            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
            أرجع src_اوږدتیا;
        }
        dest_اندازه=src_اوږدتیا;

        /* preserve character integrity */
        افعل {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_اوږدتیا;

            /* collect code units and modifier letters for one base character */
            افعل {
                U16_PREV(src, 0, src_اوږدتیا, c);
            } طالما(src_اوږدتیا>0 && IS_COMBINING(u_charType(c)));

            /* copy this "user character" */
            j=src_اوږدتیا;
            افعل {
                *dest++=src[j++];
            } طالما(j<i);
        } طالما(src_اوږدتیا>0);
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
        إذا(options&UBIDI_دننی_RUNS_ONLY) {
            إذا(!(options&UBIDI_REMOVE_BIDI_CONTROLS)) {
                i=src_اوږدتیا;
            } وإلا {
                int32_t length=src_اوږدتیا;
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
                src-=src_اوږدتیا;
            }
            إذا(dest_اندازه<i) {
                *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                أرجع i;
            }
            char16_t *saveDest=dest;
            int32_t dest_اوږدتیا=0;
            /* Loop backward across the source run segment by segment, using legacy 1-to-1 code unit stepping
             * to extract and reverse character clusters for internal setPara classification.
             */
            افعل {
                i=src_اوږدتیا;
                U16_PREV(src, 0, src_اوږدتیا, c);
                إذا(options&UBIDI_KEEP_بنسټ_COMBINING) {
                    /* Loop backward over any combining marks attached to the base character so that the combining
                     * sequence remains attached after its base character when reversed.
                     */
                    طالما(src_اوږدتیا>0 && IS_COMBINING(u_charType(c))) {
                        U16_PREV(src, 0, src_اوږدتیا, c);
                    }
                }
                إذا(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                    استمر;
                }
                int32_t orig_بنسټ_Len=U16_اوږدتیا(c);
                j=src_اوږدتیا;
                إذا(options&UBIDI_DO_MIRRORING) {
                    c=u_charMirror(c);
                    int32_t k=U16_اوږدتیا(c);
                    إذا(dest_اوږدتیا+k<=dest_اندازه) {
                        int32_t لنډمهاله=dest_اوږدتیا;
                        U16_APPEND_UNSAFE(saveDest, لنډمهاله, c);
                    }
                    dest_اوږدتیا+=k;
                    j+=orig_بنسټ_Len;
                }
                /* Loop forward from the current segment index to copy any unmirrored code units (such as combining marks
                 * or surrogate halves) into dest in forward visual order.
                 */
                طالما(j<i) {
                    إذا(dest_اوږدتیا<dest_اندازه) {
                        saveDest[dest_اوږدتیا]=src[j];
                    }
                    dest_اوږدتیا++;
                    j++;
                }
            } طالما(src_اوږدتیا>0);
            إذا(dest_اندازه<dest_اوږدتیا) {
                *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
            }
            أرجع dest_اوږدتیا;
        }
        int32_t dest_اوږدتیا=0;
        افعل {
            i=src_اوږدتیا;
            U16_PREV(src, 0, src_اوږدتیا, c);
            إذا(options&UBIDI_KEEP_بنسټ_COMBINING) {
                طالما(src_اوږدتیا>0 && IS_COMBINING(u_charType(c))) {
                    U16_PREV(src, 0, src_اوږدتیا, c);
                }
            }
            إذا(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                استمر;
            }

            int32_t orig_بنسټ_Len=U16_اوږدتیا(c);
            j=src_اوږدتیا;
            إذا(options&UBIDI_DO_MIRRORING) {
                /* mirror only the base character */
                c=u_charMirror(c);
                int32_t k=U16_اوږدتیا(c);
                إذا(dest_اوږدتیا+k<=dest_اندازه) {
                    int32_t لنډمهاله=dest_اوږدتیا;
                    U16_APPEND_UNSAFE(dest, لنډمهاله, c);
                }
                dest_اوږدتیا+=k;
                j+=orig_بنسټ_Len;
            }
            طالما(j<i) {
                إذا(dest_اوږدتیا<dest_اندازه) {
                    dest[dest_اوږدتیا]=src[j];
                }
                dest_اوږدتیا++;
                j++;
            }
        } طالما(src_اوږدتیا>0);


        إذا(dest_اندازه<dest_اوږدتیا) {
            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
        }
        أرجع dest_اوږدتیا;
    }
    } /* end of switch */
    أرجع dest_اندازه;
}

U_CAPI int32_t U_EXPORT2
ubidi_write_سرچپه(ثابت char16_t *src, int32_t src_اوږدتیا,
                   char16_t *dest, int32_t dest_اندازه,
                   uint16_t options,
                   UError_کوډ *p_Error_کوډ) {
    int32_t dest_اوږدتیا;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    /* more error checking */
    إذا( src==تش_اشاره || src_اوږدتیا<-1 ||
        dest_اندازه<0 || (dest_اندازه>0 && dest==تش_اشاره))
    {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* do input and output overlap? */
    إذا( dest!=تش_اشاره &&
        ((src>=dest && src<dest+dest_اندازه) ||
         (dest>=src && dest<src+src_اوږدتیا)))
    {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(src_اوږدتیا==-1) {
        src_اوږدتیا=u_strlen(src);
    }
    إذا(src_اوږدتیا>0) {
        dest_اوږدتیا=do_Write_سرچپه(src, src_اوږدتیا, dest, dest_اندازه, options, p_Error_کوډ);
    } وإلا {
        /* nothing to do */
        dest_اوږدتیا=0;
    }

    أرجع u_terminateUChars(dest, dest_اندازه, dest_اوږدتیا, p_Error_کوډ);
}

// Ticket 20907 - The optimizer in MSVC/Visual Studio versions below 16.4 has trouble with this
// function on Windows ARM64. As a work-around, we disable optimizations for this function.
// This work-around could/should be removed once the following versions of Visual Studio are no
// longer supported: All versions of VS2017, and versions of VS2019 below 16.4.
#إذا (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#لارښوونه optimize( "", off )
#د_که_پای
U_CAPI int32_t U_EXPORT2
ubidi_writeReordered(UBiDi *pBiDi,
                     char16_t *dest, int32_t dest_اندازه,
                     uint16_t options,
                     UError_کوډ *p_Error_کوډ) {
    ثابت char16_t *متن;
    char16_t *saveDest;
    int32_t length, dest_ظرفیت;
    int32_t پرلپسې_2, پرلپسې_شمېر, logical_پیل, پرلپسې_اوږدتیا;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    /* more error checking */
    إذا( pBiDi==تش_اشاره ||
        (متن=pBiDi->متن)==تش_اشاره || (length=pBiDi->length)<0 ||
        dest_اندازه<0 || (dest_اندازه>0 && dest==تش_اشاره))
    {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* do input and output overlap? */
    إذا( dest!=تش_اشاره &&
        ((متن>=dest && متن<dest+dest_اندازه) ||
         (dest>=متن && dest<متن+pBiDi->original_اوږدتیا)))
    {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(length==0) {
        /* nothing to do */
        أرجع u_terminateUChars(dest, dest_اندازه, 0, p_Error_کوډ);
    }

    پرلپسې_شمېر=ubidi_شمېر_Runs(pBiDi, p_Error_کوډ);
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    /* destSize shrinks, later destination length=destCapacity-destSize */
    saveDest=dest;
    dest_ظرفیت=dest_اندازه;
    پرلپسې_اوږدتیا = 0;

    /*
     * Option "insert marks" implies UBIDI_INSERT_LRM_FOR_NUMERIC if the
     * reordering mode (checked below) is appropriate.
     */
    إذا(pBiDi->reorderingOptions & UBIDI_OPTION_ورننباسه_MARKS) {
        options|=UBIDI_ورننباسه_LRM_FOR_NUMERIC;
        options&=~UBIDI_REMOVE_BIDI_CONTROLS;
    }
    /*
     * Option "remove controls" implies UBIDI_REMOVE_BIDI_CONTROLS
     * and cancels UBIDI_INSERT_LRM_FOR_NUMERIC.
     */
    إذا(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        options|=UBIDI_REMOVE_BIDI_CONTROLS;
        options&=~UBIDI_ورننباسه_LRM_FOR_NUMERIC;
    }
    /*
     * If we do not perform the "inverse BiDi" algorithm, then we
     * don't need to insert any LRMs, and don't need to test for it.
     */
    إذا((pBiDi->reordering_ډېرپېښ != UBIDI_REORDER_INVERSE_NUMBERS_AS_L) &&
       (pBiDi->reordering_ډېرپېښ != UBIDI_REORDER_INVERSE_LIKE_مستقیم)  &&
       (pBiDi->reordering_ډېرپېښ != UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL) &&
       (pBiDi->reordering_ډېرپېښ != UBIDI_REORDER_RUNS_ONLY)) {
        options&=~UBIDI_ورننباسه_LRM_FOR_NUMERIC;
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
    إذا(!(options&UBIDI_OUTPUT_سرچپه)) {
        /* forward output */
        إذا(!(options&UBIDI_ورننباسه_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            لكل(پرلپسې_2=0; پرلپسې_2<پرلپسې_شمېر; ++پرلپسې_2) {
                إذا(UBIDI_LTR==ubidi_get_Visual_پرلپسې(pBiDi, پرلپسې_2, &logical_پیل, &پرلپسې_اوږدتیا)) {
                    پرلپسې_اوږدتیا=doWriteForward(متن+logical_پیل, پرلپسې_اوږدتیا,
                                             dest, dest_اندازه,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_کوډ);
                } وإلا {
                    پرلپسې_اوږدتیا=do_Write_سرچپه(متن+logical_پیل, پرلپسې_اوږدتیا,
                                             dest, dest_اندازه,
                                             options, p_Error_کوډ);
                }
                إذا(dest!=تش_اشاره) {
                  dest+=پرلپسې_اوږدتیا;
                }
                dest_اندازه-=پرلپسې_اوږدتیا;
            }
        } وإلا {
            /* insert BiDi controls for "inverse BiDi" */
            ثابت DirProp *dirProps=pBiDi->dirProps;
            ثابت char16_t *src;
            char16_t uc;
            UBiDiDirection dir;
            int32_t markFlag;

            لكل(پرلپسې_2=0; پرلپسې_2<پرلپسې_شمېر; ++پرلپسې_2) {
                dir=ubidi_get_Visual_پرلپسې(pBiDi, پرلپسې_2, &logical_پیل, &پرلپسې_اوږدتیا);
                src=متن+logical_پیل;
                /* check if something relevant in insertPoints */
                markFlag=pBiDi->runs[پرلپسې_2].ورننباسه_Remove;
                إذا(markFlag<0) {        /* BiDi controls count */
                    markFlag=0;
                }

                إذا(UBIDI_LTR==dir) {
                    إذا((pBiDi->isInverse) &&
                       (/*run>0 &&*/ dirProps[logical_پیل]!=L)) {
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
                        إذا(dest_اندازه>0) {
                            *dest++=uc;
                        }
                        --dest_اندازه;
                    }

                    پرلپسې_اوږدتیا=doWriteForward(src, پرلپسې_اوږدتیا,
                                             dest, dest_اندازه,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_کوډ);
                    إذا(dest!=تش_اشاره) {
                      dest+=پرلپسې_اوږدتیا;
                    }
                    dest_اندازه-=پرلپسې_اوږدتیا;

                    إذا((pBiDi->isInverse) &&
                           (/*run<runCount-1 &&*/
                            پرلپسې_اوږدتیا > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            dirProps[logical_پیل+پرلپسې_اوږدتیا-1]!=L)) {
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
                        إذا(dest_اندازه>0) {
                            *dest++=uc;
                        }
                        --dest_اندازه;
                    }
                } وإلا {                /* RTL run */
                    إذا((pBiDi->isInverse) &&
                       (/*run>0 &&*/ !(ماسک_R_AL&DIRPROP_FLAG(dirProps[logical_پیل+پرلپسې_اوږدتیا-1])))) {
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
                        إذا(dest_اندازه>0) {
                            *dest++=uc;
                        }
                        --dest_اندازه;
                    }

                    پرلپسې_اوږدتیا=do_Write_سرچپه(src, پرلپسې_اوږدتیا,
                                             dest, dest_اندازه,
                                             options, p_Error_کوډ);
                    إذا(dest!=تش_اشاره) {
                      dest+=پرلپسې_اوږدتیا;
                    }
                    dest_اندازه-=پرلپسې_اوږدتیا;

                    إذا((pBiDi->isInverse) &&
                       (/*run<runCount-1 &&*/ !(ماسک_R_AL&DIRPROP_FLAG(dirProps[logical_پیل])))) {
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
                        إذا(dest_اندازه>0) {
                            *dest++=uc;
                        }
                        --dest_اندازه;
                    }
                }
            }
        }
    } وإلا {
        /* reverse output */
        إذا(!(options&UBIDI_ورننباسه_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            لكل(پرلپسې_2=پرلپسې_شمېر; --پرلپسې_2>=0;) {
                إذا(UBIDI_LTR==ubidi_get_Visual_پرلپسې(pBiDi, پرلپسې_2, &logical_پیل, &پرلپسې_اوږدتیا)) {
                    پرلپسې_اوږدتیا=do_Write_سرچپه(متن+logical_پیل, پرلپسې_اوږدتیا,
                                             dest, dest_اندازه,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_کوډ);
                } وإلا {
                    پرلپسې_اوږدتیا=doWriteForward(متن+logical_پیل, پرلپسې_اوږدتیا,
                                             dest, dest_اندازه,
                                             options, p_Error_کوډ);
                }
                إذا(dest!=تش_اشاره) {
                  dest+=پرلپسې_اوږدتیا;
                }
                dest_اندازه-=پرلپسې_اوږدتیا;
            }
        } وإلا {
            /* insert BiDi controls for "inverse BiDi" */
            ثابت DirProp *dirProps=pBiDi->dirProps;
            ثابت char16_t *src;
            UBiDiDirection dir;

            لكل(پرلپسې_2=پرلپسې_شمېر; --پرلپسې_2>=0;) {
                /* reverse output */
                dir=ubidi_get_Visual_پرلپسې(pBiDi, پرلپسې_2, &logical_پیل, &پرلپسې_اوږدتیا);
                src=متن+logical_پیل;

                إذا(UBIDI_LTR==dir) {
                    إذا(/*run<runCount-1 &&*/ dirProps[logical_پیل+پرلپسې_اوږدتیا-1]!=L) {
                        إذا(dest_اندازه>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_اندازه;
                    }

                    پرلپسې_اوږدتیا=do_Write_سرچپه(src, پرلپسې_اوږدتیا,
                                             dest, dest_اندازه,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_کوډ);
                    إذا(dest!=تش_اشاره) {
                      dest+=پرلپسې_اوږدتیا;
                    }
                    dest_اندازه-=پرلپسې_اوږدتیا;

                    إذا(/*run>0 &&*/ dirProps[logical_پیل]!=L) {
                        إذا(dest_اندازه>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_اندازه;
                    }
                } وإلا {
                    إذا(/*run<runCount-1 &&*/ !(ماسک_R_AL&DIRPROP_FLAG(dirProps[logical_پیل]))) {
                        إذا(dest_اندازه>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_اندازه;
                    }

                    int32_t original_پرلپسې_اوږدتیا = پرلپسې_اوږدتیا;
                    پرلپسې_اوږدتیا=doWriteForward(src, پرلپسې_اوږدتیا,
                                             dest, dest_اندازه,
                                             options, p_Error_کوډ);
                    إذا(dest!=تش_اشاره) {
                      dest+=پرلپسې_اوږدتیا;
                    }
                    dest_اندازه-=پرلپسې_اوږدتیا;

                    إذا(/*run>0 &&*/
                            پرلپسې_اوږدتیا > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            !(ماسک_R_AL&DIRPROP_FLAG(dirProps[logical_پیل+original_پرلپسې_اوږدتیا-1]))) {
                        إذا(dest_اندازه>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_اندازه;
                    }
                }
            }
        }
    }

    أرجع u_terminateUChars(saveDest, dest_ظرفیت, dest_ظرفیت-dest_اندازه, p_Error_کوډ);
}
#إذا (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#لارښوونه optimize( "", on )
#د_که_پای
