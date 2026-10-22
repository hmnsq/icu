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

#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/ubidi.h"
#അന്തര്ഭാവ "ubidiimp.h"
#അന്തര്ഭാവ "uassert.h"

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
സ്ഥിതം ശൂന്യം
setTrailingWSStart(UBiDi *pBiDi) {
    /* pBiDi->direction!=UBIDI_MIXED */

    സ്ഥിരം DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *levels=pBiDi->levels;
    int32_t start=pBiDi->length;
    UBiDiLevel paraLevel=pBiDi->paraLevel;

    /* If the line is terminated by a block separator, all preceding WS etc...
       are already set to paragraph level.
       Setting trailingWSStart to pBidi->length will avoid changing the
       level of B chars from 0 to paraLevel in ubidi_getLevels when
       orderParagraphsLTR==true.
     */
    എങ്കിൽ(dirProps[start-1]==B) {
        pBiDi->trailingWSStart=start;   /* currently == pBiDi->length */
        തിരികെ;
    }
    /* go backwards across all WS, BN, explicit codes */
    ആകുമ്പോൾ(start>0 && DIRPROP_FLAG(dirProps[start-1])&MASK_WS) {
        --start;
    }

    /* if the WS run can be merged with the previous run then do so here */
    ആകുമ്പോൾ(start>0 && levels[start-1]==paraLevel) {
        --start;
    }

    pBiDi->trailingWSStart=start;
}

/* ubidi_setLine ------------------------------------------------------------ */

