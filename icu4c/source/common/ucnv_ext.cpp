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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#অন্তর্ভাব "unicode/uset.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "ucnv_bld.h"
#অন্তর্ভাব "ucnv_cnv.h"
#অন্তর্ভাব "ucnv_ext.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uassert.h"

/* to Unicode --------------------------------------------------------------- */

/*
 * @return lookup value for the byte, if found; else 0
 */
স্থির অন্তঃরেখ uint32_t
ucnv_extFindToU(ধ্রুবক uint32_t *toUSection, int32_t length, uint8_t byte) {
    uint32_t word0, word;
    int32_t i, start, limit;

    /* check the input byte against the lowest and highest section bytes */
    start = static_cast<int32_t>(UCNV_EXT_TO_U_GET_BYTE(toUSection[0]));
    limit = static_cast<int32_t>(UCNV_EXT_TO_U_GET_BYTE(toUSection[length - 1]));
    যদি(byte<start || limit<byte) {
        ফেরত 0; /* the byte is out of range */
    }

    যদি(length==((limit-start)+1)) {
        /* direct access on a linear array */
        ফেরত UCNV_EXT_TO_U_GET_মান(toUSection[byte-start]); /* could be 0 */
    }

    /* word0 is suitable for <=toUSection[] comparison, word for <toUSection[] */
    word0=UCNV_EXT_TO_U_MAKE_WORD(byte, 0);

    /*
     * Shift byte once instead of each section word and add 0xffffff.
     * We will compare the shifted/added byte (bbffffff) against
     * section words which have byte values in the same bit position.
     * If and only if byte bb < section byte ss then bbffffff<ssvvvvvv
     * for all v=0..f
     * so we need not mask off the lower 24 bits of each section word.
     */
    word=word0|UCNV_EXT_TO_U_মান_MASK;

    /* binary search */
    start=0;
    limit=length;
    জন্য(;;) {
        i=limit-start;
        যদি(i<=1) {
            বিরতি; /* done */
        }
        /* start<limit-1 */

        যদি(i<=4) {
            /* linear search for the last part */
            যদি(word0<=toUSection[start]) {
                বিরতি;
            }
            যদি(++start<limit && word0<=toUSection[start]) {
                বিরতি;
            }
            যদি(++start<limit && word0<=toUSection[start]) {
                বিরতি;
            }
            /* always break at start==limit-1 */
            ++start;
            বিরতি;
        }

        i=(start+limit)/2;
        যদি(word<toUSection[i]) {
            limit=i;
        } অন্যথা {
            start=i;
        }
    }

    /* did we really find it? */
    যদি(start<limit && byte==UCNV_EXT_TO_U_GET_BYTE(word=toUSection[start])) {
        ফেরত UCNV_EXT_TO_U_GET_মান(word); /* never 0 */
    } অন্যথা {
        ফেরত 0; /* not found */
    }
}

/*
 * true if not an SI/SO stateful converter,
 * or if the match length fits with the current converter state
 */
#define UCNV_EXT_TO_U_VERIFY_SISO_MATCH(sisoState, match) \
    ((sisoState)<0 || ((sisoState)==0) == (match==1))

/*
 * this works like ucnv_extMatchFromU() except
 * - the first character is in pre
 * - no trie is used
 * - the returned matchLength is not offset by 2
 */
