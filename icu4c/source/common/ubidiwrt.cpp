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

#شامل "unicode/utypes.h"
#شامل "unicode/ustring.h"
#شامل "unicode/uchar.h"
#شامل "unicode/ubidi.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "ustr_imp.h"
#شامل "ubidiimp.h"

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
#اگر defined(UTF_سائز) && UTF_سائز==8
# error reimplement ubidi_writeReordered() برائے UTF-8, see comment above
#اگر_ختم

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
ساکن int32_t
doWriteForward(ثابت char16_t *src, int32_t src_لمبائی,
               char16_t *dest, int32_t dest_سائز,
               uint16_t options,
               UError_رمز *p_Error_رمز) {
    اگر (src_لمبائی<=0) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    /* optimize for several combinations of options */
    انتخاب(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING)) {
    حالت 0: {
        /* simply copy the LTR run to the destination */
        int32_t length=src_لمبائی;
        اگر(dest_سائز<length) {
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            واپس src_لمبائی;
        }
        کرو {
            *dest++=*src++;
        } جب(--length>0);
        واپس src_لمبائی;
    }
    حالت UBIDI_DO_MIRRORING: {
        /* do mirroring */
        int32_t i=0, j=0;
        UChar32 c;
        کرو {
            U16_اگلا(src, i, src_لمبائی, c);
            c=u_charMirror(c);
            int32_t charLen = U16_لمبائی(c);
            اگر(j + charLen <= dest_سائز) {
                int32_t عارضی = j;
                U16_APPEND_UNSAFE(dest, عارضی, c);
            }
            j += charLen;
        } جب(i<src_لمبائی);
        اگر(dest_سائز < j) {
            *p_Error_رمز = U_بفر_حد_سے_تجاوز_ERROR;
        }
        اگر(j > src_لمبائی) {
            واپس j;
        }
        /* If mirroring caused code unit contraction (e.g., U+1DB10 contracting to U+221D),
         * pad trailing array slots up to srcLength to avoid leaving uninitialized memory.
         * If j > 0, repeat the preceding code unit; if j == 0, fallback to 0x0020 (ASCII space).
         */
        جب(j < src_لمبائی && j < dest_سائز) {
            dest[j] = j > 0 ? dest[j - 1] : 0x0020;
            j++;
        }
        واپس src_لمبائی;
    }
    حالت UBIDI_REMOVE_BIDI_CONTROLS: {
        /* copy the LTR run and remove any BiDi control characters */
        int32_t بقایا=dest_سائز;
        char16_t c;
        کرو {
            c=*src++;
            اگر(!IS_BIDI_CONTROL_CHAR(c)) {
                اگر(--بقایا<0) {
                    *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;

                    /* preflight the length */
                    جب(--src_لمبائی>0) {
                        c=*src++;
                        اگر(!IS_BIDI_CONTROL_CHAR(c)) {
                            --بقایا;
                        }
                    }
                    واپس dest_سائز-بقایا;
                }
                *dest++=c;
            }
        } جب(--src_لمبائی>0);
        واپس dest_سائز-بقایا;
    }
    طےشدہ: {
        /* remove BiDi control characters and do mirroring */
        int32_t i, j=0;
        UChar32 c;
        کرو {
            i=0;
            U16_اگلا(src, i, src_لمبائی, c);
            src+=i;
            src_لمبائی-=i;
            اگر(!IS_BIDI_CONTROL_CHAR(c)) {
                c=u_charMirror(c);
                int32_t charLen = U16_لمبائی(c);
                اگر(j + charLen <= dest_سائز) {
                    int32_t عارضی = j;
                    U16_APPEND_UNSAFE(dest, عارضی, c);
                }
                j += charLen;
            }
        } جب(src_لمبائی>0);
        اگر(dest_سائز < j) {
            *p_Error_رمز = U_بفر_حد_سے_تجاوز_ERROR;
        }
        واپس j;
    }
    } /* end of switch */
}

