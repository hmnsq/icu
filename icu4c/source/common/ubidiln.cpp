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

#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubidi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubidiimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

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
ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_Trailing_WSStart(UBiDi *pBiDi) {
    /* pBiDi->direction!=UBIDI_MIXED */

    ਅਚਲ DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *ਪੱਧਰ=pBiDi->ਪੱਧਰ;
    int32_t ਸ਼ੁਰੂ=pBiDi->length;
    UBiDiLevel paraLevel=pBiDi->paraLevel;

    /* If the line is terminated by a block separator, all preceding WS etc...
       are already set to paragraph level.
       Setting trailingWSStart to pBidi->length will avoid changing the
       level of B chars from 0 to paraLevel in ubidi_getLevels when
       orderParagraphsLTR==true.
     */
    ਜੇ(dirProps[ਸ਼ੁਰੂ-1]==B) {
        pBiDi->trailingWSStart=ਸ਼ੁਰੂ;   /* currently == pBiDi->length */
        ਵਾਪਸ;
    }
    /* go backwards across all WS, BN, explicit codes */
    ਜਦੋਂ(ਸ਼ੁਰੂ>0 && DIRPROP_FLAG(dirProps[ਸ਼ੁਰੂ-1])&ਨਕਾਬ_WS) {
        --ਸ਼ੁਰੂ;
    }

    /* if the WS run can be merged with the previous run then do so here */
    ਜਦੋਂ(ਸ਼ੁਰੂ>0 && ਪੱਧਰ[ਸ਼ੁਰੂ-1]==paraLevel) {
        --ਸ਼ੁਰੂ;
    }

    pBiDi->trailingWSStart=ਸ਼ੁਰੂ;
}