স্থির int32_t
ucnv_extMatchToU(ধ্রুবক int32_t *cx, int8_t sisoState,
                 ধ্রুবক অক্ষর *pre, int32_t pre_দৈর্ঘ্য,
                 ধ্রুবক অক্ষর *src, int32_t src_দৈর্ঘ্য,
                 uint32_t *p_Match_মান,
                 UBool /*useFallback*/, UBool flush) {
    ধ্রুবক uint32_t *toUTable, *toUSection;

    uint32_t value, match_মান;
    int32_t i, j, idx, length, match_দৈর্ঘ্য;
    uint8_t b;

    যদি(cx==nullptr || cx[UCNV_EXT_TO_U_দৈর্ঘ্য]<=0) {
        ফেরত 0; /* no extension data, no match */
    }

    /* initialize */
    toUTable=UCNV_EXT_ARRAY(cx, UCNV_EXT_TO_U_সূচক, uint32_t);
    idx=0;

    match_মান=0;
    i=j=match_দৈর্ঘ্য=0;

    যদি(sisoState==0) {
        /* SBCS state of an SI/SO stateful converter, look at only exactly 1 byte */
        যদি(pre_দৈর্ঘ্য>1) {
            ফেরত 0; /* no match of a DBCS sequence in SBCS mode */
        } অন্যথা যদি(pre_দৈর্ঘ্য==1) {
            src_দৈর্ঘ্য=0;
        } অন্যথা /* preLength==0 */ {
            যদি(src_দৈর্ঘ্য>1) {
                src_দৈর্ঘ্য=1;
            }
        }
        flush=সত্য;
    }

    /* we must not remember fallback matches when not using fallbacks */

    /* match input units until there is a full match or the input is consumed */
    জন্য(;;) {
        /* go to the next section */
        toUSection=toUTable+idx;

        /* read first pair of the section */
        value=*toUSection++;
        length=UCNV_EXT_TO_U_GET_BYTE(value);
        value=UCNV_EXT_TO_U_GET_মান(value);
        যদি( value!=0 &&
            (UCNV_EXT_TO_U_IS_ROUNDTRIP(value) ||
             TO_U_USE_FALLBACK(useFallback)) &&
            UCNV_EXT_TO_U_VERIFY_SISO_MATCH(sisoState, i+j)
        ) {
            /* remember longest match so far */
            match_মান=value;
            match_দৈর্ঘ্য=i+j;
        }

        /* match pre[] then src[] */
        যদি(i<pre_দৈর্ঘ্য) {
            b = static_cast<uint8_t>(pre[i++]);
        } অন্যথা যদি(j<src_দৈর্ঘ্য) {
            b = static_cast<uint8_t>(src[j++]);
        } অন্যথা {
            /* all input consumed, partial match */
            যদি(flush || (length=(i+j))>UCNV_EXT_MAX_BYTES) {
                /*
                 * end of the entire input stream, stop with the longest match so far
                 * or: partial match must not be longer than UCNV_EXT_MAX_BYTES
                 * because it must fit into state buffers
                 */
                বিরতি;
            } অন্যথা {
                /* continue with more input next time */
                ফেরত -length;
            }
        }

        /* search for the current char16_t */
        value=ucnv_extFindToU(toUSection, length, b);
        যদি(value==0) {
            /* no match here, stop with the longest match so far */
            বিরতি;
        } অন্যথা {
            যদি(UCNV_EXT_TO_U_IS_PARTIAL(value)) {
                /* partial match, continue */
                idx = static_cast<int32_t>(UCNV_EXT_TO_U_GET_PARTIAL_সূচক(value));
            } অন্যথা {
                যদি( (UCNV_EXT_TO_U_IS_ROUNDTRIP(value) ||
                     TO_U_USE_FALLBACK(useFallback)) &&
                    UCNV_EXT_TO_U_VERIFY_SISO_MATCH(sisoState, i+j)
                ) {
                    /* full match, stop with result */
                    match_মান=value;
                    match_দৈর্ঘ্য=i+j;
                } অন্যথা {
                    /* full match on fallback not taken, stop with the longest match so far */
                }
                বিরতি;
            }
        }
    }

    যদি(match_দৈর্ঘ্য==0) {
        /* no match at all */
        ফেরত 0;
    }

    /* return result */
    *p_Match_মান=UCNV_EXT_TO_U_MASK_ROUNDTRIP(match_মান);
    ফেরত match_দৈর্ঘ্য;
}

