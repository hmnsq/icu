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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#شامل "unicode/uset.h"
#شامل "unicode/ustring.h"
#شامل "ucnv_bld.h"
#شامل "ucnv_cnv.h"
#شامل "ucnv_ext.h"
#شامل "cmemory.h"
#شامل "uassert.h"

/* to Unicode --------------------------------------------------------------- */

/*
 * @return lookup value for the byte, if found; else 0
 */
ساکن اندرونی uint32_t
ucnv_ext_ڈھونڈو_مقصد_U(ثابت uint32_t *مقصد_USection, int32_t length, uint8_t byte) {
    uint32_t word0, لفظ;
    int32_t i, آغاز, حد_بندی;

    /* check the input byte against the lowest and highest section bytes */
    آغاز = static_cast<int32_t>(UCNV_EXT_مقصد_U_GET_BYTE(مقصد_USection[0]));
    حد_بندی = static_cast<int32_t>(UCNV_EXT_مقصد_U_GET_BYTE(مقصد_USection[length - 1]));
    اگر(byte<آغاز || حد_بندی<byte) {
        واپس 0; /* the byte is out of range */
    }

    اگر(length==((حد_بندی-آغاز)+1)) {
        /* direct access on a linear array */
        واپس UCNV_EXT_مقصد_U_GET_قدر(مقصد_USection[byte-آغاز]); /* could be 0 */
    }

    /* word0 is suitable for <=toUSection[] comparison, word for <toUSection[] */
    word0=UCNV_EXT_مقصد_U_MAKE_لفظ(byte, 0);

    /*
     * Shift byte once instead of each section word and add 0xffffff.
     * We will compare the shifted/added byte (bbffffff) against
     * section words which have byte values in the same bit position.
     * If and only if byte bb < section byte ss then bbffffff<ssvvvvvv
     * for all v=0..f
     * so we need not mask off the lower 24 bits of each section word.
     */
    لفظ=word0|UCNV_EXT_مقصد_U_قدر_نقاب;

    /* binary search */
    آغاز=0;
    حد_بندی=length;
    برائے(;;) {
        i=حد_بندی-آغاز;
        اگر(i<=1) {
            توڑو; /*مکمل*/
        }
        /* start<limit-1 */

        اگر(i<=4) {
            /* linear search for the last part */
            اگر(word0<=مقصد_USection[آغاز]) {
                توڑو;
            }
            اگر(++آغاز<حد_بندی && word0<=مقصد_USection[آغاز]) {
                توڑو;
            }
            اگر(++آغاز<حد_بندی && word0<=مقصد_USection[آغاز]) {
                توڑو;
            }
            /* always break at start==limit-1 */
            ++آغاز;
            توڑو;
        }

        i=(آغاز+حد_بندی)/2;
        اگر(لفظ<مقصد_USection[i]) {
            حد_بندی=i;
        } ورنہ {
            آغاز=i;
        }
    }

    /* did we really find it? */
    اگر(آغاز<حد_بندی && byte==UCNV_EXT_مقصد_U_GET_BYTE(لفظ=مقصد_USection[آغاز])) {
        واپس UCNV_EXT_مقصد_U_GET_قدر(لفظ); /* never 0 */
    } ورنہ {
        واپس 0; /* not found */
    }
}

/*
 * true if not an SI/SO stateful converter,
 * or if the match length fits with the current converter state
 */
#تعریف_کرو UCNV_EXT_مقصد_U_VERIFY_SISO_مطابقت(sisoState, مطابقت) \
    ((sisoState)<0 || ((sisoState)==0) == (مطابقت==1))

/*
 * this works like ucnv_extMatchFromU() except
 * - the first character is in pre
 * - no trie is used
 * - the returned matchLength is not offset by 2
 */
