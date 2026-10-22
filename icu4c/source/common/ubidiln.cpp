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

#समावेश "cmemory.h"
#समावेश "unicode/utypes.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/ubidi.h"
#समावेश "ubidiimp.h"
#समावेश "uassert.h"

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
अटल व्योम
setTrailingWSStart(UBiDi *pBiDi) {
    /* pBiDi->direction!=UBIDI_MIXED */

    स्थिर DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *levels=pBiDi->levels;
    int32_t start=pBiDi->length;
    UBiDiLevel paraLevel=pBiDi->paraLevel;

    /* If the line is terminated by a block separator, all preceding WS etc...
       are already set to paragraph level.
       Setting trailingWSStart to pBidi->length will avoid changing the
       level of B chars from 0 to paraLevel in ubidi_getLevels when
       orderParagraphsLTR==true.
     */
    अगर(dirProps[start-1]==B) {
        pBiDi->trailingWSStart=start;   /* currently == pBiDi->length */
        वापस;
    }
    /* go backwards across all WS, BN, explicit codes */
    जबतक(start>0 && DIRPROP_FLAG(dirProps[start-1])&MASK_WS) {
        --start;
    }

    /* if the WS run can be merged with the previous run then do so here */
    जबतक(start>0 && levels[start-1]==paraLevel) {
        --start;
    }

    pBiDi->trailingWSStart=start;
}

/* ubidi_setLine ------------------------------------------------------------ */