স্থির অন্তঃরেখ শূন্য
ucnv_extWriteToU(UConverter *cnv, ধ্রুবক int32_t *cx,
                 uint32_t value,
                 char16_t **target, ধ্রুবক char16_t *targetLimit,
                 int32_t **offsets, int32_t src_সূচক,
                 UErrorCode *pErrorCode) {
    /* output the result */
    যদি(UCNV_EXT_TO_U_IS_CODE_POINT(value)) {
        /* output a single code point */
        ucnv_toUWriteCodePoint(
            cnv, UCNV_EXT_TO_U_GET_CODE_POINT(value),
            target, targetLimit,
            offsets, src_সূচক,
            pErrorCode);
    } অন্যথা {
        /* output a string - with correct data we have resultLength>0 */
        ucnv_toUWriteUChars(
            cnv,
            UCNV_EXT_ARRAY(cx, UCNV_EXT_TO_U_UCHARS_সূচক, char16_t)+
                UCNV_EXT_TO_U_GET_সূচক(value),
            UCNV_EXT_TO_U_GET_দৈর্ঘ্য(value),
            target, targetLimit,
            offsets, src_সূচক,
            pErrorCode);
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
#define UCNV_SISO_STATE(cnv) \
    ((cnv)->shared_তথ্য->mbcs.outputType==MBCS_OUTPUT_2_SISO ? (int8_t)(cnv)->mode : \
     (cnv)->shared_তথ্য->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ? 1 : -1)

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_extInitialMatchToU(UConverter *cnv, ধ্রুবক int32_t *cx,
                        int32_t first_দৈর্ঘ্য,
                        ধ্রুবক অক্ষর **src, ধ্রুবক অক্ষর *srcLimit,
                        char16_t **target, ধ্রুবক char16_t *targetLimit,
                        int32_t **offsets, int32_t src_সূচক,
                        UBool flush,
                        UErrorCode *pErrorCode) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match;

    /* try to match */
    match=ucnv_extMatchToU(cx, (int8_t)UCNV_SISO_STATE(cnv),
                           (ধ্রুবক অক্ষর *)cnv->toUBytes, first_দৈর্ঘ্য,
                           *src, (int32_t)(srcLimit-*src),
                           &value,
                           cnv->useFallback, flush);
    যদি(match>0) {
        /* advance src pointer for the consumed input */
        *src+=match-first_দৈর্ঘ্য;

        /* write result to target */
        ucnv_extWriteToU(cnv, cx,
                         value,
                         target, targetLimit,
                         offsets, src_সূচক,
                         pErrorCode);
        ফেরত সত্য;
    } অন্যথা যদি(match<0) {
        /* save state for partial match */
        ধ্রুবক অক্ষর *s;
        int32_t j;

        /* copy the first code point */
        s=(ধ্রুবক অক্ষর *)cnv->toUBytes;
        cnv->pre_To_UFirst_দৈর্ঘ্য=(int8_t)first_দৈর্ঘ্য;
        জন্য(j=0; j<first_দৈর্ঘ্য; ++j) {
            cnv->preToU[j]=*s++;
        }

        /* now copy the newly consumed input */
        s=*src;
        match=-match;
        জন্য(; j<match; ++j) {
            cnv->preToU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preToULength=(int8_t)match;
        ফেরত সত্য;
    } অন্যথা /* match==0 no match */ {
        ফেরত মিথ্যা;
    }
}

U_CFUNC UChar32
ucnv_extSimpleMatchToU(ধ্রুবক int32_t *cx,
                       ধ্রুবক অক্ষর *source, int32_t length,
                       UBool useFallback) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match;

    যদি(length<=0) {
        ফেরত 0xffff;
    }

    /* try to match */
    match=ucnv_extMatchToU(cx, -1,
                           source, length,
                           nullptr, 0,
                           &value,
                           useFallback, সত্য);
    যদি(match==length) {
        /* write result for simple, single-character conversion */
        যদি(UCNV_EXT_TO_U_IS_CODE_POINT(value)) {
            ফেরত UCNV_EXT_TO_U_GET_CODE_POINT(value);
        }
    }

    /*
     * return no match because
     * - match>0 && value points to string: simple conversion cannot handle multiple code points
     * - match>0 && match!=length: not all input consumed, forbidden for this function
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    ফেরত 0xfffe;
}

/*
 * continue partial match with new input
 * never called for simple, single-character conversion
 */
U_CFUNC শূন্য
ucnv_extContinueMatchToU(UConverter *cnv,
                         UConverterToUnicodeArgs *pArgs, int32_t src_সূচক,
                         UErrorCode *pErrorCode) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match, length;

    match=ucnv_extMatchToU(cnv->shared_তথ্য->mbcs.extIndexes, (int8_t)UCNV_SISO_STATE(cnv),
                           cnv->preToU, cnv->preToULength,
                           pArgs->source, (int32_t)(pArgs->sourceLimit-pArgs->source),
                           &value,
                           cnv->useFallback, pArgs->flush);
    যদি(match>0) {
        যদি(match>=cnv->preToULength) {
            /* advance src pointer for the consumed input */
            pArgs->source+=match-cnv->preToULength;
            cnv->preToULength=0;
        } অন্যথা {
            /* the match did not use all of preToU[] - keep the rest for replay */
            length=cnv->preToULength-match;
            uprv_memmove(cnv->preToU, cnv->preToU+match, length);
            cnv->preToULength=(int8_t)-length;
        }

        /* write result */
        ucnv_extWriteToU(cnv, cnv->shared_তথ্য->mbcs.extIndexes,
                         value,
                         &pArgs->target, pArgs->targetLimit,
                         &pArgs->offsets, src_সূচক,
                         pErrorCode);
    } অন্যথা যদি(match<0) {
        /* save state for partial match */
        ধ্রুবক অক্ষর *s;
        int32_t j;

        /* just _append_ the newly consumed input to preToU[] */
        s=pArgs->source;
        match=-match;
        জন্য(j=cnv->preToULength; j<match; ++j) {
            cnv->preToU[j]=*s++;
        }
        pArgs->source=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preToULength=(int8_t)match;
    } অন্যথা /* match==0 */ {
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
        uprv_memcpy(cnv->toUBytes, cnv->preToU, cnv->pre_To_UFirst_দৈর্ঘ্য);
        cnv->toULength=cnv->pre_To_UFirst_দৈর্ঘ্য;

        /* move the rest up inside the buffer */
        length=cnv->preToULength-cnv->pre_To_UFirst_দৈর্ঘ্য;
        যদি(length>0) {
            uprv_memmove(cnv->preToU, cnv->preToU+cnv->pre_To_UFirst_দৈর্ঘ্য, length);
        }

        /* mark preToU for replay */
        cnv->preToULength=(int8_t)-length;

        /* set the error code for unassigned */
        *pErrorCode=U_INVALID_CHAR_FOUND;
    }
}

/* from Unicode ------------------------------------------------------------- */

// Use roundtrips, "good one-way" mappings, and some normal fallbacks.
স্থির অন্তঃরেখ UBool
extFromUUseMapping(UBool useFallback, uint32_t value, UChar32 firstCP) {
    ফেরত
        ((value&UCNV_EXT_FROM_U_STATUS_MASK)!=0 ||
            FROM_U_USE_FALLBACK(useFallback, firstCP)) &&
        (value&UCNV_EXT_FROM_U_RESERVED_MASK)==0;
}

/*
 * @return index of the char16_t, if found; else <0
 */
স্থির অন্তঃরেখ int32_t
ucnv_extFindFromU(ধ্রুবক char16_t *fromUSection, int32_t length, char16_t u) {
    int32_t i, start, limit;

    /* binary search */
    start=0;
    limit=length;
    জন্য(;;) {
        i=limit-start;
        যদি(i<=1) {
            বিরতি; /* done */
        }
        /* start<limit-1 */

        যদি(i<=4) {
            /* linear search for the last part */
            যদি(u<=fromUSection[start]) {
                বিরতি;
            }
            যদি(++start<limit && u<=fromUSection[start]) {
                বিরতি;
            }
            যদি(++start<limit && u<=fromUSection[start]) {
                বিরতি;
            }
            /* always break at start==limit-1 */
            ++start;
            বিরতি;
        }

        i=(start+limit)/2;
        যদি(u<fromUSection[i]) {
            limit=i;
        } অন্যথা {
            start=i;
        }
    }

    /* did we really find it? */
    যদি(start<limit && u==fromUSection[start]) {
        ফেরত start;
    } অন্যথা {
        ফেরত -1; /* not found */
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
স্থির int32_t
ucnv_extMatchFromU(ধ্রুবক int32_t *cx,
                   UChar32 firstCP,
                   ধ্রুবক char16_t *pre, int32_t pre_দৈর্ঘ্য,
                   ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                   uint32_t *p_Match_মান,
                   UBool useFallback, UBool flush) {
    ধ্রুবক uint16_t *stage12, *stage3;
    ধ্রুবক uint32_t *stage3b;

    ধ্রুবক char16_t *fromUTableUChars, *fromUSectionUChars;
    ধ্রুবক uint32_t *from_UTable_মানসমূহ, *from_USection_মানসমূহ;

    uint32_t value, match_মান;
    int32_t i, j, idx, length, match_দৈর্ঘ্য;
    char16_t c;

    যদি(cx==nullptr) {
        ফেরত 0; /* no extension data, no match */
    }

    /* trie lookup of firstCP */
    idx=firstCP>>10; /* stage 1 index */
    যদি(idx>=cx[UCNV_EXT_FROM_U_STAGE_1_দৈর্ঘ্য]) {
        ফেরত 0; /* the first code point is outside the trie */
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_সূচক, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_সূচক, uint16_t);
    idx=UCNV_EXT_FROM_U(stage12, stage3, idx, firstCP);

    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_সূচক, uint32_t);
    value=stage3b[idx];
    যদি(value==0) {
        ফেরত 0;
    }

    /*
     * Tests for (value&UCNV_EXT_FROM_U_RESERVED_MASK)==0:
     * Do not interpret values with reserved bits used, for forward compatibility,
     * and do not even remember intermediate results with reserved bits used.
     */

    যদি(UCNV_EXT_TO_U_IS_PARTIAL(value)) {
        /* partial match, enter the loop below */
        idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_সূচক(value));

        /* initialize */
        fromUTableUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_সূচক, char16_t);
        from_UTable_মানসমূহ=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_মানসমূহ_সূচক, uint32_t);

        match_মান=0;
        i=j=match_দৈর্ঘ্য=0;

        /* we must not remember fallback matches when not using fallbacks */

        /* match input units until there is a full match or the input is consumed */
        জন্য(;;) {
            /* go to the next section */
            fromUSectionUChars=fromUTableUChars+idx;
            from_USection_মানসমূহ=from_UTable_মানসমূহ+idx;

            /* read first pair of the section */
            length=*fromUSectionUChars++;
            value=*from_USection_মানসমূহ++;
            যদি(value!=0 && extFromUUseMapping(useFallback, value, firstCP)) {
                /* remember longest match so far */
                match_মান=value;
                match_দৈর্ঘ্য=2+i+j;
            }

            /* match pre[] then src[] */
            যদি(i<pre_দৈর্ঘ্য) {
                c=pre[i++];
            } অন্যথা যদি(j<src_দৈর্ঘ্য) {
                c=src[j++];
            } অন্যথা {
                /* all input consumed, partial match */
                যদি(flush || (length=(i+j))>UCNV_EXT_MAX_UCHARS) {
                    /*
                     * end of the entire input stream, stop with the longest match so far
                     * or: partial match must not be longer than UCNV_EXT_MAX_UCHARS
                     * because it must fit into state buffers
                     */
                    বিরতি;
                } অন্যথা {
                    /* continue with more input next time */
                    ফেরত -(2+length);
                }
            }

            /* search for the current char16_t */
            idx=ucnv_extFindFromU(fromUSectionUChars, length, c);
            যদি(idx<0) {
                /* no match here, stop with the longest match so far */
                বিরতি;
            } অন্যথা {
                value=from_USection_মানসমূহ[idx];
                যদি(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                    /* partial match, continue */
                    idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_সূচক(value));
                } অন্যথা {
                    যদি(extFromUUseMapping(useFallback, value, firstCP)) {
                        /* full match, stop with result */
                        match_মান=value;
                        match_দৈর্ঘ্য=2+i+j;
                    } অন্যথা {
                        /* full match on fallback not taken, stop with the longest match so far */
                    }
                    বিরতি;
                }
            }
        }

        যদি(match_দৈর্ঘ্য==0) {
            /* no match at all */
            ফেরত 0;
        }
    } অন্যথা /* result from firstCP trie lookup */ {
        যদি(extFromUUseMapping(useFallback, value, firstCP)) {
            /* full match, stop with result */
            match_মান=value;
            match_দৈর্ঘ্য=2;
        } অন্যথা {
            /* fallback not taken */
            ফেরত 0;
        }
    }

    /* return result */
    যদি(match_মান==UCNV_EXT_FROM_U_SUBCHAR1) {
        ফেরত 1; /* assert matchLength==2 */
    }

    *p_Match_মান=match_মান;
    ফেরত match_দৈর্ঘ্য;
}

