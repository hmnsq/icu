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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#ضمن "unicode/uset.h"
#ضمن "unicode/ustring.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"
#ضمن "ucnv_ext.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"

/* to Unicode --------------------------------------------------------------- */

/*
 * @return lookup value for the byte, if found; else 0
 */
ساكن مضمن uint32_t
ucnv_ext_ڳوليو_منزل_U(ثابت uint32_t *منزل_USection, int32_t length, uint8_t byte) {
    uint32_t word0, لفظ_2;
    int32_t i, شروعات, حد_بندي;

    /* check the input byte against the lowest and highest section bytes */
    شروعات = static_cast<int32_t>(UCNV_EXT_منزل_U_GET_BYTE(منزل_USection[0]));
    حد_بندي = static_cast<int32_t>(UCNV_EXT_منزل_U_GET_BYTE(منزل_USection[length - 1]));
    إذا(byte<شروعات || حد_بندي<byte) {
        أرجع 0; /* the byte is out of range */
    }

    إذا(length==((حد_بندي-شروعات)+1)) {
        /* direct access on a linear array */
        أرجع UCNV_EXT_منزل_U_GET_قدر(منزل_USection[byte-شروعات]); /* could be 0 */
    }

    /* word0 is suitable for <=toUSection[] comparison, word for <toUSection[] */
    word0=UCNV_EXT_منزل_U_MAKE_لفظ(byte, 0);

    /*
     * Shift byte once instead of each section word and add 0xffffff.
     * We will compare the shifted/added byte (bbffffff) against
     * section words which have byte values in the same bit position.
     * If and only if byte bb < section byte ss then bbffffff<ssvvvvvv
     * for all v=0..f
     * so we need not mask off the lower 24 bits of each section word.
     */
    لفظ_2=word0|UCNV_EXT_منزل_U_قدر_نقاب;

    /* binary search */
    شروعات=0;
    حد_بندي=length;
    لكل(;;) {
        i=حد_بندي-شروعات;
        إذا(i<=1) {
            اكسر; /*پورو*/
        }
        /* start<limit-1 */

        إذا(i<=4) {
            /* linear search for the last part */
            إذا(word0<=منزل_USection[شروعات]) {
                اكسر;
            }
            إذا(++شروعات<حد_بندي && word0<=منزل_USection[شروعات]) {
                اكسر;
            }
            إذا(++شروعات<حد_بندي && word0<=منزل_USection[شروعات]) {
                اكسر;
            }
            /* always break at start==limit-1 */
            ++شروعات;
            اكسر;
        }

        i=(شروعات+حد_بندي)/2;
        إذا(لفظ_2<منزل_USection[i]) {
            حد_بندي=i;
        } وإلا {
            شروعات=i;
        }
    }

    /* did we really find it? */
    إذا(شروعات<حد_بندي && byte==UCNV_EXT_منزل_U_GET_BYTE(لفظ_2=منزل_USection[شروعات])) {
        أرجع UCNV_EXT_منزل_U_GET_قدر(لفظ_2); /* never 0 */
    } وإلا {
        أرجع 0; /* not found */
    }
}

/*
 * true if not an SI/SO stateful converter,
 * or if the match length fits with the current converter state
 */
#وصف_ڪريو UCNV_EXT_منزل_U_VERIFY_SISO_ميلاپ(sisoState, ميلاپ) \
    ((sisoState)<0 || ((sisoState)==0) == (ميلاپ==1))

/*
 * this works like ucnv_extMatchFromU() except
 * - the first character is in pre
 * - no trie is used
 * - the returned matchLength is not offset by 2
 */
