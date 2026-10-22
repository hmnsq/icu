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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "ucnv_ext.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"

/* to Unicode --------------------------------------------------------------- */

/*
 * @return lookup value for the byte, if found; else 0
 */
స్థిర అంతఃవరుస uint32_t
ucnv_ext_కనుగొను_గమ్యం_U(స్థిరరాశి uint32_t *గమ్యం_USection, int32_t length, uint8_t byte) {
    uint32_t word0, మాట;
    int32_t i, ప్రారంభం, హద్దు;

    /* check the input byte against the lowest and highest section bytes */
    ప్రారంభం = static_cast<int32_t>(UCNV_EXT_గమ్యం_U_GET_BYTE(గమ్యం_USection[0]));
    హద్దు = static_cast<int32_t>(UCNV_EXT_గమ్యం_U_GET_BYTE(గమ్యం_USection[length - 1]));
    ఒకవేళ(byte<ప్రారంభం || హద్దు<byte) {
        తిరిగి 0; /* the byte is out of range */
    }

    ఒకవేళ(length==((హద్దు-ప్రారంభం)+1)) {
        /* direct access on a linear array */
        తిరిగి UCNV_EXT_గమ్యం_U_GET_విలువ(గమ్యం_USection[byte-ప్రారంభం]); /* could be 0 */
    }

    /* word0 is suitable for <=toUSection[] comparison, word for <toUSection[] */
    word0=UCNV_EXT_గమ్యం_U_MAKE_మాట(byte, 0);

    /*
     * Shift byte once instead of each section word and add 0xffffff.
     * We will compare the shifted/added byte (bbffffff) against
     * section words which have byte values in the same bit position.
     * If and only if byte bb < section byte ss then bbffffff<ssvvvvvv
     * for all v=0..f
     * so we need not mask off the lower 24 bits of each section word.
     */
    మాట=word0|UCNV_EXT_గమ్యం_U_విలువ_ముసుగు;

    /* binary search */
    ప్రారంభం=0;
    హద్దు=length;
    కొరకు(;;) {
        i=హద్దు-ప్రారంభం;
        ఒకవేళ(i<=1) {
            ఆపు; /*పూర్తి*/
        }
        /* start<limit-1 */

        ఒకవేళ(i<=4) {
            /* linear search for the last part */
            ఒకవేళ(word0<=గమ్యం_USection[ప్రారంభం]) {
                ఆపు;
            }
            ఒకవేళ(++ప్రారంభం<హద్దు && word0<=గమ్యం_USection[ప్రారంభం]) {
                ఆపు;
            }
            ఒకవేళ(++ప్రారంభం<హద్దు && word0<=గమ్యం_USection[ప్రారంభం]) {
                ఆపు;
            }
            /* always break at start==limit-1 */
            ++ప్రారంభం;
            ఆపు;
        }

        i=(ప్రారంభం+హద్దు)/2;
        ఒకవేళ(మాట<గమ్యం_USection[i]) {
            హద్దు=i;
        } లేకపోతే {
            ప్రారంభం=i;
        }
    }

    /* did we really find it? */
    ఒకవేళ(ప్రారంభం<హద్దు && byte==UCNV_EXT_గమ్యం_U_GET_BYTE(మాట=గమ్యం_USection[ప్రారంభం])) {
        తిరిగి UCNV_EXT_గమ్యం_U_GET_విలువ(మాట); /* never 0 */
    } లేకపోతే {
        తిరిగి 0; /* not found */
    }
}

/*
 * true if not an SI/SO stateful converter,
 * or if the match length fits with the current converter state
 */
#నిర్వచించు UCNV_EXT_గమ్యం_U_VERIFY_SISO_సరిపోలిక(sisoState, సరిపోలిక) \
    ((sisoState)<0 || ((sisoState)==0) == (సరిపోలిక==1))

/*
 * this works like ucnv_extMatchFromU() except
 * - the first character is in pre
 * - no trie is used
 * - the returned matchLength is not offset by 2
 */