/*
 * @param value fromUnicode mapping table value; ignores roundtrip and reserved bits
 */
স্থির অন্তঃরেখ শূন্য
ucnv_extWriteFromU(UConverter *cnv, ধ্রুবক int32_t *cx,
                   uint32_t value,
                   অক্ষর **target, ধ্রুবক অক্ষর *targetLimit,
                   int32_t **offsets, int32_t src_সূচক,
                   UErrorCode *pErrorCode) {
    uint8_t বাফার[1+UCNV_EXT_MAX_BYTES];
    ধ্রুবক uint8_t *ফলাফল;
    int32_t length, পূর্ব_দৈর্ঘ্য;

    length=UCNV_EXT_FROM_U_GET_দৈর্ঘ্য(value);
    value = UCNV_EXT_FROM_U_GET_তথ্য(value);

    /* output the result */
    যদি(length<=UCNV_EXT_FROM_U_MAX_DIRECT_দৈর্ঘ্য) {
        /*
         * Generate a byte array and then write it below.
         * This is not the fastest possible way, but it should be ok for
         * extension mappings, and it is much simpler.
         * Offset and overflow handling are only done once this way.
         */
        uint8_t *p=বাফার+1; /* reserve buffer[0] for shiftByte below */
        নির্বাচন(length) {
        ক্ষেত্র 3:
            *p++ = static_cast<uint8_t>(value >> 16);
            U_FALLTHROUGH;
        ক্ষেত্র 2:
            *p++ = static_cast<uint8_t>(value >> 8);
            U_FALLTHROUGH;
        ক্ষেত্র 1:
            *p++ = static_cast<uint8_t>(value);
            U_FALLTHROUGH;
        পূর্বনির্ধারিত:
            বিরতি; /* will never occur */
        }
        ফলাফল=বাফার+1;
    } অন্যথা {
        ফলাফল=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_BYTES_সূচক, uint8_t)+value;
    }

    /* with correct data we have length>0 */

    যদি((পূর্ব_দৈর্ঘ্য=cnv->fromUnicodeStatus)!=0) {
        /* handle SI/SO stateful output */
        uint8_t shiftByte;

        যদি(পূর্ব_দৈর্ঘ্য>1 && length==1) {
            /* change from double-byte mode to single-byte */
            shiftByte = static_cast<uint8_t>(UCNV_SI);
            cnv->fromUnicodeStatus=1;
        } অন্যথা যদি(পূর্ব_দৈর্ঘ্য==1 && length>1) {
            /* change from single-byte mode to double-byte */
            shiftByte = static_cast<uint8_t>(UCNV_SO);
            cnv->fromUnicodeStatus=2;
        } অন্যথা {
            shiftByte=0;
        }

        যদি(shiftByte!=0) {
            /* prepend the shift byte to the result bytes */
            বাফার[0]=shiftByte;
            যদি(ফলাফল!=বাফার+1) {
                uprv_memcpy(বাফার+1, ফলাফল, length);
            }
            ফলাফল=বাফার;
            ++length;
        }
    }

    ucnv_fromUWriteBytes(cnv, reinterpret_cast<ধ্রুবক অক্ষর*>(ফলাফল), length,
                         target, targetLimit,
                         offsets, src_সূচক,
                         pErrorCode);
}

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_extInitialMatchFromU(UConverter *cnv, ধ্রুবক int32_t *cx,
                          UChar32 cp,
                          ধ্রুবক char16_t **src, ধ্রুবক char16_t *srcLimit,
                          অক্ষর **target, ধ্রুবক অক্ষর *targetLimit,
                          int32_t **offsets, int32_t src_সূচক,
                          UBool flush,
                          UErrorCode *pErrorCode) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match;

    /* try to match */
    match=ucnv_extMatchFromU(cx, cp,
                             nullptr, 0,
                             *src, (int32_t)(srcLimit-*src),
                             &value,
                             cnv->useFallback, flush);

    /* reject a match if the result is a single byte for DBCS-only */
    যদি( match>=2 &&
        !(UCNV_EXT_FROM_U_GET_দৈর্ঘ্য(value)==1 &&
          cnv->shared_তথ্য->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY)
    ) {
        /* advance src pointer for the consumed input */
        *src+=match-2; /* remove 2 for the initial code point */

        /* write result to target */
        ucnv_extWriteFromU(cnv, cx,
                           value,
                           target, targetLimit,
                           offsets, src_সূচক,
                           pErrorCode);
        ফেরত সত্য;
    } অন্যথা যদি(match<0) {
        /* save state for partial match */
        ধ্রুবক char16_t *s;
        int32_t j;

        /* copy the first code point */
        cnv->preFromUFirstCP=cp;

        /* now copy the newly consumed input */
        s=*src;
        match=-match-2; /* remove 2 for the initial code point */
        জন্য(j=0; j<match; ++j) {
            cnv->preFromU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)match;
        ফেরত সত্য;
    } অন্যথা যদি(match==1) {
        /* matched, no mapping but request for <subchar1> */
        cnv->useSubChar1=সত্য;
        ফেরত মিথ্যা;
    } অন্যথা /* match==0 no match */ {
        ফেরত মিথ্যা;
    }
}