U_CAPI ശൂന്യം U_EXPORT2
ubidi_set_വരി(സ്ഥിരം UBiDi *pParaBiDi,
              int32_t start, int32_t limit,
              UBiDi *p_വരി_Bi_Di,
              UErrorCode *pErrorCode) {
    int32_t length;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    RETURN_VOID_IF_NOT_VALID_PARA(pParaBiDi, *pErrorCode);
    RETURN_VOID_IF_BAD_RANGE(start, 0, limit, *pErrorCode);
    RETURN_VOID_IF_BAD_RANGE(limit, 0, pParaBiDi->length+1, *pErrorCode);
    എങ്കിൽ(p_വരി_Bi_Di==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    എങ്കിൽ(ubidi_getParagraph(pParaBiDi, start, nullptr, nullptr, nullptr, pErrorCode) !=
       ubidi_getParagraph(pParaBiDi, limit-1, nullptr, nullptr, nullptr, pErrorCode)) {
        /* the line crosses a paragraph boundary */
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    /* set the values in pLineBiDi from its pParaBiDi parent */
    p_വരി_Bi_Di->pParaBiDi=nullptr;          /* mark unfinished setLine */
    p_വരി_Bi_Di->text=pParaBiDi->text+start;
    length=p_വരി_Bi_Di->length=limit-start;
    p_വരി_Bi_Di->resultLength=p_വരി_Bi_Di->originalLength=length;
    p_വരി_Bi_Di->paraLevel=GET_PARALEVEL(pParaBiDi, start);
    p_വരി_Bi_Di->para_എണ്ണം=pParaBiDi->para_എണ്ണം;
    p_വരി_Bi_Di->runs=nullptr;
    p_വരി_Bi_Di->flags=0;
    p_വരി_Bi_Di->reorderingMode=pParaBiDi->reorderingMode;
    p_വരി_Bi_Di->reorderingOptions=pParaBiDi->reorderingOptions;
    p_വരി_Bi_Di->control_എണ്ണം=0;
    എങ്കിൽ(pParaBiDi->control_എണ്ണം>0) {
        int32_t j;
        വേണ്ടി(j=start; j<limit; j++) {
            എങ്കിൽ(IS_BIDI_CONTROL_CHAR(pParaBiDi->text[j])) {
                p_വരി_Bi_Di->control_എണ്ണം++;
            }
        }
        p_വരി_Bi_Di->resultLength-=p_വരി_Bi_Di->control_എണ്ണം;
    }

    p_വരി_Bi_Di->dirProps=pParaBiDi->dirProps+start;
    p_വരി_Bi_Di->levels=pParaBiDi->levels+start;
    p_വരി_Bi_Di->run_എണ്ണം=-1;

    എങ്കിൽ(pParaBiDi->direction!=UBIDI_MIXED) {
        /* the parent is already trivial */
        p_വരി_Bi_Di->direction=pParaBiDi->direction;

        /*
         * The parent's levels are all either
         * implicitly or explicitly ==paraLevel;
         * do the same here.
         */
        എങ്കിൽ(pParaBiDi->trailingWSStart<=start) {
            p_വരി_Bi_Di->trailingWSStart=0;
        } അല്ലെങ്കിൽ എങ്കിൽ(pParaBiDi->trailingWSStart<limit) {
            p_വരി_Bi_Di->trailingWSStart=pParaBiDi->trailingWSStart-start;
        } അല്ലെങ്കിൽ {
            p_വരി_Bi_Di->trailingWSStart=length;
        }
    } അല്ലെങ്കിൽ {
        സ്ഥിരം UBiDiLevel *levels=p_വരി_Bi_Di->levels;
        int32_t i, trailingWSStart;
        UBiDiLevel level;

        setTrailingWSStart(p_വരി_Bi_Di);
        trailingWSStart=p_വരി_Bi_Di->trailingWSStart;

        /* recalculate pLineBiDi->direction */
        എങ്കിൽ(trailingWSStart==0) {
            /* all levels are at paraLevel */
            p_വരി_Bi_Di->direction=(UBiDiDirection)(p_വരി_Bi_Di->paraLevel&1);
        } അല്ലെങ്കിൽ {
            /* get the level of the first character */
            level=(UBiDiLevel)(levels[0]&1);

            /* if there is anything of a different level, then the line is mixed */
            എങ്കിൽ(trailingWSStart<length && (p_വരി_Bi_Di->paraLevel&1)!=level) {
                /* the trailing WS is at paraLevel, which differs from levels[0] */
                p_വരി_Bi_Di->direction=UBIDI_MIXED;
            } അല്ലെങ്കിൽ {
                /* see if levels[1..trailingWSStart-1] have the same direction as levels[0] and paraLevel */
                i=1;
                വേണ്ടി(;;) {
                    എങ്കിൽ(i==trailingWSStart) {
                        /* the direction values match those in level */
                        p_വരി_Bi_Di->direction=(UBiDiDirection)level;
                        നിർത്തുക;
                    } അല്ലെങ്കിൽ എങ്കിൽ((levels[i]&1)!=level) {
                        p_വരി_Bi_Di->direction=UBIDI_MIXED;
                        നിർത്തുക;
                    }
                    ++i;
                }
            }
        }

        തിരഞ്ഞെടുപ്പ്(p_വരി_Bi_Di->direction) {
        അവസ്ഥ UBIDI_LTR:
            /* make sure paraLevel is even */
            p_വരി_Bi_Di->paraLevel=(UBiDiLevel)((p_വരി_Bi_Di->paraLevel+1)&~1);

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_വരി_Bi_Di->trailingWSStart=0;
            നിർത്തുക;
        അവസ്ഥ UBIDI_RTL:
            /* make sure paraLevel is odd */
            p_വരി_Bi_Di->paraLevel|=1;

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_വരി_Bi_Di->trailingWSStart=0;
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            നിർത്തുക;
        }
    }
    p_വരി_Bi_Di->pParaBiDi=pParaBiDi;     /* mark successful setLine */
}

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getLevelAt(സ്ഥിരം UBiDi *pBiDi, int32_t charIndex) {
    /* return paraLevel if in the trailing WS run, otherwise the real level */
    എങ്കിൽ(!IS_VALID_PARA_OR_വരി(pBiDi) || charIndex<0 || pBiDi->length<=charIndex) {
        തിരികെ 0;
    } അല്ലെങ്കിൽ എങ്കിൽ(pBiDi->direction!=UBIDI_MIXED || charIndex>=pBiDi->trailingWSStart) {
        തിരികെ GET_PARALEVEL(pBiDi, charIndex);
    } അല്ലെങ്കിൽ {
        തിരികെ pBiDi->levels[charIndex];
    }
}

U_CAPI സ്ഥിരം UBiDiLevel* U_EXPORT2
ubidi_getLevels(UBiDi* pBiDi, UErrorCode* pErrorCode) {
    int32_t start, length;

    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, nullptr);
    RETURN_IF_NOT_VALID_PARA_OR_വരി(pBiDi, *pErrorCode, nullptr);
    എങ്കിൽ((length=pBiDi->length)<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    എങ്കിൽ((start=pBiDi->trailingWSStart)==length) {
        /* the current levels array reflects the WS run */
        തിരികെ pBiDi->levels;
    }

    /*
     * After the previous if(), we know that the levels array
     * has an implicit trailing WS run and therefore does not fully
     * reflect itself all the levels.
     * This must be a UBiDi object for a line, and
     * we need to create a new levels array.
     */
    എങ്കിൽ(getLevelsMemory(pBiDi, length)) {
        UBiDiLevel *levels=pBiDi->levelsMemory;

        എങ്കിൽ(start>0 && levels!=pBiDi->levels) {
            uprv_memcpy(levels, pBiDi->levels, start);
        }
        /* pBiDi->paraLevel is ok even if contextual multiple paragraphs,
           since pBidi is a line object                                     */
        uprv_memset(levels+start, pBiDi->paraLevel, length-start);

        /* this new levels array is set for the line and reflects the WS run */
        pBiDi->trailingWSStart=length;
        തിരികെ pBiDi->levels=levels;
    } അല്ലെങ്കിൽ {
        /* out of memory */
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
}

U_CAPI ശൂന്യം U_EXPORT2
ubidi_getLogicalRun(സ്ഥിരം UBiDi *pBiDi, int32_t logicalPosition,
                    int32_t *pLogicalLimit, UBiDiLevel *pLevel) {
    UErrorCode errorCode;
    int32_t run_എണ്ണം, visualStart, logicalLimit, logicalFirst, i;
    Run iRun;

    errorCode=U_ZERO_ERROR;
    RETURN_VOID_IF_BAD_RANGE(logicalPosition, 0, pBiDi->length, errorCode);
    /* ubidi_countRuns will check VALID_PARA_OR_LINE */
    run_എണ്ണം=ubidi_എണ്ണം_Runs((UBiDi *)pBiDi, &errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    /* this is done based on runs rather than on levels since levels have
       a special interpretation when UBIDI_REORDER_RUNS_ONLY
     */
    visualStart=logicalLimit=0;
    iRun=pBiDi->runs[0];

    വേണ്ടി(i=0; i<run_എണ്ണം; i++) {
        iRun = pBiDi->runs[i];
        logicalFirst=GET_INDEX(iRun.logicalStart);
        logicalLimit=logicalFirst+iRun.visualLimit-visualStart;
        എങ്കിൽ((logicalPosition>=logicalFirst) &&
           (logicalPosition<logicalLimit)) {
            നിർത്തുക;
        }
        visualStart = iRun.visualLimit;
    }
    എങ്കിൽ(pLogicalLimit) {
        *pLogicalLimit=logicalLimit;
    }
    എങ്കിൽ(pLevel) {
        എങ്കിൽ(pBiDi->reorderingMode==UBIDI_REORDER_RUNS_ONLY) {
            *pLevel=(UBiDiLevel)GET_ODD_BIT(iRun.logicalStart);
        }
        അല്ലെങ്കിൽ എങ്കിൽ(pBiDi->direction!=UBIDI_MIXED || logicalPosition>=pBiDi->trailingWSStart) {
            *pLevel=GET_PARALEVEL(pBiDi, logicalPosition);
        } അല്ലെങ്കിൽ {
        *pLevel=pBiDi->levels[logicalPosition];
        }
    }
}

/* runs API functions ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_എണ്ണം_Runs(UBiDi *pBiDi, UErrorCode *pErrorCode) {
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_വരി(pBiDi, *pErrorCode, -1);
    ubidi_getRuns(pBiDi, pErrorCode);
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ -1;
    }
    തിരികെ pBiDi->run_എണ്ണം;
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_getVisualRun(UBiDi *pBiDi, int32_t runIndex,
                   int32_t *pLogicalStart, int32_t *pLength)
{
    int32_t start;
    UErrorCode errorCode = U_ZERO_ERROR;
    RETURN_IF_NOT_VALID_PARA_OR_വരി(pBiDi, errorCode, UBIDI_LTR);
    ubidi_getRuns(pBiDi, &errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ UBIDI_LTR;
    }
    RETURN_IF_BAD_RANGE(runIndex, 0, pBiDi->run_എണ്ണം, errorCode, UBIDI_LTR);

    start=pBiDi->runs[runIndex].logicalStart;
    എങ്കിൽ(pLogicalStart!=nullptr) {
        *pLogicalStart=GET_INDEX(start);
    }
    എങ്കിൽ(pLength!=nullptr) {
        എങ്കിൽ(runIndex>0) {
            *pLength=pBiDi->runs[runIndex].visualLimit-
                     pBiDi->runs[runIndex-1].visualLimit;
        } അല്ലെങ്കിൽ {
            *pLength=pBiDi->runs[0].visualLimit;
        }
    }
    തിരികെ (UBiDiDirection)GET_ODD_BIT(start);
}

/* in trivial cases there is only one trivial run; called by ubidi_getRuns() */
സ്ഥിതം ശൂന്യം
getSingleRun(UBiDi *pBiDi, UBiDiLevel level) {
    /* simple, single-run case */
    pBiDi->runs=pBiDi->simpleRuns;
    pBiDi->run_എണ്ണം=1;

    /* fill and reorder the single run */
    pBiDi->runs[0].logicalStart=MAKE_INDEX_ODD_PAIR(0, level);
    pBiDi->runs[0].visualLimit=pBiDi->length;
    pBiDi->runs[0].തിരുകുക_Remove=0;
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
സ്ഥിതം ശൂന്യം
reorder_വരി(UBiDi *pBiDi, UBiDiLevel minLevel, UBiDiLevel maxLevel) {
    Run *runs, tempRun;
    UBiDiLevel *levels;
    int32_t firstRun, endRun, limitRun, run_എണ്ണം;

    /* nothing to do? */
    എങ്കിൽ(maxLevel<=(minLevel|1)) {
        തിരികെ;
    }

    /*
     * Reorder only down to the lowest odd level
     * and reorder at an odd minLevel in a separate, simpler loop.
     * See comments above for why minLevel is always incremented.
     */
    ++minLevel;

    runs=pBiDi->runs;
    levels=pBiDi->levels;
    run_എണ്ണം=pBiDi->run_എണ്ണം;

    /* do not include the WS run at paraLevel<=old minLevel except in the simple loop */
    എങ്കിൽ(pBiDi->trailingWSStart<pBiDi->length) {
        --run_എണ്ണം;
    }

    ആകുമ്പോൾ(--maxLevel>=minLevel) {
        firstRun=0;

        /* loop for all sequences of runs */
        വേണ്ടി(;;) {
            /* look for a sequence of runs that are all at >=maxLevel */
            /* look for the first run of such a sequence */
            ആകുമ്പോൾ(firstRun<run_എണ്ണം && levels[runs[firstRun].logicalStart]<maxLevel) {
                ++firstRun;
            }
            എങ്കിൽ(firstRun>=run_എണ്ണം) {
                നിർത്തുക;  /* no more such runs */
            }

            /* look for the limit run of such a sequence (the run behind it) */
            വേണ്ടി(limitRun=firstRun; ++limitRun<run_എണ്ണം && levels[runs[limitRun].logicalStart]>=maxLevel;) {}

            /* Swap the entire sequence of runs from firstRun to limitRun-1. */
            endRun=limitRun-1;
            ആകുമ്പോൾ(firstRun<endRun) {
                tempRun = runs[firstRun];
                runs[firstRun]=runs[endRun];
                runs[endRun]=tempRun;
                ++firstRun;
                --endRun;
            }

            എങ്കിൽ(limitRun==run_എണ്ണം) {
                നിർത്തുക;  /* no more such runs */
            } അല്ലെങ്കിൽ {
                firstRun=limitRun+1;
            }
        }
    }

    /* now do maxLevel==old minLevel (==odd!), see above */
    എങ്കിൽ(!(minLevel&1)) {
        firstRun=0;

        /* include the trailing WS run in this complete reordering */
        എങ്കിൽ(pBiDi->trailingWSStart==pBiDi->length) {
            --run_എണ്ണം;
        }

        /* Swap the entire sequence of all runs. (endRun==runCount) */
        ആകുമ്പോൾ(firstRun<run_എണ്ണം) {
            tempRun=runs[firstRun];
            runs[firstRun]=runs[run_എണ്ണം];
            runs[run_എണ്ണം]=tempRun;
            ++firstRun;
            --run_എണ്ണം;
        }
    }
}

/* compute the runs array --------------------------------------------------- */

സ്ഥിതം int32_t getRunFromLogicalIndex(UBiDi *pBiDi, int32_t logicalIndex) {
    Run *runs=pBiDi->runs;
    int32_t run_എണ്ണം=pBiDi->run_എണ്ണം, visualStart=0, i, length, logicalStart;

    വേണ്ടി(i=0; i<run_എണ്ണം; i++) {
        length=runs[i].visualLimit-visualStart;
        logicalStart=GET_INDEX(runs[i].logicalStart);
        എങ്കിൽ((logicalIndex>=logicalStart) && (logicalIndex<(logicalStart+length))) {
            തിരികെ i;
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
    എങ്കിൽ (pBiDi->run_എണ്ണം>=0) {
        തിരികെ സത്യം;
    }

    എങ്കിൽ(pBiDi->direction!=UBIDI_MIXED) {
        /* simple, single-run case - this covers length==0 */
        /* pBiDi->paraLevel is ok even for contextual multiple paragraphs */
        getSingleRun(pBiDi, pBiDi->paraLevel);
    } അല്ലെങ്കിൽ /* UBIDI_MIXED, length>0 */ {
        /* mixed directionality */
        int32_t length=pBiDi->length, limit;
        UBiDiLevel *levels=pBiDi->levels;
        int32_t i, run_എണ്ണം;
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
        run_എണ്ണം=0;
        വേണ്ടി(i=0; i<limit; ++i) {
            /* increment runCount at the start of each run */
            എങ്കിൽ(levels[i]!=level) {
                ++run_എണ്ണം;
                level=levels[i];
            }
        }

        /*
         * We don't need to see if the last run can be merged with a trailing
         * WS run because setTrailingWSStart() would have done that.
         */
        എങ്കിൽ(run_എണ്ണം==1 && limit==length) {
            /* There is only one non-WS run and no trailing WS-run. */
            getSingleRun(pBiDi, levels[0]);
        } അല്ലെങ്കിൽ /* runCount>1 || limit<length */ {
            /* allocate and set the runs */
            Run *runs;
            int32_t runIndex, start;
            UBiDiLevel minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1, maxLevel=0;

            /* now, count a (non-mergeable) WS run */
            എങ്കിൽ(limit<length) {
                ++run_എണ്ണം;
            }

            /* runCount>1 */
            എങ്കിൽ(getRunsMemory(pBiDi, run_എണ്ണം)) {
                runs=pBiDi->runsMemory;
            } അല്ലെങ്കിൽ {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ അസത്യം;
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
            ചെയ്യുക {
                /* prepare this run */
                start=i;
                level=levels[i];
                എങ്കിൽ(level<minLevel) {
                    minLevel=level;
                }
                എങ്കിൽ(level>maxLevel) {
                    maxLevel=level;
                }

                /* look for the run limit */
                ആകുമ്പോൾ(++i<limit && levels[i]==level) {}

                /* i is another run limit */
                runs[runIndex].logicalStart=start;
                runs[runIndex].visualLimit=i-start;
                runs[runIndex].തിരുകുക_Remove=0;
                ++runIndex;
            } ആകുമ്പോൾ(i<limit);

            എങ്കിൽ(limit<length) {
                /* there is a separate WS run */
                runs[runIndex].logicalStart=limit;
                runs[runIndex].visualLimit=length-limit;
                /* For the trailing WS run, pBiDi->paraLevel is ok even
                   if contextual multiple paragraphs.                   */
                എങ്കിൽ(pBiDi->paraLevel<minLevel) {
                    minLevel=pBiDi->paraLevel;
                }
            }

            /* set the object fields */
            pBiDi->runs=runs;
            pBiDi->run_എണ്ണം=run_എണ്ണം;

            reorder_വരി(pBiDi, minLevel, maxLevel);

            /* now add the direction flags and adjust the visualLimit's to be just that */
            /* this loop will also handle the trailing WS run */
            limit=0;
            വേണ്ടി(i=0; i<run_എണ്ണം; ++i) {
                ADD_ODD_BIT_FROM_LEVEL(runs[i].logicalStart, levels[runs[i].logicalStart]);
                limit+=runs[i].visualLimit;
                runs[i].visualLimit=limit;
            }

            /* Set the "odd" bit for the trailing WS run. */
            /* For a RTL paragraph, it will be the *first* run in visual order. */
            /* For the trailing WS run, pBiDi->paraLevel is ok even if
               contextual multiple paragraphs.                          */
            എങ്കിൽ(runIndex<run_എണ്ണം) {
                int32_t trailingRun = ((pBiDi->paraLevel & 1) != 0)? 0 : runIndex;

                ADD_ODD_BIT_FROM_LEVEL(runs[trailingRun].logicalStart, pBiDi->paraLevel);
            }
        }
    }

    /* handle insert LRM/RLM BEFORE/AFTER run */
    എങ്കിൽ(pBiDi->തിരുകുക_Points.size>0) {
        Point *point, *start=pBiDi->തിരുകുക_Points.points,
                      *limit=start+pBiDi->തിരുകുക_Points.size;
        int32_t runIndex;
        വേണ്ടി(point=start; point<limit; point++) {
            runIndex=getRunFromLogicalIndex(pBiDi, point->pos);
            pBiDi->runs[runIndex].തിരുകുക_Remove|=point->flag;
        }
    }

    /* handle remove BiDi control characters */
    എങ്കിൽ(pBiDi->control_എണ്ണം>0) {
        int32_t runIndex;
        സ്ഥിരം char16_t *start=pBiDi->text, *limit=start+pBiDi->length, *pu;
        വേണ്ടി(pu=start; pu<limit; pu++) {
            എങ്കിൽ(IS_BIDI_CONTROL_CHAR(*pu)) {
                runIndex=getRunFromLogicalIndex(pBiDi, (int32_t)(pu-start));
                pBiDi->runs[runIndex].തിരുകുക_Remove--;
            }
        }
    }

    തിരികെ സത്യം;
}

സ്ഥിതം UBool
prepareReorder(സ്ഥിരം UBiDiLevel *levels, int32_t length,
               int32_t *indexMap,
               UBiDiLevel *pMinLevel, UBiDiLevel *pMaxLevel) {
    int32_t start;
    UBiDiLevel level, minLevel, maxLevel;

    എങ്കിൽ(levels==nullptr || length<=0) {
        തിരികെ അസത്യം;
    }

    /* determine minLevel and maxLevel */
    minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1;
    maxLevel=0;
    വേണ്ടി(start=length; start>0;) {
        level=levels[--start];
        എങ്കിൽ(level>UBIDI_MAX_EXPLICIT_LEVEL+1) {
            തിരികെ അസത്യം;
        }
        എങ്കിൽ(level<minLevel) {
            minLevel=level;
        }
        എങ്കിൽ(level>maxLevel) {
            maxLevel=level;
        }
    }
    *pMinLevel=minLevel;
    *pMaxLevel=maxLevel;

    /* initialize the index map */
    വേണ്ടി(start=length; start>0;) {
        --start;
        indexMap[start]=start;
    }

    തിരികെ സത്യം;
}

/* reorder a line based on a levels array (L2) ------------------------------ */

U_CAPI ശൂന്യം U_EXPORT2
ubidi_reorderLogical(സ്ഥിരം UBiDiLevel *levels, int32_t length, int32_t *indexMap) {
    int32_t start, limit, sumOfSosEos;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    എങ്കിൽ(indexMap==nullptr || !prepareReorder(levels, length, indexMap, &minLevel, &maxLevel)) {
        തിരികെ;
    }

    /* nothing to do? */
    എങ്കിൽ(minLevel==maxLevel && (minLevel&1)==0) {
        തിരികെ;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    ചെയ്യുക {
        start=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        വേണ്ടി(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            ആകുമ്പോൾ(start<length && levels[start]<maxLevel) {
                ++start;
            }
            എങ്കിൽ(start>=length) {
                നിർത്തുക;  /* no more such sequences */
            }

            /* look for the limit of such a sequence (the index behind it) */
            വേണ്ടി(limit=start; ++limit<length && levels[limit]>=maxLevel;) {}

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
            ചെയ്യുക {
                indexMap[start]=sumOfSosEos-indexMap[start];
            } ആകുമ്പോൾ(++start<limit);

            /* start==limit */
            എങ്കിൽ(limit==length) {
                നിർത്തുക;  /* no more such sequences */
            } അല്ലെങ്കിൽ {
                start=limit+1;
            }
        }
    } ആകുമ്പോൾ(--maxLevel>=minLevel);
}

U_CAPI ശൂന്യം U_EXPORT2
ubidi_reorderVisual(സ്ഥിരം UBiDiLevel *levels, int32_t length, int32_t *indexMap) {
    int32_t start, end, limit, temp;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    എങ്കിൽ(indexMap==nullptr || !prepareReorder(levels, length, indexMap, &minLevel, &maxLevel)) {
        തിരികെ;
    }

    /* nothing to do? */
    എങ്കിൽ(minLevel==maxLevel && (minLevel&1)==0) {
        തിരികെ;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    ചെയ്യുക {
        start=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        വേണ്ടി(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            ആകുമ്പോൾ(start<length && levels[start]<maxLevel) {
                ++start;
            }
            എങ്കിൽ(start>=length) {
                നിർത്തുക;  /* no more such runs */
            }

            /* look for the limit of such a sequence (the index behind it) */
            വേണ്ടി(limit=start; ++limit<length && levels[limit]>=maxLevel;) {}

            /*
             * Swap the entire interval of indexes from start to limit-1.
             * We don't need to swap the levels for the purpose of this
             * algorithm: the sequence of levels that we look at does not
             * move anyway.
             */
            end=limit-1;
            ആകുമ്പോൾ(start<end) {
                temp=indexMap[start];
                indexMap[start]=indexMap[end];
                indexMap[end]=temp;

                ++start;
                --end;
            }

            എങ്കിൽ(limit==length) {
                നിർത്തുക;  /* no more such sequences */
            } അല്ലെങ്കിൽ {
                start=limit+1;
            }
        }
    } ആകുമ്പോൾ(--maxLevel>=minLevel);
}

/* API functions for logical<->visual mapping ------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_getVisualIndex(UBiDi *pBiDi, int32_t logicalIndex, UErrorCode *pErrorCode) {
    int32_t visualIndex=UBIDI_MAP_NOWHERE;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_വരി(pBiDi, *pErrorCode, -1);
    RETURN_IF_BAD_RANGE(logicalIndex, 0, pBiDi->length, *pErrorCode, -1);

    /* we can do the trivial cases without the runs array */
    തിരഞ്ഞെടുപ്പ്(pBiDi->direction) {
    അവസ്ഥ UBIDI_LTR:
        visualIndex=logicalIndex;
        നിർത്തുക;
    അവസ്ഥ UBIDI_RTL:
        visualIndex=pBiDi->length-logicalIndex-1;
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        എങ്കിൽ(!ubidi_getRuns(pBiDi, pErrorCode)) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            തിരികെ -1;
        } അല്ലെങ്കിൽ {
            Run *runs=pBiDi->runs;
            int32_t i, visualStart=0, offset, length;

            /* linear search for the run, search on the visual runs */
            വേണ്ടി(i=0; i<pBiDi->run_എണ്ണം; ++i) {
                length=runs[i].visualLimit-visualStart;
                offset=logicalIndex-GET_INDEX(runs[i].logicalStart);
                എങ്കിൽ(offset>=0 && offset<length) {
                    എങ്കിൽ(IS_EVEN_RUN(runs[i].logicalStart)) {
                        /* LTR */
                        visualIndex=visualStart+offset;
                    } അല്ലെങ്കിൽ {
                        /* RTL */
                        visualIndex=visualStart+length-offset-1;
                    }
                    നിർത്തുക;          /* exit for loop */
                }
                visualStart+=length;
            }
            എങ്കിൽ(i>=pBiDi->run_എണ്ണം) {
                തിരികെ UBIDI_MAP_NOWHERE;
            }
        }
    }

    എങ്കിൽ(pBiDi->തിരുകുക_Points.size>0) {
        /* add the number of added marks until the calculated visual index */
        Run *runs=pBiDi->runs;
        int32_t i, length, തിരുകുക_Remove;
        int32_t visualStart=0, markFound=0;
        വേണ്ടി(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            തിരുകുക_Remove=runs[i].തിരുകുക_Remove;
            എങ്കിൽ(തിരുകുക_Remove & (LRM_BEFORE|RLM_BEFORE)) {
                markFound++;
            }
            /* is it the run containing the visual index? */
            എങ്കിൽ(visualIndex<runs[i].visualLimit) {
                തിരികെ visualIndex+markFound;
            }
            എങ്കിൽ(തിരുകുക_Remove & (LRM_AFTER|RLM_AFTER)) {
                markFound++;
            }
        }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(pBiDi->control_എണ്ണം>0) {
        /* subtract the number of controls until the calculated visual index */
        Run *runs=pBiDi->runs;
        int32_t i, j, start, limit, length, തിരുകുക_Remove;
        int32_t visualStart=0, controlFound=0;
        char16_t uchar=pBiDi->text[logicalIndex];
        /* is the logical index pointing to a control ? */
        എങ്കിൽ(IS_BIDI_CONTROL_CHAR(uchar)) {
            തിരികെ UBIDI_MAP_NOWHERE;
        }
        /* loop on runs */
        വേണ്ടി(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            തിരുകുക_Remove=runs[i].തിരുകുക_Remove;
            /* calculated visual index is beyond this run? */
            എങ്കിൽ(visualIndex>=runs[i].visualLimit) {
                controlFound-=തിരുകുക_Remove;
                തുടരുക;
            }
            /* calculated visual index must be within current run */
            എങ്കിൽ(തിരുകുക_Remove==0) {
                തിരികെ visualIndex-controlFound;
            }
            എങ്കിൽ(IS_EVEN_RUN(runs[i].logicalStart)) {
                /* LTR: check from run start to logical index */
                start=runs[i].logicalStart;
                limit=logicalIndex;
            } അല്ലെങ്കിൽ {
                /* RTL: check from logical index to run end */
                start=logicalIndex+1;
                limit=GET_INDEX(runs[i].logicalStart)+length;
            }
            വേണ്ടി(j=start; j<limit; j++) {
                uchar=pBiDi->text[j];
                എങ്കിൽ(IS_BIDI_CONTROL_CHAR(uchar)) {
                    controlFound++;
                }
            }
            തിരികെ visualIndex-controlFound;
        }
    }

    തിരികെ visualIndex;
}

U_CAPI int32_t U_EXPORT2
ubidi_getLogicalIndex(UBiDi *pBiDi, int32_t visualIndex, UErrorCode *pErrorCode) {
    Run *runs;
    int32_t i, run_എണ്ണം, start;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_വരി(pBiDi, *pErrorCode, -1);
    RETURN_IF_BAD_RANGE(visualIndex, 0, pBiDi->resultLength, *pErrorCode, -1);
    /* we can do the trivial cases without the runs array */
    എങ്കിൽ(pBiDi->തിരുകുക_Points.size==0 && pBiDi->control_എണ്ണം==0) {
        എങ്കിൽ(pBiDi->direction==UBIDI_LTR) {
            തിരികെ visualIndex;
        }
        അല്ലെങ്കിൽ എങ്കിൽ(pBiDi->direction==UBIDI_RTL) {
            തിരികെ pBiDi->length-visualIndex-1;
        }
    }
    എങ്കിൽ(!ubidi_getRuns(pBiDi, pErrorCode)) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ -1;
    }

    runs=pBiDi->runs;
    run_എണ്ണം=pBiDi->run_എണ്ണം;
    എങ്കിൽ(pBiDi->തിരുകുക_Points.size>0) {
        /* handle inserted LRM/RLM */
        int32_t markFound=0, തിരുകുക_Remove;
        int32_t visualStart=0, length;
        runs=pBiDi->runs;
        /* subtract number of marks until visual index */
        വേണ്ടി(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            തിരുകുക_Remove=runs[i].തിരുകുക_Remove;
            എങ്കിൽ(തിരുകുക_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                എങ്കിൽ(visualIndex<=(visualStart+markFound)) {
                    തിരികെ UBIDI_MAP_NOWHERE;
                }
                markFound++;
            }
            /* is adjusted visual index within this run? */
            എങ്കിൽ(visualIndex<(runs[i].visualLimit+markFound)) {
                visualIndex-=markFound;
                നിർത്തുക;
            }
            എങ്കിൽ(തിരുകുക_Remove&(LRM_AFTER|RLM_AFTER)) {
                എങ്കിൽ(visualIndex==(visualStart+length+markFound)) {
                    തിരികെ UBIDI_MAP_NOWHERE;
                }
                markFound++;
            }
        }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(pBiDi->control_എണ്ണം>0) {
        /* handle removed BiDi control characters */
        int32_t controlFound=0, തിരുകുക_Remove, length;
        int32_t logicalStart, logicalEnd, visualStart=0, j, k;
        char16_t uchar;
        UBool evenRun;
        /* add number of controls until visual index */
        വേണ്ടി(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            തിരുകുക_Remove=runs[i].തിരുകുക_Remove;
            /* is adjusted visual index beyond current run? */
            എങ്കിൽ(visualIndex>=(runs[i].visualLimit-controlFound+തിരുകുക_Remove)) {
                controlFound-=തിരുകുക_Remove;
                തുടരുക;
            }
            /* adjusted visual index is within current run */
            എങ്കിൽ(തിരുകുക_Remove==0) {
                visualIndex+=controlFound;
                നിർത്തുക;
            }
            /* count non-control chars until visualIndex */
            logicalStart=runs[i].logicalStart;
            evenRun=IS_EVEN_RUN(logicalStart);
            REMOVE_ODD_BIT(logicalStart);
            logicalEnd=logicalStart+length-1;
            വേണ്ടി(j=0; j<length; j++) {
                k= evenRun ? logicalStart+j : logicalEnd-j;
                uchar=pBiDi->text[k];
                എങ്കിൽ(IS_BIDI_CONTROL_CHAR(uchar)) {
                    controlFound++;
                }
                എങ്കിൽ((visualIndex+controlFound)==(visualStart+j)) {
                    നിർത്തുക;
                }
            }
            visualIndex+=controlFound;
            നിർത്തുക;
        }
    }
    /* handle all cases */
    എങ്കിൽ(run_എണ്ണം<=10) {
        /* linear search for the run */
        വേണ്ടി(i=0; visualIndex>=runs[i].visualLimit; ++i) {}
    } അല്ലെങ്കിൽ {
        /* binary search for the run */
        int32_t begin=0, limit=run_എണ്ണം;

        /* the middle if() is guaranteed to find the run, we don't need a loop limit */
        വേണ്ടി(;;) {
            i=(begin+limit)/2;
            എങ്കിൽ(visualIndex>=runs[i].visualLimit) {
                begin=i+1;
            } അല്ലെങ്കിൽ എങ്കിൽ(i==0 || visualIndex>=runs[i-1].visualLimit) {
                നിർത്തുക;
            } അല്ലെങ്കിൽ {
                limit=i;
            }
        }
    }

    start=runs[i].logicalStart;
    എങ്കിൽ(IS_EVEN_RUN(start)) {
        /* LTR */
        /* the offset in runs[i] is visualIndex-runs[i-1].visualLimit */
        എങ്കിൽ(i>0) {
            visualIndex-=runs[i-1].visualLimit;
        }
        തിരികെ start+visualIndex;
    } അല്ലെങ്കിൽ {
        /* RTL */
        തിരികെ GET_INDEX(start)+runs[i].visualLimit-visualIndex-1;
    }
}

U_CAPI ശൂന്യം U_EXPORT2
ubidi_getLogicalMap(UBiDi *pBiDi, int32_t *indexMap, UErrorCode *pErrorCode) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_എണ്ണം_Runs(pBiDi, pErrorCode);
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        /* no op */
    } അല്ലെങ്കിൽ എങ്കിൽ(indexMap==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } അല്ലെങ്കിൽ {
        /* fill a logical-to-visual index map using the runs[] */
        int32_t visualStart, visualLimit, i, j, k;
        int32_t logicalStart, logicalLimit;
        Run *runs=pBiDi->runs;
        എങ്കിൽ (pBiDi->length<=0) {
            തിരികെ;
        }
        എങ്കിൽ (pBiDi->length>pBiDi->resultLength) {
            uprv_memset(indexMap, 0xFF, pBiDi->length*വലുപ്പം(int32_t));
        }

        visualStart=0;
        വേണ്ടി(j=0; j<pBiDi->run_എണ്ണം; ++j) {
            logicalStart=GET_INDEX(runs[j].logicalStart);
            visualLimit=runs[j].visualLimit;
            എങ്കിൽ(IS_EVEN_RUN(runs[j].logicalStart)) {
                ചെയ്യുക { /* LTR */
                    indexMap[logicalStart++]=visualStart++;
                } ആകുമ്പോൾ(visualStart<visualLimit);
            } അല്ലെങ്കിൽ {
                logicalStart+=visualLimit-visualStart;  /* logicalLimit */
                ചെയ്യുക { /* RTL */
                    indexMap[--logicalStart]=visualStart++;
                } ആകുമ്പോൾ(visualStart<visualLimit);
            }
            /* visualStart==visualLimit; */
        }

        എങ്കിൽ(pBiDi->തിരുകുക_Points.size>0) {
            int32_t markFound=0, run_എണ്ണം=pBiDi->run_എണ്ണം;
            int32_t length, തിരുകുക_Remove;
            visualStart=0;
            /* add number of marks found until each index */
            വേണ്ടി(i=0; i<run_എണ്ണം; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                തിരുകുക_Remove=runs[i].തിരുകുക_Remove;
                എങ്കിൽ(തിരുകുക_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    markFound++;
                }
                എങ്കിൽ(markFound>0) {
                    logicalStart=GET_INDEX(runs[i].logicalStart);
                    logicalLimit=logicalStart+length;
                    വേണ്ടി(j=logicalStart; j<logicalLimit; j++) {
                        indexMap[j]+=markFound;
                    }
                }
                എങ്കിൽ(തിരുകുക_Remove&(LRM_AFTER|RLM_AFTER)) {
                    markFound++;
                }
            }
        }
        അല്ലെങ്കിൽ എങ്കിൽ(pBiDi->control_എണ്ണം>0) {
            int32_t controlFound=0, run_എണ്ണം=pBiDi->run_എണ്ണം;
            int32_t length, തിരുകുക_Remove;
            UBool evenRun;
            char16_t uchar;
            visualStart=0;
            /* subtract number of controls found until each index */
            വേണ്ടി(i=0; i<run_എണ്ണം; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                തിരുകുക_Remove=runs[i].തിരുകുക_Remove;
                /* no control found within previous runs nor within this run */
                എങ്കിൽ((controlFound-തിരുകുക_Remove)==0) {
                    തുടരുക;
                }
                logicalStart=runs[i].logicalStart;
                evenRun=IS_EVEN_RUN(logicalStart);
                REMOVE_ODD_BIT(logicalStart);
                logicalLimit=logicalStart+length;
                /* if no control within this run */
                എങ്കിൽ(തിരുകുക_Remove==0) {
                    വേണ്ടി(j=logicalStart; j<logicalLimit; j++) {
                        indexMap[j]-=controlFound;
                    }
                    തുടരുക;
                }
                വേണ്ടി(j=0; j<length; j++) {
                    k= evenRun ? logicalStart+j : logicalLimit-j-1;
                    uchar=pBiDi->text[k];
                    എങ്കിൽ(IS_BIDI_CONTROL_CHAR(uchar)) {
                        controlFound++;
                        indexMap[k]=UBIDI_MAP_NOWHERE;
                        തുടരുക;
                    }
                    indexMap[k]-=controlFound;
                }
            }
        }
    }
}

U_CAPI ശൂന്യം U_EXPORT2
ubidi_getVisualMap(UBiDi *pBiDi, int32_t *indexMap, UErrorCode *pErrorCode) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    എങ്കിൽ(indexMap==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_എണ്ണം_Runs(pBiDi, pErrorCode);
    എങ്കിൽ(U_SUCCESS(*pErrorCode)) {
        /* fill a visual-to-logical index map using the runs[] */
        Run *runs=pBiDi->runs, *runsLimit=runs+pBiDi->run_എണ്ണം;
        int32_t logicalStart, visualStart, visualLimit, *pi=indexMap;

        എങ്കിൽ (pBiDi->resultLength<=0) {
            തിരികെ;
        }
        visualStart=0;
        വേണ്ടി(; runs<runsLimit; ++runs) {
            logicalStart=runs->logicalStart;
            visualLimit=runs->visualLimit;
            എങ്കിൽ(IS_EVEN_RUN(logicalStart)) {
                ചെയ്യുക { /* LTR */
                    *pi++ = logicalStart++;
                } ആകുമ്പോൾ(++visualStart<visualLimit);
            } അല്ലെങ്കിൽ {
                REMOVE_ODD_BIT(logicalStart);
                logicalStart+=visualLimit-visualStart;  /* logicalLimit */
                ചെയ്യുക { /* RTL */
                    *pi++ = --logicalStart;
                } ആകുമ്പോൾ(++visualStart<visualLimit);
            }
            /* visualStart==visualLimit; */
        }

        എങ്കിൽ(pBiDi->തിരുകുക_Points.size>0) {
            int32_t markFound=0, run_എണ്ണം=pBiDi->run_എണ്ണം;
            int32_t തിരുകുക_Remove, i, j, k;
            runs=pBiDi->runs;
            /* count all inserted marks */
            വേണ്ടി(i=0; i<run_എണ്ണം; i++) {
                തിരുകുക_Remove=runs[i].തിരുകുക_Remove;
                എങ്കിൽ(തിരുകുക_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    markFound++;
                }
                എങ്കിൽ(തിരുകുക_Remove&(LRM_AFTER|RLM_AFTER)) {
                    markFound++;
                }
            }
            /* move back indexes by number of preceding marks */
            k=pBiDi->resultLength;
            വേണ്ടി(i=run_എണ്ണം-1; i>=0 && markFound>0; i--) {
                തിരുകുക_Remove=runs[i].തിരുകുക_Remove;
                എങ്കിൽ(തിരുകുക_Remove&(LRM_AFTER|RLM_AFTER)) {
                    indexMap[--k]= UBIDI_MAP_NOWHERE;
                    markFound--;
                }
                visualStart= i>0 ? runs[i-1].visualLimit : 0;
                വേണ്ടി(j=runs[i].visualLimit-1; j>=visualStart && markFound>0; j--) {
                    indexMap[--k]=indexMap[j];
                }
                എങ്കിൽ(തിരുകുക_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    indexMap[--k]= UBIDI_MAP_NOWHERE;
                    markFound--;
                }
            }
        }
        അല്ലെങ്കിൽ എങ്കിൽ(pBiDi->control_എണ്ണം>0) {
            int32_t run_എണ്ണം=pBiDi->run_എണ്ണം, logicalEnd;
            int32_t തിരുകുക_Remove, length, i, j, k, m;
            char16_t uchar;
            UBool evenRun;
            runs=pBiDi->runs;
            visualStart=0;
            /* move forward indexes by number of preceding controls */
            k=0;
            വേണ്ടി(i=0; i<run_എണ്ണം; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                തിരുകുക_Remove=runs[i].തിരുകുക_Remove;
                /* if no control found yet, nothing to do in this run */
                എങ്കിൽ((തിരുകുക_Remove==0)&&(k==visualStart)) {
                    k+=length;
                    തുടരുക;
                }
                /* if no control in this run */
                എങ്കിൽ(തിരുകുക_Remove==0) {
                    visualLimit=runs[i].visualLimit;
                    വേണ്ടി(j=visualStart; j<visualLimit; j++) {
                        indexMap[k++]=indexMap[j];
                    }
                    തുടരുക;
                }
                logicalStart=runs[i].logicalStart;
                evenRun=IS_EVEN_RUN(logicalStart);
                REMOVE_ODD_BIT(logicalStart);
                logicalEnd=logicalStart+length-1;
                വേണ്ടി(j=0; j<length; j++) {
                    m= evenRun ? logicalStart+j : logicalEnd-j;
                    uchar=pBiDi->text[m];
                    എങ്കിൽ(!IS_BIDI_CONTROL_CHAR(uchar)) {
                        indexMap[k++]=m;
                    }
                }
            }
        }
    }
}

U_CAPI ശൂന്യം U_EXPORT2
ubidi_invertMap(സ്ഥിരം int32_t *srcMap, int32_t *destMap, int32_t length) {
    എങ്കിൽ(srcMap!=nullptr && destMap!=nullptr && length>0) {
        സ്ഥിരം int32_t *pi;
        int32_t destLength=-1, count=0;
        /* find highest value and count positive indexes in srcMap */
        pi=srcMap+length;
        ആകുമ്പോൾ(pi>srcMap) {
            എങ്കിൽ(*--pi>destLength) {
                destLength=*pi;
            }
            എങ്കിൽ(*pi>=0) {
                count++;
            }
        }
        destLength++;           /* add 1 for origin 0 */
        എങ്കിൽ(count<destLength) {
            /* we must fill unmatched destMap entries with -1 */
            uprv_memset(destMap, 0xFF, destLength*വലുപ്പം(int32_t));
        }
        pi=srcMap+length;
        ആകുമ്പോൾ(length>0) {
            എങ്കിൽ(*--pi>=0) {
                destMap[*pi]=--length;
            } അല്ലെങ്കിൽ {
                --length;
            }
        }
    }
}
