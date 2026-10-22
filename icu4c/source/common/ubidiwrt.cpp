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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubidi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubidiimp.h"

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
#ਜੇ defined(UTF_ਮਾਪ) && UTF_ਮਾਪ==8
# error reimplement ubidi_writeReordered() ਲਈ UTF-8, see comment above
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_COMBINING(type) ((1UL<<(type))&(1UL<<U_NON_SPACING_MARK|1UL<<U_COMBINING_SPACING_MARK|1UL<<U_ENCLOSING_MARK))

/*
 * When we have UBIDI_OUTPUT_REVERSE set on ubidi_writeReordered(), then we
 * semantically write RTL runs in reverse and later reverse them again.
 * Instead, we actually write them in forward order to begin with.
 * However, if the RTL run was to be mirrored, we need to mirror here now
 * since the implicit second reversal must not do it.
 * It looks strange to do mirroring in LTR output, but it is only because
 * we are writing RTL output in reverse.
 */
ਸਥਿਰ int32_t
doWriteForward(ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
               char16_t *dest, int32_t dest_ਮਾਪ,
               uint16_t options,
               UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (src_ਲੰਮਾਈ<=0) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    /* optimize for several combinations of options */
    ਚੋਣ(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING)) {
    ਹਾਲਤ 0: {
        /* simply copy the LTR run to the destination */
        int32_t length=src_ਲੰਮਾਈ;
        ਜੇ(dest_ਮਾਪ<length) {
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ src_ਲੰਮਾਈ;
        }
        ਕਰੋ {
            *dest++=*src++;
        } ਜਦੋਂ(--length>0);
        ਵਾਪਸ src_ਲੰਮਾਈ;
    }
    ਹਾਲਤ UBIDI_DO_MIRRORING: {
        /* do mirroring */
        int32_t i=0, j=0;
        UChar32 c;
        ਕਰੋ {
            U16_ਅਗਲਾ(src, i, src_ਲੰਮਾਈ, c);
            c=u_charMirror(c);
            int32_t charLen = U16_ਲੰਮਾਈ(c);
            ਜੇ(j + charLen <= dest_ਮਾਪ) {
                int32_t ਆਰਜ਼ੀ = j;
                U16_APPEND_UNSAFE(dest, ਆਰਜ਼ੀ, c);
            }
            j += charLen;
        } ਜਦੋਂ(i<src_ਲੰਮਾਈ);
        ਜੇ(dest_ਮਾਪ < j) {
            *p_Error_ਕੋਡ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        }
        ਜੇ(j > src_ਲੰਮਾਈ) {
            ਵਾਪਸ j;
        }
        /* If mirroring caused code unit contraction (e.g., U+1DB10 contracting to U+221D),
         * pad trailing array slots up to srcLength to avoid leaving uninitialized memory.
         * If j > 0, repeat the preceding code unit; if j == 0, fallback to 0x0020 (ASCII space).
         */
        ਜਦੋਂ(j < src_ਲੰਮਾਈ && j < dest_ਮਾਪ) {
            dest[j] = j > 0 ? dest[j - 1] : 0x0020;
            j++;
        }
        ਵਾਪਸ src_ਲੰਮਾਈ;
    }
    ਹਾਲਤ UBIDI_REMOVE_BIDI_CONTROLS: {
        /* copy the LTR run and remove any BiDi control characters */
        int32_t ਬਚਿਆ=dest_ਮਾਪ;
        char16_t c;
        ਕਰੋ {
            c=*src++;
            ਜੇ(!IS_BIDI_CONTROL_CHAR(c)) {
                ਜੇ(--ਬਚਿਆ<0) {
                    *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;

                    /* preflight the length */
                    ਜਦੋਂ(--src_ਲੰਮਾਈ>0) {
                        c=*src++;
                        ਜੇ(!IS_BIDI_CONTROL_CHAR(c)) {
                            --ਬਚਿਆ;
                        }
                    }
                    ਵਾਪਸ dest_ਮਾਪ-ਬਚਿਆ;
                }
                *dest++=c;
            }
        } ਜਦੋਂ(--src_ਲੰਮਾਈ>0);
        ਵਾਪਸ dest_ਮਾਪ-ਬਚਿਆ;
    }
    ਮੂਲ: {
        /* remove BiDi control characters and do mirroring */
        int32_t i, j=0;
        UChar32 c;
        ਕਰੋ {
            i=0;
            U16_ਅਗਲਾ(src, i, src_ਲੰਮਾਈ, c);
            src+=i;
            src_ਲੰਮਾਈ-=i;
            ਜੇ(!IS_BIDI_CONTROL_CHAR(c)) {
                c=u_charMirror(c);
                int32_t charLen = U16_ਲੰਮਾਈ(c);
                ਜੇ(j + charLen <= dest_ਮਾਪ) {
                    int32_t ਆਰਜ਼ੀ = j;
                    U16_APPEND_UNSAFE(dest, ਆਰਜ਼ੀ, c);
                }
                j += charLen;
            }
        } ਜਦੋਂ(src_ਲੰਮਾਈ>0);
        ਜੇ(dest_ਮਾਪ < j) {
            *p_Error_ਕੋਡ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        }
        ਵਾਪਸ j;
    }
    } /* end of switch */
}