ساكن int32_t
ucnv_ext_ميلاپ_منزل_U(ثابت int32_t *cx, int8_t sisoState,
                 ثابت حرف *pre, int32_t pre_ڊيگهائي,
                 ثابت حرف *src, int32_t src_ڊيگهائي,
                 uint32_t *p_ميلاپ_قدر,
                 UBool /*useFallback*/, UBool flush) {
    ثابت uint32_t *منزل_UTable, *منزل_USection;

    uint32_t value, ميلاپ_قدر;
    int32_t i, j, idx, length, ميلاپ_ڊيگهائي;
    uint8_t b;

    إذا(cx==خالي_اشارو || cx[UCNV_EXT_منزل_U_ڊيگهائي]<=0) {
        أرجع 0; /* no extension data, no match */
    }

    /* initialize */
    منزل_UTable=UCNV_EXT_ARRAY(cx, UCNV_EXT_منزل_U_اشاريو, uint32_t);
    idx=0;

    ميلاپ_قدر=0;
    i=j=ميلاپ_ڊيگهائي=0;

    إذا(sisoState==0) {
        /* SBCS state of an SI/SO stateful converter, look at only exactly 1 byte */
        إذا(pre_ڊيگهائي>1) {
            أرجع 0; /* no match of a DBCS sequence in SBCS mode */
        } وإلا إذا(pre_ڊيگهائي==1) {
            src_ڊيگهائي=0;
        } وإلا /* preLength==0 */ {
            إذا(src_ڊيگهائي>1) {
                src_ڊيگهائي=1;
            }
        }
        flush=سچ;
    }

    /* we must not remember fallback matches when not using fallbacks */

    /* match input units until there is a full match or the input is consumed */
    لكل(;;) {
        /* go to the next section */
        منزل_USection=منزل_UTable+idx;

        /* read first pair of the section */
        value=*منزل_USection++;
        length=UCNV_EXT_منزل_U_GET_BYTE(value);
        value=UCNV_EXT_منزل_U_GET_قدر(value);
        إذا( value!=0 &&
            (UCNV_EXT_منزل_U_IS_ROUNDTRIP(value) ||
             منزل_U_USE_FALLBACK(useFallback)) &&
            UCNV_EXT_منزل_U_VERIFY_SISO_ميلاپ(sisoState, i+j)
        ) {
            /* remember longest match so far */
            ميلاپ_قدر=value;
            ميلاپ_ڊيگهائي=i+j;
        }

        /* match pre[] then src[] */
        إذا(i<pre_ڊيگهائي) {
            b = static_cast<uint8_t>(pre[i++]);
        } وإلا إذا(j<src_ڊيگهائي) {
            b = static_cast<uint8_t>(src[j++]);
        } وإلا {
            /* all input consumed, partial match */
            إذا(flush || (length=(i+j))>UCNV_EXT_MAX_بائيٽون) {
                /*
                 * end of the entire input stream, stop with the longest match so far
                 * or: partial match must not be longer than UCNV_EXT_MAX_BYTES
                 * because it must fit into state buffers
                 */
                اكسر;
            } وإلا {
                /* continue with more input next time */
                أرجع -length;
            }
        }

        /* search for the current char16_t */
        value=ucnv_ext_ڳوليو_منزل_U(منزل_USection, length, b);
        إذا(value==0) {
            /* no match here, stop with the longest match so far */
            اكسر;
        } وإلا {
            إذا(UCNV_EXT_منزل_U_IS_PARTIAL(value)) {
                /* partial match, continue */
                idx = static_cast<int32_t>(UCNV_EXT_منزل_U_GET_PARTIAL_اشاريو(value));
            } وإلا {
                إذا( (UCNV_EXT_منزل_U_IS_ROUNDTRIP(value) ||
                     منزل_U_USE_FALLBACK(useFallback)) &&
                    UCNV_EXT_منزل_U_VERIFY_SISO_ميلاپ(sisoState, i+j)
                ) {
                    /* full match, stop with result */
                    ميلاپ_قدر=value;
                    ميلاپ_ڊيگهائي=i+j;
                } وإلا {
                    /* full match on fallback not taken, stop with the longest match so far */
                }
                اكسر;
            }
        }
    }

    إذا(ميلاپ_ڊيگهائي==0) {
        /* no match at all */
        أرجع 0;
    }

    /* return result */
    *p_ميلاپ_قدر=UCNV_EXT_منزل_U_نقاب_ROUNDTRIP(ميلاپ_قدر);
    أرجع ميلاپ_ڊيگهائي;
}

