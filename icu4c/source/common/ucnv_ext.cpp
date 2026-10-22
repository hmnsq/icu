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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "ucnv_ext.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"

/* to Unicode --------------------------------------------------------------- */

/*
 * @return lookup value for the byte, if found; else 0
 */
स्थिर अन्तःस्थ uint32_t
ucnv_ext_Find_गन्तव्यम्_U(ध्रुव uint32_t *गन्तव्यम्_USection, int32_t length, uint8_t byte) {
    uint32_t word0, word;
    int32_t i, start, सीमा;

    /* check the input byte against the lowest and highest section bytes */
    start = static_cast<int32_t>(UCNV_EXT_गन्तव्यम्_U_GET_BYTE(गन्तव्यम्_USection[0]));
    सीमा = static_cast<int32_t>(UCNV_EXT_गन्तव्यम्_U_GET_BYTE(गन्तव्यम्_USection[length - 1]));
    यदि(byte<start || सीमा<byte) {
        प्रत्यागम 0; /* the byte is out of range */
    }

    यदि(length==((सीमा-start)+1)) {
        /* direct access on a linear array */
        प्रत्यागम UCNV_EXT_गन्तव्यम्_U_GET_मानम्(गन्तव्यम्_USection[byte-start]); /* could be 0 */
    }

    /* word0 is suitable for <=toUSection[] comparison, word for <toUSection[] */
    word0=UCNV_EXT_गन्तव्यम्_U_MAKE_WORD(byte, 0);

    /*
     * Shift byte once instead of each section word and add 0xffffff.
     * We will compare the shifted/added byte (bbffffff) against
     * section words which have byte values in the same bit position.
     * If and only if byte bb < section byte ss then bbffffff<ssvvvvvv
     * for all v=0..f
     * so we need not mask off the lower 24 bits of each section word.
     */
    word=word0|UCNV_EXT_गन्तव्यम्_U_मानम्_आवरणम्;

    /* binary search */
    start=0;
    सीमा=length;
    प्रति(;;) {
        i=सीमा-start;
        यदि(i<=1) {
            भङ्ग; /*समाप्तम्*/
        }
        /* start<limit-1 */

        यदि(i<=4) {
            /* linear search for the last part */
            यदि(word0<=गन्तव्यम्_USection[start]) {
                भङ्ग;
            }
            यदि(++start<सीमा && word0<=गन्तव्यम्_USection[start]) {
                भङ्ग;
            }
            यदि(++start<सीमा && word0<=गन्तव्यम्_USection[start]) {
                भङ्ग;
            }
            /* always break at start==limit-1 */
            ++start;
            भङ्ग;
        }

        i=(start+सीमा)/2;
        यदि(word<गन्तव्यम्_USection[i]) {
            सीमा=i;
        } अन्यथा {
            start=i;
        }
    }

    /* did we really find it? */
    यदि(start<सीमा && byte==UCNV_EXT_गन्तव्यम्_U_GET_BYTE(word=गन्तव्यम्_USection[start])) {
        प्रत्यागम UCNV_EXT_गन्तव्यम्_U_GET_मानम्(word); /* never 0 */
    } अन्यथा {
        प्रत्यागम 0; /* not found */
    }
}

/*
 * true if not an SI/SO stateful converter,
 * or if the match length fits with the current converter state
 */
#परिभाषय UCNV_EXT_गन्तव्यम्_U_VERIFY_SISO_मेलनम्(sisoState, मेलनम्) \
    ((sisoState)<0 || ((sisoState)==0) == (मेलनम्==1))

/*
 * this works like ucnv_extMatchFromU() except
 * - the first character is in pre
 * - no trie is used
 * - the returned matchLength is not offset by 2
 */
