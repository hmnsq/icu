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

#ضمن "cmemory.h"
#ضمن "unicode/utypes.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/ubidi.h"
#ضمن "ubidiimp.h"
#ضمن "uassert.h"

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
ساكن فراغ
سيٽ_Trailing_WSStart(UBiDi *pBiDi) {
    /* pBiDi->direction!=UBIDI_MIXED */

    ثابت DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *سطحون=pBiDi->سطحون;
    int32_t شروعات=pBiDi->length;
    UBiDiLevel paraLevel=pBiDi->paraLevel;

    /* If the line is terminated by a block separator, all preceding WS etc...
       are already set to paragraph level.
       Setting trailingWSStart to pBidi->length will avoid changing the
       level of B chars from 0 to paraLevel in ubidi_getLevels when
       orderParagraphsLTR==true.
     */
    إذا(dirProps[شروعات-1]==B) {
        pBiDi->trailingWSStart=شروعات;   /* currently == pBiDi->length */
        أرجع;
    }
    /* go backwards across all WS, BN, explicit codes */
    طالما(شروعات>0 && DIRPROP_FLAG(dirProps[شروعات-1])&نقاب_WS) {
        --شروعات;
    }

    /* if the WS run can be merged with the previous run then do so here */
    طالما(شروعات>0 && سطحون[شروعات-1]==paraLevel) {
        --شروعات;
    }

    pBiDi->trailingWSStart=شروعات;
}

/* ubidi_setLine ------------------------------------------------------------ */