ساکن int32_t
ucnv_ext_مطابقت_مقصد_U(ثابت int32_t *cx, int8_t sisoState,
                 ثابت حرف *pre, int32_t pre_لمبائی,
                 ثابت حرف *src, int32_t src_لمبائی,
                 uint32_t *p_مطابقت_قدر,
                 UBool /*useFallback*/, UBool flush) {
    ثابت uint32_t *مقصد_UTable, *مقصد_USection;

    uint32_t value, مطابقت_قدر;
    int32_t i, j, idx, length, مطابقت_لمبائی;
    uint8_t b;

    اگر(cx==خالی_اشاریہ || cx[UCNV_EXT_مقصد_U_لمبائی]<=0) {
        واپس 0; /* no extension data, no match */
    }

    /* initialize */
    مقصد_UTable=UCNV_EXT_ARRAY(cx, UCNV_EXT_مقصد_U_اشاریہ, uint32_t);
    idx=0;

    مطابقت_قدر=0;
    i=j=مطابقت_لمبائی=0;

    اگر(sisoState==0) {
        /* SBCS state of an SI/SO stateful converter, look at only exactly 1 byte */
        اگر(pre_لمبائی>1) {
            واپس 0; /* no match of a DBCS sequence in SBCS mode */
        } ورنہ اگر(pre_لمبائی==1) {
            src_لمبائی=0;
        } ورنہ /* preLength==0 */ {
            اگر(src_لمبائی>1) {
                src_لمبائی=1;
            }
        }
        flush=سچ;
    }

    /* we must not remember fallback matches when not using fallbacks */

    /* match input units until there is a full match or the input is consumed */
    برائے(;;) {
        /* go to the next section */
        مقصد_USection=مقصد_UTable+idx;

        /* read first pair of the section */
        value=*مقصد_USection++;
        length=UCNV_EXT_مقصد_U_GET_BYTE(value);
        value=UCNV_EXT_مقصد_U_GET_قدر(value);
        اگر( value!=0 &&
            (UCNV_EXT_مقصد_U_IS_ROUNDTRIP(value) ||
             مقصد_U_USE_FALLBACK(useFallback)) &&
            UCNV_EXT_مقصد_U_VERIFY_SISO_مطابقت(sisoState, i+j)
        ) {
            /* remember longest match so far */
            مطابقت_قدر=value;
            مطابقت_لمبائی=i+j;
        }

        /* match pre[] then src[] */
        اگر(i<pre_لمبائی) {
            b = static_cast<uint8_t>(pre[i++]);
        } ورنہ اگر(j<src_لمبائی) {
            b = static_cast<uint8_t>(src[j++]);
        } ورنہ {
            /* all input consumed, partial match */
            اگر(flush || (length=(i+j))>UCNV_EXT_MAX_بائٹیں) {
                /*
                 * end of the entire input stream, stop with the longest match so far
                 * or: partial match must not be longer than UCNV_EXT_MAX_BYTES
                 * because it must fit into state buffers
                 */
                توڑو;
            } ورنہ {
                /* continue with more input next time */
                واپس -length;
            }
        }

        /* search for the current char16_t */
        value=ucnv_ext_ڈھونڈو_مقصد_U(مقصد_USection, length, b);
        اگر(value==0) {
            /* no match here, stop with the longest match so far */
            توڑو;
        } ورنہ {
            اگر(UCNV_EXT_مقصد_U_IS_PARTIAL(value)) {
                /* partial match, continue */
                idx = static_cast<int32_t>(UCNV_EXT_مقصد_U_GET_PARTIAL_اشاریہ(value));
            } ورنہ {
                اگر( (UCNV_EXT_مقصد_U_IS_ROUNDTRIP(value) ||
                     مقصد_U_USE_FALLBACK(useFallback)) &&
                    UCNV_EXT_مقصد_U_VERIFY_SISO_مطابقت(sisoState, i+j)
                ) {
                    /* full match, stop with result */
                    مطابقت_قدر=value;
                    مطابقت_لمبائی=i+j;
                } ورنہ {
                    /* full match on fallback not taken, stop with the longest match so far */
                }
                توڑو;
            }
        }
    }

    اگر(مطابقت_لمبائی==0) {
        /* no match at all */
        واپس 0;
    }

    /* return result */
    *p_مطابقت_قدر=UCNV_EXT_مقصد_U_نقاب_ROUNDTRIP(مطابقت_قدر);
    واپس مطابقت_لمبائی;
}

