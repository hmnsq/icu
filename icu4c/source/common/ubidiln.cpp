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

#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/ubidi.h"
#அந்தர்பாவ "ubidiimp.h"
#அந்தர்பாவ "uassert.h"

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
நிலையான வெறுமை
setTrailingWSStart(UBiDi *pBiDi) {
    /* pBiDi->direction!=UBIDI_MIXED */

    மாறிலி DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *levels=pBiDi->levels;
    int32_t start=pBiDi->length;
    UBiDiLevel paraLevel=pBiDi->paraLevel;

    /* If the line is terminated by a block separator, all preceding WS etc...
       are already set to paragraph level.
       Setting trailingWSStart to pBidi->length will avoid changing the
       level of B chars from 0 to paraLevel in ubidi_getLevels when
       orderParagraphsLTR==true.
     */
    எனில்(dirProps[start-1]==B) {
        pBiDi->trailingWSStart=start;   /* currently == pBiDi->length */
        திரும்பு;
    }
    /* go backwards across all WS, BN, explicit codes */
    போது(start>0 && DIRPROP_FLAG(dirProps[start-1])&MASK_WS) {
        --start;
    }

    /* if the WS run can be merged with the previous run then do so here */
    போது(start>0 && levels[start-1]==paraLevel) {
        --start;
    }

    pBiDi->trailingWSStart=start;
}

/* ubidi_setLine ------------------------------------------------------------ */