స్థిర int32_t
ucnv_ext_సరిపోలిక_గమ్యం_U(స్థిరరాశి int32_t *cx, int8_t sisoState,
                 స్థిరరాశి అక్షరం *pre, int32_t pre_నిడివి,
                 స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
                 uint32_t *p_సరిపోలిక_విలువ,
                 UBool /*useFallback*/, UBool flush) {
    స్థిరరాశి uint32_t *గమ్యం_UTable, *గమ్యం_USection;

    uint32_t value, సరిపోలిక_విలువ;
    int32_t i, j, idx, length, సరిపోలిక_నిడివి;
    uint8_t b;

    ఒకవేళ(cx==శూన్యసూచిక || cx[UCNV_EXT_గమ్యం_U_నిడివి]<=0) {
        తిరిగి 0; /* no extension data, no match */
    }

    /* initialize */
    గమ్యం_UTable=UCNV_EXT_ARRAY(cx, UCNV_EXT_గమ్యం_U_సూచీ, uint32_t);
    idx=0;

    సరిపోలిక_విలువ=0;
    i=j=సరిపోలిక_నిడివి=0;

    ఒకవేళ(sisoState==0) {
        /* SBCS state of an SI/SO stateful converter, look at only exactly 1 byte */
        ఒకవేళ(pre_నిడివి>1) {
            తిరిగి 0; /* no match of a DBCS sequence in SBCS mode */
        } లేకపోతే ఒకవేళ(pre_నిడివి==1) {
            src_నిడివి=0;
        } లేకపోతే /* preLength==0 */ {
            ఒకవేళ(src_నిడివి>1) {
                src_నిడివి=1;
            }
        }
        flush=నిజం;
    }

    /* we must not remember fallback matches when not using fallbacks */

    /* match input units until there is a full match or the input is consumed */
    కొరకు(;;) {
        /* go to the next section */
        గమ్యం_USection=గమ్యం_UTable+idx;

        /* read first pair of the section */
        value=*గమ్యం_USection++;
        length=UCNV_EXT_గమ్యం_U_GET_BYTE(value);
        value=UCNV_EXT_గమ్యం_U_GET_విలువ(value);
        ఒకవేళ( value!=0 &&
            (UCNV_EXT_గమ్యం_U_IS_ROUNDTRIP(value) ||
             గమ్యం_U_USE_FALLBACK(useFallback)) &&
            UCNV_EXT_గమ్యం_U_VERIFY_SISO_సరిపోలిక(sisoState, i+j)
        ) {
            /* remember longest match so far */
            సరిపోలిక_విలువ=value;
            సరిపోలిక_నిడివి=i+j;
        }

        /* match pre[] then src[] */
        ఒకవేళ(i<pre_నిడివి) {
            b = static_cast<uint8_t>(pre[i++]);
        } లేకపోతే ఒకవేళ(j<src_నిడివి) {
            b = static_cast<uint8_t>(src[j++]);
        } లేకపోతే {
            /* all input consumed, partial match */
            ఒకవేళ(flush || (length=(i+j))>UCNV_EXT_MAX_బైట్లు) {
                /*
                 * end of the entire input stream, stop with the longest match so far
                 * or: partial match must not be longer than UCNV_EXT_MAX_BYTES
                 * because it must fit into state buffers
                 */
                ఆపు;
            } లేకపోతే {
                /* continue with more input next time */
                తిరిగి -length;
            }
        }

        /* search for the current char16_t */
        value=ucnv_ext_కనుగొను_గమ్యం_U(గమ్యం_USection, length, b);
        ఒకవేళ(value==0) {
            /* no match here, stop with the longest match so far */
            ఆపు;
        } లేకపోతే {
            ఒకవేళ(UCNV_EXT_గమ్యం_U_IS_PARTIAL(value)) {
                /* partial match, continue */
                idx = static_cast<int32_t>(UCNV_EXT_గమ్యం_U_GET_PARTIAL_సూచీ(value));
            } లేకపోతే {
                ఒకవేళ( (UCNV_EXT_గమ్యం_U_IS_ROUNDTRIP(value) ||
                     గమ్యం_U_USE_FALLBACK(useFallback)) &&
                    UCNV_EXT_గమ్యం_U_VERIFY_SISO_సరిపోలిక(sisoState, i+j)
                ) {
                    /* full match, stop with result */
                    సరిపోలిక_విలువ=value;
                    సరిపోలిక_నిడివి=i+j;
                } లేకపోతే {
                    /* full match on fallback not taken, stop with the longest match so far */
                }
                ఆపు;
            }
        }
    }

    ఒకవేళ(సరిపోలిక_నిడివి==0) {
        /* no match at all */
        తిరిగి 0;
    }

    /* return result */
    *p_సరిపోలిక_విలువ=UCNV_EXT_గమ్యం_U_ముసుగు_ROUNDTRIP(సరిపోలిక_విలువ);
    తిరిగి సరిపోలిక_నిడివి;
}