/*
 * Used by ISO 2022 implementation.
 * @return number of bytes in *pValue; negative number if fallback; 0 for no mapping
 */
U_CFUNC int32_t
ucnv_extSimpleMatchFromU(ধ্রুবক int32_t *cx,
                         UChar32 cp, uint32_t *p_মান,
                         UBool useFallback) {
    uint32_t value;
    int32_t match;

    /* try to match */
    match=ucnv_extMatchFromU(cx,
                             cp,
                             nullptr, 0,
                             nullptr, 0,
                             &value,
                             useFallback, সত্য);
    যদি(match>=2) {
        /* write result for simple, single-character conversion */
        int32_t length;
        পূর্ণসংখ্যা isRoundtrip;

        isRoundtrip=UCNV_EXT_FROM_U_IS_ROUNDTRIP(value);
        length=UCNV_EXT_FROM_U_GET_দৈর্ঘ্য(value);
        value = UCNV_EXT_FROM_U_GET_তথ্য(value);

        যদি(length<=UCNV_EXT_FROM_U_MAX_DIRECT_দৈর্ঘ্য) {
            *p_মান=value;
            ফেরত isRoundtrip ? length : -length;
#যদি 0 /* not currently used */
        } অন্যথা যদি(length==4) {
            /* de-serialize a 4-byte result */
            ধ্রুবক uint8_t *ফলাফল=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_BYTES_সূচক, uint8_t)+value;
            *p_মান=
                ((uint32_t)ফলাফল[0]<<24)|
                ((uint32_t)ফলাফল[1]<<16)|
                ((uint32_t)ফলাফল[2]<<8)|
                ফলাফল[3];
            ফেরত isRoundtrip ? 4 : -4;
#endif
        }
    }

    /*
     * return no match because
     * - match>1 && resultLength>4: result too long for simple conversion
     * - match==1: no match found, <subchar1> preferred
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    ফেরত 0;
}

/*
 * continue partial match with new input, requires cnv->preFromUFirstCP>=0
 * never called for simple, single-character conversion
 */
