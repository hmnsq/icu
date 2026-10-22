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

#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubidi.h"
#ಅನ್ತರ್ಭಾವ "ubidiimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

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
ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_Trailing_WSStart(UBiDi *pBiDi) {
    /* pBiDi->direction!=UBIDI_MIXED */

    ಸ್ಥಿರಾಂಕ DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *ಮಟ್ಟಗಳು=pBiDi->ಮಟ್ಟಗಳು;
    int32_t ಆರಂಭ=pBiDi->length;
    UBiDiLevel paraLevel=pBiDi->paraLevel;

    /* If the line is terminated by a block separator, all preceding WS etc...
       are already set to paragraph level.
       Setting trailingWSStart to pBidi->length will avoid changing the
       level of B chars from 0 to paraLevel in ubidi_getLevels when
       orderParagraphsLTR==true.
     */
    ಆದರೆ(dirProps[ಆರಂಭ-1]==B) {
        pBiDi->trailingWSStart=ಆರಂಭ;   /* currently == pBiDi->length */
        ಹಿಂತಿರುಗಿ;
    }
    /* go backwards across all WS, BN, explicit codes */
    ಆಗ(ಆರಂಭ>0 && DIRPROP_FLAG(dirProps[ಆರಂಭ-1])&ಮುಖವಾಡ_WS) {
        --ಆರಂಭ;
    }

    /* if the WS run can be merged with the previous run then do so here */
    ಆಗ(ಆರಂಭ>0 && ಮಟ್ಟಗಳು[ಆರಂಭ-1]==paraLevel) {
        --ಆರಂಭ;
    }

    pBiDi->trailingWSStart=ಆರಂಭ;
}

