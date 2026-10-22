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
setTrailingWSStart(UBiDi *pBiDi) {
    /* pBiDi->direction!=UBIDI_MIXED */

    ثابت DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *levels=pBiDi->levels;
    int32_t start=pBiDi->length;
    UBiDiLevel paraLevel=pBiDi->paraLevel;

    /* If the line is terminated by a block separator, all preceding WS etc...
       are already set to paragraph level.
       Setting trailingWSStart to pBidi->length will avoid changing the
       level of B chars from 0 to paraLevel in ubidi_getLevels when
       orderParagraphsLTR==true.
     */
    إذا(dirProps[start-1]==B) {
        pBiDi->trailingWSStart=start;   /* currently == pBiDi->length */
        أرجع;
    }
    /* go backwards across all WS, BN, explicit codes */
    طالما(start>0 && DIRPROP_FLAG(dirProps[start-1])&MASK_WS) {
        --start;
    }

    /* if the WS run can be merged with the previous run then do so here */
    طالما(start>0 && levels[start-1]==paraLevel) {
        --start;
    }

    pBiDi->trailingWSStart=start;
}

/* ubidi_setLine ------------------------------------------------------------ */

U_CAPI فراغ U_EXPORT2
ubidi_set_لي(ثابت UBiDi *pParaBiDi,
              int32_t start, int32_t limit,
              UBiDi *p_لي_Bi_Di,
              UErrorCode *pErrorCode) {
    int32_t length;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    RETURN_VOID_IF_NOT_VALID_PARA(pParaBiDi, *pErrorCode);
    RETURN_VOID_IF_BAD_RANGE(start, 0, limit, *pErrorCode);
    RETURN_VOID_IF_BAD_RANGE(limit, 0, pParaBiDi->length+1, *pErrorCode);
    إذا(p_لي_Bi_Di==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    إذا(ubidi_getParagraph(pParaBiDi, start, nullptr, nullptr, nullptr, pErrorCode) !=
       ubidi_getParagraph(pParaBiDi, limit-1, nullptr, nullptr, nullptr, pErrorCode)) {
        /* the line crosses a paragraph boundary */
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }

    /* set the values in pLineBiDi from its pParaBiDi parent */
    p_لي_Bi_Di->pParaBiDi=nullptr;          /* mark unfinished setLine */
    p_لي_Bi_Di->text=pParaBiDi->text+start;
    length=p_لي_Bi_Di->length=limit-start;
    p_لي_Bi_Di->resultLength=p_لي_Bi_Di->originalLength=length;
    p_لي_Bi_Di->paraLevel=GET_PARALEVEL(pParaBiDi, start);
    p_لي_Bi_Di->para_كدي=pParaBiDi->para_كدي;
    p_لي_Bi_Di->runs=nullptr;
    p_لي_Bi_Di->flags=0;
    p_لي_Bi_Di->reorderingMode=pParaBiDi->reorderingMode;
    p_لي_Bi_Di->reorderingOptions=pParaBiDi->reorderingOptions;
    p_لي_Bi_Di->control_كدي=0;
    إذا(pParaBiDi->control_كدي>0) {
        int32_t j;
        لكل(j=start; j<limit; j++) {
            إذا(IS_BIDI_CONTROL_CHAR(pParaBiDi->text[j])) {
                p_لي_Bi_Di->control_كدي++;
            }
        }
        p_لي_Bi_Di->resultLength-=p_لي_Bi_Di->control_كدي;
    }

    p_لي_Bi_Di->dirProps=pParaBiDi->dirProps+start;
    p_لي_Bi_Di->levels=pParaBiDi->levels+start;
    p_لي_Bi_Di->run_كدي=-1;

    إذا(pParaBiDi->direction!=UBIDI_MIXED) {
        /* the parent is already trivial */
        p_لي_Bi_Di->direction=pParaBiDi->direction;

        /*
         * The parent's levels are all either
         * implicitly or explicitly ==paraLevel;
         * do the same here.
         */
        إذا(pParaBiDi->trailingWSStart<=start) {
            p_لي_Bi_Di->trailingWSStart=0;
        } وإلا إذا(pParaBiDi->trailingWSStart<limit) {
            p_لي_Bi_Di->trailingWSStart=pParaBiDi->trailingWSStart-start;
        } وإلا {
            p_لي_Bi_Di->trailingWSStart=length;
        }
    } وإلا {
        ثابت UBiDiLevel *levels=p_لي_Bi_Di->levels;
        int32_t i, trailingWSStart;
        UBiDiLevel level;

        setTrailingWSStart(p_لي_Bi_Di);
        trailingWSStart=p_لي_Bi_Di->trailingWSStart;

        /* recalculate pLineBiDi->direction */
        إذا(trailingWSStart==0) {
            /* all levels are at paraLevel */
            p_لي_Bi_Di->direction=(UBiDiDirection)(p_لي_Bi_Di->paraLevel&1);
        } وإلا {
            /* get the level of the first character */
            level=(UBiDiLevel)(levels[0]&1);

            /* if there is anything of a different level, then the line is mixed */
            إذا(trailingWSStart<length && (p_لي_Bi_Di->paraLevel&1)!=level) {
                /* the trailing WS is at paraLevel, which differs from levels[0] */
                p_لي_Bi_Di->direction=UBIDI_MIXED;
            } وإلا {
                /* see if levels[1..trailingWSStart-1] have the same direction as levels[0] and paraLevel */
                i=1;
                لكل(;;) {
                    إذا(i==trailingWSStart) {
                        /* the direction values match those in level */
                        p_لي_Bi_Di->direction=(UBiDiDirection)level;
                        اكسر;
                    } وإلا إذا((levels[i]&1)!=level) {
                        p_لي_Bi_Di->direction=UBIDI_MIXED;
                        اكسر;
                    }
                    ++i;
                }
            }
        }

        بدل(p_لي_Bi_Di->direction) {
        حالة UBIDI_LTR:
            /* make sure paraLevel is even */
            p_لي_Bi_Di->paraLevel=(UBiDiLevel)((p_لي_Bi_Di->paraLevel+1)&~1);

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_لي_Bi_Di->trailingWSStart=0;
            اكسر;
        حالة UBIDI_RTL:
            /* make sure paraLevel is odd */
            p_لي_Bi_Di->paraLevel|=1;

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_لي_Bi_Di->trailingWSStart=0;
            اكسر;
        افتراضي:
            اكسر;
        }
    }
    p_لي_Bi_Di->pParaBiDi=pParaBiDi;     /* mark successful setLine */
}

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getLevelAt(ثابت UBiDi *pBiDi, int32_t charIndex) {
    /* return paraLevel if in the trailing WS run, otherwise the real level */
    إذا(!IS_VALID_PARA_OR_لي(pBiDi) || charIndex<0 || pBiDi->length<=charIndex) {
        أرجع 0;
    } وإلا إذا(pBiDi->direction!=UBIDI_MIXED || charIndex>=pBiDi->trailingWSStart) {
        أرجع GET_PARALEVEL(pBiDi, charIndex);
    } وإلا {
        أرجع pBiDi->levels[charIndex];
    }
}

U_CAPI ثابت UBiDiLevel* U_EXPORT2
ubidi_getLevels(UBiDi* pBiDi, UErrorCode* pErrorCode) {
    int32_t start, length;

    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, nullptr);
    RETURN_IF_NOT_VALID_PARA_OR_لي(pBiDi, *pErrorCode, nullptr);
    إذا((length=pBiDi->length)<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    إذا((start=pBiDi->trailingWSStart)==length) {
        /* the current levels array reflects the WS run */
        أرجع pBiDi->levels;
    }

    /*
     * After the previous if(), we know that the levels array
     * has an implicit trailing WS run and therefore does not fully
     * reflect itself all the levels.
     * This must be a UBiDi object for a line, and
     * we need to create a new levels array.
     */
    إذا(getLevelsMemory(pBiDi, length)) {
        UBiDiLevel *levels=pBiDi->levelsMemory;

        إذا(start>0 && levels!=pBiDi->levels) {
            uprv_memcpy(levels, pBiDi->levels, start);
        }
        /* pBiDi->paraLevel is ok even if contextual multiple paragraphs,
           since pBidi is a line object                                     */
        uprv_memset(levels+start, pBiDi->paraLevel, length-start);

        /* this new levels array is set for the line and reflects the WS run */
        pBiDi->trailingWSStart=length;
        أرجع pBiDi->levels=levels;
    } وإلا {
        /* out of memory */
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_getLogicalRun(ثابت UBiDi *pBiDi, int32_t logicalPosition,
                    int32_t *pLogicalLimit, UBiDiLevel *pLevel) {
    UErrorCode errorCode;
    int32_t run_كدي, visualStart, logicalLimit, logicalFirst, i;
    Run iRun;

    errorCode=U_ZERO_ERROR;
    RETURN_VOID_IF_BAD_RANGE(logicalPosition, 0, pBiDi->length, errorCode);
    /* ubidi_countRuns will check VALID_PARA_OR_LINE */
    run_كدي=ubidi_كدي_Runs((UBiDi *)pBiDi, &errorCode);
    إذا(U_FAILURE(errorCode)) {
        أرجع;
    }
    /* this is done based on runs rather than on levels since levels have
       a special interpretation when UBIDI_REORDER_RUNS_ONLY
     */
    visualStart=logicalLimit=0;
    iRun=pBiDi->runs[0];

    لكل(i=0; i<run_كدي; i++) {
        iRun = pBiDi->runs[i];
        logicalFirst=GET_INDEX(iRun.logicalStart);
        logicalLimit=logicalFirst+iRun.visualLimit-visualStart;
        إذا((logicalPosition>=logicalFirst) &&
           (logicalPosition<logicalLimit)) {
            اكسر;
        }
        visualStart = iRun.visualLimit;
    }
    إذا(pLogicalLimit) {
        *pLogicalLimit=logicalLimit;
    }
    إذا(pLevel) {
        إذا(pBiDi->reorderingMode==UBIDI_REORDER_RUNS_ONLY) {
            *pLevel=(UBiDiLevel)GET_ODD_BIT(iRun.logicalStart);
        }
        وإلا إذا(pBiDi->direction!=UBIDI_MIXED || logicalPosition>=pBiDi->trailingWSStart) {
            *pLevel=GET_PARALEVEL(pBiDi, logicalPosition);
        } وإلا {
        *pLevel=pBiDi->levels[logicalPosition];
        }
    }
}

/* runs API functions ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_كدي_Runs(UBiDi *pBiDi, UErrorCode *pErrorCode) {
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_لي(pBiDi, *pErrorCode, -1);
    ubidi_getRuns(pBiDi, pErrorCode);
    إذا(U_FAILURE(*pErrorCode)) {
        أرجع -1;
    }
    أرجع pBiDi->run_كدي;
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_getVisualRun(UBiDi *pBiDi, int32_t runIndex,
                   int32_t *pLogicalStart, int32_t *pLength)
{
    int32_t start;
    UErrorCode errorCode = U_ZERO_ERROR;
    RETURN_IF_NOT_VALID_PARA_OR_لي(pBiDi, errorCode, UBIDI_LTR);
    ubidi_getRuns(pBiDi, &errorCode);
    إذا(U_FAILURE(errorCode)) {
        أرجع UBIDI_LTR;
    }
    RETURN_IF_BAD_RANGE(runIndex, 0, pBiDi->run_كدي, errorCode, UBIDI_LTR);

    start=pBiDi->runs[runIndex].logicalStart;
    إذا(pLogicalStart!=nullptr) {
        *pLogicalStart=GET_INDEX(start);
    }
    إذا(pLength!=nullptr) {
        إذا(runIndex>0) {
            *pLength=pBiDi->runs[runIndex].visualLimit-
                     pBiDi->runs[runIndex-1].visualLimit;
        } وإلا {
            *pLength=pBiDi->runs[0].visualLimit;
        }
    }
    أرجع (UBiDiDirection)GET_ODD_BIT(start);
}

/* in trivial cases there is only one trivial run; called by ubidi_getRuns() */
ساكن فراغ
getSingleRun(UBiDi *pBiDi, UBiDiLevel level) {
    /* simple, single-run case */
    pBiDi->runs=pBiDi->simpleRuns;
    pBiDi->run_كدي=1;

    /* fill and reorder the single run */
    pBiDi->runs[0].logicalStart=MAKE_INDEX_ODD_PAIR(0, level);
    pBiDi->runs[0].visualLimit=pBiDi->length;
    pBiDi->runs[0].سك_Remove=0;
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
reorder_لي(UBiDi *pBiDi, UBiDiLevel minLevel, UBiDiLevel maxLevel) {
    Run *runs, tempRun;
    UBiDiLevel *levels;
    int32_t firstRun, endRun, limitRun, run_كدي;

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
    levels=pBiDi->levels;
    run_كدي=pBiDi->run_كدي;

    /* do not include the WS run at paraLevel<=old minLevel except in the simple loop */
    إذا(pBiDi->trailingWSStart<pBiDi->length) {
        --run_كدي;
    }

    طالما(--maxLevel>=minLevel) {
        firstRun=0;

        /* loop for all sequences of runs */
        لكل(;;) {
            /* look for a sequence of runs that are all at >=maxLevel */
            /* look for the first run of such a sequence */
            طالما(firstRun<run_كدي && levels[runs[firstRun].logicalStart]<maxLevel) {
                ++firstRun;
            }
            إذا(firstRun>=run_كدي) {
                اكسر;  /* no more such runs */
            }

            /* look for the limit run of such a sequence (the run behind it) */
            لكل(limitRun=firstRun; ++limitRun<run_كدي && levels[runs[limitRun].logicalStart]>=maxLevel;) {}

            /* Swap the entire sequence of runs from firstRun to limitRun-1. */
            endRun=limitRun-1;
            طالما(firstRun<endRun) {
                tempRun = runs[firstRun];
                runs[firstRun]=runs[endRun];
                runs[endRun]=tempRun;
                ++firstRun;
                --endRun;
            }

            إذا(limitRun==run_كدي) {
                اكسر;  /* no more such runs */
            } وإلا {
                firstRun=limitRun+1;
            }
        }
    }

    /* now do maxLevel==old minLevel (==odd!), see above */
    إذا(!(minLevel&1)) {
        firstRun=0;

        /* include the trailing WS run in this complete reordering */
        إذا(pBiDi->trailingWSStart==pBiDi->length) {
            --run_كدي;
        }

        /* Swap the entire sequence of all runs. (endRun==runCount) */
        طالما(firstRun<run_كدي) {
            tempRun=runs[firstRun];
            runs[firstRun]=runs[run_كدي];
            runs[run_كدي]=tempRun;
            ++firstRun;
            --run_كدي;
        }
    }
}

/* compute the runs array --------------------------------------------------- */

ساكن int32_t getRunFromLogicalIndex(UBiDi *pBiDi, int32_t logicalIndex) {
    Run *runs=pBiDi->runs;
    int32_t run_كدي=pBiDi->run_كدي, visualStart=0, i, length, logicalStart;

    لكل(i=0; i<run_كدي; i++) {
        length=runs[i].visualLimit-visualStart;
        logicalStart=GET_INDEX(runs[i].logicalStart);
        إذا((logicalIndex>=logicalStart) && (logicalIndex<(logicalStart+length))) {
            أرجع i;
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
ubidi_getRuns(UBiDi *pBiDi, UErrorCode* pErrorCode) {
    /*
     * This method returns immediately if the runs are already set. This
     * includes the case of length==0 (handled in setPara)..
     */
    إذا (pBiDi->run_كدي>=0) {
        أرجع true;
    }

    إذا(pBiDi->direction!=UBIDI_MIXED) {
        /* simple, single-run case - this covers length==0 */
        /* pBiDi->paraLevel is ok even for contextual multiple paragraphs */
        getSingleRun(pBiDi, pBiDi->paraLevel);
    } وإلا /* UBIDI_MIXED, length>0 */ {
        /* mixed directionality */
        int32_t length=pBiDi->length, limit;
        UBiDiLevel *levels=pBiDi->levels;
        int32_t i, run_كدي;
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
        limit=pBiDi->trailingWSStart;
        /* count the runs, there is at least one non-WS run, and limit>0 */
        run_كدي=0;
        لكل(i=0; i<limit; ++i) {
            /* increment runCount at the start of each run */
            إذا(levels[i]!=level) {
                ++run_كدي;
                level=levels[i];
            }
        }

        /*
         * We don't need to see if the last run can be merged with a trailing
         * WS run because setTrailingWSStart() would have done that.
         */
        إذا(run_كدي==1 && limit==length) {
            /* There is only one non-WS run and no trailing WS-run. */
            getSingleRun(pBiDi, levels[0]);
        } وإلا /* runCount>1 || limit<length */ {
            /* allocate and set the runs */
            Run *runs;
            int32_t runIndex, start;
            UBiDiLevel minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1, maxLevel=0;

            /* now, count a (non-mergeable) WS run */
            إذا(limit<length) {
                ++run_كدي;
            }

            /* runCount>1 */
            إذا(getRunsMemory(pBiDi, run_كدي)) {
                runs=pBiDi->runsMemory;
            } وإلا {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                أرجع خطأ;
            }

            /* set the runs */
            /* FOOD FOR THOUGHT: this could be optimized, e.g.:
             * 464->444, 484->444, 575->555, 595->555
             * However, that would take longer. Check also how it would
             * interact with BiDi control removal and inserting Marks.
             */
            runIndex=0;

            /* search for the run limits and initialize visualLimit values with the run lengths */
            i=0;
            افعل {
                /* prepare this run */
                start=i;
                level=levels[i];
                إذا(level<minLevel) {
                    minLevel=level;
                }
                إذا(level>maxLevel) {
                    maxLevel=level;
                }

                /* look for the run limit */
                طالما(++i<limit && levels[i]==level) {}

                /* i is another run limit */
                runs[runIndex].logicalStart=start;
                runs[runIndex].visualLimit=i-start;
                runs[runIndex].سك_Remove=0;
                ++runIndex;
            } طالما(i<limit);

            إذا(limit<length) {
                /* there is a separate WS run */
                runs[runIndex].logicalStart=limit;
                runs[runIndex].visualLimit=length-limit;
                /* For the trailing WS run, pBiDi->paraLevel is ok even
                   if contextual multiple paragraphs.                   */
                إذا(pBiDi->paraLevel<minLevel) {
                    minLevel=pBiDi->paraLevel;
                }
            }

            /* set the object fields */
            pBiDi->runs=runs;
            pBiDi->run_كدي=run_كدي;

            reorder_لي(pBiDi, minLevel, maxLevel);

            /* now add the direction flags and adjust the visualLimit's to be just that */
            /* this loop will also handle the trailing WS run */
            limit=0;
            لكل(i=0; i<run_كدي; ++i) {
                ADD_ODD_BIT_FROM_LEVEL(runs[i].logicalStart, levels[runs[i].logicalStart]);
                limit+=runs[i].visualLimit;
                runs[i].visualLimit=limit;
            }

            /* Set the "odd" bit for the trailing WS run. */
            /* For a RTL paragraph, it will be the *first* run in visual order. */
            /* For the trailing WS run, pBiDi->paraLevel is ok even if
               contextual multiple paragraphs.                          */
            إذا(runIndex<run_كدي) {
                int32_t trailingRun = ((pBiDi->paraLevel & 1) != 0)? 0 : runIndex;

                ADD_ODD_BIT_FROM_LEVEL(runs[trailingRun].logicalStart, pBiDi->paraLevel);
            }
        }
    }

    /* handle insert LRM/RLM BEFORE/AFTER run */
    إذا(pBiDi->سك_Points.size>0) {
        Point *point, *start=pBiDi->سك_Points.points,
                      *limit=start+pBiDi->سك_Points.size;
        int32_t runIndex;
        لكل(point=start; point<limit; point++) {
            runIndex=getRunFromLogicalIndex(pBiDi, point->pos);
            pBiDi->runs[runIndex].سك_Remove|=point->flag;
        }
    }

    /* handle remove BiDi control characters */
    إذا(pBiDi->control_كدي>0) {
        int32_t runIndex;
        ثابت char16_t *start=pBiDi->text, *limit=start+pBiDi->length, *pu;
        لكل(pu=start; pu<limit; pu++) {
            إذا(IS_BIDI_CONTROL_CHAR(*pu)) {
                runIndex=getRunFromLogicalIndex(pBiDi, (int32_t)(pu-start));
                pBiDi->runs[runIndex].سك_Remove--;
            }
        }
    }

    أرجع true;
}

ساكن UBool
prepareReorder(ثابت UBiDiLevel *levels, int32_t length,
               int32_t *indexMap,
               UBiDiLevel *pMinLevel, UBiDiLevel *pMaxLevel) {
    int32_t start;
    UBiDiLevel level, minLevel, maxLevel;

    إذا(levels==nullptr || length<=0) {
        أرجع خطأ;
    }

    /* determine minLevel and maxLevel */
    minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1;
    maxLevel=0;
    لكل(start=length; start>0;) {
        level=levels[--start];
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
    لكل(start=length; start>0;) {
        --start;
        indexMap[start]=start;
    }

    أرجع true;
}

/* reorder a line based on a levels array (L2) ------------------------------ */

U_CAPI فراغ U_EXPORT2
ubidi_reorderLogical(ثابت UBiDiLevel *levels, int32_t length, int32_t *indexMap) {
    int32_t start, limit, sumOfSosEos;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    إذا(indexMap==nullptr || !prepareReorder(levels, length, indexMap, &minLevel, &maxLevel)) {
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
        start=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        لكل(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            طالما(start<length && levels[start]<maxLevel) {
                ++start;
            }
            إذا(start>=length) {
                اكسر;  /* no more such sequences */
            }

            /* look for the limit of such a sequence (the index behind it) */
            لكل(limit=start; ++limit<length && levels[limit]>=maxLevel;) {}

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
            sumOfSosEos=start+limit-1;

            /* reorder each index in the sequence */
            افعل {
                indexMap[start]=sumOfSosEos-indexMap[start];
            } طالما(++start<limit);

            /* start==limit */
            إذا(limit==length) {
                اكسر;  /* no more such sequences */
            } وإلا {
                start=limit+1;
            }
        }
    } طالما(--maxLevel>=minLevel);
}

U_CAPI فراغ U_EXPORT2
ubidi_reorderVisual(ثابت UBiDiLevel *levels, int32_t length, int32_t *indexMap) {
    int32_t start, end, limit, temp;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    إذا(indexMap==nullptr || !prepareReorder(levels, length, indexMap, &minLevel, &maxLevel)) {
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
        start=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        لكل(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            طالما(start<length && levels[start]<maxLevel) {
                ++start;
            }
            إذا(start>=length) {
                اكسر;  /* no more such runs */
            }

            /* look for the limit of such a sequence (the index behind it) */
            لكل(limit=start; ++limit<length && levels[limit]>=maxLevel;) {}

            /*
             * Swap the entire interval of indexes from start to limit-1.
             * We don't need to swap the levels for the purpose of this
             * algorithm: the sequence of levels that we look at does not
             * move anyway.
             */
            end=limit-1;
            طالما(start<end) {
                temp=indexMap[start];
                indexMap[start]=indexMap[end];
                indexMap[end]=temp;

                ++start;
                --end;
            }

            إذا(limit==length) {
                اكسر;  /* no more such sequences */
            } وإلا {
                start=limit+1;
            }
        }
    } طالما(--maxLevel>=minLevel);
}

/* API functions for logical<->visual mapping ------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_getVisualIndex(UBiDi *pBiDi, int32_t logicalIndex, UErrorCode *pErrorCode) {
    int32_t visualIndex=UBIDI_MAP_NOWHERE;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_لي(pBiDi, *pErrorCode, -1);
    RETURN_IF_BAD_RANGE(logicalIndex, 0, pBiDi->length, *pErrorCode, -1);

    /* we can do the trivial cases without the runs array */
    بدل(pBiDi->direction) {
    حالة UBIDI_LTR:
        visualIndex=logicalIndex;
        اكسر;
    حالة UBIDI_RTL:
        visualIndex=pBiDi->length-logicalIndex-1;
        اكسر;
    افتراضي:
        إذا(!ubidi_getRuns(pBiDi, pErrorCode)) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            أرجع -1;
        } وإلا {
            Run *runs=pBiDi->runs;
            int32_t i, visualStart=0, offset, length;

            /* linear search for the run, search on the visual runs */
            لكل(i=0; i<pBiDi->run_كدي; ++i) {
                length=runs[i].visualLimit-visualStart;
                offset=logicalIndex-GET_INDEX(runs[i].logicalStart);
                إذا(offset>=0 && offset<length) {
                    إذا(IS_EVEN_RUN(runs[i].logicalStart)) {
                        /* LTR */
                        visualIndex=visualStart+offset;
                    } وإلا {
                        /* RTL */
                        visualIndex=visualStart+length-offset-1;
                    }
                    اكسر;          /* exit for loop */
                }
                visualStart+=length;
            }
            إذا(i>=pBiDi->run_كدي) {
                أرجع UBIDI_MAP_NOWHERE;
            }
        }
    }

    إذا(pBiDi->سك_Points.size>0) {
        /* add the number of added marks until the calculated visual index */
        Run *runs=pBiDi->runs;
        int32_t i, length, سك_Remove;
        int32_t visualStart=0, markFound=0;
        لكل(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            سك_Remove=runs[i].سك_Remove;
            إذا(سك_Remove & (LRM_BEFORE|RLM_BEFORE)) {
                markFound++;
            }
            /* is it the run containing the visual index? */
            إذا(visualIndex<runs[i].visualLimit) {
                أرجع visualIndex+markFound;
            }
            إذا(سك_Remove & (LRM_AFTER|RLM_AFTER)) {
                markFound++;
            }
        }
    }
    وإلا إذا(pBiDi->control_كدي>0) {
        /* subtract the number of controls until the calculated visual index */
        Run *runs=pBiDi->runs;
        int32_t i, j, start, limit, length, سك_Remove;
        int32_t visualStart=0, controlFound=0;
        char16_t uchar=pBiDi->text[logicalIndex];
        /* is the logical index pointing to a control ? */
        إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
            أرجع UBIDI_MAP_NOWHERE;
        }
        /* loop on runs */
        لكل(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            سك_Remove=runs[i].سك_Remove;
            /* calculated visual index is beyond this run? */
            إذا(visualIndex>=runs[i].visualLimit) {
                controlFound-=سك_Remove;
                استمر;
            }
            /* calculated visual index must be within current run */
            إذا(سك_Remove==0) {
                أرجع visualIndex-controlFound;
            }
            إذا(IS_EVEN_RUN(runs[i].logicalStart)) {
                /* LTR: check from run start to logical index */
                start=runs[i].logicalStart;
                limit=logicalIndex;
            } وإلا {
                /* RTL: check from logical index to run end */
                start=logicalIndex+1;
                limit=GET_INDEX(runs[i].logicalStart)+length;
            }
            لكل(j=start; j<limit; j++) {
                uchar=pBiDi->text[j];
                إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
                    controlFound++;
                }
            }
            أرجع visualIndex-controlFound;
        }
    }

    أرجع visualIndex;
}

U_CAPI int32_t U_EXPORT2
ubidi_getLogicalIndex(UBiDi *pBiDi, int32_t visualIndex, UErrorCode *pErrorCode) {
    Run *runs;
    int32_t i, run_كدي, start;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_لي(pBiDi, *pErrorCode, -1);
    RETURN_IF_BAD_RANGE(visualIndex, 0, pBiDi->resultLength, *pErrorCode, -1);
    /* we can do the trivial cases without the runs array */
    إذا(pBiDi->سك_Points.size==0 && pBiDi->control_كدي==0) {
        إذا(pBiDi->direction==UBIDI_LTR) {
            أرجع visualIndex;
        }
        وإلا إذا(pBiDi->direction==UBIDI_RTL) {
            أرجع pBiDi->length-visualIndex-1;
        }
    }
    إذا(!ubidi_getRuns(pBiDi, pErrorCode)) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع -1;
    }

    runs=pBiDi->runs;
    run_كدي=pBiDi->run_كدي;
    إذا(pBiDi->سك_Points.size>0) {
        /* handle inserted LRM/RLM */
        int32_t markFound=0, سك_Remove;
        int32_t visualStart=0, length;
        runs=pBiDi->runs;
        /* subtract number of marks until visual index */
        لكل(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            سك_Remove=runs[i].سك_Remove;
            إذا(سك_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                إذا(visualIndex<=(visualStart+markFound)) {
                    أرجع UBIDI_MAP_NOWHERE;
                }
                markFound++;
            }
            /* is adjusted visual index within this run? */
            إذا(visualIndex<(runs[i].visualLimit+markFound)) {
                visualIndex-=markFound;
                اكسر;
            }
            إذا(سك_Remove&(LRM_AFTER|RLM_AFTER)) {
                إذا(visualIndex==(visualStart+length+markFound)) {
                    أرجع UBIDI_MAP_NOWHERE;
                }
                markFound++;
            }
        }
    }
    وإلا إذا(pBiDi->control_كدي>0) {
        /* handle removed BiDi control characters */
        int32_t controlFound=0, سك_Remove, length;
        int32_t logicalStart, logicalEnd, visualStart=0, j, k;
        char16_t uchar;
        UBool evenRun;
        /* add number of controls until visual index */
        لكل(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            سك_Remove=runs[i].سك_Remove;
            /* is adjusted visual index beyond current run? */
            إذا(visualIndex>=(runs[i].visualLimit-controlFound+سك_Remove)) {
                controlFound-=سك_Remove;
                استمر;
            }
            /* adjusted visual index is within current run */
            إذا(سك_Remove==0) {
                visualIndex+=controlFound;
                اكسر;
            }
            /* count non-control chars until visualIndex */
            logicalStart=runs[i].logicalStart;
            evenRun=IS_EVEN_RUN(logicalStart);
            REMOVE_ODD_BIT(logicalStart);
            logicalEnd=logicalStart+length-1;
            لكل(j=0; j<length; j++) {
                k= evenRun ? logicalStart+j : logicalEnd-j;
                uchar=pBiDi->text[k];
                إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
                    controlFound++;
                }
                إذا((visualIndex+controlFound)==(visualStart+j)) {
                    اكسر;
                }
            }
            visualIndex+=controlFound;
            اكسر;
        }
    }
    /* handle all cases */
    إذا(run_كدي<=10) {
        /* linear search for the run */
        لكل(i=0; visualIndex>=runs[i].visualLimit; ++i) {}
    } وإلا {
        /* binary search for the run */
        int32_t begin=0, limit=run_كدي;

        /* the middle if() is guaranteed to find the run, we don't need a loop limit */
        لكل(;;) {
            i=(begin+limit)/2;
            إذا(visualIndex>=runs[i].visualLimit) {
                begin=i+1;
            } وإلا إذا(i==0 || visualIndex>=runs[i-1].visualLimit) {
                اكسر;
            } وإلا {
                limit=i;
            }
        }
    }

    start=runs[i].logicalStart;
    إذا(IS_EVEN_RUN(start)) {
        /* LTR */
        /* the offset in runs[i] is visualIndex-runs[i-1].visualLimit */
        إذا(i>0) {
            visualIndex-=runs[i-1].visualLimit;
        }
        أرجع start+visualIndex;
    } وإلا {
        /* RTL */
        أرجع GET_INDEX(start)+runs[i].visualLimit-visualIndex-1;
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_getLogicalMap(UBiDi *pBiDi, int32_t *indexMap, UErrorCode *pErrorCode) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_كدي_Runs(pBiDi, pErrorCode);
    إذا(U_FAILURE(*pErrorCode)) {
        /* no op */
    } وإلا إذا(indexMap==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } وإلا {
        /* fill a logical-to-visual index map using the runs[] */
        int32_t visualStart, visualLimit, i, j, k;
        int32_t logicalStart, logicalLimit;
        Run *runs=pBiDi->runs;
        إذا (pBiDi->length<=0) {
            أرجع;
        }
        إذا (pBiDi->length>pBiDi->resultLength) {
            uprv_memset(indexMap, 0xFF, pBiDi->length*حجم(int32_t));
        }

        visualStart=0;
        لكل(j=0; j<pBiDi->run_كدي; ++j) {
            logicalStart=GET_INDEX(runs[j].logicalStart);
            visualLimit=runs[j].visualLimit;
            إذا(IS_EVEN_RUN(runs[j].logicalStart)) {
                افعل { /* LTR */
                    indexMap[logicalStart++]=visualStart++;
                } طالما(visualStart<visualLimit);
            } وإلا {
                logicalStart+=visualLimit-visualStart;  /* logicalLimit */
                افعل { /* RTL */
                    indexMap[--logicalStart]=visualStart++;
                } طالما(visualStart<visualLimit);
            }
            /* visualStart==visualLimit; */
        }

        إذا(pBiDi->سك_Points.size>0) {
            int32_t markFound=0, run_كدي=pBiDi->run_كدي;
            int32_t length, سك_Remove;
            visualStart=0;
            /* add number of marks found until each index */
            لكل(i=0; i<run_كدي; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                سك_Remove=runs[i].سك_Remove;
                إذا(سك_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    markFound++;
                }
                إذا(markFound>0) {
                    logicalStart=GET_INDEX(runs[i].logicalStart);
                    logicalLimit=logicalStart+length;
                    لكل(j=logicalStart; j<logicalLimit; j++) {
                        indexMap[j]+=markFound;
                    }
                }
                إذا(سك_Remove&(LRM_AFTER|RLM_AFTER)) {
                    markFound++;
                }
            }
        }
        وإلا إذا(pBiDi->control_كدي>0) {
            int32_t controlFound=0, run_كدي=pBiDi->run_كدي;
            int32_t length, سك_Remove;
            UBool evenRun;
            char16_t uchar;
            visualStart=0;
            /* subtract number of controls found until each index */
            لكل(i=0; i<run_كدي; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                سك_Remove=runs[i].سك_Remove;
                /* no control found within previous runs nor within this run */
                إذا((controlFound-سك_Remove)==0) {
                    استمر;
                }
                logicalStart=runs[i].logicalStart;
                evenRun=IS_EVEN_RUN(logicalStart);
                REMOVE_ODD_BIT(logicalStart);
                logicalLimit=logicalStart+length;
                /* if no control within this run */
                إذا(سك_Remove==0) {
                    لكل(j=logicalStart; j<logicalLimit; j++) {
                        indexMap[j]-=controlFound;
                    }
                    استمر;
                }
                لكل(j=0; j<length; j++) {
                    k= evenRun ? logicalStart+j : logicalLimit-j-1;
                    uchar=pBiDi->text[k];
                    إذا(IS_BIDI_CONTROL_CHAR(uchar)) {
                        controlFound++;
                        indexMap[k]=UBIDI_MAP_NOWHERE;
                        استمر;
                    }
                    indexMap[k]-=controlFound;
                }
            }
        }
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_getVisualMap(UBiDi *pBiDi, int32_t *indexMap, UErrorCode *pErrorCode) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    إذا(indexMap==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_كدي_Runs(pBiDi, pErrorCode);
    إذا(U_SUCCESS(*pErrorCode)) {
        /* fill a visual-to-logical index map using the runs[] */
        Run *runs=pBiDi->runs, *runsLimit=runs+pBiDi->run_كدي;
        int32_t logicalStart, visualStart, visualLimit, *pi=indexMap;

        إذا (pBiDi->resultLength<=0) {
            أرجع;
        }
        visualStart=0;
        لكل(; runs<runsLimit; ++runs) {
            logicalStart=runs->logicalStart;
            visualLimit=runs->visualLimit;
            إذا(IS_EVEN_RUN(logicalStart)) {
                افعل { /* LTR */
                    *pi++ = logicalStart++;
                } طالما(++visualStart<visualLimit);
            } وإلا {
                REMOVE_ODD_BIT(logicalStart);
                logicalStart+=visualLimit-visualStart;  /* logicalLimit */
                افعل { /* RTL */
                    *pi++ = --logicalStart;
                } طالما(++visualStart<visualLimit);
            }
            /* visualStart==visualLimit; */
        }

        إذا(pBiDi->سك_Points.size>0) {
            int32_t markFound=0, run_كدي=pBiDi->run_كدي;
            int32_t سك_Remove, i, j, k;
            runs=pBiDi->runs;
            /* count all inserted marks */
            لكل(i=0; i<run_كدي; i++) {
                سك_Remove=runs[i].سك_Remove;
                إذا(سك_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    markFound++;
                }
                إذا(سك_Remove&(LRM_AFTER|RLM_AFTER)) {
                    markFound++;
                }
            }
            /* move back indexes by number of preceding marks */
            k=pBiDi->resultLength;
            لكل(i=run_كدي-1; i>=0 && markFound>0; i--) {
                سك_Remove=runs[i].سك_Remove;
                إذا(سك_Remove&(LRM_AFTER|RLM_AFTER)) {
                    indexMap[--k]= UBIDI_MAP_NOWHERE;
                    markFound--;
                }
                visualStart= i>0 ? runs[i-1].visualLimit : 0;
                لكل(j=runs[i].visualLimit-1; j>=visualStart && markFound>0; j--) {
                    indexMap[--k]=indexMap[j];
                }
                إذا(سك_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    indexMap[--k]= UBIDI_MAP_NOWHERE;
                    markFound--;
                }
            }
        }
        وإلا إذا(pBiDi->control_كدي>0) {
            int32_t run_كدي=pBiDi->run_كدي, logicalEnd;
            int32_t سك_Remove, length, i, j, k, m;
            char16_t uchar;
            UBool evenRun;
            runs=pBiDi->runs;
            visualStart=0;
            /* move forward indexes by number of preceding controls */
            k=0;
            لكل(i=0; i<run_كدي; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                سك_Remove=runs[i].سك_Remove;
                /* if no control found yet, nothing to do in this run */
                إذا((سك_Remove==0)&&(k==visualStart)) {
                    k+=length;
                    استمر;
                }
                /* if no control in this run */
                إذا(سك_Remove==0) {
                    visualLimit=runs[i].visualLimit;
                    لكل(j=visualStart; j<visualLimit; j++) {
                        indexMap[k++]=indexMap[j];
                    }
                    استمر;
                }
                logicalStart=runs[i].logicalStart;
                evenRun=IS_EVEN_RUN(logicalStart);
                REMOVE_ODD_BIT(logicalStart);
                logicalEnd=logicalStart+length-1;
                لكل(j=0; j<length; j++) {
                    m= evenRun ? logicalStart+j : logicalEnd-j;
                    uchar=pBiDi->text[m];
                    إذا(!IS_BIDI_CONTROL_CHAR(uchar)) {
                        indexMap[k++]=m;
                    }
                }
            }
        }
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_invertMap(ثابت int32_t *srcMap, int32_t *destMap, int32_t length) {
    إذا(srcMap!=nullptr && destMap!=nullptr && length>0) {
        ثابت int32_t *pi;
        int32_t destLength=-1, count=0;
        /* find highest value and count positive indexes in srcMap */
        pi=srcMap+length;
        طالما(pi>srcMap) {
            إذا(*--pi>destLength) {
                destLength=*pi;
            }
            إذا(*pi>=0) {
                count++;
            }
        }
        destLength++;           /* add 1 for origin 0 */
        إذا(count<destLength) {
            /* we must fill unmatched destMap entries with -1 */
            uprv_memset(destMap, 0xFF, destLength*حجم(int32_t));
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