स्थिर int32_t
ucnv_ext_मेलनम्_गन्तव्यम्_U(ध्रुव int32_t *cx, int8_t sisoState,
                 ध्रुव वर्ण *pre, int32_t pre_दीर्घता,
                 ध्रुव वर्ण *src, int32_t src_दीर्घता,
                 uint32_t *p_मेलनम्_मानम्,
                 UBool /*useFallback*/, UBool flush) {
    ध्रुव uint32_t *गन्तव्यम्_UTable, *गन्तव्यम्_USection;

    uint32_t value, मेलनम्_मानम्;
    int32_t i, j, idx, length, मेलनम्_दीर्घता;
    uint8_t b;

    यदि(cx==शून्यसूचकः || cx[UCNV_EXT_गन्तव्यम्_U_दीर्घता]<=0) {
        प्रत्यागम 0; /* no extension data, no match */
    }

    /* initialize */
    गन्तव्यम्_UTable=UCNV_EXT_ARRAY(cx, UCNV_EXT_गन्तव्यम्_U_सूचकांक, uint32_t);
    idx=0;

    मेलनम्_मानम्=0;
    i=j=मेलनम्_दीर्घता=0;

    यदि(sisoState==0) {
        /* SBCS state of an SI/SO stateful converter, look at only exactly 1 byte */
        यदि(pre_दीर्घता>1) {
            प्रत्यागम 0; /* no match of a DBCS sequence in SBCS mode */
        } अन्यथा यदि(pre_दीर्घता==1) {
            src_दीर्घता=0;
        } अन्यथा /* preLength==0 */ {
            यदि(src_दीर्घता>1) {
                src_दीर्घता=1;
            }
        }
        flush=सत्य;
    }

    /* we must not remember fallback matches when not using fallbacks */

    /* match input units until there is a full match or the input is consumed */
    प्रति(;;) {
        /* go to the next section */
        गन्तव्यम्_USection=गन्तव्यम्_UTable+idx;

        /* read first pair of the section */
        value=*गन्तव्यम्_USection++;
        length=UCNV_EXT_गन्तव्यम्_U_GET_BYTE(value);
        value=UCNV_EXT_गन्तव्यम्_U_GET_मानम्(value);
        यदि( value!=0 &&
            (UCNV_EXT_गन्तव्यम्_U_IS_ROUNDTRIP(value) ||
             गन्तव्यम्_U_USE_FALLBACK(useFallback)) &&
            UCNV_EXT_गन्तव्यम्_U_VERIFY_SISO_मेलनम्(sisoState, i+j)
        ) {
            /* remember longest match so far */
            मेलनम्_मानम्=value;
            मेलनम्_दीर्घता=i+j;
        }

        /* match pre[] then src[] */
        यदि(i<pre_दीर्घता) {
            b = static_cast<uint8_t>(pre[i++]);
        } अन्यथा यदि(j<src_दीर्घता) {
            b = static_cast<uint8_t>(src[j++]);
        } अन्यथा {
            /* all input consumed, partial match */
            यदि(flush || (length=(i+j))>UCNV_EXT_MAX_अष्टकानि) {
                /*
                 * end of the entire input stream, stop with the longest match so far
                 * or: partial match must not be longer than UCNV_EXT_MAX_BYTES
                 * because it must fit into state buffers
                 */
                भङ्ग;
            } अन्यथा {
                /* continue with more input next time */
                प्रत्यागम -length;
            }
        }

        /* search for the current char16_t */
        value=ucnv_ext_Find_गन्तव्यम्_U(गन्तव्यम्_USection, length, b);
        यदि(value==0) {
            /* no match here, stop with the longest match so far */
            भङ्ग;
        } अन्यथा {
            यदि(UCNV_EXT_गन्तव्यम्_U_IS_PARTIAL(value)) {
                /* partial match, continue */
                idx = static_cast<int32_t>(UCNV_EXT_गन्तव्यम्_U_GET_PARTIAL_सूचकांक(value));
            } अन्यथा {
                यदि( (UCNV_EXT_गन्तव्यम्_U_IS_ROUNDTRIP(value) ||
                     गन्तव्यम्_U_USE_FALLBACK(useFallback)) &&
                    UCNV_EXT_गन्तव्यम्_U_VERIFY_SISO_मेलनम्(sisoState, i+j)
                ) {
                    /* full match, stop with result */
                    मेलनम्_मानम्=value;
                    मेलनम्_दीर्घता=i+j;
                } अन्यथा {
                    /* full match on fallback not taken, stop with the longest match so far */
                }
                भङ्ग;
            }
        }
    }

    यदि(मेलनम्_दीर्घता==0) {
        /* no match at all */
        प्रत्यागम 0;
    }

    /* return result */
    *p_मेलनम्_मानम्=UCNV_EXT_गन्तव्यम्_U_आवरणम्_ROUNDTRIP(मेलनम्_मानम्);
    प्रत्यागम मेलनम्_दीर्घता;
}