ساکن اندرونی خالی
ucnv_ext_Write_مقصد_U(UConverter *cnv, ثابت int32_t *cx,
                 uint32_t value,
                 char16_t **ہدف, ثابت char16_t *ہدف_حد_بندی,
                 int32_t **offsets, int32_t src_اشاریہ,
                 UError_رمز *p_Error_رمز) {
    /* output the result */
    اگر(UCNV_EXT_مقصد_U_IS_رمز_نقطہ(value)) {
        /* output a single code point */
        ucnv_مقصد_UWrite_رمز_نقطہ(
            cnv, UCNV_EXT_مقصد_U_GET_رمز_نقطہ(value),
            ہدف, ہدف_حد_بندی,
            offsets, src_اشاریہ,
            p_Error_رمز);
    } ورنہ {
        /* output a string - with correct data we have resultLength>0 */
        ucnv_مقصد_UWrite_UChars(
            cnv,
            UCNV_EXT_ARRAY(cx, UCNV_EXT_مقصد_U_UCHARS_اشاریہ, char16_t)+
                UCNV_EXT_مقصد_U_GET_اشاریہ(value),
            UCNV_EXT_مقصد_U_GET_لمبائی(value),
            ہدف, ہدف_حد_بندی,
            offsets, src_اشاریہ,
            p_Error_رمز);
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
#تعریف_کرو UCNV_SISO_STATE(cnv) \
    ((cnv)->shared_ڈیٹا->mbcs.outputType==MBCS_OUTPUT_2_SISO ? (int8_t)(cnv)->منوال : \
     (cnv)->shared_ڈیٹا->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ? 1 : -1)

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_مطابقت_مقصد_U(UConverter *cnv, ثابت int32_t *cx,
                        int32_t پہلا_لمبائی,
                        ثابت حرف **src, ثابت حرف *src_حد_بندی,
                        char16_t **ہدف, ثابت char16_t *ہدف_حد_بندی,
                        int32_t **offsets, int32_t src_اشاریہ,
                        UBool flush,
                        UError_رمز *p_Error_رمز) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t مطابقت;

    /* try to match */
    مطابقت=ucnv_ext_مطابقت_مقصد_U(cx, (int8_t)UCNV_SISO_STATE(cnv),
                           (ثابت حرف *)cnv->مقصد_UBytes, پہلا_لمبائی,
                           *src, (int32_t)(src_حد_بندی-*src),
                           &value,
                           cnv->useFallback, flush);
    اگر(مطابقت>0) {
        /* advance src pointer for the consumed input */
        *src+=مطابقت-پہلا_لمبائی;

        /* write result to target */
        ucnv_ext_Write_مقصد_U(cnv, cx,
                         value,
                         ہدف, ہدف_حد_بندی,
                         offsets, src_اشاریہ,
                         p_Error_رمز);
        واپس سچ;
    } ورنہ اگر(مطابقت<0) {
        /* save state for partial match */
        ثابت حرف *s;
        int32_t j;

        /* copy the first code point */
        s=(ثابت حرف *)cnv->مقصد_UBytes;
        cnv->pre_مقصد_UFirst_لمبائی=(int8_t)پہلا_لمبائی;
        برائے(j=0; j<پہلا_لمبائی; ++j) {
            cnv->pre_مقصد_U[j]=*s++;
        }

        /* now copy the newly consumed input */
        s=*src;
        مطابقت=-مطابقت;
        برائے(; j<مطابقت; ++j) {
            cnv->pre_مقصد_U[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_مقصد_ULength=(int8_t)مطابقت;
        واپس سچ;
    } ورنہ /* match==0 no match */ {
        واپس جھوٹ;
    }
}

U_CFUNC UChar32
ucnv_ext_Simple_مطابقت_مقصد_U(ثابت int32_t *cx,
                       ثابت حرف *ماخذ, int32_t length,
                       UBool useFallback) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t مطابقت;

    اگر(length<=0) {
        واپس 0xffff;
    }

    /* try to match */
    مطابقت=ucnv_ext_مطابقت_مقصد_U(cx, -1,
                           ماخذ, length,
                           خالی_اشاریہ, 0,
                           &value,
                           useFallback, سچ);
    اگر(مطابقت==length) {
        /* write result for simple, single-character conversion */
        اگر(UCNV_EXT_مقصد_U_IS_رمز_نقطہ(value)) {
            واپس UCNV_EXT_مقصد_U_GET_رمز_نقطہ(value);
        }
    }

    /*
     * return no match because
     * - match>0 && value points to string: simple conversion cannot handle multiple code points
     * - match>0 && match!=length: not all input consumed, forbidden for this function
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    واپس 0xfffe;
}

/*
 * continue partial match with new input
 * never called for simple, single-character conversion
 */
U_CFUNC خالی
ucnv_ext_Continue_مطابقت_مقصد_U(UConverter *cnv,
                         UConverter_مقصد_Unicode_دلائل *p_دلائل, int32_t src_اشاریہ,
                         UError_رمز *p_Error_رمز) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t مطابقت, length;

    مطابقت=ucnv_ext_مطابقت_مقصد_U(cnv->shared_ڈیٹا->mbcs.extIndexes, (int8_t)UCNV_SISO_STATE(cnv),
                           cnv->pre_مقصد_U, cnv->pre_مقصد_ULength,
                           p_دلائل->ماخذ, (int32_t)(p_دلائل->ماخذ_حد_بندی-p_دلائل->ماخذ),
                           &value,
                           cnv->useFallback, p_دلائل->flush);
    اگر(مطابقت>0) {
        اگر(مطابقت>=cnv->pre_مقصد_ULength) {
            /* advance src pointer for the consumed input */
            p_دلائل->ماخذ+=مطابقت-cnv->pre_مقصد_ULength;
            cnv->pre_مقصد_ULength=0;
        } ورنہ {
            /* the match did not use all of preToU[] - keep the rest for replay */
            length=cnv->pre_مقصد_ULength-مطابقت;
            uprv_memmove(cnv->pre_مقصد_U, cnv->pre_مقصد_U+مطابقت, length);
            cnv->pre_مقصد_ULength=(int8_t)-length;
        }

        /* write result */
        ucnv_ext_Write_مقصد_U(cnv, cnv->shared_ڈیٹا->mbcs.extIndexes,
                         value,
                         &p_دلائل->ہدف, p_دلائل->ہدف_حد_بندی,
                         &p_دلائل->offsets, src_اشاریہ,
                         p_Error_رمز);
    } ورنہ اگر(مطابقت<0) {
        /* save state for partial match */
        ثابت حرف *s;
        int32_t j;

        /* just _append_ the newly consumed input to preToU[] */
        s=p_دلائل->ماخذ;
        مطابقت=-مطابقت;
        برائے(j=cnv->pre_مقصد_ULength; j<مطابقت; ++j) {
            cnv->pre_مقصد_U[j]=*s++;
        }
        p_دلائل->ماخذ=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_مقصد_ULength=(int8_t)مطابقت;
    } ورنہ /* match==0 */ {
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
        uprv_memcpy(cnv->مقصد_UBytes, cnv->pre_مقصد_U, cnv->pre_مقصد_UFirst_لمبائی);
        cnv->مقصد_ULength=cnv->pre_مقصد_UFirst_لمبائی;

        /* move the rest up inside the buffer */
        length=cnv->pre_مقصد_ULength-cnv->pre_مقصد_UFirst_لمبائی;
        اگر(length>0) {
            uprv_memmove(cnv->pre_مقصد_U, cnv->pre_مقصد_U+cnv->pre_مقصد_UFirst_لمبائی, length);
        }

        /* mark preToU for replay */
        cnv->pre_مقصد_ULength=(int8_t)-length;

        /* set the error code for unassigned */
        *p_Error_رمز=U_INVALID_CHAR_ملا;
    }
}

/* from Unicode ------------------------------------------------------------- */

// Use roundtrips, "good one-way" mappings, and some normal fallbacks.
ساکن اندرونی UBool
extFromUUseMapping(UBool useFallback, uint32_t value, UChar32 پہلا_CP) {
    واپس
        ((value&UCNV_EXT_FROM_U_حالت_نقاب)!=0 ||
            FROM_U_USE_FALLBACK(useFallback, پہلا_CP)) &&
        (value&UCNV_EXT_FROM_U_RESERVED_نقاب)==0;
}

/*
 * @return index of the char16_t, if found; else <0
 */
ساکن اندرونی int32_t
ucnv_ext_ڈھونڈو_From_U(ثابت char16_t *fromUSection, int32_t length, char16_t u) {
    int32_t i, آغاز, حد_بندی;

    /* binary search */
    آغاز=0;
    حد_بندی=length;
    برائے(;;) {
        i=حد_بندی-آغاز;
        اگر(i<=1) {
            توڑو; /*مکمل*/
        }
        /* start<limit-1 */

        اگر(i<=4) {
            /* linear search for the last part */
            اگر(u<=fromUSection[آغاز]) {
                توڑو;
            }
            اگر(++آغاز<حد_بندی && u<=fromUSection[آغاز]) {
                توڑو;
            }
            اگر(++آغاز<حد_بندی && u<=fromUSection[آغاز]) {
                توڑو;
            }
            /* always break at start==limit-1 */
            ++آغاز;
            توڑو;
        }

        i=(آغاز+حد_بندی)/2;
        اگر(u<fromUSection[i]) {
            حد_بندی=i;
        } ورنہ {
            آغاز=i;
        }
    }

    /* did we really find it? */
    اگر(آغاز<حد_بندی && u==fromUSection[آغاز]) {
        واپس آغاز;
    } ورنہ {
        واپس -1; /* not found */
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
ساکن int32_t
ucnv_ext_مطابقت_From_U(ثابت int32_t *cx,
                   UChar32 پہلا_CP,
                   ثابت char16_t *pre, int32_t pre_لمبائی,
                   ثابت char16_t *src, int32_t src_لمبائی,
                   uint32_t *p_مطابقت_قدر,
                   UBool useFallback, UBool flush) {
    ثابت uint16_t *stage12, *stage3;
    ثابت uint32_t *stage3b;

    ثابت char16_t *fromUTableUChars, *fromUSectionUChars;
    ثابت uint32_t *from_UTable_اقدار, *from_USection_اقدار;

    uint32_t value, مطابقت_قدر;
    int32_t i, j, idx, length, مطابقت_لمبائی;
    char16_t c;

    اگر(cx==خالی_اشاریہ) {
        واپس 0; /* no extension data, no match */
    }

    /* trie lookup of firstCP */
    idx=پہلا_CP>>10; /* stage 1 index */
    اگر(idx>=cx[UCNV_EXT_FROM_U_STAGE_1_لمبائی]) {
        واپس 0; /* the first code point is outside the trie */
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_اشاریہ, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_اشاریہ, uint16_t);
    idx=UCNV_EXT_FROM_U(stage12, stage3, idx, پہلا_CP);

    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_اشاریہ, uint32_t);
    value=stage3b[idx];
    اگر(value==0) {
        واپس 0;
    }

    /*
     * Tests for (value&UCNV_EXT_FROM_U_RESERVED_MASK)==0:
     * Do not interpret values with reserved bits used, for forward compatibility,
     * and do not even remember intermediate results with reserved bits used.
     */

    اگر(UCNV_EXT_مقصد_U_IS_PARTIAL(value)) {
        /* partial match, enter the loop below */
        idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_اشاریہ(value));

        /* initialize */
        fromUTableUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_اشاریہ, char16_t);
        from_UTable_اقدار=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_اقدار_اشاریہ, uint32_t);

        مطابقت_قدر=0;
        i=j=مطابقت_لمبائی=0;

        /* we must not remember fallback matches when not using fallbacks */

        /* match input units until there is a full match or the input is consumed */
        برائے(;;) {
            /* go to the next section */
            fromUSectionUChars=fromUTableUChars+idx;
            from_USection_اقدار=from_UTable_اقدار+idx;

            /* read first pair of the section */
            length=*fromUSectionUChars++;
            value=*from_USection_اقدار++;
            اگر(value!=0 && extFromUUseMapping(useFallback, value, پہلا_CP)) {
                /* remember longest match so far */
                مطابقت_قدر=value;
                مطابقت_لمبائی=2+i+j;
            }

            /* match pre[] then src[] */
            اگر(i<pre_لمبائی) {
                c=pre[i++];
            } ورنہ اگر(j<src_لمبائی) {
                c=src[j++];
            } ورنہ {
                /* all input consumed, partial match */
                اگر(flush || (length=(i+j))>UCNV_EXT_MAX_UCHARS) {
                    /*
                     * end of the entire input stream, stop with the longest match so far
                     * or: partial match must not be longer than UCNV_EXT_MAX_UCHARS
                     * because it must fit into state buffers
                     */
                    توڑو;
                } ورنہ {
                    /* continue with more input next time */
                    واپس -(2+length);
                }
            }

            /* search for the current char16_t */
            idx=ucnv_ext_ڈھونڈو_From_U(fromUSectionUChars, length, c);
            اگر(idx<0) {
                /* no match here, stop with the longest match so far */
                توڑو;
            } ورنہ {
                value=from_USection_اقدار[idx];
                اگر(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                    /* partial match, continue */
                    idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_اشاریہ(value));
                } ورنہ {
                    اگر(extFromUUseMapping(useFallback, value, پہلا_CP)) {
                        /* full match, stop with result */
                        مطابقت_قدر=value;
                        مطابقت_لمبائی=2+i+j;
                    } ورنہ {
                        /* full match on fallback not taken, stop with the longest match so far */
                    }
                    توڑو;
                }
            }
        }

        اگر(مطابقت_لمبائی==0) {
            /* no match at all */
            واپس 0;
        }
    } ورنہ /* result from firstCP trie lookup */ {
        اگر(extFromUUseMapping(useFallback, value, پہلا_CP)) {
            /* full match, stop with result */
            مطابقت_قدر=value;
            مطابقت_لمبائی=2;
        } ورنہ {
            /* fallback not taken */
            واپس 0;
        }
    }

    /* return result */
    اگر(مطابقت_قدر==UCNV_EXT_FROM_U_SUBCHAR1) {
        واپس 1; /* assert matchLength==2 */
    }

    *p_مطابقت_قدر=مطابقت_قدر;
    واپس مطابقت_لمبائی;
}

