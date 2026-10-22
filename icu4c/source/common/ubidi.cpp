// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  ubidi.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999jul27
*   created by: Markus W. Scherer, updated by Matitiahu Allouche
*
*/

#ಅನ್ತರ್ಭಾವ <limits>

#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubidi.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ubidi_props.h"
#ಅನ್ತರ್ಭಾವ "ubidiimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

/*
 * General implementation notes:
 *
 * Throughout the implementation, there are comments like (W2) that refer to
 * rules of the BiDi algorithm, in this example to the second rule of the
 * resolution of weak types.
 *
 * For handling surrogate pairs, where two char16_t's form one "abstract" (or UTF-32)
 * character according to UTF-16, the second char16_t gets the directional property of
 * the entire character assigned, while the first one gets a BN, a boundary
 * neutral, type, which is ignored by most of the algorithm according to
 * rule (X9) and the implementation suggestions of the BiDi algorithm.
 *
 * Later, adjustWSLevels() will set the level for each BN to that of the
 * following character (char16_t), which results in surrogate pairs getting the
 * same level on each of their surrogates.
 *
 * In a UTF-8 implementation, the same thing could be done: the last byte of
 * a multi-byte sequence would get the "real" property, while all previous
 * bytes of that sequence would get BN.
 *
 * It is not possible to assign all those parts of a character the same real
 * property because this would fail in the resolution of weak types with rules
 * that look at immediately surrounding types.
 *
 * As a related topic, this implementation does not remove Boundary Neutral
 * types from the input, but ignores them wherever this is relevant.
 * For example, the loop for the resolution of the weak types reads
 * types until it finds a non-BN.
 * Also, explicit embedding codes are neither changed into BN nor removed.
 * They are only treated the same way real BNs are.
 * As stated before, adjustWSLevels() takes care of them at the end.
 * For the purpose of conformance, the levels of all these codes
 * do not matter.
 *
 * Note that this implementation modifies the dirProps
 * after the initial setup, when applying X5c (replace FSI by LRI or RLI),
 * X6, N0 (replace paired brackets by L or R).
 *
 * In this implementation, the resolution of weak types (W1 to W6),
 * neutrals (N1 and N2), and the assignment of the resolved level (In)
 * are all done in one single loop, in resolveImplicitLevels().
 * Changes of dirProp values are done on the fly, without writing
 * them back to the dirProps array.
 *
 *
 * This implementation contains code that allows to bypass steps of the
 * algorithm that are not needed on the specific paragraph
 * in order to speed up the most common cases considerably,
 * like text that is entirely LTR, or RTL text without numbers.
 *
 * Most of this is done by setting a bit for each directional property
 * in a flags variable and later checking for whether there are
 * any LTR characters or any RTL characters, or both, whether
 * there are any explicit embedding codes, etc.
 *
 * If the (Xn) steps are performed, then the flags are re-evaluated,
 * because they will then not contain the embedding codes any more
 * and will be adjusted for override codes, so that subsequently
 * more bypassing may be possible than what the initial flags suggested.
 *
 * If the text is not mixed-directional, then the
 * algorithm steps for the weak type resolution are not performed,
 * and all levels are set to the paragraph level.
 *
 * If there are no explicit embedding codes, then the (Xn) steps
 * are not performed.
 *
 * If embedding levels are supplied as a parameter, then all
 * explicit embedding codes are ignored, and the (Xn) steps
 * are not performed.
 *
 * White Space types could get the level of the run they belong to,
 * and are checked with a test of (flags&MASK_EMBEDDING) to
 * consider if the paragraph direction should be considered in
 * the flags variable.
 *
 * If there are no White Space types in the paragraph, then
 * (L1) is not necessary in adjustWSLevels().
 */

/* to avoid some conditional statements, use tiny constant arrays */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Flags flagLR[2]={ DIRPROP_FLAG(L), DIRPROP_FLAG(R) };
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Flags flagE[2]={ DIRPROP_FLAG(LRE), DIRPROP_FLAG(RLE) };
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Flags flagO[2]={ DIRPROP_FLAG(LRO), DIRPROP_FLAG(RLO) };

#ವ್ಯಾಖ್ಯಾನಿಸು DIRPROP_FLAG_LR(level) flagLR[(level)&1]
#ವ್ಯಾಖ್ಯಾನಿಸು DIRPROP_FLAG_E(level)  flagE[(level)&1]
#ವ್ಯಾಖ್ಯಾನಿಸು DIRPROP_FLAG_O(level)  flagO[(level)&1]

#ವ್ಯಾಖ್ಯಾನಿಸು DIR_FROM_STRONG(strong) ((strong)==L ? L : R)

#ವ್ಯಾಖ್ಯಾನಿಸು NO_OVERRIDE(level)  ((level)&~UBIDI_LEVEL_OVERRIDE)

/* UBiDi object management -------------------------------------------------- */

U_CAPI UBiDi * U_EXPORT2
ubidi_open()
{
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಹಿಂತಿರುಗಿ ubidi_openSized(0, 0, &error_ಸಂಕೇತ);
}

