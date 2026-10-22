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
سیٹ_Trailing_WSStart(UBiDi *pBiDi) {
    /* pBiDi->direction!=UBIDI_MIXED */

    ثابت DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *پدھراں=pBiDi->پدھراں;
    int32_t شروع=pBiDi->length;
    UBiDiLevel paraLevel=pBiDi->paraLevel;

    /* If the line is terminated by a block separator, all preceding WS etc...
       are already set to paragraph level.
       Setting trailingWSStart to pBidi->length will avoid changing the
       level of B chars from 0 to paraLevel in ubidi_getLevels when
       orderParagraphsLTR==true.
     */
    إذا(dirProps[شروع-1]==B) {
        pBiDi->trailingWSStart=شروع;   /* currently == pBiDi->length */
        أرجع;
    }
    /* go backwards across all WS, BN, explicit codes */
    طالما(شروع>0 && DIRPROP_FLAG(dirProps[شروع-1])&نقاب_WS) {
        --شروع;
    }

    /* if the WS run can be merged with the previous run then do so here */
    طالما(شروع>0 && پدھراں[شروع-1]==paraLevel) {
        --شروع;
    }

    pBiDi->trailingWSStart=شروع;
}

/* ubidi_setLine ------------------------------------------------------------ */

U_CAPI فراغ U_EXPORT2
ubidi_سیٹ_سطر(ثابت UBiDi *pParaBiDi,
              int32_t شروع, int32_t حد_بندی,
              UBiDi *p_سطر_Bi_Di,
              UError_کوڈ *p_Error_کوڈ) {
    int32_t length;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوڈ);
    RETURN_VOID_IF_NOT_VALID_PARA(pParaBiDi, *p_Error_کوڈ);
    RETURN_VOID_IF_BAD_RANGE(شروع, 0, حد_بندی, *p_Error_کوڈ);
    RETURN_VOID_IF_BAD_RANGE(حد_بندی, 0, pParaBiDi->length+1, *p_Error_کوڈ);
    إذا(p_سطر_Bi_Di==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا(ubidi_getParagraph(pParaBiDi, شروع, خالی_سوچک, خالی_سوچک, خالی_سوچک, p_Error_کوڈ) !=
       ubidi_getParagraph(pParaBiDi, حد_بندی-1, خالی_سوچک, خالی_سوچک, خالی_سوچک, p_Error_کوڈ)) {
        /* the line crosses a paragraph boundary */
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    /* set the values in pLineBiDi from its pParaBiDi parent */
    p_سطر_Bi_Di->pParaBiDi=خالی_سوچک;          /* mark unfinished setLine */
    p_سطر_Bi_Di->متن=pParaBiDi->متن+شروع;
    length=p_سطر_Bi_Di->length=حد_بندی-شروع;
    p_سطر_Bi_Di->نتیجہ_لمائی=p_سطر_Bi_Di->original_لمائی=length;
    p_سطر_Bi_Di->paraLevel=GET_PARALEVEL(pParaBiDi, شروع);
    p_سطر_Bi_Di->para_گنتی=pParaBiDi->para_گنتی;
    p_سطر_Bi_Di->runs=خالی_سوچک;
    p_سطر_Bi_Di->flags=0;
    p_سطر_Bi_Di->reordering_منوال=pParaBiDi->reordering_منوال;
    p_سطر_Bi_Di->reorderingOptions=pParaBiDi->reorderingOptions;
    p_سطر_Bi_Di->control_گنتی=0;
    إذا(pParaBiDi->control_گنتی>0) {
        int32_t j;
        لكل(j=شروع; j<حد_بندی; j++) {
            إذا(IS_BIDI_CONTROL_CHAR(pParaBiDi->متن[j])) {
                p_سطر_Bi_Di->control_گنتی++;
            }
        }
        p_سطر_Bi_Di->نتیجہ_لمائی-=p_سطر_Bi_Di->control_گنتی;
    }

    p_سطر_Bi_Di->dirProps=pParaBiDi->dirProps+شروع;
    p_سطر_Bi_Di->پدھراں=pParaBiDi->پدھراں+شروع;
    p_سطر_Bi_Di->لگاتار_گنتی=-1;

    إذا(pParaBiDi->direction!=UBIDI_MIXED) {
        /* the parent is already trivial */
        p_سطر_Bi_Di->direction=pParaBiDi->direction;

        /*
         * The parent's levels are all either
         * implicitly or explicitly ==paraLevel;
         * do the same here.
         */
        إذا(pParaBiDi->trailingWSStart<=شروع) {
            p_سطر_Bi_Di->trailingWSStart=0;
        } وإلا إذا(pParaBiDi->trailingWSStart<حد_بندی) {
            p_سطر_Bi_Di->trailingWSStart=pParaBiDi->trailingWSStart-شروع;
        } وإلا {
            p_سطر_Bi_Di->trailingWSStart=length;
        }
    } وإلا {
        ثابت UBiDiLevel *پدھراں=p_سطر_Bi_Di->پدھراں;
        int32_t i, trailingWSStart;
        UBiDiLevel level;

        سیٹ_Trailing_WSStart(p_سطر_Bi_Di);
        trailingWSStart=p_سطر_Bi_Di->trailingWSStart;

        /* recalculate pLineBiDi->direction */
        إذا(trailingWSStart==0) {
            /* all levels are at paraLevel */
            p_سطر_Bi_Di->direction=(UBiDiDirection)(p_سطر_Bi_Di->paraLevel&1);
        } وإلا {
            /* get the level of the first character */
            level=(UBiDiLevel)(پدھراں[0]&1);

            /* if there is anything of a different level, then the line is mixed */
            إذا(trailingWSStart<length && (p_سطر_Bi_Di->paraLevel&1)!=level) {
                /* the trailing WS is at paraLevel, which differs from levels[0] */
                p_سطر_Bi_Di->direction=UBIDI_MIXED;
            } وإلا {
                /* see if levels[1..trailingWSStart-1] have the same direction as levels[0] and paraLevel */
                i=1;
                لكل(;;) {
                    إذا(i==trailingWSStart) {
                        /* the direction values match those in level */
                        p_سطر_Bi_Di->direction=(UBiDiDirection)level;
                        اكسر;
                    } وإلا إذا((پدھراں[i]&1)!=level) {
                        p_سطر_Bi_Di->direction=UBIDI_MIXED;
                        اكسر;
                    }
                    ++i;
                }
            }
        }

        بدل(p_سطر_Bi_Di->direction) {
        حالة UBIDI_LTR:
            /* make sure paraLevel is even */
            p_سطر_Bi_Di->paraLevel=(UBiDiLevel)((p_سطر_Bi_Di->paraLevel+1)&~1);

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_سطر_Bi_Di->trailingWSStart=0;
            اكسر;
        حالة UBIDI_RTL:
            /* make sure paraLevel is odd */
            p_سطر_Bi_Di->paraLevel|=1;

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_سطر_Bi_Di->trailingWSStart=0;
            اكسر;
        افتراضي:
            اكسر;
        }
    }
    p_سطر_Bi_Di->pParaBiDi=pParaBiDi;     /* mark successful setLine */
}

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getLevelAt(ثابت UBiDi *pBiDi, int32_t char_اشاریہ) {
    /* return paraLevel if in the trailing WS run, otherwise the real level */
    إذا(!IS_VALID_PARA_OR_سطر(pBiDi) || char_اشاریہ<0 || pBiDi->length<=char_اشاریہ) {
        أرجع 0;
    } وإلا إذا(pBiDi->direction!=UBIDI_MIXED || char_اشاریہ>=pBiDi->trailingWSStart) {
        أرجع GET_PARALEVEL(pBiDi, char_اشاریہ);
    } وإلا {
        أرجع pBiDi->پدھراں[char_اشاریہ];
    }
}

U_CAPI ثابت UBiDiLevel* U_EXPORT2
ubidi_get_پدھراں(UBiDi* pBiDi, UError_کوڈ* p_Error_کوڈ) {
    int32_t شروع, length;

    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوڈ, خالی_سوچک);
    RETURN_IF_NOT_VALID_PARA_OR_سطر(pBiDi, *p_Error_کوڈ, خالی_سوچک);
    إذا((length=pBiDi->length)<=0) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    إذا((شروع=pBiDi->trailingWSStart)==length) {
        /* the current levels array reflects the WS run */
        أرجع pBiDi->پدھراں;
    }

    /*
     * After the previous if(), we know that the levels array
     * has an implicit trailing WS run and therefore does not fully
     * reflect itself all the levels.
     * This must be a UBiDi object for a line, and
     * we need to create a new levels array.
     */
    إذا(get_پدھراں_حافظہ(pBiDi, length)) {
        UBiDiLevel *پدھراں=pBiDi->پدھراں_حافظہ;

        إذا(شروع>0 && پدھراں!=pBiDi->پدھراں) {
            uprv_memcpy(پدھراں, pBiDi->پدھراں, شروع);
        }
        /* pBiDi->paraLevel is ok even if contextual multiple paragraphs,
           since pBidi is a line object                                     */
        uprv_memset(پدھراں+شروع, pBiDi->paraLevel, length-شروع);

        /* this new levels array is set for the line and reflects the WS run */
        pBiDi->trailingWSStart=length;
        أرجع pBiDi->پدھراں=پدھراں;
    } وإلا {
        /* out of memory */
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_get_Logical_لگاتار(ثابت UBiDi *pBiDi, int32_t logicalPosition,
                    int32_t *p_Logical_حد_بندی, UBiDiLevel *pLevel) {
    UError_کوڈ error_کوڈ;
    int32_t لگاتار_گنتی, visual_شروع, logical_حد_بندی, logical_پہلا, i;
    لگاتار i_لگاتار;

    error_کوڈ=U_ZERO_ERROR;
    RETURN_VOID_IF_BAD_RANGE(logicalPosition, 0, pBiDi->length, error_کوڈ);
    /* ubidi_countRuns will check VALID_PARA_OR_LINE */
    لگاتار_گنتی=ubidi_گنتی_Runs((UBiDi *)pBiDi, &error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    /* this is done based on runs rather than on levels since levels have
       a special interpretation when UBIDI_REORDER_RUNS_ONLY
     */
    visual_شروع=logical_حد_بندی=0;
    i_لگاتار=pBiDi->runs[0];

    لكل(i=0; i<لگاتار_گنتی; i++) {
        i_لگاتار = pBiDi->runs[i];
        logical_پہلا=GET_اشاریہ(i_لگاتار.logical_شروع);
        logical_حد_بندی=logical_پہلا+i_لگاتار.visual_حد_بندی-visual_شروع;
        إذا((logicalPosition>=logical_پہلا) &&
           (logicalPosition<logical_حد_بندی)) {
            اكسر;
        }
        visual_شروع = i_لگاتار.visual_حد_بندی;
    }
    إذا(p_Logical_حد_بندی) {
        *p_Logical_حد_بندی=logical_حد_بندی;
    }
    إذا(pLevel) {
        إذا(pBiDi->reordering_منوال==UBIDI_REORDER_RUNS_ONLY) {
            *pLevel=(UBiDiLevel)GET_ODD_ثنائی_ہندسہ(i_لگاتار.logical_شروع);
        }
        وإلا إذا(pBiDi->direction!=UBIDI_MIXED || logicalPosition>=pBiDi->trailingWSStart) {
            *pLevel=GET_PARALEVEL(pBiDi, logicalPosition);
        } وإلا {
        *pLevel=pBiDi->پدھراں[logicalPosition];
        }
    }
}

/* runs API functions ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_گنتی_Runs(UBiDi *pBiDi, UError_کوڈ *p_Error_کوڈ) {
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوڈ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_سطر(pBiDi, *p_Error_کوڈ, -1);
    ubidi_getRuns(pBiDi, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع -1;
    }
    أرجع pBiDi->لگاتار_گنتی;
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_Visual_لگاتار(UBiDi *pBiDi, int32_t لگاتار_اشاریہ,
                   int32_t *p_Logical_شروع, int32_t *p_لمائی)
{
    int32_t شروع;
    UError_کوڈ error_کوڈ = U_ZERO_ERROR;
    RETURN_IF_NOT_VALID_PARA_OR_سطر(pBiDi, error_کوڈ, UBIDI_LTR);
    ubidi_getRuns(pBiDi, &error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع UBIDI_LTR;
    }
    RETURN_IF_BAD_RANGE(لگاتار_اشاریہ, 0, pBiDi->لگاتار_گنتی, error_کوڈ, UBIDI_LTR);

    شروع=pBiDi->runs[لگاتار_اشاریہ].logical_شروع;
    إذا(p_Logical_شروع!=خالی_سوچک) {
        *p_Logical_شروع=GET_اشاریہ(شروع);
    }
    إذا(p_لمائی!=خالی_سوچک) {
        إذا(لگاتار_اشاریہ>0) {
            *p_لمائی=pBiDi->runs[لگاتار_اشاریہ].visual_حد_بندی-
                     pBiDi->runs[لگاتار_اشاریہ-1].visual_حد_بندی;
        } وإلا {
            *p_لمائی=pBiDi->runs[0].visual_حد_بندی;
        }
    }
    أرجع (UBiDiDirection)GET_ODD_ثنائی_ہندسہ(شروع);
}

/* in trivial cases there is only one trivial run; called by ubidi_getRuns() */
ساكن فراغ
get_Single_لگاتار(UBiDi *pBiDi, UBiDiLevel level) {
    /* simple, single-run case */
    pBiDi->runs=pBiDi->simpleRuns;
    pBiDi->لگاتار_گنتی=1;

    /* fill and reorder the single run */
    pBiDi->runs[0].logical_شروع=MAKE_اشاریہ_ODD_PAIR(0, level);
    pBiDi->runs[0].visual_حد_بندی=pBiDi->length;
    pBiDi->runs[0].داخل_کرو_Remove=0;
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
reorder_سطر(UBiDi *pBiDi, UBiDiLevel minLevel, UBiDiLevel maxLevel) {
    لگاتار *runs, عارضی_لگاتار;
    UBiDiLevel *پدھراں;
    int32_t پہلا_لگاتار, سرا_لگاتار, حد_بندی_لگاتار, لگاتار_گنتی;

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
    پدھراں=pBiDi->پدھراں;
    لگاتار_گنتی=pBiDi->لگاتار_گنتی;

    /* do not include the WS run at paraLevel<=old minLevel except in the simple loop */
    إذا(pBiDi->trailingWSStart<pBiDi->length) {
        --لگاتار_گنتی;
    }

    طالما(--maxLevel>=minLevel) {
        پہلا_لگاتار=0;

        /* loop for all sequences of runs */
        لكل(;;) {
            /* look for a sequence of runs that are all at >=maxLevel */
            /* look for the first run of such a sequence */
            طالما(پہلا_لگاتار<لگاتار_گنتی && پدھراں[runs[پہلا_لگاتار].logical_شروع]<maxLevel) {
                ++پہلا_لگاتار;
            }
            إذا(پہلا_لگاتار>=لگاتار_گنتی) {
                اكسر;  /* no more such runs */
            }

            /* look for the limit run of such a sequence (the run behind it) */
            لكل(حد_بندی_لگاتار=پہلا_لگاتار; ++حد_بندی_لگاتار<لگاتار_گنتی && پدھراں[runs[حد_بندی_لگاتار].logical_شروع]>=maxLevel;) {}

            /* Swap the entire sequence of runs from firstRun to limitRun-1. */
            سرا_لگاتار=حد_بندی_لگاتار-1;
            طالما(پہلا_لگاتار<سرا_لگاتار) {
                عارضی_لگاتار = runs[پہلا_لگاتار];
                runs[پہلا_لگاتار]=runs[سرا_لگاتار];
                runs[سرا_لگاتار]=عارضی_لگاتار;
                ++پہلا_لگاتار;
                --سرا_لگاتار;
            }

            إذا(حد_بندی_لگاتار==لگاتار_گنتی) {
                اكسر;  /* no more such runs */
            } وإلا {
                پہلا_لگاتار=حد_بندی_لگاتار+1;
            }
        }
    }

    /* now do maxLevel==old minLevel (==odd!), see above */
    إذا(!(minLevel&1)) {
        پہلا_لگاتار=0;

        /* include the trailing WS run in this complete reordering */
        إذا(pBiDi->trailingWSStart==pBiDi->length) {
            --لگاتار_گنتی;
        }

        /* Swap the entire sequence of all runs. (endRun==runCount) */
        طالما(پہلا_لگاتار<لگاتار_گنتی) {
            عارضی_لگاتار=runs[پہلا_لگاتار];
            runs[پہلا_لگاتار]=runs[لگاتار_گنتی];
            runs[لگاتار_گنتی]=عارضی_لگاتار;
            ++پہلا_لگاتار;
            --لگاتار_گنتی;
        }
    }
}

/* compute the runs array --------------------------------------------------- */

ساكن int32_t get_لگاتار_From_Logical_اشاریہ(UBiDi *pBiDi, int32_t logical_اشاریہ) {
    لگاتار *runs=pBiDi->runs;
    int32_t لگاتار_گنتی=pBiDi->لگاتار_گنتی, visual_شروع=0, i, length, logical_شروع;

    لكل(i=0; i<لگاتار_گنتی; i++) {
        length=runs[i].visual_حد_بندی-visual_شروع;
        logical_شروع=GET_اشاریہ(runs[i].logical_شروع);
        إذا((logical_اشاریہ>=logical_شروع) && (logical_اشاریہ<(logical_شروع+length))) {
            أرجع i;
        }
        visual_شروع+=length;
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
ubidi_getRuns(UBiDi *pBiDi, UError_کوڈ* p_Error_کوڈ) {
    /*
     * This method returns immediately if the runs are already set. This
     * includes the case of length==0 (handled in setPara)..
     */
    إذا (pBiDi->لگاتار_گنتی>=0) {
        أرجع سچ;
    }

    إذا(pBiDi->direction!=UBIDI_MIXED) {
        /* simple, single-run case - this covers length==0 */
        /* pBiDi->paraLevel is ok even for contextual multiple paragraphs */
        get_Single_لگاتار(pBiDi, pBiDi->paraLevel);
    } وإلا /* UBIDI_MIXED, length>0 */ {
        /* mixed directionality */
        int32_t length=pBiDi->length, حد_بندی;
        UBiDiLevel *پدھراں=pBiDi->پدھراں;
        int32_t i, لگاتار_گنتی;
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
        حد_بندی=pBiDi->trailingWSStart;
        /* count the runs, there is at least one non-WS run, and limit>0 */
        لگاتار_گنتی=0;
        لكل(i=0; i<حد_بندی; ++i) {
            /* increment runCount at the start of each run */
            إذا(پدھراں[i]!=level) {
                ++لگاتار_گنتی;
                level=پدھراں[i];
            }
        }

        /*
         * We don't need to see if the last run can be merged with a trailing
         * WS run because setTrailingWSStart() would have done that.
         */
        إذا(لگاتار_گنتی==1 && حد_بندی==length) {
            /* There is only one non-WS run and no trailing WS-run. */
            get_Single_لگاتار(pBiDi, پدھراں[0]);
        } وإلا /* runCount>1 || limit<length */ {
            /* allocate and set the runs */
            لگاتار *runs;
            int32_t لگاتار_اشاریہ, شروع;
            UBiDiLevel minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1, maxLevel=0;

            /* now, count a (non-mergeable) WS run */
            إذا(حد_بندی<length) {
                ++لگاتار_گنتی;
            }

            /* runCount>1 */
            إذا(get_Runs_حافظہ(pBiDi, لگاتار_گنتی)) {
                runs=pBiDi->runs_حافظہ;
            } وإلا {
                *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
                أرجع خطأ;
            }

            /* set the runs */
            /* FOOD FOR THOUGHT: this could be optimized, e.g.:
             * 464->444, 484->444, 575->555, 595->555
             * However, that would take longer. Check also how it would
             * interact with BiDi control removal and inserting Marks.
             */
            لگاتار_اشاریہ=0;

            /* search for the run limits and initialize visualLimit values with the run lengths */
            i=0;
            افعل {
                /* prepare this run */
                شروع=i;
                level=پدھراں[i];
                إذا(level<minLevel) {
                    minLevel=level;
                }
                إذا(level>maxLevel) {
                    maxLevel=level;
                }

                /* look for the run limit */
                طالما(++i<حد_بندی && پدھراں[i]==level) {}

                /* i is another run limit */
                runs[لگاتار_اشاریہ].logical_شروع=شروع;
                runs[لگاتار_اشاریہ].visual_حد_بندی=i-شروع;
                runs[لگاتار_اشاریہ].داخل_کرو_Remove=0;
                ++لگاتار_اشاریہ;
            } طالما(i<حد_بندی);

            إذا(حد_بندی<length) {
                /* there is a separate WS run */
                runs[لگاتار_اشاریہ].logical_شروع=حد_بندی;
                runs[لگاتار_اشاریہ].visual_حد_بندی=length-حد_بندی;
                /* For the trailing WS run, pBiDi->paraLevel is ok even
                   if contextual multiple paragraphs.                   */
                إذا(pBiDi->paraLevel<minLevel) {
                    minLevel=pBiDi->paraLevel;
                }
            }

            /* set the object fields */
            pBiDi->runs=runs;
            pBiDi->لگاتار_گنتی=لگاتار_گنتی;

            reorder_سطر(pBiDi, minLevel, maxLevel);

            /* now add the direction flags and adjust the visualLimit's to be just that */
            /* this loop will also handle the trailing WS run */
            حد_بندی=0;
            لكل(i=0; i<لگاتار_گنتی; ++i) {
                جوڑو_ODD_ثنائی_ہندسہ_FROM_LEVEL(runs[i].logical_شروع, پدھراں[runs[i].logical_شروع]);
                حد_بندی+=runs[i].visual_حد_بندی;
                runs[i].visual_حد_بندی=حد_بندی;
            }

            /* Set the "odd" bit for the trailing WS run. */
            /* For a RTL paragraph, it will be the *first* run in visual order. */
            /* For the trailing WS run, pBiDi->paraLevel is ok even if
               contextual multiple paragraphs.                          */
            إذا(لگاتار_اشاریہ<لگاتار_گنتی) {
                int32_t trailing_لگاتار = ((pBiDi->paraLevel & 1) != 0)? 0 : لگاتار_اشاریہ;

                جوڑو_ODD_ثنائی_ہندسہ_FROM_LEVEL(runs[trailing_لگاتار].logical_شروع, pBiDi->paraLevel);
            }
        }
    }

    /* handle insert LRM/RLM BEFORE/AFTER run */
    إذا(pBiDi->داخل_کرو_Points.size>0) {
        نقطہ *نقطہ_2, *شروع=pBiDi->داخل_کرو_Points.points,
                      *حد_بندی=شروع+pBiDi->داخل_کرو_Points.size;
        int32_t لگاتار_اشاریہ;
        لكل(نقطہ_2=شروع; نقطہ_2<حد_بندی; نقطہ_2++) {
            لگاتار_اشاریہ=get_لگاتار_From_Logical_اشاریہ(pBiDi, نقطہ_2->pos);
            pBiDi->runs[لگاتار_اشاریہ].داخل_کرو_Remove|=نقطہ_2->flag;
        }
    }

    /* handle remove BiDi control characters */
    إذا(pBiDi->control_گنتی>0) {
        int32_t لگاتار_اشاریہ;
        ثابت char16_t *شروع=pBiDi->متن, *حد_بندی=شروع+pBiDi->length, *pu;
        لكل(pu=شروع; pu<حد_بندی; pu++) {
            إذا(IS_BIDI_CONTROL_CHAR(*pu)) {
                لگاتار_اشاریہ=get_لگاتار_From_Logical_اشاریہ(pBiDi, (int32_t)(pu-شروع));
                pBiDi->runs[لگاتار_اشاریہ].داخل_کرو_Remove--;
            }
        }
    }

    أرجع سچ;
}

ساكن UBool
prepareReorder(ثابت UBiDiLevel *پدھراں, int32_t length,
               int32_t *اشاریہ_Map,
               UBiDiLevel *pMinLevel, UBiDiLevel *pMaxLevel) {
    int32_t شروع;
    UBiDiLevel level, minLevel, maxLevel;

    إذا(پدھراں==خالی_سوچک || length<=0) {
        أرجع خطأ;
    }

    /* determine minLevel and maxLevel */
    minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1;
    maxLevel=0;
    لكل(شروع=length; شروع>0;) {
        level=پدھراں[--شروع];
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
    لكل(شروع=length; شروع>0;) {
        --شروع;
        اشاریہ_Map[شروع]=شروع;
    }

    أرجع سچ;
}

/* reorder a line based on a levels array (L2) ------------------------------ */

U_CAPI فراغ U_EXPORT2
ubidi_reorderLogical(ثابت UBiDiLevel *پدھراں, int32_t length, int32_t *اشاریہ_Map) {
    int32_t شروع, حد_بندی, جوڑ_دا_Sos_Eos;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    إذا(اشاریہ_Map==خالی_سوچک || !prepareReorder(پدھراں, length, اشاریہ_Map, &minLevel, &maxLevel)) {
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
        شروع=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        لكل(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            طالما(شروع<length && پدھراں[شروع]<maxLevel) {
                ++شروع;
            }
            إذا(شروع>=length) {
                اكسر;  /* no more such sequences */
            }

            /* look for the limit of such a sequence (the index behind it) */
            لكل(حد_بندی=شروع; ++حد_بندی<length && پدھراں[حد_بندی]>=maxLevel;) {}

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
            جوڑ_دا_Sos_Eos=شروع+حد_بندی-1;

            /* reorder each index in the sequence */
            افعل {
                اشاریہ_Map[شروع]=جوڑ_دا_Sos_Eos-اشاریہ_Map[شروع];
            } طالما(++شروع<حد_بندی);

            /* start==limit */
            إذا(حد_بندی==length) {
                اكسر;  /* no more such sequences */
            } وإلا {
                شروع=حد_بندی+1;
            }
        }
    } طالما(--maxLevel>=minLevel);
}

U_CAPI فراغ U_EXPORT2
ubidi_reorderVisual(ثابت UBiDiLevel *پدھراں, int32_t length, int32_t *اشاریہ_Map) {
    int32_t شروع, سرا, حد_بندی, عارضی;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    إذا(اشاریہ_Map==خالی_سوچک || !prepareReorder(پدھراں, length, اشاریہ_Map, &minLevel, &maxLevel)) {
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
        شروع=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        لكل(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            طالما(شروع<length && پدھراں[شروع]<maxLevel) {
                ++شروع;
            }
            إذا(شروع>=length) {
                اكسر;  /* no more such runs */
            }

            /* look for the limit of such a sequence (the index behind it) */
            لكل(حد_بندی=شروع; ++حد_بندی<length && پدھراں[حد_بندی]>=maxLevel;) {}

            /*
             * Swap the entire interval of indexes from start to limit-1.
             * We don't need to swap the levels for the purpose of this
             * algorithm: the sequence of levels that we look at does not
             * move anyway.
             */
            سرا=حد_بندی-1;
            طالما(شروع<سرا) {
                عارضی=اشاریہ_Map[شروع];
                اشاریہ_Map[شروع]=اشاریہ_Map[سرا];
                اشاریہ_Map[سرا]=عارضی;

                ++شروع;
                --سرا;
            }

            إذا(حد_بندی==length) {
                اكسر;  /* no more such sequences */
            } وإلا {
                شروع=حد_بندی+1;
            }
        }
    } طالما(--maxLevel>=minLevel);
}

/* API functions for logical<->visual mapping ------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_get_Visual_اشاریہ(UBiDi *pBiDi, int32_t logical_اشاریہ, UError_کوڈ *p_Error_کوڈ) {
    int32_t visual_اشاریہ=UBIDI_MAP_NOWHERE;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوڈ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_سطر(pBiDi, *p_Error_کوڈ, -1);
    RETURN_IF_BAD_RANGE(logical_اشاریہ, 0, pBiDi->length, *p_Error_کوڈ, -1);

    /* we can do the trivial cases without the runs array */
    بدل(pBiDi->direction) {
    حالة UBIDI_LTR:
        visual_اشاریہ=logical_اشاریہ;
        اكسر;
    حالة UBIDI_RTL:
        visual_اشاریہ=pBiDi->length-logical_اشاریہ-1;
        اكسر;
    افتراضي:
        إذا(!ubidi_getRuns(pBiDi, p_Error_کوڈ)) {
            *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع -1;
        } وإلا {
            لگاتار *runs=pBiDi->runs;
            int32_t i, visual_شروع=0, وتھ, length;

            /* linear search for the run, search on the visual runs */
            لكل(i=0; i<pBiDi->لگاتار_گنتی; ++i) {
                length=runs[i].visual_حد_بندی-visual_شروع;
                وتھ=logical_اشاریہ-GET_اشاریہ(runs[i].logical_شروع);
                إذا(وتھ>=0 && وتھ<length) {
                    إذا(IS_EVEN_لگاتار(runs[i].logical_شروع)) {
                        /* LTR */
                        visual_اشاریہ=visual_شروع+وتھ;
                    } وإلا {
                        /* RTL */
                        visual_اشاریہ=visual_شروع+length-وتھ-1;
                    }
                    اكسر;          /* exit for loop */
                }
                visual_شروع+=length;
            }
            إذا(i>=pBiDi->لگاتار_گنتی) {
                أرجع UBIDI_MAP_NOWHERE;
            }
        }
    }

    إذا(pBiDi->داخل_کرو_Points.size>0) {
        /* add the number of added marks until the calculated visual index */
        لگاتار *runs=pBiDi->runs;
        int32_t i, length, داخل_کرو_Remove;
        int32_t visual_شروع=0, mark_لبھیا=0;
        لكل(i=0; ; i++, visual_شروع+=length) {
            length=runs[i].visual_حد_بندی-visual_شروع;
            داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
            إذا(داخل_کرو_Remove & (LRM_BEFORE|RLM_BEFORE)) {
                mark_لبھیا++;
            }
            /* is it the run containing the visual index? */
            إذا(visual_اشاریہ<runs[i].visual_حد_بندی) {
                أرجع visual_اشاریہ+mark_لبھیا;
            }
            إذا(داخل_کرو_Remove & (LRM_AFTER|RLM_AFTER)) {
                mark_لبھیا++;
            }
        }
    }
    وإلا إذا(pBiDi->control_گنتی>0) {
        /* subtract the number of controls until the calculated visual index */
        لگاتار *runs=pBiDi->runs;
        int32_t i, j, شروع, حد_بندی, length, داخل_کرو_Remove;
        int32_t visual_شروع=0, control_لبھیا=0;
        char16_t uchar=pBiDi->متن[logical_اشاریہ];
        /* is the logical index pointing to a control ? */
        إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
            أرجع UBIDI_MAP_NOWHERE;
        }
        /* loop on runs */
        لكل(i=0; ; i++, visual_شروع+=length) {
            length=runs[i].visual_حد_بندی-visual_شروع;
            داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
            /* calculated visual index is beyond this run? */
            إذا(visual_اشاریہ>=runs[i].visual_حد_بندی) {
                control_لبھیا-=داخل_کرو_Remove;
                استمر;
            }
            /* calculated visual index must be within current run */
            إذا(داخل_کرو_Remove==0) {
                أرجع visual_اشاریہ-control_لبھیا;
            }
            إذا(IS_EVEN_لگاتار(runs[i].logical_شروع)) {
                /* LTR: check from run start to logical index */
                شروع=runs[i].logical_شروع;
                حد_بندی=logical_اشاریہ;
            } وإلا {
                /* RTL: check from logical index to run end */
                شروع=logical_اشاریہ+1;
                حد_بندی=GET_اشاریہ(runs[i].logical_شروع)+length;
            }
            لكل(j=شروع; j<حد_بندی; j++) {
                uchar=pBiDi->متن[j];
                إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_لبھیا++;
                }
            }
            أرجع visual_اشاریہ-control_لبھیا;
        }
    }

    أرجع visual_اشاریہ;
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Logical_اشاریہ(UBiDi *pBiDi, int32_t visual_اشاریہ, UError_کوڈ *p_Error_کوڈ) {
    لگاتار *runs;
    int32_t i, لگاتار_گنتی, شروع;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوڈ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_سطر(pBiDi, *p_Error_کوڈ, -1);
    RETURN_IF_BAD_RANGE(visual_اشاریہ, 0, pBiDi->نتیجہ_لمائی, *p_Error_کوڈ, -1);
    /* we can do the trivial cases without the runs array */
    إذا(pBiDi->داخل_کرو_Points.size==0 && pBiDi->control_گنتی==0) {
        إذا(pBiDi->direction==UBIDI_LTR) {
            أرجع visual_اشاریہ;
        }
        وإلا إذا(pBiDi->direction==UBIDI_RTL) {
            أرجع pBiDi->length-visual_اشاریہ-1;
        }
    }
    إذا(!ubidi_getRuns(pBiDi, p_Error_کوڈ)) {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع -1;
    }

    runs=pBiDi->runs;
    لگاتار_گنتی=pBiDi->لگاتار_گنتی;
    إذا(pBiDi->داخل_کرو_Points.size>0) {
        /* handle inserted LRM/RLM */
        int32_t mark_لبھیا=0, داخل_کرو_Remove;
        int32_t visual_شروع=0, length;
        runs=pBiDi->runs;
        /* subtract number of marks until visual index */
        لكل(i=0; ; i++, visual_شروع+=length) {
            length=runs[i].visual_حد_بندی-visual_شروع;
            داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
            إذا(داخل_کرو_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                إذا(visual_اشاریہ<=(visual_شروع+mark_لبھیا)) {
                    أرجع UBIDI_MAP_NOWHERE;
                }
                mark_لبھیا++;
            }
            /* is adjusted visual index within this run? */
            إذا(visual_اشاریہ<(runs[i].visual_حد_بندی+mark_لبھیا)) {
                visual_اشاریہ-=mark_لبھیا;
                اكسر;
            }
            إذا(داخل_کرو_Remove&(LRM_AFTER|RLM_AFTER)) {
                إذا(visual_اشاریہ==(visual_شروع+length+mark_لبھیا)) {
                    أرجع UBIDI_MAP_NOWHERE;
                }
                mark_لبھیا++;
            }
        }
    }
    وإلا إذا(pBiDi->control_گنتی>0) {
        /* handle removed BiDi control characters */
        int32_t control_لبھیا=0, داخل_کرو_Remove, length;
        int32_t logical_شروع, logical_سرا, visual_شروع=0, j, k;
        char16_t uchar;
        UBool even_لگاتار;
        /* add number of controls until visual index */
        لكل(i=0; ; i++, visual_شروع+=length) {
            length=runs[i].visual_حد_بندی-visual_شروع;
            داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
            /* is adjusted visual index beyond current run? */
            إذا(visual_اشاریہ>=(runs[i].visual_حد_بندی-control_لبھیا+داخل_کرو_Remove)) {
                control_لبھیا-=داخل_کرو_Remove;
                استمر;
            }
            /* adjusted visual index is within current run */
            إذا(داخل_کرو_Remove==0) {
                visual_اشاریہ+=control_لبھیا;
                اكسر;
            }
            /* count non-control chars until visualIndex */
            logical_شروع=runs[i].logical_شروع;
            even_لگاتار=IS_EVEN_لگاتار(logical_شروع);
            REMOVE_ODD_ثنائی_ہندسہ(logical_شروع);
            logical_سرا=logical_شروع+length-1;
            لكل(j=0; j<length; j++) {
                k= even_لگاتار ? logical_شروع+j : logical_سرا-j;
                uchar=pBiDi->متن[k];
                إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_لبھیا++;
                }
                إذا((visual_اشاریہ+control_لبھیا)==(visual_شروع+j)) {
                    اكسر;
                }
            }
            visual_اشاریہ+=control_لبھیا;
            اكسر;
        }
    }
    /* handle all cases */
    إذا(لگاتار_گنتی<=10) {
        /* linear search for the run */
        لكل(i=0; visual_اشاریہ>=runs[i].visual_حد_بندی; ++i) {}
    } وإلا {
        /* binary search for the run */
        int32_t begin=0, حد_بندی=لگاتار_گنتی;

        /* the middle if() is guaranteed to find the run, we don't need a loop limit */
        لكل(;;) {
            i=(begin+حد_بندی)/2;
            إذا(visual_اشاریہ>=runs[i].visual_حد_بندی) {
                begin=i+1;
            } وإلا إذا(i==0 || visual_اشاریہ>=runs[i-1].visual_حد_بندی) {
                اكسر;
            } وإلا {
                حد_بندی=i;
            }
        }
    }

    شروع=runs[i].logical_شروع;
    إذا(IS_EVEN_لگاتار(شروع)) {
        /* LTR */
        /* the offset in runs[i] is visualIndex-runs[i-1].visualLimit */
        إذا(i>0) {
            visual_اشاریہ-=runs[i-1].visual_حد_بندی;
        }
        أرجع شروع+visual_اشاریہ;
    } وإلا {
        /* RTL */
        أرجع GET_اشاریہ(شروع)+runs[i].visual_حد_بندی-visual_اشاریہ-1;
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_getLogicalMap(UBiDi *pBiDi, int32_t *اشاریہ_Map, UError_کوڈ *p_Error_کوڈ) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوڈ);
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_گنتی_Runs(pBiDi, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        /* no op */
    } وإلا إذا(اشاریہ_Map==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
    } وإلا {
        /* fill a logical-to-visual index map using the runs[] */
        int32_t visual_شروع, visual_حد_بندی, i, j, k;
        int32_t logical_شروع, logical_حد_بندی;
        لگاتار *runs=pBiDi->runs;
        إذا (pBiDi->length<=0) {
            أرجع;
        }
        إذا (pBiDi->length>pBiDi->نتیجہ_لمائی) {
            uprv_memset(اشاریہ_Map, 0xFF, pBiDi->length*حجم(int32_t));
        }

        visual_شروع=0;
        لكل(j=0; j<pBiDi->لگاتار_گنتی; ++j) {
            logical_شروع=GET_اشاریہ(runs[j].logical_شروع);
            visual_حد_بندی=runs[j].visual_حد_بندی;
            إذا(IS_EVEN_لگاتار(runs[j].logical_شروع)) {
                افعل { /* LTR */
                    اشاریہ_Map[logical_شروع++]=visual_شروع++;
                } طالما(visual_شروع<visual_حد_بندی);
            } وإلا {
                logical_شروع+=visual_حد_بندی-visual_شروع;  /* logicalLimit */
                افعل { /* RTL */
                    اشاریہ_Map[--logical_شروع]=visual_شروع++;
                } طالما(visual_شروع<visual_حد_بندی);
            }
            /* visualStart==visualLimit; */
        }

        إذا(pBiDi->داخل_کرو_Points.size>0) {
            int32_t mark_لبھیا=0, لگاتار_گنتی=pBiDi->لگاتار_گنتی;
            int32_t length, داخل_کرو_Remove;
            visual_شروع=0;
            /* add number of marks found until each index */
            لكل(i=0; i<لگاتار_گنتی; i++, visual_شروع+=length) {
                length=runs[i].visual_حد_بندی-visual_شروع;
                داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
                إذا(داخل_کرو_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_لبھیا++;
                }
                إذا(mark_لبھیا>0) {
                    logical_شروع=GET_اشاریہ(runs[i].logical_شروع);
                    logical_حد_بندی=logical_شروع+length;
                    لكل(j=logical_شروع; j<logical_حد_بندی; j++) {
                        اشاریہ_Map[j]+=mark_لبھیا;
                    }
                }
                إذا(داخل_کرو_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_لبھیا++;
                }
            }
        }
        وإلا إذا(pBiDi->control_گنتی>0) {
            int32_t control_لبھیا=0, لگاتار_گنتی=pBiDi->لگاتار_گنتی;
            int32_t length, داخل_کرو_Remove;
            UBool even_لگاتار;
            char16_t uchar;
            visual_شروع=0;
            /* subtract number of controls found until each index */
            لكل(i=0; i<لگاتار_گنتی; i++, visual_شروع+=length) {
                length=runs[i].visual_حد_بندی-visual_شروع;
                داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
                /* no control found within previous runs nor within this run */
                إذا((control_لبھیا-داخل_کرو_Remove)==0) {
                    استمر;
                }
                logical_شروع=runs[i].logical_شروع;
                even_لگاتار=IS_EVEN_لگاتار(logical_شروع);
                REMOVE_ODD_ثنائی_ہندسہ(logical_شروع);
                logical_حد_بندی=logical_شروع+length;
                /* if no control within this run */
                إذا(داخل_کرو_Remove==0) {
                    لكل(j=logical_شروع; j<logical_حد_بندی; j++) {
                        اشاریہ_Map[j]-=control_لبھیا;
                    }
                    استمر;
                }
                لكل(j=0; j<length; j++) {
                    k= even_لگاتار ? logical_شروع+j : logical_حد_بندی-j-1;
                    uchar=pBiDi->متن[k];
                    إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
                        control_لبھیا++;
                        اشاریہ_Map[k]=UBIDI_MAP_NOWHERE;
                        استمر;
                    }
                    اشاریہ_Map[k]-=control_لبھیا;
                }
            }
        }
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_getVisualMap(UBiDi *pBiDi, int32_t *اشاریہ_Map, UError_کوڈ *p_Error_کوڈ) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوڈ);
    إذا(اشاریہ_Map==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_گنتی_Runs(pBiDi, p_Error_کوڈ);
    إذا(U_SUCCESS(*p_Error_کوڈ)) {
        /* fill a visual-to-logical index map using the runs[] */
        لگاتار *runs=pBiDi->runs, *runs_حد_بندی=runs+pBiDi->لگاتار_گنتی;
        int32_t logical_شروع, visual_شروع, visual_حد_بندی, *pi=اشاریہ_Map;

        إذا (pBiDi->نتیجہ_لمائی<=0) {
            أرجع;
        }
        visual_شروع=0;
        لكل(; runs<runs_حد_بندی; ++runs) {
            logical_شروع=runs->logical_شروع;
            visual_حد_بندی=runs->visual_حد_بندی;
            إذا(IS_EVEN_لگاتار(logical_شروع)) {
                افعل { /* LTR */
                    *pi++ = logical_شروع++;
                } طالما(++visual_شروع<visual_حد_بندی);
            } وإلا {
                REMOVE_ODD_ثنائی_ہندسہ(logical_شروع);
                logical_شروع+=visual_حد_بندی-visual_شروع;  /* logicalLimit */
                افعل { /* RTL */
                    *pi++ = --logical_شروع;
                } طالما(++visual_شروع<visual_حد_بندی);
            }
            /* visualStart==visualLimit; */
        }

        إذا(pBiDi->داخل_کرو_Points.size>0) {
            int32_t mark_لبھیا=0, لگاتار_گنتی=pBiDi->لگاتار_گنتی;
            int32_t داخل_کرو_Remove, i, j, k;
            runs=pBiDi->runs;
            /* count all inserted marks */
            لكل(i=0; i<لگاتار_گنتی; i++) {
                داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
                إذا(داخل_کرو_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_لبھیا++;
                }
                إذا(داخل_کرو_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_لبھیا++;
                }
            }
            /* move back indexes by number of preceding marks */
            k=pBiDi->نتیجہ_لمائی;
            لكل(i=لگاتار_گنتی-1; i>=0 && mark_لبھیا>0; i--) {
                داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
                إذا(داخل_کرو_Remove&(LRM_AFTER|RLM_AFTER)) {
                    اشاریہ_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_لبھیا--;
                }
                visual_شروع= i>0 ? runs[i-1].visual_حد_بندی : 0;
                لكل(j=runs[i].visual_حد_بندی-1; j>=visual_شروع && mark_لبھیا>0; j--) {
                    اشاریہ_Map[--k]=اشاریہ_Map[j];
                }
                إذا(داخل_کرو_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    اشاریہ_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_لبھیا--;
                }
            }
        }
        وإلا إذا(pBiDi->control_گنتی>0) {
            int32_t لگاتار_گنتی=pBiDi->لگاتار_گنتی, logical_سرا;
            int32_t داخل_کرو_Remove, length, i, j, k, m;
            char16_t uchar;
            UBool even_لگاتار;
            runs=pBiDi->runs;
            visual_شروع=0;
            /* move forward indexes by number of preceding controls */
            k=0;
            لكل(i=0; i<لگاتار_گنتی; i++, visual_شروع+=length) {
                length=runs[i].visual_حد_بندی-visual_شروع;
                داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
                /* if no control found yet, nothing to do in this run */
                إذا((داخل_کرو_Remove==0)&&(k==visual_شروع)) {
                    k+=length;
                    استمر;
                }
                /* if no control in this run */
                إذا(داخل_کرو_Remove==0) {
                    visual_حد_بندی=runs[i].visual_حد_بندی;
                    لكل(j=visual_شروع; j<visual_حد_بندی; j++) {
                        اشاریہ_Map[k++]=اشاریہ_Map[j];
                    }
                    استمر;
                }
                logical_شروع=runs[i].logical_شروع;
                even_لگاتار=IS_EVEN_لگاتار(logical_شروع);
                REMOVE_ODD_ثنائی_ہندسہ(logical_شروع);
                logical_سرا=logical_شروع+length-1;
                لكل(j=0; j<length; j++) {
                    m= even_لگاتار ? logical_شروع+j : logical_سرا-j;
                    uchar=pBiDi->متن[m];
                    إذا(!IS_BIDI_CONTROL_CHAR(uchar)) {
                        اشاریہ_Map[k++]=m;
                    }
                }
            }
        }
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_invertMap(ثابت int32_t *srcMap, int32_t *destMap, int32_t length) {
    إذا(srcMap!=خالی_سوچک && destMap!=خالی_سوچک && length>0) {
        ثابت int32_t *pi;
        int32_t dest_لمائی=-1, count=0;
        /* find highest value and count positive indexes in srcMap */
        pi=srcMap+length;
        طالما(pi>srcMap) {
            إذا(*--pi>dest_لمائی) {
                dest_لمائی=*pi;
            }
            إذا(*pi>=0) {
                count++;
            }
        }
        dest_لمائی++;           /* add 1 for origin 0 */
        إذا(count<dest_لمائی) {
            /* we must fill unmatched destMap entries with -1 */
            uprv_memset(destMap, 0xFF, dest_لمائی*حجم(int32_t));
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