/*
 * @param value fromUnicode mapping table value; ignores roundtrip and reserved bits
 */
ساکن اندرونی خالی
ucnv_extWriteFromU(UConverter *cnv, ثابت int32_t *cx,
                   uint32_t value,
                   حرف **ہدف, ثابت حرف *ہدف_حد_بندی,
                   int32_t **offsets, int32_t src_اشاریہ,
                   UError_رمز *p_Error_رمز) {
    uint8_t بفر[1+UCNV_EXT_MAX_بائٹیں];
    ثابت uint8_t *نتیجہ;
    int32_t length, پچھلا_لمبائی;

    length=UCNV_EXT_FROM_U_GET_لمبائی(value);
    value = UCNV_EXT_FROM_U_GET_ڈیٹا(value);

    /* output the result */
    اگر(length<=UCNV_EXT_FROM_U_MAX_براہ_راست_لمبائی) {
        /*
         * Generate a byte array and then write it below.
         * This is not the fastest possible way, but it should be ok for
         * extension mappings, and it is much simpler.
         * Offset and overflow handling are only done once this way.
         */
        uint8_t *p=بفر+1; /* reserve buffer[0] for shiftByte below */
        انتخاب(length) {
        حالت 3:
            *p++ = static_cast<uint8_t>(value >> 16);
            U_FALLTHROUGH;
        حالت 2:
            *p++ = static_cast<uint8_t>(value >> 8);
            U_FALLTHROUGH;
        حالت 1:
            *p++ = static_cast<uint8_t>(value);
            U_FALLTHROUGH;
        طےشدہ:
            توڑو; /* will never occur */
        }
        نتیجہ=بفر+1;
    } ورنہ {
        نتیجہ=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_بائٹیں_اشاریہ, uint8_t)+value;
    }

    /* with correct data we have length>0 */

    اگر((پچھلا_لمبائی=cnv->from_Unicode_حالت)!=0) {
        /* handle SI/SO stateful output */
        uint8_t سرکاؤ_Byte;

        اگر(پچھلا_لمبائی>1 && length==1) {
            /* change from double-byte mode to single-byte */
            سرکاؤ_Byte = static_cast<uint8_t>(UCNV_SI);
            cnv->from_Unicode_حالت=1;
        } ورنہ اگر(پچھلا_لمبائی==1 && length>1) {
            /* change from single-byte mode to double-byte */
            سرکاؤ_Byte = static_cast<uint8_t>(UCNV_SO);
            cnv->from_Unicode_حالت=2;
        } ورنہ {
            سرکاؤ_Byte=0;
        }

        اگر(سرکاؤ_Byte!=0) {
            /* prepend the shift byte to the result bytes */
            بفر[0]=سرکاؤ_Byte;
            اگر(نتیجہ!=بفر+1) {
                uprv_memcpy(بفر+1, نتیجہ, length);
            }
            نتیجہ=بفر;
            ++length;
        }
    }

    ucnv_from_UWrite_بائٹیں(cnv, reinterpret_cast<ثابت حرف*>(نتیجہ), length,
                         ہدف, ہدف_حد_بندی,
                         offsets, src_اشاریہ,
                         p_Error_رمز);
}

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_مطابقت_From_U(UConverter *cnv, ثابت int32_t *cx,
                          UChar32 cp,
                          ثابت char16_t **src, ثابت char16_t *src_حد_بندی,
                          حرف **ہدف, ثابت حرف *ہدف_حد_بندی,
                          int32_t **offsets, int32_t src_اشاریہ,
                          UBool flush,
                          UError_رمز *p_Error_رمز) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t مطابقت;

    /* try to match */
    مطابقت=ucnv_ext_مطابقت_From_U(cx, cp,
                             خالی_اشاریہ, 0,
                             *src, (int32_t)(src_حد_بندی-*src),
                             &value,
                             cnv->useFallback, flush);

    /* reject a match if the result is a single byte for DBCS-only */
    اگر( مطابقت>=2 &&
        !(UCNV_EXT_FROM_U_GET_لمبائی(value)==1 &&
          cnv->shared_ڈیٹا->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY)
    ) {
        /* advance src pointer for the consumed input */
        *src+=مطابقت-2; /* remove 2 for the initial code point */

        /* write result to target */
        ucnv_extWriteFromU(cnv, cx,
                           value,
                           ہدف, ہدف_حد_بندی,
                           offsets, src_اشاریہ,
                           p_Error_رمز);
        واپس سچ;
    } ورنہ اگر(مطابقت<0) {
        /* save state for partial match */
        ثابت char16_t *s;
        int32_t j;

        /* copy the first code point */
        cnv->preFromUFirstCP=cp;

        /* now copy the newly consumed input */
        s=*src;
        مطابقت=-مطابقت-2; /* remove 2 for the initial code point */
        برائے(j=0; j<مطابقت; ++j) {
            cnv->preFromU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)مطابقت;
        واپس سچ;
    } ورنہ اگر(مطابقت==1) {
        /* matched, no mapping but request for <subchar1> */
        cnv->useSubChar1=سچ;
        واپس جھوٹ;
    } ورنہ /* match==0 no match */ {
        واپس جھوٹ;
    }
}