U_CAPI व्योम U_EXPORT2
ubidi_set_पंक्ति(स्थिर UBiDi *pParaBiDi,
              int32_t start, int32_t limit,
              UBiDi *p_पंक्ति_Bi_Di,
              UErrorCode *pErrorCode) {
    int32_t length;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    RETURN_VOID_IF_NOT_VALID_PARA(pParaBiDi, *pErrorCode);
    RETURN_VOID_IF_BAD_RANGE(start, 0, limit, *pErrorCode);
    RETURN_VOID_IF_BAD_RANGE(limit, 0, pParaBiDi->length+1, *pErrorCode);
    अगर(p_पंक्ति_Bi_Di==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    अगर(ubidi_getParagraph(pParaBiDi, start, nullptr, nullptr, nullptr, pErrorCode) !=
       ubidi_getParagraph(pParaBiDi, limit-1, nullptr, nullptr, nullptr, pErrorCode)) {
        /* the line crosses a paragraph boundary */
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    /* set the values in pLineBiDi from its pParaBiDi parent */
    p_पंक्ति_Bi_Di->pParaBiDi=nullptr;          /* mark unfinished setLine */
    p_पंक्ति_Bi_Di->text=pParaBiDi->text+start;
    length=p_पंक्ति_Bi_Di->length=limit-start;
    p_पंक्ति_Bi_Di->परिणाम_लंबाई=p_पंक्ति_Bi_Di->original_लंबाई=length;
    p_पंक्ति_Bi_Di->paraLevel=GET_PARALEVEL(pParaBiDi, start);
    p_पंक्ति_Bi_Di->para_गिनती=pParaBiDi->para_गिनती;
    p_पंक्ति_Bi_Di->runs=nullptr;
    p_पंक्ति_Bi_Di->flags=0;
    p_पंक्ति_Bi_Di->reorderingMode=pParaBiDi->reorderingMode;
    p_पंक्ति_Bi_Di->reorderingOptions=pParaBiDi->reorderingOptions;
    p_पंक्ति_Bi_Di->control_गिनती=0;
    अगर(pParaBiDi->control_गिनती>0) {
        int32_t j;
        क्रम(j=start; j<limit; j++) {
            अगर(IS_BIDI_CONTROL_CHAR(pParaBiDi->text[j])) {
                p_पंक्ति_Bi_Di->control_गिनती++;
            }
        }
        p_पंक्ति_Bi_Di->परिणाम_लंबाई-=p_पंक्ति_Bi_Di->control_गिनती;
    }

    p_पंक्ति_Bi_Di->dirProps=pParaBiDi->dirProps+start;
    p_पंक्ति_Bi_Di->levels=pParaBiDi->levels+start;
    p_पंक्ति_Bi_Di->run_गिनती=-1;

    अगर(pParaBiDi->direction!=UBIDI_MIXED) {
        /* the parent is already trivial */
        p_पंक्ति_Bi_Di->direction=pParaBiDi->direction;

        /*
         * The parent's levels are all either
         * implicitly or explicitly ==paraLevel;
         * do the same here.
         */
        अगर(pParaBiDi->trailingWSStart<=start) {
            p_पंक्ति_Bi_Di->trailingWSStart=0;
        } अन्यथा अगर(pParaBiDi->trailingWSStart<limit) {
            p_पंक्ति_Bi_Di->trailingWSStart=pParaBiDi->trailingWSStart-start;
        } अन्यथा {
            p_पंक्ति_Bi_Di->trailingWSStart=length;
        }
    } अन्यथा {
        स्थिर UBiDiLevel *levels=p_पंक्ति_Bi_Di->levels;
        int32_t i, trailingWSStart;
        UBiDiLevel level;

        setTrailingWSStart(p_पंक्ति_Bi_Di);
        trailingWSStart=p_पंक्ति_Bi_Di->trailingWSStart;

        /* recalculate pLineBiDi->direction */
        अगर(trailingWSStart==0) {
            /* all levels are at paraLevel */
            p_पंक्ति_Bi_Di->direction=(UBiDiDirection)(p_पंक्ति_Bi_Di->paraLevel&1);
        } अन्यथा {
            /* get the level of the first character */
            level=(UBiDiLevel)(levels[0]&1);

            /* if there is anything of a different level, then the line is mixed */
            अगर(trailingWSStart<length && (p_पंक्ति_Bi_Di->paraLevel&1)!=level) {
                /* the trailing WS is at paraLevel, which differs from levels[0] */
                p_पंक्ति_Bi_Di->direction=UBIDI_MIXED;
            } अन्यथा {
                /* see if levels[1..trailingWSStart-1] have the same direction as levels[0] and paraLevel */
                i=1;
                क्रम(;;) {
                    अगर(i==trailingWSStart) {
                        /* the direction values match those in level */
                        p_पंक्ति_Bi_Di->direction=(UBiDiDirection)level;
                        अवरोध;
                    } अन्यथा अगर((levels[i]&1)!=level) {
                        p_पंक्ति_Bi_Di->direction=UBIDI_MIXED;
                        अवरोध;
                    }
                    ++i;
                }
            }
        }

        चयन(p_पंक्ति_Bi_Di->direction) {
        हाल UBIDI_LTR:
            /* make sure paraLevel is even */
            p_पंक्ति_Bi_Di->paraLevel=(UBiDiLevel)((p_पंक्ति_Bi_Di->paraLevel+1)&~1);

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_पंक्ति_Bi_Di->trailingWSStart=0;
            अवरोध;
        हाल UBIDI_RTL:
            /* make sure paraLevel is odd */
            p_पंक्ति_Bi_Di->paraLevel|=1;

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_पंक्ति_Bi_Di->trailingWSStart=0;
            अवरोध;
        शेष:
            अवरोध;
        }
    }
    p_पंक्ति_Bi_Di->pParaBiDi=pParaBiDi;     /* mark successful setLine */
}

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getLevelAt(स्थिर UBiDi *pBiDi, int32_t char_सूचकांक) {
    /* return paraLevel if in the trailing WS run, otherwise the real level */
    अगर(!IS_VALID_PARA_OR_पंक्ति(pBiDi) || char_सूचकांक<0 || pBiDi->length<=char_सूचकांक) {
        वापस 0;
    } अन्यथा अगर(pBiDi->direction!=UBIDI_MIXED || char_सूचकांक>=pBiDi->trailingWSStart) {
        वापस GET_PARALEVEL(pBiDi, char_सूचकांक);
    } अन्यथा {
        वापस pBiDi->levels[char_सूचकांक];
    }
}

U_CAPI स्थिर UBiDiLevel* U_EXPORT2
ubidi_getLevels(UBiDi* pBiDi, UErrorCode* pErrorCode) {
    int32_t start, length;

    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, nullptr);
    RETURN_IF_NOT_VALID_PARA_OR_पंक्ति(pBiDi, *pErrorCode, nullptr);
    अगर((length=pBiDi->length)<=0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    अगर((start=pBiDi->trailingWSStart)==length) {
        /* the current levels array reflects the WS run */
        वापस pBiDi->levels;
    }

    /*
     * After the previous if(), we know that the levels array
     * has an implicit trailing WS run and therefore does not fully
     * reflect itself all the levels.
     * This must be a UBiDi object for a line, and
     * we need to create a new levels array.
     */
    अगर(getLevelsMemory(pBiDi, length)) {
        UBiDiLevel *levels=pBiDi->levelsMemory;

        अगर(start>0 && levels!=pBiDi->levels) {
            uprv_memcpy(levels, pBiDi->levels, start);
        }
        /* pBiDi->paraLevel is ok even if contextual multiple paragraphs,
           since pBidi is a line object                                     */
        uprv_memset(levels+start, pBiDi->paraLevel, length-start);

        /* this new levels array is set for the line and reflects the WS run */
        pBiDi->trailingWSStart=length;
        वापस pBiDi->levels=levels;
    } अन्यथा {
        /* out of memory */
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
}

U_CAPI व्योम U_EXPORT2
ubidi_getLogicalRun(स्थिर UBiDi *pBiDi, int32_t logicalPosition,
                    int32_t *pLogicalLimit, UBiDiLevel *pLevel) {
    UErrorCode errorCode;
    int32_t run_गिनती, visualStart, logicalLimit, logicalFirst, i;
    Run iRun;

    errorCode=U_ZERO_ERROR;
    RETURN_VOID_IF_BAD_RANGE(logicalPosition, 0, pBiDi->length, errorCode);
    /* ubidi_countRuns will check VALID_PARA_OR_LINE */
    run_गिनती=ubidi_गिनती_Runs((UBiDi *)pBiDi, &errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    /* this is done based on runs rather than on levels since levels have
       a special interpretation when UBIDI_REORDER_RUNS_ONLY
     */
    visualStart=logicalLimit=0;
    iRun=pBiDi->runs[0];

    क्रम(i=0; i<run_गिनती; i++) {
        iRun = pBiDi->runs[i];
        logicalFirst=GET_सूचकांक(iRun.logicalStart);
        logicalLimit=logicalFirst+iRun.visualLimit-visualStart;
        अगर((logicalPosition>=logicalFirst) &&
           (logicalPosition<logicalLimit)) {
            अवरोध;
        }
        visualStart = iRun.visualLimit;
    }
    अगर(pLogicalLimit) {
        *pLogicalLimit=logicalLimit;
    }
    अगर(pLevel) {
        अगर(pBiDi->reorderingMode==UBIDI_REORDER_RUNS_ONLY) {
            *pLevel=(UBiDiLevel)GET_ODD_BIT(iRun.logicalStart);
        }
        अन्यथा अगर(pBiDi->direction!=UBIDI_MIXED || logicalPosition>=pBiDi->trailingWSStart) {
            *pLevel=GET_PARALEVEL(pBiDi, logicalPosition);
        } अन्यथा {
        *pLevel=pBiDi->levels[logicalPosition];
        }
    }
}

/* runs API functions ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_गिनती_Runs(UBiDi *pBiDi, UErrorCode *pErrorCode) {
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_पंक्ति(pBiDi, *pErrorCode, -1);
    ubidi_getRuns(pBiDi, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        वापस -1;
    }
    वापस pBiDi->run_गिनती;
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_getVisualRun(UBiDi *pBiDi, int32_t run_सूचकांक,
                   int32_t *pLogicalStart, int32_t *p_लंबाई)
{
    int32_t start;
    UErrorCode errorCode = U_ZERO_ERROR;
    RETURN_IF_NOT_VALID_PARA_OR_पंक्ति(pBiDi, errorCode, UBIDI_LTR);
    ubidi_getRuns(pBiDi, &errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस UBIDI_LTR;
    }
    RETURN_IF_BAD_RANGE(run_सूचकांक, 0, pBiDi->run_गिनती, errorCode, UBIDI_LTR);

    start=pBiDi->runs[run_सूचकांक].logicalStart;
    अगर(pLogicalStart!=nullptr) {
        *pLogicalStart=GET_सूचकांक(start);
    }
    अगर(p_लंबाई!=nullptr) {
        अगर(run_सूचकांक>0) {
            *p_लंबाई=pBiDi->runs[run_सूचकांक].visualLimit-
                     pBiDi->runs[run_सूचकांक-1].visualLimit;
        } अन्यथा {
            *p_लंबाई=pBiDi->runs[0].visualLimit;
        }
    }
    वापस (UBiDiDirection)GET_ODD_BIT(start);
}

/* in trivial cases there is only one trivial run; called by ubidi_getRuns() */
अटल व्योम
getSingleRun(UBiDi *pBiDi, UBiDiLevel level) {
    /* simple, single-run case */
    pBiDi->runs=pBiDi->simpleRuns;
    pBiDi->run_गिनती=1;

    /* fill and reorder the single run */
    pBiDi->runs[0].logicalStart=MAKE_सूचकांक_ODD_PAIR(0, level);
    pBiDi->runs[0].visualLimit=pBiDi->length;
    pBiDi->runs[0].जोड़ो_Remove=0;
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
अटल व्योम
reorder_पंक्ति(UBiDi *pBiDi, UBiDiLevel minLevel, UBiDiLevel maxLevel) {
    Run *runs, अस्थायी_Run;
    UBiDiLevel *levels;
    int32_t firstRun, endRun, limitRun, run_गिनती;

    /* nothing to do? */
    अगर(maxLevel<=(minLevel|1)) {
        वापस;
    }

    /*
     * Reorder only down to the lowest odd level
     * and reorder at an odd minLevel in a separate, simpler loop.
     * See comments above for why minLevel is always incremented.
     */
    ++minLevel;

    runs=pBiDi->runs;
    levels=pBiDi->levels;
    run_गिनती=pBiDi->run_गिनती;

    /* do not include the WS run at paraLevel<=old minLevel except in the simple loop */
    अगर(pBiDi->trailingWSStart<pBiDi->length) {
        --run_गिनती;
    }

    जबतक(--maxLevel>=minLevel) {
        firstRun=0;

        /* loop for all sequences of runs */
        क्रम(;;) {
            /* look for a sequence of runs that are all at >=maxLevel */
            /* look for the first run of such a sequence */
            जबतक(firstRun<run_गिनती && levels[runs[firstRun].logicalStart]<maxLevel) {
                ++firstRun;
            }
            अगर(firstRun>=run_गिनती) {
                अवरोध;  /* no more such runs */
            }

            /* look for the limit run of such a sequence (the run behind it) */
            क्रम(limitRun=firstRun; ++limitRun<run_गिनती && levels[runs[limitRun].logicalStart]>=maxLevel;) {}

            /* Swap the entire sequence of runs from firstRun to limitRun-1. */
            endRun=limitRun-1;
            जबतक(firstRun<endRun) {
                अस्थायी_Run = runs[firstRun];
                runs[firstRun]=runs[endRun];
                runs[endRun]=अस्थायी_Run;
                ++firstRun;
                --endRun;
            }

            अगर(limitRun==run_गिनती) {
                अवरोध;  /* no more such runs */
            } अन्यथा {
                firstRun=limitRun+1;
            }
        }
    }

    /* now do maxLevel==old minLevel (==odd!), see above */
    अगर(!(minLevel&1)) {
        firstRun=0;

        /* include the trailing WS run in this complete reordering */
        अगर(pBiDi->trailingWSStart==pBiDi->length) {
            --run_गिनती;
        }

        /* Swap the entire sequence of all runs. (endRun==runCount) */
        जबतक(firstRun<run_गिनती) {
            अस्थायी_Run=runs[firstRun];
            runs[firstRun]=runs[run_गिनती];
            runs[run_गिनती]=अस्थायी_Run;
            ++firstRun;
            --run_गिनती;
        }
    }
}

/* compute the runs array --------------------------------------------------- */

अटल int32_t get_Run_From_Logical_सूचकांक(UBiDi *pBiDi, int32_t logical_सूचकांक) {
    Run *runs=pBiDi->runs;
    int32_t run_गिनती=pBiDi->run_गिनती, visualStart=0, i, length, logicalStart;

    क्रम(i=0; i<run_गिनती; i++) {
        length=runs[i].visualLimit-visualStart;
        logicalStart=GET_सूचकांक(runs[i].logicalStart);
        अगर((logical_सूचकांक>=logicalStart) && (logical_सूचकांक<(logicalStart+length))) {
            वापस i;
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
    अगर (pBiDi->run_गिनती>=0) {
        वापस true;
    }

    अगर(pBiDi->direction!=UBIDI_MIXED) {
        /* simple, single-run case - this covers length==0 */
        /* pBiDi->paraLevel is ok even for contextual multiple paragraphs */
        getSingleRun(pBiDi, pBiDi->paraLevel);
    } अन्यथा /* UBIDI_MIXED, length>0 */ {
        /* mixed directionality */
        int32_t length=pBiDi->length, limit;
        UBiDiLevel *levels=pBiDi->levels;
        int32_t i, run_गिनती;
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
        run_गिनती=0;
        क्रम(i=0; i<limit; ++i) {
            /* increment runCount at the start of each run */
            अगर(levels[i]!=level) {
                ++run_गिनती;
                level=levels[i];
            }
        }

        /*
         * We don't need to see if the last run can be merged with a trailing
         * WS run because setTrailingWSStart() would have done that.
         */
        अगर(run_गिनती==1 && limit==length) {
            /* There is only one non-WS run and no trailing WS-run. */
            getSingleRun(pBiDi, levels[0]);
        } अन्यथा /* runCount>1 || limit<length */ {
            /* allocate and set the runs */
            Run *runs;
            int32_t run_सूचकांक, start;
            UBiDiLevel minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1, maxLevel=0;

            /* now, count a (non-mergeable) WS run */
            अगर(limit<length) {
                ++run_गिनती;
            }

            /* runCount>1 */
            अगर(getRunsMemory(pBiDi, run_गिनती)) {
                runs=pBiDi->runsMemory;
            } अन्यथा {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                वापस false;
            }

            /* set the runs */
            /* FOOD FOR THOUGHT: this could be optimized, e.g.:
             * 464->444, 484->444, 575->555, 595->555
             * However, that would take longer. Check also how it would
             * interact with BiDi control removal and inserting Marks.
             */
            run_सूचकांक=0;

            /* search for the run limits and initialize visualLimit values with the run lengths */
            i=0;
            करो {
                /* prepare this run */
                start=i;
                level=levels[i];
                अगर(level<minLevel) {
                    minLevel=level;
                }
                अगर(level>maxLevel) {
                    maxLevel=level;
                }

                /* look for the run limit */
                जबतक(++i<limit && levels[i]==level) {}

                /* i is another run limit */
                runs[run_सूचकांक].logicalStart=start;
                runs[run_सूचकांक].visualLimit=i-start;
                runs[run_सूचकांक].जोड़ो_Remove=0;
                ++run_सूचकांक;
            } जबतक(i<limit);

            अगर(limit<length) {
                /* there is a separate WS run */
                runs[run_सूचकांक].logicalStart=limit;
                runs[run_सूचकांक].visualLimit=length-limit;
                /* For the trailing WS run, pBiDi->paraLevel is ok even
                   if contextual multiple paragraphs.                   */
                अगर(pBiDi->paraLevel<minLevel) {
                    minLevel=pBiDi->paraLevel;
                }
            }

            /* set the object fields */
            pBiDi->runs=runs;
            pBiDi->run_गिनती=run_गिनती;

            reorder_पंक्ति(pBiDi, minLevel, maxLevel);

            /* now add the direction flags and adjust the visualLimit's to be just that */
            /* this loop will also handle the trailing WS run */
            limit=0;
            क्रम(i=0; i<run_गिनती; ++i) {
                ADD_ODD_BIT_FROM_LEVEL(runs[i].logicalStart, levels[runs[i].logicalStart]);
                limit+=runs[i].visualLimit;
                runs[i].visualLimit=limit;
            }

            /* Set the "odd" bit for the trailing WS run. */
            /* For a RTL paragraph, it will be the *first* run in visual order. */
            /* For the trailing WS run, pBiDi->paraLevel is ok even if
               contextual multiple paragraphs.                          */
            अगर(run_सूचकांक<run_गिनती) {
                int32_t trailingRun = ((pBiDi->paraLevel & 1) != 0)? 0 : run_सूचकांक;

                ADD_ODD_BIT_FROM_LEVEL(runs[trailingRun].logicalStart, pBiDi->paraLevel);
            }
        }
    }

    /* handle insert LRM/RLM BEFORE/AFTER run */
    अगर(pBiDi->जोड़ो_Points.size>0) {
        Point *point, *start=pBiDi->जोड़ो_Points.points,
                      *limit=start+pBiDi->जोड़ो_Points.size;
        int32_t run_सूचकांक;
        क्रम(point=start; point<limit; point++) {
            run_सूचकांक=get_Run_From_Logical_सूचकांक(pBiDi, point->pos);
            pBiDi->runs[run_सूचकांक].जोड़ो_Remove|=point->flag;
        }
    }

    /* handle remove BiDi control characters */
    अगर(pBiDi->control_गिनती>0) {
        int32_t run_सूचकांक;
        स्थिर char16_t *start=pBiDi->text, *limit=start+pBiDi->length, *pu;
        क्रम(pu=start; pu<limit; pu++) {
            अगर(IS_BIDI_CONTROL_CHAR(*pu)) {
                run_सूचकांक=get_Run_From_Logical_सूचकांक(pBiDi, (int32_t)(pu-start));
                pBiDi->runs[run_सूचकांक].जोड़ो_Remove--;
            }
        }
    }

    वापस true;
}

अटल UBool
prepareReorder(स्थिर UBiDiLevel *levels, int32_t length,
               int32_t *सूचकांक_Map,
               UBiDiLevel *pMinLevel, UBiDiLevel *pMaxLevel) {
    int32_t start;
    UBiDiLevel level, minLevel, maxLevel;

    अगर(levels==nullptr || length<=0) {
        वापस false;
    }

    /* determine minLevel and maxLevel */
    minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1;
    maxLevel=0;
    क्रम(start=length; start>0;) {
        level=levels[--start];
        अगर(level>UBIDI_MAX_EXPLICIT_LEVEL+1) {
            वापस false;
        }
        अगर(level<minLevel) {
            minLevel=level;
        }
        अगर(level>maxLevel) {
            maxLevel=level;
        }
    }
    *pMinLevel=minLevel;
    *pMaxLevel=maxLevel;

    /* initialize the index map */
    क्रम(start=length; start>0;) {
        --start;
        सूचकांक_Map[start]=start;
    }

    वापस true;
}

/* reorder a line based on a levels array (L2) ------------------------------ */

U_CAPI व्योम U_EXPORT2
ubidi_reorderLogical(स्थिर UBiDiLevel *levels, int32_t length, int32_t *सूचकांक_Map) {
    int32_t start, limit, योग_Of_Sos_Eos;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    अगर(सूचकांक_Map==nullptr || !prepareReorder(levels, length, सूचकांक_Map, &minLevel, &maxLevel)) {
        वापस;
    }

    /* nothing to do? */
    अगर(minLevel==maxLevel && (minLevel&1)==0) {
        वापस;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    करो {
        start=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        क्रम(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            जबतक(start<length && levels[start]<maxLevel) {
                ++start;
            }
            अगर(start>=length) {
                अवरोध;  /* no more such sequences */
            }

            /* look for the limit of such a sequence (the index behind it) */
            क्रम(limit=start; ++limit<length && levels[limit]>=maxLevel;) {}

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
            योग_Of_Sos_Eos=start+limit-1;

            /* reorder each index in the sequence */
            करो {
                सूचकांक_Map[start]=योग_Of_Sos_Eos-सूचकांक_Map[start];
            } जबतक(++start<limit);

            /* start==limit */
            अगर(limit==length) {
                अवरोध;  /* no more such sequences */
            } अन्यथा {
                start=limit+1;
            }
        }
    } जबतक(--maxLevel>=minLevel);
}

U_CAPI व्योम U_EXPORT2
ubidi_reorderVisual(स्थिर UBiDiLevel *levels, int32_t length, int32_t *सूचकांक_Map) {
    int32_t start, end, limit, अस्थायी;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    अगर(सूचकांक_Map==nullptr || !prepareReorder(levels, length, सूचकांक_Map, &minLevel, &maxLevel)) {
        वापस;
    }

    /* nothing to do? */
    अगर(minLevel==maxLevel && (minLevel&1)==0) {
        वापस;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    करो {
        start=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        क्रम(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            जबतक(start<length && levels[start]<maxLevel) {
                ++start;
            }
            अगर(start>=length) {
                अवरोध;  /* no more such runs */
            }

            /* look for the limit of such a sequence (the index behind it) */
            क्रम(limit=start; ++limit<length && levels[limit]>=maxLevel;) {}

            /*
             * Swap the entire interval of indexes from start to limit-1.
             * We don't need to swap the levels for the purpose of this
             * algorithm: the sequence of levels that we look at does not
             * move anyway.
             */
            end=limit-1;
            जबतक(start<end) {
                अस्थायी=सूचकांक_Map[start];
                सूचकांक_Map[start]=सूचकांक_Map[end];
                सूचकांक_Map[end]=अस्थायी;

                ++start;
                --end;
            }

            अगर(limit==length) {
                अवरोध;  /* no more such sequences */
            } अन्यथा {
                start=limit+1;
            }
        }
    } जबतक(--maxLevel>=minLevel);
}

/* API functions for logical<->visual mapping ------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_get_Visual_सूचकांक(UBiDi *pBiDi, int32_t logical_सूचकांक, UErrorCode *pErrorCode) {
    int32_t visual_सूचकांक=UBIDI_MAP_NOWHERE;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_पंक्ति(pBiDi, *pErrorCode, -1);
    RETURN_IF_BAD_RANGE(logical_सूचकांक, 0, pBiDi->length, *pErrorCode, -1);

    /* we can do the trivial cases without the runs array */
    चयन(pBiDi->direction) {
    हाल UBIDI_LTR:
        visual_सूचकांक=logical_सूचकांक;
        अवरोध;
    हाल UBIDI_RTL:
        visual_सूचकांक=pBiDi->length-logical_सूचकांक-1;
        अवरोध;
    शेष:
        अगर(!ubidi_getRuns(pBiDi, pErrorCode)) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस -1;
        } अन्यथा {
            Run *runs=pBiDi->runs;
            int32_t i, visualStart=0, offset, length;

            /* linear search for the run, search on the visual runs */
            क्रम(i=0; i<pBiDi->run_गिनती; ++i) {
                length=runs[i].visualLimit-visualStart;
                offset=logical_सूचकांक-GET_सूचकांक(runs[i].logicalStart);
                अगर(offset>=0 && offset<length) {
                    अगर(IS_EVEN_RUN(runs[i].logicalStart)) {
                        /* LTR */
                        visual_सूचकांक=visualStart+offset;
                    } अन्यथा {
                        /* RTL */
                        visual_सूचकांक=visualStart+length-offset-1;
                    }
                    अवरोध;          /* exit for loop */
                }
                visualStart+=length;
            }
            अगर(i>=pBiDi->run_गिनती) {
                वापस UBIDI_MAP_NOWHERE;
            }
        }
    }

    अगर(pBiDi->जोड़ो_Points.size>0) {
        /* add the number of added marks until the calculated visual index */
        Run *runs=pBiDi->runs;
        int32_t i, length, जोड़ो_Remove;
        int32_t visualStart=0, markFound=0;
        क्रम(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            जोड़ो_Remove=runs[i].जोड़ो_Remove;
            अगर(जोड़ो_Remove & (LRM_BEFORE|RLM_BEFORE)) {
                markFound++;
            }
            /* is it the run containing the visual index? */
            अगर(visual_सूचकांक<runs[i].visualLimit) {
                वापस visual_सूचकांक+markFound;
            }
            अगर(जोड़ो_Remove & (LRM_AFTER|RLM_AFTER)) {
                markFound++;
            }
        }
    }
    अन्यथा अगर(pBiDi->control_गिनती>0) {
        /* subtract the number of controls until the calculated visual index */
        Run *runs=pBiDi->runs;
        int32_t i, j, start, limit, length, जोड़ो_Remove;
        int32_t visualStart=0, controlFound=0;
        char16_t uchar=pBiDi->text[logical_सूचकांक];
        /* is the logical index pointing to a control ? */
        अगर(IS_BIDI_CONTROL_CHAR(uchar)) {
            वापस UBIDI_MAP_NOWHERE;
        }
        /* loop on runs */
        क्रम(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            जोड़ो_Remove=runs[i].जोड़ो_Remove;
            /* calculated visual index is beyond this run? */
            अगर(visual_सूचकांक>=runs[i].visualLimit) {
                controlFound-=जोड़ो_Remove;
                जारी;
            }
            /* calculated visual index must be within current run */
            अगर(जोड़ो_Remove==0) {
                वापस visual_सूचकांक-controlFound;
            }
            अगर(IS_EVEN_RUN(runs[i].logicalStart)) {
                /* LTR: check from run start to logical index */
                start=runs[i].logicalStart;
                limit=logical_सूचकांक;
            } अन्यथा {
                /* RTL: check from logical index to run end */
                start=logical_सूचकांक+1;
                limit=GET_सूचकांक(runs[i].logicalStart)+length;
            }
            क्रम(j=start; j<limit; j++) {
                uchar=pBiDi->text[j];
                अगर(IS_BIDI_CONTROL_CHAR(uchar)) {
                    controlFound++;
                }
            }
            वापस visual_सूचकांक-controlFound;
        }
    }

    वापस visual_सूचकांक;
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Logical_सूचकांक(UBiDi *pBiDi, int32_t visual_सूचकांक, UErrorCode *pErrorCode) {
    Run *runs;
    int32_t i, run_गिनती, start;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_पंक्ति(pBiDi, *pErrorCode, -1);
    RETURN_IF_BAD_RANGE(visual_सूचकांक, 0, pBiDi->परिणाम_लंबाई, *pErrorCode, -1);
    /* we can do the trivial cases without the runs array */
    अगर(pBiDi->जोड़ो_Points.size==0 && pBiDi->control_गिनती==0) {
        अगर(pBiDi->direction==UBIDI_LTR) {
            वापस visual_सूचकांक;
        }
        अन्यथा अगर(pBiDi->direction==UBIDI_RTL) {
            वापस pBiDi->length-visual_सूचकांक-1;
        }
    }
    अगर(!ubidi_getRuns(pBiDi, pErrorCode)) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस -1;
    }

    runs=pBiDi->runs;
    run_गिनती=pBiDi->run_गिनती;
    अगर(pBiDi->जोड़ो_Points.size>0) {
        /* handle inserted LRM/RLM */
        int32_t markFound=0, जोड़ो_Remove;
        int32_t visualStart=0, length;
        runs=pBiDi->runs;
        /* subtract number of marks until visual index */
        क्रम(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            जोड़ो_Remove=runs[i].जोड़ो_Remove;
            अगर(जोड़ो_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                अगर(visual_सूचकांक<=(visualStart+markFound)) {
                    वापस UBIDI_MAP_NOWHERE;
                }
                markFound++;
            }
            /* is adjusted visual index within this run? */
            अगर(visual_सूचकांक<(runs[i].visualLimit+markFound)) {
                visual_सूचकांक-=markFound;
                अवरोध;
            }
            अगर(जोड़ो_Remove&(LRM_AFTER|RLM_AFTER)) {
                अगर(visual_सूचकांक==(visualStart+length+markFound)) {
                    वापस UBIDI_MAP_NOWHERE;
                }
                markFound++;
            }
        }
    }
    अन्यथा अगर(pBiDi->control_गिनती>0) {
        /* handle removed BiDi control characters */
        int32_t controlFound=0, जोड़ो_Remove, length;
        int32_t logicalStart, logicalEnd, visualStart=0, j, k;
        char16_t uchar;
        UBool evenRun;
        /* add number of controls until visual index */
        क्रम(i=0; ; i++, visualStart+=length) {
            length=runs[i].visualLimit-visualStart;
            जोड़ो_Remove=runs[i].जोड़ो_Remove;
            /* is adjusted visual index beyond current run? */
            अगर(visual_सूचकांक>=(runs[i].visualLimit-controlFound+जोड़ो_Remove)) {
                controlFound-=जोड़ो_Remove;
                जारी;
            }
            /* adjusted visual index is within current run */
            अगर(जोड़ो_Remove==0) {
                visual_सूचकांक+=controlFound;
                अवरोध;
            }
            /* count non-control chars until visualIndex */
            logicalStart=runs[i].logicalStart;
            evenRun=IS_EVEN_RUN(logicalStart);
            REMOVE_ODD_BIT(logicalStart);
            logicalEnd=logicalStart+length-1;
            क्रम(j=0; j<length; j++) {
                k= evenRun ? logicalStart+j : logicalEnd-j;
                uchar=pBiDi->text[k];
                अगर(IS_BIDI_CONTROL_CHAR(uchar)) {
                    controlFound++;
                }
                अगर((visual_सूचकांक+controlFound)==(visualStart+j)) {
                    अवरोध;
                }
            }
            visual_सूचकांक+=controlFound;
            अवरोध;
        }
    }
    /* handle all cases */
    अगर(run_गिनती<=10) {
        /* linear search for the run */
        क्रम(i=0; visual_सूचकांक>=runs[i].visualLimit; ++i) {}
    } अन्यथा {
        /* binary search for the run */
        int32_t begin=0, limit=run_गिनती;

        /* the middle if() is guaranteed to find the run, we don't need a loop limit */
        क्रम(;;) {
            i=(begin+limit)/2;
            अगर(visual_सूचकांक>=runs[i].visualLimit) {
                begin=i+1;
            } अन्यथा अगर(i==0 || visual_सूचकांक>=runs[i-1].visualLimit) {
                अवरोध;
            } अन्यथा {
                limit=i;
            }
        }
    }

    start=runs[i].logicalStart;
    अगर(IS_EVEN_RUN(start)) {
        /* LTR */
        /* the offset in runs[i] is visualIndex-runs[i-1].visualLimit */
        अगर(i>0) {
            visual_सूचकांक-=runs[i-1].visualLimit;
        }
        वापस start+visual_सूचकांक;
    } अन्यथा {
        /* RTL */
        वापस GET_सूचकांक(start)+runs[i].visualLimit-visual_सूचकांक-1;
    }
}

U_CAPI व्योम U_EXPORT2
ubidi_getLogicalMap(UBiDi *pBiDi, int32_t *सूचकांक_Map, UErrorCode *pErrorCode) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_गिनती_Runs(pBiDi, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        /* no op */
    } अन्यथा अगर(सूचकांक_Map==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } अन्यथा {
        /* fill a logical-to-visual index map using the runs[] */
        int32_t visualStart, visualLimit, i, j, k;
        int32_t logicalStart, logicalLimit;
        Run *runs=pBiDi->runs;
        अगर (pBiDi->length<=0) {
            वापस;
        }
        अगर (pBiDi->length>pBiDi->परिणाम_लंबाई) {
            uprv_memset(सूचकांक_Map, 0xFF, pBiDi->length*माप(int32_t));
        }

        visualStart=0;
        क्रम(j=0; j<pBiDi->run_गिनती; ++j) {
            logicalStart=GET_सूचकांक(runs[j].logicalStart);
            visualLimit=runs[j].visualLimit;
            अगर(IS_EVEN_RUN(runs[j].logicalStart)) {
                करो { /* LTR */
                    सूचकांक_Map[logicalStart++]=visualStart++;
                } जबतक(visualStart<visualLimit);
            } अन्यथा {
                logicalStart+=visualLimit-visualStart;  /* logicalLimit */
                करो { /* RTL */
                    सूचकांक_Map[--logicalStart]=visualStart++;
                } जबतक(visualStart<visualLimit);
            }
            /* visualStart==visualLimit; */
        }

        अगर(pBiDi->जोड़ो_Points.size>0) {
            int32_t markFound=0, run_गिनती=pBiDi->run_गिनती;
            int32_t length, जोड़ो_Remove;
            visualStart=0;
            /* add number of marks found until each index */
            क्रम(i=0; i<run_गिनती; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                जोड़ो_Remove=runs[i].जोड़ो_Remove;
                अगर(जोड़ो_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    markFound++;
                }
                अगर(markFound>0) {
                    logicalStart=GET_सूचकांक(runs[i].logicalStart);
                    logicalLimit=logicalStart+length;
                    क्रम(j=logicalStart; j<logicalLimit; j++) {
                        सूचकांक_Map[j]+=markFound;
                    }
                }
                अगर(जोड़ो_Remove&(LRM_AFTER|RLM_AFTER)) {
                    markFound++;
                }
            }
        }
        अन्यथा अगर(pBiDi->control_गिनती>0) {
            int32_t controlFound=0, run_गिनती=pBiDi->run_गिनती;
            int32_t length, जोड़ो_Remove;
            UBool evenRun;
            char16_t uchar;
            visualStart=0;
            /* subtract number of controls found until each index */
            क्रम(i=0; i<run_गिनती; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                जोड़ो_Remove=runs[i].जोड़ो_Remove;
                /* no control found within previous runs nor within this run */
                अगर((controlFound-जोड़ो_Remove)==0) {
                    जारी;
                }
                logicalStart=runs[i].logicalStart;
                evenRun=IS_EVEN_RUN(logicalStart);
                REMOVE_ODD_BIT(logicalStart);
                logicalLimit=logicalStart+length;
                /* if no control within this run */
                अगर(जोड़ो_Remove==0) {
                    क्रम(j=logicalStart; j<logicalLimit; j++) {
                        सूचकांक_Map[j]-=controlFound;
                    }
                    जारी;
                }
                क्रम(j=0; j<length; j++) {
                    k= evenRun ? logicalStart+j : logicalLimit-j-1;
                    uchar=pBiDi->text[k];
                    अगर(IS_BIDI_CONTROL_CHAR(uchar)) {
                        controlFound++;
                        सूचकांक_Map[k]=UBIDI_MAP_NOWHERE;
                        जारी;
                    }
                    सूचकांक_Map[k]-=controlFound;
                }
            }
        }
    }
}

U_CAPI व्योम U_EXPORT2
ubidi_getVisualMap(UBiDi *pBiDi, int32_t *सूचकांक_Map, UErrorCode *pErrorCode) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    अगर(सूचकांक_Map==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_गिनती_Runs(pBiDi, pErrorCode);
    अगर(U_SUCCESS(*pErrorCode)) {
        /* fill a visual-to-logical index map using the runs[] */
        Run *runs=pBiDi->runs, *runsLimit=runs+pBiDi->run_गिनती;
        int32_t logicalStart, visualStart, visualLimit, *pi=सूचकांक_Map;

        अगर (pBiDi->परिणाम_लंबाई<=0) {
            वापस;
        }
        visualStart=0;
        क्रम(; runs<runsLimit; ++runs) {
            logicalStart=runs->logicalStart;
            visualLimit=runs->visualLimit;
            अगर(IS_EVEN_RUN(logicalStart)) {
                करो { /* LTR */
                    *pi++ = logicalStart++;
                } जबतक(++visualStart<visualLimit);
            } अन्यथा {
                REMOVE_ODD_BIT(logicalStart);
                logicalStart+=visualLimit-visualStart;  /* logicalLimit */
                करो { /* RTL */
                    *pi++ = --logicalStart;
                } जबतक(++visualStart<visualLimit);
            }
            /* visualStart==visualLimit; */
        }

        अगर(pBiDi->जोड़ो_Points.size>0) {
            int32_t markFound=0, run_गिनती=pBiDi->run_गिनती;
            int32_t जोड़ो_Remove, i, j, k;
            runs=pBiDi->runs;
            /* count all inserted marks */
            क्रम(i=0; i<run_गिनती; i++) {
                जोड़ो_Remove=runs[i].जोड़ो_Remove;
                अगर(जोड़ो_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    markFound++;
                }
                अगर(जोड़ो_Remove&(LRM_AFTER|RLM_AFTER)) {
                    markFound++;
                }
            }
            /* move back indexes by number of preceding marks */
            k=pBiDi->परिणाम_लंबाई;
            क्रम(i=run_गिनती-1; i>=0 && markFound>0; i--) {
                जोड़ो_Remove=runs[i].जोड़ो_Remove;
                अगर(जोड़ो_Remove&(LRM_AFTER|RLM_AFTER)) {
                    सूचकांक_Map[--k]= UBIDI_MAP_NOWHERE;
                    markFound--;
                }
                visualStart= i>0 ? runs[i-1].visualLimit : 0;
                क्रम(j=runs[i].visualLimit-1; j>=visualStart && markFound>0; j--) {
                    सूचकांक_Map[--k]=सूचकांक_Map[j];
                }
                अगर(जोड़ो_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    सूचकांक_Map[--k]= UBIDI_MAP_NOWHERE;
                    markFound--;
                }
            }
        }
        अन्यथा अगर(pBiDi->control_गिनती>0) {
            int32_t run_गिनती=pBiDi->run_गिनती, logicalEnd;
            int32_t जोड़ो_Remove, length, i, j, k, m;
            char16_t uchar;
            UBool evenRun;
            runs=pBiDi->runs;
            visualStart=0;
            /* move forward indexes by number of preceding controls */
            k=0;
            क्रम(i=0; i<run_गिनती; i++, visualStart+=length) {
                length=runs[i].visualLimit-visualStart;
                जोड़ो_Remove=runs[i].जोड़ो_Remove;
                /* if no control found yet, nothing to do in this run */
                अगर((जोड़ो_Remove==0)&&(k==visualStart)) {
                    k+=length;
                    जारी;
                }
                /* if no control in this run */
                अगर(जोड़ो_Remove==0) {
                    visualLimit=runs[i].visualLimit;
                    क्रम(j=visualStart; j<visualLimit; j++) {
                        सूचकांक_Map[k++]=सूचकांक_Map[j];
                    }
                    जारी;
                }
                logicalStart=runs[i].logicalStart;
                evenRun=IS_EVEN_RUN(logicalStart);
                REMOVE_ODD_BIT(logicalStart);
                logicalEnd=logicalStart+length-1;
                क्रम(j=0; j<length; j++) {
                    m= evenRun ? logicalStart+j : logicalEnd-j;
                    uchar=pBiDi->text[m];
                    अगर(!IS_BIDI_CONTROL_CHAR(uchar)) {
                        सूचकांक_Map[k++]=m;
                    }
                }
            }
        }
    }
}

U_CAPI व्योम U_EXPORT2
ubidi_invertMap(स्थिर int32_t *srcMap, int32_t *destMap, int32_t length) {
    अगर(srcMap!=nullptr && destMap!=nullptr && length>0) {
        स्थिर int32_t *pi;
        int32_t dest_लंबाई=-1, count=0;
        /* find highest value and count positive indexes in srcMap */
        pi=srcMap+length;
        जबतक(pi>srcMap) {
            अगर(*--pi>dest_लंबाई) {
                dest_लंबाई=*pi;
            }
            अगर(*pi>=0) {
                count++;
            }
        }
        dest_लंबाई++;           /* add 1 for origin 0 */
        अगर(count<dest_लंबाई) {
            /* we must fill unmatched destMap entries with -1 */
            uprv_memset(destMap, 0xFF, dest_लंबाई*माप(int32_t));
        }
        pi=srcMap+length;
        जबतक(length>0) {
            अगर(*--pi>=0) {
                destMap[*pi]=--length;
            } अन्यथा {
                --length;
            }
        }
    }
}
