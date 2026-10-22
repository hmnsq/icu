// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  ubidiln.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999aug06
*   created by: Markus W. Scherer, updated by Matitiahu Allouche
*/

#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/ubidi.h"
#अन्तर्भाव "ubidiimp.h"
#अन्तर्भाव "uassert.h"

/*
 * General remarks about the functions in this file:
 *
 * These functions deal with the aspects of potentially mixed-directional
 * text in a single paragraph or in a line of a single paragraph
 * which has already been processed according to
 * the Unicode 6.3 BiDi algorithm as defined in
 * https://www.unicode.org/reports/tr9/ , version 28,
 * also described in The Unicode Standard, Version 6.3.0 .
 *
 * This means that there is a UBiDi object with a levels
 * and a dirProps array.
 * paraLevel and direction are also set.
 * Only if the length of the text is zero, then levels==dirProps==nullptr.
 *
 * The overall directionality of the paragraph
 * or line is used to bypass the reordering steps if possible.
 * Even purely RTL text does not need reordering there because
 * the ubidi_getLogical/VisualIndex() functions can compute the
 * index on the fly in such a case.
 *
 * The implementation of the access to same-level-runs and of the reordering
 * do attempt to provide better performance and less memory usage compared to
 * a direct implementation of especially rule (L2) with an array of
 * one (32-bit) integer per text character.
 *
 * Here, the levels array is scanned as soon as necessary, and a vector of
 * same-level-runs is created. Reordering then is done on this vector.
 * For each run of text positions that were resolved to the same level,
 * only 8 bytes are stored: the first text position of the run and the visual
 * position behind the run after reordering.
 * One sign bit is used to hold the directionality of the run.
 * This is inefficient if there are many very short runs. If the average run
 * length is <2, then this uses more memory.
 *
 * In a further attempt to save memory, the levels array is never changed
 * after all the resolution rules (Xn, Wn, Nn, In).
 * Many functions have to consider the field trailingWSStart:
 * if it is less than length, then there is an implicit trailing run
 * at the paraLevel,
 * which is not reflected in the levels array.
 * This allows a line UBiDi object to use the same levels array as
 * its paragraph parent object.
 *
 * When a UBiDi object is created for a line of a paragraph, then the
 * paragraph's levels and dirProps arrays are reused by way of setting
 * a pointer into them, not by copying. This again saves memory and forbids to
 * change the now shared levels for (L1).
 */

/* handle trailing WS (L1) -------------------------------------------------- */

/*
 * setTrailingWSStart() sets the start index for a trailing
 * run of WS in the line. This is necessary because we do not modify
 * the paragraph's levels array that we just point into.
 * Using trailingWSStart is another form of performing (L1).
 *
 * To make subsequent operations easier, we also include the run
 * before the WS if it is at the paraLevel - we merge the two here.
 *
 * This function is called only from ubidi_setLine(), so pBiDi->paraLevel is
 * set correctly for the line even when contextual multiple paragraphs.
 */
स्थिर शून्य
समुच्चयः_Trailing_WSStart(UBiDi *pBiDi) {
    /* pBiDi->direction!=UBIDI_MIXED */

    ध्रुव DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *स्तराः=pBiDi->स्तराः;
    int32_t start=pBiDi->length;
    UBiDiLevel paraLevel=pBiDi->paraLevel;

    /* If the line is terminated by a block separator, all preceding WS etc...
       are already set to paragraph level.
       Setting trailingWSStart to pBidi->length will avoid changing the
       level of B chars from 0 to paraLevel in ubidi_getLevels when
       orderParagraphsLTR==true.
     */
    यदि(dirProps[start-1]==B) {
        pBiDi->trailingWSStart=start;   /* currently == pBiDi->length */
        प्रत्यागम;
    }
    /* go backwards across all WS, BN, explicit codes */
    यावत्(start>0 && DIRPROP_FLAG(dirProps[start-1])&आवरणम्_WS) {
        --start;
    }

    /* if the WS run can be merged with the previous run then do so here */
    यावत्(start>0 && स्तराः[start-1]==paraLevel) {
        --start;
    }

    pBiDi->trailingWSStart=start;
}

/* ubidi_setLine ------------------------------------------------------------ */