ساکن int32_t
do_Write_الٹ(ثابت char16_t *src, int32_t src_لمبائی,
               char16_t *dest, int32_t dest_سائز,
               uint16_t options,
               UError_رمز *p_Error_رمز) {
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

    اگر(src_لمبائی<=0) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* optimize for several combinations of options */
    انتخاب(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING|UBIDI_KEEP_بنیاد_COMBINING)) {
    حالت 0:
        /*
         * With none of the "complicated" options set, the destination
         * run will have the same length as the source run,
         * and there is no mirroring and no keeping combining characters
         * with their base characters.
         */
        اگر(dest_سائز<src_لمبائی) {
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            واپس src_لمبائی;
        }
        dest_سائز=src_لمبائی;

        /* preserve character integrity */
        کرو {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_لمبائی;

            /* collect code units for one base character */
            U16_پچھلا(src, 0, src_لمبائی, c);

            /* copy this base character */
            j=src_لمبائی;
            کرو {
                *dest++=src[j++];
            } جب(j<i);
        } جب(src_لمبائی>0);
        توڑو;
    حالت UBIDI_KEEP_بنیاد_COMBINING:
        /*
         * Here, too, the destination
         * run will have the same length as the source run,
         * and there is no mirroring.
         * We do need to keep combining characters with their base characters.
         */
        اگر(dest_سائز<src_لمبائی) {
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            واپس src_لمبائی;
        }
        dest_سائز=src_لمبائی;

        /* preserve character integrity */
        کرو {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_لمبائی;

            /* collect code units and modifier letters for one base character */
            کرو {
                U16_پچھلا(src, 0, src_لمبائی, c);
            } جب(src_لمبائی>0 && IS_COMBINING(u_charType(c)));

            /* copy this "user character" */
            j=src_لمبائی;
            کرو {
                *dest++=src[j++];
            } جب(j<i);
        } جب(src_لمبائی>0);
        توڑو;
    طےشدہ: {
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
        اگر(options&UBIDI_اندرونی_RUNS_ONLY) {
            اگر(!(options&UBIDI_REMOVE_BIDI_CONTROLS)) {
                i=src_لمبائی;
            } ورنہ {
                int32_t length=src_لمبائی;
                char16_t ch;
                i=0;
                /* Loop over all code units in the source run to count those that are not Bidi control characters,
                 * determining the required destination buffer length for preflighting when UBIDI_REMOVE_BIDI_CONTROLS is set.
                 */
                کرو {
                    ch=*src++;
                    اگر(!IS_BIDI_CONTROL_CHAR(ch)) {
                        ++i;
                    }
                } جب(--length>0);
                src-=src_لمبائی;
            }
            اگر(dest_سائز<i) {
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                واپس i;
            }
            char16_t *saveDest=dest;
            int32_t dest_لمبائی=0;
            /* Loop backward across the source run segment by segment, using legacy 1-to-1 code unit stepping
             * to extract and reverse character clusters for internal setPara classification.
             */
            کرو {
                i=src_لمبائی;
                U16_پچھلا(src, 0, src_لمبائی, c);
                اگر(options&UBIDI_KEEP_بنیاد_COMBINING) {
                    /* Loop backward over any combining marks attached to the base character so that the combining
                     * sequence remains attached after its base character when reversed.
                     */
                    جب(src_لمبائی>0 && IS_COMBINING(u_charType(c))) {
                        U16_پچھلا(src, 0, src_لمبائی, c);
                    }
                }
                اگر(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                    جاری;
                }
                int32_t orig_بنیاد_Len=U16_لمبائی(c);
                j=src_لمبائی;
                اگر(options&UBIDI_DO_MIRRORING) {
                    c=u_charMirror(c);
                    int32_t k=U16_لمبائی(c);
                    اگر(dest_لمبائی+k<=dest_سائز) {
                        int32_t عارضی=dest_لمبائی;
                        U16_APPEND_UNSAFE(saveDest, عارضی, c);
                    }
                    dest_لمبائی+=k;
                    j+=orig_بنیاد_Len;
                }
                /* Loop forward from the current segment index to copy any unmirrored code units (such as combining marks
                 * or surrogate halves) into dest in forward visual order.
                 */
                جب(j<i) {
                    اگر(dest_لمبائی<dest_سائز) {
                        saveDest[dest_لمبائی]=src[j];
                    }
                    dest_لمبائی++;
                    j++;
                }
            } جب(src_لمبائی>0);
            اگر(dest_سائز<dest_لمبائی) {
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
            }
            واپس dest_لمبائی;
        }
        int32_t dest_لمبائی=0;
        کرو {
            i=src_لمبائی;
            U16_پچھلا(src, 0, src_لمبائی, c);
            اگر(options&UBIDI_KEEP_بنیاد_COMBINING) {
                جب(src_لمبائی>0 && IS_COMBINING(u_charType(c))) {
                    U16_پچھلا(src, 0, src_لمبائی, c);
                }
            }
            اگر(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                جاری;
            }

            int32_t orig_بنیاد_Len=U16_لمبائی(c);
            j=src_لمبائی;
            اگر(options&UBIDI_DO_MIRRORING) {
                /* mirror only the base character */
                c=u_charMirror(c);
                int32_t k=U16_لمبائی(c);
                اگر(dest_لمبائی+k<=dest_سائز) {
                    int32_t عارضی=dest_لمبائی;
                    U16_APPEND_UNSAFE(dest, عارضی, c);
                }
                dest_لمبائی+=k;
                j+=orig_بنیاد_Len;
            }
            جب(j<i) {
                اگر(dest_لمبائی<dest_سائز) {
                    dest[dest_لمبائی]=src[j];
                }
                dest_لمبائی++;
                j++;
            }
        } جب(src_لمبائی>0);


        اگر(dest_سائز<dest_لمبائی) {
            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        }
        واپس dest_لمبائی;
    }
    } /* end of switch */
    واپس dest_سائز;
}