/*
 * Used by ISO 2022 implementation.
 * @return number of bytes in *pValue; negative number if fallback; 0 for no mapping
 */
U_CFUNC int32_t
ucnv_ext_Simple_مطابقت_From_U(ثابت int32_t *cx,
                         UChar32 cp, uint32_t *p_قدر,
                         UBool useFallback) {
    uint32_t value;
    int32_t مطابقت;

    /* try to match */
    مطابقت=ucnv_ext_مطابقت_From_U(cx,
                             cp,
                             خالی_اشاریہ, 0,
                             خالی_اشاریہ, 0,
                             &value,
                             useFallback, سچ);
    اگر(مطابقت>=2) {
        /* write result for simple, single-character conversion */
        int32_t length;
        صحیح isRoundtrip;

        isRoundtrip=UCNV_EXT_FROM_U_IS_ROUNDTRIP(value);
        length=UCNV_EXT_FROM_U_GET_لمبائی(value);
        value = UCNV_EXT_FROM_U_GET_ڈیٹا(value);

        اگر(length<=UCNV_EXT_FROM_U_MAX_براہ_راست_لمبائی) {
            *p_قدر=value;
            واپس isRoundtrip ? length : -length;
#اگر 0 /* not currently used */
        } ورنہ اگر(length==4) {
            /* de-serialize a 4-byte result */
            ثابت uint8_t *نتیجہ=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_بائٹیں_اشاریہ, uint8_t)+value;
            *p_قدر=
                ((uint32_t)نتیجہ[0]<<24)|
                ((uint32_t)نتیجہ[1]<<16)|
                ((uint32_t)نتیجہ[2]<<8)|
                نتیجہ[3];
            واپس isRoundtrip ? 4 : -4;
#اگر_ختم
        }
    }

    /*
     * return no match because
     * - match>1 && resultLength>4: result too long for simple conversion
     * - match==1: no match found, <subchar1> preferred
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    واپس 0;
}

/*
 * continue partial match with new input, requires cnv->preFromUFirstCP>=0
 * never called for simple, single-character conversion
 */
