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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#ଅନ୍ତର୍ଭାଵ "unicode/uset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_bld.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_cnv.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_ext.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

/* to Unicode --------------------------------------------------------------- */

/*
 * @return lookup value for the byte, if found; else 0
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint32_t
ucnv_extFindToU(ଅଚଳ uint32_t *toUSection, int32_t length, uint8_t byte) {
    uint32_t word0, word;
    int32_t i, start, limit;

    /* check the input byte against the lowest and highest section bytes */
    start = static_cast<int32_t>(UCNV_EXT_TO_U_GET_BYTE(toUSection[0]));
    limit = static_cast<int32_t>(UCNV_EXT_TO_U_GET_BYTE(toUSection[length - 1]));
    ଯଦି(byte<start || limit<byte) {
        ଫେରନ୍ତୁ 0; /* the byte is out of range */
    }

    ଯଦି(length==((limit-start)+1)) {
        /* direct access on a linear array */
        ଫେରନ୍ତୁ UCNV_EXT_TO_U_GET_ମୂଲ୍ୟ(toUSection[byte-start]); /* could be 0 */
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
    word=word0|UCNV_EXT_TO_U_ମୂଲ୍ୟ_MASK;

    /* binary search */
    start=0;
    limit=length;
    ପାଇଁ(;;) {
        i=limit-start;
        ଯଦି(i<=1) {
            ଭାଙ୍ଗ; /* done */
        }
        /* start<limit-1 */

        ଯଦି(i<=4) {
            /* linear search for the last part */
            ଯଦି(word0<=toUSection[start]) {
                ଭାଙ୍ଗ;
            }
            ଯଦି(++start<limit && word0<=toUSection[start]) {
                ଭାଙ୍ଗ;
            }
            ଯଦି(++start<limit && word0<=toUSection[start]) {
                ଭାଙ୍ଗ;
            }
            /* always break at start==limit-1 */
            ++start;
            ଭାଙ୍ଗ;
        }

        i=(start+limit)/2;
        ଯଦି(word<toUSection[i]) {
            limit=i;
        } ଅନ୍ୟଥା {
            start=i;
        }
    }

    /* did we really find it? */
    ଯଦି(start<limit && byte==UCNV_EXT_TO_U_GET_BYTE(word=toUSection[start])) {
        ଫେରନ୍ତୁ UCNV_EXT_TO_U_GET_ମୂଲ୍ୟ(word); /* never 0 */
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ 0; /* not found */
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
ସ୍ଥିର int32_t
ucnv_extMatchToU(ଅଚଳ int32_t *cx, int8_t sisoState,
                 ଅଚଳ ଅକ୍ଷର *pre, int32_t preLength,
                 ଅଚଳ ଅକ୍ଷର *src, int32_t srcLength,
                 uint32_t *p_Match_ମୂଲ୍ୟ,
                 UBool /*useFallback*/, UBool flush) {
    ଅଚଳ uint32_t *toUTable, *toUSection;

    uint32_t value, match_ମୂଲ୍ୟ;
    int32_t i, j, idx, length, matchLength;
    uint8_t b;

    ଯଦି(cx==nullptr || cx[UCNV_EXT_TO_U_LENGTH]<=0) {
        ଫେରନ୍ତୁ 0; /* no extension data, no match */
    }

    /* initialize */
    toUTable=UCNV_EXT_ARRAY(cx, UCNV_EXT_TO_U_INDEX, uint32_t);
    idx=0;

    match_ମୂଲ୍ୟ=0;
    i=j=matchLength=0;

    ଯଦି(sisoState==0) {
        /* SBCS state of an SI/SO stateful converter, look at only exactly 1 byte */
        ଯଦି(preLength>1) {
            ଫେରନ୍ତୁ 0; /* no match of a DBCS sequence in SBCS mode */
        } ଅନ୍ୟଥା ଯଦି(preLength==1) {
            srcLength=0;
        } ଅନ୍ୟଥା /* preLength==0 */ {
            ଯଦି(srcLength>1) {
                srcLength=1;
            }
        }
        flush=ସତ୍ୟ;
    }

    /* we must not remember fallback matches when not using fallbacks */

    /* match input units until there is a full match or the input is consumed */
    ପାଇଁ(;;) {
        /* go to the next section */
        toUSection=toUTable+idx;

        /* read first pair of the section */
        value=*toUSection++;
        length=UCNV_EXT_TO_U_GET_BYTE(value);
        value=UCNV_EXT_TO_U_GET_ମୂଲ୍ୟ(value);
        ଯଦି( value!=0 &&
            (UCNV_EXT_TO_U_IS_ROUNDTRIP(value) ||
             TO_U_USE_FALLBACK(useFallback)) &&
            UCNV_EXT_TO_U_VERIFY_SISO_MATCH(sisoState, i+j)
        ) {
            /* remember longest match so far */
            match_ମୂଲ୍ୟ=value;
            matchLength=i+j;
        }

        /* match pre[] then src[] */
        ଯଦି(i<preLength) {
            b = static_cast<uint8_t>(pre[i++]);
        } ଅନ୍ୟଥା ଯଦି(j<srcLength) {
            b = static_cast<uint8_t>(src[j++]);
        } ଅନ୍ୟଥା {
            /* all input consumed, partial match */
            ଯଦି(flush || (length=(i+j))>UCNV_EXT_MAX_BYTES) {
                /*
                 * end of the entire input stream, stop with the longest match so far
                 * or: partial match must not be longer than UCNV_EXT_MAX_BYTES
                 * because it must fit into state buffers
                 */
                ଭାଙ୍ଗ;
            } ଅନ୍ୟଥା {
                /* continue with more input next time */
                ଫେରନ୍ତୁ -length;
            }
        }

        /* search for the current char16_t */
        value=ucnv_extFindToU(toUSection, length, b);
        ଯଦି(value==0) {
            /* no match here, stop with the longest match so far */
            ଭାଙ୍ଗ;
        } ଅନ୍ୟଥା {
            ଯଦି(UCNV_EXT_TO_U_IS_PARTIAL(value)) {
                /* partial match, continue */
                idx = static_cast<int32_t>(UCNV_EXT_TO_U_GET_PARTIAL_INDEX(value));
            } ଅନ୍ୟଥା {
                ଯଦି( (UCNV_EXT_TO_U_IS_ROUNDTRIP(value) ||
                     TO_U_USE_FALLBACK(useFallback)) &&
                    UCNV_EXT_TO_U_VERIFY_SISO_MATCH(sisoState, i+j)
                ) {
                    /* full match, stop with result */
                    match_ମୂଲ୍ୟ=value;
                    matchLength=i+j;
                } ଅନ୍ୟଥା {
                    /* full match on fallback not taken, stop with the longest match so far */
                }
                ଭାଙ୍ଗ;
            }
        }
    }

    ଯଦି(matchLength==0) {
        /* no match at all */
        ଫେରନ୍ତୁ 0;
    }

    /* return result */
    *p_Match_ମୂଲ୍ୟ=UCNV_EXT_TO_U_MASK_ROUNDTRIP(match_ମୂଲ୍ୟ);
    ଫେରନ୍ତୁ matchLength;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
ucnv_extWriteToU(UConverter *cnv, ଅଚଳ int32_t *cx,
                 uint32_t value,
                 char16_t **target, ଅଚଳ char16_t *targetLimit,
                 int32_t **offsets, int32_t srcIndex,
                 UErrorCode *pErrorCode) {
    /* output the result */
    ଯଦି(UCNV_EXT_TO_U_IS_CODE_POINT(value)) {
        /* output a single code point */
        ucnv_toUWriteCodePoint(
            cnv, UCNV_EXT_TO_U_GET_CODE_POINT(value),
            target, targetLimit,
            offsets, srcIndex,
            pErrorCode);
    } ଅନ୍ୟଥା {
        /* output a string - with correct data we have resultLength>0 */
        ucnv_toUWriteUChars(
            cnv,
            UCNV_EXT_ARRAY(cx, UCNV_EXT_TO_U_UCHARS_INDEX, char16_t)+
                UCNV_EXT_TO_U_GET_INDEX(value),
            UCNV_EXT_TO_U_GET_LENGTH(value),
            target, targetLimit,
            offsets, srcIndex,
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
    ((cnv)->sharedData->mbcs.outputType==MBCS_OUTPUT_2_SISO ? (int8_t)(cnv)->mode : \
     (cnv)->sharedData->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ? 1 : -1)

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_extInitialMatchToU(UConverter *cnv, ଅଚଳ int32_t *cx,
                        int32_t firstLength,
                        ଅଚଳ ଅକ୍ଷର **src, ଅଚଳ ଅକ୍ଷର *srcLimit,
                        char16_t **target, ଅଚଳ char16_t *targetLimit,
                        int32_t **offsets, int32_t srcIndex,
                        UBool flush,
                        UErrorCode *pErrorCode) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match;

    /* try to match */
    match=ucnv_extMatchToU(cx, (int8_t)UCNV_SISO_STATE(cnv),
                           (ଅଚଳ ଅକ୍ଷର *)cnv->toUBytes, firstLength,
                           *src, (int32_t)(srcLimit-*src),
                           &value,
                           cnv->useFallback, flush);
    ଯଦି(match>0) {
        /* advance src pointer for the consumed input */
        *src+=match-firstLength;

        /* write result to target */
        ucnv_extWriteToU(cnv, cx,
                         value,
                         target, targetLimit,
                         offsets, srcIndex,
                         pErrorCode);
        ଫେରନ୍ତୁ ସତ୍ୟ;
    } ଅନ୍ୟଥା ଯଦି(match<0) {
        /* save state for partial match */
        ଅଚଳ ଅକ୍ଷର *s;
        int32_t j;

        /* copy the first code point */
        s=(ଅଚଳ ଅକ୍ଷର *)cnv->toUBytes;
        cnv->preToUFirstLength=(int8_t)firstLength;
        ପାଇଁ(j=0; j<firstLength; ++j) {
            cnv->preToU[j]=*s++;
        }

        /* now copy the newly consumed input */
        s=*src;
        match=-match;
        ପାଇଁ(; j<match; ++j) {
            cnv->preToU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preToULength=(int8_t)match;
        ଫେରନ୍ତୁ ସତ୍ୟ;
    } ଅନ୍ୟଥା /* match==0 no match */ {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
}

U_CFUNC UChar32
ucnv_extSimpleMatchToU(ଅଚଳ int32_t *cx,
                       ଅଚଳ ଅକ୍ଷର *source, int32_t length,
                       UBool useFallback) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match;

    ଯଦି(length<=0) {
        ଫେରନ୍ତୁ 0xffff;
    }

    /* try to match */
    match=ucnv_extMatchToU(cx, -1,
                           source, length,
                           nullptr, 0,
                           &value,
                           useFallback, ସତ୍ୟ);
    ଯଦି(match==length) {
        /* write result for simple, single-character conversion */
        ଯଦି(UCNV_EXT_TO_U_IS_CODE_POINT(value)) {
            ଫେରନ୍ତୁ UCNV_EXT_TO_U_GET_CODE_POINT(value);
        }
    }

    /*
     * return no match because
     * - match>0 && value points to string: simple conversion cannot handle multiple code points
     * - match>0 && match!=length: not all input consumed, forbidden for this function
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    ଫେରନ୍ତୁ 0xfffe;
}

/*
 * continue partial match with new input
 * never called for simple, single-character conversion
 */
U_CFUNC ଶୂନ୍ୟ
ucnv_extContinueMatchToU(UConverter *cnv,
                         UConverterToUnicodeArgs *pArgs, int32_t srcIndex,
                         UErrorCode *pErrorCode) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match, length;

    match=ucnv_extMatchToU(cnv->sharedData->mbcs.extIndexes, (int8_t)UCNV_SISO_STATE(cnv),
                           cnv->preToU, cnv->preToULength,
                           pArgs->source, (int32_t)(pArgs->sourceLimit-pArgs->source),
                           &value,
                           cnv->useFallback, pArgs->flush);
    ଯଦି(match>0) {
        ଯଦି(match>=cnv->preToULength) {
            /* advance src pointer for the consumed input */
            pArgs->source+=match-cnv->preToULength;
            cnv->preToULength=0;
        } ଅନ୍ୟଥା {
            /* the match did not use all of preToU[] - keep the rest for replay */
            length=cnv->preToULength-match;
            uprv_memmove(cnv->preToU, cnv->preToU+match, length);
            cnv->preToULength=(int8_t)-length;
        }

        /* write result */
        ucnv_extWriteToU(cnv, cnv->sharedData->mbcs.extIndexes,
                         value,
                         &pArgs->target, pArgs->targetLimit,
                         &pArgs->offsets, srcIndex,
                         pErrorCode);
    } ଅନ୍ୟଥା ଯଦି(match<0) {
        /* save state for partial match */
        ଅଚଳ ଅକ୍ଷର *s;
        int32_t j;

        /* just _append_ the newly consumed input to preToU[] */
        s=pArgs->source;
        match=-match;
        ପାଇଁ(j=cnv->preToULength; j<match; ++j) {
            cnv->preToU[j]=*s++;
        }
        pArgs->source=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preToULength=(int8_t)match;
    } ଅନ୍ୟଥା /* match==0 */ {
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
        uprv_memcpy(cnv->toUBytes, cnv->preToU, cnv->preToUFirstLength);
        cnv->toULength=cnv->preToUFirstLength;

        /* move the rest up inside the buffer */
        length=cnv->preToULength-cnv->preToUFirstLength;
        ଯଦି(length>0) {
            uprv_memmove(cnv->preToU, cnv->preToU+cnv->preToUFirstLength, length);
        }

        /* mark preToU for replay */
        cnv->preToULength=(int8_t)-length;

        /* set the error code for unassigned */
        *pErrorCode=U_INVALID_CHAR_FOUND;
    }
}

/* from Unicode ------------------------------------------------------------- */

// Use roundtrips, "good one-way" mappings, and some normal fallbacks.
ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool
extFromUUseMapping(UBool useFallback, uint32_t value, UChar32 firstCP) {
    ଫେରନ୍ତୁ
        ((value&UCNV_EXT_FROM_U_STATUS_MASK)!=0 ||
            FROM_U_USE_FALLBACK(useFallback, firstCP)) &&
        (value&UCNV_EXT_FROM_U_RESERVED_MASK)==0;
}

/*
 * @return index of the char16_t, if found; else <0
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ int32_t
ucnv_extFindFromU(ଅଚଳ char16_t *fromUSection, int32_t length, char16_t u) {
    int32_t i, start, limit;

    /* binary search */
    start=0;
    limit=length;
    ପାଇଁ(;;) {
        i=limit-start;
        ଯଦି(i<=1) {
            ଭାଙ୍ଗ; /* done */
        }
        /* start<limit-1 */

        ଯଦି(i<=4) {
            /* linear search for the last part */
            ଯଦି(u<=fromUSection[start]) {
                ଭାଙ୍ଗ;
            }
            ଯଦି(++start<limit && u<=fromUSection[start]) {
                ଭାଙ୍ଗ;
            }
            ଯଦି(++start<limit && u<=fromUSection[start]) {
                ଭାଙ୍ଗ;
            }
            /* always break at start==limit-1 */
            ++start;
            ଭାଙ୍ଗ;
        }

        i=(start+limit)/2;
        ଯଦି(u<fromUSection[i]) {
            limit=i;
        } ଅନ୍ୟଥା {
            start=i;
        }
    }

    /* did we really find it? */
    ଯଦି(start<limit && u==fromUSection[start]) {
        ଫେରନ୍ତୁ start;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ -1; /* not found */
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
ସ୍ଥିର int32_t
ucnv_extMatchFromU(ଅଚଳ int32_t *cx,
                   UChar32 firstCP,
                   ଅଚଳ char16_t *pre, int32_t preLength,
                   ଅଚଳ char16_t *src, int32_t srcLength,
                   uint32_t *p_Match_ମୂଲ୍ୟ,
                   UBool useFallback, UBool flush) {
    ଅଚଳ uint16_t *stage12, *stage3;
    ଅଚଳ uint32_t *stage3b;

    ଅଚଳ char16_t *fromUTableUChars, *fromUSectionUChars;
    ଅଚଳ uint32_t *from_UTable_ମୂଲ୍ୟଗୁଡ଼ିକ, *from_USection_ମୂଲ୍ୟଗୁଡ଼ିକ;

    uint32_t value, match_ମୂଲ୍ୟ;
    int32_t i, j, idx, length, matchLength;
    char16_t c;

    ଯଦି(cx==nullptr) {
        ଫେରନ୍ତୁ 0; /* no extension data, no match */
    }

    /* trie lookup of firstCP */
    idx=firstCP>>10; /* stage 1 index */
    ଯଦି(idx>=cx[UCNV_EXT_FROM_U_STAGE_1_LENGTH]) {
        ଫେରନ୍ତୁ 0; /* the first code point is outside the trie */
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_INDEX, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_INDEX, uint16_t);
    idx=UCNV_EXT_FROM_U(stage12, stage3, idx, firstCP);

    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3B_INDEX, uint32_t);
    value=stage3b[idx];
    ଯଦି(value==0) {
        ଫେରନ୍ତୁ 0;
    }

    /*
     * Tests for (value&UCNV_EXT_FROM_U_RESERVED_MASK)==0:
     * Do not interpret values with reserved bits used, for forward compatibility,
     * and do not even remember intermediate results with reserved bits used.
     */

    ଯଦି(UCNV_EXT_TO_U_IS_PARTIAL(value)) {
        /* partial match, enter the loop below */
        idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_INDEX(value));

        /* initialize */
        fromUTableUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_INDEX, char16_t);
        from_UTable_ମୂଲ୍ୟଗୁଡ଼ିକ=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_ମୂଲ୍ୟଗୁଡ଼ିକ_INDEX, uint32_t);

        match_ମୂଲ୍ୟ=0;
        i=j=matchLength=0;

        /* we must not remember fallback matches when not using fallbacks */

        /* match input units until there is a full match or the input is consumed */
        ପାଇଁ(;;) {
            /* go to the next section */
            fromUSectionUChars=fromUTableUChars+idx;
            from_USection_ମୂଲ୍ୟଗୁଡ଼ିକ=from_UTable_ମୂଲ୍ୟଗୁଡ଼ିକ+idx;

            /* read first pair of the section */
            length=*fromUSectionUChars++;
            value=*from_USection_ମୂଲ୍ୟଗୁଡ଼ିକ++;
            ଯଦି(value!=0 && extFromUUseMapping(useFallback, value, firstCP)) {
                /* remember longest match so far */
                match_ମୂଲ୍ୟ=value;
                matchLength=2+i+j;
            }

            /* match pre[] then src[] */
            ଯଦି(i<preLength) {
                c=pre[i++];
            } ଅନ୍ୟଥା ଯଦି(j<srcLength) {
                c=src[j++];
            } ଅନ୍ୟଥା {
                /* all input consumed, partial match */
                ଯଦି(flush || (length=(i+j))>UCNV_EXT_MAX_UCHARS) {
                    /*
                     * end of the entire input stream, stop with the longest match so far
                     * or: partial match must not be longer than UCNV_EXT_MAX_UCHARS
                     * because it must fit into state buffers
                     */
                    ଭାଙ୍ଗ;
                } ଅନ୍ୟଥା {
                    /* continue with more input next time */
                    ଫେରନ୍ତୁ -(2+length);
                }
            }

            /* search for the current char16_t */
            idx=ucnv_extFindFromU(fromUSectionUChars, length, c);
            ଯଦି(idx<0) {
                /* no match here, stop with the longest match so far */
                ଭାଙ୍ଗ;
            } ଅନ୍ୟଥା {
                value=from_USection_ମୂଲ୍ୟଗୁଡ଼ିକ[idx];
                ଯଦି(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                    /* partial match, continue */
                    idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_INDEX(value));
                } ଅନ୍ୟଥା {
                    ଯଦି(extFromUUseMapping(useFallback, value, firstCP)) {
                        /* full match, stop with result */
                        match_ମୂଲ୍ୟ=value;
                        matchLength=2+i+j;
                    } ଅନ୍ୟଥା {
                        /* full match on fallback not taken, stop with the longest match so far */
                    }
                    ଭାଙ୍ଗ;
                }
            }
        }

        ଯଦି(matchLength==0) {
            /* no match at all */
            ଫେରନ୍ତୁ 0;
        }
    } ଅନ୍ୟଥା /* result from firstCP trie lookup */ {
        ଯଦି(extFromUUseMapping(useFallback, value, firstCP)) {
            /* full match, stop with result */
            match_ମୂଲ୍ୟ=value;
            matchLength=2;
        } ଅନ୍ୟଥା {
            /* fallback not taken */
            ଫେରନ୍ତୁ 0;
        }
    }

    /* return result */
    ଯଦି(match_ମୂଲ୍ୟ==UCNV_EXT_FROM_U_SUBCHAR1) {
        ଫେରନ୍ତୁ 1; /* assert matchLength==2 */
    }

    *p_Match_ମୂଲ୍ୟ=match_ମୂଲ୍ୟ;
    ଫେରନ୍ତୁ matchLength;
}

/*
 * @param value fromUnicode mapping table value; ignores roundtrip and reserved bits
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
ucnv_extWriteFromU(UConverter *cnv, ଅଚଳ int32_t *cx,
                   uint32_t value,
                   ଅକ୍ଷର **target, ଅଚଳ ଅକ୍ଷର *targetLimit,
                   int32_t **offsets, int32_t srcIndex,
                   UErrorCode *pErrorCode) {
    uint8_t buffer[1+UCNV_EXT_MAX_BYTES];
    ଅଚଳ uint8_t *result;
    int32_t length, prevLength;

    length=UCNV_EXT_FROM_U_GET_LENGTH(value);
    value = UCNV_EXT_FROM_U_GET_DATA(value);

    /* output the result */
    ଯଦି(length<=UCNV_EXT_FROM_U_MAX_DIRECT_LENGTH) {
        /*
         * Generate a byte array and then write it below.
         * This is not the fastest possible way, but it should be ok for
         * extension mappings, and it is much simpler.
         * Offset and overflow handling are only done once this way.
         */
        uint8_t *p=buffer+1; /* reserve buffer[0] for shiftByte below */
        ବାଛ(length) {
        ସ୍ଥିତି 3:
            *p++ = static_cast<uint8_t>(value >> 16);
            U_FALLTHROUGH;
        ସ୍ଥିତି 2:
            *p++ = static_cast<uint8_t>(value >> 8);
            U_FALLTHROUGH;
        ସ୍ଥିତି 1:
            *p++ = static_cast<uint8_t>(value);
            U_FALLTHROUGH;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଭାଙ୍ଗ; /* will never occur */
        }
        result=buffer+1;
    } ଅନ୍ୟଥା {
        result=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_BYTES_INDEX, uint8_t)+value;
    }

    /* with correct data we have length>0 */

    ଯଦି((prevLength=cnv->fromUnicodeStatus)!=0) {
        /* handle SI/SO stateful output */
        uint8_t shiftByte;

        ଯଦି(prevLength>1 && length==1) {
            /* change from double-byte mode to single-byte */
            shiftByte = static_cast<uint8_t>(UCNV_SI);
            cnv->fromUnicodeStatus=1;
        } ଅନ୍ୟଥା ଯଦି(prevLength==1 && length>1) {
            /* change from single-byte mode to double-byte */
            shiftByte = static_cast<uint8_t>(UCNV_SO);
            cnv->fromUnicodeStatus=2;
        } ଅନ୍ୟଥା {
            shiftByte=0;
        }

        ଯଦି(shiftByte!=0) {
            /* prepend the shift byte to the result bytes */
            buffer[0]=shiftByte;
            ଯଦି(result!=buffer+1) {
                uprv_memcpy(buffer+1, result, length);
            }
            result=buffer;
            ++length;
        }
    }

    ucnv_fromUWriteBytes(cnv, reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(result), length,
                         target, targetLimit,
                         offsets, srcIndex,
                         pErrorCode);
}

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_extInitialMatchFromU(UConverter *cnv, ଅଚଳ int32_t *cx,
                          UChar32 cp,
                          ଅଚଳ char16_t **src, ଅଚଳ char16_t *srcLimit,
                          ଅକ୍ଷର **target, ଅଚଳ ଅକ୍ଷର *targetLimit,
                          int32_t **offsets, int32_t srcIndex,
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
    ଯଦି( match>=2 &&
        !(UCNV_EXT_FROM_U_GET_LENGTH(value)==1 &&
          cnv->sharedData->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY)
    ) {
        /* advance src pointer for the consumed input */
        *src+=match-2; /* remove 2 for the initial code point */

        /* write result to target */
        ucnv_extWriteFromU(cnv, cx,
                           value,
                           target, targetLimit,
                           offsets, srcIndex,
                           pErrorCode);
        ଫେରନ୍ତୁ ସତ୍ୟ;
    } ଅନ୍ୟଥା ଯଦି(match<0) {
        /* save state for partial match */
        ଅଚଳ char16_t *s;
        int32_t j;

        /* copy the first code point */
        cnv->preFromUFirstCP=cp;

        /* now copy the newly consumed input */
        s=*src;
        match=-match-2; /* remove 2 for the initial code point */
        ପାଇଁ(j=0; j<match; ++j) {
            cnv->preFromU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)match;
        ଫେରନ୍ତୁ ସତ୍ୟ;
    } ଅନ୍ୟଥା ଯଦି(match==1) {
        /* matched, no mapping but request for <subchar1> */
        cnv->useSubChar1=ସତ୍ୟ;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    } ଅନ୍ୟଥା /* match==0 no match */ {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
}

/*
 * Used by ISO 2022 implementation.
 * @return number of bytes in *pValue; negative number if fallback; 0 for no mapping
 */
U_CFUNC int32_t
ucnv_extSimpleMatchFromU(ଅଚଳ int32_t *cx,
                         UChar32 cp, uint32_t *p_ମୂଲ୍ୟ,
                         UBool useFallback) {
    uint32_t value;
    int32_t match;

    /* try to match */
    match=ucnv_extMatchFromU(cx,
                             cp,
                             nullptr, 0,
                             nullptr, 0,
                             &value,
                             useFallback, ସତ୍ୟ);
    ଯଦି(match>=2) {
        /* write result for simple, single-character conversion */
        int32_t length;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା isRoundtrip;

        isRoundtrip=UCNV_EXT_FROM_U_IS_ROUNDTRIP(value);
        length=UCNV_EXT_FROM_U_GET_LENGTH(value);
        value = UCNV_EXT_FROM_U_GET_DATA(value);

        ଯଦି(length<=UCNV_EXT_FROM_U_MAX_DIRECT_LENGTH) {
            *p_ମୂଲ୍ୟ=value;
            ଫେରନ୍ତୁ isRoundtrip ? length : -length;
#ଯଦି 0 /* not currently used */
        } ଅନ୍ୟଥା ଯଦି(length==4) {
            /* de-serialize a 4-byte result */
            ଅଚଳ uint8_t *result=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_BYTES_INDEX, uint8_t)+value;
            *p_ମୂଲ୍ୟ=
                ((uint32_t)result[0]<<24)|
                ((uint32_t)result[1]<<16)|
                ((uint32_t)result[2]<<8)|
                result[3];
            ଫେରନ୍ତୁ isRoundtrip ? 4 : -4;
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
    ଫେରନ୍ତୁ 0;
}

/*
 * continue partial match with new input, requires cnv->preFromUFirstCP>=0
 * never called for simple, single-character conversion
 */
U_CFUNC ଶୂନ୍ୟ
ucnv_extContinueMatchFromU(UConverter *cnv,
                           UConverterFromUnicodeArgs *pArgs, int32_t srcIndex,
                           UErrorCode *pErrorCode) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match;

    match=ucnv_extMatchFromU(cnv->sharedData->mbcs.extIndexes,
                             cnv->preFromUFirstCP,
                             cnv->preFromU, cnv->preFromULength,
                             pArgs->source, (int32_t)(pArgs->sourceLimit-pArgs->source),
                             &value,
                             cnv->useFallback, pArgs->flush);
    ଯଦି(match>=2) {
        match-=2; /* remove 2 for the initial code point */

        ଯଦି(match>=cnv->preFromULength) {
            /* advance src pointer for the consumed input */
            pArgs->source+=match-cnv->preFromULength;
            cnv->preFromULength=0;
        } ଅନ୍ୟଥା {
            /* the match did not use all of preFromU[] - keep the rest for replay */
            int32_t length=cnv->preFromULength-match;
            u_memmove(cnv->preFromU, cnv->preFromU+match, length);
            cnv->preFromULength=(int8_t)-length;
        }

        /* finish the partial match */
        cnv->preFromUFirstCP=U_SENTINEL;

        /* write result */
        ucnv_extWriteFromU(cnv, cnv->sharedData->mbcs.extIndexes,
                           value,
                           &pArgs->target, pArgs->targetLimit,
                           &pArgs->offsets, srcIndex,
                           pErrorCode);
    } ଅନ୍ୟଥା ଯଦି(match<0) {
        /* save state for partial match */
        ଅଚଳ char16_t *s;
        int32_t j;

        /* just _append_ the newly consumed input to preFromU[] */
        s=pArgs->source;
        match=-match-2; /* remove 2 for the initial code point */
        ପାଇଁ(j=cnv->preFromULength; j<match; ++j) {
            U_ASSERT(j>=0);
            cnv->preFromU[j]=*s++;
        }
        pArgs->source=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)match;
    } ଅନ୍ୟଥା /* match==0 or 1 */ {
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

        ଯଦି(match==1) {
            /* matched, no mapping but request for <subchar1> */
            cnv->useSubChar1=ସତ୍ୟ;
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

ସ୍ଥିର UBool
extSetUseMapping(UConverterUnicodeSet which, int32_t minLength, uint32_t value) {
    ଯଦି(which==UCNV_ROUNDTRIP_SET) {
        // Add only code points for which the roundtrip flag is set.
        // Do not add any fallbacks, even if ucnv_fromUnicode() would use them
        // (fallbacks from PUA). See the API docs for ucnv_getUnicodeSet().
        //
        // By analogy, also do not add "good one-way" mappings.
        //
        // Do not add entries with reserved bits set.
        ଯଦି(((value&(UCNV_EXT_FROM_U_ROUNDTRIP_FLAG|UCNV_EXT_FROM_U_RESERVED_MASK))!=
                UCNV_EXT_FROM_U_ROUNDTRIP_FLAG)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    } ଅନ୍ୟଥା /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
        // Do not add entries with reserved bits set.
        ଯଦି((value&UCNV_EXT_FROM_U_RESERVED_MASK)!=0) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    // Do not add <subchar1> entries or other (future?) pseudo-entries
    // with an output length of 0.
    ଫେରନ୍ତୁ UCNV_EXT_FROM_U_GET_LENGTH(value)>=minLength;
}

ସ୍ଥିର ଶୂନ୍ୟ
ucnv_extGetUnicodeSetString(ଅଚଳ UConverterSharedData *sharedData,
                            ଅଚଳ int32_t *cx,
                            ଅଚଳ USetAdder *sa,
                            UConverterUnicodeSet which,
                            int32_t minLength,
                            UChar32 firstCP,
                            char16_t s[UCNV_EXT_MAX_UCHARS], int32_t length,
                            int32_t sectionIndex,
                            UErrorCode *pErrorCode) {
    ଅଚଳ char16_t *fromUSectionUChars;
    ଅଚଳ uint32_t *from_USection_ମୂଲ୍ୟଗୁଡ଼ିକ;

    uint32_t value;
    int32_t i, count;

    fromUSectionUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_INDEX, char16_t)+sectionIndex;
    from_USection_ମୂଲ୍ୟଗୁଡ଼ିକ=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_ମୂଲ୍ୟଗୁଡ଼ିକ_INDEX, uint32_t)+sectionIndex;

    /* read first pair of the section */
    count=*fromUSectionUChars++;
    value=*from_USection_ମୂଲ୍ୟଗୁଡ଼ିକ++;

    ଯଦି(extSetUseMapping(which, minLength, value)) {
        ଯଦି(length==U16_LENGTH(firstCP)) {
            /* add the initial code point */
            sa->add(sa->set, firstCP);
        } ଅନ୍ୟଥା {
            /* add the string so far */
            sa->addString(sa->set, s, length);
        }
    }

    ପାଇଁ(i=0; i<count; ++i) {
        /* append this code unit and recurse or add the string */
        s[length]=fromUSectionUChars[i];
        value=from_USection_ମୂଲ୍ୟଗୁଡ଼ିକ[i];

        ଯଦି(value==0) {
            /* no mapping, do nothing */
        } ଅନ୍ୟଥା ଯଦି(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
            ucnv_extGetUnicodeSetString(
                sharedData, cx, sa, which, minLength,
                firstCP, s, length+1,
                static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_INDEX(value)),
                pErrorCode);
        } ଅନ୍ୟଥା ଯଦି(extSetUseMapping(which, minLength, value)) {
            sa->addString(sa->set, s, length+1);
        }
    }
}

U_CFUNC ଶୂନ୍ୟ
ucnv_extGetUnicodeSet(ଅଚଳ UConverterSharedData *sharedData,
                      ଅଚଳ USetAdder *sa,
                      UConverterUnicodeSet which,
                      UConverterSetFilter filter,
                      UErrorCode *pErrorCode) {
    ଅଚଳ int32_t *cx;
    ଅଚଳ uint16_t *stage12, *stage3, *ps2, *ps3;
    ଅଚଳ uint32_t *stage3b;

    uint32_t value;
    int32_t st1, stage1Length, st2, st3, minLength;

    char16_t s[UCNV_EXT_MAX_UCHARS];
    UChar32 c;
    int32_t length;

    cx=sharedData->mbcs.extIndexes;
    ଯଦି(cx==nullptr) {
        ଫେରନ୍ତୁ;
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_INDEX, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_INDEX, uint16_t);
    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3B_INDEX, uint32_t);

    stage1Length=cx[UCNV_EXT_FROM_U_STAGE_1_LENGTH];

    /* enumerate the from-Unicode trie table */
    c=0; /* keep track of the current code point while enumerating */

    ଯଦି(filter==UCNV_SET_FILTER_2022_CN) {
        minLength=3;
    } ଅନ୍ୟଥା ଯଦି( sharedData->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ||
               filter!=UCNV_SET_FILTER_NONE
    ) {
        /* DBCS-only, ignore single-byte results */
        minLength=2;
    } ଅନ୍ୟଥା {
        minLength=1;
    }

    /*
     * the trie enumeration is almost the same as
     * in MBCSGetUnicodeSet() for MBCS_OUTPUT_1
     */
    ପାଇଁ(st1=0; st1<stage1Length; ++st1) {
        st2=stage12[st1];
        ଯଦି(st2>stage1Length) {
            ps2=stage12+st2;
            ପାଇଁ(st2=0; st2<64; ++st2) {
                ଯଦି((st3=(int32_t)ps2[st2]<<UCNV_EXT_STAGE_2_ବାମ_SHIFT)!=0) {
                    /* read the stage 3 block */
                    ps3=stage3+st3;

                    କର {
                        value=stage3b[*ps3++];
                        ଯଦି(value==0) {
                            /* no mapping, do nothing */
                        } ଅନ୍ୟଥା ଯଦି(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                            // Recurse for partial results.
                            length=0;
                            U16_APPEND_UNSAFE(s, length, c);
                            ucnv_extGetUnicodeSetString(
                                sharedData, cx, sa, which, minLength,
                                c, s, length,
                                (int32_t)UCNV_EXT_FROM_U_GET_PARTIAL_INDEX(value),
                                pErrorCode);
                        } ଅନ୍ୟଥା ଯଦି(extSetUseMapping(which, minLength, value)) {
                            ବାଛ(filter) {
                            ସ୍ଥିତି UCNV_SET_FILTER_2022_CN:
                                ଯଦି(!(UCNV_EXT_FROM_U_GET_LENGTH(value)==3 && UCNV_EXT_FROM_U_GET_DATA(value)<=0x82ffff)) {
                                    ଜାରି;
                                }
                                ଭାଙ୍ଗ;
                            ସ୍ଥିତି UCNV_SET_FILTER_SJIS:
                                ଯଦି(!(UCNV_EXT_FROM_U_GET_LENGTH(value)==2 && (value=UCNV_EXT_FROM_U_GET_DATA(value))>=0x8140 && value<=0xeffc)) {
                                    ଜାରି;
                                }
                                ଭାଙ୍ଗ;
                            ସ୍ଥିତି UCNV_SET_FILTER_GR94DBCS:
                                ଯଦି(!(UCNV_EXT_FROM_U_GET_LENGTH(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_DATA(value))-0xa1a1)<=(0xfefe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    ଜାରି;
                                }
                                ଭାଙ୍ଗ;
                            ସ୍ଥିତି UCNV_SET_FILTER_HZ:
                                ଯଦି(!(UCNV_EXT_FROM_U_GET_LENGTH(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_DATA(value))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    ଜାରି;
                                }
                                ଭାଙ୍ଗ;
                            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                                /*
                                 * UCNV_SET_FILTER_NONE,
                                 * or UCNV_SET_FILTER_DBCS_ONLY which is handled via minLength
                                 */
                                ଭାଙ୍ଗ;
                            }
                            sa->add(sa->set, c);
                        }
                    } ଯେତେବେଳେ((++c&0xf)!=0);
                } ଅନ୍ୟଥା {
                    c+=16; /* empty stage 3 block */
                }
            }
        } ଅନ୍ୟଥା {
            c+=1024; /* empty stage 2 block */
        }
    }
}

#endif /* #if !UCONFIG_NO_LEGACY_CONVERSION */