స్థిర అంతఃవరుస శూన్య
ucnv_ext_Write_గమ్యం_U(UConverter *cnv, స్థిరరాశి int32_t *cx,
                 uint32_t value,
                 char16_t **లక్ష్యం, స్థిరరాశి char16_t *లక్ష్యం_హద్దు,
                 int32_t **offsets, int32_t src_సూచీ,
                 UError_సంకేతం *p_Error_సంకేతం) {
    /* output the result */
    ఒకవేళ(UCNV_EXT_గమ్యం_U_IS_సంకేతం_బిందువు(value)) {
        /* output a single code point */
        ucnv_గమ్యం_UWrite_సంకేతం_బిందువు(
            cnv, UCNV_EXT_గమ్యం_U_GET_సంకేతం_బిందువు(value),
            లక్ష్యం, లక్ష్యం_హద్దు,
            offsets, src_సూచీ,
            p_Error_సంకేతం);
    } లేకపోతే {
        /* output a string - with correct data we have resultLength>0 */
        ucnv_గమ్యం_UWrite_UChars(
            cnv,
            UCNV_EXT_ARRAY(cx, UCNV_EXT_గమ్యం_U_UCHARS_సూచీ, char16_t)+
                UCNV_EXT_గమ్యం_U_GET_సూచీ(value),
            UCNV_EXT_గమ్యం_U_GET_నిడివి(value),
            లక్ష్యం, లక్ష్యం_హద్దు,
            offsets, src_సూచీ,
            p_Error_సంకేతం);
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
#నిర్వచించు UCNV_SISO_STATE(cnv) \
    ((cnv)->shared_దత్తాంశం->mbcs.outputType==MBCS_OUTPUT_2_SISO ? (int8_t)(cnv)->బహుళకం : \
     (cnv)->shared_దత్తాంశం->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ? 1 : -1)

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_సరిపోలిక_గమ్యం_U(UConverter *cnv, స్థిరరాశి int32_t *cx,
                        int32_t మొదటి_నిడివి,
                        స్థిరరాశి అక్షరం **src, స్థిరరాశి అక్షరం *src_హద్దు,
                        char16_t **లక్ష్యం, స్థిరరాశి char16_t *లక్ష్యం_హద్దు,
                        int32_t **offsets, int32_t src_సూచీ,
                        UBool flush,
                        UError_సంకేతం *p_Error_సంకేతం) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t సరిపోలిక;

    /* try to match */
    సరిపోలిక=ucnv_ext_సరిపోలిక_గమ్యం_U(cx, (int8_t)UCNV_SISO_STATE(cnv),
                           (స్థిరరాశి అక్షరం *)cnv->గమ్యం_UBytes, మొదటి_నిడివి,
                           *src, (int32_t)(src_హద్దు-*src),
                           &value,
                           cnv->useFallback, flush);
    ఒకవేళ(సరిపోలిక>0) {
        /* advance src pointer for the consumed input */
        *src+=సరిపోలిక-మొదటి_నిడివి;

        /* write result to target */
        ucnv_ext_Write_గమ్యం_U(cnv, cx,
                         value,
                         లక్ష్యం, లక్ష్యం_హద్దు,
                         offsets, src_సూచీ,
                         p_Error_సంకేతం);
        తిరిగి నిజం;
    } లేకపోతే ఒకవేళ(సరిపోలిక<0) {
        /* save state for partial match */
        స్థిరరాశి అక్షరం *s;
        int32_t j;

        /* copy the first code point */
        s=(స్థిరరాశి అక్షరం *)cnv->గమ్యం_UBytes;
        cnv->pre_గమ్యం_UFirst_నిడివి=(int8_t)మొదటి_నిడివి;
        కొరకు(j=0; j<మొదటి_నిడివి; ++j) {
            cnv->pre_గమ్యం_U[j]=*s++;
        }

        /* now copy the newly consumed input */
        s=*src;
        సరిపోలిక=-సరిపోలిక;
        కొరకు(; j<సరిపోలిక; ++j) {
            cnv->pre_గమ్యం_U[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_గమ్యం_ULength=(int8_t)సరిపోలిక;
        తిరిగి నిజం;
    } లేకపోతే /* match==0 no match */ {
        తిరిగి అబద్ధం;
    }
}

U_CFUNC UChar32
ucnv_ext_Simple_సరిపోలిక_గమ్యం_U(స్థిరరాశి int32_t *cx,
                       స్థిరరాశి అక్షరం *ఆధారం, int32_t length,
                       UBool useFallback) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t సరిపోలిక;

    ఒకవేళ(length<=0) {
        తిరిగి 0xffff;
    }

    /* try to match */
    సరిపోలిక=ucnv_ext_సరిపోలిక_గమ్యం_U(cx, -1,
                           ఆధారం, length,
                           శూన్యసూచిక, 0,
                           &value,
                           useFallback, నిజం);
    ఒకవేళ(సరిపోలిక==length) {
        /* write result for simple, single-character conversion */
        ఒకవేళ(UCNV_EXT_గమ్యం_U_IS_సంకేతం_బిందువు(value)) {
            తిరిగి UCNV_EXT_గమ్యం_U_GET_సంకేతం_బిందువు(value);
        }
    }

    /*
     * return no match because
     * - match>0 && value points to string: simple conversion cannot handle multiple code points
     * - match>0 && match!=length: not all input consumed, forbidden for this function
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    తిరిగి 0xfffe;
}

/*
 * continue partial match with new input
 * never called for simple, single-character conversion
 */
U_CFUNC శూన్య
ucnv_ext_Continue_సరిపోలిక_గమ్యం_U(UConverter *cnv,
                         UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు, int32_t src_సూచీ,
                         UError_సంకేతం *p_Error_సంకేతం) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t సరిపోలిక, length;

    సరిపోలిక=ucnv_ext_సరిపోలిక_గమ్యం_U(cnv->shared_దత్తాంశం->mbcs.extIndexes, (int8_t)UCNV_SISO_STATE(cnv),
                           cnv->pre_గమ్యం_U, cnv->pre_గమ్యం_ULength,
                           p_వాదనలు->ఆధారం, (int32_t)(p_వాదనలు->ఆధారం_హద్దు-p_వాదనలు->ఆధారం),
                           &value,
                           cnv->useFallback, p_వాదనలు->flush);
    ఒకవేళ(సరిపోలిక>0) {
        ఒకవేళ(సరిపోలిక>=cnv->pre_గమ్యం_ULength) {
            /* advance src pointer for the consumed input */
            p_వాదనలు->ఆధారం+=సరిపోలిక-cnv->pre_గమ్యం_ULength;
            cnv->pre_గమ్యం_ULength=0;
        } లేకపోతే {
            /* the match did not use all of preToU[] - keep the rest for replay */
            length=cnv->pre_గమ్యం_ULength-సరిపోలిక;
            uprv_memmove(cnv->pre_గమ్యం_U, cnv->pre_గమ్యం_U+సరిపోలిక, length);
            cnv->pre_గమ్యం_ULength=(int8_t)-length;
        }

        /* write result */
        ucnv_ext_Write_గమ్యం_U(cnv, cnv->shared_దత్తాంశం->mbcs.extIndexes,
                         value,
                         &p_వాదనలు->లక్ష్యం, p_వాదనలు->లక్ష్యం_హద్దు,
                         &p_వాదనలు->offsets, src_సూచీ,
                         p_Error_సంకేతం);
    } లేకపోతే ఒకవేళ(సరిపోలిక<0) {
        /* save state for partial match */
        స్థిరరాశి అక్షరం *s;
        int32_t j;

        /* just _append_ the newly consumed input to preToU[] */
        s=p_వాదనలు->ఆధారం;
        సరిపోలిక=-సరిపోలిక;
        కొరకు(j=cnv->pre_గమ్యం_ULength; j<సరిపోలిక; ++j) {
            cnv->pre_గమ్యం_U[j]=*s++;
        }
        p_వాదనలు->ఆధారం=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_గమ్యం_ULength=(int8_t)సరిపోలిక;
    } లేకపోతే /* match==0 */ {
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
        uprv_memcpy(cnv->గమ్యం_UBytes, cnv->pre_గమ్యం_U, cnv->pre_గమ్యం_UFirst_నిడివి);
        cnv->గమ్యం_ULength=cnv->pre_గమ్యం_UFirst_నిడివి;

        /* move the rest up inside the buffer */
        length=cnv->pre_గమ్యం_ULength-cnv->pre_గమ్యం_UFirst_నిడివి;
        ఒకవేళ(length>0) {
            uprv_memmove(cnv->pre_గమ్యం_U, cnv->pre_గమ్యం_U+cnv->pre_గమ్యం_UFirst_నిడివి, length);
        }

        /* mark preToU for replay */
        cnv->pre_గమ్యం_ULength=(int8_t)-length;

        /* set the error code for unassigned */
        *p_Error_సంకేతం=U_INVALID_CHAR_దొరికింది;
    }
}

/* from Unicode ------------------------------------------------------------- */

// Use roundtrips, "good one-way" mappings, and some normal fallbacks.
స్థిర అంతఃవరుస UBool
extFromUUseMapping(UBool useFallback, uint32_t value, UChar32 మొదటి_CP) {
    తిరిగి
        ((value&UCNV_EXT_FROM_U_స్థితి_ముసుగు)!=0 ||
            FROM_U_USE_FALLBACK(useFallback, మొదటి_CP)) &&
        (value&UCNV_EXT_FROM_U_RESERVED_ముసుగు)==0;
}

/*
 * @return index of the char16_t, if found; else <0
 */
స్థిర అంతఃవరుస int32_t
ucnv_ext_కనుగొను_From_U(స్థిరరాశి char16_t *fromUSection, int32_t length, char16_t u) {
    int32_t i, ప్రారంభం, హద్దు;

    /* binary search */
    ప్రారంభం=0;
    హద్దు=length;
    కొరకు(;;) {
        i=హద్దు-ప్రారంభం;
        ఒకవేళ(i<=1) {
            ఆపు; /*పూర్తి*/
        }
        /* start<limit-1 */

        ఒకవేళ(i<=4) {
            /* linear search for the last part */
            ఒకవేళ(u<=fromUSection[ప్రారంభం]) {
                ఆపు;
            }
            ఒకవేళ(++ప్రారంభం<హద్దు && u<=fromUSection[ప్రారంభం]) {
                ఆపు;
            }
            ఒకవేళ(++ప్రారంభం<హద్దు && u<=fromUSection[ప్రారంభం]) {
                ఆపు;
            }
            /* always break at start==limit-1 */
            ++ప్రారంభం;
            ఆపు;
        }

        i=(ప్రారంభం+హద్దు)/2;
        ఒకవేళ(u<fromUSection[i]) {
            హద్దు=i;
        } లేకపోతే {
            ప్రారంభం=i;
        }
    }

    /* did we really find it? */
    ఒకవేళ(ప్రారంభం<హద్దు && u==fromUSection[ప్రారంభం]) {
        తిరిగి ప్రారంభం;
    } లేకపోతే {
        తిరిగి -1; /* not found */
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
స్థిర int32_t
ucnv_ext_సరిపోలిక_From_U(స్థిరరాశి int32_t *cx,
                   UChar32 మొదటి_CP,
                   స్థిరరాశి char16_t *pre, int32_t pre_నిడివి,
                   స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                   uint32_t *p_సరిపోలిక_విలువ,
                   UBool useFallback, UBool flush) {
    స్థిరరాశి uint16_t *stage12, *stage3;
    స్థిరరాశి uint32_t *stage3b;

    స్థిరరాశి char16_t *fromUTableUChars, *fromUSectionUChars;
    స్థిరరాశి uint32_t *from_UTable_విలువలు, *from_USection_విలువలు;

    uint32_t value, సరిపోలిక_విలువ;
    int32_t i, j, idx, length, సరిపోలిక_నిడివి;
    char16_t c;

    ఒకవేళ(cx==శూన్యసూచిక) {
        తిరిగి 0; /* no extension data, no match */
    }

    /* trie lookup of firstCP */
    idx=మొదటి_CP>>10; /* stage 1 index */
    ఒకవేళ(idx>=cx[UCNV_EXT_FROM_U_STAGE_1_నిడివి]) {
        తిరిగి 0; /* the first code point is outside the trie */
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_సూచీ, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_సూచీ, uint16_t);
    idx=UCNV_EXT_FROM_U(stage12, stage3, idx, మొదటి_CP);

    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_సూచీ, uint32_t);
    value=stage3b[idx];
    ఒకవేళ(value==0) {
        తిరిగి 0;
    }

    /*
     * Tests for (value&UCNV_EXT_FROM_U_RESERVED_MASK)==0:
     * Do not interpret values with reserved bits used, for forward compatibility,
     * and do not even remember intermediate results with reserved bits used.
     */

    ఒకవేళ(UCNV_EXT_గమ్యం_U_IS_PARTIAL(value)) {
        /* partial match, enter the loop below */
        idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_సూచీ(value));

        /* initialize */
        fromUTableUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_సూచీ, char16_t);
        from_UTable_విలువలు=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_విలువలు_సూచీ, uint32_t);

        సరిపోలిక_విలువ=0;
        i=j=సరిపోలిక_నిడివి=0;

        /* we must not remember fallback matches when not using fallbacks */

        /* match input units until there is a full match or the input is consumed */
        కొరకు(;;) {
            /* go to the next section */
            fromUSectionUChars=fromUTableUChars+idx;
            from_USection_విలువలు=from_UTable_విలువలు+idx;

            /* read first pair of the section */
            length=*fromUSectionUChars++;
            value=*from_USection_విలువలు++;
            ఒకవేళ(value!=0 && extFromUUseMapping(useFallback, value, మొదటి_CP)) {
                /* remember longest match so far */
                సరిపోలిక_విలువ=value;
                సరిపోలిక_నిడివి=2+i+j;
            }

            /* match pre[] then src[] */
            ఒకవేళ(i<pre_నిడివి) {
                c=pre[i++];
            } లేకపోతే ఒకవేళ(j<src_నిడివి) {
                c=src[j++];
            } లేకపోతే {
                /* all input consumed, partial match */
                ఒకవేళ(flush || (length=(i+j))>UCNV_EXT_MAX_UCHARS) {
                    /*
                     * end of the entire input stream, stop with the longest match so far
                     * or: partial match must not be longer than UCNV_EXT_MAX_UCHARS
                     * because it must fit into state buffers
                     */
                    ఆపు;
                } లేకపోతే {
                    /* continue with more input next time */
                    తిరిగి -(2+length);
                }
            }

            /* search for the current char16_t */
            idx=ucnv_ext_కనుగొను_From_U(fromUSectionUChars, length, c);
            ఒకవేళ(idx<0) {
                /* no match here, stop with the longest match so far */
                ఆపు;
            } లేకపోతే {
                value=from_USection_విలువలు[idx];
                ఒకవేళ(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                    /* partial match, continue */
                    idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_సూచీ(value));
                } లేకపోతే {
                    ఒకవేళ(extFromUUseMapping(useFallback, value, మొదటి_CP)) {
                        /* full match, stop with result */
                        సరిపోలిక_విలువ=value;
                        సరిపోలిక_నిడివి=2+i+j;
                    } లేకపోతే {
                        /* full match on fallback not taken, stop with the longest match so far */
                    }
                    ఆపు;
                }
            }
        }

        ఒకవేళ(సరిపోలిక_నిడివి==0) {
            /* no match at all */
            తిరిగి 0;
        }
    } లేకపోతే /* result from firstCP trie lookup */ {
        ఒకవేళ(extFromUUseMapping(useFallback, value, మొదటి_CP)) {
            /* full match, stop with result */
            సరిపోలిక_విలువ=value;
            సరిపోలిక_నిడివి=2;
        } లేకపోతే {
            /* fallback not taken */
            తిరిగి 0;
        }
    }

    /* return result */
    ఒకవేళ(సరిపోలిక_విలువ==UCNV_EXT_FROM_U_SUBCHAR1) {
        తిరిగి 1; /* assert matchLength==2 */
    }

    *p_సరిపోలిక_విలువ=సరిపోలిక_విలువ;
    తిరిగి సరిపోలిక_నిడివి;
}