U_CFUNC خالی
ucnv_ext_Continue_مطابقت_From_U(UConverter *cnv,
                           UConverter_From_Unicode_دلائل *p_دلائل, int32_t src_اشاریہ,
                           UError_رمز *p_Error_رمز) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t مطابقت;

    مطابقت=ucnv_ext_مطابقت_From_U(cnv->shared_ڈیٹا->mbcs.extIndexes,
                             cnv->preFromUFirstCP,
                             cnv->preFromU, cnv->preFromULength,
                             p_دلائل->ماخذ, (int32_t)(p_دلائل->ماخذ_حد_بندی-p_دلائل->ماخذ),
                             &value,
                             cnv->useFallback, p_دلائل->flush);
    اگر(مطابقت>=2) {
        مطابقت-=2; /* remove 2 for the initial code point */

        اگر(مطابقت>=cnv->preFromULength) {
            /* advance src pointer for the consumed input */
            p_دلائل->ماخذ+=مطابقت-cnv->preFromULength;
            cnv->preFromULength=0;
        } ورنہ {
            /* the match did not use all of preFromU[] - keep the rest for replay */
            int32_t length=cnv->preFromULength-مطابقت;
            u_memmove(cnv->preFromU, cnv->preFromU+مطابقت, length);
            cnv->preFromULength=(int8_t)-length;
        }

        /* finish the partial match */
        cnv->preFromUFirstCP=U_SENTINEL;

        /* write result */
        ucnv_extWriteFromU(cnv, cnv->shared_ڈیٹا->mbcs.extIndexes,
                           value,
                           &p_دلائل->ہدف, p_دلائل->ہدف_حد_بندی,
                           &p_دلائل->offsets, src_اشاریہ,
                           p_Error_رمز);
    } ورنہ اگر(مطابقت<0) {
        /* save state for partial match */
        ثابت char16_t *s;
        int32_t j;

        /* just _append_ the newly consumed input to preFromU[] */
        s=p_دلائل->ماخذ;
        مطابقت=-مطابقت-2; /* remove 2 for the initial code point */
        برائے(j=cnv->preFromULength; j<مطابقت; ++j) {
            U_ASSERT(j>=0);
            cnv->preFromU[j]=*s++;
        }
        p_دلائل->ماخذ=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)مطابقت;
    } ورنہ /* match==0 or 1 */ {
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

        اگر(مطابقت==1) {
            /* matched, no mapping but request for <subchar1> */
            cnv->useSubChar1=سچ;
        }

        /* move the first code point to the error field */
        cnv->fromUChar32=cnv->preFromUFirstCP;
        cnv->preFromUFirstCP=U_SENTINEL;

        /* mark preFromU for replay */
        cnv->preFromULength=-cnv->preFromULength;

        /* set the error code for unassigned */
        *p_Error_رمز=U_INVALID_CHAR_ملا;
    }
}

