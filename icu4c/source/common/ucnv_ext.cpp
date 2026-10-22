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
ucnv_ext_پیدا_کړه_منزل_U(ثابت uint32_t *منزل_USection, int32_t length, uint8_t byte) {
    uint32_t word0, کلمه;
    int32_t i, پیل, حد;

    /* check the input byte against the lowest and highest section bytes */
    پیل = static_cast<int32_t>(UCNV_EXT_منزل_U_GET_BYTE(منزل_USection[0]));
    حد = static_cast<int32_t>(UCNV_EXT_منزل_U_GET_BYTE(منزل_USection[length - 1]));
    إذا(byte<پیل || حد<byte) {
        أرجع 0; /* the byte is out of range */
    }

    إذا(length==((حد-پیل)+1)) {
        /* direct access on a linear array */
        أرجع UCNV_EXT_منزل_U_GET_ارزښت(منزل_USection[byte-پیل]); /* could be 0 */
    }

    /* word0 is suitable for <=toUSection[] comparison, word for <toUSection[] */
    word0=UCNV_EXT_منزل_U_MAKE_کلمه(byte, 0);

    /*
     * Shift byte once instead of each section word and add 0xffffff.
     * We will compare the shifted/added byte (bbffffff) against
     * section words which have byte values in the same bit position.
     * If and only if byte bb < section byte ss then bbffffff<ssvvvvvv
     * for all v=0..f
     * so we need not mask off the lower 24 bits of each section word.
     */
    کلمه=word0|UCNV_EXT_منزل_U_ارزښت_ماسک;

    /* binary search */
    پیل=0;
    حد=length;
    لكل(;;) {
        i=حد-پیل;
        إذا(i<=1) {
            اكسر; /*بشپړ*/
        }
        /* start<limit-1 */

        إذا(i<=4) {
            /* linear search for the last part */
            إذا(word0<=منزل_USection[پیل]) {
                اكسر;
            }
            إذا(++پیل<حد && word0<=منزل_USection[پیل]) {
                اكسر;
            }
            إذا(++پیل<حد && word0<=منزل_USection[پیل]) {
                اكسر;
            }
            /* always break at start==limit-1 */
            ++پیل;
            اكسر;
        }

        i=(پیل+حد)/2;
        إذا(کلمه<منزل_USection[i]) {
            حد=i;
        } وإلا {
            پیل=i;
        }
    }

    /* did we really find it? */
    إذا(پیل<حد && byte==UCNV_EXT_منزل_U_GET_BYTE(کلمه=منزل_USection[پیل])) {
        أرجع UCNV_EXT_منزل_U_GET_ارزښت(کلمه); /* never 0 */
    } وإلا {
        أرجع 0; /* not found */
    }
}

/*
 * true if not an SI/SO stateful converter,
 * or if the match length fits with the current converter state
 */
#تعریف_کړه UCNV_EXT_منزل_U_VERIFY_SISO_سمون(sisoState, سمون) \
    ((sisoState)<0 || ((sisoState)==0) == (سمون==1))

/*
 * this works like ucnv_extMatchFromU() except
 * - the first character is in pre
 * - no trie is used
 * - the returned matchLength is not offset by 2
 */