/*
 * @param value fromUnicode mapping table value; ignores roundtrip and reserved bits
 */
స్థిర అంతఃవరుస శూన్య
ucnv_extWriteFromU(UConverter *cnv, స్థిరరాశి int32_t *cx,
                   uint32_t value,
                   అక్షరం **లక్ష్యం, స్థిరరాశి అక్షరం *లక్ష్యం_హద్దు,
                   int32_t **offsets, int32_t src_సూచీ,
                   UError_సంకేతం *p_Error_సంకేతం) {
    uint8_t బఫరు[1+UCNV_EXT_MAX_బైట్లు];
    స్థిరరాశి uint8_t *ఫలితం;
    int32_t length, prev_నిడివి;

    length=UCNV_EXT_FROM_U_GET_నిడివి(value);
    value = UCNV_EXT_FROM_U_GET_దత్తాంశం(value);

    /* output the result */
    ఒకవేళ(length<=UCNV_EXT_FROM_U_MAX_ప్రత్యక్ష_నిడివి) {
        /*
         * Generate a byte array and then write it below.
         * This is not the fastest possible way, but it should be ok for
         * extension mappings, and it is much simpler.
         * Offset and overflow handling are only done once this way.
         */
        uint8_t *p=బఫరు+1; /* reserve buffer[0] for shiftByte below */
        ఎంపిక(length) {
        స్థితి 3:
            *p++ = static_cast<uint8_t>(value >> 16);
            U_FALLTHROUGH;
        స్థితి 2:
            *p++ = static_cast<uint8_t>(value >> 8);
            U_FALLTHROUGH;
        స్థితి 1:
            *p++ = static_cast<uint8_t>(value);
            U_FALLTHROUGH;
        అప్రమేయ:
            ఆపు; /* will never occur */
        }
        ఫలితం=బఫరు+1;
    } లేకపోతే {
        ఫలితం=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_బైట్లు_సూచీ, uint8_t)+value;
    }

    /* with correct data we have length>0 */

    ఒకవేళ((prev_నిడివి=cnv->from_Unicode_స్థితి)!=0) {
        /* handle SI/SO stateful output */
        uint8_t జరుపు_Byte;

        ఒకవేళ(prev_నిడివి>1 && length==1) {
            /* change from double-byte mode to single-byte */
            జరుపు_Byte = static_cast<uint8_t>(UCNV_SI);
            cnv->from_Unicode_స్థితి=1;
        } లేకపోతే ఒకవేళ(prev_నిడివి==1 && length>1) {
            /* change from single-byte mode to double-byte */
            జరుపు_Byte = static_cast<uint8_t>(UCNV_SO);
            cnv->from_Unicode_స్థితి=2;
        } లేకపోతే {
            జరుపు_Byte=0;
        }

        ఒకవేళ(జరుపు_Byte!=0) {
            /* prepend the shift byte to the result bytes */
            బఫరు[0]=జరుపు_Byte;
            ఒకవేళ(ఫలితం!=బఫరు+1) {
                uprv_memcpy(బఫరు+1, ఫలితం, length);
            }
            ఫలితం=బఫరు;
            ++length;
        }
    }

    ucnv_from_UWrite_బైట్లు(cnv, reinterpret_cast<స్థిరరాశి అక్షరం*>(ఫలితం), length,
                         లక్ష్యం, లక్ష్యం_హద్దు,
                         offsets, src_సూచీ,
                         p_Error_సంకేతం);
}

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_సరిపోలిక_From_U(UConverter *cnv, స్థిరరాశి int32_t *cx,
                          UChar32 cp,
                          స్థిరరాశి char16_t **src, స్థిరరాశి char16_t *src_హద్దు,
                          అక్షరం **లక్ష్యం, స్థిరరాశి అక్షరం *లక్ష్యం_హద్దు,
                          int32_t **offsets, int32_t src_సూచీ,
                          UBool flush,
                          UError_సంకేతం *p_Error_సంకేతం) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t సరిపోలిక;

    /* try to match */
    సరిపోలిక=ucnv_ext_సరిపోలిక_From_U(cx, cp,
                             శూన్యసూచిక, 0,
                             *src, (int32_t)(src_హద్దు-*src),
                             &value,
                             cnv->useFallback, flush);

    /* reject a match if the result is a single byte for DBCS-only */
    ఒకవేళ( సరిపోలిక>=2 &&
        !(UCNV_EXT_FROM_U_GET_నిడివి(value)==1 &&
          cnv->shared_దత్తాంశం->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY)
    ) {
        /* advance src pointer for the consumed input */
        *src+=సరిపోలిక-2; /* remove 2 for the initial code point */

        /* write result to target */
        ucnv_extWriteFromU(cnv, cx,
                           value,
                           లక్ష్యం, లక్ష్యం_హద్దు,
                           offsets, src_సూచీ,
                           p_Error_సంకేతం);
        తిరిగి నిజం;
    } లేకపోతే ఒకవేళ(సరిపోలిక<0) {
        /* save state for partial match */
        స్థిరరాశి char16_t *s;
        int32_t j;

        /* copy the first code point */
        cnv->preFromUFirstCP=cp;

        /* now copy the newly consumed input */
        s=*src;
        సరిపోలిక=-సరిపోలిక-2; /* remove 2 for the initial code point */
        కొరకు(j=0; j<సరిపోలిక; ++j) {
            cnv->preFromU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)సరిపోలిక;
        తిరిగి నిజం;
    } లేకపోతే ఒకవేళ(సరిపోలిక==1) {
        /* matched, no mapping but request for <subchar1> */
        cnv->useSubChar1=నిజం;
        తిరిగి అబద్ధం;
    } లేకపోతే /* match==0 no match */ {
        తిరిగి అబద్ధం;
    }
}

