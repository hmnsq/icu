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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubidi.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "ubidiimp.h"

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
#ಆದರೆ defined(UTF_ಗಾತ್ರ) && UTF_ಗಾತ್ರ==8
# error reimplement ubidi_writeReordered() ಗಾಗಿ UTF-8, see comment above
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು IS_COMBINING(type) ((1UL<<(type))&(1UL<<U_NON_SPACING_MARK|1UL<<U_COMBINING_SPACING_MARK|1UL<<U_ENCLOSING_MARK))

/*
 * When we have UBIDI_OUTPUT_REVERSE set on ubidi_writeReordered(), then we
 * semantically write RTL runs in reverse and later reverse them again.
 * Instead, we actually write them in forward order to begin with.
 * However, if the RTL run was to be mirrored, we need to mirror here now
 * since the implicit second reversal must not do it.
 * It looks strange to do mirroring in LTR output, but it is only because
 * we are writing RTL output in reverse.
 */
ಸ್ಥಿರ int32_t
doWriteForward(ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
               char16_t *dest, int32_t dest_ಗಾತ್ರ,
               uint16_t options,
               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (src_ಉದ್ದಳತೆ<=0) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    /* optimize for several combinations of options */
    ಆಯ್ಕೆ(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING)) {
    ಸ್ಥಿತಿ 0: {
        /* simply copy the LTR run to the destination */
        int32_t length=src_ಉದ್ದಳತೆ;
        ಆದರೆ(dest_ಗಾತ್ರ<length) {
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ src_ಉದ್ದಳತೆ;
        }
        ಮಾಡು {
            *dest++=*src++;
        } ಆಗ(--length>0);
        ಹಿಂತಿರುಗಿ src_ಉದ್ದಳತೆ;
    }
    ಸ್ಥಿತಿ UBIDI_DO_MIRRORING: {
        /* do mirroring */
        int32_t i=0, j=0;
        UChar32 c;
        ಮಾಡು {
            U16_ಮುಂದಿನ(src, i, src_ಉದ್ದಳತೆ, c);
            c=u_charMirror(c);
            int32_t charLen = U16_ಉದ್ದಳತೆ(c);
            ಆದರೆ(j + charLen <= dest_ಗಾತ್ರ) {
                int32_t ತಾತ್ಕಾಲಿಕ = j;
                U16_APPEND_UNSAFE(dest, ತಾತ್ಕಾಲಿಕ, c);
            }
            j += charLen;
        } ಆಗ(i<src_ಉದ್ದಳತೆ);
        ಆದರೆ(dest_ಗಾತ್ರ < j) {
            *p_Error_ಸಂಕೇತ = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        }
        ಆದರೆ(j > src_ಉದ್ದಳತೆ) {
            ಹಿಂತಿರುಗಿ j;
        }
        /* If mirroring caused code unit contraction (e.g., U+1DB10 contracting to U+221D),
         * pad trailing array slots up to srcLength to avoid leaving uninitialized memory.
         * If j > 0, repeat the preceding code unit; if j == 0, fallback to 0x0020 (ASCII space).
         */
        ಆಗ(j < src_ಉದ್ದಳತೆ && j < dest_ಗಾತ್ರ) {
            dest[j] = j > 0 ? dest[j - 1] : 0x0020;
            j++;
        }
        ಹಿಂತಿರುಗಿ src_ಉದ್ದಳತೆ;
    }
    ಸ್ಥಿತಿ UBIDI_REMOVE_BIDI_CONTROLS: {
        /* copy the LTR run and remove any BiDi control characters */
        int32_t ಉಳಿದ=dest_ಗಾತ್ರ;
        char16_t c;
        ಮಾಡು {
            c=*src++;
            ಆದರೆ(!IS_BIDI_CONTROL_CHAR(c)) {
                ಆದರೆ(--ಉಳಿದ<0) {
                    *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;

                    /* preflight the length */
                    ಆಗ(--src_ಉದ್ದಳತೆ>0) {
                        c=*src++;
                        ಆದರೆ(!IS_BIDI_CONTROL_CHAR(c)) {
                            --ಉಳಿದ;
                        }
                    }
                    ಹಿಂತಿರುಗಿ dest_ಗಾತ್ರ-ಉಳಿದ;
                }
                *dest++=c;
            }
        } ಆಗ(--src_ಉದ್ದಳತೆ>0);
        ಹಿಂತಿರುಗಿ dest_ಗಾತ್ರ-ಉಳಿದ;
    }
    ಪೂರ್ವನಿಯೋಜಿತ: {
        /* remove BiDi control characters and do mirroring */
        int32_t i, j=0;
        UChar32 c;
        ಮಾಡು {
            i=0;
            U16_ಮುಂದಿನ(src, i, src_ಉದ್ದಳತೆ, c);
            src+=i;
            src_ಉದ್ದಳತೆ-=i;
            ಆದರೆ(!IS_BIDI_CONTROL_CHAR(c)) {
                c=u_charMirror(c);
                int32_t charLen = U16_ಉದ್ದಳತೆ(c);
                ಆದರೆ(j + charLen <= dest_ಗಾತ್ರ) {
                    int32_t ತಾತ್ಕಾಲಿಕ = j;
                    U16_APPEND_UNSAFE(dest, ತಾತ್ಕಾಲಿಕ, c);
                }
                j += charLen;
            }
        } ಆಗ(src_ಉದ್ದಳತೆ>0);
        ಆದರೆ(dest_ಗಾತ್ರ < j) {
            *p_Error_ಸಂಕೇತ = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        }
        ಹಿಂತಿರುಗಿ j;
    }
    } /* end of switch */
}

