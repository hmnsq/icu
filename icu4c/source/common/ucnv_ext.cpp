// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2003-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  ucnv_ext.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003jun13
*   created by: Markus W. Scherer
*
*   Conversion extensions
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "ucnv_ext.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

/* to Unicode --------------------------------------------------------------- */

/*
 * @return lookup value for the byte, if found; else 0
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t
ucnv_ext_ಕಂಡುಹಿಡಿ_ಗಮ್ಯ_U(ಸ್ಥಿರಾಂಕ uint32_t *ಗಮ್ಯ_USection, int32_t length, uint8_t byte) {
    uint32_t word0, ಶಬ್ದ;
    int32_t i, ಆರಂಭ, ಎಲ್ಲೆ;

    /* check the input byte against the lowest and highest section bytes */
    ಆರಂಭ = static_cast<int32_t>(UCNV_EXT_ಗಮ್ಯ_U_GET_BYTE(ಗಮ್ಯ_USection[0]));
    ಎಲ್ಲೆ = static_cast<int32_t>(UCNV_EXT_ಗಮ್ಯ_U_GET_BYTE(ಗಮ್ಯ_USection[length - 1]));
    ಆದರೆ(byte<ಆರಂಭ || ಎಲ್ಲೆ<byte) {
        ಹಿಂತಿರುಗಿ 0; /* the byte is out of range */
    }

    ಆದರೆ(length==((ಎಲ್ಲೆ-ಆರಂಭ)+1)) {
        /* direct access on a linear array */
        ಹಿಂತಿರುಗಿ UCNV_EXT_ಗಮ್ಯ_U_GET_ಮೌಲ್ಯ(ಗಮ್ಯ_USection[byte-ಆರಂಭ]); /* could be 0 */
    }

    /* word0 is suitable for <=toUSection[] comparison, word for <toUSection[] */
    word0=UCNV_EXT_ಗಮ್ಯ_U_MAKE_ಶಬ್ದ(byte, 0);

    /*
     * Shift byte once instead of each section word and add 0xffffff.
     * We will compare the shifted/added byte (bbffffff) against
     * section words which have byte values in the same bit position.
     * If and only if byte bb < section byte ss then bbffffff<ssvvvvvv
     * for all v=0..f
     * so we need not mask off the lower 24 bits of each section word.
     */
    ಶಬ್ದ=word0|UCNV_EXT_ಗಮ್ಯ_U_ಮೌಲ್ಯ_ಮುಖವಾಡ;

    /* binary search */
    ಆರಂಭ=0;
    ಎಲ್ಲೆ=length;
    ಗಾಗಿ(;;) {
        i=ಎಲ್ಲೆ-ಆರಂಭ;
        ಆದರೆ(i<=1) {
            ನಿಲ್ಲಿಸು; /*ಮುಗಿದ*/
        }
        /* start<limit-1 */

        ಆದರೆ(i<=4) {
            /* linear search for the last part */
            ಆದರೆ(word0<=ಗಮ್ಯ_USection[ಆರಂಭ]) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(++ಆರಂಭ<ಎಲ್ಲೆ && word0<=ಗಮ್ಯ_USection[ಆರಂಭ]) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(++ಆರಂಭ<ಎಲ್ಲೆ && word0<=ಗಮ್ಯ_USection[ಆರಂಭ]) {
                ನಿಲ್ಲಿಸು;
            }
            /* always break at start==limit-1 */
            ++ಆರಂಭ;
            ನಿಲ್ಲಿಸು;
        }

        i=(ಆರಂಭ+ಎಲ್ಲೆ)/2;
        ಆದರೆ(ಶಬ್ದ<ಗಮ್ಯ_USection[i]) {
            ಎಲ್ಲೆ=i;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆರಂಭ=i;
        }
    }

    /* did we really find it? */
    ಆದರೆ(ಆರಂಭ<ಎಲ್ಲೆ && byte==UCNV_EXT_ಗಮ್ಯ_U_GET_BYTE(ಶಬ್ದ=ಗಮ್ಯ_USection[ಆರಂಭ])) {
        ಹಿಂತಿರುಗಿ UCNV_EXT_ಗಮ್ಯ_U_GET_ಮೌಲ್ಯ(ಶಬ್ದ); /* never 0 */
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0; /* not found */
    }
}

/*
 * true if not an SI/SO stateful converter,
 * or if the match length fits with the current converter state
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_EXT_ಗಮ್ಯ_U_VERIFY_SISO_ಹೊಂದಿಕೆ(sisoState, ಹೊಂದಿಕೆ) \
    ((sisoState)<0 || ((sisoState)==0) == (ಹೊಂದಿಕೆ==1))

/*
 * this works like ucnv_extMatchFromU() except
 * - the first character is in pre
 * - no trie is used
 * - the returned matchLength is not offset by 2
 */