स्थिर अन्तःस्थ शून्य
ucnv_ext_Write_गन्तव्यम्_U(UConverter *cnv, ध्रुव int32_t *cx,
                 uint32_t value,
                 char16_t **target, ध्रुव char16_t *target_सीमा,
                 int32_t **offsets, int32_t src_सूचकांक,
                 UError_संहिता *p_Error_संहिता) {
    /* output the result */
    यदि(UCNV_EXT_गन्तव्यम्_U_IS_संहिता_POINT(value)) {
        /* output a single code point */
        ucnv_गन्तव्यम्_UWrite_संहिता_Point(
            cnv, UCNV_EXT_गन्तव्यम्_U_GET_संहिता_POINT(value),
            target, target_सीमा,
            offsets, src_सूचकांक,
            p_Error_संहिता);
    } अन्यथा {
        /* output a string - with correct data we have resultLength>0 */
        ucnv_गन्तव्यम्_UWrite_UChars(
            cnv,
            UCNV_EXT_ARRAY(cx, UCNV_EXT_गन्तव्यम्_U_UCHARS_सूचकांक, char16_t)+
                UCNV_EXT_गन्तव्यम्_U_GET_सूचकांक(value),
            UCNV_EXT_गन्तव्यम्_U_GET_दीर्घता(value),
            target, target_सीमा,
            offsets, src_सूचकांक,
            p_Error_संहिता);
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
#परिभाषय UCNV_SISO_STATE(cnv) \
    ((cnv)->shared_दत्त->mbcs.outputType==MBCS_OUTPUT_2_SISO ? (int8_t)(cnv)->बहुलकम् : \
     (cnv)->shared_दत्त->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ? 1 : -1)

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_मेलनम्_गन्तव्यम्_U(UConverter *cnv, ध्रुव int32_t *cx,
                        int32_t first_दीर्घता,
                        ध्रुव वर्ण **src, ध्रुव वर्ण *src_सीमा,
                        char16_t **target, ध्रुव char16_t *target_सीमा,
                        int32_t **offsets, int32_t src_सूचकांक,
                        UBool flush,
                        UError_संहिता *p_Error_संहिता) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t मेलनम्;

    /* try to match */
    मेलनम्=ucnv_ext_मेलनम्_गन्तव्यम्_U(cx, (int8_t)UCNV_SISO_STATE(cnv),
                           (ध्रुव वर्ण *)cnv->गन्तव्यम्_UBytes, first_दीर्घता,
                           *src, (int32_t)(src_सीमा-*src),
                           &value,
                           cnv->useFallback, flush);
    यदि(मेलनम्>0) {
        /* advance src pointer for the consumed input */
        *src+=मेलनम्-first_दीर्घता;

        /* write result to target */
        ucnv_ext_Write_गन्तव्यम्_U(cnv, cx,
                         value,
                         target, target_सीमा,
                         offsets, src_सूचकांक,
                         p_Error_संहिता);
        प्रत्यागम सत्य;
    } अन्यथा यदि(मेलनम्<0) {
        /* save state for partial match */
        ध्रुव वर्ण *s;
        int32_t j;

        /* copy the first code point */
        s=(ध्रुव वर्ण *)cnv->गन्तव्यम्_UBytes;
        cnv->pre_गन्तव्यम्_UFirst_दीर्घता=(int8_t)first_दीर्घता;
        प्रति(j=0; j<first_दीर्घता; ++j) {
            cnv->pre_गन्तव्यम्_U[j]=*s++;
        }

        /* now copy the newly consumed input */
        s=*src;
        मेलनम्=-मेलनम्;
        प्रति(; j<मेलनम्; ++j) {
            cnv->pre_गन्तव्यम्_U[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_गन्तव्यम्_ULength=(int8_t)मेलनम्;
        प्रत्यागम सत्य;
    } अन्यथा /* match==0 no match */ {
        प्रत्यागम असत्य;
    }
}

U_CFUNC UChar32
ucnv_ext_Simple_मेलनम्_गन्तव्यम्_U(ध्रुव int32_t *cx,
                       ध्रुव वर्ण *source, int32_t length,
                       UBool useFallback) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t मेलनम्;

    यदि(length<=0) {
        प्रत्यागम 0xffff;
    }

    /* try to match */
    मेलनम्=ucnv_ext_मेलनम्_गन्तव्यम्_U(cx, -1,
                           source, length,
                           शून्यसूचकः, 0,
                           &value,
                           useFallback, सत्य);
    यदि(मेलनम्==length) {
        /* write result for simple, single-character conversion */
        यदि(UCNV_EXT_गन्तव्यम्_U_IS_संहिता_POINT(value)) {
            प्रत्यागम UCNV_EXT_गन्तव्यम्_U_GET_संहिता_POINT(value);
        }
    }

    /*
     * return no match because
     * - match>0 && value points to string: simple conversion cannot handle multiple code points
     * - match>0 && match!=length: not all input consumed, forbidden for this function
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    प्रत्यागम 0xfffe;
}

/*
 * continue partial match with new input
 * never called for simple, single-character conversion
 */
U_CFUNC शून्य
ucnv_ext_Continue_मेलनम्_गन्तव्यम्_U(UConverter *cnv,
                         UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः, int32_t src_सूचकांक,
                         UError_संहिता *p_Error_संहिता) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t मेलनम्, length;

    मेलनम्=ucnv_ext_मेलनम्_गन्तव्यम्_U(cnv->shared_दत्त->mbcs.extIndexes, (int8_t)UCNV_SISO_STATE(cnv),
                           cnv->pre_गन्तव्यम्_U, cnv->pre_गन्तव्यम्_ULength,
                           p_तर्काः->source, (int32_t)(p_तर्काः->source_सीमा-p_तर्काः->source),
                           &value,
                           cnv->useFallback, p_तर्काः->flush);
    यदि(मेलनम्>0) {
        यदि(मेलनम्>=cnv->pre_गन्तव्यम्_ULength) {
            /* advance src pointer for the consumed input */
            p_तर्काः->source+=मेलनम्-cnv->pre_गन्तव्यम्_ULength;
            cnv->pre_गन्तव्यम्_ULength=0;
        } अन्यथा {
            /* the match did not use all of preToU[] - keep the rest for replay */
            length=cnv->pre_गन्तव्यम्_ULength-मेलनम्;
            uprv_memmove(cnv->pre_गन्तव्यम्_U, cnv->pre_गन्तव्यम्_U+मेलनम्, length);
            cnv->pre_गन्तव्यम्_ULength=(int8_t)-length;
        }

        /* write result */
        ucnv_ext_Write_गन्तव्यम्_U(cnv, cnv->shared_दत्त->mbcs.extIndexes,
                         value,
                         &p_तर्काः->target, p_तर्काः->target_सीमा,
                         &p_तर्काः->offsets, src_सूचकांक,
                         p_Error_संहिता);
    } अन्यथा यदि(मेलनम्<0) {
        /* save state for partial match */
        ध्रुव वर्ण *s;
        int32_t j;

        /* just _append_ the newly consumed input to preToU[] */
        s=p_तर्काः->source;
        मेलनम्=-मेलनम्;
        प्रति(j=cnv->pre_गन्तव्यम्_ULength; j<मेलनम्; ++j) {
            cnv->pre_गन्तव्यम्_U[j]=*s++;
        }
        p_तर्काः->source=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->pre_गन्तव्यम्_ULength=(int8_t)मेलनम्;
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
        uprv_memcpy(cnv->गन्तव्यम्_UBytes, cnv->pre_गन्तव्यम्_U, cnv->pre_गन्तव्यम्_UFirst_दीर्घता);
        cnv->गन्तव्यम्_ULength=cnv->pre_गन्तव्यम्_UFirst_दीर्घता;

        /* move the rest up inside the buffer */
        length=cnv->pre_गन्तव्यम्_ULength-cnv->pre_गन्तव्यम्_UFirst_दीर्घता;
        यदि(length>0) {
            uprv_memmove(cnv->pre_गन्तव्यम्_U, cnv->pre_गन्तव्यम्_U+cnv->pre_गन्तव्यम्_UFirst_दीर्घता, length);
        }

        /* mark preToU for replay */
        cnv->pre_गन्तव्यम्_ULength=(int8_t)-length;

        /* set the error code for unassigned */
        *p_Error_संहिता=U_INVALID_CHAR_प्राप्तम्;
    }
}

/* from Unicode ------------------------------------------------------------- */

// Use roundtrips, "good one-way" mappings, and some normal fallbacks.
स्थिर अन्तःस्थ UBool
extFromUUseMapping(UBool useFallback, uint32_t value, UChar32 firstCP) {
    प्रत्यागम
        ((value&UCNV_EXT_FROM_U_स्थितिः_आवरणम्)!=0 ||
            FROM_U_USE_FALLBACK(useFallback, firstCP)) &&
        (value&UCNV_EXT_FROM_U_RESERVED_आवरणम्)==0;
}

/*
 * @return index of the char16_t, if found; else <0
 */
स्थिर अन्तःस्थ int32_t
ucnv_extFindFromU(ध्रुव char16_t *fromUSection, int32_t length, char16_t u) {
    int32_t i, start, सीमा;

    /* binary search */
    start=0;
    सीमा=length;
    प्रति(;;) {
        i=सीमा-start;
        यदि(i<=1) {
            भङ्ग; /*समाप्तम्*/
        }
        /* start<limit-1 */

        यदि(i<=4) {
            /* linear search for the last part */
            यदि(u<=fromUSection[start]) {
                भङ्ग;
            }
            यदि(++start<सीमा && u<=fromUSection[start]) {
                भङ्ग;
            }
            यदि(++start<सीमा && u<=fromUSection[start]) {
                भङ्ग;
            }
            /* always break at start==limit-1 */
            ++start;
            भङ्ग;
        }

        i=(start+सीमा)/2;
        यदि(u<fromUSection[i]) {
            सीमा=i;
        } अन्यथा {
            start=i;
        }
    }

    /* did we really find it? */
    यदि(start<सीमा && u==fromUSection[start]) {
        प्रत्यागम start;
    } अन्यथा {
        प्रत्यागम -1; /* not found */
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
स्थिर int32_t
ucnv_ext_मेलनम्_From_U(ध्रुव int32_t *cx,
                   UChar32 firstCP,
                   ध्रुव char16_t *pre, int32_t pre_दीर्घता,
                   ध्रुव char16_t *src, int32_t src_दीर्घता,
                   uint32_t *p_मेलनम्_मानम्,
                   UBool useFallback, UBool flush) {
    ध्रुव uint16_t *stage12, *stage3;
    ध्रुव uint32_t *stage3b;

    ध्रुव char16_t *fromUTableUChars, *fromUSectionUChars;
    ध्रुव uint32_t *from_UTable_मानानि, *from_USection_मानानि;

    uint32_t value, मेलनम्_मानम्;
    int32_t i, j, idx, length, मेलनम्_दीर्घता;
    char16_t c;

    यदि(cx==शून्यसूचकः) {
        प्रत्यागम 0; /* no extension data, no match */
    }

    /* trie lookup of firstCP */
    idx=firstCP>>10; /* stage 1 index */
    यदि(idx>=cx[UCNV_EXT_FROM_U_STAGE_1_दीर्घता]) {
        प्रत्यागम 0; /* the first code point is outside the trie */
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_सूचकांक, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_सूचकांक, uint16_t);
    idx=UCNV_EXT_FROM_U(stage12, stage3, idx, firstCP);

    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_सूचकांक, uint32_t);
    value=stage3b[idx];
    यदि(value==0) {
        प्रत्यागम 0;
    }

    /*
     * Tests for (value&UCNV_EXT_FROM_U_RESERVED_MASK)==0:
     * Do not interpret values with reserved bits used, for forward compatibility,
     * and do not even remember intermediate results with reserved bits used.
     */

    यदि(UCNV_EXT_गन्तव्यम्_U_IS_PARTIAL(value)) {
        /* partial match, enter the loop below */
        idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_सूचकांक(value));

        /* initialize */
        fromUTableUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_सूचकांक, char16_t);
        from_UTable_मानानि=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_मानानि_सूचकांक, uint32_t);

        मेलनम्_मानम्=0;
        i=j=मेलनम्_दीर्घता=0;

        /* we must not remember fallback matches when not using fallbacks */

        /* match input units until there is a full match or the input is consumed */
        प्रति(;;) {
            /* go to the next section */
            fromUSectionUChars=fromUTableUChars+idx;
            from_USection_मानानि=from_UTable_मानानि+idx;

            /* read first pair of the section */
            length=*fromUSectionUChars++;
            value=*from_USection_मानानि++;
            यदि(value!=0 && extFromUUseMapping(useFallback, value, firstCP)) {
                /* remember longest match so far */
                मेलनम्_मानम्=value;
                मेलनम्_दीर्घता=2+i+j;
            }

            /* match pre[] then src[] */
            यदि(i<pre_दीर्घता) {
                c=pre[i++];
            } अन्यथा यदि(j<src_दीर्घता) {
                c=src[j++];
            } अन्यथा {
                /* all input consumed, partial match */
                यदि(flush || (length=(i+j))>UCNV_EXT_MAX_UCHARS) {
                    /*
                     * end of the entire input stream, stop with the longest match so far
                     * or: partial match must not be longer than UCNV_EXT_MAX_UCHARS
                     * because it must fit into state buffers
                     */
                    भङ्ग;
                } अन्यथा {
                    /* continue with more input next time */
                    प्रत्यागम -(2+length);
                }
            }

            /* search for the current char16_t */
            idx=ucnv_extFindFromU(fromUSectionUChars, length, c);
            यदि(idx<0) {
                /* no match here, stop with the longest match so far */
                भङ्ग;
            } अन्यथा {
                value=from_USection_मानानि[idx];
                यदि(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                    /* partial match, continue */
                    idx = static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_सूचकांक(value));
                } अन्यथा {
                    यदि(extFromUUseMapping(useFallback, value, firstCP)) {
                        /* full match, stop with result */
                        मेलनम्_मानम्=value;
                        मेलनम्_दीर्घता=2+i+j;
                    } अन्यथा {
                        /* full match on fallback not taken, stop with the longest match so far */
                    }
                    भङ्ग;
                }
            }
        }

        यदि(मेलनम्_दीर्घता==0) {
            /* no match at all */
            प्रत्यागम 0;
        }
    } अन्यथा /* result from firstCP trie lookup */ {
        यदि(extFromUUseMapping(useFallback, value, firstCP)) {
            /* full match, stop with result */
            मेलनम्_मानम्=value;
            मेलनम्_दीर्घता=2;
        } अन्यथा {
            /* fallback not taken */
            प्रत्यागम 0;
        }
    }

    /* return result */
    यदि(मेलनम्_मानम्==UCNV_EXT_FROM_U_SUBCHAR1) {
        प्रत्यागम 1; /* assert matchLength==2 */
    }

    *p_मेलनम्_मानम्=मेलनम्_मानम्;
    प्रत्यागम मेलनम्_दीर्घता;
}

