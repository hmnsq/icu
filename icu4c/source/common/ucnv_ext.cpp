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
ucnv_ext_پیدا_کن_مقصد_U(ثابت uint32_t *مقصد_USection, int32_t length, uint8_t byte) {
    uint32_t word0, کلمه;
    int32_t i, شروع, حد;

    /* check the input byte against the lowest and highest section bytes */
    شروع = static_cast<int32_t>(UCNV_EXT_مقصد_U_GET_BYTE(مقصد_USection[0]));
    حد = static_cast<int32_t>(UCNV_EXT_مقصد_U_GET_BYTE(مقصد_USection[length - 1]));
    إذا(byte<شروع || حد<byte) {
        أرجع 0; /* the byte is out of range */
    }

    إذا(length==((حد-شروع)+1)) {
        /* direct access on a linear array */
        أرجع UCNV_EXT_مقصد_U_GET_مقدار(مقصد_USection[byte-شروع]); /* could be 0 */
    }

    /* word0 is suitable for <=toUSection[] comparison, word for <toUSection[] */
    word0=UCNV_EXT_مقصد_U_MAKE_کلمه(byte, 0);

    /*
     * Shift byte once instead of each section word and add 0xffffff.
     * We will compare the shifted/added byte (bbffffff) against
     * section words which have byte values in the same bit position.
     * If and only if byte bb < section byte ss then bbffffff<ssvvvvvv
     * for all v=0..f
     * so we need not mask off the lower 24 bits of each section word.
     */
    کلمه=word0|UCNV_EXT_مقصد_U_مقدار_ماسک;

    /* binary search */
    شروع=0;
    حد=length;
    لكل(;;) {
        i=حد-شروع;
        إذا(i<=1) {
            اكسر; /*انجامشده*/
        }
        /* start<limit-1 */

        إذا(i<=4) {
            /* linear search for the last part */
            إذا(word0<=مقصد_USection[شروع]) {
                اكسر;
            }
            إذا(++شروع<حد && word0<=مقصد_USection[شروع]) {
                اكسر;
            }
            إذا(++شروع<حد && word0<=مقصد_USection[شروع]) {
                اكسر;
            }
            /* always break at start==limit-1 */
            ++شروع;
            اكسر;
        }

        i=(شروع+حد)/2;
        إذا(کلمه<مقصد_USection[i]) {
            حد=i;
        } وإلا {
            شروع=i;
        }
    }

    /* did we really find it? */
    إذا(شروع<حد && byte==UCNV_EXT_مقصد_U_GET_BYTE(کلمه=مقصد_USection[شروع])) {
        أرجع UCNV_EXT_مقصد_U_GET_مقدار(کلمه); /* never 0 */
    } وإلا {
        أرجع 0; /* not found */
    }
}

/*
 * true if not an SI/SO stateful converter,
 * or if the match length fits with the current converter state
 */
#تعریف_کن UCNV_EXT_مقصد_U_VERIFY_SISO_تطابق(sisoState, تطابق) \
    ((sisoState)<0 || ((sisoState)==0) == (تطابق==1))

/*
 * this works like ucnv_extMatchFromU() except
 * - the first character is in pre
 * - no trie is used
 * - the returned matchLength is not offset by 2
 */