U_CAPI int32_t U_EXPORT2
ubidi_write_الٹ(ثابت char16_t *src, int32_t src_لمبائی,
                   char16_t *dest, int32_t dest_سائز,
                   uint16_t options,
                   UError_رمز *p_Error_رمز) {
    int32_t dest_لمبائی;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* more error checking */
    اگر( src==خالی_اشاریہ || src_لمبائی<-1 ||
        dest_سائز<0 || (dest_سائز>0 && dest==خالی_اشاریہ))
    {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* do input and output overlap? */
    اگر( dest!=خالی_اشاریہ &&
        ((src>=dest && src<dest+dest_سائز) ||
         (dest>=src && dest<src+src_لمبائی)))
    {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    اگر(src_لمبائی==-1) {
        src_لمبائی=u_strlen(src);
    }
    اگر(src_لمبائی>0) {
        dest_لمبائی=do_Write_الٹ(src, src_لمبائی, dest, dest_سائز, options, p_Error_رمز);
    } ورنہ {
        /* nothing to do */
        dest_لمبائی=0;
    }

    واپس u_terminateUChars(dest, dest_سائز, dest_لمبائی, p_Error_رمز);
}

// Ticket 20907 - The optimizer in MSVC/Visual Studio versions below 16.4 has trouble with this
// function on Windows ARM64. As a work-around, we disable optimizations for this function.
// This work-around could/should be removed once the following versions of Visual Studio are no
// longer supported: All versions of VS2017, and versions of VS2019 below 16.4.
#اگر (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ہدایت optimize( "", off )
#اگر_ختم
U_CAPI int32_t U_EXPORT2
ubidi_writeReordered(UBiDi *pBiDi,
                     char16_t *dest, int32_t dest_سائز,
                     uint16_t options,
                     UError_رمز *p_Error_رمز) {
    ثابت char16_t *متن;
    char16_t *saveDest;
    int32_t length, dest_استعداد;
    int32_t تسلسل_2, تسلسل_گنتی, logical_آغاز, تسلسل_لمبائی;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* more error checking */
    اگر( pBiDi==خالی_اشاریہ ||
        (متن=pBiDi->متن)==خالی_اشاریہ || (length=pBiDi->length)<0 ||
        dest_سائز<0 || (dest_سائز>0 && dest==خالی_اشاریہ))
    {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* do input and output overlap? */
    اگر( dest!=خالی_اشاریہ &&
        ((متن>=dest && متن<dest+dest_سائز) ||
         (dest>=متن && dest<متن+pBiDi->original_لمبائی)))
    {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    اگر(length==0) {
        /* nothing to do */
        واپس u_terminateUChars(dest, dest_سائز, 0, p_Error_رمز);
    }

    تسلسل_گنتی=ubidi_گنتی_Runs(pBiDi, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* destSize shrinks, later destination length=destCapacity-destSize */
    saveDest=dest;
    dest_استعداد=dest_سائز;
    تسلسل_لمبائی = 0;

    /*
     * Option "insert marks" implies UBIDI_INSERT_LRM_FOR_NUMERIC if the
     * reordering mode (checked below) is appropriate.
     */
    اگر(pBiDi->reorderingOptions & UBIDI_OPTION_داخل_کرو_MARKS) {
        options|=UBIDI_داخل_کرو_LRM_FOR_NUMERIC;
        options&=~UBIDI_REMOVE_BIDI_CONTROLS;
    }
    /*
     * Option "remove controls" implies UBIDI_REMOVE_BIDI_CONTROLS
     * and cancels UBIDI_INSERT_LRM_FOR_NUMERIC.
     */
    اگر(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        options|=UBIDI_REMOVE_BIDI_CONTROLS;
        options&=~UBIDI_داخل_کرو_LRM_FOR_NUMERIC;
    }
    /*
     * If we do not perform the "inverse BiDi" algorithm, then we
     * don't need to insert any LRMs, and don't need to test for it.
     */
    اگر((pBiDi->reordering_منوال != UBIDI_REORDER_INVERSE_NUMBERS_AS_L) &&
       (pBiDi->reordering_منوال != UBIDI_REORDER_INVERSE_LIKE_براہ_راست)  &&
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
    اگر(!(options&UBIDI_OUTPUT_الٹ)) {
        /* forward output */
        اگر(!(options&UBIDI_داخل_کرو_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            برائے(تسلسل_2=0; تسلسل_2<تسلسل_گنتی; ++تسلسل_2) {
                اگر(UBIDI_LTR==ubidi_get_Visual_تسلسل(pBiDi, تسلسل_2, &logical_آغاز, &تسلسل_لمبائی)) {
                    تسلسل_لمبائی=doWriteForward(متن+logical_آغاز, تسلسل_لمبائی,
                                             dest, dest_سائز,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_رمز);
                } ورنہ {
                    تسلسل_لمبائی=do_Write_الٹ(متن+logical_آغاز, تسلسل_لمبائی,
                                             dest, dest_سائز,
                                             options, p_Error_رمز);
                }
                اگر(dest!=خالی_اشاریہ) {
                  dest+=تسلسل_لمبائی;
                }
                dest_سائز-=تسلسل_لمبائی;
            }
        } ورنہ {
            /* insert BiDi controls for "inverse BiDi" */
            ثابت DirProp *dirProps=pBiDi->dirProps;
            ثابت char16_t *src;
            char16_t uc;
            UBiDiDirection dir;
            int32_t markFlag;

            برائے(تسلسل_2=0; تسلسل_2<تسلسل_گنتی; ++تسلسل_2) {
                dir=ubidi_get_Visual_تسلسل(pBiDi, تسلسل_2, &logical_آغاز, &تسلسل_لمبائی);
                src=متن+logical_آغاز;
                /* check if something relevant in insertPoints */
                markFlag=pBiDi->runs[تسلسل_2].داخل_کرو_Remove;
                اگر(markFlag<0) {        /* BiDi controls count */
                    markFlag=0;
                }

                اگر(UBIDI_LTR==dir) {
                    اگر((pBiDi->isInverse) &&
                       (/*run>0 &&*/ dirProps[logical_آغاز]!=L)) {
                        markFlag |= LRM_BEFORE;
                    }
                    اگر (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    ورنہ اگر (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    ورنہ  uc=0;
                    اگر(uc) {
                        اگر(dest_سائز>0) {
                            *dest++=uc;
                        }
                        --dest_سائز;
                    }

                    تسلسل_لمبائی=doWriteForward(src, تسلسل_لمبائی,
                                             dest, dest_سائز,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_رمز);
                    اگر(dest!=خالی_اشاریہ) {
                      dest+=تسلسل_لمبائی;
                    }
                    dest_سائز-=تسلسل_لمبائی;

                    اگر((pBiDi->isInverse) &&
                           (/*run<runCount-1 &&*/
                            تسلسل_لمبائی > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            dirProps[logical_آغاز+تسلسل_لمبائی-1]!=L)) {
                        markFlag |= LRM_AFTER;
                    }
                    اگر (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    ورنہ اگر (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    ورنہ  uc=0;
                    اگر(uc) {
                        اگر(dest_سائز>0) {
                            *dest++=uc;
                        }
                        --dest_سائز;
                    }
                } ورنہ {                /* RTL run */
                    اگر((pBiDi->isInverse) &&
                       (/*run>0 &&*/ !(نقاب_R_AL&DIRPROP_FLAG(dirProps[logical_آغاز+تسلسل_لمبائی-1])))) {
                        markFlag |= RLM_BEFORE;
                    }
                    اگر (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    ورنہ اگر (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    ورنہ  uc=0;
                    اگر(uc) {
                        اگر(dest_سائز>0) {
                            *dest++=uc;
                        }
                        --dest_سائز;
                    }

                    تسلسل_لمبائی=do_Write_الٹ(src, تسلسل_لمبائی,
                                             dest, dest_سائز,
                                             options, p_Error_رمز);
                    اگر(dest!=خالی_اشاریہ) {
                      dest+=تسلسل_لمبائی;
                    }
                    dest_سائز-=تسلسل_لمبائی;

                    اگر((pBiDi->isInverse) &&
                       (/*run<runCount-1 &&*/ !(نقاب_R_AL&DIRPROP_FLAG(dirProps[logical_آغاز])))) {
                        markFlag |= RLM_AFTER;
                    }
                    اگر (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    ورنہ اگر (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    ورنہ  uc=0;
                    اگر(uc) {
                        اگر(dest_سائز>0) {
                            *dest++=uc;
                        }
                        --dest_سائز;
                    }
                }
            }
        }
    } ورنہ {
        /* reverse output */
        اگر(!(options&UBIDI_داخل_کرو_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            برائے(تسلسل_2=تسلسل_گنتی; --تسلسل_2>=0;) {
                اگر(UBIDI_LTR==ubidi_get_Visual_تسلسل(pBiDi, تسلسل_2, &logical_آغاز, &تسلسل_لمبائی)) {
                    تسلسل_لمبائی=do_Write_الٹ(متن+logical_آغاز, تسلسل_لمبائی,
                                             dest, dest_سائز,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_رمز);
                } ورنہ {
                    تسلسل_لمبائی=doWriteForward(متن+logical_آغاز, تسلسل_لمبائی,
                                             dest, dest_سائز,
                                             options, p_Error_رمز);
                }
                اگر(dest!=خالی_اشاریہ) {
                  dest+=تسلسل_لمبائی;
                }
                dest_سائز-=تسلسل_لمبائی;
            }
        } ورنہ {
            /* insert BiDi controls for "inverse BiDi" */
            ثابت DirProp *dirProps=pBiDi->dirProps;
            ثابت char16_t *src;
            UBiDiDirection dir;

            برائے(تسلسل_2=تسلسل_گنتی; --تسلسل_2>=0;) {
                /* reverse output */
                dir=ubidi_get_Visual_تسلسل(pBiDi, تسلسل_2, &logical_آغاز, &تسلسل_لمبائی);
                src=متن+logical_آغاز;

                اگر(UBIDI_LTR==dir) {
                    اگر(/*run<runCount-1 &&*/ dirProps[logical_آغاز+تسلسل_لمبائی-1]!=L) {
                        اگر(dest_سائز>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_سائز;
                    }

                    تسلسل_لمبائی=do_Write_الٹ(src, تسلسل_لمبائی,
                                             dest, dest_سائز,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_رمز);
                    اگر(dest!=خالی_اشاریہ) {
                      dest+=تسلسل_لمبائی;
                    }
                    dest_سائز-=تسلسل_لمبائی;

                    اگر(/*run>0 &&*/ dirProps[logical_آغاز]!=L) {
                        اگر(dest_سائز>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_سائز;
                    }
                } ورنہ {
                    اگر(/*run<runCount-1 &&*/ !(نقاب_R_AL&DIRPROP_FLAG(dirProps[logical_آغاز]))) {
                        اگر(dest_سائز>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_سائز;
                    }

                    int32_t original_تسلسل_لمبائی = تسلسل_لمبائی;
                    تسلسل_لمبائی=doWriteForward(src, تسلسل_لمبائی,
                                             dest, dest_سائز,
                                             options, p_Error_رمز);
                    اگر(dest!=خالی_اشاریہ) {
                      dest+=تسلسل_لمبائی;
                    }
                    dest_سائز-=تسلسل_لمبائی;

                    اگر(/*run>0 &&*/
                            تسلسل_لمبائی > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            !(نقاب_R_AL&DIRPROP_FLAG(dirProps[logical_آغاز+original_تسلسل_لمبائی-1]))) {
                        اگر(dest_سائز>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_سائز;
                    }
                }
            }
        }
    }

    واپس u_terminateUChars(saveDest, dest_استعداد, dest_استعداد-dest_سائز, p_Error_رمز);
}
#اگر (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ہدایت optimize( "", on )
#اگر_ختم
