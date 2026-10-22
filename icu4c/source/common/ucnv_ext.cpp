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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#समावेश "unicode/uset.h"
#समावेश "unicode/ustring.h"
#समावेश "ucnv_bld.h"
#समावेश "ucnv_cnv.h"
#समावेश "ucnv_ext.h"
#समावेश "cmemory.h"
#समावेश "uassert.h"

/* to Unicode --------------------------------------------------------------- */

/*
 * @return lookup value for the byte, if found; else 0
 */
अटल अंतरभूत uint32_t
ucnv_extFindToU(स्थिर uint32_t *toUSection, int32_t length, uint8_t byte) {
    uint32_t word0, word;
    int32_t i, start, limit;

    /* check the input byte against the lowest and highest section bytes */
    start = static_cast<int32_t>(UCNV_EXT_TO_U_GET_BYTE(toUSection[0]));
    limit = static_cast<int32_t>(UCNV_EXT_TO_U_GET_BYTE(toUSection[length - 1]));
    अगर(byte<start || limit<byte) {
        वापस 0; /* the byte is out of range */
    }

    अगर(length==((limit-start)+1)) {
        /* direct access on a linear array */
        वापस UCNV_EXT_TO_U_GET_मूल्य(toUSection[byte-start]); /* could be 0 */
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
    word=word0|UCNV_EXT_TO_U_मूल्य_MASK;

    /* binary search */
    start=0;
    limit=length;
    क्रम(;;) {
        i=limit-start;
        अगर(i<=1) {
            अवरोध; /* done */
        }
        /* start<limit-1 */

        अगर(i<=4) {
            /* linear search for the last part */
            अगर(word0<=toUSection[start]) {
                अवरोध;
            }
            अगर(++start<limit && word0<=toUSection[start]) {
                अवरोध;
            }
            अगर(++start<limit && word0<=toUSection[start]) {
                अवरोध;
            }
            /* always break at start==limit-1 */
            ++start;
            अवरोध;
        }

        i=(start+limit)/2;
        अगर(word<toUSection[i]) {
            limit=i;
        } अन्यथा {
            start=i;
        }
    }

    /* did we really find it? */
    अगर(start<limit && byte==UCNV_EXT_TO_U_GET_BYTE(word=toUSection[start])) {
        वापस UCNV_EXT_TO_U_GET_मूल्य(word); /* never 0 */
    } अन्यथा {
        वापस 0; /* not found */
    }
}

/*
 * true if not an SI/SO stateful converter,
 * or if the match length fits with the current converter state
 */
#घोषणा UCNV_EXT_TO_U_VERIFY_SISO_MATCH(sisoState, match) \
    ((sisoState)<0 || ((sisoState)==0) == (match==1))

/*
 * this works like ucnv_extMatchFromU() except
 * - the first character is in pre
 * - no trie is used
 * - the returned matchLength is not offset by 2
 */
अटल int32_t
ucnv_extMatchToU(स्थिर int32_t *cx, int8_t sisoState,
                 स्थिर अक्षर *pre, int32_t pre_लंबाई,
                 स्थिर अक्षर *src, int32_t src_लंबाई,
                 uint32_t *p_Match_मूल्य,
                 UBool /*useFallback*/, UBool सफाई) {
    स्थिर uint32_t *toUTable, *toUSection;

    uint32_t value, match_मूल्य;
    int32_t i, j, idx, length, match_लंबाई;
    uint8_t b;

    अगर(cx==nullptr || cx[UCNV_EXT_TO_U_लंबाई]<=0) {
        वापस 0; /* no extension data, no match */
    }

    /* initialize */
    toUTable=UCNV_EXT_ARRAY(cx, UCNV_EXT_TO_U_सूचकांक, uint32_t);
    idx=0;

    match_मूल्य=0;
    i=j=match_लंबाई=0;

    अगर(sisoState==0) {
        /* SBCS state of an SI/SO stateful converter, look at only exactly 1 byte */
        अगर(pre_लंबाई>1) {
            वापस 0; /* no match of a DBCS sequence in SBCS mode */
        } अन्यथा अगर(pre_लंबाई==1) {
            src_लंबाई=0;
        } अन्यथा /* preLength==0 */ {
            अगर(src_लंबाई>1) {
                src_लंबाई=1;
            }
        }
        सफाई=true;
    }

    /* we must not remember fallback matches when not using fallbacks */

    /* match input units until there is a full match or the input is consumed */
    क्रम(;;) {
        /* go to the next section */
        toUSection=toUTable+idx;

        /* read first pair of the section */
        value=*toUSection++;
        length=UCNV_EXT_TO_U_GET_BYTE(value);
        value=UCNV_EXT_TO_U_GET_मूल्य(value);
        अगर( value!=0 &&
            (UCNV_EXT_TO_U_IS_ROUNDTRIP(value) ||
             TO_U_USE_FALLBACK(useFallback)) &&
            UCNV_EXT_TO_U_VERIFY_SISO_MATCH(sisoState, i+j)
        ) {
            /* remember longest match so far */
            match_मूल्य=value;
            match_लंबाई=i+j;
        }

        /* match pre[] then src[] */
        अगर(i<pre_लंबाई) {
            b = static_cast<uint8_t>(pre[i++]);
        } अन्यथा अगर(j<src_लंबाई) {
            b = static_cast<uint8_t>(src[j++]);
        } अन्यथा {
            /* all input consumed, partial match */
            अगर(सफाई || (length=(i+j))>UCNV_EXT_MAX_BYTES) {
                /*
                 * end of the entire input stream, stop with the longest match so far
                 * or: partial match must not be longer than UCNV_EXT_MAX_BYTES
                 * because it must fit into state buffers
                 */
                अवरोध;
            } अन्यथा {
                /* continue with more input next time */
                वापस -length;
            }
        }

        /* search for the current char16_t */
        value=ucnv_extFindToU(toUSection, length, b);
        अगर(value==0) {
            /* no match here, stop with the longest match so far */
            अवरोध;
        } अन्यथा {
            अगर(UCNV_EXT_TO_U_IS_PARTIAL(value)) {
                /* partial match, continue */
                idx = static_cast<int32_t>(UCNV_EXT_TO_U_GET_PARTIAL_सूचकांक(value));
            } अन्यथा {
                अगर( (UCNV_EXT_TO_U_IS_ROUNDTRIP(value) ||
                     TO_U_USE_FALLBACK(useFallback)) &&
                    UCNV_EXT_TO_U_VERIFY_SISO_MATCH(sisoState, i+j)
                ) {
                    /* full match, stop with result */
                    match_मूल्य=value;
                    match_लंबाई=i+j;
                } अन्यथा {
                    /* full match on fallback not taken, stop with the longest match so far */
                }
                अवरोध;
            }
        }
    }

    अगर(match_लंबाई==0) {
        /* no match at all */
        वापस 0;
    }

    /* return result */
    *p_Match_मूल्य=UCNV_EXT_TO_U_MASK_ROUNDTRIP(match_मूल्य);
    वापस match_लंबाई;
}

अटल अंतरभूत व्योम
ucnv_extWriteToU(UConverter *cnv, स्थिर int32_t *cx,
                 uint32_t value,
                 char16_t **target, स्थिर char16_t *targetLimit,
                 int32_t **offsets, int32_t src_सूचकांक,
                 UErrorCode *pErrorCode) {
    /* output the result */
    अगर(UCNV_EXT_TO_U_IS_CODE_POINT(value)) {
        /* output a single code point */
        ucnv_toUWriteCodePoint(
            cnv, UCNV_EXT_TO_U_GET_CODE_POINT(value),
            target, targetLimit,
            offsets, src_सूचकांक,
            pErrorCode);
    } अन्यथा {
        /* output a string - with correct data we have resultLength>0 */
        ucnv_toUWriteUChars(
            cnv,
            UCNV_EXT_ARRAY(cx, UCNV_EXT_TO_U_UCHARS_सूचकांक, char16_t)+
                UCNV_EXT_TO_U_GET_सूचकांक(value),
            UCNV_EXT_TO_U_GET_लंबाई(value),
            target, targetLimit,
            offsets, src_सूचकांक,
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
#घोषणा UCNV_SISO_STATE(cnv) \
    ((cnv)->shared_तथ्य->mbcs.outputType==MBCS_OUTPUT_2_SISO ? (int8_t)(cnv)->mode : \
     (cnv)->shared_तथ्य->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ? 1 : -1)

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_extInitialMatchToU(UConverter *cnv, स्थिर int32_t *cx,
                        int32_t first_लंबाई,
                        स्थिर अक्षर **src, स्थिर अक्षर *srcLimit,
                        char16_t **target, स्थिर char16_t *targetLimit,
                        int32_t **offsets, int32_t src_सूचकांक,
                        UBool सफाई,
                        UErrorCode *pErrorCode) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match;

    /* try to match */
    match=ucnv_extMatchToU(cx, (int8_t)UCNV_SISO_STATE(cnv),
                           (स्थिर अक्षर *)cnv->toUBytes, first_लंबाई,
                           *src, (int32_t)(srcLimit-*src),
                           &value,
                           cnv->useFallback, सफाई);
    अगर(match>0) {
        /* advance src pointer for the consumed input */
        *src+=match-first_लंबाई;

        /* write result to target */
        ucnv_extWriteToU(cnv, cx,
                         value,
                         target, targetLimit,
                         offsets, src_सूचकांक,
                         pErrorCode);
        वापस true;
    } अन्यथा अगर(match<0) {
        /* save state for partial match */
        स्थिर अक्षर *s;
        int32_t j;

        /* copy the first code point */
        s=(स्थिर अक्षर *)cnv->toUBytes;
        cnv->pre_To_UFirst_लंबाई=(int8_t)first_लंबाई;
        क्रम(j=0; j<first_लंबाई; ++j) {
            cnv->preToU[j]=*s++;
        }

        /* now copy the newly consumed input */
        s=*src;
        match=-match;
        क्रम(; j<match; ++j) {
            cnv->preToU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preToULength=(int8_t)match;
        वापस true;
    } अन्यथा /* match==0 no match */ {
        वापस false;
    }
}

U_CFUNC UChar32
ucnv_extSimpleMatchToU(स्थिर int32_t *cx,
                       स्थिर अक्षर *source, int32_t length,
                       UBool useFallback) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match;

    अगर(length<=0) {
        वापस 0xffff;
    }

    /* try to match */
    match=ucnv_extMatchToU(cx, -1,
                           source, length,
                           nullptr, 0,
                           &value,
                           useFallback, true);
    अगर(match==length) {
        /* write result for simple, single-character conversion */
        अगर(UCNV_EXT_TO_U_IS_CODE_POINT(value)) {
            वापस UCNV_EXT_TO_U_GET_CODE_POINT(value);
        }
    }

    /*
     * return no match because
     * - match>0 && value points to string: simple conversion cannot handle multiple code points
     * - match>0 && match!=length: not all input consumed, forbidden for this function
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    वापस 0xfffe;
}

/*
 * continue partial match with new input
 * never called for simple, single-character conversion
 */
U_CFUNC व्योम
ucnv_extContinueMatchToU(UConverter *cnv,
                         UConverterToUnicodeArgs *pArgs, int32_t src_सूचकांक,
                         UErrorCode *pErrorCode) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match, length;

    match=ucnv_extMatchToU(cnv->shared_तथ्य->mbcs.extIndexes, (int8_t)UCNV_SISO_STATE(cnv),
                           cnv->preToU, cnv->preToULength,
                           pArgs->source, (int32_t)(pArgs->sourceLimit-pArgs->source),
                           &value,
                           cnv->useFallback, pArgs->सफाई);
    अगर(match>0) {
        अगर(match>=cnv->preToULength) {
            /* advance src pointer for the consumed input */
            pArgs->source+=match-cnv->preToULength;
            cnv->preToULength=0;
        } अन्यथा {
            /* the match did not use all of preToU[] - keep the rest for replay */
            length=cnv->preToULength-match;
            uprv_memmove(cnv->preToU, cnv->preToU+match, length);
            cnv->preToULength=(int8_t)-length;
        }

        /* write result */
        ucnv_extWriteToU(cnv, cnv->shared_तथ्य->mbcs.extIndexes,
                         value,
                         &pArgs->target, pArgs->targetLimit,
                         &pArgs->offsets, src_सूचकांक,
                         pErrorCode);
    } अन्यथा अगर(match<0) {
        /* save state for partial match */
        स्थिर अक्षर *s;
        int32_t j;

        /* just _append_ the newly consumed input to preToU[] */
        s=pArgs->source;
        match=-match;
        क्रम(j=cnv->preToULength; j<match; ++j) {
            cnv->preToU[j]=*s++;
        }
        pArgs->source=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preToULength=(int8_t)match;
    } अन्यथा /* match==0 */ {
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
        uprv_memcpy(cnv->toUBytes, cnv->preToU, cnv->pre_To_UFirst_लंबाई);
        cnv->toULength=cnv->pre_To_UFirst_लंबाई;

        /* move the rest up inside the buffer */
        length=cnv->preToULength-cnv->pre_To_UFirst_लंबाई;
        अगर(length>0) {
            uprv_memmove(cnv->preToU, cnv->preToU+cnv->pre_To_UFirst_लंबाई, length);
        }

        /* mark preToU for replay */
        cnv->preToULength=(int8_t)-length;

        /* set the error code for unassigned */
        *pErrorCode=U_INVALID_CHAR_FOUND;
    }
}

/* from Unicode ------------------------------------------------------------- */

// Use roundtrips, "good one-way" mappings, and some normal fallbacks.
अटल अंतरभूत UBool
extFromUUseMapping(UBool useFallback, uint32_t value, UChar32 firstCP) {
    वापस
        ((value&UCNV_EXT_FROM_U_STATUS_MASK)!=0 ||
            FROM_U_USE_FALLBACK(useFallback, firstCP)) &&
        (value&UCNV_EXT_FROM_U_RESERVED_MASK)==0;
}

/*
 * @return index of the char16_t, if found; else <0
 */
अटल अंतरभूत int32_t
ucnv_extFindFromU(स्थिर char16_t *fromUSection, int32_t length, char16_t u) {
    int32_t i, start, limit;

    /* binary search */
    start=0;
    limit=length;
    क्रम(;;) {
        i=limit-start;
        अगर(i<=1) {
            अवरोध; /* done */
        }
        /* start<limit-1 */

        अगर(i<=4) {
            /* linear search for the last part */
            अगर(u<=fromUSection[start]) {
                अवरोध;
            }
            अगर(++start<limit && u<=fromUSection[start]) {
                अवरोध;
            }
            अगर(++start<limit && u<=fromUSection[start]) {
                अवरोध;
            }
            /* always break at start==limit-1 */
            ++start;
            अवरोध;
        }

        i=(start+limit)/2;
        अगर(u<fromUSection[i]) {
            limit=i;
        } अन्यथा {
            start=i;
        }
    }

    /* did we really find it? */
    अगर(start<limit && u==fromUSection[start]) {
        वापस start;
    } अन्यथा {
        वापस -1; /* not found */
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
अटल int32_t
ucnv_extMatchFromU(स्थिर int32_t *cx,
                   UChar32 firstCP,
                   स्थिर char16_t *pre, int32_t pre_लंबाई,
                   स्थिर char16_t *src, int32_t src_लंबाई,
                   uint32_t *p_Match_मूल्य,
                   UBool useFallback, UBool सफाई) {
    स्थिर uint16_t *stage12, *stage3;
    स्थिर uint32_t *stage3b;

    स्थिर char16_t *fromUTableUChars, *fromUSectionUChars;
    स्थिर uint32_t *from_UTable_मूल्यावली, *from_USection_मूल्यावली;

    uint32_t value, match_मूल्य;
    int32_t i, j, idx, length, match_लंबाई;
    char16_t c;

    अगर(cx==nullptr) {
        वापस 0; /* no extension data, no match */
    }

    /* trie lookup of firstCP */
    idx=firstCP>>10; /* stage 1 index */
    अगर(idx>=cx[UCNV_EXT_FROM_U_STAGE_1_लंबाई]) {
        वापस 0; /* the first code point is outside the trie */
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_सूचकांक, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_सूचकांक, uint16_t);
    idx=UCNV_EXT_FROM_U(stage12, stage3, idx, firstCP);

    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_सूचकांक, uint32_t);
    value=stage3b[idx];
    अगर(value==0) {
        वापस 0;
    }

    /*
     * Tests for (value&UCNV_EXT_FROM_U_RESERVED_MASK)==0:
     * Do not interpret values with reserved bits used, for forward compatibility,
     * and do not even remember intermediate results with reserved bits used.
     */

    अगर(UCNV_EXT_TO_U_IS_PARTIAL(value)) {
        /* partial match, enter the loop below */
        idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_सूचकांक(value));

        /* initialize */
        fromUTableUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_सूचकांक, char16_t);
        from_UTable_मूल्यावली=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_मूल्यावली_सूचकांक, uint32_t);

        match_मूल्य=0;
        i=j=match_लंबाई=0;

        /* we must not remember fallback matches when not using fallbacks */

        /* match input units until there is a full match or the input is consumed */
        क्रम(;;) {
            /* go to the next section */
            fromUSectionUChars=fromUTableUChars+idx;
            from_USection_मूल्यावली=from_UTable_मूल्यावली+idx;

            /* read first pair of the section */
            length=*fromUSectionUChars++;
            value=*from_USection_मूल्यावली++;
            अगर(value!=0 && extFromUUseMapping(useFallback, value, firstCP)) {
                /* remember longest match so far */
                match_मूल्य=value;
                match_लंबाई=2+i+j;
            }

            /* match pre[] then src[] */
            अगर(i<pre_लंबाई) {
                c=pre[i++];
            } अन्यथा अगर(j<src_लंबाई) {
                c=src[j++];
            } अन्यथा {
                /* all input consumed, partial match */
                अगर(सफाई || (length=(i+j))>UCNV_EXT_MAX_UCHARS) {
                    /*
                     * end of the entire input stream, stop with the longest match so far
                     * or: partial match must not be longer than UCNV_EXT_MAX_UCHARS
                     * because it must fit into state buffers
                     */
                    अवरोध;
                } अन्यथा {
                    /* continue with more input next time */
                    वापस -(2+length);
                }
            }

            /* search for the current char16_t */
            idx=ucnv_extFindFromU(fromUSectionUChars, length, c);
            अगर(idx<0) {
                /* no match here, stop with the longest match so far */
                अवरोध;
            } अन्यथा {
                value=from_USection_मूल्यावली[idx];
                अगर(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                    /* partial match, continue */
                    idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_सूचकांक(value));
                } अन्यथा {
                    अगर(extFromUUseMapping(useFallback, value, firstCP)) {
                        /* full match, stop with result */
                        match_मूल्य=value;
                        match_लंबाई=2+i+j;
                    } अन्यथा {
                        /* full match on fallback not taken, stop with the longest match so far */
                    }
                    अवरोध;
                }
            }
        }

        अगर(match_लंबाई==0) {
            /* no match at all */
            वापस 0;
        }
    } अन्यथा /* result from firstCP trie lookup */ {
        अगर(extFromUUseMapping(useFallback, value, firstCP)) {
            /* full match, stop with result */
            match_मूल्य=value;
            match_लंबाई=2;
        } अन्यथा {
            /* fallback not taken */
            वापस 0;
        }
    }

    /* return result */
    अगर(match_मूल्य==UCNV_EXT_FROM_U_SUBCHAR1) {
        वापस 1; /* assert matchLength==2 */
    }

    *p_Match_मूल्य=match_मूल्य;
    वापस match_लंबाई;
}