ساكن مضمن فراغ
ucnv_ext_Write_منزل_U(UConverter *cnv, ثابت int32_t *cx,
                 uint32_t value,
                 char16_t **هدف, ثابت char16_t *هدف_حد_بندي,
                 int32_t **offsets, int32_t src_اشاريو,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    /* output the result */
    إذا(UCNV_EXT_منزل_U_IS_ڪوڊ_نقطو(value)) {
        /* output a single code point */
        ucnv_منزل_UWrite_ڪوڊ_نقطو(
            cnv, UCNV_EXT_منزل_U_GET_ڪوڊ_نقطو(value),
            هدف, هدف_حد_بندي,
            offsets, src_اشاريو,
            p_Error_ڪوڊ);
    } وإلا {
        /* output a string - with correct data we have resultLength>0 */
        ucnv_منزل_UWrite_UChars(
            cnv,
            UCNV_EXT_ARRAY(cx, UCNV_EXT_منزل_U_UCHARS_اشاريو, char16_t)+
                UCNV_EXT_منزل_U_GET_اشاريو(value),
            UCNV_EXT_منزل_U_GET_ڊيگهائي(value),
            هدف, هدف_حد_بندي,
            offsets, src_اشاريو,
            p_Error_ڪوڊ);
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
#وصف_ڪريو UCNV_SISO_STATE(cnv) \
    ((cnv)->shared_معلومات->mbcs.outputType==MBCS_OUTPUT_2_SISO ? (int8_t)(cnv)->گهڻائي : \
     (cnv)->shared_معلومات->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ? 1 : -1)

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_ميلاپ_منزل_U(UConverter *cnv, ثابت int32_t *cx,
                        int32_t پهريون_ڊيگهائي,
                        ثابت حرف **src, ثابت حرف *src_حد_بندي,
                        char16_t **هدف, ثابت char16_t *هدف_حد_بندي,
                        int32_t **offsets, int32_t src_اشاريو,
                        UBool flush,
                        UError_ڪوڊ *p_Error_ڪوڊ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ميلاپ;

    /* try to match */
    ميلاپ=ucnv_ext_ميلاپ_منزل_U(cx, (int8_t)UCNV_SISO_STATE(cnv),
                           (ثابت حرف *)cnv->منزل_UBytes, پهريون_ڊيگهائي,
                           *src, (int32_t)(src_حد_بندي-*src),
                           &value,
                           cnv->useFallback, flush);
    إذا(ميلاپ>0) {
        /* advance src pointer for the consumed input */
        *src+=ميلاپ-پهريون_ڊيگهائي;

        /* write result to target */
        ucnv_ext_Write_منزل_U(cnv, cx,
                         value,
                         هدف, هدف_حد_بندي,
                         offsets, src_اشاريو,
                         p_Error_ڪوڊ);
        أرجع سچ;
    } وإلا إذا(ميلاپ<0) {
        /* save state for partial match */
        ثابت حرف *s;
        int32_t j;

        /* copy the first code point */
        s=(ثابت حرف *)cnv->منزل_UBytes;
        cnv->pre_منزل_UFirst_ڊيگهائي=(int8_t)پهريون_ڊيگهائي;
        لكل(j=0; j<پهريون_ڊيگهائي; ++j) {
            cnv->pre_منزل_U[j]=*s++;
        }

        /* now copy the newly consumed input */
        s=*src;
        ميلاپ=-ميلاپ;
        لكل(; j<ميلاپ; ++j) {
            cnv->pre_منزل_U[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_منزل_ULength=(int8_t)ميلاپ;
        أرجع سچ;
    } وإلا /* match==0 no match */ {
        أرجع خطأ;
    }
}

U_CFUNC UChar32
ucnv_ext_Simple_ميلاپ_منزل_U(ثابت int32_t *cx,
                       ثابت حرف *ذريعو, int32_t length,
                       UBool useFallback) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ميلاپ;

    إذا(length<=0) {
        أرجع 0xffff;
    }

    /* try to match */
    ميلاپ=ucnv_ext_ميلاپ_منزل_U(cx, -1,
                           ذريعو, length,
                           خالي_اشارو, 0,
                           &value,
                           useFallback, سچ);
    إذا(ميلاپ==length) {
        /* write result for simple, single-character conversion */
        إذا(UCNV_EXT_منزل_U_IS_ڪوڊ_نقطو(value)) {
            أرجع UCNV_EXT_منزل_U_GET_ڪوڊ_نقطو(value);
        }
    }

    /*
     * return no match because
     * - match>0 && value points to string: simple conversion cannot handle multiple code points
     * - match>0 && match!=length: not all input consumed, forbidden for this function
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    أرجع 0xfffe;
}

/*
 * continue partial match with new input
 * never called for simple, single-character conversion
 */
U_CFUNC فراغ
ucnv_ext_Continue_ميلاپ_منزل_U(UConverter *cnv,
                         UConverter_منزل_Unicode_دليلون *p_دليلون, int32_t src_اشاريو,
                         UError_ڪوڊ *p_Error_ڪوڊ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ميلاپ, length;

    ميلاپ=ucnv_ext_ميلاپ_منزل_U(cnv->shared_معلومات->mbcs.extIndexes, (int8_t)UCNV_SISO_STATE(cnv),
                           cnv->pre_منزل_U, cnv->pre_منزل_ULength,
                           p_دليلون->ذريعو, (int32_t)(p_دليلون->ذريعو_حد_بندي-p_دليلون->ذريعو),
                           &value,
                           cnv->useFallback, p_دليلون->flush);
    إذا(ميلاپ>0) {
        إذا(ميلاپ>=cnv->pre_منزل_ULength) {
            /* advance src pointer for the consumed input */
            p_دليلون->ذريعو+=ميلاپ-cnv->pre_منزل_ULength;
            cnv->pre_منزل_ULength=0;
        } وإلا {
            /* the match did not use all of preToU[] - keep the rest for replay */
            length=cnv->pre_منزل_ULength-ميلاپ;
            uprv_memmove(cnv->pre_منزل_U, cnv->pre_منزل_U+ميلاپ, length);
            cnv->pre_منزل_ULength=(int8_t)-length;
        }

        /* write result */
        ucnv_ext_Write_منزل_U(cnv, cnv->shared_معلومات->mbcs.extIndexes,
                         value,
                         &p_دليلون->هدف, p_دليلون->هدف_حد_بندي,
                         &p_دليلون->offsets, src_اشاريو,
                         p_Error_ڪوڊ);
    } وإلا إذا(ميلاپ<0) {
        /* save state for partial match */
        ثابت حرف *s;
        int32_t j;

        /* just _append_ the newly consumed input to preToU[] */
        s=p_دليلون->ذريعو;
        ميلاپ=-ميلاپ;
        لكل(j=cnv->pre_منزل_ULength; j<ميلاپ; ++j) {
            cnv->pre_منزل_U[j]=*s++;
        }
        p_دليلون->ذريعو=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_منزل_ULength=(int8_t)ميلاپ;
    } وإلا /* match==0 */ {
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
        uprv_memcpy(cnv->منزل_UBytes, cnv->pre_منزل_U, cnv->pre_منزل_UFirst_ڊيگهائي);
        cnv->منزل_ULength=cnv->pre_منزل_UFirst_ڊيگهائي;

        /* move the rest up inside the buffer */
        length=cnv->pre_منزل_ULength-cnv->pre_منزل_UFirst_ڊيگهائي;
        إذا(length>0) {
            uprv_memmove(cnv->pre_منزل_U, cnv->pre_منزل_U+cnv->pre_منزل_UFirst_ڊيگهائي, length);
        }

        /* mark preToU for replay */
        cnv->pre_منزل_ULength=(int8_t)-length;

        /* set the error code for unassigned */
        *p_Error_ڪوڊ=U_INVALID_CHAR_مليو;
    }
}

/* from Unicode ------------------------------------------------------------- */

// Use roundtrips, "good one-way" mappings, and some normal fallbacks.
ساكن مضمن UBool
extFromUUseMapping(UBool useFallback, uint32_t value, UChar32 پهريون_CP) {
    أرجع
        ((value&UCNV_EXT_FROM_U_حالت_نقاب)!=0 ||
            FROM_U_USE_FALLBACK(useFallback, پهريون_CP)) &&
        (value&UCNV_EXT_FROM_U_RESERVED_نقاب)==0;
}

/*
 * @return index of the char16_t, if found; else <0
 */
ساكن مضمن int32_t
ucnv_ext_ڳوليو_From_U(ثابت char16_t *fromUSection, int32_t length, char16_t u) {
    int32_t i, شروعات, حد_بندي;

    /* binary search */
    شروعات=0;
    حد_بندي=length;
    لكل(;;) {
        i=حد_بندي-شروعات;
        إذا(i<=1) {
            اكسر; /*پورو*/
        }
        /* start<limit-1 */

        إذا(i<=4) {
            /* linear search for the last part */
            إذا(u<=fromUSection[شروعات]) {
                اكسر;
            }
            إذا(++شروعات<حد_بندي && u<=fromUSection[شروعات]) {
                اكسر;
            }
            إذا(++شروعات<حد_بندي && u<=fromUSection[شروعات]) {
                اكسر;
            }
            /* always break at start==limit-1 */
            ++شروعات;
            اكسر;
        }

        i=(شروعات+حد_بندي)/2;
        إذا(u<fromUSection[i]) {
            حد_بندي=i;
        } وإلا {
            شروعات=i;
        }
    }

    /* did we really find it? */
    إذا(شروعات<حد_بندي && u==fromUSection[شروعات]) {
        أرجع شروعات;
    } وإلا {
        أرجع -1; /* not found */
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
ساكن int32_t
ucnv_ext_ميلاپ_From_U(ثابت int32_t *cx,
                   UChar32 پهريون_CP,
                   ثابت char16_t *pre, int32_t pre_ڊيگهائي,
                   ثابت char16_t *src, int32_t src_ڊيگهائي,
                   uint32_t *p_ميلاپ_قدر,
                   UBool useFallback, UBool flush) {
    ثابت uint16_t *stage12, *stage3;
    ثابت uint32_t *stage3b;

    ثابت char16_t *fromUTableUChars, *fromUSectionUChars;
    ثابت uint32_t *from_UTable_قدرون, *from_USection_قدرون;

    uint32_t value, ميلاپ_قدر;
    int32_t i, j, idx, length, ميلاپ_ڊيگهائي;
    char16_t c;

    إذا(cx==خالي_اشارو) {
        أرجع 0; /* no extension data, no match */
    }

    /* trie lookup of firstCP */
    idx=پهريون_CP>>10; /* stage 1 index */
    إذا(idx>=cx[UCNV_EXT_FROM_U_STAGE_1_ڊيگهائي]) {
        أرجع 0; /* the first code point is outside the trie */
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_اشاريو, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_اشاريو, uint16_t);
    idx=UCNV_EXT_FROM_U(stage12, stage3, idx, پهريون_CP);

    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_اشاريو, uint32_t);
    value=stage3b[idx];
    إذا(value==0) {
        أرجع 0;
    }

    /*
     * Tests for (value&UCNV_EXT_FROM_U_RESERVED_MASK)==0:
     * Do not interpret values with reserved bits used, for forward compatibility,
     * and do not even remember intermediate results with reserved bits used.
     */

    إذا(UCNV_EXT_منزل_U_IS_PARTIAL(value)) {
        /* partial match, enter the loop below */
        idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_اشاريو(value));

        /* initialize */
        fromUTableUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_اشاريو, char16_t);
        from_UTable_قدرون=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_قدرون_اشاريو, uint32_t);

        ميلاپ_قدر=0;
        i=j=ميلاپ_ڊيگهائي=0;

        /* we must not remember fallback matches when not using fallbacks */

        /* match input units until there is a full match or the input is consumed */
        لكل(;;) {
            /* go to the next section */
            fromUSectionUChars=fromUTableUChars+idx;
            from_USection_قدرون=from_UTable_قدرون+idx;

            /* read first pair of the section */
            length=*fromUSectionUChars++;
            value=*from_USection_قدرون++;
            إذا(value!=0 && extFromUUseMapping(useFallback, value, پهريون_CP)) {
                /* remember longest match so far */
                ميلاپ_قدر=value;
                ميلاپ_ڊيگهائي=2+i+j;
            }

            /* match pre[] then src[] */
            إذا(i<pre_ڊيگهائي) {
                c=pre[i++];
            } وإلا إذا(j<src_ڊيگهائي) {
                c=src[j++];
            } وإلا {
                /* all input consumed, partial match */
                إذا(flush || (length=(i+j))>UCNV_EXT_MAX_UCHARS) {
                    /*
                     * end of the entire input stream, stop with the longest match so far
                     * or: partial match must not be longer than UCNV_EXT_MAX_UCHARS
                     * because it must fit into state buffers
                     */
                    اكسر;
                } وإلا {
                    /* continue with more input next time */
                    أرجع -(2+length);
                }
            }

            /* search for the current char16_t */
            idx=ucnv_ext_ڳوليو_From_U(fromUSectionUChars, length, c);
            إذا(idx<0) {
                /* no match here, stop with the longest match so far */
                اكسر;
            } وإلا {
                value=from_USection_قدرون[idx];
                إذا(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                    /* partial match, continue */
                    idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_اشاريو(value));
                } وإلا {
                    إذا(extFromUUseMapping(useFallback, value, پهريون_CP)) {
                        /* full match, stop with result */
                        ميلاپ_قدر=value;
                        ميلاپ_ڊيگهائي=2+i+j;
                    } وإلا {
                        /* full match on fallback not taken, stop with the longest match so far */
                    }
                    اكسر;
                }
            }
        }

        إذا(ميلاپ_ڊيگهائي==0) {
            /* no match at all */
            أرجع 0;
        }
    } وإلا /* result from firstCP trie lookup */ {
        إذا(extFromUUseMapping(useFallback, value, پهريون_CP)) {
            /* full match, stop with result */
            ميلاپ_قدر=value;
            ميلاپ_ڊيگهائي=2;
        } وإلا {
            /* fallback not taken */
            أرجع 0;
        }
    }

    /* return result */
    إذا(ميلاپ_قدر==UCNV_EXT_FROM_U_SUBCHAR1) {
        أرجع 1; /* assert matchLength==2 */
    }

    *p_ميلاپ_قدر=ميلاپ_قدر;
    أرجع ميلاپ_ڊيگهائي;
}

/*
 * @param value fromUnicode mapping table value; ignores roundtrip and reserved bits
 */
ساكن مضمن فراغ
ucnv_extWriteFromU(UConverter *cnv, ثابت int32_t *cx,
                   uint32_t value,
                   حرف **هدف, ثابت حرف *هدف_حد_بندي,
                   int32_t **offsets, int32_t src_اشاريو,
                   UError_ڪوڊ *p_Error_ڪوڊ) {
    uint8_t بفر[1+UCNV_EXT_MAX_بائيٽون];
    ثابت uint8_t *نتيجو;
    int32_t length, prev_ڊيگهائي;

    length=UCNV_EXT_FROM_U_GET_ڊيگهائي(value);
    value = UCNV_EXT_FROM_U_GET_معلومات(value);

    /* output the result */
    إذا(length<=UCNV_EXT_FROM_U_MAX_سڌو_ڊيگهائي) {
        /*
         * Generate a byte array and then write it below.
         * This is not the fastest possible way, but it should be ok for
         * extension mappings, and it is much simpler.
         * Offset and overflow handling are only done once this way.
         */
        uint8_t *p=بفر+1; /* reserve buffer[0] for shiftByte below */
        بدل(length) {
        حالة 3:
            *p++ = static_cast<uint8_t>(value >> 16);
            U_FALLTHROUGH;
        حالة 2:
            *p++ = static_cast<uint8_t>(value >> 8);
            U_FALLTHROUGH;
        حالة 1:
            *p++ = static_cast<uint8_t>(value);
            U_FALLTHROUGH;
        افتراضي:
            اكسر; /* will never occur */
        }
        نتيجو=بفر+1;
    } وإلا {
        نتيجو=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_بائيٽون_اشاريو, uint8_t)+value;
    }

    /* with correct data we have length>0 */

    إذا((prev_ڊيگهائي=cnv->from_Unicode_حالت)!=0) {
        /* handle SI/SO stateful output */
        uint8_t سرڪايو_Byte;

        إذا(prev_ڊيگهائي>1 && length==1) {
            /* change from double-byte mode to single-byte */
            سرڪايو_Byte = static_cast<uint8_t>(UCNV_SI);
            cnv->from_Unicode_حالت=1;
        } وإلا إذا(prev_ڊيگهائي==1 && length>1) {
            /* change from single-byte mode to double-byte */
            سرڪايو_Byte = static_cast<uint8_t>(UCNV_SO);
            cnv->from_Unicode_حالت=2;
        } وإلا {
            سرڪايو_Byte=0;
        }

        إذا(سرڪايو_Byte!=0) {
            /* prepend the shift byte to the result bytes */
            بفر[0]=سرڪايو_Byte;
            إذا(نتيجو!=بفر+1) {
                uprv_memcpy(بفر+1, نتيجو, length);
            }
            نتيجو=بفر;
            ++length;
        }
    }

    ucnv_from_UWrite_بائيٽون(cnv, reinterpret_cast<ثابت حرف*>(نتيجو), length,
                         هدف, هدف_حد_بندي,
                         offsets, src_اشاريو,
                         p_Error_ڪوڊ);
}

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_ميلاپ_From_U(UConverter *cnv, ثابت int32_t *cx,
                          UChar32 cp,
                          ثابت char16_t **src, ثابت char16_t *src_حد_بندي,
                          حرف **هدف, ثابت حرف *هدف_حد_بندي,
                          int32_t **offsets, int32_t src_اشاريو,
                          UBool flush,
                          UError_ڪوڊ *p_Error_ڪوڊ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ميلاپ;

    /* try to match */
    ميلاپ=ucnv_ext_ميلاپ_From_U(cx, cp,
                             خالي_اشارو, 0,
                             *src, (int32_t)(src_حد_بندي-*src),
                             &value,
                             cnv->useFallback, flush);

    /* reject a match if the result is a single byte for DBCS-only */
    إذا( ميلاپ>=2 &&
        !(UCNV_EXT_FROM_U_GET_ڊيگهائي(value)==1 &&
          cnv->shared_معلومات->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY)
    ) {
        /* advance src pointer for the consumed input */
        *src+=ميلاپ-2; /* remove 2 for the initial code point */

        /* write result to target */
        ucnv_extWriteFromU(cnv, cx,
                           value,
                           هدف, هدف_حد_بندي,
                           offsets, src_اشاريو,
                           p_Error_ڪوڊ);
        أرجع سچ;
    } وإلا إذا(ميلاپ<0) {
        /* save state for partial match */
        ثابت char16_t *s;
        int32_t j;

        /* copy the first code point */
        cnv->preFromUFirstCP=cp;

        /* now copy the newly consumed input */
        s=*src;
        ميلاپ=-ميلاپ-2; /* remove 2 for the initial code point */
        لكل(j=0; j<ميلاپ; ++j) {
            cnv->preFromU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)ميلاپ;
        أرجع سچ;
    } وإلا إذا(ميلاپ==1) {
        /* matched, no mapping but request for <subchar1> */
        cnv->useSubChar1=سچ;
        أرجع خطأ;
    } وإلا /* match==0 no match */ {
        أرجع خطأ;
    }
}