/* ubidi_setLine ------------------------------------------------------------ */

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_ಗಣ_ಸಾಲು(ಸ್ಥಿರಾಂಕ UBiDi *pParaBiDi,
              int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ,
              UBiDi *p_ಸಾಲು_Bi_Di,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    int32_t length;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ಸಂಕೇತ);
    RETURN_VOID_IF_NOT_VALID_PARA(pParaBiDi, *p_Error_ಸಂಕೇತ);
    RETURN_VOID_IF_BAD_RANGE(ಆರಂಭ, 0, ಎಲ್ಲೆ, *p_Error_ಸಂಕೇತ);
    RETURN_VOID_IF_BAD_RANGE(ಎಲ್ಲೆ, 0, pParaBiDi->length+1, *p_Error_ಸಂಕೇತ);
    ಆದರೆ(p_ಸಾಲು_Bi_Di==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(ubidi_getParagraph(pParaBiDi, ಆರಂಭ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, p_Error_ಸಂಕೇತ) !=
       ubidi_getParagraph(pParaBiDi, ಎಲ್ಲೆ-1, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, p_Error_ಸಂಕೇತ)) {
        /* the line crosses a paragraph boundary */
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    /* set the values in pLineBiDi from its pParaBiDi parent */
    p_ಸಾಲು_Bi_Di->pParaBiDi=ಶೂನ್ಯಸೂಚಕ;          /* mark unfinished setLine */
    p_ಸಾಲು_Bi_Di->ಪಠ್ಯ=pParaBiDi->ಪಠ್ಯ+ಆರಂಭ;
    length=p_ಸಾಲು_Bi_Di->length=ಎಲ್ಲೆ-ಆರಂಭ;
    p_ಸಾಲು_Bi_Di->ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ=p_ಸಾಲು_Bi_Di->original_ಉದ್ದಳತೆ=length;
    p_ಸಾಲು_Bi_Di->paraLevel=GET_PARALEVEL(pParaBiDi, ಆರಂಭ);
    p_ಸಾಲು_Bi_Di->para_ಎಣಿಕೆ=pParaBiDi->para_ಎಣಿಕೆ;
    p_ಸಾಲು_Bi_Di->runs=ಶೂನ್ಯಸೂಚಕ;
    p_ಸಾಲು_Bi_Di->flags=0;
    p_ಸಾಲು_Bi_Di->reordering_ಬಹುಲಕ=pParaBiDi->reordering_ಬಹುಲಕ;
    p_ಸಾಲು_Bi_Di->reorderingOptions=pParaBiDi->reorderingOptions;
    p_ಸಾಲು_Bi_Di->control_ಎಣಿಕೆ=0;
    ಆದರೆ(pParaBiDi->control_ಎಣಿಕೆ>0) {
        int32_t j;
        ಗಾಗಿ(j=ಆರಂಭ; j<ಎಲ್ಲೆ; j++) {
            ಆದರೆ(IS_BIDI_CONTROL_CHAR(pParaBiDi->ಪಠ್ಯ[j])) {
                p_ಸಾಲು_Bi_Di->control_ಎಣಿಕೆ++;
            }
        }
        p_ಸಾಲು_Bi_Di->ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ-=p_ಸಾಲು_Bi_Di->control_ಎಣಿಕೆ;
    }

    p_ಸಾಲು_Bi_Di->dirProps=pParaBiDi->dirProps+ಆರಂಭ;
    p_ಸಾಲು_Bi_Di->ಮಟ್ಟಗಳು=pParaBiDi->ಮಟ್ಟಗಳು+ಆರಂಭ;
    p_ಸಾಲು_Bi_Di->ಸತತ_ಎಣಿಕೆ=-1;

    ಆದರೆ(pParaBiDi->direction!=UBIDI_MIXED) {
        /* the parent is already trivial */
        p_ಸಾಲು_Bi_Di->direction=pParaBiDi->direction;

        /*
         * The parent's levels are all either
         * implicitly or explicitly ==paraLevel;
         * do the same here.
         */
        ಆದರೆ(pParaBiDi->trailingWSStart<=ಆರಂಭ) {
            p_ಸಾಲು_Bi_Di->trailingWSStart=0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pParaBiDi->trailingWSStart<ಎಲ್ಲೆ) {
            p_ಸಾಲು_Bi_Di->trailingWSStart=pParaBiDi->trailingWSStart-ಆರಂಭ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            p_ಸಾಲು_Bi_Di->trailingWSStart=length;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಿರಾಂಕ UBiDiLevel *ಮಟ್ಟಗಳು=p_ಸಾಲು_Bi_Di->ಮಟ್ಟಗಳು;
        int32_t i, trailingWSStart;
        UBiDiLevel level;

        ಗಣ_Trailing_WSStart(p_ಸಾಲು_Bi_Di);
        trailingWSStart=p_ಸಾಲು_Bi_Di->trailingWSStart;

        /* recalculate pLineBiDi->direction */
        ಆದರೆ(trailingWSStart==0) {
            /* all levels are at paraLevel */
            p_ಸಾಲು_Bi_Di->direction=(UBiDiDirection)(p_ಸಾಲು_Bi_Di->paraLevel&1);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* get the level of the first character */
            level=(UBiDiLevel)(ಮಟ್ಟಗಳು[0]&1);

            /* if there is anything of a different level, then the line is mixed */
            ಆದರೆ(trailingWSStart<length && (p_ಸಾಲು_Bi_Di->paraLevel&1)!=level) {
                /* the trailing WS is at paraLevel, which differs from levels[0] */
                p_ಸಾಲು_Bi_Di->direction=UBIDI_MIXED;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* see if levels[1..trailingWSStart-1] have the same direction as levels[0] and paraLevel */
                i=1;
                ಗಾಗಿ(;;) {
                    ಆದರೆ(i==trailingWSStart) {
                        /* the direction values match those in level */
                        p_ಸಾಲು_Bi_Di->direction=(UBiDiDirection)level;
                        ನಿಲ್ಲಿಸು;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಮಟ್ಟಗಳು[i]&1)!=level) {
                        p_ಸಾಲು_Bi_Di->direction=UBIDI_MIXED;
                        ನಿಲ್ಲಿಸು;
                    }
                    ++i;
                }
            }
        }

        ಆಯ್ಕೆ(p_ಸಾಲು_Bi_Di->direction) {
        ಸ್ಥಿತಿ UBIDI_LTR:
            /* make sure paraLevel is even */
            p_ಸಾಲು_Bi_Di->paraLevel=(UBiDiLevel)((p_ಸಾಲು_Bi_Di->paraLevel+1)&~1);

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_ಸಾಲು_Bi_Di->trailingWSStart=0;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UBIDI_RTL:
            /* make sure paraLevel is odd */
            p_ಸಾಲು_Bi_Di->paraLevel|=1;

            /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
            p_ಸಾಲು_Bi_Di->trailingWSStart=0;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ನಿಲ್ಲಿಸು;
        }
    }
    p_ಸಾಲು_Bi_Di->pParaBiDi=pParaBiDi;     /* mark successful setLine */
}

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getLevelAt(ಸ್ಥಿರಾಂಕ UBiDi *pBiDi, int32_t char_ಸೂಚ್ಯಂಕ) {
    /* return paraLevel if in the trailing WS run, otherwise the real level */
    ಆದರೆ(!IS_VALID_PARA_OR_ಸಾಲು(pBiDi) || char_ಸೂಚ್ಯಂಕ<0 || pBiDi->length<=char_ಸೂಚ್ಯಂಕ) {
        ಹಿಂತಿರುಗಿ 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pBiDi->direction!=UBIDI_MIXED || char_ಸೂಚ್ಯಂಕ>=pBiDi->trailingWSStart) {
        ಹಿಂತಿರುಗಿ GET_PARALEVEL(pBiDi, char_ಸೂಚ್ಯಂಕ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ pBiDi->ಮಟ್ಟಗಳು[char_ಸೂಚ್ಯಂಕ];
    }
}

U_CAPI ಸ್ಥಿರಾಂಕ UBiDiLevel* U_EXPORT2
ubidi_get_ಮಟ್ಟಗಳು(UBiDi* pBiDi, UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    int32_t ಆರಂಭ, length;

    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ಸಂಕೇತ, ಶೂನ್ಯಸೂಚಕ);
    RETURN_IF_NOT_VALID_PARA_OR_ಸಾಲು(pBiDi, *p_Error_ಸಂಕೇತ, ಶೂನ್ಯಸೂಚಕ);
    ಆದರೆ((length=pBiDi->length)<=0) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ((ಆರಂಭ=pBiDi->trailingWSStart)==length) {
        /* the current levels array reflects the WS run */
        ಹಿಂತಿರುಗಿ pBiDi->ಮಟ್ಟಗಳು;
    }

    /*
     * After the previous if(), we know that the levels array
     * has an implicit trailing WS run and therefore does not fully
     * reflect itself all the levels.
     * This must be a UBiDi object for a line, and
     * we need to create a new levels array.
     */
    ಆದರೆ(get_ಮಟ್ಟಗಳು_ಸ್ಮರಣೆ(pBiDi, length)) {
        UBiDiLevel *ಮಟ್ಟಗಳು=pBiDi->ಮಟ್ಟಗಳು_ಸ್ಮರಣೆ;

        ಆದರೆ(ಆರಂಭ>0 && ಮಟ್ಟಗಳು!=pBiDi->ಮಟ್ಟಗಳು) {
            uprv_memcpy(ಮಟ್ಟಗಳು, pBiDi->ಮಟ್ಟಗಳು, ಆರಂಭ);
        }
        /* pBiDi->paraLevel is ok even if contextual multiple paragraphs,
           since pBidi is a line object                                     */
        uprv_memset(ಮಟ್ಟಗಳು+ಆರಂಭ, pBiDi->paraLevel, length-ಆರಂಭ);

        /* this new levels array is set for the line and reflects the WS run */
        pBiDi->trailingWSStart=length;
        ಹಿಂತಿರುಗಿ pBiDi->ಮಟ್ಟಗಳು=ಮಟ್ಟಗಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* out of memory */
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_get_Logical_ಸತತ(ಸ್ಥಿರಾಂಕ UBiDi *pBiDi, int32_t logicalPosition,
                    int32_t *p_Logical_ಎಲ್ಲೆ, UBiDiLevel *pLevel) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ;
    int32_t ಸತತ_ಎಣಿಕೆ, visual_ಆರಂಭ, logical_ಎಲ್ಲೆ, logical_ಮೊದಲ, i;
    ಸತತ i_ಸತತ;

    error_ಸಂಕೇತ=U_ZERO_ERROR;
    RETURN_VOID_IF_BAD_RANGE(logicalPosition, 0, pBiDi->length, error_ಸಂಕೇತ);
    /* ubidi_countRuns will check VALID_PARA_OR_LINE */
    ಸತತ_ಎಣಿಕೆ=ubidi_ಎಣಿಕೆ_Runs((UBiDi *)pBiDi, &error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    /* this is done based on runs rather than on levels since levels have
       a special interpretation when UBIDI_REORDER_RUNS_ONLY
     */
    visual_ಆರಂಭ=logical_ಎಲ್ಲೆ=0;
    i_ಸತತ=pBiDi->runs[0];

    ಗಾಗಿ(i=0; i<ಸತತ_ಎಣಿಕೆ; i++) {
        i_ಸತತ = pBiDi->runs[i];
        logical_ಮೊದಲ=GET_ಸೂಚ್ಯಂಕ(i_ಸತತ.logical_ಆರಂಭ);
        logical_ಎಲ್ಲೆ=logical_ಮೊದಲ+i_ಸತತ.visual_ಎಲ್ಲೆ-visual_ಆರಂಭ;
        ಆದರೆ((logicalPosition>=logical_ಮೊದಲ) &&
           (logicalPosition<logical_ಎಲ್ಲೆ)) {
            ನಿಲ್ಲಿಸು;
        }
        visual_ಆರಂಭ = i_ಸತತ.visual_ಎಲ್ಲೆ;
    }
    ಆದರೆ(p_Logical_ಎಲ್ಲೆ) {
        *p_Logical_ಎಲ್ಲೆ=logical_ಎಲ್ಲೆ;
    }
    ಆದರೆ(pLevel) {
        ಆದರೆ(pBiDi->reordering_ಬಹುಲಕ==UBIDI_REORDER_RUNS_ONLY) {
            *pLevel=(UBiDiLevel)GET_ODD_ಬಿಟ್(i_ಸತತ.logical_ಆರಂಭ);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pBiDi->direction!=UBIDI_MIXED || logicalPosition>=pBiDi->trailingWSStart) {
            *pLevel=GET_PARALEVEL(pBiDi, logicalPosition);
        } ಇಲ್ಲದಿದ್ದರೆ {
        *pLevel=pBiDi->ಮಟ್ಟಗಳು[logicalPosition];
        }
    }
}

/* runs API functions ------------------------------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_ಎಣಿಕೆ_Runs(UBiDi *pBiDi, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ಸಂಕೇತ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_ಸಾಲು(pBiDi, *p_Error_ಸಂಕೇತ, -1);
    ubidi_getRuns(pBiDi, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ -1;
    }
    ಹಿಂತಿರುಗಿ pBiDi->ಸತತ_ಎಣಿಕೆ;
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_Visual_ಸತತ(UBiDi *pBiDi, int32_t ಸತತ_ಸೂಚ್ಯಂಕ,
                   int32_t *p_Logical_ಆರಂಭ, int32_t *p_ಉದ್ದಳತೆ)
{
    int32_t ಆರಂಭ;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
    RETURN_IF_NOT_VALID_PARA_OR_ಸಾಲು(pBiDi, error_ಸಂಕೇತ, UBIDI_LTR);
    ubidi_getRuns(pBiDi, &error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ UBIDI_LTR;
    }
    RETURN_IF_BAD_RANGE(ಸತತ_ಸೂಚ್ಯಂಕ, 0, pBiDi->ಸತತ_ಎಣಿಕೆ, error_ಸಂಕೇತ, UBIDI_LTR);

    ಆರಂಭ=pBiDi->runs[ಸತತ_ಸೂಚ್ಯಂಕ].logical_ಆರಂಭ;
    ಆದರೆ(p_Logical_ಆರಂಭ!=ಶೂನ್ಯಸೂಚಕ) {
        *p_Logical_ಆರಂಭ=GET_ಸೂಚ್ಯಂಕ(ಆರಂಭ);
    }
    ಆದರೆ(p_ಉದ್ದಳತೆ!=ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(ಸತತ_ಸೂಚ್ಯಂಕ>0) {
            *p_ಉದ್ದಳತೆ=pBiDi->runs[ಸತತ_ಸೂಚ್ಯಂಕ].visual_ಎಲ್ಲೆ-
                     pBiDi->runs[ಸತತ_ಸೂಚ್ಯಂಕ-1].visual_ಎಲ್ಲೆ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            *p_ಉದ್ದಳತೆ=pBiDi->runs[0].visual_ಎಲ್ಲೆ;
        }
    }
    ಹಿಂತಿರುಗಿ (UBiDiDirection)GET_ODD_ಬಿಟ್(ಆರಂಭ);
}

/* in trivial cases there is only one trivial run; called by ubidi_getRuns() */
ಸ್ಥಿರ ಶೂನ್ಯ
get_Single_ಸತತ(UBiDi *pBiDi, UBiDiLevel level) {
    /* simple, single-run case */
    pBiDi->runs=pBiDi->simpleRuns;
    pBiDi->ಸತತ_ಎಣಿಕೆ=1;

    /* fill and reorder the single run */
    pBiDi->runs[0].logical_ಆರಂಭ=MAKE_ಸೂಚ್ಯಂಕ_ODD_PAIR(0, level);
    pBiDi->runs[0].visual_ಎಲ್ಲೆ=pBiDi->length;
    pBiDi->runs[0].ಸೇರಿಸು_Remove=0;
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
ಸ್ಥಿರ ಶೂನ್ಯ
reorder_ಸಾಲು(UBiDi *pBiDi, UBiDiLevel minLevel, UBiDiLevel maxLevel) {
    ಸತತ *runs, ತಾತ್ಕಾಲಿಕ_ಸತತ;
    UBiDiLevel *ಮಟ್ಟಗಳು;
    int32_t ಮೊದಲ_ಸತತ, ಕೊನೆ_ಸತತ, ಎಲ್ಲೆ_ಸತತ, ಸತತ_ಎಣಿಕೆ;

    /* nothing to do? */
    ಆದರೆ(maxLevel<=(minLevel|1)) {
        ಹಿಂತಿರುಗಿ;
    }

    /*
     * Reorder only down to the lowest odd level
     * and reorder at an odd minLevel in a separate, simpler loop.
     * See comments above for why minLevel is always incremented.
     */
    ++minLevel;

    runs=pBiDi->runs;
    ಮಟ್ಟಗಳು=pBiDi->ಮಟ್ಟಗಳು;
    ಸತತ_ಎಣಿಕೆ=pBiDi->ಸತತ_ಎಣಿಕೆ;

    /* do not include the WS run at paraLevel<=old minLevel except in the simple loop */
    ಆದರೆ(pBiDi->trailingWSStart<pBiDi->length) {
        --ಸತತ_ಎಣಿಕೆ;
    }

    ಆಗ(--maxLevel>=minLevel) {
        ಮೊದಲ_ಸತತ=0;

        /* loop for all sequences of runs */
        ಗಾಗಿ(;;) {
            /* look for a sequence of runs that are all at >=maxLevel */
            /* look for the first run of such a sequence */
            ಆಗ(ಮೊದಲ_ಸತತ<ಸತತ_ಎಣಿಕೆ && ಮಟ್ಟಗಳು[runs[ಮೊದಲ_ಸತತ].logical_ಆರಂಭ]<maxLevel) {
                ++ಮೊದಲ_ಸತತ;
            }
            ಆದರೆ(ಮೊದಲ_ಸತತ>=ಸತತ_ಎಣಿಕೆ) {
                ನಿಲ್ಲಿಸು;  /* no more such runs */
            }

            /* look for the limit run of such a sequence (the run behind it) */
            ಗಾಗಿ(ಎಲ್ಲೆ_ಸತತ=ಮೊದಲ_ಸತತ; ++ಎಲ್ಲೆ_ಸತತ<ಸತತ_ಎಣಿಕೆ && ಮಟ್ಟಗಳು[runs[ಎಲ್ಲೆ_ಸತತ].logical_ಆರಂಭ]>=maxLevel;) {}

            /* Swap the entire sequence of runs from firstRun to limitRun-1. */
            ಕೊನೆ_ಸತತ=ಎಲ್ಲೆ_ಸತತ-1;
            ಆಗ(ಮೊದಲ_ಸತತ<ಕೊನೆ_ಸತತ) {
                ತಾತ್ಕಾಲಿಕ_ಸತತ = runs[ಮೊದಲ_ಸತತ];
                runs[ಮೊದಲ_ಸತತ]=runs[ಕೊನೆ_ಸತತ];
                runs[ಕೊನೆ_ಸತತ]=ತಾತ್ಕಾಲಿಕ_ಸತತ;
                ++ಮೊದಲ_ಸತತ;
                --ಕೊನೆ_ಸತತ;
            }

            ಆದರೆ(ಎಲ್ಲೆ_ಸತತ==ಸತತ_ಎಣಿಕೆ) {
                ನಿಲ್ಲಿಸು;  /* no more such runs */
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಮೊದಲ_ಸತತ=ಎಲ್ಲೆ_ಸತತ+1;
            }
        }
    }

    /* now do maxLevel==old minLevel (==odd!), see above */
    ಆದರೆ(!(minLevel&1)) {
        ಮೊದಲ_ಸತತ=0;

        /* include the trailing WS run in this complete reordering */
        ಆದರೆ(pBiDi->trailingWSStart==pBiDi->length) {
            --ಸತತ_ಎಣಿಕೆ;
        }

        /* Swap the entire sequence of all runs. (endRun==runCount) */
        ಆಗ(ಮೊದಲ_ಸತತ<ಸತತ_ಎಣಿಕೆ) {
            ತಾತ್ಕಾಲಿಕ_ಸತತ=runs[ಮೊದಲ_ಸತತ];
            runs[ಮೊದಲ_ಸತತ]=runs[ಸತತ_ಎಣಿಕೆ];
            runs[ಸತತ_ಎಣಿಕೆ]=ತಾತ್ಕಾಲಿಕ_ಸತತ;
            ++ಮೊದಲ_ಸತತ;
            --ಸತತ_ಎಣಿಕೆ;
        }
    }
}

/* compute the runs array --------------------------------------------------- */

ಸ್ಥಿರ int32_t get_ಸತತ_From_Logical_ಸೂಚ್ಯಂಕ(UBiDi *pBiDi, int32_t logical_ಸೂಚ್ಯಂಕ) {
    ಸತತ *runs=pBiDi->runs;
    int32_t ಸತತ_ಎಣಿಕೆ=pBiDi->ಸತತ_ಎಣಿಕೆ, visual_ಆರಂಭ=0, i, length, logical_ಆರಂಭ;

    ಗಾಗಿ(i=0; i<ಸತತ_ಎಣಿಕೆ; i++) {
        length=runs[i].visual_ಎಲ್ಲೆ-visual_ಆರಂಭ;
        logical_ಆರಂಭ=GET_ಸೂಚ್ಯಂಕ(runs[i].logical_ಆರಂಭ);
        ಆದರೆ((logical_ಸೂಚ್ಯಂಕ>=logical_ಆರಂಭ) && (logical_ಸೂಚ್ಯಂಕ<(logical_ಆರಂಭ+length))) {
            ಹಿಂತಿರುಗಿ i;
        }
        visual_ಆರಂಭ+=length;
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
ubidi_getRuns(UBiDi *pBiDi, UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {
    /*
     * This method returns immediately if the runs are already set. This
     * includes the case of length==0 (handled in setPara)..
     */
    ಆದರೆ (pBiDi->ಸತತ_ಎಣಿಕೆ>=0) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    ಆದರೆ(pBiDi->direction!=UBIDI_MIXED) {
        /* simple, single-run case - this covers length==0 */
        /* pBiDi->paraLevel is ok even for contextual multiple paragraphs */
        get_Single_ಸತತ(pBiDi, pBiDi->paraLevel);
    } ಇಲ್ಲದಿದ್ದರೆ /* UBIDI_MIXED, length>0 */ {
        /* mixed directionality */
        int32_t length=pBiDi->length, ಎಲ್ಲೆ;
        UBiDiLevel *ಮಟ್ಟಗಳು=pBiDi->ಮಟ್ಟಗಳು;
        int32_t i, ಸತತ_ಎಣಿಕೆ;
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
        ಎಲ್ಲೆ=pBiDi->trailingWSStart;
        /* count the runs, there is at least one non-WS run, and limit>0 */
        ಸತತ_ಎಣಿಕೆ=0;
        ಗಾಗಿ(i=0; i<ಎಲ್ಲೆ; ++i) {
            /* increment runCount at the start of each run */
            ಆದರೆ(ಮಟ್ಟಗಳು[i]!=level) {
                ++ಸತತ_ಎಣಿಕೆ;
                level=ಮಟ್ಟಗಳು[i];
            }
        }

        /*
         * We don't need to see if the last run can be merged with a trailing
         * WS run because setTrailingWSStart() would have done that.
         */
        ಆದರೆ(ಸತತ_ಎಣಿಕೆ==1 && ಎಲ್ಲೆ==length) {
            /* There is only one non-WS run and no trailing WS-run. */
            get_Single_ಸತತ(pBiDi, ಮಟ್ಟಗಳು[0]);
        } ಇಲ್ಲದಿದ್ದರೆ /* runCount>1 || limit<length */ {
            /* allocate and set the runs */
            ಸತತ *runs;
            int32_t ಸತತ_ಸೂಚ್ಯಂಕ, ಆರಂಭ;
            UBiDiLevel minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1, maxLevel=0;

            /* now, count a (non-mergeable) WS run */
            ಆದರೆ(ಎಲ್ಲೆ<length) {
                ++ಸತತ_ಎಣಿಕೆ;
            }

            /* runCount>1 */
            ಆದರೆ(get_Runs_ಸ್ಮರಣೆ(pBiDi, ಸತತ_ಎಣಿಕೆ)) {
                runs=pBiDi->runs_ಸ್ಮರಣೆ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }

            /* set the runs */
            /* FOOD FOR THOUGHT: this could be optimized, e.g.:
             * 464->444, 484->444, 575->555, 595->555
             * However, that would take longer. Check also how it would
             * interact with BiDi control removal and inserting Marks.
             */
            ಸತತ_ಸೂಚ್ಯಂಕ=0;

            /* search for the run limits and initialize visualLimit values with the run lengths */
            i=0;
            ಮಾಡು {
                /* prepare this run */
                ಆರಂಭ=i;
                level=ಮಟ್ಟಗಳು[i];
                ಆದರೆ(level<minLevel) {
                    minLevel=level;
                }
                ಆದರೆ(level>maxLevel) {
                    maxLevel=level;
                }

                /* look for the run limit */
                ಆಗ(++i<ಎಲ್ಲೆ && ಮಟ್ಟಗಳು[i]==level) {}

                /* i is another run limit */
                runs[ಸತತ_ಸೂಚ್ಯಂಕ].logical_ಆರಂಭ=ಆರಂಭ;
                runs[ಸತತ_ಸೂಚ್ಯಂಕ].visual_ಎಲ್ಲೆ=i-ಆರಂಭ;
                runs[ಸತತ_ಸೂಚ್ಯಂಕ].ಸೇರಿಸು_Remove=0;
                ++ಸತತ_ಸೂಚ್ಯಂಕ;
            } ಆಗ(i<ಎಲ್ಲೆ);

            ಆದರೆ(ಎಲ್ಲೆ<length) {
                /* there is a separate WS run */
                runs[ಸತತ_ಸೂಚ್ಯಂಕ].logical_ಆರಂಭ=ಎಲ್ಲೆ;
                runs[ಸತತ_ಸೂಚ್ಯಂಕ].visual_ಎಲ್ಲೆ=length-ಎಲ್ಲೆ;
                /* For the trailing WS run, pBiDi->paraLevel is ok even
                   if contextual multiple paragraphs.                   */
                ಆದರೆ(pBiDi->paraLevel<minLevel) {
                    minLevel=pBiDi->paraLevel;
                }
            }

            /* set the object fields */
            pBiDi->runs=runs;
            pBiDi->ಸತತ_ಎಣಿಕೆ=ಸತತ_ಎಣಿಕೆ;

            reorder_ಸಾಲು(pBiDi, minLevel, maxLevel);

            /* now add the direction flags and adjust the visualLimit's to be just that */
            /* this loop will also handle the trailing WS run */
            ಎಲ್ಲೆ=0;
            ಗಾಗಿ(i=0; i<ಸತತ_ಎಣಿಕೆ; ++i) {
                ಕೂಡಿಸು_ODD_ಬಿಟ್_FROM_LEVEL(runs[i].logical_ಆರಂಭ, ಮಟ್ಟಗಳು[runs[i].logical_ಆರಂಭ]);
                ಎಲ್ಲೆ+=runs[i].visual_ಎಲ್ಲೆ;
                runs[i].visual_ಎಲ್ಲೆ=ಎಲ್ಲೆ;
            }

            /* Set the "odd" bit for the trailing WS run. */
            /* For a RTL paragraph, it will be the *first* run in visual order. */
            /* For the trailing WS run, pBiDi->paraLevel is ok even if
               contextual multiple paragraphs.                          */
            ಆದರೆ(ಸತತ_ಸೂಚ್ಯಂಕ<ಸತತ_ಎಣಿಕೆ) {
                int32_t trailing_ಸತತ = ((pBiDi->paraLevel & 1) != 0)? 0 : ಸತತ_ಸೂಚ್ಯಂಕ;

                ಕೂಡಿಸು_ODD_ಬಿಟ್_FROM_LEVEL(runs[trailing_ಸತತ].logical_ಆರಂಭ, pBiDi->paraLevel);
            }
        }
    }

    /* handle insert LRM/RLM BEFORE/AFTER run */
    ಆದರೆ(pBiDi->ಸೇರಿಸು_Points.size>0) {
        ಬಿಂದು *ಬಿಂದು_2, *ಆರಂಭ=pBiDi->ಸೇರಿಸು_Points.points,
                      *ಎಲ್ಲೆ=ಆರಂಭ+pBiDi->ಸೇರಿಸು_Points.size;
        int32_t ಸತತ_ಸೂಚ್ಯಂಕ;
        ಗಾಗಿ(ಬಿಂದು_2=ಆರಂಭ; ಬಿಂದು_2<ಎಲ್ಲೆ; ಬಿಂದು_2++) {
            ಸತತ_ಸೂಚ್ಯಂಕ=get_ಸತತ_From_Logical_ಸೂಚ್ಯಂಕ(pBiDi, ಬಿಂದು_2->pos);
            pBiDi->runs[ಸತತ_ಸೂಚ್ಯಂಕ].ಸೇರಿಸು_Remove|=ಬಿಂದು_2->flag;
        }
    }

    /* handle remove BiDi control characters */
    ಆದರೆ(pBiDi->control_ಎಣಿಕೆ>0) {
        int32_t ಸತತ_ಸೂಚ್ಯಂಕ;
        ಸ್ಥಿರಾಂಕ char16_t *ಆರಂಭ=pBiDi->ಪಠ್ಯ, *ಎಲ್ಲೆ=ಆರಂಭ+pBiDi->length, *pu;
        ಗಾಗಿ(pu=ಆರಂಭ; pu<ಎಲ್ಲೆ; pu++) {
            ಆದರೆ(IS_BIDI_CONTROL_CHAR(*pu)) {
                ಸತತ_ಸೂಚ್ಯಂಕ=get_ಸತತ_From_Logical_ಸೂಚ್ಯಂಕ(pBiDi, (int32_t)(pu-ಆರಂಭ));
                pBiDi->runs[ಸತತ_ಸೂಚ್ಯಂಕ].ಸೇರಿಸು_Remove--;
            }
        }
    }

    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ UBool
prepareReorder(ಸ್ಥಿರಾಂಕ UBiDiLevel *ಮಟ್ಟಗಳು, int32_t length,
               int32_t *ಸೂಚ್ಯಂಕ_Map,
               UBiDiLevel *pMinLevel, UBiDiLevel *pMaxLevel) {
    int32_t ಆರಂಭ;
    UBiDiLevel level, minLevel, maxLevel;

    ಆದರೆ(ಮಟ್ಟಗಳು==ಶೂನ್ಯಸೂಚಕ || length<=0) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    /* determine minLevel and maxLevel */
    minLevel=UBIDI_MAX_EXPLICIT_LEVEL+1;
    maxLevel=0;
    ಗಾಗಿ(ಆರಂಭ=length; ಆರಂಭ>0;) {
        level=ಮಟ್ಟಗಳು[--ಆರಂಭ];
        ಆದರೆ(level>UBIDI_MAX_EXPLICIT_LEVEL+1) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        ಆದರೆ(level<minLevel) {
            minLevel=level;
        }
        ಆದರೆ(level>maxLevel) {
            maxLevel=level;
        }
    }
    *pMinLevel=minLevel;
    *pMaxLevel=maxLevel;

    /* initialize the index map */
    ಗಾಗಿ(ಆರಂಭ=length; ಆರಂಭ>0;) {
        --ಆರಂಭ;
        ಸೂಚ್ಯಂಕ_Map[ಆರಂಭ]=ಆರಂಭ;
    }

    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* reorder a line based on a levels array (L2) ------------------------------ */

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_reorderLogical(ಸ್ಥಿರಾಂಕ UBiDiLevel *ಮಟ್ಟಗಳು, int32_t length, int32_t *ಸೂಚ್ಯಂಕ_Map) {
    int32_t ಆರಂಭ, ಎಲ್ಲೆ, ಮೊತ್ತ_ದ_Sos_Eos;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    ಆದರೆ(ಸೂಚ್ಯಂಕ_Map==ಶೂನ್ಯಸೂಚಕ || !prepareReorder(ಮಟ್ಟಗಳು, length, ಸೂಚ್ಯಂಕ_Map, &minLevel, &maxLevel)) {
        ಹಿಂತಿರುಗಿ;
    }

    /* nothing to do? */
    ಆದರೆ(minLevel==maxLevel && (minLevel&1)==0) {
        ಹಿಂತಿರುಗಿ;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    ಮಾಡು {
        ಆರಂಭ=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        ಗಾಗಿ(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            ಆಗ(ಆರಂಭ<length && ಮಟ್ಟಗಳು[ಆರಂಭ]<maxLevel) {
                ++ಆರಂಭ;
            }
            ಆದರೆ(ಆರಂಭ>=length) {
                ನಿಲ್ಲಿಸು;  /* no more such sequences */
            }

            /* look for the limit of such a sequence (the index behind it) */
            ಗಾಗಿ(ಎಲ್ಲೆ=ಆರಂಭ; ++ಎಲ್ಲೆ<length && ಮಟ್ಟಗಳು[ಎಲ್ಲೆ]>=maxLevel;) {}

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
            ಮೊತ್ತ_ದ_Sos_Eos=ಆರಂಭ+ಎಲ್ಲೆ-1;

            /* reorder each index in the sequence */
            ಮಾಡು {
                ಸೂಚ್ಯಂಕ_Map[ಆರಂಭ]=ಮೊತ್ತ_ದ_Sos_Eos-ಸೂಚ್ಯಂಕ_Map[ಆರಂಭ];
            } ಆಗ(++ಆರಂಭ<ಎಲ್ಲೆ);

            /* start==limit */
            ಆದರೆ(ಎಲ್ಲೆ==length) {
                ನಿಲ್ಲಿಸು;  /* no more such sequences */
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆರಂಭ=ಎಲ್ಲೆ+1;
            }
        }
    } ಆಗ(--maxLevel>=minLevel);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_reorderVisual(ಸ್ಥಿರಾಂಕ UBiDiLevel *ಮಟ್ಟಗಳು, int32_t length, int32_t *ಸೂಚ್ಯಂಕ_Map) {
    int32_t ಆರಂಭ, ಕೊನೆ, ಎಲ್ಲೆ, ತಾತ್ಕಾಲಿಕ;
    UBiDiLevel minLevel = 0, maxLevel = 0;

    ಆದರೆ(ಸೂಚ್ಯಂಕ_Map==ಶೂನ್ಯಸೂಚಕ || !prepareReorder(ಮಟ್ಟಗಳು, length, ಸೂಚ್ಯಂಕ_Map, &minLevel, &maxLevel)) {
        ಹಿಂತಿರುಗಿ;
    }

    /* nothing to do? */
    ಆದರೆ(minLevel==maxLevel && (minLevel&1)==0) {
        ಹಿಂತಿರುಗಿ;
    }

    /* reorder only down to the lowest odd level */
    minLevel|=1;

    /* loop maxLevel..minLevel */
    ಮಾಡು {
        ಆರಂಭ=0;

        /* loop for all sequences of levels to reorder at the current maxLevel */
        ಗಾಗಿ(;;) {
            /* look for a sequence of levels that are all at >=maxLevel */
            /* look for the first index of such a sequence */
            ಆಗ(ಆರಂಭ<length && ಮಟ್ಟಗಳು[ಆರಂಭ]<maxLevel) {
                ++ಆರಂಭ;
            }
            ಆದರೆ(ಆರಂಭ>=length) {
                ನಿಲ್ಲಿಸು;  /* no more such runs */
            }

            /* look for the limit of such a sequence (the index behind it) */
            ಗಾಗಿ(ಎಲ್ಲೆ=ಆರಂಭ; ++ಎಲ್ಲೆ<length && ಮಟ್ಟಗಳು[ಎಲ್ಲೆ]>=maxLevel;) {}

            /*
             * Swap the entire interval of indexes from start to limit-1.
             * We don't need to swap the levels for the purpose of this
             * algorithm: the sequence of levels that we look at does not
             * move anyway.
             */
            ಕೊನೆ=ಎಲ್ಲೆ-1;
            ಆಗ(ಆರಂಭ<ಕೊನೆ) {
                ತಾತ್ಕಾಲಿಕ=ಸೂಚ್ಯಂಕ_Map[ಆರಂಭ];
                ಸೂಚ್ಯಂಕ_Map[ಆರಂಭ]=ಸೂಚ್ಯಂಕ_Map[ಕೊನೆ];
                ಸೂಚ್ಯಂಕ_Map[ಕೊನೆ]=ತಾತ್ಕಾಲಿಕ;

                ++ಆರಂಭ;
                --ಕೊನೆ;
            }

            ಆದರೆ(ಎಲ್ಲೆ==length) {
                ನಿಲ್ಲಿಸು;  /* no more such sequences */
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆರಂಭ=ಎಲ್ಲೆ+1;
            }
        }
    } ಆಗ(--maxLevel>=minLevel);
}

/* API functions for logical<->visual mapping ------------------------------- */

U_CAPI int32_t U_EXPORT2
ubidi_get_Visual_ಸೂಚ್ಯಂಕ(UBiDi *pBiDi, int32_t logical_ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    int32_t visual_ಸೂಚ್ಯಂಕ=UBIDI_MAP_NOWHERE;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ಸಂಕೇತ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_ಸಾಲು(pBiDi, *p_Error_ಸಂಕೇತ, -1);
    RETURN_IF_BAD_RANGE(logical_ಸೂಚ್ಯಂಕ, 0, pBiDi->length, *p_Error_ಸಂಕೇತ, -1);

    /* we can do the trivial cases without the runs array */
    ಆಯ್ಕೆ(pBiDi->direction) {
    ಸ್ಥಿತಿ UBIDI_LTR:
        visual_ಸೂಚ್ಯಂಕ=logical_ಸೂಚ್ಯಂಕ;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UBIDI_RTL:
        visual_ಸೂಚ್ಯಂಕ=pBiDi->length-logical_ಸೂಚ್ಯಂಕ-1;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಆದರೆ(!ubidi_getRuns(pBiDi, p_Error_ಸಂಕೇತ)) {
            *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ -1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸತತ *runs=pBiDi->runs;
            int32_t i, visual_ಆರಂಭ=0, ಸ್ಥಾನಾಂತರ, length;

            /* linear search for the run, search on the visual runs */
            ಗಾಗಿ(i=0; i<pBiDi->ಸತತ_ಎಣಿಕೆ; ++i) {
                length=runs[i].visual_ಎಲ್ಲೆ-visual_ಆರಂಭ;
                ಸ್ಥಾನಾಂತರ=logical_ಸೂಚ್ಯಂಕ-GET_ಸೂಚ್ಯಂಕ(runs[i].logical_ಆರಂಭ);
                ಆದರೆ(ಸ್ಥಾನಾಂತರ>=0 && ಸ್ಥಾನಾಂತರ<length) {
                    ಆದರೆ(IS_EVEN_ಸತತ(runs[i].logical_ಆರಂಭ)) {
                        /* LTR */
                        visual_ಸೂಚ್ಯಂಕ=visual_ಆರಂಭ+ಸ್ಥಾನಾಂತರ;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* RTL */
                        visual_ಸೂಚ್ಯಂಕ=visual_ಆರಂಭ+length-ಸ್ಥಾನಾಂತರ-1;
                    }
                    ನಿಲ್ಲಿಸು;          /* exit for loop */
                }
                visual_ಆರಂಭ+=length;
            }
            ಆದರೆ(i>=pBiDi->ಸತತ_ಎಣಿಕೆ) {
                ಹಿಂತಿರುಗಿ UBIDI_MAP_NOWHERE;
            }
        }
    }

    ಆದರೆ(pBiDi->ಸೇರಿಸು_Points.size>0) {
        /* add the number of added marks until the calculated visual index */
        ಸತತ *runs=pBiDi->runs;
        int32_t i, length, ಸೇರಿಸು_Remove;
        int32_t visual_ಆರಂಭ=0, mark_ಸಿಕ್ಕಿತು=0;
        ಗಾಗಿ(i=0; ; i++, visual_ಆರಂಭ+=length) {
            length=runs[i].visual_ಎಲ್ಲೆ-visual_ಆರಂಭ;
            ಸೇರಿಸು_Remove=runs[i].ಸೇರಿಸು_Remove;
            ಆದರೆ(ಸೇರಿಸು_Remove & (LRM_BEFORE|RLM_BEFORE)) {
                mark_ಸಿಕ್ಕಿತು++;
            }
            /* is it the run containing the visual index? */
            ಆದರೆ(visual_ಸೂಚ್ಯಂಕ<runs[i].visual_ಎಲ್ಲೆ) {
                ಹಿಂತಿರುಗಿ visual_ಸೂಚ್ಯಂಕ+mark_ಸಿಕ್ಕಿತು;
            }
            ಆದರೆ(ಸೇರಿಸು_Remove & (LRM_AFTER|RLM_AFTER)) {
                mark_ಸಿಕ್ಕಿತು++;
            }
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pBiDi->control_ಎಣಿಕೆ>0) {
        /* subtract the number of controls until the calculated visual index */
        ಸತತ *runs=pBiDi->runs;
        int32_t i, j, ಆರಂಭ, ಎಲ್ಲೆ, length, ಸೇರಿಸು_Remove;
        int32_t visual_ಆರಂಭ=0, control_ಸಿಕ್ಕಿತು=0;
        char16_t uchar=pBiDi->ಪಠ್ಯ[logical_ಸೂಚ್ಯಂಕ];
        /* is the logical index pointing to a control ? */
        ಆದರೆ(IS_BIDI_CONTROL_CHAR(uchar)) {
            ಹಿಂತಿರುಗಿ UBIDI_MAP_NOWHERE;
        }
        /* loop on runs */
        ಗಾಗಿ(i=0; ; i++, visual_ಆರಂಭ+=length) {
            length=runs[i].visual_ಎಲ್ಲೆ-visual_ಆರಂಭ;
            ಸೇರಿಸು_Remove=runs[i].ಸೇರಿಸು_Remove;
            /* calculated visual index is beyond this run? */
            ಆದರೆ(visual_ಸೂಚ್ಯಂಕ>=runs[i].visual_ಎಲ್ಲೆ) {
                control_ಸಿಕ್ಕಿತು-=ಸೇರಿಸು_Remove;
                ಮುಂದುವರಿ;
            }
            /* calculated visual index must be within current run */
            ಆದರೆ(ಸೇರಿಸು_Remove==0) {
                ಹಿಂತಿರುಗಿ visual_ಸೂಚ್ಯಂಕ-control_ಸಿಕ್ಕಿತು;
            }
            ಆದರೆ(IS_EVEN_ಸತತ(runs[i].logical_ಆರಂಭ)) {
                /* LTR: check from run start to logical index */
                ಆರಂಭ=runs[i].logical_ಆರಂಭ;
                ಎಲ್ಲೆ=logical_ಸೂಚ್ಯಂಕ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* RTL: check from logical index to run end */
                ಆರಂಭ=logical_ಸೂಚ್ಯಂಕ+1;
                ಎಲ್ಲೆ=GET_ಸೂಚ್ಯಂಕ(runs[i].logical_ಆರಂಭ)+length;
            }
            ಗಾಗಿ(j=ಆರಂಭ; j<ಎಲ್ಲೆ; j++) {
                uchar=pBiDi->ಪಠ್ಯ[j];
                ಆದರೆ(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_ಸಿಕ್ಕಿತು++;
                }
            }
            ಹಿಂತಿರುಗಿ visual_ಸೂಚ್ಯಂಕ-control_ಸಿಕ್ಕಿತು;
        }
    }

    ಹಿಂತಿರುಗಿ visual_ಸೂಚ್ಯಂಕ;
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Logical_ಸೂಚ್ಯಂಕ(UBiDi *pBiDi, int32_t visual_ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸತತ *runs;
    int32_t i, ಸತತ_ಎಣಿಕೆ, ಆರಂಭ;
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ಸಂಕೇತ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_ಸಾಲು(pBiDi, *p_Error_ಸಂಕೇತ, -1);
    RETURN_IF_BAD_RANGE(visual_ಸೂಚ್ಯಂಕ, 0, pBiDi->ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, *p_Error_ಸಂಕೇತ, -1);
    /* we can do the trivial cases without the runs array */
    ಆದರೆ(pBiDi->ಸೇರಿಸು_Points.size==0 && pBiDi->control_ಎಣಿಕೆ==0) {
        ಆದರೆ(pBiDi->direction==UBIDI_LTR) {
            ಹಿಂತಿರುಗಿ visual_ಸೂಚ್ಯಂಕ;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pBiDi->direction==UBIDI_RTL) {
            ಹಿಂತಿರುಗಿ pBiDi->length-visual_ಸೂಚ್ಯಂಕ-1;
        }
    }
    ಆದರೆ(!ubidi_getRuns(pBiDi, p_Error_ಸಂಕೇತ)) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ -1;
    }

    runs=pBiDi->runs;
    ಸತತ_ಎಣಿಕೆ=pBiDi->ಸತತ_ಎಣಿಕೆ;
    ಆದರೆ(pBiDi->ಸೇರಿಸು_Points.size>0) {
        /* handle inserted LRM/RLM */
        int32_t mark_ಸಿಕ್ಕಿತು=0, ಸೇರಿಸು_Remove;
        int32_t visual_ಆರಂಭ=0, length;
        runs=pBiDi->runs;
        /* subtract number of marks until visual index */
        ಗಾಗಿ(i=0; ; i++, visual_ಆರಂಭ+=length) {
            length=runs[i].visual_ಎಲ್ಲೆ-visual_ಆರಂಭ;
            ಸೇರಿಸು_Remove=runs[i].ಸೇರಿಸು_Remove;
            ಆದರೆ(ಸೇರಿಸು_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                ಆದರೆ(visual_ಸೂಚ್ಯಂಕ<=(visual_ಆರಂಭ+mark_ಸಿಕ್ಕಿತು)) {
                    ಹಿಂತಿರುಗಿ UBIDI_MAP_NOWHERE;
                }
                mark_ಸಿಕ್ಕಿತು++;
            }
            /* is adjusted visual index within this run? */
            ಆದರೆ(visual_ಸೂಚ್ಯಂಕ<(runs[i].visual_ಎಲ್ಲೆ+mark_ಸಿಕ್ಕಿತು)) {
                visual_ಸೂಚ್ಯಂಕ-=mark_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(ಸೇರಿಸು_Remove&(LRM_AFTER|RLM_AFTER)) {
                ಆದರೆ(visual_ಸೂಚ್ಯಂಕ==(visual_ಆರಂಭ+length+mark_ಸಿಕ್ಕಿತು)) {
                    ಹಿಂತಿರುಗಿ UBIDI_MAP_NOWHERE;
                }
                mark_ಸಿಕ್ಕಿತು++;
            }
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pBiDi->control_ಎಣಿಕೆ>0) {
        /* handle removed BiDi control characters */
        int32_t control_ಸಿಕ್ಕಿತು=0, ಸೇರಿಸು_Remove, length;
        int32_t logical_ಆರಂಭ, logical_ಕೊನೆ, visual_ಆರಂಭ=0, j, k;
        char16_t uchar;
        UBool even_ಸತತ;
        /* add number of controls until visual index */
        ಗಾಗಿ(i=0; ; i++, visual_ಆರಂಭ+=length) {
            length=runs[i].visual_ಎಲ್ಲೆ-visual_ಆರಂಭ;
            ಸೇರಿಸು_Remove=runs[i].ಸೇರಿಸು_Remove;
            /* is adjusted visual index beyond current run? */
            ಆದರೆ(visual_ಸೂಚ್ಯಂಕ>=(runs[i].visual_ಎಲ್ಲೆ-control_ಸಿಕ್ಕಿತು+ಸೇರಿಸು_Remove)) {
                control_ಸಿಕ್ಕಿತು-=ಸೇರಿಸು_Remove;
                ಮುಂದುವರಿ;
            }
            /* adjusted visual index is within current run */
            ಆದರೆ(ಸೇರಿಸು_Remove==0) {
                visual_ಸೂಚ್ಯಂಕ+=control_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            }
            /* count non-control chars until visualIndex */
            logical_ಆರಂಭ=runs[i].logical_ಆರಂಭ;
            even_ಸತತ=IS_EVEN_ಸತತ(logical_ಆರಂಭ);
            REMOVE_ODD_ಬಿಟ್(logical_ಆರಂಭ);
            logical_ಕೊನೆ=logical_ಆರಂಭ+length-1;
            ಗಾಗಿ(j=0; j<length; j++) {
                k= even_ಸತತ ? logical_ಆರಂಭ+j : logical_ಕೊನೆ-j;
                uchar=pBiDi->ಪಠ್ಯ[k];
                ಆದರೆ(IS_BIDI_CONTROL_CHAR(uchar)) {
                    control_ಸಿಕ್ಕಿತು++;
                }
                ಆದರೆ((visual_ಸೂಚ್ಯಂಕ+control_ಸಿಕ್ಕಿತು)==(visual_ಆರಂಭ+j)) {
                    ನಿಲ್ಲಿಸು;
                }
            }
            visual_ಸೂಚ್ಯಂಕ+=control_ಸಿಕ್ಕಿತು;
            ನಿಲ್ಲಿಸು;
        }
    }
    /* handle all cases */
    ಆದರೆ(ಸತತ_ಎಣಿಕೆ<=10) {
        /* linear search for the run */
        ಗಾಗಿ(i=0; visual_ಸೂಚ್ಯಂಕ>=runs[i].visual_ಎಲ್ಲೆ; ++i) {}
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* binary search for the run */
        int32_t begin=0, ಎಲ್ಲೆ=ಸತತ_ಎಣಿಕೆ;

        /* the middle if() is guaranteed to find the run, we don't need a loop limit */
        ಗಾಗಿ(;;) {
            i=(begin+ಎಲ್ಲೆ)/2;
            ಆದರೆ(visual_ಸೂಚ್ಯಂಕ>=runs[i].visual_ಎಲ್ಲೆ) {
                begin=i+1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(i==0 || visual_ಸೂಚ್ಯಂಕ>=runs[i-1].visual_ಎಲ್ಲೆ) {
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಎಲ್ಲೆ=i;
            }
        }
    }

    ಆರಂಭ=runs[i].logical_ಆರಂಭ;
    ಆದರೆ(IS_EVEN_ಸತತ(ಆರಂಭ)) {
        /* LTR */
        /* the offset in runs[i] is visualIndex-runs[i-1].visualLimit */
        ಆದರೆ(i>0) {
            visual_ಸೂಚ್ಯಂಕ-=runs[i-1].visual_ಎಲ್ಲೆ;
        }
        ಹಿಂತಿರುಗಿ ಆರಂಭ+visual_ಸೂಚ್ಯಂಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* RTL */
        ಹಿಂತಿರುಗಿ GET_ಸೂಚ್ಯಂಕ(ಆರಂಭ)+runs[i].visual_ಎಲ್ಲೆ-visual_ಸೂಚ್ಯಂಕ-1;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_getLogicalMap(UBiDi *pBiDi, int32_t *ಸೂಚ್ಯಂಕ_Map, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ಸಂಕೇತ);
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_ಎಣಿಕೆ_Runs(pBiDi, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        /* no op */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸೂಚ್ಯಂಕ_Map==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* fill a logical-to-visual index map using the runs[] */
        int32_t visual_ಆರಂಭ, visual_ಎಲ್ಲೆ, i, j, k;
        int32_t logical_ಆರಂಭ, logical_ಎಲ್ಲೆ;
        ಸತತ *runs=pBiDi->runs;
        ಆದರೆ (pBiDi->length<=0) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (pBiDi->length>pBiDi->ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ) {
            uprv_memset(ಸೂಚ್ಯಂಕ_Map, 0xFF, pBiDi->length*ಗಾತ್ರ(int32_t));
        }

        visual_ಆರಂಭ=0;
        ಗಾಗಿ(j=0; j<pBiDi->ಸತತ_ಎಣಿಕೆ; ++j) {
            logical_ಆರಂಭ=GET_ಸೂಚ್ಯಂಕ(runs[j].logical_ಆರಂಭ);
            visual_ಎಲ್ಲೆ=runs[j].visual_ಎಲ್ಲೆ;
            ಆದರೆ(IS_EVEN_ಸತತ(runs[j].logical_ಆರಂಭ)) {
                ಮಾಡು { /* LTR */
                    ಸೂಚ್ಯಂಕ_Map[logical_ಆರಂಭ++]=visual_ಆರಂಭ++;
                } ಆಗ(visual_ಆರಂಭ<visual_ಎಲ್ಲೆ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                logical_ಆರಂಭ+=visual_ಎಲ್ಲೆ-visual_ಆರಂಭ;  /* logicalLimit */
                ಮಾಡು { /* RTL */
                    ಸೂಚ್ಯಂಕ_Map[--logical_ಆರಂಭ]=visual_ಆರಂಭ++;
                } ಆಗ(visual_ಆರಂಭ<visual_ಎಲ್ಲೆ);
            }
            /* visualStart==visualLimit; */
        }

        ಆದರೆ(pBiDi->ಸೇರಿಸು_Points.size>0) {
            int32_t mark_ಸಿಕ್ಕಿತು=0, ಸತತ_ಎಣಿಕೆ=pBiDi->ಸತತ_ಎಣಿಕೆ;
            int32_t length, ಸೇರಿಸು_Remove;
            visual_ಆರಂಭ=0;
            /* add number of marks found until each index */
            ಗಾಗಿ(i=0; i<ಸತತ_ಎಣಿಕೆ; i++, visual_ಆರಂಭ+=length) {
                length=runs[i].visual_ಎಲ್ಲೆ-visual_ಆರಂಭ;
                ಸೇರಿಸು_Remove=runs[i].ಸೇರಿಸು_Remove;
                ಆದರೆ(ಸೇರಿಸು_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_ಸಿಕ್ಕಿತು++;
                }
                ಆದರೆ(mark_ಸಿಕ್ಕಿತು>0) {
                    logical_ಆರಂಭ=GET_ಸೂಚ್ಯಂಕ(runs[i].logical_ಆರಂಭ);
                    logical_ಎಲ್ಲೆ=logical_ಆರಂಭ+length;
                    ಗಾಗಿ(j=logical_ಆರಂಭ; j<logical_ಎಲ್ಲೆ; j++) {
                        ಸೂಚ್ಯಂಕ_Map[j]+=mark_ಸಿಕ್ಕಿತು;
                    }
                }
                ಆದರೆ(ಸೇರಿಸು_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_ಸಿಕ್ಕಿತು++;
                }
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pBiDi->control_ಎಣಿಕೆ>0) {
            int32_t control_ಸಿಕ್ಕಿತು=0, ಸತತ_ಎಣಿಕೆ=pBiDi->ಸತತ_ಎಣಿಕೆ;
            int32_t length, ಸೇರಿಸು_Remove;
            UBool even_ಸತತ;
            char16_t uchar;
            visual_ಆರಂಭ=0;
            /* subtract number of controls found until each index */
            ಗಾಗಿ(i=0; i<ಸತತ_ಎಣಿಕೆ; i++, visual_ಆರಂಭ+=length) {
                length=runs[i].visual_ಎಲ್ಲೆ-visual_ಆರಂಭ;
                ಸೇರಿಸು_Remove=runs[i].ಸೇರಿಸು_Remove;
                /* no control found within previous runs nor within this run */
                ಆದರೆ((control_ಸಿಕ್ಕಿತು-ಸೇರಿಸು_Remove)==0) {
                    ಮುಂದುವರಿ;
                }
                logical_ಆರಂಭ=runs[i].logical_ಆರಂಭ;
                even_ಸತತ=IS_EVEN_ಸತತ(logical_ಆರಂಭ);
                REMOVE_ODD_ಬಿಟ್(logical_ಆರಂಭ);
                logical_ಎಲ್ಲೆ=logical_ಆರಂಭ+length;
                /* if no control within this run */
                ಆದರೆ(ಸೇರಿಸು_Remove==0) {
                    ಗಾಗಿ(j=logical_ಆರಂಭ; j<logical_ಎಲ್ಲೆ; j++) {
                        ಸೂಚ್ಯಂಕ_Map[j]-=control_ಸಿಕ್ಕಿತು;
                    }
                    ಮುಂದುವರಿ;
                }
                ಗಾಗಿ(j=0; j<length; j++) {
                    k= even_ಸತತ ? logical_ಆರಂಭ+j : logical_ಎಲ್ಲೆ-j-1;
                    uchar=pBiDi->ಪಠ್ಯ[k];
                    ಆದರೆ(IS_BIDI_CONTROL_CHAR(uchar)) {
                        control_ಸಿಕ್ಕಿತು++;
                        ಸೂಚ್ಯಂಕ_Map[k]=UBIDI_MAP_NOWHERE;
                        ಮುಂದುವರಿ;
                    }
                    ಸೂಚ್ಯಂಕ_Map[k]-=control_ಸಿಕ್ಕಿತು;
                }
            }
        }
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_getVisualMap(UBiDi *pBiDi, int32_t *ಸೂಚ್ಯಂಕ_Map, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ಸಂಕೇತ);
    ಆದರೆ(ಸೂಚ್ಯಂಕ_Map==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    /* ubidi_countRuns() checks for VALID_PARA_OR_LINE */
    ubidi_ಎಣಿಕೆ_Runs(pBiDi, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        /* fill a visual-to-logical index map using the runs[] */
        ಸತತ *runs=pBiDi->runs, *runs_ಎಲ್ಲೆ=runs+pBiDi->ಸತತ_ಎಣಿಕೆ;
        int32_t logical_ಆರಂಭ, visual_ಆರಂಭ, visual_ಎಲ್ಲೆ, *pi=ಸೂಚ್ಯಂಕ_Map;

        ಆದರೆ (pBiDi->ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ<=0) {
            ಹಿಂತಿರುಗಿ;
        }
        visual_ಆರಂಭ=0;
        ಗಾಗಿ(; runs<runs_ಎಲ್ಲೆ; ++runs) {
            logical_ಆರಂಭ=runs->logical_ಆರಂಭ;
            visual_ಎಲ್ಲೆ=runs->visual_ಎಲ್ಲೆ;
            ಆದರೆ(IS_EVEN_ಸತತ(logical_ಆರಂಭ)) {
                ಮಾಡು { /* LTR */
                    *pi++ = logical_ಆರಂಭ++;
                } ಆಗ(++visual_ಆರಂಭ<visual_ಎಲ್ಲೆ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                REMOVE_ODD_ಬಿಟ್(logical_ಆರಂಭ);
                logical_ಆರಂಭ+=visual_ಎಲ್ಲೆ-visual_ಆರಂಭ;  /* logicalLimit */
                ಮಾಡು { /* RTL */
                    *pi++ = --logical_ಆರಂಭ;
                } ಆಗ(++visual_ಆರಂಭ<visual_ಎಲ್ಲೆ);
            }
            /* visualStart==visualLimit; */
        }

        ಆದರೆ(pBiDi->ಸೇರಿಸು_Points.size>0) {
            int32_t mark_ಸಿಕ್ಕಿತು=0, ಸತತ_ಎಣಿಕೆ=pBiDi->ಸತತ_ಎಣಿಕೆ;
            int32_t ಸೇರಿಸು_Remove, i, j, k;
            runs=pBiDi->runs;
            /* count all inserted marks */
            ಗಾಗಿ(i=0; i<ಸತತ_ಎಣಿಕೆ; i++) {
                ಸೇರಿಸು_Remove=runs[i].ಸೇರಿಸು_Remove;
                ಆದರೆ(ಸೇರಿಸು_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    mark_ಸಿಕ್ಕಿತು++;
                }
                ಆದರೆ(ಸೇರಿಸು_Remove&(LRM_AFTER|RLM_AFTER)) {
                    mark_ಸಿಕ್ಕಿತು++;
                }
            }
            /* move back indexes by number of preceding marks */
            k=pBiDi->ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ;
            ಗಾಗಿ(i=ಸತತ_ಎಣಿಕೆ-1; i>=0 && mark_ಸಿಕ್ಕಿತು>0; i--) {
                ಸೇರಿಸು_Remove=runs[i].ಸೇರಿಸು_Remove;
                ಆದರೆ(ಸೇರಿಸು_Remove&(LRM_AFTER|RLM_AFTER)) {
                    ಸೂಚ್ಯಂಕ_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_ಸಿಕ್ಕಿತು--;
                }
                visual_ಆರಂಭ= i>0 ? runs[i-1].visual_ಎಲ್ಲೆ : 0;
                ಗಾಗಿ(j=runs[i].visual_ಎಲ್ಲೆ-1; j>=visual_ಆರಂಭ && mark_ಸಿಕ್ಕಿತು>0; j--) {
                    ಸೂಚ್ಯಂಕ_Map[--k]=ಸೂಚ್ಯಂಕ_Map[j];
                }
                ಆದರೆ(ಸೇರಿಸು_Remove&(LRM_BEFORE|RLM_BEFORE)) {
                    ಸೂಚ್ಯಂಕ_Map[--k]= UBIDI_MAP_NOWHERE;
                    mark_ಸಿಕ್ಕಿತು--;
                }
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pBiDi->control_ಎಣಿಕೆ>0) {
            int32_t ಸತತ_ಎಣಿಕೆ=pBiDi->ಸತತ_ಎಣಿಕೆ, logical_ಕೊನೆ;
            int32_t ಸೇರಿಸು_Remove, length, i, j, k, m;
            char16_t uchar;
            UBool even_ಸತತ;
            runs=pBiDi->runs;
            visual_ಆರಂಭ=0;
            /* move forward indexes by number of preceding controls */
            k=0;
            ಗಾಗಿ(i=0; i<ಸತತ_ಎಣಿಕೆ; i++, visual_ಆರಂಭ+=length) {
                length=runs[i].visual_ಎಲ್ಲೆ-visual_ಆರಂಭ;
                ಸೇರಿಸು_Remove=runs[i].ಸೇರಿಸು_Remove;
                /* if no control found yet, nothing to do in this run */
                ಆದರೆ((ಸೇರಿಸು_Remove==0)&&(k==visual_ಆರಂಭ)) {
                    k+=length;
                    ಮುಂದುವರಿ;
                }
                /* if no control in this run */
                ಆದರೆ(ಸೇರಿಸು_Remove==0) {
                    visual_ಎಲ್ಲೆ=runs[i].visual_ಎಲ್ಲೆ;
                    ಗಾಗಿ(j=visual_ಆರಂಭ; j<visual_ಎಲ್ಲೆ; j++) {
                        ಸೂಚ್ಯಂಕ_Map[k++]=ಸೂಚ್ಯಂಕ_Map[j];
                    }
                    ಮುಂದುವರಿ;
                }
                logical_ಆರಂಭ=runs[i].logical_ಆರಂಭ;
                even_ಸತತ=IS_EVEN_ಸತತ(logical_ಆರಂಭ);
                REMOVE_ODD_ಬಿಟ್(logical_ಆರಂಭ);
                logical_ಕೊನೆ=logical_ಆರಂಭ+length-1;
                ಗಾಗಿ(j=0; j<length; j++) {
                    m= even_ಸತತ ? logical_ಆರಂಭ+j : logical_ಕೊನೆ-j;
                    uchar=pBiDi->ಪಠ್ಯ[m];
                    ಆದರೆ(!IS_BIDI_CONTROL_CHAR(uchar)) {
                        ಸೂಚ್ಯಂಕ_Map[k++]=m;
                    }
                }
            }
        }
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_invertMap(ಸ್ಥಿರಾಂಕ int32_t *srcMap, int32_t *destMap, int32_t length) {
    ಆದರೆ(srcMap!=ಶೂನ್ಯಸೂಚಕ && destMap!=ಶೂನ್ಯಸೂಚಕ && length>0) {
        ಸ್ಥಿರಾಂಕ int32_t *pi;
        int32_t dest_ಉದ್ದಳತೆ=-1, count=0;
        /* find highest value and count positive indexes in srcMap */
        pi=srcMap+length;
        ಆಗ(pi>srcMap) {
            ಆದರೆ(*--pi>dest_ಉದ್ದಳತೆ) {
                dest_ಉದ್ದಳತೆ=*pi;
            }
            ಆದರೆ(*pi>=0) {
                count++;
            }
        }
        dest_ಉದ್ದಳತೆ++;           /* add 1 for origin 0 */
        ಆದರೆ(count<dest_ಉದ್ದಳತೆ) {
            /* we must fill unmatched destMap entries with -1 */
            uprv_memset(destMap, 0xFF, dest_ಉದ್ದಳತೆ*ಗಾತ್ರ(int32_t));
        }
        pi=srcMap+length;
        ಆಗ(length>0) {
            ಆದರೆ(*--pi>=0) {
                destMap[*pi]=--length;
            } ಇಲ್ಲದಿದ್ದರೆ {
                --length;
            }
        }
    }
}