/*
 * @param value fromUnicode mapping table value; ignores roundtrip and reserved bits
 */
अटल अंतरभूत व्योम
ucnv_extWriteFromU(UConverter *cnv, स्थिर int32_t *cx,
                   uint32_t value,
                   अक्षर **target, स्थिर अक्षर *targetLimit,
                   int32_t **offsets, int32_t src_सूचकांक,
                   UErrorCode *pErrorCode) {
    uint8_t बफर[1+UCNV_EXT_MAX_BYTES];
    स्थिर uint8_t *परिणाम;
    int32_t length, पूर्व_लंबाई;

    length=UCNV_EXT_FROM_U_GET_लंबाई(value);
    value = UCNV_EXT_FROM_U_GET_तथ्य(value);

    /* output the result */
    अगर(length<=UCNV_EXT_FROM_U_MAX_DIRECT_लंबाई) {
        /*
         * Generate a byte array and then write it below.
         * This is not the fastest possible way, but it should be ok for
         * extension mappings, and it is much simpler.
         * Offset and overflow handling are only done once this way.
         */
        uint8_t *p=बफर+1; /* reserve buffer[0] for shiftByte below */
        चयन(length) {
        हाल 3:
            *p++ = static_cast<uint8_t>(value >> 16);
            U_FALLTHROUGH;
        हाल 2:
            *p++ = static_cast<uint8_t>(value >> 8);
            U_FALLTHROUGH;
        हाल 1:
            *p++ = static_cast<uint8_t>(value);
            U_FALLTHROUGH;
        शेष:
            अवरोध; /* will never occur */
        }
        परिणाम=बफर+1;
    } अन्यथा {
        परिणाम=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_BYTES_सूचकांक, uint8_t)+value;
    }

    /* with correct data we have length>0 */

    अगर((पूर्व_लंबाई=cnv->fromUnicodeStatus)!=0) {
        /* handle SI/SO stateful output */
        uint8_t shiftByte;

        अगर(पूर्व_लंबाई>1 && length==1) {
            /* change from double-byte mode to single-byte */
            shiftByte = static_cast<uint8_t>(UCNV_SI);
            cnv->fromUnicodeStatus=1;
        } अन्यथा अगर(पूर्व_लंबाई==1 && length>1) {
            /* change from single-byte mode to double-byte */
            shiftByte = static_cast<uint8_t>(UCNV_SO);
            cnv->fromUnicodeStatus=2;
        } अन्यथा {
            shiftByte=0;
        }

        अगर(shiftByte!=0) {
            /* prepend the shift byte to the result bytes */
            बफर[0]=shiftByte;
            अगर(परिणाम!=बफर+1) {
                uprv_memcpy(बफर+1, परिणाम, length);
            }
            परिणाम=बफर;
            ++length;
        }
    }

    ucnv_fromUWriteBytes(cnv, reinterpret_cast<स्थिर अक्षर*>(परिणाम), length,
                         target, targetLimit,
                         offsets, src_सूचकांक,
                         pErrorCode);
}

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_extInitialMatchFromU(UConverter *cnv, स्थिर int32_t *cx,
                          UChar32 cp,
                          स्थिर char16_t **src, स्थिर char16_t *srcLimit,
                          अक्षर **target, स्थिर अक्षर *targetLimit,
                          int32_t **offsets, int32_t src_सूचकांक,
                          UBool सफाई,
                          UErrorCode *pErrorCode) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match;

    /* try to match */
    match=ucnv_extMatchFromU(cx, cp,
                             nullptr, 0,
                             *src, (int32_t)(srcLimit-*src),
                             &value,
                             cnv->useFallback, सफाई);

    /* reject a match if the result is a single byte for DBCS-only */
    अगर( match>=2 &&
        !(UCNV_EXT_FROM_U_GET_लंबाई(value)==1 &&
          cnv->shared_तथ्य->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY)
    ) {
        /* advance src pointer for the consumed input */
        *src+=match-2; /* remove 2 for the initial code point */

        /* write result to target */
        ucnv_extWriteFromU(cnv, cx,
                           value,
                           target, targetLimit,
                           offsets, src_सूचकांक,
                           pErrorCode);
        वापस true;
    } अन्यथा अगर(match<0) {
        /* save state for partial match */
        स्थिर char16_t *s;
        int32_t j;

        /* copy the first code point */
        cnv->preFromUFirstCP=cp;

        /* now copy the newly consumed input */
        s=*src;
        match=-match-2; /* remove 2 for the initial code point */
        क्रम(j=0; j<match; ++j) {
            cnv->preFromU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)match;
        वापस true;
    } अन्यथा अगर(match==1) {
        /* matched, no mapping but request for <subchar1> */
        cnv->useSubChar1=true;
        वापस false;
    } अन्यथा /* match==0 no match */ {
        वापस false;
    }
}