/*
 * Used by ISO 2022 implementation.
 * @return number of bytes in *pValue; negative number if fallback; 0 for no mapping
 */
U_CFUNC int32_t
ucnv_ext_Simple_ميلاپ_From_U(ثابت int32_t *cx,
                         UChar32 cp, uint32_t *p_قدر,
                         UBool useFallback) {
    uint32_t value;
    int32_t ميلاپ;

    /* try to match */
    ميلاپ=ucnv_ext_ميلاپ_From_U(cx,
                             cp,
                             خالي_اشارو, 0,
                             خالي_اشارو, 0,
                             &value,
                             useFallback, سچ);
    إذا(ميلاپ>=2) {
        /* write result for simple, single-character conversion */
        int32_t length;
        صحيح isRoundtrip;

        isRoundtrip=UCNV_EXT_FROM_U_IS_ROUNDTRIP(value);
        length=UCNV_EXT_FROM_U_GET_ڊيگهائي(value);
        value = UCNV_EXT_FROM_U_GET_معلومات(value);

        إذا(length<=UCNV_EXT_FROM_U_MAX_سڌو_ڊيگهائي) {
            *p_قدر=value;
            أرجع isRoundtrip ? length : -length;
#إذا 0 /* not currently used */
        } وإلا إذا(length==4) {
            /* de-serialize a 4-byte result */
            ثابت uint8_t *نتيجو=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_بائيٽون_اشاريو, uint8_t)+value;
            *p_قدر=
                ((uint32_t)نتيجو[0]<<24)|
                ((uint32_t)نتيجو[1]<<16)|
                ((uint32_t)نتيجو[2]<<8)|
                نتيجو[3];
            أرجع isRoundtrip ? 4 : -4;
#جيڪڏهن_پورو
        }
    }

    /*
     * return no match because
     * - match>1 && resultLength>4: result too long for simple conversion
     * - match==1: no match found, <subchar1> preferred
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    أرجع 0;
}

/*
 * continue partial match with new input, requires cnv->preFromUFirstCP>=0
 * never called for simple, single-character conversion
 */