/* ubidi_setLine ------------------------------------------------------------ */

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_ਸੈੱਟ_ਸਤਰ(ਅਚਲ UBiDi *pParaBiDi,
              int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ,
              UBiDi *p_ਸਤਰ_Bi_Di,
              UError_ਕੋਡ *p_Error_ਕੋਡ) {
    int32_t length;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ਕੋਡ);
    RETURN_VOID_IF_NOT_VALID_PARA(pParaBiDi, *p_Error_ਕੋਡ);
    RETURN_VOID_IF_BAD_RANGE(ਸ਼ੁਰੂ, 0, ਹੱਦਬੰਦੀ, *p_Error_ਕੋਡ);
    RETURN_VOID_IF_BAD_RANGE(ਹੱਦਬੰਦੀ, 0, pParaBiDi->length+1, *p_Error_ਕੋਡ);
    ਜੇ(p_ਸਤਰ_Bi_Di==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ(ubidi_getParagraph(pParaBiDi, ਸ਼ੁਰੂ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, p_Error_ਕੋਡ) !=
       ubidi_getParagraph(pParaBiDi, ਹੱਦਬੰਦੀ-1, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, p_Error_ਕੋਡ)) {
        /* the line crosses a paragraph boundary */
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    /* set the values in pLineBiDi from its pParaBiDi parent */
    p_ਸਤਰ_Bi_Di->pParaBiDi=ਖਾਲੀ_ਸੂਚਕ;          /* mark unfinished setLine */
    p_ਸਤਰ_Bi_Di->ਪਾਠ=pParaBiDi->ਪਾਠ+ਸ਼ੁਰੂ;
    length=p_ਸਤਰ_Bi_Di->length=ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ;
    p_ਸਤਰ_Bi_Di->ਨਤੀਜਾ_ਲੰਮਾਈ=p_ਸਤਰ_Bi_Di->original_ਲੰਮਾਈ=length;
    p_ਸਤਰ_Bi_Di->paraLevel=GET_PARALEVEL(pParaBiDi, ਸ਼ੁਰੂ);
    p_ਸਤਰ_Bi_Di->para_ਗਿਣਤੀ=pParaBiDi->para_ਗਿਣਤੀ;
    p_ਸਤਰ_Bi_Di->runs=ਖਾਲੀ_ਸੂਚਕ;
    p_ਸਤਰ_Bi_Di->flags=0;
    p_ਸਤਰ_Bi_Di->reordering_ਬਹੁਲਕ=pParaBiDi->reordering_ਬਹੁਲਕ;
    p_ਸਤਰ_Bi_Di->reorderingOptions=pParaBiDi->reorderingOptions;
    p_ਸਤਰ_Bi_Di->control_ਗਿਣਤੀ=0;
    ਜੇ(pParaBiDi->control_ਗਿਣਤੀ>0) {
        int32_t j;
        ਲਈ(j=ਸ਼ੁਰੂ; j<ਹੱਦਬੰਦੀ; j++) {
            ਜੇ(IS_BIDI_CONTROL_CHAR(pParaBiDi->ਪਾਠ[j])) {
                p_ਸਤਰ_Bi_Di->control_ਗਿਣਤੀ++;
            }
        }
        p_ਸਤਰ_Bi_Di->ਨਤੀਜਾ_ਲੰਮਾਈ-=p_ਸਤਰ_Bi_Di->control_ਗਿਣਤੀ;
    }

    p_ਸਤਰ_Bi_Di->dirProps=pParaBiDi->dirProps+ਸ਼ੁਰੂ;
    p_ਸਤਰ_Bi_Di->ਪੱਧਰ=pParaBiDi->ਪੱਧਰ+ਸ਼ੁਰੂ;
    p_ਸਤਰ_Bi_Di->ਲੜੀਵਾਰ_ਗਿਣਤੀ=-1;

    ਜੇ(pParaBiDi->direction!=UBIDI_MIXED) {
        /* the parent is already trivial */
        p_ਸਤਰ_Bi_Di->direction=pParaBiDi->direction;

        /*
         * The parent's levels are all either
         * implicitly or explicitly ==paraLevel;
         * do the same here.
         */
        ਜੇ(pParaBiDi->trailingWSStart<=ਸ਼ੁਰੂ) {
            p_ਸਤਰ_Bi_Di->trailingWSStart=0;
        } ਨਹੀਂਤਾਂ ਜੇ(pParaBiDi->trailingWSStart<ਹੱਦਬੰਦੀ) {
            p_ਸਤਰ_Bi_Di->trailingWSStart=pParaBiDi->trailingWSStart-ਸ਼ੁਰੂ;
        } ਨਹੀਂਤਾਂ {
            p_ਸਤਰ_Bi_Di->trailingWSStart=length;
        }
    } ਨਹੀਂਤਾਂ {
        ਅਚਲ UBiDiLevel *ਪੱਧਰ=p_ਸਤਰ_Bi_Di->ਪੱਧਰ;
        int32_t i, trailingWSStart;
        UBiDiLevel level;

        ਸੈੱਟ_Trailing_WSStart(p_ਸਤਰ_Bi_Di);
        trailingWSStart=p_ਸਤਰ_Bi_Di->trailingWSStart;

        /* recalculate pLineBiDi->direction */
        ਜੇ(trailingWSStart==0) {
            /* all levels are at paraLevel */
            p_ਸਤਰ_Bi_Di->direction=(UBiDiDirection)(p_ਸਤਰ_Bi_Di->paraLevel&1);
        } ਨਹੀਂਤਾਂ {
            /* get the level of the first character */
            level=(UBiDiLevel)(ਪੱਧਰ[0]&1);

            /* if there is anything of a different level, then the line is mixed */
            ਜੇ(trailingWSStart<length && (p_ਸਤਰ_Bi_Di->paraLevel&1)!=level) {
                /* the trailing WS is at paraLevel, which differs from levels[0] */
                p_ਸਤਰ_Bi_Di->direction=UBIDI_MIXED;
            } ਨਹੀਂਤਾਂ {
                /* see if levels[1..trailingWSStart-1] have the same direction as levels[0] and paraLevel */
                i=1;
                ਲਈ(;;) {
                    ਜੇ(i==trailingWSStart) {
                        /* the direction values match those in level */
                        p_ਸਤਰ_Bi_Di->direction=(UBiDiDirection)level;
                        ਰੋਕੋ;
                    } ਨਹੀਂਤਾਂ ਜੇ((ਪੱਧਰ[i]&1)!=level) {
                        p_ਸਤਰ_Bi_Di->direction=UBIDI_MIXED;
                        ਰੋਕੋ;
                    }
                    ++i;
                }
            }
        }

        ਚੋਣ(p_ਸਤਰ_Bi_Di->direction) {
        ਹਾਲਤ UBIDI_LTR:
            /* make sure paraLevel is even */
            p_ਸਤਰ_Bi_Di->paraLevel=(UBiDiLevel)((p_ਸਤਰ_Bi_Di->paraLevel+1)&~1);

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_ਸਤਰ_Bi_Di->trailingWSStart=0;
            ਰੋਕੋ;
        ਹਾਲਤ UBIDI_RTL:
            /* make sure paraLevel is odd */
            p_ਸਤਰ_Bi_Di->paraLevel|=1;

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_ਸਤਰ_Bi_Di->trailingWSStart=0;
            ਰੋਕੋ;
        ਮੂਲ:
            ਰੋਕੋ;
        }
    }
    p_ਸਤਰ_Bi_Di->pParaBiDi=pParaBiDi;     /* mark successful setLine */
}

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getLevelAt(ਅਚਲ UBiDi *pBiDi, int32_t char_ਸੂਚਕਾਂਕ) {
    /* return paraLevel if in the trailing WS run, otherwise the real level */
    ਜੇ(!IS_VALID_PARA_OR_ਸਤਰ(pBiDi) || char_ਸੂਚਕਾਂਕ<0 || pBiDi->length<=char_ਸੂਚਕਾਂਕ) {
        ਵਾਪਸ 0;
    } ਨਹੀਂਤਾਂ ਜੇ(pBiDi->direction!=UBIDI_MIXED || char_ਸੂਚਕਾਂਕ>=pBiDi->trailingWSStart) {
        ਵਾਪਸ GET_PARALEVEL(pBiDi, char_ਸੂਚਕਾਂਕ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ pBiDi->ਪੱਧਰ[char_ਸੂਚਕਾਂਕ];
    }
}

U_CAPI ਅਚਲ UBiDiLevel* U_EXPORT2
ubidi_get_ਪੱਧਰ(UBiDi* pBiDi, UError_ਕੋਡ* p_Error_ਕੋਡ) {
    int32_t ਸ਼ੁਰੂ, length;

    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ਕੋਡ, ਖਾਲੀ_ਸੂਚਕ);
    RETURN_IF_NOT_VALID_PARA_OR_ਸਤਰ(pBiDi, *p_Error_ਕੋਡ, ਖਾਲੀ_ਸੂਚਕ);
    ਜੇ((length=pBiDi->length)<=0) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ((ਸ਼ੁਰੂ=pBiDi->trailingWSStart)==length) {
        /* the current levels array reflects the WS run */
        ਵਾਪਸ pBiDi->ਪੱਧਰ;
    }

    /*
     * After the previous if(), we know that the levels array
     * has an implicit trailing WS run and therefore does not fully
     * reflect itself all the levels.
     * This must be a UBiDi object for a line, and
     * we need to create a new levels array.
     */
    ਜੇ(get_ਪੱਧਰ_ਸਮ੍ਰਿਤੀ(pBiDi, length)) {
        UBiDiLevel *ਪੱਧਰ=pBiDi->ਪੱਧਰ_ਸਮ੍ਰਿਤੀ;

        ਜੇ(ਸ਼ੁਰੂ>0 && ਪੱਧਰ!=pBiDi->ਪੱਧਰ) {
            uprv_memcpy(ਪੱਧਰ, pBiDi->ਪੱਧਰ, ਸ਼ੁਰੂ);
        }
        /* pBiDi->paraLevel is ok even if contextual multiple paragraphs,
           since pBidi is a line object                                     */
        uprv_memset(ਪੱਧਰ+ਸ਼ੁਰੂ, pBiDi->paraLevel, length-ਸ਼ੁਰੂ);

        /* this new levels array is set for the line and reflects the WS run */
        pBiDi->trailingWSStart=length;
        ਵਾਪਸ pBiDi->ਪੱਧਰ=ਪੱਧਰ;
    } ਨਹੀਂਤਾਂ {
        /* out of memory */
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_get_Logical_ਲੜੀਵਾਰ(ਅਚਲ UBiDi *pBiDi, int32_t logicalPosition,
                    int32_t *p_Logical_ਹੱਦਬੰਦੀ, UBiDiLevel *pLevel) {
    UError_ਕੋਡ error_ਕੋਡ;
    int32_t ਲੜੀਵਾਰ_ਗਿਣਤੀ, visual_ਸ਼ੁਰੂ, logical_ਹੱਦਬੰਦੀ, logical_ਪਹਿਲਾ, i;
    ਲੜੀਵਾਰ i_ਲੜੀਵਾਰ;

    error_ਕੋਡ=U_ZERO_ERROR;
    RETURN_VOID_IF_BAD_RANGE(logicalPosition, 0, pBiDi->length, error_ਕੋਡ);
    /* ubidi_countRuns will check VALID_PARA_OR_LINE */
    ਲੜੀਵਾਰ_ਗਿਣਤੀ=ubidi_ਗਿਣਤੀ_Runs((UBiDi *)pBiDi, &error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    /* this is done based on runs rather than on levels since levels have
       a special interpretation when UBIDI_REORDER_RUNS_ONLY
     */
    visual_ਸ਼ੁਰੂ=logical_ਹੱਦਬੰਦੀ=0;
    i_ਲੜੀਵਾਰ=pBiDi->runs[0];

    ਲਈ(i=0; i<ਲੜੀਵਾਰ_ਗਿਣਤੀ; i++) {
        i_ਲੜੀਵਾਰ = pBiDi->runs[i];
        logical_ਪਹਿਲਾ=GET_ਸੂਚਕਾਂਕ(i_ਲੜੀਵਾਰ.logical_ਸ਼ੁਰੂ);
        logical_ਹੱਦਬੰਦੀ=logical_ਪਹਿਲਾ+i_ਲੜੀਵਾਰ.visual_ਹੱਦਬੰਦੀ-visual_ਸ਼ੁਰੂ;
        ਜੇ((logicalPosition>=logical_ਪਹਿਲਾ) &&
           (logicalPosition<logical_ਹੱਦਬੰਦੀ)) {
            ਰੋਕੋ;
        }
        visual_ਸ਼ੁਰੂ = i_ਲੜੀਵਾਰ.visual_ਹੱਦਬੰਦੀ;
    }
    ਜੇ(p_Logical_ਹੱਦਬੰਦੀ) {
        *p_Logical_ਹੱਦਬੰਦੀ=logical_ਹੱਦਬੰਦੀ;
    }
    ਜੇ(pLevel) {
        ਜੇ(pBiDi->reordering_ਬਹੁਲਕ==UBIDI_REORDER_RUNS_ONLY) {
            *pLevel=(UBiDiLevel)GET_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ(i_ਲੜੀਵਾਰ.logical_ਸ਼ੁਰੂ);
        }
        ਨਹੀਂਤਾਂ ਜੇ(pBiDi->direction!=UBIDI_MIXED || logicalPosition>=pBiDi->trailingWSStart) {
            *pLevel=GET_PARALEVEL(pBiDi, logicalPosition);
        } ਨਹੀਂਤਾਂ {
        *pLevel=pBiDi->ਪੱਧਰ[logicalPosition];
        }
    }
}

/* runs API functions ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_ਗਿਣਤੀ_Runs(UBiDi *pBiDi, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ਕੋਡ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_ਸਤਰ(pBiDi, *p_Error_ਕੋਡ, -1);
    ubidi_getRuns(pBiDi, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ -1;
    }
    ਵਾਪਸ pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ;
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_Visual_ਲੜੀਵਾਰ(UBiDi *pBiDi, int32_t ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ,
                   int32_t *p_Logical_ਸ਼ੁਰੂ, int32_t *p_ਲੰਮਾਈ)
{
    int32_t ਸ਼ੁਰੂ;
    UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
    RETURN_IF_NOT_VALID_PARA_OR_ਸਤਰ(pBiDi, error_ਕੋਡ, UBIDI_LTR);
    ubidi_getRuns(pBiDi, &error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ UBIDI_LTR;
    }
    RETURN_IF_BAD_RANGE(ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ, 0, pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ, error_ਕੋਡ, UBIDI_LTR);

    ਸ਼ੁਰੂ=pBiDi->runs[ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ].logical_ਸ਼ੁਰੂ;
    ਜੇ(p_Logical_ਸ਼ੁਰੂ!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_Logical_ਸ਼ੁਰੂ=GET_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ);
    }
    ਜੇ(p_ਲੰਮਾਈ!=ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ>0) {
            *p_ਲੰਮਾਈ=pBiDi->runs[ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ].visual_ਹੱਦਬੰਦੀ-
                     pBiDi->runs[ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ-1].visual_ਹੱਦਬੰਦੀ;
        } ਨਹੀਂਤਾਂ {
            *p_ਲੰਮਾਈ=pBiDi->runs[0].visual_ਹੱਦਬੰਦੀ;
        }
    }
    ਵਾਪਸ (UBiDiDirection)GET_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ(ਸ਼ੁਰੂ);
}

/* in trivial cases there is only one trivial run; called by ubidi_getRuns() */
ਸਥਿਰ ਖਾਲੀ
get_Single_ਲੜੀਵਾਰ(UBiDi *pBiDi, UBiDiLevel level) {
    /* simple, single-run case */
    pBiDi->runs=pBiDi->simpleRuns;
    pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ=1;

    /* fill and reorder the single run */
    pBiDi->runs[0].logical_ਸ਼ੁਰੂ=MAKE_ਸੂਚਕਾਂਕ_ODD_PAIR(0, level);
    pBiDi->runs[0].visual_ਹੱਦਬੰਦੀ=pBiDi->length;
    pBiDi->runs[0].ਪਾਓ_Remove=0;
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
ਸਥਿਰ ਖਾਲੀ
reorder_ਸਤਰ(UBiDi *pBiDi, UBiDiLevel minLevel, UBiDiLevel maxLevel) {
    ਲੜੀਵਾਰ *runs, ਆਰਜ਼ੀ_ਲੜੀਵਾਰ;
    UBiDiLevel *ਪੱਧਰ;
    int32_t ਪਹਿਲਾ_ਲੜੀਵਾਰ, ਸਿਰਾ_ਲੜੀਵਾਰ, ਹੱਦਬੰਦੀ_ਲੜੀਵਾਰ, ਲੜੀਵਾਰ_ਗਿਣਤੀ;

    /* nothing to do? */
    ਜੇ(maxLevel<=(minLevel|1)) {
        ਵਾਪਸ;
    }

    /*
     * Reorder only down to the lowest odd level
     * and reorder at an odd minLevel in a separate, simpler loop.
     * See comments above for why minLevel is always incremented.
     */
    ++minLevel;

    runs=pBiDi->runs;
    ਪੱਧਰ=pBiDi->ਪੱਧਰ;
    ਲੜੀਵਾਰ_ਗਿਣਤੀ=pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ;

    /* do not include the WS run at paraLevel<=old minLevel except in the simple loop */
    ਜੇ(pBiDi->trailingWSStart<pBiDi->length) {
        --ਲੜੀਵਾਰ_ਗਿਣਤੀ;
    }

    ਜਦੋਂ(--maxLevel>=minLevel) {
        ਪਹਿਲਾ_ਲੜੀਵਾਰ=0;

        /* loop for all sequences of runs */
        ਲਈ(;;) {
            /* look for a sequence of runs that are all at >=maxLevel */
            /* look for the first run of such a sequence */
            ਜਦੋਂ(ਪਹਿਲਾ_ਲੜੀਵਾਰ<ਲੜੀਵਾਰ_ਗਿਣਤੀ && ਪੱਧਰ[runs[ਪਹਿਲਾ_ਲੜੀਵਾਰ].logical_ਸ਼ੁਰੂ]<maxLevel) {
                ++ਪਹਿਲਾ_ਲੜੀਵਾਰ;
            }
            ਜੇ(ਪਹਿਲਾ_ਲੜੀਵਾਰ>=ਲੜੀਵਾਰ_ਗਿਣਤੀ) {
                ਰੋਕੋ;  /* no more such runs */
            }

            /* look for the limit run of such a sequence (the run behind it) */
            ਲਈ(ਹੱਦਬੰਦੀ_ਲੜੀਵਾਰ=ਪਹਿਲਾ_ਲੜੀਵਾਰ; ++ਹੱਦਬੰਦੀ_ਲੜੀਵਾਰ<ਲੜੀਵਾਰ_ਗਿਣਤੀ && ਪੱਧਰ[runs[ਹੱਦਬੰਦੀ_ਲੜੀਵਾਰ].logical_ਸ਼ੁਰੂ]>=maxLevel;) {}

            /* Swap the entire sequence of runs from firstRun to limitRun-1. */
            ਸਿਰਾ_ਲੜੀਵਾਰ=ਹੱਦਬੰਦੀ_ਲੜੀਵਾਰ-1;
            ਜਦੋਂ(ਪਹਿਲਾ_ਲੜੀਵਾਰ<ਸਿਰਾ_ਲੜੀਵਾਰ) {
                ਆਰਜ਼ੀ_ਲੜੀਵਾਰ = runs[ਪਹਿਲਾ_ਲੜੀਵਾਰ];
                runs[ਪਹਿਲਾ_ਲੜੀਵਾਰ]=runs[ਸਿਰਾ_ਲੜੀਵਾਰ];
                runs[ਸਿਰਾ_ਲੜੀਵਾਰ]=ਆਰਜ਼ੀ_ਲੜੀਵਾਰ;
                ++ਪਹਿਲਾ_ਲੜੀਵਾਰ;
                --ਸਿਰਾ_ਲੜੀਵਾਰ;
            }

            ਜੇ(ਹੱਦਬੰਦੀ_ਲੜੀਵਾਰ==ਲੜੀਵਾਰ_ਗਿਣਤੀ) {
                ਰੋਕੋ;  /* no more such runs */
            } ਨਹੀਂਤਾਂ {
                ਪਹਿਲਾ_ਲੜੀਵਾਰ=ਹੱਦਬੰਦੀ_ਲੜੀਵਾਰ+1;
            }
        }
    }

    /* now do maxLevel==old minLevel (==odd!), see above */
    ਜੇ(!(minLevel&1)) {
        ਪਹਿਲਾ_ਲੜੀਵਾਰ=0;

        /* include the trailing WS run in this complete reordering */
        ਜੇ(pBiDi->trailingWSStart==pBiDi->length) {
            --ਲੜੀਵਾਰ_ਗਿਣਤੀ;
        }

        /* Swap the entire sequence of all runs. (endRun==runCount) */
        ਜਦੋਂ(ਪਹਿਲਾ_ਲੜੀਵਾਰ<ਲੜੀਵਾਰ_ਗਿਣਤੀ) {
            ਆਰਜ਼ੀ_ਲੜੀਵਾਰ=runs[ਪਹਿਲਾ_ਲੜੀਵਾਰ];
            runs[ਪਹਿਲਾ_ਲੜੀਵਾਰ]=runs[ਲੜੀਵਾਰ_ਗਿਣਤੀ];
            runs[ਲੜੀਵਾਰ_ਗਿਣਤੀ]=ਆਰਜ਼ੀ_ਲੜੀਵਾਰ;
            ++ਪਹਿਲਾ_ਲੜੀਵਾਰ;
            --ਲੜੀਵਾਰ_ਗਿਣਤੀ;
        }
    }
}

/* compute the runs array --------------------------------------------------- */

ਸਥਿਰ int32_t get_ਲੜੀਵਾਰ_From_Logical_ਸੂਚਕਾਂਕ(UBiDi *pBiDi, int32_t logical_ਸੂਚਕਾਂਕ) {
    ਲੜੀਵਾਰ *runs=pBiDi->runs;
    int32_t ਲੜੀਵਾਰ_ਗਿਣਤੀ=pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ, visual_ਸ਼ੁਰੂ=0, i, length, logical_ਸ਼ੁਰੂ;

    ਲਈ(i=0; i<ਲੜੀਵਾਰ_ਗਿਣਤੀ; i++) {
        length=runs[i].visual_ਹੱਦਬੰਦੀ-visual_ਸ਼ੁਰੂ;
        logical_ਸ਼ੁਰੂ=GET_ਸੂਚਕਾਂਕ(runs[i].logical_ਸ਼ੁਰੂ);
        ਜੇ((logical_ਸੂਚਕਾਂਕ>=logical_ਸ਼ੁਰੂ) && (logical_ਸੂਚਕਾਂਕ<(logical_ਸ਼ੁਰੂ+length))) {
            ਵਾਪਸ i;
        }
        visual_ਸ਼ੁਰੂ+=length;
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
ubidi_getRuns(UBiDi *pBiDi, UError_ਕੋਡ* p_Error_ਕੋਡ) {
    /*
     * This method returns immediately if the runs are already set. This
     * includes the case of length==0 (handled in setPara)..
     */
    ਜੇ (pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ>=0) {
        ਵਾਪਸ ਸੱਚ;
    }

    ਜੇ(pBiDi->direction!=UBIDI_MIXED) {
        /* simple, single-run case - this covers length==0 */
        /* pBiDi->paraLevel is ok even for contextual multiple paragraphs */
        get_Single_ਲੜੀਵਾਰ(pBiDi, pBiDi->paraLevel);
    } ਨਹੀਂਤਾਂ /* UBIDI_MIXED, length>0 */ {
        /* mixed directionality */
        int32_t length=pBiDi->length, ਹੱਦਬੰਦੀ;
        UBiDiLevel *ਪੱਧਰ=pBiDi->ਪੱਧਰ;
        int32_t i, ਲੜੀਵਾਰ_ਗਿਣਤੀ;
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
        ਹੱਦਬੰਦੀ=pBiDi->trailingWSStart;
        /* count the runs, there is at least one non-WS run, and limit>0 */
        ਲੜੀਵਾਰ_ਗਿਣਤੀ=0;
        ਲਈ(i=0; i<ਹੱਦਬੰਦੀ; ++i) {
            /* increment runCount at the start of each run */
            ਜੇ(ਪੱਧਰ[i]!=level) {
                ++ਲੜੀਵਾਰ_ਗਿਣਤੀ;
                level=ਪੱਧਰ[i];
            }
        }

        /*
         * We don't need to see if the last run can be merged with a trailing
         * WS run because setTrailingWSStart() would have done that.
         */
        ਜੇ(ਲੜੀਵਾਰ_ਗਿਣਤੀ==1 && ਹੱਦਬੰਦੀ==length) {
            /* There is only one non-WS run and no trailing WS-run. */
            get_Single_ਲੜੀਵਾਰ(pBiDi, ਪੱਧਰ[0]);
        } ਨਹੀਂਤਾਂ /* runCount>1 || limit<length */ {
            /* allocate and set the runs */
            ਲੜੀਵਾਰ *runs;
            int32_t ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ, ਸ਼ੁਰੂ;
            UBiDiLevel minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1, maxLevel=0;

            /* now, count a (non-mergeable) WS run */
            ਜੇ(ਹੱਦਬੰਦੀ<length) {
                ++ਲੜੀਵਾਰ_ਗਿਣਤੀ;
            }

            /* runCount>1 */
            ਜੇ(get_Runs_ਸਮ੍ਰਿਤੀ(pBiDi, ਲੜੀਵਾਰ_ਗਿਣਤੀ)) {
                runs=pBiDi->runs_ਸਮ੍ਰਿਤੀ;
            } ਨਹੀਂਤਾਂ {
                *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ ਝੂਠ;
            }

            /* set the runs */
            /* FOOD FOR THOUGHT: this could be optimized, e.g.:
             * 464->444, 484->444, 575->555, 595->555
             * However, that would take longer. Check also how it would
             * interact with BiDi control removal and inserting Marks.
             */
            ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ=0;

            /* search for the run limits and initialize visualLimit values with the run lengths */
            i=0;
            ਕਰੋ {
                /* prepare this run */
                ਸ਼ੁਰੂ=i;
                level=ਪੱਧਰ[i];
                ਜੇ(level<minLevel) {
                    minLevel=level;
                }
                ਜੇ(level>maxLevel) {
                    maxLevel=level;
                }

                /* look for the run limit */
                ਜਦੋਂ(++i<ਹੱਦਬੰਦੀ && ਪੱਧਰ[i]==level) {}

                /* i is another run limit */
                runs[ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ].logical_ਸ਼ੁਰੂ=ਸ਼ੁਰੂ;
                runs[ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ].visual_ਹੱਦਬੰਦੀ=i-ਸ਼ੁਰੂ;
                runs[ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ].ਪਾਓ_Remove=0;
                ++ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ;
            } ਜਦੋਂ(i<ਹੱਦਬੰਦੀ);

            ਜੇ(ਹੱਦਬੰਦੀ<length) {
                /* there is a separate WS run */
                runs[ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ].logical_ਸ਼ੁਰੂ=ਹੱਦਬੰਦੀ;
                runs[ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ].visual_ਹੱਦਬੰਦੀ=length-ਹੱਦਬੰਦੀ;
                /* For the trailing WS run, pBiDi->paraLevel is ok even
                   if contextual multiple paragraphs.                   */
                ਜੇ(pBiDi->paraLevel<minLevel) {
                    minLevel=pBiDi->paraLevel;
                }
            }

            /* set the object fields */
            pBiDi->runs=runs;
            pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ=ਲੜੀਵਾਰ_ਗਿਣਤੀ;

            reorder_ਸਤਰ(pBiDi, minLevel, maxLevel);

            /* now add the direction flags and adjust the visualLimit's to be just that */
            /* this loop will also handle the trailing WS run */
            ਹੱਦਬੰਦੀ=0;
            ਲਈ(i=0; i<ਲੜੀਵਾਰ_ਗਿਣਤੀ; ++i) {
                ਜੋੜੋ_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ_FROM_LEVEL(runs[i].logical_ਸ਼ੁਰੂ, ਪੱਧਰ[runs[i].logical_ਸ਼ੁਰੂ]);
                ਹੱਦਬੰਦੀ+=runs[i].visual_ਹੱਦਬੰਦੀ;
                runs[i].visual_ਹੱਦਬੰਦੀ=ਹੱਦਬੰਦੀ;
            }

            /* Set the "odd" bit for the trailing WS run. */
            /* For a RTL paragraph, it will be the *first* run in visual order. */
            /* For the trailing WS run, pBiDi->paraLevel is ok even if
               contextual multiple paragraphs.                          */
            ਜੇ(ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ<ਲੜੀਵਾਰ_ਗਿਣਤੀ) {
                int32_t trailing_ਲੜੀਵਾਰ = ((pBiDi->paraLevel & 1) != 0)? 0 : ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ;

                ਜੋੜੋ_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ_FROM_LEVEL(runs[trailing_ਲੜੀਵਾਰ].logical_ਸ਼ੁਰੂ, pBiDi->paraLevel);
            }
        }
    }

    /* handle insert LRM/RLM BEFORE/AFTER run */
    ਜੇ(pBiDi->ਪਾਓ_Points.size>0) {
        ਬਿੰਦੂ *ਬਿੰਦੂ_2, *ਸ਼ੁਰੂ=pBiDi->ਪਾਓ_Points.points,
                      *ਹੱਦਬੰਦੀ=ਸ਼ੁਰੂ+pBiDi->ਪਾਓ_Points.size;
        int32_t ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ;
        ਲਈ(ਬਿੰਦੂ_2=ਸ਼ੁਰੂ; ਬਿੰਦੂ_2<ਹੱਦਬੰਦੀ; ਬਿੰਦੂ_2++) {
            ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ=get_ਲੜੀਵਾਰ_From_Logical_ਸੂਚਕਾਂਕ(pBiDi, ਬਿੰਦੂ_2->pos);
            pBiDi->runs[ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ].ਪਾਓ_Remove|=ਬਿੰਦੂ_2->flag;
        }
    }

    /* handle remove BiDi control characters */
    ਜੇ(pBiDi->control_ਗਿਣਤੀ>0) {
        int32_t ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ;
        ਅਚਲ char16_t *ਸ਼ੁਰੂ=pBiDi->ਪਾਠ, *ਹੱਦਬੰਦੀ=ਸ਼ੁਰੂ+pBiDi->length, *pu;
        ਲਈ(pu=ਸ਼ੁਰੂ; pu<ਹੱਦਬੰਦੀ; pu++) {
            ਜੇ(IS_BIDI_CONTROL_CHAR(*pu)) {
                ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ=get_ਲੜੀਵਾਰ_From_Logical_ਸੂਚਕਾਂਕ(pBiDi, (int32_t)(pu-ਸ਼ੁਰੂ));
                pBiDi->runs[ਲੜੀਵਾਰ_ਸੂਚਕਾਂਕ].ਪਾਓ_Remove--;
            }
        }
    }

    ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ UBool
prepareReorder(ਅਚਲ UBiDiLevel *ਪੱਧਰ, int32_t length,
               int32_t *ਸੂਚਕਾਂਕ_Map,
               UBiDiLevel *pMinLevel, UBiDiLevel *pMaxLevel) {
    int32_t ਸ਼ੁਰੂ;
    UBiDiLevel level, minLevel, maxLevel;

    ਜੇ(ਪੱਧਰ==ਖਾਲੀ_ਸੂਚਕ || length<=0) {
        ਵਾਪਸ ਝੂਠ;
    }

    /* determine minLevel and maxLevel */
    minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1;
    maxLevel=0;
    ਲਈ(ਸ਼ੁਰੂ=length; ਸ਼ੁਰੂ>0;) {
        level=ਪੱਧਰ[--ਸ਼ੁਰੂ];
        ਜੇ(level>UBIDI_MAX_EXPLICIT_LEVEL+1) {
            ਵਾਪਸ ਝੂਠ;
        }
        ਜੇ(level<minLevel) {
            minLevel=level;
        }
        ਜੇ(level>maxLevel) {
            maxLevel=level;
        }
    }
    *pMinLevel=minLevel;
    *pMaxLevel=maxLevel;

    /* initialize the index map */
    ਲਈ(ਸ਼ੁਰੂ=length; ਸ਼ੁਰੂ>0;) {
        --ਸ਼ੁਰੂ;
        ਸੂਚਕਾਂਕ_Map[ਸ਼ੁਰੂ]=ਸ਼ੁਰੂ;
    }

    ਵਾਪਸ ਸੱਚ;
}

/* reorder a line based on a levels array (L2) ------------------------------ */

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_reorderLogical(ਅਚਲ UBiDiLevel *ਪੱਧਰ, int32_t length, int32_t *ਸੂਚਕਾਂਕ_Map) {
    int32_t ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, ਜੋੜ_ਦਾ_Sos_Eos;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    ਜੇ(ਸੂਚਕਾਂਕ_Map==ਖਾਲੀ_ਸੂਚਕ || !prepareReorder(ਪੱਧਰ, length, ਸੂਚਕਾਂਕ_Map, &minLevel, &maxLevel)) {
        ਵਾਪਸ;
    }

    /* nothing to do? */
    ਜੇ(minLevel==maxLevel && (minLevel&1)==0) {
        ਵਾਪਸ;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    ਕਰੋ {
        ਸ਼ੁਰੂ=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        ਲਈ(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            ਜਦੋਂ(ਸ਼ੁਰੂ<length && ਪੱਧਰ[ਸ਼ੁਰੂ]<maxLevel) {
                ++ਸ਼ੁਰੂ;
            }
            ਜੇ(ਸ਼ੁਰੂ>=length) {
                ਰੋਕੋ;  /* no more such sequences */
            }

            /* look for the limit of such a sequence (the index behind it) */
            ਲਈ(ਹੱਦਬੰਦੀ=ਸ਼ੁਰੂ; ++ਹੱਦਬੰਦੀ<length && ਪੱਧਰ[ਹੱਦਬੰਦੀ]>=maxLevel;) {}

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
            ਜੋੜ_ਦਾ_Sos_Eos=ਸ਼ੁਰੂ+ਹੱਦਬੰਦੀ-1;

            /* reorder each index in the sequence */
            ਕਰੋ {
                ਸੂਚਕਾਂਕ_Map[ਸ਼ੁਰੂ]=ਜੋੜ_ਦਾ_Sos_Eos-ਸੂਚਕਾਂਕ_Map[ਸ਼ੁਰੂ];
            } ਜਦੋਂ(++ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ);

            /* start==limit */
            ਜੇ(ਹੱਦਬੰਦੀ==length) {
                ਰੋਕੋ;  /* no more such sequences */
            } ਨਹੀਂਤਾਂ {
                ਸ਼ੁਰੂ=ਹੱਦਬੰਦੀ+1;
            }
        }
    } ਜਦੋਂ(--maxLevel>=minLevel);
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_reorderVisual(ਅਚਲ UBiDiLevel *ਪੱਧਰ, int32_t length, int32_t *ਸੂਚਕਾਂਕ_Map) {
    int32_t ਸ਼ੁਰੂ, ਸਿਰਾ, ਹੱਦਬੰਦੀ, ਆਰਜ਼ੀ;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    ਜੇ(ਸੂਚਕਾਂਕ_Map==ਖਾਲੀ_ਸੂਚਕ || !prepareReorder(ਪੱਧਰ, length, ਸੂਚਕਾਂਕ_Map, &minLevel, &maxLevel)) {
        ਵਾਪਸ;
    }

    /* nothing to do? */
    ਜੇ(minLevel==maxLevel && (minLevel&1)==0) {
        ਵਾਪਸ;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    ਕਰੋ {
        ਸ਼ੁਰੂ=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        ਲਈ(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            ਜਦੋਂ(ਸ਼ੁਰੂ<length && ਪੱਧਰ[ਸ਼ੁਰੂ]<maxLevel) {
                ++ਸ਼ੁਰੂ;
            }
            ਜੇ(ਸ਼ੁਰੂ>=length) {
                ਰੋਕੋ;  /* no more such runs */
            }

            /* look for the limit of such a sequence (the index behind it) */
            ਲਈ(ਹੱਦਬੰਦੀ=ਸ਼ੁਰੂ; ++ਹੱਦਬੰਦੀ<length && ਪੱਧਰ[ਹੱਦਬੰਦੀ]>=maxLevel;) {}

            /*
             * Swap the entire interval of indexes from start to limit-1.
             * We don't need to swap the levels for the purpose of this
             * algorithm: the sequence of levels that we look at does not
             * move anyway.
             */
            ਸਿਰਾ=ਹੱਦਬੰਦੀ-1;
            ਜਦੋਂ(ਸ਼ੁਰੂ<ਸਿਰਾ) {
                ਆਰਜ਼ੀ=ਸੂਚਕਾਂਕ_Map[ਸ਼ੁਰੂ];
                ਸੂਚਕਾਂਕ_Map[ਸ਼ੁਰੂ]=ਸੂਚਕਾਂਕ_Map[ਸਿਰਾ];
                ਸੂਚਕਾਂਕ_Map[ਸਿਰਾ]=ਆਰਜ਼ੀ;

                ++ਸ਼ੁਰੂ;
                --ਸਿਰਾ;
            }

            ਜੇ(ਹੱਦਬੰਦੀ==length) {
                ਰੋਕੋ;  /* no more such sequences */
            } ਨਹੀਂਤਾਂ {
                ਸ਼ੁਰੂ=ਹੱਦਬੰਦੀ+1;
            }
        }
    } ਜਦੋਂ(--maxLevel>=minLevel);
}

/* API functions for logical<->visual mapping ------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_get_Visual_ਸੂਚਕਾਂਕ(UBiDi *pBiDi, int32_t logical_ਸੂਚਕਾਂਕ, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    int32_t visual_ਸੂਚਕਾਂਕ=UBIDI_MAP_NOWHERE;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ਕੋਡ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_ਸਤਰ(pBiDi, *p_Error_ਕੋਡ, -1);
    RETURN_IF_BAD_RANGE(logical_ਸੂਚਕਾਂਕ, 0, pBiDi->length, *p_Error_ਕੋਡ, -1);

    /* we can do the trivial cases without the runs array */
    ਚੋਣ(pBiDi->direction) {
    ਹਾਲਤ UBIDI_LTR:
        visual_ਸੂਚਕਾਂਕ=logical_ਸੂਚਕਾਂਕ;
        ਰੋਕੋ;
    ਹਾਲਤ UBIDI_RTL:
        visual_ਸੂਚਕਾਂਕ=pBiDi->length-logical_ਸੂਚਕਾਂਕ-1;
        ਰੋਕੋ;
    ਮੂਲ:
        ਜੇ(!ubidi_getRuns(pBiDi, p_Error_ਕੋਡ)) {
            *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ -1;
        } ਨਹੀਂਤਾਂ {
            ਲੜੀਵਾਰ *runs=pBiDi->runs;
            int32_t i, visual_ਸ਼ੁਰੂ=0, ਵਿੱਥ, length;

            /* linear search for the run, search on the visual runs */
            ਲਈ(i=0; i<pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ; ++i) {
                length=runs[i].visual_ਹੱਦਬੰਦੀ-visual_ਸ਼ੁਰੂ;
                ਵਿੱਥ=logical_ਸੂਚਕਾਂਕ-GET_ਸੂਚਕਾਂਕ(runs[i].logical_ਸ਼ੁਰੂ);
                ਜੇ(ਵਿੱਥ>=0 && ਵਿੱਥ<length) {
                    ਜੇ(IS_EVEN_ਲੜੀਵਾਰ(runs[i].logical_ਸ਼ੁਰੂ)) {
                        /* LTR */
                        visual_ਸੂਚਕਾਂਕ=visual_ਸ਼ੁਰੂ+ਵਿੱਥ;
                    } ਨਹੀਂਤਾਂ {
                        /* RTL */
                        visual_ਸੂਚਕਾਂਕ=visual_ਸ਼ੁਰੂ+length-ਵਿੱਥ-1;
                    }
                    ਰੋਕੋ;          /* exit for loop */
                }
                visual_ਸ਼ੁਰੂ+=length;
            }
            ਜੇ(i>=pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ) {
                ਵਾਪਸ UBIDI_MAP_NOWHERE;
            }
        }
    }

    ਜੇ(pBiDi->ਪਾਓ_Points.size>0) {
        /* add the number of added marks until the calculated visual index */
        ਲੜੀਵਾਰ *runs=pBiDi->runs;
        int32_t i, length, ਪਾਓ_Remove;
        int32_t visual_ਸ਼ੁਰੂ=0, mark_ਮਿਲਿਆ=0;
        ਲਈ(i=0; ; i++, visual_ਸ਼ੁਰੂ+=length) {
            length=runs[i].visual_ਹੱਦਬੰਦੀ-visual_ਸ਼ੁਰੂ;
            ਪਾਓ_Remove=runs[i].ਪਾਓ_Remove;
            ਜੇ(ਪਾਓ_Remove & (LRM_BEFORE|RLM_BEFORE)) {
                mark_ਮਿਲਿਆ++;
            }
            /* is it the run containing the visual index? */
            ਜੇ(visual_ਸੂਚਕਾਂਕ<runs[i].visual_ਹੱਦਬੰਦੀ) {
                ਵਾਪਸ visual_ਸੂਚਕਾਂਕ+mark_ਮਿਲਿਆ;
            }
            ਜੇ(ਪਾਓ_Remove & (LRM_AFTER|RLM_AFTER)) {
                mark_ਮਿਲਿਆ++;
            }
        }
    }
    ਨਹੀਂਤਾਂ ਜੇ(pBiDi->control_ਗਿਣਤੀ>0) {
        /* subtract the number of controls until the calculated visual index */
        ਲੜੀਵਾਰ *runs=pBiDi->runs;
        int32_t i, j, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, length, ਪਾਓ_Remove;
        int32_t visual_ਸ਼ੁਰੂ=0, control_ਮਿਲਿਆ=0;
        char16_t uchar=pBiDi->ਪਾਠ[logical_ਸੂਚਕਾਂਕ];
        /* is the logical index pointing to a control ? */
        ਜੇ(IS_BIDI_CONTROL_CHAR(uchar)) {
            ਵਾਪਸ UBIDI_MAP_NOWHERE;
        }
        /* loop on runs */
        ਲਈ(i=0; ; i++, visual_ਸ਼ੁਰੂ+=length) {
            length=runs[i].visual_ਹੱਦਬੰਦੀ-visual_ਸ਼ੁਰੂ;
            ਪਾਓ_Remove=runs[i].ਪਾਓ_Remove;
            /* calculated visual index is beyond this run? */
            ਜੇ(visual_ਸੂਚਕਾਂਕ>=runs[i].visual_ਹੱਦਬੰਦੀ) {
                control_ਮਿਲਿਆ-=ਪਾਓ_Remove;
                ਜਾਰੀ;
            }
            /* calculated visual index must be within current run */
            ਜੇ(ਪਾਓ_Remove==0) {
                ਵਾਪਸ visual_ਸੂਚਕਾਂਕ-control_ਮਿਲਿਆ;
            }
            ਜੇ(IS_EVEN_ਲੜੀਵਾਰ(runs[i].logical_ਸ਼ੁਰੂ)) {
                /* LTR: check from run start to logical index */
                ਸ਼ੁਰੂ=runs[i].logical_ਸ਼ੁਰੂ;
                ਹੱਦਬੰਦੀ=logical_ਸੂਚਕਾਂਕ;
            } ਨਹੀਂਤਾਂ {
                /* RTL: check from logical index to run end */
                ਸ਼ੁਰੂ=logical_ਸੂਚਕਾਂਕ+1;
                ਹੱਦਬੰਦੀ=GET_ਸੂਚਕਾਂਕ(runs[i].logical_ਸ਼ੁਰੂ)+length;
            }
            ਲਈ(j=ਸ਼ੁਰੂ; j<ਹੱਦਬੰਦੀ; j++) {
                uchar=pBiDi->ਪਾਠ[j];
                ਜੇ(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_ਮਿਲਿਆ++;
                }
            }
            ਵਾਪਸ visual_ਸੂਚਕਾਂਕ-control_ਮਿਲਿਆ;
        }
    }

    ਵਾਪਸ visual_ਸੂਚਕਾਂਕ;
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Logical_ਸੂਚਕਾਂਕ(UBiDi *pBiDi, int32_t visual_ਸੂਚਕਾਂਕ, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਲੜੀਵਾਰ *runs;
    int32_t i, ਲੜੀਵਾਰ_ਗਿਣਤੀ, ਸ਼ੁਰੂ;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ਕੋਡ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_ਸਤਰ(pBiDi, *p_Error_ਕੋਡ, -1);
    RETURN_IF_BAD_RANGE(visual_ਸੂਚਕਾਂਕ, 0, pBiDi->ਨਤੀਜਾ_ਲੰਮਾਈ, *p_Error_ਕੋਡ, -1);
    /* we can do the trivial cases without the runs array */
    ਜੇ(pBiDi->ਪਾਓ_Points.size==0 && pBiDi->control_ਗਿਣਤੀ==0) {
        ਜੇ(pBiDi->direction==UBIDI_LTR) {
            ਵਾਪਸ visual_ਸੂਚਕਾਂਕ;
        }
        ਨਹੀਂਤਾਂ ਜੇ(pBiDi->direction==UBIDI_RTL) {
            ਵਾਪਸ pBiDi->length-visual_ਸੂਚਕਾਂਕ-1;
        }
    }
    ਜੇ(!ubidi_getRuns(pBiDi, p_Error_ਕੋਡ)) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ -1;
    }

    runs=pBiDi->runs;
    ਲੜੀਵਾਰ_ਗਿਣਤੀ=pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ;
    ਜੇ(pBiDi->ਪਾਓ_Points.size>0) {
        /* handle inserted LRM/RLM */
        int32_t mark_ਮਿਲਿਆ=0, ਪਾਓ_Remove;
        int32_t visual_ਸ਼ੁਰੂ=0, length;
        runs=pBiDi->runs;
        /* subtract number of marks until visual index */
        ਲਈ(i=0; ; i++, visual_ਸ਼ੁਰੂ+=length) {
            length=runs[i].visual_ਹੱਦਬੰਦੀ-visual_ਸ਼ੁਰੂ;
            ਪਾਓ_Remove=runs[i].ਪਾਓ_Remove;
            ਜੇ(ਪਾਓ_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                ਜੇ(visual_ਸੂਚਕਾਂਕ<=(visual_ਸ਼ੁਰੂ+mark_ਮਿਲਿਆ)) {
                    ਵਾਪਸ UBIDI_MAP_NOWHERE;
                }
                mark_ਮਿਲਿਆ++;
            }
            /* is adjusted visual index within this run? */
            ਜੇ(visual_ਸੂਚਕਾਂਕ<(runs[i].visual_ਹੱਦਬੰਦੀ+mark_ਮਿਲਿਆ)) {
                visual_ਸੂਚਕਾਂਕ-=mark_ਮਿਲਿਆ;
                ਰੋਕੋ;
            }
            ਜੇ(ਪਾਓ_Remove&(LRM_AFTER|RLM_AFTER)) {
                ਜੇ(visual_ਸੂਚਕਾਂਕ==(visual_ਸ਼ੁਰੂ+length+mark_ਮਿਲਿਆ)) {
                    ਵਾਪਸ UBIDI_MAP_NOWHERE;
                }
                mark_ਮਿਲਿਆ++;
            }
        }
    }
    ਨਹੀਂਤਾਂ ਜੇ(pBiDi->control_ਗਿਣਤੀ>0) {
        /* handle removed BiDi control characters */
        int32_t control_ਮਿਲਿਆ=0, ਪਾਓ_Remove, length;
        int32_t logical_ਸ਼ੁਰੂ, logical_ਸਿਰਾ, visual_ਸ਼ੁਰੂ=0, j, k;
        char16_t uchar;
        UBool even_ਲੜੀਵਾਰ;
        /* add number of controls until visual index */
        ਲਈ(i=0; ; i++, visual_ਸ਼ੁਰੂ+=length) {
            length=runs[i].visual_ਹੱਦਬੰਦੀ-visual_ਸ਼ੁਰੂ;
            ਪਾਓ_Remove=runs[i].ਪਾਓ_Remove;
            /* is adjusted visual index beyond current run? */
            ਜੇ(visual_ਸੂਚਕਾਂਕ>=(runs[i].visual_ਹੱਦਬੰਦੀ-control_ਮਿਲਿਆ+ਪਾਓ_Remove)) {
                control_ਮਿਲਿਆ-=ਪਾਓ_Remove;
                ਜਾਰੀ;
            }
            /* adjusted visual index is within current run */
            ਜੇ(ਪਾਓ_Remove==0) {
                visual_ਸੂਚਕਾਂਕ+=control_ਮਿਲਿਆ;
                ਰੋਕੋ;
            }
            /* count non-control chars until visualIndex */
            logical_ਸ਼ੁਰੂ=runs[i].logical_ਸ਼ੁਰੂ;
            even_ਲੜੀਵਾਰ=IS_EVEN_ਲੜੀਵਾਰ(logical_ਸ਼ੁਰੂ);
            REMOVE_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ(logical_ਸ਼ੁਰੂ);
            logical_ਸਿਰਾ=logical_ਸ਼ੁਰੂ+length-1;
            ਲਈ(j=0; j<length; j++) {
                k= even_ਲੜੀਵਾਰ ? logical_ਸ਼ੁਰੂ+j : logical_ਸਿਰਾ-j;
                uchar=pBiDi->ਪਾਠ[k];
                ਜੇ(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_ਮਿਲਿਆ++;
                }
                ਜੇ((visual_ਸੂਚਕਾਂਕ+control_ਮਿਲਿਆ)==(visual_ਸ਼ੁਰੂ+j)) {
                    ਰੋਕੋ;
                }
            }
            visual_ਸੂਚਕਾਂਕ+=control_ਮਿਲਿਆ;
            ਰੋਕੋ;
        }
    }
    /* handle all cases */
    ਜੇ(ਲੜੀਵਾਰ_ਗਿਣਤੀ<=10) {
        /* linear search for the run */
        ਲਈ(i=0; visual_ਸੂਚਕਾਂਕ>=runs[i].visual_ਹੱਦਬੰਦੀ; ++i) {}
    } ਨਹੀਂਤਾਂ {
        /* binary search for the run */
        int32_t begin=0, ਹੱਦਬੰਦੀ=ਲੜੀਵਾਰ_ਗਿਣਤੀ;

        /* the middle if() is guaranteed to find the run, we don't need a loop limit */
        ਲਈ(;;) {
            i=(begin+ਹੱਦਬੰਦੀ)/2;
            ਜੇ(visual_ਸੂਚਕਾਂਕ>=runs[i].visual_ਹੱਦਬੰਦੀ) {
                begin=i+1;
            } ਨਹੀਂਤਾਂ ਜੇ(i==0 || visual_ਸੂਚਕਾਂਕ>=runs[i-1].visual_ਹੱਦਬੰਦੀ) {
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ {
                ਹੱਦਬੰਦੀ=i;
            }
        }
    }

    ਸ਼ੁਰੂ=runs[i].logical_ਸ਼ੁਰੂ;
    ਜੇ(IS_EVEN_ਲੜੀਵਾਰ(ਸ਼ੁਰੂ)) {
        /* LTR */
        /* the offset in runs[i] is visualIndex-runs[i-1].visualLimit */
        ਜੇ(i>0) {
            visual_ਸੂਚਕਾਂਕ-=runs[i-1].visual_ਹੱਦਬੰਦੀ;
        }
        ਵਾਪਸ ਸ਼ੁਰੂ+visual_ਸੂਚਕਾਂਕ;
    } ਨਹੀਂਤਾਂ {
        /* RTL */
        ਵਾਪਸ GET_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ)+runs[i].visual_ਹੱਦਬੰਦੀ-visual_ਸੂਚਕਾਂਕ-1;
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_getLogicalMap(UBiDi *pBiDi, int32_t *ਸੂਚਕਾਂਕ_Map, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ਕੋਡ);
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_ਗਿਣਤੀ_Runs(pBiDi, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        /* no op */
    } ਨਹੀਂਤਾਂ ਜੇ(ਸੂਚਕਾਂਕ_Map==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    } ਨਹੀਂਤਾਂ {
        /* fill a logical-to-visual index map using the runs[] */
        int32_t visual_ਸ਼ੁਰੂ, visual_ਹੱਦਬੰਦੀ, i, j, k;
        int32_t logical_ਸ਼ੁਰੂ, logical_ਹੱਦਬੰਦੀ;
        ਲੜੀਵਾਰ *runs=pBiDi->runs;
        ਜੇ (pBiDi->length<=0) {
            ਵਾਪਸ;
        }
        ਜੇ (pBiDi->length>pBiDi->ਨਤੀਜਾ_ਲੰਮਾਈ) {
            uprv_memset(ਸੂਚਕਾਂਕ_Map, 0xFF, pBiDi->length*ਆਕਾਰ(int32_t));
        }

        visual_ਸ਼ੁਰੂ=0;
        ਲਈ(j=0; j<pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ; ++j) {
            logical_ਸ਼ੁਰੂ=GET_ਸੂਚਕਾਂਕ(runs[j].logical_ਸ਼ੁਰੂ);
            visual_ਹੱਦਬੰਦੀ=runs[j].visual_ਹੱਦਬੰਦੀ;
            ਜੇ(IS_EVEN_ਲੜੀਵਾਰ(runs[j].logical_ਸ਼ੁਰੂ)) {
                ਕਰੋ { /* LTR */
                    ਸੂਚਕਾਂਕ_Map[logical_ਸ਼ੁਰੂ++]=visual_ਸ਼ੁਰੂ++;
                } ਜਦੋਂ(visual_ਸ਼ੁਰੂ<visual_ਹੱਦਬੰਦੀ);
            } ਨਹੀਂਤਾਂ {
                logical_ਸ਼ੁਰੂ+=visual_ਹੱਦਬੰਦੀ-visual_ਸ਼ੁਰੂ;  /* logicalLimit */
                ਕਰੋ { /* RTL */
                    ਸੂਚਕਾਂਕ_Map[--logical_ਸ਼ੁਰੂ]=visual_ਸ਼ੁਰੂ++;
                } ਜਦੋਂ(visual_ਸ਼ੁਰੂ<visual_ਹੱਦਬੰਦੀ);
            }
            /* visualStart==visualLimit; */
        }

        ਜੇ(pBiDi->ਪਾਓ_Points.size>0) {
            int32_t mark_ਮਿਲਿਆ=0, ਲੜੀਵਾਰ_ਗਿਣਤੀ=pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ;
            int32_t length, ਪਾਓ_Remove;
            visual_ਸ਼ੁਰੂ=0;
            /* add number of marks found until each index */
            ਲਈ(i=0; i<ਲੜੀਵਾਰ_ਗਿਣਤੀ; i++, visual_ਸ਼ੁਰੂ+=length) {
                length=runs[i].visual_ਹੱਦਬੰਦੀ-visual_ਸ਼ੁਰੂ;
                ਪਾਓ_Remove=runs[i].ਪਾਓ_Remove;
                ਜੇ(ਪਾਓ_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_ਮਿਲਿਆ++;
                }
                ਜੇ(mark_ਮਿਲਿਆ>0) {
                    logical_ਸ਼ੁਰੂ=GET_ਸੂਚਕਾਂਕ(runs[i].logical_ਸ਼ੁਰੂ);
                    logical_ਹੱਦਬੰਦੀ=logical_ਸ਼ੁਰੂ+length;
                    ਲਈ(j=logical_ਸ਼ੁਰੂ; j<logical_ਹੱਦਬੰਦੀ; j++) {
                        ਸੂਚਕਾਂਕ_Map[j]+=mark_ਮਿਲਿਆ;
                    }
                }
                ਜੇ(ਪਾਓ_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_ਮਿਲਿਆ++;
                }
            }
        }
        ਨਹੀਂਤਾਂ ਜੇ(pBiDi->control_ਗਿਣਤੀ>0) {
            int32_t control_ਮਿਲਿਆ=0, ਲੜੀਵਾਰ_ਗਿਣਤੀ=pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ;
            int32_t length, ਪਾਓ_Remove;
            UBool even_ਲੜੀਵਾਰ;
            char16_t uchar;
            visual_ਸ਼ੁਰੂ=0;
            /* subtract number of controls found until each index */
            ਲਈ(i=0; i<ਲੜੀਵਾਰ_ਗਿਣਤੀ; i++, visual_ਸ਼ੁਰੂ+=length) {
                length=runs[i].visual_ਹੱਦਬੰਦੀ-visual_ਸ਼ੁਰੂ;
                ਪਾਓ_Remove=runs[i].ਪਾਓ_Remove;
                /* no control found within previous runs nor within this run */
                ਜੇ((control_ਮਿਲਿਆ-ਪਾਓ_Remove)==0) {
                    ਜਾਰੀ;
                }
                logical_ਸ਼ੁਰੂ=runs[i].logical_ਸ਼ੁਰੂ;
                even_ਲੜੀਵਾਰ=IS_EVEN_ਲੜੀਵਾਰ(logical_ਸ਼ੁਰੂ);
                REMOVE_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ(logical_ਸ਼ੁਰੂ);
                logical_ਹੱਦਬੰਦੀ=logical_ਸ਼ੁਰੂ+length;
                /* if no control within this run */
                ਜੇ(ਪਾਓ_Remove==0) {
                    ਲਈ(j=logical_ਸ਼ੁਰੂ; j<logical_ਹੱਦਬੰਦੀ; j++) {
                        ਸੂਚਕਾਂਕ_Map[j]-=control_ਮਿਲਿਆ;
                    }
                    ਜਾਰੀ;
                }
                ਲਈ(j=0; j<length; j++) {
                    k= even_ਲੜੀਵਾਰ ? logical_ਸ਼ੁਰੂ+j : logical_ਹੱਦਬੰਦੀ-j-1;
                    uchar=pBiDi->ਪਾਠ[k];
                    ਜੇ(IS_BIDI_CONTROL_CHAR(uchar)) {
                        control_ਮਿਲਿਆ++;
                        ਸੂਚਕਾਂਕ_Map[k]=UBIDI_MAP_NOWHERE;
                        ਜਾਰੀ;
                    }
                    ਸੂਚਕਾਂਕ_Map[k]-=control_ਮਿਲਿਆ;
                }
            }
        }
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_getVisualMap(UBiDi *pBiDi, int32_t *ਸੂਚਕਾਂਕ_Map, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ਕੋਡ);
    ਜੇ(ਸੂਚਕਾਂਕ_Map==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_ਗਿਣਤੀ_Runs(pBiDi, p_Error_ਕੋਡ);
    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ)) {
        /* fill a visual-to-logical index map using the runs[] */
        ਲੜੀਵਾਰ *runs=pBiDi->runs, *runs_ਹੱਦਬੰਦੀ=runs+pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ;
        int32_t logical_ਸ਼ੁਰੂ, visual_ਸ਼ੁਰੂ, visual_ਹੱਦਬੰਦੀ, *pi=ਸੂਚਕਾਂਕ_Map;

        ਜੇ (pBiDi->ਨਤੀਜਾ_ਲੰਮਾਈ<=0) {
            ਵਾਪਸ;
        }
        visual_ਸ਼ੁਰੂ=0;
        ਲਈ(; runs<runs_ਹੱਦਬੰਦੀ; ++runs) {
            logical_ਸ਼ੁਰੂ=runs->logical_ਸ਼ੁਰੂ;
            visual_ਹੱਦਬੰਦੀ=runs->visual_ਹੱਦਬੰਦੀ;
            ਜੇ(IS_EVEN_ਲੜੀਵਾਰ(logical_ਸ਼ੁਰੂ)) {
                ਕਰੋ { /* LTR */
                    *pi++ = logical_ਸ਼ੁਰੂ++;
                } ਜਦੋਂ(++visual_ਸ਼ੁਰੂ<visual_ਹੱਦਬੰਦੀ);
            } ਨਹੀਂਤਾਂ {
                REMOVE_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ(logical_ਸ਼ੁਰੂ);
                logical_ਸ਼ੁਰੂ+=visual_ਹੱਦਬੰਦੀ-visual_ਸ਼ੁਰੂ;  /* logicalLimit */
                ਕਰੋ { /* RTL */
                    *pi++ = --logical_ਸ਼ੁਰੂ;
                } ਜਦੋਂ(++visual_ਸ਼ੁਰੂ<visual_ਹੱਦਬੰਦੀ);
            }
            /* visualStart==visualLimit; */
        }

        ਜੇ(pBiDi->ਪਾਓ_Points.size>0) {
            int32_t mark_ਮਿਲਿਆ=0, ਲੜੀਵਾਰ_ਗਿਣਤੀ=pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ;
            int32_t ਪਾਓ_Remove, i, j, k;
            runs=pBiDi->runs;
            /* count all inserted marks */
            ਲਈ(i=0; i<ਲੜੀਵਾਰ_ਗਿਣਤੀ; i++) {
                ਪਾਓ_Remove=runs[i].ਪਾਓ_Remove;
                ਜੇ(ਪਾਓ_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_ਮਿਲਿਆ++;
                }
                ਜੇ(ਪਾਓ_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_ਮਿਲਿਆ++;
                }
            }
            /* move back indexes by number of preceding marks */
            k=pBiDi->ਨਤੀਜਾ_ਲੰਮਾਈ;
            ਲਈ(i=ਲੜੀਵਾਰ_ਗਿਣਤੀ-1; i>=0 && mark_ਮਿਲਿਆ>0; i--) {
                ਪਾਓ_Remove=runs[i].ਪਾਓ_Remove;
                ਜੇ(ਪਾਓ_Remove&(LRM_AFTER|RLM_AFTER)) {
                    ਸੂਚਕਾਂਕ_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_ਮਿਲਿਆ--;
                }
                visual_ਸ਼ੁਰੂ= i>0 ? runs[i-1].visual_ਹੱਦਬੰਦੀ : 0;
                ਲਈ(j=runs[i].visual_ਹੱਦਬੰਦੀ-1; j>=visual_ਸ਼ੁਰੂ && mark_ਮਿਲਿਆ>0; j--) {
                    ਸੂਚਕਾਂਕ_Map[--k]=ਸੂਚਕਾਂਕ_Map[j];
                }
                ਜੇ(ਪਾਓ_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    ਸੂਚਕਾਂਕ_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_ਮਿਲਿਆ--;
                }
            }
        }
        ਨਹੀਂਤਾਂ ਜੇ(pBiDi->control_ਗਿਣਤੀ>0) {
            int32_t ਲੜੀਵਾਰ_ਗਿਣਤੀ=pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ, logical_ਸਿਰਾ;
            int32_t ਪਾਓ_Remove, length, i, j, k, m;
            char16_t uchar;
            UBool even_ਲੜੀਵਾਰ;
            runs=pBiDi->runs;
            visual_ਸ਼ੁਰੂ=0;
            /* move forward indexes by number of preceding controls */
            k=0;
            ਲਈ(i=0; i<ਲੜੀਵਾਰ_ਗਿਣਤੀ; i++, visual_ਸ਼ੁਰੂ+=length) {
                length=runs[i].visual_ਹੱਦਬੰਦੀ-visual_ਸ਼ੁਰੂ;
                ਪਾਓ_Remove=runs[i].ਪਾਓ_Remove;
                /* if no control found yet, nothing to do in this run */
                ਜੇ((ਪਾਓ_Remove==0)&&(k==visual_ਸ਼ੁਰੂ)) {
                    k+=length;
                    ਜਾਰੀ;
                }
                /* if no control in this run */
                ਜੇ(ਪਾਓ_Remove==0) {
                    visual_ਹੱਦਬੰਦੀ=runs[i].visual_ਹੱਦਬੰਦੀ;
                    ਲਈ(j=visual_ਸ਼ੁਰੂ; j<visual_ਹੱਦਬੰਦੀ; j++) {
                        ਸੂਚਕਾਂਕ_Map[k++]=ਸੂਚਕਾਂਕ_Map[j];
                    }
                    ਜਾਰੀ;
                }
                logical_ਸ਼ੁਰੂ=runs[i].logical_ਸ਼ੁਰੂ;
                even_ਲੜੀਵਾਰ=IS_EVEN_ਲੜੀਵਾਰ(logical_ਸ਼ੁਰੂ);
                REMOVE_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ(logical_ਸ਼ੁਰੂ);
                logical_ਸਿਰਾ=logical_ਸ਼ੁਰੂ+length-1;
                ਲਈ(j=0; j<length; j++) {
                    m= even_ਲੜੀਵਾਰ ? logical_ਸ਼ੁਰੂ+j : logical_ਸਿਰਾ-j;
                    uchar=pBiDi->ਪਾਠ[m];
                    ਜੇ(!IS_BIDI_CONTROL_CHAR(uchar)) {
                        ਸੂਚਕਾਂਕ_Map[k++]=m;
                    }
                }
            }
        }
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_invertMap(ਅਚਲ int32_t *srcMap, int32_t *destMap, int32_t length) {
    ਜੇ(srcMap!=ਖਾਲੀ_ਸੂਚਕ && destMap!=ਖਾਲੀ_ਸੂਚਕ && length>0) {
        ਅਚਲ int32_t *pi;
        int32_t dest_ਲੰਮਾਈ=-1, count=0;
        /* find highest value and count positive indexes in srcMap */
        pi=srcMap+length;
        ਜਦੋਂ(pi>srcMap) {
            ਜੇ(*--pi>dest_ਲੰਮਾਈ) {
                dest_ਲੰਮਾਈ=*pi;
            }
            ਜੇ(*pi>=0) {
                count++;
            }
        }
        dest_ਲੰਮਾਈ++;           /* add 1 for origin 0 */
        ਜੇ(count<dest_ਲੰਮਾਈ) {
            /* we must fill unmatched destMap entries with -1 */
            uprv_memset(destMap, 0xFF, dest_ਲੰਮਾਈ*ਆਕਾਰ(int32_t));
        }
        pi=srcMap+length;
        ਜਦੋਂ(length>0) {
            ਜੇ(*--pi>=0) {
                destMap[*pi]=--length;
            } ਨਹੀਂਤਾਂ {
                --length;
            }
        }
    }
}