ಸ್ಥಿರ int32_t
do_Write_ಹಿಮ್ಮುಖ(ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
               char16_t *dest, int32_t dest_ಗಾತ್ರ,
               uint16_t options,
               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
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

    ಆದರೆ(src_ಉದ್ದಳತೆ<=0) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* optimize for several combinations of options */
    ಆಯ್ಕೆ(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING|UBIDI_KEEP_ತಳ_COMBINING)) {
    ಸ್ಥಿತಿ 0:
        /*
         * With none of the "complicated" options set, the destination
         * run will have the same length as the source run,
         * and there is no mirroring and no keeping combining characters
         * with their base characters.
         */
        ಆದರೆ(dest_ಗಾತ್ರ<src_ಉದ್ದಳತೆ) {
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ src_ಉದ್ದಳತೆ;
        }
        dest_ಗಾತ್ರ=src_ಉದ್ದಳತೆ;

        /* preserve character integrity */
        ಮಾಡು {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_ಉದ್ದಳತೆ;

            /* collect code units for one base character */
            U16_PREV(src, 0, src_ಉದ್ದಳತೆ, c);

            /* copy this base character */
            j=src_ಉದ್ದಳತೆ;
            ಮಾಡು {
                *dest++=src[j++];
            } ಆಗ(j<i);
        } ಆಗ(src_ಉದ್ದಳತೆ>0);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UBIDI_KEEP_ತಳ_COMBINING:
        /*
         * Here, too, the destination
         * run will have the same length as the source run,
         * and there is no mirroring.
         * We do need to keep combining characters with their base characters.
         */
        ಆದರೆ(dest_ಗಾತ್ರ<src_ಉದ್ದಳತೆ) {
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ src_ಉದ್ದಳತೆ;
        }
        dest_ಗಾತ್ರ=src_ಉದ್ದಳತೆ;

        /* preserve character integrity */
        ಮಾಡು {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_ಉದ್ದಳತೆ;

            /* collect code units and modifier letters for one base character */
            ಮಾಡು {
                U16_PREV(src, 0, src_ಉದ್ದಳತೆ, c);
            } ಆಗ(src_ಉದ್ದಳತೆ>0 && IS_COMBINING(u_charType(c)));

            /* copy this "user character" */
            j=src_ಉದ್ದಳತೆ;
            ಮಾಡು {
                *dest++=src[j++];
            } ಆಗ(j<i);
        } ಆಗ(src_ಉದ್ದಳತೆ>0);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ: {
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
        ಆದರೆ(options&UBIDI_ಆಂತರಿಕ_RUNS_ONLY) {
            ಆದರೆ(!(options&UBIDI_REMOVE_BIDI_CONTROLS)) {
                i=src_ಉದ್ದಳತೆ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                int32_t length=src_ಉದ್ದಳತೆ;
                char16_t ch;
                i=0;
                /* Loop over all code units in the source run to count those that are not Bidi control characters,
                 * determining the required destination buffer length for preflighting when UBIDI_REMOVE_BIDI_CONTROLS is set.
                 */
                ಮಾಡು {
                    ch=*src++;
                    ಆದರೆ(!IS_BIDI_CONTROL_CHAR(ch)) {
                        ++i;
                    }
                } ಆಗ(--length>0);
                src-=src_ಉದ್ದಳತೆ;
            }
            ಆದರೆ(dest_ಗಾತ್ರ<i) {
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ಹಿಂತಿರುಗಿ i;
            }
            char16_t *saveDest=dest;
            int32_t dest_ಉದ್ದಳತೆ=0;
            /* Loop backward across the source run segment by segment, using legacy 1-to-1 code unit stepping
             * to extract and reverse character clusters for internal setPara classification.
             */
            ಮಾಡು {
                i=src_ಉದ್ದಳತೆ;
                U16_PREV(src, 0, src_ಉದ್ದಳತೆ, c);
                ಆದರೆ(options&UBIDI_KEEP_ತಳ_COMBINING) {
                    /* Loop backward over any combining marks attached to the base character so that the combining
                     * sequence remains attached after its base character when reversed.
                     */
                    ಆಗ(src_ಉದ್ದಳತೆ>0 && IS_COMBINING(u_charType(c))) {
                        U16_PREV(src, 0, src_ಉದ್ದಳತೆ, c);
                    }
                }
                ಆದರೆ(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                    ಮುಂದುವರಿ;
                }
                int32_t orig_ತಳ_Len=U16_ಉದ್ದಳತೆ(c);
                j=src_ಉದ್ದಳತೆ;
                ಆದರೆ(options&UBIDI_DO_MIRRORING) {
                    c=u_charMirror(c);
                    int32_t k=U16_ಉದ್ದಳತೆ(c);
                    ಆದರೆ(dest_ಉದ್ದಳತೆ+k<=dest_ಗಾತ್ರ) {
                        int32_t ತಾತ್ಕಾಲಿಕ=dest_ಉದ್ದಳತೆ;
                        U16_APPEND_UNSAFE(saveDest, ತಾತ್ಕಾಲಿಕ, c);
                    }
                    dest_ಉದ್ದಳತೆ+=k;
                    j+=orig_ತಳ_Len;
                }
                /* Loop forward from the current segment index to copy any unmirrored code units (such as combining marks
                 * or surrogate halves) into dest in forward visual order.
                 */
                ಆಗ(j<i) {
                    ಆದರೆ(dest_ಉದ್ದಳತೆ<dest_ಗಾತ್ರ) {
                        saveDest[dest_ಉದ್ದಳತೆ]=src[j];
                    }
                    dest_ಉದ್ದಳತೆ++;
                    j++;
                }
            } ಆಗ(src_ಉದ್ದಳತೆ>0);
            ಆದರೆ(dest_ಗಾತ್ರ<dest_ಉದ್ದಳತೆ) {
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            }
            ಹಿಂತಿರುಗಿ dest_ಉದ್ದಳತೆ;
        }
        int32_t dest_ಉದ್ದಳತೆ=0;
        ಮಾಡು {
            i=src_ಉದ್ದಳತೆ;
            U16_PREV(src, 0, src_ಉದ್ದಳತೆ, c);
            ಆದರೆ(options&UBIDI_KEEP_ತಳ_COMBINING) {
                ಆಗ(src_ಉದ್ದಳತೆ>0 && IS_COMBINING(u_charType(c))) {
                    U16_PREV(src, 0, src_ಉದ್ದಳತೆ, c);
                }
            }
            ಆದರೆ(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                ಮುಂದುವರಿ;
            }

            int32_t orig_ತಳ_Len=U16_ಉದ್ದಳತೆ(c);
            j=src_ಉದ್ದಳತೆ;
            ಆದರೆ(options&UBIDI_DO_MIRRORING) {
                /* mirror only the base character */
                c=u_charMirror(c);
                int32_t k=U16_ಉದ್ದಳತೆ(c);
                ಆದರೆ(dest_ಉದ್ದಳತೆ+k<=dest_ಗಾತ್ರ) {
                    int32_t ತಾತ್ಕಾಲಿಕ=dest_ಉದ್ದಳತೆ;
                    U16_APPEND_UNSAFE(dest, ತಾತ್ಕಾಲಿಕ, c);
                }
                dest_ಉದ್ದಳತೆ+=k;
                j+=orig_ತಳ_Len;
            }
            ಆಗ(j<i) {
                ಆದರೆ(dest_ಉದ್ದಳತೆ<dest_ಗಾತ್ರ) {
                    dest[dest_ಉದ್ದಳತೆ]=src[j];
                }
                dest_ಉದ್ದಳತೆ++;
                j++;
            }
        } ಆಗ(src_ಉದ್ದಳತೆ>0);


        ಆದರೆ(dest_ಗಾತ್ರ<dest_ಉದ್ದಳತೆ) {
            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        }
        ಹಿಂತಿರುಗಿ dest_ಉದ್ದಳತೆ;
    }
    } /* end of switch */
    ಹಿಂತಿರುಗಿ dest_ಗಾತ್ರ;
}

