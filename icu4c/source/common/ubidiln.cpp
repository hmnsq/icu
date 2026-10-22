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
سېټ_Trailing_WSStart(UBiDi *pBiDi) {
    /* pBiDi->direction!=UBIDI_MIXED */

    ثابت DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *پوړونه=pBiDi->پوړونه;
    int32_t پیل=pBiDi->length;
    UBiDiLevel paraLevel=pBiDi->paraLevel;

    /* If the line is terminated by a block separator, all preceding WS etc...
       are already set to paragraph level.
       Setting trailingWSStart to pBidi->length will avoid changing the
       level of B chars from 0 to paraLevel in ubidi_getLevels when
       orderParagraphsLTR==true.
     */
    إذا(dirProps[پیل-1]==B) {
        pBiDi->trailingWSStart=پیل;   /* currently == pBiDi->length */
        أرجع;
    }
    /* go backwards across all WS, BN, explicit codes */
    طالما(پیل>0 && DIRPROP_FLAG(dirProps[پیل-1])&ماسک_WS) {
        --پیل;
    }

    /* if the WS run can be merged with the previous run then do so here */
    طالما(پیل>0 && پوړونه[پیل-1]==paraLevel) {
        --پیل;
    }

    pBiDi->trailingWSStart=پیل;
}

/* ubidi_setLine ------------------------------------------------------------ */