ساكن int32_t
ucnv_ext_تطابق_مقصد_U(ثابت int32_t *cx, int8_t sisoState,
                 ثابت حرف *pre, int32_t pre_درازی,
                 ثابت حرف *src, int32_t src_درازی,
                 uint32_t *p_تطابق_مقدار,
                 UBool /*useFallback*/, UBool flush) {
    ثابت uint32_t *مقصد_UTable, *مقصد_USection;

    uint32_t value, تطابق_مقدار;
    int32_t i, j, idx, length, تطابق_درازی;
    uint8_t b;

    إذا(cx==اشاره_تهی || cx[UCNV_EXT_مقصد_U_درازی]<=0) {
        أرجع 0; /* no extension data, no match */
    }

    /* initialize */
    مقصد_UTable=UCNV_EXT_ARRAY(cx, UCNV_EXT_مقصد_U_شاخص, uint32_t);
    idx=0;

    تطابق_مقدار=0;
    i=j=تطابق_درازی=0;

    إذا(sisoState==0) {
        /* SBCS state of an SI/SO stateful converter, look at only exactly 1 byte */
        إذا(pre_درازی>1) {
            أرجع 0; /* no match of a DBCS sequence in SBCS mode */
        } وإلا إذا(pre_درازی==1) {
            src_درازی=0;
        } وإلا /* preLength==0 */ {
            إذا(src_درازی>1) {
                src_درازی=1;
            }
        }
        flush=درست;
    }

    /* we must not remember fallback matches when not using fallbacks */

    /* match input units until there is a full match or the input is consumed */
    لكل(;;) {
        /* go to the next section */
        مقصد_USection=مقصد_UTable+idx;

        /* read first pair of the section */
        value=*مقصد_USection++;
        length=UCNV_EXT_مقصد_U_GET_BYTE(value);
        value=UCNV_EXT_مقصد_U_GET_مقدار(value);
        إذا( value!=0 &&
            (UCNV_EXT_مقصد_U_IS_ROUNDTRIP(value) ||
             مقصد_U_USE_FALLBACK(useFallback)) &&
            UCNV_EXT_مقصد_U_VERIFY_SISO_تطابق(sisoState, i+j)
        ) {
            /* remember longest match so far */
            تطابق_مقدار=value;
            تطابق_درازی=i+j;
        }

        /* match pre[] then src[] */
        إذا(i<pre_درازی) {
            b = static_cast<uint8_t>(pre[i++]);
        } وإلا إذا(j<src_درازی) {
            b = static_cast<uint8_t>(src[j++]);
        } وإلا {
            /* all input consumed, partial match */
            إذا(flush || (length=(i+j))>UCNV_EXT_MAX_بایتها) {
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
        value=ucnv_ext_پیدا_کن_مقصد_U(مقصد_USection, length, b);
        إذا(value==0) {
            /* no match here, stop with the longest match so far */
            اكسر;
        } وإلا {
            إذا(UCNV_EXT_مقصد_U_IS_PARTIAL(value)) {
                /* partial match, continue */
                idx = static_cast<int32_t>(UCNV_EXT_مقصد_U_GET_PARTIAL_شاخص(value));
            } وإلا {
                إذا( (UCNV_EXT_مقصد_U_IS_ROUNDTRIP(value) ||
                     مقصد_U_USE_FALLBACK(useFallback)) &&
                    UCNV_EXT_مقصد_U_VERIFY_SISO_تطابق(sisoState, i+j)
                ) {
                    /* full match, stop with result */
                    تطابق_مقدار=value;
                    تطابق_درازی=i+j;
                } وإلا {
                    /* full match on fallback not taken, stop with the longest match so far */
                }
                اكسر;
            }
        }
    }

    إذا(تطابق_درازی==0) {
        /* no match at all */
        أرجع 0;
    }

    /* return result */
    *p_تطابق_مقدار=UCNV_EXT_مقصد_U_ماسک_ROUNDTRIP(تطابق_مقدار);
    أرجع تطابق_درازی;
}

ساكن مضمن فراغ
ucnv_ext_Write_مقصد_U(UConverter *cnv, ثابت int32_t *cx,
                 uint32_t value,
                 char16_t **هدف, ثابت char16_t *هدف_حد,
                 int32_t **offsets, int32_t src_شاخص,
                 UError_کد *p_Error_کد) {
    /* output the result */
    إذا(UCNV_EXT_مقصد_U_IS_کد_نقطه(value)) {
        /* output a single code point */
        ucnv_مقصد_UWrite_کد_نقطه(
            cnv, UCNV_EXT_مقصد_U_GET_کد_نقطه(value),
            هدف, هدف_حد,
            offsets, src_شاخص,
            p_Error_کد);
    } وإلا {
        /* output a string - with correct data we have resultLength>0 */
        ucnv_مقصد_UWrite_UChars(
            cnv,
            UCNV_EXT_ARRAY(cx, UCNV_EXT_مقصد_U_UCHARS_شاخص, char16_t)+
                UCNV_EXT_مقصد_U_GET_شاخص(value),
            UCNV_EXT_مقصد_U_GET_درازی(value),
            هدف, هدف_حد,
            offsets, src_شاخص,
            p_Error_کد);
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
#تعریف_کن UCNV_SISO_STATE(cnv) \
    ((cnv)->shared_داده->mbcs.outputType==MBCS_OUTPUT_2_SISO ? (int8_t)(cnv)->نما : \
     (cnv)->shared_داده->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ? 1 : -1)

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_تطابق_مقصد_U(UConverter *cnv, ثابت int32_t *cx,
                        int32_t اولی_درازی,
                        ثابت حرف **src, ثابت حرف *src_حد,
                        char16_t **هدف, ثابت char16_t *هدف_حد,
                        int32_t **offsets, int32_t src_شاخص,
                        UBool flush,
                        UError_کد *p_Error_کد) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t تطابق;

    /* try to match */
    تطابق=ucnv_ext_تطابق_مقصد_U(cx, (int8_t)UCNV_SISO_STATE(cnv),
                           (ثابت حرف *)cnv->مقصد_UBytes, اولی_درازی,
                           *src, (int32_t)(src_حد-*src),
                           &value,
                           cnv->useFallback, flush);
    إذا(تطابق>0) {
        /* advance src pointer for the consumed input */
        *src+=تطابق-اولی_درازی;

        /* write result to target */
        ucnv_ext_Write_مقصد_U(cnv, cx,
                         value,
                         هدف, هدف_حد,
                         offsets, src_شاخص,
                         p_Error_کد);
        أرجع درست;
    } وإلا إذا(تطابق<0) {
        /* save state for partial match */
        ثابت حرف *s;
        int32_t j;

        /* copy the first code point */
        s=(ثابت حرف *)cnv->مقصد_UBytes;
        cnv->pre_مقصد_UFirst_درازی=(int8_t)اولی_درازی;
        لكل(j=0; j<اولی_درازی; ++j) {
            cnv->pre_مقصد_U[j]=*s++;
        }

        /* now copy the newly consumed input */
        s=*src;
        تطابق=-تطابق;
        لكل(; j<تطابق; ++j) {
            cnv->pre_مقصد_U[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_مقصد_ULength=(int8_t)تطابق;
        أرجع درست;
    } وإلا /* match==0 no match */ {
        أرجع خطأ;
    }
}

U_CFUNC UChar32
ucnv_ext_Simple_تطابق_مقصد_U(ثابت int32_t *cx,
                       ثابت حرف *منبع, int32_t length,
                       UBool useFallback) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t تطابق;

    إذا(length<=0) {
        أرجع 0xffff;
    }

    /* try to match */
    تطابق=ucnv_ext_تطابق_مقصد_U(cx, -1,
                           منبع, length,
                           اشاره_تهی, 0,
                           &value,
                           useFallback, درست);
    إذا(تطابق==length) {
        /* write result for simple, single-character conversion */
        إذا(UCNV_EXT_مقصد_U_IS_کد_نقطه(value)) {
            أرجع UCNV_EXT_مقصد_U_GET_کد_نقطه(value);
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
ucnv_ext_Continue_تطابق_مقصد_U(UConverter *cnv,
                         UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها, int32_t src_شاخص,
                         UError_کد *p_Error_کد) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t تطابق, length;

    تطابق=ucnv_ext_تطابق_مقصد_U(cnv->shared_داده->mbcs.extIndexes, (int8_t)UCNV_SISO_STATE(cnv),
                           cnv->pre_مقصد_U, cnv->pre_مقصد_ULength,
                           p_آرگومانها->منبع, (int32_t)(p_آرگومانها->منبع_حد-p_آرگومانها->منبع),
                           &value,
                           cnv->useFallback, p_آرگومانها->flush);
    إذا(تطابق>0) {
        إذا(تطابق>=cnv->pre_مقصد_ULength) {
            /* advance src pointer for the consumed input */
            p_آرگومانها->منبع+=تطابق-cnv->pre_مقصد_ULength;
            cnv->pre_مقصد_ULength=0;
        } وإلا {
            /* the match did not use all of preToU[] - keep the rest for replay */
            length=cnv->pre_مقصد_ULength-تطابق;
            uprv_memmove(cnv->pre_مقصد_U, cnv->pre_مقصد_U+تطابق, length);
            cnv->pre_مقصد_ULength=(int8_t)-length;
        }

        /* write result */
        ucnv_ext_Write_مقصد_U(cnv, cnv->shared_داده->mbcs.extIndexes,
                         value,
                         &p_آرگومانها->هدف, p_آرگومانها->هدف_حد,
                         &p_آرگومانها->offsets, src_شاخص,
                         p_Error_کد);
    } وإلا إذا(تطابق<0) {
        /* save state for partial match */
        ثابت حرف *s;
        int32_t j;

        /* just _append_ the newly consumed input to preToU[] */
        s=p_آرگومانها->منبع;
        تطابق=-تطابق;
        لكل(j=cnv->pre_مقصد_ULength; j<تطابق; ++j) {
            cnv->pre_مقصد_U[j]=*s++;
        }
        p_آرگومانها->منبع=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_مقصد_ULength=(int8_t)تطابق;
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
        uprv_memcpy(cnv->مقصد_UBytes, cnv->pre_مقصد_U, cnv->pre_مقصد_UFirst_درازی);
        cnv->مقصد_ULength=cnv->pre_مقصد_UFirst_درازی;

        /* move the rest up inside the buffer */
        length=cnv->pre_مقصد_ULength-cnv->pre_مقصد_UFirst_درازی;
        إذا(length>0) {
            uprv_memmove(cnv->pre_مقصد_U, cnv->pre_مقصد_U+cnv->pre_مقصد_UFirst_درازی, length);
        }

        /* mark preToU for replay */
        cnv->pre_مقصد_ULength=(int8_t)-length;

        /* set the error code for unassigned */
        *p_Error_کد=U_INVALID_CHAR_یافت;
    }
}

/* from Unicode ------------------------------------------------------------- */

// Use roundtrips, "good one-way" mappings, and some normal fallbacks.
ساكن مضمن UBool
extFromUUseMapping(UBool useFallback, uint32_t value, UChar32 اولی_CP) {
    أرجع
        ((value&UCNV_EXT_FROM_U_وضعیت_ماسک)!=0 ||
            FROM_U_USE_FALLBACK(useFallback, اولی_CP)) &&
        (value&UCNV_EXT_FROM_U_RESERVED_ماسک)==0;
}

/*
 * @return index of the char16_t, if found; else <0
 */
ساكن مضمن int32_t
ucnv_ext_پیدا_کن_From_U(ثابت char16_t *fromUSection, int32_t length, char16_t u) {
    int32_t i, شروع, حد;

    /* binary search */
    شروع=0;
    حد=length;
    لكل(;;) {
        i=حد-شروع;
        إذا(i<=1) {
            اكسر; /*انجامشده*/
        }
        /* start<limit-1 */

        إذا(i<=4) {
            /* linear search for the last part */
            إذا(u<=fromUSection[شروع]) {
                اكسر;
            }
            إذا(++شروع<حد && u<=fromUSection[شروع]) {
                اكسر;
            }
            إذا(++شروع<حد && u<=fromUSection[شروع]) {
                اكسر;
            }
            /* always break at start==limit-1 */
            ++شروع;
            اكسر;
        }

        i=(شروع+حد)/2;
        إذا(u<fromUSection[i]) {
            حد=i;
        } وإلا {
            شروع=i;
        }
    }

    /* did we really find it? */
    إذا(شروع<حد && u==fromUSection[شروع]) {
        أرجع شروع;
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
ucnv_ext_تطابق_From_U(ثابت int32_t *cx,
                   UChar32 اولی_CP,
                   ثابت char16_t *pre, int32_t pre_درازی,
                   ثابت char16_t *src, int32_t src_درازی,
                   uint32_t *p_تطابق_مقدار,
                   UBool useFallback, UBool flush) {
    ثابت uint16_t *stage12, *stage3;
    ثابت uint32_t *stage3b;

    ثابت char16_t *fromUTableUChars, *fromUSectionUChars;
    ثابت uint32_t *from_UTable_مقادیر, *from_USection_مقادیر;

    uint32_t value, تطابق_مقدار;
    int32_t i, j, idx, length, تطابق_درازی;
    char16_t c;

    إذا(cx==اشاره_تهی) {
        أرجع 0; /* no extension data, no match */
    }

    /* trie lookup of firstCP */
    idx=اولی_CP>>10; /* stage 1 index */
    إذا(idx>=cx[UCNV_EXT_FROM_U_STAGE_1_درازی]) {
        أرجع 0; /* the first code point is outside the trie */
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_شاخص, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_شاخص, uint16_t);
    idx=UCNV_EXT_FROM_U(stage12, stage3, idx, اولی_CP);

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

    إذا(UCNV_EXT_مقصد_U_IS_PARTIAL(value)) {
        /* partial match, enter the loop below */
        idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_شاخص(value));

        /* initialize */
        fromUTableUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_شاخص, char16_t);
        from_UTable_مقادیر=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_مقادیر_شاخص, uint32_t);

        تطابق_مقدار=0;
        i=j=تطابق_درازی=0;

        /* we must not remember fallback matches when not using fallbacks */

        /* match input units until there is a full match or the input is consumed */
        لكل(;;) {
            /* go to the next section */
            fromUSectionUChars=fromUTableUChars+idx;
            from_USection_مقادیر=from_UTable_مقادیر+idx;

            /* read first pair of the section */
            length=*fromUSectionUChars++;
            value=*from_USection_مقادیر++;
            إذا(value!=0 && extFromUUseMapping(useFallback, value, اولی_CP)) {
                /* remember longest match so far */
                تطابق_مقدار=value;
                تطابق_درازی=2+i+j;
            }

            /* match pre[] then src[] */
            إذا(i<pre_درازی) {
                c=pre[i++];
            } وإلا إذا(j<src_درازی) {
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
            idx=ucnv_ext_پیدا_کن_From_U(fromUSectionUChars, length, c);
            إذا(idx<0) {
                /* no match here, stop with the longest match so far */
                اكسر;
            } وإلا {
                value=from_USection_مقادیر[idx];
                إذا(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                    /* partial match, continue */
                    idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_شاخص(value));
                } وإلا {
                    إذا(extFromUUseMapping(useFallback, value, اولی_CP)) {
                        /* full match, stop with result */
                        تطابق_مقدار=value;
                        تطابق_درازی=2+i+j;
                    } وإلا {
                        /* full match on fallback not taken, stop with the longest match so far */
                    }
                    اكسر;
                }
            }
        }

        إذا(تطابق_درازی==0) {
            /* no match at all */
            أرجع 0;
        }
    } وإلا /* result from firstCP trie lookup */ {
        إذا(extFromUUseMapping(useFallback, value, اولی_CP)) {
            /* full match, stop with result */
            تطابق_مقدار=value;
            تطابق_درازی=2;
        } وإلا {
            /* fallback not taken */
            أرجع 0;
        }
    }

    /* return result */
    إذا(تطابق_مقدار==UCNV_EXT_FROM_U_SUBCHAR1) {
        أرجع 1; /* assert matchLength==2 */
    }

    *p_تطابق_مقدار=تطابق_مقدار;
    أرجع تطابق_درازی;
}

/*
 * @param value fromUnicode mapping table value; ignores roundtrip and reserved bits
 */
ساكن مضمن فراغ
ucnv_extWriteFromU(UConverter *cnv, ثابت int32_t *cx,
                   uint32_t value,
                   حرف **هدف, ثابت حرف *هدف_حد,
                   int32_t **offsets, int32_t src_شاخص,
                   UError_کد *p_Error_کد) {
    uint8_t میانگیر[1+UCNV_EXT_MAX_بایتها];
    ثابت uint8_t *نتیجه;
    int32_t length, prev_درازی;

    length=UCNV_EXT_FROM_U_GET_درازی(value);
    value = UCNV_EXT_FROM_U_GET_داده(value);

    /* output the result */
    إذا(length<=UCNV_EXT_FROM_U_MAX_مستقیم_درازی) {
        /*
         * Generate a byte array and then write it below.
         * This is not the fastest possible way, but it should be ok for
         * extension mappings, and it is much simpler.
         * Offset and overflow handling are only done once this way.
         */
        uint8_t *p=میانگیر+1; /* reserve buffer[0] for shiftByte below */
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
        نتیجه=میانگیر+1;
    } وإلا {
        نتیجه=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_بایتها_شاخص, uint8_t)+value;
    }

    /* with correct data we have length>0 */

    إذا((prev_درازی=cnv->from_Unicode_وضعیت)!=0) {
        /* handle SI/SO stateful output */
        uint8_t لغزش_Byte;

        إذا(prev_درازی>1 && length==1) {
            /* change from double-byte mode to single-byte */
            لغزش_Byte = static_cast<uint8_t>(UCNV_SI);
            cnv->from_Unicode_وضعیت=1;
        } وإلا إذا(prev_درازی==1 && length>1) {
            /* change from single-byte mode to double-byte */
            لغزش_Byte = static_cast<uint8_t>(UCNV_SO);
            cnv->from_Unicode_وضعیت=2;
        } وإلا {
            لغزش_Byte=0;
        }

        إذا(لغزش_Byte!=0) {
            /* prepend the shift byte to the result bytes */
            میانگیر[0]=لغزش_Byte;
            إذا(نتیجه!=میانگیر+1) {
                uprv_memcpy(میانگیر+1, نتیجه, length);
            }
            نتیجه=میانگیر;
            ++length;
        }
    }

    ucnv_from_UWrite_بایتها(cnv, reinterpret_cast<ثابت حرف*>(نتیجه), length,
                         هدف, هدف_حد,
                         offsets, src_شاخص,
                         p_Error_کد);
}

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_تطابق_From_U(UConverter *cnv, ثابت int32_t *cx,
                          UChar32 cp,
                          ثابت char16_t **src, ثابت char16_t *src_حد,
                          حرف **هدف, ثابت حرف *هدف_حد,
                          int32_t **offsets, int32_t src_شاخص,
                          UBool flush,
                          UError_کد *p_Error_کد) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t تطابق;

    /* try to match */
    تطابق=ucnv_ext_تطابق_From_U(cx, cp,
                             اشاره_تهی, 0,
                             *src, (int32_t)(src_حد-*src),
                             &value,
                             cnv->useFallback, flush);

    /* reject a match if the result is a single byte for DBCS-only */
    إذا( تطابق>=2 &&
        !(UCNV_EXT_FROM_U_GET_درازی(value)==1 &&
          cnv->shared_داده->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY)
    ) {
        /* advance src pointer for the consumed input */
        *src+=تطابق-2; /* remove 2 for the initial code point */

        /* write result to target */
        ucnv_extWriteFromU(cnv, cx,
                           value,
                           هدف, هدف_حد,
                           offsets, src_شاخص,
                           p_Error_کد);
        أرجع درست;
    } وإلا إذا(تطابق<0) {
        /* save state for partial match */
        ثابت char16_t *s;
        int32_t j;

        /* copy the first code point */
        cnv->preFromUFirstCP=cp;

        /* now copy the newly consumed input */
        s=*src;
        تطابق=-تطابق-2; /* remove 2 for the initial code point */
        لكل(j=0; j<تطابق; ++j) {
            cnv->preFromU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)تطابق;
        أرجع درست;
    } وإلا إذا(تطابق==1) {
        /* matched, no mapping but request for <subchar1> */
        cnv->useSubChar1=درست;
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
ucnv_ext_Simple_تطابق_From_U(ثابت int32_t *cx,
                         UChar32 cp, uint32_t *p_مقدار,
                         UBool useFallback) {
    uint32_t value;
    int32_t تطابق;

    /* try to match */
    تطابق=ucnv_ext_تطابق_From_U(cx,
                             cp,
                             اشاره_تهی, 0,
                             اشاره_تهی, 0,
                             &value,
                             useFallback, درست);
    إذا(تطابق>=2) {
        /* write result for simple, single-character conversion */
        int32_t length;
        صحيح isRoundtrip;

        isRoundtrip=UCNV_EXT_FROM_U_IS_ROUNDTRIP(value);
        length=UCNV_EXT_FROM_U_GET_درازی(value);
        value = UCNV_EXT_FROM_U_GET_داده(value);

        إذا(length<=UCNV_EXT_FROM_U_MAX_مستقیم_درازی) {
            *p_مقدار=value;
            أرجع isRoundtrip ? length : -length;
#إذا 0 /* not currently used */
        } وإلا إذا(length==4) {
            /* de-serialize a 4-byte result */
            ثابت uint8_t *نتیجه=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_بایتها_شاخص, uint8_t)+value;
            *p_مقدار=
                ((uint32_t)نتیجه[0]<<24)|
                ((uint32_t)نتیجه[1]<<16)|
                ((uint32_t)نتیجه[2]<<8)|
                نتیجه[3];
            أرجع isRoundtrip ? 4 : -4;
#پایان_اگر
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
ucnv_ext_Continue_تطابق_From_U(UConverter *cnv,
                           UConverter_From_Unicode_آرگومانها *p_آرگومانها, int32_t src_شاخص,
                           UError_کد *p_Error_کد) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t تطابق;

    تطابق=ucnv_ext_تطابق_From_U(cnv->shared_داده->mbcs.extIndexes,
                             cnv->preFromUFirstCP,
                             cnv->preFromU, cnv->preFromULength,
                             p_آرگومانها->منبع, (int32_t)(p_آرگومانها->منبع_حد-p_آرگومانها->منبع),
                             &value,
                             cnv->useFallback, p_آرگومانها->flush);
    إذا(تطابق>=2) {
        تطابق-=2; /* remove 2 for the initial code point */

        إذا(تطابق>=cnv->preFromULength) {
            /* advance src pointer for the consumed input */
            p_آرگومانها->منبع+=تطابق-cnv->preFromULength;
            cnv->preFromULength=0;
        } وإلا {
            /* the match did not use all of preFromU[] - keep the rest for replay */
            int32_t length=cnv->preFromULength-تطابق;
            u_memmove(cnv->preFromU, cnv->preFromU+تطابق, length);
            cnv->preFromULength=(int8_t)-length;
        }

        /* finish the partial match */
        cnv->preFromUFirstCP=U_SENTINEL;

        /* write result */
        ucnv_extWriteFromU(cnv, cnv->shared_داده->mbcs.extIndexes,
                           value,
                           &p_آرگومانها->هدف, p_آرگومانها->هدف_حد,
                           &p_آرگومانها->offsets, src_شاخص,
                           p_Error_کد);
    } وإلا إذا(تطابق<0) {
        /* save state for partial match */
        ثابت char16_t *s;
        int32_t j;

        /* just _append_ the newly consumed input to preFromU[] */
        s=p_آرگومانها->منبع;
        تطابق=-تطابق-2; /* remove 2 for the initial code point */
        لكل(j=cnv->preFromULength; j<تطابق; ++j) {
            U_ASSERT(j>=0);
            cnv->preFromU[j]=*s++;
        }
        p_آرگومانها->منبع=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)تطابق;
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

        إذا(تطابق==1) {
            /* matched, no mapping but request for <subchar1> */
            cnv->useSubChar1=درست;
        }

        /* move the first code point to the error field */
        cnv->fromUChar32=cnv->preFromUFirstCP;
        cnv->preFromUFirstCP=U_SENTINEL;

        /* mark preFromU for replay */
        cnv->preFromULength=-cnv->preFromULength;

        /* set the error code for unassigned */
        *p_Error_کد=U_INVALID_CHAR_یافت;
    }
}

ساكن UBool
ext_مجموعه_Use_Mapping(UConverter_Unicode_مجموعه which, int32_t min_درازی, uint32_t value) {
    إذا(which==UCNV_ROUNDTRIP_مجموعه) {
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
    أرجع UCNV_EXT_FROM_U_GET_درازی(value)>=min_درازی;
}

ساكن فراغ
ucnv_ext_Get_Unicode_مجموعه_String(ثابت UConverter_Shared_داده *shared_داده,
                            ثابت int32_t *cx,
                            ثابت USetAdder *sa,
                            UConverter_Unicode_مجموعه which,
                            int32_t min_درازی,
                            UChar32 اولی_CP,
                            char16_t s[UCNV_EXT_MAX_UCHARS], int32_t length,
                            int32_t section_شاخص,
                            UError_کد *p_Error_کد) {
    ثابت char16_t *fromUSectionUChars;
    ثابت uint32_t *from_USection_مقادیر;

    uint32_t value;
    int32_t i, count;

    fromUSectionUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_شاخص, char16_t)+section_شاخص;
    from_USection_مقادیر=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_مقادیر_شاخص, uint32_t)+section_شاخص;

    /* read first pair of the section */
    count=*fromUSectionUChars++;
    value=*from_USection_مقادیر++;

    إذا(ext_مجموعه_Use_Mapping(which, min_درازی, value)) {
        إذا(length==U16_درازی(اولی_CP)) {
            /* add the initial code point */
            sa->جمع_کن(sa->مجموعه, اولی_CP);
        } وإلا {
            /* add the string so far */
            sa->جمع_کن_String(sa->مجموعه, s, length);
        }
    }

    لكل(i=0; i<count; ++i) {
        /* append this code unit and recurse or add the string */
        s[length]=fromUSectionUChars[i];
        value=from_USection_مقادیر[i];

        إذا(value==0) {
            /* no mapping, do nothing */
        } وإلا إذا(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
            ucnv_ext_Get_Unicode_مجموعه_String(
                shared_داده, cx, sa, which, min_درازی,
                اولی_CP, s, length+1,
                static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_شاخص(value)),
                p_Error_کد);
        } وإلا إذا(ext_مجموعه_Use_Mapping(which, min_درازی, value)) {
            sa->جمع_کن_String(sa->مجموعه, s, length+1);
        }
    }
}

U_CFUNC فراغ
ucnv_ext_Get_Unicode_مجموعه(ثابت UConverter_Shared_داده *shared_داده,
                      ثابت USetAdder *sa,
                      UConverter_Unicode_مجموعه which,
                      UConverter_مجموعه_Filter filter,
                      UError_کد *p_Error_کد) {
    ثابت int32_t *cx;
    ثابت uint16_t *stage12, *stage3, *ps2, *ps3;
    ثابت uint32_t *stage3b;

    uint32_t value;
    int32_t st1, stage1_درازی, st2, st3, min_درازی;

    char16_t s[UCNV_EXT_MAX_UCHARS];
    UChar32 c;
    int32_t length;

    cx=shared_داده->mbcs.extIndexes;
    إذا(cx==اشاره_تهی) {
        أرجع;
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_شاخص, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_شاخص, uint16_t);
    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_شاخص, uint32_t);

    stage1_درازی=cx[UCNV_EXT_FROM_U_STAGE_1_درازی];

    /* enumerate the from-Unicode trie table */
    c=0; /* keep track of the current code point while enumerating */

    إذا(filter==UCNV_مجموعه_FILTER_2022_CN) {
        min_درازی=3;
    } وإلا إذا( shared_داده->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ||
               filter!=UCNV_مجموعه_FILTER_NONE
    ) {
        /* DBCS-only, ignore single-byte results */
        min_درازی=2;
    } وإلا {
        min_درازی=1;
    }

    /*
     * the trie enumeration is almost the same as
     * in MBCSGetUnicodeSet() for MBCS_OUTPUT_1
     */
    لكل(st1=0; st1<stage1_درازی; ++st1) {
        st2=stage12[st1];
        إذا(st2>stage1_درازی) {
            ps2=stage12+st2;
            لكل(st2=0; st2<64; ++st2) {
                إذا((st3=(int32_t)ps2[st2]<<UCNV_EXT_STAGE_2_چپ_لغزش)!=0) {
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
                            ucnv_ext_Get_Unicode_مجموعه_String(
                                shared_داده, cx, sa, which, min_درازی,
                                c, s, length,
                                (int32_t)UCNV_EXT_FROM_U_GET_PARTIAL_شاخص(value),
                                p_Error_کد);
                        } وإلا إذا(ext_مجموعه_Use_Mapping(which, min_درازی, value)) {
                            بدل(filter) {
                            حالة UCNV_مجموعه_FILTER_2022_CN:
                                إذا(!(UCNV_EXT_FROM_U_GET_درازی(value)==3 && UCNV_EXT_FROM_U_GET_داده(value)<=0x82ffff)) {
                                    استمر;
                                }
                                اكسر;
                            حالة UCNV_مجموعه_FILTER_SJIS:
                                إذا(!(UCNV_EXT_FROM_U_GET_درازی(value)==2 && (value=UCNV_EXT_FROM_U_GET_داده(value))>=0x8140 && value<=0xeffc)) {
                                    استمر;
                                }
                                اكسر;
                            حالة UCNV_مجموعه_FILTER_GR94_DBCS:
                                إذا(!(UCNV_EXT_FROM_U_GET_درازی(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_داده(value))-0xa1a1)<=(0xfefe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    استمر;
                                }
                                اكسر;
                            حالة UCNV_مجموعه_FILTER_HZ:
                                إذا(!(UCNV_EXT_FROM_U_GET_درازی(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_داده(value))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
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
                            sa->جمع_کن(sa->مجموعه, c);
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

#پایان_اگر /* #if !UCONFIG_NO_LEGACY_CONVERSION */
