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

#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/ubidi.h"
#అన్తర్భావ "ubidiimp.h"
#అన్తర్భావ "uassert.h"

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
స్థిర శూన్య
సమితి_Trailing_WSStart(UBiDi *pBiDi) {
    /* pBiDi->direction!=UBIDI_MIXED */

    స్థిరరాశి DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *స్థాయిలు=pBiDi->స్థాయిలు;
    int32_t ప్రారంభం=pBiDi->length;
    UBiDiLevel paraLevel=pBiDi->paraLevel;

    /* If the line is terminated by a block separator, all preceding WS etc...
       are already set to paragraph level.
       Setting trailingWSStart to pBidi->length will avoid changing the
       level of B chars from 0 to paraLevel in ubidi_getLevels when
       orderParagraphsLTR==true.
     */
    ఒకవేళ(dirProps[ప్రారంభం-1]==B) {
        pBiDi->trailingWSStart=ప్రారంభం;   /* currently == pBiDi->length */
        తిరిగి;
    }
    /* go backwards across all WS, BN, explicit codes */
    అయితే(ప్రారంభం>0 && DIRPROP_FLAG(dirProps[ప్రారంభం-1])&ముసుగు_WS) {
        --ప్రారంభం;
    }

    /* if the WS run can be merged with the previous run then do so here */
    అయితే(ప్రారంభం>0 && స్థాయిలు[ప్రారంభం-1]==paraLevel) {
        --ప్రారంభం;
    }

    pBiDi->trailingWSStart=ప్రారంభం;
}

/* ubidi_setLine ------------------------------------------------------------ */