/*
 * Used by ISO 2022 implementation.
 * @return number of bytes in *pValue; negative number if fallback; 0 for no mapping
 */
U_CFUNC int32_t
ucnv_extSimpleMatchFromU(स्थिर int32_t *cx,
                         UChar32 cp, uint32_t *p_मूल्य,
                         UBool useFallback) {
    uint32_t value;
    int32_t match;

    /* try to match */
    match=ucnv_extMatchFromU(cx,
                             cp,
                             nullptr, 0,
                             nullptr, 0,
                             &value,
                             useFallback, true);
    अगर(match>=2) {
        /* write result for simple, single-character conversion */
        int32_t length;
        पूर्णांक isRoundtrip;

        isRoundtrip=UCNV_EXT_FROM_U_IS_ROUNDTRIP(value);
        length=UCNV_EXT_FROM_U_GET_लंबाई(value);
        value = UCNV_EXT_FROM_U_GET_तथ्य(value);

        अगर(length<=UCNV_EXT_FROM_U_MAX_DIRECT_लंबाई) {
            *p_मूल्य=value;
            वापस isRoundtrip ? length : -length;
#अगर 0 /* not currently used */
        } अन्यथा अगर(length==4) {
            /* de-serialize a 4-byte result */
            स्थिर uint8_t *परिणाम=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_BYTES_सूचकांक, uint8_t)+value;
            *p_मूल्य=
                ((uint32_t)परिणाम[0]<<24)|
                ((uint32_t)परिणाम[1]<<16)|
                ((uint32_t)परिणाम[2]<<8)|
                परिणाम[3];
            वापस isRoundtrip ? 4 : -4;
#पूर्ण_अगर
        }
    }

    /*
     * return no match because
     * - match>1 && resultLength>4: result too long for simple conversion
     * - match==1: no match found, <subchar1> preferred
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    वापस 0;
}

/*
 * continue partial match with new input, requires cnv->preFromUFirstCP>=0
 * never called for simple, single-character conversion
 */