U_CAPI शून्य U_EXPORT2
ubidi_समुच्चयः_पङ्क्तिः(ध्रुव UBiDi *pParaBiDi,
              int32_t start, int32_t सीमा,
              UBiDi *p_पङ्क्तिः_Bi_Di,
              UError_संहिता *p_Error_संहिता) {
    int32_t length;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_संहिता);
    RETURN_VOID_IF_NOT_VALID_PARA(pParaBiDi, *p_Error_संहिता);
    RETURN_VOID_IF_BAD_RANGE(start, 0, सीमा, *p_Error_संहिता);
    RETURN_VOID_IF_BAD_RANGE(सीमा, 0, pParaBiDi->length+1, *p_Error_संहिता);
    यदि(p_पङ्क्तिः_Bi_Di==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    यदि(ubidi_getParagraph(pParaBiDi, start, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, p_Error_संहिता) !=
       ubidi_getParagraph(pParaBiDi, सीमा-1, शून्यसूचकः, शून्यसूचकः, शून्यसूचकः, p_Error_संहिता)) {
        /* the line crosses a paragraph boundary */
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    /* set the values in pLineBiDi from its pParaBiDi parent */
    p_पङ्क्तिः_Bi_Di->pParaBiDi=शून्यसूचकः;          /* mark unfinished setLine */
    p_पङ्क्तिः_Bi_Di->text=pParaBiDi->text+start;
    length=p_पङ्क्तिः_Bi_Di->length=सीमा-start;
    p_पङ्क्तिः_Bi_Di->परिणाम_दीर्घता=p_पङ्क्तिः_Bi_Di->original_दीर्घता=length;
    p_पङ्क्तिः_Bi_Di->paraLevel=GET_PARALEVEL(pParaBiDi, start);
    p_पङ्क्तिः_Bi_Di->para_गणनम्=pParaBiDi->para_गणनम्;
    p_पङ्क्तिः_Bi_Di->runs=शून्यसूचकः;
    p_पङ्क्तिः_Bi_Di->flags=0;
    p_पङ्क्तिः_Bi_Di->reordering_बहुलकम्=pParaBiDi->reordering_बहुलकम्;
    p_पङ्क्तिः_Bi_Di->reorderingOptions=pParaBiDi->reorderingOptions;
    p_पङ्क्तिः_Bi_Di->control_गणनम्=0;
    यदि(pParaBiDi->control_गणनम्>0) {
        int32_t j;
        प्रति(j=start; j<सीमा; j++) {
            यदि(IS_BIDI_CONTROL_CHAR(pParaBiDi->text[j])) {
                p_पङ्क्तिः_Bi_Di->control_गणनम्++;
            }
        }
        p_पङ्क्तिः_Bi_Di->परिणाम_दीर्घता-=p_पङ्क्तिः_Bi_Di->control_गणनम्;
    }

    p_पङ्क्तिः_Bi_Di->dirProps=pParaBiDi->dirProps+start;
    p_पङ्क्तिः_Bi_Di->स्तराः=pParaBiDi->स्तराः+start;
    p_पङ्क्तिः_Bi_Di->सततम्_गणनम्=-1;

    यदि(pParaBiDi->direction!=UBIDI_MIXED) {
        /* the parent is already trivial */
        p_पङ्क्तिः_Bi_Di->direction=pParaBiDi->direction;

        /*
         * The parent's levels are all either
         * implicitly or explicitly ==paraLevel;
         * do the same here.
         */
        यदि(pParaBiDi->trailingWSStart<=start) {
            p_पङ्क्तिः_Bi_Di->trailingWSStart=0;
        } अन्यथा यदि(pParaBiDi->trailingWSStart<सीमा) {
            p_पङ्क्तिः_Bi_Di->trailingWSStart=pParaBiDi->trailingWSStart-start;
        } अन्यथा {
            p_पङ्क्तिः_Bi_Di->trailingWSStart=length;
        }
    } अन्यथा {
        ध्रुव UBiDiLevel *स्तराः=p_पङ्क्तिः_Bi_Di->स्तराः;
        int32_t i, trailingWSStart;
        UBiDiLevel level;

        समुच्चयः_Trailing_WSStart(p_पङ्क्तिः_Bi_Di);
        trailingWSStart=p_पङ्क्तिः_Bi_Di->trailingWSStart;

        /* recalculate pLineBiDi->direction */
        यदि(trailingWSStart==0) {
            /* all levels are at paraLevel */
            p_पङ्क्तिः_Bi_Di->direction=(UBiDiDirection)(p_पङ्क्तिः_Bi_Di->paraLevel&1);
        } अन्यथा {
            /* get the level of the first character */
            level=(UBiDiLevel)(स्तराः[0]&1);

            /* if there is anything of a different level, then the line is mixed */
            यदि(trailingWSStart<length && (p_पङ्क्तिः_Bi_Di->paraLevel&1)!=level) {
                /* the trailing WS is at paraLevel, which differs from levels[0] */
                p_पङ्क्तिः_Bi_Di->direction=UBIDI_MIXED;
            } अन्यथा {
                /* see if levels[1..trailingWSStart-1] have the same direction as levels[0] and paraLevel */
                i=1;
                प्रति(;;) {
                    यदि(i==trailingWSStart) {
                        /* the direction values match those in level */
                        p_पङ्क्तिः_Bi_Di->direction=(UBiDiDirection)level;
                        भङ्ग;
                    } अन्यथा यदि((स्तराः[i]&1)!=level) {
                        p_पङ्क्तिः_Bi_Di->direction=UBIDI_MIXED;
                        भङ्ग;
                    }
                    ++i;
                }
            }
        }

        विकल्प(p_पङ्क्तिः_Bi_Di->direction) {
        पक्ष UBIDI_LTR:
            /* make sure paraLevel is even */
            p_पङ्क्तिः_Bi_Di->paraLevel=(UBiDiLevel)((p_पङ्क्तिः_Bi_Di->paraLevel+1)&~1);

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_पङ्क्तिः_Bi_Di->trailingWSStart=0;
            भङ्ग;
        पक्ष UBIDI_RTL:
            /* make sure paraLevel is odd */
            p_पङ्क्तिः_Bi_Di->paraLevel|=1;

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_पङ्क्तिः_Bi_Di->trailingWSStart=0;
            भङ्ग;
        स्वभाव:
            भङ्ग;
        }
    }
    p_पङ्क्तिः_Bi_Di->pParaBiDi=pParaBiDi;     /* mark successful setLine */
}

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getLevelAt(ध्रुव UBiDi *pBiDi, int32_t char_सूचकांक) {
    /* return paraLevel if in the trailing WS run, otherwise the real level */
    यदि(!IS_VALID_PARA_OR_पङ्क्तिः(pBiDi) || char_सूचकांक<0 || pBiDi->length<=char_सूचकांक) {
        प्रत्यागम 0;
    } अन्यथा यदि(pBiDi->direction!=UBIDI_MIXED || char_सूचकांक>=pBiDi->trailingWSStart) {
        प्रत्यागम GET_PARALEVEL(pBiDi, char_सूचकांक);
    } अन्यथा {
        प्रत्यागम pBiDi->स्तराः[char_सूचकांक];
    }
}

U_CAPI ध्रुव UBiDiLevel* U_EXPORT2
ubidi_get_स्तराः(UBiDi* pBiDi, UError_संहिता* p_Error_संहिता) {
    int32_t start, length;

    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_संहिता, शून्यसूचकः);
    RETURN_IF_NOT_VALID_PARA_OR_पङ्क्तिः(pBiDi, *p_Error_संहिता, शून्यसूचकः);
    यदि((length=pBiDi->length)<=0) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    यदि((start=pBiDi->trailingWSStart)==length) {
        /* the current levels array reflects the WS run */
        प्रत्यागम pBiDi->स्तराः;
    }

    /*
     * After the previous if(), we know that the levels array
     * has an implicit trailing WS run and therefore does not fully
     * reflect itself all the levels.
     * This must be a UBiDi object for a line, and
     * we need to create a new levels array.
     */
    यदि(get_स्तराः_स्मृतिः(pBiDi, length)) {
        UBiDiLevel *स्तराः=pBiDi->स्तराः_स्मृतिः;

        यदि(start>0 && स्तराः!=pBiDi->स्तराः) {
            uprv_memcpy(स्तराः, pBiDi->स्तराः, start);
        }
        /* pBiDi->paraLevel is ok even if contextual multiple paragraphs,
           since pBidi is a line object                                     */
        uprv_memset(स्तराः+start, pBiDi->paraLevel, length-start);

        /* this new levels array is set for the line and reflects the WS run */
        pBiDi->trailingWSStart=length;
        प्रत्यागम pBiDi->स्तराः=स्तराः;
    } अन्यथा {
        /* out of memory */
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
}

U_CAPI शून्य U_EXPORT2
ubidi_get_Logical_सततम्(ध्रुव UBiDi *pBiDi, int32_t logicalPosition,
                    int32_t *p_Logical_सीमा, UBiDiLevel *pLevel) {
    UError_संहिता error_संहिता;
    int32_t सततम्_गणनम्, visualStart, logical_सीमा, logicalFirst, i;
    सततम् i_सततम्;

    error_संहिता=U_ZERO_ERROR;
    RETURN_VOID_IF_BAD_RANGE(logicalPosition, 0, pBiDi->length, error_संहिता);
    /* ubidi_countRuns will check VALID_PARA_OR_LINE */
    सततम्_गणनम्=ubidi_गणनम्_Runs((UBiDi *)pBiDi, &error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    /* this is done based on runs rather than on levels since levels have
       a special interpretation when UBIDI_REORDER_RUNS_ONLY
     */
    visualStart=logical_सीमा=0;
    i_सततम्=pBiDi->runs[0];

    प्रति(i=0; i<सततम्_गणनम्; i++) {
        i_सततम् = pBiDi->runs[i];
        logicalFirst=GET_सूचकांक(i_सततम्.logicalStart);
        logical_सीमा=logicalFirst+i_सततम्.visual_सीमा-visualStart;
        यदि((logicalPosition>=logicalFirst) &&
           (logicalPosition<logical_सीमा)) {
            भङ्ग;
        }
        visualStart = i_सततम्.visual_सीमा;
    }
    यदि(p_Logical_सीमा) {
        *p_Logical_सीमा=logical_सीमा;
    }
    यदि(pLevel) {
        यदि(pBiDi->reordering_बहुलकम्==UBIDI_REORDER_RUNS_ONLY) {
            *pLevel=(UBiDiLevel)GET_ODD_BIT(i_सततम्.logicalStart);
        }
        अन्यथा यदि(pBiDi->direction!=UBIDI_MIXED || logicalPosition>=pBiDi->trailingWSStart) {
            *pLevel=GET_PARALEVEL(pBiDi, logicalPosition);
        } अन्यथा {
        *pLevel=pBiDi->स्तराः[logicalPosition];
        }
    }
}

/* runs API functions ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_गणनम्_Runs(UBiDi *pBiDi, UError_संहिता *p_Error_संहिता) {
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_संहिता, -1);
    RETURN_IF_NOT_VALID_PARA_OR_पङ्क्तिः(pBiDi, *p_Error_संहिता, -1);
    ubidi_getRuns(pBiDi, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम -1;
    }
    प्रत्यागम pBiDi->सततम्_गणनम्;
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_Visual_सततम्(UBiDi *pBiDi, int32_t सततम्_सूचकांक,
                   int32_t *pLogicalStart, int32_t *p_दीर्घता)
{
    int32_t start;
    UError_संहिता error_संहिता = U_ZERO_ERROR;
    RETURN_IF_NOT_VALID_PARA_OR_पङ्क्तिः(pBiDi, error_संहिता, UBIDI_LTR);
    ubidi_getRuns(pBiDi, &error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम UBIDI_LTR;
    }
    RETURN_IF_BAD_RANGE(सततम्_सूचकांक, 0, pBiDi->सततम्_गणनम्, error_संहिता, UBIDI_LTR);

    start=pBiDi->runs[सततम्_सूचकांक].logicalStart;
    यदि(pLogicalStart!=शून्यसूचकः) {
        *pLogicalStart=GET_सूचकांक(start);
    }
    यदि(p_दीर्घता!=शून्यसूचकः) {
        यदि(सततम्_सूचकांक>0) {
            *p_दीर्घता=pBiDi->runs[सततम्_सूचकांक].visual_सीमा-
                     pBiDi->runs[सततम्_सूचकांक-1].visual_सीमा;
        } अन्यथा {
            *p_दीर्घता=pBiDi->runs[0].visual_सीमा;
        }
    }
    प्रत्यागम (UBiDiDirection)GET_ODD_BIT(start);
}

/* in trivial cases there is only one trivial run; called by ubidi_getRuns() */
स्थिर शून्य
get_Single_सततम्(UBiDi *pBiDi, UBiDiLevel level) {
    /* simple, single-run case */
    pBiDi->runs=pBiDi->simpleRuns;
    pBiDi->सततम्_गणनम्=1;

    /* fill and reorder the single run */
    pBiDi->runs[0].logicalStart=MAKE_सूचकांक_ODD_PAIR(0, level);
    pBiDi->runs[0].visual_सीमा=pBiDi->length;
    pBiDi->runs[0].निवेशय_Remove=0;
}

/* reorder the runs array (L2) ---------------------------------------------- */

/*
 * Reorder the same-level runs in the runs array.
 * Here, runCount>1 and maxLevel>=minLevel>=paraLevel.
 * All the visualStart fields=logical start before reordering.
 * The "odd" bits are not set yet.
 *
 * Reordering with this data structure lends itself to some handy shortcuts:
 *
 * Since each run is moved but not modified, and since at the initial maxLevel
 * each sequence of same-level runs consists of only one run each, we
 * don't need to do anything there and can predecrement maxLevel.
 * In many simple cases, the reordering is thus done entirely in the
 * index mapping.
 * Also, reordering occurs only down to the lowest odd level that occurs,
 * which is minLevel|1. However, if the lowest level itself is odd, then
 * in the last reordering the sequence of the runs at this level or higher
 * will be all runs, and we don't need the elaborate loop to search for them.
 * This is covered by ++minLevel instead of minLevel|=1 followed
 * by an extra reorder-all after the reorder-some loop.
 * About a trailing WS run:
 * Such a run would need special treatment because its level is not
 * reflected in levels[] if this is not a paragraph object.
 * Instead, all characters from trailingWSStart on are implicitly at
 * paraLevel.
 * However, for all maxLevel>paraLevel, this run will never be reordered
 * and does not need to be taken into account. maxLevel==paraLevel is only reordered
 * if minLevel==paraLevel is odd, which is done in the extra segment.
 * This means that for the main reordering loop we don't need to consider
 * this run and can --runCount. If it is later part of the all-runs
 * reordering, then runCount is adjusted accordingly.
 */
स्थिर शून्य
reorder_पङ्क्तिः(UBiDi *pBiDi, UBiDiLevel minLevel, UBiDiLevel maxLevel) {
    सततम् *runs, अस्थायी_सततम्;
    UBiDiLevel *स्तराः;
    int32_t first_सततम्, अन्तः_सततम्, सीमा_सततम्, सततम्_गणनम्;

    /* nothing to do? */
    यदि(maxLevel<=(minLevel|1)) {
        प्रत्यागम;
    }

    /*
     * Reorder only down to the lowest odd level
     * and reorder at an odd minLevel in a separate, simpler loop.
     * See comments above for why minLevel is always incremented.
     */
    ++minLevel;

    runs=pBiDi->runs;
    स्तराः=pBiDi->स्तराः;
    सततम्_गणनम्=pBiDi->सततम्_गणनम्;

    /* do not include the WS run at paraLevel<=old minLevel except in the simple loop */
    यदि(pBiDi->trailingWSStart<pBiDi->length) {
        --सततम्_गणनम्;
    }

    यावत्(--maxLevel>=minLevel) {
        first_सततम्=0;

        /* loop for all sequences of runs */
        प्रति(;;) {
            /* look for a sequence of runs that are all at >=maxLevel */
            /* look for the first run of such a sequence */
            यावत्(first_सततम्<सततम्_गणनम् && स्तराः[runs[first_सततम्].logicalStart]<maxLevel) {
                ++first_सततम्;
            }
            यदि(first_सततम्>=सततम्_गणनम्) {
                भङ्ग;  /* no more such runs */
            }

            /* look for the limit run of such a sequence (the run behind it) */
            प्रति(सीमा_सततम्=first_सततम्; ++सीमा_सततम्<सततम्_गणनम् && स्तराः[runs[सीमा_सततम्].logicalStart]>=maxLevel;) {}

            /* Swap the entire sequence of runs from firstRun to limitRun-1. */
            अन्तः_सततम्=सीमा_सततम्-1;
            यावत्(first_सततम्<अन्तः_सततम्) {
                अस्थायी_सततम् = runs[first_सततम्];
                runs[first_सततम्]=runs[अन्तः_सततम्];
                runs[अन्तः_सततम्]=अस्थायी_सततम्;
                ++first_सततम्;
                --अन्तः_सततम्;
            }

            यदि(सीमा_सततम्==सततम्_गणनम्) {
                भङ्ग;  /* no more such runs */
            } अन्यथा {
                first_सततम्=सीमा_सततम्+1;
            }
        }
    }

    /* now do maxLevel==old minLevel (==odd!), see above */
    यदि(!(minLevel&1)) {
        first_सततम्=0;

        /* include the trailing WS run in this complete reordering */
        यदि(pBiDi->trailingWSStart==pBiDi->length) {
            --सततम्_गणनम्;
        }

        /* Swap the entire sequence of all runs. (endRun==runCount) */
        यावत्(first_सततम्<सततम्_गणनम्) {
            अस्थायी_सततम्=runs[first_सततम्];
            runs[first_सततम्]=runs[सततम्_गणनम्];
            runs[सततम्_गणनम्]=अस्थायी_सततम्;
            ++first_सततम्;
            --सततम्_गणनम्;
        }
    }
}

/* compute the runs array --------------------------------------------------- */

स्थिर int32_t get_सततम्_From_Logical_सूचकांक(UBiDi *pBiDi, int32_t logical_सूचकांक) {
    सततम् *runs=pBiDi->runs;
    int32_t सततम्_गणनम्=pBiDi->सततम्_गणनम्, visualStart=0, i, length, logicalStart;

    प्रति(i=0; i<सततम्_गणनम्; i++) {
        length=runs[i].visual_सीमा-visualStart;
        logicalStart=GET_सूचकांक(runs[i].logicalStart);
        यदि((logical_सूचकांक>=logicalStart) && (logical_सूचकांक<(logicalStart+length))) {
            प्रत्यागम i;
        }
        visualStart+=length;
    }
    /* we should never get here */
    UPRV_UNREACHABLE_EXIT;
}

/*
 * Compute the runs array from the levels array.
 * After ubidi_getRuns() returns true, runCount is guaranteed to be >0
 * and the runs are reordered.
 * Odd-level runs have visualStart on their visual right edge and
 * they progress visually to the left.
 * If option UBIDI_OPTION_INSERT_MARKS is set, insertRemove will contain the
 * sum of appropriate LRM/RLM_BEFORE/AFTER flags.
 * If option UBIDI_OPTION_REMOVE_CONTROLS is set, insertRemove will contain the
 * negative number of BiDi control characters within this run.
 */
U_CFUNC UBool
ubidi_getRuns(UBiDi *pBiDi, UError_संहिता* p_Error_संहिता) {
    /*
     * This method returns immediately if the runs are already set. This
     * includes the case of length==0 (handled in setPara)..
     */
    यदि (pBiDi->सततम्_गणनम्>=0) {
        प्रत्यागम सत्य;
    }

    यदि(pBiDi->direction!=UBIDI_MIXED) {
        /* simple, single-run case - this covers length==0 */
        /* pBiDi->paraLevel is ok even for contextual multiple paragraphs */
        get_Single_सततम्(pBiDi, pBiDi->paraLevel);
    } अन्यथा /* UBIDI_MIXED, length>0 */ {
        /* mixed directionality */
        int32_t length=pBiDi->length, सीमा;
        UBiDiLevel *स्तराः=pBiDi->स्तराः;
        int32_t i, सततम्_गणनम्;
        UBiDiLevel level=UBIDI_DEFAULT_LTR;   /* initialize with no valid level */
        /*
         * If there are WS characters at the end of the line
         * and the run preceding them has a level different from
         * paraLevel, then they will form their own run at paraLevel (L1).
         * Count them separately.
         * We need some special treatment for this in order to not
         * modify the levels array which a line UBiDi object shares
         * with its paragraph parent and its other line siblings.
         * In other words, for the trailing WS, it may be
         * levels[]!=paraLevel but we have to treat it like it were so.
         */
        सीमा=pBiDi->trailingWSStart;
        /* count the runs, there is at least one non-WS run, and limit>0 */
        सततम्_गणनम्=0;
        प्रति(i=0; i<सीमा; ++i) {
            /* increment runCount at the start of each run */
            यदि(स्तराः[i]!=level) {
                ++सततम्_गणनम्;
                level=स्तराः[i];
            }
        }

        /*
         * We don't need to see if the last run can be merged with a trailing
         * WS run because setTrailingWSStart() would have done that.
         */
        यदि(सततम्_गणनम्==1 && सीमा==length) {
            /* There is only one non-WS run and no trailing WS-run. */
            get_Single_सततम्(pBiDi, स्तराः[0]);
        } अन्यथा /* runCount>1 || limit<length */ {
            /* allocate and set the runs */
            सततम् *runs;
            int32_t सततम्_सूचकांक, start;
            UBiDiLevel minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1, maxLevel=0;

            /* now, count a (non-mergeable) WS run */
            यदि(सीमा<length) {
                ++सततम्_गणनम्;
            }

            /* runCount>1 */
            यदि(get_Runs_स्मृतिः(pBiDi, सततम्_गणनम्)) {
                runs=pBiDi->runs_स्मृतिः;
            } अन्यथा {
                *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम असत्य;
            }

            /* set the runs */
            /* FOOD FOR THOUGHT: this could be optimized, e.g.:
             * 464->444, 484->444, 575->555, 595->555
             * However, that would take longer. Check also how it would
             * interact with BiDi control removal and inserting Marks.
             */
            सततम्_सूचकांक=0;

            /* search for the run limits and initialize visualLimit values with the run lengths */
            i=0;
            कुरु {
                /* prepare this run */
                start=i;
                level=स्तराः[i];
                यदि(level<minLevel) {
                    minLevel=level;
                }
                यदि(level>maxLevel) {
                    maxLevel=level;
                }

                /* look for the run limit */
                यावत्(++i<सीमा && स्तराः[i]==level) {}

                /* i is another run limit */
                runs[सततम्_सूचकांक].logicalStart=start;
                runs[सततम्_सूचकांक].visual_सीमा=i-start;
                runs[सततम्_सूचकांक].निवेशय_Remove=0;
                ++सततम्_सूचकांक;
            } यावत्(i<सीमा);

            यदि(सीमा<length) {
                /* there is a separate WS run */
                runs[सततम्_सूचकांक].logicalStart=सीमा;
                runs[सततम्_सूचकांक].visual_सीमा=length-सीमा;
                /* For the trailing WS run, pBiDi->paraLevel is ok even
                   if contextual multiple paragraphs.                   */
                यदि(pBiDi->paraLevel<minLevel) {
                    minLevel=pBiDi->paraLevel;
                }
            }

            /* set the object fields */
            pBiDi->runs=runs;
            pBiDi->सततम्_गणनम्=सततम्_गणनम्;

            reorder_पङ्क्तिः(pBiDi, minLevel, maxLevel);

            /* now add the direction flags and adjust the visualLimit's to be just that */
            /* this loop will also handle the trailing WS run */
            सीमा=0;
            प्रति(i=0; i<सततम्_गणनम्; ++i) {
                सङ्कलय_ODD_BIT_FROM_LEVEL(runs[i].logicalStart, स्तराः[runs[i].logicalStart]);
                सीमा+=runs[i].visual_सीमा;
                runs[i].visual_सीमा=सीमा;
            }

            /* Set the "odd" bit for the trailing WS run. */
            /* For a RTL paragraph, it will be the *first* run in visual order. */
            /* For the trailing WS run, pBiDi->paraLevel is ok even if
               contextual multiple paragraphs.                          */
            यदि(सततम्_सूचकांक<सततम्_गणनम्) {
                int32_t trailing_सततम् = ((pBiDi->paraLevel & 1) != 0)? 0 : सततम्_सूचकांक;

                सङ्कलय_ODD_BIT_FROM_LEVEL(runs[trailing_सततम्].logicalStart, pBiDi->paraLevel);
            }
        }
    }

    /* handle insert LRM/RLM BEFORE/AFTER run */
    यदि(pBiDi->निवेशय_Points.size>0) {
        Point *point, *start=pBiDi->निवेशय_Points.points,
                      *सीमा=start+pBiDi->निवेशय_Points.size;
        int32_t सततम्_सूचकांक;
        प्रति(point=start; point<सीमा; point++) {
            सततम्_सूचकांक=get_सततम्_From_Logical_सूचकांक(pBiDi, point->pos);
            pBiDi->runs[सततम्_सूचकांक].निवेशय_Remove|=point->flag;
        }
    }

    /* handle remove BiDi control characters */
    यदि(pBiDi->control_गणनम्>0) {
        int32_t सततम्_सूचकांक;
        ध्रुव char16_t *start=pBiDi->text, *सीमा=start+pBiDi->length, *pu;
        प्रति(pu=start; pu<सीमा; pu++) {
            यदि(IS_BIDI_CONTROL_CHAR(*pu)) {
                सततम्_सूचकांक=get_सततम्_From_Logical_सूचकांक(pBiDi, (int32_t)(pu-start));
                pBiDi->runs[सततम्_सूचकांक].निवेशय_Remove--;
            }
        }
    }

    प्रत्यागम सत्य;
}

स्थिर UBool
prepareReorder(ध्रुव UBiDiLevel *स्तराः, int32_t length,
               int32_t *सूचकांक_Map,
               UBiDiLevel *pMinLevel, UBiDiLevel *pMaxLevel) {
    int32_t start;
    UBiDiLevel level, minLevel, maxLevel;

    यदि(स्तराः==शून्यसूचकः || length<=0) {
        प्रत्यागम असत्य;
    }

    /* determine minLevel and maxLevel */
    minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1;
    maxLevel=0;
    प्रति(start=length; start>0;) {
        level=स्तराः[--start];
        यदि(level>UBIDI_MAX_EXPLICIT_LEVEL+1) {
            प्रत्यागम असत्य;
        }
        यदि(level<minLevel) {
            minLevel=level;
        }
        यदि(level>maxLevel) {
            maxLevel=level;
        }
    }
    *pMinLevel=minLevel;
    *pMaxLevel=maxLevel;

    /* initialize the index map */
    प्रति(start=length; start>0;) {
        --start;
        सूचकांक_Map[start]=start;
    }

    प्रत्यागम सत्य;
}

/* reorder a line based on a levels array (L2) ------------------------------ */

U_CAPI शून्य U_EXPORT2
ubidi_reorderLogical(ध्रुव UBiDiLevel *स्तराः, int32_t length, int32_t *सूचकांक_Map) {
    int32_t start, सीमा, योग_सम्बन्धि_Sos_Eos;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    यदि(सूचकांक_Map==शून्यसूचकः || !prepareReorder(स्तराः, length, सूचकांक_Map, &minLevel, &maxLevel)) {
        प्रत्यागम;
    }

    /* nothing to do? */
    यदि(minLevel==maxLevel && (minLevel&1)==0) {
        प्रत्यागम;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    कुरु {
        start=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        प्रति(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            यावत्(start<length && स्तराः[start]<maxLevel) {
                ++start;
            }
            यदि(start>=length) {
                भङ्ग;  /* no more such sequences */
            }

            /* look for the limit of such a sequence (the index behind it) */
            प्रति(सीमा=start; ++सीमा<length && स्तराः[सीमा]>=maxLevel;) {}

            /*
             * sos=start of sequence, eos=end of sequence
             *
             * The closed (inclusive) interval from sos to eos includes all the logical
             * and visual indexes within this sequence. They are logically and
             * visually contiguous and in the same range.
             *
             * For each run, the new visual index=sos+eos-old visual index;
             * we pre-add sos+eos into sumOfSosEos ->
             * new visual index=sumOfSosEos-old visual index;
             */
            योग_सम्बन्धि_Sos_Eos=start+सीमा-1;

            /* reorder each index in the sequence */
            कुरु {
                सूचकांक_Map[start]=योग_सम्बन्धि_Sos_Eos-सूचकांक_Map[start];
            } यावत्(++start<सीमा);

            /* start==limit */
            यदि(सीमा==length) {
                भङ्ग;  /* no more such sequences */
            } अन्यथा {
                start=सीमा+1;
            }
        }
    } यावत्(--maxLevel>=minLevel);
}

U_CAPI शून्य U_EXPORT2
ubidi_reorderVisual(ध्रुव UBiDiLevel *स्तराः, int32_t length, int32_t *सूचकांक_Map) {
    int32_t start, अन्तः, सीमा, अस्थायी;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    यदि(सूचकांक_Map==शून्यसूचकः || !prepareReorder(स्तराः, length, सूचकांक_Map, &minLevel, &maxLevel)) {
        प्रत्यागम;
    }

    /* nothing to do? */
    यदि(minLevel==maxLevel && (minLevel&1)==0) {
        प्रत्यागम;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    कुरु {
        start=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        प्रति(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            यावत्(start<length && स्तराः[start]<maxLevel) {
                ++start;
            }
            यदि(start>=length) {
                भङ्ग;  /* no more such runs */
            }

            /* look for the limit of such a sequence (the index behind it) */
            प्रति(सीमा=start; ++सीमा<length && स्तराः[सीमा]>=maxLevel;) {}

            /*
             * Swap the entire interval of indexes from start to limit-1.
             * We don't need to swap the levels for the purpose of this
             * algorithm: the sequence of levels that we look at does not
             * move anyway.
             */
            अन्तः=सीमा-1;
            यावत्(start<अन्तः) {
                अस्थायी=सूचकांक_Map[start];
                सूचकांक_Map[start]=सूचकांक_Map[अन्तः];
                सूचकांक_Map[अन्तः]=अस्थायी;

                ++start;
                --अन्तः;
            }

            यदि(सीमा==length) {
                भङ्ग;  /* no more such sequences */
            } अन्यथा {
                start=सीमा+1;
            }
        }
    } यावत्(--maxLevel>=minLevel);
}

/* API functions for logical<->visual mapping ------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_get_Visual_सूचकांक(UBiDi *pBiDi, int32_t logical_सूचकांक, UError_संहिता *p_Error_संहिता) {
    int32_t visual_सूचकांक=UBIDI_MAP_NOWHERE;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_संहिता, -1);
    RETURN_IF_NOT_VALID_PARA_OR_पङ्क्तिः(pBiDi, *p_Error_संहिता, -1);
    RETURN_IF_BAD_RANGE(logical_सूचकांक, 0, pBiDi->length, *p_Error_संहिता, -1);

    /* we can do the trivial cases without the runs array */
    विकल्प(pBiDi->direction) {
    पक्ष UBIDI_LTR:
        visual_सूचकांक=logical_सूचकांक;
        भङ्ग;
    पक्ष UBIDI_RTL:
        visual_सूचकांक=pBiDi->length-logical_सूचकांक-1;
        भङ्ग;
    स्वभाव:
        यदि(!ubidi_getRuns(pBiDi, p_Error_संहिता)) {
            *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम -1;
        } अन्यथा {
            सततम् *runs=pBiDi->runs;
            int32_t i, visualStart=0, विस्थापनम्, length;

            /* linear search for the run, search on the visual runs */
            प्रति(i=0; i<pBiDi->सततम्_गणनम्; ++i) {
                length=runs[i].visual_सीमा-visualStart;
                विस्थापनम्=logical_सूचकांक-GET_सूचकांक(runs[i].logicalStart);
                यदि(विस्थापनम्>=0 && विस्थापनम्<length) {
                    यदि(IS_EVEN_सततम्(runs[i].logicalStart)) {
                        /* LTR */
                        visual_सूचकांक=visualStart+विस्थापनम्;
                    } अन्यथा {
                        /* RTL */
                        visual_सूचकांक=visualStart+length-विस्थापनम्-1;
                    }
                    भङ्ग;          /* exit for loop */
                }
                visualStart+=length;
            }
            यदि(i>=pBiDi->सततम्_गणनम्) {
                प्रत्यागम UBIDI_MAP_NOWHERE;
            }
        }
    }

    यदि(pBiDi->निवेशय_Points.size>0) {
        /* add the number of added marks until the calculated visual index */
        सततम् *runs=pBiDi->runs;
        int32_t i, length, निवेशय_Remove;
        int32_t visualStart=0, mark_प्राप्तम्=0;
        प्रति(i=0; ; i++, visualStart+=length) {
            length=runs[i].visual_सीमा-visualStart;
            निवेशय_Remove=runs[i].निवेशय_Remove;
            यदि(निवेशय_Remove & (LRM_BEFORE|RLM_BEFORE)) {
                mark_प्राप्तम्++;
            }
            /* is it the run containing the visual index? */
            यदि(visual_सूचकांक<runs[i].visual_सीमा) {
                प्रत्यागम visual_सूचकांक+mark_प्राप्तम्;
            }
            यदि(निवेशय_Remove & (LRM_AFTER|RLM_AFTER)) {
                mark_प्राप्तम्++;
            }
        }
    }
    अन्यथा यदि(pBiDi->control_गणनम्>0) {
        /* subtract the number of controls until the calculated visual index */
        सततम् *runs=pBiDi->runs;
        int32_t i, j, start, सीमा, length, निवेशय_Remove;
        int32_t visualStart=0, control_प्राप्तम्=0;
        char16_t uchar=pBiDi->text[logical_सूचकांक];
        /* is the logical index pointing to a control ? */
        यदि(IS_BIDI_CONTROL_CHAR(uchar)) {
            प्रत्यागम UBIDI_MAP_NOWHERE;
        }
        /* loop on runs */
        प्रति(i=0; ; i++, visualStart+=length) {
            length=runs[i].visual_सीमा-visualStart;
            निवेशय_Remove=runs[i].निवेशय_Remove;
            /* calculated visual index is beyond this run? */
            यदि(visual_सूचकांक>=runs[i].visual_सीमा) {
                control_प्राप्तम्-=निवेशय_Remove;
                अनुवर्त;
            }
            /* calculated visual index must be within current run */
            यदि(निवेशय_Remove==0) {
                प्रत्यागम visual_सूचकांक-control_प्राप्तम्;
            }
            यदि(IS_EVEN_सततम्(runs[i].logicalStart)) {
                /* LTR: check from run start to logical index */
                start=runs[i].logicalStart;
                सीमा=logical_सूचकांक;
            } अन्यथा {
                /* RTL: check from logical index to run end */
                start=logical_सूचकांक+1;
                सीमा=GET_सूचकांक(runs[i].logicalStart)+length;
            }
            प्रति(j=start; j<सीमा; j++) {
                uchar=pBiDi->text[j];
                यदि(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_प्राप्तम्++;
                }
            }
            प्रत्यागम visual_सूचकांक-control_प्राप्तम्;
        }
    }

    प्रत्यागम visual_सूचकांक;
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Logical_सूचकांक(UBiDi *pBiDi, int32_t visual_सूचकांक, UError_संहिता *p_Error_संहिता) {
    सततम् *runs;
    int32_t i, सततम्_गणनम्, start;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_संहिता, -1);
    RETURN_IF_NOT_VALID_PARA_OR_पङ्क्तिः(pBiDi, *p_Error_संहिता, -1);
    RETURN_IF_BAD_RANGE(visual_सूचकांक, 0, pBiDi->परिणाम_दीर्घता, *p_Error_संहिता, -1);
    /* we can do the trivial cases without the runs array */
    यदि(pBiDi->निवेशय_Points.size==0 && pBiDi->control_गणनम्==0) {
        यदि(pBiDi->direction==UBIDI_LTR) {
            प्रत्यागम visual_सूचकांक;
        }
        अन्यथा यदि(pBiDi->direction==UBIDI_RTL) {
            प्रत्यागम pBiDi->length-visual_सूचकांक-1;
        }
    }
    यदि(!ubidi_getRuns(pBiDi, p_Error_संहिता)) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम -1;
    }

    runs=pBiDi->runs;
    सततम्_गणनम्=pBiDi->सततम्_गणनम्;
    यदि(pBiDi->निवेशय_Points.size>0) {
        /* handle inserted LRM/RLM */
        int32_t mark_प्राप्तम्=0, निवेशय_Remove;
        int32_t visualStart=0, length;
        runs=pBiDi->runs;
        /* subtract number of marks until visual index */
        प्रति(i=0; ; i++, visualStart+=length) {
            length=runs[i].visual_सीमा-visualStart;
            निवेशय_Remove=runs[i].निवेशय_Remove;
            यदि(निवेशय_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                यदि(visual_सूचकांक<=(visualStart+mark_प्राप्तम्)) {
                    प्रत्यागम UBIDI_MAP_NOWHERE;
                }
                mark_प्राप्तम्++;
            }
            /* is adjusted visual index within this run? */
            यदि(visual_सूचकांक<(runs[i].visual_सीमा+mark_प्राप्तम्)) {
                visual_सूचकांक-=mark_प्राप्तम्;
                भङ्ग;
            }
            यदि(निवेशय_Remove&(LRM_AFTER|RLM_AFTER)) {
                यदि(visual_सूचकांक==(visualStart+length+mark_प्राप्तम्)) {
                    प्रत्यागम UBIDI_MAP_NOWHERE;
                }
                mark_प्राप्तम्++;
            }
        }
    }
    अन्यथा यदि(pBiDi->control_गणनम्>0) {
        /* handle removed BiDi control characters */
        int32_t control_प्राप्तम्=0, निवेशय_Remove, length;
        int32_t logicalStart, logical_अन्तः, visualStart=0, j, k;
        char16_t uchar;
        UBool even_सततम्;
        /* add number of controls until visual index */
        प्रति(i=0; ; i++, visualStart+=length) {
            length=runs[i].visual_सीमा-visualStart;
            निवेशय_Remove=runs[i].निवेशय_Remove;
            /* is adjusted visual index beyond current run? */
            यदि(visual_सूचकांक>=(runs[i].visual_सीमा-control_प्राप्तम्+निवेशय_Remove)) {
                control_प्राप्तम्-=निवेशय_Remove;
                अनुवर्त;
            }
            /* adjusted visual index is within current run */
            यदि(निवेशय_Remove==0) {
                visual_सूचकांक+=control_प्राप्तम्;
                भङ्ग;
            }
            /* count non-control chars until visualIndex */
            logicalStart=runs[i].logicalStart;
            even_सततम्=IS_EVEN_सततम्(logicalStart);
            REMOVE_ODD_BIT(logicalStart);
            logical_अन्तः=logicalStart+length-1;
            प्रति(j=0; j<length; j++) {
                k= even_सततम् ? logicalStart+j : logical_अन्तः-j;
                uchar=pBiDi->text[k];
                यदि(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_प्राप्तम्++;
                }
                यदि((visual_सूचकांक+control_प्राप्तम्)==(visualStart+j)) {
                    भङ्ग;
                }
            }
            visual_सूचकांक+=control_प्राप्तम्;
            भङ्ग;
        }
    }
    /* handle all cases */
    यदि(सततम्_गणनम्<=10) {
        /* linear search for the run */
        प्रति(i=0; visual_सूचकांक>=runs[i].visual_सीमा; ++i) {}
    } अन्यथा {
        /* binary search for the run */
        int32_t begin=0, सीमा=सततम्_गणनम्;

        /* the middle if() is guaranteed to find the run, we don't need a loop limit */
        प्रति(;;) {
            i=(begin+सीमा)/2;
            यदि(visual_सूचकांक>=runs[i].visual_सीमा) {
                begin=i+1;
            } अन्यथा यदि(i==0 || visual_सूचकांक>=runs[i-1].visual_सीमा) {
                भङ्ग;
            } अन्यथा {
                सीमा=i;
            }
        }
    }

    start=runs[i].logicalStart;
    यदि(IS_EVEN_सततम्(start)) {
        /* LTR */
        /* the offset in runs[i] is visualIndex-runs[i-1].visualLimit */
        यदि(i>0) {
            visual_सूचकांक-=runs[i-1].visual_सीमा;
        }
        प्रत्यागम start+visual_सूचकांक;
    } अन्यथा {
        /* RTL */
        प्रत्यागम GET_सूचकांक(start)+runs[i].visual_सीमा-visual_सूचकांक-1;
    }
}

U_CAPI शून्य U_EXPORT2
ubidi_getLogicalMap(UBiDi *pBiDi, int32_t *सूचकांक_Map, UError_संहिता *p_Error_संहिता) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_संहिता);
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_गणनम्_Runs(pBiDi, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        /* no op */
    } अन्यथा यदि(सूचकांक_Map==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
    } अन्यथा {
        /* fill a logical-to-visual index map using the runs[] */
        int32_t visualStart, visual_सीमा, i, j, k;
        int32_t logicalStart, logical_सीमा;
        सततम् *runs=pBiDi->runs;
        यदि (pBiDi->length<=0) {
            प्रत्यागम;
        }
        यदि (pBiDi->length>pBiDi->परिणाम_दीर्घता) {
            uprv_memset(सूचकांक_Map, 0xFF, pBiDi->length*परिमाण(int32_t));
        }

        visualStart=0;
        प्रति(j=0; j<pBiDi->सततम्_गणनम्; ++j) {
            logicalStart=GET_सूचकांक(runs[j].logicalStart);
            visual_सीमा=runs[j].visual_सीमा;
            यदि(IS_EVEN_सततम्(runs[j].logicalStart)) {
                कुरु { /* LTR */
                    सूचकांक_Map[logicalStart++]=visualStart++;
                } यावत्(visualStart<visual_सीमा);
            } अन्यथा {
                logicalStart+=visual_सीमा-visualStart;  /* logicalLimit */
                कुरु { /* RTL */
                    सूचकांक_Map[--logicalStart]=visualStart++;
                } यावत्(visualStart<visual_सीमा);
            }
            /* visualStart==visualLimit; */
        }

        यदि(pBiDi->निवेशय_Points.size>0) {
            int32_t mark_प्राप्तम्=0, सततम्_गणनम्=pBiDi->सततम्_गणनम्;
            int32_t length, निवेशय_Remove;
            visualStart=0;
            /* add number of marks found until each index */
            प्रति(i=0; i<सततम्_गणनम्; i++, visualStart+=length) {
                length=runs[i].visual_सीमा-visualStart;
                निवेशय_Remove=runs[i].निवेशय_Remove;
                यदि(निवेशय_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_प्राप्तम्++;
                }
                यदि(mark_प्राप्तम्>0) {
                    logicalStart=GET_सूचकांक(runs[i].logicalStart);
                    logical_सीमा=logicalStart+length;
                    प्रति(j=logicalStart; j<logical_सीमा; j++) {
                        सूचकांक_Map[j]+=mark_प्राप्तम्;
                    }
                }
                यदि(निवेशय_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_प्राप्तम्++;
                }
            }
        }
        अन्यथा यदि(pBiDi->control_गणनम्>0) {
            int32_t control_प्राप्तम्=0, सततम्_गणनम्=pBiDi->सततम्_गणनम्;
            int32_t length, निवेशय_Remove;
            UBool even_सततम्;
            char16_t uchar;
            visualStart=0;
            /* subtract number of controls found until each index */
            प्रति(i=0; i<सततम्_गणनम्; i++, visualStart+=length) {
                length=runs[i].visual_सीमा-visualStart;
                निवेशय_Remove=runs[i].निवेशय_Remove;
                /* no control found within previous runs nor within this run */
                यदि((control_प्राप्तम्-निवेशय_Remove)==0) {
                    अनुवर्त;
                }
                logicalStart=runs[i].logicalStart;
                even_सततम्=IS_EVEN_सततम्(logicalStart);
                REMOVE_ODD_BIT(logicalStart);
                logical_सीमा=logicalStart+length;
                /* if no control within this run */
                यदि(निवेशय_Remove==0) {
                    प्रति(j=logicalStart; j<logical_सीमा; j++) {
                        सूचकांक_Map[j]-=control_प्राप्तम्;
                    }
                    अनुवर्त;
                }
                प्रति(j=0; j<length; j++) {
                    k= even_सततम् ? logicalStart+j : logical_सीमा-j-1;
                    uchar=pBiDi->text[k];
                    यदि(IS_BIDI_CONTROL_CHAR(uchar)) {
                        control_प्राप्तम्++;
                        सूचकांक_Map[k]=UBIDI_MAP_NOWHERE;
                        अनुवर्त;
                    }
                    सूचकांक_Map[k]-=control_प्राप्तम्;
                }
            }
        }
    }
}