U_CAPI శూన్య U_EXPORT2
ubidi_సమితి_పంక్తి(స్థిరరాశి UBiDi *pParaBiDi,
              int32_t ప్రారంభం, int32_t హద్దు,
              UBiDi *p_పంక్తి_Bi_Di,
              UError_సంకేతం *p_Error_సంకేతం) {
    int32_t length;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_సంకేతం);
    RETURN_VOID_IF_NOT_VALID_PARA(pParaBiDi, *p_Error_సంకేతం);
    RETURN_VOID_IF_BAD_RANGE(ప్రారంభం, 0, హద్దు, *p_Error_సంకేతం);
    RETURN_VOID_IF_BAD_RANGE(హద్దు, 0, pParaBiDi->length+1, *p_Error_సంకేతం);
    ఒకవేళ(p_పంక్తి_Bi_Di==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    ఒకవేళ(ubidi_getParagraph(pParaBiDi, ప్రారంభం, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, p_Error_సంకేతం) !=
       ubidi_getParagraph(pParaBiDi, హద్దు-1, శూన్యసూచిక, శూన్యసూచిక, శూన్యసూచిక, p_Error_సంకేతం)) {
        /* the line crosses a paragraph boundary */
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    /* set the values in pLineBiDi from its pParaBiDi parent */
    p_పంక్తి_Bi_Di->pParaBiDi=శూన్యసూచిక;          /* mark unfinished setLine */
    p_పంక్తి_Bi_Di->పాఠ్యం=pParaBiDi->పాఠ్యం+ప్రారంభం;
    length=p_పంక్తి_Bi_Di->length=హద్దు-ప్రారంభం;
    p_పంక్తి_Bi_Di->ఫలితం_నిడివి=p_పంక్తి_Bi_Di->original_నిడివి=length;
    p_పంక్తి_Bi_Di->paraLevel=GET_PARALEVEL(pParaBiDi, ప్రారంభం);
    p_పంక్తి_Bi_Di->para_లెక్క=pParaBiDi->para_లెక్క;
    p_పంక్తి_Bi_Di->runs=శూన్యసూచిక;
    p_పంక్తి_Bi_Di->flags=0;
    p_పంక్తి_Bi_Di->reordering_బహుళకం=pParaBiDi->reordering_బహుళకం;
    p_పంక్తి_Bi_Di->reorderingOptions=pParaBiDi->reorderingOptions;
    p_పంక్తి_Bi_Di->control_లెక్క=0;
    ఒకవేళ(pParaBiDi->control_లెక్క>0) {
        int32_t j;
        కొరకు(j=ప్రారంభం; j<హద్దు; j++) {
            ఒకవేళ(IS_BIDI_CONTROL_CHAR(pParaBiDi->పాఠ్యం[j])) {
                p_పంక్తి_Bi_Di->control_లెక్క++;
            }
        }
        p_పంక్తి_Bi_Di->ఫలితం_నిడివి-=p_పంక్తి_Bi_Di->control_లెక్క;
    }

    p_పంక్తి_Bi_Di->dirProps=pParaBiDi->dirProps+ప్రారంభం;
    p_పంక్తి_Bi_Di->స్థాయిలు=pParaBiDi->స్థాయిలు+ప్రారంభం;
    p_పంక్తి_Bi_Di->నిరంతరం_లెక్క=-1;

    ఒకవేళ(pParaBiDi->direction!=UBIDI_MIXED) {
        /* the parent is already trivial */
        p_పంక్తి_Bi_Di->direction=pParaBiDi->direction;

        /*
         * The parent's levels are all either
         * implicitly or explicitly ==paraLevel;
         * do the same here.
         */
        ఒకవేళ(pParaBiDi->trailingWSStart<=ప్రారంభం) {
            p_పంక్తి_Bi_Di->trailingWSStart=0;
        } లేకపోతే ఒకవేళ(pParaBiDi->trailingWSStart<హద్దు) {
            p_పంక్తి_Bi_Di->trailingWSStart=pParaBiDi->trailingWSStart-ప్రారంభం;
        } లేకపోతే {
            p_పంక్తి_Bi_Di->trailingWSStart=length;
        }
    } లేకపోతే {
        స్థిరరాశి UBiDiLevel *స్థాయిలు=p_పంక్తి_Bi_Di->స్థాయిలు;
        int32_t i, trailingWSStart;
        UBiDiLevel level;

        సమితి_Trailing_WSStart(p_పంక్తి_Bi_Di);
        trailingWSStart=p_పంక్తి_Bi_Di->trailingWSStart;

        /* recalculate pLineBiDi->direction */
        ఒకవేళ(trailingWSStart==0) {
            /* all levels are at paraLevel */
            p_పంక్తి_Bi_Di->direction=(UBiDiDirection)(p_పంక్తి_Bi_Di->paraLevel&1);
        } లేకపోతే {
            /* get the level of the first character */
            level=(UBiDiLevel)(స్థాయిలు[0]&1);

            /* if there is anything of a different level, then the line is mixed */
            ఒకవేళ(trailingWSStart<length && (p_పంక్తి_Bi_Di->paraLevel&1)!=level) {
                /* the trailing WS is at paraLevel, which differs from levels[0] */
                p_పంక్తి_Bi_Di->direction=UBIDI_MIXED;
            } లేకపోతే {
                /* see if levels[1..trailingWSStart-1] have the same direction as levels[0] and paraLevel */
                i=1;
                కొరకు(;;) {
                    ఒకవేళ(i==trailingWSStart) {
                        /* the direction values match those in level */
                        p_పంక్తి_Bi_Di->direction=(UBiDiDirection)level;
                        ఆపు;
                    } లేకపోతే ఒకవేళ((స్థాయిలు[i]&1)!=level) {
                        p_పంక్తి_Bi_Di->direction=UBIDI_MIXED;
                        ఆపు;
                    }
                    ++i;
                }
            }
        }

        ఎంపిక(p_పంక్తి_Bi_Di->direction) {
        స్థితి UBIDI_LTR:
            /* make sure paraLevel is even */
            p_పంక్తి_Bi_Di->paraLevel=(UBiDiLevel)((p_పంక్తి_Bi_Di->paraLevel+1)&~1);

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_పంక్తి_Bi_Di->trailingWSStart=0;
            ఆపు;
        స్థితి UBIDI_RTL:
            /* make sure paraLevel is odd */
            p_పంక్తి_Bi_Di->paraLevel|=1;

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_పంక్తి_Bi_Di->trailingWSStart=0;
            ఆపు;
        అప్రమేయ:
            ఆపు;
        }
    }
    p_పంక్తి_Bi_Di->pParaBiDi=pParaBiDi;     /* mark successful setLine */
}

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getLevelAt(స్థిరరాశి UBiDi *pBiDi, int32_t char_సూచీ) {
    /* return paraLevel if in the trailing WS run, otherwise the real level */
    ఒకవేళ(!IS_VALID_PARA_OR_పంక్తి(pBiDi) || char_సూచీ<0 || pBiDi->length<=char_సూచీ) {
        తిరిగి 0;
    } లేకపోతే ఒకవేళ(pBiDi->direction!=UBIDI_MIXED || char_సూచీ>=pBiDi->trailingWSStart) {
        తిరిగి GET_PARALEVEL(pBiDi, char_సూచీ);
    } లేకపోతే {
        తిరిగి pBiDi->స్థాయిలు[char_సూచీ];
    }
}

U_CAPI స్థిరరాశి UBiDiLevel* U_EXPORT2
ubidi_get_స్థాయిలు(UBiDi* pBiDi, UError_సంకేతం* p_Error_సంకేతం) {
    int32_t ప్రారంభం, length;

    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_సంకేతం, శూన్యసూచిక);
    RETURN_IF_NOT_VALID_PARA_OR_పంక్తి(pBiDi, *p_Error_సంకేతం, శూన్యసూచిక);
    ఒకవేళ((length=pBiDi->length)<=0) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ((ప్రారంభం=pBiDi->trailingWSStart)==length) {
        /* the current levels array reflects the WS run */
        తిరిగి pBiDi->స్థాయిలు;
    }

    /*
     * After the previous if(), we know that the levels array
     * has an implicit trailing WS run and therefore does not fully
     * reflect itself all the levels.
     * This must be a UBiDi object for a line, and
     * we need to create a new levels array.
     */
    ఒకవేళ(get_స్థాయిలు_జ్ఞాపకం(pBiDi, length)) {
        UBiDiLevel *స్థాయిలు=pBiDi->స్థాయిలు_జ్ఞాపకం;

        ఒకవేళ(ప్రారంభం>0 && స్థాయిలు!=pBiDi->స్థాయిలు) {
            uprv_memcpy(స్థాయిలు, pBiDi->స్థాయిలు, ప్రారంభం);
        }
        /* pBiDi->paraLevel is ok even if contextual multiple paragraphs,
           since pBidi is a line object                                     */
        uprv_memset(స్థాయిలు+ప్రారంభం, pBiDi->paraLevel, length-ప్రారంభం);

        /* this new levels array is set for the line and reflects the WS run */
        pBiDi->trailingWSStart=length;
        తిరిగి pBiDi->స్థాయిలు=స్థాయిలు;
    } లేకపోతే {
        /* out of memory */
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
}

U_CAPI శూన్య U_EXPORT2
ubidi_get_Logical_నిరంతరం(స్థిరరాశి UBiDi *pBiDi, int32_t logicalPosition,
                    int32_t *p_Logical_హద్దు, UBiDiLevel *pLevel) {
    UError_సంకేతం error_సంకేతం;
    int32_t నిరంతరం_లెక్క, visual_ప్రారంభం, logical_హద్దు, logical_మొదటి, i;
    నిరంతరం i_నిరంతరం;

    error_సంకేతం=U_ZERO_ERROR;
    RETURN_VOID_IF_BAD_RANGE(logicalPosition, 0, pBiDi->length, error_సంకేతం);
    /* ubidi_countRuns will check VALID_PARA_OR_LINE */
    నిరంతరం_లెక్క=ubidi_లెక్క_Runs((UBiDi *)pBiDi, &error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    /* this is done based on runs rather than on levels since levels have
       a special interpretation when UBIDI_REORDER_RUNS_ONLY
     */
    visual_ప్రారంభం=logical_హద్దు=0;
    i_నిరంతరం=pBiDi->runs[0];

    కొరకు(i=0; i<నిరంతరం_లెక్క; i++) {
        i_నిరంతరం = pBiDi->runs[i];
        logical_మొదటి=GET_సూచీ(i_నిరంతరం.logical_ప్రారంభం);
        logical_హద్దు=logical_మొదటి+i_నిరంతరం.visual_హద్దు-visual_ప్రారంభం;
        ఒకవేళ((logicalPosition>=logical_మొదటి) &&
           (logicalPosition<logical_హద్దు)) {
            ఆపు;
        }
        visual_ప్రారంభం = i_నిరంతరం.visual_హద్దు;
    }
    ఒకవేళ(p_Logical_హద్దు) {
        *p_Logical_హద్దు=logical_హద్దు;
    }
    ఒకవేళ(pLevel) {
        ఒకవేళ(pBiDi->reordering_బహుళకం==UBIDI_REORDER_RUNS_ONLY) {
            *pLevel=(UBiDiLevel)GET_ODD_బిట్టు(i_నిరంతరం.logical_ప్రారంభం);
        }
        లేకపోతే ఒకవేళ(pBiDi->direction!=UBIDI_MIXED || logicalPosition>=pBiDi->trailingWSStart) {
            *pLevel=GET_PARALEVEL(pBiDi, logicalPosition);
        } లేకపోతే {
        *pLevel=pBiDi->స్థాయిలు[logicalPosition];
        }
    }
}

/* runs API functions ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_లెక్క_Runs(UBiDi *pBiDi, UError_సంకేతం *p_Error_సంకేతం) {
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_సంకేతం, -1);
    RETURN_IF_NOT_VALID_PARA_OR_పంక్తి(pBiDi, *p_Error_సంకేతం, -1);
    ubidi_getRuns(pBiDi, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి -1;
    }
    తిరిగి pBiDi->నిరంతరం_లెక్క;
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_Visual_నిరంతరం(UBiDi *pBiDi, int32_t నిరంతరం_సూచీ,
                   int32_t *p_Logical_ప్రారంభం, int32_t *p_నిడివి)
{
    int32_t ప్రారంభం;
    UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
    RETURN_IF_NOT_VALID_PARA_OR_పంక్తి(pBiDi, error_సంకేతం, UBIDI_LTR);
    ubidi_getRuns(pBiDi, &error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి UBIDI_LTR;
    }
    RETURN_IF_BAD_RANGE(నిరంతరం_సూచీ, 0, pBiDi->నిరంతరం_లెక్క, error_సంకేతం, UBIDI_LTR);

    ప్రారంభం=pBiDi->runs[నిరంతరం_సూచీ].logical_ప్రారంభం;
    ఒకవేళ(p_Logical_ప్రారంభం!=శూన్యసూచిక) {
        *p_Logical_ప్రారంభం=GET_సూచీ(ప్రారంభం);
    }
    ఒకవేళ(p_నిడివి!=శూన్యసూచిక) {
        ఒకవేళ(నిరంతరం_సూచీ>0) {
            *p_నిడివి=pBiDi->runs[నిరంతరం_సూచీ].visual_హద్దు-
                     pBiDi->runs[నిరంతరం_సూచీ-1].visual_హద్దు;
        } లేకపోతే {
            *p_నిడివి=pBiDi->runs[0].visual_హద్దు;
        }
    }
    తిరిగి (UBiDiDirection)GET_ODD_బిట్టు(ప్రారంభం);
}

/* in trivial cases there is only one trivial run; called by ubidi_getRuns() */
స్థిర శూన్య
get_Single_నిరంతరం(UBiDi *pBiDi, UBiDiLevel level) {
    /* simple, single-run case */
    pBiDi->runs=pBiDi->simpleRuns;
    pBiDi->నిరంతరం_లెక్క=1;

    /* fill and reorder the single run */
    pBiDi->runs[0].logical_ప్రారంభం=MAKE_సూచీ_ODD_PAIR(0, level);
    pBiDi->runs[0].visual_హద్దు=pBiDi->length;
    pBiDi->runs[0].చొప్పించు_Remove=0;
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
స్థిర శూన్య
reorder_పంక్తి(UBiDi *pBiDi, UBiDiLevel minLevel, UBiDiLevel maxLevel) {
    నిరంతరం *runs, తాత్కాలిక_నిరంతరం;
    UBiDiLevel *స్థాయిలు;
    int32_t మొదటి_నిరంతరం, చివర_నిరంతరం, హద్దు_నిరంతరం, నిరంతరం_లెక్క;

    /* nothing to do? */
    ఒకవేళ(maxLevel<=(minLevel|1)) {
        తిరిగి;
    }

    /*
     * Reorder only down to the lowest odd level
     * and reorder at an odd minLevel in a separate, simpler loop.
     * See comments above for why minLevel is always incremented.
     */
    ++minLevel;

    runs=pBiDi->runs;
    స్థాయిలు=pBiDi->స్థాయిలు;
    నిరంతరం_లెక్క=pBiDi->నిరంతరం_లెక్క;

    /* do not include the WS run at paraLevel<=old minLevel except in the simple loop */
    ఒకవేళ(pBiDi->trailingWSStart<pBiDi->length) {
        --నిరంతరం_లెక్క;
    }

    అయితే(--maxLevel>=minLevel) {
        మొదటి_నిరంతరం=0;

        /* loop for all sequences of runs */
        కొరకు(;;) {
            /* look for a sequence of runs that are all at >=maxLevel */
            /* look for the first run of such a sequence */
            అయితే(మొదటి_నిరంతరం<నిరంతరం_లెక్క && స్థాయిలు[runs[మొదటి_నిరంతరం].logical_ప్రారంభం]<maxLevel) {
                ++మొదటి_నిరంతరం;
            }
            ఒకవేళ(మొదటి_నిరంతరం>=నిరంతరం_లెక్క) {
                ఆపు;  /* no more such runs */
            }

            /* look for the limit run of such a sequence (the run behind it) */
            కొరకు(హద్దు_నిరంతరం=మొదటి_నిరంతరం; ++హద్దు_నిరంతరం<నిరంతరం_లెక్క && స్థాయిలు[runs[హద్దు_నిరంతరం].logical_ప్రారంభం]>=maxLevel;) {}

            /* Swap the entire sequence of runs from firstRun to limitRun-1. */
            చివర_నిరంతరం=హద్దు_నిరంతరం-1;
            అయితే(మొదటి_నిరంతరం<చివర_నిరంతరం) {
                తాత్కాలిక_నిరంతరం = runs[మొదటి_నిరంతరం];
                runs[మొదటి_నిరంతరం]=runs[చివర_నిరంతరం];
                runs[చివర_నిరంతరం]=తాత్కాలిక_నిరంతరం;
                ++మొదటి_నిరంతరం;
                --చివర_నిరంతరం;
            }

            ఒకవేళ(హద్దు_నిరంతరం==నిరంతరం_లెక్క) {
                ఆపు;  /* no more such runs */
            } లేకపోతే {
                మొదటి_నిరంతరం=హద్దు_నిరంతరం+1;
            }
        }
    }

    /* now do maxLevel==old minLevel (==odd!), see above */
    ఒకవేళ(!(minLevel&1)) {
        మొదటి_నిరంతరం=0;

        /* include the trailing WS run in this complete reordering */
        ఒకవేళ(pBiDi->trailingWSStart==pBiDi->length) {
            --నిరంతరం_లెక్క;
        }

        /* Swap the entire sequence of all runs. (endRun==runCount) */
        అయితే(మొదటి_నిరంతరం<నిరంతరం_లెక్క) {
            తాత్కాలిక_నిరంతరం=runs[మొదటి_నిరంతరం];
            runs[మొదటి_నిరంతరం]=runs[నిరంతరం_లెక్క];
            runs[నిరంతరం_లెక్క]=తాత్కాలిక_నిరంతరం;
            ++మొదటి_నిరంతరం;
            --నిరంతరం_లెక్క;
        }
    }
}

/* compute the runs array --------------------------------------------------- */

స్థిర int32_t get_నిరంతరం_From_Logical_సూచీ(UBiDi *pBiDi, int32_t logical_సూచీ) {
    నిరంతరం *runs=pBiDi->runs;
    int32_t నిరంతరం_లెక్క=pBiDi->నిరంతరం_లెక్క, visual_ప్రారంభం=0, i, length, logical_ప్రారంభం;

    కొరకు(i=0; i<నిరంతరం_లెక్క; i++) {
        length=runs[i].visual_హద్దు-visual_ప్రారంభం;
        logical_ప్రారంభం=GET_సూచీ(runs[i].logical_ప్రారంభం);
        ఒకవేళ((logical_సూచీ>=logical_ప్రారంభం) && (logical_సూచీ<(logical_ప్రారంభం+length))) {
            తిరిగి i;
        }
        visual_ప్రారంభం+=length;
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
ubidi_getRuns(UBiDi *pBiDi, UError_సంకేతం* p_Error_సంకేతం) {
    /*
     * This method returns immediately if the runs are already set. This
     * includes the case of length==0 (handled in setPara)..
     */
    ఒకవేళ (pBiDi->నిరంతరం_లెక్క>=0) {
        తిరిగి నిజం;
    }

    ఒకవేళ(pBiDi->direction!=UBIDI_MIXED) {
        /* simple, single-run case - this covers length==0 */
        /* pBiDi->paraLevel is ok even for contextual multiple paragraphs */
        get_Single_నిరంతరం(pBiDi, pBiDi->paraLevel);
    } లేకపోతే /* UBIDI_MIXED, length>0 */ {
        /* mixed directionality */
        int32_t length=pBiDi->length, హద్దు;
        UBiDiLevel *స్థాయిలు=pBiDi->స్థాయిలు;
        int32_t i, నిరంతరం_లెక్క;
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
        హద్దు=pBiDi->trailingWSStart;
        /* count the runs, there is at least one non-WS run, and limit>0 */
        నిరంతరం_లెక్క=0;
        కొరకు(i=0; i<హద్దు; ++i) {
            /* increment runCount at the start of each run */
            ఒకవేళ(స్థాయిలు[i]!=level) {
                ++నిరంతరం_లెక్క;
                level=స్థాయిలు[i];
            }
        }

        /*
         * We don't need to see if the last run can be merged with a trailing
         * WS run because setTrailingWSStart() would have done that.
         */
        ఒకవేళ(నిరంతరం_లెక్క==1 && హద్దు==length) {
            /* There is only one non-WS run and no trailing WS-run. */
            get_Single_నిరంతరం(pBiDi, స్థాయిలు[0]);
        } లేకపోతే /* runCount>1 || limit<length */ {
            /* allocate and set the runs */
            నిరంతరం *runs;
            int32_t నిరంతరం_సూచీ, ప్రారంభం;
            UBiDiLevel minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1, maxLevel=0;

            /* now, count a (non-mergeable) WS run */
            ఒకవేళ(హద్దు<length) {
                ++నిరంతరం_లెక్క;
            }

            /* runCount>1 */
            ఒకవేళ(get_Runs_జ్ఞాపకం(pBiDi, నిరంతరం_లెక్క)) {
                runs=pBiDi->runs_జ్ఞాపకం;
            } లేకపోతే {
                *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి అబద్ధం;
            }

            /* set the runs */
            /* FOOD FOR THOUGHT: this could be optimized, e.g.:
             * 464->444, 484->444, 575->555, 595->555
             * However, that would take longer. Check also how it would
             * interact with BiDi control removal and inserting Marks.
             */
            నిరంతరం_సూచీ=0;

            /* search for the run limits and initialize visualLimit values with the run lengths */
            i=0;
            చేయి {
                /* prepare this run */
                ప్రారంభం=i;
                level=స్థాయిలు[i];
                ఒకవేళ(level<minLevel) {
                    minLevel=level;
                }
                ఒకవేళ(level>maxLevel) {
                    maxLevel=level;
                }

                /* look for the run limit */
                అయితే(++i<హద్దు && స్థాయిలు[i]==level) {}

                /* i is another run limit */
                runs[నిరంతరం_సూచీ].logical_ప్రారంభం=ప్రారంభం;
                runs[నిరంతరం_సూచీ].visual_హద్దు=i-ప్రారంభం;
                runs[నిరంతరం_సూచీ].చొప్పించు_Remove=0;
                ++నిరంతరం_సూచీ;
            } అయితే(i<హద్దు);

            ఒకవేళ(హద్దు<length) {
                /* there is a separate WS run */
                runs[నిరంతరం_సూచీ].logical_ప్రారంభం=హద్దు;
                runs[నిరంతరం_సూచీ].visual_హద్దు=length-హద్దు;
                /* For the trailing WS run, pBiDi->paraLevel is ok even
                   if contextual multiple paragraphs.                   */
                ఒకవేళ(pBiDi->paraLevel<minLevel) {
                    minLevel=pBiDi->paraLevel;
                }
            }

            /* set the object fields */
            pBiDi->runs=runs;
            pBiDi->నిరంతరం_లెక్క=నిరంతరం_లెక్క;

            reorder_పంక్తి(pBiDi, minLevel, maxLevel);

            /* now add the direction flags and adjust the visualLimit's to be just that */
            /* this loop will also handle the trailing WS run */
            హద్దు=0;
            కొరకు(i=0; i<నిరంతరం_లెక్క; ++i) {
                కూడు_ODD_బిట్టు_FROM_LEVEL(runs[i].logical_ప్రారంభం, స్థాయిలు[runs[i].logical_ప్రారంభం]);
                హద్దు+=runs[i].visual_హద్దు;
                runs[i].visual_హద్దు=హద్దు;
            }

            /* Set the "odd" bit for the trailing WS run. */
            /* For a RTL paragraph, it will be the *first* run in visual order. */
            /* For the trailing WS run, pBiDi->paraLevel is ok even if
               contextual multiple paragraphs.                          */
            ఒకవేళ(నిరంతరం_సూచీ<నిరంతరం_లెక్క) {
                int32_t trailing_నిరంతరం = ((pBiDi->paraLevel & 1) != 0)? 0 : నిరంతరం_సూచీ;

                కూడు_ODD_బిట్టు_FROM_LEVEL(runs[trailing_నిరంతరం].logical_ప్రారంభం, pBiDi->paraLevel);
            }
        }
    }

    /* handle insert LRM/RLM BEFORE/AFTER run */
    ఒకవేళ(pBiDi->చొప్పించు_Points.size>0) {
        బిందువు *బిందువు_2, *ప్రారంభం=pBiDi->చొప్పించు_Points.points,
                      *హద్దు=ప్రారంభం+pBiDi->చొప్పించు_Points.size;
        int32_t నిరంతరం_సూచీ;
        కొరకు(బిందువు_2=ప్రారంభం; బిందువు_2<హద్దు; బిందువు_2++) {
            నిరంతరం_సూచీ=get_నిరంతరం_From_Logical_సూచీ(pBiDi, బిందువు_2->pos);
            pBiDi->runs[నిరంతరం_సూచీ].చొప్పించు_Remove|=బిందువు_2->flag;
        }
    }

    /* handle remove BiDi control characters */
    ఒకవేళ(pBiDi->control_లెక్క>0) {
        int32_t నిరంతరం_సూచీ;
        స్థిరరాశి char16_t *ప్రారంభం=pBiDi->పాఠ్యం, *హద్దు=ప్రారంభం+pBiDi->length, *pu;
        కొరకు(pu=ప్రారంభం; pu<హద్దు; pu++) {
            ఒకవేళ(IS_BIDI_CONTROL_CHAR(*pu)) {
                నిరంతరం_సూచీ=get_నిరంతరం_From_Logical_సూచీ(pBiDi, (int32_t)(pu-ప్రారంభం));
                pBiDi->runs[నిరంతరం_సూచీ].చొప్పించు_Remove--;
            }
        }
    }

    తిరిగి నిజం;
}

స్థిర UBool
prepareReorder(స్థిరరాశి UBiDiLevel *స్థాయిలు, int32_t length,
               int32_t *సూచీ_Map,
               UBiDiLevel *pMinLevel, UBiDiLevel *pMaxLevel) {
    int32_t ప్రారంభం;
    UBiDiLevel level, minLevel, maxLevel;

    ఒకవేళ(స్థాయిలు==శూన్యసూచిక || length<=0) {
        తిరిగి అబద్ధం;
    }

    /* determine minLevel and maxLevel */
    minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1;
    maxLevel=0;
    కొరకు(ప్రారంభం=length; ప్రారంభం>0;) {
        level=స్థాయిలు[--ప్రారంభం];
        ఒకవేళ(level>UBIDI_MAX_EXPLICIT_LEVEL+1) {
            తిరిగి అబద్ధం;
        }
        ఒకవేళ(level<minLevel) {
            minLevel=level;
        }
        ఒకవేళ(level>maxLevel) {
            maxLevel=level;
        }
    }
    *pMinLevel=minLevel;
    *pMaxLevel=maxLevel;

    /* initialize the index map */
    కొరకు(ప్రారంభం=length; ప్రారంభం>0;) {
        --ప్రారంభం;
        సూచీ_Map[ప్రారంభం]=ప్రారంభం;
    }

    తిరిగి నిజం;
}

/* reorder a line based on a levels array (L2) ------------------------------ */

U_CAPI శూన్య U_EXPORT2
ubidi_reorderLogical(స్థిరరాశి UBiDiLevel *స్థాయిలు, int32_t length, int32_t *సూచీ_Map) {
    int32_t ప్రారంభం, హద్దు, కూడిక_యొక్క_Sos_Eos;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    ఒకవేళ(సూచీ_Map==శూన్యసూచిక || !prepareReorder(స్థాయిలు, length, సూచీ_Map, &minLevel, &maxLevel)) {
        తిరిగి;
    }

    /* nothing to do? */
    ఒకవేళ(minLevel==maxLevel && (minLevel&1)==0) {
        తిరిగి;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    చేయి {
        ప్రారంభం=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        కొరకు(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            అయితే(ప్రారంభం<length && స్థాయిలు[ప్రారంభం]<maxLevel) {
                ++ప్రారంభం;
            }
            ఒకవేళ(ప్రారంభం>=length) {
                ఆపు;  /* no more such sequences */
            }

            /* look for the limit of such a sequence (the index behind it) */
            కొరకు(హద్దు=ప్రారంభం; ++హద్దు<length && స్థాయిలు[హద్దు]>=maxLevel;) {}

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
            కూడిక_యొక్క_Sos_Eos=ప్రారంభం+హద్దు-1;

            /* reorder each index in the sequence */
            చేయి {
                సూచీ_Map[ప్రారంభం]=కూడిక_యొక్క_Sos_Eos-సూచీ_Map[ప్రారంభం];
            } అయితే(++ప్రారంభం<హద్దు);

            /* start==limit */
            ఒకవేళ(హద్దు==length) {
                ఆపు;  /* no more such sequences */
            } లేకపోతే {
                ప్రారంభం=హద్దు+1;
            }
        }
    } అయితే(--maxLevel>=minLevel);
}

U_CAPI శూన్య U_EXPORT2
ubidi_reorderVisual(స్థిరరాశి UBiDiLevel *స్థాయిలు, int32_t length, int32_t *సూచీ_Map) {
    int32_t ప్రారంభం, చివర, హద్దు, తాత్కాలిక;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    ఒకవేళ(సూచీ_Map==శూన్యసూచిక || !prepareReorder(స్థాయిలు, length, సూచీ_Map, &minLevel, &maxLevel)) {
        తిరిగి;
    }

    /* nothing to do? */
    ఒకవేళ(minLevel==maxLevel && (minLevel&1)==0) {
        తిరిగి;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    చేయి {
        ప్రారంభం=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        కొరకు(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            అయితే(ప్రారంభం<length && స్థాయిలు[ప్రారంభం]<maxLevel) {
                ++ప్రారంభం;
            }
            ఒకవేళ(ప్రారంభం>=length) {
                ఆపు;  /* no more such runs */
            }

            /* look for the limit of such a sequence (the index behind it) */
            కొరకు(హద్దు=ప్రారంభం; ++హద్దు<length && స్థాయిలు[హద్దు]>=maxLevel;) {}

            /*
             * Swap the entire interval of indexes from start to limit-1.
             * We don't need to swap the levels for the purpose of this
             * algorithm: the sequence of levels that we look at does not
             * move anyway.
             */
            చివర=హద్దు-1;
            అయితే(ప్రారంభం<చివర) {
                తాత్కాలిక=సూచీ_Map[ప్రారంభం];
                సూచీ_Map[ప్రారంభం]=సూచీ_Map[చివర];
                సూచీ_Map[చివర]=తాత్కాలిక;

                ++ప్రారంభం;
                --చివర;
            }

            ఒకవేళ(హద్దు==length) {
                ఆపు;  /* no more such sequences */
            } లేకపోతే {
                ప్రారంభం=హద్దు+1;
            }
        }
    } అయితే(--maxLevel>=minLevel);
}

/* API functions for logical<->visual mapping ------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_get_Visual_సూచీ(UBiDi *pBiDi, int32_t logical_సూచీ, UError_సంకేతం *p_Error_సంకేతం) {
    int32_t visual_సూచీ=UBIDI_MAP_NOWHERE;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_సంకేతం, -1);
    RETURN_IF_NOT_VALID_PARA_OR_పంక్తి(pBiDi, *p_Error_సంకేతం, -1);
    RETURN_IF_BAD_RANGE(logical_సూచీ, 0, pBiDi->length, *p_Error_సంకేతం, -1);

    /* we can do the trivial cases without the runs array */
    ఎంపిక(pBiDi->direction) {
    స్థితి UBIDI_LTR:
        visual_సూచీ=logical_సూచీ;
        ఆపు;
    స్థితి UBIDI_RTL:
        visual_సూచీ=pBiDi->length-logical_సూచీ-1;
        ఆపు;
    అప్రమేయ:
        ఒకవేళ(!ubidi_getRuns(pBiDi, p_Error_సంకేతం)) {
            *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి -1;
        } లేకపోతే {
            నిరంతరం *runs=pBiDi->runs;
            int32_t i, visual_ప్రారంభం=0, స్థానభ్రంశం, length;

            /* linear search for the run, search on the visual runs */
            కొరకు(i=0; i<pBiDi->నిరంతరం_లెక్క; ++i) {
                length=runs[i].visual_హద్దు-visual_ప్రారంభం;
                స్థానభ్రంశం=logical_సూచీ-GET_సూచీ(runs[i].logical_ప్రారంభం);
                ఒకవేళ(స్థానభ్రంశం>=0 && స్థానభ్రంశం<length) {
                    ఒకవేళ(IS_EVEN_నిరంతరం(runs[i].logical_ప్రారంభం)) {
                        /* LTR */
                        visual_సూచీ=visual_ప్రారంభం+స్థానభ్రంశం;
                    } లేకపోతే {
                        /* RTL */
                        visual_సూచీ=visual_ప్రారంభం+length-స్థానభ్రంశం-1;
                    }
                    ఆపు;          /* exit for loop */
                }
                visual_ప్రారంభం+=length;
            }
            ఒకవేళ(i>=pBiDi->నిరంతరం_లెక్క) {
                తిరిగి UBIDI_MAP_NOWHERE;
            }
        }
    }

    ఒకవేళ(pBiDi->చొప్పించు_Points.size>0) {
        /* add the number of added marks until the calculated visual index */
        నిరంతరం *runs=pBiDi->runs;
        int32_t i, length, చొప్పించు_Remove;
        int32_t visual_ప్రారంభం=0, mark_దొరికింది=0;
        కొరకు(i=0; ; i++, visual_ప్రారంభం+=length) {
            length=runs[i].visual_హద్దు-visual_ప్రారంభం;
            చొప్పించు_Remove=runs[i].చొప్పించు_Remove;
            ఒకవేళ(చొప్పించు_Remove & (LRM_BEFORE|RLM_BEFORE)) {
                mark_దొరికింది++;
            }
            /* is it the run containing the visual index? */
            ఒకవేళ(visual_సూచీ<runs[i].visual_హద్దు) {
                తిరిగి visual_సూచీ+mark_దొరికింది;
            }
            ఒకవేళ(చొప్పించు_Remove & (LRM_AFTER|RLM_AFTER)) {
                mark_దొరికింది++;
            }
        }
    }
    లేకపోతే ఒకవేళ(pBiDi->control_లెక్క>0) {
        /* subtract the number of controls until the calculated visual index */
        నిరంతరం *runs=pBiDi->runs;
        int32_t i, j, ప్రారంభం, హద్దు, length, చొప్పించు_Remove;
        int32_t visual_ప్రారంభం=0, control_దొరికింది=0;
        char16_t uchar=pBiDi->పాఠ్యం[logical_సూచీ];
        /* is the logical index pointing to a control ? */
        ఒకవేళ(IS_BIDI_CONTROL_CHAR(uchar)) {
            తిరిగి UBIDI_MAP_NOWHERE;
        }
        /* loop on runs */
        కొరకు(i=0; ; i++, visual_ప్రారంభం+=length) {
            length=runs[i].visual_హద్దు-visual_ప్రారంభం;
            చొప్పించు_Remove=runs[i].చొప్పించు_Remove;
            /* calculated visual index is beyond this run? */
            ఒకవేళ(visual_సూచీ>=runs[i].visual_హద్దు) {
                control_దొరికింది-=చొప్పించు_Remove;
                కొనసాగు;
            }
            /* calculated visual index must be within current run */
            ఒకవేళ(చొప్పించు_Remove==0) {
                తిరిగి visual_సూచీ-control_దొరికింది;
            }
            ఒకవేళ(IS_EVEN_నిరంతరం(runs[i].logical_ప్రారంభం)) {
                /* LTR: check from run start to logical index */
                ప్రారంభం=runs[i].logical_ప్రారంభం;
                హద్దు=logical_సూచీ;
            } లేకపోతే {
                /* RTL: check from logical index to run end */
                ప్రారంభం=logical_సూచీ+1;
                హద్దు=GET_సూచీ(runs[i].logical_ప్రారంభం)+length;
            }
            కొరకు(j=ప్రారంభం; j<హద్దు; j++) {
                uchar=pBiDi->పాఠ్యం[j];
                ఒకవేళ(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_దొరికింది++;
                }
            }
            తిరిగి visual_సూచీ-control_దొరికింది;
        }
    }

    తిరిగి visual_సూచీ;
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Logical_సూచీ(UBiDi *pBiDi, int32_t visual_సూచీ, UError_సంకేతం *p_Error_సంకేతం) {
    నిరంతరం *runs;
    int32_t i, నిరంతరం_లెక్క, ప్రారంభం;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_సంకేతం, -1);
    RETURN_IF_NOT_VALID_PARA_OR_పంక్తి(pBiDi, *p_Error_సంకేతం, -1);
    RETURN_IF_BAD_RANGE(visual_సూచీ, 0, pBiDi->ఫలితం_నిడివి, *p_Error_సంకేతం, -1);
    /* we can do the trivial cases without the runs array */
    ఒకవేళ(pBiDi->చొప్పించు_Points.size==0 && pBiDi->control_లెక్క==0) {
        ఒకవేళ(pBiDi->direction==UBIDI_LTR) {
            తిరిగి visual_సూచీ;
        }
        లేకపోతే ఒకవేళ(pBiDi->direction==UBIDI_RTL) {
            తిరిగి pBiDi->length-visual_సూచీ-1;
        }
    }
    ఒకవేళ(!ubidi_getRuns(pBiDi, p_Error_సంకేతం)) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి -1;
    }

    runs=pBiDi->runs;
    నిరంతరం_లెక్క=pBiDi->నిరంతరం_లెక్క;
    ఒకవేళ(pBiDi->చొప్పించు_Points.size>0) {
        /* handle inserted LRM/RLM */
        int32_t mark_దొరికింది=0, చొప్పించు_Remove;
        int32_t visual_ప్రారంభం=0, length;
        runs=pBiDi->runs;
        /* subtract number of marks until visual index */
        కొరకు(i=0; ; i++, visual_ప్రారంభం+=length) {
            length=runs[i].visual_హద్దు-visual_ప్రారంభం;
            చొప్పించు_Remove=runs[i].చొప్పించు_Remove;
            ఒకవేళ(చొప్పించు_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                ఒకవేళ(visual_సూచీ<=(visual_ప్రారంభం+mark_దొరికింది)) {
                    తిరిగి UBIDI_MAP_NOWHERE;
                }
                mark_దొరికింది++;
            }
            /* is adjusted visual index within this run? */
            ఒకవేళ(visual_సూచీ<(runs[i].visual_హద్దు+mark_దొరికింది)) {
                visual_సూచీ-=mark_దొరికింది;
                ఆపు;
            }
            ఒకవేళ(చొప్పించు_Remove&(LRM_AFTER|RLM_AFTER)) {
                ఒకవేళ(visual_సూచీ==(visual_ప్రారంభం+length+mark_దొరికింది)) {
                    తిరిగి UBIDI_MAP_NOWHERE;
                }
                mark_దొరికింది++;
            }
        }
    }
    లేకపోతే ఒకవేళ(pBiDi->control_లెక్క>0) {
        /* handle removed BiDi control characters */
        int32_t control_దొరికింది=0, చొప్పించు_Remove, length;
        int32_t logical_ప్రారంభం, logical_చివర, visual_ప్రారంభం=0, j, k;
        char16_t uchar;
        UBool even_నిరంతరం;
        /* add number of controls until visual index */
        కొరకు(i=0; ; i++, visual_ప్రారంభం+=length) {
            length=runs[i].visual_హద్దు-visual_ప్రారంభం;
            చొప్పించు_Remove=runs[i].చొప్పించు_Remove;
            /* is adjusted visual index beyond current run? */
            ఒకవేళ(visual_సూచీ>=(runs[i].visual_హద్దు-control_దొరికింది+చొప్పించు_Remove)) {
                control_దొరికింది-=చొప్పించు_Remove;
                కొనసాగు;
            }
            /* adjusted visual index is within current run */
            ఒకవేళ(చొప్పించు_Remove==0) {
                visual_సూచీ+=control_దొరికింది;
                ఆపు;
            }
            /* count non-control chars until visualIndex */
            logical_ప్రారంభం=runs[i].logical_ప్రారంభం;
            even_నిరంతరం=IS_EVEN_నిరంతరం(logical_ప్రారంభం);
            REMOVE_ODD_బిట్టు(logical_ప్రారంభం);
            logical_చివర=logical_ప్రారంభం+length-1;
            కొరకు(j=0; j<length; j++) {
                k= even_నిరంతరం ? logical_ప్రారంభం+j : logical_చివర-j;
                uchar=pBiDi->పాఠ్యం[k];
                ఒకవేళ(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_దొరికింది++;
                }
                ఒకవేళ((visual_సూచీ+control_దొరికింది)==(visual_ప్రారంభం+j)) {
                    ఆపు;
                }
            }
            visual_సూచీ+=control_దొరికింది;
            ఆపు;
        }
    }
    /* handle all cases */
    ఒకవేళ(నిరంతరం_లెక్క<=10) {
        /* linear search for the run */
        కొరకు(i=0; visual_సూచీ>=runs[i].visual_హద్దు; ++i) {}
    } లేకపోతే {
        /* binary search for the run */
        int32_t begin=0, హద్దు=నిరంతరం_లెక్క;

        /* the middle if() is guaranteed to find the run, we don't need a loop limit */
        కొరకు(;;) {
            i=(begin+హద్దు)/2;
            ఒకవేళ(visual_సూచీ>=runs[i].visual_హద్దు) {
                begin=i+1;
            } లేకపోతే ఒకవేళ(i==0 || visual_సూచీ>=runs[i-1].visual_హద్దు) {
                ఆపు;
            } లేకపోతే {
                హద్దు=i;
            }
        }
    }

    ప్రారంభం=runs[i].logical_ప్రారంభం;
    ఒకవేళ(IS_EVEN_నిరంతరం(ప్రారంభం)) {
        /* LTR */
        /* the offset in runs[i] is visualIndex-runs[i-1].visualLimit */
        ఒకవేళ(i>0) {
            visual_సూచీ-=runs[i-1].visual_హద్దు;
        }
        తిరిగి ప్రారంభం+visual_సూచీ;
    } లేకపోతే {
        /* RTL */
        తిరిగి GET_సూచీ(ప్రారంభం)+runs[i].visual_హద్దు-visual_సూచీ-1;
    }
}

U_CAPI శూన్య U_EXPORT2
ubidi_getLogicalMap(UBiDi *pBiDi, int32_t *సూచీ_Map, UError_సంకేతం *p_Error_సంకేతం) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_సంకేతం);
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_లెక్క_Runs(pBiDi, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        /* no op */
    } లేకపోతే ఒకవేళ(సూచీ_Map==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
    } లేకపోతే {
        /* fill a logical-to-visual index map using the runs[] */
        int32_t visual_ప్రారంభం, visual_హద్దు, i, j, k;
        int32_t logical_ప్రారంభం, logical_హద్దు;
        నిరంతరం *runs=pBiDi->runs;
        ఒకవేళ (pBiDi->length<=0) {
            తిరిగి;
        }
        ఒకవేళ (pBiDi->length>pBiDi->ఫలితం_నిడివి) {
            uprv_memset(సూచీ_Map, 0xFF, pBiDi->length*పరిమాణం(int32_t));
        }

        visual_ప్రారంభం=0;
        కొరకు(j=0; j<pBiDi->నిరంతరం_లెక్క; ++j) {
            logical_ప్రారంభం=GET_సూచీ(runs[j].logical_ప్రారంభం);
            visual_హద్దు=runs[j].visual_హద్దు;
            ఒకవేళ(IS_EVEN_నిరంతరం(runs[j].logical_ప్రారంభం)) {
                చేయి { /* LTR */
                    సూచీ_Map[logical_ప్రారంభం++]=visual_ప్రారంభం++;
                } అయితే(visual_ప్రారంభం<visual_హద్దు);
            } లేకపోతే {
                logical_ప్రారంభం+=visual_హద్దు-visual_ప్రారంభం;  /* logicalLimit */
                చేయి { /* RTL */
                    సూచీ_Map[--logical_ప్రారంభం]=visual_ప్రారంభం++;
                } అయితే(visual_ప్రారంభం<visual_హద్దు);
            }
            /* visualStart==visualLimit; */
        }

        ఒకవేళ(pBiDi->చొప్పించు_Points.size>0) {
            int32_t mark_దొరికింది=0, నిరంతరం_లెక్క=pBiDi->నిరంతరం_లెక్క;
            int32_t length, చొప్పించు_Remove;
            visual_ప్రారంభం=0;
            /* add number of marks found until each index */
            కొరకు(i=0; i<నిరంతరం_లెక్క; i++, visual_ప్రారంభం+=length) {
                length=runs[i].visual_హద్దు-visual_ప్రారంభం;
                చొప్పించు_Remove=runs[i].చొప్పించు_Remove;
                ఒకవేళ(చొప్పించు_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_దొరికింది++;
                }
                ఒకవేళ(mark_దొరికింది>0) {
                    logical_ప్రారంభం=GET_సూచీ(runs[i].logical_ప్రారంభం);
                    logical_హద్దు=logical_ప్రారంభం+length;
                    కొరకు(j=logical_ప్రారంభం; j<logical_హద్దు; j++) {
                        సూచీ_Map[j]+=mark_దొరికింది;
                    }
                }
                ఒకవేళ(చొప్పించు_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_దొరికింది++;
                }
            }
        }
        లేకపోతే ఒకవేళ(pBiDi->control_లెక్క>0) {
            int32_t control_దొరికింది=0, నిరంతరం_లెక్క=pBiDi->నిరంతరం_లెక్క;
            int32_t length, చొప్పించు_Remove;
            UBool even_నిరంతరం;
            char16_t uchar;
            visual_ప్రారంభం=0;
            /* subtract number of controls found until each index */
            కొరకు(i=0; i<నిరంతరం_లెక్క; i++, visual_ప్రారంభం+=length) {
                length=runs[i].visual_హద్దు-visual_ప్రారంభం;
                చొప్పించు_Remove=runs[i].చొప్పించు_Remove;
                /* no control found within previous runs nor within this run */
                ఒకవేళ((control_దొరికింది-చొప్పించు_Remove)==0) {
                    కొనసాగు;
                }
                logical_ప్రారంభం=runs[i].logical_ప్రారంభం;
                even_నిరంతరం=IS_EVEN_నిరంతరం(logical_ప్రారంభం);
                REMOVE_ODD_బిట్టు(logical_ప్రారంభం);
                logical_హద్దు=logical_ప్రారంభం+length;
                /* if no control within this run */
                ఒకవేళ(చొప్పించు_Remove==0) {
                    కొరకు(j=logical_ప్రారంభం; j<logical_హద్దు; j++) {
                        సూచీ_Map[j]-=control_దొరికింది;
                    }
                    కొనసాగు;
                }
                కొరకు(j=0; j<length; j++) {
                    k= even_నిరంతరం ? logical_ప్రారంభం+j : logical_హద్దు-j-1;
                    uchar=pBiDi->పాఠ్యం[k];
                    ఒకవేళ(IS_BIDI_CONTROL_CHAR(uchar)) {
                        control_దొరికింది++;
                        సూచీ_Map[k]=UBIDI_MAP_NOWHERE;
                        కొనసాగు;
                    }
                    సూచీ_Map[k]-=control_దొరికింది;
                }
            }
        }
    }
}

U_CAPI శూన్య U_EXPORT2
ubidi_getVisualMap(UBiDi *pBiDi, int32_t *సూచీ_Map, UError_సంకేతం *p_Error_సంకేతం) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_సంకేతం);
    ఒకవేళ(సూచీ_Map==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_లెక్క_Runs(pBiDi, p_Error_సంకేతం);
    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం)) {
        /* fill a visual-to-logical index map using the runs[] */
        నిరంతరం *runs=pBiDi->runs, *runs_హద్దు=runs+pBiDi->నిరంతరం_లెక్క;
        int32_t logical_ప్రారంభం, visual_ప్రారంభం, visual_హద్దు, *pi=సూచీ_Map;

        ఒకవేళ (pBiDi->ఫలితం_నిడివి<=0) {
            తిరిగి;
        }
        visual_ప్రారంభం=0;
        కొరకు(; runs<runs_హద్దు; ++runs) {
            logical_ప్రారంభం=runs->logical_ప్రారంభం;
            visual_హద్దు=runs->visual_హద్దు;
            ఒకవేళ(IS_EVEN_నిరంతరం(logical_ప్రారంభం)) {
                చేయి { /* LTR */
                    *pi++ = logical_ప్రారంభం++;
                } అయితే(++visual_ప్రారంభం<visual_హద్దు);
            } లేకపోతే {
                REMOVE_ODD_బిట్టు(logical_ప్రారంభం);
                logical_ప్రారంభం+=visual_హద్దు-visual_ప్రారంభం;  /* logicalLimit */
                చేయి { /* RTL */
                    *pi++ = --logical_ప్రారంభం;
                } అయితే(++visual_ప్రారంభం<visual_హద్దు);
            }
            /* visualStart==visualLimit; */
        }

        ఒకవేళ(pBiDi->చొప్పించు_Points.size>0) {
            int32_t mark_దొరికింది=0, నిరంతరం_లెక్క=pBiDi->నిరంతరం_లెక్క;
            int32_t చొప్పించు_Remove, i, j, k;
            runs=pBiDi->runs;
            /* count all inserted marks */
            కొరకు(i=0; i<నిరంతరం_లెక్క; i++) {
                చొప్పించు_Remove=runs[i].చొప్పించు_Remove;
                ఒకవేళ(చొప్పించు_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_దొరికింది++;
                }
                ఒకవేళ(చొప్పించు_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_దొరికింది++;
                }
            }
            /* move back indexes by number of preceding marks */
            k=pBiDi->ఫలితం_నిడివి;
            కొరకు(i=నిరంతరం_లెక్క-1; i>=0 && mark_దొరికింది>0; i--) {
                చొప్పించు_Remove=runs[i].చొప్పించు_Remove;
                ఒకవేళ(చొప్పించు_Remove&(LRM_AFTER|RLM_AFTER)) {
                    సూచీ_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_దొరికింది--;
                }
                visual_ప్రారంభం= i>0 ? runs[i-1].visual_హద్దు : 0;
                కొరకు(j=runs[i].visual_హద్దు-1; j>=visual_ప్రారంభం && mark_దొరికింది>0; j--) {
                    సూచీ_Map[--k]=సూచీ_Map[j];
                }
                ఒకవేళ(చొప్పించు_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    సూచీ_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_దొరికింది--;
                }
            }
        }
        లేకపోతే ఒకవేళ(pBiDi->control_లెక్క>0) {
            int32_t నిరంతరం_లెక్క=pBiDi->నిరంతరం_లెక్క, logical_చివర;
            int32_t చొప్పించు_Remove, length, i, j, k, m;
            char16_t uchar;
            UBool even_నిరంతరం;
            runs=pBiDi->runs;
            visual_ప్రారంభం=0;
            /* move forward indexes by number of preceding controls */
            k=0;
            కొరకు(i=0; i<నిరంతరం_లెక్క; i++, visual_ప్రారంభం+=length) {
                length=runs[i].visual_హద్దు-visual_ప్రారంభం;
                చొప్పించు_Remove=runs[i].చొప్పించు_Remove;
                /* if no control found yet, nothing to do in this run */
                ఒకవేళ((చొప్పించు_Remove==0)&&(k==visual_ప్రారంభం)) {
                    k+=length;
                    కొనసాగు;
                }
                /* if no control in this run */
                ఒకవేళ(చొప్పించు_Remove==0) {
                    visual_హద్దు=runs[i].visual_హద్దు;
                    కొరకు(j=visual_ప్రారంభం; j<visual_హద్దు; j++) {
                        సూచీ_Map[k++]=సూచీ_Map[j];
                    }
                    కొనసాగు;
                }
                logical_ప్రారంభం=runs[i].logical_ప్రారంభం;
                even_నిరంతరం=IS_EVEN_నిరంతరం(logical_ప్రారంభం);
                REMOVE_ODD_బిట్టు(logical_ప్రారంభం);
                logical_చివర=logical_ప్రారంభం+length-1;
                కొరకు(j=0; j<length; j++) {
                    m= even_నిరంతరం ? logical_ప్రారంభం+j : logical_చివర-j;
                    uchar=pBiDi->పాఠ్యం[m];
                    ఒకవేళ(!IS_BIDI_CONTROL_CHAR(uchar)) {
                        సూచీ_Map[k++]=m;
                    }
                }
            }
        }
    }
}

U_CAPI శూన్య U_EXPORT2
ubidi_invertMap(స్థిరరాశి int32_t *srcMap, int32_t *destMap, int32_t length) {
    ఒకవేళ(srcMap!=శూన్యసూచిక && destMap!=శూన్యసూచిక && length>0) {
        స్థిరరాశి int32_t *pi;
        int32_t dest_నిడివి=-1, count=0;
        /* find highest value and count positive indexes in srcMap */
        pi=srcMap+length;
        అయితే(pi>srcMap) {
            ఒకవేళ(*--pi>dest_నిడివి) {
                dest_నిడివి=*pi;
            }
            ఒకవేళ(*pi>=0) {
                count++;
            }
        }
        dest_నిడివి++;           /* add 1 for origin 0 */
        ఒకవేళ(count<dest_నిడివి) {
            /* we must fill unmatched destMap entries with -1 */
            uprv_memset(destMap, 0xFF, dest_నిడివి*పరిమాణం(int32_t));
        }
        pi=srcMap+length;
        అయితే(length>0) {
            ఒకవేళ(*--pi>=0) {
                destMap[*pi]=--length;
            } లేకపోతే {
                --length;
            }
        }
    }
}
