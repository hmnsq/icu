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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_ext.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

/* to Unicode --------------------------------------------------------------- */

/*
 * @return lookup value for the byte, if found; else 0
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t
ucnv_ext_ਲੱਭੋ_ਮੰਜ਼ਿਲ_U(ਅਚਲ uint32_t *ਮੰਜ਼ਿਲ_USection, int32_t length, uint8_t byte) {
    uint32_t word0, ਸ਼ਬਦ_2;
    int32_t i, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ;

    /* check the input byte against the lowest and highest section bytes */
    ਸ਼ੁਰੂ = static_cast<int32_t>(UCNV_EXT_ਮੰਜ਼ਿਲ_U_GET_BYTE(ਮੰਜ਼ਿਲ_USection[0]));
    ਹੱਦਬੰਦੀ = static_cast<int32_t>(UCNV_EXT_ਮੰਜ਼ਿਲ_U_GET_BYTE(ਮੰਜ਼ਿਲ_USection[length - 1]));
    ਜੇ(byte<ਸ਼ੁਰੂ || ਹੱਦਬੰਦੀ<byte) {
        ਵਾਪਸ 0; /* the byte is out of range */
    }

    ਜੇ(length==((ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ)+1)) {
        /* direct access on a linear array */
        ਵਾਪਸ UCNV_EXT_ਮੰਜ਼ਿਲ_U_GET_ਮੁੱਲ(ਮੰਜ਼ਿਲ_USection[byte-ਸ਼ੁਰੂ]); /* could be 0 */
    }

    /* word0 is suitable for <=toUSection[] comparison, word for <toUSection[] */
    word0=UCNV_EXT_ਮੰਜ਼ਿਲ_U_MAKE_ਸ਼ਬਦ(byte, 0);

    /*
     * Shift byte once instead of each section word and add 0xffffff.
     * We will compare the shifted/added byte (bbffffff) against
     * section words which have byte values in the same bit position.
     * If and only if byte bb < section byte ss then bbffffff<ssvvvvvv
     * for all v=0..f
     * so we need not mask off the lower 24 bits of each section word.
     */
    ਸ਼ਬਦ_2=word0|UCNV_EXT_ਮੰਜ਼ਿਲ_U_ਮੁੱਲ_ਨਕਾਬ;

    /* binary search */
    ਸ਼ੁਰੂ=0;
    ਹੱਦਬੰਦੀ=length;
    ਲਈ(;;) {
        i=ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ;
        ਜੇ(i<=1) {
            ਰੋਕੋ; /*ਪੂਰਾ*/
        }
        /* start<limit-1 */

        ਜੇ(i<=4) {
            /* linear search for the last part */
            ਜੇ(word0<=ਮੰਜ਼ਿਲ_USection[ਸ਼ੁਰੂ]) {
                ਰੋਕੋ;
            }
            ਜੇ(++ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ && word0<=ਮੰਜ਼ਿਲ_USection[ਸ਼ੁਰੂ]) {
                ਰੋਕੋ;
            }
            ਜੇ(++ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ && word0<=ਮੰਜ਼ਿਲ_USection[ਸ਼ੁਰੂ]) {
                ਰੋਕੋ;
            }
            /* always break at start==limit-1 */
            ++ਸ਼ੁਰੂ;
            ਰੋਕੋ;
        }

        i=(ਸ਼ੁਰੂ+ਹੱਦਬੰਦੀ)/2;
        ਜੇ(ਸ਼ਬਦ_2<ਮੰਜ਼ਿਲ_USection[i]) {
            ਹੱਦਬੰਦੀ=i;
        } ਨਹੀਂਤਾਂ {
            ਸ਼ੁਰੂ=i;
        }
    }

    /* did we really find it? */
    ਜੇ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ && byte==UCNV_EXT_ਮੰਜ਼ਿਲ_U_GET_BYTE(ਸ਼ਬਦ_2=ਮੰਜ਼ਿਲ_USection[ਸ਼ੁਰੂ])) {
        ਵਾਪਸ UCNV_EXT_ਮੰਜ਼ਿਲ_U_GET_ਮੁੱਲ(ਸ਼ਬਦ_2); /* never 0 */
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0; /* not found */
    }
}

/*
 * true if not an SI/SO stateful converter,
 * or if the match length fits with the current converter state
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_EXT_ਮੰਜ਼ਿਲ_U_VERIFY_SISO_ਮੇਲ(sisoState, ਮੇਲ) \
    ((sisoState)<0 || ((sisoState)==0) == (ਮੇਲ==1))

/*
 * this works like ucnv_extMatchFromU() except
 * - the first character is in pre
 * - no trie is used
 * - the returned matchLength is not offset by 2
 */