U_CAPI शून्य U_EXPORT2
ubidi_getVisualMap(UBiDi *pBiDi, int32_t *सूचकांक_Map, UError_संहिता *p_Error_संहिता) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_संहिता);
    यदि(सूचकांक_Map==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_गणनम्_Runs(pBiDi, p_Error_संहिता);
    यदि(U_SUCCESS(*p_Error_संहिता)) {
        /* fill a visual-to-logical index map using the runs[] */
        सततम् *runs=pBiDi->runs, *runs_सीमा=runs+pBiDi->सततम्_गणनम्;
        int32_t logicalStart, visualStart, visual_सीमा, *pi=सूचकांक_Map;

        यदि (pBiDi->परिणाम_दीर्घता<=0) {
            प्रत्यागम;
        }
        visualStart=0;
        प्रति(; runs<runs_सीमा; ++runs) {
            logicalStart=runs->logicalStart;
            visual_सीमा=runs->visual_सीमा;
            यदि(IS_EVEN_सततम्(logicalStart)) {
                कुरु { /* LTR */
                    *pi++ = logicalStart++;
                } यावत्(++visualStart<visual_सीमा);
            } अन्यथा {
                REMOVE_ODD_BIT(logicalStart);
                logicalStart+=visual_सीमा-visualStart;  /* logicalLimit */
                कुरु { /* RTL */
                    *pi++ = --logicalStart;
                } यावत्(++visualStart<visual_सीमा);
            }
            /* visualStart==visualLimit; */
        }

        यदि(pBiDi->निवेशय_Points.size>0) {
            int32_t mark_प्राप्तम्=0, सततम्_गणनम्=pBiDi->सततम्_गणनम्;
            int32_t निवेशय_Remove, i, j, k;
            runs=pBiDi->runs;
            /* count all inserted marks */
            प्रति(i=0; i<सततम्_गणनम्; i++) {
                निवेशय_Remove=runs[i].निवेशय_Remove;
                यदि(निवेशय_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_प्राप्तम्++;
                }
                यदि(निवेशय_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_प्राप्तम्++;
                }
            }
            /* move back indexes by number of preceding marks */
            k=pBiDi->परिणाम_दीर्घता;
            प्रति(i=सततम्_गणनम्-1; i>=0 && mark_प्राप्तम्>0; i--) {
                निवेशय_Remove=runs[i].निवेशय_Remove;
                यदि(निवेशय_Remove&(LRM_AFTER|RLM_AFTER)) {
                    सूचकांक_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_प्राप्तम्--;
                }
                visualStart= i>0 ? runs[i-1].visual_सीमा : 0;
                प्रति(j=runs[i].visual_सीमा-1; j>=visualStart && mark_प्राप्तम्>0; j--) {
                    सूचकांक_Map[--k]=सूचकांक_Map[j];
                }
                यदि(निवेशय_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    सूचकांक_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_प्राप्तम्--;
                }
            }
        }
        अन्यथा यदि(pBiDi->control_गणनम्>0) {
            int32_t सततम्_गणनम्=pBiDi->सततम्_गणनम्, logical_अन्तः;
            int32_t निवेशय_Remove, length, i, j, k, m;
            char16_t uchar;
            UBool even_सततम्;
            runs=pBiDi->runs;
            visualStart=0;
            /* move forward indexes by number of preceding controls */
            k=0;
            प्रति(i=0; i<सततम्_गणनम्; i++, visualStart+=length) {
                length=runs[i].visual_सीमा-visualStart;
                निवेशय_Remove=runs[i].निवेशय_Remove;
                /* if no control found yet, nothing to do in this run */
                यदि((निवेशय_Remove==0)&&(k==visualStart)) {
                    k+=length;
                    अनुवर्त;
                }
                /* if no control in this run */
                यदि(निवेशय_Remove==0) {
                    visual_सीमा=runs[i].visual_सीमा;
                    प्रति(j=visualStart; j<visual_सीमा; j++) {
                        सूचकांक_Map[k++]=सूचकांक_Map[j];
                    }
                    अनुवर्त;
                }
                logicalStart=runs[i].logicalStart;
                even_सततम्=IS_EVEN_सततम्(logicalStart);
                REMOVE_ODD_BIT(logicalStart);
                logical_अन्तः=logicalStart+length-1;
                प्रति(j=0; j<length; j++) {
                    m= even_सततम् ? logicalStart+j : logical_अन्तः-j;
                    uchar=pBiDi->text[m];
                    यदि(!IS_BIDI_CONTROL_CHAR(uchar)) {
                        सूचकांक_Map[k++]=m;
                    }
                }
            }
        }
    }
}

U_CAPI शून्य U_EXPORT2
ubidi_invertMap(ध्रुव int32_t *srcMap, int32_t *destMap, int32_t length) {
    यदि(srcMap!=शून्यसूचकः && destMap!=शून्यसूचकः && length>0) {
        ध्रुव int32_t *pi;
        int32_t dest_दीर्घता=-1, count=0;
        /* find highest value and count positive indexes in srcMap */
        pi=srcMap+length;
        यावत्(pi>srcMap) {
            यदि(*--pi>dest_दीर्घता) {
                dest_दीर्घता=*pi;
            }
            यदि(*pi>=0) {
                count++;
            }
        }
        dest_दीर्घता++;           /* add 1 for origin 0 */
        यदि(count<dest_दीर्घता) {
            /* we must fill unmatched destMap entries with -1 */
            uprv_memset(destMap, 0xFF, dest_दीर्घता*परिमाण(int32_t));
        }
        pi=srcMap+length;
        यावत्(length>0) {
            यदि(*--pi>=0) {
                destMap[*pi]=--length;
            } अन्यथा {
                --length;
            }
        }
    }
}