ساكن int32_t
ucnv_ext_سمون_منزل_U(ثابت int32_t *cx, int8_t sisoState,
                 ثابت حرف *pre, int32_t pre_اوږدتیا,
                 ثابت حرف *src, int32_t src_اوږدتیا,
                 uint32_t *p_سمون_ارزښت,
                 UBool /*useFallback*/, UBool flush) {
    ثابت uint32_t *منزل_UTable, *منزل_USection;

    uint32_t value, سمون_ارزښت;
    int32_t i, j, idx, length, سمون_اوږدتیا;
    uint8_t b;

    إذا(cx==تش_اشاره || cx[UCNV_EXT_منزل_U_اوږدتیا]<=0) {
        أرجع 0; /* no extension data, no match */
    }

    /* initialize */
    منزل_UTable=UCNV_EXT_ARRAY(cx, UCNV_EXT_منزل_U_شاخص, uint32_t);
    idx=0;

    سمون_ارزښت=0;
    i=j=سمون_اوږدتیا=0;

    إذا(sisoState==0) {
        /* SBCS state of an SI/SO stateful converter, look at only exactly 1 byte */
        إذا(pre_اوږدتیا>1) {
            أرجع 0; /* no match of a DBCS sequence in SBCS mode */
        } وإلا إذا(pre_اوږدتیا==1) {
            src_اوږدتیا=0;
        } وإلا /* preLength==0 */ {
            إذا(src_اوږدتیا>1) {
                src_اوږدتیا=1;
            }
        }
        flush=ریښتیا;
    }

    /* we must not remember fallback matches when not using fallbacks */

    /* match input units until there is a full match or the input is consumed */
    لكل(;;) {
        /* go to the next section */
        منزل_USection=منزل_UTable+idx;

        /* read first pair of the section */
        value=*منزل_USection++;
        length=UCNV_EXT_منزل_U_GET_BYTE(value);
        value=UCNV_EXT_منزل_U_GET_ارزښت(value);
        إذا( value!=0 &&
            (UCNV_EXT_منزل_U_IS_ROUNDTRIP(value) ||
             منزل_U_USE_FALLBACK(useFallback)) &&
            UCNV_EXT_منزل_U_VERIFY_SISO_سمون(sisoState, i+j)
        ) {
            /* remember longest match so far */
            سمون_ارزښت=value;
            سمون_اوږدتیا=i+j;
        }

        /* match pre[] then src[] */
        إذا(i<pre_اوږدتیا) {
            b = static_cast<uint8_t>(pre[i++]);
        } وإلا إذا(j<src_اوږدتیا) {
            b = static_cast<uint8_t>(src[j++]);
        } وإلا {
            /* all input consumed, partial match */
            إذا(flush || (length=(i+j))>UCNV_EXT_MAX_بایټونه) {
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
        value=ucnv_ext_پیدا_کړه_منزل_U(منزل_USection, length, b);
        إذا(value==0) {
            /* no match here, stop with the longest match so far */
            اكسر;
        } وإلا {
            إذا(UCNV_EXT_منزل_U_IS_PARTIAL(value)) {
                /* partial match, continue */
                idx = static_cast<int32_t>(UCNV_EXT_منزل_U_GET_PARTIAL_شاخص(value));
            } وإلا {
                إذا( (UCNV_EXT_منزل_U_IS_ROUNDTRIP(value) ||
                     منزل_U_USE_FALLBACK(useFallback)) &&
                    UCNV_EXT_منزل_U_VERIFY_SISO_سمون(sisoState, i+j)
                ) {
                    /* full match, stop with result */
                    سمون_ارزښت=value;
                    سمون_اوږدتیا=i+j;
                } وإلا {
                    /* full match on fallback not taken, stop with the longest match so far */
                }
                اكسر;
            }
        }
    }

    إذا(سمون_اوږدتیا==0) {
        /* no match at all */
        أرجع 0;
    }

    /* return result */
    *p_سمون_ارزښت=UCNV_EXT_منزل_U_ماسک_ROUNDTRIP(سمون_ارزښت);
    أرجع سمون_اوږدتیا;
}

ساكن مضمن فراغ
ucnv_ext_Write_منزل_U(UConverter *cnv, ثابت int32_t *cx,
                 uint32_t value,
                 char16_t **موخه, ثابت char16_t *موخه_حد,
                 int32_t **offsets, int32_t src_شاخص,
                 UError_کوډ *p_Error_کوډ) {
    /* output the result */
    إذا(UCNV_EXT_منزل_U_IS_کوډ_ټکی(value)) {
        /* output a single code point */
        ucnv_منزل_UWrite_کوډ_ټکی(
            cnv, UCNV_EXT_منزل_U_GET_کوډ_ټکی(value),
            موخه, موخه_حد,
            offsets, src_شاخص,
            p_Error_کوډ);
    } وإلا {
        /* output a string - with correct data we have resultLength>0 */
        ucnv_منزل_UWrite_UChars(
            cnv,
            UCNV_EXT_ARRAY(cx, UCNV_EXT_منزل_U_UCHARS_شاخص, char16_t)+
                UCNV_EXT_منزل_U_GET_شاخص(value),
            UCNV_EXT_منزل_U_GET_اوږدتیا(value),
            موخه, موخه_حد,
            offsets, src_شاخص,
            p_Error_کوډ);
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
#تعریف_کړه UCNV_SISO_STATE(cnv) \
    ((cnv)->shared_معلومات->mbcs.outputType==MBCS_OUTPUT_2_SISO ? (int8_t)(cnv)->ډېرپېښ : \
     (cnv)->shared_معلومات->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ? 1 : -1)

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_سمون_منزل_U(UConverter *cnv, ثابت int32_t *cx,
                        int32_t لومړی_اوږدتیا,
                        ثابت حرف **src, ثابت حرف *src_حد,
                        char16_t **موخه, ثابت char16_t *موخه_حد,
                        int32_t **offsets, int32_t src_شاخص,
                        UBool flush,
                        UError_کوډ *p_Error_کوډ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t سمون;

    /* try to match */
    سمون=ucnv_ext_سمون_منزل_U(cx, (int8_t)UCNV_SISO_STATE(cnv),
                           (ثابت حرف *)cnv->منزل_UBytes, لومړی_اوږدتیا,
                           *src, (int32_t)(src_حد-*src),
                           &value,
                           cnv->useFallback, flush);
    إذا(سمون>0) {
        /* advance src pointer for the consumed input */
        *src+=سمون-لومړی_اوږدتیا;

        /* write result to target */
        ucnv_ext_Write_منزل_U(cnv, cx,
                         value,
                         موخه, موخه_حد,
                         offsets, src_شاخص,
                         p_Error_کوډ);
        أرجع ریښتیا;
    } وإلا إذا(سمون<0) {
        /* save state for partial match */
        ثابت حرف *s;
        int32_t j;

        /* copy the first code point */
        s=(ثابت حرف *)cnv->منزل_UBytes;
        cnv->pre_منزل_UFirst_اوږدتیا=(int8_t)لومړی_اوږدتیا;
        لكل(j=0; j<لومړی_اوږدتیا; ++j) {
            cnv->pre_منزل_U[j]=*s++;
        }

        /* now copy the newly consumed input */
        s=*src;
        سمون=-سمون;
        لكل(; j<سمون; ++j) {
            cnv->pre_منزل_U[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_منزل_ULength=(int8_t)سمون;
        أرجع ریښتیا;
    } وإلا /* match==0 no match */ {
        أرجع خطأ;
    }
}

U_CFUNC UChar32
ucnv_ext_Simple_سمون_منزل_U(ثابت int32_t *cx,
                       ثابت حرف *سرچینه, int32_t length,
                       UBool useFallback) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t سمون;

    إذا(length<=0) {
        أرجع 0xffff;
    }

    /* try to match */
    سمون=ucnv_ext_سمون_منزل_U(cx, -1,
                           سرچینه, length,
                           تش_اشاره, 0,
                           &value,
                           useFallback, ریښتیا);
    إذا(سمون==length) {
        /* write result for simple, single-character conversion */
        إذا(UCNV_EXT_منزل_U_IS_کوډ_ټکی(value)) {
            أرجع UCNV_EXT_منزل_U_GET_کوډ_ټکی(value);
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
ucnv_ext_Continue_سمون_منزل_U(UConverter *cnv,
                         UConverter_منزل_Unicode_دلیلونه *p_دلیلونه, int32_t src_شاخص,
                         UError_کوډ *p_Error_کوډ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t سمون, length;

    سمون=ucnv_ext_سمون_منزل_U(cnv->shared_معلومات->mbcs.extIndexes, (int8_t)UCNV_SISO_STATE(cnv),
                           cnv->pre_منزل_U, cnv->pre_منزل_ULength,
                           p_دلیلونه->سرچینه, (int32_t)(p_دلیلونه->سرچینه_حد-p_دلیلونه->سرچینه),
                           &value,
                           cnv->useFallback, p_دلیلونه->flush);
    إذا(سمون>0) {
        إذا(سمون>=cnv->pre_منزل_ULength) {
            /* advance src pointer for the consumed input */
            p_دلیلونه->سرچینه+=سمون-cnv->pre_منزل_ULength;
            cnv->pre_منزل_ULength=0;
        } وإلا {
            /* the match did not use all of preToU[] - keep the rest for replay */
            length=cnv->pre_منزل_ULength-سمون;
            uprv_memmove(cnv->pre_منزل_U, cnv->pre_منزل_U+سمون, length);
            cnv->pre_منزل_ULength=(int8_t)-length;
        }

        /* write result */
        ucnv_ext_Write_منزل_U(cnv, cnv->shared_معلومات->mbcs.extIndexes,
                         value,
                         &p_دلیلونه->موخه, p_دلیلونه->موخه_حد,
                         &p_دلیلونه->offsets, src_شاخص,
                         p_Error_کوډ);
    } وإلا إذا(سمون<0) {
        /* save state for partial match */
        ثابت حرف *s;
        int32_t j;

        /* just _append_ the newly consumed input to preToU[] */
        s=p_دلیلونه->سرچینه;
        سمون=-سمون;
        لكل(j=cnv->pre_منزل_ULength; j<سمون; ++j) {
            cnv->pre_منزل_U[j]=*s++;
        }
        p_دلیلونه->سرچینه=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_منزل_ULength=(int8_t)سمون;
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
        uprv_memcpy(cnv->منزل_UBytes, cnv->pre_منزل_U, cnv->pre_منزل_UFirst_اوږدتیا);
        cnv->منزل_ULength=cnv->pre_منزل_UFirst_اوږدتیا;

        /* move the rest up inside the buffer */
        length=cnv->pre_منزل_ULength-cnv->pre_منزل_UFirst_اوږدتیا;
        إذا(length>0) {
            uprv_memmove(cnv->pre_منزل_U, cnv->pre_منزل_U+cnv->pre_منزل_UFirst_اوږدتیا, length);
        }

        /* mark preToU for replay */
        cnv->pre_منزل_ULength=(int8_t)-length;

        /* set the error code for unassigned */
        *p_Error_کوډ=U_INVALID_CHAR_وموندل;
    }
}

/* from Unicode ------------------------------------------------------------- */

// Use roundtrips, "good one-way" mappings, and some normal fallbacks.
ساكن مضمن UBool
extFromUUseMapping(UBool useFallback, uint32_t value, UChar32 لومړی_CP) {
    أرجع
        ((value&UCNV_EXT_FROM_U_حالت_ماسک)!=0 ||
            FROM_U_USE_FALLBACK(useFallback, لومړی_CP)) &&
        (value&UCNV_EXT_FROM_U_RESERVED_ماسک)==0;
}

/*
 * @return index of the char16_t, if found; else <0
 */
ساكن مضمن int32_t
ucnv_ext_پیدا_کړه_From_U(ثابت char16_t *fromUSection, int32_t length, char16_t u) {
    int32_t i, پیل, حد;

    /* binary search */
    پیل=0;
    حد=length;
    لكل(;;) {
        i=حد-پیل;
        إذا(i<=1) {
            اكسر; /*بشپړ*/
        }
        /* start<limit-1 */

        إذا(i<=4) {
            /* linear search for the last part */
            إذا(u<=fromUSection[پیل]) {
                اكسر;
            }
            إذا(++پیل<حد && u<=fromUSection[پیل]) {
                اكسر;
            }
            إذا(++پیل<حد && u<=fromUSection[پیل]) {
                اكسر;
            }
            /* always break at start==limit-1 */
            ++پیل;
            اكسر;
        }

        i=(پیل+حد)/2;
        إذا(u<fromUSection[i]) {
            حد=i;
        } وإلا {
            پیل=i;
        }
    }

    /* did we really find it? */
    إذا(پیل<حد && u==fromUSection[پیل]) {
        أرجع پیل;
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
ucnv_ext_سمون_From_U(ثابت int32_t *cx,
                   UChar32 لومړی_CP,
                   ثابت char16_t *pre, int32_t pre_اوږدتیا,
                   ثابت char16_t *src, int32_t src_اوږدتیا,
                   uint32_t *p_سمون_ارزښت,
                   UBool useFallback, UBool flush) {
    ثابت uint16_t *stage12, *stage3;
    ثابت uint32_t *stage3b;

    ثابت char16_t *fromUTableUChars, *fromUSectionUChars;
    ثابت uint32_t *from_UTable_ارزښتونه, *from_USection_ارزښتونه;

    uint32_t value, سمون_ارزښت;
    int32_t i, j, idx, length, سمون_اوږدتیا;
    char16_t c;

    إذا(cx==تش_اشاره) {
        أرجع 0; /* no extension data, no match */
    }

    /* trie lookup of firstCP */
    idx=لومړی_CP>>10; /* stage 1 index */
    إذا(idx>=cx[UCNV_EXT_FROM_U_STAGE_1_اوږدتیا]) {
        أرجع 0; /* the first code point is outside the trie */
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_شاخص, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_شاخص, uint16_t);
    idx=UCNV_EXT_FROM_U(stage12, stage3, idx, لومړی_CP);

    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_شاخص, uint32_t);
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
        idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_شاخص(value));

        /* initialize */
        fromUTableUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_شاخص, char16_t);
        from_UTable_ارزښتونه=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_ارزښتونه_شاخص, uint32_t);

        سمون_ارزښت=0;
        i=j=سمون_اوږدتیا=0;

        /* we must not remember fallback matches when not using fallbacks */

        /* match input units until there is a full match or the input is consumed */
        لكل(;;) {
            /* go to the next section */
            fromUSectionUChars=fromUTableUChars+idx;
            from_USection_ارزښتونه=from_UTable_ارزښتونه+idx;

            /* read first pair of the section */
            length=*fromUSectionUChars++;
            value=*from_USection_ارزښتونه++;
            إذا(value!=0 && extFromUUseMapping(useFallback, value, لومړی_CP)) {
                /* remember longest match so far */
                سمون_ارزښت=value;
                سمون_اوږدتیا=2+i+j;
            }

            /* match pre[] then src[] */
            إذا(i<pre_اوږدتیا) {
                c=pre[i++];
            } وإلا إذا(j<src_اوږدتیا) {
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
            idx=ucnv_ext_پیدا_کړه_From_U(fromUSectionUChars, length, c);
            إذا(idx<0) {
                /* no match here, stop with the longest match so far */
                اكسر;
            } وإلا {
                value=from_USection_ارزښتونه[idx];
                إذا(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                    /* partial match, continue */
                    idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_شاخص(value));
                } وإلا {
                    إذا(extFromUUseMapping(useFallback, value, لومړی_CP)) {
                        /* full match, stop with result */
                        سمون_ارزښت=value;
                        سمون_اوږدتیا=2+i+j;
                    } وإلا {
                        /* full match on fallback not taken, stop with the longest match so far */
                    }
                    اكسر;
                }
            }
        }

        إذا(سمون_اوږدتیا==0) {
            /* no match at all */
            أرجع 0;
        }
    } وإلا /* result from firstCP trie lookup */ {
        إذا(extFromUUseMapping(useFallback, value, لومړی_CP)) {
            /* full match, stop with result */
            سمون_ارزښت=value;
            سمون_اوږدتیا=2;
        } وإلا {
            /* fallback not taken */
            أرجع 0;
        }
    }

    /* return result */
    إذا(سمون_ارزښت==UCNV_EXT_FROM_U_SUBCHAR1) {
        أرجع 1; /* assert matchLength==2 */
    }

    *p_سمون_ارزښت=سمون_ارزښت;
    أرجع سمون_اوږدتیا;
}

/*
 * @param value fromUnicode mapping table value; ignores roundtrip and reserved bits
 */
ساكن مضمن فراغ
ucnv_extWriteFromU(UConverter *cnv, ثابت int32_t *cx,
                   uint32_t value,
                   حرف **موخه, ثابت حرف *موخه_حد,
                   int32_t **offsets, int32_t src_شاخص,
                   UError_کوډ *p_Error_کوډ) {
    uint8_t بفر[1+UCNV_EXT_MAX_بایټونه];
    ثابت uint8_t *پایله;
    int32_t length, prev_اوږدتیا;

    length=UCNV_EXT_FROM_U_GET_اوږدتیا(value);
    value = UCNV_EXT_FROM_U_GET_معلومات(value);

    /* output the result */
    إذا(length<=UCNV_EXT_FROM_U_MAX_مستقیم_اوږدتیا) {
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
        پایله=بفر+1;
    } وإلا {
        پایله=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_بایټونه_شاخص, uint8_t)+value;
    }

    /* with correct data we have length>0 */

    إذا((prev_اوږدتیا=cnv->from_Unicode_حالت)!=0) {
        /* handle SI/SO stateful output */
        uint8_t ښويدنه_Byte;

        إذا(prev_اوږدتیا>1 && length==1) {
            /* change from double-byte mode to single-byte */
            ښويدنه_Byte = static_cast<uint8_t>(UCNV_SI);
            cnv->from_Unicode_حالت=1;
        } وإلا إذا(prev_اوږدتیا==1 && length>1) {
            /* change from single-byte mode to double-byte */
            ښويدنه_Byte = static_cast<uint8_t>(UCNV_SO);
            cnv->from_Unicode_حالت=2;
        } وإلا {
            ښويدنه_Byte=0;
        }

        إذا(ښويدنه_Byte!=0) {
            /* prepend the shift byte to the result bytes */
            بفر[0]=ښويدنه_Byte;
            إذا(پایله!=بفر+1) {
                uprv_memcpy(بفر+1, پایله, length);
            }
            پایله=بفر;
            ++length;
        }
    }

    ucnv_from_UWrite_بایټونه(cnv, reinterpret_cast<ثابت حرف*>(پایله), length,
                         موخه, موخه_حد,
                         offsets, src_شاخص,
                         p_Error_کوډ);
}

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_سمون_From_U(UConverter *cnv, ثابت int32_t *cx,
                          UChar32 cp,
                          ثابت char16_t **src, ثابت char16_t *src_حد,
                          حرف **موخه, ثابت حرف *موخه_حد,
                          int32_t **offsets, int32_t src_شاخص,
                          UBool flush,
                          UError_کوډ *p_Error_کوډ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t سمون;

    /* try to match */
    سمون=ucnv_ext_سمون_From_U(cx, cp,
                             تش_اشاره, 0,
                             *src, (int32_t)(src_حد-*src),
                             &value,
                             cnv->useFallback, flush);

    /* reject a match if the result is a single byte for DBCS-only */
    إذا( سمون>=2 &&
        !(UCNV_EXT_FROM_U_GET_اوږدتیا(value)==1 &&
          cnv->shared_معلومات->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY)
    ) {
        /* advance src pointer for the consumed input */
        *src+=سمون-2; /* remove 2 for the initial code point */

        /* write result to target */
        ucnv_extWriteFromU(cnv, cx,
                           value,
                           موخه, موخه_حد,
                           offsets, src_شاخص,
                           p_Error_کوډ);
        أرجع ریښتیا;
    } وإلا إذا(سمون<0) {
        /* save state for partial match */
        ثابت char16_t *s;
        int32_t j;

        /* copy the first code point */
        cnv->preFromUFirstCP=cp;

        /* now copy the newly consumed input */
        s=*src;
        سمون=-سمون-2; /* remove 2 for the initial code point */
        لكل(j=0; j<سمون; ++j) {
            cnv->preFromU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)سمون;
        أرجع ریښتیا;
    } وإلا إذا(سمون==1) {
        /* matched, no mapping but request for <subchar1> */
        cnv->useSubChar1=ریښتیا;
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
ucnv_ext_Simple_سمون_From_U(ثابت int32_t *cx,
                         UChar32 cp, uint32_t *p_ارزښت,
                         UBool useFallback) {
    uint32_t value;
    int32_t سمون;

    /* try to match */
    سمون=ucnv_ext_سمون_From_U(cx,
                             cp,
                             تش_اشاره, 0,
                             تش_اشاره, 0,
                             &value,
                             useFallback, ریښتیا);
    إذا(سمون>=2) {
        /* write result for simple, single-character conversion */
        int32_t length;
        صحيح isRoundtrip;

        isRoundtrip=UCNV_EXT_FROM_U_IS_ROUNDTRIP(value);
        length=UCNV_EXT_FROM_U_GET_اوږدتیا(value);
        value = UCNV_EXT_FROM_U_GET_معلومات(value);

        إذا(length<=UCNV_EXT_FROM_U_MAX_مستقیم_اوږدتیا) {
            *p_ارزښت=value;
            أرجع isRoundtrip ? length : -length;
#إذا 0 /* not currently used */
        } وإلا إذا(length==4) {
            /* de-serialize a 4-byte result */
            ثابت uint8_t *پایله=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_بایټونه_شاخص, uint8_t)+value;
            *p_ارزښت=
                ((uint32_t)پایله[0]<<24)|
                ((uint32_t)پایله[1]<<16)|
                ((uint32_t)پایله[2]<<8)|
                پایله[3];
            أرجع isRoundtrip ? 4 : -4;
#د_که_پای
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
ucnv_ext_Continue_سمون_From_U(UConverter *cnv,
                           UConverter_From_Unicode_دلیلونه *p_دلیلونه, int32_t src_شاخص,
                           UError_کوډ *p_Error_کوډ) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t سمون;

    سمون=ucnv_ext_سمون_From_U(cnv->shared_معلومات->mbcs.extIndexes,
                             cnv->preFromUFirstCP,
                             cnv->preFromU, cnv->preFromULength,
                             p_دلیلونه->سرچینه, (int32_t)(p_دلیلونه->سرچینه_حد-p_دلیلونه->سرچینه),
                             &value,
                             cnv->useFallback, p_دلیلونه->flush);
    إذا(سمون>=2) {
        سمون-=2; /* remove 2 for the initial code point */

        إذا(سمون>=cnv->preFromULength) {
            /* advance src pointer for the consumed input */
            p_دلیلونه->سرچینه+=سمون-cnv->preFromULength;
            cnv->preFromULength=0;
        } وإلا {
            /* the match did not use all of preFromU[] - keep the rest for replay */
            int32_t length=cnv->preFromULength-سمون;
            u_memmove(cnv->preFromU, cnv->preFromU+سمون, length);
            cnv->preFromULength=(int8_t)-length;
        }

        /* finish the partial match */
        cnv->preFromUFirstCP=U_SENTINEL;

        /* write result */
        ucnv_extWriteFromU(cnv, cnv->shared_معلومات->mbcs.extIndexes,
                           value,
                           &p_دلیلونه->موخه, p_دلیلونه->موخه_حد,
                           &p_دلیلونه->offsets, src_شاخص,
                           p_Error_کوډ);
    } وإلا إذا(سمون<0) {
        /* save state for partial match */
        ثابت char16_t *s;
        int32_t j;

        /* just _append_ the newly consumed input to preFromU[] */
        s=p_دلیلونه->سرچینه;
        سمون=-سمون-2; /* remove 2 for the initial code point */
        لكل(j=cnv->preFromULength; j<سمون; ++j) {
            U_ASSERT(j>=0);
            cnv->preFromU[j]=*s++;
        }
        p_دلیلونه->سرچینه=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)سمون;
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

        إذا(سمون==1) {
            /* matched, no mapping but request for <subchar1> */
            cnv->useSubChar1=ریښتیا;
        }

        /* move the first code point to the error field */
        cnv->fromUChar32=cnv->preFromUFirstCP;
        cnv->preFromUFirstCP=U_SENTINEL;

        /* mark preFromU for replay */
        cnv->preFromULength=-cnv->preFromULength;

        /* set the error code for unassigned */
        *p_Error_کوډ=U_INVALID_CHAR_وموندل;
    }
}

ساكن UBool
ext_سېټ_Use_Mapping(UConverter_Unicode_سېټ which, int32_t min_اوږدتیا, uint32_t value) {
    إذا(which==UCNV_ROUNDTRIP_سېټ) {
        // Add only code points for which the roundtrip flag is set.
        // Do not add any fallbacks, even if ucnv_fromUnicode() would use them
        // (fallbacks from PUA). See the API docs for ucnv_getUnicodeSet().
        //
        // By analogy, also do not add "good one-way" mappings.
        //
        // Do not add entries with reserved bits set.
        إذا(((value&(UCNV_EXT_FROM_U_ROUNDTRIP_FLAG|UCNV_EXT_FROM_U_RESERVED_ماسک))!=
                UCNV_EXT_FROM_U_ROUNDTRIP_FLAG)) {
            أرجع خطأ;
        }
    } وإلا /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
        // Do not add entries with reserved bits set.
        إذا((value&UCNV_EXT_FROM_U_RESERVED_ماسک)!=0) {
            أرجع خطأ;
        }
    }
    // Do not add <subchar1> entries or other (future?) pseudo-entries
    // with an output length of 0.
    أرجع UCNV_EXT_FROM_U_GET_اوږدتیا(value)>=min_اوږدتیا;
}

ساكن فراغ
ucnv_ext_Get_Unicode_سېټ_String(ثابت UConverter_Shared_معلومات *shared_معلومات,
                            ثابت int32_t *cx,
                            ثابت USetAdder *sa,
                            UConverter_Unicode_سېټ which,
                            int32_t min_اوږدتیا,
                            UChar32 لومړی_CP,
                            char16_t s[UCNV_EXT_MAX_UCHARS], int32_t length,
                            int32_t section_شاخص,
                            UError_کوډ *p_Error_کوډ) {
    ثابت char16_t *fromUSectionUChars;
    ثابت uint32_t *from_USection_ارزښتونه;

    uint32_t value;
    int32_t i, count;

    fromUSectionUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_شاخص, char16_t)+section_شاخص;
    from_USection_ارزښتونه=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_ارزښتونه_شاخص, uint32_t)+section_شاخص;

    /* read first pair of the section */
    count=*fromUSectionUChars++;
    value=*from_USection_ارزښتونه++;

    إذا(ext_سېټ_Use_Mapping(which, min_اوږدتیا, value)) {
        إذا(length==U16_اوږدتیا(لومړی_CP)) {
            /* add the initial code point */
            sa->جمع_کړه(sa->سېټ, لومړی_CP);
        } وإلا {
            /* add the string so far */
            sa->جمع_کړه_String(sa->سېټ, s, length);
        }
    }

    لكل(i=0; i<count; ++i) {
        /* append this code unit and recurse or add the string */
        s[length]=fromUSectionUChars[i];
        value=from_USection_ارزښتونه[i];

        إذا(value==0) {
            /* no mapping, do nothing */
        } وإلا إذا(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
            ucnv_ext_Get_Unicode_سېټ_String(
                shared_معلومات, cx, sa, which, min_اوږدتیا,
                لومړی_CP, s, length+1,
                static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_شاخص(value)),
                p_Error_کوډ);
        } وإلا إذا(ext_سېټ_Use_Mapping(which, min_اوږدتیا, value)) {
            sa->جمع_کړه_String(sa->سېټ, s, length+1);
        }
    }
}

U_CFUNC فراغ
ucnv_ext_Get_Unicode_سېټ(ثابت UConverter_Shared_معلومات *shared_معلومات,
                      ثابت USetAdder *sa,
                      UConverter_Unicode_سېټ which,
                      UConverter_سېټ_Filter filter,
                      UError_کوډ *p_Error_کوډ) {
    ثابت int32_t *cx;
    ثابت uint16_t *stage12, *stage3, *ps2, *ps3;
    ثابت uint32_t *stage3b;

    uint32_t value;
    int32_t st1, stage1_اوږدتیا, st2, st3, min_اوږدتیا;

    char16_t s[UCNV_EXT_MAX_UCHARS];
    UChar32 c;
    int32_t length;

    cx=shared_معلومات->mbcs.extIndexes;
    إذا(cx==تش_اشاره) {
        أرجع;
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_شاخص, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_شاخص, uint16_t);
    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_شاخص, uint32_t);

    stage1_اوږدتیا=cx[UCNV_EXT_FROM_U_STAGE_1_اوږدتیا];

    /* enumerate the from-Unicode trie table */
    c=0; /* keep track of the current code point while enumerating */

    إذا(filter==UCNV_سېټ_FILTER_2022_CN) {
        min_اوږدتیا=3;
    } وإلا إذا( shared_معلومات->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ||
               filter!=UCNV_سېټ_FILTER_NONE
    ) {
        /* DBCS-only, ignore single-byte results */
        min_اوږدتیا=2;
    } وإلا {
        min_اوږدتیا=1;
    }

    /*
     * the trie enumeration is almost the same as
     * in MBCSGetUnicodeSet() for MBCS_OUTPUT_1
     */
    لكل(st1=0; st1<stage1_اوږدتیا; ++st1) {
        st2=stage12[st1];
        إذا(st2>stage1_اوږدتیا) {
            ps2=stage12+st2;
            لكل(st2=0; st2<64; ++st2) {
                إذا((st3=(int32_t)ps2[st2]<<UCNV_EXT_STAGE_2_کیڼ_ښويدنه)!=0) {
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
                            ucnv_ext_Get_Unicode_سېټ_String(
                                shared_معلومات, cx, sa, which, min_اوږدتیا,
                                c, s, length,
                                (int32_t)UCNV_EXT_FROM_U_GET_PARTIAL_شاخص(value),
                                p_Error_کوډ);
                        } وإلا إذا(ext_سېټ_Use_Mapping(which, min_اوږدتیا, value)) {
                            بدل(filter) {
                            حالة UCNV_سېټ_FILTER_2022_CN:
                                إذا(!(UCNV_EXT_FROM_U_GET_اوږدتیا(value)==3 && UCNV_EXT_FROM_U_GET_معلومات(value)<=0x82ffff)) {
                                    استمر;
                                }
                                اكسر;
                            حالة UCNV_سېټ_FILTER_SJIS:
                                إذا(!(UCNV_EXT_FROM_U_GET_اوږدتیا(value)==2 && (value=UCNV_EXT_FROM_U_GET_معلومات(value))>=0x8140 && value<=0xeffc)) {
                                    استمر;
                                }
                                اكسر;
                            حالة UCNV_سېټ_FILTER_GR94_DBCS:
                                إذا(!(UCNV_EXT_FROM_U_GET_اوږدتیا(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_معلومات(value))-0xa1a1)<=(0xfefe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    استمر;
                                }
                                اكسر;
                            حالة UCNV_سېټ_FILTER_HZ:
                                إذا(!(UCNV_EXT_FROM_U_GET_اوږدتیا(value)==2 &&
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
                            sa->جمع_کړه(sa->سېټ, c);
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

#د_که_پای /* #if !UCONFIG_NO_LEGACY_CONVERSION */
