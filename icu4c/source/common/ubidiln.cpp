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

#include "cmemory.h"
#include "unicode/utypes.h"
#include "unicode/ustring.h"
#include "unicode/uchar.h"
#include "unicode/ubidi.h"
#include "ubidiimp.h"
#include "uassert.h"

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
static void
setTrailingWSStart(UBiDi *pBiDi) {
    /* pBiDi->direction!=UBIDI_MIXED */

    const DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *levels=pBiDi->levels;
    int32_t start=pBiDi->length;
    UBiDiLevel paraLevel=pBiDi->paraLevel;

    /* If the line is terminated by a block separator, all preceding WS etc...
       are already set to paragraph level.
       Setting trailingWSStart to pBidi->length will avoid changing the
       level of B chars from 0 to paraLevel in ubidi_getLevels when
       orderParagraphsLTR==true.
     */
    如果(dirProps[start-1]==B) {
        pBiDi->trailingWSStart=start;   /* currently == pBiDi->length */
        返回;
    }
    /* go backwards across all WS, BN, explicit codes */
    当(start>0 && DIRPROP_FLAG(dirProps[start-1])&MASK_WS) {
        --start;
    }

    /* if the WS run can be merged with the previous run then do so here */
    当(start>0 && levels[start-1]==paraLevel) {
        --start;
    }

    pBiDi->trailingWSStart=start;
}

/* ubidi_setLine ------------------------------------------------------------ */

U_CAPI void U_EXPORT2
ubidi_setLine(const UBiDi *pParaBiDi,
              int32_t start, int32_t limit,
              UBiDi *pLineBiDi,
              UErrorCode *pErrorCode) {
    int32_t length;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    RETURN_VOID_IF_NOT_VALID_PARA(pParaBiDi, *pErrorCode);
    RETURN_VOID_IF_BAD_RANGE(start, 0, limit, *pErrorCode);
    RETURN_VOID_IF_BAD_RANGE(limit, 0, pParaBiDi->length+1, *pErrorCode);
    如果(pLineBiDi==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        返回;
    }
    如果(ubidi_getParagraph(pParaBiDi, start, nullptr, nullptr, nullptr, pErrorCode) !=
       ubidi_getParagraph(pParaBiDi, limit-1, nullptr, nullptr, nullptr, pErrorCode)) {
        /* the line crosses a paragraph boundary */
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        返回;
    }

    /* set the values in pLineBiDi from its pParaBiDi parent */
    pLineBiDi->pParaBiDi=nullptr;          /* mark unfinished setLine */
    pLineBiDi->text=pParaBiDi->text+start;
    length=pLineBiDi->length=limit-start;
    pLineBiDi->resultLength=pLineBiDi->originalLength=length;
    pLineBiDi->paraLevel=GET_PARALEVEL(pParaBiDi, start);
    pLineBiDi->paraCount=pParaBiDi->paraCount;
    pLineBiDi->runs=nullptr;
    pLineBiDi->flags=0;
    pLineBiDi->reorderingMode=pParaBiDi->reorderingMode;
    pLineBiDi->reorderingOptions=pParaBiDi->reorderingOptions;
    pLineBiDi->controlCount=0;
    如果(pParaBiDi->controlCount>0) {
        int32_t j;
        为(j=start; j<limit; j++) {
            如果(IS_BIDI_CONTROL_CHAR(pParaBiDi->text[j])) {
                pLineBiDi->controlCount++;
            }
        }
        pLineBiDi->resultLength-=pLineBiDi->controlCount;
    }

    pLineBiDi->dirProps=pParaBiDi->dirProps+start;
    pLineBiDi->levels=pParaBiDi->levels+start;
    pLineBiDi->runCount=-1;

    如果(pParaBiDi->direction!=UBIDI_MIXED) {
        /* the parent is already trivial */
        pLineBiDi->direction=pParaBiDi->direction;

        /*
         * The parent's levels are all either
         * implicitly or explicitly ==paraLevel;
         * do the same here.
         */
        如果(pParaBiDi->trailingWSStart<=start) {
            pLineBiDi->trailingWSStart=0;
        } 否则 如果(pParaBiDi->trailingWSStart<limit) {
            pLineBiDi->trailingWSStart=pParaBiDi->trailingWSStart-start;
        } 否则 {
            pLineBiDi->trailingWSStart=length;
        }
    } 否则 {
        const UBiDiLevel *levels=pLineBiDi->levels;
        int32_t i, trailingWSStart;
        UBiDiLevel level;

        setTrailingWSStart(pLineBiDi);
        trailingWSStart=pLineBiDi->trailingWSStart;

        /* recalculate pLineBiDi->direction */
        如果(trailingWSStart==0) {
            /* all levels are at paraLevel */
            pLineBiDi->direction=(UBiDiDirection)(pLineBiDi->paraLevel&1);
        } 否则 {
            /* get the level of the first character */
            level=(UBiDiLevel)(levels[0]&1);

            /* if there is anything of a different level, then the line is mixed */
            如果(trailingWSStart<length && (pLineBiDi->paraLevel&1)!=level) {
                /* the trailing WS is at paraLevel, which differs from levels[0] */
                pLineBiDi->direction=UBIDI_MIXED;
            } 否则 {
                /* see if levels[1..trailingWSStart-1] have the same direction as levels[0] and paraLevel */
                i=1;
                为(;;) {
                    如果(i==trailingWSStart) {
                        /* the direction values match those in level */
                        pLineBiDi->direction=(UBiDiDirection)level;
                        break;
                    } 否则 如果((levels[i]&1)!=level) {
                        pLineBiDi->direction=UBIDI_MIXED;
                        break;
                    }
                    ++i;
                }
            }
        }

        switch(pLineBiDi->direction) {
        case UBIDI_LTR:
            /* make sure paraLevel is even */
            pLineBiDi->paraLevel=(UBiDiLevel)((pLineBiDi->paraLevel+1)&~1);

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            pLineBiDi->trailingWSStart=0;
            break;
        case UBIDI_RTL:
            /* make sure paraLevel is odd */
            pLineBiDi->paraLevel|=1;

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            pLineBiDi->trailingWSStart=0;
            break;
        default:
            break;
        }
    }
    pLineBiDi->pParaBiDi=pParaBiDi;     /* mark successful setLine */
}

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getLevelAt(const UBiDi *pBiDi, int32_t charIndex) {
    /* return paraLevel if in the trailing WS run, otherwise the real level */
    如果(!IS_VALID_PARA_OR_LINE(pBiDi) || charIndex<0 || pBiDi->length<=charIndex) {
        返回 0;
    } 否则 如果(pBiDi->direction!=UBIDI_MIXED || charIndex>=pBiDi->trailingWSStart) {
        返回 GET_PARALEVEL(pBiDi, charIndex);
    } 否则 {
        返回 pBiDi->levels[charIndex];
    }
}