ਸਥਿਰ int32_t
do_Write_ਉਲਟ(ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
               char16_t *dest, int32_t dest_ਮਾਪ,
               uint16_t options,
               UError_ਕੋਡ *p_Error_ਕੋਡ) {
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

    ਜੇ(src_ਲੰਮਾਈ<=0) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* optimize for several combinations of options */
    ਚੋਣ(options&(UBIDI_REMOVE_BIDI_CONTROLS|UBIDI_DO_MIRRORING|UBIDI_KEEP_ਅਧਾਰ_COMBINING)) {
    ਹਾਲਤ 0:
        /*
         * With none of the "complicated" options set, the destination
         * run will have the same length as the source run,
         * and there is no mirroring and no keeping combining characters
         * with their base characters.
         */
        ਜੇ(dest_ਮਾਪ<src_ਲੰਮਾਈ) {
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ src_ਲੰਮਾਈ;
        }
        dest_ਮਾਪ=src_ਲੰਮਾਈ;

        /* preserve character integrity */
        ਕਰੋ {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_ਲੰਮਾਈ;

            /* collect code units for one base character */
            U16_PREV(src, 0, src_ਲੰਮਾਈ, c);

            /* copy this base character */
            j=src_ਲੰਮਾਈ;
            ਕਰੋ {
                *dest++=src[j++];
            } ਜਦੋਂ(j<i);
        } ਜਦੋਂ(src_ਲੰਮਾਈ>0);
        ਰੋਕੋ;
    ਹਾਲਤ UBIDI_KEEP_ਅਧਾਰ_COMBINING:
        /*
         * Here, too, the destination
         * run will have the same length as the source run,
         * and there is no mirroring.
         * We do need to keep combining characters with their base characters.
         */
        ਜੇ(dest_ਮਾਪ<src_ਲੰਮਾਈ) {
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ src_ਲੰਮਾਈ;
        }
        dest_ਮਾਪ=src_ਲੰਮਾਈ;

        /* preserve character integrity */
        ਕਰੋ {
            /* i is always after the last code unit known to need to be kept in this segment */
            i=src_ਲੰਮਾਈ;

            /* collect code units and modifier letters for one base character */
            ਕਰੋ {
                U16_PREV(src, 0, src_ਲੰਮਾਈ, c);
            } ਜਦੋਂ(src_ਲੰਮਾਈ>0 && IS_COMBINING(u_charType(c)));

            /* copy this "user character" */
            j=src_ਲੰਮਾਈ;
            ਕਰੋ {
                *dest++=src[j++];
            } ਜਦੋਂ(j<i);
        } ਜਦੋਂ(src_ਲੰਮਾਈ>0);
        ਰੋਕੋ;
    ਮੂਲ: {
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
        ਜੇ(options&UBIDI_ਅੰਦਰੂਨੀ_RUNS_ONLY) {
            ਜੇ(!(options&UBIDI_REMOVE_BIDI_CONTROLS)) {
                i=src_ਲੰਮਾਈ;
            } ਨਹੀਂਤਾਂ {
                int32_t length=src_ਲੰਮਾਈ;
                char16_t ch;
                i=0;
                /* Loop over all code units in the source run to count those that are not Bidi control characters,
                 * determining the required destination buffer length for preflighting when UBIDI_REMOVE_BIDI_CONTROLS is set.
                 */
                ਕਰੋ {
                    ch=*src++;
                    ਜੇ(!IS_BIDI_CONTROL_CHAR(ch)) {
                        ++i;
                    }
                } ਜਦੋਂ(--length>0);
                src-=src_ਲੰਮਾਈ;
            }
            ਜੇ(dest_ਮਾਪ<i) {
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਵਾਪਸ i;
            }
            char16_t *saveDest=dest;
            int32_t dest_ਲੰਮਾਈ=0;
            /* Loop backward across the source run segment by segment, using legacy 1-to-1 code unit stepping
             * to extract and reverse character clusters for internal setPara classification.
             */
            ਕਰੋ {
                i=src_ਲੰਮਾਈ;
                U16_PREV(src, 0, src_ਲੰਮਾਈ, c);
                ਜੇ(options&UBIDI_KEEP_ਅਧਾਰ_COMBINING) {
                    /* Loop backward over any combining marks attached to the base character so that the combining
                     * sequence remains attached after its base character when reversed.
                     */
                    ਜਦੋਂ(src_ਲੰਮਾਈ>0 && IS_COMBINING(u_charType(c))) {
                        U16_PREV(src, 0, src_ਲੰਮਾਈ, c);
                    }
                }
                ਜੇ(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                    ਜਾਰੀ;
                }
                int32_t orig_ਅਧਾਰ_Len=U16_ਲੰਮਾਈ(c);
                j=src_ਲੰਮਾਈ;
                ਜੇ(options&UBIDI_DO_MIRRORING) {
                    c=u_charMirror(c);
                    int32_t k=U16_ਲੰਮਾਈ(c);
                    ਜੇ(dest_ਲੰਮਾਈ+k<=dest_ਮਾਪ) {
                        int32_t ਆਰਜ਼ੀ=dest_ਲੰਮਾਈ;
                        U16_APPEND_UNSAFE(saveDest, ਆਰਜ਼ੀ, c);
                    }
                    dest_ਲੰਮਾਈ+=k;
                    j+=orig_ਅਧਾਰ_Len;
                }
                /* Loop forward from the current segment index to copy any unmirrored code units (such as combining marks
                 * or surrogate halves) into dest in forward visual order.
                 */
                ਜਦੋਂ(j<i) {
                    ਜੇ(dest_ਲੰਮਾਈ<dest_ਮਾਪ) {
                        saveDest[dest_ਲੰਮਾਈ]=src[j];
                    }
                    dest_ਲੰਮਾਈ++;
                    j++;
                }
            } ਜਦੋਂ(src_ਲੰਮਾਈ>0);
            ਜੇ(dest_ਮਾਪ<dest_ਲੰਮਾਈ) {
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
            }
            ਵਾਪਸ dest_ਲੰਮਾਈ;
        }
        int32_t dest_ਲੰਮਾਈ=0;
        ਕਰੋ {
            i=src_ਲੰਮਾਈ;
            U16_PREV(src, 0, src_ਲੰਮਾਈ, c);
            ਜੇ(options&UBIDI_KEEP_ਅਧਾਰ_COMBINING) {
                ਜਦੋਂ(src_ਲੰਮਾਈ>0 && IS_COMBINING(u_charType(c))) {
                    U16_PREV(src, 0, src_ਲੰਮਾਈ, c);
                }
            }
            ਜੇ(options&UBIDI_REMOVE_BIDI_CONTROLS && IS_BIDI_CONTROL_CHAR(c)) {
                ਜਾਰੀ;
            }

            int32_t orig_ਅਧਾਰ_Len=U16_ਲੰਮਾਈ(c);
            j=src_ਲੰਮਾਈ;
            ਜੇ(options&UBIDI_DO_MIRRORING) {
                /* mirror only the base character */
                c=u_charMirror(c);
                int32_t k=U16_ਲੰਮਾਈ(c);
                ਜੇ(dest_ਲੰਮਾਈ+k<=dest_ਮਾਪ) {
                    int32_t ਆਰਜ਼ੀ=dest_ਲੰਮਾਈ;
                    U16_APPEND_UNSAFE(dest, ਆਰਜ਼ੀ, c);
                }
                dest_ਲੰਮਾਈ+=k;
                j+=orig_ਅਧਾਰ_Len;
            }
            ਜਦੋਂ(j<i) {
                ਜੇ(dest_ਲੰਮਾਈ<dest_ਮਾਪ) {
                    dest[dest_ਲੰਮਾਈ]=src[j];
                }
                dest_ਲੰਮਾਈ++;
                j++;
            }
        } ਜਦੋਂ(src_ਲੰਮਾਈ>0);


        ਜੇ(dest_ਮਾਪ<dest_ਲੰਮਾਈ) {
            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        }
        ਵਾਪਸ dest_ਲੰਮਾਈ;
    }
    } /* end of switch */
    ਵਾਪਸ dest_ਮਾਪ;
}