U_CFUNC শূন্য
ucnv_extContinueMatchFromU(UConverter *cnv,
                           UConverterFromUnicodeArgs *pArgs, int32_t src_সূচক,
                           UErrorCode *pErrorCode) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match;

    match=ucnv_extMatchFromU(cnv->shared_তথ্য->mbcs.extIndexes,
                             cnv->preFromUFirstCP,
                             cnv->preFromU, cnv->preFromULength,
                             pArgs->source, (int32_t)(pArgs->sourceLimit-pArgs->source),
                             &value,
                             cnv->useFallback, pArgs->flush);
    যদি(match>=2) {
        match-=2; /* remove 2 for the initial code point */

        যদি(match>=cnv->preFromULength) {
            /* advance src pointer for the consumed input */
            pArgs->source+=match-cnv->preFromULength;
            cnv->preFromULength=0;
        } অন্যথা {
            /* the match did not use all of preFromU[] - keep the rest for replay */
            int32_t length=cnv->preFromULength-match;
            u_memmove(cnv->preFromU, cnv->preFromU+match, length);
            cnv->preFromULength=(int8_t)-length;
        }

        /* finish the partial match */
        cnv->preFromUFirstCP=U_SENTINEL;

        /* write result */
        ucnv_extWriteFromU(cnv, cnv->shared_তথ্য->mbcs.extIndexes,
                           value,
                           &pArgs->target, pArgs->targetLimit,
                           &pArgs->offsets, src_সূচক,
                           pErrorCode);
    } অন্যথা যদি(match<0) {
        /* save state for partial match */
        ধ্রুবক char16_t *s;
        int32_t j;

        /* just _append_ the newly consumed input to preFromU[] */
        s=pArgs->source;
        match=-match-2; /* remove 2 for the initial code point */
        জন্য(j=cnv->preFromULength; j<match; ++j) {
            U_ASSERT(j>=0);
            cnv->preFromU[j]=*s++;
        }
        pArgs->source=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)match;
    } অন্যথা /* match==0 or 1 */ {
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

        যদি(match==1) {
            /* matched, no mapping but request for <subchar1> */
            cnv->useSubChar1=সত্য;
        }

        /* move the first code point to the error field */
        cnv->fromUChar32=cnv->preFromUFirstCP;
        cnv->preFromUFirstCP=U_SENTINEL;

        /* mark preFromU for replay */
        cnv->preFromULength=-cnv->preFromULength;

        /* set the error code for unassigned */
        *pErrorCode=U_INVALID_CHAR_FOUND;
    }
}