ਸਥਿਰ int32_t
ucnv_ext_ਮੇਲ_ਮੰਜ਼ਿਲ_U(ਅਚਲ int32_t *cx, int8_t sisoState,
                 ਅਚਲ ਅੱਖਰ *pre, int32_t pre_ਲੰਮਾਈ,
                 ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
                 uint32_t *p_ਮੇਲ_ਮੁੱਲ,
                 UBool /*useFallback*/, UBool flush) {
    ਅਚਲ uint32_t *ਮੰਜ਼ਿਲ_UTable, *ਮੰਜ਼ਿਲ_USection;

    uint32_t value, ਮੇਲ_ਮੁੱਲ;
    int32_t i, j, idx, length, ਮੇਲ_ਲੰਮਾਈ;
    uint8_t b;

    ਜੇ(cx==ਖਾਲੀ_ਸੂਚਕ || cx[UCNV_EXT_ਮੰਜ਼ਿਲ_U_ਲੰਮਾਈ]<=0) {
        ਵਾਪਸ 0; /* no extension data, no match */
    }

    /* initialize */
    ਮੰਜ਼ਿਲ_UTable=UCNV_EXT_ARRAY(cx, UCNV_EXT_ਮੰਜ਼ਿਲ_U_ਸੂਚਕਾਂਕ, uint32_t);
    idx=0;

    ਮੇਲ_ਮੁੱਲ=0;
    i=j=ਮੇਲ_ਲੰਮਾਈ=0;

    ਜੇ(sisoState==0) {
        /* SBCS state of an SI/SO stateful converter, look at only exactly 1 byte */
        ਜੇ(pre_ਲੰਮਾਈ>1) {
            ਵਾਪਸ 0; /* no match of a DBCS sequence in SBCS mode */
        } ਨਹੀਂਤਾਂ ਜੇ(pre_ਲੰਮਾਈ==1) {
            src_ਲੰਮਾਈ=0;
        } ਨਹੀਂਤਾਂ /* preLength==0 */ {
            ਜੇ(src_ਲੰਮਾਈ>1) {
                src_ਲੰਮਾਈ=1;
            }
        }
        flush=ਸੱਚ;
    }

    /* we must not remember fallback matches when not using fallbacks */

    /* match input units until there is a full match or the input is consumed */
    ਲਈ(;;) {
        /* go to the next section */
        ਮੰਜ਼ਿਲ_USection=ਮੰਜ਼ਿਲ_UTable+idx;

        /* read first pair of the section */
        value=*ਮੰਜ਼ਿਲ_USection++;
        length=UCNV_EXT_ਮੰਜ਼ਿਲ_U_GET_BYTE(value);
        value=UCNV_EXT_ਮੰਜ਼ਿਲ_U_GET_ਮੁੱਲ(value);
        ਜੇ( value!=0 &&
            (UCNV_EXT_ਮੰਜ਼ਿਲ_U_IS_ROUNDTRIP(value) ||
             ਮੰਜ਼ਿਲ_U_USE_FALLBACK(useFallback)) &&
            UCNV_EXT_ਮੰਜ਼ਿਲ_U_VERIFY_SISO_ਮੇਲ(sisoState, i+j)
        ) {
            /* remember longest match so far */
            ਮੇਲ_ਮੁੱਲ=value;
            ਮੇਲ_ਲੰਮਾਈ=i+j;
        }

        /* match pre[] then src[] */
        ਜੇ(i<pre_ਲੰਮਾਈ) {
            b = static_cast<uint8_t>(pre[i++]);
        } ਨਹੀਂਤਾਂ ਜੇ(j<src_ਲੰਮਾਈ) {
            b = static_cast<uint8_t>(src[j++]);
        } ਨਹੀਂਤਾਂ {
            /* all input consumed, partial match */
            ਜੇ(flush || (length=(i+j))>UCNV_EXT_MAX_ਬਾਈਟਾਂ) {
                /*
                 * end of the entire input stream, stop with the longest match so far
                 * or: partial match must not be longer than UCNV_EXT_MAX_BYTES
                 * because it must fit into state buffers
                 */
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ {
                /* continue with more input next time */
                ਵਾਪਸ -length;
            }
        }

        /* search for the current char16_t */
        value=ucnv_ext_ਲੱਭੋ_ਮੰਜ਼ਿਲ_U(ਮੰਜ਼ਿਲ_USection, length, b);
        ਜੇ(value==0) {
            /* no match here, stop with the longest match so far */
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ {
            ਜੇ(UCNV_EXT_ਮੰਜ਼ਿਲ_U_IS_PARTIAL(value)) {
                /* partial match, continue */
                idx = static_cast<int32_t>(UCNV_EXT_ਮੰਜ਼ਿਲ_U_GET_PARTIAL_ਸੂਚਕਾਂਕ(value));
            } ਨਹੀਂਤਾਂ {
                ਜੇ( (UCNV_EXT_ਮੰਜ਼ਿਲ_U_IS_ROUNDTRIP(value) ||
                     ਮੰਜ਼ਿਲ_U_USE_FALLBACK(useFallback)) &&
                    UCNV_EXT_ਮੰਜ਼ਿਲ_U_VERIFY_SISO_ਮੇਲ(sisoState, i+j)
                ) {
                    /* full match, stop with result */
                    ਮੇਲ_ਮੁੱਲ=value;
                    ਮੇਲ_ਲੰਮਾਈ=i+j;
                } ਨਹੀਂਤਾਂ {
                    /* full match on fallback not taken, stop with the longest match so far */
                }
                ਰੋਕੋ;
            }
        }
    }

    ਜੇ(ਮੇਲ_ਲੰਮਾਈ==0) {
        /* no match at all */
        ਵਾਪਸ 0;
    }

    /* return result */
    *p_ਮੇਲ_ਮੁੱਲ=UCNV_EXT_ਮੰਜ਼ਿਲ_U_ਨਕਾਬ_ROUNDTRIP(ਮੇਲ_ਮੁੱਲ);
    ਵਾਪਸ ਮੇਲ_ਲੰਮਾਈ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
ucnv_ext_Write_ਮੰਜ਼ਿਲ_U(UConverter *cnv, ਅਚਲ int32_t *cx,
                 uint32_t value,
                 char16_t **ਨਿਸ਼ਾਨਾ, ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                 int32_t **offsets, int32_t src_ਸੂਚਕਾਂਕ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    /* output the result */
    ਜੇ(UCNV_EXT_ਮੰਜ਼ਿਲ_U_IS_ਕੋਡ_ਬਿੰਦੂ(value)) {
        /* output a single code point */
        ucnv_ਮੰਜ਼ਿਲ_UWrite_ਕੋਡ_ਬਿੰਦੂ(
            cnv, UCNV_EXT_ਮੰਜ਼ਿਲ_U_GET_ਕੋਡ_ਬਿੰਦੂ(value),
            ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
            offsets, src_ਸੂਚਕਾਂਕ,
            p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        /* output a string - with correct data we have resultLength>0 */
        ucnv_ਮੰਜ਼ਿਲ_UWrite_UChars(
            cnv,
            UCNV_EXT_ARRAY(cx, UCNV_EXT_ਮੰਜ਼ਿਲ_U_UCHARS_ਸੂਚਕਾਂਕ, char16_t)+
                UCNV_EXT_ਮੰਜ਼ਿਲ_U_GET_ਸੂਚਕਾਂਕ(value),
            UCNV_EXT_ਮੰਜ਼ਿਲ_U_GET_ਲੰਮਾਈ(value),
            ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
            offsets, src_ਸੂਚਕਾਂਕ,
            p_Error_ਕੋਡ);
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_SISO_STATE(cnv) \
    ((cnv)->shared_ਅੰਕੜੇ->mbcs.outputType==MBCS_OUTPUT_2_SISO ? (int8_t)(cnv)->ਬਹੁਲਕ : \
     (cnv)->shared_ਅੰਕੜੇ->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ? 1 : -1)

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_ਮੇਲ_ਮੰਜ਼ਿਲ_U(UConverter *cnv, ਅਚਲ int32_t *cx,
                        int32_t ਪਹਿਲਾ_ਲੰਮਾਈ,
                        ਅਚਲ ਅੱਖਰ **src, ਅਚਲ ਅੱਖਰ *src_ਹੱਦਬੰਦੀ,
                        char16_t **ਨਿਸ਼ਾਨਾ, ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                        int32_t **offsets, int32_t src_ਸੂਚਕਾਂਕ,
                        UBool flush,
                        UError_ਕੋਡ *p_Error_ਕੋਡ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ਮੇਲ;

    /* try to match */
    ਮੇਲ=ucnv_ext_ਮੇਲ_ਮੰਜ਼ਿਲ_U(cx, (int8_t)UCNV_SISO_STATE(cnv),
                           (ਅਚਲ ਅੱਖਰ *)cnv->ਮੰਜ਼ਿਲ_UBytes, ਪਹਿਲਾ_ਲੰਮਾਈ,
                           *src, (int32_t)(src_ਹੱਦਬੰਦੀ-*src),
                           &value,
                           cnv->useFallback, flush);
    ਜੇ(ਮੇਲ>0) {
        /* advance src pointer for the consumed input */
        *src+=ਮੇਲ-ਪਹਿਲਾ_ਲੰਮਾਈ;

        /* write result to target */
        ucnv_ext_Write_ਮੰਜ਼ਿਲ_U(cnv, cx,
                         value,
                         ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                         offsets, src_ਸੂਚਕਾਂਕ,
                         p_Error_ਕੋਡ);
        ਵਾਪਸ ਸੱਚ;
    } ਨਹੀਂਤਾਂ ਜੇ(ਮੇਲ<0) {
        /* save state for partial match */
        ਅਚਲ ਅੱਖਰ *s;
        int32_t j;

        /* copy the first code point */
        s=(ਅਚਲ ਅੱਖਰ *)cnv->ਮੰਜ਼ਿਲ_UBytes;
        cnv->pre_ਮੰਜ਼ਿਲ_UFirst_ਲੰਮਾਈ=(int8_t)ਪਹਿਲਾ_ਲੰਮਾਈ;
        ਲਈ(j=0; j<ਪਹਿਲਾ_ਲੰਮਾਈ; ++j) {
            cnv->pre_ਮੰਜ਼ਿਲ_U[j]=*s++;
        }

        /* now copy the newly consumed input */
        s=*src;
        ਮੇਲ=-ਮੇਲ;
        ਲਈ(; j<ਮੇਲ; ++j) {
            cnv->pre_ਮੰਜ਼ਿਲ_U[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_ਮੰਜ਼ਿਲ_ULength=(int8_t)ਮੇਲ;
        ਵਾਪਸ ਸੱਚ;
    } ਨਹੀਂਤਾਂ /* match==0 no match */ {
        ਵਾਪਸ ਝੂਠ;
    }
}

U_CFUNC UChar32
ucnv_ext_Simple_ਮੇਲ_ਮੰਜ਼ਿਲ_U(ਅਚਲ int32_t *cx,
                       ਅਚਲ ਅੱਖਰ *ਸਰੋਤ, int32_t length,
                       UBool useFallback) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ਮੇਲ;

    ਜੇ(length<=0) {
        ਵਾਪਸ 0xffff;
    }

    /* try to match */
    ਮੇਲ=ucnv_ext_ਮੇਲ_ਮੰਜ਼ਿਲ_U(cx, -1,
                           ਸਰੋਤ, length,
                           ਖਾਲੀ_ਸੂਚਕ, 0,
                           &value,
                           useFallback, ਸੱਚ);
    ਜੇ(ਮੇਲ==length) {
        /* write result for simple, single-character conversion */
        ਜੇ(UCNV_EXT_ਮੰਜ਼ਿਲ_U_IS_ਕੋਡ_ਬਿੰਦੂ(value)) {
            ਵਾਪਸ UCNV_EXT_ਮੰਜ਼ਿਲ_U_GET_ਕੋਡ_ਬਿੰਦੂ(value);
        }
    }

    /*
     * return no match because
     * - match>0 && value points to string: simple conversion cannot handle multiple code points
     * - match>0 && match!=length: not all input consumed, forbidden for this function
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    ਵਾਪਸ 0xfffe;
}

/*
 * continue partial match with new input
 * never called for simple, single-character conversion
 */
U_CFUNC ਖਾਲੀ
ucnv_ext_Continue_ਮੇਲ_ਮੰਜ਼ਿਲ_U(UConverter *cnv,
                         UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, int32_t src_ਸੂਚਕਾਂਕ,
                         UError_ਕੋਡ *p_Error_ਕੋਡ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ਮੇਲ, length;

    ਮੇਲ=ucnv_ext_ਮੇਲ_ਮੰਜ਼ਿਲ_U(cnv->shared_ਅੰਕੜੇ->mbcs.extIndexes, (int8_t)UCNV_SISO_STATE(cnv),
                           cnv->pre_ਮੰਜ਼ਿਲ_U, cnv->pre_ਮੰਜ਼ਿਲ_ULength,
                           p_ਦਲੀਲਾਂ->ਸਰੋਤ, (int32_t)(p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ-p_ਦਲੀਲਾਂ->ਸਰੋਤ),
                           &value,
                           cnv->useFallback, p_ਦਲੀਲਾਂ->flush);
    ਜੇ(ਮੇਲ>0) {
        ਜੇ(ਮੇਲ>=cnv->pre_ਮੰਜ਼ਿਲ_ULength) {
            /* advance src pointer for the consumed input */
            p_ਦਲੀਲਾਂ->ਸਰੋਤ+=ਮੇਲ-cnv->pre_ਮੰਜ਼ਿਲ_ULength;
            cnv->pre_ਮੰਜ਼ਿਲ_ULength=0;
        } ਨਹੀਂਤਾਂ {
            /* the match did not use all of preToU[] - keep the rest for replay */
            length=cnv->pre_ਮੰਜ਼ਿਲ_ULength-ਮੇਲ;
            uprv_memmove(cnv->pre_ਮੰਜ਼ਿਲ_U, cnv->pre_ਮੰਜ਼ਿਲ_U+ਮੇਲ, length);
            cnv->pre_ਮੰਜ਼ਿਲ_ULength=(int8_t)-length;
        }

        /* write result */
        ucnv_ext_Write_ਮੰਜ਼ਿਲ_U(cnv, cnv->shared_ਅੰਕੜੇ->mbcs.extIndexes,
                         value,
                         &p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ, p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                         &p_ਦਲੀਲਾਂ->offsets, src_ਸੂਚਕਾਂਕ,
                         p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ ਜੇ(ਮੇਲ<0) {
        /* save state for partial match */
        ਅਚਲ ਅੱਖਰ *s;
        int32_t j;

        /* just _append_ the newly consumed input to preToU[] */
        s=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
        ਮੇਲ=-ਮੇਲ;
        ਲਈ(j=cnv->pre_ਮੰਜ਼ਿਲ_ULength; j<ਮੇਲ; ++j) {
            cnv->pre_ਮੰਜ਼ਿਲ_U[j]=*s++;
        }
        p_ਦਲੀਲਾਂ->ਸਰੋਤ=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_ਮੰਜ਼ਿਲ_ULength=(int8_t)ਮੇਲ;
    } ਨਹੀਂਤਾਂ /* match==0 */ {
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
        uprv_memcpy(cnv->ਮੰਜ਼ਿਲ_UBytes, cnv->pre_ਮੰਜ਼ਿਲ_U, cnv->pre_ਮੰਜ਼ਿਲ_UFirst_ਲੰਮਾਈ);
        cnv->ਮੰਜ਼ਿਲ_ULength=cnv->pre_ਮੰਜ਼ਿਲ_UFirst_ਲੰਮਾਈ;

        /* move the rest up inside the buffer */
        length=cnv->pre_ਮੰਜ਼ਿਲ_ULength-cnv->pre_ਮੰਜ਼ਿਲ_UFirst_ਲੰਮਾਈ;
        ਜੇ(length>0) {
            uprv_memmove(cnv->pre_ਮੰਜ਼ਿਲ_U, cnv->pre_ਮੰਜ਼ਿਲ_U+cnv->pre_ਮੰਜ਼ਿਲ_UFirst_ਲੰਮਾਈ, length);
        }

        /* mark preToU for replay */
        cnv->pre_ਮੰਜ਼ਿਲ_ULength=(int8_t)-length;

        /* set the error code for unassigned */
        *p_Error_ਕੋਡ=U_INVALID_CHAR_ਮਿਲਿਆ;
    }
}

/* from Unicode ------------------------------------------------------------- */

// Use roundtrips, "good one-way" mappings, and some normal fallbacks.
ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
extFromUUseMapping(UBool useFallback, uint32_t value, UChar32 ਪਹਿਲਾ_CP) {
    ਵਾਪਸ
        ((value&UCNV_EXT_FROM_U_ਸਥਿਤੀ_ਨਕਾਬ)!=0 ||
            FROM_U_USE_FALLBACK(useFallback, ਪਹਿਲਾ_CP)) &&
        (value&UCNV_EXT_FROM_U_RESERVED_ਨਕਾਬ)==0;
}

/*
 * @return index of the char16_t, if found; else <0
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
ucnv_ext_ਲੱਭੋ_From_U(ਅਚਲ char16_t *fromUSection, int32_t length, char16_t u) {
    int32_t i, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ;

    /* binary search */
    ਸ਼ੁਰੂ=0;
    ਹੱਦਬੰਦੀ=length;
    ਲਈ(;;) {
        i=ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ;
        ਜੇ(i<=1) {
            ਰੋਕੋ; /*ਪੂਰਾ*/
        }
        /* start<limit-1 */

        ਜੇ(i<=4) {
            /* linear search for the last part */
            ਜੇ(u<=fromUSection[ਸ਼ੁਰੂ]) {
                ਰੋਕੋ;
            }
            ਜੇ(++ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ && u<=fromUSection[ਸ਼ੁਰੂ]) {
                ਰੋਕੋ;
            }
            ਜੇ(++ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ && u<=fromUSection[ਸ਼ੁਰੂ]) {
                ਰੋਕੋ;
            }
            /* always break at start==limit-1 */
            ++ਸ਼ੁਰੂ;
            ਰੋਕੋ;
        }

        i=(ਸ਼ੁਰੂ+ਹੱਦਬੰਦੀ)/2;
        ਜੇ(u<fromUSection[i]) {
            ਹੱਦਬੰਦੀ=i;
        } ਨਹੀਂਤਾਂ {
            ਸ਼ੁਰੂ=i;
        }
    }

    /* did we really find it? */
    ਜੇ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ && u==fromUSection[ਸ਼ੁਰੂ]) {
        ਵਾਪਸ ਸ਼ੁਰੂ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ -1; /* not found */
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
ਸਥਿਰ int32_t
ucnv_ext_ਮੇਲ_From_U(ਅਚਲ int32_t *cx,
                   UChar32 ਪਹਿਲਾ_CP,
                   ਅਚਲ char16_t *pre, int32_t pre_ਲੰਮਾਈ,
                   ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                   uint32_t *p_ਮੇਲ_ਮੁੱਲ,
                   UBool useFallback, UBool flush) {
    ਅਚਲ uint16_t *stage12, *stage3;
    ਅਚਲ uint32_t *stage3b;

    ਅਚਲ char16_t *fromUTableUChars, *fromUSectionUChars;
    ਅਚਲ uint32_t *from_UTable_ਮੁੱਲਾਂ, *from_USection_ਮੁੱਲਾਂ;

    uint32_t value, ਮੇਲ_ਮੁੱਲ;
    int32_t i, j, idx, length, ਮੇਲ_ਲੰਮਾਈ;
    char16_t c;

    ਜੇ(cx==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 0; /* no extension data, no match */
    }

    /* trie lookup of firstCP */
    idx=ਪਹਿਲਾ_CP>>10; /* stage 1 index */
    ਜੇ(idx>=cx[UCNV_EXT_FROM_U_STAGE_1_ਲੰਮਾਈ]) {
        ਵਾਪਸ 0; /* the first code point is outside the trie */
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_ਸੂਚਕਾਂਕ, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_ਸੂਚਕਾਂਕ, uint16_t);
    idx=UCNV_EXT_FROM_U(stage12, stage3, idx, ਪਹਿਲਾ_CP);

    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_ਸੂਚਕਾਂਕ, uint32_t);
    value=stage3b[idx];
    ਜੇ(value==0) {
        ਵਾਪਸ 0;
    }

    /*
     * Tests for (value&UCNV_EXT_FROM_U_RESERVED_MASK)==0:
     * Do not interpret values with reserved bits used, for forward compatibility,
     * and do not even remember intermediate results with reserved bits used.
     */

    ਜੇ(UCNV_EXT_ਮੰਜ਼ਿਲ_U_IS_PARTIAL(value)) {
        /* partial match, enter the loop below */
        idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_ਸੂਚਕਾਂਕ(value));

        /* initialize */
        fromUTableUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_ਸੂਚਕਾਂਕ, char16_t);
        from_UTable_ਮੁੱਲਾਂ=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_ਮੁੱਲਾਂ_ਸੂਚਕਾਂਕ, uint32_t);

        ਮੇਲ_ਮੁੱਲ=0;
        i=j=ਮੇਲ_ਲੰਮਾਈ=0;

        /* we must not remember fallback matches when not using fallbacks */

        /* match input units until there is a full match or the input is consumed */
        ਲਈ(;;) {
            /* go to the next section */
            fromUSectionUChars=fromUTableUChars+idx;
            from_USection_ਮੁੱਲਾਂ=from_UTable_ਮੁੱਲਾਂ+idx;

            /* read first pair of the section */
            length=*fromUSectionUChars++;
            value=*from_USection_ਮੁੱਲਾਂ++;
            ਜੇ(value!=0 && extFromUUseMapping(useFallback, value, ਪਹਿਲਾ_CP)) {
                /* remember longest match so far */
                ਮੇਲ_ਮੁੱਲ=value;
                ਮੇਲ_ਲੰਮਾਈ=2+i+j;
            }

            /* match pre[] then src[] */
            ਜੇ(i<pre_ਲੰਮਾਈ) {
                c=pre[i++];
            } ਨਹੀਂਤਾਂ ਜੇ(j<src_ਲੰਮਾਈ) {
                c=src[j++];
            } ਨਹੀਂਤਾਂ {
                /* all input consumed, partial match */
                ਜੇ(flush || (length=(i+j))>UCNV_EXT_MAX_UCHARS) {
                    /*
                     * end of the entire input stream, stop with the longest match so far
                     * or: partial match must not be longer than UCNV_EXT_MAX_UCHARS
                     * because it must fit into state buffers
                     */
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ {
                    /* continue with more input next time */
                    ਵਾਪਸ -(2+length);
                }
            }

            /* search for the current char16_t */
            idx=ucnv_ext_ਲੱਭੋ_From_U(fromUSectionUChars, length, c);
            ਜੇ(idx<0) {
                /* no match here, stop with the longest match so far */
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ {
                value=from_USection_ਮੁੱਲਾਂ[idx];
                ਜੇ(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                    /* partial match, continue */
                    idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_ਸੂਚਕਾਂਕ(value));
                } ਨਹੀਂਤਾਂ {
                    ਜੇ(extFromUUseMapping(useFallback, value, ਪਹਿਲਾ_CP)) {
                        /* full match, stop with result */
                        ਮੇਲ_ਮੁੱਲ=value;
                        ਮੇਲ_ਲੰਮਾਈ=2+i+j;
                    } ਨਹੀਂਤਾਂ {
                        /* full match on fallback not taken, stop with the longest match so far */
                    }
                    ਰੋਕੋ;
                }
            }
        }

        ਜੇ(ਮੇਲ_ਲੰਮਾਈ==0) {
            /* no match at all */
            ਵਾਪਸ 0;
        }
    } ਨਹੀਂਤਾਂ /* result from firstCP trie lookup */ {
        ਜੇ(extFromUUseMapping(useFallback, value, ਪਹਿਲਾ_CP)) {
            /* full match, stop with result */
            ਮੇਲ_ਮੁੱਲ=value;
            ਮੇਲ_ਲੰਮਾਈ=2;
        } ਨਹੀਂਤਾਂ {
            /* fallback not taken */
            ਵਾਪਸ 0;
        }
    }

    /* return result */
    ਜੇ(ਮੇਲ_ਮੁੱਲ==UCNV_EXT_FROM_U_SUBCHAR1) {
        ਵਾਪਸ 1; /* assert matchLength==2 */
    }

    *p_ਮੇਲ_ਮੁੱਲ=ਮੇਲ_ਮੁੱਲ;
    ਵਾਪਸ ਮੇਲ_ਲੰਮਾਈ;
}

/*
 * @param value fromUnicode mapping table value; ignores roundtrip and reserved bits
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
ucnv_extWriteFromU(UConverter *cnv, ਅਚਲ int32_t *cx,
                   uint32_t value,
                   ਅੱਖਰ **ਨਿਸ਼ਾਨਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                   int32_t **offsets, int32_t src_ਸੂਚਕਾਂਕ,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    uint8_t ਬਫਰ[1+UCNV_EXT_MAX_ਬਾਈਟਾਂ];
    ਅਚਲ uint8_t *ਨਤੀਜਾ;
    int32_t length, prev_ਲੰਮਾਈ;

    length=UCNV_EXT_FROM_U_GET_ਲੰਮਾਈ(value);
    value = UCNV_EXT_FROM_U_GET_ਅੰਕੜੇ(value);

    /* output the result */
    ਜੇ(length<=UCNV_EXT_FROM_U_MAX_ਸਿੱਧਾ_ਲੰਮਾਈ) {
        /*
         * Generate a byte array and then write it below.
         * This is not the fastest possible way, but it should be ok for
         * extension mappings, and it is much simpler.
         * Offset and overflow handling are only done once this way.
         */
        uint8_t *p=ਬਫਰ+1; /* reserve buffer[0] for shiftByte below */
        ਚੋਣ(length) {
        ਹਾਲਤ 3:
            *p++ = static_cast<uint8_t>(value >> 16);
            U_FALLTHROUGH;
        ਹਾਲਤ 2:
            *p++ = static_cast<uint8_t>(value >> 8);
            U_FALLTHROUGH;
        ਹਾਲਤ 1:
            *p++ = static_cast<uint8_t>(value);
            U_FALLTHROUGH;
        ਮੂਲ:
            ਰੋਕੋ; /* will never occur */
        }
        ਨਤੀਜਾ=ਬਫਰ+1;
    } ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_ਬਾਈਟਾਂ_ਸੂਚਕਾਂਕ, uint8_t)+value;
    }

    /* with correct data we have length>0 */

    ਜੇ((prev_ਲੰਮਾਈ=cnv->from_Unicode_ਸਥਿਤੀ)!=0) {
        /* handle SI/SO stateful output */
        uint8_t ਖਿਸਕਾਓ_Byte;

        ਜੇ(prev_ਲੰਮਾਈ>1 && length==1) {
            /* change from double-byte mode to single-byte */
            ਖਿਸਕਾਓ_Byte = static_cast<uint8_t>(UCNV_SI);
            cnv->from_Unicode_ਸਥਿਤੀ=1;
        } ਨਹੀਂਤਾਂ ਜੇ(prev_ਲੰਮਾਈ==1 && length>1) {
            /* change from single-byte mode to double-byte */
            ਖਿਸਕਾਓ_Byte = static_cast<uint8_t>(UCNV_SO);
            cnv->from_Unicode_ਸਥਿਤੀ=2;
        } ਨਹੀਂਤਾਂ {
            ਖਿਸਕਾਓ_Byte=0;
        }

        ਜੇ(ਖਿਸਕਾਓ_Byte!=0) {
            /* prepend the shift byte to the result bytes */
            ਬਫਰ[0]=ਖਿਸਕਾਓ_Byte;
            ਜੇ(ਨਤੀਜਾ!=ਬਫਰ+1) {
                uprv_memcpy(ਬਫਰ+1, ਨਤੀਜਾ, length);
            }
            ਨਤੀਜਾ=ਬਫਰ;
            ++length;
        }
    }

    ucnv_from_UWrite_ਬਾਈਟਾਂ(cnv, reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਨਤੀਜਾ), length,
                         ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                         offsets, src_ਸੂਚਕਾਂਕ,
                         p_Error_ਕੋਡ);
}

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_ਮੇਲ_From_U(UConverter *cnv, ਅਚਲ int32_t *cx,
                          UChar32 cp,
                          ਅਚਲ char16_t **src, ਅਚਲ char16_t *src_ਹੱਦਬੰਦੀ,
                          ਅੱਖਰ **ਨਿਸ਼ਾਨਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                          int32_t **offsets, int32_t src_ਸੂਚਕਾਂਕ,
                          UBool flush,
                          UError_ਕੋਡ *p_Error_ਕੋਡ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ਮੇਲ;

    /* try to match */
    ਮੇਲ=ucnv_ext_ਮੇਲ_From_U(cx, cp,
                             ਖਾਲੀ_ਸੂਚਕ, 0,
                             *src, (int32_t)(src_ਹੱਦਬੰਦੀ-*src),
                             &value,
                             cnv->useFallback, flush);

    /* reject a match if the result is a single byte for DBCS-only */
    ਜੇ( ਮੇਲ>=2 &&
        !(UCNV_EXT_FROM_U_GET_ਲੰਮਾਈ(value)==1 &&
          cnv->shared_ਅੰਕੜੇ->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY)
    ) {
        /* advance src pointer for the consumed input */
        *src+=ਮੇਲ-2; /* remove 2 for the initial code point */

        /* write result to target */
        ucnv_extWriteFromU(cnv, cx,
                           value,
                           ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                           offsets, src_ਸੂਚਕਾਂਕ,
                           p_Error_ਕੋਡ);
        ਵਾਪਸ ਸੱਚ;
    } ਨਹੀਂਤਾਂ ਜੇ(ਮੇਲ<0) {
        /* save state for partial match */
        ਅਚਲ char16_t *s;
        int32_t j;

        /* copy the first code point */
        cnv->preFromUFirstCP=cp;

        /* now copy the newly consumed input */
        s=*src;
        ਮੇਲ=-ਮੇਲ-2; /* remove 2 for the initial code point */
        ਲਈ(j=0; j<ਮੇਲ; ++j) {
            cnv->preFromU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)ਮੇਲ;
        ਵਾਪਸ ਸੱਚ;
    } ਨਹੀਂਤਾਂ ਜੇ(ਮੇਲ==1) {
        /* matched, no mapping but request for <subchar1> */
        cnv->useSubChar1=ਸੱਚ;
        ਵਾਪਸ ਝੂਠ;
    } ਨਹੀਂਤਾਂ /* match==0 no match */ {
        ਵਾਪਸ ਝੂਠ;
    }
}

/*
 * Used by ISO 2022 implementation.
 * @return number of bytes in *pValue; negative number if fallback; 0 for no mapping
 */
U_CFUNC int32_t
ucnv_ext_Simple_ਮੇਲ_From_U(ਅਚਲ int32_t *cx,
                         UChar32 cp, uint32_t *p_ਮੁੱਲ,
                         UBool useFallback) {
    uint32_t value;
    int32_t ਮੇਲ;

    /* try to match */
    ਮੇਲ=ucnv_ext_ਮੇਲ_From_U(cx,
                             cp,
                             ਖਾਲੀ_ਸੂਚਕ, 0,
                             ਖਾਲੀ_ਸੂਚਕ, 0,
                             &value,
                             useFallback, ਸੱਚ);
    ਜੇ(ਮੇਲ>=2) {
        /* write result for simple, single-character conversion */
        int32_t length;
        ਪੂਰਨ isRoundtrip;

        isRoundtrip=UCNV_EXT_FROM_U_IS_ROUNDTRIP(value);
        length=UCNV_EXT_FROM_U_GET_ਲੰਮਾਈ(value);
        value = UCNV_EXT_FROM_U_GET_ਅੰਕੜੇ(value);

        ਜੇ(length<=UCNV_EXT_FROM_U_MAX_ਸਿੱਧਾ_ਲੰਮਾਈ) {
            *p_ਮੁੱਲ=value;
            ਵਾਪਸ isRoundtrip ? length : -length;
#ਜੇ 0 /* not currently used */
        } ਨਹੀਂਤਾਂ ਜੇ(length==4) {
            /* de-serialize a 4-byte result */
            ਅਚਲ uint8_t *ਨਤੀਜਾ=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_ਬਾਈਟਾਂ_ਸੂਚਕਾਂਕ, uint8_t)+value;
            *p_ਮੁੱਲ=
                ((uint32_t)ਨਤੀਜਾ[0]<<24)|
                ((uint32_t)ਨਤੀਜਾ[1]<<16)|
                ((uint32_t)ਨਤੀਜਾ[2]<<8)|
                ਨਤੀਜਾ[3];
            ਵਾਪਸ isRoundtrip ? 4 : -4;
#ਜੇ_ਅੰਤ
        }
    }

    /*
     * return no match because
     * - match>1 && resultLength>4: result too long for simple conversion
     * - match==1: no match found, <subchar1> preferred
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    ਵਾਪਸ 0;
}

/*
 * continue partial match with new input, requires cnv->preFromUFirstCP>=0
 * never called for simple, single-character conversion
 */
U_CFUNC ਖਾਲੀ
ucnv_ext_Continue_ਮੇਲ_From_U(UConverter *cnv,
                           UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, int32_t src_ਸੂਚਕਾਂਕ,
                           UError_ਕੋਡ *p_Error_ਕੋਡ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ਮੇਲ;

    ਮੇਲ=ucnv_ext_ਮੇਲ_From_U(cnv->shared_ਅੰਕੜੇ->mbcs.extIndexes,
                             cnv->preFromUFirstCP,
                             cnv->preFromU, cnv->preFromULength,
                             p_ਦਲੀਲਾਂ->ਸਰੋਤ, (int32_t)(p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ-p_ਦਲੀਲਾਂ->ਸਰੋਤ),
                             &value,
                             cnv->useFallback, p_ਦਲੀਲਾਂ->flush);
    ਜੇ(ਮੇਲ>=2) {
        ਮੇਲ-=2; /* remove 2 for the initial code point */

        ਜੇ(ਮੇਲ>=cnv->preFromULength) {
            /* advance src pointer for the consumed input */
            p_ਦਲੀਲਾਂ->ਸਰੋਤ+=ਮੇਲ-cnv->preFromULength;
            cnv->preFromULength=0;
        } ਨਹੀਂਤਾਂ {
            /* the match did not use all of preFromU[] - keep the rest for replay */
            int32_t length=cnv->preFromULength-ਮੇਲ;
            u_memmove(cnv->preFromU, cnv->preFromU+ਮੇਲ, length);
            cnv->preFromULength=(int8_t)-length;
        }

        /* finish the partial match */
        cnv->preFromUFirstCP=U_SENTINEL;

        /* write result */
        ucnv_extWriteFromU(cnv, cnv->shared_ਅੰਕੜੇ->mbcs.extIndexes,
                           value,
                           &p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ, p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                           &p_ਦਲੀਲਾਂ->offsets, src_ਸੂਚਕਾਂਕ,
                           p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ ਜੇ(ਮੇਲ<0) {
        /* save state for partial match */
        ਅਚਲ char16_t *s;
        int32_t j;

        /* just _append_ the newly consumed input to preFromU[] */
        s=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
        ਮੇਲ=-ਮੇਲ-2; /* remove 2 for the initial code point */
        ਲਈ(j=cnv->preFromULength; j<ਮੇਲ; ++j) {
            U_ASSERT(j>=0);
            cnv->preFromU[j]=*s++;
        }
        p_ਦਲੀਲਾਂ->ਸਰੋਤ=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)ਮੇਲ;
    } ਨਹੀਂਤਾਂ /* match==0 or 1 */ {
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

        ਜੇ(ਮੇਲ==1) {
            /* matched, no mapping but request for <subchar1> */
            cnv->useSubChar1=ਸੱਚ;
        }

        /* move the first code point to the error field */
        cnv->fromUChar32=cnv->preFromUFirstCP;
        cnv->preFromUFirstCP=U_SENTINEL;

        /* mark preFromU for replay */
        cnv->preFromULength=-cnv->preFromULength;

        /* set the error code for unassigned */
        *p_Error_ਕੋਡ=U_INVALID_CHAR_ਮਿਲਿਆ;
    }
}

ਸਥਿਰ UBool
ext_ਸੈੱਟ_Use_Mapping(UConverter_Unicode_ਸੈੱਟ which, int32_t min_ਲੰਮਾਈ, uint32_t value) {
    ਜੇ(which==UCNV_ROUNDTRIP_ਸੈੱਟ) {
        // Add only code points for which the roundtrip flag is set.
        // Do not add any fallbacks, even if ucnv_fromUnicode() would use them
        // (fallbacks from PUA). See the API docs for ucnv_getUnicodeSet().
        //
        // By analogy, also do not add "good one-way" mappings.
        //
        // Do not add entries with reserved bits set.
        ਜੇ(((value&(UCNV_EXT_FROM_U_ROUNDTRIP_FLAG|UCNV_EXT_FROM_U_RESERVED_ਨਕਾਬ))!=
                UCNV_EXT_FROM_U_ROUNDTRIP_FLAG)) {
            ਵਾਪਸ ਝੂਠ;
        }
    } ਨਹੀਂਤਾਂ /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
        // Do not add entries with reserved bits set.
        ਜੇ((value&UCNV_EXT_FROM_U_RESERVED_ਨਕਾਬ)!=0) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    // Do not add <subchar1> entries or other (future?) pseudo-entries
    // with an output length of 0.
    ਵਾਪਸ UCNV_EXT_FROM_U_GET_ਲੰਮਾਈ(value)>=min_ਲੰਮਾਈ;
}

ਸਥਿਰ ਖਾਲੀ
ucnv_ext_Get_Unicode_ਸੈੱਟ_String(ਅਚਲ UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ,
                            ਅਚਲ int32_t *cx,
                            ਅਚਲ USetAdder *sa,
                            UConverter_Unicode_ਸੈੱਟ which,
                            int32_t min_ਲੰਮਾਈ,
                            UChar32 ਪਹਿਲਾ_CP,
                            char16_t s[UCNV_EXT_MAX_UCHARS], int32_t length,
                            int32_t section_ਸੂਚਕਾਂਕ,
                            UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ char16_t *fromUSectionUChars;
    ਅਚਲ uint32_t *from_USection_ਮੁੱਲਾਂ;

    uint32_t value;
    int32_t i, count;

    fromUSectionUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_ਸੂਚਕਾਂਕ, char16_t)+section_ਸੂਚਕਾਂਕ;
    from_USection_ਮੁੱਲਾਂ=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_ਮੁੱਲਾਂ_ਸੂਚਕਾਂਕ, uint32_t)+section_ਸੂਚਕਾਂਕ;

    /* read first pair of the section */
    count=*fromUSectionUChars++;
    value=*from_USection_ਮੁੱਲਾਂ++;

    ਜੇ(ext_ਸੈੱਟ_Use_Mapping(which, min_ਲੰਮਾਈ, value)) {
        ਜੇ(length==U16_ਲੰਮਾਈ(ਪਹਿਲਾ_CP)) {
            /* add the initial code point */
            sa->ਜੋੜੋ(sa->ਸੈੱਟ, ਪਹਿਲਾ_CP);
        } ਨਹੀਂਤਾਂ {
            /* add the string so far */
            sa->ਜੋੜੋ_String(sa->ਸੈੱਟ, s, length);
        }
    }

    ਲਈ(i=0; i<count; ++i) {
        /* append this code unit and recurse or add the string */
        s[length]=fromUSectionUChars[i];
        value=from_USection_ਮੁੱਲਾਂ[i];

        ਜੇ(value==0) {
            /* no mapping, do nothing */
        } ਨਹੀਂਤਾਂ ਜੇ(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
            ucnv_ext_Get_Unicode_ਸੈੱਟ_String(
                shared_ਅੰਕੜੇ, cx, sa, which, min_ਲੰਮਾਈ,
                ਪਹਿਲਾ_CP, s, length+1,
                static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_ਸੂਚਕਾਂਕ(value)),
                p_Error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ ਜੇ(ext_ਸੈੱਟ_Use_Mapping(which, min_ਲੰਮਾਈ, value)) {
            sa->ਜੋੜੋ_String(sa->ਸੈੱਟ, s, length+1);
        }
    }
}

U_CFUNC ਖਾਲੀ
ucnv_ext_Get_Unicode_ਸੈੱਟ(ਅਚਲ UConverter_Shared_ਅੰਕੜੇ *shared_ਅੰਕੜੇ,
                      ਅਚਲ USetAdder *sa,
                      UConverter_Unicode_ਸੈੱਟ which,
                      UConverter_ਸੈੱਟ_Filter filter,
                      UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ int32_t *cx;
    ਅਚਲ uint16_t *stage12, *stage3, *ps2, *ps3;
    ਅਚਲ uint32_t *stage3b;

    uint32_t value;
    int32_t st1, stage1_ਲੰਮਾਈ, st2, st3, min_ਲੰਮਾਈ;

    char16_t s[UCNV_EXT_MAX_UCHARS];
    UChar32 c;
    int32_t length;

    cx=shared_ਅੰਕੜੇ->mbcs.extIndexes;
    ਜੇ(cx==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_ਸੂਚਕਾਂਕ, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_ਸੂਚਕਾਂਕ, uint16_t);
    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_ਸੂਚਕਾਂਕ, uint32_t);

    stage1_ਲੰਮਾਈ=cx[UCNV_EXT_FROM_U_STAGE_1_ਲੰਮਾਈ];

    /* enumerate the from-Unicode trie table */
    c=0; /* keep track of the current code point while enumerating */

    ਜੇ(filter==UCNV_ਸੈੱਟ_FILTER_2022_CN) {
        min_ਲੰਮਾਈ=3;
    } ਨਹੀਂਤਾਂ ਜੇ( shared_ਅੰਕੜੇ->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ||
               filter!=UCNV_ਸੈੱਟ_FILTER_NONE
    ) {
        /* DBCS-only, ignore single-byte results */
        min_ਲੰਮਾਈ=2;
    } ਨਹੀਂਤਾਂ {
        min_ਲੰਮਾਈ=1;
    }

    /*
     * the trie enumeration is almost the same as
     * in MBCSGetUnicodeSet() for MBCS_OUTPUT_1
     */
    ਲਈ(st1=0; st1<stage1_ਲੰਮਾਈ; ++st1) {
        st2=stage12[st1];
        ਜੇ(st2>stage1_ਲੰਮਾਈ) {
            ps2=stage12+st2;
            ਲਈ(st2=0; st2<64; ++st2) {
                ਜੇ((st3=(int32_t)ps2[st2]<<UCNV_EXT_STAGE_2_ਖੱਬਾ_ਖਿਸਕਾਓ)!=0) {
                    /* read the stage 3 block */
                    ps3=stage3+st3;

                    ਕਰੋ {
                        value=stage3b[*ps3++];
                        ਜੇ(value==0) {
                            /* no mapping, do nothing */
                        } ਨਹੀਂਤਾਂ ਜੇ(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                            // Recurse for partial results.
                            length=0;
                            U16_APPEND_UNSAFE(s, length, c);
                            ucnv_ext_Get_Unicode_ਸੈੱਟ_String(
                                shared_ਅੰਕੜੇ, cx, sa, which, min_ਲੰਮਾਈ,
                                c, s, length,
                                (int32_t)UCNV_EXT_FROM_U_GET_PARTIAL_ਸੂਚਕਾਂਕ(value),
                                p_Error_ਕੋਡ);
                        } ਨਹੀਂਤਾਂ ਜੇ(ext_ਸੈੱਟ_Use_Mapping(which, min_ਲੰਮਾਈ, value)) {
                            ਚੋਣ(filter) {
                            ਹਾਲਤ UCNV_ਸੈੱਟ_FILTER_2022_CN:
                                ਜੇ(!(UCNV_EXT_FROM_U_GET_ਲੰਮਾਈ(value)==3 && UCNV_EXT_FROM_U_GET_ਅੰਕੜੇ(value)<=0x82ffff)) {
                                    ਜਾਰੀ;
                                }
                                ਰੋਕੋ;
                            ਹਾਲਤ UCNV_ਸੈੱਟ_FILTER_SJIS:
                                ਜੇ(!(UCNV_EXT_FROM_U_GET_ਲੰਮਾਈ(value)==2 && (value=UCNV_EXT_FROM_U_GET_ਅੰਕੜੇ(value))>=0x8140 && value<=0xeffc)) {
                                    ਜਾਰੀ;
                                }
                                ਰੋਕੋ;
                            ਹਾਲਤ UCNV_ਸੈੱਟ_FILTER_GR94_DBCS:
                                ਜੇ(!(UCNV_EXT_FROM_U_GET_ਲੰਮਾਈ(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_ਅੰਕੜੇ(value))-0xa1a1)<=(0xfefe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    ਜਾਰੀ;
                                }
                                ਰੋਕੋ;
                            ਹਾਲਤ UCNV_ਸੈੱਟ_FILTER_HZ:
                                ਜੇ(!(UCNV_EXT_FROM_U_GET_ਲੰਮਾਈ(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_ਅੰਕੜੇ(value))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    ਜਾਰੀ;
                                }
                                ਰੋਕੋ;
                            ਮੂਲ:
                                /*
                                 * UCNV_SET_FILTER_NONE,
                                 * or UCNV_SET_FILTER_DBCS_ONLY which is handled via minLength
                                 */
                                ਰੋਕੋ;
                            }
                            sa->ਜੋੜੋ(sa->ਸੈੱਟ, c);
                        }
                    } ਜਦੋਂ((++c&0xf)!=0);
                } ਨਹੀਂਤਾਂ {
                    c+=16; /* empty stage 3 block */
                }
            }
        } ਨਹੀਂਤਾਂ {
            c+=1024; /* empty stage 2 block */
        }
    }
}

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_LEGACY_CONVERSION */