/*
 * @param value fromUnicode mapping table value; ignores roundtrip and reserved bits
 */
स्थिर अन्तःस्थ शून्य
ucnv_extWriteFromU(UConverter *cnv, ध्रुव int32_t *cx,
                   uint32_t value,
                   वर्ण **target, ध्रुव वर्ण *target_सीमा,
                   int32_t **offsets, int32_t src_सूचकांक,
                   UError_संहिता *p_Error_संहिता) {
    uint8_t बफर[1+UCNV_EXT_MAX_अष्टकानि];
    ध्रुव uint8_t *परिणाम;
    int32_t length, पूर्व_दीर्घता;

    length=UCNV_EXT_FROM_U_GET_दीर्घता(value);
    value = UCNV_EXT_FROM_U_GET_दत्त(value);

    /* output the result */
    यदि(length<=UCNV_EXT_FROM_U_MAX_प्रत्यक्षम्_दीर्घता) {
        /*
         * Generate a byte array and then write it below.
         * This is not the fastest possible way, but it should be ok for
         * extension mappings, and it is much simpler.
         * Offset and overflow handling are only done once this way.
         */
        uint8_t *p=बफर+1; /* reserve buffer[0] for shiftByte below */
        विकल्प(length) {
        पक्ष 3:
            *p++ = static_cast<uint8_t>(value >> 16);
            U_FALLTHROUGH;
        पक्ष 2:
            *p++ = static_cast<uint8_t>(value >> 8);
            U_FALLTHROUGH;
        पक्ष 1:
            *p++ = static_cast<uint8_t>(value);
            U_FALLTHROUGH;
        स्वभाव:
            भङ्ग; /* will never occur */
        }
        परिणाम=बफर+1;
    } अन्यथा {
        परिणाम=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_अष्टकानि_सूचकांक, uint8_t)+value;
    }

    /* with correct data we have length>0 */

    यदि((पूर्व_दीर्घता=cnv->from_Unicode_स्थितिः)!=0) {
        /* handle SI/SO stateful output */
        uint8_t सरणम्_Byte;

        यदि(पूर्व_दीर्घता>1 && length==1) {
            /* change from double-byte mode to single-byte */
            सरणम्_Byte = static_cast<uint8_t>(UCNV_SI);
            cnv->from_Unicode_स्थितिः=1;
        } अन्यथा यदि(पूर्व_दीर्घता==1 && length>1) {
            /* change from single-byte mode to double-byte */
            सरणम्_Byte = static_cast<uint8_t>(UCNV_SO);
            cnv->from_Unicode_स्थितिः=2;
        } अन्यथा {
            सरणम्_Byte=0;
        }

        यदि(सरणम्_Byte!=0) {
            /* prepend the shift byte to the result bytes */
            बफर[0]=सरणम्_Byte;
            यदि(परिणाम!=बफर+1) {
                uprv_memcpy(बफर+1, परिणाम, length);
            }
            परिणाम=बफर;
            ++length;
        }
    }

    ucnv_from_UWrite_अष्टकानि(cnv, reinterpret_cast<ध्रुव वर्ण*>(परिणाम), length,
                         target, target_सीमा,
                         offsets, src_सूचकांक,
                         p_Error_संहिता);
}