U_CAPI வெறுமை U_EXPORT2
ubidi_set_வரி(மாறிலி UBiDi *pParaBiDi,
              int32_t start, int32_t limit,
              UBiDi *p_வரி_Bi_Di,
              UErrorCode *pErrorCode) {
    int32_t length;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    RETURN_VOID_IF_NOT_VALID_PARA(pParaBiDi, *pErrorCode);
    RETURN_VOID_IF_BAD_RANGE(start, 0, limit, *pErrorCode);
    RETURN_VOID_IF_BAD_RANGE(limit, 0, pParaBiDi->length+1, *pErrorCode);
    எனில்(p_வரி_Bi_Di==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    எனில்(ubidi_getParagraph(pParaBiDi, start, nullptr, nullptr, nullptr, pErrorCode) !=
       ubidi_getParagraph(pParaBiDi, limit-1, nullptr, nullptr, nullptr, pErrorCode)) {
        /* the line crosses a paragraph boundary */
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    /* set the values in pLineBiDi from its pParaBiDi parent */
    p_வரி_Bi_Di->pParaBiDi=nullptr;          /* mark unfinished setLine */
    p_வரி_Bi_Di->text=pParaBiDi->text+start;
    length=p_வரி_Bi_Di->length=limit-start;
    p_வரி_Bi_Di->resultLength=p_வரி_Bi_Di->originalLength=length;
    p_வரி_Bi_Di->paraLevel=GET_PARALEVEL(pParaBiDi, start);
    p_வரி_Bi_Di->para_மொத்தம்=pParaBiDi->para_மொத்தம்;
    p_வரி_Bi_Di->runs=nullptr;
    p_வரி_Bi_Di->flags=0;
    p_வரி_Bi_Di->reorderingMode=pParaBiDi->reorderingMode;
    p_வரி_Bi_Di->reorderingOptions=pParaBiDi->reorderingOptions;
    p_வரி_Bi_Di->control_மொத்தம்=0;
    எனில்(pParaBiDi->control_மொத்தம்>0) {
        int32_t j;
        க்கு(j=start; j<limit; j++) {
            எனில்(IS_BIDI_CONTROL_CHAR(pParaBiDi->text[j])) {
                p_வரி_Bi_Di->control_மொத்தம்++;
            }
        }
        p_வரி_Bi_Di->resultLength-=p_வரி_Bi_Di->control_மொத்தம்;
    }

    p_வரி_Bi_Di->dirProps=pParaBiDi->dirProps+start;
    p_வரி_Bi_Di->levels=pParaBiDi->levels+start;
    p_வரி_Bi_Di->run_மொத்தம்=-1;

    எனில்(pParaBiDi->direction!=UBIDI_MIXED) {
        /* the parent is already trivial */
        p_வரி_Bi_Di->direction=pParaBiDi->direction;

        /*
         * The parent's levels are all either
         * implicitly or explicitly ==paraLevel;
         * do the same here.
         */
        எனில்(pParaBiDi->trailingWSStart<=start) {
            p_வரி_Bi_Di->trailingWSStart=0;
        } இல்லையெனில் எனில்(pParaBiDi->trailingWSStart<limit) {
            p_வரி_Bi_Di->trailingWSStart=pParaBiDi->trailingWSStart-start;
        } இல்லையெனில் {
            p_வரி_Bi_Di->trailingWSStart=length;
        }
    } இல்லையெனில் {
        மாறிலி UBiDiLevel *levels=p_வரி_Bi_Di->levels;
        int32_t i, trailingWSStart;
        UBiDiLevel level;

        setTrailingWSStart(p_வரி_Bi_Di);
        trailingWSStart=p_வரி_Bi_Di->trailingWSStart;

        /* recalculate pLineBiDi->direction */
        எனில்(trailingWSStart==0) {
            /* all levels are at paraLevel */
            p_வரி_Bi_Di->direction=(UBiDiDirection)(p_வரி_Bi_Di->paraLevel&1);
        } இல்லையெனில் {
            /* get the level of the first character */
            level=(UBiDiLevel)(levels[0]&1);

            /* if there is anything of a different level, then the line is mixed */
            எனில்(trailingWSStart<length && (p_வரி_Bi_Di->paraLevel&1)!=level) {
                /* the trailing WS is at paraLevel, which differs from levels[0] */
                p_வரி_Bi_Di->direction=UBIDI_MIXED;
            } இல்லையெனில் {
                /* see if levels[1..trailingWSStart-1] have the same direction as levels[0] and paraLevel */
                i=1;
                க்கு(;;) {
                    எனில்(i==trailingWSStart) {
                        /* the direction values match those in level */
                        p_வரி_Bi_Di->direction=(UBiDiDirection)level;
                        நிறுத்து;
                    } இல்லையெனில் எனில்((levels[i]&1)!=level) {
                        p_வரி_Bi_Di->direction=UBIDI_MIXED;
                        நிறுத்து;
                    }
                    ++i;
                }
            }
        }

        தேர்வு(p_வரி_Bi_Di->direction) {
        நிலை UBIDI_LTR:
            /* make sure paraLevel is even */
            p_வரி_Bi_Di->paraLevel=(UBiDiLevel)((p_வரி_Bi_Di->paraLevel+1)&~1);

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_வரி_Bi_Di->trailingWSStart=0;
            நிறுத்து;
        நிலை UBIDI_RTL:
            /* make sure paraLevel is odd */
            p_வரி_Bi_Di->paraLevel|=1;

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_வரி_Bi_Di->trailingWSStart=0;
            நிறுத்து;
        இயல்பு:
            நிறுத்து;
        }
    }
    p_வரி_Bi_Di->pParaBiDi=pParaBiDi;     /* mark successful setLine */
}

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getLevelAt(மாறிலி UBiDi *pBiDi, int32_t charIndex) {
    /* return paraLevel if in the trailing WS run, otherwise the real level */
    எனில்(!IS_VALID_PARA_OR_வரி(pBiDi) || charIndex<0 || pBiDi->length<=charIndex) {
        திரும்பு 0;
    } இல்லையெனில் எனில்(pBiDi->direction!=UBIDI_MIXED || charIndex>=pBiDi->trailingWSStart) {
        திரும்பு GET_PARALEVEL(pBiDi, charIndex);
    } இல்லையெனில் {
        திரும்பு pBiDi->levels[charIndex];
    }
}

U_CAPI மாறிலி UBiDiLevel* U_EXPORT2
ubidi_getLevels(UBiDi* pBiDi, UErrorCode* pErrorCode) {
    int32_t start, length;

    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, nullptr);
    RETURN_IF_NOT_VALID_PARA_OR_வரி(pBiDi, *pErrorCode, nullptr);
    எனில்((length=pBiDi->length)<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    எனில்((start=pBiDi->trailingWSStart)==length) {
        /* the current levels array reflects the WS run */
        திரும்பு pBiDi->levels;
    }

    /*
     * After the previous if(), we know that the levels array
     * has an implicit trailing WS run and therefore does not fully
     * reflect itself all the levels.
     * This must be a UBiDi object for a line, and
     * we need to create a new levels array.
     */
    எனில்(getLevelsMemory(pBiDi, length)) {
        UBiDiLevel *levels=pBiDi->levelsMemory;

        எனில்(start>0 && levels!=pBiDi->levels) {
            uprv_memcpy(levels, pBiDi->levels, start);
        }
        /* pBiDi->paraLevel is ok even if contextual multiple paragraphs,
           since pBidi is a line object                                     */
        uprv_memset(levels+start, pBiDi->paraLevel, length-start);

        /* this new levels array is set for the line and reflects the WS run */
        pBiDi->trailingWSStart=length;
        திரும்பு pBiDi->levels=levels;
    } இல்லையெனில் {
        /* out of memory */
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
}

U_CAPI வெறுமை U_EXPORT2
ubidi_getLogicalRun(மாறிலி UBiDi *pBiDi, int32_t logicalPosition,
                    int32_t *pLogicalLimit, UBiDiLevel *pLevel) {
    UErrorCode errorCode;
    int32_t run_மொத்தம், visualStart, logicalLimit, logicalFirst, i;
    Run iRun;

    errorCode=U_ZERO_ERROR;
    RETURN_VOID_IF_BAD_RANGE(logicalPosition, 0, pBiDi->length, errorCode);
    /* ubidi_countRuns will check VALID_PARA_OR_LINE */
    run_மொத்தம்=ubidi_மொத்தம்_Runs((UBiDi *)pBiDi, &errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    /* this is done based on runs rather than on levels since levels have
       a special interpretation when UBIDI_REORDER_RUNS_ONLY
     */
    visualStart=logicalLimit=0;
    iRun=pBiDi->runs[0];

    க்கு(i=0; i<run_மொத்தம்; i++) {
        iRun = pBiDi->runs[i];
        logicalFirst=GET_INDEX(iRun.logicalStart);
        logicalLimit=logicalFirst+iRun.visualLimit-visualStart;
        எனில்((logicalPosition>=logicalFirst) &&
           (logicalPosition<logicalLimit)) {
            நிறுத்து;
        }
        visualStart = iRun.visualLimit;
    }
    எனில்(pLogicalLimit) {
        *pLogicalLimit=logicalLimit;
    }
    எனில்(pLevel) {
        எனில்(pBiDi->reorderingMode==UBIDI_REORDER_RUNS_ONLY) {
            *pLevel=(UBiDiLevel)GET_ODD_BIT(iRun.logicalStart);
        }
        இல்லையெனில் எனில்(pBiDi->direction!=UBIDI_MIXED || logicalPosition>=pBiDi->trailingWSStart) {
            *pLevel=GET_PARALEVEL(pBiDi, logicalPosition);
        } இல்லையெனில் {
        *pLevel=pBiDi->levels[logicalPosition];
        }
    }
}

/* runs API functions ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_மொத்தம்_Runs(UBiDi *pBiDi, UErrorCode *pErrorCode) {
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_வரி(pBiDi, *pErrorCode, -1);
    ubidi_getRuns(pBiDi, pErrorCode);
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு -1;
    }
    திரும்பு pBiDi->run_மொத்தம்;
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_getVisualRun(UBiDi *pBiDi, int32_t runIndex,
                   int32_t *pLogicalStart, int32_t *pLength)
{
    int32_t start;
    UErrorCode errorCode = U_ZERO_ERROR;
    RETURN_IF_NOT_VALID_PARA_OR_வரி(pBiDi, errorCode, UBIDI_LTR);
    ubidi_getRuns(pBiDi, &errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு UBIDI_LTR;
    }
    RETURN_IF_BAD_RANGE(runIndex, 0, pBiDi->run_மொத்தம், errorCode, UBIDI_LTR);

    start=pBiDi->runs[runIndex].logicalStart;
    எனில்(pLogicalStart!=nullptr) {
        *pLogicalStart=GET_INDEX(start);
    }
    எனில்(pLength!=nullptr) {
        எனில்(runIndex>0) {
            *pLength=pBiDi->runs[runIndex].visualLimit-
                     pBiDi->runs[runIndex-1].visualLimit;
        } இல்லையெனில் {
            *pLength=pBiDi->runs[0].visualLimit;
        }
    }
    திரும்பு (UBiDiDirection)GET_ODD_BIT(start);
}

/* in trivial cases there is only one trivial run; called by ubidi_getRuns() */
நிலையான வெறுமை
getSingleRun(UBiDi *pBiDi, UBiDiLevel level) {
    /* simple, single-run case */
    pBiDi->runs=pBiDi->simpleRuns;
    pBiDi->run_மொத்தம்=1;

    /* fill and reorder the single run */
    pBiDi->runs[0].logicalStart=MAKE_INDEX_ODD_PAIR(0, level);
    pBiDi->runs[0].visualLimit=pBiDi->length;
    pBiDi->runs[0].செருகு_Remove=0;
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
நிலையான வெறுமை
reorder_வரி(UBiDi *pBiDi, UBiDiLevel minLevel, UBiDiLevel maxLevel) {
    Run *runs, tempRun;
    UBiDiLevel *levels;
    int32_t firstRun, endRun, limitRun, run_மொத்தம்;

    /* nothing to do? */
    எனில்(maxLevel<=(minLevel|1)) {
        திரும்பு;
    }

    /*
     * Reorder only down to the lowest odd level
     * and reorder at an odd minLevel in a separate, simpler loop.
     * See comments above for why minLevel is always incremented.
     */
    ++minLevel;

    runs=pBiDi->runs;
    levels=pBiDi->levels;
    run_மொத்தம்=pBiDi->run_மொத்தம்;

    /* do not include the WS run at paraLevel<=old minLevel except in the simple loop */
    எனில்(pBiDi->trailingWSStart<pBiDi->length) {
        --run_மொத்தம்;
    }

    போது(--maxLevel>=minLevel) {
        firstRun=0;

        /* loop for all sequences of runs */
        க்கு(;;) {
            /* look for a sequence of runs that are all at >=maxLevel */
            /* look for the first run of such a sequence */
            போது(firstRun<run_மொத்தம் && levels[runs[firstRun].logicalStart]<maxLevel) {
                ++firstRun;
            }
            எனில்(firstRun>=run_மொத்தம்) {
                நிறுத்து;  /* no more such runs */
            }

            /* look for the limit run of such a sequence (the run behind it) */
            க்கு(limitRun=firstRun; ++limitRun<run_மொத்தம் && levels[runs[limitRun].logicalStart]>=maxLevel;) {}

            /* Swap the entire sequence of runs from firstRun to limitRun-1. */
            endRun=limitRun-1;
            போது(firstRun<endRun) {
                tempRun = runs[firstRun];
                runs[firstRun]=runs[endRun];
                runs[endRun]=tempRun;
                ++firstRun;
                --endRun;
            }

            எனில்(limitRun==run_மொத்தம்) {
                நிறுத்து;  /* no more such runs */
            } இல்லையெனில் {
                firstRun=limitRun+1;
            }
        }
    }

    /* now do maxLevel==old minLevel (==odd!), see above */
    எனில்(!(minLevel&1)) {
        firstRun=0;

        /* include the trailing WS run in this complete reordering */
        எனில்(pBiDi->trailingWSStart==pBiDi->length) {
            --run_மொத்தம்;
        }

        /* Swap the entire sequence of all runs. (endRun==runCount) */
        போது(firstRun<run_மொத்தம்) {
            tempRun=runs[firstRun];
            runs[firstRun]=runs[run_மொத்தம்];
            runs[run_மொத்தம்]=tempRun;
            ++firstRun;
            --run_மொத்தம்;
        }
    }
}

/* compute the runs array --------------------------------------------------- */

நிலையான int32_t getRunFromLogicalIndex(UBiDi *pBiDi, int32_t logicalIndex) {
    Run *runs=pBiDi->runs;
    int32_t run_மொத்தம்=pBiDi->run_மொத்தம், visualStart=0, i, length, logicalStart;

    க்கு(i=0; i<run_மொத்தம்; i++) {
        length=runs[i].visualLimit-visualStart;
        logicalStart=GET_INDEX(runs[i].logicalStart);
        எனில்((logicalIndex>=logicalStart) && (logicalIndex<(logicalStart+length))) {
            திரும்பு i;
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
    எனில் (pBiDi->run_மொத்தம்>=0) {
        திரும்பு உண்மை;
    }

    எனில்(pBiDi->direction!=UBIDI_MIXED) {
        /* simple, single-run case - this covers length==0 */
        /* pBiDi->paraLevel is ok even for contextual multiple paragraphs */
        getSingleRun(pBiDi, pBiDi->paraLevel);
    } இல்லையெனில் /* UBIDI_MIXED, length>0 */ {
        /* mixed directionality */
        int32_t length=pBiDi->length, limit;
        UBiDiLevel *levels=pBiDi->levels;
        int32_t i, run_மொத்தம்;
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
        run_மொத்தம்=0;
        க்கு(i=0; i<limit; ++i) {
            /* increment runCount at the start of each run */
            எனில்(levels[i]!=level) {
                ++run_மொத்தம்;
                level=levels[i];
            }
        }

        /*
         * We don't need to see if the last run can be merged with a trailing
         * WS run because setTrailingWSStart() would have done that.
         */
        எனில்(run_மொத்தம்==1 && limit==length) {
            /* There is only one non-WS run and no trailing WS-run. */
            getSingleRun(pBiDi, levels[0]);
        } இல்லையெனில் /* runCount>1 || limit<length */ {
            /* allocate and set the runs */
            Run *runs;
            int32_t runIndex, start;
            UBiDiLevel minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1, maxLevel=0;

            /* now, count a (non-mergeable) WS run */
            எனில்(limit<length) {
                ++run_மொத்தம்;
            }

            /* runCount>1 */
            எனில்(getRunsMemory(pBiDi, run_மொத்தம்)) {
                runs=pBiDi->runsMemory;
            } இல்லையெனில் {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு பொய்;
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
            செய் {
                /* prepare this run */
                start=i;
                level=levels[i];
                எனில்(level<minLevel) {
                    minLevel=level;
                }
                எனில்(level>maxLevel) {
                    maxLevel=level;
                }

                /* look for the run limit */
                போது(++i<limit && levels[i]==level) {}

                /* i is another run limit */
                runs[runIndex].logicalStart=start;
                runs[runIndex].visualLimit=i-start;
                runs[runIndex].செருகு_Remove=0;
                ++runIndex;
            } போது(i<limit);

            எனில்(limit<length) {
                /* there is a separate WS run */
                runs[runIndex].logicalStart=limit;
                runs[runIndex].visualLimit=length-limit;
                /* For the trailing WS run, pBiDi->paraLevel is ok even
                   if contextual multiple paragraphs.                   */
                எனில்(pBiDi->paraLevel<minLevel) {
                    minLevel=pBiDi->paraLevel;
                }
            }

            /* set the object fields */
            pBiDi->runs=runs;
            pBiDi->run_மொத்தம்=run_மொத்தம்;

            reorder_வரி(pBiDi, minLevel, maxLevel);

            /* now add the direction flags and adjust the visualLimit's to be just that */
            /* this loop will also handle the trailing WS run */
            limit=0;
            க்கு(i=0; i<run_மொத்தம்; ++i) {
                ADD_ODD_BIT_FROM_LEVEL(runs[i].logicalStart, levels[runs[i].logicalStart]);
                limit+=runs[i].visualLimit;
                runs[i].visualLimit=limit;
            }

            /* Set the "odd" bit for the trailing WS run. */
            /* For a RTL paragraph, it will be the *first* run in visual order. */
            /* For the trailing WS run, pBiDi->paraLevel is ok even if
               contextual multiple paragraphs.                          */
            எனில்(runIndex<run_மொத்தம்) {
                int32_t trailingRun = ((pBiDi->paraLevel & 1) != 0)? 0 : runIndex;

                ADD_ODD_BIT_FROM_LEVEL(runs[trailingRun].logicalStart, pBiDi->paraLevel);
            }
        }
    }

    /* handle insert LRM/RLM BEFORE/AFTER run */
    எனில்(pBiDi->செருகு_Points.size>0) {
        Point *point, *start=pBiDi->செருகு_Points.points,
                      *limit=start+pBiDi->செருகு_Points.size;
        int32_t runIndex;
        க்கு(point=start; point<limit; point++) {
            runIndex=getRunFromLogicalIndex(pBiDi, point->pos);
            pBiDi->runs[runIndex].செருகு_Remove|=point->flag;
        }
    }

    /* handle remove BiDi control characters */
    எனில்(pBiDi->control_மொத்தம்>0) {
        int32_t runIndex;
        மாறிலி char16_t *start=pBiDi->text, *limit=start+pBiDi->length, *pu;
        க்கு(pu=start; pu<limit; pu++) {
            எனில்(IS_BIDI_CONTROL_CHAR(*pu)) {
                runIndex=getRunFromLogicalIndex(pBiDi, (int32_t)(pu-start));
                pBiDi->runs[runIndex].செருகு_Remove--;
            }
        }
    }

    திரும்பு உண்மை;
}

நிலையான UBool
prepareReorder(மாறிலி UBiDiLevel *levels, int32_t length,
               int32_t *indexMap,
               UBiDiLevel *pMinLevel, UBiDiLevel *pMaxLevel) {
    int32_t start;
    UBiDiLevel level, minLevel, maxLevel;

    எனில்(levels==nullptr || length<=0) {
        திரும்பு பொய்;
    }

    /* determine minLevel and maxLevel */
    minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1;
    maxLevel=0;
    க்கு(start=length; start>0;) {
        level=levels[--start];
        எனில்(level>UBIDI_MAX_EXPLICIT_LEVEL+1) {
            திரும்பு பொய்;
        }
        எனில்(level<minLevel) {
            minLevel=level;
        }
        எனில்(level>maxLevel) {
            maxLevel=level;
        }
    }
    *pMinLevel=minLevel;
    *pMaxLevel=maxLevel;

    /* initialize the index map */
    க்கு(start=length; start>0;) {
        --start;
        indexMap[start]=start;
    }

    திரும்பு உண்மை;
}

/* reorder a line based on a levels array (L2) ------------------------------ */

U_CAPI வெறுமை U_EXPORT2
ubidi_reorderLogical(மாறிலி UBiDiLevel *levels, int32_t length, int32_t *indexMap) {
    int32_t start, limit, sumOfSosEos;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    எனில்(indexMap==nullptr || !prepareReorder(levels, length, indexMap, &minLevel, &maxLevel)) {
        திரும்பு;
    }

    /* nothing to do? */
    எனில்(minLevel==maxLevel && (minLevel&1)==0) {
        திரும்பு;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    செய் {
        start=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        க்கு(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            போது(start<length && levels[start]<maxLevel) {
                ++start;
            }
            எனில்(start>=length) {
                நிறுத்து;  /* no more such sequences */
            }

            /* look for the limit of such a sequence (the index behind it) */
            க்கு(limit=start; ++limit<length && levels[limit]>=maxLevel;) {}

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
            செய் {
                indexMap[start]=sumOfSosEos-indexMap[start];
            } போது(++start<limit);

            /* start==limit */
            எனில்(limit==length) {
                நிறுத்து;  /* no more such sequences */
            } இல்லையெனில் {
                start=limit+1;
            }
        }
    } போது(--maxLevel>=minLevel);
}

U_CAPI வெறுமை U_EXPORT2
ubidi_reorderVisual(மாறிலி UBiDiLevel *levels, int32_t length, int32_t *indexMap) {
    int32_t start, end, limit, temp;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    எனில்(indexMap==nullptr || !prepareReorder(levels, length, indexMap, &minLevel, &maxLevel)) {
        திரும்பு;
    }

    /* nothing to do? */
    எனில்(minLevel==maxLevel && (minLevel&1)==0) {
        திரும்பு;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    செய் {
        start=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        க்கு(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            போது(start<length && levels[start]<maxLevel) {
                ++start;
            }
            எனில்(start>=length) {
                நிறுத்து;  /* no more such runs */
            }

            /* look for the limit of such a sequence (the index behind it) */
            க்கு(limit=start; ++limit<length && levels[limit]>=maxLevel;) {}

            /*
             * Swap the entire interval of indexes from start to limit-1.
             * We don't need to swap the levels for the purpose of this
             * algorithm: the sequence of levels that we look at does not
             * move anyway.
             */
            end=limit-1;
            போது(start<end) {
                temp=indexMap[start];
                indexMap[start]=indexMap[end];
                indexMap[end]=temp;

                ++start;
                --end;
            }

            எனில்(limit==length) {
                நிறுத்து;  /* no more such sequences */
            } இல்லையெனில் {
                start=limit+1;
            }
        }
    } போது(--maxLevel>=minLevel);
}

/* API functions for logical<->visual mapping ------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_getVisualIndex(UBiDi *pBiDi, int32_t logicalIndex, UErrorCode *pErrorCode) {
    int32_t visualIndex=UBIDI_MAP_NOWHERE;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_வரி(pBiDi, *pErrorCode, -1);
    RETURN_IF_BAD_RANGE(logicalIndex, 0, pBiDi->length, *pErrorCode, -1);

    /* we can do the trivial cases without the runs array */
    தேர்வு(pBiDi->direction) {
    நிலை UBIDI_LTR:
        visualIndex=logicalIndex;
        நிறுத்து;
    நிலை UBIDI_RTL:
        visualIndex=pBiDi->length-logicalIndex-1;
        நிறுத்து;
    இயல்பு:
        எனில்(!ubidi_getRuns(pBiDi, pErrorCode)) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            திரும்பு -1;
        } இல்லையெனில் {
            Run *runs=pBiDi->runs;
            int32_t i, visualStart=0, offset, length;

            /* linear search for the run, search on the visual runs */
            க்கு(i=0; i<pBiDi->run_மொத்தம்; ++i) {
                length=runs[i].visualLimit-visualStart;
                offset=logicalIndex-GET_INDEX(runs[i].logicalStart);
                எனில்(offset>=0 && offset<length) {
                    எனில்(IS_EVEN_RUN(runs[i].logicalStart)) {
                        /* LTR */
                        visualIndex=visualStart+offset;
                    } இல்லையெனில் {
                        /* RTL */
                        visualIndex=visualStart+length-offset-1;
                    }
                    நிறுத்து;          /* exit for loop */
                }
                visualStart+=length;
            }
            எனில்(i>=pBiDi->run_மொத்தம்) {
                திரும்பு UBIDI_MAP_NOWHERE;
            }
        }
    }

    எனில்(pBiDi->செருகு_Points.size>0) {
        /* add the number of added marks until the calculated visual index */
        Run *runs=pBiDi->runs;
        int32_t i, length, செருகு_Remove;
        int32_t visualStart=0, markFound=0;
        க்கு(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            செருகு_Remove=runs[i].செருகு_Remove;
            எனில்(செருகு_Remove & (LRM_BEFORE|RLM_BEFORE)) {
                markFound++;
            }
            /* is it the run containing the visual index? */
            எனில்(visualIndex<runs[i].visualLimit) {
                திரும்பு visualIndex+markFound;
            }
            எனில்(செருகு_Remove & (LRM_AFTER|RLM_AFTER)) {
                markFound++;
            }
        }
    }
    இல்லையெனில் எனில்(pBiDi->control_மொத்தம்>0) {
        /* subtract the number of controls until the calculated visual index */
        Run *runs=pBiDi->runs;
        int32_t i, j, start, limit, length, செருகு_Remove;
        int32_t visualStart=0, controlFound=0;
        char16_t uchar=pBiDi->text[logicalIndex];
        /* is the logical index pointing to a control ? */
        எனில்(IS_BIDI_CONTROL_CHAR(uchar)) {
            திரும்பு UBIDI_MAP_NOWHERE;
        }
        /* loop on runs */
        க்கு(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            செருகு_Remove=runs[i].செருகு_Remove;
            /* calculated visual index is beyond this run? */
            எனில்(visualIndex>=runs[i].visualLimit) {
                controlFound-=செருகு_Remove;
                தொடர்;
            }
            /* calculated visual index must be within current run */
            எனில்(செருகு_Remove==0) {
                திரும்பு visualIndex-controlFound;
            }
            எனில்(IS_EVEN_RUN(runs[i].logicalStart)) {
                /* LTR: check from run start to logical index */
                start=runs[i].logicalStart;
                limit=logicalIndex;
            } இல்லையெனில் {
                /* RTL: check from logical index to run end */
                start=logicalIndex+1;
                limit=GET_INDEX(runs[i].logicalStart)+length;
            }
            க்கு(j=start; j<limit; j++) {
                uchar=pBiDi->text[j];
                எனில்(IS_BIDI_CONTROL_CHAR(uchar)) {
                    controlFound++;
                }
            }
            திரும்பு visualIndex-controlFound;
        }
    }

    திரும்பு visualIndex;
}

U_CAPI int32_t U_EXPORT2
ubidi_getLogicalIndex(UBiDi *pBiDi, int32_t visualIndex, UErrorCode *pErrorCode) {
    Run *runs;
    int32_t i, run_மொத்தம், start;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_வரி(pBiDi, *pErrorCode, -1);
    RETURN_IF_BAD_RANGE(visualIndex, 0, pBiDi->resultLength, *pErrorCode, -1);
    /* we can do the trivial cases without the runs array */
    எனில்(pBiDi->செருகு_Points.size==0 && pBiDi->control_மொத்தம்==0) {
        எனில்(pBiDi->direction==UBIDI_LTR) {
            திரும்பு visualIndex;
        }
        இல்லையெனில் எனில்(pBiDi->direction==UBIDI_RTL) {
            திரும்பு pBiDi->length-visualIndex-1;
        }
    }
    எனில்(!ubidi_getRuns(pBiDi, pErrorCode)) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு -1;
    }

    runs=pBiDi->runs;
    run_மொத்தம்=pBiDi->run_மொத்தம்;
    எனில்(pBiDi->செருகு_Points.size>0) {
        /* handle inserted LRM/RLM */
        int32_t markFound=0, செருகு_Remove;
        int32_t visualStart=0, length;
        runs=pBiDi->runs;
        /* subtract number of marks until visual index */
        க்கு(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            செருகு_Remove=runs[i].செருகு_Remove;
            எனில்(செருகு_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                எனில்(visualIndex<=(visualStart+markFound)) {
                    திரும்பு UBIDI_MAP_NOWHERE;
                }
                markFound++;
            }
            /* is adjusted visual index within this run? */
            எனில்(visualIndex<(runs[i].visualLimit+markFound)) {
                visualIndex-=markFound;
                நிறுத்து;
            }
            எனில்(செருகு_Remove&(LRM_AFTER|RLM_AFTER)) {
                எனில்(visualIndex==(visualStart+length+markFound)) {
                    திரும்பு UBIDI_MAP_NOWHERE;
                }
                markFound++;
            }
        }
    }
    இல்லையெனில் எனில்(pBiDi->control_மொத்தம்>0) {
        /* handle removed BiDi control characters */
        int32_t controlFound=0, செருகு_Remove, length;
        int32_t logicalStart, logicalEnd, visualStart=0, j, k;
        char16_t uchar;
        UBool evenRun;
        /* add number of controls until visual index */
        க்கு(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            செருகு_Remove=runs[i].செருகு_Remove;
            /* is adjusted visual index beyond current run? */
            எனில்(visualIndex>=(runs[i].visualLimit-controlFound+செருகு_Remove)) {
                controlFound-=செருகு_Remove;
                தொடர்;
            }
            /* adjusted visual index is within current run */
            எனில்(செருகு_Remove==0) {
                visualIndex+=controlFound;
                நிறுத்து;
            }
            /* count non-control chars until visualIndex */
            logicalStart=runs[i].logicalStart;
            evenRun=IS_EVEN_RUN(logicalStart);
            REMOVE_ODD_BIT(logicalStart);
            logicalEnd=logicalStart+length-1;
            க்கு(j=0; j<length; j++) {
                k= evenRun ? logicalStart+j : logicalEnd-j;
                uchar=pBiDi->text[k];
                எனில்(IS_BIDI_CONTROL_CHAR(uchar)) {
                    controlFound++;
                }
                எனில்((visualIndex+controlFound)==(visualStart+j)) {
                    நிறுத்து;
                }
            }
            visualIndex+=controlFound;
            நிறுத்து;
        }
    }
    /* handle all cases */
    எனில்(run_மொத்தம்<=10) {
        /* linear search for the run */
        க்கு(i=0; visualIndex>=runs[i].visualLimit; ++i) {}
    } இல்லையெனில் {
        /* binary search for the run */
        int32_t begin=0, limit=run_மொத்தம்;

        /* the middle if() is guaranteed to find the run, we don't need a loop limit */
        க்கு(;;) {
            i=(begin+limit)/2;
            எனில்(visualIndex>=runs[i].visualLimit) {
                begin=i+1;
            } இல்லையெனில் எனில்(i==0 || visualIndex>=runs[i-1].visualLimit) {
                நிறுத்து;
            } இல்லையெனில் {
                limit=i;
            }
        }
    }

    start=runs[i].logicalStart;
    எனில்(IS_EVEN_RUN(start)) {
        /* LTR */
        /* the offset in runs[i] is visualIndex-runs[i-1].visualLimit */
        எனில்(i>0) {
            visualIndex-=runs[i-1].visualLimit;
        }
        திரும்பு start+visualIndex;
    } இல்லையெனில் {
        /* RTL */
        திரும்பு GET_INDEX(start)+runs[i].visualLimit-visualIndex-1;
    }
}

U_CAPI வெறுமை U_EXPORT2
ubidi_getLogicalMap(UBiDi *pBiDi, int32_t *indexMap, UErrorCode *pErrorCode) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_மொத்தம்_Runs(pBiDi, pErrorCode);
    எனில்(U_FAILURE(*pErrorCode)) {
        /* no op */
    } இல்லையெனில் எனில்(indexMap==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } இல்லையெனில் {
        /* fill a logical-to-visual index map using the runs[] */
        int32_t visualStart, visualLimit, i, j, k;
        int32_t logicalStart, logicalLimit;
        Run *runs=pBiDi->runs;
        எனில் (pBiDi->length<=0) {
            திரும்பு;
        }
        எனில் (pBiDi->length>pBiDi->resultLength) {
            uprv_memset(indexMap, 0xFF, pBiDi->length*அளவு(int32_t));
        }

        visualStart=0;
        க்கு(j=0; j<pBiDi->run_மொத்தம்; ++j) {
            logicalStart=GET_INDEX(runs[j].logicalStart);
            visualLimit=runs[j].visualLimit;
            எனில்(IS_EVEN_RUN(runs[j].logicalStart)) {
                செய் { /* LTR */
                    indexMap[logicalStart++]=visualStart++;
                } போது(visualStart<visualLimit);
            } இல்லையெனில் {
                logicalStart+=visualLimit-visualStart;  /* logicalLimit */
                செய் { /* RTL */
                    indexMap[--logicalStart]=visualStart++;
                } போது(visualStart<visualLimit);
            }
            /* visualStart==visualLimit; */
        }

        எனில்(pBiDi->செருகு_Points.size>0) {
            int32_t markFound=0, run_மொத்தம்=pBiDi->run_மொத்தம்;
            int32_t length, செருகு_Remove;
            visualStart=0;
            /* add number of marks found until each index */
            க்கு(i=0; i<run_மொத்தம்; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                செருகு_Remove=runs[i].செருகு_Remove;
                எனில்(செருகு_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    markFound++;
                }
                எனில்(markFound>0) {
                    logicalStart=GET_INDEX(runs[i].logicalStart);
                    logicalLimit=logicalStart+length;
                    க்கு(j=logicalStart; j<logicalLimit; j++) {
                        indexMap[j]+=markFound;
                    }
                }
                எனில்(செருகு_Remove&(LRM_AFTER|RLM_AFTER)) {
                    markFound++;
                }
            }
        }
        இல்லையெனில் எனில்(pBiDi->control_மொத்தம்>0) {
            int32_t controlFound=0, run_மொத்தம்=pBiDi->run_மொத்தம்;
            int32_t length, செருகு_Remove;
            UBool evenRun;
            char16_t uchar;
            visualStart=0;
            /* subtract number of controls found until each index */
            க்கு(i=0; i<run_மொத்தம்; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                செருகு_Remove=runs[i].செருகு_Remove;
                /* no control found within previous runs nor within this run */
                எனில்((controlFound-செருகு_Remove)==0) {
                    தொடர்;
                }
                logicalStart=runs[i].logicalStart;
                evenRun=IS_EVEN_RUN(logicalStart);
                REMOVE_ODD_BIT(logicalStart);
                logicalLimit=logicalStart+length;
                /* if no control within this run */
                எனில்(செருகு_Remove==0) {
                    க்கு(j=logicalStart; j<logicalLimit; j++) {
                        indexMap[j]-=controlFound;
                    }
                    தொடர்;
                }
                க்கு(j=0; j<length; j++) {
                    k= evenRun ? logicalStart+j : logicalLimit-j-1;
                    uchar=pBiDi->text[k];
                    எனில்(IS_BIDI_CONTROL_CHAR(uchar)) {
                        controlFound++;
                        indexMap[k]=UBIDI_MAP_NOWHERE;
                        தொடர்;
                    }
                    indexMap[k]-=controlFound;
                }
            }
        }
    }
}

U_CAPI வெறுமை U_EXPORT2
ubidi_getVisualMap(UBiDi *pBiDi, int32_t *indexMap, UErrorCode *pErrorCode) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    எனில்(indexMap==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_மொத்தம்_Runs(pBiDi, pErrorCode);
    எனில்(U_SUCCESS(*pErrorCode)) {
        /* fill a visual-to-logical index map using the runs[] */
        Run *runs=pBiDi->runs, *runsLimit=runs+pBiDi->run_மொத்தம்;
        int32_t logicalStart, visualStart, visualLimit, *pi=indexMap;

        எனில் (pBiDi->resultLength<=0) {
            திரும்பு;
        }
        visualStart=0;
        க்கு(; runs<runsLimit; ++runs) {
            logicalStart=runs->logicalStart;
            visualLimit=runs->visualLimit;
            எனில்(IS_EVEN_RUN(logicalStart)) {
                செய் { /* LTR */
                    *pi++ = logicalStart++;
                } போது(++visualStart<visualLimit);
            } இல்லையெனில் {
                REMOVE_ODD_BIT(logicalStart);
                logicalStart+=visualLimit-visualStart;  /* logicalLimit */
                செய் { /* RTL */
                    *pi++ = --logicalStart;
                } போது(++visualStart<visualLimit);
            }
            /* visualStart==visualLimit; */
        }

        எனில்(pBiDi->செருகு_Points.size>0) {
            int32_t markFound=0, run_மொத்தம்=pBiDi->run_மொத்தம்;
            int32_t செருகு_Remove, i, j, k;
            runs=pBiDi->runs;
            /* count all inserted marks */
            க்கு(i=0; i<run_மொத்தம்; i++) {
                செருகு_Remove=runs[i].செருகு_Remove;
                எனில்(செருகு_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    markFound++;
                }
                எனில்(செருகு_Remove&(LRM_AFTER|RLM_AFTER)) {
                    markFound++;
                }
            }
            /* move back indexes by number of preceding marks */
            k=pBiDi->resultLength;
            க்கு(i=run_மொத்தம்-1; i>=0 && markFound>0; i--) {
                செருகு_Remove=runs[i].செருகு_Remove;
                எனில்(செருகு_Remove&(LRM_AFTER|RLM_AFTER)) {
                    indexMap[--k]= UBIDI_MAP_NOWHERE;
                    markFound--;
                }
                visualStart= i>0 ? runs[i-1].visualLimit : 0;
                க்கு(j=runs[i].visualLimit-1; j>=visualStart && markFound>0; j--) {
                    indexMap[--k]=indexMap[j];
                }
                எனில்(செருகு_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    indexMap[--k]= UBIDI_MAP_NOWHERE;
                    markFound--;
                }
            }
        }
        இல்லையெனில் எனில்(pBiDi->control_மொத்தம்>0) {
            int32_t run_மொத்தம்=pBiDi->run_மொத்தம், logicalEnd;
            int32_t செருகு_Remove, length, i, j, k, m;
            char16_t uchar;
            UBool evenRun;
            runs=pBiDi->runs;
            visualStart=0;
            /* move forward indexes by number of preceding controls */
            k=0;
            க்கு(i=0; i<run_மொத்தம்; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                செருகு_Remove=runs[i].செருகு_Remove;
                /* if no control found yet, nothing to do in this run */
                எனில்((செருகு_Remove==0)&&(k==visualStart)) {
                    k+=length;
                    தொடர்;
                }
                /* if no control in this run */
                எனில்(செருகு_Remove==0) {
                    visualLimit=runs[i].visualLimit;
                    க்கு(j=visualStart; j<visualLimit; j++) {
                        indexMap[k++]=indexMap[j];
                    }
                    தொடர்;
                }
                logicalStart=runs[i].logicalStart;
                evenRun=IS_EVEN_RUN(logicalStart);
                REMOVE_ODD_BIT(logicalStart);
                logicalEnd=logicalStart+length-1;
                க்கு(j=0; j<length; j++) {
                    m= evenRun ? logicalStart+j : logicalEnd-j;
                    uchar=pBiDi->text[m];
                    எனில்(!IS_BIDI_CONTROL_CHAR(uchar)) {
                        indexMap[k++]=m;
                    }
                }
            }
        }
    }
}

U_CAPI வெறுமை U_EXPORT2
ubidi_invertMap(மாறிலி int32_t *srcMap, int32_t *destMap, int32_t length) {
    எனில்(srcMap!=nullptr && destMap!=nullptr && length>0) {
        மாறிலி int32_t *pi;
        int32_t destLength=-1, count=0;
        /* find highest value and count positive indexes in srcMap */
        pi=srcMap+length;
        போது(pi>srcMap) {
            எனில்(*--pi>destLength) {
                destLength=*pi;
            }
            எனில்(*pi>=0) {
                count++;
            }
        }
        destLength++;           /* add 1 for origin 0 */
        எனில்(count<destLength) {
            /* we must fill unmatched destMap entries with -1 */
            uprv_memset(destMap, 0xFF, destLength*அளவு(int32_t));
        }
        pi=srcMap+length;
        போது(length>0) {
            எனில்(*--pi>=0) {
                destMap[*pi]=--length;
            } இல்லையெனில் {
                --length;
            }
        }
    }
}