U_CAPI int32_t U_EXPORT2
ubidi_write_ಹಿಮ್ಮುಖ(ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                   char16_t *dest, int32_t dest_ಗಾತ್ರ,
                   uint16_t options,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    int32_t dest_ಉದ್ದಳತೆ;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* more error checking */
    ಆದರೆ( src==ಶೂನ್ಯಸೂಚಕ || src_ಉದ್ದಳತೆ<-1 ||
        dest_ಗಾತ್ರ<0 || (dest_ಗಾತ್ರ>0 && dest==ಶೂನ್ಯಸೂಚಕ))
    {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* do input and output overlap? */
    ಆದರೆ( dest!=ಶೂನ್ಯಸೂಚಕ &&
        ((src>=dest && src<dest+dest_ಗಾತ್ರ) ||
         (dest>=src && dest<src+src_ಉದ್ದಳತೆ)))
    {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(src_ಉದ್ದಳತೆ==-1) {
        src_ಉದ್ದಳತೆ=u_strlen(src);
    }
    ಆದರೆ(src_ಉದ್ದಳತೆ>0) {
        dest_ಉದ್ದಳತೆ=do_Write_ಹಿಮ್ಮುಖ(src, src_ಉದ್ದಳತೆ, dest, dest_ಗಾತ್ರ, options, p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* nothing to do */
        dest_ಉದ್ದಳತೆ=0;
    }

    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಗಾತ್ರ, dest_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
}

// Ticket 20907 - The optimizer in MSVC/Visual Studio versions below 16.4 has trouble with this
// function on Windows ARM64. As a work-around, we disable optimizations for this function.
// This work-around could/should be removed once the following versions of Visual Studio are no
// longer supported: All versions of VS2017, and versions of VS2019 below 16.4.
#ಆದರೆ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ಸೂಚನೆ optimize( "", off )
#ಆದರೆ_ಅಂತ್ಯ
U_CAPI int32_t U_EXPORT2
ubidi_writeReordered(UBiDi *pBiDi,
                     char16_t *dest, int32_t dest_ಗಾತ್ರ,
                     uint16_t options,
                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ;
    char16_t *saveDest;
    int32_t length, dest_ಸಾಮರ್ಥ್ಯ;
    int32_t ಸತತ_2, ಸತತ_ಎಣಿಕೆ, logical_ಆರಂಭ, ಸತತ_ಉದ್ದಳತೆ;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* more error checking */
    ಆದರೆ( pBiDi==ಶೂನ್ಯಸೂಚಕ ||
        (ಪಠ್ಯ=pBiDi->ಪಠ್ಯ)==ಶೂನ್ಯಸೂಚಕ || (length=pBiDi->length)<0 ||
        dest_ಗಾತ್ರ<0 || (dest_ಗಾತ್ರ>0 && dest==ಶೂನ್ಯಸೂಚಕ))
    {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* do input and output overlap? */
    ಆದರೆ( dest!=ಶೂನ್ಯಸೂಚಕ &&
        ((ಪಠ್ಯ>=dest && ಪಠ್ಯ<dest+dest_ಗಾತ್ರ) ||
         (dest>=ಪಠ್ಯ && dest<ಪಠ್ಯ+pBiDi->original_ಉದ್ದಳತೆ)))
    {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(length==0) {
        /* nothing to do */
        ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಗಾತ್ರ, 0, p_Error_ಸಂಕೇತ);
    }

    ಸತತ_ಎಣಿಕೆ=ubidi_ಎಣಿಕೆ_Runs(pBiDi, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* destSize shrinks, later destination length=destCapacity-destSize */
    saveDest=dest;
    dest_ಸಾಮರ್ಥ್ಯ=dest_ಗಾತ್ರ;
    ಸತತ_ಉದ್ದಳತೆ = 0;

    /*
     * Option "insert marks" implies UBIDI_INSERT_LRM_FOR_NUMERIC if the
     * reordering mode (checked below) is appropriate.
     */
    ಆದರೆ(pBiDi->reorderingOptions & UBIDI_OPTION_ಸೇರಿಸು_MARKS) {
        options|=UBIDI_ಸೇರಿಸು_LRM_FOR_NUMERIC;
        options&=~UBIDI_REMOVE_BIDI_CONTROLS;
    }
    /*
     * Option "remove controls" implies UBIDI_REMOVE_BIDI_CONTROLS
     * and cancels UBIDI_INSERT_LRM_FOR_NUMERIC.
     */
    ಆದರೆ(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        options|=UBIDI_REMOVE_BIDI_CONTROLS;
        options&=~UBIDI_ಸೇರಿಸು_LRM_FOR_NUMERIC;
    }
    /*
     * If we do not perform the "inverse BiDi" algorithm, then we
     * don't need to insert any LRMs, and don't need to test for it.
     */
    ಆದರೆ((pBiDi->reordering_ಬಹುಲಕ != UBIDI_REORDER_INVERSE_NUMBERS_AS_L) &&
       (pBiDi->reordering_ಬಹುಲಕ != UBIDI_REORDER_INVERSE_LIKE_ನೇರ)  &&
       (pBiDi->reordering_ಬಹುಲಕ != UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL) &&
       (pBiDi->reordering_ಬಹುಲಕ != UBIDI_REORDER_RUNS_ONLY)) {
        options&=~UBIDI_ಸೇರಿಸು_LRM_FOR_NUMERIC;
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
    ಆದರೆ(!(options&UBIDI_OUTPUT_ಹಿಮ್ಮುಖ)) {
        /* forward output */
        ಆದರೆ(!(options&UBIDI_ಸೇರಿಸು_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            ಗಾಗಿ(ಸತತ_2=0; ಸತತ_2<ಸತತ_ಎಣಿಕೆ; ++ಸತತ_2) {
                ಆದರೆ(UBIDI_LTR==ubidi_get_Visual_ಸತತ(pBiDi, ಸತತ_2, &logical_ಆರಂಭ, &ಸತತ_ಉದ್ದಳತೆ)) {
                    ಸತತ_ಉದ್ದಳತೆ=doWriteForward(ಪಠ್ಯ+logical_ಆರಂಭ, ಸತತ_ಉದ್ದಳತೆ,
                                             dest, dest_ಗಾತ್ರ,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_ಸಂಕೇತ);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಸತತ_ಉದ್ದಳತೆ=do_Write_ಹಿಮ್ಮುಖ(ಪಠ್ಯ+logical_ಆರಂಭ, ಸತತ_ಉದ್ದಳತೆ,
                                             dest, dest_ಗಾತ್ರ,
                                             options, p_Error_ಸಂಕೇತ);
                }
                ಆದರೆ(dest!=ಶೂನ್ಯಸೂಚಕ) {
                  dest+=ಸತತ_ಉದ್ದಳತೆ;
                }
                dest_ಗಾತ್ರ-=ಸತತ_ಉದ್ದಳತೆ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* insert BiDi controls for "inverse BiDi" */
            ಸ್ಥಿರಾಂಕ DirProp *dirProps=pBiDi->dirProps;
            ಸ್ಥಿರಾಂಕ char16_t *src;
            char16_t uc;
            UBiDiDirection dir;
            int32_t markFlag;

            ಗಾಗಿ(ಸತತ_2=0; ಸತತ_2<ಸತತ_ಎಣಿಕೆ; ++ಸತತ_2) {
                dir=ubidi_get_Visual_ಸತತ(pBiDi, ಸತತ_2, &logical_ಆರಂಭ, &ಸತತ_ಉದ್ದಳತೆ);
                src=ಪಠ್ಯ+logical_ಆರಂಭ;
                /* check if something relevant in insertPoints */
                markFlag=pBiDi->runs[ಸತತ_2].ಸೇರಿಸು_Remove;
                ಆದರೆ(markFlag<0) {        /* BiDi controls count */
                    markFlag=0;
                }

                ಆದರೆ(UBIDI_LTR==dir) {
                    ಆದರೆ((pBiDi->isInverse) &&
                       (/*run>0 &&*/ dirProps[logical_ಆರಂಭ]!=L)) {
                        markFlag |= LRM_BEFORE;
                    }
                    ಆದರೆ (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ  uc=0;
                    ಆದರೆ(uc) {
                        ಆದರೆ(dest_ಗಾತ್ರ>0) {
                            *dest++=uc;
                        }
                        --dest_ಗಾತ್ರ;
                    }

                    ಸತತ_ಉದ್ದಳತೆ=doWriteForward(src, ಸತತ_ಉದ್ದಳತೆ,
                                             dest, dest_ಗಾತ್ರ,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_ಸಂಕೇತ);
                    ಆದರೆ(dest!=ಶೂನ್ಯಸೂಚಕ) {
                      dest+=ಸತತ_ಉದ್ದಳತೆ;
                    }
                    dest_ಗಾತ್ರ-=ಸತತ_ಉದ್ದಳತೆ;

                    ಆದರೆ((pBiDi->isInverse) &&
                           (/*run<runCount-1 &&*/
                            ಸತತ_ಉದ್ದಳತೆ > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            dirProps[logical_ಆರಂಭ+ಸತತ_ಉದ್ದಳತೆ-1]!=L)) {
                        markFlag |= LRM_AFTER;
                    }
                    ಆದರೆ (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ  uc=0;
                    ಆದರೆ(uc) {
                        ಆದರೆ(dest_ಗಾತ್ರ>0) {
                            *dest++=uc;
                        }
                        --dest_ಗಾತ್ರ;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {                /* RTL run */
                    ಆದರೆ((pBiDi->isInverse) &&
                       (/*run>0 &&*/ !(ಮುಖವಾಡ_R_AL&DIRPROP_FLAG(dirProps[logical_ಆರಂಭ+ಸತತ_ಉದ್ದಳತೆ-1])))) {
                        markFlag |= RLM_BEFORE;
                    }
                    ಆದರೆ (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ  uc=0;
                    ಆದರೆ(uc) {
                        ಆದರೆ(dest_ಗಾತ್ರ>0) {
                            *dest++=uc;
                        }
                        --dest_ಗಾತ್ರ;
                    }

                    ಸತತ_ಉದ್ದಳತೆ=do_Write_ಹಿಮ್ಮುಖ(src, ಸತತ_ಉದ್ದಳತೆ,
                                             dest, dest_ಗಾತ್ರ,
                                             options, p_Error_ಸಂಕೇತ);
                    ಆದರೆ(dest!=ಶೂನ್ಯಸೂಚಕ) {
                      dest+=ಸತತ_ಉದ್ದಳತೆ;
                    }
                    dest_ಗಾತ್ರ-=ಸತತ_ಉದ್ದಳತೆ;

                    ಆದರೆ((pBiDi->isInverse) &&
                       (/*run<runCount-1 &&*/ !(ಮುಖವಾಡ_R_AL&DIRPROP_FLAG(dirProps[logical_ಆರಂಭ])))) {
                        markFlag |= RLM_AFTER;
                    }
                    ಆದರೆ (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ  uc=0;
                    ಆದರೆ(uc) {
                        ಆದರೆ(dest_ಗಾತ್ರ>0) {
                            *dest++=uc;
                        }
                        --dest_ಗಾತ್ರ;
                    }
                }
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* reverse output */
        ಆದರೆ(!(options&UBIDI_ಸೇರಿಸು_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            ಗಾಗಿ(ಸತತ_2=ಸತತ_ಎಣಿಕೆ; --ಸತತ_2>=0;) {
                ಆದರೆ(UBIDI_LTR==ubidi_get_Visual_ಸತತ(pBiDi, ಸತತ_2, &logical_ಆರಂಭ, &ಸತತ_ಉದ್ದಳತೆ)) {
                    ಸತತ_ಉದ್ದಳತೆ=do_Write_ಹಿಮ್ಮುಖ(ಪಠ್ಯ+logical_ಆರಂಭ, ಸತತ_ಉದ್ದಳತೆ,
                                             dest, dest_ಗಾತ್ರ,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_ಸಂಕೇತ);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಸತತ_ಉದ್ದಳತೆ=doWriteForward(ಪಠ್ಯ+logical_ಆರಂಭ, ಸತತ_ಉದ್ದಳತೆ,
                                             dest, dest_ಗಾತ್ರ,
                                             options, p_Error_ಸಂಕೇತ);
                }
                ಆದರೆ(dest!=ಶೂನ್ಯಸೂಚಕ) {
                  dest+=ಸತತ_ಉದ್ದಳತೆ;
                }
                dest_ಗಾತ್ರ-=ಸತತ_ಉದ್ದಳತೆ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* insert BiDi controls for "inverse BiDi" */
            ಸ್ಥಿರಾಂಕ DirProp *dirProps=pBiDi->dirProps;
            ಸ್ಥಿರಾಂಕ char16_t *src;
            UBiDiDirection dir;

            ಗಾಗಿ(ಸತತ_2=ಸತತ_ಎಣಿಕೆ; --ಸತತ_2>=0;) {
                /* reverse output */
                dir=ubidi_get_Visual_ಸತತ(pBiDi, ಸತತ_2, &logical_ಆರಂಭ, &ಸತತ_ಉದ್ದಳತೆ);
                src=ಪಠ್ಯ+logical_ಆರಂಭ;

                ಆದರೆ(UBIDI_LTR==dir) {
                    ಆದರೆ(/*run<runCount-1 &&*/ dirProps[logical_ಆರಂಭ+ಸತತ_ಉದ್ದಳತೆ-1]!=L) {
                        ಆದರೆ(dest_ಗಾತ್ರ>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_ಗಾತ್ರ;
                    }

                    ಸತತ_ಉದ್ದಳತೆ=do_Write_ಹಿಮ್ಮುಖ(src, ಸತತ_ಉದ್ದಳತೆ,
                                             dest, dest_ಗಾತ್ರ,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_ಸಂಕೇತ);
                    ಆದರೆ(dest!=ಶೂನ್ಯಸೂಚಕ) {
                      dest+=ಸತತ_ಉದ್ದಳತೆ;
                    }
                    dest_ಗಾತ್ರ-=ಸತತ_ಉದ್ದಳತೆ;

                    ಆದರೆ(/*run>0 &&*/ dirProps[logical_ಆರಂಭ]!=L) {
                        ಆದರೆ(dest_ಗಾತ್ರ>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_ಗಾತ್ರ;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ(/*run<runCount-1 &&*/ !(ಮುಖವಾಡ_R_AL&DIRPROP_FLAG(dirProps[logical_ಆರಂಭ]))) {
                        ಆದರೆ(dest_ಗಾತ್ರ>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_ಗಾತ್ರ;
                    }

                    int32_t original_ಸತತ_ಉದ್ದಳತೆ = ಸತತ_ಉದ್ದಳತೆ;
                    ಸತತ_ಉದ್ದಳತೆ=doWriteForward(src, ಸತತ_ಉದ್ದಳತೆ,
                                             dest, dest_ಗಾತ್ರ,
                                             options, p_Error_ಸಂಕೇತ);
                    ಆದರೆ(dest!=ಶೂನ್ಯಸೂಚಕ) {
                      dest+=ಸತತ_ಉದ್ದಳತೆ;
                    }
                    dest_ಗಾತ್ರ-=ಸತತ_ಉದ್ದಳತೆ;

                    ಆದರೆ(/*run>0 &&*/
                            ಸತತ_ಉದ್ದಳತೆ > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            !(ಮುಖವಾಡ_R_AL&DIRPROP_FLAG(dirProps[logical_ಆರಂಭ+original_ಸತತ_ಉದ್ದಳತೆ-1]))) {
                        ಆದರೆ(dest_ಗಾತ್ರ>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_ಗಾತ್ರ;
                    }
                }
            }
        }
    }

    ಹಿಂತಿರುಗಿ u_terminateUChars(saveDest, dest_ಸಾಮರ್ಥ್ಯ, dest_ಸಾಮರ್ಥ್ಯ-dest_ಗಾತ್ರ, p_Error_ಸಂಕೇತ);
}
#ಆದರೆ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ಸೂಚನೆ optimize( "", on )
#ಆದರೆ_ಅಂತ್ಯ