/*
 * target<targetLimit; set error code for overflow
 */
U_CFUNC UBool
ucnv_ext_Initial_मेलनम्_From_U(UConverter *cnv, ध्रुव int32_t *cx,
                          UChar32 cp,
                          ध्रुव char16_t **src, ध्रुव char16_t *src_सीमा,
                          वर्ण **target, ध्रुव वर्ण *target_सीमा,
                          int32_t **offsets, int32_t src_सूचकांक,
                          UBool flush,
                          UError_संहिता *p_Error_संहिता) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t मेलनम्;

    /* try to match */
    मेलनम्=ucnv_ext_मेलनम्_From_U(cx, cp,
                             शून्यसूचकः, 0,
                             *src, (int32_t)(src_सीमा-*src),
                             &value,
                             cnv->useFallback, flush);

    /* reject a match if the result is a single byte for DBCS-only */
    यदि( मेलनम्>=2 &&
        !(UCNV_EXT_FROM_U_GET_दीर्घता(value)==1 &&
          cnv->shared_दत्त->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY)
    ) {
        /* advance src pointer for the consumed input */
        *src+=मेलनम्-2; /* remove 2 for the initial code point */

        /* write result to target */
        ucnv_extWriteFromU(cnv, cx,
                           value,
                           target, target_सीमा,
                           offsets, src_सूचकांक,
                           p_Error_संहिता);
        प्रत्यागम सत्य;
    } अन्यथा यदि(मेलनम्<0) {
        /* save state for partial match */
        ध्रुव char16_t *s;
        int32_t j;

        /* copy the first code point */
        cnv->preFromUFirstCP=cp;

        /* now copy the newly consumed input */
        s=*src;
        मेलनम्=-मेलनम्-2; /* remove 2 for the initial code point */
        प्रति(j=0; j<मेलनम्; ++j) {
            cnv->preFromU[j]=*s++;
        }
        *src=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)मेलनम्;
        प्रत्यागम सत्य;
    } अन्यथा यदि(मेलनम्==1) {
        /* matched, no mapping but request for <subchar1> */
        cnv->useSubChar1=सत्य;
        प्रत्यागम असत्य;
    } अन्यथा /* match==0 no match */ {
        प्रत्यागम असत्य;
    }
}