ساکن UBool
ext_سیٹ_Use_Mapping(UConverter_Unicode_سیٹ which, int32_t min_لمبائی, uint32_t value) {
    اگر(which==UCNV_ROUNDTRIP_سیٹ) {
        // Add only code points for which the roundtrip flag is set.
        // Do not add any fallbacks, even if ucnv_fromUnicode() would use them
        // (fallbacks from PUA). See the API docs for ucnv_getUnicodeSet().
        //
        // By analogy, also do not add "good one-way" mappings.
        //
        // Do not add entries with reserved bits set.
        اگر(((value&(UCNV_EXT_FROM_U_ROUNDTRIP_FLAG|UCNV_EXT_FROM_U_RESERVED_نقاب))!=
                UCNV_EXT_FROM_U_ROUNDTRIP_FLAG)) {
            واپس جھوٹ;
        }
    } ورنہ /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
        // Do not add entries with reserved bits set.
        اگر((value&UCNV_EXT_FROM_U_RESERVED_نقاب)!=0) {
            واپس جھوٹ;
        }
    }
    // Do not add <subchar1> entries or other (future?) pseudo-entries
    // with an output length of 0.
    واپس UCNV_EXT_FROM_U_GET_لمبائی(value)>=min_لمبائی;
}

ساکن خالی
ucnv_ext_Get_Unicode_سیٹ_String(ثابت UConverter_Shared_ڈیٹا *shared_ڈیٹا,
                            ثابت int32_t *cx,
                            ثابت USetAdder *sa,
                            UConverter_Unicode_سیٹ which,
                            int32_t min_لمبائی,
                            UChar32 پہلا_CP,
                            char16_t s[UCNV_EXT_MAX_UCHARS], int32_t length,
                            int32_t section_اشاریہ,
                            UError_رمز *p_Error_رمز) {
    ثابت char16_t *fromUSectionUChars;
    ثابت uint32_t *from_USection_اقدار;

    uint32_t value;
    int32_t i, count;

    fromUSectionUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_اشاریہ, char16_t)+section_اشاریہ;
    from_USection_اقدار=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_اقدار_اشاریہ, uint32_t)+section_اشاریہ;

    /* read first pair of the section */
    count=*fromUSectionUChars++;
    value=*from_USection_اقدار++;

    اگر(ext_سیٹ_Use_Mapping(which, min_لمبائی, value)) {
        اگر(length==U16_لمبائی(پہلا_CP)) {
            /* add the initial code point */
            sa->جوڑو(sa->سیٹ, پہلا_CP);
        } ورنہ {
            /* add the string so far */
            sa->جوڑو_String(sa->سیٹ, s, length);
        }
    }

    برائے(i=0; i<count; ++i) {
        /* append this code unit and recurse or add the string */
        s[length]=fromUSectionUChars[i];
        value=from_USection_اقدار[i];

        اگر(value==0) {
            /* no mapping, do nothing */
        } ورنہ اگر(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
            ucnv_ext_Get_Unicode_سیٹ_String(
                shared_ڈیٹا, cx, sa, which, min_لمبائی,
                پہلا_CP, s, length+1,
                static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_اشاریہ(value)),
                p_Error_رمز);
        } ورنہ اگر(ext_سیٹ_Use_Mapping(which, min_لمبائی, value)) {
            sa->جوڑو_String(sa->سیٹ, s, length+1);
        }
    }
}