/*
 * Used by ISO 2022 implementation.
 * @return number of bytes in *pValue; negative number if fallback; 0 for no mapping
 */
U_CFUNC int32_t
ucnv_ext_Simple_సరిపోలిక_From_U(స్థిరరాశి int32_t *cx,
                         UChar32 cp, uint32_t *p_విలువ,
                         UBool useFallback) {
    uint32_t value;
    int32_t సరిపోలిక;

    /* try to match */
    సరిపోలిక=ucnv_ext_సరిపోలిక_From_U(cx,
                             cp,
                             శూన్యసూచిక, 0,
                             శూన్యసూచిక, 0,
                             &value,
                             useFallback, నిజం);
    ఒకవేళ(సరిపోలిక>=2) {
        /* write result for simple, single-character conversion */
        int32_t length;
        పూర్ణాంకం isRoundtrip;

        isRoundtrip=UCNV_EXT_FROM_U_IS_ROUNDTRIP(value);
        length=UCNV_EXT_FROM_U_GET_నిడివి(value);
        value = UCNV_EXT_FROM_U_GET_దత్తాంశం(value);

        ఒకవేళ(length<=UCNV_EXT_FROM_U_MAX_ప్రత్యక్ష_నిడివి) {
            *p_విలువ=value;
            తిరిగి isRoundtrip ? length : -length;
#ఒకవేళ 0 /* not currently used */
        } లేకపోతే ఒకవేళ(length==4) {
            /* de-serialize a 4-byte result */
            స్థిరరాశి uint8_t *ఫలితం=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_బైట్లు_సూచీ, uint8_t)+value;
            *p_విలువ=
                ((uint32_t)ఫలితం[0]<<24)|
                ((uint32_t)ఫలితం[1]<<16)|
                ((uint32_t)ఫలితం[2]<<8)|
                ఫలితం[3];
            తిరిగి isRoundtrip ? 4 : -4;
#అయితే_ముగింపు
        }
    }

    /*
     * return no match because
     * - match>1 && resultLength>4: result too long for simple conversion
     * - match==1: no match found, <subchar1> preferred
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    తిరిగి 0;
}

/*
 * continue partial match with new input, requires cnv->preFromUFirstCP>=0
 * never called for simple, single-character conversion
 */