U_CAPI const UBiDiLevel* U_EXPORT2
ubidi_getLevels(UBiDi* pBiDi, UErrorCode* pErrorCode) {
    int32_t start, length;

    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, nullptr);
    RETURN_IF_NOT_VALID_PARA_OR_LINE(pBiDi, *pErrorCode, nullptr);
    如果((length=pBiDi->length)<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        返回 nullptr;
    }
    如果((start=pBiDi->trailingWSStart)==length) {
        /* the current levels array reflects the WS run */
        返回 pBiDi->levels;
    }

    /*
     * After the previous if(), we know that the levels array
     * has an implicit trailing WS run and therefore does not fully
     * reflect itself all the levels.
     * This must be a UBiDi object for a line, and
     * we need to create a new levels array.
     */
    如果(getLevelsMemory(pBiDi, length)) {
        UBiDiLevel *levels=pBiDi->levelsMemory;

        如果(start>0 && levels!=pBiDi->levels) {
            uprv_memcpy(levels, pBiDi->levels, start);
        }
        /* pBiDi->paraLevel is ok even if contextual multiple paragraphs,
           since pBidi is a line object                                     */
        uprv_memset(levels+start, pBiDi->paraLevel, length-start);

        /* this new levels array is set for the line and reflects the WS run */
        pBiDi->trailingWSStart=length;
        返回 pBiDi->levels=levels;
    } 否则 {
        /* out of memory */
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        返回 nullptr;
    }
}

U_CAPI void U_EXPORT2
ubidi_getLogicalRun(const UBiDi *pBiDi, int32_t logicalPosition,
                    int32_t *pLogicalLimit, UBiDiLevel *pLevel) {
    UErrorCode errorCode;
    int32_t runCount, visualStart, logicalLimit, logicalFirst, i;
    Run iRun;

    errorCode=U_ZERO_ERROR;
    RETURN_VOID_IF_BAD_RANGE(logicalPosition, 0, pBiDi->length, errorCode);
    /* ubidi_countRuns will check VALID_PARA_OR_LINE */
    runCount=ubidi_countRuns((UBiDi *)pBiDi, &errorCode);
    如果(U_FAILURE(errorCode)) {
        返回;
    }
    /* this is done based on runs rather than on levels since levels have
       a special interpretation when UBIDI_REORDER_RUNS_ONLY
     */
    visualStart=logicalLimit=0;
    iRun=pBiDi->runs[0];

    为(i=0; i<runCount; i++) {
        iRun = pBiDi->runs[i];
        logicalFirst=GET_INDEX(iRun.logicalStart);
        logicalLimit=logicalFirst+iRun.visualLimit-visualStart;
        如果((logicalPosition>=logicalFirst) &&
           (logicalPosition<logicalLimit)) {
            break;
        }
        visualStart = iRun.visualLimit;
    }
    如果(pLogicalLimit) {
        *pLogicalLimit=logicalLimit;
    }
    如果(pLevel) {
        如果(pBiDi->reorderingMode==UBIDI_REORDER_RUNS_ONLY) {
            *pLevel=(UBiDiLevel)GET_ODD_BIT(iRun.logicalStart);
        }
        否则 如果(pBiDi->direction!=UBIDI_MIXED || logicalPosition>=pBiDi->trailingWSStart) {
            *pLevel=GET_PARALEVEL(pBiDi, logicalPosition);
        } 否则 {
        *pLevel=pBiDi->levels[logicalPosition];
        }
    }
}