U_CFUNC فراغ
ucnv_ext_Continue_ميلاپ_From_U(UConverter *cnv,
                           UConverter_From_Unicode_دليلون *p_دليلون, int32_t src_اشاريو,
                           UError_ڪوڊ *p_Error_ڪوڊ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t ميلاپ;

    ميلاپ=ucnv_ext_ميلاپ_From_U(cnv->shared_معلومات->mbcs.extIndexes,
                             cnv->preFromUFirstCP,
                             cnv->preFromU, cnv->preFromULength,
                             p_دليلون->ذريعو, (int32_t)(p_دليلون->ذريعو_حد_بندي-p_دليلون->ذريعو),
                             &value,
                             cnv->useFallback, p_دليلون->flush);
    إذا(ميلاپ>=2) {
        ميلاپ-=2; /* remove 2 for the initial code point */

        إذا(ميلاپ>=cnv->preFromULength) {
            /* advance src pointer for the consumed input */
            p_دليلون->ذريعو+=ميلاپ-cnv->preFromULength;
            cnv->preFromULength=0;
        } وإلا {
            /* the match did not use all of preFromU[] - keep the rest for replay */
            int32_t length=cnv->preFromULength-ميلاپ;
            u_memmove(cnv->preFromU, cnv->preFromU+ميلاپ, length);
            cnv->preFromULength=(int8_t)-length;
        }

        /* finish the partial match */
        cnv->preFromUFirstCP=U_SENTINEL;

        /* write result */
        ucnv_extWriteFromU(cnv, cnv->shared_معلومات->mbcs.extIndexes,
                           value,
                           &p_دليلون->هدف, p_دليلون->هدف_حد_بندي,
                           &p_دليلون->offsets, src_اشاريو,
                           p_Error_ڪوڊ);
    } وإلا إذا(ميلاپ<0) {
        /* save state for partial match */
        ثابت char16_t *s;
        int32_t j;

        /* just _append_ the newly consumed input to preFromU[] */
        s=p_دليلون->ذريعو;
        ميلاپ=-ميلاپ-2; /* remove 2 for the initial code point */
        لكل(j=cnv->preFromULength; j<ميلاپ; ++j) {
            U_ASSERT(j>=0);
            cnv->preFromU[j]=*s++;
        }
        p_دليلون->ذريعو=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)ميلاپ;
    } وإلا /* match==0 or 1 */ {
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

        إذا(ميلاپ==1) {
            /* matched, no mapping but request for <subchar1> */
            cnv->useSubChar1=سچ;
        }

        /* move the first code point to the error field */
        cnv->fromUChar32=cnv->preFromUFirstCP;
        cnv->preFromUFirstCP=U_SENTINEL;

        /* mark preFromU for replay */
        cnv->preFromULength=-cnv->preFromULength;

        /* set the error code for unassigned */
        *p_Error_ڪوڊ=U_INVALID_CHAR_مليو;
    }
}