U_CFUNC శూన్య
ucnv_ext_Continue_సరిపోలిక_From_U(UConverter *cnv,
                           UConverter_From_Unicode_వాదనలు *p_వాదనలు, int32_t src_సూచీ,
                           UError_సంకేతం *p_Error_సంకేతం) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t సరిపోలిక;

    సరిపోలిక=ucnv_ext_సరిపోలిక_From_U(cnv->shared_దత్తాంశం->mbcs.extIndexes,
                             cnv->preFromUFirstCP,
                             cnv->preFromU, cnv->preFromULength,
                             p_వాదనలు->ఆధారం, (int32_t)(p_వాదనలు->ఆధారం_హద్దు-p_వాదనలు->ఆధారం),
                             &value,
                             cnv->useFallback, p_వాదనలు->flush);
    ఒకవేళ(సరిపోలిక>=2) {
        సరిపోలిక-=2; /* remove 2 for the initial code point */

        ఒకవేళ(సరిపోలిక>=cnv->preFromULength) {
            /* advance src pointer for the consumed input */
            p_వాదనలు->ఆధారం+=సరిపోలిక-cnv->preFromULength;
            cnv->preFromULength=0;
        } లేకపోతే {
            /* the match did not use all of preFromU[] - keep the rest for replay */
            int32_t length=cnv->preFromULength-సరిపోలిక;
            u_memmove(cnv->preFromU, cnv->preFromU+సరిపోలిక, length);
            cnv->preFromULength=(int8_t)-length;
        }

        /* finish the partial match */
        cnv->preFromUFirstCP=U_SENTINEL;

        /* write result */
        ucnv_extWriteFromU(cnv, cnv->shared_దత్తాంశం->mbcs.extIndexes,
                           value,
                           &p_వాదనలు->లక్ష్యం, p_వాదనలు->లక్ష్యం_హద్దు,
                           &p_వాదనలు->offsets, src_సూచీ,
                           p_Error_సంకేతం);
    } లేకపోతే ఒకవేళ(సరిపోలిక<0) {
        /* save state for partial match */
        స్థిరరాశి char16_t *s;
        int32_t j;

        /* just _append_ the newly consumed input to preFromU[] */
        s=p_వాదనలు->ఆధారం;
        సరిపోలిక=-సరిపోలిక-2; /* remove 2 for the initial code point */
        కొరకు(j=cnv->preFromULength; j<సరిపోలిక; ++j) {
            U_ASSERT(j>=0);
            cnv->preFromU[j]=*s++;
        }
        p_వాదనలు->ఆధారం=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)సరిపోలిక;
    } లేకపోతే /* match==0 or 1 */ {
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

        ఒకవేళ(సరిపోలిక==1) {
            /* matched, no mapping but request for <subchar1> */
            cnv->useSubChar1=నిజం;
        }

        /* move the first code point to the error field */
        cnv->fromUChar32=cnv->preFromUFirstCP;
        cnv->preFromUFirstCP=U_SENTINEL;

        /* mark preFromU for replay */
        cnv->preFromULength=-cnv->preFromULength;

        /* set the error code for unassigned */
        *p_Error_సంకేతం=U_INVALID_CHAR_దొరికింది;
    }
}

