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

#ਅਨ੍ਤਰ੍ਭਾਵ <limits>

#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubidi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubidi_props.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubidiimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

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
ਸਥਿਰ ਅਚਲ Flags flagLR[2]={ DIRPROP_FLAG(L), DIRPROP_FLAG(R) };
ਸਥਿਰ ਅਚਲ Flags flagE[2]={ DIRPROP_FLAG(LRE), DIRPROP_FLAG(RLE) };
ਸਥਿਰ ਅਚਲ Flags flagO[2]={ DIRPROP_FLAG(LRO), DIRPROP_FLAG(RLO) };

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIRPROP_FLAG_LR(level) flagLR[(level)&1]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIRPROP_FLAG_E(level)  flagE[(level)&1]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIRPROP_FLAG_O(level)  flagO[(level)&1]

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIR_FROM_STRONG(strong) ((strong)==L ? L : R)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NO_OVERRIDE(level)  ((level)&~UBIDI_LEVEL_OVERRIDE)

/* UBiDi object management -------------------------------------------------- */

U_CAPI UBiDi * U_EXPORT2
ubidi_open()
{
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਵਾਪਸ ubidi_openSized(0, 0, &error_ਕੋਡ);
}

U_CAPI UBiDi * U_EXPORT2
ubidi_openSized(int32_t max_ਲੰਮਾਈ_2, int32_t max_ਲੜੀਵਾਰ_ਗਿਣਤੀ, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UBiDi *pBiDi;

    /* check the argument values */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ ਜੇ(max_ਲੰਮਾਈ_2<0 || max_ਲੜੀਵਾਰ_ਗਿਣਤੀ<0) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;    /* invalid arguments */
    }

    /* allocate memory for the object */
    pBiDi=(UBiDi *)uprv_malloc(ਆਕਾਰ(UBiDi));
    ਜੇ(pBiDi==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* reset the object, all pointers nullptr, all flags false, all sizes 0 */
    uprv_memset(pBiDi, 0, ਆਕਾਰ(UBiDi));

    /* allocate memory for arrays as requested */
    ਜੇ(max_ਲੰਮਾਈ_2>0) {
        ਜੇ( !get_Initial_Dir_Props_ਸਮ੍ਰਿਤੀ(pBiDi, max_ਲੰਮਾਈ_2) ||
            !get_Initial_ਪੱਧਰ_ਸਮ੍ਰਿਤੀ(pBiDi, max_ਲੰਮਾਈ_2)
        ) {
            *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
    } ਨਹੀਂਤਾਂ {
        pBiDi->may_Allocate_ਪਾਠ=ਸੱਚ;
    }

    ਜੇ(max_ਲੜੀਵਾਰ_ਗਿਣਤੀ>0) {
        ਜੇ(max_ਲੜੀਵਾਰ_ਗਿਣਤੀ==1) {
            /* use simpleRuns[] */
            pBiDi->runs_ਮਾਪ=ਆਕਾਰ(ਲੜੀਵਾਰ);
        } ਨਹੀਂਤਾਂ ਜੇ(!get_Initial_Runs_ਸਮ੍ਰਿਤੀ(pBiDi, max_ਲੜੀਵਾਰ_ਗਿਣਤੀ)) {
            *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
    } ਨਹੀਂਤਾਂ {
        pBiDi->mayAllocateRuns=ਸੱਚ;
    }

    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ pBiDi;
    } ਨਹੀਂਤਾਂ {
        ubidi_close(pBiDi);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
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
ubidi_get_ਸਮ੍ਰਿਤੀ(Bidi_ਸਮ੍ਰਿਤੀ_For_ਵੰਡ *bidiMem, int32_t *p_ਮਾਪ, UBool mayAllocate, size_t ਮਾਪ_Needed) {
    ਜੇ (ਮਾਪ_Needed > std::numeric_limits<int32_t>::max()) {
      // TODO(egg): Ugly guard for ICU-23397.  A cleaner fix would be to change the pSize to size_t*
      // and update callers throughout to use size_t for sizes (just like uprv_malloc already does).
      // Not that anyone should be running the UBA on text with that many runs…
      ਵਾਪਸ ਝੂਠ;
    }
    ਖਾਲੀ **p_ਸਮ੍ਰਿਤੀ = (ਖਾਲੀ **)bidiMem;
    /* check for existing memory */
    ਜੇ(*p_ਸਮ੍ਰਿਤੀ==ਖਾਲੀ_ਸੂਚਕ) {
        /* we need to allocate memory */
        ਜੇ(mayAllocate && (*p_ਸਮ੍ਰਿਤੀ=uprv_malloc(ਮਾਪ_Needed))!=ਖਾਲੀ_ਸੂਚਕ) {
            *p_ਮਾਪ=static_cast<int32_t>(ਮਾਪ_Needed);
            ਵਾਪਸ ਸੱਚ;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ ਝੂਠ;
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ(ਮਾਪ_Needed<=static_cast<size_t>(*p_ਮਾਪ)) {
            /* there is already enough memory */
            ਵਾਪਸ ਸੱਚ;
        }
        ਨਹੀਂਤਾਂ ਜੇ(!mayAllocate) {
            /* not enough memory, and we must not allocate */
            ਵਾਪਸ ਝੂਠ;
        } ਨਹੀਂਤਾਂ {
            /* we try to grow */
            ਖਾਲੀ *ਸਮ੍ਰਿਤੀ;
            /* in most cases, we do not need the copy-old-data part of
             * realloc, but it is needed when adding runs using getRunsMemory()
             * in setParaRunsOnly()
             */
            ਜੇ((ਸਮ੍ਰਿਤੀ=uprv_realloc(*p_ਸਮ੍ਰਿਤੀ, ਮਾਪ_Needed))!=ਖਾਲੀ_ਸੂਚਕ) {
                *p_ਸਮ੍ਰਿਤੀ=ਸਮ੍ਰਿਤੀ;
                *p_ਮਾਪ=static_cast<int32_t>(ਮਾਪ_Needed);
                ਵਾਪਸ ਸੱਚ;
            } ਨਹੀਂਤਾਂ {
                /* we failed to grow */
                ਵਾਪਸ ਝੂਠ;
            }
        }
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_close(UBiDi *pBiDi) {
    ਜੇ(pBiDi!=ਖਾਲੀ_ਸੂਚਕ) {
        pBiDi->pParaBiDi=ਖਾਲੀ_ਸੂਚਕ;          /* in case one tries to reuse this block */
        ਜੇ(pBiDi->dir_Props_ਸਮ੍ਰਿਤੀ!=ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(pBiDi->dir_Props_ਸਮ੍ਰਿਤੀ);
        }
        ਜੇ(pBiDi->ਪੱਧਰ_ਸਮ੍ਰਿਤੀ!=ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(pBiDi->ਪੱਧਰ_ਸਮ੍ਰਿਤੀ);
        }
        ਜੇ(pBiDi->openings_ਸਮ੍ਰਿਤੀ!=ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(pBiDi->openings_ਸਮ੍ਰਿਤੀ);
        }
        ਜੇ(pBiDi->paras_ਸਮ੍ਰਿਤੀ!=ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(pBiDi->paras_ਸਮ੍ਰਿਤੀ);
        }
        ਜੇ(pBiDi->runs_ਸਮ੍ਰਿਤੀ!=ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(pBiDi->runs_ਸਮ੍ਰਿਤੀ);
        }
        ਜੇ(pBiDi->isolates_ਸਮ੍ਰਿਤੀ!=ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(pBiDi->isolates_ਸਮ੍ਰਿਤੀ);
        }
        ਜੇ(pBiDi->ਪਾਓ_Points.points!=ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(pBiDi->ਪਾਓ_Points.points);
        }

        uprv_free(pBiDi);
    }
}

/* set to approximate "inverse BiDi" ---------------------------------------- */

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_ਸੈੱਟ_Inverse(UBiDi *pBiDi, UBool isInverse) {
    ਜੇ(pBiDi!=ਖਾਲੀ_ਸੂਚਕ) {
        pBiDi->isInverse=isInverse;
        pBiDi->reordering_ਬਹੁਲਕ = isInverse ? UBIDI_REORDER_INVERSE_NUMBERS_AS_L
                                          : UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_isInverse(UBiDi *pBiDi) {
    ਜੇ(pBiDi!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ pBiDi->isInverse;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਝੂਠ;
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
U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_ਸੈੱਟ_Reordering_ਬਹੁਲਕ(UBiDi *pBiDi, UBi_Di_Reordering_ਬਹੁਲਕ reordering_ਬਹੁਲਕ) UPRV_NO_SANITIZE_UNDEFINED {
    ਜੇ ((pBiDi!=ਖਾਲੀ_ਸੂਚਕ) && (reordering_ਬਹੁਲਕ >= UBIDI_REORDER_DEFAULT)
                        && (reordering_ਬਹੁਲਕ < UBIDI_REORDER_ਗਿਣਤੀ)) {
        pBiDi->reordering_ਬਹੁਲਕ = reordering_ਬਹੁਲਕ;
        pBiDi->isInverse = reordering_ਬਹੁਲਕ == UBIDI_REORDER_INVERSE_NUMBERS_AS_L;
    }
}

U_CAPI UBi_Di_Reordering_ਬਹੁਲਕ U_EXPORT2
ubidi_get_Reordering_ਬਹੁਲਕ(UBiDi *pBiDi) {
    ਜੇ (pBiDi!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ pBiDi->reordering_ਬਹੁਲਕ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_ਸੈੱਟ_Reordering_Options(UBiDi *pBiDi, uint32_t reorderingOptions) {
    ਜੇ (reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        reorderingOptions&=~UBIDI_OPTION_ਪਾਓ_MARKS;
    }
    ਜੇ (pBiDi!=ਖਾਲੀ_ਸੂਚਕ) {
        pBiDi->reorderingOptions=reorderingOptions;
    }
}

U_CAPI uint32_t U_EXPORT2
ubidi_getReorderingOptions(UBiDi *pBiDi) {
    ਜੇ (pBiDi!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ pBiDi->reorderingOptions;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_ਅਧਾਰ_Direction(ਅਚਲ char16_t *ਪਾਠ,
int32_t length){

    int32_t i;
    UChar32 uchar;
    UCharDirection dir;

    ਜੇ( ਪਾਠ==ਖਾਲੀ_ਸੂਚਕ || length<-1 ){
        ਵਾਪਸ UBIDI_NEUTRAL;
    }

    ਜੇ(length==-1) {
        length=u_strlen(ਪਾਠ);
    }

    ਲਈ( i = 0 ; i < length; ) {
        /* i is incremented by U16_NEXT */
        U16_ਅਗਲਾ(ਪਾਠ, i, length, uchar);
        dir = u_charDirection(uchar);
        ਜੇ( dir == U_ਖੱਬਾ_ਮੰਜ਼ਿਲ_ਸੱਜਾ )
                ਵਾਪਸ UBIDI_LTR;
        ਜੇ( dir == U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ || dir ==U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ_ARABIC )
                ਵਾਪਸ UBIDI_RTL;
    }
    ਵਾਪਸ UBIDI_NEUTRAL;
}

/* perform (P2)..(P3) ------------------------------------------------------- */

/**
 * Returns the directionality of the first strong character
 * after the last B in prologue, if any.
 * Requires prologue!=null.
 */
ਸਥਿਰ DirProp
ਪਹਿਲਾ_L_R_AL(UBiDi *pBiDi) {
    ਅਚਲ char16_t *ਪਾਠ=pBiDi->prologue;
    int32_t length=pBiDi->pro_ਲੰਮਾਈ;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp, ਨਤੀਜਾ=ON;
    ਲਈ(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_ਅਗਲਾ(ਪਾਠ, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        ਜੇ(ਨਤੀਜਾ==ON) {
            ਜੇ(dirProp==L || dirProp==R || dirProp==AL) {
                ਨਤੀਜਾ=dirProp;
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ(dirProp==B) {
                ਨਤੀਜਾ=ON;
            }
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Check that there are enough entries in the array pointed to by pBiDi->paras
 */
ਸਥਿਰ UBool
check_Para_ਗਿਣਤੀ(UBiDi *pBiDi) {
    int32_t count=pBiDi->para_ਗਿਣਤੀ;
    ਜੇ(pBiDi->paras==pBiDi->simpleParas) {
        ਜੇ(count<=SIMPLE_PARAS_ਗਿਣਤੀ)
            ਵਾਪਸ ਸੱਚ;
        ਜੇ(!get_Initial_Paras_ਸਮ੍ਰਿਤੀ(pBiDi, SIMPLE_PARAS_ਗਿਣਤੀ * 2))
            ਵਾਪਸ ਝੂਠ;
        pBiDi->paras=pBiDi->paras_ਸਮ੍ਰਿਤੀ;
        uprv_memcpy(pBiDi->paras_ਸਮ੍ਰਿਤੀ, pBiDi->simpleParas, SIMPLE_PARAS_ਗਿਣਤੀ * ਆਕਾਰ(Para));
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ(!get_Initial_Paras_ਸਮ੍ਰਿਤੀ(pBiDi, count * 2))
        ਵਾਪਸ ਝੂਠ;
    pBiDi->paras=pBiDi->paras_ਸਮ੍ਰਿਤੀ;
    ਵਾਪਸ ਸੱਚ;
}

/*
 * Get the directional properties for the text, calculate the flags bit-set, and
 * determine the paragraph level if necessary (in pBiDi->paras[i].level).
 * FSI initiators are also resolved and their dirProp replaced with LRI or RLI.
 * When encountering an FSI, it is initially replaced with an LRI, which is the
 * default. Only if a strong R or AL is found within its scope will the LRI be
 * replaced by an RLI.
 */
ਸਥਿਰ UBool
getDirProps(UBiDi *pBiDi) {
    ਅਚਲ char16_t *ਪਾਠ=pBiDi->ਪਾਠ;
    DirProp *dirProps=pBiDi->dir_Props_ਸਮ੍ਰਿਤੀ;    /* pBiDi->dirProps is const */

    int32_t i=0, original_ਲੰਮਾਈ=pBiDi->original_ਲੰਮਾਈ;
    Flags flags=0;      /* collect all directionalities in the text */
    UChar32 uchar;
    DirProp dirProp=0, defaultParaLevel=0;  /* initialize to avoid compiler warnings */
    UBool isDefaultLevel=IS_DEFAULT_LEVEL(pBiDi->paraLevel);
    /* for inverse BiDi, the default para level is set to RTL if there is a
       strong R or AL character at either end of the text                            */
    UBool isDefaultLevelInverse = isDefaultLevel && static_cast<UBool>(
            pBiDi->reordering_ਬਹੁਲਕ == UBIDI_REORDER_INVERSE_LIKE_ਸਿੱਧਾ ||
            pBiDi->reordering_ਬਹੁਲਕ == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL);
    int32_t ਆਖਰੀ_Arabic_Pos=-1;
    int32_t control_ਗਿਣਤੀ=0;
    UBool removeBiDiControls =
        static_cast<UBool>(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS);

    ਗਿਣਤੀ State {
         NOT_SEEKING_STRONG,            /* 0: not contextual paraLevel, not after FSI */
         SEEKING_STRONG_FOR_PARA,       /* 1: looking for first strong char in para */
         SEEKING_STRONG_FOR_FSI,        /* 2: looking for first strong after FSI */
         LOOKING_FOR_PDI                /* 3: found strong after FSI, looking for PDI */
    };
    State state;
    DirProp ਆਖਰੀ_Strong=ON;              /* for default level & inverse BiDi */
    /* The following stacks are used to manage isolate sequences. Those
       sequences may be nested, but obviously never more deeply than the
       maximum explicit embedding level.
       lastStack is the index of the last used entry in the stack. A value of -1
       means that there is no open isolate sequence.
       lastStack is reset to -1 on paragraph boundaries. */
    /* The following stack contains the position of the initiator of
       each open isolate sequence */
    int32_t isolate_ਸ਼ੁਰੂ_ਢੇਰ[UBIDI_MAX_EXPLICIT_LEVEL+1];
    /* The following stack contains the last known state before
       encountering the initiator of an isolate sequence */
    State  ਪਿਛਲਾ_State_ਢੇਰ[UBIDI_MAX_EXPLICIT_LEVEL+1];
    int32_t ਢੇਰ_ਆਖਰੀ=-1;

    ਜੇ(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING)
        pBiDi->length=0;
    defaultParaLevel=pBiDi->paraLevel&1;
    ਜੇ(isDefaultLevel) {
        pBiDi->paras[0].level=defaultParaLevel;
        ਆਖਰੀ_Strong=defaultParaLevel;
        ਜੇ(pBiDi->pro_ਲੰਮਾਈ>0 &&                    /* there is a prologue */
           (dirProp=ਪਹਿਲਾ_L_R_AL(pBiDi))!=ON) {  /* with a strong character */
            ਜੇ(dirProp==L)
                pBiDi->paras[0].level=0;    /* set the default para level */
            ਨਹੀਂਤਾਂ
                pBiDi->paras[0].level=1;    /* set the default para level */
            state=NOT_SEEKING_STRONG;
        } ਨਹੀਂਤਾਂ {
            state=SEEKING_STRONG_FOR_PARA;
        }
    } ਨਹੀਂਤਾਂ {
        pBiDi->paras[0].level=pBiDi->paraLevel;
        state=NOT_SEEKING_STRONG;
    }
    /* count paragraphs and determine the paragraph level (P2..P3) */
    /*
     * see comment in ubidi.h:
     * the UBIDI_DEFAULT_XXX values are designed so that
     * their bit 0 alone yields the intended default
     */
    ਲਈ( /* i=0 above */ ; i<original_ਲੰਮਾਈ; ) {
        /* i is incremented by U16_NEXT */
        U16_ਅਗਲਾ(ਪਾਠ, i, original_ਲੰਮਾਈ, uchar);
        flags|=DIRPROP_FLAG(dirProp=(DirProp)ubidi_getCustomizedClass(pBiDi, uchar));
        dirProps[i-1]=dirProp;
        ਜੇ(uchar>0xffff) {  /* set the lead surrogate's property to BN */
            flags|=DIRPROP_FLAG(BN);
            dirProps[i-2]=BN;
        }
        ਜੇ(removeBiDiControls && IS_BIDI_CONTROL_CHAR(uchar))
            control_ਗਿਣਤੀ++;
        ਜੇ(dirProp==L) {
            ਜੇ(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_ਗਿਣਤੀ-1].level=0;
                state=NOT_SEEKING_STRONG;
            }
            ਨਹੀਂਤਾਂ ਜੇ(state==SEEKING_STRONG_FOR_FSI) {
                ਜੇ(ਢੇਰ_ਆਖਰੀ<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
                state=LOOKING_FOR_PDI;
            }
            ਆਖਰੀ_Strong=L;
            ਜਾਰੀ;
        }
        ਜੇ(dirProp==R || dirProp==AL) {
            ਜੇ(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_ਗਿਣਤੀ-1].level=1;
                state=NOT_SEEKING_STRONG;
            }
            ਨਹੀਂਤਾਂ ਜੇ(state==SEEKING_STRONG_FOR_FSI) {
                ਜੇ(ਢੇਰ_ਆਖਰੀ<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    dirProps[isolate_ਸ਼ੁਰੂ_ਢੇਰ[ਢੇਰ_ਆਖਰੀ]]=RLI;
                    flags|=DIRPROP_FLAG(RLI);
                }
                state=LOOKING_FOR_PDI;
            }
            ਆਖਰੀ_Strong=R;
            ਜੇ(dirProp==AL)
                ਆਖਰੀ_Arabic_Pos=i-1;
            ਜਾਰੀ;
        }
        ਜੇ(dirProp>=FSI && dirProp<=RLI) {  /* FSI, LRI or RLI */
            ਢੇਰ_ਆਖਰੀ++;
            ਜੇ(ਢੇਰ_ਆਖਰੀ<=UBIDI_MAX_EXPLICIT_LEVEL) {
                isolate_ਸ਼ੁਰੂ_ਢੇਰ[ਢੇਰ_ਆਖਰੀ]=i-1;
                ਪਿਛਲਾ_State_ਢੇਰ[ਢੇਰ_ਆਖਰੀ]=state;
            }
            ਜੇ(dirProp==FSI) {
                dirProps[i-1]=LRI;      /* default if no strong char */
                state=SEEKING_STRONG_FOR_FSI;
            }
            ਨਹੀਂਤਾਂ
                state=LOOKING_FOR_PDI;
            ਜਾਰੀ;
        }
        ਜੇ(dirProp==PDI) {
            ਜੇ(state==SEEKING_STRONG_FOR_FSI) {
                ਜੇ(ਢੇਰ_ਆਖਰੀ<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
            }
            ਜੇ(ਢੇਰ_ਆਖਰੀ>=0) {
                ਜੇ(ਢੇਰ_ਆਖਰੀ<=UBIDI_MAX_EXPLICIT_LEVEL)
                    state=ਪਿਛਲਾ_State_ਢੇਰ[ਢੇਰ_ਆਖਰੀ];
                ਢੇਰ_ਆਖਰੀ--;
            }
            ਜਾਰੀ;
        }
        ਜੇ(dirProp==B) {
            ਜੇ(i<original_ਲੰਮਾਈ && uchar==CR && ਪਾਠ[i]==LF) /* do nothing on the CR */
                ਜਾਰੀ;
            pBiDi->paras[pBiDi->para_ਗਿਣਤੀ-1].ਹੱਦਬੰਦੀ=i;
            ਜੇ(isDefaultLevelInverse && ਆਖਰੀ_Strong==R)
                pBiDi->paras[pBiDi->para_ਗਿਣਤੀ-1].level=1;
            ਜੇ(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
                /* When streaming, we only process whole paragraphs
                   thus some updates are only done on paragraph boundaries */
                pBiDi->length=i;        /* i is index to next character */
                pBiDi->control_ਗਿਣਤੀ=control_ਗਿਣਤੀ;
            }
            ਜੇ(i<original_ਲੰਮਾਈ) {              /* B not last char in text */
                pBiDi->para_ਗਿਣਤੀ++;
                ਜੇ(check_Para_ਗਿਣਤੀ(pBiDi)==ਝੂਠ)    /* not enough memory for a new para entry */
                    ਵਾਪਸ ਝੂਠ;
                ਜੇ(isDefaultLevel) {
                    pBiDi->paras[pBiDi->para_ਗਿਣਤੀ-1].level=defaultParaLevel;
                    state=SEEKING_STRONG_FOR_PARA;
                    ਆਖਰੀ_Strong=defaultParaLevel;
                } ਨਹੀਂਤਾਂ {
                    pBiDi->paras[pBiDi->para_ਗਿਣਤੀ-1].level=pBiDi->paraLevel;
                    state=NOT_SEEKING_STRONG;
                }
                ਢੇਰ_ਆਖਰੀ=-1;
            }
            ਜਾਰੀ;
        }
    }
    /* Ignore still open isolate sequences with overflow */
    ਜੇ(ਢੇਰ_ਆਖਰੀ>UBIDI_MAX_EXPLICIT_LEVEL) {
        ਢੇਰ_ਆਖਰੀ=UBIDI_MAX_EXPLICIT_LEVEL;
        state=SEEKING_STRONG_FOR_FSI;   /* to be on the safe side */
    }
    /* Resolve direction of still unresolved open FSI sequences */
    ਜਦੋਂ(ਢੇਰ_ਆਖਰੀ>=0) {
        ਜੇ(state==SEEKING_STRONG_FOR_FSI) {
            /* no need for next statement, already set by default */
            /* dirProps[isolateStartStack[stackLast]]=LRI; */
            flags|=DIRPROP_FLAG(LRI);
            ਰੋਕੋ;
        }
        state=ਪਿਛਲਾ_State_ਢੇਰ[ਢੇਰ_ਆਖਰੀ];
        ਢੇਰ_ਆਖਰੀ--;
    }
    /* When streaming, ignore text after the last paragraph separator */
    ਜੇ(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
        ਜੇ(pBiDi->length<original_ਲੰਮਾਈ)
            pBiDi->para_ਗਿਣਤੀ--;
    } ਨਹੀਂਤਾਂ {
        pBiDi->paras[pBiDi->para_ਗਿਣਤੀ-1].ਹੱਦਬੰਦੀ=original_ਲੰਮਾਈ;
        pBiDi->control_ਗਿਣਤੀ=control_ਗਿਣਤੀ;
    }
    /* For inverse bidi, default para direction is RTL if there is
       a strong R or AL at either end of the paragraph */
    ਜੇ(isDefaultLevelInverse && ਆਖਰੀ_Strong==R) {
        pBiDi->paras[pBiDi->para_ਗਿਣਤੀ-1].level=1;
    }
    ਜੇ(isDefaultLevel) {
        pBiDi->paraLevel=static_cast<UBiDiLevel>(pBiDi->paras[0].level);
    }
    /* The following is needed to resolve the text direction for default level
       paragraphs containing no strong character */
    ਲਈ(i=0; i<pBiDi->para_ਗਿਣਤੀ; i++)
        flags|=DIRPROP_FLAG_LR(pBiDi->paras[i].level);

    ਜੇ(pBiDi->ਕ੍ਰਮ_Paragraphs_LTR && (flags&DIRPROP_FLAG(B))) {
        flags|=DIRPROP_FLAG(L);
    }
    pBiDi->flags=flags;
    pBiDi->ਆਖਰੀ_Arabic_Pos=ਆਖਰੀ_Arabic_Pos;
    ਵਾਪਸ ਸੱਚ;
}

/* determine the paragraph level at position index */
U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_ਸੂਚਕਾਂਕ(ਅਚਲ UBiDi *pBiDi, int32_t pindex) {
    int32_t i;
    ਲਈ(i=0; i<pBiDi->para_ਗਿਣਤੀ; i++)
        ਜੇ(pindex<pBiDi->paras[i].ਹੱਦਬੰਦੀ)
            ਰੋਕੋ;
    ਜੇ(i>=pBiDi->para_ਗਿਣਤੀ)
        i=pBiDi->para_ਗਿਣਤੀ-1;
    ਵਾਪਸ (UBiDiLevel)(pBiDi->paras[i].level);
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

ਸਥਿਰ ਖਾਲੀ
bracketInit(UBiDi *pBiDi, Bracket_ਅੰਕੜੇ *bd) {
    bd->pBiDi=pBiDi;
    bd->iso_ਲੜੀਵਾਰ_ਆਖਰੀ=0;
    bd->isoRuns[0].ਸ਼ੁਰੂ=0;
    bd->isoRuns[0].ਹੱਦਬੰਦੀ=0;
    bd->isoRuns[0].level=GET_PARALEVEL(pBiDi, 0);
    UBiDiLevel t = GET_PARALEVEL(pBiDi, 0) & 1;
    bd->isoRuns[0].ਆਖਰੀ_Strong = bd->isoRuns[0].ਆਖਰੀ_ਅਧਾਰ = t;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(t);
    bd->isoRuns[0].contextPos=0;
    ਜੇ(pBiDi->openings_ਸਮ੍ਰਿਤੀ) {
        bd->openings=pBiDi->openings_ਸਮ੍ਰਿਤੀ;
        bd->openings_ਗਿਣਤੀ=pBiDi->openings_ਮਾਪ / ਆਕਾਰ(Opening);
    } ਨਹੀਂਤਾਂ {
        bd->openings=bd->simpleOpenings;
        bd->openings_ਗਿਣਤੀ=SIMPLE_OPENINGS_ਗਿਣਤੀ;
    }
    bd->isNumbersSpecial=bd->pBiDi->reordering_ਬਹੁਲਕ==UBIDI_REORDER_NUMBERS_SPECIAL ||
                         bd->pBiDi->reordering_ਬਹੁਲਕ==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL;
}

/* paragraph boundary */
ਸਥਿਰ ਖਾਲੀ
bracketProcessB(Bracket_ਅੰਕੜੇ *bd, UBiDiLevel level) {
    bd->iso_ਲੜੀਵਾਰ_ਆਖਰੀ=0;
    bd->isoRuns[0].ਹੱਦਬੰਦੀ=0;
    bd->isoRuns[0].level=level;
    bd->isoRuns[0].ਆਖਰੀ_Strong=bd->isoRuns[0].ਆਖਰੀ_ਅਧਾਰ=level&1;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(level & 1);
    bd->isoRuns[0].contextPos=0;
}

/* LRE, LRO, RLE, RLO, PDF */
ਸਥਿਰ ਖਾਲੀ
bracketProcessBoundary(Bracket_ਅੰਕੜੇ *bd, int32_t ਆਖਰੀ_Cc_Pos,
                       UBiDiLevel contextLevel, UBiDiLevel embeddingLevel) {
    Iso_ਲੜੀਵਾਰ *p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ=&bd->isoRuns[bd->iso_ਲੜੀਵਾਰ_ਆਖਰੀ];
    DirProp *dirProps=bd->pBiDi->dirProps;
    ਜੇ(DIRPROP_FLAG(dirProps[ਆਖਰੀ_Cc_Pos])&ਨਕਾਬ_ISO)  /* after an isolate */
        ਵਾਪਸ;
    ਜੇ(NO_OVERRIDE(embeddingLevel)>NO_OVERRIDE(contextLevel))   /* not a PDF */
        contextLevel=embeddingLevel;
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ=p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਸ਼ੁਰੂ;
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->level=embeddingLevel;
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_Strong=p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_ਅਧਾਰ=contextLevel&1;
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextDir = static_cast<UBiDiDirection>(contextLevel & 1);
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextPos = static_cast<UBiDiDirection>(ਆਖਰੀ_Cc_Pos);
}

/* LRI or RLI */
ਸਥਿਰ ਖਾਲੀ
bracketProcessLRI_RLI(Bracket_ਅੰਕੜੇ *bd, UBiDiLevel level) {
    Iso_ਲੜੀਵਾਰ *p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ=&bd->isoRuns[bd->iso_ਲੜੀਵਾਰ_ਆਖਰੀ];
    int16_t ਆਖਰੀ_ਹੱਦਬੰਦੀ;
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_ਅਧਾਰ=ON;
    ਆਖਰੀ_ਹੱਦਬੰਦੀ=p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ;
    bd->iso_ਲੜੀਵਾਰ_ਆਖਰੀ++;
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ++;
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਸ਼ੁਰੂ=p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ=ਆਖਰੀ_ਹੱਦਬੰਦੀ;
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->level=level;
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_Strong=p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_ਅਧਾਰ=level&1;
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextDir = static_cast<UBiDiDirection>(level & 1);
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextPos=0;
}

/* PDI */
ਸਥਿਰ ਖਾਲੀ
bracketProcessPDI(Bracket_ਅੰਕੜੇ *bd) {
    Iso_ਲੜੀਵਾਰ *p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ;
    bd->iso_ਲੜੀਵਾਰ_ਆਖਰੀ--;
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ=&bd->isoRuns[bd->iso_ਲੜੀਵਾਰ_ਆਖਰੀ];
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_ਅਧਾਰ=ON;
}

/* newly found opening bracket: create an openings entry */
ਸਥਿਰ UBool                            /* return true if success */
bracket_ਜੋੜੋ_Opening(Bracket_ਅੰਕੜੇ *bd, char16_t ਮੇਲ, int32_t position) {
    Iso_ਲੜੀਵਾਰ *p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ=&bd->isoRuns[bd->iso_ਲੜੀਵਾਰ_ਆਖਰੀ];
    Opening *pOpening;
    ਜੇ(p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ>=bd->openings_ਗਿਣਤੀ) {  /* no available new entry */
        UBiDi *pBiDi=bd->pBiDi;
        ਜੇ(!get_Initial_Openings_ਸਮ੍ਰਿਤੀ(pBiDi, p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ * 2))
            ਵਾਪਸ ਝੂਠ;
        ਜੇ(bd->openings==bd->simpleOpenings)
            uprv_memcpy(pBiDi->openings_ਸਮ੍ਰਿਤੀ, bd->simpleOpenings,
                        SIMPLE_OPENINGS_ਗਿਣਤੀ * ਆਕਾਰ(Opening));
        bd->openings=pBiDi->openings_ਸਮ੍ਰਿਤੀ;     /* may have changed */
        bd->openings_ਗਿਣਤੀ=pBiDi->openings_ਮਾਪ / ਆਕਾਰ(Opening);
    }
    pOpening=&bd->openings[p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ];
    pOpening->position=position;
    pOpening->ਮੇਲ=ਮੇਲ;
    pOpening->contextDir=p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextDir;
    pOpening->contextPos=p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextPos;
    pOpening->flags=0;
    p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ++;
    ਵਾਪਸ ਸੱਚ;
}

/* change N0c1 to N0c2 when a preceding bracket is assigned the embedding level */
ਸਥਿਰ ਖਾਲੀ
fixN0c(Bracket_ਅੰਕੜੇ *bd, int32_t opening_ਸੂਚਕਾਂਕ, int32_t newPropPosition, DirProp newProp) {
    /* This function calls itself recursively */
    Iso_ਲੜੀਵਾਰ *p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ=&bd->isoRuns[bd->iso_ਲੜੀਵਾਰ_ਆਖਰੀ];
    Opening *qOpening;
    DirProp *dirProps=bd->pBiDi->dirProps;
    int32_t k, openingPosition, closingPosition;
    ਲਈ(k=opening_ਸੂਚਕਾਂਕ+1, qOpening=&bd->openings[k]; k<p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ; k++, qOpening++) {
        ਜੇ(qOpening->ਮੇਲ>=0)      /* not an N0c match */
            ਜਾਰੀ;
        ਜੇ(newPropPosition<qOpening->contextPos)
            ਰੋਕੋ;
        ਜੇ(newPropPosition>=qOpening->position)
            ਜਾਰੀ;
        ਜੇ(newProp==qOpening->contextDir)
            ਰੋਕੋ;
        openingPosition=qOpening->position;
        dirProps[openingPosition]=newProp;
        closingPosition=-(qOpening->ਮੇਲ);
        dirProps[closingPosition]=newProp;
        qOpening->ਮੇਲ=0;                      /* prevent further changes */
        fixN0c(bd, k, openingPosition, newProp);
        fixN0c(bd, k, closingPosition, newProp);
    }
}

/* process closing bracket */
ਸਥਿਰ DirProp              /* return L or R if N0b or N0c, ON if N0d */
bracketProcessClosing(Bracket_ਅੰਕੜੇ *bd, int32_t openIdx, int32_t position) {
    Iso_ਲੜੀਵਾਰ *p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ=&bd->isoRuns[bd->iso_ਲੜੀਵਾਰ_ਆਖਰੀ];
    Opening *pOpening, *qOpening;
    UBiDiDirection direction;
    UBool stable;
    DirProp newProp;
    pOpening=&bd->openings[openIdx];
    direction = static_cast<UBiDiDirection>(p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->level & 1);
    stable=ਸੱਚ;            /* assume stable until proved otherwise */

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

    ਜੇ((direction==0 && pOpening->flags&ਮਿਲਿਆ_L) ||
       (direction==1 && pOpening->flags&ਮਿਲਿਆ_R)) {                         /* N0b */
        newProp=static_cast<DirProp>(direction);
    }
    ਨਹੀਂਤਾਂ ਜੇ(pOpening->flags&(ਮਿਲਿਆ_L|ਮਿਲਿਆ_R)) {                            /* N0c */
        /* it is stable if there is no containing pair or in
           conditions too complicated and not worth checking */
        stable=(openIdx==p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਸ਼ੁਰੂ);
        ਜੇ(direction!=pOpening->contextDir)
            newProp= static_cast<DirProp>(pOpening->contextDir);           /* N0c1 */
        ਨਹੀਂਤਾਂ
            newProp= static_cast<DirProp>(direction);                      /* N0c2 */
    } ਨਹੀਂਤਾਂ {
        /* forget this and any brackets nested within this pair */
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ= static_cast<uint16_t>(openIdx);
        ਵਾਪਸ ON;                                                          /* N0d */
    }
    bd->pBiDi->dirProps[pOpening->position]=newProp;
    bd->pBiDi->dirProps[position]=newProp;
    /* Update nested N0c pairs that may be affected */
    fixN0c(bd, openIdx, pOpening->position, newProp);
    ਜੇ(stable) {
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ= static_cast<uint16_t>(openIdx); /* forget any brackets nested within this pair */
        /* remove lower located synonyms if any */
        ਜਦੋਂ(p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ>p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਸ਼ੁਰੂ &&
              bd->openings[p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ-1].position==pOpening->position)
            p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ--;
    } ਨਹੀਂਤਾਂ {
        int32_t k;
        pOpening->ਮੇਲ=-position;
        /* neutralize lower located synonyms if any */
        k=openIdx-1;
        ਜਦੋਂ(k>=p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਸ਼ੁਰੂ &&
              bd->openings[k].position==pOpening->position)
            bd->openings[k--].ਮੇਲ=0;
        /* neutralize any unmatched opening between the current pair;
           this will also neutralize higher located synonyms if any */
        ਲਈ(k=openIdx+1; k<p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ; k++) {
            qOpening=&bd->openings[k];
            ਜੇ(qOpening->position>=position)
                ਰੋਕੋ;
            ਜੇ(qOpening->ਮੇਲ>0)
                qOpening->ਮੇਲ=0;
        }
    }
    ਵਾਪਸ newProp;
}

/* handle strong characters, digits and candidates for closing brackets */
ਸਥਿਰ UBool                            /* return true if success */
bracketProcessChar(Bracket_ਅੰਕੜੇ *bd, int32_t position) {
    Iso_ਲੜੀਵਾਰ *p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ=&bd->isoRuns[bd->iso_ਲੜੀਵਾਰ_ਆਖਰੀ];
    DirProp *dirProps, dirProp, newProp;
    UBiDiLevel level;
    dirProps=bd->pBiDi->dirProps;
    dirProp=dirProps[position];
    ਜੇ(dirProp==ON) {
        char16_t c, ਮੇਲ;
        int32_t idx;
        /* First see if it is a matching closing bracket. Hopefully, this is
           more efficient than checking if it is a closing bracket at all */
        c=bd->pBiDi->ਪਾਠ[position];
        ਲਈ(idx=p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ-1; idx>=p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਸ਼ੁਰੂ; idx--) {
            ਜੇ(bd->openings[idx].ਮੇਲ!=c)
                ਜਾਰੀ;
            /* We have a match */
            newProp=bracketProcessClosing(bd, idx, position);
            ਜੇ(newProp==ON) {           /* N0d */
                c=0;        /* prevent handling as an opening */
                ਰੋਕੋ;
            }
            p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_ਅਧਾਰ=ON;
            p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextDir = static_cast<UBiDiDirection>(newProp);
            p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextPos=position;
            level=bd->pBiDi->ਪੱਧਰ[position];
            ਜੇ(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
                uint16_t flag;
                int32_t i;
                newProp=level&1;
                p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_Strong=newProp;
                flag=DIRPROP_FLAG(newProp);
                ਲਈ(i=p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਸ਼ੁਰੂ; i<idx; i++)
                    bd->openings[i].flags|=flag;
                /* matching brackets are not overridden by LRO/RLO */
                bd->pBiDi->ਪੱਧਰ[position]&=~UBIDI_LEVEL_OVERRIDE;
            }
            /* matching brackets are not overridden by LRO/RLO */
            bd->pBiDi->ਪੱਧਰ[bd->openings[idx].position]&=~UBIDI_LEVEL_OVERRIDE;
            ਵਾਪਸ ਸੱਚ;
        }
        /* We get here only if the ON character is not a matching closing
           bracket or it is a case of N0d */
        /* Now see if it is an opening bracket */
        ਜੇ(c)
            ਮੇਲ= static_cast<char16_t>(u_getBidiPairedBracket(c));    /* get the matching char */
        ਨਹੀਂਤਾਂ
            ਮੇਲ=0;
        ਜੇ(ਮੇਲ!=c &&                  /* has a matching char */
           ubidi_getPairedBracketType(c)==U_BPT_OPEN) { /* opening bracket */
            /* special case: process synonyms
               create an opening entry for each synonym */
            ਜੇ(ਮੇਲ==0x232A) {     /* RIGHT-POINTING ANGLE BRACKET */
                ਜੇ(!bracket_ਜੋੜੋ_Opening(bd, 0x3009, position))
                    ਵਾਪਸ ਝੂਠ;
            }
            ਨਹੀਂਤਾਂ ਜੇ(ਮੇਲ==0x3009) {         /* RIGHT ANGLE BRACKET */
                ਜੇ(!bracket_ਜੋੜੋ_Opening(bd, 0x232A, position))
                    ਵਾਪਸ ਝੂਠ;
            }
            ਜੇ(!bracket_ਜੋੜੋ_Opening(bd, ਮੇਲ, position))
                ਵਾਪਸ ਝੂਠ;
        }
    }
    level=bd->pBiDi->ਪੱਧਰ[position];
    ਜੇ(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
        newProp=level&1;
        ਜੇ(dirProp!=S && dirProp!=WS && dirProp!=ON)
            dirProps[position]=newProp;
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_ਅਧਾਰ=newProp;
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_Strong=newProp;
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextDir = static_cast<UBiDiDirection>(newProp);
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextPos=position;
    }
    ਨਹੀਂਤਾਂ ਜੇ(dirProp<=R || dirProp==AL) {
        newProp= static_cast<DirProp>(DIR_FROM_STRONG(dirProp));
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_ਅਧਾਰ=dirProp;
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_Strong=dirProp;
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextDir = static_cast<UBiDiDirection>(newProp);
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextPos=position;
    }
    ਨਹੀਂਤਾਂ ਜੇ(dirProp==EN) {
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_ਅਧਾਰ=EN;
        ਜੇ(p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_Strong==L) {
            newProp=L;                  /* W7 */
            ਜੇ(!bd->isNumbersSpecial)
                dirProps[position]=ENL;
            p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextDir = static_cast<UBiDiDirection>(L);
            p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextPos=position;
        }
        ਨਹੀਂਤਾਂ {
            newProp=R;                  /* N0 */
            ਜੇ(p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_Strong==AL)
                dirProps[position]=AN;  /* W2 */
            ਨਹੀਂਤਾਂ
                dirProps[position]=ENR;
            p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextDir = static_cast<UBiDiDirection>(R);
            p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextPos=position;
        }
    }
    ਨਹੀਂਤਾਂ ਜੇ(dirProp==AN) {
        newProp=R;                      /* N0 */
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_ਅਧਾਰ=AN;
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextDir = static_cast<UBiDiDirection>(R);
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->contextPos=position;
    }
    ਨਹੀਂਤਾਂ ਜੇ(dirProp==NSM) {
        /* if the last real char was ON, change NSM to ON so that it
           will stay ON even if the last real char is a bracket which
           may be changed to L or R */
        newProp=p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_ਅਧਾਰ;
        ਜੇ(newProp==ON)
            dirProps[position]=newProp;
    }
    ਨਹੀਂਤਾਂ {
        newProp=dirProp;
        p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਆਖਰੀ_ਅਧਾਰ=dirProp;
    }
    ਜੇ(newProp<=R || newProp==AL) {
        int32_t i;
        uint16_t flag=DIRPROP_FLAG(DIR_FROM_STRONG(newProp));
        ਲਈ(i=p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਸ਼ੁਰੂ; i<p_ਆਖਰੀ_Iso_ਲੜੀਵਾਰ->ਹੱਦਬੰਦੀ; i++)
            ਜੇ(position>bd->openings[i].position)
                bd->openings[i].flags|=flag;
    }
    ਵਾਪਸ ਸੱਚ;
}

/* perform (X1)..(X9) ------------------------------------------------------- */

/* determine if the text is mixed-directional or single-directional */
ਸਥਿਰ UBiDiDirection
directionFromFlags(UBiDi *pBiDi) {
    Flags flags=pBiDi->flags;
    /* if the text contains AN and neutrals, then some neutrals may become RTL */
    ਜੇ(!(flags&ਨਕਾਬ_RTL || ((flags&DIRPROP_FLAG(AN)) && (flags&ਨਕਾਬ_POSSIBLE_N)))) {
        ਵਾਪਸ UBIDI_LTR;
    } ਨਹੀਂਤਾਂ ਜੇ(!(flags&ਨਕਾਬ_LTR)) {
        ਵਾਪਸ UBIDI_RTL;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ UBIDI_MIXED;
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
ਸਥਿਰ UBiDiDirection
resolve_Explicit_ਪੱਧਰ(UBiDi *pBiDi, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *ਪੱਧਰ=pBiDi->ਪੱਧਰ;
    ਅਚਲ char16_t *ਪਾਠ=pBiDi->ਪਾਠ;

    int32_t i=0, length=pBiDi->length;
    Flags flags=pBiDi->flags;       /* collect all directionalities in the text */
    DirProp dirProp;
    UBiDiLevel level=GET_PARALEVEL(pBiDi, 0);
    UBiDiDirection direction;
    pBiDi->isolate_ਗਿਣਤੀ=0;

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) { ਵਾਪਸ UBIDI_LTR; }

    /* determine if the text is mixed-directional or single-directional */
    direction=directionFromFlags(pBiDi);

    /* we may not need to resolve any explicit levels */
    ਜੇ((direction!=UBIDI_MIXED)) {
        /* not mixed directionality: levels don't matter - trailingWSStart will be 0 */
        ਵਾਪਸ direction;
    }
    ਜੇ(pBiDi->reordering_ਬਹੁਲਕ > UBIDI_REORDER_ਆਖਰੀ_LOGICAL_ਮੰਜ਼ਿਲ_VISUAL) {
        /* inverse BiDi: mixed, but all characters are at the same embedding level */
        /* set all levels to the paragraph level */
        int32_t para_ਸੂਚਕਾਂਕ, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ;
        ਲਈ(para_ਸੂਚਕਾਂਕ=0; para_ਸੂਚਕਾਂਕ<pBiDi->para_ਗਿਣਤੀ; para_ਸੂਚਕਾਂਕ++) {
            ਜੇ(para_ਸੂਚਕਾਂਕ==0)
                ਸ਼ੁਰੂ=0;
            ਨਹੀਂਤਾਂ
                ਸ਼ੁਰੂ=pBiDi->paras[para_ਸੂਚਕਾਂਕ-1].ਹੱਦਬੰਦੀ;
            ਹੱਦਬੰਦੀ=pBiDi->paras[para_ਸੂਚਕਾਂਕ].ਹੱਦਬੰਦੀ;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_ਸੂਚਕਾਂਕ].level);
            ਲਈ(i=ਸ਼ੁਰੂ; i<ਹੱਦਬੰਦੀ; i++)
                ਪੱਧਰ[i]=level;
        }
        ਵਾਪਸ direction;   /* no bracket matching for inverse BiDi */
    }
    ਜੇ(!(flags&(ਨਕਾਬ_EXPLICIT|ਨਕਾਬ_ISO))) {
        /* no embeddings, set all levels to the paragraph level */
        /* we still have to perform bracket matching */
        int32_t para_ਸੂਚਕਾਂਕ, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ;
        Bracket_ਅੰਕੜੇ bracket_ਅੰਕੜੇ;
        bracketInit(pBiDi, &bracket_ਅੰਕੜੇ);
        ਲਈ(para_ਸੂਚਕਾਂਕ=0; para_ਸੂਚਕਾਂਕ<pBiDi->para_ਗਿਣਤੀ; para_ਸੂਚਕਾਂਕ++) {
            ਜੇ(para_ਸੂਚਕਾਂਕ==0)
                ਸ਼ੁਰੂ=0;
            ਨਹੀਂਤਾਂ
                ਸ਼ੁਰੂ=pBiDi->paras[para_ਸੂਚਕਾਂਕ-1].ਹੱਦਬੰਦੀ;
            ਹੱਦਬੰਦੀ=pBiDi->paras[para_ਸੂਚਕਾਂਕ].ਹੱਦਬੰਦੀ;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_ਸੂਚਕਾਂਕ].level);
            ਲਈ(i=ਸ਼ੁਰੂ; i<ਹੱਦਬੰਦੀ; i++) {
                ਪੱਧਰ[i]=level;
                dirProp=dirProps[i];
                ਜੇ(dirProp==BN)
                    ਜਾਰੀ;
                ਜੇ(dirProp==B) {
                    ਜੇ((i+1)<length) {
                        ਜੇ(ਪਾਠ[i]==CR && ਪਾਠ[i+1]==LF)
                            ਜਾਰੀ;   /* skip CR when followed by LF */
                        bracketProcessB(&bracket_ਅੰਕੜੇ, level);
                    }
                    ਜਾਰੀ;
                }
                ਜੇ(!bracketProcessChar(&bracket_ਅੰਕੜੇ, i)) {
                    *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    ਵਾਪਸ UBIDI_LTR;
                }
            }
        }
        ਵਾਪਸ direction;
    }
    {
        /* continue to perform (Xn) */

        /* (X1) level is set for all codes, embeddingLevel keeps track of the push/pop operations */
        /* both variables may carry the UBIDI_LEVEL_OVERRIDE flag to indicate the override status */
        UBiDiLevel embeddingLevel=level, newLevel;
        UBiDiLevel ਪਿਛਲਾ_Level=level;     /* previous level for regular (not CC) characters */
        int32_t ਆਖਰੀ_Cc_Pos=0;                /* index of last effective LRx,RLx, PDx */

        /* The following stack remembers the embedding level and the ISOLATE flag of level runs.
           stackLast points to its current entry. */
        uint16_t ਢੇਰ_5[UBIDI_MAX_EXPLICIT_LEVEL+2];   /* we never push anything >=UBIDI_MAX_EXPLICIT_LEVEL
                                                        but we need one more entry as base */
        uint32_t ਢੇਰ_ਆਖਰੀ=0;
        int32_t ਉਛਾਲ_Isolate_ਗਿਣਤੀ=0;
        int32_t ਉਛਾਲ_Embedding_ਗਿਣਤੀ=0;
        int32_t valid_Isolate_ਗਿਣਤੀ=0;
        Bracket_ਅੰਕੜੇ bracket_ਅੰਕੜੇ;
        bracketInit(pBiDi, &bracket_ਅੰਕੜੇ);
        ਢੇਰ_5[0]=level;     /* initialize base entry to para level, no override, no isolate */

        /* recalculate the flags */
        flags=0;

        ਲਈ(i=0; i<length; ++i) {
            dirProp=dirProps[i];
            ਚੋਣ(dirProp) {
            ਹਾਲਤ LRE:
            ਹਾਲਤ RLE:
            ਹਾਲਤ LRO:
            ਹਾਲਤ RLO:
                /* (X2, X3, X4, X5) */
                flags|=DIRPROP_FLAG(BN);
                ਪੱਧਰ[i]=ਪਿਛਲਾ_Level;
                ਜੇ (dirProp==LRE || dirProp==LRO)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                ਨਹੀਂਤਾਂ
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                ਜੇ(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && ਉਛਾਲ_Isolate_ਗਿਣਤੀ==0 &&
                                                         ਉਛਾਲ_Embedding_ਗਿਣਤੀ==0) {
                    ਆਖਰੀ_Cc_Pos=i;
                    embeddingLevel=newLevel;
                    ਜੇ(dirProp==LRO || dirProp==RLO)
                        embeddingLevel|=UBIDI_LEVEL_OVERRIDE;
                    ਢੇਰ_ਆਖਰੀ++;
                    ਢੇਰ_5[ਢੇਰ_ਆਖਰੀ]=embeddingLevel;
                    /* we don't need to set UBIDI_LEVEL_OVERRIDE off for LRE and RLE
                       since this has already been done for newLevel which is
                       the source for embeddingLevel.
                     */
                } ਨਹੀਂਤਾਂ {
                    ਜੇ(ਉਛਾਲ_Isolate_ਗਿਣਤੀ==0)
                        ਉਛਾਲ_Embedding_ਗਿਣਤੀ++;
                }
                ਰੋਕੋ;
            ਹਾਲਤ PDF:
                /* (X7) */
                flags|=DIRPROP_FLAG(BN);
                ਪੱਧਰ[i]=ਪਿਛਲਾ_Level;
                /* handle all the overflow cases first */
                ਜੇ(ਉਛਾਲ_Isolate_ਗਿਣਤੀ) {
                    ਰੋਕੋ;
                }
                ਜੇ(ਉਛਾਲ_Embedding_ਗਿਣਤੀ) {
                    ਉਛਾਲ_Embedding_ਗਿਣਤੀ--;
                    ਰੋਕੋ;
                }
                ਜੇ(ਢੇਰ_ਆਖਰੀ>0 && ਢੇਰ_5[ਢੇਰ_ਆਖਰੀ]<ISOLATE) {   /* not an isolate entry */
                    ਆਖਰੀ_Cc_Pos=i;
                    ਢੇਰ_ਆਖਰੀ--;
                    embeddingLevel = static_cast<UBiDiLevel>(ਢੇਰ_5[ਢੇਰ_ਆਖਰੀ]);
                }
                ਰੋਕੋ;
            ਹਾਲਤ LRI:
            ਹਾਲਤ RLI:
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                ਪੱਧਰ[i]=NO_OVERRIDE(embeddingLevel);
                ਜੇ(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(ਪਿਛਲਾ_Level)) {
                    bracketProcessBoundary(&bracket_ਅੰਕੜੇ, ਆਖਰੀ_Cc_Pos,
                                           ਪਿਛਲਾ_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                ਪਿਛਲਾ_Level=embeddingLevel;
                /* (X5a, X5b) */
                ਜੇ(dirProp==LRI)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                ਨਹੀਂਤਾਂ
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                ਜੇ(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && ਉਛਾਲ_Isolate_ਗਿਣਤੀ==0 &&
                                                         ਉਛਾਲ_Embedding_ਗਿਣਤੀ==0) {
                    flags|=DIRPROP_FLAG(dirProp);
                    ਆਖਰੀ_Cc_Pos=i;
                    valid_Isolate_ਗਿਣਤੀ++;
                    ਜੇ(valid_Isolate_ਗਿਣਤੀ>pBiDi->isolate_ਗਿਣਤੀ)
                        pBiDi->isolate_ਗਿਣਤੀ=valid_Isolate_ਗਿਣਤੀ;
                    embeddingLevel=newLevel;
                    /* we can increment stackLast without checking because newLevel
                       will exceed UBIDI_MAX_EXPLICIT_LEVEL before stackLast overflows */
                    ਢੇਰ_ਆਖਰੀ++;
                    ਢੇਰ_5[ਢੇਰ_ਆਖਰੀ]=embeddingLevel+ISOLATE;
                    bracketProcessLRI_RLI(&bracket_ਅੰਕੜੇ, embeddingLevel);
                } ਨਹੀਂਤਾਂ {
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                    ਉਛਾਲ_Isolate_ਗਿਣਤੀ++;
                }
                ਰੋਕੋ;
            ਹਾਲਤ PDI:
                ਜੇ(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(ਪਿਛਲਾ_Level)) {
                    bracketProcessBoundary(&bracket_ਅੰਕੜੇ, ਆਖਰੀ_Cc_Pos,
                                           ਪਿਛਲਾ_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                /* (X6a) */
                ਜੇ(ਉਛਾਲ_Isolate_ਗਿਣਤੀ) {
                    ਉਛਾਲ_Isolate_ਗਿਣਤੀ--;
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                }
                ਨਹੀਂਤਾਂ ਜੇ(valid_Isolate_ਗਿਣਤੀ) {
                    flags|=DIRPROP_FLAG(PDI);
                    ਆਖਰੀ_Cc_Pos=i;
                    ਉਛਾਲ_Embedding_ਗਿਣਤੀ=0;
                    ਜਦੋਂ(ਢੇਰ_5[ਢੇਰ_ਆਖਰੀ]<ISOLATE) /* pop embedding entries */
                        ਢੇਰ_ਆਖਰੀ--;                /* until the last isolate entry */
                    ਢੇਰ_ਆਖਰੀ--;                    /* pop also the last isolate entry */
                    valid_Isolate_ਗਿਣਤੀ--;
                    bracketProcessPDI(&bracket_ਅੰਕੜੇ);
                } ਨਹੀਂਤਾਂ
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                embeddingLevel = static_cast<UBiDiLevel>(ਢੇਰ_5[ਢੇਰ_ਆਖਰੀ]) & ~ISOLATE;
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                ਪਿਛਲਾ_Level=embeddingLevel;
                ਪੱਧਰ[i]=NO_OVERRIDE(embeddingLevel);
                ਰੋਕੋ;
            ਹਾਲਤ B:
                flags|=DIRPROP_FLAG(B);
                ਪੱਧਰ[i]=GET_PARALEVEL(pBiDi, i);
                ਜੇ((i+1)<length) {
                    ਜੇ(ਪਾਠ[i]==CR && ਪਾਠ[i+1]==LF)
                        ਰੋਕੋ;          /* skip CR when followed by LF */
                    ਉਛਾਲ_Embedding_ਗਿਣਤੀ=ਉਛਾਲ_Isolate_ਗਿਣਤੀ=0;
                    valid_Isolate_ਗਿਣਤੀ=0;
                    ਢੇਰ_ਆਖਰੀ=0;
                    ਪਿਛਲਾ_Level=embeddingLevel=GET_PARALEVEL(pBiDi, i+1);
                    ਢੇਰ_5[0]=embeddingLevel; /* initialize base entry to para level, no override, no isolate */
                    bracketProcessB(&bracket_ਅੰਕੜੇ, embeddingLevel);
                }
                ਰੋਕੋ;
            ਹਾਲਤ BN:
                /* BN, LRE, RLE, and PDF are supposed to be removed (X9) */
                /* they will get their levels set correctly in adjustWSLevels() */
                ਪੱਧਰ[i]=ਪਿਛਲਾ_Level;
                flags|=DIRPROP_FLAG(BN);
                ਰੋਕੋ;
            ਮੂਲ:
                /* all other types are normal characters and get the "real" level */
                ਜੇ(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(ਪਿਛਲਾ_Level)) {
                    bracketProcessBoundary(&bracket_ਅੰਕੜੇ, ਆਖਰੀ_Cc_Pos,
                                           ਪਿਛਲਾ_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                    ਜੇ(embeddingLevel&UBIDI_LEVEL_OVERRIDE)
                        flags|=DIRPROP_FLAG_O(embeddingLevel);
                    ਨਹੀਂਤਾਂ
                        flags|=DIRPROP_FLAG_E(embeddingLevel);
                }
                ਪਿਛਲਾ_Level=embeddingLevel;
                ਪੱਧਰ[i]=embeddingLevel;
                ਜੇ(!bracketProcessChar(&bracket_ਅੰਕੜੇ, i))
                    ਵਾਪਸ static_cast<UBiDiDirection>(-1);
                /* the dirProp may have been changed in bracketProcessChar() */
                flags|=DIRPROP_FLAG(dirProps[i]);
                ਰੋਕੋ;
            }
        }
        ਜੇ(flags&ਨਕਾਬ_EMBEDDING)
            flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
        ਜੇ(pBiDi->ਕ੍ਰਮ_Paragraphs_LTR && (flags&DIRPROP_FLAG(B)))
            flags|=DIRPROP_FLAG(L);
        /* again, determine if the text is mixed-directional or single-directional */
        pBiDi->flags=flags;
        direction=directionFromFlags(pBiDi);
    }
    ਵਾਪਸ direction;
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
ਸਥਿਰ UBiDiDirection
check_Explicit_ਪੱਧਰ(UBiDi *pBiDi, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *ਪੱਧਰ=pBiDi->ਪੱਧਰ;
    int32_t isolate_ਗਿਣਤੀ=0;

    int32_t length=pBiDi->length;
    Flags flags=0;  /* collect all directionalities in the text */
    pBiDi->isolate_ਗਿਣਤੀ=0;

    int32_t current_Para_ਸੂਚਕਾਂਕ = 0;
    int32_t current_Para_ਹੱਦਬੰਦੀ = pBiDi->paras[0].ਹੱਦਬੰਦੀ;
    int32_t currentParaLevel = pBiDi->paraLevel;

    ਲਈ(int32_t i=0; i<length; ++i) {
        UBiDiLevel level=ਪੱਧਰ[i];
        DirProp dirProp=dirProps[i];
        ਜੇ(dirProp==LRI || dirProp==RLI) {
            isolate_ਗਿਣਤੀ++;
            ਜੇ(isolate_ਗਿਣਤੀ>pBiDi->isolate_ਗਿਣਤੀ)
                pBiDi->isolate_ਗਿਣਤੀ=isolate_ਗਿਣਤੀ;
        }
        ਨਹੀਂਤਾਂ ਜੇ(dirProp==PDI)
            isolate_ਗਿਣਤੀ--;
        ਨਹੀਂਤਾਂ ਜੇ(dirProp==B)
            isolate_ਗਿਣਤੀ=0;

        // optimized version of  int32_t currentParaLevel = GET_PARALEVEL(pBiDi, i);
        ਜੇ (pBiDi->defaultParaLevel != 0 &&
                i == current_Para_ਹੱਦਬੰਦੀ && (current_Para_ਸੂਚਕਾਂਕ + 1) < pBiDi->para_ਗਿਣਤੀ) {
            currentParaLevel = pBiDi->paras[++current_Para_ਸੂਚਕਾਂਕ].level;
            current_Para_ਹੱਦਬੰਦੀ = pBiDi->paras[current_Para_ਸੂਚਕਾਂਕ].ਹੱਦਬੰਦੀ;
        }

        UBiDiLevel overrideFlag = level & UBIDI_LEVEL_OVERRIDE;
        level &= ~UBIDI_LEVEL_OVERRIDE;
        ਜੇ (level < currentParaLevel || UBIDI_MAX_EXPLICIT_LEVEL < level) {
            ਜੇ (level == 0) {
                ਜੇ (dirProp == B) {
                    // Paragraph separators are ok with explicit level 0.
                    // Prevents reordering of paragraphs.
                } ਨਹੀਂਤਾਂ {
                    // Treat explicit level 0 as a wildcard for the paragraph level.
                    // Avoid making the caller guess what the paragraph level would be.
                    level = static_cast<UBiDiLevel>(currentParaLevel);
                    ਪੱਧਰ[i] = level | overrideFlag;
                }
            } ਨਹੀਂਤਾਂ {
                // 1 <= level < currentParaLevel or UBIDI_MAX_EXPLICIT_LEVEL < level
                /* level out of bounds */
                *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
                ਵਾਪਸ UBIDI_LTR;
            }
        }
        ਜੇ (overrideFlag != 0) {
            /* keep the override flag in levels[i] but adjust the flags */
            flags|=DIRPROP_FLAG_O(level);
        } ਨਹੀਂਤਾਂ {
            /* set the flags */
            flags|=DIRPROP_FLAG_E(level)|DIRPROP_FLAG(dirProp);
        }
    }
    ਜੇ(flags&ਨਕਾਬ_EMBEDDING)
        flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
    /* determine if the text is mixed-directional or single-directional */
    pBiDi->flags=flags;
    ਵਾਪਸ directionFromFlags(pBiDi);
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMPTABPROPS_COLUMNS 16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMPTABPROPS_RES (IMPTABPROPS_COLUMNS - 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_STATEPROPS(cell) ((cell)&0x1f)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_ACTIONPROPS(cell) ((cell)>>5)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ s(action, newState) ((uint8_t)(newState+(action<<5)))

ਸਥਿਰ ਅਚਲ uint8_t ਸਮੂਹ_Prop[] =          /* dirProp regrouped */
{
/*  L   R   EN  ES  ET  AN  CS  B   S   WS  ON  LRE LRO AL  RLE RLO PDF NSM BN  FSI LRI RLI PDI ENL ENR */
    0,  1,  2,  7,  8,  3,  9,  6,  5,  4,  4,  10, 10, 12, 10, 10, 10, 11, 10, 4,  4,  4,  4,  13, 14
};
ਗਿਣਤੀ { DirProp_L=0, DirProp_R=1, DirProp_EN=2, DirProp_AN=3, DirProp_ON=4, DirProp_S=5, DirProp_B=6 }; /* reduced dirProp */

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
ਸਥਿਰ ਅਚਲ uint8_t impTabProps[][IMPTABPROPS_COLUMNS] =
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
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ s

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMPTABLEVELS_COLUMNS (DirProp_B + 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMPTABLEVELS_RES (IMPTABLEVELS_COLUMNS - 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_STATE(cell) ((cell)&0x0f)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_ACTION(cell) ((cell)>>4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ s(action, newState) ((uint8_t)(newState+(action<<4)))

ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint8_t ImpTab[][IMPTABLEVELS_COLUMNS];
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint8_t ImpAct[];

/* FOOD FOR THOUGHT: each ImpTab should have its associated ImpAct,
 * instead of having a pair of ImpTab and a pair of ImpAct.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ImpTabPair {
    ਅਚਲ ਖਾਲੀ * pImpTab[2];
    ਅਚਲ ਖਾਲੀ * pImpAct[2];
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

ਸਥਿਰ ਅਚਲ ImpTab impTabL_DEFAULT =   /* Even paragraph level */
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
ਸਥਿਰ ਅਚਲ ImpTab impTabR_DEFAULT =   /* Odd  paragraph level */
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
ਸਥਿਰ ਅਚਲ ImpAct impAct0 = {0,1,2,3,4};
ਸਥਿਰ ਅਚਲ ImpTabPair impTab_DEFAULT = {{&impTabL_DEFAULT,
                                           &impTabR_DEFAULT},
                                          {&impAct0, &impAct0}};

ਸਥਿਰ ਅਚਲ ImpTab impTabL_NUMBERS_SPECIAL =   /* Even paragraph level */
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
ਸਥਿਰ ਅਚਲ ImpTabPair impTab_NUMBERS_SPECIAL = {{&impTabL_NUMBERS_SPECIAL,
                                                   &impTabR_DEFAULT},
                                                  {&impAct0, &impAct0}};

ਸਥਿਰ ਅਚਲ ImpTab imp_Tab_L_ਸਮੂਹ_NUMBERS_WITH_R =
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
ਸਥਿਰ ਅਚਲ ImpTab imp_Tab_R_ਸਮੂਹ_NUMBERS_WITH_R =
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
ਸਥਿਰ ਅਚਲ ImpTabPair imp_Tab_ਸਮੂਹ_NUMBERS_WITH_R = {
                        {&imp_Tab_L_ਸਮੂਹ_NUMBERS_WITH_R,
                         &imp_Tab_R_ਸਮੂਹ_NUMBERS_WITH_R},
                        {&impAct0, &impAct0}};


ਸਥਿਰ ਅਚਲ ImpTab impTabL_INVERSE_NUMBERS_AS_L =
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
ਸਥਿਰ ਅਚਲ ImpTab impTabR_INVERSE_NUMBERS_AS_L =
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
ਸਥਿਰ ਅਚਲ ImpTabPair impTab_INVERSE_NUMBERS_AS_L = {
                        {&impTabL_INVERSE_NUMBERS_AS_L,
                         &impTabR_INVERSE_NUMBERS_AS_L},
                        {&impAct0, &impAct0}};

ਸਥਿਰ ਅਚਲ ImpTab imp_Tab_R_INVERSE_LIKE_ਸਿੱਧਾ =   /* Odd  paragraph level */
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
ਸਥਿਰ ਅਚਲ ImpAct impAct1 = {0,1,13,14};
/* FOOD FOR THOUGHT: in LTR table below, check case "JKL 123abc"
 */
ਸਥਿਰ ਅਚਲ ImpTabPair imp_Tab_INVERSE_LIKE_ਸਿੱਧਾ = {
                        {&impTabL_DEFAULT,
                         &imp_Tab_R_INVERSE_LIKE_ਸਿੱਧਾ},
                        {&impAct0, &impAct1}};

ਸਥਿਰ ਅਚਲ ImpTab imp_Tab_L_INVERSE_LIKE_ਸਿੱਧਾ_WITH_MARKS =
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
ਸਥਿਰ ਅਚਲ ImpTab imp_Tab_R_INVERSE_LIKE_ਸਿੱਧਾ_WITH_MARKS =
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
ਸਥਿਰ ਅਚਲ ImpAct impAct2 = {0,1,2,5,6,7,8};
ਸਥਿਰ ਅਚਲ ImpAct impAct3 = {0,1,9,10,11,12};
ਸਥਿਰ ਅਚਲ ImpTabPair imp_Tab_INVERSE_LIKE_ਸਿੱਧਾ_WITH_MARKS = {
                        {&imp_Tab_L_INVERSE_LIKE_ਸਿੱਧਾ_WITH_MARKS,
                         &imp_Tab_R_INVERSE_LIKE_ਸਿੱਧਾ_WITH_MARKS},
                        {&impAct2, &impAct3}};

ਸਥਿਰ ਅਚਲ ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL = {
                        {&impTabL_NUMBERS_SPECIAL,
                         &imp_Tab_R_INVERSE_LIKE_ਸਿੱਧਾ},
                        {&impAct0, &impAct1}};

ਸਥਿਰ ਅਚਲ ImpTab impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS =
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
ਸਥਿਰ ਅਚਲ ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS = {
                        {&impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS,
                         &imp_Tab_R_INVERSE_LIKE_ਸਿੱਧਾ_WITH_MARKS},
                        {&impAct2, &impAct3}};

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ s

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅਚਲ ImpTab * pImpTab;             /* level table pointer          */
    ਅਚਲ ImpAct * pImpAct;             /* action map array             */
    int32_t ਸ਼ੁਰੂ_ON;                    /* start of ON sequence         */
    int32_t ਸ਼ੁਰੂ_L2_EN;                  /* start of level 2 sequence    */
    int32_t ਆਖਰੀ_Strong_RTL;              /* index of last found R or AL  */
    int32_t state;                      /* current state                */
    int32_t ਲੜੀਵਾਰ_ਸ਼ੁਰੂ;                   /* start position of the run    */
    UBiDiLevel ਲੜੀਵਾਰ_Level;                /* run level before implicit solving */
} LevState;

/*------------------------------------------------------------------------*/

ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_ਬਿੰਦੂ(UBiDi *pBiDi, int32_t pos, int32_t flag)
  /* param pos:     position where to insert
     param flag:    one of LRM_BEFORE, LRM_AFTER, RLM_BEFORE, RLM_AFTER
  */
{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIRSTALLOC  10
    ਬਿੰਦੂ ਬਿੰਦੂ_2;
    ਪਾਓ_Points_2 * p_ਪਾਓ_Points=&(pBiDi->ਪਾਓ_Points);

    ਜੇ (p_ਪਾਓ_Points->capacity == 0)
    {
        p_ਪਾਓ_Points->points=static_cast<ਬਿੰਦੂ *>(uprv_malloc(ਆਕਾਰ(ਬਿੰਦੂ)*FIRSTALLOC));
        ਜੇ (p_ਪਾਓ_Points->points == ਖਾਲੀ_ਸੂਚਕ)
        {
            p_ਪਾਓ_Points->error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
        p_ਪਾਓ_Points->capacity=FIRSTALLOC;
    }
    ਜੇ (p_ਪਾਓ_Points->size >= p_ਪਾਓ_Points->capacity) /* no room for new point */
    {
        ਬਿੰਦੂ * savePoints=p_ਪਾਓ_Points->points;
        p_ਪਾਓ_Points->points=static_cast<ਬਿੰਦੂ *>(uprv_realloc(p_ਪਾਓ_Points->points,
                                           p_ਪਾਓ_Points->capacity*2*ਆਕਾਰ(ਬਿੰਦੂ)));
        ਜੇ (p_ਪਾਓ_Points->points == ਖਾਲੀ_ਸੂਚਕ)
        {
            p_ਪਾਓ_Points->points=savePoints;
            p_ਪਾਓ_Points->error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
        ਨਹੀਂਤਾਂ  p_ਪਾਓ_Points->capacity*=2;
    }
    ਬਿੰਦੂ_2.pos=pos;
    ਬਿੰਦੂ_2.flag=flag;
    p_ਪਾਓ_Points->points[p_ਪਾਓ_Points->size]=ਬਿੰਦੂ_2;
    p_ਪਾਓ_Points->size++;
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ FIRSTALLOC
}

ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_ਪੱਧਰ_Outside_Isolates(UBiDi *pBiDi, int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, UBiDiLevel level)
{
    DirProp *dirProps=pBiDi->dirProps, dirProp;
    UBiDiLevel *ਪੱਧਰ=pBiDi->ਪੱਧਰ;
    int32_t isolate_ਗਿਣਤੀ=0, k;
    ਲਈ(k=ਸ਼ੁਰੂ; k<ਹੱਦਬੰਦੀ; k++) {
        dirProp=dirProps[k];
        ਜੇ(dirProp==PDI)
            isolate_ਗਿਣਤੀ--;
        ਜੇ(isolate_ਗਿਣਤੀ==0)
            ਪੱਧਰ[k]=level;
        ਜੇ(dirProp==LRI || dirProp==RLI)
            isolate_ਗਿਣਤੀ++;
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

ਸਥਿਰ ਖਾਲੀ
processPropertySeq(UBiDi *pBiDi, LevState *pLevState, uint8_t _prop,
                   int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ) {
    uint8_t cell, oldStateSeq, actionSeq;
    ਅਚਲ ImpTab * pImpTab=pLevState->pImpTab;
    ਅਚਲ ImpAct * pImpAct=pLevState->pImpAct;
    UBiDiLevel * ਪੱਧਰ=pBiDi->ਪੱਧਰ;
    UBiDiLevel level, ਜੋੜੋ_Level;
    ਪਾਓ_Points_2 * p_ਪਾਓ_Points;
    int32_t start0, k;

    start0=ਸ਼ੁਰੂ;                           /* save original start position */
    oldStateSeq = static_cast<uint8_t>(pLevState->state);
    cell=(*pImpTab)[oldStateSeq][_prop];
    pLevState->state=GET_STATE(cell);       /* isolate the new state */
    actionSeq=(*pImpAct)[GET_ACTION(cell)]; /* isolate the action */
    ਜੋੜੋ_Level=(*pImpTab)[pLevState->state][IMPTABLEVELS_RES];

    ਜੇ(actionSeq) {
        ਚੋਣ(actionSeq) {
        ਹਾਲਤ 1:                         /* init ON seq */
            pLevState->ਸ਼ੁਰੂ_ON=start0;
            ਰੋਕੋ;

        ਹਾਲਤ 2:                         /* prepend ON seq to current seq */
            ਸ਼ੁਰੂ=pLevState->ਸ਼ੁਰੂ_ON;
            ਰੋਕੋ;

        ਹਾਲਤ 3:                         /* EN/AN after R+ON */
            level=pLevState->ਲੜੀਵਾਰ_Level+1;
            ਸੈੱਟ_ਪੱਧਰ_Outside_Isolates(pBiDi, pLevState->ਸ਼ੁਰੂ_ON, start0, level);
            ਰੋਕੋ;

        ਹਾਲਤ 4:                         /* EN/AN before R for NUMBERS_SPECIAL */
            level=pLevState->ਲੜੀਵਾਰ_Level+2;
            ਸੈੱਟ_ਪੱਧਰ_Outside_Isolates(pBiDi, pLevState->ਸ਼ੁਰੂ_ON, start0, level);
            ਰੋਕੋ;

        ਹਾਲਤ 5:                         /* L or S after possible relevant EN/AN */
            /* check if we had EN after R/AL */
            ਜੇ (pLevState->ਸ਼ੁਰੂ_L2_EN >= 0) {
                ਜੋੜੋ_ਬਿੰਦੂ(pBiDi, pLevState->ਸ਼ੁਰੂ_L2_EN, LRM_BEFORE);
            }
            pLevState->ਸ਼ੁਰੂ_L2_EN=-1;  /* not within previous if since could also be -2 */
            /* check if we had any relevant EN/AN after R/AL */
            p_ਪਾਓ_Points=&(pBiDi->ਪਾਓ_Points);
            ਜੇ ((p_ਪਾਓ_Points->capacity == 0) ||
                (p_ਪਾਓ_Points->size <= p_ਪਾਓ_Points->confirmed))
            {
                /* nothing, just clean up */
                pLevState->ਆਖਰੀ_Strong_RTL=-1;
                /* check if we have a pending conditional segment */
                level=(*pImpTab)[oldStateSeq][IMPTABLEVELS_RES];
                ਜੇ ((level & 1) && (pLevState->ਸ਼ੁਰੂ_ON > 0)) {  /* after ON */
                    ਸ਼ੁਰੂ=pLevState->ਸ਼ੁਰੂ_ON;   /* reset to basic run level */
                }
                ਜੇ (_prop == DirProp_S)                /* add LRM before S */
                {
                    ਜੋੜੋ_ਬਿੰਦੂ(pBiDi, start0, LRM_BEFORE);
                    p_ਪਾਓ_Points->confirmed=p_ਪਾਓ_Points->size;
                }
                ਰੋਕੋ;
            }
            /* reset previous RTL cont to level for LTR text */
            ਲਈ (k=pLevState->ਆਖਰੀ_Strong_RTL+1; k<start0; k++)
            {
                /* reset odd level, leave runLevel+2 as is */
                ਪੱਧਰ[k]=(ਪੱਧਰ[k] - 2) & ~1;
            }
            /* mark insert points as confirmed */
            p_ਪਾਓ_Points->confirmed=p_ਪਾਓ_Points->size;
            pLevState->ਆਖਰੀ_Strong_RTL=-1;
            ਜੇ (_prop == DirProp_S)            /* add LRM before S */
            {
                ਜੋੜੋ_ਬਿੰਦੂ(pBiDi, start0, LRM_BEFORE);
                p_ਪਾਓ_Points->confirmed=p_ਪਾਓ_Points->size;
            }
            ਰੋਕੋ;

        ਹਾਲਤ 6:                         /* R/AL after possible relevant EN/AN */
            /* just clean up */
            p_ਪਾਓ_Points=&(pBiDi->ਪਾਓ_Points);
            ਜੇ (p_ਪਾਓ_Points->capacity > 0)
                /* remove all non confirmed insert points */
                p_ਪਾਓ_Points->size=p_ਪਾਓ_Points->confirmed;
            pLevState->ਸ਼ੁਰੂ_ON=-1;
            pLevState->ਸ਼ੁਰੂ_L2_EN=-1;
            pLevState->ਆਖਰੀ_Strong_RTL=ਹੱਦਬੰਦੀ - 1;
            ਰੋਕੋ;

        ਹਾਲਤ 7:                         /* EN/AN after R/AL + possible cont */
            /* check for real AN */
            ਜੇ ((_prop == DirProp_AN) && (pBiDi->dirProps[start0] == AN) &&
                (pBiDi->reordering_ਬਹੁਲਕ!=UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))
            {
                /* real AN */
                ਜੇ (pLevState->ਸ਼ੁਰੂ_L2_EN == -1) /* if no relevant EN already found */
                {
                    /* just note the righmost digit as a strong RTL */
                    pLevState->ਆਖਰੀ_Strong_RTL=ਹੱਦਬੰਦੀ - 1;
                    ਰੋਕੋ;
                }
                ਜੇ (pLevState->ਸ਼ੁਰੂ_L2_EN >= 0)  /* after EN, no AN */
                {
                    ਜੋੜੋ_ਬਿੰਦੂ(pBiDi, pLevState->ਸ਼ੁਰੂ_L2_EN, LRM_BEFORE);
                    pLevState->ਸ਼ੁਰੂ_L2_EN=-2;
                }
                /* note AN */
                ਜੋੜੋ_ਬਿੰਦੂ(pBiDi, start0, LRM_BEFORE);
                ਰੋਕੋ;
            }
            /* if first EN/AN after R/AL */
            ਜੇ (pLevState->ਸ਼ੁਰੂ_L2_EN == -1) {
                pLevState->ਸ਼ੁਰੂ_L2_EN=start0;
            }
            ਰੋਕੋ;

        ਹਾਲਤ 8:                         /* note location of latest R/AL */
            pLevState->ਆਖਰੀ_Strong_RTL=ਹੱਦਬੰਦੀ - 1;
            pLevState->ਸ਼ੁਰੂ_ON=-1;
            ਰੋਕੋ;

        ਹਾਲਤ 9:                         /* L after R+ON/EN/AN */
            /* include possible adjacent number on the left */
            ਲਈ (k=start0-1; k>=0 && !(ਪੱਧਰ[k]&1); k--);
            ਜੇ(k>=0) {
                ਜੋੜੋ_ਬਿੰਦੂ(pBiDi, k, RLM_BEFORE);             /* add RLM before */
                p_ਪਾਓ_Points=&(pBiDi->ਪਾਓ_Points);
                p_ਪਾਓ_Points->confirmed=p_ਪਾਓ_Points->size;   /* confirm it */
            }
            pLevState->ਸ਼ੁਰੂ_ON=start0;
            ਰੋਕੋ;

        ਹਾਲਤ 10:                        /* AN after L */
            /* AN numbers between L text on both sides may be trouble. */
            /* tentatively bracket with LRMs; will be confirmed if followed by L */
            ਜੋੜੋ_ਬਿੰਦੂ(pBiDi, start0, LRM_BEFORE);    /* add LRM before */
            ਜੋੜੋ_ਬਿੰਦੂ(pBiDi, start0, LRM_AFTER);     /* add LRM after  */
            ਰੋਕੋ;

        ਹਾਲਤ 11:                        /* R after L+ON/EN/AN */
            /* false alert, infirm LRMs around previous AN */
            p_ਪਾਓ_Points=&(pBiDi->ਪਾਓ_Points);
            p_ਪਾਓ_Points->size=p_ਪਾਓ_Points->confirmed;
            ਜੇ (_prop == DirProp_S)            /* add RLM before S */
            {
                ਜੋੜੋ_ਬਿੰਦੂ(pBiDi, start0, RLM_BEFORE);
                p_ਪਾਓ_Points->confirmed=p_ਪਾਓ_Points->size;
            }
            ਰੋਕੋ;

        ਹਾਲਤ 12:                        /* L after L+ON/AN */
            level=pLevState->ਲੜੀਵਾਰ_Level + ਜੋੜੋ_Level;
            ਲਈ(k=pLevState->ਸ਼ੁਰੂ_ON; k<start0; k++) {
                ਜੇ (ਪੱਧਰ[k]<level)
                    ਪੱਧਰ[k]=level;
            }
            p_ਪਾਓ_Points=&(pBiDi->ਪਾਓ_Points);
            p_ਪਾਓ_Points->confirmed=p_ਪਾਓ_Points->size;   /* confirm inserts */
            pLevState->ਸ਼ੁਰੂ_ON=start0;
            ਰੋਕੋ;

        ਹਾਲਤ 13:                        /* L after L+ON+EN/AN/ON */
            level=pLevState->ਲੜੀਵਾਰ_Level;
            ਲਈ(k=start0-1; k>=pLevState->ਸ਼ੁਰੂ_ON; k--) {
                ਜੇ(ਪੱਧਰ[k]==level+3) {
                    ਜਦੋਂ(ਪੱਧਰ[k]==level+3) {
                        ਪੱਧਰ[k--]-=2;
                    }
                    ਜਦੋਂ(ਪੱਧਰ[k]==level) {
                        k--;
                    }
                }
                ਜੇ(ਪੱਧਰ[k]==level+2) {
                    ਪੱਧਰ[k]=level;
                    ਜਾਰੀ;
                }
                ਪੱਧਰ[k]=level+1;
            }
            ਰੋਕੋ;

        ਹਾਲਤ 14:                        /* R after L+ON+EN/AN/ON */
            level=pLevState->ਲੜੀਵਾਰ_Level+1;
            ਲਈ(k=start0-1; k>=pLevState->ਸ਼ੁਰੂ_ON; k--) {
                ਜੇ(ਪੱਧਰ[k]>level) {
                    ਪੱਧਰ[k]-=2;
                }
            }
            ਰੋਕੋ;

        ਮੂਲ:                        /* we should never get here */
            UPRV_UNREACHABLE_EXIT;
        }
    }
    ਜੇ((ਜੋੜੋ_Level) || (ਸ਼ੁਰੂ < start0)) {
        level=pLevState->ਲੜੀਵਾਰ_Level + ਜੋੜੋ_Level;
        ਜੇ(ਸ਼ੁਰੂ>=pLevState->ਲੜੀਵਾਰ_ਸ਼ੁਰੂ) {
            ਲਈ(k=ਸ਼ੁਰੂ; k<ਹੱਦਬੰਦੀ; k++) {
                ਪੱਧਰ[k]=level;
            }
        } ਨਹੀਂਤਾਂ {
            ਸੈੱਟ_ਪੱਧਰ_Outside_Isolates(pBiDi, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, level);
        }
    }
}

/**
 * Returns the directionality of the last strong character at the end of the prologue, if any.
 * Requires prologue!=null.
 */
ਸਥਿਰ DirProp
ਆਖਰੀ_L_R_AL(UBiDi *pBiDi) {
    ਅਚਲ char16_t *ਪਾਠ=pBiDi->prologue;
    int32_t length=pBiDi->pro_ਲੰਮਾਈ;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    ਲਈ(i=length; i>0; ) {
        /* i is decremented by U16_PREV */
        U16_PREV(ਪਾਠ, 0, i, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        ਜੇ(dirProp==L) {
            ਵਾਪਸ DirProp_L;
        }
        ਜੇ(dirProp==R || dirProp==AL) {
            ਵਾਪਸ DirProp_R;
        }
        ਜੇ(dirProp==B) {
            ਵਾਪਸ DirProp_ON;
        }
    }
    ਵਾਪਸ DirProp_ON;
}

/**
 * Returns the directionality of the first strong character, or digit, in the epilogue, if any.
 * Requires epilogue!=null.
 */
ਸਥਿਰ DirProp
ਪਹਿਲਾ_L_R_AL_EN_AN(UBiDi *pBiDi) {
    ਅਚਲ char16_t *ਪਾਠ=pBiDi->epilogue;
    int32_t length=pBiDi->epi_ਲੰਮਾਈ;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    ਲਈ(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_ਅਗਲਾ(ਪਾਠ, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        ਜੇ(dirProp==L) {
            ਵਾਪਸ DirProp_L;
        }
        ਜੇ(dirProp==R || dirProp==AL) {
            ਵਾਪਸ DirProp_R;
        }
        ਜੇ(dirProp==EN) {
            ਵਾਪਸ DirProp_EN;
        }
        ਜੇ(dirProp==AN) {
            ਵਾਪਸ DirProp_AN;
        }
    }
    ਵਾਪਸ DirProp_ON;
}

ਸਥਿਰ ਖਾਲੀ
resolve_Implicit_ਪੱਧਰ(UBiDi *pBiDi,
                      int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ,
                      DirProp sor, DirProp eor) {
    ਅਚਲ DirProp *dirProps=pBiDi->dirProps;
    DirProp dirProp;
    LevState levState;
    int32_t i, start1, start2;
    uint16_t oldStateImp, stateImp, actionImp;
    uint8_t gprop, resProp, cell;
    UBool inverseRTL;
    DirProp ਅਗਲਾ_Strong_Prop=R;
    int32_t ਅਗਲਾ_Strong_Pos=-1;

    /* check for RTL inverse BiDi mode */
    /* FOOD FOR THOUGHT: in case of RTL inverse BiDi, it would make sense to
     * loop on the text characters from end to start.
     * This would need a different properties state table (at least different
     * actions) and different levels state tables (maybe very similar to the
     * LTR corresponding ones.
     */
    inverseRTL =
        static_cast<UBool>((ਸ਼ੁਰੂ < pBiDi->ਆਖਰੀ_Arabic_Pos) && (GET_PARALEVEL(pBiDi, ਸ਼ੁਰੂ) & 1) &&
                           (pBiDi->reordering_ਬਹੁਲਕ == UBIDI_REORDER_INVERSE_LIKE_ਸਿੱਧਾ ||
                            pBiDi->reordering_ਬਹੁਲਕ == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL));

    /* initialize for property and levels state tables */
    levState.ਸ਼ੁਰੂ_L2_EN=-1;              /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.ਆਖਰੀ_Strong_RTL=-1;          /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.ਲੜੀਵਾਰ_ਸ਼ੁਰੂ=ਸ਼ੁਰੂ;
    levState.ਲੜੀਵਾਰ_Level=pBiDi->ਪੱਧਰ[ਸ਼ੁਰੂ];
    levState.pImpTab = static_cast<ਅਚਲ ImpTab*>(((pBiDi->pImpTabPair)->pImpTab)[levState.ਲੜੀਵਾਰ_Level & 1]);
    levState.pImpAct = static_cast<ਅਚਲ ImpAct*>(((pBiDi->pImpTabPair)->pImpAct)[levState.ਲੜੀਵਾਰ_Level & 1]);
    ਜੇ(ਸ਼ੁਰੂ==0 && pBiDi->pro_ਲੰਮਾਈ>0) {
        DirProp ਆਖਰੀ_Strong=ਆਖਰੀ_L_R_AL(pBiDi);
        ਜੇ(ਆਖਰੀ_Strong!=DirProp_ON) {
            sor=ਆਖਰੀ_Strong;
        }
    }
    /* The isolates[] entries contain enough information to
       resume the bidi algorithm in the same state as it was
       when it was interrupted by an isolate sequence. */
    ਜੇ(dirProps[ਸ਼ੁਰੂ]==PDI  && pBiDi->isolate_ਗਿਣਤੀ >= 0) {
        levState.ਸ਼ੁਰੂ_ON=pBiDi->isolates[pBiDi->isolate_ਗਿਣਤੀ].ਸ਼ੁਰੂ_ON;
        start1=pBiDi->isolates[pBiDi->isolate_ਗਿਣਤੀ].start1;
        stateImp=pBiDi->isolates[pBiDi->isolate_ਗਿਣਤੀ].stateImp;
        levState.state=pBiDi->isolates[pBiDi->isolate_ਗਿਣਤੀ].state;
        pBiDi->isolate_ਗਿਣਤੀ--;
    } ਨਹੀਂਤਾਂ {
        levState.ਸ਼ੁਰੂ_ON=-1;
        start1=ਸ਼ੁਰੂ;
        ਜੇ(dirProps[ਸ਼ੁਰੂ]==NSM)
            stateImp = 1 + sor;
        ਨਹੀਂਤਾਂ
            stateImp=0;
        levState.state=0;
        processPropertySeq(pBiDi, &levState, sor, ਸ਼ੁਰੂ, ਸ਼ੁਰੂ);
    }
    start2=ਸ਼ੁਰੂ;                       /* to make Java compiler happy */

    ਲਈ(i=ਸ਼ੁਰੂ; i<=ਹੱਦਬੰਦੀ; i++) {
        ਜੇ(i>=ਹੱਦਬੰਦੀ) {
            int32_t k;
            ਲਈ(k=ਹੱਦਬੰਦੀ-1; k>ਸ਼ੁਰੂ&&(DIRPROP_FLAG(dirProps[k])&ਨਕਾਬ_BN_EXPLICIT); k--);
            dirProp=dirProps[k];
            ਜੇ(dirProp==LRI || dirProp==RLI)
                ਰੋਕੋ;      /* no forced closing for sequence ending with LRI/RLI */
            gprop=eor;
        } ਨਹੀਂਤਾਂ {
            DirProp prop, prop1;
            prop=dirProps[i];
            ਜੇ(prop==B) {
                pBiDi->isolate_ਗਿਣਤੀ=-1; /* current isolates stack entry == none */
            }
            ਜੇ(inverseRTL) {
                ਜੇ(prop==AL) {
                    /* AL before EN does not make it AN */
                    prop=R;
                } ਨਹੀਂਤਾਂ ਜੇ(prop==EN) {
                    ਜੇ(ਅਗਲਾ_Strong_Pos<=i) {
                        /* look for next strong char (L/R/AL) */
                        int32_t j;
                        ਅਗਲਾ_Strong_Prop=R;   /* set default */
                        ਅਗਲਾ_Strong_Pos=ਹੱਦਬੰਦੀ;
                        ਲਈ(j=i+1; j<ਹੱਦਬੰਦੀ; j++) {
                            prop1=dirProps[j];
                            ਜੇ(prop1==L || prop1==R || prop1==AL) {
                                ਅਗਲਾ_Strong_Prop=prop1;
                                ਅਗਲਾ_Strong_Pos=j;
                                ਰੋਕੋ;
                            }
                        }
                    }
                    ਜੇ(ਅਗਲਾ_Strong_Prop==AL) {
                        prop=AN;
                    }
                }
            }
            gprop=ਸਮੂਹ_Prop[prop];
        }
        oldStateImp=stateImp;
        cell=impTabProps[oldStateImp][gprop];
        stateImp=GET_STATEPROPS(cell);      /* isolate the new state */
        actionImp=GET_ACTIONPROPS(cell);    /* isolate the action */
        ਜੇ((i==ਹੱਦਬੰਦੀ) && (actionImp==0)) {
            /* there is an unprocessed sequence if its property == eor   */
            actionImp=1;                    /* process the last sequence */
        }
        ਜੇ(actionImp) {
            resProp=impTabProps[oldStateImp][IMPTABPROPS_RES];
            ਚੋਣ(actionImp) {
            ਹਾਲਤ 1:             /* process current seq1, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, i);
                start1=i;
                ਰੋਕੋ;
            ਹਾਲਤ 2:             /* init new seq2 */
                start2=i;
                ਰੋਕੋ;
            ਹਾਲਤ 3:             /* process seq1, process seq2, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                processPropertySeq(pBiDi, &levState, DirProp_ON, start2, i);
                start1=i;
                ਰੋਕੋ;
            ਹਾਲਤ 4:             /* process seq1, set seq1=seq2, init new seq2 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                start1=start2;
                start2=i;
                ਰੋਕੋ;
            ਮੂਲ:            /* we should never get here */
                UPRV_UNREACHABLE_EXIT;
            }
        }
    }

    /* flush possible pending sequence, e.g. ON */
    ਜੇ(ਹੱਦਬੰਦੀ==pBiDi->length && pBiDi->epi_ਲੰਮਾਈ>0) {
        DirProp ਪਹਿਲਾ_Strong=ਪਹਿਲਾ_L_R_AL_EN_AN(pBiDi);
        ਜੇ(ਪਹਿਲਾ_Strong!=DirProp_ON) {
            eor=ਪਹਿਲਾ_Strong;
        }
    }

    /* look for the last char not a BN or LRE/RLE/LRO/RLO/PDF */
    ਲਈ(i=ਹੱਦਬੰਦੀ-1; i>ਸ਼ੁਰੂ&&(DIRPROP_FLAG(dirProps[i])&ਨਕਾਬ_BN_EXPLICIT); i--);
    dirProp=dirProps[i];
    ਜੇ((dirProp==LRI || dirProp==RLI) && ਹੱਦਬੰਦੀ<pBiDi->length) {
        pBiDi->isolate_ਗਿਣਤੀ++;
        pBiDi->isolates[pBiDi->isolate_ਗਿਣਤੀ].stateImp=stateImp;
        pBiDi->isolates[pBiDi->isolate_ਗਿਣਤੀ].state=levState.state;
        pBiDi->isolates[pBiDi->isolate_ਗਿਣਤੀ].start1=start1;
        pBiDi->isolates[pBiDi->isolate_ਗਿਣਤੀ].ਸ਼ੁਰੂ_ON=levState.ਸ਼ੁਰੂ_ON;
    }
    ਨਹੀਂਤਾਂ
        processPropertySeq(pBiDi, &levState, eor, ਹੱਦਬੰਦੀ, ਹੱਦਬੰਦੀ);
}

/* perform (L1) and (X9) ---------------------------------------------------- */

/*
 * Reset the embedding levels for some non-graphic characters (L1).
 * This function also sets appropriate levels for BN, and
 * explicit embedding types that are supposed to have been removed
 * from the paragraph in (X9).
 */
ਸਥਿਰ ਖਾਲੀ
adjustWSLevels(UBiDi *pBiDi) {
    ਅਚਲ DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *ਪੱਧਰ=pBiDi->ਪੱਧਰ;
    int32_t i;

    ਜੇ(pBiDi->flags&ਨਕਾਬ_WS) {
        UBool ਕ੍ਰਮ_Paragraphs_LTR=pBiDi->ਕ੍ਰਮ_Paragraphs_LTR;
        Flags flag;

        i=pBiDi->trailingWSStart;
        ਜਦੋਂ(i>0) {
            /* reset a sequence of WS/BN before eop and B/S to the paragraph paraLevel */
            ਜਦੋਂ(i>0 && (flag=DIRPROP_FLAG(dirProps[--i]))&ਨਕਾਬ_WS) {
                ਜੇ(ਕ੍ਰਮ_Paragraphs_LTR&&(flag&DIRPROP_FLAG(B))) {
                    ਪੱਧਰ[i]=0;
                } ਨਹੀਂਤਾਂ {
                    ਪੱਧਰ[i]=GET_PARALEVEL(pBiDi, i);
                }
            }

            /* reset BN to the next character's paraLevel until B/S, which restarts above loop */
            /* here, i+1 is guaranteed to be <length */
            ਜਦੋਂ(i>0) {
                flag=DIRPROP_FLAG(dirProps[--i]);
                ਜੇ(flag&ਨਕਾਬ_BN_EXPLICIT) {
                    ਪੱਧਰ[i]=ਪੱਧਰ[i+1];
                } ਨਹੀਂਤਾਂ ਜੇ(ਕ੍ਰਮ_Paragraphs_LTR&&(flag&DIRPROP_FLAG(B))) {
                    ਪੱਧਰ[i]=0;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(flag&ਨਕਾਬ_B_S) {
                    ਪੱਧਰ[i]=GET_PARALEVEL(pBiDi, i);
                    ਰੋਕੋ;
                }
            }
        }
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_ਸੈੱਟ_Context(UBiDi *pBiDi,
                 ਅਚਲ char16_t *prologue, int32_t pro_ਲੰਮਾਈ,
                 ਅਚਲ char16_t *epilogue, int32_t epi_ਲੰਮਾਈ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ਕੋਡ);
    ਜੇ(pBiDi==ਖਾਲੀ_ਸੂਚਕ || pro_ਲੰਮਾਈ<-1 || epi_ਲੰਮਾਈ<-1 ||
       (prologue==ਖਾਲੀ_ਸੂਚਕ && pro_ਲੰਮਾਈ!=0) || (epilogue==ਖਾਲੀ_ਸੂਚਕ && epi_ਲੰਮਾਈ!=0)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    ਜੇ(pro_ਲੰਮਾਈ==-1) {
        pBiDi->pro_ਲੰਮਾਈ=u_strlen(prologue);
    } ਨਹੀਂਤਾਂ {
        pBiDi->pro_ਲੰਮਾਈ=pro_ਲੰਮਾਈ;
    }
    ਜੇ(epi_ਲੰਮਾਈ==-1) {
        pBiDi->epi_ਲੰਮਾਈ=u_strlen(epilogue);
    } ਨਹੀਂਤਾਂ {
        pBiDi->epi_ਲੰਮਾਈ=epi_ਲੰਮਾਈ;
    }
    pBiDi->prologue=prologue;
    pBiDi->epilogue=epilogue;
}

ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_Para_Success(UBiDi *pBiDi) {
    pBiDi->pro_ਲੰਮਾਈ=0;                 /* forget the last context */
    pBiDi->epi_ਲੰਮਾਈ=0;
    pBiDi->pParaBiDi=pBiDi;             /* mark successful setPara */
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BIDI_MIN(x, y)   ((x)<(y) ? (x) : (y))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BIDI_ABS(x)      ((x)>=0  ? (x) : (-(x)))

ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_Para_Runs_Only(UBiDi *pBiDi, ਅਚਲ char16_t *ਪਾਠ, int32_t length,
                UBiDiLevel paraLevel, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    int32_t *runs_Only_ਸਮ੍ਰਿਤੀ = ਖਾਲੀ_ਸੂਚਕ;
    int32_t *visualMap;
    char16_t *visual_ਪਾਠ;
    int32_t save_ਲੰਮਾਈ, saveTrailingWSStart;
    ਅਚਲ UBiDiLevel *ਪੱਧਰ;
    UBiDiLevel *save_ਪੱਧਰ;
    UBiDiDirection saveDirection;
    UBool save_May_Allocate_ਪਾਠ;
    ਲੜੀਵਾਰ *runs;
    int32_t visual_ਲੰਮਾਈ, i, j, visual_ਸ਼ੁਰੂ, logical_ਸ਼ੁਰੂ,
            ਲੜੀਵਾਰ_ਗਿਣਤੀ, ਲੜੀਵਾਰ_ਲੰਮਾਈ, addedRuns, ਪਾਓ_Remove,
            ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, ਕਦਮ, ਸੂਚਕਾਂਕ_Odd_ਦੋਆਧਾਰੀ_ਅੰਕ, logicalPos,
            index0, index1;
    uint32_t saveOptions;

    pBiDi->reordering_ਬਹੁਲਕ=UBIDI_REORDER_DEFAULT;
    ਜੇ(length==0) {
        ubidi_ਸੈੱਟ_Para(pBiDi, ਪਾਠ, length, paraLevel, ਖਾਲੀ_ਸੂਚਕ, p_Error_ਕੋਡ);
        ਜਾਓ cleanup3;
    }
    /* obtain memory for mapping table and visual text */
    runs_Only_ਸਮ੍ਰਿਤੀ=static_cast<int32_t *>(uprv_malloc(length*(ਆਕਾਰ(int32_t)+ਆਕਾਰ(char16_t)+ਆਕਾਰ(UBiDiLevel))));
    ਜੇ(runs_Only_ਸਮ੍ਰਿਤੀ==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਜਾਓ cleanup3;
    }
    visualMap=runs_Only_ਸਮ੍ਰਿਤੀ;
    visual_ਪਾਠ = reinterpret_cast<char16_t*>(&visualMap[length]);
    save_ਪੱਧਰ = reinterpret_cast<UBiDiLevel*>(&visual_ਪਾਠ[length]);
    saveOptions=pBiDi->reorderingOptions;
    ਜੇ(saveOptions & UBIDI_OPTION_ਪਾਓ_MARKS) {
        pBiDi->reorderingOptions&=~UBIDI_OPTION_ਪਾਓ_MARKS;
        pBiDi->reorderingOptions|=UBIDI_OPTION_REMOVE_CONTROLS;
    }
    paraLevel&=1;                       /* accept only 0 or 1 */
    ubidi_ਸੈੱਟ_Para(pBiDi, ਪਾਠ, length, paraLevel, ਖਾਲੀ_ਸੂਚਕ, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਜਾਓ cleanup3;
    }
    /* we cannot access directly pBiDi->levels since it is not yet set if
     * direction is not MIXED
     */
    ਪੱਧਰ=ubidi_get_ਪੱਧਰ(pBiDi, p_Error_ਕੋਡ);
    uprv_memcpy(save_ਪੱਧਰ, ਪੱਧਰ, (size_t)pBiDi->length*ਆਕਾਰ(UBiDiLevel));
    saveTrailingWSStart=pBiDi->trailingWSStart;
    save_ਲੰਮਾਈ=pBiDi->length;
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
    visual_ਲੰਮਾਈ=ubidi_writeReordered(pBiDi, visual_ਪਾਠ, length,
                                      (saveOptions & UBIDI_DO_MIRRORING) | UBIDI_ਅੰਦਰੂਨੀ_RUNS_ONLY, p_Error_ਕੋਡ);
    ubidi_getVisualMap(pBiDi, visualMap, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਜਾਓ cleanup2;
    }
    pBiDi->reorderingOptions=saveOptions;

    pBiDi->reordering_ਬਹੁਲਕ=UBIDI_REORDER_INVERSE_LIKE_ਸਿੱਧਾ;
    paraLevel^=1;
    /* Because what we did with reorderingOptions, visualText may be shorter
     * than the original text. But we don't want the levels memory to be
     * reallocated shorter than the original length, since we need to restore
     * the levels as after the first call to ubidi_setpara() before returning.
     * We will force mayAllocateText to false before the second call to
     * ubidi_setpara(), and will restore it afterwards.
     */
    save_May_Allocate_ਪਾਠ=pBiDi->may_Allocate_ਪਾਠ;
    pBiDi->may_Allocate_ਪਾਠ=ਝੂਠ;
    ubidi_ਸੈੱਟ_Para(pBiDi, visual_ਪਾਠ, visual_ਲੰਮਾਈ, paraLevel, ਖਾਲੀ_ਸੂਚਕ, p_Error_ਕੋਡ);
    pBiDi->may_Allocate_ਪਾਠ=save_May_Allocate_ਪਾਠ;
    ubidi_getRuns(pBiDi, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਜਾਓ cleanup1;
    }
    /* check if some runs must be split, count how many splits */
    addedRuns=0;
    ਲੜੀਵਾਰ_ਗਿਣਤੀ=pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ;
    runs=pBiDi->runs;
    visual_ਸ਼ੁਰੂ=0;
    ਲਈ(i=0; i<ਲੜੀਵਾਰ_ਗਿਣਤੀ; i++, visual_ਸ਼ੁਰੂ+=ਲੜੀਵਾਰ_ਲੰਮਾਈ) {
        ਲੜੀਵਾਰ_ਲੰਮਾਈ=runs[i].visual_ਹੱਦਬੰਦੀ-visual_ਸ਼ੁਰੂ;
        ਜੇ(ਲੜੀਵਾਰ_ਲੰਮਾਈ<2) {
            ਜਾਰੀ;
        }
        logical_ਸ਼ੁਰੂ=GET_ਸੂਚਕਾਂਕ(runs[i].logical_ਸ਼ੁਰੂ);
        ਲਈ(j=logical_ਸ਼ੁਰੂ+1; j<logical_ਸ਼ੁਰੂ+ਲੜੀਵਾਰ_ਲੰਮਾਈ; j++) {
            index0=visualMap[j];
            index1=visualMap[j-1];
            ਜੇ((BIDI_ABS(index0-index1)!=1) || (save_ਪੱਧਰ[index0]!=save_ਪੱਧਰ[index1])) {
                addedRuns++;
            }
        }
    }
    ਜੇ(addedRuns) {
        ਜੇ(get_Runs_ਸਮ੍ਰਿਤੀ(pBiDi, ਲੜੀਵਾਰ_ਗਿਣਤੀ+addedRuns)) {
            ਜੇ(ਲੜੀਵਾਰ_ਗਿਣਤੀ==1) {
                /* because we switch from UBiDi.simpleRuns to UBiDi.runs */
                pBiDi->runs_ਸਮ੍ਰਿਤੀ[0]=runs[0];
            }
            runs=pBiDi->runs=pBiDi->runs_ਸਮ੍ਰਿਤੀ;
            pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ+=addedRuns;
        } ਨਹੀਂਤਾਂ {
            *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਜਾਓ cleanup1;
        }
    }
    /* split runs which are not consecutive in source text */
    ਲਈ(i=ਲੜੀਵਾਰ_ਗਿਣਤੀ-1; i>=0; i--) {
        ਲੜੀਵਾਰ_ਲੰਮਾਈ= i==0 ? runs[0].visual_ਹੱਦਬੰਦੀ :
                          runs[i].visual_ਹੱਦਬੰਦੀ-runs[i-1].visual_ਹੱਦਬੰਦੀ;
        logical_ਸ਼ੁਰੂ=runs[i].logical_ਸ਼ੁਰੂ;
        ਸੂਚਕਾਂਕ_Odd_ਦੋਆਧਾਰੀ_ਅੰਕ=GET_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ(logical_ਸ਼ੁਰੂ);
        logical_ਸ਼ੁਰੂ=GET_ਸੂਚਕਾਂਕ(logical_ਸ਼ੁਰੂ);
        ਜੇ(ਲੜੀਵਾਰ_ਲੰਮਾਈ<2) {
            ਜੇ(addedRuns) {
                runs[i+addedRuns]=runs[i];
            }
            logicalPos=visualMap[logical_ਸ਼ੁਰੂ];
            runs[i+addedRuns].logical_ਸ਼ੁਰੂ=MAKE_ਸੂਚਕਾਂਕ_ODD_PAIR(logicalPos,
                                            save_ਪੱਧਰ[logicalPos]^ਸੂਚਕਾਂਕ_Odd_ਦੋਆਧਾਰੀ_ਅੰਕ);
            ਜਾਰੀ;
        }
        ਜੇ(ਸੂਚਕਾਂਕ_Odd_ਦੋਆਧਾਰੀ_ਅੰਕ) {
            ਸ਼ੁਰੂ=logical_ਸ਼ੁਰੂ;
            ਹੱਦਬੰਦੀ=logical_ਸ਼ੁਰੂ+ਲੜੀਵਾਰ_ਲੰਮਾਈ-1;
            ਕਦਮ=1;
        } ਨਹੀਂਤਾਂ {
            ਸ਼ੁਰੂ=logical_ਸ਼ੁਰੂ+ਲੜੀਵਾਰ_ਲੰਮਾਈ-1;
            ਹੱਦਬੰਦੀ=logical_ਸ਼ੁਰੂ;
            ਕਦਮ=-1;
        }
        ਲਈ(j=ਸ਼ੁਰੂ; j!=ਹੱਦਬੰਦੀ; j+=ਕਦਮ) {
            index0=visualMap[j];
            index1=visualMap[j+ਕਦਮ];
            ਜੇ((BIDI_ABS(index0-index1)!=1) || (save_ਪੱਧਰ[index0]!=save_ਪੱਧਰ[index1])) {
                logicalPos=BIDI_MIN(visualMap[ਸ਼ੁਰੂ], index0);
                runs[i+addedRuns].logical_ਸ਼ੁਰੂ=MAKE_ਸੂਚਕਾਂਕ_ODD_PAIR(logicalPos,
                                            save_ਪੱਧਰ[logicalPos]^ਸੂਚਕਾਂਕ_Odd_ਦੋਆਧਾਰੀ_ਅੰਕ);
                runs[i+addedRuns].visual_ਹੱਦਬੰਦੀ=runs[i].visual_ਹੱਦਬੰਦੀ;
                runs[i].visual_ਹੱਦਬੰਦੀ-=BIDI_ABS(j-ਸ਼ੁਰੂ)+1;
                ਪਾਓ_Remove=runs[i].ਪਾਓ_Remove&(LRM_AFTER|RLM_AFTER);
                runs[i+addedRuns].ਪਾਓ_Remove=ਪਾਓ_Remove;
                runs[i].ਪਾਓ_Remove&=~ਪਾਓ_Remove;
                ਸ਼ੁਰੂ=j+ਕਦਮ;
                addedRuns--;
            }
        }
        ਜੇ(addedRuns) {
            runs[i+addedRuns]=runs[i];
        }
        logicalPos=BIDI_MIN(visualMap[ਸ਼ੁਰੂ], visualMap[ਹੱਦਬੰਦੀ]);
        runs[i+addedRuns].logical_ਸ਼ੁਰੂ=MAKE_ਸੂਚਕਾਂਕ_ODD_PAIR(logicalPos,
                                            save_ਪੱਧਰ[logicalPos]^ਸੂਚਕਾਂਕ_Odd_ਦੋਆਧਾਰੀ_ਅੰਕ);
    }

  cleanup1:
    /* restore initial paraLevel */
    pBiDi->paraLevel^=1;
  cleanup2:
    /* restore real text */
    pBiDi->ਪਾਠ=ਪਾਠ;
    pBiDi->length=save_ਲੰਮਾਈ;
    pBiDi->original_ਲੰਮਾਈ=length;
    pBiDi->direction=saveDirection;
    /* the saved levels should never excess levelsSize, but we check anyway */
    ਜੇ(save_ਲੰਮਾਈ>pBiDi->ਪੱਧਰ_ਮਾਪ) {
        save_ਲੰਮਾਈ=pBiDi->ਪੱਧਰ_ਮਾਪ;
    }
    uprv_memcpy(pBiDi->ਪੱਧਰ, save_ਪੱਧਰ, (size_t)save_ਲੰਮਾਈ*ਆਕਾਰ(UBiDiLevel));
    pBiDi->trailingWSStart=saveTrailingWSStart;
    ਜੇ(pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ>1) {
        pBiDi->direction=UBIDI_MIXED;
    }
  cleanup3:
    /* free memory for mapping table and visual text */
    uprv_free(runs_Only_ਸਮ੍ਰਿਤੀ);

    pBiDi->reordering_ਬਹੁਲਕ=UBIDI_REORDER_RUNS_ONLY;
}

/* ubidi_setPara ------------------------------------------------------------ */

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_ਸੈੱਟ_Para(UBiDi *pBiDi, ਅਚਲ char16_t *ਪਾਠ, int32_t length,
              UBiDiLevel paraLevel, UBiDiLevel *embedding_ਪੱਧਰ,
              UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UBiDiDirection direction;
    DirProp *dirProps;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ਕੋਡ);
    ਜੇ(pBiDi==ਖਾਲੀ_ਸੂਚਕ || ਪਾਠ==ਖਾਲੀ_ਸੂਚਕ || length<-1 ||
       (paraLevel>UBIDI_MAX_EXPLICIT_LEVEL && paraLevel<UBIDI_DEFAULT_LTR)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    ਜੇ(length==-1) {
        length=u_strlen(ਪਾਠ);
    }

    /* special treatment for RUNS_ONLY mode */
    ਜੇ(pBiDi->reordering_ਬਹੁਲਕ==UBIDI_REORDER_RUNS_ONLY) {
        ਸੈੱਟ_Para_Runs_Only(pBiDi, ਪਾਠ, length, paraLevel, p_Error_ਕੋਡ);
        ਵਾਪਸ;
    }

    /* initialize the UBiDi structure */
    pBiDi->pParaBiDi=ਖਾਲੀ_ਸੂਚਕ;          /* mark unfinished setPara */
    pBiDi->ਪਾਠ=ਪਾਠ;
    pBiDi->length=pBiDi->original_ਲੰਮਾਈ=pBiDi->ਨਤੀਜਾ_ਲੰਮਾਈ=length;
    pBiDi->paraLevel=paraLevel;
    pBiDi->direction=(UBiDiDirection)(paraLevel&1);
    pBiDi->para_ਗਿਣਤੀ=1;

    pBiDi->dirProps=ਖਾਲੀ_ਸੂਚਕ;
    pBiDi->ਪੱਧਰ=ਖਾਲੀ_ਸੂਚਕ;
    pBiDi->runs=ਖਾਲੀ_ਸੂਚਕ;
    pBiDi->ਪਾਓ_Points.size=0;         /* clean up from last call */
    pBiDi->ਪਾਓ_Points.confirmed=0;    /* clean up from last call */

    /*
     * Save the original paraLevel if contextual; otherwise, set to 0.
     */
    pBiDi->defaultParaLevel=IS_DEFAULT_LEVEL(paraLevel);

    ਜੇ(length==0) {
        /*
         * For an empty paragraph, create a UBiDi object with the paraLevel and
         * the flags and the direction set but without allocating zero-length arrays.
         * There is nothing more to do.
         */
        ਜੇ(IS_DEFAULT_LEVEL(paraLevel)) {
            pBiDi->paraLevel&=1;
            pBiDi->defaultParaLevel=0;
        }
        pBiDi->flags=DIRPROP_FLAG_LR(paraLevel);
        pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ=0;
        pBiDi->para_ਗਿਣਤੀ=0;
        ਸੈੱਟ_Para_Success(pBiDi);          /* mark successful setPara */
        ਵਾਪਸ;
    }

    pBiDi->ਲੜੀਵਾਰ_ਗਿਣਤੀ=-1;

    /* allocate paras memory */
    ਜੇ(pBiDi->paras_ਸਮ੍ਰਿਤੀ)
        pBiDi->paras=pBiDi->paras_ਸਮ੍ਰਿਤੀ;
    ਨਹੀਂਤਾਂ
        pBiDi->paras=pBiDi->simpleParas;

    /*
     * Get the directional properties,
     * the flags bit-set, and
     * determine the paragraph level if necessary.
     */
    ਜੇ(get_Dir_Props_ਸਮ੍ਰਿਤੀ(pBiDi, length)) {
        pBiDi->dirProps=pBiDi->dir_Props_ਸਮ੍ਰਿਤੀ;
        ਜੇ(!getDirProps(pBiDi)) {
            *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
    } ਨਹੀਂਤਾਂ {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    dirProps=pBiDi->dirProps;
    /* the processed length may have changed if UBIDI_OPTION_STREAMING */
    length= pBiDi->length;
    pBiDi->trailingWSStart=length;  /* the levels[] will reflect the WS run */

    /* are explicit levels specified? */
    ਜੇ(embedding_ਪੱਧਰ==ਖਾਲੀ_ਸੂਚਕ) {
        /* no: determine explicit levels according to the (Xn) rules */\
        ਜੇ(get_ਪੱਧਰ_ਸਮ੍ਰਿਤੀ(pBiDi, length)) {
            pBiDi->ਪੱਧਰ=pBiDi->ਪੱਧਰ_ਸਮ੍ਰਿਤੀ;
            direction=resolve_Explicit_ਪੱਧਰ(pBiDi, p_Error_ਕੋਡ);
            ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                ਵਾਪਸ;
            }
        } ਨਹੀਂਤਾਂ {
            *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
    } ਨਹੀਂਤਾਂ {
        /* set BN for all explicit codes, check that all levels are 0 or paraLevel..UBIDI_MAX_EXPLICIT_LEVEL */
        pBiDi->ਪੱਧਰ=embedding_ਪੱਧਰ;
        direction=check_Explicit_ਪੱਧਰ(pBiDi, p_Error_ਕੋਡ);
        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            ਵਾਪਸ;
        }
    }

    /* allocate isolate memory */
    ਜੇ(pBiDi->isolate_ਗਿਣਤੀ<=SIMPLE_ISOLATES_ਗਿਣਤੀ)
        pBiDi->isolates=pBiDi->simpleIsolates;
    ਨਹੀਂਤਾਂ
        ਜੇ((int32_t)(pBiDi->isolate_ਗਿਣਤੀ*ਆਕਾਰ(Isolate))<=pBiDi->isolates_ਮਾਪ)
            pBiDi->isolates=pBiDi->isolates_ਸਮ੍ਰਿਤੀ;
        ਨਹੀਂਤਾਂ {
            ਜੇ(get_Initial_Isolates_ਸਮ੍ਰਿਤੀ(pBiDi, pBiDi->isolate_ਗਿਣਤੀ)) {
                pBiDi->isolates=pBiDi->isolates_ਸਮ੍ਰਿਤੀ;
            } ਨਹੀਂਤਾਂ {
                *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ;
            }
        }
    pBiDi->isolate_ਗਿਣਤੀ=-1;             /* current isolates stack entry == none */

    /*
     * The steps after (X9) in the UBiDi algorithm are performed only if
     * the paragraph text has mixed directionality!
     */
    pBiDi->direction=direction;
    ਚੋਣ(direction) {
    ਹਾਲਤ UBIDI_LTR:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        ਰੋਕੋ;
    ਹਾਲਤ UBIDI_RTL:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        ਰੋਕੋ;
    ਮੂਲ:
        /*
         *  Choose the right implicit state table
         */
        ਚੋਣ(pBiDi->reordering_ਬਹੁਲਕ) {
        ਹਾਲਤ UBIDI_REORDER_DEFAULT:
            pBiDi->pImpTabPair=&impTab_DEFAULT;
            ਰੋਕੋ;
        ਹਾਲਤ UBIDI_REORDER_NUMBERS_SPECIAL:
            pBiDi->pImpTabPair=&impTab_NUMBERS_SPECIAL;
            ਰੋਕੋ;
        ਹਾਲਤ UBIDI_REORDER_ਸਮੂਹ_NUMBERS_WITH_R:
            pBiDi->pImpTabPair=&imp_Tab_ਸਮੂਹ_NUMBERS_WITH_R;
            ਰੋਕੋ;
        ਹਾਲਤ UBIDI_REORDER_INVERSE_NUMBERS_AS_L:
            pBiDi->pImpTabPair=&impTab_INVERSE_NUMBERS_AS_L;
            ਰੋਕੋ;
        ਹਾਲਤ UBIDI_REORDER_INVERSE_LIKE_ਸਿੱਧਾ:
            ਜੇ (pBiDi->reorderingOptions & UBIDI_OPTION_ਪਾਓ_MARKS) {
                pBiDi->pImpTabPair=&imp_Tab_INVERSE_LIKE_ਸਿੱਧਾ_WITH_MARKS;
            } ਨਹੀਂਤਾਂ {
                pBiDi->pImpTabPair=&imp_Tab_INVERSE_LIKE_ਸਿੱਧਾ;
            }
            ਰੋਕੋ;
        ਹਾਲਤ UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL:
            ਜੇ (pBiDi->reorderingOptions & UBIDI_OPTION_ਪਾਓ_MARKS) {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS;
            } ਨਹੀਂਤਾਂ {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL;
            }
            ਰੋਕੋ;
        ਮੂਲ:
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
        ਜੇ(embedding_ਪੱਧਰ==ਖਾਲੀ_ਸੂਚਕ && pBiDi->para_ਗਿਣਤੀ<=1 &&
                                   !(pBiDi->flags&DIRPROP_FLAG_MULTI_RUNS)) {
            resolve_Implicit_ਪੱਧਰ(pBiDi, 0, length,
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, 0)),
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, length-1)));
        } ਨਹੀਂਤਾਂ {
            /* sor, eor: start and end types of same-level-run */
            UBiDiLevel *ਪੱਧਰ=pBiDi->ਪੱਧਰ;
            int32_t ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ=0;
            UBiDiLevel level, ਅਗਲਾ_Level;
            DirProp sor, eor;

            /* determine the first sor and set eor to it because of the loop body (sor=eor there) */
            level=GET_PARALEVEL(pBiDi, 0);
            ਅਗਲਾ_Level=ਪੱਧਰ[0];
            ਜੇ(level<ਅਗਲਾ_Level) {
                eor=GET_LR_FROM_LEVEL(ਅਗਲਾ_Level);
            } ਨਹੀਂਤਾਂ {
                eor=GET_LR_FROM_LEVEL(level);
            }

            ਕਰੋ {
                /* determine start and limit of the run (end points just behind the run) */

                /* the values for this run's start are the same as for the previous run's end */
                ਸ਼ੁਰੂ=ਹੱਦਬੰਦੀ;
                level=ਅਗਲਾ_Level;
                ਜੇ((ਸ਼ੁਰੂ>0) && (dirProps[ਸ਼ੁਰੂ-1]==B)) {
                    /* except if this is a new paragraph, then set sor = para level */
                    sor=GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, ਸ਼ੁਰੂ));
                } ਨਹੀਂਤਾਂ {
                    sor=eor;
                }

                /* search for the limit of this run */
                ਜਦੋਂ((++ਹੱਦਬੰਦੀ<length) &&
                      ((ਪੱਧਰ[ਹੱਦਬੰਦੀ]==level) ||
                       (DIRPROP_FLAG(dirProps[ਹੱਦਬੰਦੀ])&ਨਕਾਬ_BN_EXPLICIT))) {}

                /* get the correct level of the next run */
                ਜੇ(ਹੱਦਬੰਦੀ<length) {
                    ਅਗਲਾ_Level=ਪੱਧਰ[ਹੱਦਬੰਦੀ];
                } ਨਹੀਂਤਾਂ {
                    ਅਗਲਾ_Level=GET_PARALEVEL(pBiDi, length-1);
                }

                /* determine eor from max(level, nextLevel); sor is last run's eor */
                ਜੇ(NO_OVERRIDE(level)<NO_OVERRIDE(ਅਗਲਾ_Level)) {
                    eor=GET_LR_FROM_LEVEL(ਅਗਲਾ_Level);
                } ਨਹੀਂਤਾਂ {
                    eor=GET_LR_FROM_LEVEL(level);
                }

                /* if the run consists of overridden directional types, then there
                   are no implicit types to be resolved */
                ਜੇ(!(level&UBIDI_LEVEL_OVERRIDE)) {
                    resolve_Implicit_ਪੱਧਰ(pBiDi, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, sor, eor);
                } ਨਹੀਂਤਾਂ {
                    /* remove the UBIDI_LEVEL_OVERRIDE flags */
                    ਕਰੋ {
                        ਪੱਧਰ[ਸ਼ੁਰੂ++]&=~UBIDI_LEVEL_OVERRIDE;
                    } ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ);
                }
            } ਜਦੋਂ(ਹੱਦਬੰਦੀ<length);
        }
        /* check if we got any memory shortage while adding insert points */
        ਜੇ (U_FAILURE(pBiDi->ਪਾਓ_Points.error_ਕੋਡ))
        {
            *p_Error_ਕੋਡ=pBiDi->ਪਾਓ_Points.error_ਕੋਡ;
            ਵਾਪਸ;
        }
        /* reset the embedding levels for some non-graphic characters (L1), (X9) */
        adjustWSLevels(pBiDi);
        ਰੋਕੋ;
    }
    /* add RLM for inverse Bidi with contextual orientation resolving
     * to RTL which would not round-trip otherwise
     */
    ਜੇ((pBiDi->defaultParaLevel>0) &&
       (pBiDi->reorderingOptions & UBIDI_OPTION_ਪਾਓ_MARKS) &&
       ((pBiDi->reordering_ਬਹੁਲਕ==UBIDI_REORDER_INVERSE_LIKE_ਸਿੱਧਾ) ||
        (pBiDi->reordering_ਬਹੁਲਕ==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))) {
        int32_t i, j, ਸ਼ੁਰੂ, ਆਖਰੀ;
        UBiDiLevel level;
        DirProp dirProp;
        ਲਈ(i=0; i<pBiDi->para_ਗਿਣਤੀ; i++) {
            ਆਖਰੀ=(pBiDi->paras[i].ਹੱਦਬੰਦੀ)-1;
            level= static_cast<UBiDiLevel>(pBiDi->paras[i].level);
            ਜੇ(level==0)
                ਜਾਰੀ;           /* LTR paragraph */
            ਸ਼ੁਰੂ= i==0 ? 0 : pBiDi->paras[i-1].ਹੱਦਬੰਦੀ;
            ਲਈ(j=ਆਖਰੀ; j>=ਸ਼ੁਰੂ; j--) {
                dirProp=dirProps[j];
                ਜੇ(dirProp==L) {
                    ਜੇ(j<ਆਖਰੀ) {
                        ਜਦੋਂ(dirProps[ਆਖਰੀ]==B) {
                            ਆਖਰੀ--;
                        }
                    }
                    ਜੋੜੋ_ਬਿੰਦੂ(pBiDi, ਆਖਰੀ, RLM_BEFORE);
                    ਰੋਕੋ;
                }
                ਜੇ(DIRPROP_FLAG(dirProp) & ਨਕਾਬ_R_AL) {
                    ਰੋਕੋ;
                }
            }
        }
    }

    ਜੇ(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        pBiDi->ਨਤੀਜਾ_ਲੰਮਾਈ -= pBiDi->control_ਗਿਣਤੀ;
    } ਨਹੀਂਤਾਂ {
        pBiDi->ਨਤੀਜਾ_ਲੰਮਾਈ += pBiDi->ਪਾਓ_Points.size;
    }
    ਸੈੱਟ_Para_Success(pBiDi);              /* mark successful setPara */
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_ਕ੍ਰਮ_Paragraphs_LTR(UBiDi *pBiDi, UBool ਕ੍ਰਮ_Paragraphs_LTR) {
    ਜੇ(pBiDi!=ਖਾਲੀ_ਸੂਚਕ) {
        pBiDi->ਕ੍ਰਮ_Paragraphs_LTR=ਕ੍ਰਮ_Paragraphs_LTR;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_is_ਕ੍ਰਮ_Paragraphs_LTR(UBiDi *pBiDi) {
    ਜੇ(pBiDi!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ pBiDi->ਕ੍ਰਮ_Paragraphs_LTR;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਝੂਠ;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_getDirection(ਅਚਲ UBiDi *pBiDi) {
    ਜੇ(IS_VALID_PARA_OR_ਸਤਰ(pBiDi)) {
        ਵਾਪਸ pBiDi->direction;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ UBIDI_LTR;
    }
}

U_CAPI ਅਚਲ char16_t* U_EXPORT2
ubidi_get_ਪਾਠ(ਅਚਲ UBiDi* pBiDi) {
    ਜੇ(IS_VALID_PARA_OR_ਸਤਰ(pBiDi)) {
        ਵਾਪਸ pBiDi->ਪਾਠ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_ਲੰਮਾਈ(ਅਚਲ UBiDi *pBiDi) {
    ਜੇ(IS_VALID_PARA_OR_ਸਤਰ(pBiDi)) {
        ਵਾਪਸ pBiDi->original_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Processed_ਲੰਮਾਈ(ਅਚਲ UBiDi *pBiDi) {
    ਜੇ(IS_VALID_PARA_OR_ਸਤਰ(pBiDi)) {
        ਵਾਪਸ pBiDi->length;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_ਨਤੀਜਾ_ਲੰਮਾਈ(ਅਚਲ UBiDi *pBiDi) {
    ਜੇ(IS_VALID_PARA_OR_ਸਤਰ(pBiDi)) {
        ਵਾਪਸ pBiDi->ਨਤੀਜਾ_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
}

/* paragraphs API functions ------------------------------------------------- */

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getParaLevel(ਅਚਲ UBiDi *pBiDi) {
    ਜੇ(IS_VALID_PARA_OR_ਸਤਰ(pBiDi)) {
        ਵਾਪਸ pBiDi->paraLevel;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_ਗਿਣਤੀ_Paragraphs(UBiDi *pBiDi) {
    ਜੇ(!IS_VALID_PARA_OR_ਸਤਰ(pBiDi)) {
        ਵਾਪਸ 0;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ pBiDi->para_ਗਿਣਤੀ;
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_get_Paragraph_By_ਸੂਚਕਾਂਕ(ਅਚਲ UBiDi *pBiDi, int32_t para_ਸੂਚਕਾਂਕ,
                          int32_t *p_Para_ਸ਼ੁਰੂ, int32_t *p_Para_ਹੱਦਬੰਦੀ,
                          UBiDiLevel *pParaLevel, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    int32_t para_ਸ਼ੁਰੂ;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ਕੋਡ);
    RETURN_VOID_IF_NOT_VALID_PARA_OR_ਸਤਰ(pBiDi, *p_Error_ਕੋਡ);
    RETURN_VOID_IF_BAD_RANGE(para_ਸੂਚਕਾਂਕ, 0, pBiDi->para_ਗਿਣਤੀ, *p_Error_ਕੋਡ);

    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    ਜੇ(para_ਸੂਚਕਾਂਕ) {
        para_ਸ਼ੁਰੂ=pBiDi->paras[para_ਸੂਚਕਾਂਕ-1].ਹੱਦਬੰਦੀ;
    } ਨਹੀਂਤਾਂ {
        para_ਸ਼ੁਰੂ=0;
    }
    ਜੇ(p_Para_ਸ਼ੁਰੂ!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_Para_ਸ਼ੁਰੂ=para_ਸ਼ੁਰੂ;
    }
    ਜੇ(p_Para_ਹੱਦਬੰਦੀ!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_Para_ਹੱਦਬੰਦੀ=pBiDi->paras[para_ਸੂਚਕਾਂਕ].ਹੱਦਬੰਦੀ;
    }
    ਜੇ(pParaLevel!=ਖਾਲੀ_ਸੂਚਕ) {
        *pParaLevel=GET_PARALEVEL(pBiDi, para_ਸ਼ੁਰੂ);
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_getParagraph(ਅਚਲ UBiDi *pBiDi, int32_t char_ਸੂਚਕਾਂਕ,
                          int32_t *p_Para_ਸ਼ੁਰੂ, int32_t *p_Para_ਹੱਦਬੰਦੀ,
                          UBiDiLevel *pParaLevel, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    int32_t para_ਸੂਚਕਾਂਕ;

    /* check the argument values */
    /* pErrorCode will be checked by the call to ubidi_getParagraphByIndex */
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_ਕੋਡ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_ਸਤਰ(pBiDi, *p_Error_ਕੋਡ, -1);
    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    RETURN_IF_BAD_RANGE(char_ਸੂਚਕਾਂਕ, 0, pBiDi->length, *p_Error_ਕੋਡ, -1);

    ਲਈ(para_ਸੂਚਕਾਂਕ=0; char_ਸੂਚਕਾਂਕ>=pBiDi->paras[para_ਸੂਚਕਾਂਕ].ਹੱਦਬੰਦੀ; para_ਸੂਚਕਾਂਕ++);
    ubidi_get_Paragraph_By_ਸੂਚਕਾਂਕ(pBiDi, para_ਸੂਚਕਾਂਕ, p_Para_ਸ਼ੁਰੂ, p_Para_ਹੱਦਬੰਦੀ, pParaLevel, p_Error_ਕੋਡ);
    ਵਾਪਸ para_ਸੂਚਕਾਂਕ;
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_ਸੈੱਟ_Class_Callback(UBiDi *pBiDi, UBiDiClassCallback *newFn,
                       ਅਚਲ ਖਾਲੀ *newContext, UBiDiClassCallback **oldFn,
                       ਅਚਲ ਖਾਲੀ **oldContext, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_ਕੋਡ);
    ਜੇ(pBiDi==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ( oldFn )
    {
        *oldFn = pBiDi->fnClassCallback;
    }
    ਜੇ( oldContext )
    {
        *oldContext = pBiDi->coClassCallback;
    }
    pBiDi->fnClassCallback = newFn;
    pBiDi->coClassCallback = newContext;
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubidi_getClassCallback(UBiDi *pBiDi, UBiDiClassCallback **fn, ਅਚਲ ਖਾਲੀ **context)
{
    ਜੇ(pBiDi==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }
    ਜੇ( fn )
    {
        *fn = pBiDi->fnClassCallback;
    }
    ਜੇ( context )
    {
        *context = pBiDi->coClassCallback;
    }
}

U_CAPI UCharDirection U_EXPORT2
ubidi_getCustomizedClass(UBiDi *pBiDi, UChar32 c)
{
    UCharDirection dir;

    ਜੇ( pBiDi->fnClassCallback == ਖਾਲੀ_ਸੂਚਕ ||
        (dir = (*pBiDi->fnClassCallback)(pBiDi->coClassCallback, c)) == U_BIDI_CLASS_DEFAULT )
    {
        dir = ubidi_getClass(c);
    }
    ਜੇ(dir >= U_CHAR_DIRECTION_ਗਿਣਤੀ) {
        dir = (UCharDirection)ON;
    }
    ਵਾਪਸ dir;
}

/* Utilities for MessageFormat ------------------------------------------------*/

#ਜੇ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

U_CAPI UMFBidiOption U_EXPORT2
ubidi_getMFOption(ਅਚਲ UnicodeString& s) {
    ਜੇ (s == u"ltr") {
        ਵਾਪਸ U_MF_BIDI_OPTION_LTR;
    }
    ਜੇ (s == u"rtl") {
        ਵਾਪਸ U_MF_BIDI_OPTION_RTL;
    }
    ਜੇ (s == u"auto") {
        ਵਾਪਸ U_MF_BIDI_OPTION_AUTO;
    }
    ਵਾਪਸ U_MF_BIDI_OPTION_INHERIT; // inherit is default
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
    ਜੇ (bidiIsolationStrategy == U_MF_BIDI_OFF)
        ਵਾਪਸ &fmt;

    /* 1. Let msgdir be the directionality of the whole message, one of « 'LTR', 'RTL', 'unknown' ». These correspond to the message having left-to-right directionality, right-to-left directionality, and to the message's directionality not being known. */

    // 2i Let fmt be the formatted string representation of the resolved value of exp.
    // (Passed as argument)

    // 2ii Let dir be the directionality of fmt, one of « 'LTR', 'RTL', 'unknown' », with the same meanings as for msgdir
    // (Passed as argument)

    // 2iii. Let the boolean value isolate be True if the u:dir option of the resolved value of exp has a value other than 'inherit', or False otherwise.
    ਤਾਰਕਿਕ isolate = uDirOption != U_MF_BIDI_OPTION_INHERIT;

    UnicodeString bdiOpen("<bdi>");
    UnicodeString bdiClose("</bdi>");

    // 2iv. If dir is 'LTR'
    ਚੋਣ (dir) {
        ਹਾਲਤ U_MF_DIRECTIONALITY_LTR:
            ਜੇ (msgdir == U_MF_DIRECTIONALITY_LTR && !isolate) {
                // 2iv(a). If msgdir is 'LTR' in the formatted output, let fmt be itself
                ਵਾਪਸ &fmt;
            }
            // 2iii(b) Else, in the formatted output, prefix fmt with U+2066 LEFT-TO-RIGHT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            ਜੇ (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, LRI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } ਨਹੀਂਤਾਂ {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            ਰੋਕੋ; // End of 2iii
        // 2iv. Else, if dir is 'RTL':
        ਹਾਲਤ U_MF_DIRECTIONALITY_RTL:
            // 2iv(a). In the formatted output, prefix fmt with U+2067 RIGHT-TO-LEFT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            ਜੇ (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, RLI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } ਨਹੀਂਤਾਂ {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            ਰੋਕੋ; // End of 2iv.
        // 2v. Else:
        ਮੂਲ:
            // 2v(a). In the formatted output, prefix fmt with U+2068 FIRST STRONG ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            ਜੇ (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, FSI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } ਨਹੀਂਤਾਂ {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            ਰੋਕੋ; // End of 2v
    } // `fmt` now contains the isolated string
    ਵਾਪਸ &fmt;
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ // #if SHOW_CPLUSPLUS_API