ساكن UBool
ext_سيٽ_Use_Mapping(UConverter_Unicode_سيٽ which, int32_t min_ڊيگهائي, uint32_t value) {
    إذا(which==UCNV_ROUNDTRIP_سيٽ) {
        // Add only code points for which the roundtrip flag is set.
        // Do not add any fallbacks, even if ucnv_fromUnicode() would use them
        // (fallbacks from PUA). See the API docs for ucnv_getUnicodeSet().
        //
        // By analogy, also do not add "good one-way" mappings.
        //
        // Do not add entries with reserved bits set.
        إذا(((value&(UCNV_EXT_FROM_U_ROUNDTRIP_FLAG|UCNV_EXT_FROM_U_RESERVED_نقاب))!=
                UCNV_EXT_FROM_U_ROUNDTRIP_FLAG)) {
            أرجع خطأ;
        }
    } وإلا /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
        // Do not add entries with reserved bits set.
        إذا((value&UCNV_EXT_FROM_U_RESERVED_نقاب)!=0) {
            أرجع خطأ;
        }
    }
    // Do not add <subchar1> entries or other (future?) pseudo-entries
    // with an output length of 0.
    أرجع UCNV_EXT_FROM_U_GET_ڊيگهائي(value)>=min_ڊيگهائي;
}