U_CAPI فراغ U_EXPORT2
ubidi_سيٽ_سٽ(ثابت UBiDi *pParaBiDi,
              int32_t شروعات, int32_t حد_بندي,
              UBiDi *p_سٽ_Bi_Di,
              UError_ڪوڊ *p_Error_ڪوڊ) {
    int32_t length;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ڪوڊ);
    RETURN_VOID_IF_NOT_VALID_PARA(pParaBiDi, *p_Error_ڪوڊ);
    RETURN_VOID_IF_BAD_RANGE(شروعات, 0, حد_بندي, *p_Error_ڪوڊ);
    RETURN_VOID_IF_BAD_RANGE(حد_بندي, 0, pParaBiDi->length+1, *p_Error_ڪوڊ);
    إذا(p_سٽ_Bi_Di==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    إذا(ubidi_getParagraph(pParaBiDi, شروعات, خالي_اشارو, خالي_اشارو, خالي_اشارو, p_Error_ڪوڊ) !=
       ubidi_getParagraph(pParaBiDi, حد_بندي-1, خالي_اشارو, خالي_اشارو, خالي_اشارو, p_Error_ڪوڊ)) {
        /* the line crosses a paragraph boundary */
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    /* set the values in pLineBiDi from its pParaBiDi parent */
    p_سٽ_Bi_Di->pParaBiDi=خالي_اشارو;          /* mark unfinished setLine */
    p_سٽ_Bi_Di->متن=pParaBiDi->متن+شروعات;
    length=p_سٽ_Bi_Di->length=حد_بندي-شروعات;
    p_سٽ_Bi_Di->نتيجو_ڊيگهائي=p_سٽ_Bi_Di->original_ڊيگهائي=length;
    p_سٽ_Bi_Di->paraLevel=GET_PARALEVEL(pParaBiDi, شروعات);
    p_سٽ_Bi_Di->para_ڳڻپ=pParaBiDi->para_ڳڻپ;
    p_سٽ_Bi_Di->runs=خالي_اشارو;
    p_سٽ_Bi_Di->flags=0;
    p_سٽ_Bi_Di->reordering_گهڻائي=pParaBiDi->reordering_گهڻائي;
    p_سٽ_Bi_Di->reorderingOptions=pParaBiDi->reorderingOptions;
    p_سٽ_Bi_Di->control_ڳڻپ=0;
    إذا(pParaBiDi->control_ڳڻپ>0) {
        int32_t j;
        لكل(j=شروعات; j<حد_بندي; j++) {
            إذا(IS_BIDI_CONTROL_CHAR(pParaBiDi->متن[j])) {
                p_سٽ_Bi_Di->control_ڳڻپ++;
            }
        }
        p_سٽ_Bi_Di->نتيجو_ڊيگهائي-=p_سٽ_Bi_Di->control_ڳڻپ;
    }

    p_سٽ_Bi_Di->dirProps=pParaBiDi->dirProps+شروعات;
    p_سٽ_Bi_Di->سطحون=pParaBiDi->سطحون+شروعات;
    p_سٽ_Bi_Di->لاڳيتو_ڳڻپ=-1;

    إذا(pParaBiDi->direction!=UBIDI_MIXED) {
        /* the parent is already trivial */
        p_سٽ_Bi_Di->direction=pParaBiDi->direction;

        /*
         * The parent's levels are all either
         * implicitly or explicitly ==paraLevel;
         * do the same here.
         */
        إذا(pParaBiDi->trailingWSStart<=شروعات) {
            p_سٽ_Bi_Di->trailingWSStart=0;
        } وإلا إذا(pParaBiDi->trailingWSStart<حد_بندي) {
            p_سٽ_Bi_Di->trailingWSStart=pParaBiDi->trailingWSStart-شروعات;
        } وإلا {
            p_سٽ_Bi_Di->trailingWSStart=length;
        }
    } وإلا {
        ثابت UBiDiLevel *سطحون=p_سٽ_Bi_Di->سطحون;
        int32_t i, trailingWSStart;
        UBiDiLevel level;

        سيٽ_Trailing_WSStart(p_سٽ_Bi_Di);
        trailingWSStart=p_سٽ_Bi_Di->trailingWSStart;

        /* recalculate pLineBiDi->direction */
        إذا(trailingWSStart==0) {
            /* all levels are at paraLevel */
            p_سٽ_Bi_Di->direction=(UBiDiDirection)(p_سٽ_Bi_Di->paraLevel&1);
        } وإلا {
            /* get the level of the first character */
            level=(UBiDiLevel)(سطحون[0]&1);

            /* if there is anything of a different level, then the line is mixed */
            إذا(trailingWSStart<length && (p_سٽ_Bi_Di->paraLevel&1)!=level) {
                /* the trailing WS is at paraLevel, which differs from levels[0] */
                p_سٽ_Bi_Di->direction=UBIDI_MIXED;
            } وإلا {
                /* see if levels[1..trailingWSStart-1] have the same direction as levels[0] and paraLevel */
                i=1;
                لكل(;;) {
                    إذا(i==trailingWSStart) {
                        /* the direction values match those in level */
                        p_سٽ_Bi_Di->direction=(UBiDiDirection)level;
                        اكسر;
                    } وإلا إذا((سطحون[i]&1)!=level) {
                        p_سٽ_Bi_Di->direction=UBIDI_MIXED;
                        اكسر;
                    }
                    ++i;
                }
            }
        }

        بدل(p_سٽ_Bi_Di->direction) {
        حالة UBIDI_LTR:
            /* make sure paraLevel is even */
            p_سٽ_Bi_Di->paraLevel=(UBiDiLevel)((p_سٽ_Bi_Di->paraLevel+1)&~1);

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_سٽ_Bi_Di->trailingWSStart=0;
            اكسر;
        حالة UBIDI_RTL:
            /* make sure paraLevel is odd */
            p_سٽ_Bi_Di->paraLevel|=1;

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_سٽ_Bi_Di->trailingWSStart=0;
            اكسر;
        افتراضي:
            اكسر;
        }
    }
    p_سٽ_Bi_Di->pParaBiDi=pParaBiDi;     /* mark successful setLine */
}

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getLevelAt(ثابت UBiDi *pBiDi, int32_t char_اشاريو) {
    /* return paraLevel if in the trailing WS run, otherwise the real level */
    إذا(!IS_VALID_PARA_OR_سٽ(pBiDi) || char_اشاريو<0 || pBiDi->length<=char_اشاريو) {
        أرجع 0;
    } وإلا إذا(pBiDi->direction!=UBIDI_MIXED || char_اشاريو>=pBiDi->trailingWSStart) {
        أرجع GET_PARALEVEL(pBiDi, char_اشاريو);
    } وإلا {
        أرجع pBiDi->سطحون[char_اشاريو];
    }
}

U_CAPI ثابت UBiDiLevel* U_EXPORT2
ubidi_get_سطحون(UBiDi* pBiDi, UError_ڪوڊ* p_Error_ڪوڊ) {
    int32_t شروعات, length;

    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ڪوڊ, خالي_اشارو);
    RETURN_IF_NOT_VALID_PARA_OR_سٽ(pBiDi, *p_Error_ڪوڊ, خالي_اشارو);
    إذا((length=pBiDi->length)<=0) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    إذا((شروعات=pBiDi->trailingWSStart)==length) {
        /* the current levels array reflects the WS run */
        أرجع pBiDi->سطحون;
    }

    /*
     * After the previous if(), we know that the levels array
     * has an implicit trailing WS run and therefore does not fully
     * reflect itself all the levels.
     * This must be a UBiDi object for a line, and
     * we need to create a new levels array.
     */
    إذا(get_سطحون_يادگيري(pBiDi, length)) {
        UBiDiLevel *سطحون=pBiDi->سطحون_يادگيري;

        إذا(شروعات>0 && سطحون!=pBiDi->سطحون) {
            uprv_memcpy(سطحون, pBiDi->سطحون, شروعات);
        }
        /* pBiDi->paraLevel is ok even if contextual multiple paragraphs,
           since pBidi is a line object                                     */
        uprv_memset(سطحون+شروعات, pBiDi->paraLevel, length-شروعات);

        /* this new levels array is set for the line and reflects the WS run */
        pBiDi->trailingWSStart=length;
        أرجع pBiDi->سطحون=سطحون;
    } وإلا {
        /* out of memory */
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_get_Logical_لاڳيتو(ثابت UBiDi *pBiDi, int32_t logicalPosition,
                    int32_t *p_Logical_حد_بندي, UBiDiLevel *pLevel) {
    UError_ڪوڊ error_ڪوڊ;
    int32_t لاڳيتو_ڳڻپ, visual_شروعات, logical_حد_بندي, logical_پهريون, i;
    لاڳيتو i_لاڳيتو;

    error_ڪوڊ=U_ZERO_ERROR;
    RETURN_VOID_IF_BAD_RANGE(logicalPosition, 0, pBiDi->length, error_ڪوڊ);
    /* ubidi_countRuns will check VALID_PARA_OR_LINE */
    لاڳيتو_ڳڻپ=ubidi_ڳڻپ_Runs((UBiDi *)pBiDi, &error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    /* this is done based on runs rather than on levels since levels have
       a special interpretation when UBIDI_REORDER_RUNS_ONLY
     */
    visual_شروعات=logical_حد_بندي=0;
    i_لاڳيتو=pBiDi->runs[0];

    لكل(i=0; i<لاڳيتو_ڳڻپ; i++) {
        i_لاڳيتو = pBiDi->runs[i];
        logical_پهريون=GET_اشاريو(i_لاڳيتو.logical_شروعات);
        logical_حد_بندي=logical_پهريون+i_لاڳيتو.visual_حد_بندي-visual_شروعات;
        إذا((logicalPosition>=logical_پهريون) &&
           (logicalPosition<logical_حد_بندي)) {
            اكسر;
        }
        visual_شروعات = i_لاڳيتو.visual_حد_بندي;
    }
    إذا(p_Logical_حد_بندي) {
        *p_Logical_حد_بندي=logical_حد_بندي;
    }
    إذا(pLevel) {
        إذا(pBiDi->reordering_گهڻائي==UBIDI_REORDER_RUNS_ONLY) {
            *pLevel=(UBiDiLevel)GET_ODD_ٻٽو_انگ(i_لاڳيتو.logical_شروعات);
        }
        وإلا إذا(pBiDi->direction!=UBIDI_MIXED || logicalPosition>=pBiDi->trailingWSStart) {
            *pLevel=GET_PARALEVEL(pBiDi, logicalPosition);
        } وإلا {
        *pLevel=pBiDi->سطحون[logicalPosition];
        }
    }
}

/* runs API functions ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_ڳڻپ_Runs(UBiDi *pBiDi, UError_ڪوڊ *p_Error_ڪوڊ) {
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ڪوڊ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_سٽ(pBiDi, *p_Error_ڪوڊ, -1);
    ubidi_getRuns(pBiDi, p_Error_ڪوڊ);
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع -1;
    }
    أرجع pBiDi->لاڳيتو_ڳڻپ;
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_Visual_لاڳيتو(UBiDi *pBiDi, int32_t لاڳيتو_اشاريو,
                   int32_t *p_Logical_شروعات, int32_t *p_ڊيگهائي)
{
    int32_t شروعات;
    UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
    RETURN_IF_NOT_VALID_PARA_OR_سٽ(pBiDi, error_ڪوڊ, UBIDI_LTR);
    ubidi_getRuns(pBiDi, &error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع UBIDI_LTR;
    }
    RETURN_IF_BAD_RANGE(لاڳيتو_اشاريو, 0, pBiDi->لاڳيتو_ڳڻپ, error_ڪوڊ, UBIDI_LTR);

    شروعات=pBiDi->runs[لاڳيتو_اشاريو].logical_شروعات;
    إذا(p_Logical_شروعات!=خالي_اشارو) {
        *p_Logical_شروعات=GET_اشاريو(شروعات);
    }
    إذا(p_ڊيگهائي!=خالي_اشارو) {
        إذا(لاڳيتو_اشاريو>0) {
            *p_ڊيگهائي=pBiDi->runs[لاڳيتو_اشاريو].visual_حد_بندي-
                     pBiDi->runs[لاڳيتو_اشاريو-1].visual_حد_بندي;
        } وإلا {
            *p_ڊيگهائي=pBiDi->runs[0].visual_حد_بندي;
        }
    }
    أرجع (UBiDiDirection)GET_ODD_ٻٽو_انگ(شروعات);
}

/* in trivial cases there is only one trivial run; called by ubidi_getRuns() */
ساكن فراغ
get_Single_لاڳيتو(UBiDi *pBiDi, UBiDiLevel level) {
    /* simple, single-run case */
    pBiDi->runs=pBiDi->simpleRuns;
    pBiDi->لاڳيتو_ڳڻپ=1;

    /* fill and reorder the single run */
    pBiDi->runs[0].logical_شروعات=MAKE_اشاريو_ODD_PAIR(0, level);
    pBiDi->runs[0].visual_حد_بندي=pBiDi->length;
    pBiDi->runs[0].وجهو_Remove=0;
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
ساكن فراغ
reorder_سٽ(UBiDi *pBiDi, UBiDiLevel minLevel, UBiDiLevel maxLevel) {
    لاڳيتو *runs, عارضي_لاڳيتو;
    UBiDiLevel *سطحون;
    int32_t پهريون_لاڳيتو, پڇاڙي_لاڳيتو, حد_بندي_لاڳيتو, لاڳيتو_ڳڻپ;

    /* nothing to do? */
    إذا(maxLevel<=(minLevel|1)) {
        أرجع;
    }

    /*
     * Reorder only down to the lowest odd level
     * and reorder at an odd minLevel in a separate, simpler loop.
     * See comments above for why minLevel is always incremented.
     */
    ++minLevel;

    runs=pBiDi->runs;
    سطحون=pBiDi->سطحون;
    لاڳيتو_ڳڻپ=pBiDi->لاڳيتو_ڳڻپ;

    /* do not include the WS run at paraLevel<=old minLevel except in the simple loop */
    إذا(pBiDi->trailingWSStart<pBiDi->length) {
        --لاڳيتو_ڳڻپ;
    }

    طالما(--maxLevel>=minLevel) {
        پهريون_لاڳيتو=0;

        /* loop for all sequences of runs */
        لكل(;;) {
            /* look for a sequence of runs that are all at >=maxLevel */
            /* look for the first run of such a sequence */
            طالما(پهريون_لاڳيتو<لاڳيتو_ڳڻپ && سطحون[runs[پهريون_لاڳيتو].logical_شروعات]<maxLevel) {
                ++پهريون_لاڳيتو;
            }
            إذا(پهريون_لاڳيتو>=لاڳيتو_ڳڻپ) {
                اكسر;  /* no more such runs */
            }

            /* look for the limit run of such a sequence (the run behind it) */
            لكل(حد_بندي_لاڳيتو=پهريون_لاڳيتو; ++حد_بندي_لاڳيتو<لاڳيتو_ڳڻپ && سطحون[runs[حد_بندي_لاڳيتو].logical_شروعات]>=maxLevel;) {}

            /* Swap the entire sequence of runs from firstRun to limitRun-1. */
            پڇاڙي_لاڳيتو=حد_بندي_لاڳيتو-1;
            طالما(پهريون_لاڳيتو<پڇاڙي_لاڳيتو) {
                عارضي_لاڳيتو = runs[پهريون_لاڳيتو];
                runs[پهريون_لاڳيتو]=runs[پڇاڙي_لاڳيتو];
                runs[پڇاڙي_لاڳيتو]=عارضي_لاڳيتو;
                ++پهريون_لاڳيتو;
                --پڇاڙي_لاڳيتو;
            }

            إذا(حد_بندي_لاڳيتو==لاڳيتو_ڳڻپ) {
                اكسر;  /* no more such runs */
            } وإلا {
                پهريون_لاڳيتو=حد_بندي_لاڳيتو+1;
            }
        }
    }

    /* now do maxLevel==old minLevel (==odd!), see above */
    إذا(!(minLevel&1)) {
        پهريون_لاڳيتو=0;

        /* include the trailing WS run in this complete reordering */
        إذا(pBiDi->trailingWSStart==pBiDi->length) {
            --لاڳيتو_ڳڻپ;
        }

        /* Swap the entire sequence of all runs. (endRun==runCount) */
        طالما(پهريون_لاڳيتو<لاڳيتو_ڳڻپ) {
            عارضي_لاڳيتو=runs[پهريون_لاڳيتو];
            runs[پهريون_لاڳيتو]=runs[لاڳيتو_ڳڻپ];
            runs[لاڳيتو_ڳڻپ]=عارضي_لاڳيتو;
            ++پهريون_لاڳيتو;
            --لاڳيتو_ڳڻپ;
        }
    }
}

/* compute the runs array --------------------------------------------------- */

ساكن int32_t get_لاڳيتو_From_Logical_اشاريو(UBiDi *pBiDi, int32_t logical_اشاريو) {
    لاڳيتو *runs=pBiDi->runs;
    int32_t لاڳيتو_ڳڻپ=pBiDi->لاڳيتو_ڳڻپ, visual_شروعات=0, i, length, logical_شروعات;

    لكل(i=0; i<لاڳيتو_ڳڻپ; i++) {
        length=runs[i].visual_حد_بندي-visual_شروعات;
        logical_شروعات=GET_اشاريو(runs[i].logical_شروعات);
        إذا((logical_اشاريو>=logical_شروعات) && (logical_اشاريو<(logical_شروعات+length))) {
            أرجع i;
        }
        visual_شروعات+=length;
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
ubidi_getRuns(UBiDi *pBiDi, UError_ڪوڊ* p_Error_ڪوڊ) {
    /*
     * This method returns immediately if the runs are already set. This
     * includes the case of length==0 (handled in setPara)..
     */
    إذا (pBiDi->لاڳيتو_ڳڻپ>=0) {
        أرجع سچ;
    }

    إذا(pBiDi->direction!=UBIDI_MIXED) {
        /* simple, single-run case - this covers length==0 */
        /* pBiDi->paraLevel is ok even for contextual multiple paragraphs */
        get_Single_لاڳيتو(pBiDi, pBiDi->paraLevel);
    } وإلا /* UBIDI_MIXED, length>0 */ {
        /* mixed directionality */
        int32_t length=pBiDi->length, حد_بندي;
        UBiDiLevel *سطحون=pBiDi->سطحون;
        int32_t i, لاڳيتو_ڳڻپ;
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
        حد_بندي=pBiDi->trailingWSStart;
        /* count the runs, there is at least one non-WS run, and limit>0 */
        لاڳيتو_ڳڻپ=0;
        لكل(i=0; i<حد_بندي; ++i) {
            /* increment runCount at the start of each run */
            إذا(سطحون[i]!=level) {
                ++لاڳيتو_ڳڻپ;
                level=سطحون[i];
            }
        }

        /*
         * We don't need to see if the last run can be merged with a trailing
         * WS run because setTrailingWSStart() would have done that.
         */
        إذا(لاڳيتو_ڳڻپ==1 && حد_بندي==length) {
            /* There is only one non-WS run and no trailing WS-run. */
            get_Single_لاڳيتو(pBiDi, سطحون[0]);
        } وإلا /* runCount>1 || limit<length */ {
            /* allocate and set the runs */
            لاڳيتو *runs;
            int32_t لاڳيتو_اشاريو, شروعات;
            UBiDiLevel minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1, maxLevel=0;

            /* now, count a (non-mergeable) WS run */
            إذا(حد_بندي<length) {
                ++لاڳيتو_ڳڻپ;
            }

            /* runCount>1 */
            إذا(get_Runs_يادگيري(pBiDi, لاڳيتو_ڳڻپ)) {
                runs=pBiDi->runs_يادگيري;
            } وإلا {
                *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
                أرجع خطأ;
            }

            /* set the runs */
            /* FOOD FOR THOUGHT: this could be optimized, e.g.:
             * 464->444, 484->444, 575->555, 595->555
             * However, that would take longer. Check also how it would
             * interact with BiDi control removal and inserting Marks.
             */
            لاڳيتو_اشاريو=0;

            /* search for the run limits and initialize visualLimit values with the run lengths */
            i=0;
            افعل {
                /* prepare this run */
                شروعات=i;
                level=سطحون[i];
                إذا(level<minLevel) {
                    minLevel=level;
                }
                إذا(level>maxLevel) {
                    maxLevel=level;
                }

                /* look for the run limit */
                طالما(++i<حد_بندي && سطحون[i]==level) {}

                /* i is another run limit */
                runs[لاڳيتو_اشاريو].logical_شروعات=شروعات;
                runs[لاڳيتو_اشاريو].visual_حد_بندي=i-شروعات;
                runs[لاڳيتو_اشاريو].وجهو_Remove=0;
                ++لاڳيتو_اشاريو;
            } طالما(i<حد_بندي);

            إذا(حد_بندي<length) {
                /* there is a separate WS run */
                runs[لاڳيتو_اشاريو].logical_شروعات=حد_بندي;
                runs[لاڳيتو_اشاريو].visual_حد_بندي=length-حد_بندي;
                /* For the trailing WS run, pBiDi->paraLevel is ok even
                   if contextual multiple paragraphs.                   */
                إذا(pBiDi->paraLevel<minLevel) {
                    minLevel=pBiDi->paraLevel;
                }
            }

            /* set the object fields */
            pBiDi->runs=runs;
            pBiDi->لاڳيتو_ڳڻپ=لاڳيتو_ڳڻپ;

            reorder_سٽ(pBiDi, minLevel, maxLevel);

            /* now add the direction flags and adjust the visualLimit's to be just that */
            /* this loop will also handle the trailing WS run */
            حد_بندي=0;
            لكل(i=0; i<لاڳيتو_ڳڻپ; ++i) {
                جوڙيو_ODD_ٻٽو_انگ_FROM_LEVEL(runs[i].logical_شروعات, سطحون[runs[i].logical_شروعات]);
                حد_بندي+=runs[i].visual_حد_بندي;
                runs[i].visual_حد_بندي=حد_بندي;
            }

            /* Set the "odd" bit for the trailing WS run. */
            /* For a RTL paragraph, it will be the *first* run in visual order. */
            /* For the trailing WS run, pBiDi->paraLevel is ok even if
               contextual multiple paragraphs.                          */
            إذا(لاڳيتو_اشاريو<لاڳيتو_ڳڻپ) {
                int32_t trailing_لاڳيتو = ((pBiDi->paraLevel & 1) != 0)? 0 : لاڳيتو_اشاريو;

                جوڙيو_ODD_ٻٽو_انگ_FROM_LEVEL(runs[trailing_لاڳيتو].logical_شروعات, pBiDi->paraLevel);
            }
        }
    }

    /* handle insert LRM/RLM BEFORE/AFTER run */
    إذا(pBiDi->وجهو_Points.size>0) {
        نقطو *نقطو_2, *شروعات=pBiDi->وجهو_Points.points,
                      *حد_بندي=شروعات+pBiDi->وجهو_Points.size;
        int32_t لاڳيتو_اشاريو;
        لكل(نقطو_2=شروعات; نقطو_2<حد_بندي; نقطو_2++) {
            لاڳيتو_اشاريو=get_لاڳيتو_From_Logical_اشاريو(pBiDi, نقطو_2->pos);
            pBiDi->runs[لاڳيتو_اشاريو].وجهو_Remove|=نقطو_2->flag;
        }
    }

    /* handle remove BiDi control characters */
    إذا(pBiDi->control_ڳڻپ>0) {
        int32_t لاڳيتو_اشاريو;
        ثابت char16_t *شروعات=pBiDi->متن, *حد_بندي=شروعات+pBiDi->length, *pu;
        لكل(pu=شروعات; pu<حد_بندي; pu++) {
            إذا(IS_BIDI_CONTROL_CHAR(*pu)) {
                لاڳيتو_اشاريو=get_لاڳيتو_From_Logical_اشاريو(pBiDi, (int32_t)(pu-شروعات));
                pBiDi->runs[لاڳيتو_اشاريو].وجهو_Remove--;
            }
        }
    }

    أرجع سچ;
}

ساكن UBool
prepareReorder(ثابت UBiDiLevel *سطحون, int32_t length,
               int32_t *اشاريو_Map,
               UBiDiLevel *pMinLevel, UBiDiLevel *pMaxLevel) {
    int32_t شروعات;
    UBiDiLevel level, minLevel, maxLevel;

    إذا(سطحون==خالي_اشارو || length<=0) {
        أرجع خطأ;
    }

    /* determine minLevel and maxLevel */
    minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1;
    maxLevel=0;
    لكل(شروعات=length; شروعات>0;) {
        level=سطحون[--شروعات];
        إذا(level>UBIDI_MAX_EXPLICIT_LEVEL+1) {
            أرجع خطأ;
        }
        إذا(level<minLevel) {
            minLevel=level;
        }
        إذا(level>maxLevel) {
            maxLevel=level;
        }
    }
    *pMinLevel=minLevel;
    *pMaxLevel=maxLevel;

    /* initialize the index map */
    لكل(شروعات=length; شروعات>0;) {
        --شروعات;
        اشاريو_Map[شروعات]=شروعات;
    }

    أرجع سچ;
}

/* reorder a line based on a levels array (L2) ------------------------------ */

U_CAPI فراغ U_EXPORT2
ubidi_reorderLogical(ثابت UBiDiLevel *سطحون, int32_t length, int32_t *اشاريو_Map) {
    int32_t شروعات, حد_بندي, جوڙ_جو_Sos_Eos;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    إذا(اشاريو_Map==خالي_اشارو || !prepareReorder(سطحون, length, اشاريو_Map, &minLevel, &maxLevel)) {
        أرجع;
    }

    /* nothing to do? */
    إذا(minLevel==maxLevel && (minLevel&1)==0) {
        أرجع;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    افعل {
        شروعات=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        لكل(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            طالما(شروعات<length && سطحون[شروعات]<maxLevel) {
                ++شروعات;
            }
            إذا(شروعات>=length) {
                اكسر;  /* no more such sequences */
            }

            /* look for the limit of such a sequence (the index behind it) */
            لكل(حد_بندي=شروعات; ++حد_بندي<length && سطحون[حد_بندي]>=maxLevel;) {}

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
            جوڙ_جو_Sos_Eos=شروعات+حد_بندي-1;

            /* reorder each index in the sequence */
            افعل {
                اشاريو_Map[شروعات]=جوڙ_جو_Sos_Eos-اشاريو_Map[شروعات];
            } طالما(++شروعات<حد_بندي);

            /* start==limit */
            إذا(حد_بندي==length) {
                اكسر;  /* no more such sequences */
            } وإلا {
                شروعات=حد_بندي+1;
            }
        }
    } طالما(--maxLevel>=minLevel);
}

U_CAPI فراغ U_EXPORT2
ubidi_reorderVisual(ثابت UBiDiLevel *سطحون, int32_t length, int32_t *اشاريو_Map) {
    int32_t شروعات, پڇاڙي, حد_بندي, عارضي;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    إذا(اشاريو_Map==خالي_اشارو || !prepareReorder(سطحون, length, اشاريو_Map, &minLevel, &maxLevel)) {
        أرجع;
    }

    /* nothing to do? */
    إذا(minLevel==maxLevel && (minLevel&1)==0) {
        أرجع;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    افعل {
        شروعات=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        لكل(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            طالما(شروعات<length && سطحون[شروعات]<maxLevel) {
                ++شروعات;
            }
            إذا(شروعات>=length) {
                اكسر;  /* no more such runs */
            }

            /* look for the limit of such a sequence (the index behind it) */
            لكل(حد_بندي=شروعات; ++حد_بندي<length && سطحون[حد_بندي]>=maxLevel;) {}

            /*
             * Swap the entire interval of indexes from start to limit-1.
             * We don't need to swap the levels for the purpose of this
             * algorithm: the sequence of levels that we look at does not
             * move anyway.
             */
            پڇاڙي=حد_بندي-1;
            طالما(شروعات<پڇاڙي) {
                عارضي=اشاريو_Map[شروعات];
                اشاريو_Map[شروعات]=اشاريو_Map[پڇاڙي];
                اشاريو_Map[پڇاڙي]=عارضي;

                ++شروعات;
                --پڇاڙي;
            }

            إذا(حد_بندي==length) {
                اكسر;  /* no more such sequences */
            } وإلا {
                شروعات=حد_بندي+1;
            }
        }
    } طالما(--maxLevel>=minLevel);
}

/* API functions for logical<->visual mapping ------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_get_Visual_اشاريو(UBiDi *pBiDi, int32_t logical_اشاريو, UError_ڪوڊ *p_Error_ڪوڊ) {
    int32_t visual_اشاريو=UBIDI_MAP_NOWHERE;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ڪوڊ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_سٽ(pBiDi, *p_Error_ڪوڊ, -1);
    RETURN_IF_BAD_RANGE(logical_اشاريو, 0, pBiDi->length, *p_Error_ڪوڊ, -1);

    /* we can do the trivial cases without the runs array */
    بدل(pBiDi->direction) {
    حالة UBIDI_LTR:
        visual_اشاريو=logical_اشاريو;
        اكسر;
    حالة UBIDI_RTL:
        visual_اشاريو=pBiDi->length-logical_اشاريو-1;
        اكسر;
    افتراضي:
        إذا(!ubidi_getRuns(pBiDi, p_Error_ڪوڊ)) {
            *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            أرجع -1;
        } وإلا {
            لاڳيتو *runs=pBiDi->runs;
            int32_t i, visual_شروعات=0, وٿي, length;

            /* linear search for the run, search on the visual runs */
            لكل(i=0; i<pBiDi->لاڳيتو_ڳڻپ; ++i) {
                length=runs[i].visual_حد_بندي-visual_شروعات;
                وٿي=logical_اشاريو-GET_اشاريو(runs[i].logical_شروعات);
                إذا(وٿي>=0 && وٿي<length) {
                    إذا(IS_EVEN_لاڳيتو(runs[i].logical_شروعات)) {
                        /* LTR */
                        visual_اشاريو=visual_شروعات+وٿي;
                    } وإلا {
                        /* RTL */
                        visual_اشاريو=visual_شروعات+length-وٿي-1;
                    }
                    اكسر;          /* exit for loop */
                }
                visual_شروعات+=length;
            }
            إذا(i>=pBiDi->لاڳيتو_ڳڻپ) {
                أرجع UBIDI_MAP_NOWHERE;
            }
        }
    }

    إذا(pBiDi->وجهو_Points.size>0) {
        /* add the number of added marks until the calculated visual index */
        لاڳيتو *runs=pBiDi->runs;
        int32_t i, length, وجهو_Remove;
        int32_t visual_شروعات=0, mark_مليو=0;
        لكل(i=0; ; i++, visual_شروعات+=length) {
            length=runs[i].visual_حد_بندي-visual_شروعات;
            وجهو_Remove=runs[i].وجهو_Remove;
            إذا(وجهو_Remove & (LRM_BEFORE|RLM_BEFORE)) {
                mark_مليو++;
            }
            /* is it the run containing the visual index? */
            إذا(visual_اشاريو<runs[i].visual_حد_بندي) {
                أرجع visual_اشاريو+mark_مليو;
            }
            إذا(وجهو_Remove & (LRM_AFTER|RLM_AFTER)) {
                mark_مليو++;
            }
        }
    }
    وإلا إذا(pBiDi->control_ڳڻپ>0) {
        /* subtract the number of controls until the calculated visual index */
        لاڳيتو *runs=pBiDi->runs;
        int32_t i, j, شروعات, حد_بندي, length, وجهو_Remove;
        int32_t visual_شروعات=0, control_مليو=0;
        char16_t uchar=pBiDi->متن[logical_اشاريو];
        /* is the logical index pointing to a control ? */
        إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
            أرجع UBIDI_MAP_NOWHERE;
        }
        /* loop on runs */
        لكل(i=0; ; i++, visual_شروعات+=length) {
            length=runs[i].visual_حد_بندي-visual_شروعات;
            وجهو_Remove=runs[i].وجهو_Remove;
            /* calculated visual index is beyond this run? */
            إذا(visual_اشاريو>=runs[i].visual_حد_بندي) {
                control_مليو-=وجهو_Remove;
                استمر;
            }
            /* calculated visual index must be within current run */
            إذا(وجهو_Remove==0) {
                أرجع visual_اشاريو-control_مليو;
            }
            إذا(IS_EVEN_لاڳيتو(runs[i].logical_شروعات)) {
                /* LTR: check from run start to logical index */
                شروعات=runs[i].logical_شروعات;
                حد_بندي=logical_اشاريو;
            } وإلا {
                /* RTL: check from logical index to run end */
                شروعات=logical_اشاريو+1;
                حد_بندي=GET_اشاريو(runs[i].logical_شروعات)+length;
            }
            لكل(j=شروعات; j<حد_بندي; j++) {
                uchar=pBiDi->متن[j];
                إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_مليو++;
                }
            }
            أرجع visual_اشاريو-control_مليو;
        }
    }

    أرجع visual_اشاريو;
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Logical_اشاريو(UBiDi *pBiDi, int32_t visual_اشاريو, UError_ڪوڊ *p_Error_ڪوڊ) {
    لاڳيتو *runs;
    int32_t i, لاڳيتو_ڳڻپ, شروعات;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ڪوڊ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_سٽ(pBiDi, *p_Error_ڪوڊ, -1);
    RETURN_IF_BAD_RANGE(visual_اشاريو, 0, pBiDi->نتيجو_ڊيگهائي, *p_Error_ڪوڊ, -1);
    /* we can do the trivial cases without the runs array */
    إذا(pBiDi->وجهو_Points.size==0 && pBiDi->control_ڳڻپ==0) {
        إذا(pBiDi->direction==UBIDI_LTR) {
            أرجع visual_اشاريو;
        }
        وإلا إذا(pBiDi->direction==UBIDI_RTL) {
            أرجع pBiDi->length-visual_اشاريو-1;
        }
    }
    إذا(!ubidi_getRuns(pBiDi, p_Error_ڪوڊ)) {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع -1;
    }

    runs=pBiDi->runs;
    لاڳيتو_ڳڻپ=pBiDi->لاڳيتو_ڳڻپ;
    إذا(pBiDi->وجهو_Points.size>0) {
        /* handle inserted LRM/RLM */
        int32_t mark_مليو=0, وجهو_Remove;
        int32_t visual_شروعات=0, length;
        runs=pBiDi->runs;
        /* subtract number of marks until visual index */
        لكل(i=0; ; i++, visual_شروعات+=length) {
            length=runs[i].visual_حد_بندي-visual_شروعات;
            وجهو_Remove=runs[i].وجهو_Remove;
            إذا(وجهو_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                إذا(visual_اشاريو<=(visual_شروعات+mark_مليو)) {
                    أرجع UBIDI_MAP_NOWHERE;
                }
                mark_مليو++;
            }
            /* is adjusted visual index within this run? */
            إذا(visual_اشاريو<(runs[i].visual_حد_بندي+mark_مليو)) {
                visual_اشاريو-=mark_مليو;
                اكسر;
            }
            إذا(وجهو_Remove&(LRM_AFTER|RLM_AFTER)) {
                إذا(visual_اشاريو==(visual_شروعات+length+mark_مليو)) {
                    أرجع UBIDI_MAP_NOWHERE;
                }
                mark_مليو++;
            }
        }
    }
    وإلا إذا(pBiDi->control_ڳڻپ>0) {
        /* handle removed BiDi control characters */
        int32_t control_مليو=0, وجهو_Remove, length;
        int32_t logical_شروعات, logical_پڇاڙي, visual_شروعات=0, j, k;
        char16_t uchar;
        UBool even_لاڳيتو;
        /* add number of controls until visual index */
        لكل(i=0; ; i++, visual_شروعات+=length) {
            length=runs[i].visual_حد_بندي-visual_شروعات;
            وجهو_Remove=runs[i].وجهو_Remove;
            /* is adjusted visual index beyond current run? */
            إذا(visual_اشاريو>=(runs[i].visual_حد_بندي-control_مليو+وجهو_Remove)) {
                control_مليو-=وجهو_Remove;
                استمر;
            }
            /* adjusted visual index is within current run */
            إذا(وجهو_Remove==0) {
                visual_اشاريو+=control_مليو;
                اكسر;
            }
            /* count non-control chars until visualIndex */
            logical_شروعات=runs[i].logical_شروعات;
            even_لاڳيتو=IS_EVEN_لاڳيتو(logical_شروعات);
            REMOVE_ODD_ٻٽو_انگ(logical_شروعات);
            logical_پڇاڙي=logical_شروعات+length-1;
            لكل(j=0; j<length; j++) {
                k= even_لاڳيتو ? logical_شروعات+j : logical_پڇاڙي-j;
                uchar=pBiDi->متن[k];
                إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_مليو++;
                }
                إذا((visual_اشاريو+control_مليو)==(visual_شروعات+j)) {
                    اكسر;
                }
            }
            visual_اشاريو+=control_مليو;
            اكسر;
        }
    }
    /* handle all cases */
    إذا(لاڳيتو_ڳڻپ<=10) {
        /* linear search for the run */
        لكل(i=0; visual_اشاريو>=runs[i].visual_حد_بندي; ++i) {}
    } وإلا {
        /* binary search for the run */
        int32_t begin=0, حد_بندي=لاڳيتو_ڳڻپ;

        /* the middle if() is guaranteed to find the run, we don't need a loop limit */
        لكل(;;) {
            i=(begin+حد_بندي)/2;
            إذا(visual_اشاريو>=runs[i].visual_حد_بندي) {
                begin=i+1;
            } وإلا إذا(i==0 || visual_اشاريو>=runs[i-1].visual_حد_بندي) {
                اكسر;
            } وإلا {
                حد_بندي=i;
            }
        }
    }

    شروعات=runs[i].logical_شروعات;
    إذا(IS_EVEN_لاڳيتو(شروعات)) {
        /* LTR */
        /* the offset in runs[i] is visualIndex-runs[i-1].visualLimit */
        إذا(i>0) {
            visual_اشاريو-=runs[i-1].visual_حد_بندي;
        }
        أرجع شروعات+visual_اشاريو;
    } وإلا {
        /* RTL */
        أرجع GET_اشاريو(شروعات)+runs[i].visual_حد_بندي-visual_اشاريو-1;
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_getLogicalMap(UBiDi *pBiDi, int32_t *اشاريو_Map, UError_ڪوڊ *p_Error_ڪوڊ) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ڪوڊ);
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_ڳڻپ_Runs(pBiDi, p_Error_ڪوڊ);
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        /* no op */
    } وإلا إذا(اشاريو_Map==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
    } وإلا {
        /* fill a logical-to-visual index map using the runs[] */
        int32_t visual_شروعات, visual_حد_بندي, i, j, k;
        int32_t logical_شروعات, logical_حد_بندي;
        لاڳيتو *runs=pBiDi->runs;
        إذا (pBiDi->length<=0) {
            أرجع;
        }
        إذا (pBiDi->length>pBiDi->نتيجو_ڊيگهائي) {
            uprv_memset(اشاريو_Map, 0xFF, pBiDi->length*حجم(int32_t));
        }

        visual_شروعات=0;
        لكل(j=0; j<pBiDi->لاڳيتو_ڳڻپ; ++j) {
            logical_شروعات=GET_اشاريو(runs[j].logical_شروعات);
            visual_حد_بندي=runs[j].visual_حد_بندي;
            إذا(IS_EVEN_لاڳيتو(runs[j].logical_شروعات)) {
                افعل { /* LTR */
                    اشاريو_Map[logical_شروعات++]=visual_شروعات++;
                } طالما(visual_شروعات<visual_حد_بندي);
            } وإلا {
                logical_شروعات+=visual_حد_بندي-visual_شروعات;  /* logicalLimit */
                افعل { /* RTL */
                    اشاريو_Map[--logical_شروعات]=visual_شروعات++;
                } طالما(visual_شروعات<visual_حد_بندي);
            }
            /* visualStart==visualLimit; */
        }

        إذا(pBiDi->وجهو_Points.size>0) {
            int32_t mark_مليو=0, لاڳيتو_ڳڻپ=pBiDi->لاڳيتو_ڳڻپ;
            int32_t length, وجهو_Remove;
            visual_شروعات=0;
            /* add number of marks found until each index */
            لكل(i=0; i<لاڳيتو_ڳڻپ; i++, visual_شروعات+=length) {
                length=runs[i].visual_حد_بندي-visual_شروعات;
                وجهو_Remove=runs[i].وجهو_Remove;
                إذا(وجهو_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_مليو++;
                }
                إذا(mark_مليو>0) {
                    logical_شروعات=GET_اشاريو(runs[i].logical_شروعات);
                    logical_حد_بندي=logical_شروعات+length;
                    لكل(j=logical_شروعات; j<logical_حد_بندي; j++) {
                        اشاريو_Map[j]+=mark_مليو;
                    }
                }
                إذا(وجهو_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_مليو++;
                }
            }
        }
        وإلا إذا(pBiDi->control_ڳڻپ>0) {
            int32_t control_مليو=0, لاڳيتو_ڳڻپ=pBiDi->لاڳيتو_ڳڻپ;
            int32_t length, وجهو_Remove;
            UBool even_لاڳيتو;
            char16_t uchar;
            visual_شروعات=0;
            /* subtract number of controls found until each index */
            لكل(i=0; i<لاڳيتو_ڳڻپ; i++, visual_شروعات+=length) {
                length=runs[i].visual_حد_بندي-visual_شروعات;
                وجهو_Remove=runs[i].وجهو_Remove;
                /* no control found within previous runs nor within this run */
                إذا((control_مليو-وجهو_Remove)==0) {
                    استمر;
                }
                logical_شروعات=runs[i].logical_شروعات;
                even_لاڳيتو=IS_EVEN_لاڳيتو(logical_شروعات);
                REMOVE_ODD_ٻٽو_انگ(logical_شروعات);
                logical_حد_بندي=logical_شروعات+length;
                /* if no control within this run */
                إذا(وجهو_Remove==0) {
                    لكل(j=logical_شروعات; j<logical_حد_بندي; j++) {
                        اشاريو_Map[j]-=control_مليو;
                    }
                    استمر;
                }
                لكل(j=0; j<length; j++) {
                    k= even_لاڳيتو ? logical_شروعات+j : logical_حد_بندي-j-1;
                    uchar=pBiDi->متن[k];
                    إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
                        control_مليو++;
                        اشاريو_Map[k]=UBIDI_MAP_NOWHERE;
                        استمر;
                    }
                    اشاريو_Map[k]-=control_مليو;
                }
            }
        }
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_getVisualMap(UBiDi *pBiDi, int32_t *اشاريو_Map, UError_ڪوڊ *p_Error_ڪوڊ) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ڪوڊ);
    إذا(اشاريو_Map==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_ڳڻپ_Runs(pBiDi, p_Error_ڪوڊ);
    إذا(U_SUCCESS(*p_Error_ڪوڊ)) {
        /* fill a visual-to-logical index map using the runs[] */
        لاڳيتو *runs=pBiDi->runs, *runs_حد_بندي=runs+pBiDi->لاڳيتو_ڳڻپ;
        int32_t logical_شروعات, visual_شروعات, visual_حد_بندي, *pi=اشاريو_Map;

        إذا (pBiDi->نتيجو_ڊيگهائي<=0) {
            أرجع;
        }
        visual_شروعات=0;
        لكل(; runs<runs_حد_بندي; ++runs) {
            logical_شروعات=runs->logical_شروعات;
            visual_حد_بندي=runs->visual_حد_بندي;
            إذا(IS_EVEN_لاڳيتو(logical_شروعات)) {
                افعل { /* LTR */
                    *pi++ = logical_شروعات++;
                } طالما(++visual_شروعات<visual_حد_بندي);
            } وإلا {
                REMOVE_ODD_ٻٽو_انگ(logical_شروعات);
                logical_شروعات+=visual_حد_بندي-visual_شروعات;  /* logicalLimit */
                افعل { /* RTL */
                    *pi++ = --logical_شروعات;
                } طالما(++visual_شروعات<visual_حد_بندي);
            }
            /* visualStart==visualLimit; */
        }

        إذا(pBiDi->وجهو_Points.size>0) {
            int32_t mark_مليو=0, لاڳيتو_ڳڻپ=pBiDi->لاڳيتو_ڳڻپ;
            int32_t وجهو_Remove, i, j, k;
            runs=pBiDi->runs;
            /* count all inserted marks */
            لكل(i=0; i<لاڳيتو_ڳڻپ; i++) {
                وجهو_Remove=runs[i].وجهو_Remove;
                إذا(وجهو_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_مليو++;
                }
                إذا(وجهو_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_مليو++;
                }
            }
            /* move back indexes by number of preceding marks */
            k=pBiDi->نتيجو_ڊيگهائي;
            لكل(i=لاڳيتو_ڳڻپ-1; i>=0 && mark_مليو>0; i--) {
                وجهو_Remove=runs[i].وجهو_Remove;
                إذا(وجهو_Remove&(LRM_AFTER|RLM_AFTER)) {
                    اشاريو_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_مليو--;
                }
                visual_شروعات= i>0 ? runs[i-1].visual_حد_بندي : 0;
                لكل(j=runs[i].visual_حد_بندي-1; j>=visual_شروعات && mark_مليو>0; j--) {
                    اشاريو_Map[--k]=اشاريو_Map[j];
                }
                إذا(وجهو_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    اشاريو_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_مليو--;
                }
            }
        }
        وإلا إذا(pBiDi->control_ڳڻپ>0) {
            int32_t لاڳيتو_ڳڻپ=pBiDi->لاڳيتو_ڳڻپ, logical_پڇاڙي;
            int32_t وجهو_Remove, length, i, j, k, m;
            char16_t uchar;
            UBool even_لاڳيتو;
            runs=pBiDi->runs;
            visual_شروعات=0;
            /* move forward indexes by number of preceding controls */
            k=0;
            لكل(i=0; i<لاڳيتو_ڳڻپ; i++, visual_شروعات+=length) {
                length=runs[i].visual_حد_بندي-visual_شروعات;
                وجهو_Remove=runs[i].وجهو_Remove;
                /* if no control found yet, nothing to do in this run */
                إذا((وجهو_Remove==0)&&(k==visual_شروعات)) {
                    k+=length;
                    استمر;
                }
                /* if no control in this run */
                إذا(وجهو_Remove==0) {
                    visual_حد_بندي=runs[i].visual_حد_بندي;
                    لكل(j=visual_شروعات; j<visual_حد_بندي; j++) {
                        اشاريو_Map[k++]=اشاريو_Map[j];
                    }
                    استمر;
                }
                logical_شروعات=runs[i].logical_شروعات;
                even_لاڳيتو=IS_EVEN_لاڳيتو(logical_شروعات);
                REMOVE_ODD_ٻٽو_انگ(logical_شروعات);
                logical_پڇاڙي=logical_شروعات+length-1;
                لكل(j=0; j<length; j++) {
                    m= even_لاڳيتو ? logical_شروعات+j : logical_پڇاڙي-j;
                    uchar=pBiDi->متن[m];
                    إذا(!IS_BIDI_CONTROL_CHAR(uchar)) {
                        اشاريو_Map[k++]=m;
                    }
                }
            }
        }
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_invertMap(ثابت int32_t *srcMap, int32_t *destMap, int32_t length) {
    إذا(srcMap!=خالي_اشارو && destMap!=خالي_اشارو && length>0) {
        ثابت int32_t *pi;
        int32_t dest_ڊيگهائي=-1, count=0;
        /* find highest value and count positive indexes in srcMap */
        pi=srcMap+length;
        طالما(pi>srcMap) {
            إذا(*--pi>dest_ڊيگهائي) {
                dest_ڊيگهائي=*pi;
            }
            إذا(*pi>=0) {
                count++;
            }
        }
        dest_ڊيگهائي++;           /* add 1 for origin 0 */
        إذا(count<dest_ڊيگهائي) {
            /* we must fill unmatched destMap entries with -1 */
            uprv_memset(destMap, 0xFF, dest_ڊيگهائي*حجم(int32_t));
        }
        pi=srcMap+length;
        طالما(length>0) {
            إذا(*--pi>=0) {
                destMap[*pi]=--length;
            } وإلا {
                --length;
            }
        }
    }
}