স্থির UBool
extSetUseMapping(UConverterUnicodeSet which, int32_t min_দৈর্ঘ্য, uint32_t value) {
    যদি(which==UCNV_ROUNDTRIP_SET) {
        // Add only code points for which the roundtrip flag is set.
        // Do not add any fallbacks, even if ucnv_fromUnicode() would use them
        // (fallbacks from PUA). See the API docs for ucnv_getUnicodeSet().
        //
        // By analogy, also do not add "good one-way" mappings.
        //
        // Do not add entries with reserved bits set.
        যদি(((value&(UCNV_EXT_FROM_U_ROUNDTRIP_FLAG|UCNV_EXT_FROM_U_RESERVED_MASK))!=
                UCNV_EXT_FROM_U_ROUNDTRIP_FLAG)) {
            ফেরত মিথ্যা;
        }
    } অন্যথা /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
        // Do not add entries with reserved bits set.
        যদি((value&UCNV_EXT_FROM_U_RESERVED_MASK)!=0) {
            ফেরত মিথ্যা;
        }
    }
    // Do not add <subchar1> entries or other (future?) pseudo-entries
    // with an output length of 0.
    ফেরত UCNV_EXT_FROM_U_GET_দৈর্ঘ্য(value)>=min_দৈর্ঘ্য;
}

স্থির শূন্য
ucnv_extGetUnicodeSetString(ধ্রুবক UConverter_Shared_তথ্য *shared_তথ্য,
                            ধ্রুবক int32_t *cx,
                            ধ্রুবক USetAdder *sa,
                            UConverterUnicodeSet which,
                            int32_t min_দৈর্ঘ্য,
                            UChar32 firstCP,
                            char16_t s[UCNV_EXT_MAX_UCHARS], int32_t length,
                            int32_t section_সূচক,
                            UErrorCode *pErrorCode) {
    ধ্রুবক char16_t *fromUSectionUChars;
    ধ্রুবক uint32_t *from_USection_মানসমূহ;

    uint32_t value;
    int32_t i, count;

    fromUSectionUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_সূচক, char16_t)+section_সূচক;
    from_USection_মানসমূহ=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_মানসমূহ_সূচক, uint32_t)+section_সূচক;

    /* read first pair of the section */
    count=*fromUSectionUChars++;
    value=*from_USection_মানসমূহ++;

    যদি(extSetUseMapping(which, min_দৈর্ঘ্য, value)) {
        যদি(length==U16_দৈর্ঘ্য(firstCP)) {
            /* add the initial code point */
            sa->add(sa->set, firstCP);
        } অন্যথা {
            /* add the string so far */
            sa->addString(sa->set, s, length);
        }
    }

    জন্য(i=0; i<count; ++i) {
        /* append this code unit and recurse or add the string */
        s[length]=fromUSectionUChars[i];
        value=from_USection_মানসমূহ[i];

        যদি(value==0) {
            /* no mapping, do nothing */
        } অন্যথা যদি(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
            ucnv_extGetUnicodeSetString(
                shared_তথ্য, cx, sa, which, min_দৈর্ঘ্য,
                firstCP, s, length+1,
                static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_সূচক(value)),
                pErrorCode);
        } অন্যথা যদি(extSetUseMapping(which, min_দৈর্ঘ্য, value)) {
            sa->addString(sa->set, s, length+1);
        }
    }
}