ساكن فراغ
ucnv_ext_Get_Unicode_سيٽ_String(ثابت UConverter_Shared_معلومات *shared_معلومات,
                            ثابت int32_t *cx,
                            ثابت USetAdder *sa,
                            UConverter_Unicode_سيٽ which,
                            int32_t min_ڊيگهائي,
                            UChar32 پهريون_CP,
                            char16_t s[UCNV_EXT_MAX_UCHARS], int32_t length,
                            int32_t section_اشاريو,
                            UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت char16_t *fromUSectionUChars;
    ثابت uint32_t *from_USection_قدرون;

    uint32_t value;
    int32_t i, count;

    fromUSectionUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_اشاريو, char16_t)+section_اشاريو;
    from_USection_قدرون=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_قدرون_اشاريو, uint32_t)+section_اشاريو;

    /* read first pair of the section */
    count=*fromUSectionUChars++;
    value=*from_USection_قدرون++;

    إذا(ext_سيٽ_Use_Mapping(which, min_ڊيگهائي, value)) {
        إذا(length==U16_ڊيگهائي(پهريون_CP)) {
            /* add the initial code point */
            sa->جوڙيو(sa->سيٽ, پهريون_CP);
        } وإلا {
            /* add the string so far */
            sa->جوڙيو_String(sa->سيٽ, s, length);
        }
    }

    لكل(i=0; i<count; ++i) {
        /* append this code unit and recurse or add the string */
        s[length]=fromUSectionUChars[i];
        value=from_USection_قدرون[i];

        إذا(value==0) {
            /* no mapping, do nothing */
        } وإلا إذا(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
            ucnv_ext_Get_Unicode_سيٽ_String(
                shared_معلومات, cx, sa, which, min_ڊيگهائي,
                پهريون_CP, s, length+1,
                static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_اشاريو(value)),
                p_Error_ڪوڊ);
        } وإلا إذا(ext_سيٽ_Use_Mapping(which, min_ڊيگهائي, value)) {
            sa->جوڙيو_String(sa->سيٽ, s, length+1);
        }
    }
}