U_CAPI فراغ U_EXPORT2
ubidi_سېټ_کرښه(ثابت UBiDi *pParaBiDi,
              int32_t پیل, int32_t حد,
              UBiDi *p_کرښه_Bi_Di,
              UError_کوډ *p_Error_کوډ) {
    int32_t length;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوډ);
    RETURN_VOID_IF_NOT_VALID_PARA(pParaBiDi, *p_Error_کوډ);
    RETURN_VOID_IF_BAD_RANGE(پیل, 0, حد, *p_Error_کوډ);
    RETURN_VOID_IF_BAD_RANGE(حد, 0, pParaBiDi->length+1, *p_Error_کوډ);
    إذا(p_کرښه_Bi_Di==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا(ubidi_getParagraph(pParaBiDi, پیل, تش_اشاره, تش_اشاره, تش_اشاره, p_Error_کوډ) !=
       ubidi_getParagraph(pParaBiDi, حد-1, تش_اشاره, تش_اشاره, تش_اشاره, p_Error_کوډ)) {
        /* the line crosses a paragraph boundary */
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    /* set the values in pLineBiDi from its pParaBiDi parent */
    p_کرښه_Bi_Di->pParaBiDi=تش_اشاره;          /* mark unfinished setLine */
    p_کرښه_Bi_Di->متن=pParaBiDi->متن+پیل;
    length=p_کرښه_Bi_Di->length=حد-پیل;
    p_کرښه_Bi_Di->پایله_اوږدتیا=p_کرښه_Bi_Di->original_اوږدتیا=length;
    p_کرښه_Bi_Di->paraLevel=GET_PARALEVEL(pParaBiDi, پیل);
    p_کرښه_Bi_Di->para_شمېر=pParaBiDi->para_شمېر;
    p_کرښه_Bi_Di->runs=تش_اشاره;
    p_کرښه_Bi_Di->flags=0;
    p_کرښه_Bi_Di->reordering_ډېرپېښ=pParaBiDi->reordering_ډېرپېښ;
    p_کرښه_Bi_Di->reorderingOptions=pParaBiDi->reorderingOptions;
    p_کرښه_Bi_Di->control_شمېر=0;
    إذا(pParaBiDi->control_شمېر>0) {
        int32_t j;
        لكل(j=پیل; j<حد; j++) {
            إذا(IS_BIDI_CONTROL_CHAR(pParaBiDi->متن[j])) {
                p_کرښه_Bi_Di->control_شمېر++;
            }
        }
        p_کرښه_Bi_Di->پایله_اوږدتیا-=p_کرښه_Bi_Di->control_شمېر;
    }

    p_کرښه_Bi_Di->dirProps=pParaBiDi->dirProps+پیل;
    p_کرښه_Bi_Di->پوړونه=pParaBiDi->پوړونه+پیل;
    p_کرښه_Bi_Di->پرلپسې_شمېر=-1;

    إذا(pParaBiDi->direction!=UBIDI_MIXED) {
        /* the parent is already trivial */
        p_کرښه_Bi_Di->direction=pParaBiDi->direction;

        /*
         * The parent's levels are all either
         * implicitly or explicitly ==paraLevel;
         * do the same here.
         */
        إذا(pParaBiDi->trailingWSStart<=پیل) {
            p_کرښه_Bi_Di->trailingWSStart=0;
        } وإلا إذا(pParaBiDi->trailingWSStart<حد) {
            p_کرښه_Bi_Di->trailingWSStart=pParaBiDi->trailingWSStart-پیل;
        } وإلا {
            p_کرښه_Bi_Di->trailingWSStart=length;
        }
    } وإلا {
        ثابت UBiDiLevel *پوړونه=p_کرښه_Bi_Di->پوړونه;
        int32_t i, trailingWSStart;
        UBiDiLevel level;

        سېټ_Trailing_WSStart(p_کرښه_Bi_Di);
        trailingWSStart=p_کرښه_Bi_Di->trailingWSStart;

        /* recalculate pLineBiDi->direction */
        إذا(trailingWSStart==0) {
            /* all levels are at paraLevel */
            p_کرښه_Bi_Di->direction=(UBiDiDirection)(p_کرښه_Bi_Di->paraLevel&1);
        } وإلا {
            /* get the level of the first character */
            level=(UBiDiLevel)(پوړونه[0]&1);

            /* if there is anything of a different level, then the line is mixed */
            إذا(trailingWSStart<length && (p_کرښه_Bi_Di->paraLevel&1)!=level) {
                /* the trailing WS is at paraLevel, which differs from levels[0] */
                p_کرښه_Bi_Di->direction=UBIDI_MIXED;
            } وإلا {
                /* see if levels[1..trailingWSStart-1] have the same direction as levels[0] and paraLevel */
                i=1;
                لكل(;;) {
                    إذا(i==trailingWSStart) {
                        /* the direction values match those in level */
                        p_کرښه_Bi_Di->direction=(UBiDiDirection)level;
                        اكسر;
                    } وإلا إذا((پوړونه[i]&1)!=level) {
                        p_کرښه_Bi_Di->direction=UBIDI_MIXED;
                        اكسر;
                    }
                    ++i;
                }
            }
        }

        بدل(p_کرښه_Bi_Di->direction) {
        حالة UBIDI_LTR:
            /* make sure paraLevel is even */
            p_کرښه_Bi_Di->paraLevel=(UBiDiLevel)((p_کرښه_Bi_Di->paraLevel+1)&~1);

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_کرښه_Bi_Di->trailingWSStart=0;
            اكسر;
        حالة UBIDI_RTL:
            /* make sure paraLevel is odd */
            p_کرښه_Bi_Di->paraLevel|=1;

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_کرښه_Bi_Di->trailingWSStart=0;
            اكسر;
        افتراضي:
            اكسر;
        }
    }
    p_کرښه_Bi_Di->pParaBiDi=pParaBiDi;     /* mark successful setLine */
}

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getLevelAt(ثابت UBiDi *pBiDi, int32_t char_شاخص) {
    /* return paraLevel if in the trailing WS run, otherwise the real level */
    إذا(!IS_VALID_PARA_OR_کرښه(pBiDi) || char_شاخص<0 || pBiDi->length<=char_شاخص) {
        أرجع 0;
    } وإلا إذا(pBiDi->direction!=UBIDI_MIXED || char_شاخص>=pBiDi->trailingWSStart) {
        أرجع GET_PARALEVEL(pBiDi, char_شاخص);
    } وإلا {
        أرجع pBiDi->پوړونه[char_شاخص];
    }
}

U_CAPI ثابت UBiDiLevel* U_EXPORT2
ubidi_get_پوړونه(UBiDi* pBiDi, UError_کوډ* p_Error_کوډ) {
    int32_t پیل, length;

    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوډ, تش_اشاره);
    RETURN_IF_NOT_VALID_PARA_OR_کرښه(pBiDi, *p_Error_کوډ, تش_اشاره);
    إذا((length=pBiDi->length)<=0) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    إذا((پیل=pBiDi->trailingWSStart)==length) {
        /* the current levels array reflects the WS run */
        أرجع pBiDi->پوړونه;
    }

    /*
     * After the previous if(), we know that the levels array
     * has an implicit trailing WS run and therefore does not fully
     * reflect itself all the levels.
     * This must be a UBiDi object for a line, and
     * we need to create a new levels array.
     */
    إذا(get_پوړونه_حافظه(pBiDi, length)) {
        UBiDiLevel *پوړونه=pBiDi->پوړونه_حافظه;

        إذا(پیل>0 && پوړونه!=pBiDi->پوړونه) {
            uprv_memcpy(پوړونه, pBiDi->پوړونه, پیل);
        }
        /* pBiDi->paraLevel is ok even if contextual multiple paragraphs,
           since pBidi is a line object                                     */
        uprv_memset(پوړونه+پیل, pBiDi->paraLevel, length-پیل);

        /* this new levels array is set for the line and reflects the WS run */
        pBiDi->trailingWSStart=length;
        أرجع pBiDi->پوړونه=پوړونه;
    } وإلا {
        /* out of memory */
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_get_Logical_پرلپسې(ثابت UBiDi *pBiDi, int32_t logicalPosition,
                    int32_t *p_Logical_حد, UBiDiLevel *pLevel) {
    UError_کوډ error_کوډ;
    int32_t پرلپسې_شمېر, visual_پیل, logical_حد, logical_لومړی, i;
    پرلپسې i_پرلپسې;

    error_کوډ=U_ZERO_ERROR;
    RETURN_VOID_IF_BAD_RANGE(logicalPosition, 0, pBiDi->length, error_کوډ);
    /* ubidi_countRuns will check VALID_PARA_OR_LINE */
    پرلپسې_شمېر=ubidi_شمېر_Runs((UBiDi *)pBiDi, &error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    /* this is done based on runs rather than on levels since levels have
       a special interpretation when UBIDI_REORDER_RUNS_ONLY
     */
    visual_پیل=logical_حد=0;
    i_پرلپسې=pBiDi->runs[0];

    لكل(i=0; i<پرلپسې_شمېر; i++) {
        i_پرلپسې = pBiDi->runs[i];
        logical_لومړی=GET_شاخص(i_پرلپسې.logical_پیل);
        logical_حد=logical_لومړی+i_پرلپسې.visual_حد-visual_پیل;
        إذا((logicalPosition>=logical_لومړی) &&
           (logicalPosition<logical_حد)) {
            اكسر;
        }
        visual_پیل = i_پرلپسې.visual_حد;
    }
    إذا(p_Logical_حد) {
        *p_Logical_حد=logical_حد;
    }
    إذا(pLevel) {
        إذا(pBiDi->reordering_ډېرپېښ==UBIDI_REORDER_RUNS_ONLY) {
            *pLevel=(UBiDiLevel)GET_ODD_بټ(i_پرلپسې.logical_پیل);
        }
        وإلا إذا(pBiDi->direction!=UBIDI_MIXED || logicalPosition>=pBiDi->trailingWSStart) {
            *pLevel=GET_PARALEVEL(pBiDi, logicalPosition);
        } وإلا {
        *pLevel=pBiDi->پوړونه[logicalPosition];
        }
    }
}

/* runs API functions ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_شمېر_Runs(UBiDi *pBiDi, UError_کوډ *p_Error_کوډ) {
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوډ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_کرښه(pBiDi, *p_Error_کوډ, -1);
    ubidi_getRuns(pBiDi, p_Error_کوډ);
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع -1;
    }
    أرجع pBiDi->پرلپسې_شمېر;
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_Visual_پرلپسې(UBiDi *pBiDi, int32_t پرلپسې_شاخص,
                   int32_t *p_Logical_پیل, int32_t *p_اوږدتیا)
{
    int32_t پیل;
    UError_کوډ error_کوډ = U_ZERO_ERROR;
    RETURN_IF_NOT_VALID_PARA_OR_کرښه(pBiDi, error_کوډ, UBIDI_LTR);
    ubidi_getRuns(pBiDi, &error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع UBIDI_LTR;
    }
    RETURN_IF_BAD_RANGE(پرلپسې_شاخص, 0, pBiDi->پرلپسې_شمېر, error_کوډ, UBIDI_LTR);

    پیل=pBiDi->runs[پرلپسې_شاخص].logical_پیل;
    إذا(p_Logical_پیل!=تش_اشاره) {
        *p_Logical_پیل=GET_شاخص(پیل);
    }
    إذا(p_اوږدتیا!=تش_اشاره) {
        إذا(پرلپسې_شاخص>0) {
            *p_اوږدتیا=pBiDi->runs[پرلپسې_شاخص].visual_حد-
                     pBiDi->runs[پرلپسې_شاخص-1].visual_حد;
        } وإلا {
            *p_اوږدتیا=pBiDi->runs[0].visual_حد;
        }
    }
    أرجع (UBiDiDirection)GET_ODD_بټ(پیل);
}

/* in trivial cases there is only one trivial run; called by ubidi_getRuns() */
ساكن فراغ
get_Single_پرلپسې(UBiDi *pBiDi, UBiDiLevel level) {
    /* simple, single-run case */
    pBiDi->runs=pBiDi->simpleRuns;
    pBiDi->پرلپسې_شمېر=1;

    /* fill and reorder the single run */
    pBiDi->runs[0].logical_پیل=MAKE_شاخص_ODD_PAIR(0, level);
    pBiDi->runs[0].visual_حد=pBiDi->length;
    pBiDi->runs[0].ورننباسه_Remove=0;
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
reorder_کرښه(UBiDi *pBiDi, UBiDiLevel minLevel, UBiDiLevel maxLevel) {
    پرلپسې *runs, لنډمهاله_پرلپسې;
    UBiDiLevel *پوړونه;
    int32_t لومړی_پرلپسې, پای_ټکی_پرلپسې, حد_پرلپسې, پرلپسې_شمېر;

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
    پوړونه=pBiDi->پوړونه;
    پرلپسې_شمېر=pBiDi->پرلپسې_شمېر;

    /* do not include the WS run at paraLevel<=old minLevel except in the simple loop */
    إذا(pBiDi->trailingWSStart<pBiDi->length) {
        --پرلپسې_شمېر;
    }

    طالما(--maxLevel>=minLevel) {
        لومړی_پرلپسې=0;

        /* loop for all sequences of runs */
        لكل(;;) {
            /* look for a sequence of runs that are all at >=maxLevel */
            /* look for the first run of such a sequence */
            طالما(لومړی_پرلپسې<پرلپسې_شمېر && پوړونه[runs[لومړی_پرلپسې].logical_پیل]<maxLevel) {
                ++لومړی_پرلپسې;
            }
            إذا(لومړی_پرلپسې>=پرلپسې_شمېر) {
                اكسر;  /* no more such runs */
            }

            /* look for the limit run of such a sequence (the run behind it) */
            لكل(حد_پرلپسې=لومړی_پرلپسې; ++حد_پرلپسې<پرلپسې_شمېر && پوړونه[runs[حد_پرلپسې].logical_پیل]>=maxLevel;) {}

            /* Swap the entire sequence of runs from firstRun to limitRun-1. */
            پای_ټکی_پرلپسې=حد_پرلپسې-1;
            طالما(لومړی_پرلپسې<پای_ټکی_پرلپسې) {
                لنډمهاله_پرلپسې = runs[لومړی_پرلپسې];
                runs[لومړی_پرلپسې]=runs[پای_ټکی_پرلپسې];
                runs[پای_ټکی_پرلپسې]=لنډمهاله_پرلپسې;
                ++لومړی_پرلپسې;
                --پای_ټکی_پرلپسې;
            }

            إذا(حد_پرلپسې==پرلپسې_شمېر) {
                اكسر;  /* no more such runs */
            } وإلا {
                لومړی_پرلپسې=حد_پرلپسې+1;
            }
        }
    }

    /* now do maxLevel==old minLevel (==odd!), see above */
    إذا(!(minLevel&1)) {
        لومړی_پرلپسې=0;

        /* include the trailing WS run in this complete reordering */
        إذا(pBiDi->trailingWSStart==pBiDi->length) {
            --پرلپسې_شمېر;
        }

        /* Swap the entire sequence of all runs. (endRun==runCount) */
        طالما(لومړی_پرلپسې<پرلپسې_شمېر) {
            لنډمهاله_پرلپسې=runs[لومړی_پرلپسې];
            runs[لومړی_پرلپسې]=runs[پرلپسې_شمېر];
            runs[پرلپسې_شمېر]=لنډمهاله_پرلپسې;
            ++لومړی_پرلپسې;
            --پرلپسې_شمېر;
        }
    }
}

/* compute the runs array --------------------------------------------------- */

ساكن int32_t get_پرلپسې_From_Logical_شاخص(UBiDi *pBiDi, int32_t logical_شاخص) {
    پرلپسې *runs=pBiDi->runs;
    int32_t پرلپسې_شمېر=pBiDi->پرلپسې_شمېر, visual_پیل=0, i, length, logical_پیل;

    لكل(i=0; i<پرلپسې_شمېر; i++) {
        length=runs[i].visual_حد-visual_پیل;
        logical_پیل=GET_شاخص(runs[i].logical_پیل);
        إذا((logical_شاخص>=logical_پیل) && (logical_شاخص<(logical_پیل+length))) {
            أرجع i;
        }
        visual_پیل+=length;
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
ubidi_getRuns(UBiDi *pBiDi, UError_کوډ* p_Error_کوډ) {
    /*
     * This method returns immediately if the runs are already set. This
     * includes the case of length==0 (handled in setPara)..
     */
    إذا (pBiDi->پرلپسې_شمېر>=0) {
        أرجع ریښتیا;
    }

    إذا(pBiDi->direction!=UBIDI_MIXED) {
        /* simple, single-run case - this covers length==0 */
        /* pBiDi->paraLevel is ok even for contextual multiple paragraphs */
        get_Single_پرلپسې(pBiDi, pBiDi->paraLevel);
    } وإلا /* UBIDI_MIXED, length>0 */ {
        /* mixed directionality */
        int32_t length=pBiDi->length, حد;
        UBiDiLevel *پوړونه=pBiDi->پوړونه;
        int32_t i, پرلپسې_شمېر;
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
        حد=pBiDi->trailingWSStart;
        /* count the runs, there is at least one non-WS run, and limit>0 */
        پرلپسې_شمېر=0;
        لكل(i=0; i<حد; ++i) {
            /* increment runCount at the start of each run */
            إذا(پوړونه[i]!=level) {
                ++پرلپسې_شمېر;
                level=پوړونه[i];
            }
        }

        /*
         * We don't need to see if the last run can be merged with a trailing
         * WS run because setTrailingWSStart() would have done that.
         */
        إذا(پرلپسې_شمېر==1 && حد==length) {
            /* There is only one non-WS run and no trailing WS-run. */
            get_Single_پرلپسې(pBiDi, پوړونه[0]);
        } وإلا /* runCount>1 || limit<length */ {
            /* allocate and set the runs */
            پرلپسې *runs;
            int32_t پرلپسې_شاخص, پیل;
            UBiDiLevel minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1, maxLevel=0;

            /* now, count a (non-mergeable) WS run */
            إذا(حد<length) {
                ++پرلپسې_شمېر;
            }

            /* runCount>1 */
            إذا(get_Runs_حافظه(pBiDi, پرلپسې_شمېر)) {
                runs=pBiDi->runs_حافظه;
            } وإلا {
                *p_Error_کوډ = U_حافظه_ویش_ERROR;
                أرجع خطأ;
            }

            /* set the runs */
            /* FOOD FOR THOUGHT: this could be optimized, e.g.:
             * 464->444, 484->444, 575->555, 595->555
             * However, that would take longer. Check also how it would
             * interact with BiDi control removal and inserting Marks.
             */
            پرلپسې_شاخص=0;

            /* search for the run limits and initialize visualLimit values with the run lengths */
            i=0;
            افعل {
                /* prepare this run */
                پیل=i;
                level=پوړونه[i];
                إذا(level<minLevel) {
                    minLevel=level;
                }
                إذا(level>maxLevel) {
                    maxLevel=level;
                }

                /* look for the run limit */
                طالما(++i<حد && پوړونه[i]==level) {}

                /* i is another run limit */
                runs[پرلپسې_شاخص].logical_پیل=پیل;
                runs[پرلپسې_شاخص].visual_حد=i-پیل;
                runs[پرلپسې_شاخص].ورننباسه_Remove=0;
                ++پرلپسې_شاخص;
            } طالما(i<حد);

            إذا(حد<length) {
                /* there is a separate WS run */
                runs[پرلپسې_شاخص].logical_پیل=حد;
                runs[پرلپسې_شاخص].visual_حد=length-حد;
                /* For the trailing WS run, pBiDi->paraLevel is ok even
                   if contextual multiple paragraphs.                   */
                إذا(pBiDi->paraLevel<minLevel) {
                    minLevel=pBiDi->paraLevel;
                }
            }

            /* set the object fields */
            pBiDi->runs=runs;
            pBiDi->پرلپسې_شمېر=پرلپسې_شمېر;

            reorder_کرښه(pBiDi, minLevel, maxLevel);

            /* now add the direction flags and adjust the visualLimit's to be just that */
            /* this loop will also handle the trailing WS run */
            حد=0;
            لكل(i=0; i<پرلپسې_شمېر; ++i) {
                جمع_کړه_ODD_بټ_FROM_LEVEL(runs[i].logical_پیل, پوړونه[runs[i].logical_پیل]);
                حد+=runs[i].visual_حد;
                runs[i].visual_حد=حد;
            }

            /* Set the "odd" bit for the trailing WS run. */
            /* For a RTL paragraph, it will be the *first* run in visual order. */
            /* For the trailing WS run, pBiDi->paraLevel is ok even if
               contextual multiple paragraphs.                          */
            إذا(پرلپسې_شاخص<پرلپسې_شمېر) {
                int32_t trailing_پرلپسې = ((pBiDi->paraLevel & 1) != 0)? 0 : پرلپسې_شاخص;

                جمع_کړه_ODD_بټ_FROM_LEVEL(runs[trailing_پرلپسې].logical_پیل, pBiDi->paraLevel);
            }
        }
    }

    /* handle insert LRM/RLM BEFORE/AFTER run */
    إذا(pBiDi->ورننباسه_Points.size>0) {
        ټکی *ټکی_2, *پیل=pBiDi->ورننباسه_Points.points,
                      *حد=پیل+pBiDi->ورننباسه_Points.size;
        int32_t پرلپسې_شاخص;
        لكل(ټکی_2=پیل; ټکی_2<حد; ټکی_2++) {
            پرلپسې_شاخص=get_پرلپسې_From_Logical_شاخص(pBiDi, ټکی_2->pos);
            pBiDi->runs[پرلپسې_شاخص].ورننباسه_Remove|=ټکی_2->flag;
        }
    }

    /* handle remove BiDi control characters */
    إذا(pBiDi->control_شمېر>0) {
        int32_t پرلپسې_شاخص;
        ثابت char16_t *پیل=pBiDi->متن, *حد=پیل+pBiDi->length, *pu;
        لكل(pu=پیل; pu<حد; pu++) {
            إذا(IS_BIDI_CONTROL_CHAR(*pu)) {
                پرلپسې_شاخص=get_پرلپسې_From_Logical_شاخص(pBiDi, (int32_t)(pu-پیل));
                pBiDi->runs[پرلپسې_شاخص].ورننباسه_Remove--;
            }
        }
    }

    أرجع ریښتیا;
}

ساكن UBool
prepareReorder(ثابت UBiDiLevel *پوړونه, int32_t length,
               int32_t *شاخص_Map,
               UBiDiLevel *pMinLevel, UBiDiLevel *pMaxLevel) {
    int32_t پیل;
    UBiDiLevel level, minLevel, maxLevel;

    إذا(پوړونه==تش_اشاره || length<=0) {
        أرجع خطأ;
    }

    /* determine minLevel and maxLevel */
    minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1;
    maxLevel=0;
    لكل(پیل=length; پیل>0;) {
        level=پوړونه[--پیل];
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
    لكل(پیل=length; پیل>0;) {
        --پیل;
        شاخص_Map[پیل]=پیل;
    }

    أرجع ریښتیا;
}

/* reorder a line based on a levels array (L2) ------------------------------ */

U_CAPI فراغ U_EXPORT2
ubidi_reorderLogical(ثابت UBiDiLevel *پوړونه, int32_t length, int32_t *شاخص_Map) {
    int32_t پیل, حد, مجموعه_د_Sos_Eos;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    إذا(شاخص_Map==تش_اشاره || !prepareReorder(پوړونه, length, شاخص_Map, &minLevel, &maxLevel)) {
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
        پیل=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        لكل(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            طالما(پیل<length && پوړونه[پیل]<maxLevel) {
                ++پیل;
            }
            إذا(پیل>=length) {
                اكسر;  /* no more such sequences */
            }

            /* look for the limit of such a sequence (the index behind it) */
            لكل(حد=پیل; ++حد<length && پوړونه[حد]>=maxLevel;) {}

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
            مجموعه_د_Sos_Eos=پیل+حد-1;

            /* reorder each index in the sequence */
            افعل {
                شاخص_Map[پیل]=مجموعه_د_Sos_Eos-شاخص_Map[پیل];
            } طالما(++پیل<حد);

            /* start==limit */
            إذا(حد==length) {
                اكسر;  /* no more such sequences */
            } وإلا {
                پیل=حد+1;
            }
        }
    } طالما(--maxLevel>=minLevel);
}

U_CAPI فراغ U_EXPORT2
ubidi_reorderVisual(ثابت UBiDiLevel *پوړونه, int32_t length, int32_t *شاخص_Map) {
    int32_t پیل, پای_ټکی, حد, لنډمهاله;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    إذا(شاخص_Map==تش_اشاره || !prepareReorder(پوړونه, length, شاخص_Map, &minLevel, &maxLevel)) {
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
        پیل=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        لكل(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            طالما(پیل<length && پوړونه[پیل]<maxLevel) {
                ++پیل;
            }
            إذا(پیل>=length) {
                اكسر;  /* no more such runs */
            }

            /* look for the limit of such a sequence (the index behind it) */
            لكل(حد=پیل; ++حد<length && پوړونه[حد]>=maxLevel;) {}

            /*
             * Swap the entire interval of indexes from start to limit-1.
             * We don't need to swap the levels for the purpose of this
             * algorithm: the sequence of levels that we look at does not
             * move anyway.
             */
            پای_ټکی=حد-1;
            طالما(پیل<پای_ټکی) {
                لنډمهاله=شاخص_Map[پیل];
                شاخص_Map[پیل]=شاخص_Map[پای_ټکی];
                شاخص_Map[پای_ټکی]=لنډمهاله;

                ++پیل;
                --پای_ټکی;
            }

            إذا(حد==length) {
                اكسر;  /* no more such sequences */
            } وإلا {
                پیل=حد+1;
            }
        }
    } طالما(--maxLevel>=minLevel);
}

/* API functions for logical<->visual mapping ------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_get_Visual_شاخص(UBiDi *pBiDi, int32_t logical_شاخص, UError_کوډ *p_Error_کوډ) {
    int32_t visual_شاخص=UBIDI_MAP_NOWHERE;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوډ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_کرښه(pBiDi, *p_Error_کوډ, -1);
    RETURN_IF_BAD_RANGE(logical_شاخص, 0, pBiDi->length, *p_Error_کوډ, -1);

    /* we can do the trivial cases without the runs array */
    بدل(pBiDi->direction) {
    حالة UBIDI_LTR:
        visual_شاخص=logical_شاخص;
        اكسر;
    حالة UBIDI_RTL:
        visual_شاخص=pBiDi->length-logical_شاخص-1;
        اكسر;
    افتراضي:
        إذا(!ubidi_getRuns(pBiDi, p_Error_کوډ)) {
            *p_Error_کوډ=U_حافظه_ویش_ERROR;
            أرجع -1;
        } وإلا {
            پرلپسې *runs=pBiDi->runs;
            int32_t i, visual_پیل=0, دننه_واټن, length;

            /* linear search for the run, search on the visual runs */
            لكل(i=0; i<pBiDi->پرلپسې_شمېر; ++i) {
                length=runs[i].visual_حد-visual_پیل;
                دننه_واټن=logical_شاخص-GET_شاخص(runs[i].logical_پیل);
                إذا(دننه_واټن>=0 && دننه_واټن<length) {
                    إذا(IS_EVEN_پرلپسې(runs[i].logical_پیل)) {
                        /* LTR */
                        visual_شاخص=visual_پیل+دننه_واټن;
                    } وإلا {
                        /* RTL */
                        visual_شاخص=visual_پیل+length-دننه_واټن-1;
                    }
                    اكسر;          /* exit for loop */
                }
                visual_پیل+=length;
            }
            إذا(i>=pBiDi->پرلپسې_شمېر) {
                أرجع UBIDI_MAP_NOWHERE;
            }
        }
    }

    إذا(pBiDi->ورننباسه_Points.size>0) {
        /* add the number of added marks until the calculated visual index */
        پرلپسې *runs=pBiDi->runs;
        int32_t i, length, ورننباسه_Remove;
        int32_t visual_پیل=0, mark_وموندل=0;
        لكل(i=0; ; i++, visual_پیل+=length) {
            length=runs[i].visual_حد-visual_پیل;
            ورننباسه_Remove=runs[i].ورننباسه_Remove;
            إذا(ورننباسه_Remove & (LRM_BEFORE|RLM_BEFORE)) {
                mark_وموندل++;
            }
            /* is it the run containing the visual index? */
            إذا(visual_شاخص<runs[i].visual_حد) {
                أرجع visual_شاخص+mark_وموندل;
            }
            إذا(ورننباسه_Remove & (LRM_AFTER|RLM_AFTER)) {
                mark_وموندل++;
            }
        }
    }
    وإلا إذا(pBiDi->control_شمېر>0) {
        /* subtract the number of controls until the calculated visual index */
        پرلپسې *runs=pBiDi->runs;
        int32_t i, j, پیل, حد, length, ورننباسه_Remove;
        int32_t visual_پیل=0, control_وموندل=0;
        char16_t uchar=pBiDi->متن[logical_شاخص];
        /* is the logical index pointing to a control ? */
        إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
            أرجع UBIDI_MAP_NOWHERE;
        }
        /* loop on runs */
        لكل(i=0; ; i++, visual_پیل+=length) {
            length=runs[i].visual_حد-visual_پیل;
            ورننباسه_Remove=runs[i].ورننباسه_Remove;
            /* calculated visual index is beyond this run? */
            إذا(visual_شاخص>=runs[i].visual_حد) {
                control_وموندل-=ورننباسه_Remove;
                استمر;
            }
            /* calculated visual index must be within current run */
            إذا(ورننباسه_Remove==0) {
                أرجع visual_شاخص-control_وموندل;
            }
            إذا(IS_EVEN_پرلپسې(runs[i].logical_پیل)) {
                /* LTR: check from run start to logical index */
                پیل=runs[i].logical_پیل;
                حد=logical_شاخص;
            } وإلا {
                /* RTL: check from logical index to run end */
                پیل=logical_شاخص+1;
                حد=GET_شاخص(runs[i].logical_پیل)+length;
            }
            لكل(j=پیل; j<حد; j++) {
                uchar=pBiDi->متن[j];
                إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_وموندل++;
                }
            }
            أرجع visual_شاخص-control_وموندل;
        }
    }

    أرجع visual_شاخص;
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Logical_شاخص(UBiDi *pBiDi, int32_t visual_شاخص, UError_کوډ *p_Error_کوډ) {
    پرلپسې *runs;
    int32_t i, پرلپسې_شمېر, پیل;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوډ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_کرښه(pBiDi, *p_Error_کوډ, -1);
    RETURN_IF_BAD_RANGE(visual_شاخص, 0, pBiDi->پایله_اوږدتیا, *p_Error_کوډ, -1);
    /* we can do the trivial cases without the runs array */
    إذا(pBiDi->ورننباسه_Points.size==0 && pBiDi->control_شمېر==0) {
        إذا(pBiDi->direction==UBIDI_LTR) {
            أرجع visual_شاخص;
        }
        وإلا إذا(pBiDi->direction==UBIDI_RTL) {
            أرجع pBiDi->length-visual_شاخص-1;
        }
    }
    إذا(!ubidi_getRuns(pBiDi, p_Error_کوډ)) {
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        أرجع -1;
    }

    runs=pBiDi->runs;
    پرلپسې_شمېر=pBiDi->پرلپسې_شمېر;
    إذا(pBiDi->ورننباسه_Points.size>0) {
        /* handle inserted LRM/RLM */
        int32_t mark_وموندل=0, ورننباسه_Remove;
        int32_t visual_پیل=0, length;
        runs=pBiDi->runs;
        /* subtract number of marks until visual index */
        لكل(i=0; ; i++, visual_پیل+=length) {
            length=runs[i].visual_حد-visual_پیل;
            ورننباسه_Remove=runs[i].ورننباسه_Remove;
            إذا(ورننباسه_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                إذا(visual_شاخص<=(visual_پیل+mark_وموندل)) {
                    أرجع UBIDI_MAP_NOWHERE;
                }
                mark_وموندل++;
            }
            /* is adjusted visual index within this run? */
            إذا(visual_شاخص<(runs[i].visual_حد+mark_وموندل)) {
                visual_شاخص-=mark_وموندل;
                اكسر;
            }
            إذا(ورننباسه_Remove&(LRM_AFTER|RLM_AFTER)) {
                إذا(visual_شاخص==(visual_پیل+length+mark_وموندل)) {
                    أرجع UBIDI_MAP_NOWHERE;
                }
                mark_وموندل++;
            }
        }
    }
    وإلا إذا(pBiDi->control_شمېر>0) {
        /* handle removed BiDi control characters */
        int32_t control_وموندل=0, ورننباسه_Remove, length;
        int32_t logical_پیل, logical_پای_ټکی, visual_پیل=0, j, k;
        char16_t uchar;
        UBool even_پرلپسې;
        /* add number of controls until visual index */
        لكل(i=0; ; i++, visual_پیل+=length) {
            length=runs[i].visual_حد-visual_پیل;
            ورننباسه_Remove=runs[i].ورننباسه_Remove;
            /* is adjusted visual index beyond current run? */
            إذا(visual_شاخص>=(runs[i].visual_حد-control_وموندل+ورننباسه_Remove)) {
                control_وموندل-=ورننباسه_Remove;
                استمر;
            }
            /* adjusted visual index is within current run */
            إذا(ورننباسه_Remove==0) {
                visual_شاخص+=control_وموندل;
                اكسر;
            }
            /* count non-control chars until visualIndex */
            logical_پیل=runs[i].logical_پیل;
            even_پرلپسې=IS_EVEN_پرلپسې(logical_پیل);
            REMOVE_ODD_بټ(logical_پیل);
            logical_پای_ټکی=logical_پیل+length-1;
            لكل(j=0; j<length; j++) {
                k= even_پرلپسې ? logical_پیل+j : logical_پای_ټکی-j;
                uchar=pBiDi->متن[k];
                إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_وموندل++;
                }
                إذا((visual_شاخص+control_وموندل)==(visual_پیل+j)) {
                    اكسر;
                }
            }
            visual_شاخص+=control_وموندل;
            اكسر;
        }
    }
    /* handle all cases */
    إذا(پرلپسې_شمېر<=10) {
        /* linear search for the run */
        لكل(i=0; visual_شاخص>=runs[i].visual_حد; ++i) {}
    } وإلا {
        /* binary search for the run */
        int32_t begin=0, حد=پرلپسې_شمېر;

        /* the middle if() is guaranteed to find the run, we don't need a loop limit */
        لكل(;;) {
            i=(begin+حد)/2;
            إذا(visual_شاخص>=runs[i].visual_حد) {
                begin=i+1;
            } وإلا إذا(i==0 || visual_شاخص>=runs[i-1].visual_حد) {
                اكسر;
            } وإلا {
                حد=i;
            }
        }
    }

    پیل=runs[i].logical_پیل;
    إذا(IS_EVEN_پرلپسې(پیل)) {
        /* LTR */
        /* the offset in runs[i] is visualIndex-runs[i-1].visualLimit */
        إذا(i>0) {
            visual_شاخص-=runs[i-1].visual_حد;
        }
        أرجع پیل+visual_شاخص;
    } وإلا {
        /* RTL */
        أرجع GET_شاخص(پیل)+runs[i].visual_حد-visual_شاخص-1;
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_getLogicalMap(UBiDi *pBiDi, int32_t *شاخص_Map, UError_کوډ *p_Error_کوډ) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوډ);
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_شمېر_Runs(pBiDi, p_Error_کوډ);
    إذا(U_FAILURE(*p_Error_کوډ)) {
        /* no op */
    } وإلا إذا(شاخص_Map==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
    } وإلا {
        /* fill a logical-to-visual index map using the runs[] */
        int32_t visual_پیل, visual_حد, i, j, k;
        int32_t logical_پیل, logical_حد;
        پرلپسې *runs=pBiDi->runs;
        إذا (pBiDi->length<=0) {
            أرجع;
        }
        إذا (pBiDi->length>pBiDi->پایله_اوږدتیا) {
            uprv_memset(شاخص_Map, 0xFF, pBiDi->length*حجم(int32_t));
        }

        visual_پیل=0;
        لكل(j=0; j<pBiDi->پرلپسې_شمېر; ++j) {
            logical_پیل=GET_شاخص(runs[j].logical_پیل);
            visual_حد=runs[j].visual_حد;
            إذا(IS_EVEN_پرلپسې(runs[j].logical_پیل)) {
                افعل { /* LTR */
                    شاخص_Map[logical_پیل++]=visual_پیل++;
                } طالما(visual_پیل<visual_حد);
            } وإلا {
                logical_پیل+=visual_حد-visual_پیل;  /* logicalLimit */
                افعل { /* RTL */
                    شاخص_Map[--logical_پیل]=visual_پیل++;
                } طالما(visual_پیل<visual_حد);
            }
            /* visualStart==visualLimit; */
        }

        إذا(pBiDi->ورننباسه_Points.size>0) {
            int32_t mark_وموندل=0, پرلپسې_شمېر=pBiDi->پرلپسې_شمېر;
            int32_t length, ورننباسه_Remove;
            visual_پیل=0;
            /* add number of marks found until each index */
            لكل(i=0; i<پرلپسې_شمېر; i++, visual_پیل+=length) {
                length=runs[i].visual_حد-visual_پیل;
                ورننباسه_Remove=runs[i].ورننباسه_Remove;
                إذا(ورننباسه_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_وموندل++;
                }
                إذا(mark_وموندل>0) {
                    logical_پیل=GET_شاخص(runs[i].logical_پیل);
                    logical_حد=logical_پیل+length;
                    لكل(j=logical_پیل; j<logical_حد; j++) {
                        شاخص_Map[j]+=mark_وموندل;
                    }
                }
                إذا(ورننباسه_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_وموندل++;
                }
            }
        }
        وإلا إذا(pBiDi->control_شمېر>0) {
            int32_t control_وموندل=0, پرلپسې_شمېر=pBiDi->پرلپسې_شمېر;
            int32_t length, ورننباسه_Remove;
            UBool even_پرلپسې;
            char16_t uchar;
            visual_پیل=0;
            /* subtract number of controls found until each index */
            لكل(i=0; i<پرلپسې_شمېر; i++, visual_پیل+=length) {
                length=runs[i].visual_حد-visual_پیل;
                ورننباسه_Remove=runs[i].ورننباسه_Remove;
                /* no control found within previous runs nor within this run */
                إذا((control_وموندل-ورننباسه_Remove)==0) {
                    استمر;
                }
                logical_پیل=runs[i].logical_پیل;
                even_پرلپسې=IS_EVEN_پرلپسې(logical_پیل);
                REMOVE_ODD_بټ(logical_پیل);
                logical_حد=logical_پیل+length;
                /* if no control within this run */
                إذا(ورننباسه_Remove==0) {
                    لكل(j=logical_پیل; j<logical_حد; j++) {
                        شاخص_Map[j]-=control_وموندل;
                    }
                    استمر;
                }
                لكل(j=0; j<length; j++) {
                    k= even_پرلپسې ? logical_پیل+j : logical_حد-j-1;
                    uchar=pBiDi->متن[k];
                    إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
                        control_وموندل++;
                        شاخص_Map[k]=UBIDI_MAP_NOWHERE;
                        استمر;
                    }
                    شاخص_Map[k]-=control_وموندل;
                }
            }
        }
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_getVisualMap(UBiDi *pBiDi, int32_t *شاخص_Map, UError_کوډ *p_Error_کوډ) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوډ);
    إذا(شاخص_Map==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_شمېر_Runs(pBiDi, p_Error_کوډ);
    إذا(U_SUCCESS(*p_Error_کوډ)) {
        /* fill a visual-to-logical index map using the runs[] */
        پرلپسې *runs=pBiDi->runs, *runs_حد=runs+pBiDi->پرلپسې_شمېر;
        int32_t logical_پیل, visual_پیل, visual_حد, *pi=شاخص_Map;

        إذا (pBiDi->پایله_اوږدتیا<=0) {
            أرجع;
        }
        visual_پیل=0;
        لكل(; runs<runs_حد; ++runs) {
            logical_پیل=runs->logical_پیل;
            visual_حد=runs->visual_حد;
            إذا(IS_EVEN_پرلپسې(logical_پیل)) {
                افعل { /* LTR */
                    *pi++ = logical_پیل++;
                } طالما(++visual_پیل<visual_حد);
            } وإلا {
                REMOVE_ODD_بټ(logical_پیل);
                logical_پیل+=visual_حد-visual_پیل;  /* logicalLimit */
                افعل { /* RTL */
                    *pi++ = --logical_پیل;
                } طالما(++visual_پیل<visual_حد);
            }
            /* visualStart==visualLimit; */
        }

        إذا(pBiDi->ورننباسه_Points.size>0) {
            int32_t mark_وموندل=0, پرلپسې_شمېر=pBiDi->پرلپسې_شمېر;
            int32_t ورننباسه_Remove, i, j, k;
            runs=pBiDi->runs;
            /* count all inserted marks */
            لكل(i=0; i<پرلپسې_شمېر; i++) {
                ورننباسه_Remove=runs[i].ورننباسه_Remove;
                إذا(ورننباسه_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_وموندل++;
                }
                إذا(ورننباسه_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_وموندل++;
                }
            }
            /* move back indexes by number of preceding marks */
            k=pBiDi->پایله_اوږدتیا;
            لكل(i=پرلپسې_شمېر-1; i>=0 && mark_وموندل>0; i--) {
                ورننباسه_Remove=runs[i].ورننباسه_Remove;
                إذا(ورننباسه_Remove&(LRM_AFTER|RLM_AFTER)) {
                    شاخص_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_وموندل--;
                }
                visual_پیل= i>0 ? runs[i-1].visual_حد : 0;
                لكل(j=runs[i].visual_حد-1; j>=visual_پیل && mark_وموندل>0; j--) {
                    شاخص_Map[--k]=شاخص_Map[j];
                }
                إذا(ورننباسه_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    شاخص_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_وموندل--;
                }
            }
        }
        وإلا إذا(pBiDi->control_شمېر>0) {
            int32_t پرلپسې_شمېر=pBiDi->پرلپسې_شمېر, logical_پای_ټکی;
            int32_t ورننباسه_Remove, length, i, j, k, m;
            char16_t uchar;
            UBool even_پرلپسې;
            runs=pBiDi->runs;
            visual_پیل=0;
            /* move forward indexes by number of preceding controls */
            k=0;
            لكل(i=0; i<پرلپسې_شمېر; i++, visual_پیل+=length) {
                length=runs[i].visual_حد-visual_پیل;
                ورننباسه_Remove=runs[i].ورننباسه_Remove;
                /* if no control found yet, nothing to do in this run */
                إذا((ورننباسه_Remove==0)&&(k==visual_پیل)) {
                    k+=length;
                    استمر;
                }
                /* if no control in this run */
                إذا(ورننباسه_Remove==0) {
                    visual_حد=runs[i].visual_حد;
                    لكل(j=visual_پیل; j<visual_حد; j++) {
                        شاخص_Map[k++]=شاخص_Map[j];
                    }
                    استمر;
                }
                logical_پیل=runs[i].logical_پیل;
                even_پرلپسې=IS_EVEN_پرلپسې(logical_پیل);
                REMOVE_ODD_بټ(logical_پیل);
                logical_پای_ټکی=logical_پیل+length-1;
                لكل(j=0; j<length; j++) {
                    m= even_پرلپسې ? logical_پیل+j : logical_پای_ټکی-j;
                    uchar=pBiDi->متن[m];
                    إذا(!IS_BIDI_CONTROL_CHAR(uchar)) {
                        شاخص_Map[k++]=m;
                    }
                }
            }
        }
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_invertMap(ثابت int32_t *srcMap, int32_t *destMap, int32_t length) {
    إذا(srcMap!=تش_اشاره && destMap!=تش_اشاره && length>0) {
        ثابت int32_t *pi;
        int32_t dest_اوږدتیا=-1, count=0;
        /* find highest value and count positive indexes in srcMap */
        pi=srcMap+length;
        طالما(pi>srcMap) {
            إذا(*--pi>dest_اوږدتیا) {
                dest_اوږدتیا=*pi;
            }
            إذا(*pi>=0) {
                count++;
            }
        }
        dest_اوږدتیا++;           /* add 1 for origin 0 */
        إذا(count<dest_اوږدتیا) {
            /* we must fill unmatched destMap entries with -1 */
            uprv_memset(destMap, 0xFF, dest_اوږدتیا*حجم(int32_t));
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
