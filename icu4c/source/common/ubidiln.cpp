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

#شامل "cmemory.h"
#شامل "unicode/utypes.h"
#شامل "unicode/ustring.h"
#شامل "unicode/uchar.h"
#شامل "unicode/ubidi.h"
#شامل "ubidiimp.h"
#شامل "uassert.h"

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
ساکن خالی
سیٹ_Trailing_WSStart(UBiDi *pBiDi) {
    /* pBiDi->direction!=UBIDI_MIXED */

    ثابت DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *سطحیں=pBiDi->سطحیں;
    int32_t آغاز=pBiDi->length;
    UBiDiLevel paraLevel=pBiDi->paraLevel;

    /* If the line is terminated by a block separator, all preceding WS etc...
       are already set to paragraph level.
       Setting trailingWSStart to pBidi->length will avoid changing the
       level of B chars from 0 to paraLevel in ubidi_getLevels when
       orderParagraphsLTR==true.
     */
    اگر(dirProps[آغاز-1]==B) {
        pBiDi->trailingWSStart=آغاز;   /* currently == pBiDi->length */
        واپس;
    }
    /* go backwards across all WS, BN, explicit codes */
    جب(آغاز>0 && DIRPROP_FLAG(dirProps[آغاز-1])&نقاب_WS) {
        --آغاز;
    }

    /* if the WS run can be merged with the previous run then do so here */
    جب(آغاز>0 && سطحیں[آغاز-1]==paraLevel) {
        --آغاز;
    }

    pBiDi->trailingWSStart=آغاز;
}

/* ubidi_setLine ------------------------------------------------------------ */