ಸ್ಥಿರ int32_t
ucnv_ext_ಹೊಂದಿಕೆ_ಗಮ್ಯ_U(ಸ್ಥಿರಾಂಕ int32_t *cx, int8_t sisoState,
                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pre, int32_t pre_ಉದ್ದಳತೆ,
                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t src_ಉದ್ದಳತೆ,
                 uint32_t *p_ಹೊಂದಿಕೆ_ಮೌಲ್ಯ,
                 UBool /*useFallback*/, UBool flush) {
    ಸ್ಥಿರಾಂಕ uint32_t *ಗಮ್ಯ_UTable, *ಗಮ್ಯ_USection;

    uint32_t value, ಹೊಂದಿಕೆ_ಮೌಲ್ಯ;
    int32_t i, j, idx, length, ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;
    uint8_t b;

    ಆದರೆ(cx==ಶೂನ್ಯಸೂಚಕ || cx[UCNV_EXT_ಗಮ್ಯ_U_ಉದ್ದಳತೆ]<=0) {
        ಹಿಂತಿರುಗಿ 0; /* no extension data, no match */
    }

    /* initialize */
    ಗಮ್ಯ_UTable=UCNV_EXT_ARRAY(cx, UCNV_EXT_ಗಮ್ಯ_U_ಸೂಚ್ಯಂಕ, uint32_t);
    idx=0;

    ಹೊಂದಿಕೆ_ಮೌಲ್ಯ=0;
    i=j=ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=0;

    ಆದರೆ(sisoState==0) {
        /* SBCS state of an SI/SO stateful converter, look at only exactly 1 byte */
        ಆದರೆ(pre_ಉದ್ದಳತೆ>1) {
            ಹಿಂತಿರುಗಿ 0; /* no match of a DBCS sequence in SBCS mode */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pre_ಉದ್ದಳತೆ==1) {
            src_ಉದ್ದಳತೆ=0;
        } ಇಲ್ಲದಿದ್ದರೆ /* preLength==0 */ {
            ಆದರೆ(src_ಉದ್ದಳತೆ>1) {
                src_ಉದ್ದಳತೆ=1;
            }
        }
        flush=ನಿಜ;
    }

    /* we must not remember fallback matches when not using fallbacks */

    /* match input units until there is a full match or the input is consumed */
    ಗಾಗಿ(;;) {
        /* go to the next section */
        ಗಮ್ಯ_USection=ಗಮ್ಯ_UTable+idx;

        /* read first pair of the section */
        value=*ಗಮ್ಯ_USection++;
        length=UCNV_EXT_ಗಮ್ಯ_U_GET_BYTE(value);
        value=UCNV_EXT_ಗಮ್ಯ_U_GET_ಮೌಲ್ಯ(value);
        ಆದರೆ( value!=0 &&
            (UCNV_EXT_ಗಮ್ಯ_U_IS_ROUNDTRIP(value) ||
             ಗಮ್ಯ_U_USE_FALLBACK(useFallback)) &&
            UCNV_EXT_ಗಮ್ಯ_U_VERIFY_SISO_ಹೊಂದಿಕೆ(sisoState, i+j)
        ) {
            /* remember longest match so far */
            ಹೊಂದಿಕೆ_ಮೌಲ್ಯ=value;
            ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=i+j;
        }

        /* match pre[] then src[] */
        ಆದರೆ(i<pre_ಉದ್ದಳತೆ) {
            b = static_cast<uint8_t>(pre[i++]);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(j<src_ಉದ್ದಳತೆ) {
            b = static_cast<uint8_t>(src[j++]);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* all input consumed, partial match */
            ಆದರೆ(flush || (length=(i+j))>UCNV_EXT_MAX_ಬೈಟುಗಳು) {
                /*
                 * end of the entire input stream, stop with the longest match so far
                 * or: partial match must not be longer than UCNV_EXT_MAX_BYTES
                 * because it must fit into state buffers
                 */
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* continue with more input next time */
                ಹಿಂತಿರುಗಿ -length;
            }
        }

        /* search for the current char16_t */
        value=ucnv_ext_ಕಂಡುಹಿಡಿ_ಗಮ್ಯ_U(ಗಮ್ಯ_USection, length, b);
        ಆದರೆ(value==0) {
            /* no match here, stop with the longest match so far */
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(UCNV_EXT_ಗಮ್ಯ_U_IS_PARTIAL(value)) {
                /* partial match, continue */
                idx = static_cast<int32_t>(UCNV_EXT_ಗಮ್ಯ_U_GET_PARTIAL_ಸೂಚ್ಯಂಕ(value));
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ( (UCNV_EXT_ಗಮ್ಯ_U_IS_ROUNDTRIP(value) ||
                     ಗಮ್ಯ_U_USE_FALLBACK(useFallback)) &&
                    UCNV_EXT_ಗಮ್ಯ_U_VERIFY_SISO_ಹೊಂದಿಕೆ(sisoState, i+j)
                ) {
                    /* full match, stop with result */
                    ಹೊಂದಿಕೆ_ಮೌಲ್ಯ=value;
                    ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=i+j;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* full match on fallback not taken, stop with the longest match so far */
                }
                ನಿಲ್ಲಿಸು;
            }
        }
    }

    ಆದರೆ(ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ==0) {
        /* no match at all */
        ಹಿಂತಿರುಗಿ 0;
    }

    /* return result */
    *p_ಹೊಂದಿಕೆ_ಮೌಲ್ಯ=UCNV_EXT_ಗಮ್ಯ_U_ಮುಖವಾಡ_ROUNDTRIP(ಹೊಂದಿಕೆ_ಮೌಲ್ಯ);
    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
ucnv_ext_Write_ಗಮ್ಯ_U(UConverter *cnv, ಸ್ಥಿರಾಂಕ int32_t *cx,
                 uint32_t value,
                 char16_t **ಗುರಿ, ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ,
                 int32_t **offsets, int32_t src_ಸೂಚ್ಯಂಕ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    /* output the result */
    ಆದರೆ(UCNV_EXT_ಗಮ್ಯ_U_IS_ಸಂಕೇತ_ಬಿಂದು(value)) {
        /* output a single code point */
        ucnv_ಗಮ್ಯ_UWrite_ಸಂಕೇತ_ಬಿಂದು(
            cnv, UCNV_EXT_ಗಮ್ಯ_U_GET_ಸಂಕೇತ_ಬಿಂದು(value),
            ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ,
            offsets, src_ಸೂಚ್ಯಂಕ,
            p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* output a string - with correct data we have resultLength>0 */
        ucnv_ಗಮ್ಯ_UWrite_UChars(
            cnv,
            UCNV_EXT_ARRAY(cx, UCNV_EXT_ಗಮ್ಯ_U_UCHARS_ಸೂಚ್ಯಂಕ, char16_t)+
                UCNV_EXT_ಗಮ್ಯ_U_GET_ಸೂಚ್ಯಂಕ(value),
            UCNV_EXT_ಗಮ್ಯ_U_GET_ಉದ್ದಳತೆ(value),
            ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ,
            offsets, src_ಸೂಚ್ಯಂಕ,
            p_Error_ಸಂಕೇತ);
    }
}

/*
 * get the SI/SO toU state (state 0 is for SBCS, 1 for DBCS),
 * or 1 for DBCS-only,
 * or -1 if the converter is not SI/SO stateful
 *
 * Note: For SI/SO stateful converters getting here,
 * cnv->mode==0 is equivalent to firstLength==1.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_SISO_STATE(cnv) \
    ((cnv)->shared_ದತ್ತಾಂಶ->mbcs.outputType==MBCS_OUTPUT_2_SISO ? (int8_t)(cnv)->ಬಹುಲಕ : \
     (cnv)->shared_ದತ್ತಾಂಶ->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ? 1 : -1)

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_ಹೊಂದಿಕೆ_ಗಮ್ಯ_U(UConverter *cnv, ಸ್ಥಿರಾಂಕ int32_t *cx,
                        int32_t ಮೊದಲ_ಉದ್ದಳತೆ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **src, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src_ಎಲ್ಲೆ,
                        char16_t **ಗುರಿ, ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ,
                        int32_t **offsets, int32_t src_ಸೂಚ್ಯಂಕ,
                        UBool flush,
                        UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ಹೊಂದಿಕೆ;

    /* try to match */
    ಹೊಂದಿಕೆ=ucnv_ext_ಹೊಂದಿಕೆ_ಗಮ್ಯ_U(cx, (int8_t)UCNV_SISO_STATE(cnv),
                           (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)cnv->ಗಮ್ಯ_UBytes, ಮೊದಲ_ಉದ್ದಳತೆ,
                           *src, (int32_t)(src_ಎಲ್ಲೆ-*src),
                           &value,
                           cnv->useFallback, flush);
    ಆದರೆ(ಹೊಂದಿಕೆ>0) {
        /* advance src pointer for the consumed input */
        *src+=ಹೊಂದಿಕೆ-ಮೊದಲ_ಉದ್ದಳತೆ;

        /* write result to target */
        ucnv_ext_Write_ಗಮ್ಯ_U(cnv, cx,
                         value,
                         ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ,
                         offsets, src_ಸೂಚ್ಯಂಕ,
                         p_Error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ ನಿಜ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹೊಂದಿಕೆ<0) {
        /* save state for partial match */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
        int32_t j;

        /* copy the first code point */
        s=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)cnv->ಗಮ್ಯ_UBytes;
        cnv->pre_ಗಮ್ಯ_UFirst_ಉದ್ದಳತೆ=(int8_t)ಮೊದಲ_ಉದ್ದಳತೆ;
        ಗಾಗಿ(j=0; j<ಮೊದಲ_ಉದ್ದಳತೆ; ++j) {
            cnv->pre_ಗಮ್ಯ_U[j]=*s++;
        }

        /* now copy the newly consumed input */
        s=*src;
        ಹೊಂದಿಕೆ=-ಹೊಂದಿಕೆ;
        ಗಾಗಿ(; j<ಹೊಂದಿಕೆ; ++j) {
            cnv->pre_ಗಮ್ಯ_U[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_ಗಮ್ಯ_ULength=(int8_t)ಹೊಂದಿಕೆ;
        ಹಿಂತಿರುಗಿ ನಿಜ;
    } ಇಲ್ಲದಿದ್ದರೆ /* match==0 no match */ {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
}

U_CFUNC UChar32
ucnv_ext_Simple_ಹೊಂದಿಕೆ_ಗಮ್ಯ_U(ಸ್ಥಿರಾಂಕ int32_t *cx,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ, int32_t length,
                       UBool useFallback) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ಹೊಂದಿಕೆ;

    ಆದರೆ(length<=0) {
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    /* try to match */
    ಹೊಂದಿಕೆ=ucnv_ext_ಹೊಂದಿಕೆ_ಗಮ್ಯ_U(cx, -1,
                           ಮೂಲ, length,
                           ಶೂನ್ಯಸೂಚಕ, 0,
                           &value,
                           useFallback, ನಿಜ);
    ಆದರೆ(ಹೊಂದಿಕೆ==length) {
        /* write result for simple, single-character conversion */
        ಆದರೆ(UCNV_EXT_ಗಮ್ಯ_U_IS_ಸಂಕೇತ_ಬಿಂದು(value)) {
            ಹಿಂತಿರುಗಿ UCNV_EXT_ಗಮ್ಯ_U_GET_ಸಂಕೇತ_ಬಿಂದು(value);
        }
    }

    /*
     * return no match because
     * - match>0 && value points to string: simple conversion cannot handle multiple code points
     * - match>0 && match!=length: not all input consumed, forbidden for this function
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    ಹಿಂತಿರುಗಿ 0xfffe;
}

/*
 * continue partial match with new input
 * never called for simple, single-character conversion
 */
U_CFUNC ಶೂನ್ಯ
ucnv_ext_Continue_ಹೊಂದಿಕೆ_ಗಮ್ಯ_U(UConverter *cnv,
                         UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು, int32_t src_ಸೂಚ್ಯಂಕ,
                         UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ಹೊಂದಿಕೆ, length;

    ಹೊಂದಿಕೆ=ucnv_ext_ಹೊಂದಿಕೆ_ಗಮ್ಯ_U(cnv->shared_ದತ್ತಾಂಶ->mbcs.extIndexes, (int8_t)UCNV_SISO_STATE(cnv),
                           cnv->pre_ಗಮ್ಯ_U, cnv->pre_ಗಮ್ಯ_ULength,
                           p_ವಾದಗಳು->ಮೂಲ, (int32_t)(p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ-p_ವಾದಗಳು->ಮೂಲ),
                           &value,
                           cnv->useFallback, p_ವಾದಗಳು->flush);
    ಆದರೆ(ಹೊಂದಿಕೆ>0) {
        ಆದರೆ(ಹೊಂದಿಕೆ>=cnv->pre_ಗಮ್ಯ_ULength) {
            /* advance src pointer for the consumed input */
            p_ವಾದಗಳು->ಮೂಲ+=ಹೊಂದಿಕೆ-cnv->pre_ಗಮ್ಯ_ULength;
            cnv->pre_ಗಮ್ಯ_ULength=0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* the match did not use all of preToU[] - keep the rest for replay */
            length=cnv->pre_ಗಮ್ಯ_ULength-ಹೊಂದಿಕೆ;
            uprv_memmove(cnv->pre_ಗಮ್ಯ_U, cnv->pre_ಗಮ್ಯ_U+ಹೊಂದಿಕೆ, length);
            cnv->pre_ಗಮ್ಯ_ULength=(int8_t)-length;
        }

        /* write result */
        ucnv_ext_Write_ಗಮ್ಯ_U(cnv, cnv->shared_ದತ್ತಾಂಶ->mbcs.extIndexes,
                         value,
                         &p_ವಾದಗಳು->ಗುರಿ, p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ,
                         &p_ವಾದಗಳು->offsets, src_ಸೂಚ್ಯಂಕ,
                         p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹೊಂದಿಕೆ<0) {
        /* save state for partial match */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
        int32_t j;

        /* just _append_ the newly consumed input to preToU[] */
        s=p_ವಾದಗಳು->ಮೂಲ;
        ಹೊಂದಿಕೆ=-ಹೊಂದಿಕೆ;
        ಗಾಗಿ(j=cnv->pre_ಗಮ್ಯ_ULength; j<ಹೊಂದಿಕೆ; ++j) {
            cnv->pre_ಗಮ್ಯ_U[j]=*s++;
        }
        p_ವಾದಗಳು->ಮೂಲ=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_ಗಮ್ಯ_ULength=(int8_t)ಹೊಂದಿಕೆ;
    } ಇಲ್ಲದಿದ್ದರೆ /* match==0 */ {
        /*
         * no match
         *
         * We need to split the previous input into two parts:
         *
         * 1. The first codepage character is unmappable - that's how we got into
         *    trying the extension data in the first place.
         *    We need to move it from the preToU buffer
         *    to the error buffer, set an error code,
         *    and prepare the rest of the previous input for 2.
         *
         * 2. The rest of the previous input must be converted once we
         *    come back from the callback for the first character.
         *    At that time, we have to try again from scratch to convert
         *    these input characters.
         *    The replay will be handled by the ucnv.c conversion code.
         */

        /* move the first codepage character to the error field */
        uprv_memcpy(cnv->ಗಮ್ಯ_UBytes, cnv->pre_ಗಮ್ಯ_U, cnv->pre_ಗಮ್ಯ_UFirst_ಉದ್ದಳತೆ);
        cnv->ಗಮ್ಯ_ULength=cnv->pre_ಗಮ್ಯ_UFirst_ಉದ್ದಳತೆ;

        /* move the rest up inside the buffer */
        length=cnv->pre_ಗಮ್ಯ_ULength-cnv->pre_ಗಮ್ಯ_UFirst_ಉದ್ದಳತೆ;
        ಆದರೆ(length>0) {
            uprv_memmove(cnv->pre_ಗಮ್ಯ_U, cnv->pre_ಗಮ್ಯ_U+cnv->pre_ಗಮ್ಯ_UFirst_ಉದ್ದಳತೆ, length);
        }

        /* mark preToU for replay */
        cnv->pre_ಗಮ್ಯ_ULength=(int8_t)-length;

        /* set the error code for unassigned */
        *p_Error_ಸಂಕೇತ=U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
    }
}

/* from Unicode ------------------------------------------------------------- */

// Use roundtrips, "good one-way" mappings, and some normal fallbacks.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
extFromUUseMapping(UBool useFallback, uint32_t value, UChar32 ಮೊದಲ_CP) {
    ಹಿಂತಿರುಗಿ
        ((value&UCNV_EXT_FROM_U_ಸ್ಥಿತಿ_ಮುಖವಾಡ)!=0 ||
            FROM_U_USE_FALLBACK(useFallback, ಮೊದಲ_CP)) &&
        (value&UCNV_EXT_FROM_U_RESERVED_ಮುಖವಾಡ)==0;
}

/*
 * @return index of the char16_t, if found; else <0
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
ucnv_ext_ಕಂಡುಹಿಡಿ_From_U(ಸ್ಥಿರಾಂಕ char16_t *fromUSection, int32_t length, char16_t u) {
    int32_t i, ಆರಂಭ, ಎಲ್ಲೆ;

    /* binary search */
    ಆರಂಭ=0;
    ಎಲ್ಲೆ=length;
    ಗಾಗಿ(;;) {
        i=ಎಲ್ಲೆ-ಆರಂಭ;
        ಆದರೆ(i<=1) {
            ನಿಲ್ಲಿಸು; /*ಮುಗಿದ*/
        }
        /* start<limit-1 */

        ಆದರೆ(i<=4) {
            /* linear search for the last part */
            ಆದರೆ(u<=fromUSection[ಆರಂಭ]) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(++ಆರಂಭ<ಎಲ್ಲೆ && u<=fromUSection[ಆರಂಭ]) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(++ಆರಂಭ<ಎಲ್ಲೆ && u<=fromUSection[ಆರಂಭ]) {
                ನಿಲ್ಲಿಸು;
            }
            /* always break at start==limit-1 */
            ++ಆರಂಭ;
            ನಿಲ್ಲಿಸು;
        }

        i=(ಆರಂಭ+ಎಲ್ಲೆ)/2;
        ಆದರೆ(u<fromUSection[i]) {
            ಎಲ್ಲೆ=i;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆರಂಭ=i;
        }
    }

    /* did we really find it? */
    ಆದರೆ(ಆರಂಭ<ಎಲ್ಲೆ && u==fromUSection[ಆರಂಭ]) {
        ಹಿಂತಿರುಗಿ ಆರಂಭ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ -1; /* not found */
    }
}

/*
 * @param cx pointer to extension data; if nullptr, returns 0
 * @param firstCP the first code point before all the other UChars
 * @param pre UChars that must match; !initialMatch: partial match with them
 * @param preLength length of pre, >=0
 * @param src UChars that can be used to complete a match
 * @param srcLength length of src, >=0
 * @param pMatchValue [out] output result value for the match from the data structure
 * @param useFallback "use fallback" flag, usually from cnv->useFallback
 * @param flush true if the end of the input stream is reached
 * @return >1: matched, return value=total match length (number of input units matched)
 *          1: matched, no mapping but request for <subchar1>
 *             (only for the first code point)
 *          0: no match
 *         <0: partial match, return value=negative total match length
 *             (partial matches are never returned for flush==true)
 *             (partial matches are never returned as being longer than UCNV_EXT_MAX_UCHARS)
 *         the matchLength is 2 if only firstCP matched, and >2 if firstCP and
 *         further code units matched
 */
ಸ್ಥಿರ int32_t
ucnv_ext_ಹೊಂದಿಕೆ_From_U(ಸ್ಥಿರಾಂಕ int32_t *cx,
                   UChar32 ಮೊದಲ_CP,
                   ಸ್ಥಿರಾಂಕ char16_t *pre, int32_t pre_ಉದ್ದಳತೆ,
                   ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                   uint32_t *p_ಹೊಂದಿಕೆ_ಮೌಲ್ಯ,
                   UBool useFallback, UBool flush) {
    ಸ್ಥಿರಾಂಕ uint16_t *stage12, *stage3;
    ಸ್ಥಿರಾಂಕ uint32_t *stage3b;

    ಸ್ಥಿರಾಂಕ char16_t *fromUTableUChars, *fromUSectionUChars;
    ಸ್ಥಿರಾಂಕ uint32_t *from_UTable_ಮೌಲ್ಯಗಳು, *from_USection_ಮೌಲ್ಯಗಳು;

    uint32_t value, ಹೊಂದಿಕೆ_ಮೌಲ್ಯ;
    int32_t i, j, idx, length, ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;
    char16_t c;

    ಆದರೆ(cx==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 0; /* no extension data, no match */
    }

    /* trie lookup of firstCP */
    idx=ಮೊದಲ_CP>>10; /* stage 1 index */
    ಆದರೆ(idx>=cx[UCNV_EXT_FROM_U_STAGE_1_ಉದ್ದಳತೆ]) {
        ಹಿಂತಿರುಗಿ 0; /* the first code point is outside the trie */
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_ಸೂಚ್ಯಂಕ, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_ಸೂಚ್ಯಂಕ, uint16_t);
    idx=UCNV_EXT_FROM_U(stage12, stage3, idx, ಮೊದಲ_CP);

    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_ಸೂಚ್ಯಂಕ, uint32_t);
    value=stage3b[idx];
    ಆದರೆ(value==0) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /*
     * Tests for (value&UCNV_EXT_FROM_U_RESERVED_MASK)==0:
     * Do not interpret values with reserved bits used, for forward compatibility,
     * and do not even remember intermediate results with reserved bits used.
     */

    ಆದರೆ(UCNV_EXT_ಗಮ್ಯ_U_IS_PARTIAL(value)) {
        /* partial match, enter the loop below */
        idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_ಸೂಚ್ಯಂಕ(value));

        /* initialize */
        fromUTableUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_ಸೂಚ್ಯಂಕ, char16_t);
        from_UTable_ಮೌಲ್ಯಗಳು=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ, uint32_t);

        ಹೊಂದಿಕೆ_ಮೌಲ್ಯ=0;
        i=j=ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=0;

        /* we must not remember fallback matches when not using fallbacks */

        /* match input units until there is a full match or the input is consumed */
        ಗಾಗಿ(;;) {
            /* go to the next section */
            fromUSectionUChars=fromUTableUChars+idx;
            from_USection_ಮೌಲ್ಯಗಳು=from_UTable_ಮೌಲ್ಯಗಳು+idx;

            /* read first pair of the section */
            length=*fromUSectionUChars++;
            value=*from_USection_ಮೌಲ್ಯಗಳು++;
            ಆದರೆ(value!=0 && extFromUUseMapping(useFallback, value, ಮೊದಲ_CP)) {
                /* remember longest match so far */
                ಹೊಂದಿಕೆ_ಮೌಲ್ಯ=value;
                ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=2+i+j;
            }

            /* match pre[] then src[] */
            ಆದರೆ(i<pre_ಉದ್ದಳತೆ) {
                c=pre[i++];
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(j<src_ಉದ್ದಳತೆ) {
                c=src[j++];
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* all input consumed, partial match */
                ಆದರೆ(flush || (length=(i+j))>UCNV_EXT_MAX_UCHARS) {
                    /*
                     * end of the entire input stream, stop with the longest match so far
                     * or: partial match must not be longer than UCNV_EXT_MAX_UCHARS
                     * because it must fit into state buffers
                     */
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* continue with more input next time */
                    ಹಿಂತಿರುಗಿ -(2+length);
                }
            }

            /* search for the current char16_t */
            idx=ucnv_ext_ಕಂಡುಹಿಡಿ_From_U(fromUSectionUChars, length, c);
            ಆದರೆ(idx<0) {
                /* no match here, stop with the longest match so far */
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                value=from_USection_ಮೌಲ್ಯಗಳು[idx];
                ಆದರೆ(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                    /* partial match, continue */
                    idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_ಸೂಚ್ಯಂಕ(value));
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ(extFromUUseMapping(useFallback, value, ಮೊದಲ_CP)) {
                        /* full match, stop with result */
                        ಹೊಂದಿಕೆ_ಮೌಲ್ಯ=value;
                        ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=2+i+j;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* full match on fallback not taken, stop with the longest match so far */
                    }
                    ನಿಲ್ಲಿಸು;
                }
            }
        }

        ಆದರೆ(ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ==0) {
            /* no match at all */
            ಹಿಂತಿರುಗಿ 0;
        }
    } ಇಲ್ಲದಿದ್ದರೆ /* result from firstCP trie lookup */ {
        ಆದರೆ(extFromUUseMapping(useFallback, value, ಮೊದಲ_CP)) {
            /* full match, stop with result */
            ಹೊಂದಿಕೆ_ಮೌಲ್ಯ=value;
            ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* fallback not taken */
            ಹಿಂತಿರುಗಿ 0;
        }
    }

    /* return result */
    ಆದರೆ(ಹೊಂದಿಕೆ_ಮೌಲ್ಯ==UCNV_EXT_FROM_U_SUBCHAR1) {
        ಹಿಂತಿರುಗಿ 1; /* assert matchLength==2 */
    }

    *p_ಹೊಂದಿಕೆ_ಮೌಲ್ಯ=ಹೊಂದಿಕೆ_ಮೌಲ್ಯ;
    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;
}

/*
 * @param value fromUnicode mapping table value; ignores roundtrip and reserved bits
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
ucnv_extWriteFromU(UConverter *cnv, ಸ್ಥಿರಾಂಕ int32_t *cx,
                   uint32_t value,
                   ಅಕ್ಷರ **ಗುರಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ,
                   int32_t **offsets, int32_t src_ಸೂಚ್ಯಂಕ,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    uint8_t ಬಫರ್[1+UCNV_EXT_MAX_ಬೈಟುಗಳು];
    ಸ್ಥಿರಾಂಕ uint8_t *ಫಲಿತಾಂಶ;
    int32_t length, prev_ಉದ್ದಳತೆ;

    length=UCNV_EXT_FROM_U_GET_ಉದ್ದಳತೆ(value);
    value = UCNV_EXT_FROM_U_GET_ದತ್ತಾಂಶ(value);

    /* output the result */
    ಆದರೆ(length<=UCNV_EXT_FROM_U_MAX_ನೇರ_ಉದ್ದಳತೆ) {
        /*
         * Generate a byte array and then write it below.
         * This is not the fastest possible way, but it should be ok for
         * extension mappings, and it is much simpler.
         * Offset and overflow handling are only done once this way.
         */
        uint8_t *p=ಬಫರ್+1; /* reserve buffer[0] for shiftByte below */
        ಆಯ್ಕೆ(length) {
        ಸ್ಥಿತಿ 3:
            *p++ = static_cast<uint8_t>(value >> 16);
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 2:
            *p++ = static_cast<uint8_t>(value >> 8);
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 1:
            *p++ = static_cast<uint8_t>(value);
            U_FALLTHROUGH;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ನಿಲ್ಲಿಸು; /* will never occur */
        }
        ಫಲಿತಾಂಶ=ಬಫರ್+1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_ಬೈಟುಗಳು_ಸೂಚ್ಯಂಕ, uint8_t)+value;
    }

    /* with correct data we have length>0 */

    ಆದರೆ((prev_ಉದ್ದಳತೆ=cnv->from_Unicode_ಸ್ಥಿತಿ)!=0) {
        /* handle SI/SO stateful output */
        uint8_t ಸರಿಸು_Byte;

        ಆದರೆ(prev_ಉದ್ದಳತೆ>1 && length==1) {
            /* change from double-byte mode to single-byte */
            ಸರಿಸು_Byte = static_cast<uint8_t>(UCNV_SI);
            cnv->from_Unicode_ಸ್ಥಿತಿ=1;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(prev_ಉದ್ದಳತೆ==1 && length>1) {
            /* change from single-byte mode to double-byte */
            ಸರಿಸು_Byte = static_cast<uint8_t>(UCNV_SO);
            cnv->from_Unicode_ಸ್ಥಿತಿ=2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸರಿಸು_Byte=0;
        }

        ಆದರೆ(ಸರಿಸು_Byte!=0) {
            /* prepend the shift byte to the result bytes */
            ಬಫರ್[0]=ಸರಿಸು_Byte;
            ಆದರೆ(ಫಲಿತಾಂಶ!=ಬಫರ್+1) {
                uprv_memcpy(ಬಫರ್+1, ಫಲಿತಾಂಶ, length);
            }
            ಫಲಿತಾಂಶ=ಬಫರ್;
            ++length;
        }
    }

    ucnv_from_UWrite_ಬೈಟುಗಳು(cnv, reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಫಲಿತಾಂಶ), length,
                         ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ,
                         offsets, src_ಸೂಚ್ಯಂಕ,
                         p_Error_ಸಂಕೇತ);
}

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_ಹೊಂದಿಕೆ_From_U(UConverter *cnv, ಸ್ಥಿರಾಂಕ int32_t *cx,
                          UChar32 cp,
                          ಸ್ಥಿರಾಂಕ char16_t **src, ಸ್ಥಿರಾಂಕ char16_t *src_ಎಲ್ಲೆ,
                          ಅಕ್ಷರ **ಗುರಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ,
                          int32_t **offsets, int32_t src_ಸೂಚ್ಯಂಕ,
                          UBool flush,
                          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ಹೊಂದಿಕೆ;

    /* try to match */
    ಹೊಂದಿಕೆ=ucnv_ext_ಹೊಂದಿಕೆ_From_U(cx, cp,
                             ಶೂನ್ಯಸೂಚಕ, 0,
                             *src, (int32_t)(src_ಎಲ್ಲೆ-*src),
                             &value,
                             cnv->useFallback, flush);

    /* reject a match if the result is a single byte for DBCS-only */
    ಆದರೆ( ಹೊಂದಿಕೆ>=2 &&
        !(UCNV_EXT_FROM_U_GET_ಉದ್ದಳತೆ(value)==1 &&
          cnv->shared_ದತ್ತಾಂಶ->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY)
    ) {
        /* advance src pointer for the consumed input */
        *src+=ಹೊಂದಿಕೆ-2; /* remove 2 for the initial code point */

        /* write result to target */
        ucnv_extWriteFromU(cnv, cx,
                           value,
                           ಗುರಿ, ಗುರಿ_ಎಲ್ಲೆ,
                           offsets, src_ಸೂಚ್ಯಂಕ,
                           p_Error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ ನಿಜ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹೊಂದಿಕೆ<0) {
        /* save state for partial match */
        ಸ್ಥಿರಾಂಕ char16_t *s;
        int32_t j;

        /* copy the first code point */
        cnv->preFromUFirstCP=cp;

        /* now copy the newly consumed input */
        s=*src;
        ಹೊಂದಿಕೆ=-ಹೊಂದಿಕೆ-2; /* remove 2 for the initial code point */
        ಗಾಗಿ(j=0; j<ಹೊಂದಿಕೆ; ++j) {
            cnv->preFromU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)ಹೊಂದಿಕೆ;
        ಹಿಂತಿರುಗಿ ನಿಜ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹೊಂದಿಕೆ==1) {
        /* matched, no mapping but request for <subchar1> */
        cnv->useSubChar1=ನಿಜ;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ /* match==0 no match */ {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
}

/*
 * Used by ISO 2022 implementation.
 * @return number of bytes in *pValue; negative number if fallback; 0 for no mapping
 */
U_CFUNC int32_t
ucnv_ext_Simple_ಹೊಂದಿಕೆ_From_U(ಸ್ಥಿರಾಂಕ int32_t *cx,
                         UChar32 cp, uint32_t *p_ಮೌಲ್ಯ,
                         UBool useFallback) {
    uint32_t value;
    int32_t ಹೊಂದಿಕೆ;

    /* try to match */
    ಹೊಂದಿಕೆ=ucnv_ext_ಹೊಂದಿಕೆ_From_U(cx,
                             cp,
                             ಶೂನ್ಯಸೂಚಕ, 0,
                             ಶೂನ್ಯಸೂಚಕ, 0,
                             &value,
                             useFallback, ನಿಜ);
    ಆದರೆ(ಹೊಂದಿಕೆ>=2) {
        /* write result for simple, single-character conversion */
        int32_t length;
        ಪೂರ್ಣಾಂಕ isRoundtrip;

        isRoundtrip=UCNV_EXT_FROM_U_IS_ROUNDTRIP(value);
        length=UCNV_EXT_FROM_U_GET_ಉದ್ದಳತೆ(value);
        value = UCNV_EXT_FROM_U_GET_ದತ್ತಾಂಶ(value);

        ಆದರೆ(length<=UCNV_EXT_FROM_U_MAX_ನೇರ_ಉದ್ದಳತೆ) {
            *p_ಮೌಲ್ಯ=value;
            ಹಿಂತಿರುಗಿ isRoundtrip ? length : -length;
#ಆದರೆ 0 /* not currently used */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(length==4) {
            /* de-serialize a 4-byte result */
            ಸ್ಥಿರಾಂಕ uint8_t *ಫಲಿತಾಂಶ=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_ಬೈಟುಗಳು_ಸೂಚ್ಯಂಕ, uint8_t)+value;
            *p_ಮೌಲ್ಯ=
                ((uint32_t)ಫಲಿತಾಂಶ[0]<<24)|
                ((uint32_t)ಫಲಿತಾಂಶ[1]<<16)|
                ((uint32_t)ಫಲಿತಾಂಶ[2]<<8)|
                ಫಲಿತಾಂಶ[3];
            ಹಿಂತಿರುಗಿ isRoundtrip ? 4 : -4;
#ಆದರೆ_ಅಂತ್ಯ
        }
    }

    /*
     * return no match because
     * - match>1 && resultLength>4: result too long for simple conversion
     * - match==1: no match found, <subchar1> preferred
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * continue partial match with new input, requires cnv->preFromUFirstCP>=0
 * never called for simple, single-character conversion
 */
U_CFUNC ಶೂನ್ಯ
ucnv_ext_Continue_ಹೊಂದಿಕೆ_From_U(UConverter *cnv,
                           UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು, int32_t src_ಸೂಚ್ಯಂಕ,
                           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ಹೊಂದಿಕೆ;

    ಹೊಂದಿಕೆ=ucnv_ext_ಹೊಂದಿಕೆ_From_U(cnv->shared_ದತ್ತಾಂಶ->mbcs.extIndexes,
                             cnv->preFromUFirstCP,
                             cnv->preFromU, cnv->preFromULength,
                             p_ವಾದಗಳು->ಮೂಲ, (int32_t)(p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ-p_ವಾದಗಳು->ಮೂಲ),
                             &value,
                             cnv->useFallback, p_ವಾದಗಳು->flush);
    ಆದರೆ(ಹೊಂದಿಕೆ>=2) {
        ಹೊಂದಿಕೆ-=2; /* remove 2 for the initial code point */

        ಆದರೆ(ಹೊಂದಿಕೆ>=cnv->preFromULength) {
            /* advance src pointer for the consumed input */
            p_ವಾದಗಳು->ಮೂಲ+=ಹೊಂದಿಕೆ-cnv->preFromULength;
            cnv->preFromULength=0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* the match did not use all of preFromU[] - keep the rest for replay */
            int32_t length=cnv->preFromULength-ಹೊಂದಿಕೆ;
            u_memmove(cnv->preFromU, cnv->preFromU+ಹೊಂದಿಕೆ, length);
            cnv->preFromULength=(int8_t)-length;
        }

        /* finish the partial match */
        cnv->preFromUFirstCP=U_SENTINEL;

        /* write result */
        ucnv_extWriteFromU(cnv, cnv->shared_ದತ್ತಾಂಶ->mbcs.extIndexes,
                           value,
                           &p_ವಾದಗಳು->ಗುರಿ, p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ,
                           &p_ವಾದಗಳು->offsets, src_ಸೂಚ್ಯಂಕ,
                           p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹೊಂದಿಕೆ<0) {
        /* save state for partial match */
        ಸ್ಥಿರಾಂಕ char16_t *s;
        int32_t j;

        /* just _append_ the newly consumed input to preFromU[] */
        s=p_ವಾದಗಳು->ಮೂಲ;
        ಹೊಂದಿಕೆ=-ಹೊಂದಿಕೆ-2; /* remove 2 for the initial code point */
        ಗಾಗಿ(j=cnv->preFromULength; j<ಹೊಂದಿಕೆ; ++j) {
            U_ASSERT(j>=0);
            cnv->preFromU[j]=*s++;
        }
        p_ವಾದಗಳು->ಮೂಲ=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)ಹೊಂದಿಕೆ;
    } ಇಲ್ಲದಿದ್ದರೆ /* match==0 or 1 */ {
        /*
         * no match
         *
         * We need to split the previous input into two parts:
         *
         * 1. The first code point is unmappable - that's how we got into
         *    trying the extension data in the first place.
         *    We need to move it from the preFromU buffer
         *    to the error buffer, set an error code,
         *    and prepare the rest of the previous input for 2.
         *
         * 2. The rest of the previous input must be converted once we
         *    come back from the callback for the first code point.
         *    At that time, we have to try again from scratch to convert
         *    these input characters.
         *    The replay will be handled by the ucnv.c conversion code.
         */

        ಆದರೆ(ಹೊಂದಿಕೆ==1) {
            /* matched, no mapping but request for <subchar1> */
            cnv->useSubChar1=ನಿಜ;
        }

        /* move the first code point to the error field */
        cnv->fromUChar32=cnv->preFromUFirstCP;
        cnv->preFromUFirstCP=U_SENTINEL;

        /* mark preFromU for replay */
        cnv->preFromULength=-cnv->preFromULength;

        /* set the error code for unassigned */
        *p_Error_ಸಂಕೇತ=U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
    }
}

ಸ್ಥಿರ UBool
ext_ಗಣ_Use_Mapping(UConverter_Unicode_ಗಣ which, int32_t min_ಉದ್ದಳತೆ, uint32_t value) {
    ಆದರೆ(which==UCNV_ROUNDTRIP_ಗಣ) {
        // Add only code points for which the roundtrip flag is set.
        // Do not add any fallbacks, even if ucnv_fromUnicode() would use them
        // (fallbacks from PUA). See the API docs for ucnv_getUnicodeSet().
        //
        // By analogy, also do not add "good one-way" mappings.
        //
        // Do not add entries with reserved bits set.
        ಆದರೆ(((value&(UCNV_EXT_FROM_U_ROUNDTRIP_FLAG|UCNV_EXT_FROM_U_RESERVED_ಮುಖವಾಡ))!=
                UCNV_EXT_FROM_U_ROUNDTRIP_FLAG)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    } ಇಲ್ಲದಿದ್ದರೆ /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
        // Do not add entries with reserved bits set.
        ಆದರೆ((value&UCNV_EXT_FROM_U_RESERVED_ಮುಖವಾಡ)!=0) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    // Do not add <subchar1> entries or other (future?) pseudo-entries
    // with an output length of 0.
    ಹಿಂತಿರುಗಿ UCNV_EXT_FROM_U_GET_ಉದ್ದಳತೆ(value)>=min_ಉದ್ದಳತೆ;
}

ಸ್ಥಿರ ಶೂನ್ಯ
ucnv_ext_Get_Unicode_ಗಣ_String(ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ int32_t *cx,
                            ಸ್ಥಿರಾಂಕ USetAdder *sa,
                            UConverter_Unicode_ಗಣ which,
                            int32_t min_ಉದ್ದಳತೆ,
                            UChar32 ಮೊದಲ_CP,
                            char16_t s[UCNV_EXT_MAX_UCHARS], int32_t length,
                            int32_t section_ಸೂಚ್ಯಂಕ,
                            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ char16_t *fromUSectionUChars;
    ಸ್ಥಿರಾಂಕ uint32_t *from_USection_ಮೌಲ್ಯಗಳು;

    uint32_t value;
    int32_t i, count;

    fromUSectionUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_ಸೂಚ್ಯಂಕ, char16_t)+section_ಸೂಚ್ಯಂಕ;
    from_USection_ಮೌಲ್ಯಗಳು=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ, uint32_t)+section_ಸೂಚ್ಯಂಕ;

    /* read first pair of the section */
    count=*fromUSectionUChars++;
    value=*from_USection_ಮೌಲ್ಯಗಳು++;

    ಆದರೆ(ext_ಗಣ_Use_Mapping(which, min_ಉದ್ದಳತೆ, value)) {
        ಆದರೆ(length==U16_ಉದ್ದಳತೆ(ಮೊದಲ_CP)) {
            /* add the initial code point */
            sa->ಕೂಡಿಸು(sa->ಗಣ, ಮೊದಲ_CP);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* add the string so far */
            sa->ಕೂಡಿಸು_String(sa->ಗಣ, s, length);
        }
    }

    ಗಾಗಿ(i=0; i<count; ++i) {
        /* append this code unit and recurse or add the string */
        s[length]=fromUSectionUChars[i];
        value=from_USection_ಮೌಲ್ಯಗಳು[i];

        ಆದರೆ(value==0) {
            /* no mapping, do nothing */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
            ucnv_ext_Get_Unicode_ಗಣ_String(
                shared_ದತ್ತಾಂಶ, cx, sa, which, min_ಉದ್ದಳತೆ,
                ಮೊದಲ_CP, s, length+1,
                static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_ಸೂಚ್ಯಂಕ(value)),
                p_Error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ext_ಗಣ_Use_Mapping(which, min_ಉದ್ದಳತೆ, value)) {
            sa->ಕೂಡಿಸು_String(sa->ಗಣ, s, length+1);
        }
    }
}

U_CFUNC ಶೂನ್ಯ
ucnv_ext_Get_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ *shared_ದತ್ತಾಂಶ,
                      ಸ್ಥಿರಾಂಕ USetAdder *sa,
                      UConverter_Unicode_ಗಣ which,
                      UConverter_ಗಣ_Filter filter,
                      UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ int32_t *cx;
    ಸ್ಥಿರಾಂಕ uint16_t *stage12, *stage3, *ps2, *ps3;
    ಸ್ಥಿರಾಂಕ uint32_t *stage3b;

    uint32_t value;
    int32_t st1, stage1_ಉದ್ದಳತೆ, st2, st3, min_ಉದ್ದಳತೆ;

    char16_t s[UCNV_EXT_MAX_UCHARS];
    UChar32 c;
    int32_t length;

    cx=shared_ದತ್ತಾಂಶ->mbcs.extIndexes;
    ಆದರೆ(cx==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_ಸೂಚ್ಯಂಕ, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_ಸೂಚ್ಯಂಕ, uint16_t);
    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_ಸೂಚ್ಯಂಕ, uint32_t);

    stage1_ಉದ್ದಳತೆ=cx[UCNV_EXT_FROM_U_STAGE_1_ಉದ್ದಳತೆ];

    /* enumerate the from-Unicode trie table */
    c=0; /* keep track of the current code point while enumerating */

    ಆದರೆ(filter==UCNV_ಗಣ_FILTER_2022_CN) {
        min_ಉದ್ದಳತೆ=3;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ( shared_ದತ್ತಾಂಶ->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ||
               filter!=UCNV_ಗಣ_FILTER_NONE
    ) {
        /* DBCS-only, ignore single-byte results */
        min_ಉದ್ದಳತೆ=2;
    } ಇಲ್ಲದಿದ್ದರೆ {
        min_ಉದ್ದಳತೆ=1;
    }

    /*
     * the trie enumeration is almost the same as
     * in MBCSGetUnicodeSet() for MBCS_OUTPUT_1
     */
    ಗಾಗಿ(st1=0; st1<stage1_ಉದ್ದಳತೆ; ++st1) {
        st2=stage12[st1];
        ಆದರೆ(st2>stage1_ಉದ್ದಳತೆ) {
            ps2=stage12+st2;
            ಗಾಗಿ(st2=0; st2<64; ++st2) {
                ಆದರೆ((st3=(int32_t)ps2[st2]<<UCNV_EXT_STAGE_2_ಎಡ_ಸರಿಸು)!=0) {
                    /* read the stage 3 block */
                    ps3=stage3+st3;

                    ಮಾಡು {
                        value=stage3b[*ps3++];
                        ಆದರೆ(value==0) {
                            /* no mapping, do nothing */
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                            // Recurse for partial results.
                            length=0;
                            U16_APPEND_UNSAFE(s, length, c);
                            ucnv_ext_Get_Unicode_ಗಣ_String(
                                shared_ದತ್ತಾಂಶ, cx, sa, which, min_ಉದ್ದಳತೆ,
                                c, s, length,
                                (int32_t)UCNV_EXT_FROM_U_GET_PARTIAL_ಸೂಚ್ಯಂಕ(value),
                                p_Error_ಸಂಕೇತ);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ext_ಗಣ_Use_Mapping(which, min_ಉದ್ದಳತೆ, value)) {
                            ಆಯ್ಕೆ(filter) {
                            ಸ್ಥಿತಿ UCNV_ಗಣ_FILTER_2022_CN:
                                ಆದರೆ(!(UCNV_EXT_FROM_U_GET_ಉದ್ದಳತೆ(value)==3 && UCNV_EXT_FROM_U_GET_ದತ್ತಾಂಶ(value)<=0x82ffff)) {
                                    ಮುಂದುವರಿ;
                                }
                                ನಿಲ್ಲಿಸು;
                            ಸ್ಥಿತಿ UCNV_ಗಣ_FILTER_SJIS:
                                ಆದರೆ(!(UCNV_EXT_FROM_U_GET_ಉದ್ದಳತೆ(value)==2 && (value=UCNV_EXT_FROM_U_GET_ದತ್ತಾಂಶ(value))>=0x8140 && value<=0xeffc)) {
                                    ಮುಂದುವರಿ;
                                }
                                ನಿಲ್ಲಿಸು;
                            ಸ್ಥಿತಿ UCNV_ಗಣ_FILTER_GR94_DBCS:
                                ಆದರೆ(!(UCNV_EXT_FROM_U_GET_ಉದ್ದಳತೆ(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_ದತ್ತಾಂಶ(value))-0xa1a1)<=(0xfefe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    ಮುಂದುವರಿ;
                                }
                                ನಿಲ್ಲಿಸು;
                            ಸ್ಥಿತಿ UCNV_ಗಣ_FILTER_HZ:
                                ಆದರೆ(!(UCNV_EXT_FROM_U_GET_ಉದ್ದಳತೆ(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_ದತ್ತಾಂಶ(value))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    ಮುಂದುವರಿ;
                                }
                                ನಿಲ್ಲಿಸು;
                            ಪೂರ್ವನಿಯೋಜಿತ:
                                /*
                                 * UCNV_SET_FILTER_NONE,
                                 * or UCNV_SET_FILTER_DBCS_ONLY which is handled via minLength
                                 */
                                ನಿಲ್ಲಿಸು;
                            }
                            sa->ಕೂಡಿಸು(sa->ಗಣ, c);
                        }
                    } ಆಗ((++c&0xf)!=0);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    c+=16; /* empty stage 3 block */
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            c+=1024; /* empty stage 2 block */
        }
    }
}

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_LEGACY_CONVERSION */