U_CFUNC خالی
ucnv_ext_Get_Unicode_سیٹ(ثابت UConverter_Shared_ڈیٹا *shared_ڈیٹا,
                      ثابت USetAdder *sa,
                      UConverter_Unicode_سیٹ which,
                      UConverter_سیٹ_Filter filter,
                      UError_رمز *p_Error_رمز) {
    ثابت int32_t *cx;
    ثابت uint16_t *stage12, *stage3, *ps2, *ps3;
    ثابت uint32_t *stage3b;

    uint32_t value;
    int32_t st1, stage1_لمبائی, st2, st3, min_لمبائی;

    char16_t s[UCNV_EXT_MAX_UCHARS];
    UChar32 c;
    int32_t length;

    cx=shared_ڈیٹا->mbcs.extIndexes;
    اگر(cx==خالی_اشاریہ) {
        واپس;
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_اشاریہ, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_اشاریہ, uint16_t);
    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_اشاریہ, uint32_t);

    stage1_لمبائی=cx[UCNV_EXT_FROM_U_STAGE_1_لمبائی];

    /* enumerate the from-Unicode trie table */
    c=0; /* keep track of the current code point while enumerating */

    اگر(filter==UCNV_سیٹ_FILTER_2022_CN) {
        min_لمبائی=3;
    } ورنہ اگر( shared_ڈیٹا->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ||
               filter!=UCNV_سیٹ_FILTER_NONE
    ) {
        /* DBCS-only, ignore single-byte results */
        min_لمبائی=2;
    } ورنہ {
        min_لمبائی=1;
    }

    /*
     * the trie enumeration is almost the same as
     * in MBCSGetUnicodeSet() for MBCS_OUTPUT_1
     */
    برائے(st1=0; st1<stage1_لمبائی; ++st1) {
        st2=stage12[st1];
        اگر(st2>stage1_لمبائی) {
            ps2=stage12+st2;
            برائے(st2=0; st2<64; ++st2) {
                اگر((st3=(int32_t)ps2[st2]<<UCNV_EXT_STAGE_2_بایاں_سرکاؤ)!=0) {
                    /* read the stage 3 block */
                    ps3=stage3+st3;

                    کرو {
                        value=stage3b[*ps3++];
                        اگر(value==0) {
                            /* no mapping, do nothing */
                        } ورنہ اگر(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                            // Recurse for partial results.
                            length=0;
                            U16_APPEND_UNSAFE(s, length, c);
                            ucnv_ext_Get_Unicode_سیٹ_String(
                                shared_ڈیٹا, cx, sa, which, min_لمبائی,
                                c, s, length,
                                (int32_t)UCNV_EXT_FROM_U_GET_PARTIAL_اشاریہ(value),
                                p_Error_رمز);
                        } ورنہ اگر(ext_سیٹ_Use_Mapping(which, min_لمبائی, value)) {
                            انتخاب(filter) {
                            حالت UCNV_سیٹ_FILTER_2022_CN:
                                اگر(!(UCNV_EXT_FROM_U_GET_لمبائی(value)==3 && UCNV_EXT_FROM_U_GET_ڈیٹا(value)<=0x82ffff)) {
                                    جاری;
                                }
                                توڑو;
                            حالت UCNV_سیٹ_FILTER_SJIS:
                                اگر(!(UCNV_EXT_FROM_U_GET_لمبائی(value)==2 && (value=UCNV_EXT_FROM_U_GET_ڈیٹا(value))>=0x8140 && value<=0xeffc)) {
                                    جاری;
                                }
                                توڑو;
                            حالت UCNV_سیٹ_FILTER_GR94_DBCS:
                                اگر(!(UCNV_EXT_FROM_U_GET_لمبائی(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_ڈیٹا(value))-0xa1a1)<=(0xfefe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    جاری;
                                }
                                توڑو;
                            حالت UCNV_سیٹ_FILTER_HZ:
                                اگر(!(UCNV_EXT_FROM_U_GET_لمبائی(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_ڈیٹا(value))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    جاری;
                                }
                                توڑو;
                            طےشدہ:
                                /*
                                 * UCNV_SET_FILTER_NONE,
                                 * or UCNV_SET_FILTER_DBCS_ONLY which is handled via minLength
                                 */
                                توڑو;
                            }
                            sa->جوڑو(sa->سیٹ, c);
                        }
                    } جب((++c&0xf)!=0);
                } ورنہ {
                    c+=16; /* empty stage 3 block */
                }
            }
        } ورنہ {
            c+=1024; /* empty stage 2 block */
        }
    }
}

#اگر_ختم /* #if !UCONFIG_NO_LEGACY_CONVERSION */