U_CAPI int32_t U_EXPORT2
ubidi_write_ਉਲਟ(ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                   char16_t *dest, int32_t dest_ਮਾਪ,
                   uint16_t options,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    int32_t dest_ਲੰਮਾਈ;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* more error checking */
    ਜੇ( src==ਖਾਲੀ_ਸੂਚਕ || src_ਲੰਮਾਈ<-1 ||
        dest_ਮਾਪ<0 || (dest_ਮਾਪ>0 && dest==ਖਾਲੀ_ਸੂਚਕ))
    {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* do input and output overlap? */
    ਜੇ( dest!=ਖਾਲੀ_ਸੂਚਕ &&
        ((src>=dest && src<dest+dest_ਮਾਪ) ||
         (dest>=src && dest<src+src_ਲੰਮਾਈ)))
    {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(src_ਲੰਮਾਈ==-1) {
        src_ਲੰਮਾਈ=u_strlen(src);
    }
    ਜੇ(src_ਲੰਮਾਈ>0) {
        dest_ਲੰਮਾਈ=do_Write_ਉਲਟ(src, src_ਲੰਮਾਈ, dest, dest_ਮਾਪ, options, p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        /* nothing to do */
        dest_ਲੰਮਾਈ=0;
    }

    ਵਾਪਸ u_terminateUChars(dest, dest_ਮਾਪ, dest_ਲੰਮਾਈ, p_Error_ਕੋਡ);
}

// Ticket 20907 - The optimizer in MSVC/Visual Studio versions below 16.4 has trouble with this
// function on Windows ARM64. As a work-around, we disable optimizations for this function.
// This work-around could/should be removed once the following versions of Visual Studio are no
// longer supported: All versions of VS2017, and versions of VS2019 below 16.4.
#ਜੇ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ਹਦਾਇਤ optimize( "", off )
#ਜੇ_ਅੰਤ
U_CAPI int32_t U_EXPORT2
ubidi_writeReordered(UBiDi *pBiDi,
                     char16_t *dest, int32_t dest_ਮਾਪ,
                     uint16_t options,
                     UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ char16_t *ਪਾਠ;
    char16_t *saveDest;
    int32_t length, dest_ਸਮਰੱਥਾ;
    int32_t ਲੜੀਵਾਰ_2, ਲੜੀਵਾਰ_ਗਿਣਤੀ, logical_ਸ਼ੁਰੂ, ਲੜੀਵਾਰ_ਲੰਮਾਈ;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* more error checking */
    ਜੇ( pBiDi==ਖਾਲੀ_ਸੂਚਕ ||
        (ਪਾਠ=pBiDi->ਪਾਠ)==ਖਾਲੀ_ਸੂਚਕ || (length=pBiDi->length)<0 ||
        dest_ਮਾਪ<0 || (dest_ਮਾਪ>0 && dest==ਖਾਲੀ_ਸੂਚਕ))
    {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* do input and output overlap? */
    ਜੇ( dest!=ਖਾਲੀ_ਸੂਚਕ &&
        ((ਪਾਠ>=dest && ਪਾਠ<dest+dest_ਮਾਪ) ||
         (dest>=ਪਾਠ && dest<ਪਾਠ+pBiDi->original_ਲੰਮਾਈ)))
    {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(length==0) {
        /* nothing to do */
        ਵਾਪਸ u_terminateUChars(dest, dest_ਮਾਪ, 0, p_Error_ਕੋਡ);
    }

    ਲੜੀਵਾਰ_ਗਿਣਤੀ=ubidi_ਗਿਣਤੀ_Runs(pBiDi, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* destSize shrinks, later destination length=destCapacity-destSize */
    saveDest=dest;
    dest_ਸਮਰੱਥਾ=dest_ਮਾਪ;
    ਲੜੀਵਾਰ_ਲੰਮਾਈ = 0;

    /*
     * Option "insert marks" implies UBIDI_INSERT_LRM_FOR_NUMERIC if the
     * reordering mode (checked below) is appropriate.
     */
    ਜੇ(pBiDi->reorderingOptions & UBIDI_OPTION_ਪਾਓ_MARKS) {
        options|=UBIDI_ਪਾਓ_LRM_FOR_NUMERIC;
        options&=~UBIDI_REMOVE_BIDI_CONTROLS;
    }
    /*
     * Option "remove controls" implies UBIDI_REMOVE_BIDI_CONTROLS
     * and cancels UBIDI_INSERT_LRM_FOR_NUMERIC.
     */
    ਜੇ(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        options|=UBIDI_REMOVE_BIDI_CONTROLS;
        options&=~UBIDI_ਪਾਓ_LRM_FOR_NUMERIC;
    }
    /*
     * If we do not perform the "inverse BiDi" algorithm, then we
     * don't need to insert any LRMs, and don't need to test for it.
     */
    ਜੇ((pBiDi->reordering_ਬਹੁਲਕ != UBIDI_REORDER_INVERSE_NUMBERS_AS_L) &&
       (pBiDi->reordering_ਬਹੁਲਕ != UBIDI_REORDER_INVERSE_LIKE_ਸਿੱਧਾ)  &&
       (pBiDi->reordering_ਬਹੁਲਕ != UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL) &&
       (pBiDi->reordering_ਬਹੁਲਕ != UBIDI_REORDER_RUNS_ONLY)) {
        options&=~UBIDI_ਪਾਓ_LRM_FOR_NUMERIC;
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
    ਜੇ(!(options&UBIDI_OUTPUT_ਉਲਟ)) {
        /* forward output */
        ਜੇ(!(options&UBIDI_ਪਾਓ_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            ਲਈ(ਲੜੀਵਾਰ_2=0; ਲੜੀਵਾਰ_2<ਲੜੀਵਾਰ_ਗਿਣਤੀ; ++ਲੜੀਵਾਰ_2) {
                ਜੇ(UBIDI_LTR==ubidi_get_Visual_ਲੜੀਵਾਰ(pBiDi, ਲੜੀਵਾਰ_2, &logical_ਸ਼ੁਰੂ, &ਲੜੀਵਾਰ_ਲੰਮਾਈ)) {
                    ਲੜੀਵਾਰ_ਲੰਮਾਈ=doWriteForward(ਪਾਠ+logical_ਸ਼ੁਰੂ, ਲੜੀਵਾਰ_ਲੰਮਾਈ,
                                             dest, dest_ਮਾਪ,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_ਕੋਡ);
                } ਨਹੀਂਤਾਂ {
                    ਲੜੀਵਾਰ_ਲੰਮਾਈ=do_Write_ਉਲਟ(ਪਾਠ+logical_ਸ਼ੁਰੂ, ਲੜੀਵਾਰ_ਲੰਮਾਈ,
                                             dest, dest_ਮਾਪ,
                                             options, p_Error_ਕੋਡ);
                }
                ਜੇ(dest!=ਖਾਲੀ_ਸੂਚਕ) {
                  dest+=ਲੜੀਵਾਰ_ਲੰਮਾਈ;
                }
                dest_ਮਾਪ-=ਲੜੀਵਾਰ_ਲੰਮਾਈ;
            }
        } ਨਹੀਂਤਾਂ {
            /* insert BiDi controls for "inverse BiDi" */
            ਅਚਲ DirProp *dirProps=pBiDi->dirProps;
            ਅਚਲ char16_t *src;
            char16_t uc;
            UBiDiDirection dir;
            int32_t markFlag;

            ਲਈ(ਲੜੀਵਾਰ_2=0; ਲੜੀਵਾਰ_2<ਲੜੀਵਾਰ_ਗਿਣਤੀ; ++ਲੜੀਵਾਰ_2) {
                dir=ubidi_get_Visual_ਲੜੀਵਾਰ(pBiDi, ਲੜੀਵਾਰ_2, &logical_ਸ਼ੁਰੂ, &ਲੜੀਵਾਰ_ਲੰਮਾਈ);
                src=ਪਾਠ+logical_ਸ਼ੁਰੂ;
                /* check if something relevant in insertPoints */
                markFlag=pBiDi->runs[ਲੜੀਵਾਰ_2].ਪਾਓ_Remove;
                ਜੇ(markFlag<0) {        /* BiDi controls count */
                    markFlag=0;
                }

                ਜੇ(UBIDI_LTR==dir) {
                    ਜੇ((pBiDi->isInverse) &&
                       (/*run>0 &&*/ dirProps[logical_ਸ਼ੁਰੂ]!=L)) {
                        markFlag |= LRM_BEFORE;
                    }
                    ਜੇ (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    ਨਹੀਂਤਾਂ ਜੇ (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    ਨਹੀਂਤਾਂ  uc=0;
                    ਜੇ(uc) {
                        ਜੇ(dest_ਮਾਪ>0) {
                            *dest++=uc;
                        }
                        --dest_ਮਾਪ;
                    }

                    ਲੜੀਵਾਰ_ਲੰਮਾਈ=doWriteForward(src, ਲੜੀਵਾਰ_ਲੰਮਾਈ,
                                             dest, dest_ਮਾਪ,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_ਕੋਡ);
                    ਜੇ(dest!=ਖਾਲੀ_ਸੂਚਕ) {
                      dest+=ਲੜੀਵਾਰ_ਲੰਮਾਈ;
                    }
                    dest_ਮਾਪ-=ਲੜੀਵਾਰ_ਲੰਮਾਈ;

                    ਜੇ((pBiDi->isInverse) &&
                           (/*run<runCount-1 &&*/
                            ਲੜੀਵਾਰ_ਲੰਮਾਈ > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            dirProps[logical_ਸ਼ੁਰੂ+ਲੜੀਵਾਰ_ਲੰਮਾਈ-1]!=L)) {
                        markFlag |= LRM_AFTER;
                    }
                    ਜੇ (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    ਨਹੀਂਤਾਂ ਜੇ (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    ਨਹੀਂਤਾਂ  uc=0;
                    ਜੇ(uc) {
                        ਜੇ(dest_ਮਾਪ>0) {
                            *dest++=uc;
                        }
                        --dest_ਮਾਪ;
                    }
                } ਨਹੀਂਤਾਂ {                /* RTL run */
                    ਜੇ((pBiDi->isInverse) &&
                       (/*run>0 &&*/ !(ਨਕਾਬ_R_AL&DIRPROP_FLAG(dirProps[logical_ਸ਼ੁਰੂ+ਲੜੀਵਾਰ_ਲੰਮਾਈ-1])))) {
                        markFlag |= RLM_BEFORE;
                    }
                    ਜੇ (markFlag & LRM_BEFORE) {
                        uc=LRM_CHAR;
                    }
                    ਨਹੀਂਤਾਂ ਜੇ (markFlag & RLM_BEFORE) {
                        uc=RLM_CHAR;
                    }
                    ਨਹੀਂਤਾਂ  uc=0;
                    ਜੇ(uc) {
                        ਜੇ(dest_ਮਾਪ>0) {
                            *dest++=uc;
                        }
                        --dest_ਮਾਪ;
                    }

                    ਲੜੀਵਾਰ_ਲੰਮਾਈ=do_Write_ਉਲਟ(src, ਲੜੀਵਾਰ_ਲੰਮਾਈ,
                                             dest, dest_ਮਾਪ,
                                             options, p_Error_ਕੋਡ);
                    ਜੇ(dest!=ਖਾਲੀ_ਸੂਚਕ) {
                      dest+=ਲੜੀਵਾਰ_ਲੰਮਾਈ;
                    }
                    dest_ਮਾਪ-=ਲੜੀਵਾਰ_ਲੰਮਾਈ;

                    ਜੇ((pBiDi->isInverse) &&
                       (/*run<runCount-1 &&*/ !(ਨਕਾਬ_R_AL&DIRPROP_FLAG(dirProps[logical_ਸ਼ੁਰੂ])))) {
                        markFlag |= RLM_AFTER;
                    }
                    ਜੇ (markFlag & LRM_AFTER) {
                        uc=LRM_CHAR;
                    }
                    ਨਹੀਂਤਾਂ ਜੇ (markFlag & RLM_AFTER) {
                        uc=RLM_CHAR;
                    }
                    ਨਹੀਂਤਾਂ  uc=0;
                    ਜੇ(uc) {
                        ਜੇ(dest_ਮਾਪ>0) {
                            *dest++=uc;
                        }
                        --dest_ਮਾਪ;
                    }
                }
            }
        }
    } ਨਹੀਂਤਾਂ {
        /* reverse output */
        ਜੇ(!(options&UBIDI_ਪਾਓ_LRM_FOR_NUMERIC)) {
            /* do not insert BiDi controls */
            ਲਈ(ਲੜੀਵਾਰ_2=ਲੜੀਵਾਰ_ਗਿਣਤੀ; --ਲੜੀਵਾਰ_2>=0;) {
                ਜੇ(UBIDI_LTR==ubidi_get_Visual_ਲੜੀਵਾਰ(pBiDi, ਲੜੀਵਾਰ_2, &logical_ਸ਼ੁਰੂ, &ਲੜੀਵਾਰ_ਲੰਮਾਈ)) {
                    ਲੜੀਵਾਰ_ਲੰਮਾਈ=do_Write_ਉਲਟ(ਪਾਠ+logical_ਸ਼ੁਰੂ, ਲੜੀਵਾਰ_ਲੰਮਾਈ,
                                             dest, dest_ਮਾਪ,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_ਕੋਡ);
                } ਨਹੀਂਤਾਂ {
                    ਲੜੀਵਾਰ_ਲੰਮਾਈ=doWriteForward(ਪਾਠ+logical_ਸ਼ੁਰੂ, ਲੜੀਵਾਰ_ਲੰਮਾਈ,
                                             dest, dest_ਮਾਪ,
                                             options, p_Error_ਕੋਡ);
                }
                ਜੇ(dest!=ਖਾਲੀ_ਸੂਚਕ) {
                  dest+=ਲੜੀਵਾਰ_ਲੰਮਾਈ;
                }
                dest_ਮਾਪ-=ਲੜੀਵਾਰ_ਲੰਮਾਈ;
            }
        } ਨਹੀਂਤਾਂ {
            /* insert BiDi controls for "inverse BiDi" */
            ਅਚਲ DirProp *dirProps=pBiDi->dirProps;
            ਅਚਲ char16_t *src;
            UBiDiDirection dir;

            ਲਈ(ਲੜੀਵਾਰ_2=ਲੜੀਵਾਰ_ਗਿਣਤੀ; --ਲੜੀਵਾਰ_2>=0;) {
                /* reverse output */
                dir=ubidi_get_Visual_ਲੜੀਵਾਰ(pBiDi, ਲੜੀਵਾਰ_2, &logical_ਸ਼ੁਰੂ, &ਲੜੀਵਾਰ_ਲੰਮਾਈ);
                src=ਪਾਠ+logical_ਸ਼ੁਰੂ;

                ਜੇ(UBIDI_LTR==dir) {
                    ਜੇ(/*run<runCount-1 &&*/ dirProps[logical_ਸ਼ੁਰੂ+ਲੜੀਵਾਰ_ਲੰਮਾਈ-1]!=L) {
                        ਜੇ(dest_ਮਾਪ>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_ਮਾਪ;
                    }

                    ਲੜੀਵਾਰ_ਲੰਮਾਈ=do_Write_ਉਲਟ(src, ਲੜੀਵਾਰ_ਲੰਮਾਈ,
                                             dest, dest_ਮਾਪ,
                                             (uint16_t)(options&~UBIDI_DO_MIRRORING), p_Error_ਕੋਡ);
                    ਜੇ(dest!=ਖਾਲੀ_ਸੂਚਕ) {
                      dest+=ਲੜੀਵਾਰ_ਲੰਮਾਈ;
                    }
                    dest_ਮਾਪ-=ਲੜੀਵਾਰ_ਲੰਮਾਈ;

                    ਜੇ(/*run>0 &&*/ dirProps[logical_ਸ਼ੁਰੂ]!=L) {
                        ਜੇ(dest_ਮਾਪ>0) {
                            *dest++=LRM_CHAR;
                        }
                        --dest_ਮਾਪ;
                    }
                } ਨਹੀਂਤਾਂ {
                    ਜੇ(/*run<runCount-1 &&*/ !(ਨਕਾਬ_R_AL&DIRPROP_FLAG(dirProps[logical_ਸ਼ੁਰੂ]))) {
                        ਜੇ(dest_ਮਾਪ>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_ਮਾਪ;
                    }

                    int32_t original_ਲੜੀਵਾਰ_ਲੰਮਾਈ = ਲੜੀਵਾਰ_ਲੰਮਾਈ;
                    ਲੜੀਵਾਰ_ਲੰਮਾਈ=doWriteForward(src, ਲੜੀਵਾਰ_ਲੰਮਾਈ,
                                             dest, dest_ਮਾਪ,
                                             options, p_Error_ਕੋਡ);
                    ਜੇ(dest!=ਖਾਲੀ_ਸੂਚਕ) {
                      dest+=ਲੜੀਵਾਰ_ਲੰਮਾਈ;
                    }
                    dest_ਮਾਪ-=ਲੜੀਵਾਰ_ਲੰਮਾਈ;

                    ਜੇ(/*run>0 &&*/
                            ਲੜੀਵਾਰ_ਲੰਮਾਈ > 0 && // doWriteForward may return 0 if src
                                             // only include bidi control chars
                            !(ਨਕਾਬ_R_AL&DIRPROP_FLAG(dirProps[logical_ਸ਼ੁਰੂ+original_ਲੜੀਵਾਰ_ਲੰਮਾਈ-1]))) {
                        ਜੇ(dest_ਮਾਪ>0) {
                            *dest++=RLM_CHAR;
                        }
                        --dest_ਮਾਪ;
                    }
                }
            }
        }
    }

    ਵਾਪਸ u_terminateUChars(saveDest, dest_ਸਮਰੱਥਾ, dest_ਸਮਰੱਥਾ-dest_ਮਾਪ, p_Error_ਕੋਡ);
}
#ਜੇ (defined(_MSC_VER) && (defined(_M_ARM64)) && (_MSC_VER < 1924))
#ਹਦਾਇਤ optimize( "", on )
#ਜੇ_ਅੰਤ