U_CFUNC শূন্য
ucnv_extGetUnicodeSet(ধ্রুবক UConverter_Shared_তথ্য *shared_তথ্য,
                      ধ্রুবক USetAdder *sa,
                      UConverterUnicodeSet which,
                      UConverterSetFilter filter,
                      UErrorCode *pErrorCode) {
    ধ্রুবক int32_t *cx;
    ধ্রুবক uint16_t *stage12, *stage3, *ps2, *ps3;
    ধ্রুবক uint32_t *stage3b;

    uint32_t value;
    int32_t st1, stage1_দৈর্ঘ্য, st2, st3, min_দৈর্ঘ্য;

    char16_t s[UCNV_EXT_MAX_UCHARS];
    UChar32 c;
    int32_t length;

    cx=shared_তথ্য->mbcs.extIndexes;
    যদি(cx==nullptr) {
        ফেরত;
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_সূচক, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_সূচক, uint16_t);
    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_সূচক, uint32_t);

    stage1_দৈর্ঘ্য=cx[UCNV_EXT_FROM_U_STAGE_1_দৈর্ঘ্য];

    /* enumerate the from-Unicode trie table */
    c=0; /* keep track of the current code point while enumerating */

    যদি(filter==UCNV_SET_FILTER_2022_CN) {
        min_দৈর্ঘ্য=3;
    } অন্যথা যদি( shared_তথ্য->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ||
               filter!=UCNV_SET_FILTER_NONE
    ) {
        /* DBCS-only, ignore single-byte results */
        min_দৈর্ঘ্য=2;
    } অন্যথা {
        min_দৈর্ঘ্য=1;
    }

    /*
     * the trie enumeration is almost the same as
     * in MBCSGetUnicodeSet() for MBCS_OUTPUT_1
     */
    জন্য(st1=0; st1<stage1_দৈর্ঘ্য; ++st1) {
        st2=stage12[st1];
        যদি(st2>stage1_দৈর্ঘ্য) {
            ps2=stage12+st2;
            জন্য(st2=0; st2<64; ++st2) {
                যদি((st3=(int32_t)ps2[st2]<<UCNV_EXT_STAGE_2_বাম_SHIFT)!=0) {
                    /* read the stage 3 block */
                    ps3=stage3+st3;

                    করো {
                        value=stage3b[*ps3++];
                        যদি(value==0) {
                            /* no mapping, do nothing */
                        } অন্যথা যদি(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                            // Recurse for partial results.
                            length=0;
                            U16_APPEND_UNSAFE(s, length, c);
                            ucnv_extGetUnicodeSetString(
                                shared_তথ্য, cx, sa, which, min_দৈর্ঘ্য,
                                c, s, length,
                                (int32_t)UCNV_EXT_FROM_U_GET_PARTIAL_সূচক(value),
                                pErrorCode);
                        } অন্যথা যদি(extSetUseMapping(which, min_দৈর্ঘ্য, value)) {
                            নির্বাচন(filter) {
                            ক্ষেত্র UCNV_SET_FILTER_2022_CN:
                                যদি(!(UCNV_EXT_FROM_U_GET_দৈর্ঘ্য(value)==3 && UCNV_EXT_FROM_U_GET_তথ্য(value)<=0x82ffff)) {
                                    চালু;
                                }
                                বিরতি;
                            ক্ষেত্র UCNV_SET_FILTER_SJIS:
                                যদি(!(UCNV_EXT_FROM_U_GET_দৈর্ঘ্য(value)==2 && (value=UCNV_EXT_FROM_U_GET_তথ্য(value))>=0x8140 && value<=0xeffc)) {
                                    চালু;
                                }
                                বিরতি;
                            ক্ষেত্র UCNV_SET_FILTER_GR94DBCS:
                                যদি(!(UCNV_EXT_FROM_U_GET_দৈর্ঘ্য(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_তথ্য(value))-0xa1a1)<=(0xfefe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    চালু;
                                }
                                বিরতি;
                            ক্ষেত্র UCNV_SET_FILTER_HZ:
                                যদি(!(UCNV_EXT_FROM_U_GET_দৈর্ঘ্য(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_তথ্য(value))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    চালু;
                                }
                                বিরতি;
                            পূর্বনির্ধারিত:
                                /*
                                 * UCNV_SET_FILTER_NONE,
                                 * or UCNV_SET_FILTER_DBCS_ONLY which is handled via minLength
                                 */
                                বিরতি;
                            }
                            sa->add(sa->set, c);
                        }
                    } যখন((++c&0xf)!=0);
                } অন্যথা {
                    c+=16; /* empty stage 3 block */
                }
            }
        } অন্যথা {
            c+=1024; /* empty stage 2 block */
        }
    }
}

#endif /* #if !UCONFIG_NO_LEGACY_CONVERSION */