U_CAPI UBiDi * U_EXPORT2
ubidi_openSized(int32_t max_ಉದ್ದಳತೆ_2, int32_t max_ಸತತ_ಎಣಿಕೆ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UBiDi *pBiDi;

    /* check the argument values */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(max_ಉದ್ದಳತೆ_2<0 || max_ಸತತ_ಎಣಿಕೆ<0) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;    /* invalid arguments */
    }

    /* allocate memory for the object */
    pBiDi=(UBiDi *)uprv_malloc(ಗಾತ್ರ(UBiDi));
    ಆದರೆ(pBiDi==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* reset the object, all pointers nullptr, all flags false, all sizes 0 */
    uprv_memset(pBiDi, 0, ಗಾತ್ರ(UBiDi));

    /* allocate memory for arrays as requested */
    ಆದರೆ(max_ಉದ್ದಳತೆ_2>0) {
        ಆದರೆ( !get_Initial_Dir_Props_ಸ್ಮರಣೆ(pBiDi, max_ಉದ್ದಳತೆ_2) ||
            !get_Initial_ಮಟ್ಟಗಳು_ಸ್ಮರಣೆ(pBiDi, max_ಉದ್ದಳತೆ_2)
        ) {
            *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        pBiDi->may_Allocate_ಪಠ್ಯ=ನಿಜ;
    }

    ಆದರೆ(max_ಸತತ_ಎಣಿಕೆ>0) {
        ಆದರೆ(max_ಸತತ_ಎಣಿಕೆ==1) {
            /* use simpleRuns[] */
            pBiDi->runs_ಗಾತ್ರ=ಗಾತ್ರ(ಸತತ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!get_Initial_Runs_ಸ್ಮರಣೆ(pBiDi, max_ಸತತ_ಎಣಿಕೆ)) {
            *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        pBiDi->mayAllocateRuns=ನಿಜ;
    }

    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ pBiDi;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ubidi_close(pBiDi);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

/*
 * We are allowed to allocate memory if memory==nullptr or
 * mayAllocate==true for each array that we need.
 * We also try to grow memory as needed if we
 * allocate it.
 *
 * Assume sizeNeeded>0.
 * If *pMemory!=nullptr, then assume *pSize>0.
 *
 * ### this realloc() may unnecessarily copy the old data,
 * which we know we don't need any more;
 * is this the best way to do this??
 */
U_CFUNC UBool
ubidi_get_ಸ್ಮರಣೆ(Bidi_ಸ್ಮರಣೆ_For_ಹಂಚಿಕೆ *bidiMem, int32_t *p_ಗಾತ್ರ, UBool mayAllocate, size_t ಗಾತ್ರ_Needed) {
    ಆದರೆ (ಗಾತ್ರ_Needed > std::numeric_limits<int32_t>::max()) {
      // TODO(egg): Ugly guard for ICU-23397.  A cleaner fix would be to change the pSize to size_t*
      // and update callers throughout to use size_t for sizes (just like uprv_malloc already does).
      // Not that anyone should be running the UBA on text with that many runs…
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಶೂನ್ಯ **p_ಸ್ಮರಣೆ = (ಶೂನ್ಯ **)bidiMem;
    /* check for existing memory */
    ಆದರೆ(*p_ಸ್ಮರಣೆ==ಶೂನ್ಯಸೂಚಕ) {
        /* we need to allocate memory */
        ಆದರೆ(mayAllocate && (*p_ಸ್ಮರಣೆ=uprv_malloc(ಗಾತ್ರ_Needed))!=ಶೂನ್ಯಸೂಚಕ) {
            *p_ಗಾತ್ರ=static_cast<int32_t>(ಗಾತ್ರ_Needed);
            ಹಿಂತಿರುಗಿ ನಿಜ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(ಗಾತ್ರ_Needed<=static_cast<size_t>(*p_ಗಾತ್ರ)) {
            /* there is already enough memory */
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!mayAllocate) {
            /* not enough memory, and we must not allocate */
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* we try to grow */
            ಶೂನ್ಯ *ಸ್ಮರಣೆ;
            /* in most cases, we do not need the copy-old-data part of
             * realloc, but it is needed when adding runs using getRunsMemory()
             * in setParaRunsOnly()
             */
            ಆದರೆ((ಸ್ಮರಣೆ=uprv_realloc(*p_ಸ್ಮರಣೆ, ಗಾತ್ರ_Needed))!=ಶೂನ್ಯಸೂಚಕ) {
                *p_ಸ್ಮರಣೆ=ಸ್ಮರಣೆ;
                *p_ಗಾತ್ರ=static_cast<int32_t>(ಗಾತ್ರ_Needed);
                ಹಿಂತಿರುಗಿ ನಿಜ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* we failed to grow */
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        }
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_close(UBiDi *pBiDi) {
    ಆದರೆ(pBiDi!=ಶೂನ್ಯಸೂಚಕ) {
        pBiDi->pParaBiDi=ಶೂನ್ಯಸೂಚಕ;          /* in case one tries to reuse this block */
        ಆದರೆ(pBiDi->dir_Props_ಸ್ಮರಣೆ!=ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(pBiDi->dir_Props_ಸ್ಮರಣೆ);
        }
        ಆದರೆ(pBiDi->ಮಟ್ಟಗಳು_ಸ್ಮರಣೆ!=ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(pBiDi->ಮಟ್ಟಗಳು_ಸ್ಮರಣೆ);
        }
        ಆದರೆ(pBiDi->openings_ಸ್ಮರಣೆ!=ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(pBiDi->openings_ಸ್ಮರಣೆ);
        }
        ಆದರೆ(pBiDi->paras_ಸ್ಮರಣೆ!=ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(pBiDi->paras_ಸ್ಮರಣೆ);
        }
        ಆದರೆ(pBiDi->runs_ಸ್ಮರಣೆ!=ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(pBiDi->runs_ಸ್ಮರಣೆ);
        }
        ಆದರೆ(pBiDi->isolates_ಸ್ಮರಣೆ!=ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(pBiDi->isolates_ಸ್ಮರಣೆ);
        }
        ಆದರೆ(pBiDi->ಸೇರಿಸು_Points.points!=ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(pBiDi->ಸೇರಿಸು_Points.points);
        }

        uprv_free(pBiDi);
    }
}

/* set to approximate "inverse BiDi" ---------------------------------------- */

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_ಗಣ_Inverse(UBiDi *pBiDi, UBool isInverse) {
    ಆದರೆ(pBiDi!=ಶೂನ್ಯಸೂಚಕ) {
        pBiDi->isInverse=isInverse;
        pBiDi->reordering_ಬಹುಲಕ = isInverse ? UBIDI_REORDER_INVERSE_NUMBERS_AS_L
                                          : UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_isInverse(UBiDi *pBiDi) {
    ಆದರೆ(pBiDi!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ pBiDi->isInverse;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
}

/* FOOD FOR THOUGHT: currently the reordering modes are a mixture of
 * algorithm for direct BiDi, algorithm for inverse BiDi and the bizarre
 * concept of RUNS_ONLY which is a double operation.
 * It could be advantageous to divide this into 3 concepts:
 * a) Operation: direct / inverse / RUNS_ONLY
 * b) Direct algorithm: default / NUMBERS_SPECIAL / GROUP_NUMBERS_WITH_R
 * c) Inverse algorithm: default / INVERSE_LIKE_DIRECT / NUMBERS_SPECIAL
 * This would allow combinations not possible today like RUNS_ONLY with
 * NUMBERS_SPECIAL.
 * Also allow to set INSERT_MARKS for the direct step of RUNS_ONLY and
 * REMOVE_CONTROLS for the inverse step.
 * Not all combinations would be supported, and probably not all do make sense.
 * This would need to document which ones are supported and what are the
 * fallbacks for unsupported combinations.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_ಗಣ_Reordering_ಬಹುಲಕ(UBiDi *pBiDi, UBi_Di_Reordering_ಬಹುಲಕ reordering_ಬಹುಲಕ) UPRV_NO_SANITIZE_UNDEFINED {
    ಆದರೆ ((pBiDi!=ಶೂನ್ಯಸೂಚಕ) && (reordering_ಬಹುಲಕ >= UBIDI_REORDER_DEFAULT)
                        && (reordering_ಬಹುಲಕ < UBIDI_REORDER_ಎಣಿಕೆ)) {
        pBiDi->reordering_ಬಹುಲಕ = reordering_ಬಹುಲಕ;
        pBiDi->isInverse = reordering_ಬಹುಲಕ == UBIDI_REORDER_INVERSE_NUMBERS_AS_L;
    }
}

U_CAPI UBi_Di_Reordering_ಬಹುಲಕ U_EXPORT2
ubidi_get_Reordering_ಬಹುಲಕ(UBiDi *pBiDi) {
    ಆದರೆ (pBiDi!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ pBiDi->reordering_ಬಹುಲಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_ಗಣ_Reordering_Options(UBiDi *pBiDi, uint32_t reorderingOptions) {
    ಆದರೆ (reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        reorderingOptions&=~UBIDI_OPTION_ಸೇರಿಸು_MARKS;
    }
    ಆದರೆ (pBiDi!=ಶೂನ್ಯಸೂಚಕ) {
        pBiDi->reorderingOptions=reorderingOptions;
    }
}

U_CAPI uint32_t U_EXPORT2
ubidi_getReorderingOptions(UBiDi *pBiDi) {
    ಆದರೆ (pBiDi!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ pBiDi->reorderingOptions;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_ತಳ_Direction(ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ,
int32_t length){

    int32_t i;
    UChar32 uchar;
    UCharDirection dir;

    ಆದರೆ( ಪಠ್ಯ==ಶೂನ್ಯಸೂಚಕ || length<-1 ){
        ಹಿಂತಿರುಗಿ UBIDI_NEUTRAL;
    }

    ಆದರೆ(length==-1) {
        length=u_strlen(ಪಠ್ಯ);
    }

    ಗಾಗಿ( i = 0 ; i < length; ) {
        /* i is incremented by U16_NEXT */
        U16_ಮುಂದಿನ(ಪಠ್ಯ, i, length, uchar);
        dir = u_charDirection(uchar);
        ಆದರೆ( dir == U_ಎಡ_ಗಮ್ಯ_ಬಲ )
                ಹಿಂತಿರುಗಿ UBIDI_LTR;
        ಆದರೆ( dir == U_ಬಲ_ಗಮ್ಯ_ಎಡ || dir ==U_ಬಲ_ಗಮ್ಯ_ಎಡ_ARABIC )
                ಹಿಂತಿರುಗಿ UBIDI_RTL;
    }
    ಹಿಂತಿರುಗಿ UBIDI_NEUTRAL;
}

/* perform (P2)..(P3) ------------------------------------------------------- */

/**
 * Returns the directionality of the first strong character
 * after the last B in prologue, if any.
 * Requires prologue!=null.
 */
ಸ್ಥಿರ DirProp
ಮೊದಲ_L_R_AL(UBiDi *pBiDi) {
    ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ=pBiDi->prologue;
    int32_t length=pBiDi->pro_ಉದ್ದಳತೆ;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp, ಫಲಿತಾಂಶ=ON;
    ಗಾಗಿ(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_ಮುಂದಿನ(ಪಠ್ಯ, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        ಆದರೆ(ಫಲಿತಾಂಶ==ON) {
            ಆದರೆ(dirProp==L || dirProp==R || dirProp==AL) {
                ಫಲಿತಾಂಶ=dirProp;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(dirProp==B) {
                ಫಲಿತಾಂಶ=ON;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Check that there are enough entries in the array pointed to by pBiDi->paras
 */
ಸ್ಥಿರ UBool
check_Para_ಎಣಿಕೆ(UBiDi *pBiDi) {
    int32_t count=pBiDi->para_ಎಣಿಕೆ;
    ಆದರೆ(pBiDi->paras==pBiDi->simpleParas) {
        ಆದರೆ(count<=SIMPLE_PARAS_ಎಣಿಕೆ)
            ಹಿಂತಿರುಗಿ ನಿಜ;
        ಆದರೆ(!get_Initial_Paras_ಸ್ಮರಣೆ(pBiDi, SIMPLE_PARAS_ಎಣಿಕೆ * 2))
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        pBiDi->paras=pBiDi->paras_ಸ್ಮರಣೆ;
        uprv_memcpy(pBiDi->paras_ಸ್ಮರಣೆ, pBiDi->simpleParas, SIMPLE_PARAS_ಎಣಿಕೆ * ಗಾತ್ರ(Para));
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ(!get_Initial_Paras_ಸ್ಮರಣೆ(pBiDi, count * 2))
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    pBiDi->paras=pBiDi->paras_ಸ್ಮರಣೆ;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * Get the directional properties for the text, calculate the flags bit-set, and
 * determine the paragraph level if necessary (in pBiDi->paras[i].level).
 * FSI initiators are also resolved and their dirProp replaced with LRI or RLI.
 * When encountering an FSI, it is initially replaced with an LRI, which is the
 * default. Only if a strong R or AL is found within its scope will the LRI be
 * replaced by an RLI.
 */
ಸ್ಥಿರ UBool
getDirProps(UBiDi *pBiDi) {
    ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ=pBiDi->ಪಠ್ಯ;
    DirProp *dirProps=pBiDi->dir_Props_ಸ್ಮರಣೆ;    /* pBiDi->dirProps is const */

    int32_t i=0, original_ಉದ್ದಳತೆ=pBiDi->original_ಉದ್ದಳತೆ;
    Flags flags=0;      /* collect all directionalities in the text */
    UChar32 uchar;
    DirProp dirProp=0, defaultParaLevel=0;  /* initialize to avoid compiler warnings */
    UBool isDefaultLevel=IS_DEFAULT_LEVEL(pBiDi->paraLevel);
    /* for inverse BiDi, the default para level is set to RTL if there is a
       strong R or AL character at either end of the text                            */
    UBool isDefaultLevelInverse = isDefaultLevel && static_cast<UBool>(
            pBiDi->reordering_ಬಹುಲಕ == UBIDI_REORDER_INVERSE_LIKE_ನೇರ ||
            pBiDi->reordering_ಬಹುಲಕ == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL);
    int32_t ಕೊನೆಯ_Arabic_Pos=-1;
    int32_t control_ಎಣಿಕೆ=0;
    UBool removeBiDiControls =
        static_cast<UBool>(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS);

    ಎಣಿಕೆ State {
         NOT_SEEKING_STRONG,            /* 0: not contextual paraLevel, not after FSI */
         SEEKING_STRONG_FOR_PARA,       /* 1: looking for first strong char in para */
         SEEKING_STRONG_FOR_FSI,        /* 2: looking for first strong after FSI */
         LOOKING_FOR_PDI                /* 3: found strong after FSI, looking for PDI */
    };
    State state;
    DirProp ಕೊನೆಯ_Strong=ON;              /* for default level & inverse BiDi */
    /* The following stacks are used to manage isolate sequences. Those
       sequences may be nested, but obviously never more deeply than the
       maximum explicit embedding level.
       lastStack is the index of the last used entry in the stack. A value of -1
       means that there is no open isolate sequence.
       lastStack is reset to -1 on paragraph boundaries. */
    /* The following stack contains the position of the initiator of
       each open isolate sequence */
    int32_t isolate_ಆರಂಭ_ರಾಶಿ[UBIDI_MAX_EXPLICIT_LEVEL+1];
    /* The following stack contains the last known state before
       encountering the initiator of an isolate sequence */
    State  ಹಿಂದಿನ_State_ರಾಶಿ[UBIDI_MAX_EXPLICIT_LEVEL+1];
    int32_t ರಾಶಿ_ಕೊನೆಯ=-1;

    ಆದರೆ(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING)
        pBiDi->length=0;
    defaultParaLevel=pBiDi->paraLevel&1;
    ಆದರೆ(isDefaultLevel) {
        pBiDi->paras[0].level=defaultParaLevel;
        ಕೊನೆಯ_Strong=defaultParaLevel;
        ಆದರೆ(pBiDi->pro_ಉದ್ದಳತೆ>0 &&                    /* there is a prologue */
           (dirProp=ಮೊದಲ_L_R_AL(pBiDi))!=ON) {  /* with a strong character */
            ಆದರೆ(dirProp==L)
                pBiDi->paras[0].level=0;    /* set the default para level */
            ಇಲ್ಲದಿದ್ದರೆ
                pBiDi->paras[0].level=1;    /* set the default para level */
            state=NOT_SEEKING_STRONG;
        } ಇಲ್ಲದಿದ್ದರೆ {
            state=SEEKING_STRONG_FOR_PARA;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        pBiDi->paras[0].level=pBiDi->paraLevel;
        state=NOT_SEEKING_STRONG;
    }
    /* count paragraphs and determine the paragraph level (P2..P3) */
    /*
     * see comment in ubidi.h:
     * the UBIDI_DEFAULT_XXX values are designed so that
     * their bit 0 alone yields the intended default
     */
    ಗಾಗಿ( /* i=0 above */ ; i<original_ಉದ್ದಳತೆ; ) {
        /* i is incremented by U16_NEXT */
        U16_ಮುಂದಿನ(ಪಠ್ಯ, i, original_ಉದ್ದಳತೆ, uchar);
        flags|=DIRPROP_FLAG(dirProp=(DirProp)ubidi_getCustomizedClass(pBiDi, uchar));
        dirProps[i-1]=dirProp;
        ಆದರೆ(uchar>0xffff) {  /* set the lead surrogate's property to BN */
            flags|=DIRPROP_FLAG(BN);
            dirProps[i-2]=BN;
        }
        ಆದರೆ(removeBiDiControls && IS_BIDI_CONTROL_CHAR(uchar))
            control_ಎಣಿಕೆ++;
        ಆದರೆ(dirProp==L) {
            ಆದರೆ(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_ಎಣಿಕೆ-1].level=0;
                state=NOT_SEEKING_STRONG;
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(state==SEEKING_STRONG_FOR_FSI) {
                ಆದರೆ(ರಾಶಿ_ಕೊನೆಯ<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
                state=LOOKING_FOR_PDI;
            }
            ಕೊನೆಯ_Strong=L;
            ಮುಂದುವರಿ;
        }
        ಆದರೆ(dirProp==R || dirProp==AL) {
            ಆದರೆ(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_ಎಣಿಕೆ-1].level=1;
                state=NOT_SEEKING_STRONG;
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(state==SEEKING_STRONG_FOR_FSI) {
                ಆದರೆ(ರಾಶಿ_ಕೊನೆಯ<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    dirProps[isolate_ಆರಂಭ_ರಾಶಿ[ರಾಶಿ_ಕೊನೆಯ]]=RLI;
                    flags|=DIRPROP_FLAG(RLI);
                }
                state=LOOKING_FOR_PDI;
            }
            ಕೊನೆಯ_Strong=R;
            ಆದರೆ(dirProp==AL)
                ಕೊನೆಯ_Arabic_Pos=i-1;
            ಮುಂದುವರಿ;
        }
        ಆದರೆ(dirProp>=FSI && dirProp<=RLI) {  /* FSI, LRI or RLI */
            ರಾಶಿ_ಕೊನೆಯ++;
            ಆದರೆ(ರಾಶಿ_ಕೊನೆಯ<=UBIDI_MAX_EXPLICIT_LEVEL) {
                isolate_ಆರಂಭ_ರಾಶಿ[ರಾಶಿ_ಕೊನೆಯ]=i-1;
                ಹಿಂದಿನ_State_ರಾಶಿ[ರಾಶಿ_ಕೊನೆಯ]=state;
            }
            ಆದರೆ(dirProp==FSI) {
                dirProps[i-1]=LRI;      /* default if no strong char */
                state=SEEKING_STRONG_FOR_FSI;
            }
            ಇಲ್ಲದಿದ್ದರೆ
                state=LOOKING_FOR_PDI;
            ಮುಂದುವರಿ;
        }
        ಆದರೆ(dirProp==PDI) {
            ಆದರೆ(state==SEEKING_STRONG_FOR_FSI) {
                ಆದರೆ(ರಾಶಿ_ಕೊನೆಯ<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
            }
            ಆದರೆ(ರಾಶಿ_ಕೊನೆಯ>=0) {
                ಆದರೆ(ರಾಶಿ_ಕೊನೆಯ<=UBIDI_MAX_EXPLICIT_LEVEL)
                    state=ಹಿಂದಿನ_State_ರಾಶಿ[ರಾಶಿ_ಕೊನೆಯ];
                ರಾಶಿ_ಕೊನೆಯ--;
            }
            ಮುಂದುವರಿ;
        }
        ಆದರೆ(dirProp==B) {
            ಆದರೆ(i<original_ಉದ್ದಳತೆ && uchar==CR && ಪಠ್ಯ[i]==LF) /* do nothing on the CR */
                ಮುಂದುವರಿ;
            pBiDi->paras[pBiDi->para_ಎಣಿಕೆ-1].ಎಲ್ಲೆ=i;
            ಆದರೆ(isDefaultLevelInverse && ಕೊನೆಯ_Strong==R)
                pBiDi->paras[pBiDi->para_ಎಣಿಕೆ-1].level=1;
            ಆದರೆ(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
                /* When streaming, we only process whole paragraphs
                   thus some updates are only done on paragraph boundaries */
                pBiDi->length=i;        /* i is index to next character */
                pBiDi->control_ಎಣಿಕೆ=control_ಎಣಿಕೆ;
            }
            ಆದರೆ(i<original_ಉದ್ದಳತೆ) {              /* B not last char in text */
                pBiDi->para_ಎಣಿಕೆ++;
                ಆದರೆ(check_Para_ಎಣಿಕೆ(pBiDi)==ಸುಳ್ಳು)    /* not enough memory for a new para entry */
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                ಆದರೆ(isDefaultLevel) {
                    pBiDi->paras[pBiDi->para_ಎಣಿಕೆ-1].level=defaultParaLevel;
                    state=SEEKING_STRONG_FOR_PARA;
                    ಕೊನೆಯ_Strong=defaultParaLevel;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    pBiDi->paras[pBiDi->para_ಎಣಿಕೆ-1].level=pBiDi->paraLevel;
                    state=NOT_SEEKING_STRONG;
                }
                ರಾಶಿ_ಕೊನೆಯ=-1;
            }
            ಮುಂದುವರಿ;
        }
    }
    /* Ignore still open isolate sequences with overflow */
    ಆದರೆ(ರಾಶಿ_ಕೊನೆಯ>UBIDI_MAX_EXPLICIT_LEVEL) {
        ರಾಶಿ_ಕೊನೆಯ=UBIDI_MAX_EXPLICIT_LEVEL;
        state=SEEKING_STRONG_FOR_FSI;   /* to be on the safe side */
    }
    /* Resolve direction of still unresolved open FSI sequences */
    ಆಗ(ರಾಶಿ_ಕೊನೆಯ>=0) {
        ಆದರೆ(state==SEEKING_STRONG_FOR_FSI) {
            /* no need for next statement, already set by default */
            /* dirProps[isolateStartStack[stackLast]]=LRI; */
            flags|=DIRPROP_FLAG(LRI);
            ನಿಲ್ಲಿಸು;
        }
        state=ಹಿಂದಿನ_State_ರಾಶಿ[ರಾಶಿ_ಕೊನೆಯ];
        ರಾಶಿ_ಕೊನೆಯ--;
    }
    /* When streaming, ignore text after the last paragraph separator */
    ಆದರೆ(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
        ಆದರೆ(pBiDi->length<original_ಉದ್ದಳತೆ)
            pBiDi->para_ಎಣಿಕೆ--;
    } ಇಲ್ಲದಿದ್ದರೆ {
        pBiDi->paras[pBiDi->para_ಎಣಿಕೆ-1].ಎಲ್ಲೆ=original_ಉದ್ದಳತೆ;
        pBiDi->control_ಎಣಿಕೆ=control_ಎಣಿಕೆ;
    }
    /* For inverse bidi, default para direction is RTL if there is
       a strong R or AL at either end of the paragraph */
    ಆದರೆ(isDefaultLevelInverse && ಕೊನೆಯ_Strong==R) {
        pBiDi->paras[pBiDi->para_ಎಣಿಕೆ-1].level=1;
    }
    ಆದರೆ(isDefaultLevel) {
        pBiDi->paraLevel=static_cast<UBiDiLevel>(pBiDi->paras[0].level);
    }
    /* The following is needed to resolve the text direction for default level
       paragraphs containing no strong character */
    ಗಾಗಿ(i=0; i<pBiDi->para_ಎಣಿಕೆ; i++)
        flags|=DIRPROP_FLAG_LR(pBiDi->paras[i].level);

    ಆದರೆ(pBiDi->ಕ್ರಮ_Paragraphs_LTR && (flags&DIRPROP_FLAG(B))) {
        flags|=DIRPROP_FLAG(L);
    }
    pBiDi->flags=flags;
    pBiDi->ಕೊನೆಯ_Arabic_Pos=ಕೊನೆಯ_Arabic_Pos;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* determine the paragraph level at position index */
U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UBiDi *pBiDi, int32_t pindex) {
    int32_t i;
    ಗಾಗಿ(i=0; i<pBiDi->para_ಎಣಿಕೆ; i++)
        ಆದರೆ(pindex<pBiDi->paras[i].ಎಲ್ಲೆ)
            ನಿಲ್ಲಿಸು;
    ಆದರೆ(i>=pBiDi->para_ಎಣಿಕೆ)
        i=pBiDi->para_ಎಣಿಕೆ-1;
    ಹಿಂತಿರುಗಿ (UBiDiLevel)(pBiDi->paras[i].level);
}

/* Functions for handling paired brackets ----------------------------------- */

/* In the isoRuns array, the first entry is used for text outside of any
   isolate sequence.  Higher entries are used for each more deeply nested
   isolate sequence. isoRunLast is the index of the last used entry.  The
   openings array is used to note the data of opening brackets not yet
   matched by a closing bracket, or matched but still susceptible to change
   level.
   Each isoRun entry contains the index of the first and
   one-after-last openings entries for pending opening brackets it
   contains.  The next openings entry to use is the one-after-last of the
   most deeply nested isoRun entry.
   isoRun entries also contain their current embedding level and the last
   encountered strong character, since these will be needed to resolve
   the level of paired brackets.  */

ಸ್ಥಿರ ಶೂನ್ಯ
bracketInit(UBiDi *pBiDi, Bracket_ದತ್ತಾಂಶ *bd) {
    bd->pBiDi=pBiDi;
    bd->iso_ಸತತ_ಕೊನೆಯ=0;
    bd->isoRuns[0].ಆರಂಭ=0;
    bd->isoRuns[0].ಎಲ್ಲೆ=0;
    bd->isoRuns[0].level=GET_PARALEVEL(pBiDi, 0);
    UBiDiLevel t = GET_PARALEVEL(pBiDi, 0) & 1;
    bd->isoRuns[0].ಕೊನೆಯ_Strong = bd->isoRuns[0].ಕೊನೆಯ_ತಳ = t;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(t);
    bd->isoRuns[0].contextPos=0;
    ಆದರೆ(pBiDi->openings_ಸ್ಮರಣೆ) {
        bd->openings=pBiDi->openings_ಸ್ಮರಣೆ;
        bd->openings_ಎಣಿಕೆ=pBiDi->openings_ಗಾತ್ರ / ಗಾತ್ರ(Opening);
    } ಇಲ್ಲದಿದ್ದರೆ {
        bd->openings=bd->simpleOpenings;
        bd->openings_ಎಣಿಕೆ=SIMPLE_OPENINGS_ಎಣಿಕೆ;
    }
    bd->isNumbersSpecial=bd->pBiDi->reordering_ಬಹುಲಕ==UBIDI_REORDER_NUMBERS_SPECIAL ||
                         bd->pBiDi->reordering_ಬಹುಲಕ==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL;
}

/* paragraph boundary */
ಸ್ಥಿರ ಶೂನ್ಯ
bracketProcessB(Bracket_ದತ್ತಾಂಶ *bd, UBiDiLevel level) {
    bd->iso_ಸತತ_ಕೊನೆಯ=0;
    bd->isoRuns[0].ಎಲ್ಲೆ=0;
    bd->isoRuns[0].level=level;
    bd->isoRuns[0].ಕೊನೆಯ_Strong=bd->isoRuns[0].ಕೊನೆಯ_ತಳ=level&1;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(level & 1);
    bd->isoRuns[0].contextPos=0;
}

/* LRE, LRO, RLE, RLO, PDF */
ಸ್ಥಿರ ಶೂನ್ಯ
bracketProcessBoundary(Bracket_ದತ್ತಾಂಶ *bd, int32_t ಕೊನೆಯ_Cc_Pos,
                       UBiDiLevel contextLevel, UBiDiLevel embeddingLevel) {
    Iso_ಸತತ *p_ಕೊನೆಯ_Iso_ಸತತ=&bd->isoRuns[bd->iso_ಸತತ_ಕೊನೆಯ];
    DirProp *dirProps=bd->pBiDi->dirProps;
    ಆದರೆ(DIRPROP_FLAG(dirProps[ಕೊನೆಯ_Cc_Pos])&ಮುಖವಾಡ_ISO)  /* after an isolate */
        ಹಿಂತಿರುಗಿ;
    ಆದರೆ(NO_OVERRIDE(embeddingLevel)>NO_OVERRIDE(contextLevel))   /* not a PDF */
        contextLevel=embeddingLevel;
    p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ=p_ಕೊನೆಯ_Iso_ಸತತ->ಆರಂಭ;
    p_ಕೊನೆಯ_Iso_ಸತತ->level=embeddingLevel;
    p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_Strong=p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_ತಳ=contextLevel&1;
    p_ಕೊನೆಯ_Iso_ಸತತ->contextDir = static_cast<UBiDiDirection>(contextLevel & 1);
    p_ಕೊನೆಯ_Iso_ಸತತ->contextPos = static_cast<UBiDiDirection>(ಕೊನೆಯ_Cc_Pos);
}

/* LRI or RLI */
ಸ್ಥಿರ ಶೂನ್ಯ
bracketProcessLRI_RLI(Bracket_ದತ್ತಾಂಶ *bd, UBiDiLevel level) {
    Iso_ಸತತ *p_ಕೊನೆಯ_Iso_ಸತತ=&bd->isoRuns[bd->iso_ಸತತ_ಕೊನೆಯ];
    int16_t ಕೊನೆಯ_ಎಲ್ಲೆ;
    p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_ತಳ=ON;
    ಕೊನೆಯ_ಎಲ್ಲೆ=p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ;
    bd->iso_ಸತತ_ಕೊನೆಯ++;
    p_ಕೊನೆಯ_Iso_ಸತತ++;
    p_ಕೊನೆಯ_Iso_ಸತತ->ಆರಂಭ=p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ=ಕೊನೆಯ_ಎಲ್ಲೆ;
    p_ಕೊನೆಯ_Iso_ಸತತ->level=level;
    p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_Strong=p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_ತಳ=level&1;
    p_ಕೊನೆಯ_Iso_ಸತತ->contextDir = static_cast<UBiDiDirection>(level & 1);
    p_ಕೊನೆಯ_Iso_ಸತತ->contextPos=0;
}

/* PDI */
ಸ್ಥಿರ ಶೂನ್ಯ
bracketProcessPDI(Bracket_ದತ್ತಾಂಶ *bd) {
    Iso_ಸತತ *p_ಕೊನೆಯ_Iso_ಸತತ;
    bd->iso_ಸತತ_ಕೊನೆಯ--;
    p_ಕೊನೆಯ_Iso_ಸತತ=&bd->isoRuns[bd->iso_ಸತತ_ಕೊನೆಯ];
    p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_ತಳ=ON;
}

/* newly found opening bracket: create an openings entry */
ಸ್ಥಿರ UBool                            /* return true if success */
bracket_ಕೂಡಿಸು_Opening(Bracket_ದತ್ತಾಂಶ *bd, char16_t ಹೊಂದಿಕೆ, int32_t position) {
    Iso_ಸತತ *p_ಕೊನೆಯ_Iso_ಸತತ=&bd->isoRuns[bd->iso_ಸತತ_ಕೊನೆಯ];
    Opening *pOpening;
    ಆದರೆ(p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ>=bd->openings_ಎಣಿಕೆ) {  /* no available new entry */
        UBiDi *pBiDi=bd->pBiDi;
        ಆದರೆ(!get_Initial_Openings_ಸ್ಮರಣೆ(pBiDi, p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ * 2))
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        ಆದರೆ(bd->openings==bd->simpleOpenings)
            uprv_memcpy(pBiDi->openings_ಸ್ಮರಣೆ, bd->simpleOpenings,
                        SIMPLE_OPENINGS_ಎಣಿಕೆ * ಗಾತ್ರ(Opening));
        bd->openings=pBiDi->openings_ಸ್ಮರಣೆ;     /* may have changed */
        bd->openings_ಎಣಿಕೆ=pBiDi->openings_ಗಾತ್ರ / ಗಾತ್ರ(Opening);
    }
    pOpening=&bd->openings[p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ];
    pOpening->position=position;
    pOpening->ಹೊಂದಿಕೆ=ಹೊಂದಿಕೆ;
    pOpening->contextDir=p_ಕೊನೆಯ_Iso_ಸತತ->contextDir;
    pOpening->contextPos=p_ಕೊನೆಯ_Iso_ಸತತ->contextPos;
    pOpening->flags=0;
    p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ++;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* change N0c1 to N0c2 when a preceding bracket is assigned the embedding level */
ಸ್ಥಿರ ಶೂನ್ಯ
fixN0c(Bracket_ದತ್ತಾಂಶ *bd, int32_t opening_ಸೂಚ್ಯಂಕ, int32_t newPropPosition, DirProp newProp) {
    /* This function calls itself recursively */
    Iso_ಸತತ *p_ಕೊನೆಯ_Iso_ಸತತ=&bd->isoRuns[bd->iso_ಸತತ_ಕೊನೆಯ];
    Opening *qOpening;
    DirProp *dirProps=bd->pBiDi->dirProps;
    int32_t k, openingPosition, closingPosition;
    ಗಾಗಿ(k=opening_ಸೂಚ್ಯಂಕ+1, qOpening=&bd->openings[k]; k<p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ; k++, qOpening++) {
        ಆದರೆ(qOpening->ಹೊಂದಿಕೆ>=0)      /* not an N0c match */
            ಮುಂದುವರಿ;
        ಆದರೆ(newPropPosition<qOpening->contextPos)
            ನಿಲ್ಲಿಸು;
        ಆದರೆ(newPropPosition>=qOpening->position)
            ಮುಂದುವರಿ;
        ಆದರೆ(newProp==qOpening->contextDir)
            ನಿಲ್ಲಿಸು;
        openingPosition=qOpening->position;
        dirProps[openingPosition]=newProp;
        closingPosition=-(qOpening->ಹೊಂದಿಕೆ);
        dirProps[closingPosition]=newProp;
        qOpening->ಹೊಂದಿಕೆ=0;                      /* prevent further changes */
        fixN0c(bd, k, openingPosition, newProp);
        fixN0c(bd, k, closingPosition, newProp);
    }
}

/* process closing bracket */
ಸ್ಥಿರ DirProp              /* return L or R if N0b or N0c, ON if N0d */
bracketProcessClosing(Bracket_ದತ್ತಾಂಶ *bd, int32_t openIdx, int32_t position) {
    Iso_ಸತತ *p_ಕೊನೆಯ_Iso_ಸತತ=&bd->isoRuns[bd->iso_ಸತತ_ಕೊನೆಯ];
    Opening *pOpening, *qOpening;
    UBiDiDirection direction;
    UBool stable;
    DirProp newProp;
    pOpening=&bd->openings[openIdx];
    direction = static_cast<UBiDiDirection>(p_ಕೊನೆಯ_Iso_ಸತತ->level & 1);
    stable=ನಿಜ;            /* assume stable until proved otherwise */

    /* The stable flag is set when brackets are paired and their
       level is resolved and cannot be changed by what will be
       found later in the source string.
       An unstable match can occur only when applying N0c, where
       the resolved level depends on the preceding context, and
       this context may be affected by text occurring later.
       Example: RTL paragraph containing:  abc[(latin) HEBREW]
       When the closing parenthesis is encountered, it appears
       that N0c1 must be applied since 'abc' sets an opposite
       direction context and both parentheses receive level 2.
       However, when the closing square bracket is processed,
       N0b applies because of 'HEBREW' being included within the
       brackets, thus the square brackets are treated like R and
       receive level 1. However, this changes the preceding
       context of the opening parenthesis, and it now appears
       that N0c2 must be applied to the parentheses rather than
       N0c1. */

    ಆದರೆ((direction==0 && pOpening->flags&ಸಿಕ್ಕಿತು_L) ||
       (direction==1 && pOpening->flags&ಸಿಕ್ಕಿತು_R)) {                         /* N0b */
        newProp=static_cast<DirProp>(direction);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pOpening->flags&(ಸಿಕ್ಕಿತು_L|ಸಿಕ್ಕಿತು_R)) {                            /* N0c */
        /* it is stable if there is no containing pair or in
           conditions too complicated and not worth checking */
        stable=(openIdx==p_ಕೊನೆಯ_Iso_ಸತತ->ಆರಂಭ);
        ಆದರೆ(direction!=pOpening->contextDir)
            newProp= static_cast<DirProp>(pOpening->contextDir);           /* N0c1 */
        ಇಲ್ಲದಿದ್ದರೆ
            newProp= static_cast<DirProp>(direction);                      /* N0c2 */
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* forget this and any brackets nested within this pair */
        p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ= static_cast<uint16_t>(openIdx);
        ಹಿಂತಿರುಗಿ ON;                                                          /* N0d */
    }
    bd->pBiDi->dirProps[pOpening->position]=newProp;
    bd->pBiDi->dirProps[position]=newProp;
    /* Update nested N0c pairs that may be affected */
    fixN0c(bd, openIdx, pOpening->position, newProp);
    ಆದರೆ(stable) {
        p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ= static_cast<uint16_t>(openIdx); /* forget any brackets nested within this pair */
        /* remove lower located synonyms if any */
        ಆಗ(p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ>p_ಕೊನೆಯ_Iso_ಸತತ->ಆರಂಭ &&
              bd->openings[p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ-1].position==pOpening->position)
            p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ--;
    } ಇಲ್ಲದಿದ್ದರೆ {
        int32_t k;
        pOpening->ಹೊಂದಿಕೆ=-position;
        /* neutralize lower located synonyms if any */
        k=openIdx-1;
        ಆಗ(k>=p_ಕೊನೆಯ_Iso_ಸತತ->ಆರಂಭ &&
              bd->openings[k].position==pOpening->position)
            bd->openings[k--].ಹೊಂದಿಕೆ=0;
        /* neutralize any unmatched opening between the current pair;
           this will also neutralize higher located synonyms if any */
        ಗಾಗಿ(k=openIdx+1; k<p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ; k++) {
            qOpening=&bd->openings[k];
            ಆದರೆ(qOpening->position>=position)
                ನಿಲ್ಲಿಸು;
            ಆದರೆ(qOpening->ಹೊಂದಿಕೆ>0)
                qOpening->ಹೊಂದಿಕೆ=0;
        }
    }
    ಹಿಂತಿರುಗಿ newProp;
}

/* handle strong characters, digits and candidates for closing brackets */
ಸ್ಥಿರ UBool                            /* return true if success */
bracketProcessChar(Bracket_ದತ್ತಾಂಶ *bd, int32_t position) {
    Iso_ಸತತ *p_ಕೊನೆಯ_Iso_ಸತತ=&bd->isoRuns[bd->iso_ಸತತ_ಕೊನೆಯ];
    DirProp *dirProps, dirProp, newProp;
    UBiDiLevel level;
    dirProps=bd->pBiDi->dirProps;
    dirProp=dirProps[position];
    ಆದರೆ(dirProp==ON) {
        char16_t c, ಹೊಂದಿಕೆ;
        int32_t idx;
        /* First see if it is a matching closing bracket. Hopefully, this is
           more efficient than checking if it is a closing bracket at all */
        c=bd->pBiDi->ಪಠ್ಯ[position];
        ಗಾಗಿ(idx=p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ-1; idx>=p_ಕೊನೆಯ_Iso_ಸತತ->ಆರಂಭ; idx--) {
            ಆದರೆ(bd->openings[idx].ಹೊಂದಿಕೆ!=c)
                ಮುಂದುವರಿ;
            /* We have a match */
            newProp=bracketProcessClosing(bd, idx, position);
            ಆದರೆ(newProp==ON) {           /* N0d */
                c=0;        /* prevent handling as an opening */
                ನಿಲ್ಲಿಸು;
            }
            p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_ತಳ=ON;
            p_ಕೊನೆಯ_Iso_ಸತತ->contextDir = static_cast<UBiDiDirection>(newProp);
            p_ಕೊನೆಯ_Iso_ಸತತ->contextPos=position;
            level=bd->pBiDi->ಮಟ್ಟಗಳು[position];
            ಆದರೆ(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
                uint16_t flag;
                int32_t i;
                newProp=level&1;
                p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_Strong=newProp;
                flag=DIRPROP_FLAG(newProp);
                ಗಾಗಿ(i=p_ಕೊನೆಯ_Iso_ಸತತ->ಆರಂಭ; i<idx; i++)
                    bd->openings[i].flags|=flag;
                /* matching brackets are not overridden by LRO/RLO */
                bd->pBiDi->ಮಟ್ಟಗಳು[position]&=~UBIDI_LEVEL_OVERRIDE;
            }
            /* matching brackets are not overridden by LRO/RLO */
            bd->pBiDi->ಮಟ್ಟಗಳು[bd->openings[idx].position]&=~UBIDI_LEVEL_OVERRIDE;
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
        /* We get here only if the ON character is not a matching closing
           bracket or it is a case of N0d */
        /* Now see if it is an opening bracket */
        ಆದರೆ(c)
            ಹೊಂದಿಕೆ= static_cast<char16_t>(u_getBidiPairedBracket(c));    /* get the matching char */
        ಇಲ್ಲದಿದ್ದರೆ
            ಹೊಂದಿಕೆ=0;
        ಆದರೆ(ಹೊಂದಿಕೆ!=c &&                  /* has a matching char */
           ubidi_getPairedBracketType(c)==U_BPT_OPEN) { /* opening bracket */
            /* special case: process synonyms
               create an opening entry for each synonym */
            ಆದರೆ(ಹೊಂದಿಕೆ==0x232A) {     /* RIGHT-POINTING ANGLE BRACKET */
                ಆದರೆ(!bracket_ಕೂಡಿಸು_Opening(bd, 0x3009, position))
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹೊಂದಿಕೆ==0x3009) {         /* RIGHT ANGLE BRACKET */
                ಆದರೆ(!bracket_ಕೂಡಿಸು_Opening(bd, 0x232A, position))
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಆದರೆ(!bracket_ಕೂಡಿಸು_Opening(bd, ಹೊಂದಿಕೆ, position))
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    level=bd->pBiDi->ಮಟ್ಟಗಳು[position];
    ಆದರೆ(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
        newProp=level&1;
        ಆದರೆ(dirProp!=S && dirProp!=WS && dirProp!=ON)
            dirProps[position]=newProp;
        p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_ತಳ=newProp;
        p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_Strong=newProp;
        p_ಕೊನೆಯ_Iso_ಸತತ->contextDir = static_cast<UBiDiDirection>(newProp);
        p_ಕೊನೆಯ_Iso_ಸತತ->contextPos=position;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dirProp<=R || dirProp==AL) {
        newProp= static_cast<DirProp>(DIR_FROM_STRONG(dirProp));
        p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_ತಳ=dirProp;
        p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_Strong=dirProp;
        p_ಕೊನೆಯ_Iso_ಸತತ->contextDir = static_cast<UBiDiDirection>(newProp);
        p_ಕೊನೆಯ_Iso_ಸತತ->contextPos=position;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dirProp==EN) {
        p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_ತಳ=EN;
        ಆದರೆ(p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_Strong==L) {
            newProp=L;                  /* W7 */
            ಆದರೆ(!bd->isNumbersSpecial)
                dirProps[position]=ENL;
            p_ಕೊನೆಯ_Iso_ಸತತ->contextDir = static_cast<UBiDiDirection>(L);
            p_ಕೊನೆಯ_Iso_ಸತತ->contextPos=position;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            newProp=R;                  /* N0 */
            ಆದರೆ(p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_Strong==AL)
                dirProps[position]=AN;  /* W2 */
            ಇಲ್ಲದಿದ್ದರೆ
                dirProps[position]=ENR;
            p_ಕೊನೆಯ_Iso_ಸತತ->contextDir = static_cast<UBiDiDirection>(R);
            p_ಕೊನೆಯ_Iso_ಸತತ->contextPos=position;
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dirProp==AN) {
        newProp=R;                      /* N0 */
        p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_ತಳ=AN;
        p_ಕೊನೆಯ_Iso_ಸತತ->contextDir = static_cast<UBiDiDirection>(R);
        p_ಕೊನೆಯ_Iso_ಸತತ->contextPos=position;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dirProp==NSM) {
        /* if the last real char was ON, change NSM to ON so that it
           will stay ON even if the last real char is a bracket which
           may be changed to L or R */
        newProp=p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_ತಳ;
        ಆದರೆ(newProp==ON)
            dirProps[position]=newProp;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        newProp=dirProp;
        p_ಕೊನೆಯ_Iso_ಸತತ->ಕೊನೆಯ_ತಳ=dirProp;
    }
    ಆದರೆ(newProp<=R || newProp==AL) {
        int32_t i;
        uint16_t flag=DIRPROP_FLAG(DIR_FROM_STRONG(newProp));
        ಗಾಗಿ(i=p_ಕೊನೆಯ_Iso_ಸತತ->ಆರಂಭ; i<p_ಕೊನೆಯ_Iso_ಸತತ->ಎಲ್ಲೆ; i++)
            ಆದರೆ(position>bd->openings[i].position)
                bd->openings[i].flags|=flag;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/* perform (X1)..(X9) ------------------------------------------------------- */

/* determine if the text is mixed-directional or single-directional */
ಸ್ಥಿರ UBiDiDirection
directionFromFlags(UBiDi *pBiDi) {
    Flags flags=pBiDi->flags;
    /* if the text contains AN and neutrals, then some neutrals may become RTL */
    ಆದರೆ(!(flags&ಮುಖವಾಡ_RTL || ((flags&DIRPROP_FLAG(AN)) && (flags&ಮುಖವಾಡ_POSSIBLE_N)))) {
        ಹಿಂತಿರುಗಿ UBIDI_LTR;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!(flags&ಮುಖವಾಡ_LTR)) {
        ಹಿಂತಿರುಗಿ UBIDI_RTL;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ UBIDI_MIXED;
    }
}

/*
 * Resolve the explicit levels as specified by explicit embedding codes.
 * Recalculate the flags to have them reflect the real properties
 * after taking the explicit embeddings into account.
 *
 * The BiDi algorithm is designed to result in the same behavior whether embedding
 * levels are externally specified (from "styled text", supposedly the preferred
 * method) or set by explicit embedding codes (LRx, RLx, PDF, FSI, PDI) in the plain text.
 * That is why (X9) instructs to remove all not-isolate explicit codes (and BN).
 * However, in a real implementation, the removal of these codes and their index
 * positions in the plain text is undesirable since it would result in
 * reallocated, reindexed text.
 * Instead, this implementation leaves the codes in there and just ignores them
 * in the subsequent processing.
 * In order to get the same reordering behavior, positions with a BN or a not-isolate
 * explicit embedding code just get the same level assigned as the last "real"
 * character.
 *
 * Some implementations, not this one, then overwrite some of these
 * directionality properties at "real" same-level-run boundaries by
 * L or R codes so that the resolution of weak types can be performed on the
 * entire paragraph at once instead of having to parse it once more and
 * perform that resolution on same-level-runs.
 * This limits the scope of the implicit rules in effectively
 * the same way as the run limits.
 *
 * Instead, this implementation does not modify these codes, except for
 * paired brackets whose properties (ON) may be replaced by L or R.
 * On one hand, the paragraph has to be scanned for same-level-runs, but
 * on the other hand, this saves another loop to reset these codes,
 * or saves making and modifying a copy of dirProps[].
 *
 *
 * Note that (Pn) and (Xn) changed significantly from version 4 of the BiDi algorithm.
 *
 *
 * Handling the stack of explicit levels (Xn):
 *
 * With the BiDi stack of explicit levels, as pushed with each
 * LRE, RLE, LRO, RLO, LRI, RLI and FSI and popped with each PDF and PDI,
 * the explicit level must never exceed UBIDI_MAX_EXPLICIT_LEVEL.
 *
 * In order to have a correct push-pop semantics even in the case of overflows,
 * overflow counters and a valid isolate counter are used as described in UAX#9
 * section 3.3.2 "Explicit Levels and Directions".
 *
 * This implementation assumes that UBIDI_MAX_EXPLICIT_LEVEL is odd.
 *
 * Returns normally the direction; -1 if there was a memory shortage
 *
 */
ಸ್ಥಿರ UBiDiDirection
resolve_Explicit_ಮಟ್ಟಗಳು(UBiDi *pBiDi, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *ಮಟ್ಟಗಳು=pBiDi->ಮಟ್ಟಗಳು;
    ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ=pBiDi->ಪಠ್ಯ;

    int32_t i=0, length=pBiDi->length;
    Flags flags=pBiDi->flags;       /* collect all directionalities in the text */
    DirProp dirProp;
    UBiDiLevel level=GET_PARALEVEL(pBiDi, 0);
    UBiDiDirection direction;
    pBiDi->isolate_ಎಣಿಕೆ=0;

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ UBIDI_LTR; }

    /* determine if the text is mixed-directional or single-directional */
    direction=directionFromFlags(pBiDi);

    /* we may not need to resolve any explicit levels */
    ಆದರೆ((direction!=UBIDI_MIXED)) {
        /* not mixed directionality: levels don't matter - trailingWSStart will be 0 */
        ಹಿಂತಿರುಗಿ direction;
    }
    ಆದರೆ(pBiDi->reordering_ಬಹುಲಕ > UBIDI_REORDER_ಕೊನೆಯ_LOGICAL_ಗಮ್ಯ_VISUAL) {
        /* inverse BiDi: mixed, but all characters are at the same embedding level */
        /* set all levels to the paragraph level */
        int32_t para_ಸೂಚ್ಯಂಕ, ಆರಂಭ, ಎಲ್ಲೆ;
        ಗಾಗಿ(para_ಸೂಚ್ಯಂಕ=0; para_ಸೂಚ್ಯಂಕ<pBiDi->para_ಎಣಿಕೆ; para_ಸೂಚ್ಯಂಕ++) {
            ಆದರೆ(para_ಸೂಚ್ಯಂಕ==0)
                ಆರಂಭ=0;
            ಇಲ್ಲದಿದ್ದರೆ
                ಆರಂಭ=pBiDi->paras[para_ಸೂಚ್ಯಂಕ-1].ಎಲ್ಲೆ;
            ಎಲ್ಲೆ=pBiDi->paras[para_ಸೂಚ್ಯಂಕ].ಎಲ್ಲೆ;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_ಸೂಚ್ಯಂಕ].level);
            ಗಾಗಿ(i=ಆರಂಭ; i<ಎಲ್ಲೆ; i++)
                ಮಟ್ಟಗಳು[i]=level;
        }
        ಹಿಂತಿರುಗಿ direction;   /* no bracket matching for inverse BiDi */
    }
    ಆದರೆ(!(flags&(ಮುಖವಾಡ_EXPLICIT|ಮುಖವಾಡ_ISO))) {
        /* no embeddings, set all levels to the paragraph level */
        /* we still have to perform bracket matching */
        int32_t para_ಸೂಚ್ಯಂಕ, ಆರಂಭ, ಎಲ್ಲೆ;
        Bracket_ದತ್ತಾಂಶ bracket_ದತ್ತಾಂಶ;
        bracketInit(pBiDi, &bracket_ದತ್ತಾಂಶ);
        ಗಾಗಿ(para_ಸೂಚ್ಯಂಕ=0; para_ಸೂಚ್ಯಂಕ<pBiDi->para_ಎಣಿಕೆ; para_ಸೂಚ್ಯಂಕ++) {
            ಆದರೆ(para_ಸೂಚ್ಯಂಕ==0)
                ಆರಂಭ=0;
            ಇಲ್ಲದಿದ್ದರೆ
                ಆರಂಭ=pBiDi->paras[para_ಸೂಚ್ಯಂಕ-1].ಎಲ್ಲೆ;
            ಎಲ್ಲೆ=pBiDi->paras[para_ಸೂಚ್ಯಂಕ].ಎಲ್ಲೆ;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_ಸೂಚ್ಯಂಕ].level);
            ಗಾಗಿ(i=ಆರಂಭ; i<ಎಲ್ಲೆ; i++) {
                ಮಟ್ಟಗಳು[i]=level;
                dirProp=dirProps[i];
                ಆದರೆ(dirProp==BN)
                    ಮುಂದುವರಿ;
                ಆದರೆ(dirProp==B) {
                    ಆದರೆ((i+1)<length) {
                        ಆದರೆ(ಪಠ್ಯ[i]==CR && ಪಠ್ಯ[i+1]==LF)
                            ಮುಂದುವರಿ;   /* skip CR when followed by LF */
                        bracketProcessB(&bracket_ದತ್ತಾಂಶ, level);
                    }
                    ಮುಂದುವರಿ;
                }
                ಆದರೆ(!bracketProcessChar(&bracket_ದತ್ತಾಂಶ, i)) {
                    *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    ಹಿಂತಿರುಗಿ UBIDI_LTR;
                }
            }
        }
        ಹಿಂತಿರುಗಿ direction;
    }
    {
        /* continue to perform (Xn) */

        /* (X1) level is set for all codes, embeddingLevel keeps track of the push/pop operations */
        /* both variables may carry the UBIDI_LEVEL_OVERRIDE flag to indicate the override status */
        UBiDiLevel embeddingLevel=level, newLevel;
        UBiDiLevel ಹಿಂದಿನ_Level=level;     /* previous level for regular (not CC) characters */
        int32_t ಕೊನೆಯ_Cc_Pos=0;                /* index of last effective LRx,RLx, PDx */

        /* The following stack remembers the embedding level and the ISOLATE flag of level runs.
           stackLast points to its current entry. */
        uint16_t ರಾಶಿ_5[UBIDI_MAX_EXPLICIT_LEVEL+2];   /* we never push anything >=UBIDI_MAX_EXPLICIT_LEVEL
                                                        but we need one more entry as base */
        uint32_t ರಾಶಿ_ಕೊನೆಯ=0;
        int32_t ಅತಿಪ್ರವಾಹ_Isolate_ಎಣಿಕೆ=0;
        int32_t ಅತಿಪ್ರವಾಹ_Embedding_ಎಣಿಕೆ=0;
        int32_t valid_Isolate_ಎಣಿಕೆ=0;
        Bracket_ದತ್ತಾಂಶ bracket_ದತ್ತಾಂಶ;
        bracketInit(pBiDi, &bracket_ದತ್ತಾಂಶ);
        ರಾಶಿ_5[0]=level;     /* initialize base entry to para level, no override, no isolate */

        /* recalculate the flags */
        flags=0;

        ಗಾಗಿ(i=0; i<length; ++i) {
            dirProp=dirProps[i];
            ಆಯ್ಕೆ(dirProp) {
            ಸ್ಥಿತಿ LRE:
            ಸ್ಥಿತಿ RLE:
            ಸ್ಥಿತಿ LRO:
            ಸ್ಥಿತಿ RLO:
                /* (X2, X3, X4, X5) */
                flags|=DIRPROP_FLAG(BN);
                ಮಟ್ಟಗಳು[i]=ಹಿಂದಿನ_Level;
                ಆದರೆ (dirProp==LRE || dirProp==LRO)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                ಇಲ್ಲದಿದ್ದರೆ
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                ಆದರೆ(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && ಅತಿಪ್ರವಾಹ_Isolate_ಎಣಿಕೆ==0 &&
                                                         ಅತಿಪ್ರವಾಹ_Embedding_ಎಣಿಕೆ==0) {
                    ಕೊನೆಯ_Cc_Pos=i;
                    embeddingLevel=newLevel;
                    ಆದರೆ(dirProp==LRO || dirProp==RLO)
                        embeddingLevel|=UBIDI_LEVEL_OVERRIDE;
                    ರಾಶಿ_ಕೊನೆಯ++;
                    ರಾಶಿ_5[ರಾಶಿ_ಕೊನೆಯ]=embeddingLevel;
                    /* we don't need to set UBIDI_LEVEL_OVERRIDE off for LRE and RLE
                       since this has already been done for newLevel which is
                       the source for embeddingLevel.
                     */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ(ಅತಿಪ್ರವಾಹ_Isolate_ಎಣಿಕೆ==0)
                        ಅತಿಪ್ರವಾಹ_Embedding_ಎಣಿಕೆ++;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ PDF:
                /* (X7) */
                flags|=DIRPROP_FLAG(BN);
                ಮಟ್ಟಗಳು[i]=ಹಿಂದಿನ_Level;
                /* handle all the overflow cases first */
                ಆದರೆ(ಅತಿಪ್ರವಾಹ_Isolate_ಎಣಿಕೆ) {
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ(ಅತಿಪ್ರವಾಹ_Embedding_ಎಣಿಕೆ) {
                    ಅತಿಪ್ರವಾಹ_Embedding_ಎಣಿಕೆ--;
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ(ರಾಶಿ_ಕೊನೆಯ>0 && ರಾಶಿ_5[ರಾಶಿ_ಕೊನೆಯ]<ISOLATE) {   /* not an isolate entry */
                    ಕೊನೆಯ_Cc_Pos=i;
                    ರಾಶಿ_ಕೊನೆಯ--;
                    embeddingLevel = static_cast<UBiDiLevel>(ರಾಶಿ_5[ರಾಶಿ_ಕೊನೆಯ]);
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ LRI:
            ಸ್ಥಿತಿ RLI:
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                ಮಟ್ಟಗಳು[i]=NO_OVERRIDE(embeddingLevel);
                ಆದರೆ(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(ಹಿಂದಿನ_Level)) {
                    bracketProcessBoundary(&bracket_ದತ್ತಾಂಶ, ಕೊನೆಯ_Cc_Pos,
                                           ಹಿಂದಿನ_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                ಹಿಂದಿನ_Level=embeddingLevel;
                /* (X5a, X5b) */
                ಆದರೆ(dirProp==LRI)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                ಇಲ್ಲದಿದ್ದರೆ
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                ಆದರೆ(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && ಅತಿಪ್ರವಾಹ_Isolate_ಎಣಿಕೆ==0 &&
                                                         ಅತಿಪ್ರವಾಹ_Embedding_ಎಣಿಕೆ==0) {
                    flags|=DIRPROP_FLAG(dirProp);
                    ಕೊನೆಯ_Cc_Pos=i;
                    valid_Isolate_ಎಣಿಕೆ++;
                    ಆದರೆ(valid_Isolate_ಎಣಿಕೆ>pBiDi->isolate_ಎಣಿಕೆ)
                        pBiDi->isolate_ಎಣಿಕೆ=valid_Isolate_ಎಣಿಕೆ;
                    embeddingLevel=newLevel;
                    /* we can increment stackLast without checking because newLevel
                       will exceed UBIDI_MAX_EXPLICIT_LEVEL before stackLast overflows */
                    ರಾಶಿ_ಕೊನೆಯ++;
                    ರಾಶಿ_5[ರಾಶಿ_ಕೊನೆಯ]=embeddingLevel+ISOLATE;
                    bracketProcessLRI_RLI(&bracket_ದತ್ತಾಂಶ, embeddingLevel);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                    ಅತಿಪ್ರವಾಹ_Isolate_ಎಣಿಕೆ++;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ PDI:
                ಆದರೆ(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(ಹಿಂದಿನ_Level)) {
                    bracketProcessBoundary(&bracket_ದತ್ತಾಂಶ, ಕೊನೆಯ_Cc_Pos,
                                           ಹಿಂದಿನ_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                /* (X6a) */
                ಆದರೆ(ಅತಿಪ್ರವಾಹ_Isolate_ಎಣಿಕೆ) {
                    ಅತಿಪ್ರವಾಹ_Isolate_ಎಣಿಕೆ--;
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                }
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(valid_Isolate_ಎಣಿಕೆ) {
                    flags|=DIRPROP_FLAG(PDI);
                    ಕೊನೆಯ_Cc_Pos=i;
                    ಅತಿಪ್ರವಾಹ_Embedding_ಎಣಿಕೆ=0;
                    ಆಗ(ರಾಶಿ_5[ರಾಶಿ_ಕೊನೆಯ]<ISOLATE) /* pop embedding entries */
                        ರಾಶಿ_ಕೊನೆಯ--;                /* until the last isolate entry */
                    ರಾಶಿ_ಕೊನೆಯ--;                    /* pop also the last isolate entry */
                    valid_Isolate_ಎಣಿಕೆ--;
                    bracketProcessPDI(&bracket_ದತ್ತಾಂಶ);
                } ಇಲ್ಲದಿದ್ದರೆ
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                embeddingLevel = static_cast<UBiDiLevel>(ರಾಶಿ_5[ರಾಶಿ_ಕೊನೆಯ]) & ~ISOLATE;
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                ಹಿಂದಿನ_Level=embeddingLevel;
                ಮಟ್ಟಗಳು[i]=NO_OVERRIDE(embeddingLevel);
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ B:
                flags|=DIRPROP_FLAG(B);
                ಮಟ್ಟಗಳು[i]=GET_PARALEVEL(pBiDi, i);
                ಆದರೆ((i+1)<length) {
                    ಆದರೆ(ಪಠ್ಯ[i]==CR && ಪಠ್ಯ[i+1]==LF)
                        ನಿಲ್ಲಿಸು;          /* skip CR when followed by LF */
                    ಅತಿಪ್ರವಾಹ_Embedding_ಎಣಿಕೆ=ಅತಿಪ್ರವಾಹ_Isolate_ಎಣಿಕೆ=0;
                    valid_Isolate_ಎಣಿಕೆ=0;
                    ರಾಶಿ_ಕೊನೆಯ=0;
                    ಹಿಂದಿನ_Level=embeddingLevel=GET_PARALEVEL(pBiDi, i+1);
                    ರಾಶಿ_5[0]=embeddingLevel; /* initialize base entry to para level, no override, no isolate */
                    bracketProcessB(&bracket_ದತ್ತಾಂಶ, embeddingLevel);
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ BN:
                /* BN, LRE, RLE, and PDF are supposed to be removed (X9) */
                /* they will get their levels set correctly in adjustWSLevels() */
                ಮಟ್ಟಗಳು[i]=ಹಿಂದಿನ_Level;
                flags|=DIRPROP_FLAG(BN);
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                /* all other types are normal characters and get the "real" level */
                ಆದರೆ(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(ಹಿಂದಿನ_Level)) {
                    bracketProcessBoundary(&bracket_ದತ್ತಾಂಶ, ಕೊನೆಯ_Cc_Pos,
                                           ಹಿಂದಿನ_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                    ಆದರೆ(embeddingLevel&UBIDI_LEVEL_OVERRIDE)
                        flags|=DIRPROP_FLAG_O(embeddingLevel);
                    ಇಲ್ಲದಿದ್ದರೆ
                        flags|=DIRPROP_FLAG_E(embeddingLevel);
                }
                ಹಿಂದಿನ_Level=embeddingLevel;
                ಮಟ್ಟಗಳು[i]=embeddingLevel;
                ಆದರೆ(!bracketProcessChar(&bracket_ದತ್ತಾಂಶ, i))
                    ಹಿಂತಿರುಗಿ static_cast<UBiDiDirection>(-1);
                /* the dirProp may have been changed in bracketProcessChar() */
                flags|=DIRPROP_FLAG(dirProps[i]);
                ನಿಲ್ಲಿಸು;
            }
        }
        ಆದರೆ(flags&ಮುಖವಾಡ_EMBEDDING)
            flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
        ಆದರೆ(pBiDi->ಕ್ರಮ_Paragraphs_LTR && (flags&DIRPROP_FLAG(B)))
            flags|=DIRPROP_FLAG(L);
        /* again, determine if the text is mixed-directional or single-directional */
        pBiDi->flags=flags;
        direction=directionFromFlags(pBiDi);
    }
    ಹಿಂತಿರುಗಿ direction;
}

/*
 * Use a pre-specified embedding levels array:
 *
 * Adjust the directional properties for overrides (->LEVEL_OVERRIDE),
 * ignore all explicit codes (X9),
 * and check all the preset levels.
 *
 * Recalculate the flags to have them reflect the real properties
 * after taking the explicit embeddings into account.
 */
ಸ್ಥಿರ UBiDiDirection
check_Explicit_ಮಟ್ಟಗಳು(UBiDi *pBiDi, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *ಮಟ್ಟಗಳು=pBiDi->ಮಟ್ಟಗಳು;
    int32_t isolate_ಎಣಿಕೆ=0;

    int32_t length=pBiDi->length;
    Flags flags=0;  /* collect all directionalities in the text */
    pBiDi->isolate_ಎಣಿಕೆ=0;

    int32_t current_Para_ಸೂಚ್ಯಂಕ = 0;
    int32_t current_Para_ಎಲ್ಲೆ = pBiDi->paras[0].ಎಲ್ಲೆ;
    int32_t currentParaLevel = pBiDi->paraLevel;

    ಗಾಗಿ(int32_t i=0; i<length; ++i) {
        UBiDiLevel level=ಮಟ್ಟಗಳು[i];
        DirProp dirProp=dirProps[i];
        ಆದರೆ(dirProp==LRI || dirProp==RLI) {
            isolate_ಎಣಿಕೆ++;
            ಆದರೆ(isolate_ಎಣಿಕೆ>pBiDi->isolate_ಎಣಿಕೆ)
                pBiDi->isolate_ಎಣಿಕೆ=isolate_ಎಣಿಕೆ;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dirProp==PDI)
            isolate_ಎಣಿಕೆ--;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dirProp==B)
            isolate_ಎಣಿಕೆ=0;

        // optimized version of  int32_t currentParaLevel = GET_PARALEVEL(pBiDi, i);
        ಆದರೆ (pBiDi->defaultParaLevel != 0 &&
                i == current_Para_ಎಲ್ಲೆ && (current_Para_ಸೂಚ್ಯಂಕ + 1) < pBiDi->para_ಎಣಿಕೆ) {
            currentParaLevel = pBiDi->paras[++current_Para_ಸೂಚ್ಯಂಕ].level;
            current_Para_ಎಲ್ಲೆ = pBiDi->paras[current_Para_ಸೂಚ್ಯಂಕ].ಎಲ್ಲೆ;
        }

        UBiDiLevel overrideFlag = level & UBIDI_LEVEL_OVERRIDE;
        level &= ~UBIDI_LEVEL_OVERRIDE;
        ಆದರೆ (level < currentParaLevel || UBIDI_MAX_EXPLICIT_LEVEL < level) {
            ಆದರೆ (level == 0) {
                ಆದರೆ (dirProp == B) {
                    // Paragraph separators are ok with explicit level 0.
                    // Prevents reordering of paragraphs.
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // Treat explicit level 0 as a wildcard for the paragraph level.
                    // Avoid making the caller guess what the paragraph level would be.
                    level = static_cast<UBiDiLevel>(currentParaLevel);
                    ಮಟ್ಟಗಳು[i] = level | overrideFlag;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                // 1 <= level < currentParaLevel or UBIDI_MAX_EXPLICIT_LEVEL < level
                /* level out of bounds */
                *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
                ಹಿಂತಿರುಗಿ UBIDI_LTR;
            }
        }
        ಆದರೆ (overrideFlag != 0) {
            /* keep the override flag in levels[i] but adjust the flags */
            flags|=DIRPROP_FLAG_O(level);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* set the flags */
            flags|=DIRPROP_FLAG_E(level)|DIRPROP_FLAG(dirProp);
        }
    }
    ಆದರೆ(flags&ಮುಖವಾಡ_EMBEDDING)
        flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
    /* determine if the text is mixed-directional or single-directional */
    pBiDi->flags=flags;
    ಹಿಂತಿರುಗಿ directionFromFlags(pBiDi);
}

/******************************************************************
 The Properties state machine table
*******************************************************************

 All table cells are 8 bits:
      bits 0..4:  next state
      bits 5..7:  action to perform (if > 0)

 Cells may be of format "n" where n represents the next state
 (except for the rightmost column).
 Cells may also be of format "s(x,y)" where x represents an action
 to perform and y represents the next state.

*******************************************************************
 Definitions and type for properties state table
*******************************************************************
*/
#ವ್ಯಾಖ್ಯಾನಿಸು IMPTABPROPS_COLUMNS 16
#ವ್ಯಾಖ್ಯಾನಿಸು IMPTABPROPS_RES (IMPTABPROPS_COLUMNS - 1)
#ವ್ಯಾಖ್ಯಾನಿಸು GET_STATEPROPS(cell) ((cell)&0x1f)
#ವ್ಯಾಖ್ಯಾನಿಸು GET_ACTIONPROPS(cell) ((cell)>>5)
#ವ್ಯಾಖ್ಯಾನಿಸು s(action, newState) ((uint8_t)(newState+(action<<5)))

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t ಗುಂಪು_Prop[] =          /* dirProp regrouped */
{
/*  L   R   EN  ES  ET  AN  CS  B   S   WS  ON  LRE LRO AL  RLE RLO PDF NSM BN  FSI LRI RLI PDI ENL ENR */
    0,  1,  2,  7,  8,  3,  9,  6,  5,  4,  4,  10, 10, 12, 10, 10, 10, 11, 10, 4,  4,  4,  4,  13, 14
};
ಎಣಿಕೆ { DirProp_L=0, DirProp_R=1, DirProp_EN=2, DirProp_AN=3, DirProp_ON=4, DirProp_S=5, DirProp_B=6 }; /* reduced dirProp */

/******************************************************************

      PROPERTIES  STATE  TABLE

 In table impTabProps,
      - the ON column regroups ON and WS, FSI, RLI, LRI and PDI
      - the BN column regroups BN, LRE, RLE, LRO, RLO, PDF
      - the Res column is the reduced property assigned to a run

 Action 1: process current run1, init new run1
        2: init new run2
        3: process run1, process run2, init new run1
        4: process run1, set run1=run2, init new run2

 Notes:
  1) This table is used in resolveImplicitLevels().
  2) This table triggers actions when there is a change in the Bidi
     property of incoming characters (action 1).
  3) Most such property sequences are processed immediately (in
     fact, passed to processPropertySeq().
  4) However, numbers are assembled as one sequence. This means
     that undefined situations (like CS following digits, until
     it is known if the next char will be a digit) are held until
     following chars define them.
     Example: digits followed by CS, then comes another CS or ON;
              the digits will be processed, then the CS assigned
              as the start of an ON sequence (action 3).
  5) There are cases where more than one sequence must be
     processed, for instance digits followed by CS followed by L:
     the digits must be processed as one sequence, and the CS
     must be processed as an ON sequence, all this before starting
     assembling chars for the opening L sequence.


*/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t impTabProps[][IMPTABPROPS_COLUMNS] =
{
/*                        L ,     R ,    EN ,    AN ,    ON ,     S ,     B ,    ES ,    ET ,    CS ,    BN ,   NSM ,    AL ,   ENL ,   ENR , Res */
/* 0 Init        */ {     1 ,     2 ,     4 ,     5 ,     7 ,    15 ,    17 ,     7 ,     9 ,     7 ,     0 ,     7 ,     3 ,    18 ,    21 , DirProp_ON },
/* 1 L           */ {     1 , s(1,2), s(1,4), s(1,5), s(1,7),s(1,15),s(1,17), s(1,7), s(1,9), s(1,7),     1 ,     1 , s(1,3),s(1,18),s(1,21),  DirProp_L },
/* 2 R           */ { s(1,1),     2 , s(1,4), s(1,5), s(1,7),s(1,15),s(1,17), s(1,7), s(1,9), s(1,7),     2 ,     2 , s(1,3),s(1,18),s(1,21),  DirProp_R },
/* 3 AL          */ { s(1,1), s(1,2), s(1,6), s(1,6), s(1,8),s(1,16),s(1,17), s(1,8), s(1,8), s(1,8),     3 ,     3 ,     3 ,s(1,18),s(1,21),  DirProp_R },
/* 4 EN          */ { s(1,1), s(1,2),     4 , s(1,5), s(1,7),s(1,15),s(1,17),s(2,10),    11 ,s(2,10),     4 ,     4 , s(1,3),    18 ,    21 , DirProp_EN },
/* 5 AN          */ { s(1,1), s(1,2), s(1,4),     5 , s(1,7),s(1,15),s(1,17), s(1,7), s(1,9),s(2,12),     5 ,     5 , s(1,3),s(1,18),s(1,21), DirProp_AN },
/* 6 AL:EN/AN    */ { s(1,1), s(1,2),     6 ,     6 , s(1,8),s(1,16),s(1,17), s(1,8), s(1,8),s(2,13),     6 ,     6 , s(1,3),    18 ,    21 , DirProp_AN },
/* 7 ON          */ { s(1,1), s(1,2), s(1,4), s(1,5),     7 ,s(1,15),s(1,17),     7 ,s(2,14),     7 ,     7 ,     7 , s(1,3),s(1,18),s(1,21), DirProp_ON },
/* 8 AL:ON       */ { s(1,1), s(1,2), s(1,6), s(1,6),     8 ,s(1,16),s(1,17),     8 ,     8 ,     8 ,     8 ,     8 , s(1,3),s(1,18),s(1,21), DirProp_ON },
/* 9 ET          */ { s(1,1), s(1,2),     4 , s(1,5),     7 ,s(1,15),s(1,17),     7 ,     9 ,     7 ,     9 ,     9 , s(1,3),    18 ,    21 , DirProp_ON },
/*10 EN+ES/CS    */ { s(3,1), s(3,2),     4 , s(3,5), s(4,7),s(3,15),s(3,17), s(4,7),s(4,14), s(4,7),    10 , s(4,7), s(3,3),    18 ,    21 , DirProp_EN },
/*11 EN+ET       */ { s(1,1), s(1,2),     4 , s(1,5), s(1,7),s(1,15),s(1,17), s(1,7),    11 , s(1,7),    11 ,    11 , s(1,3),    18 ,    21 , DirProp_EN },
/*12 AN+CS       */ { s(3,1), s(3,2), s(3,4),     5 , s(4,7),s(3,15),s(3,17), s(4,7),s(4,14), s(4,7),    12 , s(4,7), s(3,3),s(3,18),s(3,21), DirProp_AN },
/*13 AL:EN/AN+CS */ { s(3,1), s(3,2),     6 ,     6 , s(4,8),s(3,16),s(3,17), s(4,8), s(4,8), s(4,8),    13 , s(4,8), s(3,3),    18 ,    21 , DirProp_AN },
/*14 ON+ET       */ { s(1,1), s(1,2), s(4,4), s(1,5),     7 ,s(1,15),s(1,17),     7 ,    14 ,     7 ,    14 ,    14 , s(1,3),s(4,18),s(4,21), DirProp_ON },
/*15 S           */ { s(1,1), s(1,2), s(1,4), s(1,5), s(1,7),    15 ,s(1,17), s(1,7), s(1,9), s(1,7),    15 , s(1,7), s(1,3),s(1,18),s(1,21),  DirProp_S },
/*16 AL:S        */ { s(1,1), s(1,2), s(1,6), s(1,6), s(1,8),    16 ,s(1,17), s(1,8), s(1,8), s(1,8),    16 , s(1,8), s(1,3),s(1,18),s(1,21),  DirProp_S },
/*17 B           */ { s(1,1), s(1,2), s(1,4), s(1,5), s(1,7),s(1,15),    17 , s(1,7), s(1,9), s(1,7),    17 , s(1,7), s(1,3),s(1,18),s(1,21),  DirProp_B },
/*18 ENL         */ { s(1,1), s(1,2),    18 , s(1,5), s(1,7),s(1,15),s(1,17),s(2,19),    20 ,s(2,19),    18 ,    18 , s(1,3),    18 ,    21 ,  DirProp_L },
/*19 ENL+ES/CS   */ { s(3,1), s(3,2),    18 , s(3,5), s(4,7),s(3,15),s(3,17), s(4,7),s(4,14), s(4,7),    19 , s(4,7), s(3,3),    18 ,    21 ,  DirProp_L },
/*20 ENL+ET      */ { s(1,1), s(1,2),    18 , s(1,5), s(1,7),s(1,15),s(1,17), s(1,7),    20 , s(1,7),    20 ,    20 , s(1,3),    18 ,    21 ,  DirProp_L },
/*21 ENR         */ { s(1,1), s(1,2),    21 , s(1,5), s(1,7),s(1,15),s(1,17),s(2,22),    23 ,s(2,22),    21 ,    21 , s(1,3),    18 ,    21 , DirProp_AN },
/*22 ENR+ES/CS   */ { s(3,1), s(3,2),    21 , s(3,5), s(4,7),s(3,15),s(3,17), s(4,7),s(4,14), s(4,7),    22 , s(4,7), s(3,3),    18 ,    21 , DirProp_AN },
/*23 ENR+ET      */ { s(1,1), s(1,2),    21 , s(1,5), s(1,7),s(1,15),s(1,17), s(1,7),    23 , s(1,7),    23 ,    23 , s(1,3),    18 ,    21 , DirProp_AN }
};

/*  we must undef macro s because the levels tables have a different
 *  structure (4 bits for action and 4 bits for next state.
 */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು s

/******************************************************************
 The levels state machine tables
*******************************************************************

 All table cells are 8 bits:
      bits 0..3:  next state
      bits 4..7:  action to perform (if > 0)

 Cells may be of format "n" where n represents the next state
 (except for the rightmost column).
 Cells may also be of format "s(x,y)" where x represents an action
 to perform and y represents the next state.

 This format limits each table to 16 states each and to 15 actions.

*******************************************************************
 Definitions and type for levels state tables
*******************************************************************
*/
#ವ್ಯಾಖ್ಯಾನಿಸು IMPTABLEVELS_COLUMNS (DirProp_B + 2)
#ವ್ಯಾಖ್ಯಾನಿಸು IMPTABLEVELS_RES (IMPTABLEVELS_COLUMNS - 1)
#ವ್ಯಾಖ್ಯಾನಿಸು GET_STATE(cell) ((cell)&0x0f)
#ವ್ಯಾಖ್ಯಾನಿಸು GET_ACTION(cell) ((cell)>>4)
#ವ್ಯಾಖ್ಯಾನಿಸು s(action, newState) ((uint8_t)(newState+(action<<4)))

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint8_t ImpTab[][IMPTABLEVELS_COLUMNS];
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint8_t ImpAct[];

/* FOOD FOR THOUGHT: each ImpTab should have its associated ImpAct,
 * instead of having a pair of ImpTab and a pair of ImpAct.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ImpTabPair {
    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ * pImpTab[2];
    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ * pImpAct[2];
} ImpTabPair;

/******************************************************************

      LEVELS  STATE  TABLES

 In all levels state tables,
      - state 0 is the initial state
      - the Res column is the increment to add to the text level
        for this property sequence.

 The impAct arrays for each table of a pair map the local action
 numbers of the table to the total list of actions. For instance,
 action 2 in a given table corresponds to the action number which
 appears in entry [2] of the impAct array for that table.
 The first entry of all impAct arrays must be 0.

 Action 1: init conditional sequence
        2: prepend conditional sequence to current sequence
        3: set ON sequence to new level - 1
        4: init EN/AN/ON sequence
        5: fix EN/AN/ON sequence followed by R
        6: set previous level sequence to level 2

 Notes:
  1) These tables are used in processPropertySeq(). The input
     is property sequences as determined by resolveImplicitLevels.
  2) Most such property sequences are processed immediately
     (levels are assigned).
  3) However, some sequences cannot be assigned a final level till
     one or more following sequences are received. For instance,
     ON following an R sequence within an even-level paragraph.
     If the following sequence is R, the ON sequence will be
     assigned basic run level+1, and so will the R sequence.
  4) S is generally handled like ON, since its level will be fixed
     to paragraph level in adjustWSLevels().

*/

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTab impTabL_DEFAULT =   /* Even paragraph level */
/*  In this table, conditional sequences receive the lower possible level
    until proven otherwise.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     0 ,     1 ,     0 ,     2 ,     0 ,     0 ,     0 ,  0 },
/* 1 : R          */ {     0 ,     1 ,     3 ,     3 , s(1,4), s(1,4),     0 ,  1 },
/* 2 : AN         */ {     0 ,     1 ,     0 ,     2 , s(1,5), s(1,5),     0 ,  2 },
/* 3 : R+EN/AN    */ {     0 ,     1 ,     3 ,     3 , s(1,4), s(1,4),     0 ,  2 },
/* 4 : R+ON       */ {     0 , s(2,1), s(3,3), s(3,3),     4 ,     4 ,     0 ,  0 },
/* 5 : AN+ON      */ {     0 , s(2,1),     0 , s(3,2),     5 ,     5 ,     0 ,  0 }
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTab impTabR_DEFAULT =   /* Odd  paragraph level */
/*  In this table, conditional sequences receive the lower possible level
    until proven otherwise.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     1 ,     0 ,     2 ,     2 ,     0 ,     0 ,     0 ,  0 },
/* 1 : L          */ {     1 ,     0 ,     1 ,     3 , s(1,4), s(1,4),     0 ,  1 },
/* 2 : EN/AN      */ {     1 ,     0 ,     2 ,     2 ,     0 ,     0 ,     0 ,  1 },
/* 3 : L+AN       */ {     1 ,     0 ,     1 ,     3 ,     5 ,     5 ,     0 ,  1 },
/* 4 : L+ON       */ { s(2,1),     0 , s(2,1),     3 ,     4 ,     4 ,     0 ,  0 },
/* 5 : L+AN+ON    */ {     1 ,     0 ,     1 ,     3 ,     5 ,     5 ,     0 ,  0 }
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpAct impAct0 = {0,1,2,3,4};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTabPair impTab_DEFAULT = {{&impTabL_DEFAULT,
                                           &impTabR_DEFAULT},
                                          {&impAct0, &impAct0}};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTab impTabL_NUMBERS_SPECIAL =   /* Even paragraph level */
/*  In this table, conditional sequences receive the lower possible level
    until proven otherwise.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     0 ,     2 , s(1,1), s(1,1),     0 ,     0 ,     0 ,  0 },
/* 1 : L+EN/AN    */ {     0 , s(4,2),     1 ,     1 ,     0 ,     0 ,     0 ,  0 },
/* 2 : R          */ {     0 ,     2 ,     4 ,     4 , s(1,3), s(1,3),     0 ,  1 },
/* 3 : R+ON       */ {     0 , s(2,2), s(3,4), s(3,4),     3 ,     3 ,     0 ,  0 },
/* 4 : R+EN/AN    */ {     0 ,     2 ,     4 ,     4 , s(1,3), s(1,3),     0 ,  2 }
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTabPair impTab_NUMBERS_SPECIAL = {{&impTabL_NUMBERS_SPECIAL,
                                                   &impTabR_DEFAULT},
                                                  {&impAct0, &impAct0}};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTab imp_Tab_L_ಗುಂಪು_NUMBERS_WITH_R =
/*  In this table, EN/AN+ON sequences receive levels as if associated with R
    until proven that there is L or sor/eor on both sides. AN is handled like EN.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 init         */ {     0 ,     3 , s(1,1), s(1,1),     0 ,     0 ,     0 ,  0 },
/* 1 EN/AN        */ { s(2,0),     3 ,     1 ,     1 ,     2 , s(2,0), s(2,0),  2 },
/* 2 EN/AN+ON     */ { s(2,0),     3 ,     1 ,     1 ,     2 , s(2,0), s(2,0),  1 },
/* 3 R            */ {     0 ,     3 ,     5 ,     5 , s(1,4),     0 ,     0 ,  1 },
/* 4 R+ON         */ { s(2,0),     3 ,     5 ,     5 ,     4 , s(2,0), s(2,0),  1 },
/* 5 R+EN/AN      */ {     0 ,     3 ,     5 ,     5 , s(1,4),     0 ,     0 ,  2 }
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTab imp_Tab_R_ಗುಂಪು_NUMBERS_WITH_R =
/*  In this table, EN/AN+ON sequences receive levels as if associated with R
    until proven that there is L on both sides. AN is handled like EN.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 init         */ {     2 ,     0 ,     1 ,     1 ,     0 ,     0 ,     0 ,  0 },
/* 1 EN/AN        */ {     2 ,     0 ,     1 ,     1 ,     0 ,     0 ,     0 ,  1 },
/* 2 L            */ {     2 ,     0 , s(1,4), s(1,4), s(1,3),     0 ,     0 ,  1 },
/* 3 L+ON         */ { s(2,2),     0 ,     4 ,     4 ,     3 ,     0 ,     0 ,  0 },
/* 4 L+EN/AN      */ { s(2,2),     0 ,     4 ,     4 ,     3 ,     0 ,     0 ,  1 }
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTabPair imp_Tab_ಗುಂಪು_NUMBERS_WITH_R = {
                        {&imp_Tab_L_ಗುಂಪು_NUMBERS_WITH_R,
                         &imp_Tab_R_ಗುಂಪು_NUMBERS_WITH_R},
                        {&impAct0, &impAct0}};


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTab impTabL_INVERSE_NUMBERS_AS_L =
/*  This table is identical to the Default LTR table except that EN and AN are
    handled like L.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     0 ,     1 ,     0 ,     0 ,     0 ,     0 ,     0 ,  0 },
/* 1 : R          */ {     0 ,     1 ,     0 ,     0 , s(1,4), s(1,4),     0 ,  1 },
/* 2 : AN         */ {     0 ,     1 ,     0 ,     0 , s(1,5), s(1,5),     0 ,  2 },
/* 3 : R+EN/AN    */ {     0 ,     1 ,     0 ,     0 , s(1,4), s(1,4),     0 ,  2 },
/* 4 : R+ON       */ { s(2,0),     1 , s(2,0), s(2,0),     4 ,     4 , s(2,0),  1 },
/* 5 : AN+ON      */ { s(2,0),     1 , s(2,0), s(2,0),     5 ,     5 , s(2,0),  1 }
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTab impTabR_INVERSE_NUMBERS_AS_L =
/*  This table is identical to the Default RTL table except that EN and AN are
    handled like L.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     1 ,     0 ,     1 ,     1 ,     0 ,     0 ,     0 ,  0 },
/* 1 : L          */ {     1 ,     0 ,     1 ,     1 , s(1,4), s(1,4),     0 ,  1 },
/* 2 : EN/AN      */ {     1 ,     0 ,     1 ,     1 ,     0 ,     0 ,     0 ,  1 },
/* 3 : L+AN       */ {     1 ,     0 ,     1 ,     1 ,     5 ,     5 ,     0 ,  1 },
/* 4 : L+ON       */ { s(2,1),     0 , s(2,1), s(2,1),     4 ,     4 ,     0 ,  0 },
/* 5 : L+AN+ON    */ {     1 ,     0 ,     1 ,     1 ,     5 ,     5 ,     0 ,  0 }
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTabPair impTab_INVERSE_NUMBERS_AS_L = {
                        {&impTabL_INVERSE_NUMBERS_AS_L,
                         &impTabR_INVERSE_NUMBERS_AS_L},
                        {&impAct0, &impAct0}};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTab imp_Tab_R_INVERSE_LIKE_ನೇರ =   /* Odd  paragraph level */
/*  In this table, conditional sequences receive the lower possible level
    until proven otherwise.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     1 ,     0 ,     2 ,     2 ,     0 ,     0 ,     0 ,  0 },
/* 1 : L          */ {     1 ,     0 ,     1 ,     2 , s(1,3), s(1,3),     0 ,  1 },
/* 2 : EN/AN      */ {     1 ,     0 ,     2 ,     2 ,     0 ,     0 ,     0 ,  1 },
/* 3 : L+ON       */ { s(2,1), s(3,0),     6 ,     4 ,     3 ,     3 , s(3,0),  0 },
/* 4 : L+ON+AN    */ { s(2,1), s(3,0),     6 ,     4 ,     5 ,     5 , s(3,0),  3 },
/* 5 : L+AN+ON    */ { s(2,1), s(3,0),     6 ,     4 ,     5 ,     5 , s(3,0),  2 },
/* 6 : L+ON+EN    */ { s(2,1), s(3,0),     6 ,     4 ,     3 ,     3 , s(3,0),  1 }
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpAct impAct1 = {0,1,13,14};
/* FOOD FOR THOUGHT: in LTR table below, check case "JKL 123abc"
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTabPair imp_Tab_INVERSE_LIKE_ನೇರ = {
                        {&impTabL_DEFAULT,
                         &imp_Tab_R_INVERSE_LIKE_ನೇರ},
                        {&impAct0, &impAct1}};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTab imp_Tab_L_INVERSE_LIKE_ನೇರ_WITH_MARKS =
/*  The case handled in this table is (visually):  R EN L
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     0 , s(6,3),     0 ,     1 ,     0 ,     0 ,     0 ,  0 },
/* 1 : L+AN       */ {     0 , s(6,3),     0 ,     1 , s(1,2), s(3,0),     0 ,  4 },
/* 2 : L+AN+ON    */ { s(2,0), s(6,3), s(2,0),     1 ,     2 , s(3,0), s(2,0),  3 },
/* 3 : R          */ {     0 , s(6,3), s(5,5), s(5,6), s(1,4), s(3,0),     0 ,  3 },
/* 4 : R+ON       */ { s(3,0), s(4,3), s(5,5), s(5,6),     4 , s(3,0), s(3,0),  3 },
/* 5 : R+EN       */ { s(3,0), s(4,3),     5 , s(5,6), s(1,4), s(3,0), s(3,0),  4 },
/* 6 : R+AN       */ { s(3,0), s(4,3), s(5,5),     6 , s(1,4), s(3,0), s(3,0),  4 }
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTab imp_Tab_R_INVERSE_LIKE_ನೇರ_WITH_MARKS =
/*  The cases handled in this table are (visually):  R EN L
                                                     R L AN L
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ { s(1,3),     0 ,     1 ,     1 ,     0 ,     0 ,     0 ,  0 },
/* 1 : R+EN/AN    */ { s(2,3),     0 ,     1 ,     1 ,     2 , s(4,0),     0 ,  1 },
/* 2 : R+EN/AN+ON */ { s(2,3),     0 ,     1 ,     1 ,     2 , s(4,0),     0 ,  0 },
/* 3 : L          */ {     3 ,     0 ,     3 , s(3,6), s(1,4), s(4,0),     0 ,  1 },
/* 4 : L+ON       */ { s(5,3), s(4,0),     5 , s(3,6),     4 , s(4,0), s(4,0),  0 },
/* 5 : L+ON+EN    */ { s(5,3), s(4,0),     5 , s(3,6),     4 , s(4,0), s(4,0),  1 },
/* 6 : L+AN       */ { s(5,3), s(4,0),     6 ,     6 ,     4 , s(4,0), s(4,0),  3 }
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpAct impAct2 = {0,1,2,5,6,7,8};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpAct impAct3 = {0,1,9,10,11,12};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTabPair imp_Tab_INVERSE_LIKE_ನೇರ_WITH_MARKS = {
                        {&imp_Tab_L_INVERSE_LIKE_ನೇರ_WITH_MARKS,
                         &imp_Tab_R_INVERSE_LIKE_ನೇರ_WITH_MARKS},
                        {&impAct2, &impAct3}};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL = {
                        {&impTabL_NUMBERS_SPECIAL,
                         &imp_Tab_R_INVERSE_LIKE_ನೇರ},
                        {&impAct0, &impAct1}};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTab impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS =
/*  The case handled in this table is (visually):  R EN L
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     0 , s(6,2),     1 ,     1 ,     0 ,     0 ,     0 ,  0 },
/* 1 : L+EN/AN    */ {     0 , s(6,2),     1 ,     1 ,     0 , s(3,0),     0 ,  4 },
/* 2 : R          */ {     0 , s(6,2), s(5,4), s(5,4), s(1,3), s(3,0),     0 ,  3 },
/* 3 : R+ON       */ { s(3,0), s(4,2), s(5,4), s(5,4),     3 , s(3,0), s(3,0),  3 },
/* 4 : R+EN/AN    */ { s(3,0), s(4,2),     4 ,     4 , s(1,3), s(3,0), s(3,0),  4 }
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS = {
                        {&impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS,
                         &imp_Tab_R_INVERSE_LIKE_ನೇರ_WITH_MARKS},
                        {&impAct2, &impAct3}};

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು s

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಸ್ಥಿರಾಂಕ ImpTab * pImpTab;             /* level table pointer          */
    ಸ್ಥಿರಾಂಕ ImpAct * pImpAct;             /* action map array             */
    int32_t ಆರಂಭ_ON;                    /* start of ON sequence         */
    int32_t ಆರಂಭ_L2_EN;                  /* start of level 2 sequence    */
    int32_t ಕೊನೆಯ_Strong_RTL;              /* index of last found R or AL  */
    int32_t state;                      /* current state                */
    int32_t ಸತತ_ಆರಂಭ;                   /* start position of the run    */
    UBiDiLevel ಸತತ_Level;                /* run level before implicit solving */
} LevState;

/*------------------------------------------------------------------------*/

ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_ಬಿಂದು(UBiDi *pBiDi, int32_t pos, int32_t flag)
  /* param pos:     position where to insert
     param flag:    one of LRM_BEFORE, LRM_AFTER, RLM_BEFORE, RLM_AFTER
  */
{
#ವ್ಯಾಖ್ಯಾನಿಸು FIRSTALLOC  10
    ಬಿಂದು ಬಿಂದು_2;
    ಸೇರಿಸು_Points_2 * p_ಸೇರಿಸು_Points=&(pBiDi->ಸೇರಿಸು_Points);

    ಆದರೆ (p_ಸೇರಿಸು_Points->capacity == 0)
    {
        p_ಸೇರಿಸು_Points->points=static_cast<ಬಿಂದು *>(uprv_malloc(ಗಾತ್ರ(ಬಿಂದು)*FIRSTALLOC));
        ಆದರೆ (p_ಸೇರಿಸು_Points->points == ಶೂನ್ಯಸೂಚಕ)
        {
            p_ಸೇರಿಸು_Points->error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        p_ಸೇರಿಸು_Points->capacity=FIRSTALLOC;
    }
    ಆದರೆ (p_ಸೇರಿಸು_Points->size >= p_ಸೇರಿಸು_Points->capacity) /* no room for new point */
    {
        ಬಿಂದು * savePoints=p_ಸೇರಿಸು_Points->points;
        p_ಸೇರಿಸು_Points->points=static_cast<ಬಿಂದು *>(uprv_realloc(p_ಸೇರಿಸು_Points->points,
                                           p_ಸೇರಿಸು_Points->capacity*2*ಗಾತ್ರ(ಬಿಂದು)));
        ಆದರೆ (p_ಸೇರಿಸು_Points->points == ಶೂನ್ಯಸೂಚಕ)
        {
            p_ಸೇರಿಸು_Points->points=savePoints;
            p_ಸೇರಿಸು_Points->error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        ಇಲ್ಲದಿದ್ದರೆ  p_ಸೇರಿಸು_Points->capacity*=2;
    }
    ಬಿಂದು_2.pos=pos;
    ಬಿಂದು_2.flag=flag;
    p_ಸೇರಿಸು_Points->points[p_ಸೇರಿಸು_Points->size]=ಬಿಂದು_2;
    p_ಸೇರಿಸು_Points->size++;
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು FIRSTALLOC
}

ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_ಮಟ್ಟಗಳು_Outside_Isolates(UBiDi *pBiDi, int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, UBiDiLevel level)
{
    DirProp *dirProps=pBiDi->dirProps, dirProp;
    UBiDiLevel *ಮಟ್ಟಗಳು=pBiDi->ಮಟ್ಟಗಳು;
    int32_t isolate_ಎಣಿಕೆ=0, k;
    ಗಾಗಿ(k=ಆರಂಭ; k<ಎಲ್ಲೆ; k++) {
        dirProp=dirProps[k];
        ಆದರೆ(dirProp==PDI)
            isolate_ಎಣಿಕೆ--;
        ಆದರೆ(isolate_ಎಣಿಕೆ==0)
            ಮಟ್ಟಗಳು[k]=level;
        ಆದರೆ(dirProp==LRI || dirProp==RLI)
            isolate_ಎಣಿಕೆ++;
    }
}

/* perform rules (Wn), (Nn), and (In) on a run of the text ------------------ */

/*
 * This implementation of the (Wn) rules applies all rules in one pass.
 * In order to do so, it needs a look-ahead of typically 1 character
 * (except for W5: sequences of ET) and keeps track of changes
 * in a rule Wp that affect a later Wq (p<q).
 *
 * The (Nn) and (In) rules are also performed in that same single loop,
 * but effectively one iteration behind for white space.
 *
 * Since all implicit rules are performed in one step, it is not necessary
 * to actually store the intermediate directional properties in dirProps[].
 */

ಸ್ಥಿರ ಶೂನ್ಯ
processPropertySeq(UBiDi *pBiDi, LevState *pLevState, uint8_t _prop,
                   int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ) {
    uint8_t cell, oldStateSeq, actionSeq;
    ಸ್ಥಿರಾಂಕ ImpTab * pImpTab=pLevState->pImpTab;
    ಸ್ಥಿರಾಂಕ ImpAct * pImpAct=pLevState->pImpAct;
    UBiDiLevel * ಮಟ್ಟಗಳು=pBiDi->ಮಟ್ಟಗಳು;
    UBiDiLevel level, ಕೂಡಿಸು_Level;
    ಸೇರಿಸು_Points_2 * p_ಸೇರಿಸು_Points;
    int32_t start0, k;

    start0=ಆರಂಭ;                           /* save original start position */
    oldStateSeq = static_cast<uint8_t>(pLevState->state);
    cell=(*pImpTab)[oldStateSeq][_prop];
    pLevState->state=GET_STATE(cell);       /* isolate the new state */
    actionSeq=(*pImpAct)[GET_ACTION(cell)]; /* isolate the action */
    ಕೂಡಿಸು_Level=(*pImpTab)[pLevState->state][IMPTABLEVELS_RES];

    ಆದರೆ(actionSeq) {
        ಆಯ್ಕೆ(actionSeq) {
        ಸ್ಥಿತಿ 1:                         /* init ON seq */
            pLevState->ಆರಂಭ_ON=start0;
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 2:                         /* prepend ON seq to current seq */
            ಆರಂಭ=pLevState->ಆರಂಭ_ON;
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 3:                         /* EN/AN after R+ON */
            level=pLevState->ಸತತ_Level+1;
            ಗಣ_ಮಟ್ಟಗಳು_Outside_Isolates(pBiDi, pLevState->ಆರಂಭ_ON, start0, level);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 4:                         /* EN/AN before R for NUMBERS_SPECIAL */
            level=pLevState->ಸತತ_Level+2;
            ಗಣ_ಮಟ್ಟಗಳು_Outside_Isolates(pBiDi, pLevState->ಆರಂಭ_ON, start0, level);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 5:                         /* L or S after possible relevant EN/AN */
            /* check if we had EN after R/AL */
            ಆದರೆ (pLevState->ಆರಂಭ_L2_EN >= 0) {
                ಕೂಡಿಸು_ಬಿಂದು(pBiDi, pLevState->ಆರಂಭ_L2_EN, LRM_BEFORE);
            }
            pLevState->ಆರಂಭ_L2_EN=-1;  /* not within previous if since could also be -2 */
            /* check if we had any relevant EN/AN after R/AL */
            p_ಸೇರಿಸು_Points=&(pBiDi->ಸೇರಿಸು_Points);
            ಆದರೆ ((p_ಸೇರಿಸು_Points->capacity == 0) ||
                (p_ಸೇರಿಸು_Points->size <= p_ಸೇರಿಸು_Points->confirmed))
            {
                /* nothing, just clean up */
                pLevState->ಕೊನೆಯ_Strong_RTL=-1;
                /* check if we have a pending conditional segment */
                level=(*pImpTab)[oldStateSeq][IMPTABLEVELS_RES];
                ಆದರೆ ((level & 1) && (pLevState->ಆರಂಭ_ON > 0)) {  /* after ON */
                    ಆರಂಭ=pLevState->ಆರಂಭ_ON;   /* reset to basic run level */
                }
                ಆದರೆ (_prop == DirProp_S)                /* add LRM before S */
                {
                    ಕೂಡಿಸು_ಬಿಂದು(pBiDi, start0, LRM_BEFORE);
                    p_ಸೇರಿಸು_Points->confirmed=p_ಸೇರಿಸು_Points->size;
                }
                ನಿಲ್ಲಿಸು;
            }
            /* reset previous RTL cont to level for LTR text */
            ಗಾಗಿ (k=pLevState->ಕೊನೆಯ_Strong_RTL+1; k<start0; k++)
            {
                /* reset odd level, leave runLevel+2 as is */
                ಮಟ್ಟಗಳು[k]=(ಮಟ್ಟಗಳು[k] - 2) & ~1;
            }
            /* mark insert points as confirmed */
            p_ಸೇರಿಸು_Points->confirmed=p_ಸೇರಿಸು_Points->size;
            pLevState->ಕೊನೆಯ_Strong_RTL=-1;
            ಆದರೆ (_prop == DirProp_S)            /* add LRM before S */
            {
                ಕೂಡಿಸು_ಬಿಂದು(pBiDi, start0, LRM_BEFORE);
                p_ಸೇರಿಸು_Points->confirmed=p_ಸೇರಿಸು_Points->size;
            }
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 6:                         /* R/AL after possible relevant EN/AN */
            /* just clean up */
            p_ಸೇರಿಸು_Points=&(pBiDi->ಸೇರಿಸು_Points);
            ಆದರೆ (p_ಸೇರಿಸು_Points->capacity > 0)
                /* remove all non confirmed insert points */
                p_ಸೇರಿಸು_Points->size=p_ಸೇರಿಸು_Points->confirmed;
            pLevState->ಆರಂಭ_ON=-1;
            pLevState->ಆರಂಭ_L2_EN=-1;
            pLevState->ಕೊನೆಯ_Strong_RTL=ಎಲ್ಲೆ - 1;
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 7:                         /* EN/AN after R/AL + possible cont */
            /* check for real AN */
            ಆದರೆ ((_prop == DirProp_AN) && (pBiDi->dirProps[start0] == AN) &&
                (pBiDi->reordering_ಬಹುಲಕ!=UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))
            {
                /* real AN */
                ಆದರೆ (pLevState->ಆರಂಭ_L2_EN == -1) /* if no relevant EN already found */
                {
                    /* just note the righmost digit as a strong RTL */
                    pLevState->ಕೊನೆಯ_Strong_RTL=ಎಲ್ಲೆ - 1;
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ (pLevState->ಆರಂಭ_L2_EN >= 0)  /* after EN, no AN */
                {
                    ಕೂಡಿಸು_ಬಿಂದು(pBiDi, pLevState->ಆರಂಭ_L2_EN, LRM_BEFORE);
                    pLevState->ಆರಂಭ_L2_EN=-2;
                }
                /* note AN */
                ಕೂಡಿಸು_ಬಿಂದು(pBiDi, start0, LRM_BEFORE);
                ನಿಲ್ಲಿಸು;
            }
            /* if first EN/AN after R/AL */
            ಆದರೆ (pLevState->ಆರಂಭ_L2_EN == -1) {
                pLevState->ಆರಂಭ_L2_EN=start0;
            }
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 8:                         /* note location of latest R/AL */
            pLevState->ಕೊನೆಯ_Strong_RTL=ಎಲ್ಲೆ - 1;
            pLevState->ಆರಂಭ_ON=-1;
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 9:                         /* L after R+ON/EN/AN */
            /* include possible adjacent number on the left */
            ಗಾಗಿ (k=start0-1; k>=0 && !(ಮಟ್ಟಗಳು[k]&1); k--);
            ಆದರೆ(k>=0) {
                ಕೂಡಿಸು_ಬಿಂದು(pBiDi, k, RLM_BEFORE);             /* add RLM before */
                p_ಸೇರಿಸು_Points=&(pBiDi->ಸೇರಿಸು_Points);
                p_ಸೇರಿಸು_Points->confirmed=p_ಸೇರಿಸು_Points->size;   /* confirm it */
            }
            pLevState->ಆರಂಭ_ON=start0;
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 10:                        /* AN after L */
            /* AN numbers between L text on both sides may be trouble. */
            /* tentatively bracket with LRMs; will be confirmed if followed by L */
            ಕೂಡಿಸು_ಬಿಂದು(pBiDi, start0, LRM_BEFORE);    /* add LRM before */
            ಕೂಡಿಸು_ಬಿಂದು(pBiDi, start0, LRM_AFTER);     /* add LRM after  */
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 11:                        /* R after L+ON/EN/AN */
            /* false alert, infirm LRMs around previous AN */
            p_ಸೇರಿಸು_Points=&(pBiDi->ಸೇರಿಸು_Points);
            p_ಸೇರಿಸು_Points->size=p_ಸೇರಿಸು_Points->confirmed;
            ಆದರೆ (_prop == DirProp_S)            /* add RLM before S */
            {
                ಕೂಡಿಸು_ಬಿಂದು(pBiDi, start0, RLM_BEFORE);
                p_ಸೇರಿಸು_Points->confirmed=p_ಸೇರಿಸು_Points->size;
            }
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 12:                        /* L after L+ON/AN */
            level=pLevState->ಸತತ_Level + ಕೂಡಿಸು_Level;
            ಗಾಗಿ(k=pLevState->ಆರಂಭ_ON; k<start0; k++) {
                ಆದರೆ (ಮಟ್ಟಗಳು[k]<level)
                    ಮಟ್ಟಗಳು[k]=level;
            }
            p_ಸೇರಿಸು_Points=&(pBiDi->ಸೇರಿಸು_Points);
            p_ಸೇರಿಸು_Points->confirmed=p_ಸೇರಿಸು_Points->size;   /* confirm inserts */
            pLevState->ಆರಂಭ_ON=start0;
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 13:                        /* L after L+ON+EN/AN/ON */
            level=pLevState->ಸತತ_Level;
            ಗಾಗಿ(k=start0-1; k>=pLevState->ಆರಂಭ_ON; k--) {
                ಆದರೆ(ಮಟ್ಟಗಳು[k]==level+3) {
                    ಆಗ(ಮಟ್ಟಗಳು[k]==level+3) {
                        ಮಟ್ಟಗಳು[k--]-=2;
                    }
                    ಆಗ(ಮಟ್ಟಗಳು[k]==level) {
                        k--;
                    }
                }
                ಆದರೆ(ಮಟ್ಟಗಳು[k]==level+2) {
                    ಮಟ್ಟಗಳು[k]=level;
                    ಮುಂದುವರಿ;
                }
                ಮಟ್ಟಗಳು[k]=level+1;
            }
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 14:                        /* R after L+ON+EN/AN/ON */
            level=pLevState->ಸತತ_Level+1;
            ಗಾಗಿ(k=start0-1; k>=pLevState->ಆರಂಭ_ON; k--) {
                ಆದರೆ(ಮಟ್ಟಗಳು[k]>level) {
                    ಮಟ್ಟಗಳು[k]-=2;
                }
            }
            ನಿಲ್ಲಿಸು;

        ಪೂರ್ವನಿಯೋಜಿತ:                        /* we should never get here */
            UPRV_UNREACHABLE_EXIT;
        }
    }
    ಆದರೆ((ಕೂಡಿಸು_Level) || (ಆರಂಭ < start0)) {
        level=pLevState->ಸತತ_Level + ಕೂಡಿಸು_Level;
        ಆದರೆ(ಆರಂಭ>=pLevState->ಸತತ_ಆರಂಭ) {
            ಗಾಗಿ(k=ಆರಂಭ; k<ಎಲ್ಲೆ; k++) {
                ಮಟ್ಟಗಳು[k]=level;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಣ_ಮಟ್ಟಗಳು_Outside_Isolates(pBiDi, ಆರಂಭ, ಎಲ್ಲೆ, level);
        }
    }
}

/**
 * Returns the directionality of the last strong character at the end of the prologue, if any.
 * Requires prologue!=null.
 */
ಸ್ಥಿರ DirProp
ಕೊನೆಯ_L_R_AL(UBiDi *pBiDi) {
    ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ=pBiDi->prologue;
    int32_t length=pBiDi->pro_ಉದ್ದಳತೆ;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    ಗಾಗಿ(i=length; i>0; ) {
        /* i is decremented by U16_PREV */
        U16_PREV(ಪಠ್ಯ, 0, i, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        ಆದರೆ(dirProp==L) {
            ಹಿಂತಿರುಗಿ DirProp_L;
        }
        ಆದರೆ(dirProp==R || dirProp==AL) {
            ಹಿಂತಿರುಗಿ DirProp_R;
        }
        ಆದರೆ(dirProp==B) {
            ಹಿಂತಿರುಗಿ DirProp_ON;
        }
    }
    ಹಿಂತಿರುಗಿ DirProp_ON;
}

/**
 * Returns the directionality of the first strong character, or digit, in the epilogue, if any.
 * Requires epilogue!=null.
 */
ಸ್ಥಿರ DirProp
ಮೊದಲ_L_R_AL_EN_AN(UBiDi *pBiDi) {
    ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ=pBiDi->epilogue;
    int32_t length=pBiDi->epi_ಉದ್ದಳತೆ;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    ಗಾಗಿ(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_ಮುಂದಿನ(ಪಠ್ಯ, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        ಆದರೆ(dirProp==L) {
            ಹಿಂತಿರುಗಿ DirProp_L;
        }
        ಆದರೆ(dirProp==R || dirProp==AL) {
            ಹಿಂತಿರುಗಿ DirProp_R;
        }
        ಆದರೆ(dirProp==EN) {
            ಹಿಂತಿರುಗಿ DirProp_EN;
        }
        ಆದರೆ(dirProp==AN) {
            ಹಿಂತಿರುಗಿ DirProp_AN;
        }
    }
    ಹಿಂತಿರುಗಿ DirProp_ON;
}

ಸ್ಥಿರ ಶೂನ್ಯ
resolve_Implicit_ಮಟ್ಟಗಳು(UBiDi *pBiDi,
                      int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ,
                      DirProp sor, DirProp eor) {
    ಸ್ಥಿರಾಂಕ DirProp *dirProps=pBiDi->dirProps;
    DirProp dirProp;
    LevState levState;
    int32_t i, start1, start2;
    uint16_t oldStateImp, stateImp, actionImp;
    uint8_t gprop, resProp, cell;
    UBool inverseRTL;
    DirProp ಮುಂದಿನ_Strong_Prop=R;
    int32_t ಮುಂದಿನ_Strong_Pos=-1;

    /* check for RTL inverse BiDi mode */
    /* FOOD FOR THOUGHT: in case of RTL inverse BiDi, it would make sense to
     * loop on the text characters from end to start.
     * This would need a different properties state table (at least different
     * actions) and different levels state tables (maybe very similar to the
     * LTR corresponding ones.
     */
    inverseRTL =
        static_cast<UBool>((ಆರಂಭ < pBiDi->ಕೊನೆಯ_Arabic_Pos) && (GET_PARALEVEL(pBiDi, ಆರಂಭ) & 1) &&
                           (pBiDi->reordering_ಬಹುಲಕ == UBIDI_REORDER_INVERSE_LIKE_ನೇರ ||
                            pBiDi->reordering_ಬಹುಲಕ == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL));

    /* initialize for property and levels state tables */
    levState.ಆರಂಭ_L2_EN=-1;              /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.ಕೊನೆಯ_Strong_RTL=-1;          /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.ಸತತ_ಆರಂಭ=ಆರಂಭ;
    levState.ಸತತ_Level=pBiDi->ಮಟ್ಟಗಳು[ಆರಂಭ];
    levState.pImpTab = static_cast<ಸ್ಥಿರಾಂಕ ImpTab*>(((pBiDi->pImpTabPair)->pImpTab)[levState.ಸತತ_Level & 1]);
    levState.pImpAct = static_cast<ಸ್ಥಿರಾಂಕ ImpAct*>(((pBiDi->pImpTabPair)->pImpAct)[levState.ಸತತ_Level & 1]);
    ಆದರೆ(ಆರಂಭ==0 && pBiDi->pro_ಉದ್ದಳತೆ>0) {
        DirProp ಕೊನೆಯ_Strong=ಕೊನೆಯ_L_R_AL(pBiDi);
        ಆದರೆ(ಕೊನೆಯ_Strong!=DirProp_ON) {
            sor=ಕೊನೆಯ_Strong;
        }
    }
    /* The isolates[] entries contain enough information to
       resume the bidi algorithm in the same state as it was
       when it was interrupted by an isolate sequence. */
    ಆದರೆ(dirProps[ಆರಂಭ]==PDI  && pBiDi->isolate_ಎಣಿಕೆ >= 0) {
        levState.ಆರಂಭ_ON=pBiDi->isolates[pBiDi->isolate_ಎಣಿಕೆ].ಆರಂಭ_ON;
        start1=pBiDi->isolates[pBiDi->isolate_ಎಣಿಕೆ].start1;
        stateImp=pBiDi->isolates[pBiDi->isolate_ಎಣಿಕೆ].stateImp;
        levState.state=pBiDi->isolates[pBiDi->isolate_ಎಣಿಕೆ].state;
        pBiDi->isolate_ಎಣಿಕೆ--;
    } ಇಲ್ಲದಿದ್ದರೆ {
        levState.ಆರಂಭ_ON=-1;
        start1=ಆರಂಭ;
        ಆದರೆ(dirProps[ಆರಂಭ]==NSM)
            stateImp = 1 + sor;
        ಇಲ್ಲದಿದ್ದರೆ
            stateImp=0;
        levState.state=0;
        processPropertySeq(pBiDi, &levState, sor, ಆರಂಭ, ಆರಂಭ);
    }
    start2=ಆರಂಭ;                       /* to make Java compiler happy */

    ಗಾಗಿ(i=ಆರಂಭ; i<=ಎಲ್ಲೆ; i++) {
        ಆದರೆ(i>=ಎಲ್ಲೆ) {
            int32_t k;
            ಗಾಗಿ(k=ಎಲ್ಲೆ-1; k>ಆರಂಭ&&(DIRPROP_FLAG(dirProps[k])&ಮುಖವಾಡ_BN_EXPLICIT); k--);
            dirProp=dirProps[k];
            ಆದರೆ(dirProp==LRI || dirProp==RLI)
                ನಿಲ್ಲಿಸು;      /* no forced closing for sequence ending with LRI/RLI */
            gprop=eor;
        } ಇಲ್ಲದಿದ್ದರೆ {
            DirProp prop, prop1;
            prop=dirProps[i];
            ಆದರೆ(prop==B) {
                pBiDi->isolate_ಎಣಿಕೆ=-1; /* current isolates stack entry == none */
            }
            ಆದರೆ(inverseRTL) {
                ಆದರೆ(prop==AL) {
                    /* AL before EN does not make it AN */
                    prop=R;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(prop==EN) {
                    ಆದರೆ(ಮುಂದಿನ_Strong_Pos<=i) {
                        /* look for next strong char (L/R/AL) */
                        int32_t j;
                        ಮುಂದಿನ_Strong_Prop=R;   /* set default */
                        ಮುಂದಿನ_Strong_Pos=ಎಲ್ಲೆ;
                        ಗಾಗಿ(j=i+1; j<ಎಲ್ಲೆ; j++) {
                            prop1=dirProps[j];
                            ಆದರೆ(prop1==L || prop1==R || prop1==AL) {
                                ಮುಂದಿನ_Strong_Prop=prop1;
                                ಮುಂದಿನ_Strong_Pos=j;
                                ನಿಲ್ಲಿಸು;
                            }
                        }
                    }
                    ಆದರೆ(ಮುಂದಿನ_Strong_Prop==AL) {
                        prop=AN;
                    }
                }
            }
            gprop=ಗುಂಪು_Prop[prop];
        }
        oldStateImp=stateImp;
        cell=impTabProps[oldStateImp][gprop];
        stateImp=GET_STATEPROPS(cell);      /* isolate the new state */
        actionImp=GET_ACTIONPROPS(cell);    /* isolate the action */
        ಆದರೆ((i==ಎಲ್ಲೆ) && (actionImp==0)) {
            /* there is an unprocessed sequence if its property == eor   */
            actionImp=1;                    /* process the last sequence */
        }
        ಆದರೆ(actionImp) {
            resProp=impTabProps[oldStateImp][IMPTABPROPS_RES];
            ಆಯ್ಕೆ(actionImp) {
            ಸ್ಥಿತಿ 1:             /* process current seq1, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, i);
                start1=i;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ 2:             /* init new seq2 */
                start2=i;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ 3:             /* process seq1, process seq2, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                processPropertySeq(pBiDi, &levState, DirProp_ON, start2, i);
                start1=i;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ 4:             /* process seq1, set seq1=seq2, init new seq2 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                start1=start2;
                start2=i;
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:            /* we should never get here */
                UPRV_UNREACHABLE_EXIT;
            }
        }
    }

    /* flush possible pending sequence, e.g. ON */
    ಆದರೆ(ಎಲ್ಲೆ==pBiDi->length && pBiDi->epi_ಉದ್ದಳತೆ>0) {
        DirProp ಮೊದಲ_Strong=ಮೊದಲ_L_R_AL_EN_AN(pBiDi);
        ಆದರೆ(ಮೊದಲ_Strong!=DirProp_ON) {
            eor=ಮೊದಲ_Strong;
        }
    }

    /* look for the last char not a BN or LRE/RLE/LRO/RLO/PDF */
    ಗಾಗಿ(i=ಎಲ್ಲೆ-1; i>ಆರಂಭ&&(DIRPROP_FLAG(dirProps[i])&ಮುಖವಾಡ_BN_EXPLICIT); i--);
    dirProp=dirProps[i];
    ಆದರೆ((dirProp==LRI || dirProp==RLI) && ಎಲ್ಲೆ<pBiDi->length) {
        pBiDi->isolate_ಎಣಿಕೆ++;
        pBiDi->isolates[pBiDi->isolate_ಎಣಿಕೆ].stateImp=stateImp;
        pBiDi->isolates[pBiDi->isolate_ಎಣಿಕೆ].state=levState.state;
        pBiDi->isolates[pBiDi->isolate_ಎಣಿಕೆ].start1=start1;
        pBiDi->isolates[pBiDi->isolate_ಎಣಿಕೆ].ಆರಂಭ_ON=levState.ಆರಂಭ_ON;
    }
    ಇಲ್ಲದಿದ್ದರೆ
        processPropertySeq(pBiDi, &levState, eor, ಎಲ್ಲೆ, ಎಲ್ಲೆ);
}

/* perform (L1) and (X9) ---------------------------------------------------- */

/*
 * Reset the embedding levels for some non-graphic characters (L1).
 * This function also sets appropriate levels for BN, and
 * explicit embedding types that are supposed to have been removed
 * from the paragraph in (X9).
 */
ಸ್ಥಿರ ಶೂನ್ಯ
adjustWSLevels(UBiDi *pBiDi) {
    ಸ್ಥಿರಾಂಕ DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *ಮಟ್ಟಗಳು=pBiDi->ಮಟ್ಟಗಳು;
    int32_t i;

    ಆದರೆ(pBiDi->flags&ಮುಖವಾಡ_WS) {
        UBool ಕ್ರಮ_Paragraphs_LTR=pBiDi->ಕ್ರಮ_Paragraphs_LTR;
        Flags flag;

        i=pBiDi->trailingWSStart;
        ಆಗ(i>0) {
            /* reset a sequence of WS/BN before eop and B/S to the paragraph paraLevel */
            ಆಗ(i>0 && (flag=DIRPROP_FLAG(dirProps[--i]))&ಮುಖವಾಡ_WS) {
                ಆದರೆ(ಕ್ರಮ_Paragraphs_LTR&&(flag&DIRPROP_FLAG(B))) {
                    ಮಟ್ಟಗಳು[i]=0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಮಟ್ಟಗಳು[i]=GET_PARALEVEL(pBiDi, i);
                }
            }

            /* reset BN to the next character's paraLevel until B/S, which restarts above loop */
            /* here, i+1 is guaranteed to be <length */
            ಆಗ(i>0) {
                flag=DIRPROP_FLAG(dirProps[--i]);
                ಆದರೆ(flag&ಮುಖವಾಡ_BN_EXPLICIT) {
                    ಮಟ್ಟಗಳು[i]=ಮಟ್ಟಗಳು[i+1];
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಕ್ರಮ_Paragraphs_LTR&&(flag&DIRPROP_FLAG(B))) {
                    ಮಟ್ಟಗಳು[i]=0;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flag&ಮುಖವಾಡ_B_S) {
                    ಮಟ್ಟಗಳು[i]=GET_PARALEVEL(pBiDi, i);
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_ಗಣ_Context(UBiDi *pBiDi,
                 ಸ್ಥಿರಾಂಕ char16_t *prologue, int32_t pro_ಉದ್ದಳತೆ,
                 ಸ್ಥಿರಾಂಕ char16_t *epilogue, int32_t epi_ಉದ್ದಳತೆ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ಸಂಕೇತ);
    ಆದರೆ(pBiDi==ಶೂನ್ಯಸೂಚಕ || pro_ಉದ್ದಳತೆ<-1 || epi_ಉದ್ದಳತೆ<-1 ||
       (prologue==ಶೂನ್ಯಸೂಚಕ && pro_ಉದ್ದಳತೆ!=0) || (epilogue==ಶೂನ್ಯಸೂಚಕ && epi_ಉದ್ದಳತೆ!=0)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(pro_ಉದ್ದಳತೆ==-1) {
        pBiDi->pro_ಉದ್ದಳತೆ=u_strlen(prologue);
    } ಇಲ್ಲದಿದ್ದರೆ {
        pBiDi->pro_ಉದ್ದಳತೆ=pro_ಉದ್ದಳತೆ;
    }
    ಆದರೆ(epi_ಉದ್ದಳತೆ==-1) {
        pBiDi->epi_ಉದ್ದಳತೆ=u_strlen(epilogue);
    } ಇಲ್ಲದಿದ್ದರೆ {
        pBiDi->epi_ಉದ್ದಳತೆ=epi_ಉದ್ದಳತೆ;
    }
    pBiDi->prologue=prologue;
    pBiDi->epilogue=epilogue;
}

ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_Para_Success(UBiDi *pBiDi) {
    pBiDi->pro_ಉದ್ದಳತೆ=0;                 /* forget the last context */
    pBiDi->epi_ಉದ್ದಳತೆ=0;
    pBiDi->pParaBiDi=pBiDi;             /* mark successful setPara */
}

#ವ್ಯಾಖ್ಯಾನಿಸು BIDI_MIN(x, y)   ((x)<(y) ? (x) : (y))
#ವ್ಯಾಖ್ಯಾನಿಸು BIDI_ABS(x)      ((x)>=0  ? (x) : (-(x)))

ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_Para_Runs_Only(UBiDi *pBiDi, ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ, int32_t length,
                UBiDiLevel paraLevel, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    int32_t *runs_Only_ಸ್ಮರಣೆ = ಶೂನ್ಯಸೂಚಕ;
    int32_t *visualMap;
    char16_t *visual_ಪಠ್ಯ;
    int32_t save_ಉದ್ದಳತೆ, saveTrailingWSStart;
    ಸ್ಥಿರಾಂಕ UBiDiLevel *ಮಟ್ಟಗಳು;
    UBiDiLevel *save_ಮಟ್ಟಗಳು;
    UBiDiDirection saveDirection;
    UBool save_May_Allocate_ಪಠ್ಯ;
    ಸತತ *runs;
    int32_t visual_ಉದ್ದಳತೆ, i, j, visual_ಆರಂಭ, logical_ಆರಂಭ,
            ಸತತ_ಎಣಿಕೆ, ಸತತ_ಉದ್ದಳತೆ, addedRuns, ಸೇರಿಸು_Remove,
            ಆರಂಭ, ಎಲ್ಲೆ, ಹೆಜ್ಜೆ, ಸೂಚ್ಯಂಕ_Odd_ಬಿಟ್, logicalPos,
            index0, index1;
    uint32_t saveOptions;

    pBiDi->reordering_ಬಹುಲಕ=UBIDI_REORDER_DEFAULT;
    ಆದರೆ(length==0) {
        ubidi_ಗಣ_Para(pBiDi, ಪಠ್ಯ, length, paraLevel, ಶೂನ್ಯಸೂಚಕ, p_Error_ಸಂಕೇತ);
        ಹೋಗು cleanup3;
    }
    /* obtain memory for mapping table and visual text */
    runs_Only_ಸ್ಮರಣೆ=static_cast<int32_t *>(uprv_malloc(length*(ಗಾತ್ರ(int32_t)+ಗಾತ್ರ(char16_t)+ಗಾತ್ರ(UBiDiLevel))));
    ಆದರೆ(runs_Only_ಸ್ಮರಣೆ==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹೋಗು cleanup3;
    }
    visualMap=runs_Only_ಸ್ಮರಣೆ;
    visual_ಪಠ್ಯ = reinterpret_cast<char16_t*>(&visualMap[length]);
    save_ಮಟ್ಟಗಳು = reinterpret_cast<UBiDiLevel*>(&visual_ಪಠ್ಯ[length]);
    saveOptions=pBiDi->reorderingOptions;
    ಆದರೆ(saveOptions & UBIDI_OPTION_ಸೇರಿಸು_MARKS) {
        pBiDi->reorderingOptions&=~UBIDI_OPTION_ಸೇರಿಸು_MARKS;
        pBiDi->reorderingOptions|=UBIDI_OPTION_REMOVE_CONTROLS;
    }
    paraLevel&=1;                       /* accept only 0 or 1 */
    ubidi_ಗಣ_Para(pBiDi, ಪಠ್ಯ, length, paraLevel, ಶೂನ್ಯಸೂಚಕ, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹೋಗು cleanup3;
    }
    /* we cannot access directly pBiDi->levels since it is not yet set if
     * direction is not MIXED
     */
    ಮಟ್ಟಗಳು=ubidi_get_ಮಟ್ಟಗಳು(pBiDi, p_Error_ಸಂಕೇತ);
    uprv_memcpy(save_ಮಟ್ಟಗಳು, ಮಟ್ಟಗಳು, (size_t)pBiDi->length*ಗಾತ್ರ(UBiDiLevel));
    saveTrailingWSStart=pBiDi->trailingWSStart;
    save_ಉದ್ದಳತೆ=pBiDi->length;
    saveDirection=pBiDi->direction;

    /* FOOD FOR THOUGHT: instead of writing the visual text, we could use
     * the visual map and the dirProps array to drive the second call
     * to ubidi_setPara (but must make provision for possible removal of
     * BiDi controls.  Alternatively, only use the dirProps array via
     * customized classifier callback.
     *
     * Note: We pass UBIDI_INTERNAL_RUNS_ONLY so that if mirroring changes code point
     * length (e.g., U+1DB10 contracting back to U+221D), doWriteReverse() uses legacy
     * 1-to-1 code unit stepping (j += k) rather than character stepping (j += origBaseLen).
     * This ensures visualLength remains synchronized with pBiDi->length and allows the
     * trailing surrogate half (Bidi class LTR) to act as a run boundary splitter in the
     * subsequent ubidi_setPara call.
     */
    visual_ಉದ್ದಳತೆ=ubidi_writeReordered(pBiDi, visual_ಪಠ್ಯ, length,
                                      (saveOptions & UBIDI_DO_MIRRORING) | UBIDI_ಆಂತರಿಕ_RUNS_ONLY, p_Error_ಸಂಕೇತ);
    ubidi_getVisualMap(pBiDi, visualMap, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹೋಗು cleanup2;
    }
    pBiDi->reorderingOptions=saveOptions;

    pBiDi->reordering_ಬಹುಲಕ=UBIDI_REORDER_INVERSE_LIKE_ನೇರ;
    paraLevel^=1;
    /* Because what we did with reorderingOptions, visualText may be shorter
     * than the original text. But we don't want the levels memory to be
     * reallocated shorter than the original length, since we need to restore
     * the levels as after the first call to ubidi_setpara() before returning.
     * We will force mayAllocateText to false before the second call to
     * ubidi_setpara(), and will restore it afterwards.
     */
    save_May_Allocate_ಪಠ್ಯ=pBiDi->may_Allocate_ಪಠ್ಯ;
    pBiDi->may_Allocate_ಪಠ್ಯ=ಸುಳ್ಳು;
    ubidi_ಗಣ_Para(pBiDi, visual_ಪಠ್ಯ, visual_ಉದ್ದಳತೆ, paraLevel, ಶೂನ್ಯಸೂಚಕ, p_Error_ಸಂಕೇತ);
    pBiDi->may_Allocate_ಪಠ್ಯ=save_May_Allocate_ಪಠ್ಯ;
    ubidi_getRuns(pBiDi, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹೋಗು cleanup1;
    }
    /* check if some runs must be split, count how many splits */
    addedRuns=0;
    ಸತತ_ಎಣಿಕೆ=pBiDi->ಸತತ_ಎಣಿಕೆ;
    runs=pBiDi->runs;
    visual_ಆರಂಭ=0;
    ಗಾಗಿ(i=0; i<ಸತತ_ಎಣಿಕೆ; i++, visual_ಆರಂಭ+=ಸತತ_ಉದ್ದಳತೆ) {
        ಸತತ_ಉದ್ದಳತೆ=runs[i].visual_ಎಲ್ಲೆ-visual_ಆರಂಭ;
        ಆದರೆ(ಸತತ_ಉದ್ದಳತೆ<2) {
            ಮುಂದುವರಿ;
        }
        logical_ಆರಂಭ=GET_ಸೂಚ್ಯಂಕ(runs[i].logical_ಆರಂಭ);
        ಗಾಗಿ(j=logical_ಆರಂಭ+1; j<logical_ಆರಂಭ+ಸತತ_ಉದ್ದಳತೆ; j++) {
            index0=visualMap[j];
            index1=visualMap[j-1];
            ಆದರೆ((BIDI_ABS(index0-index1)!=1) || (save_ಮಟ್ಟಗಳು[index0]!=save_ಮಟ್ಟಗಳು[index1])) {
                addedRuns++;
            }
        }
    }
    ಆದರೆ(addedRuns) {
        ಆದರೆ(get_Runs_ಸ್ಮರಣೆ(pBiDi, ಸತತ_ಎಣಿಕೆ+addedRuns)) {
            ಆದರೆ(ಸತತ_ಎಣಿಕೆ==1) {
                /* because we switch from UBiDi.simpleRuns to UBiDi.runs */
                pBiDi->runs_ಸ್ಮರಣೆ[0]=runs[0];
            }
            runs=pBiDi->runs=pBiDi->runs_ಸ್ಮರಣೆ;
            pBiDi->ಸತತ_ಎಣಿಕೆ+=addedRuns;
        } ಇಲ್ಲದಿದ್ದರೆ {
            *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹೋಗು cleanup1;
        }
    }
    /* split runs which are not consecutive in source text */
    ಗಾಗಿ(i=ಸತತ_ಎಣಿಕೆ-1; i>=0; i--) {
        ಸತತ_ಉದ್ದಳತೆ= i==0 ? runs[0].visual_ಎಲ್ಲೆ :
                          runs[i].visual_ಎಲ್ಲೆ-runs[i-1].visual_ಎಲ್ಲೆ;
        logical_ಆರಂಭ=runs[i].logical_ಆರಂಭ;
        ಸೂಚ್ಯಂಕ_Odd_ಬಿಟ್=GET_ODD_ಬಿಟ್(logical_ಆರಂಭ);
        logical_ಆರಂಭ=GET_ಸೂಚ್ಯಂಕ(logical_ಆರಂಭ);
        ಆದರೆ(ಸತತ_ಉದ್ದಳತೆ<2) {
            ಆದರೆ(addedRuns) {
                runs[i+addedRuns]=runs[i];
            }
            logicalPos=visualMap[logical_ಆರಂಭ];
            runs[i+addedRuns].logical_ಆರಂಭ=MAKE_ಸೂಚ್ಯಂಕ_ODD_PAIR(logicalPos,
                                            save_ಮಟ್ಟಗಳು[logicalPos]^ಸೂಚ್ಯಂಕ_Odd_ಬಿಟ್);
            ಮುಂದುವರಿ;
        }
        ಆದರೆ(ಸೂಚ್ಯಂಕ_Odd_ಬಿಟ್) {
            ಆರಂಭ=logical_ಆರಂಭ;
            ಎಲ್ಲೆ=logical_ಆರಂಭ+ಸತತ_ಉದ್ದಳತೆ-1;
            ಹೆಜ್ಜೆ=1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆರಂಭ=logical_ಆರಂಭ+ಸತತ_ಉದ್ದಳತೆ-1;
            ಎಲ್ಲೆ=logical_ಆರಂಭ;
            ಹೆಜ್ಜೆ=-1;
        }
        ಗಾಗಿ(j=ಆರಂಭ; j!=ಎಲ್ಲೆ; j+=ಹೆಜ್ಜೆ) {
            index0=visualMap[j];
            index1=visualMap[j+ಹೆಜ್ಜೆ];
            ಆದರೆ((BIDI_ABS(index0-index1)!=1) || (save_ಮಟ್ಟಗಳು[index0]!=save_ಮಟ್ಟಗಳು[index1])) {
                logicalPos=BIDI_MIN(visualMap[ಆರಂಭ], index0);
                runs[i+addedRuns].logical_ಆರಂಭ=MAKE_ಸೂಚ್ಯಂಕ_ODD_PAIR(logicalPos,
                                            save_ಮಟ್ಟಗಳು[logicalPos]^ಸೂಚ್ಯಂಕ_Odd_ಬಿಟ್);
                runs[i+addedRuns].visual_ಎಲ್ಲೆ=runs[i].visual_ಎಲ್ಲೆ;
                runs[i].visual_ಎಲ್ಲೆ-=BIDI_ABS(j-ಆರಂಭ)+1;
                ಸೇರಿಸು_Remove=runs[i].ಸೇರಿಸು_Remove&(LRM_AFTER|RLM_AFTER);
                runs[i+addedRuns].ಸೇರಿಸು_Remove=ಸೇರಿಸು_Remove;
                runs[i].ಸೇರಿಸು_Remove&=~ಸೇರಿಸು_Remove;
                ಆರಂಭ=j+ಹೆಜ್ಜೆ;
                addedRuns--;
            }
        }
        ಆದರೆ(addedRuns) {
            runs[i+addedRuns]=runs[i];
        }
        logicalPos=BIDI_MIN(visualMap[ಆರಂಭ], visualMap[ಎಲ್ಲೆ]);
        runs[i+addedRuns].logical_ಆರಂಭ=MAKE_ಸೂಚ್ಯಂಕ_ODD_PAIR(logicalPos,
                                            save_ಮಟ್ಟಗಳು[logicalPos]^ಸೂಚ್ಯಂಕ_Odd_ಬಿಟ್);
    }

  cleanup1:
    /* restore initial paraLevel */
    pBiDi->paraLevel^=1;
  cleanup2:
    /* restore real text */
    pBiDi->ಪಠ್ಯ=ಪಠ್ಯ;
    pBiDi->length=save_ಉದ್ದಳತೆ;
    pBiDi->original_ಉದ್ದಳತೆ=length;
    pBiDi->direction=saveDirection;
    /* the saved levels should never excess levelsSize, but we check anyway */
    ಆದರೆ(save_ಉದ್ದಳತೆ>pBiDi->ಮಟ್ಟಗಳು_ಗಾತ್ರ) {
        save_ಉದ್ದಳತೆ=pBiDi->ಮಟ್ಟಗಳು_ಗಾತ್ರ;
    }
    uprv_memcpy(pBiDi->ಮಟ್ಟಗಳು, save_ಮಟ್ಟಗಳು, (size_t)save_ಉದ್ದಳತೆ*ಗಾತ್ರ(UBiDiLevel));
    pBiDi->trailingWSStart=saveTrailingWSStart;
    ಆದರೆ(pBiDi->ಸತತ_ಎಣಿಕೆ>1) {
        pBiDi->direction=UBIDI_MIXED;
    }
  cleanup3:
    /* free memory for mapping table and visual text */
    uprv_free(runs_Only_ಸ್ಮರಣೆ);

    pBiDi->reordering_ಬಹುಲಕ=UBIDI_REORDER_RUNS_ONLY;
}

/* ubidi_setPara ------------------------------------------------------------ */

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_ಗಣ_Para(UBiDi *pBiDi, ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ, int32_t length,
              UBiDiLevel paraLevel, UBiDiLevel *embedding_ಮಟ್ಟಗಳು,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UBiDiDirection direction;
    DirProp *dirProps;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ಸಂಕೇತ);
    ಆದರೆ(pBiDi==ಶೂನ್ಯಸೂಚಕ || ಪಠ್ಯ==ಶೂನ್ಯಸೂಚಕ || length<-1 ||
       (paraLevel>UBIDI_MAX_EXPLICIT_LEVEL && paraLevel<UBIDI_DEFAULT_LTR)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(length==-1) {
        length=u_strlen(ಪಠ್ಯ);
    }

    /* special treatment for RUNS_ONLY mode */
    ಆದರೆ(pBiDi->reordering_ಬಹುಲಕ==UBIDI_REORDER_RUNS_ONLY) {
        ಗಣ_Para_Runs_Only(pBiDi, ಪಠ್ಯ, length, paraLevel, p_Error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ;
    }

    /* initialize the UBiDi structure */
    pBiDi->pParaBiDi=ಶೂನ್ಯಸೂಚಕ;          /* mark unfinished setPara */
    pBiDi->ಪಠ್ಯ=ಪಠ್ಯ;
    pBiDi->length=pBiDi->original_ಉದ್ದಳತೆ=pBiDi->ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ=length;
    pBiDi->paraLevel=paraLevel;
    pBiDi->direction=(UBiDiDirection)(paraLevel&1);
    pBiDi->para_ಎಣಿಕೆ=1;

    pBiDi->dirProps=ಶೂನ್ಯಸೂಚಕ;
    pBiDi->ಮಟ್ಟಗಳು=ಶೂನ್ಯಸೂಚಕ;
    pBiDi->runs=ಶೂನ್ಯಸೂಚಕ;
    pBiDi->ಸೇರಿಸು_Points.size=0;         /* clean up from last call */
    pBiDi->ಸೇರಿಸು_Points.confirmed=0;    /* clean up from last call */

    /*
     * Save the original paraLevel if contextual; otherwise, set to 0.
     */
    pBiDi->defaultParaLevel=IS_DEFAULT_LEVEL(paraLevel);

    ಆದರೆ(length==0) {
        /*
         * For an empty paragraph, create a UBiDi object with the paraLevel and
         * the flags and the direction set but without allocating zero-length arrays.
         * There is nothing more to do.
         */
        ಆದರೆ(IS_DEFAULT_LEVEL(paraLevel)) {
            pBiDi->paraLevel&=1;
            pBiDi->defaultParaLevel=0;
        }
        pBiDi->flags=DIRPROP_FLAG_LR(paraLevel);
        pBiDi->ಸತತ_ಎಣಿಕೆ=0;
        pBiDi->para_ಎಣಿಕೆ=0;
        ಗಣ_Para_Success(pBiDi);          /* mark successful setPara */
        ಹಿಂತಿರುಗಿ;
    }

    pBiDi->ಸತತ_ಎಣಿಕೆ=-1;

    /* allocate paras memory */
    ಆದರೆ(pBiDi->paras_ಸ್ಮರಣೆ)
        pBiDi->paras=pBiDi->paras_ಸ್ಮರಣೆ;
    ಇಲ್ಲದಿದ್ದರೆ
        pBiDi->paras=pBiDi->simpleParas;

    /*
     * Get the directional properties,
     * the flags bit-set, and
     * determine the paragraph level if necessary.
     */
    ಆದರೆ(get_Dir_Props_ಸ್ಮರಣೆ(pBiDi, length)) {
        pBiDi->dirProps=pBiDi->dir_Props_ಸ್ಮರಣೆ;
        ಆದರೆ(!getDirProps(pBiDi)) {
            *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    dirProps=pBiDi->dirProps;
    /* the processed length may have changed if UBIDI_OPTION_STREAMING */
    length= pBiDi->length;
    pBiDi->trailingWSStart=length;  /* the levels[] will reflect the WS run */

    /* are explicit levels specified? */
    ಆದರೆ(embedding_ಮಟ್ಟಗಳು==ಶೂನ್ಯಸೂಚಕ) {
        /* no: determine explicit levels according to the (Xn) rules */\
        ಆದರೆ(get_ಮಟ್ಟಗಳು_ಸ್ಮರಣೆ(pBiDi, length)) {
            pBiDi->ಮಟ್ಟಗಳು=pBiDi->ಮಟ್ಟಗಳು_ಸ್ಮರಣೆ;
            direction=resolve_Explicit_ಮಟ್ಟಗಳು(pBiDi, p_Error_ಸಂಕೇತ);
            ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* set BN for all explicit codes, check that all levels are 0 or paraLevel..UBIDI_MAX_EXPLICIT_LEVEL */
        pBiDi->ಮಟ್ಟಗಳು=embedding_ಮಟ್ಟಗಳು;
        direction=check_Explicit_ಮಟ್ಟಗಳು(pBiDi, p_Error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ;
        }
    }

    /* allocate isolate memory */
    ಆದರೆ(pBiDi->isolate_ಎಣಿಕೆ<=SIMPLE_ISOLATES_ಎಣಿಕೆ)
        pBiDi->isolates=pBiDi->simpleIsolates;
    ಇಲ್ಲದಿದ್ದರೆ
        ಆದರೆ((int32_t)(pBiDi->isolate_ಎಣಿಕೆ*ಗಾತ್ರ(Isolate))<=pBiDi->isolates_ಗಾತ್ರ)
            pBiDi->isolates=pBiDi->isolates_ಸ್ಮರಣೆ;
        ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(get_Initial_Isolates_ಸ್ಮರಣೆ(pBiDi, pBiDi->isolate_ಎಣಿಕೆ)) {
                pBiDi->isolates=pBiDi->isolates_ಸ್ಮರಣೆ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
        }
    pBiDi->isolate_ಎಣಿಕೆ=-1;             /* current isolates stack entry == none */

    /*
     * The steps after (X9) in the UBiDi algorithm are performed only if
     * the paragraph text has mixed directionality!
     */
    pBiDi->direction=direction;
    ಆಯ್ಕೆ(direction) {
    ಸ್ಥಿತಿ UBIDI_LTR:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UBIDI_RTL:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        /*
         *  Choose the right implicit state table
         */
        ಆಯ್ಕೆ(pBiDi->reordering_ಬಹುಲಕ) {
        ಸ್ಥಿತಿ UBIDI_REORDER_DEFAULT:
            pBiDi->pImpTabPair=&impTab_DEFAULT;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UBIDI_REORDER_NUMBERS_SPECIAL:
            pBiDi->pImpTabPair=&impTab_NUMBERS_SPECIAL;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UBIDI_REORDER_ಗುಂಪು_NUMBERS_WITH_R:
            pBiDi->pImpTabPair=&imp_Tab_ಗುಂಪು_NUMBERS_WITH_R;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UBIDI_REORDER_INVERSE_NUMBERS_AS_L:
            pBiDi->pImpTabPair=&impTab_INVERSE_NUMBERS_AS_L;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UBIDI_REORDER_INVERSE_LIKE_ನೇರ:
            ಆದರೆ (pBiDi->reorderingOptions & UBIDI_OPTION_ಸೇರಿಸು_MARKS) {
                pBiDi->pImpTabPair=&imp_Tab_INVERSE_LIKE_ನೇರ_WITH_MARKS;
            } ಇಲ್ಲದಿದ್ದರೆ {
                pBiDi->pImpTabPair=&imp_Tab_INVERSE_LIKE_ನೇರ;
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL:
            ಆದರೆ (pBiDi->reorderingOptions & UBIDI_OPTION_ಸೇರಿಸು_MARKS) {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS;
            } ಇಲ್ಲದಿದ್ದರೆ {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL;
            }
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            /* we should never get here */
            UPRV_UNREACHABLE_EXIT;
        }
        /*
         * If there are no external levels specified and there
         * are no significant explicit level codes in the text,
         * then we can treat the entire paragraph as one run.
         * Otherwise, we need to perform the following rules on runs of
         * the text with the same embedding levels. (X10)
         * "Significant" explicit level codes are ones that actually
         * affect non-BN characters.
         * Examples for "insignificant" ones are empty embeddings
         * LRE-PDF, LRE-RLE-PDF-PDF, etc.
         */
        ಆದರೆ(embedding_ಮಟ್ಟಗಳು==ಶೂನ್ಯಸೂಚಕ && pBiDi->para_ಎಣಿಕೆ<=1 &&
                                   !(pBiDi->flags&DIRPROP_FLAG_MULTI_RUNS)) {
            resolve_Implicit_ಮಟ್ಟಗಳು(pBiDi, 0, length,
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, 0)),
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, length-1)));
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* sor, eor: start and end types of same-level-run */
            UBiDiLevel *ಮಟ್ಟಗಳು=pBiDi->ಮಟ್ಟಗಳು;
            int32_t ಆರಂಭ, ಎಲ್ಲೆ=0;
            UBiDiLevel level, ಮುಂದಿನ_Level;
            DirProp sor, eor;

            /* determine the first sor and set eor to it because of the loop body (sor=eor there) */
            level=GET_PARALEVEL(pBiDi, 0);
            ಮುಂದಿನ_Level=ಮಟ್ಟಗಳು[0];
            ಆದರೆ(level<ಮುಂದಿನ_Level) {
                eor=GET_LR_FROM_LEVEL(ಮುಂದಿನ_Level);
            } ಇಲ್ಲದಿದ್ದರೆ {
                eor=GET_LR_FROM_LEVEL(level);
            }

            ಮಾಡು {
                /* determine start and limit of the run (end points just behind the run) */

                /* the values for this run's start are the same as for the previous run's end */
                ಆರಂಭ=ಎಲ್ಲೆ;
                level=ಮುಂದಿನ_Level;
                ಆದರೆ((ಆರಂಭ>0) && (dirProps[ಆರಂಭ-1]==B)) {
                    /* except if this is a new paragraph, then set sor = para level */
                    sor=GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, ಆರಂಭ));
                } ಇಲ್ಲದಿದ್ದರೆ {
                    sor=eor;
                }

                /* search for the limit of this run */
                ಆಗ((++ಎಲ್ಲೆ<length) &&
                      ((ಮಟ್ಟಗಳು[ಎಲ್ಲೆ]==level) ||
                       (DIRPROP_FLAG(dirProps[ಎಲ್ಲೆ])&ಮುಖವಾಡ_BN_EXPLICIT))) {}

                /* get the correct level of the next run */
                ಆದರೆ(ಎಲ್ಲೆ<length) {
                    ಮುಂದಿನ_Level=ಮಟ್ಟಗಳು[ಎಲ್ಲೆ];
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಮುಂದಿನ_Level=GET_PARALEVEL(pBiDi, length-1);
                }

                /* determine eor from max(level, nextLevel); sor is last run's eor */
                ಆದರೆ(NO_OVERRIDE(level)<NO_OVERRIDE(ಮುಂದಿನ_Level)) {
                    eor=GET_LR_FROM_LEVEL(ಮುಂದಿನ_Level);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    eor=GET_LR_FROM_LEVEL(level);
                }

                /* if the run consists of overridden directional types, then there
                   are no implicit types to be resolved */
                ಆದರೆ(!(level&UBIDI_LEVEL_OVERRIDE)) {
                    resolve_Implicit_ಮಟ್ಟಗಳು(pBiDi, ಆರಂಭ, ಎಲ್ಲೆ, sor, eor);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* remove the UBIDI_LEVEL_OVERRIDE flags */
                    ಮಾಡು {
                        ಮಟ್ಟಗಳು[ಆರಂಭ++]&=~UBIDI_LEVEL_OVERRIDE;
                    } ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ);
                }
            } ಆಗ(ಎಲ್ಲೆ<length);
        }
        /* check if we got any memory shortage while adding insert points */
        ಆದರೆ (U_FAILURE(pBiDi->ಸೇರಿಸು_Points.error_ಸಂಕೇತ))
        {
            *p_Error_ಸಂಕೇತ=pBiDi->ಸೇರಿಸು_Points.error_ಸಂಕೇತ;
            ಹಿಂತಿರುಗಿ;
        }
        /* reset the embedding levels for some non-graphic characters (L1), (X9) */
        adjustWSLevels(pBiDi);
        ನಿಲ್ಲಿಸು;
    }
    /* add RLM for inverse Bidi with contextual orientation resolving
     * to RTL which would not round-trip otherwise
     */
    ಆದರೆ((pBiDi->defaultParaLevel>0) &&
       (pBiDi->reorderingOptions & UBIDI_OPTION_ಸೇರಿಸು_MARKS) &&
       ((pBiDi->reordering_ಬಹುಲಕ==UBIDI_REORDER_INVERSE_LIKE_ನೇರ) ||
        (pBiDi->reordering_ಬಹುಲಕ==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))) {
        int32_t i, j, ಆರಂಭ, ಕೊನೆಯ;
        UBiDiLevel level;
        DirProp dirProp;
        ಗಾಗಿ(i=0; i<pBiDi->para_ಎಣಿಕೆ; i++) {
            ಕೊನೆಯ=(pBiDi->paras[i].ಎಲ್ಲೆ)-1;
            level= static_cast<UBiDiLevel>(pBiDi->paras[i].level);
            ಆದರೆ(level==0)
                ಮುಂದುವರಿ;           /* LTR paragraph */
            ಆರಂಭ= i==0 ? 0 : pBiDi->paras[i-1].ಎಲ್ಲೆ;
            ಗಾಗಿ(j=ಕೊನೆಯ; j>=ಆರಂಭ; j--) {
                dirProp=dirProps[j];
                ಆದರೆ(dirProp==L) {
                    ಆದರೆ(j<ಕೊನೆಯ) {
                        ಆಗ(dirProps[ಕೊನೆಯ]==B) {
                            ಕೊನೆಯ--;
                        }
                    }
                    ಕೂಡಿಸು_ಬಿಂದು(pBiDi, ಕೊನೆಯ, RLM_BEFORE);
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ(DIRPROP_FLAG(dirProp) & ಮುಖವಾಡ_R_AL) {
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
    }

    ಆದರೆ(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        pBiDi->ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ -= pBiDi->control_ಎಣಿಕೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        pBiDi->ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ += pBiDi->ಸೇರಿಸು_Points.size;
    }
    ಗಣ_Para_Success(pBiDi);              /* mark successful setPara */
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_ಕ್ರಮ_Paragraphs_LTR(UBiDi *pBiDi, UBool ಕ್ರಮ_Paragraphs_LTR) {
    ಆದರೆ(pBiDi!=ಶೂನ್ಯಸೂಚಕ) {
        pBiDi->ಕ್ರಮ_Paragraphs_LTR=ಕ್ರಮ_Paragraphs_LTR;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_is_ಕ್ರಮ_Paragraphs_LTR(UBiDi *pBiDi) {
    ಆದರೆ(pBiDi!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ pBiDi->ಕ್ರಮ_Paragraphs_LTR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_getDirection(ಸ್ಥಿರಾಂಕ UBiDi *pBiDi) {
    ಆದರೆ(IS_VALID_PARA_OR_ಸಾಲು(pBiDi)) {
        ಹಿಂತಿರುಗಿ pBiDi->direction;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ UBIDI_LTR;
    }
}

U_CAPI ಸ್ಥಿರಾಂಕ char16_t* U_EXPORT2
ubidi_get_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ UBiDi* pBiDi) {
    ಆದರೆ(IS_VALID_PARA_OR_ಸಾಲು(pBiDi)) {
        ಹಿಂತಿರುಗಿ pBiDi->ಪಠ್ಯ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ UBiDi *pBiDi) {
    ಆದರೆ(IS_VALID_PARA_OR_ಸಾಲು(pBiDi)) {
        ಹಿಂತಿರುಗಿ pBiDi->original_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Processed_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ UBiDi *pBiDi) {
    ಆದರೆ(IS_VALID_PARA_OR_ಸಾಲು(pBiDi)) {
        ಹಿಂತಿರುಗಿ pBiDi->length;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ UBiDi *pBiDi) {
    ಆದರೆ(IS_VALID_PARA_OR_ಸಾಲು(pBiDi)) {
        ಹಿಂತಿರುಗಿ pBiDi->ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}

/* paragraphs API functions ------------------------------------------------- */

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getParaLevel(ಸ್ಥಿರಾಂಕ UBiDi *pBiDi) {
    ಆದರೆ(IS_VALID_PARA_OR_ಸಾಲು(pBiDi)) {
        ಹಿಂತಿರುಗಿ pBiDi->paraLevel;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_ಎಣಿಕೆ_Paragraphs(UBiDi *pBiDi) {
    ಆದರೆ(!IS_VALID_PARA_OR_ಸಾಲು(pBiDi)) {
        ಹಿಂತಿರುಗಿ 0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ pBiDi->para_ಎಣಿಕೆ;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_get_Paragraph_By_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UBiDi *pBiDi, int32_t para_ಸೂಚ್ಯಂಕ,
                          int32_t *p_Para_ಆರಂಭ, int32_t *p_Para_ಎಲ್ಲೆ,
                          UBiDiLevel *pParaLevel, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    int32_t para_ಆರಂಭ;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ಸಂಕೇತ);
    RETURN_VOID_IF_NOT_VALID_PARA_OR_ಸಾಲು(pBiDi, *p_Error_ಸಂಕೇತ);
    RETURN_VOID_IF_BAD_RANGE(para_ಸೂಚ್ಯಂಕ, 0, pBiDi->para_ಎಣಿಕೆ, *p_Error_ಸಂಕೇತ);

    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    ಆದರೆ(para_ಸೂಚ್ಯಂಕ) {
        para_ಆರಂಭ=pBiDi->paras[para_ಸೂಚ್ಯಂಕ-1].ಎಲ್ಲೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        para_ಆರಂಭ=0;
    }
    ಆದರೆ(p_Para_ಆರಂಭ!=ಶೂನ್ಯಸೂಚಕ) {
        *p_Para_ಆರಂಭ=para_ಆರಂಭ;
    }
    ಆದರೆ(p_Para_ಎಲ್ಲೆ!=ಶೂನ್ಯಸೂಚಕ) {
        *p_Para_ಎಲ್ಲೆ=pBiDi->paras[para_ಸೂಚ್ಯಂಕ].ಎಲ್ಲೆ;
    }
    ಆದರೆ(pParaLevel!=ಶೂನ್ಯಸೂಚಕ) {
        *pParaLevel=GET_PARALEVEL(pBiDi, para_ಆರಂಭ);
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_getParagraph(ಸ್ಥಿರಾಂಕ UBiDi *pBiDi, int32_t char_ಸೂಚ್ಯಂಕ,
                          int32_t *p_Para_ಆರಂಭ, int32_t *p_Para_ಎಲ್ಲೆ,
                          UBiDiLevel *pParaLevel, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    int32_t para_ಸೂಚ್ಯಂಕ;

    /* check the argument values */
    /* pErrorCode will be checked by the call to ubidi_getParagraphByIndex */
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ಸಂಕೇತ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_ಸಾಲು(pBiDi, *p_Error_ಸಂಕೇತ, -1);
    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    RETURN_IF_BAD_RANGE(char_ಸೂಚ್ಯಂಕ, 0, pBiDi->length, *p_Error_ಸಂಕೇತ, -1);

    ಗಾಗಿ(para_ಸೂಚ್ಯಂಕ=0; char_ಸೂಚ್ಯಂಕ>=pBiDi->paras[para_ಸೂಚ್ಯಂಕ].ಎಲ್ಲೆ; para_ಸೂಚ್ಯಂಕ++);
    ubidi_get_Paragraph_By_ಸೂಚ್ಯಂಕ(pBiDi, para_ಸೂಚ್ಯಂಕ, p_Para_ಆರಂಭ, p_Para_ಎಲ್ಲೆ, pParaLevel, p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ para_ಸೂಚ್ಯಂಕ;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_ಗಣ_Class_Callback(UBiDi *pBiDi, UBiDiClassCallback *newFn,
                       ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *newContext, UBiDiClassCallback **oldFn,
                       ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **oldContext, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ಸಂಕೇತ);
    ಆದರೆ(pBiDi==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ( oldFn )
    {
        *oldFn = pBiDi->fnClassCallback;
    }
    ಆದರೆ( oldContext )
    {
        *oldContext = pBiDi->coClassCallback;
    }
    pBiDi->fnClassCallback = newFn;
    pBiDi->coClassCallback = newContext;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubidi_getClassCallback(UBiDi *pBiDi, UBiDiClassCallback **fn, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **context)
{
    ಆದರೆ(pBiDi==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ( fn )
    {
        *fn = pBiDi->fnClassCallback;
    }
    ಆದರೆ( context )
    {
        *context = pBiDi->coClassCallback;
    }
}

U_CAPI UCharDirection U_EXPORT2
ubidi_getCustomizedClass(UBiDi *pBiDi, UChar32 c)
{
    UCharDirection dir;

    ಆದರೆ( pBiDi->fnClassCallback == ಶೂನ್ಯಸೂಚಕ ||
        (dir = (*pBiDi->fnClassCallback)(pBiDi->coClassCallback, c)) == U_BIDI_CLASS_DEFAULT )
    {
        dir = ubidi_getClass(c);
    }
    ಆದರೆ(dir >= U_CHAR_DIRECTION_ಎಣಿಕೆ) {
        dir = (UCharDirection)ON;
    }
    ಹಿಂತಿರುಗಿ dir;
}

/* Utilities for MessageFormat ------------------------------------------------*/

#ಆದರೆ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

U_CAPI UMFBidiOption U_EXPORT2
ubidi_getMFOption(ಸ್ಥಿರಾಂಕ UnicodeString& s) {
    ಆದರೆ (s == u"ltr") {
        ಹಿಂತಿರುಗಿ U_MF_BIDI_OPTION_LTR;
    }
    ಆದರೆ (s == u"rtl") {
        ಹಿಂತಿರುಗಿ U_MF_BIDI_OPTION_RTL;
    }
    ಆದರೆ (s == u"auto") {
        ಹಿಂತಿರುಗಿ U_MF_BIDI_OPTION_AUTO;
    }
    ಹಿಂತಿರುಗಿ U_MF_BIDI_OPTION_INHERIT; // inherit is default
}

// BiDi isolation
// --------------
// `uDirOption` is the directionality from a u:dir annotation on the expression
// that produced this formatted value, if present.
// `dir` is the directionality of `fmt`. This is determined from the resolved
// value that `fmt` is part of; that is, each function can set the directionality
// of the resolved value of its result.
U_CAPI UnicodeString* U_EXPORT2
ubidi_isolate(UMFBidiIsolationStrategy bidiIsolationStrategy,
                            UMFDirectionality msgdir,
                            UMFBidiIsolationStyle bidiIsolationStyle,
                            UMFBidiOption uDirOption,
                            UMFDirectionality dir,
                            UnicodeString& fmt) {
    // See "The Default Bidi Strategy" at:
    // https://github.com/unicode-org/message-format-wg/blob/main/spec/formatting.md#handling-bidirectional-text

    // If strategy is 'none', just return the string
    ಆದರೆ (bidiIsolationStrategy == U_MF_BIDI_OFF)
        ಹಿಂತಿರುಗಿ &fmt;

    /* 1. Let msgdir be the directionality of the whole message, one of « 'LTR', 'RTL', 'unknown' ». These correspond to the message having left-to-right directionality, right-to-left directionality, and to the message's directionality not being known. */

    // 2i Let fmt be the formatted string representation of the resolved value of exp.
    // (Passed as argument)

    // 2ii Let dir be the directionality of fmt, one of « 'LTR', 'RTL', 'unknown' », with the same meanings as for msgdir
    // (Passed as argument)

    // 2iii. Let the boolean value isolate be True if the u:dir option of the resolved value of exp has a value other than 'inherit', or False otherwise.
    ತಾರ್ಕಿಕ isolate = uDirOption != U_MF_BIDI_OPTION_INHERIT;

    UnicodeString bdiOpen("<bdi>");
    UnicodeString bdiClose("</bdi>");

    // 2iv. If dir is 'LTR'
    ಆಯ್ಕೆ (dir) {
        ಸ್ಥಿತಿ U_MF_DIRECTIONALITY_LTR:
            ಆದರೆ (msgdir == U_MF_DIRECTIONALITY_LTR && !isolate) {
                // 2iv(a). If msgdir is 'LTR' in the formatted output, let fmt be itself
                ಹಿಂತಿರುಗಿ &fmt;
            }
            // 2iii(b) Else, in the formatted output, prefix fmt with U+2066 LEFT-TO-RIGHT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            ಆದರೆ (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, LRI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } ಇಲ್ಲದಿದ್ದರೆ {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            ನಿಲ್ಲಿಸು; // End of 2iii
        // 2iv. Else, if dir is 'RTL':
        ಸ್ಥಿತಿ U_MF_DIRECTIONALITY_RTL:
            // 2iv(a). In the formatted output, prefix fmt with U+2067 RIGHT-TO-LEFT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            ಆದರೆ (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, RLI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } ಇಲ್ಲದಿದ್ದರೆ {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            ನಿಲ್ಲಿಸು; // End of 2iv.
        // 2v. Else:
        ಪೂರ್ವನಿಯೋಜಿತ:
            // 2v(a). In the formatted output, prefix fmt with U+2068 FIRST STRONG ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            ಆದರೆ (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, FSI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } ಇಲ್ಲದಿದ್ದರೆ {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            ನಿಲ್ಲಿಸು; // End of 2v
    } // `fmt` now contains the isolated string
    ಹಿಂತಿರುಗಿ &fmt;
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ // #if SHOW_CPLUSPLUS_API