/*
 * Used by ISO 2022 implementation.
 * @return number of bytes in *pValue; negative number if fallback; 0 for no mapping
 */
U_CFUNC int32_t
ucnv_ext_Simple_मेलनम्_From_U(ध्रुव int32_t *cx,
                         UChar32 cp, uint32_t *p_मानम्,
                         UBool useFallback) {
    uint32_t value;
    int32_t मेलनम्;

    /* try to match */
    मेलनम्=ucnv_ext_मेलनम्_From_U(cx,
                             cp,
                             शून्यसूचकः, 0,
                             शून्यसूचकः, 0,
                             &value,
                             useFallback, सत्य);
    यदि(मेलनम्>=2) {
        /* write result for simple, single-character conversion */
        int32_t length;
        पूर्णाङ्क isRoundtrip;

        isRoundtrip=UCNV_EXT_FROM_U_IS_ROUNDTRIP(value);
        length=UCNV_EXT_FROM_U_GET_दीर्घता(value);
        value = UCNV_EXT_FROM_U_GET_दत्त(value);

        यदि(length<=UCNV_EXT_FROM_U_MAX_प्रत्यक्षम्_दीर्घता) {
            *p_मानम्=value;
            प्रत्यागम isRoundtrip ? length : -length;
#यदि 0 /* not currently used */
        } अन्यथा यदि(length==4) {
            /* de-serialize a 4-byte result */
            ध्रुव uint8_t *परिणाम=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_अष्टकानि_सूचकांक, uint8_t)+value;
            *p_मानम्=
                ((uint32_t)परिणाम[0]<<24)|
                ((uint32_t)परिणाम[1]<<16)|
                ((uint32_t)परिणाम[2]<<8)|
                परिणाम[3];
            प्रत्यागम isRoundtrip ? 4 : -4;
#यदि_अन्तः
        }
    }

    /*
     * return no match because
     * - match>1 && resultLength>4: result too long for simple conversion
     * - match==1: no match found, <subchar1> preferred
     * - match==0: no match found in the first place
     * - match<0: partial match, not supported for simple conversion (and flush==true)
     */
    प्रत्यागम 0;
}

/*
 * continue partial match with new input, requires cnv->preFromUFirstCP>=0
 * never called for simple, single-character conversion
 */