U_CFUNC व्योम
ucnv_extContinueMatchFromU(UConverter *cnv,
                           UConverterFromUnicodeArgs *pArgs, int32_t src_सूचकांक,
                           UErrorCode *pErrorCode) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t match;

    match=ucnv_extMatchFromU(cnv->shared_तथ्य->mbcs.extIndexes,
                             cnv->preFromUFirstCP,
                             cnv->preFromU, cnv->preFromULength,
                             pArgs->source, (int32_t)(pArgs->sourceLimit-pArgs->source),
                             &value,
                             cnv->useFallback, pArgs->सफाई);
    अगर(match>=2) {
        match-=2; /* remove 2 for the initial code point */

        अगर(match>=cnv->preFromULength) {
            /* advance src pointer for the consumed input */
            pArgs->source+=match-cnv->preFromULength;
            cnv->preFromULength=0;
        } अन्यथा {
            /* the match did not use all of preFromU[] - keep the rest for replay */
            int32_t length=cnv->preFromULength-match;
            u_memmove(cnv->preFromU, cnv->preFromU+match, length);
            cnv->preFromULength=(int8_t)-length;
        }

        /* finish the partial match */
        cnv->preFromUFirstCP=U_SENTINEL;

        /* write result */
        ucnv_extWriteFromU(cnv, cnv->shared_तथ्य->mbcs.extIndexes,
                           value,
                           &pArgs->target, pArgs->targetLimit,
                           &pArgs->offsets, src_सूचकांक,
                           pErrorCode);
    } अन्यथा अगर(match<0) {
        /* save state for partial match */
        स्थिर char16_t *s;
        int32_t j;

        /* just _append_ the newly consumed input to preFromU[] */
        s=pArgs->source;
        match=-match-2; /* remove 2 for the initial code point */
        क्रम(j=cnv->preFromULength; j<match; ++j) {
            U_ASSERT(j>=0);
            cnv->preFromU[j]=*s++;
        }
        pArgs->source=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)match;
    } अन्यथा /* match==0 or 1 */ {
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

        अगर(match==1) {
            /* matched, no mapping but request for <subchar1> */
            cnv->useSubChar1=true;
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

अटल UBool
extSetUseMapping(UConverterUnicodeSet which, int32_t min_लंबाई, uint32_t value) {
    अगर(which==UCNV_ROUNDTRIP_SET) {
        // Add only code points for which the roundtrip flag is set.
        // Do not add any fallbacks, even if ucnv_fromUnicode() would use them
        // (fallbacks from PUA). See the API docs for ucnv_getUnicodeSet().
        //
        // By analogy, also do not add "good one-way" mappings.
        //
        // Do not add entries with reserved bits set.
        अगर(((value&(UCNV_EXT_FROM_U_ROUNDTRIP_FLAG|UCNV_EXT_FROM_U_RESERVED_MASK))!=
                UCNV_EXT_FROM_U_ROUNDTRIP_FLAG)) {
            वापस false;
        }
    } अन्यथा /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
        // Do not add entries with reserved bits set.
        अगर((value&UCNV_EXT_FROM_U_RESERVED_MASK)!=0) {
            वापस false;
        }
    }
    // Do not add <subchar1> entries or other (future?) pseudo-entries
    // with an output length of 0.
    वापस UCNV_EXT_FROM_U_GET_लंबाई(value)>=min_लंबाई;
}

अटल व्योम
ucnv_extGetUnicodeSetString(स्थिर UConverter_Shared_तथ्य *shared_तथ्य,
                            स्थिर int32_t *cx,
                            स्थिर USetAdder *sa,
                            UConverterUnicodeSet which,
                            int32_t min_लंबाई,
                            UChar32 firstCP,
                            char16_t s[UCNV_EXT_MAX_UCHARS], int32_t length,
                            int32_t section_सूचकांक,
                            UErrorCode *pErrorCode) {
    स्थिर char16_t *fromUSectionUChars;
    स्थिर uint32_t *from_USection_मूल्यावली;

    uint32_t value;
    int32_t i, count;

    fromUSectionUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_सूचकांक, char16_t)+section_सूचकांक;
    from_USection_मूल्यावली=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_मूल्यावली_सूचकांक, uint32_t)+section_सूचकांक;

    /* read first pair of the section */
    count=*fromUSectionUChars++;
    value=*from_USection_मूल्यावली++;

    अगर(extSetUseMapping(which, min_लंबाई, value)) {
        अगर(length==U16_लंबाई(firstCP)) {
            /* add the initial code point */
            sa->add(sa->set, firstCP);
        } अन्यथा {
            /* add the string so far */
            sa->addString(sa->set, s, length);
        }
    }

    क्रम(i=0; i<count; ++i) {
        /* append this code unit and recurse or add the string */
        s[length]=fromUSectionUChars[i];
        value=from_USection_मूल्यावली[i];

        अगर(value==0) {
            /* no mapping, do nothing */
        } अन्यथा अगर(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
            ucnv_extGetUnicodeSetString(
                shared_तथ्य, cx, sa, which, min_लंबाई,
                firstCP, s, length+1,
                static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_सूचकांक(value)),
                pErrorCode);
        } अन्यथा अगर(extSetUseMapping(which, min_लंबाई, value)) {
            sa->addString(sa->set, s, length+1);
        }
    }
}