/* runs API functions ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_countRuns(UBiDi *pBiDi, UErrorCode *pErrorCode) {
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_LINE(pBiDi, *pErrorCode, -1);
    ubidi_getRuns(pBiDi, pErrorCode);
    如果(U_FAILURE(*pErrorCode)) {
        返回 -1;
    }
    返回 pBiDi->runCount;
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_getVisualRun(UBiDi *pBiDi, int32_t runIndex,
                   int32_t *pLogicalStart, int32_t *pLength)
{
    int32_t start;
    UErrorCode errorCode = U_ZERO_ERROR;
    RETURN_IF_NOT_VALID_PARA_OR_LINE(pBiDi, errorCode, UBIDI_LTR);
    ubidi_getRuns(pBiDi, &errorCode);
    如果(U_FAILURE(errorCode)) {
        返回 UBIDI_LTR;
    }
    RETURN_IF_BAD_RANGE(runIndex, 0, pBiDi->runCount, errorCode, UBIDI_LTR);

    start=pBiDi->runs[runIndex].logicalStart;
    如果(pLogicalStart!=nullptr) {
        *pLogicalStart=GET_INDEX(start);
    }
    如果(pLength!=nullptr) {
        如果(runIndex>0) {
            *pLength=pBiDi->runs[runIndex].visualLimit-
                     pBiDi->runs[runIndex-1].visualLimit;
        } 否则 {
            *pLength=pBiDi->runs[0].visualLimit;
        }
    }
    返回 (UBiDiDirection)GET_ODD_BIT(start);
}

/* in trivial cases there is only one trivial run; called by ubidi_getRuns() */
static void
getSingleRun(UBiDi *pBiDi, UBiDiLevel level) {
    /* simple, single-run case */
    pBiDi->runs=pBiDi->simpleRuns;
    pBiDi->runCount=1;

    /* fill and reorder the single run */
    pBiDi->runs[0].logicalStart=MAKE_INDEX_ODD_PAIR(0, level);
    pBiDi->runs[0].visualLimit=pBiDi->length;
    pBiDi->runs[0].insertRemove=0;
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
static void
reorderLine(UBiDi *pBiDi, UBiDiLevel minLevel, UBiDiLevel maxLevel) {
    Run *runs, tempRun;
    UBiDiLevel *levels;
    int32_t firstRun, endRun, limitRun, runCount;

    /* nothing to do? */
    如果(maxLevel<=(minLevel|1)) {
        返回;
    }

    /*
     * Reorder only down to the lowest odd level
     * and reorder at an odd minLevel in a separate, simpler loop.
     * See comments above for why minLevel is always incremented.
     */
    ++minLevel;

    runs=pBiDi->runs;
    levels=pBiDi->levels;
    runCount=pBiDi->runCount;

    /* do not include the WS run at paraLevel<=old minLevel except in the simple loop */
    如果(pBiDi->trailingWSStart<pBiDi->length) {
        --runCount;
    }

    当(--maxLevel>=minLevel) {
        firstRun=0;

        /* loop for all sequences of runs */
        为(;;) {
            /* look for a sequence of runs that are all at >=maxLevel */
            /* look for the first run of such a sequence */
            当(firstRun<runCount && levels[runs[firstRun].logicalStart]<maxLevel) {
                ++firstRun;
            }
            如果(firstRun>=runCount) {
                break;  /* no more such runs */
            }

            /* look for the limit run of such a sequence (the run behind it) */
            为(limitRun=firstRun; ++limitRun<runCount && levels[runs[limitRun].logicalStart]>=maxLevel;) {}

            /* Swap the entire sequence of runs from firstRun to limitRun-1. */
            endRun=limitRun-1;
            当(firstRun<endRun) {
                tempRun = runs[firstRun];
                runs[firstRun]=runs[endRun];
                runs[endRun]=tempRun;
                ++firstRun;
                --endRun;
            }

            如果(limitRun==runCount) {
                break;  /* no more such runs */
            } 否则 {
                firstRun=limitRun+1;
            }
        }
    }

    /* now do maxLevel==old minLevel (==odd!), see above */
    如果(!(minLevel&1)) {
        firstRun=0;

        /* include the trailing WS run in this complete reordering */
        如果(pBiDi->trailingWSStart==pBiDi->length) {
            --runCount;
        }

        /* Swap the entire sequence of all runs. (endRun==runCount) */
        当(firstRun<runCount) {
            tempRun=runs[firstRun];
            runs[firstRun]=runs[runCount];
            runs[runCount]=tempRun;
            ++firstRun;
            --runCount;
        }
    }
}

/* compute the runs array --------------------------------------------------- */

static int32_t getRunFromLogicalIndex(UBiDi *pBiDi, int32_t logicalIndex) {
    Run *runs=pBiDi->runs;
    int32_t runCount=pBiDi->runCount, visualStart=0, i, length, logicalStart;

    为(i=0; i<runCount; i++) {
        length=runs[i].visualLimit-visualStart;
        logicalStart=GET_INDEX(runs[i].logicalStart);
        如果((logicalIndex>=logicalStart) && (logicalIndex<(logicalStart+length))) {
            返回 i;
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
    如果 (pBiDi->runCount>=0) {
        返回 true;
    }

    如果(pBiDi->direction!=UBIDI_MIXED) {
        /* simple, single-run case - this covers length==0 */
        /* pBiDi->paraLevel is ok even for contextual multiple paragraphs */
        getSingleRun(pBiDi, pBiDi->paraLevel);
    } 否则 /* UBIDI_MIXED, length>0 */ {
        /* mixed directionality */
        int32_t length=pBiDi->length, limit;
        UBiDiLevel *levels=pBiDi->levels;
        int32_t i, runCount;
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
        runCount=0;
        为(i=0; i<limit; ++i) {
            /* increment runCount at the start of each run */
            如果(levels[i]!=level) {
                ++runCount;
                level=levels[i];
            }
        }

        /*
         * We don't need to see if the last run can be merged with a trailing
         * WS run because setTrailingWSStart() would have done that.
         */
        如果(runCount==1 && limit==length) {
            /* There is only one non-WS run and no trailing WS-run. */
            getSingleRun(pBiDi, levels[0]);
        } 否则 /* runCount>1 || limit<length */ {
            /* allocate and set the runs */
            Run *runs;
            int32_t runIndex, start;
            UBiDiLevel minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1, maxLevel=0;

            /* now, count a (non-mergeable) WS run */
            如果(limit<length) {
                ++runCount;
            }

            /* runCount>1 */
            如果(getRunsMemory(pBiDi, runCount)) {
                runs=pBiDi->runsMemory;
            } 否则 {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                返回 false;
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
            do {
                /* prepare this run */
                start=i;
                level=levels[i];
                如果(level<minLevel) {
                    minLevel=level;
                }
                如果(level>maxLevel) {
                    maxLevel=level;
                }

                /* look for the run limit */
                当(++i<limit && levels[i]==level) {}

                /* i is another run limit */
                runs[runIndex].logicalStart=start;
                runs[runIndex].visualLimit=i-start;
                runs[runIndex].insertRemove=0;
                ++runIndex;
            } 当(i<limit);

            如果(limit<length) {
                /* there is a separate WS run */
                runs[runIndex].logicalStart=limit;
                runs[runIndex].visualLimit=length-limit;
                /* For the trailing WS run, pBiDi->paraLevel is ok even
                   if contextual multiple paragraphs.                   */
                如果(pBiDi->paraLevel<minLevel) {
                    minLevel=pBiDi->paraLevel;
                }
            }

            /* set the object fields */
            pBiDi->runs=runs;
            pBiDi->runCount=runCount;

            reorderLine(pBiDi, minLevel, maxLevel);

            /* now add the direction flags and adjust the visualLimit's to be just that */
            /* this loop will also handle the trailing WS run */
            limit=0;
            为(i=0; i<runCount; ++i) {
                ADD_ODD_BIT_FROM_LEVEL(runs[i].logicalStart, levels[runs[i].logicalStart]);
                limit+=runs[i].visualLimit;
                runs[i].visualLimit=limit;
            }

            /* Set the "odd" bit for the trailing WS run. */
            /* For a RTL paragraph, it will be the *first* run in visual order. */
            /* For the trailing WS run, pBiDi->paraLevel is ok even if
               contextual multiple paragraphs.                          */
            如果(runIndex<runCount) {
                int32_t trailingRun = ((pBiDi->paraLevel & 1) != 0)? 0 : runIndex;

                ADD_ODD_BIT_FROM_LEVEL(runs[trailingRun].logicalStart, pBiDi->paraLevel);
            }
        }
    }

    /* handle insert LRM/RLM BEFORE/AFTER run */
    如果(pBiDi->insertPoints.size>0) {
        Point *point, *start=pBiDi->insertPoints.points,
                      *limit=start+pBiDi->insertPoints.size;
        int32_t runIndex;
        为(point=start; point<limit; point++) {
            runIndex=getRunFromLogicalIndex(pBiDi, point->pos);
            pBiDi->runs[runIndex].insertRemove|=point->flag;
        }
    }

    /* handle remove BiDi control characters */
    如果(pBiDi->controlCount>0) {
        int32_t runIndex;
        const char16_t *start=pBiDi->text, *limit=start+pBiDi->length, *pu;
        为(pu=start; pu<limit; pu++) {
            如果(IS_BIDI_CONTROL_CHAR(*pu)) {
                runIndex=getRunFromLogicalIndex(pBiDi, (int32_t)(pu-start));
                pBiDi->runs[runIndex].insertRemove--;
            }
        }
    }

    返回 true;
}

static UBool
prepareReorder(const UBiDiLevel *levels, int32_t length,
               int32_t *indexMap,
               UBiDiLevel *pMinLevel, UBiDiLevel *pMaxLevel) {
    int32_t start;
    UBiDiLevel level, minLevel, maxLevel;

    如果(levels==nullptr || length<=0) {
        返回 false;
    }

    /* determine minLevel and maxLevel */
    minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1;
    maxLevel=0;
    为(start=length; start>0;) {
        level=levels[--start];
        如果(level>UBIDI_MAX_EXPLICIT_LEVEL+1) {
            返回 false;
        }
        如果(level<minLevel) {
            minLevel=level;
        }
        如果(level>maxLevel) {
            maxLevel=level;
        }
    }
    *pMinLevel=minLevel;
    *pMaxLevel=maxLevel;

    /* initialize the index map */
    为(start=length; start>0;) {
        --start;
        indexMap[start]=start;
    }

    返回 true;
}

/* reorder a line based on a levels array (L2) ------------------------------ */

U_CAPI void U_EXPORT2
ubidi_reorderLogical(const UBiDiLevel *levels, int32_t length, int32_t *indexMap) {
    int32_t start, limit, sumOfSosEos;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    如果(indexMap==nullptr || !prepareReorder(levels, length, indexMap, &minLevel, &maxLevel)) {
        返回;
    }

    /* nothing to do? */
    如果(minLevel==maxLevel && (minLevel&1)==0) {
        返回;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    do {
        start=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        为(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            当(start<length && levels[start]<maxLevel) {
                ++start;
            }
            如果(start>=length) {
                break;  /* no more such sequences */
            }

            /* look for the limit of such a sequence (the index behind it) */
            为(limit=start; ++limit<length && levels[limit]>=maxLevel;) {}

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
            do {
                indexMap[start]=sumOfSosEos-indexMap[start];
            } 当(++start<limit);

            /* start==limit */
            如果(limit==length) {
                break;  /* no more such sequences */
            } 否则 {
                start=limit+1;
            }
        }
    } 当(--maxLevel>=minLevel);
}

U_CAPI void U_EXPORT2
ubidi_reorderVisual(const UBiDiLevel *levels, int32_t length, int32_t *indexMap) {
    int32_t start, end, limit, temp;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    如果(indexMap==nullptr || !prepareReorder(levels, length, indexMap, &minLevel, &maxLevel)) {
        返回;
    }

    /* nothing to do? */
    如果(minLevel==maxLevel && (minLevel&1)==0) {
        返回;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    do {
        start=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        为(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            当(start<length && levels[start]<maxLevel) {
                ++start;
            }
            如果(start>=length) {
                break;  /* no more such runs */
            }

            /* look for the limit of such a sequence (the index behind it) */
            为(limit=start; ++limit<length && levels[limit]>=maxLevel;) {}

            /*
             * Swap the entire interval of indexes from start to limit-1.
             * We don't need to swap the levels for the purpose of this
             * algorithm: the sequence of levels that we look at does not
             * move anyway.
             */
            end=limit-1;
            当(start<end) {
                temp=indexMap[start];
                indexMap[start]=indexMap[end];
                indexMap[end]=temp;

                ++start;
                --end;
            }

            如果(limit==length) {
                break;  /* no more such sequences */
            } 否则 {
                start=limit+1;
            }
        }
    } 当(--maxLevel>=minLevel);
}

/* API functions for logical<->visual mapping ------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_getVisualIndex(UBiDi *pBiDi, int32_t logicalIndex, UErrorCode *pErrorCode) {
    int32_t visualIndex=UBIDI_MAP_NOWHERE;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_LINE(pBiDi, *pErrorCode, -1);
    RETURN_IF_BAD_RANGE(logicalIndex, 0, pBiDi->length, *pErrorCode, -1);

    /* we can do the trivial cases without the runs array */
    switch(pBiDi->direction) {
    case UBIDI_LTR:
        visualIndex=logicalIndex;
        break;
    case UBIDI_RTL:
        visualIndex=pBiDi->length-logicalIndex-1;
        break;
    default:
        如果(!ubidi_getRuns(pBiDi, pErrorCode)) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            返回 -1;
        } 否则 {
            Run *runs=pBiDi->runs;
            int32_t i, visualStart=0, offset, length;

            /* linear search for the run, search on the visual runs */
            为(i=0; i<pBiDi->runCount; ++i) {
                length=runs[i].visualLimit-visualStart;
                offset=logicalIndex-GET_INDEX(runs[i].logicalStart);
                如果(offset>=0 && offset<length) {
                    如果(IS_EVEN_RUN(runs[i].logicalStart)) {
                        /* LTR */
                        visualIndex=visualStart+offset;
                    } 否则 {
                        /* RTL */
                        visualIndex=visualStart+length-offset-1;
                    }
                    break;          /* exit for loop */
                }
                visualStart+=length;
            }
            如果(i>=pBiDi->runCount) {
                返回 UBIDI_MAP_NOWHERE;
            }
        }
    }

    如果(pBiDi->insertPoints.size>0) {
        /* add the number of added marks until the calculated visual index */
        Run *runs=pBiDi->runs;
        int32_t i, length, insertRemove;
        int32_t visualStart=0, markFound=0;
        为(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            insertRemove=runs[i].insertRemove;
            如果(insertRemove & (LRM_BEFORE|RLM_BEFORE)) {
                markFound++;
            }
            /* is it the run containing the visual index? */
            如果(visualIndex<runs[i].visualLimit) {
                返回 visualIndex+markFound;
            }
            如果(insertRemove & (LRM_AFTER|RLM_AFTER)) {
                markFound++;
            }
        }
    }
    否则 如果(pBiDi->controlCount>0) {
        /* subtract the number of controls until the calculated visual index */
        Run *runs=pBiDi->runs;
        int32_t i, j, start, limit, length, insertRemove;
        int32_t visualStart=0, controlFound=0;
        char16_t uchar=pBiDi->text[logicalIndex];
        /* is the logical index pointing to a control ? */
        如果(IS_BIDI_CONTROL_CHAR(uchar)) {
            返回 UBIDI_MAP_NOWHERE;
        }
        /* loop on runs */
        为(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            insertRemove=runs[i].insertRemove;
            /* calculated visual index is beyond this run? */
            如果(visualIndex>=runs[i].visualLimit) {
                controlFound-=insertRemove;
                continue;
            }
            /* calculated visual index must be within current run */
            如果(insertRemove==0) {
                返回 visualIndex-controlFound;
            }
            如果(IS_EVEN_RUN(runs[i].logicalStart)) {
                /* LTR: check from run start to logical index */
                start=runs[i].logicalStart;
                limit=logicalIndex;
            } 否则 {
                /* RTL: check from logical index to run end */
                start=logicalIndex+1;
                limit=GET_INDEX(runs[i].logicalStart)+length;
            }
            为(j=start; j<limit; j++) {
                uchar=pBiDi->text[j];
                如果(IS_BIDI_CONTROL_CHAR(uchar)) {
                    controlFound++;
                }
            }
            返回 visualIndex-controlFound;
        }
    }

    返回 visualIndex;
}

U_CAPI int32_t U_EXPORT2
ubidi_getLogicalIndex(UBiDi *pBiDi, int32_t visualIndex, UErrorCode *pErrorCode) {
    Run *runs;
    int32_t i, runCount, start;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_LINE(pBiDi, *pErrorCode, -1);
    RETURN_IF_BAD_RANGE(visualIndex, 0, pBiDi->resultLength, *pErrorCode, -1);
    /* we can do the trivial cases without the runs array */
    如果(pBiDi->insertPoints.size==0 && pBiDi->controlCount==0) {
        如果(pBiDi->direction==UBIDI_LTR) {
            返回 visualIndex;
        }
        否则 如果(pBiDi->direction==UBIDI_RTL) {
            返回 pBiDi->length-visualIndex-1;
        }
    }
    如果(!ubidi_getRuns(pBiDi, pErrorCode)) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        返回 -1;
    }

    runs=pBiDi->runs;
    runCount=pBiDi->runCount;
    如果(pBiDi->insertPoints.size>0) {
        /* handle inserted LRM/RLM */
        int32_t markFound=0, insertRemove;
        int32_t visualStart=0, length;
        runs=pBiDi->runs;
        /* subtract number of marks until visual index */
        为(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            insertRemove=runs[i].insertRemove;
            如果(insertRemove&(LRM_BEFORE|RLM_BEFORE)) {
                如果(visualIndex<=(visualStart+markFound)) {
                    返回 UBIDI_MAP_NOWHERE;
                }
                markFound++;
            }
            /* is adjusted visual index within this run? */
            如果(visualIndex<(runs[i].visualLimit+markFound)) {
                visualIndex-=markFound;
                break;
            }
            如果(insertRemove&(LRM_AFTER|RLM_AFTER)) {
                如果(visualIndex==(visualStart+length+markFound)) {
                    返回 UBIDI_MAP_NOWHERE;
                }
                markFound++;
            }
        }
    }
    否则 如果(pBiDi->controlCount>0) {
        /* handle removed BiDi control characters */
        int32_t controlFound=0, insertRemove, length;
        int32_t logicalStart, logicalEnd, visualStart=0, j, k;
        char16_t uchar;
        UBool evenRun;
        /* add number of controls until visual index */
        为(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            insertRemove=runs[i].insertRemove;
            /* is adjusted visual index beyond current run? */
            如果(visualIndex>=(runs[i].visualLimit-controlFound+insertRemove)) {
                controlFound-=insertRemove;
                continue;
            }
            /* adjusted visual index is within current run */
            如果(insertRemove==0) {
                visualIndex+=controlFound;
                break;
            }
            /* count non-control chars until visualIndex */
            logicalStart=runs[i].logicalStart;
            evenRun=IS_EVEN_RUN(logicalStart);
            REMOVE_ODD_BIT(logicalStart);
            logicalEnd=logicalStart+length-1;
            为(j=0; j<length; j++) {
                k= evenRun ? logicalStart+j : logicalEnd-j;
                uchar=pBiDi->text[k];
                如果(IS_BIDI_CONTROL_CHAR(uchar)) {
                    controlFound++;
                }
                如果((visualIndex+controlFound)==(visualStart+j)) {
                    break;
                }
            }
            visualIndex+=controlFound;
            break;
        }
    }
    /* handle all cases */
    如果(runCount<=10) {
        /* linear search for the run */
        为(i=0; visualIndex>=runs[i].visualLimit; ++i) {}
    } 否则 {
        /* binary search for the run */
        int32_t begin=0, limit=runCount;

        /* the middle if() is guaranteed to find the run, we don't need a loop limit */
        为(;;) {
            i=(begin+limit)/2;
            如果(visualIndex>=runs[i].visualLimit) {
                begin=i+1;
            } 否则 如果(i==0 || visualIndex>=runs[i-1].visualLimit) {
                break;
            } 否则 {
                limit=i;
            }
        }
    }

    start=runs[i].logicalStart;
    如果(IS_EVEN_RUN(start)) {
        /* LTR */
        /* the offset in runs[i] is visualIndex-runs[i-1].visualLimit */
        如果(i>0) {
            visualIndex-=runs[i-1].visualLimit;
        }
        返回 start+visualIndex;
    } 否则 {
        /* RTL */
        返回 GET_INDEX(start)+runs[i].visualLimit-visualIndex-1;
    }
}

U_CAPI void U_EXPORT2
ubidi_getLogicalMap(UBiDi *pBiDi, int32_t *indexMap, UErrorCode *pErrorCode) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_countRuns(pBiDi, pErrorCode);
    如果(U_FAILURE(*pErrorCode)) {
        /* no op */
    } 否则 如果(indexMap==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } 否则 {
        /* fill a logical-to-visual index map using the runs[] */
        int32_t visualStart, visualLimit, i, j, k;
        int32_t logicalStart, logicalLimit;
        Run *runs=pBiDi->runs;
        如果 (pBiDi->length<=0) {
            返回;
        }
        如果 (pBiDi->length>pBiDi->resultLength) {
            uprv_memset(indexMap, 0xFF, pBiDi->length*sizeof(int32_t));
        }

        visualStart=0;
        为(j=0; j<pBiDi->runCount; ++j) {
            logicalStart=GET_INDEX(runs[j].logicalStart);
            visualLimit=runs[j].visualLimit;
            如果(IS_EVEN_RUN(runs[j].logicalStart)) {
                do { /* LTR */
                    indexMap[logicalStart++]=visualStart++;
                } 当(visualStart<visualLimit);
            } 否则 {
                logicalStart+=visualLimit-visualStart;  /* logicalLimit */
                do { /* RTL */
                    indexMap[--logicalStart]=visualStart++;
                } 当(visualStart<visualLimit);
            }
            /* visualStart==visualLimit; */
        }

        如果(pBiDi->insertPoints.size>0) {
            int32_t markFound=0, runCount=pBiDi->runCount;
            int32_t length, insertRemove;
            visualStart=0;
            /* add number of marks found until each index */
            为(i=0; i<runCount; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                insertRemove=runs[i].insertRemove;
                如果(insertRemove&(LRM_BEFORE|RLM_BEFORE)) {
                    markFound++;
                }
                如果(markFound>0) {
                    logicalStart=GET_INDEX(runs[i].logicalStart);
                    logicalLimit=logicalStart+length;
                    为(j=logicalStart; j<logicalLimit; j++) {
                        indexMap[j]+=markFound;
                    }
                }
                如果(insertRemove&(LRM_AFTER|RLM_AFTER)) {
                    markFound++;
                }
            }
        }
        否则 如果(pBiDi->controlCount>0) {
            int32_t controlFound=0, runCount=pBiDi->runCount;
            int32_t length, insertRemove;
            UBool evenRun;
            char16_t uchar;
            visualStart=0;
            /* subtract number of controls found until each index */
            为(i=0; i<runCount; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                insertRemove=runs[i].insertRemove;
                /* no control found within previous runs nor within this run */
                如果((controlFound-insertRemove)==0) {
                    continue;
                }
                logicalStart=runs[i].logicalStart;
                evenRun=IS_EVEN_RUN(logicalStart);
                REMOVE_ODD_BIT(logicalStart);
                logicalLimit=logicalStart+length;
                /* if no control within this run */
                如果(insertRemove==0) {
                    为(j=logicalStart; j<logicalLimit; j++) {
                        indexMap[j]-=controlFound;
                    }
                    continue;
                }
                为(j=0; j<length; j++) {
                    k= evenRun ? logicalStart+j : logicalLimit-j-1;
                    uchar=pBiDi->text[k];
                    如果(IS_BIDI_CONTROL_CHAR(uchar)) {
                        controlFound++;
                        indexMap[k]=UBIDI_MAP_NOWHERE;
                        continue;
                    }
                    indexMap[k]-=controlFound;
                }
            }
        }
    }
}

U_CAPI void U_EXPORT2
ubidi_getVisualMap(UBiDi *pBiDi, int32_t *indexMap, UErrorCode *pErrorCode) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    如果(indexMap==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        返回;
    }
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_countRuns(pBiDi, pErrorCode);
    如果(U_SUCCESS(*pErrorCode)) {
        /* fill a visual-to-logical index map using the runs[] */
        Run *runs=pBiDi->runs, *runsLimit=runs+pBiDi->runCount;
        int32_t logicalStart, visualStart, visualLimit, *pi=indexMap;

        如果 (pBiDi->resultLength<=0) {
            返回;
        }
        visualStart=0;
        为(; runs<runsLimit; ++runs) {
            logicalStart=runs->logicalStart;
            visualLimit=runs->visualLimit;
            如果(IS_EVEN_RUN(logicalStart)) {
                do { /* LTR */
                    *pi++ = logicalStart++;
                } 当(++visualStart<visualLimit);
            } 否则 {
                REMOVE_ODD_BIT(logicalStart);
                logicalStart+=visualLimit-visualStart;  /* logicalLimit */
                do { /* RTL */
                    *pi++ = --logicalStart;
                } 当(++visualStart<visualLimit);
            }
            /* visualStart==visualLimit; */
        }

        如果(pBiDi->insertPoints.size>0) {
            int32_t markFound=0, runCount=pBiDi->runCount;
            int32_t insertRemove, i, j, k;
            runs=pBiDi->runs;
            /* count all inserted marks */
            为(i=0; i<runCount; i++) {
                insertRemove=runs[i].insertRemove;
                如果(insertRemove&(LRM_BEFORE|RLM_BEFORE)) {
                    markFound++;
                }
                如果(insertRemove&(LRM_AFTER|RLM_AFTER)) {
                    markFound++;
                }
            }
            /* move back indexes by number of preceding marks */
            k=pBiDi->resultLength;
            为(i=runCount-1; i>=0 && markFound>0; i--) {
                insertRemove=runs[i].insertRemove;
                如果(insertRemove&(LRM_AFTER|RLM_AFTER)) {
                    indexMap[--k]= UBIDI_MAP_NOWHERE;
                    markFound--;
                }
                visualStart= i>0 ? runs[i-1].visualLimit : 0;
                为(j=runs[i].visualLimit-1; j>=visualStart && markFound>0; j--) {
                    indexMap[--k]=indexMap[j];
                }
                如果(insertRemove&(LRM_BEFORE|RLM_BEFORE)) {
                    indexMap[--k]= UBIDI_MAP_NOWHERE;
                    markFound--;
                }
            }
        }
        否则 如果(pBiDi->controlCount>0) {
            int32_t runCount=pBiDi->runCount, logicalEnd;
            int32_t insertRemove, length, i, j, k, m;
            char16_t uchar;
            UBool evenRun;
            runs=pBiDi->runs;
            visualStart=0;
            /* move forward indexes by number of preceding controls */
            k=0;
            为(i=0; i<runCount; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                insertRemove=runs[i].insertRemove;
                /* if no control found yet, nothing to do in this run */
                如果((insertRemove==0)&&(k==visualStart)) {
                    k+=length;
                    continue;
                }
                /* if no control in this run */
                如果(insertRemove==0) {
                    visualLimit=runs[i].visualLimit;
                    为(j=visualStart; j<visualLimit; j++) {
                        indexMap[k++]=indexMap[j];
                    }
                    continue;
                }
                logicalStart=runs[i].logicalStart;
                evenRun=IS_EVEN_RUN(logicalStart);
                REMOVE_ODD_BIT(logicalStart);
                logicalEnd=logicalStart+length-1;
                为(j=0; j<length; j++) {
                    m= evenRun ? logicalStart+j : logicalEnd-j;
                    uchar=pBiDi->text[m];
                    如果(!IS_BIDI_CONTROL_CHAR(uchar)) {
                        indexMap[k++]=m;
                    }
                }
            }
        }
    }
}

U_CAPI void U_EXPORT2
ubidi_invertMap(const int32_t *srcMap, int32_t *destMap, int32_t length) {
    如果(srcMap!=nullptr && destMap!=nullptr && length>0) {
        const int32_t *pi;
        int32_t destLength=-1, count=0;
        /* find highest value and count positive indexes in srcMap */
        pi=srcMap+length;
        当(pi>srcMap) {
            如果(*--pi>destLength) {
                destLength=*pi;
            }
            如果(*pi>=0) {
                count++;
            }
        }
        destLength++;           /* add 1 for origin 0 */
        如果(count<destLength) {
            /* we must fill unmatched destMap entries with -1 */
            uprv_memset(destMap, 0xFF, destLength*sizeof(int32_t));
        }
        pi=srcMap+length;
        当(length>0) {
            如果(*--pi>=0) {
                destMap[*pi]=--length;
            } 否则 {
                --length;
            }
        }
    }
}