U_CFUNC शून्य
ucnv_ext_Continue_मेलनम्_From_U(UConverter *cnv,
                           UConverter_From_Unicode_तर्काः *p_तर्काः, int32_t src_सूचकांक,
                           UError_संहिता *p_Error_संहिता) {
    uint32_t value = 0;  /* initialize output-only param to 0 to silence gcc */
    int32_t मेलनम्;

    मेलनम्=ucnv_ext_मेलनम्_From_U(cnv->shared_दत्त->mbcs.extIndexes,
                             cnv->preFromUFirstCP,
                             cnv->preFromU, cnv->preFromULength,
                             p_तर्काः->source, (int32_t)(p_तर्काः->source_सीमा-p_तर्काः->source),
                             &value,
                             cnv->useFallback, p_तर्काः->flush);
    यदि(मेलनम्>=2) {
        मेलनम्-=2; /* remove 2 for the initial code point */

        यदि(मेलनम्>=cnv->preFromULength) {
            /* advance src pointer for the consumed input */
            p_तर्काः->source+=मेलनम्-cnv->preFromULength;
            cnv->preFromULength=0;
        } अन्यथा {
            /* the match did not use all of preFromU[] - keep the rest for replay */
            int32_t length=cnv->preFromULength-मेलनम्;
            u_memmove(cnv->preFromU, cnv->preFromU+मेलनम्, length);
            cnv->preFromULength=(int8_t)-length;
        }

        /* finish the partial match */
        cnv->preFromUFirstCP=U_SENTINEL;

        /* write result */
        ucnv_extWriteFromU(cnv, cnv->shared_दत्त->mbcs.extIndexes,
                           value,
                           &p_तर्काः->target, p_तर्काः->target_सीमा,
                           &p_तर्काः->offsets, src_सूचकांक,
                           p_Error_संहिता);
    } अन्यथा यदि(मेलनम्<0) {
        /* save state for partial match */
        ध्रुव char16_t *s;
        int32_t j;

        /* just _append_ the newly consumed input to preFromU[] */
        s=p_तर्काः->source;
        मेलनम्=-मेलनम्-2; /* remove 2 for the initial code point */
        प्रति(j=cnv->preFromULength; j<मेलनम्; ++j) {
            U_ASSERT(j>=0);
            cnv->preFromU[j]=*s++;
        }
        p_तर्काः->source=s; /* same as *src=srcLimit; because we reached the end of input */
        cnv->preFromULength=(int8_t)मेलनम्;
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

        यदि(मेलनम्==1) {
            /* matched, no mapping but request for <subchar1> */
            cnv->useSubChar1=सत्य;
        }

        /* move the first code point to the error field */
        cnv->fromUChar32=cnv->preFromUFirstCP;
        cnv->preFromUFirstCP=U_SENTINEL;

        /* mark preFromU for replay */
        cnv->preFromULength=-cnv->preFromULength;

        /* set the error code for unassigned */
        *p_Error_संहिता=U_INVALID_CHAR_प्राप्तम्;
    }
}

स्थिर UBool
ext_समुच्चयः_Use_Mapping(UConverter_Unicode_समुच्चयः which, int32_t min_दीर्घता, uint32_t value) {
    यदि(which==UCNV_ROUNDTRIP_समुच्चयः) {
        // Add only code points for which the roundtrip flag is set.
        // Do not add any fallbacks, even if ucnv_fromUnicode() would use them
        // (fallbacks from PUA). See the API docs for ucnv_getUnicodeSet().
        //
        // By analogy, also do not add "good one-way" mappings.
        //
        // Do not add entries with reserved bits set.
        यदि(((value&(UCNV_EXT_FROM_U_ROUNDTRIP_FLAG|UCNV_EXT_FROM_U_RESERVED_आवरणम्))!=
                UCNV_EXT_FROM_U_ROUNDTRIP_FLAG)) {
            प्रत्यागम असत्य;
        }
    } अन्यथा /* UCNV_ROUNDTRIP_AND_FALLBACK_SET */ {
        // Do not add entries with reserved bits set.
        यदि((value&UCNV_EXT_FROM_U_RESERVED_आवरणम्)!=0) {
            प्रत्यागम असत्य;
        }
    }
    // Do not add <subchar1> entries or other (future?) pseudo-entries
    // with an output length of 0.
    प्रत्यागम UCNV_EXT_FROM_U_GET_दीर्घता(value)>=min_दीर्घता;
}

स्थिर शून्य
ucnv_ext_Get_Unicode_समुच्चयः_String(ध्रुव UConverter_Shared_दत्त *shared_दत्त,
                            ध्रुव int32_t *cx,
                            ध्रुव USetAdder *sa,
                            UConverter_Unicode_समुच्चयः which,
                            int32_t min_दीर्घता,
                            UChar32 firstCP,
                            char16_t s[UCNV_EXT_MAX_UCHARS], int32_t length,
                            int32_t section_सूचकांक,
                            UError_संहिता *p_Error_संहिता) {
    ध्रुव char16_t *fromUSectionUChars;
    ध्रुव uint32_t *from_USection_मानानि;

    uint32_t value;
    int32_t i, count;

    fromUSectionUChars=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_UCHARS_सूचकांक, char16_t)+section_सूचकांक;
    from_USection_मानानि=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_मानानि_सूचकांक, uint32_t)+section_सूचकांक;

    /* read first pair of the section */
    count=*fromUSectionUChars++;
    value=*from_USection_मानानि++;

    यदि(ext_समुच्चयः_Use_Mapping(which, min_दीर्घता, value)) {
        यदि(length==U16_दीर्घता(firstCP)) {
            /* add the initial code point */
            sa->सङ्कलय(sa->समुच्चयः, firstCP);
        } अन्यथा {
            /* add the string so far */
            sa->सङ्कलय_String(sa->समुच्चयः, s, length);
        }
    }

    प्रति(i=0; i<count; ++i) {
        /* append this code unit and recurse or add the string */
        s[length]=fromUSectionUChars[i];
        value=from_USection_मानानि[i];

        यदि(value==0) {
            /* no mapping, do nothing */
        } अन्यथा यदि(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
            ucnv_ext_Get_Unicode_समुच्चयः_String(
                shared_दत्त, cx, sa, which, min_दीर्घता,
                firstCP, s, length+1,
                static_cast<int32_t>(UCNV_EXT_FROM_U_GET_PARTIAL_सूचकांक(value)),
                p_Error_संहिता);
        } अन्यथा यदि(ext_समुच्चयः_Use_Mapping(which, min_दीर्घता, value)) {
            sa->सङ्कलय_String(sa->समुच्चयः, s, length+1);
        }
    }
}