U_CFUNC व्योम
ucnv_extGetUnicodeSet(स्थिर UConverter_Shared_तथ्य *shared_तथ्य,
                      स्थिर USetAdder *sa,
                      UConverterUnicodeSet which,
                      UConverterSetFilter filter,
                      UErrorCode *pErrorCode) {
    स्थिर int32_t *cx;
    स्थिर uint16_t *stage12, *stage3, *ps2, *ps3;
    स्थिर uint32_t *stage3b;

    uint32_t value;
    int32_t st1, stage1_लंबाई, st2, st3, min_लंबाई;

    char16_t s[UCNV_EXT_MAX_UCHARS];
    UChar32 c;
    int32_t length;

    cx=shared_तथ्य->mbcs.extIndexes;
    अगर(cx==nullptr) {
        वापस;
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_सूचकांक, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_सूचकांक, uint16_t);
    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_सूचकांक, uint32_t);

    stage1_लंबाई=cx[UCNV_EXT_FROM_U_STAGE_1_लंबाई];

    /* enumerate the from-Unicode trie table */
    c=0; /* keep track of the current code point while enumerating */

    अगर(filter==UCNV_SET_FILTER_2022_CN) {
        min_लंबाई=3;
    } अन्यथा अगर( shared_तथ्य->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ||
               filter!=UCNV_SET_FILTER_NONE
    ) {
        /* DBCS-only, ignore single-byte results */
        min_लंबाई=2;
    } अन्यथा {
        min_लंबाई=1;
    }

    /*
     * the trie enumeration is almost the same as
     * in MBCSGetUnicodeSet() for MBCS_OUTPUT_1
     */
    क्रम(st1=0; st1<stage1_लंबाई; ++st1) {
        st2=stage12[st1];
        अगर(st2>stage1_लंबाई) {
            ps2=stage12+st2;
            क्रम(st2=0; st2<64; ++st2) {
                अगर((st3=(int32_t)ps2[st2]<<UCNV_EXT_STAGE_2_बायाँ_SHIFT)!=0) {
                    /* read the stage 3 block */
                    ps3=stage3+st3;

                    करो {
                        value=stage3b[*ps3++];
                        अगर(value==0) {
                            /* no mapping, do nothing */
                        } अन्यथा अगर(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                            // Recurse for partial results.
                            length=0;
                            U16_APPEND_UNSAFE(s, length, c);
                            ucnv_extGetUnicodeSetString(
                                shared_तथ्य, cx, sa, which, min_लंबाई,
                                c, s, length,
                                (int32_t)UCNV_EXT_FROM_U_GET_PARTIAL_सूचकांक(value),
                                pErrorCode);
                        } अन्यथा अगर(extSetUseMapping(which, min_लंबाई, value)) {
                            चयन(filter) {
                            हाल UCNV_SET_FILTER_2022_CN:
                                अगर(!(UCNV_EXT_FROM_U_GET_लंबाई(value)==3 && UCNV_EXT_FROM_U_GET_तथ्य(value)<=0x82ffff)) {
                                    जारी;
                                }
                                अवरोध;
                            हाल UCNV_SET_FILTER_SJIS:
                                अगर(!(UCNV_EXT_FROM_U_GET_लंबाई(value)==2 && (value=UCNV_EXT_FROM_U_GET_तथ्य(value))>=0x8140 && value<=0xeffc)) {
                                    जारी;
                                }
                                अवरोध;
                            हाल UCNV_SET_FILTER_GR94DBCS:
                                अगर(!(UCNV_EXT_FROM_U_GET_लंबाई(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_तथ्य(value))-0xa1a1)<=(0xfefe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    जारी;
                                }
                                अवरोध;
                            हाल UCNV_SET_FILTER_HZ:
                                अगर(!(UCNV_EXT_FROM_U_GET_लंबाई(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_तथ्य(value))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    जारी;
                                }
                                अवरोध;
                            शेष:
                                /*
                                 * UCNV_SET_FILTER_NONE,
                                 * or UCNV_SET_FILTER_DBCS_ONLY which is handled via minLength
                                 */
                                अवरोध;
                            }
                            sa->add(sa->set, c);
                        }
                    } जबतक((++c&0xf)!=0);
                } अन्यथा {
                    c+=16; /* empty stage 3 block */
                }
            }
        } अन्यथा {
            c+=1024; /* empty stage 2 block */
        }
    }
}

#पूर्ण_अगर /* #if !UCONFIG_NO_LEGACY_CONVERSION */