స్థిర UBool
ext_సమితి_Use_Mapping(UConverter_Unicode_సమితి which, int32_t min_నిడివి, uint32_t value) {
    ఒకవేళ(which==UCNV_ROUNDTRIP_సమితి) {
        // Add only code points for which the roundtrip flag is set.
        // Do not add any fallbacks, even if ucnv_fromUnicode() would use them
        // (fallbacks from PUA). See the API docs for ucnv_getUnicodeSet().
        //
        // By analogy, also do not add "good one-way" mappings.
        //
        // Do not add entries with reserved bits set.
        ఒకవేళ(((value&(UCNV_EXT_FROM_U_ROUNDTRIP_FLAG|UCNV_EXT_FROM_U_RESERVED_ముసుగు))!=
                UCNV_EXT_FROM_U_ROUNDTRIP_FLAG)) {
            తిరిగి అబద్ధం;
        }
    } లేకపోతే /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
        // Do not add entries with reserved bits set.
        ఒకవేళ((value&UCNV_EXT_FROM_U_RESERVED_ముసుగు)!=0) {
            తిరిగి అబద్ధం;
        }
    }
    // Do not add <subchar1> entries or other (future?) pseudo-entries
    // with an output length of 0.
    తిరిగి UCNV_EXT_FROM_U_GET_నిడివి(value)>=min_నిడివి;
}

స్థిర శూన్య
ucnv_ext_Get_Unicode_సమితి_String(స్థిరరాశి UConverter_Shared_దత్తాంశం *shared_దత్తాంశం,
                            స్థిరరాశి int32_t *cx,
                            స్థిరరాశి USetAdder *sa,
                            UConverter_Unicode_సమితి which,
                            int32_t min_నిడివి,
                            UChar32 మొదటి_CP,
                            char16_t s[UCNV_EXT_MAX_UCHARS], int32_t length,
                            int32_t section_సూచీ,
                            UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి char16_t *fromUSectionUChars;
    స్థిరరాశి uint32_t *from_USection_విలువలు;

    uint32_t value;
    int32_t i, count;

    fromUSectionUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_సూచీ, char16_t)+section_సూచీ;
    from_USection_విలువలు=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_విలువలు_సూచీ, uint32_t)+section_సూచీ;

    /* read first pair of the section */
    count=*fromUSectionUChars++;
    value=*from_USection_విలువలు++;

    ఒకవేళ(ext_సమితి_Use_Mapping(which, min_నిడివి, value)) {
        ఒకవేళ(length==U16_నిడివి(మొదటి_CP)) {
            /* add the initial code point */
            sa->కూడు(sa->సమితి, మొదటి_CP);
        } లేకపోతే {
            /* add the string so far */
            sa->కూడు_String(sa->సమితి, s, length);
        }
    }

    కొరకు(i=0; i<count; ++i) {
        /* append this code unit and recurse or add the string */
        s[length]=fromUSectionUChars[i];
        value=from_USection_విలువలు[i];

        ఒకవేళ(value==0) {
            /* no mapping, do nothing */
        } లేకపోతే ఒకవేళ(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
            ucnv_ext_Get_Unicode_సమితి_String(
                shared_దత్తాంశం, cx, sa, which, min_నిడివి,
                మొదటి_CP, s, length+1,
                static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_సూచీ(value)),
                p_Error_సంకేతం);
        } లేకపోతే ఒకవేళ(ext_సమితి_Use_Mapping(which, min_నిడివి, value)) {
            sa->కూడు_String(sa->సమితి, s, length+1);
        }
    }
}