U_CFUNC शून्य
ucnv_ext_Get_Unicode_समुच्चयः(ध्रुव UConverter_Shared_दत्त *shared_दत्त,
                      ध्रुव USetAdder *sa,
                      UConverter_Unicode_समुच्चयः which,
                      UConverter_समुच्चयः_Filter filter,
                      UError_संहिता *p_Error_संहिता) {
    ध्रुव int32_t *cx;
    ध्रुव uint16_t *stage12, *stage3, *ps2, *ps3;
    ध्रुव uint32_t *stage3b;

    uint32_t value;
    int32_t st1, stage1_दीर्घता, st2, st3, min_दीर्घता;

    char16_t s[UCNV_EXT_MAX_UCHARS];
    UChar32 c;
    int32_t length;

    cx=shared_दत्त->mbcs.extIndexes;
    यदि(cx==शून्यसूचकः) {
        प्रत्यागम;
    }

    stage12=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_12_सूचकांक, uint16_t);
    stage3=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_सूचकांक, uint16_t);
    stage3b=UCNV_EXT_ARRAY(cx, UCNV_EXT_FROM_U_STAGE_3_B_सूचकांक, uint32_t);

    stage1_दीर्घता=cx[UCNV_EXT_FROM_U_STAGE_1_दीर्घता];

    /* enumerate the from-Unicode trie table */
    c=0; /* keep track of the current code point while enumerating */

    यदि(filter==UCNV_समुच्चयः_FILTER_2022_CN) {
        min_दीर्घता=3;
    } अन्यथा यदि( shared_दत्त->mbcs.outputType==MBCS_OUTPUT_DBCS_ONLY ||
               filter!=UCNV_समुच्चयः_FILTER_NONE
    ) {
        /* DBCS-only, ignore single-byte results */
        min_दीर्घता=2;
    } अन्यथा {
        min_दीर्घता=1;
    }

    /*
     * the trie enumeration is almost the same as
     * in MBCSGetUnicodeSet() for MBCS_OUTPUT_1
     */
    प्रति(st1=0; st1<stage1_दीर्घता; ++st1) {
        st2=stage12[st1];
        यदि(st2>stage1_दीर्घता) {
            ps2=stage12+st2;
            प्रति(st2=0; st2<64; ++st2) {
                यदि((st3=(int32_t)ps2[st2]<<UCNV_EXT_STAGE_2_वामः_सरणम्)!=0) {
                    /* read the stage 3 block */
                    ps3=stage3+st3;

                    कुरु {
                        value=stage3b[*ps3++];
                        यदि(value==0) {
                            /* no mapping, do nothing */
                        } अन्यथा यदि(UCNV_EXT_FROM_U_IS_PARTIAL(value)) {
                            // Recurse for partial results.
                            length=0;
                            U16_APPEND_UNSAFE(s, length, c);
                            ucnv_ext_Get_Unicode_समुच्चयः_String(
                                shared_दत्त, cx, sa, which, min_दीर्घता,
                                c, s, length,
                                (int32_t)UCNV_EXT_FROM_U_GET_PARTIAL_सूचकांक(value),
                                p_Error_संहिता);
                        } अन्यथा यदि(ext_समुच्चयः_Use_Mapping(which, min_दीर्घता, value)) {
                            विकल्प(filter) {
                            पक्ष UCNV_समुच्चयः_FILTER_2022_CN:
                                यदि(!(UCNV_EXT_FROM_U_GET_दीर्घता(value)==3 && UCNV_EXT_FROM_U_GET_दत्त(value)<=0x82ffff)) {
                                    अनुवर्त;
                                }
                                भङ्ग;
                            पक्ष UCNV_समुच्चयः_FILTER_SJIS:
                                यदि(!(UCNV_EXT_FROM_U_GET_दीर्घता(value)==2 && (value=UCNV_EXT_FROM_U_GET_दत्त(value))>=0x8140 && value<=0xeffc)) {
                                    अनुवर्त;
                                }
                                भङ्ग;
                            पक्ष UCNV_समुच्चयः_FILTER_GR94_DBCS:
                                यदि(!(UCNV_EXT_FROM_U_GET_दीर्घता(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_दत्त(value))-0xa1a1)<=(0xfefe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    अनुवर्त;
                                }
                                भङ्ग;
                            पक्ष UCNV_समुच्चयः_FILTER_HZ:
                                यदि(!(UCNV_EXT_FROM_U_GET_दीर्घता(value)==2 &&
                                     (uint16_t)((value=UCNV_EXT_FROM_U_GET_दत्त(value))-0xa1a1)<=(0xfdfe - 0xa1a1) &&
                                     (uint8_t)(value-0xa1)<=(0xfe - 0xa1))) {
                                    अनुवर्त;
                                }
                                भङ्ग;
                            स्वभाव:
                                /*
                                 * UCNV_SET_FILTER_NONE,
                                 * or UCNV_SET_FILTER_DBCS_ONLY which is handled via minLength
                                 */
                                भङ्ग;
                            }
                            sa->सङ्कलय(sa->समुच्चयः, c);
                        }
                    } यावत्((++c&0xf)!=0);
                } अन्यथा {
                    c+=16; /* empty stage 3 block */
                }
            }
        } अन्यथा {
            c+=1024; /* empty stage 2 block */
        }
    }
}

#यदि_अन्तः /* #if !UCONFIG_NO_LEGACY_CONVERSION */