U_CAPI خالی U_EXPORT2
ubidi_سیٹ_سطر(ثابت UBiDi *pParaBiDi,
              int32_t آغاز, int32_t حد_بندی,
              UBiDi *p_سطر_Bi_Di,
              UError_رمز *p_Error_رمز) {
    int32_t length;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_رمز);
    RETURN_VOID_IF_NOT_VALID_PARA(pParaBiDi, *p_Error_رمز);
    RETURN_VOID_IF_BAD_RANGE(آغاز, 0, حد_بندی, *p_Error_رمز);
    RETURN_VOID_IF_BAD_RANGE(حد_بندی, 0, pParaBiDi->length+1, *p_Error_رمز);
    اگر(p_سطر_Bi_Di==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    اگر(ubidi_getParagraph(pParaBiDi, آغاز, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, p_Error_رمز) !=
       ubidi_getParagraph(pParaBiDi, حد_بندی-1, خالی_اشاریہ, خالی_اشاریہ, خالی_اشاریہ, p_Error_رمز)) {
        /* the line crosses a paragraph boundary */
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    /* set the values in pLineBiDi from its pParaBiDi parent */
    p_سطر_Bi_Di->pParaBiDi=خالی_اشاریہ;          /* mark unfinished setLine */
    p_سطر_Bi_Di->متن=pParaBiDi->متن+آغاز;
    length=p_سطر_Bi_Di->length=حد_بندی-آغاز;
    p_سطر_Bi_Di->نتیجہ_لمبائی=p_سطر_Bi_Di->original_لمبائی=length;
    p_سطر_Bi_Di->paraLevel=GET_PARALEVEL(pParaBiDi, آغاز);
    p_سطر_Bi_Di->para_گنتی=pParaBiDi->para_گنتی;
    p_سطر_Bi_Di->runs=خالی_اشاریہ;
    p_سطر_Bi_Di->flags=0;
    p_سطر_Bi_Di->reordering_منوال=pParaBiDi->reordering_منوال;
    p_سطر_Bi_Di->reorderingOptions=pParaBiDi->reorderingOptions;
    p_سطر_Bi_Di->control_گنتی=0;
    اگر(pParaBiDi->control_گنتی>0) {
        int32_t j;
        برائے(j=آغاز; j<حد_بندی; j++) {
            اگر(IS_BIDI_CONTROL_CHAR(pParaBiDi->متن[j])) {
                p_سطر_Bi_Di->control_گنتی++;
            }
        }
        p_سطر_Bi_Di->نتیجہ_لمبائی-=p_سطر_Bi_Di->control_گنتی;
    }

    p_سطر_Bi_Di->dirProps=pParaBiDi->dirProps+آغاز;
    p_سطر_Bi_Di->سطحیں=pParaBiDi->سطحیں+آغاز;
    p_سطر_Bi_Di->تسلسل_گنتی=-1;

    اگر(pParaBiDi->direction!=UBIDI_MIXED) {
        /* the parent is already trivial */
        p_سطر_Bi_Di->direction=pParaBiDi->direction;

        /*
         * The parent's levels are all either
         * implicitly or explicitly ==paraLevel;
         * do the same here.
         */
        اگر(pParaBiDi->trailingWSStart<=آغاز) {
            p_سطر_Bi_Di->trailingWSStart=0;
        } ورنہ اگر(pParaBiDi->trailingWSStart<حد_بندی) {
            p_سطر_Bi_Di->trailingWSStart=pParaBiDi->trailingWSStart-آغاز;
        } ورنہ {
            p_سطر_Bi_Di->trailingWSStart=length;
        }
    } ورنہ {
        ثابت UBiDiLevel *سطحیں=p_سطر_Bi_Di->سطحیں;
        int32_t i, trailingWSStart;
        UBiDiLevel level;

        سیٹ_Trailing_WSStart(p_سطر_Bi_Di);
        trailingWSStart=p_سطر_Bi_Di->trailingWSStart;

        /* recalculate pLineBiDi->direction */
        اگر(trailingWSStart==0) {
            /* all levels are at paraLevel */
            p_سطر_Bi_Di->direction=(UBiDiDirection)(p_سطر_Bi_Di->paraLevel&1);
        } ورنہ {
            /* get the level of the first character */
            level=(UBiDiLevel)(سطحیں[0]&1);

            /* if there is anything of a different level, then the line is mixed */
            اگر(trailingWSStart<length && (p_سطر_Bi_Di->paraLevel&1)!=level) {
                /* the trailing WS is at paraLevel, which differs from levels[0] */
                p_سطر_Bi_Di->direction=UBIDI_MIXED;
            } ورنہ {
                /* see if levels[1..trailingWSStart-1] have the same direction as levels[0] and paraLevel */
                i=1;
                برائے(;;) {
                    اگر(i==trailingWSStart) {
                        /* the direction values match those in level */
                        p_سطر_Bi_Di->direction=(UBiDiDirection)level;
                        توڑو;
                    } ورنہ اگر((سطحیں[i]&1)!=level) {
                        p_سطر_Bi_Di->direction=UBIDI_MIXED;
                        توڑو;
                    }
                    ++i;
                }
            }
        }

        انتخاب(p_سطر_Bi_Di->direction) {
        حالت UBIDI_LTR:
            /* make sure paraLevel is even */
            p_سطر_Bi_Di->paraLevel=(UBiDiLevel)((p_سطر_Bi_Di->paraLevel+1)&~1);

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_سطر_Bi_Di->trailingWSStart=0;
            توڑو;
        حالت UBIDI_RTL:
            /* make sure paraLevel is odd */
            p_سطر_Bi_Di->paraLevel|=1;

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_سطر_Bi_Di->trailingWSStart=0;
            توڑو;
        طےشدہ:
            توڑو;
        }
    }
    p_سطر_Bi_Di->pParaBiDi=pParaBiDi;     /* mark successful setLine */
}

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getLevelAt(ثابت UBiDi *pBiDi, int32_t char_اشاریہ) {
    /* return paraLevel if in the trailing WS run, otherwise the real level */
    اگر(!IS_VALID_PARA_OR_سطر(pBiDi) || char_اشاریہ<0 || pBiDi->length<=char_اشاریہ) {
        واپس 0;
    } ورنہ اگر(pBiDi->direction!=UBIDI_MIXED || char_اشاریہ>=pBiDi->trailingWSStart) {
        واپس GET_PARALEVEL(pBiDi, char_اشاریہ);
    } ورنہ {
        واپس pBiDi->سطحیں[char_اشاریہ];
    }
}

U_CAPI ثابت UBiDiLevel* U_EXPORT2
ubidi_get_سطحیں(UBiDi* pBiDi, UError_رمز* p_Error_رمز) {
    int32_t آغاز, length;

    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_رمز, خالی_اشاریہ);
    RETURN_IF_NOT_VALID_PARA_OR_سطر(pBiDi, *p_Error_رمز, خالی_اشاریہ);
    اگر((length=pBiDi->length)<=0) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    اگر((آغاز=pBiDi->trailingWSStart)==length) {
        /* the current levels array reflects the WS run */
        واپس pBiDi->سطحیں;
    }

    /*
     * After the previous if(), we know that the levels array
     * has an implicit trailing WS run and therefore does not fully
     * reflect itself all the levels.
     * This must be a UBiDi object for a line, and
     * we need to create a new levels array.
     */
    اگر(get_سطحیں_حافظہ(pBiDi, length)) {
        UBiDiLevel *سطحیں=pBiDi->سطحیں_حافظہ;

        اگر(آغاز>0 && سطحیں!=pBiDi->سطحیں) {
            uprv_memcpy(سطحیں, pBiDi->سطحیں, آغاز);
        }
        /* pBiDi->paraLevel is ok even if contextual multiple paragraphs,
           since pBidi is a line object                                     */
        uprv_memset(سطحیں+آغاز, pBiDi->paraLevel, length-آغاز);

        /* this new levels array is set for the line and reflects the WS run */
        pBiDi->trailingWSStart=length;
        واپس pBiDi->سطحیں=سطحیں;
    } ورنہ {
        /* out of memory */
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
}

U_CAPI خالی U_EXPORT2
ubidi_get_Logical_تسلسل(ثابت UBiDi *pBiDi, int32_t logicalPosition,
                    int32_t *p_Logical_حد_بندی, UBiDiLevel *pLevel) {
    UError_رمز error_رمز;
    int32_t تسلسل_گنتی, visual_آغاز, logical_حد_بندی, logical_پہلا, i;
    تسلسل i_تسلسل;

    error_رمز=U_ZERO_ERROR;
    RETURN_VOID_IF_BAD_RANGE(logicalPosition, 0, pBiDi->length, error_رمز);
    /* ubidi_countRuns will check VALID_PARA_OR_LINE */
    تسلسل_گنتی=ubidi_گنتی_Runs((UBiDi *)pBiDi, &error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    /* this is done based on runs rather than on levels since levels have
       a special interpretation when UBIDI_REORDER_RUNS_ONLY
     */
    visual_آغاز=logical_حد_بندی=0;
    i_تسلسل=pBiDi->runs[0];

    برائے(i=0; i<تسلسل_گنتی; i++) {
        i_تسلسل = pBiDi->runs[i];
        logical_پہلا=GET_اشاریہ(i_تسلسل.logical_آغاز);
        logical_حد_بندی=logical_پہلا+i_تسلسل.visual_حد_بندی-visual_آغاز;
        اگر((logicalPosition>=logical_پہلا) &&
           (logicalPosition<logical_حد_بندی)) {
            توڑو;
        }
        visual_آغاز = i_تسلسل.visual_حد_بندی;
    }
    اگر(p_Logical_حد_بندی) {
        *p_Logical_حد_بندی=logical_حد_بندی;
    }
    اگر(pLevel) {
        اگر(pBiDi->reordering_منوال==UBIDI_REORDER_RUNS_ONLY) {
            *pLevel=(UBiDiLevel)GET_ODD_ثنائی_ہندسہ(i_تسلسل.logical_آغاز);
        }
        ورنہ اگر(pBiDi->direction!=UBIDI_MIXED || logicalPosition>=pBiDi->trailingWSStart) {
            *pLevel=GET_PARALEVEL(pBiDi, logicalPosition);
        } ورنہ {
        *pLevel=pBiDi->سطحیں[logicalPosition];
        }
    }
}

/* runs API functions ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_گنتی_Runs(UBiDi *pBiDi, UError_رمز *p_Error_رمز) {
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_رمز, -1);
    RETURN_IF_NOT_VALID_PARA_OR_سطر(pBiDi, *p_Error_رمز, -1);
    ubidi_getRuns(pBiDi, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس -1;
    }
    واپس pBiDi->تسلسل_گنتی;
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_Visual_تسلسل(UBiDi *pBiDi, int32_t تسلسل_اشاریہ,
                   int32_t *p_Logical_آغاز, int32_t *p_لمبائی)
{
    int32_t آغاز;
    UError_رمز error_رمز = U_ZERO_ERROR;
    RETURN_IF_NOT_VALID_PARA_OR_سطر(pBiDi, error_رمز, UBIDI_LTR);
    ubidi_getRuns(pBiDi, &error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس UBIDI_LTR;
    }
    RETURN_IF_BAD_RANGE(تسلسل_اشاریہ, 0, pBiDi->تسلسل_گنتی, error_رمز, UBIDI_LTR);

    آغاز=pBiDi->runs[تسلسل_اشاریہ].logical_آغاز;
    اگر(p_Logical_آغاز!=خالی_اشاریہ) {
        *p_Logical_آغاز=GET_اشاریہ(آغاز);
    }
    اگر(p_لمبائی!=خالی_اشاریہ) {
        اگر(تسلسل_اشاریہ>0) {
            *p_لمبائی=pBiDi->runs[تسلسل_اشاریہ].visual_حد_بندی-
                     pBiDi->runs[تسلسل_اشاریہ-1].visual_حد_بندی;
        } ورنہ {
            *p_لمبائی=pBiDi->runs[0].visual_حد_بندی;
        }
    }
    واپس (UBiDiDirection)GET_ODD_ثنائی_ہندسہ(آغاز);
}

/* in trivial cases there is only one trivial run; called by ubidi_getRuns() */
ساکن خالی
get_Single_تسلسل(UBiDi *pBiDi, UBiDiLevel level) {
    /* simple, single-run case */
    pBiDi->runs=pBiDi->simpleRuns;
    pBiDi->تسلسل_گنتی=1;

    /* fill and reorder the single run */
    pBiDi->runs[0].logical_آغاز=MAKE_اشاریہ_ODD_PAIR(0, level);
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
ساکن خالی
reorder_سطر(UBiDi *pBiDi, UBiDiLevel minLevel, UBiDiLevel maxLevel) {
    تسلسل *runs, عارضی_تسلسل;
    UBiDiLevel *سطحیں;
    int32_t پہلا_تسلسل, انتہا_تسلسل, حد_بندی_تسلسل, تسلسل_گنتی;

    /* nothing to do? */
    اگر(maxLevel<=(minLevel|1)) {
        واپس;
    }

    /*
     * Reorder only down to the lowest odd level
     * and reorder at an odd minLevel in a separate, simpler loop.
     * See comments above for why minLevel is always incremented.
     */
    ++minLevel;

    runs=pBiDi->runs;
    سطحیں=pBiDi->سطحیں;
    تسلسل_گنتی=pBiDi->تسلسل_گنتی;

    /* do not include the WS run at paraLevel<=old minLevel except in the simple loop */
    اگر(pBiDi->trailingWSStart<pBiDi->length) {
        --تسلسل_گنتی;
    }

    جب(--maxLevel>=minLevel) {
        پہلا_تسلسل=0;

        /* loop for all sequences of runs */
        برائے(;;) {
            /* look for a sequence of runs that are all at >=maxLevel */
            /* look for the first run of such a sequence */
            جب(پہلا_تسلسل<تسلسل_گنتی && سطحیں[runs[پہلا_تسلسل].logical_آغاز]<maxLevel) {
                ++پہلا_تسلسل;
            }
            اگر(پہلا_تسلسل>=تسلسل_گنتی) {
                توڑو;  /* no more such runs */
            }

            /* look for the limit run of such a sequence (the run behind it) */
            برائے(حد_بندی_تسلسل=پہلا_تسلسل; ++حد_بندی_تسلسل<تسلسل_گنتی && سطحیں[runs[حد_بندی_تسلسل].logical_آغاز]>=maxLevel;) {}

            /* Swap the entire sequence of runs from firstRun to limitRun-1. */
            انتہا_تسلسل=حد_بندی_تسلسل-1;
            جب(پہلا_تسلسل<انتہا_تسلسل) {
                عارضی_تسلسل = runs[پہلا_تسلسل];
                runs[پہلا_تسلسل]=runs[انتہا_تسلسل];
                runs[انتہا_تسلسل]=عارضی_تسلسل;
                ++پہلا_تسلسل;
                --انتہا_تسلسل;
            }

            اگر(حد_بندی_تسلسل==تسلسل_گنتی) {
                توڑو;  /* no more such runs */
            } ورنہ {
                پہلا_تسلسل=حد_بندی_تسلسل+1;
            }
        }
    }

    /* now do maxLevel==old minLevel (==odd!), see above */
    اگر(!(minLevel&1)) {
        پہلا_تسلسل=0;

        /* include the trailing WS run in this complete reordering */
        اگر(pBiDi->trailingWSStart==pBiDi->length) {
            --تسلسل_گنتی;
        }

        /* Swap the entire sequence of all runs. (endRun==runCount) */
        جب(پہلا_تسلسل<تسلسل_گنتی) {
            عارضی_تسلسل=runs[پہلا_تسلسل];
            runs[پہلا_تسلسل]=runs[تسلسل_گنتی];
            runs[تسلسل_گنتی]=عارضی_تسلسل;
            ++پہلا_تسلسل;
            --تسلسل_گنتی;
        }
    }
}

/* compute the runs array --------------------------------------------------- */

ساکن int32_t get_تسلسل_From_Logical_اشاریہ(UBiDi *pBiDi, int32_t logical_اشاریہ) {
    تسلسل *runs=pBiDi->runs;
    int32_t تسلسل_گنتی=pBiDi->تسلسل_گنتی, visual_آغاز=0, i, length, logical_آغاز;

    برائے(i=0; i<تسلسل_گنتی; i++) {
        length=runs[i].visual_حد_بندی-visual_آغاز;
        logical_آغاز=GET_اشاریہ(runs[i].logical_آغاز);
        اگر((logical_اشاریہ>=logical_آغاز) && (logical_اشاریہ<(logical_آغاز+length))) {
            واپس i;
        }
        visual_آغاز+=length;
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
ubidi_getRuns(UBiDi *pBiDi, UError_رمز* p_Error_رمز) {
    /*
     * This method returns immediately if the runs are already set. This
     * includes the case of length==0 (handled in setPara)..
     */
    اگر (pBiDi->تسلسل_گنتی>=0) {
        واپس سچ;
    }

    اگر(pBiDi->direction!=UBIDI_MIXED) {
        /* simple, single-run case - this covers length==0 */
        /* pBiDi->paraLevel is ok even for contextual multiple paragraphs */
        get_Single_تسلسل(pBiDi, pBiDi->paraLevel);
    } ورنہ /* UBIDI_MIXED, length>0 */ {
        /* mixed directionality */
        int32_t length=pBiDi->length, حد_بندی;
        UBiDiLevel *سطحیں=pBiDi->سطحیں;
        int32_t i, تسلسل_گنتی;
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
        تسلسل_گنتی=0;
        برائے(i=0; i<حد_بندی; ++i) {
            /* increment runCount at the start of each run */
            اگر(سطحیں[i]!=level) {
                ++تسلسل_گنتی;
                level=سطحیں[i];
            }
        }

        /*
         * We don't need to see if the last run can be merged with a trailing
         * WS run because setTrailingWSStart() would have done that.
         */
        اگر(تسلسل_گنتی==1 && حد_بندی==length) {
            /* There is only one non-WS run and no trailing WS-run. */
            get_Single_تسلسل(pBiDi, سطحیں[0]);
        } ورنہ /* runCount>1 || limit<length */ {
            /* allocate and set the runs */
            تسلسل *runs;
            int32_t تسلسل_اشاریہ, آغاز;
            UBiDiLevel minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1, maxLevel=0;

            /* now, count a (non-mergeable) WS run */
            اگر(حد_بندی<length) {
                ++تسلسل_گنتی;
            }

            /* runCount>1 */
            اگر(get_Runs_حافظہ(pBiDi, تسلسل_گنتی)) {
                runs=pBiDi->runs_حافظہ;
            } ورنہ {
                *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
                واپس جھوٹ;
            }

            /* set the runs */
            /* FOOD FOR THOUGHT: this could be optimized, e.g.:
             * 464->444, 484->444, 575->555, 595->555
             * However, that would take longer. Check also how it would
             * interact with BiDi control removal and inserting Marks.
             */
            تسلسل_اشاریہ=0;

            /* search for the run limits and initialize visualLimit values with the run lengths */
            i=0;
            کرو {
                /* prepare this run */
                آغاز=i;
                level=سطحیں[i];
                اگر(level<minLevel) {
                    minLevel=level;
                }
                اگر(level>maxLevel) {
                    maxLevel=level;
                }

                /* look for the run limit */
                جب(++i<حد_بندی && سطحیں[i]==level) {}

                /* i is another run limit */
                runs[تسلسل_اشاریہ].logical_آغاز=آغاز;
                runs[تسلسل_اشاریہ].visual_حد_بندی=i-آغاز;
                runs[تسلسل_اشاریہ].داخل_کرو_Remove=0;
                ++تسلسل_اشاریہ;
            } جب(i<حد_بندی);

            اگر(حد_بندی<length) {
                /* there is a separate WS run */
                runs[تسلسل_اشاریہ].logical_آغاز=حد_بندی;
                runs[تسلسل_اشاریہ].visual_حد_بندی=length-حد_بندی;
                /* For the trailing WS run, pBiDi->paraLevel is ok even
                   if contextual multiple paragraphs.                   */
                اگر(pBiDi->paraLevel<minLevel) {
                    minLevel=pBiDi->paraLevel;
                }
            }

            /* set the object fields */
            pBiDi->runs=runs;
            pBiDi->تسلسل_گنتی=تسلسل_گنتی;

            reorder_سطر(pBiDi, minLevel, maxLevel);

            /* now add the direction flags and adjust the visualLimit's to be just that */
            /* this loop will also handle the trailing WS run */
            حد_بندی=0;
            برائے(i=0; i<تسلسل_گنتی; ++i) {
                جوڑو_ODD_ثنائی_ہندسہ_FROM_LEVEL(runs[i].logical_آغاز, سطحیں[runs[i].logical_آغاز]);
                حد_بندی+=runs[i].visual_حد_بندی;
                runs[i].visual_حد_بندی=حد_بندی;
            }

            /* Set the "odd" bit for the trailing WS run. */
            /* For a RTL paragraph, it will be the *first* run in visual order. */
            /* For the trailing WS run, pBiDi->paraLevel is ok even if
               contextual multiple paragraphs.                          */
            اگر(تسلسل_اشاریہ<تسلسل_گنتی) {
                int32_t trailing_تسلسل = ((pBiDi->paraLevel & 1) != 0)? 0 : تسلسل_اشاریہ;

                جوڑو_ODD_ثنائی_ہندسہ_FROM_LEVEL(runs[trailing_تسلسل].logical_آغاز, pBiDi->paraLevel);
            }
        }
    }

    /* handle insert LRM/RLM BEFORE/AFTER run */
    اگر(pBiDi->داخل_کرو_Points.size>0) {
        نقطہ *نقطہ_2, *آغاز=pBiDi->داخل_کرو_Points.points,
                      *حد_بندی=آغاز+pBiDi->داخل_کرو_Points.size;
        int32_t تسلسل_اشاریہ;
        برائے(نقطہ_2=آغاز; نقطہ_2<حد_بندی; نقطہ_2++) {
            تسلسل_اشاریہ=get_تسلسل_From_Logical_اشاریہ(pBiDi, نقطہ_2->pos);
            pBiDi->runs[تسلسل_اشاریہ].داخل_کرو_Remove|=نقطہ_2->flag;
        }
    }

    /* handle remove BiDi control characters */
    اگر(pBiDi->control_گنتی>0) {
        int32_t تسلسل_اشاریہ;
        ثابت char16_t *آغاز=pBiDi->متن, *حد_بندی=آغاز+pBiDi->length, *pu;
        برائے(pu=آغاز; pu<حد_بندی; pu++) {
            اگر(IS_BIDI_CONTROL_CHAR(*pu)) {
                تسلسل_اشاریہ=get_تسلسل_From_Logical_اشاریہ(pBiDi, (int32_t)(pu-آغاز));
                pBiDi->runs[تسلسل_اشاریہ].داخل_کرو_Remove--;
            }
        }
    }

    واپس سچ;
}

ساکن UBool
prepareReorder(ثابت UBiDiLevel *سطحیں, int32_t length,
               int32_t *اشاریہ_Map,
               UBiDiLevel *pMinLevel, UBiDiLevel *pMaxLevel) {
    int32_t آغاز;
    UBiDiLevel level, minLevel, maxLevel;

    اگر(سطحیں==خالی_اشاریہ || length<=0) {
        واپس جھوٹ;
    }

    /* determine minLevel and maxLevel */
    minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1;
    maxLevel=0;
    برائے(آغاز=length; آغاز>0;) {
        level=سطحیں[--آغاز];
        اگر(level>UBIDI_MAX_EXPLICIT_LEVEL+1) {
            واپس جھوٹ;
        }
        اگر(level<minLevel) {
            minLevel=level;
        }
        اگر(level>maxLevel) {
            maxLevel=level;
        }
    }
    *pMinLevel=minLevel;
    *pMaxLevel=maxLevel;

    /* initialize the index map */
    برائے(آغاز=length; آغاز>0;) {
        --آغاز;
        اشاریہ_Map[آغاز]=آغاز;
    }

    واپس سچ;
}

/* reorder a line based on a levels array (L2) ------------------------------ */

U_CAPI خالی U_EXPORT2
ubidi_reorderLogical(ثابت UBiDiLevel *سطحیں, int32_t length, int32_t *اشاریہ_Map) {
    int32_t آغاز, حد_بندی, مجموع_کا_Sos_Eos;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    اگر(اشاریہ_Map==خالی_اشاریہ || !prepareReorder(سطحیں, length, اشاریہ_Map, &minLevel, &maxLevel)) {
        واپس;
    }

    /* nothing to do? */
    اگر(minLevel==maxLevel && (minLevel&1)==0) {
        واپس;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    کرو {
        آغاز=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        برائے(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            جب(آغاز<length && سطحیں[آغاز]<maxLevel) {
                ++آغاز;
            }
            اگر(آغاز>=length) {
                توڑو;  /* no more such sequences */
            }

            /* look for the limit of such a sequence (the index behind it) */
            برائے(حد_بندی=آغاز; ++حد_بندی<length && سطحیں[حد_بندی]>=maxLevel;) {}

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
            مجموع_کا_Sos_Eos=آغاز+حد_بندی-1;

            /* reorder each index in the sequence */
            کرو {
                اشاریہ_Map[آغاز]=مجموع_کا_Sos_Eos-اشاریہ_Map[آغاز];
            } جب(++آغاز<حد_بندی);

            /* start==limit */
            اگر(حد_بندی==length) {
                توڑو;  /* no more such sequences */
            } ورنہ {
                آغاز=حد_بندی+1;
            }
        }
    } جب(--maxLevel>=minLevel);
}

U_CAPI خالی U_EXPORT2
ubidi_reorderVisual(ثابت UBiDiLevel *سطحیں, int32_t length, int32_t *اشاریہ_Map) {
    int32_t آغاز, انتہا, حد_بندی, عارضی;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    اگر(اشاریہ_Map==خالی_اشاریہ || !prepareReorder(سطحیں, length, اشاریہ_Map, &minLevel, &maxLevel)) {
        واپس;
    }

    /* nothing to do? */
    اگر(minLevel==maxLevel && (minLevel&1)==0) {
        واپس;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    کرو {
        آغاز=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        برائے(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            جب(آغاز<length && سطحیں[آغاز]<maxLevel) {
                ++آغاز;
            }
            اگر(آغاز>=length) {
                توڑو;  /* no more such runs */
            }

            /* look for the limit of such a sequence (the index behind it) */
            برائے(حد_بندی=آغاز; ++حد_بندی<length && سطحیں[حد_بندی]>=maxLevel;) {}

            /*
             * Swap the entire interval of indexes from start to limit-1.
             * We don't need to swap the levels for the purpose of this
             * algorithm: the sequence of levels that we look at does not
             * move anyway.
             */
            انتہا=حد_بندی-1;
            جب(آغاز<انتہا) {
                عارضی=اشاریہ_Map[آغاز];
                اشاریہ_Map[آغاز]=اشاریہ_Map[انتہا];
                اشاریہ_Map[انتہا]=عارضی;

                ++آغاز;
                --انتہا;
            }

            اگر(حد_بندی==length) {
                توڑو;  /* no more such sequences */
            } ورنہ {
                آغاز=حد_بندی+1;
            }
        }
    } جب(--maxLevel>=minLevel);
}

/* API functions for logical<->visual mapping ------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_get_Visual_اشاریہ(UBiDi *pBiDi, int32_t logical_اشاریہ, UError_رمز *p_Error_رمز) {
    int32_t visual_اشاریہ=UBIDI_MAP_NOWHERE;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_رمز, -1);
    RETURN_IF_NOT_VALID_PARA_OR_سطر(pBiDi, *p_Error_رمز, -1);
    RETURN_IF_BAD_RANGE(logical_اشاریہ, 0, pBiDi->length, *p_Error_رمز, -1);

    /* we can do the trivial cases without the runs array */
    انتخاب(pBiDi->direction) {
    حالت UBIDI_LTR:
        visual_اشاریہ=logical_اشاریہ;
        توڑو;
    حالت UBIDI_RTL:
        visual_اشاریہ=pBiDi->length-logical_اشاریہ-1;
        توڑو;
    طےشدہ:
        اگر(!ubidi_getRuns(pBiDi, p_Error_رمز)) {
            *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس -1;
        } ورنہ {
            تسلسل *runs=pBiDi->runs;
            int32_t i, visual_آغاز=0, انحراف, length;

            /* linear search for the run, search on the visual runs */
            برائے(i=0; i<pBiDi->تسلسل_گنتی; ++i) {
                length=runs[i].visual_حد_بندی-visual_آغاز;
                انحراف=logical_اشاریہ-GET_اشاریہ(runs[i].logical_آغاز);
                اگر(انحراف>=0 && انحراف<length) {
                    اگر(IS_EVEN_تسلسل(runs[i].logical_آغاز)) {
                        /* LTR */
                        visual_اشاریہ=visual_آغاز+انحراف;
                    } ورنہ {
                        /* RTL */
                        visual_اشاریہ=visual_آغاز+length-انحراف-1;
                    }
                    توڑو;          /* exit for loop */
                }
                visual_آغاز+=length;
            }
            اگر(i>=pBiDi->تسلسل_گنتی) {
                واپس UBIDI_MAP_NOWHERE;
            }
        }
    }

    اگر(pBiDi->داخل_کرو_Points.size>0) {
        /* add the number of added marks until the calculated visual index */
        تسلسل *runs=pBiDi->runs;
        int32_t i, length, داخل_کرو_Remove;
        int32_t visual_آغاز=0, mark_ملا=0;
        برائے(i=0; ; i++, visual_آغاز+=length) {
            length=runs[i].visual_حد_بندی-visual_آغاز;
            داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
            اگر(داخل_کرو_Remove & (LRM_BEFORE|RLM_BEFORE)) {
                mark_ملا++;
            }
            /* is it the run containing the visual index? */
            اگر(visual_اشاریہ<runs[i].visual_حد_بندی) {
                واپس visual_اشاریہ+mark_ملا;
            }
            اگر(داخل_کرو_Remove & (LRM_AFTER|RLM_AFTER)) {
                mark_ملا++;
            }
        }
    }
    ورنہ اگر(pBiDi->control_گنتی>0) {
        /* subtract the number of controls until the calculated visual index */
        تسلسل *runs=pBiDi->runs;
        int32_t i, j, آغاز, حد_بندی, length, داخل_کرو_Remove;
        int32_t visual_آغاز=0, control_ملا=0;
        char16_t uchar=pBiDi->متن[logical_اشاریہ];
        /* is the logical index pointing to a control ? */
        اگر(IS_BIDI_CONTROL_CHAR(uchar)) {
            واپس UBIDI_MAP_NOWHERE;
        }
        /* loop on runs */
        برائے(i=0; ; i++, visual_آغاز+=length) {
            length=runs[i].visual_حد_بندی-visual_آغاز;
            داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
            /* calculated visual index is beyond this run? */
            اگر(visual_اشاریہ>=runs[i].visual_حد_بندی) {
                control_ملا-=داخل_کرو_Remove;
                جاری;
            }
            /* calculated visual index must be within current run */
            اگر(داخل_کرو_Remove==0) {
                واپس visual_اشاریہ-control_ملا;
            }
            اگر(IS_EVEN_تسلسل(runs[i].logical_آغاز)) {
                /* LTR: check from run start to logical index */
                آغاز=runs[i].logical_آغاز;
                حد_بندی=logical_اشاریہ;
            } ورنہ {
                /* RTL: check from logical index to run end */
                آغاز=logical_اشاریہ+1;
                حد_بندی=GET_اشاریہ(runs[i].logical_آغاز)+length;
            }
            برائے(j=آغاز; j<حد_بندی; j++) {
                uchar=pBiDi->متن[j];
                اگر(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_ملا++;
                }
            }
            واپس visual_اشاریہ-control_ملا;
        }
    }

    واپس visual_اشاریہ;
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Logical_اشاریہ(UBiDi *pBiDi, int32_t visual_اشاریہ, UError_رمز *p_Error_رمز) {
    تسلسل *runs;
    int32_t i, تسلسل_گنتی, آغاز;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_رمز, -1);
    RETURN_IF_NOT_VALID_PARA_OR_سطر(pBiDi, *p_Error_رمز, -1);
    RETURN_IF_BAD_RANGE(visual_اشاریہ, 0, pBiDi->نتیجہ_لمبائی, *p_Error_رمز, -1);
    /* we can do the trivial cases without the runs array */
    اگر(pBiDi->داخل_کرو_Points.size==0 && pBiDi->control_گنتی==0) {
        اگر(pBiDi->direction==UBIDI_LTR) {
            واپس visual_اشاریہ;
        }
        ورنہ اگر(pBiDi->direction==UBIDI_RTL) {
            واپس pBiDi->length-visual_اشاریہ-1;
        }
    }
    اگر(!ubidi_getRuns(pBiDi, p_Error_رمز)) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس -1;
    }

    runs=pBiDi->runs;
    تسلسل_گنتی=pBiDi->تسلسل_گنتی;
    اگر(pBiDi->داخل_کرو_Points.size>0) {
        /* handle inserted LRM/RLM */
        int32_t mark_ملا=0, داخل_کرو_Remove;
        int32_t visual_آغاز=0, length;
        runs=pBiDi->runs;
        /* subtract number of marks until visual index */
        برائے(i=0; ; i++, visual_آغاز+=length) {
            length=runs[i].visual_حد_بندی-visual_آغاز;
            داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
            اگر(داخل_کرو_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                اگر(visual_اشاریہ<=(visual_آغاز+mark_ملا)) {
                    واپس UBIDI_MAP_NOWHERE;
                }
                mark_ملا++;
            }
            /* is adjusted visual index within this run? */
            اگر(visual_اشاریہ<(runs[i].visual_حد_بندی+mark_ملا)) {
                visual_اشاریہ-=mark_ملا;
                توڑو;
            }
            اگر(داخل_کرو_Remove&(LRM_AFTER|RLM_AFTER)) {
                اگر(visual_اشاریہ==(visual_آغاز+length+mark_ملا)) {
                    واپس UBIDI_MAP_NOWHERE;
                }
                mark_ملا++;
            }
        }
    }
    ورنہ اگر(pBiDi->control_گنتی>0) {
        /* handle removed BiDi control characters */
        int32_t control_ملا=0, داخل_کرو_Remove, length;
        int32_t logical_آغاز, logical_انتہا, visual_آغاز=0, j, k;
        char16_t uchar;
        UBool even_تسلسل;
        /* add number of controls until visual index */
        برائے(i=0; ; i++, visual_آغاز+=length) {
            length=runs[i].visual_حد_بندی-visual_آغاز;
            داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
            /* is adjusted visual index beyond current run? */
            اگر(visual_اشاریہ>=(runs[i].visual_حد_بندی-control_ملا+داخل_کرو_Remove)) {
                control_ملا-=داخل_کرو_Remove;
                جاری;
            }
            /* adjusted visual index is within current run */
            اگر(داخل_کرو_Remove==0) {
                visual_اشاریہ+=control_ملا;
                توڑو;
            }
            /* count non-control chars until visualIndex */
            logical_آغاز=runs[i].logical_آغاز;
            even_تسلسل=IS_EVEN_تسلسل(logical_آغاز);
            REMOVE_ODD_ثنائی_ہندسہ(logical_آغاز);
            logical_انتہا=logical_آغاز+length-1;
            برائے(j=0; j<length; j++) {
                k= even_تسلسل ? logical_آغاز+j : logical_انتہا-j;
                uchar=pBiDi->متن[k];
                اگر(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_ملا++;
                }
                اگر((visual_اشاریہ+control_ملا)==(visual_آغاز+j)) {
                    توڑو;
                }
            }
            visual_اشاریہ+=control_ملا;
            توڑو;
        }
    }
    /* handle all cases */
    اگر(تسلسل_گنتی<=10) {
        /* linear search for the run */
        برائے(i=0; visual_اشاریہ>=runs[i].visual_حد_بندی; ++i) {}
    } ورنہ {
        /* binary search for the run */
        int32_t begin=0, حد_بندی=تسلسل_گنتی;

        /* the middle if() is guaranteed to find the run, we don't need a loop limit */
        برائے(;;) {
            i=(begin+حد_بندی)/2;
            اگر(visual_اشاریہ>=runs[i].visual_حد_بندی) {
                begin=i+1;
            } ورنہ اگر(i==0 || visual_اشاریہ>=runs[i-1].visual_حد_بندی) {
                توڑو;
            } ورنہ {
                حد_بندی=i;
            }
        }
    }

    آغاز=runs[i].logical_آغاز;
    اگر(IS_EVEN_تسلسل(آغاز)) {
        /* LTR */
        /* the offset in runs[i] is visualIndex-runs[i-1].visualLimit */
        اگر(i>0) {
            visual_اشاریہ-=runs[i-1].visual_حد_بندی;
        }
        واپس آغاز+visual_اشاریہ;
    } ورنہ {
        /* RTL */
        واپس GET_اشاریہ(آغاز)+runs[i].visual_حد_بندی-visual_اشاریہ-1;
    }
}

U_CAPI خالی U_EXPORT2
ubidi_getLogicalMap(UBiDi *pBiDi, int32_t *اشاریہ_Map, UError_رمز *p_Error_رمز) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_رمز);
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_گنتی_Runs(pBiDi, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        /* no op */
    } ورنہ اگر(اشاریہ_Map==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
    } ورنہ {
        /* fill a logical-to-visual index map using the runs[] */
        int32_t visual_آغاز, visual_حد_بندی, i, j, k;
        int32_t logical_آغاز, logical_حد_بندی;
        تسلسل *runs=pBiDi->runs;
        اگر (pBiDi->length<=0) {
            واپس;
        }
        اگر (pBiDi->length>pBiDi->نتیجہ_لمبائی) {
            uprv_memset(اشاریہ_Map, 0xFF, pBiDi->length*حجم(int32_t));
        }

        visual_آغاز=0;
        برائے(j=0; j<pBiDi->تسلسل_گنتی; ++j) {
            logical_آغاز=GET_اشاریہ(runs[j].logical_آغاز);
            visual_حد_بندی=runs[j].visual_حد_بندی;
            اگر(IS_EVEN_تسلسل(runs[j].logical_آغاز)) {
                کرو { /* LTR */
                    اشاریہ_Map[logical_آغاز++]=visual_آغاز++;
                } جب(visual_آغاز<visual_حد_بندی);
            } ورنہ {
                logical_آغاز+=visual_حد_بندی-visual_آغاز;  /* logicalLimit */
                کرو { /* RTL */
                    اشاریہ_Map[--logical_آغاز]=visual_آغاز++;
                } جب(visual_آغاز<visual_حد_بندی);
            }
            /* visualStart==visualLimit; */
        }

        اگر(pBiDi->داخل_کرو_Points.size>0) {
            int32_t mark_ملا=0, تسلسل_گنتی=pBiDi->تسلسل_گنتی;
            int32_t length, داخل_کرو_Remove;
            visual_آغاز=0;
            /* add number of marks found until each index */
            برائے(i=0; i<تسلسل_گنتی; i++, visual_آغاز+=length) {
                length=runs[i].visual_حد_بندی-visual_آغاز;
                داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
                اگر(داخل_کرو_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_ملا++;
                }
                اگر(mark_ملا>0) {
                    logical_آغاز=GET_اشاریہ(runs[i].logical_آغاز);
                    logical_حد_بندی=logical_آغاز+length;
                    برائے(j=logical_آغاز; j<logical_حد_بندی; j++) {
                        اشاریہ_Map[j]+=mark_ملا;
                    }
                }
                اگر(داخل_کرو_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_ملا++;
                }
            }
        }
        ورنہ اگر(pBiDi->control_گنتی>0) {
            int32_t control_ملا=0, تسلسل_گنتی=pBiDi->تسلسل_گنتی;
            int32_t length, داخل_کرو_Remove;
            UBool even_تسلسل;
            char16_t uchar;
            visual_آغاز=0;
            /* subtract number of controls found until each index */
            برائے(i=0; i<تسلسل_گنتی; i++, visual_آغاز+=length) {
                length=runs[i].visual_حد_بندی-visual_آغاز;
                داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
                /* no control found within previous runs nor within this run */
                اگر((control_ملا-داخل_کرو_Remove)==0) {
                    جاری;
                }
                logical_آغاز=runs[i].logical_آغاز;
                even_تسلسل=IS_EVEN_تسلسل(logical_آغاز);
                REMOVE_ODD_ثنائی_ہندسہ(logical_آغاز);
                logical_حد_بندی=logical_آغاز+length;
                /* if no control within this run */
                اگر(داخل_کرو_Remove==0) {
                    برائے(j=logical_آغاز; j<logical_حد_بندی; j++) {
                        اشاریہ_Map[j]-=control_ملا;
                    }
                    جاری;
                }
                برائے(j=0; j<length; j++) {
                    k= even_تسلسل ? logical_آغاز+j : logical_حد_بندی-j-1;
                    uchar=pBiDi->متن[k];
                    اگر(IS_BIDI_CONTROL_CHAR(uchar)) {
                        control_ملا++;
                        اشاریہ_Map[k]=UBIDI_MAP_NOWHERE;
                        جاری;
                    }
                    اشاریہ_Map[k]-=control_ملا;
                }
            }
        }
    }
}

U_CAPI خالی U_EXPORT2
ubidi_getVisualMap(UBiDi *pBiDi, int32_t *اشاریہ_Map, UError_رمز *p_Error_رمز) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_رمز);
    اگر(اشاریہ_Map==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_گنتی_Runs(pBiDi, p_Error_رمز);
    اگر(U_SUCCESS(*p_Error_رمز)) {
        /* fill a visual-to-logical index map using the runs[] */
        تسلسل *runs=pBiDi->runs, *runs_حد_بندی=runs+pBiDi->تسلسل_گنتی;
        int32_t logical_آغاز, visual_آغاز, visual_حد_بندی, *pi=اشاریہ_Map;

        اگر (pBiDi->نتیجہ_لمبائی<=0) {
            واپس;
        }
        visual_آغاز=0;
        برائے(; runs<runs_حد_بندی; ++runs) {
            logical_آغاز=runs->logical_آغاز;
            visual_حد_بندی=runs->visual_حد_بندی;
            اگر(IS_EVEN_تسلسل(logical_آغاز)) {
                کرو { /* LTR */
                    *pi++ = logical_آغاز++;
                } جب(++visual_آغاز<visual_حد_بندی);
            } ورنہ {
                REMOVE_ODD_ثنائی_ہندسہ(logical_آغاز);
                logical_آغاز+=visual_حد_بندی-visual_آغاز;  /* logicalLimit */
                کرو { /* RTL */
                    *pi++ = --logical_آغاز;
                } جب(++visual_آغاز<visual_حد_بندی);
            }
            /* visualStart==visualLimit; */
        }

        اگر(pBiDi->داخل_کرو_Points.size>0) {
            int32_t mark_ملا=0, تسلسل_گنتی=pBiDi->تسلسل_گنتی;
            int32_t داخل_کرو_Remove, i, j, k;
            runs=pBiDi->runs;
            /* count all inserted marks */
            برائے(i=0; i<تسلسل_گنتی; i++) {
                داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
                اگر(داخل_کرو_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_ملا++;
                }
                اگر(داخل_کرو_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_ملا++;
                }
            }
            /* move back indexes by number of preceding marks */
            k=pBiDi->نتیجہ_لمبائی;
            برائے(i=تسلسل_گنتی-1; i>=0 && mark_ملا>0; i--) {
                داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
                اگر(داخل_کرو_Remove&(LRM_AFTER|RLM_AFTER)) {
                    اشاریہ_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_ملا--;
                }
                visual_آغاز= i>0 ? runs[i-1].visual_حد_بندی : 0;
                برائے(j=runs[i].visual_حد_بندی-1; j>=visual_آغاز && mark_ملا>0; j--) {
                    اشاریہ_Map[--k]=اشاریہ_Map[j];
                }
                اگر(داخل_کرو_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    اشاریہ_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_ملا--;
                }
            }
        }
        ورنہ اگر(pBiDi->control_گنتی>0) {
            int32_t تسلسل_گنتی=pBiDi->تسلسل_گنتی, logical_انتہا;
            int32_t داخل_کرو_Remove, length, i, j, k, m;
            char16_t uchar;
            UBool even_تسلسل;
            runs=pBiDi->runs;
            visual_آغاز=0;
            /* move forward indexes by number of preceding controls */
            k=0;
            برائے(i=0; i<تسلسل_گنتی; i++, visual_آغاز+=length) {
                length=runs[i].visual_حد_بندی-visual_آغاز;
                داخل_کرو_Remove=runs[i].داخل_کرو_Remove;
                /* if no control found yet, nothing to do in this run */
                اگر((داخل_کرو_Remove==0)&&(k==visual_آغاز)) {
                    k+=length;
                    جاری;
                }
                /* if no control in this run */
                اگر(داخل_کرو_Remove==0) {
                    visual_حد_بندی=runs[i].visual_حد_بندی;
                    برائے(j=visual_آغاز; j<visual_حد_بندی; j++) {
                        اشاریہ_Map[k++]=اشاریہ_Map[j];
                    }
                    جاری;
                }
                logical_آغاز=runs[i].logical_آغاز;
                even_تسلسل=IS_EVEN_تسلسل(logical_آغاز);
                REMOVE_ODD_ثنائی_ہندسہ(logical_آغاز);
                logical_انتہا=logical_آغاز+length-1;
                برائے(j=0; j<length; j++) {
                    m= even_تسلسل ? logical_آغاز+j : logical_انتہا-j;
                    uchar=pBiDi->متن[m];
                    اگر(!IS_BIDI_CONTROL_CHAR(uchar)) {
                        اشاریہ_Map[k++]=m;
                    }
                }
            }
        }
    }
}

U_CAPI خالی U_EXPORT2
ubidi_invertMap(ثابت int32_t *srcMap, int32_t *destMap, int32_t length) {
    اگر(srcMap!=خالی_اشاریہ && destMap!=خالی_اشاریہ && length>0) {
        ثابت int32_t *pi;
        int32_t dest_لمبائی=-1, count=0;
        /* find highest value and count positive indexes in srcMap */
        pi=srcMap+length;
        جب(pi>srcMap) {
            اگر(*--pi>dest_لمبائی) {
                dest_لمبائی=*pi;
            }
            اگر(*pi>=0) {
                count++;
            }
        }
        dest_لمبائی++;           /* add 1 for origin 0 */
        اگر(count<dest_لمبائی) {
            /* we must fill unmatched destMap entries with -1 */
            uprv_memset(destMap, 0xFF, dest_لمبائی*حجم(int32_t));
        }
        pi=srcMap+length;
        جب(length>0) {
            اگر(*--pi>=0) {
                destMap[*pi]=--length;
            } ورنہ {
                --length;
            }
        }
    }
}