U_CFUNC శూన్య
ucnv_ext_Get_Unicode_సమితి(స్థిరరాశి UConverter_Shared_దత్తాంశం *shared_దత్తాంశం,
                      స్థిరరాశి USetAdder *sa,
                      UConverter_Unicode_సమితి which,
                      UConverter_సమితి_Filter filter,
                      UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి int32_t *cx;
    స్థిరరాశి uint16_t *stage12, *stage3, *ps2, *ps3;
    స్థిరరాశి uint32_t *stage3b;

    uint32_t value;
    int32_t st1, stage1_నిడివి, st2, st3, min_నిడివి;

    char16_t s[UCNV_EXT_MAX_UCHARS];
    UChar32 c;
    int32_t length;

    cx=shared_దత్తాంశం->mbcs.extIndexes;
    ఒకవేళ(cx==శూన్యసూచిక) {
        తిరిగి;
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_సూచీ, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_సూచీ, uint16_t);
    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_సూచీ, uint32_t);

    stage1_నిడివి=cx[UCNV_EXT_FROM_U_STAGE_1_నిడివి];

    /* enumerate the from-Unicode trie table */
    c=0; /* keep track of the current code point while enumerating */

    ఒకవేళ(filter==UCNV_సమితి_FILTER_2022_CN) {
        min_నిడివి=3;
    } లేకపోతే ఒకవేళ( shared_దత్తాంశం->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ||
               filter!=UCNV_సమితి_FILTER_NONE
    ) {
        /* DBCS-only, ignore single-byte results */
        min_నిడివి=2;
    } లేకపోతే {
        min_నిడివి=1;
    }

    /*
     * the trie enumeration is almost the same as
     * in MBCSGetUnicodeSet() for MBCS_OUTPUT_1
     */
    కొరకు(st1=0; st1<stage1_నిడివి; ++st1) {
        st2=stage12[st1];
        ఒకవేళ(st2>stage1_నిడివి) {
            ps2=stage12+st2;
            కొరకు(st2=0; st2<64; ++st2) {
                ఒకవేళ((st3=(int32_t)ps2[st2]<<UCNV_EXT_STAGE_2_ఎడమ_జరుపు)!=0) {
                    /* read the stage 3 block */
                    ps3=stage3+st3;

                    చేయి {
                        value=stage3b[*ps3++];
                        ఒకవేళ(value==0) {
                            /* no mapping, do nothing */
                        } లేకపోతే ఒకవేళ(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                            // Recurse for partial results.
                            length=0;
                            U16_APPEND_UNSAFE(s, length, c);
                            ucnv_ext_Get_Unicode_సమితి_String(
                                shared_దత్తాంశం, cx, sa, which, min_నిడివి,
                                c, s, length,
                                (int32_t)UCNV_EXT_FROM_U_GET_PARTIAL_సూచీ(value),
                                p_Error_సంకేతం);
                        } లేకపోతే ఒకవేళ(ext_సమితి_Use_Mapping(which, min_నిడివి, value)) {
                            ఎంపిక(filter) {
                            స్థితి UCNV_సమితి_FILTER_2022_CN:
                                ఒకవేళ(!(UCNV_EXT_FROM_U_GET_నిడివి(value)==3 && UCNV_EXT_FROM_U_GET_దత్తాంశం(value)<=0x82ffff)) {
                                    కొనసాగు;
                                }
                                ఆపు;
                            స్థితి UCNV_సమితి_FILTER_SJIS:
                                ఒకవేళ(!(UCNV_EXT_FROM_U_GET_నిడివి(value)==2 && (value=UCNV_EXT_FROM_U_GET_దత్తాంశం(value))>=0x8140 && value<=0xeffc)) {
                                    కొనసాగు;
                                }
                                ఆపు;
                            స్థితి UCNV_సమితి_FILTER_GR94_DBCS:
                                ఒకవేళ(!(UCNV_EXT_FROM_U_GET_నిడివి(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_దత్తాంశం(value))-0xa1a1)<=(0xfefe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    కొనసాగు;
                                }
                                ఆపు;
                            స్థితి UCNV_సమితి_FILTER_HZ:
                                ఒకవేళ(!(UCNV_EXT_FROM_U_GET_నిడివి(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_దత్తాంశం(value))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    కొనసాగు;
                                }
                                ఆపు;
                            అప్రమేయ:
                                /*
                                 * UCNV_SET_FILTER_NONE,
                                 * or UCNV_SET_FILTER_DBCS_ONLY which is handled via minLength
                                 */
                                ఆపు;
                            }
                            sa->కూడు(sa->సమితి, c);
                        }
                    } అయితే((++c&0xf)!=0);
                } లేకపోతే {
                    c+=16; /* empty stage 3 block */
                }
            }
        } లేకపోతే {
            c+=1024; /* empty stage 2 block */
        }
    }
}

#అయితే_ముగింపు /* #if !UCONFIG_NO_LEGACY_CONVERSION */