U_CFUNC فراغ
ucnv_ext_Get_Unicode_سيٽ(ثابت UConverter_Shared_معلومات *shared_معلومات,
                      ثابت USetAdder *sa,
                      UConverter_Unicode_سيٽ which,
                      UConverter_سيٽ_Filter filter,
                      UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت int32_t *cx;
    ثابت uint16_t *stage12, *stage3, *ps2, *ps3;
    ثابت uint32_t *stage3b;

    uint32_t value;
    int32_t st1, stage1_ڊيگهائي, st2, st3, min_ڊيگهائي;

    char16_t s[UCNV_EXT_MAX_UCHARS];
    UChar32 c;
    int32_t length;

    cx=shared_معلومات->mbcs.extIndexes;
    إذا(cx==خالي_اشارو) {
        أرجع;
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_اشاريو, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_اشاريو, uint16_t);
    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_اشاريو, uint32_t);

    stage1_ڊيگهائي=cx[UCNV_EXT_FROM_U_STAGE_1_ڊيگهائي];

    /* enumerate the from-Unicode trie table */
    c=0; /* keep track of the current code point while enumerating */

    إذا(filter==UCNV_سيٽ_FILTER_2022_CN) {
        min_ڊيگهائي=3;
    } وإلا إذا( shared_معلومات->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ||
               filter!=UCNV_سيٽ_FILTER_NONE
    ) {
        /* DBCS-only, ignore single-byte results */
        min_ڊيگهائي=2;
    } وإلا {
        min_ڊيگهائي=1;
    }

    /*
     * the trie enumeration is almost the same as
     * in MBCSGetUnicodeSet() for MBCS_OUTPUT_1
     */
    لكل(st1=0; st1<stage1_ڊيگهائي; ++st1) {
        st2=stage12[st1];
        إذا(st2>stage1_ڊيگهائي) {
            ps2=stage12+st2;
            لكل(st2=0; st2<64; ++st2) {
                إذا((st3=(int32_t)ps2[st2]<<UCNV_EXT_STAGE_2_کاٻو_سرڪايو)!=0) {
                    /* read the stage 3 block */
                    ps3=stage3+st3;

                    افعل {
                        value=stage3b[*ps3++];
                        إذا(value==0) {
                            /* no mapping, do nothing */
                        } وإلا إذا(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                            // Recurse for partial results.
                            length=0;
                            U16_APPEND_UNSAFE(s, length, c);
                            ucnv_ext_Get_Unicode_سيٽ_String(
                                shared_معلومات, cx, sa, which, min_ڊيگهائي,
                                c, s, length,
                                (int32_t)UCNV_EXT_FROM_U_GET_PARTIAL_اشاريو(value),
                                p_Error_ڪوڊ);
                        } وإلا إذا(ext_سيٽ_Use_Mapping(which, min_ڊيگهائي, value)) {
                            بدل(filter) {
                            حالة UCNV_سيٽ_FILTER_2022_CN:
                                إذا(!(UCNV_EXT_FROM_U_GET_ڊيگهائي(value)==3 && UCNV_EXT_FROM_U_GET_معلومات(value)<=0x82ffff)) {
                                    استمر;
                                }
                                اكسر;
                            حالة UCNV_سيٽ_FILTER_SJIS:
                                إذا(!(UCNV_EXT_FROM_U_GET_ڊيگهائي(value)==2 && (value=UCNV_EXT_FROM_U_GET_معلومات(value))>=0x8140 && value<=0xeffc)) {
                                    استمر;
                                }
                                اكسر;
                            حالة UCNV_سيٽ_FILTER_GR94_DBCS:
                                إذا(!(UCNV_EXT_FROM_U_GET_ڊيگهائي(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_معلومات(value))-0xa1a1)<=(0xfefe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    استمر;
                                }
                                اكسر;
                            حالة UCNV_سيٽ_FILTER_HZ:
                                إذا(!(UCNV_EXT_FROM_U_GET_ڊيگهائي(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_معلومات(value))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    استمر;
                                }
                                اكسر;
                            افتراضي:
                                /*
                                 * UCNV_SET_FILTER_NONE,
                                 * or UCNV_SET_FILTER_DBCS_ONLY which is handled via minLength
                                 */
                                اكسر;
                            }
                            sa->جوڙيو(sa->سيٽ, c);
                        }
                    } طالما((++c&0xf)!=0);
                } وإلا {
                    c+=16; /* empty stage 3 block */
                }
            }
        } وإلا {
            c+=1024; /* empty stage 2 block */
        }
    }
}

#جيڪڏهن_پورو /* #if !UCONFIG_NO_LEGACY_CONVERSION */
